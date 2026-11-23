#!/usr/bin/env python3
"""Ledger of inline helper families: who inlines them, which callers have strict
original matches, and whether src/ has a header and a candidate body.

  python3 tools/helper_ledger.py              # write data/2.6.2/helper_ledger.csv, print the queues
  python3 tools/helper_ledger.py --top 40 --prefix X/   # longer queues, game code only

Only Quality O in functions.csv is strict original-caller evidence. M/m are
unmatched for this ledger. A matching caller establishes evidence in that caller's
compiled context, not correctness of every instantiation or every caller.

The DWARF index groups by (file, line, template-stripped name), merging template
instantiations and sometimes overloads. Function is a representative signature,
not an exact instantiation identity. Caller address lists are deduplicated low
32-bit binary addresses, matching the project function inventory. Fan-in measures
observed inline relationships, not required dependencies or guaranteed unlocks.

Columns
  File, Line, Function        where the DWARF says the helper lives, and its signature
                              (a grouped family; the signature is representative)
  Callers                     distinct functions in the binary it was inlined into
  MatchedCallers              of those, how many have strict Quality O evidence today
  UnmatchedCallers            the rest
  HeaderInSrc                 yes if src/<File> exists
  Bodied                      does src/<File> define a body for it: yes, no (declared only),
                              absent (neither the name nor its class is in the file), or
                              unknown (text heuristic could not tell; it handles operators,
                              destructors, templates, initialiser lists and `= default`)
  Static                      yes if File is a .cpp/.cc/.c (a location heuristic,
                              not linkage or caller-ownership proof)
  SmallestUnmatchedCallerFile the unmatched caller's file with the fewest original bytes
                              (then functions), with SmallestCallerFunctions and
                              SmallestCallerBytes
  *CallerAddresses            semicolon-separated sorted original caller addresses
  EvidenceScope               grouped family; strict caller contexts only

Queues printed at the end (all also derivable from the CSV):
  1. high fan-in families without strict caller evidence (MatchedCallers = 0)
  2. source-file families without strict caller evidence, grouped by file
  3. declared but bodiless helpers     HeaderInSrc = yes, Bodied = no
"""
import argparse
import csv
import pickle
import re
import sys
from collections import defaultdict
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import dwarf_index

ROOT = Path(__file__).resolve().parent.parent
DATA = ROOT / "data/2.6.2"
SRC = ROOT / "src"
LEDGER = DATA / "helper_ledger.csv"
MATCHED = ("O",)
SOURCE_EXT = (".cpp", ".cc", ".c")
COLUMNS = ["File", "Line", "Function", "Callers", "MatchedCallers", "UnmatchedCallers",
           "HeaderInSrc", "Bodied", "Static", "SmallestUnmatchedCallerFile",
           "SmallestCallerFunctions", "SmallestCallerBytes", "CallerAddresses",
           "MatchedCallerAddresses", "UnmatchedCallerAddresses", "EvidenceScope"]


# ---------------------------------------------------------------- inputs

def load_index():
    """build/dwarf/index.pkl. Read directly: dwarf_index.load() rebuilds (and rewrites the
    shared cache) whenever dwarf_index.py is newer than the pickle, which a fresh checkout
    always is."""
    if dwarf_index.CACHE.is_file():
        with dwarf_index.CACHE.open("rb") as f:
            return pickle.load(f)
    return dwarf_index.load()


def low(address):
    return int(address, 16) & 0xFFFFFFFF


def load_functions(data=DATA):
    """quality and file per function (low 32 bits of the address), and per file
    (function count, byte size)."""
    quality, size, file_of = {}, {}, {}
    with (data / "functions.csv").open(newline="") as f:
        for r in csv.DictReader(f):
            a = low(r["Address"])
            quality[a] = r["Quality"]
            size[a] = int(r["Size"])
    with (data / "function_files.csv").open(newline="") as f:
        for r in csv.DictReader(f):
            file_of[low(r["Address"])] = r["File"]
    files = defaultdict(lambda: [0, 0])
    for a, path in file_of.items():
        files[path][0] += 1
        files[path][1] += size.get(a, 0)
    return quality, file_of, dict(files)


def signature(rec):
    scope = dwarf_index.strip_targs("::".join(rec["scope"]))
    name = dwarf_index.strip_targs(rec["name"])
    params = ", ".join(f"{t} {n}".strip() for t, n in rec["params"])
    return f"{rec['ret']} {scope + '::' if scope else ''}{name}({params}){' const' if rec['const'] else ''}"


