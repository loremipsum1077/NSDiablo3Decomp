#!/usr/bin/env python3
"""Derive proposed original data identities for checker proof gaps.

Reads only pinned original inputs (main.elf, the unstripped NSS and its DWARF,
data_symbols.csv, functions.csv, function_files.csv) plus retained checker
results. Writes a *proposed* identity CSV and a JSON classification; it never
edits data_symbols.csv, the checker or any inventory.

  python3 tools/data_identity.py \\
      --result build/.../normal/.../result.json --result build/.../lua/.../result.json \\
      --f3 build/checker-f3-20261029/candidate-2-classification.json \\
      --proposed build/data-identity/data_symbols.proposed.csv \\
      --report build/data-identity/report.json

Derivation sources, strongest first, for an original target address T:

- ``csv``: T is already a data_symbols.csv base (the gap is rebuilt-side).
- ``nss_symtab``: a named STT_OBJECT/FUNC/TLS NSS symbol whose original extent
  covers T (base, or strictly inside st_size).  Mapping symbols ($d/$x) never
  count.  Names that several NSS symbols carry are qualified ``name@file``
  with the owning translation unit from DWARF or the preceding STT_FILE.
- ``dwarf_variable``: a DW_TAG_variable whose DW_AT_location is DW_OP_addr T
  (optionally plus DW_OP_plus_uconst), covering file/function statics,
  template statics and globals folded into _MergedGlobals.
- ``function``: T is a functions.csv entry (a function-pointer GOT slot).
- ``anonymous_readonly`` / ``anonymous_writable``: no identity; the former is
  the content-anchored candidate described in docs/data_identity.md.

A proposal is emitted only for a single unambiguous identity.  Aliases with
distinct names at one extent, or overlapping candidate extents, are reported
as ``ambiguous`` and proposed nowhere.  Everything here is evidence for review,
not a verification result.
"""
from __future__ import annotations

import argparse
import bisect
import collections
import csv
import gzip
import hashlib
import json
import re
import shutil
import subprocess
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from data_elf import inspect_elf  # noqa: E402

ROOT = Path(__file__).resolve().parent.parent
BASE = 0x7100000000
STT_OBJECT, STT_FUNC, STT_FILE, STT_TLS = 1, 2, 4, 6
R_GLOB_DAT, R_RELATIVE = 1025, 1027
MAPPING = re.compile(r'^\$[adxt](\.|$)')

GOT_UNKNOWN = 'unverified GOT reference: original GOT target lacks known data identity'
GOT_UNDEFINED = 'unverified GOT reference: rebuilt undefined GOT target symbol'


# ------------------------------------------------------------------ decoding

def decode_adrp(insn, pc):
    """(Rd, page) for ADRP, else None."""
    if insn & 0x9F000000 != 0x90000000:
        return None
    imm = ((insn >> 5) & 0x7FFFF) << 2 | (insn >> 29) & 3
    if imm & (1 << 20):
        imm -= 1 << 21
    return insn & 31, (pc & ~0xFFF) + (imm << 12)


def decode_ldr_uimm(insn):
    """(Rt, Rn, byte offset, width) for LDR (unsigned immediate, GPR), else None."""
    if insn & 0xBFC00000 == 0xB9400000:      # 32/64-bit LDR
        width = 8 if insn & 0x40000000 else 4
        return insn & 31, (insn >> 5) & 31, ((insn >> 10) & 0xFFF) * width, width
    return None


def decode_add_imm(insn):
    """(Rd, Rn, imm) for 64-bit ADD (immediate, no shift), else None."""
    if insn & 0xFFC00000 != 0x91000000:
        return None
    return insn & 31, (insn >> 5) & 31, (insn >> 10) & 0xFFF


# ------------------------------------------------------------------ images

