#!/usr/bin/env python3
"""Score a candidate clang against an original build by recompiling known source.

A fixed sample of translation units whose functions are O (matching) in the old
build is recompiled with the candidate compiler inside the project's Docker
image, using the old build's exact compile commands. Each compiled function is
compared, with the same address masking as tools/version_diff.py, against its
counterpart in the new build (via version_diff's mapping.csv) and, as a
control, against the old build.

The right compiler reproduces unchanged functions exactly. Source that really
changed between versions caps the score below 100% for every compiler, so
compare candidates against each other, not against 100%.

  python3 tools/compiler_scan.py --label 5.0.1 --builtin
  python3 tools/compiler_scan.py --label 8.0.0 --compiler-dir ~/llvm/clang+llvm-8.0.0-aarch64-linux-gnu
  python3 tools/compiler_scan.py --report        # table of every scanned compiler

Diagnostic only: nothing here changes the build recipe or grants match credit.
"""
from __future__ import annotations

import argparse
import csv
import hashlib
import json
import re
import shlex
import struct
import subprocess
import sys
from collections import defaultdict
from difflib import SequenceMatcher
from pathlib import Path

import version_diff as vd

ROOT = Path(__file__).resolve().parent.parent
DEFAULT_OUT = ROOT / 'build' / 'analysis' / '2.7.7' / 'compiler_scan'
SKIP = re.compile(r'Protobuf|/lua/|Swig|_wrap\.')


# ------------------------------------------------------------------ objects

