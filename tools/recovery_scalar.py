"""Fail-closed evidence for a single Radius scalar in an existing MERGE4 pool.

This is recovery-only evidence under the authenticated compiler/linker relocation
premise. It does not prove global control flow, allocation identity, runtime
bindings, or exact completion. Callers must authenticate receipt roots and the
original/NSS pins before invoking it and must repeat this check on landing.
"""
from collections import defaultdict
import csv
import hashlib
import os
from pathlib import Path
import re
import struct

from data_provenance import verify_link_provenance
from data_relocations import RelocationImage
from validation import common as c, strict
from validation.elf import Elf, MASKS
from validation.imports import _mapped, _section, _covered
from verify_original_inputs import fingerprint

OWNER='X/Client/Behavior/RadiusTrigger.cpp'
FUNCTION='_Z27BehaviorRadiusTriggerUpdateP6RActor'
VALUE=bytes.fromhex('8988883c')


def require(ok, message):
    if not ok: raise c.Halt('Recovery scalar: '+message)


def _names(e):
    strict._object_layout(e)
    index=struct.unpack_from('<H',e.d,0x3e)[0];s=e.sections[index]
    require(s[1]==3,'section name table')
    data=e.d[s[4]:s[4]+s[5]]
    def get(i):
        require(i<len(data),'section name offset');end=data.find(b'\0',i)
        require(end>=0,'unterminated section name')
        return data[i:end].decode('utf8','strict')
    return [get(s[0]) for s in e.sections]


