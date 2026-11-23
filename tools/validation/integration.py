"""Provider-free CPU build/check and comparison primitives; existing gate policy retained."""
import csv
import os
from pathlib import Path
import re
import shutil
import subprocess
import time

from . import common as w


def identities(rows, address='address', name='name'):
    """Read exact, unique checker/inventory identities without silently deduplicating."""
    if not isinstance(rows, list):
        raise ValueError('Invalid identity list')
    result = set()
    for row in rows:
        if (not isinstance(row, dict) or not isinstance(row.get(address), str) or
                not re.fullmatch(r'0x[0-9a-fA-F]+', row[address]) or
                not isinstance(row.get(name), str) or not row[name].strip()):
            raise ValueError('Invalid address/name identity')
        identity = row[address], row[name]
        if identity in result:
            raise ValueError('Duplicate identity')
        result.add(identity)
    return result


def comparison(before, after, targets):
    """One selected-target policy for receipt construction and report diagnostics.

    Baseline U targets may be absent from the non-U receipt request. Only explicitly
    selected identities may be added; all previously checked identities must remain.
    """
    required = identities(targets, 'Address', 'Name')
    old_ids, new_ids = identities(before['results']), identities(after['results'])
    if (not 1 <= len(required) <= 64 or not old_ids or not old_ids <= new_ids or
            not required <= new_ids or not new_ids - old_ids <= required):
        raise ValueError('Comparison scope/identity mismatch')
    known = {'ok', 'mismatch', 'not-checkable', 'missing', 'error'}
    if any(row.get('status') not in known for row in before['results'] + after['results']):
        raise ValueError('Invalid checker status')
    old = {(r['address'], r['name']): r['status'] for r in before['results']}
    new = {(r['address'], r['name']): r['status'] for r in after['results']}
    regressions = sum(new[k] != status and not (k in required and new[k] == 'ok')
                      for k, status in old.items())
    pending = sum(new[k] != 'ok' for k in required)
    unknown = sum(status == new[k] and status not in ('ok', 'mismatch') for k, status in old.items())
    return {'regressions': regressions, 'owner_not_ok': pending, 'preexisting_unverifiable': unknown}


def compare(before, after, targets):
    try:
        result = comparison(before, after, targets)
    except (KeyError, TypeError, ValueError):
        return False
    # Receipt construction retains unchanged unknowns; the report marks them
    # partial. Checker errors still prohibit construction, as before.
    return (not result['regressions'] and not result['owner_not_ok'] and
            not any(row['status'] == 'error' for row in after['results']))


def run(command, log, deadline, cwd=None, retries=0):
    if time.monotonic() >= deadline:
        raise w.Halt('Integration runtime cap')
    marker = log.with_suffix('.stage.json')
    previous = w.read(marker) if marker.exists() else {'attempts':0}
    if previous.get('command_id') not in (None,w.digest(command)):
        raise w.Halt('Stage command changed')
    if previous.get('status')=='complete':
        return
    if previous['attempts'] >= retries+1:
        raise w.Halt('Deterministic stage retry cap: '+str(log))
    attempt = previous['attempts']+1
    if attempt>1:
        log = log.with_name(log.stem+'.retry'+str(attempt)+log.suffix)
    w.write(log.with_suffix('.command.json'), command,True)
    w.write(marker,{'attempts':attempt,'status':'started','command_id':w.digest(command)})
    with log.open('xb') as stream:
        try:
            env = {key: value for key, value in os.environ.items() if key in
                   ('PATH', 'HOME', 'DOCKER_HOST', 'DOCKER_CONTEXT', 'DOCKER_CONFIG', 'SSH_AUTH_SOCK')}
            env.update(LANG='C', LC_ALL='C')
            p = subprocess.run(command, stdin=subprocess.DEVNULL, stdout=stream,
                               stderr=subprocess.STDOUT, cwd=cwd, env=env,
                               timeout=deadline-time.monotonic())
        except subprocess.TimeoutExpired as error:
            if command[:2] == ['docker','run'] and '--name' in command:
                name = command[command.index('--name')+1]
                try:
                    subprocess.run(['docker','kill',name],stdout=subprocess.DEVNULL,
                                   stderr=subprocess.DEVNULL,timeout=5)
                except (OSError,subprocess.TimeoutExpired):
                    pass
            raise w.Halt('Integration timeout; inspect recorded container before resume') from error
    if p.returncode:
        raise w.Halt('Integration failed: ' + str(log))
    w.write(marker,{'attempts':attempt,'status':'complete','command_id':w.digest(command)})


def snapshot(repo, dest):
    dest.mkdir(parents=True)
    for name in ('src', 'include', 'toolchain'):
        ignored={'bin','nx-decomp-tools-binaries','__pycache__'} if name=='toolchain' else set()
        if any(p.is_symlink() for p in (repo/name).rglob('*') if not set(p.relative_to(repo/name).parts)&ignored):
            raise w.Halt('Snapshot symlink requires explicit resolution: '+name)
        shutil.copytree(repo/name, dest/name,ignore=shutil.ignore_patterns(*ignored))
    shutil.copy2(repo/'CMakeLists.txt', dest/'CMakeLists.txt')


