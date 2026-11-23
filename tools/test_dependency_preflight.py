import unittest
import copy
import hashlib
import json
from pathlib import Path
import struct
import tempfile
from validation.elf import Elf
from recovery_mapping import inspect as inspect_object
from dependency_preflight import run
from dependency_preflight import slot_report


class SlotTests(unittest.TestCase):
    def setUp(self):
        self.symbol = {'name':'f','address':0x40,'size':8,'binding':1,'visibility':0,'type':2,'defined':True}
        self.entry = {'symbol':{'name':'f'},'original_target':0x40}

    def evaluate(self, original=None, baseline=None, candidate=None):
        return slot_report(0,self.entry, [self.symbol] if original is None else original,
                           [] if baseline is None else baseline, [] if candidate is None else candidate, [])

    def test_missing_is_not_success(self):
        result = self.evaluate()
        self.assertIn('provider_missing',result['reasons'])
        self.assertFalse(result['semantic_equivalence'])
        self.assertFalse(result['provider_selection_proven'])

    def test_extent_mismatch_reported_even_if_other_debts(self):
        result = self.evaluate(baseline=[dict(self.symbol,size=12)])
        self.assertIn('baseline_extent_differs',result['reasons'])
        self.assertFalse(result['baseline_extent_compatible'])

    def test_weak_copy_pending_not_accepted(self):
        copy={'name':'f','section':'.text.f','offset':0,'size':8,'binding':2,'other':0,'type':2}
        result=self.evaluate(candidate=[copy])
        self.assertIn('candidate_provider_selection_pending',result['reasons'])
        self.assertNotIn('provider_missing',result['reasons'])
        self.assertFalse(result['provider_selection_proven'])

    def test_duplicate_and_alias_ambiguities(self):
        other=dict(self.symbol,name='g',size=12)
        result=self.evaluate(original=[self.symbol,other], baseline=[self.symbol,dict(self.symbol,address=0x90)])
        self.assertIn('original_target_extent_ambiguous',result['reasons'])
        self.assertIn('baseline_provider_ambiguous',result['reasons'])
        result=self.evaluate(original=[dict(self.symbol,name='g')])
        self.assertIn('original_candidate_alias_unresolved',result['reasons'])

    def test_unsupported_nonfunction_and_candidate_extent(self):
        copy={'name':'f','section':'.text.f','offset':0,'size':12,'binding':2,'other':0,'type':2}
        wrong=dict(copy,type=1)
        result=self.evaluate(baseline=[dict(self.symbol,type=1)],candidate=[copy,wrong])
        self.assertIn('candidate_symbol_metadata_unsupported',result['reasons'])
        self.assertIn('baseline_symbol_metadata_unsupported',result['reasons'])
        self.assertIn('candidate_extent_differs',result['reasons'])
        self.assertIn('candidate_provider_selection_pending',result['reasons'])

    def test_matching_extent_does_not_prove_equivalence(self):
        result=self.evaluate(baseline=[self.symbol])
        self.assertTrue(result['baseline_extent_compatible'])
        self.assertFalse(result['semantic_equivalence'])
        self.assertFalse(result['provider_selection_proven'])

def retained_request(start, name):
    start = Path(start).resolve()
    for root in (start, *start.parents):
        path = root/'build/game-recovery-ui-actor-v5/preflight'/f'{name}-request.json'
        if path.is_file():
            return path
    return None


class RetainedEvidenceTests(unittest.TestCase):
    def request(self, name):
        path = retained_request(__file__, name)
        if path is None:
            self.skipTest('retained local evidence unavailable')
        return json.loads(path.read_text())

    def test_request_discovery_from_worktree_and_primary(self):
        worktree = retained_request(__file__, 'attempt02')
        if worktree is None:
            self.skipTest('retained local evidence unavailable')
        primary = worktree.parents[3]
        self.assertEqual(worktree, retained_request(primary/'tools'/'test_dependency_preflight.py', 'attempt02'))

    def test_both_retained_objects_report_expected_debts(self):
        for name,missing,pending in [('attempt02',11,7),('closure',0,18)]:
            result=run(self.request(name))
            self.assertEqual(len(result['slots']),39)
            self.assertEqual(result['summary'].get('provider_missing',0),missing)
            self.assertEqual(result['summary']['candidate_provider_selection_pending'],pending)
            self.assertEqual(result['summary']['baseline_extent_differs'],1)
            self.assertEqual(result['status'],'diagnostic_only')
            self.assertFalse(result['accepted'])

    def test_mutated_relocation_expression_is_rejected(self):
        request=self.request('attempt02')
        source=Path(request['inputs']['owner']['path'])
        snapshot=inspect_object(source)
        relocation=next(r for r in snapshot['relocations'] if r['section']=='.data.rel.ro')
        elf=Elf(source)
        data=bytearray(elf.d)
        section=elf.sections[relocation['table_index']]
        at=section[4]+8
        info=struct.unpack_from('<Q',data,at)[0]
        struct.pack_into('<Q',data,at,(info & ~0xffffffff)|258)
        with tempfile.TemporaryDirectory() as tmp:
            path=Path(tmp)/'changed.o'
            path.write_bytes(data)
            request=copy.deepcopy(request)
            request['inputs']['owner']={'path':str(path),'sha256':hashlib.sha256(data).hexdigest()}
            with self.assertRaisesRegex(Exception,'unsupported vtable pointer expression'):
                run(request)

if __name__ == '__main__':
    unittest.main()
