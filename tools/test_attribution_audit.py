"""Conservation and negative cases for the unowned-function audit."""
import copy
import json
from pathlib import Path
import subprocess
import tempfile
import unittest

import attribution_audit as audit


def fn(address, name='FMOD_Test', quality='U', size=16):
    return dict(Address=hex(address), Name=name, Quality=quality, Size=str(size))


def rules():
    return {'schema_version': 1, 'rules': [
        {'id': 'P03-C01', 'component': 'FMOD', 'name_patterns': ['^FMOD_'],
         'confidence': 'symbol-family', 'evidence': 'Exact exported namespace; owner unproved.',
         'follow_up': 'P03-F01'}]}


class ReconciliationTests(unittest.TestCase):
    def test_all_quality_labels_and_unknowns_conserve_inventory(self):
        functions = [fn(0x7100001000 + 16*i, quality=q, size=4*(i+1))
                     for i, q in enumerate(('O', 'M', 'm', 'W', 'U'))]
        functions += [fn(0x7100010000, 'FUN_7100010000', size=24),
                      fn(0x7100020000, 'owned', quality='O', size=8)]
        owners = [{'Address': '0x7100020000', 'File': 'X/Test.cpp'}]
        original = copy.deepcopy((functions, owners))
        result = audit.reconcile(functions, owners, rules())
        records = result['records']
        self.assertEqual(len(records), 6)
        self.assertEqual(sum(r['bytes'] for r in records), 84)
        self.assertEqual(sum(r['quality'] == 'O' for r in records), 1)
        self.assertEqual(len({r['address'] for r in records}), 6)
        self.assertEqual(sum(r['functions'] for r in result['bins']), 6)
        self.assertEqual(sum(r['bytes'] for r in result['components']), 84)
        for row in records:
            self.assertIsNone(row['source_owner'])
            self.assertEqual(row['ownership_status'], 'unresolved')
            self.assertFalse(row['excluded'])
            self.assertTrue(row['follow_up'])
        self.assertEqual((functions, owners), original)
        self.assertIsNone(records[-1]['rule_id'])

    def test_order_and_address_format_do_not_change_identity(self):
        functions = [fn(0x7100010010, 'other'), fn(0x7100001000)]
        a = audit.reconcile(functions, [], rules())
        b = audit.reconcile(list(reversed(functions)), [], rules())
        self.assertEqual(a, b)
        self.assertEqual(a['records'][0]['address'], '0x0000007100001000')
        self.assertEqual(a['records'][0]['bin_id'], 'BIN-710000')
        self.assertEqual(a['records'][1]['bin_id'], 'BIN-710001')

    def test_duplicate_inventory_address_is_rejected(self):
        first = fn(0x7100001000)
        duplicate = {**first, 'Address': '0x0000007100001000'}
        for second in (duplicate, {**duplicate, 'Name': 'different'}):
            with self.assertRaises(ValueError):
                audit.reconcile([first, second], [], rules())

    def test_conflicting_owner_or_orphan_owner_is_rejected(self):
        owners = [{'Address': '0x7100001000', 'File': 'X/A.cpp'},
                  {'Address': '0x0000007100001000', 'File': 'X/B.cpp'}]
        with self.assertRaises(ValueError):
            audit.reconcile([fn(0x7100001000)], owners, rules())
        with self.assertRaises(ValueError):
            audit.reconcile([fn(0x7100001000)], [{'Address': '0x7100002000', 'File': 'X/A.cpp'}], rules())

    def test_ambiguous_rules_fail_instead_of_first_match_wins(self):
        config = rules()
        config['rules'].append({**config['rules'][0], 'id': 'P03-C02', 'component': 'Other'})
        with self.assertRaises(ValueError):
            audit.reconcile([fn(0x7100001000)], [], config)

    def test_malformed_inventory_is_rejected(self):
        for change in ({'Size': '-1'}, {'Address': '-1'}, {'Quality': 'done'}, {'Size': 'abc'}):
            with self.subTest(change=change), self.assertRaises(ValueError):
                audit.reconcile([{**fn(0x7100001000), **change}], [], rules())

    def test_empty_inventory_and_blank_ownership(self):
        self.assertEqual(audit.reconcile([], [], rules())['records'], [])
        functions = [fn(0x7100001000)]
        blank = [{'Address': '0x7100001000', 'File': ''}]
        self.assertEqual(audit.reconcile(functions, blank, rules())['records'],
                         audit.reconcile(functions, [], rules())['records'])

    def test_binary_evidence_rejects_conflicting_symbols_and_missing_extents(self):
        records = audit.reconcile([fn(0x7100001000)], [], rules())['records']
        evidence = {'images':[{'image':'nss'}], 'functions':[{
            'address':0x7100001000, 'size':16,
            'sections':[{'image':'nss', 'contains_extent':True}],
            'symbols':[{'image':'nss', 'type':2, 'name':'FMOD_Test', 'size':16}]}]}
        self.assertEqual(audit.corroborate_binary(records, evidence)['images'][0]['exact_name_and_size'], 1)
        for field, value in (('name','different'), ('size',8)):
            bad = copy.deepcopy(evidence)
            bad['functions'][0]['symbols'][0][field] = value
            with self.assertRaises(ValueError):
                audit.corroborate_binary(records, bad)
        bad = copy.deepcopy(evidence)
        bad['functions'][0]['sections'][0]['contains_extent'] = False
        with self.assertRaises(ValueError):
            audit.corroborate_binary(records, bad)
        for rows in ([], evidence['functions'] * 2):
            with self.assertRaises(ValueError):
                audit.corroborate_binary(records, {**evidence, 'functions':rows})

    def test_placeholder_aliases_do_not_replace_inventory_identity(self):
        records = audit.reconcile([fn(0x7100001000, 'FUN_00001000')], [], rules())['records']
        evidence = {'images':[{'image':'nss'}], 'functions':[{
            'address':0x7100001000, 'size':16,
            'sections':[{'image':'nss', 'contains_extent':True}],
            'symbols':[{'image':'nss', 'type':2, 'name':'static_helper', 'size':16}]}]}
        before = copy.deepcopy(records)
        self.assertEqual(audit.corroborate_binary(records, evidence)['images'][0]['placeholder_with_size_match'], 1)
        self.assertEqual(records, before)

    def test_snapshot_reads_committed_inventory_and_preserves_worktree(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            def git(*args):
                return subprocess.check_output(['git', '-C', str(root), *args], text=True)
            git('init', '-q')
            data = root / 'data/2.6.2'; data.mkdir(parents=True)
            functions = data / 'functions.csv'
            functions.write_text('Address,Quality,Size,Name\n0x7100001000,U,16,FMOD_Test\n')
            (data / 'function_files.csv').write_text('Address,File,Compiler\n0x7100001000,,\n')
            rule_path = root / 'config/component_rules.json'
            rule_path.parent.mkdir(parents=True)
            rule_path.write_text(json.dumps(rules()))
            (root/'tools').mkdir()
            (root/'tools/setup.py').write_text("ELF_SHA256 = '" + '0'*64 + "'\n")
            git('add', '.')
            git('-c', 'user.name=Test', '-c', 'user.email=test@example.test', 'commit', '-qm', 'fixture')
            functions.write_text('dirty live inventory is deliberately invalid\n')
            before = git('status', '--porcelain')
            result = audit.snapshot(root=root, rules_path=rule_path)
            self.assertEqual(len(result['records']), 1)
            self.assertEqual(result['records'][0]['quality'], 'U')
            self.assertEqual(git('status', '--porcelain'), before)
            self.assertEqual(functions.read_text(), 'dirty live inventory is deliberately invalid\n')
            from test_checker import write_elf, words, RET
            binary = root/'fixture.elf'
            write_elf(binary, [('FMOD_Test', 0x1000, words(RET)*4)])
            with self.assertRaisesRegex(ValueError, 'ELF SHA256 differs'):
                audit.snapshot(root=root, rules_path=rule_path, elf=binary)


if __name__ == '__main__':
    unittest.main()
