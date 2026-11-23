#!/usr/bin/env python3
"""PreToolUse guard: refuse the edits and commands the repo's rules forbid.

The core engineering rules also apply without assistant hooks. Reads the
hook JSON on stdin and answers with a permissionDecision of "deny" (plus the
reason, which the agent sees) when the call would:

  * hand-edit data/2.6.2/functions.csv -- Quality is written by tools/land.py
    only; the rows themselves come from tools/gen_functions_csv.py;
  * hand-edit the other generated data (data_symbols.csv,
    duplicate_names.csv, main.elf, main.uncompressed.nso) -- regenerate with
    tools/setup.py / tools/gen_functions_csv.py;
  * edit anything under tools/common (the nx-decomp-tools submodule);
  * write register pinning (`register ... asm("wN")`, an inline `mov`) --
    match by reshaping the source instead;
  * `git commit --no-verify`, `git push --force` to dev/main, or `git
    checkout/restore` of functions.csv (which silently drops tools/check's
    O results).

Anything else is allowed (exit 0, no output). It is a fence against mistakes
agents make by reflex, not a lock: the maintainer bypasses it by editing
outside Claude Code.
"""
import json
import re
import sys

GENERATED = (
    re.compile(r"/data/2\.6\.2/(data_symbols|duplicate_names|function_files|inline_helpers|helper_ledger)\.csv$"),
    re.compile(r"/data/2\.6\.2/main(\.uncompressed)?\.(elf|nso)$"),
    re.compile(r"/tools/common/"),
)
DATA_TABLES = (
    re.compile(r"/data/2\.6\.2/functions\.csv$"),
)
REGISTER_PIN = re.compile(r'register\s+[^;]*\basm\s*\(\s*"[wxvq]\d+"|asm\s+volatile\s*\(\s*"\s*mov\b')

BASH_RULES = (
    (re.compile(r"\bgit\s+commit\b[^\n;&|]*--no-verify"),
     "--no-verify skips the gate"),
    (re.compile(r"\bgit\s+push\b[^\n;&|]*(--force|-f\b)[^\n;&|]*\b(dev|main)\b"),
     "force-push to dev/main"),
    (re.compile(r"\bgit\s+(checkout|restore)\b[^\n;&|]*functions\.csv"),
     "reverting functions.csv discards tools/check's O results"),
)


def deny(reason):
    print(json.dumps({"hookSpecificOutput": {
        "hookEventName": "PreToolUse",
        "permissionDecision": "deny",
        "permissionDecisionReason": f"guard.py: {reason} (see CONTRIBUTING.md, validation and review)",
    }}))
    sys.exit(0)


def main():
    try:
        data = json.load(sys.stdin)
    except Exception:
        return
    tool = data.get("tool_name", "")
    inp = data.get("tool_input", {}) or {}

    if tool in ("Edit", "Write", "MultiEdit"):
        path = inp.get("file_path", "")
        for rx in GENERATED:
            if rx.search(path):
                deny(f"{path} is generated or vendored; re-run tools/setup.py / tools/gen_functions_csv.py instead")
        for rx in DATA_TABLES:
            if rx.search(path):
                deny(f"{path} is written by tools/land.py and tools/gen_functions_csv.py -- not by hand")
        text = inp.get("new_string", "") or inp.get("content", "")
        for e in inp.get("edits", []) or []:
            text += "\n" + (e.get("new_string", "") or "")
        if REGISTER_PIN.search(text):
            deny("register pinning / inline mov is banned; reshape the source instead")

    elif tool == "Bash":
        cmd = inp.get("command", "") or ""
        for rx, why in BASH_RULES:
            if rx.search(cmd):
                deny(why)


if __name__ == "__main__":
    main()
