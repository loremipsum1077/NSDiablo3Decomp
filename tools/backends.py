#!/usr/bin/env python3
"""Check compiler readiness without writes; explicitly prepare or adopt a Lua cache."""
import argparse
import json
import os
from pathlib import Path
import platform
import shlex
import shutil
import subprocess
import sys
import tempfile

sys.dont_write_bytecode = True
ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'toolchain'))
import backend
import image_provenance
import llvm_archive

NORMAL_REPAIR = 'python3 tools/bootstrap.py --prepare-toolchain --allow-downloads'
LUA_REPAIR = 'python3 tools/backends.py --prepare-lua --allow-downloads'


def configuration():
    selected = os.environ.get('D3_DOCKER_PLATFORM', 'linux/arm64')
    expected = {'arm64': 'linux/arm64', 'aarch64': 'linux/arm64',
                'x86_64': 'linux/amd64', 'amd64': 'linux/amd64'}.get(platform.machine().lower())
    if platform.system() not in ('Linux', 'Darwin') or not expected:
        raise backend.BackendError('unsupported', 'use macOS/Linux ARM64 or x86_64')
    if selected != expected:
        raise backend.BackendError('unsupported', 'export D3_DOCKER_PLATFORM=' + expected)
    try:
        llvm_archive.select(selected, os.environ.get('CLANG_VERSION', '5.0.1'),
                            os.environ.get('LLVM_ARCH', ''), os.environ.get('CLANG_URL', ''))
    except (ValueError, RuntimeError) as error:
        raise backend.BackendError('unsupported', str(error)) from error
    return selected


def image_check(root, tag):
    selected = configuration()
    inspect = subprocess.run(['docker', 'image', 'inspect', tag], capture_output=True, text=True, timeout=20)
    if inspect.returncode:
        raise backend.BackendError('missing', 'image/daemon unavailable: ' + tag)
    info = json.loads(inspect.stdout)[0]
    if info['Os'] != 'linux' or info['Architecture'] != selected.split('/')[1]:
        raise backend.BackendError('unsupported', 'image platform mismatch: ' + tag)
    image = info['Id']
    # Existing image only, no mount, network, or writable root. Check actual bytes,
    # not just a version string or the existence of a self-generated receipt.
    result = subprocess.run(['docker', 'run', '--rm', '--pull', 'never', '--network', 'none',
                             '--read-only', '-v', f'{root / "toolchain/image_provenance.py"}:/probe.py:ro',
                             image, 'python3', '-B', '/probe.py'],
                            capture_output=True, text=True, timeout=120)
    if result.returncode:
        status = 'stale' if ("can't open file" in result.stderr or "No such file or directory" in result.stderr) else 'corrupted'
        raise backend.BackendError(status, 'compiler extraction provenance unavailable/invalid: ' + tag)
    expected = {name: backend.sha(root / 'toolchain' / name) for name in image_provenance.FILES}
    if json.loads(result.stdout) != expected:
        raise backend.BackendError('stale', 'compiler image recipe differs: ' + tag)
    return image


def check(root=ROOT, lua=True, cache=None):
    errors = []
    images = {}

    def image_ready(tag):
        images[tag] = image_check(root, tag)

    def lua_ready():
        selected = backend.resolve(root, cache)
        receipt = backend.validate(root, selected)
        image = images.get('d3-lua-build:5.0.1')
        if image is None:
            raise backend.BackendError('unsupported', 'prepare the Lua image before checking executable runtime')
        result = subprocess.run(['docker', 'run', '--rm', '--pull', 'never', '--network', 'none',
                                 '--read-only', '-v', f'{selected.absolute()}:/backend:ro', image,
                                 '/backend/out/bin/llc', '--version'],
                                capture_output=True, text=True, timeout=20)
        if result.returncode or result.stdout != receipt['llc_version']:
            raise backend.BackendError('unsupported', 'Lua executable/runtime version probe failed')
    checks = [('normal', lambda: image_ready('d3-build:5.0.1'), NORMAL_REPAIR)]
    if lua:
        checks += [('Lua image', lambda: image_ready('d3-lua-build:5.0.1'), LUA_REPAIR),
                   ('Lua backend', lua_ready, LUA_REPAIR)]
    for label, action, repair in checks:
        try:
            action()
            print(label + ': ready')
        except (backend.BackendError, OSError, ValueError, subprocess.TimeoutExpired) as error:
            if label == 'Lua backend':
                path = backend.resolve(root, cache)
                if cache is not None or os.environ.get('D3_LUA_TOOLCHAIN'):
                    repair = 'unset D3_LUA_TOOLCHAIN; ' + LUA_REPAIR + '; select the new recipe cache explicitly; preserve the old reference'
                elif path.exists():
                    saved = shlex.quote(str(path) + '.preserved')
                    repair = ('test ! -e ' + saved + ' && mv ' + shlex.quote(str(path)) + ' ' +
                              saved + ' && ' + LUA_REPAIR)
            message = label + ': ' + str(error) + '; repair: ' + repair
            errors.append(message)
    return errors


