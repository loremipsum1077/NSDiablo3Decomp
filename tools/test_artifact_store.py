"""Streaming archive safety and complete sealed-tree round trips."""
import contextlib
import gzip
import hashlib
import json
import os
import shutil
import subprocess
import sys
from pathlib import Path
import tempfile
import unittest
from unittest.mock import patch
import artifact_store as a

class ArtifactTests(unittest.TestCase):
    def setUp(self):
        t=tempfile.TemporaryDirectory(dir=str(Path(tempfile.gettempdir()).resolve()));self.addCleanup(t.cleanup);self.root=Path(t.name)
        self.source=self.root/'source';self.source.mkdir(mode=0o750)
        self.receipt=self.source/'normal'/'receipt';self.receipt.mkdir(parents=True)
        (self.receipt/'objects').mkdir();(self.receipt/'objects'/'a.obj').write_bytes(b'object bytes\0'*100)
        (self.receipt/'objects').chmod(0o710)
        (self.receipt/'run.sh').write_bytes(b'#!/bin/sh\nexit 0\n');(self.receipt/'run.sh').chmod(0o751)
        self.seal()
        self.store=self.root/'store'
        self.lease=patch.object(a.resource_admission,'lease',side_effect=contextlib.nullcontext);self.lease.start();self.addCleanup(self.lease.stop)
        self.check=patch.object(a.resource_admission,'check');self.check.start();self.addCleanup(self.check.stop)
    def seal(self,files=None):
        if files is None:
            files={str(p.relative_to(self.receipt)):hashlib.sha256(p.read_bytes()).hexdigest() for p in self.receipt.rglob('*') if p.is_file() and p.name!='done.json'}
        (self.receipt/'done.json').write_text(json.dumps({'files':files}))
    def create(self):return a.create(self.source,self.store)['snapshot']
    def test_roundtrip_and_modes(self):
        snap=self.create();report=a.verify(self.store,snap);self.assertFalse(report['receipt_acceptance'])
        target=self.root/'restored';self.assertEqual(a.restore(self.store,snap,target)['status'],'archive_restored')
        self.assertEqual((target/'normal/receipt/objects/a.obj').read_bytes(),(self.receipt/'objects/a.obj').read_bytes())
        self.assertEqual((target/'normal/receipt/run.sh').stat().st_mode&0o777,0o751)
        self.assertEqual((target/'normal/receipt/objects').stat().st_mode&0o777,0o710)
        self.assertEqual(target.stat().st_mode&0o777,0o750)
        self.assertEqual((target/'normal/receipt/done.json').read_bytes(),(self.receipt/'done.json').read_bytes())
        with self.assertRaises(FileExistsError):a.restore(self.store,snap,target)
    def test_optional_write_limit_fails_before_snapshot_publication(self):
        with self.assertRaisesRegex(a.ArchiveError, 'write limit exceeded'):
            a.create(self.source, self.store, write_limit_bytes=30)
        self.assertFalse(any((self.store/'snapshots').iterdir()))
        self.assertFalse(any((self.store/'blobs').rglob('*.gz')))
        compressed = sum(len(gzip.compress(p.read_bytes(), mtime=0)) for p in self.source.rglob('*') if p.is_file())
        with self.assertRaisesRegex(a.ArchiveError, 'write limit exceeded'):
            a.create(self.source, self.store, write_limit_bytes=compressed + 1)
        self.assertFalse(any((self.store/'blobs').rglob('*.gz')))
        self.assertEqual(a.create(self.source, self.store, write_limit_bytes=100000)['status'], 'archive_created')
        with self.assertRaisesRegex(a.ArchiveError, 'invalid write limit'):
            a.create(self.source, self.store, write_limit_bytes=True)

    def test_missing_changed_or_unsafe_seal(self):
        for files in ({'missing':'0'*64},{'../escape':'0'*64},{'run.sh':'0'*64}):
            self.seal(files)
            with self.subTest(files=files),self.assertRaises(a.ArchiveError):self.create()
        (self.receipt/'done.json').write_text('{"files":{"x":"a","x":"b"}}')
        with self.assertRaises(a.ArchiveError):self.create()
    def test_symlink_and_hardlink_source_rejected(self):
        (self.source/'link').symlink_to(self.receipt/'run.sh')
        with self.assertRaisesRegex(a.ArchiveError,'symlink'):self.create()
        (self.source/'link').unlink();os.link(self.receipt/'run.sh',self.source/'hardlink')
        with self.assertRaisesRegex(a.ArchiveError,'hardlinked'):self.create()
    def test_snapshot_and_blob_tamper(self):
        snap=self.create();obj,filemap,_=a.load_snapshot(self.store,snap);item=filemap['normal/receipt/run.sh']
        blob=a.blob_path(self.store,item['sha256']);blob.chmod(0o600);blob.write_bytes(blob.read_bytes()+b'extra');blob.chmod(0o400)
        with self.assertRaises(a.ArchiveError):a.verify(self.store,snap)
        with self.assertRaises(a.ArchiveError):self.create()  # Corrupted blobs cannot be reused.
        manifest=self.store/'snapshots'/(snap+'.json');manifest.chmod(0o600);manifest.write_bytes(manifest.read_bytes()+b' ');manifest.chmod(0o400)
        with self.assertRaisesRegex(a.ArchiveError,'snapshot id'):a.verify(self.store,snap)
    def test_concatenated_gzip_and_padding_rejected_even_when_manifest_rehashed(self):
        snap=self.create();obj,files,_=a.load_snapshot(self.store,snap);item=files['normal/receipt/run.sh']
        blob=a.blob_path(self.store,item['sha256']);original=blob.read_bytes()
        for suffix in (gzip.compress(b'',mtime=0),b'\0\0'):
            encoded=original+suffix;blob.chmod(0o600);blob.write_bytes(encoded);blob.chmod(0o400)
            for file in obj['files']:
                if file['sha256']==item['sha256']:
                    file['compressed_size']=len(encoded);file['compressed_sha256']=hashlib.sha256(encoded).hexdigest()
            payload=a.manifest_bytes(obj);new_id=hashlib.sha256(payload).hexdigest()
            forged=self.store/'snapshots'/(new_id+'.json');forged.write_bytes(payload);forged.chmod(0o400)
            with self.subTest(suffix=suffix),self.assertRaisesRegex(a.ArchiveError,'trailing compressed data'):
                a.verify(self.store,new_id)
        blob.chmod(0o600);blob.write_bytes(original);blob.chmod(0o400)
    def test_compressible_blob_is_decoded_in_bounded_chunks(self):
        (self.receipt/'zeros').write_bytes(bytes(a.CHUNK*3+11));self.seal();snap=self.create()
        original=a.zlib.decompressobj;limits=[]
        class Decoder:
            def __init__(self, *args):self.inner=original(*args)
            def decompress(self, data, limit):
                limits.append(limit);return self.inner.decompress(data,limit)
            def __getattr__(self,name):return getattr(self.inner,name)
        with patch.object(a.zlib,'decompressobj',side_effect=Decoder):a.verify(self.store,snap)
        self.assertTrue(limits);self.assertLessEqual(max(limits),a.CHUNK)
    def test_restore_rejects_symlink_blob(self):
        snap=self.create();_,files,_=a.load_snapshot(self.store,snap);item=files['normal/receipt/run.sh']
        blob=a.blob_path(self.store,item['sha256']);blob.unlink();blob.symlink_to(self.receipt/'run.sh')
        with self.assertRaises(OSError):a.restore(self.store,snap,self.root/'new')
        self.assertFalse((self.root/'new').exists())
    def test_store_blob_hardlink_alias_rejected(self):
        snap=self.create();_,files,_=a.load_snapshot(self.store,snap);item=files['normal/receipt/run.sh']
        blob=a.blob_path(self.store,item['sha256']);os.link(blob,self.root/'blob-alias')
        with self.assertRaisesRegex(a.ArchiveError,'immutable'):a.verify(self.store,snap)
    def test_case_collision_rejected(self):
        (self.source/'Readme').write_text('a');(self.source/'README').write_text('b')
        if len(list(self.source.glob('[Rr][Ee][Aa][Dd][Mm][Ee]'))) < 2:self.skipTest('case-insensitive filesystem')
        with self.assertRaisesRegex(a.ArchiveError,'collision'):self.create()
    def test_case_collision_in_manifest_rejected_on_any_host(self):
        bad={'kind':a.KIND,'root_mode':0o700,'directories':[{'path':'Name','mode':0o700},
             {'path':'name','mode':0o700}], 'files':[], 'seals':['done.json']}
        with self.assertRaisesRegex(a.ArchiveError,'collision'):a._snapshot_manifest(a.manifest_bytes(bad))
    def test_source_changes_before_publication(self):
        original=a.enumerate_tree;calls=0
        def mutate(root):
            nonlocal calls
            calls+=1
            if calls==2:(self.source/'late').write_text('changed')
            return original(root)
        with patch.object(a,'enumerate_tree',side_effect=mutate),self.assertRaisesRegex(a.ArchiveError,'source tree changed'):
            self.create()
        self.assertFalse(any((self.store/'snapshots').iterdir()))
    def test_no_seal_rejected(self):
        (self.receipt/'done.json').unlink()
        with self.assertRaisesRegex(a.ArchiveError,'archive entries'):self.create()
    def test_incomplete_run_rejected(self):
        (self.source/'run.json').write_text('{"status":"in_progress","modes":{}}')
        with self.assertRaisesRegex(a.ArchiveError,'run is not complete'):self.create()
    def test_completed_run_rejects_swapped_checkpoint(self):
        shutil.rmtree(self.source/'normal')
        (self.source/'binding.json').write_text('binding')
        (self.source/'evidence.json').write_text('evidence')
        digest=lambda p:hashlib.sha256(p.read_bytes()).hexdigest()
        modes={}
        for label in ('baseline','candidate'):
            for variant in ('normal','lua'):
                attempt=self.source/label/variant/'attempt-0001';receipt=attempt/'receipt'
                receipt.mkdir(parents=True);(receipt/'result.json').write_text(label+variant)
                (receipt/'done.json').write_text(json.dumps({'files':{'result.json':digest(receipt/'result.json')}}))
                (attempt/'binding.json').write_text(label+variant+' binding')
                modes[label+'_'+variant]={'root':str(receipt.relative_to(self.source)),
                    'receipt_sha256':digest(receipt/'done.json'), 'attempt_binding_sha256':digest(attempt/'binding.json')}
        run={'status':'recovery_evidence_complete','modes':modes,
             'binding_sha256':digest(self.source/'binding.json'),'evidence_sha256':digest(self.source/'evidence.json')}
        (self.source/'run.json').write_text(json.dumps(run));self.create()
        run['modes']['candidate_normal']=dict(run['modes']['baseline_normal'])
        (self.source/'run.json').write_text(json.dumps(run))
        with self.assertRaisesRegex(a.ArchiveError,'checkpoint mode path'):
            self.create()
    def test_snapshot_canonical_path_validation(self):
        bad={'kind':a.KIND,'root_mode':0o700,'directories':[],
             'files':[{'path':'../evil','mode':0o600,'size':0,'sha256':'0'*64,
                       'compressed_size':20,'compressed_sha256':'1'*64}], 'seals':['done.json']}
        with self.assertRaises(a.ArchiveError):a._snapshot_manifest(a.manifest_bytes(bad))
    def test_symlink_in_store_ancestor_rejected(self):
        link=self.root/'linkdir';link.symlink_to(self.root,target_is_directory=True)
        with self.assertRaisesRegex(a.ArchiveError,'symlink path component'):
            a.create(self.source,link/'store')
    def test_special_fifo_is_rejected_without_blocking(self):
        if not hasattr(os,'mkfifo'):self.skipTest('FIFO unsupported')
        fifo=self.root/'fifo';os.mkfifo(fifo)
        command=[sys.executable,'-c',
                 'import artifact_store,sys; artifact_store.regular_open(sys.argv[1])',str(fifo)]
        result=subprocess.run(command,capture_output=True,timeout=3,env={**os.environ,'PYTHONPATH':str(Path(a.__file__).parent)})
        self.assertNotEqual(result.returncode,0);self.assertIn(b'expected regular file',result.stderr)
        rootfd=os.open(self.root,os.O_RDONLY|os.O_DIRECTORY)
        try:
            with patch.object(a.os,'open',wraps=os.open) as opened:
                fd=a.safe_open(rootfd,'fifo');os.close(fd)
                self.assertTrue(opened.call_args.args[1] & os.O_NONBLOCK)
        finally:os.close(rootfd)

if __name__=='__main__':unittest.main()
