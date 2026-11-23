#!/usr/bin/env python3
"""Claim a source file and get a private checkout to work on it.

  tools/claim.py next                 claim the best unclaimed ready file
  tools/claim.py next -n 5 --area Contrib/Contrib/DemonWare
  tools/claim.py next --who agent3    record a name other than your login
  tools/claim.py take X/Sigma/Math/Rect.cpp
  tools/claim.py done X/Sigma/Math/Rect.cpp       finished: ready for tools/land.py
  tools/claim.py stuck X/Sigma/Math/Rect.cpp "needs Foo.h, which holds a template"
  tools/claim.py release X/Sigma/Math/Rect.cpp    give it back untouched
  tools/claim.py list

Claims are rows in claims.csv at the top of the main checkout (not committed).
tools/next.py leaves claimed and stuck files out, so two people or agents do
not pick the same file.

A claim comes with a git worktree at build/work/<name> on branch work/<name>:
a full copy of the sources where tools/try_file.py, tools/dwarf_fn.py,
tools/data.py and tools/gen_headers.py --adopt all work. Do everything for the
file there. Do not run tools/build.py or tools/check in it; tools/land.py
merges the work into the main checkout, runs the full build and check, and
commits.

`done` is for any useful result, complete or not (NON_MATCHING functions are
fine). `stuck` records why a file cannot proceed and keeps it out of the queue.
Both retain the worktree, including ignored build and evidence files.
"""
import argparse
import csv
import fcntl
import getpass
import json
import shutil
import subprocess
import sys
import time
from game_data_manifest import MANIFEST, generated_files
from contextlib import contextmanager
from pathlib import Path

FIELDS = ["File", "State", "Who", "When", "Note"]
STATES = ("claimed", "done", "stuck")
# Inputs that are not in git (the dump, the DWARF reference tree, the decompiler output).
LINKS = ["data/2.6.2/main.elf", "build/dwarf", "build/ghidra", "build/romfs", "build/lua-backends"]
COPIES = ["build/2.6.2/compile_commands.json"]
# Copied, not linked: the compile flags load build/latepipe/<hash>/LatePipe.so from the
# worktree mounted at /work, where a link to the main checkout does not resolve.
COPY_TREES = ["build/latepipe"]


def git(*args, cwd=None, check=True):
    return subprocess.run(["git", *args], cwd=cwd, check=check, capture_output=True, text=True).stdout.strip()


def main_root():
    """The main checkout, also when called from inside a worktree."""
    here = Path(__file__).resolve().parent
    common = Path(git("rev-parse", "--path-format=absolute", "--git-common-dir", cwd=here))
    return common.parent


MAIN = main_root()
CLAIMS = MAIN / "claims.csv"


def coordination_queues(root=None):
    """Optional local ownership controller; no workflow package is imported."""
    root = Path(root) if root is not None else MAIN
    legacy = root / 'build/work-queue/queue.json'
    queues = [legacy] if legacy.exists() else []
    registry = root / '.git/d3-coordination.json'
    if registry.is_symlink():
        raise RuntimeError('Coordination registry must not be a symlink')
    if registry.exists():
        if registry.stat().st_size > 16384:
            raise RuntimeError('Coordination registry is oversized')
        value = json.loads(registry.read_text())
        if value.get('format') != 1 or not isinstance(value.get('queue_path'), str):
            raise RuntimeError('Unsupported coordination registry')
        path = Path(value['queue_path'])
        if (not path.is_absolute() or path != path.resolve()
                or path.is_symlink() or not path.is_file()):
            raise RuntimeError('Registered coordination queue is missing or not canonical')
        if path not in queues:
            queues.append(path)
    return queues


def check_coordination(root=None, selected_queue=None):
    for queue in coordination_queues(root):
        if selected_queue is not None and queue.resolve() == Path(selected_queue).resolve():
            continue  # A controller checks its own claim/resume policy under its queue lock.
        ledger = json.loads(queue.read_text())
        if not isinstance(ledger, dict) or not isinstance(ledger.get('items'), dict):
            raise RuntimeError('Invalid coordination queue; reconcile before claim changes')
        if any(not isinstance(item, dict) for item in ledger['items'].values()):
            raise RuntimeError('Invalid coordination queue item')
        if any(item.get('active') for item in ledger['items'].values()):
            raise RuntimeError('Durable queue controller has an active claim; reconcile it before legacy claim changes')


@contextmanager
def locked():
    """Read claims.csv, let the caller change the rows, write it back; one writer at a time."""
    CLAIMS.touch(exist_ok=True)
    with CLAIMS.open("r+", newline="") as f:
        fcntl.flock(f, fcntl.LOCK_EX)
        check_coordination()
        rows = {r["File"]: r for r in csv.DictReader(f)}
        yield rows
        f.seek(0)
        f.truncate()
        w = csv.DictWriter(f, FIELDS)
        w.writeheader()
        w.writerows(rows.values())


def load():
    if not CLAIMS.is_file():
        return {}
    return {r["File"]: r for r in csv.DictReader(CLAIMS.open(newline=""))}


def slug(file):
    return file.rsplit(".", 1)[0].replace("/", "_").replace(" ", "_")


def worktree(file):
    return MAIN / "build/work" / slug(file)


def branch(file):
    return "work/" + slug(file)


