#!/usr/bin/env python3
"""Add DWARF layout checks (sizeof/offsetof static_asserts) to headers in src/.

For each header the DWARF types of the matching original file are looked up
(the same index tools/gen_headers.py uses). Every complete, named, non-template,
top-level struct/class/union that the header actually defines gets
`static_assert(sizeof(T) == 0x.., "T");`, plus one offsetof check per data
member when that is well-formed from namespace scope (no bases, no vtable,
every non-static data member public). C headers get `_Static_assert` in the
style already used under Externals/curl.

The checks go in one block at the end of the file (before a closing include
guard), so no existing line moves: line numbers reach __LINE__ and the line
tables. Templates (the DWARF has only instantiations), incomplete types and
omitted checks are listed in the report and in a `// Not checked:` comment.
Nested types, and DWARF types this header does not define, are left alone.
A type the header defines whose DWARF owner is another file is checked when
the DWARF has exactly one layout for that name.

  python3 tools/layout_asserts.py src/X/Sigma/RGBAColor.h          # show the block
  python3 tools/layout_asserts.py --missing X/Sigma --write        # every header there without checks
  python3 tools/layout_asserts.py --missing X/Sigma --write --report out.json

--omit (default config/layout_assert_omit.txt) names checks to leave out, one
per line: `<header> <type>` (the whole type) or `<header> <type> <field>`. Use it for checks that fail to compile:
a failing check is a real layout or naming difference to fix separately, and
the struct definition must not be bent to pass it.

tools/test_layout_asserts.py fails when a header that defines a struct/class
has no layout checks; justified exceptions are in config/layout_assert_allowlist.txt.
"""
import argparse
import json
import pickle
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
SRC = ROOT / "src"
MARK = "// Layout checks generated from the DWARF by tools/layout_asserts.py."
# C libraries: their headers are compiled as C (zlib.h also from C++, where
# clang accepts _Static_assert too).
C_DIRS = tuple(f"{d}/" for d in ("Externals/curl", "Contrib/Contrib/OggVorbis", "Contrib/Contrib/PCRE",
                                 "Contrib/Contrib/Zlib", "Contrib/Contrib/lua"))
ASSERT_RE = re.compile(r"\b(?:static_assert|_Static_assert|LUA_LAYOUT_ASSERT)\s*\(\s*(?:sizeof|offsetof)\b")
COMMENT_RE = re.compile(r"//[^\n]*|/\*.*?\*/", re.S)
STRING_RE = re.compile(r'"(?:\\.|[^"\\\n])*"')
# `enum class X {` / `enum struct X {` define a scoped enum, not a record.
TYPE_DEF_RE = re.compile(
    r"(?<!\benum\s)\b(?:struct|class|union)\s+(?:__attribute__\s*\(\(.*?\)\)\s*|alignas\s*\([^)]*\)\s*|[A-Z][A-Z0-9_]+\s+)*"
    r"([A-Za-z_]\w*)\s*(?:final\s*)?(?::[^;{}]*)?\{")
ANON_TYPEDEF_RE = re.compile(r"\btypedef\s+(?:struct|union)\s*\{")


def strip_comments(text):
    return COMMENT_RE.sub(lambda m: "\n" * m.group(0).count("\n"), STRING_RE.sub('""', text))


def defined_type_names(text):
    """Names of struct/class/union bodies in a header (comments ignored)."""
    code = strip_comments(text)
    names = set(TYPE_DEF_RE.findall(code))
    if ANON_TYPEDEF_RE.search(code):
        names |= set(re.findall(r"\}\s*([A-Za-z_]\w*)\s*[;,]", code))
    return names


def untagged_typedef_names(text):
    """Names a header gives only to `typedef struct { ... } Name;` bodies. C has no
    `struct Name` for these, even when the DWARF (from a C++ unit) records the
    struct under the typedef's name."""
    code = strip_comments(text)
    names = set()
    for m in ANON_TYPEDEF_RE.finditer(code):
        depth, i = 1, m.end()
        while i < len(code) and depth:  # skip the body, nested braces included
            depth += {"{": 1, "}": -1}.get(code[i], 0)
            i += 1
        closer = re.match(r"\s*([A-Za-z_]\w*)\s*[;,]", code[i:])
        if closer:
            names.add(closer.group(1))
    return names


def defines_types(text):
    code = strip_comments(text)
    return bool(TYPE_DEF_RE.search(code) or ANON_TYPEDEF_RE.search(code))


def has_layout_asserts(text):
    return bool(ASSERT_RE.search(strip_comments(text)))