class Original:
    """The checker's original ELF (main.elf): bytes by vaddr and GOT targets."""

    def __init__(self, path):
        self.path = Path(path)
        self.meta = inspect_elf(self.path)
        self.blob = self.path.read_bytes()
        if hashlib.sha256(self.blob).hexdigest() != self.meta['sha256']:
            raise ValueError('original ELF changed while loading')
        self.sections = [s for s in self.meta['sections'] if s['flags'] & 2 and s['size']]
        self.segments = [s for s in self.meta['segments'] if s['type'] == 1]
        got = [s for s in self.sections if s['name'] in ('.got', '.got.plt')]
        dyn = {s['index']: s for s in self.meta['sections']}
        dynsyms = {}
        for symbol in self.meta['symbols']:
            dynsyms[(symbol['table_section'], symbol['index'])] = symbol
        self.relocated = sorted(r['offset'] for r in self.meta['relocations'])
        self.got = {}
        for r in self.meta['relocations']:
            if not any(s['address'] <= r['offset'] < s['address'] + s['size'] for s in got):
                continue
            symbol = dynsyms.get((r['symbol_table'], r['symbol_index']))
            if r['type'] == R_RELATIVE:
                entry = dict(kind='relative', target=r['addend'])
            elif r['type'] == R_GLOB_DAT and symbol and symbol['defined']:
                entry = dict(kind='glob_dat', target=symbol['address'] + (r['addend'] or 0),
                             symbol=symbol['name'])
            else:
                entry = dict(kind='import' if symbol and not symbol['defined'] else 'unsupported',
                             target=None, symbol=symbol and symbol['name'],
                             type=r['type'], section=dyn[r['section_index']]['name'])
            if r['offset'] in self.got:
                entry = dict(kind='duplicate', target=None)
            self.got[r['offset']] = entry

    def section_of(self, address):
        hits = [s for s in self.sections if s['address'] <= address < s['address'] + s['size']]
        return hits[0] if len(hits) == 1 else None

    def read(self, address, size):
        for seg in self.segments:
            if seg['address'] <= address and address + size <= seg['address'] + seg['file_size']:
                off = seg['offset'] + address - seg['address']
                return self.blob[off:off + size]
        return None

    def word(self, address):
        raw = self.read(address, 4)
        return None if raw is None else int.from_bytes(raw, 'little')

    def relocated_in(self, address, size):
        """Whether any relocation record overlaps [address, address+size)."""
        i = bisect.bisect_left(self.relocated, address - 7)
        return i < len(self.relocated) and self.relocated[i] < address + size


def got_slot_at(original, function, insn):
    """Resolve the GOT slot that the GOT load at/near ``insn`` reads.

    ``insn`` is the checker-reported mismatch address (no 0x71 base).  An LDR
    pairs with the nearest preceding ADRP of its base register inside the
    function; an ADRP pairs with the nearest following LDR from it.  Returns
    (slot, detail) or (None, reason).  This is a linear scan, so it reproduces
    the checker's flow-sensitive pairing only for straight-line uses.
    """
    word = original.word(insn)
    if word is None:
        return None, 'unreadable instruction'
    ldr = decode_ldr_uimm(word)
    if ldr and ldr[3] == 8:
        _, rn, off, _ = ldr
        pc = insn - 4
        while pc >= function:
            w = original.word(pc)
            adrp = decode_adrp(w, pc)
            if adrp and adrp[0] == rn:
                return adrp[1] + off, dict(adrp=pc, ldr=insn)
            pc -= 4
        return None, 'no preceding ADRP for GOT load'
    adrp = decode_adrp(word, insn)
    if adrp:
        pc = insn + 4
        for _ in range(64):
            w = original.word(pc)
            if w is None:
                break
            ldr = decode_ldr_uimm(w)
            if ldr and ldr[1] == adrp[0] and ldr[3] == 8:
                return adrp[1] + ldr[2], dict(adrp=insn, ldr=pc)
            pc += 4
        return None, 'no following GOT load for ADRP'
    return None, f'unsupported instruction {word:#010x}'


# ------------------------------------------------------------------ NSS symbols

