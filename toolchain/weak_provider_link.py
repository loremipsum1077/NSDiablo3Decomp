#!/usr/bin/env python3
"""Preserve reviewed COMDAT providers while invoking the normal link command.

The manifest records observed stock contributors, not the original linker order
or proof of semantic equivalence. Only the project's current object-only link
recipe is supported. Unknown duplicated groups and ambiguous maps fail closed.
"""
import argparse
import collections
import hashlib
import heapq
import json
import os
from pathlib import Path
import re
import stat
import struct
import subprocess
import sys
import tempfile

MAX_OBJECT = 128 * 1024 * 1024
MAX_MAP = 128 * 1024 * 1024
MAX_INPUTS = 20000


class ProviderError(ValueError):
    pass


def require(condition, message):
    if not condition:
        raise ProviderError(message)


def digest(data):
    return hashlib.sha256(data).hexdigest()


def unique_json(path):
    def pairs(items):
        result = {}
        for key, value in items:
            require(key not in result, 'duplicate manifest key: ' + key)
            result[key] = value
        return result
    return json.loads(bounded_file(path, 2 * 1024 * 1024), object_pairs_hook=pairs)


def bounded_file(path, limit):
    path = Path(path).absolute()
    for parent in path.parents:
        mode = parent.lstat().st_mode
        require(stat.S_ISDIR(mode) and not stat.S_ISLNK(mode), 'symlink or invalid parent: ' + str(parent))
    fd = os.open(str(path), os.O_RDONLY | os.O_NOFOLLOW | os.O_NONBLOCK)
    with os.fdopen(fd, 'rb') as stream:
        before = os.fstat(stream.fileno())
        require(stat.S_ISREG(before.st_mode), 'regular file required: ' + str(path))
        require(before.st_size <= limit, 'file too large: ' + str(path))
        data = stream.read(limit + 1)
        after = os.fstat(stream.fileno())
    current = path.lstat()
    require(stat.S_ISREG(current.st_mode), 'regular file changed: ' + str(path))
    require(len(data) == before.st_size and len(data) <= limit, 'file changed: ' + str(path))
    fields = lambda s: (s.st_dev, s.st_ino, s.st_size, s.st_mtime_ns, s.st_ctime_ns)
    require(fields(before) == fields(after) == fields(current), 'file changed: ' + str(path))
    return data


class ObjectGroups(dict):
    def __init__(self):
        super().__init__()
        self.definitions = {}
        self.has_initializers = False


