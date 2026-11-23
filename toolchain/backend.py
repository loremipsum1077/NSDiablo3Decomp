"""Read-only backend provenance and recipe-keyed Lua cache selection.

Receipts bind local build evidence, not independent approval of its producer.
"""
import hashlib
import json
import os
from pathlib import Path
import sys

sys.dont_write_bytecode = True
ROOT = Path(__file__).resolve().parents[1]
RECIPE = ('bootstrap.py', 'Dockerfile', '../llvm.lock.json', '../llvm_archive.py')
OPTIONS = ['-G', 'Ninja', '-DPYTHON_EXECUTABLE=/usr/bin/python3', '-DCMAKE_BUILD_TYPE=Release',
           '-DCMAKE_C_COMPILER=/usr/bin/gcc', '-DCMAKE_CXX_COMPILER=/usr/bin/g++',
           '-DLLVM_TARGETS_TO_BUILD=AArch64', '-DLLVM_ENABLE_ASSERTIONS=ON',
           '-DLLVM_INCLUDE_TESTS=OFF', '-DLLVM_INCLUDE_EXAMPLES=OFF', '-DLLVM_INCLUDE_DOCS=OFF',
           '-DLLVM_ENABLE_TERMINFO=OFF', '-DLLVM_ENABLE_ZLIB=OFF']


class BackendError(RuntimeError):
    def __init__(self, status, detail):
        self.status = status
        super().__init__(status + ': ' + detail)


def sha(path):
    h = hashlib.sha256()
    with Path(path).open('rb') as f:
        for chunk in iter(lambda: f.read(1024 * 1024), b''):
            h.update(chunk)
    return h.hexdigest()


def spec(root=ROOT):
    here = Path(root) / 'toolchain/lua'
    config = json.loads((here / 'config.json').read_text())
    if sha(here / 'llvm-5.0.1.patch') != config['patch_sha256']:
        raise BackendError('corrupted', 'Lua patch differs from config.json')
    return {'config': config, 'recipe': {name: sha(here / name) for name in RECIPE}}


def cache_path(root=ROOT):
    identity = spec(root)
    identity['platform'] = os.environ.get('D3_DOCKER_PLATFORM', 'linux/arm64')
    key = hashlib.sha256(json.dumps(identity, sort_keys=True).encode()).hexdigest()
    return Path(root) / 'build/lua-backends' / key


def resolve(root=ROOT, explicit=None):
    # Explicit paths (including retained receipt mounts) never fall back.
    if explicit is not None:
        return Path(explicit)
    if os.environ.get('D3_LUA_TOOLCHAIN'):
        return Path(os.environ['D3_LUA_TOOLCHAIN'])
    return cache_path(root)


def validate(root=ROOT, base=None, expected=None):
    base = resolve(root, base)
    expected = spec(root) if expected is None else expected
    if not base.exists():
        raise BackendError('missing', str(base))
    try:
        for path in (base, base / 'receipt.json', base / 'out', base / 'out/bin', base / 'out/bin/llc'):
            if path.is_symlink():
                raise BackendError('corrupted', 'symlink in Lua cache: ' + str(path))
        receipt = json.loads((base / 'receipt.json').read_text())
        if not isinstance(receipt, dict):
            raise ValueError('receipt must be an object')
        if any(receipt.get(k) != v for k, v in expected.items()):
            raise BackendError('stale', 'Lua config/recipe mismatch at ' + str(base))
        if receipt.get('configure_options') != OPTIONS:
            raise BackendError('corrupted', 'Lua configure options mismatch')
        if not isinstance(receipt.get('gcc_version'), str) or not receipt['gcc_version'].startswith('gcc '):
            raise BackendError('corrupted', 'missing GCC provenance')
        version = receipt.get('llc_version', '')
        if 'LLVM version 5.0.1' not in version or 'with assertions' not in version or 'aarch64' not in version:
            raise BackendError('unsupported', 'Lua version/target configuration')
        binary = base / 'out/bin/llc'
        if not binary.is_file() or not binary.stat().st_mode & 0o111 or sha(binary) != receipt.get('binary_sha256'):
            raise BackendError('corrupted', 'Lua binary hash/executable mismatch')
        with binary.open('rb') as stream:
            header = stream.read(20)
        machine = {'linux/arm64': 183, 'linux/amd64': 62}.get(os.environ.get('D3_DOCKER_PLATFORM', 'linux/arm64'))
        if (header[:6] != b'\x7fELF\x02\x01' or len(header) != 20
                or int.from_bytes(header[18:20], 'little') != machine):
            raise BackendError('unsupported', 'Lua executable host architecture differs from D3_DOCKER_PLATFORM')
        return receipt
    except (OSError, ValueError, TypeError, KeyError) as error:
        raise BackendError('corrupted', str(error)) from error


if __name__ == '__main__':
    import argparse
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--path', action='store_true', help='print current recipe cache path only')
    ap.add_argument('--cache', type=Path)
    args = ap.parse_args()
    try:
        path = resolve(explicit=args.cache)
        if not args.path:
            validate(base=path)
        print(path)
    except (BackendError, OSError, ValueError) as error:
        sys.exit(str(error))
