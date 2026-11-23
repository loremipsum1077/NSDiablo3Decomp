"""Opt-in recovery evidence policy. Never certifies exactness or grants O credit.

Use trusted local tooling and an independently supplied, hash-pinned review.
A review is an accountable attestation, not a cryptographic proof of independence.
"""
import csv
import io
import json
from pathlib import Path
import re
import tempfile

from validation import common as c, integration
import receipts as r

KIND = 'recovery-only-v1'


def rows(repo, commit, name='functions.csv'):
    return list(csv.DictReader(io.StringIO(r.git(repo, 'show', commit + ':data/2.6.2/' + name).decode())))


def inventory(before, after, targets, post=False):
    if len(before) != len(after):
        raise c.Halt('Inventory membership changed')
    chosen = {(x['address'], x['name']) for x in targets}
    for a, b in zip(before, after):
        permitted = dict(a)
        if post and (a['Address'], a['Name']) in chosen and a['Quality'] == 'U':
            permitted['Quality'] = 'M'
        if b != (permitted if post else a):
            raise c.Halt('Recovery inventory permits only selected U to M; never O credit')


def configure(args, binding):
    if not args.owner or not args.nss or not args.nss_sha256:
        raise c.Halt('Recovery requires --owner, --nss and --nss-sha256')
    if not r.HEX.fullmatch(args.nss_sha256) or c.sha(r.regular(args.nss)) != args.nss_sha256:
        raise c.Halt('NSS input pin mismatch')
    baseline, candidate = args.baseline, args.candidate
    members = {x['Address'] for x in rows(args.repo, baseline, 'function_files.csv') if x['File'] == args.owner}
    all_rows = [x for x in rows(args.repo, baseline) if x['Address'] in members]
    selected = {(x['address'], x['name']) for x in binding['targets']['functions']}
    if not 1 <= len(all_rows) <= 64 or selected != {(x['Address'], x['Name']) for x in all_rows}:
        raise c.Halt('Recovery v1 requires an explicit complete owner of 1 to 64 identities')
    inventory(rows(args.repo, baseline), rows(args.repo, candidate), binding['targets']['functions'], args.post_landing)
    for name in r.SUPPORT:
        if name.endswith('functions.csv'):
            continue
        if binding['versions']['baseline']['support'][name] != binding['versions']['candidate']['support'][name]:
            raise c.Halt('Checker configuration/ownership/data lineage changed')
    changed = r.git(args.repo, 'diff', '--no-renames', '--name-only', '-z', baseline, candidate).decode().split('\0')[:-1]
    allowed = lambda p: p == 'src/' + args.owner or (p.startswith(('src/', 'include/')) and p.endswith(('.h', '.hpp', '.inl'))) or p.startswith(('tests/', 'docs/'))
    if any(not allowed(p) and not (args.post_landing and p == 'data/2.6.2/functions.csv') for p in changed):
        raise c.Halt('Recovery scope excludes tools, build recipes, other owners and inventory edits')
    if 'src/' + args.owner not in binding['versions']['candidate']['source']:
        raise c.Halt('Recovered owner source missing')
    binding.update(policy='recovery-only', owner=args.owner, nss_sha256=args.nss_sha256,
                   nss_path=str(Path(args.nss).resolve()),
                   post_landing=args.post_landing, changed_paths=changed)
    dwarf_path, dwarf_sha = getattr(args, 'dwarfdump', None), getattr(args, 'dwarfdump_sha256', None)
    if dwarf_path is not None or dwarf_sha is not None:
        if (dwarf_path is None or not r.HEX.fullmatch(dwarf_sha or '')
                or c.sha(r.regular(Path(dwarf_path).resolve())) != dwarf_sha):
            raise c.Halt('Named-data DWARF reader requires an exact executable pin')
        binding['named_data_dwarfdump'] = {'path': str(Path(dwarf_path).resolve()), 'sha256': dwarf_sha}
    # Bind the entire trusted Python validator closure, not candidate tooling.
    folder = Path(__file__).parent
    binding['recovery_tools'] = {str(p.relative_to(folder)): c.sha(p) for p in sorted(folder.rglob('*.py'))
        if not any(part in ('common', '__pycache__') for part in p.relative_to(folder).parts)}
    import recovery_baseline_reference
    recovery_baseline_reference.configure(args, binding)


