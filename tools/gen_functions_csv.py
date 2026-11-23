#!/usr/bin/env python3
"""Generate data/functions.csv (the list tools/check reads) from the unstripped
.nss ELF that ships in the RomFS. Its vaddrs equal main.elf's, and every
function symbol carries a mangled name and size.

Also writes data_symbols.csv (Address,Name) beside it.

Row format: Address,Quality,Size,Name   (Address = 0x7100000000 | vaddr)
Quality is U (not decompiled) for new rows; tools/check upgrades rows, and a
re-run keeps the quality already recorded for each address.
"""
import argparse
import shutil
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent


def find_nm():
    for cand in ("llvm-nm", "/opt/homebrew/opt/llvm/bin/llvm-nm", "nm"):
        if shutil.which(cand):
            return shutil.which(cand)
    sys.exit("llvm-nm not found (brew install llvm / apt install llvm)")


def shortest_suffix(target, others):
    """The shortest '/'-suffix of `target` that is not a suffix of any of `others`."""
    parts = target.split("/")
    for n in range(1, len(parts) + 1):
        cand = "/".join(parts[-n:])
        if not any(o == cand or o.endswith("/" + cand) for o in others):
            return cand
    return target


def main():
    p = argparse.ArgumentParser()
    p.add_argument("nss", type=Path, nargs="?", default=ROOT / "build/romfs/DiabloIIINX64r.nss")
    p.add_argument("-o", "--out", type=Path, default=ROOT / "data/2.6.2/functions.csv")
    args = p.parse_args()
    if not args.nss.is_file():
        sys.exit(f"{args.nss} not found")

    out = subprocess.check_output([find_nm(), "-S", "-n", "--defined-only", str(args.nss)], text=True)
    rows, seen = [], set()
    data_rows, data_names = [], set()
    for line in out.splitlines():
        parts = line.split(None, 3)
        if len(parts) != 4:
            continue
        if parts[2] in ("D", "d", "B", "b", "R", "r", "V", "v"):
            # global data symbols (for tools/check's ADRP/global matching);
            # tools/check wants unique names, so keep the first of each
            if int(parts[1], 16) and parts[3] not in data_names:
                data_names.add(parts[3])
                data_rows.append((int(parts[0], 16), parts[3]))
            continue
        if parts[2] not in ("T", "t", "W", "w"):
            continue
        addr, size, name = int(parts[0], 16), int(parts[1], 16), parts[3]
        if size == 0 or addr in seen:
            continue
        seen.add(addr)
        rows.append((addr, size, name))

    # tools/check needs unique names. A mangled name that several TUs define
    # (a file-static, an anonymous-namespace function) is written as
    # <name>@<file>, where <file> is the shortest path suffix of the owning
    # source file that is unique among the copies; tools/check resolves it
    # through the link map (see tools/patches/0002). Copies with no DWARF
    # file cannot be qualified and become FUN_<vaddr>.
    # The owning file comes from function_files.csv, so on a first run this
    # script has to run again after tools/gen_function_files.py.
    files = {}
    ff = args.out.parent / "function_files.csv"
    if ff.is_file():
        with ff.open() as f:
            for line in f.readlines()[1:]:
                a, fn = line.split(",", 2)[:2]
                files[a] = fn
    by_name = {}
    for addr, size, name in rows:
        by_name.setdefault(name, []).append(addr)
    names = {}
    dups = []
    for name, addrs in by_name.items():
        if len(addrs) == 1:
            continue
        paths = {a: files.get(f"0x{0x7100000000 | a:016x}", "") for a in addrs}
        for a in addrs:
            q = shortest_suffix(paths[a], [paths[b] for b in addrs if b != a]) if paths[a] else ""
            names[a] = f"{name}@{q}" if q else f"FUN_{a:08x}"
            dups.append((a, names[a]))
    quality = {}
    if args.out.is_file():
        with args.out.open() as f:
            for line in f.readlines()[1:]:
                a, q = line.split(",", 2)[:2]
                quality[a] = q
    args.out.parent.mkdir(parents=True, exist_ok=True)
    with args.out.open("w") as f:
        f.write("Address,Quality,Size,Name\n")
        for addr, size, name in rows:
            key = f"0x{0x7100000000 | addr:016x}"
            f.write(f"{key},{quality.get(key, 'U')},{size:06d},{names.get(addr, name)}\n")
    with (args.out.parent / "duplicate_names.csv").open("w") as f:
        f.write("Address,Name\n")
        for addr, name in dups:
            f.write(f"0x{0x7100000000 | addr:016x},{name}\n")
    with (args.out.parent / "data_symbols.csv").open("w") as f:
        f.write("Address,Name\n")
        for addr, name in data_rows:
            f.write(f"0x{0x7100000000 | addr:016x},{name}\n")
    print(f">>> wrote {len(data_rows)} data symbols to data_symbols.csv")
    print(f">>> wrote {len(rows)} functions to {args.out} ({len(dups)} duplicate names qualified)")


if __name__ == "__main__":
    main()
