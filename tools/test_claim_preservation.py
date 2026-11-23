"""A stuck claim must never delete an apparently clean evidence worktree."""
import contextlib
import csv
import io
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest
from unittest import mock

from tools import claim


class ClaimEvidencePreservationTests(unittest.TestCase):
    def test_stuck_keeps_clean_worktree_and_ignored_evidence(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory).resolve()

            def git(*args):
                return subprocess.run(['git', *args], cwd=root, check=True,
                                      capture_output=True, text=True).stdout.strip()

            git('init', '-q')
            git('config', 'user.name', 'Claim Test')
            git('config', 'user.email', 'claim-test@example.invalid')
            (root / '.gitignore').write_text('build/\n')
            (root / 'source.cpp').write_text('int original;\n')
            git('add', '.gitignore', 'source.cpp')
            git('commit', '-qm', 'test baseline')

            owner = 'X/Client/Test.cpp'
            wt = root / 'build/work' / claim.slug(owner)
            wt.parent.mkdir(parents=True)
            git('worktree', 'add', '-qb', claim.branch(owner), str(wt))
            evidence = wt / 'build/private-evidence/measurement.json'
            evidence.parent.mkdir(parents=True)
            original = b'{"sample": 123}\n'
            evidence.write_bytes(original)
            self.assertEqual(git('-C', str(wt), 'status', '--porcelain'), '')

            claims = root / 'claims.csv'
            with claims.open('w', newline='') as stream:
                writer = csv.DictWriter(stream, claim.FIELDS)
                writer.writeheader()
                writer.writerow({'File': owner, 'State': 'claimed', 'Who': 'worker',
                                 'When': '2026-11-02 00:00', 'Note': ''})
            argv = ['claim.py', 'stuck', owner, 'needs evidence review']
            with mock.patch.object(claim, 'MAIN', root), mock.patch.object(claim, 'CLAIMS', claims), \
                 mock.patch.object(sys, 'argv', argv), contextlib.redirect_stdout(io.StringIO()):
                claim.main()

            self.assertTrue(wt.is_dir())
            self.assertEqual(evidence.read_bytes(), original)
            self.assertEqual(git('-C', str(wt), 'status', '--porcelain'), '')
            self.assertIn(claim.branch(owner), git('branch', '--list', claim.branch(owner)))
            with claims.open(newline='') as stream:
                rows = list(csv.DictReader(stream))
            self.assertEqual(len(rows), 1)
            self.assertEqual(rows[0]['File'], owner)
            self.assertEqual(rows[0]['State'], 'stuck')
            self.assertEqual(rows[0]['Who'], 'worker')
            self.assertEqual(rows[0]['Note'], 'needs evidence review')


if __name__ == '__main__':
    unittest.main()
