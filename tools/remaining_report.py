#!/usr/bin/env python3
"""Write docs/remaining_functions.csv and docs/remaining_functions.md: every function
that is not matching (Quality M, W, U) plus the excluded L rows that carry a category in
config/excluded_functions.csv, each with a cause, a one-line reason and whether the
difference matters for the game.

  python3 tools/remaining_report.py

Read-only apart from the two outputs. Inputs:
  data/2.6.2/functions.csv, data/2.6.2/function_files.csv   inventory and owner files
  build/status/equiv.json                                    tier per M row (equiv.py)
  src/<owner>                                                // NON_MATCHING: comments
  build/try/draft/<stem>*.txt                                worker notes
  build/try/triage/unwritten.csv                             U rows
  config/excluded_functions.csv                              categorised L rows
  config/remaining_reasons.csv                               hand-written overrides (by mangled name)

Priority for each field: override file > NON_MATCHING comment in the source > the latest
worker-note line naming the function > the equivalence tier alone.
"""
import collections
import csv
import json
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
DATA = ROOT / "data/2.6.2"
OUT_CSV = ROOT / "docs/remaining_functions.csv"
OUT_MD = ROOT / "docs/remaining_functions.md"
OVERRIDES = ROOT / "config/remaining_reasons.csv"
NOTES = ROOT / "build/try/draft"

CAUSES = {
    "compiler-version": "Lua and LibTomCrypt/LibTomMath were built unoptimised (-O0), and Lua's debug "
        "information names a Nintendo clang 5.0.0 snapshot rather than the 5.0.1 fork used for the rest of the game. "
        "Unoptimised code mirrors the exact compiler build, which we do not have, so these drift by a few "
        "instructions (extra parameter copies and spills) whatever the source says. The source is the upstream "
        "library source. This is the best explanation, not a proven one.",
    "compiler-pass": "One specific optimisation step in the original compiler (a vendor fork of LLVM 5.0.1) "
        "transforms this code differently from ours: for example InstCombine folding a pointer cast into a second "
        "copy of a loop variable in the DemonWare socket-router loops, or GlobalOpt splitting a static. The source "
        "is believed right; the difference is in the compiler.",
    "register-allocation": "Register allocation is the compiler's choice of which CPU register holds each value. "
        "Here the original and ours compute the same thing with the same (or nearly the same) instructions, but "
        "in different registers, sometimes with an extra move or a value spilled to the stack.",
    "stack-layout": "The same values are kept in different stack slots (one shared slot in ours, two in the "
        "original, or a different frame size). The code does the same work.",
    "instruction-scheduling-or-pairing": "The same operations are issued in a different order, or two loads/stores "
        "are combined into one paired instruction (ldp/stp) in one build and not the other.",
    "block-placement-or-tail-merge": "The same blocks of code are laid out in a different order in memory, or the "
        "compiler merged/duplicated a shared tail (such as a common return path) differently.",
    "folding-or-instruction-choice": "The same logic, but the optimiser folds, combines or reuses a comparison or "
        "calculation differently, or picks a different instruction for it: a branch turned into a branchless select "
        "(csel/cset), two tests merged into one, a repeated load or call shared, a byte load instead of a halfword "
        "load, a bit test instead of a compare. The values tested and the results are the same.",
    "loop-transform": "A loop is reshaped differently: rotated, unrolled, unswitched or vectorised in one build "
        "and not the other.",
    "inlining-decision": "The compiler copied a small helper into this function (inlining) where the original "
        "called it, or the reverse. The work done is the same; the code size and call list differ.",
    "tls-checker-limit": "A limit of the comparison tool, not of the code: ours keeps the object-file form of a "
        "thread-local variable access and the original has the form the linker rewrites it to.",
    "header-version": "The difference sits inside code inlined from the shared blz container headers "
        "(vector.h, string.h). Our copy of those headers is evidently a slightly different version from the one "
        "the game was built with (different growth or reload sequences), and changing them affects hundreds of "
        "files, so it was left alone.",
    "lambda-numbering": "The compiler numbers anonymous lambdas ($_0, $_1, ...) per file. Ours gives this lambda a "
        "different number, so the symbol name differs although the code is emitted.",
    "unknown-statement": "The original contains a statement (a store, test or call) whose source we could not "
        "identify from the debug information; by policy nothing was invented, so ours lacks it.",
    "needs-invented-type": "Writing it needs a type whose layout is not recorded anywhere; by policy we do not "
        "invent layouts, so it is left unwritten or approximate.",
    "no-evidence": "Nothing in the debug information or the binary supports writing it.",
    "rough-draft": "A rough first version (often from decompiler output) that was never brought close; the "
        "difference has not been analysed.",
    "unexplained": "Written and examined, but no cause has been identified, or the notes do not say.",
    "sdk-runtime": "Nintendo SDK startup code, not game code. It is excluded because defining it in our build "
        "changes how the program links; the original SDK object provides it.",
    "checker-cannot-identify": "Excluded: a static helper with no debug information whose name is shared with "
        "another file, so the comparison tool cannot tell which is which. The source is present and compiled.",
}

