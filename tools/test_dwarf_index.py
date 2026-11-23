"""DWARF cache/output determinism without the proprietary NSS or LLVM tools."""
import contextlib
import copy
import hashlib
import io
import itertools
import json
import os
import pickle
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest
from unittest import mock

sys.path.insert(0, str(Path(__file__).resolve().parent))
import dwarf_index as di
import gen_headers
import inline_rank


FILE = "X/Game/Example.h"
TYPE_KEY = ("Example", FILE, 10)
FUNC_KEY = (FILE, 40, "helper")


def unit(variant):
    """Realistic CUs disagree on template instantiations and declaration detail."""
    dies = {}

    def die(tag, parent=None, **attrs):
        offset = len(dies) + 1
        result = dict(o=offset, t=tag, k=[], p=parent, **attrs)
        dies[offset] = result
        if parent is not None:
            parent["k"].append(result)
        return result

    cu = die("compile_unit", name=f'"unit{variant}.cpp"')
    base = die("base_type", cu, name='"int"')
    integer = f'0x{base["o"]:x} "int"'
    struct = die("structure_type", cu, name='"Example"', decl_file=f'"{FILE}"',
                 decl_line="10", byte_size="0x10" if variant == 2 else "0x8")
    if variant != 0:
        # Layout order is semantic even when members share their source line.
        for name, offset in (("z_first", 0), ("a_second", 8 if variant == 2 else 4)):
            die("member", struct, name=f'"{name}"', type=integer,
                decl_line="11", data_member_location=hex(offset))
    for name in ("common", f"method{variant}"):
        method = die("subprogram", struct, name=f'"{name}"',
                     type=integer if variant != 2 else f'0x{base["o"]:x} "long"',
                     decl_line="20", linkage_name=f'"_ZN7Example{name}E"')
        die("formal_parameter", method, type=integer)
    function = die("subprogram", cu, name=f'"helper<{("int", "long", "short")[variant]}>"',
                   decl_file=f'"{FILE}"', decl_line="40", type=integer,
                   low_pc=hex(0x1000 + variant * 0x100), external="true",
                   linkage_name=f'"_Zhelper{variant}"')
    die("formal_parameter", function, type=integer,
        **({"name": '"value"'} if variant else {}))
    if variant == 2:
        function["inline"] = "DW_INL_inlined"
    die("inlined_subroutine", function, abstract_origin=f'0x{function["o"]:x}')
    die("variable", cu, name='"global"', type=integer, decl_file=f'"{FILE}"',
        decl_line="60", **({"external": "true"} if variant else {}))
    return dies


def fixture_parts():
    parts = []
    for variant in range(3):
        index = di.new_index()
        dies = unit(variant)
        di.walk(dies, next(iter(dies.values())), index)
        index["units"].append(f"unit{variant}.cpp")
        # Multiple independent dependencies stress set iteration across processes.
        index["types"][TYPE_KEY]["deps"].update(
            (name, "struct", False) for name in ("Zebra", "Alpha", "Beta"))
        parts.append(index)
    return parts


def merged(parts):
    result = di.new_index()
    for part in parts:
        di.merge(result, copy.deepcopy(part))
    return di.canonicalize(result)


def outputs(index):
    with tempfile.TemporaryDirectory() as tmp:
        root = Path(tmp)
        header_root = root / "headers"
        paths = gen_headers.Generator(index).run(header_root)
        csv_path = root / "helpers.csv"
        with mock.patch.object(di, "load", return_value=index), \
                mock.patch.object(sys, "argv", ["inline_rank.py", "--out", str(csv_path), "--top", "0"]), \
                contextlib.redirect_stdout(io.StringIO()):
            inline_rank.main()
        return ({path: (header_root / path).read_bytes() for path in paths}, csv_path.read_bytes())


def output_digest():
    headers, ranking = outputs(merged(fixture_parts()))
    payload = json.dumps({path: data.decode() for path, data in headers.items()}, sort_keys=True).encode()
    return hashlib.sha256(payload + ranking).hexdigest()


