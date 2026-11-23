#!/usr/bin/env python3
"""Read-only ELF64/AArch64 attribution evidence; never infers source ownership.

The original Switch images have image-relative virtual addresses. ``load_bias``
explicitly translates those into the inventory address space. Relocation targets
are link-time expressions, not proof of runtime resolution or of a call edge.
Only absolute-pointer RELA forms with known semantics are resolved. No binary
bytes are emitted and no optional Python dependencies are required.
"""
from bisect import bisect_left
from collections import Counter
import hashlib
from pathlib import Path
import struct

LOAD_BIAS = 0x7100000000


class BinaryImage:
    def __init__(self, path, label, load_bias):
        self.path, self.label, self.bias = Path(path), label, load_bias
        self.blob = self.path.read_bytes()
        if self.blob[:7] != b'\x7fELF\x02\x01\x01':
            raise ValueError(f'{path}: expected ELF64 little-endian version 1')
        header = self.unpack('<HHIQQQIHHHHHH', 16)
        kind, machine = header[:2]
        if machine != 183 or kind not in (2, 3):
            raise ValueError(f'{path}: expected AArch64 ET_EXEC or ET_DYN')
        self.kind = 'ET_EXEC' if kind == 2 else 'ET_DYN'
        shoff, shentsize, shnum, names_index = header[5], header[10], header[11], header[12]
        if not shoff or shentsize != 64 or not shnum or names_index >= shnum:
            raise ValueError(f'{path}: missing/unsupported section table')
        raw = [self.unpack('<IIQQQQIIQQ', shoff + i * shentsize) for i in range(shnum)]
        names = self.slice(raw[names_index][4], raw[names_index][5])
        self.sections = []
        for i, s in enumerate(raw):
            name = self.string(names, s[0])
            section = dict(index=i, name=name, type=s[1], flags=s[2], raw_address=s[3],
                           address=s[3] + load_bias, offset=s[4], size=s[5], link=s[6],
                           info=s[7], entry_size=s[9])
            if s[1] != 8:
                self.slice(s[4], s[5])
            self.sections.append(section)
        self.symbols, self.tables = [], {}
        for s in self.sections:
            if s['type'] not in (2, 11):
                continue
            if s['entry_size'] != 24 or s['size'] % 24:
                raise ValueError(f'{path}: invalid symbol entry size')
            strings = self.section_data(s['link'])
            table = []
            for offset in range(s['offset'], s['offset'] + s['size'], 24):
                name, info, other, section_index, value, size = self.unpack('<IBBHQQ', offset)
                defined = (0 < section_index < len(self.sections) and bool(self.sections[section_index]['flags'] & 2)) or section_index == 0xfff1
                # SHN_ABS is already absolute; undefined values are not addresses.
                address = value + (0 if section_index == 0xfff1 else load_bias) if defined else None
                symbol = dict(image=label, table=s['name'], name=self.string(strings, name),
                              address=address, raw_address=value, size=size, type=info & 15,
                              bind=info >> 4, visibility=other & 3, section_index=section_index,
                              section=self.sections[section_index]['name'] if 0 < section_index < len(self.sections) else None)
                table.append(symbol)
                if defined:
                    self.symbols.append(symbol)
            self.tables[s['index']] = table
        self.relocations = []
        for s in self.sections:
            if s['type'] not in (4, 9):
                continue
            entry_size = 24 if s['type'] == 4 else 16
            if s['entry_size'] != entry_size or s['size'] % entry_size:
                raise ValueError(f'{path}: invalid relocation entry size')
            table = self.tables.get(s['link'])
            if table is None:
                raise ValueError(f'{path}: relocation references invalid symbol table')
            for offset in range(s['offset'], s['offset'] + s['size'], entry_size):
                values = self.unpack('<QQq' if entry_size == 24 else '<QQ', offset)
                location, info = values[:2]
                addend = values[2] if entry_size == 24 else None
                symbol_index, relocation_type = info >> 32, info & 0xffffffff
                if symbol_index >= len(table):
                    raise ValueError(f'{path}: relocation symbol index out of bounds')
                symbol = table[symbol_index]
                target, expression = None, None
                if addend is not None and relocation_type == 1027 and symbol_index == 0:
                    target, expression = load_bias + addend, 'load_bias + addend'
                elif addend is not None and relocation_type in (257, 1025, 1026) and symbol['address'] is not None:
                    target, expression = symbol['address'] + addend, 'defined_symbol + addend'
                self.relocations.append(dict(image=label, section=s['name'], address=location + load_bias,
                    raw_address=location, type=relocation_type, symbol=symbol['name'],
                    symbol_address=symbol['address'], addend=addend, target=target, expression=expression))

    def slice(self, offset, size):
        if offset < 0 or size < 0 or offset + size > len(self.blob):
            raise ValueError(f'{self.path}: truncated or invalid ELF bounds')
        return self.blob[offset:offset + size]

    def unpack(self, fmt, offset):
        return struct.unpack(fmt, self.slice(offset, struct.calcsize(fmt)))

    @staticmethod
    def string(data, offset):
        if offset >= len(data):
            raise ValueError('ELF string offset out of bounds')
        end = data.find(b'\0', offset)
        if end < 0:
            raise ValueError('unterminated ELF string')
        return data[offset:end].decode('utf-8', 'replace')

    def section_data(self, index):
        if index >= len(self.sections):
            raise ValueError('ELF section index out of bounds')
        s = self.sections[index]
        return self.slice(s['offset'], s['size'])

    def function_bytes(self, address, size):
        if size <= 0:
            return None
        for section in self.sections:
            if (section['flags'] & 2 and section['type'] != 8
                    and section['address'] <= address
                    and address + size <= section['address'] + section['size']):
                return self.slice(section['offset'] + address - section['address'], size)
        return None

    def metadata(self):
        return dict(image=self.label, sha256=hashlib.sha256(self.blob).hexdigest(),
            elf_type=self.kind, machine='AArch64', load_bias=self.bias,
            symbol_count=len(self.symbols), relocation_count=len(self.relocations),
            relocation_types=dict(sorted(Counter(str(r['type']) for r in self.relocations).items())),
            allocated_sections=[{k: s[k] for k in ('index', 'name', 'address', 'raw_address', 'size', 'type', 'flags')}
                                for s in self.sections if s['flags'] & 2])