# Causes that by themselves imply no behavioural difference.
BENIGN = {"compiler-pass", "folding-or-instruction-choice", "register-allocation", "stack-layout", "instruction-scheduling-or-pairing",
          "block-placement-or-tail-merge", "loop-transform", "inlining-decision", "tls-checker-limit",
          "lambda-numbering", "compiler-version", "checker-cannot-identify"}

# (cause, regex) checked against the evidence text; the earliest match in the text wins,
# except the "strong" ones which win wherever they appear.
STRONG = [
    ("tls-checker-limit", r"\btls\b|thread-local|tlsdesc"),
    ("needs-invented-type", r"invent\w* (a |the )?(type|layout)|no (type )?layout|uicontextmenu"),
    ("compiler-pass", r"instcombine|globalopt|split static|licm\b|\bgvn\b|simplifycfg|ir probe|codegenprepare|"
                      r"machine ?sink|\bsroa\b|deadargelim|dead-argument|libcallsshrinkwrap"),
    ("unknown-statement", r"unknown statement|unidentified|could not identify|no source (for|statement)|"
                          r"source cause is unknown|statement[^.;]{0,40}(unknown|not identified)"),
    ("header-version", r"blz header|header version|shared header code|push_back growth|inlined blz::|"
                       r"blz/(vector|string)\.h is a different"),
    ("lambda-numbering", r"\$_\d.*(number|id)|lambda (number|id|ordinal)"),
]
WEAK = [
    ("inlining-decision", r"(is|are|gets|was|stays?|be) (not )?inlined|\binlines\b|inlined (here|into)|out of line|outlin|"
                          r"inline cost|inlining (decision|cost|differs)|-rpass|deferr"),
    ("loop-transform", r"unroll|unswitch|latch|loop ?rotate|looprotate|vectori[sz]|loop shape|loop-back"),
    ("block-placement-or-tail-merge", r"\bplace[sd]?\b|placement|tail block|own copy|\blayout\b|\blays? out|opposite sense|tail-?calls?|block before|tail[- ]?merg|tail[- ]?dup|block order|block layout|"
                                       r"branch layout|blocks? (is|are) (laid|ordered)|laid out"),
    ("instruction-scheduling-or-pairing", r"schedul|\bpair(s|ing|ed)?\b|(other|opposite|reverse|different) order|\bldp\b|\bstp\b|interleav|order of the|"
                                           r"store order|load order|hoist|sinks?\b|operand order|commut"),
    ("stack-layout", r"stack slot|frame (0x|size)|stack (layout|frame)"),
    ("register-allocation", r"register|regalloc|\bspill|\b[xwsdq]\d{1,2}\b"),
    ("folding-or-instruction-choice", r"if-conver|\bcsel|\bcset|\bcinc|\bccmp|\bfolds?\b|folded|\bcses?\b|jump-thread|"
                              r"jump thread|re-?tests?|reuses|branchless|\bselect|\bldrh\b|\bldrb\b|\btbnz\b|\bcbnz\b|ldrsw|\bpre\b|"
                              r"widened|narrows|materiali[sz]"),
    ("rough-draft", r"decompiler output|\brough\b|not done|first draft|ghidra"),
]
# Words in the evidence that mean behaviour may differ.
RISKY = re.compile(r"semantic|behaviou?r (may )?differ|wrong (value|constant|store)|\bundef\b|"
                   r"(?<!no )missing (a |the |whole )?(store|call|statement|loop)|\bbug\b", re.I)


