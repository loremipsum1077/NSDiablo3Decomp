"""Synthetic pinned ELF fixtures for bounded Lua switch-table correspondence."""
import hashlib
from pathlib import Path
import struct
import tempfile
import unittest

from test_literal_proof import original as original_fixture
from test_import_proof import SHOFF, RDYN
from validation.common import Halt
from validation.strict import build_owner_context
from validation.tables import PATTERNS, OWNER, prove_lgc_tables

NAMES=list(PATTERNS)
BASES=[0x300,0x380,0x400,0x480]
TABLES=[0x1200,0x1220,0x1240,0x1260]
COUNTS=[5,7,7,5]
PROLOGUE=[0xd10143ff,0xa9047bfd,0x910103fd,0xaa0103e2,0xaa0003e3,0xf81f83a0,0xf81f03a1,0xf85f03a0]


def dispatch(name, original_address=None, function_base=None):
    prefix,_,_,load=PATTERNS[name]
    branch=0x54000008 | (((0x7c-0x40)//4)<<5)
    adrp=0x90000008;add=0x91000108
    if original_address is not None:
        pc=function_base+0x44;delta=((original_address&~4095)-(pc&~4095))//4096
        adrp|=(delta&3)<<29|((delta>>2)&0x7ffff)<<5
        add|=(original_address&4095)<<10
    return prefix+[branch,adrp,add,load,0xb8a9790a,0x8b080148,0xd61f0100]


def original():
    d=original_fixture()
    struct.pack_into('<QQQ',d,SHOFF+64+16,0x300,0x300,0x200) # text virtual/file/size
    struct.pack_into('<Q',d,SHOFF+10*64+32,0x80) # readonly table range
    for i,name in enumerate(NAMES):
        base=BASES[i];buf=bytearray(128)
        if name=='freeobj':struct.pack_into('<8I',buf,0,*PROLOGUE)
        struct.pack_into('<15I',buf,0x20,*dispatch(name,TABLES[i],base))
        d[base:base+128]=buf
        for j in range(COUNTS[i]):struct.pack_into('<i',d,TABLES[i]+j*4,base+0x5c+4*j-TABLES[i])
    return d


def candidate():
    specs=[]
    for i,name in enumerate(NAMES):
        code=bytearray(128)
        if name=='freeobj':struct.pack_into('<8I',code,0,*PROLOGUE)
        struct.pack_into('<15I',code,0x20,*dispatch(name))
        specs.extend([('.text.'+name,1,6,code,0,0,4,0),('.rodata.'+name,1,2,bytes(COUNTS[i]*4),0,0,4,0)])
    symtabidx=17;stridx=18;shstridx=19
    symbols=bytes(24)
    for i in range(4):symbols+=struct.pack('<IBBHQQ',0,3,0,2+i*2,0,0)
    for i in range(4):symbols+=struct.pack('<IBBHQQ',0,3,0,1+i*2,0,0)
    strtab=bytearray(b'\0')
    for i,name in enumerate(NAMES):
        n=len(strtab);strtab.extend(name.encode()+b'\0');symbols+=struct.pack('<IBBHQQ',n,2,0,1+i*2,0,128)
    for i,name in enumerate(NAMES):
        rel=struct.pack('<QQq',0x44,((1+i)<<32)|275,0)+struct.pack('<QQq',0x48,((1+i)<<32)|277,0)
        specs.append(('.rela.text.'+name,4,0,rel,symtabidx,1+i*2,8,24))
    for i,name in enumerate(NAMES):
        rel=b''.join(struct.pack('<QQq',j*4,((5+i)<<32)|261,0x5c+j*8) for j in range(COUNTS[i]))
        specs.append(('.rela.rodata.'+name,4,0,rel,symtabidx,2+i*2,8,24))
    specs.append(('.symtab',2,0,symbols,stridx,9,8,24));specs.append(('.strtab',3,0,strtab,0,0,1,0))
    names=b'\0'+b''.join(x[0].encode()+b'\0' for x in specs)+b'.shstrtab\0.eh_frame\0'
    specs.append(('.shstrtab',3,0,names,0,0,1,0));specs.append(('.eh_frame',1,2,b'',0,0,8,0))
    d=bytearray(64);headers=[bytes(64)]
    for name,typ,flags,body,link,info,align,entry in specs:
        while len(d)%8:d.append(0)
        ni=names.index(b'\0'+name.encode()+b'\0')+1
        headers.append(struct.pack('<IIQQQQIIQQ',ni,typ,flags,0,len(d),len(body),link,info,align,entry));d.extend(body)
    while len(d)%8:d.append(0)
    shoff=len(d);d.extend(b''.join(headers));d[:64]=struct.pack('<16sHHIQQQIHHHHHH',b'\x7fELF\x02\x01\x01'+bytes(9),1,183,1,0,0,shoff,0,64,0,0,64,len(headers),shstridx)
    return d


class TableTests(unittest.TestCase):
    def setUp(self):
        self.tmp=tempfile.TemporaryDirectory();self.addCleanup(self.tmp.cleanup);self.root=Path(self.tmp.name)
        self.base=self.root/'data/2.6.2';self.base.mkdir(parents=True);self.orig=self.base/'main.elf'
        self.functions=self.base/'functions.csv';self.files=self.base/'function_files.csv'
        self.functions.write_text('Address,Quality,Size,Name\n'+''.join(f'0x{0x7100000000+base:x},U,128,{name}\n' for base,name in zip(BASES,NAMES)))
        self.files.write_text('Address,File,Compiler\n'+''.join(f'0x{0x7100000000+base:x},{OWNER},clang\n' for base in BASES))
        self.obj=self.root/'object';self.owner=self.root/'owner';self.ir=self.root/'ir';self.owner.write_bytes(b'owner');self.ir.write_bytes(b'ir')
        self.a=original();self.b=candidate()
    def prove(self,a=None,b=None,**kwargs):
        a=self.a if a is None else a;b=self.b if b is None else b
        self.orig.write_bytes(a);self.obj.write_bytes(b);sha=lambda x:hashlib.sha256(x).hexdigest()
        pins={'original_sha256':sha(a),'functions_sha256':sha(self.functions.read_bytes()),'function_files_sha256':sha(self.files.read_bytes())}
        c=build_owner_context(self.root,OWNER,pins)
        return prove_lgc_tables(self.orig,kwargs.get('pin',sha(a)),self.obj,sha(b),self.owner,sha(b'owner'),self.ir,sha(b'ir'),c)
    def test_four_complete_tables(self):
        w=self.prove();self.assertEqual([t['extent_bytes'] for t in w['tables']],[20,28,28,20]);self.assertEqual(sum(len(t['slots']) for t in w['tables']),24)
        self.assertFalse(w['candidate_linked_data_proven']);self.assertFalse(w['tables'][0]['dispatch']['global_control_flow_proven'])
    def test_table_and_target_corruption_rejected(self):
        sh=struct.unpack_from('<Q',self.b,40)[0];table=struct.unpack_from('<Q',self.b,sh+2*64+24)[0];rel=struct.unpack_from('<Q',self.b,sh+13*64+24)[0]
        changes=[(table,'<B',1),(sh+2*64+32,'<Q',16),(rel+8,'<Q',(5<<32)|257),
                 (rel+16,'<q',0),(rel+24,'<Q',0),(rel+24+16,'<q',0x60)]
        for off,fmt,val in changes:
            b=bytearray(self.b);struct.pack_into(fmt,b,off,val)
            with self.subTest(off=off,val=val),self.assertRaises((ValueError,Halt)):self.prove(b=b)
        a=bytearray(self.a);struct.pack_into('<i',a,TABLES[0],0)
        with self.assertRaises((ValueError,Halt)):self.prove(a=a)
    def test_dispatch_bounds_default_register_and_frame_rejected(self):
        sh=struct.unpack_from('<Q',self.b,40)[0]
        for i,off,word in ((0,0x30,0x71001508),(0,0x40,0x54000009),(0,0x50,0xb8a9791f),
                           (0,0x58,0xd61f03e0),(2,0x08,0x9100e3fd)):
            b=bytearray(self.b);text=struct.unpack_from('<Q',b,sh+(1+2*i)*64+24)[0];struct.pack_into('<I',b,text+off,word)
            with self.subTest(i=i,off=off),self.assertRaises((ValueError,Halt)):self.prove(b=b)
    def test_extra_consumer_overlapping_dispatch_and_target_into_guard(self):
        sh=struct.unpack_from('<Q',self.b,40)[0]
        # Redirect another function's table consumer to the first table.
        b=bytearray(self.b);rela2=struct.unpack_from('<Q',b,sh+10*64+24)[0]
        struct.pack_into('<Q',b,rela2+8,(1<<32)|275)
        with self.assertRaisesRegex(ValueError,'ambiguous table consumers'):self.prove(b=b)
        # Extra ABS64 write starts before and overlaps the 60-byte guard window.
        b=bytearray(self.b);rela=struct.unpack_from('<Q',b,sh+9*64+24)[0];new=len(b)
        b.extend(b[rela:rela+48]);b.extend(struct.pack('<QQq',0x1c,(5<<32)|257,0))
        struct.pack_into('<QQ',b,sh+9*64+24,new,72)
        with self.assertRaisesRegex(ValueError,'relocation overlaps dispatch'):self.prove(b=b)
        # Original and candidate agree on an interior target, but it bypasses guard.
        a=bytearray(self.a);b=bytearray(self.b);rel=struct.unpack_from('<Q',b,sh+13*64+24)[0]
        struct.pack_into('<q',b,rel+16,0x30);struct.pack_into('<i',a,TABLES[0],BASES[0]+0x30-TABLES[0])
        with self.assertRaisesRegex(ValueError,'table target differs'):self.prove(a=a,b=b)

    def test_nonzero_function_section_offset_preserves_relocation_arithmetic(self):
        b=bytearray(self.b);sh=struct.unpack_from('<Q',b,40)[0]
        text=struct.unpack_from('<Q',b,sh+64+24)[0];new=len(b)
        b.extend(bytes(4)+b[text:text+128]);struct.pack_into('<QQ',b,sh+64+24,new,132)
        syms=struct.unpack_from('<Q',b,sh+17*64+24)[0]
        struct.pack_into('<Q',b,syms+9*24+8,4)
        code_reloc=struct.unpack_from('<Q',b,sh+9*64+24)[0]
        struct.pack_into('<Q',b,code_reloc,0x48);struct.pack_into('<Q',b,code_reloc+24,0x4c)
        table_reloc=struct.unpack_from('<Q',b,sh+13*64+24)[0]
        for i in range(COUNTS[0]):
            off=table_reloc+i*24+16;old=struct.unpack_from('<q',b,off)[0];struct.pack_into('<q',b,off,old+4)
        table=self.prove(b=b)['tables'][0]
        self.assertEqual(table['object_function_start'],4)
        for slot in table['slots']:
            self.assertEqual(slot['candidate_signed_in_original_layout'],slot['original_signed'])

    def test_freeobj_prologue_relocation_write_rejected(self):
        sh=struct.unpack_from('<Q',self.b,40)[0];b=bytearray(self.b)
        rel=struct.unpack_from('<Q',b,sh+11*64+24)[0];new=len(b)
        b.extend(b[rel:rel+48]);b.extend(struct.pack('<QQq',8,(7<<32)|257,0))
        struct.pack_into('<QQ',b,sh+11*64+24,new,72)
        with self.assertRaisesRegex(ValueError,'overlaps freeobj prologue'):self.prove(b=b)
        a=bytearray(self.a);struct.pack_into('<Q',a,RDYN,BASES[2]+8)
        with self.assertRaisesRegex(ValueError,'overlaps proof range'):self.prove(a=a)

    def test_original_write_overlap_and_wrong_pin(self):
        a=bytearray(self.a);struct.pack_into('<Q',a,RDYN,TABLES[0]-4)
        with self.assertRaisesRegex(ValueError,'overlaps proof range'):self.prove(a=a)
        with self.assertRaises(ValueError):self.prove(pin='0'*64)


if __name__=='__main__':unittest.main()
