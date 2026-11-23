"""Fail-closed evidence for narrow new named initialized writable storage.

A section descriptor is *pending*, not admission. `prove` must be rerun over
sealed receipt trees and trusted external pins before any ledger transition.
This initial form supports one complete global object and one 32-bit GOT read;
unknown consumers, relocations, aliases, or storage forms are rejected.
"""
import hashlib
import os
from pathlib import Path
import re
import selectors
import time
import struct
import subprocess

import recovery_mapping
import receipts
from data_identity import parse_dwarf_variables
from data_provenance import verify_link_provenance
from validation import common as c, strict
from validation.elf import Elf, MASKS
from validation.imports import _mapped
from validation.linked_attribution import _linked_metadata, _no_linked_write, _map_records, _route, _unique_allocated_extent
from validation.qualified_owner import _pinned_reader, _file_table, _file_symbols

MAX_OBJECT=4096
MAX_ELF=64*1024*1024
MAX_FUNCTION=1024*1024

def require(ok,why):
    if not ok:raise c.Halt('Named initialized data: '+why)

def sha(path):
    h=hashlib.sha256()
    with open(path,'rb') as f:
        for block in iter(lambda:f.read(1024*1024),b''):h.update(block)
    return h.hexdigest()

def one(values,why):
    require(len(values)==1,why)
    return values[0]

def named_section(snapshot,section,object_path):
    """Return a raw pending descriptor; never authorize a transition here."""
    require(Path(object_path).is_file() and not Path(object_path).is_symlink() and
            Path(object_path).stat().st_size<=MAX_ELF,'bounded regular owner object required')
    s=snapshot['sections'][section]
    require(not any(x[1]==19 for x in Elf(object_path).sections),'RELR is unsupported')
    require(s['type']==1 and s['flags']==3 and 0<s['size']<=MAX_OBJECT and
            s['alignment']>0 and s['alignment']&(s['alignment']-1)==0 and s['entry_size']==0,
            'unsupported writable section')
    require(not any(any(m['index']==s['index'] for m in g['members']) for g in snapshot['groups']),'group data unsupported')
    markers=[m for m in snapshot['markers'] if m['section']==section]
    require(not markers or len(markers)==1 and (markers[0]['kind'],markers[0]['offset'])==('d',0),'invalid data marker')
    marker_ids={m['symbol_index'] for m in markers}
    entries=[x for x in snapshot['symbols'] if x['section_index']==s['index'] and x['symbol_index'] not in marker_ids]
    symbols=[x for x in entries if x['type']!=3]
    symbol=one(symbols,'one named storage identity required')
    require(bool(re.fullmatch(r'[A-Za-z_][A-Za-z0-9_.$]*',symbol['name'])) and
            (symbol['type'],symbol['binding'],symbol['other'],symbol['offset'],symbol['size'])==(1,1,0,0,s['size']),'unsupported data identity/extent')
    require(sum(x['name']==symbol['name'] for x in snapshot['symbols'])==1,'duplicate data symbol')
    require(all(x['type']==3 and x['name'] in ('',section) and x['binding']==x['other']==x['offset']==x['size']==0 for x in entries if x is not symbol),'section symbol invalid')
    require(not any(r['section']==section for r in snapshot['relocations']),'writable initializer has relocations')
    e=Elf(object_path);raw=e.sections[s['index']];value=e.d[raw[4]:raw[4]+raw[5]]
    require(len(value)==s['size'] and hashlib.sha256(value).hexdigest()==s['sha256'],'object changed')
    incoming=[r for r in snapshot['relocations'] if snapshot['sections'][r['section']]['flags']&2 and
              (r['symbol']['name']==symbol['name'] or r['symbol']['section_index']==s['index'])]
    require(len(incoming)==2,'unsupported or incomplete allocated consumer set')
    a,b=sorted(incoming,key=lambda r:(r['section'],r['offset']))
    require(a['section']==b['section'] and a['offset']%4==0 and b['offset']==a['offset']+4 and
            (a['type'],b['type'],a['addend'],b['addend'])==(311,312,0,0) and
            a['symbol']==b['symbol']==symbol,'unsupported consumer relocation pair')
    source=snapshot['sections'][a['section']];require(source['type']==1 and source['flags']&4 and b['offset']+8<=source['size'],'consumer outside code')
    f=one([x for x in snapshot['functions'] if x['section']==a['section'] and x['offset']<=a['offset'] and b['offset']+8<=x['offset']+x['size']],'ambiguous consuming function')
    require(0<f['size']<=MAX_FUNCTION and f['size']%4==0,'unsupported consumer extent')
    rels=[r for r in snapshot['relocations'] if r['section']==a['section']]
    require(all(r['type'] in MASKS and r['offset']%4==0 and r['offset']+4<=source['size'] for r in rels) and len({r['offset'] for r in rels})==len(rels),'unknown/overlapping executable relocations')
    require(not any(r['offset']==b['offset']+4 for r in rels),'load instruction relocated')
    base=e.sections[source['index']][4]+a['offset'];words=struct.unpack_from('<III',e.d,base)
    _pair(words)
    require(words[0]&MASKS[311]==0 and words[1]&MASKS[312]==0,'object GOT immediates not zero')
    return {'classification':'pending-named-initialized-data','section':section,'metadata':s,'symbol':symbol,'initializer_hex':value.hex(),'consumer_function':f,'consumer_offset':a['offset']-f['offset'],'consumer_words':list(words)}

