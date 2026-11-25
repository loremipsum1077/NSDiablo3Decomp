#!/usr/bin/env python3
"""Match functions between two builds of the game (e.g. 2.6.2 -> 2.7.7).

Function boundaries come from each ELF's .eh_frame_hdr/.eh_frame. Each function
is reduced to its instruction words with address-dependent fields masked out
(ADRP pages, :lo12: offsets off ADRP bases, calls and branches leaving the
function, literal loads), then matched in phases:

  1. unique masked hash in both builds              -> "hash"
  2. link order between matched neighbours          -> "order" / "order-fuzzy"
  3. call graph: the i-th call of a matched pair    -> "callgraph"

Every 2.6.2 and 2.7.7 function lands in one bucket:

  identical  masked code equal (existing source is very likely still right)
  changed    paired with an old function, but the code differs
  new        2.7.7 function with no 2.6.2 counterpart
  removed    2.6.2 function with no 2.7.7 counterpart (deleted or inlined)

"identical" is evidence, not proof: data layout, constants reached through
masked offsets, or call targets can still differ. The callee_check column says
whether each call of an identical pair lands on the matched counterpart; the
checker run against the new ELF is the final word.

Outputs (default build/analysis/<new-version>/version_diff/): mapping.csv, summary.md.
"""
from __future__ import annotations

import argparse
import bisect
import csv
import hashlib
import struct
import sys
from collections import Counter, defaultdict
from dataclasses import dataclass, field
from difflib import SequenceMatcher
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
CSV_BASE = 0x7100000000  # functions.csv addresses = module base + ELF vaddr
FUZZY_MIN = 0.5          # minimum similarity for a fuzzy pairing
GAP_PAIR_LIMIT = 400     # gaps with more candidate pairs use a position window
GAP_WINDOW = 8
LONE_PAIR_MIN = 0.3      # one unmatched function on each side, same neighbours
# Opcode-group bits only: register numbers and most immediates are dropped, so
# two compilations of the same source with different register allocation still
# look alike. Pairing uses this; the reported similarity uses full masked words.
SHAPE_MASK = 0xFFE00000


# --------------------------------------------------------------------------- ELF

class ELF:
    def __init__(self, path: Path):
        self.path = path
        self.data = path.read_bytes()
        d = self.data
        if d[:4] != b'\x7fELF' or d[4] != 2 or d[5] != 1:
            raise ValueError(f'{path}: not a little-endian ELF64')
        phoff, = struct.unpack_from('<Q', d, 0x20)
        shoff, = struct.unpack_from('<Q', d, 0x28)
        phentsize, phnum, shentsize, shnum, shstrndx = struct.unpack_from('<HHHHH', d, 0x36)
        self.segments = []  # (vaddr, filesz, offset, flags)
        for i in range(phnum):
            p_type, flags, off, vaddr, _, filesz, _, _ = struct.unpack_from('<IIQQQQQQ', d, phoff + i * phentsize)
            if p_type == 1:
                self.segments.append((vaddr, filesz, off, flags))
        raw = [struct.unpack_from('<IIQQQQIIQQ', d, shoff + i * shentsize) for i in range(shnum)]
        strtab = raw[shstrndx][4]
        self.sections = {}
        for name, _, _, addr, off, size, *_ in raw:
            end = d.index(b'\0', strtab + name)
            self.sections[d[strtab + name:end].decode()] = (addr, off, size)

    def offset(self, vaddr: int) -> int:
        for start, size, off, _ in self.segments:
            if start <= vaddr < start + size:
                return off + vaddr - start
        raise ValueError(f'{self.path}: address {vaddr:#x} is not file-backed')

    def is_exec(self, vaddr: int) -> bool:
        return any(s <= vaddr < s + z and f & 1 for s, z, _, f in self.segments)

    def words(self, vaddr: int, size: int) -> tuple[int, ...]:
        off = self.offset(vaddr)
        return struct.unpack_from(f'<{size // 4}I', self.data, off)


def _uleb(d, o):
    value = shift = 0
    while True:
        b = d[o]; o += 1
        value |= (b & 0x7f) << shift; shift += 7
        if not b & 0x80:
            return value, o


