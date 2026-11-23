"""End-to-end validation evidence and read-only CLI boundaries."""
import gzip
import hashlib
import json
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest
from unittest.mock import patch

from data_coverage import build_ledger
from data_relocations import RelocationImage
from test_data_elf import write_data_elf
from validate_data import validate_data


class ValidateDataTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory(prefix='validate data ')
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)
        self.csv = self.root / 'symbols.csv'
        self.csv.write_text('Address,Name\n0x1000,table\n0x1000,table\n0x1000,alias\n0x2000,zero\n0x1010,unknown\n')
        self.original = write_data_elf(self.root / 'original.elf')
        self.rebuilt = write_data_elf(self.root / 'rebuilt.elf')
        self.nss = write_data_elf(self.root / 'symbols.nss')
        self.link_map = Path(str(self.rebuilt) + '.map')
        self.link_map.write_bytes(b'synthetic link map\n')
        self.proof = Path(str(self.link_map) + '.provenance')
        self.proof.write_text(json.dumps(dict(format=1,
            elf_sha256=hashlib.sha256(self.rebuilt.read_bytes()).hexdigest(),
            map_sha256=hashlib.sha256(self.link_map.read_bytes()).hexdigest())) + '\n')

    def validate(self, **kwargs):
        return validate_data(self.csv, self.original, self.rebuilt, load_bias=0, **kwargs)

    def cli(self, output, *extra):
        return subprocess.run([sys.executable, str(Path(__file__).with_name('validate_data.py')),
            '--csv', str(self.csv), '--elf', str(self.original), '--rebuilt', str(self.rebuilt),
            '--load-bias', '0', '--output', str(output), *map(str, extra)],
            capture_output=True, text=True)

    def test_conserves_inventory_duplicates_and_unresolved_rows(self):
        before = build_ledger(self.csv, self.original, rebuilt_path=self.rebuilt, load_bias=0)
        result = self.validate()
        self.assertEqual(result['schema_version'], 2)
        self.assertEqual(result['summary']['row_count'], 5)
        self.assertEqual(result['summary']['unique_address_name_count'], 4)
        self.assertEqual(result['sections'], before['sections'])
        for inventory, validated in zip(before['rows'], result['rows']):
            self.assertEqual(inventory, {k: v for k, v in validated.items() if k != 'validation'})
        self.assertEqual(result['rows'][-1]['validation']['status'], 'unverified')
        self.assertEqual(sum(result['summary']['validation_status'].values()), 5)
        self.assertEqual(result['summary']['runtime_verified_count'], 0)
        self.assertEqual(result['provenance']['status'], 'verified_link_artifacts')

    def test_missing_provenance_is_mandatory(self):
        self.proof.unlink()
        with self.assertRaisesRegex(ValueError, 'provenance'):
            self.validate()
        self.assertFalse(self.proof.exists())

    def test_local_objects_need_owner_identity_for_object_and_notype(self):
        self.csv.write_text('Address,Name\n0x1000,local\n')
        for symbol_type in (0, 1):
            with self.subTest(symbol_type=symbol_type):
                symbols = [('local', 1, 0x1000, 8, symbol_type)]
                write_data_elf(self.original, symbols=symbols)
                write_data_elf(self.rebuilt, symbols=symbols)
                record = json.loads(self.proof.read_text())
                record['elf_sha256'] = hashlib.sha256(self.rebuilt.read_bytes()).hexdigest()
                self.proof.write_text(json.dumps(record))
                row = self.validate()['rows'][0]
                self.assertEqual(row['validation']['status'], 'unverified')
                self.assertIn('local_object_without_owner_identity', row['validation']['reasons'])

    def test_malformed_symbol_bytes_cannot_collapse_to_one_identity(self):
        self.csv.write_text('Address,Name\n0x1000,bad\ufffd\n')
        for path, invalid in ((self.original, b'bad\xff'), (self.rebuilt, b'bad\xfe')):
            write_data_elf(path, symbols=[('badX', 1, 0x1000, 8, 0x11)])
            path.write_bytes(path.read_bytes().replace(b'badX\0', invalid + b'\0'))
        record = json.loads(self.proof.read_text())
        record['elf_sha256'] = hashlib.sha256(self.rebuilt.read_bytes()).hexdigest()
        self.proof.write_text(json.dumps(record))
        with self.assertRaises((UnicodeError, ValueError)):
            self.validate()

    def test_cli_json_and_gzip_are_deterministic(self):
        for suffix in ('.json', '.json.gz'):
            output = self.root / ('validation' + suffix)
            with self.subTest(suffix=suffix):
                first = self.cli(output)
                self.assertEqual(first.returncode, 0, first.stderr)
                content = output.read_bytes()
                second = self.cli(output)
                self.assertEqual(second.returncode, 0, second.stderr)
                self.assertEqual(content, output.read_bytes())
                self.assertEqual(first.stdout, second.stdout)
                payload = gzip.decompress(content) if suffix.endswith('.gz') else content
                self.assertEqual(json.loads(payload), self.validate())

    def test_cli_corrupt_provenance_fails_without_replacing_output(self):
        output = self.root / 'validation.json'
        output.write_bytes(b'previous report')
        for corruption in ('not JSON', json.dumps(dict(format=1, elf_sha256='0' * 64,
                                                       map_sha256='0' * 64))):
            self.proof.write_text(corruption)
            result = self.cli(output)
            self.assertNotEqual(result.returncode, 0)
            self.assertIn('provenance', result.stderr)
            self.assertEqual(output.read_bytes(), b'previous report')

    def test_cli_protects_all_inputs_and_aliases(self):
        for source in (self.csv, self.original, self.rebuilt, self.nss, self.link_map, self.proof):
            content = source.read_bytes()
            symlink = self.root / (source.name + '.symlink')
            symlink.symlink_to(source)
            hardlink = self.root / (source.name + '.hardlink')
            hardlink.hardlink_to(source)
            for output in (source, symlink, hardlink):
                with self.subTest(source=source.name, output=output.name):
                    result = self.cli(output, '--nss', self.nss)
                    self.assertNotEqual(result.returncode, 0)
                    self.assertIn('must not overwrite', result.stderr)
                    self.assertEqual(source.read_bytes(), content)

    def test_cli_protects_explicit_map_and_provenance_paths(self):
        archived_map = self.root / 'archived.map'
        archived_proof = self.root / 'archived.proof'
        self.link_map.rename(archived_map)
        self.proof.rename(archived_proof)
        for source in (archived_map, archived_proof):
            output = self.root / (source.name + '.hardlink')
            output.hardlink_to(source)
            content = source.read_bytes()
            result = self.cli(output, '--map', archived_map, '--provenance', archived_proof)
            self.assertNotEqual(result.returncode, 0)
            self.assertIn('must not overwrite', result.stderr)
            self.assertEqual(source.read_bytes(), content)
        result = self.cli(self.root / 'archived.json', '--map', archived_map,
                          '--provenance', archived_proof)
        self.assertEqual(result.returncode, 0, result.stderr)

    def test_relocation_snapshots_must_match_inventory_snapshot(self):
        changed = write_data_elf(self.root / 'changed.elf', data=b'X' * 32)
        for replaced_label in ('original', 'rebuilt', 'nss'):
            def load_snapshot(path, label='image', load_bias=0):
                return RelocationImage(changed if label == replaced_label else path, label, load_bias)
            with self.subTest(label=replaced_label):
                with patch('validate_data.RelocationImage', side_effect=load_snapshot):
                    with self.assertRaises(ValueError):
                        self.validate(nss_path=self.nss)

    def test_linkage_snapshots_must_match_inventory_snapshot(self):
        for replaced_label in ('original', 'rebuilt', 'nss'):
            def load_ledger(*args, **kwargs):
                ledger = build_ledger(*args, **kwargs)
                ledger['linkage'][replaced_label]['sha256'] = '0' * 64
                return ledger
            with self.subTest(label=replaced_label):
                with patch('validate_data.build_ledger', side_effect=load_ledger):
                    with self.assertRaisesRegex(ValueError, 'snapshots changed'):
                        self.validate(nss_path=self.nss)


if __name__ == '__main__':
    unittest.main()
