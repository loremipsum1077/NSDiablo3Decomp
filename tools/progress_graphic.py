#!/usr/bin/env python3
"""Draw the README progress infographic (docs/progress.svg) and refresh the README's
progress block from the committed inventory.

  python3 tools/progress_graphic.py           write docs/progress.svg and update README.md
  python3 tools/progress_graphic.py --check   exit 1 if either is out of date

Read-only apart from the two outputs. Inputs:
  data/2.6.2/functions.csv        Address,Quality,Size,Name (O = matching)
  data/2.6.2/function_files.csv   owner file per function, for the per-area bars
  docs/remaining_functions.csv    MattersForGame per non-matching function

L (excluded library) rows are left out of every denominator, as in status_report.py.
The output is deterministic (no dates), so --check is stable across machines.
"""
import argparse
import collections
import csv
from html import escape
from pathlib import Path
import sys

ROOT = Path(__file__).resolve().parent.parent
DATA = ROOT / "data/2.6.2"
SVG_OUT = ROOT / "docs/progress.svg"
README = ROOT / "README.md"
START, END = "<!-- progress:start -->", "<!-- progress:end -->"

# (path prefix, label) in display order; anything unmatched goes to "Other".
AREAS = [
    ("X/Client/", "Game client"),
    ("X/Common/", "Shared game logic"),
    ("X/Server/", "Game server"),
    ("X/Sigma/", "Sigma engine"),
    ("Contrib/", "Middleware (DemonWare, Protobuf, Lua…)"),
    ("BlizzardCore/", "BlizzardCore"),
    ("Externals/", "Externals (curl)"),
]

C = {
    "bg": "#0d1117", "panel": "#161b22", "border": "#30363d", "track": "#21262d",
    "text": "#e6edf3", "muted": "#8b949e", "faint": "#6e7681",
    "ok": "#3fb950", "warn": "#d29922", "bad": "#f85149", "info": "#58a6ff",
}
FONT = "-apple-system, BlinkMacSystemFont, 'Segoe UI', Helvetica, Arial, sans-serif"


def load():
    funcs = list(csv.DictReader(open(DATA / "functions.csv", newline="")))
    owner = {r["Address"]: r["File"] for r in csv.DictReader(open(DATA / "function_files.csv", newline=""))}
    remaining = list(csv.DictReader(open(ROOT / "docs/remaining_functions.csv", newline="")))

    s = {"q": collections.Counter(), "qb": collections.Counter()}
    areas = collections.OrderedDict((label, [0, 0, 0, 0]) for _, label in AREAS)
    areas["Other"] = [0, 0, 0, 0]
    for r in funcs:
        q, size = r["Quality"], int(r["Size"])
        s["q"][q] += 1
        s["qb"][q] += size
        if q == "L":
            continue
        path = owner.get(r["Address"], "")
        label = next((lbl for pre, lbl in AREAS if path.startswith(pre)), "Other")
        a = areas[label]
        a[0] += 1
        a[1] += size
        if q == "O":
            a[2] += 1
            a[3] += size
    s["areas"] = [(k, v) for k, v in areas.items() if v[0]]

    counted = [q for q in s["q"] if q != "L"]
    s["total"] = sum(s["q"][q] for q in counted)
    s["total_b"] = sum(s["qb"][q] for q in counted)
    s["match"], s["match_b"] = s["q"]["O"], s["qb"]["O"]
    s["written"] = s["q"]["M"] + s["q"]["m"] + s["q"]["W"]
    s["unwritten"] = s["q"]["U"]
    s["fn_pct"] = 100 * s["match"] / s["total"]
    s["b_pct"] = 100 * s["match_b"] / s["total_b"]

    matters = collections.Counter(r["MattersForGame"] for r in remaining if r["Label"] != "L")
    s["matters"] = matters
    s["remaining"] = sum(matters.values())
    return s


def pct(x):
    # Floor to one decimal so 99.96% never displays as 100.0%.
    return f"{int(x * 10) / 10:.1f}%"


def mb(n):
    return f"{n / 1_000_000:.2f} MB"