def _sleb(d, o):
    value = shift = 0
    while True:
        b = d[o]; o += 1
        value |= (b & 0x7f) << shift; shift += 7
        if not b & 0x80:
            if b & 0x40:
                value -= 1 << shift
            return value, o


_FIXED = {0x02: '<H', 0x03: '<I', 0x04: '<Q', 0x0a: '<h', 0x0b: '<i', 0x0c: '<q', 0x00: '<Q'}


def _encoded(d, o, enc, field_vaddr, datarel=0, apply=True):
    """Read a DW_EH_PE value at file offset o; return (value, new offset)."""
    if enc == 0xff:
        return None, o
    fmt = enc & 0x0f
    if fmt == 0x01:
        value, o = _uleb(d, o)
    elif fmt == 0x09:
        value, o = _sleb(d, o)
    else:
        f = _FIXED[fmt]
        value, = struct.unpack_from(f, d, o)
        o += struct.calcsize(f)
    if apply:
        app = enc & 0x70
        if app == 0x10:
            value += field_vaddr
        elif app == 0x30:
            value += datarel
        elif app:
            raise ValueError(f'unsupported pointer encoding {enc:#x}')
    return value & 0xffffffffffffffff, o


def eh_functions(elf: ELF) -> dict[int, int]:
    """{start vaddr: size} for every FDE listed in .eh_frame_hdr."""
    d = elf.data
    hdr_vaddr, hdr_off, _ = elf.sections['.eh_frame_hdr']
    version, ptr_enc, count_enc, table_enc = d[hdr_off:hdr_off + 4]
    if version != 1:
        raise ValueError('unsupported .eh_frame_hdr version')
    o = hdr_off + 4
    _, o = _encoded(d, o, ptr_enc, hdr_vaddr + (o - hdr_off), hdr_vaddr)
    count, o = _encoded(d, o, count_enc, hdr_vaddr + (o - hdr_off), hdr_vaddr)
    cie_cache: dict[int, int] = {}
    funcs: dict[int, int] = {}
    for _ in range(count):
        _, o = _encoded(d, o, table_enc, hdr_vaddr + (o - hdr_off), hdr_vaddr)
        fde_vaddr, o = _encoded(d, o, table_enc, hdr_vaddr + (o - hdr_off), hdr_vaddr)
        start, size = _parse_fde(elf, fde_vaddr, cie_cache)
        if size:
            funcs[start] = max(size, funcs.get(start, 0))
    return funcs


def _parse_fde(elf: ELF, fde_vaddr: int, cie_cache: dict[int, int]) -> tuple[int, int]:
    d = elf.data
    o = elf.offset(fde_vaddr)
    length, = struct.unpack_from('<I', d, o); o += 4
    if length == 0xffffffff:
        o += 8
    cie_field = o
    cie_ptr, = struct.unpack_from('<I', d, o); o += 4
    cie_off = cie_field - cie_ptr
    if cie_off not in cie_cache:
        cie_cache[cie_off] = _cie_fde_encoding(d, cie_off)
    enc = cie_cache[cie_off]
    field_vaddr = fde_vaddr + (o - elf.offset(fde_vaddr))
    start, o = _encoded(d, o, enc, field_vaddr)
    size, o = _encoded(d, o, enc & 0x0f, 0, apply=False)
    return start, size


def _cie_fde_encoding(d, o) -> int:
    length, = struct.unpack_from('<I', d, o); o += 4
    if length == 0xffffffff:
        o += 8
    o += 4  # CIE id
    version = d[o]; o += 1
    end = d.index(b'\0', o)
    aug = d[o:end].decode(); o = end + 1
    if 'eh' in aug:
        o += 8
    _, o = _uleb(d, o)
    _, o = _sleb(d, o)
    if version == 1:
        o += 1
    else:
        _, o = _uleb(d, o)
    enc = 0x00
    if aug.startswith('z'):
        _, o = _uleb(d, o)
        for ch in aug[1:]:
            if ch == 'R':
                enc = d[o]; o += 1
            elif ch == 'P':
                penc = d[o]; o += 1
                _, o = _encoded(d, o, penc & 0x0f, 0, apply=False)
            elif ch == 'L':
                o += 1
    return enc


