"""Pinned original/object diagnostic for four Lua AArch64 switch tables.

This recognizes only the observed four compiler patterns. It proves bounded
object expressions and original dispatch bytes; it is not linked data proof,
a claim about original allocation ownership, or an admission decision.
"""
import hashlib
from pathlib import Path
import struct
import tempfile

from data_elf import _inspect
import recovery_mapping
from . import strict
from .elf import Elf, MASKS
from .imports import _mapped, _require, _section
from .literals import _read

OWNER='Contrib/Contrib/lua/src/lgc.c'
# Nine instructions preceding ADRP: load/index normalization, guarded compare,
# spill, then b.hi. Branch displacement is decoded and checked separately.
PATTERNS={
 'singlestep':([0xf85e83a0,0x39408408,0x2a0803e0,0x2a0003e8,0x71001108,0xf9001be1,0xf90017e0,0xb90027e8],4,0,0xf94017e9),
 'reallymarkobject':([0x39402008,0x71001108,0x2a0803e0,0x71001908,0xf90017e2,0xf90013e3,0xf9000fe0,0xb90017e8],3,4,0xf9400fe9),
 'freeobj':([0x39402008,0x71001108,0x2a0803e0,0x71001908,0xf81e83a2,0xf90013e3,0xf9000fe0,0xb90017e8],3,4,0xf9400fe9),
 'propagatemark':([0xf85e83a0,0x39402008,0x71001508,0x2a0803e0,0x71001108,0xf90013e1,0xf9000fe0,0xb90017e8],4,5,0xf9400fe9),
}