def area_of(path):
    p = path.lower()
    if not path:
        return "other"
    if "libtom" in p:
        return "LibTom"
    if "/lua/" in p:
        return "Lua"
    if "demonware" in p:
        return "DemonWare"
    if "protobuf" in p:
        return "protobuf"
    if "curl" in p:
        return "curl"
    if "rocrt" in p or "mainruntime" in p:
        return "SDK runtime"
    if path.startswith("X/"):
        return "game code"
    return "other"


def demangle(names):
    out = {}
    todo = [n for n in names if n.startswith("_Z")]
    if todo:
        res = subprocess.run(["c++filt"], input="\n".join(todo), capture_output=True, text=True, check=True)
        for n, d in zip(todo, res.stdout.splitlines()):
            out[n] = d
    for n in names:
        out.setdefault(n, n)
    return out


def qual_name(dem):
    """Qualified name without the parameter list or return type: 'A::B::f'."""
    dem = dem.replace("(anonymous namespace)::", "{anon}::")
    depth = 0
    for i, ch in enumerate(dem):
        if ch in "<":
            depth += 1
        elif ch == ">":
            depth -= 1
        elif ch == "(" and depth == 0 and not dem[:i].endswith("operator"):
            head = dem[:i]
            break
    else:
        head = dem
    # drop a leading return type ("void blz::list<...>::merge<...>")
    depth, cut = 0, 0
    for i, ch in enumerate(head):
        if ch == "<":
            depth += 1
        elif ch == ">":
            depth -= 1
        elif ch == " " and depth == 0:
            cut = i + 1
    head = head[cut:].replace("{anon}::", "")
    return re.sub(r"<[^<>]*(<[^<>]*(<[^<>]*>[^<>]*)*>[^<>]*)*>", "", head)


def clean(text, n=260):
    text = re.sub(r"\s+", " ", text).strip()
    return text if len(text) <= n else text[: n - 3].rstrip() + "..."


# ------------------------------------------------------------ source comments
_src_cache = {}


def param_words(dem):
    """Type words of the demangled parameter list, for telling overloads apart."""
    i = dem.find("(", dem.find("::") if "::" in dem else 0)
    j = dem.rfind(")")
    if i < 0 or j <= i:
        return set()
    return set(re.findall(r"[A-Za-z_]\w+", dem[i + 1:j])) - {"const", "unsigned", "char", "int", "long", "float",
                                                              "void", "bool", "short", "double", "signed"}


def nargs(text):
    """Number of top-level parameters in '(a, b<c, d>, e)...'."""
    depth, n, seen = 0, 1, False
    for ch in text:
        if ch in "(<[":
            depth += 1
        elif ch in ")>]":
            depth -= 1
            if depth == 0:
                break
        elif ch == "," and depth == 1:
            n += 1
        elif depth == 1 and not ch.isspace():
            seen = True
    return n if seen else 0


