"""Offline setup pinning tests with a real local Git remote."""
import hashlib
import importlib.util
from pathlib import Path
import subprocess
import tempfile
import unittest
from unittest.mock import patch

from test_verify_original_inputs import fixture_manifest

spec = importlib.util.spec_from_file_location("d3_setup", Path(__file__).with_name("setup.py"))
setup = importlib.util.module_from_spec(spec)
spec.loader.exec_module(setup)


class SetupTests(unittest.TestCase):
    def setUp(self):
        temp = tempfile.TemporaryDirectory()
        self.addCleanup(temp.cleanup)
        self.root = Path(temp.name)
        self.remote = self.root / "remote"
        self.remote.mkdir()
        self.git("init", "--quiet")
        self.binary = self.remote / "linux/nx2elf"
        self.binary.parent.mkdir()
        self.binary.write_bytes(b"pinned binary fixture")
        self.binary.chmod(0o755)
        self.git("add", ".")
        self.git("-c", "user.name=Test", "-c", "user.email=test@example.com",
                 "-c", "commit.gpgsign=false", "commit", "--quiet", "-m", "pinned")
        self.revision = self.git("rev-parse", "HEAD").strip()
        self.lock = {"schema_version": 1, "repository": str(self.remote),
                     "revision": self.revision,
                     "sha256": {"linux/nx2elf": hashlib.sha256(self.binary.read_bytes()).hexdigest()}}
        self.destination = self.root / "toolchain/binaries"
        self.addCleanup(patch.stopall)
        patch.object(setup, "BINARIES", self.destination).start()
        patch.object(setup, "load_lock", return_value=self.lock).start()

    def git(self, *args):
        return subprocess.check_output(["git", "-C", str(self.remote), *args], text=True)

    def test_fetches_exact_revision_even_when_remote_advances(self):
        self.binary.write_bytes(b"new unapproved binary")
        self.git("add", ".")
        self.git("-c", "user.name=Test", "-c", "user.email=test@example.com",
                 "-c", "commit.gpgsign=false", "commit", "--quiet", "-m", "new head")
        setup.fetch_binaries()
        self.assertEqual((self.destination / "linux/nx2elf").read_bytes(), b"pinned binary fixture")
        setup.verify_binaries(self.destination, self.lock)

    def test_existing_verified_directory_requires_no_network(self):
        setup.fetch_binaries()
        with patch.object(setup.subprocess, "check_call", side_effect=AssertionError("network")):
            setup.fetch_binaries()

    def test_wrong_revision_rejected_without_mutation(self):
        setup.fetch_binaries()
        self.lock["revision"] = "0" * 40
        with self.assertRaisesRegex(RuntimeError, "revision"):
            setup.fetch_binaries()
        self.assertEqual((self.destination / "linux/nx2elf").read_bytes(), b"pinned binary fixture")

    def test_changed_hash_rejected_without_mutation(self):
        setup.fetch_binaries()
        binary = self.destination / "linux/nx2elf"
        binary.write_bytes(b"local edit")
        with self.assertRaisesRegex(RuntimeError, "SHA-256"):
            setup.fetch_binaries()
        self.assertEqual(binary.read_bytes(), b"local edit")

    def test_untracked_existing_directory_preserved(self):
        self.destination.mkdir(parents=True)
        marker = self.destination / "important"
        marker.write_text("keep me")
        with self.assertRaisesRegex(RuntimeError, "not a binaries git checkout"):
            setup.fetch_binaries()
        self.assertEqual(marker.read_text(), "keep me")

    def test_existing_file_preserved(self):
        self.destination.parent.mkdir()
        self.destination.write_text("keep me")
        with self.assertRaisesRegex(RuntimeError, "not a binaries git checkout"):
            setup.fetch_binaries()
        self.assertEqual(self.destination.read_text(), "keep me")

    def test_failed_download_does_not_publish_directory(self):
        self.lock["sha256"]["linux/nx2elf"] = "0" * 64
        with self.assertRaisesRegex(RuntimeError, "SHA-256"):
            setup.fetch_binaries()
        self.assertFalse(self.destination.exists())
        self.assertEqual(list(self.destination.parent.iterdir()), [])

    def test_symlink_binary_rejected(self):
        setup.fetch_binaries()
        binary = self.destination / "linux/nx2elf"
        binary.unlink()
        binary.symlink_to(self.binary)
        with self.assertRaisesRegex(RuntimeError, "SHA-256"):
            setup.fetch_binaries()

    def test_nonexecutable_binary_rejected(self):
        setup.fetch_binaries()
        (self.destination / "linux/nx2elf").chmod(0o644)
        with self.assertRaisesRegex(RuntimeError, "not executable"):
            setup.fetch_binaries()

    def test_tool_selection_ignores_environment_override(self):
        setup.fetch_binaries()
        with patch.object(setup.platform, "system", return_value="Linux"), \
             patch.dict("os.environ", {"NX_DECOMP_TOOLS_NX2ELF": "/unapproved/nx2elf"}):
            self.assertEqual(setup.pinned_nx2elf(), self.destination / "linux/nx2elf")

    def test_unsupported_platform_rejected(self):
        setup.fetch_binaries()
        with patch.object(setup.platform, "system", return_value="Windows"):
            with self.assertRaisesRegex(RuntimeError, "no pinned nx2elf"):
                setup.pinned_nx2elf()