def _original_metadata(data):
    meta=_inspect(data,'<pinned-original>')
    text=_section(meta,'.text',1,6);ro=_section(meta,'.rodata',1,2)
    dyn=_section(meta,'.dynamic',6,3);rd=_section(meta,'.rela.dyn',4,2);rp=_section(meta,'.rela.plt',4,2)
    syms=_section(meta,'.dynsym',11,2);strings=_section(meta,'.dynstr',3,2)
    _require(rd['link']==rp['link']==syms['index'] and syms['link']==dyn['link']==strings['index'] and syms['entry_size']==24,'dynamic symbol links differ')
    for sec,flags,forbid in ((text,5,2),(ro,4,3),(dyn,6,1),(rd,4,3),(rp,4,3),(syms,4,3),(strings,4,3)):
        _require(_mapped(meta,sec['address'],sec['size'],flags,forbid)==sec['offset'],'original section mapping differs')
    dynsegs=[s for s in meta['segments'] if s['type']==2]
    _require(len(dynsegs)==1 and (dynsegs[0]['address'],dynsegs[0]['offset'],dynsegs[0]['file_size'],dynsegs[0]['memory_size']) ==
             (dyn['address'],dyn['offset'],dyn['size'],dyn['size']), 'dynamic segment differs')
    tags=meta['dynamic_tags'];allowed={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,20,21,23,25,27,0x6ffffef5,0x6ffffff9}
    _require(tags and tags[-1]['tag']==0 and all(t['section_index']==dyn['index'] and t['tag'] in allowed for t in tags),'unsupported dynamic tags')
    for number,value in ((2,rp['size']),(23,rp['address']),(20,7),(7,rd['address']),(8,rd['size']),(9,24),
                         (6,syms['address']),(11,24),(5,strings['address']),(10,strings['size'])):
        _require([t['value'] for t in tags if t['tag']==number]==[value],'dynamic relocation binding differs')
    counts=[t['value'] for t in tags if t['tag']==0x6ffffff9]
    _require(len(counts)<=1 and (not counts or counts[0]<=rd['size']//24),'relative count differs')
    if counts:
        for i in range(counts[0]):
            _require(struct.unpack_from('<Q',data,rd['offset']+i*24+8)[0]==1027,'relative prefix differs')
    for r in meta['relocations']:
        _require(r['section_index'] in (rd['index'],rp['index']) and r['type'] in (257,1025,1026,1027),'unsupported original relocation')
    return meta,text,ro


def _no_original_write(meta,start,end):
    _require(not any(r['offset']<end and start<r['offset']+8 for r in meta['relocations']),'original relocation overlaps proof range')


def prove_lgc_tables(original_path, original_sha256, object_path, object_sha256,
                     owner_path, owner_sha256, ir_path, ir_sha256, owner_context):
    """Prove observed original/object dispatch and PREL32 expressions, not linking."""
    _require(isinstance(owner_context,strict._OwnerContext) and owner_context.owner==OWNER and
             owner_context.original_sha256==original_sha256 and
             Path(original_path).resolve()==(Path(owner_context.root)/'data/2.6.2/main.elf').resolve() and
             strict.build_owner_context(owner_context.root,owner_context.owner,dict(owner_context.pins))==owner_context,
             'owner context missing or changed')
    original=_read(original_path,original_sha256);obj=_read(object_path,object_sha256)
    _read(owner_path,owner_sha256);_read(ir_path,ir_sha256)
    meta,text,ro=_original_metadata(original)
    with tempfile.TemporaryDirectory(prefix='table-witness-') as directory:
        p=Path(directory)/'owner.o';p.write_bytes(obj);p.chmod(0o400)
        snapshot=recovery_mapping.inspect(p);e=Elf(p);strict._object_layout(e)
        _require(hashlib.sha256(e.d).hexdigest()==object_sha256,'private object changed')
    expected_ro={'.rodata.'+name for name in PATTERNS}
    actual_ro={name for name in snapshot['sections'] if name=='.rodata' or name.startswith('.rodata.')}
    _require(actual_ro==expected_ro,'extra or missing object rodata section')
    allocated_data={name for name,section in snapshot['sections'].items() if section['flags']&2 and not section['flags']&4}
    _require(allocated_data==expected_ro|{'.eh_frame'},'extra or missing allocated data section')
    widths={257:8,258:4,259:2,260:8,261:4,262:2,**{k:4 for k in MASKS}}
    tables=[]
    for name,(prefix,cmp_index,index_base,load) in PATTERNS.items():
        members=[m for m in owner_context.members if m[0]==name]
        funcs=[f for f in snapshot['functions'] if f['name']==name]
        _require(len(members)==len(funcs)==1,'missing or ambiguous owner function')
        f=funcs[0];base=members[0][1]&0xffffffff;size=members[0][2]
        _require(f['size']==size and size>0 and size%4==0 and f['offset']%4==0 and f['binding']==0 and f['other']==0,'owner function size or alignment differs')
        code=snapshot['sections'][f['section']]
        _require(code['type']==1 and code['flags']&7==6 and f['offset']+size<=code['size'],'candidate function extent differs')
        _require(not any(g is not f and g['section']==f['section'] and g['offset']<f['offset']+size and f['offset']<g['offset']+g['size'] for g in snapshot['functions']),'overlapping function aliases')
        tabname='.rodata.'+name;tab=snapshot['sections'][tabname]
        _require(tab['type']==1 and tab['flags']==2 and tab['alignment']==4 and tab['entry_size']==0,'unsupported table section')
        incoming=[r for r in snapshot['relocations'] if r['symbol']['section_index']==tab['index']]
        _require(len(incoming)==2,'ambiguous table consumers')
        incoming.sort(key=lambda r:r['offset']);first,second=incoming;symbol=first['symbol']
        _require(first['section']==second['section']==f['section'] and first['type']==275 and second['type']==277 and
                 first['offset']%4==0 and second['offset']==first['offset']+4 and first['addend']==second['addend']==0 and
                 symbol==second['symbol'] and symbol['type']==3 and symbol['binding']==0 and symbol['other']==0 and
                 symbol['offset']==0 and symbol['size']==0,'unsupported table consumer pair')
        relative=first['offset']-f['offset'];_require(relative>=36 and relative+24<=size,'dispatch outside function')
        start=first['offset']-36;end=first['offset']+24
        for r in snapshot['relocations']:
            if r['section']!=f['section']:continue
            width=widths.get(r['type']);_require(width is not None and r['offset']+width<=code['size'],'unknown or invalid executable relocation')
            if name=='freeobj':
                _require(not (r['offset']<f['offset']+32 and f['offset']<r['offset']+width),'relocation overlaps freeobj prologue')
            if r is not first and r is not second:
                _require(not (r['offset']<end and start<r['offset']+width),'relocation overlaps dispatch')
        raw=e.sections[code['index']];candidate=list(struct.unpack_from('<15I',e.d,raw[4]+start))
        _require(candidate[:8]==prefix and candidate[8]&0xff00001f==0x54000008 and
                 candidate[9]==0x90000008 and candidate[10]==0x91000108 and
                 candidate[11:]==[load,0xb8a9790a,0x8b080148,0xd61f0100], 'unsupported candidate dispatch/register flow')
        count=((candidate[cmp_index]>>10)&0xfff)+1
        _require(1<=count<=256 and tab['size']==count*4,'table extent does not match dispatch bound')
        branch=candidate[8];delta=(branch>>5)&0x7ffff
        if delta & (1<<18):delta-=1<<19
        default_relative=relative-4+delta*4
        _require(0<=default_relative<size and default_relative%4==0 and not relative-36<=default_relative<relative+24,'default target outside function or into guard')
        if name=='freeobj':
            # X29=SP+0x40; STUR X2,[X29,#-0x18] writes SP+0x28,
            # disjoint from the indexed spill at SP+0x18.
            prologue=[0xd10143ff,0xa9047bfd,0x910103fd,0xaa0103e2,0xaa0003e3,0xf81f83a0,0xf81f03a1,0xf85f03a0]
            _require(relative==0x44 and list(struct.unpack_from('<8I',e.d,raw[4]+f['offset']))==prologue, 'freeobj frame/register flow differs')
        pc=base+relative;window=pc-36
        _require(text['address']<=window and window+60<=text['address']+text['size'],'original dispatch outside text')
        original_words=list(struct.unpack_from('<15I',original,_mapped(meta,window,60,5,2)))
        if name=='freeobj':
            _require(list(struct.unpack_from('<8I',original,_mapped(meta,base,32,5,2)))==prologue, 'original freeobj frame differs')
            _no_original_write(meta,base,base+32)
        for i in range(15):
            mask=MASKS[275] if i==9 else MASKS[277] if i==10 else 0
            _require((candidate[i]&~mask)==(original_words[i]&~mask),'original dispatch instructions differ')
        oa,ob=original_words[9:11];imm=((oa>>29)&3)|(((oa>>5)&0x7ffff)<<2)
        if imm&(1<<20):imm-=1<<21
        address=(pc&~4095)+(imm<<12)+((ob>>10)&4095)
        _require(address%4==0 and ro['address']<=address and address+tab['size']<=ro['address']+ro['size'],'original table outside readonly section')
        _require(not any(s['index']!=ro['index'] and s['flags']&2 and s['address']<address+tab['size'] and address<s['address']+s['size'] for s in meta['sections']), 'original table overlaps another allocated section')
        original_offset=_mapped(meta,address,tab['size'],4,3);_no_original_write(meta,window,window+60);_no_original_write(meta,address,address+tab['size'])
        rawtable=e.sections[tab['index']];_require(e.d[rawtable[4]:rawtable[4]+rawtable[5]]==bytes(tab['size']),'object table payload is not zero')
        rels=[r for r in snapshot['relocations'] if r['section']==tabname]
        _require(len(rels)==count and sorted(r['offset'] for r in rels)==list(range(0,count*4,4)),'incomplete or overlapping table writes')
        slots=[];targets=set()
        for r in sorted(rels,key=lambda x:x['offset']):
            sym=r['symbol'];off=r['offset']
            _require(r['type']==261 and sym['type']==3 and sym['binding']==0 and sym['other']==0 and
                     sym['section_index']==code['index'] and sym['offset']==0 and sym['size']==0,'unsupported table target expression')
            target_offset=r['addend']-off-f['offset']
            signed=struct.unpack_from('<i',original,original_offset+off)[0]
            target=address+signed
            candidate_signed=base-f['offset']+r['addend']-(address+off)
            _require(-(1<<31)<=candidate_signed<(1<<31) and candidate_signed==signed and
                     0<target_offset<size and target_offset%4==0 and not relative-36<=target_offset<relative+24 and target==base+target_offset and target_offset not in targets,
                     'table target differs, ambiguous or outside function')
            targets.add(target_offset)
            slots.append({'offset':off,'original_slot_address':address+off,'type':261,'addend':r['addend'],
                          'original_signed':signed,'candidate_signed_in_original_layout':candidate_signed,
                          'target_offset':target_offset,'original_target':target})
        tables.append({'function':name,'original_function_address':base,'original_function_size':size,
                       'object_section':tabname,'object_section_index':tab['index'],'object_table_addend':0,
                       'object_function_section_index':code['index'],'object_function_start':f['offset'],
                       'object_function_offset':relative,'original_consumer':pc,'original_address':address,'extent_bytes':tab['size'],
                       'original_instructions':original_words[9:11],'object_instructions':candidate[9:11],
                       'dispatch':{'index_base':index_base,'bound_inclusive':count-1,'entry_count':count,
                                   'default_function_offset':default_relative,'original_words':original_words,'object_words':candidate,
                                   'scope':'conditional on entry through the recognized guard','global_control_flow_proven':False},'slots':slots})
    ranges=sorted((t['original_address'],t['original_address']+t['extent_bytes']) for t in tables)
    _require(all(a[1]<=b[0] for a,b in zip(ranges,ranges[1:])), 'original table extents overlap')
    return {'kind':'lua-lgc-switch-original-object-v1','owner':OWNER,'original_sha256':original_sha256,'object_sha256':object_sha256,
            'owner_sha256':owner_sha256,'ir_sha256':ir_sha256,'original_inventory_pins':dict(owner_context.pins),'tables':tables,
            'source_ir_object_lineage_proven':False,'original_allocation_identity_proven':False,'candidate_linked_data_proven':False,
            'shared_consumers_proven':False,'receipt_acceptance':False}
