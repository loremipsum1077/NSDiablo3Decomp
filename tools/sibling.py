#!/usr/bin/env python3
"""Look a file or function up in the sibling decomp of the 1.0 build.

  tools/sibling.py X/Common/WorldPlace.cpp      every function of the file
  tools/sibling.py _ZN7SNOName3SetEiPKc         one function

The sibling repo (default ../Diablo3Decomp, or $D3_SIBLING) decompiles the 1.0
launch build of the same game. For each function this prints whether the
sibling has it matching there (O), and where in its src/ the name appears.

Use it as a reference for bodies, not as source to paste: it matches a
different binary, names things by its own conventions, and some of its files
force bytes in ways this repo does not allow. Port the logic into this repo's
headers and DWARF names, then prove it here with tools/try_file.py.
"""
import argparse
import csv
import os
import re
import shutil
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
DATA = ROOT / "data/2.6.2"


def sibling_root():
    env = os.environ.get("D3_SIBLING")
    cands = [Path(env)] if env else []
    try:
        common = subprocess.run(["git", "rev-parse", "--path-format=absolute", "--git-common-dir"], cwd=ROOT,
                                capture_output=True, text=True).stdout.strip()
        cands.append(Path(common).parent.parent / "Diablo3Decomp")
    except OSError:
        pass
    cands.append(ROOT.parent / "Diablo3Decomp")
    for c in cands:
        if (c / "data/1.0/functions.csv").is_file():
            return c
    sys.exit("sibling repo not found (set D3_SIBLING to a checkout of Diablo3Decomp); this tool is optional")


def demangle(names):
    exe = shutil.which("llvm-cxxfilt") or shutil.which("c++filt")
    if not exe:
        return {n: n for n in names}
    out = subprocess.run([exe], input="\n".join(names), capture_output=True, text=True).stdout.splitlines()
    return dict(zip(names, out))


def short(demangled):
    """`Class::method` (or the bare function name) out of a demangled signature."""
    head = re.sub(r"\(.*$", "", demangled)
    head = re.sub(r"<[^<>]*>", "", head)
    parts = head.split()[-1].split("::") if head.split() else [head]
    return "::".join(parts[-2:]), parts[-1]


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("what", help="a source file from function_files.csv, or a mangled function name")
    args = ap.parse_args()
    sib = sibling_root()

    files = {r["Address"]: r["File"] for r in csv.DictReader((DATA / "function_files.csv").open())}
    ours = list(csv.DictReader((DATA / "functions.csv").open()))
    if args.what.endswith((".cpp", ".cc", ".c")):
        rows = [r for r in ours if files.get(r["Address"], "").endswith(args.what)]
        if not rows:
            sys.exit(f"{args.what}: not a source file in function_files.csv")
        base = Path(args.what).name
        same = subprocess.run(["git", "ls-files", f"*/{base}", f"*/{Path(base).stem}.h", f"*/{Path(base).stem}.inl"],
                              cwd=sib, capture_output=True, text=True).stdout.split()
        print("sibling files with this name:")
        for f in same or ["(none)"]:
            print(f"  {sib / f}" if f != "(none)" else "  (none)")
    else:
        rows = [r for r in ours if r["Name"].split("@")[0] == args.what]
        if not rows:
            sys.exit(f"{args.what}: not in functions.csv")

    theirs = {r["Name"]: r for r in csv.DictReader((sib / "data/1.0/functions.csv").open())}
    names = demangle([r["Name"].split("@")[0] for r in rows])
    print(f"{'here':4} {'1.0':4} {'size':>6} {'1.0 size':>8}  function")
    for r in rows:
        name = r["Name"].split("@")[0]
        t = theirs.get(name)
        print(f"{r['Quality']:4} {t['Quality'] if t else '-':4} {int(r['Size']):>6} {int(t['Size']) if t else '':>8}  {names[name]}")
        if not t or t["Quality"] == "U" or name.startswith("_GLOBAL__"):
            continue
        qualified, last = short(names[name])
        for pattern in dict.fromkeys((qualified, last)):
            hits = subprocess.run(["git", "grep", "-n", "-w", "-F", pattern + "(", "--", "src"], cwd=sib,
                                  capture_output=True, text=True).stdout.splitlines()
            hits = [h for h in hits if not h.split(":", 2)[2].lstrip().startswith(("//", "*"))]
            if hits:
                for h in hits[:4]:
                    path, line, text = h.split(":", 2)
                    print(f"           {path}:{line}: {text.strip()[:100]}")
                if len(hits) > 4:
                    print(f"           ... {len(hits) - 4} more")
                break
    have = sum(1 for r in rows if theirs.get(r["Name"].split("@")[0], {}).get("Quality") in ("O", "m", "M"))
    print(f">>> the sibling has {have} of {len(rows)} written (sizes that differ mean the function changed between builds)")


if __name__ == "__main__":
    main()
