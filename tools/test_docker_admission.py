import contextlib
import json
import multiprocessing
import os
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest
from unittest.mock import patch

import docker_admission as admission
import resource_admission as resources


class SessionTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        root = Path(self.temp.name)
        self.journal = root / 'session.json'
        self.idle = {'daemon_id': 'daemon-a', 'active_project_containers': [], 'global_vm_idle_proven': False}
        for target, field, value in [(admission, 'JOURNAL', self.journal),
                                     (resources, 'LOCK_PATH', str(root / 'lock'))]:
            mock = patch.object(target, field, value)
            mock.start()
            self.addCleanup(mock.stop)
        self.observer = patch.object(admission, 'observe', return_value=self.idle).start()
        self.addCleanup(patch.stopall)

    def fail_session(self):
        with self.assertRaisesRegex(RuntimeError, 'failed'):
            with admission.session('test'):
                raise RuntimeError('failed')
        return admission.read_journal()

    def test_success_and_nested_session_keep_one_identity(self):
        with admission.session('outer'):
            first = admission.read_journal()
            self.assertEqual(first[0]['pid'], os.getpid())
            with admission.session('inner'):
                self.assertEqual(first, admission.read_journal())
        self.assertFalse(self.journal.exists())
        self.assertEqual(self.observer.call_count, 2)

    def test_failure_retained_and_never_automatically_cleared_when_idle(self):
        pending = self.fail_session()
        with self.assertRaisesRegex(resources.AdmissionError, 'Unresolved'):
            with admission.session('next'):
                self.fail('must not begin work')
        self.assertEqual(pending, admission.read_journal())
        self.assertEqual(self.observer.call_count, 1)

    def test_caught_nested_failure_poisoned_outer_session(self):
        with self.assertRaisesRegex(resources.AdmissionError, 'Nested Docker session failed'):
            with admission.session('outer'):
                try:
                    with admission.session('inner'):
                        raise RuntimeError('ambiguous nested failure')
                except RuntimeError:
                    pass
        self.assertIsNotNone(admission.read_journal())

    def test_fifo_journal_refused_without_blocking(self):
        os.mkfifo(self.journal, 0o600)
        with self.assertRaises(resources.AdmissionError):
            admission.read_journal()

    def test_raw_fork_child_cannot_clear_parent_intent(self):
        if not hasattr(os, 'fork'):
            self.skipTest('fork required')
        child = None
        with admission.session('parent'):
            expected = admission.read_journal()
            child = os.fork()
            if child != 0:
                _, status = os.waitpid(child, 0)
                self.assertTrue(os.WIFEXITED(status) and os.WEXITSTATUS(status) == 0)
                self.assertEqual(admission.read_journal(), expected)
        if child == 0:
            os._exit(0 if admission.read_journal() == expected else 1)
        self.assertFalse(self.journal.exists())

    def test_observation_failure_precedes_journal_and_work(self):
        self.observer.side_effect = resources.AdmissionError('unavailable')
        with self.assertRaisesRegex(resources.AdmissionError, 'unavailable'):
            with admission.session('test'):
                self.fail('must not begin work')
        self.assertFalse(self.journal.exists())

    def test_active_container_denies_without_stopping_it(self):
        self.observer.return_value = dict(self.idle, active_project_containers=[{'name': 'leftover'}])
        with self.assertRaisesRegex(resources.AdmissionError, 'leftover'):
            with admission.session('test'):
                self.fail('must not begin work')
        self.assertFalse(self.journal.exists())

    def test_daemon_change_or_active_end_retains_pending(self):
        for end in [dict(self.idle, daemon_id='other'),
                    dict(self.idle, active_project_containers=[{'name': 'orphan'}])]:
            with self.subTest(end=end):
                self.observer.side_effect = [self.idle, end]
                with self.assertRaises(resources.AdmissionError):
                    with admission.session('test'):
                        pass
                self.assertIsNotNone(admission.read_journal())
                self.journal.unlink()  # fixture only

    def test_replaced_or_malformed_journal_is_not_removed(self):
        with self.assertRaises(resources.AdmissionError):
            with admission.session('test'):
                self.journal.write_text('{broken')
        self.assertEqual(self.journal.read_text(), '{broken')
        with self.assertRaises(resources.AdmissionError):
            with admission.session('next'):
                pass

    def test_reconcile_requires_exact_identity_dead_pid_and_original_idle_daemon(self):
        pending = self.fail_session()
        with self.assertRaisesRegex(resources.AdmissionError, 'Exact'):
            admission.reconcile('0' * 64)
        with self.assertRaisesRegex(resources.AdmissionError, 'PID still'):
            admission.reconcile(pending[1])
        with patch.object(admission.os, 'kill', side_effect=ProcessLookupError):
            self.observer.return_value = dict(self.idle, daemon_id='other')
            with self.assertRaisesRegex(resources.AdmissionError, 'original Docker'):
                admission.reconcile(pending[1])
            self.observer.return_value = dict(self.idle, active_project_containers=[{'name': 'orphan'}])
            with self.assertRaisesRegex(resources.AdmissionError, 'orphan'):
                admission.reconcile(pending[1])
            self.observer.return_value = self.idle
            result = admission.reconcile(pending[1])
        self.assertFalse(result['queued_daemon_work_absence_proven'])
        self.assertFalse(self.journal.exists())

    def test_abrupt_process_exit_leaves_durable_pending_marker(self):
        if not hasattr(os, 'fork'):
            self.skipTest('fork required')
        def child():
            with admission.session('interrupted child'):
                os._exit(0)
        process = multiprocessing.get_context('fork').Process(target=child)
        process.start()
        process.join(5)
        self.assertEqual(process.exitcode, 0)
        pending = admission.read_journal()
        self.assertEqual(pending[0]['pid'], process.pid)
        with self.assertRaisesRegex(resources.AdmissionError, 'Unresolved'):
            with admission.session('replacement'):
                pass
        admission.reconcile(pending[1])
        self.assertFalse(self.journal.exists())

    def test_unsafe_journal_and_duplicate_json_keys_refused(self):
        target = self.journal.parent / 'target'
        target.write_text('{}')
        self.journal.symlink_to(target)
        with self.assertRaises(OSError):
            admission.read_journal()
        self.journal.unlink()
        self.fail_session()
        original = self.journal.read_bytes()
        self.journal.chmod(0o644)
        with self.assertRaises(resources.AdmissionError):
            admission.read_journal()
        self.journal.chmod(0o600)
        os.link(self.journal, self.journal.parent / 'alias')
        with self.assertRaises(resources.AdmissionError):
            admission.read_journal()
        (self.journal.parent / 'alias').unlink()
        self.journal.write_bytes(original[:-2] + b', "pid": 1}\n')
        with self.assertRaises(resources.AdmissionError):
            admission.read_journal()


