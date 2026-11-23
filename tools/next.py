#!/usr/bin/env python3
"""Which source file to decompile next.

Reads data/2.6.2/{functions,function_files,inline_helpers}.csv, src/ and the
reference headers in build/dwarf/headers/, and ranks the original source files
(the unit of work, see docs/decomp_workflow.md).

  tools/next.py                       top ready files
  tools/next.py --area X/Sigma/Math   restrict to a directory prefix
  tools/next.py --blocked             files that cannot start yet, and on what
  tools/next.py --all                 include finished files
  tools/next.py --file X/Sigma/Math/Rect.cpp   one file in detail
  tools/next.py --summary             progress per directory

Files held through tools/claim.py (claimed, done but not landed, or stuck) are
left out unless --all is given.

A file is
  done      every function in it is O (matches)
  partial   it exists under src/ or has some O/W functions, but is not done
  todo      nothing written yet
and, from the headers that the reference header beside it includes:
  ready     all of them are already under src/
  headers   some are not; `gen_headers.py --adopt` them first (listed)
  blocked   a missing one holds a class template, which exists in the DWARF only
            as an example instantiation and has to be written by hand first
            (the reference marks it `#if 0`). Do the blocker file first.

Rank: Value = Unlocks per KB of code. Unlocks is the number of functions
elsewhere that inline helpers defined in this file's headers
(inline_helpers.csv), so small files that many others lean on come first. It is
a hint, not a promise: a helper only helps once its callers are written the same
way. --by unlocks and --by size rank on those alone.
"""
import argparse
import csv
import os
import re
import sys
from collections import defaultdict
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
DATA = ROOT / "data/2.6.2"
HEADERS = ROOT / "build/dwarf/headers"
SRC = ROOT / "src"
SOURCE_EXT = (".cpp", ".cc", ".c")
INCLUDE = re.compile(r'^#include\s+"([^"]+)"', re.M)


def load():
    quality = {}
    size = {}
    for r in csv.DictReader((DATA / "functions.csv").open()):
        quality[r["Address"]] = r["Quality"]
        size[r["Address"]] = int(r["Size"])
    files = defaultdict(list)
    for r in csv.DictReader((DATA / "function_files.csv").open()):
        if r["File"].endswith(SOURCE_EXT):
            files[r["File"]].append(r["Address"])
    helper_callers = defaultdict(int)
    p = DATA / "inline_helpers.csv"
    if p.is_file():
        for r in csv.DictReader(p.open()):
            helper_callers[r["File"]] += int(r["Callers"])
    return quality, size, files, helper_callers


def stem(path):
    return path.rsplit(".", 1)[0]


def needs_handwriting(rel):
    ref = HEADERS / rel
    return ref.is_file() and "\n#if 0" in ref.read_text(errors="replace")


def missing_headers(path):
    """Project headers the reference header beside `path` includes that are not in src/."""
    out = []
    for ext in (".h", ".hpp"):
        ref = HEADERS / (stem(path) + ext)
        if not ref.is_file():
            continue
        for inc in INCLUDE.findall(ref.read_text(errors="replace")):
            if inc.endswith(SOURCE_EXT):
                continue
            rel = os.path.normpath(os.path.join(os.path.dirname(ref.relative_to(HEADERS)), inc))
            if rel.startswith(".."):
                continue
            if not (SRC / rel).is_file() and not (HEADERS / rel).is_file():
                continue  # not a project header (SDK / system): not ours to write
            if not (SRC / rel).is_file() and rel not in out:
                out.append(rel)
        break
    return out


def analyse():
    quality, size, files, helper_callers = load()
    rows = []
    for f, addrs in files.items():
        q = [quality.get(a, "U") for a in addrs]
        n_o, n_w = q.count("O"), q.count("W")
        exists = (SRC / f).is_file()
        if n_o == len(addrs):
            status = "done"
        elif exists or n_o or n_w or q.count("M") or q.count("m"):
            status = "partial"
        else:
            status = "todo"
        unlocks = sum(helper_callers.get(stem(f) + e, 0) for e in (".h", ".hpp", ".inl", ".cpp"))
        rows.append({
            "file": f, "status": status, "funcs": len(addrs), "o": n_o, "w": n_w,
            "bytes": sum(size.get(a, 0) for a in addrs), "unlocks": unlocks,
            "missing": [] if status == "done" else missing_headers(f),
        })
        r = rows[-1]
        r["tier"] = ("ready" if not r["missing"] else
                     "blocked" if any(needs_handwriting(m) for m in r["missing"]) else "headers")
        r["value"] = r["unlocks"] / (r["bytes"] / 1024 + 1)
    return rows


def claims():
    """Files someone holds (tools/claim.py). claims.csv lives in the main checkout."""
    try:
        import claim
        return claim.load()
    except Exception:
        return {}


def key(by):
    if by == "size":
        return lambda r: (-r["bytes"], r["file"])
    if by == "unlocks":
        return lambda r: (-r["unlocks"], -r["funcs"], r["file"])
    return lambda r: (-r["value"], r["file"])


def fmt(r):
    prog = f"{r['o']}/{r['funcs']}" + (f" +{r['w']}W" if r["w"] else "")
    return f"{r['status']:8} {r['tier']:8} {prog:>11} {r['bytes']:>8}B {r['unlocks']:>6} {r['value']:>6.0f}  {r['file']}"


