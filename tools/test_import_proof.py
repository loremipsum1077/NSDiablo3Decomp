"""Minimal executable ELF fixtures for original-only import-chain witnesses."""
import hashlib
import os
from pathlib import Path
import struct
import tempfile
import unittest

from validation.imports import prove_original_call

CALL, PLT, SLOT = 0x400, 0x520, 0x2118
RELA, DYN, SYM, STR, RDYN = 0x1000, 0x2000, 0x1080, 0x10c0, 0x1040
SHOFF = 0x3000


def fixture():
    data = bytearray(0x3300)
    data[:64] = struct.pack('<16sHHIQQQIHHHHHH', b'\x7fELF\x02\x01\x01'+bytes(9),
                            3,183,1,0,64,SHOFF,0,64,56,4,64,10,9)
    for i, ph in enumerate(((1,5,0,0,0,0x700,0x700,1),
                            (1,4,0x1000,0x1000,0x1000,0x400,0x400,1),
                            (1,6,0x2000,0x2000,0x2000,0x400,0x400,1),
                            (2,6,DYN,DYN,DYN,12*16,12*16,8))):
        struct.pack_into('<IIQQQQQQ',data,64+i*56,*ph)
    struct.pack_into('<I',data,CALL,0x94000000 | ((PLT-CALL)//4))
    # adrp x16, slot page; ldr x17,[x16,#lo]; add x16,x16,#lo; br x17
    delta=((SLOT & ~4095)-(PLT & ~4095))//4096
    adrp=0x90000010 | ((delta&3)<<29) | (((delta>>2)&0x7ffff)<<5)
    struct.pack_into('<IIII',data,PLT,adrp,0xf9400211|(((SLOT&4095)//8)<<10),
                     0x91000210|((SLOT&4095)<<10),0xd61f0220)
    struct.pack_into('<QQq',data,RELA,SLOT,(1<<32)|1026,0)
    struct.pack_into('<QQq',data,RDYN,0x2200,1027,0)
    struct.pack_into('<IBBHQQ',data,SYM+24,1,0x12,0,0,0,0)
    data[STR:STR+8]=b'\0strchr\0'
    tags=((3,0x2100),(2,24),(23,RELA),(20,7),(7,RDYN),(8,24),(9,24),
          (6,SYM),(11,24),(5,STR),(10,8),(0,0))
    for i, entry in enumerate(tags):struct.pack_into('<qQ',data,DYN+i*16,*entry)
    names=b'\0.text\0.plt\0.got.plt\0.rela.plt\0.dynamic\0.dynsym\0.dynstr\0.rela.dyn\0.shstrtab\0'
    data[0x2800:0x2800+len(names)]=names
    def n(name):return names.index(b'\0'+name.encode()+b'\0')+1
    sections=[(0,0,0,0,0,0,0,0,0,0),
      (n('.text'),1,6,CALL,CALL,16,0,0,4,0),
      (n('.plt'),1,6,0x500,0x500,48,0,0,16,16),
      (n('.got.plt'),1,3,0x2100,0x2100,32,0,0,8,8),
      (n('.rela.plt'),4,2,RELA,RELA,24,6,2,8,24),
      (n('.dynamic'),6,3,DYN,DYN,12*16,7,0,8,16),
      (n('.dynsym'),11,2,SYM,SYM,48,7,1,8,24),
      (n('.dynstr'),3,2,STR,STR,8,0,0,1,0),
      (n('.rela.dyn'),4,2,RDYN,RDYN,24,6,0,8,24),
      (n('.shstrtab'),3,0,0,0x2800,len(names),0,0,1,0)]
    for i,sec in enumerate(sections):struct.pack_into('<IIQQQQIIQQ',data,SHOFF+i*64,*sec)
    return data


class OriginalImportTests(unittest.TestCase):
    def setUp(self):
        self.temp=tempfile.TemporaryDirectory();self.addCleanup(self.temp.cleanup)
        self.path=Path(self.temp.name)/'original.elf';self.data=fixture()
    def prove(self, data=None, site=CALL, symbol='strchr', pin=None):
        data=self.data if data is None else data
        self.path.write_bytes(data)
        return prove_original_call(self.path, pin or hashlib.sha256(data).hexdigest(), site, symbol)
    def test_complete_original_chain(self):
        proof=self.prove()
        self.assertEqual((proof['call_address'],proof['plt_address'],proof['got_address']), (CALL,PLT,SLOT))
        self.assertEqual((proof['relocation_type'],proof['dynamic_symbol_index']), (1026,1))
        self.assertIn('no candidate',proof['scope'])
    def test_wrong_pin_call_and_import_rejected(self):
        for kw in ({'pin':'0'*64},{'site':CALL+2},{'site':CALL+4},{'symbol':'other'}):
            with self.subTest(kw=kw),self.assertRaises(ValueError):self.prove(**kw)
    def test_malformed_instruction_stub_and_slot_rejected(self):
        changes=((CALL,0x14000000), (CALL,0x94000000|((PLT+4-CALL)//4)),
                 (PLT,0x9000000f),(PLT+4,0xf9400210),(PLT+8,0x91000211),
                 (PLT+12,0xd61f0200),(PLT+4,0xf9400211|((0x120//8)<<10)))
        for off,word in changes:
            with self.subTest(off=off,word=word):
                data=bytearray(self.data);struct.pack_into('<I',data,off,word)
                with self.assertRaises(ValueError):self.prove(data)
    def test_relocation_and_symbol_constraints(self):
        cases=[(RELA,'<Q',SLOT+8),(RELA+8,'<Q',(1<<32)|1025),(RELA+16,'<q',1),
               (SYM+24+4,'<B',0x22),(SYM+24+4,'<B',0x10),(SYM+24+5,'<B',2),
               (SYM+24+6,'<H',1),(SYM+24+8,'<Q',1),(RDYN,'<Q',SLOT)]
        for off,fmt,value in cases:
            with self.subTest(off=off,value=value):
                data=bytearray(self.data);struct.pack_into(fmt,data,off,value)
                with self.assertRaises(ValueError):self.prove(data)
        data=bytearray(self.data);data[SYM:SYM+24]=data[SYM+24:SYM+48]
        with self.assertRaisesRegex(ValueError,'ambiguous imported symbol'):self.prove(data)
    def test_mapping_and_dynamic_chain_must_match(self):
        cases=[(64+4,'<I',6),  # text LOAD loses executable
               (64+56+32,'<Q',0x20),  # readonly LOAD too short
               (64+3*56+8,'<Q',DYN+8),  # PT_DYNAMIC offset disagrees
               (DYN+16*2+8,'<Q',RELA+24), # DT_JMPREL differs
               (DYN+16*3+8,'<Q',17), # REL instead of RELA
               (DYN+16*10,'<q',0x6ffffff0), # unmodelled version tag
               (DYN+16*10,'<q',17), # unsupported DT_REL
               (DYN+16*10,'<q',23), # duplicate DT_JMPREL
               (64+56+16,'<Q',0x500), # overlapping PT_LOAD
               (SHOFF+4*64+40,'<I',7)] # relocation linked to wrong table
        for off,fmt,value in cases:
            with self.subTest(off=off,value=value):
                data=bytearray(self.data);struct.pack_into(fmt,data,off,value)
                with self.assertRaises(ValueError):self.prove(data)
    def test_relative_count_and_nonregular_input_fail_closed(self):
        # Add an optional count before the dynamic terminator.
        data=bytearray(self.data)
        struct.pack_into('<qQ', data, DYN+11*16, 0x6ffffff9, 2)
        # Extend the dynamic section and PT_DYNAMIC to retain the terminator.
        struct.pack_into('<qQ', data, DYN+12*16, 0, 0)
        struct.pack_into('<QQ', data, 64+3*56+32, 13*16, 13*16)
        struct.pack_into('<Q', data, SHOFF+5*64+32, 13*16)
        with self.assertRaisesRegex(ValueError, 'count exceeds'):
            self.prove(data)
        struct.pack_into('<Q', data, DYN+11*16+8, 1)
        struct.pack_into('<Q', data, RDYN+8, (1<<32)|257)
        with self.assertRaisesRegex(ValueError, 'prefix does not match'):
            self.prove(data)
        fifo=Path(self.temp.name)/'fifo';os.mkfifo(fifo)
        with self.assertRaisesRegex(ValueError, 'bounded regular file'):
            prove_original_call(fifo, '0'*64, CALL, 'strchr')

    def test_truncated_file_rejected(self):
        with self.assertRaises(ValueError):self.prove(self.data[:0x200])


if __name__=='__main__':unittest.main()