class NssSymbols:
    """Named, allocated NSS symbols with original extents and owning TU."""

    def __init__(self, path, meta=None):
        meta = meta or inspect_elf(path)
        self.sha256 = meta['sha256']
        sections = {s['index']: s for s in meta['sections']}
        symtabs = [s['index'] for s in meta['sections'] if s['type'] == 2]
        self.symbols = []
        current_file = None
        for symbol in meta['symbols']:
            if symbol['table_section'] not in symtabs:
                continue
            if symbol['type'] == STT_FILE:
                current_file = symbol['name']
                continue
            section = sections.get(symbol['section_index'])
            if (not symbol['name'] or MAPPING.match(symbol['name']) or not symbol['defined']
                    or not section or not section['flags'] & 2
                    or symbol['type'] not in (STT_OBJECT, STT_FUNC, STT_TLS)):
                continue
            self.symbols.append(dict(name=symbol['name'], address=symbol['address'],
                                     size=symbol['size'], type=symbol['type'],
                                     binding=symbol['binding'], section=section['name'],
                                     stt_file=current_file if symbol['binding'] == 0 else None))
        self.symbols.sort(key=lambda s: (s['address'], s['name']))
        self.starts = [s['address'] for s in self.symbols]
        self.name_count = collections.Counter(s['name'] for s in self.symbols)
        self.max_size = max((s['size'] for s in self.symbols), default=0)

    def covering(self, address):
        """Symbols at ``address`` or strictly containing it in their extent."""
        lo = bisect.bisect_left(self.starts, address - self.max_size)
        hi = bisect.bisect_right(self.starts, address)
        return [s for s in self.symbols[lo:hi]
                if s['address'] == address or s['address'] < address < s['address'] + s['size']]


# ------------------------------------------------------------------ DWARF

_ATTR = re.compile(r'^\s*DW_AT_(\w+)\s+\((.*)\)\s*$')
_ADDR = re.compile(r'DW_OP_addr (0x[0-9a-f]+)(?:, DW_OP_plus_uconst (0x[0-9a-f]+))?$')
_QUOTED = re.compile(r'"(.*)"')


def _norm_file(raw):
    if not raw:
        return None
    try:
        from dwarf_index import norm_path
        return norm_path(raw)
    except Exception:  # pragma: no cover - dwarf_index is part of the tree
        return raw.replace('\\', '/')


def parse_dwarf_variables(lines):
    """Variables with a static DW_OP_addr location from llvm-dwarfdump text.

    Each record has address, name, linkage, decl_file, external, and the
    enclosing subprogram's linkage name for function-local statics.
    """
    out, stack, cur = [], [], None

    def flush():
        if cur is None or cur['tag'] != 'variable' or 'location' not in cur['attrs']:
            return
        m = _ADDR.match(cur['attrs']['location'])
        if not m:
            return
        a = cur['attrs']
        spec = a.get('specification')
        name = _unquote(a.get('name')) or (_unquote(spec) if spec else None)
        func = next((s for s in reversed(stack[:-1]) if s['tag'] == 'subprogram'), None)
        unit = None
        if stack and stack[0]['tag'] == 'compile_unit':
            unit = _unquote(stack[0]['attrs'].get('name'))
            comp_dir = _unquote(stack[0]['attrs'].get('comp_dir'))
            if unit and comp_dir and not re.match(r'^([A-Za-z]:)?[\\/]', unit):
                unit = comp_dir.rstrip('\\/') + '/' + unit
        out.append(dict(cu=_norm_file((unit or '').replace('\\\\', '\\')) or None,address=int(m.group(1), 16) + int(m.group(2) or '0', 16),
                        merged_offset=int(m.group(2) or '0', 16),
                        name=name, linkage=_unquote(a.get('linkage_name')),
                        decl_file=_norm_file(_unquote(a.get('decl_file')) or '') or None,
                        decl_line=a.get('decl_line'), external='external' in a,
                        specification=bool(spec),
                        function=func and (_unquote(func['attrs'].get('linkage_name'))
                                           or _unquote(func['attrs'].get('name')))))

    for line in lines:
        if line[:2] == '0x' and ':' in line[:12]:
            colon = line.find(':')
            rest = line[colon + 2:]
            body = rest.lstrip(' ')
            flush()
            cur = None
            if not body.startswith('DW_TAG_'):
                if body.startswith('NULL'):
                    depth = (len(rest) - len(body)) // 2
                    del stack[depth:]
                continue
            depth = (len(rest) - len(body)) // 2
            del stack[depth:]
            cur = dict(tag=body[7:].strip(), attrs={})
            stack.append(cur)
        elif cur is not None:
            m = _ATTR.match(line)
            if m and m.group(1) not in cur['attrs']:
                cur['attrs'][m.group(1)] = m.group(2)
    flush()
    return out


def _unquote(value):
    if value is None:
        return None
    m = _QUOTED.search(value)
    return m.group(1) if m else value


