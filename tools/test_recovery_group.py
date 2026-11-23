"""Group partition and receipt dispatch contracts, without game builds."""
import unittest
import tempfile
import shutil
import copy
from types import SimpleNamespace
from pathlib import Path
from unittest.mock import patch

import recovery_group as group
import receipts
from validation import common as c
import test_recovery


class PartitionTests(unittest.TestCase):
    def setUp(self):
        self.owners = ['a/X.cpp', 'b/X.cpp']
        self.functions = [{'Address': '0x10', 'Name': 'one'}, {'Address': '0x20', 'Name': 'two'}]
        self.files = [{'Address': '0x10', 'File': self.owners[0]}, {'Address': '0x20', 'File': self.owners[1]}]
        self.targets = [{'address': '0x10', 'name': 'one'}, {'address': '0x20', 'name': 'two'}]

    def test_complete_partition_and_same_basename(self):
        parts = group.partition(self.owners, self.targets, self.functions, self.files)
        self.assertEqual([p['functions'] for p in parts], [[self.targets[0]], [self.targets[1]]])

    def test_reject_incomplete_duplicate_empty_or_ambiguous(self):
        cases = [
            (self.owners, self.targets[:1], self.functions, self.files),
            ([self.owners[0], self.owners[0]], self.targets, self.functions, self.files),
            (['a/X.cpp', 'c/X.cpp'], self.targets, self.functions, self.files),
            (self.owners, self.targets, self.functions, self.files + [self.files[0]]),
            (self.owners, self.targets, self.functions[:1], self.files),
            (self.owners, self.targets, self.functions + [self.functions[0]], self.files),
            (self.owners[::-1], self.targets, self.functions, self.files),
            (['../X.cpp', 'b/X.cpp'], self.targets, self.functions, self.files),
        ]
        for case in cases:
            with self.subTest(case=case), self.assertRaises(c.Halt):
                group.partition(*case)

    def test_qualified_and_duplicate_names_rejected(self):
        for name in ('one', 'two@file'):
            functions = [self.functions[0], {'Address': '0x20', 'Name': name}]
            targets = [self.targets[0], {'address': '0x20', 'name': name}]
            with self.assertRaises(c.Halt):
                group.partition(self.owners, targets, functions, self.files)


class ObjectRoutingTests(unittest.TestCase):
    def test_exact_owner_suffixes_and_unrelated_changes(self):
        with tempfile.TemporaryDirectory() as temp:
            roots = {label: Path(temp)/label for label in ('baseline', 'candidate')}
            def write(label, name, data=b'unchanged'):
                path = roots[label]/'objects'/name
                path.parent.mkdir(parents=True, exist_ok=True)
                path.write_bytes(data)
                return path
            owners = ['a/X.cpp', 'b/X.cpp']
            for label in roots:
                for owner in owners:
                    write(label, 'CMakeFiles/unit.dir/src/'+owner+'.obj')
                write(label, 'CMakeFiles/unit.dir/src/other.cpp.obj')
            routed = group.route_objects(roots, owners)
            self.assertEqual(len(routed[owners[0]]['candidate']), 1)
            changed = write('candidate', 'CMakeFiles/unit.dir/src/other.cpp.obj', b'changed')
            with patch('validation.recovery_diagnostics.runtime_digest', side_effect=lambda p: p.read_bytes()):
                with self.assertRaises(c.Halt):
                    group.route_objects(roots, owners)
            changed.write_bytes(b'unchanged')
            write('candidate', 'CMakeFiles/duplicate.dir/src/a/X.cpp.obj')
            with self.assertRaises(c.Halt):
                group.route_objects(roots, owners)


