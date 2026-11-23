"""Adversarial map/provenance fixtures; no linked-input claims."""
import hashlib
import json
from pathlib import Path
import tempfile
import unittest

from test_literal_proof import original, candidate
from test_import_proof import RDYN
from validation.common import Halt
from validation.linked_attribution import corroborate_contribution

INPUT='/work/build/2.6.2/CMakeFiles/d3_game_obj.dir/src/Contrib/Contrib/lua/src/lstate.c.obj'
SECTION='.rodata.str1.1'
HEADER='Address Size Align Out In Symbol\n'
ROW=f'0000000000001200 0000000000000012 1 {INPUT}:({SECTION})\n'


class AttributionTests(unittest.TestCase):
    def setUp(self):
        self.tmp=tempfile.TemporaryDirectory();self.addCleanup(self.tmp.cleanup);root=Path(self.tmp.name)
        self.link=root/'linked';self.map=root/'map';self.provenance=root/'map.provenance';self.obj=root/'owner.o'
        self.a=original();self.b=candidate();self.text=HEADER+ROW
    def prove(self, data=None, maptext=None, obj=None, mutate_record=None, **overrides):
        data=self.a if data is None else data;obj=self.b if obj is None else obj;maptext=self.text if maptext is None else maptext
        self.link.write_bytes(data);self.map.write_text(maptext);self.obj.write_bytes(obj)
        sha=lambda x:hashlib.sha256(x).hexdigest()
        record={'format':1,'elf_sha256':sha(data),'map_sha256':sha(maptext.encode())}
        if mutate_record:record.update(mutate_record)
        raw=json.dumps(record).encode();self.provenance.write_bytes(raw)
        return corroborate_contribution(self.link,overrides.get('elf_pin',sha(data)),self.map,sha(maptext.encode()),
               self.provenance,sha(raw),self.obj,sha(obj),overrides.get('identity',INPUT),SECTION)
    def test_explicit_corroboration_does_not_claim_input_or_linked_consumer(self):
        w=self.prove();self.assertEqual((w['address'],w['size']),(0x1200,18));self.assertTrue(w['merged_input'])
        for key in ('link_input_identity_proven','linked_consumer_proven','all_merge_aliases_proven','source_lineage_proven','receipt_acceptance'):
            self.assertFalse(w[key])
    def test_duplicate_missing_internal_and_overlap_are_rejected(self):
        for m in (HEADER,HEADER+ROW+ROW,HEADER+ROW.replace(INPUT,'<internal>'),
                  HEADER+ROW+f'00000000000011ff 0000000000000002 1 <internal>:(.rodata)\n'):
            with self.subTest(m=m),self.assertRaises(ValueError):self.prove(maptext=m)
    def test_wrong_address_extent_alignment_and_malformed_rows_rejected(self):
        for m in (HEADER+ROW.replace('0000000000001200','0000000000001201'),
                  HEADER+ROW.replace('0000000000000012','0000000000000011'),
                  HEADER+ROW.replace(' 1 ',' 2 '),HEADER+ROW+'bad duplicate :( invalid\n'):
            with self.subTest(m=m),self.assertRaises(ValueError):self.prove(maptext=m)
    def test_wrong_content_pin_record_and_identity_rejected(self):
        a=bytearray(self.a);a[0x1200]=0
        with self.assertRaisesRegex(ValueError,'linked bytes differ'):self.prove(data=a)
        with self.assertRaisesRegex(ValueError,'pin mismatch'):self.prove(elf_pin='0'*64)
        with self.assertRaisesRegex(ValueError,'provenance'):self.prove(mutate_record={'map_sha256':'0'*64})
        with self.assertRaisesRegex(ValueError,'input identity'):self.prove(identity='/work/build/2.6.2/../a.obj')
    def test_loader_write_overlap_rejected(self):
        a=bytearray(self.a)
        import struct
        struct.pack_into('<Q',a,RDYN,0x11ff)
        with self.assertRaisesRegex(ValueError,'overlaps proof range'):self.prove(data=a)


