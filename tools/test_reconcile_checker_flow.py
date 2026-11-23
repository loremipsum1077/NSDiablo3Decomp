"""Synthetic, asset-free checks for the read-only flow reconciliation audit."""
import copy
import tempfile
from pathlib import Path
import unittest

import reconcile_checker_flow as audit
from test_checker import RET, branch, words, write_elf


class ReconciliationTests(unittest.TestCase):
    def fixture(self, code, size=None):
        with tempfile.TemporaryDirectory() as temporary:
            path = Path(temporary) / 'original.elf'
            write_elf(path, [('fixture', 0x1000, code)])
            elf = path.read_bytes()
        address = hex(audit.BASE + 0x1000)
        inputs = {
            audit.INVENTORY: ('Address,Quality,Size,Name\n'
                              f'{address},O,{len(code) if size is None else size},fixture\n').encode(),
            audit.ELF: elf,
            audit.OWNERS: f'Address,File,Compiler\n{address},Example.cpp,5.0.1\n'.encode(),
        }
        report = {
            'inputs_sha256': {key: audit.digest(value) for key, value in inputs.items()},
            'checker_sha256': 'synthetic-checker',
            'failures': [{'address': address, 'name': 'fixture', 'status': 'mismatch',
                          'reason': 'mismatch at 0x1000: synthetic reason'}],
        }
        return report, inputs

    def test_branch_categories_and_signed_targets(self):
        cases = [
            ('other', words(RET), []),
            ('indirect_br', words(0xD61F0060, RET),
             [('indirect_br', '0x1000', None)]),
            ('recursive_entry_bl', words(RET, branch(0x1004, 0x1000)),
             [('recursive_entry_bl', '0x1004', '0x1000')]),
            ('interior_bl', words(branch(0x1000, 0x1008), RET, branch(0x1008, 0x1004)),
             [('interior_bl', '0x1000', '0x1008'), ('interior_bl', '0x1008', '0x1004')]),
            ('other', words(branch(0x1000, 0x0F00), RET), []),
        ]
        for category, code, branches in cases:
            with self.subTest(category=category, code=code.hex()):
                report, inputs = self.fixture(code)
                result = audit.reconcile(report, inputs)
                function = result['functions'][0]
                self.assertEqual(function['category'], category)
                self.assertEqual([(b['kind'], b['address'], b.get('target'))
                                  for b in function['branches']], branches)
                self.assertEqual(function['original_owner_candidates'], ['Example.cpp'])
                self.assertEqual(result['prior_O_failure_bytes'], len(code))
                self.assertEqual(result['categories'], {category: 1})
                self.assertEqual(result['category_original_bytes'], {category: len(code)})
                self.assertEqual(function['mismatch_instruction_word'],
                                 f'0x{int.from_bytes(code[:4], "little"):08x}')
                if category == 'indirect_br':
                    self.assertEqual(function['branches'][0]['register'], 'x3')

    def test_indirect_classification_preserves_other_branch_evidence(self):
        report, inputs = self.fixture(words(branch(0x1000, 0x1004),
                                           branch(0x1004, 0x1000), 0xD61F0000))
        function = audit.reconcile(report, inputs)['functions'][0]
        self.assertEqual(function['category'], 'indirect_br')
        self.assertEqual({b['kind'] for b in function['branches']},
                         {'interior_bl', 'recursive_entry_bl', 'indirect_br'})

    def test_pinned_inputs_must_match(self):
        for key in (audit.INVENTORY, audit.ELF):
            with self.subTest(key=key):
                report, inputs = self.fixture(words(RET))
                inputs[key] += b'\0'
                with self.assertRaisesRegex(ValueError, 'input hash does not match'):
                    audit.reconcile(report, inputs)

    def test_missing_input_pin_is_rejected(self):
        report, inputs = self.fixture(words(RET))
        del report['inputs_sha256'][audit.ELF]
        with self.assertRaisesRegex(ValueError, 'input hash does not match'):
            audit.reconcile(report, inputs)

    def test_duplicate_failure_is_rejected(self):
        report, inputs = self.fixture(words(RET))
        report['failures'].append(copy.deepcopy(report['failures'][0]))
        with self.assertRaisesRegex(ValueError, 'Duplicate failure'):
            audit.reconcile(report, inputs)

    def test_duplicate_inventory_identity_is_rejected(self):
        report, inputs = self.fixture(words(RET))
        inputs[audit.INVENTORY] += inputs[audit.INVENTORY].splitlines(keepends=True)[1]
        report['inputs_sha256'][audit.INVENTORY] = audit.digest(inputs[audit.INVENTORY])
        with self.assertRaisesRegex(ValueError, 'Duplicate inventory identity'):
            audit.reconcile(report, inputs)

    def test_malformed_function_ranges_are_rejected(self):
        for size in (0, -4, 3, 0x10000):
            with self.subTest(size=size):
                report, inputs = self.fixture(words(RET), size=size)
                with self.assertRaisesRegex(ValueError, 'Invalid or ambiguous function range'):
                    audit.reconcile(report, inputs)

    def test_ambiguous_load_segments_are_rejected(self):
        report, inputs = self.fixture(words(RET))
        segments = audit.load_segments(inputs[audit.ELF])
        with self.assertRaisesRegex(ValueError, 'Invalid or ambiguous function range'):
            audit.extract(inputs[audit.ELF], segments + segments, 0x1000, 4)


if __name__ == '__main__':
    unittest.main()
