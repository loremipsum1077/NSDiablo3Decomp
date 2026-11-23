#!/usr/bin/env python3
"""Select and verify locked LLVM archives; never extract, install or execute them.

Python 3.8+ (also used inside Ubuntu 20.04). Downloads require explicit opt-in.
See llvm-downloads.md for the pins' provenance and authentication limits.
"""
import argparse
import hashlib
import json
from pathlib import Path
import re
import sys
import tempfile
import time
import urllib.request

LOCK = Path(__file__).with_name('llvm.lock.json')
PLATFORMS = {'linux/arm64': 'aarch64-linux-gnu',
             'linux/amd64': 'x86_64-linux-gnu-ubuntu-16.04', 'source': 'source'}


def select(platform, version='5.0.1', arch='', url='', lock_path=LOCK):
    """Validate all selection/pin fields before any download or cache mutation."""
    lock = json.loads(Path(lock_path).read_text())
    if lock.get('schema_version') != 1 or lock.get('version') != '5.0.1' or version != lock['version']:
        raise RuntimeError('only the locked LLVM 5.0.1 release is supported')
    if platform not in PLATFORMS:
        raise RuntimeError('unsupported LLVM platform: ' + platform)
    entry = lock.get('archives', {}).get(platform, {})
    expected_arch = PLATFORMS[platform]
    filename = ('llvm-5.0.1.src.tar.xz' if platform == 'source' else
                'clang+llvm-5.0.1-' + expected_arch + '.tar.xz')
    official = 'https://releases.llvm.org/5.0.1/' + filename
    if entry.get('arch') != expected_arch or (arch and arch != expected_arch):
        raise RuntimeError('LLVM_ARCH differs from the locked platform archive')
    if entry.get('filename') != filename or entry.get('url') != official or (url and url != official):
        raise RuntimeError('LLVM URL must be the locked original HTTPS release URL')
    if not isinstance(entry.get('sha256'), str) or not re.fullmatch('[0-9a-f]{64}', entry['sha256']):
        raise RuntimeError('missing or invalid LLVM SHA-256 pin')
    size = entry.get('size_bytes')
    if type(size) is not int or not 0 < size <= 270000000:
        raise RuntimeError('missing or invalid LLVM archive size pin')
    return entry


def verify(path, entry):
    path = Path(path)
    if path.is_symlink() or not path.is_file():
        raise RuntimeError('LLVM archive must be an existing regular non-symlink file: ' + str(path))
    if path.stat().st_size != entry['size_bytes']:
        raise RuntimeError('LLVM archive size mismatch: ' + str(path))
    digest = hashlib.sha256()
    with path.open('rb') as stream:
        for chunk in iter(lambda: stream.read(1024 * 1024), b''):
            digest.update(chunk)
    if digest.hexdigest() != entry['sha256']:
        raise RuntimeError('LLVM archive SHA-256 mismatch: ' + str(path))
    return path


class OfficialRedirects(urllib.request.HTTPRedirectHandler):
    def redirect_request(self, req, fp, code, msg, headers, newurl):
        # Do not send even a redirected request to another host, scheme or path.
        if newurl != req.full_url:
            raise RuntimeError('LLVM download redirect refused: ' + newurl)
        return super().redirect_request(req, fp, code, msg, headers, newurl)


def fetch(entry, cache_dir, allow_downloads=False):
    cache_dir = Path(cache_dir)
    target = cache_dir / entry['filename']
    if target.exists() or target.is_symlink():
        return verify(target, entry)  # Corrupt caches fail; never silently redownload.
    if not allow_downloads:
        raise RuntimeError('LLVM archive missing; provide the pinned cache or explicitly allow downloads')
    if cache_dir.is_symlink():
        raise RuntimeError('LLVM cache directory must not be a symlink')
    cache_dir.mkdir(parents=True, exist_ok=True)
    temporary = None
    try:
        opener = urllib.request.build_opener(OfficialRedirects())
        started = time.monotonic()
        with opener.open(entry['url'], timeout=30) as response:
            if response.geturl() != entry['url']:
                raise RuntimeError('LLVM download URL changed')
            with tempfile.NamedTemporaryFile(prefix='.llvm-', dir=cache_dir, delete=False) as stream:
                temporary = Path(stream.name)
                count = 0
                while True:
                    chunk = response.read(1024 * 1024)
                    if not chunk:
                        break
                    count += len(chunk)
                    if count > entry['size_bytes'] or time.monotonic() - started > 300:
                        raise RuntimeError('LLVM download exceeded locked size or time limit')
                    stream.write(chunk)
        verify(temporary, entry)
        if target.exists() or target.is_symlink():
            return verify(target, entry)
        temporary.replace(target)
        return verify(target, entry)
    finally:
        if temporary is not None:
            temporary.unlink(missing_ok=True)


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--platform', required=True)
    ap.add_argument('--version', default='5.0.1')
    ap.add_argument('--arch', default='')
    ap.add_argument('--url', default='')
    ap.add_argument('--cache-dir', type=Path, required=True)
    ap.add_argument('--allow-downloads', action='store_true')
    args = ap.parse_args(argv)
    try:
        entry = select(args.platform, args.version, args.arch, args.url)
        print(fetch(entry, args.cache_dir, args.allow_downloads))
        return 0
    except (OSError, ValueError, RuntimeError) as error:
        print('LLVM: ' + str(error), file=sys.stderr)
        return 1


if __name__ == '__main__':
    raise SystemExit(main())
