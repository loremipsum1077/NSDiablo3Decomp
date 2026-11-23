"""Ownership fallback regressions, independent of the proprietary NSS and LLVM."""
import contextlib
import csv
import io
from pathlib import Path
import sys
import subprocess
import tempfile
import unittest
from unittest import mock

sys.path.insert(0, str(Path(__file__).resolve().parent))
import gen_function_files as gf


SOURCE = "BlizzardCore/Source/System/Debug/ErrorStack.cpp"
CU_SOURCE = "BlizzardCore/Source/System/Debug/System_Debug.cpp"
COMPILER = "5.0.1"


def body(lo=0xaec0, hi=0xb0c4, file=CU_SOURCE, compiler=COMPILER, **kwargs):
    return dict(low_pc=lo, high_pc=hi, file=file, compiler=compiler,
                declaration=False, **kwargs)


def dump(*entries):
    """Produce llvm-dwarfdump text, keeping DIE nesting and reference syntax."""
    lines = [
        "0x00000000: DW_TAG_compile_unit",
        f'              DW_AT_name ("{CU_SOURCE}")',
        '              DW_AT_comp_dir ("D:/BuildServer/2/work/code/branches/2_6_2")',
        '              DW_AT_producer ("Rynda clang version 1.4.9 (based on LLVM 5.0.1)")',
    ]
    for offset, tag, attrs in entries:
        lines.append(f"0x{offset:08x}:   DW_TAG_{tag}")
        lines.extend(f"                DW_AT_{key} ({value})" for key, value in attrs.items())
    return lines


def emitted_attrs(**overrides):
    return dict(name='"AddErrorToStack"', low_pc="0x0000aec0", high_pc="0x0000b0c4",
                decl_file=f'"D:/BuildServer/2/work/code/branches/2_6_2/{SOURCE}"',
                **overrides)


class FallbackOwnershipTests(unittest.TestCase):
    def test_missing_cu_range_has_exact_body_evidence(self):
        self.assertEqual(gf.fallback_owners(0xaec0, 516, [body()]), {CU_SOURCE: COMPILER})
        self.assertEqual(gf.fallback_owners(0xb0d0, 68, [body(0xb0d0, 0xb114)]),
                         {CU_SOURCE: COMPILER})

    def test_requires_whole_inventory_extent_and_exact_start(self):
        for start, size in ((0xaec0, 515), (0xaec0, 517), (0xaec4, 512),
                            (0xaebc, 520), (0xaec0, 0), (0xaec0, -1)):
            with self.subTest(start=start, size=size):
                self.assertEqual(gf.fallback_owners(start, size, [body()]), {})

    def test_disjoint_and_touching_records_do_not_conflict(self):
        records = [body(), body(0xae00, 0xaec0, "Other.cpp"),
                   body(0xb0c4, 0xb100, "Another.cpp")]
        self.assertEqual(gf.fallback_owners(0xaec0, 516, records), {CU_SOURCE: COMPILER})
        self.assertEqual(gf.fallback_owners(0xb0c4, 4, [body()]), {})

    def test_conflicts_fail_closed_regardless_of_record_order(self):
        conflicts = [body(file="Other.cpp"), body(compiler="4.0.0"),
                     body(0xaec0, 0xb0c8), body(0xaec4, 0xb0c4),
                     body(0xae00, 0xaf00), body(file="")]
        for conflict in conflicts:
            for records in ([body(), conflict], [conflict, body()]):
                with self.subTest(conflict=conflict, records=records):
                    self.assertEqual(gf.fallback_owners(0xaec0, 516, records), {})

    def test_duplicate_agreeing_evidence_does_not_create_ambiguity(self):
        self.assertEqual(gf.fallback_owners(0xaec0, 516, [body(), body()]),
                         {CU_SOURCE: COMPILER})


