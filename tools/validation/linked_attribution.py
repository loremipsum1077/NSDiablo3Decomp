"""Bounded, non-admitting linked-data diagnostics.

`corroborate_contribution` checks an explicit map row and file bytes; it
cannot attribute LLD's internal-only merged string map records.
`prove_linked_lstate_literal` instead routes the exact owner function and
decodes its linked consumer, permitting internal merged string storage. It
reconstructs all function relocations it accepts rather than masking them.
Neither API authenticates that a declared manifest is the actual link command,
proves source-to-link lineage or complete merge/consumer closure, or changes
receipt acceptance. Unsupported inputs fail closed.
"""
import hashlib
import json
from pathlib import Path
import re
import struct
import tempfile

import recovery_mapping
from .elf import Elf
from .imports import _mapped, _require
from .literals import _read
from .tables import _original_metadata, _no_original_write

_RECORD=re.compile(r'([^\s():]+):\(([^\s()]+)\)\Z')
_HEX=re.compile(r'[0-9a-fA-F]{1,16}\Z')


def _json_unique(raw):
    def pairs(values):
        out={}
        for key,value in values:
            _require(key not in out,'duplicate provenance field');out[key]=value
        return out
    try:return json.loads(raw,object_pairs_hook=pairs)
    except (ValueError,UnicodeError) as exc:raise ValueError('invalid provenance record') from exc


def _map_records(raw):
    try:lines=raw.decode('utf-8').splitlines()
    except UnicodeError as exc:raise ValueError('invalid map encoding') from exc
    _require(lines and lines[0].split()==['Address','Size','Align','Out','In','Symbol'],'unsupported map format')
    records=[]
    for line in lines[1:]:
        fields=line.split()
        if ':(' not in line:continue
        _require(len(fields)==4 and _HEX.fullmatch(fields[0]) and _HEX.fullmatch(fields[1]) and
                 fields[2].isdigit() and len(fields[2])<=10,'malformed contribution row')
        m=_RECORD.fullmatch(fields[3]);_require(m is not None,'malformed contribution identity')
        address,size,alignment=int(fields[0],16),int(fields[1],16),int(fields[2])
        _require(address+size<=1<<64 and 0<alignment<=1<<31 and alignment&(alignment-1)==0,
                 'invalid contribution range or alignment')
        records.append({'address':address,'size':size,'alignment':alignment,'input':m[1],'section':m[2]})
    return records