def result_map(result):
    if set(result) - {'format', 'results'} or result.get('format', 1) != 1:
        raise c.Halt('Malformed checker result')
    integration.identities(result['results'])
    out = {}
    for row in result['results']:
        if set(row) != {'address', 'name', 'status', 'reason'} or not isinstance(row['reason'], str):
            raise c.Halt('Incomplete precise checker diagnostic')
        if row['status'] not in ('ok', 'mismatch', 'not-checkable'):
            raise c.Halt('Missing/error checker identity blocks recovery')
        if row['status'] == 'ok' and row['reason']:
            raise c.Halt('Malformed OK diagnostic')
        out[(row['address'], row['name'])] = row
    return out


def compare(before, after, targets, absent_allowed=(), call_evidence=None, function_got_evidence=None):
    old, new = result_map(before), result_map(after)
    selected = {(x['address'], x['name']) for x in targets}
    if old.keys() != new.keys() or not selected <= old.keys():
        raise c.Halt('Recovery request lost or added identities; target omitted')
    debt = []
    for identity, a in old.items():
        b = new[identity]
        if a['status'] != 'ok':
            debt.append(a)
        if identity not in selected:
            if a != b and not ((call_evidence and call_evidence.equivalent(a, b))
                    or (function_got_evidence and function_got_evidence.equivalent(a, b))):
                raise c.Halt('Unselected status or precise proof diagnostic changed: ' + str(identity))
        else:
            if a['status'] == 'not-checkable' and a['reason'] != 'unknown function: ' + a['name']:
                raise c.Halt('Ambiguous baseline target is not recoverable evidence')
            # Only an explicitly absent U source may become emitted. Other missing
            # or ambiguous lookup failures are never waivable by a review file.
            absent = a['status'] == 'not-checkable' and a['reason'] == 'unknown function: ' + a['name'] and identity in set(absent_allowed)
            if a['status'] == 'not-checkable' and not absent:
                raise c.Halt('Absent baseline target must be explicitly U')
            if b['status'] not in ('ok', 'mismatch'):
                raise c.Halt('Target missing or ambiguous')
            if not (a == b or b['status'] == 'ok' or absent):
                raise c.Halt('Known target failure worsened or changed proof category')
    return debt


def runtime_object(path):
    """Retain exact non-debug object sections, including relocations and unwind."""
    from validation.elf import Elf
    import struct
    elf = Elf(path)
    strings = elf.sections[struct.unpack_from('<H', elf.d, 0x3e)[0]]
    answer = {}
    for section in elf.sections:
        at = strings[4] + section[0]
        name = elf.d[at:elf.d.index(b'\0', at)].decode()
        if not name or '.debug' in name or name in ('.shstrtab', '.strtab'):
            continue
        data = elf.d[section[4]:section[4]+section[5]] if section[1] != 8 else b''
        answer[name] = {'type': section[1], 'flags': section[2], 'size': section[5],
                        'sha256': __import__('hashlib').sha256(data).hexdigest()}
    # Symbol names must remain bound even when debug strings change section offsets.
    from validation import strict
    answer['symbols'] = strict.symbols(elf)
    return answer


