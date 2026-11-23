#!/usr/bin/env python3
"""Emit a shareable, allowlisted local candidate report; never build or land code.

Run this from a trusted tooling checkout. Candidate repository contents are read
as Git data, never imported/executed. Explicit trusted checker + artifact inputs
permit deterministic checks. This is not a hardened service for hostile ELFs.
"""
import argparse
from collections import Counter
import csv
import hashlib
import io
import json
import os
from pathlib import Path
import re
import shutil
import subprocess
import sys
import tempfile
import time

import bootstrap
import contributor_adapter as adapter
from data_provenance import verify_link_provenance
from validation import integration

ROOT = Path(__file__).resolve().parents[1]
STATUSES = {'ok', 'mismatch', 'not-checkable', 'missing', 'error'}


class CheckError(Exception):
    """Only fixed, public reason codes may cross the report boundary."""


def git(repo, *args, input=None):
    env = os.environ.copy()
    env['GIT_OPTIONAL_LOCKS'] = '0'
    env['GIT_NO_REPLACE_OBJECTS'] = '1'
    env['GIT_NO_LAZY_FETCH'] = '1'
    p = subprocess.run(['git', '-c', 'core.fsmonitor=false', '-C', str(repo), *args], capture_output=True,
                       env=env, timeout=30, input=input)
    if p.returncode:
        raise CheckError('git_read_failed')
    return p.stdout


def commit(repo, ref):
    # End option parsing; no user ref is shell-expanded.
    value = git(repo, 'rev-parse', '--verify', '--end-of-options', ref + '^{commit}').decode().strip()
    if not re.fullmatch('[0-9a-f]{40,64}', value):
        raise CheckError('invalid_commit_identity')
    return value


def blob(repo, revision, path):
    return git(repo, 'show', revision + ':' + path)


def rows(content):
    return list(csv.DictReader(io.StringIO(content.decode('utf-8'))))


def gate(status, reason, **values):
    return dict(status=status, reason=reason, **values)


def request_for(repo, revision, owner):
    functions = rows(blob(repo, revision, 'data/2.6.2/functions.csv'))
    owners = rows(blob(repo, revision, 'data/2.6.2/function_files.csv'))
    try:
        integration.identities(functions, 'Address', 'Name')
    except ValueError as error:
        raise CheckError('invalid_inventory_identities') from error
    addresses = {r['Address'] for r in owners if r['File'] == owner}
    owner_counts = Counter(r['Address'] for r in owners)
    if any(owner_counts[address] != 1 for address in addresses):
        raise CheckError('ambiguous_owner_inventory')
    if not addresses:
        raise CheckError('owner_absent_from_committed_inventory')
    if not any(r['Address'] in addresses for r in functions):
        raise CheckError('owner_has_no_function_inventory_members')
    selected = [r for r in functions if r['Quality'] != 'U' or r['Address'] in addresses]
    request = [{'address': r['Address'], 'name': r['Name']} for r in selected]
    return request, {r['Address'] for r in functions if r['Address'] in addresses}


def source_hashes(repo, revision):
    entries = []
    for entry in git(repo, 'ls-tree', '-rz', revision).split(b'\0'):
        if not entry:
            continue
        meta, raw_path = entry.split(b'\t', 1)
        mode, kind, oid = meta.split()
        path = raw_path.decode('utf-8')
        if path != 'CMakeLists.txt' and path.split('/')[0] not in ('src', 'include', 'toolchain'):
            continue
        if kind != b'blob' or mode not in (b'100644', b'100755'):
            raise CheckError('unsupported_source_tree_entry')
        entries.append((path, oid))
    data = git(repo, 'cat-file', '--batch', input=b'\n'.join(oid for _, oid in entries) + b'\n')
    result, offset = {}, 0
    for path, oid in entries:
        end = data.index(b'\n', offset)
        actual, kind, length = data[offset:end].split()
        size = int(length)
        if actual != oid or kind != b'blob':
            raise CheckError('source_blob_identity_mismatch')
        content = data[end + 1:end + 1 + size]
        if len(content) != size or data[end + 1 + size:end + 2 + size] != b'\n':
            raise CheckError('source_blob_truncated')
        result[path] = hashlib.sha256(content).hexdigest()
        offset = end + 2 + size
    return result


