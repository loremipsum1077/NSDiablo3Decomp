"""Separate two-owner recovery policy; never grants exact-match credit."""
import csv
import json
from pathlib import Path, PurePosixPath
import shutil
import tempfile

from validation import common as c
import receipts as r
import recovery

KIND = 'recovery-group-v1'


def partition(owners, targets, functions, files):
    if not isinstance(owners, list) or len(owners) != 2 or not all(isinstance(x, str) for x in owners) or len(set(owners)) != 2:
        raise c.Halt('Exactly two distinct owners required')
    if owners != sorted(owners):
        raise c.Halt('Owners must be in canonical sorted order')
    for owner in owners:
        if (not isinstance(owner, str) or not owner or '\\' in owner or
                PurePosixPath(owner).is_absolute() or any(x in ('', '.', '..') for x in owner.split('/'))):
            raise c.Halt('Normalized owner path required')
    by_address = {}
    for row in functions:
        address = row['Address']
        if address in by_address:
            raise c.Halt('Duplicate inventory address')
        by_address[address] = row
    assignment = {}
    for row in files:
        address = row['Address']
        if address in assignment:
            raise c.Halt('Duplicate owner assignment')
        assignment[address] = row['File']
    result = []
    for owner in owners:
        members = [address for address, path in assignment.items() if path == owner]
        if not members or any(address not in by_address for address in members):
            raise c.Halt('Each group owner needs complete function identities')
        identities = sorted([{'address': a, 'name': by_address[a]['Name']} for a in members],
                            key=lambda x: (x['address'], x['name']))
        if any('@' in row['name'] for row in identities):
            raise c.Halt('Qualified owner names unsupported')
        result.append({'owner': owner, 'kind': 'function-owner', 'functions': identities})
    union = sorted([x for part in result for x in part['functions']], key=lambda x: (x['address'], x['name']))
    if not 1 <= len(union) <= 64 or len({(x['address'], x['name']) for x in union}) != len(union):
        raise c.Halt('Invalid bounded group union')
    if len({x['name'] for x in union}) != len(union):
        raise c.Halt('Group function names must be unique')
    if sorted(targets, key=lambda x: (x['address'], x['name'])) != union:
        raise c.Halt('Targets must equal the complete group partition')
    return result


def validate_bound_scope(binding):
    """Recheck the complete permitted change and input lineage from the binding."""
    if binding.get('policy') != 'recovery-group' or binding.get('kind') != KIND or type(binding.get('post_landing')) is not bool:
        raise c.Halt('Invalid group policy or landing state')
    owners = binding['owners']
    changed = binding['changed_paths']
    if not isinstance(changed, list) or not all(isinstance(p, str) and p and not p.startswith('/') and
            all(part not in ('', '.', '..') for part in p.split('/')) and '\\' not in p for p in changed) or len(set(changed)) != len(changed):
        raise c.Halt('Invalid group changed paths')
    def allowed(p):
        return (p in {'src/' + owner for owner in owners} or
                (p.startswith(('src/', 'include/')) and p.endswith(('.h', '.hpp', '.inl'))) or
                p.startswith(('tests/', 'docs/')) or
                (binding['post_landing'] and p == 'data/2.6.2/functions.csv'))
    if any(not allowed(p) for p in changed):
        raise c.Halt('Recovery group scope excludes unrelated owners and tools')
    before = binding['versions']['baseline']
    after = binding['versions']['candidate']
    if any('src/' + owner not in after['source'] for owner in owners):
        raise c.Halt('Both recovered owner sources required')
    if set(before['support']) != set(r.SUPPORT) or set(after['support']) != set(r.SUPPORT):
        raise c.Halt('Complete checker support required')
    inventory_name = 'data/2.6.2/functions.csv'
    for name in r.SUPPORT:
        if before['support'][name] != after['support'][name] and (name != inventory_name or not binding['post_landing']):
            raise c.Halt('Checker configuration/ownership/data lineage changed')
    inventory_changed = before['support'][inventory_name] != after['support'][inventory_name]
    if inventory_changed != (inventory_name in changed):
        raise c.Halt('Inventory changed paths do not reproduce')
    old_source, new_source = before['source'], after['source']
    source_changes = {name for name in old_source.keys() | new_source.keys() if old_source.get(name) != new_source.get(name)}
    reported_source = {name for name in changed if name.startswith(('src/', 'include/', 'toolchain/')) or name == 'CMakeLists.txt'}
    if source_changes != reported_source:
        raise c.Halt('Bound changed source paths do not reproduce')