class ObservationTests(unittest.TestCase):
    def test_legacy_immutable_images_names_and_new_labels_are_detected(self):
        rows = []
        for index, (image, name, state, labels) in enumerate([
                ('sha256:' + 'a'*64, 'random', 'running', ''),
                ('d3-build:5.0.1', 'compose', 'paused', ''),
                ('other', 'd3-receipt-old', 'created', ''),
                ('other', 'custom', 'running', 'org.nsd3.resource-session=x'),
                ('other', 'old-compose', 'running', 'com.docker.compose.service=build,com.docker.compose.project=nsd3decomp'),
                ('other', 'unrelated', 'running', ''),
                ('d3-build:5.0.1', 'finished', 'exited', '')]):
            rows.append(json.dumps({'ID': f'{index:064x}', 'Image': image, 'Names': name, 'State': state, 'Labels': labels}))
        responses = [json.dumps('daemon-a'), json.dumps({'Repository': 'd3-build', 'ID': 'sha256:'+'a'*64}), '\n'.join(rows)]
        with patch.object(admission, 'query', side_effect=responses):
            report = admission.observe()
        self.assertEqual([x['name'] for x in report['active_project_containers']], ['random', 'compose', 'd3-receipt-old', 'custom', 'old-compose'])
        self.assertFalse(report['global_vm_idle_proven'])

    def test_bad_daemon_or_container_metadata_is_not_idle(self):
        for responses in [['null'], ['"daemon"', '', '{"State":"running"}']]:
            with patch.object(admission, 'query', side_effect=responses):
                with self.assertRaises(resources.AdmissionError):
                    admission.observe()

    def test_query_stream_limit_and_command_failure(self):
        real = subprocess.Popen
        for code in ["import sys;sys.stdout.write('x'*2000)", "raise SystemExit(3)"]:
            with patch.object(admission, 'MAX_OUTPUT', 1024), \
                 patch.object(admission.subprocess, 'Popen', side_effect=lambda *a, **kw: real([sys.executable, '-c', code], **kw)):
                with self.assertRaises(resources.AdmissionError):
                    admission.query(['info'])


if __name__ == '__main__':
    unittest.main()
