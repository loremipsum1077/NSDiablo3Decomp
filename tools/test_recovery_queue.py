"""Exercise the real queue CLI using isolated Git repos and fixed fixture tools.

Fixture tools test dispatch/state only: they are never source/build evidence.
"""
import fcntl
import hashlib
import json
import os
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest

import recovery_queue as queue


FIXTURE = r'''
import hashlib,json,os,pathlib,subprocess,sys,time
root=pathlib.Path(__file__).resolve().parents[1]
def arg(flag):return sys.argv[sys.argv.index(flag)+1]
def write(p,x):p.parent.mkdir(parents=True,exist_ok=True);p.write_text(json.dumps(x,sort_keys=True)+'\n')
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
with pathlib.Path(EVENTS).open('a') as f:f.write(json.dumps(sys.argv)+'\n')
mode=os.environ.get('QUEUE_FIXTURE_MODE','')
if mode=='fail':print('fixture failure');sys.exit(3)
if mode=='sleep':time.sleep(30)
if mode=='replace-lock':
 lock=pathlib.Path(os.environ['QUEUE_FIXTURE_LOCK_PATH']);lock.unlink();lock.write_text('replacement')
if pathlib.Path(__file__).name=='receipts.py':
 out=pathlib.Path(arg('--output'));out.mkdir(parents=True,exist_ok=False)
 policy=arg('--mode');group=policy=='recovery-group'
 binding={'policy':policy,'post_landing':False,'versions':{v:{'commit':arg('--'+v)} for v in ('baseline','candidate')}}
 if group:binding['owners']=json.loads(pathlib.Path(arg('--owners')).read_text())['owners']
 else:binding['owner']=arg('--owner')
 if mode=='wrong-binding':binding['versions']['candidate']['commit']=binding['versions']['baseline']['commit']
 audit='recovery-group-audit.json' if group else 'recovery-audit.json'
 for name,value in [('binding.json',binding),('evidence.json',{'fixture':True}),
                    (audit,{'fixture':True}),('review-template.json',{'fixture':True})]:write(out/name,value)
 write(out/'run.json',{'status':'recovery_evidence_complete','accepted':False,'provider_calls':0,
       'binding_sha256':sha(out/'binding.json'),'evidence_sha256':sha(out/'evidence.json'),'audit_sha256':sha(out/audit)})
 print(json.dumps({'status':'recovery_evidence_complete','accepted':False}))
else:
 assert '--recovery-only' in sys.argv or '--recovery-group' in sys.argv
 review=json.loads(pathlib.Path(arg('--review')).read_text())
 if review.get('decision')!='approve':print('review rejected');sys.exit(4)
 binding=json.loads((pathlib.Path(arg('--recovery-receipts'))/'binding.json').read_text())
 subprocess.run(['git','merge','--ff-only',binding['versions']['candidate']['commit']],cwd=root,check=True,stdout=subprocess.DEVNULL)
 subprocess.run(['git','commit','--allow-empty','-qm','fixture integration'],cwd=root,check=True)
 commit=subprocess.check_output(['git','rev-parse','HEAD'],cwd=root,text=True).strip()
 status='recovery_group_landed' if '--recovery-group' in sys.argv else 'recovery_landed'
 stage=root/'build/fixture-land'
 write(stage/'landed.json',{'status':status,'strict_certified':False,'commit':commit,'review_sha256':arg('--review-sha256')})
 print(json.dumps({'status':status,'strict_certified':False,'commit':commit,'retained':str(stage)}))
'''


