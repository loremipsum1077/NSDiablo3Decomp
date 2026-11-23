import csv
from dataclasses import replace
import hashlib
import io
from pathlib import Path
import struct
import tempfile
import unittest

from validation import qualified_owner as q
from verify_original_inputs import fingerprint


def digest(data):return hashlib.sha256(data).hexdigest()


def fixture_elf(kind,records,*,bad_code=False):
    """Valid tiny AArch64 ELF containing the entire synthetic owner."""
    data=bytearray(0x1000 if kind==3 else 64)
    data[:7]=b'\x7fELF\x02\x01\x01';struct.pack_into('<HHI',data,16,kind,183,1);struct.pack_into('<H',data,52,64)
    sections=[(0,0,0,0,0,0,0,0,0,0)]
    def append(blob,typ,flags=0,addr=0,link=0,entsize=0,info=0):
        while len(data)%8:data.append(0)
        offset=len(data);data.extend(blob);sections.append((0,typ,flags,addr,offset,len(blob),link,info,8,entsize))
        return len(sections)-1
    if kind==3:
        base=0xa00000;end=0xa8c000;code=bytearray(end-base)
        if bad_code:code[0]=1
        text=append(code,1,6,base)
        struct.pack_into('<Q',data,0x20,64);struct.pack_into('<HH',data,0x36,56,1)
        struct.pack_into('<IIQQQQQQ',data,64,1,5,sections[text][4],base,0,len(code),len(code),4096)
    names=bytearray(b'\0');entries=[(0,0,0,0,0,0)]
    for entry in records:
        name=entry['name'];pos=len(names);names.extend(name.encode()+b'\0')
        if kind==1:
            section=entry.get('section')
            if section is None:section=append(bytes(16),1,entry.get('flags',6))
            value=entry.get('value',0);size=entry.get('size',16)
        else:section=text;value=entry['value'];size=entry['size']
        info=(entry.get('binding',0)<<4)|entry.get('type',2)
        entries.append((pos,info,0,section,value,size))
    if records:
        entries.sort(key=lambda row:row[1]>>4!=0)
        strings=append(names,3);append(b''.join(struct.pack('<IBBHQQ',*row) for row in entries),2,link=strings,entsize=24,
                                       info=sum((row[1]>>4)==0 for row in entries))
    while len(data)%8:data.append(0)
    shoff=len(data);data.extend(b''.join(struct.pack('<IIQQQQIIQQ',*row) for row in sections))
    struct.pack_into('<Q',data,0x28,shoff);struct.pack_into('<HHH',data,0x3a,64,len(sections),0)
    return bytes(data)