def source_comment(owner, qn, dem=""):
    """The NON_MATCHING comment attached to the definition of qn in src/<owner>, or ''.
    With several definitions of the name (overloads), the one whose parameter list best fits dem."""
    path = ROOT / "src" / owner
    if owner not in _src_cache:
        try:
            _src_cache[owner] = path.read_text(errors="replace").splitlines()
        except OSError:
            _src_cache[owner] = []
    lines = _src_cache[owner]
    parts = [p for p in qn.split("::") if p]
    if not parts:
        return ""
    last = parts[-1]
    pats = []
    if len(parts) >= 2:
        pats.append(re.compile(r"(^|[\s*&])" + re.escape(parts[-2]) + r"\s*::\s*" + re.escape(last) + r"\s*(<[^()]*>)?\s*\("))
    pats.append(re.compile(r"^(?!\s*(return|if|while|for|else|case|delete|new)\b)\s*(?:[\w:<>,*&]+\s+)+[*&]*" + re.escape(last) + r"\s*(<[^()]*>)?\s*\("))
    want = param_words(dem)
    for pat in pats:
        cands = []
        for i, line in enumerate(lines):
            if not pat.search(line) or line.rstrip().endswith(";"):
                continue
            if pat is pats[-1] and len(parts) >= 2 and "::" in line:
                continue  # another class's member of the same name
            sig = " ".join(lines[i:i + 25])
            sig = sig[:sig.find(")") + 1] if ")" in sig else sig
            have = set(re.findall(r"[A-Za-z_]\w+", sig))
            score = len(want & have) - 2 * abs(nargs(sig[sig.find("("):]) - nargs(dem[dem.find("(", len(dem) - len(dem.lstrip())):]))
            cands.append((score, i))
        if cands:
            best = max(cands)[0]
            for score, i in cands:
                if score == best:
                    got = _comment_above(lines, i)
                    if got:
                        return got
            return ""  # found the definition; it carries no comment
    return ""


def _comment_above(lines, i):
    m = re.search(r"NON_MATCHING:?\s*(.*)$", lines[i])
    if m and "//" in lines[i]:
        return m.group(1)
    j = i - 1
    # skip return-type / template / #line / blank lines between the comment and the name
    hops = 0
    while j >= 0 and hops < 4:
        s = lines[j].strip()
        if s == "" or s.startswith("template") or s.startswith("#line") or (
                s and not s.startswith("//") and not s.startswith("/*") and not s.endswith((";", "}", "{"))
                and not s.startswith("*")):
            j -= 1
            hops += 1
            continue
        break
    block = []
    while j >= 0:
        s = lines[j].strip()
        if s.startswith("//") or s.startswith("/*") or s.startswith("*"):
            block.insert(0, s)
            j -= 1
        else:
            break
    text = " ".join(block)
    k = text.find("NON_MATCHING")
    if k < 0:
        return ""
    text = text[k + len("NON_MATCHING"):].lstrip(": ")
    text = re.sub(r"\s*(//|/\*|\*/|^\*)\s*", " ", text)
    return text.strip()


# ------------------------------------------------------------ worker notes
_note_cache = {}


def note_lines(owner):
    stem = Path(owner).stem
    if stem not in _note_cache:
        out = []
        for p in sorted(NOTES.glob(stem + ".txt")) + sorted(NOTES.glob(stem + ".*.txt")):
            out += p.read_text(errors="replace").splitlines()
        _note_cache[stem] = out
    return _note_cache[stem]


def note_for(owner, qn):
    """Latest note line naming the function (file order = time order)."""
    lines = note_lines(owner)
    parts = [p for p in qn.split("::") if p]
    if not parts or not lines:
        return ""
    last = parts[-1]
    keys = []
    if len(parts) >= 2:
        keys.append(parts[-2] + "::" + last)
    if len(last) >= 6 or len(parts) == 1:
        keys.append(last)
    for key in keys:
        rx = re.compile(r"(?<![\w:])" + re.escape(key) + r"(?!\w)")
        hits = [i for i, ln in enumerate(lines) if rx.search(ln)]
        if hits:
            i = hits[-1]
            text = lines[i]
            # pull in an indented continuation line
            if i + 1 < len(lines) and lines[i + 1].startswith((" ", "\t")):
                text += " " + lines[i + 1]
            return text.strip()
    return ""