class DwarfIndexDeterminismTests(unittest.TestCase):
    def test_worker_completion_order_preserves_evidence_and_outputs(self):
        parts = fixture_parts()
        baseline = merged(parts)
        expected_outputs = outputs(baseline)
        for order in itertools.permutations(parts):
            with self.subTest(order=[p["units"] for p in order]):
                result = merged(order)
                self.assertEqual(result, baseline)
                self.assertEqual(outputs(result), expected_outputs)
        record = baseline["types"][TYPE_KEY]
        self.assertEqual([m["name"] for m in record["members"]], ["z_first", "a_second"])
        self.assertIn((record["size"], record["members"][1]["offset"]), ((8, 4), (16, 8)))
        self.assertEqual(len(record["methods"]), 4)
        self.assertEqual(len(record["deps"]), 3)
        self.assertEqual(baseline["funcs"][FUNC_KEY]["addrs"], {0x1000, 0x1100, 0x1200})
        self.assertTrue(baseline["funcs"][FUNC_KEY]["inline"])
        self.assertEqual(baseline["funcs"][FUNC_KEY]["params"], [("int", "value")])
        self.assertEqual(baseline["inlined"][FUNC_KEY], [{0x1000, 0x1100, 0x1200}, 3])
        self.assertEqual(baseline["units"], ["unit0.cpp", "unit1.cpp", "unit2.cpp"])

    def test_cu_order_and_worker_partition_do_not_change_representatives(self):
        expected = merged(fixture_parts())
        for order in itertools.permutations(range(3)):
            index = di.new_index()
            for variant in order:
                dies = unit(variant)
                di.walk(dies, next(iter(dies.values())), index)
                index["units"].append(f"unit{variant}.cpp")
                index["types"][TYPE_KEY]["deps"].update(
                    (name, "struct", False) for name in ("Zebra", "Alpha", "Beta"))
            self.assertEqual(di.canonicalize(index), expected)
            parts = fixture_parts()
            self.assertEqual(merged([merged([parts[order[0]], parts[order[1]]]), parts[order[2]]]), expected)

    def test_consumers_handle_reversed_maps_and_equal_rank_ties(self):
        index = merged(fixture_parts())
        # Same rank, source file and line leave signature as the final CSV key.
        for name in ("zeta", "alpha"):
            key = (FILE, 40, name)
            rec = copy.deepcopy(index["funcs"][FUNC_KEY])
            rec["name"] = name
            rec["linkage"] = "_Z" + name
            index["funcs"][key] = rec
            index["inlined"][key] = [{0x1000}, 1]
        # Case aliases must choose the same on-disk path on every host.
        key = ("Other", "X/game/Example.h", 10)
        rec = copy.deepcopy(index["types"][TYPE_KEY])
        rec.update(name="Other", file=key[1])
        index["types"][key] = rec
        for name in ("z_global", "a_global"):
            key = (FILE, 60, name)
            rec = copy.deepcopy(next(iter(index["vars"].values())))
            rec["name"] = name
            index["vars"][key] = rec
        reversed_index = copy.deepcopy(index)
        for section in ("types", "funcs", "vars", "inlined"):
            reversed_index[section] = dict(reversed(list(reversed_index[section].items())))
        for rec in reversed_index["types"].values():
            rec["methods"] = dict(reversed(list(rec["methods"].items())))
        expected = outputs(index)
        self.assertEqual(outputs(reversed_index), expected)
        ranking = expected[1].decode()
        self.assertLess(ranking.index("alpha("), ranking.index("zeta("))
        header = expected[0][FILE].decode()
        self.assertLess(header.index("z_first"), header.index("a_second"))
        self.assertEqual(len(expected[0]), 1)

    def test_enum_definition_survives_same_key_self_typedef(self):
        # Real C headers encode "typedef enum UnOpr {...} UnOpr" with both
        # DIEs at one source location. An alias must not erase the constants.
        enum = dict(o=1, t="enumeration_type", p=None, k=[], name='"UnOpr"',
                    decl_file=f'"{FILE}"', decl_line="75", byte_size="0x4")
        for offset, (name, value) in enumerate((("OPR_MINUS", "0"), ("OPR_NOT", "1")), 3):
            enum["k"].append(dict(o=offset, t="enumerator", p=enum, k=[],
                                  name=f'"{name}"', const_value=value))
        alias = dict(o=2, t="typedef", p=None, k=[], name='"UnOpr"',
                     decl_file=f'"{FILE}"', decl_line="75", type='0x1 "UnOpr"')
        dies = {1: enum, 2: alias}
        key = ("UnOpr", FILE, 75)
        separate = []
        for die in (enum, alias):
            part = di.new_index()
            di.build_type(dies, die, part)
            separate.append(part)
        candidates = [merged(order) for order in itertools.permutations(separate)]
        for order in itertools.permutations((enum, alias)):
            index = di.new_index()
            for die in order:
                di.build_type(dies, die, index)
            candidates.append(di.canonicalize(index))
        expected = outputs(candidates[0])
        for index in candidates:
            self.assertEqual(index, candidates[0])
            self.assertEqual(index["types"][key]["kind"], "enum")
            self.assertEqual(index["types"][key]["enumerators"],
                             [("OPR_MINUS", "0"), ("OPR_NOT", "1")])
            self.assertEqual(outputs(index), expected)
        header = expected[0][FILE].decode()
        self.assertIn("enum UnOpr", header)
        self.assertIn("OPR_MINUS = 0", header)
        self.assertIn("OPR_NOT = 1", header)
        self.assertNotIn("typedef UnOpr UnOpr", header)

    def test_nested_declarations_union_across_worker_partitions(self):
        parts = fixture_parts()
        nested_keys = []
        for variant, part in enumerate(parts):
            key = (f"Example::Nested{variant}", FILE, 30 + variant)
            nested_keys.append(key)
            nested = copy.deepcopy(part["types"][TYPE_KEY])
            nested.update(name=f"Nested{variant}", scope=("Example",),
                          line=30 + variant, nested=True, members=[], methods={},
                          deps=set(), inner=[], size=1)
            part["types"][key] = nested
            part["types"][TYPE_KEY]["inner"] = [key]
        expected = merged(parts)
        self.assertEqual(set(expected["types"][TYPE_KEY]["inner"]), set(nested_keys))
        self.assertEqual(len(expected["types"][TYPE_KEY]["inner"]), len(nested_keys))
        expected_outputs = outputs(expected)
        for order in itertools.permutations(parts):
            result = merged(order)
            grouped = merged([merged(order[:2]), order[2]])
            self.assertEqual(result, expected)
            self.assertEqual(grouped, expected)
            self.assertEqual(outputs(result), expected_outputs)
        header = expected_outputs[0][FILE].decode()
        for variant in range(3):
            self.assertIn(f"struct Nested{variant}", header)

    def test_canonicalization_is_idempotent(self):
        index = merged(fixture_parts())
        self.assertEqual(di.canonicalize(copy.deepcopy(index)), index)

    def test_outputs_stable_across_python_hash_seeds(self):
        script = "import test_dwarf_index as t; print(t.output_digest())"
        digests = []
        for seed in ("1", "7", "42"):
            env = dict(os.environ, PYTHONHASHSEED=seed)
            result = subprocess.run([sys.executable, "-c", script], cwd=Path(__file__).parent,
                                    env=env, check=True, capture_output=True, text=True)
            digests.append(result.stdout.strip())
        self.assertEqual(len(set(digests)), 1, digests)