# ---------------------------------------------------------------- DWARF

def load_index():
    """Read the cached DWARF index without ever rebuilding it (the cache may be
    a read-only link to the main checkout's build/dwarf)."""
    sys.path.insert(0, str(Path(__file__).resolve().parent))
    import dwarf_index
    if not dwarf_index.CACHE.is_file():
        sys.exit(f"{dwarf_index.CACHE} not found: run python3 tools/dwarf_index.py in the main checkout")
    with dwarf_index.CACHE.open("rb") as f:
        index = pickle.load(f)
    if index.pop("_cache_identity", None) != dwarf_index.cache_identity():
        sys.exit(f"{dwarf_index.CACHE} is stale for this NSS/parser: rebuild it in the main checkout")
    return index


class Layouts:
    def __init__(self, index):
        self.types = index["types"]
        self.by_file = {}
        self.by_qual = {}
        for key, rec in self.types.items():
            self.by_file.setdefault((rec["file"] or "").lower(), []).append(key)
            self.by_qual.setdefault(key[0], []).append(key)

    def record_for(self, key):
        """(display record, layout record): a typedef of an anonymous struct
        is checked under the typedef name with the struct's layout."""
        rec = self.types[key]
        if rec["kind"] == "typedef":
            anon = rec["anon"] and self.types.get(rec["anon"])
            if anon and anon["kind"] in ("struct", "class", "union"):
                return rec, anon
            return rec, None
        return rec, rec

    def candidates(self, rel):
        """DWARF records owned by this original file (paths compare without case:
        the originals were built on Windows)."""
        return list(self.by_file.get(rel.lower(), []))

    def unique(self, qualname):
        keys, layouts = [], set()
        for k in self.by_qual.get(qualname, []):
            rec, lay = self.record_for(k)
            if lay is not None and lay["size"] is not None and not lay["template"]:
                keys.append(k)
                layouts.add((lay["size"], tuple((m["name"], m["offset"]) for m in lay["members"])))
        sizes = layouts
        return keys[0] if len(sizes) == 1 else None


def offsetof_ok(rec):
    if rec["kind"] == "union" or rec["bases"]:
        return False
    fields = [m for m in rec["members"] if not m["static"]]
    if any(m["name"].startswith("_vptr$") for m in fields if m["name"]):
        return False
    default = "DW_ACCESS_private" if rec["kind"] == "class" else "DW_ACCESS_public"
    return all((m["access"] or default) == "DW_ACCESS_public" for m in fields)


def plan(rel, text, layouts, c_style, omit={}):
    """(assert lines, skipped [(name, reason)]) for one header."""
    defined = defined_type_names(text)
    untagged = untagged_typedef_names(text)
    seen, items, skipped = set(), [], []
    keys = layouts.candidates(rel)
    owned = {layouts.types[k]["name"] for k in keys}
    # Types the header defines that the DWARF puts in another file.
    for name in sorted(defined - owned):
        k = layouts.unique(name)
        if k is not None:
            keys.append(k)
    for key in sorted(keys, key=lambda k: (layouts.types[k]["line"], k[0])):
        rec, lay = layouts.record_for(key)
        name = rec["name"]
        if rec["kind"] not in ("struct", "class", "union", "typedef") or not name:
            continue
        if rec["kind"] == "typedef" and lay is None:
            continue
        qual = "::".join(rec["scope"] + (name,))
        if qual in seen:
            continue
        if rec["nested"]:
            continue  # checked (or not) with the enclosing type
        base = name.split("<", 1)[0]
        if (rec["template"] or "<" in name) and base in defined:
            if base not in seen:
                seen.add(base)
                skipped.append((base, "template"))
            continue
        if any("anonymous" in s for s in rec["scope"]):
            skipped.append((qual, "anonymous namespace"))
            continue
        if name not in defined:
            continue  # declared elsewhere in src/, or not recovered yet
        seen.add(qual)
        if lay["template"] or "<" in name:
            skipped.append((qual, "template"))
            continue
        if lay["size"] is None:
            skipped.append((qual, "incomplete in the DWARF"))
            continue
        if (rel, qual) in omit:
            skipped.append((qual, "omitted: " + omit[(rel, qual)]))
            continue
        if c_style:
            ref = name if rec["kind"] == "typedef" or name in untagged else f"{rec['kind']} {name}"
        else:
            ref = qual
        sa = "_Static_assert" if c_style else "static_assert"
        items.append(f'{sa}(sizeof({ref}) == 0x{lay["size"]:x}, "{qual}");')
        if offsetof_ok(lay):
            for m in lay["members"]:
                if m["static"] or not m["name"] or m["anon"] or m["bits"] or m["offset"] is None:
                    continue
                if (rel, qual, m["name"]) in omit:
                    skipped.append((f"{qual}::{m['name']}", "omitted: " + omit[(rel, qual, m["name"])]))
                    continue
                msg = f"{qual}.{m['name']}" if c_style else qual
                items.append(f'{sa}(offsetof({ref}, {m["name"]}) == 0x{m["offset"]:x}, "{msg}");')
    return items, skipped


