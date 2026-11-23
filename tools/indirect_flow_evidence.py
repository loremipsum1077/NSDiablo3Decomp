#!/usr/bin/env python3
"""Read-only evidence extraction for P19-F04: indirect control flow in rejected functions.

For every function enumerated in evidence/checker/reconciliation.json this script
disassembles the original and rebuilt bodies, builds the direct control-flow graph the
checker patch 0006 uses, slices every indirect BR back to its defining instructions and
classifies it:

  switch_dispatch      PC-relative jump table (adrp/add base, ldrsw|ldrb|ldrh entry, add, br)
  indirect_tail_call   target loaded from memory or taken from an argument register
  other / unresolved   anything the slicer cannot describe

Jump tables are read from both ELF images, bounded by the guarding compare, and the
resulting targets are checked against the function range. Nothing here edits inventory,
claims, source or the checker. Output goes to a directory outside data/.

  python3 tools/indirect_flow_evidence.py --output build/indirect-flow-evidence
"""
import argparse
from collections import Counter, defaultdict, deque
import csv
import hashlib
import io
import json
from pathlib import Path
import re
import struct
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[1]
RECONCILIATION = 'evidence/checker/reconciliation.json'
INVENTORY = 'data/2.6.2/functions.csv'
OWNERS = 'data/2.6.2/function_files.csv'
DATA_SYMBOLS = 'data/2.6.2/data_symbols.csv'
ORIGINAL_ELF = 'data/2.6.2/main.elf'
REBUILT_ELF = 'build/2.6.2/d3'
REBUILT_MAP = 'build/2.6.2/d3.map'
BASE = 0x7100000000
OBJDUMP_CANDIDATES = [ROOT / 'toolchain/bin/llvm-objdump', Path('/opt/homebrew/opt/llvm/bin/llvm-objdump')]
SLICE_LIMIT = 100000       # instructions visited per backward search (bounded by function size)
CHAIN_DEPTH = 8            # nested value descriptions
R_AARCH64_ABS64, R_AARCH64_GLOB_DAT, R_AARCH64_JUMP_SLOT, R_AARCH64_RELATIVE = 257, 1025, 1026, 1027
# Hand-reviewed representatives written in full to representatives.json (see findings.md).
REPRESENTATIVES = [
    '_ZN12CGameGlobals13SetCameraModeE10CameraMode',          # seed: 4-entry switch, b.hi guard
    '_ZN18NVNShaderAllocator7DestroyEv',                      # seed: GOT global function pointer
    '_ZN2D37Account6Digest27MergePartialFromCodedStreamEPN6google8protobuf2io16CodedInputStreamE',
    'adjust_recurse',                                         # b.ls guard toward dispatch, hoisted base, two guards
    'compare_opcodes',                                        # table base saved/restored around a call
    '_Z27ScriptEventInitializeForAppv',                       # dispatch reachable only through another table
    '_ZN7RWindow13PostFXGfxInitEv',                           # and x,x,#0xffffffff index copy
    'match@pcre_exec.c',                                      # largest function, 16 tables, qualified owner
    '_Z12TagNameToTagRK10CRefString8TagBases',                # table differs between binaries
    '_Z24TagGetDefinitionDataType10AxeTagType',               # table differs between binaries
    '_ZNK27TypeDescriptorClass_DT_GBID21ToXMLSerializedStringERK36TypeDescriptorFieldSerializationInfoPKvR10CRefString',
    '_ZN8bdMemory10deallocateEPv',                            # GOT pointer with null check
    '_ZNK21ComplexTypeDescriptor9ToVariantEPKv',              # field pointer of argument, null checked
    '_ZN14UIConfirmation9OnButton2ERK4UIIDi',                 # stack slot reload
    '_Z50CGameAsyncRenderRegisterGPUResourceDelayedDeletionPFvPvS_ES_S_',  # argument register
    '_ZN13UIFriendsList7Console9OnButton1ERK4UIIDi',          # post-index load of callback
]


def sha256(data):
    return hashlib.sha256(data).hexdigest()


# ----------------------------------------------------------------------------- ELF
class Elf:
    """Minimal ELF64 little-endian reader: segments, sections, symbols, RELA."""

    def __init__(self, path):
        self.path = Path(path)
        self.blob = self.path.read_bytes()
        b = self.blob
        if b[:6] != b'\x7fELF\x02\x01' or struct.unpack_from('<H', b, 18)[0] != 183:
            raise ValueError(f'{path}: expected ELF64 little-endian AArch64')
        phoff = struct.unpack_from('<Q', b, 32)[0]
        shoff = struct.unpack_from('<Q', b, 40)[0]
        phentsize, phnum, shentsize, shnum, shstrndx = struct.unpack_from('<HHHHH', b, 54)
        self.segments = [struct.unpack_from('<IIQQQQQQ', b, phoff + i * phentsize) for i in range(phnum)]
        raw_sections = [struct.unpack_from('<IIQQQQIIQQ', b, shoff + i * shentsize) for i in range(shnum)]
        names = raw_sections[shstrndx] if shstrndx < shnum else None
        self.sections = []
        for s in raw_sections:
            name = self._cstr(names[4] + s[0]) if names else ''
            self.sections.append({'name': name, 'type': s[1], 'flags': s[2], 'addr': s[3],
                                  'offset': s[4], 'size': s[5], 'link': s[6], 'info': s[7],
                                  'entsize': s[9]})
        self.symbols = self._symbols('.symtab') or self._symbols('.dynsym')
        self.relocs = {}
        for sec in self.sections:
            if sec['type'] == 4:  # SHT_RELA
                symtab = self.sections[sec['link']]
                strtab = self.sections[symtab['link']]
                for off in range(sec['offset'], sec['offset'] + sec['size'], 24):
                    r_offset, r_info, r_addend = struct.unpack_from('<QQq', b, off)
                    r_type = r_info & 0xffffffff
                    r_sym = r_info >> 32
                    name = ''
                    if r_sym:
                        sym_off = symtab['offset'] + r_sym * 24
                        st_name = struct.unpack_from('<I', b, sym_off)[0]
                        name = self._cstr(strtab['offset'] + st_name)
                    self.relocs[r_offset] = {'type': r_type, 'symbol': name, 'addend': r_addend,
                                             'section': sec['name']}

    def _cstr(self, off):
        end = self.blob.index(b'\0', off)
        return self.blob[off:end].decode('utf-8', 'replace')

    def _symbols(self, name):
        sec = next((s for s in self.sections if s['name'] == name), None)
        if not sec:
            return []
        strtab = self.sections[sec['link']]
        out = []
        for off in range(sec['offset'], sec['offset'] + sec['size'], 24):
            st_name, st_info, _, st_shndx, st_value, st_size = struct.unpack_from('<IBBHQQ', self.blob, off)
            out.append({'name': self._cstr(strtab['offset'] + st_name), 'type': st_info & 0xf,
                        'bind': st_info >> 4, 'shndx': st_shndx, 'value': st_value, 'size': st_size})
        return out

    def section_of(self, vaddr):
        for s in self.sections:
            if s['flags'] & 2 and s['addr'] <= vaddr < s['addr'] + s['size']:
                return s
        return None

    def read(self, vaddr, size):
        """Bytes at a virtual address from file-backed data (segments first, then sections)."""
        for seg in self.segments:
            if seg[0] == 1 and seg[3] <= vaddr and vaddr + size <= seg[3] + seg[5]:
                start = seg[2] + vaddr - seg[3]
                return self.blob[start:start + size]
        s = self.section_of(vaddr)
        if s and s['type'] != 8 and vaddr + size <= s['addr'] + s['size']:
            start = s['offset'] + vaddr - s['addr']
            return self.blob[start:start + size]
        return None


