"""Focused fail-closed scalar proof tests; no compiler or Docker."""
import csv
import hashlib
from pathlib import Path
import struct
import tempfile
from types import SimpleNamespace
import unittest
from unittest.mock import patch

import recovery_scalar as s
from validation.common import Halt
from validation.elf import Elf
from data_relocations import RelocationImage

ROOT=Path(__file__).resolve().parents[1]


class ScalarPrimitivesTests(unittest.TestCase):
    def test_adrp_and_load_register_validation(self):
        self.assertEqual(s._page(0x1000,0x90000008),(0x1000,8))
        self.assertEqual(s._load(0xbd400101),(0,8))
        with self.assertRaises(Halt):s._page(0,0x9000001f)
        with self.assertRaises(Halt):s._load(0xb9400101)

    def test_address_materialization_rejects_sp_or_different_register(self):
        self.assertEqual(s._add(0x91000108),(0,8))
        for w in (0x910003ff,0x91000109,0x11000108):
            with self.subTest(word=w),self.assertRaises(Halt):s._add(w)

    def test_pool_rejects_duplicate_values_even_if_link_bytes_are_rehashed(self):
        image=SimpleNamespace(metadata={},blob=b'abcdabcd')
        image.object_bytes=lambda *_:(image.blob,'file')
        line=['00000000 00000008 4 <internal>:(.rodata)']
        with patch.object(s,'_mapped',return_value=0):
            with self.assertRaises(Halt):s._pool(line,image)

    def test_pool_reports_exact_contents(self):
        image=SimpleNamespace(metadata={},blob=b'abcdefgh')
        image.object_bytes=lambda *_:(image.blob,'file')
        with patch.object(s,'_mapped',return_value=0):
            self.assertEqual(s._pool(['00000000 00000008 4 <internal>:(.rodata)'],image)[2],{b'abcd':0,b'efgh':4})

    def test_map_requires_exact_object_alignment_and_text_output(self):
        lines=['00000100 00000100 4 .text','00000120 00000010 4 /work/build/2.6.2/x.obj:(.text.fn)']
        self.assertEqual(s._contribution(lines,'x.obj','.text.fn',4),(0x120,16))
        for bad in (lines+[lines[1]], [lines[0],lines[1].replace(' 4 /work',' 8 /work')],
                    [lines[0],lines[1].replace('x.obj','y.obj')],
                    ['00000200 00000100 4 .text',lines[1]]):
            with self.subTest(lines=bad),self.assertRaises(Halt):s._contribution(bad,'x.obj','.text.fn',4)

    def test_map_rejects_second_overlapping_contribution(self):
        lines=['00000100 00000100 4 .text','00000120 00000010 4 /work/build/2.6.2/x.obj:(.text.fn)',
               '00000128 00000008 4 /work/build/2.6.2/y.obj:(.text.other)']
        with self.assertRaises(Halt):s._contribution(lines,'x.obj','.text.fn',4)

    def test_pool_rejects_unowned_added_or_omitted_cell(self):
        s._pool_ownership({s.VALUE:0,b'abcd':4},{s.VALUE:8,b'abcd':0},{s.VALUE,b'abcd'})
        for before,after,inputs in (({s.VALUE:0,b'abcd':4},{s.VALUE:8,b'abcd':0},{s.VALUE}),
                                    ({s.VALUE:0,b'abcd':4},{s.VALUE:8},{s.VALUE,b'abcd'}),
                                    ({s.VALUE:0,b'abcd':4},{s.VALUE:8,b'abcd':0},{b'abcd'})):
            with self.assertRaises(Halt):s._pool_ownership(before,after,inputs)

    def test_allocated_debug_reference_is_rejected(self):
        s._debug_reference('.debug_info',0,4,4)
        with self.assertRaises(Halt):s._debug_reference('.debug_info',2,4,4)

    def test_rehashed_wrong_relocation_immediate_is_rejected(self):
        # Both words remain valid instructions, but the candidate points four bytes away.
        with patch.object(s,'_u32',side_effect=[0x90000008,0xbd400101,0x90000008,0xbd400501]):
            with self.assertRaises(Halt):s._relocated_reference([None,None],[0,0],0,4,285,[0,0])

    def test_wrong_owner_is_refused_before_reading_receipts(self):
        with self.assertRaises(Halt):s.prove('missing','missing','missing','missing','missing',[],'wrong')

    def test_rehashed_wrong_linked_instruction_bits_are_rejected(self):
        code=struct.pack('<II',0x90000008,0xbd400101)
        e=SimpleNamespace(sections=[(0,1,6,0,0,8,0,0,4,0)],d=code)
        good=SimpleNamespace(metadata={},blob=code)
        bad=SimpleNamespace(metadata={},blob=code[:4]+struct.pack('<I',0xbd400102))
        good.unmodeled_relocations=bad.unmodeled_relocations=False
        good.object_relocations=bad.object_relocations=lambda *_:[]
        good.object_bytes=lambda *_:(good.blob,'file')
        bad.object_bytes=lambda *_:(bad.blob,'file')
        with patch.object(s,'_word_masks',return_value={0:s.MASKS[275],4:s.MASKS[285]}), \
             patch.object(s,'_contribution',return_value=(0,8)),patch.object(s,'_mapped',return_value=0), \
             patch.object(s,'_section',return_value={'address':0,'size':8}):
            with self.assertRaises(Halt):s._code(e,0,'x.obj','.text.fn',[good,bad],[['0 8 4 .text']]*2)


class RetainedOwnerTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.owner=ROOT/'build/work/X_Client_Behavior_RadiusTrigger/build/radius-evidence/attempt-02.o'
        cls.original=ROOT/'data/2.6.2/main.elf';cls.nss=ROOT/'build/romfs/DiabloIIINX64r.nss'
        cls.link=ROOT/'build/game-recovery-radius-v2/relink/normal/candidate/d3'
        if not all(p.is_file() for p in (cls.owner,cls.original,cls.nss,cls.link,Path(str(cls.link)+'.map'))):
            raise unittest.SkipTest('retained Radius diagnostic artifacts unavailable')
        with (ROOT/'data/2.6.2/functions.csv').open() as f:
            cls.rows=[x for x in csv.DictReader(f) if 'BehaviorRadiusTrigger' in x['Name']]
        cls.lines=Path(str(cls.link)+'.map').read_text().splitlines();cls.image=RelocationImage(cls.link)
        cls.relative='CMakeFiles/d3_game_obj.dir/src/X/Client/Behavior/RadiusTrigger.cpp.obj'

    def prove(self,path,rows=None):
        return s._owner_proof(self.original,self.nss,path,self.rows if rows is None else rows,self.image,self.lines,self.relative)

    def test_retained_owner(self):
        witness=self.prove(self.owner)
        self.assertEqual((witness['offset'],witness['bytes']),(340,'8988883c'))

    def test_rehashed_wrong_scalar_is_rejected(self):
        e=Elf(self.owner);names=s._names(e);idx=names.index('.rodata.cst4')
        b=bytearray(e.d);b[e.sections[idx][4]]^=1
        with tempfile.TemporaryDirectory() as tmp:
            p=Path(tmp)/'mutated.o';p.write_bytes(b)
            self.assertNotEqual(hashlib.sha256(b).hexdigest(),hashlib.sha256(e.d).hexdigest())
            with self.assertRaises(Halt):self.prove(p)

    def test_omitted_owner_identity_is_rejected(self):
        with self.assertRaises(Halt):self.prove(self.owner,self.rows[:-1])

    def test_rehashed_wrong_owner_load_opcode_is_rejected(self):
        e=Elf(self.owner);names=s._names(e);idx=names.index('.text.'+s.FUNCTION)
        b=bytearray(e.d);struct.pack_into('<I',b,e.sections[idx][4]+344,0xb9400101)
        with tempfile.TemporaryDirectory() as tmp:
            p=Path(tmp)/'mutated.o';p.write_bytes(b)
            with self.assertRaises(Halt):self.prove(p)

    def test_rehashed_real_dollar_name_is_not_discarded_as_marker(self):
        data=self.owner.read_bytes();needle=b'$d.2\0';self.assertEqual(data.count(needle),1)
        with tempfile.TemporaryDirectory() as tmp:
            p=Path(tmp)/'mutated.o';p.write_bytes(data.replace(needle,b'$dog\0'))
            with self.assertRaises(Halt):self.prove(p)

    def test_rehashed_omitted_scalar_relocation_is_rejected(self):
        e=Elf(self.owner);names=s._names(e);idx=names.index('.rela.text.'+s.FUNCTION);sec=e.sections[idx]
        b=bytearray(e.d);found=False
        for off in range(sec[4],sec[4]+sec[5],24):
            pos,info,add=struct.unpack_from('<QQq',b,off)
            if pos==344 and (info&0xffffffff)==285:
                struct.pack_into('<Q',b,off+8,285);found=True;break
        self.assertTrue(found)
        with tempfile.TemporaryDirectory() as tmp:
            p=Path(tmp)/'mutated.o';p.write_bytes(b)
            with self.assertRaises(Halt):self.prove(p)

if __name__=='__main__':unittest.main()
