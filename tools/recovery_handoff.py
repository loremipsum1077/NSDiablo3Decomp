#!/usr/bin/env python3
"""Provider-free local-session handoff to the existing recovery queue.

No source, claims, commits, review decisions, builds, or queue state are created.
`prepare` defaults to read-only; --write publishes a new handoff directory.
`review-index` checks bound metadata and prints an index; receipt replay remains
required of the independent reviewer and the existing landing gate.
"""
import argparse
from contextlib import contextmanager
import csv
import fcntl
import hashlib
import io
import json
import os
from pathlib import Path
import re
import stat
import subprocess

import recovery_queue as q
import recovery as recovery
import receipts
import claim

KIND='local-recovery-handoff-v1'
REQUEST='local-recovery-handoff-request-v1'
LIMIT=8*1024*1024

def require(ok,message):
    if not ok:raise ValueError(message)

def encode(value):return (json.dumps(value,sort_keys=True,separators=(',',':'))+'\n').encode()
def sha(data):return hashlib.sha256(data).hexdigest()

def _git(repo,*args):
    # Even git status may otherwise opportunistically refresh the index.
    env=dict(os.environ,GIT_OPTIONAL_LOCKS='0')
    return subprocess.check_output(['git','--no-replace-objects','-C',str(repo),*args],
                                   stderr=subprocess.PIPE,timeout=30,env=env).decode('utf-8','strict').strip()

def changed_files(repo,baseline,candidate):
    raw=subprocess.check_output(['git','--no-replace-objects','diff','--no-renames','--name-only','-z',baseline,candidate],cwd=repo,timeout=30)
    return raw.decode('utf-8','strict').split('\0')[:-1]

def _outside_worktrees(repo,*paths):
    raw=subprocess.check_output(['git','--no-replace-objects','worktree','list','--porcelain','-z'],cwd=repo,timeout=30)
    for item in raw.decode('utf-8','strict').split('\0'):
        if not item.startswith('worktree '):continue
        work=Path(item[len('worktree '):]).resolve()
        if work==repo:continue
        require(all(not p.is_relative_to(work) and not work.is_relative_to(p) for p in paths),
                'handoff/output overlaps a registered worktree')

def tool_pins(repo):
    result={}
    raw=subprocess.check_output(['git','--no-replace-objects','ls-files','--stage','-z','--','tools'],cwd=repo,timeout=30).decode()
    for record in raw.split('\0'):
        if not record:continue
        header,path=record.split('\t',1);mode,_,stage=header.split()
        require(stage=='0','unmerged tool')
        if mode=='160000':
            require(path in q.NON_RUNTIME_GITLINKS,'unsupported tool gitlink');continue
        require(mode in ('100644','100755'),'unsupported tool file')
        f=q.canonical(str(repo/path));result[path]=q.digest(q.regular(f,64*1024*1024))
    require(result.get('tools/recovery_queue.py')==q.digest(Path(q.__file__)),'queue implementation mismatch')
    require(result.get('tools/recovery_handoff.py')==q.digest(Path(__file__)),'handoff implementation mismatch')
    return result

def _blob(repo,commit,path):
    raw=subprocess.check_output(['git','--no-replace-objects','ls-tree','-z',commit,'--',path],cwd=repo,timeout=30)
    entries=[x for x in raw.split(b'\0') if x];require(len(entries)==1,'candidate blob missing or ambiguous')
    header,name=entries[0].split(b'\t',1);mode,kind,oid=header.decode().split()
    require(mode in ('100644','100755') and kind=='blob' and name.decode()==path,'unsupported candidate file')
    content=subprocess.check_output(['git','--no-replace-objects','show',commit+':'+path],cwd=repo,timeout=30)
    require(len(content)<=16*1024*1024,'candidate source file exceeds bound')
    return {'git_blob':oid,'sha256':sha(content),'size':len(content),'mode':mode}

