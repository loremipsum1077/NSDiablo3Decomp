#!/usr/bin/env python3
"""Print what the DWARF records about one function: its prototype, parameter
and local names, and the tree of calls that were inlined into it (with the
line of each call site). Use it to write a function with the original names
and the original inline helpers.

  python3 tools/dwarf_fn.py _ZN4AABB7InflateEf
  python3 tools/dwarf_fn.py 0x97bf70

The compile unit dump is cached under build/dwarf/.
"""
import argparse
import csv
import difflib
import re
import shutil
import subprocess
import sys
import tempfile
import os
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
NSS = ROOT / "build/romfs/DiabloIIINX64r.nss"
CACHE = ROOT / "build/dwarf"
DIE = re.compile(r"^0x([0-9a-f]+): ( *)(DW_TAG_\w+|NULL)")
ATTR = re.compile(r"^\s+DW_AT_(\w+)\s+\((.*)\)\s*$")
SHOWN = ("formal_parameter", "variable", "inlined_subroutine", "lexical_block")


def dwarfdump(*args):
    exe = shutil.which("llvm-dwarfdump") or "/opt/homebrew/opt/llvm/bin/llvm-dwarfdump"
    return subprocess.run([exe, *args, str(NSS)], capture_output=True, text=True).stdout


def resolve(arg):
    if arg.lower().startswith("0x"):
        return int(arg, 16) & 0xFFFFFFFF
    for r in csv.DictReader((ROOT / "data/2.6.2/functions.csv").open()):
        if r["Name"] == arg:
            return int(r["Address"], 16) & 0xFFFFFFFF
    names = [r["Name"] for r in csv.DictReader((ROOT / "data/2.6.2/functions.csv").open())]
    near = [n for n in names if arg in n] or difflib.get_close_matches(arg, names, 8, 0.8)
    sys.exit(f"{arg} is not in functions.csv" + ("; did you mean:\n  " + "\n  ".join(near[:20]) if near else "")
             + "\n(a plain name like `UIIDCreate` lists every mangled name containing it)")


def load_unit(vaddr):
    m = re.search(r"^0x([0-9a-f]+): DW_TAG_compile_unit", dwarfdump(f"--lookup=0x{vaddr:x}"), re.M)
    if not m:
        sys.exit(f"no DWARF covers 0x{vaddr:x}")
    cached = CACHE / f"cu_{m.group(1)}.txt"
    if not cached.is_file():
        CACHE.mkdir(parents=True, exist_ok=True)
        text = dwarfdump(f"--debug-info=0x{m.group(1)}", "-c")
        # Parallel evidence collectors must never read a partially written unit.
        with tempfile.NamedTemporaryFile(mode='w', dir=CACHE, delete=False) as stream:
            temporary = Path(stream.name)
            stream.write(text)
        try:
            os.replace(temporary, cached)
        finally:
            temporary.unlink(missing_ok=True)
    dies, stack = {}, []
    cur = None
    for line in cached.read_text().splitlines():
        m = DIE.match(line)
        if m:
            if m.group(3) == "NULL":
                continue
            depth = len(m.group(2)) // 2
            cur = {"off": int(m.group(1), 16), "tag": m.group(3)[7:], "attrs": {}, "kids": []}
            dies[cur["off"]] = cur
            del stack[depth:]
            if stack:
                stack[-1]["kids"].append(cur)
            stack.append(cur)
            continue
        m = ATTR.match(line)
        if m and cur is not None:
            cur["attrs"].setdefault(m.group(1), m.group(2))
    return dies


def origin(dies, die):
    """Follow abstract_origin / specification to the DIE holding the names."""
    seen = set()
    while die["off"] not in seen:
        seen.add(die["off"])
        ref = die["attrs"].get("abstract_origin") or die["attrs"].get("specification")
        m = re.match(r"0x([0-9a-f]+)", ref or "")
        if not m or int(m.group(1), 16) not in dies:
            break
        yield die
        die = dies[int(m.group(1), 16)]
    yield die


def attr(dies, die, name):
    for d in origin(dies, die):
        if name in d["attrs"]:
            return d["attrs"][name]
    return ""


def type_name(dies, die):
    m = re.search(r'"(.*)"', attr(dies, die, "type"))
    return m.group(1) if m else "void"


def short_file(dies, die):
    f = attr(dies, die, "decl_file").strip('"').replace("\\", "/")
    return re.sub(r".*/2_6_2/", "", f)


def show(dies, die, indent):
    pad = "  " * indent
    tag = die["tag"]
    name = attr(dies, die, "name").strip('"')
    if tag == "formal_parameter":
        print(f"{pad}param  {type_name(dies, die)} {name}")
    elif tag == "variable":
        print(f"{pad}local  {type_name(dies, die)} {name}")
    elif tag == "inlined_subroutine":
        link = attr(dies, die, "linkage_name").strip('"')
        line = die["attrs"].get("call_line", "?")
        print(f"{pad}inlined at line {line}: {type_name(dies, die)} {name}  [{link}]  ({short_file(dies, die)}:"
              f"{attr(dies, die, 'decl_line')})")
    elif tag == "lexical_block":
        print(f"{pad}block")
    for kid in die["kids"]:
        if kid["tag"] in SHOWN:
            show(dies, kid, indent + 1)


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("function", help="mangled name from functions.csv, or 0x<vaddr>")
    args = ap.parse_args()
    vaddr = resolve(args.function)
    dies = load_unit(vaddr)
    for die in dies.values():
        if die["tag"] == "subprogram" and int(die["attrs"].get("low_pc", "0x0"), 16) == vaddr \
                and "high_pc" in die["attrs"]:
            print(f"{type_name(dies, die)} {attr(dies, die, 'name').strip(chr(34))}"
                  f"  [{attr(dies, die, 'linkage_name').strip(chr(34))}]")
            print(f"  defined at {short_file(dies, die)}:{attr(dies, die, 'decl_line')}")
            for kid in die["kids"]:
                if kid["tag"] in SHOWN:
                    show(dies, kid, 1)
            return
    sys.exit(f"no subprogram starts at 0x{vaddr:x}")


if __name__ == "__main__":
    main()
