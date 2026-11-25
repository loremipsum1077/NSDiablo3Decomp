#!/usr/bin/env python3
"""Generate a new game version's data/<version>/ files from an older version.

Reads tools/version_diff.py's mapping.csv and both ELFs, and writes:

  functions.csv        every function of the new build. Names come from the old
                       build. Functions that had source (O/m/M/W) become M, so
                       the checker re-verifies them and promotes real matches
                       to O. L stays L. New functions are U, named
                       sub_<address>.
  function_files.csv   source file per function, with its compiler: the new one,
                       or 5.0.1 for the prebuilt libraries (project_version.LEGACY_DIRS).
  data_symbols.csv     old data symbols located in the new build. Each one is
                       found by lining up address computations (ADRP +
                       ADD/LDR/STR, following GOT slots) inside matched
                       functions: identical ones, and runs of equal masked
                       code in changed ones at >= 0.8 shape similarity. A
                       symbol is kept when its votes agree (>= 90%).
  duplicate_names.csv  the old file's entries whose function was mapped.

Nothing is marked O here. Run the checker on a full build for that.

  python3 tools/port_version.py                         # 2.6.2 -> 2.7.7, clang 7.1.0
"""
from __future__ import annotations

import argparse
import bisect
import csv
import sys
from collections import Counter, defaultdict
from pathlib import Path

import project_version
import version_diff as vd

ROOT = Path(__file__).resolve().parent.parent
CARRY = {'O': 'M', 'm': 'M', 'M': 'M', 'W': 'W', 'L': 'L', 'U': 'U'}


def _sext(v, bits):
    return v - (1 << bits) if v & (1 << (bits - 1)) else v


