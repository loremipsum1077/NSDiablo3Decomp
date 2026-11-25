import struct
import unittest

import game_descriptors as gd


class FakeELF:
    """Code words at fixed addresses, in place of a parsed main.elf."""
    def __init__(self, start, words):
        self.start, self.code = start, list(words)

    def words(self, vaddr, size):
        i = (vaddr - self.start) // 4
        return tuple(self.code[i:i + size // 4])


def adrp(rd, pc, target):
    imm = ((target & ~0xFFF) - (pc & ~0xFFF)) >> 12
    imm &= (1 << 21) - 1
    return 0x90000000 | ((imm & 3) << 29) | ((imm >> 2) << 5) | rd


def add(rd, rn, imm):
    return 0x91000000 | (imm << 10) | (rn << 5) | rd


def movz(rd, imm, hw=0):
    return 0x52800000 | (hw << 21) | (imm << 5) | rd


def movk(rd, imm, hw):
    return 0x72800000 | (hw << 21) | (imm << 5) | rd


def bl(pc, target):
    return 0x94000000 | (((target - pc) >> 2) & 0x3FFFFFF)


class RegisteredFileTests(unittest.TestCase):
    START, ADD_FILE, DATA = 0x10000, 0x8000, 0x123456

    def run_code(self, *build):
        code = [f(self.START + 4 * i) for i, f in enumerate(build)]
        elf = FakeELF(self.START, code)
        return gd.registered_file(elf, self.START, 4 * len(code), self.ADD_FILE)

    def test_address_and_wide_size(self):
        found = self.run_code(lambda pc: adrp(0, pc, self.DATA),
                              lambda pc: add(0, 0, self.DATA & 0xFFF),
                              lambda pc: movz(1, 0x2345),
                              lambda pc: movk(1, 0x1, 1),
                              lambda pc: bl(pc, self.ADD_FILE))
        self.assertEqual(found, (self.DATA, 0x12345))

    def test_other_call_clears_registers(self):
        found = self.run_code(lambda pc: adrp(0, pc, self.DATA),
                              lambda pc: add(0, 0, self.DATA & 0xFFF),
                              lambda pc: movz(1, 0x40),
                              lambda pc: bl(pc, 0x9000),
                              lambda pc: bl(pc, self.ADD_FILE))
        self.assertIsNone(found)

    def test_no_call(self):
        self.assertIsNone(self.run_code(lambda pc: movz(1, 0x40)))


class ProtoNameTests(unittest.TestCase):
    def test_name_field(self):
        name = b'Items.proto'
        self.assertEqual(gd._proto_name(b'\x0a' + bytes([len(name)]) + name + b'\x12\x02D3'), 'Items')

    def test_long_name_uses_varint(self):
        name = b'x' * 200 + b'.proto'
        self.assertEqual(gd._proto_name(b'\x0a\xce\x01' + name), 'x' * 200)

    def test_rejects_other_first_field(self):
        with self.assertRaises(ValueError):
            gd._proto_name(b'\x12\x02D3')


if __name__ == '__main__':
    unittest.main()
