"""Fail-closed checks for the independent DWARF regeneration proof."""
import argparse
import contextlib
import io
import json
from pathlib import Path
import tempfile
import unittest
from unittest import mock

import verify_dwarf_reproducibility as proof


class ArtifactComparisonTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.first = Path(self.temp.name) / 'first'
        self.second = Path(self.temp.name) / 'second'
        for directory in (self.first, self.second):
            directory.mkdir()
            for name in ('inline_helpers.csv', 'helper_ledger.csv', 'counts.json'):
                (directory / name).write_text('same\n')
            (directory / 'headers').mkdir()
            (directory / 'headers' / 'A.h').write_text('struct A {};\n')
            (directory / 'headers.json').write_text(json.dumps(proof.header_manifest(directory / 'headers')))

    def test_identical_artifacts_pass(self):
        self.assertEqual(proof.compare(self.first, self.second), {})

    def test_each_changed_inventory_fails(self):
        for name in ('inline_helpers.csv', 'helper_ledger.csv', 'counts.json'):
            with self.subTest(name=name):
                path = self.second / name
                path.write_text('changed\n')
                self.assertIn(name, proof.compare(self.first, self.second))
                path.write_text('same\n')

    def test_missing_inventory_never_passes(self):
        (self.second / 'inline_helpers.csv').unlink()
        with self.assertRaises(FileNotFoundError):
            proof.compare(self.first, self.second)

    def test_changed_and_missing_header_entries_fail(self):
        manifest = self.second / 'headers.json'
        manifest.write_text(json.dumps({'B.h': 'different'}))
        differences = proof.compare(self.first, self.second)
        self.assertTrue(differences)

    def test_manifest_tracks_paths_and_content(self):
        first = proof.header_manifest(self.first / 'headers')
        (self.first / 'headers' / 'A.h').rename(self.first / 'headers' / 'B.h')
        second = proof.header_manifest(self.first / 'headers')
        self.assertEqual(list(first), ['A.h'])
        self.assertEqual(list(second), ['B.h'])
        self.assertEqual(first['A.h'], second['B.h'])

    def test_stale_manifest_cannot_hide_changed_or_deleted_headers(self):
        header = self.second / 'headers' / 'A.h'
        header.write_text('changed after manifest generation')
        self.assertIn('second_header_manifest', proof.compare(self.first, self.second))
        header.unlink()
        self.assertIn('second_header_manifest', proof.compare(self.first, self.second))

    def test_identically_stale_manifests_cannot_pass(self):
        for directory in (self.first, self.second):
            (directory / 'headers' / 'A.h').unlink()
        self.assertTrue(proof.compare(self.first, self.second))


class CommandTests(unittest.TestCase):
    def test_provenance_failure_retains_failed_report(self):
        with tempfile.TemporaryDirectory() as temporary:
            output = Path(temporary) / 'proof'
            with mock.patch.object(proof.subprocess, 'check_output', side_effect=FileNotFoundError('missing tool')), contextlib.redirect_stdout(io.StringIO()):
                self.assertEqual(proof.run(argparse.Namespace(output=output, workers=[1, 3])), 1)
            report = json.loads((output / 'report.json').read_text())
            self.assertFalse(report['passed'])
            self.assertIn('missing tool', report['error'])
            self.assertEqual(report['runs'], [])

    def test_invalid_worker_counts_rejected_before_any_build(self):
        for counts in (['1'], ['0', '2'], ['-1', '3']):
            with self.subTest(counts=counts), mock.patch('sys.argv', ['proof', '--output', '/unused', '--workers', *counts]), mock.patch.object(proof, 'run') as run, contextlib.redirect_stderr(io.StringIO()):
                with self.assertRaises(SystemExit) as result:
                    proof.main()
                self.assertEqual(result.exception.code, 2)
                run.assert_not_called()

    def test_failed_index_stage_retains_report_and_log(self):
        with tempfile.TemporaryDirectory() as temporary:
            output = Path(temporary) / 'proof'
            args = argparse.Namespace(output=output, workers=[1, 3])
            with mock.patch.object(proof, 'sha', return_value='test-hash'), mock.patch.object(proof.subprocess, 'check_output', return_value='test-version\n'), mock.patch.object(proof.subprocess, 'run', return_value=mock.Mock(returncode=17)), contextlib.redirect_stdout(io.StringIO()):
                self.assertEqual(proof.run(args), 1)
            report = json.loads((output / 'report.json').read_text())
            self.assertFalse(report['passed'])
            self.assertIn('index failed', report['error'])
            self.assertEqual(len(report['runs']), 1)
            stage, = report['runs'][0]['stages']
            self.assertEqual(stage['exit_code'], 17)
            self.assertTrue((output / stage['log']).is_file())

    def test_existing_output_is_not_overwritten(self):
        with tempfile.TemporaryDirectory() as temporary:
            output = Path(temporary)
            marker = output / 'keep'
            marker.write_text('existing evidence')
            with self.assertRaises(FileExistsError):
                proof.run(argparse.Namespace(output=output, workers=[1, 3]))
            self.assertEqual(marker.read_text(), 'existing evidence')


if __name__ == '__main__':
    unittest.main()