def _targets(repo,baseline,owner):
    owners=recovery.rows(repo,baseline,'function_files.csv');table=recovery.rows(repo,baseline)
    addresses=[r['Address'] for r in owners if r['File']==owner]
    require(addresses and len(addresses)==len(set(addresses)),'missing/ambiguous owner inventory')
    selected=[{'address':r['Address'],'name':r['Name']} for r in table if r['Address'] in set(addresses)]
    require(1<=len(selected)<=64 and len(selected)==len(addresses) and
            len({(r['address'],r['name']) for r in selected})==len(selected) and
            all(r['name'] and '@' not in r['name'] for r in selected),'unsupported complete owner')
    return {'format':1,'functions':selected}

def _claim(path,owner):
    fd=os.open(path,os.O_RDONLY|os.O_NOFOLLOW|os.O_NONBLOCK)
    try:
        before=os.fstat(fd);require(stat.S_ISREG(before.st_mode) and before.st_size<=LIMIT,'invalid claim file')
        raw=os.read(fd,before.st_size+1);after=os.fstat(fd);entry=os.stat(path,follow_symlinks=False)
        key=lambda s:(s.st_dev,s.st_ino,s.st_size,s.st_mtime_ns,s.st_ctime_ns)
        require(len(raw)==before.st_size and key(before)==key(after)==key(entry),'claim file changed')
        rows=list(csv.DictReader(io.StringIO(raw.decode('utf-8','strict'))))
        selected=[x for x in rows if x.get('File')==owner]
        require(len(selected)==1 and selected[0].get('State') in ('claimed','done'),'owner claim missing or ambiguous')
        # Other owners' independent claims may change between stages.
        return {'record':selected[0]}
    finally:os.close(fd)

def validate_handoff(value):
    keys={'format','kind','provenance','baseline','candidate','owner','worktree','candidate_files',
          'targets_sha256','queue_manifest_sha256','request_sha256','claim','allowed_headers',
          'accepted','exact_certified','review_completed'}
    require(isinstance(value,dict) and set(value)==keys and type(value['format']) is int and value['format']==1 and value['kind']==KIND and
            value['accepted'] is False and value['exact_certified'] is False and value['review_completed'] is False,
            'invalid handoff schema or unsupported approval claim')
    provenance=value['provenance']
    require(isinstance(provenance,dict) and set(provenance)=={'kind','declared_actor','evidence','actor_identity_verified'} and
            provenance['kind']=='local-session' and provenance['actor_identity_verified'] is False and
            isinstance(provenance['declared_actor'],str) and 0<len(provenance['declared_actor'])<=120 and
            bool(provenance['declared_actor'].strip()) and '\0' not in provenance['declared_actor'] and
            isinstance(provenance['evidence'],list) and 1<=len(provenance['evidence'])<=64,
            'invalid local-session provenance')
    for name in ('baseline','candidate'):
        require(isinstance(value[name],str) and q.COMMIT.fullmatch(value[name]),'invalid handoff commit')
    for name in ('targets_sha256','queue_manifest_sha256','request_sha256'):
        require(isinstance(value[name],str) and q.SHA.fullmatch(value[name]),'invalid handoff digest')
    q.owner(value['owner']);q.canonical(value['worktree'])
    headers=value['allowed_headers']
    require(isinstance(headers,list) and len(headers)<=64 and all(isinstance(p,str) for p in headers) and
            headers==sorted(set(headers)) and all(p.startswith(('src/','include/')) and p.endswith(('.h','.hpp','.inl')) and
            '\\' not in p and '\0' not in p and all(part not in ('','.','..') for part in p.split('/')) for p in headers),
            'invalid explicit header allowlist')
    require(isinstance(value['candidate_files'],dict) and set(value['candidate_files'])=={'src/'+value['owner'],*headers},
            'handoff source scope differs')
    return value

