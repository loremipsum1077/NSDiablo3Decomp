#!/usr/bin/env python3
"""Verify local original-input identity using only Python's standard library.

Read-only: no downloads, builds, providers, hash regeneration or original copies.
SHA-256 consistency establishes identity, not ownership or redistribution rights.
"""
import argparse
import ast
import hashlib
import json
import os
from pathlib import Path
import re
import stat
import sys

ROOT = Path(__file__).resolve().parents[1]
VERSION = '2.6.2'
PLATFORM = 'Switch'
CHUNK_SIZE = 1024 * 1024


class InputError(RuntimeError):
    """A safe diagnostic code, without private paths or file contents."""


def safe_filename(value):
    if (not isinstance(value, str) or
            not re.fullmatch(r'[A-Za-z0-9_.-]+(?:/[A-Za-z0-9_.-]+)*', value) or
            any(part in ('.', '..') for part in value.split('/'))):
        raise InputError('unsafe filename in original-input manifest')
    return value


def load_manifest(root=ROOT, version=VERSION, platform=PLATFORM):
    if version != VERSION:
        raise InputError('unsupported version; only Switch 2.6.2 is supported')
    if platform != PLATFORM:
        raise InputError('unsupported platform; only Switch 2.6.2 is supported')
    try:
        manifest = json.loads((Path(root) / 'config/original_inputs.json').read_text())
        if (manifest['schema_version'] != 1 or manifest['version'] != VERSION or
                manifest['platform'] != PLATFORM):
            raise InputError('unsupported original-input manifest schema or target')
        rows = manifest['inputs']
        if not isinstance(rows, list) or not rows:
            raise InputError('invalid original-input manifest entries')
        ids, filenames = set(), set()
        for row in rows:
            filename = safe_filename(row['filename'])
            if (not re.fullmatch(r'[a-z][a-z0-9_]*', row['id']) or row['id'] in ids or
                    filename in filenames or type(row['required']) is not bool or
                    type(row['size']) is not int or row['size'] < 0 or
                    not re.fullmatch(r'[a-f0-9]{64}', row['sha256']) or
                    not isinstance(row['role'], str) or not row['role']):
                raise InputError('invalid or duplicate original-input manifest entry')
            ids.add(row['id'])
            filenames.add(filename)
            for alias in row.get('equivalent_filenames', []):
                safe_filename(alias)
        required = {row['id'] for row in rows if row['required']}
        if required != {'nso', 'nss'}:
            raise InputError('manifest must require exactly nso and nss')
        # Keep the historical literal available to contributor_check's AST reader.
        # A mirror mismatch fails closed; it never creates an alternate accepted hash.
        pins = []
        for node in ast.parse((Path(root) / 'tools/setup.py').read_text()).body:
            if isinstance(node, ast.Assign) and any(
                    isinstance(t, ast.Name) and t.id == 'NSO_SHA256' for t in node.targets):
                pins.append(ast.literal_eval(node.value))
        nso = next(row for row in rows if row['id'] == 'nso')
        if pins != [nso['sha256']]:
            raise InputError('setup NSO_SHA256 mirror differs from original-input manifest')
        return manifest
    except InputError:
        raise
    except (OSError, ValueError, SyntaxError, KeyError, TypeError, AttributeError):
        raise InputError('cannot read valid original-input manifest/setup pin') from None


def stat_identity(value):
    # atime may change merely by reading; mtime/ctime must not.
    return (value.st_dev, value.st_ino, value.st_mode, value.st_size,
            value.st_mtime_ns, value.st_ctime_ns)