def audit(args, output, binding, evidence):
    from validate_data import validate_data
    from validation import strict, unwind
    import contributor_adapter as adapter
    targets = binding['targets']['functions']
    report = {'kind': KIND, 'binding_sha256': c.sha(output/'binding.json'),
              'strict_certified': False, 'review_required': True, 'debt': {}, 'modes': {}, 'obligations': {}}
    def obligation(key, value):
        report['obligations'][key] = {'evidence_sha256': c.digest(value), 'evidence': value}
    with tempfile.TemporaryDirectory(prefix='d3-recovery-audit-') as temp:
        context = Path(temp)/'data/2.6.2'; context.mkdir(parents=True)
        import shutil
        shutil.copy2(args.original_elf, context/'main.elf')
        for name in ('functions.csv', 'function_files.csv', 'data_symbols.csv'):
            (context/name).write_bytes(r.git(args.repo, 'show', args.baseline+':data/2.6.2/'+name))
        strict.configure(Path(temp))
        selected_rows = [x for x in rows(args.repo, args.baseline) if (x['Address'], x['Name']) in {(t['address'],t['name']) for t in targets}]
        links = {label: {mode: Path(evidence[label][mode]['root'])/'build/2.6.2/d3' for mode in ('normal','lua')} for label in ('baseline','candidate')}
        for mode in ('normal', 'lua'):
            roots = {label: Path(evidence[label][mode]['root']) for label in ('baseline','candidate')}
            results = {label: r.read_json(root/'result.json') for label,root in roots.items()}
            expected_request = {(x['Address'], x['Name']) for x in rows(args.repo, args.baseline)
                                if x['Quality'] != 'U' or (x['Address'], x['Name']) in {(t['address'],t['name']) for t in targets}}
            if set(result_map(results['baseline'])) != expected_request:
                raise c.Halt('Baseline request omitted a required identity')
            from validation.recovery_diagnostics import CallFailureEvidence
            from validation.recovery_function_got import FunctionGOTEvidence
            call_evidence = CallFailureEvidence(roots)
            function_got = FunctionGOTEvidence(roots, args.original_elf, args.nss, selected_rows, args.owner)
            report['debt'][mode] = compare(results['baseline'], results['candidate'], targets, {(x['Address'],x['Name']) for x in selected_rows if x['Quality']=='U'}, call_evidence, function_got)
            report.setdefault('stable_call_failures', {})[mode] = call_evidence.witnesses
            if function_got.witnesses:
                obligation(mode+':function-got-refinements', function_got.witnesses)
            report['modes'][mode] = {'results_sha256': {label: c.sha(root/'result.json') for label,root in roots.items()}}
            for row in results['candidate']['results']:
                if (row['address'],row['name']) in {(t['address'],t['name']) for t in targets} and row['status'] != 'ok':
                    obligation(mode+':linked:'+row['address'], row)
            object_maps = {label: {str(p.relative_to(root/'objects')):p for p in (root/'objects').rglob('*.obj')} for label,root in roots.items()}
            owner_suffix = '/src/' + args.owner + '.obj'
            owned = {label: [p for name,p in objs.items() if name.endswith(owner_suffix)] for label,objs in object_maps.items()}
            if len(owned['candidate']) != 1 or len(owned['baseline']) > 1:
                raise c.Halt('Unique owner object required')
            if not owned['baseline'] and any(x['Quality'] != 'U' for x in selected_rows):
                raise c.Halt('Previously emitted owner disappeared')
            for name,p in object_maps['baseline'].items():
                if name.endswith(owner_suffix): continue
                q = object_maps['candidate'].get(name)
                if q is None or (c.sha(p) != c.sha(q) and runtime_object(p) != runtime_object(q)):
                    raise c.Halt('Unrelated runtime object changed: ' + name)
            if any(name not in object_maps['baseline'] and not name.endswith(owner_suffix) for name in object_maps['candidate']):
                raise c.Halt('Unrelated extra object')
            observations = {}
            emissions = {}
            for label in ('baseline','candidate'):
                obj = owned[label][0] if owned[label] else None
                result = strict.verify(obj, selected_rows) if obj and not any('@' in x['Name'] for x in selected_rows) else {'results': [], 'symbols': []}
                observations[label] = result
                from validation.elf import Elf
                emissions[label] = [v for v in strict.symbols(Elf(obj)).values() if v['section'] and v['name'] and v['type'] not in (3,4)] if obj else []
                if obj:
                    uncovered = adapter.executable_coverage(strict, obj)
                    if uncovered: raise c.Halt('Unattributed owner executable bytes')
            prior = {x['symbol']: x for x in observations['baseline']['results']}
            for x in observations['candidate']['results']:
                if prior.get(x['symbol'], {}).get('exact') and not x.get('exact'):
                    raise c.Halt('Previously proven object relocation/instruction evidence lost')
            before_symbols = set(observations['baseline']['symbols']); after_symbols = set(observations['candidate']['symbols'])
            if before_symbols - after_symbols:
                raise c.Halt('Lost owner emissions')
            if any('@' in x['Name'] for x in selected_rows):
                raise c.Halt('Qualified owner object mapping unsupported; no recovery waiver')
            expected = {x['Name'] for x in selected_rows}
            if not expected <= after_symbols: raise c.Halt('Missing owner emission')
            import recovery_mapping
            scalar_context = dict(original=args.original_elf, nss=args.nss,
                before_receipt=roots['baseline'], after_receipt=roots['candidate'],
                owner_object=owned['candidate'][0], selected_rows=selected_rows, owner=args.owner)
            mapping = recovery_mapping.compare(owned['baseline'][0] if owned['baseline'] else None,
                                               owned['candidate'][0], expected, scalar_context=scalar_context,
                                               named_data='named_data_dwarfdump' in binding)
            obligation(mode+':owner-mapping-symbols', mapping)
            for key, extra in recovery_mapping.extra_obligations(mapping).items():
                obligation(mode+':'+key, extra)
            obligation(mode+':owner-defined-symbols-data-extras', emissions)
            obligation(mode+':owner-object-relocations-extras', observations)
            obj = owned['candidate'][0]
            unwind_rows = [unwind.audit(context/'main.elf', obj, links['candidate'], x) for x in selected_rows]
            prior_unwind = [unwind.audit(context/'main.elf', owned['baseline'][0], links['baseline'], x) for x in selected_rows] if owned['baseline'] else []
            if any(a.get('status') == 'pass' and b.get('status') != 'pass' for a,b in zip(prior_unwind,unwind_rows)):
                raise c.Halt('Previously proven unwind evidence lost')
            obligation(mode+':abi-unwind', {'baseline':prior_unwind,'candidate':unwind_rows})
            # Exact identity/status/reason preservation of all original data, not counts.
            data = {}
            ledgers = {}
            for label in ('baseline','candidate'):
                ledger = validate_data(context/'data_symbols.csv', args.original_elf, links[label][mode], args.nss)
                ledgers[label] = ledger
                data[label] = {(x['address'],x['name']): x['validation'] for x in ledger['rows']}
                if len(data[label]) != len(ledger['rows']): raise c.Halt('Duplicate data identity')
            import recovery_data
            witnesses = recovery_data.prove(mapping, ledgers['candidate'], links['candidate'][mode], args.owner, owned['candidate'][0])
            named_witnesses = named_data_proof(mapping, ledgers['candidate'], roots, binding,
                args.original_elf, args.nss, owned['candidate'][0])
            transitions = recovery_data.compare_outcomes(data['baseline'], data['candidate'], witnesses + named_witnesses)
            obligation(mode+':new-vtable-data-proof', {'witnesses': witnesses,
                'transitions': witness_transitions(transitions, witnesses),
                'nss': str(Path(args.nss).resolve()), 'nss_sha256': binding['nss_sha256']})
            if named_witnesses:
                obligation(mode+':new-named-data-proof', {'witnesses': named_witnesses,
                    'transitions': witness_transitions(transitions, named_witnesses)})
            # Retain full records separately; bind their hashes in the review obligation.
            retained={}
            for label, values in data.items():
                name=mode+'-'+label+'-data.json'
                c.write(output/name, sorted((list(k),v) for k,v in values.items()))
                retained[label]={'path':name,'sha256':c.sha(output/name)}
            obligation(mode+':owner-data-abi', retained)
    if c.sha(args.original_elf) != binding['original_sha256'] or c.sha(args.nss) != binding['nss_sha256']:
        raise c.Halt('Inputs changed during recovery audit')
    obligation('source-semantics-provenance', {'changed_paths':binding['changed_paths'], 'source':binding['versions']['candidate']['source']})
    report['debt_sha256'] = c.digest(report['debt'])
    c.write(output/'recovery-audit.json', report)
    c.write(output/'review-template.json', review_template(output, report))


