import struct
import unittest
from unittest.mock import patch
from validation import recovery_function_got as g

class FunctionGotTest(unittest.TestCase):
 def setUp(self):
  self.meta={'segments':[{'type':1,'address':0x2000,'memory_size':16,'file_size':16,'flags':6,'offset':0}], 'sections':[{'name':'.got','type':1,'flags':3,'address':0x2000,'size':16}], 'relocations':[{'offset':0x2000,'type':1027,'symbol_index':0,'addend':0x1234}], 'symbols':[]}
 def test_relative_and_overlap(self):
  self.assertEqual(g.got(self.meta,0x2000,1027,'f',0x1234)['addend'],0x1234)
  for change in ('target','preceding','wrongkind','unaligned'):
   m={**self.meta,'relocations':[dict(self.meta['relocations'][0])]}
   if change=='target':m['relocations'][0]['addend']=0x1235
   if change=='preceding':m['relocations'].append({'offset':0x1ffc,'type':1027,'symbol_index':0,'addend':0})
   if change=='wrongkind':m['relocations'][0]['type']=1025
   with self.assertRaises(ValueError):g.got(m,0x2001 if change=='unaligned' else 0x2000,1027,'f',0x1234)
 def test_undefined_symbol_is_exact(self):
  m={**self.meta,'relocations':[{'offset':0x2000,'type':1025,'symbol_index':1,'symbol_table':2,'symbol_name':'f','addend':0}], 'symbols':[{'table_section':2,'index':1,'defined':False,'type':0,'binding':1,'visibility':0,'address':0,'size':0}]}
  g.got(m,0x2000,1025,'f')
  for key,value in [('binding',2),('defined',True),('visibility',2),('type',1)]:
   x={**m,'symbols':[dict(m['symbols'][0],**{key:value})]}
   with self.assertRaises(ValueError):g.got(x,0x2000,1025,'f')
 def test_instruction_pair(self):
  data=struct.pack('<II',0x90000008,0xf9400508)
  with patch.object(g,'_mapped',side_effect=lambda meta,addr,size,req,forbid:addr-0x1000):
   self.assertEqual(g.slot({},data,0x1000,0,4),0x1008)
   for x,y in [(0x9000001f,0xf94007ff),(0x90000008,0xf9400509),(0x90000009,0xf9400508)]:
    # Different destination register is legal for LDR; base register is what matters.
    if x==0x90000008 and y==0xf9400509:continue
    with self.assertRaises(ValueError):g.slot({},struct.pack('<II',x,y),0x1000,0,4)
 def test_exact_diagnostic_and_no_waiver(self):
  p=g.FunctionGOTEvidence({},'x','y',[],g.OWNER)
  before={'status':'mismatch','reason':g.BEFORE,'address':'0x000000710019cba0','name':g.CALLER};after={**before,'reason':g.AFTER}
  with patch.object(p,'_prove',return_value={'callbacks':[1,2,3]}):
   self.assertTrue(p.equivalent(before,after));self.assertEqual(len(p.witnesses),1)
   self.assertFalse(p.equivalent(before,{**after,'reason':g.AFTER+' '}))
   self.assertFalse(p.equivalent(before,{**after,'name':'other'}))
  with patch.object(p,'_prove',side_effect=ValueError('ambiguous')):self.assertFalse(p.equivalent(before,after))


class RetainedRadiusIntegration(unittest.TestCase):
 def test_real_retained_two_mode_diagnostic(self):
  import json,tempfile
  from pathlib import Path
  root=Path(__file__).resolve().parents[1]; data=root/'build/game-recovery-radius-v2/relink'; obj=root/'build/work/X_Client_Behavior_RadiusTrigger/build/radius-evidence/attempt-02.o';nss=root/'build/romfs/DiabloIIINX64r.nss'
  if not (data/'run.json').exists() or not obj.exists() or not nss.exists():self.skipTest('retained private pilot inputs unavailable')
  targets=[{'Name':n,'Address':hex(a),'Size':str(z)} for n,a,z,_,_ in g.ITEMS]
  with tempfile.TemporaryDirectory() as tmp:
   for mode in ('normal','lua'):
    roots={}
    for label in ('baseline','candidate'):
     d=Path(tmp)/mode/label;(d/'build/2.6.2').mkdir(parents=True);roots[label]=d
     for ext in ('','.map'):(d/('build/2.6.2/d3'+ext)).symlink_to(data/mode/label/('d3'+ext))
     elf=d/'build/2.6.2/d3';Path(str(elf)+'.map.provenance').write_text(json.dumps({'format':1,'elf_sha256':g.digest(elf),'map_sha256':g.digest(Path(str(elf)+'.map'))}))
     src=root/'build/game-recovery-experience-v2/receipts/candidate'/mode/'attempt-0001/receipt/objects';rel='CMakeFiles/d3_game_obj.dir/src/X/Client/Behavior/Behavior.cpp.obj';dst=d/'objects'/rel;dst.parent.mkdir(parents=True);dst.symlink_to(src/rel)
     if label=='candidate':(dst.parent/'RadiusTrigger.cpp.obj').symlink_to(obj)
    proof=g.FunctionGOTEvidence(roots,root/'data/2.6.2/main.elf',nss,targets,g.OWNER)
    result=proof._prove();self.assertEqual([x['name'] for x in result['callbacks']],[x[0] for x in g.ITEMS]);self.assertFalse(result['accepted'])
    if mode=='normal':
     # Mutate the actual retained provider ELF in the private test root.
     import shutil,struct
     candidate=roots['candidate']/'objects/CMakeFiles/d3_game_obj.dir/src/X/Client/Behavior/RadiusTrigger.cpp.obj'
     candidate.unlink();shutil.copyfile(obj,candidate)
     e=g.Elf(candidate);table=next(t for t in e.sections if t[1]==2);symbols=g.strict.symbols(e);idx=next(i for i,v in symbols.items() if v['name']==g.ITEMS[1][0]);payload=bytearray(candidate.read_bytes());payload[table[4]+idx*24+5]=0x80;candidate.write_bytes(payload)
     with self.assertRaises(ValueError):proof._prove()
     candidate.unlink();candidate.symlink_to(obj)
     actual_slot=g.slot
     for hidden in (0xa8,0xb8):
      with self.subTest(hidden=hidden), patch.object(g,'slot',side_effect=lambda meta,data,base,a,b: actual_slot(meta,data,base,a,b)+(8 if a==hidden else 0)):
       with self.assertRaises(ValueError):proof._prove()
    proof.targets=targets[:-1]
    with self.assertRaisesRegex(ValueError,'selected owner identities'):proof._prove()