def collect_binary_evidence(elf_path, addresses, nss_path=None, include_hashes=False, load_bias=LOAD_BIAS):
    """Return deterministic evidence for full inventory addresses.

    ``addresses`` is either {address: byte_size}, or an iterable of addresses
    (size unknown/zero). Exact symbol starts, including aliases and conflicting
    sizes, are retained. Relocation membership uses half-open function extents;
    size-zero queries match only the exact address. Files are identified by hash,
    not machine-dependent filesystem paths. When both images are supplied,
    requested function bytes must agree; a mismatch raises ValueError.
    """
    sizes = dict(addresses) if hasattr(addresses, 'items') else {a: 0 for a in addresses}
    if any(not isinstance(a, int) or a < 0 or not isinstance(n, int) or n < 0 for a, n in sizes.items()):
        raise ValueError('addresses and sizes must be nonnegative integers')
    images = [BinaryImage(elf_path, 'elf', load_bias)] if elf_path is not None else []
    if nss_path is not None:
        images.append(BinaryImage(nss_path, 'nss', load_bias))
    if not images:
        raise ValueError('at least one ELF or NSS image is required')
    records = {a: dict(address=a, size=n, symbols=[], sections=[], relocations_at_or_within=[], relocations_targeting=[])
               for a, n in sorted(sizes.items())}
    for image in images:
        for symbol in image.symbols:
            if symbol['address'] in records:
                records[symbol['address']]['symbols'].append(symbol)
        locations = sorted(image.relocations, key=lambda r: r['address'])
        targets = sorted((r for r in image.relocations if r['target'] is not None), key=lambda r: r['target'])
        location_keys, target_keys = [r['address'] for r in locations], [r['target'] for r in targets]
        for address, record in records.items():
            end = address + max(1, record['size'])
            record['relocations_at_or_within'].extend(locations[bisect_left(location_keys, address):bisect_left(location_keys, end)])
            record['relocations_targeting'].extend(targets[bisect_left(target_keys, address):bisect_left(target_keys, end)])
            for s in image.sections:
                if s['flags'] & 2 and s['address'] <= address < s['address'] + s['size']:
                    record['sections'].append(dict(image=image.label, name=s['name'], index=s['index'],
                                                   contains_extent=end <= s['address'] + s['size']))
            if include_hashes and image.label == 'elf':
                body = image.function_bytes(address, record['size'])
                if body is not None:
                    record['sha256'] = hashlib.sha256(body).hexdigest()
        for record in records.values():
            record['symbols'].sort(key=lambda s: (s['image'], s['table'], s['name'], s['size'], s['type'], s['bind']))
    if len(images) == 2:
        for address, record in records.items():
            bodies = [image.function_bytes(address, record['size']) for image in images]
            if any(body is None for body in bodies):
                if record['size'] > 0:
                    raise ValueError(f'ELF/NSS function bytes unavailable at {address:#x}')
                status = 'unavailable'
            elif bodies[0] == bodies[1]:
                status = 'identical'
            else:
                raise ValueError(f'ELF/NSS function bytes differ at {address:#x}')
            record['byte_comparison'] = {
                'status': status,
                'elf_sha256': hashlib.sha256(bodies[0]).hexdigest() if bodies[0] is not None else None,
                'nss_sha256': hashlib.sha256(bodies[1]).hexdigest() if bodies[1] is not None else None,
            }
    return dict(schema_version=1, images=[image.metadata() for image in images], functions=list(records.values()))
