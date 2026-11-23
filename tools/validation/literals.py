"""Pinned original/object diagnostic for Lua's merged out-of-memory string.

This proves only the bounded bytes and address-materialization correspondence.
It does not establish source-to-object production, original allocation ownership,
candidate link placement, shared consumers, or data-policy acceptance.
"""
import hashlib
import os
from pathlib import Path
import re
import stat
import struct
import tempfile

from data_elf import _inspect
import recovery_mapping
from .elf import Elf, MASKS
from . import strict
from .imports import _mapped, _require, _section

LITERAL = b'not enough memory\0'
OWNER = 'Contrib/Contrib/lua/src/lstate.c'
FUNCTION = 'f_luaopen'
MAX_INPUT = 64 * 1024 * 1024


def _read(path, expected):
    _require(isinstance(expected, str) and re.fullmatch(r'[0-9a-f]{64}', expected), 'SHA-256 pin required')
    fd = os.open(path, os.O_RDONLY | os.O_NOFOLLOW | os.O_NONBLOCK)
    with os.fdopen(fd, 'rb') as f:
        before = os.fstat(f.fileno())
        _require(stat.S_ISREG(before.st_mode) and 0 < before.st_size <= MAX_INPUT, 'bounded regular input required')
        data = f.read(before.st_size + 1)
        after = os.fstat(f.fileno())
        _require(len(data) == before.st_size and (before.st_dev,before.st_ino,before.st_size,before.st_mtime_ns,before.st_ctime_ns) ==
                 (after.st_dev,after.st_ino,after.st_size,after.st_mtime_ns,after.st_ctime_ns), 'input changed')
    _require(hashlib.sha256(data).hexdigest() == expected, 'input pin mismatch')
    return data