def configure(args, binding):
    if not args.owners or args.owner or not args.nss or not args.nss_sha256:
        raise c.Halt('Group requires --owners, --nss and --nss-sha256')
    manifest = r.read_json(r.regular(args.owners))
    if not isinstance(manifest, dict) or set(manifest) != {'format', 'owners'} or type(manifest['format']) is not int or manifest['format'] != 1:
        raise c.Halt('Invalid owners manifest')
    if not r.HEX.fullmatch(args.nss_sha256) or c.sha(r.regular(args.nss)) != args.nss_sha256:
        raise c.Halt('NSS input pin mismatch')
    owners = manifest['owners']
    parts = partition(owners, binding['targets']['functions'], recovery.rows(args.repo, args.baseline), recovery.rows(args.repo, args.baseline, 'function_files.csv'))
    recovery.inventory(recovery.rows(args.repo, args.baseline), recovery.rows(args.repo, args.candidate), binding['targets']['functions'], args.post_landing)
    changed = r.git(args.repo, 'diff', '--no-renames', '--name-only', '-z', args.baseline, args.candidate).decode().split('\0')[:-1]
    binding.update(policy='recovery-group', kind=KIND, owners=owners, partition=parts, nss_sha256=args.nss_sha256,
                   post_landing=args.post_landing, changed_paths=changed, nss_path=str(Path(args.nss).resolve()))
    validate_bound_scope(binding)
    folder = Path(__file__).parent
    binding['recovery_tools'] = {str(p.relative_to(folder)): c.sha(p) for p in sorted(folder.rglob('*.py'))
        if not any(part in ('common', '__pycache__') for part in p.relative_to(folder).parts)}


def global_definitions(path):
    import struct
    from validation.elf import Elf
    elf = Elf(path)
    found = set()
    for section in elf.sections:
        if section[1] != 2:
            continue
        strings = elf.sections[section[6]]
        for offset in range(section[4], section[4] + section[5], 24):
            name, info, _, index, _, _ = struct.unpack_from('<IBBHQQ', elf.d, offset)
            if index and index < 0xff00 and info >> 4 in (1, 2) and info & 15 in (1, 2):
                start = strings[4] + name
                value = elf.d[start:elf.d.index(b'\0', start)].decode()
                if value:
                    found.add(value)
    return found


def route_objects(roots, owners):
    from validation.recovery_diagnostics import runtime_digest
    maps = {label: {str(p.relative_to(root/'objects')): p for p in (root/'objects').rglob('*.obj')} for label, root in roots.items()}
    suffixes = {owner: '/src/'+owner+'.obj' for owner in owners}
    owned = {owner: {label: [p for name, p in objs.items() if name.endswith(suffix)]
             for label, objs in maps.items()} for owner, suffix in suffixes.items()}
    for owner, objects in owned.items():
        if len(objects['candidate']) != 1 or len(objects['baseline']) > 1:
            raise c.Halt('Unique group owner object required: '+owner)
    for label in ('baseline', 'candidate'):
        paths = [p for objects in owned.values() for p in objects[label]]
        if len(paths) != len(set(paths)):
            raise c.Halt('Group object routed to multiple owners')
    for name, obj in maps['baseline'].items():
        if any(name.endswith(s) for s in suffixes.values()):
            continue
        other = maps['candidate'].get(name)
        if other is None or (c.sha(obj) != c.sha(other) and runtime_digest(obj) != runtime_digest(other)):
            raise c.Halt('Unrelated runtime object changed: '+name)
    if any(name not in maps['baseline'] and not any(name.endswith(s) for s in suffixes.values()) for name in maps['candidate']):
        raise c.Halt('Unrelated extra object')
    return owned


