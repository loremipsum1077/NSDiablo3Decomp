"""Admission must precede all receipt and recovery integration writes."""
from contextlib import contextmanager, nullcontext
from pathlib import Path
from types import SimpleNamespace
import unittest
from unittest.mock import patch

import receipts
import recovery
import recovery_group_land
import resource_admission
from validation import common


class ResourceIntegrationTests(unittest.TestCase):
    def test_both_landing_paths_check_before_starting_transaction(self):
        for module in (recovery, recovery_group_land):
            with self.subTest(module=module.__name__), \
                 patch.object(resource_admission, 'lease', return_value=nullcontext()), \
                 patch.object(resource_admission, 'check', side_effect=resource_admission.AdmissionError('low space')), \
                 patch.object(module, '_land') as inner:
                with self.assertRaisesRegex(common.Halt, 'low space'):
                    module.land(SimpleNamespace(), Path('/tmp'))
                inner.assert_not_called()

    def test_receipt_busy_fails_before_inner_build(self):
        @contextmanager
        def busy():
            raise resource_admission.AdmissionError('lease busy')
            yield
        args = SimpleNamespace(run=True, output=Path('/tmp/output'))
        with patch.object(resource_admission, 'lease', busy), patch.object(receipts, '_build') as inner:
            with self.assertRaisesRegex(common.Halt, 'lease busy'):
                receipts.build(args)
            inner.assert_not_called()


if __name__ == '__main__':
    unittest.main()