def _pair(words):
    a,b,d=words
    require(a&0x9f000000==0x90000000 and a&31!=31 and b&0xffc00000==0xf9400000 and
            (b>>5)&31==a&31 and b&31!=31 and d&0xffc00000==0xb9400000 and
            (d>>5)&31==b&31 and d&31!=31 and (d>>10)&4095==0,'unsupported 32-bit GOT load')

def _slot(meta,blob,pc):
    off=_mapped(meta,pc,12,5,2);words=struct.unpack_from('<III',blob,off);_pair(words)
    a,b,_=words;imm=((a>>29)&3)|(((a>>5)&0x7ffff)<<2)
    if imm&(1<<20):imm-=1<<21
    return (pc&~4095)+(imm<<12)+((b>>10)&4095)*8,words

def _no_writes(meta,widths,start,size):
    _no_linked_write(meta,widths,start,start+size)

def _writable(meta,section,address,size):
    require(section['type']==1 and section['flags']&7==3 and not section['flags']&0x400 and
            section['address']<=address and address+size<=section['address']+section['size'],
            'unsupported writable storage')
    _mapped(meta,address,size,6,1)
    _unique_allocated_extent(meta,section,address,size)
    require(not any(s['type'] in (7,0x6474e552) and s['address']<address+size and
                    address<s['address']+s['memory_size'] for s in meta['segments']),
            'TLS or GNU_RELRO overlaps writable storage')

def _is_marker(name,typ,binding,other,size):
    return typ==0 and binding==0 and other==0 and size==0 and bool(re.fullmatch(r'\$d(?:\.[0-9]+)?',name))

def _reject_nss_aliases(symbols, selected, sections, address, size):
    for s in symbols:
        if s is selected or s[1] not in (0,1,2,5,6):continue
        if not (0<s[4]<len(sections) or s[4]==0xfff1):continue
        if s[4]==selected[4] and _is_marker(s[0],s[1],s[2],s[3],s[6]):continue
        require(not (s[5]<address+size and address<s[5]+max(s[6],1)),'original NSS extent alias')

