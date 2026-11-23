"""Original-side AArch64 BL -> ABI PLT -> JUMP_SLOT evidence only.

Requires an externally pinned original ELF SHA-256. No candidate object/link,
provider identity, runtime binding or receipt acceptance is established here.
"""
import hashlib
import os
import re
import stat
import struct
from pathlib import Path

from data_elf import _inspect

MAX_ELF = 64 * 1024 * 1024
MASK64 = (1 << 64) - 1


def _require(condition, reason):
    if not condition:
        raise ValueError(reason)


def _section(meta, name, kind, flags):
    matches = [s for s in meta['sections'] if s['name'] == name]
    _require(len(matches) == 1, 'missing or ambiguous '+name)
    section = matches[0]
    _require(section['type'] == kind and section['flags'] & 7 == flags,
             'unexpected '+name+' type or permissions')
    return section


def _mapped(meta, address, size, required, forbidden=0):
    _require(type(address) is int and type(size) is int and address >= 0 and size > 0
             and address + size <= MASK64, 'invalid address extent')
    segments = [s for s in meta['segments'] if s['type'] == 1 and
                s['address'] < address + size and address < s['address'] + s['memory_size']]
    _require(len(segments) == 1, 'ambiguous or missing load mapping')
    seg = segments[0]
    _require(seg['flags'] & required == required and not seg['flags'] & forbidden and
             seg['address'] <= address and address + size <= seg['address'] + seg['file_size'],
             'extent not file-backed with expected permissions')
    return seg['offset'] + address - seg['address']


def _covered(meta, section, required, forbidden=0):
    offset = _mapped(meta, section['address'], section['size'], required, forbidden)
    _require(offset == section['offset'], 'section and load mappings disagree')


