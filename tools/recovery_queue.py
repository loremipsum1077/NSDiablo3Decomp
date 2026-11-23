#!/usr/bin/env python3
"""Provider-free dispatcher for already prepared, pinned recovery jobs.

`--manifest FILE --state DIR` is read-only. Add `--run` to perform at most
`--max-actions` existing receipt/landing commands. A successful validation waits
for an independently supplied `--job ID --review FILE --review-sha256 SHA`.
Failures and interrupted attempts never retry. Rebase/source preparation, claims,
review authorship are outside this controller. Optional integration_profile
reuse-reviewed-baseline delegates to the separately verified landing policy;
fresh-all is the default and grouped jobs cannot select reuse.

Manifest format 1: kind=prepared-recovery-queue-v1, canonical repo path,
queue_sha256, tools={tracked tools-relative path: sha256}, and jobs=[...].
Each job contains id, baseline/candidate full commits, mode (recovery-only or
recovery-group), profile (standard or named-writable), owner or pinned owners
manifest, pinned targets/original_elf/checker/nss, canonical lua_cache,
lua_receipt_sha256, output, seconds; named-writable additionally pins dwarfdump.
Every pinned file is {path: canonical absolute path, sha256: SHA}.
The tool map covers every tracked regular file under tools/ exactly. The known
non-runtime tools/common gitlink is excluded, matching the recovery validator;
other gitlinks and tracked symlinks are unsupported.
"""
import argparse
import fcntl
import hashlib
import json
import os
from pathlib import Path, PurePosixPath
import re
import selectors
import signal
import stat
import subprocess
import sys
import time
import uuid

KIND = 'prepared-recovery-queue-v1'
PHASES = {'prepared', 'validating', 'awaiting_review', 'landing', 'landed',
          'failed', 'uncertain', 'needs_rebase'}
RUNNING = {'validating', 'landing'}
SHA = re.compile('[0-9a-f]{64}')
COMMIT = re.compile('[0-9a-f]{40}')
LIMIT = 8 * 1024 * 1024
NON_RUNTIME_GITLINKS = {'tools/common'}


def require(ok, message):
    if not ok:
        raise ValueError(message)


def digest(path):
    h = hashlib.sha256()
    with Path(path).open('rb') as stream:
        for block in iter(lambda: stream.read(1024 * 1024), b''):
            h.update(block)
    return h.hexdigest()


def canonical(value):
    require(isinstance(value, str), 'Path must be a string')
    p = Path(value)
    require(p.is_absolute() and p == p.resolve(), 'Canonical absolute path required: ' + value)
    return p


def regular(path, limit=LIMIT):
    require(path.is_file() and not path.is_symlink() and path.stat().st_size <= limit,
            'Bounded regular file required: ' + str(path))
    return path


def read(path, limit=LIMIT):
    def pairs(items):
        result = {}
        for key, value in items:
            require(key not in result, 'Duplicate JSON key: ' + key)
            result[key] = value
        return result
    return json.loads(regular(path, limit).read_text(), object_pairs_hook=pairs)


def pin(value):
    require(isinstance(value, dict) and set(value) == {'path', 'sha256'} and
            isinstance(value['sha256'], str) and SHA.fullmatch(value['sha256']), 'Invalid file pin')
    p = canonical(value['path'])
    regular(p, 512 * 1024 * 1024)
    require(digest(p) == value['sha256'], 'File pin changed: ' + str(p))
    return p


def git(repo, *words):
    return subprocess.check_output(['git', *words], cwd=repo, stderr=subprocess.PIPE,
                                   timeout=30).decode().strip()


def owner(value):
    require(isinstance(value, str) and value and '\\' not in value and
            not PurePosixPath(value).is_absolute() and
            all(p not in ('', '.', '..') for p in value.split('/')) and
            value.endswith(('.c', '.cc', '.cpp', '.cxx')), 'Normalized original C/C++ owner required')
    return value


def owners(job):
    if job['mode'] == 'recovery-only':
        return [owner(job['owner'])]
    value = read(pin(job['owners']))
    require(set(value) == {'format', 'owners'} and value['format'] == 1 and
            isinstance(value['owners'], list) and len(value['owners']) == 2,
            'Exactly two grouped owners required')
    result = [owner(x) for x in value['owners']]
    require(result == sorted(set(result)), 'Grouped owners must be sorted and distinct')
    return result


