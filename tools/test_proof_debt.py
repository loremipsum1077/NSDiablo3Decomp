"""Proof-debt ledger: only O rows that are not ok, stable categories, deterministic output."""
import contextlib
import io
import json
from pathlib import Path
import tempfile
import unittest

import proof_debt as ledger


def result(rows):
    return {'format': 1, 'results': [dict(zip(('address', 'name', 'status', 'reason'), row)) for row in rows]}


INVENTORY = ('Address,Quality,Size,Name\n'
             '0x0000007100000010,O,000016,first\n'
             '0x0000007100000020,O,000016,"tmpl<a, b>"\n'
             '0x0000007100000030,M,000016,recovered\n'
             '0x0000007100000040,U,000016,open\n'
             '0x0000007100000050,O,000016,absent\n')
NORMAL = [('0x0000007100000010', 'first', 'mismatch',
           'mismatch at 0x14: unverified GOT reference: original GOT target lacks known data identity\nmore'),
          ('0x0000007100000020', 'tmpl<a, b>', 'ok', ''),
          ('0x0000007100000030', 'recovered', 'mismatch', 'mismatch at 0x30: wrong function size'),
          ('0x0000007100000050', 'absent', 'ok', '')]
LUA = [('0x0000007100000010', 'first', 'ok', ''),
       ('0x0000007100000020', 'tmpl<a, b>', 'mismatch',
        'mismatch at 0x24: unproven jump table: original jump table lacks a supported guard, base or readable target list')]


class ProofDebtTests(unittest.TestCase):
    def setUp(self):
        self.temp = Path(tempfile.mkdtemp())
        self.addCleanup(__import__('shutil').rmtree, self.temp)
        self.functions = self.temp/'functions.csv'
        self.functions.write_text(INVENTORY)
        self.normal, self.lua = self.temp/'normal.json', self.temp/'lua.json'
        self.normal.write_text(json.dumps(result(NORMAL)))
        self.lua.write_text(json.dumps(result(LUA)))

    def build(self):
        return ledger.build(self.functions, {'normal': ledger.load_results(self.normal),
                                             'lua': ledger.load_results(self.lua)})

    def test_categories_are_stable_slugs_of_the_first_line(self):
        cases = {
            'unverified GOT reference: original GOT target lacks known data identity': 'got-original-unknown',
            'unverified GOT reference: rebuilt undefined GOT target symbol': 'got-rebuilt-undefined',
            'unverified GOT reference: original undefined GOT target symbol': 'got-original-undefined',
            'unverified GOT reference: GOT target or interior offset differs for x: expected 0x1, got 0x2': 'got-target-differs',
            'unproven ADRP pair: reachable indirect jump or interior call requires unresolved control-flow edges': 'adrp-indirect-flow',
            'unproven ADRP pair: no common live page pair on every predecessor (clobber or conflicting flow)': 'adrp-conflicting-flow',
            'unproven jump table: original jump table lacks a supported guard, base or readable target list': 'jump-table-unproven',
            'unproven jump table: jump-table guard cannot be proved with unresolved indirect predecessors': 'jump-table-indirect-guard',
            'wrong function size': 'wrong-size', 'something new': 'other'}
        for text, slug in cases.items():
            self.assertEqual(ledger.category('mismatch', 'mismatch at 0x10: ' + text + '\nsecond line'), slug, text)
        self.assertEqual(ledger.category('not-checkable', 'unknown function: x'), 'not-checkable')
        self.assertEqual(ledger.category('error', ''), 'error')

    def test_only_o_rows_not_ok_with_absent_results(self):
        rows = self.build()
        self.assertEqual([(r['Name'], r['Mode'], r['Category']) for r in rows], [
            ('first', 'normal', 'got-original-unknown'),
            ('tmpl<a, b>', 'lua', 'jump-table-unproven'),
            ('absent', 'lua', 'no-result')])
        self.assertEqual(rows[0]['Reason'],
                         'mismatch at 0x14: unverified GOT reference: original GOT target lacks known data identity')

    def test_output_is_deterministic_and_round_trips(self):
        rows = self.build()
        data = ledger.render(rows)
        self.assertEqual(data, ledger.render(list(reversed(rows))))
        self.assertTrue(data.startswith(b'Address,Name,Mode,Category,Reason\n'))
        self.assertIn(b'"tmpl<a, b>"', data)
        out = self.temp/'proof_debt.csv'
        ledger.write(rows, out)
        self.assertEqual(out.read_bytes(), data)
        self.assertEqual(ledger.render(ledger.read(out)), data)

    def test_malformed_or_duplicate_results_fail(self):
        for value in ({'format': 2, 'results': []}, {'format': 1},
                      result([('0x10', 'x', 'odd', '')]), result([('10', 'x', 'ok', '')]),
                      {'format': 1, 'results': [{'address': '0x10', 'name': 'x', 'status': 'ok'}]},
                      result([('0x10', 'x', 'ok', ''), ('0x10', 'x', 'mismatch', 'y')])):
            self.normal.write_text(json.dumps(value))
            with self.assertRaises(ledger.Error):
                ledger.load_results(self.normal)
        with self.assertRaises(ledger.Error):
            ledger.debt([('0x10', 'x')], {'normal': {}})

    def test_main_writes_and_checks(self):
        out = self.temp/'proof_debt.csv'
        args = ['--normal', str(self.normal), '--lua', str(self.lua), '--functions', str(self.functions), '--output', str(out)]
        with contextlib.redirect_stdout(io.StringIO()):
            self.assertEqual(ledger.main(args + ['--check']), 1)
            self.assertEqual(ledger.main(args), 0)
            self.assertEqual(ledger.main(args + ['--check']), 0)
            out.write_text(out.read_text() + 'stale\n')
            self.assertEqual(ledger.main(args + ['--check']), 1)


if __name__ == '__main__':
    unittest.main()
