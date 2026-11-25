import hashlib
import struct
import unittest

import version_diff as vd


def func(addr, words, name=''):
    masked, calls = vd.mask_function(tuple(words), addr)
    f = vd.Func(addr, 4 * len(words), masked,
                hashlib.blake2b(struct.pack(f'<{len(masked)}I', *masked), digest_size=16).digest(),
                calls, tuple(w & vd.SHAPE_MASK for w in masked),
                vd.branch_targets(tuple(words), addr), name=name)
    return f


def indexed(*funcs):
    for i, f in enumerate(funcs):
        f.index = i
    return list(funcs)


def bl(pc, target):
    return 0x94000000 | (((target - pc) // 4) & 0x3FFFFFF)


def b(pc, target):
    return 0x14000000 | (((target - pc) // 4) & 0x3FFFFFF)


RET = 0xD65F03C0
NOP = 0xD503201F


def adrp(rd, imm):
    return 0x90000000 | ((imm & 3) << 29) | (((imm >> 2) & 0x7FFFF) << 5) | rd


def ldr_x(rt, rn, imm12):
    return 0xF9400000 | (imm12 << 10) | (rn << 5) | rt


def add_x_imm(rd, rn, imm12):
    return 0x91000000 | (imm12 << 10) | (rn << 5) | rd


class MaskTests(unittest.TestCase):
    def test_calls_masked_and_recorded(self):
        a = func(0x1000, [bl(0x1000, 0x5000), RET])
        c = func(0x2000, [bl(0x2000, 0x9000), RET])
        self.assertEqual(a.masked, c.masked)
        self.assertEqual(a.calls, [0x5000])
        self.assertEqual(c.calls, [0x9000])

    def test_internal_branch_kept_external_masked(self):
        inner = func(0x1000, [b(0x1000, 0x1008), NOP, RET])
        moved = func(0x3000, [b(0x3000, 0x3008), NOP, RET])
        other = func(0x1000, [b(0x1000, 0x1004), NOP, RET])
        self.assertEqual(inner.masked, moved.masked)
        self.assertNotEqual(inner.masked, other.masked)
        tail_a = func(0x1000, [NOP, b(0x1004, 0x8000)])
        tail_b = func(0x2000, [NOP, b(0x2004, 0x9990)])
        self.assertEqual(tail_a.masked, tail_b.masked)

    def test_adrp_lo12_masked_but_plain_offsets_kept(self):
        a = func(0x1000, [adrp(8, 0x10), ldr_x(0, 8, 0x12), ldr_x(1, 0, 3), RET])
        c = func(0x1000, [adrp(8, 0x99), ldr_x(0, 8, 0x40), ldr_x(1, 0, 3), RET])
        d = func(0x1000, [adrp(8, 0x99), ldr_x(0, 8, 0x40), ldr_x(1, 0, 4), RET])
        self.assertEqual(a.masked, c.masked)
        self.assertNotEqual(a.masked, d.masked)   # struct offset after the load differs
        e = func(0x1000, [adrp(9, 1), add_x_imm(9, 9, 0x80), RET])
        f = func(0x1000, [adrp(9, 7), add_x_imm(9, 9, 0x10), RET])
        self.assertEqual(e.masked, f.masked)
        g = func(0x1000, [add_x_imm(0, 1, 8), RET])
        h = func(0x1000, [add_x_imm(0, 1, 16), RET])
        self.assertNotEqual(g.masked, h.masked)


class MatchTests(unittest.TestCase):
    def build(self, specs, base):
        funcs, addr = [], base
        for name, body in specs:
            words = body(addr)
            funcs.append(func(addr, words, name))
            addr += 4 * len(words) + 0x10
        for i, f in enumerate(funcs):
            f.index = i
        return funcs

    def test_buckets(self):
        same = lambda pc: [NOP, NOP, add_x_imm(0, 0, 1), RET]
        dup = lambda pc: [NOP, RET]
        changed_old = lambda pc: [add_x_imm(0, 0, 2)] * 6 + [RET]
        changed_new = lambda pc: [add_x_imm(1, 1, 2)] * 6 + [NOP, RET]   # registers differ
        gone = lambda pc: [ldr_x(3, 4, 5)] * 9 + [RET]
        added = lambda pc: [0x8B020020] * 12 + [RET]
        old = self.build([('a', same), ('d1', dup), ('c', changed_old), ('x', gone), ('d2', dup)], 0x1000)
        new = self.build([('a', same), ('d1', dup), ('c', changed_new), ('n', added), ('d2', dup)], 0x2000)
        vd.run_matching(old, new)
        got = {f.name: (vd.bucket(f, 'old'), f.match.name if f.match else None) for f in old}
        self.assertEqual(got['a'], ('identical', 'a'))
        self.assertEqual(got['d1'], ('identical', 'd1'))   # duplicate bodies resolved by order
        self.assertEqual(got['d2'], ('identical', 'd2'))
        self.assertEqual(got['c'], ('changed', 'c'))
        self.assertEqual(got['x'], ('removed', None))
        self.assertEqual(vd.bucket(new[3], 'new'), 'new')

    def test_callee_check(self):
        callee_old = func(0x100, [RET])
        callee_new = func(0x900, [RET])
        caller_old = func(0x200, [bl(0x200, 0x100), RET])
        caller_new = func(0xa00, [bl(0xa00, 0x900), RET])
        vd.pair(callee_old, callee_new, 'hash')
        vd.pair(caller_old, caller_new, 'hash')
        old_by = {f.addr: f for f in (callee_old, caller_old)}
        new_by = {f.addr: f for f in (callee_new, caller_new)}
        self.assertEqual(vd.callee_check(caller_old, new_by, old_by), 'ok')
        other = func(0x800, [RET])
        new_by[0x800] = other
        stray = func(0xb00, [bl(0xb00, 0x800), RET])
        vd.pair(caller_old, stray, 'hash')
        vd.pair(func(0x50, [RET]), other, 'hash')
        self.assertEqual(vd.callee_check(caller_old, new_by, old_by), '1-mismatched')


class BranchTargetTests(unittest.TestCase):
    def test_tail_calls_are_targets_but_not_calls(self):
        f = func(0x1000, [bl(0x1000, 0x5000), b(0x1004, 0x1000), b(0x1008, 0x6000)])
        self.assertEqual(f.calls, [0x5000])
        self.assertEqual(f.targets, [0x5000, 0x6000])     # internal branch skipped


class RepairTests(unittest.TestCase):
    def test_callers_fix_swapped_lookalikes(self):
        # Both builds: a caller calling two identical one-instruction targets.
        caller_old = func(0x1000, [bl(0x1000, 0x1010), bl(0x1004, 0x1020), RET])
        t1, t2 = func(0x1010, [RET], 't1'), func(0x1020, [RET], 't2')
        caller_new = func(0x2000, [bl(0x2000, 0x2010), bl(0x2004, 0x2020), RET])
        u1, u2 = func(0x2010, [RET], 'u1'), func(0x2020, [RET], 'u2')
        old, new = indexed(caller_old, t1, t2), indexed(caller_new, u1, u2)
        vd.pair(caller_old, caller_new, 'hash')
        vd.pair(t1, u2, 'order')                       # link order guessed wrong
        vd.pair(t2, u1, 'order')
        self.assertEqual(vd.repair_by_callers(old, new), 1)
        self.assertIs(t1.match, u1)
        self.assertIs(t2.match, u2)

    def test_neighbours_fix_a_lookalike(self):
        h = lambda pc: [add_x_imm(0, 0, 7), NOP, RET]
        k = lambda pc: [add_x_imm(1, 1, 9), NOP, RET]
        a0, s, a1 = func(0x1000, h(0)), func(0x1010, [RET], 's'), func(0x1020, k(0))
        b0, s1, b1, s2 = func(0x2000, h(0)), func(0x2010, [RET]), func(0x2020, k(0)), func(0x2030, [RET])
        old, new = indexed(a0, s, a1), indexed(b0, s1, b1, s2)
        vd.pair(a0, b0, 'hash')
        vd.pair(a1, b1, 'hash')
        vd.pair(s, s2, 'order')
        self.assertEqual(vd.repair_by_neighbours(old, new), 1)
        self.assertIs(s.match, s1)
        self.assertEqual(s.method, 'neighbour')

    def test_neighbours_do_not_steal_an_equally_supported_pair(self):
        h = lambda pc: [add_x_imm(0, 0, 7), NOP, RET]
        k = lambda pc: [add_x_imm(1, 1, 9), NOP, RET]
        # s claims x1 through its left neighbour; y holds x1 through its right one.
        a0, s, u = func(0x1000, h(0)), func(0x1010, [RET], 's'), func(0x1020, [NOP, NOP, NOP, RET])
        y, d = func(0x1040, [RET], 'y'), func(0x1050, k(0))
        b0, x1, e1 = func(0x2000, h(0)), func(0x2010, [RET]), func(0x2020, k(0))
        old, new = indexed(a0, s, u, y, d), indexed(b0, x1, e1)
        vd.pair(a0, b0, 'hash')
        vd.pair(d, e1, 'hash')
        vd.pair(y, x1, 'order')
        self.assertEqual(vd.repair_by_neighbours(old, new), 0)
        self.assertIs(y.match, x1)
        self.assertIsNone(s.match)

    def test_hash_pairs_are_never_moved(self):
        caller_old = func(0x1000, [bl(0x1000, 0x1010), RET])
        t1 = func(0x1010, [RET])
        caller_new = func(0x2000, [bl(0x2000, 0x2010), RET])
        u1, u2 = func(0x2010, [RET]), func(0x2020, [RET])
        old, new = indexed(caller_old, t1), indexed(caller_new, u1, u2)
        vd.pair(caller_old, caller_new, 'hash')
        vd.pair(t1, u2, 'hash')
        self.assertEqual(vd.repair_by_callers(old, new), 0)
        self.assertIs(t1.match, u2)


class EncodingTests(unittest.TestCase):
    def test_leb128_and_pointer_encodings(self):
        self.assertEqual(vd._uleb(bytes([0xE5, 0x8E, 0x26]), 0), (624485, 3))
        self.assertEqual(vd._sleb(bytes([0x7F]), 0), (-1, 1))
        data = struct.pack('<i', -16)
        self.assertEqual(vd._encoded(data, 0, 0x1b, 0x1000)[0], 0x1000 - 16)   # pcrel sdata4
        self.assertEqual(vd._encoded(data, 0, 0x3b, 0, datarel=0x2000)[0], 0x2000 - 16)


if __name__ == '__main__':
    unittest.main()