def corroborate_contribution(link_path, link_sha256, map_path, map_sha256, provenance_path, provenance_sha256,
                             object_path, object_sha256, input_identity, section_name):
    """Check one explicit same-sized data contribution and linked file bytes.

    All hashes are externally supplied and establish consistency only. A map
    path labels a purported input, not a cryptographic proof it was linked.
    """
    _require(isinstance(input_identity,str) and input_identity.startswith('/work/build/2.6.2/') and
             len(input_identity)<=4096 and all(x not in ('','.','..') for x in input_identity[1:].split('/')) and
             not any(x in input_identity for x in ('\\','(',')',':','\0',' ')) and input_identity.endswith('.obj'),
             'unsupported linker input identity')
    _require(isinstance(section_name,str) and re.fullmatch(r'\.[A-Za-z0-9_.$-]+',section_name), 'invalid section name')
    link=_read(link_path,link_sha256);mapdata=_read(map_path,map_sha256);record_data=_read(provenance_path,provenance_sha256)
    obj=_read(object_path,object_sha256)
    record=_json_unique(record_data)
    _require(isinstance(record,dict) and set(record)=={'format','elf_sha256','map_sha256'} and
             type(record['format']) is int and record['format']==1 and record['elf_sha256']==link_sha256 and
             record['map_sha256']==map_sha256,'link provenance does not bind pinned ELF and map')
    with tempfile.TemporaryDirectory(prefix='linked-attribution-') as directory:
        p=Path(directory)/'object.o';p.write_bytes(obj);p.chmod(0o400)
        snapshot=recovery_mapping.inspect(p);e=Elf(p)
        _require(hashlib.sha256(e.d).hexdigest()==object_sha256,'private object changed')
    section=snapshot['sections'].get(section_name)
    _require(section is not None and section['type']==1 and section['flags'] in (2,0x32) and
             section['size']>0 and section['alignment']>0 and section['alignment']&(section['alignment']-1)==0,
             'unsupported object data section')
    rawsection=e.sections[section['index']];payload=e.d[rawsection[4]:rawsection[4]+rawsection[5]]
    _require(len(payload)==section['size'] and hashlib.sha256(payload).hexdigest()==section['sha256'], 'object section changed')
    _require(not any(r['section']==section_name for r in snapshot['relocations']), 'relocation writes object contribution')
    records=_map_records(mapdata)
    matches=[r for r in records if r['input']==input_identity and r['section']==section_name]
    _require(len(matches)==1, 'missing or ambiguous explicit map contribution')
    row=matches[0];address=row['address'];size=row['size']
    _require(size==section['size'] and row['alignment']==section['alignment'] and address%section['alignment']==0,
             'map contribution extent or alignment differs')
    _require(not any(r is not row and r['size'] and r['address']<address+size and address<r['address']+r['size'] for r in records),
             'overlapping or merged map contribution')
    meta,_,ro=_original_metadata(link)
    _require(ro['address']<=address and address+size<=ro['address']+ro['size'], 'contribution outside readonly output')
    _require(not any(s['index']!=ro['index'] and s['flags']&2 and s['address']<address+size and address<s['address']+s['size'] for s in meta['sections']),
             'contribution overlaps allocated output section')
    pos=_mapped(meta,address,size,4,3);_no_original_write(meta,address,address+size)
    _require(link[pos:pos+size]==payload, 'linked bytes differ from object section')
    return {'kind':'explicit-link-map-contribution-corroboration-v1','link_sha256':link_sha256,'map_sha256':map_sha256,
            'provenance_sha256':provenance_sha256,'object_sha256':object_sha256,'input_identity':input_identity,
            'object_section':section_name,'address':address,'size':size,'alignment':section['alignment'],
            'section_sha256':section['sha256'],'merged_input':bool(section['flags']&0x10),
            'scope':'explicit pinned map and linked file bytes only',
            'link_input_identity_proven':False,'linked_consumer_proven':False,'all_merge_aliases_proven':False,
            'source_lineage_proven':False,'receipt_acceptance':False}


