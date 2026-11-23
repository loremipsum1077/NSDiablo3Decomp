#!/usr/bin/env python3
"""Reproduce committed sources in a private checkout; default is read-only preflight.

Only explicit raw dumps and optional committed nx2elf binaries are imported.
Existing builds, checker executables, generated ELF files and venvs are never copied.
The report and command logs survive failures, as does the private checkout.
"""
import argparse
from collections import Counter
import csv
import hashlib
import json
import os
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile
import time

ROOT = Path(__file__).resolve().parents[1]
INVENTORIES = ('functions.csv', 'function_files.csv', 'data_symbols.csv',
               'duplicate_names.csv', 'inline_helpers.csv')


def sha256(path):
    digest = hashlib.sha256()
    with Path(path).open('rb') as stream:
        for chunk in iter(lambda: stream.read(1024 * 1024), b''):
            digest.update(chunk)
    return digest.hexdigest()


def git(root, *args):
    return subprocess.check_output(['git', '-C', str(root), *args], text=True).strip()


def inventory(root):
    result = {}
    for name in INVENTORIES:
        path = Path(root) / 'data/2.6.2' / name
        if path.is_file():
            with path.open(newline='') as stream:
                rows = list(csv.reader(stream))
            result[name] = {'sha256': sha256(path), 'rows': rows}
    return result


def compare_inventories(before, after):
    result = {}
    for name in sorted(before.keys() | after.keys()):
        a, b = before.get(name), after.get(name)
        # Compare complete rows including quality, names, sizes and duplicate counts.
        equal = bool(a and b and a['rows'][:1] == b['rows'][:1] and
                     Counter(map(tuple, a['rows'][1:])) == Counter(map(tuple, b['rows'][1:])))
        result[name] = {'equal': equal, 'before_sha256': a['sha256'] if a else None,
                        'after_sha256': b['sha256'] if b else None,
                        'before_rows': len(a['rows']) - 1 if a else None,
                        'after_rows': len(b['rows']) - 1 if b else None}
    return result


class Evidence:
    def __init__(self, output):
        self.output = Path(output).resolve()
        self.logs = self.output.with_suffix(self.output.suffix + '.logs')
        self.data = {'format': 1, 'status': 'preflight', 'commands': [],
                     'limitations': ['Shared host tools, Docker images and package caches are dependencies, not independently rebuilt toolchains.',
                                    'Inventory quality labels are retained by setup; checker regression must verify them.']}

    def save(self):
        self.output.parent.mkdir(parents=True, exist_ok=True)
        temp = self.output.with_suffix(self.output.suffix + '.tmp')
        temp.write_text(json.dumps(self.data, indent=2) + '\n')
        temp.replace(self.output)

    def run(self, stage, command, cwd, env=None):
        self.logs.mkdir(parents=True, exist_ok=True)
        log = self.logs / f'{len(self.data["commands"]):03d}-{stage}.log'
        entry = {'stage': stage, 'argv': list(map(str, command)), 'cwd': str(cwd),
                 'log': str(log), 'started': time.time(), 'returncode': None}
        self.data['commands'].append(entry)
        self.data['stage'] = stage
        self.save()
        try:
            with log.open('w') as stream:
                run = subprocess.run(entry['argv'], cwd=cwd, env=env,
                                     stdout=stream, stderr=subprocess.STDOUT)
            entry['returncode'] = run.returncode
            if run.returncode:
                raise RuntimeError(f'{stage} failed ({run.returncode}); see {log}')
        finally:
            entry['finished'] = time.time()
            if log.is_file():
                entry['log_sha256'] = sha256(log)
            self.save()


def clone_tree(source, target, revision, evidence, label='source'):
    evidence.run('snapshot', ['git', 'clone', '--no-hardlinks', '--no-checkout', str(source), str(target)], source)
    evidence.run('snapshot', ['git', 'checkout', '--detach', revision], target)
    evidence.data.setdefault('repositories', []).append({'name': label, 'revision': revision})
    # Populate each gitlink from the local repository, never its patched worktree.
    entries = git(target, 'ls-tree', '-r', revision).splitlines()
    for entry in entries:
        metadata, relative = entry.split('\t', 1)
        mode, kind, pinned = metadata.split()
        if mode == '160000':
            local = Path(source) / relative
            if not (local / '.git').exists():
                raise RuntimeError(f'Local pinned submodule unavailable: {local}; initialize it first')
            clone_tree(local, Path(target) / relative, pinned, evidence, label + '/' + relative)