def named_data_proof(mapping, ledger, roots, binding, original, nss, owner_object):
    if not mapping.get('new_named_data_sections'):
        return []
    from recovery_named_data import prove
    reader = named_data_reader(binding)
    if reader is None:
        raise c.Halt('Named-data proof lacks a pinned DWARF reader')
    witnesses = prove(mapping, ledger, original, nss, binding['owner'], owner_object,
        roots['baseline'], roots['candidate'], original_sha256=binding['original_sha256'],
        nss_sha256=binding['nss_sha256'], dwarfdump_path=reader['path'], dwarfdump_sha256=reader['sha256'])
    if not witnesses or len(witnesses) != len(mapping['new_named_data_sections']):
        raise c.Halt('Incomplete named initialized data proof')
    return witnesses


def named_data_reader(binding):
    reader = binding.get('named_data_dwarfdump')
    if reader is None:
        return None
    if (not isinstance(reader, dict) or set(reader) != {'path', 'sha256'}
            or not isinstance(reader.get('sha256'), str) or not r.HEX.fullmatch(reader['sha256'])
            or not isinstance(reader.get('path'), str) or not Path(reader['path']).is_absolute()
            or str(Path(reader['path']).resolve()) != reader['path']
            or c.sha(r.regular(Path(reader['path']))) != reader['sha256']):
        raise c.Halt('Named-data DWARF reader changed or malformed')
    return reader


