"""Bounded, read-only adapter for existing owner/object/integration validators.

Evidence hashes authenticate consistency only. Supplied historical receipts never
prove a fresh build or replace independent source review. Raw validator outputs
stay private; this module returns fixed status/count/hash fields only.
"""
from collections import Counter
import hashlib
import importlib.util
import json
from pathlib import Path, PurePosixPath
import re
import shutil
import struct
import tempfile

FIELDS = ('format', 'elf_sha256', 'map_sha256', 'record_sha256')
HASH = re.compile(r'[0-9a-f]{64}\Z')


def digest(path):
    h = hashlib.sha256()
    with Path(path).open('rb') as stream:
        for chunk in iter(lambda: stream.read(1024 * 1024), b''):
            h.update(chunk)
    return h.hexdigest()


def unique(pairs):
    out = {}
    for key, value in pairs:
        if key in out:
            raise ValueError('duplicate_evidence_key')
        out[key] = value
    return out


def read(path, expected=None):
    if Path(path).is_symlink() or not Path(path).is_file() or Path(path).stat().st_size > 8 * 1024 * 1024:
        raise ValueError('invalid_evidence_file')
    content = Path(path).read_bytes()
    if expected is not None and hashlib.sha256(content).hexdigest() != expected:
        raise ValueError('evidence_bytes_hash_mismatch')
    return json.loads(content, object_pairs_hook=unique)


def relative(value):
    if not isinstance(value, str) or '\\' in value or ':' in value or '\0' in value:
        raise ValueError('invalid_evidence_path')
    p = PurePosixPath(value)
    if p.is_absolute() or any(part in ('', '.', '..') for part in value.split('/')):
        raise ValueError('invalid_evidence_path')
    return p


def regular(root, value):
    p = Path(root)
    for part in relative(value).parts:
        p = p / part
        if p.is_symlink():
            raise ValueError('evidence_symlinks_not_supported')
    if not p.is_file():
        raise ValueError('evidence_file_missing')
    return p


def hash_map(value):
    if not isinstance(value, dict) or not value or len(value) > 20000:
        raise ValueError('invalid_evidence_file_map')
    for name, sha in value.items():
        relative(name)
        if not isinstance(sha, str) or not HASH.fullmatch(sha):
            raise ValueError('invalid_evidence_digest')
    return value


def validators():
    """Load fixed CPU modules, keeping per-report original bindings isolated."""
    import validation
    folder = Path(validation.__file__).parent
    result = {}
    for name in ('strict', 'unwind', 'integration'):
        spec = importlib.util.spec_from_file_location('validation._contributor_' + name, folder / (name + '.py'))
        module = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(module)
        result[name] = module
    return result


def load(path, expected, commits):
    path = Path(path).resolve()
    if not isinstance(expected, str) or not HASH.fullmatch(expected):
        raise ValueError('evidence_manifest_hash_mismatch')
    value = read(path, expected)
    if set(value) != {'format', 'baseline', 'candidate', 'toolchain'} or type(value['format']) is not int or value['format'] != 1:
        raise ValueError('unsupported_evidence_manifest')
    for version in ('baseline', 'candidate'):
        item = value[version]
        if set(item) != {'commit', 'source', 'normal', 'lua'} or item['commit'] != commits[version]:
            raise ValueError('evidence_commit_binding_mismatch')
        for mode in ('normal', 'lua'):
            entry = item[mode]
            if set(entry) != {'root', 'receipt_sha256'} or not HASH.fullmatch(entry['receipt_sha256']):
                raise ValueError('invalid_mode_evidence')
            entry['root'] = local(path.parent, entry['root'])
        source = item['source']
        if set(source) != {'root', 'manifest', 'sha256'} or not HASH.fullmatch(source['sha256']):
            raise ValueError('invalid_source_evidence')
        source['root'] = local(path.parent, source['root'])
        source['manifest'] = local(path.parent, source['manifest'])
    if not isinstance(value['toolchain'], dict) or set(value['toolchain']) - {'normal_compiler', 'lua_backend', 'linker'}:
        raise ValueError('invalid_toolchain_evidence')
    for entry in value['toolchain'].values():
        if set(entry) != {'path', 'sha256', 'version'} or not HASH.fullmatch(entry['sha256']):
            raise ValueError('invalid_toolchain_evidence')
        if not re.fullmatch(r'\d{1,3}(?:\.\d{1,3}){1,3}', entry['version']):
            raise ValueError('invalid_toolchain_version')
        entry['path'] = local(path.parent, entry['path'])
        if entry['path'].is_symlink() or digest(entry['path']) != entry['sha256']:
            raise ValueError('toolchain_hash_mismatch')
    value['_manifest_sha256'] = expected
    value['_path'] = path
    return value


