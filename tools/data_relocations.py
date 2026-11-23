#!/usr/bin/env python3
"""Conservative comparison of data bytes and explicit AArch64 RELA expressions.

Only relocation records authorize ignoring pointer bytes. A successful comparison
is link-time evidence, not a claim about dynamic interposition or runtime state.
Addresses supplied by callers include the explicitly configured load bias.
"""
from __future__ import annotations

from bisect import bisect_left, bisect_right
import hashlib
from pathlib import Path

from data_elf import inspect_elf

SUPPORTED = {257: 'ABS64', 1025: 'GLOB_DAT', 1026: 'JUMP_SLOT', 1027: 'RELATIVE'}


class RelocationImage:
    """An immutable file snapshot plus checked ELF metadata and identity indexes."""

    def __init__(self, path, label='image', load_bias=0):
        self.path, self.label, self.bias = Path(path), label, load_bias
        self.metadata = inspect_elf(path)
        self.blob = self.path.read_bytes()
        if hashlib.sha256(self.blob).hexdigest() != self.metadata['sha256']:
            raise ValueError('ELF changed while loading relocation evidence')
        self.sections = {s['index']: s for s in self.metadata['sections']}
        self.symbols = []
        self.tables = {}
        self.names = {}
        for symbol in self.metadata['symbols']:
            self.tables[(symbol['table_section'], symbol['index'])] = symbol
            section = self.sections.get(symbol['section_index'])
            if (not symbol['name'] or not section or not section['flags'] & 2
                    or symbol['type'] not in (1, 2) or not symbol['defined']):
                continue
            if not (section['address'] <= symbol['address']
                    and symbol['address'] + symbol['size'] <= section['address'] + section['size']):
                continue
            self.symbols.append(symbol)
            key = self._key(symbol)
            self.names.setdefault(key, set()).add(self._extent(symbol))
        self._index_symbols()
        self.relocations = sorted(self.metadata['relocations'], key=lambda r: r['offset'])
        self.relocation_offsets = [r['offset'] for r in self.relocations]
        self.unknown_relocations = [r for r in self.relocations if r['type'] not in SUPPORTED]
        self.unmodeled_relocations = any(s['type'] == 19 for s in self.sections.values()) or any(
            d['tag'] in (35, 36, 37) for d in self.metadata['dynamic_tags'])
        self.versioned_symbols = any(s['type'] in (0x6ffffffd, 0x6ffffffe, 0x6fffffff)
                                     for s in self.sections.values())

    def _index_symbols(self):
        self.symbols.sort(key=lambda s: s['address'])
        self.symbol_starts = [s['address'] for s in self.symbols]
        self.symbol_ends = []
        end = -1
        for symbol in self.symbols:
            end = max(end, symbol['address'] + max(1, symbol['size']))
            self.symbol_ends.append(end)

    def add_symbol_evidence(self, other):
        """Supplement stripped metadata only with same-address, identical bytes.

        The source must share the address space and have allocated file-backed
        extents. Its relocation records are never imported. This does not confer
        identity on local symbols or infer unknown symbol sizes.
        """
        self.versioned_symbols = self.versioned_symbols or other.versioned_symbols
        added = 0
        existing = {(self._key(s), self._extent(s)) for s in self.symbols}
        for symbol in other.symbols:
            if symbol['binding'] == 0 or symbol['size'] <= 0:
                continue
            address = symbol['address'] + other.bias
            body, storage = self.object_bytes(address, symbol['size'])
            corroborating, other_storage = other.object_bytes(address, symbol['size'])
            if body is None or body != corroborating or storage != 'file' or storage != other_storage:
                continue
            raw = address - self.bias
            sections = [s for s in self.sections.values() if s['flags'] & 2
                        and s['address'] <= raw and raw + symbol['size'] <= s['address'] + s['size']]
            if len(sections) != 1:
                continue
            imported = dict(symbol, address=raw, section_index=sections[0]['index'],
                            evidence_source=other.label)
            identity = (self._key(imported), self._extent(imported))
            if identity in existing:
                continue
            existing.add(identity)
            self.symbols.append(imported)
            self.names.setdefault(self._key(imported), set()).add(self._extent(imported))
            added += 1
        self._index_symbols()
        return added

    @staticmethod
    def _key(symbol):
        return (symbol['name'], symbol['type'], symbol['binding'], symbol['visibility'])

    @staticmethod
    def _extent(symbol):
        return (symbol['address'], symbol['size'], symbol['section_index'], symbol['type'])

    def target_identity(self, address):
        """Resolve an explicit target, retaining aliases and strict interior offset.

        Same-name symbols at different locations, overlapping extents, zero-sized
        interior targets and nameless/section targets cannot establish identity.
        """
        if self.versioned_symbols:
            return dict(resolved=False, reason='unmodeled_symbol_versions')
        raw = address - self.bias
        lo = bisect_right(self.symbol_ends, raw)
        hi = bisect_right(self.symbol_starts, raw)
        candidates = [s for s in self.symbols[lo:hi] if s['address'] == raw
                      or s['address'] < raw < s['address'] + s['size']]
        extents = {self._extent(s) for s in candidates}
        if len(extents) != 1:
            return dict(resolved=False, reason='missing_target_identity' if not extents else 'ambiguous_target_extent')
        if any(len(self.names[self._key(s)]) != 1 for s in candidates):
            return dict(resolved=False, reason='ambiguous_target_name')
        aliases = sorted({self._key(s) for s in candidates if s['binding'] in (1, 2)})
        if not aliases:
            return dict(resolved=False, reason='local_target_without_owner_identity')
        return dict(resolved=True, aliases=[list(k) for k in aliases],
                    offset=raw - candidates[0]['address'], size=candidates[0]['size'])

    def object_bytes(self, address, size):
        raw = address - self.bias
        sections = [s for s in self.sections.values() if s['flags'] & 2
                    and s['address'] <= raw and raw + size <= s['address'] + s['size']]
        if len(sections) != 1 or size <= 0:
            return None, None
        section = sections[0]
        storage = 'nobits' if section['type'] == 8 else 'file'
        if storage == 'nobits':
            return bytes(size), storage
        offset = section['offset'] + raw - section['address']
        return self.blob[offset:offset + size], storage

    def object_relocations(self, address, size):
        start = address - self.bias
        # Supported slots are eight bytes. Unknown widths cannot be guessed;
        # include starts in the preceding eight bytes to fail closed at edges.
        lo = bisect_left(self.relocation_offsets, start - 7)
        hi = bisect_left(self.relocation_offsets, start + size)
        selected = self.relocations[lo:hi]
        # Unknown writes have unknown widths. Conservatively include earlier
        # records in the containing section; never assume an eight-byte width.
        containers = [s for s in self.sections.values() if s['flags'] & 2
                      and s['address'] <= start < s['address'] + s['size']]
        selected.extend(r for r in self.unknown_relocations if r['offset'] < start - 7
                        and any(s['address'] <= r['offset'] for s in containers))
        return sorted(selected, key=lambda r: r['offset'])

    def expression(self, relocation):
        typ, addend = relocation['type'], relocation['addend']
        evidence = dict(type=typ, kind=SUPPORTED.get(typ), addend=addend,
                        symbol=relocation['symbol_name'])
        if typ not in SUPPORTED or addend is None:
            return dict(evidence, resolved=False, reason='unsupported_relocation')
        if typ == 1027:
            if relocation['symbol_index'] != 0:
                return dict(evidence, resolved=False, reason='invalid_relative_symbol')
            identity = self.target_identity(self.bias + addend)
            return dict(evidence, resolved=identity['resolved'], target=identity,
                        reason=identity.get('reason'))
        if self.versioned_symbols:
            return dict(evidence, resolved=False, reason='unmodeled_symbol_versions')
        symbol = self.tables[(relocation['symbol_table'], relocation['symbol_index'])]
        if symbol not in self.symbols or symbol['binding'] not in (1, 2):
            return dict(evidence, resolved=False, reason='unsupported_symbol_target')
        # Keep S and A separately: equal numeric destinations alone do not prove
        # equivalent symbolic expressions, especially under interposition.
        identity = self.target_identity(self.bias + symbol['address'])
        if addend < 0 or (addend and addend >= symbol['size']):
            return dict(evidence, resolved=False, reason='symbol_addend_outside_extent')
        if identity['resolved']:
            identity = dict(identity, aliases=[list(self._key(symbol))])
        return dict(evidence, resolved=identity['resolved'], target=identity,
                    reason=identity.get('reason'))