def prepare(request_path,export_dir):
    request_path=q.canonical(str(Path(request_path).absolute()));v=q.read(request_path)
    required={'kind','repo','baseline','candidate','owner','worktree','actor_label','evidence','allowed_headers',
              'id','profile','integration_profile','original_elf','checker','nss','lua_cache',
              'lua_receipt_sha256','output','seconds'}
    require(isinstance(v,dict) and v.get('kind')==REQUEST,'unsupported request')
    if v.get('profile')=='named-writable':required.add('dwarfdump')
    require(set(v)==required and v['profile'] in ('standard','named-writable') and
            v['integration_profile'] in ('fresh-all','reuse-reviewed-baseline'),'unsupported request fields')
    repo=q.canonical(v['repo']);owner=q.owner(v['owner']);worktree=q.canonical(v['worktree'])
    export=q.canonical(str(Path(export_dir).absolute()));output=q.canonical(v['output']);cache=q.canonical(v['lua_cache'])
    require(Path(_git(repo,'rev-parse','--show-toplevel'))==repo and
            Path(_git(repo,'rev-parse','--path-format=absolute','--git-common-dir'))==repo/'.git','primary checkout required')
    q.isolated(export,repo,cache);q.isolated(output,repo,cache)
    require(export.is_relative_to(repo/'build'),'handoff must be under build/')
    _outside_worktrees(repo,export,output)
    require(export!=output and not export.is_relative_to(output) and not output.is_relative_to(export),'handoff/output overlap')
    require(not export.exists() and not output.exists(),'handoff or receipt output exists')
    require(not _git(repo,'status','--porcelain','--untracked-files=all'),'primary checkout must be clean')
    require(q.COMMIT.fullmatch(v['baseline']) and q.COMMIT.fullmatch(v['candidate']) and
            _git(repo,'rev-parse','HEAD')==v['baseline'] and
            _git(repo,'rev-parse',v['candidate']+'^{commit}')==v['candidate'] and v['candidate']!=v['baseline'],'stale baseline/candidate')
    subprocess.run(['git','--no-replace-objects','merge-base','--is-ancestor',v['baseline'],v['candidate']],cwd=repo,check=True,timeout=30,stdout=subprocess.DEVNULL,stderr=subprocess.PIPE)
    slug=owner.rsplit('.',1)[0].replace('/','_').replace(' ','_');require(worktree==repo/'build/work'/slug,'worktree does not match owner')
    require(worktree.is_dir() and _git(worktree,'rev-parse','--show-toplevel')==str(worktree) and
            _git(worktree,'rev-parse','--path-format=absolute','--git-common-dir')==str(repo/'.git') and
            _git(worktree,'rev-parse','HEAD')==v['candidate'] and
            not _git(worktree,'status','--porcelain','--untracked-files=all'),'candidate worktree changed')
    claim.check_coordination(repo)
    claim_row=_claim(repo/'claims.csv',owner)
    diff=changed_files(repo,v['baseline'],v['candidate']);source='src/'+owner
    headers=v['allowed_headers']
    require(isinstance(headers,list) and len(headers)<=64 and all(isinstance(p,str) for p in headers) and
            headers==sorted(set(headers)) and
            all(p.startswith(('src/','include/')) and p.endswith(('.h','.hpp','.inl')) and
                '\\' not in p and all(part not in ('','.','..') for part in p.split('/')) for p in headers),
            'invalid explicit header allowlist')
    require(len(diff)==len(set(diff)) and set(diff)=={source,*headers},'candidate scope exceeds explicit owner/header paths')
    blobs={name:_blob(repo,v['candidate'],name) for name in sorted(diff)}
    targets=_targets(repo,v['baseline'],owner)
    recovery.inventory(recovery.rows(repo,v['baseline']),recovery.rows(repo,v['candidate']),targets['functions'])
    for name in receipts.SUPPORT:
        require(receipts.committed_files(repo,v['baseline'],[name])==receipts.committed_files(repo,v['candidate'],[name]),'candidate changed checker inputs')
    require(isinstance(v['actor_label'],str) and 0<len(v['actor_label'])<=120 and v['actor_label'].strip() and '\0' not in v['actor_label'],'actor label required')
    require(isinstance(v['evidence'],list) and 1<=len(v['evidence'])<=64,'source evidence pins required')
    pins=[v[k] for k in ('original_elf','checker','nss')]+v['evidence']+([v['dwarfdump']] if 'dwarfdump' in v else [])
    inputs=[q.pin(x) for x in pins];inputs.append(request_path);inputs.append(cache)
    require(cache.is_dir() and re.fullmatch('[0-9a-f]{64}',v['lua_receipt_sha256']) and
            q.digest(q.regular(cache/'receipt.json'))==v['lua_receipt_sha256'],'Lua receipt pin differs')
    lua=q.read(cache/'receipt.json')
    require(isinstance(lua.get('binary_sha256'),str) and q.SHA.fullmatch(lua['binary_sha256']) and
            q.digest(q.regular(cache/'out/bin/llc',512*1024*1024))==lua['binary_sha256'],'Lua backend differs')
    for p in inputs:
        require(not p.is_relative_to(export) and not export.is_relative_to(p) and
                not p.is_relative_to(output) and not output.is_relative_to(p),'handoff overlaps input')
    require(re.fullmatch('[A-Za-z0-9][A-Za-z0-9_-]{0,63}',v['id']) and type(v['seconds']) is int and 1<=v['seconds']<=86400,'invalid job id or deadline')
    tools=tool_pins(repo);target_bytes=encode(targets);target_pin={'path':str(export/'targets.json'),'sha256':sha(target_bytes)}
    job={k:v[k] for k in ('id','baseline','candidate','profile','owner','original_elf','checker','nss','lua_cache','lua_receipt_sha256','output','seconds','integration_profile')}
    job.update(mode='recovery-only',targets=target_pin)
    if 'dwarfdump' in v:job['dwarfdump']=v['dwarfdump']
    manifest={'format':1,'kind':q.KIND,'repo':str(repo),'queue_sha256':q.digest(Path(q.__file__)),'tools':tools,'jobs':[job]}
    handoff={'format':1,'kind':KIND,'provenance':{'kind':'local-session','declared_actor':v['actor_label'],'evidence':v['evidence'],
             'actor_identity_verified':False},'baseline':v['baseline'],'candidate':v['candidate'],'owner':owner,
             'worktree':str(worktree),'candidate_files':blobs,'targets_sha256':sha(target_bytes),
             'claim':claim_row,'allowed_headers':headers,
             'queue_manifest_sha256':sha(encode(manifest)),'request_sha256':q.digest(request_path),
             'accepted':False,'exact_certified':False,'review_completed':False}
    validate_handoff(handoff)
    return {'request':v,'repo':repo,'export':export,'targets':targets,'manifest':manifest,'handoff':handoff}