class LinkedLiteralTests(unittest.TestCase):
    def setUp(self):
        from test_literal_proof import LiteralTests
        self.fixture=LiteralTests('test_exact_witness');self.fixture.setUp();self.addCleanup(self.fixture.doCleanups)
        root=self.fixture.root
        self.link=root/'linked-elf';self.map=root/'linked-map';self.provenance=root/'linked-provenance';self.manifest=root/'link-inputs'
        self.a=self.fixture.a;self.b=self.fixture.b
        self.image=self.make_linked()
        self.text=HEADER+f'0000000000000400 0000000000000008 4 {INPUT}:(.text.f_luaopen)\n'
    def make_linked(self):
        import struct
        d=bytearray(self.a)
        if len(d)<0x3400:d.extend(bytes(0x3400-len(d)))
        shoff=struct.unpack_from('<Q',d,40)[0];old=struct.unpack_from('<Q',d,shoff+9*64+32)[0]
        extra=b'.symtab\0.strtab\0';d[0x2800+old:0x2800+old+len(extra)]=extra
        struct.pack_into('<Q',d,shoff+9*64+32,old+len(extra));struct.pack_into('<H',d,60,13)
        strings=b'\0f_luaopen\0';d[0x2a80:0x2a80+len(strings)]=strings
        syms=bytes(24)+struct.pack('<IBBHQQ',1,2,0,1,0x400,8);d[0x2a00:0x2a00+len(syms)]=syms
        struct.pack_into('<IIQQQQIIQQ',d,shoff+11*64,old,2,0,0,0x2a00,len(syms),12,2,8,24)
        struct.pack_into('<IIQQQQIIQQ',d,shoff+12*64,old+8,3,0,0,0x2a80,len(strings),0,0,1,0)
        return d
    def prove(self,image=None,maptext=None,manifest=None,**kw):
        from validation.linked_attribution import prove_linked_lstate_literal
        from validation.strict import build_owner_context
        image=self.image if image is None else image;maptext=self.text if maptext is None else maptext
        f=self.fixture;f.original.write_bytes(self.a);f.object.write_bytes(self.b)
        sha=lambda x:hashlib.sha256(x).hexdigest()
        pins={'original_sha256':sha(self.a),'functions_sha256':sha(f.functions.read_bytes()),'function_files_sha256':sha(f.files.read_bytes())}
        context=build_owner_context(f.root,'Contrib/Contrib/lua/src/lstate.c',pins)
        self.link.write_bytes(image);self.map.write_text(maptext)
        rec={'format':1,'elf_sha256':sha(image),'map_sha256':sha(maptext.encode())};rb=json.dumps(rec).encode();self.provenance.write_bytes(rb)
        manifest=manifest or {'kind':'link-input-manifest-v1','complete':True,'inputs':[{'path':INPUT,'sha256':sha(self.b)}]}
        mb=json.dumps(manifest).encode();self.manifest.write_bytes(mb)
        return prove_linked_lstate_literal(f.original,sha(self.a),f.object,sha(self.b),f.source,sha(b'source'),f.ir,sha(b'ir'),context,
                    self.link,sha(image),self.map,sha(maptext.encode()),self.provenance,sha(rb),self.manifest,sha(mb),kw.get('identity',INPUT))
    def test_internal_merge_correspondence_without_claiming_link_lineage(self):
        w=self.prove();self.assertEqual((w['function_address'],w['linked_consumer'],w['linked_literal_address']),(0x400,0x400,0x1200))
        self.assertTrue(w['linked_full_function_reconstructed']);self.assertFalse(w['manifest_actual_link_inputs_authenticated'])
        self.assertFalse(w['all_merge_aliases_proven']);self.assertFalse(w['receipt_acceptance'])
    def test_named_call_relocation_is_reconstructed_not_masked(self):
        import struct
        # Extend the synthetic function with an undefined named CALL26.
        self.a=bytearray(self.a);struct.pack_into('<Q',self.a,0x3000+64+32,0x100)
        struct.pack_into('<I',self.a,0x408,0x94000000|((0x460-0x408)//4))
        self.fixture.functions.write_text('Address,Quality,Size,Name\n0x7100000400,U,12,f_luaopen\n')
        b=bytearray(self.b);sh=struct.unpack_from('<Q',b,40)[0]
        text=struct.unpack_from('<Q',b,sh+64+24)[0];new=len(b);b.extend(b[text:text+8]+struct.pack('<I',0x94000000))
        struct.pack_into('<QQ',b,sh+64+24,new,12)
        syms=struct.unpack_from('<Q',b,sh+4*64+24)[0];newsym=len(b);raw=bytearray(b[syms:syms+72]);struct.pack_into('<Q',raw,2*24+16,12)
        string=struct.unpack_from('<Q',b,sh+5*64+24)[0];oldsize=struct.unpack_from('<Q',b,sh+5*64+32)[0]
        raw.extend(struct.pack('<IBBHQQ',oldsize,0x10,0,0,0,0));b.extend(raw);struct.pack_into('<QQ',b,sh+4*64+24,newsym,len(raw))
        newstr=len(b);b.extend(b[string:string+oldsize]+b'callee\0');struct.pack_into('<QQ',b,sh+5*64+24,newstr,oldsize+7)
        rela=struct.unpack_from('<Q',b,sh+3*64+24)[0];newrel=len(b);b.extend(b[rela:rela+48]+struct.pack('<QQq',8,(3<<32)|283,0));struct.pack_into('<QQ',b,sh+3*64+24,newrel,72)
        self.b=b;self.image=self.make_linked();image=bytearray(self.image);struct.pack_into('<Q',image,0x3000+64+32,0x100)
        struct.pack_into('<Q',image,0x2a00+24+16,12);strings=b'\0f_luaopen\0callee\0';image[0x2a80:0x2a80+len(strings)]=strings
        struct.pack_into('<Q',image,0x3000+12*64+32,len(strings));struct.pack_into('<Q',image,0x3000+11*64+32,72)
        struct.pack_into('<IBBHQQ',image,0x2a00+48,11,0x12,0,1,0x460,4)
        struct.pack_into('<I',image,0x408,0x94000000|((0x460-0x408)//4));struct.pack_into('<I',image,0x460,0xd65f03c0)
        self.image=image;self.text=HEADER+f'0000000000000400 000000000000000c 4 {INPUT}:(.text.f_luaopen)\n'
        result=self.prove();self.assertEqual(result['linked_calls'][0]['symbol'],'callee')
        image=bytearray(image);struct.pack_into('<I',image,0x408,0x94000000|((0x464-0x408)//4))
        with self.assertRaisesRegex(ValueError,'unproved instruction changes'):self.prove(image=image)

    def test_same_owner_section_call_requires_exact_map_route(self):
        import struct
        self.test_named_call_relocation_is_reconstructed_not_masked()
        b=bytearray(self.b);sh=struct.unpack_from('<Q',b,40)[0]
        # Turn the third symbol into a local section symbol and add a local FUNC.
        sym=struct.unpack_from('<Q',b,sh+4*64+24)[0];st=struct.unpack_from('<Q',b,sh+5*64+24)[0]
        stsize=struct.unpack_from('<Q',b,sh+5*64+32)[0];newstr=len(b);b.extend(b[st:st+stsize]+b'stack_init\0')
        struct.pack_into('<QQ',b,sh+5*64+24,newstr,stsize+11)
        raw=bytearray(b[sym:sym+96]);struct.pack_into('<IBBHQQ',raw,3*24,0,3,0,7,0,0)
        raw.extend(struct.pack('<IBBHQQ',stsize,2,0,7,0,4));newsym=len(b);b.extend(raw)
        struct.pack_into('<QQ',b,sh+4*64+24,newsym,len(raw))
        names=struct.unpack_from('<Q',b,sh+6*64+24)[0];nsize=struct.unpack_from('<Q',b,sh+6*64+32)[0]
        newnames=len(b);b.extend(b[names:names+nsize]+b'.text.stack_init\0');struct.pack_into('<QQ',b,sh+6*64+24,newnames,nsize+17)
        target_code=len(b);b.extend(struct.pack('<I',0xd65f03c0))
        header=struct.pack('<IIQQQQIIQQ',nsize,1,6,0,target_code,4,0,0,4,0)
        newsh=len(b);b.extend(b[sh:sh+7*64]+header);struct.pack_into('<Q',b,40,newsh);struct.pack_into('<H',b,60,8)
        self.b=b;image=bytearray(self.image);names=b'\0f_luaopen\0stack_init\0';image[0x2a80:0x2a80+len(names)]=names
        struct.pack_into('<Q',image,0x3000+12*64+32,len(names));struct.pack_into('<IBBHQQ',image,0x2a00+48,11,2,0,1,0x460,4)
        self.image=image;row=f'0000000000000460 0000000000000004 4 {INPUT}:(.text.stack_init)\n';self.text+=row
        w=self.prove();self.assertEqual(w['linked_calls'][0]['symbol'],'stack_init')
        with self.assertRaisesRegex(ValueError,'missing or ambiguous explicit function'):
            self.prove(maptext=self.text.replace(INPUT+':(.text.stack_init)','/work/build/2.6.2/other.obj:(.text.stack_init)'))

    def test_function_route_duplicate_wrong_owner_or_bad_manifest_rejected(self):
        with self.assertRaisesRegex(ValueError,'ambiguous explicit function'):self.prove(maptext=self.text+self.text.splitlines(True)[1])
        with self.assertRaisesRegex(ValueError,'wrong owner linker path'):self.prove(identity='/work/build/2.6.2/elsewhere.obj')
        bad={'kind':'link-input-manifest-v1','complete':True,'inputs':[{'path':INPUT,'sha256':'0'*64}]}
        with self.assertRaisesRegex(ValueError,'manifest owner input mismatch'):self.prove(manifest=bad)
    def test_linked_function_or_literal_mutations_rejected(self):
        for at in (0x400,0x404,0x1200,0x1211):
            image=bytearray(self.image);image[at]^=1
            with self.subTest(at=at),self.assertRaises((ValueError,Halt)):self.prove(image=image)
    def test_tlsdesc_overlap_and_unaligned_function_extent(self):
        import struct
        for position in (0x1200-8,0x400-8):
            image=bytearray(self.image);struct.pack_into('<QQ',image,RDYN,position,1031)
            with self.subTest(position=position),self.assertRaisesRegex(ValueError,'linked relocation overlaps'):
                self.prove(image=image)
        image=bytearray(self.image);struct.pack_into('<Q',image,RDYN,(1<<64)-8);struct.pack_into('<Q',image,RDYN+8,1031)
        with self.assertRaisesRegex(ValueError,'overflows address space'):self.prove(image=image)
        image=bytearray(self.image);struct.pack_into('<Q',image,0x2a00+24+16,7)
        with self.assertRaisesRegex(ValueError,'unaligned linked function extent'):self.prove(image=image)

    def test_zero_sized_function_alias_in_owner_extent_rejected(self):
        import struct
        image=bytearray(self.image);names=b'\0f_luaopen\0alias\0';image[0x2a80:0x2a80+len(names)]=names
        struct.pack_into('<Q',image,0x3000+12*64+32,len(names));struct.pack_into('<Q',image,0x3000+11*64+32,72)
        struct.pack_into('<IBBHQQ',image,0x2a00+48,11,2,0,1,0x400,0)
        with self.assertRaisesRegex(ValueError,'overlapping linked function aliases'):
            self.prove(image=image)

    def test_internal_map_rows_allowed_away_from_function(self):
        m=self.text+'0000000000001200 0000000000000012 1 <internal>:(.rodata)\n'
        self.assertEqual(self.prove(maptext=m)['linked_literal_address'],0x1200)


if __name__=='__main__':unittest.main()
