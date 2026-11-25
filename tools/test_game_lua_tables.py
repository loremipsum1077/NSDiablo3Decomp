import hashlib
import json
import re
import unittest

import game_lua_tables as glt

LOCATED = re.compile(r"^(swig_(module|type_initial|commands|constants)|_swig[tc]_|_wrap_class_|swig_\w+_(methods|attributes|bases)$)")


def image(version):
    from validation.elf import Elf
    path = glt.ROOT / "data" / version / "main.elf"
    if not path.is_file():
        raise unittest.SkipTest(f"{path} not present")
    return glt.LuaTableImage(glt.ROOT, Elf(path), version)


def strip(source):
    """Drop every table name locate() recovers, as in a version whose data symbols lack them."""
    dropped = {name: address for name, address in source.addr.items() if LOCATED.match(name)}
    for name, address in dropped.items():
        del source.addr[name]
        source.syms.pop(address, None)
    return dropped


class Version262Tests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.source = image("2.6.2")
        cls.text, cls.evidence = glt.generate_lua_tables(cls.source)

    def test_output_and_evidence_match_manifest(self):
        manifest = json.loads((glt.ROOT / "config/local_game_data.json").read_text())
        self.assertEqual(hashlib.sha256(self.text.encode()).hexdigest(), manifest["includes"]["lua_tables.inc"])
        self.assertEqual(hashlib.sha256(self.evidence.encode()).hexdigest(), manifest["lua_evidence_sha256"])

    def test_locate_recovers_the_named_tables(self):
        located = image("2.6.2")
        named = strip(located)
        self.assertEqual(len(named), 40)
        located.locate()
        self.assertEqual({name: located.addr.get(name) for name in named}, named)
        self.assertEqual(located.counts, {"swig_commands": 1116, "swig_constants": 3330})
        self.assertEqual((located.inferred, located.misnamed), ({}, {}))
        self.assertEqual(glt.generate_lua_tables(located), (self.text, self.evidence))


class Version277Tests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.source = image("2.7.7")
        cls.text, cls.evidence = glt.generate_lua_tables(cls.source)

    def test_counts(self):
        self.assertEqual(self.source.counts, {"swig_commands": 1124, "swig_constants": 3372})
        self.assertIn("swig_commands[1124]", self.text)
        self.assertIn("swig_constants[3372]", self.text)

    def test_cast_initial_named_correctly(self):
        # data_symbols.csv had swig_cast_initial at 0x7101129bd0; it is the 12-pointer array at
        # 0x7101129bc8 (swig_type_initial's last pointer, 0x1129808, is at 0x1129bc0).
        self.assertEqual(self.source.misnamed, {})
        self.assertEqual(self.source.addr["swig_cast_initial"], 0x1129bc8)

    def test_new_wrappers_named(self):
        # The 8 command wrappers new in 2.7.7 are named in functions.csv (wave 2), so nothing is
        # inferred any more; before that, the tool named them _wrap_<command> itself.
        self.assertEqual({address: self.source.syms.get(address) for address in (
            0x8c16a0, 0x8c8b00, 0x8c8b20, 0x8db7d0, 0x8db8b0, 0x8db9c0, 0x8dcc80, 0x8ed5b0)}, {
            0x8c16a0: "_wrap_PlayerGrantLargeAmountOfExperience",
            0x8c8b00: "_wrap_ConductorLogActorRefs",
            0x8c8b20: "_wrap_ConductorClearActorRefs",
            0x8db7d0: "_wrap_SwarmRiftClosingTriggered",
            0x8db8b0: "_wrap_WarningCountdownNotification",
            0x8db9c0: "_wrap_UberBossClosingTriggered",
            0x8dcc80: "_wrap_GameTieredRiftClose",
            0x8ed5b0: "_wrap_PowerSendOwnerCheatedDeath",
        })
        self.assertEqual(self.source.inferred, {})
        evidence = json.loads(self.evidence)
        self.assertEqual(evidence["source"], "data/2.7.7/main.elf")
        self.assertNotIn("inferred_wrappers", evidence)

if __name__ == "__main__":
    unittest.main()
