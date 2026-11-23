#!/usr/bin/env python3
"""Generate data/2.6.2/function_files.csv: which original source file (DWARF
compile unit) each function in functions.csv was compiled from.

Row format: Address,File,Compiler
  Address   same as functions.csv (0x7100000000 | vaddr)
  File      the compile unit's source path, with the build server prefix
            stripped (so game code reads X/Client/CGame.cpp)
  Compiler  the LLVM version from DW_AT_producer

A function is assigned to the compile unit whose address ranges contain it.
An inline/template function emitted in several units is claimed by all of
them; it goes to the claimant that owns the functions next to it, which is
the unit the linker kept the copy from. Otherwise, an unambiguous concrete
subprogram range exactly covering the inventory function can recover its
emitting CU. Declaration files never override CU ownership. Functions without
either kind of evidence (SDK objects, linker stubs) get an empty File, unless
config/function_file_supplement.csv names an owner for them (prebuilt libraries
with no DWARF whose upstream source has been identified).
"""
import argparse
import bisect
import csv
import os
import posixpath
import re
import shutil
import subprocess
import sys
import tempfile
from contextlib import contextmanager
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
PREFIXES = (
    "D:/BuildServer/2/work/code/branches/2_6_2/",
    "D:/home/teamcity2/work/sdk/",
    "C:/Projects/D3/Main/Trunk/",
    "C:/dev/d3/main/trunk/",
    "C:/dev/d3/trunk/",
    "C:/Project/",
)
ATTR = re.compile(r'^\s+DW_AT_(\w+)\s+\((.*?)\)?$')
RANGE = re.compile(r'\[0x([0-9a-f]+), 0x([0-9a-f]+)\)')
DIE = re.compile(r'^0x([0-9a-f]+):\s+(DW_TAG_\w+|NULL)')


@contextmanager
def atomic_output(path):
    """Never replace the prior mapping with a partial or failed generation."""
    name = None
    try:
        with tempfile.NamedTemporaryFile(mode="w", dir=path.parent,
                                         prefix=path.name + ".", suffix=".tmp",
                                         delete=False) as output:
            name = output.name
            yield output
        os.replace(name, path)
    finally:
        if name is not None:
            Path(name).unlink(missing_ok=True)


def compiler_version(producer):
    match = re.search(r"based on LLVM ([\w.\-]+)", producer)
    return match.group(1) if match else ""


def parse_subprograms(lines):
    """Collect concrete body ranges, retaining their *emitting CU* ownership.

    Declaration paths are provenance only. In particular, an aggregate CU can
    emit bodies declared in another .cpp or in a header. Never inherit ranges
    from an abstract origin/specification or count inline instances as bodies.
    Keep unknown owners and non-contiguous ranges as conflicting evidence.
    """
    records, dies, unit, cur = [], {}, {}, None

    def flush():
        name = unquote(unit.get("name", ""))
        directory = unquote(unit.get("comp_dir", ""))
        path = source_path(name, directory) if name else ""
        compiler = compiler_version(unquote(unit.get("producer", "")))
        for die in dies.values():
            if die.get("declaration", "false") not in ("false", "0", "0x00"):
                continue
            ranges = die.get("ranges", [])
            if not ranges and "low_pc" in die and "high_pc" in die:
                # llvm-dwarfdump's default rendering resolves high_pc offsets
                # to absolute PCs; do not request --show-form or --verbose.
                ranges = [(int(die["low_pc"], 16), int(die["high_pc"], 16))]
            decl, seen, origin = "", set(), die
            while origin is not None and id(origin) not in seen:
                seen.add(id(origin))
                if "decl_file" in origin:
                    decl = source_path(unquote(origin["decl_file"]), directory)
                    break
                reference = origin.get("abstract_origin") or origin.get("specification")
                origin = dies.get(int(reference.split()[0], 16)) if reference else None
            for lo, hi in ranges:
                if lo > 0 and hi > lo:
                    records.append({"low_pc": lo, "high_pc": hi, "file": path,
                                    "compiler": compiler, "decl_file": decl,
                                    "contiguous": len(ranges) == 1})

    for line in lines:
        match = DIE.match(line)
        if match:
            tag = match.group(2)
            if tag == "DW_TAG_compile_unit":
                flush()
                dies, unit = {}, {}
                cur = unit
            elif tag == "DW_TAG_subprogram":
                cur = {}
                dies[int(match.group(1), 16)] = cur
            else:
                cur = None
            continue
        if cur is None:
            continue
        match = ATTR.match(line)
        if match and match.group(1) in ("name", "comp_dir", "producer", "low_pc",
                                       "high_pc", "declaration", "decl_file",
                                       "abstract_origin", "specification"):
            cur[match.group(1)] = match.group(2)
        for lo, hi in RANGE.findall(line):
            cur.setdefault("ranges", []).append((int(lo, 16), int(hi, 16)))
    flush()
    return records