def probe(command, cwd):
    """Best-effort dependency metadata; unavailable metadata is explicit."""
    try:
        result = subprocess.run(command, cwd=cwd, text=True, capture_output=True, timeout=20)
        return {'argv': command, 'returncode': result.returncode,
                'stdout': result.stdout, 'stderr': result.stderr}
    except (OSError, subprocess.TimeoutExpired) as error:
        return {'argv': command, 'error': str(error)}


def checker_sources(root):
    paths = (root / 'tools/patches').glob('*.patch')
    result = {str(p.relative_to(root)): sha256(p) for p in paths}
    common = root / 'tools/common'
    if common.is_dir():
        for name in git(common, 'ls-files').splitlines():
            path = common / name
            if path.is_file():
                result[str(path.relative_to(root))] = sha256(path)
    for path in (common / 'viking/src').rglob('*.rs'):
        result[str(path.relative_to(root))] = sha256(path)
    for name in ('Cargo.toml', 'Cargo.lock'):
        path = common / 'viking' / name
        if path.is_file():
            result[str(path.relative_to(root))] = sha256(path)
    return result


def parser():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--output', type=Path, required=True)
    ap.add_argument('--nso', type=Path, required=True, help='Independently supplied ExeFS main')
    ap.add_argument('--nss', type=Path, required=True, help='Independently supplied DiabloIIINX64r.nss')
    ap.add_argument('--revision', default='HEAD', help='Committed source revision (working changes excluded)')
    ap.add_argument('--run', action='store_true', help='Execute setup, build, and regression in a private checkout')
    ap.add_argument('--work-dir', type=Path, help='New, nonexistent private checkout directory; retained on completion')
    ap.add_argument('--binaries', type=Path, help='Optional local nx-decomp-tools-binaries Git repository, snapshotted at HEAD')
    ap.add_argument('--python', default=sys.executable)
    ap.add_argument('--use-existing-python', action='store_true', help='Use supplied Python environment instead of installing a clean venv; disclosed in report')
    return ap


