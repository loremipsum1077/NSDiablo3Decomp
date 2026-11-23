import contextlib
import io
from pathlib import Path
import subprocess
import tempfile
import unittest
from unittest.mock import patch

import try_file


class CompileDiagnosticsTests(unittest.TestCase):
    def report(self, returncode, stdout, stderr, object_exists, warnings=False):
        output = io.StringIO()
        result = subprocess.CompletedProcess(['compiler'], returncode, stdout, stderr)
        with contextlib.redirect_stderr(output):
            ok = try_file.report_compile_result(result, object_exists, warnings)
        return ok, output.getvalue()

    def test_infrastructure_failure_keeps_unfiltered_output_and_status(self):
        ok, output = self.report(125, '', 'permission denied while connecting to daemon\n', False)
        self.assertFalse(ok)
        self.assertIn('status 125', output)
        self.assertIn('permission denied while connecting to daemon\n', output)

    def test_compiler_failure_preserves_both_streams_even_if_object_exists(self):
        ok, output = self.report(1, 'compiler context\n', 'file.c:1: error: bad type\n  detail\n', True)
        self.assertFalse(ok)
        self.assertIn('compiler context\n', output)
        self.assertIn('file.c:1: error: bad type\n  detail\n', output)

    def test_missing_object_after_success_is_failure(self):
        ok, output = self.report(0, '', 'unexpected container output', False)
        self.assertFalse(ok)
        self.assertIn('status 0; object file was not produced', output)
        self.assertIn('unexpected container output', output)

    def test_success_keeps_existing_opt_in_warning_filter(self):
        ok, quiet = self.report(0, '', 'noise\nwarning: example\n  detail\n', True)
        self.assertTrue(ok)
        self.assertEqual(quiet, '')
        ok, shown = self.report(0, '', 'noise\nwarning: example\n  detail\n', True, True)
        self.assertTrue(ok)
        self.assertEqual(shown, 'warning: example\n  detail\n')

    def test_json_failure_keeps_machine_output_separate_from_diagnostics(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory).resolve()
            source = root / 'src' / 'example.c'
            source.parent.mkdir()
            source.write_text('')
            result = subprocess.CompletedProcess(['docker'], 125, '', 'daemon unavailable\n')
            stdout, stderr = io.StringIO(), io.StringIO()
            with patch.object(try_file, 'ROOT', root), patch.object(try_file, 'compile_command', return_value=[]), \
                 patch.object(try_file.subprocess, 'run', return_value=result), \
                 patch.dict(try_file.os.environ, {'D3_VALIDATION_CONTAINER': '', 'D3_PILOT_DB': ''}), \
                 patch('sys.argv', ['try_file.py', str(source), '--json']), \
                 contextlib.redirect_stdout(stdout), contextlib.redirect_stderr(stderr):
                with self.assertRaises(SystemExit) as exit_result:
                    try_file.main()
            self.assertEqual(exit_result.exception.code, 1)
            self.assertIn('"compiled": false', stdout.getvalue())
            self.assertNotIn('daemon unavailable', stdout.getvalue())
            self.assertIn('status 125', stderr.getvalue())
            self.assertIn('daemon unavailable', stderr.getvalue())

if __name__ == '__main__':
    unittest.main()
