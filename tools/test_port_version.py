import unittest
from collections import Counter

import port_version as pv
from test_version_diff import adrp, add_x_imm, ldr_x, func, RET


class AddressRefTests(unittest.TestCase):
    def test_adrp_add_and_ldr(self):
        # adrp x8, #0x5000 page ; add x0, x8, #0x10 ; ldr x1, [x8, #0x18] (scaled by 8)
        words = [adrp(8, 5), add_x_imm(0, 8, 0x10), ldr_x(1, 8, 3), RET]
        refs = pv.address_refs(words, 0x1000)
        self.assertEqual(refs, [(1, 0x6010), (2, 0x6018)])

    def test_got_slot_is_followed(self):
        words = [adrp(8, 5), ldr_x(1, 8, 3), RET]
        self.assertEqual(pv.address_refs(words, 0x1000, {0x6018: 0xABC0}), [(1, 0xABC0)])

    def test_load_clobbers_page_register(self):
        words = [adrp(8, 5), ldr_x(8, 8, 0), ldr_x(1, 8, 2), RET]
        self.assertEqual(pv.address_refs(words, 0x1000), [(1, 0x6000)])


class FakeELF:
    def __init__(self, funcs):
        self.funcs = {f.addr: f for f in funcs}
        self.words_by = {}

    def words(self, addr, size):
        return self.words_by[addr]


class MapDataSymbolTests(unittest.TestCase):
    def setUp(self):
        self._got = pv.got_targets
        pv.got_targets = lambda elf: {}

    def tearDown(self):
        pv.got_targets = self._got

    def pair(self, old_page, new_page, lo12):
        wa = [adrp(8, old_page), add_x_imm(0, 8, lo12), RET]
        wb = [adrp(8, new_page), add_x_imm(0, 8, lo12), RET]
        a, b = func(0x1000, wa), func(0x9000, wb)
        return a, b, wa, wb

    def test_offset_reference_maps_symbol_start_and_conflicts_drop(self):
        a1, b1, wa1, wb1 = self.pair(0x10, 0x20, 0x18)     # old 0x11018 -> new 0x29018
        old, new = FakeELF([a1]), FakeELF([b1])
        old.words_by[a1.addr], new.words_by[b1.addr] = wa1, wb1
        symbols = {0x11010: 'g_thing', 0x11100: 'g_next'}
        mapped, conflicts, seen = pv.map_data_symbols([(a1, b1)], old, new, symbols)
        self.assertEqual(mapped, {0x11010: 0x29010})          # start = target - offset
        self.assertEqual(conflicts, 0)

    def test_two_symbols_on_one_address_are_dropped(self):
        # ADRP pages are relative to the function's own page.
        # old: 0x1000 + 0x10 pages -> 0x11000 ('a');  0x2000 + 0x10 pages -> 0x12000 ('b')
        # new: 0x9000 + 0x20 pages -> 0x29000;        0xA000 + 0x1F pages -> 0x29000 (same)
        wa1 = [adrp(8, 0x10), add_x_imm(0, 8, 0), RET]
        wa2 = [adrp(8, 0x10), add_x_imm(0, 8, 0), RET]
        wb1 = [adrp(8, 0x20), add_x_imm(0, 8, 0), RET]
        wb2 = [adrp(8, 0x1F), add_x_imm(0, 8, 0), RET]
        a1, a2 = func(0x1000, wa1), func(0x2000, wa2)
        b1, b2 = func(0x9000, wb1), func(0xA000, wb2)
        old, new = FakeELF([a1, a2]), FakeELF([b1, b2])
        old.words_by.update({a1.addr: wa1, a2.addr: wa2})
        new.words_by.update({b1.addr: wb1, b2.addr: wb2})
        symbols = {0x11000: 'a', 0x12000: 'b'}
        mapped, conflicts, _ = pv.map_data_symbols([(a1, b1), (a2, b2)], old, new, symbols)
        self.assertEqual(mapped, {})
        self.assertEqual(conflicts, 2)


class CarryTests(unittest.TestCase):
    def test_matched_statuses_are_rechecked(self):
        self.assertEqual(pv.CARRY['O'], 'M')
        self.assertEqual(pv.CARRY['L'], 'L')
        self.assertEqual(Counter(pv.CARRY.values())['O'], 0)


if __name__ == '__main__':
    unittest.main()