class LinkMap:
    """Parse an lld map: input sections with their object file, and symbol lines."""

    LINE = re.compile(r'^([0-9a-f]{16}) ([0-9a-f]{16}) +(\d+) ( *)(\S.*)$')

    def __init__(self, path):
        self.inputs = []       # (addr, size, object, section)
        self.symbol_objects = defaultdict(list)   # name -> [(addr, object, section)]
        current = None
        allocated = False
        for line in Path(path).read_text(errors='replace').splitlines():
            m = self.LINE.match(line)
            if not m:
                continue
            addr, size, indent, text = int(m[1], 16), int(m[2], 16), len(m[4]), m[5].strip()
            if indent == 0:
                current = None
                # Non-allocated output sections (.debug_*, .comment, .symtab ...) are listed
                # with file offsets, not virtual addresses; they must not shadow .rodata.
                allocated = not text.startswith(('.debug', '.comment', '.symtab', '.strtab', '.shstrtab'))
            elif not allocated:
                continue
            elif ':(' in text and text.endswith(')'):
                obj, sec = text.rsplit(':(', 1)
                current = (addr, size, obj, sec[:-1])
                self.inputs.append(current)
            elif current and text and not text.startswith('$'):
                self.symbol_objects[text].append((addr, current[2], current[3]))

    def input_section_at(self, vaddr):
        best = None
        for addr, size, obj, sec in self.inputs:
            if addr <= vaddr < addr + max(size, 1):
                if best is None or addr >= best[0]:
                    best = (addr, size, obj, sec)
        return best


# ----------------------------------------------------------------------- decoding
COND_NAMES = ['eq', 'ne', 'hs', 'lo', 'mi', 'pl', 'vs', 'vc', 'hi', 'ls', 'ge', 'lt', 'gt', 'le', 'al', 'nv']
STORE_PREFIXES = ('st',)
NO_WRITE = {'cmp', 'cmn', 'tst', 'ccmp', 'ccmn', 'fcmp', 'fcmpe', 'fccmp', 'fccmpe', 'b', 'bl', 'blr', 'br',
            'cbz', 'cbnz', 'tbz', 'tbnz',
            'ret', 'prfm', 'prfum', 'nop', 'hint', 'dmb', 'dsb', 'isb', 'brk', 'svc', 'hlt', 'yield',
            'wfe', 'wfi', 'sev', 'sevl', 'clrex', 'msr', 'eret', 'drps', 'udf'}
FLAG_WRITERS = {'cmp', 'cmn', 'tst', 'adds', 'subs', 'ands', 'bics', 'negs', 'ngcs', 'adcs', 'sbcs',
                'ccmp', 'ccmn', 'fcmp', 'fcmpe', 'fccmp', 'fccmpe'}
REG = re.compile(r'^(?:([wx])(\d+)|(wzr|xzr)|(sp)|(lr)|(fp))$')


def sign_extend(value, bits):
    return value - (1 << bits) if value & (1 << (bits - 1)) else value


def reg_number(token):
    """GPR number 0..30 for a register token, 31 for sp, None otherwise (zr excluded)."""
    m = REG.match(token.strip())
    if not m:
        return None
    if m[1]:
        return int(m[2])
    if m[5]:
        return 30
    if m[6]:
        return 29
    if m[4]:
        return 31
    return None


class Insn:
    __slots__ = ('addr', 'word', 'mnemonic', 'operands', 'text', 'index', 'writes', 'writes32',
                 'succ', 'pred', 'kind', 'target', 'kills_calls', 'cond')

    def __init__(self, addr, word, mnemonic, operands):
        self.addr, self.word, self.mnemonic, self.operands = addr, word, mnemonic, operands
        self.text = f'{mnemonic} {operands}'.strip()
        self.succ, self.pred = [], []
        self.kind, self.target, self.cond = 'other', None, None
        self.kills_calls = False
        self.writes, self.writes32 = set(), set()

    def decode(self):
        w = self.word
        if w & 0xfc000000 == 0x14000000:
            self.kind = 'b'
            self.target = self.addr + sign_extend(w & 0x3ffffff, 26) * 4
        elif w & 0xfc000000 == 0x94000000:
            self.kind = 'bl'
            self.target = self.addr + sign_extend(w & 0x3ffffff, 26) * 4
        elif w & 0xff000010 == 0x54000000:
            self.kind = 'bcond'
            self.cond = COND_NAMES[w & 0xf]
            self.target = self.addr + sign_extend((w >> 5) & 0x7ffff, 19) * 4
        elif w & 0x7e000000 == 0x34000000:
            self.kind = 'cbz'
            self.target = self.addr + sign_extend((w >> 5) & 0x7ffff, 19) * 4
        elif w & 0x7e000000 == 0x36000000:
            self.kind = 'tbz'
            self.target = self.addr + sign_extend((w >> 5) & 0x3fff, 14) * 4
        elif w & 0xfffffc1f == 0xd61f0000:
            self.kind = 'br'
        elif w & 0xfffffc1f == 0xd63f0000:
            self.kind = 'blr'
        elif w & 0xfffffc1f == 0xd65f0000:
            self.kind = 'ret'
        elif w & 0xffe0001f == 0xd4200000 or w == 0xe7ffdefe:
            self.kind = 'trap'
        self.kills_calls = self.kind in ('bl', 'blr')
        self._written_registers()

    def _written_registers(self):
        mn = self.mnemonic
        if mn in NO_WRITE or mn.startswith('b.'):
            return
        ops = [o.strip() for o in split_operands(self.operands)]
        if not ops:
            return
        writeback = re.search(r'\[\s*([wx]\d+|sp|fp|lr)[^\]]*\]!|\]\s*,\s*#', self.operands)
        if writeback:
            base = re.search(r'\[\s*([wx]\d+|sp|fp|lr)', self.operands)
            if base:
                n = reg_number(base[1])
                if n is not None and n != 31:
                    self.writes.add(n)
        if mn.startswith(STORE_PREFIXES):
            # stxr/stlxr/stxp write a status register as the first operand
            if mn.startswith(('stxr', 'stlxr', 'stxp', 'stlxp')):
                n = reg_number(ops[0])
                if n is not None and n != 31:
                    self.writes.add(n)
                    self.writes32.add(n)
            return
        count = 2 if mn.startswith(('ldp', 'ldnp', 'ldxp', 'ldaxp', 'cas')) else 1
        if mn.startswith('cas') and not mn.startswith('casp'):
            count = 1
        for op in ops[:count]:
            n = reg_number(op)
            if n is None or n == 31 and not mn.startswith(('add', 'sub', 'mov')):
                continue
            if n == 31:
                continue
            self.writes.add(n)
            if op.startswith('w'):
                self.writes32.add(n)


def split_operands(text):
    out, depth, cur = [], 0, ''
    for ch in text:
        if ch in '[{':
            depth += 1
        elif ch in ']}':
            depth -= 1
        if ch == ',' and depth == 0:
            out.append(cur)
            cur = ''
        else:
            cur += ch
    if cur.strip():
        out.append(cur)
    return out


