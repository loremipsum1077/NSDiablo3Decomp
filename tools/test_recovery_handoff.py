"""Local-session handoff: real isolated Git fixtures, no worker or builds."""
import csv,hashlib,json,os,shutil,subprocess,tempfile,unittest
from pathlib import Path
from unittest.mock import patch
import recovery_handoff as h
import recovery_queue as q

class HandoffTests(unittest.TestCase):
 def setUp(self):
  self.tmp=tempfile.TemporaryDirectory();self.addCleanup(self.tmp.cleanup);self.root=Path(self.tmp.name).resolve();self.repo=self.root/'repo';self.repo.mkdir()
  self.git('init','-q');self.git('config','user.name','Fixture');self.git('config','user.email','fixture@example.invalid')
  (self.repo/'.gitignore').write_text('build/\nclaims.csv\n');(self.repo/'tools').mkdir();(self.repo/'src/X').mkdir(parents=True);(self.repo/'data/2.6.2').mkdir(parents=True)
  for name in ('recovery_queue.py','recovery_handoff.py'):shutil.copyfile(Path(h.__file__).parent/name,self.repo/'tools'/name)
  for name in ('receipts.py','land.py','config.toml'):(self.repo/'tools'/name).write_text('fixture\n')
  (self.repo/'src/X/Owner.cpp').write_text('// old\n');(self.repo/'data/2.6.2/functions.csv').write_text('Address,Quality,Size,Name\n0x0000007100001000,U,000004,target\n');(self.repo/'data/2.6.2/function_files.csv').write_text('Address,File,Compiler\n0x0000007100001000,X/Owner.cpp,5.0.1\n');(self.repo/'data/2.6.2/data_symbols.csv').write_text('Address,Name\n')
  self.git('add','.');self.git('commit','-qm','baseline');self.baseline=self.git('rev-parse','HEAD')
  self.work=self.repo/'build/work/X_Owner';self.work.parent.mkdir(parents=True);self.git('worktree','add','-qb','work/owner',str(self.work),self.baseline)
  (self.work/'src/X/Owner.cpp').write_text('// new\n');self.git('add','src/X/Owner.cpp',cwd=self.work);self.git('commit','-qm','candidate',cwd=self.work);self.candidate=self.git('rev-parse','HEAD',cwd=self.work)
  (self.repo/'claims.csv').write_text('File,State,Who,When,Note\nX/Owner.cpp,claimed,fixture,,\n')
  self.inputs={}
  for name in ('original_elf','checker','nss','evidence','dwarfdump'):
   p=self.root/name;p.write_text(name);self.inputs[name]={'path':str(p),'sha256':q.digest(p)}
  self.cache=self.root/'cache';(self.cache/'out/bin').mkdir(parents=True);(self.cache/'out/bin/llc').write_bytes(b'backend');(self.cache/'receipt.json').write_text(json.dumps({'binary_sha256':q.digest(self.cache/'out/bin/llc')}))
  self.export=self.repo/'build/handoff';self.output=self.repo/'build/receipts';self.request=self.root/'request.json'
  self.value={'kind':h.REQUEST,'repo':str(self.repo),'baseline':self.baseline,'candidate':self.candidate,'owner':'X/Owner.cpp','worktree':str(self.work),'actor_label':'local author','evidence':[self.inputs['evidence']],'allowed_headers':[], 'id':'sample','profile':'standard','integration_profile':'reuse-reviewed-baseline','original_elf':self.inputs['original_elf'],'checker':self.inputs['checker'],'nss':self.inputs['nss'],'lua_cache':str(self.cache),'lua_receipt_sha256':q.digest(self.cache/'receipt.json'),'output':str(self.output),'seconds':20}
  self.write_request();self.patch=patch.object(q,'__file__',str(self.repo/'tools/recovery_queue.py'));self.patch.start();self.addCleanup(self.patch.stop)
 def git(self,*args,cwd=None):return subprocess.check_output(['git',*args],cwd=cwd or self.repo,stderr=subprocess.PIPE,text=True).strip()
 def write_request(self):self.request.write_text(json.dumps(self.value))
 def test_read_only_and_write_manifest(self):
  indexes=[Path(self.git('rev-parse','--git-path','index',cwd=x)) for x in (self.repo,self.work)]
  indexes=[p if p.is_absolute() else x/p for p,x in zip(indexes,(self.repo,self.work))]
  before=[(p.stat().st_mtime_ns,p.read_bytes()) for p in indexes]
  got=h.prepare(self.request,self.export);self.assertEqual([(p.stat().st_mtime_ns,p.read_bytes()) for p in indexes],before)
  self.assertFalse(self.export.exists());self.assertEqual(got['targets']['functions'],[{'address':'0x0000007100001000','name':'target'}]);self.assertFalse(got['handoff']['provenance']['actor_identity_verified'])
  result=h.publish(self.request,self.export);self.assertEqual(result['status'],'exported');self.assertEqual(result['handoff_sha256'],q.digest(self.export/'handoff.json'));m=q.manifest(self.export/'manifest.json');self.assertEqual(m['jobs'][0]['integration_profile'],'reuse-reviewed-baseline');self.assertFalse(self.output.exists())
 def test_stale_inputs_and_candidate_are_blocked_before_write(self):
  for mutation in ('baseline','candidate','owner','evidence','dirty','cache'):
   with self.subTest(mutation=mutation):
    saved=dict(self.value)
    if mutation=='baseline':self.value['baseline']='0'*40
    if mutation=='candidate':self.value['candidate']='1'*40
    if mutation=='owner':self.value['owner']='X/Other.cpp'
    if mutation=='evidence':self.value['evidence']=[dict(self.inputs['evidence'],sha256='0'*64)]
    if mutation=='dirty':(self.work/'src/X/Owner.cpp').write_text('// dirty\n')
    if mutation=='cache':(self.cache/'out/bin/llc').write_bytes(b'changed')
    self.write_request()
    with self.assertRaises((ValueError,OSError,subprocess.SubprocessError)):h.publish(self.request,self.export)
    self.assertFalse(self.export.exists());self.value=saved
    if mutation=='dirty':self.git('checkout','--','src/X/Owner.cpp',cwd=self.work)
    if mutation=='cache':(self.cache/'out/bin/llc').write_bytes(b'backend')
  self.write_request()
 def test_wrong_scope_missing_claim_and_output_overlap(self):
  (self.work/'src/X/Other.cpp').write_text('// unrelated\n');self.git('add','src/X/Other.cpp',cwd=self.work);self.git('commit','-qm','extra',cwd=self.work);self.value['candidate']=self.git('rev-parse','HEAD',cwd=self.work);self.write_request()
  with self.assertRaisesRegex(ValueError,'scope'):h.prepare(self.request,self.export)
  self.git('reset','--hard','HEAD~1',cwd=self.work);self.value['candidate']=self.candidate;(self.repo/'claims.csv').write_text('File,State,Who,When,Note\n');self.write_request()
  with self.assertRaisesRegex(ValueError,'claim'):h.prepare(self.request,self.export)
  (self.repo/'claims.csv').write_text('File,State,Who,When,Note\nX/Owner.cpp,stuck,fixture,,\n')
  with self.assertRaisesRegex(ValueError,'claim'):h.prepare(self.request,self.export)
  (self.repo/'claims.csv').write_text('File,State,Who,When,Note\nX/Owner.cpp,claimed,fixture,,\n');self.value['output']=str(self.export/'receipts');self.write_request()
  with self.assertRaisesRegex(ValueError,'overlap'):h.prepare(self.request,self.export)
 def test_named_profile_and_incorrect_reader_pin(self):
  self.value['profile']='named-writable';self.value['dwarfdump']=dict(self.inputs['dwarfdump'],sha256='0'*64);self.write_request()
  with self.assertRaisesRegex(ValueError,'pin changed'):h.prepare(self.request,self.export)
  self.value['dwarfdump']=self.inputs['dwarfdump'];self.write_request();self.assertIn('dwarfdump',h.prepare(self.request,self.export)['manifest']['jobs'][0])
 def test_blank_actor_and_rehashed_approval_claims_rejected(self):
  self.value['actor_label']='   ';self.write_request()
  with self.assertRaisesRegex(ValueError,'actor label'):h.prepare(self.request,self.export)
  self.value['actor_label']='local author';self.write_request();h.publish(self.request,self.export)
  value=json.loads((self.export/'handoff.json').read_text())
  state=self.repo/'build/queue-state';state.mkdir();(state/'.lock').write_bytes(b'')
  for change in (lambda v:v.update(accepted=True),lambda v:v.update(review_completed=True),
                 lambda v:v.update(extra_approval=True),lambda v:v['provenance'].update(declared_actor='  '),
                 lambda v:v.update(allowed_headers=['src/X/Other.h'])):
   altered=json.loads(json.dumps(value));change(altered);path=self.repo/'build/forged-handoff.json';path.write_text(json.dumps(altered))
   with self.subTest(altered=altered),self.assertRaises(ValueError):
    h.review_index(self.export/'manifest.json',state,path,q.digest(path),'sample')
 def test_explicit_header_authority_and_worktree_outputs(self):
  (self.work/'src/X/Needed.h').write_text('// header\n');self.git('add','src/X/Needed.h',cwd=self.work);self.git('commit','-qm','header',cwd=self.work)
  self.value['candidate']=self.git('rev-parse','HEAD',cwd=self.work);self.write_request()
  with self.assertRaisesRegex(ValueError,'scope'):h.prepare(self.request,self.export)
  self.value['allowed_headers']=['src/X/Needed.h'];self.write_request()
  self.assertEqual(h.prepare(self.request,self.export)['handoff']['allowed_headers'],['src/X/Needed.h'])
  self.value['output']=str(self.work/'build/receipts');self.write_request()
  with self.assertRaisesRegex(ValueError,'worktree'):h.prepare(self.request,self.export)
  self.value['output']=str(self.output);self.write_request()
  with self.assertRaisesRegex(ValueError,'worktree'):h.prepare(self.request,self.work/'build/handoff')
 def test_active_ownership_controller_blocks_without_writes(self):
  queue=self.repo/'build/work-queue/queue.json';queue.parent.mkdir(parents=True);queue.write_text(json.dumps({'items':{'other':{'active':True}}}))
  with self.assertRaisesRegex(RuntimeError,'active claim'):h.prepare(self.request,self.export)
  self.assertFalse(self.export.exists())
 def test_replace_ref_cannot_rewrite_candidate_provenance(self):
  name='src/X/Owner.cpp';expected=h._blob(self.repo,self.candidate,name)
  (self.work/name).write_text('// replacement\n');self.git('add',name,cwd=self.work);self.git('commit','-qm','alternate',cwd=self.work);alternate=self.git('rev-parse','HEAD',cwd=self.work)
  self.git('reset','--hard',self.candidate,cwd=self.work);self.git('replace',self.candidate,alternate)
  self.assertEqual(h._blob(self.repo,self.candidate,name),expected)
  self.assertEqual(h.changed_files(self.repo,self.baseline,self.candidate),[name])
 def test_cli_preflight_export_and_failure(self):
  env=dict(os.environ,PYTHONPATH=str(Path(h.__file__).parent))
  cmd=[os.sys.executable,'-m','recovery_handoff','prepare','--request',str(self.request),'--output',str(self.export)]
  read=subprocess.run(cmd,env=env,text=True,capture_output=True);self.assertEqual(read.returncode,0,read.stdout);self.assertEqual(json.loads(read.stdout)['writes'],0);self.assertFalse(self.export.exists())
  self.value['baseline']='0'*40;self.write_request();bad=subprocess.run(cmd+['--write'],env=env,text=True,capture_output=True);self.assertNotEqual(bad.returncode,0);self.assertFalse(self.export.exists())
  self.value['baseline']=self.baseline;self.write_request();result=subprocess.run(cmd+['--write'],env=env,text=True,capture_output=True);self.assertEqual(result.returncode,0,result.stdout);self.assertEqual(json.loads(result.stdout)['status'],'exported')
 def test_review_index_is_read_only_and_requires_fresh_claim_and_pins(self):
  h.publish(self.request,self.export);manifest=self.export/'manifest.json';handoff=self.export/'handoff.json';pin=q.digest(handoff)
  self.output.mkdir();blob=h._blob(self.repo,self.candidate,'src/X/Owner.cpp')
  (self.output/'binding.json').write_text(json.dumps({'versions':{'candidate':{'source':{'src/X/Owner.cpp':{'object':blob['git_blob'],'sha256':blob['sha256'],'executable':False}}}}}))
  state=self.repo/'build/queue-state';state.mkdir();(state/'.lock').write_bytes(b'')
  value={'format':1,'manifest_sha256':q.digest(manifest),'provider_calls':0,'jobs':{'sample':{'phase':'awaiting_review','attempts':[],'validation_artifacts':{'audit':'a'}}}}
  (state/'state.json').write_text(json.dumps(value));original=(state/'state.json').read_bytes()
  with patch.object(q,'validation_pins',return_value={'audit':'a'}):
   result=h.review_index(manifest,state,handoff,pin,'sample');self.assertEqual(result['status'],'review_required');self.assertEqual(result['receipt_verification'],'required_by_independent_reviewer');self.assertFalse(result['actor_identity_verified']);self.assertEqual((state/'state.json').read_bytes(),original)
   with self.assertRaisesRegex(ValueError,'pin changed'):h.review_index(manifest,state,handoff,'0'*64,'sample')
   binding=self.output/'binding.json';saved=binding.read_text();value=json.loads(saved);value['versions']['candidate']['source']['src/X/Owner.cpp']['sha256']='0'*64;binding.write_text(json.dumps(value))
   with self.assertRaisesRegex(ValueError,'receipt candidate source differs'):h.review_index(manifest,state,handoff,pin,'sample')
   binding.write_text(saved)
   (self.repo/'claims.csv').write_text('File,State,Who,When,Note\nX/Owner.cpp,claimed,fixture,,\nX/Unrelated.cpp,claimed,other,,\n')
   self.assertEqual(h.review_index(manifest,state,handoff,pin,'sample')['status'],'review_required')
   (self.repo/'claims.csv').write_text('File,State,Who,When,Note\nX/Owner.cpp,claimed,different,,\n')
   with self.assertRaisesRegex(ValueError,'claim or candidate'):h.review_index(manifest,state,handoff,pin,'sample')
 def test_review_index_rejects_state_receipt_and_lock_changes(self):
  h.publish(self.request,self.export);manifest=self.export/'manifest.json';handoff=self.export/'handoff.json';pin=q.digest(handoff)
  self.output.mkdir();blob=h._blob(self.repo,self.candidate,'src/X/Owner.cpp')
  binding={'policy':'recovery-only','post_landing':False,'owner':'X/Owner.cpp','versions':{'baseline':{'commit':self.baseline},'candidate':{'commit':self.candidate,'source':{'src/X/Owner.cpp':{'object':blob['git_blob'],'sha256':blob['sha256'],'executable':False}}}}}
  for name,value in [('binding.json',binding),('evidence.json',{}),('recovery-audit.json',{}),('review-template.json',{})]:
   (self.output/name).write_text(json.dumps(value))
  run={'status':'recovery_evidence_complete','accepted':False,'provider_calls':0,'binding_sha256':q.digest(self.output/'binding.json'),'evidence_sha256':q.digest(self.output/'evidence.json'),'audit_sha256':q.digest(self.output/'recovery-audit.json')}
  (self.output/'run.json').write_text(json.dumps(run));m=q.manifest(manifest);job=m['jobs'][0];pins=q.validation_pins(job)
  state=self.repo/'build/queue-state';state.mkdir();lock=state/'.lock';lock.write_bytes(b'')
  value={'format':1,'manifest_sha256':q.digest(manifest),'provider_calls':0,'jobs':{'sample':{'phase':'awaiting_review','attempts':[],'validation_artifacts':pins}}};statefile=state/'state.json';statefile.write_text(json.dumps(value))
  def index():return h.review_index(manifest,state,handoff,pin,'sample')
  self.assertEqual(index()['status'],'review_required')
  statefile.write_text(json.dumps({**value,'jobs':{'sample':{'phase':'landed','attempts':[]}}}))
  with self.assertRaisesRegex(ValueError,'queue evidence'):index()
  statefile.write_text(json.dumps(value))
  for name in ('evidence.json','review-template.json'):
   path=self.output/name;before=path.read_bytes();path.write_bytes(b'{"changed":true}')
   with self.subTest(name=name),self.assertRaises(ValueError):index()
   path.write_bytes(before)
  actual=q.validation_pins
  def replace_lock(*a,**kw):
   result=actual(*a,**kw)
   if lock.exists():lock.unlink();lock.write_bytes(b'')
   return result
  with patch.object(q,'validation_pins',side_effect=replace_lock),self.assertRaisesRegex(ValueError,'lock replaced'):index()

if __name__=='__main__':unittest.main()