def read_subprograms(nss):
    # Stream the full dump: --lookup relies on the very CU ranges missing here.
    with subprocess.Popen([find_dwarfdump(), "--debug-info", str(nss)],
                          stdout=subprocess.PIPE, stderr=subprocess.DEVNULL,
                          text=True, errors="replace") as proc:
        try:
            records = parse_subprograms(proc.stdout)
        except BaseException:
            # Popen.__exit__ waits: kill before leaving if parsing fails while
            # dwarfdump may still be blocked writing its large stdout pipe.
            proc.kill()
            proc.wait()
            raise
        if proc.wait():
            raise subprocess.CalledProcessError(proc.returncode, proc.args)
    return records


def fallback_owners(vaddr, size, records):
    """Return a unique exact-body CU; contradictory overlap stays unattributed."""
    if vaddr <= 0 or size <= 0:
        return {}
    candidates = set()
    for rec in records:
        lo, hi = rec["low_pc"], rec["high_pc"]
        if lo >= vaddr + size or hi <= vaddr:
            continue
        path = rec.get("file", "")
        if (lo != vaddr or hi != vaddr + size or not rec.get("contiguous", True)
                or rec.get("declaration", False) or not path
                or posixpath.splitext(path)[1].lower() not in (".c", ".cpp", ".cc", ".cxx")
                or path.startswith(("/", "../")) or ":" in path):
            return {}
        candidates.add((path, rec.get("compiler", "")))
    return dict(candidates) if len(candidates) == 1 else {}


def find_dwarfdump():
    for cand in ("llvm-dwarfdump", "/opt/homebrew/opt/llvm/bin/llvm-dwarfdump"):
        if shutil.which(cand):
            return shutil.which(cand)
    sys.exit("llvm-dwarfdump not found (brew install llvm / apt install llvm)")


def unquote(v):
    return v.strip('"').replace("\\\\", "/").replace("\\", "/")


def source_path(name, comp_dir):
    if not re.match(r"^([A-Za-z]:)?/", name):
        name = comp_dir + "/" + name
    name = posixpath.normpath(re.sub(r"/+", "/", name))
    for prefix in PREFIXES:
        if name.lower().startswith(prefix.lower()):
            return name[len(prefix):]
    return name


def read_units(nss):
    out = subprocess.check_output(
        [find_dwarfdump(), "--debug-info", "--show-children=false", "-r", "0", str(nss)],
        text=True, stderr=subprocess.DEVNULL)
    units, cur = [], None
    for line in out.splitlines():
        if "DW_TAG_compile_unit" in line:
            cur = {"ranges": []}
            units.append(cur)
            continue
        if cur is None:
            continue
        m = ATTR.match(line)
        if m and m.group(1) in ("name", "comp_dir", "producer"):
            cur[m.group(1)] = unquote(m.group(2))
        elif m and m.group(1) in ("low_pc", "high_pc"):
            cur[m.group(1)] = int(m.group(2), 16)
        for lo, hi in RANGE.findall(line):
            cur["ranges"].append((int(lo, 16), int(hi, 16)))
    for u in units:
        if not u["ranges"] and u.get("high_pc"):
            u["ranges"].append((u["low_pc"], u["high_pc"]))
    return units