def rd(w):
    return w & 31


def rn(w):
    return (w >> 5) & 31


def rm(w):
    return (w >> 16) & 31


def decode_adrp(w):
    if w & 0x9f000000 == 0x90000000:
        immlo = (w >> 29) & 3
        immhi = (w >> 5) & 0x7ffff
        return rd(w), sign_extend((immhi << 2) | immlo, 21) << 12
    return None


def decode_adr(w):
    if w & 0x9f000000 == 0x10000000:
        immlo = (w >> 29) & 3
        immhi = (w >> 5) & 0x7ffff
        return rd(w), sign_extend((immhi << 2) | immlo, 21)
    return None


def decode_add_imm(w):
    """ADD/SUB (immediate), 64-bit, no flags: (rd, rn, signed_imm)."""
    if w & 0x7f800000 == 0x11000000 and w & 0x80000000:
        imm = (w >> 10) & 0xfff
        if (w >> 22) & 1:
            imm <<= 12
        return rd(w), rn(w), (-imm if w & 0x40000000 else imm)
    return None


def decode_add_imm_any(w):
    """ADD/SUB immediate in either width: (sf, is_sub, rd, rn, imm)."""
    if w & 0x1f800000 == 0x11000000:
        imm = (w >> 10) & 0xfff
        if (w >> 22) & 1:
            imm <<= 12
        return (w >> 31) & 1, (w >> 30) & 1, rd(w), rn(w), imm
    return None


def decode_subs_imm(w):
    """SUBS/CMP (immediate): (sf, rd, rn, imm)."""
    if w & 0x7f800000 == 0x71000000:
        imm = (w >> 10) & 0xfff
        if (w >> 22) & 1:
            imm <<= 12
        return (w >> 31) & 1, rd(w), rn(w), imm
    return None


def decode_add_shifted(w):
    """ADD (shifted register) 64-bit: (rd, rn, rm, shift_type, amount)."""
    if w & 0xff200000 == 0x8b000000:
        return rd(w), rn(w), rm(w), (w >> 22) & 3, (w >> 10) & 0x3f
    return None


def decode_mov_reg(w):
    """ORR Rd, ZR, Rm (register MOV): (sf, rd, rm)."""
    if w & 0x7fe0ffe0 == 0x2a0003e0:
        return (w >> 31) & 1, rd(w), rm(w)
    return None


def decode_copy32(w):
    """Zero-extending 32-bit register copy: MOV Wd, Wm or AND Xd, Xm, #0xffffffff -> (rd, rm)."""
    m = decode_mov_reg(w)
    if m and m[0] == 0:
        return m[1], m[2]
    if w & 0xfffffc00 == 0x92407c00:
        return rd(w), rn(w)
    return None


def decode_ldr_imm(w):
    """LDR/LDRSW (unsigned immediate) into a GPR: (rt, rn, offset_bytes, size, signed)."""
    size = (w >> 30) & 3
    if w & 0x3fc00000 == 0x39400000:            # LDR{B,H,W,X} unsigned (opc=01)
        return rd(w), rn(w), ((w >> 10) & 0xfff) << size, 1 << size, False
    if w & 0xffc00000 == 0xb9800000:            # LDRSW unsigned
        return rd(w), rn(w), ((w >> 10) & 0xfff) << 2, 4, True
    if w & 0x3fe00400 == 0x38400400:            # LDR{B,H,W,X} post-index (bit 11 = 0) / pre-index (bit 11 = 1)
        size = (w >> 30) & 3
        pre = (w >> 11) & 1
        offset = sign_extend((w >> 12) & 0x1ff, 9) if pre else 0
        return rd(w), rn(w), offset, 1 << size, False
    return None


def decode_ldr_reg(w):
    """LDR{,B,H,SW} (register offset): (rt, rn, rm, size, signed, extend, shift)."""
    if w & 0x3fe00c00 == 0x38600800:
        size = (w >> 30) & 3
        return rd(w), rn(w), rm(w), 1 << size, False, (w >> 13) & 7, size if (w >> 12) & 1 else 0
    if w & 0xffe00c00 == 0xb8a00800:            # LDRSW register
        return rd(w), rn(w), rm(w), 4, True, (w >> 13) & 7, 2 if (w >> 12) & 1 else 0
    if w & 0xbfa00c00 == 0x38a00800:            # LDRSB/LDRSH register (opc=1x)
        size = (w >> 30) & 3
        return rd(w), rn(w), rm(w), 1 << size, True, (w >> 13) & 7, size if (w >> 12) & 1 else 0
    return None


# ------------------------------------------------------------------ disassembly
def find_objdump():
    for candidate in OBJDUMP_CANDIDATES:
        if candidate.exists():
            return candidate
    raise FileNotFoundError('llvm-objdump not found')


LINE = re.compile(r'^\s*([0-9a-f]+):\s+([0-9a-f]{8})\s+(\S+)\s*(.*)$')


def disassemble(objdump, elf_path, start, size):
    run = subprocess.run([str(objdump), '-d', f'--start-address={start:#x}',
                          f'--stop-address={start + size:#x}', str(elf_path)],
                         capture_output=True, text=True, check=True)
    insns = []
    for line in run.stdout.splitlines():
        m = LINE.match(line)
        if not m:
            continue
        addr = int(m[1], 16)
        if not start <= addr < start + size:
            continue
        operands = re.sub(r'\s*//.*$', '', m[4])
        operands = re.sub(r'\s*<[^>]*>', '', operands).strip()
        insn = Insn(addr, int(m[2], 16), m[3], operands)
        insn.decode()
        insns.append(insn)
    if len(insns) != size // 4:
        raise ValueError(f'disassembly of {start:#x}+{size:#x} produced {len(insns)} instructions')
    return insns


# --------------------------------------------------------------------- analysis
class Function:
    def __init__(self, name, elf, start, size, insns):
        self.name, self.elf, self.start, self.size, self.insns = name, elf, start, size, insns
        self.end = start + size
        self.by_addr = {i.addr: i for i in insns}
        for index, i in enumerate(insns):
            i.index = index
        self.build_cfg()

    def contains(self, addr):
        return self.start <= addr < self.end

    def build_cfg(self, extra_edges=None):
        for i in self.insns:
            i.succ, i.pred = [], []
        for i in self.insns:
            nxt = i.index + 1 < len(self.insns)
            if i.kind in ('b',):
                if self.contains(i.target):
                    i.succ.append(self.by_addr[i.target])
            elif i.kind in ('bcond', 'cbz', 'tbz'):
                if self.contains(i.target):
                    i.succ.append(self.by_addr[i.target])
                if nxt:
                    i.succ.append(self.insns[i.index + 1])
            elif i.kind in ('br', 'ret', 'trap'):
                pass
            else:
                if nxt:
                    i.succ.append(self.insns[i.index + 1])
        if extra_edges:
            for src, targets in extra_edges.items():
                i = self.by_addr[src]
                for t in targets:
                    if self.contains(t) and self.by_addr[t] not in i.succ:
                        i.succ.append(self.by_addr[t])
        for i in self.insns:
            for s in i.succ:
                s.pred.append(i)

    def reachable(self):
        seen = set()
        work = [self.insns[0]] if self.insns else []
        while work:
            i = work.pop()
            if i.index in seen:
                continue
            seen.add(i.index)
            work.extend(i.succ)
        return seen

    def reaching_defs(self, insn, reg, limit=SLICE_LIMIT):
        """Instructions that may define `reg` on some path reaching `insn`; 'entry' for parameters.

        Returns (defs, complete): complete is False if the search was cut by the limit.
        """
        defs, seen, work, visited = set(), set(), list(insn.pred), 0
        if not insn.pred and insn.index == 0:
            defs.add('entry')
        while work:
            p = work.pop()
            if p.index in seen:
                continue
            seen.add(p.index)
            visited += 1
            if visited > limit:
                return defs, False
            if reg in p.writes or (p.kills_calls and (reg <= 18 or reg == 30)):
                defs.add(p)
                continue
            if p.index == 0:
                defs.add('entry')
            work.extend(p.pred)
        return defs, True


