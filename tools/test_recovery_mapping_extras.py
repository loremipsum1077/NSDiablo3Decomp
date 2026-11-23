"""Extra bodies and authenticated COMDAT metadata, without builds."""
import copy,json,struct,tempfile,unittest
from pathlib import Path
from unittest.mock import patch
import recovery_mapping as m
import recovery
from validation import common as c
from test_recovery_mapping import fixture


def groups_fixture(path):
    # Two genuinely distinct COMDATs sharing their conventional metadata name.
    names=['','.text.old','.debug_str','.text.new','.shstrtab','.strtab','.symtab','.rela.text.old','.group','.group']
    syms=[('',0,0,0,0,0),('$x.0',0,0,1,0,0),('$d.1',0,0,2,0,0),
          ('old',0x22,0,1,0,4),('$x.2',0,0,3,0,0),('new',0x22,0,3,0,4)]
    def strings(items):
        data=b'\0';offsets={'':0}
        for item in items:
            if item not in offsets:offsets[item]=len(data);data+=item.encode()+b'\0'
        return data,offsets
    sh,ns=strings(names);st,ss=strings(x[0] for x in syms)
    # type,flags,link,info,entsize,payload
    secs=[(0,0,0,0,0,b''),(1,0x206,0,0,0,b'CODE'),(1,0,0,0,0,b'abc\0'),
          (1,0x206,0,0,0,b'CODE'),(3,0,0,0,0,sh),(3,0,0,0,0,st),
          (2,0,5,0,24,b''.join(struct.pack('<IBBHQQ',ss[n],i,o,ix,v,z) for n,i,o,ix,v,z in syms)),
          (4,0x200,6,1,24,struct.pack('<QQq',0,(3<<32)|283,0)),
          (17,0,6,3,4,struct.pack('<III',1,1,7)),(17,0,6,5,4,struct.pack('<II',1,3))]
    data=bytearray(64);data[:7]=b'\x7fELF\x02\x01\x01';struct.pack_into('<HH',data,16,1,183);headers=[]
    for i,(kind,flags,link,info,entsize,payload) in enumerate(secs):
        at=len(data);data+=payload;headers.append((ns[names[i]],kind,flags,0,at,len(payload),link,info,1,entsize))
    start=len(data)
    for h in headers:data+=struct.pack('<IIQQQQIIQQ',*h)
    struct.pack_into('<H',data,0x34,64);struct.pack_into('<Q',data,0x28,start);struct.pack_into('<HHH',data,0x3a,64,len(headers),4);path.write_bytes(data)


def section_field(path,index,offset,fmt,value):
    data=bytearray(path.read_bytes());start=struct.unpack_from('<Q',data,0x28)[0]
    struct.pack_into(fmt,data,start+64*index+offset,value);path.write_bytes(data)


def payload_word(path,section,word,value):
    data=bytearray(path.read_bytes());start=struct.unpack_from('<Q',data,0x28)[0]
    offset=struct.unpack_from('<Q',data,start+64*section+24)[0]
    struct.pack_into('<I',data,offset+word*4,value);path.write_bytes(data)