def build_check(root, repo, dest, mode, deadline, name, retries=0, *, images=None, lua_cache=None, recovery_targets=None):
    dest.mkdir(parents=True, exist_ok=True)
    done = dest/'done.json'
    if done.exists():
        old = w.read(done)
        if recovery_targets is not None:
            if old.get('kind') != 'recovery-only-mode-v1' or old.get('mode') != mode or old.get('full_check_executed') is not False:
                raise w.Halt('Wrong cached recovery receipt mode')
        elif set(old) != {'full_check_rc','files'} or old['full_check_rc'] != 0:
            raise w.Halt('Recovery receipt cannot certify exactness')
        for path, expected in old['files'].items():
            if w.sha(dest/path) != expected:
                raise w.Halt('Cached integration output changed')
        return
    build = root/'build/2.6.2'
    build.mkdir(parents=True, exist_ok=True)
    (root/'build/lua-toolchain').mkdir(parents=True, exist_ok=True)
    image = images[mode] if images else ('d3-build:5.0.1' if mode == 'normal' else 'd3-lua-build:5.0.1')
    args = ['docker', 'run', '--rm', '--pull', 'never', '--name', name, '--network', 'none', '--read-only',
            '--cap-drop', 'ALL', '--security-opt', 'no-new-privileges', '--memory', '4g', '--cpus', '2',
            '--pids-limit', '256', '--mount', f'type=bind,src={root},dst=/work,readonly',
            '--mount', f'type=bind,src={build},dst=/work/build/2.6.2',
            '--tmpfs', '/tmp:rw,nosuid,size=512m', '-w', '/work']
    if mode == 'lua':
        if lua_cache is None:
            import sys
            sys.path.insert(0, str(repo / 'toolchain'))
            import backend
            lua_cache = backend.resolve(repo)
        lua_cache = Path(lua_cache)
        if not (lua_cache/'receipt.json').exists():
            raise w.Halt('Existing Lua compiler cache unavailable; no bootstrap authorized')
        args += ['--mount', f'type=bind,src={lua_cache},dst=/work/build/lua-toolchain,readonly',
                 '-e', 'D3_LUA_TOOLCHAIN=/work/build/lua-toolchain',
                 '-e', 'D3_DOCKER_PLATFORM=' + os.environ.get('D3_DOCKER_PLATFORM', 'linux/arm64')]
    args += [image]
    run(args+['cmake', '-G', 'Ninja', '-DCMAKE_TOOLCHAIN_FILE=toolchain/ToolchainNX64.cmake',
              '-DD3_LUA_COMPILER='+('ON' if mode == 'lua' else 'OFF'), '-B', 'build/2.6.2'], dest/'configure.log', deadline,retries=retries)
    run(args+['cmake', '--build', 'build/2.6.2', '--', '-j2'], dest/'build.log', deadline,retries=retries)
    # Native checker is trusted; generated d3 is read as ELF, never executed.
    for relative in ('tools/check', 'tools/config.toml', 'data/2.6.2/functions.csv',
                     'data/2.6.2/data_symbols.csv', 'data/2.6.2/function_files.csv',
                     'build/2.6.2/d3', 'build/2.6.2/d3.map', 'build/2.6.2/d3.map.provenance'):
        out = dest/relative
        out.parent.mkdir(parents=True, exist_ok=True)
        source = root/relative if (root/relative).is_file() else repo/relative
        # Full checker updates only this private inventory. Preserve its completed
        # checkpoint when resuming between full-check and batch-check stages.
        if not (relative.startswith('data/') and out.exists()):
            shutil.copy2(source, out)
    # Original dump is copied, never linked to a writable user path.
    if not (dest/'data/2.6.2/main.elf').exists():
        shutil.copy2(repo/'data/2.6.2/main.elf', dest/'data/2.6.2/main.elf')
    (dest/'src').mkdir(exist_ok=True)
    if recovery_targets is None:
        run([str(dest/'tools/check'), '--warnings-as-errors'], dest/'full-check.log', deadline, dest,retries)
    rows = w.rows(dest/'data/2.6.2/functions.csv')
    selected = {(r['address'], r['name']) for r in recovery_targets or []}
    w.write(dest/'request.json', {'functions':[{'address':r['Address'],'name':r['Name']} for r in rows
        if r['Quality'] != 'U' or (r['Address'], r['Name']) in selected]})
    run([str(dest/'tools/check'), '--batch', 'request.json', '--batch-output', 'result.json'], dest/'batch-check.log', deadline, dest,retries)
    if any(r['status']=='error' for r in w.read(dest/'result.json')['results']):
        raise w.Halt('Full checker errors')
    # Retain both-mode owner objects independently of subsequent reconfiguration.
    for obj in build.rglob('*.obj'):
        out = dest/'objects'/obj.relative_to(build)
        out.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(obj, out)
    files = {str(p.relative_to(dest)):w.sha(p) for p in dest.rglob('*') if p.is_file()}
    receipt = {'full_check_rc': 0, 'files': files} if recovery_targets is None else {
        'kind': 'recovery-only-mode-v1', 'mode': mode, 'full_check_executed': False, 'files': files}
    w.write(done, receipt, True)
