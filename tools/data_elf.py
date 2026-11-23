#!/usr/bin/env python3
"""Read bounded ELF64/AArch64 metadata for the data evidence ledger.

Addresses and sizes are the integers recorded by the ELF, without rebasing or
invented extents. No section contents or raw initializer bytes are returned.
Only explicit RELATIVE relocation addends are reported as resolved targets;
these targets are relative to the load bias, which this reader does not assume.
"""
from __future__ import annotations

import hashlib
import mmap
import struct
from pathlib import Path


class ELFError(ValueError):
    """An unsupported or structurally malformed ELF input."""


def inspect_elf(path: str | Path) -> dict:
    """Return JSON-serializable metadata; reject invalid ranges and links.

    Symbol ``type``/``binding`` and section/relocation ``type`` are ELF numeric
    constants. Zero symbol sizes remain zero. Init/fini array entries expose
    addresses and relocation evidence, never the stored pointer bytes.
    """
    path = Path(path)
    with path.open('rb') as source:
        if path.stat().st_size < 64:
            raise ELFError('truncated ELF header')
        with mmap.mmap(source.fileno(), 0, access=mmap.ACCESS_READ) as data:
            return _inspect(data, str(path))


def _inspect(data, path):
    def require(condition, message):
        if not condition:
            raise ELFError(message)

    def span(offset, size, label):
        require(0 <= offset <= len(data) and 0 <= size <= len(data) - offset,
                f'{label}: file range out of bounds')

    def unpack(fmt, offset, label):
        span(offset, struct.calcsize(fmt), label)
        return struct.unpack_from(fmt, data, offset)

    require(data[:4] == b'\x7fELF', 'not an ELF file')
    require(data[4:7] == b'\x02\x01\x01', 'expected ELF64 little-endian version 1')
    (elf_type, machine, version, entry, phoff, shoff, flags, ehsize,
     phentsize, phnum, shentsize, shnum, shstrndx) = unpack('<HHIQQQIHHHHHH', 16, 'header')
    require(machine == 183 and version == 1, 'expected AArch64 ELF version 1')
    require(elf_type in (2, 3), 'expected executable or shared-object ELF')
    require(ehsize == 64, 'invalid ELF header size')
    # Extended numbering is carried by section zero.
    if shoff:
        require(shentsize == 64, 'invalid section header entry size')
        zero = unpack('<IIQQQQIIQQ', shoff, 'section zero')
        if shnum == 0:
            shnum = zero[5]
        if shstrndx == 0xffff:
            shstrndx = zero[6]
        if phnum == 0xffff:
            phnum = zero[7]
    else:
        require(shnum == 0 and shstrndx == 0 and phnum != 0xffff,
                'missing section table for section numbering')
    span(shoff, shnum * shentsize, 'section table')
    sections = []
    names = []
    for i in range(shnum):
        name, typ, sf, address, offset, size, link, info, align, entsize = unpack(
            '<IIQQQQIIQQ', shoff + i * shentsize, 'section header')
        if typ not in (0, 8):  # NULL and NOBITS have no file contents.
            span(offset, size, f'section {i}')
        require(align == 0 or align & (align - 1) == 0, f'section {i}: invalid alignment')
        names.append(name)
        sections.append(dict(index=i, name='', type=typ, flags=sf, address=address,
                             offset=offset, size=size, link=link, info=info,
                             alignment=align, entry_size=entsize))

    def linked(index, typ, label):
        require(0 <= index < len(sections), f'{label}: invalid section link')
        section = sections[index]
        require(section['type'] in typ, f'{label}: unexpected linked section type')
        return section

    def string(section, index):
        require(0 <= index < section['size'], 'string index out of bounds')
        start = section['offset'] + index
        end = data.find(b'\0', start, section['offset'] + section['size'])
        require(end >= 0, 'unterminated ELF string')
        return data[start:end].decode('utf-8')

    if shstrndx:
        strings = linked(shstrndx, (3,), 'section names')
        for section, name in zip(sections, names):
            section['name'] = string(strings, name)
    else:
        require(not any(names), 'section names without a string table')

    def records(section, size, label):
        require(section['entry_size'] == size and section['size'] % size == 0,
                f'{label}: invalid entry size or truncated record')
        return range(section['offset'], section['offset'] + section['size'], size)

    segments = []
    if phnum:
        require(phentsize == 56, 'invalid program header entry size')
        span(phoff, phnum * phentsize, 'program header table')
    for i in range(phnum):
        typ, pf, off, address, physical, filesz, memsz, align = unpack(
            '<IIQQQQQQ', phoff + i * phentsize, 'program header')
        span(off, filesz, f'segment {i}')
        require(typ not in (1, 7) or filesz <= memsz, 'segment file size exceeds memory size')
        segments.append(dict(index=i, type=typ, flags=pf, offset=off, address=address,
                             file_size=filesz, memory_size=memsz, alignment=align))

    extended = {}
    for section in sections:
        if section['type'] == 18:  # SHT_SYMTAB_SHNDX
            linked(section['link'], (2, 11), 'extended symbol indices')
            require(section['link'] not in extended, 'duplicate extended symbol index table')
            extended[section['link']] = [unpack('<I', off, 'symbol section index')[0]
                                         for off in records(section, 4, 'symbol indices')]
    symbols, tables = [], {}
    for section in sections:
        if section['type'] not in (2, 11):
            continue
        strings = linked(section['link'], (3,), 'symbol strings')
        table = []
        for index, off in enumerate(records(section, 24, 'symbols')):
            name, info, other, shndx, value, size = unpack('<IBBHQQ', off, 'symbol')
            if shndx == 0xffff:
                indices = extended.get(section['index'], [])
                require(index < len(indices), 'missing extended symbol index')
                shndx = indices[index]
                require(shndx < len(sections), 'invalid extended symbol section')
            else:
                require(shndx >= 0xff00 or shndx < len(sections), 'invalid symbol section')
            symbol = dict(table_section=section['index'], index=index, name=string(strings, name),
                          address=value, size=size, type=info & 15, binding=info >> 4,
                          visibility=other & 3, section_index=shndx, defined=shndx != 0)
            table.append(symbol)
        if section['index'] in extended:
            require(len(extended[section['index']]) == len(table), 'symbol index count mismatch')
        tables[section['index']] = table
        symbols.extend(table)

    relocations = []
    for section in sections:
        if section['type'] not in (4, 9):
            continue
        linked(section['link'], (2, 11), 'relocation symbols')
        require(section['info'] < len(sections), 'invalid relocation target section')
        table = tables[section['link']]
        rela = section['type'] == 4
        for off in records(section, 24 if rela else 16, 'relocations'):
            values = unpack('<QQq' if rela else '<QQ', off, 'relocation')
            address, info = values[:2]
            index, typ = info >> 32, info & 0xffffffff
            require(index < len(table), 'relocation symbol index out of bounds')
            relocation = dict(section_index=section['index'], offset=address, type=typ,
                              symbol_index=index, symbol_name=table[index]['name'],
                              symbol_table=section['link'], addend=values[2] if rela else None)
            if typ == 1027:
                require(index == 0, 'RELATIVE relocation has a symbol index')
            if typ == 1027 and rela:
                relocation['target'] = values[2]
                relocation['target_basis'] = 'load_bias_relative'
            relocations.append(relocation)

    by_offset = {}
    for relocation in relocations:
        by_offset.setdefault(relocation['offset'], []).append(relocation)
    arrays, dynamic_tags = [], []
    for section in sections:
        if section['type'] in (14, 15, 16):
            require(section['size'] % 8 == 0 and section['entry_size'] in (0, 8),
                    'invalid initializer array size')
            entries = []
            for index in range(section['size'] // 8):
                address = section['address'] + index * 8
                entries.append(dict(index=index, address=address,
                                    relocations=by_offset.get(address, [])))
            arrays.append(dict(section_index=section['index'], name=section['name'],
                               kind={14: 'init', 15: 'fini', 16: 'preinit'}[section['type']],
                               address=section['address'], size=section['size'], entries=entries))
        if section['type'] == 6:
            for off in records(section, 16, 'dynamic tags'):
                tag, value = unpack('<qQ', off, 'dynamic tag')
                dynamic_tags.append(dict(section_index=section['index'], tag=tag, value=value))
                if tag == 0:
                    break
    return dict(path=path, sha256=hashlib.sha256(data).hexdigest(), file_size=len(data),
                elf_type=elf_type, machine=machine, entry=entry, sections=sections,
                symbols=symbols, relocations=relocations, arrays=arrays,
                dynamic_tags=dynamic_tags, tls_segments=[s for s in segments if s['type'] == 7],
                segments=segments)
