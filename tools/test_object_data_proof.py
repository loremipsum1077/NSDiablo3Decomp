"""End-to-end synthetic ELF and adversarial witness-routing checks."""
import copy
import hashlib
import struct
import unittest
from unittest.mock import patch

import test_literal_proof as literal_fixtures
from validation import object_data, strict, literals


class ObjectDataTests(unittest.TestCase):
    def setUp(self):
        literal_fixtures.LiteralTests.setUp(self)
        (self.root/'data/2.6.2/data_symbols.csv').write_text('Address,Name\n')
        self.targets = [{'Name':'f_luaopen', 'Address':'0x7100000400', 'Size':'8'}]

    def prepare(self):
        self.original.write_bytes(self.a)
        self.object.write_bytes(self.b)
        pin = lambda p: hashlib.sha256(p.read_bytes()).hexdigest()
        context = strict.build_owner_context(self.root, literals.OWNER,
            {'original_sha256':pin(self.original), 'functions_sha256':pin(self.functions),
             'function_files_sha256':pin(self.files)})
        strict.configure(self.root)
        return (self.object, self.targets, context, pin(self.object),
                self.source, pin(self.source), self.ir, pin(self.ir))

    def test_full_regenerated_witness_resolves_pair_and_preserves_reference(self):
        args = self.prepare()
        default_before = strict.verify(self.object, self.targets)
        result = object_data.verify(*args)
        self.assertFalse(result['accepted'])
        self.assertFalse(result['candidate_linked_data_proven'])
        self.assertFalse(result['source_ir_object_lineage_proven'])
        self.assertEqual(result['reference_comparison']['results'][0]['reason'], 'unsupported_relocation')
        row = result['comparison']['results'][0]
        self.assertTrue(row['exact'])
        self.assertEqual(row['relocated_bytes_hex'], row['original_bytes_hex'])
        self.assertEqual(len(row['relocations']), 2)
        self.assertEqual(result['comparison']['object'], str(self.object))
        self.assertEqual(strict.verify(self.object, self.targets), default_before)

    def test_bad_input_pin_and_unknown_owner_rejected(self):
        args = list(self.prepare());args[3] = '0' * 64
        with self.assertRaisesRegex(ValueError, 'pin mismatch'): object_data.verify(*args)
        args = list(self.prepare());args[5] = '0' * 64
        with self.assertRaisesRegex(ValueError, 'pin mismatch'): object_data.verify(*args)
        args = list(self.prepare());args[2] = None
        with self.assertRaisesRegex(ValueError, 'context'): object_data.verify(*args)

    def test_input_changed_after_snapshot_cannot_be_saved(self):
        args = self.prepare();real = literals.prove_lstate_literal
        def change_caller(*values):
            result = real(*values)
            self.object.write_bytes(b'changed after snapshot')
            return result
        with patch.object(literals, 'prove_lstate_literal', side_effect=change_caller):
            with self.assertRaisesRegex(ValueError, 'pin mismatch'): object_data.verify(*args)

    def witness_fixture(self):
        result = object_data.verify(*self.prepare())
        row = result['reference_comparison']['results'][0]
        witness = result['data_witness']
        elf = strict.Elf(self.object)
        function = next(s for s in strict.symbols(elf).values() if s['type'] == 2)
        return row, function, 2, witness

    def test_no_unmatched_obligation_is_removed(self):
        row, function, index, witness = self.witness_fixture()
        extra = {'offset':0, 'reason':'unproved indirect control flow'}
        row['unsupported'].append(extra)
        object_data._apply_pair(row, function, index, witness)
        self.assertEqual(row['unsupported'], [extra])
        self.assertFalse(row['exact'])
        self.assertEqual(row['reason'], 'unsupported_relocation')

    def test_unrelated_instruction_mismatch_remains(self):
        row, function, index, witness = self.witness_fixture()
        row['original_size'] = function['size'] = witness['original_function_size'] = 12
        row['original_bytes_hex'] += struct.pack('<I', 0xd65f03c0).hex()
        row['candidate_bytes_hex'] += struct.pack('<I', 0xd503201f).hex()
        row['relocated_bytes_hex'] += struct.pack('<I', 0xd503201f).hex()
        object_data._apply_pair(row, function, index, witness)
        self.assertFalse(row['exact'])
        self.assertEqual(row['reason'], 'instructions')
        self.assertEqual(row['differing_offsets'], [8])

    def test_wrong_pair_identity_target_or_instruction_cannot_authorize_rewrite(self):
        original = self.witness_fixture()
        for change in ('offset','function','section','symbol','addend','duplicate','missing',
                       'target','out_of_range','candidate','original','previously_resolved'):
            row, function, index, witness = copy.deepcopy(original)
            if change == 'offset': witness['object_function_offset'] = 4
            elif change == 'function': witness['function'] = 'other'
            elif change == 'section': index = 1
            elif change == 'symbol': row['unsupported'][0]['symbol']['value'] = 4
            elif change == 'addend': row['unsupported'][0]['addend'] = 4
            elif change == 'duplicate': row['unsupported'].append(copy.deepcopy(row['unsupported'][0]))
            elif change == 'missing': row['unsupported'].pop()
            elif change == 'target': witness['original_address'] += 4
            elif change == 'out_of_range': witness['original_address'] = 1 << 40
            elif change == 'candidate': witness['object_instructions'][0] ^= 1
            elif change == 'original': witness['original_instructions'][1] ^= 1
            else: row['relocations'].append({'offset':4})
            with self.subTest(change=change), self.assertRaises(ValueError):
                object_data._apply_pair(row, function, index, witness)

    def test_zero_register_shifted_add_and_nonzero_rela_bits_rejected(self):
        original = self.witness_fixture()
        for change in ('zero','shift','immediate'):
            row, function, index, witness = copy.deepcopy(original)
            a,b = witness['object_instructions'];oa,ob = witness['original_instructions']
            if change == 'zero': a=(a&~31)|31;b=(b&~1023)|1023;oa=(oa&~31)|31;ob=(ob&~1023)|1023
            elif change == 'shift': b|=1<<22;ob|=1<<22
            else: a|=1<<29
            witness['object_instructions']=[a,b];witness['original_instructions']=[oa,ob]
            row['candidate_bytes_hex']=struct.pack('<II',a,b).hex()
            row['original_bytes_hex']=struct.pack('<II',oa,ob).hex()
            with self.subTest(change=change), self.assertRaises(ValueError):
                object_data._apply_pair(row,function,index,witness)