# ------------------------------------------------------------------ masking

def _sext(value: int, bits: int) -> int:
    return value - (1 << bits) if value & (1 << (bits - 1)) else value


def branch_targets(words: tuple[int, ...], start: int) -> list[int]:
    """BL targets and B targets outside the function (tail calls), in order."""
    end = start + 4 * len(words)
    out = []
    for i, w in enumerate(words):
        top6 = w & 0xFC000000
        if top6 in (0x14000000, 0x94000000):
            pc = start + 4 * i
            target = pc + 4 * _sext(w & 0x3FFFFFF, 26)
            if top6 == 0x94000000 or not start <= target < end:
                out.append(target)
    return out


def mask_function(words: tuple[int, ...], start: int) -> tuple[tuple[int, ...], list[int]]:
    """Return (masked words, call targets in order).

    Branches that stay inside the function keep their offsets; anything that
    depends on where code or data was placed by the linker is zeroed.
    """
    end = start + 4 * len(words)
    out = []
    calls = []
    adrp_regs: set[int] = set()
    for i, w in enumerate(words):
        pc = start + 4 * i
        m = w
        top6 = w & 0xFC000000
        if top6 in (0x14000000, 0x94000000):          # B / BL
            target = pc + 4 * _sext(w & 0x3FFFFFF, 26)
            if top6 == 0x94000000:
                calls.append(target)
            if top6 == 0x94000000 or not start <= target < end:
                m = top6
        elif (w & 0x1F000000) == 0x10000000:            # ADR / ADRP
            rd = w & 31
            if w & 0x80000000:
                adrp_regs.add(rd)
                m = w & 0x9F00001F
            else:
                imm = ((w >> 29) & 3) | (((w >> 5) & 0x7FFFF) << 2)
                if not start <= pc + _sext(imm, 21) < end:
                    m = w & 0x9F00001F
                adrp_regs.discard(rd)
        elif (w & 0x3B000000) == 0x18000000:            # LDR (literal)
            target = pc + 4 * _sext((w >> 5) & 0x7FFFF, 19)
            if not start <= target < end:
                m = w & 0xFF00001F
        elif (w & 0x3B000000) == 0x39000000:            # LDR/STR unsigned imm
            rn, rt = (w >> 5) & 31, w & 31
            if rn in adrp_regs:
                m = w & ~(0xFFF << 10) & 0xFFFFFFFF
            if (w >> 22) & 1:                           # load overwrites Rt
                adrp_regs.discard(rt)
        elif (w & 0x7F800000) == 0x11000000:            # ADD (immediate)
            rn, rd = (w >> 5) & 31, w & 31
            if rn in adrp_regs:
                m = w & ~(0x1FFF << 10) & 0xFFFFFFFF
            adrp_regs.discard(rd)
        out.append(m)
    return tuple(out), calls


# ------------------------------------------------------------------ model

@dataclass
class Func:
    addr: int
    size: int
    masked: tuple[int, ...]
    digest: bytes
    calls: list[int]
    shape: tuple[int, ...] = ()
    targets: list[int] = field(default_factory=list)   # calls + tail calls
    name: str = ''
    quality: str = ''
    file: str = ''
    match: 'Func | None' = None
    method: str = ''
    similarity: float = 0.0
    shape_similarity: float = 0.0
    index: int = 0
    extra: dict = field(default_factory=dict)


def load_functions(elf: ELF, csv_rows: dict[int, dict] | None = None) -> list[Func]:
    bounds = eh_functions(elf)
    if csv_rows:
        for addr, row in csv_rows.items():
            if addr not in bounds and row['size'] and elf.is_exec(addr):
                bounds[addr] = row['size']
    funcs = []
    for addr in sorted(bounds):
        size = bounds[addr] & ~3
        if not size or not elf.is_exec(addr):
            continue
        words = elf.words(addr, size)
        masked, calls = mask_function(words, addr)
        f = Func(addr, size, masked, hashlib.blake2b(struct.pack(f'<{len(masked)}I', *masked),
                                                    digest_size=16).digest(), calls,
                 tuple(w & SHAPE_MASK for w in masked), branch_targets(words, addr))
        if csv_rows and addr in csv_rows:
            f.name = csv_rows[addr]['name']
            f.quality = csv_rows[addr]['quality']
            f.file = csv_rows[addr].get('file', '')
        funcs.append(f)
    for i, f in enumerate(funcs):
        f.index = i
    return funcs


