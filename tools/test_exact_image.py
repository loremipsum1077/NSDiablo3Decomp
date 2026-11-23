import hashlib
from pathlib import Path
import struct
import tempfile
import unittest
from unittest.mock import patch

import exact_image as e


def literal_block(data):
    result = bytearray([min(len(data), 15) << 4])
    if len(data) >= 15:
        remaining = len(data) - 15
        while remaining >= 255:
            result.append(255)
            remaining -= 255
        result.append(remaining)
    return bytes(result) + data


def nso(compressed=False):
    result = bytearray(256)
    result[:4] = b'NSO0'
    struct.pack_into('<I', result, 12, 56 | (7 if compressed else 0))
    for i, data in enumerate((b'instructions and padding', b'constants\0', b'globals')):
        stored = literal_block(data) if compressed else data
        struct.pack_into('<IIII', result, 16 + 16 * i, len(result), i * 4096,
                         len(data), 256 if i == 0 else 0 if i == 1 else 16)
        struct.pack_into('<I', result, 96 + 4 * i, len(stored))
        result[160 + 32 * i:192 + 32 * i] = hashlib.sha256(data).digest()
        result.extend(stored)
    return bytes(result)


class ExactImageTests(unittest.TestCase):
    def compare(self, reference, candidate, **kwargs):
        with tempfile.TemporaryDirectory() as directory:
            old, new = Path(directory) / 'old', Path(directory) / 'new'
            old.write_bytes(reference)
            new.write_bytes(candidate)
            return e.compare(old, new, {'sha256': e.sha(reference), 'size': len(reference)}, **kwargs)

    def test_equal_bytes_are_not_source_certification(self):
        report = self.compare(nso(True), nso(True))
        self.assertTrue(report['container_bytes_equal'])
        self.assertTrue(report['declared_segment_bytes_equal'])
        self.assertFalse(report['accepted'])
        self.assertEqual(report['source_and_build_lineage'], 'not_assessed')

    def test_same_loaded_segments_different_packaging_fails(self):
        report = self.compare(nso(True), nso(False))
        self.assertTrue(report['declared_segment_bytes_equal'])
        self.assertTrue(report['declared_memory_layout_equal'])
        self.assertTrue(report['bss_equal'])
        self.assertFalse(report['header_bytes_equal'])
        self.assertFalse(report['container_bytes_equal'])

    def test_header_padding_and_tail_bytes_cannot_be_ignored(self):
        original = nso()
        for offset in (128, 255):
            changed = bytearray(original)
            changed[offset] ^= 1
            report = self.compare(original, changed)
            self.assertFalse(report['container_bytes_equal'])
            self.assertEqual(report['differences']['differing_bytes'], 1)
        report = self.compare(original, original + b'ignored?')
        self.assertFalse(report['container_bytes_equal'])
        self.assertEqual(report['differences']['differing_bytes'], 8)

    def test_hash_layout_and_bss_differences(self):
        original = nso()
        corrupt = bytearray(original)
        corrupt[-1] ^= 1
        result = self.compare(original, corrupt)
        self.assertFalse(result['candidate_nso']['supported'])
        self.assertIn('hash mismatch', result['candidate_nso']['error'])
        for offset, field in ((52, 'declared_memory_layout_equal'), (60, 'bss_equal')):
            changed = bytearray(original)
            value = struct.unpack_from('<I', changed, offset)[0]
            struct.pack_into('<I', changed, offset, value + 32)
            result = self.compare(original, changed)
            self.assertFalse(result[field])
            self.assertTrue(result['declared_segment_bytes_equal'])

    def test_unsupported_candidate_remains_a_mismatch(self):
        result = self.compare(nso(), b'\x7fELF')
        self.assertFalse(result['container_bytes_equal'])
        self.assertFalse(result['candidate_nso']['supported'])

    def test_original_pin_is_required(self):
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / 'image'
            path.write_bytes(nso())
            with self.assertRaisesRegex(e.ImageError, 'pin mismatch'):
                e.compare(path, path, {'size': len(nso()), 'sha256': '0' * 64})

    def test_lz4_overlap_extensions_and_empty(self):
        # One literal + 15 overlapping match bytes, then five trailing literals.
        self.assertEqual(e.decode_lz4(b'\x1ba\x01\x00\x50bcdef', 21), b'a' * 16 + b'bcdef')
        for payload in (b'', b'abc', b'x' * 15, b'y' * 300):
            self.assertEqual(e.decode_lz4(literal_block(payload), len(payload)), payload)

    def test_lz4_rejects_corruption_and_bombs(self):
        cases = [(b'', 0), (b'\xf0', 30), (b'\x30ab', 3),
                 (b'\x10a\x00\x00\x50bcdef', 10),
                 (b'\x10a\x02\x00\x50bcdef', 10),
                 (b'\x1fa\x01\x00\xff\xff\x00', 20),
                 (b'\x10a\x01', 5), (b'\x10a\x01\x00', 5),
                 (b'\x10a\x01\x00\x10z', 6)]
        for block, size in cases:
            with self.subTest(block=block), self.assertRaises(e.ImageError):
                e.decode_lz4(block, size)
        with self.assertRaises(e.ImageError):
            e.decode_lz4(b'\0', e.MAX_DECODED + 1)

    def test_nso_ranges_flags_and_aggregate_bound(self):
        original = nso()
        mutations = [(12, 0x80), (16, 0), (32, 256), (36, 0),
                     (20, 0xffffffff), (60, 0xffffffff), (28, 257), (44, 1)]
        for offset, value in mutations:
            changed = bytearray(original)
            struct.pack_into('<I', changed, offset, value)
            # With a nonempty module name, its offset overlaps the first segment.
            if offset == 28:
                struct.pack_into('<I', changed, 44, 1)
            with self.subTest(offset=offset), self.assertRaises(e.ImageError):
                e.inspect_nso(changed)
        with patch.object(e, 'MAX_DECODED', 10), self.assertRaisesRegex(e.ImageError, 'aggregate'):
            e.inspect_nso(original)

    def test_diff_ranges_count_all_but_bound_output(self):
        result = e.differences(b'abcde', b'axcyeTAIL', 1)
        self.assertEqual(result['differing_bytes'], 6)
        self.assertEqual(result['range_count'], 3)
        self.assertEqual(result['ranges'], [{'start': 1, 'end_exclusive': 2}])
        self.assertTrue(result['ranges_truncated'])
        self.assertEqual(e.differences(b'ab', b'a', 4)['ranges'], [{'start': 1, 'end_exclusive': 2}])

    def test_diff_crosses_chunk_boundary(self):
        original = b'a' * (1024 * 1024 + 3)
        candidate = original[:1024 * 1024 - 1] + b'bb' + original[1024 * 1024 + 1:]
        result = e.differences(original, candidate, 5)
        self.assertEqual(result['ranges'], [{'start': 1024 * 1024 - 1, 'end_exclusive': 1024 * 1024 + 1}])

    def test_stable_reader_refuses_symlink_size_and_race(self):
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / 'original'
            path.write_bytes(b'abc')
            link = Path(directory) / 'link'
            link.symlink_to(path)
            with self.assertRaises(e.ImageError):
                e.stable_read(link)
            with patch.object(e, 'MAX_FILE', 2), self.assertRaises(e.ImageError):
                e.stable_read(path)
            before = path.stat()
            changed = Path(directory) / 'changed'
            changed.write_bytes(b'1234')
            with patch.object(e.os, 'fstat', side_effect=[before, changed.stat()]), self.assertRaises(e.ImageError):
                e.stable_read(path)


if __name__ == '__main__':
    unittest.main()
