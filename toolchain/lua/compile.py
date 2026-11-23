#!/usr/bin/env python3
"""CMake launcher: preserve normal compile commands; replace only Lua's backend."""
import hashlib
import json
import os
from pathlib import Path
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[2]
HERE = Path(__file__).resolve().parent
sys.dont_write_bytecode = True
sys.path.insert(0, str(ROOT / 'toolchain'))
import backend as provenance


def main():
    command = sys.argv[1:]
    lua_dir = ROOT / 'src/Contrib/Contrib/lua/src'
    sources = [Path(arg).resolve() for arg in command if arg.endswith('.c')]
    if not any(path.parent == lua_dir for path in sources):
        os.execvp(command[0], command)
    config = json.loads((HERE / 'config.json').read_text())
    base = provenance.resolve(ROOT)
    receipt = provenance.validate(ROOT, base)
    binary = base / 'out/bin/llc'
    digest = provenance.sha
    if '-c' not in command or command.count('-o') != 1:
        raise SystemExit('Unsupported Lua compile command: expected -c and one -o')
    output_index = command.index('-o') + 1
    output = Path(command[output_index])
    ir = Path(str(output) + '.ll')
    temporary = Path(str(output) + '.tmp')
    frontend = command.copy()
    frontend[output_index] = str(ir)
    # Keep dependency generation (-MD/-MF/-MT), includes, defines and debug paths.
    frontend.insert(1, '-emit-llvm')
    frontend.insert(2, '-S')
    section_options = [arg for arg in command if arg in ('-ffunction-sections', '-fno-function-sections')]
    section_flags = ['-function-sections'] if section_options and section_options[-1] == '-ffunction-sections' else []
    backend = [str(binary)] + config['flags'] + section_flags + ['-filetype=obj', str(ir), '-o', str(temporary)]
    try:
        subprocess.run(frontend, check=True)
        subprocess.run(backend, check=True)
        temporary.replace(output)
    except BaseException:
        output.unlink(missing_ok=True)
        temporary.unlink(missing_ok=True)
        raise
    Path(str(output) + '.commands.json').write_text(json.dumps({
        'frontend': frontend, 'backend': backend, 'binary_sha256': receipt['binary_sha256'],
        'ir_sha256': digest(ir), 'object_sha256': digest(output)}, indent=2) + '\n')


if __name__ == '__main__':
    main()
