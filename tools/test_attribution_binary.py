#!/usr/bin/env python3
"""Synthetic ELF tests; no game image or optional packages needed."""
import hashlib
import json
from pathlib import Path
import struct
import tempfile
import unittest

from attribution_binary import BinaryImage, collect_binary_evidence, LOAD_BIAS
from test_checker import write_elf, words, RET, NOP


class BinaryEvidenceTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.path = Path(self.temp.name) / 'original.elf'
        write_elf(self.path, [('first', 0x1000, words(RET)), ('alias', 0x1000, words(RET, NOP)),
                              ('next', 0x1008, words(RET))])

    def mutate(self, offset, fmt, *values):
        blob = bytearray(self.path.read_bytes())
        struct.pack_into(fmt, blob, offset, *values)
        self.path.write_bytes(blob)

    def test_aliases_extents_bias_and_hash(self):
        result = collect_binary_evidence(self.path, {LOAD_BIAS + 0x1000: 8}, include_hashes=True)
        record = result['functions'][0]
        self.assertEqual([(s['name'], s['size']) for s in record['symbols']], [('alias', 8), ('first', 4)])
        self.assertEqual(record['sha256'], hashlib.sha256(words(RET, NOP)).hexdigest())
        self.assertEqual(result['images'][0]['load_bias'], LOAD_BIAS)
        self.assertTrue(record['sections'][0]['contains_extent'])
        self.assertEqual(json.dumps(result, sort_keys=True), json.dumps(collect_binary_evidence(
            self.path, {LOAD_BIAS + 0x1000: 8}, include_hashes=True), sort_keys=True))

    def test_nss_only_and_missing_images(self):
        result = collect_binary_evidence(None, [0x1000], nss_path=self.path, load_bias=0)
        self.assertEqual(result['functions'][0]['symbols'][0]['image'], 'nss')
        with self.assertRaises(ValueError):
            collect_binary_evidence(None, [])

    def test_relocations_use_addends_and_half_open_ranges(self):
        image = BinaryImage(self.path, 'elf', 0)
        rel = next(s for s in image.sections if s['name'] == '.rela.dyn')
        blob = bytearray(self.path.read_bytes())
        shoff = struct.unpack_from('<Q', blob, 40)[0]
        data_offset = len(blob)
        # ABS64 to first + 4; RELATIVE to start; unsupported CALL26 is unresolved.
        for location, symbol, kind, addend in [(0x2000, 1, 257, 4), (0x2008, 0, 1027, 0x1000),
                                                (0x2010, 1, 283, 0), (0x2018, 1, 257, 8)]:
            blob.extend(struct.pack('<QQq', location, (symbol << 32) | kind, addend))
        struct.pack_into('<QQ', blob, shoff + rel['index'] * 64 + 24, data_offset, 96)
        self.path.write_bytes(blob)
        result = collect_binary_evidence(self.path, {0x1000: 8, 0x2000: 32}, load_bias=0)
        incoming = result['functions'][0]['relocations_targeting']
        self.assertEqual([r['target'] for r in incoming], [0x1000, 0x1004])
        outgoing = result['functions'][1]['relocations_at_or_within']
        self.assertIsNone(outgoing[2]['target'])
        self.assertEqual(outgoing[3]['target'], 0x1008)

    def test_nonallocated_symbol_not_code_address(self):
        image = BinaryImage(self.path, 'elf', 0)
        sym = next(s for s in image.sections if s['name'] == '.symtab')
        self.mutate(sym['offset'] + 24 + 6, '<H', 3)  # .strtab is nonallocated
        record = collect_binary_evidence(self.path, [0x1000], load_bias=0)['functions'][0]
        self.assertEqual([s['name'] for s in record['symbols']], ['alias'])

    def test_bad_architecture_type_and_truncation(self):
        original = self.path.read_bytes()
        for offset, value in [(18, 62), (16, 1)]:
            self.path.write_bytes(original)
            self.mutate(offset, '<H', value)
            with self.assertRaises(ValueError):
                BinaryImage(self.path, 'elf', 0)
        self.path.write_bytes(original[:80])
        with self.assertRaises(ValueError):
            BinaryImage(self.path, 'elf', 0)

    def test_original_and_nss_bytes_must_agree(self):
        other = self.path.with_name('symbols.nss')
        other.write_bytes(self.path.read_bytes())
        result = collect_binary_evidence(self.path, {0x1000: 8}, other, load_bias=0)
        self.assertEqual(result['functions'][0]['byte_comparison']['status'], 'identical')
        self.mutate(0x1004, '<I', RET)
        with self.assertRaisesRegex(ValueError, 'function bytes differ'):
            collect_binary_evidence(self.path, {0x1000: 8}, other, load_bias=0)
        result = collect_binary_evidence(self.path, [0x1000], other, load_bias=0)
        self.assertEqual(result['functions'][0]['byte_comparison']['status'], 'unavailable')

    def test_invalid_sizes(self):
        for values in [{0: -1}, {-1: 4}, {'0x1000': 4}]:
            with self.assertRaises(ValueError):
                collect_binary_evidence(self.path, values)

    def test_nobits_extent_cannot_corroborate_function_bytes(self):
        other = self.path.with_name('symbols.nss')
        other.write_bytes(self.path.read_bytes())
        blob = bytearray(other.read_bytes())
        shoff = struct.unpack_from('<Q', blob, 40)[0]
        struct.pack_into('<I', blob, shoff + 64 + 4, 8)  # .text -> SHT_NOBITS
        other.write_bytes(blob)
        with self.assertRaisesRegex(ValueError, 'function bytes unavailable'):
            collect_binary_evidence(self.path, {0x1000:8}, other, load_bias=0)


if __name__ == '__main__':
    unittest.main()