def witness_transitions(transitions, witnesses):
    identities = {tuple(w['identity']) for w in witnesses}
    return [t for t in transitions if tuple(t['identity']) in identities]


def review_template(output, report):
    return {'kind':'independent-recovery-review-v1', 'binding_sha256':c.sha(output/'binding.json'),
                'audit_sha256':c.sha(output/'recovery-audit.json'), 'evidence_sha256':c.sha(output/'evidence.json'),
                'reviewer':'', 'independent':False, 'blocking_findings':[], 'obligations':{
                    k:{'evidence_sha256':v['evidence_sha256'], 'decision':'pending', 'rationale':'', 'evidence_reference':''}
                    for k,v in report['obligations'].items()}}


def verify_audit(output, binding, state):
    named_data_reader(binding)
    audit = r.read_json(output/'recovery-audit.json')
    if (c.sha(output/'recovery-audit.json') != state['audit_sha256'] or audit['kind'] != KIND or
            audit['binding_sha256'] != c.sha(output/'binding.json') or audit['strict_certified'] is not False or
            audit['debt_sha256'] != c.digest(audit['debt'])):
        raise c.Halt('Stale recovery audit')
    if not audit.get('obligations') or any(set(v) != {'evidence_sha256','evidence'} or v['evidence_sha256'] != c.digest(v['evidence']) for v in audit['obligations'].values()):
        raise c.Halt('Malformed or changed review obligation evidence')
    for mode in ('normal','lua'):
        entry=audit['obligations'].get(mode+':owner-data-abi')
        if entry:
            for label,v in entry['evidence'].items():
                if label not in ('baseline','candidate') or v['path'] != mode+'-'+label+'-data.json' or c.sha(r.regular(output/v['path'])) != v['sha256']:
                    raise c.Halt('Retained data proof changed')
    evidence = r.read_json(output/'evidence.json')
    targets = binding['targets']['functions']
    for mode in ('normal','lua'):
        roots = {label: Path(evidence[label][mode]['root']) for label in ('baseline','candidate')}
        table = c.rows(roots['baseline']/'data/2.6.2/functions.csv')
        selected = {(t['address'], t['name']) for t in targets}
        expected = {(x['Address'], x['Name']) for x in table if x['Quality'] != 'U' or (x['Address'],x['Name']) in selected}
        old,new = [r.read_json(roots[label]/'result.json') for label in ('baseline','candidate')]
        from validation.recovery_diagnostics import CallFailureEvidence
        from validation.recovery_function_got import FunctionGOTEvidence
        call_evidence = CallFailureEvidence(roots)
        # New proofs require the configured raw NSS path. Historical reports
        # without extensions remain replayable with their frozen validator tree.
        nss = binding.get('nss_path')
        function_got = FunctionGOTEvidence(roots, roots['baseline']/'data/2.6.2/main.elf',
            nss, [x for x in table if (x['Address'],x['Name']) in selected], binding['owner']) if nss is not None else None
        if set(result_map(old)) != expected or compare(old,new,targets, {(x['Address'],x['Name']) for x in table if x['Quality']=='U'}, call_evidence, function_got) != audit['debt'][mode]:
            raise c.Halt('Precise debt changed or identity omitted')
        if call_evidence.witnesses != audit.get('stable_call_failures', {}).get(mode, []):
            raise c.Halt('Stable call-failure witness changed or omitted')
        got_witnesses = function_got.witnesses if function_got else []
        expected_got = ({'evidence': got_witnesses, 'evidence_sha256': c.digest(got_witnesses)}
                        if got_witnesses else None)
        if audit['obligations'].get(mode+':function-got-refinements') != expected_got:
            raise c.Halt('Function-GOT refinement witness changed, omitted or invented')
        if got_witnesses and (c.sha(r.regular(nss)) != binding['nss_sha256']
                or c.sha(roots['baseline']/'data/2.6.2/main.elf') != binding['original_sha256']):
            raise c.Halt('Function-GOT original inputs changed')
        import recovery_mapping
        owned = {label: list((root/'objects').rglob(Path(binding['owner']).name+'.obj'))
                 for label,root in roots.items()}
        suffix = '/src/'+binding['owner']+'.obj'
        owned = {label: [p for p in paths if str(p).endswith(suffix)] for label,paths in owned.items()}
        if len(owned['candidate']) != 1 or len(owned['baseline']) > 1:
            raise c.Halt('Unique retained owner object required')
        scalar_context = (dict(original=roots['baseline']/'data/2.6.2/main.elf', nss=nss,
            before_receipt=roots['baseline'], after_receipt=roots['candidate'],
            owner_object=owned['candidate'][0],
            selected_rows=[x for x in table if (x['Address'],x['Name']) in selected], owner=binding['owner'])
            if nss is not None else None)
        mapping = recovery_mapping.verify_obligations(audit['obligations'], mode,
            owned['baseline'][0] if owned['baseline'] else None, owned['candidate'][0],
            {t['name'] for t in targets}, scalar_context=scalar_context,
            named_data='named_data_dwarfdump' in binding)
        if 'scalar_proof' in mapping and (c.sha(r.regular(nss)) != binding['nss_sha256']
                or c.sha(roots['baseline']/'data/2.6.2/main.elf') != binding['original_sha256']):
            raise c.Halt('Scalar original inputs changed')
        import recovery_data
        proof = audit['obligations'].get(mode+':new-vtable-data-proof', {}).get('evidence')
        if proof is None:
            raise c.Halt('Missing new-vtable data proof obligation')
        retained = {label: recovery_data.read_retained_data(output/(mode+'-'+label+'-data.json'))
                    for label in ('baseline','candidate')}
        witnesses = []
        named_witnesses = []
        if mapping['new_data_sections'] or mapping.get('new_named_data_sections'):
            from validate_data import validate_data
            if proof['nss_sha256'] != binding['nss_sha256'] or c.sha(r.regular(proof['nss'])) != binding['nss_sha256']:
                raise c.Halt('Vtable NSS input changed')
            # Recompute original/linked proof from authenticated raw inputs, not review JSON.
            for label, root in roots.items():
                original = root/'data/2.6.2/main.elf'
                csv_path = root/'data/2.6.2/data_symbols.csv'
                if c.sha(original) != binding['original_sha256']:
                    raise c.Halt('Vtable original input changed')
                ledger = validate_data(csv_path, original, root/'build/2.6.2/d3', proof['nss'])
                if {(x['address'],x['name']):x['validation'] for x in ledger['rows']} != retained[label]:
                    raise c.Halt('Retained data proof does not reproduce')
                if label == 'candidate':
                    witnesses = recovery_data.prove(mapping, ledger, root/'build/2.6.2/d3', binding['owner'], owned['candidate'][0])
                    named_witnesses = named_data_proof(mapping, ledger, roots, binding,
                        original, proof['nss'], owned['candidate'][0])
        transitions = recovery_data.compare_outcomes(retained['baseline'], retained['candidate'], witnesses + named_witnesses)
        if witnesses != proof['witnesses'] or witness_transitions(transitions, witnesses) != proof['transitions']:
            raise c.Halt('New-vtable data witness omitted or changed')
        named_proof = ({'witnesses': named_witnesses,
                        'transitions': witness_transitions(transitions, named_witnesses)}
                       if named_witnesses else None)
        expected_named = ({'evidence': named_proof, 'evidence_sha256': c.digest(named_proof)}
                          if named_proof is not None else None)
        if audit['obligations'].get(mode+':new-named-data-proof') != expected_named:
            raise c.Halt('Named initialized data proof omitted, changed or invented')
    folder = Path(__file__).parent
    if any(c.sha(r.regular(folder/name)) != sha for name,sha in binding['recovery_tools'].items()):
        raise c.Halt('Trusted recovery tooling changed; rebuild evidence')
    return audit


