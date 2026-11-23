#!/usr/bin/env python3
"""Unit tests for tools/indirect_flow_evidence.py using synthetic instruction streams.

No game binary is needed. Instruction words come from the AArch64 encodings observed in
the original image; the fake ELF only serves table bytes and section names.
"""
import struct
import sys
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import indirect_flow_evidence as E  # noqa: E402


class FakeElf:
    def __init__(self, tables):
        self.tables = tables       # address -> bytes
        self.relocs = {}

    def section_of(self, vaddr):
        for addr, data in self.tables.items():
            if addr <= vaddr < addr + len(data):
                return {'name': '.rodata', 'type': 1}
        return None

    def read(self, vaddr, size):
        for addr, data in self.tables.items():
            if addr <= vaddr and vaddr + size <= addr + len(data):
                return data[vaddr - addr:vaddr - addr + size]
        return None


def insn(addr, word, text):
    mnemonic, _, operands = text.partition(' ')
    i = E.Insn(addr, word, mnemonic, operands)
    i.decode()
    return i


def function(start, listing, tables):
    insns = [insn(start + 4 * k, word, text) for k, (word, text) in enumerate(listing)]
    return E.Function('fixture', FakeElf(tables), start, 4 * len(insns), insns)


def table_bytes(base, targets):
    return b''.join(struct.pack('<i', t - base) for t in targets)


# SetCameraMode-shaped dispatch at 0x1000: cmp w19,#3 / b.hi / mov w8,w19 / adrp / add /
# ldrsw / add / br, then three case blocks and a shared tail.
TABLE = 0x3000


def dispatch(guard=(0x71000e7f, 'cmp w19, #0x3'), branch=(0x540001a8, 'b.hi 0x1034'),
             index=(0x2a1303e8, 'mov w8, w19'), base_add=(0x91000129, 'add x9, x9, #0x0')):
    return [
        (0xa9be4ff4, 'stp x20, x19, [sp, #-0x20]!'),
        (0x2a0103f3, 'mov w19, w1'),
        guard,
        branch,
        index,
        (0x90000009, 'adrp x9, 0x3000'),      # page: resolves to 0x3000 from 0x1014 (same page offset trick below)
        base_add,
        (0xb8a87928, 'ldrsw x8, [x9, x8, lsl #2]'),
        (0x8b090108, 'add x8, x8, x9'),
        (0xd61f0100, 'br x8'),
        (0x94000000, 'bl 0x1024'),               # 0x1028
        (0x14000003, 'b 0x1038'),                # 0x102c -> 0x1038
        (0x94000000, 'bl 0x1030'),               # 0x1030
        (0x14000001, 'b 0x1038'),                # 0x1034 -> 0x1038
        (0xd65f03c0, 'ret'),                     # 0x1038
    ]


class DecoderTests(unittest.TestCase):
    def test_observed_encodings(self):
        self.assertEqual(E.decode_ldr_reg(0xb8a87928), (8, 9, 8, 4, True, 3, 2))
        self.assertEqual(E.decode_add_imm(0x9121c129), (9, 9, 0x870))
        self.assertEqual(E.decode_adrp(0xf0007109)[0], 9)
        self.assertEqual(E.decode_subs_imm(0x71000e7f), (0, 31, 19, 3))
        self.assertEqual(E.decode_copy32(0x2a1303e8), (8, 19))
        self.assertEqual(E.decode_copy32(0x92407f28), (8, 25))      # and x8, x25, #0xffffffff
        self.assertIsNone(E.decode_copy32(0x92401f28))               # and x8, x25, #0xff is not a copy
        self.assertEqual(E.decode_ldr_imm(0xf9432908), (8, 8, 0x650, 8, False))
        self.assertEqual(E.decode_ldr_imm(0xf8418402), (2, 0, 0, 8, False))   # ldr x2, [x0], #0x18
        self.assertEqual(E.decode_add_shifted(0x8b090108), (8, 8, 9, 0, 0))
        self.assertEqual(E.decode_mov_reg(0xaa1903fa), (1, 26, 25))

    def test_edge_bounds(self):
        self.assertEqual(E.edge_bound('hi', False, 3)[0], 4)
        self.assertEqual(E.edge_bound('ls', True, 3)[0], 4)
        self.assertEqual(E.edge_bound('hs', False, 4)[0], 4)
        self.assertEqual(E.edge_bound('lo', True, 4)[0], 4)
        self.assertIsNone(E.edge_bound('hi', True, 3)[0])
        self.assertIsNone(E.edge_bound('ls', False, 3)[0])
        self.assertIsNone(E.edge_bound('gt', False, 3)[0])      # signed: no unsigned bound
        self.assertIsNone(E.edge_bound('ne', True, 3)[0])

    def test_register_writes(self):
        self.assertEqual(insn(0, 0xb4000041, 'cbz x1, 0x8').writes, set())
        self.assertEqual(insn(0, 0xf8418402, 'ldr x2, [x0], #0x18').writes, {0, 2})
        self.assertEqual(insn(0, 0xa9017bfd, 'stp x29, x30, [sp, #0x10]').writes, set())
        self.assertEqual(insn(0, 0xa8c24ff4, 'ldp x20, x19, [sp], #0x20').writes, {19, 20})
        self.assertEqual(insn(0, 0x2a1303e8, 'mov w8, w19').writes32, {8})


