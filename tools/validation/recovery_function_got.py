"""Narrow recovery-only evidence for the three Radius function GOT entries.

Never rewrites checker records, proves provider behavior, or certifies exactness.
The only supported transition is the observed BehaviorInit diagnostic refinement.
"""
import hashlib
from pathlib import Path
import struct

import recovery_mapping
from data_provenance import verify_link_provenance
from . import common as c, strict
from .elf import Elf, MASKS
from .imports import _mapped
from .linked_attribution import _linked_metadata, _map_records, _route, _no_linked_write, _unique_allocated_extent
from .qualified_owner import _pinned_reader, _file_table, _file_symbols, _file_mapped_code
from .recovery_diagnostics import runtime_digest

ORIGINAL_SHA='9c246371f89c55c2f3bbacc69d0be2c1c60f6a070d66284ce5869171f88be90d'
NSS_SHA='dffc3f496a03cb1c10ef59db13c04779fb89d087fdf1050e87197f2284807a4f'
OWNER='X/Client/Behavior/RadiusTrigger.cpp'
CALLER='_Z12BehaviorInitP6RActor'
ITEMS=(('_Z25BehaviorRadiusTriggerInitP6RActor',0x19dc30,188,0xa4,0xac),
       ('_Z27BehaviorRadiusTriggerUpdateP6RActor',0x19dd20,792,0xa8,0xb0),
       ('_Z28BehaviorRadiusTriggerDisposeP6RActor',0x19dcf0,48,0xb8,0xbc))
BEFORE='mismatch at 0x19cc4c: unverified GOT reference: rebuilt undefined GOT target symbol'
AFTER='mismatch at 0x19cc4c: unverified GOT reference: original GOT target lacks known data identity'

def require(value,reason):
    if not value: raise ValueError(reason)

