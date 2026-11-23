#!/usr/bin/env python3
"""Check contributor prerequisites without writes; opt in to the existing isolated runner.

No downloads, installs, builds, claims, models or services run in check/dry-run mode.
Use --toolchain-plan to preview container preparation without original inputs.
--prepare-toolchain --allow-downloads builds only the compiler image;
--run-reproduce --allow-downloads executes setup/build/check in a new checkout.
"""
import argparse
import hashlib
import importlib.util
import json
import os
from pathlib import Path
import platform
import shlex
import shutil
import subprocess
import sys

# Keep the read-only input check from creating a local bytecode cache.
sys.dont_write_bytecode = True
from verify_original_inputs import InputError, verify_inputs
from backends import check as check_backends

ROOT = Path(__file__).resolve().parents[1]
INVENTORIES = ('functions.csv', 'function_files.csv', 'data_symbols.csv',
               'duplicate_names.csv', 'inline_helpers.csv')


def sha256(path):
    digest = hashlib.sha256()
    with path.open('rb') as stream:
        for chunk in iter(lambda: stream.read(1024 * 1024), b''):
            digest.update(chunk)
    return digest.hexdigest()


def read_command(command, root):
    env = os.environ.copy()
    env['GIT_OPTIONAL_LOCKS'] = '0'
    env['GIT_NO_REPLACE_OBJECTS'] = '1'
    env['GIT_NO_LAZY_FETCH'] = '1'
    result = subprocess.run(command, cwd=root, env=env, capture_output=True,
                            text=True, timeout=20)
    if result.returncode:
        raise RuntimeError('command failed: ' + shlex.join(map(str, command)))
    return result.stdout.strip()


def git(root, *args):
    return read_command(['git', '-C', str(root), *args], root)


def check_submodules(root, errors, prefix=''):
    for line in git(root, 'ls-tree', '-r', 'HEAD').splitlines():
        meta, relative = line.split('\t', 1)
        mode, _, revision = meta.split()
        if mode != '160000':
            continue
        label = prefix + relative
        child = root / relative
        if not (child / '.git').exists():
            errors.append(label + ': uninitialized; run git submodule update --init --recursive explicitly')
        elif git(child, 'rev-parse', 'HEAD') != revision:
            errors.append(label + ': wrong commit; preserve local work, then reconcile the recorded gitlink')
        else:
            check_submodules(child, errors, label + '/')


def check_binaries(root, path):
    lock = json.loads((root / 'tools/setup_lock.json').read_text())
    if path.is_symlink() or not (path / '.git').exists():
        raise RuntimeError('--binaries must be an existing non-symlink Git checkout')
    if git(path, 'rev-parse', 'HEAD') != lock['revision']:
        raise RuntimeError('--binaries revision differs from tools/setup_lock.json')
    for relative, digest in lock['sha256'].items():
        binary = path / relative
        if (binary.is_symlink() or not binary.is_file() or
                not binary.stat().st_mode & 0o111 or sha256(binary) != digest):
            raise RuntimeError('--binaries file/hash/executable mismatch: ' + relative)