def selected_targets(repo, revision, owner, path=None):
    functions = rows(blob(repo, revision, 'data/2.6.2/functions.csv'))
    owners = rows(blob(repo, revision, 'data/2.6.2/function_files.csv'))
    try:
        integration.identities(functions, 'Address', 'Name')
    except ValueError as error:
        raise CheckError('invalid_inventory_identities') from error
    addresses = {r['Address'] for r in owners if r['File'] == owner}
    owner_counts = Counter(r['Address'] for r in owners)
    if any(owner_counts[address] != 1 for address in addresses):
        raise CheckError('ambiguous_owner_inventory')
    members = [r for r in functions if r['Address'] in addresses]
    if path:
        value = adapter.read(path)
        if (not isinstance(value, dict) or set(value) != {'format', 'functions'} or
                type(value['format']) is not int or value['format'] != 1 or not isinstance(value['functions'], list)):
            raise CheckError('invalid_selected_target_manifest')
        try:
            identities = integration.identities(value['functions'])
            if any(set(r) != {'address', 'name'} for r in value['functions']):
                raise ValueError('Invalid target fields')
        except ValueError as error:
            raise CheckError('invalid_selected_target_manifest') from error
        selected = [r for r in members if (r['Address'], r['Name']) in identities]
        if len(selected) != len(identities):
            raise CheckError('selected_targets_outside_owner')
    else:
        selected = members
    if not selected or len(selected) > 64:
        raise CheckError('select_one_to_64_named_owner_targets')
    return members, selected


def linked_summary(request, before, after, targets):
    try:
        known = integration.identities(request)
        required = integration.identities(targets, 'Address', 'Name')
        if not required <= known or len(before) != len(request) or len(after) != len(request):
            raise ValueError('Linked request mismatch')
        policy = integration.comparison(
            {'results': [dict(ident, status=status) for ident, status in zip(request, before)]},
            {'results': [dict(ident, status=status) for ident, status in zip(request, after)]}, targets)
    except (KeyError, TypeError, ValueError) as error:
        raise CheckError('linked_summary_identity_mismatch') from error
    changes = [{'address': ident['address'], 'before': a, 'after': b}
               for ident, a, b in zip(request, before, after) if a != b]
    return gate('fail' if policy['regressions'] else 'partial' if any(policy.values()) else 'pass',
                'linked_byte_relocation_and_prior_identity_preservation', baseline_counts=dict(Counter(before)),
                candidate_counts=dict(Counter(after)), changed_statuses=changes, **policy)


ARTIFACT_FIELDS = ('format', 'elf_sha256', 'map_sha256', 'record_sha256')


def bound_artifacts(actual, expected):
    if any(actual.get(key) != expected.get(key) for key in ARTIFACT_FIELDS):
        raise CheckError('snapshot_artifact_binding_mismatch')