def _linked_metadata(data):
    """Bound the dynamic relocation tables and known loader write widths."""
    from data_elf import _inspect
    from .imports import _section
    meta=_inspect(data,'<pinned-linked>')
    text=_section(meta,'.text',1,6);ro=_section(meta,'.rodata',1,2)
    dyn=_section(meta,'.dynamic',6,3);rd=_section(meta,'.rela.dyn',4,2);rp=_section(meta,'.rela.plt',4,2)
    syms=_section(meta,'.dynsym',11,2);strings=_section(meta,'.dynstr',3,2)
    _require(rd['link']==rp['link']==syms['index'] and syms['link']==dyn['link']==strings['index'] and
             syms['entry_size']==24,'linked dynamic symbol links differ')
    for sec,flags,forbidden in ((text,5,2),(ro,4,3),(dyn,6,1),(rd,4,3),(rp,4,3),(syms,4,3),(strings,4,3)):
        _require(_mapped(meta,sec['address'],sec['size'],flags,forbidden)==sec['offset'],'linked section mapping differs')
    dynsegs=[s for s in meta['segments'] if s['type']==2]
    _require(len(dynsegs)==1 and (dynsegs[0]['address'],dynsegs[0]['offset'],dynsegs[0]['file_size'],dynsegs[0]['memory_size'])==
             (dyn['address'],dyn['offset'],dyn['size'],dyn['size']), 'linked dynamic segment differs')
    tags=meta['dynamic_tags'];allowed={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,20,21,23,25,27,30,0x6ffffef5,0x6ffffff9}
    _require(tags and tags[-1]['tag']==0 and all(t['section_index']==dyn['index'] and t['tag'] in allowed for t in tags), 'unsupported linked dynamic tags')
    flags=[t['value'] for t in tags if t['tag']==30]
    _require(len(flags)<=1 and (not flags or flags[0]&~2==0), 'unsupported linked dynamic flags')
    for number,value in ((2,rp['size']),(23,rp['address']),(20,7),(7,rd['address']),(8,rd['size']),(9,24),
                         (6,syms['address']),(11,24),(5,strings['address']),(10,strings['size'])):
        _require([t['value'] for t in tags if t['tag']==number]==[value], 'linked dynamic relocation binding differs')
    counts=[t['value'] for t in tags if t['tag']==0x6ffffff9]
    _require(len(counts)<=1 and (not counts or counts[0]<=rd['size']//24),'linked relative count differs')
    if counts:
        for i in range(counts[0]):
            _require(struct.unpack_from('<Q',data,rd['offset']+i*24+8)[0]==1027,'linked relative prefix differs')
    widths={257:8,1025:8,1026:8,1027:8,1031:16}
    for r in meta['relocations']:
        _require(r['section_index'] in (rd['index'],rp['index']) and r['type'] in widths,'unsupported linked relocation write')
        _require(r['offset']+widths[r['type']]<=1<<64,'linked relocation write overflows address space')
    _require(all(s['address']+s['memory_size']<=1<<64 for s in meta['segments'] if s['type']==1), 'linked segment address overflow')
    return meta,text,ro,widths


def _unique_allocated_extent(meta, section, start, size):
    _require(not any(s['index']!=section['index'] and s['flags']&2 and s['address']<start+size and
                     start<s['address']+s['size'] for s in meta['sections']), 'ambiguous allocated output extent')


def _no_linked_write(meta,widths,start,end):
    _require(not any(r['offset']<end and start<r['offset']+widths[r['type']] for r in meta['relocations']),
             'linked relocation overlaps proof range')


def _safe_identity(value):
    return (isinstance(value,str) and value.startswith('/work/build/2.6.2/') and len(value)<=4096 and
            all(x not in ('','.','..') for x in value[1:].split('/')) and
            not any(x in value for x in ('\\','(',')',':','\0',' ','\t','\n','\r')) and value.endswith('.obj'))


def _manifest(raw,identity,object_sha256):
    value=_json_unique(raw)
    _require(isinstance(value,dict) and set(value)=={'kind','complete','inputs'} and
             value['kind']=='link-input-manifest-v1' and value['complete'] is True and
             isinstance(value['inputs'],list) and 0<len(value['inputs'])<=100000, 'invalid complete link-input manifest')
    seen=set()
    for row in value['inputs']:
        _require(isinstance(row,dict) and set(row)=={'path','sha256'} and _safe_identity(row['path']) and
                 isinstance(row['sha256'],str) and re.fullmatch(r'[0-9a-f]{64}',row['sha256']) and row['path'] not in seen,
                 'invalid or duplicate link input')
        seen.add(row['path'])
    matches=[r for r in value['inputs'] if r['path']==identity]
    _require(len(matches)==1 and matches[0]['sha256']==object_sha256,'manifest owner input mismatch')
    return len(value['inputs'])


def _route(records,identity,section,size,alignment):
    rows=[r for r in records if r['input']==identity and r['section']==section]
    _require(len(rows)==1,'missing or ambiguous explicit function map contribution')
    row=rows[0];address=row['address']
    _require(type(size) is int and size>0 and type(alignment) is int and alignment>0 and alignment&(alignment-1)==0, 'invalid function contribution extent')
    _require(row['size']==size and row['alignment']==alignment and address%alignment==0,'function map extent differs')
    _require(not any(r is not row and r['size'] and r['address']<address+size and address<r['address']+r['size'] for r in records),
             'overlapping function map contribution')
    return row


def prove_linked_lstate_literal(original_path,original_sha256,object_path,object_sha256,
                                owner_path,owner_sha256,ir_path,ir_sha256,owner_context,
                                link_path,link_sha256,map_path,map_sha256,provenance_path,provenance_sha256,
                                manifest_path,manifest_sha256,input_identity):
    """Regenerate object proof and corroborate actual linked call-site bytes.

    The pinned input manifest and map record are observations, not an
    independently authenticated link command or complete consumer inventory.
    """
    from . import literals
    from .elf import MASKS
    _require(_safe_identity(input_identity) and input_identity.endswith('/src/'+literals.OWNER+'.obj'), 'wrong owner linker path')
    proof=literals.prove_lstate_literal(original_path,original_sha256,object_path,object_sha256,
                                        owner_path,owner_sha256,ir_path,ir_sha256,owner_context)
    link=_read(link_path,link_sha256);mapdata=_read(map_path,map_sha256);rawrecord=_read(provenance_path,provenance_sha256)
    rawmanifest=_read(manifest_path,manifest_sha256);obj=_read(object_path,object_sha256)
    record=_json_unique(rawrecord)
    _require(isinstance(record,dict) and set(record)=={'format','elf_sha256','map_sha256'} and
             type(record['format']) is int and record['format']==1 and record['elf_sha256']==link_sha256 and
             record['map_sha256']==map_sha256,'link provenance does not bind ELF and map')
    manifest_count=_manifest(rawmanifest,input_identity,object_sha256)
    records=_map_records(mapdata);meta,text,ro,widths=_linked_metadata(link)
    with tempfile.TemporaryDirectory(prefix='linked-literal-') as directory:
        p=Path(directory)/'owner.o';p.write_bytes(obj);p.chmod(0o400)
        snap=recovery_mapping.inspect(p);e=Elf(p)
        _require(hashlib.sha256(e.d).hexdigest()==object_sha256,'private object changed')
    funcs=[f for f in snap['functions'] if f['name']==literals.FUNCTION]
    _require(len(funcs)==1,'candidate function missing or ambiguous')
    func=funcs[0];code=snap['sections'][func['section']]
    _require(func['offset']==0 and func['size']==code['size'] and code['type']==1 and code['flags']&7==6,
             'unsupported candidate function contribution')
    row=_route(records,input_identity,func['section'],code['size'],code['alignment'])
    address=row['address'];size=func['size'];site=proof['object_function_offset']
    _require(text['address']<=address and address+size<=text['address']+text['size'], 'linked function outside executable text')
    start=_mapped(meta,address,size,5,2);_unique_allocated_extent(meta,text,address,size);_no_linked_write(meta,widths,address,address+size)
    symtabs=[s for s in meta['sections'] if s['type']==2]
    _require(len(symtabs)==1,'linked static symbol table missing or ambiguous')
    table_index=symtabs[0]['index']
    linked_symbols=[s for s in meta['symbols'] if s['table_section']==table_index]
    def linked_function(name, expected=None, global_only=False):
        found=[s for s in linked_symbols if s['name']==name and s['type']==2 and s['defined']]
        if expected is not None:found=[s for s in found if s['address']==expected and s['binding']==0]
        if global_only:found=[s for s in found if s['binding']==1 and s['visibility']==0]
        _require(len(found)==1 and found[0]['size']>0 and 0<found[0]['section_index']<len(meta['sections']), 'ambiguous linked function identity')
        symbol=found[0];section=meta['sections'][symbol['section_index']]
        _require(symbol['address']%4==0 and symbol['size']%4==0, 'unaligned linked function extent')
        _require(link[symtabs[0]['offset']+symbol['index']*24+5]==0, 'unsupported linked function visibility metadata')
        _require(section['type']==1 and section['flags']&7==6 and not section['flags']&0x400 and
                 section['address']<=symbol['address'] and symbol['address']+symbol['size']<=section['address']+section['size'], 'linked function extent differs')
        _mapped(meta,symbol['address'],symbol['size'],5,2)
        return symbol
    own=linked_function(literals.FUNCTION,address)
    _require(own['size']==size,'linked owner function size differs')
    aliases=[s for s in linked_symbols if s['type']==2 and s['defined'] and s is not own and
             (address<=s['address']<address+size if s['size']==0 else
              s['address']<address+size and address<s['address']+s['size'])]
    _require(not aliases,'overlapping linked function aliases')
    code_section=e.sections[code['index']];candidate=e.d[code_section[4]:code_section[4]+size]
    linked=link[start:start+size];_require(len(candidate)==len(linked)==size,'function bytes truncated')
    a,b=struct.unpack_from('<II',linked,site)
    _require(a&0x9f000000==0x90000000 and b&0xffc00000==0x91000000 and
             a&31!=31 and a&31==(b>>5)&31==b&31,'unsupported linked string address pair')
    imm=((a>>29)&3)|(((a>>5)&0x7ffff)<<2)
    if imm&(1<<20):imm-=1<<21
    target=((address+site)&~4095)+(imm<<12)+((b>>10)&4095)
    _require(ro['address']<=target and target+len(literals.LITERAL)<=ro['address']+ro['size'], 'linked string outside readonly storage')
    pos=_mapped(meta,target,len(literals.LITERAL),4,3);_unique_allocated_extent(meta,ro,target,len(literals.LITERAL));_no_linked_write(meta,widths,target,target+len(literals.LITERAL))
    _require(link[pos:pos+len(literals.LITERAL)]==literals.LITERAL,'linked string bytes differ')
    rels=[r for r in snap['relocations'] if r['section']==func['section'] and func['offset']<=r['offset']<func['offset']+size]
    _require(len(rels)>1 and len({r['offset'] for r in rels})==len(rels),'ambiguous function relocations')
    patched=bytearray(candidate);calls=[];pair=set()
    for r in rels:
        off=r['offset']-func['offset'];sym=r['symbol'];kind=r['type']
        _require(off%4==0 and off+4<=size,'function relocation extent differs')
        if off in (site,site+4):
            _require((off==site and kind==275 or off==site+4 and kind==277) and r['addend']==0 and
                     sym['section_index']==snap['sections'][proof['object_section']]['index'] and sym['type']==3 and
                     sym['binding']==0 and sym['other']==0 and sym['offset']==sym['size']==0,'string relocation identity differs')
            pair.add(off);continue
        _require(kind==283 and r['addend']==0,'unsupported non-string function relocation')
        word=struct.unpack_from('<I',candidate,off)[0]
        _require(word>>26==0b100101,'candidate call relocation is not BL')
        if sym['type']==3 and sym['binding']==0 and sym['other']==0 and sym['offset']==sym['size']==0:
            targets=[f for f in snap['functions'] if f['section_index']==sym['section_index'] and f['offset']==0]
            _require(len(targets)==1,'ambiguous same-owner call target')
            target_func=targets[0];target_sec=snap['sections'][target_func['section']]
            _require(target_func['size']==target_sec['size'], 'same-owner target extent differs')
            target_row=_route(records,input_identity,target_func['section'],target_sec['size'],target_sec['alignment'])
            call_target=target_row['address'];mapped_target=linked_function(target_func['name'],call_target)
            _require(mapped_target['size']==target_sec['size'], 'linked same-owner target size differs')
            target_name=target_func['name']
        else:
            _require(sym['section_index']==0 and sym['type'] in (0,2) and sym['binding']==1 and sym['other']==0 and
                     sym['offset']==sym['size']==0 and sym['name'],'unsupported named call identity')
            target_name=sym['name'];call_target=linked_function(target_name,global_only=True)['address']
        delta=call_target-(address+off)
        _require(delta%4==0 and -(1<<27)<=delta<(1<<27),'linked call target out of range')
        struct.pack_into('<I',patched,off,(word&0xfc000000)|((delta>>2)&0x3ffffff))
        calls.append({'offset':off,'symbol':target_name,'linked_address':call_target})
    _require(pair=={site,site+4},'missing paired string relocations')
    for step,actual in enumerate((a,b)):
        off=site+4*step;kind=(275,277)[step]
        raw=struct.unpack_from('<I',candidate,off)[0]
        _require((raw&~MASKS[kind])==(actual&~MASKS[kind]) and raw&MASKS[kind]==0, 'linked address instruction differs')
        struct.pack_into('<I',patched,off,actual)
    _require(bytes(patched)==linked,'linked function contains unproved instruction changes')
    return {'kind':'lua-lstate-linked-literal-diagnostic-v1','original_object_witness':proof,'object_sha256':object_sha256,
            'link_sha256':link_sha256,'map_sha256':map_sha256,'provenance_sha256':provenance_sha256,
            'input_manifest_sha256':manifest_sha256,'manifest_input_count':manifest_count,'input_identity':input_identity,
            'function_address':address,'function_size':size,'linked_consumer':address+site,'linked_literal_address':target,
            'literal_size':len(literals.LITERAL),'linked_calls':calls,'linked_full_function_reconstructed':True,
            'manifest_actual_link_inputs_authenticated':False,'source_ir_object_lineage_proven':False,
            'original_allocation_identity_proven':False,'all_merge_aliases_proven':False,'shared_consumers_proven':False,
            'receipt_acceptance':False}
