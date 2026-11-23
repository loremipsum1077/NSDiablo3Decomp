import contextlib
"""Provider-free receipt construction contracts; all builders/images are fixtures."""
import json
import os
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile
import unittest
from unittest.mock import patch

import contributor_check as cc
import receipts as r
import backends
from backends import backend
from receipt_test_fixtures import linked_cases
from validation import common as c


class ReceiptTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)
        self.repo = self.root / 'core'
        self.repo.mkdir()
        self.git('init', '-q')
        for name, value in {
            'src/X/Owner.cpp': 'int prior() { return 1; }\n',
            'include/fixture.h': 'int prior();\n',
            'toolchain/fixture.cmake': 'fixture compiler settings\n',
            'CMakeLists.txt': 'fixture recipe\n',
            'tools/config.toml': 'default_version = "2.6.2"\n',
            'data/2.6.2/functions.csv': 'Address,Quality,Size,Name\n0x10,O,4,prior\n0x20,U,4,target\n',
            'data/2.6.2/function_files.csv': 'Address,File,Compiler\n0x10,X/Owner.cpp,clang\n0x20,X/Owner.cpp,clang\n',
            'data/2.6.2/data_symbols.csv': 'Address,Name\n',
        }.items():
            path = self.repo / name
            path.parent.mkdir(parents=True, exist_ok=True)
            path.write_text(value)
        shutil.copytree(backends.ROOT / 'toolchain/lua', self.repo / 'toolchain/lua', dirs_exist_ok=True)
        for name in ('llvm.lock.json', 'llvm_archive.py'):
            shutil.copy2(backends.ROOT / 'toolchain' / name, self.repo / 'toolchain' / name)
        self.git('add', '.')
        self.git('commit', '-qm', 'baseline fixture')
        self.baseline = self.git('rev-parse', 'HEAD')
        (self.repo / 'src/X/Owner.cpp').write_text('int prior() { return 1; }\nint target() { return 2; }\n')
        self.git('add', 'src')
        self.git('commit', '-qm', 'candidate fixture')
        self.candidate = self.git('rev-parse', 'HEAD')
        self.original = self.root / 'original'
        self.original.write_bytes(b'private original fixture')
        self.checker = self.root / 'checker'
        self.checker.write_bytes(b'trusted fixture checker, never executed')
        self.checker.chmod(0o755)
        self.cache = self.root / 'lua-cache'
        (self.cache / 'out/bin').mkdir(parents=True)
        (self.cache / 'out/bin/llc').write_bytes(b'\x7fELF\x02\x01' + b'\0'*12 + (183).to_bytes(2, 'little') + b'fixture')
        (self.cache / 'out/bin/llc').chmod(0o755)
        c.write(self.cache / 'receipt.json', backend.spec(self.repo) | {
            'binary_sha256': c.sha(self.cache / 'out/bin/llc'), 'configure_options': backend.OPTIONS,
            'gcc_version': 'gcc fixture', 'llc_version': 'LLVM version 5.0.1 with assertions aarch64'})
        self.targets = self.root / 'targets.json'
        c.write(self.targets, {'format': 1, 'functions': [{'address': '0x20', 'name': 'target'}]})
        self.output = self.root / 'private receipts'
        self.args = r.parser().parse_args(['build', '--repo', str(self.repo),
            '--baseline', self.baseline, '--candidate', self.candidate,
            '--original-elf', str(self.original), '--original-sha256', c.sha(self.original),
            '--checker', str(self.checker), '--checker-sha256', c.sha(self.checker),
            '--lua-cache', str(self.cache), '--lua-receipt-sha256', c.sha(self.cache / 'receipt.json'),
            '--targets', str(self.targets), '--output', str(self.output)])
        self.calls = []
        self.container_names = []
        self.addCleanup(patch.stopall)
        patch.object(r.resource_admission, 'LOCK_PATH', str(self.root / 'fixture-resource.lock')).start()
        patch.object(r.resource_admission, 'check', return_value=None).start()
        patch.object(r.docker_admission, 'session', side_effect=lambda _: contextlib.nullcontext()).start()
        patch.object(backends, 'check', return_value=[]).start()
        patch.object(r, 'image_ids', return_value={'normal': 'sha256:' + 'a'*64, 'lua': 'sha256:' + 'b'*64}).start()

    def git(self, *args):
        return subprocess.check_output(['git', '-C', str(self.repo), '-c', 'user.name=Fixture',
            '-c', 'user.email=fixture@example.invalid', '-c', 'commit.gpgsign=false', *args], text=True).strip()

    def builder(self, source, support, dest, mode, *args, **kwargs):
        self.calls.append((source, dest, mode))
        self.container_names.append(args[1])
        self.assertFalse(source.is_relative_to(self.repo))
        self.assertNotIn('PROVIDER_FIXTURE', kwargs)
        dest.mkdir(parents=True)
        data = dest / 'build/2.6.2'
        data.mkdir(parents=True)
        (data / 'd3').write_bytes(b'rebuilt fixture ' + mode.encode())
        (data / 'd3.map').write_bytes(b'fixture map')
        c.write(data / 'd3.map.provenance', {'format': 1, 'elf_sha256': c.sha(data / 'd3'), 'map_sha256': c.sha(data / 'd3.map')})
        obj = dest / 'objects/CMakeFiles/fixture.dir/src/X/Owner.cpp.obj'
        obj.parent.mkdir(parents=True)
        obj.write_bytes(b'fixture object')
        docker = ['docker', 'run', '--rm', '--pull', 'never', '--mount',
                  'type=bind,src=' + str(source) + ',dst=/work,readonly', kwargs['images'][mode]]
        c.write(dest / 'configure.command.json', docker + ['cmake', '-G', 'Ninja',
            '-DCMAKE_TOOLCHAIN_FILE=toolchain/ToolchainNX64.cmake',
            '-DD3_LUA_COMPILER=' + ('ON' if mode == 'lua' else 'OFF'), '-B', 'build/2.6.2'])
        c.write(dest / 'build.command.json', docker + ['cmake', '--build', 'build/2.6.2', '--', '-j2'])
        results = [{'address': '0x10', 'name': 'prior', 'status': 'ok'}]
        if 'candidate' in source.parts:
            results.append({'address': '0x20', 'name': 'target', 'status': 'ok'})
        c.write(dest / 'result.json', {'results': results})
        c.write(dest / 'done.json', {'full_check_rc': 0, 'files': {
            p.relative_to(dest).as_posix(): c.sha(p) for p in dest.rglob('*') if p.is_file()}})

    def run_build(self):
        self.args.run = True
        return r.build(self.args, self.builder)

    def rebind_evidence(self, state):
        evidence = c.read(self.output / 'evidence.json')
        for label in ('baseline', 'candidate'):
            for mode in ('normal', 'lua'):
                entry = state['modes'][label + '_' + mode]
                evidence[label][mode] = {'root': str(self.output / entry['root']),
                                         'receipt_sha256': entry['receipt_sha256']}
        c.write(self.output / 'evidence.json', evidence)
        state['evidence_sha256'] = c.sha(self.output / 'evidence.json')
        c.write(self.output / 'run.json', state)

    def verify_and_resume_refuse(self):
        with self.assertRaises(c.Halt):
            r.verify(self.output)
        before = (self.output / 'run.json').read_bytes()
        self.args.resume = True
        with self.assertRaises(c.Halt):
            r.build(self.args, lambda *args, **kwargs: self.fail('corrupt checkpoint launched a build'))
        self.assertEqual(before, (self.output / 'run.json').read_bytes())

    def test_preflight_is_read_only_and_never_probes_docker(self):
        with patch.object(r, 'image_ids', side_effect=AssertionError('no daemon in preflight')), \
             patch.object(r.resource_admission, 'lease', side_effect=AssertionError('no lease in preflight')):
            answer = r.build(self.args, self.builder)
        self.assertEqual(answer['status'], 'preflight')
        self.assertTrue(answer['resource_assessment']['advisory'])
        self.assertFalse(answer['resource_assessment']['reservation_held'])
        self.assertFalse(self.output.exists())
        self.assertEqual(self.calls, [])

    def test_admission_failure_precedes_output_creation(self):
        self.args.run = True
        with patch.object(r.resource_admission, 'check', side_effect=r.resource_admission.AdmissionError('insufficient capacity')):
            with self.assertRaisesRegex(c.Halt, 'insufficient capacity'):
                r.build(self.args, self.builder)
        self.assertFalse(self.output.exists())
        self.assertEqual(self.calls, [])

    def test_pending_docker_session_precedes_receipt_output_creation(self):
        self.args.run = True
        with patch.object(r.docker_admission, 'session', side_effect=r.resource_admission.AdmissionError('pending session')):
            with self.assertRaisesRegex(c.Halt, 'pending session'):
                r.build(self.args, self.builder)
        self.assertFalse(self.output.exists())
        self.assertEqual(self.calls, [])

    def test_preflight_assessment_error_is_controlled_without_output(self):
        with patch.object(r.resource_admission, 'assess', side_effect=RuntimeError('filesystem unavailable')):
            with self.assertRaisesRegex(c.Halt, 'filesystem unavailable'):
                r.build(self.args, self.builder)
        self.assertFalse(self.output.exists())

    def test_late_capacity_failure_preserves_checkpoint_and_starts_no_attempt(self):
        self.args.run = True
        with patch.object(r.resource_admission, 'check', side_effect=[None, None, r.resource_admission.AdmissionError('capacity fell')]):
            with self.assertRaisesRegex(c.Halt, 'capacity fell'):
                r.build(self.args, self.builder)
        self.assertTrue((self.output / 'binding.json').is_file())
        state = c.read(self.output / 'run.json')
        self.assertEqual(set(state['modes']), {'baseline_normal'})
        saved = (self.output / state['modes']['baseline_normal']['root'] / 'done.json').read_bytes()
        self.assertFalse((self.output / 'baseline/lua').exists())
        self.assertEqual(len(self.calls), 1)
        self.args.resume = True
        self.run_build()
        self.assertEqual((self.output / state['modes']['baseline_normal']['root'] / 'done.json').read_bytes(), saved)
        self.assertEqual(len(self.calls), 4)

    def test_post_landing_verify_binds_exact_commit_and_inventory(self):
        self.run_build()
        r.verify(self.output, self.repo, self.candidate)
        with self.assertRaises(c.Halt):
            r.verify(self.output, self.repo, self.baseline)
        inventory = self.repo / 'data/2.6.2/functions.csv'
        inventory.write_text(inventory.read_text().replace('0x20,U,', '0x20,O,'))
        self.git('add', 'data')
        self.git('commit', '-qm', 'legitimate landing inventory')
        landed = self.git('rev-parse', 'HEAD')
        with self.assertRaises(c.Halt):
            r.verify(self.output, self.repo, landed)
        self.args.candidate = landed
        self.args.output = self.root / 'post-landing'
        r.build(self.args, self.builder)
        r.verify(self.args.output, self.repo, landed)

    def test_four_fresh_mode_receipts_and_replay_do_not_accept_source(self):
        with patch.dict(os.environ, {'PROVIDER_FIXTURE': 'not needed'}):
            answer = self.run_build()
        self.assertEqual(len(self.calls), 4)
        self.assertTrue(answer['fresh_build_executed'])
        self.assertFalse(answer['accepted'])
        self.assertIn('unwind', answer['pending_gates'])
        evidence = c.read(self.output / 'evidence.json')
        self.assertEqual(set(evidence), {'format', 'baseline', 'candidate', 'toolchain'})
        with patch.object(r.resource_admission, 'lease', side_effect=AssertionError('no lease in verify')):
            replay = r.verify(self.output)
        self.assertFalse(replay['fresh_build_executed'])
        self.assertFalse(replay['accepted'])

    def test_selected_mismatch_improvement_agrees_with_report(self):
        request = [{'address': '0x10', 'name': 'prior'}, {'address': '0x20', 'name': 'target'}]
        targets = [{'Address': '0x20', 'Name': 'target'}]
        report = cc.linked_summary(request, ['ok', 'mismatch'], ['ok', 'ok'], targets)
        self.assertEqual(report['status'], 'pass')
        def improvement(*args, **kwargs):
            self.builder(*args, **kwargs)
            dest = args[2]
            if 'baseline' in args[0].parts:
                result = c.read(dest / 'result.json')
                result['results'].append(dict(request[1], status='mismatch'))
                c.write(dest / 'result.json', result)
                done = c.read(dest / 'done.json')
                done['files']['result.json'] = c.sha(dest / 'result.json')
                c.write(dest / 'done.json', done)
        self.args.run = True
        self.assertEqual(r.build(self.args, improvement)['status'], 'complete')

    def test_producer_uses_shared_selected_scope_cases(self):
        self.args.run = True
        for name, old, new, targets, succeeds, _ in linked_cases():
            with self.subTest(case=name):
                self.args.output = self.root / name
                c.write(self.targets, {'format': 1, 'functions': targets})
                def fixture(*args, **kwargs):
                    self.builder(*args, **kwargs)
                    dest = args[2]
                    c.write(dest / 'result.json', {'format': 1, 'results':
                        old if 'baseline' in args[0].parts else new})
                    done = c.read(dest / 'done.json')
                    done['files']['result.json'] = c.sha(dest / 'result.json')
                    c.write(dest / 'done.json', done)
                if succeeds:
                    answer = r.build(self.args, fixture)
                    self.assertEqual(answer['status'], 'complete')
                    self.assertFalse(answer['accepted'])
                    self.assertEqual(answer['pending_gates'], r.PENDING)
                else:
                    with self.assertRaises(c.Halt):
                        r.build(self.args, fixture)
                    self.assertFalse((self.args.output / 'evidence.json').exists())

    def test_scope_change_cannot_reuse_completed_attempts(self):
        self.run_build()
        c.write(self.targets, {'format': 1, 'functions': [{'address': '0x10', 'name': 'prior'}]})
        self.args.resume = True
        with self.assertRaisesRegex(c.Halt, 'binding changed'):
            r.build(self.args, lambda *args, **kwargs: self.fail('changed scope launched build'))
        binding = c.read(self.output / 'binding.json')
        binding['targets'] = c.read(self.targets)
        c.write(self.output / 'binding.json', binding)
        state = c.read(self.output / 'run.json')
        state['binding_sha256'] = c.sha(self.output / 'binding.json')
        c.write(self.output / 'run.json', state)
        self.verify_and_resume_refuse()

    def test_duplicate_inventory_identity_cannot_authorize_scope(self):
        path = self.repo / 'data/2.6.2/functions.csv'
        path.write_text(path.read_text() + '0x20,U,4,target\n')
        self.git('add', 'data')
        self.git('commit', '-qm', 'duplicate inventory fixture')
        self.args.candidate = self.git('rev-parse', 'HEAD')
        with self.assertRaisesRegex(c.Halt, 'inventory identities'):
            r.build(self.args)

    def test_completed_resume_verifies_without_new_fresh_build_claim(self):
        self.run_build()
        self.args.resume = True
        answer = r.build(self.args, lambda *args, **kwargs: self.fail('completed mode rebuilt'))
        self.assertFalse(answer['fresh_build_executed'])

    def test_lua_receipts_cannot_substitute_for_normal_checkpoints(self):
        self.run_build()
        state = c.read(self.output / 'run.json')
        for label in ('baseline', 'candidate'):
            state['modes'][label + '_normal'] = dict(state['modes'][label + '_lua'])
        self.rebind_evidence(state)
        self.verify_and_resume_refuse()

    def test_candidate_receipt_cannot_substitute_for_baseline(self):
        self.run_build()
        state = c.read(self.output / 'run.json')
        state['modes']['baseline_normal'] = dict(state['modes']['candidate_normal'])
        self.rebind_evidence(state)
        self.verify_and_resume_refuse()

    def test_attempt_binding_fields_cannot_be_hash_rebound(self):
        self.run_build()
        saved_state = c.read(self.output / 'run.json')
        entry = saved_state['modes']['baseline_normal']
        proof = self.output / entry['root'] / '..' / 'binding.json'
        original = c.read(proof)
        for field in ('run_sha256', 'version', 'mode', 'commit', 'image_id',
                      'source_files_sha256', 'support_files_sha256', 'checker_sha256',
                      'original_sha256', 'lua_receipt_sha256', 'lua_backend_sha256'):
            with self.subTest(field=field):
                altered = dict(original, **{field: 'changed'})
                c.write(proof, altered)
                state = json.loads(json.dumps(saved_state))
                state['modes']['baseline_normal']['attempt_binding_sha256'] = c.sha(proof)
                self.rebind_evidence(state)
                self.verify_and_resume_refuse()
        c.write(proof, original)

    def test_tampered_attempt_source_and_support_cannot_be_skipped(self):
        self.run_build()
        state = c.read(self.output / 'run.json')
        attempt = (self.output / state['modes']['baseline_normal']['root']).parent
        for name in ('source/src/X/Owner.cpp', 'support/tools/check',
                     'support/tools/config.toml', 'support/data/2.6.2/functions.csv',
                     'support/data/2.6.2/main.elf'):
            path = attempt / name
            original = path.read_bytes()
            with self.subTest(name=name):
                path.write_bytes(b'tampered attempt input')
                self.verify_and_resume_refuse()
            path.write_bytes(original)

    def test_mode_command_cannot_be_rebound_to_another_compiler(self):
        self.run_build()
        state = c.read(self.output / 'run.json')
        entry = state['modes']['baseline_normal']
        receipt = self.output / entry['root']
        command = c.read(receipt / 'configure.command.json')
        command[command.index('-DD3_LUA_COMPILER=OFF')] = '-DD3_LUA_COMPILER=ON'
        c.write(receipt / 'configure.command.json', command)
        done = c.read(receipt / 'done.json')
        done['files']['configure.command.json'] = c.sha(receipt / 'configure.command.json')
        c.write(receipt / 'done.json', done)
        entry['receipt_sha256'] = c.sha(receipt / 'done.json')
        self.rebind_evidence(state)
        self.verify_and_resume_refuse()

    def test_isolated_outputs_use_distinct_container_names(self):
        self.run_build()
        first = set(self.container_names)
        self.args.output = self.root / 'other private output'
        self.run_build()
        self.assertEqual(len(set(self.container_names)), 8)
        self.assertTrue(first.isdisjoint(set(self.container_names[4:])))

    def test_partial_attempt_is_retained_and_restarted_in_new_attempt(self):
        self.args.run = True
        def interrupted(*args, **kwargs):
            self.builder(*args, **kwargs)
            raise c.Halt('simulated interruption before checkpoint')
        with self.assertRaisesRegex(c.Halt, 'simulated'):
            r.build(self.args, interrupted)
        original = self.output / 'baseline/normal/attempt-0001/receipt/done.json'
        before = original.read_bytes()
        self.args.resume = True
        self.run_build()
        self.assertEqual(before, original.read_bytes())
        self.assertTrue((self.output / 'baseline/normal/attempt-0002/receipt/done.json').is_file())

    def test_changed_input_and_image_pins_refuse_resume(self):
        self.run_build()
        self.args.resume = True
        with patch.object(r, 'image_ids', return_value={'normal': 'sha256:' + 'c'*64, 'lua': 'sha256:' + 'b'*64}):
            with self.assertRaisesRegex(c.Halt, 'binding changed'):
                self.run_build()
        self.original.write_bytes(b'changed fixture')
        with self.assertRaisesRegex(c.Halt, 'pin mismatch'):
            self.run_build()

    def test_source_and_artifact_tamper_fail_closed(self):
        self.run_build()
        source = self.output / 'candidate/source/src/X/Owner.cpp'
        source.write_text('tampered source')
        with self.assertRaisesRegex(c.Halt, 'Source snapshot changed'):
            r.verify(self.output)
        source.write_bytes(self.git('show', self.candidate + ':src/X/Owner.cpp').encode() + b'\n')
        evidence = c.read(self.output / 'evidence.json')
        receipt = Path(evidence['candidate']['normal']['root'])
        (receipt / 'build/2.6.2/d3').write_bytes(b'tampered ELF')
        with self.assertRaisesRegex(c.Halt, 'Retained artifact changed'):
            r.verify(self.output)

    def test_missing_mode_and_forged_manifest_never_verify(self):
        self.run_build()
        state = c.read(self.output / 'run.json')
        del state['modes']['candidate_lua']
        c.write(self.output / 'run.json', state)
        with self.assertRaisesRegex(c.Halt, 'Missing or stale'):
            r.verify(self.output)

    def test_forged_evidence_identity_cannot_be_rebound_by_new_hash(self):
        self.run_build()
        manifest = self.output / 'evidence.json'
        evidence = c.read(manifest)
        evidence['candidate']['commit'] = '0' * 40
        c.write(manifest, evidence)
        state = c.read(self.output / 'run.json')
        state['evidence_sha256'] = c.sha(manifest)
        c.write(self.output / 'run.json', state)
        with self.assertRaisesRegex(c.Halt, 'does not match bound'):
            r.verify(self.output)

    def test_target_identity_and_backend_pins_are_required(self):
        c.write(self.targets, {'format': 1, 'functions': [{'address': '0x20', 'name': 'wrong'}]})
        with self.assertRaisesRegex(c.Halt, 'Target absent'):
            r.build(self.args)
        (self.cache / 'out/bin/llc').write_bytes(b'tampered backend')
        with self.assertRaisesRegex(c.Halt, 'backend'):
            r.build(self.args)

    def test_new_regression_keeps_run_incomplete(self):
        self.args.run = True
        def regressed(*args, **kwargs):
            self.builder(*args, **kwargs)
            dest = args[2]
            result = c.read(dest / 'result.json')
            if 'candidate' in args[0].parts:
                result['results'][0]['status'] = 'mismatch'
            c.write(dest / 'result.json', result)
            files = {p.relative_to(dest).as_posix(): c.sha(p) for p in dest.rglob('*') if p.is_file() and p.name != 'done.json'}
            c.write(dest / 'done.json', {'full_check_rc': 0, 'files': files})
        with self.assertRaisesRegex(c.Halt, 'identity preservation'):
            r.build(self.args, regressed)
        self.assertFalse((self.output / 'evidence.json').exists())

    def test_retained_path_escape_and_symlink_are_rejected(self):
        with self.assertRaises(c.Halt):
            r.inside(self.root, '../outside')
        link = self.root / 'link'
        link.symlink_to(self.repo, target_is_directory=True)
        with self.assertRaises(c.Halt):
            r.inside(self.root, 'link/src/X/Owner.cpp')

    def test_duplicate_or_malformed_metadata_is_rejected(self):
        self.targets.write_text('{"format":1,"format":1,"functions":[]}')
        with self.assertRaisesRegex(c.Halt, 'Duplicate metadata'):
            r.build(self.args)
        c.write(self.targets, {'format': True, 'functions': []})
        with self.assertRaisesRegex(c.Halt, 'target manifest'):
            r.build(self.args)

    def test_cpu_modules_import_without_orchestration_or_companion(self):
        tools = Path(__file__).resolve().parent
        standalone = self.root / 'standalone/tools'
        standalone.mkdir(parents=True)
        shutil.copytree(tools / 'validation', standalone / 'validation', ignore=shutil.ignore_patterns('__pycache__'))
        for name in ('receipts.py', 'game_data_manifest.py', 'contributor_adapter.py', 'data_provenance.py', 'resource_admission.py', 'docker_admission.py'):
            shutil.copyfile(tools / name, standalone / name)
        code = "import sys;sys.path.insert(0," + repr(str(standalone)) + ");sys.modules['bounded_workflow']=None;sys.modules['flag_sweep']=None;import receipts,contributor_adapter;contributor_adapter.validators();print('provider-free')"
        result = subprocess.run([sys.executable, '-c', code], cwd=self.root, env={'PATH': os.defpath,
            'PYTHONDONTWRITEBYTECODE': '1'}, text=True, capture_output=True, check=True)
        self.assertIn('provider-free', result.stdout)


if __name__ == '__main__':
    unittest.main()
