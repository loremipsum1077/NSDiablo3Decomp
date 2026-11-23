"""Engineering safeguards remain available without assistant-specific wiring."""
import json
from pathlib import Path
import subprocess
import sys
import unittest


class EngineeringGuardTests(unittest.TestCase):
    def check(self, tool, payload):
        result = subprocess.run(
            [sys.executable, str(Path(__file__).with_name('hooks') / 'guard.py')],
            input=json.dumps({'tool_name': tool, 'tool_input': payload}),
            text=True, capture_output=True, check=True)
        return json.loads(result.stdout)['hookSpecificOutput']['permissionDecision'] if result.stdout else 'allow'

    def test_engineering_boundaries(self):
        cases = [
            ('Edit', {'file_path': '/repo/data/2.6.2/functions.csv', 'new_string': 'O'}),
            ('Write', {'file_path': '/repo/tools/common/check.rs', 'content': 'replacement'}),
            ('Edit', {'file_path': '/repo/src/X/Owner.cpp', 'new_string': 'register int v asm("w1");'}),
            ('Bash', {'command': 'git commit --no-verify'}),
            ('Bash', {'command': 'git push --force origin dev'}),
        ]
        for tool, payload in cases:
            with self.subTest(tool=tool, payload=payload):
                self.assertEqual(self.check(tool, payload), 'deny')

    def test_ordinary_source_edit_is_allowed(self):
        self.assertEqual(self.check('Edit', {'file_path': '/repo/src/X/Owner.cpp',
                                           'new_string': 'return count + 1;'}), 'allow')


if __name__ == '__main__':
    unittest.main()
