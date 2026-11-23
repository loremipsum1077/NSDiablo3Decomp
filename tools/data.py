#!/usr/bin/env python3
"""Print the original bytes of a global variable or an address in main.elf.

  tools/data.py crgbaWhite                 a symbol from data_symbols.csv
  tools/data.py _ZL10cgUIIDRoot            mangled names work too
  tools/data.py 0x7100e5de88 -n 64         an address and a byte count
  tools/data.py 0x71010ccf30 -n 32 --ptr   as 8-byte pointers

Each row shows hex, the 32-bit ints, the floats and any text. With --ptr each
8-byte word is shown as a pointer: the value a relocation puts there (the file
itself holds zero for those), the symbol it lands on, and the string there if
it points at text. Use this for the initialisers of constants, tables behind a
switch, vtables and string tables; the DWARF records none of those values.

Without -n the size runs to the next known symbol (at most 256 bytes).
"""
import argparse
import bisect
import csv
import struct
import sys
from pathlib import Path

from validation import elf as elf_reader

ROOT = Path(__file__).resolve().parent.parent
DATA = ROOT / "data/2.6.2"
BASE = 0x7100000000


def symbols():
    out = []
    for name in ("data_symbols.csv", "functions.csv"):
        for r in csv.DictReader((DATA / name).open()):
            out.append((int(r["Address"], 16) & 0xFFFFFFFF, r["Name"]))
    out.sort()
    return out


def relative_relocs(elf):
    """{offset: addend} for R_AARCH64_RELATIVE entries."""
    out = {}
    for s in elf.sections:
        if s[1] == 4:  # SHT_RELA
            for o in range(s[4], s[4] + s[5], 24):
                r_off, r_info, addend = struct.unpack_from("<QQq", elf.d, o)
                if r_info & 0xFFFFFFFF == 1027:
                    out[r_off & 0xFFFFFFFF] = addend & 0xFFFFFFFF
    return out


def text_at(elf, vaddr, limit=60):
    raw = elf.read_vaddr(vaddr, limit) or b""
    end = raw.find(b"\0")
    s = raw[:end] if end >= 0 else raw
    if s and all(32 <= c < 127 or c in (9, 10) for c in s):
        return s.decode()
    return None


def rows(raw, start=0):
    """The hex / int / float / text rows of a byte range, 16 bytes per row."""
    out = []
    for o in range(0, len(raw), 16):
        row = raw[o:o + 16]
        words = [struct.unpack_from("<i", row, i)[0] for i in range(0, len(row) - 3, 4)]
        floats = [struct.unpack_from("<f", row, i)[0] for i in range(0, len(row) - 3, 4)]
        txt = "".join(chr(c) if 32 <= c < 127 else "." for c in row)
        out.append(f"  +0x{start + o:03x}  {row.hex(' ', 4):35}  |{txt:16}|")
        out.append(f"          int   {' '.join(f'{w:>11}' for w in words)}")
        out.append(f"          float {' '.join(f'{f:>11.6g}' for f in floats)}")
    return out


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("what", help="symbol name or address")
    ap.add_argument("-n", type=int, help="bytes to print")
    ap.add_argument("--ptr", action="store_true", help="show 8-byte words as pointers")
    args = ap.parse_args()

    syms = symbols()
    addrs = [a for a, _ in syms]
    if args.what.lower().startswith("0x"):
        vaddr = int(args.what, 16) & 0xFFFFFFFF
    else:
        hits = [a for a, n in syms if n == args.what or n.split("@")[0] == args.what]
        if not hits:
            hits = [a for a, n in syms if args.what in n]
            if len(hits) != 1:
                for a in hits[:20]:
                    print(f"  0x{BASE + a:x}  {syms[addrs.index(a)][1]}")
                sys.exit(f"{args.what}: {len(hits)} symbols contain that name" if hits else f"{args.what}: no such symbol")
        vaddr = hits[0]

    def name_of(a):
        i = bisect.bisect_right(addrs, a) - 1
        if i < 0:
            return "?"
        off = a - addrs[i]
        return syms[i][1] + (f"+0x{off:x}" if off else "")

    size = args.n
    if size is None:
        i = bisect.bisect_right(addrs, vaddr)
        size = min(addrs[i] - vaddr, 256) if i < len(addrs) else 64
    elf = elf_reader.Elf(DATA / "main.elf")
    raw = elf.read_vaddr(vaddr, size)
    if not raw:
        sys.exit(f"0x{BASE + vaddr:x}: not in a loaded segment with file contents (.bss?)")
    print(f"0x{BASE + vaddr:x}  {name_of(vaddr)}  ({size} bytes)")

    if args.ptr:
        relocs = relative_relocs(elf)
        for o in range(0, size - 7, 8):
            v, = struct.unpack_from("<Q", raw, o)
            if vaddr + o in relocs:
                t = relocs[vaddr + o]
                s = text_at(elf, t)
                print(f"  +0x{o:03x}  -> 0x{BASE + t:x}  " + (f'"{s}"' if s else name_of(t)))
            else:
                print(f"  +0x{o:03x}  0x{v:016x}")
        return

    for line in rows(raw):
        print(line)


if __name__ == "__main__":
    main()
