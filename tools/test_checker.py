#!/usr/bin/env python3
"""End-to-end trust regressions for the installed full checker.

Run ``python3 tools/test_checker.py`` after setup, or set D3_CHECKER to a newly
built check executable. No game binary, cross compiler or Rust dependencies are
needed: tiny ELF64/AArch64 images are emitted using only Python's stdlib. Each
checker invocation has its own temporary repository, including writable CSVs.
These fixtures exercise real disassembly and symbol resolution, not mocks.
"""
import hashlib
import json
import os
from pathlib import Path
import struct
import subprocess
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]
CHECKER = Path(os.environ.get("D3_CHECKER", ROOT / "tools/check")).resolve()
BASE = 0x7100000000
RET = 0xD65F03C0
NOP = 0xD503201F


def words(*values):
    return struct.pack("<" + "I" * len(values), *values)


def branch(at, target, link=True):
    return (0x94000000 if link else 0x14000000) | (((target - at) // 4) & 0x3FFFFFF)


def adrp(at, target, reg=0):
    """Encode ADRP using page-relative signed imm21."""
    pages = ((target >> 12) - (at >> 12)) & 0x1FFFFF
    return 0x90000000 | ((pages & 3) << 29) | ((pages >> 2) << 5) | reg


def add_immediate(dest, base, immediate, shift=False):
    return 0x91000000 | (int(shift) << 22) | (immediate << 10) | (base << 5) | dest


def cbnz(at, target, reg=2):
    return 0xB5000000 | ((((target - at) // 4) & 0x7FFFF) << 5) | reg


def data_address(at, target):
    """adrp x0, page(target); add x0, x0, lo12(target); ret."""
    pages = ((target >> 12) - (at >> 12)) & 0x1FFFFF
    return words(0x90000000 | ((pages & 3) << 29) | ((pages >> 2) << 5),
                 0x91000000 | ((target & 0xFFF) << 10), RET)


def write_elf(path, functions, objects=()):
    """Minimal linked image: one PT_LOAD; symtab, empty rela.dyn and plt.

    functions: (name, virtual address, bytes); objects: (name, address, size).
    File offsets equal virtual addresses in the load segment. No dynamic
    linking is used by these direct-call/direct-data-reference cases.
    """
    payload = bytearray(0x6000)
    strings = bytearray(b"\0")
    symbols = bytearray(24)  # required undefined symbol
    # Local static copies precede global symbols, as required by ELF symtab.
    functions = sorted(functions, key=lambda entry: not entry[0].startswith("static_"))
    local_count = sum(name.startswith("static_") for name, _, _ in functions)
    for name, addr, code in functions:
        payload[addr:addr + len(code)] = code
        name_offset = len(strings)
        strings.extend(name.encode() + b"\0")
        symbols.extend(struct.pack("<IBBHQQ", name_offset,
                                   0x02 if name.startswith("static_") else 0x12,
                                   0, 1, addr, len(code)))
    for name, addr, size in objects:
        name_offset = len(strings)
        strings.extend(name.encode() + b"\0")
        symbols.extend(struct.pack("<IBBHQQ", name_offset, 0x11, 0, 1, addr, size))
    section_names = b"\0.text\0.symtab\0.strtab\0.shstrtab\0.rela.dyn\0.plt\0"
    sections = [(0, 0, 0, 0, 0, 0, 0, 0, 0, 0),
                (1, 1, 6, 0x1000, 0x1000, 0x4000, 0, 0, 4, 0)]
    for name, typ, data, link, entsize in [
        (".symtab", 2, symbols, 3, 24),
        (".strtab", 3, strings, 0, 0),
        (".shstrtab", 3, section_names, 0, 0),
        (".rela.dyn", 4, b"", 2, 24),
        (".plt", 1, b"", 0, 0),
    ]:
        offset = len(payload)
        payload.extend(data)
        sections.append((section_names.index(name.encode()), typ, 0, 0,
                         offset, len(data), link, local_count + 1 if typ == 2 else 0, 1, entsize))
    shoff = len(payload)
    for section in sections:
        payload.extend(struct.pack("<IIQQQQIIQQ", *section))
    ident = b"\x7fELF" + bytes([2, 1, 1, 0]) + bytes(8)
    payload[:64] = struct.pack("<16sHHIQQQIHHHHHH", ident, 2, 183, 1, 0x1000,
                               64, shoff, 0, 64, 56, 1, 64, len(sections), 4)
    payload[64:120] = struct.pack("<IIQQQQQQ", 1, 7, 0, 0, 0,
                                  len(payload), len(payload), 0x1000)
    path.write_bytes(payload)


def link_map(entries):
    """Emit LLD's column-aligned map layout (including the defining object)."""
    result = [f"{'VMA':>16} {'LMA':>16} {'Size':>8} {'Align':>5} Out     In      Symbol"]
    for name, addr, owner, size in entries:
        prefix = f"{addr:16x} {addr:16x} {size:8x} {4:5d} "
        result.extend([prefix + '        ' + owner + ':(.text)',
                       prefix + '                ' + name])
    return '\n'.join(result) + '\n'


class CheckerTests(unittest.TestCase):
    def setUp(self):
        self.assertTrue(CHECKER.is_file(), f"Build tools/check or set D3_CHECKER: {CHECKER}")
        self.temp = tempfile.TemporaryDirectory(prefix="d3-checker-test-")
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)
        for directory in ("src", "data", "build", "tools"):
            (self.root / directory).mkdir()
        (self.root / 'tools/config.toml').write_text(
            'build_target = "fixture"\nfunctions_csv = "data/functions.csv"\n')

    def check(self, original, rebuilt, rows=None, objects=(), data=(), owners=(), request=None,
              map_binding='valid'):
        write_elf(self.root / 'data/main.elf', original)
        map_text = None
        if owners:
            sizes = {(name, addr): len(code) for name, addr, code in rebuilt}
            map_text = link_map([
                tuple(entry) if len(entry) == 4 else (*entry, sizes.get((entry[0], entry[1]), 4))
                for entry in owners])
        elf_path = self.root / 'build/fixture'
        write_elf(elf_path, rebuilt, objects)
        if rows is None:
            rows = [(name, addr, len(code)) for name, addr, code in original]
        (self.root / 'data/functions.csv').write_text(
            'Address,Quality,Size,Name\n' + ''.join(
                f'0x{BASE + addr:x},O,{size},{name}\n' for name, addr, size in rows))
        (self.root / 'data/data_symbols.csv').write_text(
            'Address,Name\n' + ''.join(f'0x{BASE + addr:x},{name}\n' for name, addr in data))
        map_path = self.root / 'build/fixture.map'
        if map_text is not None:
            map_path.write_text(map_text)
        else:
            map_path.unlink(missing_ok=True)
        provenance = self.root / 'build/fixture.map.provenance'
        provenance.unlink(missing_ok=True)
        if map_text is not None and map_binding != 'missing':
            provenance.write_text(json.dumps({
                'format': 2 if map_binding == 'unsupported-format' else 1,
                'elf_sha256': hashlib.sha256(elf_path.read_bytes()).hexdigest(),
                'map_sha256': hashlib.sha256(map_path.read_bytes()).hexdigest(),
            }))
            if map_binding == 'stale-map':
                map_path.write_text(map_text + '\n# map from a different link\n')
            elif map_binding == 'stale-elf':
                # Change unused bytes, leaving symbols and all checked code
                # identical: generation binding must still reject the map.
                payload = bytearray(elf_path.read_bytes())
                payload[0x5000] ^= 1
                elf_path.write_bytes(payload)
            elif map_binding == 'malformed':
                provenance.write_text('{invalid JSON')
        if request is None:
            request = [{'address': f'0x{BASE + rows[0][1]:x}', 'name': rows[0][0]}]
        (self.root / 'request.json').write_text(json.dumps({'functions': request}))
        run = subprocess.run([str(CHECKER), '--batch', 'request.json', '--batch-output', 'result.json'],
                             cwd=self.root, text=True, capture_output=True, timeout=30)
        self.assertEqual(run.returncode, 0, run.stdout + run.stderr)
        result = json.loads((self.root / 'result.json').read_text())
        self.assertEqual(result['format'], 1)
        self.assertEqual(len(result['results']), len(request))
        return result['results'][0]

    def assert_single_rejected_without_promotion(self, name):
        inventory = self.root / 'data/functions.csv'
        inventory.write_text(inventory.read_text().replace(',O,', ',U,'))
        before = inventory.read_bytes()
        run = subprocess.run([str(CHECKER), name], cwd=self.root, input='',
                             text=True, capture_output=True, timeout=30)
        self.assertNotEqual(run.returncode, 0, run.stdout + run.stderr)
        self.assertEqual(inventory.read_bytes(), before, 'unresolved identity must not promote a row')

    def test_call_and_tail_call_targets(self):
        for link in (True, False):
            for wrong in (False, True):
                with self.subTest(link=link, wrong=wrong):
                    result = self.check(
                        [('caller', 0x1000, words(branch(0x1000, 0x1100, link), RET)),
                         ('right', 0x1100, words(RET)), ('wrong', 0x1200, words(RET))],
                        [('caller', 0x2000, words(branch(0x2000, 0x2400 if wrong else 0x2300, link), RET)),
                         ('right', 0x2300, words(RET)), ('wrong', 0x2400, words(RET))])
                    self.assertEqual(result['status'], 'mismatch' if wrong else 'ok')
                    if wrong:
                        self.assertIn('wrong function call', result['reason'])

    def test_external_conditional_branch_preserves_condition(self):
        # B.cond displacement occupies bits 23:5; bit 0 distinguishes eq/ne.
        original = 0x54000000 | (((0x1100 - 0x1000) // 4) << 5)
        for condition, expected in ((0, 'ok'), (1, 'mismatch')):
            with self.subTest(condition=condition):
                rebuilt = 0x54000000 | (((0x2300 - 0x2000) // 4) << 5) | condition
                result = self.check(
                    [('caller', 0x1000, words(original, RET)), ('target', 0x1100, words(RET))],
                    [('caller', 0x2000, words(rebuilt, RET)), ('target', 0x2300, words(RET))])
                self.assertEqual(result['status'], expected)

    def test_direct_data_target(self):
        for wrong in (False, True):
            with self.subTest(wrong=wrong):
                result = self.check([('reader', 0x1000, data_address(0x1000, 0x3100))],
                                    [('reader', 0x2000, data_address(0x2000, 0x4200 if wrong else 0x4100))],
                                    objects=[('right_data', 0x4100, 8), ('wrong_data', 0x4200, 8)],
                                    data=[('right_data', 0x3100)])
                self.assertEqual(result['status'], 'mismatch' if wrong else 'ok')
                if wrong:
                    self.assertIn('wrong data reference', result['reason'])

    def pair_check(self, original, rebuilt, rebuilt_data=0x4200):
        return self.check(
            [('reader', 0x1000, words(*original)), ('callee', 0x1800, words(RET))],
            [('reader', 0x2000, words(*rebuilt)), ('callee', 0x2800, words(RET))],
            objects=[('value', rebuilt_data, 8)], data=[('value', 0x3100)])

    def test_add_pair_preserves_shift(self):
        for shifted in (False, True):
            with self.subTest(shifted=shifted):
                result = self.pair_check(
                    [adrp(0x1000, 0x3100), add_immediate(1, 0, 0x100), RET],
                    [adrp(0x2000, 0x4200), add_immediate(1, 0, 0x200, shifted), RET])
                self.assertEqual(result['status'], 'mismatch' if shifted else 'ok')

    def test_add_register_cannot_replace_pair_immediate(self):
        result = self.pair_check(
            [adrp(0x1000, 0x3100), add_immediate(1, 0, 0x100), RET],
            [adrp(0x2000, 0x4200), 0x8B020001, RET])  # add x1,x0,x2
        self.assertEqual(result['status'], 'mismatch')

    def test_indexed_memory_pair_preserves_all_addressing_bits(self):
        # ldr/str x1, [x0, x2] with index, scale and extension mutations.
        for opcode in (0xF8626801, 0xF8226801):
            for mutation in (0, 1 << 16, 1 << 12, 1 << 15):
                with self.subTest(opcode=hex(opcode), mutation=hex(mutation)):
                    result = self.pair_check(
                        [adrp(0x1000, 0x3000), opcode, RET],
                        [adrp(0x2000, 0x4000), opcode ^ mutation, RET])
                    self.assertEqual(result['status'], 'mismatch' if mutation else 'ok')

    def test_memory_writeback_modes_are_not_relocation(self):
        # ldr/str x1,[x0,#8]! versus the same access without writeback or
        # post-indexed: same base/displacement does not mean same operation.
        for load in (False, True):
            pre = (0xF8408C01 if load else 0xF8008C01)
            unscaled = pre & ~0xC00
            post = (pre & ~0xC00) | 0x400
            for replacement in (pre, unscaled, post):
                with self.subTest(load=load, replacement=hex(replacement)):
                    result = self.pair_check(
                        [adrp(0x1000, 0x3000), pre, RET],
                        [adrp(0x2000, 0x4000), replacement, RET])
                    self.assertEqual(result['status'], 'ok' if replacement == pre else 'mismatch')

    def test_unsigned_memory_relocation_retains_size_and_registers(self):
        # The supported immediate-offset ldr/str pair may change only imm12.
        for opcode in (0xF9400001, 0xF9000001):
            for mutation in (0, 1, 1 << 30):
                with self.subTest(opcode=hex(opcode), mutation=hex(mutation)):
                    result = self.pair_check(
                        [adrp(0x1000, 0x3000), opcode | (0x20 << 10), RET],
                        [adrp(0x2000, 0x4000), (opcode | (0x40 << 10)) ^ mutation, RET])
                    self.assertEqual(result['status'], 'mismatch' if mutation else 'ok')

    def test_calls_invalidate_only_abi_clobbered_pair_registers(self):
        for indirect in (False, True):
            for reg in (0, 8, 16, 18, 19, 29, 30):
                with self.subTest(indirect=indirect, reg=reg):
                    call1 = 0xD63F0200 if indirect else branch(0x1004, 0x1800)
                    call2 = 0xD63F0200 if indirect else branch(0x2004, 0x2800)
                    result = self.pair_check(
                        [adrp(0x1000, 0x3100, reg), call1, add_immediate(1, reg, 0x100), RET],
                        [adrp(0x2000, 0x4200, reg), call2, add_immediate(1, reg, 0x200), RET])
                    self.assertEqual(result['status'], 'ok' if reg in (19, 29) else 'mismatch')

    def test_recursive_entry_calls_obey_the_same_abi(self):
        for reg in (0, 18, 19, 29, 30):
            with self.subTest(reg=reg):
                result = self.pair_check(
                    [adrp(0x1000, 0x3100, reg), branch(0x1004, 0x1000),
                     add_immediate(1, reg, 0x100), RET],
                    [adrp(0x2000, 0x4200, reg), branch(0x2004, 0x2000),
                     add_immediate(1, reg, 0x200), RET])
                self.assertEqual(result['status'], 'ok' if reg in (19, 29) else 'mismatch')

    def test_backward_edges_require_pair_agreement(self):
        # All direct branch families must feed the fixed-point join. These
        # encodings branch from offset 12 back to offset 4.
        backedges = (0x17FFFFFE, 0x54FFFFC1, 0xB5FFFFC2, 0x3707FFC2)
        for backedge in backedges:
            for clobber in (False, True):
                with self.subTest(backedge=hex(backedge), clobber=clobber):
                    # The first iteration has a valid ADRP; later iterations
                    # do not if x19 is rewritten before returning to the ADD.
                    overwrite = 0xAA1F03F3 if clobber else NOP  # mov x19,xzr
                    result = self.pair_check(
                        [adrp(0x1000, 0x3100, 19), add_immediate(1, 19, 0x100),
                         overwrite, backedge, RET],
                        [adrp(0x2000, 0x4200, 19), add_immediate(1, 19, 0x200),
                         overwrite, backedge, RET])
                    self.assertEqual(result['status'], 'mismatch' if clobber else 'ok')

    def test_forward_join_requires_pair_agreement(self):
        for clobber in (False, True):
            with self.subTest(clobber=clobber):
                overwrite = 0xAA1F03F3 if clobber else NOP
                result = self.pair_check(
                    [adrp(0x1000, 0x3100, 19), cbnz(0x1004, 0x100C), overwrite,
                     add_immediate(1, 19, 0x100), RET],
                    [adrp(0x2000, 0x4200, 19), cbnz(0x2004, 0x200C), overwrite,
                     add_immediate(1, 19, 0x200), RET])
                self.assertEqual(result['status'], 'mismatch' if clobber else 'ok')

    def test_forward_branch_skips_unreachable_pair_definition(self):
        result = self.pair_check(
            [adrp(0x1000, 0x3100, 19), branch(0x1004, 0x100C, False),
             adrp(0x1008, 0x5000, 19), add_immediate(1, 19, 0x100), RET],
            [adrp(0x2000, 0x4200, 19), branch(0x2004, 0x200C, False),
             adrp(0x2008, 0x5000, 19), add_immediate(1, 19, 0x200), RET])
        self.assertEqual(result['status'], 'ok')

    def test_trap_terminates_fallthrough_but_not_branch_around_it(self):
        for branch_around in (False, True):
            with self.subTest(branch_around=branch_around):
                edge = branch(0x1004, 0x100C, False) if branch_around else NOP
                result = self.pair_check(
                    [adrp(0x1000, 0x3100, 19), edge, 0xE7FFDEFE,
                     add_immediate(1, 19, 0x100), RET],
                    [adrp(0x2000, 0x4200, 19), edge, 0xD4200000,
                     add_immediate(1, 19, 0x200), RET])
                self.assertEqual(result['status'], 'ok' if branch_around else 'mismatch')

    def test_loop_can_reestablish_pair_after_clobber(self):
        # A loop-carried clobber is harmless when every path executes ADRP
        # again before the relocated use.
        result = self.pair_check(
            [adrp(0x1000, 0x3100, 19), add_immediate(1, 19, 0x100),
             0xAA1F03F3, cbnz(0x100C, 0x1000), RET],
            [adrp(0x2000, 0x4200, 19), add_immediate(1, 19, 0x200),
             0xAA1F03F3, cbnz(0x200C, 0x2000), RET])
        self.assertEqual(result['status'], 'ok')

    def test_unmodeled_control_flow_requires_literal_pair_instructions(self):
        # A computed branch can re-enter before the use without executing the
        # ADRP; an internal BL needs return-edge analysis. Until modeled, both
        # disable relocation normalization for the containing function.
        for control in (0xD61F0060, branch(0x1008, 0x1004)):  # br x3; bl -4
            for relocated in (False, True):
                with self.subTest(control=hex(control), relocated=relocated):
                    result = self.pair_check(
                        [adrp(0x1000, 0x3100, 19), add_immediate(1, 19, 0x100),
                         control, RET],
                        [adrp(0x2000, 0x4200, 19),
                         add_immediate(1, 19, 0x200 if relocated else 0x100), control, RET],
                        rebuilt_data=0x4200 if relocated else 0x4100)
                    self.assertEqual(result['status'], 'mismatch' if relocated else 'ok')

    def test_unmodeled_control_flow_rejects_changed_page_with_literal_use(self):
        # The literal-pair rule covers the ADRP too: a changed page must not be
        # excused because its use keeps the same lo12. Supported flow still
        # verifies the same pair, and accepts it only with the right target.
        ldr = 0xF9408261  # ldr x1, [x19, #0x100]
        for control in (0xD61F0060, branch(0x100C, 0x1004), NOP):  # br x3; bl -8
            for use in (add_immediate(1, 19, 0x100), ldr):
                for right in (True, False):
                    if use == ldr and control == NOP:
                        continue  # a supported LDR use requires GOT evidence
                    with self.subTest(control=hex(control), use=hex(use), right=right):
                        result = self.pair_check(
                            [adrp(0x1000, 0x3100, 19), use, NOP, control, RET],
                            [adrp(0x2000, 0x5100, 19), use, NOP, control, RET],
                            rebuilt_data=0x5100 if right else 0x4100)
                        if control == NOP:
                            self.assertEqual(result['status'], 'ok' if right else 'mismatch')
                        else:
                            self.assertEqual(result['status'], 'mismatch')
                            self.assertIn('mismatch at 0x1000:', result['reason'])
                            self.assertIn('unresolved control-flow edges', result['reason'])

    def test_unmodeled_flow_page_rule_spans_trap_segments_and_reports_first(self):
        trap = 0xE7FFDEFE
        # cbnz x0 skips an embedded trap; the changed ADRP is in the next segment.
        result = self.pair_check(
            [cbnz(0x1000, 0x1008, 0), trap, adrp(0x1008, 0x3100, 19),
             add_immediate(1, 19, 0x100), 0xD61F0060, RET],
            [cbnz(0x2000, 0x2008, 0), trap, adrp(0x2008, 0x5100, 19),
             add_immediate(1, 19, 0x100), 0xD61F0060, RET],
            rebuilt_data=0x5100)
        self.assertEqual(result['status'], 'mismatch')
        self.assertIn('mismatch at 0x1008:', result['reason'])
        result = self.pair_check(
            [adrp(0x1000, 0x3100, 19), add_immediate(1, 19, 0x100),
             adrp(0x1008, 0x3100, 20), add_immediate(2, 20, 0x100), 0xD61F0060, RET],
            [adrp(0x2000, 0x5100, 19), add_immediate(1, 19, 0x100),
             adrp(0x2008, 0x5100, 20), add_immediate(2, 20, 0x100), 0xD61F0060, RET],
            rebuilt_data=0x5100)
        self.assertEqual(result['status'], 'mismatch')
        self.assertIn('mismatch at 0x1000:', result['reason'])

    def test_unmodeled_control_flow_keeps_first_use_diagnostic(self):
        # A changed use is still reported at the use, as before.
        result = self.pair_check(
            [adrp(0x1000, 0x3100, 19), add_immediate(1, 19, 0x100), 0xD61F0060, RET],
            [adrp(0x2000, 0x5200, 19), add_immediate(1, 19, 0x200), 0xD61F0060, RET],
            rebuilt_data=0x5200)
        self.assertEqual(result['status'], 'mismatch')
        self.assertIn('mismatch at 0x1004:', result['reason'])
        self.assertIn('unresolved control-flow edges', result['reason'])

    def test_writes_invalidate_pairs_including_w_alias_and_writeback(self):
        for overwrite in (0xAA1F03F3, 0x2A1F03F3, 0xF8008E61):
            with self.subTest(overwrite=hex(overwrite)):
                # mov x19,xzr; mov w19,wzr; str x1,[x19,#8]!
                result = self.pair_check(
                    [adrp(0x1000, 0x3100, 19), overwrite, add_immediate(1, 19, 0x100), RET],
                    [adrp(0x2000, 0x4200, 19), overwrite, add_immediate(1, 19, 0x200), RET])
                self.assertEqual(result['status'], 'mismatch')

    def test_register_move_carries_pair(self):
        # mov x20,x19 hands the page to x20; the use through x20 is verified
        # against the data map, so a wrong target is still a mismatch.
        for low, expected in ((0x200, 'ok'), (0x208, 'mismatch')):
            with self.subTest(low=hex(low)):
                result = self.pair_check(
                    [adrp(0x1000, 0x3100, 19), 0xAA1303F4, add_immediate(1, 20, 0x100), RET],
                    [adrp(0x2000, 0x4200, 19), 0xAA1303F4, add_immediate(1, 20, low), RET])
                self.assertEqual(result['status'], expected)
                if expected == 'mismatch':
                    self.assertIn('wrong data reference', result['reason'])

    def test_page_parked_in_second_register_survives_loop(self):
        # The compiler reuses x19 inside a loop: park the page in x20, clobber
        # x19, move it back before the backedge. Every path into the use then
        # holds the same pair. Without the move back, it does not.
        for restore in (0xAA1403F3, NOP):  # mov x19,x20
            with self.subTest(restore=hex(restore)):
                result = self.pair_check(
                    [adrp(0x1000, 0x3100, 19), add_immediate(1, 19, 0x100), 0xAA1303F4,
                     0xAA1F03F3, restore, cbnz(0x1014, 0x1004), RET],
                    [adrp(0x2000, 0x4200, 19), add_immediate(1, 19, 0x200), 0xAA1303F4,
                     0xAA1F03F3, restore, cbnz(0x2014, 0x2004), RET])
                self.assertEqual(result['status'], 'ok' if restore != NOP else 'mismatch')

    def test_register_move_carries_nothing_it_does_not_hold(self):
        cases = {
            'w move': [0xAA1F03F4, 0x2A1303F4, NOP],            # mov x20,xzr; mov w20,w19
            'source clobbered': [0xAA1303F4, 0xAA1F03F4, 0xAA1403F5],  # x20=x19; x20=xzr; x21=x20
            'copy lost in call': [0xAA1303E8, None, 0xAA0803F4],  # x8=x19; bl; x20=x8
            'copy on one path': [None, 0xAA1303F4, NOP],          # cbnz over mov x20,x19
        }
        for name, middle in cases.items():
            with self.subTest(name=name):
                use = 21 if name == 'source clobbered' else 20
                def body(base, low):
                    code = [adrp(base, 0x3100 if base == 0x1000 else 0x4200, 19)] + list(middle)
                    if name == 'copy lost in call':
                        code[2] = branch(base + 8, base + 0x800)
                    if name == 'copy on one path':
                        code[1] = cbnz(base + 4, base + 12)
                        code.insert(1, 0xAA1F03F4)  # x20 starts without a pair
                    return code + [add_immediate(1, use, low), RET]
                result = self.pair_check(body(0x1000, 0x100), body(0x2000, 0x200))
                self.assertEqual(result['status'], 'mismatch')
                self.assertIn('unproven ADRP pair', result['reason'])

    def test_static_call_uses_qualified_owner(self):
        for wrong in (False, True):
            with self.subTest(wrong=wrong):
                result = self.check(
                    [('caller', 0x1000, words(branch(0x1000, 0x1100), RET)),
                     ('static_fn', 0x1100, words(RET)), ('static_fn', 0x1200, words(RET))],
                    [('caller', 0x2000, words(branch(0x2000, 0x2400 if wrong else 0x2300), RET)),
                     ('static_fn', 0x2400, words(RET)), ('static_fn', 0x2300, words(RET))],
                    rows=[('caller', 0x1000, 8), ('static_fn@A.cpp', 0x1100, 4),
                          ('static_fn@B.cpp', 0x1200, 4)],
                    owners=[('static_fn', 0x2300, 'src/A.cpp.obj'),
                            ('static_fn', 0x2400, 'src/B.cpp.obj')])
                self.assertEqual(result['status'], 'mismatch' if wrong else 'ok')
                if wrong:
                    self.assertIn('static_fn@A.cpp', result['reason'])

    def test_qualified_body_cannot_use_other_owner(self):
        result = self.check([('static_fn', 0x1000, words(NOP, RET))],
                            [('static_fn', 0x2000, words(RET, RET)),
                             ('static_fn', 0x2100, words(NOP, RET))],
                            rows=[('static_fn@A.cpp', 0x1000, 8)],
                            owners=[('static_fn', 0x2000, 'src/A.cpp.obj'),
                                    ('static_fn', 0x2100, 'src/B.cpp.obj')])
        self.assertEqual(result['status'], 'mismatch')

    def test_bare_duplicate_name_is_ambiguous_even_when_first_body_matches(self):
        for reverse in (False, True):
            with self.subTest(reverse=reverse):
                rebuilt = [('static_fn', 0x2000, words(NOP, RET)),
                           ('static_fn', 0x2100, words(RET, RET))]
                result = self.check([('static_fn', 0x1000, words(NOP, RET))],
                                    rebuilt[::-1] if reverse else rebuilt)
                self.assertEqual(result['status'], 'not-checkable')
                self.assert_single_rejected_without_promotion('static_fn')

    def test_qualified_duplicate_bodies_resolve_independent_of_symbol_order(self):
        for reverse in (False, True):
            for owner, body in (('A.cpp', words(NOP, RET)), ('B.cpp', words(RET, RET))):
                with self.subTest(reverse=reverse, owner=owner):
                    rebuilt = [('static_fn', 0x2000, words(NOP, RET)),
                               ('static_fn', 0x2100, words(RET, RET))]
                    result = self.check(
                        [('static_fn', 0x1000, body)], rebuilt[::-1] if reverse else rebuilt,
                        rows=[(f'static_fn@{owner}', 0x1000, 8)],
                        owners=[('static_fn', 0x2000, 'src/A.cpp.obj'),
                                ('static_fn', 0x2100, 'src/B.cpp.obj')])
                    self.assertEqual(result['status'], 'ok')

    def test_single_ambiguous_exact_name_cannot_fuzzy_select_another_function(self):
        result = self.check(
            [('static_fn', 0x1000, words(RET)),
             ('static_fn_helper', 0x1100, words(RET))],
            [('static_fn', 0x2000, words(RET)),
             ('static_fn', 0x2100, words(RET)),
             ('static_fn_helper', 0x2200, words(RET))])
        self.assertEqual(result['status'], 'not-checkable')
        self.assert_single_rejected_without_promotion('static_fn')

    def test_stale_map_cannot_substitute_different_elf_symbol_at_same_address(self):
        # A stale address still points into valid code with equal bytes, but
        # the rebuilt ELF says the named function is elsewhere.
        result = self.check(
            [('static_fn', 0x1000, words(NOP, RET))],
            [('other_function', 0x2000, words(NOP, RET)),
             ('static_fn', 0x2100, words(RET, RET))],
            rows=[('static_fn@A.cpp', 0x1000, 8)],
            owners=[('static_fn', 0x2000, 'src/A.cpp.obj', 8)])
        self.assertEqual(result['status'], 'not-checkable')

    def test_qualified_map_requires_binding_to_loaded_elf(self):
        for binding in ('missing', 'stale-map', 'stale-elf', 'malformed',
                        'unsupported-format', 'valid'):
            with self.subTest(binding=binding):
                result = self.check(
                    [('static_fn', 0x1000, words(RET))],
                    [('static_fn', 0x2000, words(RET))],
                    rows=[('static_fn@A.cpp', 0x1000, 4)],
                    owners=[('static_fn', 0x2000, 'src/A.cpp.obj')], map_binding=binding)
                self.assertEqual(result['status'], 'ok' if binding == 'valid' else 'not-checkable')
                if binding != 'valid':
                    self.assert_single_rejected_without_promotion('static_fn@A.cpp')

    def test_qualified_map_size_must_agree_with_elf(self):
        result = self.check(
            [('static_fn', 0x1000, words(NOP, RET))],
            [('static_fn', 0x2000, words(NOP, RET))],
            rows=[('static_fn@A.cpp', 0x1000, 8)],
            owners=[('static_fn', 0x2000, 'src/A.cpp.obj', 4)])
        self.assertEqual(result['status'], 'not-checkable')

    def test_literal_qualified_elf_name_cannot_bypass_ownership_evidence(self):
        result = self.check(
            [('static_fn', 0x1000, words(RET))],
            [('static_fn@A.cpp', 0x2000, words(RET))],
            rows=[('static_fn@A.cpp', 0x1000, 4)])
        self.assertEqual(result['status'], 'not-checkable')
        self.assert_single_rejected_without_promotion('static_fn@A.cpp')

    def test_missing_or_ambiguous_owner_is_not_checkable(self):
        for owners in ([], [('static_fn', 0x2000, 'src/B.cpp.obj')],
                       [('static_fn', 0x2000, 'src/one/A.cpp.obj'),
                        ('static_fn', 0x2100, 'src/two/A.cpp.obj')]):
            with self.subTest(owners=owners):
                result = self.check([('static_fn', 0x1000, words(RET))],
                                    [('static_fn', 0x2000, words(RET)), ('static_fn', 0x2100, words(RET))],
                                    rows=[('static_fn@A.cpp', 0x1000, 4)], owners=owners)
                self.assertEqual(result['status'], 'not-checkable')
                self.assertIn('unknown function', result['reason'])

    def test_undecodable_words_fail_closed(self):
        # 0xffffffff is unallocated AArch64, unlike the game's recognized trap.
        good = words(NOP, NOP, RET)
        for offset in (0, 4, 8):
            bad = good[:offset] + words(0xFFFFFFFF) + good[offset + 4:]
            for original, rebuilt in ((bad, good), (good, bad), (bad, bad)):
                with self.subTest(offset=offset, original=original.hex(), rebuilt=rebuilt.hex()):
                    result = self.check([('body', 0x1000, original)], [('body', 0x2000, rebuilt)])
                    self.assertEqual(result['status'], 'mismatch')

    def test_partial_instruction_tails_fail_closed(self):
        for tail in (b'\x00', b'\x00\x00', b'\x00\x00\x00'):
            with self.subTest(tail=len(tail)):
                body = words(RET) + tail
                result = self.check([('body', 0x1000, body)], [('body', 0x2000, body)])
                self.assertEqual(result['status'], 'mismatch')

    def test_only_real_brk_can_replace_game_trap(self):
        for replacement, expected in ((0xE7FFDEFE, 'ok'), (0xD4200000, 'ok'),
                                      (0xD43FFFE0, 'ok'), (0xD4200001, 'mismatch'),
                                      (NOP, 'mismatch')):
            with self.subTest(replacement=hex(replacement)):
                result = self.check([('body', 0x1000, words(0xE7FFDEFE, RET))],
                                    [('body', 0x2000, words(replacement, RET))])
                self.assertEqual(result['status'], expected)

    def test_mismatch_after_game_trap_is_checked(self):
        result = self.check([('body', 0x1000, words(0xE7FFDEFE, NOP, RET))],
                            [('body', 0x2000, words(0xD4200000, RET, RET))])
        self.assertEqual(result['status'], 'mismatch')

    def test_full_check_missing_matching_symbol_fails_but_unimplemented_is_allowed(self):
        # Populate an isolated repository, then exercise the real full-check CLI.
        self.check([('present', 0x1000, words(RET))], [('present', 0x2000, words(RET))])
        for quality, succeeds in (('O', False), ('U', True)):
            with self.subTest(quality=quality):
                (self.root / 'data/functions.csv').write_text(
                    f'Address,Quality,Size,Name\n0x{BASE + 0x1000:x},{quality},4,missing\n')
                run = subprocess.run([str(CHECKER), '--warnings-as-errors'], cwd=self.root,
                                     text=True, capture_output=True, timeout=30)
                self.assertEqual(run.returncode == 0, succeeds, run.stdout + run.stderr)
                if not succeeds:
                    self.assertIn('unknown function: missing', run.stdout + run.stderr)

    def test_same_address_function_alias_is_valid(self):
        result = self.check(
            [('caller', 0x1000, words(branch(0x1000, 0x1100), RET)),
             ('target_alias', 0x1100, words(RET))],
            [('caller', 0x2000, words(branch(0x2000, 0x2300), RET)),
             ('target', 0x2300, words(RET)), ('target_alias', 0x2300, words(RET))])
        self.assertEqual(result['status'], 'ok')

    def test_same_name_same_address_alias_is_valid(self):
        result = self.check([('body', 0x1000, words(RET))],
                            [('body', 0x2000, words(RET)), ('body', 0x2000, words(RET))])
        self.assertEqual(result['status'], 'ok')

    def test_qualified_same_address_alias_is_valid(self):
        for reverse in (False, True):
            with self.subTest(reverse=reverse):
                rebuilt = [('static_fn_alias', 0x2000, words(RET)),
                           ('static_fn', 0x2000, words(RET))]
                result = self.check(
                    [('static_fn', 0x1000, words(RET))],
                    rebuilt[::-1] if reverse else rebuilt,
                    rows=[('static_fn@A.cpp', 0x1000, 4)],
                    owners=[('static_fn', 0x2000, 'src/A.cpp.obj')])
                self.assertEqual(result['status'], 'ok')

    def test_conflicting_rebuilt_alias_sizes_are_not_checkable(self):
        for reverse in (False, True):
            with self.subTest(reverse=reverse):
                # Both symbols describe the same start, but disagree on the
                # body boundary. Picking either one silently is unsafe.
                rebuilt = [('body', 0x2000, words(NOP, RET)),
                           ('body_alias', 0x2000, words(NOP))]
                result = self.check([('body', 0x1000, words(NOP, RET))],
                                    rebuilt[::-1] if reverse else rebuilt)
                self.assertEqual(result['status'], 'not-checkable')

    def test_conflicting_alias_cannot_be_accepted_as_call_target(self):
        result = self.check(
            [('caller', 0x1000, words(branch(0x1000, 0x1100), RET)),
             ('target', 0x1100, words(NOP, RET))],
            [('caller', 0x2000, words(branch(0x2000, 0x2300), RET)),
             ('target', 0x2300, words(NOP, RET)),
             ('target_alias', 0x2300, words(NOP))])
        self.assertNotEqual(result['status'], 'ok')

    def test_object_symbol_cannot_be_accepted_as_function_call_target(self):
        result = self.check(
            [('caller', 0x1000, words(branch(0x1000, 0x1100), RET)),
             ('target', 0x1100, words(RET))],
            [('caller', 0x2000, words(branch(0x2000, 0x2300), RET))],
            objects=[('target', 0x2300, 4)])
        self.assertNotEqual(result['status'], 'ok')

    def test_conflicting_original_alias_sizes_reject_inventory(self):
        self.check([('body', 0x1000, words(NOP, RET))],
                   [('body', 0x2000, words(NOP, RET))])
        (self.root / 'data/functions.csv').write_text(
            'Address,Quality,Size,Name\n'
            f'0x{BASE + 0x1000:x},O,8,body\n'
            f'0x{BASE + 0x1000:x},O,4,body_alias\n')
        result_path = self.root / 'result.json'
        result_path.unlink()
        run = subprocess.run(
            [str(CHECKER), '--batch', 'request.json', '--batch-output', 'result.json'],
            cwd=self.root, text=True, capture_output=True, timeout=30)
        self.assertNotEqual(run.returncode, 0, run.stdout + run.stderr)
        self.assertFalse(result_path.exists(), 'invalid inventory must not publish successful results')

    def test_batch_rejects_wrong_name_at_valid_address(self):
        result = self.check([('right', 0x1000, words(RET))], [('right', 0x2000, words(RET))],
                            request=[{'address': f'0x{BASE + 0x1000:x}', 'name': 'wrong'}])
        self.assertEqual(result['status'], 'not-checkable')
        self.assertIn('identity name disagrees', result['reason'])


if __name__ == '__main__':
    unittest.main(verbosity=2)
