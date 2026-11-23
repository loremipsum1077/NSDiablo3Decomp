"""Fail-closed artifact provenance checks, independent of the game toolchain."""
import hashlib
import json
from pathlib import Path
import tempfile
import unittest

from data_provenance import verify_link_provenance


class DataProvenanceTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory(prefix='data provenance ')
        self.addCleanup(self.temp.cleanup)
        self.elf = Path(self.temp.name) / 'd3'
        self.link_map = Path(str(self.elf) + '.map')
        self.proof = Path(str(self.link_map) + '.provenance')
        self.elf.write_bytes(b'ELF\x00\xff')
        self.link_map.write_bytes(b'link map\r\n\x00')
        self.record = dict(format=1, elf_sha256=hashlib.sha256(self.elf.read_bytes()).hexdigest(),
                           map_sha256=hashlib.sha256(self.link_map.read_bytes()).hexdigest())
        self.write_record(self.record)

    def write_record(self, value):
        self.proof.write_text(json.dumps(value) + '\n')

    def test_exact_byte_hashes_and_read_only(self):
        before = {p: (p.read_bytes(), p.stat().st_mtime_ns)
                  for p in (self.elf, self.link_map, self.proof)}
        self.assertEqual(verify_link_provenance(self.elf), dict(self.record,
            status='verified_link_artifacts', record_sha256=hashlib.sha256(self.proof.read_bytes()).hexdigest()))
        self.assertEqual(before, {p: (p.read_bytes(), p.stat().st_mtime_ns) for p in before})

    def test_explicit_archive_paths(self):
        archived_map = self.link_map.with_name('archived-map')
        archived_record = self.proof.with_name('archived-record')
        self.link_map.rename(archived_map)
        self.proof.rename(archived_record)
        self.assertEqual(verify_link_provenance(self.elf, archived_map, archived_record)['status'],
                         'verified_link_artifacts')

    def test_each_missing_input_fails(self):
        for path in (self.elf, self.link_map, self.proof):
            with self.subTest(path=path):
                content = path.read_bytes()
                path.unlink()
                with self.assertRaises(ValueError):
                    verify_link_provenance(self.elf)
                self.assertFalse(path.exists())
                path.write_bytes(content)

    def test_each_changed_artifact_fails(self):
        for path in (self.elf, self.link_map):
            with self.subTest(path=path):
                original = path.read_bytes()
                path.write_bytes(original + b'changed')
                with self.assertRaisesRegex(ValueError, 'stale link provenance'):
                    verify_link_provenance(self.elf)
                path.write_bytes(original)

    def test_invalid_json_utf8_and_duplicate_keys(self):
        for blob in (b'', b'not json', b'\xff', b'[]', b'null',
                     ('{"format":1,' + json.dumps(self.record)[1:]).encode()):
            with self.subTest(blob=blob):
                self.proof.write_bytes(blob)
                with self.assertRaises(ValueError):
                    verify_link_provenance(self.elf)

    def test_exact_schema(self):
        for field in self.record:
            malformed = dict(self.record)
            del malformed[field]
            self.write_record(malformed)
            with self.assertRaises(ValueError):
                verify_link_provenance(self.elf)
        for extra in ('elf_path', 'map_path', 'elf_name', 'source_revision'):
            self.write_record(dict(self.record, **{extra: 'unvalidated claim'}))
            with self.assertRaises(ValueError):
                verify_link_provenance(self.elf)

    def test_format_must_be_integer_one(self):
        for value in (True, False, 1.0, '1', None, 0, 2, {}, []):
            with self.subTest(value=value):
                self.write_record(dict(self.record, format=value))
                with self.assertRaises(ValueError):
                    verify_link_provenance(self.elf)

    def test_hashes_must_be_exact_lowercase_sha256(self):
        for key in ('elf_sha256', 'map_sha256'):
            for value in (None, 1, [], {}, 'a' * 63, 'a' * 65, 'g' * 64,
                          self.record[key].upper(), self.record[key] + '\n'):
                with self.subTest(key=key, value=value):
                    self.write_record(dict(self.record, **{key: value}))
                    with self.assertRaises(ValueError):
                        verify_link_provenance(self.elf)


if __name__ == '__main__':
    unittest.main()
