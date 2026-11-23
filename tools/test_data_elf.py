"""Synthetic binary evidence regressions; no game image or compiler required."""
from pathlib import Path
import struct
import tempfile
import unittest


def write_data_elf(path, *, symbols=None, relocations=(), data=b'\x00' * 32,
                   section_overrides=None):
    """Write ELF64/AArch64 with file data, zero-fill, imports and lifecycle arrays.

    Symbols are (name, section index, value, size, ELF info); relocation tuples
    are (location, type, symbol index, addend). Section indexes are stable.
    """
    symbols = symbols if symbols is not None else [
        ('table', 1, 0x1000, 16, 0x11),
        ('alias', 1, 0x1000, 16, 0x11),
        ('tail', 1, 0x1008, 16, 0x11),
        ('zero', 2, 0x2000, 32, 0x11),
        ('external', 0, 0, 0, 0x12),
        ('_ZTV4Test', 1, 0x1018, 8, 0x11)]
    names = b'\0.data\0.bss\0.init_array\0.fini_array\0.dynsym\0.dynstr\0.rela.dyn\0.shstrtab\0'
    strings = bytearray(b'\0')
    symbytes = bytearray(24)
    for name, sec, value, size, info in symbols:
        name_offset = len(strings)
        strings.extend(name.encode() + b'\0')
        symbytes.extend(struct.pack('<IBBHQQ', name_offset, info, 0, sec, value, size))
    relbytes = b''.join(struct.pack('<QQq', addr, (sym << 32) | typ, addend)
                        for addr, typ, sym, addend in relocations)
    payload = bytearray(64)
    sections = [(0,) * 10]
    specs = [('.data', 1, 3, 0x1000, data, 0, 0),
             ('.bss', 8, 3, 0x2000, 32, 0, 0),
             ('.init_array', 14, 3, 0x3000, b'\0' * 8, 0, 8),
             ('.fini_array', 15, 3, 0x3010, b'\0' * 8, 0, 8),
             ('.dynsym', 11, 2, 0x4000, symbytes, 6, 24),
             ('.dynstr', 3, 2, 0x5000, strings, 0, 0),
             ('.rela.dyn', 4, 2, 0x6000, relbytes, 5, 24),
             ('.shstrtab', 3, 0, 0, names, 0, 0)]
    for name, typ, flags, addr, content, link, entsize in specs:
        offset = len(payload)
        size = content if typ == 8 else len(content)
        if typ != 8:
            payload.extend(content)
        sections.append((names.index(name.encode()), typ, flags, addr,
                         offset, size, link, 1 if typ == 11 else 0, 8, entsize))
    for index, changes in (section_overrides or {}).items():
        section = list(sections[index])
        for field, value in changes.items():
            section[field] = value
        sections[index] = tuple(section)
    shoff = len(payload)
    for section in sections:
        payload.extend(struct.pack('<IIQQQQIIQQ', *section))
    ident = b'\x7fELF' + bytes([2, 1, 1, 0]) + bytes(8)
    payload[:64] = struct.pack('<16sHHIQQQIHHHHHH', ident, 3, 183, 1,
                               0, 0, shoff, 0, 64, 56, 0, 64, len(sections), 8)
    path.write_bytes(payload)
    return path


class ELFTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.path = Path(self.temp.name) / 'fixture.elf'

    def inspect(self, **kwargs):
        from data_elf import inspect_elf
        return inspect_elf(write_data_elf(self.path, **kwargs))

    def test_symbols_preserve_aliases_and_undefined_imports(self):
        result = self.inspect()
        aliases = [s['name'] for s in result['symbols'] if s['address'] == 0x1000]
        self.assertEqual(aliases, ['table', 'alias'])
        external = next(s for s in result['symbols'] if s['name'] == 'external')
        self.assertFalse(external['defined'])
        self.assertEqual(external['size'], 0)

    def test_nobits_is_memory_extent_not_file_content(self):
        result = self.inspect(section_overrides={2: {4: 0xfffffff, 5: 0x100000}})
        bss = result['sections'][2]
        self.assertEqual(bss['size'], 0x100000)
        self.assertEqual(bss['type'], 8)

    def test_relocation_evidence_distinguishes_relative_and_import(self):
        result = self.inspect(relocations=[(0x3000, 1027, 0, 0x1234),
                                           (0x3010, 1026, 5, 0)])
        relative, imported = result['relocations']
        self.assertEqual(relative['target'], 0x1234)
        self.assertEqual(relative['target_basis'], 'load_bias_relative')
        self.assertEqual(imported['symbol_name'], 'external')
        self.assertNotIn('target', imported)
        self.assertEqual([a['kind'] for a in result['arrays']], ['init', 'fini'])
        self.assertEqual(result['arrays'][0]['entries'][0]['relocations'], [relative])
        self.assertEqual(result['arrays'][1]['entries'][0]['relocations'], [imported])

    def test_malformed_tables_and_links_fail_closed(self):
        from data_elf import ELFError
        variants = [{1: {4: 0xfffffff}}, {5: {9: 16}}, {5: {5: 25}},
                    {5: {6: 99}}, {5: {6: 1}}, {7: {6: 1}},
                    {7: {9: 16}}, {3: {5: 7}}, {1: {8: 3}}]
        for overrides in variants:
            with self.subTest(overrides=overrides), self.assertRaises(ELFError):
                self.inspect(section_overrides=overrides)
        with self.assertRaises(ELFError):
            self.inspect(relocations=[(0x1000, 257, 99, 0)])
        with self.assertRaises(ELFError):
            self.inspect(symbols=[('invalid', 99, 0x1000, 8, 0x11)])

    def test_truncated_header_section_table_and_strings_rejected(self):
        from data_elf import ELFError, inspect_elf
        for length in (0, 4, 63, 70, -1):
            write_data_elf(self.path)
            original = self.path.read_bytes()
            self.path.write_bytes(original[:length])
            with self.subTest(length=length), self.assertRaises(ELFError):
                inspect_elf(self.path)
        result = self.inspect()
        blob = bytearray(self.path.read_bytes())
        strings = result['sections'][6]
        blob[strings['offset'] + strings['size'] - 1] = ord('x')
        self.path.write_bytes(blob)
        with self.assertRaises(ELFError):
            inspect_elf(self.path)

    def test_non_linked_elf_rejected(self):
        from data_elf import ELFError, inspect_elf
        write_data_elf(self.path)
        blob = bytearray(self.path.read_bytes())
        struct.pack_into('<H', blob, 16, 1)  # ET_REL offsets are section-relative.
        self.path.write_bytes(blob)
        with self.assertRaises(ELFError):
            inspect_elf(self.path)

    def test_relative_relocation_with_symbol_is_not_claimed_resolved(self):
        from data_elf import ELFError
        try:
            result = self.inspect(relocations=[(0x1000, 1027, 1, 0x1234)])
        except ELFError:
            return
        self.assertNotIn('target', result['relocations'][0])


if __name__ == '__main__':
    unittest.main()
