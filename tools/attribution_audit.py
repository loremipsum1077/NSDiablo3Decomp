#!/usr/bin/env python3
"""Reconcile every committed unowned function into explicit component evidence.

This read-only audit never assigns an original source owner, changes quality, or
approves an exclusion. Regex rules establish symbol-family evidence only.
"""
import argparse
import ast
from collections import Counter, defaultdict
import csv
import hashlib
import io
import json
from pathlib import Path
import re
import subprocess

ROOT = Path(__file__).resolve().parents[1]
RULES_PATH = Path('config/component_rules.json')
POLICY = ('Every unowned inventory address is retained in one fixed 64 KiB BIN and one '
          'component evidence record. Symbol families and optional binary evidence do not '
          'prove original source ownership. Only Quality=O is exact; all other qualities '
          'remain open. No exclusions or inventory changes are approved by this report.')


def address(value):
    """Parse full stable addresses without truncating their high bits."""
    try:
        result = int(value, 16) if isinstance(value, str) else int(value)
    except (ValueError, TypeError) as exc:
        raise ValueError(f'Invalid address: {value!r}') from exc
    if result < 0 or result >= 1 << 64:
        raise ValueError(f'Address outside uint64: {value!r}')
    return result


def metrics(rows):
    counts = Counter(row['Quality'] for row in rows)
    sizes = Counter()
    for row in rows:
        sizes[row['Quality']] += int(row['Size'])
    return dict(functions=len(rows), bytes=sum(sizes.values()), exact=counts['O'],
                exact_bytes=sizes['O'], quality=dict(sorted(counts.items())),
                quality_bytes=dict(sorted(sizes.items())))


def validate_rules(document):
    if not isinstance(document, dict) or document.get('schema_version') != 1:
        raise ValueError('Rules require schema_version=1')
    if not isinstance(document.get('rules'), list):
        raise ValueError('Rules must be a list')
    result, ids = [], set()
    for rule in document['rules']:
        if not isinstance(rule, dict):
            raise ValueError('Each rule must be an object')
        for key in ('id', 'component', 'confidence', 'evidence', 'follow_up'):
            if not isinstance(rule.get(key), str) or not rule[key].strip():
                raise ValueError(f'Rule requires nonempty {key}')
        if rule['id'] in ids:
            raise ValueError(f"Duplicate rule ID: {rule['id']}")
        ids.add(rule['id'])
        if rule['component'] == 'unresolved':
            raise ValueError('unresolved is reserved for unmatched functions')
        patterns = rule.get('name_patterns')
        if not isinstance(patterns, list) or not patterns or any(
                not isinstance(pattern, str) or not pattern for pattern in patterns):
            raise ValueError(f"Rule {rule['id']} requires nonempty name_patterns")
        try:
            compiled = [re.compile(pattern) for pattern in patterns]
        except re.error as exc:
            raise ValueError(f"Invalid regex in {rule['id']}: {exc}") from exc
        result.append((rule, compiled))
    return sorted(result, key=lambda item: item[0]['id'])


