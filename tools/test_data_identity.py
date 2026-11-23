"""Proposed data identity derivation; synthetic inputs, no game image required."""
import struct
import unittest

import data_identity as di


def sym(name, address, size, typ=1, binding=1, section='RW', stt_file=None):
    return dict(name=name, address=address, size=size, type=typ, binding=binding,
                section=section, stt_file=stt_file)


class FakeNss(di.NssSymbols):
    def __init__(self, symbols):  # noqa: D401 - bypass ELF parsing
        import bisect  # noqa: F401
        import collections
        self.sha256 = 'x'
        self.symbols = sorted(symbols, key=lambda s: (s['address'], s['name']))
        self.starts = [s['address'] for s in self.symbols]
        self.name_count = collections.Counter(s['name'] for s in self.symbols)
        self.max_size = max((s['size'] for s in self.symbols), default=0)


class FakeOriginal:
    def __init__(self, sections, memory=None, relocated=()):
        self.sections = sections
        self.memory = memory or {}
        self.relocated = sorted(relocated)

    section_of = di.Original.section_of
    relocated_in = di.Original.relocated_in

    def read(self, address, size):
        for base, blob in self.memory.items():
            if base <= address < base + len(blob):
                return blob[address - base:address - base + size]
        return None


SECTIONS = [dict(name='.text', address=0x0, size=0x1000, flags=6),
            dict(name='.rodata', address=0x1000, size=0x1000, flags=2),
            dict(name='.data', address=0x2000, size=0x1000, flags=3),
            dict(name='.bss', address=0x3000, size=0x1000, flags=3)]


def deriver(symbols=(), dwarf=(), known=None, functions=None, files=None, memory=None,
            relocated=()):
    return di.Deriver(FakeOriginal(SECTIONS, memory, relocated), FakeNss(list(symbols)),
                      list(dwarf), known or {}, functions or {}, files or {})


class DecodeTests(unittest.TestCase):
    def test_adrp_page(self):
        # adrp x8, +0x10e5000 from pc 0x15d18 (from the original image)
        rd, page = di.decode_adrp(0x90008688, 0x15d18)
        self.assertEqual((rd, page), (8, 0x10e5000))

    def test_negative_adrp(self):
        insn = 0x90000000 | (0x7FFFF << 5) | (3 << 29) | 1   # imm = -1 page
        self.assertEqual(di.decode_adrp(insn, 0x5000), (1, 0x4000))

    def test_ldr_and_add(self):
        self.assertEqual(di.decode_ldr_uimm(0xF942A508), (8, 8, 0x548, 8))
        self.assertEqual(di.decode_ldr_uimm(0xB9400000), (0, 0, 0, 4))
        self.assertIsNone(di.decode_ldr_uimm(0x91000000))
        self.assertEqual(di.decode_add_imm(0x91001021), (1, 1, 4))

    def test_got_slot_pairs_preceding_adrp(self):
        code = struct.pack('<III', 0x90008688, 0xD503201F, 0xF942A508)
        orig = FakeOriginal(SECTIONS, {0x15d18: code})
        orig.word = lambda a: (lambda r: None if r is None or len(r) < 4 else
                               int.from_bytes(r, 'little'))(orig.read(a, 4))
        slot, info = di.got_slot_at(orig, 0x15d18, 0x15d20)
        self.assertEqual(slot, 0x10e5548)
        self.assertEqual(info['adrp'], 0x15d18)
        slot, info = di.got_slot_at(orig, 0x15d18, 0x15d18)
        self.assertEqual(slot, 0x10e5548)


