"""Synthetic original and relocatable-object merged-string proof fixtures."""
import hashlib
from pathlib import Path
import struct
import tempfile
import unittest

from test_import_proof import fixture as original_fixture, CALL, SHOFF, RDYN, DYN
from validation.literals import LITERAL, prove_lstate_literal
from validation.common import Halt
from validation.strict import build_owner_context


def original():
    data=original_fixture(); names=b'.rodata\0'; names_offset=0x2800
    oldsize=struct.unpack_from('<Q',data,SHOFF+9*64+32)[0]
    data[names_offset+oldsize:names_offset+oldsize+len(names)]=names
    struct.pack_into('<Q',data,SHOFF+9*64+32,oldsize+len(names))
    struct.pack_into('<H',data,60,11)
    struct.pack_into('<IIQQQQIIQQ',data,SHOFF+10*64,oldsize,1,2,0x1200,0x1200,len(LITERAL),0,0,1,0)
    data[0x1200:0x1200+len(LITERAL)]=LITERAL
    delta=1 # PC page 0 -> string page 0x1000
    adrp=0x90000000|((delta&3)<<29)|(((delta>>2)&0x7ffff)<<5)
    struct.pack_into('<II',data,CALL,adrp,0x91000000|(0x200<<10))
    return data


def candidate():
    names=b'\0.text.f_luaopen\0.rodata.str1.1\0.rela.text.f_luaopen\0.symtab\0.strtab\0.shstrtab\0'
    nm=lambda x:names.index(b'\0'+x.encode()+b'\0')+1
    strtab=b'\0f_luaopen\0'
    symbols=bytes(24)+struct.pack('<IBBHQQ',0,3,0,2,0,0)+struct.pack('<IBBHQQ',1,2,0,1,0,8)
    rel=struct.pack('<QQq',0,(1<<32)|275,0)+struct.pack('<QQq',4,(1<<32)|277,0)
    specs=[('.text.f_luaopen',1,6,struct.pack('<II',0x90000000,0x91000000),0,0,4,0),
           ('.rodata.str1.1',1,0x32,LITERAL,0,0,1,1),
           ('.rela.text.f_luaopen',4,0,rel,4,1,8,24),
           ('.symtab',2,0,symbols,5,2,8,24),('.strtab',3,0,strtab,0,0,1,0),
           ('.shstrtab',3,0,names,0,0,1,0)]
    data=bytearray(64);headers=[bytes(64)]
    for name,typ,flags,payload,link,info,align,entsize in specs:
        while len(data)%8:data.append(0)
        headers.append(struct.pack('<IIQQQQIIQQ',nm(name),typ,flags,0,len(data),len(payload),link,info,align,entsize))
        data.extend(payload)
    while len(data)%8:data.append(0)
    off=len(data);data.extend(b''.join(headers))
    data[:64]=struct.pack('<16sHHIQQQIHHHHHH',b'\x7fELF\x02\x01\x01'+bytes(9),1,183,1,0,0,off,0,64,0,0,64,len(headers),6)
    return data


