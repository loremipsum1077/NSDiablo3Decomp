"""line_evidence.py parsing of objdump output: locations, branch targets, data pairs, immediates."""
from pathlib import Path
import sys
import unittest

sys.path.insert(0, str(Path(__file__).resolve().parent))
import line_evidence as le

SAMPLE = '''
build/romfs/DiabloIIINX64r.nss:\tfile format elf64-littleaarch64

Disassembly of section EX:

000000000095e920 <_ZN8VoteKick6Server17OnVoteKickTimeOutEiiPv>:
; _ZN8VoteKick6Server17OnVoteKickTimeOutEiiPv():
; D:/BuildServer/2/work/code/branches/2_6_2/X/Server/System/SVoteKick.cpp:78
  95e920:      \tstr\tx21, [sp, #-0x30]!
; D:/BuildServer/2/work/code/branches/2_6_2/X/Server/System/SVoteKick.cpp:79
  95e930:      \tbl\t0x486cd0 <_Z20PlayerGetFirstInGamev>
  95e938:      \tcbz\tx19, 0x95e984 <_ZN8VoteKick6Server17OnVoteKickTimeOutEiiPv+0x64>
  95e93c:      \tadrp\tx21, 0x10e5000 <__got_start+0xda0>
  95e940:      \tldr\tx21, [x21, #0x5d0]
  95e944:      \tmov\tw20, #0xd768            // =55144
; D:\\BuildServer\\2\\work\\code\\branches\\2_6_2\\X\\Server\\..\\Sigma/SigmaThreadLocalStorage.h:62
  95e950:      \tldr\tx0, [x21]
  95e968:      \tcmn\tw8, #0x1
  95e96c:      \tb.eq\t0x95e974 <_ZN8VoteKick6Server17OnVoteKickTimeOutEiiPv+0x54>
  95e970:      \tadrp\tx8, 0xd75000
  95e974:      \tadd\tx8, x8, #0x9d7
  95e978:      \tadrp\tx9, 0xd76000
  95e97c:      \tmov\tx9, x0
  95e980:      \tldr\ts0, [x9, #0x10]
  95e984:      \tcmp\tw8, #0x19c
  95e988:      \ttbz\tw8, #0x5, 0x95e920
  95e98c:      \tlsl\tw8, w8, #0x3
  95e990:      \tret
'''


class ParseTests(unittest.TestCase):
    def setUp(self):
        self.insns = le.parse(SAMPLE)

    def test_locations_are_normalised_per_instruction(self):
        by = {i['address']: i for i in self.insns}
        self.assertEqual((by[0x95e920]['file'], by[0x95e920]['line']), ('X/Server/System/SVoteKick.cpp', 78))
        self.assertEqual((by[0x95e930]['file'], by[0x95e930]['line']), ('X/Server/System/SVoteKick.cpp', 79))
        self.assertEqual((by[0x95e950]['file'], by[0x95e950]['line']), ('X/Sigma/SigmaThreadLocalStorage.h', 62))
        self.assertEqual(by[0x95e944]['operands'], 'w20, #0xd768')
        self.assertEqual(by[0x95e930]['operands'], '0x486cd0')
        self.assertEqual(len(self.insns), 18)

    def test_branch_targets(self):
        by = {i['address']: i for i in self.insns}
        self.assertEqual(le.branch_target(by[0x95e930]), 0x486cd0)
        self.assertEqual(le.branch_target(by[0x95e938]), 0x95e984)
        self.assertEqual(le.branch_target(by[0x95e96c]), 0x95e974)
        self.assertEqual(le.branch_target(by[0x95e988]), 0x95e920)
        self.assertIsNone(le.branch_target(by[0x95e940]))

    def test_data_references_pair_adrp_with_first_use_only(self):
        refs = le.data_references(self.insns)
        addresses = {self.insns[i]['address']: a for i, a in refs.items()}
        self.assertEqual(addresses, {0x95e940: 0x10e55d0, 0x95e974: 0xd759d7})  # x9 was overwritten before its load

    def test_immediates_skip_stack_offsets_shifts_and_bits(self):
        values = le.immediates(self.insns)
        self.assertIn(-1, values)      # cmn w8, #1
        self.assertIn(0x19c, values)   # cmp
        self.assertIn(55144, values)   # mov
        self.assertNotIn(0x30, values) # stack
        self.assertNotIn(5, values)    # tbz bit number
        self.assertNotIn(3, values)    # lsl amount
        self.assertNotIn(0x10, values) # memory offset

    def test_value_text_decodes_floats_and_wide_values(self):
        self.assertEqual(le.value_text('1119092736', 'const float'), '90f (0x42b40000)')
        self.assertEqual(le.value_text('-1'), '-1 (0xffffffff)')
        self.assertEqual(le.value_text('18446744073709551615'), '18446744073709551615 (signed -1)')
        self.assertEqual(le.value_text('3'), '3')


if __name__ == '__main__':
    unittest.main()