class DwarfCacheTests(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.addCleanup(self.tmp.cleanup)
        root = Path(self.tmp.name)
        self.nss = root / "fixture.nss"
        self.nss.write_bytes(b"synthetic input")
        self.cache = root / "index.pkl"
        for name, value in (("NSS", self.nss), ("CACHE", self.cache)):
            patcher = mock.patch.object(di, name, value)
            patcher.start()
            self.addCleanup(patcher.stop)
        self.index = merged(fixture_parts())

    def build(self):
        with mock.patch.object(di, "work", return_value=copy.deepcopy(self.index)):
            return di.build(workers=1)

    def test_valid_cache_is_reused_and_source_change_invalidates_it(self):
        self.assertEqual(self.build(), self.index)
        with mock.patch.object(di, "build") as rebuild:
            self.assertEqual(di.load(), self.index)
            rebuild.assert_not_called()
        self.nss.write_bytes(b"different synthetic input")
        with mock.patch.object(di, "build", return_value=self.index) as rebuild:
            self.assertEqual(di.load(), self.index)
            rebuild.assert_called_once_with()

    def test_legacy_and_truncated_caches_are_rebuilt(self):
        for contents in (pickle.dumps(self.index), b"\x80\x05"):
            self.cache.write_bytes(contents)
            with mock.patch.object(di, "build", return_value=self.index) as rebuild:
                self.assertEqual(di.load(), self.index)
                rebuild.assert_called_once_with()

    def test_interrupted_serialization_preserves_previous_cache(self):
        self.build()
        previous = self.cache.read_bytes()
        with mock.patch.object(di, "pickle") as serializer:
            serializer.HIGHEST_PROTOCOL = pickle.HIGHEST_PROTOCOL
            serializer.dump.side_effect = OSError("simulated disk failure")
            with self.assertRaisesRegex(OSError, "simulated disk failure"):
                self.build()
        self.assertEqual(self.cache.read_bytes(), previous)
        self.assertEqual(list(self.cache.parent.glob("index.pkl.*.tmp")), [])

    def test_failed_dwarfdump_does_not_publish_a_partial_index(self):
        self.build()
        previous = self.cache.read_bytes()
        proc = mock.Mock(stdout=io.StringIO("0x00000000: DW_TAG_compile_unit\n"),
                         returncode=1, args=["llvm-dwarfdump"])
        proc.wait.return_value = 1
        with mock.patch.object(di.subprocess, "Popen", return_value=proc):
            with self.assertRaises(subprocess.CalledProcessError):
                di.build(workers=1)
        self.assertEqual(self.cache.read_bytes(), previous)

    def test_input_changed_during_build_is_not_published(self):
        self.build()
        previous = self.cache.read_bytes()
        with mock.patch.object(di, "cache_identity", side_effect=[{"version": 1}, {"version": 2}]):
            with self.assertRaisesRegex(RuntimeError, "changed while indexing"):
                self.build()
        self.assertEqual(self.cache.read_bytes(), previous)


if __name__ == "__main__":
    unittest.main()
