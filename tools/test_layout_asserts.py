"""Every header in src/ that defines a struct/class/union checks its layout.

A header without sizeof/offsetof static_asserts lets a wrong layout spread to
every file that includes it. Add checks with tools/layout_asserts.py; list a
justified exception in config/layout_assert_allowlist.txt with its reason.
"""
from pathlib import Path
import sys
import unittest

sys.path.insert(0, str(Path(__file__).resolve().parent))
import layout_asserts as L

ALLOWLIST = L.ROOT / "config/layout_assert_allowlist.txt"


def read_allowlist():
    entries = {}
    for line in ALLOWLIST.read_text().splitlines():
        path, _, reason = line.partition("#")
        if path.strip():
            entries[path.strip()] = reason.strip()
    return entries


class LayoutAssertCoverageTests(unittest.TestCase):
    def test_headers_defining_types_have_layout_asserts(self):
        allowed = read_allowlist()
        missing = []
        for path in sorted(L.SRC.rglob("*.h")):
            rel = path.relative_to(L.SRC).as_posix()
            text = path.read_text(errors="replace")
            if L.defines_types(text) and not L.has_layout_asserts(text) and rel not in allowed:
                missing.append(rel)
        self.assertEqual(missing, [], "headers define types but have no layout static_asserts; run "
                         "tools/layout_asserts.py on them or allowlist them with a reason in "
                         f"{ALLOWLIST.relative_to(L.ROOT)}")

    def test_allowlist_entries_are_current_and_justified(self):
        stale = []
        for rel, reason in read_allowlist().items():
            path = L.SRC / rel
            if not reason:
                stale.append(f"{rel}: no reason given")
            elif not path.is_file():
                stale.append(f"{rel}: no such header")
            else:
                text = path.read_text(errors="replace")
                if not L.defines_types(text) or L.has_layout_asserts(text):
                    stale.append(f"{rel}: no longer needs an exception")
        self.assertEqual(stale, [])


class LayoutAssertToolTests(unittest.TestCase):
    def test_detection_ignores_comments_and_declarations(self):
        self.assertFalse(L.defines_types("struct A;\n// struct B { int x; };\nclass C;\n"))
        self.assertTrue(L.defines_types("class LIBPROTOBUF_EXPORT Tokenizer {\n};\n"))
        self.assertTrue(L.defines_types("typedef struct {\n int x;\n} T;\n"))
        self.assertEqual(L.defined_type_names("struct D : public B { };\ntypedef struct { int a; } E;"),
                         {"D", "E"})
        self.assertFalse(L.defines_types("enum class E : unsigned {\n A,\n};\nenum struct F { B };\n"))
        self.assertTrue(L.defines_types("enum class E { A };\nstruct S { int x; };\n"))
        self.assertFalse(L.has_layout_asserts('// static_assert(sizeof(A) == 4, "A");\n'))
        self.assertTrue(L.has_layout_asserts('_Static_assert(offsetof(struct a, b) == 0, "a.b");\n'))
        self.assertTrue(L.has_layout_asserts('LUA_LAYOUT_ASSERT(sizeof(T) == 0x10, "T");\n'))

    def test_untagged_typedef_is_named_without_struct_keyword(self):
        text = "typedef struct\n{\n    enum { N = 4 } Sizes;\n    char a[N];\n} T;\nstruct U { int x; };\n"
        self.assertEqual(L.untagged_typedef_names(text), {"T"})
        self.assertEqual(L.untagged_typedef_names("typedef struct V { int x; } V;\n"), set())
        self.assertEqual(L.untagged_typedef_names("struct W { int x; } gW;\n"), set())

    def test_block_goes_last_without_moving_lines(self):
        guarded = "#ifndef X_H\n#define X_H\nstruct A { int x; };\n#endif /* X_H */\n"
        out = L.apply(guarded, ["", L.MARK, 'static_assert(sizeof(A) == 0x4, "A");'])
        self.assertEqual(out.split("\n")[:3], guarded.split("\n")[:3])
        self.assertTrue(out.rstrip().endswith("#endif /* X_H */"))
        once = "#pragma once\nstruct A { int x; };\n#include \"A.inl\"\n"
        out = L.apply(once, ["", L.MARK, 'static_assert(sizeof(A) == 0x4, "A");'])
        self.assertTrue(out.startswith(once))
        with self.assertRaises(ValueError):
            L.apply(out, ["", L.MARK])

    def test_offsetof_only_where_well_formed(self):
        field = {"name": "x", "static": False, "access": ""}
        self.assertTrue(L.offsetof_ok({"kind": "struct", "bases": [], "members": [field]}))
        self.assertFalse(L.offsetof_ok({"kind": "class", "bases": [], "members": [field]}))
        self.assertFalse(L.offsetof_ok({"kind": "struct", "bases": [("B", "", False)], "members": [field]}))
        vptr = {"name": "_vptr$A", "static": False, "access": ""}
        self.assertFalse(L.offsetof_ok({"kind": "struct", "bases": [], "members": [vptr, field]}))


if __name__ == "__main__":
    unittest.main()
