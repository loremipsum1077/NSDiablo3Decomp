"""Tests for bounded lexer data diagnostics."""
import hashlib
from pathlib import Path
import struct
import unittest
from unittest.mock import patch
from dataclasses import replace

from validation.elf import Elf

from data_elf import _inspect
from test_literal_proof import original
from test_import_proof import CALL, RDYN, DYN
from validation import lexer_data, qualified_owner


class LexerPrimitiveTests(unittest.TestCase):
    def test_string_bounds_and_terminator(self):
        data=b'abc\0suffix\0';sec=(0,1,0,0,0,len(data),0,0,1,1)
        self.assertEqual(lexer_data._cstring(data,sec,0),b'abc\0')
        self.assertEqual(lexer_data._cstring(data,sec,1),b'bc\0')
        for offset in (-1,len(data)):
            with self.assertRaises(ValueError):lexer_data._cstring(data,sec,offset)
        with self.assertRaises(ValueError):lexer_data._cstring(b'bad', (0,1,0,0,0,3,0,0,1,1),0)

    def test_pair_and_register_mutations(self):
        a=original();meta=_inspect(a,'fixture');words=(0x90000000,0x91000000)
        address,_,_=lexer_data._pair(a,meta,CALL,words,18)
        self.assertEqual(address,0x1200)
        for words in ((0x9000001f,0x910003ff),(0x90000000,0x91000020),(0x90000020,0x91000000)):
            with self.subTest(words=words),self.assertRaises(ValueError):lexer_data._pair(a,meta,CALL,words,18)
        with self.assertRaises(ValueError):lexer_data._pair(a,meta,CALL,words,5000)

    def test_original_relocation_intersection(self):
        for start in (0xff9,0x1000,0x1007):
            with self.subTest(start=start),self.assertRaises(ValueError):
                lexer_data._no_original_writes([{'offset':start}],0x1000,8)
        lexer_data._no_original_writes([{'offset':0xff8},{'offset':0x1008}],0x1000,8)

    def test_dynamic_metadata_and_relocation_kind(self):
        good=original();lexer_data._dynamic(good,_inspect(good,'fixture'))
        for where,fmt,value in ((DYN+10*16,'<q',17),(RDYN+8,'<Q',9999),(RDYN+8,'<Q',1031),(RDYN,'<Q',(1<<64)-7)):
            a=bytearray(good);struct.pack_into(fmt,a,where,value)
            with self.subTest(where=where),self.assertRaises(ValueError):lexer_data._dynamic(a,_inspect(a,'fixture'))


class RetainedLexerIntegration(unittest.TestCase):
    def test_pinned_real_object(self):
        root=Path(__file__).resolve().parent.parent
        obj=root/'build/compiler-calibration/lua-full-lexer-on-v1/out/llex.o'
        if not obj.exists() or not (root/qualified_owner.PINS['nss_sha256']).exists():self.skipTest('retained lexer inputs unavailable')
        def sha(p):
            h=hashlib.sha256()
            with p.open('rb') as stream:
                for block in iter(lambda:stream.read(1024*1024),b''):h.update(block)
            return h.hexdigest()
        pins={key:sha(root/path) for key,path in qualified_owner.PINS.items()}
        objsha=sha(obj)
        if objsha!='50491449866c14784484c9f36f1d32f8ada8e373908fcb2015261884edf60e4f':self.skipTest('retained pilot object changed')
        ctx=qualified_owner.build_context(root,qualified_owner.OWNER,pins,obj,objsha)
        w=lexer_data.prove(root/qualified_owner.PINS['original_sha256'],pins['original_sha256'],root/qualified_owner.PINS['nss_sha256'],pins['nss_sha256'],obj,objsha,ctx)
        self.assertEqual((len(w['consumers']),sum(x['kind']=='string' for x in w['consumers']),len(w['token_slots'])),(20,18,32))
        self.assertTrue(w['token_slots'][-1]['null'])
        self.assertTrue(w['original_table_writable']);self.assertFalse(w['original_relro_proven']);self.assertFalse(w['receipt_acceptance'])

    def test_mutated_token_and_consumer_rejected(self):
        root=Path(__file__).resolve().parent.parent
        obj=root/'build/compiler-calibration/lua-full-lexer-on-v1/out/llex.o'
        if not obj.exists() or not (root/qualified_owner.PINS['nss_sha256']).exists():self.skipTest('retained lexer inputs unavailable')
        def sha(data):return hashlib.sha256(data).hexdigest()
        blobs={k:(root/path).read_bytes() for k,path in qualified_owner.PINS.items() if k!='nss_sha256'}
        clean=obj.read_bytes()
        if sha(clean)!='50491449866c14784484c9f36f1d32f8ada8e373908fcb2015261884edf60e4f':self.skipTest('retained pilot object changed')
        pins={k:sha(v) for k,v in blobs.items()}
        with (root/qualified_owner.PINS['nss_sha256']).open('rb') as stream:
            digest=hashlib.sha256()
            for block in iter(lambda:stream.read(1024*1024),b''):digest.update(block)
            pins['nss_sha256']=digest.hexdigest()
        ctx=qualified_owner.build_context(root,qualified_owner.OWNER,pins,obj,sha(clean)); e=Elf(obj)
        # Change a token pointer addend, remove an ABS64 type, and change an ADD register.
        rela=e.sections[43][4];text=e.sections[5][4];consumer_rela=e.sections[6][4]
        for position,fmt,value in ((rela+16,'<q',1),(rela+8,'<Q',0),(text+0x58,'<I',0x910003ff),(consumer_rela,'<Q',0x50)):
            mutated=bytearray(clean);struct.pack_into(fmt,mutated,position,value)
            if position==consumer_rela:
                info=struct.unpack_from('<Q',mutated,consumer_rela+8)[0];struct.pack_into('<Q',mutated,consumer_rela+8,(info & ~0xffffffff)|257)
            mutated=bytes(mutated)
            altered=replace(ctx,object_sha256=sha(mutated))
            def snapshot(path,limit,digest):
                if Path(path).resolve()==obj.resolve():return mutated
                for key,rel in qualified_owner.PINS.items():
                    if Path(path).resolve()==(root/rel).resolve():return blobs[key]
                raise AssertionError(path)
            with self.subTest(position=position),patch.object(qualified_owner,'revalidate_context',return_value=altered),patch.object(qualified_owner,'_snapshot',side_effect=snapshot),self.assertRaises(ValueError):
                lexer_data.prove(root/qualified_owner.PINS['original_sha256'],pins['original_sha256'],root/qualified_owner.PINS['nss_sha256'],pins['nss_sha256'],obj,sha(mutated),altered)

if __name__=='__main__':unittest.main()