def isolated(path, repo, cache=None):
    require(path != repo and not any(path.is_relative_to(repo / x) for x in
            ('.git', 'src', 'include', 'data', 'tools', 'toolchain')) and
            (cache is None or not path.is_relative_to(cache)), 'Output/state overlaps protected tree')


def manifest(path):
    m = read(path)
    require(set(m) == {'format', 'kind', 'repo', 'queue_sha256', 'tools', 'jobs'} and
            m['format'] == 1 and m['kind'] == KIND, 'Unsupported queue manifest')
    require(m['queue_sha256'] == digest(Path(__file__)), 'Queue implementation pin changed')
    repo = canonical(m['repo'])
    require(Path(git(repo, 'rev-parse', '--show-toplevel')) == repo and
            Path(git(repo, 'rev-parse', '--path-format=absolute', '--git-common-dir')) == repo / '.git',
            'Primary checkout required')
    paths = []
    for record in git(repo, 'ls-files', '--stage', '-z', '--', 'tools').split('\0'):
        if not record:
            continue
        header, rel = record.split('\t', 1); mode, _, stage = header.split()
        require(stage == '0', 'Unmerged tool index entry')
        if mode == '160000':
            require(rel in NON_RUNTIME_GITLINKS, 'Unsupported runtime tool gitlink: ' + rel)
            continue
        require(mode in ('100644', '100755'), 'Unsupported tracked tool kind: ' + rel)
        paths.append(rel)
    require(isinstance(m['tools'], dict) and set(m['tools']) == set(paths) and
            {'tools/receipts.py', 'tools/land.py'} <= set(paths), 'Incomplete tracked tool closure')
    for rel, expected in m['tools'].items():
        p = canonical(str(repo / rel))
        require(isinstance(expected, str) and SHA.fullmatch(expected) and
                digest(regular(p, 64 * 1024 * 1024)) == expected, 'Tool changed: ' + rel)
    require(isinstance(m['jobs'], list) and 1 <= len(m['jobs']) <= 64, 'Queue requires 1..64 jobs')
    ids, outputs = set(), []
    for j in m['jobs']:
        required = {'id', 'baseline', 'candidate', 'mode', 'profile', 'targets', 'original_elf',
                    'checker', 'nss', 'lua_cache', 'lua_receipt_sha256', 'output', 'seconds'}
        require(isinstance(j, dict) and j.get('mode') in ('recovery-only', 'recovery-group'), 'Recovery mode required')
        required.add('owner' if j['mode'] == 'recovery-only' else 'owners')
        if j.get('profile') == 'named-writable':
            required.add('dwarfdump')
        require(required <= set(j) <= required | {'integration_profile'} and
                j['profile'] in ('standard', 'named-writable'), 'Unsupported job fields/profile')
        require(j['mode'] == 'recovery-only' or j['profile'] == 'standard', 'Grouped writable-data/DWARF profile unsupported')
        integration = j.get('integration_profile', 'fresh-all')
        require(integration in ('fresh-all', 'reuse-reviewed-baseline') and
                (integration == 'fresh-all' or j['mode'] == 'recovery-only'), 'Unsupported integration profile')
        require(isinstance(j['id'], str) and re.fullmatch('[A-Za-z0-9][A-Za-z0-9_-]{0,63}', j['id']) and
                j['id'] not in ids, 'Unique bounded job id required')
        ids.add(j['id'])
        for key in ('baseline', 'candidate'):
            require(isinstance(j[key], str) and COMMIT.fullmatch(j[key]) and
                    git(repo, 'rev-parse', j[key] + '^{commit}') == j[key], 'Exact existing commit required')
        require(j['baseline'] != j['candidate'], 'Candidate must differ from baseline')
        subprocess.run(['git', 'merge-base', '--is-ancestor', j['baseline'], j['candidate']],
                       cwd=repo, check=True, timeout=30, stdout=subprocess.DEVNULL, stderr=subprocess.PIPE)
        for key in ('targets', 'original_elf', 'checker', 'nss'):
            pin(j[key])
        if 'dwarfdump' in j:
            pin(j['dwarfdump'])
        owners(j)
        cache = canonical(j['lua_cache'])
        require(cache.is_dir(), 'Lua cache directory missing')
        require(isinstance(j['lua_receipt_sha256'], str) and SHA.fullmatch(j['lua_receipt_sha256']), 'Lua receipt pin required')
        pin({'path': str(cache / 'receipt.json'), 'sha256': j['lua_receipt_sha256']})
        require(type(j['seconds']) is int and 1 <= j['seconds'] <= 86400, 'Job seconds must be 1..86400')
        output = canonical(j['output']); isolated(output, repo, cache)
        require(not any(output.is_relative_to(p) or p.is_relative_to(output) for p in outputs), 'Overlapping receipt outputs')
        outputs.append(output)
    return m


