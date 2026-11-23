#!/usr/bin/env python3
"""Read-only filesystem capacity estimate; not a build reservation.

All byte estimates and per-filesystem reserve are caller supplied. A passing
result is advisory: concurrent or non-cooperating writers can consume space.
This module never creates directories, deletes artifacts, or starts builds.
"""
import argparse
from contextlib import contextmanager
import errno
import fcntl
import json
import os
from pathlib import Path
import stat
import threading

MAX_BYTES = (1 << 63) - 1
MAX_REQUESTS = 64
DEFAULT_GROWTH = 8 * 1024**3
DEFAULT_RESERVE = 12 * 1024**3
ATTEMPT_GROWTH = 2 * 1024**3
LOCK_PATH = f'/tmp/nsd3-resource-{os.getuid()}.lock'


class AdmissionError(RuntimeError):
    """Resource availability is insufficient or cannot be established."""


_local = threading.local()
_fd_guard = threading.RLock()
_open_fds = set()


def _after_fork_child():
    # flock is inherited through the shared open-file description. Close the
    # child's copies so it cannot retain or reenter the parent's lease.
    global _local
    for fd in _open_fds:
        try:
            os.close(fd)
        except OSError:
            pass
    _open_fds.clear()
    _local = threading.local()
    _fd_guard.release()


if hasattr(os, 'register_at_fork'):
    os.register_at_fork(before=_fd_guard.acquire, after_in_parent=_fd_guard.release,
                        after_in_child=_after_fork_child)


@contextmanager
def lease():
    """Hold the nonblocking per-user host lease; only this thread may reenter.

    The file is intentionally retained after release: unlinking a lock file can
    split contenders across different inodes. Non-cooperating jobs are outside
    this lease, so space estimates and external resource limits still apply.
    """
    owner_pid = os.getpid()
    if getattr(_local, 'depth', 0):
        _local.depth += 1
        try:
            yield
        finally:
            if os.getpid() == owner_pid:
                _local.depth -= 1
        return
    fd = None
    try:
        with _fd_guard:
            if not hasattr(os, 'O_NOFOLLOW'):
                raise AdmissionError('Resource lease unavailable: no symlink protection')
            flags = os.O_RDWR | os.O_CREAT | os.O_NOFOLLOW | getattr(os, 'O_CLOEXEC', 0)
            fd = os.open(LOCK_PATH, flags, 0o600)
            _open_fds.add(fd)
        def authenticate():
            info = os.fstat(fd)
            named = os.lstat(LOCK_PATH)
            if (not stat.S_ISREG(info.st_mode) or info.st_uid != os.getuid()
                    or info.st_nlink != 1 or info.st_mode & 0o077
                    or (info.st_dev, info.st_ino) != (named.st_dev, named.st_ino)
                    or not stat.S_ISREG(named.st_mode)):
                raise AdmissionError('Unsafe resource lease file; inspect its ownership and type')
        authenticate()
        try:
            fcntl.flock(fd, fcntl.LOCK_EX | fcntl.LOCK_NB)
        except OSError as exc:
            if exc.errno in (errno.EACCES, errno.EAGAIN, errno.EWOULDBLOCK):
                raise AdmissionError('Resource lease busy: another cooperating build is active') from exc
            raise
        authenticate()
    except OSError as exc:
        if fd is not None:
            with _fd_guard:
                _open_fds.discard(fd)
                os.close(fd)
        raise AdmissionError('Resource lease unavailable: '+str(exc)) from exc
    except BaseException:
        if fd is not None:
            with _fd_guard:
                _open_fds.discard(fd)
                os.close(fd)
        raise
    _local.depth = 1
    try:
        yield
    finally:
        # A forked child closed its inherited descriptor in _after_fork_child.
        # The number may already have been reused; do not touch it or child TLS.
        if os.getpid() == owner_pid:
            _local.depth = 0
            try:
                fcntl.flock(fd, fcntl.LOCK_UN)
            finally:
                with _fd_guard:
                    _open_fds.discard(fd)
                    os.close(fd)


def _bytes(value, *, positive=False):
    if type(value) is not int or value < (1 if positive else 0) or value > MAX_BYTES:
        raise ValueError('invalid byte budget')
    return value