def dwarf_variables(nss, cache_dir):
    """Cached parse of the NSS DWARF variables, keyed by the NSS SHA-256."""
    sha = hashlib.sha256()
    with open(nss, 'rb') as f:
        for chunk in iter(lambda: f.read(1 << 22), b''):
            sha.update(chunk)
    cache = Path(cache_dir) / f'dwarf_vars2-{sha.hexdigest()[:16]}.json.gz'
    if cache.is_file():
        with gzip.open(cache, 'rt') as f:
            return json.load(f)
    exe = shutil.which('llvm-dwarfdump') or '/opt/homebrew/opt/llvm/bin/llvm-dwarfdump'
    proc = subprocess.Popen([exe, '--debug-info', str(nss)], stdout=subprocess.PIPE,
                            stderr=subprocess.DEVNULL, text=True, errors='replace',
                            bufsize=1 << 20)
    records = parse_dwarf_variables(proc.stdout)
    if proc.wait():
        raise subprocess.CalledProcessError(proc.returncode, proc.args)
    records.sort(key=lambda r: (r['address'], r['name'] or '', r['decl_file'] or ''))
    cache.parent.mkdir(parents=True, exist_ok=True)
    with gzip.GzipFile(cache, 'wb', mtime=0) as f:
        f.write(json.dumps(records, sort_keys=True).encode())
    return records


# ------------------------------------------------------------------ inventories

def read_csv_rows(path):
    with open(path, newline='') as f:
        return list(csv.DictReader(f))


def load_inventories(data_dir):
    data_dir = Path(data_dir)
    known = {}
    for row in read_csv_rows(data_dir / 'data_symbols.csv'):
        known.setdefault(int(row['Address'], 16) - BASE, row['Name'])
    functions, quality = {}, {}
    for row in read_csv_rows(data_dir / 'functions.csv'):
        a = int(row['Address'], 16)
        functions[a - BASE] = (row['Name'], int(row['Size']))
        quality[a] = row['Quality']
    files = {int(r['Address'], 16) - BASE: r['File']
             for r in read_csv_rows(data_dir / 'function_files.csv')}
    return known, functions, quality, files


def shortest_suffix(target, others):
    parts = target.split('/')
    for n in range(1, len(parts) + 1):
        cand = '/'.join(parts[-n:])
        if not any(o == cand or o.endswith('/' + cand) for o in others):
            return cand
    return target


# ------------------------------------------------------------------ derivation