class SubprogramParsingTests(unittest.TestCase):
    def parsed(self, *entries):
        return gf.parse_subprograms(dump(*entries))

    def test_normalizes_source_and_extracts_producer(self):
        records = self.parsed((0x20, "subprogram", emitted_attrs()))
        self.assertEqual(gf.fallback_owners(0xaec0, 516, records), {CU_SOURCE: COMPILER})
        self.assertEqual(records[0]["decl_file"], SOURCE)

    def test_declaration_and_inline_instance_are_not_emitted_bodies(self):
        for tag, attrs in (("subprogram", emitted_attrs(declaration="true")),
                           ("inlined_subroutine", emitted_attrs())):
            with self.subTest(tag=tag):
                self.assertEqual(self.parsed((0x20, tag, attrs)), [])

    def test_zero_discarded_and_empty_extents_cannot_own_functions(self):
        for lo, hi in ((0, 516), (0xaec0, 0xaec0), (0xaec0, 0xaeb0)):
            attrs = emitted_attrs()
            attrs.update(low_pc=hex(lo), high_pc=hex(hi))
            with self.subTest(lo=lo, hi=hi):
                self.assertEqual(self.parsed((0x20, "subprogram", attrs)), [])

    def test_specification_supplies_file_but_not_body_range(self):
        declaration = dict(name='"AddErrorToStack"', decl_file=f'"{SOURCE}"',
                           declaration="true")
        concrete = dict(specification="0x00000020", low_pc="0xaec0", high_pc="0xb0c4")
        records = self.parsed((0x20, "subprogram", declaration), (0x40, "subprogram", concrete))
        self.assertEqual(gf.fallback_owners(0xaec0, 516, records), {CU_SOURCE: COMPILER})
        # A reference without its own PC is not a second emitted instance.
        records = self.parsed((0x20, "subprogram", emitted_attrs()),
                              (0x40, "subprogram", dict(abstract_origin="0x00000020")))
        self.assertEqual(len(records), 1)

    def test_forward_reference_and_abstract_origin_resolve_same_identity(self):
        concrete = dict(abstract_origin="0x00000040", low_pc="0xaec0", high_pc="0xb0c4")
        origin = dict(name='"AddErrorToStack"', decl_file=f'"{SOURCE}"')
        records = self.parsed((0x20, "subprogram", concrete), (0x40, "subprogram", origin))
        self.assertEqual(gf.fallback_owners(0xaec0, 516, records), {CU_SOURCE: COMPILER})

    def test_missing_declaration_path_does_not_erase_emitting_cu(self):
        incomplete = dict(low_pc="0xaec0", high_pc="0xb0c4", name='"unknown"')
        records = self.parsed((0x20, "subprogram", emitted_attrs()),
                              (0x40, "subprogram", incomplete))
        self.assertEqual(gf.fallback_owners(0xaec0, 516, records), {CU_SOURCE: COMPILER})

    def test_header_declaration_keeps_emitting_cu_owner(self):
        attrs = emitted_attrs()
        attrs["decl_file"] = '"BlizzardCore/Source/System/Debug/ErrorStack.h"'
        self.assertEqual(gf.fallback_owners(0xaec0, 516,
                         self.parsed((0x20, "subprogram", attrs))), {CU_SOURCE: COMPILER})


    def test_noncontiguous_body_is_not_an_exact_function(self):
        lines = dump((0x20, "subprogram", dict(decl_file=f'"{SOURCE}"')))
        lines.extend(["                DW_AT_ranges (0x0)",
                      "                  [0x0000aec0, 0x0000b0c4)",
                      "                  [0x0000c000, 0x0000c004)"])
        self.assertEqual(gf.fallback_owners(0xaec0, 516, gf.parse_subprograms(lines)), {})

    def test_mixed_discarded_and_live_ranges_do_not_prove_single_body(self):
        lines = dump((0x20, "subprogram", {}))
        lines.extend(["                DW_AT_ranges (0x0)",
                      "                  [0x00000000, 0x00000004)",
                      "                  [0x0000aec0, 0x0000b0c4)"])
        self.assertEqual(gf.fallback_owners(0xaec0, 516, gf.parse_subprograms(lines)), {})

    def test_unknown_cu_and_cross_cu_conflicts_stay_unowned(self):
        first = dump((0x20, "subprogram", emitted_attrs()))
        second = dump((0x40, "subprogram", emitted_attrs()))
        second[1] = '              DW_AT_name ("Other.cpp")'
        for lines in (first + second, second + first):
            self.assertEqual(gf.fallback_owners(0xaec0, 516, gf.parse_subprograms(lines)), {})
        second = [line for line in second if "DW_AT_name" not in line]
        self.assertEqual(gf.fallback_owners(0xaec0, 516, gf.parse_subprograms(first + second)), {})


