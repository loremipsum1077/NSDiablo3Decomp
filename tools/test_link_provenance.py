"""Exercise the link-time producer used by CMake, without the game toolchain."""
import hashlib
import json
from pathlib import Path
import shutil
import subprocess
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]
SCRIPT = ROOT / 'toolchain/RecordLinkProvenance.cmake'


@unittest.skipUnless(shutil.which('cmake'), 'cmake is required')
class LinkProvenanceTests(unittest.TestCase):
    def test_hashes_exact_bytes_and_replaces_previous_record(self):
        with tempfile.TemporaryDirectory(prefix='link provenance ') as temp:
            elf = Path(temp) / 'fixture'
            link_map = Path(str(elf) + '.map')
            proof = Path(str(link_map) + '.provenance')
            for content in (b'ELF\x00\xff', b'changed ELF\x00'):
                elf.write_bytes(content)
                link_map.write_bytes(b'map\r\n\x00')
                subprocess.run(['cmake', f'-DELF={elf}', '-P', str(SCRIPT)], check=True)
                self.assertEqual(json.loads(proof.read_text()), {
                    'format': 1,
                    'elf_sha256': hashlib.sha256(content).hexdigest(),
                    'map_sha256': hashlib.sha256(link_map.read_bytes()).hexdigest(),
                })
                self.assertFalse(Path(str(proof) + '.tmp').exists())

    def test_missing_link_outputs_cannot_create_record(self):
        with tempfile.TemporaryDirectory() as temp:
            elf = Path(temp) / 'fixture'
            for create_elf in (False, True):
                if create_elf:
                    elf.write_bytes(b'elf')
                result = subprocess.run(['cmake', f'-DELF={elf}', '-P', str(SCRIPT)],
                                        capture_output=True, text=True)
                self.assertNotEqual(result.returncode, 0)
                self.assertFalse(Path(str(elf) + '.map.provenance').exists())


if __name__ == '__main__':
    unittest.main()