def read_function_csvs(functions_csv: Path, files_csv: Path | None) -> dict[int, dict]:
    rows = {}
    with functions_csv.open(newline='') as fh:
        for row in csv.DictReader(fh):
            addr = int(row['Address'], 16) - CSV_BASE
            rows[addr] = {'name': row['Name'], 'quality': row['Quality'],
                          'size': int(row['Size'] or 0)}
    if files_csv and files_csv.exists():
        with files_csv.open(newline='') as fh:
            for row in csv.DictReader(fh):
                addr = int(row['Address'], 16) - CSV_BASE
                if addr in rows:
                    rows[addr]['file'] = row['File']
    return rows


# ------------------------------------------------------------------ matching

def _ratio(x: tuple, y: tuple, floor: float) -> float:
    la, lb = len(x), len(y)
    if not la or not lb or min(la, lb) / max(la, lb) < floor:
        return 0.0
    sm = SequenceMatcher(None, x, y, autojunk=False)
    if sm.real_quick_ratio() < floor or sm.quick_ratio() < floor:
        return 0.0
    return sm.ratio()


def similarity(a: Func, b: Func) -> float:
    """Register-insensitive similarity used to decide pairings."""
    if a.digest == b.digest:
        return 1.0
    return _ratio(a.shape, b.shape, FUZZY_MIN)


def pair(a: Func, b: Func, method: str, score: float | None = None):
    a.match, b.match = b, a
    a.method = b.method = method
    a.shape_similarity = b.shape_similarity = similarity(a, b) if score is None else score
    a.similarity = b.similarity = 1.0 if a.digest == b.digest else _ratio(a.masked, b.masked, 0.0)


def match_unique_hashes(old: list[Func], new: list[Func]) -> int:
    ca = Counter(f.digest for f in old)
    cb = Counter(f.digest for f in new)
    by_digest = {f.digest: f for f in new if cb[f.digest] == 1}
    n = 0
    for f in old:
        if ca[f.digest] == 1 and f.digest in by_digest:
            pair(f, by_digest[f.digest], 'hash', 1.0)
            n += 1
    return n


def _backbone(old: list[Func]) -> list[Func]:
    """Longest run of matched pairs whose order agrees in both builds."""
    matched = [f for f in old if f.match]
    tails: list[int] = []
    tail_idx: list[int] = []
    prev = [-1] * len(matched)
    for i, f in enumerate(matched):
        key = f.match.index
        j = bisect.bisect_left(tails, key)
        if j == len(tails):
            tails.append(key); tail_idx.append(i)
        else:
            tails[j] = key; tail_idx[j] = i
        prev[i] = tail_idx[j - 1] if j else -1
    out = []
    i = tail_idx[-1] if tail_idx else -1
    while i >= 0:
        out.append(matched[i]); i = prev[i]
    return out[::-1]


def _local_anchors(old: list[Func]) -> list[Func]:
    """Matched functions whose new-build index rises with the previous one's."""
    out = []
    for f in old:
        if f.match and (not out or f.match.index > out[-1].match.index):
            out.append(f)
    return out


