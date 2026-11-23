"""Tiny synthetic original-input fixtures; no game material or external tools."""
import contextlib
import hashlib
import io
import json
import os
from pathlib import Path
import tempfile
import unittest
from unittest.mock import patch

import verify_original_inputs as verify


def fixture_manifest(root, nso, nss):
    """Write synthetic expectations, only in a test's temporary repository."""
    manifest = {'schema_version': 1, 'version': '2.6.2', 'platform': 'Switch', 'inputs': []}
    for id_, path, filename, required in (
            ('nso', nso, 'exefs/main', True),
            ('nss', nss, 'romfs/DiabloIIINX64r.nss', True)):
        manifest['inputs'].append({'id': id_, 'filename': filename, 'required': required,
                                  'role': 'synthetic fixture', 'size': path.stat().st_size,
                                  'sha256': hashlib.sha256(path.read_bytes()).hexdigest()})
    manifest['inputs'].append({'id': 'optional', 'filename': 'romfs/evidence.nss',
                              'role': 'optional synthetic evidence', 'required': False,
                              'size': 8, 'sha256': hashlib.sha256(b'optional').hexdigest()})
    (root / 'config').mkdir(exist_ok=True)
    (root / 'config/original_inputs.json').write_text(json.dumps(manifest))
    (root / 'tools').mkdir(exist_ok=True)
    (root / 'tools/setup.py').write_text('NSO_SHA256 = ' + repr(manifest['inputs'][0]['sha256']))
    return manifest