def atomic(path, value):
    temporary = path.with_name('.' + path.name + '.' + uuid.uuid4().hex)
    try:
        with temporary.open('x') as stream:
            json.dump(value, stream, sort_keys=True, separators=(',', ':')); stream.write('\n')
            stream.flush(); os.fsync(stream.fileno())
        os.replace(temporary, path)
        fd = os.open(path.parent, os.O_RDONLY)
        try:
            os.fsync(fd)
        finally:
            os.close(fd)
    finally:
        if temporary.exists():
            temporary.unlink()


def load_state(directory, m, expected):
    path = directory / 'state.json'
    if not path.exists():
        return {'format': 1, 'manifest_sha256': expected, 'provider_calls': 0,
                'jobs': {j['id']: {'phase': 'prepared', 'attempts': []} for j in m['jobs']}}
    value = read(path)
    require(value.get('format') == 1 and value.get('manifest_sha256') == expected and
            value.get('provider_calls') == 0 and set(value['jobs']) == {j['id'] for j in m['jobs']}, 'Queue state binding changed')
    require(all(v.get('phase') in PHASES and isinstance(v.get('attempts'), list) and len(v['attempts']) <= 2
                for v in value['jobs'].values()), 'Invalid queue state')
    return value


def build_command(m, j):
    args = [sys.executable, str(Path(m['repo']) / 'tools/receipts.py'), 'build', '--repo', m['repo'],
            '--baseline', j['baseline'], '--candidate', j['candidate'], '--mode', j['mode'],
            '--output', j['output'], '--lua-cache', j['lua_cache'], '--lua-receipt-sha256', j['lua_receipt_sha256']]
    for field in ('original_elf', 'checker', 'nss', 'targets'):
        flag = field.replace('_', '-')
        args += ['--' + flag, j[field]['path']]
        if field != 'targets':
            args += ['--' + ('original' if field == 'original_elf' else flag) + '-sha256', j[field]['sha256']]
    args += ['--owner', j['owner']] if j['mode'] == 'recovery-only' else ['--owners', j['owners']['path']]
    if 'dwarfdump' in j:
        args += ['--dwarfdump', j['dwarfdump']['path'], '--dwarfdump-sha256', j['dwarfdump']['sha256']]
    return args + ['--seconds', str(j['seconds']), '--run']


def land_command(m, j, review):
    args = [sys.executable, str(Path(m['repo']) / 'tools/land.py'), *owners(j), '--' + j['mode'],
            '--recovery-receipts', j['output'], '--review', review['path'], '--review-sha256', review['sha256']]
    for field in ('original_elf', 'checker', 'nss'):
        args += ['--' + field.replace('_', '-'), j[field]['path']]
    if j.get('integration_profile', 'fresh-all') == 'reuse-reviewed-baseline':
        args.append('--reuse-reviewed-baseline')
    return args + ['--lua-cache', j['lua_cache'], '--seconds', str(j['seconds'])]


def validation_pins(j):
    output = Path(j['output']); run = read(output / 'run.json'); binding = read(output / 'binding.json', 64 * 1024 * 1024)
    audit = 'recovery-group-audit.json' if j['mode'] == 'recovery-group' else 'recovery-audit.json'
    names = ['binding.json', 'evidence.json', audit, 'review-template.json', 'run.json']
    pins = {name: digest(regular(output / name, 64 * 1024 * 1024)) for name in names}
    require(run.get('status') == 'recovery_evidence_complete' and run.get('accepted') is False and
            run.get('provider_calls') == 0 and all(run.get(key) == pins[name] for key, name in
            (('binding_sha256', 'binding.json'), ('evidence_sha256', 'evidence.json'), ('audit_sha256', audit))),
            'Incomplete or changed receipt artifacts')
    require(binding.get('policy') == j['mode'] and binding.get('post_landing') is False and
            'baseline_reference' not in binding and all(binding['versions'][v]['commit'] == j[v]
            for v in ('baseline', 'candidate')), 'Receipt source/policy binding differs')
    require(([binding.get('owner')] if j['mode'] == 'recovery-only' else binding.get('owners')) == owners(j), 'Receipt owners differ')
    return pins


