#!/usr/bin/env python3
"""Restore an authenticated archive compactly for an isolated read-only rehearsal.

Internal hardlinks share identical content and mode only inside the new tree.
This is not receipt acceptance and the tree must never be made writable during
verification. A failed restore leaves its partial destination for inspection.
"""
import argparse
import hashlib
import json
import os
from pathlib import Path
import stat

import artifact_store as archive
import resource_admission

MAX_GROWTH = 2 * 1024**3
# Conservative planning allowance for directory entries, inode metadata and
# filesystem block rounding; not a reservation or physical allocation bound.
PER_ENTRY_OVERHEAD = 16384


def estimate(snapshot):
    unique = {}
    for item in snapshot['files']:
        key = (item['sha256'], item['mode'])
        previous = unique.setdefault(key, item['size'])
        archive.require(previous == item['size'], 'conflicting content size')
    data = sum(unique.values())
    entries = len(snapshot['files']) + len(snapshot['directories']) + 1
    return {'unique_bytes': data, 'unique_files': len(unique),
            'estimated_growth_bytes': data + entries * PER_ENTRY_OVERHEAD}


def verify_restore(store, snapshot, destination):
    """Check every path, file digest, mode and internal-only hardlink count."""
    obj, filemap, _ = archive.load_snapshot(store, snapshot)
    root = Path(destination).absolute()
    archive.reject_symlink_ancestors(root)
    info = root.lstat()
    archive.require(stat.S_ISDIR(info.st_mode) and stat.S_IMODE(info.st_mode) == obj['root_mode'], 'restored root changed')
    dirs = {x['path']: x for x in obj['directories']}
    found_dirs, found_files, identities = set(), set(), {}
    directory_metadata = {root: archive.metadata(info)}
    rootfd = os.open(root, os.O_RDONLY | os.O_DIRECTORY | os.O_NOFOLLOW)
    try:
        def onerror(exc):
            raise exc
        for base, names, files in os.walk(root, followlinks=False, onerror=onerror):
            for name in names + files:
                path = Path(base) / name
                relative = path.relative_to(root).as_posix()
                observed = path.lstat()
                if stat.S_ISDIR(observed.st_mode):
                    archive.require(relative in dirs and stat.S_IMODE(observed.st_mode) == dirs[relative]['mode'], 'unexpected or changed directory')
                    found_dirs.add(relative)
                    directory_metadata[path] = archive.metadata(observed)
                    continue
                archive.require(stat.S_ISREG(observed.st_mode) and relative in filemap, 'unexpected or nonregular file')
                item = filemap[relative]
                fd = archive.safe_open(rootfd, relative)
                with os.fdopen(fd, 'rb') as stream:
                    before = os.fstat(stream.fileno())
                    archive.require(archive.metadata(before) == archive.metadata(observed) and stat.S_IMODE(before.st_mode) == item['mode'], 'restored file changed')
                    digest = hashlib.sha256()
                    count = 0
                    while True:
                        block = stream.read(min(archive.CHUNK, item['size'] - count + 1))
                        if not block:
                            break
                        count += len(block)
                        archive.require(count <= item['size'], 'restored content grew')
                        digest.update(block)
                    after = os.fstat(stream.fileno())
                    archive.require(archive.metadata(after) == archive.metadata(before) and
                                    archive.metadata(path.lstat()) == archive.metadata(before) and
                                    count == item['size'] and digest.hexdigest() == item['sha256'], 'restored content changed')
                    key = (after.st_dev, after.st_ino)
                    entry = identities.setdefault(key, {'count': 0, 'links': after.st_nlink, 'content': (item['sha256'], item['mode'])})
                    archive.require(entry['links'] == after.st_nlink and entry['content'] == (item['sha256'], item['mode']), 'inconsistent hardlink')
                    entry['count'] += 1
                found_files.add(relative)
    finally:
        os.close(rootfd)
    archive.require(all(archive.metadata(path.lstat()) == saved for path, saved in directory_metadata.items()), 'restored directory changed')
    archive.require(found_dirs == set(dirs) and found_files == set(filemap), 'restored paths missing')
    archive.require(all(x['count'] == x['links'] for x in identities.values()), 'restored hardlink has external alias')
    return {'status': 'compact_bundle_verified', 'snapshot': snapshot, 'files': len(found_files),
            'directories': len(found_dirs), 'receipt_acceptance': False}


def restore(store, snapshot, destination, original_root, max_growth=MAX_GROWTH):
    archive.require(type(max_growth) is int and 0 < max_growth <= MAX_GROWTH,
                    'invalid growth budget')
    store = Path(store).absolute()
    destination = Path(destination).absolute()
    original_root = Path(original_root).absolute()
    archive.reject_symlink_ancestors(original_root)
    archive.reject_symlink_ancestors(store)
    archive.reject_symlink_ancestors(destination)
    archive.require(not destination.resolve(strict=False).is_relative_to(store.resolve())
                    and not store.resolve().is_relative_to(destination.resolve(strict=False)),
                    'destination overlaps archive')
    original = original_root.resolve(strict=False)
    target = destination.resolve(strict=False)
    archive.require(not target.is_relative_to(original) and not original.is_relative_to(target),
                    'destination overlaps original root')
    with resource_admission.lease():
        obj, _, total = archive.load_snapshot(store, snapshot)
        plan = estimate(obj)
        archive.require(plan['estimated_growth_bytes'] <= max_growth, 'restore exceeds growth budget')
        resource_admission.check(destination.parent, growth_bytes=plan['estimated_growth_bytes'])
        archive.verify(store, snapshot)
        os.mkdir(destination, 0o700)
        try:
            for item in sorted(obj['directories'], key=lambda x: (x['path'].count('/'), x['path'])):
                (destination / item['path']).mkdir(mode=0o700)
            first = {}
            for item in obj['files']:
                path = destination / item['path']
                key = (item['sha256'], item['mode'])
                if key in first:
                    os.link(first[key], path, follow_symlinks=False)
                else:
                    fd = os.open(path, os.O_WRONLY | os.O_CREAT | os.O_EXCL | os.O_NOFOLLOW, 0o600)
                    with os.fdopen(fd, 'wb') as out:
                        archive.read_blob(store, item, sink=out)
                    os.chmod(path, item['mode'], follow_symlinks=False)
                    first[key] = path
            for item in sorted(obj['directories'], key=lambda x: (-x['path'].count('/'), x['path'])):
                os.chmod(destination / item['path'], item['mode'], follow_symlinks=False)
            os.chmod(destination, obj['root_mode'], follow_symlinks=False)
            verify_restore(store, snapshot, destination)
        except BaseException as exc:
            raise archive.ArchiveError('compact restore incomplete; new directory retained: '+str(destination)) from exc
    return {'status': 'compact_bundle_restored', 'snapshot': snapshot,
            'destination': str(destination), 'logical_bytes': total, **plan,
            'independent_of_source_and_store': True, 'read_only_rehearsal_required': True,
            'receipt_acceptance': False}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--store', required=True)
    parser.add_argument('--snapshot', required=True)
    parser.add_argument('--destination', required=True)
    parser.add_argument('--original-root', required=True)
    args = parser.parse_args()
    print(json.dumps(restore(args.store, args.snapshot, args.destination, args.original_root), sort_keys=True))


if __name__ == '__main__':
    main()