class JumpTableTests(unittest.TestCase):
    def analyse(self, listing, targets, start=0x1000):
        fn = function(start, listing, {TABLE: table_bytes(TABLE, targets)})
        # adrp from 0x1014 to page 0x3000: patch the fixture word so the decoder lands on TABLE
        br = next(i for i in fn.insns if i.kind == 'br')
        fn.build_cfg()
        return E.analyse_branch(fn, br), fn

    def fixture_adrp(self, listing, at=0x1014):
        pages = ((TABLE >> 12) - (at >> 12)) & 0x1fffff
        word = 0x90000009 | ((pages & 3) << 29) | ((pages >> 2) << 5)
        return [(word, 'adrp x9, 0x3000') if text.startswith('adrp') else (w, text) for w, text in listing]

    def test_bounded_table_is_proved_and_targets_enumerated(self):
        targets = [0x1028, 0x1030, 0x1038, 0x1030]
        result, fn = self.analyse(self.fixture_adrp(dispatch()), targets)
        self.assertEqual(result['pattern'], 'switch_dispatch')
        self.assertEqual(result['proof'], 'proved')
        jt = result['jump_table']
        self.assertEqual(jt['entry_count'], 4)
        self.assertEqual([int(t, 16) for t in jt['targets']], targets)
        self.assertTrue(jt['all_targets_inside_function'])
        self.assertEqual(jt['guards']['bounds'], [4])

    def test_missing_guard_is_not_proved(self):
        listing = self.fixture_adrp(dispatch(guard=(0xd503201f, 'nop'), branch=(0xd503201f, 'nop')))
        result, _ = self.analyse(listing, [0x1028, 0x1030, 0x1038, 0x1030])
        self.assertEqual(result['proof'], 'partial')
        self.assertFalse(result['jump_table']['entry_count_proved'])

    def test_guard_on_other_register_is_not_proved(self):
        listing = self.fixture_adrp(dispatch(guard=(0x71000e9f, 'cmp w20, #0x3')))
        result, _ = self.analyse(listing, [0x1028, 0x1030, 0x1038, 0x1030])
        self.assertEqual(result['proof'], 'partial')

    def test_inverted_guard_edge_is_not_proved(self):
        # b.ls skips the dispatch: the fallthrough into the table carries index > 3.
        listing = self.fixture_adrp(dispatch(branch=(0x540001a9, 'b.ls 0x1034')))
        result, _ = self.analyse(listing, [0x1028, 0x1030, 0x1038, 0x1030])
        self.assertEqual(result['proof'], 'partial')

    def test_signed_guard_is_not_proved(self):
        listing = self.fixture_adrp(dispatch(branch=(0x540001ac, 'b.gt 0x1034')))
        result, _ = self.analyse(listing, [0x1028, 0x1030, 0x1038, 0x1030])
        self.assertEqual(result['proof'], 'partial')

    def test_sixty_four_bit_index_definition_is_not_proved(self):
        # and x8, x19, #0xff keeps bits the W compare never saw if the guard is on x19's W view only.
        listing = self.fixture_adrp(dispatch(index=(0x92401e68, 'and x8, x19, #0xff')))
        result, _ = self.analyse(listing, [0x1028, 0x1030, 0x1038, 0x1030])
        self.assertEqual(result['proof'], 'partial')

    def test_target_outside_function_is_unresolved(self):
        result, _ = self.analyse(self.fixture_adrp(dispatch()), [0x1028, 0x1030, 0x2000, 0x1030])
        self.assertEqual(result['proof'], 'unresolved')

    def test_zero_extending_and_mask_counts_as_copy(self):
        listing = self.fixture_adrp(dispatch(index=(0x92407e68, 'and x8, x19, #0xffffffff')))
        result, _ = self.analyse(listing, [0x1028, 0x1030, 0x1038, 0x1030])
        self.assertEqual(result['proof'], 'proved')


class TailCallTests(unittest.TestCase):
    def test_got_loaded_global_pointer(self):
        fn = function(0x1000, [
            (0xb0000008, 'adrp x8, 0x2000'),
            (0xf9400108, 'ldr x8, [x8, #0x0]'),
            (0xf9400101, 'ldr x1, [x8]'),
            (0xd61f0020, 'br x1'),
        ], {})
        fn.elf.relocs[0x2000] = {'type': E.R_AARCH64_RELATIVE, 'symbol': '', 'addend': 0x5000, 'section': '.rela.dyn'}
        fn.elf.section_of = lambda v: {'name': '.got', 'type': 1} if 0x2000 <= v < 0x2100 else None
        result = E.analyse_branch(fn, fn.insns[3])
        self.assertEqual(result['pattern'], 'indirect_tail_call')
        self.assertEqual(result['subtype'], 'global_function_pointer_via_got')
        self.assertEqual(result['proof'], 'inferred')

    def test_null_checked_argument_field(self):
        fn = function(0x1000, [
            (0xf9401c02, 'ldr x2, [x0, #0x38]'),
            (0xb4000062, 'cbz x2, 0x100c'),
            (0xaa0103e0, 'mov x0, x1'),
            (0xd61f0040, 'br x2'),
            (0xd65f03c0, 'ret'),
        ], {})
        result = E.analyse_branch(fn, fn.insns[3])
        self.assertEqual(result['subtype'], 'field_function_pointer_via_parameter')

    def test_pc_relative_table_of_absolute_pointers_is_not_a_tail_call(self):
        # ldr x8, [x9, x8, lsl #3] from a PC-relative table: neither a proved switch nor a tail call.
        fn = function(0x1000, [
            (0x90000009, 'adrp x9, 0x2000'),
            (0x91000129, 'add x9, x9, #0x0'),
            (0xf8687928, 'ldr x8, [x9, x8, lsl #3]'),
            (0xd61f0100, 'br x8'),
        ], {})
        result = E.analyse_branch(fn, fn.insns[3])
        self.assertNotEqual(result['pattern'], 'indirect_tail_call')
        self.assertNotEqual(result['proof'], 'proved')


if __name__ == '__main__':
    unittest.main()