def adopt(root, source):
    """Copy only verified compiler/receipt bytes; never relabel a historical build."""
    configuration()
    source = source.absolute()
    backend.validate(root, source)
    destination = backend.cache_path(root)
    if destination.exists():
        backend.validate(root, destination)
        if (backend.sha(destination / 'receipt.json') != backend.sha(source / 'receipt.json')):
            raise backend.BackendError('corrupted', 'recipe cache already occupied by different evidence; preserve both')
        return destination
    destination.parent.mkdir(parents=True, exist_ok=True)
    with tempfile.TemporaryDirectory(prefix='.adopt-', dir=destination.parent) as temp:
        stage = Path(temp) / 'cache'
        (stage / 'out/bin').mkdir(parents=True)
        shutil.copy2(source / 'out/bin/llc', stage / 'out/bin/llc')
        shutil.copy2(source / 'receipt.json', stage / 'receipt.json')
        backend.validate(root, stage)
        # rename refuses an occupied nonempty cache; no existing evidence replaced.
        stage.rename(destination)
    return destination


def prepare_lua(root, archive=None, jobs=2, allow_downloads=False):
    """Guard the direct bootstrap entry point as well as callers such as build.py."""
    import resource_admission
    import docker_admission
    if type(jobs) is not int or not 1 <= jobs <= 8:
        raise ValueError('Lua bootstrap jobs must be between 1 and 8')
    with resource_admission.lease():
        resource_admission.check(str((Path(root).resolve() / 'build' / 'lua-backends')))
        if archive is not None:
            llvm_archive.verify(Path(archive).absolute(), llvm_archive.select('source'))
        with docker_admission.session('Lua backend preparation'):
            return _prepare_lua(root, archive, jobs, allow_downloads)


def _prepare_lua(root, archive, jobs, allow_downloads):
    configuration()
    if archive is not None:
        llvm_archive.verify(Path(archive).absolute(), llvm_archive.select('source'))
    image_check(root, 'd3-build:5.0.1')
    try:
        image_check(root, 'd3-lua-build:5.0.1')
    except backend.BackendError:
        if not allow_downloads:
            raise RuntimeError('Lua image needs preparation; run ' + LUA_REPAIR)
        subprocess.run(['docker', 'build', '-t', 'd3-lua-build:5.0.1', '-f', 'toolchain/lua/Dockerfile',
                        'toolchain/lua'], cwd=root, check=True)
        image_check(root, 'd3-lua-build:5.0.1')
    destination = backend.cache_path(root)
    if destination.exists():
        backend.validate(root, destination)
        print('Verified cached Lua backend:', destination)
        return destination
    # A legacy cache may be adopted only when every current check passes.
    legacy = root / 'build/lua-toolchain'
    try:
        backend.validate(root, legacy)
    except backend.BackendError:
        pass
    else:
        return adopt(root, legacy)
    entry = llvm_archive.select('source')
    if archive is None:
        archive = llvm_archive.fetch(entry, root / 'build/downloads', allow_downloads=allow_downloads)
    archive = Path(archive).absolute()
    llvm_archive.verify(archive, entry)
    destination.parent.mkdir(parents=True, exist_ok=True)
    # Unique staging retains incomplete evidence on failure; bootstrap recipe and
    # its fixed in-container path stay unchanged. Publish only after validation.
    (root / 'build/lua-toolchain').mkdir(parents=True, exist_ok=True)
    stage = Path(tempfile.mkdtemp(prefix='.prepare-', dir=destination.parent))
    subprocess.run(['docker', 'run', '--rm', '--pull', 'never', '--network', 'none',
                    '-v', f'{root}:/work:ro', '-v', f'{stage}:/work/build/lua-toolchain',
                    '-v', f'{archive}:/llvm.tar.xz:ro', '-w', '/work', 'd3-lua-build:5.0.1',
                    'python3', '-B', 'toolchain/lua/bootstrap.py', '--archive', '/llvm.tar.xz',
                    '--jobs', str(jobs)], check=True)
    backend.validate(root, stage)
    stage.rename(destination)
    return destination


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    mode = ap.add_mutually_exclusive_group()
    mode.add_argument('--check', action='store_true')
    mode.add_argument('--prepare-lua', action='store_true')
    mode.add_argument('--adopt-lua', type=Path, metavar='EXISTING_CACHE')
    ap.add_argument('--normal-only', action='store_true')
    ap.add_argument('--lua-cache', type=Path, help='check this exact cache; never fall back')
    ap.add_argument('--archive', type=Path)
    ap.add_argument('--jobs', type=int, default=2)
    ap.add_argument('--allow-downloads', action='store_true')
    args = ap.parse_args()
    if not 1 <= args.jobs <= 8:
        ap.error('--jobs must be between 1 and 8')
    try:
        if args.adopt_lua:
            print(adopt(ROOT, args.adopt_lua))
        elif args.prepare_lua:
            print(prepare_lua(ROOT, args.archive, args.jobs, args.allow_downloads))
        else:
            errors = check(ROOT, not args.normal_only, args.lua_cache)
            for error in errors:
                print(error, file=sys.stderr)
            return bool(errors)
        return 0
    except (RuntimeError, OSError, ValueError, subprocess.SubprocessError) as error:
        print(str(error), file=sys.stderr)
        return 1


if __name__ == '__main__':
    raise SystemExit(main())