def classify(text):
    t = text.lower()
    for cause, rx in STRONG:
        if re.search(rx, t):
            return cause
    best = None
    for cause, rx in WEAK:
        m = re.search(rx, t)
        if m and (best is None or m.start() < best[0]):
            best = (m.start(), cause)
    return best[1] if best else None


TIER_WORDS = {
    "regalloc": "regalloc tier (same instructions, different registers)",
    "equivalent": "equivalent tier (same calls in the same order and nearly the same instructions)",
    "close": "close tier (similar size and instructions, real differences remain)",
    "draft": "draft tier (rough; far from the original)",
}


def reason_for(cause, er, evidence_src):
    size = ""
    if er:
        dw = er["differing_words"]
        size = f" ({er['orig_size']} vs our {er['our_size']} bytes" + (
            (f", {dw} instruction word{' differs' if dw == 1 else 's differ'})") if er["orig_size"] == er["our_size"] else ")")
        cm, ce = len(er.get("calls_missing") or []), len(er.get("calls_extra") or [])
        if cm or ce:
            size += f"; call list differs ({cm} missing, {ce} extra)"
    base = {
        "compiler-version": "Upstream library source; the original compiler build cannot be reproduced",
        "compiler-pass": "An optimisation step in the original compiler transforms this code differently",
        "register-allocation": "Same work, different register choices",
        "stack-layout": "Same work, different stack-slot or frame layout",
        "instruction-scheduling-or-pairing": "Same operations in a different order or pairing",
        "block-placement-or-tail-merge": "Same code blocks laid out or merged differently",
        "folding-or-instruction-choice": "The same logic, but the optimiser folds, combines or reuses a comparison or "
        "calculation differently, or picks a different instruction for it: a branch turned into a branchless select "
        "(csel/cset), two tests merged into one, a repeated load or call shared, a byte load instead of a halfword "
        "load, a bit test instead of a compare. The values tested and the results are the same.",
    "loop-transform": "A loop is reshaped differently by the optimiser",
        "folding-or-instruction-choice": "Same logic; a test or calculation is folded or encoded with different instructions",
        "inlining-decision": "A helper is inlined in one build and called in the other",
        "tls-checker-limit": "Thread-local access form differs; a comparison-tool limit",
        "header-version": "Differs inside inlined code from the shared blz container headers, whose version differs",
        "lambda-numbering": "Lambda numbered differently, so the symbol name differs",
        "unknown-statement": "The original has a statement we could not identify",
        "needs-invented-type": "Needs a type layout that is not recorded",
        "rough-draft": "Rough draft, not analysed",
        "unexplained": "Differs for a reason not yet identified",
    }[cause]
    if evidence_src == "tier":
        size += "; cause from the tier only, no note"
    return base + size


def matters_for(row, cause, er, evidence):
    q = row["Quality"]
    if q == "U":
        return "yes"
    if q == "L":
        return "no"
    if cause in ("unknown-statement", "needs-invented-type", "rough-draft", "unexplained") and (
            not er or er["tier"] not in ("regalloc", "equivalent")):
        return "possibly"
    if evidence and RISKY.search(evidence):
        return "possibly"
    if er:
        if er["tier"] in ("regalloc", "equivalent"):
            return "no"
        calls_diff = bool(er.get("calls_missing") or er.get("calls_extra"))
        if cause in BENIGN and (not calls_diff or cause in ("inlining-decision", "compiler-version", "lambda-numbering")):
            return "no"
        return "possibly"
    return "no" if cause in BENIGN else "possibly"