class LiteralTests(unittest.TestCase):
    def setUp(self):
        self.temp=tempfile.TemporaryDirectory();self.addCleanup(self.temp.cleanup);root=Path(self.temp.name)
        self.root=root; base=root/'data/2.6.2';base.mkdir(parents=True)
        self.original=base/'main.elf';self.functions=base/'functions.csv';self.files=base/'function_files.csv'
        self.functions.write_text('Address,Quality,Size,Name\n0x7100000400,U,8,f_luaopen\n')
        self.files.write_text('Address,File,Compiler\n0x7100000400,Contrib/Contrib/lua/src/lstate.c,clang\n')
        self.object=root/'object';self.source=root/'source';self.ir=root/'ir'
        self.source.write_bytes(b'source');self.ir.write_bytes(b'ir')
        self.a=original();self.b=candidate()
    def prove(self,a=None,b=None,**kwargs):
        a=self.a if a is None else a;b=self.b if b is None else b
        self.original.write_bytes(a);self.object.write_bytes(b)
        pin=lambda x:hashlib.sha256(x).hexdigest()
        if 'function_size' in kwargs:
            self.functions.write_text('Address,Quality,Size,Name\n0x7100000400,U,'+str(kwargs['function_size'])+',f_luaopen\n')
        context=build_owner_context(self.root,'Contrib/Contrib/lua/src/lstate.c', {'original_sha256':pin(a),'functions_sha256':pin(self.functions.read_bytes()),'function_files_sha256':pin(self.files.read_bytes())})
        return prove_lstate_literal(self.original,kwargs.get('original_pin',pin(a)),self.object,pin(b),
                                    self.source,pin(b'source'),self.ir,pin(b'ir'),context)
    def test_exact_witness(self):
        w=self.prove();self.assertEqual((w['object_function_offset'],w['original_consumer'],w['original_address']),(0,CALL,0x1200))
        self.assertFalse(w['candidate_linked_data_proven']);self.assertFalse(w['source_ir_object_lineage_proven'])
    def test_original_bytes_pin_and_bounds(self):
        for at,value in ((0x1200,0),(0x1200+17,1),(CALL+3,0)):
            a=bytearray(self.a);a[at]=value
            with self.subTest(at=at),self.assertRaises((ValueError,Halt)):self.prove(a=a)
        with self.assertRaises(ValueError):self.prove(original_pin='0'*64)
        with self.assertRaises((ValueError,Halt)):self.prove(function_size=12)
    def test_extra_overlapping_and_unknown_executable_relocations_rejected(self):
        shoff=struct.unpack_from('<Q',self.b,40)[0]
        oldoff=struct.unpack_from('<Q',self.b,shoff+3*64+24)[0]
        for offset,kind in ((0,257),(4,257),(0,9999)):
            b=bytearray(self.b);new=len(b);b.extend(b[oldoff:oldoff+48])
            b.extend(struct.pack('<QQq',offset,(2<<32)|kind,0))
            struct.pack_into('<QQ',b,shoff+3*64+24,new,72)
            with self.subTest(offset=offset,kind=kind),self.assertRaises((ValueError,Halt)):
                self.prove(b=b)
        b=bytearray(self.b);struct.pack_into('<I',b,shoff+3*64+4,19)
        with self.assertRaises((ValueError,Halt)):self.prove(b=b)

    def test_relocation_starting_before_consumer_pair_rejected(self):
        a=bytearray(self.a);a[CALL+4:CALL+12]=a[CALL:CALL+8];struct.pack_into('<I',a,CALL,0xd503201f)
        b=bytearray(self.b);shoff=struct.unpack_from('<Q',b,40)[0]
        text=struct.unpack_from('<Q',b,shoff+64+24)[0]
        sym=struct.unpack_from('<Q',b,shoff+4*64+24)[0]
        rel=struct.unpack_from('<Q',b,shoff+3*64+24)[0]
        newtext=len(b);b.extend(struct.pack('<I',0xd503201f)+b[text:text+8])
        struct.pack_into('<QQ',b,shoff+64+24,newtext,12)
        struct.pack_into('<Q',b,sym+2*24+16,12)
        newrel=len(b);b.extend(struct.pack('<QQq',4,(1<<32)|275,0)+struct.pack('<QQq',8,(1<<32)|277,0))
        struct.pack_into('<QQ',b,shoff+3*64+24,newrel,48)
        self.assertEqual(self.prove(a=a,b=b,function_size=12)['object_function_offset'],4)
        b.extend(struct.pack('<QQq',0,(2<<32)|257,0))
        struct.pack_into('<Q',b,shoff+3*64+32,72)
        with self.assertRaisesRegex(ValueError,'overlaps literal consumer'):
            self.prove(a=a,b=b,function_size=12)

    def test_zero_register_and_unaligned_function_rejected(self):
        a=bytearray(self.a);b=bytearray(self.b)
        shoff=struct.unpack_from('<Q',b,40)[0]
        text=struct.unpack_from('<Q',b,shoff+64+24)[0]
        for data,off in ((a,CALL),(b,text)):
            first,second=struct.unpack_from('<II',data,off)
            struct.pack_into('<II',data,off,(first&~31)|31,(second&~1023)|(31<<5)|31)
        with self.assertRaisesRegex(ValueError,'register flow'):
            self.prove(a=a,b=b)
        b=bytearray(self.b);sym=struct.unpack_from('<Q',b,shoff+4*64+24)[0]
        struct.pack_into('<Q',b,sym+2*24+8,1)
        struct.pack_into('<Q',b,shoff+64+32,12)
        with self.assertRaisesRegex(ValueError,'invalid candidate function section'):self.prove(b=b)

    def test_original_relocation_overlap_and_dynamic_write_fail_closed(self):
        for offset in (0x11ff, 0x1200, CALL):
            a=bytearray(self.a);struct.pack_into('<Q',a,RDYN,offset)
            with self.subTest(offset=offset),self.assertRaisesRegex(ValueError,'overlaps'):
                self.prove(a=a)
        a=bytearray(self.a);struct.pack_into('<q',a,DYN+10*16,17)
        with self.assertRaisesRegex(ValueError,'unsupported dynamic tags'):
            self.prove(a=a)
    def test_owner_and_ir_pins_fail_closed(self):
        self.source.write_bytes(b'other')
        with self.assertRaisesRegex(ValueError,'pin mismatch'):self.prove()
        self.source.write_bytes(b'source');self.ir.write_bytes(b'other')
        with self.assertRaisesRegex(ValueError,'pin mismatch'):self.prove()

    def test_object_string_extent_and_bytes(self):
        shoff=struct.unpack_from('<Q',self.b,40)[0]
        off=struct.unpack_from('<Q',self.b,shoff+2*64+24)[0]
        for index,value in ((0,0),(5,0),(17,1)):
            b=bytearray(self.b);b[off+index]=value
            with self.subTest(index=index),self.assertRaises((ValueError,Halt)):self.prove(b=b)
        for size in (17,19):
            b=bytearray(self.b);struct.pack_into('<Q',b,shoff+2*64+32,size)
            with self.subTest(size=size),self.assertRaises((ValueError,Halt)):self.prove(b=b)
    def test_bad_register_pair_relocation_and_storage_write(self):
        shoff=struct.unpack_from('<Q',self.b,40)[0]
        text=struct.unpack_from('<Q',self.b,shoff+64+24)[0]
        rela=struct.unpack_from('<Q',self.b,shoff+3*64+24)[0]
        for off,fmt,value in ((text+4,'<I',0x91000001),(rela+8,'<Q',(1<<32)|274),
                              (rela+16,'<q',1),(rela+24,'<Q',8),
                              (shoff+3*64+44,'<I',2)):
            b=bytearray(self.b);struct.pack_into(fmt,b,off,value)
            with self.subTest(off=off),self.assertRaises((ValueError,Halt)):self.prove(b=b)


if __name__=='__main__':unittest.main()