class Deriver:
    def __init__(self, original, nss, dwarf, known, functions, files):
        self.original, self.nss, self.known = original, nss, known
        self.functions, self.files = functions, files
        self.known_names = collections.Counter(known.values())
        self.dwarf = collections.defaultdict(list)
        for record in dwarf:
            self.dwarf[record['address']].append(record)
        # DWARF owner for NSS symbols, by (address, linkage/name).
        self.dwarf_by_name = collections.defaultdict(set)
        for record in dwarf:
            for key in (record['linkage'], record['name']):
                if key:
                    self.dwarf_by_name[(record['address'], key)].add(
                        (record['decl_file'], record.get('cu')))
        self.dwarf_names = collections.defaultdict(set)
        for record in dwarf:
            name = record['linkage'] or record['name']
            if name:
                self.dwarf_names[name].add(record['address'])
        self.function_starts = sorted(functions)
        self.dwarf_starts = sorted(self.dwarf)

    def units_at(self, address, name):
        """Distinct compile units DWARF records for ``name`` at ``address``."""
        return sorted({u for _, u in self.dwarf_by_name.get((address, name), ()) if u})

    def owner_file(self, symbol):
        """(declaring file, source) for reports: DWARF decl_file, else the TU."""
        files = {d for d, _ in self.dwarf_by_name.get((symbol['address'], symbol['name']), ()) if d}
        if len(files) == 1:
            return next(iter(files)), 'dwarf_decl_file'
        return self.owner_tu(symbol)

    def owner_tu(self, symbol):
        """(translation unit, source): the qualifier a link map can resolve.

        Header-declared statics share one decl_file across every copy, so the
        qualifier must be the defining compile unit, never the decl_file.
        """
        units = {u for _, u in self.dwarf_by_name.get((symbol['address'], symbol['name']), ()) if u}
        if len(units) == 1:
            return next(iter(units)), 'dwarf_compile_unit'
        if symbol['type'] == STT_FUNC and symbol['address'] in self.files:
            return self.files[symbol['address']], 'function_files'
        if symbol.get('stt_file'):
            return symbol['stt_file'], 'stt_file'
        return None, None

    def qualified(self, symbol):
        """Unique checker name: bare if the NSS has one copy, else name@TU-suffix."""
        if self.nss.name_count[symbol['name']] == 1:
            return symbol['name']
        owner, _ = self.owner_tu(symbol)
        if not owner:
            return None
        others = []
        for s in self.nss.symbols:
            if s['name'] == symbol['name'] and s is not symbol:
                o, _ = self.owner_tu(s)
                if o is None or o == owner:
                    return None  # an unowned or same-TU copy cannot be told apart
                others.append(o)
        return f"{symbol['name']}@{shortest_suffix(owner, others)}"

    def derive(self, target):
        """Classify one original target address (no 0x71 base)."""
        section = self.original.section_of(target)
        sname = section['name'] if section else None
        writable = bool(section and section['flags'] & 1)
        out = dict(target=target, section=sname)
        if target in self.known:
            name = self.known[target]
            out.update(source='csv', name=name, proposal=None)
            same = [s for s in self.nss.covering(target)
                    if s['name'] == name and s['address'] == target]
            if len(same) == 1:
                owner, src = self.owner_file(same[0])
                tu, _ = self.owner_tu(same[0])
                out.update(owner_file=owner, owner_source=src, owner_tu=tu,
                           size=same[0]['size'], binding=same[0]['binding'])
            elif self.dwarf.get(target):
                r = self.dwarf[target][0]
                out.update(owner_file=r['decl_file'], owner_source='dwarf_decl_file',
                           owner_tu=r.get('cu'))
            return out
        covering = self.nss.covering(target)
        names = {s['name'] for s in covering}
        if target in self.functions and all(s['type'] == STT_FUNC for s in covering):
            # Function-pointer slot: functions.csv already carries the checker's
            # (possibly name@file-qualified) function identity.  Same-extent
            # aliases (D1/D2) are recorded, not resolved here.
            name, size = self.functions[target]
            extents = {(s['address'], s['size']) for s in covering}
            aliases = sorted(names - {name.split('@')[0]})
            out.update(source='function', name=name, owner_file=self.files.get(target),
                       aliases=aliases)
            if len(extents) > 1:
                out.update(source='ambiguous', candidates=sorted(names), proposal=None)
            elif self.known_names.get(name):
                out.update(proposal=None, reason='name already bound to another CSV address')
            else:
                out['proposal'] = dict(address=target, name=name, evidence=(
                    f"function:functions_csv:size={size}"
                    + (f":same_extent_aliases={len(aliases)}" if aliases else '')))
            return out
        if covering:
            extents = {(s['address'], s['size']) for s in covering}
            if len(names) > 1 or len(extents) > 1:
                out.update(source='ambiguous', candidates=sorted(names), proposal=None)
                return out
            symbol = covering[0]
            owner, owner_src = self.owner_file(symbol)
            offset = target - symbol['address']
            out.update(source='nss_symtab', symbol=symbol['name'], symbol_address=symbol['address'],
                       offset=offset, size=symbol['size'], owner_file=owner,
                       owner_source=owner_src, binding=symbol['binding'], type=symbol['type'])
            if symbol['address'] in self.known and self.known[symbol['address']] == symbol['name']:
                # Base already known: the gap is an interior offset (F4 range evidence).
                out.update(source='csv_interior', name=symbol['name'], proposal=None)
                return out
            tu, tu_src = self.owner_tu(symbol)
            out.update(owner_tu=tu, owner_tu_source=tu_src)
            name = self.qualified(symbol)
            if name is None:
                out.update(proposal=None, reason='duplicate NSS name without distinct owning TU')
                return out
            if self.known_names.get(name):
                out.update(proposal=None, reason='name already bound to another CSV address')
                return out
            out.update(name=name, proposal=dict(
                address=symbol['address'], name=name,
                evidence=(f"nss_symtab:{'local' if symbol['binding'] == 0 else 'global'}"
                          f":type={symbol['type']}:size={symbol['size']}"
                          f":target_offset={offset}"
                          + (f":qualifier={tu_src}" if '@' in name else ''))))
            return out
        records = self.dwarf.get(target, [])
        if records:
            idents = {(r['linkage'] or r['name'], r['decl_file']) for r in records}
            if len({i[0] for i in idents}) != 1:
                out.update(source='ambiguous', candidates=sorted(str(i) for i in idents), proposal=None)
                return out
            r = records[0]
            name = r['linkage'] or r['name']
            if not r['linkage'] and not r['external']:
                # A static without a linkage name has no rebuilt-checkable symbol name.
                out.update(source='dwarf_variable', name=None, owner_file=r['decl_file'],
                           dwarf_name=r['name'], function=r['function'], proposal=None,
                           reason='internal DWARF variable without linkage name')
                return out
            out.update(source='dwarf_variable', name=name, owner_file=r['decl_file'],
                       owner_tu=r.get('cu'), merged_offset=r['merged_offset'],
                       function=r['function'])
            copies = self.dwarf_names.get(name, set()) | {
                s['address'] for s in self.nss.symbols if s['name'] == name}
            if copies - {target}:
                others = [u for a in copies - {target} for u in self.units_at(a, name)]
                unit = r.get('cu')
                if not unit or unit in others or len(others) < len(copies - {target}):
                    out.update(proposal=None, reason='duplicate DWARF name without distinct owning TU')
                    return out
                name = f'{name}@{shortest_suffix(unit, others)}'
                out['name'] = name
            if self.known_names.get(name):
                out.update(proposal=None, reason='name already bound to another CSV address')
                return out
            out['proposal'] = dict(address=target, name=name, evidence=(
                f"dwarf_variable:DW_OP_addr{'+merged' if r['merged_offset'] else ''}"
                f":{'external' if r['external'] else 'internal'}"
                + (':function_static' if r['function'] else '')))
            return out
        if section and section['flags'] & 4:
            i = bisect.bisect_right(self.function_starts, target) - 1
            if i >= 0:
                fa = self.function_starts[i]
                fname, fsize = self.functions[fa]
                if target < fa + fsize:
                    out.update(source='function_interior', name=fname, offset=target - fa,
                               proposal=None)
                    return out
        out.update(source='anonymous_writable' if writable else 'anonymous_readonly',
                   proposal=None)
        i = bisect.bisect_right(self.dwarf_starts, target) - 1
        if i >= 0 and section and section['address'] <= self.dwarf_starts[i]:
            # Lead only: DWARF variable sizes are not modelled, so this is never
            # interior-offset evidence.
            r = self.dwarf[self.dwarf_starts[i]][0]
            out['preceding_dwarf_variable'] = dict(name=r['linkage'] or r['name'],
                                                   distance=target - self.dwarf_starts[i])
        if not writable and section:
            out.update(self.content_shape(target))
        return out

    def content_shape(self, target):
        """Original-side shape for content-anchored eligibility (bounded)."""
        raw = self.original.read(target, 4096) or b''
        end = raw.find(b'\0')
        if end < 0:
            return dict(shape='unterminated')
        text = raw[:end]
        relocated = self.original.relocated_in(target, end + 1)
        if relocated:
            return dict(shape='relocated_bytes', length=end + 1)
        if end == 0:
            return dict(shape='nul', length=1)
        if all(32 <= b < 127 or b in (9, 10, 13) for b in text) or _utf8(text):
            before = self.original.read(target - 1, 1)
            return dict(shape='c_string', length=end + 1,
                        sha256=hashlib.sha256(raw[:end + 1]).hexdigest(),
                        # a non-NUL predecessor means T may be a merged string tail
                        tail_position=bool(before and before != b'\0'))
        return dict(shape='binary', length=end + 1)