# ---------------------------------------------------------------- body detection

COMMENT = re.compile(r"//[^\n]*|/\*.*?\*/", re.S)
STRING = re.compile(r'"(?:\\.|[^"\\\n])*"|\'(?:\\.|[^\'\\\n])*\'')
PREPROCESSOR = re.compile(r"^[ \t]*#(?:[^\n\\]|\\\n|\\[^\n])*", re.M)
QUALIFIERS = ("const", "volatile", "noexcept", "override", "final", "throw", "&&", "&")
NOT_BEFORE = {"return", "new", "delete", "throw", "else", "case", "sizeof", "if", "while",
              "for", "switch", "do", "typedef", "using", "friend", "goto", "co_return"}

_text = {}


def clean(text):
    """Source with comments, string/char literals and preprocessor lines blanked."""
    text = COMMENT.sub(lambda m: re.sub(r"[^\n]", " ", m.group()), text)
    text = STRING.sub('""', text)
    return PREPROCESSOR.sub(lambda m: re.sub(r"[^\n]", " ", m.group()), text)


def source_text(path):
    t = _text.get(path)
    if t is None:
        t = _text[path] = clean(path.read_text(errors="replace")) if path.is_file() else ""
    return t


def name_pattern(name):
    """Regex for the helper's unqualified name followed by its parameter list's '('.
    None when the name is not something this heuristic can look for."""
    name = name.strip()
    if not name:
        return None
    if name.startswith("operator"):
        rest = name[8:].strip()
        if not rest:
            return None
        if re.match(r"[A-Za-z_]", rest):  # operator new, operator bool, operator T *
            tokens = re.findall(r"[A-Za-z_]\w*|::|[*&<>,]", rest)
            core = r"operator\s+" + r"\s*".join(re.escape(t) for t in tokens)
        else:  # operator==, operator(), operator[]
            core = r"operator\s*" + r"\s*".join(re.escape(c) for c in rest)
    elif name.startswith("~"):
        core = r"~\s*" + re.escape(name[1:].strip()) + r"\b"
    else:
        core = r"\b" + re.escape(name) + r"\b"
    return re.compile(core + r"(?:\s*<[^(){};<>]*>)?\s*\(")


def balanced(text, i, open_, close):
    """Index just past the bracket that closes text[i] (which is open_), or None."""
    depth = 0
    for j in range(i, len(text)):
        c = text[j]
        if c == open_:
            depth += 1
        elif c == close:
            depth -= 1
            if depth == 0:
                return j + 1
    return None


def skip_ws(text, i):
    while i < len(text) and text[i].isspace():
        i += 1
    return i


def preceding_ok(text, start):
    """False when what comes before the name says 'call site', not 'declaration'."""
    i = start - 1
    while i >= 0 and text[i].isspace():
        i -= 1
    if i < 0:
        return True
    if text[i] in "=(,!?|+-/%[<.":
        return False
    if text[i] == ">" and i > 0 and text[i - 1] == "-":
        return False
    m = re.search(r"([A-Za-z_]\w*)$", text[max(0, i - 15):i + 1])
    return not (m and m.group(1) in NOT_BEFORE)


def classify_match(text, start, paren):
    """'body', 'decl' or 'unknown' for a name match whose '(' is at `paren`."""
    qualified = re.search(r"::\s*$", text[max(0, start - 8):start]) is not None
    end = balanced(text, paren, "(", ")")
    if end is None:
        return "unknown"
    j = skip_ws(text, end)
    while True:
        tail = text[j:j + 9]
        for q in QUALIFIERS:
            if tail.startswith(q) and (not q.isalpha() or not re.match(r"\w", text[j + len(q):j + len(q) + 1])):
                j += len(q)
                j = skip_ws(text, j)
                if text[j:j + 1] == "(":
                    j = balanced(text, j, "(", ")")
                    if j is None:
                        return "unknown"
                    j = skip_ws(text, j)
                break
        else:
            break
    if j >= len(text):
        return "unknown"
    c = text[j]
    if c == "{":
        return "body"
    if c == ";":
        return "unknown" if qualified else "decl"  # `X::f();` is a call, not a declaration
    if c == "=":
        k = skip_ws(text, j + 1)
        if text.startswith(("default", "delete"), k):
            return "body"
        if text[k:k + 1] == "0":
            return "decl"  # pure virtual
        return "unknown"
    if c == ":" and text[j + 1:j + 2] != ":":  # constructor initialiser list
        depth, k = 0, j + 1
        while k < len(text):
            ch = text[k]
            if ch in "([":
                depth += 1
            elif ch in ")]":
                depth -= 1
            elif ch == "{" and depth == 0:
                prev = text[max(0, k - 64):k].rstrip()[-1:]
                if prev in (")", "}"):
                    return "body"
                k = balanced(text, k, "{", "}")  # a brace initialiser: m_x{0}
                if k is None:
                    return "unknown"
                continue
            elif ch == ";" and depth == 0:
                return "unknown"
            k += 1
    return "unknown"