def publish(request,export):
    first=prepare(request,export);claim_path=first['repo']/'claims.csv'
    claim_fd=os.open(claim_path,os.O_RDONLY|os.O_NOFOLLOW|os.O_NONBLOCK)
    try:
        fcntl.flock(claim_fd,fcntl.LOCK_SH|fcntl.LOCK_NB)
        claim_stat=os.fstat(claim_fd)
        def check_claim_lock():
            current=os.stat(claim_path,follow_symlinks=False)
            require(stat.S_ISREG(claim_stat.st_mode) and (claim_stat.st_dev,claim_stat.st_ino)==
                    (current.st_dev,current.st_ino),'claim lock replaced')
        check_claim_lock();claim.check_coordination(first['repo']);second=prepare(request,export);require(first==second,'handoff inputs changed before export')
        directory=first['export'];directory.mkdir(mode=0o700,parents=False,exist_ok=False)
        for name,value in (('targets.json',first['targets']),('manifest.json',first['manifest']),('handoff.json',first['handoff'])):
            with (directory/name).open('xb') as stream:
                stream.write(encode(value));stream.flush();os.fsync(stream.fileno())
        q.manifest(directory/'manifest.json');check_claim_lock()
        # Inputs must still be identical; permit our newly created output only.
        require(_claim(claim_path,first['handoff']['owner'])==first['handoff']['claim'] and
                q.digest(Path(request))==first['handoff']['request_sha256'] and
                _git(first['repo'],'rev-parse','HEAD')==first['handoff']['baseline'] and
                not _git(first['repo'],'status','--porcelain','--untracked-files=all') and
                _git(Path(first['handoff']['worktree']),'rev-parse','HEAD')==first['handoff']['candidate'] and
                not _git(Path(first['handoff']['worktree']),'status','--porcelain','--untracked-files=all'),
                'handoff inputs changed during export')
        for pin in first['request']['evidence']:q.pin(pin)
        cache=Path(first['request']['lua_cache']);lua=q.read(cache/'receipt.json')
        require(q.digest(q.regular(cache/'receipt.json'))==first['request']['lua_receipt_sha256'] and
                q.digest(q.regular(cache/'out/bin/llc',512*1024*1024))==lua.get('binary_sha256'),
                'Lua backend changed during export')
        for name in ('targets.json','manifest.json','handoff.json'):
            out_fd=os.open(directory/name,os.O_RDONLY|os.O_NOFOLLOW)
            try:os.fchmod(out_fd,0o400);os.fsync(out_fd)
            finally:os.close(out_fd)
        os.chmod(directory,0o500)
        fd=os.open(directory,os.O_RDONLY)
        try:os.fsync(fd)
        finally:os.close(fd)
        fd=os.open(directory.parent,os.O_RDONLY)
        try:os.fsync(fd)
        finally:os.close(fd)
        check_claim_lock()
        return {'status':'exported','manifest':str(directory/'manifest.json'),'handoff':str(directory/'handoff.json'),
                'manifest_sha256':q.digest(directory/'manifest.json'),'handoff_sha256':q.digest(directory/'handoff.json'),
                'provider_calls':0,'accepted':False,'exact_certified':False}
    finally:os.close(claim_fd)

