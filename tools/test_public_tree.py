import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

from check_public_tree import private_header, private_path


class PublicationTests(unittest.TestCase):
    def test_paths_and_binary_signatures(self):
        for path in ("game/readme", "private/romfs/a", "ExEfS/main", "title.keys", "backup.XCI",
                     "src/X/Common/Protobuf/Demo.pb.h", "src/X/Common/generated-game-data/test.inc"):
            self.assertTrue(private_path(path), path)
        for data in (b"NSO0", b"\x7fELF", bytes(256) + b"HEAD", bytes(512) + b"NCA3",
                     b"master_key_00 = " + b"a" * 32):
            self.assertTrue(private_header(data))
        self.assertFalse(private_path("src/X/Common/Protobuf/layout/Test.inc"))
        self.assertFalse(private_header(b"// NSO0 in a source comment"))

    def test_guard_reads_staged_content(self):
        script = Path(__file__).with_name("check_public_tree.py").resolve()
        with tempfile.TemporaryDirectory() as tmp:
            subprocess.run(["git", "init", "-q"], cwd=tmp, check=True)
            p = Path(tmp, "ordinary")
            p.write_bytes(b"NSO0")
            subprocess.run(["git", "add", "ordinary"], cwd=tmp, check=True)
            p.write_text("clean working copy")
            run = subprocess.run([sys.executable, script], cwd=tmp, capture_output=True)
            self.assertEqual(run.returncode, 1)
            self.assertIn(b"ordinary", run.stderr)