def regname(n, width='x'):
    return f'{width}{n}'


def insn_json(i):
    return {'address': hex(i.addr), 'word': f'0x{i.word:08x}', 'text': i.text}


def describe(fn, insn, reg, depth=CHAIN_DEPTH, trail=None):
    """Structured description of the value held by `reg` at `insn` (before it executes)."""
    trail = trail or []
    defs, complete = fn.reaching_defs(insn, reg)
    if not complete:
        return {'kind': 'unknown', 'reason': 'slice limit', 'register': regname(reg)}
    if not defs:
        return {'kind': 'unknown', 'reason': 'no reaching definition', 'register': regname(reg)}
    if len(defs) > 1:
        parts = []
        for d in sorted(defs, key=lambda d: -1 if d == 'entry' else d.index)[:8]:
            if d == 'entry':
                parts.append({'kind': 'parameter', 'register': regname(reg)})
            else:
                parts.append(describe_def(fn, d, reg, depth, trail))
        return {'kind': 'phi', 'register': regname(reg), 'definitions': parts}
    d = next(iter(defs))
    if d == 'entry':
        return {'kind': 'parameter', 'register': regname(reg)}
    return describe_def(fn, d, reg, depth, trail)


def describe_def(fn, d, reg, depth, trail):
    if d.index in trail:
        # The value is being described already: a copy cycle around a loop (x = y; ...; y = x).
        return {'kind': 'cycle', 'register': regname(reg), 'definition': insn_json(d)}
    if depth <= 0:
        return {'kind': 'opaque', 'register': regname(reg), 'definition': insn_json(d)}
    trail = trail + [d.index]
    w = d.word
    if d.kills_calls and reg not in d.writes:
        return {'kind': 'call_clobber', 'register': regname(reg), 'definition': insn_json(d)}
    a = decode_adrp(w)
    if a and a[0] == reg:
        return {'kind': 'page', 'register': regname(reg), 'page': hex((d.addr & ~0xfff) + a[1]),
                'definition': insn_json(d)}
    a = decode_adr(w)
    if a and a[0] == reg:
        return {'kind': 'address', 'register': regname(reg), 'address': hex(d.addr + a[1]),
                'definition': insn_json(d), 'from': 'adr'}
    a = decode_add_imm(w)
    if a and a[0] == reg:
        base = describe(fn, d, a[1], depth - 1, trail)
        if base['kind'] == 'page':
            addr = int(base['page'], 16) + a[2]
            sec = fn.elf.section_of(addr)
            return {'kind': 'address', 'register': regname(reg), 'address': hex(addr),
                    'section': sec['name'] if sec else None, 'definition': insn_json(d), 'base': base}
        return {'kind': 'add_immediate', 'register': regname(reg), 'immediate': a[2],
                'definition': insn_json(d), 'base': base}
    m = decode_mov_reg(w)
    c = decode_copy32(w)
    if (m and m[1] == reg) or (c and c[0] == reg):
        src = m[2] if m and m[1] == reg else c[1]
        width = 64 if (m and m[1] == reg and m[0]) else 32
        inner = describe(fn, d, src, depth - 1, trail)
        return {'kind': 'copy', 'register': regname(reg), 'width': width,
                'definition': insn_json(d), 'source': inner}
    l = decode_ldr_imm(w)
    if l and l[0] == reg:
        if l[1] == 31:
            return {'kind': 'load', 'register': regname(reg), 'offset': l[2], 'size': l[3],
                    'definition': insn_json(d), 'base': {'kind': 'stack', 'register': 'sp'}}
        base = describe(fn, d, l[1], depth - 1, trail)
        out = {'kind': 'load', 'register': regname(reg), 'offset': l[2], 'size': l[3],
               'definition': insn_json(d), 'base': base}
        if base['kind'] in ('address', 'page'):
            addr = int(base.get('address') or base['page'], 16) + l[2]
            out.update(resolve_static_load(fn.elf, addr, l[3]))
        return out
    l = decode_ldr_reg(w)
    if l and l[0] == reg:
        return {'kind': 'indexed_load', 'register': regname(reg), 'size': l[3], 'signed': l[4],
                'shift': l[6], 'definition': insn_json(d),
                'base': describe(fn, d, l[1], depth - 1, trail),
                'index': describe(fn, d, l[2], depth - 1, trail)}
    s = decode_add_shifted(w)
    if s and s[0] == reg:
        return {'kind': 'add_register', 'register': regname(reg), 'shift_type': s[3], 'shift': s[4],
                'definition': insn_json(d),
                'operands': [describe(fn, d, s[1], depth - 1, trail), describe(fn, d, s[2], depth - 1, trail)]}
    return {'kind': 'opaque', 'register': regname(reg), 'definition': insn_json(d)}


def prune(desc, depth=6, phi_limit=4):
    """Bounded copy of a value description for the evidence files (loops expand without bound)."""
    if not isinstance(desc, dict):
        return desc
    if depth <= 0:
        return {'kind': 'elided', 'was': desc.get('kind'), 'register': desc.get('register')}
    out = {}
    for key, value in desc.items():
        if key == 'definitions':
            out[key] = [prune(d, depth - 1, phi_limit) for d in value[:phi_limit]]
            if len(value) > phi_limit:
                out['definitions_elided'] = len(value) - phi_limit
        elif key == 'operands':
            out[key] = [prune(d, depth - 1, phi_limit) for d in value]
        elif key in ('base', 'source', 'index'):
            out[key] = prune(value, depth - 1, phi_limit)
        else:
            out[key] = value
    return out


def resolve_static_load(elf, addr, size):
    """What a load from a link-time constant address reads: relocation, symbol, raw bytes."""
    sec = elf.section_of(addr)
    out = {'static_address': hex(addr), 'static_section': sec['name'] if sec else None}
    reloc = elf.relocs.get(addr)
    if reloc:
        out['relocation'] = {'type': reloc['type'], 'symbol': reloc['symbol'], 'addend': reloc['addend']}
        # A relocated 8-byte slot in .got holds the address of a global: the load yields &global.
        out['got_slot'] = bool(sec and sec['name'] == '.got' and size == 8)
    raw = elf.read(addr, size)
    if raw is not None and sec and sec['type'] != 8:
        out['raw'] = raw.hex()
    return out