class Uncertain(Exception):
    pass


def dispatch(command, log, seconds, started):
    """Bound host output/time; forced stops leave uncertain state for inspection."""
    p = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, start_new_session=True)
    try:
        started(p.pid); deadline = time.monotonic() + seconds; total = 0
        with log.open('xb') as out, selectors.DefaultSelector() as selector:
            selector.register(p.stdout, selectors.EVENT_READ)
            while selector.get_map():
                left = deadline - time.monotonic()
                if left <= 0:
                    raise Uncertain('Command timed out; inspect retained process/container artifacts')
                for key, _ in selector.select(min(left, .5)):
                    data = os.read(key.fileobj.fileno(), 65536)
                    if not data:
                        selector.unregister(key.fileobj); continue
                    total += len(data)
                    if total > LIMIT:
                        raise Uncertain('Command exceeded bounded log output')
                    out.write(data)
            out.flush(); os.fsync(out.fileno())
        return p.wait(timeout=max(.01, deadline - time.monotonic()))
    except subprocess.TimeoutExpired as error:
        raise Uncertain('Command did not exit after closing output') from error
    finally:
        if p.poll() is None:
            os.killpg(p.pid, signal.SIGTERM)
            try:
                p.wait(timeout=2)
            except subprocess.TimeoutExpired:
                os.killpg(p.pid, signal.SIGKILL); p.wait(timeout=2)
        p.stdout.close()


