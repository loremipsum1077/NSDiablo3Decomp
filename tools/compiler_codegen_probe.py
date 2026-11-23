#!/usr/bin/env python3
"""Run bounded offline Lua backend probes on a caller-pinned LLVM IR file.

This is diagnostic only. Variant flags never change the repository recipe or
grant source or exact-match credit. Output is retained in a new private directory.
"""
import argparse
import json
import os
from pathlib import Path
import re
import selectors
import shutil
import subprocess
import sys
import time
import uuid

import resource_admission
from verify_original_inputs import fingerprint

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / 'toolchain'))
import backend

MAX_IR_BYTES = 512 * 1024
MAX_LOG_BYTES = 4 * 1024 * 1024
MAX_OBJECT_BYTES = 4 * 1024 * 1024
PROBE_SECONDS = 60


def sha(path):
    return fingerprint(path)['sha256']


def stop_container(name):
    """Confirm the named container is stopped before releasing the lease."""
    killed = subprocess.run(['docker', 'kill', name], capture_output=True, text=True, timeout=10)
    if killed.returncode == 0:
        return
    inspected = subprocess.run(['docker', 'inspect', '--format', '{{.State.Running}}', name],
                               capture_output=True, text=True, timeout=10)
    if inspected.returncode == 0 and inspected.stdout.strip() == 'false':
        return
    if inspected.returncode and ('No such object' in inspected.stderr or 'No such container' in inspected.stderr):
        return
    raise RuntimeError('could not confirm probe container cleanup: ' + name)


def bounded_run(cmd, name):
    """Drain two bounded pipes, killing the named container on timeout/overflow."""
    proc = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    selector = selectors.DefaultSelector()
    streams = {'stdout': bytearray(), 'stderr': bytearray()}
    deadline = time.monotonic() + PROBE_SECONDS
    try:
        for label in streams:
            selector.register(getattr(proc, label), selectors.EVENT_READ, label)
        while selector.get_map():
            remaining = deadline - time.monotonic()
            if remaining <= 0:
                raise RuntimeError('backend probe timed out')
            for key, _ in selector.select(min(remaining, 1)):
                block = os.read(key.fileobj.fileno(), 65536)
                if not block:
                    selector.unregister(key.fileobj)
                    continue
                data = streams[key.data]
                if len(data) + len(block) > MAX_LOG_BYTES:
                    raise RuntimeError('backend probe log limit exceeded')
                data.extend(block)
        remaining = deadline - time.monotonic()
        if remaining <= 0:
            raise RuntimeError('backend probe timed out')
        code = proc.wait(timeout=remaining)
        return code, bytes(streams['stdout']), bytes(streams['stderr'])
    except BaseException:
        try:
            stop_container(name)
        finally:
            if proc.poll() is None:
                proc.kill()
            proc.wait()
        raise
    finally:
        selector.close()
        for label in streams:
            getattr(proc, label).close()


def diagnostic_variants(flags, isel_debug=False, dag_debug=False):
    variants = {
        'baseline': flags,
        'dag': [x for x in flags if x != '-fast-isel=true'] + ['-fast-isel=false'],
        'loadchain': [x for x in flags if x != '-experiment-disable-o0-load-chain=true'],
        'trace': flags + ['-filter-print-funcs=callallgcTM', '-print-after-all'],
    }
    if isel_debug:
        variants['isel-debug'] = flags + ['-debug-only=isel']
    if dag_debug:
        variants['dag-debug'] = flags + ['-debug-only=dagcombine']
    return variants