# ------------------------------------------------------------ jump-table proofs
def edge_bound(cond, taken, imm):
    """Upper bound (entry count) an edge proves for an unsigned index compared with imm.

    Returns (count, note) or (None, reason)."""
    effective = cond if taken else invert(cond)
    if effective == 'ls':
        return imm + 1, f'{effective}: index <= {imm}'
    if effective == 'lo':
        return imm, f'{effective}: index < {imm}'
    if effective == 'eq':
        return imm + 1, f'{effective}: index == {imm}'
    if effective in ('le', 'lt', 'ge', 'gt', 'mi', 'pl'):
        return None, f'{effective}: signed comparison does not bound an unsigned index'
    return None, f'{effective}: edge does not bound the index'


def invert(cond):
    pairs = {'eq': 'ne', 'hs': 'lo', 'mi': 'pl', 'vs': 'vc', 'hi': 'ls', 'ge': 'lt', 'gt': 'le'}
    inv = {**pairs, **{v: k for k, v in pairs.items()}}
    return inv.get(cond, cond)


def find_guards(fn, load, index_reg):
    """Walk backwards from the table load collecting a bound for every incoming path.

    The tracked register may change through 32-bit register copies. A path ends with a
    bound when it crosses a B.cond whose flags come from `cmp Wreg, #imm` with the edge
    implying an upper bound; it ends without one at a non-copy definition or at entry.
    """
    results = {'bounds': [], 'unbounded_paths': [], 'complete': True, 'guards': []}
    start = [(p, index_reg, load) for p in load.pred]
    seen = set()
    work = list(start)
    visited = 0
    while work:
        p, reg, came_from = work.pop()
        key = (p.index, reg, came_from.index)
        if key in seen:
            continue
        seen.add(key)
        visited += 1
        if visited > SLICE_LIMIT:
            results['complete'] = False
            break
        if p.kind == 'bcond' and p.index + 1 < len(fn.insns):
            taken = came_from.addr == p.target and came_from.index != p.index + 1
            flag_setter = find_flag_setter(fn, p)
            if flag_setter is not None:
                s = decode_subs_imm(flag_setter.word)
                if s and s[1] == 31 and s[2] == reg and s[0] == 0:
                    count, note = edge_bound(p.cond, taken, s[3])
                    guard = {'compare': insn_json(flag_setter), 'branch': insn_json(p),
                             'edge': 'taken' if taken else 'fallthrough', 'register': regname(reg, 'w'),
                             'note': note}
                    results['guards'].append(guard)
                    if count is not None:
                        results['bounds'].append(count)
                        continue
                    results['unbounded_paths'].append({**guard, 'reason': note})
                    continue
        if reg in p.writes or (p.kills_calls and (reg <= 18 or reg == 30)):
            c = decode_copy32(p.word)
            if c and c[0] == reg:
                for q in p.pred:
                    work.append((q, c[1], p))
                if not p.pred:
                    results['unbounded_paths'].append({'reason': 'copy at entry', 'at': insn_json(p)})
                continue
            results['unbounded_paths'].append({'reason': 'definition without guard', 'at': insn_json(p)})
            continue
        if not p.pred:
            results['unbounded_paths'].append({'reason': 'function entry reached', 'at': insn_json(p)})
        for q in p.pred:
            work.append((q, reg, p))
    return results


def find_flag_setter(fn, bcond):
    """Nearest preceding flag-writing instruction on the unique linear predecessor chain."""
    cur = bcond
    for _ in range(12):
        if len(cur.pred) != 1:
            return None
        cur = cur.pred[0]
        if cur.mnemonic in FLAG_WRITERS:
            return cur
        # CBZ/CBNZ/TBZ/TBNZ and ordinary instructions leave NZCV untouched.
        if cur.kind not in ('other', 'cbz', 'tbz'):
            return None
    return None


def zero_extended_index(fn, load, index_reg, extend):
    """True if every definition of the 64-bit index is a 32-bit write (or the load extends w)."""
    if extend in (2, 6):   # UXTW / SXTW: the load itself extends a W register
        return True, 'load extends a W register'
    defs, complete = fn.reaching_defs(load, index_reg)
    if not complete or not defs or 'entry' in defs:
        return False, 'index definition unknown or parameter'
    bad = [d for d in defs if index_reg not in d.writes32
           and not (decode_copy32(d.word) and decode_copy32(d.word)[0] == index_reg)]
    if bad:
        return False, 'index has a 64-bit definition: ' + '; '.join(d.text for d in bad)
    return True, 'all index definitions are 32-bit writes'


def analyse_jump_table(fn, br, desc):
    """Prove a PC-relative jump table from the description of the BR register."""
    if desc['kind'] != 'add_register' or desc['shift_type'] != 0:
        return None
    ops = [origin_of(o) for o in desc['operands']]
    base = next((o for o in ops if o['kind'] == 'address'), None)
    entry = next((o for o in ops if o['kind'] == 'indexed_load'), None)
    if base is None or entry is None:
        return None
    if origin_of(entry['base']).get('definition') != base.get('definition'):
        return {'status': 'unresolved', 'reason': 'entry load base is not the table base'}
    table = int(base['address'], 16)
    shift = desc['shift']
    entry_size, signed = entry['size'], entry['signed']
    load_insn = fn.by_addr[int(entry['definition']['address'], 16)]
    l = decode_ldr_reg(load_insn.word)
    index_reg = l[2]
    if l[6] not in (0, entry_size.bit_length() - 1):
        return {'status': 'unresolved', 'reason': 'unexpected index scale on entry load'}
    guards = find_guards(fn, load_insn, index_reg)
    extended, extend_note = zero_extended_index(fn, load_insn, index_reg, l[5])
    proved_count = None
    if guards['bounds'] and not guards['unbounded_paths'] and guards['complete'] and extended:
        proved_count = max(guards['bounds'])
    # Read entries: the proved count, else probe while targets stay inside the function.
    probe = proved_count if proved_count else 1024
    targets, entries = [], []
    for k in range(probe):
        raw = fn.elf.read(table + k * entry_size, entry_size)
        if raw is None:
            break
        value = int.from_bytes(raw, 'little', signed=signed)
        target = table + (value << shift)
        if proved_count is None and (not fn.contains(target) or target % 4):
            break
        entries.append(value)
        targets.append(target)
    sec = fn.elf.section_of(table)
    provisional = bool(base.get('provisional')) or bool(origin_of(entry['base']).get('provisional'))
    result = {
        'status': ('provisional' if provisional else 'proved') if proved_count else 'partial',
        'table_address': hex(table), 'table_section': sec['name'] if sec else None,
        'entry_size': entry_size, 'entry_signed': signed, 'target_shift': shift,
        'index_register': regname(index_reg, 'w'), 'index_zero_extended': extended,
        'index_note': extend_note, 'guards': guards,
        'entry_count': len(targets), 'entry_count_proved': proved_count is not None,
        'entries': [hex(v) for v in entries], 'targets': [hex(t) for t in targets],
        'relative_targets': [t - fn.start for t in targets],
        'all_targets_inside_function': all(fn.contains(t) and t % 4 == 0 for t in targets) and bool(targets),
        'table_bytes': (fn.elf.read(table, len(targets) * entry_size) or b'').hex(),
    }
    if proved_count and not result['all_targets_inside_function']:
        result['status'] = 'unresolved'
        result['reason'] = 'proved bound yields a target outside the function'
    if proved_count is None:
        result['reason'] = 'bound not proved on every path: ' + json.dumps(
            guards['unbounded_paths'][:3]) if guards['unbounded_paths'] else (
            'index extension unproved: ' + extend_note if not extended else 'no guard found')
    return result