def got_targets(elf: vd.ELF) -> dict[int, int]:
    """{GOT/data slot address: address it points to} from .rela.dyn."""
    _, off, size = elf.sections['.rela.dyn']
    _, symoff, symsize = elf.sections['.dynsym']
    d = elf.data
    out = {}
    for i in range(size // 24):
        r_offset, r_info, r_addend = vd.struct.unpack_from('<QQq', d, off + 24 * i)
        kind, sym = r_info & 0xffffffff, r_info >> 32
        if kind == 1027:                                    # R_AARCH64_RELATIVE
            out[r_offset] = r_addend
        elif kind in (1025, 257) and sym:                   # GLOB_DAT / ABS64
            value, = vd.struct.unpack_from('<Q', d, symoff + 24 * sym + 8)
            if value:
                out[r_offset] = value + r_addend
    return out


def address_refs(words, start, got=None):
    """[(instruction index, absolute target)] for ADRP-based and PC-relative refs.

    A 64-bit load through a GOT slot reports the slot's target instead.
    """
    page = {}
    out = []
    for i, w in enumerate(words):
        pc = start + 4 * i
        if (w & 0x9F000000) == 0x90000000:                       # ADRP
            imm = ((w >> 29) & 3) | (((w >> 5) & 0x7FFFF) << 2)
            page[w & 31] = (pc & ~0xFFF) + (_sext(imm, 21) << 12)
            continue
        if (w & 0x9F000000) == 0x10000000:                       # ADR
            imm = ((w >> 29) & 3) | (((w >> 5) & 0x7FFFF) << 2)
            out.append((i, pc + _sext(imm, 21)))
            page.pop(w & 31, None)
            continue
        if (w & 0x7F800000) == 0x11000000:                       # ADD imm
            rn, rd = (w >> 5) & 31, w & 31
            if rn in page:
                shift = 12 if (w >> 22) & 1 else 0
                out.append((i, page[rn] + (((w >> 10) & 0xFFF) << shift)))
            page.pop(rd, None)
            continue
        if (w & 0x3B000000) == 0x39000000:                       # LDR/STR unsigned imm
            rn, rt = (w >> 5) & 31, w & 31
            size = (w >> 30) & 3
            if (w >> 26) & 1 and (w >> 23) & 1:                 # 128-bit SIMD
                size = 4
            if rn in page:
                target = page[rn] + (((w >> 10) & 0xFFF) << size)
                if got and size == 3 and (w >> 22) & 1 and not (w >> 26) & 1 and target in got:
                    target = got[target]
                out.append((i, target))
            if (w >> 22) & 1:
                page.pop(rt, None)
            continue
        if (w & 0x3B000000) == 0x18000000:                       # LDR literal
            out.append((i, pc + 4 * _sext((w >> 5) & 0x7FFFF, 19)))
    return out


def _aligned(a, b):
    """{old instruction index: new instruction index} over equal masked runs."""
    if a.digest == b.digest:
        return {i: i for i in range(len(a.masked))}
    sm = vd.SequenceMatcher(None, a.masked, b.masked, autojunk=False)
    out = {}
    for blk in sm.get_matching_blocks():
        for k in range(blk.size):
            out[blk.a + k] = blk.b + k
    return out


def map_data_symbols(pairs, old_elf, new_elf, old_symbols):
    """pairs: [(old Func, new Func)] of matched functions."""
    votes = defaultdict(Counter)   # old symbol address -> Counter(new address)
    starts = sorted(old_symbols)
    got_a, got_b = got_targets(old_elf), got_targets(new_elf)
    for a, b in pairs:
        ra = address_refs(old_elf.words(a.addr, a.size), a.addr, got_a)
        if not ra:
            continue
        rb = dict(address_refs(new_elf.words(b.addr, b.size), b.addr, got_b))
        align = _aligned(a, b)
        for i, ta in ra:
            j = align.get(i)
            tb = rb.get(j) if j is not None else None
            if tb is None:
                continue
            k = bisect.bisect_right(starts, ta) - 1
            if k < 0:
                continue
            sym = starts[k]
            nxt = starts[k + 1] if k + 1 < len(starts) else None
            if nxt is not None and ta >= nxt:
                continue
            votes[sym][tb - (ta - sym)] += 1
    out = {}
    conflicts = 0
    for sym, c in votes.items():
        (best, n), = c.most_common(1)
        # Unanimous, or an overwhelming majority (misaligned runs in changed
        # functions can cast the odd stray vote).
        if len(c) == 1 or n >= 0.9 * sum(c.values()) and n >= 3:
            out[sym] = best
        else:
            conflicts += 1
    # Two old symbols landing on one new address is ambiguous: drop both.
    shared = Counter(out.values())
    for sym in [s for s, a in out.items() if shared[a] > 1]:
        del out[sym]
        conflicts += 1
    return out, conflicts, len(votes)


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__.split('\n\n')[0])
    ap.add_argument('--old-version', default='2.6.2')
    ap.add_argument('--new-version', default='2.7.7')
    ap.add_argument('--compiler', default='7.1.0', help='label written to function_files.csv')
    ap.add_argument('--mapping', type=Path)
    ap.add_argument('--force', action='store_true', help='overwrite existing CSVs')
    args = ap.parse_args(argv)
    ov, nv = args.old_version, args.new_version
    old_dir, new_dir = ROOT / 'data' / ov, ROOT / 'data' / nv
    mapping = args.mapping or ROOT / 'build' / 'analysis' / nv / 'version_diff' / 'mapping.csv'
    targets = [new_dir / n for n in ('functions.csv', 'function_files.csv', 'data_symbols.csv',
                                      'duplicate_names.csv')]
    existing = [t for t in targets if t.exists()]
    if existing and not args.force:
        print('refusing to overwrite (pass --force): ' + ', '.join(map(str, existing)), file=sys.stderr)
        return 2

    old_elf = vd.ELF(old_dir / 'main.elf')
    new_elf = vd.ELF(new_dir / 'main.elf')
    new_bounds = vd.eh_functions(new_elf)

    rows = list(csv.DictReader(mapping.open(newline='')))
    new_addr = f'addr_{nv}'
    old_addr = f'addr_{ov}'
    by_new = {int(r[new_addr], 16): r for r in rows if r[new_addr]}
    old_to_new = {int(r[old_addr], 16): int(r[new_addr], 16) for r in rows if r[old_addr] and r[new_addr]}

    func_rows, file_rows = [], []
    status = Counter()
    for addr in sorted(new_bounds):
        full = vd.CSV_BASE + addr
        size = new_bounds[addr] & ~3
        if not size or not new_elf.is_exec(addr):
            continue
        r = by_new.get(full)
        if r and r['name']:
            q = CARRY.get(r[f'quality_{ov}'], 'U')
            name, src = r['name'], r['file']
        else:
            q, name, src = 'U', f'sub_{full:x}', ''
        status[q] += 1
        func_rows.append([f'{full:#018x}', q, f'{size:06d}', name])
        label = ''
        if src:
            # Prebuilt libraries keep the old compiler in a mixed build.
            label = '5.0.1' if args.compiler != '5.0.1' and any(
                ('src/' + src).startswith(d) for d in project_version.LEGACY_DIRS) else args.compiler
        file_rows.append([f'{full:#018x}', src, label])

    new_dir.mkdir(parents=True, exist_ok=True)
    with (new_dir / 'functions.csv').open('w', newline='') as fh:
        w = csv.writer(fh, lineterminator='\n')
        w.writerow(['Address', 'Quality', 'Size', 'Name'])
        w.writerows(func_rows)
    with (new_dir / 'function_files.csv').open('w', newline='') as fh:
        w = csv.writer(fh, lineterminator='\n')
        w.writerow(['Address', 'File', 'Compiler'])
        w.writerows(file_rows)

    # Data symbols, from identical functions only.
    old_syms = {}
    with (old_dir / 'data_symbols.csv').open(newline='') as fh:
        for r in csv.DictReader(fh):
            old_syms[int(r['Address'], 16) - vd.CSV_BASE] = r['Name']
    old_funcs = {f.addr: f for f in vd.load_functions(old_elf)}
    new_funcs = {f.addr: f for f in vd.load_functions(new_elf)}
    pairs = []
    for r in rows:
        if r['bucket'] == 'identical' or (r['bucket'] == 'changed' and float(r['shape_similarity']) >= 0.8):
            a = old_funcs.get(int(r[old_addr], 16) - vd.CSV_BASE)
            b = new_funcs.get(int(r[new_addr], 16) - vd.CSV_BASE)
            if a and b:
                pairs.append((a, b))
    mapped, conflicts, seen = map_data_symbols(pairs, old_elf, new_elf, old_syms)
    with (new_dir / 'data_symbols.csv').open('w', newline='') as fh:
        w = csv.writer(fh, lineterminator='\n')
        w.writerow(['Address', 'Name'])
        for new, name in sorted((mapped[a], old_syms[a]) for a in mapped):
            w.writerow([f'{vd.CSV_BASE + new:#018x}', name])

    dup_written = 0
    dup_src = old_dir / 'duplicate_names.csv'
    if dup_src.exists():
        with dup_src.open(newline='') as fh, (new_dir / 'duplicate_names.csv').open('w', newline='') as out:
            w = csv.writer(out, lineterminator='\n')
            w.writerow(['Address', 'Name'])
            for r in csv.DictReader(fh):
                new = old_to_new.get(int(r['Address'], 16))
                if new:
                    w.writerow([f'{new:#018x}', r['Name']])
                    dup_written += 1

    print(f'functions.csv: {len(func_rows)} functions ' +
          ', '.join(f'{k}={v}' for k, v in sorted(status.items())))
    print(f'data_symbols.csv: {len(mapped)} of {len(old_syms)} old data symbols located '
          f'({seen} referenced by matched functions, {conflicts} with conflicting votes dropped)')
    print(f'duplicate_names.csv: {dup_written} entries')
    return 0


if __name__ == '__main__':
    sys.exit(main())
