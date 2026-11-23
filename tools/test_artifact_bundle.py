"""Independent compact bundle recovery preserves authenticated bytes and modes."""
import contextlib
import hashlib
import json
import os
from pathlib import Path
import tempfile
import unittest
from unittest.mock import patch

import artifact_bundle as bundle
import artifact_store as archive


class BundleTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory(dir=str(Path(tempfile.gettempdir()).resolve()))
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)
        self.source = self.root / 'source'
        self.source.mkdir(mode=0o750)
        for name in ('first', 'second', 'executable'):
            (self.source / name).write_bytes(b'identical bytes')
        (self.source / 'executable').chmod(0o755)
        digest = hashlib.sha256(b'identical bytes').hexdigest()
        (self.source / 'done.json').write_text(json.dumps({'files': {x: digest for x in ('first','second','executable')}}))
        self.store = self.root / 'store'
        self.lease = patch.object(archive.resource_admission, 'lease', side_effect=contextlib.nullcontext)
        self.lease.start(); self.addCleanup(self.lease.stop)
        self.check = patch.object(archive.resource_admission, 'check')
        self.check.start(); self.addCleanup(self.check.stop)
        self.snapshot = archive.create(self.source, self.store)['snapshot']
        self.destination = self.root / 'restored'

    def test_duplicate_content_and_modes(self):
        result = bundle.restore(self.store, self.snapshot, self.destination, self.source)
        one, two, executable = (self.destination / name for name in ('first','second','executable'))
        self.assertEqual(one.read_bytes(), b'identical bytes')
        self.assertEqual(one.stat().st_ino, two.stat().st_ino)
        self.assertNotEqual(one.stat().st_ino, executable.stat().st_ino)
        self.assertNotEqual(one.stat().st_ino, (self.source/'first').stat().st_ino)
        self.assertEqual(executable.stat().st_mode & 0o777, 0o755)
        self.assertEqual(self.destination.stat().st_mode & 0o777, 0o750)
        self.assertFalse(result['receipt_acceptance'])
        with self.assertRaises(FileExistsError):
            bundle.restore(self.store, self.snapshot, self.destination, self.source)

    def test_tamper_and_budget_fail_before_destination(self):
        with self.assertRaisesRegex(archive.ArchiveError, 'growth budget'):
            bundle.restore(self.store, self.snapshot, self.destination, self.source, max_growth=1)
        self.assertFalse(self.destination.exists())
        _, files, _ = archive.load_snapshot(self.store, self.snapshot)
        blob = archive.blob_path(self.store, files['first']['sha256'])
        blob.chmod(0o600); blob.write_bytes(b'tampered'); blob.chmod(0o400)
        with self.assertRaises(archive.ArchiveError):
            bundle.restore(self.store, self.snapshot, self.destination, self.source)
        self.assertFalse(self.destination.exists())

    def test_restore_verification_rejects_added_changed_and_external_alias(self):
        bundle.restore(self.store, self.snapshot, self.destination, self.source)
        item = self.destination / 'first'
        extra = self.destination / 'extra'; extra.write_text('unexpected')
        with self.assertRaises(archive.ArchiveError):
            bundle.verify_restore(self.store, self.snapshot, self.destination)
        extra.unlink()
        alias = self.root / 'outside'; os.link(item, alias)
        with self.assertRaisesRegex(archive.ArchiveError, 'external alias'):
            bundle.verify_restore(self.store, self.snapshot, self.destination)
        alias.unlink()
        item.write_bytes(b'changed')
        with self.assertRaisesRegex(archive.ArchiveError, 'content changed'):
            bundle.verify_restore(self.store, self.snapshot, self.destination)

    def test_store_overlap_and_symlink_ancestor_refused(self):
        with self.assertRaisesRegex(archive.ArchiveError, 'overlaps'):
            bundle.restore(self.store, self.snapshot, self.store/'restored', self.source)
        with self.assertRaisesRegex(archive.ArchiveError, 'overlaps original'):
            bundle.restore(self.store, self.snapshot, self.source/'nested', self.source)
        link = self.root/'alias'; link.symlink_to(self.root, target_is_directory=True)
        with self.assertRaisesRegex(archive.ArchiveError, 'symlink'):
            bundle.restore(self.store, self.snapshot, link/'restored', self.source)


if __name__ == '__main__':
    unittest.main()