def _utf8(text):
    try:
        decoded = text.decode('utf-8')
    except UnicodeDecodeError:
        return False
    return all(c.isprintable() or c in '\t\r\n' for c in decoded)


# ------------------------------------------------------------------ checker rows

def checker_rows(result_path, quality, original, functions):
    """Proof-gap rows of one checker result, with GOT slot evidence."""
    rows = json.loads(Path(result_path).read_text())['results']
    out = []
    for r in rows:
        reason = r.get('reason', '')
        m = re.match(r'mismatch at (0x[0-9a-f]+): (.*)', reason, re.S)
        if r['status'] == 'ok' or not m:
            continue
        address = int(r['address'], 16)
        insn, detail = int(m.group(1), 16), m.group(2)
        kind = ('got_unknown' if detail.startswith(GOT_UNKNOWN) else
                'got_undefined' if detail.startswith(GOT_UNDEFINED) else
                'adrp_indirect' if detail.startswith('unproven ADRP pair') else
                'jump_table' if detail.startswith('unproven jump table') else 'other')
        row = dict(function=address, name=r['name'], quality=quality.get(address), kind=kind,
                   insn=insn)
        if kind.startswith('got_'):
            fsize = functions.get(address - BASE, (None, 0))[1]
            slot, info = got_slot_at(original, address - BASE, insn)
            row['slot'] = slot
            if slot is None:
                row['slot_error'] = info
            else:
                entry = original.got.get(slot)
                row['got'] = entry or dict(kind='not_a_got_slot', target=None)
        out.append(row)
    return out