def main(argv=None, root=ROOT):
    args = parser().parse_args(argv)
    root = Path(root).resolve()
    evidence = Evidence(args.output)
    before_live = {}
    try:
        before_live = inventory(root)
        revision = git(root, 'rev-parse', '--verify', args.revision + '^{commit}')
        evidence.data.update({'source': str(root), 'revision': revision,
                              'runner_sha256': sha256(Path(__file__)),
                              'working_changes_excluded': bool(git(root, 'status', '--porcelain')),
                              'python_environment': 'existing' if args.use_existing_python else 'fresh-venv',
                              'inputs': {}, 'tools': {}, 'planned_stages': ['snapshot', 'setup', 'inventory', 'build', 'regression']})
        for name in ('nso', 'nss'):
            path = getattr(args, name).resolve()
            evidence.data['inputs'][name] = {'path': str(path), 'sha256': sha256(path), 'bytes': path.stat().st_size}
        for name in ('git', 'docker', 'cargo', 'rustc', 'llvm-nm', 'llvm-dwarfdump', 'llvm-objdump'):
            path = shutil.which(name)
            evidence.data['tools'][name] = {'path': path, 'sha256': sha256(path) if path else None}
        python = shutil.which(args.python)
        if not python:
            raise RuntimeError(f'Python executable unavailable: {args.python}')
        python = os.path.abspath(python)
        evidence.data['tools']['python'] = {'path': python, 'sha256': sha256(python)}
        evidence.data['build_environment'] = {key: os.environ.get(key) for key in ('CLANG_VERSION', 'D3_DOCKER_PLATFORM', 'LLVM_ARCH', 'CARGO_HOME', 'CARGO_NET_OFFLINE', 'PIP_NO_INDEX', 'PIP_FIND_LINKS')}
        if args.binaries:
            evidence.data['binaries_revision'] = git(args.binaries.resolve(), 'rev-parse', 'HEAD')
        evidence.save()
        if args.run:
            if args.work_dir:
                target = args.work_dir.resolve()
                if target.exists():
                    raise RuntimeError('--work-dir must not exist (no existing artifacts permitted)')
                if target == root or root in target.parents:
                    raise RuntimeError('--work-dir must be outside the source checkout')
                target.parent.mkdir(parents=True, exist_ok=True)
            else:
                target = Path(tempfile.mkdtemp(prefix='d3-reproduce-')) / 'checkout'
            if target == evidence.output or target in evidence.output.parents:
                raise RuntimeError('Evidence output must be outside the new checkout')
            evidence.data['checkout'] = str(target)
            evidence.data['status'] = 'running'
            clone_tree(root, target, revision, evidence)
            # Reject committed artifacts too: they must not provide a false clean proof.
            forbidden = ['build/2.6.2', 'tools/check', 'tools/common/.venv', 'data/2.6.2/main.elf', 'tools/common/viking/target', 'build/checker-target', 'build/dwarf']
            for relative in forbidden:
                if (target / relative).exists():
                    raise RuntimeError(f'Committed generated artifact blocks clean proof: {relative}')
            baseline = inventory(target)
            missing = sorted(set(INVENTORIES) - baseline.keys())
            if missing:
                raise RuntimeError(f'Committed baseline is missing required inventories: {missing}')
            for name, relative in [('nso', 'build/exefs/main'), ('nss', 'build/romfs/DiabloIIINX64r.nss')]:
                destination = target / relative
                destination.parent.mkdir(parents=True, exist_ok=True)
                shutil.copyfile(getattr(args, name).resolve(), destination)
                if sha256(destination) != evidence.data['inputs'][name]['sha256']:
                    raise RuntimeError(f'{name} changed while copying')
            if args.binaries:
                clone_tree(args.binaries.resolve(), target / 'toolchain/nx-decomp-tools-binaries',
                           evidence.data['binaries_revision'], evidence, 'binaries')
            env = os.environ.copy()
            env.pop('NX_DECOMP_TOOLS_IN_VENV', None)
            evidence.data['disabled_compiler_wrappers'] = {key: env.pop(key)
                for key in ('RUSTC_WRAPPER', 'RUSTC_WORKSPACE_WRAPPER') if key in env}
            # Empty values override wrappers configured in shared Cargo config.
            env['RUSTC_WRAPPER'] = ''
            env['RUSTC_WORKSPACE_WRAPPER'] = ''
            env['CARGO_TARGET_DIR'] = str(target / 'build/checker-target')
            evidence.data['cargo_target_dir'] = env['CARGO_TARGET_DIR']
            if args.use_existing_python:
                env['NX_DECOMP_TOOLS_IN_VENV'] = '1'
            evidence.data['dependency_versions'] = {name: probe([name, '--version'], target)
                for name in ('git', 'cargo', 'rustc', 'docker', 'llvm-nm', 'llvm-dwarfdump')}
            evidence.data['python_packages'] = probe([python, '-m', 'pip', 'freeze'], target)
            lock = target / 'tools/common/viking/Cargo.lock'
            evidence.data['cargo_lock_before_sha256'] = sha256(lock) if lock.is_file() else None
            try:
                evidence.run('setup', [python, 'tools/setup.py', 'build/exefs/main'], target, env)
            finally:
                evidence.data['cargo_lock_after_sha256'] = sha256(lock) if lock.is_file() else None
                evidence.data['checker_sources_sha256'] = checker_sources(target)
                effective_python = python if args.use_existing_python else str(target / 'tools/common/.venv/bin/python')
                evidence.data['effective_python_packages'] = probe([effective_python, '-m', 'pip', 'freeze'], target)
            evidence.data['stage'] = 'inventory'
            comparison = compare_inventories(baseline, inventory(target))
            evidence.data['inventory_comparison'] = comparison
            evidence.save()
            if not comparison or not all(row['equal'] for row in comparison.values()):
                raise RuntimeError('Regenerated inventories differ from committed baseline')
            try:
                evidence.run('build', [python, 'tools/build.py'], target, env)
            finally:
                evidence.data['docker_image'] = probe(['docker', 'image', 'inspect',
                    'd3-build:' + env.get('CLANG_VERSION', '5.0.1')], target)
            evidence.data['artifacts_sha256'] = {relative: sha256(target / relative) for relative in
                ('tools/check', 'build/2.6.2/d3', 'build/2.6.2/d3.map', 'build/2.6.2/d3.map.provenance')}
            regression = evidence.output.with_suffix(evidence.output.suffix + '.regression.json')
            try:
                evidence.run('regression', [python, 'tools/checker_regression.py', '--output', str(regression)], target, env)
            finally:
                if regression.is_file():
                    evidence.data['regression'] = {'path': str(regression), 'sha256': sha256(regression)}
            evidence.data['status'] = 'passed'
    except (Exception, KeyboardInterrupt) as error:
        evidence.data.update(status='failed', failure_stage=evidence.data.get('stage', 'preflight'), error=str(error))
    finally:
        evidence.data['live_inventory_comparison'] = compare_inventories(before_live, inventory(root))
        if any(not row['equal'] or row['before_sha256'] != row['after_sha256']
               for row in evidence.data['live_inventory_comparison'].values()):
            evidence.data.update(status='failed', failure_stage='live-inventory', error='Live inventories changed during run; evidence invalid')
        evidence.save()
        print(f'{evidence.data["status"]}: {evidence.output}')
    return int(evidence.data['status'] == 'failed')


if __name__ == '__main__':
    raise SystemExit(main())
