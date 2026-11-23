"""Read-only status semantics and existing receipt-verifier integration."""
import json
from pathlib import Path
import shutil
import unittest
from unittest.mock import patch
import status_report as s
import test_receipts
import receipts


class StatusTests(unittest.TestCase):
    def test_only_O_counts_and_bytes_are_function_sizes(self):
        rows = [{'Address': hex(i), 'Name': str(i), 'Size': str(i * 4), 'Quality': q}
                for i, q in enumerate(['O', 'M', 'm', 'W', 'U'], 1)]
        result = s.totals(rows)
        self.assertEqual((result['matching_functions'], result['matching_function_bytes']), (1, 4))
        self.assertEqual((result['functions'], result['original_function_bytes']), (5, 60))
        with self.assertRaisesRegex(ValueError, 'Duplicate'):
            s.totals(rows + rows[:1])

    def test_default_never_discovers_private_receipts_or_runs_builds(self):
        with patch('receipts.verify', side_effect=AssertionError('No implicit receipt discovery')):
            value = s.snapshot()
        self.assertTrue(value['modes']['normal']['available'])
        self.assertFalse(value['modes']['lua']['available'])
        self.assertEqual(value['verification']['status'], 'not_supplied')
        self.assertFalse(value['live'])
        self.assertIn('complete executable', ' '.join(value['limitations']))

    def test_both_modes_use_sealed_inventories_and_stale_source_is_explicit(self):
        f = test_receipts.ReceiptTests('test_four_fresh_mode_receipts_and_replay_do_not_accept_source')
        f.setUp(); self.addCleanup(f.doCleanups)
        builder = f.builder
        def with_inventory(source, support, dest, mode, *args, **kwargs):
            builder(source, support, dest, mode, *args, **kwargs)
            target = dest / 'data/2.6.2/functions.csv'; target.parent.mkdir(parents=True)
            shutil.copyfile(support / 'data/2.6.2/functions.csv', target)
            if mode == 'lua':
                target.write_text(target.read_text().replace('0x20,U,', '0x20,O,'))
            done = receipts.read_json(dest / 'done.json')
            done['files']['data/2.6.2/functions.csv'] = receipts.c.sha(target)
            receipts.c.write(dest / 'done.json', done)
        f.args.run = True
        receipts.build(f.args, with_inventory)
        value = s.snapshot(f.repo, f.output)
        self.assertEqual(value['verification']['source_relation'], 'same_revision')
        self.assertEqual(value['modes']['normal']['metrics']['matching_functions'], 1)
        self.assertEqual(value['modes']['lua']['metrics']['matching_functions'], 2)
        self.assertFalse(value['verification']['accepted'])
        self.assertFalse(value['verification']['fresh_build_executed'])
        real_verify = receipts.verify
        mutated = []
        def mutate_after_verify(folder):
            result = real_verify(folder)
            if not mutated:
                mutated.append(True)
                state = receipts.read_json(f.output / 'run.json')
                output = receipts.inside(f.output, state['modes']['candidate_lua']['root'])
                path = output / 'data/2.6.2/functions.csv'
                path.write_text(path.read_text().replace('0x20,O,', '0x20,U,'))
                done = receipts.read_json(output / 'done.json')
                done['files']['data/2.6.2/functions.csv'] = receipts.c.sha(path)
                receipts.c.write(output / 'done.json', done)
            return result
        state = receipts.read_json(f.output / 'run.json')
        output = receipts.inside(f.output, state['modes']['candidate_lua']['root'])
        saved_csv = (output / 'data/2.6.2/functions.csv').read_bytes()
        saved_done = (output / 'done.json').read_bytes()
        with patch.object(receipts, 'verify', side_effect=mutate_after_verify):
            with self.assertRaisesRegex(RuntimeError, 'receipt changed'):
                s.snapshot(f.repo, f.output)
        (output / 'data/2.6.2/functions.csv').write_bytes(saved_csv)
        (output / 'done.json').write_bytes(saved_done)
        f.git('commit', '--allow-empty', '-qm', 'later source boundary')
        self.assertEqual(s.snapshot(f.repo, f.output)['verification']['source_relation'], 'different_revision')
        state = receipts.read_json(f.output / 'run.json')
        output = receipts.inside(f.output, state['modes']['candidate_lua']['root'])
        (output / 'data/2.6.2/functions.csv').write_text('tampered')
        with self.assertRaises(RuntimeError):
            s.snapshot(f.repo, f.output)


if __name__ == '__main__':
    unittest.main()