def _symbol(meta,blob,name,kind=1):
    rows=[s for s in meta['symbols'] if s['name']==name]
    require(rows and all(s['defined'] for s in rows),'missing/conflicting defined symbol')
    require(len({s['table_section'] for s in rows})==len(rows),'duplicate symbol in one table')
    identities={(x['address'],x['size'],x['type'],x['binding'],x['visibility'],x['section_index']) for x in rows}
    require(len(identities)==1,'ambiguous linked identity')
    for r in rows:
        table=meta['sections'][r['table_section']]
        require(table['entry_size']==24 and r['index']*24+24<=table['size'] and blob[table['offset']+r['index']*24+5]==0,'unsupported st_other')
    r=rows[0];require((r['type'],r['binding'],r['visibility'])==(kind,1,0) and 0<r['section_index']<len(meta['sections']),'linked symbol attributes')
    sec=meta['sections'][r['section_index']];_unique_allocated_extent(meta,sec,r['address'],r['size'])
    if kind==1:
        for other in meta['symbols']:
            if not other['defined'] or other['name']==name or other['type'] not in (0,1,2,5,6):continue
            table=meta['sections'][other['table_section']]
            require(table['entry_size']==24 and other['index']*24+24<=table['size'],'invalid symbol record')
            raw_other=blob[table['offset']+other['index']*24+5]
            if other['section_index']==r['section_index'] and _is_marker(other['name'],other['type'],other['binding'],raw_other,other['size']):continue
            require(not (other['address']<r['address']+r['size'] and r['address']<other['address']+max(other['size'],1)),
                    'linked data symbol alias')
    return r

def _got(meta,widths,slot,target):
    require(slot%8==0,'unaligned GOT slot');_mapped(meta,slot,8,6,1)
    sections=[s for s in meta['sections'] if s['name']=='.got' and s['address']<=slot and slot+8<=s['address']+s['size'] and s['flags']&3==3]
    one(sections,'GOT section')
    rel=one([r for r in meta['relocations'] if r['offset']<slot+8 and slot<r['offset']+widths[r['type']]],'GOT writes')
    require((rel['offset'],rel['type'],rel['symbol_index'],rel['addend'])==(slot,1027,0,target),'GOT target identity')
    return rel

def _dwarf_owner(nss,name,address,owner,executable,executable_sha256):
    # Exact-name query is bounded; do not trust a caller-supplied parsed record.
    executable=Path(executable).resolve()
    require(executable.is_file() and sha(executable)==executable_sha256,'DWARF tool pin mismatch')
    proc=subprocess.Popen([str(executable),'--name='+name,'--show-parents',str(nss)],stdout=subprocess.PIPE,stderr=subprocess.DEVNULL)
    out=bytearray();deadline=time.monotonic()+30
    try:
        with selectors.DefaultSelector() as selector:
            selector.register(proc.stdout,selectors.EVENT_READ)
            while True:
                remaining=deadline-time.monotonic();require(remaining>0,'DWARF lookup timeout')
                require(selector.select(remaining),'DWARF lookup timeout')
                block=os.read(proc.stdout.fileno(),min(65536,2*1024*1024+1-len(out)))
                if not block:break
                out.extend(block);require(len(out)<=2*1024*1024,'DWARF output exceeds limit')
        require(proc.wait(timeout=max(0.01,deadline-time.monotonic()))==0,'DWARF lookup failed')
    finally:
        if proc.poll() is None:proc.kill();proc.wait()
        proc.stdout.close()
    values=parse_dwarf_variables(out.decode('utf-8','strict').splitlines())
    row=one([v for v in values if (v['linkage'] or v['name'])==name],'DWARF defining variable')
    require(row['address']==address and not row['merged_offset'] and row['external'] and not row['specification'] and
            row['cu']==owner and row['decl_file']==owner and not row['function'],'DWARF owner differs')
    require(sha(executable)==executable_sha256,'DWARF tool changed')
    return row

def _receipt_objects(root):
    root=Path(root);done=receipts.read_json(root/'done.json');mode=done.get('mode')
    require(mode in ('normal','lua'),'unsupported receipt mode')
    names={k[len('objects/'):]:v for k,v in done['files'].items() if k.startswith('objects/') and k.endswith('.obj')}
    require(names and len(names)<=100000,'receipt lacks bounded object set')
    object_root=root/'objects';require(object_root.is_dir() and not object_root.is_symlink(),'invalid object root')
    entries=list(object_root.rglob('*'))
    require(not any(p.is_symlink() for p in entries),'object namespace contains symlink')
    actual={p.relative_to(object_root).as_posix() for p in entries if p.is_file() and p.name.endswith('.obj')}
    require(actual==set(names),'unsealed or missing object in namespace')
    require(all((object_root/name).stat().st_size<=MAX_ELF for name in names),'input object exceeds bound')
    seal=receipts.retained_mode(root,'recovery-only',mode)
    return mode,seal,names

