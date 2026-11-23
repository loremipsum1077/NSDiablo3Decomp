#!/usr/bin/env python3
"""Rank inline functions by how many functions in the binary they were inlined
into, and write data/2.6.2/inline_helpers.csv. An inline helper has to be
written the way the original was before its callers can match, so the top of
this list is the work that unlocks the most functions.

Row format: Callers,Sites,File,Line,Function
  Callers  distinct functions in the binary with this helper inlined into them
           (directly or through another inlined helper)
  Sites    total inlined call sites
  File     the source file that defines the helper, and the Line
All instantiations of a template are counted as one helper.
"""
import argparse
import csv
from pathlib import Path

import dwarf_index

ROOT = Path(__file__).resolve().parent.parent


def signature(rec):
    scope = dwarf_index.strip_targs("::".join(rec["scope"]))
    name = dwarf_index.strip_targs(rec["name"])
    params = ", ".join(f"{t} {n}".strip() for t, n in rec["params"])
    return f"{rec['ret']} {scope + '::' if scope else ''}{name}({params}){' const' if rec['const'] else ''}"


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("-o", "--out", type=Path, default=ROOT / "data/2.6.2/inline_helpers.csv")
    ap.add_argument("--top", type=int, default=25, help="rows to print")
    ap.add_argument("--prefix", default="", help="only print helpers defined under this path")
    args = ap.parse_args()

    index = dwarf_index.load()
    rows = []
    for key, (callers, sites) in index["inlined"].items():
        rec = index["funcs"].get(key)
        if rec is None:
            continue
        rows.append((len(callers), sites, rec["file"], rec["line"], signature(rec)))
    # Different helpers can share a source line and caller/site counts.
    rows.sort(key=lambda r: (-r[0], -r[1], r[2], r[3], r[4]))
    with args.out.open("w", newline="") as f:
        w = csv.writer(f, lineterminator="\n")
        w.writerow(["Callers", "Sites", "File", "Line", "Function"])
        w.writerows(rows)
    print(f">>> wrote {len(rows)} inline helpers to {args.out}")
    shown = [r for r in rows if r[2].startswith(args.prefix)][:args.top]
    for callers, sites, path, line, sig in shown:
        print(f"{callers:6} {sites:7}  {path}:{line}  {sig[:110]}")


if __name__ == "__main__":
    main()