class DeriveTests(unittest.TestCase):
    def test_known_csv_base_is_not_reproposed(self):
        d = deriver([sym('g', 0x2000, 8)], known={0x2000: 'g'}).derive(0x2000)
        self.assertEqual(d['source'], 'csv')
        self.assertIsNone(d['proposal'])

    def test_interior_of_known_base_is_range_gap(self):
        d = deriver([sym('g', 0x2000, 16)], known={0x2000: 'g'}).derive(0x2008)
        self.assertEqual((d['source'], d['offset']), ('csv_interior', 8))
        self.assertIsNone(d['proposal'])

    def test_one_past_extent_is_not_covered(self):
        d = deriver([sym('g', 0x2000, 16)]).derive(0x2010)
        self.assertEqual(d['source'], 'anonymous_writable')

    def test_zero_size_symbol_covers_only_its_base(self):
        dv = deriver([sym('z', 0x2000, 0)])
        self.assertEqual(dv.derive(0x2000)['source'], 'nss_symtab')
        self.assertEqual(dv.derive(0x2001)['source'], 'anonymous_writable')

    def test_unique_local_symbol_proposed_bare(self):
        d = deriver([sym('_ZL5sLock', 0x3000, 8, binding=0, section='ZI')]).derive(0x3000)
        self.assertEqual(d['proposal']['name'], '_ZL5sLock')
        self.assertIn('nss_symtab:local', d['proposal']['evidence'])

    def test_interior_proposal_names_base_extent(self):
        d = deriver([sym('tbl', 0x1100, 32, section='RO')]).derive(0x1108)
        self.assertEqual((d['proposal']['address'], d['offset']), (0x1100, 8))

    def test_duplicate_name_is_qualified_by_owner(self):
        symbols = [sym('_ZL1s', 0x3000, 8, binding=0, stt_file='A.cpp'),
                   sym('_ZL1s', 0x3010, 8, binding=0, stt_file='B.cpp')]
        dwarf = [dict(address=0x3010, name='s', linkage='_ZL1s', decl_file='X/B.cpp',
                      external=False, merged_offset=0, function=None),
                 dict(address=0x3000, name='s', linkage='_ZL1s', decl_file='X/A.cpp',
                      external=False, merged_offset=0, function=None)]
        d = deriver(symbols, dwarf, known={0x3000: '_ZL1s'}).derive(0x3010)
        self.assertEqual(d['proposal']['name'], '_ZL1s@B.cpp')

    def test_header_static_is_qualified_by_compile_unit_not_decl_file(self):
        symbols = [sym('_ZN3blzL6ignoreE', 0x3000, 1, binding=0),
                   sym('_ZN3blzL6ignoreE', 0x3010, 1, binding=0)]
        dwarf = [dict(address=a, name='ignore', linkage='_ZN3blzL6ignoreE', cu=cu,
                      decl_file='blz/tuple.h', external=False, merged_offset=0, function=None)
                 for a, cu in ((0x3000, 'X/A.cpp'), (0x3010, 'Y/A.cpp'))]
        d = deriver(symbols, dwarf).derive(0x3010)
        self.assertEqual(d['owner_file'], 'blz/tuple.h')
        self.assertEqual(d['proposal']['name'], '_ZN3blzL6ignoreE@Y/A.cpp')

    def test_same_unit_duplicates_are_not_proposed(self):
        symbols = [sym('_ZL1s', 0x3000, 8, binding=0, stt_file='A.cpp'),
                   sym('_ZL1s', 0x3010, 8, binding=0, stt_file='A.cpp')]
        self.assertIsNone(deriver(symbols).derive(0x3010)['proposal'])

    def test_duplicate_dwarf_only_name_is_qualified(self):
        dwarf = [dict(address=a, name='k', linkage='_ZL1k', cu=cu, decl_file='k.h',
                      external=False, merged_offset=0, function=None)
                 for a, cu in ((0x3000, 'X/A.cpp'), (0x3010, 'X/B.cpp'))]
        d = deriver(dwarf=dwarf).derive(0x3010)
        self.assertEqual(d['proposal']['name'], '_ZL1k@B.cpp')

    def test_duplicate_name_without_owner_is_not_proposed(self):
        symbols = [sym('_ZL1s', 0x3000, 8, binding=0), sym('_ZL1s', 0x3010, 8, binding=0)]
        d = deriver(symbols).derive(0x3010)
        self.assertIsNone(d['proposal'])

    def test_distinct_aliases_are_ambiguous(self):
        d = deriver([sym('a', 0x2000, 8), sym('b', 0x2000, 8)]).derive(0x2000)
        self.assertEqual(d['source'], 'ambiguous')
        self.assertIsNone(d['proposal'])

    def test_overlapping_extents_are_ambiguous(self):
        d = deriver([sym('outer', 0x2000, 32), sym('inner', 0x2008, 8)]).derive(0x2008)
        self.assertEqual(d['source'], 'ambiguous')

    def test_function_pointer_uses_functions_csv_name(self):
        symbols = [sym('_ZN1AD1Ev', 0x100, 16, typ=2, section='EX'),
                   sym('_ZN1AD2Ev', 0x100, 16, typ=2, section='EX')]
        d = deriver(symbols, functions={0x100: ('_ZN1AD2Ev', 16)}).derive(0x100)
        self.assertEqual((d['source'], d['proposal']['name']), ('function', '_ZN1AD2Ev'))
        self.assertEqual(d['aliases'], ['_ZN1AD1Ev'])

    def test_function_interior_is_not_proposed(self):
        d = deriver(functions={0x100: ('f', 32)}).derive(0x108)
        self.assertEqual(d['source'], 'function_interior')

    def test_dwarf_only_merged_global(self):
        dwarf = [dict(address=0x300c, name='Zero', linkage='_ZN2nnL4ZeroE', decl_file='a.h',
                      external=False, merged_offset=0xc, function=None)]
        d = deriver(dwarf=dwarf).derive(0x300c)
        self.assertEqual(d['source'], 'dwarf_variable')
        self.assertIn('+merged', d['proposal']['evidence'])

    def test_dwarf_internal_without_linkage_is_not_proposed(self):
        dwarf = [dict(address=0x3000, name='x', linkage=None, decl_file='a.c',
                      external=False, merged_offset=0, function=None)]
        d = deriver(dwarf=dwarf).derive(0x3000)
        self.assertEqual(d['source'], 'dwarf_variable')
        self.assertIsNone(d['proposal'])

    def test_conflicting_dwarf_names_are_ambiguous(self):
        dwarf = [dict(address=0x3000, name=n, linkage=n, decl_file='a.c', external=True,
                      merged_offset=0, function=None) for n in ('x', 'y')]
        self.assertEqual(deriver(dwarf=dwarf).derive(0x3000)['source'], 'ambiguous')

    def test_name_already_in_csv_elsewhere_not_reproposed(self):
        d = deriver([sym('g', 0x2010, 8)], known={0x2000: 'g'}).derive(0x2010)
        self.assertIsNone(d['proposal'])


