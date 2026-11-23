"""Offline LLVM pin/download/cache and Docker policy fixtures; no real installs."""
import contextlib
import hashlib
import importlib.util
import io
import json
import os
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest
from unittest.mock import Mock, patch
import urllib.error
import urllib.request

import bootstrap
import bootstrap_toolchain

ROOT = Path(__file__).resolve().parents[1]
spec = importlib.util.spec_from_file_location('llvm_archive', ROOT / 'toolchain/llvm_archive.py')
llvm = importlib.util.module_from_spec(spec)
spec.loader.exec_module(llvm)


class ArchiveTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.base = Path(self.temp.name)
        self.data = b'synthetic archive contents; never executed'
        self.lock = json.loads(llvm.LOCK.read_text())
        for entry in self.lock['archives'].values():
            entry.update(sha256=hashlib.sha256(self.data).hexdigest(), size_bytes=len(self.data))
        self.lock_path = self.base / 'lock.json'
        self.lock_path.write_text(json.dumps(self.lock))
        self.entry = self.select('linux/arm64')
        self.cache = self.base / 'cache'
        self.network = patch.object(urllib.request, 'build_opener').start()
        self.addCleanup(patch.stopall)

    def select(self, platform='linux/arm64', **kwargs):
        return llvm.select(platform, lock_path=self.lock_path, **kwargs)

    def cached(self, data=None):
        self.cache.mkdir(exist_ok=True)
        target = self.cache / self.entry['filename']
        target.write_bytes(self.data if data is None else data)
        return target

    def download(self, data=None):
        response = io.BytesIO(self.data if data is None else data)
        response.geturl = lambda: self.entry['url']
        self.network.return_value.open.return_value = response

    def test_supported_archives_have_exact_original_url_mapping(self):
        for platform, arch in llvm.PLATFORMS.items():
            entry = llvm.select(platform)
            self.assertEqual(entry['arch'], arch)
            self.assertTrue(entry['url'].startswith('https://releases.llvm.org/5.0.1/'))
            self.assertRegex(entry['sha256'], '^[0-9a-f]{64}$')
        self.network.assert_not_called()

    def test_unknown_platform_arch_version_and_url_fail_closed(self):
        for args in [dict(platform='linux/arm64/v8'), dict(platform='linux/riscv64'),
                     dict(arch='x86_64-linux-gnu-ubuntu-16.04'), dict(version='6.0.0'),
                     dict(url='http://releases.llvm.org/5.0.1/' + self.entry['filename']),
                     dict(url='https://mirror.invalid/file')]:
            with self.subTest(args=args), self.assertRaises(RuntimeError):
                self.select(**args)
        self.network.assert_not_called()
        self.assertFalse(self.cache.exists())

    def test_missing_invalid_checksum_and_size_fail_before_cache_or_network(self):
        for value in ('', 'A' * 64, 'skip', None):
            self.lock['archives']['linux/arm64']['sha256'] = value
            self.lock_path.write_text(json.dumps(self.lock))
            with self.subTest(value=value), self.assertRaises(RuntimeError):
                self.select()
        self.lock['archives']['linux/arm64'].pop('sha256')
        self.lock_path.write_text(json.dumps(self.lock))
        with self.assertRaises(RuntimeError):
            self.select()
        self.network.assert_not_called()
        self.assertFalse(self.cache.exists())

    def test_missing_archive_without_opt_in_makes_no_files(self):
        with self.assertRaisesRegex(RuntimeError, 'missing'):
            llvm.fetch(self.entry, self.cache)
        self.network.assert_not_called()
        self.assertFalse(self.cache.exists())

    def test_valid_download_is_verified_and_published_atomically(self):
        self.download()
        target = llvm.fetch(self.entry, self.cache, allow_downloads=True)
        self.assertEqual(target.read_bytes(), self.data)
        self.assertEqual(list(self.cache.iterdir()), [target])
        self.network.return_value.open.assert_called_once_with(self.entry['url'], timeout=30)

    def test_corrupt_truncated_and_oversized_download_never_becomes_cache(self):
        for data in (b'X' * len(self.data), self.data[:-1], self.data + b'X'):
            self.download(data)
            with self.subTest(data=data), self.assertRaises(RuntimeError):
                llvm.fetch(self.entry, self.cache, allow_downloads=True)
            self.assertEqual(list(self.cache.iterdir()), [])

    def test_network_error_leaves_no_archive(self):
        self.network.return_value.open.side_effect = urllib.error.URLError('fixture offline')
        with self.assertRaises(urllib.error.URLError):
            llvm.fetch(self.entry, self.cache, allow_downloads=True)
        self.assertEqual(list(self.cache.iterdir()), [])

    def test_read_error_removes_partial_download(self):
        self.download()
        response = self.network.return_value.open.return_value
        response.read = Mock(side_effect=[b'partial', OSError('interrupted')])
        with self.assertRaises(OSError):
            llvm.fetch(self.entry, self.cache, allow_downloads=True)
        self.assertEqual(list(self.cache.iterdir()), [])

    def test_valid_cache_is_rehashed_on_each_use_without_network(self):
        target = self.cached()
        self.assertEqual(llvm.fetch(self.entry, self.cache, True), target)
        target.write_bytes(b'X' * len(self.data))
        with self.assertRaisesRegex(RuntimeError, 'SHA-256 mismatch'):
            llvm.fetch(self.entry, self.cache, True)
        self.assertEqual(target.read_bytes(), b'X' * len(self.data))
        self.network.assert_not_called()

    def test_truncated_cache_is_preserved_and_never_replaced(self):
        target = self.cached(self.data[:-1])
        with self.assertRaisesRegex(RuntimeError, 'size mismatch'):
            llvm.fetch(self.entry, self.cache, True)
        self.assertEqual(target.read_bytes(), self.data[:-1])
        self.network.assert_not_called()

    def test_archive_symlink_and_directory_are_rejected(self):
        real = self.base / 'real'
        real.write_bytes(self.data)
        self.cache.mkdir()
        target = self.cache / self.entry['filename']
        target.symlink_to(real)
        with self.assertRaises(RuntimeError):
            llvm.fetch(self.entry, self.cache, True)
        target.unlink()
        target.mkdir()
        with self.assertRaises(RuntimeError):
            llvm.fetch(self.entry, self.cache, True)
        self.network.assert_not_called()

    def test_cache_symlink_download_refused(self):
        real = self.base / 'real'
        real.mkdir()
        self.cache.symlink_to(real)
        with self.assertRaisesRegex(RuntimeError, 'symlink'):
            llvm.fetch(self.entry, self.cache, True)
        self.network.assert_not_called()

    def test_cross_host_insecure_and_other_path_redirect_refused(self):
        request = urllib.request.Request(self.entry['url'])
        for url in ('http://releases.llvm.org/x', 'https://example.invalid/x',
                    'https://releases.llvm.org/other.tar.xz'):
            with self.subTest(url=url), self.assertRaisesRegex(RuntimeError, 'redirect refused'):
                llvm.OfficialRedirects().redirect_request(request, None, 302, 'Moved', {}, url)

    def test_response_changed_url_refused_before_writing(self):
        self.download()
        self.network.return_value.open.return_value.geturl = lambda: 'http://invalid/'
        with self.assertRaisesRegex(RuntimeError, 'URL changed'):
            llvm.fetch(self.entry, self.cache, True)
        self.assertEqual(list(self.cache.iterdir()), [])

    def test_cli_reports_expected_failure_without_traceback(self):
        with contextlib.redirect_stderr(io.StringIO()) as err:
            result = llvm.main(['--platform', 'unsupported', '--cache-dir', str(self.cache)])
        self.assertEqual(result, 1)
        self.assertIn('unsupported', err.getvalue())
        self.network.assert_not_called()


