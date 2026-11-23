"""Real tiny ELF fixtures for stable recovery call-failure evidence; no builds."""
import copy
from pathlib import Path
import struct
import tempfile
import unittest

import recovery
from validation import common as c
from validation.recovery_diagnostics import CallFailureEvidence
from test_checker import write_elf, words, branch, RET, adrp, add_immediate


def diagnostic(actual=0x2000, expected=0x3000):
    reason = ('mismatch at 0x1000: wrong function call\n'
              'incorrect reference; expected to see 0x3000 expected\n'
              f'--> decomp source code is referencing {actual:#x} actual\n')
    if expected is not None:
        reason += f'--> expected to see {expected:#x} to match original code\n'
    return dict(address='0x0000007100001000', name='caller', status='mismatch', reason=reason)


def write_object(path, code=None, text_rela=((0, 283, 2, 0),), debug=b'\x01DWARF', debug_rela=((0, 257, 1, 0),),
                 caller_info=0x12, debug_flags=0, debug_type=1, debug_rela_name='.rela.debug_info',
                 debug_rela_link=5, debug_rela_info=3, bss_size=16, sizes=None, e_type=1, machine=183,
                 e_flags=0, phnum=0):
    """Minimal ET_REL caller object: .text + .rela.text, .debug_info + .rela.debug_info, .bss.

    Symbols: 0 null, 1 local SECTION(.text), 2 global undefined callee, 3 global caller.
    sizes overrides sh_size by section index to build truncated/malformed tables.
    """
    code = words(branch(0, 0x1000)) if code is None else code
    rela = lambda rows: b''.join(struct.pack('<QQq', off, (sym << 32) | kind, add) for off, kind, sym, add in rows)
    strtab = b'\0callee\0caller\0'
    symtab = bytes(24) + struct.pack('<IBBHQQ', 0, 0x03, 0, 1, 0, 0) \
        + struct.pack('<IBBHQQ', 1, 0x10, 0, 0, 0, 0) + struct.pack('<IBBHQQ', 8, caller_info, 0, 1, 0, len(code))
    names = b'\0.text\0.rela.text\0.debug_info\0.rela.debug_info\0.symtab\0.strtab\0.shstrtab\0.bss\0.rela.debug_line\0'
    n = lambda name: names.index(b'\0' + name.encode() + b'\0') + 1
    # (name, type, flags, content, link, info, align, entsize)
    layout = [('.text', 1, 6, code, 0, 0, 4, 0), ('.rela.text', 4, 0x40, rela(text_rela), 5, 1, 8, 24),
              ('.debug_info', debug_type, debug_flags, debug, 0, 0, 1, 0),
              (debug_rela_name, 4, 0x40, rela(debug_rela), debug_rela_link, debug_rela_info, 8, 24),
              ('.symtab', 2, 0, symtab, 6, 2, 8, 24), ('.strtab', 3, 0, strtab, 0, 0, 1, 0),
              ('.shstrtab', 3, 0, names, 0, 0, 1, 0), ('.bss', 8, 3, b'', 0, 0, 8, 0)]
    data = bytearray(64); headers = [bytes(64)]
    for index, (name, kind, flags, content, link, info, align, entsize) in enumerate(layout, 1):
        while len(data) % 8: data.append(0)
        size = (sizes or {}).get(index, bss_size if kind == 8 else len(content))
        headers.append(struct.pack('<IIQQQQIIQQ', n(name), kind, flags, 0, len(data), size, link, info, align, entsize))
        data.extend(content)
    while len(data) % 8: data.append(0)
    shoff = len(data); data.extend(b''.join(headers))
    data[:64] = struct.pack('<16sHHIQQQIHHHHHH', b'\x7fELF' + bytes([2, 1, 1, 0]) + bytes(8),
                            e_type, machine, 1, 0, 64 if phnum else 0, shoff, e_flags, 64, 56 if phnum else 0,
                            phnum, 64, len(headers), 7)
    path.write_bytes(data)