def audit(args, output, binding, evidence, baseline_rows=None, file_rows=None):
    from validate_data import validate_data
    from validation import strict, unwind
    from validation.recovery_diagnostics import CallFailureEvidence
    from validation.elf import Elf
    import contributor_adapter as adapter
    import recovery_data, recovery_mapping
    output = Path(output)
    baseline_rows = baseline_rows if baseline_rows is not None else recovery.rows(args.repo, args.baseline)
    file_rows = file_rows if file_rows is not None else recovery.rows(args.repo, args.baseline, 'function_files.csv')
    if partition(binding['owners'], binding['targets']['functions'], baseline_rows, file_rows) != binding['partition']:
        raise c.Halt('Group partition changed')
    report = {'kind': KIND, 'binding_sha256': c.sha(output/'binding.json'), 'strict_certified': False,
              'review_required': True, 'debt': {}, 'modes': {}, 'obligations': {}}
    def obligation(key, value):
        report['obligations'][key] = {'evidence_sha256': c.digest(value), 'evidence': value}
    targets = binding['targets']['functions']
    selected = {(x['address'], x['name']) for x in targets}
    links = {label: {mode: Path(evidence[label][mode]['root'])/'build/2.6.2/d3' for mode in ('normal', 'lua')} for label in ('baseline', 'candidate')}
    with tempfile.TemporaryDirectory(prefix='d3-group-audit-') as temp:
        context = Path(temp)/'data/2.6.2'
        context.mkdir(parents=True)
        shutil.copy2(args.original_elf, context/'main.elf')
        # Retained support is authenticated by the receipt checkpoint before verification.
        support = Path(evidence['baseline']['normal']['root']).parent/'support/data/2.6.2'
        for name in ('functions.csv', 'function_files.csv', 'data_symbols.csv'):
            shutil.copy2(support/name, context/name)
        strict.configure(Path(temp))
        for mode in ('normal', 'lua'):
            roots = {label: Path(evidence[label][mode]['root']) for label in ('baseline', 'candidate')}
            results = {label: r.read_json(root/'result.json') for label, root in roots.items()}
            request = {(x['Address'], x['Name']) for x in baseline_rows if x['Quality'] != 'U' or (x['Address'], x['Name']) in selected}
            call_evidence = CallFailureEvidence(roots)
            if set(recovery.result_map(results['baseline'])) != request:
                raise c.Halt('Baseline request omitted a required identity')
            report['debt'][mode] = recovery.compare(results['baseline'], results['candidate'], targets,
                {(x['Address'], x['Name']) for x in baseline_rows if x['Quality'] == 'U'}, call_evidence)
            report.setdefault('stable_call_failures', {})[mode] = call_evidence.witnesses
            report['modes'][mode] = {'results_sha256': {label: c.sha(root/'result.json') for label, root in roots.items()}}
            for row in results['candidate']['results']:
                if (row['address'], row['name']) in selected and row['status'] != 'ok':
                    obligation(mode+':linked:'+row['address'], row)
            owned = route_objects(roots, binding['owners'])
            for label in ('baseline', 'candidate'):
                objects = [owned[owner][label] for owner in binding['owners']]
                if all(objects) and global_definitions(objects[0][0]) & global_definitions(objects[1][0]):
                    raise c.Halt('Cross-owner duplicate global definitions')
            for index, part in enumerate(binding['partition']):
                owner = part['owner']
                prefix = mode+':owner-'+str(index)+':'
                objects = owned[owner]
                identities = {(x['address'], x['name']) for x in part['functions']}
                rows = [x for x in baseline_rows if (x['Address'], x['Name']) in identities]
                if not objects['baseline'] and any(x['Quality'] != 'U' for x in rows):
                    raise c.Halt('Previously emitted owner disappeared')
                observations, emissions = {}, {}
                for label in ('baseline', 'candidate'):
                    obj = objects[label][0] if objects[label] else None
                    observations[label] = strict.verify(obj, rows) if obj else {'results': [], 'symbols': []}
                    emissions[label] = [v for v in strict.symbols(Elf(obj)).values() if v['section'] and v['name'] and v['type'] not in (3, 4)] if obj else []
                    if obj and adapter.executable_coverage(strict, obj):
                        raise c.Halt('Unattributed owner executable bytes')
                prior = {x['symbol']: x for x in observations['baseline']['results']}
                if any(prior.get(x['symbol'], {}).get('exact') and not x.get('exact') for x in observations['candidate']['results']):
                    raise c.Halt('Previously proven object evidence lost')
                before, after = (set(observations[label]['symbols']) for label in ('baseline', 'candidate'))
                if before - after or not {x['Name'] for x in rows} <= after:
                    raise c.Halt('Lost or missing owner emissions')
                # Selected functions must not also be defined by the other group owner.
                other = owned[binding['owners'][1-index]]['candidate'][0]
                other_names = {v['name'] for v in strict.symbols(Elf(other)).values() if v['section'] and v['type'] == 2}
                if {x['Name'] for x in rows} & other_names:
                    raise c.Halt('Cross-owner duplicate or misrouted function')
                mapping = recovery_mapping.compare(objects['baseline'][0] if objects['baseline'] else None, objects['candidate'][0], {x['Name'] for x in rows})
                if mapping['new_data_sections']:
                    raise c.Halt('Group policy does not admit new data')
                obligation(prefix+'owner-mapping-symbols', mapping)
                for key, value in recovery_mapping.extra_obligations(mapping).items():
                    obligation(prefix+key, value)
                obligation(prefix+'owner-defined-symbols-data-extras', emissions)
                obligation(prefix+'owner-object-relocations-extras', observations)
                current = [unwind.audit(context/'main.elf', objects['candidate'][0], links['candidate'], x) for x in rows]
                prior_unwind = [unwind.audit(context/'main.elf', objects['baseline'][0], links['baseline'], x) for x in rows] if objects['baseline'] else []
                if any(a.get('status') == 'pass' and b.get('status') != 'pass' for a, b in zip(prior_unwind, current)):
                    raise c.Halt('Previously proven unwind evidence lost')
                obligation(prefix+'abi-unwind', {'baseline': prior_unwind, 'candidate': current})
                obligation(prefix+'object-paths', {label: [str(p.relative_to(roots[label]/'objects')) for p in objects[label]] for label in objects})
            data = {}
            retained = {}
            for label in ('baseline', 'candidate'):
                ledger = validate_data(context/'data_symbols.csv', args.original_elf, links[label][mode], args.nss)
                data[label] = {(x['address'], x['name']): x['validation'] for x in ledger['rows']}
                if len(data[label]) != len(ledger['rows']):
                    raise c.Halt('Duplicate data identity')
                name = mode+'-'+label+'-data.json'
                c.write(output/name, sorted((list(k), v) for k, v in data[label].items()))
                retained[label] = {'path': name, 'sha256': c.sha(output/name)}
            recovery_data.compare_outcomes(data['baseline'], data['candidate'], [])
            obligation(mode+':global-data-abi', retained)
    if c.sha(args.original_elf) != binding['original_sha256'] or c.sha(args.nss) != binding['nss_sha256']:
        raise c.Halt('Inputs changed during group audit')
    obligation('source-semantics-provenance', {'changed_paths': binding['changed_paths'], 'source': binding['versions']['candidate']['source']})
    report['debt_sha256'] = c.digest(report['debt'])
    c.write(output/'recovery-group-audit.json', report)
    c.write(output/'review-template.json', review_template(output, report))


