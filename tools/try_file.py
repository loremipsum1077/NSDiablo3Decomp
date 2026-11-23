#!/usr/bin/env python3
"""Compile one source file on its own and compare every function it defines
against main.elf, without touching the shared build or functions.csv. Safe
to run from several sessions at once.

  python3 tools/try_file.py src/X/Sigma/Math/Rect.cpp
  python3 tools/try_file.py src/X/Sigma/Math/Rect.cpp --diff _ZN4AABB7InflateEf
  python3 tools/try_file.py src/X/Sigma/Math/Rect.cpp --asm _ZN4AABB7InflateEf

--diff prints the two disassemblies as one diff (addresses removed, original
lines marked `-`, ours `+`; an inserted instruction also shifts later branch targets); --asm prints them one after the other. Compiler
warnings are hidden unless the compile fails or you pass --warnings. Includes
resolve from src/ and include/.

A function is OK when it has the original size and identical instructions
after masking the bits relocations fill in (so call and data targets are not
verified here; tools/check does that on the full build). Functions the
original file defines that this file does not yet are listed as missing.

A file under tests/ (not part of the build) checks templates: explicitly
instantiate a template with arguments the binary uses, e.g.
`template class blz::list<unsigned int>;`, and each emitted function whose
mangled name is in functions.csv is compared.
The flags come from build/2.6.2/compile_commands.json (run tools/build.py once).
"""
import argparse
import csv
import difflib
import json
import os
import hashlib
import sqlite3
from contextlib import closing
import time
import re
import shlex
import shutil
import subprocess
import sys
from pathlib import Path

from validation import elf as elf_reader
from validation import scoring

ROOT = Path(__file__).resolve().parent.parent
DATA = ROOT / "data/2.6.2"
NSS = ROOT / "build/romfs/DiabloIIINX64r.nss"


def compile_command(src):
    db = ROOT / "build/2.6.2/compile_commands.json"
    if not db.is_file():
        sys.exit("build/2.6.2/compile_commands.json not found: run python3 tools/build.py first")
    entries = json.loads(db.read_text())
    wanted = "/work/" + src.relative_to(ROOT).as_posix()
    same_dir = [e for e in entries if Path(e["file"]).parent == Path(wanted).parent]
    # files outside src/ (tests/) are instantiated from game code: game flags first
    same_kind = sorted((e for e in entries if e["file"].endswith(".c") == (src.suffix == ".c")
                        and "/src/" in e["file"]), key=lambda e: "/src/X/" not in e["file"])
    for entry in [e for e in entries if e["file"] == wanted] + same_dir + same_kind:
        if entry["file"].endswith(".c") == (src.suffix == ".c"):
            cmd = shlex.split(entry["command"])
            break
    else:
        sys.exit("no compile command to copy flags from: run python3 tools/build.py")
    out, skip = [], False
    for arg in cmd:
        if skip:
            skip = False
        elif arg in ("-o", "-MF", "-MT"):
            skip = True
        elif arg.startswith("-I../../"):
            # relative to the build directory; we compile from the repo root
            out.append("-I/work/" + arg[len("-I../../"):])
        elif arg not in ("-c", "-MD") and not arg.endswith((".cpp", ".cc", ".c")):
            out.append(arg)
        if arg.startswith("-fplugin=/work/") and not (ROOT / arg[len("-fplugin=/work/"):]).is_file():
            # the late-GlobalOpt plugin (docs/late_globalopt.md); claim worktrees get a copy
            sys.exit(f"{arg[len('-fplugin='):]} not found under {ROOT}: run "
                     "python3 toolchain/latepipe/latepipe.py --build (or tools/build.py)")
    return out


LUA_CACHES = ROOT / "build/lua-backends"


def is_lua(src):
    """Lua was built with a different code generator; use it when it is prepared (tools/build.py does)."""
    return src.parent == ROOT / "src/Contrib/Contrib/lua/src" and LUA_CACHES.exists() and any(LUA_CACHES.iterdir())


def lua_command(cmd):
    sys.path.insert(0, str(ROOT / "toolchain"))
    import backend
    cache = backend.resolve(ROOT).resolve()
    return ["docker", "run", "--rm", "--pull", "never", "--network", "none", "-v", f"{ROOT}:/work",
            "-v", f"{cache}:/opt/d3-lua-cache:ro", "-e", "D3_LUA_TOOLCHAIN=/opt/d3-lua-cache",
            "-w", "/work", "d3-lua-build:5.0.1", "python3", "toolchain/lua/compile.py", *cmd]


