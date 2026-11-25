#!/usr/bin/env python3
"""Build, check and commit finished source files.

  tools/land.py X/Sigma/Math/Rect.cpp     land one original source file
  tools/land.py                           land every claim marked done
  tools/land.py --sync                    no source change: rebuild, recheck, update labels
  tools/land.py --no-commit ...           do everything except the commit

A file with a claim worktree (tools/claim.py) is squash-merged into the main
checkout first. A file without one is landed from the main checkout as it
stands, uncommitted edits included.

The gate is one incremental build and one checker run:

  * the full build must succeed;
  * no function that was O may now differ from the original;
  * every function the checker accepts becomes O;
  * a function of the landed file that the checker cannot finish proving
    (PROOF_GAPS: a reference to data nobody has defined yet, an unproven jump
    table) becomes O when tools/try_file.py finds every instruction equal;
  * a function of the landed file that is compiled but differs becomes M;
  * a function labelled L (library code excluded from the project, see
    docs/library_functions.md) is never checked and keeps its label.

Proof gaps are counted in the output, not treated as failures: they close as
the code and data they refer to are written. Lua-mode builds, receipts and
review records are not part of this gate.

On failure nothing is committed and functions.csv is put back. Merged
worktree changes are undone; work done directly in the main checkout is left
alone. The full checker output is in build/land-failed.txt.
"""
import argparse
import csv
import fcntl
import json
import re
import subprocess
import sys

import claim
import proof_debt

MAIN = claim.MAIN
import project_version

VERSION = project_version.VERSION
FUNCTIONS = MAIN / f"data/{VERSION}/functions.csv"
FAILED = MAIN / "build/land-failed.txt"
# Checker verdicts that mean "could not prove this reference", not "the code differs".
PROOF_GAPS = {"got-original-unknown", "got-rebuilt-undefined", "got-original-undefined",
              "adrp-indirect-flow", "jump-table-indirect-guard", "jump-table-unproven"}
# "adrp-conflicting-flow" is deliberately absent. The checker prints that text for any
# ADD/LDR/STR whose imm12 differs while its base register holds no page on every path,
# which is also what a changed stack or struct offset looks like: a real difference.


SAME_NAME = re.compile(r"expected to see 0x[0-9a-f]+ .*\((\S+)\)\n--> decomp source code is referencing 0x[0-9a-f]+ .*\((\S+)\)")


def proof_gap(category, reason):
    """True when the checker could not prove a reference, as opposed to finding a difference."""
    if category in PROOF_GAPS:
        return True
    # A file-local constant from a header (`static const`) has one copy per file
    # that uses it, all with the same name. The checker looks data up by name,
    # so it can compare against another file's copy of the same symbol.
    m = SAME_NAME.search(reason) if "wrong data reference" in reason else None
    return bool(m and m.group(1) == m.group(2))


def save_failure(file, text):
    """Keep the failure where the next land cannot overwrite it."""
    FAILED.write_text(text)
    own = FAILED.with_name("land-failed-" + re.sub(r"[^A-Za-z0-9]+", "-", file).strip("-") + ".txt")
    own.write_text(text)
    return own


def run(*cmd, cwd=MAIN):
    p = subprocess.run(cmd, cwd=cwd, capture_output=True, text=True)
    return p.returncode, p.stdout + p.stderr


def inventory():
    with FUNCTIONS.open(newline="") as f:
        return list(csv.DictReader(f))


def owners():
    with (MAIN / f"data/{VERSION}/function_files.csv").open(newline="") as f:
        return {r["Address"]: r["File"] for r in csv.DictReader(f)}


def write_quality(quality):
    """Rewrite only the Quality column, keeping every other byte of the file."""
    lines = FUNCTIONS.read_text().split("\n")
    for i, line in enumerate(lines[1:], 1):
        if line:
            address, _, rest = line.split(",", 2)
            lines[i] = f"{address},{quality[address]},{rest}"
    FUNCTIONS.write_text("\n".join(lines))


def object_matches(file):
    """Addresses of this file's functions whose instructions all equal the original."""
    code, out = run("python3", "tools/try_file.py", "src/" + file, "--json")
    try:
        result = json.loads(out[out.index("{"):])
    except ValueError:
        return None, out
    if code or not result.get("compiled"):
        return None, out
    return {r["address"]: r["status"] for r in result["functions"]}, out