def match_gaps(old: list[Func], new: list[Func], local: bool = False) -> tuple[int, int]:
    """Pair unmatched functions that sit between the same matched neighbours.

    The first pass uses the longest order-consistent backbone; later passes use
    every adjacent matched pair, so short gaps are judged in isolation. A gap
    holding exactly one function on each side is accepted at LONE_PAIR_MIN.
    """
    anchors = _local_anchors(old) if local else _backbone(old)
    bounds = [(-1, -1)] + [(f.index, f.match.index) for f in anchors] + [(len(old), len(new))]
    exact = fuzzy = 0
    for (a0, b0), (a1, b1) in zip(bounds, bounds[1:]):
        ga = [f for f in old[a0 + 1:a1] if not f.match]
        gb = [f for f in new[b0 + 1:b1] if not f.match]
        if not ga or not gb:
            continue
        sm = SequenceMatcher(None, [f.digest for f in ga], [f.digest for f in gb], autojunk=False)
        for blk in sm.get_matching_blocks():
            for k in range(blk.size):
                pair(ga[blk.a + k], gb[blk.b + k], 'order', 1.0)
                exact += 1
        ga = [f for f in ga if not f.match]
        gb = [f for f in gb if not f.match]
        if not ga or not gb:
            continue
        if len(ga) == 1 and len(gb) == 1:
            s = _ratio(ga[0].shape, gb[0].shape, 0.0)
            if s >= LONE_PAIR_MIN:
                pair(ga[0], gb[0], 'order-fuzzy' if s >= FUZZY_MIN else 'order-lone', s)
                fuzzy += 1
            continue
        candidates = []
        windowed = len(ga) * len(gb) > GAP_PAIR_LIMIT
        for i, fa in enumerate(ga):
            if windowed:
                centre = round(i * len(gb) / len(ga))
                js = range(max(0, centre - GAP_WINDOW), min(len(gb), centre + GAP_WINDOW + 1))
            else:
                js = range(len(gb))
            for j in js:
                s = similarity(fa, gb[j])
                if s >= FUZZY_MIN:
                    candidates.append((s, -abs(i - j), i, j))
        for s, _, i, j in sorted(candidates, reverse=True):
            if not ga[i].match and not gb[j].match:
                pair(ga[i], gb[j], 'order-fuzzy', s)
                fuzzy += 1
    return exact, fuzzy


def match_callgraph(old: list[Func], new: list[Func]) -> int:
    by_old = {f.addr: f for f in old}
    by_new = {f.addr: f for f in new}
    added = 0
    queue = [f for f in old if f.match]
    while queue:
        nxt = []
        for fa in queue:
            fb = fa.match
            # Calls plus tail calls when both sides agree on them, else calls.
            if len(fa.targets) == len(fb.targets):
                edges = zip(fa.targets, fb.targets)
            elif len(fa.calls) == len(fb.calls):
                edges = zip(fa.calls, fb.calls)
            else:
                continue
            for ta, tb in edges:
                ca, cb = by_old.get(ta), by_new.get(tb)
                if not ca or not cb or ca.match or cb.match:
                    continue
                s = similarity(ca, cb)
                if s >= FUZZY_MIN:
                    pair(ca, cb, 'callgraph', s)
                    added += 1
                    nxt.append(ca)
        queue = nxt
    return added


def unpair(f: Func):
    if f.match is not None:
        f.match.match = None
        f.match.method = ''
        f.match = None
        f.method = ''


REPAIR_SHARE = 0.75      # share of caller votes needed to override a pairing


def _support(fa: Func, fb: Func, old: list[Func], new: list[Func]) -> int:
    """How many of fa's two neighbours are paired with fb's matching neighbours."""
    n = 0
    for step in (-1, 1):
        i, j = fa.index + step, fb.index + step
        if 0 <= i < len(old) and 0 <= j < len(new) and old[i].match is new[j]:
            n += 1
    return n


def _movable(fa: Func, fb: Func, old: list[Func], new: list[Func]) -> bool:
    """Re-pair fa with fb only if no unique-hash pairing is broken and fb's
    neighbourhood agrees at least as well as fa's current one does."""
    if fa.method == 'hash' or fb.method == 'hash':
        return False
    current = _support(fa, fa.match, old, new) if fa.match is not None else 0
    return _support(fa, fb, old, new) >= current