def review_template(output, report):
    return {'kind': 'independent-recovery-group-review-v1', 'binding_sha256': c.sha(output/'binding.json'),
            'audit_sha256': c.sha(output/'recovery-group-audit.json'), 'evidence_sha256': c.sha(output/'evidence.json'),
            'reviewer': '', 'independent': False, 'blocking_findings': [], 'obligations': {
                k: {'evidence_sha256': v['evidence_sha256'], 'decision': 'pending', 'rationale': '', 'evidence_reference': ''}
                for k, v in report['obligations'].items()}}


def verify_audit(output, binding, state):
    from types import SimpleNamespace
    output = Path(output)
    existing = r.read_json(r.regular(output/'recovery-group-audit.json'))
    if c.sha(output/'recovery-group-audit.json') != state['audit_sha256'] or existing.get('kind') != KIND:
        raise c.Halt('Stale group audit')
    folder = Path(__file__).parent
    if any(c.sha(r.regular(folder/name)) != sha for name, sha in binding['recovery_tools'].items()):
        raise c.Halt('Trusted recovery tooling changed; rebuild evidence')
    evidence = r.read_json(output/'evidence.json')
    support = Path(evidence['baseline']['normal']['root']).parent/'support/data/2.6.2'
    with (support/'functions.csv').open(newline='') as stream:
        function_rows = list(csv.DictReader(stream))
    with (support/'function_files.csv').open(newline='') as stream:
        file_rows = list(csv.DictReader(stream))
    candidate_support = Path(evidence['candidate']['normal']['root']).parent/'support/data/2.6.2'
    with (candidate_support/'functions.csv').open(newline='') as stream:
        candidate_rows = list(csv.DictReader(stream))
    recovery.inventory(function_rows, candidate_rows, binding['targets']['functions'], binding['post_landing'])
    validate_bound_scope(binding)
    args = SimpleNamespace(original_elf=support/'main.elf', nss=Path(binding['nss_path']))
    with tempfile.TemporaryDirectory(prefix='d3-group-verify-') as temp:
        dest = Path(temp)
        shutil.copy2(output/'binding.json', dest/'binding.json')
        shutil.copy2(output/'evidence.json', dest/'evidence.json')
        audit(args, dest, binding, evidence, function_rows, file_rows)
        for name in ('recovery-group-audit.json', 'normal-baseline-data.json', 'normal-candidate-data.json',
                     'lua-baseline-data.json', 'lua-candidate-data.json'):
            if c.sha(r.regular(output/name)) != c.sha(dest/name):
                raise c.Halt('Group audit or retained proof does not reproduce: '+name)
    return existing


