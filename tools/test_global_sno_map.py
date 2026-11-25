import unittest

import global_sno_map as g

HEADER = """enum GlobalSNOFlags {
    GLOBALSNO_FLAG_DO_NOT_PRELOAD_BIT = 0,
};

enum {
    GLOBALSNO_INVALID = -1,
    GLOBALSNO_A = 0,
    GLOBALSNO_B = 1,
    // a stale comment
    GLOBALSNO_UNUSED = 2,
    GLOBALSNO_C = 3,
    NUM_GLOBAL_SNOS = 4,
};
"""


class KeyMapTests(unittest.TestCase):
    def test_unique_and_shared_keys(self):
        old = {0: 'a', 1: 'x', 2: 'x', 3: 'b'}
        new = {0: 'a', 1: 'new', 2: 'x', 3: 'x', 4: 'b'}
        self.assertEqual(g._key_map(old, new), {0: 0, 1: 2, 2: 3, 3: 4})

    def test_shared_keys_with_different_counts_stay_unmapped(self):
        self.assertEqual(g._key_map({0: 'x', 1: 'x'}, {0: 'x'}), {})

    def test_unregistered_values_pair_in_order(self):
        self.assertEqual(g.full_map({0: 0, 2: 3}, {0: 'a', 2: 'b'}, {0: 'a', 2: 'n', 3: 'b'}, 3, 4),
                         {0: 0, 2: 3, 1: 1})
        with self.assertRaises(ValueError):
            g.full_map({}, {0: 'a'}, {}, 2, 3)


class RenumberTests(unittest.TestCase):
    def test_rewrites_only_the_counted_block(self):
        # A and C keep their order, B moves past a new entry, the unused name follows its hole.
        mapping = {0: 0, 1: 2, 2: 3, 3: 4, 4: 5}
        new = {0: ('a',), 1: ('n',), 2: ('b',), 4: ('c',)}
        text, renumbered, added = g.renumber(HEADER, 'NUM_GLOBAL_SNOS', mapping, new, lambda k: k[0])
        self.assertIn('GLOBALSNO_FLAG_DO_NOT_PRELOAD_BIT = 0,', text)
        block = text[text.index('enum {'):]
        self.assertEqual([l.strip() for l in block.splitlines()[1:-1]], [
            'GLOBALSNO_INVALID = -1,', 'GLOBALSNO_A = 0,',
            'GLOBALSNO_UNK_1 = 1,  // only in 2.7.7: n', 'GLOBALSNO_B = 2,',
            'GLOBALSNO_UNUSED = 3,', 'GLOBALSNO_C = 4,', 'NUM_GLOBAL_SNOS = 5,'])
        self.assertEqual((renumbered, added), (3, 1))

    def test_refuses_unmapped_names_and_gaps(self):
        with self.assertRaises(ValueError):
            g.renumber(HEADER, 'NUM_GLOBAL_SNOS', {0: 0, 1: 1, 3: 3, 4: 4}, {}, str)
        with self.assertRaises(ValueError):                # value 1 left unused
            g.renumber(HEADER, 'NUM_GLOBAL_SNOS', {0: 0, 1: 2, 2: 3, 3: 4, 4: 5}, {}, str)


if __name__ == '__main__':
    unittest.main()