def local(base, value):
    if not isinstance(value, str) or '\0' in value:
        raise ValueError('invalid_local_evidence_path')
    # Explicit local paths are accepted but never exported. Refuse symlink roots.
    p = Path(value) if Path(value).is_absolute() else base / value
    if p.is_symlink():
        raise ValueError('evidence_symlink_root')
    return p.resolve()


def source_check(entry, expected):
    value = read(entry['manifest'], entry['sha256'])
    files = hash_map(value['files'] if set(value) == {'files'} else value)
    metadata = {name for name in files if PurePosixPath(name).name == '.DS_Store'}
    if set(files) - metadata != set(expected):
        raise ValueError('source_commit_file_set_mismatch')
    for name, sha in files.items():
        if digest(regular(entry['root'], name)) != sha or (name not in metadata and expected[name] != sha):
            raise ValueError('source_commit_content_mismatch')
    paths = [p for p in entry['root'].rglob('*') if p.relative_to(entry['root']).parts[0] != 'build']
    if any(p.is_symlink() for p in paths):
        raise ValueError('source_snapshot_symlink')
    actual = {p.relative_to(entry['root']).as_posix() for p in paths if p.is_file()}
    if actual != set(files):
        raise ValueError('source_snapshot_extra_files')
    return {'status': 'pass', 'reason': 'source_snapshot_matches_commit_not_build_lineage',
            'manifest_sha256': entry['sha256'], 'files': len(expected), 'ignored_metadata_files': len(metadata)}


def mode_files(entry, owner, allow_absent=False):
    root = entry['root']
    proof = regular(root, 'done.json')
    receipt = read(proof, entry['receipt_sha256'])
    if set(receipt) != {'files', 'full_check_rc'} or type(receipt['full_check_rc']) is not int or receipt['full_check_rc'] != 0:
        raise ValueError('invalid_integration_receipt')
    files = hash_map(receipt['files'])
    for name, sha in files.items():
        if digest(regular(root, name)) != sha:
            raise ValueError('mode_receipt_file_mismatch')
    needed = ['build/2.6.2/' + name for name in ('d3', 'd3.map', 'd3.map.provenance')]
    objects = [name for name in files if name.startswith('objects/') and name.endswith('/src/' + owner + '.obj')]
    if (len(objects) != 1 and not (allow_absent and not objects)) or any(name not in files for name in needed):
        raise ValueError('unique_bound_owner_object_and_links_required')
    return files, objects[0] if objects else None


def object_status(row):
    if row.get('exact') is True:
        return 'ok'
    return {'missing': 'missing', 'unsupported_relocation': 'pending', 'size': 'mismatch',
            'instructions': 'mismatch'}.get(row.get('reason'), 'pending')


def executable_coverage(strict, path):
    """Account for executable bytes even when they have no function symbol."""
    elf = strict.Elf(path)
    if struct.unpack_from('<H', elf.d, 16)[0] != 1:
        raise ValueError('relocatable_owner_object_required')
    intervals, identities = {}, {}
    for symbol in strict.symbols(elf).values():
        if symbol['type'] != 2 or not symbol['section']:
            continue
        index = symbol['section']
        if index >= len(elf.sections):
            raise ValueError('invalid_function_section')
        section = elf.sections[index]
        start, size = symbol['value'], symbol['size']
        if not section[2] & 4 or start > section[5] or size > section[5] - start:
            raise ValueError('invalid_function_range')
        identity = (index, start, size)
        if symbol['name'] in identities and identities[symbol['name']] != identity:
            raise ValueError('ambiguous_object_function_identity')
        identities[symbol['name']] = identity
        if size:
            intervals.setdefault(index, set()).add((start, start + size))
    uncovered = 0
    for index, section in enumerate(elf.sections):
        if not section[2] & 4 or not section[5]:
            continue
        if not section[2] & 2 or section[1] == 8 or section[4] + section[5] > len(elf.d):
            raise ValueError('invalid_executable_section')
        end, covered = 0, 0
        for start, stop in sorted(intervals.get(index, ())):
            if start < end:
                raise ValueError('overlapping_function_ranges')
            covered += stop - start
            end = stop
        uncovered += section[5] - covered
    return uncovered


