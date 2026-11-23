"""Conservation and fail-closed data coverage evidence tests."""
import csv
import gzip
import json
import subprocess
import sys
from pathlib import Path
import tempfile
import unittest

from data_coverage import build_ledger
from test_data_elf import write_data_elf


class CoverageTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)
        self.csv = self.root / 'symbols.csv'
        self.original = write_data_elf(self.root / 'original.elf')
        self.rebuilt = write_data_elf(self.root / 'rebuilt.elf')

    def ledger(self, rows=None, *, rebuilt=False, nss=None):
        if rows is None:
            rows = [(0x1000, 'table'), (0x1000, 'table'), (0x1000, 'alias'),
                    (0x1008, 'tail'), (0x2000, 'zero'), (0x1018, '_ZTV4Test')]
        with self.csv.open('w') as stream:
            writer = csv.writer(stream)
            writer.writerow(['Address', 'Name'])
            writer.writerows((hex(addr), name) for addr, name in rows)
        return build_ledger(self.csv, self.original, nss_path=nss,
                            rebuilt_path=self.rebuilt if rebuilt else None, load_bias=0)

    def test_duplicates_aliases_and_overlap_conserve_union_and_rows(self):
        result = self.ledger()
        self.assertEqual(result['summary']['row_count'], 6)
        self.assertEqual(result['summary']['unique_address_name_count'], 5)
        self.assertEqual([r['row_id'] for r in result['rows']], list(range(1, 7)))
        data = next(s for s in result['sections'] if s['name'] == '.data')
        self.assertEqual(data['covered_bytes'], 32)
        self.assertEqual(data['gap_bytes'], 0)
        for section in result['sections']:
            self.assertEqual(section['covered_bytes'] + section['gap_bytes'], section['size'])
        self.assertEqual(result['rows'][-1]['category'], 'vtable')
        self.assertTrue(all(r['owner'] is None for r in result['rows']))

    def test_partial_section_gap_and_unknown_sizes_are_not_guessed(self):
        result = self.ledger([(0x1000, 'table'), (0x1010, 'unknown')])
        data = next(s for s in result['sections'] if s['name'] == '.data')
        self.assertEqual(data['gaps'], [{'address': 0x1010, 'size': 16}])
        self.assertIsNone(result['rows'][1]['size'])
        self.assertEqual(result['rows'][1]['size_status'], 'missing_symbol')

    def test_equal_bytes_never_claim_target_or_runtime_verification(self):
        result = self.ledger(rebuilt=True)
        for row in result['rows']:
            compare = row['comparison']
            self.assertFalse(compare['target_verified'])
            self.assertEqual(compare['status'], 'file_bytes_unavailable' if row['name'] == 'zero'
                             else 'raw_equal_targets_unverified')
        self.assertEqual(result['summary']['target_verified_count'], 0)
        self.assertEqual(result['summary']['runtime_verified_count'], 0)
        self.assertIsNone(result['provenance']['rebuilt_source_revision'])

    def test_raw_difference_and_missing_rebuilt_symbol(self):
        write_data_elf(self.rebuilt, data=b'X' * 32, symbols=[('table', 1, 0x1000, 16, 0x11)])
        result = self.ledger([(0x1000, 'table'), (0x1000, 'alias')], rebuilt=True)
        self.assertEqual(result['rows'][0]['comparison']['status'], 'raw_different')
        self.assertFalse(result['rows'][0]['comparison']['raw_equal'])
        self.assertEqual(result['rows'][1]['comparison']['status'], 'missing_rebuilt_symbol')

    def test_reused_name_at_multiple_addresses_fails_closed(self):
        write_data_elf(self.original, symbols=[('table', 1, 0x1000, 8, 0x11),
                                               ('table', 1, 0x1010, 8, 0x11)])
        write_data_elf(self.rebuilt, symbols=[('table', 1, 0x1000, 8, 0x11)])
        result = self.ledger([(0x1000, 'table')], rebuilt=True)
        self.assertEqual(result['rows'][0]['comparison']['status'], 'ambiguous_original_identity')
        self.assertIsNone(result['rows'][0]['comparison']['raw_equal'])

    def test_conflicting_and_zero_sizes_remain_unresolved(self):
        write_data_elf(self.original, symbols=[('table', 1, 0x1000, 8, 0x11),
                                               ('table', 1, 0x1000, 16, 0x11),
                                               ('zero_size', 1, 0x1010, 0, 0x11)])
        result = self.ledger([(0x1000, 'table'), (0x1010, 'zero_size')], rebuilt=True)
        self.assertEqual([r['size_status'] for r in result['rows']], ['conflicting_sizes', 'zero_size'])
        self.assertTrue(all(r['size'] is None for r in result['rows']))
        self.assertTrue(all(r['comparison']['status'] == 'original_extent_unresolved' for r in result['rows']))

    def test_nss_size_requires_corroborating_original_bytes(self):
        write_data_elf(self.original, symbols=[])
        nss = write_data_elf(self.root / 'symbols.nss')
        result = self.ledger([(0x1000, 'table'), (0x2000, 'zero')], nss=nss)
        self.assertEqual(result['rows'][0]['size'], 16)
        self.assertIsNone(result['rows'][1]['size'])
        write_data_elf(nss, data=b'X' * 32)
        result = self.ledger([(0x1000, 'table')], nss=nss)
        self.assertIsNone(result['rows'][0]['size'])

    def test_linkage_import_and_lifecycle_evidence_survives(self):
        write_data_elf(self.original, relocations=[(0x3000, 1027, 0, 0x1234),
                                                   (0x3010, 1026, 5, 0)])
        result = self.ledger()
        linkage = result['linkage']['original']
        external = next(s for s in linkage['imports'] if s['name'] == 'external')
        self.assertFalse(external['defined'])
        self.assertEqual([a['kind'] for a in linkage['arrays']], ['init', 'fini'])
        self.assertEqual(linkage['relocations'][1]['symbol_name'], 'external')
        self.assertFalse(result['summary']['runtime_verified_count'])

    def test_rebuilt_symbol_in_wrong_section_cannot_claim_equality(self):
        write_data_elf(self.rebuilt, symbols=[('table', 2, 0x1000, 16, 0x11)])
        result = self.ledger([(0x1000, 'table')], rebuilt=True)
        self.assertIsNone(result['rows'][0]['comparison']['raw_equal'])
        self.assertNotEqual(result['rows'][0]['comparison']['status'], 'raw_equal_targets_unverified')

    def test_changed_relocation_target_does_not_become_verified(self):
        # Both pointer storage regions are zero before loader relocations.
        write_data_elf(self.original, relocations=[(0x1000, 1027, 0, 0x2000)])
        write_data_elf(self.rebuilt, relocations=[(0x1000, 1027, 0, 0x3000)])
        result = self.ledger([(0x1000, 'table')], rebuilt=True)
        comparison = result['rows'][0]['comparison']
        self.assertEqual(comparison['status'], 'raw_equal_targets_unverified')
        self.assertFalse(comparison['target_verified'])
        self.assertNotEqual(result['linkage']['original']['relocations'][0]['target'],
                            result['linkage']['rebuilt']['relocations'][0]['target'])

    def test_tls_offset_is_never_counted_as_virtual_address_coverage(self):
        write_data_elf(self.original, symbols=[('tls_value', 1, 0x1000, 8, 0x16)],
                       section_overrides={1: {2: 0x403}})
        result = self.ledger([(0x1000, 'tls_value')])
        self.assertEqual(result['rows'][0]['category'], 'tls')
        self.assertIsNone(result['rows'][0]['size'])
        self.assertEqual(result['summary']['covered_bytes'], 0)

    def test_identical_symbol_table_entries_do_not_create_false_ambiguity(self):
        symbol = ('table', 1, 0x1000, 16, 0x11)
        write_data_elf(self.rebuilt, symbols=[symbol, symbol])
        result = self.ledger([(0x1000, 'table')], rebuilt=True)
        self.assertEqual(result['rows'][0]['comparison']['status'], 'raw_equal_targets_unverified')

    def test_nss_tls_offset_does_not_establish_extent(self):
        write_data_elf(self.original, symbols=[])
        nss = write_data_elf(self.root / 'symbols.nss',
                             symbols=[('tls_value', 1, 0x1000, 8, 0x16)],
                             section_overrides={1: {2: 0x403}})
        result = self.ledger([(0x1000, 'tls_value')], nss=nss)
        self.assertEqual(result['rows'][0]['category'], 'tls')
        self.assertIsNone(result['rows'][0]['size'])
        self.assertEqual(result['summary']['covered_bytes'], 0)

    def run_cli(self, output):
        return subprocess.run([sys.executable, str(Path(__file__).with_name('data_coverage.py')),
                               '--csv', str(self.csv), '--elf', str(self.original),
                               '--load-bias', '0', '--output', str(output)],
                              text=True, capture_output=True)

    def test_cli_json_and_gzip_are_repeatable(self):
        self.ledger()
        for suffix in ('.json', '.json.gz'):
            output = self.root / ('ledger' + suffix)
            with self.subTest(suffix=suffix):
                run = self.run_cli(output)
                self.assertEqual(run.returncode, 0, run.stderr)
                first = output.read_bytes()
                run = self.run_cli(output)
                self.assertEqual(run.returncode, 0, run.stderr)
                self.assertEqual(output.read_bytes(), first)
                payload = gzip.decompress(first) if suffix.endswith('gz') else first
                self.assertEqual(json.loads(payload)['summary']['row_count'], 6)

    def test_cli_refuses_direct_symlink_and_hardlink_input_output_aliases(self):
        self.ledger()
        for source in (self.csv, self.original):
            original = source.read_bytes()
            symlink = self.root / (source.name + '.symlink')
            symlink.symlink_to(source)
            hardlink = self.root / (source.name + '.hardlink')
            hardlink.hardlink_to(source)
            for output in (source, symlink, hardlink):
                with self.subTest(source=source.name, output=output.name):
                    run = self.run_cli(output)
                    self.assertNotEqual(run.returncode, 0)
                    self.assertIn('must not overwrite', run.stderr)
                    self.assertEqual(source.read_bytes(), original)

    def test_invalid_csv_rejected(self):
        self.csv.write_text('Address,Name\nnot_an_address,table\n')
        with self.assertRaises(ValueError):
            build_ledger(self.csv, self.original, load_bias=0)
        self.csv.write_text('Wrong,Columns\n1,2\n')
        with self.assertRaises(ValueError):
            build_ledger(self.csv, self.original, load_bias=0)


if __name__ == '__main__':
    unittest.main()