class ToolchainBootstrapTests(unittest.TestCase):
    def setUp(self):
        self.addCleanup(patch.stopall)
        patch.dict(os.environ, {'D3_DOCKER_PLATFORM': 'linux/arm64', 'CLANG_VERSION': '5.0.1',
                                'LLVM_ARCH': '', 'CLANG_URL': ''}).start()
        patch.object(bootstrap_toolchain.platform, 'system', return_value='Linux').start()
        patch.object(bootstrap_toolchain.platform, 'machine', return_value='aarch64').start()
        patch.object(bootstrap_toolchain.shutil, 'which', return_value='/fixture/docker').start()
        self.run = patch.object(bootstrap_toolchain.subprocess, 'run', return_value=subprocess.CompletedProcess([], 0)).start()
        self.out, self.err = io.StringIO(), io.StringIO()

    def prepare(self, **kwargs):
        with contextlib.redirect_stdout(self.out), contextlib.redirect_stderr(self.err):
            return bootstrap_toolchain.prepare(ROOT, **kwargs)

    def test_plan_needs_no_originals_and_does_not_contact_daemon(self):
        self.assertEqual(self.prepare(), 0)
        self.assertEqual(self.run.call_args.args[0], ['docker', 'compose', 'version'])
        self.run.assert_called_once()
        self.assertIn('docker compose build build', self.out.getvalue())

    def test_execute_requires_opt_in_before_probes(self):
        self.assertEqual(self.prepare(execute=True), 1)
        self.run.assert_not_called()

    def test_execute_only_builds_image_after_read_only_probe(self):
        self.assertEqual(self.prepare(execute=True, allow_downloads=True), 0)
        commands = [c.args[0] for c in self.run.call_args_list]
        self.assertEqual(commands, [['docker', 'compose', 'version'], ['docker', 'info', '--format', '{{.OSType}}'],
                                    ['docker', 'compose', 'build', 'build']])
        self.assertNotIn('shell', self.run.call_args.kwargs)

    def test_failed_daemon_probe_prevents_image_build(self):
        self.run.side_effect = [subprocess.CompletedProcess([], 0), subprocess.CalledProcessError(1, ['docker', 'info'])]
        self.assertEqual(self.prepare(execute=True, allow_downloads=True), 1)
        self.assertEqual(self.run.call_count, 2)

    def test_build_failure_is_propagated(self):
        self.run.side_effect = [subprocess.CompletedProcess([], 0), subprocess.CompletedProcess([], 0), subprocess.CompletedProcess([], 7)]
        self.assertEqual(self.prepare(execute=True, allow_downloads=True), 7)

    def test_missing_docker_unsupported_host_and_wrong_platform_block(self):
        with patch.object(bootstrap_toolchain.shutil, 'which', return_value=None):
            self.assertEqual(self.prepare(), 1)
        with patch.object(bootstrap_toolchain.platform, 'machine', return_value='riscv64'):
            self.assertEqual(self.prepare(), 1)
        with patch.object(bootstrap_toolchain.platform, 'machine', return_value='x86_64'):
            self.assertEqual(self.prepare(), 1)
            with patch.dict(os.environ, {'D3_DOCKER_PLATFORM': 'linux/amd64'}):
                self.assertEqual(self.prepare(), 0)

    def test_existing_bootstrap_dispatches_before_original_checks(self):
        with patch.object(bootstrap_toolchain, 'prepare', return_value=0) as prepare, patch.object(bootstrap, 'preflight') as preflight:
            self.assertEqual(bootstrap.main(['--toolchain-plan']), 0)
            prepare.assert_called_once_with(ROOT, execute=False, allow_downloads=False)
            preflight.assert_not_called()