def exact_unwind_programs(strict, unwind, original, obj, links, target):
    """Require exact selected CIE and FDE programs, without stripping operands."""
    try:
        _, originals, original_cies = unwind.frame(strict.Elf(original), True)
        elf = strict.Elf(obj)
        index, frames, cies = unwind.frame(elf, False)
        symbols = strict.symbols(elf)
        symbol = unwind.one([s for s in symbols.values() if s['name'] == target['Name']
                             and s['section'] and s['type'] == 2])
        relocation = unwind.one([r for r in strict.relocs(elf, symbols) if r['section'] == index
            and r['type'] == 261 and r['symbol']['section'] == symbol['section']
            and r['symbol']['value'] + r['addend'] == symbol['value']])
        frame = unwind.one([f for f in frames if f['at'] + 8 == relocation['offset']])
        original_frame = unwind.one([f for f in originals if f['pc'] & 0xffffffff == int(target['Address'], 16) & 0xffffffff])
        # A trailing zero can be an operand, so do not strip apparent padding.
        # Different encodings/padding remain pending without a CFI decoder.
        body = original_cies[original_frame['cie']]
        if body != cies[frame['cie']] or original_frame['instr'] != frame['instr']:
            return False
        for path in links.values():
            linked_elf = strict.Elf(path)
            _, linked_frames, linked_cies = unwind.frame(linked_elf, True)
            linked_symbol = unwind.one([s for s in strict.symbols(linked_elf).values()
                if s['name'] == target['Name'] and s['section'] and s['type'] == 2])
            linked_frame = unwind.one([f for f in linked_frames if f['pc'] == linked_symbol['value']])
            if body != linked_cies[linked_frame['cie']] or original_frame['instr'] != linked_frame['instr']:
                return False
        return True
    except (KeyError, ValueError, IndexError, struct.error):
        return False