def repair_by_callers(old: list[Func], new: list[Func]) -> int:
    """Re-pair call targets using the callers that are surely matched.

    Look-alike functions (empty destructors, one-line getters) share a masked
    hash, so link order is all that pairs them, and it guesses wrong when the
    layout shifts. A caller whose code is identical in both builds, and is
    either uniquely hashed or not tiny, calls the i-th target in each build:
    those votes say which new function each old target is. A clear majority
    that disagrees with the current pairing replaces it; the two displaced
    partners are paired with each other when they look alike, else freed for
    the next gap pass.
    """
    by_old = {f.addr: f for f in old}
    by_new = {f.addr: f for f in new}
    votes: dict[int, Counter] = defaultdict(Counter)
    for a in old:
        b = a.match
        if (b is None or a.digest != b.digest or len(a.targets) != len(b.targets)
                or not (a.method == 'hash' or a.size >= 32)):
            continue
        for ta, tb in zip(a.targets, b.targets):
            if ta in by_old and tb in by_new and ta != a.addr:
                votes[ta][tb] += 1
    changed = 0
    locked_old, locked_new = set(), set()
    ranked = sorted(votes.items(), key=lambda kv: -kv[1].most_common(1)[0][1])
    for ta, c in ranked:
        tb, n = c.most_common(1)[0]
        if n < REPAIR_SHARE * sum(c.values()):
            continue
        fa, fb = by_old[ta], by_new[tb]
        if fa.match is fb:
            locked_old.add(ta); locked_new.add(tb)
            continue
        if ta in locked_old or tb in locked_new or similarity(fa, fb) < 0.9 \
                or not _movable(fa, fb, old, new):
            continue
        old_partner, new_partner = fa.match, fb.match
        unpair(fa)
        unpair(fb)
        pair(fa, fb, 'callers')
        if old_partner is not None and new_partner is not None \
                and new_partner.addr not in locked_old and old_partner.addr not in locked_new \
                and similarity(new_partner, old_partner) >= FUZZY_MIN:
            pair(new_partner, old_partner, 'callers-swap')
        locked_old.add(ta); locked_new.add(tb)
        changed += 1
    return changed


STRONG = ('hash', 'callers', 'neighbour')


def repair_by_neighbours(old: list[Func], new: list[Func]) -> int:
    """Re-pair look-alike functions from their surely-matched neighbours.

    Functions nobody calls directly (deleting destructors reached only through
    vtables) get no caller votes. Link order is kept within a compile unit, so
    when the old function right before (or after) a look-alike is strongly
    matched, the new function right after (or before) its partner is the
    look-alike's counterpart, if the masked code is identical. Strong pairings
    are never undone.
    """
    changed = 0
    for i, fa in enumerate(old):
        if fa.method in STRONG:
            continue
        candidates = []
        for step in (-1, 1):
            j = i + step
            if not 0 <= j < len(old):
                continue
            na = old[j]
            if na.match is None or na.method not in STRONG:
                continue
            k = na.match.index - step
            if 0 <= k < len(new) and new[k].digest == fa.digest:
                candidates.append(new[k])
        if not candidates or any(c is not candidates[0] for c in candidates):
            continue
        fb = candidates[0]
        if fa.match is fb or fb.method in STRONG:
            if fa.match is fb and fa.method not in STRONG:
                fa.method = fb.method = 'neighbour'
            continue
        # Strictly better neighbourhood support than the current pairing.
        current = _support(fa, fa.match, old, new) if fa.match is not None else 0
        claim = _support(fa, fb, old, new)
        if claim <= current or not _movable(fa, fb, old, new):
            continue
        # fb's current partner keeps it if its own neighbours agree as well.
        if fb.match is not None and _support(fb.match, fb, old, new) >= claim:
            continue
        old_partner, new_partner = fa.match, fb.match
        unpair(fa)
        unpair(fb)
        pair(fa, fb, 'neighbour')
        if old_partner is not None and new_partner is not None \
                and new_partner.method not in STRONG and old_partner.method not in STRONG \
                and new_partner.digest == old_partner.digest:
            pair(new_partner, old_partner, 'order')
        changed += 1
    return changed


def callee_check(fa: Func, new_by_addr: dict[int, Func], old_by_addr: dict[int, Func]) -> str:
    if not fa.calls:
        return 'no-calls'
    if len(fa.calls) != len(fa.match.calls):
        return 'count-differs'
    bad = unknown = 0
    for ta, tb in zip(fa.calls, fa.match.calls):
        ca = old_by_addr.get(ta)
        if ca is None or ca.match is None:
            cb = new_by_addr.get(tb)
            unknown += 1 if (cb is None or cb.match is None) else 0
            bad += 0 if (cb is None or cb.match is None) else 1
        elif ca.match.addr != tb:
            bad += 1
    if bad:
        return f'{bad}-mismatched'
    return f'{unknown}-unmatched' if unknown else 'ok'