def summary(rows):
    by = defaultdict(lambda: [0, 0, 0, 0])
    for r in rows:
        d = "/".join(r["file"].split("/")[:3])
        by[d][0] += r["funcs"]
        by[d][1] += r["o"]
        by[d][2] += 1
        by[d][3] += r["status"] == "done"
    print(f"{'directory':44} {'funcs O/total':>16} {'files done/total':>18}")
    for d, (n, o, nf, nd) in sorted(by.items(), key=lambda kv: -kv[1][0])[:40]:
        print(f"{d:44} {f'{o}/{n}':>16} {f'{nd}/{nf}':>18}")
    tn, to = sum(v[0] for v in by.values()), sum(v[1] for v in by.values())
    print(f"\ntotal: {to}/{tn} functions match ({100 * to / tn:.2f}%)")


def detail(rows, name):
    hit = [r for r in rows if r["file"] == name or r["file"].endswith("/" + name)]
    if not hit:
        sys.exit(f"{name}: not a source file in function_files.csv")
    r = hit[0]
    print(fmt(r))
    c = claims().get(r["file"])
    if c:
        print(f"claim:     {c['State']} by {c['Who']} {c['When']}" + (f" ({c['Note']})" if c["Note"] else ""))
    if r["missing"]:
        print("headers not in src/ yet" + (" (* needs hand-written templates)" if r["tier"] == "blocked" else "") + ":")
        for m in r["missing"]:
            print("  ", m, "*" if needs_handwriting(m) else "")
    else:
        print("ready: no missing headers")
    print(f"reference: build/dwarf/headers/{r['file']}")
    print(f"ghidra:    build/ghidra/decomp/{r['file']}")
    refs = sorted(p.relative_to(HEADERS).as_posix() for p in (HEADERS / r["file"]).parent.glob("*")
                  if p.suffix in (".h", ".hpp", ".inl") and p.name.startswith(Path(r["file"]).stem))
    for h in refs:
        if (SRC / h).is_file():
            print(f"have:      src/{h}")
        else:
            print(f"start:     python3 tools/gen_headers.py --adopt {h}")
    if not refs:
        print("start:     no header of its own in the DWARF; see what its functions use (tools/dwarf_fn.py)")
    print("note:      `ready` only means the headers its own header includes are in src/. Declarations inside\n"
          "           them, and headers the .cpp includes directly, may still be missing.")
    print(f"try:       python3 tools/try_file.py src/{r['file']}")
    ledger(r["file"])


def ledger(file):
    """The helper ledger's rows for the inline helpers this file's headers (and the file
    itself) define: how many callers wait on each, and whether src/ has a body for it."""
    import helper_ledger
    rows = helper_ledger.rows_for(stem(file))
    if not rows:
        if not helper_ledger.LEDGER.is_file():
            print("helpers:   no ledger yet; python3 tools/helper_ledger.py writes data/2.6.2/helper_ledger.csv")
        return
    rows.sort(key=lambda h: (-int(h["UnmatchedCallers"]), -int(h["Callers"])))
    print(f"helpers:   {len(rows)} inline helpers defined here (data/2.6.2/helper_ledger.csv)")
    print(f"{'':11}{'callers':>7} {'matched':>7} {'body':>7}  helper  ->  smallest unmatched caller file")
    for h in rows[:12]:
        print(f"{'':11}{h['Callers']:>7} {h['MatchedCallers']:>7} {h['Bodied']:>7}  {h['File'].rsplit('/', 1)[-1]}:{h['Line']} "
              f"{helper_ledger.short(h['Function'], 60)}"
              + (f"  ->  {h['SmallestUnmatchedCallerFile']}" if h["SmallestUnmatchedCallerFile"] else ""))
    if len(rows) > 12:
        print(f"{'':11}... {len(rows) - 12} more in the ledger")


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--area", default="", help="only files under this path prefix")
    ap.add_argument("--tier", choices=("ready", "headers", "blocked"), default="ready",
                    help="ready (default), headers (adopt some headers first) or blocked")
    ap.add_argument("--blocked", action="store_const", const="blocked", dest="tier", help=argparse.SUPPRESS)
    ap.add_argument("--all", action="store_true", help="include finished files")
    ap.add_argument("--status", choices=("todo", "partial", "done"))
    ap.add_argument("--by", choices=("value", "unlocks", "size"), default="value")
    ap.add_argument("-n", "--limit", type=int, default=25)
    ap.add_argument("--file", help="show one file in detail")
    ap.add_argument("--summary", action="store_true", help="progress per directory")
    args = ap.parse_args()

    rows = [r for r in analyse() if r["file"].startswith(args.area)]
    if args.file:
        return detail(rows, args.file)
    if args.summary:
        return summary(rows)

    if args.status:
        rows = [r for r in rows if r["status"] == args.status]
    elif not args.all:
        rows = [r for r in rows if r["status"] != "done"]
    rows = [r for r in rows if r["tier"] == args.tier]
    held = claims()
    if not args.all:
        n = len(rows)
        rows = [r for r in rows if r["file"] not in held]
        if n != len(rows):
            print(f"({n - len(rows)} claimed or stuck files left out; tools/claim.py list)")
    rows.sort(key=key(args.by))

    print(f"{'status':8} {'tier':8} {'O/funcs':>11} {'size':>9} {'unlocks':>6} {'value':>6}  file")
    for r in rows[:args.limit]:
        print(fmt(r))
        if args.tier != "ready":
            print("           needs:", ", ".join(r["missing"][:4]) + (" ..." if len(r["missing"]) > 4 else ""))
    if len(rows) > args.limit:
        print(f"... {len(rows) - args.limit} more (-n to show more)")


if __name__ == "__main__":
    main()