def linked_check(repo, revision, original, artifacts, checker, request,
                 expected_artifacts=None, expected_original=None, expected_checker=None):
    # Follow the existing checker_regression.py snapshot contract. Batch writes
    # quality fields only into this disposable copy, never the source inventory.
    expected_artifacts = expected_artifacts or verify_link_provenance(artifacts / 'd3')
    expected_original = expected_original or bootstrap.sha256(original)
    expected_checker = expected_checker or bootstrap.sha256(checker)
    with tempfile.TemporaryDirectory(prefix='d3-contributor-check-') as tmp:
        snapshot = Path(tmp)
        for relative in ('tools/config.toml', 'data/2.6.2/functions.csv', 'data/2.6.2/data_symbols.csv'):
            p = snapshot / relative
            p.parent.mkdir(parents=True, exist_ok=True)
            p.write_bytes(blob(repo, revision, relative))
        (snapshot / 'src').mkdir()
        target = snapshot / 'build/2.6.2'
        target.mkdir(parents=True)
        for name in ('d3', 'd3.map', 'd3.map.provenance'):
            shutil.copyfile(artifacts / name, target / name)
        shutil.copyfile(original, snapshot / 'data/2.6.2/main.elf')
        copied_checker = snapshot / 'tools/check'
        shutil.copy2(checker, copied_checker)
        bound_artifacts(verify_link_provenance(target / 'd3'), expected_artifacts)
        if (bootstrap.sha256(snapshot / 'data/2.6.2/main.elf') != expected_original or
                bootstrap.sha256(copied_checker) != expected_checker):
            raise CheckError('snapshot_input_or_checker_binding_mismatch')
        (snapshot / 'request.json').write_text(json.dumps({'functions': request}))
        # No candidate scripts/compiler/hooks. The trusted checker gets a minimal
        # environment, never inherited model credentials or provider settings.
        p = subprocess.run([str(copied_checker), '--batch', 'request.json', '--batch-output', 'result.json'],
                           cwd=snapshot, capture_output=True, timeout=600, stdin=subprocess.DEVNULL,
                           env={'PATH': os.defpath, 'LANG': 'C', 'LC_ALL': 'C'})
        if p.returncode or not (snapshot / 'result.json').is_file():
            raise CheckError('checker_failed_or_no_result')
        bound_artifacts(verify_link_provenance(target / 'd3'), expected_artifacts)
        if (bootstrap.sha256(snapshot / 'data/2.6.2/main.elf') != expected_original or
                bootstrap.sha256(copied_checker) != expected_checker):
            raise CheckError('snapshot_inputs_changed_during_check')
        response = json.loads((snapshot / 'result.json').read_text())
        if not isinstance(response, dict) or not isinstance(response.get('results'), list):
            raise CheckError('checker_result_identity_mismatch')
        results = response['results']
        if any(not isinstance(r, dict) for r in results) or response.get('format') != 1 or [
                {'address': r.get('address'), 'name': r.get('name')} for r in results] != request:
            raise CheckError('checker_result_identity_mismatch')
        # Do not export checker reasons, paths, disassembly or arbitrary strings.
        return [r['status'] if r.get('status') in STATUSES else 'error' for r in results]


def data_check(repo, revision, original, nss, artifacts, expected_artifacts=None, expected_inputs=None, details=False):
    # Trusted in-process validator; raw ledger/bytes never enter the report.
    from validate_data import validate_data
    expected_artifacts = expected_artifacts or verify_link_provenance(artifacts / 'd3')
    expected_inputs = expected_inputs or {'original_elf': {'sha256': bootstrap.sha256(original)},
                                           'nss': {'sha256': bootstrap.sha256(nss)}}
    with tempfile.TemporaryDirectory(prefix='d3-contributor-data-') as tmp:
        table = Path(tmp) / 'data_symbols.csv'
        table.write_bytes(blob(repo, revision, 'data/2.6.2/data_symbols.csv'))
        ledger = validate_data(table, original, artifacts / 'd3', nss)
        bound_artifacts(ledger['provenance'], expected_artifacts)
        if (ledger['inputs']['original']['sha256'] != expected_inputs['original_elf']['sha256'] or
                ledger['inputs']['nss']['sha256'] != expected_inputs['nss']['sha256'] or
                ledger['inputs']['rebuilt']['sha256'] != expected_artifacts['elf_sha256']):
            raise CheckError('data_input_binding_mismatch')
        counts = Counter(row['validation']['status'] if row['validation']['status'] in
                         ('verified_equal', 'different', 'unverified') else 'unverified'
                         for row in ledger['rows'])
        result = gate('partial' if counts.get('unverified') or counts.get('different') else 'pass',
                      'recorded_relocations_only_source_ownership_unverified', counts=dict(counts))
        if not details:
            return result
        states = Counter((hashlib.sha256(json.dumps([r['address'], r['name']]).encode()).hexdigest(),
                          r['validation']['status']) for r in ledger['rows'])
        return result, states


