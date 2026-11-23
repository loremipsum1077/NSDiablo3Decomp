"""Focused tests for the stock COMDAT provider planner and ELF boundaries."""
import struct
import sys
import unittest
import collections
import os
import tempfile
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'toolchain'))
import weak_provider_link as link


def group(*members, aliases=()):
    return {'members': list(members), 'aliases': list(aliases), 'signature_type': 2, 'binding': 2}


def elf_fixture(alias=False, invalid_member=False):
    names = b'\0.shstrtab\0.strtab\0.symtab\0.text.x\0.group\0'
    strings = b'\0func\0alias\0'
    symbols = bytes(24) + struct.pack('<IBBHQQ', 1, 0x22, 0, 4, 0, 4)
    if alias:
        symbols += struct.pack('<IBBHQQ', 6, 0x22, 0, 4, 0, 4)
    payloads = [b'', names, strings, symbols, b'\xc0\x03\x5f\xd6',
                struct.pack('<II', 1, 12 if invalid_member else 4)]
    data = bytearray(64)
    offsets = []
    for payload in payloads:
        offsets.append(len(data))
        data.extend(payload)
    shoff = len(data)
    sections = [
        (0, 0, 0, 0, 0, 0, 0, 0, 0, 0),
        (names.index(b'.shstrtab'), 3, 0, 0, offsets[1], len(names), 0, 0, 1, 0),
        (names.index(b'.strtab'), 3, 0, 0, offsets[2], len(strings), 0, 0, 1, 0),
        (names.index(b'.symtab'), 2, 0, 0, offsets[3], len(symbols), 2, 1, 8, 24),
        (names.index(b'.text.x'), 1, 0x206, 0, offsets[4], 4, 0, 0, 4, 0),
        (names.index(b'.group'), 17, 0, 0, offsets[5], 8, 3, 1, 4, 4),
    ]
    for section in sections:
        data.extend(struct.pack('<IIQQQQIIQQ', *section))
    data[:16] = b'\x7fELF\x02\x01\x01' + bytes(9)
    struct.pack_into('<HHI', data, 16, 1, 183, 1)
    struct.pack_into('<Q', data, 0x28, shoff)
    struct.pack_into('<H', data, 52, 64)
    struct.pack_into('<HHH', data, 0x3a, 64, len(sections), 1)
    return bytes(data)


class ObjectParserTests(unittest.TestCase):
    def test_realistic_comdat_and_alias(self):
        plain = link.parse_object(elf_fixture())
        self.assertEqual(plain['func']['members'], ['.text.x'])
        self.assertEqual(plain['func']['aliases'], [])
        aliased = link.parse_object(elf_fixture(alias=True))
        self.assertEqual(aliased['func']['aliases'], ['alias'])

    def test_invalid_group_member_and_truncated_object(self):
        with self.assertRaises(link.ProviderError):
            link.parse_object(elf_fixture(invalid_member=True))
        with self.assertRaises(link.ProviderError):
            link.parse_object(elf_fixture()[:-5])

    def test_fifo_and_symlink_inputs_fail_without_blocking(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory).resolve()
            fifo = root / 'pipe'
            os.mkfifo(fifo)
            with self.assertRaises(link.ProviderError):
                link.bounded_file(fifo, 1024)
            source = root / 'source'
            source.write_bytes(b'file')
            (root / 'alias').symlink_to(source)
            with self.assertRaises(OSError):
                link.bounded_file(root / 'alias', 1024)


