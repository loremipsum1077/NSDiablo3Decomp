import types
import unittest
from unittest.mock import patch

from validation import lexer_calls as calls


class CallIdentityTests(unittest.TestCase):
    def test_external_symbol_requires_regular_executable_unique_extent(self):
        good = ('target', 2, 0, 2, 1, 0x1000, 16)
        sections = [(0, 0, 0, 0, 0, 0), (0, 1, 6, 0x1000, 64, 32)]
        reader = types.SimpleNamespace(read=lambda offset, size: bytes(size))
        with patch.object(calls.qualified, '_file_mapped_code', return_value=bytes(16)), \
             patch.object(calls.qualified, '_mapped_code', return_value=bytes(16)):
            self.assertEqual(calls.external_target('target', 0x1000, 16, [good], sections, reader, b''), 0x1000)
            variants = ([('target', 2, 0, 2, 0xfff1, 0x1000, 16)],
                        [good, ('resolver', 10, 0, 2, 1, 0x1000, 0)],
                        [good, ('overlap', 2, 0, 2, 1, 0x1004, 4)],
                        [good, ('interior_entry', 2, 0, 2, 1, 0x1004, 0)], [good, good])
            for symbols in variants:
                with self.subTest(symbols=symbols), self.assertRaises(ValueError):
                    calls.external_target('target', 0x1000, 16, symbols, sections, reader, b'')
            for bad in ((0, 8, 6, 0x1000, 64, 32), (0, 1, 2, 0x1000, 64, 32),
                        (0, 1, 6, 0x1000, 64, 8)):
                with self.subTest(section=bad), self.assertRaises(ValueError):
                    calls.external_target('target', 0x1000, 16, [good], [sections[0], bad], reader, b'')

    def test_call_must_encode_exact_original_target(self):
        self.assertEqual(calls.patch_call(0x94000000, 0x94000004, 0x1000, 0x1010), 0x94000004)
        self.assertEqual(calls.patch_call(0x94000000, 0x97ffffff, 0x1000, 0xffc), 0x97ffffff)
        for args in ((0x14000000, 0x14000004, 0x1000, 0x1010),
                     (0x94000001, 0x94000004, 0x1000, 0x1010),
                     (0x94000000, 0x94000005, 0x1000, 0x1010),
                     (0x94000000, 0x94000000, 0x1000, 0x1001),
                     (0x94000000, 0x94000000, 0, 1 << 27)):
            with self.subTest(args=args), self.assertRaises(ValueError):
                calls.patch_call(*args)

    def test_scoped_section_target_requires_exact_unique_entry(self):
        member = types.SimpleNamespace(candidate_section=1, candidate_offset=16,
                                       candidate_name='__isspace', candidate_size=88)
        symbol = dict(type=3, section=1, value=0, size=0, name='')
        self.assertIs(calls.defined_target(symbol, 16, [member]), member)
        for addend, members in ((20, [member]), (-1, [member]), (16, [member, member]), (16, [])):
            with self.subTest(addend=addend), self.assertRaises(ValueError):
                calls.defined_target(symbol, addend, members)

    def test_named_target_cannot_alias_or_enter_function_interior(self):
        member = types.SimpleNamespace(candidate_section=1, candidate_offset=16,
                                       candidate_name='local', candidate_size=88)
        good = dict(type=2, section=1, value=16, size=88, name='local')
        self.assertIs(calls.defined_target(good, 0, [member]), member)
        for symbol, addend in ((dict(good, name='other'), 0), (dict(good, value=12), 4),
                              (dict(good, size=84), 0), (dict(good, type=1), 0)):
            with self.subTest(symbol=symbol), self.assertRaises(ValueError):
                calls.defined_target(symbol, addend, [member])


if __name__ == '__main__':
    unittest.main()