def reconcile(functions, owners, rules):
    """Pure reconciliation API; malformed/duplicate/conflicting input fails closed."""
    inventory = {}
    for row in functions:
        if not all(key in row for key in ('Address', 'Name', 'Quality', 'Size')):
            raise ValueError('Function row missing required columns')
        addr = address(row['Address'])
        if addr in inventory:
            raise ValueError(f'Duplicate function address: {addr:#x}')
        if row['Quality'] not in ('O', 'M', 'm', 'W', 'U', 'L'):
            raise ValueError(f'Unknown quality at {addr:#x}: {row["Quality"]!r}')
        try:
            size = int(row['Size'])
        except (TypeError, ValueError) as exc:
            raise ValueError(f'Invalid size at {addr:#x}') from exc
        if size < 0 or addr + size > 1 << 64:
            raise ValueError(f'Invalid size at {addr:#x}')
        if not isinstance(row['Name'], str) or not row['Name']:
            raise ValueError(f'Missing symbol name at {addr:#x}')
        inventory[addr] = row
    owner_of = {}
    for row in owners:
        if not all(key in row for key in ('Address', 'File')):
            raise ValueError('Owner row missing required columns')
        addr = address(row['Address'])
        if addr in owner_of:
            raise ValueError(f'Duplicate owner address: {addr:#x}')
        if addr not in inventory:
            raise ValueError(f'Owner address absent from inventory: {addr:#x}')
        if not isinstance(row['File'], str) or (row['File'] and not row['File'].strip()):
            raise ValueError(f'Invalid owner at {addr:#x}')
        owner_of[addr] = row['File']
    compiled = validate_rules(rules)
    records, pending = [], []
    components, bins, follow_ups = defaultdict(list), defaultdict(list), defaultdict(list)
    for addr, row in sorted(inventory.items()):
        if owner_of.get(addr):
            continue
        matched = [rule for rule, patterns in compiled
                   if any(pattern.search(row['Name']) for pattern in patterns)]
        if len(matched) > 1:
            raise ValueError(f'Ambiguous component rules at {addr:#x}: ' +
                             ', '.join(rule['id'] for rule in matched))
        bin_id = f'BIN-{addr // 0x10000:x}'
        rule = matched[0] if matched else None
        component = rule['component'] if rule else 'unresolved'
        follow_up = rule['follow_up'] if rule else bin_id
        record = dict(address=f'0x{addr:016x}', name=row['Name'], quality=row['Quality'],
                      bytes=int(row['Size']), bin_id=bin_id, component=component,
                      rule_id=rule['id'] if rule else None,
                      confidence=rule['confidence'] if rule else 'unresolved',
                      evidence=rule['evidence'] if rule else
                      'No reviewed component rule matches; original source ownership remains unknown.',
                      source_owner=None, ownership_status='unresolved', excluded=False,
                      follow_up=follow_up)
        records.append(record)
        pending.append(row)
        components[component].append(row)
        bins[bin_id].append(row)
        follow_ups[follow_up].append(row)
    summary = metrics(pending)
    summary.update(component_classified_functions=sum(r['component'] != 'unresolved' for r in records),
                   component_classified_bytes=sum(r['bytes'] for r in records if r['component'] != 'unresolved'),
                   unresolved_component_functions=sum(r['component'] == 'unresolved' for r in records),
                   unresolved_component_bytes=sum(r['bytes'] for r in records if r['component'] == 'unresolved'),
                   unresolved_source_owners=len(records), excluded_functions=0, excluded_bytes=0,
                   bins=len(bins), components=len(components))
    return dict(schema_version=1, policy=POLICY, inventory=metrics(functions), summary=summary,
                components=[dict(component=name, **metrics(rows)) for name, rows in sorted(components.items())],
                bins=[dict(id=name, **metrics(rows)) for name, rows in sorted(bins.items())],
                follow_ups=[dict(id=name, status='open', **metrics(rows))
                            for name, rows in sorted(follow_ups.items())], records=records)


def corroborate_binary(records, binary):
    """Reject extent/name contradictions; retain placeholder aliases as evidence."""
    by_address = {address(row['address']): row for row in records}
    evidence = {row['address']: row for row in binary['functions']}
    if len(evidence) != len(binary['functions']) or evidence.keys() != by_address.keys():
        raise ValueError('Binary evidence does not cover exactly the requested addresses')
    summaries = []
    for image in binary['images']:
        label = image['image']
        counts = Counter()
        for addr, row in by_address.items():
            observed = evidence[addr]
            if not any(s['image'] == label and s['contains_extent'] for s in observed['sections']):
                raise ValueError(f'{label} does not contain inventory extent at {addr:#x}')
            counts['extent_covered'] += 1
            symbols = [s for s in observed['symbols'] if s['image'] == label and s['type'] == 2]
            if not symbols:
                counts['without_function_symbol'] += 1
                continue
            counts['with_function_symbol'] += 1
            if any(s['name'] == row['name'] and s['size'] == row['bytes'] for s in symbols):
                counts['exact_name_and_size'] += 1
            elif row['name'].startswith('FUN_') and any(s['size'] == row['bytes'] for s in symbols):
                counts['placeholder_with_size_match'] += 1
            else:
                raise ValueError(f'{label} function symbols contradict inventory at {addr:#x}')
        summaries.append(dict(image=label, **dict(sorted(counts.items()))))
    return dict(images=summaries, policy=(
        'Full extents must be covered in every image. Present function symbols must '
        'corroborate name and size, except inventory FUN_ placeholders may corroborate size. '
        'Missing symbols remain explicitly uncorroborated. Extent coverage alone does not '
        'authenticate a stripped binary; hashes pin supplied inputs, not their provenance.'))