def review(output, path, expected):
    output=Path(output).resolve()
    r.verify(output, policy='recovery-only')
    if not r.HEX.fullmatch(expected or '') or c.sha(r.regular(path)) != expected:
        raise c.Halt('Independent review pin mismatch')
    value = r.read_json(path)
    audit = r.read_json(output/'recovery-audit.json')
    template = review_template(output, audit)
    if set(value) != set(template) or any(value[k] != template[k] for k in ('kind','binding_sha256','audit_sha256','evidence_sha256')):
        raise c.Halt('Review source/evidence binding mismatch')
    if not isinstance(value['reviewer'],str) or not value['reviewer'].strip() or value['independent'] is not True or value['blocking_findings'] != []:
        raise c.Halt('Independent review missing or blocking findings unresolved')
    if value['obligations'].keys() != template['obligations'].keys(): raise c.Halt('Review obligation omitted')
    for key, row in value['obligations'].items():
        if (set(row) != set(template['obligations'][key]) or row['evidence_sha256'] != template['obligations'][key]['evidence_sha256'] or
                row['decision'] not in ('verified','safe-uncertified') or
                any(not isinstance(row[k],str) or not row[k].strip() for k in ('rationale','evidence_reference'))):
            raise c.Halt('Unreviewed or unsafe obligation: '+key)
    return {'status':'recovery_review_verified', 'strict_certified':False, 'reviewer':value['reviewer'], 'review_sha256':expected}


