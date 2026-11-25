#!/usr/bin/env python3
"""Generate the protobuf bindings (src/X/Common/Protobuf/*.pb.{cc,h}) for a game version.

The descriptors come straight from data/<version>/main.elf (tools/game_descriptors.py),
so no .nss is needed. They are rendered to .proto, compiled with the pinned
protoc 2.6.0, and adapted exactly as tools/bootstrap_game_data.py does for
2.6.2 (Hydra ABI, original diagnostic lines). The #line directive names the
version's build-server path. A header keeps its layout/<Name>.inc size checks
(measured on 2.6.2) only when its descriptor is unchanged from 2.6.2.

  python3 tools/bootstrap_protobuf.py --protoc /path/to/protoc-2.6.0
  python3 tools/bootstrap_protobuf.py --check     # compare with the installed files

The outputs are derived from the game and stay ignored by git, like 2.6.2's.
"""
from __future__ import annotations

import argparse
import subprocess
import sys
from pathlib import Path
from tempfile import TemporaryDirectory

import project_version
from bootstrap_game_data import BINDINGS, adapt
from game_descriptors import descriptors
from game_proto import schema

ROOT = Path(__file__).resolve().parent.parent
SOURCE_DIRS = {
    '2.6.2': 'D:/BuildServer/2/work/code/branches/2_6_2/X/Common/Protobuf/',
    '2.7.7': 'D:/BuildServer/1/work/code/branches/2_7_7/X/Common/Protobuf/',
}


def default_protoc() -> str:
    for candidate in (ROOT / 'build/game-tools/protoc-2.6.0',
                      ROOT.parent / 'NSDiablo3Decomp/build/game-tools/protoc-2.6.0'):
        if candidate.is_file():
            return str(candidate)
    return 'protoc'


def generate(version: str, protoc: str) -> dict[Path, bytes]:
    found = descriptors(ROOT, version)
    baseline = descriptors(ROOT, '2.6.2') if version != '2.6.2' else found
    raw = {name: data for name, (_, data) in found.items() if (BINDINGS / 'layout' / f'{name}.inc').is_file()}
    out_version = subprocess.check_output([protoc, '--version'], text=True).strip()
    if out_version != 'libprotoc 2.6.0':
        raise SystemExit(f'expected libprotoc 2.6.0, found {out_version} ({protoc})')
    outputs = {}
    with TemporaryDirectory() as tmp:
        stage = Path(tmp)
        for name, data in raw.items():
            (stage / f'{name}.proto').write_text(schema(data))
        subprocess.run([protoc, '-I.', '--cpp_out=.', *[f'{n}.proto' for n in raw]], cwd=stage, check=True)
        for name in raw:
            unchanged = baseline.get(name, (0, b''))[1] == raw[name]
            for ext in ('cc', 'h'):
                path = BINDINGS / f'{name}.pb.{ext}'
                text = adapt(path, (stage / path.name).read_text(), SOURCE_DIRS[version],
                             layout_checks=unchanged)
                outputs[path] = text.encode()
    return outputs


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__.split('\n\n')[0])
    ap.add_argument('--version', default=project_version.VERSION)
    ap.add_argument('--protoc', default=None, help='protoc 2.6.0 (default: build/game-tools/protoc-2.6.0)')
    ap.add_argument('--check', action='store_true', help='report differences, write nothing')
    args = ap.parse_args(argv)
    outputs = generate(args.version, args.protoc or default_protoc())
    changed = [p for p, data in outputs.items() if not p.is_file() or p.read_bytes() != data]
    if args.check:
        print(f'{len(changed)} of {len(outputs)} binding files differ from {args.version}')
        for p in changed:
            print(f'  {p.relative_to(ROOT)}')
        return 1 if changed else 0
    for p in changed:
        p.write_bytes(outputs[p])
    print(f'{args.version}: wrote {len(changed)} of {len(outputs)} binding files')
    return 0


if __name__ == '__main__':
    sys.exit(main())
