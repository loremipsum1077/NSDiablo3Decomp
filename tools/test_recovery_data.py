"""CPU ELF fixtures for fail-closed recovery data admission."""
import json,struct,tempfile,unittest
from pathlib import Path
import recovery_mapping as mapping
import recovery_data as data
from validation.common import Halt

def fixture(path, mutate=None):
    names=['','.text.ctor','.data.rel.ro','.shstrtab','.strtab','.symtab','.rela.data.rel.ro','.rela.text.ctor']
    symbols=[('',0,0,0,0,0),('$x.0',0,0,1,0,0),('$d.1',0,0,2,0,0),('ctor',0x12,0,1,0,8),('_ZTV4Test',0x11,0,2,0,24)]
    sections=[(0,0,b''),(1,6,bytes.fromhex('00000090000040f9')),(1,3,bytes(24)),(3,0,b''),(3,0,b''),(2,0,b''),
              (4,0,struct.pack('<QQq',16,(3<<32)|257,0)),(4,0,struct.pack('<QQq',0,(4<<32)|311,0)+struct.pack('<QQq',4,(4<<32)|312,0))]
    if mutate:mutate(symbols,sections)
    def strings(values):
        b=b'\0';o={'':0}
        for v in values:
            if v not in o:o[v]=len(b);b+=v.encode()+b'\0'
        return b,o
    sh,so=strings(names);st,sto=strings(s[0] for s in symbols)
    sections[3]=(3,0,sh);sections[4]=(3,0,st)
    sections[5]=(2,0,b''.join(struct.pack('<IBBHQQ',sto[n],i,o,ix,v,z) for n,i,o,ix,v,z in symbols))
    b=bytearray(64);b[:7]=b'\x7fELF\x02\x01\x01';struct.pack_into('<HH',b,16,1,183);heads=[]
    for i,(t,f,p) in enumerate(sections):
        off=len(b);b+=p;heads.append((so[names[i]],t,f,0,off,len(p),4 if i==5 else 5 if i in (6,7) else 0,2 if i==6 else 1 if i==7 else 0,8 if i==2 else 1,24 if i in (5,6,7) else 0))
    at=len(b)
    for h in heads:b+=struct.pack('<IIQQQQIIQQ',*h)
    struct.pack_into('<Q',b,0x28,at);struct.pack_into('<H',b,0x34,64);struct.pack_into('<HHH',b,0x3a,64,len(heads),3);path.write_bytes(b)