class OriginalInputGateTests(unittest.TestCase):
    def setUp(self):
        temp = tempfile.TemporaryDirectory()
        self.addCleanup(temp.cleanup)
        self.root = Path(temp.name).resolve()
        self.nso = self.root / 'build/exefs/main'
        self.nss = self.root / 'build/romfs/DiabloIIINX64r.nss'
        self.nso.parent.mkdir(parents=True)
        self.nss.parent.mkdir(parents=True)
        self.nso.write_bytes(b'NSO fixture')
        self.nss.write_bytes(b'NSS fixture')
        fixture_manifest(self.root, self.nso, self.nss)

    def test_missing_or_corrupt_inputs_block_before_install_download_or_conversion(self):
        for missing in (False, True):
            if missing:
                self.nss.unlink()
            else:
                self.nss.write_bytes(b'bad fixture')
            with self.subTest(missing=missing), patch.object(setup, 'ROOT', self.root), \
                    patch.object(setup, 'fetch_binaries') as fetch, \
                    patch.object(setup, 'prepare_executable') as convert, \
                    patch.object(setup, 'install_checker') as install, \
                    patch.object(setup.subprocess, 'check_call') as command:
                # tools/common need not even be initialized for this early rejection.
                with self.assertRaisesRegex(RuntimeError, 'missing|wrong_hash'):
                    setup.main([str(self.nso)])
                fetch.assert_not_called()
                convert.assert_not_called()
                install.assert_not_called()
                command.assert_not_called()


class CheckerInstallTests(unittest.TestCase):
    def setUp(self):
        temp = tempfile.TemporaryDirectory()
        self.addCleanup(temp.cleanup)
        self.root = Path(temp.name)
        (self.root / "tools").mkdir()
        root_patch = patch.object(setup, "ROOT", self.root)
        root_patch.start()
        self.addCleanup(root_patch.stop)

    def test_locked_build_uses_explicit_target_and_copies_outputs(self):
        def build(command):
            self.assertIn("--locked", command)
            self.assertEqual(command[command.index("--target-dir") + 1],
                             str(self.root / "build/checker-target"))
            directory = self.root / "build/checker-target/release"
            directory.mkdir(parents=True)
            for name in ("check", "listsym", "decompme"):
                binary = directory / name
                binary.write_bytes(name.encode())
                binary.chmod(0o755)
        with patch.object(setup.subprocess, "check_call", side_effect=build), \
             patch.dict("os.environ", {"CARGO_TARGET_DIR": "/unrelated/cache"}):
            setup.install_checker()
        for name in ("check", "listsym", "decompme"):
            binary = self.root / "tools" / name
            self.assertEqual(binary.read_bytes(), name.encode())
            self.assertTrue(binary.stat().st_mode & 0o111)

    def test_build_failure_preserves_existing_checker(self):
        checker = self.root / "tools/check"
        checker.write_bytes(b"previous checker")
        with patch.object(setup.subprocess, "check_call", side_effect=subprocess.CalledProcessError(1, "cargo")):
            with self.assertRaises(subprocess.CalledProcessError):
                setup.install_checker()
        self.assertEqual(checker.read_bytes(), b"previous checker")

    def test_incomplete_outputs_do_not_partially_install(self):
        directory = self.root / "build/checker-target/release"
        directory.mkdir(parents=True)
        (directory / "check").write_bytes(b"new checker")
        checker = self.root / "tools/check"
        checker.write_bytes(b"previous checker")
        with patch.object(setup.subprocess, "check_call"):
            with self.assertRaisesRegex(RuntimeError, "did not produce"):
                setup.install_checker()
        self.assertEqual(checker.read_bytes(), b"previous checker")


if __name__ == "__main__":
    unittest.main()
