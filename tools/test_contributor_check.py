"""Synthetic report/privacy and strict-preservation tests; no game bytes or model calls."""
import contextlib
import io
import json
import os
from pathlib import Path
import subprocess
import shutil
import tempfile
import unittest
from unittest.mock import patch

import bootstrap
import contributor_check as cc
from receipt_test_fixtures import linked_cases


class ContributorReportTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.base = Path(self.temp.name)
        self.repo = self.base / 'untrusted source'
        self.repo.mkdir()
        self.git('init', '-q')
        files = {
            '.gitignore': 'build/\n',
            'data/2.6.2/functions.csv': 'Address,Quality,Size,Name\n0x10,O,4,prior\n0x20,U,4,target\n',
            'data/2.6.2/function_files.csv': 'Address,File,Compiler\n0x10,X/Other.cpp,clang\n0x20,X/Owner.cpp,clang\n',
            'data/2.6.2/data_symbols.csv': 'Address,Name\n',
            'tools/config.toml': 'default_version = "2.6.2"\n',
            'CMakeLists.txt': 'fixture settings',
            'toolchain/ToolchainNX64.cmake': 'fixture target',
            'toolchain/lua/config.json': '{}',
            'src/X/Owner.cpp': 'baseline source',
        }
        for name, content in files.items():
            p = self.repo / name
            p.parent.mkdir(parents=True, exist_ok=True)
            p.write_text(content)
        self.git('add', '.')
        self.git('commit', '-qm', 'fixture baseline')
        self.baseline = self.git('rev-parse', 'HEAD').strip()
        (self.repo / 'src/X/Owner.cpp').write_text('candidate source')
        self.git('add', 'src')
        self.git('commit', '-qm', 'fixture candidate')
        self.candidate = self.git('rev-parse', 'HEAD').strip()
        self.nso, self.nss, self.elf = [self.base / n for n in ('own.nso', 'own.nss', 'own.elf')]
        for p in (self.nso, self.nss, self.elf):
            p.write_bytes(b'private original fixture: DO_NOT_EXPORT_BYTES')
        self.trusted = self.base / 'trusted'
        (self.trusted / 'tools').mkdir(parents=True)
        (self.trusted / 'tools/setup.py').write_text(
            'NSO_SHA256 = ' + repr(bootstrap.sha256(self.nso)) + '\nELF_SHA256 = ' + repr(bootstrap.sha256(self.elf)))
        self.addCleanup(patch.stopall)
        patch.object(cc, 'ROOT', self.trusted).start()
        self.checker = self.base / 'checker'
        self.checker.write_bytes(b'trusted checker fixture, never executed')
        self.output = self.base / 'safe.json'
        self.args = ['--repo', str(self.repo), '--baseline', self.baseline, '--candidate', self.candidate,
                     '--owner', 'X/Owner.cpp', '--nso', str(self.nso), '--nss', str(self.nss),
                     '--original-elf', str(self.elf), '--output', str(self.output)]
        self.artifacts = {}
        for mode in ('normal', 'lua'):
            for version in ('baseline', 'candidate'):
                key = mode + '_' + version
                p = self.base / key
                p.mkdir()
                (p / 'd3').write_bytes(('ELF fixture ' + key).encode())
                (p / 'd3.map').write_bytes(b'private map DO_NOT_EXPORT_MAP')
                (p / 'd3.map.provenance').write_text(json.dumps({
                    'format': 1, 'elf_sha256': bootstrap.sha256(p / 'd3'),
                    'map_sha256': bootstrap.sha256(p / 'd3.map')}))
                self.artifacts[key] = p
        self.private = 'API_KEY=DO_NOT_EXPORT_SECRET /Users/private-user/home raw original bytes'

    def git(self, *args):
        return subprocess.check_output(['git', '-C', str(self.repo), '-c', 'user.name=Fixture',
                                        '-c', 'user.email=fixture@example.invalid', '-c', 'commit.gpgsign=false',
                                        *args], text=True, stderr=subprocess.PIPE)

    def report(self, extras=()):
        return cc.make_report(cc.parser().parse_args(self.args + list(extras)))

    def full_args(self):
        extras = ['--run-checker', '--checker', str(self.checker),
                  '--checker-sha256', bootstrap.sha256(self.checker)]
        for name, path in self.artifacts.items():
            extras += ['--' + name.replace('_', '-'), str(path)]
        return extras

    def successful_link(self, repo, revision, *args):
        return ['ok', 'mismatch'] if revision == self.baseline else ['ok', 'ok']

    def test_missing_modes_are_skipped_and_never_accepted(self):
        report = self.report()
        self.assertEqual(report['baseline'], self.baseline)
        self.assertEqual(report['candidate'], self.candidate)
        self.assertEqual(report['status'], 'incomplete')
        self.assertEqual(report['checks']['normal']['status'], 'skipped')
        self.assertEqual(report['checks']['unwind']['status'], 'skipped')
        self.assertFalse(report['accepted'])
        self.assertFalse(report['landed'])
        self.assertEqual(report['models_called'], 0)
        self.assertIn('independent_source_review_required', report['gaps'])
        self.assertNotIn('independent_source_and_targeted_AI_review_required', report['gaps'])
        self.assertIsNone(report['tools']['compiler']['actual_version'])

    def test_shareable_report_has_no_paths_environment_or_original_bytes(self):
        with patch.dict(os.environ, {'SECRET_FIXTURE': self.private}):
            payload = json.dumps(self.report())
        for forbidden in (str(self.base), '/Users/', 'private-user', 'DO_NOT_EXPORT', 'SECRET_FIXTURE'):
            self.assertNotIn(forbidden, payload)
        self.assertIn(bootstrap.sha256(self.nso), payload)

    def test_missing_input_is_failure_and_checks_stay_skipped(self):
        self.nss.unlink()
        report = self.report()
        self.assertEqual(report['status'], 'failed')
        self.assertEqual(report['checks']['preflight']['reason'], 'required_original_input_missing')
        self.assertEqual(report['checks']['normal']['status'], 'skipped')

    def test_wrong_version_fails_before_checker(self):
        self.nso.write_bytes(b'wrong version')
        with patch.object(cc, 'linked_check') as run:
            report = self.report(self.full_args())
        self.assertEqual(report['status'], 'failed')
        run.assert_not_called()

    def test_checker_hash_must_match_before_execution(self):
        extras = self.full_args()
        extras[extras.index('--checker-sha256') + 1] = '0' * 64
        with patch.object(cc, 'linked_check') as run:
            report = self.report(extras)
        self.assertEqual(report['checks']['preflight']['reason'], 'trusted_checker_hash_mismatch')
        run.assert_not_called()

    def test_all_owner_U_targets_are_checked_beside_prior_non_U(self):
        request, owner = cc.request_for(self.repo, self.baseline, 'X/Owner.cpp')
        self.assertEqual([r['address'] for r in request], ['0x10', '0x20'])
        self.assertEqual(owner, {'0x20'})

    def test_incomplete_artifact_pair_skips_expensive_checker(self):
        extras = ['--run-checker', '--checker', str(self.checker),
                  '--checker-sha256', bootstrap.sha256(self.checker),
                  '--normal-candidate', str(self.artifacts['normal_candidate']), '--run-data']
        with patch.object(cc, 'linked_check') as run:
            report = self.report(extras)
        run.assert_not_called()
        self.assertEqual(report['checks']['normal']['status'], 'skipped')
        self.assertEqual(report['checks']['normal_data']['reason'], 'baseline_and_candidate_artifacts_required')

    def test_both_modes_record_gain_without_claiming_completion(self):
        with patch.object(cc, 'linked_check', side_effect=self.successful_link) as run:
            report = self.report(self.full_args())
        self.assertEqual(run.call_count, 4)
        for mode in ('normal', 'lua'):
            self.assertEqual(report['checks'][mode]['status'], 'pass')
            self.assertEqual(report['checks'][mode]['regressions'], 0)
        self.assertEqual(report['status'], 'incomplete')
        self.assertEqual(report['checks']['object']['status'], 'skipped')
        self.assertIn('fresh_source_to_artifact_build_lineage_not_verified', report['gaps'])

    def test_owner_adapter_receives_manifest_not_link_record(self):
        evidence = {'toolchain': {}, 'baseline': {}, 'candidate': {}}
        outcome = {name: cc.gate('partial', 'fixture_pending') for name in
                   ('object', 'unwind', 'emitted_extras', 'fresh_build_lineage')}
        outcome.update(source_snapshot={}, bindings={})
        with patch.object(cc.adapter, 'load', return_value=evidence), \
                patch.object(cc.adapter, 'owner_gates', return_value=outcome) as owner, \
                patch.object(cc, 'linked_check', side_effect=self.successful_link):
            report = self.report(self.full_args() + ['--evidence', str(self.base/'manifest'),
                '--evidence-sha256', '0'*64, '--run-owner-gates'])
        self.assertIs(owner.call_args.args[0], evidence)
        self.assertEqual(report['checks']['object']['status'], 'partial')
        self.assertEqual(report['checks']['independent_review']['status'], 'skipped')

    def test_forged_receipt_cannot_establish_candidate_build_lineage(self):
        for artifacts in self.artifacts.values():
            (artifacts / 'done.json').write_text(json.dumps({
                'source_revision': self.candidate, 'fresh_build': True,
                'accepted': True, 'secret': self.private}))
        with patch.object(cc, 'linked_check', side_effect=self.successful_link):
            report = self.report(self.full_args())
        self.assertEqual(report['status'], 'incomplete')
        self.assertFalse(report['accepted'])
        self.assertIn('fresh_source_to_artifact_build_lineage_not_verified', report['gaps'])
        self.assertNotIn('DO_NOT_EXPORT', json.dumps(report))

    def test_prior_identity_regression_fails(self):
        def run(repo, revision, *args):
            return ['ok', 'mismatch'] if revision == self.baseline else ['mismatch', 'ok']
        with patch.object(cc, 'linked_check', side_effect=run):
            report = self.report(self.full_args())
        self.assertEqual(report['checks']['normal']['status'], 'fail')
        self.assertEqual(report['status'], 'failed')

    def test_unexpected_outside_owner_improvement_fails_preservation(self):
        def run(repo, revision, *args):
            return ['mismatch', 'mismatch'] if revision == self.baseline else ['ok', 'ok']
        with patch.object(cc, 'linked_check', side_effect=run):
            report = self.report(self.full_args())
        self.assertEqual(report['checks']['normal']['regressions'], 1)

    def test_owner_still_mismatching_is_partial(self):
        with patch.object(cc, 'linked_check', return_value=['ok', 'mismatch']):
            report = self.report(self.full_args())
        self.assertEqual(report['checks']['normal']['status'], 'partial')
        self.assertEqual(report['checks']['normal']['owner_not_ok'], 1)

    def test_stale_provenance_fails_without_repair(self):
        path = self.artifacts['normal_candidate'] / 'd3'
        path.write_bytes(b'changed')
        with patch.object(cc, 'linked_check', side_effect=self.successful_link):
            report = self.report(self.full_args())
        self.assertEqual(report['checks']['normal']['status'], 'fail')
        self.assertEqual(path.read_bytes(), b'changed')

    def test_raw_checker_failure_reason_never_exported(self):
        with patch.object(cc, 'linked_check', side_effect=RuntimeError(self.private)):
            report = self.report(self.full_args())
        self.assertEqual(report['checks']['normal']['status'], 'fail')
        self.assertNotIn('DO_NOT_EXPORT', json.dumps(report))
        self.assertNotIn(str(self.base), json.dumps(report))

    def test_data_failure_is_truthful_and_redacted(self):
        with patch.object(cc, 'linked_check', side_effect=self.successful_link), \
                patch.object(cc, 'data_check', side_effect=ValueError(self.private)):
            report = self.report(self.full_args() + ['--run-data'])
        self.assertEqual(report['checks']['normal_data']['status'], 'fail')
        self.assertNotIn('DO_NOT_EXPORT', json.dumps(report))

    def test_input_changed_during_check_invalidates_report(self):
        def run(*args):
            self.nss.write_bytes(b'changed')
            return ['ok', 'ok']
        with patch.object(cc, 'linked_check', side_effect=run):
            report = self.report(self.full_args())
        self.assertEqual(report['checks']['preflight']['reason'], 'original_inputs_changed_during_checks')

    def test_candidate_inventory_tamper_fails(self):
        path = self.repo / 'data/2.6.2/functions.csv'
        path.write_text(path.read_text().replace('0x20,U', '0x20,O'))
        self.git('add', 'data')
        self.git('commit', '-qm', 'invalid quality edit')
        report = self.report(['--candidate', 'HEAD'])
        self.assertEqual(report['checks']['preflight']['reason'], 'candidate_changed_inventory_or_checker_configuration')

    def test_git_replacement_refs_do_not_redefine_reported_commits(self):
        baseline = cc.blob(self.repo, self.baseline, 'CMakeLists.txt')
        (self.repo / 'CMakeLists.txt').write_text('forged replacement settings')
        self.git('add', 'CMakeLists.txt')
        self.git('commit', '-qm', 'replacement fixture')
        replacement = self.git('rev-parse', 'HEAD').strip()
        self.git('replace', self.baseline, replacement)
        self.assertEqual(cc.commit(self.repo, self.baseline), self.baseline)
        self.assertEqual(cc.blob(self.repo, self.baseline, 'CMakeLists.txt'), baseline)
        self.assertEqual(self.report()['baseline'], self.baseline)

    def test_legacy_grafts_fail_closed(self):
        grafts = self.repo / '.git/info/grafts'
        grafts.parent.mkdir(exist_ok=True)
        grafts.write_text(self.baseline + '\n')
        report = self.report()
        self.assertEqual(report['checks']['preflight']['reason'], 'legacy_git_grafts_not_supported')

    def test_swap_during_artifact_copy_cannot_rebind_report_hashes(self):
        request, _ = cc.request_for(self.repo, self.baseline, 'X/Owner.cpp')
        artifacts = self.artifacts['normal_candidate']
        expected = cc.verify_link_provenance(artifacts / 'd3')
        copy = shutil.copyfile
        def swapped(source, destination, *args, **kwargs):
            if Path(source).name == 'd3':
                Path(destination).write_bytes(b'swapped-and-restored artifact')
                return destination
            return copy(source, destination, *args, **kwargs)
        with patch.object(cc.shutil, 'copyfile', side_effect=swapped), patch.object(cc, 'blob', return_value=b'fixture'):
            with self.assertRaises((cc.CheckError, ValueError)):
                cc.linked_check(self.repo, self.baseline, self.elf, artifacts, self.checker, request, expected)
        self.assertEqual(cc.verify_link_provenance(artifacts / 'd3'), expected)

    def test_checker_copy_must_match_explicit_trusted_hash(self):
        request, _ = cc.request_for(self.repo, self.baseline, 'X/Owner.cpp')
        def swapped(source, destination, *args, **kwargs):
            Path(destination).write_bytes(b'changed checker')
        with patch.object(cc.shutil, 'copy2', side_effect=swapped), patch.object(cc, 'blob', return_value=b'fixture'):
            with self.assertRaisesRegex(cc.CheckError, 'snapshot_input_or_checker_binding_mismatch'):
                cc.linked_check(self.repo, self.baseline, self.elf, self.artifacts['normal_candidate'],
                                self.checker, request, expected_checker=bootstrap.sha256(self.checker))

    def test_checker_snapshot_drops_raw_reasons_and_never_changes_repo(self):
        before = (self.repo / 'data/2.6.2/functions.csv').read_bytes()
        request, _ = cc.request_for(self.repo, self.baseline, 'X/Owner.cpp')
        def fake(command, **kwargs):
            snapshot = Path(kwargs['cwd'])
            self.assertEqual(set(kwargs['env']), {'PATH', 'LANG', 'LC_ALL'})
            self.assertEqual(kwargs['stdin'], subprocess.DEVNULL)
            (snapshot / 'data/2.6.2/functions.csv').write_text('mutated snapshot')
            (snapshot / 'result.json').write_text(json.dumps({'format': 1, 'results': [
                {**item, 'status': 'ok', 'reason': self.private} for item in request]}))
            return subprocess.CompletedProcess(command, 0, stdout=self.private.encode(), stderr=self.private.encode())
        # blob() uses Git subprocess too; freeze committed fixture data first.
        contents = {}
        for name in ('tools/config.toml', 'data/2.6.2/functions.csv', 'data/2.6.2/data_symbols.csv'):
            contents[name] = cc.blob(self.repo, self.baseline, name)
        with patch.object(cc, 'blob', side_effect=lambda repo, rev, name: contents[name]), \
                patch.object(cc.subprocess, 'run', side_effect=fake):
            result = cc.linked_check(self.repo, self.baseline, self.elf,
                                     self.artifacts['normal_candidate'], self.checker, request)
        self.assertEqual(result, ['ok', 'ok'])
        self.assertEqual((self.repo / 'data/2.6.2/functions.csv').read_bytes(), before)

    def test_missing_report_identity_fails_closed(self):
        request, _ = cc.request_for(self.repo, self.baseline, 'X/Owner.cpp')
        contents = {name: cc.blob(self.repo, self.baseline, name) for name in
                    ('tools/config.toml', 'data/2.6.2/functions.csv', 'data/2.6.2/data_symbols.csv')}
        def fake(command, **kwargs):
            (Path(kwargs['cwd']) / 'result.json').write_text('{"format":1,"results":[]}')
            return subprocess.CompletedProcess(command, 0)
        with patch.object(cc, 'blob', side_effect=lambda repo, rev, name: contents[name]), \
                patch.object(cc.subprocess, 'run', side_effect=fake):
            with self.assertRaisesRegex(cc.CheckError, 'identity_mismatch'):
                cc.linked_check(self.repo, self.baseline, self.elf,
                                self.artifacts['normal_candidate'], self.checker, request)

    def test_report_uses_shared_selected_scope_cases(self):
        targets_path = self.base / 'selected.json'
        contents = {name: cc.blob(self.repo, self.baseline, name) for name in
                    ('tools/config.toml', 'data/2.6.2/functions.csv', 'data/2.6.2/data_symbols.csv')}
        linked_check = cc.linked_check
        for name, old, new, targets, _, expected in linked_cases():
            with self.subTest(case=name):
                targets_path.write_text(json.dumps({'format': 1, 'functions': targets}))
                def checked(repo, revision, *args, **kwargs):
                    def fake(command, **options):
                        (Path(options['cwd']) / 'result.json').write_text(json.dumps({
                            'format': 1, 'results': old if revision == self.baseline else new}))
                        return subprocess.CompletedProcess(command, 0)
                    with patch.object(cc, 'blob', side_effect=lambda repo, rev, path: contents[path]), \
                            patch.object(cc.subprocess, 'run', side_effect=fake):
                        return linked_check(repo, revision, *args, **kwargs)
                with patch.object(cc, 'linked_check', side_effect=checked):
                    report = self.report(self.full_args() + ['--targets', str(targets_path)])
                self.assertFalse(report['accepted'])
                self.assertEqual(report['checks']['independent_review']['status'], 'skipped')
                if expected == 'invalid':
                    self.assertEqual(report['status'], 'failed')
                    self.assertNotEqual(report['checks']['normal']['status'], 'pass')
                    self.assertNotEqual(report['checks']['lua']['status'], 'pass')
                else:
                    for mode in ('normal', 'lua'):
                        self.assertEqual(report['checks'][mode]['status'], expected)

    def test_subset_does_not_exempt_other_functions_in_same_owner(self):
        functions = cc.blob(self.repo, self.baseline, 'data/2.6.2/functions.csv')
        owners = b'Address,File,Compiler\n0x10,X/Owner.cpp,clang\n0x20,X/Owner.cpp,clang\n'
        path = self.base / 'subset.json'
        path.write_text(json.dumps({'format': 1, 'functions': [{'address': '0x20', 'name': 'target'}]}))
        with patch.object(cc, 'blob', side_effect=[functions, owners]):
            members, targets = cc.selected_targets(self.repo, self.baseline, 'X/Owner.cpp', path)
        self.assertEqual(len(members), 2)
        request = [{'address': r['Address'], 'name': r['Name']} for r in members]
        report = cc.linked_summary(request, ['mismatch', 'mismatch'], ['ok', 'ok'], targets)
        self.assertEqual(report['status'], 'fail')
        self.assertEqual(report['regressions'], 1)

    def test_scope_must_belong_to_unambiguous_owner_inventory(self):
        path = self.base / 'subset.json'
        path.write_text(json.dumps({'format': 1, 'functions': [{'address': '0x10', 'name': 'prior'}]}))
        with self.assertRaisesRegex(cc.CheckError, 'outside_owner'):
            cc.selected_targets(self.repo, self.baseline, 'X/Owner.cpp', path)
        functions = cc.blob(self.repo, self.baseline, 'data/2.6.2/functions.csv')
        owners = cc.blob(self.repo, self.baseline, 'data/2.6.2/function_files.csv')
        for changed_functions, changed_owners, reason in (
                (functions + b'0x20,U,4,target\n', owners, 'inventory_identities'),
                (functions, owners + b'0x20,X/Other.cpp,clang\n', 'ambiguous_owner')):
            with self.subTest(reason=reason), patch.object(cc, 'blob', side_effect=[changed_functions, changed_owners]):
                with self.assertRaisesRegex(cc.CheckError, reason):
                    cc.selected_targets(self.repo, self.baseline, 'X/Owner.cpp')

    def test_data_ledger_exports_only_allowlisted_counts(self):
        ledger = {'rows': [{'validation': {'status': 'verified_equal', 'raw_bytes': self.private}},
                           {'validation': {'status': self.private}, 'name': self.private}],
                  'inputs': {'path': self.private,
                             'original': {'sha256': bootstrap.sha256(self.elf)},
                             'nss': {'sha256': bootstrap.sha256(self.nss)},
                             'rebuilt': {'sha256': bootstrap.sha256(self.artifacts['normal_candidate'] / 'd3')}},
                  'provenance': cc.verify_link_provenance(self.artifacts['normal_candidate'] / 'd3'),
                  'summary': {'secret': self.private}}
        with patch('validate_data.validate_data', return_value=ledger):
            result = cc.data_check(self.repo, self.candidate, self.elf, self.nss,
                                   self.artifacts['normal_candidate'])
        self.assertEqual(result['counts'], {'verified_equal': 1, 'unverified': 1})
        self.assertEqual(result['status'], 'partial')
        self.assertNotIn('DO_NOT_EXPORT', json.dumps(result))

    def test_hostile_filename_and_owner_free_text_never_enter_report(self):
        hostile = self.repo / ('src/DO_NOT_EXPORT_PRIVATE_USER\nPRIVATE_TOKEN.cpp')
        hostile.write_text('fixture')
        self.git('add', 'src')
        self.git('commit', '-qm', 'hostile filename fixture')
        report = self.report(['--candidate', 'HEAD'])
        self.assertNotIn('DO_NOT_EXPORT', json.dumps(report))
        self.assertNotIn('PRIVATE_TOKEN', json.dumps(report))
        report = self.report(['--owner', self.private])
        self.assertEqual(report['status'], 'failed')
        self.assertNotIn('DO_NOT_EXPORT', json.dumps(report))

    def test_data_ledger_hashes_must_bind_to_report_inputs(self):
        artifacts = self.artifacts['normal_candidate']
        ledger = {'rows': [], 'provenance': cc.verify_link_provenance(artifacts / 'd3'),
                  'inputs': {'original': {'sha256': bootstrap.sha256(self.elf)},
                             'nss': {'sha256': bootstrap.sha256(self.nss)},
                             'rebuilt': {'sha256': bootstrap.sha256(artifacts / 'd3')}}}
        for group, field in (('inputs', 'original'), ('inputs', 'nss'), ('inputs', 'rebuilt'),
                             ('provenance', 'record_sha256')):
            with self.subTest(group=group, field=field):
                changed = json.loads(json.dumps(ledger))
                if group == 'inputs':
                    changed[group][field]['sha256'] = '0' * 64
                else:
                    changed[group][field] = '0' * 64
                with patch('validate_data.validate_data', return_value=changed):
                    with self.assertRaises(cc.CheckError):
                        cc.data_check(self.repo, self.candidate, self.elf, self.nss, artifacts)

    def test_candidate_setup_script_is_read_as_data_never_executed(self):
        sentinel = self.base / 'must-not-exist'
        (self.repo / 'tools/setup.py').write_text(
            'from pathlib import Path\nPath(' + repr(str(sentinel)) + ').write_text("unsafe")\n')
        self.git('add', 'tools/setup.py')
        self.git('commit', '-qm', 'hostile candidate script fixture')
        report = self.report(['--candidate', 'HEAD'])
        self.assertEqual(report['status'], 'incomplete')
        self.assertFalse(sentinel.exists())

    def test_output_exclusive_and_exit_is_incomplete(self):
        with contextlib.redirect_stdout(io.StringIO()):
            self.assertEqual(cc.main(self.args), 2)
        before = self.output.read_bytes()
        with contextlib.redirect_stderr(io.StringIO()):
            self.assertEqual(cc.main(self.args), 1)
        self.assertEqual(self.output.read_bytes(), before)

    def test_missing_inputs_emit_sanitized_failure_report(self):
        self.nso.unlink()
        with contextlib.redirect_stdout(io.StringIO()):
            self.assertEqual(cc.main(self.args), 1)
        report = json.loads(self.output.read_text())
        self.assertEqual(report['status'], 'failed')
        self.assertNotIn(str(self.base), self.output.read_text())


if __name__ == '__main__':
    unittest.main()