class DataTests(unittest.TestCase):
    def setUp(self):
        t=tempfile.TemporaryDirectory();self.addCleanup(t.cleanup);self.path=Path(t.name)/'owner.obj'
    def witness(self,mutate=None):
        fixture(self.path,mutate);return data.vtable_section(mapping.inspect(self.path),'.data.rel.ro',self.path)
    def test_positive_real_elf(self):
        w=self.witness();self.assertEqual(len(w['relocations']),1);self.assertEqual(len(w['consumers']),2)
        self.assertEqual(mapping.compare(None,self.path,{'ctor'})['new_data_sections'],[w])
    def test_malformed_nonobject_symbols(self):
        for info,offset in [(3,8),(4,0)]:
            with self.subTest(info=info),self.assertRaises(Halt):self.witness(lambda s,h:s.append(('',info,0,2,offset,0)))
    def test_wrong_storage(self):
        with self.assertRaisesRegex(Halt,'nonzero'):self.witness(lambda s,h:h.__setitem__(2,(1,3,b'\1'+bytes(23))))
    def test_wrong_target(self):
        with self.assertRaisesRegex(Halt,'pointer'):self.witness(lambda s,h:s.__setitem__(3,('ctor',0x02,0,1,0,8)))
    def test_wrong_relocation(self):
        for off,typ in [(24,257),(16,258)]:
            with self.subTest(off=off,typ=typ),self.assertRaises(Halt):self.witness(lambda s,h:h.__setitem__(6,(4,0,struct.pack('<QQq',off,(3<<32)|typ,0))))
    def test_aliases(self):
        for name in ['alias','$dollar','_ZTV4Other']:
            with self.subTest(name=name),self.assertRaises(Halt):self.witness(lambda s,h:s.append((name,0x11,0,2,0,24)))
    def test_extent_and_flags(self):
        with self.assertRaises(Halt):self.witness(lambda s,h:s.__setitem__(4,('_ZTV4Test',0x11,0,2,8,24)))
        with self.assertRaises(Halt):self.witness(lambda s,h:h.__setitem__(2,(1,2,bytes(24))))
    def test_unknown_consumer(self):
        with self.assertRaisesRegex(Halt,'consumer'):self.witness(lambda s,h:h.__setitem__(7,(4,0,struct.pack('<QQq',0,(4<<32)|257,0))))
    def test_bad_provenance(self):
        w=self.witness()
        with self.assertRaises(ValueError):data.prove({'new_data_sections':[w]},{},self.path,'Test.cpp',self.path)
        Path(str(self.path)+'.map').write_text('map')
        Path(str(self.path)+'.map.provenance').write_text(json.dumps(dict(format=1,elf_sha256='0'*64,map_sha256='0'*64)))
        with self.assertRaisesRegex(ValueError,'stale'):data.prove({'new_data_sections':[w]},{},self.path,'Test.cpp',self.path)
    def test_exact_transition_and_regressions(self):
        key=(123,'_ZTV4Test');old={key:dict(status='unverified',reasons=['missing_rebuilt_symbol'])};new={key:dict(status='verified_equal',reasons=[])};proof=[dict(identity=list(key))]
        self.assertEqual(len(data.compare_outcomes(old,new,proof)),1)
        for a,b,w in [(old,new,[]),(new,old,proof),(old,{key:dict(status='different',reasons=[])},proof),({key:dict(status='unverified',reasons=['unsupported_symbol_target'])},new,proof)]:
            with self.subTest(a=a,b=b),self.assertRaises(Halt):data.compare_outcomes(a,b,w)
    def linked_case(self, *, target='ctor', prefix='/work/build/2.6.2/', extent=24, proof_status='verified_equal', alias=False, linked_flags=3):
        from test_data_elf import write_data_elf
        from data_provenance import verify_link_provenance
        from data_relocations import RelocationImage, compare_objects
        import hashlib
        obj=self.path.parent/'objects/CMakeFiles/fixture.dir/src/Test.cpp.obj'
        obj.parent.mkdir(parents=True,exist_ok=True);fixture(obj)
        witness=data.vtable_section(mapping.inspect(obj),'.data.rel.ro',obj)
        link=self.path.parent/'linked'
        symbols=[('_ZTV4Test',1,0x1000,24,0x11),(target,2,0x2000,8,0x12)]
        if alias:symbols.append(('alias',1,0x1000,24,0x11))
        write_data_elf(link,symbols=symbols,relocations=[(0x1010,1027,0,0x2000)],
                       data=bytes(24)+bytes.fromhex('c0035fd6c0035fd6'),section_overrides={1:{5:24,2:linked_flags},2:{1:1,2:6,4:88,5:8}})
        mapfile=Path(str(link)+'.map');mapfile.write_text(f'0000000000001000 {extent:016x} 8 {prefix}CMakeFiles/fixture.dir/src/Test.cpp.obj:(.data.rel.ro)\n')
        Path(str(mapfile)+'.provenance').write_text(json.dumps(dict(format=1,elf_sha256=hashlib.sha256(link.read_bytes()).hexdigest(),map_sha256=hashlib.sha256(mapfile.read_bytes()).hexdigest())))
        image=RelocationImage(link);validation=compare_objects(image,image,0x1000,0x1000,24)
        validation['status']=proof_status
        row=dict(name='_ZTV4Test',address=0x7100001000,size=24,size_status='exact_symbol',comparison=dict(rebuilt_address=0x1000),validation=validation)
        ledger=dict(provenance=verify_link_provenance(link),rows=[row])
        return data.prove(dict(new_data_sections=[witness]),ledger,link,'Test.cpp',obj)
    def test_linked_positive(self):
        self.assertEqual(len(self.linked_case()),1)
    def test_linked_section_metadata(self):
        for flags in (7, 0x403):
            with self.subTest(flags=flags),self.assertRaisesRegex(Halt,'section metadata'):self.linked_case(linked_flags=flags)
    def test_linked_wrong_target(self):
        with self.assertRaisesRegex(Halt,'pointer target'):self.linked_case(target='wrong')
    def test_linked_wrong_rehashed_map_provenance(self):
        with self.assertRaisesRegex(Halt,'map provenance'):self.linked_case(prefix='/wrong/')
        with self.assertRaisesRegex(Halt,'map extent'):self.linked_case(extent=32)
    def test_linked_alias_or_unresolved_proof(self):
        with self.assertRaisesRegex(Halt,'aliases'):self.linked_case(alias=True)
        with self.assertRaisesRegex(Halt,'unresolved'):self.linked_case(proof_status='unverified')
    def test_consumer_opcode_and_position(self):
        with self.assertRaisesRegex(Halt,'opcode'):self.witness(lambda s,h:h.__setitem__(1,(1,6,bytes(8))))
        with self.assertRaisesRegex(Halt,'consumer'):self.witness(lambda s,h:h.__setitem__(7,(4,0,struct.pack('<QQq',1,(4<<32)|311,0)+struct.pack('<QQq',5,(4<<32)|312,0))))
    def test_malformed_retained_records(self):
        for value in [None,{},[[]],[[['bad','name'],{}]],[[[1,'x'],{}],[[1,'x'],{}]]]:
            with self.subTest(value=value),self.assertRaises(Halt):data.read_records(value)
    def test_large_realistic_retained_ledger(self):
        # 12k distinct original identities with complete per-slot proof records.
        # This reproduces the production >8MiB shape without game data.
        expression=dict(type=1027,kind='RELATIVE',addend=0x2000,symbol='',resolved=True,
                        target=dict(resolved=True,aliases=[['target',2,1,0]],offset=0,size=8),reason=None)
        validation=dict(status='verified_equal',reasons=[],raw_equal=False,non_relocated_equal=True,
                        storage_verified=True,masked_bytes=8,relocations=[dict(offset=16,left=[expression],right=[expression],status='equivalent')])
        records=[[[0x7100000000+i*32,'fixture_table_'+str(i)],validation] for i in range(12000)]
        self.path.write_text(json.dumps(records,indent=2))
        self.assertGreater(self.path.stat().st_size,8*1024*1024)
        result=data.read_retained_data(self.path)
        self.assertEqual(len(result),12000)
        self.assertEqual(result[(0x7100000000,'fixture_table_0')],validation)
        import receipts
        with self.assertRaisesRegex(Halt,'bounded reader limit'):receipts.read_json(self.path)
    def test_retained_reader_excessive_size(self):
        with self.path.open('wb') as stream:stream.truncate(data.MAX_RETAINED_DATA_BYTES+1)
        with self.assertRaisesRegex(Halt,'bounded reader limit'):data.read_retained_data(self.path)
    def test_retained_reader_malformed_duplicate_truncated(self):
        cases=[b'[[[1,"name"],{"status":"unverified","status":"different","reasons":[]}]]',
               b'[[[1,"name"],{"status":"unverified","reasons":[]}],[[1,"name"],{"status":"unverified","reasons":[]}]]',
               b'[[[1,"name"],{"status":"unverified","reasons":[]}',b'\xff',b'{}',
               b'[[[1,"name"],{"status":"unknown","reasons":[]}]]',
               b'[[[1,"name"],{"status":"unverified","reasons":{},"raw_equal":NaN}]]']
        for value in cases:
            with self.subTest(value=value),self.assertRaises(Halt):
                self.path.write_bytes(value);data.read_retained_data(self.path)
    def test_cst4_unknown_proof(self):
        from test_recovery_mapping import fixture as general
        def constant(s,n,h):
            n[3]='.rodata.cst4';h[3]=(1,0x12,struct.pack('<f',0.25));s[5]=('$d.4',0,0,3,0,0);s.pop()
        baseline=self.path.with_name('baseline');general(baseline)
        general(self.path,True,constant)
        with self.assertRaisesRegex(Halt,'original identity and consumer proof'):mapping.compare(baseline,self.path,{'old'})
if __name__=='__main__':unittest.main()
