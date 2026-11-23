"""Offline wizard regressions: no actual software install, Docker or private inputs."""
import contextlib
import importlib.util
import io
import json
import os
from pathlib import Path
import subprocess
import sys
import tempfile
import types
import unittest
from unittest.mock import Mock, patch

import onboarding as o


class WizardTests(unittest.TestCase):
    def setUp(self):
        temp = tempfile.TemporaryDirectory()
        self.addCleanup(temp.cleanup)
        self.root = Path(temp.name)
        self.w = o.Wizard(self.root)
        self.output = io.StringIO()
        self.capture = contextlib.redirect_stdout(self.output)
        self.capture.__enter__()
        self.addCleanup(self.capture.__exit__, None, None, None)

    def test_missing_prerequisite_has_verification_and_never_installs(self):
        with patch.object(o.platform, 'system', return_value='Darwin'), \
             patch.object(o.platform, 'machine', return_value='arm64'), \
             patch.dict(os.environ, {'D3_DOCKER_PLATFORM': 'linux/arm64'}), \
             patch.object(o.shutil, 'which', return_value=None), \
             patch.object(self.w, 'command') as command:
            with self.assertRaisesRegex(o.Blocked, 'git missing from PATH.*Verify: git --version'):
                self.w.prerequisites()
            command.assert_not_called()

    def test_wrong_architecture_and_unsupported_hosts_stop_before_commands(self):
        for system, machine, selection in [('Linux', 'x86_64', 'linux/arm64'),
                                           ('Windows', 'arm64', 'linux/arm64'),
                                           ('Linux', 'riscv64', 'linux/arm64')]:
            with self.subTest(system=system, machine=machine), \
                 patch.object(o.platform, 'system', return_value=system), \
                 patch.object(o.platform, 'machine', return_value=machine), \
                 patch.dict(os.environ, {'D3_DOCKER_PLATFORM': selection}), \
                 patch.object(self.w, 'command') as command:
                with self.assertRaises(o.Blocked):
                    self.w.prerequisites()
                command.assert_not_called()

    def test_decline_eof_and_unattended_never_grant_consent(self):
        for answer in ('no', 'y', '', EOFError()):
            self.w.interactive = True
            with self.subTest(answer=repr(answer)), patch('builtins.input', side_effect=[answer]), \
                 patch.object(self.w, 'command') as command:
                with self.assertRaisesRegex(o.Blocked, 'declined'):
                    self.w.consent(['python3', 'tools/setup.py'], 'Install')
                command.assert_not_called()
        self.w.interactive = False
        with patch('builtins.input', side_effect=AssertionError('must not prompt')), \
             patch.object(self.w, 'command') as command:
            with self.assertRaises(o.Blocked):
                self.w.consent(['anything'], 'Install')
            command.assert_not_called()

    def test_failed_optional_stage_is_skipped_not_passed(self):
        with patch.object(self.w, 'ask', return_value='s'):
            self.assertFalse(self.w.stage(1, 'optional', lambda: (_ for _ in ()).throw(o.Blocked('missing')), optional=True))
        self.assertEqual(self.w.state['stages']['optional']['status'], 'skipped')
        self.assertNotIn('PASS:', self.output.getvalue())

    def test_required_failure_cannot_be_skipped(self):
        with patch.object(self.w, 'ask', return_value='s'):
            with self.assertRaises(o.Blocked):
                self.w.stage(1, 'required', lambda: (_ for _ in ()).throw(o.Blocked('missing')))
        self.assertEqual(self.w.state['stages']['required']['status'], 'blocked')

    def test_retry_executes_check_again(self):
        from unittest.mock import Mock
        action = Mock(side_effect=[o.Blocked('missing'), None])
        with patch.object(self.w, 'ask', return_value='r'):
            self.assertTrue(self.w.stage(1, 'required', action))
        self.assertEqual(action.call_count, 2)

    def test_input_mismatch_is_blocked_without_leaking_imported_exception(self):
        with patch.object(o, 'require_inputs', side_effect=RuntimeError('/private/token=secret')):
            with self.assertRaisesRegex(o.Blocked, 'checksum/size mismatch') as caught:
                self.w.originals()
        self.assertNotIn('secret', str(caught.exception))
        self.assertNotIn('inputs', self.w.state)

    def test_readonly_missing_generated_inputs_gives_bootstrap_command(self):
        config = self.root / 'config'
        config.mkdir()
        (config / 'local_game_data.json').write_text(json.dumps({
            'descriptors': {}, 'includes': {'example.inc': '0' * 64}
        }))
        with patch.object(o, 'require_inputs', return_value=[]), patch.object(self.w, 'consent') as consent:
            with self.assertRaisesRegex(o.Blocked, 'bootstrap_game_data.py build --build-protoc'):
                self.w.originals()
            consent.assert_not_called()

    def test_missing_stale_corrupt_compiler_never_becomes_ready_when_declined(self):
        import backends
        for reason in ('missing', 'stale', 'corrupted'):
            with self.subTest(reason=reason), patch.object(backends, 'check', return_value=[reason]), \
                 patch.object(self.w, 'command') as command:
                with self.assertRaises(o.Blocked):
                    self.w.stage(4, 'compiler', self.w.backend)
                self.assertEqual(self.w.state['stages']['compiler']['status'], 'blocked')
                command.assert_not_called()

    def test_compiler_must_pass_recheck_after_preparation(self):
        import backends
        with patch.object(backends, 'check', return_value=['stale']) as check, \
             patch.object(self.w, 'consent'):
            with self.assertRaisesRegex(o.Blocked, 'still not ready'):
                self.w.backend()
            self.assertEqual(check.call_count, 2)

    def test_malformed_and_untrusted_state_never_controls_execution(self):
        for content in ('{bad', '[]', '{"schema": 99}',
                        json.dumps({'schema': 1, 'stages': {'danger': {'status': 'passed'}}, 'command': 'touch injected'})):
            self.w.directory.mkdir(exist_ok=True)
            (self.w.directory / 'state.json').write_text(content)
            with self.w.checkpoint():
                self.assertEqual(self.w.state, {'schema': 1, 'stages': {}})
                self.w.stage(1, 'again', lambda: None)
            state = json.loads((self.w.directory / 'state.json').read_text())
            self.assertNotIn('command', state)
            self.assertEqual(state['stages']['again']['status'], 'passed')
            self.w = o.Wizard(self.root)

    def test_concurrent_invocation_rejected_and_lock_released(self):
        other = o.Wizard(self.root)
        with self.w.checkpoint():
            with self.assertRaisesRegex(o.Blocked, 'Another wizard'):
                with other.checkpoint():
                    self.fail('concurrent checkpoint entered')
        with other.checkpoint():
            self.assertTrue(other.writable)

    def test_interrupted_stage_is_saved_and_rechecked_on_resume(self):
        with self.assertRaises(KeyboardInterrupt):
            with self.w.checkpoint():
                self.w.stage(1, 'probe', lambda: (_ for _ in ()).throw(KeyboardInterrupt()))
        state = json.loads((self.w.directory / 'state.json').read_text())
        self.assertEqual(state['stages']['probe']['status'], 'running')
        fresh = o.Wizard(self.root)
        calls = []
        with fresh.checkpoint():
            fresh.stage(1, 'probe', lambda: calls.append(True))
        self.assertEqual(calls, [True])
        self.assertEqual(fresh.state['stages']['probe']['status'], 'passed')

    def test_symlink_state_directory_rejected(self):
        elsewhere = self.root / 'other'
        elsewhere.mkdir()
        self.w.directory.symlink_to(elsewhere, target_is_directory=True)
        with self.assertRaisesRegex(o.Blocked, 'symlink'):
            with self.w.checkpoint():
                self.fail('entered symlink')
        self.assertEqual(list(elsewhere.iterdir()), [])

    def test_fingerprint_changes_with_recipe_platform_tool_and_head(self):
        with patch.object(self.w, 'tracked', return_value={'recipe': 'a'}) as tracked, \
             patch.object(self.w, 'git', return_value='head') as git, \
             patch.object(o.platform, 'machine', return_value='arm64') as machine, \
             patch.object(o.shutil, 'which', return_value='/bin/tool') as which:
            original = self.w.fingerprint()
            for mock, value in [(tracked, {'recipe': 'b'}), (git, 'newhead'),
                                (machine, 'x86_64'), (which, '/new/tool')]:
                old = mock.return_value
                mock.return_value = value
                self.assertNotEqual(self.w.fingerprint(), original)
                mock.return_value = old

    def test_subprocess_diagnostics_discard_secrets_and_shell_is_not_used(self):
        process = Mock(pid=12345, returncode=7)
        process.communicate.return_value = (b'token=secret', None)
        with patch.object(o.subprocess, 'Popen', return_value=process) as run, \
             patch.object(o.os, 'killpg'):
            with self.assertRaisesRegex(o.Blocked, 'exit 7') as caught:
                self.w.command(['tool', 'literal;$(bad)'])
            self.assertNotIn('secret', str(caught.exception))
            self.assertNotIn('shell', run.call_args.kwargs)
            self.assertTrue(run.call_args.kwargs['start_new_session'])
            self.assertEqual(run.call_args.kwargs['stderr'], subprocess.DEVNULL)
            self.assertEqual(run.call_args.args[0], ['tool', 'literal;$(bad)'])

    def test_timeout_and_interrupt_stop_entire_process_group(self):
        for exception in (subprocess.TimeoutExpired(['tool'], 1), KeyboardInterrupt()):
            process = Mock(pid=12345)
            process.communicate.side_effect = exception
            process.wait.side_effect = [subprocess.TimeoutExpired(['tool'], 2), 0]
            with self.subTest(exception=type(exception).__name__), \
                 patch.object(o.subprocess, 'Popen', return_value=process), \
                 patch.object(o.os, 'killpg') as kill, \
                 patch.object(o.time, 'monotonic', side_effect=[0, 0, 2]):
                expected = KeyboardInterrupt if isinstance(exception, KeyboardInterrupt) else o.Blocked
                with self.assertRaises(expected):
                    self.w.command(['tool'], timeout=1)
                self.assertEqual(kill.call_args_list[0].args, (12345, o.signal.SIGTERM))
                self.assertEqual(kill.call_args_list[1].args, (12345, o.signal.SIGKILL))
                self.assertEqual(process.wait.call_count, 2)

    def test_wizard_routes_both_modes_to_separate_resumable_outputs(self):
        with patch.object(self.w, 'prerequisites'), patch.object(self.w, 'repository'), \
             patch.object(self.w, 'originals'), patch.object(self.w, 'backend'), \
             patch.object(self.w, 'fingerprint', return_value='fixture'), \
             patch.object(self.w, 'protected') as protected:
            for lua in (False, True):
                self.w.execute(lua=lua)
                args = protected.call_args_list[-2].args[0]
                self.assertIn('--onboarding-output', args)
                self.assertEqual('--lua-compiler' in args, lua)
                checker = protected.call_args.args[0]
                self.assertIn('tools/checker_regression.py', checker)
                self.assertIn('--build-dir', checker)
                selected = checker[checker.index('--build-dir') + 1]
                self.assertTrue(selected.endswith('onboarding-' + ('lua' if lua else 'normal')))
                self.assertIn('--output', checker)

    def test_regression_failure_stays_blocked_with_report_and_policy_guidance(self):
        for mode in ('normal', 'lua'):
            with self.subTest(mode=mode), patch.object(self.w, 'protected',
                    side_effect=o.Blocked('Command failed (exit 1).')):
                with self.assertRaisesRegex(o.Blocked, 'Stopped at validation'):
                    self.w.stage(8, 'validation', lambda: self.w.validate_baseline(mode))
            result = self.w.state['stages']['validation']
            self.assertEqual(result['status'], 'blocked')
            self.assertIn('.onboarding/regression-' + mode + '.json', result['reason'])
            self.assertIn('docs/checker_policy.md', result['reason'])
            self.assertIn('do not change inventories or relax checker rules', result['reason'])
            self.assertNotIn('PASS: validation', self.output.getvalue())

    def test_validation_does_not_mask_decline_timeout_or_inventory_drift(self):
        for message in ('Operation declined; nothing was authorized.',
                        'Command timed out. Local child processes stopped.',
                        'Tracked-file drift detected. STOP: inspect git diff.'):
            original = o.Blocked(message)
            with self.subTest(message=message), patch.object(self.w, 'protected', side_effect=original):
                with self.assertRaises(o.Blocked) as caught:
                    self.w.validate_baseline('lua')
                self.assertIs(caught.exception, original)

    def test_inventory_drift_stops_without_automatically_restoring_or_blessing(self):
        inventory = self.root / 'inventory.csv'
        inventory.write_text('reviewed')
        with patch.object(self.w, 'tracked', side_effect=lambda: {'inventory.csv': o.digest(inventory)}), \
             patch.object(self.w, 'consent', side_effect=lambda *a: inventory.write_text('drift')):
            with self.assertRaisesRegex(o.Blocked, 'Tracked-file drift'):
                self.w.protected(['setup'], 'Prepare')
        self.assertEqual(inventory.read_text(), 'drift')

    def test_check_mode_has_no_prompt_state_download_or_install(self):
        import bootstrap
        with patch.object(self.w, 'prerequisites'), patch.object(bootstrap, 'check_submodules', side_effect=lambda root, errors: errors.append('missing')), \
             patch.object(o.subprocess, 'Popen', side_effect=AssertionError('network/process forbidden')), \
             patch('builtins.input', side_effect=AssertionError('prompt forbidden')):
            with self.assertRaises(o.Blocked):
                self.w.execute(check=True)
        self.assertFalse(self.w.directory.exists())

    def test_plan_is_read_only_and_noninteractive_default_exits(self):
        with patch.object(o, 'Wizard', side_effect=AssertionError('must not construct')):
            self.assertEqual(o.main(['--plan']), 0)
        with patch.object(o.sys.stdin, 'isatty', return_value=False), \
             patch.object(o.subprocess, 'Popen', side_effect=AssertionError('no commands')):
            self.assertEqual(o.main([]), 2)

    def test_main_check_forces_noninteractive_even_with_tty(self):
        with patch.object(o.sys.stdin, 'isatty', return_value=True), patch.object(o, 'Wizard') as wizard:
            wizard.return_value.execute.return_value = 0
            self.assertEqual(o.main(['--check']), 0)
            wizard.assert_called_once_with(interactive=False)
            wizard.return_value.checkpoint.assert_not_called()


