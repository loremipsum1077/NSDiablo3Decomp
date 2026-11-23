"""Zero-fill corroboration rejects ambiguous or unsupported image evidence."""
import copy
import hashlib
import tempfile
import unittest
from pathlib import Path
from unittest.mock import patch
import original_zerofill_evidence as evidence

class ZeroFillTests(unittest.TestCase):
    def setUp(self):
        tmp=tempfile.TemporaryDirectory();self.addCleanup(tmp.cleanup)
        self.a=Path(tmp.name)/'original';self.b=Path(tmp.name)/'nss'
        self.a.write_bytes(b'proof');self.b.write_bytes(b'proof')
        self.sha=hashlib.sha256(b'proof').hexdigest()
        sec=dict(index=1,type=8,flags=3,address=0x1005,size=0x100,alignment=8)
        load=dict(index=0,type=1,flags=6,address=0x1000,offset=0,file_size=5,memory_size=0x105,alignment=8)
        sym=dict(name='target',address=0x1010,size=8,type=1,binding=1,visibility=0,defined=True,section_index=1,table_section=2)
        self.original=dict(sha256=self.sha,elf_type=3,machine=183,segments=[load],sections=[sec,dict(index=2,type=2,flags=0,address=0,size=0)],symbols=[],relocations=[],dynamic_tags=[])
        self.nss=copy.deepcopy(self.original);self.nss['symbols']=[sym]
    def run_case(self, *, digest=None, name='target', address=0x1010):
        with patch.object(evidence,'inspect_elf',side_effect=[self.original,self.nss]):
            return evidence.extract(self.a,self.b,digest or self.sha,self.sha,name,address)
    def test_positive(self):
        result=self.run_case();self.assertEqual(result['nss_symbol']['size'],8)
        self.assertIn('Does not establish owner',result['limitation'])
        self.assertEqual(result['coverage']['dynamic_loader_relocation_writes'],'unknown')
        self.assertEqual(result['coverage']['owner'],'unknown')
    def test_bad_pins_and_identity(self):
        for args in [dict(digest='a'*64),dict(digest='bad'),dict(name='missing'),dict(address=0x1011)]:
            with self.subTest(args=args),self.assertRaises(ValueError):self.run_case(**args)
    def test_loaded_bytes_and_layout_must_match(self):
        self.nss['segments'][0]['flags']=4
        with self.assertRaisesRegex(ValueError,'layouts differ'):self.run_case()
        self.nss['segments'][0]['flags']=6;self.b.write_bytes(b'wrong')
        with self.assertRaisesRegex(ValueError,'changed while reading'):self.run_case()
    def test_symbols_and_overlap(self):
        for field,value in [('type',6),('binding',0),('visibility',2),('size',0),('section_index',0xfff1)]:
            with self.subTest(field=field):
                old=self.nss['symbols'][0][field];self.nss['symbols'][0][field]=value
                with self.assertRaises(ValueError):self.run_case()
                self.nss['symbols'][0][field]=old
        self.nss['symbols'].append(dict(self.nss['symbols'][0],name='alias'))
        with self.assertRaisesRegex(ValueError,'overlapping'):self.run_case()
        self.nss['symbols'][-1]['name']=''
        with self.assertRaisesRegex(ValueError,'overlapping'):self.run_case()
    def test_sections_and_loads(self):
        for container,field,value in [(self.original['sections'][0],'type',1),(self.nss['sections'][0],'flags',7),
                                      (self.original['segments'][0],'memory_size',8)]:
            old=container[field];container[field]=value
            with self.assertRaises(ValueError):self.run_case()
            container[field]=old
        self.nss['segments'].append(dict(self.nss['segments'][0],index=1))
        with self.assertRaisesRegex(ValueError,'overlapping load'):self.run_case()
    def test_relocation_write_vs_reference(self):
        self.original['relocations']=[dict(offset=0x100c,type=1027)]
        with self.assertRaisesRegex(ValueError,'writes to object'):self.run_case()
        self.original['relocations']=[dict(offset=0x2000,type=1027)]
        self.run_case()
        self.original['relocations']=[dict(offset=0x2000,type=999)]
        with self.assertRaisesRegex(ValueError,'unsupported relocation'):self.run_case()
    def test_packed_relocations(self):
        self.nss['dynamic_tags']=[dict(tag=36)]
        with self.assertRaisesRegex(ValueError,'packed'):self.run_case()


    def test_real_elf_parser_and_loads(self):
        import struct
        from test_data_elf import write_data_elf
        for path in (self.a,self.b):
            write_data_elf(path,symbols=[('target',2,0x2000,8,0x11)],section_overrides={5:{1:2}})
            blob=bytearray(path.read_bytes());phoff=len(blob)
            blob.extend(struct.pack('<IIQQQQQQ',1,6,64,0x1000,0,32,32,8))
            blob.extend(struct.pack('<IIQQQQQQ',1,6,0,0x2000,0,0,32,8))
            struct.pack_into('<Q',blob,32,phoff);struct.pack_into('<H',blob,56,2)
            path.write_bytes(blob)
        sha=hashlib.sha256(self.a.read_bytes()).hexdigest()
        got=evidence.extract(self.a,self.b,sha,sha,'target',0x2000)
        self.assertEqual(got['nss_symbol']['size'],8)
        self.assertEqual(len(got['matched_file_backed_loads']),1)

    def test_partial_overlap_and_tls(self):
        self.nss['sections'].append(dict(index=9,type=1,flags=3,address=0x1014,size=8))
        with self.assertRaisesRegex(ValueError,'unique writable NOBITS'):self.run_case()
        self.nss['sections'].pop()
        self.original['segments'].append(dict(index=2,type=7,address=0x1014,memory_size=8))
        with self.assertRaisesRegex(ValueError,'TLS'):self.run_case()

if __name__=='__main__':unittest.main()