def digest(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest()

def one(items,reason):
    require(len(items)==1,reason)
    return items[0]

def slot(meta,data,base,a,b):
    def word(pc):return struct.unpack_from('<I',data,_mapped(meta,pc,4,5,2))[0]
    pc=base+a;x,y=word(pc),word(base+b)
    require(x&0x9f000000==0x90000000 and y&0xffc00000==0xf9400000 and x&31==(y>>5)&31 and x&31!=31,'GOT instruction shape')
    imm=((x>>29)&3)|(((x>>5)&0x7ffff)<<2)
    if imm&(1<<20):imm-=1<<21
    return (pc&~4095)+(imm<<12)+((y>>10)&4095)*8

def raw_symbols(meta,data,name,defined):
    syms=[s for s in meta['symbols'] if s['name']==name]
    require(bool(syms) and all(s['defined']==defined for s in syms),'conflicting or missing symbol definitions')
    for s in syms:
        table=meta['sections'][s['table_section']]
        require(table['entry_size']==24 and s['index']*24+24<=table['size'] and data[table['offset']+s['index']*24+5]==0,'unsupported raw symbol visibility/other')
    return syms

def unique_function(meta,name,size):
    rows=[x for x in meta['symbols'] if x['name']==name and x['defined']]
    keys={(x['address'],x['size'],x['type'],x['binding'],x['visibility'],x['section_index']) for x in rows}
    require(len(keys)==1,'missing/ambiguous function '+name)
    address,got_size,typ,bind,vis,sec=next(iter(keys))
    require((got_size,typ,bind,vis)==(size,2,1,0) and 0<sec<len(meta['sections']),'function metadata')
    section=meta['sections'][sec]
    require(section['flags']&4 and section['address']<=address and address+size<=section['address']+section['size'],'function extent')
    _unique_allocated_extent(meta,section,address,size)
    return address

def text_map(data, meta):
    """Index contributions under the authenticated linked .text output only."""
    sections=[s for s in meta['sections'] if s['name']=='.text']
    section=one(sections,'missing/ambiguous text section')
    current=None; headers=[]; lines=[]
    for line in data.decode('utf-8','strict').splitlines()[1:]:
        fields=line.split()
        if len(fields)!=4:continue
        if fields[3].startswith('.') and ':(' not in fields[3]:
            current=fields[3]
            if current=='.text':headers.append((int(fields[0],16),int(fields[1],16)))
        elif current=='.text' and ':(' in fields[3]:
            lines.append(line)
    require(headers==[(section['address'],section['size'])],'map text output differs')
    records=_map_records(('Address Size Align Out In Symbol\n'+'\n'.join(lines)).encode())
    for r in records:
        require(section['address']<=r['address'] and r['address']+r['size']<=section['address']+section['size'],'text contribution outside output')
    spans=sorted((r['address'],r['address']+r['size']) for r in records if r['size'])
    require(all(a[1]<=b[0] for a,b in zip(spans,spans[1:])),'overlapping text contributions')
    return records

def got(meta,address,kind,name,target=None):
    got_section=one([s for s in meta['sections'] if s['name']=='.got' and s['type']==1 and s['flags']&3==3 and s['address']<=address and address+8<=s['address']+s['size']],'GOT storage')
    require(address%8==0,'unaligned GOT slot')
    _mapped(meta,address,8,6,1)
    # Loader model bounds every relocation width; reject partial overlaps.
    widths={257:8,1025:8,1026:8,1027:8,1031:16}
    hits=[r for r in meta['relocations'] if r['offset']<address+8 and address<r['offset']+widths[r['type']]]
    r=one(hits,'GOT relocation ambiguity')
    require(r['offset']==address and r['type']==kind,'GOT relocation kind')
    if kind==1027:require(r['symbol_index']==0 and r['addend']==target,'relative function target')
    else:
        require(r['symbol_name']==name and r['addend']==0,'undefined GOT expression')
        sym=one([s for s in meta['symbols'] if s['table_section']==r['symbol_table'] and s['index']==r['symbol_index']],'dynamic GOT symbol')
        require(not sym['defined'] and (sym['type'],sym['binding'],sym['visibility'],sym['address'],sym['size'])==(0,1,0,0,0),'GOT import metadata')
    return r

class FunctionGOTEvidence:
    def __init__(self, roots, original, nss, targets, owner):
        self.roots=roots;self.original=Path(original);self.nss=Path(nss);self.targets=targets;self.owner=owner;self.witnesses=[]

    def equivalent(self,before,after):
        if (before.get('status'),after.get('status'),before.get('reason'),after.get('reason'))!=('mismatch','mismatch',BEFORE,AFTER):return False
        if (before.get('name'),before.get('address'))!=(CALLER,'0x000000710019cba0') or (after.get('name'),after.get('address'))!=(CALLER,'0x000000710019cba0'):return False
        try:
            witness=self._prove()
        except (ValueError,KeyError,IndexError,OSError,struct.error,c.Halt):
            return False
        self.witnesses.append({'kind':'radius-function-got-refinement-v1','before':before,'after':after,**witness})
        return True

    def _prove(self):
        require(self.owner==OWNER and set(self.roots)=={'baseline','candidate'},'owner/roots')
        rows={r['Name']:(int(r['Address'],16)&0xffffffff,int(r['Size'])) for r in self.targets}
        require(len(rows)==len(self.targets) and rows=={n:(a,s) for n,a,s,_,_ in ITEMS},'selected owner identities')
        original=self.original.read_bytes();require(hashlib.sha256(original).hexdigest()==ORIGINAL_SHA,'original hash')
        om,_,_,ow= _linked_metadata(original)
        _no_linked_write(om,ow,0x19cba0,0x19cba0+340)
        for _,address,size,_,_ in ITEMS:_no_linked_write(om,ow,address,address+size)
        with _pinned_reader(self.nss,320*1024**2,NSS_SHA) as nss:
            syms=_file_symbols(nss,_file_table(nss))
            for name,address,size in [(CALLER,0x19cba0,340)]+[(n,a,z) for n,a,z,_,_ in ITEMS]:
                row=one([x for x in syms if x[0]==name],'original function identity')
                require(row[1:4]==(2,1,0) and row[5:]==(address,size),'NSS function attributes')
                start=_mapped(om,address,size,5,2)
                require(_file_mapped_code(nss,address,size)==original[start:start+size],'NSS original code differs')
        roots={k:Path(v) for k,v in self.roots.items()}
        caller_rel='CMakeFiles/d3_game_obj.dir/src/X/Client/Behavior/Behavior.cpp.obj'
        owner_rel='CMakeFiles/d3_game_obj.dir/src/'+OWNER+'.obj'
        callers={k:v/'objects'/caller_rel for k,v in roots.items()};owner_obj=roots['candidate']/'objects'/owner_rel
        require(runtime_digest(callers['baseline'])==runtime_digest(callers['candidate']),'caller object changed')
        snapshots={k:recovery_mapping.inspect(v) for k,v in callers.items()}
        caller=one([f for f in snapshots['candidate']['functions'] if f['name']==CALLER],'caller function')
        require(caller['offset']==0 and caller['size']==340 and caller['binding']==1 and caller['other']==0,'caller object metadata')
        owner_snap=recovery_mapping.inspect(owner_obj)
        objcode,masks=Elf(callers['candidate']).functions()[CALLER]
        original_code=original[_mapped(om,0x19cba0,340,5,2):][:340]
        require(len(original_code)==len(objcode)==340 and all((int.from_bytes(objcode[i:i+4],'little')^int.from_bytes(original_code[i:i+4],'little')) & ~masks.get(i,0)==0 for i in range(0,340,4)),'original caller instructions')
        require(struct.unpack_from('<I',original_code,0xb4)[0]==0xa9012408 and struct.unpack_from('<I',original_code,0xc0)[0]==0xf9001008,'callback stores')
        for name,_,size,_,_ in ITEMS:
            f=one([f for f in owner_snap['functions'] if f['name']==name],'provider object function')
            require((f['offset'],f['size'],f['binding'],f['other'])==(0,size,1,0),'provider object metadata')
        images={};records={}
        for label,root in roots.items():
            elf=root/'build/2.6.2/d3';prov=verify_link_provenance(elf);data=elf.read_bytes();require(hashlib.sha256(data).hexdigest()==prov['elf_sha256'],'linked hash')
            meta,_,_,widths= _linked_metadata(data);records[label]=text_map(Path(str(elf)+'.map').read_bytes(),meta)
            raw_symbols(meta,data,CALLER,True)
            addr=unique_function(meta,CALLER,340)
            _no_linked_write(meta,widths,addr,addr+340)
            route=_route(records[label],'/work/build/2.6.2/'+caller_rel,caller['section'],340,snapshots[label]['sections'][caller['section']]['alignment'])
            require(route['address']==addr,'caller route')
            images[label]=(meta,data,addr,prov,widths)
            # All caller non-relocated instruction bits must be preserved.
            code,masks=Elf(callers[label]).functions()[CALLER];linked=data[_mapped(meta,addr,340,5,2):][:340]
            require(len(code)==len(linked)==340 and all((int.from_bytes(code[i:i+4],'little')^int.from_bytes(linked[i:i+4],'little')) & ~masks.get(i,0)==0 for i in range(0,340,4)),'caller linked instructions')
            caller_rels=[r for r in snapshots[label]['relocations'] if r['section']==caller['section']]
            require(all(r['type'] in MASKS and r['offset']%4==0 and r['offset']+4<=340 for r in caller_rels) and len({r['offset'] for r in caller_rels})==len(caller_rels),'unmodeled or overlapping caller relocation')
            require(not any(r['offset'] in (0xb4,0xc0) for r in caller_rels) and struct.unpack_from('<I',code,0xb4)[0]==struct.unpack_from('<I',linked,0xb4)[0]==0xa9012408 and struct.unpack_from('<I',code,0xc0)[0]==struct.unpack_from('<I',linked,0xc0)[0]==0xf9001008,'callback stores modified')
        result=[]
        for name,address,size,a,b in ITEMS:
            for label in roots:
                rels=[r for r in snapshots[label]['relocations'] if r['section']==caller['section'] and r['symbol']['name']==name]
                require(len(rels)==2 and {(x['offset'],x['type'],x['addend']) for x in rels}=={(a,311,0),(b,312,0)},'object GOT pair')
            original_slot=slot(om,original,0x19cba0,a,b);got(om,original_slot,1027,name,address)
            proof={'name':name,'original_address':address,'original_slot':original_slot,'modes':{}}
            for label in roots:
                meta,data,base,prov,widths=images[label];where=slot(meta,data,base,a,b)
                target=None
                raw_symbols(meta,data,name,label=='candidate')
                if label=='candidate':
                    target=unique_function(meta,name,size)
                    _no_linked_write(meta,widths,target,target+size)
                    f=one([f for f in owner_snap['functions'] if f['name']==name],'provider object')
                    route=_route(records[label],'/work/build/2.6.2/'+owner_rel,f['section'],size,owner_snap['sections'][f['section']]['alignment'])
                    require(route['address']==target,'provider map route')
                    provider_code,provider_masks=Elf(owner_obj).functions()[name]
                    provider_link=data[_mapped(meta,target,size,5,2):][:size]
                    rels=[r for r in owner_snap['relocations'] if r['section']==f['section']]
                    require(len(provider_code)==len(provider_link)==size and all(r['type'] in MASKS and r['offset']%4==0 and r['offset']+4<=size for r in rels) and len({r['offset'] for r in rels})==len(rels),'provider relocation shape')
                    require(all((int.from_bytes(provider_code[i:i+4],'little')^int.from_bytes(provider_link[i:i+4],'little')) & ~provider_masks.get(i,0)==0 for i in range(0,size,4)),'provider linked instructions')
                r=got(meta,where,1027 if label=='candidate' else 1025,name,target)
                proof['modes'][label]={'slot':where,'target':target,'relocation':r,'elf_sha256':prov['elf_sha256'],'map_sha256':prov['map_sha256']}
            result.append(proof)
        return {'caller_object':caller_rel,'caller_runtime_sha256':runtime_digest(callers['baseline']),'owner_object':owner_rel,'owner_object_sha256':digest(owner_obj),'original_sha256':ORIGINAL_SHA,'nss_sha256':NSS_SHA,'callbacks':result,'accepted':False,'strict_certified':False}
