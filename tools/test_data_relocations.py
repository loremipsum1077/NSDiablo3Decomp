"""Synthetic relocation proofs independent of proprietary images and compilers."""
from pathlib import Path
import struct
import tempfile
import unittest

from data_relocations import RelocationImage, compare_objects
from test_data_elf import write_data_elf


class RelocationTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)

    def image(self, label, *, address=0x1000, target=0x2000, symbols=None,
              relocations=None, data=b'\0' * 32, overrides=None, load_bias=0):
        if symbols is None:
            symbols = [('table', 1, address, 16, 0x11),
                       ('target', 2, target, 32, 0x11)]
        if relocations is None:
            relocations = [(address, 1027, 0, target)]
        sections = {1: {3: address}, 2: {3: target}}
        for index, changes in (overrides or {}).items():
            sections.setdefault(index, {}).update(changes)
        path = write_data_elf(self.root / (label + '.elf'), symbols=symbols,
                              relocations=relocations, data=data,
                              section_overrides=sections)
        return RelocationImage(path, label=label, load_bias=load_bias)

    def compare(self, left=None, right=None, **kwargs):
        return compare_objects(left or self.image('left'), right or self.image('right'),
                               kwargs.pop('left_address', 0x1000),
                               kwargs.pop('right_address', 0x1000),
                               kwargs.pop('size', 16), **kwargs)

    def assertVerified(self, result):
        self.assertEqual(result['status'], 'verified_equal', result)

    def assertRejected(self, result):
        self.assertIn(result['status'], ('different', 'unverified'), result)

    def test_identical_relative_relocations_verify(self):
        self.assertVerified(self.compare())

    def test_identical_stored_bytes_do_not_hide_wrong_target(self):
        right = self.image('right', symbols=[('table', 1, 0x1000, 16, 0x11),
                                             ('other', 2, 0x2000, 32, 0x11)])
        result = self.compare(right=right)
        self.assertTrue(result['raw_equal'])
        self.assertRejected(result)

    def test_moved_object_and_target_ignore_only_relocated_bytes(self):
        left = self.image('left', data=struct.pack('<Q', 0x2000) + b'Q' * 24)
        right = self.image('right', address=0x7000, target=0x8000,
                           data=struct.pack('<Q', 0x8000) + b'Q' * 24)
        result = self.compare(left, right, right_address=0x7000)
        self.assertFalse(result['raw_equal'])
        self.assertTrue(result['non_relocated_equal'])
        self.assertEqual(result['masked_bytes'], 8)
        self.assertVerified(result)

    def test_non_relocated_difference_is_not_masked(self):
        right = self.image('right', data=b'\0' * 8 + b'X' + b'\0' * 23)
        self.assertRejected(self.compare(right=right))

    def test_interior_target_offsets_follow_named_object(self):
        left = self.image('left', relocations=[(0x1000, 1027, 0, 0x2008)])
        right = self.image('right', target=0x8000,
                           relocations=[(0x1000, 1027, 0, 0x8008)])
        self.assertVerified(self.compare(left, right))

    def test_changed_interior_offset_rejected(self):
        right = self.image('right', relocations=[(0x1000, 1027, 0, 0x2008)])
        self.assertRejected(self.compare(right=right))

    def test_one_past_target_extent_is_unresolved(self):
        left = self.image('left', relocations=[(0x1000, 1027, 0, 0x2020)])
        right = self.image('right', relocations=[(0x1000, 1027, 0, 0x2020)])
        self.assertRejected(self.compare(left, right))

    def test_duplicate_target_names_at_distinct_addresses_are_ambiguous(self):
        symbols = [('table', 1, 0x1000, 16, 0x11),
                   ('target', 2, 0x2000, 8, 0x11),
                   ('target', 2, 0x2010, 8, 0x11)]
        self.assertRejected(self.compare(self.image('left', symbols=symbols),
                                         self.image('right', symbols=symbols)))

    def test_local_target_names_cannot_establish_cross_image_identity(self):
        symbols = [('table', 1, 0x1000, 16, 0x11),
                   ('target', 2, 0x2000, 32, 0x01)]
        self.assertRejected(self.compare(self.image('left', symbols=symbols),
                                         self.image('right', symbols=symbols)))

    def test_shared_unique_alias_identifies_same_extent(self):
        left = self.image('left', symbols=[('table', 1, 0x1000, 16, 0x11),
                                          ('target', 2, 0x2000, 32, 0x11),
                                          ('alias', 2, 0x2000, 32, 0x11)])
        right = self.image('right', symbols=[('table', 1, 0x1000, 16, 0x11),
                                            ('alias', 2, 0x2000, 32, 0x11)])
        self.assertVerified(self.compare(left, right))

    def test_shared_alias_does_not_hide_conflicting_target_extents(self):
        symbols = [('table', 1, 0x1000, 16, 0x11),
                   ('target', 2, 0x2000, 32, 0x11),
                   ('alias', 2, 0x2000, 16, 0x11)]
        self.assertRejected(self.compare(self.image('left', symbols=symbols),
                                         self.image('right', symbols=symbols)))

    def test_unknown_relocation_type_fails_closed_even_when_identical(self):
        rel = [(0x1000, 9999, 0, 0x2000)]
        self.assertRejected(self.compare(self.image('left', relocations=rel),
                                         self.image('right', relocations=rel)))

    def test_overlapping_relocations_fail_closed(self):
        for offset in (0, 4):
            rel = [(0x1000, 1027, 0, 0x2000), (0x1000 + offset, 1027, 0, 0x2000)]
            with self.subTest(offset=offset):
                self.assertRejected(self.compare(self.image('left', relocations=rel),
                                                 self.image('right', relocations=rel)))

    def test_relocation_crossing_either_object_boundary_fails_closed(self):
        for offset in (0x1004, 0x100c):
            rel = [(offset, 1027, 0, 0x2000)]
            with self.subTest(offset=offset):
                self.assertRejected(self.compare(self.image('left', relocations=rel),
                                                 self.image('right', relocations=rel),
                                                 left_address=0x1008, right_address=0x1008,
                                                 size=8))

    def test_wide_unsupported_relocation_crossing_left_boundary_fails_closed(self):
        # AArch64 TLSDESC writes two 64-bit words; its start is outside this object.
        rel = [(0x1000, 1031, 0, 0)]
        self.assertRejected(self.compare(self.image('left', relocations=rel),
                                         self.image('right', relocations=rel),
                                         left_address=0x1008, right_address=0x1008, size=8))

    def test_missing_relocation_cannot_be_replaced_by_equal_bytes(self):
        self.assertRejected(self.compare(right=self.image('right', relocations=[])))

    def test_different_object_sizes_are_rejected(self):
        self.assertRejected(self.compare(right_size=8))

    def test_zero_fill_extents_verify_without_file_bytes(self):
        self.assertVerified(self.compare(left_address=0x2000, right_address=0x2000, size=32))

    def test_zero_fill_with_unknown_relocation_is_not_verified(self):
        rel = [(0x2000, 9999, 0, 0)]
        self.assertRejected(self.compare(self.image('left', relocations=rel),
                                         self.image('right', relocations=rel),
                                         left_address=0x2000, right_address=0x2000, size=32))

    def test_zero_fill_relocation_target_mismatch_is_not_verified(self):
        left = self.image('left', relocations=[(0x2000, 1027, 0, 0x1000)])
        right = self.image('right', relocations=[(0x2000, 1027, 0, 0x1008)])
        self.assertRejected(self.compare(left, right, left_address=0x2000,
                                         right_address=0x2000, size=32))

    def test_file_zeros_and_zero_fill_have_different_storage(self):
        self.assertRejected(self.compare(left_address=0x2000, right_address=0x1000, size=16))

    def test_explicit_symbol_absolute_relocation_verifies(self):
        rel = [(0x1000, 257, 2, 8)]
        self.assertVerified(self.compare(self.image('left', relocations=rel),
                                         self.image('right', relocations=rel)))

    def test_undefined_import_remains_unverified_without_provider_identity(self):
        symbols = [('table', 1, 0x1000, 16, 0x11), ('external', 0, 0, 0, 0x12)]
        rel = [(0x1000, 1025, 2, 0)]
        self.assertRejected(self.compare(self.image('left', symbols=symbols, relocations=rel),
                                         self.image('right', symbols=symbols, relocations=rel)))

    def test_relative_target_with_unmodeled_symbol_versions_is_unverified(self):
        for section_type in (0x6ffffffd, 0x6ffffffe, 0x6fffffff):
            with self.subTest(section_type=section_type):
                versioned = self.image('versioned', overrides={4: {1: section_type}})
                for left, right in ((versioned, self.image('right')),
                                    (self.image('left'), versioned)):
                    result = self.compare(left, right)
                    self.assertEqual(result['status'], 'unverified')
                    self.assertIn('unmodeled_symbol_versions', result['reasons'])
                    self.assertEqual(result['masked_bytes'], 0)

    def test_supplementary_version_evidence_prevents_relative_target_proof(self):
        symbols = [('table', 1, 0x1000, 8, 0x11),
                   ('target', 1, 0x1010, 8, 0x11)]
        rel = [(0x1000, 1027, 0, 0x1010)]
        for versioned in (False, True):
            with self.subTest(versioned=versioned):
                stripped = self.image('stripped', symbols=[], relocations=rel)
                supplement = self.image('supplement', symbols=symbols, relocations=[],
                                        overrides={4: {1: 0x6fffffff}} if versioned else {})
                rebuilt = self.image('rebuilt', symbols=symbols, relocations=rel)
                self.assertEqual(stripped.add_symbol_evidence(supplement), 2)
                result = self.compare(stripped, rebuilt)
                if versioned:
                    self.assertEqual(result['status'], 'unverified')
                    self.assertIn('unmodeled_symbol_versions', result['reasons'])
                    self.assertEqual(result['masked_bytes'], 0)
                else:
                    self.assertVerified(result)

    def test_load_bias_preserves_relative_identity(self):
        left = self.image('left', load_bias=0x100000)
        right = self.image('right', load_bias=0x200000)
        self.assertVerified(self.compare(left, right, left_address=0x101000,
                                         right_address=0x201000))


if __name__ == '__main__':
    unittest.main()