class ContentShapeTests(unittest.TestCase):
    def shape(self, blob, relocated=()):
        return deriver(memory={0x1000: blob}, relocated=relocated).derive(0x1000)

    def test_string_literal(self):
        d = self.shape(b'hello\0world\0')
        self.assertEqual((d['source'], d['shape'], d['length']), ('anonymous_readonly', 'c_string', 6))

    def test_nul_is_ambiguous_shape(self):
        self.assertEqual(self.shape(b'\0abc')['shape'], 'nul')

    def test_relocated_bytes_excluded(self):
        self.assertEqual(self.shape(b'abcdefgh\0', relocated=[0x1000])['shape'], 'relocated_bytes')

    def test_binary_constant(self):
        self.assertEqual(self.shape(b'\x00\x00\x80\x3f'[2:] + b'\0')['shape'], 'binary')

    def test_writable_is_never_content_candidate(self):
        d = deriver(memory={0x2000: b'abc\0'}).derive(0x2000)
        self.assertEqual(d['source'], 'anonymous_writable')
        self.assertNotIn('shape', d)


class DwarfParseTests(unittest.TestCase):
    TEXT = '''0x0000000b: DW_TAG_compile_unit
              DW_AT_name\t("a.cpp")

0x0000002a:   DW_TAG_variable
                DW_AT_name\t("g")
                DW_AT_external\t(true)
                DW_AT_decl_file\t("D:/BuildServer/2/work/code/branches/2_6_2/X/a.cpp")
                DW_AT_location\t(DW_OP_addr 0x1035000)

0x00000040:   DW_TAG_subprogram
                DW_AT_linkage_name\t("_Z1fv")
                DW_AT_name\t("f")

0x00000050:     DW_TAG_variable
                  DW_AT_name\t("s")
                  DW_AT_location\t(DW_OP_addr 0x10f80c8)

0x00000060:     DW_TAG_variable
                  DW_AT_name\t("local")
                  DW_AT_location\t(DW_OP_fbreg -8)

0x00000070:     NULL

0x00000080:   DW_TAG_variable
                DW_AT_specification\t(0x00000099 "m")
                DW_AT_location\t(DW_OP_addr 0x16ff400, DW_OP_plus_uconst 0xc)
                DW_AT_linkage_name\t("_ZN1AL1mE")
'''

    def test_parse(self):
        records = di.parse_dwarf_variables(self.TEXT.splitlines(True))
        self.assertEqual([(r['name'], r['address']) for r in records],
                         [('g', 0x1035000), ('s', 0x10f80c8), ('m', 0x16ff40c)])
        self.assertEqual(records[0]['decl_file'], 'X/a.cpp')
        self.assertEqual({r['cu'] for r in records}, {'a.cpp'})
        self.assertTrue(records[0]['external'])
        self.assertEqual(records[1]['function'], '_Z1fv')
        self.assertIsNone(records[2]['function'])
        self.assertEqual(records[2]['merged_offset'], 0xc)
        self.assertEqual(records[2]['linkage'], '_ZN1AL1mE')

    def test_relative_unit_name_joins_comp_dir(self):
        text = ('0x0000000b: DW_TAG_compile_unit\n'
                '              DW_AT_name\t("CGame.cpp")\n'
                '              DW_AT_comp_dir\t("D:\\\\BuildServer\\\\2\\\\work\\\\code\\\\branches'
                '\\\\2_6_2\\\\X\\\\Client")\n\n'
                '0x0000002a:   DW_TAG_variable\n'
                '                DW_AT_name\t("g")\n'
                '                DW_AT_location\t(DW_OP_addr 0x10)\n')
        [record] = di.parse_dwarf_variables(text.splitlines(True))
        self.assertEqual(record['cu'], 'X/Client/CGame.cpp')


if __name__ == '__main__':
    unittest.main()