def run_matching(old: list[Func], new: list[Func]) -> dict[str, int]:
    stats = Counter({'hash': match_unique_hashes(old, new)})
    e, z = match_gaps(old, new)
    stats['order'] += e
    stats['order_fuzzy'] += z
    stats['callgraph'] += match_callgraph(old, new)
    for _ in range(4):
        while True:
            e, z = match_gaps(old, new, local=True)
            c = match_callgraph(old, new)
            stats['order'] += e
            stats['order_fuzzy'] += z
            stats['callgraph'] += c
            stats['local_rounds'] += 1
            if not (e or z or c):
                break
        r = repair_by_callers(old, new)
        n = repair_by_neighbours(old, new)
        stats['caller_repairs'] += r
        stats['neighbour_repairs'] += n
        if not (r or n):
            break
    # Repairs free some functions; give them a last chance to pair.
    while True:
        e, z = match_gaps(old, new, local=True)
        c = match_callgraph(old, new)
        stats['order'] += e
        stats['order_fuzzy'] += z
        stats['callgraph'] += c
        if not (e or z or c):
            break
    return dict(stats)


# ------------------------------------------------------------------ output

def bucket(f: Func, side: str) -> str:
    if f.match is None:
        return 'removed' if side == 'old' else 'new'
    return 'identical' if f.digest == f.match.digest else 'changed'