class PlannerTests(unittest.TestCase):
    def test_stable_stock_and_new_candidate(self):
        objects = {'stock': {'f': group('.text.f')}, 'next': {},
                   'candidate': {'f': group('.text.f')}}
        planned, duplicates, edges = link.plan(['stock', 'next', 'candidate'], objects, {'f': 'stock'})
        self.assertEqual(planned, ['stock', 'next', 'candidate'])
        self.assertEqual(edges, [('stock', 'candidate')])
        planned, _, _ = link.plan(['candidate', 'stock', 'next'], objects, {'f': 'stock'})
        self.assertEqual(planned, ['stock', 'candidate', 'next'])
        self.assertEqual(duplicates['f']['winner'], 'stock')

    def test_unknown_duplicate_missing_provider_and_alias_refused(self):
        objects = {'a': {'f': group('.text.f')}, 'b': {'f': group('.text.f')}}
        result = link.plan(['a', 'b'], objects, {})  # unregistered: linker's natural choice
        self.assertEqual(result[0], ['a', 'b'])
        self.assertIn({'winner': 'a', 'candidates': ['a', 'b']}, [r.get('f') for r in result if isinstance(r, dict)])
        with self.assertRaisesRegex(link.ProviderError, 'missing'):
            link.plan(['a', 'b'], objects, {'f': 'missing'})
        objects['b']['f']['aliases'] = ['other']
        with self.assertRaisesRegex(link.ProviderError, 'alias'):
            link.plan(['a', 'b'], objects, {'f': 'a'})

    def test_raw_ungrouped_strong_and_common_overrides_refused(self):
        good = link.parse_object(elf_fixture())
        for common in (False, True):
            raw = bytearray(elf_fixture())
            shoff = struct.unpack_from('<Q', raw, 0x28)[0]
            symoff = struct.unpack_from('<Q', raw, shoff + 3 * 64 + 24)[0]
            raw[symoff + 24 + 4] = 0x12
            if common:
                struct.pack_into('<H', raw, symoff + 24 + 6, 0xfff2)
            struct.pack_into('<I', raw, shoff + 5 * 64 + 4, 0)
            rogue = link.parse_object(bytes(raw))
            objects = {'a': good, 'b': good, 'rogue': rogue}
            with self.assertRaisesRegex(link.ProviderError, 'conflicting definition'):
                link.plan(['a', 'b', 'rogue'], objects, {'func': 'a'})
            with self.assertRaisesRegex(link.ProviderError, 'conflicting definition'):
                link.plan(['a', 'rogue'], {'a': good, 'rogue': rogue}, {'func': 'a'})
            with self.assertRaisesRegex(link.ProviderError, 'conflicting definition'):
                link.plan(['a', 'rogue'], {'a': good, 'rogue': rogue}, {})

    def test_raw_additional_group_export_refused_even_if_unique(self):
        raw = bytearray(elf_fixture(alias=True))
        shoff = struct.unpack_from('<Q', raw, 0x28)[0]
        symoff = struct.unpack_from('<Q', raw, shoff + 3 * 64 + 24)[0]
        struct.pack_into('<Q', raw, symoff + 48 + 8, 1)
        obj = link.parse_object(bytes(raw))
        with self.assertRaisesRegex(link.ProviderError, 'additional group exports'):
            link.plan(['a'], {'a': obj}, {'func': 'a'})

    def test_non_group_provider_and_initializer_order_cannot_reverse(self):
        a = link.parse_object(elf_fixture())
        b = link.parse_object(elf_fixture())
        symbol = {'name': 'other', 'binding': 2, 'section': 4, 'type': 2,
                  'value': 1, 'size': 1, 'other': 0}
        a.definitions['other'] = [symbol]
        b.definitions['other'] = [symbol]
        # Definition and initialiser order outrank a registered provider.
        self.assertEqual(link.plan(['a', 'b'], {'a': a, 'b': b}, {'func': 'b'})[0], ['a', 'b'])
        del a.definitions['other']
        del b.definitions['other']
        a.has_initializers = b.has_initializers = True
        self.assertEqual(link.plan(['a', 'b'], {'a': a, 'b': b}, {'func': 'b'})[0], ['a', 'b'])

    def test_conflicting_provider_cycle_keeps_input_order(self):
        objects = {'a': {'f': group('.text.f'), 'g': group('.text.g')},
                   'b': {'f': group('.text.f'), 'g': group('.text.g')}}
        self.assertEqual(link.plan(['a', 'b'], objects, {'f': 'a', 'g': 'b'})[0], ['a', 'b'])

    def initialised(self, *names, **groups):
        objects = {}
        for name in names:
            objects[name] = link.ObjectGroups()
            objects[name].has_initializers = not name.startswith('plain')
            for signature in groups.get(name, ()):
                objects[name][signature] = group('.text.' + signature)
        return objects

    def test_original_initializer_order_settles_conflict(self):
        objects = self.initialised('b', 'a', b=['f'], a=['f'])
        # Without the original's order the incoming initialiser order wins.
        self.assertEqual(link.plan(['b', 'a'], objects, {'f': 'a'})[0], ['b', 'a'])
        # The original initialised a first: the registered provider is honoured.
        self.assertEqual(link.plan(['b', 'a'], objects, {'f': 'a'}, ['a', 'b'])[0], ['a', 'b'])
        # The original agrees with the incoming order: nothing moves.
        self.assertEqual(link.plan(['b', 'a'], objects, {'f': 'a'}, ['b', 'a'])[0], ['b', 'a'])
        # Only one of the pair is known: nothing moves.
        self.assertEqual(link.plan(['b', 'a'], objects, {'f': 'a'}, ['a'])[0], ['b', 'a'])

    def test_original_order_move_keeps_unrelated_copies(self):
        # Moving a before b would also move a before d and hand d's copy of g to a,
        # an object the original's order does not place before d; moving b after a
        # instead keeps g and honours f.
        objects = self.initialised('b', 'd', 'a', b=['f'], d=['g'], a=['f', 'g'])
        planned, duplicates, _ = link.plan(['b', 'd', 'a'], objects, {'f': 'a'}, ['a', 'b'])
        self.assertEqual(planned, ['d', 'a', 'b'])
        self.assertEqual(duplicates['g']['winner'], 'd')

    def test_original_order_never_overrides_definition_order(self):
        objects = self.initialised('b', 'a', b=['f'], a=['f'])
        symbol = {'name': 'other', 'binding': 2, 'section': 4, 'type': 2,
                  'value': 1, 'size': 1, 'other': 0}
        objects['b'].definitions['other'] = [symbol]
        objects['a'].definitions['other'] = [symbol]
        self.assertEqual(link.plan(['b', 'a'], objects, {'f': 'a'}, ['a', 'b'])[0], ['b', 'a'])

    def test_manifest_initializer_order(self):
        base = {'format': 1, 'description': '', 'reference': {}, 'providers': {}}
        self.assertEqual(link.validate_initializer_order(base), [])
        names = ['CMakeFiles/x.dir/a.cpp.obj', 'CMakeFiles/x.dir/b.cpp.obj']
        manifest = dict(base, initializer_order=names)
        link.validate_manifest(manifest)
        self.assertEqual(link.validate_initializer_order(manifest), names)
        for bad in (names + names[:1], ['CMakeFiles/../a.cpp.obj'], 'CMakeFiles/x.dir/a.cpp.obj'):
            with self.assertRaises(link.ProviderError):
                link.validate_initializer_order(dict(base, initializer_order=bad))
        with self.assertRaises(link.ProviderError):
            link.validate_manifest(dict(base, unknown=[]))

    def test_new_unique_group_needs_no_manifest_entry(self):
        planned, duplicates, edges = link.plan(['a'], {'a': {'new': group('.text.new')}}, {})
        self.assertEqual((planned, duplicates, edges), (['a'], {}, []))

    def test_mapped_provider_must_be_unique_and_match(self):
        objects = {'a': {'f': group('.text.f')}, 'b': {'f': group('.text.f')}}
        _, duplicates, _ = link.plan(['a', 'b'], objects, {'f': 'a'})
        absolute = {'a': '/build/a', 'b': '/build/b'}
        self.assertEqual(link.verify_map({('/build/a', '.text.f')}, duplicates, objects, absolute), {'f': 'a'})
        # The copy the linker kept is recorded even when it is not the planned one.
        self.assertEqual(link.verify_map({('/build/b', '.text.f')}, duplicates, objects, absolute), {'f': 'b'})
        self.assertEqual(link.verify_map(set(), duplicates, objects, absolute), {'f': 'a'})
        with self.assertRaises(link.ProviderError):
            link.verify_map({('/build/a', '.text.f'), ('/build/b', '.text.f')}, duplicates, objects, absolute)
        with self.assertRaises(link.ProviderError):
            link.verify_map(collections.Counter({('/build/a', '.text.f'): 2}), duplicates, objects, absolute)


if __name__ == '__main__':
    unittest.main()
