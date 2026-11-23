"""Recovery policy and receipt negatives; disposable fixtures, never real checkout writes."""
import copy
import json
from pathlib import Path
from unittest.mock import patch
import unittest

import recovery as q
import receipts as r
from validation import common as c
from test_receipts import ReceiptTests


def row(address='0x10', name='prior', status='mismatch', reason='unproven jump table: guard'):
    return dict(address=address,name=name,status=status,reason=reason)


class PolicyTests(unittest.TestCase):
    def test_named_data_requires_pinned_reader_and_complete_raw_witness(self):
        import recovery_named_data
        mapping = {'new_named_data_sections': [{'section': '.data'}]}
        roots = {'baseline': Path('/baseline'), 'candidate': Path('/candidate')}
        binding = {'owner': 'X/Owner.cpp', 'original_sha256': 'a'*64, 'nss_sha256': 'b'*64}
        with self.assertRaises(c.Halt):
            q.named_data_proof(mapping, {}, roots, binding, '/original', '/nss', '/object')
        binding['named_data_dwarfdump'] = {'path': '/reader', 'sha256': 'c'*64}
        witness = {'identity': [16, 'globalScalar']}
        with patch.object(q, 'named_data_reader', return_value=binding['named_data_dwarfdump']), \
                patch.object(recovery_named_data, 'prove', return_value=[witness]) as prove:
            self.assertEqual(q.named_data_proof(mapping, {}, roots, binding, '/original', '/nss', '/object'), [witness])
            self.assertEqual(prove.call_args.kwargs, {'original_sha256': 'a'*64, 'nss_sha256': 'b'*64,
                'dwarfdump_path': '/reader', 'dwarfdump_sha256': 'c'*64})
            self.assertEqual(prove.call_args.args[-2:], (roots['baseline'], roots['candidate']))
        with patch.object(q, 'named_data_reader', return_value=binding['named_data_dwarfdump']), \
                patch.object(recovery_named_data, 'prove', return_value=[]), self.assertRaises(c.Halt):
            q.named_data_proof(mapping, {}, roots, binding, '/original', '/nss', '/object')

    def compare(self,before,after,targets=None):
        return q.compare({'format':1,'results':before},{'format':1,'results':after},
                         targets or [{'address':'0x20','name':'target'}], {('0x20','target')})

    def test_same_failure_count_different_identity_rejected(self):
        a=[row(),row('0x20','target','mismatch','wrong function size')]
        b=[row('0x11'),a[1]]
        with self.assertRaises(c.Halt):self.compare(a,b)

    def test_same_count_different_diagnostic_rejected(self):
        a=[row(),row('0x20','target','mismatch','wrong function size')];b=copy.deepcopy(a)
        b[0]['reason']='unverified GOT reference'
        with self.assertRaises(c.Halt):self.compare(a,b)

    def test_target_failure_worsening_and_omission_rejected(self):
        a=[row(),row('0x20','target','mismatch','wrong function size')]
        for b in ([a[0]], [a[0],row('0x20','target','mismatch','unsupported indirect flow')],
                  [a[0],row('0x20','target','not-checkable','unknown function: target')]):
            with self.subTest(b=b),self.assertRaises(c.Halt):self.compare(a,b)

    def test_explicit_absent_target_recovered_without_credit(self):
        a=[row(),row('0x20','target','not-checkable','unknown function: target')]
        b=[row(),row('0x20','target','mismatch','wrong function size')]
        self.assertEqual(len(self.compare(a,b)),2)
        b[1]['status']='error'
        with self.assertRaises(c.Halt):self.compare(a,b)

    def test_ambiguous_target_not_waivable(self):
        a=[row(),row('0x20','target','not-checkable','ambiguous function')]
        b=[row(),row('0x20','target','mismatch','wrong function size')]
        with self.assertRaises(c.Halt):self.compare(a,b)

    def test_function_got_error_text_needs_raw_binding_proof(self):
        from validation.recovery_function_got import FunctionGOTEvidence, CALLER, BEFORE, AFTER, OWNER
        a = [row('0x000000710019cba0', CALLER, 'mismatch', BEFORE)]
        b = [row('0x000000710019cba0', CALLER, 'mismatch', AFTER)]
        evidence = FunctionGOTEvidence({}, Path('/missing-original'), Path('/missing-nss'), [], OWNER)
        for supplied in (None, evidence):
            with self.subTest(supplied=supplied), self.assertRaises(c.Halt):
                q.compare({'results':a}, {'results':b}, [], function_got_evidence=supplied)
        self.assertEqual(evidence.witnesses, [])

    def test_inventory_only_exact_selected_U_to_M(self):
        a=[{'Address':'0x10','Name':'prior','Quality':'O','Size':'4'},
           {'Address':'0x20','Name':'target','Quality':'U','Size':'4'}]
        targets=[{'address':'0x20','name':'target'}];b=copy.deepcopy(a);b[1]['Quality']='M'
        q.inventory(a,b,targets,True)
        for mutate in (lambda x:x[1].update(Quality='O'),lambda x:x[0].update(Quality='M'),lambda x:x[1].update(Size='8')):
            bad=copy.deepcopy(b);mutate(bad)
            with self.assertRaises(c.Halt):q.inventory(a,bad,targets,True)
        with self.assertRaises(c.Halt):q.inventory(a,b,targets,False)


