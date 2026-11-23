"""Synthetic onboarding/cache checks; no Docker, originals or downloads."""
import json
from contextlib import nullcontext
import os
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile
import unittest
from unittest.mock import patch
import backends as b
from backends import backend
import resource_admission


class BackendTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)
        shutil.copytree(b.ROOT / 'toolchain', self.root / 'toolchain')
        self.cache = self.root / 'old-cache'
        (self.cache / 'out/bin').mkdir(parents=True)
        binary = self.cache / 'out/bin/llc'
        binary.write_bytes(b'\x7fELF\x02\x01' + b'\0'*12 + (183).to_bytes(2, 'little') + b'fixture')
        binary.chmod(0o755)
        self.receipt = backend.spec(self.root) | {'configure_options': backend.OPTIONS,
            'gcc_version': 'gcc fixture', 'llc_version': 'LLVM version 5.0.1 with assertions aarch64',
            'binary_sha256': backend.sha(binary)}
        self.write()
        self.addCleanup(patch.stopall)
        patch.object(b, 'configuration', return_value='linux/arm64').start()

    def write(self):
        (self.cache / 'receipt.json').write_text(json.dumps(self.receipt))

    def snapshot(self):
        return {str(p.relative_to(self.root)): p.read_bytes() for p in self.root.rglob('*') if p.is_file()}

    def test_missing_and_check_side_effect_freedom(self):
        before = self.snapshot()
        with patch.object(b, 'image_check', side_effect=backend.BackendError('missing', 'image')):
            errors = b.check(self.root)
        self.assertEqual(len(errors), 3)
        self.assertIn('missing', errors[-1])
        self.assertEqual(before, self.snapshot())

    def test_valid_adoption_preserves_receipt_and_original(self):
        before = {p: backend.sha(self.cache / p) for p in ('receipt.json', 'out/bin/llc')}
        path = b.adopt(self.root, self.cache)
        self.assertEqual(path, backend.resolve(self.root))
        backend.validate(self.root)
        for p, digest in before.items():
            self.assertEqual(backend.sha(path / p), digest)
            self.assertEqual(backend.sha(self.cache / p), digest)
        self.assertEqual(b.adopt(self.root, self.cache), path)

    def test_upgrade_never_rewrites_old_cache_or_explicit_reference(self):
        first = b.adopt(self.root, self.cache)
        old = self.snapshot()
        recipe = self.root / 'toolchain/lua/Dockerfile'
        recipe.write_text(recipe.read_text() + '\n# changed recipe\n')
        self.assertNotEqual(first, backend.resolve(self.root))
        with self.assertRaisesRegex(backend.BackendError, 'missing'):
            backend.validate(self.root)
        with self.assertRaisesRegex(backend.BackendError, 'stale'):
            backend.validate(self.root, first)
        with self.assertRaisesRegex(backend.BackendError, 'stale'):
            b.adopt(self.root, self.cache)
        for name, value in old.items():
            if name != 'toolchain/lua/Dockerfile':
                self.assertEqual((self.root / name).read_bytes(), value)

    def test_configuration_binary_receipt_corruption(self):
        for field, value, status in [('recipe', {}, 'stale'), ('config', {}, 'stale'),
                                    ('configure_options', [], 'corrupted'),
                                    ('binary_sha256', '0'*64, 'corrupted'),
                                    ('llc_version', 'LLVM 99', 'unsupported')]:
            with self.subTest(field=field):
                saved = self.receipt[field]
                self.receipt[field] = value
                self.write()
                with self.assertRaisesRegex(backend.BackendError, status):
                    backend.validate(self.root, self.cache)
                self.receipt[field] = saved
        (self.cache / 'receipt.json').write_text('{}')
        with self.assertRaises(backend.BackendError):
            backend.validate(self.root, self.cache)
        (self.cache / 'receipt.json').write_text('broken')
        with self.assertRaisesRegex(backend.BackendError, 'corrupted'):
            backend.validate(self.root, self.cache)

    def test_bad_archive_rejected_before_docker_or_writes(self):
        archive = self.root / 'bad.tar.xz'
        archive.write_bytes(b'bad')
        before = self.snapshot()
        with patch.object(resource_admission, 'lease', return_value=nullcontext()), \
             patch.object(resource_admission, 'check'), \
             patch.object(b, 'image_check') as image, patch.object(b.subprocess, 'run') as run:
            with self.assertRaises(RuntimeError):
                b.prepare_lua(self.root, archive)
        image.assert_not_called()
        run.assert_not_called()
        self.assertEqual(before, self.snapshot())

    def test_direct_lua_bootstrap_denial_and_job_limits_precede_work(self):
        with patch.object(resource_admission, 'lease', return_value=nullcontext()), \
             patch.object(resource_admission, 'check', side_effect=resource_admission.AdmissionError('low space')), \
             patch.object(b, '_prepare_lua') as prepare:
            with self.assertRaisesRegex(resource_admission.AdmissionError, 'low space'):
                b.prepare_lua(self.root)
            prepare.assert_not_called()
        for jobs in (0, 9, True):
            with self.subTest(jobs=jobs), self.assertRaises(ValueError):
                b.prepare_lua(self.root, jobs=jobs)

    def test_occupied_cache_cannot_be_overwritten(self):
        path = b.adopt(self.root, self.cache)
        (path / 'out/bin/llc').write_bytes(b'corrupt')
        before = self.snapshot()
        with self.assertRaisesRegex(backend.BackendError, 'corrupted'):
            b.adopt(self.root, self.cache)
        self.assertEqual(before, self.snapshot())

    def test_image_missing_stale_corrupted_ready(self):
        info = subprocess.CompletedProcess([], 0, json.dumps([{'Id': 'sha256:fixture',
                   'Os': 'linux', 'Architecture': 'arm64'}]), '')
        expected = {name: backend.sha(self.root / 'toolchain' / name) for name in b.image_provenance.FILES}
        for result, status in [(subprocess.CompletedProcess([], 1, '', "can't open file"), 'stale'),
                               (subprocess.CompletedProcess([], 1, '', 'hash mismatch'), 'corrupted'),
                               (subprocess.CompletedProcess([], 0, '{}', ''), 'stale'),
                               (subprocess.CompletedProcess([], 0, json.dumps(expected), ''), 'ready')]:
            with patch.object(b.subprocess, 'run', side_effect=[info, result]):
                if status == 'ready':
                    self.assertEqual(b.image_check(self.root, 'fixture'), 'sha256:fixture')
                else:
                    with self.assertRaisesRegex(backend.BackendError, status):
                        b.image_check(self.root, 'fixture')

    def test_wrong_host_binary_and_explicit_old_cache_fail_closed(self):
        path = b.adopt(self.root, self.cache)
        binary = self.cache / 'out/bin/llc'
        binary.write_bytes(b'\x7fELF\x02\x01' + b'\0'*12 + (62).to_bytes(2, 'little'))
        self.receipt['binary_sha256'] = backend.sha(binary)
        self.write()
        with self.assertRaisesRegex(backend.BackendError, 'unsupported'):
            backend.validate(self.root, self.cache)
        with patch.dict(os.environ, {'D3_LUA_TOOLCHAIN': str(self.cache)}):
            self.assertEqual(backend.resolve(self.root), self.cache)
            with self.assertRaises(backend.BackendError):
                backend.validate(self.root)
        backend.validate(self.root, path)

    def test_runtime_probe_rejects_missing_libraries(self):
        with patch.object(b, 'image_check', return_value='sha256:fixture'), patch.object(
                b.subprocess, 'run', return_value=subprocess.CompletedProcess([], 1, '', 'missing library')):
            errors = b.check(self.root, cache=self.cache)
        self.assertEqual(len(errors), 1)
        self.assertIn('runtime version probe failed', errors[0])
        self.assertNotIn('mv ', errors[0])

    def test_readiness_blocks_build_before_configuration_or_cleanup(self):
        import build
        with patch.object(sys, 'argv', ['build.py', '--reconfigure']), patch.object(
                build, 'check', return_value=['stale backend']), patch.object(
                build.resource_admission, 'lease', return_value=nullcontext()), patch.object(
                build.resource_admission, 'check'), patch.object(
                build.shutil, 'which', return_value='/fixture/docker'), patch.object(
                build.shutil, 'rmtree') as remove, patch.object(build, 'compose') as run:
            with self.assertRaises(SystemExit):
                build.main()
        remove.assert_not_called()
        run.assert_not_called()

    def test_plain_python_check_writes_no_bytecode(self):
        before = self.snapshot()
        result = subprocess.run([sys.executable, str(self.root / 'toolchain/backend.py')],
                                capture_output=True, text=True)
        self.assertNotEqual(result.returncode, 0)
        self.assertEqual(before, self.snapshot())


if __name__ == '__main__':
    unittest.main()