class AuditReplayTests(unittest.TestCase):
    """Run the real audit and replay on small synthetic ELF object files."""
    def setUp(self):
        from test_recovery_mapping import fixture
        from validation import strict
        import contributor_adapter
        import validate_data
        import validation.unwind as unwind
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)
        self.output = self.root/'output'
        self.output.mkdir()
        self.owners = ['a/X.cpp', 'b/X.cpp']
        self.rows = [{'Address': '0x10', 'Name': 'one', 'Quality': 'O', 'Size': '4'},
                     {'Address': '0x20', 'Name': 'two', 'Quality': 'O', 'Size': '4'}]
        self.files = [{'Address': '0x10', 'File': self.owners[0]}, {'Address': '0x20', 'File': self.owners[1]}]
        self.targets = [{'address': x['Address'], 'name': x['Name']} for x in self.rows]
        self.nss = self.root/'nss'
        self.nss.write_bytes(b'fixture nss')
        self.original = self.root/'main.elf'
        self.original.write_bytes(b'fixture original')
        self.evidence = {'baseline': {}, 'candidate': {}}
        self.objects = {}
        for label in ('baseline', 'candidate'):
            for mode in ('normal', 'lua'):
                attempt = self.root/label/mode/'attempt-0001'
                root = attempt/'receipt'
                support = attempt/'support/data/2.6.2'
                support.mkdir(parents=True)
                shutil.copy2(self.original, support/'main.elf')
                (support/'functions.csv').write_text('Address,Name,Quality,Size\n0x10,one,O,4\n0x20,two,O,4\n')
                (support/'function_files.csv').write_text('Address,File\n0x10,a/X.cpp\n0x20,b/X.cpp\n')
                (support/'data_symbols.csv').write_text('Address,Name\n')
                c.write(root/'result.json', {'format': 1, 'results': [
                    {'address': x['address'], 'name': x['name'], 'status': 'ok', 'reason': ''} for x in self.targets]})
                for owner, name in zip(self.owners, ('one', 'two')):
                    obj = root/'objects/CMakeFiles/unit.dir/src'/(owner+'.obj')
                    obj.parent.mkdir(parents=True, exist_ok=True)
                    def rename(symbols, sections, metadata, name=name):
                        symbols[3] = (name, *symbols[3][1:])
                    fixture(obj, mutate=rename)
                    self.objects[label, mode, owner] = obj
                self.evidence[label][mode] = {'root': str(root)}
        self.binding = {'kind': group.KIND, 'policy': 'recovery-group', 'owners': self.owners,
            'partition': group.partition(self.owners, self.targets, self.rows, self.files),
            'targets': {'format': 1, 'functions': self.targets},
            'nss_sha256': c.sha(self.nss), 'nss_path': str(self.nss), 'original_sha256': c.sha(self.original),
            'post_landing': False, 'changed_paths': [], 'recovery_tools': {},
            'versions': {label: {'source': {'src/a/X.cpp': {'sha256': 'a'}, 'src/b/X.cpp': {'sha256': 'b'}},
                                 'support': {name: {'sha256': name} for name in receipts.SUPPORT}}
                         for label in ('baseline', 'candidate')}}
        c.write(self.output/'binding.json', self.binding)
        c.write(self.output/'evidence.json', self.evidence)
        self.args = SimpleNamespace(original_elf=self.original, nss=self.nss)
        self.regress = False
        self.data_change = False
        self.ledger_calls = []
        def verify(obj, rows):
            symbols = {x['name'] for x in strict.symbols(group_elf(obj)).values() if x['section']}
            exact = not (self.regress and 'candidate' in obj.parts)
            return {'results': [{'symbol': row['Name'], 'exact': exact} for row in rows], 'symbols': sorted(symbols)}
        def ledger(csv_path, original, link, nss):
            self.ledger_calls.append(link)
            changed = self.data_change and 'candidate' in link.parts
            return {'rows': [{'address': 1, 'name': 'data', 'validation': {
                'status': 'different' if changed else 'verified_equal', 'reasons': []}}]}
        from validation.elf import Elf as group_elf
        self.addCleanup(patch.stopall)
        patch.object(strict, 'configure').start()
        patch.object(strict, 'verify', side_effect=verify).start()
        patch.object(contributor_adapter, 'executable_coverage', return_value=[]).start()
        patch.object(unwind, 'audit', return_value={'status': 'pass'}).start()
        patch.object(validate_data, 'validate_data', side_effect=ledger).start()

    def audit(self):
        group.audit(self.args, self.output, self.binding, self.evidence, self.rows, self.files)
        return c.read(self.output/'recovery-group-audit.json')

    def replay(self):
        return group.verify_audit(self.output, self.binding, {'audit_sha256': c.sha(self.output/'recovery-group-audit.json')})

    def test_full_audit_and_replay_and_tampering(self):
        report = self.audit()
        self.assertEqual(len(self.ledger_calls), 4)
        for mode in ('normal', 'lua'):
            for index in (0, 1):
                for obligation in ('owner-mapping-symbols', 'owner-object-relocations-extras', 'abi-unwind', 'object-paths'):
                    self.assertIn(f'{mode}:owner-{index}:{obligation}', report['obligations'])
            self.assertIn(mode+':global-data-abi', report['obligations'])
        self.assertEqual(report, self.replay())
        del report['obligations']['normal:owner-0:abi-unwind']
        c.write(self.output/'recovery-group-audit.json', report)
        with self.assertRaises(c.Halt):
            self.replay()

    def test_replay_rejects_rehashed_forbidden_scope_and_lineage(self):
        self.audit()
        baseline = copy.deepcopy(self.binding)
        for kind in ('unrelated', 'toolchain', 'ownership', 'missing-owner', 'policy', 'landing-type'):
            self.binding = copy.deepcopy(baseline)
            candidate = self.binding['versions']['candidate']
            if kind == 'unrelated':
                candidate['source']['src/unrelated.cpp'] = {'sha256': 'changed'}
                self.binding['changed_paths'] = ['src/unrelated.cpp']
            elif kind == 'toolchain':
                candidate['source']['toolchain/recipe'] = {'sha256': 'changed'}
                self.binding['changed_paths'] = ['toolchain/recipe']
            elif kind == 'ownership':
                candidate['support']['data/2.6.2/function_files.csv'] = {'sha256': 'changed'}
            elif kind == 'missing-owner':
                del candidate['source']['src/b/X.cpp']
                self.binding['changed_paths'] = ['src/b/X.cpp']
            elif kind == 'policy':
                self.binding['kind'] = 'recovery-only-v1'
            else:
                self.binding['post_landing'] = 1
            c.write(self.output/'binding.json', self.binding)
            with self.subTest(kind=kind), self.assertRaises(c.Halt):
                self.replay()
        self.binding = baseline

    def test_missing_or_swapped_owner_functions_rejected(self):
        a = self.objects['candidate', 'normal', self.owners[0]]
        b = self.objects['candidate', 'normal', self.owners[1]]
        contents = a.read_bytes(), b.read_bytes()
        a.write_bytes(contents[1]); b.write_bytes(contents[0])
        with self.assertRaises(c.Halt):
            self.audit()
        b.write_bytes(contents[1])
        with self.assertRaises(c.Halt):
            self.audit()

    def test_lost_exact_and_global_data_change_rejected(self):
        self.regress = True
        with self.assertRaises(c.Halt):
            self.audit()
        self.regress = False
        self.data_change = True
        with self.assertRaises(c.Halt):
            self.audit()



class GroupBuildTests(unittest.TestCase):
    # Exercise receipt orchestration using synthetic build and audit fixtures.
    def test_four_shared_builds_and_policy_separation(self):
        fixture = test_recovery.RecoveryReceipts()
        fixture.setUp()
        self.addCleanup(fixture.doCleanups)
        fixture.recovery_args()
        fixture.args.mode = 'recovery-group'
        fixture.args.run = True
        def configure(args, binding):
            binding.update(policy='recovery-group', owners=['A.cpp', 'B.cpp'])
        def audit(args, output, binding, evidence):
            c.write(output/'recovery-group-audit.json', {'fixture': True})
        with patch.object(group, 'configure', side_effect=configure), patch.object(group, 'audit', side_effect=audit):
            result = receipts.build(fixture.args, fixture.recovery_builder)
        self.assertEqual(len(fixture.calls), 4)
        self.assertEqual(result['policy'], 'recovery-group')
        for policy in ('strict', 'recovery-only'):
            with self.assertRaises(c.Halt):
                receipts.verify(fixture.output, policy=policy)


if __name__ == '__main__':
    unittest.main()