class SymbolAndSlotMutationTest(unittest.TestCase):
 def test_map_uses_output_section_not_input_name(self):
  meta={'sections':[{'name':'.text','address':0x1000,'size':0x100}]}
  prefix='Address Size Align Out In Symbol\n00001000 00000100 16 .text\n'
  first='00001000 00000010 4 /work/a.obj:(.text.a)\n'
  debug='00001008 00000004 4 /work/b.obj:(.debug_spoof)\n'
  with self.assertRaisesRegex(ValueError,'overlapping'):
   g.text_map((prefix+first+debug).encode(),meta)
  separate='00000000 00001000 1 .debug_info\n'+debug
  self.assertEqual(len(g.text_map((prefix+first+separate).encode(),meta)),1)
  with self.assertRaisesRegex(ValueError,'text output differs'):
   g.text_map((prefix.replace('00000100','00000080')+first).encode(),meta)
 def test_raw_other_and_conflicting_definitions(self):
  data=bytearray(48);sym={'name':'f','defined':True,'table_section':0,'index':1};meta={'symbols':[sym], 'sections':[{'entry_size':24,'size':48,'offset':0}]}
  g.raw_symbols(meta,data,'f',True)
  for raw in (2,0x80,0xff):
   altered=bytearray(data);altered[29]=raw
   with self.assertRaisesRegex(ValueError,'visibility'):g.raw_symbols(meta,altered,'f',True)
  with self.assertRaisesRegex(ValueError,'conflicting'):g.raw_symbols({**meta,'symbols':[sym,dict(sym,defined=False)]},data,'f',True)
 def test_all_provider_metadata_mutations(self):
  for name,address,size,_,_ in g.ITEMS:
   sym={'name':name,'defined':True,'address':address,'size':size,'type':2,'binding':1,'visibility':0,'section_index':1}
   meta={'symbols':[sym], 'sections':[{'index':0,'flags':0}, {'index':1,'flags':6,'address':address,'size':size}]}
   self.assertEqual(g.unique_function(meta,name,size),address)
   for key,value in [('size',size+4),('type',1),('binding',2),('visibility',2),('section_index',0)]:
    with self.subTest(name=name,key=key):
     with self.assertRaises(ValueError):g.unique_function({**meta,'symbols':[dict(sym,**{key:value})]},name,size)
   with self.assertRaises(ValueError):g.unique_function({**meta,'symbols':[sym,dict(sym,address=address+4)]},name,size)
 def test_all_slots_fail_on_wrong_addend_and_overlap(self):
  for name,address,_,_,_ in g.ITEMS:
   meta={'segments':[{'type':1,'address':0x2000,'memory_size':16,'file_size':16,'flags':6,'offset':0}], 'sections':[{'name':'.got','type':1,'flags':3,'address':0x2000,'size':16}], 'symbols':[], 'relocations':[{'offset':0x2000,'type':1027,'symbol_index':0,'addend':address}]}
   g.got(meta,0x2000,1027,name,address)
   for bad in (address+4,0):
    m={**meta,'relocations':[dict(meta['relocations'][0],addend=bad)]}
    with self.assertRaises(ValueError):g.got(m,0x2000,1027,name,address)
   m={**meta,'relocations':meta['relocations']+[{'offset':0x1ff8,'type':1031,'symbol_index':0,'addend':0}]}
   with self.assertRaises(ValueError):g.got(m,0x2000,1027,name,address)

if __name__=='__main__':unittest.main()