def review(output, path, expected):
    output = Path(output).resolve()
    r.verify(output, policy='recovery-group')
    if not r.HEX.fullmatch(expected or '') or c.sha(r.regular(path)) != expected:
        raise c.Halt('Independent review pin mismatch')
    value = r.read_json(path)
    template = review_template(output, r.read_json(output/'recovery-group-audit.json'))
    if set(value) != set(template) or any(value[k] != template[k] for k in ('kind', 'binding_sha256', 'audit_sha256', 'evidence_sha256')):
        raise c.Halt('Group review binding mismatch')
    if not isinstance(value['reviewer'], str) or not value['reviewer'].strip() or value['independent'] is not True or value['blocking_findings'] != []:
        raise c.Halt('Independent review missing or blocking findings unresolved')
    if value['obligations'].keys() != template['obligations'].keys():
        raise c.Halt('Review obligation omitted')
    for key, row in value['obligations'].items():
        if (set(row) != set(template['obligations'][key]) or row['evidence_sha256'] != template['obligations'][key]['evidence_sha256'] or
                row['decision'] not in ('verified', 'safe-uncertified') or
                any(not isinstance(row[k], str) or not row[k].strip() for k in ('rationale', 'evidence_reference'))):
            raise c.Halt('Unreviewed or unsafe obligation: '+key)
    return {'status': 'recovery_group_review_verified', 'strict_certified': False, 'reviewer': value['reviewer'], 'review_sha256': expected}