def object_functions(path: Path) -> dict[str, tuple[int, ...]]:
    """{symbol: instruction words} for sized FUNC symbols in an ELF64 .o."""
    d = path.read_bytes()
    shoff, = struct.unpack_from('<Q', d, 0x28)
    shentsize, shnum, shstrndx = struct.unpack_from('<HHH', d, 0x3a)
    sh = [struct.unpack_from('<IIQQQQIIQQ', d, shoff + i * shentsize) for i in range(shnum)]
    out = {}
    for sec in sh:
        if sec[1] != 2:  # SHT_SYMTAB
            continue
        _, _, _, _, off, size, link, _, _, entsize = sec
        stroff = sh[link][4]
        for i in range(size // entsize):
            name, info, _, shndx, value, sz = struct.unpack_from('<IBBHQQ', d, off + i * entsize)
            if info & 0xf != 2 or not sz or not 0 < shndx < shnum:
                continue
            sym = d[stroff + name:d.index(b'\0', stroff + name)].decode()
            body = sh[shndx][4] + value
            out[sym] = struct.unpack_from(f'<{sz // 4}I', d, body)
    return out


# ------------------------------------------------------------------ sample

def load_mapping(path: Path):
    by_file = defaultdict(list)
    with path.open(newline='') as fh:
        for r in csv.DictReader(fh):
            if r['quality_2.6.2'] == 'O' and r['file'] and r['bucket'] in ('identical', 'changed'):
                by_file[r['file']].append(r)
    return by_file


def pick_files(by_file, commands, count: int) -> list[str]:
    eligible = []
    for f, rows in by_file.items():
        buckets = {r['bucket'] for r in rows}
        if f in commands and not SKIP.search(f) and 5 <= len(rows) <= 60 and len(buckets) == 2:
            eligible.append(f)
    eligible.sort(key=lambda f: hashlib.sha256(f.encode()).hexdigest())
    return sorted(eligible[:count])


def container_command(entry: dict, out_obj: str, compiler: str, libcxx: str | None,
                      keep_g: bool = False) -> str:
    args = shlex.split(entry['command'])
    args[0] = compiler if args[0].endswith('++') else compiler.removesuffix('++')
    cleaned = []
    skip_next = False
    for a in args[1:]:
        if skip_next:
            skip_next = False
            continue
        if a.startswith('-fplugin='):
            continue
        if a == '-o':
            skip_next = True
            continue
        if a in ('-Wall', '-Wextra') or (a == '-g' and not keep_g):
            continue
        cleaned.append(a)
    if libcxx and entry['file'].endswith(('.cpp', '.cc')):
        cleaned = ['-nostdinc++', '-isystem', libcxx] + cleaned
    cmd = [args[0], *cleaned, '-w', '-o', out_obj]
    return f'cd {shlex.quote(entry["directory"])} && ' + shlex.join(cmd)


# ------------------------------------------------------------------ scoring

def masked(words, start=0):
    return vd.mask_function(tuple(words), start)[0]


def score(label: str, files: list[str], by_file, objdir: Path, old_elf: vd.ELF, new_elf: vd.ELF):
    per_file = {}
    totals = defaultdict(int)
    sims = []
    for f in files:
        obj = objdir / (f.replace('/', '__') + '.o')
        stats = defaultdict(int)
        if not obj.exists():
            stats['compile_failed'] = 1
            per_file[f] = dict(stats)
            totals['compile_failed'] += 1
            continue
        funcs = object_functions(obj)
        for r in by_file[f]:
            body = funcs.get(r['name'])
            if body is None:
                stats['not_emitted'] += 1
                continue
            mine = masked(body)
            old_addr = int(r['addr_2.6.2'], 16) - vd.CSV_BASE
            new_addr = int(r['addr_2.7.7'], 16) - vd.CSV_BASE
            old = masked(old_elf.words(old_addr, int(r['size_2.6.2'])), old_addr)
            new = masked(new_elf.words(new_addr, int(r['size_2.7.7'])), new_addr)
            stats['functions'] += 1
            stats['old_exact'] += mine == old
            stats['new_exact'] += mine == new
            stats[f'new_exact_{r["bucket"]}'] += mine == new
            stats[f'n_{r["bucket"]}'] += 1
            s = SequenceMatcher(None, mine, new, autojunk=False).ratio()
            sims.append(s)
        per_file[f] = dict(stats)
        for k, v in stats.items():
            totals[k] += v
    totals = dict(totals)
    totals['new_mean_similarity'] = round(sum(sims) / len(sims), 4) if sims else 0
    return {'label': label, 'files': len(files), 'totals': totals, 'per_file': per_file}


def report(out: Path):
    results = []
    for p in sorted(out.glob('*/result.json')):
        results.append(json.loads(p.read_text()))
    if not results:
        print('no results yet')
        return
    hdr = ('compiler', 'funcs', 'exact vs 2.6.2', 'exact vs 2.7.7', '  of identical', '  of changed',
           'mean sim 2.7.7', 'failed TUs')
    print(' | '.join(hdr))
    for r in sorted(results, key=lambda r: -r['totals'].get('new_exact', 0)):
        t = r['totals']
        n = t.get('functions', 0) or 1
        pct = lambda k, d=n: f"{t.get(k, 0)} ({100 * t.get(k, 0) / (d or 1):.0f}%)"
        print(' | '.join([r['label'], str(t.get('functions', 0)), pct('old_exact'), pct('new_exact'),
                          pct('new_exact_identical', t.get('n_identical', 0)),
                          pct('new_exact_changed', t.get('n_changed', 0)),
                          str(t.get('new_mean_similarity')), str(t.get('compile_failed', 0))]))


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__.split('\n\n')[0])
    ap.add_argument('--label', help='name for this compiler in the results')
    g = ap.add_mutually_exclusive_group()
    g.add_argument('--builtin', action='store_true', help="use the image's own /opt/llvm")
    g.add_argument('--compiler-dir', type=Path, help='extracted clang+llvm release directory on the host')
    ap.add_argument('--image', default='d3-build:5.0.1')
    ap.add_argument('--source-root', type=Path, default=ROOT,
                    help='checkout whose src/ and include/ are compiled (needs generated sources)')
    ap.add_argument('--compile-commands', type=Path,
                    help='default: <source-root>/build/2.6.2/compile_commands.json')
    ap.add_argument('--mapping', type=Path, default=ROOT / 'build' / 'analysis' / '2.7.7' / 'version_diff' / 'mapping.csv')
    ap.add_argument('--old-elf', type=Path, default=ROOT / 'data' / '2.6.2' / 'main.elf')
    ap.add_argument('--new-elf', type=Path, default=ROOT / 'data' / '2.7.7' / 'main.elf')
    ap.add_argument('--files', type=int, default=40, help='number of translation units to sample')
    ap.add_argument('--libcxx', choices=('compiler', '5.0.1'), default='5.0.1',
                    help="libc++ headers: the candidate's own, or clang 5.0.1's from the image")
    ap.add_argument('--keep-g', action='store_true', help='keep -g (it changes clang 5 codegen)')
    ap.add_argument('--jobs', type=int, default=8)
    ap.add_argument('--out', type=Path, default=DEFAULT_OUT)
    ap.add_argument('--report', action='store_true', help='print the comparison table and exit')
    args = ap.parse_args(argv)

    if args.report:
        report(args.out)
        return 0
    if not args.label or not (args.builtin or args.compiler_dir):
        ap.error('--label and one of --builtin/--compiler-dir are required')
    src_root = args.source_root.resolve()
    cc_path = args.compile_commands or src_root / 'build' / '2.6.2' / 'compile_commands.json'
    commands = {}
    for e in json.loads(cc_path.read_text()):
        commands[e['file'].replace('/work/src/', '')] = e
    by_file = load_mapping(args.mapping)
    files = pick_files(by_file, commands, args.files)

    run_dir = args.out / args.label
    objdir = run_dir / 'obj'
    objdir.mkdir(parents=True, exist_ok=True)
    for stale in objdir.glob('*.o'):
        stale.unlink()
    compiler = '/opt/llvm/bin/clang++' if args.builtin else '/opt/cand/bin/clang++'
    libcxx = '/opt/llvm/include/c++/v1' if args.libcxx == '5.0.1' else None
    lines = ['set -u', 'fail=0']
    for f in files:
        cmd = container_command(commands[f], f'/out/obj/{f.replace("/", "__")}.o', compiler, libcxx, args.keep_g)
        lines.append(f'( {cmd} ) > /out/log_{f.replace("/", "__")}.txt 2>&1 &')
        lines.append(f'while [ "$(jobs -rp | wc -l)" -ge {args.jobs} ]; do sleep 0.2; done')
    lines.append('wait')
    (run_dir / 'compile.sh').write_text('\n'.join(lines) + '\n')

    docker = ['docker', 'run', '--rm', '--network', 'none',
              '-v', f'{src_root}:/work:ro', '-v', f'{run_dir.resolve()}:/out']
    if args.compiler_dir:
        docker += ['-v', f'{args.compiler_dir.resolve()}:/opt/cand:ro']
    docker += [args.image, 'bash', '/out/compile.sh']
    print(f'compiling {len(files)} translation units with {args.label} ...', flush=True)
    subprocess.run(docker, check=True)

    result = score(args.label, files, by_file, objdir, vd.ELF(args.old_elf), vd.ELF(args.new_elf))
    (run_dir / 'result.json').write_text(json.dumps(result, indent=1))
    report(args.out)
    return 0


if __name__ == '__main__':
    sys.exit(main())