def probe(path):
    """Find the existing ancestor and use unprivileged available blocks."""
    if not isinstance(path, str) or not path or not Path(path).is_absolute():
        raise ValueError('absolute request path required')
    candidate = Path(path).resolve(strict=False)
    while True:
        try:
            info = candidate.stat()
            break
        except FileNotFoundError:
            parent = candidate.parent
            if parent == candidate:
                raise ValueError('no existing filesystem ancestor')
            candidate = parent
    if not stat.S_ISDIR(info.st_mode):
        raise ValueError('existing ancestor is not a directory')
    volume = os.statvfs(candidate)
    if volume.f_frsize <= 0 or volume.f_bavail < 0:
        raise ValueError('invalid filesystem capacity')
    return {'ancestor': str(candidate), 'device': info.st_dev,
            'available_bytes': volume.f_bavail * volume.f_frsize}


def assess(requests, reserve_bytes, *, inspect=probe):
    """Conservatively assume distinct devices can share a backing capacity pool."""
    _bytes(reserve_bytes, positive=True)
    if not isinstance(requests, list) or not 1 <= len(requests) <= MAX_REQUESTS:
        raise ValueError('bounded nonempty request list required')
    devices = {}
    for entry in requests:
        if not isinstance(entry, dict) or set(entry) != {'path', 'bytes'}:
            raise ValueError('request requires path and bytes')
        amount = _bytes(entry['bytes'])
        result = inspect(entry['path'])
        if (not isinstance(result, dict) or set(result) != {'ancestor', 'device', 'available_bytes'}
                or not isinstance(result['ancestor'],str) or type(result['device']) is not int
                or type(result['available_bytes']) is not int or result['available_bytes'] < 0):
            raise ValueError('invalid filesystem observation')
        group = devices.setdefault(result['device'], {'device': result['device'],
            'available_bytes': result['available_bytes'], 'requested_bytes': 0,
            'reserve_bytes': reserve_bytes, 'requests': []})
        group['available_bytes'] = min(group['available_bytes'],result['available_bytes'])
        group['requested_bytes'] += amount
        if group['requested_bytes'] > MAX_BYTES - reserve_bytes:
            raise ValueError('aggregate byte budget overflow')
        group['requests'].append({'path': entry['path'], 'ancestor': result['ancestor'], 'bytes': amount})
    result = sorted(devices.values(),key=lambda x:x['device'])
    total = sum(x['requested_bytes'] for x in result)
    if total > MAX_BYTES - reserve_bytes:
        raise ValueError('aggregate byte budget overflow')
    for volume in result:
        # APFS and other pooled filesystems can report distinct device IDs while
        # drawing from the same physical free space. Without proven separate
        # pools, every observation must cover all requested growth plus reserve.
        volume['combined_growth_bytes'] = total
        volume['required_bytes'] = total + reserve_bytes
        volume['shortfall_bytes'] = max(0,volume['required_bytes'] - volume['available_bytes'])
    return {'kind':'filesystem-capacity-estimate-v1',
            'advisory':True, 'reservation_held':False,
            'limitation':'Assumes filesystems may share a pool; caller budgets must cover peak growth and retries. Capacity, quotas and Docker VM limits may differ or change concurrently.',
            'sufficient':all(x['shortfall_bytes']==0 for x in result),'volumes':result}


def check(path, growth_bytes=DEFAULT_GROWTH, reserve_bytes=DEFAULT_RESERVE):
    """Require the caller-supplied filesystem budget; no capacity is reserved."""
    try:
        report = assess([{'path':str(path), 'bytes':growth_bytes}], reserve_bytes)
    except (OSError, RuntimeError, ValueError) as exc:
        raise AdmissionError('Cannot determine resource availability: '+str(exc)) from exc
    if not report['sufficient']:
        shortage = max(x['shortfall_bytes'] for x in report['volumes'])
        raise AdmissionError(f'Insufficient disk space: need {shortage} more bytes after reserve')
    return report


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--reserve-bytes',type=int,required=True)
    parser.add_argument('--request',action='append',nargs=2,metavar=('ABSOLUTE_PATH','BYTES'),required=True)
    args=parser.parse_args()
    try:
        report=assess([{'path':path,'bytes':int(amount)} for path,amount in args.request],args.reserve_bytes)
    except (OSError,RuntimeError,ValueError) as exc:
        print(json.dumps({'kind':'filesystem-capacity-estimate-v1','sufficient':False,
                          'advisory':True,'reservation_held':False,'error':str(exc)}))
        return 2
    print(json.dumps(report,indent=2))
    return 0 if report['sufficient'] else 1

if __name__ == '__main__':
    raise SystemExit(main())