# ------------------------------------------------------------------ main

def summarize(rows, derived, key='target'):
    by_source = collections.Counter()
    by_section = collections.Counter()
    by_pair = collections.Counter()
    for row in rows:
        d = derived.get(row.get(key))
        src = d['source'] if d else 'no_target'
        sec = d['section'] if d else None
        by_source[src] += 1
        by_section[str(sec)] += 1
        by_pair[f'{src}|{sec}'] += 1
    return dict(rows=len(rows), by_source=dict(sorted(by_source.items())),
                by_section=dict(sorted(by_section.items())),
                by_source_section=dict(sorted(by_pair.items())))


def build(args):
    original = Original(args.elf)
    nss_meta = inspect_elf(args.nss)
    nss = NssSymbols(args.nss, nss_meta)
    dwarf = dwarf_variables(args.nss, args.cache) if not args.no_dwarf else []
    known, functions, quality, files = load_inventories(args.data)
    deriver = Deriver(original, nss, dwarf, known, functions, files)

    report = dict(format=1, inputs={}, modes={})
    for path in [args.elf, args.data / 'data_symbols.csv', args.data / 'functions.csv',
                 args.data / 'function_files.csv', *args.result, *([args.f3] if args.f3 else [])]:
        report['inputs'][str(path)] = hashlib.sha256(Path(path).read_bytes()).hexdigest()
    report['inputs']['nss'] = nss.sha256

    derived = {}
    all_got = {}
    for path in args.result:
        mode = Path(path).parts[-4] if len(Path(path).parts) >= 4 else str(path)
        rows = checker_rows(path, quality, original, functions)
        o_rows = [r for r in rows if r['quality'] == 'O']
        for r in o_rows:
            if r['kind'].startswith('got_') and r.get('got', {}).get('target') is not None:
                r['target'] = r['got']['target']
                derived.setdefault(r['target'], deriver.derive(r['target']))
        got_unknown = [r for r in o_rows if r['kind'] == 'got_unknown']
        got_undef = [r for r in o_rows if r['kind'] == 'got_undefined']
        mode_report = dict(
            o_rows_by_kind=dict(collections.Counter(r['kind'] for r in o_rows)),
            got_unknown=summarize(got_unknown, derived),
            got_unknown_distinct=summarize([dict(target=t) for t in
                                            sorted({r.get('target') for r in got_unknown} - {None})],
                                           derived),
            got_unknown_slot_errors=collections.Counter(
                r.get('slot_error') or r.get('got', {}).get('kind') for r in got_unknown
                if r.get('target') is None),
            got_undefined=summarize(got_undef, derived),
            got_undefined_owners=undefined_owners(got_undef, derived, files),
        )
        report['modes'][mode] = mode_report
        all_got[mode] = rows

    if args.f3:
        report['f3'] = f3_report(args.f3, deriver, derived)

    proposals = {}
    for d in derived.values():
        p = d.get('proposal')
        if p:
            proposals.setdefault((p['address'], p['name']), p)
    names = collections.Counter(name for _, name in proposals)
    rows_out = sorted((a, n, p['evidence']) for (a, n), p in proposals.items() if names[n] == 1)
    report['proposed_rows'] = len(rows_out)
    report['proposed_by_evidence'] = dict(collections.Counter(
        e.split(':')[0] + ':' + e.split(':')[1] for _, _, e in rows_out))
    report['targets'] = {f'{t:#x}': d for t, d in sorted(derived.items())}
    if args.proposed:
        args.proposed.parent.mkdir(parents=True, exist_ok=True)
        with open(args.proposed, 'w', newline='') as f:
            w = csv.writer(f, lineterminator='\n')
            w.writerow(['Address', 'Name', 'Evidence'])
            for a, n, e in rows_out:
                w.writerow([f'0x{BASE | a:016x}', n, e])
    if args.rows:
        args.rows.parent.mkdir(parents=True, exist_ok=True)
        args.rows.write_text(json.dumps(all_got, indent=1, sort_keys=True, default=str))
    return report