def body_status(text, name, scope=()):
    """yes / no / absent / unknown: does `text` (one source file, cleaned) define a body
    for the helper `name` declared in class `scope[-1]` (or at namespace scope)."""
    pattern = name_pattern(name)
    if pattern is None:
        return "unknown"
    if scope:
        owner = dwarf_index.strip_targs(scope[-1]).split("::")[-1]
        if owner and owner != "(anonymous)" and not re.search(r"\b" + re.escape(owner) + r"\b", text):
            return "absent"
    verdicts, found = set(), False
    for m in pattern.finditer(text):
        found = True
        if not preceding_ok(text, m.start()):
            continue
        verdicts.add(classify_match(text, m.start(), m.end() - 1))
    if "body" in verdicts:
        return "yes"
    if "decl" in verdicts:
        return "no"
    return "unknown" if found else "absent"


# ---------------------------------------------------------------- ledger

def format_addresses(addresses):
    return ";".join(f"0x{a:08x}" for a in sorted(set(addresses)))


def caller_union(rows, column="UnmatchedCallerAddresses"):
    """Distinct observed callers, never a sum of overlapping helper fan-ins.

    Older CSVs omit address evidence and cannot supply this union; refuse to
    fabricate a distinct total from their counts.
    """
    result = set()
    for row in rows:
        if column not in row:
            raise ValueError("regenerate helper ledger to obtain caller address evidence")
        result.update(int(a, 16) for a in row[column].split(";") if a)
    return result


def build(index=None, quality=None, file_of=None, files=None, src=SRC, bodies=True):
    """One dict per inline helper with the CSV columns plus
    'key' (file, line, name) and 'caller_files': [(file, unmatched callers)] sorted by
    the file's (bytes, functions, name). bodies=False skips the Bodied text heuristic
    (the slow part) and reports it as '-'."""
    if index is None:
        index = load_index()
    if quality is None:
        quality, file_of, files = load_functions()
    rows = []
    for key, (callers, _sites) in index["inlined"].items():
        rec = index["funcs"].get(key)
        if rec is None:
            continue
        path, line, name = key
        callers = sorted({a & 0xFFFFFFFF for a in callers})
        matched_addresses = [a for a in callers if quality.get(a) in MATCHED]
        matched = len(matched_addresses)
        unmatched = [a for a in callers if quality.get(a) not in MATCHED]
        per_file = defaultdict(int)
        for a in unmatched:
            per_file[file_of.get(a, "")] += 1
        per_file.pop("", None)
        caller_files = sorted(per_file.items(),
                              key=lambda kv: (files.get(kv[0], (1 << 30, 1 << 30))[1],
                                              files.get(kv[0], (1 << 30, 1 << 30))[0], kv[0]))
        smallest = caller_files[0][0] if caller_files else ""
        sfuncs, sbytes = files.get(smallest, (0, 0)) if smallest else (0, 0)
        src_file = src / path
        in_src = src_file.is_file()
        if not bodies:
            bodied = "-"
        elif in_src:
            bodied = body_status(source_text(src_file), rec["name"], rec["scope"])
        else:
            bodied = "absent"
        rows.append({
            "File": path, "Line": line, "Function": signature(rec),
            "Callers": len(callers), "MatchedCallers": matched, "UnmatchedCallers": len(unmatched),
            "HeaderInSrc": "yes" if in_src else "no", "Bodied": bodied,
            "Static": "yes" if path.endswith(SOURCE_EXT) else "no",
            "SmallestUnmatchedCallerFile": smallest,
            "SmallestCallerFunctions": sfuncs, "SmallestCallerBytes": sbytes,
            "CallerAddresses": format_addresses(callers),
            "MatchedCallerAddresses": format_addresses(matched_addresses),
            "UnmatchedCallerAddresses": format_addresses(unmatched),
            "EvidenceScope": "grouped-family; strict caller contexts only; instantiations unresolved",
            "key": key, "caller_files": caller_files,
        })
    rows.sort(key=lambda r: (-r["UnmatchedCallers"], -r["Callers"], r["File"], r["Line"], r["Function"]))
    return rows