def check(files):
    """Build and check. Returns (error or None, new quality map, summary)."""
    rows = inventory()
    before = {r["Address"]: r["Quality"] for r in rows}
    owner = owners()
    code, out = run("python3", "tools/build.py", "--jobs", "4")
    if code:
        errors = [l for l in out.splitlines() if re.search(r" error: |^FAILED:", l)]
        return "the full build fails", before, "\n".join(errors[:80]) + "\n\n" + out[-20000:]
    # L = library code excluded from the project (no source, no DWARF): never checked.
    wanted = [r for r in rows if r["Quality"] != "L"
              and (r["Quality"] != "U" or owner.get(r["Address"]) in files)]
    request, result = MAIN / "build/land-request.json", MAIN / "build/land-result.json"
    request.write_text(json.dumps({"functions": [{"address": r["Address"], "name": r["Name"]} for r in wanted]}))
    result.unlink(missing_ok=True)
    saved = FUNCTIONS.read_bytes()
    code, out = run(str(MAIN / "tools/check"), "--batch", str(request), "--batch-output", str(result))
    FUNCTIONS.write_bytes(saved)  # labels are written below, in one place
    if code or not result.is_file():
        return "tools/check did not run", before, out[-20000:]
    verdict = {r["address"]: (r["status"], proof_debt.category(r["status"], r["reason"]), r["reason"])
               for r in json.loads(result.read_text())["results"]}
    names = {r["Address"]: r["Name"] for r in rows}

    after, broken, gaps, differing = dict(before), [], 0, []
    objects = {}
    for file in files:
        objects[file], log = object_matches(file)
        if objects[file] is None:
            return f"{file} does not compile on its own (tools/try_file.py)", before, log[-20000:]
    for address, (status, category, reason) in verdict.items():
        mine = owner.get(address) in files
        if status == "ok":
            after[address] = "O"
        elif proof_gap(category, reason):
            gaps += 1
            if mine and objects[owner[address]].get(address) == "match":
                after[address] = "O"
        elif before[address] == "O":
            broken.append(f"{names[address]}: {proof_debt.first_line(reason)}")
        if mine and after[address] == "U" and objects[owner[address]].get(address) in ("match", "mismatch"):
            after[address] = "M"
        if mine and after[address] == "M":
            differing.append(f"  M {names[address]}: {proof_debt.first_line(reason) or status}")
    if broken:
        return (f"{len(broken)} functions that matched no longer do", before,
                "\n".join(broken) + "\n\n" + out[-20000:])
    gained = sum(after[a] == "O" and before[a] != "O" for a in after)
    lines = [f"+{gained} functions newly match; {gaps} proof gaps open"]
    for file in files:
        mine = [a for a in after if owner.get(a) == file]
        lines.append(f"{file}: {sum(after[a] == 'O' for a in mine)}/{len(mine)} functions match")
    lines += differing
    return None, after, "\n".join(lines)


def commit(files, summary, extra):
    body = (", ".join(files) if files else "Sync function labels with the checker") + "\n\n" + summary
    if extra:
        body += "\n\n" + extra
    claim.git("add", "-A", "--", "src", "tests", "include", cwd=MAIN)
    # data/ is ignored, but this generated inventory is already tracked.
    claim.git("add", "-u", "--", f"data/{VERSION}/functions.csv", cwd=MAIN)
    if claim.git("diff", "--cached", "--name-only", cwd=MAIN):
        claim.git("commit", "-q", "-m", body, cwd=MAIN)


