"""Offline bootstrap safety tests; no real software, inputs or services required."""
import contextlib
import hashlib
import io
import os
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile
import unittest
from unittest.mock import patch

import bootstrap
from test_verify_original_inputs import fixture_manifest


class BootstrapProcessTests(unittest.TestCase):
    def test_plain_python_check_and_setup_rejection_create_no_bytecode_or_other_files(self):
        with tempfile.TemporaryDirectory() as temp:
            root = Path(temp)
            tools = root / 'tools'
            tools.mkdir()
            for name in ('bootstrap.py', 'backends.py', 'setup.py', 'verify_original_inputs.py'):
                shutil.copyfile(Path(__file__).with_name(name), tools / name)
            env = os.environ.copy()
            shutil.copytree(bootstrap.ROOT / 'toolchain', root / 'toolchain')
            for name in ('PYTHONDONTWRITEBYTECODE', 'PYTHONPYCACHEPREFIX'):
                env.pop(name, None)
            env.update(PATH='', PYTHONPATH=str(tools))
            before = {path: path.read_bytes() for path in root.rglob('*') if path.is_file()}
            for name, flags in [('bootstrap.py', ['--check']), ('setup.py', [])]:
                run = subprocess.run([sys.executable, str(tools / name), *flags],
                                     env=env, capture_output=True, text=True, timeout=20)
                self.assertEqual(run.returncode, 1, run.stderr)
                self.assertNotIn('Traceback', run.stderr)
            self.assertEqual(before, {path: path.read_bytes() for path in root.rglob('*') if path.is_file()})


class BootstrapTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.base = Path(self.temp.name)
        self.root = self.base / 'source'
        (self.root / 'tools').mkdir(parents=True)
        self.nso = self.root / 'build/exefs/main'
        self.nso.parent.mkdir(parents=True)
        self.nso.write_bytes(b'own input fixture')
        (self.root / 'tools/setup.py').write_text('NSO_SHA256 = ' + repr(bootstrap.sha256(self.nso)))
        self.nss = self.root / 'build/romfs/DiabloIIINX64r.nss'
        self.nss.parent.mkdir(parents=True)
        self.nss.write_bytes(b'\x7fELF\x02\x01' + b'\0' * 12 + b'\xb7\0')
        fixture_manifest(self.root, self.nso, self.nss)
        data = self.root / 'data/2.6.2'
        data.mkdir(parents=True)
        for name in bootstrap.INVENTORIES:
            (data / name).write_text('fixture\n')
        self.addCleanup(patch.stopall)
        patch.object(bootstrap, 'check_backends', return_value=[]).start()
        patch.dict(os.environ, {'CLANG_VERSION': '5.0.1', 'D3_DOCKER_PLATFORM': 'linux/arm64', 'LLVM_ARCH': ''}).start()
        patch.object(bootstrap.platform, 'system', return_value='Linux').start()
        patch.object(bootstrap.platform, 'machine', return_value='aarch64').start()
        self.git = patch.object(bootstrap, 'git', side_effect=lambda root, *a: 'a' * 40 if a == ('rev-parse', 'HEAD') else '').start()
        self.read = patch.object(bootstrap, 'read_command', return_value='available').start()
        patch.object(bootstrap.shutil, 'which', side_effect=lambda name: '/fixture/' + name).start()
        self.run = patch.object(bootstrap.subprocess, 'run', return_value=subprocess.CompletedProcess([], 0)).start()
        self.stdout, self.stderr = io.StringIO(), io.StringIO()

    def main(self, *args):
        with contextlib.redirect_stdout(self.stdout), contextlib.redirect_stderr(self.stderr):
            return bootstrap.main(list(args), self.root)

    def manifest(self):
        return {str(p.relative_to(self.root)): hashlib.sha256(p.read_bytes()).hexdigest()
                for p in self.root.rglob('*') if p.is_file()}

    def test_default_check_is_read_only_and_runs_no_setup(self):
        before = self.manifest()
        self.assertEqual(self.main(), 0)
        self.assertEqual(self.manifest(), before)
        self.run.assert_not_called()
        self.assertEqual(self.read.call_args.args[0], ['docker', 'compose', 'version'])
        self.assertIn('not setup', self.stdout.getvalue())

    def test_all_missing_inputs_are_reported_without_download(self):
        self.nso.unlink()
        self.nss.unlink()
        self.assertEqual(self.main(), 1)
        self.assertIn('exefs/main: missing', self.stderr.getvalue())
        self.assertIn('romfs/DiabloIIINX64r.nss: missing', self.stderr.getvalue())
        self.run.assert_not_called()

    def test_wrong_nso_rejected(self):
        self.nso.write_bytes(b'wrong version')
        self.assertEqual(self.main(), 1)
        self.assertIn('wrong_size', self.stderr.getvalue())

    def test_wrong_nss_identity_rejected(self):
        self.nss.write_bytes(b'not ELF')
        self.assertEqual(self.main(), 1)
        self.assertIn('romfs/DiabloIIINX64r.nss: wrong_size', self.stderr.getvalue())

    def test_hash_display_is_opt_in_but_both_inputs_are_always_verified(self):
        self.assertEqual(self.main('--hash-inputs'), 0)
        self.assertIn(bootstrap.sha256(self.nss), self.stdout.getvalue())
        self.assertIn('not ownership/redistribution rights', self.stdout.getvalue())

    def test_same_size_nss_corruption_blocks_before_runner(self):
        self.nss.write_bytes(b'X' * self.nss.stat().st_size)
        self.assertEqual(self.main('--run-reproduce', '--allow-downloads', *self.plan_args()), 1)
        self.assertIn('wrong_hash', self.stderr.getvalue())
        self.run.assert_not_called()

    def test_x86_requires_explicit_compose_platform(self):
        with patch.object(bootstrap.platform, 'machine', return_value='x86_64'):
            self.assertEqual(self.main(), 1)
            self.assertIn('D3_DOCKER_PLATFORM=linux/amd64', self.stderr.getvalue())
            with patch.dict(os.environ, {'D3_DOCKER_PLATFORM': 'linux/amd64'}):
                self.assertEqual(self.main(), 0)

    def test_missing_tool_is_actionable(self):
        with patch.object(bootstrap.shutil, 'which', side_effect=lambda n: None if n == 'cargo' else '/fixture/' + n):
            self.assertEqual(self.main(), 1)
        self.assertIn('cargo: missing from PATH', self.stderr.getvalue())

    def test_daemon_is_only_explicit_read_only_probe(self):
        self.assertEqual(self.main('--check-docker'), 0)
        self.assertEqual(self.read.call_args.args[0], ['docker', 'info', '--format', '{{.OSType}}'])
        self.run.assert_not_called()

    def test_probe_failure_blocks(self):
        self.read.side_effect = RuntimeError('unavailable')
        self.assertEqual(self.main('--check-docker'), 1)
        self.run.assert_not_called()

    def plan_args(self):
        return ['--work-dir', str(self.base / 'new checkout'), '--output', str(self.base / 'new report.json')]

    def test_dry_run_makes_no_files_and_no_runner_call(self):
        before = sorted(str(p) for p in self.base.rglob('*'))
        self.assertEqual(self.main('--dry-run', *self.plan_args()), 0)
        self.assertEqual(sorted(str(p) for p in self.base.rglob('*')), before)
        self.assertIn('reproduce.py', self.stdout.getvalue())
        self.run.assert_not_called()

    def test_run_requires_download_acknowledgment_before_probes(self):
        self.assertEqual(self.main('--run-reproduce', *self.plan_args()), 1)
        self.git.assert_not_called()
        self.run.assert_not_called()

    def test_explicit_run_delegates_once_with_argv_and_propagates_failure(self):
        self.run.return_value = subprocess.CompletedProcess([], 7)
        self.assertEqual(self.main('--run-reproduce', '--allow-downloads', *self.plan_args()), 7)
        self.run.assert_called_once()
        command = self.run.call_args.args[0]
        self.assertIn(str((self.base / 'new checkout').resolve()), command)
        self.assertEqual(command[-1], '--run')
        self.assertNotIn('shell', self.run.call_args.kwargs)

    def test_failed_prerequisites_prevent_explicit_runner(self):
        self.nso.unlink()
        self.assertEqual(self.main('--run-reproduce', '--allow-downloads', *self.plan_args()), 1)
        self.run.assert_not_called()

    def test_existing_report_or_checkout_is_preserved(self):
        target = self.base / 'new checkout'
        target.mkdir()
        (target / 'keep').write_text('work')
        self.assertEqual(self.main('--dry-run', *self.plan_args()), 1)
        self.assertEqual((target / 'keep').read_text(), 'work')
        self.run.assert_not_called()

    def test_report_log_reuse_is_rejected(self):
        (self.base / 'new report.json.logs').mkdir()
        self.assertEqual(self.main('--dry-run', *self.plan_args()), 1)

    def test_nested_or_inside_checkout_output_is_rejected(self):
        self.assertEqual(self.main('--dry-run', '--work-dir', str(self.root / 'new'), '--output', str(self.base / 'r.json')), 1)
        target = self.base / 'outside'
        self.assertEqual(self.main('--dry-run', '--work-dir', str(target), '--output', str(target / 'r.json')), 1)

    def test_submodule_missing_not_confused_with_parent_repository(self):
        self.git.side_effect = lambda root, *a: '160000 commit ' + 'b' * 40 + '\ttools/common' if a == ('ls-tree', '-r', 'HEAD') else ''
        errors = []
        bootstrap.check_submodules(self.root, errors)
        self.assertIn('uninitialized', errors[0])

    def test_submodule_wrong_revision_is_rejected_without_reset(self):
        child = self.root / 'tools/common'
        child.mkdir()
        (child / '.git').write_text('fixture')
        self.git.side_effect = lambda root, *a: ('160000 commit ' + 'b' * 40 + '\ttools/common') if root == self.root else 'c' * 40
        errors = []
        bootstrap.check_submodules(self.root, errors)
        self.assertIn('wrong commit', errors[0])
        self.assertEqual((child / '.git').read_text(), 'fixture')

    def test_compiler_override_cannot_silently_change_policy(self):
        with patch.dict(os.environ, {'CLANG_VERSION': '17'}):
            self.assertEqual(self.main(), 1)
        with patch.dict(os.environ, {'LLVM_ARCH': 'wrong'}):
            self.assertEqual(self.main(), 1)


if __name__ == '__main__':
    unittest.main()