def preflight(args, root):
    errors = []
    if sys.version_info < (3, 10):
        errors.append('Python 3.10+ required by repository tools; select an installed interpreter')
    host, machine = platform.system(), platform.machine().lower()
    if host not in ('Darwin', 'Linux') or machine not in ('arm64', 'aarch64', 'x86_64', 'amd64'):
        errors.append('documented host path is macOS/Linux on ARM64 or x86_64; see docs/contributor_startup.md')
    selected = os.environ.get('D3_DOCKER_PLATFORM', 'linux/arm64')
    expected = 'linux/amd64' if machine in ('x86_64', 'amd64') else 'linux/arm64'
    if selected != expected:
        errors.append('set D3_DOCKER_PLATFORM=' + expected + ' for this host (Compose defaults to ARM64)')
    if os.environ.get('CLANG_VERSION', '5.0.1') != '5.0.1':
        errors.append('CLANG_VERSION must be 5.0.1; preserve the matching compiler policy')
    arch = {'linux/arm64': 'aarch64-linux-gnu',
            'linux/amd64': 'x86_64-linux-gnu-ubuntu-16.04'}.get(selected)
    if os.environ.get('LLVM_ARCH') and os.environ['LLVM_ARCH'] != arch:
        errors.append('LLVM_ARCH override differs from the documented platform archive; unset it for the standard recipe')
    for module in ('venv', 'ensurepip'):
        if importlib.util.find_spec(module) is None:
            errors.append('Python ' + module + ' unavailable; provision Python venv/pip support explicitly')
    for name in ('git', 'cargo', 'rustc', 'llvm-nm', 'llvm-dwarfdump', 'llvm-objdump', 'docker'):
        if shutil.which(name) is None:
            errors.append(name + ': missing from PATH; install/provision it explicitly, then rerun')
    if shutil.which('git'):
        try:
            print('Source commit: ' + git(root, 'rev-parse', 'HEAD'))
            if git(root, 'status', '--porcelain'):
                print('NOTE: working changes exist; reproduce.py uses committed HEAD only.')
            check_submodules(root, errors)
        except (OSError, RuntimeError, subprocess.TimeoutExpired) as error:
            errors.append(str(error))
    if shutil.which('docker'):
        try:
            read_command(['docker', 'compose', 'version'], root)
            if args.check_docker:
                read_command(['docker', 'info', '--format', '{{.OSType}}'], root)
        except (OSError, RuntimeError, subprocess.TimeoutExpired):
            errors.append('Docker Compose or requested daemon probe failed; check installation/context/daemon yourself')
    for name in INVENTORIES:
        if not (root / 'data/2.6.2' / name).is_file():
            errors.append('missing committed inventory: data/2.6.2/' + name)
    try:
        for row in verify_inputs(root / 'build', root=root, nso=args.nso, nss=args.nss,
                                 include_optional=False):
            if row['status'] != 'verified':
                errors.append(row['filename'] + ': ' + row['status'] +
                              '; supply your matching Switch 2.6.2 original; see docs/original_inputs.md')
            else:
                print(row['filename'] + ': pinned SHA-256 and size verified.')
                if args.hash_inputs:
                    print(row['filename'] + ' SHA-256: ' + row['sha256'])
    except InputError as error:
        errors.append(str(error))
    if args.binaries:
        try:
            check_binaries(root, args.binaries)
        except (OSError, RuntimeError, subprocess.TimeoutExpired) as error:
            errors.append(str(error))
    errors.extend(check_backends(root))
    return errors


def run_paths(args, root):
    if args.work_dir is None or args.output is None:
        raise RuntimeError('--dry-run/--run-reproduce requires explicit --work-dir NEW_DIR and --output NEW_REPORT')
    target, output = args.work_dir.resolve(), args.output.resolve()
    if target.exists() or args.work_dir.is_symlink():
        raise RuntimeError('--work-dir must not exist; existing directories are never removed or reused')
    if target == root or root in target.parents:
        raise RuntimeError('--work-dir must be outside this checkout')
    if target == output or target in output.parents or output in target.parents:
        raise RuntimeError('--output must be separate from the new checkout')
    # reproduce.py retains these paths, but can overwrite reports; refuse reuse here.
    for path in (output, Path(str(output) + '.logs'), Path(str(output) + '.regression.json'),
                 Path(str(output) + '.tmp')):
        if path.exists() or path.is_symlink():
            raise RuntimeError('--output report/log paths already exist; choose a new report name')
    return target, output