def parser():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--repo', type=Path, default=ROOT, help='candidate Git repository (read as data only)')
    p.add_argument('--baseline', required=True)
    p.add_argument('--candidate', default='HEAD')
    p.add_argument('--owner', required=True, help='exact original owner path')
    p.add_argument('--nso', type=Path, required=True)
    p.add_argument('--nss', type=Path, required=True)
    p.add_argument('--original-elf', type=Path, required=True, help='converted own main.elf; verified against setup pin')
    p.add_argument('--output', type=Path, required=True, help='new sanitized JSON report; never overwritten')
    for mode in ('normal', 'lua'):
        for version in ('baseline', 'candidate'):
            p.add_argument('--' + mode + '-' + version, type=Path,
                           help='prebuilt directory containing d3, d3.map and d3.map.provenance')
    p.add_argument('--run-checker', action='store_true', help='CPU-expensive; execute only the explicit trusted checker')
    p.add_argument('--checker', type=Path)
    p.add_argument('--checker-sha256', help='expected independently established trusted checker hash')
    p.add_argument('--run-data', action='store_true', help='CPU/memory-expensive trusted relocation-aware data validation')
    p.add_argument('--targets', type=Path, help='optional bounded address/name subset manifest; whole-owner accounting remains')
    p.add_argument('--evidence', type=Path, help='local adapter manifest referencing existing integration/source receipts')
    p.add_argument('--evidence-sha256', help='expected adapter manifest SHA-256; consistency pin, not approval')
    p.add_argument('--run-owner-gates', action='store_true', help='run existing strict object/extras/unwind validators from bound receipts')
    p.add_argument('--recovery-receipts', type=Path, help='Explicit recovery-only evidence directory; never certification')
    p.add_argument('--review', type=Path)
    p.add_argument('--review-sha256')
    p.add_argument('--run-all', action='store_true', help='explicitly enable checker, data and owner gates; never builds/downloads')
    return p