@contextmanager
def _queue_lock(directory):
    path=directory/'.lock';fd=os.open(path,os.O_RDONLY|os.O_NOFOLLOW|os.O_NONBLOCK)
    try:
        fcntl.flock(fd,fcntl.LOCK_SH|fcntl.LOCK_NB)
        original=os.fstat(fd)
        def check():
            current=os.stat(path,follow_symlinks=False)
            require(stat.S_ISREG(original.st_mode) and stat.S_ISREG(current.st_mode) and current.st_nlink==1 and
                    (original.st_dev,original.st_ino)==(current.st_dev,current.st_ino),'queue lock replaced')
        check();yield check;check()
    finally:os.close(fd)

def review_index(manifest_path,state_dir,handoff_path,handoff_sha256,job_id):
    path=q.canonical(str(Path(manifest_path).absolute()));m=q.manifest(path);directory=q.canonical(str(Path(state_dir).absolute()))
    q.isolated(directory,Path(m['repo']))
    for job in m['jobs']:
        output=q.canonical(job['output']);q.isolated(directory,Path(m['repo']),q.canonical(job['lua_cache']))
        require(not directory.is_relative_to(output) and not output.is_relative_to(directory),'state/output overlap')
    with _queue_lock(directory) as check:
        value=_review_index_locked(path,m,directory,handoff_path,handoff_sha256,job_id)
        check();return value

