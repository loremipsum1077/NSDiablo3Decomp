"""Recovery landing transactions: real Git, fixture builds/audits, no private inputs."""
from pathlib import Path
import shutil
from types import SimpleNamespace
import unittest
from unittest.mock import patch

import claim
import receipts as r
import recovery as q
import test_recovery as fixtures
from test_recovery import row
from validation import common as c


class RecoveryLandTests(unittest.TestCase):
    def setUp(self):
        self.fixture = f = fixtures.RecoveryReceipts(methodName='test_recovery_review_is_explicit_pinned_and_complete')
        f.setUp()
        self.addCleanup(f.doCleanups)
        self.repo = f.repo
        f.git('config', 'user.name', 'Recovery transaction fixture')
        f.git('config', 'user.email', 'fixture@example.invalid')
        f.git('config', 'commit.gpgsign', 'false')
        # Local exclusions keep retained build/claim state out of source commits.
        with (self.repo/'.git/info/exclude').open('a') as stream:
            stream.write('\n/build/\n/claims.csv\n')
        f.git('checkout', '-q', '-B', 'fixture-main', f.baseline)
        f.recovery_args()
        f.args.run = True
        self.build_calls = []
        self.real_build = r.build
        with patch.object(q, 'audit', side_effect=f.fake_audit):
            self.real_build(f.args, self.builder)
        review = c.read(f.output/'review-template.json')
        review.update(reviewer='Independent transaction fixture', independent=True)
        for obligation in review['obligations'].values():
            obligation.update(decision='safe-uncertified', rationale='Synthetic transaction only',
                              evidence_reference='Fixture audit; no real acceptance')
        self.review = f.root/'review.json'
        c.write(self.review, review)
        self.args = SimpleNamespace(recovery_receipts=f.output, files=['X/Owner.cpp'],
            review=self.review, review_sha256=c.sha(self.review), original_elf=f.original,
            checker=f.checker, lua_cache=f.cache, nss=f.args.nss, seconds=60)
        self.patchers = [patch.object(claim, 'MAIN', self.repo),
                         patch.object(claim, 'CLAIMS', self.repo/'claims.csv')]
        for item in self.patchers:
            item.start()
            self.addCleanup(item.stop)

    def builder(self, source, support, dest, mode, *unused, **options):
        self.build_calls.append((str(source), mode))
        dest.mkdir(parents=True)
        build = dest/'build/2.6.2'
        build.mkdir(parents=True)
        (build/'d3').write_bytes(b'fixture ELF '+mode.encode())
        (build/'d3.map').write_bytes(b'fixture map')
        c.write(build/'d3.map.provenance', {'format':1, 'elf_sha256':c.sha(build/'d3'),
                                         'map_sha256':c.sha(build/'d3.map')})
        obj = dest/'objects/CMakeFiles/fixture.dir/src/X/Owner.cpp.obj'
        obj.parent.mkdir(parents=True)
        from test_recovery_mapping import fixture
        def names(symbols, sections, metadata):
            for i, symbol in enumerate(symbols):
                if symbol[0] in ('old','new'):
                    symbols[i] = (('prior' if symbol[0] == 'old' else 'target'), *symbol[1:])
        fixture(obj, 'baseline' not in source.parts, names)
        command = ['docker','run','--rm','--pull','never','--mount',
                   'type=bind,src='+str(source)+',dst=/work,readonly',options['images'][mode]]
        c.write(dest/'configure.command.json',command+['cmake','-G','Ninja',
            '-DD3_LUA_COMPILER='+('ON' if mode=='lua' else 'OFF'),'-B','build/2.6.2'])
        c.write(dest/'build.command.json',command+['cmake','--build','build/2.6.2','--','-j2'])
        table = dest/'data/2.6.2/functions.csv'
        table.parent.mkdir(parents=True)
        shutil.copy2(support/'data/2.6.2/functions.csv',table)
        target = row('0x20','target','mismatch','wrong function size')
        if 'baseline' in source.parts:
            target = row('0x20','target','not-checkable','unknown function: target')
        c.write(dest/'result.json',{'format':1,'results':[row('0x10','prior','ok',''),target]})
        c.write(dest/'done.json',{'kind':'recovery-only-mode-v1','mode':mode,
            'full_check_executed':False,'files':{str(p.relative_to(dest)):c.sha(p)
            for p in dest.rglob('*') if p.is_file()}})

    def land(self, after_build=None):
        def build(args):
            answer = self.real_build(args, self.builder)
            if after_build:
                after_build(args)
            return answer
        with patch.object(q,'audit',side_effect=self.fixture.fake_audit), patch.object(r,'build',side_effect=build):
            return q.land(self.args,self.repo)

    def assert_main_unchanged(self):
        self.assertEqual(self.fixture.git('rev-parse','HEAD'),self.fixture.baseline)
        self.assertEqual(q.rows(self.repo,self.fixture.baseline),
                         c.rows(self.repo/'data/2.6.2/functions.csv'))
        self.assertNotIn('target()', (self.repo/'src/X/Owner.cpp').read_text())

    def test_success_real_worktree_commit_and_fast_forward_only_U_to_M(self):
        answer = self.land()
        self.assertEqual(answer['status'],'recovery_landed')
        self.assertFalse(answer['strict_certified'])
        self.assertEqual(self.fixture.git('rev-parse','HEAD'),answer['commit'])
        self.assertEqual(self.fixture.git('rev-parse','HEAD^'),self.fixture.baseline)
        table = c.rows(self.repo/'data/2.6.2/functions.csv')
        self.assertEqual([x['Quality'] for x in table],['O','M'])
        q.inventory(q.rows(self.repo,self.fixture.baseline),table,
                    [{'address':'0x10','name':'prior'},{'address':'0x20','name':'target'}],True)
        self.assertEqual(r.committed_files(self.repo,answer['commit'],r.SOURCE),
                         r.committed_files(self.repo,self.fixture.candidate,r.SOURCE))
        retained = Path(answer['retained'])
        self.assertTrue((retained/'integration/.git').is_file())
        self.assertTrue((retained/'landed.json').is_file())
        self.assertEqual(len(self.build_calls),8)
        self.assertEqual(self.fixture.git('status','--porcelain'),'')
        r.verify(retained/'post-receipts',self.repo,answer['commit'],policy='recovery-only')
        with self.assertRaises(c.Halt):
            r.verify(retained/'post-receipts')

    def test_dirty_main_blocks_before_any_integration(self):
        source=self.repo/'src/X/Owner.cpp'
        source.write_text(source.read_text()+'// user edit\n')
        with self.assertRaisesRegex(c.Halt,'Primary HEAD/work changed'):
            self.land()
        self.assertIn('// user edit',source.read_text())
        self.assertEqual(len(self.build_calls),4)

    def test_reviewed_baseline_reuse_lands_with_exactly_two_fresh_candidate_builds(self):
        self.args.reuse_reviewed_baseline = True
        answer = self.land()
        self.assertEqual((answer['fresh_integration_builds'], answer['reused_baseline_builds']), (2, 2))
        self.assertEqual(len(self.build_calls), 6)
        self.assertTrue(all('/candidate/' in path for path, mode in self.build_calls[4:]))
        stage = Path(answer['retained']); binding = c.read(stage/'post-receipts/binding.json')
        self.assertEqual(binding['baseline_reference']['output'], str(self.fixture.output.resolve()))
        self.assertEqual(c.read(stage/'landed.json')['fresh_integration_builds'], 2)
        self.assertEqual(self.fixture.git('rev-parse', 'HEAD'), answer['commit'])
        self.assertEqual([x['Quality'] for x in c.rows(self.repo/'data/2.6.2/functions.csv')], ['O','M'])
        r.verify(stage/'post-receipts', self.repo, answer['commit'], policy='recovery-only')

    def test_reused_baseline_still_rejects_resealed_different_candidate_artifacts(self):
        self.args.reuse_reviewed_baseline = True
        def changed(args):
            output = args.output; evidence = c.read(output/'evidence.json')
            root = Path(evidence['candidate']['lua']['root'])
            obj = next((root/'objects').rglob('*.obj'))
            obj.write_bytes(obj.read_bytes().replace(b'CODE', b'DIFF'))
            done = c.read(root/'done.json'); done['files'][str(obj.relative_to(root))] = c.sha(obj)
            c.write(root/'done.json', done)
            state = c.read(output/'run.json')
            state['modes']['candidate_lua']['receipt_sha256'] = c.sha(root/'done.json')
            evidence['candidate']['lua']['receipt_sha256'] = c.sha(root/'done.json')
            c.write(output/'evidence.json', evidence)
            self.fixture.fake_audit(args, output, c.read(output/'binding.json'), evidence)
            state.update(evidence_sha256=c.sha(output/'evidence.json'),
                         audit_sha256=c.sha(output/'recovery-audit.json'))
            c.write(output/'run.json', state)
        with self.assertRaisesRegex(c.Halt, 'Fresh integration artifacts differ'):
            self.land(changed)
        self.assert_main_unchanged()
        self.assertEqual(len(self.build_calls), 6)
        self.assertFalse(list((self.repo/'build/recovery-land').glob('*/landed.json')))

    def test_post_build_dirty_edit_is_preserved(self):
        def change(_):
            (self.repo/'src/X/Owner.cpp').write_text('// new user edit\n')
        with self.assertRaisesRegex(c.Halt,'Primary HEAD/work changed'):
            self.land(change)
        self.assertEqual(self.fixture.git('rev-parse','HEAD'),self.fixture.baseline)
        self.assertEqual((self.repo/'src/X/Owner.cpp').read_text(),'// new user edit\n')
        self.assertTrue(list((self.repo/'build/recovery-land').glob('*/transaction.json')))

    def test_post_build_base_advance_is_preserved(self):
        head=[]
        def advance(_):
            self.fixture.git('commit','--allow-empty','-qm','concurrent work')
            head.append(self.fixture.git('rev-parse','HEAD'))
        with self.assertRaisesRegex(c.Halt,'Primary HEAD/work changed'):
            self.land(advance)
        self.assertEqual(self.fixture.git('rev-parse','HEAD'),head[0])
        self.assertNotIn('target()', (self.repo/'src/X/Owner.cpp').read_text())

    def test_post_build_same_commit_branch_switch_is_preserved(self):
        def switch(_):
            self.fixture.git('checkout','-q','-b','other-user-branch')
        with self.assertRaisesRegex(c.Halt,'Primary HEAD/work changed'):
            self.land(switch)
        self.assert_main_unchanged()
        self.assertEqual(self.fixture.git('branch','--show-current'),'other-user-branch')

    def test_failed_post_receipt_keeps_main_and_retains_integration(self):
        def corrupt(args):
            evidence=c.read(args.output/'evidence.json')
            Path(evidence['candidate']['lua']['root'],'result.json').write_text('{}')
        with self.assertRaises(c.Halt):
            self.land(corrupt)
        self.assert_main_unchanged()
        self.assertTrue(list((self.repo/'build/recovery-land').glob('*/transaction.json')))
        self.assertFalse(list((self.repo/'build/recovery-land').glob('*/landed.json')))

    def test_new_conflicting_claim_during_build_blocks(self):
        def claim_owner(_):
            with claim.locked() as claims:
                claims['X/Owner.cpp']={'File':'X/Owner.cpp','State':'claimed','Who':'other',
                                      'When':'fixture','Note':''}
        # A real worktree exists, but its source is the unreviewed baseline.
        other=self.fixture.root/'other-claim'
        self.fixture.git('worktree','add','--detach',str(other),self.fixture.baseline)
        with patch.object(claim,'worktree',return_value=other):
            with self.assertRaisesRegex(c.Halt,'Claim.*changed'):
                self.land(claim_owner)
        self.assert_main_unchanged()
        self.assertEqual(claim.load()['X/Owner.cpp']['Who'],'other')

    def test_existing_claim_removed_during_build_blocks(self):
        candidate_claim=self.fixture.root/'reviewed-claim'
        self.fixture.git('worktree','add','--detach',str(candidate_claim),self.fixture.candidate)
        with claim.locked() as claims:
            claims['X/Owner.cpp']={'File':'X/Owner.cpp','State':'claimed','Who':'reviewed',
                                  'When':'fixture','Note':''}
        def remove_claim(_):
            with claim.locked() as claims:
                del claims['X/Owner.cpp']
        with patch.object(claim,'worktree',return_value=candidate_claim):
            with self.assertRaisesRegex(c.Halt,'Claim.*changed'):
                self.land(remove_claim)
        self.assert_main_unchanged()
        self.assertNotIn('X/Owner.cpp',claim.load())

    def test_wrong_policy_rejected_before_worktree_creation(self):
        binding=c.read(self.fixture.output/'binding.json')
        binding['policy']='strict'
        c.write(self.fixture.output/'binding.json',binding)
        with self.assertRaisesRegex(c.Halt,'Pre-landing recovery receipts required'):
            self.land()
        self.assert_main_unchanged()
        self.assertEqual(len(self.build_calls),4)


if __name__=='__main__':
    unittest.main()