def main():
    inv = list(csv.DictReader((DATA / "functions.csv").open()))
    files = {r["Address"]: r["File"] for r in csv.DictReader((DATA / "function_files.csv").open())}
    excl = {r["Address"]: r for r in csv.DictReader((ROOT / "config/excluded_functions.csv").open())}
    eq = json.loads((ROOT / "build/status/equiv.json").read_text())
    erows = {r["address"]: r for r in eq["rows"]}
    unw = {}
    p = ROOT / "build/try/triage/unwritten.csv"
    if p.exists():
        unw = {r["Address"]: r for r in csv.DictReader(p.open())}
    over = {}
    if OVERRIDES.exists():
        over = {r["Mangled"]: r for r in csv.DictReader(OVERRIDES.open())}

    rows = [r for r in inv if r["Quality"] in ("M", "W", "U") or (r["Quality"] == "L" and r["Address"] in excl)]
    dem = demangle([r["Name"] for r in rows])
    out, origin = [], collections.Counter()
    for r in rows:
        a, name = r["Address"], r["Name"]
        owner = files.get(a, "")
        er = erows.get(a)
        d = dem[name]
        qn = qual_name(d)
        tier = er["tier"] if er else {"W": "n/a (W)", "U": "unwritten", "L": "excluded"}[r["Quality"]]
        cause = reason = notes = mfg = None
        src_txt = note_txt = ""
        if r["Quality"] in ("M", "W"):
            src_txt = source_comment(owner, qn, d)
            note_txt = note_for(owner, qn)
        evidence = src_txt or note_txt
        ev_src = "source comment" if src_txt else ("notes" if note_txt else "tier")
        if r["Quality"] == "L":
            e = excl[a]
            cause = "sdk-runtime" if e["Category"] == "sdk-runtime" else "checker-cannot-identify"
            reason = clean(e["Reason"], 200)
            ev_src = "excluded_functions.csv"
        elif r["Quality"] == "U":
            u = unw.get(a)
            cause = "no-evidence"
            reason = "Not written"
            notes = clean((u or {}).get("Why", "") + " " + (u or {}).get("EarlierNote", ""), 300)
            ev_src = "unwritten.csv" if u else "none"
        else:
            area = area_of(owner)
            if area in ("LibTom", "Lua"):
                cause = "compiler-version"
                if not evidence:
                    ev_src = "area (LibTom/Lua)"
            else:
                cause = classify(evidence) if evidence else None
                if er and er["tier"] == "regalloc" and cause not in (
                        "register-allocation", "stack-layout", "tls-checker-limit", "compiler-pass"):
                    cause = "register-allocation"  # same instruction sequence: nothing else can differ
                if cause is None:
                    if er and er["tier"] == "regalloc":
                        cause = "register-allocation"
                        if not evidence:
                            ev_src = "tier"
                    elif er and er["tier"] == "draft" and not evidence:
                        cause, ev_src = "rough-draft", "tier"
                    else:
                        cause = "unexplained"
                        if not evidence:
                            ev_src = "tier"
            reason = reason_for(cause, er, ev_src)
        if notes is None:
            bits = []
            if evidence:
                bits.append(("src: " if src_txt else "notes: ") + clean(evidence, 260))
            if er and (er.get("calls_missing") or er.get("calls_extra")):
                bits.append(f"calls missing {len(er['calls_missing'])}, extra {len(er['calls_extra'])}")
            notes = "; ".join(bits)
        mfg = matters_for(r, cause, er, evidence)
        o = over.get(name)
        if o:
            cause = o.get("Cause") or cause
            reason = o.get("Reason") or reason
            mfg = o.get("MattersForGame") or mfg
            if o.get("Notes"):
                notes = o["Notes"]
            ev_src = "override"
        origin[ev_src] += 1
        if cause not in CAUSES:
            sys.exit(f"unknown cause {cause!r} for {name}")
        fn = d if len(d) <= 120 else d[:117] + "..."
        out.append({
            "Address": a, "Label": r["Quality"], "File": owner, "Function": fn, "Size": int(r["Size"]),
            "OurSize": er["our_size"] if er else "", "Tier": tier, "Cause": cause, "Reason": reason,
            "MattersForGame": mfg, "Notes": notes, "_area": area_of(owner), "_src": ev_src, "_mangled": name, "_qn": qn,
        })

    cols = ["Address", "Label", "File", "Function", "Size", "OurSize", "Tier", "Cause", "Reason",
            "MattersForGame", "Notes"]
    with OUT_CSV.open("w", newline="") as f:
        w = csv.DictWriter(f, fieldnames=cols, extrasaction="ignore", lineterminator="\n")
        w.writeheader()
        for o in sorted(out, key=lambda o: (o["File"], o["Address"])):
            w.writerow(o)
    write_md(out, eq.get("commit", "?"), origin)
    print(f"{len(out)} rows -> {OUT_CSV.relative_to(ROOT)}, {OUT_MD.relative_to(ROOT)}")
    print("evidence:", dict(origin))
    print("cause:", dict(collections.Counter(o["Cause"] for o in out)))
    print("matters:", dict(collections.Counter(o["MattersForGame"] for o in out)))


