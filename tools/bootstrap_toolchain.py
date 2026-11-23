"""Composable compiler-image preparation for tools/bootstrap.py; no game build."""
import importlib.util
import os
from pathlib import Path
import platform
import shlex
import shutil
import subprocess
import sys


def prepare(root, execute=False, allow_downloads=False):
    try:
        if execute and not allow_downloads:
            raise RuntimeError('--prepare-toolchain requires --allow-downloads; Docker may download images, packages and LLVM')
        if sys.version_info < (3, 10):
            raise RuntimeError('Python 3.10+ required by contributor tools')
        if platform.system() not in ('Linux', 'Darwin'):
            raise RuntimeError('toolchain preparation supports macOS/Linux; see docs/contributor_startup.md')
        machine = platform.machine().lower()
        selected = {'aarch64': 'linux/arm64', 'arm64': 'linux/arm64',
                    'x86_64': 'linux/amd64', 'amd64': 'linux/amd64'}.get(machine)
        if not selected:
            raise RuntimeError('unsupported host architecture: ' + machine)
        # Use Compose's actual default, and reject accidental x86 emulation.
        requested = os.environ.get('D3_DOCKER_PLATFORM', 'linux/arm64')
        if requested != selected:
            raise RuntimeError('set D3_DOCKER_PLATFORM=' + selected + ' for this host')
        spec = importlib.util.spec_from_file_location('d3_llvm_archive', Path(root) / 'toolchain/llvm_archive.py')
        module = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(module)
        entry = module.select(requested, os.environ.get('CLANG_VERSION', '5.0.1'),
                              os.environ.get('LLVM_ARCH', ''), os.environ.get('CLANG_URL', ''))
        if shutil.which('docker') is None:
            raise RuntimeError('docker missing from PATH; provision Docker/Compose explicitly')
        subprocess.run(['docker', 'compose', 'version'], cwd=root, check=True,
                       capture_output=True, text=True, timeout=20)
        command = ['docker', 'compose', 'build', 'build']
        print('LLVM archive: ' + entry['filename'] + ' (locked SHA-256 ' + entry['sha256'] + ')')
        print('Pin authentication limits: toolchain/llvm-downloads.md')
        print('Plan: ' + shlex.join(command), flush=True)
        print('This prepares only the Linux compiler image. Supply your own original inputs before --check/--run-reproduce.')
        if execute:
            # Probe an already-running daemon; do not start services or change context.
            subprocess.run(['docker', 'info', '--format', '{{.OSType}}'], cwd=root,
                           check=True, capture_output=True, text=True, timeout=20)
            return subprocess.run(command, cwd=root).returncode
        return 0
    except (OSError, ValueError, RuntimeError, subprocess.CalledProcessError, subprocess.TimeoutExpired) as error:
        print('BLOCKED: ' + str(error), file=sys.stderr)
        return 1