class Svg:
    def __init__(self, w, h):
        self.w, self.h, self.parts = w, h, []

    def add(self, s):
        self.parts.append(s)

    def rect(self, x, y, w, h, fill, rx=0, extra=""):
        self.add(f'<rect x="{x:.1f}" y="{y:.1f}" width="{max(w, 0):.1f}" height="{h:.1f}" rx="{rx}" fill="{fill}"{extra}/>')

    def text(self, x, y, s, size=14, fill=None, weight=400, anchor="start", extra=""):
        self.add(f'<text x="{x:.1f}" y="{y:.1f}" font-size="{size}" font-weight="{weight}" '
                 f'fill="{fill or C["text"]}" text-anchor="{anchor}"{extra}>{escape(s)}</text>')

    def bar(self, x, y, w, h, frac, color):
        self.rect(x, y, w, h, C["track"], rx=h / 2)
        if frac > 0:
            self.rect(x, y, max(w * frac, h), h, color, rx=h / 2)

    def render(self, title):
        return (f'<svg xmlns="http://www.w3.org/2000/svg" width="{self.w}" height="{self.h}" '
                f'viewBox="0 0 {self.w} {self.h}" role="img" aria-label="{escape(title)}" '
                f'font-family="{FONT}">\n<title>{escape(title)}</title>\n'
                + "\n".join(self.parts) + "\n</svg>\n")


def draw(s):
    W, P = 880, 32
    area_rows = len(s["areas"])
    H = 480 + 28 + area_rows * 30 + 50
    g = Svg(W, H)
    g.rect(0.5, 0.5, W - 1, H - 1, C["bg"], rx=14, extra=f' stroke="{C["border"]}"')

    # Header
    g.text(P, 44, "DIABLO III 2.6.2 · NINTENDO SWITCH", 12, C["muted"], 600, extra=' letter-spacing="1.5"')
    g.text(P, 76, "Decompilation progress", 28, weight=700)
    pill = "✓ Decompilation complete"
    pw = 236
    g.rect(W - P - pw, 40, pw, 32, "#12261a", rx=16, extra=f' stroke="{C["ok"]}"')
    g.text(W - P - pw / 2, 61, pill, 14, C["ok"], 600, "middle")

    # Two headline cards
    y, cw, ch = 104, (W - 2 * P - 16) / 2, 132
    cards = [
        ("Functions matching", s["fn_pct"], f'{s["match"]:,} of {s["total"]:,} functions compile to identical code'),
        ("Code bytes matching", s["b_pct"], f'{mb(s["match_b"])} of {mb(s["total_b"])} of original machine code'),
    ]
    for i, (label, value, sub) in enumerate(cards):
        x = P + i * (cw + 16)
        g.rect(x, y, cw, ch, C["panel"], rx=10, extra=f' stroke="{C["border"]}"')
        g.text(x + 20, y + 30, label, 14, C["muted"], 600)
        g.text(x + 20, y + 76, pct(value), 42, C["ok"], 700)
        g.bar(x + 20, y + 92, cw - 40, 10, value / 100, C["ok"])
        g.text(x + 20, y + 120, sub, 12, C["muted"])

    # Status of every function (stacked)
    y = 268
    g.text(P, y, "Every function accounted for", 16, weight=600)
    bw, bx, by = W - 2 * P, P, y + 14
    segs = [
        (s["match"], C["ok"], "Matching byte for byte"),
        (s["written"], C["warn"], "Written, not byte-identical"),
        (s["unwritten"], C["info"], "Not written as its own symbol"),
    ]
    g.rect(bx, by, bw, 18, C["track"], rx=9)
    g.add(f'<clipPath id="stack"><rect x="{bx}" y="{by}" width="{bw}" height="18" rx="9"/></clipPath>')
    cx = bx
    for n, color, _ in segs:
        w = bw * n / s["total"]
        if n:
            w = max(w, 3)
        g.rect(cx, by, w, 18, color, extra=' clip-path="url(#stack)"')
        cx += w
    lx = P
    for n, color, label in segs:
        g.rect(lx, by + 34, 10, 10, color, rx=2)
        t = f"{label}  {n:,}"
        g.text(lx + 16, by + 43, t, 12, C["muted"])
        lx += 16 + len(t) * 6.6 + 28

    # Behaviour of the rest
    y = 360
    g.text(P, y, f'The {s["remaining"]:,} non-matching functions: do they change how the game behaves?', 16, weight=600)
    m = s["matters"]
    boxes = [
        (m.get("no", 0), C["ok"], "No known difference", "same behaviour; compiler/codegen gap"),
        (m.get("possibly", 0), C["warn"], "Possibly", "not yet shown to behave the same"),
        (m.get("yes", 0), C["bad"] if m.get("yes", 0) else C["ok"], "Missing code", "functions the game needs but lacks"),
    ]
    bw3 = (W - 2 * P - 32) / 3
    for i, (n, color, label, sub) in enumerate(boxes):
        x = P + i * (bw3 + 16)
        g.rect(x, y + 14, bw3, 74, C["panel"], rx=10, extra=f' stroke="{C["border"]}"')
        g.rect(x, y + 14, 4, 74, color, rx=2)
        g.text(x + 20, y + 50, f"{n:,}", 26, color, 700)
        g.text(x + 20 + len(f"{n:,}") * 16 + 10, y + 46, label, 13, weight=600)
        g.text(x + 20, y + 74, sub, 11, C["muted"])

    # Per-area bars (by bytes)
    y = 480
    g.text(P, y, "By area (share of code bytes matching)", 16, weight=600)
    lw, vw = 290, 190
    bx, bw = P + lw, W - 2 * P - lw - vw
    for i, (label, (n, b, on, ob)) in enumerate(s["areas"]):
        ry = y + 28 + i * 30
        frac = ob / b if b else 0
        g.text(P, ry + 4, label, 13)
        g.bar(bx, ry - 6, bw, 12, frac, C["ok"])
        g.text(bx + bw + 62, ry + 5, pct(100 * frac), 13, weight=600, anchor="end")
        g.text(W - P, ry + 5, f"{on:,}/{n:,} fn", 11, C["muted"], anchor="end")

    # Footer
    fy = H - 22
    excl = s["q"]["L"]
    g.text(P, fy - 16, f"Not counted: {excl:,} excluded library functions (FMOD, SDK runtime). "
           "Some data tables are rebuilt from your own game files at bootstrap.", 11, C["faint"])
    g.text(P, fy, "Generated by tools/progress_graphic.py from data/2.6.2/functions.csv.", 11, C["faint"])
    return g.render(f'Decompilation progress: {pct(s["fn_pct"])} of functions, {pct(s["b_pct"])} of code bytes matching')