class ExtraTests(unittest.TestCase):
    def setUp(self):
        temp=tempfile.TemporaryDirectory();self.addCleanup(temp.cleanup)
        self.root=Path(temp.name);self.a=self.root/'a';self.b=self.root/'b';fixture(self.a)
    def test_positive_groups(self):
        groups_fixture(self.b);self.assertEqual(len(m.inspect(self.b)['groups']),2)
    def test_bad_group_metadata(self):
        for idx,off,fmt,value in [(8,4,'<I',1),(8,40,'<I',5),(8,44,'<I',99),
                                   (8,8,'<Q',2),(8,56,'<Q',8),(9,44,'<I',3),
                                   (1,8,'<Q',6),(7,44,'<I',3)]:
            with self.subTest(index=idx,off=off,value=value):
                groups_fixture(self.b);section_field(self.b,idx,off,fmt,value)
                with self.assertRaises(c.Halt):m.inspect(self.b)
    def test_bad_group_payload(self):
        for section,word,value in [(8,0,2),(8,1,8),(8,1,3),(8,2,1),(9,1,1),(9,1,99)]:
            with self.subTest(word=word,value=value):
                groups_fixture(self.b);payload_word(self.b,section,word,value)
                with self.assertRaises(c.Halt):m.inspect(self.b)
    def test_duplicate_function_identity(self):
        def mutate(s,n,h):s[6]=('old',0x12,0,3,0,4)
        fixture(self.b,True,mutate)
        with self.assertRaises(c.Halt):m.compare(self.a,self.b,{'old'})
    def test_no_marker_new_code(self):
        fixture(self.b,True,lambda s,n,h:s.pop(5))
        with self.assertRaises(c.Halt):m.compare(self.a,self.b,{'old','new'})
    def test_placeholder_becomes_code_without_marker(self):
        def before(s,n,h):
            h[3]=(1,0,b'');s.pop();s.pop()
        fixture(self.a,True,before)
        fixture(self.b,True,lambda s,n,h:s.pop(5))
        with self.assertRaises(c.Halt):m.compare(self.a,self.b,{'old','new'})
    def test_extra_payload_outside_file(self):
        fixture(self.b,True);section_field(self.b,3,24,'<Q',self.b.stat().st_size+100)
        with self.assertRaises(c.Halt):m.compare(self.a,self.b,{'old'})
    def test_truncated_extra_payload(self):
        fixture(self.b,True);section_field(self.b,3,24,'<Q',self.b.stat().st_size-2)
        with self.assertRaises(c.Halt):m.compare(self.a,self.b,{'old'})
    def test_bad_section_entry_size(self):
        fixture(self.b,True);data=bytearray(self.b.read_bytes());struct.pack_into('<H',data,0x3a,63);self.b.write_bytes(data)
        with self.assertRaises(c.Halt):m.compare(self.a,self.b,{'old'})
    def test_incomplete_instruction_extra(self):
        def mutate(s,n,h):h[3]=(1,6,b'X');s[6]=('new',0x12,0,3,0,1)
        fixture(self.b,True,mutate)
        with self.assertRaises(c.Halt):m.compare(self.a,self.b,{'old'})
    def test_partial_extra_body(self):
        def mutate(s,n,h):s[6]=('new',0x12,0,3,0,2)
        fixture(self.b,True,mutate)
        with self.assertRaises(c.Halt):m.compare(self.a,self.b,{'old'})
    def test_unnamed_extra_body(self):
        def mutate(s,n,h):s[6]=('',0x12,0,3,0,4)
        fixture(self.b,True,mutate)
        with self.assertRaises(c.Halt):m.compare(self.a,self.b,{'old'})
    def test_alias_extra_existing_section(self):
        def mutate(s,n,h):s.append(('alias',0x12,0,1,0,4))
        fixture(self.b,False,mutate)
        w=m.compare(self.a,self.b,{'old'})
        self.assertEqual(w['new_extra_sections'][0]['extra_names'],['alias'])
    def obligations(self):
        fixture(self.b,True);w=m.compare(self.a,self.b,{'old'})
        return {'normal:'+k:{'evidence':v,'evidence_sha256':c.digest(v)}
                for k,v in {'owner-mapping-symbols':w,**m.extra_obligations(w)}.items()}
    def test_omitted_witness_or_obligation(self):
        obs=self.obligations();m.verify_obligations(obs,'normal',self.a,self.b,{'old'})
        for key in obs:
            bad=copy.deepcopy(obs);bad.pop(key)
            with self.subTest(key=key),self.assertRaises(c.Halt):m.verify_obligations(bad,'normal',self.a,self.b,{'old'})
        bad=copy.deepcopy(obs);bad['normal:owner-mapping-symbols']['evidence']['new_extra_sections']=[]
        bad['normal:owner-mapping-symbols']['evidence_sha256']=c.digest(bad['normal:owner-mapping-symbols']['evidence'])
        with self.assertRaises(c.Halt):m.verify_obligations(bad,'normal',self.a,self.b,{'old'})
    def test_omitted_independent_review_extra(self):
        obs=self.obligations();out=self.root/'review';out.mkdir()
        for name in ['binding.json','evidence.json']:c.write(out/name,{})
        audit={'obligations':obs};c.write(out/'recovery-audit.json',audit)
        value=recovery.review_template(out,audit);value.update(reviewer='independent test reviewer',independent=True)
        for row in value['obligations'].values():row.update(decision='safe-uncertified',rationale='fixture review',evidence_reference='fixture')
        path=out/'independent.json';c.write(path,value)
        with patch.object(recovery.r,'verify'):
            recovery.review(out,path,c.sha(path))
            value['obligations'].pop('normal:unreviewed-extra:.text.new');c.write(path,value)
            with self.assertRaises(c.Halt):recovery.review(out,path,c.sha(path))

if __name__=='__main__':unittest.main()