def _census_object(path,name):
    e=Elf(path);strict._object_layout(e)
    require(not any(s[1]==11 for s in e.sections),'unexpected object dynamic symbol table')
    require(not any(s['name']==name for s in strict.symbols(e).values()),'existing object references named data')

def _map_contribution(mapdata,meta,output,owner,section,expected_size,alignment):
    """Route only real contributions under the authenticated writable output."""
    require(output['name']=='.data','unsupported writable output section')
    current=None;headers=[];lines=[]
    for line in mapdata.decode('utf-8','strict').splitlines()[1:]:
        fields=line.split()
        if len(fields)!=4:continue
        if fields[3].startswith('.') and ':(' not in fields[3]:
            current=fields[3]
            if current==output['name']:headers.append((int(fields[0],16),int(fields[1],16)))
        elif current==output['name'] and ':(' in fields[3]:lines.append(line)
    require(headers==[(output['address'],output['size'])],'data map output extent')
    records=_map_records(('Address Size Align Out In Symbol\n'+'\n'.join(lines)).encode())
    for r in records:
        require(output['address']<=r['address'] and r['address']+r['size']<=output['address']+output['size'],'data contribution outside output')
    spans=sorted((r['address'],r['address']+r['size']) for r in records if r['size'])
    require(all(a[1]<=b[0] for a,b in zip(spans,spans[1:])),'overlapping data contributions')
    row=_route(records,'/work/build/2.6.2/'+owner,section,expected_size,alignment)
    return row['address']

