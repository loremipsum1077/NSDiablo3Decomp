"""The landing gate: real differences fail, proof gaps do not, labels follow the checker."""
import json
import tempfile
import unittest
from pathlib import Path
from unittest.mock import patch

import land

A, B, C, D = ('0x000000710000100%d' % i for i in range(4))
FILE = 'X/Test.cpp'
GAP = 'mismatch at 0x1000: unverified GOT reference: rebuilt undefined GOT target symbol'
REAL = 'mismatch at 0x1000: wrong function size'


class LandingGateTests(unittest.TestCase):
    def check(self, before, verdicts, objects, files=(FILE,)):
        """Run land.check with a fake build, checker and try_file."""
        rows = [{'Address': a, 'Quality': q, 'Size': '4', 'Name': 'f' + a[-1]} for a, q in before.items()]
        with tempfile.TemporaryDirectory() as temp:
            temp = Path(temp)
            (temp / 'build').mkdir()
            functions = temp / 'functions.csv'
            functions.write_text('Address,Quality,Size,Name\n')

            def run(*args, **kwargs):
                if args[0] == str(temp / 'tools/check'):
                    Path(args[4]).write_text(json.dumps({'format': 1, 'results': [
                        {'address': a, 'name': 'f' + a[-1], 'status': s, 'reason': r}
                        for a, (s, r) in verdicts.items()]}))
                return 0, ''
            with patch.object(land, 'MAIN', temp), patch.object(land, 'FUNCTIONS', functions), \
                 patch.object(land, 'inventory', return_value=rows), \
                 patch.object(land, 'owners', return_value={a: FILE for a in before}), \
                 patch.object(land, 'object_matches', return_value=(objects, '')), \
                 patch.object(land, 'run', side_effect=run):
                return land.check(list(files))

    def test_real_difference_in_a_matched_function_fails(self):
        error, quality, _ = self.check({A: 'O'}, {A: ('mismatch', REAL)}, {A: 'mismatch'})
        self.assertIn('no longer', error)
        self.assertEqual(quality, {A: 'O'})

    def test_missing_matched_symbol_fails(self):
        error, _, _ = self.check({A: 'O'}, {A: ('missing', '')}, {})
        self.assertIn('no longer', error)

    def test_labels_follow_checker_and_object_comparison(self):
        error, quality, summary = self.check(
            {A: 'U', B: 'U', C: 'U', D: 'O'},
            {A: ('ok', ''), B: ('mismatch', GAP), C: ('mismatch', REAL), D: ('mismatch', GAP)},
            {A: 'match', B: 'match', C: 'mismatch', D: 'match'})
        self.assertIsNone(error)
        self.assertEqual(quality, {A: 'O', B: 'O', C: 'M', D: 'O'})
        self.assertIn('+2 functions newly match', summary)

    def test_proof_gap_needs_equal_instructions(self):
        _, quality, _ = self.check({A: 'U'}, {A: ('mismatch', GAP)}, {A: 'mismatch'})
        self.assertEqual(quality, {A: 'M'})

    def test_same_named_local_copy_is_a_proof_gap(self):
        same = ('mismatch at 0x1: wrong data reference\nincorrect reference; expected to see 0x10 K (_ZL1K)\n'
                '--> decomp source code is referencing 0x20 K (_ZL1K)\n')
        other = same.replace('0x20 K (_ZL1K)', '0x20 J (_ZL1J)')
        self.assertIsNone(self.check({A: 'O'}, {A: ('mismatch', same)}, {A: 'match'})[0])
        self.assertIn('no longer', self.check({A: 'O'}, {A: ('mismatch', other)}, {A: 'match'})[0])

    def test_unwritten_function_stays_unwritten(self):
        _, quality, _ = self.check({A: 'U'}, {A: ('missing', '')}, {A: 'missing'})
        self.assertEqual(quality, {A: 'U'})


if __name__ == '__main__':
    unittest.main()