def origin_of(desc):
    """Follow register copies to the underlying value description; collapse phis of one address."""
    while True:
        if desc.get('kind') == 'copy':
            desc = desc['source']
            continue
        if desc.get('kind') == 'phi':
            parts = [origin_of(d) for d in desc['definitions']]
            addresses = {p.get('address') for p in parts if p.get('kind') == 'address'}
            dead = [p for p in parts if p.get('kind') == 'unknown' and p.get('reason') == 'no reaching definition']
            cycles = [p for p in parts if p.get('kind') == 'cycle']
            if len(addresses) == 1 and all(p.get('kind') == 'address' or p in dead or p in cycles for p in parts):
                first = next(p for p in parts if p.get('kind') == 'address')
                out = {**first, 'phi_definitions': [p['definition'] for p in parts if 'definition' in p]}
                if dead:
                    # Some path starts in a block with no direct predecessor: it can only be
                    # entered through an unresolved indirect edge. Accept provisionally and
                    # re-verify once this table's own edges are in the CFG.
                    out['provisional'] = True
                return out
        return desc


def classify_tail_call(desc):
    """Label a memory-loaded or argument BR target. Returns (subtype, note)."""
    desc = origin_of(desc)
    k = desc['kind']
    if k == 'parameter':
        return 'argument_register', 'target is an unmodified incoming argument register'
    if k != 'load':
        return None, None
    b = origin_of(desc['base'])
    if b['kind'] == 'load' and b.get('got_slot'):
        return 'global_function_pointer_via_got', 'pointer value of a named global reached through a GOT slot'
    if b['kind'] in ('address', 'page'):
        return 'global_function_pointer_direct', 'pointer value loaded from a link-time constant address'
    if b['kind'] == 'parameter':
        return 'field_function_pointer_via_parameter', f'pointer field of argument {b["register"]}'
    if b['kind'] == 'stack':
        return 'stack_slot_reload', 'pointer reloaded from a stack slot; the stored value is not traced'
    if b['kind'] == 'add_immediate':
        return 'field_function_pointer_via_pointer_arithmetic', 'pointer field at a computed address'
    if b['kind'] == 'load' and origin_of(b['base']).get('got_slot'):
        return 'field_function_pointer_via_global_object_got', 'pointer field of an object that is the value of a named global reached through a GOT slot'
    if b['kind'] == 'load' and desc.get('offset', 0) != 0 and b.get('offset', 0) != 0:
        # pointer field at a nonzero offset of an object that was itself a nonzero-offset field
        return 'field_function_pointer_via_loaded_object', 'pointer field of an object pointer loaded from memory'
    if b['kind'] == 'load':
        # vtable-style: slot <- vtable pointer <- object pointer
        o = origin_of(b['base'])
        if o['kind'] == 'load' and o.get('got_slot'):
            return 'vtable_slot_via_global_object_got', 'object pointer is a named global reached through a GOT slot'
        if o['kind'] == 'load' and origin_of(o['base']).get('got_slot'):
            return 'vtable_slot_via_global_object_got', 'object pointer is the value of a named global reached through a GOT slot'
        if o['kind'] == 'load' and origin_of(o['base']).get('kind') in ('address', 'page'):
            return 'vtable_slot_via_global_object_direct', 'object pointer is the value of a global at a link-time constant address'
        if o['kind'] in ('address', 'page'):
            return 'vtable_slot_via_global_object_direct', 'object pointer loaded from a link-time constant address'
        if o['kind'] == 'parameter':
            return 'vtable_slot_via_parameter', f'vtable pointer loaded from argument {o["register"]}'
        if o['kind'] == 'load':
            return 'vtable_slot_via_loaded_object', 'object pointer itself loaded from memory'
        if o['kind'] == 'add_immediate':
            return 'vtable_slot_via_pointer_arithmetic', 'object pointer at a computed address'
        if o['kind'] == 'call_clobber':
            return 'vtable_slot_via_call_result', 'object pointer is a call result'
        return 'vtable_slot_via_other', f'object pointer comes from {o["kind"]}'
    if b['kind'] == 'call_clobber':
        return 'field_function_pointer_via_call_result', 'pointer field of a call result'
    return 'field_function_pointer_via_other', f'pointer field read from {b["kind"]}'


def resolve_got_chain(desc):
    """Name the GOT slot / global behind a load chain if present (for cross-ELF comparison)."""
    cur = desc
    names = []
    for _ in range(CHAIN_DEPTH):
        if cur.get('kind') == 'load':
            if 'relocation' in cur:
                r = cur['relocation']
                names.append({'slot': cur['static_address'], 'section': cur['static_section'],
                              'type': r['type'], 'symbol': r['symbol'], 'addend': r['addend']})
            cur = cur['base']
        elif cur.get('kind') == 'copy':
            cur = cur['source']
        else:
            break
    return names


def analyse_branch(fn, br, data_symbols=None):
    reg = rn(br.word)
    desc = describe(fn, br, reg)
    out = {'address': hex(br.addr), 'word': f'0x{br.word:08x}', 'register': regname(reg),
           'value': prune(desc), 'pattern': 'other', 'proof': 'unresolved'}
    jt = analyse_jump_table(fn, br, desc)
    if jt is not None:
        out['pattern'] = 'switch_dispatch'
        out['jump_table'] = jt
        out['proof'] = {'proved': 'proved', 'partial': 'partial', 'unresolved': 'unresolved',
                        'provisional': 'provisional'}[jt['status']]
        return out
    subtype, note = classify_tail_call(desc)
    if subtype:
        out['pattern'] = 'indirect_tail_call'
        out['subtype'] = subtype
        out['note'] = note
        out['proof'] = 'inferred'
        got = resolve_got_chain(desc)
        if got:
            out['static_chain'] = got
            if data_symbols is not None:
                for g in got:
                    if g['type'] == R_AARCH64_RELATIVE:
                        g['data_symbol'] = data_symbols.lookup(g['addend'])
        return out
    if desc['kind'] == 'phi':
        subtypes = {classify_tail_call(d)[0] for d in desc['definitions']}
        if None not in subtypes:
            out['pattern'] = 'indirect_tail_call'
            out['subtype'] = 'phi:' + '|'.join(sorted(subtypes))
            out['proof'] = 'inferred'
            return out
    out['pattern'] = 'unresolved' if desc['kind'] in ('unknown', 'opaque', 'phi', 'call_clobber') else 'other'
    return out


class DataSymbols:
    def __init__(self, text):
        import bisect
        rows = sorted((int(r['Address'], 16) - BASE, r['Name']) for r in csv.DictReader(io.StringIO(text)))
        self.addrs = [a for a, _ in rows]
        self.names = [n for _, n in rows]
        self.bisect = bisect

    def lookup(self, addr):
        i = self.bisect.bisect_right(self.addrs, addr) - 1
        if i < 0:
            return None
        off = addr - self.addrs[i]
        return self.names[i] + (f'+0x{off:x}' if off else '')


