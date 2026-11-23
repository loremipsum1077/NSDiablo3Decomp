"""Bounded original/object Lua lexer data diagnostic; never grants acceptance."""
import hashlib
import os
from pathlib import Path
import struct
import tempfile

from data_elf import _inspect
import recovery_mapping
from . import qualified_owner, strict
from .elf import Elf, MASKS
from .imports import _mapped, _require, _section

OWNER = qualified_owner.OWNER


def _dynamic(original, meta):
    """Bind dynamic relocation sections to PT_DYNAMIC, rejecting unknown writes."""
    dyn = _section(meta, '.dynamic', 6, 3)
    rd = _section(meta, '.rela.dyn', 4, 2)
    rp = _section(meta, '.rela.plt', 4, 2)
    syms = _section(meta, '.dynsym', 11, 2)
    strings = _section(meta, '.dynstr', 3, 2)
    _require(rd['link'] == rp['link'] == syms['index'] and syms['link'] == dyn['link'] == strings['index'] and syms['entry_size'] == 24, 'dynamic table links differ')
    segments = [x for x in meta['segments'] if x['type'] == 2]
    _require(len(segments) == 1 and (segments[0]['address'], segments[0]['offset'], segments[0]['file_size'], segments[0]['memory_size']) == (dyn['address'], dyn['offset'], dyn['size'], dyn['size']), 'dynamic segment differs')
    for sec, need, forbid in ((dyn,6,1),(rd,4,3),(rp,4,3),(syms,4,3),(strings,4,3)):
        _require(_mapped(meta,sec['address'],sec['size'],need,forbid) == sec['offset'], 'dynamic mapping differs')
    _require(rd['entry_size'] == rp['entry_size'] == 24, 'invalid dynamic relocation width')
    tags=meta['dynamic_tags']; allowed={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,20,21,23,25,27,0x6ffffef5,0x6ffffff9}
    _require(tags and tags[-1]['tag']==0 and all(t['section_index']==dyn['index'] and t['tag'] in allowed for t in tags), 'unsupported dynamic tags')
    for tag,value in ((2,rp['size']),(23,rp['address']),(20,7),(7,rd['address']),(8,rd['size']),(9,24),(6,syms['address']),(11,24),(5,strings['address']),(10,strings['size'])):
        _require([t['value'] for t in tags if t['tag']==tag]==[value], 'dynamic binding differs')
    counts=[t['value'] for t in tags if t['tag']==0x6ffffff9]
    _require(len(counts)<=1 and (not counts or counts[0]<=rd['size']//24), 'relative count differs')
    if counts:
        for i in range(counts[0]):
            _require(struct.unpack_from('<Q',original,rd['offset']+i*24+8)[0]==1027, 'relative prefix differs')
    for r in meta['relocations']:
        _require(r['section_index'] in (rd['index'],rp['index']) and r['type'] in (257,1025,1026,1027) and 0<=r['offset']<=(1<<64)-8, 'unsupported original relocation or overflowing write')
    return rd


def _no_original_writes(relocations,start,size):
    _require(not any(r['offset'] < start+size and start < r['offset']+8 for r in relocations), 'original relocation overlaps protected bytes')


def _pair(original, meta, pc, words, size, *, readonly=True):
    oa,ob=struct.unpack_from('<II',original,_mapped(meta,pc,8,5,2)); ca,cb=words
    for a,b in ((oa,ob),(ca,cb)):
        _require(a&0x9f000000==0x90000000 and b&0xffc00000==0x91000000 and a&31!=31 and a&31==(b>>5)&31==b&31, 'unsupported ADRP/ADD register flow')
    _require((oa&~MASKS[275])==(ca&~MASKS[275]) and (ob&~MASKS[277])==(cb&~MASKS[277]) and ca&MASKS[275]==0 and cb&MASKS[277]==0, 'pair opcode or immediate differs')
    imm=((oa>>29)&3)|(((oa>>5)&0x7ffff)<<2)
    if imm&(1<<20):imm-=1<<21
    target=(pc&~4095)+(imm<<12)+((ob>>10)&4095)
    if readonly:
        ro=_section(meta,'.rodata',1,2)
        _require(ro['address']<=target and target+size<=ro['address']+ro['size'], 'string outside rodata')
        pos=_mapped(meta,target,size,4,3)
    else:pos=_mapped(meta,target,size,6,1)
    _no_original_writes(meta['relocations'],pc,8)
    return target,pos,(oa,ob)


def _cstring(data, section, start):
    _require(type(start)==int and 0<=start<section[5], 'string offset outside section')
    lo=section[4]+start; end=data.find(b'\0',lo,section[4]+section[5])
    _require(end>=lo and end-lo<4096, 'unterminated or excessive object string')
    return data[lo:end+1]


def prove(original_path, original_sha256, nss_path, nss_sha256, object_path, object_sha256, owner_context):
    """Prove lexer object/original references and data, conditional on pinned inputs.

    Original table is writable and no runtime RELRO protection is claimed.
    """
    ctx=qualified_owner.revalidate_context(owner_context); pins=dict(ctx.input_pins)
    _require(ctx.owner==OWNER and pins['original_sha256']==original_sha256 and pins['nss_sha256']==nss_sha256 and ctx.object_sha256==object_sha256, 'context pins differ')
    _require(Path(original_path).resolve()==Path(ctx.root)/qualified_owner.PINS['original_sha256'] and Path(nss_path).resolve()==Path(ctx.root)/qualified_owner.PINS['nss_sha256'] and Path(object_path).resolve()==Path(ctx.object_path), 'context paths differ')
    original=qualified_owner._snapshot(original_path,qualified_owner.LIMITS['original_sha256'],original_sha256)
    obj=qualified_owner._snapshot(object_path,4*1024**2,object_sha256)
    with qualified_owner._pinned_reader(nss_path,qualified_owner.LIMITS['nss_sha256'],nss_sha256) as reader:
        nsec=qualified_owner._file_table(reader); nsyms=qualified_owner._file_symbols(reader,nsec)
        matches=[s for s in nsyms if s[0]=='luaX_tokens' and s[4]!=0]
        _require(len(matches)==1 and matches[0][1:4]==(1,0,2) and matches[0][5:]==(0x10dca38,256), 'original token symbol differs')
        _require(not any(s!=matches[0] and s[1] in (1,2) and s[4]!=0 and s[6] and s[5]<0x10dca38+256 and 0x10dca38<s[5]+s[6] for s in nsyms), 'overlapping original token symbol')
        ns=nsec[matches[0][4]];_require(ns[1]==1 and ns[2]&7==3 and ns[3]<=0x10dca38 and 0x10dca38+256<=ns[3]+ns[5], 'original token extent differs')
        nss_table=reader.read(ns[4]+0x10dca38-ns[3],256)
    meta=_inspect(original,'<pinned-original>'); rd=_dynamic(original,meta); table_addr=0x10dca38
    tab_off=_mapped(meta,table_addr,256,6,1)
    _require(nss_table==original[tab_off:tab_off+256], 'NSS and original table bytes differ')
    ro=_section(meta,'.rodata',1,2)
    _require(_mapped(meta,ro['address'],ro['size'],4,3)==ro['offset'], 'rodata mapping differs')
    with tempfile.TemporaryDirectory(prefix='lexer-data-') as d:
        p=Path(d)/'candidate.o';p.write_bytes(obj);p.chmod(0o400);snap=recovery_mapping.inspect(p); e=Elf(p);strict._object_layout(e)
    _require(e.d==obj, 'object snapshot differs')
    sections=snap['sections']; strings=sections.get('.rodata.str1.1');table=sections.get('.data.rel.ro')
    _require(strings and (strings['type'],strings['flags'],strings['size'],strings['alignment'],strings['entry_size'])==(1,0x32,421,1,1), 'unsupported object strings')
    _require(table and (table['type'],table['flags'],table['size'],table['alignment'])==(1,3,256,8), 'unsupported object table')
    ts=[s for s in snap['symbols'] if s['name']=='luaX_tokens' and s['section_index']!=0]
    _require(len(ts)==1 and (ts[0]['type'],ts[0]['binding'],ts[0]['other'],ts[0]['section_index'],ts[0]['offset'],ts[0]['size'])==(1,1,2,table['index'],0,256),'candidate token symbol differs')
    _require(not any(s is not ts[0] and s['section_index']==table['index'] and s['type'] in (1,2) and s['size'] and s['offset']<256 and 0<s['offset']+s['size'] for s in snap['symbols']), 'overlapping candidate token symbol')
    rawsec=e.sections[table['index']];_require(obj[rawsec[4]:rawsec[4]+256]==bytes(256), 'table initializer differs')
    rels=snap['relocations'];_require(not any(r['section']=='.rodata.str1.1' for r in rels),'writes to object strings')
    table_rels=[r for r in rels if r['section']=='.data.rel.ro'];_require(len(table_rels)==31,'table relocation count differs')
    def is_str_symbol(s):return (s['type'],s['binding'],s['other'],s['section_index'],s['offset'],s['size'])==(3,0,0,strings['index'],0,0)
    slots=[]
    for i in range(32):
        addr=table_addr+i*8; hits=[r for r in meta['relocations'] if r['offset']<addr+8 and addr<r['offset']+8]
        if i==31:
            _require(not hits and original[tab_off+i*8:tab_off+i*8+8]==bytes(8), 'null token differs');slots.append({'index':i,'null':True});continue
        cr=[r for r in table_rels if r['offset']==i*8];_require(len(cr)==1 and cr[0]['type']==257 and is_str_symbol(cr[0]['symbol']), 'candidate token relocation differs')
        _require(len(hits)==1 and hits[0]['offset']==addr and hits[0]['section_index']==rd['index'] and hits[0]['type']==1027 and hits[0]['symbol_index']==0, 'original token relocation differs')
        target=hits[0]['addend']; _require(struct.unpack_from('<Q',original,tab_off+i*8)[0]==target, 'original token raw initializer differs')
        val=_cstring(obj,e.sections[strings['index']],cr[0]['addend']); off=_mapped(meta,target,len(val),4,3)
        ro=_section(meta,'.rodata',1,2);_require(ro['address']<=target and target+len(val)<=ro['address']+ro['size'] and original[off:off+len(val)]==val, 'token text differs');_no_original_writes(meta['relocations'],target,len(val))
        slots.append({'index':i,'object_string_addend':cr[0]['addend'],'original_slot':addr,'original_string_address':target,'extent_bytes':len(val),'string_sha256':hashlib.sha256(val).hexdigest()})
    # Examine all allocated references, including data slots, rather than debug records.
    alloc={n for n,s in sections.items() if s['flags']&2}; incoming=[r for r in rels if r['section'] in alloc and r['symbol']['section_index'] in (strings['index'],table['index'])]
    code=[r for r in incoming if r['section']!='.data.rel.ro'];_require(len(incoming)==71 and len(code)==40, 'data reference membership differs')
    _require(all(r['type'] in (275,277) for r in code), 'unsupported data consumer')
    widths={257:8,258:4,259:2,260:8,261:4,262:2,**{k:4 for k in MASKS}}
    result=[]; used=set()
    for a in [r for r in code if r['type']==275]:
        b=[r for r in code if r['section']==a['section'] and r['offset']==a['offset']+4 and r['type']==277 and r['symbol']==a['symbol'] and r['addend']==a['addend']]
        _require(len(b)==1 and a['offset']%4==0, 'incomplete consumer pair');b=b[0]
        _require(id(a) not in used and id(b) not in used, 'duplicate consumer pair')
        used.update((id(a),id(b)))
        for r in rels:
            if r['section']!=a['section']:continue
            width=widths.get(r['type']);_require(width is not None and r['offset']+width<=sections[a['section']]['size'],'unsupported code relocation width or extent')
            if r is not a and r is not b:_require(not(r['offset']<b['offset']+4 and a['offset']<r['offset']+width),'overlapping consumer write')
        members=[m for m in ctx.members if sections[a['section']]['index']==m.candidate_section and m.candidate_offset<=a['offset'] and b['offset']+4<=m.candidate_offset+m.candidate_size]
        _require(len(members)==1,'consumer owner ambiguous');m=members[0];_require(m.candidate_size==m.original_size,'function extent differs')
        offset=a['offset']-m.candidate_offset;pc=(m.original_address&0xffffffff)+offset
        is_table=a['symbol']['section_index']==table['index'];_require((a['symbol']==ts[0] and a['addend']==0) if is_table else is_str_symbol(a['symbol']), 'consumer symbol differs')
        val=b'' if is_table else _cstring(obj,e.sections[strings['index']],a['addend']);sec=e.sections[m.candidate_section];words=struct.unpack_from('<II',obj,sec[4]+a['offset'])
        target,original_offset,original_words=_pair(original,meta,pc,words,256 if is_table else len(val),readonly=not is_table)
        if is_table:_require(target==table_addr,'original table address differs')
        else:_require(original[original_offset:original_offset+len(val)]==val,'literal bytes differ');_no_original_writes(meta['relocations'],target,len(val))
        result.append({'function':m.original_name,'original_function_address':m.original_address,'original_function_size':m.original_size,'object_function_section_index':m.candidate_section,'object_function_start':m.candidate_offset,'object_function_offset':offset,'object_data_section_index':a['symbol']['section_index'],'object_data_addend':a['addend'],'original_consumer':pc,'original_address':target,'extent_bytes':256 if is_table else len(val),'object_instructions':list(words),'original_instructions':list(original_words),'kind':'table' if is_table else 'string','string_sha256':None if is_table else hashlib.sha256(val).hexdigest()})
    _require(len(used)==len(code) and len(result)==20 and sum(x['kind']=='string' for x in result)==18 and sum(x['kind']=='table' for x in result)==2,'consumer count differs')
    return {'kind':'lua-lexer-original-object-data-v1','original_sha256':original_sha256,'nss_sha256':nss_sha256,'object_sha256':object_sha256,'owner_inventory_pins':pins,'consumers':result,'token_slots':slots,'original_table_writable':True,'original_relro_proven':False,'original_allocation_identity_proven':False,'source_ir_object_lineage_proven':False,'candidate_linked_data_proven':False,'shared_consumers_proven':False,'receipt_acceptance':False}