def objdump(*args):
    exe = shutil.which("llvm-objdump") or "/opt/homebrew/opt/llvm/bin/llvm-objdump"
    return subprocess.run([exe, "-d", "--no-show-raw-insn", *args], capture_output=True, text=True).stdout


INSN = re.compile(r"^\s*([0-9a-f]+):\s+(?:(?:[0-9a-f]{2} ){4})?\s*(.*)$")


def clean(text, base):
    """Instruction lines without addresses (branch targets become offsets from
    the function start), so the two listings line up. Operands a relocation fills in (adrp pages, call
    targets, :lo12: offsets) still differ; they are not what you are looking for."""
    out = []
    for line in text.splitlines():
        m = INSN.match(line)
        if not m:
            continue
        ins = re.sub(r"\s+", " ", m.group(2).strip())
        ins = re.sub(r"0x[0-9a-f]+ <[^>+]*\+(0x[0-9a-f]+)>", r"<+\1>", ins)   # branch inside the function
        ins = re.sub(r"0x[0-9a-f]+ <([^>]*)>", r"<\1>", ins)
        # Operands the linker fills in are never comparable here: hide them so the
        # diff shows real differences only (tools/land.py verifies the targets).
        ins = re.sub(r"^bl <.*>$", "bl <call>", ins)
        ins = re.sub(r"^(adrp x\d+), .*$", r"\1, <page>", ins)
        ins = ins.replace(".word 0xe7ffdefe", "<trap>")
        ins = re.sub(r"^brk #0x[0-9a-f]+$", "<trap>", ins)
        out.append(ins)
    return out


def report_compile_result(run, object_exists, warnings=False):
    """Report failed compiler/container output without filtering its cause."""
    failed = run.returncode != 0 or not object_exists
    if failed:
        sys.stderr.write(f"Compile command exited with status {run.returncode}")
        if not object_exists:
            sys.stderr.write("; object file was not produced")
        sys.stderr.write("\n")
        for output in (run.stdout, run.stderr):
            if output:
                sys.stderr.write(output)
                if not output.endswith("\n"):
                    sys.stderr.write("\n")
    elif warnings and run.stderr:
        lines = [line for line in run.stderr.splitlines() if "warning" in line or "error" in line
                 or line.startswith(" ") or "note:" in line]
        if lines:
            sys.stderr.write("\n".join(lines) + "\n")
    return not failed


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("source", type=Path, help="a file under src/")
    ap.add_argument("--asm", metavar="NAME", help="print our and the original disassembly of one function")
    ap.add_argument("--diff", metavar="NAME", help="print a diff of the original and our disassembly of one function")
    ap.add_argument("--warnings", action="store_true", help="show compiler warnings")
    ap.add_argument("--json", action="store_true", help="structured address-based results; matches still mask relocations")
    args = ap.parse_args()
    if args.json and (args.asm or args.diff):
        ap.error("--json cannot be combined with --asm or --diff")

    # One object compile is small and writes only under build/try/<file>: no host
    # lease or Docker session, so any number of sessions can run this at once and
    # a compile error cannot block later runs.
    return _run(args)


