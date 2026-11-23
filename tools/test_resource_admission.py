"""Resource estimates sum shared filesystems and fail closed on bad evidence."""
import tempfile
import multiprocessing
import os
import threading
import unittest
from pathlib import Path
from unittest.mock import patch
import resource_admission as admission

class ResourceTests(unittest.TestCase):
    def test_shared_device_sums_and_uses_lowest_observation(self):
        def probe(path):
            return {'ancestor':path,'device':9,'available_bytes':100 if path=='/a' else 90}
        result=admission.assess([{'path':'/a','bytes':30},{'path':'/b','bytes':41}],20,inspect=probe)
        self.assertFalse(result['sufficient'])
        self.assertEqual(result['volumes'][0]['shortfall_bytes'],1)
        self.assertEqual(result['volumes'][0]['requested_bytes'],71)
        self.assertFalse(result['reservation_held'])
    def test_distinct_devices_may_share_capacity(self):
        def probe(path):
            return {'ancestor':path,'device':1 if path=='/a' else 2,'available_bytes':100}
        result=admission.assess([{'path':'/a','bytes':80},{'path':'/b','bytes':80}],20,inspect=probe)
        self.assertFalse(result['sufficient']);self.assertEqual(len(result['volumes']),2)
        self.assertTrue(all(v['required_bytes']==180 for v in result['volumes']))
    def test_bad_inputs_and_aggregate_overflow(self):
        bad=[([],1),([{'path':'/a','bytes':1}],0),([{'path':'/a','bytes':True}],1),
             ([{'path':'/a','bytes':-1}],1),([{'path':'/a','bytes':1,'x':2}],1),
             ([{'path':'/a','bytes':admission.MAX_BYTES}],1),
             ([{'path':'/a','bytes':1}]*65,1)]
        probe=lambda path:{'ancestor':'/','device':1,'available_bytes':1}
        for requests,reserve in bad:
            with self.subTest(requests=requests[:1],reserve=reserve),self.assertRaises(ValueError):
                admission.assess(requests,reserve,inspect=probe)
    def test_probe_errors_do_not_become_success(self):
        with self.assertRaises(OSError):
            admission.assess([{'path':'/a','bytes':1}],1,inspect=lambda _:(_ for _ in ()).throw(OSError('unavailable')))
        with self.assertRaises(ValueError):
            admission.assess([{'path':'/a','bytes':1}],1,inspect=lambda _:{'ancestor':'/','device':1,'available_bytes':-1})
        with patch.object(Path,'stat',side_effect=PermissionError('denied')):
            with self.assertRaises(PermissionError):admission.probe('/tmp/protected')
    def test_real_nonexistent_child_does_not_create_it(self):
        with tempfile.TemporaryDirectory() as tmp:
            child=Path(tmp)/'missing'/'future'
            result=admission.assess([{'path':str(child),'bytes':0}],1)
            self.assertTrue(result['sufficient']);self.assertFalse(child.exists())
            self.assertEqual(result['volumes'][0]['requests'][0]['ancestor'],str(Path(tmp).resolve()))
        with self.assertRaises(ValueError):admission.probe('relative/path')
    def test_existing_file_cannot_be_used_as_output_parent(self):
        with tempfile.NamedTemporaryFile() as file:
            with self.assertRaises(ValueError):admission.probe(file.name)

    def test_check_rejects_insufficient_and_unavailable(self):
        with patch.object(admission, 'assess', return_value={'sufficient':False,'volumes':[{'shortfall_bytes':3}]}):
            with self.assertRaisesRegex(admission.AdmissionError,'3 more bytes'):admission.check('/tmp')
        with patch.object(admission, 'assess', side_effect=OSError('unavailable')):
            with self.assertRaisesRegex(admission.AdmissionError,'unavailable'):admission.check('/tmp')

    def test_lease_nested_exception_and_other_thread(self):
        with tempfile.TemporaryDirectory() as tmp, patch.object(admission,'LOCK_PATH',str(Path(tmp)/'lock')):
            with admission.lease():
                with admission.lease():
                    errors=[]
                    def contender():
                        try:
                            with admission.lease():pass
                        except admission.AdmissionError as exc:errors.append(str(exc))
                    thread=threading.Thread(target=contender);thread.start();thread.join()
                    self.assertEqual(len(errors),1);self.assertIn('busy',errors[0])
                with self.assertRaisesRegex(OSError,'caller failure'):
                    with admission.lease():raise OSError('caller failure')
            with admission.lease():pass
            with self.assertRaisesRegex(RuntimeError,'outer failure'):
                with admission.lease():raise RuntimeError('outer failure')
            with self.assertRaisesRegex(OSError,'build disk full'):
                with admission.lease():raise OSError('build disk full')
            with admission.lease():pass

    def test_fork_cannot_reenter_parent_lease(self):
        if not hasattr(os,'fork'):self.skipTest('fork required')
        ctx=multiprocessing.get_context('fork')
        with tempfile.TemporaryDirectory() as tmp, patch.object(admission,'LOCK_PATH',str(Path(tmp)/'lock')):
            result=ctx.Queue()
            def contender():
                try:
                    with admission.lease():
                        admission.check(str(Path(tmp)/'different-output'), growth_bytes=0, reserve_bytes=1)
                        result.put('acquired')
                except admission.AdmissionError:result.put('busy')
            with admission.lease():
                admission.check(str(Path(tmp)/'first-output'),growth_bytes=0,reserve_bytes=1)
                child=ctx.Process(target=contender);child.start();child.join(5)
                self.assertEqual(child.exitcode,0);self.assertEqual(result.get(timeout=1),'busy')
            child=ctx.Process(target=contender);child.start();child.join(5)
            self.assertEqual(child.exitcode,0);self.assertEqual(result.get(timeout=1),'acquired')

    def test_raw_fork_outer_context_preserves_reused_child_fd(self):
        if not hasattr(os,'fork'):self.skipTest('fork required')
        with tempfile.TemporaryDirectory() as tmp, patch.object(admission,'LOCK_PATH',str(Path(tmp)/'lock')):
            read_fd, write_fd = os.pipe()
            child_pid = None
            try:
                with admission.lease():
                    lease_fd = next(iter(admission._open_fds))
                    child_pid = os.fork()
                    if child_pid == 0:
                        os.close(read_fd)
                        source = os.open('/dev/null',os.O_RDONLY)
                        if source != lease_fd:
                            os.dup2(source,lease_fd);os.close(source)
                    else:
                        os.close(write_fd)
                        outcome = os.read(read_fd,20)
                        _, status = os.waitpid(child_pid,0)
                        self.assertEqual(outcome,b'preserved')
                        self.assertTrue(os.WIFEXITED(status) and os.WEXITSTATUS(status)==0)
                if child_pid == 0:
                    try:
                        os.fstat(lease_fd)
                        os.write(write_fd,b'preserved')
                        os._exit(0)
                    except OSError:
                        os.write(write_fd,b'closed')
                        os._exit(1)
            finally:
                if child_pid != 0:
                    os.close(read_fd)

    def test_inherited_context_does_not_clear_child_owned_lease(self):
        if not hasattr(os,'fork'):self.skipTest('fork required')
        with tempfile.TemporaryDirectory() as tmp, patch.object(admission,'LOCK_PATH',str(Path(tmp)/'lock')):
            read_fd, write_fd = os.pipe()
            child_pid = None
            child_lease = None
            with admission.lease():
                child_pid = os.fork()
                if child_pid == 0:
                    os.close(write_fd)
                    os.read(read_fd,1)  # Parent releases its lease first.
                    child_lease = admission.lease()
                    child_lease.__enter__()
                else:
                    os.close(read_fd)
            if child_pid == 0:
                ok = getattr(admission._local,'depth',0) == 1
                child_lease.__exit__(None,None,None)
                os._exit(0 if ok else 1)
            os.write(write_fd,b'x');os.close(write_fd)
            _, status = os.waitpid(child_pid,0)
            self.assertTrue(os.WIFEXITED(status) and os.WEXITSTATUS(status)==0)

    def test_unsafe_symlink_or_permissions_fail_closed(self):
        with tempfile.TemporaryDirectory() as tmp:
            target=Path(tmp)/'target';target.write_text('')
            lock=Path(tmp)/'lock';lock.symlink_to(target)
            with patch.object(admission,'LOCK_PATH',str(lock)):
                with self.assertRaises(admission.AdmissionError):
                    with admission.lease():pass
                lock.unlink();lock.write_text('');lock.chmod(0o644)
                with self.assertRaisesRegex(admission.AdmissionError,'Unsafe'):
                    with admission.lease():pass
                lock.chmod(0o600);os.link(lock,Path(tmp)/'hardlink')
                with self.assertRaisesRegex(admission.AdmissionError,'Unsafe'):
                    with admission.lease():pass

if __name__=='__main__':unittest.main()