def owner_gates(evidence, owner, all_rows, targets, source_hashes, original, original_sha, linked_evidence, tables):
    mods = validators()
    strict, unwind = mods['strict'], mods['unwind']
    # Qualified identities need an authenticated object-owner mapping; never strip.
    qualified = any('@' in row['Name'] for row in all_rows)
    answer = {'object': {'status': 'partial', 'reason': 'qualified_object_identity_pending'} if qualified else {},
              'unwind': {}, 'emitted_extras': {}, 'source_snapshot': {}, 'bindings': {},
              'fresh_build_lineage': {'status': 'partial', 'reason': 'supplied_receipts_do_not_prove_trusted_fresh_build'}}
    snapshots, observed = {}, []
    with tempfile.TemporaryDirectory(prefix='d3-owner-gates-') as temp:
        temp = Path(temp)
        context = temp / 'context/data/2.6.2'
        context.mkdir(parents=True)
        shutil.copyfile(original, context / 'main.elf')
        if digest(context / 'main.elf') != original_sha:
            raise ValueError('owner_original_binding_mismatch')
        for name, content in tables.items():
            (context / name).write_bytes(content)
        strict.configure(temp / 'context')
        for version in ('baseline', 'candidate'):
            answer['source_snapshot'][version] = source_check(evidence[version]['source'], source_hashes[version])
            observed.append((evidence[version]['source']['manifest'], evidence[version]['source']['sha256']))
            for mode in ('normal', 'lua'):
                entry = evidence[version][mode]
                absent_baseline = (version == 'baseline' and 'src/' + owner not in source_hashes[version]
                                   and all(row['Quality'] == 'U' for row in all_rows))
                files, obj = mode_files(entry, owner, allow_absent=absent_baseline)
                observed.append((regular(entry['root'], 'done.json'), entry['receipt_sha256']))
                dest = temp / version / mode
                dest.mkdir(parents=True)
                names = {'d3': 'build/2.6.2/d3', 'd3.map': 'build/2.6.2/d3.map',
                         'd3.map.provenance': 'build/2.6.2/d3.map.provenance'}
                if obj is not None:
                    names['object'] = obj
                for label, name in names.items():
                    source = regular(entry['root'], name)
                    shutil.copyfile(source, dest / label)
                    if digest(dest / label) != files[name]:
                        raise ValueError('owner_copied_artifact_binding_mismatch')
                    observed.append((source, files[name]))
                key = mode + '_' + version
                actual = {'elf_sha256': digest(dest / 'd3'), 'map_sha256': digest(dest / 'd3.map'),
                          'record_sha256': digest(dest / 'd3.map.provenance')}
                if any(actual[name] != linked_evidence[key][name] for name in actual):
                    raise ValueError('owner_link_binding_mismatch')
                answer['bindings'][key] = dict(actual, object_sha256=digest(dest / 'object') if obj is not None else None,
                                               object_state='present' if obj is not None else 'absent_in_unimplemented_baseline_commit',
                                               receipt_sha256=entry['receipt_sha256'], commit=evidence[version]['commit'])
                snapshots[(version, mode)] = dest
        required = {row['Name'] for row in targets}
        object_rows, symbols = {}, {}
        for (version, mode), dest in snapshots.items():
            if not (dest / 'object').exists():
                result = {'results': [{'symbol': row['Name'], 'reason': 'missing'} for row in all_rows], 'symbols': []}
            else:
                result = strict.verify(dest / 'object', all_rows) if not qualified else {'results': [], 'symbols': []}
            if not qualified and [r.get('symbol') for r in result['results']] != [r['Name'] for r in all_rows]:
                raise ValueError('owner_object_result_identity_mismatch')
            object_rows[(version, mode)] = {r['symbol']: object_status(r) for r in result['results']}
            symbols[(version, mode)] = set(result['symbols'])
        if not qualified:
            modes = {}
            for mode in ('normal', 'lua'):
                before, after = object_rows[('baseline', mode)], object_rows[('candidate', mode)]
                regressions = sum(a != after[name] and not (name in required and after[name] == 'ok') for name, a in before.items())
                pending = sum(after[name] != 'ok' for name in required)
                modes[mode] = {'baseline_counts': dict(Counter(before.values())), 'candidate_counts': dict(Counter(after.values())),
                               'regressions': regressions, 'required_not_ok': pending}
            answer['object'] = {'status': 'fail' if any(v['regressions'] for v in modes.values()) else
                                'partial' if any(v['required_not_ok'] for v in modes.values()) else 'pass',
                                'reason': 'strict_original_object_named_relocations', 'modes': modes}
        expected_names = {r['Name'] for r in all_rows}
        extras = {}
        for mode in ('normal', 'lua'):
            before, after = symbols[('baseline', mode)], symbols[('candidate', mode)]
            extra = after - expected_names
            # As in integration.py, unknown emissions/data need independent audit.
            # Preserve all emitted owner members, even those outside selected targets.
            elf = strict.Elf(snapshots[('candidate', mode)] / 'object')
            strings = elf.sections[struct.unpack_from('<H', elf.d, 0x3e)[0]]
            payload = 0
            for section in elf.sections:
                pos = strings[4] + section[0]
                name = elf.d[pos:elf.d.index(b'\0', pos)]
                payload += bool(section[2] & 2 and not section[2] & 4 and section[5] and name != b'.eh_frame')
            extras[mode] = {'extra_count': len(extra), 'extra_set_sha256': hashlib.sha256(json.dumps(sorted(extra)).encode()).hexdigest(),
                            'lost_emissions': len(before - after), 'missing_owner_count': len(expected_names - after),
                            'allocated_data_sections_requiring_audit': payload,
                            'unattributed_executable_bytes': executable_coverage(strict, snapshots[('candidate', mode)] / 'object')}
            baseline_object = snapshots[('baseline', mode)] / 'object'
            extras[mode]['baseline_unattributed_executable_bytes'] = executable_coverage(strict, baseline_object) if baseline_object.exists() else 0
        answer['emitted_extras'] = {'status': 'fail' if any(v['lost_emissions'] for v in extras.values()) else
                                    'partial' if qualified or any(v['extra_count'] or v['missing_owner_count'] or v['allocated_data_sections_requiring_audit'] or v['unattributed_executable_bytes'] for v in extras.values()) else 'pass',
                                    'reason': 'whole_owner_emission_and_allocated_data_accounting', 'modes': extras}
        eh, eh_values = {}, {}
        for version in ('baseline', 'candidate'):
            links = {mode: snapshots[(version, mode)] / 'd3' for mode in ('normal', 'lua')}
            values = []
            for mode in ('normal', 'lua'):
                for target in targets:
                    obj = snapshots[(version, mode)] / 'object'
                    result = unwind.audit(context / 'main.elf', obj, links, target) if obj.exists() else {'status': 'pending'}
                    values.append('pass' if result.get('status') == 'pass' and
                                  exact_unwind_programs(strict, unwind, context / 'main.elf', obj, links, target) else 'pending')
            eh[version] = dict(Counter(values))
            eh_values[version] = values
        eh_lost = sum(a == 'pass' and b != 'pass' for a,b in zip(eh_values['baseline'],eh_values['candidate']))
        answer['unwind'] = {'status': 'fail' if eh_lost else 'pass' if not eh['candidate'].get('pending') else 'partial',
                            'reason': 'original_object_both_link_unwind_selected_targets', 'counts': eh,
                            'regressions': eh_lost}
        if any(digest(path) != sha for path, sha in observed) or digest(context / 'main.elf') != original_sha:
            raise ValueError('owner_inputs_changed_during_validation')
        for version in ('baseline', 'candidate'):
            source_check(evidence[version]['source'], source_hashes[version])
        for key, dest in snapshots.items():
            record = answer['bindings'][key[1] + '_' + key[0]]
            if any(digest(dest / name) != record[field] for name,field in (
                    ('object','object_sha256'),('d3','elf_sha256'),('d3.map','map_sha256'),
                    ('d3.map.provenance','record_sha256')) if record[field] is not None):
                raise ValueError('owner_snapshot_changed_during_validation')
        if digest(evidence['_path']) != evidence['_manifest_sha256']:
            raise ValueError('evidence_manifest_changed')
        for entry in evidence['toolchain'].values():
            if digest(entry['path']) != entry['sha256']:
                raise ValueError('toolchain_changed_during_validation')
    return answer
