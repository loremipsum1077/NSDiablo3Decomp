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
        # sno_calls.inc and gbid_rows.inc need the game's SNO data, not just main.elf.
        from_game_data = {"sno_calls.inc", "gbid_rows.inc"}
        self.assertEqual(tables.keys(), expected["includes"].keys() - from_game_data)
        pins = dict(expected, includes={k: v for k, v in expected["includes"].items() if k in tables})
        bgd.verify(tables, evidence, pins, "2.7.7")
        self.assertEqual(len(tables["account_attribs.inc"].splitlines()), 23)
        self.assertEqual(len(tables["partition_attribs.inc"].splitlines()), 13)

    def test_2_6_2_pins_are_the_top_level_manifest(self):
        manifest = json.loads((bgd.ROOT / "config/local_game_data.json").read_text())
        self.assertEqual(bgd.expected_includes(manifest, "2.6.2"), manifest["includes"])
        later = bgd.expected_includes(manifest, "2.7.7")
        self.assertEqual(later.keys(), manifest["includes"].keys())
        for name, sha in manifest["versions"]["2.7.7"]["includes"].items():
            self.assertEqual(later[name], sha)

    def test_inventory_names_unpinned_digests(self):
        expected = {"includes": {"a.inc": bgd.digest(b"x")}}
        bgd.check_inventory({"a.inc": "x"}, expected, "test")
        with self.assertRaisesRegex(ValueError, "not pinned: b.inc " + bgd.digest(b"y")):
            bgd.check_inventory({"a.inc": "x", "b.inc": "y"}, expected, "test")
        with self.assertRaisesRegex(ValueError, "pinned but not generated: a.inc"):
            bgd.check_inventory({}, expected, "test")

    def test_verify_rejects_a_changed_table(self):
        expected = {"includes": {"a.inc": bgd.digest(b"x")}, "lua_evidence_sha256": bgd.digest(b"e")}
        bgd.verify({"a.inc": "x"}, "e", expected, "test")
        with self.assertRaises(ValueError):
            bgd.verify({"a.inc": "y"}, "e", expected, "test")
        with self.assertRaises(ValueError):
            bgd.verify({"a.inc": "x"}, "f", expected, "test")


class VersionSnoTableTests(unittest.TestCase):
    TEMPLATE = bgd.ROOT / "build/game-data-2.6.2/sno_calls.inc"

    def setUp(self):
        for path in (self.TEMPLATE, bgd.ROOT / "data/2.6.2/main.elf", bgd.ROOT / "data/2.7.7/main.elf"):
            if not path.is_file():
                raise unittest.SkipTest(f"{path} not present")

    def test_2_7_7_sno_tables_come_from_its_image(self):
        import global_sno_map as gsm
        tables = bgd.version_sno_tables(self.TEMPLATE.read_text(), "2.7.7")
        self.assertEqual(sorted(tables), ["gbid_rows.inc", "sno_calls.inc"])
        self.assertEqual(tables["sno_calls.inc"].count("sLookupSNO("), len(gsm.lookup_calls("2.7.7")))
        self.assertNotEqual(tables["sno_calls.inc"].count("sLookupSNO("), len(gsm.lookup_calls("2.6.2")))
        self.assertEqual(len(tables["gbid_rows.inc"].splitlines()), len(gsm.gbid_rows("2.7.7")))


class NvnEntryPointTests(unittest.TestCase):
    def test_2_7_7_loader_adds_18_procedures_in_order(self):
        from validation.elf import Elf
        paths = [bgd.ROOT / f"data/{v}/main.elf" for v in ("2.6.2", "2.7.7")]
        if not all(p.is_file() for p in paths):
            raise unittest.SkipTest("main.elf not present")
        old, new = (bgd.nvn_entry_points(Elf(p), v).splitlines() for p, v in zip(paths, ("2.6.2", "2.7.7")))
        self.assertEqual((len(old), len(new)), (498, 516))
        it = iter(new)
        self.assertTrue(all(line in it for line in old))  # 2.6.2 order kept, entries only inserted
        added = [line for line in new if line not in set(old)]
        self.assertEqual(added[:2], ["NVN_ENTRY(QueueBuilderSetQueuePriority)", "NVN_ENTRY(QueueBuilderGetQueuePriority)"])
        self.assertEqual(added[-1], "NVN_ENTRY(EventGetMemoryOffset)")

    def test_loader_decodes_adrp_add_x1(self):
        # adrp x1, 0x1000 at pc 0x0 ; add x1, x1, #0x10 ; name at 0x1010
        code = struct.pack("<II", 0xB0000001, 0x91004021)
        data = bytearray(0x1200)
        data[:8] = code
        data[0x1010:0x1018] = b"nvnTest\0"
        saved = dict(bgd.NVN_LOADERS)
        bgd.NVN_LOADERS["test"] = (0, 8, 1)
        try:
            self.assertEqual(bgd.nvn_entry_points(FakeImage(0, bytes(data)), "test"), "NVN_ENTRY(Test)\n")
        finally:
            bgd.NVN_LOADERS.clear()
            bgd.NVN_LOADERS.update(saved)


if __name__ == "__main__":
    unittest.main()
