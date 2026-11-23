import unittest

import helper_audit as audit


class HelperAuditTests(unittest.TestCase):
    def test_exact_contexts_nonmatches_unknowns_and_overlaps(self):
        def record(name):
            return dict(name=name, scope=[], params=[], ret='void', const=False)
        keys = [('X/Shared.h', 1, 'Shared'), ('X/Owner.cpp', 2, 'Local')]
        index = dict(funcs={k: record(k[2]) for k in keys},
                     inlined={keys[0]: ({1, 2, 3, 9}, 5), keys[1]: ({2, 3}, 2)})
        functions = [dict(Address=hex(a), Name='f' + str(a), Size=str(a * 4), Quality=q)
                     for a, q in [(1, 'O'), (2, 'M'), (3, 'm'), (4, 'U')]]
        owners = [dict(Address=hex(a), File='X/Caller.cpp') for a in range(1, 5)]
        result = audit.summarize(index, functions, owners, {'src/X/Shared.h'})
        self.assertEqual(result['inventory']['exact'], 1)
        self.assertEqual(result['inventory']['exact_bytes'], 4)
        self.assertEqual(result['summary']['distinct_pending_inventory_callers'], 2)
        self.assertEqual(result['summary']['families_previously_supported_only_by_accepted_nonmatches'], 1)
        self.assertEqual(result['unproved'][0]['name'], 'Local')
        row = result['exact_contexts'][0]
        self.assertEqual((row['exact_callers'], row['pending_callers'], row['unknown_callers']), (1, 2, 1))
        self.assertEqual(row['exact_examples'][0]['source_job'], audit.project_plan.job_id('X/Caller.cpp'))
        self.assertIsNone(row['unknown_examples'][0]['quality'])
        self.assertTrue(row['source_present'])
        self.assertFalse(result['unproved'][0]['source_present'])

    def test_missing_definitions_and_deterministic_ties(self):
        rec = dict(name='f', scope=[], params=[], ret='void', const=False)
        a, b, missing = ('A.h', 1, 'f'), ('B.h', 1, 'f'), ('C.h', 1, 'f')
        index = dict(funcs={b: rec, a: rec}, inlined={b: ({1}, 1), missing: ({1}, 1), a: ({1}, 1)})
        functions = [dict(Address='0x1', Name='f', Size='4', Quality='W')]
        result = audit.summarize(index, functions, [], set(), top=1)
        self.assertEqual(result['unproved'][0]['owner'], 'A.h')
        self.assertEqual(result['summary']['skipped_missing_definition_records'], 1)
        self.assertEqual(result['summary']['distinct_pending_inventory_callers'], 1)
        self.assertIsNone(result['unproved'][0]['pending_examples'][0]['source_job'])


if __name__ == '__main__':
    unittest.main()