def block(items, skipped, text):
    lines = ["", MARK]
    if any("offsetof(" in i for i in items) and not re.search(r"#\s*include\s*<(?:c?stddef(?:\.h)?)>", text):
        lines.append("#include <stddef.h>")
    if skipped:
        lines.append("// Not checked: " + "; ".join(f"{n} ({r})" for n, r in skipped))
    return lines + items


def include_guard(code_lines):
    directives = [l.strip() for l in code_lines if l.strip().startswith("#")]
    if len(directives) >= 2 and re.match(r"#\s*ifndef\s+(\w+)", directives[0]):
        guard = re.match(r"#\s*ifndef\s+(\w+)", directives[0]).group(1)
        return re.match(rf"#\s*define\s+{guard}\b", directives[1]) is not None and \
            re.match(r"#\s*endif\b", directives[-1]) is not None
    return False


def apply(text, new_lines):
    """Insert the block at the end of the file, inside an include guard."""
    if MARK in text:
        raise ValueError("already has a generated layout block")
    lines = text.split("\n")
    trailing = text.endswith("\n")
    if trailing:
        lines = lines[:-1]
    code_lines = strip_comments(text).split("\n")[:len(lines)]
    at = len(lines)
    if include_guard(code_lines):
        at = max(i for i, l in enumerate(code_lines) if re.match(r"\s*#\s*endif\b", l))
        new_lines = new_lines + [""]
    out = lines[:at] + new_lines + lines[at:]
    return "\n".join(out) + "\n"


def headers_missing(dirs):
    out = []
    for d in dirs:
        for p in sorted((SRC / d).rglob("*.h")):
            text = p.read_text(errors="replace")
            if defines_types(text) and not has_layout_asserts(text):
                out.append(p)
    return out


def read_omit(path):
    """{(header, type[, field]): reason} from lines `header type [field]  # reason`."""
    omit = {}
    if path and Path(path).is_file():
        for line in Path(path).read_text().splitlines():
            code, _, reason = line.partition("#")
            parts = code.split()
            if parts:
                omit[tuple(parts)] = reason.strip() or "check failed"
    return omit


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("headers", nargs="*", type=Path, help="headers under src/")
    ap.add_argument("--missing", nargs="+", metavar="DIR", default=[],
                    help="every header under src/DIR that defines types and has no layout checks")
    ap.add_argument("--c", nargs="*", metavar="HEADER", default=None,
                    help="headers to treat as C (_Static_assert, struct tags); default: the C libraries in C_DIRS")
    ap.add_argument("--omit", type=Path, default=ROOT / "config/layout_assert_omit.txt",
                    help="checks to leave out (see above)")
    ap.add_argument("--write", action="store_true", help="edit the headers in place")
    ap.add_argument("--report", type=Path, help="write a JSON report of checks and skips")
    args = ap.parse_args()

    headers = [h.resolve() for h in args.headers] + headers_missing(args.missing)
    layouts = Layouts(load_index())
    omit = read_omit(args.omit)
    c_set = {(ROOT / c).resolve() for c in args.c} if args.c is not None else None
    report = {}
    for path in headers:
        rel = path.relative_to(SRC).as_posix()
        text = path.read_text()
        c_style = path in c_set if c_set is not None else rel.startswith(C_DIRS)
        items, skipped = plan(rel, text, layouts, c_style, omit)
        report[rel] = {"checks": items, "skipped": skipped}
        if not items:
            print(f"{rel}: nothing to check" + (f" (skipped {len(skipped)})" if skipped else ""))
            continue
        new = block(items, skipped, text)
        if args.write:
            if MARK in text:
                print(f"{rel}: already has a generated block, left alone")
                continue
            path.write_text(apply(text, new))
            print(f"{rel}: {len(items)} checks" + (f", skipped {len(skipped)}" if skipped else ""))
        else:
            print(f"--- {rel}\n" + "\n".join(new))
    if args.report:
        args.report.write_text(json.dumps(report, indent=1, sort_keys=True))


if __name__ == "__main__":
    main()
