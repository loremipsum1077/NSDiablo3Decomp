import hashlib
from pathlib import Path
import tempfile
import unittest
import sys
from unittest.mock import patch

import compiler_codegen_probe as probe


class ProbeTests(unittest.TestCase):
    def test_volatile_control_only_changes_target_load(self):
        fixture = Path(__file__).parent / 'fixtures/compiler_calibration'
        def body(name):
            return '\n'.join(line for line in (fixture / name).read_text().splitlines()
                             if not line.startswith(';'))
        baseline = body('unused_ud.ll')
        volatile = body('unused_ud_volatile.ll')
        before = '%5 = load i8*, i8** %4, align 8'
        after = '%5 = load volatile i8*, i8** %4, align 8'
        self.assertEqual(baseline.count(before), 1)
        self.assertEqual(volatile, baseline.replace(before, after, 1))

    def test_debug_variants_are_opt_in_and_leave_baseline_unchanged(self):
        flags = ['-O0', '-fast-isel=true', '-experiment-disable-o0-load-chain=true']
        normal = probe.diagnostic_variants(flags)
        debug = probe.diagnostic_variants(flags, isel_debug=True, dag_debug=True)
        self.assertNotIn('isel-debug', normal)
        self.assertNotIn('dag-debug', normal)
        self.assertEqual(debug['baseline'], flags)
        self.assertEqual(debug['isel-debug'], flags + ['-debug-only=isel'])
        self.assertEqual(debug['dag-debug'], flags + ['-debug-only=dagcombine'])
        self.assertEqual(debug['dag'], ['-O0', '-experiment-disable-o0-load-chain=true', '-fast-isel=false'])
        self.assertEqual(debug['loadchain'], ['-O0', '-fast-isel=true'])

    def test_wrong_ir_hash_never_calls_backend_or_creates_output(self):
        with tempfile.TemporaryDirectory() as temp:
            root = Path(temp).resolve()
            ir = root / 'input.ll'
            ir.write_text('fixture')
            output = root / 'build' / 'probe'
            with patch.object(probe, 'ROOT', root), patch.object(probe.backend, 'validate') as validate:
                with self.assertRaisesRegex(ValueError, 'IR pin mismatch'):
                    probe.run(ir, '0' * 64, root / 'cache', 'sha256:' + 'a' * 64, output)
            validate.assert_not_called()
            self.assertFalse(output.exists())

    def test_output_outside_build_is_rejected(self):
        with tempfile.TemporaryDirectory() as temp:
            root = Path(temp).resolve()
            ir = root / 'input.ll'
            ir.write_text('fixture')
            digest = hashlib.sha256(ir.read_bytes()).hexdigest()
            with patch.object(probe, 'ROOT', root), self.assertRaisesRegex(ValueError, 'under build'):
                probe.run(ir, digest, root / 'cache', 'sha256:' + 'a' * 64, root / 'unsafe')

    def test_oversize_ir_is_rejected_before_backend_or_output(self):
        with tempfile.TemporaryDirectory() as temp:
            root = Path(temp).resolve()
            ir = root / 'input.ll'
            ir.write_bytes(b'x' * 16)
            with patch.object(probe, 'ROOT', root), patch.object(probe, 'MAX_IR_BYTES', 8), \
                 patch.object(probe.backend, 'validate') as validate:
                with self.assertRaisesRegex(ValueError, 'IR exceeds'):
                    probe.run(ir, '0' * 64, root / 'cache', 'sha256:' + 'a' * 64, root / 'build/probe')
            validate.assert_not_called()
            self.assertFalse((root / 'build').exists())

    def test_log_overflow_and_timeout_request_container_cleanup(self):
        for command, budget, timeout in (([sys.executable, '-c', 'print("x" * 200000)'], 1024, 5),
                                         ([sys.executable, '-c', 'import time; time.sleep(5)'], 1024, .05)):
            with self.subTest(command=command), patch.object(probe, 'MAX_LOG_BYTES', budget), \
                 patch.object(probe, 'PROBE_SECONDS', timeout), patch.object(probe, 'stop_container') as stop:
                with self.assertRaises(RuntimeError):
                    probe.bounded_run(command, 'fixture-name')
                stop.assert_called_once_with('fixture-name')


if __name__ == '__main__':
    unittest.main()