class RecoveryReceipts(ReceiptTests):
    # Inherited strict tests deliberately remain inherited: their defaults must not change.
    def recovery_args(self):
        self.args.mode='recovery-only';self.args.owner='X/Owner.cpp'
        self.args.nss=self.root/'nss';self.args.nss.write_bytes(b'NSS fixture');self.args.nss_sha256=c.sha(self.args.nss)
        c.write(self.targets,{'format':1,'functions':[{'address':'0x10','name':'prior'},{'address':'0x20','name':'target'}]})

    def recovery_builder(self,*args,**kwargs):
        self.builder(*args,**kwargs)
        source,support,dest,mode=args[:4]
        results=[row('0x10','prior','ok',''),row('0x20','target','mismatch','wrong function size')]
        if 'baseline' in source.parts:results[1]=row('0x20','target','not-checkable','unknown function: target')
        import shutil
        (dest/'data/2.6.2').mkdir(parents=True,exist_ok=True)
        shutil.copy2(support/'data/2.6.2/functions.csv',dest/'data/2.6.2/functions.csv')
        c.write(dest/'result.json',{'format':1,'results':results})
        from test_recovery_mapping import fixture
        obj = dest/'objects/CMakeFiles/fixture.dir/src/X/Owner.cpp.obj'
        obj.parent.mkdir(parents=True, exist_ok=True)
        def names(symbols, sections, metadata):
            for i, symbol in enumerate(symbols):
                if symbol[0] in ('old','new'):
                    symbols[i] = (('prior' if symbol[0] == 'old' else 'target'), *symbol[1:])
        fixture(obj, 'baseline' not in source.parts, names)
        c.write(dest/'done.json',{'kind':'recovery-only-mode-v1','mode':mode,'full_check_executed':False,
                'files':{str(p.relative_to(dest)):c.sha(p) for p in dest.rglob('*') if p.is_file() and p.name!='done.json'}})

    def fake_audit(self,args,output,binding,evidence):
        data={'kind':q.KIND,'binding_sha256':c.sha(output/'binding.json'),'strict_certified':False,
              'debt':{mode:[row('0x20','target','not-checkable','unknown function: target')] for mode in ('normal','lua')},'debt_sha256':c.digest({mode:[row('0x20','target','not-checkable','unknown function: target')] for mode in ('normal','lua')}),'obligations':{'fixture':{'evidence_sha256':c.digest('fixture'),'evidence':'fixture'}}}
        import recovery_mapping
        for mode in ('normal','lua'):
            paths = [next((Path(evidence[label][mode]['root'])/'objects').rglob('Owner.cpp.obj'))
                     for label in ('baseline','candidate')]
            witness = recovery_mapping.compare(*paths, {'prior','target'})
            proof={'witnesses':[], 'transitions':[], 'nss':str(self.args.nss), 'nss_sha256':self.args.nss_sha256}
            data['obligations'][mode+':new-vtable-data-proof']={'evidence':proof,'evidence_sha256':c.digest(proof)}
            for label in ('baseline','candidate'):
                c.write(output/(mode+'-'+label+'-data.json'), [])
            for key, value in {'owner-mapping-symbols':witness, **recovery_mapping.extra_obligations(witness)}.items():
                data['obligations'][mode+':'+key] = {'evidence_sha256':c.digest(value),'evidence':value}
        c.write(output/'recovery-audit.json',data)
        c.write(output/'review-template.json',q.review_template(output,data))

    def recovery_build(self):
        self.recovery_args();self.args.run=True
        with patch.object(q,'audit',side_effect=self.fake_audit):return r.build(self.args,self.recovery_builder)

    def preliminary_callback_case(self, supported):
        # Exercise the real receipts._build preliminary comparison, before the
        # audit. Raw ELF/GOT authentication is tested separately on real inputs.
        from validation import recovery_function_got
        self.git('checkout', '--detach', self.baseline)
        for name, text in [('functions.csv','0x30,M,4,observer\n'),
                           ('function_files.csv','0x30,X/Observer.cpp,clang\n')]:
            path = self.repo/'data/2.6.2'/name
            path.write_text(path.read_text()+text)
        self.git('add','data');self.git('commit','-qm','unselected observer fixture')
        self.baseline = self.args.baseline = self.git('rev-parse','HEAD')
        (self.repo/'src/X/Owner.cpp').write_text('int prior() { return 1; }\nint target() { return 2; }\n')
        self.git('add','src');self.git('commit','-qm','candidate with observer fixture')
        self.candidate = self.args.candidate = self.git('rev-parse','HEAD')
        self.recovery_args();self.args.run = True
        instances = []
        test = self
        class Evidence:
            def __init__(self, roots, original, nss, selected, owner):
                test.assertEqual(set(roots), {'baseline','candidate'})
                test.assertEqual((original,nss,owner),(test.original,test.args.nss,'X/Owner.cpp'))
                test.assertEqual([(x['Name'],x['Size']) for x in selected],[('prior','4'),('target','4')])
                self.witnesses=[];instances.append(self)
            def equivalent(self, before, after):
                test.assertEqual((before['name'],after['name']),('observer','observer'))
                test.assertEqual((before['reason'],after['reason']),('unresolved callback','resolved function identity'))
                if supported:self.witnesses.append({'fixture':True})
                return supported
        def builder(*args, **kwargs):
            self.recovery_builder(*args, **kwargs)
            source,_,dest,_ = args[:4]
            result = c.read(dest/'result.json')
            result['results'].append(row('0x30','observer','mismatch',
                'unresolved callback' if 'baseline' in source.parts else 'resolved function identity'))
            c.write(dest/'result.json', result)
            done = c.read(dest/'done.json');done['files']['result.json']=c.sha(dest/'result.json')
            c.write(dest/'done.json',done)
        with patch.object(recovery_function_got,'FunctionGOTEvidence',Evidence), \
                patch.object(q,'audit',side_effect=self.fake_audit) as audit:
            if supported:
                result=r.build(self.args,builder)
                self.assertEqual(result['status'],'recovery_evidence_complete')
                audit.assert_called_once()
                self.assertEqual(len(instances),2)
                self.assertTrue(all(x.witnesses for x in instances))
            else:
                with self.assertRaisesRegex(c.Halt,'Unselected status or precise proof diagnostic changed'):
                    r.build(self.args,builder)
                audit.assert_not_called()

    def test_receipt_preliminary_comparison_supplies_raw_callback_evidence(self):
        self.preliminary_callback_case(True)

    def test_receipt_preliminary_comparison_rejects_unproven_callback_change(self):
        self.preliminary_callback_case(False)

    def test_recovery_mode_cannot_be_used_as_strict_receipt(self):
        result=self.recovery_build();self.assertEqual(result['status'],'recovery_evidence_complete')
        self.assertFalse(result['accepted']);r.verify(self.output,policy='recovery-only')
        with self.assertRaises(c.Halt):r.verify(self.output)
        entry=c.read(self.output/'run.json')['modes']['candidate_lua'];root=self.output/entry['root']
        with self.assertRaises(c.Halt):r.retained_mode(root)
        with self.assertRaises(c.Halt):r.retained_mode(root,'recovery-only','normal')

    def test_rehashed_invented_extension_obligations_rejected(self):
        self.recovery_build()
        original = c.read(self.output/'recovery-audit.json')
        for key, value in [('function-got-refinements', []),
                           ('function-got-refinements', [{'accepted':True}]),
                           ('new-scalar-data-proof', {'exact_complete':False}),
                           ('new-named-data-structure', []),
                           ('new-named-data-proof', {'witnesses':[], 'transitions':[]})]:
            audit = copy.deepcopy(original)
            audit['obligations']['normal:'+key] = {'evidence':value, 'evidence_sha256':c.digest(value)}
            c.write(self.output/'recovery-audit.json', audit)
            state = c.read(self.output/'run.json')
            state['audit_sha256'] = c.sha(self.output/'recovery-audit.json')
            with self.subTest(key=key,value=value), self.assertRaises(c.Halt):
                q.verify_audit(self.output, c.read(self.output/'binding.json'), state)

    def test_named_reader_pin_is_bound_before_build(self):
        self.recovery_args()
        reader = self.root/'reader'; reader.write_bytes(b'fixture executable')
        self.args.dwarfdump = reader
        for pin in (None, '0'*64):
            self.args.dwarfdump_sha256 = pin
            with self.subTest(pin=pin), self.assertRaises(c.Halt): q.configure(self.args, r.context(self.args))
        self.args.dwarfdump_sha256 = c.sha(reader)
        binding = r.context(self.args); q.configure(self.args, binding)
        self.assertEqual(binding['named_data_dwarfdump'], {'path':str(reader.resolve()), 'sha256':c.sha(reader)})

    def test_changed_named_reader_blocks_receipt_replay(self):
        reader = self.root/'reader'; reader.write_bytes(b'fixture executable')
        self.args.dwarfdump = reader; self.args.dwarfdump_sha256 = c.sha(reader)
        self.recovery_build()
        reader.write_bytes(b'changed executable')
        with self.assertRaisesRegex(c.Halt, 'DWARF reader changed'):
            r.verify(self.output, policy='recovery-only')

    def test_other_modes_reject_named_reader_before_admission(self):
        self.args.run = True
        for mode in ('strict', 'recovery-group'):
            for field, value in (('dwarfdump', Path('/reader')), ('dwarfdump_sha256', '0'*64)):
                self.args.mode = mode; self.args.dwarfdump = None; self.args.dwarfdump_sha256 = None
                setattr(self.args, field, value)
                with self.subTest(mode=mode, field=field), \
                        patch.object(r.resource_admission, 'lease', side_effect=AssertionError('no admission')), \
                        self.assertRaisesRegex(c.Halt, 'single-owner recovery'):
                    r.build(self.args, self.recovery_builder)
        self.assertFalse(self.output.exists())

    def test_forged_stable_call_witness_rejected_after_repinning(self):
        self.recovery_build()
        audit=c.read(self.output/'recovery-audit.json')
        audit['stable_call_failures']={'normal':[{'forged':'witness'}], 'lua':[]}
        c.write(self.output/'recovery-audit.json',audit)
        state=c.read(self.output/'run.json');state['audit_sha256']=c.sha(self.output/'recovery-audit.json')
        with self.assertRaisesRegex(c.Halt,'Stable call-failure witness changed or omitted'):
            q.verify_audit(self.output,c.read(self.output/'binding.json'),state)

    def test_recovery_review_is_explicit_pinned_and_complete(self):
        self.recovery_build();review=self.root/'review.json';value=c.read(self.output/'review-template.json');c.write(review,value)
        with self.assertRaises(c.Halt):q.review(self.output,review,c.sha(review))
        value.update(reviewer='Independent fixture reviewer',independent=True)
        for obligation in value['obligations'].values():
            obligation.update(decision='safe-uncertified',rationale='Fixture only',evidence_reference='fixture audit')
        c.write(review,value);q.review(self.output,review,c.sha(review))
        for field,new in [('blocking_findings',['unsafe ABI']),('audit_sha256','0'*64),('obligations',{})]:
            bad=copy.deepcopy(value);bad[field]=new;c.write(review,bad)
            with self.subTest(field=field),self.assertRaises(c.Halt):q.review(self.output,review,c.sha(review))
        c.write(review,value)
        with self.assertRaises(c.Halt):q.review(self.output,review,'0'*64)

    def test_template_cannot_remove_review_obligations(self):
        self.recovery_build()
        value=c.read(self.output/'review-template.json')
        value.update(reviewer='Independent fixture reviewer',independent=True,obligations={})
        c.write(self.output/'review-template.json',value)
        path=self.root/'review.json';c.write(path,value)
        with self.assertRaises(c.Halt):q.review(self.output,path,c.sha(path))

    def test_recovery_stale_inputs_checker_source_and_mode_fail_closed(self):
        self.recovery_build();self.args.resume=True
        for path in (self.original,self.checker,self.args.nss):
            original=path.read_bytes();path.write_bytes(original+b'changed')
            with self.subTest(path=path),self.assertRaises(c.Halt):r.build(self.args,self.recovery_builder)
            path.write_bytes(original)
        entry=c.read(self.output/'run.json')['modes']['candidate_normal'];root=self.output/entry['root']
        done=c.read(root/'done.json');done['mode']='lua';c.write(root/'done.json',done)
        with self.assertRaises(c.Halt):r.verify(self.output,policy='recovery-only')

    def test_recovery_scope_rejects_omitted_owner_and_O_promotion(self):
        self.recovery_args();c.write(self.targets,{'format':1,'functions':[{'address':'0x20','name':'target'}]})
        with self.assertRaises(c.Halt):r.build(self.args,self.recovery_builder)
        self.recovery_args()
        p=self.repo/'data/2.6.2/functions.csv';p.write_text(p.read_text().replace('0x20,U,','0x20,O,'))
        self.git('add','data');self.git('commit','-qm','forbidden O promotion');self.args.candidate=self.git('rev-parse','HEAD')
        self.args.post_landing=True
        with self.assertRaises(c.Halt):r.build(self.args,self.recovery_builder)

    def test_shared_inl_is_bound_in_receipt_and_source_snapshot(self):
        for name in ('src/X/shared\ninline.inl', 'include/shared.inl'):
            path = self.repo/name
            path.write_text('// shared inline fixture\n')
        self.git('add', 'src', 'include')
        self.git('commit', '-qm', 'shared inline fixture')
        self.args.candidate = self.git('rev-parse', 'HEAD')
        self.recovery_build()
        binding = c.read(self.output/'binding.json')
        for name in ('src/X/shared\ninline.inl', 'include/shared.inl'):
            self.assertIn(name, binding['changed_paths'])
            self.assertIn(name, binding['versions']['candidate']['source'])
            self.assertEqual((self.output/'candidate/source'/name).read_text(), '// shared inline fixture\n')
        r.verify(self.output, policy='recovery-only')

    def test_scope_rejects_other_inl_and_renamed_disallowed_source(self):
        self.recovery_args()
        path = self.repo/'toolchain/other.inl'
        path.write_text('// not shared source\n')
        self.git('add', 'toolchain/other.inl')
        self.git('commit', '-qm', 'disallowed inline fixture')
        self.args.candidate = self.git('rev-parse', 'HEAD')
        with self.assertRaisesRegex(c.Halt, 'Recovery scope excludes'):
            r.build(self.args, self.recovery_builder)
        self.git('reset', '--hard', self.candidate)
        self.git('mv', 'toolchain/fixture.cmake', 'src/X/renamed.inl')
        self.git('commit', '-qm', 'disallowed rename fixture')
        self.args.candidate = self.git('rev-parse', 'HEAD')
        with self.assertRaisesRegex(c.Halt, 'Recovery scope excludes'):
            r.build(self.args, self.recovery_builder)


if __name__=='__main__':unittest.main()
