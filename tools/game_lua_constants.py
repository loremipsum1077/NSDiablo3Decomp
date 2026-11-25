#!/usr/bin/env python3
"""Renumber the SWIG-exported enums (attributes, inventory slots, pet types, ...) for a version.

The game registers its Lua constants in the SWIG table `swig_constants`: 48-byte entries
{int kind; const char *name; long value; double; void *; swig_type_info **}. The name pointer
is an R_AARCH64_RELATIVE relocation, so the table is found from any constant's name string and
read straight from main.elf. Comparing the 2.6.2 and 2.7.7 tables by name gives the exact new
value of every exported enumerator, and where each new one goes (the table keeps header order).

  python3 tools/game_lua_constants.py            # report the differences
  python3 tools/game_lua_constants.py --apply    # rewrite the *Swig.h enum headers

The rewrite only touches `NAME = <old literal>,` lines of names in the table, inserts each new
name after its predecessor in the new table, and refuses to write if any table name ends up
defined zero or several times or with a value other than the table's.
"""
from __future__ import annotations

import argparse
import re
import struct
import subprocess
import sys
from pathlib import Path

import version_diff as vd

ROOT = Path(__file__).resolve().parent.parent
ENTRY = 48
ANCHOR = 'ATTRIBUTE_OBSERVER'
ENUMERATOR = re.compile(r'^(\s*)([A-Z][A-Z0-9_]*)(\s*=\s*)(-?\d+)(\s*,.*)$')


def relative_relocs(elf: vd.ELF) -> dict[int, int]:
    _, off, size = elf.sections['.rela.dyn']
    out = {}
    for o in range(off, off + size, 24):
        r_off, info, addend = struct.unpack_from('<QQq', elf.data, o)
        if info & 0xFFFFFFFF == 1027:
            out[r_off] = addend
    return out


def _cstr(elf: vd.ELF, vaddr: int) -> str | None:
    try:
        o = elf.offset(vaddr)
    except ValueError:
        return None
    end = elf.data.find(b'\0', o, o + 256)
    raw = elf.data[o:end] if end > o else b''
    return raw.decode() if raw and all(32 < c < 127 for c in raw) else None


def read_table(elf: vd.ELF, rel: dict[int, int], anchor: str = ANCHOR) -> list[tuple[str, int, int]]:
    """[(name, kind, integer value)] of swig_constants, in table order."""
    def entry(a):
        name = _cstr(elf, rel[a + 8]) if a + 8 in rel else None
        if name is None:
            return None
        kind, = struct.unpack_from('<i', elf.data, elf.offset(a))
        value, = struct.unpack_from('<q', elf.data, elf.offset(a + 16))
        return name, kind, value
    starts = [s - 8 for s, t in rel.items() if _cstr(elf, t) == anchor and (e := entry(s - 8)) and e[1] == 1]
    if len(starts) != 1:
        raise ValueError(f'{anchor}: expected one constant entry, found {len(starts)}')
    a = starts[0]
    while entry(a - ENTRY):
        a -= ENTRY
    out = []
    while (e := entry(a)):
        out.append(e)
        a += ENTRY
    return out


def constants(version: str) -> list[tuple[str, int, int]]:
    elf = vd.ELF(ROOT / 'data' / version / 'main.elf')
    return read_table(elf, relative_relocs(elf))


def plan(old: list, new: list) -> tuple[dict[str, tuple[int, int]], list[tuple[str, str, int]]]:
    """({name: (old, new)} for changed values, [(predecessor, name, value)] for new names)."""
    before = {n: v for n, _, v in old}
    after = {n: v for n, _, v in new}
    missing = [n for n in before if n not in after]
    if missing:
        raise ValueError(f'{len(missing)} constants were removed, e.g. {missing[:5]}; not handled')
    changed = {n: (before[n], after[n]) for n in before if before[n] != after[n]}
    added = []
    for i, (n, _, v) in enumerate(new):
        if n not in before:
            if i == 0:
                raise ValueError(f'{n}: new constant has no predecessor')
            added.append((new[i - 1][0], n, v))
    return changed, added


def rewrite(text: str, changed: dict, added: list, table: dict[str, int]) -> tuple[str, int]:
    """Rewrite one header; returns (text, edits)."""
    lines = text.split('\n')
    out, edits = [], 0
    after_name = {}
    for pred, name, value in added:
        after_name.setdefault(pred, []).append((name, value))
    for line in lines:
        m = ENUMERATOR.match(line)
        if m and m[2] in changed:
            old, new = changed[m[2]]
            if int(m[4]) != old:
                raise ValueError(f'{m[2]} = {m[4]}, expected the 2.6.2 value {old}')
            line = f'{m[1]}{m[2]}{m[3]}{new}{m[5]}'
            edits += 1
        out.append(line)
        if m and m[2] in after_name:
            pending = [m[2]]
            while pending:
                for name, value in after_name.pop(pending.pop(), []):
                    out.append(f'{m[1]}{name}{m[3]}{value},')
                    edits += 1
                    pending.append(name)
    return '\n'.join(out), edits


def headers(changed: dict, added: list) -> list[Path]:
    """Tracked source files defining a changed name or an added name's predecessor."""
    names = set(changed) | {p for p, _, _ in added}
    hits = subprocess.run(['git', 'grep', '-lE', r'^[[:space:]]*[A-Z][A-Z0-9_]*[[:space:]]*=[[:space:]]*-?[0-9]', '--', 'src'],
                          cwd=ROOT, capture_output=True, text=True, check=True).stdout.split()
    out = []
    for rel in hits:
        text = (ROOT / rel).read_text()
        if any(m and m[2] in names for m in map(ENUMERATOR.match, text.split('\n'))):
            out.append(ROOT / rel)
    return out


def verify(texts: dict[Path, str], table: dict[str, int]) -> None:
    seen: dict[str, list[int]] = {}
    for text in texts.values():
        for m in map(ENUMERATOR.match, text.split('\n')):
            if m and m[2] in table:
                seen.setdefault(m[2], []).append(int(m[4]))
    bad = [(n, seen.get(n)) for n, v in table.items() if n in seen and seen[n] != [v]]
    if bad:
        raise ValueError(f'{len(bad)} constants defined wrongly after rewrite, e.g. {bad[:5]}')


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__.split('\n\n')[0])
    ap.add_argument('--old', default='2.6.2')
    ap.add_argument('--new', default='2.7.7')
    ap.add_argument('--apply', action='store_true')
    args = ap.parse_args(argv)
    old, new = constants(args.old), constants(args.new)
    changed, added = plan(old, new)
    table = {n: v for n, _, v in new}
    print(f'{len(old)} -> {len(new)} constants: {len(changed)} changed, {len(added)} added')
    files = headers(changed, added)
    texts, total = {}, 0
    for path in files:
        texts[path], edits = rewrite(path.read_text(), changed, added, table)
        total += edits
        print(f'  {path.relative_to(ROOT)}: {edits} edits')
    if total != len(changed) + len(added):
        raise SystemExit(f'expected {len(changed) + len(added)} edits, made {total}')
    verify(texts, table)
    if args.apply:
        for path, text in texts.items():
            path.write_text(text)
        print('written')
    return 0


if __name__ == '__main__':
    sys.exit(main())
