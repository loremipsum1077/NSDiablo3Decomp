"""Disposable ELF fixtures, no compiler."""
import struct,tempfile,unittest
from pathlib import Path
import recovery_mapping as q
from validation.common import Halt

def fixture(path,new=False,mutate=None,reloc=False):
    names=['','.text.old','.debug_str','.text.new','.shstrtab','.strtab','.symtab','.rela.text.old']
    secs=[(0,0,b''),(1,6,b'CODE'),(1,0,b'abc\0'),(1,6,b'CODE') if new else (1,0,b''),(3,0,b''),(3,0,b''),(2,0,b''),(4,0,b'')]
    syms=[('',0,0,0,0,0),('$x.0',0,0,1,0,0),('$d.3' if new else '$d.2',0,0,2,0,0),('old',0x12,0,1,0,4),('$ordinary',1,0,2,0,1)]
    if new:syms += [('$x.1',0,0,3,0,0),('new',0x12,0,3,0,4)]
    if mutate:mutate(syms,names,secs)
    # Empty baseline placeholder must not claim the new section's identity.
    if not new:names[3]='.unused'
    def strings(values):
        b=b'\0';o={'':0}
        for v in values:
            if v not in o:o[v]=len(b);b+=v.encode()+b'\0'
        return b,o
    sh,so=strings(names);st,sto=strings([x[0] for x in syms]);secs[4]=(3,0,sh);secs[5]=(3,0,st)
    secs[6]=(2,0,b''.join(struct.pack('<IBBHQQ',sto[n],i,o,ix,v,z) for n,i,o,ix,v,z in syms))
    if reloc:secs[7]=(4,0,struct.pack('<QQq',0,(1<<32)|283,0))
    b=bytearray(64);b[:7]=b'\x7fELF\x02\x01\x01';struct.pack_into('<HH',b,16,1,183);heads=[]
    for i,(t,f,data) in enumerate(secs):
        off=len(b);b+=data;heads.append((so[names[i]],t,f,0,off,len(data),5 if i==6 else 6 if i==7 else 0,1 if i==7 else 0,1,24 if i in (6,7) else 0))
    at=len(b)
    for h in heads:b+=struct.pack('<IIQQQQIIQQ',*h)
    struct.pack_into('<H',b,0x34,64);struct.pack_into('<Q',b,0x28,at);struct.pack_into('<HHH',b,0x3a,64,len(heads),4);path.write_bytes(b)

def named_fixture(path):
    from test_recovery_data import fixture as data_fixture
    def change(symbols, sections):
        symbols[3] = ('ctor', 0x12, 0, 1, 0, 12)
        symbols[4] = ('globalScalar', 0x11, 0, 2, 0, 4)
        sections[1] = (1, 6, bytes.fromhex('08000090080140f9080140b9'))
        sections[2] = (1, 3, bytes.fromhex('32000000'))
        sections[6] = (4, 0, b'')
    data_fixture(path, change)
    # Preserve all ELF string offsets while changing the fixture's storage name.
    path.write_bytes(path.read_bytes().replace(b'.data.rel.ro\0', b'.data' + b'\0' * 8))


class NamedMappingTests(unittest.TestCase):
    def setUp(self):
        tmp = tempfile.TemporaryDirectory(); self.addCleanup(tmp.cleanup)
        self.path = Path(tmp.name)/'owner.o'; named_fixture(self.path)

    def test_default_remains_closed_and_opt_in_only_emits_pending_structure(self):
        with self.assertRaises(Halt): q.compare(None, self.path, {'ctor'})
        witness = q.compare(None, self.path, {'ctor'}, named_data=True)
        self.assertEqual(witness['new_data_sections'], [])
        self.assertEqual(witness['new_named_data_sections'][0]['symbol']['name'], 'globalScalar')
        self.assertEqual(witness['new_named_data_sections'][0]['classification'], 'pending-named-initialized-data')
        self.assertNotIn('new-named-data-proof', q.extra_obligations(witness))

    def test_structure_is_regenerated_and_cannot_be_omitted_or_invented(self):
        from validation.common import digest
        witness = q.compare(None, self.path, {'ctor'}, named_data=True)
        values = {'owner-mapping-symbols': witness, **q.extra_obligations(witness)}
        obligations = {'normal:'+key: {'evidence': value, 'evidence_sha256': digest(value)}
                       for key, value in values.items()}
        q.verify_obligations(obligations, 'normal', None, self.path, {'ctor'}, named_data=True)
        del obligations['normal:new-named-data-structure']
        with self.assertRaises(Halt):
            q.verify_obligations(obligations, 'normal', None, self.path, {'ctor'}, named_data=True)


