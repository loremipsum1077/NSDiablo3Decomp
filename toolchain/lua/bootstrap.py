#!/usr/bin/env python3
"""Build the pinned Lua backend inside the build image, without network access."""
import argparse
import hashlib
import json
from pathlib import Path
import subprocess
import sys
import tarfile

ROOT = Path(__file__).resolve().parents[2]
HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(ROOT / "toolchain"))
import llvm_archive

CONFIG = json.loads((HERE / 'config.json').read_text())


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--archive', required=True, type=Path)
    parser.add_argument('--jobs', type=int, default=6)
    args = parser.parse_args()
    archive = args.archive.absolute()
    patch = HERE / 'llvm-5.0.1.patch'
    llvm_archive.verify(archive, llvm_archive.select(CONFIG['archive_key']))
    if digest(patch) != CONFIG['patch_sha256']:
        raise SystemExit('LLVM patch checksum mismatch')
    recipe = {name: digest(HERE / name) for name in ('bootstrap.py', 'Dockerfile', '../llvm.lock.json', '../llvm_archive.py')}
    base = ROOT / 'build/lua-toolchain'
    base.mkdir(parents=True, exist_ok=True)
    binary = base / 'out/bin/llc'
    receipt = base / 'receipt.json'
    if receipt.exists():
        saved = json.loads(receipt.read_text())
        if saved['config'] == CONFIG and saved.get('recipe') == recipe and binary.exists() and digest(binary) == saved['binary_sha256']:
            print('Verified cached Lua backend:', binary)
            return
        raise SystemExit('Lua backend receipt mismatch; remove build/lua-toolchain to rebuild cleanly')
    source = base / 'llvm-5.0.1.src'
    # A partial extraction/build cannot silently become a new trusted compiler.
    if source.exists():
        raise SystemExit('Incomplete Lua bootstrap; remove build/lua-toolchain and retry')
    with tarfile.open(archive) as bundle:
        bundle.extractall(base)  # archive size/hash verified against the lock above
    subprocess.run(['patch', '-d', str(source), '-p1', '-i', str(patch)], check=True)
    options = ['-G', 'Ninja', '-DPYTHON_EXECUTABLE=/usr/bin/python3', '-DCMAKE_BUILD_TYPE=Release',
               '-DCMAKE_C_COMPILER=/usr/bin/gcc', '-DCMAKE_CXX_COMPILER=/usr/bin/g++',
               '-DLLVM_TARGETS_TO_BUILD=AArch64', '-DLLVM_ENABLE_ASSERTIONS=ON',
               '-DLLVM_INCLUDE_TESTS=OFF', '-DLLVM_INCLUDE_EXAMPLES=OFF', '-DLLVM_INCLUDE_DOCS=OFF',
               '-DLLVM_ENABLE_TERMINFO=OFF', '-DLLVM_ENABLE_ZLIB=OFF']
    subprocess.run(['cmake', '-S', str(source), '-B', str(base / 'out')] + options, check=True)
    subprocess.run(['cmake', '--build', str(base / 'out'), '--target', 'llc', '--', '-j' + str(args.jobs)], check=True)
    receipt.write_text(json.dumps({'config': CONFIG, 'recipe': recipe, 'binary_sha256': digest(binary),
        'configure_options': options, 'gcc_version': subprocess.check_output(['/usr/bin/gcc', '--version'], text=True),
        'llc_version': subprocess.check_output([str(binary), '--version'], text=True)}, indent=2) + '\n')


if __name__ == '__main__':
    main()