def land(args, main):
    """Hold host-wide build capacity through validation and fast-forward."""
    import resource_admission
    import docker_admission
    try:
        with resource_admission.lease():
            resource_admission.check(str((Path(main).resolve() / 'build' / 'recovery-land')))
            with docker_admission.session('recovery integration'):
                return _land(args, main)
    except resource_admission.AdmissionError as error:
        raise c.Halt(str(error)) from error


def _land(args, main):
    """Prepare and validate an integration commit away from the checked-out branch.

    Caller holds the existing land.lock. No reset, stash or worktree deletion.
    Git's fast-forward checkout checks protect user edits; failed attempts survive.
    """
    import shutil
    import subprocess
    import uuid
    main=Path(main).resolve(); output=Path(args.recovery_receipts).resolve()
    evidence=r.read_json(output/'evidence.json'); binding=r.read_json(output/'binding.json')
    if binding.get('policy') != 'recovery-only' or binding.get('post_landing'):
        raise c.Halt('Pre-landing recovery receipts required')
    if args.files != [binding['owner']]: raise c.Halt('Name exactly the reviewed recovery owner')
    approved=review(output,args.review,args.review_sha256)
    base=binding['versions']['baseline']['commit']; candidate=binding['versions']['candidate']['commit']
    def git(*words):return r.git(main,*words).decode().strip()
    branch = git('rev-parse','--symbolic-full-name','HEAD')
    def unchanged_main():
        if git('rev-parse','--symbolic-full-name','HEAD')!=branch or git('rev-parse','HEAD')!=base or git('status','--porcelain'):
            raise c.Halt('Primary HEAD/work changed; preserve evidence and rebase/review')
    unchanged_main()
    r.verify(output,main,candidate,policy='recovery-only')
    import claim
    claim.check_coordination(main)
    with claim.locked() as claims:
        original_claim = dict(claims[binding['owner']]) if binding['owner'] in claims else None
    # A claimed owner must still point at the exact reviewed clean candidate.
    if binding['owner'] in claim.load():
        wt=claim.worktree(binding['owner'])
        if r.git(wt,'rev-parse','HEAD').decode().strip()!=candidate or r.git(wt,'status','--porcelain').strip():
            raise c.Halt('Claim source changed after review')
    stage=main/'build/recovery-land'/uuid.uuid4().hex
    stage.mkdir(parents=True)
    checkout=stage/'integration'
    targets=stage/'targets.json';c.write(targets,binding['targets'],True)
    # Revalidate live input pins/toolchain recipe before any source mutation.
    words=['build','--mode','recovery-only','--repo',str(main),'--baseline',base,'--candidate',candidate,
           '--owner',binding['owner'],'--original-elf',str(args.original_elf),'--original-sha256',binding['original_sha256'],
           '--checker',str(args.checker),'--checker-sha256',binding['checker_sha256'],
           '--lua-cache',str(args.lua_cache),'--lua-receipt-sha256',binding['lua_receipt_sha256'],
           '--nss',str(args.nss),'--nss-sha256',binding['nss_sha256'],'--targets',str(targets),
           '--output',str(stage/'post-receipts'),'--seconds',str(args.seconds)]
    if 'named_data_dwarfdump' in binding:
        reader = binding['named_data_dwarfdump']
        words.extend(['--dwarfdump', reader['path'], '--dwarfdump-sha256', reader['sha256']])
    build_args=r.parser().parse_args(words)
    current=r.context(build_args);configure(build_args,current)
    if current != {k:v for k,v in binding.items() if k!='images'} or r.image_ids()!=binding['images']:
        raise c.Halt('Live input/checker/config/toolchain lineage changed')
    r.git(main,'worktree','add','--detach',str(checkout),base)
    patch=r.git(main,'diff','--binary',base,candidate)
    (stage/'reviewed.patch').write_bytes(patch)
    shutil.copy2(args.review,stage/'independent-review.json')
    subprocess.run(['git','-C',str(checkout),'apply','--index',str(stage/'reviewed.patch')],check=True)
    before=rows(main,base); after=[dict(x) for x in before]
    selected={(x['address'],x['name']) for x in binding['targets']['functions']}
    for row in after:
        if (row['Address'],row['Name']) in selected and row['Quality']=='U':row['Quality']='M'
    inventory(before,after,binding['targets']['functions'],True)
    table=checkout/'data/2.6.2/functions.csv'
    with table.open('w',newline='') as stream:
        writer=csv.DictWriter(stream,fieldnames=list(before[0]),lineterminator='\n');writer.writeheader();writer.writerows(after)
    r.git(checkout,'add','-u','--','data/2.6.2/functions.csv')
    r.git(checkout,'commit','-m','Recover '+binding['owner']+' as uncertified source',
          '-m','Recovery-only; no exact-match credit. Review SHA256: '+args.review_sha256)
    integrated=r.git(checkout,'rev-parse','HEAD').decode().strip()
    if r.committed_files(main,integrated,r.SOURCE)!=binding['versions']['candidate']['source']:
        raise c.Halt('Integration source differs from reviewed source')
    c.write(stage/'transaction.json',{'kind':KIND,'baseline':base,'reviewed_candidate':candidate,
            'integration':integrated,'review':approved,'state':'postintegration_validation_pending'},True)
    build_args.candidate=integrated;build_args.post_landing=True;build_args.run=True
    if getattr(args, 'reuse_reviewed_baseline', False):
        build_args.reuse_baseline_from = output
    r.build(build_args)
    post=stage/'post-receipts';r.verify(post,main,integrated,policy='recovery-only')
    # Fresh final links and every retained object must reproduce reviewed artifacts.
    final=r.read_json(post/'evidence.json')
    for label in ('baseline','candidate'):
        for mode in ('normal','lua'):
            old=Path(evidence[label][mode]['root']);new=Path(final[label][mode]['root'])
            def artifacts(root):
                return {str(p.relative_to(root)):c.sha(p) for p in root.rglob('*') if p.is_file() and
                    (p.relative_to(root).parts[0]=='objects' or str(p.relative_to(root)) in
                     ('build/2.6.2/d3','build/2.6.2/d3.map','build/2.6.2/d3.map.provenance','result.json'))}
            if artifacts(old)!=artifacts(new):raise c.Halt('Fresh integration artifacts differ; new independent review required')
    review(output,args.review,args.review_sha256)
    with claim.locked() as claims:
        claim.check_coordination(main)
        if claims.get(binding['owner']) != original_claim:
            raise c.Halt('Claim ownership changed during validation')
        if binding['owner'] in claim.load():
            wt=claim.worktree(binding['owner'])
            if r.git(wt,'rev-parse','HEAD').decode().strip()!=candidate or r.git(wt,'status','--porcelain').strip():
                raise c.Halt('Claim changed during validation')
        unchanged_main()
        # Preserve rollback identity before a fast-forward. No destructive rollback on failure.
        r.git(main,'update-ref','refs/recovery-preserved/'+stage.name,base)
        r.git(main,'merge','--ff-only',integrated)
        if git('rev-parse','HEAD')!=integrated:
            raise c.Halt('Unexpected integration HEAD; inspect retained transaction')
    reuse = bool(getattr(args, 'reuse_reviewed_baseline', False))
    profile = ({'fresh_integration_builds': 2, 'reused_baseline_builds': 2} if reuse else {})
    c.write(stage/'landed.json',{'kind':KIND,'status':'recovery_landed','strict_certified':False,
            'commit':integrated,'review_sha256':args.review_sha256,'post_receipts':str(post), **profile},True)
    return {'status':'recovery_landed','strict_certified':False,'commit':integrated,'retained':str(stage), **profile}