class OriginalInputTests(unittest.TestCase):
    def setUp(self):
        temp = tempfile.TemporaryDirectory()
        self.addCleanup(temp.cleanup)
        self.root = Path(temp.name).resolve()
        self.inputs = self.root / 'private originals'
        self.nso = self.inputs / 'exefs/main'
        self.nss = self.inputs / 'romfs/DiabloIIINX64r.nss'
        self.nso.parent.mkdir(parents=True)
        self.nss.parent.mkdir(parents=True)
        self.nso.write_bytes(b'original NSO fixture')
        self.nss.write_bytes(b'original NSS fixture')
        self.manifest = fixture_manifest(self.root, self.nso, self.nss)
        self.manifest_path = self.root / 'config/original_inputs.json'

    def save_manifest(self):
        self.manifest_path.write_text(json.dumps(self.manifest))

    def verify(self, **kwargs):
        return verify.verify_inputs(self.inputs, root=self.root, **kwargs)

    def statuses(self):
        return {row['id']: row['status'] for row in self.verify()}

    def test_valid_inputs_and_absent_optional_are_read_only(self):
        before = {p: (p.read_bytes(), verify.stat_identity(p.stat()))
                  for p in self.root.rglob('*') if p.is_file()}
        self.assertEqual(self.statuses(), {'nso': 'verified', 'nss': 'verified', 'optional': 'missing_optional'})
        self.assertFalse(verify.failed(self.verify()))
        self.assertEqual(before, {p: (p.read_bytes(), verify.stat_identity(p.stat()))
                                  for p in self.root.rglob('*') if p.is_file()})

    def test_both_missing_required_inputs_are_reported(self):
        self.nso.unlink()
        self.nss.unlink()
        self.assertEqual(self.statuses(), {'nso': 'missing', 'nss': 'missing', 'optional': 'missing_optional'})
        self.assertTrue(verify.failed(self.verify()))

    def test_wrong_size_and_same_size_corruption_are_distinct(self):
        self.nso.write_bytes(b'short')
        self.nss.write_bytes(b'X' * self.manifest['inputs'][1]['size'])
        self.assertEqual(self.statuses()['nso'], 'wrong_size')
        self.assertEqual(self.statuses()['nss'], 'wrong_hash')
        self.assertTrue(verify.failed(self.verify()))

    def test_optional_present_valid_then_corrupt_and_required_only_mode(self):
        optional = self.inputs / 'romfs/evidence.nss'
        optional.write_bytes(b'optional')
        self.assertEqual(self.statuses()['optional'], 'verified')
        optional.write_bytes(b'corrupt!')
        self.assertEqual(self.statuses()['optional'], 'wrong_hash')
        self.assertTrue(verify.failed(self.verify()))
        self.assertFalse(verify.failed(self.verify(include_optional=False)))

    def test_explicit_external_paths_require_the_same_identity(self):
        external = self.root / 'separate original'
        self.nso.rename(external)
        self.assertEqual(self.verify(nso=external)[0]['status'], 'verified')
        external.write_bytes(b'X' * self.manifest['inputs'][0]['size'])
        self.assertEqual(self.verify(nso=external)[0]['status'], 'wrong_hash')

    def test_no_fallback_to_unselected_alias_or_generated_intermediate(self):
        self.manifest['inputs'][0]['equivalent_filenames'] = ['romfs/another-main']
        self.save_manifest()
        self.nso.rename(self.inputs / 'romfs/another-main')
        self.assertEqual(self.statuses()['nso'], 'missing')
        self.assertEqual(self.verify(nso=self.inputs / 'romfs/another-main')[0]['status'], 'verified')
        self.nss.rename(self.inputs / 'romfs/renamed-ra.nss')
        self.assertEqual(self.statuses()['nss'], 'missing')

    def test_unsupported_target_fails_before_file_reads(self):
        with patch.object(verify, 'fingerprint', side_effect=AssertionError('must not read')):
            for kwargs, message in [({'version': '2.7.0'}, 'unsupported version'),
                                    ({'platform': 'PC'}, 'unsupported platform')]:
                with self.subTest(kwargs=kwargs), self.assertRaisesRegex(verify.InputError, message):
                    self.verify(**kwargs)

    def test_unsafe_manifest_paths_fail_before_file_reads(self):
        for value in ('../escape', '/absolute', 'a/../../escape', 'a/./main', 'a//main',
                      'C:/private', r'a\main', 'a\nmain', '', '.', '..'):
            with self.subTest(value=value):
                self.manifest['inputs'][1]['filename'] = value
                self.save_manifest()
                with patch.object(verify, 'fingerprint', side_effect=AssertionError('must not read')):
                    with self.assertRaisesRegex(verify.InputError, 'unsafe filename'):
                        self.verify()

    def test_alias_paths_are_also_validated(self):
        self.manifest['inputs'][0]['equivalent_filenames'] = ['../escape']
        self.save_manifest()
        with self.assertRaisesRegex(verify.InputError, 'unsafe filename'):
            self.verify()

    def test_manifest_schema_pin_and_entry_validation(self):
        cases = [lambda: self.manifest.update(schema_version=2),
                 lambda: self.manifest['inputs'][1].update(size=-1),
                 lambda: self.manifest['inputs'][1].update(sha256='bad'),
                 lambda: self.manifest['inputs'][1].update(required=False),
                 lambda: self.manifest['inputs'][1].update(id='nso'),
                 lambda: self.manifest['inputs'][1].update(filename='exefs/main')]
        original = json.dumps(self.manifest)
        for mutation in cases:
            self.manifest = json.loads(original)
            mutation()
            self.save_manifest()
            with self.assertRaises(verify.InputError):
                self.verify()
        self.manifest = json.loads(original)
        self.save_manifest()
        (self.root / 'tools/setup.py').write_text('NSO_SHA256 = ' + repr('0' * 64))
        with self.assertRaisesRegex(verify.InputError, 'mirror differs'):
            self.verify()

    def test_symlink_files_and_directories_are_rejected(self):
        outside = self.root / 'outside'
        self.nso.rename(outside)
        self.nso.symlink_to(outside)
        self.assertIn('unsafe_path', self.statuses()['nso'])
        link = self.root / 'link'
        link.symlink_to(outside)
        self.assertIn('not_regular', self.verify(nso=link)[0]['status'])
        self.nso.unlink()
        self.nso.parent.rmdir()
        self.nso.parent.symlink_to(self.nss.parent, target_is_directory=True)
        self.assertIn('unsafe_path', self.statuses()['nso'])

    def test_directory_and_fifo_inputs_are_rejected_without_blocking(self):
        self.nso.unlink()
        self.nso.mkdir()
        self.assertIn('not_regular', self.statuses()['nso'])
        self.nso.rmdir()
        os.mkfifo(self.nso)
        self.assertIn('not_regular', self.statuses()['nso'])

    def test_file_mutation_is_detected_even_if_size_and_mtime_restored(self):
        original_hash = verify.hashlib.sha256
        initial = self.nss.stat()
        class MutatingHash:
            def __init__(inner):
                inner.digest = original_hash()
            def update(inner, block):
                inner.digest.update(block)
                self.nss.write_bytes(b'Z' * initial.st_size)
                os.utime(self.nss, ns=(initial.st_atime_ns, initial.st_mtime_ns))
            def hexdigest(inner):
                return inner.digest.hexdigest()
        with patch.object(verify.hashlib, 'sha256', MutatingHash):
            with self.assertRaisesRegex(verify.InputError, 'changed_during_hash'):
                verify.fingerprint(self.nss)

    def test_path_replacement_and_disappearance_during_hash_are_detected(self):
        original_hash = verify.hashlib.sha256
        for disappear in (False, True):
            self.nss.write_bytes(b'original NSS fixture')
            class ReplacingHash:
                def __init__(inner):
                    inner.digest = original_hash()
                def update(inner, block):
                    inner.digest.update(block)
                    self.nss.unlink()
                    if not disappear:
                        self.nss.write_bytes(block)
                def hexdigest(inner):
                    return inner.digest.hexdigest()
            with self.subTest(disappear=disappear), patch.object(verify.hashlib, 'sha256', ReplacingHash):
                with self.assertRaisesRegex(verify.InputError, 'changed_during_hash'):
                    verify.fingerprint(self.nss)

    def test_streams_multiple_small_chunks(self):
        with patch.object(verify, 'CHUNK_SIZE', 3):
            self.assertEqual(verify.fingerprint(self.nso)['sha256'], self.manifest['inputs'][0]['sha256'])

    def test_cli_reports_safe_filenames_and_nonzero_errors(self):
        real_verify = verify.verify_inputs
        def fixture_verify(input_root, **kwargs):
            return real_verify(input_root, root=self.root, **kwargs)
        stdout, stderr = io.StringIO(), io.StringIO()
        self.nss.write_bytes(b'bad')
        with patch.object(verify, 'verify_inputs', side_effect=fixture_verify), \
                contextlib.redirect_stdout(stdout), contextlib.redirect_stderr(stderr):
            self.assertEqual(verify.main(['--input-root', str(self.inputs)]), 1)
        self.assertIn('wrong_size: romfs/DiabloIIINX64r.nss', stdout.getvalue())
        self.assertIn('missing_optional', stdout.getvalue())
        self.assertNotIn(str(self.root), stdout.getvalue() + stderr.getvalue())
        self.assertNotIn('original NSS fixture', stdout.getvalue() + stderr.getvalue())

    def test_repository_manifest_keeps_the_historical_pin(self):
        manifest = verify.load_manifest()
        self.assertEqual({row['id'] for row in manifest['inputs'] if row['required']}, {'nso', 'nss'})


if __name__ == '__main__':
    unittest.main()
