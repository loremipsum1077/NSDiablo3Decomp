#!/usr/bin/env python3
"""Conservative Docker session admission and explicit interruption reconciliation.

A pending session is never automatically cleared just because no container is
visible. Reconciliation is an operator-reviewed override, not proof that a lost
Docker create request cannot still arrive. No containers are stopped or deleted.
"""
import argparse
from contextlib import contextmanager
import hashlib
import json
import os
from pathlib import Path
import re
import selectors
import stat
import subprocess
import threading
import time
import uuid

import resource_admission as resources

JOURNAL = Path(f'/tmp/nsd3-docker-{os.getuid()}.json')
MAX_OUTPUT = 1024 * 1024
MAX_JOURNAL = 16384
_local = threading.local()
PREFIXES = ('d3-receipt-', 'd3-integration-', 'd3-codegen-probe-', 'd3-lua-probe-', 'd3-artifact-rehearsal-', 'd3-compiler-experiment-')
REPOSITORIES = {'d3-build', 'd3-lua-build'}
ROOT = Path(__file__).resolve().parent.parent


def query(arguments):
    """Bound both output pipes and elapsed time; never mutate Docker state."""
    command = ['docker', *arguments]
    try:
        process = subprocess.Popen(command, stdin=subprocess.DEVNULL,
                                   stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        streams = [bytearray(), bytearray()]
        deadline = time.monotonic() + 20
        with selectors.DefaultSelector() as selector:
            for index, pipe in enumerate((process.stdout, process.stderr)):
                selector.register(pipe, selectors.EVENT_READ, index)
            try:
                while selector.get_map():
                    left = deadline - time.monotonic()
                    if left <= 0:
                        raise resources.AdmissionError('Docker admission query timed out')
                    for key, _ in selector.select(min(1, left)):
                        data = os.read(key.fileobj.fileno(), 65536)
                        if not data:
                            selector.unregister(key.fileobj)
                            continue
                        if len(streams[key.data]) + len(data) > MAX_OUTPUT:
                            raise resources.AdmissionError('Docker admission query exceeds output limit')
                        streams[key.data].extend(data)
                process.wait(timeout=max(0.001, deadline - time.monotonic()))
                if process.returncode:
                    raise resources.AdmissionError('Docker admission query failed; daemon state is unknown')
                return streams[0].decode('utf-8')
            finally:
                if process.poll() is None:
                    process.kill()
                process.wait()
                process.stdout.close()
                process.stderr.close()
    except (OSError, UnicodeError, subprocess.SubprocessError) as error:
        raise resources.AdmissionError('Docker admission unavailable: ' + str(error)) from error


def decode(raw):
    def pairs(items):
        result = {}
        for key, value in items:
            if key in result:
                raise ValueError('duplicate JSON key')
            result[key] = value
        return result
    return json.loads(raw, object_pairs_hook=pairs)


def observe():
    """Legacy image/name detection is conservative, not a global VM-idle proof."""
    try:
        daemon = decode(query(['info', '--format', '{{json .ID}}']))
        if daemon == '':
            raise resources.AdmissionError('Docker daemon is not reachable; start Docker Desktop and retry')
        if not isinstance(daemon, str) or not 1 <= len(daemon) <= 256:
            raise ValueError('invalid daemon identity')
        images = set()
        for line in query(['image', 'ls', '--no-trunc', '--format', '{{json .}}']).splitlines():
            row = decode(line)
            if not isinstance(row, dict) or not all(isinstance(row.get(k), str) for k in ('Repository', 'ID')):
                raise ValueError('invalid image observation')
            if row['Repository'] in REPOSITORIES:
                images.add(row['ID'])
        active = []
        for line in query(['ps', '--all', '--no-trunc', '--format', '{{json .}}']).splitlines():
            row = decode(line)
            if not isinstance(row, dict) or not all(isinstance(row.get(k), str) for k in ('ID', 'Names', 'Image', 'State', 'Labels')):
                raise ValueError('invalid container observation')
            if not re.fullmatch(r'[0-9a-f]{64}', row['ID']):
                raise ValueError('invalid container ID')
            labels = dict(part.split('=', 1) for part in row['Labels'].split(',') if '=' in part)
            workdir = labels.get('com.docker.compose.project.working_dir', '')
            project = labels.get('com.docker.compose.project', '').lower()
            compose = labels.get('com.docker.compose.service') == 'build' and (
                project == 'nsd3decomp' or workdir == str(ROOT) or workdir.startswith(str(ROOT) + '/')
                or '/NSD3Decomp/' in workdir or workdir.endswith('/NSD3Decomp'))
            # Older receipts use immutable IDs, Compose commonly uses a tag.
            related = (compose or row['Image'] in images or row['Image'].split(':')[0] in REPOSITORIES
                       or row['Names'].startswith(PREFIXES)
                       or 'org.nsd3.resource-session=' in row['Labels'])
            if related and row['State'] not in ('exited', 'dead'):
                active.append({'id': row['ID'], 'name': row['Names'], 'state': row['State']})
        return {'daemon_id': daemon, 'active_project_containers': active,
                'global_vm_idle_proven': False}
    except (ValueError, TypeError, KeyError) as error:
        raise resources.AdmissionError('Invalid Docker admission observation: ' + str(error)) from error


def require_idle(observation):
    if observation['active_project_containers']:
        names = ', '.join(item['name'] for item in observation['active_project_containers'])
        raise resources.AdmissionError('Project Docker work may still be active: ' + names)


def read_journal():
    """Authenticate a bounded, same-user regular file; retain malformed evidence."""
    try:
        fd = os.open(JOURNAL, os.O_RDONLY | os.O_NOFOLLOW | os.O_CLOEXEC | os.O_NONBLOCK)
    except FileNotFoundError:
        return None
    try:
        before = os.fstat(fd)
        named = JOURNAL.lstat()
        if (not stat.S_ISREG(before.st_mode) or before.st_uid != os.getuid()
                or before.st_nlink != 1 or before.st_mode & 0o077 or before.st_size > MAX_JOURNAL
                or (before.st_dev, before.st_ino) != (named.st_dev, named.st_ino)):
            raise resources.AdmissionError('Unsafe Docker session journal; preserve and inspect it')
        raw = os.read(fd, MAX_JOURNAL + 1)
        after = os.fstat(fd)
        if len(raw) != before.st_size or (before.st_size, before.st_mtime_ns, before.st_ctime_ns) != (after.st_size, after.st_mtime_ns, after.st_ctime_ns):
            raise resources.AdmissionError('Docker session journal changed while reading')
        try:
            record = decode(raw)
            if (not isinstance(record, dict) or set(record) != {'kind', 'token', 'pid', 'uid', 'daemon_id', 'purpose', 'started_ns'}
                    or record['kind'] != 'nsd3-docker-session-v1' or type(record['uid']) is not int or record['uid'] != os.getuid()
                    or type(record['pid']) is not int or record['pid'] <= 0
                    or type(record['started_ns']) is not int or record['started_ns'] <= 0
                    or not isinstance(record['token'], str) or not re.fullmatch(r'[0-9a-f]{32}', record['token'])
                    or not isinstance(record['daemon_id'], str) or not 1 <= len(record['daemon_id']) <= 256
                    or not isinstance(record['purpose'], str) or not 1 <= len(record['purpose']) <= 128):
                raise ValueError('invalid session schema')
        except (ValueError, TypeError, UnicodeError) as error:
            raise resources.AdmissionError('Malformed Docker session journal; preserve and inspect it') from error
        return record, hashlib.sha256(raw).hexdigest(), (before.st_dev, before.st_ino)
    finally:
        os.close(fd)


def sync_parent():
    fd = os.open(JOURNAL.parent, os.O_RDONLY)
    try:
        os.fsync(fd)
    finally:
        os.close(fd)


def publish(record):
    raw = (json.dumps(record, sort_keys=True) + '\n').encode()
    fd = os.open(JOURNAL, os.O_WRONLY | os.O_CREAT | os.O_EXCL | os.O_NOFOLLOW | os.O_CLOEXEC, 0o600)
    try:
        with os.fdopen(fd, 'wb') as stream:
            stream.write(raw)
            stream.flush()
            os.fsync(stream.fileno())
        sync_parent()
    except BaseException:
        # Partial publication is deliberately retained as an admission blocker.
        raise
    published = read_journal()
    if published is None or published[0] != record:
        raise resources.AdmissionError('Docker intent changed during publication')
    return published


def clear(expected):
    if read_journal() != expected:
        raise resources.AdmissionError('Docker session journal changed; refusing to clear it')
    JOURNAL.unlink()
    sync_parent()


@contextmanager
def session(purpose):
    """Before staging heavy Docker work; normal failure also retains the journal."""
    if not isinstance(purpose, str) or not 1 <= len(purpose) <= 128:
        raise ValueError('bounded Docker session purpose required')
    pid = os.getpid()
    with resources.lease():
        inherited = getattr(_local, 'owner', None)
        if inherited is not None and inherited[0] == pid:
            if read_journal() != inherited[1]:
                raise resources.AdmissionError('Nested Docker session journal changed')
            try:
                yield
            except BaseException:
                _local.failed = True
                raise
            return
        pending = read_journal()
        if pending is not None:
            raise resources.AdmissionError('Unresolved Docker session: ' + pending[0]['purpose']
                + '; inspect tools/docker_admission.py status before reviewed reconciliation')
        observation = observe()
        require_idle(observation)
        expected = publish({'kind': 'nsd3-docker-session-v1', 'token': uuid.uuid4().hex,
                            'pid': pid, 'uid': os.getuid(), 'daemon_id': observation['daemon_id'],
                            'purpose': purpose, 'started_ns': time.time_ns()})
        _local.owner = (pid, expected)
        _local.failed = False
        try:
            yield
            if os.getpid() != pid:
                return  # A forked child must never clear its parent's intent.
            if _local.failed:
                raise resources.AdmissionError('Nested Docker session failed; reconciliation required')
            after = observe()
            if after['daemon_id'] != observation['daemon_id']:
                raise resources.AdmissionError('Docker daemon changed during session')
            require_idle(after)
            clear(expected)
        finally:
            if os.getpid() == pid:
                _local.owner = None


def reconcile(expected_sha256):
    """Explicit reviewed override only, never an automatic missing-container fix."""
    with resources.lease():
        pending = read_journal()
        if pending is None or pending[1] != expected_sha256:
            raise resources.AdmissionError('Exact pending journal SHA256 required')
        try:
            os.kill(pending[0]['pid'], 0)
        except ProcessLookupError:
            pass
        except PermissionError:
            raise resources.AdmissionError('Cannot establish that journal owner exited')
        else:
            raise resources.AdmissionError('Journal owner PID still exists; do not override')
        observation = observe()
        if observation['daemon_id'] != pending[0]['daemon_id']:
            raise resources.AdmissionError('Reconciliation requires the original Docker daemon')
        require_idle(observation)
        clear(pending)
        return {'status': 'operator_reviewed_override', 'journal_sha256': pending[1],
                'queued_daemon_work_absence_proven': False, 'observation': observation}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('action', choices=('status', 'reconcile'))
    parser.add_argument('--journal-sha256')
    parser.add_argument('--acknowledge-reviewed-abandoned-session', action='store_true')
    args = parser.parse_args()
    try:
        if args.action == 'reconcile':
            if not args.acknowledge_reviewed_abandoned_session or not re.fullmatch(r'[0-9a-f]{64}', args.journal_sha256 or ''):
                parser.error('reconcile requires exact --journal-sha256 and --acknowledge-reviewed-abandoned-session')
            result = reconcile(args.journal_sha256)
        else:
            pending = read_journal()
            observation = observe()
            result = {'pending': None if pending is None else {'session': pending[0], 'sha256': pending[1]},
                      'observation': observation, 'admitted': False,
                      'journal_unchanged_during_observation': read_journal() == pending}
        print(json.dumps(result, indent=2))
        return 0
    except (OSError, RuntimeError, ValueError) as error:
        print(json.dumps({'admitted': False, 'error': str(error)}))
        return 2


if __name__ == '__main__':
    raise SystemExit(main())