class QualifiedOwnerTests(unittest.TestCase):
    def setUp(self):
        self.temp=tempfile.TemporaryDirectory();self.addCleanup(self.temp.cleanup);self.root=Path(self.temp.name).resolve()
        names=sorted((q.GLOBAL_NAMES|q.LOCAL_NAMES)-{q.ALIAS_ORIGINAL})
        self.rows=[]
        for i,name in enumerate(names):self.rows.append((name,0x7100a00000+i*0x4000,9260 if i==0 else 4))
        self.rows.append((q.ALIAS_ORIGINAL,0x7100a8b990,88))
        self.nss=[{'name':q.ALIAS_CANDIDATE if n==q.ALIAS_ORIGINAL else n,'value':a&0xffffffff,'size':s} for n,a,s in self.rows]
        self.obj=[{'name':q.ALIAS_CANDIDATE if n==q.ALIAS_ORIGINAL else n,'binding':1 if n in q.GLOBAL_NAMES else 0} for n,_,_ in self.rows]
        self.write_inputs()

    def write_inputs(self,*,nss=None,obj=None,bad_code=False,rows=None,owners=None):
        rows=self.rows if rows is None else rows
        files={'original_sha256':fixture_elf(3,[]),'nss_sha256':fixture_elf(3,self.nss if nss is None else nss,bad_code=bad_code)}
        functions=io.StringIO();w=csv.writer(functions);w.writerow(['Address','Name','Size'])
        for n,a,s in rows:w.writerow([hex(a),n,s])
        owner_csv=io.StringIO();w=csv.writer(owner_csv);w.writerow(['Address','File'])
        for n,a,s in rows:w.writerow([hex(a),q.OWNER if owners is None else owners.get(n,q.OWNER)])
        files['functions_sha256']=functions.getvalue().encode();files['function_files_sha256']=owner_csv.getvalue().encode()
        for key,data in files.items():
            path=self.root/q.PINS[key];path.parent.mkdir(parents=True,exist_ok=True);path.write_bytes(data)
        self.pins={key:digest(data) for key,data in files.items()}
        data=fixture_elf(1,self.obj if obj is None else obj);self.object=self.root/'build/candidate.o';self.object.parent.mkdir(parents=True,exist_ok=True);self.object.write_bytes(data)
        self.object_sha=digest(data)

    def build(self):return q.build_context(self.root,q.OWNER,self.pins,self.object,self.object_sha)

    def test_valid_context_is_immutable_and_reauthenticated(self):
        context=self.build();self.assertEqual(len(context.members),20)
        alias=next(x for x in context.members if x.original_name==q.ALIAS_ORIGINAL)
        self.assertEqual((alias.original_address,alias.original_size,alias.candidate_name,alias.candidate_binding,alias.nss_binding),
                         (0x7100a8b990,88,'__isspace',0,0))
        self.assertEqual(q.revalidate_context(context),context)
        with self.assertRaisesRegex(ValueError,'context differs'):q.revalidate_context(replace(context,members=context.members[:-1]))
        self.object.write_bytes(self.object.read_bytes()+b'changed')
        with self.assertRaisesRegex(ValueError,'SHA-256 differs'):q.revalidate_context(context)

    def test_qualified_mapping_requires_exact_owner_original_address_and_complete_membership(self):
        with self.assertRaisesRegex(ValueError,'unsupported owner'):q.build_context(self.root,'wrong/llex.c',self.pins,self.object,self.object_sha)
        rows=[(n,a+4 if n==q.ALIAS_ORIGINAL else a,s) for n,a,s in self.rows];self.write_inputs(rows=rows)
        with self.assertRaisesRegex(ValueError,'qualified alias original address'):self.build()
        self.write_inputs(owners={q.ALIAS_ORIGINAL:'elsewhere/llex.c'})
        with self.assertRaisesRegex(ValueError,'twenty'):self.build()

    def test_nss_requires_unique_local_symbol_correct_identity_and_original_bytes(self):
        alias=next(i for i,x in enumerate(self.nss) if x['name']=='__isspace')
        for variant in ('binding','name','size','duplicate','overlap','ifunc','code'):
            with self.subTest(variant=variant):
                nss=[dict(x) for x in self.nss]
                if variant=='binding':nss[alias]['binding']=1
                if variant=='name':nss[alias]['name']='different'
                if variant=='size':nss[alias]['size']+=4
                if variant=='duplicate':nss.append(dict(nss[alias]))
                if variant=='overlap':nss.append(dict(nss[alias],name='overlap',value=nss[alias]['value']+4,size=4))
                if variant=='ifunc':nss.append(dict(nss[alias],type=10))
                self.write_inputs(nss=nss,bad_code=variant=='code')
                with self.assertRaises(ValueError):self.build()

    def test_candidate_rejects_nonlocal_alias_missing_duplicates_extras_and_overlap(self):
        for variant in ('binding','missing','duplicate','extra','overlap','collision','ifunc','nonexec'):
            with self.subTest(variant=variant):
                obj=[dict(x) for x in self.obj];alias=next(i for i,x in enumerate(obj) if x['name']=='__isspace')
                if variant=='binding':obj[alias]['binding']=1
                if variant=='missing':obj.pop(alias)
                if variant=='duplicate':obj.append(dict(obj[alias]))
                if variant=='extra':obj.append({'name':'unrelated','binding':0})
                if variant=='overlap':obj[1]['section']=1
                if variant=='collision':obj.append({'name':'__isspace','type':1,'binding':0})
                if variant=='ifunc':obj.append({'name':'resolver','type':10,'binding':0})
                if variant=='nonexec':obj[alias]['flags']=2
                self.write_inputs(obj=obj)
                with self.assertRaises(ValueError):self.build()

    def test_wrong_pin_and_aliased_input_rejected(self):
        bad=dict(self.pins,original_sha256='0'*64)
        with self.assertRaisesRegex(ValueError,'SHA-256 differs'):q.build_context(self.root,q.OWNER,bad,self.object,self.object_sha)
        original=self.root/q.PINS['original_sha256'];real=original.with_name('real.elf');original.rename(real);original.symlink_to(real)
        with self.assertRaisesRegex(ValueError,'aliased input'):self.build()

    def test_malformed_symbol_local_boundary_rejected(self):
        raw=bytearray(self.object.read_bytes());shoff=struct.unpack_from('<Q',raw,0x28)[0]
        count=struct.unpack_from('<H',raw,0x3c)[0]
        index=next(i for i in range(count) if struct.unpack_from('<I',raw,shoff+i*64+4)[0]==2)
        struct.pack_into('<I',raw,shoff+index*64+44,0)
        self.object.write_bytes(raw);self.object_sha=digest(raw)
        with self.assertRaisesRegex(ValueError,'local-symbol boundary'):self.build()


class RetainedEvidenceTests(unittest.TestCase):
    def test_actual_lexer_on_object(self):
        root=Path(__file__).resolve().parents[1];obj=root/'build/compiler-calibration/lua-full-lexer-on-v1/out/llex.o'
        if not obj.is_file() or not (root/q.PINS['nss_sha256']).is_file():self.skipTest('retained original inputs unavailable')
        pins={key:fingerprint(root/path)['sha256'] for key,path in q.PINS.items()}
        context=q.build_context(root,q.OWNER,pins,obj,fingerprint(obj)['sha256'])
        self.assertEqual(len(context.members),20)
        self.assertEqual(next(x for x in context.members if x.original_name==q.ALIAS_ORIGINAL).candidate_name,'__isspace')


if __name__=='__main__':unittest.main()