def _original_data(data, pc, size, words):
    meta = _inspect(data, '<pinned-original>')
    text = _section(meta, '.text', 1, 6)
    ro = _section(meta, '.rodata', 1, 2)
    _require(text['address'] <= pc and pc + 8 <= text['address'] + text['size'], 'original pair outside text')
    for section, flags, forbidden in ((text,5,2),(ro,4,3)):
        _require(_mapped(meta, section['address'], section['size'], flags, forbidden) == section['offset'], 'original section mapping differs')
    a, b = struct.unpack_from('<II', data, _mapped(meta, pc, 8, 5, 2))
    ca, cb = words
    _require(a & 0x9f000000 == 0x90000000 and b & 0xffc00000 == 0x91000000 and
             ca & 0x9f000000 == 0x90000000 and cb & 0xffc00000 == 0x91000000,
             'unsupported ADRP/ADD instruction')
    _require(a & 31 != 31 and ca & 31 != 31 and
             a & 31 == (b >> 5) & 31 == b & 31 and ca & 31 == (cb >> 5) & 31 == cb & 31,
             'ADRP/ADD register flow differs')
    _require((a & ~MASKS[275]) == (ca & ~MASKS[275]) and
             (b & ~MASKS[277]) == (cb & ~MASKS[277]), 'original and object opcodes/registers differ')
    _require(ca & MASKS[275] == 0 and cb & MASKS[277] == 0, 'nonzero candidate RELA instruction immediate')
    imm = ((a >> 29) & 3) | (((a >> 5) & 0x7ffff) << 2)
    if imm & (1 << 20): imm -= 1 << 21
    target = (pc & ~4095) + (imm << 12) + ((b >> 10) & 4095)
    _require(ro['address'] <= target and target + size <= ro['address'] + ro['size'], 'literal outside readonly section')
    offset = _mapped(meta, target, size, 4, 3)
    _require(data[offset:offset+size] == LITERAL, 'original literal bytes differ')
    # Dynamic tables must describe exactly the parsed relocation sections. This
    # bounded case supports only the original's known 8-byte write kinds.
    dyn = _section(meta, '.dynamic', 6, 3)
    rd = _section(meta, '.rela.dyn', 4, 2)
    rp = _section(meta, '.rela.plt', 4, 2)
    syms = _section(meta, '.dynsym', 11, 2)
    strings = _section(meta, '.dynstr', 3, 2)
    _require(rd['link']==rp['link']==syms['index'] and syms['link']==dyn['link']==strings['index'] and syms['entry_size']==24, 'dynamic symbol table links differ')
    for sec in (syms,strings):
        _require(_mapped(meta,sec['address'],sec['size'],4,3)==sec['offset'], 'dynamic symbol mapping differs')
    segments = [s for s in meta['segments'] if s['type'] == 2]
    _require(len(segments)==1 and (segments[0]['address'],segments[0]['offset'],segments[0]['file_size'],segments[0]['memory_size']) ==
             (dyn['address'],dyn['offset'],dyn['size'],dyn['size']), 'dynamic segment differs')
    _require(_mapped(meta,dyn['address'],dyn['size'],6,1)==dyn['offset'], 'dynamic mapping differs')
    for sec in (rd,rp):
        _require(_mapped(meta,sec['address'],sec['size'],4,3)==sec['offset'] and sec['entry_size']==24, 'relocation table mapping differs')
    tags = meta['dynamic_tags']
    allowed={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,20,21,23,25,27,0x6ffffef5,0x6ffffff9}
    _require(tags and tags[-1]['tag']==0 and all(t['section_index']==dyn['index'] and t['tag'] in allowed for t in tags), 'unsupported dynamic tags')
    for number,value in ((2,rp['size']),(23,rp['address']),(20,7),(7,rd['address']),(8,rd['size']),(9,24),(6,syms['address']),(11,24),(5,strings['address']),(10,strings['size'])):
        found=[t['value'] for t in tags if t['tag']==number]
        _require(found==[value], 'dynamic relocation binding differs')
    counts=[t['value'] for t in tags if t['tag']==0x6ffffff9]
    _require(len(counts)<=1 and (not counts or counts[0] <= rd['size']//24), 'relative count differs')
    if counts:
        for i in range(counts[0]):
            _require(struct.unpack_from('<Q',data,rd['offset']+i*24+8)[0]==1027, 'relative prefix differs')
    for r in meta['relocations']:
        _require(r['section_index'] in (rd['index'],rp['index']) and r['type'] in (257,1025,1026,1027), 'unsupported original relocation')
        _require(not any(r['offset'] < end and start < r['offset']+8 for start,end in ((pc,pc+8),(target,target+size))),
                 'original relocation overlaps literal or consumer')
    return target, (a,b)


def prove_lstate_literal(original_path, original_sha256, object_path, object_sha256,
                         owner_path, owner_sha256, ir_path, ir_sha256, owner_context):
    """Prove one exact pinned original/object string use; no admission changes."""
    _require(isinstance(owner_context, strict._OwnerContext) and owner_context.owner == OWNER and
             owner_context.original_sha256 == original_sha256 and
             Path(original_path).resolve() == (Path(owner_context.root)/'data/2.6.2/main.elf').resolve() and
             strict.build_owner_context(owner_context.root, owner_context.owner, dict(owner_context.pins)) == owner_context,
             'owner context is missing or changed')
    members = [m for m in owner_context.members if m[0] == FUNCTION]
    _require(len(members) == 1, 'original owner function missing or ambiguous')
    original_function_address = members[0][1] & 0xffffffff
    original_function_size = members[0][2]
    original = _read(original_path, original_sha256)
    obj = _read(object_path, object_sha256)
    _read(owner_path, owner_sha256); _read(ir_path, ir_sha256)
    _require(type(original_function_address) is int and original_function_address >= 0 and original_function_address % 4 == 0 and
             type(original_function_size) is int and original_function_size > 0 and original_function_size % 4 == 0,
             'invalid original function extent')
    # The private copy binds both existing object parsers to exactly the pinned
    # bytes, without allowing changes to the caller's object during parsing.
    with tempfile.TemporaryDirectory(prefix='literal-witness-') as directory:
        path=Path(directory)/'owner.o';path.write_bytes(obj);path.chmod(0o400)
        snap=recovery_mapping.inspect(path); e=Elf(path)
        strict._object_layout(e)
        _require(hashlib.sha256(e.d).hexdigest()==object_sha256, 'private object changed')
    sec=snap['sections'].get('.rodata.str1.1')
    _require(sec is not None and (sec['type'],sec['flags'],sec['size'],sec['alignment'],sec['entry_size']) == (1,0x32,18,1,1),
             'unsupported merged string section')
    raw=e.sections[sec['index']]
    _require(e.d[raw[4]:raw[4]+raw[5]]==LITERAL and LITERAL.find(b'\0')==17, 'object string bytes differ')
    funcs=[f for f in snap['functions'] if f['name']==FUNCTION]
    _require(len(funcs)==1 and funcs[0]['size']==original_function_size and funcs[0]['binding']==0 and funcs[0]['other']==0, 'candidate function extent differs')
    function=funcs[0];code=snap['sections'][function['section']]
    _require(not any(f is not function and f['section']==function['section'] and f['offset'] < function['offset']+function['size'] and function['offset'] < f['offset']+f['size'] for f in snap['functions']), 'overlapping function aliases')
    _require(code['type']==1 and code['flags'] & 7==6 and function['offset'] % 4 == 0 and
             function['offset']+function['size']<=code['size'], 'invalid candidate function section')
    _require(not any(r['section']=='.rodata.str1.1' for r in snap['relocations']), 'relocation writes into object literal')
    incoming=[r for r in snap['relocations'] if r['symbol']['section_index']==sec['index']]
    _require(len(incoming)==2, 'ambiguous or unsupported literal consumers')
    incoming.sort(key=lambda r:r['offset']);first,second=incoming
    widths={257:8,258:4,259:2,260:8,261:4,262:2,**{kind:4 for kind in MASKS}}
    for r in snap['relocations']:
        if r['section'] != function['section']:
            continue
        width=widths.get(r['type'])
        _require(width is not None and r['offset']+width <= code['size'], 'unsupported executable relocation width or extent')
        if r is not first and r is not second:
            _require(not (r['offset'] < second['offset']+4 and first['offset'] < r['offset']+width), 'relocation overlaps literal consumer')
    symbol=first['symbol']
    _require(first['section']==second['section']==function['section'] and first['type']==275 and second['type']==277 and
             first['addend']==second['addend']==0 and first['offset'] % 4 == 0 and second['offset']==first['offset']+4 and symbol==second['symbol'] and
             symbol['type']==3 and symbol['binding']==0 and symbol['other']==0 and symbol['offset']==0 and symbol['size']==0 and
             function['offset']<=first['offset'] and second['offset']+4<=function['offset']+function['size'], 'unsupported literal relocation pair')
    relative=first['offset']-function['offset']
    code_raw=e.sections[code['index']]
    words=struct.unpack_from('<II',e.d,code_raw[4]+first['offset'])
    pc=original_function_address+relative
    target,original_words=_original_data(original,pc,len(LITERAL),words)
    return {'kind':'lua-lstate-merged-string-original-object-v1','owner':OWNER,'function':FUNCTION,
            'original_sha256':original_sha256,'object_sha256':object_sha256,'owner_sha256':owner_sha256,'ir_sha256':ir_sha256,
            'original_inventory_pins':dict(owner_context.pins),
            'original_function_address':original_function_address,'original_function_size':original_function_size,
            'object_section':'.rodata.str1.1','object_function_offset':relative,'original_consumer':pc,
            'original_address':target,'extent_bytes':len(LITERAL),'string_sha256':hashlib.sha256(LITERAL).hexdigest(),
            'original_instructions':list(original_words),'object_instructions':list(words),
            'source_ir_object_lineage_proven':False,'original_allocation_identity_proven':False,
            'candidate_linked_data_proven':False,'shared_consumers_proven':False,'receipt_acceptance':False}