class MainOutcomeTests(unittest.TestCase):
    """Exercise real CLI, stage, consent and persistence flows with synthetic tools."""
    def setUp(self):
        temporary = tempfile.TemporaryDirectory()
        self.addCleanup(temporary.cleanup)
        self.root = Path(temporary.name)
        self.w = o.Wizard(self.root, interactive=True)
        self.output = io.StringIO()
        self.tracked_files = {'inventory.csv': 'original'}
        self.addCleanup(patch.stopall)
        patch.object(o.platform, 'system', return_value='Linux').start()
        patch.object(o.sys.stdin, 'isatty', return_value=True).start()
        def construct(*, interactive):
            self.w.interactive = interactive
            return self.w
        patch.object(o, 'Wizard', side_effect=construct).start()
        self.prerequisites = patch.object(self.w, 'prerequisites').start()
        patch.object(self.w, 'repository').start()
        patch.object(self.w, 'originals').start()
        self.backend = patch.object(self.w, 'backend').start()
        patch.object(self.w, 'fingerprint', return_value='current-fingerprint').start()
        patch.object(self.w, 'tracked', side_effect=lambda: self.tracked_files.copy()).start()
        self.command = patch.object(self.w, 'command', return_value='').start()
        # Any accidental escape from the synthetic command boundary is a test failure.
        patch.object(o.subprocess, 'Popen', side_effect=AssertionError('external command forbidden')).start()

    def run_main(self, arguments=(), answers=('yes', 'yes', 'yes')):
        with patch('builtins.input', side_effect=list(answers)), contextlib.redirect_stdout(self.output):
            return o.main(list(arguments))

    def state(self):
        return json.loads((self.w.directory / 'state.json').read_text())

    def assert_partial(self, mode, status, code):
        outcomes = self.state()['outcomes']
        self.assertEqual(outcomes, {'mode': mode, 'environment': 'ready', 'project_setup': 'ready',
                                   'build': 'ready', 'strict_validation': status, 'exit_code': code,
                                   'report': '.onboarding/regression-' + mode + '.json'})
        text = self.output.getvalue()
        self.assertIn('Environment: READY', text)
        self.assertIn('Baseline build (' + mode + '): READY', text)
        self.assertIn('Strict match validation (' + mode + '): ' + status.upper(), text)
        self.assertNotIn('Baseline build (' + ('normal' if mode == 'lua' else 'lua') + '): READY', text)

    def test_successful_cli_records_only_selected_mode_and_zero(self):
        self.assertEqual(self.run_main(['--lua']), 0)
        self.assert_partial('lua', 'passed', 0)
        self.assertEqual(self.state()['stages']['Validating baseline (lua)']['status'], 'passed')
        self.assertEqual(self.command.call_count, 3)
        self.assertIn('--lua-compiler', self.command.call_args_list[1].args[0])

    def test_validation_failure_records_partial_readiness_and_exit_three(self):
        self.command.side_effect = ['', '', o.Blocked('Command failed (exit 1).')]
        self.assertEqual(self.run_main([], ['yes', 'yes', 'yes', 'e']), 3)
        self.assert_partial('normal', 'blocked', 3)
        stage = self.state()['stages']['Validating baseline (normal)']
        self.assertEqual(stage['status'], 'blocked')
        self.assertIn('docs/checker_policy.md', stage['reason'])
        self.assertNotIn('PASS: Validating baseline', self.output.getvalue())

    def test_validation_decline_and_eof_are_partial_not_passed(self):
        for answer in ('no', EOFError()):
            with self.subTest(answer=repr(answer)):
                self.assertEqual(self.run_main(['--lua'], ['yes', 'yes', answer, 'e']), 3)
                self.assert_partial('lua', 'blocked', 3)
                self.assertEqual(self.state()['stages']['Validating baseline (lua)']['status'], 'blocked')
                self.assertEqual(self.command.call_count, 2)
                self.command.reset_mock()

    def test_stale_completed_checkpoint_does_not_bypass_current_checks(self):
        self.w.directory.mkdir()
        (self.w.directory / 'state.json').write_text(json.dumps({
            'schema': 1, 'fingerprint': 'stale',
            'stages': {'Checking prerequisites': {'status': 'passed'},
                       'Validating baseline (lua)': {'status': 'passed'}},
            'outcomes': {'mode': 'lua', 'build': 'ready', 'strict_validation': 'passed', 'exit_code': 0}}))
        self.prerequisites.side_effect = o.Blocked('missing current tool')
        self.assertEqual(self.run_main(['--lua'], ['e']), 1)
        self.prerequisites.assert_called_once()
        self.command.assert_not_called()
        self.assertNotIn('outcomes', self.state())
        self.assertEqual(self.state()['stages']['Checking prerequisites']['status'], 'blocked')
        self.assertNotIn('Environment: READY', self.output.getvalue())

    def test_build_failure_returns_one_without_partial_ready_summary(self):
        self.command.side_effect = ['', o.Blocked('Command failed (exit 1).')]
        self.assertEqual(self.run_main(['--lua'], ['yes', 'yes', 'e']), 1)
        self.assertNotIn('outcomes', self.state())
        self.assertEqual(self.state()['stages']['Building baseline (Lua)']['status'], 'blocked')
        self.assertNotIn('Environment: READY', self.output.getvalue())
        self.assertEqual(self.command.call_count, 2)

    def test_validation_tracked_drift_invalidates_build_and_returns_one(self):
        def command(args, timeout):
            if 'tools/checker_regression.py' in args:
                self.tracked_files['inventory.csv'] = 'unexpected modification'
            return ''
        self.command.side_effect = command
        self.assertEqual(self.run_main(['--lua']), 1)
        outcomes = self.state()['outcomes']
        self.assertEqual(outcomes['project_setup'], 'invalidated')
        self.assertEqual(outcomes['build'], 'invalidated')
        self.assertEqual(outcomes['strict_validation'], 'blocked')
        self.assertEqual(outcomes['exit_code'], 1)
        self.assertEqual(self.state()['stages']['Validating baseline (lua)']['status'], 'blocked')
        self.assertNotIn('Environment: READY', self.output.getvalue())
        self.assertIn('Tracked-file drift detected', self.output.getvalue())

    def test_validation_before_and_after_snapshot_failures_never_claim_ready(self):
        for index in (4, 5):
            for error in (o.Blocked('Command failed (exit 1).'), OSError('private path'),
                          RuntimeError('private detail'), ValueError('corrupt metadata')):
                with self.subTest(snapshot='before' if index == 4 else 'after', error=type(error).__name__):
                    self.w.tracked.side_effect = [self.tracked_files.copy() for _ in range(index)] + [error]
                    self.assertEqual(self.run_main(['--lua']), 1)
                    outcomes = self.state()['outcomes']
                    self.assertEqual(outcomes['build'], 'invalidated')
                    self.assertEqual(outcomes['project_setup'], 'invalidated')
                    self.assertEqual(outcomes['strict_validation'], 'blocked')
                    self.assertEqual(outcomes['exit_code'], 1)
                    self.assertEqual(self.state()['stages']['Validating baseline (lua)']['status'], 'blocked')
                    self.assertNotIn('Environment: READY', self.output.getvalue())
                    self.assertIn('Tracked-file integrity could not be verified', self.output.getvalue())
                    self.assertNotIn('private detail', self.output.getvalue())
                    self.assertNotIn('private path', self.output.getvalue())

    def test_snapshot_interrupt_invalidates_readiness_without_ready_summary(self):
        for index in (4, 5):
            with self.subTest(snapshot='before' if index == 4 else 'after'):
                self.w.tracked.side_effect = [self.tracked_files.copy() for _ in range(index)] + [KeyboardInterrupt()]
                self.assertEqual(self.run_main(['--lua']), 130)
                outcomes = self.state()['outcomes']
                self.assertEqual(outcomes['build'], 'invalidated')
                self.assertEqual(outcomes['project_setup'], 'invalidated')
                self.assertEqual(outcomes['strict_validation'], 'interrupted')
                self.assertEqual(outcomes['exit_code'], 130)
                self.assertEqual(self.state()['stages']['Validating baseline (lua)']['status'], 'interrupted')
                self.assertNotIn('Environment: READY', self.output.getvalue())

    def test_validation_interrupt_saves_selected_mode_outcomes_and_exit_130(self):
        self.command.side_effect = ['', '', KeyboardInterrupt()]
        self.assertEqual(self.run_main(['--lua']), 130)
        self.assert_partial('lua', 'interrupted', 130)
        self.assertEqual(self.state()['stages']['Validating baseline (lua)']['status'], 'interrupted')
        self.assertNotIn('PASS: Validating baseline', self.output.getvalue())

    def test_check_mode_selects_backend_scope_without_build_match_or_state_claims(self):
        for args, expected in [(['--check'], [()]), (['--check', '--lua'], [(), (True,)])]:
            with self.subTest(args=args):
                self.assertEqual(self.run_main(args, []), 0)
                self.assertEqual([call.args for call in self.backend.call_args_list], expected)
                self.backend.reset_mock()
                self.command.assert_not_called()
                self.assertFalse(self.w.directory.exists())
                self.assertNotIn('outcomes', self.w.state)
        self.assertIn('Project setup and baseline build were NOT run', self.output.getvalue())
        self.assertNotIn('Environment: READY', self.output.getvalue())

    def test_plan_and_lua_plan_do_not_probe_or_claim_readiness(self):
        for args in (['--plan'], ['--plan', '--lua']):
            self.assertEqual(self.run_main(args, []), 0)
        self.prerequisites.assert_not_called()
        self.backend.assert_not_called()
        self.command.assert_not_called()
        self.assertFalse(self.w.directory.exists())
        self.assertIn('--lua adds Lua readiness and selects Lua baseline', self.output.getvalue())
        self.assertNotIn('READY', self.output.getvalue())