def fingerprint(path, expected_size=None):
    """Stream a regular file, binding its pathname and open descriptor to stable stats."""
    path = Path(path)
    try:
        before = path.lstat()
    except FileNotFoundError:
        raise InputError('missing') from None
    except OSError:
        raise InputError('unreadable') from None
    try:
        if not stat.S_ISREG(before.st_mode):
            raise InputError('not_regular (symlinks and special files are unsupported)')
        # NONBLOCK also prevents a raced-in FIFO from blocking before fstat.
        flags = os.O_RDONLY | os.O_NONBLOCK | os.O_NOFOLLOW
        fd = os.open(path, flags)
        with os.fdopen(fd, 'rb') as stream:
            opened = os.fstat(stream.fileno())
            if stat_identity(before) != stat_identity(opened):
                raise InputError('changed_during_hash')
            if expected_size is not None and opened.st_size != expected_size:
                raise InputError('wrong_size')
            digest = hashlib.sha256()
            count = 0
            for block in iter(lambda: stream.read(CHUNK_SIZE), b''):
                count += len(block)
                digest.update(block)
            after_open = os.fstat(stream.fileno())
        after_path = path.lstat()
        if (stat_identity(before) != stat_identity(after_open) or
                stat_identity(before) != stat_identity(after_path) or count != before.st_size):
            raise InputError('changed_during_hash')
        return {'sha256': digest.hexdigest(), 'size': count}
    except FileNotFoundError:
        raise InputError('changed_during_hash') from None
    except OSError:
        raise InputError('unreadable_or_changed') from None


def verify_inputs(input_root, *, root=ROOT, nso=None, nss=None, include_optional=True,
                  version=VERSION, platform=PLATFORM):
    manifest = load_manifest(root, version, platform)
    input_root = Path(input_root).resolve()
    overrides = {'nso': nso, 'nss': nss}
    results = []
    for row in manifest['inputs']:
        if not row['required'] and not include_optional:
            continue
        result = {'id': row['id'], 'filename': row['filename'], 'required': row['required']}
        override = overrides.get(row['id'])
        path = Path(override) if override is not None else input_root / row['filename']
        try:
            try:
                relative = path.absolute().relative_to(input_root)
            except ValueError:
                relative = None  # Explicit external paths remain supported.
            if relative is not None:
                # Never follow an input-layout subdirectory out of the input root.
                current = input_root
                for part in relative.parts:
                    current /= part
                    if current.is_symlink():
                        raise InputError('unsafe_path (symlink in input layout)')
            actual = fingerprint(path, row['size'])
            result.update(actual)
            result['status'] = 'verified' if actual['sha256'] == row['sha256'] else 'wrong_hash'
        except InputError as error:
            result['status'] = 'missing_optional' if str(error) == 'missing' and not row['required'] else str(error)
        results.append(result)
    return results


def failed(results):
    return any(row['status'] not in ('verified', 'missing_optional') for row in results)


def require_inputs(input_root, **kwargs):
    results = verify_inputs(input_root, include_optional=False, **kwargs)
    if failed(results):
        raise InputError('; '.join(row['filename'] + ': ' + row['status'] for row in results
                                   if row['status'] != 'verified'))
    return results


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--input-root', type=Path, default=ROOT / 'build',
                    help='local dump layout containing exefs/ and romfs/ (default: build/)')
    ap.add_argument('--nso', type=Path, help='explicit local original NSO; same pinned bytes required')
    ap.add_argument('--nss', type=Path, help='explicit local original r NSS; same pinned bytes required')
    ap.add_argument('--version', default=VERSION)
    ap.add_argument('--platform', default=PLATFORM)
    args = ap.parse_args(argv)
    try:
        results = verify_inputs(args.input_root, nso=args.nso, nss=args.nss,
                                version=args.version, platform=args.platform)
    except (InputError, OSError) as error:
        # InputError strings are controlled; OSError may contain private paths.
        print('BLOCKED: ' + (str(error) if isinstance(error, InputError) else 'input root unavailable'), file=sys.stderr)
        return 1
    for row in results:
        print(row['status'] + ': ' + row['filename'])
    if failed(results):
        print('BLOCKED: supply the matching original inputs; expected hashes are never regenerated.', file=sys.stderr)
        return 1
    print('Original-input identity verified for Switch 2.6.2; ownership/redistribution rights are not established.')
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