class GeneratorIntegrationTests(unittest.TestCase):
    def run_generator(self, units, records, rows):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            nss, functions, out = (root / name for name in ("input.nss", "functions.csv", "owners.csv"))
            nss.touch()
            with functions.open("w", newline="") as stream:
                writer = csv.writer(stream)
                writer.writerow(["Address", "Quality", "Size", "Name"])
                writer.writerows(rows)
            original = functions.read_bytes()
            with mock.patch.object(gf, "read_units", return_value=units), \
                    mock.patch.object(gf, "read_subprograms", return_value=records) as loader, \
                    mock.patch.object(sys, "argv", ["gen_function_files.py", str(nss),
                                      "--functions", str(functions), "-o", str(out)]), \
                    contextlib.redirect_stdout(io.StringIO()):
                gf.main()
            self.assertEqual(functions.read_bytes(), original)
            with out.open() as stream:
                result = list(csv.DictReader(stream))
            return result, loader.call_count

    @staticmethod
    def unit(name, ranges):
        return dict(name=name, comp_dir="D:/BuildServer/2/work/code/branches/2_6_2",
                    producer="clang (based on LLVM 5.0.1)", ranges=ranges)

    def test_existing_cu_owner_takes_priority_over_body_evidence(self):
        rows = [("0x000000710000aec0", "O", "000516", "name,with,commas"),
                ("0x000000710000b0d0", "U", "000068", "second")]
        owners, _ = self.run_generator([self.unit("Existing.cpp", [(0xaec0, 0xb0c4)])],
                                       [body(), body(0xb0d0, 0xb114)], rows)
        self.assertEqual([r["File"] for r in owners], ["Existing.cpp", CU_SOURCE])
        self.assertEqual([r["Address"] for r in owners], [r[0] for r in rows])
        self.assertEqual([r["Compiler"] for r in owners], [COMPILER, COMPILER])

    def test_fallback_cannot_perturb_existing_neighbour_tiebreak(self):
        # The closer recovered A.cpp must not override the old unambiguous B.cpp.
        units = [self.unit("A.cpp", [(0x200, 0x204)]),
                 self.unit("B.cpp", [(0x200, 0x204), (0x400, 0x404)])]
        rows = [(f"0x000000710000{addr:04x}", "U", "4", f"f{addr}")
                for addr in (0x100, 0x200, 0x300, 0x400)]
        owners, _ = self.run_generator(units, [body(0x100, 0x104, "A.cpp")], rows)
        self.assertEqual([r["File"] for r in owners], ["A.cpp", "B.cpp", "", "B.cpp"])
        reversed_owners, _ = self.run_generator(list(reversed(units)),
                                               [body(0x100, 0x104, "A.cpp")], rows)
        self.assertEqual(owners, reversed_owners)

    def test_no_fallback_scan_when_all_functions_have_cu_owner(self):
        rows = [("0x000000710000aec0", "U", "516", "function")]
        owners, calls = self.run_generator([self.unit("Normal.cpp", [(0xaec0, 0xb0c4)])],
                                           [], rows)
        self.assertEqual(calls, 0)
        self.assertEqual(owners[0]["File"], "Normal.cpp")

    def test_unowned_input_order_and_address_spelling_are_stable(self):
        rows = [("0x710000b0d0", "U", "68", "FUN_0000b0d0"),
                ("0x000000710000aec0", "U", "516", "AddErrorToStack")]
        records = [body(), body(0xb0d0, 0xb114)]
        owners, _ = self.run_generator([], records, rows)
        reversed_owners, _ = self.run_generator([], list(reversed(records)), rows)
        self.assertEqual(owners, reversed_owners)
        self.assertEqual([r["Address"] for r in owners], [r[0] for r in rows])
        self.assertEqual([r["File"] for r in owners], [CU_SOURCE, CU_SOURCE])


class GeneratorFailureTests(unittest.TestCase):
    def test_failed_write_preserves_old_mapping_and_cleans_temporary_file(self):
        with tempfile.TemporaryDirectory() as tmp:
            out = Path(tmp) / "owners.csv"
            out.write_text("old mapping\n")
            with self.assertRaisesRegex(OSError, "disk full"):
                with gf.atomic_output(out) as stream:
                    stream.write("partial mapping\n")
                    raise OSError("disk full")
            self.assertEqual(out.read_text(), "old mapping\n")
            self.assertEqual(list(Path(tmp).iterdir()), [out])

    def test_parse_error_terminates_and_reaps_dump_process(self):
        proc = mock.MagicMock()
        proc.__enter__.return_value = proc
        proc.stdout = io.StringIO("malformed dump")
        with mock.patch.object(gf, "find_dwarfdump", return_value="llvm-dwarfdump"), \
                mock.patch.object(gf.subprocess, "Popen", return_value=proc), \
                mock.patch.object(gf, "parse_subprograms", side_effect=ValueError("malformed range")):
            with self.assertRaisesRegex(ValueError, "malformed range"):
                gf.read_subprograms(Path("input.nss"))
        proc.kill.assert_called_once_with()
        proc.wait.assert_called_once_with()

    def test_nonzero_dwarfdump_cannot_publish_partial_output(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            nss, functions, out = (root / name for name in ("input.nss", "functions.csv", "owners.csv"))
            nss.touch()
            functions.write_text("Address,Quality,Size,Name\n0x710000aec0,U,516,f\n")
            original = b"previous ownership output\n"
            out.write_bytes(original)
            proc = mock.MagicMock()
            proc.__enter__.return_value = proc
            proc.stdout = io.StringIO("\n".join(dump((0x20, "subprogram", emitted_attrs()))))
            proc.wait.return_value = 1
            proc.returncode = 1
            proc.args = ["llvm-dwarfdump"]
            with mock.patch.object(gf, "read_units", return_value=[]), \
                    mock.patch.object(gf, "find_dwarfdump", return_value="llvm-dwarfdump"), \
                    mock.patch.object(gf.subprocess, "Popen", return_value=proc), \
                    mock.patch.object(sys, "argv", ["gen_function_files.py", str(nss),
                                      "--functions", str(functions), "-o", str(out)]):
                with self.assertRaises(subprocess.CalledProcessError):
                    gf.main()
            self.assertEqual(out.read_bytes(), original)
            self.assertEqual(sorted(path.name for path in root.iterdir()),
                             ["functions.csv", "input.nss", "owners.csv"])


if __name__ == "__main__":
    unittest.main()