def make_worktree(file):
    wt = worktree(file)
    if wt.is_dir():
        return wt
    wt.parent.mkdir(parents=True, exist_ok=True)
    base = git("rev-parse", "--abbrev-ref", "HEAD", cwd=MAIN)
    have = git("branch", "--list", branch(file), cwd=MAIN)
    git("worktree", "add", *([] if have else ["-b", branch(file)]), str(wt), *([branch(file)] if have else [base]),
        cwd=MAIN)
    for rel in LINKS:
        src, dst = MAIN / rel, wt / rel
        if src.exists() and not dst.exists():
            dst.parent.mkdir(parents=True, exist_ok=True)
            dst.symlink_to(src)
    for rel in COPIES:
        src, dst = MAIN / rel, wt / rel
        if src.is_file():
            dst.parent.mkdir(parents=True, exist_ok=True)
            shutil.copyfile(src, dst)
    for rel in COPY_TREES:
        src, dst = MAIN / rel, wt / rel
        if src.is_dir():
            shutil.copytree(src, dst, dirs_exist_ok=True)
    # The matching sources use private generated files that a Git worktree omits.
    # Copy only generated outputs, keeping each worktree's tracked layout checks intact.
    manifest = wt / MANIFEST
    if manifest.is_file():
        for rel in generated_files(json.loads(manifest.read_text())):
            if (MAIN / rel).is_file():
                (wt / rel).parent.mkdir(parents=True, exist_ok=True)
                shutil.copyfile(MAIN / rel, wt / rel)
    return wt


def remove_worktree(file, force=False):
    wt = worktree(file)
    if wt.is_dir():
        if not force and git("status", "--porcelain", cwd=wt):
            sys.exit(f"{wt} has uncommitted changes; land it, or pass --force to throw them away")
        git("worktree", "remove", "--force", str(wt), cwd=MAIN)
    if git("branch", "--list", branch(file), cwd=MAIN):
        git("branch", "-D", branch(file), cwd=MAIN)


def candidates(area):
    sys.path.insert(0, str(Path(__file__).resolve().parent))
    import next as queue
    rows = [r for r in queue.analyse() if r["file"].startswith(area) and r["status"] != "done"
            and r["tier"] == "ready"]
    rows.sort(key=queue.key("value"))
    return [r["file"] for r in rows]


def known_file(file):
    with (MAIN / "data/2.6.2/function_files.csv").open() as f:
        return any(line.split(",")[1] == file for line in f)


def stamp(rows, file, state, who, note=""):
    rows[file] = {"File": file, "State": state, "Who": who, "When": time.strftime("%Y-%m-%d %H:%M"), "Note": note}


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = ap.add_subparsers(dest="cmd", required=True)
    p = sub.add_parser("next", help="claim the best unclaimed ready file(s)")
    p.add_argument("-n", type=int, default=1)
    p.add_argument("--area", default="")
    p.add_argument("--who", default=getpass.getuser())
    p = sub.add_parser("take", help="claim one named file")
    p.add_argument("file")
    p.add_argument("--who", default=getpass.getuser())
    p = sub.add_parser("done", help="mark a claimed file finished, ready to land")
    p.add_argument("file")
    p.add_argument("note", nargs="?", default="")
    p = sub.add_parser("stuck", help="record why a file cannot be done yet")
    p.add_argument("file")
    p.add_argument("note")
    p = sub.add_parser("release", help="drop a claim and its worktree")
    p.add_argument("file")
    p.add_argument("--force", action="store_true", help="also throw away uncommitted work")
    sub.add_parser("list", help="show all claims")
    args = ap.parse_args()

    if args.cmd == "list":
        for r in load().values():
            print(f"{r['State']:8} {r['Who']:12} {r['When']:16}  {r['File']}" + (f"   ({r['Note']})" if r["Note"] else ""))
        return

    if args.cmd in ("next", "take"):
        if args.cmd == "take" and not known_file(args.file):
            sys.exit(f"{args.file}: not a source file in function_files.csv")
        pool = [args.file] if args.cmd == "take" else candidates(args.area)
        want = 1 if args.cmd == "take" else args.n
        got = []
        with locked() as rows:
            for f in pool:
                if len(got) == want:
                    break
                if f in rows:
                    if args.cmd == "take":
                        r = rows[f]
                        sys.exit(f"{f} is already {r['State']} by {r['Who']}" + (f": {r['Note']}" if r["Note"] else ""))
                    continue
                stamp(rows, f, "claimed", args.who)
                got.append(f)
        if not got:
            sys.exit("nothing unclaimed is ready")
        for f in got:
            wt = make_worktree(f)
            print(f"{f}\t{wt}")
            if len(got) == 1:
                print(f"\nWork in that directory:\n  cd {wt}\n  python3 tools/next.py --file {f}\n"
                      f"When finished, from there: python3 tools/claim.py done {f}\n"
                      "No need to commit; tools/land.py commits what is in the directory.")
        return

    with locked() as rows:
        if args.file not in rows:
            sys.exit(f"{args.file} is not claimed")
        who = rows[args.file]["Who"]
        if args.cmd == "done":
            stamp(rows, args.file, "done", who, args.note)
        elif args.cmd == "stuck":
            stamp(rows, args.file, "stuck", who, args.note)
        else:
            remove_worktree(args.file, args.force)
            del rows[args.file]
    print({"done": f"{args.file}: marked done. `python3 tools/land.py` in the main checkout builds, checks and commits it.",
           "stuck": f"{args.file}: marked stuck and left out of the queue.",
           "release": f"{args.file}: released."}[args.cmd])
if __name__ == "__main__":
    main()
