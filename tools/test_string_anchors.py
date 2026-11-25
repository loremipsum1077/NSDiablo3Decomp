import tempfile
import unittest
from collections import defaultdict
from pathlib import Path

import string_anchors as sa
import version_diff as vd


def fake_image(starts, names, refs=None, pointers=None, strings=None):
    """An Image with hand-made tables instead of a parsed ELF."""
    img = object.__new__(sa.Image)
    img.starts = sorted(starts)
    img.sizes = {a: 0x10 for a in starts}
    img.names = {a: v for a, v in names.items()}
    img.refs = defaultdict(set, {k: set(v) for k, v in (refs or {}).items()})
    img.ref_targets = sorted(img.refs)
    img.pointers = defaultdict(list, pointers or {})
    img.callers, img.callees = defaultdict(set), {}
    img.strings = lambda min_length: dict(strings or {})
    return img


class SourcePathTests(unittest.TestCase):
    def test_server_and_branch_are_ignored(self):
        old = 'D:/BuildServer/2/work/code/branches/2_6_2/X/Common/Items.cpp'
        new = 'D:/BuildServer/1/work/code/branches/2_7_7/X/Common/Items.cpp'
        self.assertEqual(sa._unbranched(old), sa._unbranched(new))
        self.assertEqual(sa._unbranched('D:\\BuildServer\\1\\work\\code\\branches\\2_7_7\\X\\a.cpp'),
                         'source:X/a.cpp')
        self.assertEqual(sa._unbranched('Primordial Ashes'), 'Primordial Ashes')


class UsersTests(unittest.TestCase):
    def test_direct_pointer_and_table_references(self):
        string, slot, other_slot, table = 0x5000, 0x9010, 0x9fff0, 0x9000
        img = fake_image([0x100, 0x200, 0x300], {}, refs={string: {0x100}, table: {0x200}},
                         pointers={string: [slot]})
        funcs, slots, tables = img.users(string)
        self.assertEqual(funcs, {0x100})
        self.assertEqual(slots, [slot])
        self.assertEqual(tables, {0x200: table})            # slot 0x10 into the table
        self.assertIsNone(img.table_base(other_slot))      # nothing referenced nearby


class OwnersTests(unittest.TestCase):
    def test_path_strings_then_agreeing_neighbours(self):
        U = lambda a: ('U', f'sub_{a:x}')
        starts = [0x100, 0x110, 0x120, 0x130, 0x140, 0x150]
        names = {0x100: ('O', 'a'), 0x110: U(0x110), 0x120: ('O', 'b'),
                 0x130: U(0x130), 0x140: ('O', 'c'), 0x150: U(0x150)}
        path = 'D:/BuildServer/1/work/code/branches/2_7_7/X/Common/Hireling.cpp'
        img = fake_image(starts, names, refs={0x8000: {0x150}}, strings={path: 0x8000})
        with tempfile.TemporaryDirectory() as t:
            files = Path(t) / 'function_files.csv'
            files.write_text('Address,File,Compiler\n'
                             f'{vd.CSV_BASE + 0x100:#018x},X/A.cpp,7.1.0\n'
                             f'{vd.CSV_BASE + 0x120:#018x},X/A.cpp,7.1.0\n'
                             f'{vd.CSV_BASE + 0x140:#018x},X/B.cpp,7.1.0\n')
            rows = {int(r['address'], 16) - vd.CSV_BASE: r for r in sa.owners(img, files)}
        self.assertEqual((rows[0x110]['file'], rows[0x110]['evidence']), ('X/A.cpp', 'neighbours'))
        self.assertNotIn(0x130, rows)                      # A.cpp before, B.cpp after
        self.assertEqual((rows[0x150]['file'], rows[0x150]['evidence']),
                         ('X/Common/Hireling.cpp', 'path-string'))


if __name__ == '__main__':
    unittest.main()
