"""Bounded initialized-writable section and raw instruction proof tests."""
import struct,tempfile,unittest
from pathlib import Path
from unittest.mock import patch
import recovery_mapping as mapping
import recovery_named_data as data
from validation.common import Halt
from validation.elf import Elf
from test_recovery_data import fixture

class NamedDataTests(unittest.TestCase):
 def setUp(self):
  self.tmp=tempfile.TemporaryDirectory();self.addCleanup(self.tmp.cleanup);self.path=Path(self.tmp.name)/'owner.obj'
 def witness(self,mutate=None):
  def change(s,h):
   s[3]=('ctor',0x12,0,1,0,12);s[4]=('globalScalar',0x11,0,2,0,4)
   h[1]=(1,6,bytes.fromhex('08000090080140f9080140b9'));h[2]=(1,3,bytes.fromhex('32000000'));h[6]=(4,0,b'')
   if mutate:mutate(s,h)
  fixture(self.path,change);snap=mapping.inspect(self.path)
  return data.named_section(snap,'.data.rel.ro',self.path)
 def test_positive(self):
  r=self.witness();self.assertEqual(r['initializer_hex'],'32000000');self.assertEqual(r['symbol']['name'],'globalScalar');self.assertEqual(r['consumer_offset'],0)
 def test_wrong_symbols_and_alias(self):
  for mutate in (lambda s,h:s.append(('alias',0x11,0,2,0,4)),lambda s,h:s.__setitem__(4,('globalScalar',0x16,0,2,0,4)),lambda s,h:s.__setitem__(4,('globalScalar',0x11,0x80,2,0,4)),lambda s,h:s.__setitem__(4,('globalScalar',0x11,0,2,0,3))):
   with self.subTest(mutate=mutate),self.assertRaises(Halt):self.witness(mutate)
 def test_bad_storage(self):
  for typ,flags in ((8,3),(1,2),(1,0x403),(1,0x13)):
   with self.subTest(typ=typ,flags=flags),self.assertRaises(Halt):self.witness(lambda s,h:h.__setitem__(2,(typ,flags,b'\x32\0\0\0')))
  with self.assertRaises(Halt):self.witness(lambda s,h:h.__setitem__(6,(4,0,struct.pack('<QQq',0,(3<<32)|257,0))))
 def test_consumer_overlap_and_wrong_target(self):
  rel=lambda a,t,add=0:struct.pack('<QQq',a,(4<<32)|t,add)
  for records in (rel(0,311,1)+rel(4,312,0),rel(0,311)+rel(4,312)+rel(8,283),rel(0,311),rel(0,275)+rel(4,312)):
   with self.subTest(records=records),self.assertRaises(Halt):self.witness(lambda s,h:h.__setitem__(7,(4,0,records)))
 def test_prior_object_hidden_name_and_duplicate_symbol_table(self):
  self.witness()
  raw=bytearray(self.path.read_bytes());struct.pack_into('<I',raw,20,1);self.path.write_bytes(raw)
  with self.assertRaisesRegex(Halt,'existing object'):data._census_object(self.path,'globalScalar')
  data._census_object(self.path,'otherName')
  raw=bytearray(self.path.read_bytes());table=struct.unpack_from('<Q',raw,0x28)[0]
  struct.pack_into('<I',raw,table+6*64+4,2);self.path.write_bytes(raw)
  with self.assertRaisesRegex(ValueError,'ambiguous symbol tables'):data._census_object(self.path,'otherName')
 def test_register_and_opcode_mismatch(self):
  for raw in ('1f000090e10340f9e10340b9','08000090090140f9080140b9','08000090080140f9080140f9'):
   with self.subTest(raw=raw),self.assertRaises(Halt):self.witness(lambda s,h:h.__setitem__(1,(1,6,bytes.fromhex(raw))))
 def test_unwitnessed_proof_is_noop(self):
  self.assertEqual(data.prove({},None,None,None,None,None,None,None,original_sha256='',nss_sha256='',dwarfdump_path='',dwarfdump_sha256=''),[])
 def test_data_map_output_and_overlapping_aliases(self):
  out={'name':'.data','address':0x1000,'size':0x100};meta={}
  head='Address Size Align Out In Symbol\n00001000 00000100 4 .data\n'
  good='00001004 00000004 4 /work/build/2.6.2/CMakeFiles/d3_game_obj.dir/src/Test.cpp.obj:(.data)\n'
  kwargs=(meta,out,'CMakeFiles/d3_game_obj.dir/src/Test.cpp.obj','.data',4,4)
  self.assertEqual(data._map_contribution((head+good).encode(),*kwargs),0x1004)
  overlap='00001006 00000004 2 /work/build/2.6.2/CMakeFiles/other.obj:(.debug_spoof)\n'
  with self.assertRaises(Halt):data._map_contribution((head+good+overlap).encode(),*kwargs)
  with self.assertRaises((Halt,ValueError)):data._map_contribution((head+good.replace('src/Test.cpp','src/Wrong.cpp')).encode(),*kwargs)
  with self.assertRaises(Halt):data._map_contribution((head.replace('00000100','00000080')+good).encode(),*kwargs)
 def test_raw_got_target_and_partial_write(self):
  meta={'segments':[{'type':1,'flags':6,'address':0x2000,'offset':0,'file_size':16,'memory_size':16}],
        'sections':[{'name':'.got','address':0x2000,'size':16,'flags':3}],
        'relocations':[{'offset':0x2000,'type':1027,'symbol_index':0,'addend':0x1234}]}
  widths={1027:8,1031:16}
  self.assertEqual(data._got(meta,widths,0x2000,0x1234)['addend'],0x1234)
  with self.assertRaises(Halt):data._got(meta,widths,0x2000,0x1235)
  overlap={**meta,'relocations':meta['relocations']+[{'offset':0x1ff8,'type':1031}]}
  with self.assertRaises(Halt):data._got(overlap,widths,0x2000,0x1234)
 def test_raw_linked_symbol_other_and_duplicate(self):
  meta={'sections':[{'index':0,'flags':0}, {'index':1,'flags':3,'address':0x1000,'size':4},
                    {'index':2,'flags':0,'entry_size':24,'size':48,'offset':0}],
        'symbols':[{'name':'x','defined':True,'table_section':2,'index':1,'address':0x1000,'size':4,
                    'type':1,'binding':1,'visibility':0,'section_index':1}]}
  raw=bytearray(48);self.assertEqual(data._symbol(meta,raw,'x')['address'],0x1000)
  raw[29]=0x80
  with self.assertRaisesRegex(Halt,'st_other'):data._symbol(meta,raw,'x')
  raw[29]=0;dup={**meta,'symbols':meta['symbols']*2}
  with self.assertRaisesRegex(Halt,'duplicate'):data._symbol(dup,raw,'x')
  conflict={**meta,'symbols':meta['symbols']+[dict(meta['symbols'][0],defined=False)]}
  with self.assertRaisesRegex(Halt,'conflicting'):data._symbol(conflict,raw,'x')
  alias={'name':'alias','defined':True,'table_section':2,'index':0,'address':0x1001,'size':0,
         'type':0,'binding':1,'visibility':0,'section_index':1}
  with self.assertRaisesRegex(Halt,'alias'):data._symbol({**meta,'symbols':meta['symbols']+[alias]},raw,'x')
 def test_writable_rejects_relro_and_tls(self):
  section={'index':1,'type':1,'flags':3,'address':0x1000,'size':4}
  seg={'type':1,'flags':6,'address':0x1000,'memory_size':4,'file_size':4,'offset':0}
  meta={'sections':[section],'segments':[seg]};data._writable(meta,section,0x1000,4)
  for typ in (7,0x6474e552):
   with self.subTest(typ=typ),self.assertRaisesRegex(Halt,'TLS or GNU_RELRO'):
    data._writable({**meta,'segments':[seg,{'type':typ,'address':0xfff,'memory_size':2}]},section,0x1000,4)
 def test_original_absolute_alias_and_mapping_marker(self):
  selected=('x',1,1,0,1,0x1000,4);sections=[None,None]
  valid=('$d.1',0,0,0,1,0x1000,0)
  data._reject_nss_aliases([selected,valid],selected,sections,0x1000,4)
  for alias in [('absolute',0,1,0,0xfff1,0x1000,0),('$x.1',0,0,0,1,0x1000,0),
                ('$d.1',0,0,0,0xfff1,0x1000,0)]:
   with self.subTest(alias=alias),self.assertRaisesRegex(Halt,'alias'):
    data._reject_nss_aliases([selected,alias],selected,sections,0x1000,4)
 def test_actual_dwarf_owner_when_present(self):
  nss=Path(__file__).resolve().parents[1]/'build/romfs/DiabloIIINX64r.nss'
  if not nss.exists():self.skipTest('original NSS unavailable')
  import shutil
  tool=Path(shutil.which('llvm-dwarfdump')).resolve();pin=data.sha(tool)
  row=data._dwarf_owner(nss,'s_nTouchDelay',0x10449e8,'X/Client/Console/TouchGestureTracker.cpp',tool,pin)
  self.assertEqual(row['address'],0x10449e8)
  with self.assertRaises(Halt):data._dwarf_owner(nss,'s_nTouchDelay',0x10449e8,'Other.cpp',tool,pin)
 def test_actual_touch_owner_descriptor(self):
  p=Path(__file__).resolve().parents[1]/'build/work/X_Client_Console_TouchGestureTracker/build/touch-evidence/attempt-03.o'
  if not p.exists():self.skipTest('private Touch object unavailable')
  r=data.named_section(mapping.inspect(p),'.data',p)
  self.assertEqual((r['symbol']['name'],r['symbol']['size'],r['initializer_hex'],r['consumer_offset']),
                   ('s_nTouchDelay',4,'32000000',0xb0))
 def test_unsealed_extra_object_rejected(self):
  root=Path(self.tmp.name);(root/'objects').mkdir();(root/'objects'/'base.obj').write_bytes(b'old')
  done={'mode':'normal','files':{'objects/base.obj':'hash'}}
  with patch.object(data.receipts,'read_json',return_value=done),patch.object(data.receipts,'retained_mode',return_value='sealed'):
   self.assertEqual(data._receipt_objects(root)[2],{'base.obj':'hash'})
   (root/'objects'/'hidden.obj').write_bytes(b'new')
   with self.assertRaisesRegex(Halt,'unsealed'):data._receipt_objects(root)