# ------------------------------------------------------------------ comparison
def compare_branch(orig, rebuilt):
    diffs = []
    if orig['pattern'] != rebuilt['pattern']:
        diffs.append(f"pattern {orig['pattern']} vs {rebuilt['pattern']}")
    if orig.get('subtype') != rebuilt.get('subtype'):
        diffs.append(f"subtype {orig.get('subtype')} vs {rebuilt.get('subtype')}")
    if orig['register'] != rebuilt['register']:
        diffs.append(f"register {orig['register']} vs {rebuilt['register']}")
    jo, jr = orig.get('jump_table'), rebuilt.get('jump_table')
    if jo and jr:
        for key in ('entry_size', 'entry_signed', 'target_shift', 'index_register', 'entry_count'):
            if jo[key] != jr[key]:
                diffs.append(f'jump table {key} {jo[key]} vs {jr[key]}')
        if jo['relative_targets'] != jr['relative_targets']:
            diffs.append('jump table relative targets differ')
        if jo['status'] != jr['status']:
            diffs.append(f"jump table proof {jo['status']} vs {jr['status']}")
    so, sr = orig.get('static_chain', []), rebuilt.get('static_chain', [])
    if so or sr:
        on = [s.get('data_symbol') or s.get('symbol') for s in so]
        rn_ = [s.get('symbol') for s in sr]
        if len(on) != len(rn_):
            diffs.append(f'static chain length {len(on)} vs {len(rn_)}')
        else:
            for a, b in zip(on, rn_):
                if a and b and a.split('+')[0] != b:
                    diffs.append(f'static symbol {a} vs {b}')
    return diffs


def strip_value(desc):
    """Keep the structure of a value description but drop the noisy raw/instruction texts."""
    return desc


def resolve_switches(fn, brs, data_symbols):
    """Analyse every BR to a fixed point over proved (and provisionally proved) switch edges.

    Returns (analyses by BR address, resolved edges, passes). A provisional table whose base
    still depends on a predecessor-less block after its own edges are present is downgraded
    to unresolved and its edges withdrawn.
    """
    analyses, edges, passes = {}, {}, 0
    while passes < 8:
        passes += 1
        fn.build_cfg(edges)
        changed = False
        for br in brs:
            analysis = analyse_branch(fn, br, data_symbols)
            analysis['resolved_in_pass'] = passes
            previous = analyses.get(br.addr)
            analyses[br.addr] = analysis
            jt = analysis.get('jump_table')
            new_edges = [int(t, 16) for t in jt['targets']] if jt and jt['status'] in ('proved', 'provisional') else None
            if new_edges != edges.get(br.addr):
                changed = True
                if new_edges is None:
                    edges.pop(br.addr, None)
                else:
                    edges[br.addr] = new_edges
        if not changed:
            break
    # Final consistency: nothing may remain provisional once the edge set is stable.
    for br in brs:
        analysis = analyses[br.addr]
        jt = analysis.get('jump_table')
        if jt and jt['status'] == 'provisional':
            jt['status'] = 'unresolved'
            jt['reason'] = 'table base depends on a block without predecessors even with all resolved edges'
            analysis['proof'] = 'unresolved'
            edges.pop(br.addr, None)
    fn.build_cfg()
    return analyses, edges, passes


