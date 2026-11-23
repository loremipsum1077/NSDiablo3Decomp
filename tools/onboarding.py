"""Small deterministic onboarding driver. Receipts are history, never authority."""
import argparse
import contextlib
import hashlib
import importlib.util
import json
import os
from pathlib import Path
import platform
import shlex
import signal
import shutil
import subprocess
import sys
import tempfile
import time

ROOT = Path(__file__).resolve().parents[1]
sys.dont_write_bytecode = True
sys.path.insert(0, str(ROOT / 'tools'))
from verify_original_inputs import require_inputs


class Blocked(RuntimeError):
    pass


class IntegrityBlocked(Blocked):
    """Tracked drift invalidates readiness; it cannot be reported as partial success."""


def digest(path):
    h = hashlib.sha256()
    with path.open('rb') as stream:
        for block in iter(lambda: stream.read(1024 * 1024), b''):
            h.update(block)
    return h.hexdigest()


class Wizard:
    def __init__(self, root=ROOT, interactive=False):
        self.root = Path(root)
        self.interactive = interactive
        self.directory = self.root / '.onboarding'
        self.state = {'schema': 1, 'stages': {}}
        self.writable = False
        self.current = None

    def say(self, text):
        print(text, flush=True)

    def ask(self, prompt):
        if not self.interactive:
            return 'exit'
        try:
            return input(prompt + ' ').strip().lower()
        except EOFError:
            return 'exit'

    def command(self, args, timeout=30):
        # Raw subprocess output may contain credentials or private paths. Never
        # print or persist it. Record only the fixed command label and exit code.
        env = os.environ.copy()
        env.update(PYTHONDONTWRITEBYTECODE='1', GIT_OPTIONAL_LOCKS='0',
                   GIT_NO_LAZY_FETCH='1', GIT_NO_REPLACE_OBJECTS='1')
        self.say('  Checking/running: ' + shlex.join(args))
        process = None
        try:
            process = subprocess.Popen(args, cwd=self.root, env=env,
                                       stdout=subprocess.PIPE, stderr=subprocess.DEVNULL,
                                       start_new_session=True)
            deadline = time.monotonic() + timeout
            while True:
                try:
                    output, _ = process.communicate(timeout=min(15, max(0.01, deadline - time.monotonic())))
                    break
                except subprocess.TimeoutExpired:
                    if time.monotonic() >= deadline:
                        raise Blocked('Command timed out. Local child processes stopped; inspect Docker for daemon-side work before retrying.')
                    self.say('  Still working; waiting for the command to finish...')
            if process.returncode:
                raise Blocked('Command failed (exit %d). Run the displayed command locally for detailed diagnostics.' % process.returncode)
            return output.decode('utf-8', errors='replace').strip()
        except OSError:
            raise Blocked('Command unavailable or access denied; verify PATH and permissions.') from None
        finally:
            if process is not None:
                # Always reap the whole local process group, including descendants
                # of wrappers. Docker daemon-side work must be checked separately.
                try:
                    os.killpg(process.pid, signal.SIGTERM)
                except ProcessLookupError:
                    pass
                try:
                    process.wait(timeout=2)
                except subprocess.TimeoutExpired:
                    pass
                try:
                    os.killpg(process.pid, signal.SIGKILL)
                except ProcessLookupError:
                    pass
                process.wait()

    def git(self, *args):
        return self.command(['git', *args])

    def tracked(self):
        names = self.git('ls-files', '-z').split('\0')
        return {name: digest(self.root / name) if (self.root / name).is_file() else 'missing'
                for name in names if name and not (self.root / name).is_dir()}

    def fingerprint(self):
        data = {'files': self.tracked(), 'head': self.git('rev-parse', 'HEAD'),
                'platform': [platform.system(), platform.machine()],
                'python': sys.version, 'tools': {n: shutil.which(n) for n in
                ('git', 'cargo', 'rustc', 'docker', 'llvm-nm', 'llvm-dwarfdump', 'llvm-objdump')},
                'selection': {n: os.environ.get(n, '') for n in
                ('D3_DOCKER_PLATFORM', 'CLANG_VERSION', 'LLVM_ARCH', 'CLANG_URL', 'D3_LUA_TOOLCHAIN')}}
        return hashlib.sha256(json.dumps(data, sort_keys=True).encode()).hexdigest()

    def save(self):
        if not self.writable:
            return
        fd, name = tempfile.mkstemp(prefix='state-', dir=self.directory)
        try:
            with os.fdopen(fd, 'w') as stream:
                json.dump(self.state, stream, indent=2)
                stream.flush()
                os.fsync(stream.fileno())
            os.replace(name, self.directory / 'state.json')
        finally:
            if os.path.exists(name):
                os.unlink(name)

    @contextlib.contextmanager
    def checkpoint(self):
        import fcntl
        if self.directory.is_symlink():
            raise Blocked('State directory must not be a symlink.')
        self.directory.mkdir(mode=0o700, exist_ok=True)
        lock_path = self.directory / 'lock'
        fd = os.open(lock_path, os.O_CREAT | os.O_RDWR | os.O_NOFOLLOW, 0o600)
        with os.fdopen(fd, 'w') as lock:
            try:
                fcntl.flock(lock, fcntl.LOCK_EX | fcntl.LOCK_NB)
            except BlockingIOError:
                raise Blocked('Another wizard is running in this checkout.') from None
            path = self.directory / 'state.json'
            try:
                if path.is_symlink() or path.stat().st_size > 1024 * 1024:
                    raise ValueError()
                old = json.loads(path.read_text())
                if not isinstance(old, dict) or old.get('schema') != 1 or not isinstance(old.get('stages'), dict):
                    raise ValueError()
                self.say('Checkpoint found. Rechecking all current facts; previous passes never skip validation.')
            except FileNotFoundError:
                pass
            except (ValueError, OSError):
                self.say('Unreadable checkpoint ignored. Starting fresh checks; existing outputs are preserved.')
            self.writable = True
            self.save()
            try:
                yield
            finally:
                self.save()
                self.writable = False

    def stage(self, number, label, action, optional=False):
        self.current = label
        self.say('\nStep %d: %s' % (number, label))
        while True:
            self.state['stages'][label] = {'status': 'running'}
            self.save()
            try:
                action()
            except (Blocked, RuntimeError, OSError, ValueError) as error:
                # Only our controlled diagnostics are safe; imported errors aren't.
                reason = str(error) if isinstance(error, Blocked) else 'Validation failed; consult the documented command and retry.'
                self.state['stages'][label] = {'status': 'blocked', 'reason': reason}
                self.save()
                self.say('BLOCKED: ' + reason)
                if isinstance(error, IntegrityBlocked):
                    raise
                choice = self.ask('[r]etry / ' + ('[s]kip optional / ' if optional else '') + '[e]xit:')
                if choice in ('r', 'retry'):
                    continue
                if optional and choice in ('s', 'skip'):
                    self.state['stages'][label]['status'] = 'skipped'
                    self.save()
                    return False
                raise Blocked('Stopped at ' + label + '; fix the issue and rerun python3 setup.py.')
            self.state['stages'][label] = {'status': 'passed', 'checked_at': int(time.time())}
            self.save()
            self.say('PASS: ' + label)
            return True

    def consent(self, args, purpose, timeout=7200):
        self.say(purpose + '\n  ' + shlex.join(args))
        if self.ask('Run this operation? [yes/no]:') != 'yes':
            raise Blocked('Operation declined; nothing was authorized.')
        self.command(args, timeout)

    def prerequisites(self):
        host, machine = platform.system(), platform.machine().lower()
        expected = {'arm64': 'linux/arm64', 'aarch64': 'linux/arm64',
                    'x86_64': 'linux/amd64', 'amd64': 'linux/amd64'}.get(machine)
        if host not in ('Darwin', 'Linux') or not expected:
            raise Blocked('Unsupported host: use macOS or Linux on ARM64/x86_64. See docs/contributor_startup.md.')
        if os.environ.get('D3_DOCKER_PLATFORM', 'linux/arm64') != expected:
            raise Blocked('Docker platform differs from host. Set D3_DOCKER_PLATFORM=' + expected + ' in this terminal and retry.')
        if sys.version_info < (3, 10):
            raise Blocked('Python 3.10+ required; verify python3 --version.')
        for module in ('venv', 'ensurepip'):
            if importlib.util.find_spec(module) is None:
                raise Blocked('Python ' + module + ' missing. Provision Python with venv/pip support explicitly.')
        for name in ('git', 'rustc', 'cargo', 'llvm-nm', 'llvm-dwarfdump', 'llvm-objdump', 'docker'):
            if not shutil.which(name):
                hint = ('On macOS, check your installed tool bin directories: Rust commonly uses ~/.cargo/bin and Homebrew LLVM uses /opt/homebrew/opt/llvm/bin.' if host == 'Darwin' else 'On Linux, check your distribution LLVM/Rust/Git packages and installed bin directories.')
                raise Blocked(name + ' missing from PATH. ' + hint + ' Add existing tools to this terminal PATH or install explicitly. Verify: ' + name + ' --version')
            self.command([name, '--version'])
            self.say('  available: ' + name)
        self.say('  docker compose version; docker info (Linux daemon required)')
        self.command(['docker', 'compose', 'version'])
        if self.command(['docker', 'info', '--format', '{{.OSType}}']) != 'linux':
            raise Blocked('Docker must use a running Linux daemon. Verify docker context show and docker info; start/configure Docker explicitly.')

    def repository(self):
        from bootstrap import check_submodules
        errors = []
        check_submodules(self.root, errors)
        if errors:
            self.say('Pinned submodules missing or mismatched. Preserve local changes before reconciling pins.')
            self.consent(['git', 'submodule', 'update', '--init', '--recursive'], 'Download/initialize pinned public submodules.')
            errors = []
            check_submodules(self.root, errors)
            if errors:
                raise Blocked('Submodule pins still differ; inspect git submodule status --recursive.')
        from checker_patches import INCORPORATED_PATCHES
        for name, expected in INCORPORATED_PATCHES.items():
            if digest(self.root / 'tools/patches' / name) != expected:
                raise Blocked('Checker patch pin mismatch. Preserve changes and reconcile tools/patches.')
        for name in ('functions', 'function_files', 'data_symbols', 'duplicate_names', 'inline_helpers', 'helper_ledger'):
            if not (self.root / ('data/2.6.2/' + name + '.csv')).is_file():
                raise Blocked('Reviewed inventory missing; restore it from the agreed repository revision.')

    def originals(self):
        self.say('Supply your own Switch 2.6.2 dump locally:\n  build/exefs/main\n  build/romfs/DiabloIIINX64r.nss\nVerify: python3 tools/verify_original_inputs.py. No game assets are downloaded.')
        try:
            rows = require_inputs(self.root / 'build', root=self.root)
        except RuntimeError:
            raise Blocked('Originals missing, unreadable, or checksum/size mismatch. Place matching regular files at the paths above, then retry.') from None
        self.state['inputs'] = {row['id']: row['sha256'] for row in rows}
        from game_data_manifest import MANIFEST, generated_files
        pins = json.loads((self.root / MANIFEST).read_text())
        def generated_ready():
            return all((self.root / name).is_file() and digest(self.root / name) == expected
                       for name, expected in generated_files(pins).items())
        if not generated_ready():
            args = [sys.executable, 'tools/bootstrap_game_data.py', 'build', '--build-protoc']
            if not self.interactive:
                raise Blocked('Private build inputs missing or stale. Run ' + shlex.join(args))
            self.consent(args, 'Generate private build inputs and build the pinned public protoc if needed.')
            if not generated_ready():
                raise Blocked('Generated game inputs still differ from the reviewed hashes.')

    def backend(self, lua=False):
        from backends import check
        errors = check(self.root, lua=lua)
        if errors:
            self.say('Compiler missing, stale, corrupt, or unavailable; strict provenance checks did not pass.')
            args = [sys.executable, 'tools/backends.py', '--prepare-lua', '--allow-downloads'] if lua else [sys.executable, 'tools/bootstrap.py', '--prepare-toolchain', '--allow-downloads']
            self.consent(args, 'Download pinned public compiler sources/images and build compiler (may take hours).')
            if check(self.root, lua=lua):
                raise Blocked('Compiler still not ready. Run python3 tools/backends.py --check; preserve old caches and evidence.')

    def invalidate_readiness(self):
        if 'outcomes' in self.state:
            self.state['outcomes'].update(project_setup='invalidated', build='invalidated',
                                          strict_validation='blocked', exit_code=1)
            self.save()

    def tracked_snapshot(self):
        try:
            return self.tracked()
        except KeyboardInterrupt:
            self.invalidate_readiness()
            raise
        except (Blocked, OSError, RuntimeError, ValueError):
            self.invalidate_readiness()
            raise IntegrityBlocked('Tracked-file integrity could not be verified. '
                                   'Check Git access and file permissions; readiness is not certified.') from None

    def protected(self, args, purpose):
        before = self.tracked_snapshot()
        try:
            self.consent(args, purpose)
        finally:
            if self.tracked_snapshot() != before:
                self.invalidate_readiness()
                raise IntegrityBlocked('Tracked-file drift detected. STOP: inspect git diff; preserve your edits and generated output. No automatic restore or inventory acceptance was performed.')

    def validate_baseline(self, mode):
        try:
            self.protected(
                [sys.executable, 'tools/checker_regression.py', '--build-dir', 'build/onboarding-' + mode,
                 '--output', '.onboarding/regression-' + mode + '.json'],
                'Check every non-U inventory identity against the fresh linked ELF in an isolated snapshot.')
        except Blocked as error:
            if str(error).startswith('Command failed'):
                raise Blocked('Baseline regression failed. Inspect the generated .onboarding/regression-' +
                              mode + '.json report (if produced) and docs/checker_policy.md. '
                              'Inventory O labels are not fresh proof. Preserve the report; '
                              'do not change inventories or relax checker rules to pass setup.') from None
            raise

    def validation_summary(self, status, exit_code):
        outcomes = self.state['outcomes']
        outcomes.update(strict_validation=status, exit_code=exit_code)
        self.save()
        self.say('\nEnvironment: READY')
        self.say('Project setup: READY')
        self.say('Baseline build (' + outcomes['mode'] + '): READY — built in this invocation')
        self.say('Strict match validation (' + outcomes['mode'] + '): ' + status.upper())
        self.say('Report (if produced by this attempt): ' + outcomes['report'])
        if status != 'passed':
            self.say('Setup/build readiness does not certify matches. Preserve the failure report; '
                     'contribution acceptance still requires strict checks. Earlier reports are not fresh proof.')
        else:
            self.say('Inventory regression passed; contribution-specific source/data/unwind review remains required.')
        self.say('Exit code: ' + str(exit_code))

    def execute(self, check=False, lua=False):
        self.state.pop('outcomes', None)
        self.say('NSD3Decomp contributor setup | local, deterministic, no AI\nReadiness checks are rerun on every launch. No previous build is treated as proof.')
        self.stage(1, 'Checking prerequisites', self.prerequisites)
        self.stage(2, 'Checking repository pins and inventories', self.repository)
        self.state['fingerprint'] = self.fingerprint()
        self.stage(3, 'Verifying private originals', self.originals)
        self.stage(4, 'Checking normal compiler', self.backend)
        if lua:
            self.stage(5, 'Checking optional Lua compiler', lambda: self.backend(True))
        if check:
            self.say('\nReadiness passed. Project setup and baseline build were NOT run.')
            return 0
        self.stage(6, 'Preparing project', lambda: self.protected(
            [sys.executable, 'tools/setup.py', '--preserve-inventories'],
            'Install project Python/Cargo dependencies, convert originals and build checker; preserve reviewed inventories.'))
        # Mode-specific outputs preserve normal/Lua separation. Always rebuild all
        # objects: changed image bytes at a stable compiler path evade CMake mtimes.
        command = [sys.executable, 'tools/build.py', '--onboarding-output', '--clean'] + (['--lua-compiler'] if lua else ['--no-lua-compiler'])
        self.stage(7, 'Building baseline (' + ('Lua' if lua else 'normal') + ')',
                   lambda: self.protected(command, 'Run actual baseline compilation; this is not a matching-quality claim.'))
        mode = 'lua' if lua else 'normal'
        self.state['outcomes'] = {
            'mode': mode, 'environment': 'ready', 'project_setup': 'ready', 'build': 'ready',
            'strict_validation': 'not_run', 'report': '.onboarding/regression-' + mode + '.json',
        }
        self.save()
        try:
            self.stage(8, 'Validating baseline (' + mode + ')', lambda: self.validate_baseline(mode))
        except IntegrityBlocked:
            raise
        except Blocked:
            self.validation_summary('blocked', 3)
            return 3
        self.validation_summary('passed', 0)
        return 0


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__, epilog=(
        'Exit codes: 0 = requested workflow passed (help/plan/check do not certify builds or matches); '
        '1 = setup/build or tracked-file integrity failure; 2 = invalid usage/non-TTY invocation; '
        '3 = setup/build ready, strict validation blocked or declined; 130 = interrupted. '
        'Even a successful inventory regression is not full contribution acceptance.'))
    group = parser.add_mutually_exclusive_group()
    group.add_argument('--check', action='store_true', help='read-only readiness checks; no prompts, downloads, state or builds')
    group.add_argument('--plan', action='store_true', help='print stages without running commands or creating state')
    parser.add_argument('--lua', action='store_true', help='also require Lua backend and build into a separate Lua baseline directory')
    args = parser.parse_args(argv)
    if args.plan:
        print('1 Prerequisites → 2 Pins/inventories → 3 Private originals and generated inputs → 4 Compiler readiness → 6 Project setup → 7 Baseline build → 8 Inventory regression\nEvery write/download/build needs interactive consent. --lua adds Lua readiness and selects Lua baseline.\nAdvanced commands: tools/bootstrap.py, tools/backends.py, tools/verify_original_inputs.py, tools/setup.py, tools/build.py')
        return 0
    if platform.system() not in ('Linux', 'Darwin'):
        print('Unsupported host: use macOS or Linux on ARM64/x86_64.')
        return 1
    wizard = Wizard(interactive=not args.check and sys.stdin.isatty())
    try:
        if args.check:
            return wizard.execute(check=True, lua=args.lua)
        if not wizard.interactive:
            print('Noninteractive input: use --check or --plan. No consent inferred.')
            return 2
        with wizard.checkpoint():
            try:
                return wizard.execute(lua=args.lua)
            except KeyboardInterrupt:
                if wizard.current:
                    wizard.state['stages'][wizard.current] = {'status': 'interrupted'}
                if wizard.state.get('outcomes', {}).get('build') == 'ready':
                    wizard.validation_summary('interrupted', 130)
                elif 'outcomes' in wizard.state:
                    wizard.state['outcomes'].update(strict_validation='interrupted', exit_code=130)
                    wizard.save()
                wizard.say('\nInterrupted. Checkpoint saved; local children stopped. Inspect Docker for daemon-side work before retrying.')
                return 130
    except (Blocked, OSError, RuntimeError, ValueError) as error:
        print('\nSetup incomplete: ' + (str(error) if isinstance(error, Blocked) else 'Local validation failed; check permissions and documented commands.'))
        return 1
