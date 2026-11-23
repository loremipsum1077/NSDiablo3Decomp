#!/usr/bin/env python3
"""Exercise patch installation using an isolated checkout of the pinned vendor."""
import contextlib
import io
from pathlib import Path
import shutil
import subprocess
import tempfile
import unittest

from checker_patches import INCORPORATED_COMMIT, apply_local_patches

ROOT = Path(__file__).resolve().parent.parent


class CheckerPatchTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory(prefix="d3-checker-patches-")
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)
        self.common = self.root / "common"
        self.patches = self.root / "patches"
        # Clone only committed state, never the active submodule's worktree edits.
        subprocess.run(["git", "clone", "--quiet", "--no-hardlinks", "--no-checkout",
                        str(ROOT / "tools/common"), str(self.common)], check=True)
        subprocess.run(["git", "checkout", "--quiet", "--detach", INCORPORATED_COMMIT],
                       cwd=self.common, check=True)
        shutil.copytree(ROOT / "tools/patches", self.patches)

    def apply(self):
        with contextlib.redirect_stdout(io.StringIO()):
            apply_local_patches(self.common, self.patches)

    def test_pinned_checkout_and_second_install(self):
        self.apply()
        result = (self.common / "viking/src/checks.rs").read_bytes()
        self.assertIn(b"for off in (start..end).step_by(4)", result)
        self.assertIn(b"fn is_brk", result)
        self.assertIn(b"fn pair_flow", result)
        self.assertIn(b"fn same_add_encoding", result)
        self.assertIn(b"fn same_memory_encoding", result)
        self.assertIn(b"GotEvidence", result)
        self.assertIn(b"jump_tables::resolve", result)
        self.assertIn(b"unverified_adrp", result)
        self.assertIn(b"copied: Option<(cs::RegId, u8)>", result)
        identity = (self.common / "viking/src/elf.rs").read_bytes()
        self.assertIn(b"fn verify_link_map_provenance", identity)
        self.assertIn(b"fn conflicting_function_addresses", identity)
        self.assertIn(b"fn build_got_targets", identity)
        digest = (self.common / "viking/src/sha256.rs").read_bytes()
        tables = (self.common / "viking/src/jump_tables.rs").read_bytes()
        self.apply()
        self.assertEqual(result, (self.common / "viking/src/checks.rs").read_bytes())
        self.assertEqual(identity, (self.common / "viking/src/elf.rs").read_bytes())
        self.assertEqual(digest, (self.common / "viking/src/sha256.rs").read_bytes())
        self.assertEqual(tables, (self.common / "viking/src/jump_tables.rs").read_bytes())
        subprocess.run(["git", "diff", "--check"], cwd=self.common, check=True)

    def test_changed_incorporated_patch_fails_before_mutation(self):
        before = (self.common / "viking/src/checks.rs").read_bytes()
        patch = next(self.patches.glob("0001-*.patch"))
        patch.write_bytes(patch.read_bytes() + b"\n")
        with self.assertRaisesRegex(RuntimeError, "incorporated patch content changed"):
            self.apply()
        self.assertEqual(before, (self.common / "viking/src/checks.rs").read_bytes())

    def test_upgrade_from_previous_patch_preserves_unrelated_edits(self):
        previous = next(self.patches.glob("0005-*.patch"))
        subprocess.run(["git", "apply", str(previous)], cwd=self.common, check=True)
        unrelated = self.common / "viking/Cargo.toml"
        original = unrelated.read_bytes() + b"\n# local unrelated edit\n"
        unrelated.write_bytes(original)
        self.apply()
        self.assertIn(b"fn pair_flow", (self.common / "viking/src/checks.rs").read_bytes())
        self.assertEqual(unrelated.read_bytes(), original)
        self.apply()
        self.assertEqual(unrelated.read_bytes(), original)

    def test_unknown_bad_patch_fails(self):
        (self.patches / "0006-unknown.patch").write_text("not a patch\n")
        with self.assertRaises(subprocess.CalledProcessError):
            self.apply()

    def test_upgrade_from_cfg_patch(self):
        for prefix in ('0005-', '0006-'):
            patch = next(self.patches.glob(prefix + '*.patch'))
            subprocess.run(['git', 'apply', str(patch)], cwd=self.common, check=True)
        self.apply()
        self.assertIn(b'fn verify_link_map_provenance',
                      (self.common / 'viking/src/elf.rs').read_bytes())
        self.assertTrue((self.common / 'viking/src/sha256.rs').is_file())
        self.apply()

    def test_unknown_history_gets_no_incorporation_exemption(self):
        # Same file contents but no trusted commit history must not silently skip
        # overlapping historical patches on the strength of a filename alone.
        shutil.rmtree(self.common / ".git")
        subprocess.run(["git", "init", "--quiet"], cwd=self.common, check=True)
        with self.assertRaises(subprocess.CalledProcessError):
            self.apply()

    def test_upgrade_from_identity_and_switch_patches(self):
        # Both existing 0007 installs and partially upgraded 0008 installs must
        # reach the same final source without losing unrelated local changes.
        for prefix in ('0005-', '0006-', '0007-'):
            patch = next(self.patches.glob(prefix + '*.patch'))
            subprocess.run(['git', 'apply', str(patch)], cwd=self.common, check=True)
        before = (self.common / 'viking/src/sha256.rs').read_bytes()
        # A partial 0008 install predates every later patch.
        later = [patch for patch in sorted(self.patches.glob('*.patch')) if patch.name >= '0009-']
        for patch in later:
            patch.rename(patch.with_suffix('.held'))
        try:
            self.apply()
            self.assertTrue((self.common / 'viking/src/jump_tables.rs').is_file())
            self.apply()
        finally:
            for patch in later:
                patch.with_suffix('.held').rename(patch)
        self.apply()
        result = (self.common / 'viking/src/checks.rs').read_bytes()
        self.assertIn(b'GotEvidence', result)
        self.assertIn(b'unverified_adrp', result)
        self.assertEqual(before, (self.common / 'viking/src/sha256.rs').read_bytes())
        self.apply()
        self.assertEqual(result, (self.common / 'viking/src/checks.rs').read_bytes())


if __name__ == "__main__":
    unittest.main()