def _run(args):

    src = args.source.resolve()
    # A file under tests/ is a template check: it instantiates templates with
    # the arguments the binary uses, and every function it emits that
    # functions.csv knows is compared, whichever original file owns it.
    in_src = (ROOT / "src") in src.parents
    rel = src.relative_to(ROOT / "src" if in_src else ROOT).as_posix()
    workdir = ROOT / "build/try" / rel.replace("/", "_")
    workdir.mkdir(parents=True, exist_ok=True)
    obj = workdir / "obj.o"
    obj.unlink(missing_ok=True)
    cmd = compile_command(src) + ["-c", "-o", "/work/" + obj.relative_to(ROOT).as_posix(),
                                  "/work/" + src.relative_to(ROOT).as_posix()]
    try:
        if os.environ.get("D3_VALIDATION_CONTAINER") and os.environ.get("D3_PILOT_DB"):
            import fast_compile
            run = fast_compile.compile(ROOT, cmd, obj)
        else:
            run = subprocess.run(lua_command(cmd) if is_lua(src) else
                                 ["docker", "compose", "run", "--rm", "-T", "build", *cmd],
                                 cwd=ROOT, capture_output=True, text=True, stdin=subprocess.DEVNULL)
    except OSError as error:
        sys.stderr.write(f"Could not start compile command: {error}\n")
        if args.json:
            print(json.dumps({"schema": 1, "compiled": False, "source": rel, "error": "compile failed"}))
        sys.exit(1)
    if not report_compile_result(run, obj.is_file(), args.warnings):
        if args.json:
            print(json.dumps({"schema": 1, "compiled": False, "source": rel, "error": "compile failed"}))
        sys.exit(1)

    files = {r["Address"]: r["File"] for r in csv.DictReader((DATA / "function_files.csv").open())}
    known = {}
    for r in csv.DictReader((DATA / "functions.csv").open()):
        known[r["Name"]] = (int(r["Address"], 16) & 0xFFFFFFFF, int(r["Size"]))
    # A name several files define is <name>@<file> in functions.csv: resolve
    # the bare name in our object to the copy that belongs to this source file.
    for r in csv.DictReader((DATA / "functions.csv").open()):
        if "@" in r["Name"] and files.get(r["Address"], "").lower() == rel.lower():
            known[r["Name"].split("@", 1)[0]] = known[r["Name"]]
    built = elf_reader.Elf(obj).functions()
    targets = {rel: {name: known[name] for name in built if name in known}}

    one = args.asm or args.diff
    if one and one not in targets[rel]:
        near = [n for n in targets[rel] if one in n] or sorted(targets[rel])
        sys.exit(f"{one} is not both in the object and in functions.csv. Functions in the object:\n  "
                 + "\n  ".join(near[:40]))
    if args.diff:
        vaddr, size = known[one]
        ours = clean(objdump(f"--disassemble-symbols={one}", str(obj)), 0)
        theirs = clean(objdump(f"--start-address=0x{vaddr:x}", f"--stop-address=0x{vaddr + size:x}", str(NSS)), vaddr)
        for line in difflib.unified_diff(theirs, ours, "original", "ours", n=3, lineterm=""):
            print(line)
        if theirs == ours:
            print("no difference outside relocated operands")
        return
    if args.asm:
        vaddr, size = known[args.asm]
        print("=== ours")
        print(objdump(f"--disassemble-symbols={args.asm}", str(obj)))
        print("=== original")
        print(objdump(f"--start-address=0x{vaddr:x}", f"--stop-address=0x{vaddr + size:x}", str(NSS)))
        return

    orig = elf_reader.Elf(DATA / "main.elf")
    # score() looks for <objdir>/<source name>.o
    shutil.copyfile(obj, workdir / (Path(rel).name + ".o"))
    exact, _, _, detail = scoring.score(workdir, targets, orig)
    try:
        from validation_result import summarize
    except ImportError:
        if args.json:
            raise
        summarize = None
    with (DATA / "functions.csv").open() as stream:
        inventory = list(csv.DictReader(stream))
    result = summarize(rel, inventory, files, targets[rel], detail, built) if summarize else None
    # Record each scored check, including repeated identical object output. These
    # are observations, not a claim that each invocation tested a new hypothesis.
    if result and os.environ.get("D3_PILOT_DB") and os.environ.get("D3_PILOT_TASK"):
        with closing(sqlite3.connect(os.environ["D3_PILOT_DB"], timeout=60)) as db, db:
            db.execute("CREATE TABLE IF NOT EXISTS check_results(task TEXT, checked REAL, source_sha256 TEXT, object_sha256 TEXT, result TEXT)")
            db.execute("INSERT INTO check_results VALUES (?,?,?,?,?)", (
                os.environ["D3_PILOT_TASK"], time.time(), hashlib.sha256(src.read_bytes()).hexdigest(),
                hashlib.sha256(obj.read_bytes()).hexdigest(), json.dumps(result)))
    if args.json:
        print(json.dumps(result))
        return
    for name, d in sorted(detail.items(), key=lambda kv: targets[rel][kv[0]][0]):
        print(f"  {d:28} {name}")
    unknown = sorted(n for n in built if n not in known)
    if unknown:
        print(f"  not in functions.csv (inlined or absent in the original; an unmangled original name "
              f"means the function is extern \"C\"): {' '.join(unknown)}")
    if not in_src:
        print(f">>> {exact}/{len(detail)} functions match")
        return
    done = {targets[rel][n][0] for n in targets[rel]}
    missing = [r["Name"] for r in csv.DictReader((DATA / "functions.csv").open())
               if files.get(r["Address"], "").lower() == rel.lower()
               and int(r["Address"], 16) & 0xFFFFFFFF not in done]
    print(f">>> {exact}/{len(detail)} functions match; {len(missing)} of this file's functions not written yet")
    if missing:
        print("    missing: " + " ".join(missing[:40]) + (" ..." if len(missing) > 40 else ""))


if __name__ == "__main__":
    main()
