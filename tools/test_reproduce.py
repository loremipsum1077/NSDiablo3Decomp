#!/usr/bin/env python3
"""Synthetic, offline tests for isolated reproducibility evidence."""
import json
from pathlib import Path
import subprocess
import tempfile
import unittest
from unittest.mock import patch

import reproduce


class ReproduceTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.base = Path(self.temp.name)
        self.root = self.base / 'source'
        self.root.mkdir()
        subprocess.run(['git', 'init', '-q', str(self.root)], check=True)
        for key, value in [('user.name', 'Test'), ('user.email', 'test@example.invalid')]:
            self.git('config', key, value)
        data = self.root / 'data/2.6.2'
        data.mkdir(parents=True)
        (data / 'functions.csv').write_text('Address,Quality,Size,Name\n0x1,O,4,fn\n')
        for name in reproduce.INVENTORIES[1:]:
            (data / name).write_text('Name,Value\nfixture,1\n')
        (self.root / 'source.txt').write_text('committed\n')
        self.git('add', '.')
        self.git('commit', '-qm', 'baseline')
        self.output = self.base / 'report.json'
        self.nso = self.base / 'main'
        self.nss = self.base / 'main.nss'
        self.nso.write_bytes(b'raw-nso')
        self.nss.write_bytes(b'raw-nss')
        self.args = ['--output', str(self.output), '--nso', str(self.nso), '--nss', str(self.nss)]

    def git(self, *args):
        return reproduce.git(self.root, *args)

    def report(self):
        return json.loads(self.output.read_text())

    def test_preflight_has_hashes_and_no_checkout(self):
        self.assertEqual(reproduce.main(self.args, self.root), 0)
        report = self.report()
        self.assertEqual(report['status'], 'preflight')
        self.assertEqual(report['inputs']['nso']['sha256'], reproduce.sha256(self.nso))
        self.assertNotIn('checkout', report)
        self.assertEqual(report['commands'], [])

    def test_snapshot_excludes_dirty_and_ignored_artifacts(self):
        (self.root / 'source.txt').write_text('dirty')
        artifact = self.root / 'build/2.6.2/d3'
        artifact.parent.mkdir(parents=True)
        artifact.write_text('old artifact')
        evidence = reproduce.Evidence(self.output)
        target = self.base / 'clone'
        reproduce.clone_tree(self.root, target, self.git('rev-parse', 'HEAD'), evidence)
        self.assertEqual((target / 'source.txt').read_text(), 'committed\n')
        self.assertFalse((target / 'build').exists())
        self.assertEqual((self.root / 'source.txt').read_text(), 'dirty')

    def test_recursive_submodules_use_gitlink_revisions_not_dirty_worktrees(self):
        vendor = self.root / 'tools/common'
        nested = vendor / 'asm-differ'
        for directory in (vendor, nested):
            directory.mkdir(parents=True)
            subprocess.run(['git', 'init', '-q', str(directory)], check=True)
            for key, value in [('user.name', 'Test'), ('user.email', 'test@example.invalid')]:
                reproduce.git(directory, 'config', key, value)
            (directory / 'code.txt').write_text('pinned source')
            reproduce.git(directory, 'add', 'code.txt')
            reproduce.git(directory, 'commit', '-qm', 'pinned source')
        nested_revision = reproduce.git(nested, 'rev-parse', 'HEAD')
        reproduce.git(vendor, 'update-index', '--add', '--cacheinfo',
                      '160000,' + nested_revision + ',asm-differ')
        reproduce.git(vendor, 'commit', '-qm', 'pin nested dependency')
        vendor_revision = reproduce.git(vendor, 'rev-parse', 'HEAD')
        self.git('update-index', '--add', '--cacheinfo',
                 '160000,' + vendor_revision + ',tools/common')
        self.git('commit', '-qm', 'pin vendor')
        for directory in (vendor, nested):
            (directory / 'code.txt').write_text('newer committed source')
            reproduce.git(directory, 'add', 'code.txt')
            reproduce.git(directory, 'commit', '-qm', 'newer source must be excluded')
            (directory / 'code.txt').write_text('local patched source')
            (directory / 'cached-checker').write_text('cached binary')
        evidence = reproduce.Evidence(self.output)
        target = self.base / 'recursive-clone'
        reproduce.clone_tree(self.root, target, self.git('rev-parse', 'HEAD'), evidence)
        for relative, revision in [('tools/common', vendor_revision),
                                   ('tools/common/asm-differ', nested_revision)]:
            cloned = target / relative
            self.assertEqual(reproduce.git(cloned, 'rev-parse', 'HEAD'), revision)
            self.assertEqual((cloned / 'code.txt').read_text(), 'pinned source')
            self.assertFalse((cloned / 'cached-checker').exists())
            self.assertEqual((self.root / relative / 'code.txt').read_text(), 'local patched source')
        self.assertEqual(len(evidence.data['repositories']), 3)

    def test_missing_committed_inventory_fails_before_setup(self):
        missing = 'data/2.6.2/data_symbols.csv'
        self.git('rm', missing)
        self.git('commit', '-qm', 'missing required inventory')
        # A live untracked replacement must not conceal a missing committed input.
        (self.root / missing).write_text('Name,Value\nlive,1\n')
        with patch.object(reproduce.Evidence, 'run', autospec=True, side_effect=REAL_RUN) as run:
            self.assertEqual(reproduce.main(self.args + ['--run', '--work-dir',
                             str(self.base / 'missing-inventory')], self.root), 1)
        self.assertIn('missing required inventories', self.report()['error'])
        self.assertIn('data_symbols.csv', self.report()['error'])
        self.assertFalse(any(call.args[1] == 'setup' for call in run.call_args_list))
        self.assertEqual((self.root / missing).read_text(), 'Name,Value\nlive,1\n')

    def test_fresh_venv_metadata_uses_snapshot_interpreter_after_setup(self):
        target = (self.base / 'fresh-venv').resolve()
        events = []
        def fake_run(evidence, stage, command, cwd, env=None):
            if stage == 'snapshot':
                return REAL_RUN(evidence, stage, command, cwd, env)
            evidence.data['stage'] = stage
            events.append(stage)
            if stage == 'setup':
                self.assertNotIn('NX_DECOMP_TOOLS_IN_VENV', env)
                python = Path(cwd) / 'tools/common/.venv/bin/python'
                python.parent.mkdir(parents=True)
                python.write_text('synthetic venv interpreter')
            if stage == 'build':
                raise RuntimeError('stop after setup metadata')
        def fake_probe(command, cwd):
            if command[1:] == ['-m', 'pip', 'freeze']:
                events.append(command[0])
            return {'argv': command, 'returncode': 0, 'stdout': 'fixture==1'}
        with patch.object(reproduce.Evidence, 'run', fake_run), patch.object(reproduce, 'probe', fake_probe):
            self.assertEqual(reproduce.main(self.args + ['--run', '--work-dir', str(target)], self.root), 1)
        metadata = self.report()['effective_python_packages']
        effective_python = str(target / 'tools/common/.venv/bin/python')
        self.assertEqual(metadata['argv'], [effective_python, '-m', 'pip', 'freeze'])
        self.assertLess(events.index('setup'), events.index(effective_python))
        self.assertLess(events.index(effective_python), events.index('build'))
        self.assertNotEqual(self.report()['python_packages']['argv'][0], effective_python)

    def test_missing_input_reports_preflight_failure(self):
        self.nss.unlink()
        self.assertEqual(reproduce.main(self.args, self.root), 1)
        self.assertEqual(self.report()['failure_stage'], 'preflight')

    def test_existing_checkout_is_never_reused(self):
        self.assertEqual(reproduce.main(self.args + ['--run', '--work-dir', str(self.root)], self.root), 1)
        self.assertEqual((self.root / 'source.txt').read_text(), 'committed\n')

    def test_failure_durable_and_later_stages_skipped(self):
        target = self.base / 'run'
        with patch.object(reproduce.Evidence, 'run', autospec=True) as run:
            def fake(evidence, stage, command, cwd, env=None):
                if stage == 'setup':
                    evidence.data['stage'] = stage
                    raise RuntimeError('synthetic setup failure')
                return original(evidence, stage, command, cwd, env)
            original = REAL_RUN
            run.side_effect = fake
            self.assertEqual(reproduce.main(self.args + ['--run', '--work-dir', str(target)], self.root), 1)
            self.assertEqual(self.report()['failure_stage'], 'setup')
            self.assertEqual((target / 'build/exefs/main').read_bytes(), b'raw-nso')
            self.assertFalse(any(call.args[1] == 'build' for call in run.call_args_list))
        self.assertTrue(self.report()['live_inventory_comparison']['functions.csv']['equal'])

    def test_inventory_detects_quality_and_duplicate_changes(self):
        before = reproduce.inventory(self.root)
        path = self.root / 'data/2.6.2/functions.csv'
        path.write_text('Address,Quality,Size,Name\n0x1,U,4,fn\n')
        self.assertFalse(reproduce.compare_inventories(before, reproduce.inventory(self.root))['functions.csv']['equal'])
        path.write_text('Address,Quality,Size,Name\n0x1,O,4,fn\n0x1,O,4,fn\n')
        self.assertFalse(reproduce.compare_inventories(before, reproduce.inventory(self.root))['functions.csv']['equal'])

    def test_live_mutation_overrides_success_exit(self):
        original = reproduce.inventory
        calls = 0
        def changed(root):
            nonlocal calls
            calls += 1
            if calls == 2:
                (self.root / 'data/2.6.2/functions.csv').write_text('changed')
            return original(root)
        with patch.object(reproduce, 'inventory', changed):
            self.assertEqual(reproduce.main(self.args, self.root), 1)
        self.assertEqual(self.report()['failure_stage'], 'live-inventory')

    def test_command_failure_keeps_log_and_exit_code(self):
        evidence = reproduce.Evidence(self.output)
        with self.assertRaises(RuntimeError):
            evidence.run('test', [reproduce.sys.executable, '-c', 'print("diagnostic"); raise SystemExit(7)'], self.root)
        report = self.report()
        self.assertEqual(report['commands'][0]['returncode'], 7)
        self.assertIn('diagnostic', Path(report['commands'][0]['log']).read_text())

    def test_success_requires_all_stages_and_artifacts(self):
        def fake(evidence, stage, command, cwd, env=None):
            if stage == 'snapshot':
                return REAL_RUN(evidence, stage, command, cwd, env)
            evidence.data['stage'] = stage
            evidence.data.setdefault('test_stages', []).append(stage)
            if stage == 'build':
                for relative in ('tools/check', 'build/2.6.2/d3', 'build/2.6.2/d3.map', 'build/2.6.2/d3.map.provenance'):
                    path = Path(cwd) / relative
                    path.parent.mkdir(parents=True, exist_ok=True)
                    path.write_text('fresh')
            if stage == 'regression':
                Path(command[-1]).write_text('{"failures": []}')
        with patch.object(reproduce.Evidence, 'run', fake), patch.object(reproduce, 'probe', return_value={}):
            self.assertEqual(reproduce.main(self.args + ['--run'], self.root), 0)
        report = self.report()
        self.assertEqual(report['status'], 'passed')
        self.assertEqual(report['test_stages'], ['setup', 'build', 'regression'])
        self.assertIn('build/2.6.2/d3', report['artifacts_sha256'])
        # main intentionally retains real runs; remove this synthetic checkout.
        import shutil
        shutil.rmtree(Path(report['checkout']).parent)


REAL_RUN = reproduce.Evidence.run

if __name__ == '__main__':
    unittest.main()