def parser():
    ap = argparse.ArgumentParser(description=__doc__)
    modes = ap.add_mutually_exclusive_group()
    modes.add_argument('--check', dest='mode', action='store_const', const='check', help='read-only prerequisite checks (default)')
    modes.add_argument('--dry-run', dest='mode', action='store_const', const='dry-run', help='check and print the isolated reproduction command; no writes')
    modes.add_argument('--run-reproduce', dest='mode', action='store_const', const='run', help='run existing setup/build/regression in a new checkout; expensive')
    modes.add_argument('--toolchain-plan', dest='mode', action='store_const', const='toolchain-plan', help='check compiler-image prerequisites and print preparation command; no originals needed')
    modes.add_argument('--prepare-toolchain', dest='mode', action='store_const', const='toolchain', help='prepare the verified LLVM Docker image only; requires --allow-downloads')
    ap.set_defaults(mode='check')
    ap.add_argument('--allow-downloads', action='store_true', help='acknowledge public tool/package/image downloads and project-local installation during reproduction')
    ap.add_argument('--nso', type=Path, help='your own ExeFS main; defaults to build/exefs/main')
    ap.add_argument('--nss', type=Path, help='your own DiabloIIINX64r.nss; defaults to build/romfs/DiabloIIINX64r.nss')
    ap.add_argument('--binaries', type=Path, help='optional pre-provisioned pinned nx-decomp-tools-binaries checkout')
    ap.add_argument('--work-dir', type=Path, help='new retained reproduction checkout outside this checkout')
    ap.add_argument('--output', type=Path, help='new private report path, outside the reproduction checkout')
    ap.add_argument('--hash-inputs', action='store_true', help='display verified input hashes (both required inputs are always hashed)')
    ap.add_argument('--check-docker', action='store_true', help='also query the existing Docker daemon read-only; never starts it')
    return ap


def main(argv=None, root=ROOT):
    args = parser().parse_args(argv)
    root = Path(root).resolve()
    if args.mode in ('toolchain-plan', 'toolchain'):
        # Lazy import keeps the normal input-check path independent and read-only.
        sys.dont_write_bytecode = True
        from bootstrap_toolchain import prepare
        return prepare(root, execute=args.mode == 'toolchain', allow_downloads=args.allow_downloads)
    args.nso = (args.nso or root / 'build/exefs/main').absolute()
    args.nss = (args.nss or root / 'build/romfs/DiabloIIINX64r.nss').absolute()
    try:
        if args.binaries:
            if args.binaries.is_symlink():
                raise RuntimeError('--binaries must not be a symlink')
            args.binaries = args.binaries.resolve()
        if args.mode == 'run' and not args.allow_downloads:
            raise RuntimeError('--run-reproduce also requires --allow-downloads; setup may fetch public tools/packages/images and install project dependencies')
        paths = run_paths(args, root) if args.mode != 'check' else None
        errors = preflight(args, root)
        if errors:
            for error in errors:
                print('BLOCKED: ' + error, file=sys.stderr)
            print('No setup/build was started. See docs/contributor_startup.md.', file=sys.stderr)
            return 1
        print('Prerequisite checks passed; this is not setup, matching or clean-clone reproducibility proof.')
        print('Docker readiness is unverified unless probed; hashes establish identity, not ownership/redistribution rights.')
        if paths:
            target, output = paths
            command = [sys.executable, str(root / 'tools/reproduce.py'), '--revision', 'HEAD',
                       '--nso', str(args.nso), '--nss', str(args.nss),
                       '--work-dir', str(target), '--output', str(output), '--run']
            if args.binaries:
                command += ['--binaries', str(args.binaries)]
            print('Plan: ' + shlex.join(command), flush=True)
            if args.mode == 'run':
                env = os.environ.copy()
                env['GIT_NO_REPLACE_OBJECTS'] = '1'
                env['GIT_NO_LAZY_FETCH'] = '1'
                return subprocess.run(command, cwd=root, env=env).returncode
        return 0
    except (OSError, ValueError, RuntimeError, subprocess.TimeoutExpired) as error:
        print('BLOCKED: ' + str(error), file=sys.stderr)
        return 1


if __name__ == '__main__':
    raise SystemExit(main())