def add_plt(path, address, name='actual', kind=1026, addend=0, wrong_slot=False):
    """Add a genuine .dynsym/.rela.plt and executable AArch64 ABI PLT stub."""
    data = bytearray(path.read_bytes())
    shoff = struct.unpack_from('<Q', data, 40)[0]
    count = struct.unpack_from('<H', data, 60)[0]
    sections = [struct.unpack_from('<IIQQQQIIQQ', data, shoff + i * 64) for i in range(count)]
    names = bytearray(data[sections[4][4]:sections[4][4] + sections[4][5]])
    def add(section_name, kind, content, link=0, entry=0):
        no = len(names); names.extend(section_name.encode() + b'\0')
        offset = len(data); data.extend(content)
        sections.append((no, kind, 0, 0, offset, len(content), link, 0, 8, entry))
    dynstr = b'\0' + name.encode() + b'\0'
    dynsym = bytes(24) + struct.pack('<IBBHQQ', 1, 0x12, 0, 0, 0, 0)
    # Indices are relative to the initial fixture's seven sections.
    add('.dynstr', 3, dynstr)
    add('.dynsym', 11, dynsym, link=7, entry=24)
    got = 0x5000
    add('.rela.plt', 4, struct.pack('<QQq', got, (1 << 32) | kind, addend), link=8, entry=24)
    start = address - 32
    sections[6] = (sections[6][0], 1, 6, start, start, 48, 0, 0, 16, 0)
    data[address:address + 16] = words(adrp(address, got, 16),
        0xf9400211 | (((got & 4095) // 8) << 10),
        add_immediate(16, 16, (got & 4095) + int(wrong_slot) * 8), 0xd61f0220)
    no = len(data); data.extend(names)
    sections[4] = (sections[4][0], 3, 0, 0, no, len(names), 0, 0, 1, 0)
    shoff = len(data)
    for section in sections:
        data.extend(struct.pack('<IIQQQQIIQQ', *section))
    struct.pack_into('<Q', data, 40, shoff)
    struct.pack_into('<H', data, 60, len(sections))
    struct.pack_into('<QQ', data, 64 + 32, len(data), len(data))
    path.write_bytes(data)


class StableCalls(unittest.TestCase):
    def setUp(self):
        temp = tempfile.TemporaryDirectory(); self.addCleanup(temp.cleanup)
        self.roots = {label: Path(temp.name)/label for label in ('baseline', 'candidate')}
        for root in self.roots.values():
            (root/'build/2.6.2').mkdir(parents=True); (root/'objects').mkdir()
            # The same retained relocatable caller object is used across both links.
            write_object(root/'objects/caller.obj')
        self.a, self.b = diagnostic(), diagnostic(0x2100, 0x3100)
        self.link()

    def link(self, absent=False, plt=False, kind=1026, addend=0, wrong_slot=False):
        for label, delta in (('baseline', 0), ('candidate', 0x100)):
            root = self.roots[label]; path = root/'build/2.6.2/d3'
            funcs = [('caller', 0x1000 + delta, words(branch(0x1000 + delta, 0x2000 + delta)))]
            if not plt: funcs.append(('actual', 0x2000 + delta, words(RET)))
            if not absent: funcs.append(('expected', 0x3000 + delta, words(RET)))
            write_elf(path, funcs)
            if plt: add_plt(path, 0x2000 + delta, kind=kind, addend=addend, wrong_slot=wrong_slot)

    def equal(self, a=None, b=None):
        ev = CallFailureEvidence(self.roots)
        result = ev.equivalent(a or self.a, b or self.b)
        return result, ev

    def test_defined_address_shift_retains_raw_witness(self):
        snapshot = copy.deepcopy((self.a, self.b))
        ok, ev = self.equal(); self.assertTrue(ok)
        self.assertEqual((self.a, self.b), snapshot)
        self.assertEqual(ev.witnesses[0]['before'], self.a)
        self.assertEqual(ev.witnesses[0]['after'], self.b)
        self.assertEqual(ev.witnesses[0]['resolved']['actual']['symbol'], 'actual')

    def test_plt_address_shift(self):
        self.link(plt=True)
        self.assertTrue(self.equal()[0])
        self.assertEqual(self.equal()[1].witnesses[0]['resolved']['actual']['relocation_type'], 1026)

    def test_absent_expected_stays_absent(self):
        self.link(absent=True)
        self.a, self.b = diagnostic(expected=None), diagnostic(0x2100, None)
        self.assertTrue(self.equal()[0])
        self.link()
        self.assertFalse(self.equal()[0])

    def test_changed_identity_kind_site_original_target_and_names(self):
        variants = [dict(self.b, status='not-checkable'), dict(self.b, name='other'), dict(self.b, address='0x1004')]
        for old, new in [('wrong function call', 'wrong data reference'), ('0x1000:', '0x1004:'),
                         ('see 0x3000 expected', 'see 0x3004 expected'), (' expected\n', ' other\n'),
                         (' actual\n', ' other\n'), (' actual\n', ' unknown\n')]:
            variants.append(dict(self.b, reason=self.b['reason'].replace(old, new)))
        variants.append(dict(self.b, reason=self.b['reason'] + 'extra detail\n'))
        variants.append(diagnostic(0x2100, None))
        for row in variants:
            with self.subTest(row=row): self.assertFalse(self.equal(b=row)[0])

    def test_bad_address_and_interior_target(self):
        for actual, expected in [(0x2104, 0x3100), (0x2100, 0x3104), (0x2200, 0x3100)]:
            with self.subTest(actual=actual, expected=expected):
                self.assertFalse(self.equal(b=diagnostic(actual, expected))[0])
        self.link(plt=True)
        self.assertFalse(self.equal(b=diagnostic(0x2104, 0x3100))[0])

    def candidate_object(self, **kwargs):
        write_object(self.roots['candidate']/'objects/caller.obj', **kwargs)
        return self.equal()

    def test_debug_only_caller_object_change_is_runtime_identical(self):
        before = c.sha(self.roots['baseline']/'objects/caller.obj')
        ok, ev = self.candidate_object(debug=b'\x02longer DWARF for a new member', debug_rela=((0, 257, 1, 0), (8, 257, 3, 4)))
        self.assertNotEqual(before, c.sha(self.roots['candidate']/'objects/caller.obj'))
        self.assertTrue(ok)
        self.assertEqual(len(ev.witnesses[0]['caller_object_runtime_sha256']), 64)

    def test_allocated_section_byte_change_rejected(self):
        self.assertFalse(self.candidate_object(code=words(branch(0, 0x1004)))[0])
        self.assertFalse(self.candidate_object(code=words(branch(0, 0x1000), RET))[0])
        # SHF_ALLOC is runtime content even when named like DWARF.
        write_object(self.roots['baseline']/'objects/caller.obj', debug_flags=2)
        self.assertFalse(self.candidate_object(debug=b'\x02', debug_flags=2)[0])

    def test_runtime_relocation_change_rejected(self):
        for rows in (((0, 282, 2, 0),), ((0, 283, 2, 4),), ((0, 283, 1, 0),), ((4, 283, 2, 0),), ()):
            with self.subTest(rows=rows):
                self.assertFalse(self.candidate_object(text_rela=rows)[0])

    def test_caller_object_symbol_change_rejected(self):
        for info in (0x22, 0x11, 0x02):
            with self.subTest(info=info):
                self.assertFalse(self.candidate_object(caller_info=info)[0])
        p = self.roots['candidate']/'objects/caller.obj'
        write_object(p)
        data = bytearray(p.read_bytes()); shoff = struct.unpack_from('<Q', data, 40)[0]
        symtab = struct.unpack_from('<IIQQQQIIQQ', data, shoff + 5 * 64)
        for delta, value in ((5, 2), (16, 8)):  # visibility, size of caller
            with self.subTest(delta=delta):
                changed = bytearray(data); changed[symtab[4] + 3 * 24 + delta] = value; p.write_bytes(changed)
                self.assertFalse(self.equal()[0])

    def both_objects(self, baseline, candidate):
        write_object(self.roots['baseline']/'objects/caller.obj', **baseline)
        write_object(self.roots['candidate']/'objects/caller.obj', **candidate)
        return self.equal()[0]

    def test_only_progbits_debug_sections_and_their_rela_are_exempt(self):
        changed_debug = dict(debug=b'\x02longer DWARF')
        changed_rela = dict(debug_rela=((0, 257, 3, 8),))
        cases = {
            'debug-named NOTE section bytes': ({'debug_type': 7}, changed_debug),
            'rela name does not match its target': ({'debug_rela_name': '.rela.debug_line'}, changed_rela),
            'debug rela not linked to the symtab': ({'debug_rela_link': 6}, changed_rela),
            'debug rela patches an allocated section': ({'debug_flags': 2}, changed_rela),
            'debug rela patches a non-PROGBITS section': ({'debug_type': 7}, changed_rela),
        }
        self.assertTrue(self.both_objects({}, changed_rela))
        for label, (shape, change) in cases.items():
            with self.subTest(label):
                self.assertFalse(self.both_objects(shape, dict(shape, **change)))

    def test_non_debug_section_size_and_elf_header_bound(self):
        for change in ({'bss_size': 32}, {'e_flags': 1}, {'machine': 62}):
            with self.subTest(change=change):
                self.assertFalse(self.both_objects({}, change))

    def test_malformed_or_unusual_caller_object_fails_closed(self):
        # Identical on both sides, so only the shape guards can reject them.
        for shape in ({'e_type': 2}, {'phnum': 1}, {'debug_rela_info': 99},
                      {'sizes': {2: 1 << 20}}, {'sizes': {3: 1 << 20}}):
            with self.subTest(shape=shape):
                self.assertFalse(self.both_objects(shape, shape))

    def test_missing_or_ambiguous_caller_object(self):
        p = self.roots['candidate']/'objects/caller.obj'
        (p.parent/'duplicate.obj').write_bytes(p.read_bytes())
        self.assertFalse(self.equal()[0])
        (p.parent/'duplicate.obj').unlink(); p.unlink()
        self.assertFalse(self.equal()[0])

    def test_changed_plt_relocation_type_addend_or_slot(self):
        for kwargs in ({'kind': 1025}, {'addend': 4}, {'wrong_slot': True}):
            with self.subTest(kwargs=kwargs):
                self.link(plt=True, **kwargs); self.assertFalse(self.equal()[0])

    def test_wrong_call_opcode_or_operand_even_with_same_diagnostic(self):
        p = self.roots['candidate']/'build/2.6.2/d3'
        for word in (branch(0x1100, 0x2100, link=False), branch(0x1100, 0x2200), RET):
            data = bytearray(p.read_bytes()); struct.pack_into('<I', data, 0x1100, word); p.write_bytes(data)
            self.assertFalse(self.equal()[0])

    def test_same_display_name_with_changed_endpoint_kind(self):
        p = self.roots['candidate']/'build/2.6.2/d3'
        write_elf(p, [('caller', 0x1100, words(branch(0x1100, 0x2100))), ('expected', 0x3100, words(RET))])
        add_plt(p, 0x2100)
        self.assertFalse(self.equal()[0])

    def test_symbol_binding_visibility_and_ambiguity(self):
        p = self.roots['candidate']/'build/2.6.2/d3'
        pristine = p.read_bytes()
        shoff = struct.unpack_from('<Q', pristine, 40)[0]
        sym = struct.unpack_from('<IIQQQQIIQQ', pristine, shoff + 2 * 64)
        # actual is symbol 2; binding and visibility must be stable, not just name.
        for delta, value in ((4, 0x22), (5, 2), (5, 0x80)):
            data = bytearray(pristine); data[sym[4] + 2 * 24 + delta] = value; p.write_bytes(data)
            self.assertFalse(self.equal()[0])
        # Same spelling as a FUNC and an OBJECT must not select the convenient one.
        write_elf(p, [('caller', 0x1100, words(branch(0x1100, 0x2100))),
                      ('actual', 0x2100, words(RET)), ('expected', 0x3100, words(RET))],
                  objects=[('actual', 0x4000, 8)])
        self.assertFalse(self.equal()[0])
        # Duplicate symbol tables are unsupported even if their records agree.
        data = bytearray(pristine)
        # Empty .rela.dyn header becomes another SYMTAB referencing the same table.
        struct.pack_into('<IIQQQQIIQQ', data, shoff + 5 * 64, *sym)
        p.write_bytes(data); self.assertFalse(self.equal()[0])

    def test_plt_symbol_metadata_changes(self):
        self.link(plt=True)
        p = self.roots['candidate']/'build/2.6.2/d3'
        pristine = p.read_bytes(); shoff = struct.unpack_from('<Q', pristine, 40)[0]
        dyn = struct.unpack_from('<IIQQQQIIQQ', pristine, shoff + 8 * 64)
        for delta, value in ((4, 0x22), (4, 0x10), (4, 0x11), (5, 2), (8, 4), (16, 4)):
            data = bytearray(pristine); data[dyn[4] + 24 + delta] = value; p.write_bytes(data)
            self.assertFalse(self.equal()[0])
        # Both NOTYPE imports remain supported with exact kind retained.
        for root in self.roots.values():
            path = root/'build/2.6.2/d3'; data = bytearray(path.read_bytes())
            shoff = struct.unpack_from('<Q', data, 40)[0]
            dyn = struct.unpack_from('<IIQQQQIIQQ', data, shoff + 8 * 64)
            data[dyn[4] + 24 + 4] = 0x10
            data[dyn[4] + 24 + 16] = 0
            path.write_bytes(data)
        self.assertTrue(self.equal()[0])

    def test_policy_integration_and_unknown_fail_closed(self):
        result = lambda row: {'format': 1, 'results': [row]}
        with self.assertRaises(c.Halt): recovery.compare(result(self.a), result(self.b), [])
        ev = CallFailureEvidence(self.roots)
        self.assertEqual(recovery.compare(result(self.a), result(self.b), [], call_evidence=ev), [self.a])
        a = dict(self.a, reason='unknown reason 0x1'); b = dict(a, reason='unknown reason 0x2')
        with self.assertRaises(c.Halt): recovery.compare(result(a), result(b), [], call_evidence=ev)
        self.assertEqual(recovery.compare(result(a), result(a), [], call_evidence=ev), [a])


if __name__ == '__main__':
    unittest.main()
