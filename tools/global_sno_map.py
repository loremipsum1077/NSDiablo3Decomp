#!/usr/bin/env python3
"""Map GlobalSNO and GlobalGBID enum values between two game versions.

Each build registers its global SNOs in GlobalSNOListRefresh, one
sLookupSNO(platforms, bucket, index, group, "path", flags) call per entry, and
lists its global GBIDs in the sgtGlobalGBIDs table ({index, name, type} rows).
Both are read straight from each version's main.elf, so the mapping is exact:
an old entry and a new entry are the same when their (group, path) or (name,
type) agree.

  python3 tools/global_sno_map.py                 # report 2.6.2 -> 2.7.7
  python3 tools/global_sno_map.py --apply         # rewrite the values in GlobalSNO.h

--apply renumbers every GLOBALSNO_* / GLOBALGBID_* value (and NUM_GLOBAL_SNOS /
NUM_GLOBAL_GBIDS) to the new build, keeps the enum sorted by value, and adds a
GLOBALSNO_UNK_<n> / GLOBALGBID_UNK_<n> entry, commented with its path or name,
for each entry only the new build has. Old entries the new build no longer
registers keep their old position and are reported.
"""
from __future__ import annotations

import argparse
import csv
import re
import shutil
import struct
import subprocess
import sys
from pathlib import Path

import port_version
import version_diff as vd

ROOT = Path(__file__).resolve().parent.parent
HEADER = ROOT / 'src/X/Common/GlobalSNO.h'
GBID_ROW = struct.Struct('<i64si')           # {GlobalGBID, char[64], GameBalanceType}


def _objdump(elf: Path, start: int, size: int) -> str:
    exe = shutil.which('llvm-objdump') or '/opt/homebrew/opt/llvm/bin/llvm-objdump'
    return subprocess.run([exe, '-d', '--no-show-raw-insn', f'--start-address={start:#x}',
                           f'--stop-address={start + size:#x}', str(elf)],
                          capture_output=True, text=True, check=True).stdout


def _cstring(elf: vd.ELF, vaddr: int) -> str:
    off = elf.offset(vaddr)
    return elf.data[off:elf.data.index(b'\0', off)].decode('latin-1')


def _functions(version: str) -> dict[str, tuple[int, int]]:
    out = {}
    with (ROOT / 'data' / version / 'functions.csv').open(newline='') as fh:
        for r in csv.DictReader(fh):
            out[r['Name']] = (int(r['Address'], 16) - vd.CSV_BASE, int(r['Size']))
    return out


def lookup_calls(version: str) -> list[dict]:
    """Every sLookupSNO call in GlobalSNOListRefresh, in code order.

    index is None for calls whose index is a variable (the nExtrasIndex block).
    """
    path = ROOT / 'data' / version / 'main.elf'
    elf = vd.ELF(path)
    funcs = _functions(version)
    start, size = funcs['_Z20GlobalSNOListRefreshv']
    target = funcs['_ZL10sLookupSNOj15GlobalSNOBucketiiPKcj'][0]
    got = port_version.got_targets(elf)
    regs: dict[str, object] = {'zr': 0}
    calls = []
    insn = re.compile(r'\s*([0-9a-f]+):\s+(\S+)\s*(.*)')
    for line in _objdump(path, start, size).splitlines():
        m = insn.match(line)
        if not m:
            continue
        _, op, rest = m.groups()
        args = rest.partition('//')[0].strip().split(', ')
        if op in ('b', 'bl'):
            dest = int(args[0].split()[0], 16) if args[0].startswith('0x') else None
            if dest == target:
                mask, bucket, index, group, string, flags = (regs.get(str(i)) for i in range(6))
                if isinstance(string, tuple):            # GOT slot or pointer variable
                    string = string[1]
                calls.append({'mask': mask, 'bucket': bucket, 'index': index, 'group': group,
                              'path': _cstring(elf, string) if isinstance(string, int) else None,
                              'flags': flags})
            if op == 'bl':
                for i in range(19):
                    regs.pop(str(i), None)
            continue
        if not args or not re.fullmatch(r'[wx]\d+', args[0]):
            continue
        d = args[0][1:]
        if op == 'mov':
            src = args[1]
            regs[d] = int(src[1:], 0) if src.startswith('#') else regs.get(src[1:])
        elif op == 'orr' and args[1] in ('xzr', 'wzr') and args[2].startswith('#'):
            regs[d] = int(args[2][1:], 0)
        elif op == 'movk' and isinstance(regs.get(d), int):
            shift = int(args[2].split('#')[1]) if len(args) > 2 else 0
            imm = int(args[1][1:], 0)
            regs[d] = (regs[d] & ~(0xffff << shift)) | (imm << shift)
        elif op == 'adrp':
            regs[d] = int(args[1].split()[0], 16)
        elif op == 'add' and len(args) == 3 and args[2].startswith('#') \
                and isinstance(regs.get(args[1][1:]), int):
            regs[d] = regs[args[1][1:]] + int(args[2][1:], 0)
        elif op == 'ldr' and args[0].startswith('x') and len(args) == 3 and args[2].startswith('#'):
            base = regs.get(args[1].strip('[]')[1:])
            slot = base + int(args[2][1:].rstrip(']'), 0) if isinstance(base, int) else None
            regs[d] = ('got', got[slot]) if slot in got else None
        elif op == 'ldr' and args[0].startswith('x') and len(args) == 2:
            base = regs.get(args[1].strip('[]')[1:])     # const char *g = "..." through its GOT slot
            regs[d] = ('ptr', got[base[1]]) if isinstance(base, tuple) and base[1] in got else None
        elif op not in ('stp', 'str', 'strb', 'strh', 'cmp', 'cmn', 'tst', 'cbz', 'cbnz', 'tbz',
                        'tbnz', 'ccmp', 'b.eq', 'b.ne'):
            regs.pop(d, None)
    return calls


