"""Core metadata/ownership boundaries without workflow packages or private inputs."""
import hashlib
import json
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest
from unittest.mock import patch

import claim
import core_api
import project_plan


class CoreApiTests(unittest.TestCase):
    def test_versioned_real_snapshot_keeps_technical_facts_only(self):
        result = core_api.snapshot(core_api.ROOT)
        self.assertEqual((result['format'], result['operation']), (1, 'snapshot'))
        self.assertEqual(len(result['functions']), result['plan']['totals']['functions'])
        self.assertNotIn('estimates', result['config'])
        self.assertTrue(all(not {'priority', 'effort', 'claim'} & row.keys()
                            for row in result['plan']['jobs']))
        self.assertEqual(result['bindings']['config_sha256'],
                         hashlib.sha256((core_api.ROOT / project_plan.CONFIG).read_bytes()).hexdigest())
        self.assertIn('not fresh', result['acceptance'])

    def test_unknown_wire_version_fails_before_metadata_access(self):
        result = subprocess.run([sys.executable, str(Path(core_api.__file__)), '--format', '99', 'snapshot'],
                                text=True, capture_output=True)
        self.assertNotEqual(result.returncode, 0)
        self.assertIn('invalid choice', result.stderr)

    def test_mid_snapshot_config_change_fails_closed(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory); (root / 'config').mkdir()
            target = root / project_plan.CONFIG; target.write_text('{}')
            def changed(_):
                target.write_text('{"changed":true}')
                return {'revision': 'a' * 40}
            with patch.object(project_plan, 'git', return_value='a' * 40), \
                    patch.object(project_plan, 'inventory', return_value=([], [])), \
                    patch.object(project_plan, 'snapshot', side_effect=changed), \
                    patch.object(project_plan, 'load_config', return_value={}):
                with self.assertRaisesRegex(ValueError, 'inputs changed'):
                    core_api.snapshot(root)


class OwnershipGuardTests(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory(); self.addCleanup(self.tmp.cleanup)
        self.root = Path(self.tmp.name).resolve(); (self.root / '.git').mkdir()
        self.queue = self.root / 'external/queue.json'; self.queue.parent.mkdir()
        self.registry = self.root / '.git/d3-coordination.json'
        self.registry.write_text(json.dumps({'format': 1, 'queue_path': str(self.queue)}))

    def test_registered_external_active_claim_blocks_manual_writer(self):
        self.queue.write_text(json.dumps({'items': {'SRC-stable': {'active': {'id': 'retained'}}}}))
        claims = self.root / 'claims.csv'; claims.write_text('File,State,Who,When,Note\n')
        before = claims.read_bytes()
        with patch.object(claim, 'MAIN', self.root), patch.object(claim, 'CLAIMS', claims):
            with self.assertRaisesRegex(RuntimeError, 'active claim'):
                with claim.locked():
                    self.fail('Blocked writer entered mutation body')
        self.assertEqual(claims.read_bytes(), before)

    def test_missing_or_invalid_registered_ledger_fails_closed(self):
        with self.assertRaisesRegex(RuntimeError, 'missing'):
            claim.check_coordination(self.root)
        for invalid in ({}, {'items': []}, {'items': {'id': None}}):
            self.queue.write_text(json.dumps(invalid))
            with self.assertRaisesRegex(RuntimeError, 'Invalid'):
                claim.check_coordination(self.root)

    def test_legacy_and_external_claims_are_both_checked(self):
        self.queue.write_text('{"items":{}}')
        old = self.root / 'build/work-queue/queue.json'; old.parent.mkdir(parents=True)
        old.write_text('{"items":{"old":{"active":true}}}')
        with self.assertRaisesRegex(RuntimeError, 'active claim'):
            claim.check_coordination(self.root)
        old.write_text('{"items":{}}'); claim.check_coordination(self.root)
        self.registry.unlink(); old.unlink(); claim.check_coordination(self.root)

    def test_controller_skips_only_own_ledger_and_blocks_foreign_claim(self):
        self.queue.write_text('{"items":{"own":{"active":true}}}')
        claim.check_coordination(self.root, selected_queue=self.queue)
        old = self.root / 'build/work-queue/queue.json'; old.parent.mkdir(parents=True)
        old.write_text('{"items":{"foreign":{"active":true}}}')
        with self.assertRaisesRegex(RuntimeError, 'active claim'):
            claim.check_coordination(self.root, selected_queue=self.queue)


if __name__ == '__main__':
    unittest.main()