def _same_expression(left, right):
    if left['type'] != right['type']:
        return False
    a, b = left['target'], right['target']
    if a['offset'] != b['offset'] or a['size'] != b['size']:
        return False
    if left['type'] != 1027 and left['addend'] != right['addend']:
        return False
    return bool({tuple(k) for k in a['aliases']} & {tuple(k) for k in b['aliases']})


def compare_objects(left, right, left_address, right_address, size, right_size=None):
    """Compare equal extents, ignoring only proven equivalent relocation slots.

    ``unverified`` never establishes equality, even when ``raw_equal`` is true.
    Evidence includes expressions and reasons without leaking initializer bytes.
    """
    result = dict(status='unverified', raw_equal=None, non_relocated_equal=None,
                  relocations=[], reasons=[], masked_bytes=0, storage_verified=False)
    right_size = size if right_size is None else right_size
    if size <= 0 or right_size <= 0:
        result['reasons'].append('unknown_or_empty_extent')
        return result
    if size != right_size:
        result['status'] = 'different'
        result['reasons'].append('size_mismatch')
        return result
    a, sa = left.object_bytes(left_address, size)
    b, sb = right.object_bytes(right_address, size)
    if sa != sb:
        result['status'] = 'different'
        result['reasons'].append('storage_mismatch')
        return result
    if a is None or b is None:
        result['reasons'].append('file_bytes_unavailable')
        return result
    result['raw_equal'] = a == b
    result['storage'] = sa
    result['storage_verified'] = True
    for image, address in ((left, left_address), (right, right_address)):
        if image.unmodeled_relocations:
            result['reasons'].append('unmodeled_packed_relocations')
    slots = []
    for image, address in ((left, left_address), (right, right_address)):
        found = {}
        previous_end = -1
        for relocation in image.object_relocations(address, size):
            offset = relocation['offset'] + image.bias - address
            if offset < 0 or offset + 8 > size:
                result['reasons'].append('boundary_relocation')
            if offset < previous_end:
                result['reasons'].append('overlapping_relocations')
            previous_end = offset + 8
            expression = image.expression(relocation)
            if not expression['resolved']:
                result['reasons'].append(expression['reason'])
            found.setdefault(offset, []).append(expression)
        slots.append(found)
    masked = bytearray(size)
    mismatch = False
    for offset in sorted(set(slots[0]) | set(slots[1])):
        aa, bb = slots[0].get(offset, []), slots[1].get(offset, [])
        valid = len(aa) == len(bb) == 1 and aa[0]['resolved'] and bb[0]['resolved']
        equal = bool(valid and _same_expression(aa[0], bb[0]))
        result['relocations'].append(dict(offset=offset, left=aa, right=bb,
                                          status='equivalent' if equal else 'unverified' if not valid else 'different'))
        if not aa or not bb:
            result['reasons'].append('relocation_layout_mismatch')
        if valid and not equal:
            mismatch = True
        if equal and 0 <= offset <= size - 8:
            masked[offset:offset + 8] = b'\x01' * 8
    # Structural/identity uncertainty means no byte is authorized for masking.
    if result['reasons']:
        result['reasons'] = sorted(set(result['reasons']))
        return result
    result['masked_bytes'] = sum(masked)
    result['non_relocated_equal'] = all(x == y for i, (x, y) in enumerate(zip(a, b)) if not masked[i])
    if not result['non_relocated_equal']:
        result['reasons'].append('non_relocated_bytes_differ')
    if mismatch:
        result['reasons'].append('relocation_target_mismatch')
    result['status'] = 'verified_equal' if result['non_relocated_equal'] and not mismatch else 'different'
    return result
