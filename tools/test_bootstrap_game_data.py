import json
import struct
import unittest

import bootstrap_game_data as bgd


class FakeImage:
    def __init__(self, base, data):
        self.base, self.data = base, data

    def read_vaddr(self, vaddr, size):
        start = vaddr - self.base
        if start < 0 or start + size > len(self.data):
            return b""
        return self.data[start:start + size]


def layout_of(version, name):
    return next(row for row in bgd.RAW_TABLES[version] if row[0] == name)


class RawTableTests(unittest.TestCase):
    def test_2_6_2_tables_are_unchanged(self):
        self.assertEqual(bgd.RAW_TABLES["2.6.2"], (
            ("account_attribs", 0xe53ed4, 21, "<ii"),
            ("account_wide_attribs", 0xe53f7c, 10, "<ii"),
            ("partition_attribs", 0xe53fcc, 11, "<ii"),
            ("hero_attribs", 0xe54c10, 27, "<ii"),
            ("actor_particles", 0xe42d58, 51, "<iiiI"),
        ))

    def test_2_7_7_account_tables_are_consecutive(self):
        # AccountDefinition_SerializeAttributes passes 23 / 10 / 13 rows; the three arrays are adjacent.
        rows = [layout_of("2.7.7", n) for n in ("account_attribs", "account_wide_attribs", "partition_attribs")]
        self.assertEqual([count for _, _, count, _ in rows], [23, 10, 13])
        for (_, address, count, layout), (_, following, _, _) in zip(rows, rows[1:]):
            self.assertEqual(address + count * struct.calcsize(layout), following)

    def test_rows_are_rendered_in_order(self):
        name, address, count, layout = layout_of("2.7.7", "partition_attribs")
        rows = [(i, 1000 + i) for i in range(count)]
        base = 0xe00000  # covers every 2.7.7 table; only the partition rows are non-zero
        data = bytearray(0x100000)
        packed = b"".join(struct.pack(layout, *row) for row in rows)
        data[address - base:address - base + len(packed)] = packed
        image = FakeImage(base, bytes(data))
        text = bgd.raw_tables(image, "2.7.7")["partition_attribs.inc"]
        self.assertEqual(text.splitlines(), [f"    {{ {a}, {b} }}," for a, b in rows])

    def test_short_read_is_refused(self):
        with self.assertRaises(ValueError):
            bgd.raw_tables(FakeImage(0, b""), "2.7.7")


class VersionManifestTests(unittest.TestCase):
    def test_2_7_7_tables_match_manifest(self):
        from validation.elf import Elf
        path = bgd.ROOT / "data/2.7.7/main.elf"
        if not path.is_file():
            raise unittest.SkipTest(f"{path} not present")
        manifest = json.loads((bgd.ROOT / "config/local_game_data.json").read_text())
        expected = manifest["versions"]["2.7.7"]
        tables, evidence = bgd.version_tables(Elf(path), "2.7.7")
        self.assertEqual(tables.keys(), expected["includes"].keys())
        bgd.verify(tables, evidence, expected, "2.7.7")
        self.assertEqual(len(tables["account_attribs.inc"].splitlines()), 23)
        self.assertEqual(len(tables["partition_attribs.inc"].splitlines()), 13)

    def test_verify_rejects_a_changed_table(self):
        expected = {"includes": {"a.inc": bgd.digest(b"x")}, "lua_evidence_sha256": bgd.digest(b"e")}
        bgd.verify({"a.inc": "x"}, "e", expected, "test")
        with self.assertRaises(ValueError):
            bgd.verify({"a.inc": "y"}, "e", expected, "test")
        with self.assertRaises(ValueError):
            bgd.verify({"a.inc": "x"}, "f", expected, "test")


if __name__ == "__main__":
    unittest.main()