class BuildRoutingTests(unittest.TestCase):
    def test_build_modes_have_distinct_outputs_and_always_reconfigure(self):
        import build
        with tempfile.TemporaryDirectory() as temp, patch.object(build, 'ROOT', Path(temp)), \
             patch.object(build.latepipe, 'build'), patch.object(build.shutil, 'which', return_value='/docker'), \
             patch.object(build.resource_admission, 'lease', return_value=contextlib.nullcontext()), \
             patch.object(build.resource_admission, 'check'), \
             patch.object(build, 'check', return_value=[]), \
             patch.object(build, 'compose') as normal, patch.object(build, 'lua_container') as lua:
            for extra, runner, target, setting in [(['--no-lua-compiler'], normal, 'build/onboarding-normal', 'OFF'),
                    (['--lua-compiler'], lua, 'build/onboarding-lua', 'ON')]:
                with patch.object(sys, 'argv', ['build.py', '--onboarding-output', *extra]):
                    build.main()
                configure, compile_call = runner.call_args_list
                self.assertEqual(configure.args[-2:], ('-B', target))
                self.assertIn('-DD3_LUA_COMPILER=' + setting, configure.args)
                self.assertEqual(compile_call.args, ('cmake', '--build', target, '--', '-j2'))

    def test_resource_denial_precedes_bootstrap_cleanup_and_configure(self):
        import build
        with tempfile.TemporaryDirectory() as temp, patch.object(build, 'ROOT', Path(temp)), \
             patch.object(build.latepipe, 'build'), patch.object(build.shutil, 'which', return_value='/docker'), \
             patch.object(build.resource_admission, 'lease', return_value=contextlib.nullcontext()), \
             patch.object(build.resource_admission, 'check', side_effect=build.resource_admission.AdmissionError('low space')), \
             patch.object(build, 'prepare_lua') as bootstrap, patch.object(build.shutil, 'rmtree') as remove, \
             patch.object(build, 'compose') as normal, patch.object(build, 'lua_container') as lua, \
             patch.object(sys, 'argv', ['build.py', '--lua-compiler', '--lua-archive', 'fixture', '--reconfigure']):
            with self.assertRaisesRegex(SystemExit, 'low space'):
                build.main()
            bootstrap.assert_not_called()
            remove.assert_not_called()
            normal.assert_not_called()
            lua.assert_not_called()

    def test_explicit_jobs_are_bounded_and_passed_to_builder(self):
        import build
        with tempfile.TemporaryDirectory() as temp, patch.object(build, 'ROOT', Path(temp)), \
             patch.object(build.latepipe, 'build'), patch.object(build.shutil, 'which', return_value='/docker'), \
             patch.object(build.resource_admission, 'lease', return_value=contextlib.nullcontext()), \
             patch.object(build.resource_admission, 'check'), patch.object(build, 'check', return_value=[]), \
             patch.object(build, 'compose') as runner:
            with patch.object(sys, 'argv', ['build.py', '--no-lua-compiler', '--jobs', '4']):
                build.main()
            self.assertEqual(runner.call_args.args[-2:], ('--', '-j4'))
            for jobs in ('0', '9'):
                with patch.object(sys, 'argv', ['build.py', '--jobs', jobs]):
                    with self.assertRaises(SystemExit):
                        build.main()


class PreserveInventoryTests(unittest.TestCase):
    def test_project_setup_preservation_generates_only_private_inputs(self):
        spec = importlib.util.spec_from_file_location('project_setup_for_wizard_test', Path(__file__).with_name('setup.py'))
        module = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(module)
        venv = types.ModuleType('common.setup_venv')
        venv.enter_venv = lambda: None
        with patch.dict(sys.modules, {'common.setup_venv': venv}), \
             patch.object(module, 'require_inputs'), patch.object(module, 'fetch_binaries'), \
             patch.object(module, 'link_system_objdump'), patch.object(module, 'prepare_executable'), \
             patch.object(module, 'apply_local_patches'), patch.object(module, 'install_checker'), \
             patch.object(module.subprocess, 'check_call') as command, contextlib.redirect_stdout(io.StringIO()):
            module.main(['--preserve-inventories'])
            command.assert_called_once()
            self.assertEqual(Path(command.call_args.args[0][1]).name, 'bootstrap_game_data.py')


if __name__ == '__main__':
    unittest.main()