def _review_index_locked(path,m,directory,handoff_path,handoff_sha256,job_id):
    handoff_path=q.pin({'path':str(Path(handoff_path).absolute()),'sha256':handoff_sha256})
    handoff=validate_handoff(q.read(handoff_path));require(handoff['queue_manifest_sha256']==q.digest(path),'handoff/manifest mismatch')
    job=next((j for j in m['jobs'] if j['id']==job_id),None);require(job is not None,'unknown job')
    require((handoff.get('baseline'),handoff.get('candidate'),handoff.get('owner'))==
            (job['baseline'],job['candidate'],job['owner']) and handoff.get('targets_sha256')==job['targets']['sha256'],
            'handoff candidate identity differs')
    claim.check_coordination(Path(m['repo']))
    provenance=handoff.get('provenance');require(isinstance(provenance,dict) and provenance.get('kind')=='local-session' and
            provenance.get('actor_identity_verified') is False and isinstance(provenance.get('evidence'),list),'invalid local provenance')
    for evidence in provenance['evidence']:q.pin(evidence)
    files=changed_files(Path(m['repo']),job['baseline'],job['candidate'])
    require(set(files)=={'src/'+job['owner'],*handoff['allowed_headers']} and
            handoff.get('candidate_files')=={name:_blob(Path(m['repo']),job['candidate'],name) for name in sorted(files)},
            'candidate source blobs differ')
    require(_git(Path(m['repo']),'rev-parse','HEAD')==job['baseline'] and
            not _git(Path(m['repo']),'status','--porcelain','--untracked-files=all'),'baseline changed')
    worktree=q.canonical(handoff['worktree'])
    require(worktree==Path(m['repo'])/'build/work'/job['owner'].rsplit('.',1)[0].replace('/','_').replace(' ','_') and
            _git(worktree,'rev-parse','--show-toplevel')==str(worktree) and
            _git(worktree,'rev-parse','--path-format=absolute','--git-common-dir')==str(Path(m['repo'])/'.git') and
            _git(worktree,'rev-parse','HEAD')==job['candidate'] and
            not _git(worktree,'status','--porcelain','--untracked-files=all') and
            _claim(Path(m['repo'])/'claims.csv',job['owner'])==handoff['claim'],'claim or candidate worktree changed')
    state=q.load_state(directory,m,q.digest(path));row=state['jobs'][job_id]
    require(row['phase']=='awaiting_review' and q.validation_pins(job)==row['validation_artifacts'],'validated queue evidence changed')
    binding=q.read(Path(job['output'])/'binding.json',64*1024*1024)
    source=binding['versions']['candidate']['source']
    for name,blob in handoff['candidate_files'].items():
        require(source.get(name)=={'object':blob['git_blob'],'sha256':blob['sha256'],
                                   'executable':blob['mode']=='100755'},'receipt candidate source differs')
    require(q.load_state(directory,m,q.digest(path))==state and q.validation_pins(job)==row['validation_artifacts'] and
            q.manifest(path)==m and q.digest(handoff_path)==handoff_sha256 and
            _git(Path(m['repo']),'rev-parse','HEAD')==job['baseline'] and
            not _git(Path(m['repo']),'status','--porcelain','--untracked-files=all') and
            _git(worktree,'rev-parse','HEAD')==job['candidate'] and
            not _git(worktree,'status','--porcelain','--untracked-files=all') and
            _claim(Path(m['repo'])/'claims.csv',job['owner'])==handoff['claim'],
            'queue or handoff changed during inspection')
    for evidence in provenance['evidence']:q.pin(evidence)
    return {'status':'review_required','provider_calls':0,'accepted':False,'exact_certified':False,
            'receipt_verification':'required_by_independent_reviewer',
            'handoff_sha256':q.digest(Path(handoff_path)),'manifest_sha256':q.digest(path),'job':job_id,
            'declared_actor':provenance.get('declared_actor'),'actor_identity_verified':False,
            'source_evidence':provenance['evidence'],'candidate_files':handoff['candidate_files'],
            'baseline':job['baseline'],'candidate':job['candidate'],'owner':job['owner'],
            'evidence':row['validation_artifacts'],'receipt_output':job['output']}

def main(argv=None):
    p=argparse.ArgumentParser(description=__doc__);sub=p.add_subparsers(dest='action',required=True)
    a=sub.add_parser('prepare');a.add_argument('--request',required=True,type=Path);a.add_argument('--output',required=True,type=Path);a.add_argument('--write',action='store_true')
    b=sub.add_parser('review-index');b.add_argument('--manifest',required=True,type=Path);b.add_argument('--state',required=True,type=Path);b.add_argument('--handoff',required=True,type=Path);b.add_argument('--handoff-sha256',required=True);b.add_argument('--job',required=True)
    args=p.parse_args(argv)
    try:
        if args.action=='prepare':
            result=publish(args.request,args.output) if args.write else {'status':'preflight','writes':0,'provider_calls':0,'accepted':False,'exact_certified':False,'handoff':prepare(args.request,args.output)['handoff']}
        else:result=review_index(args.manifest,args.state,args.handoff,args.handoff_sha256,args.job)
        print(json.dumps(result,sort_keys=True));return 0
    except (ValueError,OSError,KeyError,TypeError,RuntimeError,subprocess.SubprocessError) as e:
        print(json.dumps({'status':'failed','accepted':False,'provider_calls':0,'error':str(e)}));return 1
if __name__=='__main__':raise SystemExit(main())