class QueueEntryTests(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory(); self.addCleanup(self.tmp.cleanup)
        self.base = Path(self.tmp.name).resolve(); self.repo = self.base / 'repo'; self.repo.mkdir()
        self.events = self.base / 'events.jsonl'; self.state = self.base / 'state'
        self.git('init', '-q'); self.git('config', 'user.name', 'fixture'); self.git('config', 'user.email', 'fixture@example.invalid')
        (self.repo / 'tools').mkdir(); (self.repo / 'src').mkdir()
        (self.repo / '.gitignore').write_text('build/\n')
        for name in ('receipts.py', 'land.py'):
            (self.repo / 'tools' / name).write_text('EVENTS=' + repr(str(self.events)) + '\n' + FIXTURE)
        (self.repo / 'src/owner.cpp').write_text('// prepared baseline\n')
        self.git('add', '.'); self.git('commit', '-qm', 'baseline'); self.baseline = self.git('rev-parse', 'HEAD')
        (self.repo / 'src/owner.cpp').write_text('// prepared recovery\n')
        self.git('add', '.'); self.git('commit', '-qm', 'candidate'); self.candidate = self.git('rev-parse', 'HEAD')
        self.git('checkout', '-q', self.baseline)
        inputs = {}
        for name in ('original_elf', 'checker', 'nss', 'targets', 'dwarfdump'):
            p = self.base / name; p.write_text('fixture ' + name)
            inputs[name] = {'path': str(p), 'sha256': queue.digest(p)}
        cache = self.base / 'lua-cache'; cache.mkdir()
        (cache / 'receipt.json').write_text('{"fixture":true}')
        self.job = dict(id='owner', baseline=self.baseline, candidate=self.candidate, mode='recovery-only',
                        profile='standard', owner='X/Owner.cpp', targets=inputs['targets'],
                        original_elf=inputs['original_elf'], checker=inputs['checker'], nss=inputs['nss'],
                        lua_cache=str(cache), lua_receipt_sha256=queue.digest(cache / 'receipt.json'),
                        output=str(self.repo / 'build/receipts'), seconds=30)
        self.inputs = inputs
        self.value = {'format': 1, 'kind': queue.KIND, 'repo': str(self.repo),
                      'queue_sha256': queue.digest(Path(queue.__file__)),
                      'tools': {str(p.relative_to(self.repo)): queue.digest(p) for p in (self.repo / 'tools').iterdir()},
                      'jobs': [self.job]}
        self.path = self.base / 'manifest.json'; self.write_manifest()
        self.review = self.base / 'review.json'; self.review.write_text(json.dumps({'decision': 'approve'}))

    def git(self, *args):
        return subprocess.check_output(['git', *args], cwd=self.repo, stderr=subprocess.PIPE, text=True).strip()

    def write_manifest(self):
        self.path.write_text(json.dumps(self.value, sort_keys=True))

    def run_queue(self, *args, mode='', expected=0):
        env = dict(os.environ, QUEUE_FIXTURE_MODE=mode, QUEUE_FIXTURE_LOCK_PATH=str(self.state / '.lock'))
        result = subprocess.run([sys.executable, queue.__file__, '--manifest', str(self.path),
                                 '--state', str(self.state), *args], env=env, text=True,
                                stdout=subprocess.PIPE, stderr=subprocess.PIPE, timeout=40)
        self.assertEqual(result.returncode, expected, result.stdout + result.stderr)
        return json.loads(result.stdout)

    def calls(self):
        return [json.loads(x) for x in self.events.read_text().splitlines()] if self.events.exists() else []

    def row(self):
        return json.loads((self.state / 'state.json').read_text())['jobs']['owner']

    def test_read_only_preflight_then_reviewed_landing(self):
        result = self.run_queue()
        self.assertEqual(result['writes'], 0); self.assertFalse(self.state.exists()); self.assertFalse(self.calls())
        self.assertEqual(self.run_queue('--run')['jobs']['owner'], 'awaiting_review')
        self.assertEqual(len(self.calls()), 1)
        self.assertEqual(self.run_queue('--run')['jobs']['owner'], 'awaiting_review')
        self.assertEqual(len(self.calls()), 1)
        result = self.run_queue('--run', '--job', 'owner', '--review', str(self.review),
                                '--review-sha256', queue.digest(self.review))
        self.assertEqual(result['jobs']['owner'], 'landed'); self.assertFalse(result['admission'])
        command = self.calls()[1]
        self.assertIn('--recovery-only', command); self.assertNotIn('--reuse-reviewed-baseline', command)
        self.assertEqual(self.git('rev-parse', 'HEAD'), self.row()['commit'])
        self.run_queue('--run'); self.assertEqual(len(self.calls()), 2)

    def test_failed_validation_is_terminal_and_does_not_land(self):
        self.assertEqual(self.run_queue('--run', mode='fail')['jobs']['owner'], 'failed')
        self.run_queue('--run'); self.assertEqual(len(self.calls()), 1)

    def test_timeout_and_surviving_running_state_are_uncertain(self):
        self.job['seconds'] = 1; self.write_manifest()
        self.assertEqual(self.run_queue('--run', mode='sleep')['jobs']['owner'], 'uncertain')
        self.run_queue('--run'); self.assertEqual(len(self.calls()), 1)
        state = json.loads((self.state / 'state.json').read_text()); state['jobs']['owner']['phase'] = 'validating'
        (self.state / 'state.json').write_text(json.dumps(state))
        self.assertEqual(self.run_queue()['jobs']['owner'], 'uncertain')
        self.assertEqual(json.loads((self.state / 'state.json').read_text())['jobs']['owner']['phase'], 'validating')
        self.assertEqual(self.run_queue('--run')['jobs']['owner'], 'uncertain')
        self.assertEqual(len(self.calls()), 1)

    def test_stale_baseline_parks_without_source_mutation(self):
        self.git('checkout', '-q', self.candidate)
        self.assertEqual(self.run_queue()['jobs']['owner'], 'needs_rebase')
        self.assertFalse(self.state.exists())
        self.assertEqual(self.run_queue('--run')['jobs']['owner'], 'needs_rebase')
        self.assertEqual(self.git('rev-parse', 'HEAD'), self.candidate); self.assertFalse(self.calls())

    def test_tool_input_and_manifest_drift_reject(self):
        self.run_queue('--run')
        original = (self.repo / 'tools/receipts.py').read_text()
        (self.repo / 'tools/receipts.py').write_text(original + '\n# drift\n')
        self.assertIn('Tool changed', self.run_queue('--run', expected=1)['error'])
        (self.repo / 'tools/receipts.py').write_text(original)
        self.job['seconds'] += 1; self.write_manifest()
        self.assertIn('state binding', self.run_queue('--run', expected=1)['error'])
        self.assertEqual(len(self.calls()), 1)

    def test_changed_artifacts_or_review_pin_do_not_dispatch(self):
        self.run_queue('--run')
        result = self.run_queue('--run', '--job', 'owner', '--review', str(self.review),
                                '--review-sha256', 'b' * 64, expected=1)
        self.assertIn('pin changed', result['error']); self.assertEqual(len(self.calls()), 1)
        (Path(self.job['output']) / 'evidence.json').write_text('{}')
        self.run_queue('--run', expected=1); self.assertEqual(len(self.calls()), 1)

    def test_existing_tool_rejects_supplied_bad_review_and_queue_never_retries(self):
        self.run_queue('--run'); self.review.write_text('{"decision":"reject"}')
        result = self.run_queue('--run', '--job', 'owner', '--review', str(self.review),
                                '--review-sha256', queue.digest(self.review))
        self.assertEqual(result['jobs']['owner'], 'uncertain')
        self.run_queue('--run'); self.assertEqual(len(self.calls()), 2)
        self.assertEqual(self.git('rev-parse', 'HEAD'), self.baseline)

    def test_wrong_source_binding_cannot_reach_review(self):
        self.assertEqual(self.run_queue('--run', mode='wrong-binding')['jobs']['owner'], 'failed')

    def test_stable_lock_and_symlink_rejection(self):
        self.state.mkdir()
        with (self.state / '.lock').open('a') as stream:
            fcntl.flock(stream, fcntl.LOCK_EX | fcntl.LOCK_NB)
            self.run_queue('--run', expected=1)
        self.assertFalse(self.calls())
        (self.state / '.lock').unlink(); (self.state / '.lock').symlink_to(self.path)
        self.run_queue('--run', expected=1); self.assertFalse(self.calls())

    def test_group_uses_separate_mode_and_rejects_writable_profile(self):
        p = self.base / 'owners.json'; p.write_text(json.dumps({'format': 1, 'owners': ['X/A.cpp', 'X/B.cpp']}))
        del self.job['owner']; self.job.update(mode='recovery-group', owners={'path': str(p), 'sha256': queue.digest(p)})
        self.write_manifest(); self.run_queue('--run')
        self.run_queue('--run', '--job', 'owner', '--review', str(self.review), '--review-sha256', queue.digest(self.review))
        self.assertIn('--recovery-group', self.calls()[1]); self.assertNotIn('--recovery-only', self.calls()[1])
        self.job['profile'] = 'named-writable'; self.job['dwarfdump'] = self.inputs['dwarfdump']; self.write_manifest()
        self.assertIn('unsupported', self.run_queue(expected=1)['error'])

    def test_named_profile_passes_pinned_reader_only_to_receipt_entry(self):
        self.job.update(profile='named-writable', dwarfdump=self.inputs['dwarfdump']); self.write_manifest()
        self.run_queue('--run')
        self.assertIn('--dwarfdump', self.calls()[0]); self.assertIn('--dwarfdump-sha256', self.calls()[0])

    def test_action_bound_and_no_premature_review(self):
        other = dict(self.job, id='second', output=str(self.repo / 'build/second-receipts'))
        self.value['jobs'].append(other); self.write_manifest()
        self.run_queue('--run', '--job', 'owner', '--review', str(self.review),
                       '--review-sha256', queue.digest(self.review), expected=1)
        self.assertFalse(self.calls())
        result = self.run_queue('--run')
        self.assertEqual(result['jobs'], {'owner': 'awaiting_review', 'second': 'prepared'})
        self.assertEqual(len(self.calls()), 1)
        result = self.run_queue('--run', '--max-actions', '2')
        self.assertEqual(result['jobs']['second'], 'awaiting_review')
        self.assertEqual(len(self.calls()), 2)

    def test_unknown_fields_and_path_aliases_fail_before_dispatch(self):
        self.job['reuse_reviewed_baseline'] = True; self.write_manifest()
        self.run_queue('--run', expected=1); self.assertFalse(self.calls())
        del self.job['reuse_reviewed_baseline']
        alias = self.base / 'alias'; alias.symlink_to(self.base / 'nss')
        self.job['nss'] = {'path': str(alias), 'sha256': queue.digest(alias)}; self.write_manifest()
        self.run_queue('--run', expected=1); self.assertFalse(self.calls())

    def test_realistic_nonruntime_gitlink_and_tracked_symlink_boundary(self):
        # Gitlinks point into another repository and need not have objects here.
        (self.repo / 'tools/common').mkdir()
        self.git('update-index', '--add', '--cacheinfo', '160000', 'b' * 40, 'tools/common')
        self.git('commit', '-qm', 'non-runtime common gitlink')
        self.job['baseline'] = self.git('rev-parse', 'HEAD')
        (self.repo / 'src/owner.cpp').write_text('// second prepared recovery\n')
        self.git('add', 'src/owner.cpp'); self.git('commit', '-qm', 'new candidate')
        self.job['candidate'] = self.git('rev-parse', 'HEAD'); self.git('checkout', '-q', self.job['baseline'])
        self.write_manifest(); self.run_queue(); self.run_queue('--run')
        self.assertEqual(self.row()['phase'], 'awaiting_review')
        (self.repo / 'tools/alias.py').symlink_to('receipts.py')
        self.git('add', 'tools/alias.py')
        self.assertIn('tool kind', self.run_queue(expected=1)['error'])

    def test_lua_receipt_bytes_and_lock_replacement_are_checked(self):
        receipt = Path(self.job['lua_cache']) / 'receipt.json'; original = receipt.read_bytes()
        receipt.write_bytes(b'changed')
        self.assertIn('pin changed', self.run_queue(expected=1)['error']); self.assertFalse(self.state.exists())
        receipt.write_bytes(original)
        self.assertIn('inode replaced', self.run_queue('--run', mode='replace-lock', expected=1)['error'])
        self.assertEqual(self.row()['phase'], 'validating')
        self.assertEqual(self.run_queue('--run')['jobs']['owner'], 'uncertain')
        self.assertEqual(len(self.calls()), 1)

    def test_state_cannot_write_inside_a_pinned_lua_cache(self):
        cache = Path(self.job['lua_cache'])
        before = {p.name: p.read_bytes() for p in cache.iterdir()}
        for directory in (cache, cache / 'queue-state'):
            self.state = directory
            for flags in ((), ('--run',)):
                with self.subTest(directory=directory, flags=flags):
                    result = self.run_queue(*flags, expected=1)
                    self.assertIn('protected tree', result['error'])
                    self.assertEqual({p.name: p.read_bytes() for p in cache.iterdir()}, before)
        self.assertFalse(self.calls())

    def test_reviewed_baseline_profile_is_explicit_single_only(self):
        self.job.update(owner='X/Owner.c', integration_profile='reuse-reviewed-baseline'); self.write_manifest()
        self.run_queue('--run')
        self.run_queue('--run', '--job', 'owner', '--review', str(self.review), '--review-sha256', queue.digest(self.review))
        self.assertIn('--reuse-reviewed-baseline', self.calls()[1])
        self.assertNotIn('--reuse-baseline-from', self.calls()[0])
        self.job['integration_profile'] = 'unknown'; self.write_manifest()
        self.run_queue(expected=1)
        self.job['integration_profile'] = 'reuse-reviewed-baseline'; self.job['mode'] = 'recovery-group'
        del self.job['owner']
        p = self.base / 'group.json'; p.write_text(json.dumps({'format': 1, 'owners': ['X/A.c', 'X/B.cc']}))
        self.job['owners'] = {'path': str(p), 'sha256': queue.digest(p)}; self.write_manifest()
        self.assertIn('integration profile', self.run_queue(expected=1)['error'])


if __name__ == '__main__':
    unittest.main()
