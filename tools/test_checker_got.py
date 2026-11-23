#!/usr/bin/env python3
"""Asset-free GOT relocation identity tests for checker patch 0009."""
import struct
import unittest
from unittest.mock import patch

import test_checker as fixtures

BASE_WRITER = fixtures.write_elf


def got_elf(path, functions, objects, *, slot, target, kind=1027,
            symbol='right_data', offset=0, missing=False, duplicate=False,
            undefined=False, zero_size=False):
    """Extend the minimal fixture with .got and section-backed RELA evidence."""
    definitions = [('right_data', target, 0 if zero_size else 32),
                   ('wrong_data', target + 0x100, 32)]
    BASE_WRITER(path, functions, definitions)
    data = bytearray(path.read_bytes())
    shoff = struct.unpack_from('<Q', data, 40)[0]
    count = struct.unpack_from('<H', data, 60)[0]
    sections = [list(struct.unpack_from('<IIQQQQIIQQ', data, shoff + i * 64))
                for i in range(count)]
    names = bytes(data[sections[4][4]:sections[4][4] + sections[4][5]])
    got_name = len(names)
    names += b'.got\0'
    sections[4][4], sections[4][5] = len(data), len(names)
    data.extend(names)
    symtab, strings = sections[2], sections[3]
    sym_index = None
    for i in range(symtab[5] // 24):
        pos = symtab[4] + i * 24
        name = struct.unpack_from('<I', data, pos)[0]
        begin = strings[4] + name
        end = data.find(0, begin)
        if data[begin:end].decode() == symbol:
            sym_index = i
            if undefined:
                struct.pack_into('<H', data, pos + 6, 0)
    assert sym_index is not None
    if kind == 1027:
        addend, index = target + offset, 0
    else:
        addend, index = offset, sym_index
    relocations = b'' if missing else struct.pack('<QQq', slot, (index << 32) | kind, addend)
    if duplicate:
        relocations *= 2
    sections[5][4], sections[5][5] = len(data), len(relocations)
    data.extend(relocations)
    sections.append([got_name, 1, 3, slot, slot, 8, 0, 0, 8, 0])
    # Deliberately disagreeing in-place content proves RELA addend is authoritative.
    struct.pack_into('<Q', data, slot, 0xDEADBEEF)
    shoff = len(data)
    for section in sections:
        data.extend(struct.pack('<IIQQQQIIQQ', *section))
    struct.pack_into('<Q', data, 40, shoff)
    struct.pack_into('<H', data, 60, len(sections))
    struct.pack_into('<Q', data, 64 + 32, len(data))
    struct.pack_into('<Q', data, 64 + 40, len(data))
    path.write_bytes(data)


class GotIdentityTests(unittest.TestCase):
    def check_pair(self, original_options=None, rebuilt_options=None, known=True):
        original_options = original_options or {}
        rebuilt_options = rebuilt_options or {}
        test = fixtures.CheckerTests(methodName='runTest')
        test.setUp()
        self.addCleanup(test.doCleanups)

        def writer(path, functions, objects=()):
            original = path.parent.name == 'data'
            got_elf(path, functions, objects, slot=0x3500 if original else 0x4500,
                    target=0x3100 if original else 0x4100,
                    **(original_options if original else rebuilt_options))

        code1 = fixtures.words(fixtures.adrp(0x1000, 0x3500, 8),
                               0xF9400100 | ((0x500 // 8) << 10), fixtures.RET)
        code2 = fixtures.words(fixtures.adrp(0x2000, 0x4500, 8),
                               0xF9400100 | ((0x500 // 8) << 10), fixtures.RET)
        with patch.object(fixtures, 'write_elf', writer):
            return test.check([('reader', 0x1000, code1)], [('reader', 0x2000, code2)],
                              data=[('right_data', 0x3100)] if known else [])

    def test_relative_and_glob_dat_identity(self):
        for original_kind in (1025, 1027):
            for rebuilt_kind in (1025, 1027):
                with self.subTest(original=original_kind, rebuilt=rebuilt_kind):
                    self.assertEqual(self.check_pair({'kind': original_kind},
                                                    {'kind': rebuilt_kind})['status'], 'ok')

    def test_relative_wrong_target_and_glob_dat_wrong_name(self):
        for options in ({'offset': 0x100}, {'kind': 1025, 'symbol': 'wrong_data'}):
            with self.subTest(options=options):
                result = self.check_pair(rebuilt_options=options)
                self.assertEqual(result['status'], 'mismatch')
                self.assertIn('unverified GOT reference', result['reason'])

    def test_interior_offsets_must_be_preserved(self):
        for kind in (1025, 1027):
            for offset in (0, 8, 16):
                with self.subTest(kind=kind, rebuilt_offset=offset):
                    result = self.check_pair({'offset': 8}, {'kind': kind, 'offset': offset})
                    self.assertEqual(result['status'], 'ok' if offset == 8 else 'mismatch')

    def test_missing_duplicate_unsupported_or_undefined_evidence_rejected(self):
        for original in (False, True):
            for options in ({'missing': True}, {'duplicate': True}, {'kind': 999},
                            {'kind': 1025, 'undefined': True}):
                with self.subTest(original=original, options=options):
                    result = self.check_pair(options if original else {},
                                             {} if original else options)
                    self.assertEqual(result['status'], 'mismatch')
                    self.assertIn('unverified GOT reference', result['reason'])

    def test_unknown_original_identity_rejected(self):
        result = self.check_pair(known=False)
        self.assertEqual(result['status'], 'mismatch')
        self.assertIn('lacks known data identity', result['reason'])

    def test_zero_size_exact_base_has_identity(self):
        self.assertEqual(self.check_pair({'zero_size': True},
                                        {'zero_size': True})['status'], 'ok')


if __name__ == '__main__':
    unittest.main()