def gbid_rows(version: str) -> list[tuple[int, str, int]]:
    elf = vd.ELF(ROOT / 'data' / version / 'main.elf')
    with (ROOT / 'data' / version / 'data_symbols.csv').open(newline='') as fh:
        addr = next(int(r['Address'], 16) - vd.CSV_BASE for r in csv.DictReader(fh)
                    if r['Name'] == '_ZL14sgtGlobalGBIDs')
    rows, off = [], elf.offset(addr)
    while True:                       # rows until one is not {small index, name, small type}
        index, name, kind = GBID_ROW.unpack_from(elf.data, off)
        text = name.split(b'\0', 1)[0]
        if not (0 <= index < 4096 and 0 <= kind < 64 and text and text.isascii() and text.decode().isprintable()):
            break
        rows.append((index, text.decode(), kind))
        off += GBID_ROW.size
    return rows


def _key_map(old: dict[int, tuple], new: dict[int, tuple]) -> dict[int, int]:
    """{old value: new value} for entries with the same identifying key.

    Several values can share a key (two enum names registering one asset); when
    both sides have the same number of them they pair in value order, which
    insertions elsewhere in the enum preserve.
    """
    by_new, by_old = {}, {}
    for v, k in sorted(new.items()):
        by_new.setdefault(k, []).append(v)
    for v, k in sorted(old.items()):
        by_old.setdefault(k, []).append(v)
    out = {}
    for k, vs in by_new.items():
        olds = by_old.get(k, [])
        if len(olds) == len(vs):
            out.update(zip(olds, vs))
    return out


def sno_map(old_v: str, new_v: str):
    def table(v):
        # An index can be registered once per platform set: key it by all its paths.
        keys: dict[int, set] = {}
        for c in lookup_calls(v):
            if c['index'] is not None:
                keys.setdefault(c['index'], set()).add((c['group'], c['path']))
        return {i: tuple(sorted(k, key=str)) for i, k in keys.items()}
    old, new = table(old_v), table(new_v)
    return _key_map(old, new), old, new


def gbid_map(old_v: str, new_v: str):
    old = {i: (n, k) for i, n, k in gbid_rows(old_v)}
    new = {i: (n, k) for i, n, k in gbid_rows(new_v)}
    return _key_map(old, new), old, new


ENTRY = re.compile(r'^(\s*)([A-Z][A-Z0-9_]*)\s*=\s*(-?\d+),(.*)$')


def full_map(mapping: dict[int, int], old: dict[int, tuple], new: dict[int, tuple],
             old_count: int, new_count: int) -> dict[int, int]:
    """mapping plus the values neither build registers (unused enum names), which
    pair in order when both builds have the same number of them."""
    holes_old = [v for v in range(old_count) if v not in old]
    holes_new = [v for v in range(new_count) if v not in new]
    if len(holes_old) != len(holes_new):
        raise ValueError(f'{len(holes_old)} unregistered old values but {len(holes_new)} new ones')
    out = dict(mapping)
    out.update(zip(holes_old, holes_new))
    return out


