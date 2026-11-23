#!/usr/bin/env python3
"""Adversarial real-ELF tests for bounded signed-offset switch recovery.

Run with D3_CHECKER pointing at the candidate checker. Every invocation uses
private inventories and a freshly hash-bound ELF/link map; no game data changes.
"""
import hashlib
import json
import struct
import subprocess
import unittest

import test_checker as common


def with_table(path, functions, address, entries, flags=2, declared_size=None, relocation=False, load_flags=5):
    """Add an allocated table section to the minimal ELF, preserving its symbols."""
    common.write_elf(path, functions)
    payload = bytearray(path.read_bytes())
    shoff = struct.unpack_from('<Q', payload, 40)[0]
    shnum = struct.unpack_from('<H', payload, 60)[0]
    sections = [list(struct.unpack_from('<IIQQQQIIQQ', payload, shoff + 64 * i))
                for i in range(shnum)]
    names = bytes(payload[sections[4][4]:sections[4][4] + sections[4][5]])
    name_offset = len(names)
    names += b'.rodata\0'
    sections[4][4:6] = [len(payload), len(names)]
    payload.extend(names)
    # Do not overlap the table with the synthetic executable section.
    sections[1][5] = max(addr + len(code) for _, addr, code in functions) - 0x1000
    raw = struct.pack('<' + 'i' * len(entries), *entries)
    payload[address:address + len(raw)] = raw
    sections.append([name_offset, 1, flags, address, address,
                     len(raw) if declared_size is None else declared_size, 0, 0, 4, 0])
    if relocation:
        sections[5][4:6] = [len(payload), 24]
        payload.extend(struct.pack('<QQq', address, 1027, 0))  # R_AARCH64_RELATIVE
    new_shoff = len(payload)
    for section in sections:
        payload.extend(struct.pack('<IIQQQQIIQQ', *section))
    struct.pack_into('<Q', payload, 40, new_shoff)
    struct.pack_into('<H', payload, 60, len(sections))
    # This fixture's code and tables are in a read/execute load segment.
    struct.pack_into('<I', payload, 68, load_flags)
    # Cover the appended section headers within the existing load segment.
    struct.pack_into('<QQ', payload, 96, len(payload), len(payload))
    path.write_bytes(payload)