def _object(path):
    e=Elf(path);names=_names(e);syms=strict.symbols(e);rels=strict.relocs(e,syms)
    pools={}
    for i,s in enumerate(e.sections):
        if s[2]&18==18 and not s[2]&0x20 and s[9]==4:
            require(s[1]==1 and s[2]==18 and s[5]%4==0 and s[8] in (1,4),'invalid MERGE4 extent')
            values=[e.d[s[4]+j:s[4]+j+4] for j in range(0,s[5],4)]
            pools[i]={'index':i,'name':names[i],'size':s[5],'values':values,'aliases':[],'references':[]}
    # strict.symbols exposes every symbol in the single validated symbol table.
    symtab=next(s for s in e.sections if s[1]==2)
    for index,symbol in syms.items():
        if symbol['section'] in pools and re.fullmatch(r'\$d(?:\.[0-9]+)?',symbol['name']):
            _,info,other,_,value,size=struct.unpack_from('<IBBHQQ',e.d,symtab[4]+index*24)
            require(info==0 and other==0 and size==0 and value<pools[symbol['section']]['size'],'invalid pool mapping marker')
        elif symbol['section'] in pools and symbol['type'] not in (3,4):
            require(bool(symbol['name']),'unnamed scalar alias')
            _,info,other,_,_,_=struct.unpack_from('<IBBHQQ',e.d,symtab[4]+index*24)
            pools[symbol['section']]['aliases'].append(dict(symbol,binding=info>>4,visibility=other))
    symbol_ids={id(symbol):i for i,symbol in syms.items()}
    for rel in rels:
        require(rel['section'] not in pools,'relocation writes to scalar input')
        sym=rel['symbol'];idx=sym['section']
        if idx not in pools:continue
        require(id(sym) in symbol_ids,'ambiguous scalar relocation symbol')
        _,info,other,shndx,value,size=struct.unpack_from('<IBBHQQ',e.d,symtab[4]+symbol_ids[id(sym)]*24)
        require((info,other,shndx,value,size)==(3,0,idx,0,0),'unsupported scalar relocation symbol')
        p=pools[idx];off=sym['value']+rel['addend']
        require(0<=off<p['size'] and off%4==0,'interior/end/out-of-range scalar reference')
        p['references'].append({'section':rel['section'],'source':names[rel['section']],
            'offset':rel['offset'],'type':rel['type'],'symbol':sym,'addend':rel['addend'],
            'target_offset':off,'value':p['values'][off//4]})
    return e,names,pools


def _maps(path):
    provenance=verify_link_provenance(path);map_path=Path(str(path)+'.map')
    pin=fingerprint(map_path);data=map_path.read_bytes()
    require(hashlib.sha256(data).hexdigest()==pin['sha256']==provenance['map_sha256'] and
            fingerprint(map_path)==pin,'map changed while reading')
    return _Map(data.decode('utf8','strict').splitlines())


class _Map:
    """Index authenticated text output and contributions once per link."""
    def __init__(self, lines):
        self.raw=tuple(lines);self.text=[];self.contributions=defaultdict(list);ranges=[];current=None
        for line in lines:
            fields=line.split()
            if len(fields)!=4:continue
            if fields[3].startswith('.') and ':(' not in fields[3]:
                current=fields[3]
                if current=='.text':self.text.append((int(fields[0],16),int(fields[1],16)))
            elif current=='.text' and ':(' in fields[3]:
                at,length=int(fields[0],16),int(fields[1],16)
                require(fields[2].isdigit(),'malformed contribution alignment')
                self.contributions[fields[3]].append((at,length,int(fields[2])))
                if length:ranges.append((at,at+length))
        ranges.sort()
        require(len(self.text)==1 and all(a[1]<=b[0] for a,b in zip(ranges,ranges[1:])),
                'ambiguous or overlapping text contributions')


def _contribution(lines, obj, sec, alignment):
    expected='/work/build/2.6.2/'+obj+':('+sec+')'
    if not isinstance(lines,_Map):lines=_Map(lines)
    matches=lines.contributions.get(expected,[])
    require(len(matches)==1,'missing/ambiguous map contribution: '+expected)
    start,size,actual_alignment=matches[0];require(actual_alignment==alignment,'contribution alignment changed')
    require(lines.text[0][0]<=start and start+size<=sum(lines.text[0]),'contribution outside text output')
    return start,size


def _pool(lines,image):
    # Pool metadata is read from the separately authenticated raw map below.
    if isinstance(lines,_Map):lines=lines.raw
    matches=[]
    for line in lines:
        f=line.split()
        if len(f)==4 and f[2]=='4' and f[3]=='<internal>:(.rodata)':
            try:matches.append((int(f[0],16),int(f[1],16)))
            except ValueError as exc:raise c.Halt('Recovery scalar: malformed merge pool') from exc
    # A single complete linker-synthesized MERGE4 extent is supported.
    require(len(matches)==1,'missing/ambiguous merged scalar pool')
    start,size=matches[0];require(size and size%4==0,'invalid pool size')
    off=_mapped(image.metadata,start,size,4,3);raw=image.blob[off:off+size]
    require(image.object_bytes(start,size)==(raw,'file'),'ambiguous allocated pool extent')
    values={}
    for i in range(0,size,4):values.setdefault(raw[i:i+4],[]).append(start+i)
    require(all(len(x)==1 for x in values.values()),'ambiguous merged scalar values')
    return start,size,{k:v[0] for k,v in values.items()},hashlib.sha256(raw).hexdigest()


def _page(pc,w):
    require(w&0x9f000000==0x90000000 and w&31!=31,'not ADRP')
    n=((w>>29)&3)|(((w>>5)&0x7ffff)<<2)
    if n&(1<<20):n-=1<<21
    return (pc&~4095)+(n<<12),w&31


def _load(w):
    require(w&0xffc00000==0xbd400000,'not LDR S')
    return ((w>>10)&4095)*4,(w>>5)&31


def _add(w):
    require(w&0xffc00000==0x91000000 and (w&31)!=31 and (w&31)==(w>>5)&31,'not a same-register ADD')
    return (w>>10)&4095,(w>>5)&31


def _pool_ownership(before,after,inputs):
    require(VALUE in inputs and set(before)==set(after)==set(inputs),'linked pool has unowned or omitted values')


def _debug_reference(source,flags,before_address,after_address):
    require(source.startswith('.debug') and not flags&2 and before_address==after_address,
            'unsupported absolute scalar reference')


def _relocated_reference(images,starts,high,low,kind,expected):
    observed=[]
    for i in range(2):
        pc=starts[i]+high;page,reg=_page(pc,_u32(images[i],pc));word=_u32(images[i],starts[i]+low)
        delta,base=(_load(word) if kind==285 else _add(word))
        require(reg==base and page+delta==expected[i],'linked scalar reference differs')
        observed.append(page+delta)
    return observed


def _u32(image,pc):
    return struct.unpack_from('<I',image.blob,_mapped(image.metadata,pc,4,5,2))[0]


def _loader(image,start,size):
    m=image.metadata;require(not image.unmodeled_relocations,'unmodeled dynamic relocation format')
    ro=_section(m,'.rodata',1,2);_covered(m,ro,4,3)
    require(ro['address']<=start and start+size<=ro['address']+ro['size'],'pool outside rodata')
    dyn=_section(m,'.dynamic',6,3);syms=_section(m,'.dynsym',11,2);strings=_section(m,'.dynstr',3,2)
    rd=_section(m,'.rela.dyn',4,2);rp=_section(m,'.rela.plt',4,2);plt=_section(m,'.plt',1,6);got=_section(m,'.got.plt',1,3)
    require(dyn['link']==strings['index'] and rd['link']==rp['link']==syms['index'] and syms['link']==strings['index'] and rp['info']==0 and rd['entry_size']==rp['entry_size']==24 and syms['entry_size']==24 and dyn['entry_size']==16,'dynamic section linkage')
    for sec,flags,forbid in ((dyn,6,1),(syms,4,3),(strings,4,3),(rd,4,3),(rp,4,3),(plt,5,2),(got,6,1)):_covered(m,sec,flags,forbid)
    segments=[x for x in m['segments'] if x['type']==2]
    require(len(segments)==1 and all(segments[0][a]==dyn[b] for a,b in (('address','address'),('offset','offset'),('file_size','size'),('memory_size','size'))),'dynamic segment')
    tags=m['dynamic_tags'];allowed={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,20,21,23,25,27,30,0x6ffffef5,0x6ffffff9}
    require(tags and tags[-1]['tag']==0 and all(t['section_index']==dyn['index'] and t['tag'] in allowed for t in tags),'dynamic tags')
    def tag(n):
        v=[t['value'] for t in tags if t['tag']==n];require(len(v)==1,'dynamic tag cardinality');return v[0]
    for n,v in ((3,got['address']),(2,rp['size']),(23,rp['address']),(20,7),(7,rd['address']),(8,rd['size']),(9,24),(6,syms['address']),(11,24),(5,strings['address']),(10,strings['size'])):require(tag(n)==v,'dynamic relocation binding')
    require(tag(30)==2,'unsupported dynamic flags')
    num=tag(0x6ffffff9);require(num<=rd['size']//24,'relative relocation count')
    for i in range(num):require(struct.unpack_from('<Q',image.blob,rd['offset']+i*24+8)[0]==1027,'relative relocation prefix')
    require({s['index'] for s in m['sections'] if s['type'] in (4,9,19)}=={rd['index'],rp['index']},'unmodeled relocation section')
    require(not image.object_relocations(start,size),'pool may receive loader relocation')


def _word_masks(e,index,size):
    syms=strict.symbols(e);rels=strict.relocs(e,syms);masks={}
    for r in rels:
        if r['section']!=index:continue
        off=r['offset'];kind=r['type']
        require(kind in MASKS and off%4==0 and off+4<=size and off not in masks,'unsupported code relocation')
        masks[off]=MASKS[kind]
    return masks


def _code(e,index,obj,section,images,lines):
    s=e.sections[index];size=s[5]
    require(s[1]==1 and s[2]&7==6 and size%4==0,'unsupported consumer section')
    masks=_word_masks(e,index,size);raw=e.d[s[4]:s[4]+size];positions=[]
    for image,line in zip(images,lines):
        start,mapped_size=_contribution(line,obj,section,s[8]);require(mapped_size==size,'consumer extent changed')
        text=_section(image.metadata,'.text',1,6)
        if not isinstance(line,_Map):line=_Map(line)
        require(line.text==[(text['address'],text['size'])],
                'map text output differs from ELF')
        off=_mapped(image.metadata,start,size,5,2);linked=image.blob[off:off+size]
        require(image.object_bytes(start,size)==(linked,'file'),'ambiguous allocated consumer extent')
        require(not image.unmodeled_relocations and not image.object_relocations(start,size),'consumer may receive loader relocation')
        for pos in range(0,size,4):
            a=struct.unpack_from('<I',raw,pos)[0];b=struct.unpack_from('<I',linked,pos)[0]
            require(not ((a^b)&~masks.get(pos,0)),'consumer nonrelocation instruction differs')
        positions.append(start)
    return positions


def _nss_value(path,digest,address,value,rows,original):
    from validation.qualified_owner import _pinned_reader,_file_table,_file_symbols,_file_mapped_code
    with _pinned_reader(path,512*1024*1024,digest) as reader:
        table=_file_table(reader);symbols=_file_symbols(reader,table)
        for row in rows:
            name=row['Name'];pc=int(row['Address'],16)&0xffffffff;size=int(row['Size'])
            hits=[x for x in symbols if x[0]==name and x[1]==2]
            require(len(hits)==1 and hits[0][2:4]==(1,0) and hits[0][5:]==(pc,size),'original NSS function identity differs')
            section=hits[0][4]
            require(0<section<len(table) and table[section][1]==1 and table[section][2]&7==6 and
                    table[section][3]<=pc and pc+size<=table[section][3]+table[section][5],
                    'original NSS symbol section differs')
            actual=_file_mapped_code(reader,pc,size);fo=_mapped(original.metadata,pc,size,5,2)
            require(actual==original.blob[fo:fo+size],'original/NSS function bytes differ')
            require(not original.object_relocations(pc,size),'original function may receive relocation')
        h=reader.read(0,64);strict._elf_header(h,3)
        off=struct.unpack_from('<Q',h,0x20)[0];ent,num=struct.unpack_from('<HH',h,0x36)
        require(ent==56 and 0<num<=4096,'NSS program table')
        raw=reader.read(off,ent*num);hits=[]
        for i in range(num):
            typ,flags,fo,va,_,fs,ms,_=struct.unpack_from('<IIQQQQQQ',raw,i*ent)
            if typ==1:
                require(fs<=ms and fo+fs<=reader.size,'invalid NSS segment')
                if va<address+len(value) and address<va+ms:hits.append((flags,fo,va,fs))
        require(len(hits)==1,'ambiguous NSS storage');flags,fo,va,fs=hits[0]
        require(flags&7==4 and va<=address and address+len(value)<=va+fs and reader.read(fo+address-va,len(value))==value,'NSS literal mismatch')


def _owner_proof(original,nss,owner_object,rows,linked,lines,owner_relative):
    expected={'_Z25BehaviorRadiusTriggerInitP6RActor','_Z28BehaviorRadiusTriggerDisposeP6RActor',FUNCTION}
    require(len(rows)==3 and {r['Name'] for r in rows}==expected and len({r['Address'] for r in rows})==3,'owner identity set')
    target=[r for r in rows if r['Name']==FUNCTION]
    require(len(target)==1 and int(target[0]['Size'])==792,'missing Radius Update identity')
    e,names,pools=_object(owner_object)
    all_symbols=strict.symbols(e);symtab=next(sec for sec in e.sections if sec[1]==2)
    for row in rows:
        matches=[(i,z) for i,z in all_symbols.items() if z['name']==row['Name']]
        require(len(matches)==1,'ambiguous owner function symbol')
        i,symbol=matches[0];_,info,other,sec,val,size=struct.unpack_from('<IBBHQQ',e.d,symtab[4]+i*24)
        require(info==0x12 and other==0 and 0<sec<len(e.sections) and size==int(row['Size']) and
                e.sections[sec][1]==1 and e.sections[sec][2]&7==6 and val+size<=e.sections[sec][5],
                'owner function symbol extent differs')
    require(len(pools)==1,'owner scalar pool count');idx,pool=next(iter(pools.items()));s=e.sections[idx]
    require(pool['name']=='.rodata.cst4' and s[2]==18 and s[5]==s[8]==s[9]==4 and s[6]==s[7]==0 and pool['values']==[VALUE] and not pool['aliases'],'owner pool shape')
    for group in e.sections:
        if group[1]==17:
            require(group[5]>=4 and group[5]%4==0,'invalid group extent')
            require(idx not in struct.unpack_from('<'+'I'*(group[5]//4),e.d,group[4])[1:],'owner pool in group')
    snapshot=__import__('recovery_mapping').inspect(owner_object)
    marks=[x for x in snapshot['markers'] if x['section']==pool['name']]
    require(len(marks)==1 and marks[0]['kind']=='d' and marks[0]['offset']==0,'owner scalar marker')
    refs=pool['references'];require(len(refs)==2,'owner scalar reference count')
    hi=[r for r in refs if r['type']==275];lo=[r for r in refs if r['type']==285]
    require(len(hi)==len(lo)==1,'owner scalar reference type');a,b=hi[0],lo[0]
    require(a['source']=='.text.'+FUNCTION and a['section']==b['section'] and b['offset']==a['offset']+4 and a['addend']==b['addend']==0 and a['symbol']==b['symbol'] and a['symbol']['type']==3 and a['symbol']['value']==0,'owner scalar reference pair')
    symtab=next(sec for sec in e.sections if sec[1]==2);symbols=strict.symbols(e)
    ids=[i for i,symbol in symbols.items() if symbol==a['symbol']]
    require(len(ids)==1,'ambiguous owner section symbol')
    _,info,other,shndx,value,size=struct.unpack_from('<IBBHQQ',e.d,symtab[4]+ids[0]*24)
    require((info,other,shndx,value,size)==(3,0,idx,0,0),'owner section symbol metadata differs')
    funcs=[x for x in strict.symbols(e).values() if x['name']==FUNCTION and x['type']==2]
    require(len(funcs)==1 and funcs[0]['section']==a['section'] and funcs[0]['value']==0 and funcs[0]['size']==792 and a['offset']+8<=792,'owner function extent')
    raw=e.sections[a['section']];cw=struct.unpack_from('<II',e.d,raw[4]+a['offset'])
    original_pc=(int(target[0]['Address'],16)&0xffffffff)+a['offset'];orig=RelocationImage(original,'original')
    ow=struct.unpack('<II',orig.blob[_mapped(orig.metadata,original_pc,8,5,2):][:8]);page,reg=_page(original_pc,ow[0]);off,r2=_load(ow[1])
    require(reg==r2 and (cw[0]&~MASKS[275])==(ow[0]&~MASKS[275]) and (cw[1]&~MASKS[285])==(ow[1]&~MASKS[285]),'original scalar consumer differs')
    addr=page+off;v=orig.object_bytes(addr,4);require(v==(VALUE,'file'),'original scalar value')
    off=_mapped(orig.metadata,addr,4,4,3);require(orig.blob[off:off+4]==VALUE,'original scalar is not immutable')
    require(not orig.unmodeled_relocations and not orig.object_relocations(addr,4),'original scalar relocation overlap')
    _nss_value(nss,fingerprint(nss)['sha256'],addr,VALUE,rows,orig)
    base,size=_contribution(lines,owner_relative,a['source'],raw[8]);require(size==792,'linked owner extent')
    require(not linked.unmodeled_relocations and not linked.object_relocations(base,size),'owner code may receive loader relocation')
    pc=base+a['offset'];lw=(_u32(linked,pc),_u32(linked,pc+4));lp,lr=_page(pc,lw[0]);ll,lr2=_load(lw[1])
    require(lr==lr2 and (cw[0]&~MASKS[275])==(lw[0]&~MASKS[275]) and (cw[1]&~MASKS[285])==(lw[1]&~MASKS[285]),'linked owner scalar consumer differs')
    return {'function':FUNCTION,'offset':a['offset'],'original_pc':original_pc,'original_address':addr,
            'original_words':list(ow),'linked_pc':pc,'linked_address':lp+ll,'linked_words':list(lw),'bytes':VALUE.hex()}


def prove(original, nss, before_receipt, after_receipt, owner_object, selected_rows, owner):
    """Recompute narrow scalar/read and preexisting MERGE4 consumer evidence.

    The caller must supply already-bound receipt roots, selected inventory, and
    original/NSS identities; this function rechecks the receipt seals itself.
    All unaffected objects must be byte-identical. No quality promotion follows.
    """
    require(owner==OWNER,'unsupported owner')
    import receipts
    before_receipt,after_receipt=Path(before_receipt),Path(after_receipt)
    before_mode=receipts.read_json(before_receipt/'done.json').get('mode')
    after_mode=receipts.read_json(after_receipt/'done.json').get('mode')
    require(before_mode in ('normal','lua') and before_mode==after_mode,'receipt modes differ')
    before_done=receipts.retained_mode(before_receipt,'recovery-only',before_mode)
    after_done=receipts.retained_mode(after_receipt,'recovery-only',after_mode)
    links=[before_receipt/'build/2.6.2/d3',after_receipt/'build/2.6.2/d3']
    lines=[_maps(x) for x in links];images=[RelocationImage(x) for x in links]
    pins={str(p):fingerprint(p) for p in (Path(original),Path(nss),Path(owner_object),*links)}
    old={str(p.relative_to(before_receipt/'objects')):p for p in (before_receipt/'objects').rglob('*.obj')}
    new={str(p.relative_to(after_receipt/'objects')):p for p in (after_receipt/'objects').rglob('*.obj')}
    for root,actual in ((before_receipt,old),(after_receipt,new)):
        recorded=receipts.read_json(root/'done.json')['files']
        sealed={k[len('objects/'):] for k in recorded if k.startswith('objects/') and k.endswith('.obj')}
        require(set(actual)==sealed,'receipt object inventory differs from seal')
    owner_matches=[name for name in new if name.endswith('/src/'+owner+'.obj')]
    require(len(owner_matches)==1 and Path(owner_object).resolve()==new[owner_matches[0]].resolve(),'owner object identity')
    owner_relative=owner_matches[0]
    require(owner_relative not in old and set(new)==set(old)|{owner_relative},'object membership changed')
    object_pins={};object_fingerprints={}
    for name,path in old.items():
        before=fingerprint(path);after=fingerprint(new[name]);require(before['sha256']==after['sha256'],'unrelated object differs: '+name)
        object_pins[name]=before['sha256']
        object_fingerprints[str(path)]=before;object_fingerprints[str(new[name])]=after
    ps=[_pool(l,i) for l,i in zip(lines,images)]
    require(set(ps[0][2])==set(ps[1][2]),'merged scalar value set changed')
    for p,img in zip(ps,images):_loader(img,p[0],p[1])
    owner_witness=_owner_proof(Path(original),Path(nss),Path(owner_object),selected_rows,images[1],lines[1],owner_relative)
    require(owner_witness['linked_address']==ps[1][2].get(VALUE),'owner linked scalar not in pool')
    oldrefs=[];aliases=[];sections=0;input_values=set()
    for name,path in sorted(old.items()):
        e,names,pools=_object(path)
        for idx,pool in pools.items():
            sections+=1
            input_values.update(pool['values'])
            require(all(v in ps[0][2] and v in ps[1][2] for v in pool['values']),'input scalar omitted from linked pool')
            for alias in pool['aliases']:
                v=alias['value'];size=alias['size']
                require(alias['type']==1 and alias['binding']==0 and alias['visibility']==0 and v%4==0 and 0<=v<pool['size'] and size==4 and v+size<=pool['size'],'unmodeled scalar alias extent')
                val=pool['values'][v//4];require(val in ps[0][2] and val in ps[1][2] and ps[0][2][val]==ps[1][2][val],'named scalar alias moved')
                for image in images:
                    found=[x for x in image.metadata['symbols'] if x['name']==alias['name'] and x['defined']]
                    require(len(found)==1 and found[0]['address']==ps[0][2][val] and found[0]['size']==size and found[0]['type']==alias['type'] and found[0]['binding']==alias['binding'] and found[0]['visibility']==alias['visibility'],'linked scalar alias changed')
                aliases.append({'object':name,'name':alias['name'],'address':ps[0][2][val],'value':val.hex()})
            grouped=defaultdict(list)
            for ref in pool['references']:
                val=ref['value'];require(val in ps[0][2] and val in ps[1][2],'missing linked pool value')
                typ=ref['type']
                if typ==257:
                    _debug_reference(ref['source'],e.sections[ref['section']][2],ps[0][2][val],ps[1][2][val])
                    continue
                if typ==277:
                    require(ps[0][2][val]==ps[1][2][val],'address-taken scalar moved')
                require(typ in (275,277,285),'unsupported scalar relocation')
                grouped[(ref['section'],ref['source'])].append(ref)
            for (index,source),refs in grouped.items():
                starts=_code(e,index,name,source,images,lines)
                his=[r for r in refs if r['type']==275];los=[r for r in refs if r['type'] in (277,285)]
                for h in his:
                    pc=[s+h['offset'] for s in starts];hw=[_u32(im,at) for im,at in zip(images,pc)]
                    hp=[_page(at,w) for at,w in zip(pc,hw)];val=h['value']
                    require(hp[0][0]==hp[1][0] and all(hp[i][0]==ps[i][2][val]&~4095 for i in range(2)),'consumer ADRP page changed')
                for lo in los:
                    candidates=[h for h in his if h['symbol']==lo['symbol'] and h['addend']==lo['addend'] and h['offset']<lo['offset']]
                    require(candidates,'missing scalar ADRP producer')
                    h=max(candidates,key=lambda r:r['offset']);val=lo['value']
                    obs=_relocated_reference(images,starts,h['offset'],lo['offset'],lo['type'],
                                             [ps[i][2][val] for i in range(2)])
                    oldrefs.append({'object':name,'section':source,'high_offset':h['offset'],'low_offset':lo['offset'],
                                    'value':val.hex(),'before':obs[0],'after':obs[1]})
    require(oldrefs,'no prior scalar consumer evidence')
    _pool_ownership(ps[0][2],ps[1][2],input_values)
    # Bind all bytes again after the final parse, including both receipt seals.
    require(receipts.retained_mode(before_receipt,'recovery-only',before_mode)==before_done and receipts.retained_mode(after_receipt,'recovery-only',after_mode)==after_done,'receipt changed')
    for path,pin in pins.items():require(fingerprint(path)==pin,'input changed: '+path)
    for path,pin in object_fingerprints.items():require(fingerprint(path)==pin,'object changed: '+path)
    return {'kind':'radius-recovery-scalar-v1','owner':owner,'section':'.rodata.cst4',
      'original_sha256':pins[str(Path(original))]['sha256'],'nss_sha256':pins[str(Path(nss))]['sha256'],
      'owner_object_sha256':pins[str(Path(owner_object))]['sha256'],'owner_read':owner_witness,
      'before_receipt_sha256':before_done,'after_receipt_sha256':after_done,
      'pool_before':{'address':ps[0][0],'size':ps[0][1],'sha256':ps[0][3]},
      'pool_after':{'address':ps[1][0],'size':ps[1][1],'sha256':ps[1][3]},
      'unchanged_object_sha256':object_pins,'input_pool_sections':sections,'old_reads':oldrefs,'stationary_aliases':aliases,
      'compiler_and_linker_relocation_premise':True,'original_allocation_identity_proven':False,
      'global_control_flow_proven':False,'runtime_binding_proven':False,'exact_complete':False}
