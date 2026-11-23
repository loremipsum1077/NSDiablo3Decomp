"""Bind verified LLVM extraction to the local image recipe and installed bytes."""
import hashlib
import json
from pathlib import Path
import sys

sys.dont_write_bytecode = True
FILES = ('Dockerfile', 'llvm.lock.json', 'llvm_archive.py', 'image_provenance.py')


def sha(path):
    h = hashlib.sha256()
    with path.open('rb') as stream:
        for chunk in iter(lambda: stream.read(1024 * 1024), b''):
            h.update(chunk)
    return h.hexdigest()


def manifest(root):
    return {str(p.relative_to(root)): sha(p) for p in sorted(root.rglob('*')) if p.is_file()}


def inspect(record=False):
    here = Path('/opt/d3-toolchain')
    root = Path('/opt/llvm').resolve()
    actual = {'recipe': {name: sha(here / name) for name in FILES},
              'files': manifest(root)}
    if record:
        (here / 'installed.json').write_text(json.dumps(actual, sort_keys=True))
    elif json.loads((here / 'installed.json').read_text()) != actual:
        raise RuntimeError('installed compiler bytes differ from verified extraction')
    return actual['recipe']


if __name__ == '__main__':
    print(json.dumps(inspect('--record' in sys.argv)))
