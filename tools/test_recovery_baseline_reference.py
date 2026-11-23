"""Real receipt entry paths with disposable builders; no Docker or live writes."""
import copy
import unittest
from unittest.mock import patch
import test_recovery
import receipts as r
import recovery as q
import recovery_baseline_reference as ref
from validation import common as c


class BaselineReferenceTests(unittest.TestCase):
    def setUp(self):
        self.f = test_recovery.RecoveryReceipts('test_recovery_review_is_explicit_pinned_and_complete')
        self.f.setUp(); self.addCleanup(self.f.doCleanups)
        self.f.recovery_build()
        self.reviewed = self.f.output
        self.old_binding = c.read(self.reviewed/'binding.json')
        self.original_commands = {(mode, name): (self.reviewed/'baseline'/mode/'attempt-0001/receipt'/name).read_bytes()
            for mode in ('normal', 'lua') for name in ('configure.command.json', 'build.command.json')}
        table = self.f.repo/'data/2.6.2/functions.csv'
        table.write_text(table.read_text().replace('0x20,U,', '0x20,M,'))
        self.f.git('add', 'data/2.6.2/functions.csv'); self.f.git('commit', '-qm', 'integrated quality fixture')
        self.f.args.candidate = self.f.git('rev-parse', 'HEAD')
        self.f.args.post_landing = True
        self.f.args.reuse_baseline_from = self.reviewed
        self.f.output = self.f.root/'post receipts'; self.f.args.output = self.f.output
        self.f.calls.clear()

    def build(self):
        with patch.object(q, 'audit', side_effect=self.f.fake_audit):
            return r.build(self.f.args, self.f.recovery_builder)

    def test_only_candidate_modes_build_and_old_commands_remain_untouched(self):
        result = self.build()
        self.assertEqual((result['fresh_build_count'], result['reused_baseline_count']), (2, 2))
        self.assertEqual([x[2] for x in self.f.calls], ['normal', 'lua'])
        self.assertTrue(all('candidate' in x[0].parts for x in self.f.calls))
        state = c.read(self.f.output/'run.json')
        self.assertEqual(state['modes']['baseline_normal']['kind'], ref.KIND)
        self.assertFalse((self.f.output/'baseline/normal').exists())
        for (mode, name), content in self.original_commands.items():
            self.assertEqual((self.reviewed/'baseline'/mode/'attempt-0001/receipt'/name).read_bytes(), content)
        r.verify(self.f.output, policy='recovery-only')

    def test_reference_contract_rejects_policy_inputs_scope_source_and_images(self):
        self.build(); current = c.read(self.f.output/'binding.json')
        mutations = [lambda x:x.update(post_landing=False), lambda x:x.update(owner='Other.cpp'),
            lambda x:x.update(original_sha256='0'*64), lambda x:x.update(targets={'functions':[]}),
            lambda x:x['images'].update(lua='sha256:'+'0'*64),
            lambda x:x['versions']['baseline'].update(commit='0'*40),
            lambda x:x['versions']['candidate'].update(source={}),
            lambda x:x.update(recovery_tools={}), lambda x:x.update(unrecognized_policy='new')]
        for change in mutations:
            bad = copy.deepcopy(current); change(bad)
            with self.subTest(change=change), self.assertRaises(c.Halt): ref.contract(bad, self.old_binding)
        old = copy.deepcopy(self.old_binding); old['baseline_reference'] = {}
        with self.assertRaises(c.Halt): ref.contract(current, old)

    def test_referenced_artifact_mutation_blocks_replay_and_resume_before_build(self):
        self.build()
        root = self.reviewed/'baseline/normal/attempt-0001/receipt'
        next((root/'objects').rglob('*.obj')).write_bytes(b'changed artifact')
        with self.assertRaises(c.Halt): r.verify(self.f.output, policy='recovery-only')
        self.f.args.resume = True
        with self.assertRaises(c.Halt):
            r.build(self.f.args, lambda *a, **kw: self.fail('corrupt reference launched builder'))

    def test_candidate_reference_and_changed_metadata_are_rejected(self):
        self.build(); binding = c.read(self.f.output/'binding.json')
        entry = c.read(self.f.output/'run.json')['modes']['baseline_normal']
        with self.assertRaises(c.Halt): r.checkpoint(self.f.output, binding, '', 'candidate', 'normal', entry)
        state = c.read(self.reviewed/'run.json'); state['status'] = 'in_progress'
        c.write(self.reviewed/'run.json', state)
        with self.assertRaises(c.Halt): ref.checkpoint(binding, 'baseline', 'normal', entry)

    def test_cross_mode_reference_and_nested_output_are_rejected(self):
        self.build(); binding = c.read(self.f.output/'binding.json')
        entry = c.read(self.f.output/'run.json')['modes']['baseline_normal']
        with self.assertRaises(c.Halt): ref.checkpoint(binding, 'baseline', 'lua', entry)
        self.f.args.output = self.reviewed/'nested'
        with self.assertRaises(c.Halt): ref.configure(self.f.args, copy.deepcopy(binding))

    def test_resume_builds_only_missing_candidate_mode(self):
        def fail_lua(source, support, dest, mode, *args, **kwargs):
            if mode == 'lua': raise RuntimeError('fixture interrupted candidate')
            return self.f.recovery_builder(source, support, dest, mode, *args, **kwargs)
        with patch.object(q, 'audit', side_effect=self.f.fake_audit), self.assertRaises(RuntimeError):
            r.build(self.f.args, fail_lua)
        state = c.read(self.f.output/'run.json')
        self.assertEqual(set(state['modes']), {'baseline_normal', 'baseline_lua', 'candidate_normal'})
        self.f.args.resume = True
        answer = self.build()
        self.assertEqual((answer['fresh_build_count'], answer['reused_baseline_count']), (1, 2))
        self.assertEqual([call[2] for call in self.f.calls], ['normal', 'lua'])
        self.assertTrue((self.f.output/'candidate/lua/attempt-0001').exists())
        self.assertTrue((self.f.output/'candidate/lua/attempt-0002/receipt').exists())
        r.verify(self.f.output, policy='recovery-only')

    def test_prelanding_and_group_reuse_are_rejected_without_build(self):
        self.f.args.post_landing = False
        with self.assertRaises(c.Halt): self.build()
        self.f.args.mode = 'recovery-group'
        with self.assertRaises(c.Halt): self.build()
        self.assertEqual(self.f.calls, [])


if __name__ == '__main__':
    unittest.main()