class LuaExtractionOrderTests(unittest.TestCase):
    def setUp(self):
        spec = importlib.util.spec_from_file_location('d3_lua_bootstrap_fixture', ROOT / 'toolchain/lua/bootstrap.py')
        self.lua = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(self.lua)
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.base = Path(self.temp.name)
        self.lua.ROOT = self.base
        self.payload = b'synthetic source; never extracted'
        self.archive = self.base / 'source.tar.xz'
        self.entry = dict(llvm.select('source'), sha256=hashlib.sha256(self.payload).hexdigest(), size_bytes=len(self.payload))
        patch.object(self.lua.llvm_archive, 'select', return_value=self.entry).start()
        patch.object(sys, 'argv', ['bootstrap.py', '--archive', str(self.archive)]).start()
        self.tar = patch.object(self.lua.tarfile, 'open', side_effect=RuntimeError('fixture stopped before extraction')).start()
        self.run = patch.object(self.lua.subprocess, 'run').start()
        self.addCleanup(patch.stopall)

    def test_corrupt_and_truncated_sources_fail_before_extraction_or_build_files(self):
        for data in (b'X' * len(self.payload), self.payload[:-1]):
            self.archive.write_bytes(data)
            with self.subTest(data=data), self.assertRaisesRegex(RuntimeError, 'mismatch'):
                self.lua.main()
            self.tar.assert_not_called()
            self.run.assert_not_called()
            self.assertFalse((self.base / 'build').exists())

    def test_valid_source_reaches_extraction_only_after_verification(self):
        self.archive.write_bytes(self.payload)
        with self.assertRaisesRegex(RuntimeError, 'fixture stopped before extraction'):
            self.lua.main()
        self.tar.assert_called_once_with(self.archive)
        self.run.assert_not_called()


class PolicyTests(unittest.TestCase):
    def test_docker_verification_precedes_tar_and_no_unchecked_curl(self):
        docker = (ROOT / 'toolchain/Dockerfile').read_text()
        self.assertLess(docker.index('COPY Dockerfile llvm.lock.json llvm_archive.py image_provenance.py'), docker.index('archive="$(python3'))
        self.assertLess(docker.index('archive="$(python3'), docker.index('tar -xJf "$archive"'))
        self.assertIn('--platform "${D3_DOCKER_PLATFORM}"', docker)
        self.assertIn('--url "${CLANG_URL}"', docker)
        self.assertIn('--version "${CLANG_VERSION}"', docker)
        self.assertNotIn('curl ', docker)
        self.assertIn('python3 xz-utils ca-certificates', docker)

    def test_lua_config_uses_canonical_source_pin(self):
        config = json.loads((ROOT / 'toolchain/lua/config.json').read_text())
        self.assertEqual(config['archive_key'], 'source')
        self.assertNotIn('archive_sha256', config)
        self.assertIn("'../llvm.lock.json'", (ROOT / 'toolchain/lua/bootstrap.py').read_text())
        self.assertIn("'../llvm.lock.json'", (ROOT / 'toolchain/backend.py').read_text())
        cmake = (ROOT / 'CMakeLists.txt').read_text()
        self.assertIn('/toolchain/llvm.lock.json;', cmake)


if __name__ == '__main__':
    unittest.main()
