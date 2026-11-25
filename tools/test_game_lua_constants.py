import unittest

import game_lua_constants as glc

OLD = [('SLOT_A', 1, 0), ('SLOT_B', 1, 1), ('SLOT_END', 1, 2), ('ATTR_X', 1, 10)]
NEW = [('SLOT_A', 1, 0), ('SLOT_B', 1, 1), ('SLOT_C', 1, 2), ('SLOT_D', 1, 3), ('SLOT_END', 1, 4),
       ('ATTR_X', 1, 10)]
HEADER = '''enum Slot {
    SLOT_A = 0,
    SLOT_B = 1,
    SLOT_END = 2,  // end
};
enum Attr {
    ATTR_X = 10,
};'''


class PlanTests(unittest.TestCase):
    def test_changed_and_added_in_table_order(self):
        changed, added = glc.plan(OLD, NEW)
        self.assertEqual(changed, {'SLOT_END': (2, 4)})
        self.assertEqual(added, [('SLOT_B', 'SLOT_C', 2), ('SLOT_C', 'SLOT_D', 3)])

    def test_removed_constant_is_refused(self):
        with self.assertRaises(ValueError):
            glc.plan(OLD, NEW[1:])


class RewriteTests(unittest.TestCase):
    def test_renumber_and_insert_chain(self):
        changed, added = glc.plan(OLD, NEW)
        table = {n: v for n, _, v in NEW}
        text, edits = glc.rewrite(HEADER, changed, added, table)
        self.assertEqual(edits, 3)
        self.assertIn('    SLOT_B = 1,\n    SLOT_C = 2,\n    SLOT_D = 3,\n    SLOT_END = 4,  // end', text)
        self.assertIn('ATTR_X = 10,', text)
        glc.verify({None: text}, table)

    def test_unexpected_old_value_is_refused(self):
        changed, added = glc.plan(OLD, NEW)
        with self.assertRaises(ValueError):
            glc.rewrite(HEADER.replace('SLOT_END = 2', 'SLOT_END = 7'), changed, added, {})

    def test_verify_rejects_duplicates(self):
        table = {'SLOT_A': 0}
        with self.assertRaises(ValueError):
            glc.verify({1: '    SLOT_A = 0,', 2: '    SLOT_A = 0,'}, table)


if __name__ == '__main__':
    unittest.main()