def renumber(text: str, count_name: str, mapping: dict[int, int], new: dict[int, tuple],
             label) -> tuple[str, int, int]:
    """Rewrite the enum block that defines count_name; returns (text, renumbered, added).

    Every name with a value >= 0 must be in mapping (ValueError otherwise). Values
    only the new build uses get <PREFIX>_UNK_<value> names. Comment lines inside
    the block are dropped: the block is regenerated as a whole.
    """
    lines = text.split('\n')
    at = next(i for i, l in enumerate(lines) if re.match(rf'\s*{count_name}\s*=', l))
    first = max(i for i in range(at) if lines[i].strip().startswith('enum')) + 1
    last = next(i for i in range(at, len(lines)) if lines[i].strip() == '};')
    indent, entries = '    ', []
    prefix = {'NUM_GLOBAL_SNOS': 'GLOBALSNO', 'NUM_GLOBAL_GBIDS': 'GLOBALGBID'}[count_name]
    old_count = None
    for l in lines[first:last]:
        m = ENTRY.match(l)
        if not m:
            continue
        indent, name, value, tail = m.group(1), m.group(2), int(m.group(3)), m.group(4)
        if name == count_name:
            old_count = value
            continue
        if value < 0:
            entries.append((value, name, tail))
        elif value in mapping:
            entries.append((mapping[value], name, tail))
        else:
            raise ValueError(f'{name} = {value} has no new value')
    if old_count not in mapping:
        raise ValueError(f'{count_name} = {old_count} has no new value')
    new_count = mapping[old_count]
    used = {v for v, _, _ in entries}
    added = 0
    for v in sorted(new):
        if v not in used and v < new_count:
            entries.append((v, f'{prefix}_UNK_{v}', f'  // only in 2.7.7: {label(new[v])}'))
            added += 1
    entries.sort(key=lambda e: e[0])
    values = [v for v, _, _ in entries]
    if len(set(values)) != len(values) or [v for v in values if v >= 0] != list(range(new_count)):
        raise ValueError(f'{count_name}: renumbered values are not 0..{new_count - 1} exactly once')
    renumbered = sum(1 for o, n in mapping.items() if o != n and o < old_count)
    body = [f'{indent}{n} = {v},{t}' for v, n, t in entries] + [f'{indent}{count_name} = {new_count},']
    lines[first:last] = body
    return '\n'.join(lines), renumbered, added


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__.split('\n\n')[0])
    ap.add_argument('--old-version', default='2.6.2')
    ap.add_argument('--new-version', default='2.7.7')
    ap.add_argument('--apply', action='store_true', help='rewrite src/X/Common/GlobalSNO.h')
    args = ap.parse_args(argv)
    smap, sold, snew = sno_map(args.old_version, args.new_version)
    gmap, gold, gnew = gbid_map(args.old_version, args.new_version)
    print(f'GlobalSNO: {len(sold)} old, {len(snew)} new registrations; {len(smap)} mapped, '
          f'{sum(1 for o, n in smap.items() if o != n)} renumbered, '
          f'{len(snew) - len(set(smap.values()))} only in {args.new_version}, '
          f'{len(sold) - len(smap)} only in {args.old_version}')
    print(f'GlobalGBID: {len(gold)} old, {len(gnew)} new rows; {len(gmap)} mapped, '
          f'{sum(1 for o, n in gmap.items() if o != n)} renumbered, '
          f'{len(gnew) - len(set(gmap.values()))} only in {args.new_version}')
    if args.apply:
        text = HEADER.read_text()
        old_sno = int(re.search(r'NUM_GLOBAL_SNOS\s*=\s*(\d+)', text).group(1))
        old_gbid = int(re.search(r'NUM_GLOBAL_GBIDS\s*=\s*(\d+)', text).group(1))
        # The first nExtrasIndex call registers index NUM_GLOBAL_SNOS: its partner
        # in the new build is the new count.
        new_sno = smap[old_sno]
        text, rs, as_ = renumber(text, 'NUM_GLOBAL_SNOS',
                                 full_map(smap, sold, snew, old_sno, new_sno) | {old_sno: new_sno},
                                 snew, lambda k: '; '.join(f'group {g} "{p}"' for g, p in k))
        new_gbid = max(gnew) + 1
        text, rg, ag = renumber(text, 'NUM_GLOBAL_GBIDS',
                                full_map(gmap, gold, gnew, old_gbid, new_gbid) | {old_gbid: new_gbid},
                                gnew, lambda k: f'"{k[0]}" type {k[1]}')
        HEADER.write_text(text)
        print(f'rewrote {HEADER.relative_to(ROOT)}: GlobalSNO {rs} renumbered, {as_} added, '
              f'NUM_GLOBAL_SNOS {old_sno} -> {new_sno}; GlobalGBID {rg} renumbered, {ag} added, '
              f'NUM_GLOBAL_GBIDS {old_gbid} -> {new_gbid}')
    return 0


if __name__ == '__main__':
    sys.exit(main())