class MappingTests(unittest.TestCase):
    def setUp(self):
        t=tempfile.TemporaryDirectory();self.addCleanup(t.cleanup);self.a=Path(t.name)/'a';self.b=Path(t.name)/'b';fixture(self.a)
    def compare(self,mutate=None,reloc=False):
        fixture(self.b,True,mutate,reloc);return q.compare(self.a,self.b,{'old','new'})
    def test_positive(self):
        w=self.compare();self.assertEqual(w['new_selected_sections'],['.text.new']);self.assertIn({'name':'$ordinary','type':1},w['candidate']['real_emissions'])
    def test_real_dollar_data_loss(self):
        with self.assertRaises(Halt):self.compare(lambda s,n,h:s.pop(4))
    def test_metadata_kind_position_spelling(self):
        for idx,value in [(1,0x10),(1,1),(2,2),(3,0),(3,0xfff1),(5,1),(0,'$d.0'),(0,'$x.bad'),(4,1),(3,2)]:
            def mutate(s,n,h):
                x=list(s[1]);x[idx]=value;s[1]=tuple(x)
            with self.subTest(value=value),self.assertRaises(Halt):self.compare(mutate)
    def test_relocation_to_marker(self):
        with self.assertRaises(Halt):self.compare(reloc=True)
    def test_duplicate_marker(self):
        with self.assertRaises(Halt):self.compare(lambda s,n,h:s.append(('$x.9',0,0,1,0,0)))
    def test_missing_marker(self):
        with self.assertRaises(Halt):self.compare(lambda s,n,h:s.pop(1))
    def test_duplicate_section(self):
        with self.assertRaises(Halt):self.compare(lambda s,n,h:n.__setitem__(3,'.text.old'))
    def test_unselected_new_body_requires_obligation(self):
        fixture(self.b,True)
        w=q.compare(self.a,self.b,{'old'})
        self.assertEqual(w['new_extra_sections'][0]['extra_names'],['new'])
        self.assertIn('unreviewed-extra:.text.new',q.extra_obligations(w))
    def test_data_marker_offset(self):
        def mutate(s,n,h):s[2]=('$d.3',0,0,2,1,0)
        with self.assertRaises(Halt):self.compare(mutate)
    def test_new_debug_loc(self):
        def mutate(s,n,h):
            n[3]='.debug_loc';h[3]=(1,0,b'abc\0');s[5]=('$d.4',0,0,3,0,0);s.pop(6)
        self.assertEqual(self.compare(mutate)['new_nonallocated_debug_sections'],['.debug_loc'])
        def allocated(s,n,h):
            mutate(s,n,h);h[3]=(1,2,b'abc\0')
        with self.assertRaises(Halt):self.compare(allocated)
    def test_empty_debug_ranges_added_marker(self):
        # Build baseline through new=True with an empty section and no symbols in it.
        def empty(s,n,h):
            n[3]='.debug_ranges';h[3]=(1,0,b'');s.pop();s.pop()
        fixture(self.a,True,empty)
        def after(s,n,h):
            n[3]='.debug_ranges';h[3]=(1,0,b'abcd');s[5]=('$d.4',0,0,3,0,0);s.pop()
        self.assertEqual(self.compare(after)['new_nonallocated_debug_sections'],['.debug_ranges'])
        def nonempty(s,n,h):empty(s,n,h);h[3]=(1,0,b'abcd')
        fixture(self.a,True,nonempty)
        with self.assertRaises(Halt):self.compare(after)
    def test_changed_existing_section_flags(self):
        def mutate(s,n,h):h[1]=(1,7,b'CODE')
        with self.assertRaises(Halt):self.compare(mutate)
    def test_non_progbits_new_code(self):
        def mutate(s,n,h):h[3]=(8,6,b'CODE')
        with self.assertRaises(Halt):self.compare(mutate)
    def test_architecture(self):
        fixture(self.b,True);b=bytearray(self.b.read_bytes());struct.pack_into('<H',b,18,62);self.b.write_bytes(b)
        with self.assertRaises(Halt):q.compare(self.a,self.b,{'new'})
    # Previously absent owner object: the standard per-object metadata markers.
    def metadata(self,name,flags,offset=0,typ=1):
        def mutate(s,n,h):
            h[2]=(1,0x30,b'abc\0')  # real .debug_str is SHF_MERGE|SHF_STRINGS
            n[3]=name;h[3]=(typ,flags,b'abc\0');s[5]=('$d.4',0,0,3,offset,0);s.pop(6)
        return mutate
    def new_object(self,mutate):
        fixture(self.b,True,mutate);return q.compare(None,self.b,{'old'})
    def test_new_object_metadata(self):
        for name,flags in [('.debug_abbrev',0),('.debug_info',0),('.debug_line',0),('.debug_macinfo',0),
                           ('.debug_pubnames',0),('.debug_pubtypes',0),('.comment',0x30),('.eh_frame',2)]:
            with self.subTest(name=name):
                w=self.new_object(self.metadata(name,flags))
                self.assertEqual(w['new_object_metadata_sections'],sorted(['.debug_str',name]))
                self.assertEqual(w['new_selected_sections'],['.text.old'])
    def test_new_object_metadata_requires_absent_baseline(self):
        # Same .debug_str metadata in both, so only the absent-baseline guard can reject.
        fixture(self.a,False,lambda s,n,h:h.__setitem__(2,(1,0x30,b'abc\0')))
        fixture(self.b,True,self.metadata('.debug_pubnames',0))
        with self.assertRaisesRegex(Halt,'Unsupported new mapping section: .debug_pubnames'):
            q.compare(self.a,self.b,{'old'})

    def test_scalar_report_is_not_a_raw_context(self):
        fixture(self.b, True, self.metadata('.rodata.cst4', 18))
        for context in ({'accepted':True}, {'section':'.rodata.cst4','exact_complete':False}, {}):
            with self.subTest(context=context), self.assertRaisesRegex(Halt, 'Incomplete scalar raw-input context'):
                q.compare(None, self.b, {'old'}, scalar_context=context)
    def test_new_object_debug_str_flags(self):
        def plain(s,n,h):self.metadata('.debug_info',0)(s,n,h);h[2]=(1,0,b'abc\0')
        with self.assertRaisesRegex(Halt,'Unsupported new mapping section: .debug_str'):self.new_object(plain)
    def test_new_object_metadata_second_marker(self):
        def twice(s,n,h):self.metadata('.debug_info',0)(s,n,h);s.append(('$d.9',0,0,3,2,0))
        with self.assertRaisesRegex(Halt,'Unsupported new mapping section: .debug_info'):self.new_object(twice)
    def test_new_object_metadata_exact_metadata(self):
        unsupported='Unsupported new mapping section: '
        for name,flags,offset,typ,message in [('.debug_pubnames',2,0,1,unsupported),('.comment',0,0,1,unsupported),
                                              ('.eh_frame',3,0,1,unsupported),('.eh_frame',6,0,1,'New executable extent lacks x-at-zero marker'),
                                              ('.debug_info',0,1,1,unsupported),('.debug_line',0,0,8,unsupported),('.debug_frame',0,0,1,unsupported)]:
            with self.subTest(name=name,flags=flags,offset=offset,typ=typ),self.assertRaisesRegex(Halt,message):
                self.new_object(self.metadata(name,flags,offset,typ))
    def test_new_object_runtime_data_still_rejected(self):
        for name,flags,typ,message in [('.rodata',2,1,'Unsupported new mapping section'),('.rodata.str1.1',0x32,1,'Unsupported new mapping section'),
                                       ('.rodata.cst4',0x12,1,'lacks authenticated original identity'),('.data',3,1,'lacks authenticated original identity'),
                                       ('.bss',3,8,'Unsupported new mapping section'),('.init_array',3,14,'Unsupported new mapping section')]:
            with self.subTest(name=name),self.assertRaisesRegex(Halt,message+'.*'+name.replace('.','\\.')):
                self.new_object(self.metadata(name,flags,0,typ))
    def test_existing_object_witness_unchanged(self):
        self.assertNotIn('new_object_metadata_sections',self.compare())
    def test_markerless_new_runtime_data_rejected(self):
        for name, typ, flags in [('.bss',8,3),('.data',1,3),('.rodata',1,2)]:
            def data(s,n,h):
                n[3]=name;h[3]=(typ,flags,bytes(4));s.pop();s.pop()
                s.append(('new_data',0x11,0,3,0,4))
            with self.subTest(name=name):
                fixture(self.b,True,data)
                with self.assertRaisesRegex(Halt,'allocated data section'):
                    q.compare(self.a,self.b,{'old'})
                def new_object(s,n,h):
                    data(s,n,h);h[2]=(1,0x30,b'abc\0')
                fixture(self.b,True,new_object)
                with self.assertRaisesRegex(Halt,'allocated data section'):
                    q.compare(None,self.b,{'old'})
    def test_markerless_existing_data_growth_and_new_symbol(self):
        def data(size, extra=False):
            def mutate(s,n,h):
                n[3]='.bss';h[3]=(8,3,bytes(size));s.pop();s.pop()
                s.append(('existing_data',0x11,0,3,0,4))
                if extra:s.append(('new_data',0x11,0,3,4,4))
            return mutate
        fixture(self.a,True,data(8));fixture(self.b,True,data(8))
        q.compare(self.a,self.b,{'old'})  # Unchanged storage and identities stay valid.
        fixture(self.b,True,data(12))
        with self.assertRaisesRegex(Halt,'changed allocated data section'):
            q.compare(self.a,self.b,{'old'})
        fixture(self.b,True,data(8,True))
        with self.assertRaisesRegex(Halt,'allocated data symbol.*new_data'):
            q.compare(self.a,self.b,{'old'})
        def unnamed(s,n,h):
            data(8)(s,n,h);s.append(('',0,0,3,4,4))
        fixture(self.b,True,unnamed)
        with self.assertRaisesRegex(Halt,'allocated data symbol'):
            q.compare(self.a,self.b,{'old'})
    def test_markerless_allocated_data_metadata_change_rejected(self):
        def data(typ,flags):
            def mutate(s,n,h):
                n[3]='.data';h[3]=(typ,flags,bytes(4));s.pop();s.pop()
                s.append(('item',0x11,0,3,0,4))
            return mutate
        fixture(self.a,True,data(1,3));fixture(self.b,True,data(8,3))
        with self.assertRaisesRegex(Halt,'allocated data section'):
            q.compare(self.a,self.b,{'old'})
    def test_new_special_index_data_rejected(self):
        for index,info in ((0xfff1,0x11),(0xfff2,0x11),(0xfff2,0x15)):
            def add(s,n,h):s.append(('new_special',info,0,index,4,4))
            with self.subTest(index=index,info=info):
                fixture(self.b,True,add)
                with self.assertRaisesRegex(Halt,'data symbol.*new_special'):
                    q.compare(self.a,self.b,{'old','new'})
                fixture(self.a,False,add)
                q.compare(self.a,self.b,{'old','new'})
                fixture(self.a)
    def test_lost_data_identity_rejected(self):
        def common(s,n,h):s.append(('common_data',0x15,0,0xfff2,4,4))
        fixture(self.a,False,common);fixture(self.b,True)
        with self.assertRaisesRegex(Halt,'Lost allocated data symbol.*common_data'):
            q.compare(self.a,self.b,{'old','new'})
        def section(add_symbol):
            def mutate(s,n,h):
                n[3]='.data';h[3]=(1,3,bytes(4));s.pop();s.pop()
                if add_symbol:s.append(('owned_data',0x11,0,3,0,4))
            return mutate
        fixture(self.a,True,section(True));fixture(self.b,True,section(False))
        with self.assertRaisesRegex(Halt,'Lost allocated data symbol.*owned_data'):
            q.compare(self.a,self.b,{'old'})
    def test_anonymous_data_and_storage_mutation(self):
        def data(payload):
            def mutate(s,n,h):
                n[3]='.data';h[3]=(1,3,payload);s.pop();s.pop()
            return mutate
        fixture(self.a,True,data(b'ABCD'))
        for payload in (b'ABCE',b'ABC',b'ABCDE'):
            fixture(self.b,True,data(payload))
            with self.subTest(payload=payload),self.assertRaisesRegex(Halt,'changed allocated data section'):
                q.compare(self.a,self.b,{'old'})
        fixture(self.b,True,data(b'ABCD'))
        blob=bytearray(self.b.read_bytes());shoff=struct.unpack_from('<Q',blob,0x28)[0]
        struct.pack_into('<Q',blob,shoff+3*64+48,8);self.b.write_bytes(blob)
        with self.assertRaisesRegex(Halt,'changed allocated data section'):
            q.compare(self.a,self.b,{'old'})
        def bss(s,n,h):
            n[3]='.bss';h[3]=(8,3,bytes(4));h[2]=(1,0x30,b'abc\0');s.pop();s.pop()
        fixture(self.b,True,bss)
        with self.assertRaisesRegex(Halt,'allocated data section'):
            q.compare(None,self.b,{'old'})
    def test_relocated_data_expression_must_remain_unchanged(self):
        def data(target, swapped=False):
            def mutate(s,n,h):
                n[3]='.data';h[3]=(1,3,bytes(8));s.pop();s.pop()
                names = ('bar','foo') if swapped else ('foo','bar')
                s.extend((name,0x10,0,0,0,0) for name in names)
                h[7]=(4,0,struct.pack('<QQq',0,(target<<32)|257,0))
            return mutate
        def write(path,target,swapped=False):
            fixture(path,True,data(target,swapped));blob=bytearray(path.read_bytes())
            shoff=struct.unpack_from('<Q',blob,0x28)[0]
            struct.pack_into('<I',blob,shoff+7*64+44,3);path.write_bytes(blob)
        write(self.a,5);write(self.b,5);q.compare(self.a,self.b,{'old'})
        write(self.b,6,True);q.compare(self.a,self.b,{'old'})  # Same target, different symbol index.
        write(self.b,6)
        with self.assertRaisesRegex(Halt,'relocation expression changed'):
            q.compare(self.a,self.b,{'old'})
if __name__=='__main__':unittest.main()