def main():
    p = argparse.ArgumentParser()
    p.add_argument("nss", type=Path, nargs="?", default=ROOT / "build/romfs/DiabloIIINX64r.nss")
    p.add_argument("--functions", type=Path, default=ROOT / "data/2.6.2/functions.csv")
    p.add_argument("-o", "--out", type=Path, default=ROOT / "data/2.6.2/function_files.csv")
    args = p.parse_args()
    if not args.nss.is_file():
        sys.exit(f"{args.nss} not found")

    spans = []
    for u in read_units(args.nss):
        path = source_path(u.get("name", ""), u.get("comp_dir", ""))
        compiler = compiler_version(u.get("producer", ""))
        for lo, hi in u["ranges"]:
            if hi > lo:
                spans.append((lo, hi, path, compiler))
    spans.sort()
    starts = [s[0] for s in spans]

    # An inline function that was not inlined is emitted by every unit that uses
    # it; the linker keeps one copy and every such unit's ranges point at it.
    # The copy sits among the other functions of the unit it was kept from, so
    # give it to the claimant that owns the nearest unambiguous neighbour.
    claims = {}
    for lo, hi, path, compiler in spans:
        claims.setdefault(lo, {})[path] = compiler

    def owners(vaddr):
        if vaddr in claims:
            return claims[vaddr]
        i = bisect.bisect_right(starts, vaddr) - 1
        if i >= 0 and vaddr < spans[i][1]:
            return {spans[i][2]: spans[i][3]}
        return {}

    with args.functions.open() as f:
        functions = list(csv.DictReader(f))
    addrs = [row["Address"] for row in functions]
    sizes = [int(row["Size"]) for row in functions]
    own = [owners(int(a, 16) & 0xFFFFFFFF) for a in addrs]
    subprograms = read_subprograms(args.nss) if any(not c for c in own) else []
    assigned = shared = recovered = supplemented = 0
    supplement = {}
    supplement_csv = ROOT / "config/function_file_supplement.csv"
    if supplement_csv.is_file():
        with supplement_csv.open(newline="") as f:
            supplement = {r["Address"]: (r["File"], r["Compiler"]) for r in csv.DictReader(f)}
    files = set()
    with atomic_output(args.out) as o:
        o.write("Address,File,Compiler\n")
        for n, (addr, cands) in enumerate(zip(addrs, own)):
            path = compiler = ""
            if len(cands) == 1:
                (path, compiler), = cands.items()
            elif cands:
                shared += 1
                for d in range(1, 64):
                    near = [own[k] for k in (n - d, n + d) if 0 <= k < len(own) and len(own[k]) == 1]
                    hit = [q for c in near for q in c if q in cands]
                    if hit:
                        path = hit[0]
                        break
                else:
                    path = sorted(cands)[-1]
                compiler = cands[path]
            else:
                # Apply only after the original CU/neighbour pass. Recovered
                # owners must not change tie-breaking for existing claims.
                fallback = fallback_owners(int(addr, 16) & 0xFFFFFFFF,
                                           sizes[n], subprograms)
                if fallback:
                    (path, compiler), = fallback.items()
                    recovered += 1
            if not path and addr in supplement:
                # No DWARF at all (prebuilt library): owner identified from
                # upstream source, see config/function_file_supplement.csv.
                path, compiler = supplement[addr]
                supplemented += 1
            if path:
                assigned += 1
                files.add(path)
            o.write(f"{addr},{path},{compiler}\n")
    total = len(addrs)
    print(f">>> {shared} functions are claimed by several units; assigned by neighbour")
    print(f">>> {recovered} previously unowned functions recovered from concrete subprograms")
    print(f">>> {supplemented} functions without DWARF assigned from config/function_file_supplement.csv")
    print(f">>> wrote {args.out}: {assigned}/{total} functions in {len(files)} source files")


if __name__ == "__main__":
    main()
