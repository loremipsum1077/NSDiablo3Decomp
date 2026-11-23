"""Transaction-level grouped landing tests, using real Git and synthetic evidence."""
import shutil
from pathlib import Path
from types import SimpleNamespace
import unittest
from unittest.mock import patch

import claim
import receipts as r
import recovery_group as group
import recovery_group_land as land
import test_receipts
from validation import common as c


class GroupLandingTests(unittest.TestCase):
    def setUp(self):
        self.f = f = test_receipts.ReceiptTests()
        f.setUp()
        self.addCleanup(f.doCleanups)
        self.repo = f.repo
        f.git('config', 'user.name', 'Fixture')
        f.git('config', 'user.email', 'fixture@example.invalid')
        f.git('config', 'commit.gpgsign', 'false')
        f.git('checkout', '-q', '-B', 'main', f.baseline)
        (self.repo/'src/X/Other.cpp').write_text('int other() { return 0; }\n')
        (self.repo/'data/2.6.2/function_files.csv').write_text('Address,File,Compiler\n0x10,X/Owner.cpp,clang\n0x20,X/Other.cpp,clang\n')
        f.git('add', '.')
        f.git('commit', '--amend', '-qm', 'two-owner baseline')
        self.base = f.git('rev-parse', 'HEAD')
        (self.repo/'src/X/Owner.cpp').write_text('int prior() { return 2; }\n')
        (self.repo/'src/X/Other.cpp').write_text('int target() { return 2; }\n')
        (self.repo/'include/fixture.h').write_text('int target();\n')
        f.git('add', '.')
        f.git('commit', '-qm', 'reviewed candidate')
        self.candidate = f.git('rev-parse', 'HEAD')
        f.git('checkout', '-q', 'main')
        f.git('reset', '--hard', self.base)
        with (self.repo/'.git/info/exclude').open('a') as out:
            out.write('\n/build/\n/claims.csv\n')
        self.owners = ['X/Other.cpp', 'X/Owner.cpp']
        self.binding = {'policy':'recovery-group', 'owners':self.owners, 'post_landing':False,
            'partition':[{'owner':x,'functions':[]} for x in self.owners],
            'targets':{'format':1,'functions':[{'address':'0x10','name':'prior'}, {'address':'0x20','name':'target'}]},
            'versions':{'baseline':{'commit':self.base},'candidate':{'commit':self.candidate,'source':r.committed_files(self.repo,self.candidate,r.SOURCE)}},
            'images':{'normal':'n','lua':'l'}, 'original_sha256':'a'*64,'checker_sha256':'b'*64,
            'lua_receipt_sha256':'c'*64,'nss_sha256':'d'*64}
        f.output.mkdir()
        c.write(f.output/'binding.json',self.binding)
        roots = {}
        for label in ('baseline','candidate'):
            roots[label]={}
            for mode in ('normal','lua'):
                path=f.root/'review-artifacts'/label/mode
                (path/'objects').mkdir(parents=True)
                (path/'objects/unit.o').write_bytes((label+mode).encode())
                roots[label][mode]={'root':str(path)}
        c.write(f.output/'evidence.json', roots)
        self.review=f.root/'review.json'; self.review.write_text('{}')
        self.args=SimpleNamespace(recovery_receipts=f.output,files=list(self.owners),review=self.review,
            review_sha256=c.sha(self.review), original_elf=f.original,checker=f.checker,lua_cache=f.cache,nss=f.original,seconds=60)
        for name, value in [('MAIN',self.repo),('CLAIMS',self.repo/'claims.csv')]:
            p=patch.object(claim,name,value);p.start();self.addCleanup(p.stop)
        for owner in self.owners:
            path=claim.worktree(owner);path.parent.mkdir(parents=True,exist_ok=True)
            f.git('worktree','add','--detach',str(path),self.candidate)
        with claim.locked() as claims:
            for owner in self.owners:
                claims[owner]={'File':owner,'State':'done','Who':'fixture','When':'now','Note':''}
        self.after_build=lambda args:None
        self.build_count=0

    def run_land(self):
        expected = {k:v for k,v in self.binding.items() if k != 'images'}
        def build(args):
            self.build_count += 4
            evidence=c.read(self.f.output/'evidence.json')
            for label in ('baseline','candidate'):
                for mode in ('normal','lua'):
                    dest=args.output/label/mode
                    shutil.copytree(Path(evidence[label][mode]['root']),dest)
                    evidence[label][mode]['root']=str(dest)
            c.write(args.output/'evidence.json',evidence)
            self.after_build(args)
        fake_parser=SimpleNamespace(parse_args=lambda words:SimpleNamespace(candidate=self.candidate,post_landing=False,run=False,output=Path(words[words.index('--output')+1])))
        with patch.object(r,'parser',return_value=fake_parser), patch.object(r,'context',return_value=expected.copy()), \
             patch.object(group,'configure'), patch.object(group,'review',return_value={'reviewer':'fixture'}), \
             patch.object(r,'verify'), patch.object(r,'image_ids',return_value=self.binding['images']), patch.object(r,'build',side_effect=build):
            return land.land(self.args,self.repo)

    def test_atomic_success_retains_full_patch_and_changes_only_selected_inventory(self):
        result=self.run_land()
        self.assertEqual(result['status'],'recovery_group_landed')
        self.assertEqual(self.build_count,4)
        self.assertEqual(self.f.git('rev-parse','HEAD^'),self.base)
        self.assertEqual(self.f.git('diff','--name-only',self.candidate,'HEAD'),'data/2.6.2/functions.csv')
        self.assertEqual([x['Quality'] for x in c.rows(self.repo/'data/2.6.2/functions.csv')],['O','M'])
        stage=Path(result['retained'])
        self.assertTrue((stage/'reviewed.patch').is_file())
        self.assertEqual(set(c.read(stage/'transaction.json')['claims']),set(self.owners))
        self.assertTrue(all(claim.worktree(x).exists() for x in self.owners))

    def test_claim_race_blocks_both_without_landing(self):
        def mutate(_):
            with claim.locked() as claims:
                claims[self.owners[1]]['Who']='other'
        self.after_build=mutate
        with self.assertRaisesRegex(c.Halt,'Claim ownership changed'):
            self.run_land()
        self.assertEqual(self.f.git('rev-parse','HEAD'),self.base)
        self.assertTrue(all(claim.worktree(x).exists() for x in self.owners))
        self.assertFalse(list((self.repo/'build/recovery-land').glob('*/landed.json')))

    def test_missing_claim_and_wrong_membership_rejected(self):
        with claim.locked() as claims:
            del claims[self.owners[0]]
        with self.assertRaisesRegex(c.Halt,'Both reviewed owner claims'):
            self.run_land()
        self.assertEqual(self.build_count,0)
        self.args.files=[self.owners[1],self.owners[1]]
        with self.assertRaisesRegex(c.Halt,'exactly the reviewed'):
            self.run_land()

    def test_claim_worktree_race_blocks_landing(self):
        def mutate(_):
            (claim.worktree(self.owners[0])/'src/X/Other.cpp').write_text('// concurrent work\n')
        self.after_build = mutate
        with self.assertRaisesRegex(c.Halt, 'Claim source changed'):
            self.run_land()
        self.assertEqual(self.f.git('rev-parse', 'HEAD'), self.base)
        self.assertEqual((claim.worktree(self.owners[0])/'src/X/Other.cpp').read_text(), '// concurrent work\n')

    def test_primary_edit_during_build_is_preserved(self):
        source = self.repo/'src/X/Owner.cpp'
        self.after_build = lambda _: source.write_text('// concurrent primary work\n')
        with self.assertRaisesRegex(c.Halt, 'Primary HEAD/work changed'):
            self.run_land()
        self.assertEqual(source.read_text(), '// concurrent primary work\n')
        self.assertEqual(self.f.git('rev-parse', 'HEAD'), self.base)

    def test_build_failure_preserves_claims_and_integration(self):
        def fail(_):
            raise c.Halt('build failed')
        self.after_build = fail
        with self.assertRaisesRegex(c.Halt, 'build failed'):
            self.run_land()
        self.assertEqual(self.f.git('rev-parse', 'HEAD'), self.base)
        self.assertTrue(all(claim.worktree(x).exists() for x in self.owners))
        self.assertTrue(list((self.repo/'build/recovery-land').glob('*/transaction.json')))

    def test_changed_artifact_blocks_landing(self):
        self.after_build=lambda args: (args.output/'candidate/lua/objects/unit.o').write_bytes(b'changed')
        with self.assertRaisesRegex(c.Halt,'artifacts differ'):
            self.run_land()
        self.assertEqual(self.f.git('rev-parse','HEAD'),self.base)


if __name__ == '__main__':
    unittest.main()