def land(file, extra, no_commit):
    """Land one file: from its claim worktree if it has one, else from the main checkout."""
    wt = claim.worktree(file)
    merged = wt.is_dir()
    if merged:
        if claim.git("status", "--porcelain", "--", "src", "tests", "include", cwd=MAIN):
            print(f"SKIP    {file}: commit or remove the main checkout's source changes before merging a worktree")
            return False
        if claim.git("status", "--porcelain", cwd=wt):
            claim.git("add", "-A", cwd=wt)
            claim.git("commit", "-q", "-m", f"work: {file}", cwd=wt)
        code, out = run("git", "merge", "--squash", claim.branch(file))
        touched = claim.git("diff", "--cached", "--name-only", cwd=MAIN).splitlines()
        if code or any(t.startswith("data/") for t in touched):
            run("git", "reset", "--hard", "-q", "HEAD")
            print(f"FAILED  {file}: merge conflict, or the branch edits data/  (see {save_failure(file, out)})")
            return False
    elif not (MAIN / "src" / file).is_file():
        print(f"SKIP    {file}: no src/{file} and no claim worktree")
        return False

    saved = FUNCTIONS.read_bytes()
    error, quality, summary = check([file])
    if error:
        FUNCTIONS.write_bytes(saved)
        if merged:
            run("git", "reset", "--hard", "-q", "HEAD")
        print(f"FAILED  {file}: {error}  (see {save_failure(file, summary)})")
        return False
    write_quality(quality)
    print("LANDED  " + summary.replace("\n", "\n        "))
    if no_commit:
        return True
    commit([file], summary, extra)
    if merged:
        claim.remove_worktree(file, force=True)
    with claim.locked() as rows:
        rows.pop(file, None)
    return True


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("files", nargs="*", help="default: every claim marked done")
    ap.add_argument("--trailer", default="", help="text appended to each commit message")
    ap.add_argument("--sync", action="store_true", help="no source change: rebuild, recheck and update labels")
    ap.add_argument("--no-commit", action="store_true", help="leave the result uncommitted")
    mode = ap.add_mutually_exclusive_group()
    mode.add_argument('--recovery-only', action='store_true', help='Opt-in reviewed source recovery; never exact credit')
    mode.add_argument('--recovery-group', action='store_true', help='Opt-in reviewed two-owner recovery; never exact credit')
    ap.add_argument('--recovery-receipts', type=__import__('pathlib').Path)
    ap.add_argument('--review', type=__import__('pathlib').Path)
    ap.add_argument('--review-sha256')
    ap.add_argument('--reuse-reviewed-baseline', action='store_true',
                    help='Single-owner recovery: reverify reviewed baseline modes and freshly build both integration candidate modes')
    for name in ('original-elf', 'checker', 'lua-cache', 'nss'):
        ap.add_argument('--' + name, type=__import__('pathlib').Path)
    ap.add_argument('--seconds', type=int, default=1800)
    args = ap.parse_args()
    if args.reuse_reviewed_baseline and not args.recovery_only:
        ap.error('--reuse-reviewed-baseline requires --recovery-only')
    if (args.recovery_only or args.recovery_group) and (not all((args.recovery_receipts, args.review, args.review_sha256,
            args.original_elf, args.checker, args.lua_cache, args.nss)) or not 1 <= args.seconds <= 86400):
        ap.error('Recovery requires receipts, pinned independent review and all live input paths')

    # one land at a time, whoever starts it (a person, an agent, tools/dashboard.py)
    lockfile = (MAIN / "build/land.lock").open("w")
    fcntl.flock(lockfile, fcntl.LOCK_EX)
    if args.recovery_only or args.recovery_group:
        if claim.git("status", "--porcelain", cwd=MAIN):
            sys.exit("the main checkout has uncommitted changes; commit or remove them first")
        import json
        if args.recovery_group:
            import recovery_group_land as recovery_landing
        else:
            import recovery as recovery_landing
        try:
            print(json.dumps(recovery_landing.land(args, MAIN), sort_keys=True))
        except Exception as error:
            sys.exit('Recovery landing blocked; evidence/worktrees preserved: ' + str(error))
        return
    if args.sync:
        saved = FUNCTIONS.read_bytes()
        error, quality, summary = check([])
        if error:
            FUNCTIONS.write_bytes(saved)
            FAILED.write_text(summary)
            sys.exit(f"FAILED  {error}  (see {FAILED})")
        write_quality(quality)
        print(summary)
        if not args.no_commit:
            commit([], summary, args.trailer)
        return
    files = args.files or [f for f, r in claim.load().items() if r["State"] == "done"]
    if not files:
        sys.exit("no claim is marked done")
    files = [f[4:] if f.startswith("src/") else f for f in files]
    results = [land(f, args.trailer, args.no_commit) for f in files]
    print(f">>> landed {sum(results)} of {len(files)}")
    if not all(results):
        sys.exit(1)


if __name__ == "__main__":
    main()