class TableObjectDataTests(unittest.TestCase):
    def setUp(self):
        import test_table_proof as table_fixtures
        table_fixtures.TableTests.setUp(self)
        (self.base/'data_symbols.csv').write_text('Address,Name\n')
        self.orig.write_bytes(self.a);self.obj.write_bytes(self.b)
        pin = lambda p: hashlib.sha256(p.read_bytes()).hexdigest()
        from validation import tables
        context = strict.build_owner_context(self.root, tables.OWNER,
            {'original_sha256':pin(self.orig), 'functions_sha256':pin(self.functions),
             'function_files_sha256':pin(self.files)})
        strict.configure(self.root)
        targets = [{'Name':name, 'Address':hex(address), 'Size':str(size)}
                   for name, address, size in context.members]
        self.args = (self.obj, targets, context, pin(self.obj),
                     self.owner, pin(self.owner), self.ir, pin(self.ir))

    def test_all_four_tables_recomputed_from_same_snapshot(self):
        result = object_data.verify(*self.args)
        self.assertEqual(sum(len(t['slots']) for t in result['data_witness']['tables']), 24)
        self.assertTrue(all(r['exact'] for r in result['comparison']['results']))
        self.assertTrue(all(not r['exact'] for r in result['reference_comparison']['results']))
        self.assertFalse(result['global_control_flow_proven'])
        self.assertFalse(result['receipt_acceptance'])

    def test_equal_size_table_witness_cannot_be_routed_to_different_section(self):
        from validation import tables
        real = tables.prove_lgc_tables
        def misroute(*args):
            proof = real(*args)
            proof['tables'][0]['object_section'] = proof['tables'][3]['object_section']
            return proof
        with patch.object(tables, 'prove_lgc_tables', side_effect=misroute):
            with self.assertRaisesRegex(ValueError, 'relocation identity'):
                object_data.verify(*self.args)


if __name__ == '__main__': unittest.main()