def write_md(out, commit, origin):
    head = subprocess.run(["git", "rev-parse", "--short", "HEAD"], cwd=ROOT, capture_output=True,
                          text=True).stdout.strip()
    lab = collections.Counter(o["Label"] for o in out)
    mfg = collections.Counter(o["MattersForGame"] for o in out)
    tot_b = sum(o["Size"] for o in out)
    L = []
    L.append("# Remaining functions\n")
    L.append(f"Every function in the game binary that does not yet match byte for byte, with a short reason "
             f"and whether the difference matters for how the game behaves. Generated by "
             f"`tools/remaining_report.py` at commit `{head}` (comparison data `build/status/equiv.json` from "
             f"`{commit}`). The full list is [remaining_functions.csv](remaining_functions.csv); this page is the "
             f"summary.\n")
    L.append(f"**{len(out)} functions, {tot_b:,} bytes**: {lab['M']} written but not matching (M), "
             f"{lab['W']} written but not matching and outside the comparison report (W), {lab['U']} not written "
             f"(U), and {lab['L']} excluded functions that carry a category (L). Plain excluded library code "
             f"(FMOD, 2,180 functions) is not listed; see [library_functions.md](library_functions.md).\n")
    L.append(f"MattersForGame: **{mfg['no']} no**, **{mfg['possibly']} possibly**, **{mfg['yes']} yes**.\n")
    L.append("Of the 9 unwritten (U) functions, 8 are template or lambda code whose source is written: our build "
             "inlines it into its caller or emits it under a different lambda number, so the game still has the "
             "code; they are marked `no` (or `possibly` where the instance's user is unknown) rather than `yes`. "
             "Only `UI_Input` is genuinely missing.\n")
    L.append("## Terms\n")
    L.append("- **Tier**: how close a written function is, measured automatically by `build/status/equiv.py`. "
             "*regalloc*: the same instructions, only the registers differ. *equivalent*: the same calls in the "
             "same order and nearly the same instructions, so it behaves the same by a heuristic. *close*: "
             "similar, with real differences left. *draft*: rough. **The tiers are a heuristic, not a proof**, so "
             "`no` below means \"no known behavioural difference\", not \"proven identical\".")
    L.append("- **Register allocation**: the compiler's choice of which CPU register holds each value.")
    L.append("- **Inlining**: copying a small function's body into its caller instead of calling it.")
    L.append("- **MattersForGame**: `no` = written and, as far as is known, behaves like the original (or the "
             "difference is a limit of the comparison tool); `possibly` = written but not shown to behave the "
             "same (a real operation difference, a missing or extra call, an unidentified statement, or a rough "
             "draft); `yes` = missing code the game needs.")
    L.append("- **Reason source**: where each row's cause came from. " + ", ".join(
        f"{k}: {v}" for k, v in origin.most_common()) + ". \"tier\" means no note named the function and the "
             "cause is read from the tier alone.\n")

    L.append("## What matters\n")
    L.append("Every row marked `yes` or `possibly`, largest first.\n")
    L.append("| Function | File | Bytes | Tier | Matters | Reason |")
    L.append("|---|---|---:|---|---|---|")
    for o in sorted((o for o in out if o["MattersForGame"] != "no"),
                    key=lambda o: (o["MattersForGame"] != "yes", -o["Size"])):
        tl = o["Label"] if o["Label"] != "M" else o["Tier"]
        fn = o["_qn"].replace("{anon}::", "")
        fn = fn if len(fn) <= 60 else "..." + fn[-57:]
        L.append(f"| `{fn}` | {o['File']} | {o['Size']:,} | {tl} | {o['MattersForGame']} | "
                 f"{o['Reason'].replace('|', '/')} |")
    L.append("")
    L.append("## By cause\n")
    L.append("| Cause | Functions | Bytes | no | possibly | yes |")
    L.append("|---|---:|---:|---:|---:|---:|")
    bc = collections.defaultdict(list)
    for o in out:
        bc[o["Cause"]].append(o)
    for c in sorted(bc, key=lambda c: -len(bc[c])):
        g = bc[c]
        m = collections.Counter(o["MattersForGame"] for o in g)
        L.append(f"| {c} | {len(g)} | {sum(o['Size'] for o in g):,} | {m['no']} | {m['possibly']} | {m['yes']} |")
    L.append("")
    for c in sorted(bc, key=lambda c: -len(bc[c])):
        L.append(f"**{c}.** {CAUSES[c]}\n")

    L.append("## By area\n")
    L.append("Game code is everything under `X/`. *Other* is Blizzard engine and container code "
             "(BlizzardCore, blz, Domino collision).\n")
    L.append("| Area | Functions | Bytes | no | possibly | yes |")
    L.append("|---|---:|---:|---:|---:|---:|")
    ba = collections.defaultdict(list)
    for o in out:
        ba[o["_area"]].append(o)
    for a in ["game code", "DemonWare", "LibTom", "Lua", "protobuf", "curl", "SDK runtime", "other"]:
        g = ba.get(a, [])
        if not g:
            continue
        m = collections.Counter(o["MattersForGame"] for o in g)
        L.append(f"| {a} | {len(g)} | {sum(o['Size'] for o in g):,} | {m['no']} | {m['possibly']} | {m['yes']} |")
    L.append("")

    L.append("## By file\n")
    L.append("Each line: function, size, label/tier, MattersForGame, reason. Files with something that "
             "matters (`yes` or `possibly`) come first within each area.\n")
    bf = collections.defaultdict(list)
    for o in out:
        bf[o["File"]].append(o)
    order = ["game code", "other", "DemonWare", "protobuf", "curl", "Lua", "LibTom", "SDK runtime"]
    for a in order:
        fs = [f for f in bf if area_of(f) == a]
        if not fs:
            continue
        L.append(f"### {a}\n")
        rank = {"yes": 0, "possibly": 1, "no": 2}
        fs.sort(key=lambda f: (min(rank[o["MattersForGame"]] for o in bf[f]), f))
        if a == "LibTom" and len(fs) > 20:
            L.append(f"{sum(len(bf[f]) for f in fs)} functions in {len(fs)} files, all cause "
                     f"`compiler-version`; see the CSV for the list.\n")
            continue
        for f in fs:
            L.append(f"#### {f or '(no owner file)'}\n")
            for o in sorted(bf[f], key=lambda o: (rank[o["MattersForGame"]], -o["Size"])):
                tl = o["Label"] if o["Label"] != "M" else o["Tier"]
                L.append(f"- `{o['Function'][:90]}` {o['Size']} B, {tl}, **{o['MattersForGame']}**: {o['Reason']}")
            L.append("")
    OUT_MD.write_text("\n".join(L) + "\n")


if __name__ == "__main__":
    main()
