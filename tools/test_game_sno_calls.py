import unittest

import game_sno_calls as gsc


def call(index, path, group=14, flags=0, mask=31, bucket=2):
    return {'mask': mask, 'bucket': bucket, 'index': index, 'group': group, 'path': path, 'flags': flags}


GROUPS = {14: 'EFFECTGROUP_SNO', 21: 'GLOBAL_SNO'}
NAMES = {0: 'GLOBALSNO_GLOBALS', 1: 'GLOBALSNO_UNK_1', 2: 'GLOBALSNO_FX'}


def line(index_name, path):
    return (f'    sLookupSNO((1 << SIGMAPLATFORM_PC), BUCKET_MAIN, {index_name}, '
            f'EFFECTGROUP_SNO, "{path}", 0);')


class ExtrasTests(unittest.TestCase):
    def test_folded_first_extras_call_is_marked(self):
        calls = [call(0, 'a'), call(5, 'b'), call(None, None), call(None, 'c')]
        out = gsc.extras_normalized(calls)
        self.assertEqual([c['index'] for c in out], [0, None, None, None])


class SnoCallsTests(unittest.TestCase):
    def test_reuse_rename_and_new(self):
        old = gsc.extras_normalized([call(0, 'globals'), call(1, 'fx'), call(2, 'x1'), call(None, None)])
        old_lines = [line('GLOBALSNO_GLOBALS', 'globals'), line('GLOBALSNO_FX', 'fx'),
                     '    if (sLookupSNO(A, nExtrasIndex)) nExtrasIndex++;',
                     '    if (sLookupSNO(B, nExtrasIndex)) nExtrasIndex++;']
        new = gsc.extras_normalized([call(0, 'globals'), call(1, 'new_one'), call(2, 'fx'),
                                     call(3, 'x1'), call(None, None)])
        text, reused, added = gsc.sno_calls(old, old_lines, new, NAMES, GROUPS)
        lines = text.rstrip('\n').split('\n')
        self.assertEqual((reused, added), (4, 1))
        self.assertIn('GLOBALSNO_UNK_1, EFFECTGROUP_SNO, "new_one", 0);', lines[1])
        self.assertIn('GLOBALSNO_FX, EFFECTGROUP_SNO, "fx"', lines[2])
        self.assertEqual(lines[3], gsc.DECL)
        self.assertEqual(lines[4:], old_lines[2:])

    def test_extras_size_change_is_refused(self):
        old = gsc.extras_normalized([call(0, 'a'), call(None, None)])
        new = gsc.extras_normalized([call(0, 'a'), call(None, None), call(None, 'z')])
        with self.assertRaises(ValueError):
            gsc.sno_calls(old, [line('GLOBALSNO_GLOBALS', 'a'), 'x', ], new, NAMES, GROUPS)


if __name__ == '__main__':
    unittest.main()