def write(rows, out=LEDGER):
    out.parent.mkdir(parents=True, exist_ok=True)
    with out.open("w", newline="") as f:
        w = csv.DictWriter(f, COLUMNS, extrasaction="ignore", lineterminator="\n")
        w.writeheader()
        w.writerows(rows)


def read(path=LEDGER):
    """Rows of a written ledger (strings), or [] when it has not been generated."""
    if not Path(path).is_file():
        return []
    with Path(path).open(newline="") as f:
        return list(csv.DictReader(f))


def rows_for(stem, path=LEDGER):
    """Ledger rows for the helpers a source file's headers define (next.py --file)."""
    own = {stem + e for e in (".h", ".hpp", ".inl", ".cpp", ".cc", ".c")}
    return [r for r in read(path) if r["File"] in own]


# ---------------------------------------------------------------- queues

def queues(rows, min_callers=100, prefix=""):
    rows = [r for r in rows if r["File"].startswith(prefix)]
    unproven = [r for r in rows if r["MatchedCallers"] == 0 and r["Callers"] >= min_callers]
    static = defaultdict(list)
    for r in rows:
        if r["Static"] == "yes" and r["MatchedCallers"] == 0:
            static[r["File"]].append(r)
    static_files = sorted(static.items(), key=lambda kv: (-len(caller_union(kv[1])), kv[0]))
    bodiless = [r for r in rows if r["HeaderInSrc"] == "yes" and r["Bodied"] == "no"]
    return unproven, static_files, bodiless


def short(sig, width=70):
    return sig if len(sig) <= width else sig[:width - 3] + "..."


def print_summary(rows, top=15, min_callers=100, prefix=""):
    unproven, static_files, bodiless = queues(rows, min_callers, prefix)
    if prefix:
        print(f"(queues restricted to helpers defined under {prefix})")
    evidenced = sum(1 for r in rows if r["MatchedCallers"])
    in_src = sum(1 for r in rows if r["HeaderInSrc"] == "yes")
    print(f"{len(rows)} inline helper families; {evidenced} with strict O caller evidence; "
          f"{in_src} have their file under src/")

    print(f"\n1. families without strict caller evidence with >= {min_callers} callers: {len(unproven)} "
          f"({len(caller_union(unproven))} distinct unmatched callers observed)")
    print(f"{'callers':>7} {'hdr':>3} {'body':>7}  helper  ->  smallest unmatched caller file")
    for r in unproven[:top]:
        print(f"{r['Callers']:7} {r['HeaderInSrc']:>3} {r['Bodied']:>7}  {r['File']}:{r['Line']} "
              f"{short(r['Function'], 60)}\n{'':21}-> {r['SmallestUnmatchedCallerFile']} "
              f"({r['SmallestCallerFunctions']} funcs, {r['SmallestCallerBytes']} B)")

    n_static = sum(len(v) for _, v in static_files)
    print(f"\n2. source-defined families without strict caller evidence (.cpp/.cc/.c): {n_static} in {len(static_files)} files")
    print(f"{'helpers':>7} {'unmatched':>9}  file")
    for path, hs in static_files[:top]:
        print(f"{len(hs):7} {len(caller_union(hs)):9}  {path}"
              f"  (top: {', '.join(dwarf_index.strip_targs(r['key'][2]) for r in hs[:3])})")

    print(f"\n3. declared in src/ but bodiless: {len(bodiless)} "
          f"({sum(1 for r in bodiless if r['MatchedCallers'] == 0)} without strict caller evidence)")
    print(f"{'unmatch':>7} {'matched':>7}  helper")
    for r in bodiless[:top]:
        print(f"{r['UnmatchedCallers']:7} {r['MatchedCallers']:7}  {r['File']}:{r['Line']} {short(r['Function'])}")


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("-o", "--out", type=Path, default=LEDGER)
    ap.add_argument("--top", type=int, default=15, help="rows per queue")
    ap.add_argument("--min-callers", type=int, default=100, help="fan-in for queue 1")
    ap.add_argument("--prefix", default="", help="only queue helpers defined under this path")
    args = ap.parse_args()

    rows = build()
    write(rows, args.out)
    print(f">>> wrote {len(rows)} rows to {args.out}\n")
    print_summary(rows, args.top, args.min_callers, args.prefix)


if __name__ == "__main__":
    main()