def make_report(args):
    if args.recovery_receipts:
        import recovery, receipts
        try:
            receipts.verify(args.recovery_receipts, args.repo, commit(args.repo, args.candidate), policy='recovery-only')
            binding = receipts.read_json(args.recovery_receipts / 'binding.json')
            if binding['versions']['baseline']['commit'] != commit(args.repo, args.baseline) or binding['owner'] != args.owner:
                raise ValueError('Recovery report scope mismatch')
            pins = ((args.original_elf, binding['original_sha256']), (args.nss, binding['nss_sha256']),
                    (args.checker, binding['checker_sha256']))
            if any(not path or bootstrap.sha256(path) != sha for path, sha in pins):
                raise ValueError('Recovery report live input mismatch')
            result = recovery.review(args.recovery_receipts, args.review, args.review_sha256) if args.review else {'status':'review_pending'}
            return {'format':1, 'kind':'recovery_only_contributor_report', 'status':'incomplete',
                    'strict_certified':False, 'accepted':False, 'landed':False,
                    'baseline':binding['versions']['baseline']['commit'], 'candidate':binding['versions']['candidate']['commit'],
                    'recovery_review':result, 'postintegration_required':True}
        except (OSError, ValueError, RuntimeError, TypeError, KeyError) as error:
            return {'format':1, 'kind':'recovery_only_contributor_report', 'status':'failed',
                    'strict_certified':False, 'accepted':False, 'landed':False, 'reason':'recovery_evidence_or_review_invalid'}
    if args.run_all:
        args.run_checker = args.run_data = args.run_owner_gates = True
    started = time.monotonic()
    report = {'format': 1, 'kind': 'contributor_supplied_candidate_evidence',
              'status': 'incomplete', 'accepted': False, 'landed': False,
              'baseline': None, 'candidate': None, 'owner_id': None,
              'inputs': {}, 'artifacts': {}, 'checks': {},
              'tools': {'python': '.'.join(map(str, sys.version_info[:3])),
                        'runner_sha256': bootstrap.sha256(Path(__file__)),
                        'support_sha256': {name: bootstrap.sha256(Path(__file__).with_name(name))
                            for name in ('bootstrap.py', 'data_provenance.py', 'validate_data.py',
                                         'data_coverage.py', 'data_relocations.py', 'data_elf.py',
                                         'attribution_binary.py', 'contributor_adapter.py')},
                        'owner_validator_sha256': {name: bootstrap.sha256(Path(__file__).with_name('validation') / name)
                                                   for name in ('strict.py', 'unwind.py', 'integration.py', 'elf.py', 'common.py', 'scoring.py')},
                        'compiler': {'expected_version': '5.0.1', 'actual_version': None,
                                     'status': 'not_verified'},
                        'checker': {'actual_version': None, 'status': 'not_run'},
                        'checker_options': ['--batch', '--batch-output']},
              'gaps': ['independent_source_review_required',
                       'fresh_source_to_artifact_build_lineage_not_verified',
                       'historical_checks_outside_baseline_nonU_and_owner_not_imported',
                       'postintegration_validation_required'],
              'models_called': 0}
    checks = report['checks']
    for mode in ('normal', 'lua'):
        checks[mode] = gate('skipped', 'baseline_and_candidate_artifacts_required')
        checks[mode + '_data'] = gate('skipped', 'explicit_run_data_and_artifacts_required')
    for name, reason in (
            ('object', 'per_file_object_evidence_not_run'),
            ('unwind', 'explicit_owner_unwind_checks_not_run'),
            ('emitted_extras', 'whole_owner_object_and_source_review_required'),
            ('independent_review', 'contributor_report_is_not_independent_approval')):
        checks[name] = gate('skipped', reason)
    try:
        grafts = Path(os.fsdecode(git(args.repo, 'rev-parse', '--path-format=absolute', '--git-path', 'info/grafts')).strip())
        if grafts.exists():
            raise CheckError('legacy_git_grafts_not_supported')
        report['baseline'] = commit(args.repo, args.baseline)
        report['candidate'] = commit(args.repo, args.candidate)
        if git(args.repo, 'status', '--porcelain'):
            report['gaps'].append('working_changes_excluded_committed_candidate_only')
        request, owner_addresses = request_for(args.repo, report['baseline'], args.owner)
        members, targets = selected_targets(args.repo, report['baseline'], args.owner, args.targets)
        if any(not re.fullmatch(r'0x[0-9a-fA-F]+', item['address']) for item in request):
            raise CheckError('invalid_inventory_address')
        report['owner_id'] = 'SRC-' + hashlib.sha256(args.owner.encode()).hexdigest()[:12]
        report['scope'] = {'whole_owner_functions': len(owner_addresses), 'requested_functions': len(request),
                           'request_sha256': hashlib.sha256(json.dumps(request, sort_keys=True).encode()).hexdigest()}
        report['scope']['required_targets'] = len(targets)
        report['scope']['required_targets_sha256'] = hashlib.sha256(json.dumps(targets, sort_keys=True).encode()).hexdigest()
        evidence = None
        if args.evidence:
            evidence = adapter.load(args.evidence, args.evidence_sha256,
                                    {key: report[key] for key in ('baseline', 'candidate')})
            report['adapter_manifest_sha256'] = args.evidence_sha256
            report['tools']['toolchain'] = {name: {'sha256': entry['sha256'], 'recorded_version': entry['version'],
                                                   'version_verified': False} for name, entry in evidence['toolchain'].items()}
            checks['toolchain'] = gate('partial', 'observed_file_hashes_runtime_versions_and_build_lineage_pending',
                                       supplied_tools=len(evidence['toolchain']))
            for version in ('baseline', 'candidate'):
                for mode in ('normal', 'lua'):
                    if getattr(args, mode + '_' + version) is None:
                        setattr(args, mode + '_' + version, evidence[version][mode]['root'] / 'build/2.6.2')
        elif args.run_owner_gates:
            raise CheckError('owner_gates_require_pinned_adapter_evidence')
        # Candidate inventories/config are not allowed to redefine the test oracle.
        for relative in ('tools/config.toml', 'data/2.6.2/functions.csv',
                         'data/2.6.2/function_files.csv', 'data/2.6.2/data_symbols.csv'):
            if blob(args.repo, report['baseline'], relative) != blob(args.repo, report['candidate'], relative):
                raise CheckError('candidate_changed_inventory_or_checker_configuration')
        report['settings_sha256'] = {
            name: hashlib.sha256(blob(args.repo, report['candidate'], name)).hexdigest()
            for name in ('CMakeLists.txt', 'toolchain/ToolchainNX64.cmake', 'toolchain/lua/config.json')}
        changed = git(args.repo, 'diff', '--no-ext-diff', '--no-textconv', '--name-only', '-z',
                      report['baseline'], report['candidate']).split(b'\0')
        report['changed_files_count'] = sum(bool(p) for p in changed)
        report['changed_paths_sha256'] = hashlib.sha256(b'\0'.join(changed)).hexdigest()
        for name, path in (('nso', args.nso), ('nss', args.nss), ('original_elf', args.original_elf)):
            if not path.is_file():
                raise CheckError('required_original_input_missing')
            report['inputs'][name] = {'sha256': bootstrap.sha256(path), 'bytes': path.stat().st_size}
        pins = {}
        # Use this trusted tooling checkout's pins, never candidate Python code.
        import ast
        for node in ast.parse((ROOT / 'tools/setup.py').read_text()).body:
            if isinstance(node, ast.Assign):
                for target in node.targets:
                    if isinstance(target, ast.Name) and target.id in ('NSO_SHA256', 'ELF_SHA256'):
                        pins[target.id] = ast.literal_eval(node.value)
        if report['inputs']['nso']['sha256'] != pins['NSO_SHA256'] or report['inputs']['original_elf']['sha256'] != pins['ELF_SHA256']:
            raise CheckError('original_input_hash_wrong_for_switch_2_6_2')
        report['inputs']['version'] = 'Switch 2.6.2 (NSO/ELF hash pins)'
        report['inputs']['nss_identity'] = 'hash_recorded_version_and_DWARF_not_independently_verified'
        report['gaps'].append('NSS_build_identity_and_DWARF_authenticity_need_review')
        checker = None
        if args.run_checker:
            if not args.checker or not args.checker_sha256 or not re.fullmatch('[0-9a-f]{64}', args.checker_sha256):
                raise CheckError('explicit_trusted_checker_and_expected_sha256_required')
            checker = args.checker.resolve()
            if bootstrap.sha256(checker) != args.checker_sha256:
                raise CheckError('trusted_checker_hash_mismatch')
            report['tools']['checker'] = {'sha256': args.checker_sha256, 'actual_version': None,
                                          'status': 'hash_verified_source_build_lineage_requires_review'}
        for mode in ('normal', 'lua'):
            if any(getattr(args, mode + '_' + version) is None for version in ('baseline', 'candidate')):
                if args.run_data:
                    checks[mode + '_data'] = gate('skipped', 'baseline_and_candidate_artifacts_required')
                continue
            statuses = {}
            available = True
            for version in ('baseline', 'candidate'):
                directory = getattr(args, mode + '_' + version)
                if directory is None:
                    available = False
                    continue
                try:
                    link_evidence = verify_link_provenance(directory / 'd3')
                    report['artifacts'][mode + '_' + version] = {
                        key: value for key, value in link_evidence.items() if key in ARTIFACT_FIELDS}
                    if checker:
                        statuses[version] = linked_check(args.repo, report[version], args.original_elf,
                                                         directory, checker, request,
                                                         report['artifacts'][mode + '_' + version],
                                                         report['inputs']['original_elf']['sha256'],
                                                         args.checker_sha256)
                except Exception:
                    checks[mode] = gate('fail', 'artifact_provenance_or_checker_failed')
                    available = False
            if available and checks[mode]['status'] != 'fail':
                if not checker:
                    checks[mode] = gate('skipped', 'explicit_run_checker_required')
                else:
                    checks[mode] = linked_summary(request, statuses['baseline'], statuses['candidate'], targets)
            if available and args.run_data:
                try:
                    data_results = {
                        version: data_check(args.repo, report[version], args.original_elf, args.nss,
                                            getattr(args, mode + '_' + version),
                                            report['artifacts'][mode + '_' + version], report['inputs'], details=True)
                        for version in ('baseline', 'candidate')}
                    old, new = data_results['baseline'][1], data_results['candidate'][1]
                    lost = sum(count - new.get(key, 0) for key, count in old.items()
                               if key[1] == 'verified_equal' and new.get(key, 0) < count)
                    old_rows, new_rows = Counter(), Counter()
                    for (identity, _), count in old.items():
                        old_rows[identity] += count
                    for (identity, _), count in new.items():
                        new_rows[identity] += count
                    missing = sum((old_rows - new_rows).values())
                    checks[mode + '_data'] = {version: result[0] for version, result in data_results.items()}
                    checks[mode + '_data'].update(status='fail' if lost or missing else 'partial',
                        reason='per_identity_data_preservation_unknown_objects_pending', verified_regressions=lost,
                        missing_inventory_rows=missing)
                except Exception:
                    checks[mode + '_data'] = gate('fail', 'data_validation_failed')
            elif args.run_data:
                checks[mode + '_data'] = gate('skipped', 'artifact_provenance_or_link_check_unavailable')
        if args.run_owner_gates:
            try:
                owner = adapter.owner_gates(evidence, args.owner, members, targets,
                    {version: source_hashes(args.repo, report[version]) for version in ('baseline', 'candidate')},
                    args.original_elf, report['inputs']['original_elf']['sha256'], report['artifacts'],
                    {name: blob(args.repo, report['baseline'], 'data/2.6.2/' + name)
                     for name in ('functions.csv', 'data_symbols.csv')})
                for name in ('object', 'unwind', 'emitted_extras', 'fresh_build_lineage'):
                    checks[name] = owner[name]
                checks['source_snapshot'] = gate('pass', 'bound_source_snapshots_match_exact_commits', versions=owner['source_snapshot'])
                report['owner_artifact_bindings'] = owner['bindings']
            except Exception:
                checks['object'] = gate('fail', 'authenticated_owner_adapter_failed')
                checks['unwind'] = gate('skipped', 'authenticated_owner_inputs_unavailable')
                checks['emitted_extras'] = gate('skipped', 'authenticated_owner_inputs_unavailable')
        for name, path in (('nso', args.nso), ('nss', args.nss), ('original_elf', args.original_elf)):
            if bootstrap.sha256(path) != report['inputs'][name]['sha256']:
                raise CheckError('original_inputs_changed_during_checks')
        if checker and bootstrap.sha256(checker) != args.checker_sha256:
            raise CheckError('trusted_checker_changed_during_checks')
        for mode in ('normal', 'lua'):
            for version in ('baseline', 'candidate'):
                key = mode + '_' + version
                if key in report['artifacts']:
                    current = verify_link_provenance(getattr(args, key) / 'd3')
                    if any(current[field] != digest for field, digest in report['artifacts'][key].items()):
                        raise CheckError('linked_artifacts_changed_during_checks')
        checks['inputs'] = gate('pass', 'NSO_and_ELF_hashes_match_NSS_hash_recorded_only')
    except CheckError as error:
        checks['preflight'] = gate('fail', str(error))
    except Exception:
        # No exception text: it can contain input paths, usernames or secret strings.
        checks['preflight'] = gate('fail', 'input_or_tool_read_failed')
    report['gaps'].extend(name + ':' + result['reason'] for name, result in checks.items()
                          if result['status'] in ('skipped', 'partial', 'fail'))
    if any(result['status'] == 'fail' for result in checks.values()):
        report['status'] = 'failed'
    report['elapsed_cpu_work_wall_seconds'] = round(time.monotonic() - started, 3)
    return report


def main(argv=None):
    args = parser().parse_args(argv)
    # Exclusive creation prevents overwriting originals, prior reports or symlinks.
    try:
        if args.output.exists() or args.output.is_symlink():
            raise OSError('exists')
        args.output.parent.mkdir(parents=True, exist_ok=True)
        with args.output.open('x') as stream:
            report = make_report(args)
            json.dump(report, stream, indent=2, sort_keys=True)
            stream.write('\n')
    except OSError:
        print('Report output unavailable or already exists; choose a new path.', file=sys.stderr)
        return 1
    print('Candidate report: ' + report['status'] + '; not accepted or landed.')
    return 1 if report['status'] == 'failed' else 2


if __name__ == '__main__':
    raise SystemExit(main())