def parse_object(data):
    """Return all COMDAT signatures, members and same-address named aliases."""
    require(len(data) >= 64 and data[:7] == b'\x7fELF\x02\x01\x01', 'ELF64 little-endian required')
    etype, machine, version = struct.unpack_from('<HHI', data, 16)
    require((etype, machine, version) == (1, 183, 1), 'AArch64 ET_REL required')
    shoff = struct.unpack_from('<Q', data, 0x28)[0]
    shsize, count, shstr = struct.unpack_from('<HHH', data, 0x3a)
    require(shsize == 64 and count > 0 and 0 < shstr < count and
            shoff >= 64 and shoff + count * shsize <= len(data), 'invalid section table')
    sections = [struct.unpack_from('<IIQQQQIIQQ', data, shoff + i * shsize) for i in range(count)]
    for section in sections:
        if section[1] != 8:
            require(section[4] <= len(data) and section[5] <= len(data) - section[4], 'section outside file')
    require(sections[shstr][1] == 3, 'invalid section name table')

    def string(table, offset):
        require(offset < table[5], 'string outside table')
        start = table[4] + offset
        end = data.find(b'\0', start, table[4] + table[5])
        require(end >= 0, 'unterminated string')
        try:
            return data[start:end].decode('utf-8')
        except UnicodeDecodeError as error:
            raise ProviderError('non-UTF8 ELF name') from error

    names = [string(sections[shstr], section[0]) for section in sections]
    symtabs = [i for i, section in enumerate(sections) if section[1] == 2]
    require(len(symtabs) == 1, 'exactly one object symbol table required')
    symidx = symtabs[0]
    table = sections[symidx]
    require(table[9] == 24 and table[5] % 24 == 0 and table[6] < count and
            sections[table[6]][1] == 3, 'invalid symbol table')
    symbols = []
    for offset in range(table[4], table[4] + table[5], 24):
        name, info, other, section, value, size = struct.unpack_from('<IBBHQQ', data, offset)
        symbols.append({'name': string(sections[table[6]], name), 'type': info & 15,
                        'binding': info >> 4, 'other': other, 'section': section,
                        'value': value, 'size': size})
    groups = ObjectGroups()
    initializer_names = ('.init', '.fini', '.preinit_array', '.init_array', '.fini_array', '.ctors', '.dtors')
    groups.has_initializers = any(name == base or name.startswith(base + '.')
                                  for name in names for base in initializer_names)
    for symbol in symbols:
        if symbol['name'] and symbol['binding'] != 0 and symbol['section'] != 0:
            groups.definitions.setdefault(symbol['name'], []).append(symbol)
    assigned = set()
    for section in sections:
        if section[1] != 17:
            continue
        require(section[6] == symidx and section[5] >= 8 and section[5] % 4 == 0 and
                section[7] < len(symbols), 'invalid COMDAT group')
        words = struct.unpack_from('<' + 'I' * (section[5] // 4), data, section[4])
        require(words[0] == 1, 'unsupported group flags')
        indexes = words[1:]
        require(len(set(indexes)) == len(indexes) and all(0 < i < count for i in indexes),
                'duplicate or invalid group member')
        require(not assigned.intersection(indexes), 'member belongs to multiple groups')
        assigned.update(indexes)
        signature = symbols[section[7]]
        name = signature['name']
        require(name and name not in groups, 'empty or repeated group signature')
        defined_signature = (signature['section'] in indexes and signature['binding'] in (1, 2) and
                             signature['type'] in (1, 2) and signature['other'] == 0)
        local_group_marker = (signature['binding'] == 0 and signature['type'] == 0 and
                              signature['other'] == 0 and signature['value'] == 0 and
                              signature['size'] == 0 and 0 < signature['section'] < count)
        require(defined_signature or local_group_marker, 'unsupported group signature: ' + name)
        if defined_signature:
            extent = sections[signature['section']][5]
            require(signature['value'] <= extent and signature['size'] <= extent - signature['value'],
                    'group signature outside section')
        member_names = [names[i] for i in indexes]
        require(all(member_names) and len(set(member_names)) == len(member_names),
                'ambiguous group member names')
        require(all(sections[i][2] & 0x200 for i in indexes), 'member missing SHF_GROUP')
        require(all(names.count(member) == 1 for member in member_names),
                'group member name repeated in object')
        aliases = sorted({symbol['name'] for symbol in symbols
                          if symbol['name'] and symbol['name'] != name and
                          symbol['type'] in (1, 2) and
                          symbol['section'] == signature['section'] and
                          symbol['value'] == signature['value']}) if defined_signature else []
        extra_exports = sorted({symbol['name'] for symbol in symbols
                                if symbol['name'] and symbol['name'] != name and
                                symbol['binding'] != 0 and symbol['section'] in indexes})
        groups[name] = {'members': member_names, 'aliases': aliases,
                        'signature_type': signature['type'], 'binding': signature['binding'],
                        'signature_section': signature['section'], 'signature_value': signature['value'],
                        'extra_exports': extra_exports}
    return groups


def valid_object_name(name):
    return (isinstance(name, str) and name.startswith('CMakeFiles/') and name.endswith('.obj') and
            all(x not in ('', '.', '..') for x in name.split('/')) and
            not any(x in name for x in ('\\', ':', '\0')))


def validate_manifest(manifest):
    keys = {'format', 'description', 'reference', 'providers'}
    require(isinstance(manifest, dict) and keys <= set(manifest) <= keys | {'initializer_order'} and
            manifest['format'] == 1 and isinstance(manifest['providers'], dict), 'invalid manifest')
    for signature, provider in manifest['providers'].items():
        require(isinstance(signature, str) and signature and valid_object_name(provider), 'invalid manifest provider')
    return manifest['providers']


def validate_initializer_order(manifest):
    """Objects in the original's .init_array order (where a source file is known)."""
    names = manifest.get('initializer_order', [])
    require(isinstance(names, list) and all(valid_object_name(name) for name in names) and
            len(set(names)) == len(names), 'invalid manifest initializer order')
    return names


def plan(order, objects, providers, initializer_order=()):
    """Stable topological order with fail-closed duplicate and alias handling."""
    require(len(order) == len(set(order)) == len(objects), 'duplicate input objects')
    require(set(order) == set(objects), 'object identity mismatch')
    appearances = {}
    for obj in order:
        for signature, group in objects[obj].items():
            appearances.setdefault(signature, []).append((obj, group))
    edges = set()
    provider_edges = set()
    provider_groups = []
    duplicates = {}
    for signature, candidates in appearances.items():
        if len(candidates) < 2 and signature not in providers:
            continue
        if signature not in providers:
            # A new file emitted its own copy of a shared inline. Keep the linker's
            # natural choice (first in input order). If that displaces a copy another
            # file's match depends on, landing reports the regression: register the
            # right provider in weak-providers.json then.
            duplicates[signature] = {'winner': candidates[0][0], 'candidates': [obj for obj, _ in candidates]}
            continue
        winner = providers[signature]
        require(sum(obj == winner for obj, _ in candidates) == 1, 'designated provider missing or ambiguous: ' + signature)
        require(all(not group['aliases'] for _, group in candidates), 'ambiguous signature alias: ' + signature)
        require(all(not group.get('extra_exports', []) for _, group in candidates),
                'unsupported additional group exports: ' + signature)
        require(all(group['binding'] == 2 for _, group in candidates), 'non-weak duplicate signature: ' + signature)
        for obj in order:
            definitions = getattr(objects[obj], 'definitions', {}).get(signature, [])
            if not definitions:
                continue
            candidate = objects[obj].get(signature)
            require(candidate is not None and len(definitions) == 1 and
                    definitions[0]['binding'] == 2 and
                    definitions[0]['type'] == candidate['signature_type'] and
                    definitions[0]['section'] == candidate['signature_section'] and
                    definitions[0]['value'] == candidate['signature_value'] and
                    definitions[0]['other'] == 0, 'conflicting definition of duplicate: ' + signature)
        duplicates[signature] = {'winner': winner, 'candidates': [obj for obj, _ in candidates]}
        provider_groups.append((signature, {(winner, obj) for obj, _ in candidates if obj != winner}))
        provider_edges.update(provider_groups[-1][1])
    for signature in providers:
        require(signature in duplicates, 'designated provider missing: ' + signature)
    # A COMDAT constraint must not silently reverse unrelated non-group weak
    # definitions. Preserve their complete incoming object order as well.
    named_definitions = {}
    for obj in order:
        for name, symbols in getattr(objects[obj], 'definitions', {}).items():
            named_definitions.setdefault(name, []).append((obj, symbols))
    for signature, candidates in appearances.items():
        if len(candidates) != 1 or signature in providers:
            continue
        obj, group = candidates[0]
        definitions = named_definitions.get(signature, [])
        require(not definitions or (len(definitions) == 1 and definitions[0][0] == obj and
                                   len(definitions[0][1]) == 1 and
                                   definitions[0][1][0]['section'] == group['signature_section'] and
                                   definitions[0][1][0]['value'] == group['signature_value']),
                'conflicting definition of unique COMDAT: ' + signature)
        for exported in group.get('extra_exports', []):
            definitions = named_definitions.get(exported, [])
            require(len(definitions) == 1 and definitions[0][0] == obj and len(definitions[0][1]) == 1,
                    'ambiguous additional group export: ' + exported)
    for name, candidates in named_definitions.items():
        if len(candidates) < 2 or all(name in objects[obj] for obj, _ in candidates):
            continue
        require(all(len(symbols) == 1 for _, symbols in candidates),
                'ambiguous duplicate symbol definitions: ' + name)
        for (before, _), (after, _) in zip(candidates, candidates[1:]):
            edges.add((before, after))
    definition_edges = set(edges)
    initializers = [obj for obj in order if getattr(objects[obj], 'has_initializers', False)]
    edges.update(zip(initializers, initializers[1:]))
    index = {obj: i for i, obj in enumerate(order)}

    def sort(constraints):
        successors = {obj: set() for obj in order}
        indegree = {obj: 0 for obj in order}
        for before, after in constraints:
            if after not in successors[before]:
                successors[before].add(after)
                indegree[after] += 1
        ready = [index[obj] for obj in order if not indegree[obj]]
        heapq.heapify(ready)
        ordered = []
        while ready:
            current = order[heapq.heappop(ready)]
            ordered.append(current)
            for child in successors[current]:
                indegree[child] -= 1
                if indegree[child] == 0:
                    heapq.heappush(ready, index[child])
        return ordered

    ordered = sort(edges | provider_edges)
    if len(ordered) == len(order):
        return ordered, duplicates, sorted(edges | provider_edges)
    # The registered providers only record which copy an earlier build happened to
    # keep. When they contradict definition or initialiser order (a newly written
    # file gained a static initialiser), that order wins; landing still rejects the
    # link if a matched function changes.
    require(len(sort(edges)) == len(order), 'object ordering cycle')
    kept = set(edges)
    honoured = set(definition_edges)
    dropped = []
    for signature, group in provider_groups:  # keep every registered provider that still fits
        if len(sort(kept | group)) == len(order):
            kept |= group
            honoured |= group
        else:
            dropped.append((signature, group))
    # Second chance: the incoming initialiser order is only CMake's, and the original's
    # .init_array order is known (manifest "initializer_order"). A dropped provider whose
    # winner the original initialised before a loser may move one of those two objects
    # within the initialiser chain, provided that every provider kept above still holds,
    # the chain gets no further from the original's order (pairwise inversions), and any
    # other COMDAT copy that changes moves to its registered provider or to an object
    # the original initialised (so linked) earlier than the previous one.
    rank = {obj: i for i, obj in enumerate(initializer_order)}
    chain = list(initializers)
    kept = honoured

    def inversions(sequence):
        ranks = [rank[obj] for obj in sequence if obj in rank]
        return sum(a > b for i, a in enumerate(ranks) for b in ranks[i + 1:])

    def winners(ordered):
        position = {obj: i for i, obj in enumerate(ordered)}
        return {name: min((obj for obj, _ in candidates), key=position.get)
                for name, candidates in appearances.items() if len(candidates) > 1}

    ignored = []
    for signature, group in dropped:
        before = winners(sort(kept | set(zip(chain, chain[1:]))))
        best = None
        for winner, loser in sorted(group):
            if not (winner in rank and loser in rank and rank[winner] < rank[loser] and
                    winner in chain and loser in chain and chain.index(loser) < chain.index(winner)):
                continue
            rest = [obj for obj in chain if obj != winner]
            early = rest[:rest.index(loser)] + [winner] + rest[rest.index(loser):]
            rest = [obj for obj in chain if obj != loser]
            late = rest[:rest.index(winner) + 1] + [loser] + rest[rest.index(winner) + 1:]
            for candidate in (early, late):
                score = inversions(candidate)
                if score > inversions(chain) or (best is not None and score >= best[0]):
                    continue
                ordered = sort(kept | group | set(zip(candidate, candidate[1:])))
                if len(ordered) != len(order):
                    continue
                after = winners(ordered)
                if all(after[name] == before[name] or after[name] == providers.get(name) or
                       rank.get(after[name], len(rank)) < rank.get(before[name], -1) for name in before):
                    best = (score, candidate)
        if best is None:
            ignored.append(signature)
            continue
        chain = best[1]
        kept |= group
        sys.stderr.write(f'weak-provider link: {signature} honoured by original initialiser order\n')
    kept |= set(zip(chain, chain[1:]))
    for signature in ignored:
        sys.stderr.write(f'weak-provider link: {signature} conflicts with input order; ignored\n')
    sys.stderr.write(f'weak-provider link: {len(ignored)} registered providers conflict with input order; ignoring those\n')
    return sort(kept), duplicates, sorted(kept)


def parse_map(data):
    try:
        lines = data.decode('utf-8').splitlines()
    except UnicodeDecodeError as error:
        raise ProviderError('invalid map encoding') from error
    require(lines and lines[0].split() == ['Address', 'Size', 'Align', 'Out', 'In', 'Symbol'],
            'unsupported map header')
    result = collections.Counter()
    for line in lines[1:]:
        if ':(' not in line:
            continue
        fields = line.split()
        require(len(fields) == 4 and re.fullmatch('[0-9a-fA-F]+', fields[0]) and
                re.fullmatch('[0-9a-fA-F]+', fields[1]) and fields[2].isdigit(), 'invalid map contribution')
        match = re.fullmatch(r'([^():\s]+):\(([^()\s]+)\)', fields[3])
        require(match is not None, 'invalid map contribution identity')
        result[(match[1], match[2])] += 1
    return result


def verify_map(contributions, duplicates, objects, absolute):
    evidence = {}
    for signature, row in duplicates.items():
        selected = []
        for obj in row['candidates']:
            members = objects[obj][signature]['members']
            matches = [(absolute[obj], member) for member in members if (absolute[obj], member) in contributions]
            if isinstance(contributions, dict):
                require(all(contributions[key] == 1 for key in matches), 'ambiguous repeated group contribution: ' + signature)
            if matches:
                selected.append(obj)
        # Record the copy the linker actually kept. It can differ from the planned
        # winner when the registry was overridden; landing judges the result.
        require(len(selected) <= 1, 'several mapped COMDAT providers: ' + signature)
        evidence[signature] = selected[0] if selected else row['winner']
    return evidence


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--manifest', type=Path, required=True)
    parser.add_argument('--build-root', type=Path, required=True)
    parser.add_argument('--evidence', type=Path, required=True)
    parser.add_argument('command', nargs=argparse.REMAINDER)
    args = parser.parse_args()
    command = args.command[1:] if args.command and args.command[0] == '--' else args.command
    root = args.build_root.resolve()
    require(root.is_dir() and len(command) >= 8, 'invalid link command')
    require(Path(command[0]).name == 'ld.lld' and command[1:4] == ['-shared', '-Bsymbolic', '--no-demangle'] and
            command[4].startswith('-Map=') and command[-2] == '-o', 'unsupported linker recipe')
    map_path = Path(command[4][5:])
    require(map_path == root / 'd3.map' and Path(command[-1]) == root / 'd3', 'unexpected linker outputs')
    inputs = command[5:-2]
    require(0 < len(inputs) <= MAX_INPUTS, 'invalid object count')
    absolute = {}
    objects = {}
    fingerprints = {}
    for entry in inputs:
        path = Path(entry)
        require(path.is_absolute() and path.suffix == '.obj', 'unsupported linker input: ' + entry)
        try:
            relative = path.relative_to(root).as_posix()
        except ValueError as error:
            raise ProviderError('object outside build root: ' + entry) from error
        require(relative.startswith('CMakeFiles/') and relative not in objects and
                all(part not in ('', '.', '..') for part in relative.split('/')) and
                path == root / relative, 'ambiguous object input')
        data = bounded_file(path, MAX_OBJECT)
        objects[relative] = parse_object(data)
        fingerprints[relative] = digest(data)
        absolute[relative] = entry
    manifest_bytes = bounded_file(args.manifest, 2 * 1024 * 1024)
    manifest = unique_json(args.manifest)
    providers = validate_manifest(manifest)
    initializer_order = validate_initializer_order(manifest)
    require(digest(manifest_bytes) == digest(bounded_file(args.manifest, 2 * 1024 * 1024)), 'manifest changed')
    order, duplicates, edges = plan(list(objects), objects, providers, initializer_order)
    new_command = command[:5] + [absolute[obj] for obj in order] + command[-2:]
    subprocess.run(new_command, check=True)
    for obj in order:
        require(digest(bounded_file(Path(absolute[obj]), MAX_OBJECT)) == fingerprints[obj], 'object changed during link')
    require(digest(bounded_file(args.manifest, 2 * 1024 * 1024)) == digest(manifest_bytes), 'manifest changed during link')
    map_bytes = bounded_file(map_path, MAX_MAP)
    mapped = verify_map(parse_map(map_bytes), duplicates, objects, absolute)
    evidence = {'format': 1, 'manifest_sha256': digest(manifest_bytes), 'object_sha256': fingerprints,
                'helper_sha256': digest(bounded_file(Path(__file__).resolve(), 1024 * 1024)),
                'linker_sha256': digest(bounded_file(Path(command[0]).resolve(), MAX_OBJECT)),
                'link_argv': new_command, 'input_order': list(objects), 'output_order': order, 'constraints': edges,
                'mapped_providers': mapped, 'map_sha256': digest(map_bytes),
                'elf_sha256': digest(bounded_file(root / 'd3', 256 * 1024 * 1024)),
                'scope': 'Stock contributor preservation only; original order and runtime equivalence unproved.'}
    require(args.evidence.parent == root and not args.evidence.exists(), 'fresh evidence path required')
    with tempfile.NamedTemporaryFile(mode='w', dir=str(root), delete=False) as stream:
        json.dump(evidence, stream, sort_keys=True, indent=2)
        stream.write('\n')
        stream.flush()
        os.fsync(stream.fileno())
        temporary = stream.name
    os.replace(temporary, args.evidence)


if __name__ == '__main__':
    try:
        main()
    except (ProviderError, OSError, struct.error, subprocess.CalledProcessError) as error:
        print('weak-provider link refused: ' + str(error), file=sys.stderr)
        sys.exit(1)