def undefined_owners(rows, derived, files):
    """Rebuilt-undefined GOT targets grouped by original owner file, ranked.

    The owner is the TU whose DWARF definition carries the location (or the
    function's function_files.csv entry), else its DWARF decl_file, else the
    single file all referencing functions come from (marked as such).
    """
    targets = collections.defaultdict(set)
    for r in rows:
        targets[r.get('target')].add(r['function'])
    groups = collections.defaultdict(lambda: dict(functions=set(), globals=[]))
    for target, funcs in targets.items():
        d = derived.get(target) or {}
        name = d.get('name') or d.get('symbol') or (
            f"<{d['source']}@{target:#x}>" if d else '<unresolved slot>')
        owner, basis = d.get('owner_tu'), 'defining_unit'
        if d.get('source') == 'function':
            owner, basis = d.get('owner_file'), 'function_files'
        if not owner and d.get('owner_file'):
            owner, basis = d['owner_file'], 'decl_file'
        if not owner:
            users = {files.get(f - BASE) for f in funcs}
            owner, basis = ((f'{next(iter(users))} (referencing functions)', 'referencing_functions')
                            if len(users) == 1 and None not in users else ('<unknown>', None))
        g = groups[owner]
        g['functions'] |= funcs
        g['globals'].append(dict(name=name, address=f'{target:#x}' if target is not None else None,
                                 o_functions=len(funcs), section=d.get('section'),
                                 decl_file=d.get('owner_file'), basis=basis))
    ranked = sorted(groups.items(), key=lambda kv: (-len(kv[1]['functions']), kv[0]))
    src = ROOT / 'src'
    return [dict(owner_file=owner, o_functions=len(g['functions']),
                 owner_in_src=(src / owner).is_file() if owner and not owner.startswith('<')
                 and ' ' not in owner else None,
                 globals=sorted(g['globals'], key=lambda x: (-x['o_functions'], x['name'])))
            for owner, g in ranked]


def f3_report(path, deriver, derived):
    data = json.loads(Path(path).read_text())
    out = {}
    for mode, m in data['modes'].items():
        rows = m['all_new']
        for r in rows:
            t = int(r['original_target'], 16)
            r['target'] = t
            derived.setdefault(t, deriver.derive(t))
        summary = summarize(rows, derived)
        content = collections.Counter()
        for r in rows:
            d = derived[r['target']]
            if d['source'] == 'anonymous_readonly':
                content[d.get('shape', 'unknown')] += 1
        summary['anonymous_readonly_shape'] = dict(sorted(content.items()))
        summary['distinct'] = summarize([dict(target=t) for t in sorted({r['target'] for r in rows})],
                                        derived)
        out[mode] = summary
    return out


def main(argv=None):
    p = argparse.ArgumentParser(description=__doc__.split('\n\n')[0])
    p.add_argument('--elf', type=Path, default=ROOT / 'data/2.6.2/main.elf')
    p.add_argument('--nss', type=Path, default=ROOT / 'build/romfs/DiabloIIINX64r.nss')
    p.add_argument('--data', type=Path, default=ROOT / 'data/2.6.2')
    p.add_argument('--result', type=Path, action='append', default=[],
                   help='checker batch result.json (repeatable, e.g. normal and lua)')
    p.add_argument('--f3', type=Path, help='retained F3 candidate classification JSON')
    p.add_argument('--cache', type=Path, default=ROOT / 'build/data-identity',
                   help='directory for the DWARF variable cache (never a link into the main checkout)')
    p.add_argument('--no-dwarf', action='store_true')
    p.add_argument('--proposed', type=Path, help='write the proposed identity CSV here')
    p.add_argument('--rows', type=Path, help='write per-row evidence JSON here')
    p.add_argument('--report', type=Path, help='write the JSON report here (default stdout)')
    args = p.parse_args(argv)
    report = build(args)
    text = json.dumps(report, indent=1, sort_keys=True, default=str)
    if args.report:
        args.report.parent.mkdir(parents=True, exist_ok=True)
        args.report.write_text(text + '\n')
    else:
        print(text)


if __name__ == '__main__':
    main()
