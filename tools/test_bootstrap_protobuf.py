import unittest

import bootstrap_protobuf as bp
from test_game_descriptors import add, adrp, bl

START, NEW, CTOR = 0x10000, 0x8000, 0x20000
GOT, VTABLE = 0x40000, 0x50000


def ldr(rt, rn, offset):
    return 0xF9400000 | ((offset // 8) << 10) | (rn << 5) | rt


def mov(rd, rm):
    return 0xAA0003E0 | (rm << 16) | rd


def code(start, *build):
    return tuple(f(start + 4 * i) for i, f in enumerate(build))


def vtable_store(reg=8, got=GOT):
    return [lambda pc: adrp(reg, pc, got), lambda pc: ldr(reg, reg, got & 0xFFF),
            lambda pc: add(reg, reg, 0x10), lambda pc: mov(19, 0)]


class VtableTests(unittest.TestCase):
    def test_got_load_needs_add_0x10(self):
        words = code(START, *vtable_store())
        self.assertEqual(bp.vtable_got_loads(words, START), [(1, GOT)])
        plain = code(START, lambda pc: adrp(8, pc, GOT), lambda pc: ldr(8, 8, GOT & 0xFFF),
                     lambda pc: add(8, 8, 0x18))
        self.assertEqual(bp.vtable_got_loads(plain, START), [])

    def test_default_instances_inline_outline_and_strings(self):
        slots = {GOT: VTABLE, GOT + 8: VTABLE + 0x100}
        ctor = code(CTOR, *vtable_store(got=GOT + 8))
        words = code(START,
                     lambda pc: bl(pc, NEW), *vtable_store(),          # inline constructor
                     lambda pc: bl(pc, NEW),                           # a std::string default
                     lambda pc: bl(pc, NEW), lambda pc: bl(pc, CTOR))  # out-of-line constructor
        found = bp.default_instance_vtables(words, START, NEW, slots.get,
                                            lambda a: ctor if a == CTOR else ())
        self.assertEqual(found, [VTABLE, VTABLE + 0x100])

    def test_complete_object_names(self):
        self.assertEqual(bp._complete('_ZN2D33Foo3BarD2Ev'), '_ZN2D33Foo3BarD1Ev')
        self.assertEqual(bp._complete('_ZN2D33Foo3BarC2ERKS1_'), '_ZN2D33Foo3BarC1ERKS1_')

    def test_same_masks_relocated_bits(self):
        ours = (0x94000000).to_bytes(4, 'little')
        theirs = (0x94001234).to_bytes(4, 'little')
        self.assertTrue(bp._same(ours, {0: 0x03FFFFFF}, theirs))
        self.assertFalse(bp._same(ours, {}, theirs))
        self.assertFalse(bp._same(ours, {}, theirs + theirs))


if __name__ == '__main__':
    unittest.main()