class RetainedDiagnosticIntegration(unittest.TestCase):
 def test_full_proof_and_rehashed_initializer_mutation(self):
  import json,os,shutil
  from validate_data import validate_data
  root=Path(__file__).resolve().parents[1]
  baseline=root/'build/recovery-land/eb3587dc718e4658b63ebc2f446b19bb/post-receipts/candidate/normal/attempt-0001/receipt'
  candidate=root/'build/game-recovery-touch-v2/diagnostic/artifacts/normal/receipt'
  nss=root/'build/romfs/DiabloIIINX64r.nss';original=root/'data/2.6.2/main.elf'
  if not all(p.exists() for p in (baseline,candidate,nss,original)):
   self.skipTest('private retained Touch diagnostic unavailable')
  executable=Path(shutil.which('llvm-dwarfdump')).resolve()
  with tempfile.TemporaryDirectory(dir=root/'build',prefix='named-data-test-') as directory:
   temp=Path(directory);b=temp/'baseline';a=temp/'candidate'
   shutil.copytree(baseline,b,copy_function=os.link);shutil.copytree(candidate,a,copy_function=os.link)
   rel='objects/CMakeFiles/d3_game_obj.dir/src/X/Client/Console/TouchGestureTracker.cpp.obj';obj=a/rel
   snapshot=mapping.inspect(obj);desc=data.named_section(snapshot,'.data',obj)
   ledger=validate_data(b/'data/2.6.2/data_symbols.csv',original,a/'build/2.6.2/d3',nss)
   kwargs={'original_sha256':data.sha(original),'nss_sha256':data.sha(nss),
           'dwarfdump_path':executable,'dwarfdump_sha256':data.sha(executable)}
   def prove(descriptor):
    return data.prove({'new_named_data_sections':[descriptor]},ledger,original,nss,
                      'X/Client/Console/TouchGestureTracker.cpp',obj,b,a,**kwargs)
   witness=prove(desc);self.assertEqual(witness[0]['identity'][1],'s_nTouchDelay')
   # Replace hardlinks before mutation; never alter retained diagnostic inputs.
   payload=bytearray(obj.read_bytes());offset=Elf(obj).sections[snapshot['sections']['.data']['index']][4]
   payload[offset]^=1;obj.unlink();obj.write_bytes(payload)
   done=a/'done.json';record=json.loads(done.read_text());record['files'][rel]=data.sha(obj)
   done.unlink();done.write_text(json.dumps(record,sort_keys=True,separators=(',',':'))+'\n')
   forged=data.named_section(mapping.inspect(obj),'.data',obj)
   with self.assertRaisesRegex(Halt,'initializer differs'):prove(forged)

if __name__=='__main__':unittest.main()