def dispatch(at, table, *, guard=8, bound=1, before_guard=None,
             before_load=common.NOP, copy=0x2A0003E9, load=0xB8A9790A,
             target_add=0x8B08014A):
    # The spare instruction slots allow mutations without moving targets.
    # nop; cmp w0,#1; b.hi default; adrp/add x8; mov w9,w0;
    # nop; ldrsw x10,[x8,x9,lsl#2]; add x10,x10,x8; br x10.
    compare = 0x7100001F | (bound << 10)
    prefix = [common.NOP, compare] if before_guard is None else [compare, before_guard]
    return prefix + [
            0x54000000 | (((56 - 8) // 4) << 5) | guard,
            common.adrp(at + 12, table, 8), common.add_immediate(8, 8, table & 0xFFF),
            copy, before_load, load, target_add, 0xD61F0140,
            0x52800021, common.RET, 0x52800041, common.RET, common.RET]


class SwitchTests(unittest.TestCase):
    setUp = common.CheckerTests.setUp

    def check_switch(self, *, original_code=None, rebuilt_code=None,
                     original_targets=(40, 48), rebuilt_targets=(40, 48),
                     original_flags=2, rebuilt_flags=2, original_size=None,
                     rebuilt_size=None, owner='src/switch.cpp.obj', section='.rodata.switcher',
                     bind=True, rebuilt_table=0x4200, original_relocation=False,
                     rebuilt_relocation=False, original_load_flags=5, rebuilt_load_flags=5):
        original_code = original_code or dispatch(0x1000, 0x3100)
        rebuilt_code = rebuilt_code or dispatch(0x2000, rebuilt_table)
        original = [('switcher', 0x1000, common.words(*original_code))]
        rebuilt = [('switcher', 0x2000, common.words(*rebuilt_code))]
        original_path = self.root / 'data/main.elf'
        rebuilt_path = self.root / 'build/fixture'
        with_table(original_path, original, 0x3100,
                   [0x1000 + target - 0x3100 for target in original_targets],
                   original_flags, original_size, original_relocation, original_load_flags)
        with_table(rebuilt_path, rebuilt, rebuilt_table,
                   [0x2000 + target - rebuilt_table for target in rebuilt_targets],
                   rebuilt_flags, rebuilt_size, rebuilt_relocation, rebuilt_load_flags)
        (self.root / 'data/functions.csv').write_text(
            f'Address,Quality,Size,Name\n0x{common.BASE + 0x1000:x},O,{len(original_code) * 4},switcher\n')
        (self.root / 'data/data_symbols.csv').write_text('Address,Name\n')
        map_text = common.link_map([('switcher', 0x2000, 'src/switch.cpp.obj', len(rebuilt_code) * 4)])
        map_text += f'{rebuilt_table:16x} {rebuilt_table:16x} {len(rebuilt_targets) * 4:8x} {4:5d} ' + (
            '        ' + owner + ':(' + section + ')\n')
        map_path = self.root / 'build/fixture.map'
        map_path.write_text(map_text)
        if bind:
            (self.root / 'build/fixture.map.provenance').write_text(json.dumps({
                'format': 1, 'elf_sha256': hashlib.sha256(rebuilt_path.read_bytes()).hexdigest(),
                'map_sha256': hashlib.sha256(map_path.read_bytes()).hexdigest()}))
        else:
            (self.root / 'build/fixture.map.provenance').unlink(missing_ok=True)
        (self.root / 'request.json').write_text(json.dumps({'functions': [
            {'address': hex(common.BASE + 0x1000), 'name': 'switcher'}]}))
        run = subprocess.run([str(common.CHECKER), '--batch', 'request.json',
                              '--batch-output', 'result.json'], cwd=self.root,
                             capture_output=True, text=True, timeout=30)
        self.assertEqual(run.returncode, 0, run.stdout + run.stderr)
        return json.loads((self.root / 'result.json').read_text())['results'][0]

    def test_matching_relocated_switch(self):
        self.assertEqual(self.check_switch()['status'], 'ok')

    def test_loop_rechecks_guard_and_reconstructs_table_base(self):
        self.assertEqual(self.check_switch(original_targets=(0, 40),
                                           rebuilt_targets=(0, 40))['status'], 'ok')

    def test_table_base_can_be_hoisted_before_adjacent_guard(self):
        def hoisted(at, table):
            code = dispatch(at, table)
            code[:7] = [common.adrp(at, table, 8),
                        common.add_immediate(8, 8, table & 0xFFF),
                        0x7100041F, 0x54000168, 0x2A0003E9, common.NOP, common.NOP]
            return code
        self.assertEqual(self.check_switch(original_code=hoisted(0x1000, 0x3100),
                                           rebuilt_code=hoisted(0x2000, 0x4200))['status'], 'ok')

    def test_cycle_cannot_supply_its_own_guard_or_base(self):
        for bypass in (12, 20, 28):
            with self.subTest(bypass=bypass):
                self.assertEqual(self.check_switch(original_targets=(bypass, 40),
                                                   rebuilt_targets=(bypass, 40))['status'], 'mismatch')

    def test_unsigned_low_or_same_guard_uses_taken_edge(self):
        def taken(at, table):
            code = dispatch(at, table)
            code[2:7] = [0x54000049, common.branch(at + 12, at + 56, False),
                         common.adrp(at + 16, table, 8),
                         common.add_immediate(8, 8, table & 0xFFF), 0x2A0003E9]
            return code
        self.assertEqual(self.check_switch(original_code=taken(0x1000, 0x3100),
                                           rebuilt_code=taken(0x2000, 0x4200))['status'], 'ok')

    def test_changed_target_is_rejected(self):
        self.assertEqual(self.check_switch(rebuilt_targets=(48, 40))['status'], 'mismatch')

    def test_identical_instructions_do_not_hide_changed_table(self):
        # Matching code bytes cannot prove the out-of-line branch destinations.
        for targets, expected in (((40, 48), 'ok'), ((48, 40), 'mismatch')):
            with self.subTest(targets=targets):
                self.assertEqual(self.check_switch(rebuilt_table=0x4100,
                                                   rebuilt_targets=targets)['status'], expected)

    def test_unverified_switch_does_not_fall_back_to_code_equality(self):
        for kwargs in ({'rebuilt_flags': 3}, {'owner': 'src/unrelated.cpp.obj'},
                       {'rebuilt_size': 4}, {'bind': False}):
            with self.subTest(kwargs=kwargs):
                self.assertEqual(self.check_switch(rebuilt_table=0x4100,
                    rebuilt_targets=(48, 40), **kwargs)['status'], 'mismatch')

    def test_unknown_indirect_edge_cannot_bypass_switch_guard(self):
        original = dispatch(0x1000, 0x3100)
        rebuilt = dispatch(0x2000, 0x4100)
        # The default branch can re-enter the load with an unbounded index.
        # First two entries agree; an unchecked third entry deliberately differs.
        original[-1] = rebuilt[-1] = 0xD61F0160  # br x11
        self.assertEqual(self.check_switch(original_code=original, rebuilt_code=rebuilt,
            rebuilt_table=0x4100, original_targets=(40, 48, 40),
            rebuilt_targets=(40, 48, 48))['status'], 'mismatch')

    def test_guard_must_dominate_dispatch(self):
        for bypass in ('entry', 'taken_guard'):
            with self.subTest(bypass=bypass):
                original = dispatch(0x1000, 0x3100)
                rebuilt = dispatch(0x2000, 0x4200)
                if bypass == 'entry':
                    # A direct entry edge skips the textual CMP/B.HI entirely.
                    original[0] = rebuilt[0] = common.branch(0, 12, False)
                else:
                    # The supposedly out-of-range edge also reaches dispatch.
                    original[2] = rebuilt[2] = 0x54000028  # b.hi +4
                self.assertEqual(self.check_switch(original_code=original,
                                                   rebuilt_code=rebuilt)['status'], 'mismatch')

    def test_changed_guard_count_is_rejected(self):
        self.assertEqual(self.check_switch(rebuilt_code=dispatch(0x2000, 0x4200, bound=0))['status'],
                         'mismatch')

    def test_invalid_table_targets_are_rejected(self):
        for targets in ((40, 41), (40, 60), (40, -4)):
            with self.subTest(targets=targets):
                self.assertEqual(self.check_switch(original_targets=targets,
                                                   rebuilt_targets=targets)['status'], 'mismatch')

    def test_truncated_tables_are_rejected(self):
        for key in ('original_size', 'rebuilt_size'):
            with self.subTest(key=key):
                self.assertEqual(self.check_switch(**{key: 4})['status'], 'mismatch')

    def test_tables_require_read_only_allocated_sections(self):
        for key in ('original_flags', 'rebuilt_flags'):
            for flags in (0, 3, 6):
                with self.subTest(key=key, flags=flags):
                    self.assertEqual(self.check_switch(**{key: flags})['status'], 'mismatch')

    def test_read_only_section_in_writable_load_segment_is_rejected(self):
        for key in ('original_load_flags', 'rebuilt_load_flags'):
            with self.subTest(key=key):
                self.assertEqual(self.check_switch(**{key: 7})['status'], 'mismatch')

    def test_loader_relocations_cannot_rewrite_proved_table(self):
        for key in ('original_relocation', 'rebuilt_relocation'):
            with self.subTest(key=key):
                self.assertEqual(self.check_switch(**{key: True})['status'], 'mismatch')

    def test_table_requires_correct_bound_map_owner(self):
        for kwargs in ({'owner': 'src/unrelated.cpp.obj'}, {'section': '.rodata.unrelated'},
                       {'bind': False}):
            with self.subTest(kwargs=kwargs):
                self.assertEqual(self.check_switch(**kwargs)['status'], 'mismatch')

    def test_guard_must_bound_the_actual_index(self):
        for change in ({'guard': 12},  # signed GT admits large unsigned indices
                       {'before_guard': 0x52800FE0},  # mov w0,#127 preserves flags but loses bound
                       {'before_guard': 0x7100043F},  # cmp w1,#1 overwrites NZCV
                       {'before_load': 0x91000508},  # add x8,x8,#1 corrupts table base
                       {'before_load': 0x11000529},  # add w9,w9,#1 after bounds check
                       {'copy': 0x2A0103E9},  # mov w9,w1: wrong source
                       {'copy': 0xAA0003E9}):  # mov x9,x0 does not zero high bits
            with self.subTest(change=change):
                self.assertEqual(self.check_switch(
                    original_code=dispatch(0x1000, 0x3100, **change),
                    rebuilt_code=dispatch(0x2000, 0x4200, **change))['status'], 'mismatch')

    def test_dispatch_registers_and_scale_are_semantic(self):
        for change in ({'load': 0xB8A9690A},  # no LSL #2
                       {'load': 0xB8AB790A},  # different index x11
                       {'target_add': 0x8B08054A}):  # shifted ADD
            with self.subTest(change=change):
                self.assertEqual(self.check_switch(
                    original_code=dispatch(0x1000, 0x3100, **change),
                    rebuilt_code=dispatch(0x2000, 0x4200, **change))['status'], 'mismatch')


if __name__ == '__main__':
    unittest.main(verbosity=2)