def prove_original_call(path, expected_sha256, call_address, import_name):
    """Return a pinned call-site witness, or raise ValueError on any ambiguity.

    Addresses are unrebased ELF virtual addresses. This is evidence about a
    call instruction and its original import metadata, not caller ownership.
    """
    _require(isinstance(expected_sha256, str) and re.fullmatch(r'[0-9a-f]{64}', expected_sha256), 'original SHA-256 pin required')
    _require(type(call_address) is int and 0 <= call_address <= MASK64 - 4 and call_address % 4 == 0,
             'invalid call address')
    _require(isinstance(import_name, str) and re.fullmatch(r'[A-Za-z_][A-Za-z_0-9.$]*', import_name), 'invalid import name')
    path = Path(path)
    fd = os.open(path, os.O_RDONLY | os.O_NOFOLLOW | os.O_NONBLOCK)
    with os.fdopen(fd, 'rb') as f:
        before = os.fstat(f.fileno())
        _require(stat.S_ISREG(before.st_mode) and 0 < before.st_size <= MAX_ELF, 'original ELF must be a bounded regular file')
        data = f.read(before.st_size + 1)
        after = os.fstat(f.fileno())
        _require(len(data) == before.st_size and (before.st_dev,before.st_ino,before.st_size,before.st_mtime_ns,before.st_ctime_ns) ==
                 (after.st_dev,after.st_ino,after.st_size,after.st_mtime_ns,after.st_ctime_ns), 'original ELF changed')
    digest = hashlib.sha256(data).hexdigest()
    _require(digest == expected_sha256, 'original ELF pin mismatch')
    try:
        meta = _inspect(data, str(path))
    except (IndexError, struct.error, UnicodeError, OverflowError) as exc:
        raise ValueError('malformed original ELF') from exc
    _require(meta['elf_type'] in (2, 3), 'unsupported ELF type')
    text = _section(meta, '.text', 1, 6)
    plt = _section(meta, '.plt', 1, 6)
    got = _section(meta, '.got.plt', 1, 3)
    rela = _section(meta, '.rela.plt', 4, 2)
    dyn = _section(meta, '.dynamic', 6, 3)
    syms = _section(meta, '.dynsym', 11, 2)
    strings = _section(meta, '.dynstr', 3, 2)
    reladyn = _section(meta, '.rela.dyn', 4, 2)
    _require(rela['link'] == syms['index'] and rela['info'] == plt['index'] and
             reladyn['link'] == syms['index'] and syms['link'] == strings['index'] and
             dyn['link'] == strings['index'] and rela['entry_size'] == 24 and syms['entry_size'] == 24 and
             reladyn['entry_size'] == 24, 'unexpected dynamic section links')
    for section, flags, forbidden in ((text,5,2),(plt,5,2),(got,6,1),(rela,4,3),
                                      (dyn,6,1),(syms,4,3),(strings,4,3),(reladyn,4,3)):
        _covered(meta, section, flags, forbidden)
    dynamic = [s for s in meta['segments'] if s['type'] == 2]
    _require(len(dynamic) == 1 and dynamic[0]['address'] == dyn['address'] and
             dynamic[0]['offset'] == dyn['offset'] and dynamic[0]['file_size'] == dyn['size'] and
             dynamic[0]['memory_size'] == dyn['size'], 'dynamic segment and section differ')
    tags = meta['dynamic_tags']
    allowed = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,20,21,23,25,27,0x6ffffef5,0x6ffffff9}
    _require(tags and tags[-1]['tag'] == 0 and all(t['section_index'] == dyn['index'] and t['tag'] in allowed for t in tags),
             'unsupported dynamic tags')
    def tag(number):
        values = [t['value'] for t in tags if t['tag'] == number]
        _require(len(values) == 1, 'missing or duplicate dynamic tag')
        return values[0]
    for n, expected in ((3,got['address']),(2,rela['size']),(23,rela['address']),(20,7),
                        (7,reladyn['address']),(8,reladyn['size']),(9,24),(6,syms['address']),
                        (11,24),(5,strings['address']),(10,strings['size'])):
        _require(tag(n) == expected, 'dynamic relocation or symbol binding differs')
    relative_counts = [t['value'] for t in tags if t['tag'] == 0x6ffffff9]
    _require(len(relative_counts) <= 1, 'duplicate relative relocation count')
    if relative_counts:
        count = relative_counts[0]
        _require(count <= reladyn['size'] // 24, 'relative relocation count exceeds table')
        for i in range(count):
            _, info, _ = struct.unpack_from('<QQq', data, reladyn['offset'] + i * 24)
            _require(info == 1027, 'relative relocation prefix does not match count')
    _require(text['address'] <= call_address and call_address + 4 <= text['address'] + text['size'], 'call outside text')
    off = _mapped(meta, call_address, 4, 5, 2)
    word, = struct.unpack_from('<I', data, off)
    _require(word >> 26 == 0b100101, 'original instruction is not BL')
    imm = word & 0x3ffffff
    if imm & (1 << 25): imm -= 1 << 26
    target = call_address + imm * 4
    _require(target >= plt['address'] + 32 and target + 16 <= plt['address'] + plt['size'] and
             (target - plt['address'] - 32) % 16 == 0, 'call does not target PLT entry start')
    index = (target - plt['address'] - 32) // 16
    _require(index < rela['size'] // 24, 'PLT index outside relocation table')
    roff = rela['offset'] + index * 24
    slot, info, addend = struct.unpack_from('<QQq', data, roff)
    symidx, rtype = info >> 32, info & 0xffffffff
    _require(rtype == 1026 and addend == 0 and symidx and symidx * 24 + 24 <= syms['size'], 'unsupported PLT relocation')
    n, sym_info, other, shndx, value, size = struct.unpack_from('<IBBHQQ', data, syms['offset'] + symidx * 24)
    _require(n < strings['size'], 'symbol name outside string table')
    start = strings['offset'] + n
    end = data.find(b'\0', start, strings['offset'] + strings['size'])
    _require(end >= 0 and data[start:end] == import_name.encode() and shndx == 0 and value == 0 and size == 0 and
             sym_info >> 4 == 1 and sym_info & 15 == 2 and other == 0, 'unexpected imported symbol')
    same_name = [s for s in meta['symbols'] if s['table_section'] == syms['index'] and s['name'] == import_name]
    _require(len(same_name) == 1 and same_name[0]['index'] == symidx, 'ambiguous imported symbol')
    _require(slot % 8 == 0 and got['address'] <= slot and slot + 8 <= got['address'] + got['size'], 'GOT slot outside .got.plt')
    _mapped(meta, slot, 8, 6, 1)
    a,l,d,b = struct.unpack_from('<IIII', data, _mapped(meta, target, 16, 5, 2))
    _require(a & 0x9f00001f == 0x90000010 and l & 0xffc003ff == 0xf9400211 and
             d & 0xffc003ff == 0x91000210 and b == 0xd61f0220, 'unsupported PLT instructions')
    pageimm = ((a >> 29) & 3) | (((a >> 5) & 0x7ffff) << 2)
    if pageimm & (1 << 20): pageimm -= 1 << 21
    page = (target & ~4095) + (pageimm << 12)
    _require(page + ((l >> 10) & 4095) * 8 == slot and page + ((d >> 10) & 4095) == slot,
             'PLT load/add do not name relocation slot')
    # Supported relocation writes are all 8 bytes; reject unmodeled write kinds
    # and any second write overlapping code, stub, or slot.
    hit = []
    for r in meta['relocations']:
        _require(r['section_index'] in (rela['index'],reladyn['index']) and r['type'] in (257,1025,1026,1027),
                 'unmodeled relocation writes')
        pos = r['offset']
        for lo, hi in ((call_address,call_address+4),(target,target+16),(slot,slot+8)):
            if pos < hi and lo < pos + 8:
                hit.append(r)
                break
    _require(len(hit) == 1 and hit[0]['section_index'] == rela['index'] and hit[0]['offset'] == slot and
             hit[0]['symbol_index'] == symidx and hit[0]['type'] == 1026 and hit[0]['addend'] == 0,
             'conflicting relocation write')
    return {'kind':'original-import-call-v1','original_sha256':digest,'call_address':call_address,
            'call_instruction':word,'plt_address':target,'plt_instructions':[a,l,d,b],
            'plt_index':index,'got_address':slot,'relocation_type':rtype,'relocation_addend':addend,
            'dynamic_symbol_index':symidx,'symbol':import_name,'symbol_binding':sym_info >> 4,
            'symbol_type':sym_info & 15,'symbol_visibility':other,
            'scope':'original call and import metadata only; no candidate or runtime provider proof'}
