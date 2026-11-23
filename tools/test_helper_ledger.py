"""Strict evidence and non-additive fan-in regression tests for helper families."""
import contextlib
import io
from pathlib import Path
import tempfile
import unittest

import helper_ledger as ledger


class StrictHelperEvidenceTests(unittest.TestCase):
    def rows(self, reverse=False):
        keys = [('X/H.h', 10, 'Get'), ('X/H.h', 10, 'Other'),
                ('X/Static.cpp', 4, 'One'), ('X/Static.cpp', 5, 'Two')]
        if reverse:
            keys.reverse()
        index = {'inlined': {}, 'funcs': {}}
        for key in keys:
            index['inlined'][key] = ({0x100, 0x200, 0x300, 0x400, 0x500}, 8)
            index['funcs'][key] = {'name': key[2], 'scope': ('Vector<int>',),
                                   'ret': 'int', 'params': [], 'const': False}
        return ledger.build(index, {0x100: 'O', 0x200: 'M', 0x300: 'm', 0x400: 'U'},
                            {a: 'X/Caller.cpp' for a in range(0x100, 0x600, 0x100)},
                            {'X/Caller.cpp': (5, 100)}, src=Path('/nonexistent'), bodies=False)

    def test_only_original_matches_are_evidence(self):
        for row in self.rows():
            self.assertEqual((row['Callers'], row['MatchedCallers'], row['UnmatchedCallers']), (5, 1, 4))
            self.assertEqual(row['MatchedCallerAddresses'], '0x00000100')
            self.assertEqual(row['UnmatchedCallerAddresses'], '0x00000200;0x00000300;0x00000400;0x00000500')
            self.assertIn('instantiations unresolved', row['EvidenceScope'])

    def test_union_does_not_add_overlapping_callers(self):
        self.assertEqual(ledger.caller_union(self.rows()), {0x200, 0x300, 0x400, 0x500})
        rows = self.rows()
        for row in rows:
            row['MatchedCallers'] = 0
        output = io.StringIO()
        with contextlib.redirect_stdout(output):
            ledger.print_summary(rows, min_callers=1)
        self.assertIn('4 distinct unmatched callers observed', output.getvalue())
        self.assertNotIn('proven', output.getvalue())
        self.assertRegex(output.getvalue(), r'2\s+4\s+X/Static.cpp')

    def test_address_lists_and_rows_are_deterministic_across_index_order(self):
        with tempfile.TemporaryDirectory() as tmp:
            first, second = Path(tmp) / 'first.csv', Path(tmp) / 'second.csv'
            ledger.write(self.rows(), first)
            ledger.write(self.rows(reverse=True), second)
            self.assertEqual(first.read_bytes(), second.read_bytes())
            self.assertEqual(ledger.caller_union(ledger.read(first)), {0x200, 0x300, 0x400, 0x500})

    def test_legacy_counts_do_not_fabricate_unique_totals(self):
        with self.assertRaisesRegex(ValueError, 'regenerate'):
            ledger.caller_union([{'UnmatchedCallers': 5}])

    def test_normalizes_and_deduplicates_binary_addresses(self):
        key = ('X/H.h', 1, 'f')
        index = {'inlined': {key: ({0x100, 0x100000100}, 2)},
                 'funcs': {key: {'name': 'f', 'scope': (), 'ret': 'void', 'params': [], 'const': False}}}
        row, = ledger.build(index, {0x100: 'O'}, {0x100: 'X/C.cpp'}, {'X/C.cpp': (1, 4)}, bodies=False)
        self.assertEqual(row['Callers'], 1)
        self.assertEqual(row['MatchedCallers'], 1)
        self.assertEqual(row['CallerAddresses'], '0x00000100')


if __name__ == '__main__':
    unittest.main()