def snapshot(root=ROOT, rules_path=None, elf=None, nss=None, revision='HEAD'):
    root = Path(root)
    def git(*args):
        return subprocess.check_output(['git', *args], cwd=root)
    revision = git('rev-parse', '--verify', revision + '^{commit}').decode().strip()
    inputs = {}
    tables = []
    for name in ('functions', 'function_files'):
        path = f'data/2.6.2/{name}.csv'
        raw = git('show', f'{revision}:{path}')
        inputs[path] = dict(sha256=hashlib.sha256(raw).hexdigest(), bytes=len(raw),
                           git_blob=git('rev-parse', f'{revision}:{path}').decode().strip())
        tables.append(list(csv.DictReader(io.StringIO(raw.decode('utf-8')))))
    rules_path = Path(rules_path) if rules_path else root / RULES_PATH
    raw = rules_path.read_bytes()
    result = reconcile(*tables, json.loads(raw))
    try:
        shown_path = str(rules_path.relative_to(root))
    except ValueError:
        shown_path = str(rules_path)
    result['provenance'] = dict(revision=revision, inventory_inputs=inputs,
                                rules=dict(path=shown_path, sha256=hashlib.sha256(raw).hexdigest(),
                                           bytes=len(raw), source='working-tree file, hash pinned'),
                                tool_sha256={Path(__file__).name: hashlib.sha256(Path(__file__).read_bytes()).hexdigest()})
    if elf is not None or nss is not None:
        import attribution_binary
        sizes = {address(row['address']): row['bytes'] for row in result['records']}
        result['binary_evidence'] = attribution_binary.collect_binary_evidence(
            elf, sizes, nss_path=nss)
        if elf is not None:
            # Read the repository's committed setup authority without executing it.
            setup_raw = git('show', f'{revision}:tools/setup.py')
            expected = [node.value.value for node in ast.parse(setup_raw).body
                        if isinstance(node, ast.Assign) and isinstance(node.value, ast.Constant)
                        and any(isinstance(target, ast.Name) and target.id == 'ELF_SHA256'
                                for target in node.targets)]
            if len(expected) != 1 or not isinstance(expected[0], str) or not re.fullmatch('[0-9a-f]{64}', expected[0]):
                raise ValueError('Committed tools/setup.py has no unique valid ELF_SHA256 authority')
            observed = next(image['sha256'] for image in result['binary_evidence']['images']
                            if image['image'] == 'elf')
            if observed != expected[0]:
                raise ValueError('ELF SHA256 differs from committed tools/setup.py target 2.6.2')
            result['binary_evidence']['identity'] = dict(
                elf='verified repository target 2.6.2 SHA256', expected_elf_sha256=expected[0],
                authority='tools/setup.py', authority_revision=revision,
                authority_sha256=hashlib.sha256(setup_raw).hexdigest())
        tool = Path(attribution_binary.__file__)
        result['provenance']['tool_sha256'][tool.name] = hashlib.sha256(tool.read_bytes()).hexdigest()
        result['binary_evidence']['corroboration'] = corroborate_binary(result['records'], result['binary_evidence'])
    return result


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--output', '--out', type=Path, help='Write JSON here; default stdout')
    parser.add_argument('--rules', type=Path, help='Reviewed component rules JSON')
    parser.add_argument('--revision', default='HEAD', help='Committed inventory revision (default HEAD)')
    parser.add_argument('--elf', type=Path, help='Optional original ELF for binary evidence')
    parser.add_argument('--nss', type=Path, help='Optional original NSS for binary evidence')
    args = parser.parse_args()
    try:
        result = snapshot(rules_path=args.rules, elf=args.elf, nss=args.nss, revision=args.revision)
    except (ValueError, OSError, subprocess.CalledProcessError) as exc:
        parser.exit(1, f'attribution audit failed: {exc}\n')
    text = json.dumps(result, indent=2, sort_keys=True) + '\n'
    if args.output:
        args.output.parent.mkdir(parents=True, exist_ok=True)
        args.output.write_text(text, encoding='utf-8')
    else:
        print(text, end='')


if __name__ == '__main__':
    main()