def run(ir, expected_ir, cache, image, output, isel_debug=False, dag_debug=False):
    ir, cache, output = Path(ir).absolute(), Path(cache).resolve(), Path(output).resolve()
    if not output.is_relative_to((ROOT / 'build').resolve()) or output == (ROOT / 'build').resolve():
        raise ValueError('output must be a new directory under build/')
    if ir.stat().st_size > MAX_IR_BYTES:
        raise ValueError('IR exceeds bounded probe size')
    if not re.fullmatch(r'[0-9a-f]{64}', expected_ir) or sha(ir) != expected_ir:
        raise ValueError('IR pin mismatch')
    if not re.fullmatch(r'sha256:[0-9a-f]{64}', image):
        raise ValueError('immutable image ID required')
    paths = [ROOT / 'toolchain/lua/config.json', ROOT / 'toolchain/backend.py',
             ROOT / 'tools/resource_admission.py', ROOT / 'tools/verify_original_inputs.py', Path(__file__)]
    pins = {str(path): sha(path) for path in paths}
    receipt = backend.validate(ROOT, cache)
    if output.exists():
        raise ValueError('output exists; preserve earlier results')
    flags = json.loads((ROOT / 'toolchain/lua/config.json').read_text())['flags']
    if flags.count('-fast-isel=true') != 1 or flags.count('-experiment-disable-o0-load-chain=true') != 1:
        raise ValueError('unexpected baseline flags; review variants before use')
    variants = diagnostic_variants(flags, isel_debug, dag_debug)
    with resource_admission.lease():
        resource_admission.check(str(output), growth_bytes=128 * 1024**2)
        output.mkdir(parents=True)
        shutil.copyfile(ir, output / 'input.ll')
        if sha(output / 'input.ll') != expected_ir:
            raise ValueError('IR changed while copying')
        records = {}
        for name, options in variants.items():
            container = 'd3-codegen-probe-' + uuid.uuid4().hex
            cmd = ['docker', 'run', '--rm', '--name', container, '--pull', 'never', '--network', 'none',
                   '--log-driver', 'none', '--ulimit', f'fsize={MAX_OBJECT_BYTES}:{MAX_OBJECT_BYTES}',
                   '--memory', '2g', '--cpus', '2', '--read-only',
                   '--mount', f'type=bind,src={output},dst=/out',
                   '--mount', f'type=bind,src={cache},dst=/lua-cache,readonly',
                   image, '/lua-cache/out/bin/llc', *options, '-function-sections', '-filetype=obj',
                   '/out/input.ll', '-o', '/out/' + name + '.o']
            try:
                code, stdout, stderr = bounded_run(cmd, container)
            except (OSError, RuntimeError, subprocess.SubprocessError) as error:
                records[name] = {'command': cmd, 'error': str(error)}
                (output / 'results.json').write_text(json.dumps({'status': 'failed', 'accepted': False, 'records': records}, indent=2) + '\n')
                raise
            (output / (name + '.stdout')).write_bytes(stdout)
            (output / (name + '.stderr')).write_bytes(stderr)
            record = {'command': cmd, 'returncode': code,
                      'stdout_sha256': sha(output / (name + '.stdout')),
                      'stderr_sha256': sha(output / (name + '.stderr'))}
            if code:
                records[name] = record
                (output / 'results.json').write_text(json.dumps({'status': 'failed', 'accepted': False, 'records': records}, indent=2) + '\n')
                raise RuntimeError('backend probe failed: ' + name)
            if (output / (name + '.o')).stat().st_size > MAX_OBJECT_BYTES:
                raise RuntimeError('backend probe object exceeds size limit: ' + name)
            record['object_sha256'] = sha(output / (name + '.o'))
            records[name] = record
        if (sha(output / 'input.ll') != expected_ir or backend.validate(ROOT, cache) != receipt or
                any(sha(path) != digest for path, digest in pins.items())):
            raise ValueError('IR or Lua backend changed during probing')
        report = {'kind': 'lua-backend-codegen-probe-v1', 'accepted': False,
                  'ir_sha256': expected_ir, 'compiler_sha256': receipt['binary_sha256'],
                  'compiler_receipt_sha256': sha(cache / 'receipt.json'), 'image': image,
                  'config_sha256': sha(ROOT / 'toolchain/lua/config.json'),
                  'probe_sha256': sha(__file__),
                  'fingerprint_sha256': sha(ROOT / 'tools/verify_original_inputs.py'),
                  'tool_pins': pins, 'records': records}
        (output / 'results.json').write_text(json.dumps(report, indent=2) + '\n')
    return report


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--ir', type=Path, required=True)
    parser.add_argument('--ir-sha256', required=True)
    parser.add_argument('--cache', type=Path, required=True)
    parser.add_argument('--image', required=True)
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--isel-debug', action='store_true', help='also retain bounded ISel DAG diagnostics')
    parser.add_argument('--dag-debug', action='store_true', help='also retain bounded DAG combiner diagnostics')
    args = parser.parse_args()
    try:
        run(args.ir, args.ir_sha256, args.cache, args.image, args.output, args.isel_debug, args.dag_debug)
        print(json.dumps({'status': 'complete', 'accepted': False, 'output': str(args.output)}))
        return 0
    except (OSError, ValueError, RuntimeError, subprocess.SubprocessError) as error:
        print(json.dumps({'status': 'failed', 'accepted': False, 'error': str(error)}))
        return 1


if __name__ == '__main__':
    raise SystemExit(main())