def prove(mapping,ledger,original,nss,owner,owner_object,baseline_root,candidate_root,*,original_sha256,nss_sha256,dwarfdump_path,dwarfdump_sha256):
    """Regenerate narrow raw evidence from trusted original pins and sealed roots.

    A receipt seal binds artifacts, not the origin of externally supplied hashes;
    caller must authenticate the receipt chain and pins before using witnesses.
    """
    descriptors=mapping.get('new_named_data_sections',[])
    if not descriptors:return []
    require(len(descriptors)==1,'one new named data section supported')
    baseline_root,candidate_root=Path(baseline_root),Path(candidate_root)
    bm,bs,bfiles=_receipt_objects(baseline_root);cm,cs,cfiles=_receipt_objects(candidate_root);require(bm==cm,'receipt modes differ')
    owner_object=Path(owner_object);parts=owner_object.parts;require(parts.count('objects')==1,'ambiguous owner path')
    relative=Path(*parts[parts.index('objects')+1:]).as_posix();require(relative.endswith('/src/'+owner+'.obj') and candidate_root/'objects'/relative==owner_object,'owner path differs')
    require(set(cfiles)==set(bfiles)|{relative} and relative not in bfiles,'unexpected object-set delta')
    from validation.recovery_diagnostics import runtime_digest
    for rel in bfiles:
        require((baseline_root/'objects'/rel).stat().st_size<=MAX_ELF and
                (candidate_root/'objects'/rel).stat().st_size<=MAX_ELF,'input object exceeds bound')
        require(runtime_digest(baseline_root/'objects'/rel)==runtime_digest(candidate_root/'objects'/rel),'previous object runtime changed')
    snap=recovery_mapping.inspect(owner_object);desc=named_section(snap,descriptors[0]['section'],owner_object)
    require(desc==descriptors[0],'pending section changed')
    symbol=desc['symbol'];name=symbol['name'];size=symbol['size'];value=bytes.fromhex(desc['initializer_hex']);require(size==4,'only scalar 32-bit consumer supported')
    # Complete baseline symbol-table census: no old definition, import, or
    # relocation target may already name this storage (including debug refs).
    for rel in bfiles:
        _census_object(baseline_root/'objects'/rel,name)
    oldlink=baseline_root/'build/2.6.2/d3';require(oldlink.stat().st_size<=MAX_ELF,'baseline ELF exceeds bound')
    oldprov=verify_link_provenance(oldlink);oldblob=oldlink.read_bytes()
    require(hashlib.sha256(oldblob).hexdigest()==oldprov['elf_sha256'],'baseline ELF changed')
    oldmeta,_,_,_=_linked_metadata(oldblob)
    require(not any(s['name']==name for s in oldmeta['symbols']) and
            not any(r['symbol_name']==name for r in oldmeta['relocations']),
            'baseline link already names data')
    rows=[r for r in ledger['rows'] if r['name']==name];row=one(rows,'original data ledger identity')
    require(row['size']==size and row['size_status']=='exact_symbol','original exact symbol size')
    val=row['validation'];require(val['status']=='verified_equal' and val['reasons']==[] and val['storage_verified'] and val['non_relocated_equal'],'data ledger not verified equal')
    addr=int(row['address'])&0xffffffff
    original=Path(original);nss=Path(nss)
    require(original.is_file() and not original.is_symlink() and original.stat().st_size<=MAX_ELF and
            nss.is_file() and not nss.is_symlink() and nss.stat().st_size<=320*1024**2,'bounded original inputs required')
    require(sha(original)==original_sha256 and sha(nss)==nss_sha256,'original input pin mismatch')
    original_data=original.read_bytes();om,_,_,ow=_linked_metadata(original_data)
    require(hashlib.sha256(original_data).hexdigest()==original_sha256,'original changed')
    nss_evidence=[]
    with _pinned_reader(nss,320*1024**2,nss_sha256) as nssreader:
        sections=_file_table(nssreader);syms=_file_symbols(nssreader,sections)
        ns=one([s for s in syms if s[0]==name],'original NSS name ambiguous')
        require(ns[1:4]==(1,1,0) and 0<ns[4]<len(sections) and ns[5:]==(addr,size),'original NSS data identity')
        _reject_nss_aliases(syms,ns,sections,addr,size)
        sec=sections[ns[4]];require(sec[1]==1 and sec[2]&7==3 and not sec[2]&0x400 and sec[3]<=addr and addr+size<=sec[3]+sec[5],'NSS data section')
        require(nssreader.read(sec[4]+addr-sec[3],size)==value,'NSS initializer differs')
        f=desc['consumer_function'];original_func=one([s for s in syms if s[0]==f['name']],'NSS consumer name')
        require(original_func[1:4]==(2,1,0) and original_func[6]==f['size'] and (f['binding'],f['other'],f['offset'])==(1,0,0),'NSS consumer identity')
        faddr=original_func[5];nss_evidence=[ns,original_func]
        from validation.qualified_owner import _file_mapped_code
        original_off=_mapped(om,faddr,f['size'],5,2)
        require(_file_mapped_code(nssreader,faddr,f['size'])==original_data[original_off:original_off+f['size']],'NSS consumer code differs')
    off=_mapped(om,addr,size,6,1);require(original_data[off:off+size]==value,'original initializer differs')
    alloc=one([s for s in om['sections'] if s['flags']&2 and s['address']<=addr and addr+size<=s['address']+s['size']],'original data section')
    _writable(om,alloc,addr,size);require(addr%desc['metadata']['alignment']==0,'original alignment differs');_no_writes(om,ow,addr,size)
    dwarf_owner=_dwarf_owner(nss,name,addr,owner,dwarfdump_path,dwarfdump_sha256)
    # The consumer must be attributed to the same owner in the bound inventory.
    from validation.common import rows
    files=rows(baseline_root/'data/2.6.2/function_files.csv')
    require(len([r for r in files if (int(r['Address'],16)&0xffffffff)==faddr and r['File']==owner])==1 and len([r for r in files if (int(r['Address'],16)&0xffffffff)==faddr])==1,'consumer owner inventory differs')
    pc=faddr+desc['consumer_offset'];_no_writes(om,ow,pc,12);origslot,origwords=_slot(om,original_data,pc);_got(om,ow,origslot,addr)
    for i,mask in enumerate((MASKS[311],MASKS[312],0)):
        require((origwords[i]^desc['consumer_words'][i])&~mask==0,'original consumer instruction differs')
    link=candidate_root/'build/2.6.2/d3';require(link.stat().st_size<=MAX_ELF,'linked ELF exceeds bound')
    require(Path(str(link)+'.map').stat().st_size<=MAX_ELF,'map exceeds bound')
    from validate_data import validate_data
    fresh=validate_data(baseline_root/'data/2.6.2/data_symbols.csv',original,link,nss)
    require(fresh==ledger,'data ledger does not reproduce')
    prov=verify_link_provenance(link);require(ledger['provenance']==prov and ledger['inputs']['original']['sha256']==original_sha256 and ledger['inputs']['nss']['sha256']==nss_sha256 and ledger['inputs']['rebuilt']['sha256']==prov['elf_sha256'],'ledger provenance differs');blob=link.read_bytes();require(hashlib.sha256(blob).hexdigest()==prov['elf_sha256'],'candidate ELF changed')
    meta,_,_,widths=_linked_metadata(blob);rebuilt=_symbol(meta,blob,name);require(rebuilt['size']==size,'linked data size')
    sec=meta['sections'][rebuilt['section_index']];_writable(meta,sec,rebuilt['address'],size)
    mappath=Path(str(link)+'.map');require(mappath.stat().st_size<=MAX_ELF,'map exceeds bound')
    mapdata=mappath.read_bytes();require(hashlib.sha256(mapdata).hexdigest()==prov['map_sha256'],'map changed')
    start=_map_contribution(mapdata,meta,sec,relative,desc['section'],size,desc['metadata']['alignment']);require(start==rebuilt['address'] and row['comparison'].get('rebuilt_address')==start,'linked data route')
    pos=_mapped(meta,start,size,6,1);require(blob[pos:pos+size]==value,'linked initializer differs');_no_writes(meta,widths,start,size)
    caller=_symbol(meta,blob,f['name'],2);require(caller['size']==f['size'],'linked consumer size')
    from validation.recovery_function_got import text_map
    records=text_map(mapdata,meta)
    route=_route(records,'/work/build/2.6.2/'+relative,f['section'],f['size'],snap['sections'][f['section']]['alignment']);require(route['address']==caller['address'],'linked consumer route')
    _no_writes(meta,widths,caller['address'],caller['size']);caller_code,caller_masks=Elf(owner_object).functions()[f['name']];caller_bytes=blob[_mapped(meta,caller['address'],caller['size'],5,2):][:caller['size']]
    require(len(caller_code)==len(caller_bytes)==f['size'] and all((int.from_bytes(caller_code[i:i+4],'little')^int.from_bytes(caller_bytes[i:i+4],'little')) & ~caller_masks.get(i,0)==0 for i in range(0,f['size'],4)),'linked caller code differs')
    linkedslot,linkedwords=_slot(meta,blob,caller['address']+desc['consumer_offset']);_got(meta,widths,linkedslot,start)
    for i,mask in enumerate((MASKS[311],MASKS[312],0)):
        require((linkedwords[i]^desc['consumer_words'][i])&~mask==0,'linked consumer instruction differs')
    require(sha(original)==original_sha256 and sha(nss)==nss_sha256,'original changed during proof')
    require(sha(owner_object)==cfiles[relative] and receipts.retained_mode(baseline_root,'recovery-only',bm)==bs and receipts.retained_mode(candidate_root,'recovery-only',cm)==cs,'receipt changed during proof')
    return [{'kind':'named-initialized-writable-data-v1','identity':[int(row['address']),name],'owner':owner,'object':relative,'object_sha256':sha(owner_object),'original_sha256':original_sha256,'nss_sha256':nss_sha256,'dwarfdump_path':str(Path(dwarfdump_path).resolve()),'dwarfdump_sha256':dwarfdump_sha256,'dwarf_owner':dwarf_owner,'baseline_receipt_sha256':bs,'candidate_receipt_sha256':cs,'provenance':prov,'descriptor':desc,'original_address':addr,'original_got_slot':origslot,'linked_address':start,'linked_got_slot':linkedslot,'nss_symbols':[list(x) for x in nss_evidence],'runtime_dynamic_consumers_proven':False,'exact_certified':False}]
