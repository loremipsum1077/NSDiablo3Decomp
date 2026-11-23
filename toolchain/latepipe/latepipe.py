#!/usr/bin/env python3
"""Build, pin and locate the LatePipe compiler plugin (docs/late_globalopt.md).

  python3 toolchain/latepipe/latepipe.py --path    validate the pinned plugin, print its path
  python3 toolchain/latepipe/latepipe.py --build   build it if missing (host; needs Docker)

The plugin lives at build/latepipe/<first 16 hex of its pinned SHA-256>/LatePipe.so,
so the compile flags name the exact binary: a different plugin means a different
command line, which rebuilds every object and invalidates every compile cache keyed
on flags. latepipe.lock.json pins the source hash, the build image, its g++ version,
the build flags and the resulting binary hash per Docker platform. --path (CMake's
configure step, inside the build container) and tools/build.py (every build) refuse
a plugin whose bytes differ from the pin; nothing rewrites the pin.

The plugin must be built in d3-lua-build:5.0.1: it is the d3-build image (same
/opt/llvm 5.0.1 headers and clang binary) plus g++, and the plugin has to share the
release clang's libstdc++ ABI (RegisterStandardPasses holds a std::function).
-z nodelete keeps the library mapped until exit: llvm_shutdown dlcloses plugins
before it destroys the global extension list that points into them.
"""
import argparse
import hashlib
import json
import os
from pathlib import Path
import subprocess
import sys
import tempfile

sys.dont_write_bytecode = True
HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]
LOCK = HERE / 'latepipe.lock.json'
SOURCE = HERE / 'LatePipe.cpp'


class PluginError(RuntimeError):
    pass


def sha(path):
    h = hashlib.sha256()
    with Path(path).open('rb') as stream:
        for chunk in iter(lambda: stream.read(1024 * 1024), b''):
            h.update(chunk)
    return h.hexdigest()


def platform():
    return os.environ.get('D3_DOCKER_PLATFORM', 'linux/arm64')


def lock(root=ROOT):
    data = json.loads((Path(root) / 'toolchain/latepipe/latepipe.lock.json').read_text())
    if sha(Path(root) / 'toolchain/latepipe/LatePipe.cpp') != data['source_sha256']:
        raise PluginError('toolchain/latepipe/LatePipe.cpp differs from latepipe.lock.json source_sha256; '
                          'a plugin change needs a new reviewed pin (docs/late_globalopt.md)')
    pin = data['plugin_sha256'].get(platform())
    if not pin:
        raise PluginError('no pinned plugin for ' + platform() + ' in latepipe.lock.json')
    return data, pin


def path(root=ROOT):
    _, pin = lock(root)
    return Path(root) / 'build/latepipe' / pin[:16] / 'LatePipe.so'


def validate(root=ROOT):
    _, pin = lock(root)
    plugin = path(root)
    if not plugin.is_file():
        raise PluginError('missing: ' + str(plugin) + '; run python3 toolchain/latepipe/latepipe.py --build '
                          '(tools/build.py does this)')
    if plugin.is_symlink() or plugin.parent.is_symlink():
        raise PluginError('symlink in plugin path: ' + str(plugin))
    actual = sha(plugin)
    if actual != pin:
        raise PluginError(f'{plugin} has SHA-256 {actual}, pinned {pin}; move it aside and rebuild')
    return plugin


def build(root=ROOT):
    """Build into a temporary directory, check the pin, then move into place."""
    root = Path(root)
    try:
        return validate(root)
    except PluginError as error:
        if not str(error).startswith('missing'):
            raise
    data, pin = lock(root)
    plugin = path(root)
    plugin.parent.parent.mkdir(parents=True, exist_ok=True)
    with tempfile.TemporaryDirectory(prefix='.build-', dir=plugin.parent.parent) as temp:
        out = '/work/' + (Path(temp) / 'LatePipe.so').relative_to(root).as_posix()
        script = ('test "$(g++ --version | head -n 1)" = "$1" || { echo "g++ version differs from the lock: '
                  '$(g++ --version | head -n 1)" >&2; exit 3; }; shift; exec g++ "$@"')
        cmd = ['docker', 'run', '--rm', '--pull', 'never', '--network', 'none', '-v', f'{root}:/work',
               '-w', '/work', data['image'], 'sh', '-c', script, 'sh', data['gxx_version'],
               *data['flags'], 'toolchain/latepipe/LatePipe.cpp', '-o', out]
        run = subprocess.run(cmd, capture_output=True, text=True, stdin=subprocess.DEVNULL)
        built = Path(temp) / 'LatePipe.so'
        if run.returncode or not built.is_file():
            raise PluginError('plugin build failed in ' + data['image'] + ':\n' + run.stdout + run.stderr)
        actual = sha(built)
        if actual != pin:
            raise PluginError(f'built plugin has SHA-256 {actual}, pinned {pin} for {platform()}: the image or '
                              'its g++ differs from the pinned recipe; not installed')
        plugin.parent.mkdir(exist_ok=True)
        os.replace(built, plugin)
    return validate(root)


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    mode = ap.add_mutually_exclusive_group(required=True)
    mode.add_argument('--path', action='store_true', help='validate the pinned plugin and print its path')
    mode.add_argument('--build', action='store_true', help='build the plugin if it is missing')
    args = ap.parse_args()
    try:
        print(build() if args.build else validate())
    except (PluginError, OSError, ValueError, KeyError) as error:
        sys.exit('LatePipe plugin: ' + str(error))


if __name__ == '__main__':
    main()