def execute(args):
    path = canonical(str(args.manifest.absolute())); m = manifest(path); expected = digest(path)
    directory = canonical(str(args.state.absolute())); isolated(directory, Path(m['repo']))
    for j in m['jobs']:
        isolated(directory, Path(m['repo']), canonical(j['lua_cache']))
        output = Path(j['output'])
        require(not (directory.is_relative_to(output) or output.is_relative_to(directory)), 'State and receipt outputs overlap')
    require(not path.is_relative_to(directory), 'Immutable manifest must be outside state directory')
    require(args.run or not any((args.review, args.review_sha256)), 'Review submission requires --run')
    require(bool(args.review) == bool(args.review_sha256) and (not args.review or args.job), 'Review requires --job, --review and --review-sha256')
    if args.job:
        require(args.job in {j['id'] for j in m['jobs']}, 'Unknown job')
    state = load_state(directory, m, expected)
    if not args.run:
        head = git(Path(m['repo']), 'rev-parse', 'HEAD')
        require(not git(Path(m['repo']), 'status', '--porcelain', '--untracked-files=all'), 'Primary checkout must be clean')
        return {'status': 'preflight', 'writes': 0, 'provider_calls': 0, 'admission': False,
                'jobs': {j['id']: ('uncertain' if state['jobs'][j['id']]['phase'] in RUNNING else
                         'needs_rebase' if state['jobs'][j['id']]['phase'] in ('prepared', 'awaiting_review') and
                         head != j['baseline'] else state['jobs'][j['id']]['phase']) for j in m['jobs']}}
    directory.mkdir(parents=True, exist_ok=True)
    fd = os.open(directory / '.lock', os.O_RDWR | os.O_CREAT | os.O_NOFOLLOW, 0o600)
    try:
        fcntl.flock(fd, fcntl.LOCK_EX | fcntl.LOCK_NB)
        locked = os.fstat(fd)
        def check_lock():
            current = os.stat(directory / '.lock', follow_symlinks=False)
            require(stat.S_ISREG(locked.st_mode) and stat.S_ISREG(current.st_mode) and
                    current.st_nlink == 1 and (current.st_dev, current.st_ino) ==
                    (locked.st_dev, locked.st_ino), 'Queue lock inode replaced; inspect interrupted attempt')
        check_lock()
        state = load_state(directory, m, expected)
        def save():
            check_lock(); atomic(directory / 'state.json', state)
        for row in state['jobs'].values():
            if row['phase'] in RUNNING:
                row.update(phase='uncertain', error='Interrupted attempt; inspect artifacts, never automatically retry')
        save(); actions = 0
        if args.review:
            require(state['jobs'][args.job]['phase'] == 'awaiting_review', 'Review submission requires completed validation awaiting review')
        for j in m['jobs']:
            if args.job and j['id'] != args.job:
                continue
            row = state['jobs'][j['id']]
            if row['phase'] not in ('prepared', 'awaiting_review') or actions >= args.max_actions:
                continue
            # Reauthenticate immediately before every subprocess, including tool closure.
            require(manifest(path) == m and digest(path) == expected, 'Manifest changed')
            repo = Path(m['repo'])
            if git(repo, 'rev-parse', 'HEAD') != j['baseline']:
                row.update(phase='needs_rebase', error='Primary baseline changed; prepare a new source/manifest externally'); save(); continue
            require(not git(repo, 'status', '--porcelain', '--untracked-files=all'), 'Primary checkout must be clean')
            if row['phase'] == 'prepared':
                require(not Path(j['output']).exists(), 'Receipt output already exists; reconcile outside queue')
                command = build_command(m, j); phase = 'validating'
            else:
                require(validation_pins(j) == row['validation_artifacts'], 'Validated artifacts changed')
                if not args.review or args.job != j['id']:
                    continue
                review = {'path': str(canonical(str(args.review.absolute()))), 'sha256': args.review_sha256}
                pin(review); require('review' not in row or row['review'] == review, 'Supplied review changed')
                row['review'] = review; command = land_command(m, j, review); phase = 'landing'
            attempt = directory / (j['id'] + '-' + phase)
            attempt.mkdir(exist_ok=False)
            atomic(attempt / 'command.json', command)
            row.update(phase=phase); row['attempts'].append({'phase': phase, 'command_sha256': digest(attempt / 'command.json')}); save()
            def started(pid):
                row['attempts'][-1]['pid'] = pid; save()
            try:
                check_lock()
                code = dispatch(command, attempt / 'output.log', j['seconds'], started)
                check_lock()
                row['attempts'][-1].update(returncode=code, log_sha256=digest(attempt / 'output.log'))
                require(code == 0, 'Existing command failed; retained artifacts require inspection')
                require(manifest(path) == m and digest(path) == expected, 'Inputs/tools changed during command')
                if phase == 'validating':
                    require(git(repo, 'rev-parse', 'HEAD') == j['baseline'] and not git(repo, 'status', '--porcelain'), 'Primary changed during validation')
                    row.update(phase='awaiting_review', validation_artifacts=validation_pins(j))
                else:
                    lines = (attempt / 'output.log').read_text().splitlines()
                    result = json.loads(lines[-1])
                    status = 'recovery_group_landed' if j['mode'] == 'recovery-group' else 'recovery_landed'
                    require(result.get('status') == status and result.get('strict_certified') is False and
                            COMMIT.fullmatch(result.get('commit', '')), 'Landing result identity missing')
                    landed = canonical(result['retained']) / 'landed.json'; record = read(landed)
                    require(record.get('commit') == result['commit'] and record.get('status') == status and
                            record.get('strict_certified') is False and record.get('review_sha256') == row['review']['sha256'] and
                            git(repo, 'rev-parse', 'HEAD') == result['commit'] and not git(repo, 'status', '--porcelain'), 'Landing receipt/HEAD differs')
                    row.update(phase='landed', commit=result['commit'], landed_path=str(landed), landed_sha256=digest(landed))
            except (KeyboardInterrupt, Uncertain) as error:
                row.update(phase='uncertain', error=str(error) or 'Interrupted'); save()
                if isinstance(error, KeyboardInterrupt):
                    raise
            except (ValueError, OSError, KeyError, TypeError, subprocess.SubprocessError) as error:
                # Landing may have advanced HEAD before its final receipt failed.
                row.update(phase='uncertain' if phase == 'landing' else 'failed', error=str(error))
            save(); actions += 1
        return {'status': 'queue_checkpoint', 'provider_calls': 0, 'admission': False, 'actions': actions,
                'jobs': {k: v['phase'] for k, v in state['jobs'].items()}}
    finally:
        os.close(fd)


def main(argv=None):
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--manifest', type=Path, required=True)
    p.add_argument('--state', type=Path, required=True)
    p.add_argument('--run', action='store_true')
    p.add_argument('--job')
    p.add_argument('--review', type=Path)
    p.add_argument('--review-sha256')
    p.add_argument('--max-actions', type=int, default=1)
    args = p.parse_args(argv)
    try:
        require(1 <= args.max_actions <= 16, '--max-actions must be 1..16')
        print(json.dumps(execute(args), sort_keys=True)); return 0
    except (ValueError, OSError, KeyError, TypeError, subprocess.SubprocessError) as error:
        print(json.dumps({'status': 'blocked', 'error': str(error), 'provider_calls': 0, 'admission': False}, sort_keys=True)); return 1


if __name__ == '__main__':
    raise SystemExit(main())