def write_outputs(old: list[Func], new: list[Func], stats: dict, out: Path,
                  old_label: str, new_label: str):
    out.mkdir(parents=True, exist_ok=True)
    old_by = {f.addr: f for f in old}
    new_by = {f.addr: f for f in new}
    rows = []
    for f in old:
        g = f.match
        rows.append({
            f'addr_{old_label}': f'{CSV_BASE + f.addr:#018x}',
            f'addr_{new_label}': f'{CSV_BASE + g.addr:#018x}' if g else '',
            f'size_{old_label}': f.size,
            f'size_{new_label}': g.size if g else '',
            'bucket': bucket(f, 'old'),
            'similarity': f'{f.similarity:.3f}' if g else '',
            'shape_similarity': f'{f.shape_similarity:.3f}' if g else '',
            'method': f.method,
            'callee_check': callee_check(f, new_by, old_by) if g else '',
            f'quality_{old_label}': f.quality,
            'name': f.name,
            'file': f.file,
        })
    for g in new:
        if g.match is None:
            rows.append({
                f'addr_{old_label}': '', f'addr_{new_label}': f'{CSV_BASE + g.addr:#018x}',
                f'size_{old_label}': '', f'size_{new_label}': g.size, 'bucket': 'new',
                'similarity': '', 'shape_similarity': '', 'method': '', 'callee_check': '',
                f'quality_{old_label}': '', 'name': '', 'file': '',
            })
    with (out / 'mapping.csv').open('w', newline='') as fh:
        w = csv.DictWriter(fh, fieldnames=list(rows[0]))
        w.writeheader()
        w.writerows(rows)

    by_bucket = Counter(r['bucket'] for r in rows)
    q = f'quality_{old_label}'
    cross = Counter((r['bucket'], r[q]) for r in rows if r[q])
    qualities = sorted({r[q] for r in rows if r[q]})
    ident_ok = sum(1 for r in rows if r['bucket'] == 'identical' and r['callee_check'] in ('ok', 'no-calls'))
    ident_o = sum(1 for r in rows if r['bucket'] == 'identical' and r[q] == 'O')
    ident_o_ok = sum(1 for r in rows if r['bucket'] == 'identical' and r[q] == 'O'
                     and r['callee_check'] in ('ok', 'no-calls'))
    changed_o = [r for r in rows if r['bucket'] == 'changed' and r[q] == 'O']
    sim_bands = Counter()
    for r in changed_o:
        s = float(r['shape_similarity'])
        sim_bands['>= 0.95' if s >= .95 else '0.80-0.95' if s >= .8 else '0.50-0.80']+= 1
    new_bytes = sum(r[f'size_{new_label}'] for r in rows if r['bucket'] == 'new')
    files = Counter(r['file'] for r in changed_o if r['file'])

    lines = [f'# Function diff {old_label} -> {new_label}', '',
             f'{old_label}: {len(old)} functions; {new_label}: {len(new)} functions.', '',
             '## Buckets', '', '| bucket | functions |', '|---|---:|']
    for b in ('identical', 'changed', 'new', 'removed'):
        lines.append(f'| {b} | {by_bucket[b]} |')
    lines += ['', f'New code: {new_bytes} bytes in {by_bucket["new"]} functions.', '',
              f'## By {old_label} quality', '',
              '| bucket | ' + ' | '.join(qualities) + ' |',
              '|---|' + '---:|' * len(qualities)]
    for b in ('identical', 'changed', 'removed'):
        lines.append(f'| {b} | ' + ' | '.join(str(cross[(b, x)]) for x in qualities) + ' |')
    lines += ['', '## What this means for matched (O) functions', '',
              f'- **Keep as is:** {ident_o} O functions are identical after masking addresses; '
              f'{ident_o_ok} of them also call the matched counterpart of every callee.',
              f'- **Needs update:** {len(changed_o)} O functions changed '
              f'({sim_bands[">= 0.95"]} at >= 0.95 shape similarity, {sim_bands["0.80-0.95"]} at 0.80-0.95, '
              f'{sim_bands["0.50-0.80"]} at 0.50-0.80).',
              f'- **Removed or inlined:** {cross[("removed", "O")]} O functions have no {new_label} counterpart.',
              f'- Identical functions overall with consistent callees: {ident_ok}.', '',
              '## Source files with the most changed O functions', '',
              '| file | changed O |', '|---|---:|']
    for name, n in files.most_common(25):
        lines.append(f'| `{name}` | {n} |')
    lines += ['', '## Matching phases', '', '| phase | pairs |', '|---|---:|']
    for k, v in stats.items():
        lines.append(f'| {k} | {v} |')
    lines += ['', 'Masked hashes ignore linker-placed addresses, so "identical" means the',
              'instructions agree once code and data positions are factored out. Confirm with',
              'the checker against the new ELF before treating any function as matching.', '']
    (out / 'summary.md').write_text('\n'.join(lines))


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__.split('\n\n')[0])
    ap.add_argument('--old-version', default='2.6.2')
    ap.add_argument('--new-version', default='2.7.7')
    ap.add_argument('--old-elf', type=Path)
    ap.add_argument('--new-elf', type=Path)
    ap.add_argument('--old-functions', type=Path)
    ap.add_argument('--old-files', type=Path)
    ap.add_argument('--out', type=Path)
    args = ap.parse_args(argv)
    ov, nv = args.old_version, args.new_version
    old_elf = args.old_elf or ROOT / 'data' / ov / 'main.elf'
    new_elf = args.new_elf or ROOT / 'data' / nv / 'main.elf'
    old_csv = args.old_functions or ROOT / 'data' / ov / 'functions.csv'
    old_files = args.old_files or ROOT / 'data' / ov / 'function_files.csv'
    out = args.out or ROOT / 'build' / 'analysis' / nv / 'version_diff'
    for p in (old_elf, new_elf, old_csv):
        if not p.is_file():
            print(f'missing input: {p}', file=sys.stderr)
            return 2

    rows = read_function_csvs(old_csv, old_files)
    old = load_functions(ELF(old_elf), rows)
    new = load_functions(ELF(new_elf))
    missing = sum(1 for a in rows if a not in {f.addr for f in old})
    print(f'{ov}: {len(old)} functions ({missing} functions.csv entries not found); {nv}: {len(new)} functions')
    stats = run_matching(old, new)
    write_outputs(old, new, stats, out, ov, nv)
    print((out / 'summary.md').read_text())
    print(f'wrote {out / "mapping.csv"}')
    return 0


if __name__ == '__main__':
    sys.exit(main())