# ------------------------------------------------------------------------ main
def locate_rebuilt(name, rebuilt, linkmap):
    bare, _, qualifier = name.partition('@')
    candidates = [s for s in rebuilt.symbols if s['name'] == bare and s['type'] == 2 and s['size']]
    if qualifier:
        suffix = '/' + qualifier + '.obj'
        matched = []
        for s in candidates:
            sec = linkmap.input_section_at(s['value'])
            if sec and sec[2].endswith(suffix):
                matched.append(s)
        candidates = matched
    unique = {(s['value'], s['size']) for s in candidates}
    if len(unique) != 1:
        return None, f'{len(unique)} rebuilt candidates'
    value, size = next(iter(unique))
    return {'address': value, 'size': size}, None


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--root', type=Path, default=ROOT)
    parser.add_argument('--only', nargs='*', help='restrict to these function names')
    args = parser.parse_args()
    root = args.root.resolve()
    output = args.output.resolve()
    if output.is_relative_to(root / 'data'):
        raise SystemExit('output must not be under data/')
    objdump = find_objdump()
    objdump_version = subprocess.run([str(objdump), '--version'], capture_output=True, text=True).stdout.splitlines()[0]

    inputs = {}
    for rel in (RECONCILIATION, INVENTORY, OWNERS, DATA_SYMBOLS, ORIGINAL_ELF, REBUILT_ELF, REBUILT_MAP):
        inputs[rel] = sha256((root / rel).read_bytes())
    reconciliation = json.loads((root / RECONCILIATION).read_text())
    for rel in (INVENTORY, OWNERS, ORIGINAL_ELF):
        if reconciliation['inputs_sha256'].get(rel) != inputs[rel]:
            raise SystemExit(f'input differs from the P19-F02 reconciliation pin: {rel}')

    inventory = {(int(r['Address'], 16), r['Name']): r
                 for r in csv.DictReader((root / INVENTORY).open())}
    original = Elf(root / ORIGINAL_ELF)
    rebuilt = Elf(root / REBUILT_ELF)
    linkmap = LinkMap(root / REBUILT_MAP)
    data_symbols = DataSymbols((root / DATA_SYMBOLS).read_text())

    functions = []
    for entry in reconciliation['functions']:
        if args.only and entry['name'] not in args.only:
            continue
        identity = (int(entry['address'], 16), entry['name'])
        row = inventory[identity]
        start = identity[0] - BASE
        size = int(row['Size'])
        orig_fn = Function(entry['name'], original, start, size, disassemble(objdump, original.path, start, size))
        record = {'name': entry['name'], 'address': entry['address'], 'original_start': hex(start),
                  'original_bytes': size, 'prior_quality': row['Quality'],
                  'original_owner_candidates': entry['original_owner_candidates'],
                  'f02_reason': entry['reason'], 'branches': []}
        loc, err = locate_rebuilt(entry['name'], rebuilt, linkmap)
        reb_fn = None
        if loc is None:
            record['rebuilt'] = {'error': err}
        else:
            record['rebuilt'] = {'address': hex(loc['address']), 'size': loc['size']}
            sec = linkmap.input_section_at(loc['address'])
            record['rebuilt']['object'] = sec[2] if sec else None
            if loc['size'] == size:
                reb_fn = Function(entry['name'], rebuilt, loc['address'], loc['size'],
                                  disassemble(objdump, rebuilt.path, loc['address'], loc['size']))
            else:
                record['rebuilt']['error'] = 'size differs'

        direct_reachable = orig_fn.reachable()
        brs = [i for i in orig_fn.insns if i.kind == 'br']
        reb_brs = [i for i in reb_fn.insns if i.kind == 'br'] if reb_fn else []
        reb_reachable = reb_fn.reachable() if reb_fn else set()
        analyses, resolved_edges, passes = resolve_switches(orig_fn, brs, data_symbols)
        orig_fn.build_cfg(resolved_edges)
        with_edges = orig_fn.reachable()
        orig_fn.build_cfg()
        for br in brs:
            analysis = analyses[br.addr]
            analysis['reachable_direct_cfg'] = br.index in direct_reachable
            analysis['reachable_with_proved_switch_edges'] = br.index in with_edges
            record['branches'].append(analysis)
        record['switch_resolution_passes'] = passes
        unresolved_reachable = [a for a in record['branches']
                                if a['reachable_with_proved_switch_edges'] and a['proof'] != 'proved']
        if reb_fn:
            if len(reb_brs) != len(brs) or any(a.index != b.index for a, b in zip(brs, reb_brs)):
                record['rebuilt']['error'] = 'BR positions differ'
            else:
                reb_analyses, reb_edges, _ = resolve_switches(reb_fn, reb_brs, None)
                reb_fn.build_cfg()
                for analysis, rbr in zip(record['branches'], reb_brs):
                    ra = reb_analyses[rbr.addr]
                    ra['reachable_direct_cfg'] = rbr.index in reb_reachable
                    analysis['rebuilt'] = ra
                    analysis['differences'] = compare_branch(analysis, ra)
                    rt = ra.get('jump_table')
                    if rt:
                        table = int(rt['table_address'], 16)
                        sec = linkmap.input_section_at(table)
                        if sec:
                            end = table + rt['entry_count'] * rt['entry_size']
                            rt['map_input_section'] = {'object': sec[2], 'section': sec[3],
                                                       'address': hex(sec[0]), 'size': sec[1],
                                                       'table_within_section': sec[0] <= table and end <= sec[0] + sec[1],
                                                       'section_is_function_rodata': sec[3] == '.rodata.' + entry['name'].split('@')[0]}
                        else:
                            rt['map_input_section'] = None
                        if analysis.get('jump_table') and (not sec or not rt['map_input_section']['table_within_section']):
                            analysis['differences'].append('rebuilt table not within a mapped .rodata input section')
        record['recursive_entry_bl'] = sum(1 for i in orig_fn.insns if i.kind == 'bl' and i.target == start)
        record['interior_bl'] = sum(1 for i in orig_fn.insns if i.kind == 'bl'
                                    and orig_fn.contains(i.target) and i.target != start)
        record['direct_reachable_instructions'] = len(direct_reachable)
        record['instructions'] = len(orig_fn.insns)
        patterns = sorted({a['pattern'] for a in record['branches']})
        proofs = Counter(a['proof'] for a in record['branches'])
        record['patterns'] = patterns
        record['proof_counts'] = dict(proofs)
        if all(a['proof'] == 'proved' for a in record['branches']) and record['branches']:
            status = 'all_proved'
        elif not unresolved_reachable:
            status = 'all_reachable_proved'
        elif all(a['proof'] in ('proved', 'inferred') for a in record['branches']):
            status = 'inferred_only'
        else:
            status = 'unresolved'
        record['function_status'] = status
        record['rebuilt_agrees'] = (reb_fn is not None and 'error' not in record['rebuilt']
                                    and all(not a.get('differences') for a in record['branches']))
        functions.append(record)
        print(f"{entry['name'][:70]:70} {status:22} {','.join(patterns)}", file=sys.stderr)

    # Coverage without double counting: each function counted once.
    def bucket(records, key):
        counter = Counter()
        for r in records:
            counter[key(r)] += 1
        return dict(counter)

    summary = {
        'format': 1,
        'note': 'Read-only evidence. Pattern labels describe instruction structure; "proved" means the '
                'table bound, entry bytes and in-function targets were verified in both ELFs; "inferred" '
                'means the target is loaded from memory or an argument and cannot be enumerated statically. '
                'No function is credited as exact by this script.',
        'inputs_sha256': inputs,
        'f02_candidate_checker_sha256': reconciliation['candidate_checker_sha256'],
        'objdump': objdump_version,
        'function_count': len(functions),
        'original_bytes': sum(r['original_bytes'] for r in functions),
        'branch_count': sum(len(r['branches']) for r in functions),
        'functions_by_status': bucket(functions, lambda r: r['function_status']),
        'bytes_by_status': {s: sum(r['original_bytes'] for r in functions if r['function_status'] == s)
                            for s in {r['function_status'] for r in functions}},
        'functions_by_pattern_set': bucket(functions, lambda r: '+'.join(r['patterns'])),
        'bytes_by_pattern_set': {p: sum(r['original_bytes'] for r in functions if '+'.join(r['patterns']) == p)
                                 for p in {'+'.join(r['patterns']) for r in functions}},
        'branches_by_pattern': dict(Counter(a['pattern'] for r in functions for a in r['branches'])),
        'branches_by_subtype': dict(Counter(a.get('subtype', a['pattern']) for r in functions for a in r['branches'])),
        'branches_by_proof': dict(Counter(a['proof'] for r in functions for a in r['branches'])),
        'branches_unreachable_direct_cfg': sum(1 for r in functions for a in r['branches'] if not a['reachable_direct_cfg']),
        'branches_reachable_only_via_switch_edges': sum(
            1 for r in functions for a in r['branches']
            if a['reachable_with_proved_switch_edges'] and not a['reachable_direct_cfg']),
        'proved_tables_in_rebuilt_function_rodata': sum(
            1 for r in functions for a in r['branches']
            if a.get('jump_table', {}).get('status') == 'proved'
            and (a.get('rebuilt', {}).get('jump_table', {}).get('map_input_section') or {}).get('section_is_function_rodata')),
        'proved_tables_in_other_rebuilt_section': [
            (r['name'], a['address'], a['rebuilt']['jump_table']['map_input_section']['section'])
            for r in functions for a in r['branches']
            if a.get('jump_table', {}).get('status') == 'proved'
            and a.get('rebuilt', {}).get('jump_table', {}).get('map_input_section')
            and not a['rebuilt']['jump_table']['map_input_section']['section_is_function_rodata']],
        'functions_rebuilt_agrees': sum(1 for r in functions if r['rebuilt_agrees']),
        'functions_rebuilt_disagrees': [r['name'] for r in functions if not r['rebuilt_agrees']],
    }
    for r in functions:
        for a in r['branches']:
            # The rebuilt value tree repeats the original one when nothing differs; keep the
            # rebuilt jump-table record and chain, drop the duplicate tree to keep the file small.
            if a.get('rebuilt') and not a.get('differences'):
                a['rebuilt'].pop('value', None)
    output.mkdir(parents=True, exist_ok=True)
    (output / 'summary.json').write_text(json.dumps(summary, indent=2, sort_keys=True) + '\n')
    (output / 'branches.json').write_text(json.dumps({'format': 1, 'inputs_sha256': inputs,
                                                      'functions': functions}, indent=1) + '\n')
    (output / 'representatives.json').write_text(json.dumps(
        {'format': 1, 'inputs_sha256': inputs,
         'functions': [r for r in functions if r['name'] in REPRESENTATIVES]}, indent=1) + '\n')
    compact = []
    for r in functions:
        compact.append({k: r[k] for k in ('name', 'address', 'original_bytes', 'original_owner_candidates',
                                           'patterns', 'proof_counts', 'function_status', 'rebuilt_agrees',
                                           'recursive_entry_bl', 'interior_bl')}
                       | {'rebuilt': r['rebuilt']}
                       | {'branches': [{k: a.get(k) for k in ('address', 'register', 'pattern', 'subtype',
                                                              'proof', 'reachable_direct_cfg',
                                                              'reachable_with_proved_switch_edges')}
                                       | ({'entry_count': a['jump_table']['entry_count'],
                                           'table_address': a['jump_table']['table_address']}
                                          if a.get('jump_table') else {})
                                       | ({'differences': a['differences']} if a.get('differences') else {})
                                       for a in r['branches']]})
    (output / 'classification.json').write_text(json.dumps({'format': 1, 'inputs_sha256': inputs,
                                                            'functions': compact}, indent=1) + '\n')
    print(json.dumps({k: v for k, v in summary.items() if k != 'functions_rebuilt_disagrees'}, indent=2))
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