def badge(label, value, color):
    q = lambda t: t.replace("-", "--").replace("_", "__").replace(" ", "_").replace("%", "%25")
    return f"https://img.shields.io/badge/{q(label)}-{q(value)}-{color}"


def readme_block(s):
    m = s["matters"]
    return "\n".join([
        START,
        f'![Functions]({badge("functions", pct(s["fn_pct"]), "brightgreen")}) '
        f'![Code bytes]({badge("code bytes", pct(s["b_pct"]), "brightgreen")}) '
        f'![Missing code]({badge("missing game code", str(m.get("yes", 0)), "brightgreen" if not m.get("yes") else "red")}) '
        f'![Status]({badge("status", "decompilation complete", "blue")})',
        "",
        '<p align="center"><img src="docs/progress.svg" alt="Decompilation progress infographic" width="880"></p>',
        "",
        "| | Functions | Original code |",
        "|---|---:|---:|",
        f'| **Matching byte for byte** | {s["match"]:,} ({pct(s["fn_pct"])}) | {s["match_b"]:,} B ({pct(s["b_pct"])}) |',
        f'| Written, not byte-identical | {s["written"]:,} | {s["qb"]["M"] + s["qb"]["m"] + s["qb"]["W"]:,} B |',
        f'| Not written as its own symbol | {s["unwritten"]:,} | {s["qb"]["U"]:,} B |',
        f'| **Total counted** | {s["total"]:,} | {s["total_b"]:,} B |',
        "",
        f'Of the {s["remaining"]:,} functions that do not match, **{m.get("no", 0):,}** have no known behavioural '
        f'difference, **{m.get("possibly", 0):,}** possibly differ, and **{m.get("yes", 0):,}** are missing code the '
        "game needs. Each one is listed with its reason in [remaining_functions.md](docs/remaining_functions.md). "
        f'{s["q"]["L"]:,} excluded library functions (FMOD, SDK runtime) are not counted; see '
        "[library_functions.md](docs/library_functions.md).",
        "",
        "<sub>Regenerate with `python3 tools/progress_graphic.py` after landing functions.</sub>",
        END,
    ])


def updated_readme(text, block):
    if START in text and END in text:
        head, rest = text.split(START, 1)
        return head + block + rest.split(END, 1)[1]
    raise SystemExit(f"README.md has no {START} ... {END} block")


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    ap.add_argument("--check", action="store_true", help="fail if outputs are stale; write nothing")
    args = ap.parse_args()

    s = load()
    svg = draw(s)
    readme = updated_readme(README.read_text(), readme_block(s))
    stale = [p for p, new in ((SVG_OUT, svg), (README, readme))
             if not p.exists() or p.read_text() != new]
    if args.check:
        for p in stale:
            print(f"stale: {p.relative_to(ROOT)} (run python3 tools/progress_graphic.py)")
        return 1 if stale else 0
    for p, new in ((SVG_OUT, svg), (README, readme)):
        if p in stale:
            p.write_text(new)
            print(f"wrote {p.relative_to(ROOT)}")
    print(f'{pct(s["fn_pct"])} functions, {pct(s["b_pct"])} bytes matching')
    return 0


if __name__ == "__main__":
    sys.exit(main())
