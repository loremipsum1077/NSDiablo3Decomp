"""Offline snapshot evidence routing tests; never run the real checker."""
import contextlib
import io
import json
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest
from unittest.mock import patch

import checker_regression as c


class CheckerRegressionTests(unittest.TestCase):
    def setUp(self):
        temporary = tempfile.TemporaryDirectory()
        self.addCleanup(temporary.cleanup)
        self.root = Path(temporary.name)
        self.build = self.root / 'build/onboarding-lua'
        self.report = self.root / '.onboarding/report.json'
        files = {
            'tools/check': 'fixture checker',
            'tools/config.toml': '[fixture]\n',
            'data/2.6.2/functions.csv': 'Address,Name,Quality,Size\n0x100,exact,O,4\n0x104,partial,M,8\n0x10c,unknown,U,12\n',
            'data/2.6.2/data_symbols.csv': 'Address,Name\n',
            'data/2.6.2/main.elf': 'original fixture',
            'build/onboarding-lua/d3': 'lua fixture ELF',
            'build/onboarding-lua/d3.map': 'lua fixture map',
            'build/onboarding-lua/d3.map.provenance': 'existing evidence; never stamp me',
            'build/2.6.2/d3': 'unrelated normal ELF',
            'build/2.6.2/d3.map': 'unrelated normal map',
            'build/2.6.2/d3.map.provenance': 'unrelated normal evidence',
        }
        for relative, contents in files.items():
            path = self.root / relative
            path.parent.mkdir(parents=True, exist_ok=True)
            path.write_text(contents)
        self.addCleanup(patch.stopall)
        patch.object(c, 'ROOT', self.root).start()
        self.git = patch.object(c.subprocess, 'check_output', return_value='fixture-head').start()
        self.run = patch.object(c.subprocess, 'run', side_effect=self.fake_checker).start()
        self.output = io.StringIO()

    def fake_checker(self, command, *, cwd, **kwargs):
        snapshot = Path(cwd)
        self.assertEqual((snapshot / 'build/2.6.2/d3').read_text(), 'lua fixture ELF')
        self.assertEqual((snapshot / 'build/2.6.2/d3.map').read_text(), 'lua fixture map')
        self.assertEqual((snapshot / 'build/2.6.2/d3.map.provenance').read_text(), 'existing evidence; never stamp me')
        self.assertFalse((snapshot / 'build/onboarding-lua').exists())
        request = json.loads((snapshot / 'request.json').read_text())
        self.assertEqual([r['name'] for r in request['functions']], ['exact', 'partial'])
        results = [dict(item, status='ok' if item['name'] == 'exact' else 'mismatch')
                   for item in request['functions']]
        (snapshot / 'result.json').write_text(json.dumps({'format': 1, 'results': results}))
        return subprocess.CompletedProcess(command, 0, '', '')

    def main(self):
        with patch.object(sys, 'argv', ['checker_regression.py', '--checker', str(self.root / 'tools/check'),
                                      '--build-dir', str(self.build), '--output', str(self.report)]), \
             contextlib.redirect_stdout(self.output):
            return c.main()

    def snapshot(self):
        return {str(p.relative_to(self.root)): p.read_bytes() for p in self.root.rglob('*') if p.is_file()}

    def test_explicit_build_directory_is_remapped_without_touching_live_inventory(self):
        before = self.snapshot()
        self.assertEqual(self.main(), 0)
        after = self.snapshot()
        self.assertEqual(before, {k: v for k, v in after.items() if k != '.onboarding/report.json'})
        report = json.loads(self.report.read_text())
        self.assertEqual(report['checked'], 2)
        self.assertEqual(report['verified_O_count'], 1)
        self.assertEqual(report['unverified_O_count'], 0)
        self.assertEqual(report['failures'], [])
        self.assertIn(c.sha256(self.build / 'd3'), report['inputs_sha256'].values())
        self.assertNotIn(c.sha256(self.root / 'build/2.6.2/d3'), report['inputs_sha256'].values())

    def test_missing_selected_build_evidence_never_falls_back_or_stamps(self):
        for name in ('d3', 'd3.map'):
            target = self.build / name
            contents = target.read_bytes()
            target.unlink()
            before = self.snapshot()
            with self.subTest(name=name), self.assertRaises((RuntimeError, OSError)):
                self.main()
            self.assertFalse(target.exists())
            self.assertEqual(self.snapshot(), before)
            self.run.assert_not_called()
            target.write_bytes(contents)

    def test_missing_provenance_is_not_fabricated_and_checker_rejection_is_reported(self):
        (self.build / 'd3.map.provenance').unlink()
        def reject_unproven(command, *, cwd, **kwargs):
            snapshot = Path(cwd)
            self.assertFalse((snapshot / 'build/2.6.2/d3.map.provenance').exists())
            request = json.loads((snapshot / 'request.json').read_text())
            results = [dict(row, status='missing_provenance') for row in request['functions']]
            (snapshot / 'result.json').write_text(json.dumps({'format': 1, 'results': results}))
            return subprocess.CompletedProcess(command, 0, '', '')
        self.run.side_effect = reject_unproven
        self.assertEqual(self.main(), 1)
        report = json.loads(self.report.read_text())
        self.assertEqual(report['verified_O_count'], 0)
        self.assertEqual(len(report['failures']), 2)
        self.assertFalse((self.build / 'd3.map.provenance').exists())

    def test_selected_source_change_is_detected_after_checker(self):
        original = self.fake_checker
        def mutate(command, **kwargs):
            result = original(command, **kwargs)
            (self.build / 'd3').write_text('changed while checker ran')
            return result
        self.run.side_effect = mutate
        with self.assertRaisesRegex(RuntimeError, 'changed'):
            self.main()
        self.assertFalse(self.report.exists())

    def test_live_inventory_change_is_detected_and_not_overwritten(self):
        original = self.fake_checker
        inventory = self.root / 'data/2.6.2/functions.csv'
        def mutate(command, **kwargs):
            result = original(command, **kwargs)
            inventory.write_text('concurrent user edit')
            return result
        self.run.side_effect = mutate
        with self.assertRaisesRegex(RuntimeError, 'changed'):
            self.main()
        self.assertEqual(inventory.read_text(), 'concurrent user edit')
        self.assertFalse(self.report.exists())

    def test_strict_exact_mismatch_produces_failed_evidence_without_csv_edits(self):
        original = self.fake_checker
        inventory = self.root / 'data/2.6.2/functions.csv'
        before = inventory.read_bytes()
        def mismatch(command, **kwargs):
            result = original(command, **kwargs)
            path = Path(kwargs['cwd']) / 'result.json'
            payload = json.loads(path.read_text())
            payload['results'][0]['status'] = 'mismatch'
            path.write_text(json.dumps(payload))
            return result
        self.run.side_effect = mismatch
        self.assertEqual(self.main(), 1)
        report = json.loads(self.report.read_text())
        self.assertEqual(report['verified_O_count'], 0)
        self.assertEqual(report['unverified_O_count'], 1)
        self.assertEqual(len(report['failures']), 1)
        self.assertEqual(inventory.read_bytes(), before)

    def test_incomplete_checker_results_cannot_produce_success_report(self):
        original = self.fake_checker
        def incomplete(command, **kwargs):
            result = original(command, **kwargs)
            (Path(kwargs['cwd']) / 'result.json').write_text(json.dumps({'format': 1, 'results': []}))
            return result
        self.run.side_effect = incomplete
        with self.assertRaisesRegex(RuntimeError, 'incomplete'):
            self.main()
        self.assertFalse(self.report.exists())


if __name__ == '__main__':
    unittest.main()
