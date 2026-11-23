"""Authenticated, owner-scoped ELF identities; no byte-match or acceptance claim.

This deliberately supports only the reviewed Lua lexer alias.  Consumers must
call revalidate_context before using a context and pin their own proof inputs.
"""
from dataclasses import dataclass
from contextlib import contextmanager
import csv
import hashlib
import io
import os
from pathlib import Path
import re
import stat
import struct
from types import SimpleNamespace

from . import strict

OWNER='Contrib/Contrib/lua/src/llex.c'
ALIAS_ORIGINAL='__isspace@llex.c'
ALIAS_CANDIDATE='__isspace'
GLOBAL_NAMES=frozenset(('luaX_init','luaX_token2str','luaX_lexerror','luaX_syntaxerror',
                        'luaX_newstring','luaX_setinput','luaX_next','luaX_lookahead'))
LOCAL_NAMES=frozenset(('txtToken','llex','save','inclinenumber','skip_sep','read_long_string',
                       'read_string','check_next','read_numeral',ALIAS_ORIGINAL,'buffreplace','trydecpoint'))
PINS={'original_sha256':'data/2.6.2/main.elf','functions_sha256':'data/2.6.2/functions.csv',
      'function_files_sha256':'data/2.6.2/function_files.csv','nss_sha256':'build/romfs/DiabloIIINX64r.nss'}
LIMITS={'original_sha256':64*1024**2,'functions_sha256':8*1024**2,
        'function_files_sha256':8*1024**2,'nss_sha256':320*1024**2}


@dataclass(frozen=True)
class Member:
    original_name: str
    original_address: int
    original_size: int
    candidate_name: str
    candidate_section: int
    candidate_offset: int
    candidate_size: int
    candidate_binding: int
    candidate_visibility: int
    nss_binding: int
    nss_visibility: int


@dataclass(frozen=True)
class QualifiedOwnerContext:
    root: str
    owner: str
    input_pins: tuple
    object_path: str
    object_sha256: str
    members: tuple


def require(condition,message):
    if not condition:raise ValueError(message)


def _snapshot(path,limit,digest):
    fd=os.open(path,os.O_RDONLY|os.O_NOFOLLOW|os.O_NONBLOCK)
    try:
        before=os.fstat(fd)
        require(stat.S_ISREG(before.st_mode) and 0<=before.st_size<=limit,'bounded regular file required')
        data=bytearray()
        while len(data)<=before.st_size:
            block=os.read(fd,min(1024*1024,before.st_size-len(data)+1))
            if not block:break
            data.extend(block)
        def identity(s):return s.st_dev,s.st_ino,s.st_mode,s.st_size,s.st_mtime_ns,s.st_ctime_ns
        require(len(data)==before.st_size and identity(before)==identity(os.fstat(fd))==identity(Path(path).lstat()),
                'input changed during read')
        require(hashlib.sha256(data).hexdigest()==digest,'input SHA-256 differs')
        return bytes(data)
    finally:os.close(fd)


@contextmanager
def _pinned_reader(path,limit,digest):
    """Stream-hash a large file and use bounded positioned reads for its tables."""
    fd=os.open(path,os.O_RDONLY|os.O_NOFOLLOW|os.O_NONBLOCK)
    try:
        before=os.fstat(fd)
        require(stat.S_ISREG(before.st_mode) and 0<before.st_size<=limit,'bounded regular file required')
        def identity(s):return s.st_dev,s.st_ino,s.st_mode,s.st_size,s.st_mtime_ns,s.st_ctime_ns
        class Reader:
            size=before.st_size
            def read(self,offset,size):
                require(0<=offset<=self.size and 0<=size<=self.size-offset and size<=32*1024**2,'positioned read exceeds bounds')
                data=os.pread(fd,size,offset);require(len(data)==size,'short positioned read');return data
            def digest(self):
                result=hashlib.sha256();offset=0
                while offset<self.size:
                    data=self.read(offset,min(1024*1024,self.size-offset));result.update(data);offset+=len(data)
                return result.hexdigest()
        reader=Reader();require(reader.digest()==digest and identity(before)==identity(os.fstat(fd))==identity(Path(path).lstat()),
                                'input SHA-256 or identity differs')
        yield reader
        require(reader.digest()==digest and identity(before)==identity(os.fstat(fd))==identity(Path(path).lstat()),
                'input changed during proof')
    finally:os.close(fd)


def _input_path(root,name):
    current=root
    for part in Path(name).parts:
        require(part not in ('','.','..'),'invalid input path')
        current=current/part
        require(not current.is_symlink(),'aliased input path')
    return current


def _table(data,kind):
    strict._elf_header(data,kind)
    shoff=struct.unpack_from('<Q',data,0x28)[0]
    ent,num,strings=struct.unpack_from('<HHH',data,0x3a)
    require(ent==64 and num>0 and strings<num and shoff>=64 and shoff+num*64<=len(data),'invalid section table')
    sections=[struct.unpack_from('<IIQQQQIIQQ',data,shoff+i*64) for i in range(num)]
    for sec in sections:
        if sec[1] not in (0,8):require(sec[4]<=len(data) and sec[5]<=len(data)-sec[4],'section exceeds file')
    return sections


def _symbol_rows(data,sections):
    tables=[i for i,s in enumerate(sections) if s[1]==2]
    require(len(tables)==1,'ambiguous symbol tables')
    table=sections[tables[0]]
    require(table[9]==24 and table[5]%24==0 and table[6]<len(sections) and sections[table[6]][1]==3,
            'invalid symbol/string table')
    strings=sections[table[6]]
    return _decode_symbols(data[table[4]:table[4]+table[5]],data[strings[4]:strings[4]+strings[5]],table[7])


def _decode_symbols(table,strings,first_nonlocal):
    require(0<first_nonlocal<=len(table)//24,'invalid local-symbol boundary')
    rows=[]
    for off in range(0,len(table),24):
        index,info,other,section,value,size=struct.unpack_from('<IBBHQQ',table,off)
        require((info>>4==0)==(off//24<first_nonlocal),'invalid local-symbol ordering')
        require(index<len(strings),'symbol name outside string table')
        start=index;stop=strings.find(b'\0',start)
        require(stop>=0,'unterminated symbol name')
        try:name=strings[start:stop].decode('utf-8')
        except UnicodeDecodeError as error:raise ValueError('invalid symbol name encoding') from error
        rows.append((name,info&15,info>>4,other,section,value,size))
    return rows


def _file_table(reader):
    header=reader.read(0,64);strict._elf_header(header,3)
    shoff=struct.unpack_from('<Q',header,0x28)[0];ent,num,strings=struct.unpack_from('<HHH',header,0x3a)
    require(ent==64 and 0<num<=4096 and strings<num and shoff>=64,'invalid section table')
    raw=reader.read(shoff,num*64);sections=[struct.unpack_from('<IIQQQQIIQQ',raw,i*64) for i in range(num)]
    for sec in sections:
        if sec[1] not in (0,8):require(sec[4]<=reader.size and sec[5]<=reader.size-sec[4],'section exceeds file')
    return sections


def _file_symbols(reader,sections):
    tables=[s for s in sections if s[1]==2];require(len(tables)==1,'ambiguous symbol tables')
    table=tables[0];require(table[9]==24 and table[5]%24==0 and table[6]<len(sections) and
                            sections[table[6]][1]==3,'invalid symbol/string table')
    strings=sections[table[6]]
    require(table[5]<=8*1024**2 and strings[5]<=8*1024**2,'NSS symbol tables exceed bounds')
    return _decode_symbols(reader.read(table[4],table[5]),reader.read(strings[4],strings[5]),table[7])


def _file_mapped_code(reader,address,size):
    header=reader.read(0,64);phoff=struct.unpack_from('<Q',header,0x20)[0];ent,num=struct.unpack_from('<HH',header,0x36)
    require(ent==56 and 0<num<=4096 and phoff>=64,'invalid program table')
    raw=reader.read(phoff,ent*num);hits=[]
    for i in range(num):
        typ,flags,off,va,_,filesz,memsz,_=struct.unpack_from('<IIQQQQQQ',raw,i*ent)
        if typ!=1:continue
        require(filesz<=memsz and off<=reader.size and filesz<=reader.size-off,'invalid load segment')
        if va<address+size and address<va+memsz:hits.append((flags,off,va,filesz))
    require(len(hits)==1,'ambiguous original load mapping')
    flags,off,va,filesz=hits[0]
    require(flags&3==1 and va<=address and address+size<=va+filesz,'original code is not executable and file-backed')
    return reader.read(off+address-va,size)


def _mapped_code(data,address,size):
    phoff=struct.unpack_from('<Q',data,0x20)[0];ent,num=struct.unpack_from('<HH',data,0x36)
    require(ent==56 and num>0 and phoff>=64 and phoff+ent*num<=len(data),'invalid program table')
    hits=[]
    for i in range(num):
        typ,flags,off,va,_,filesz,memsz,_=struct.unpack_from('<IIQQQQQQ',data,phoff+i*ent)
        if typ!=1:continue
        require(filesz<=memsz and off<=len(data) and filesz<=len(data)-off,'invalid load segment')
        if va<address+size and address<va+memsz:hits.append((flags,off,va,filesz,memsz))
    require(len(hits)==1,'ambiguous original load mapping')
    flags,off,va,filesz,_=hits[0]
    require(flags&3==1 and va<=address and address+size<=va+filesz,'original code is not executable and file-backed')
    return data[off+address-va:off+address-va+size]


def _inventory(functions,files,owner):
    def rows(data):return list(csv.DictReader(io.StringIO(data.decode('utf-8'),newline='')))
    functions=rows(functions);files=rows(files)
    chosen=[row for row in files if row['File']==owner]
    require(len(chosen)==20,'owner membership must contain twenty functions')
    members=[]
    for item in chosen:
        address=int(item['Address'],16)
        require(address>>32==0x71,'unsupported original address base')
        owned=[row for row in files if int(row['Address'],16)==address]
        matched=[row for row in functions if int(row['Address'],16)==address]
        require(len(owned)==1 and len(matched)==1,'ambiguous original owner or address')
        row=matched[0];name=row['Name'];size=int(row['Size'])
        require(name and size>0 and size%4==0 and address%4==0,'invalid original identity or extent')
        if '@' in name:require(name==ALIAS_ORIGINAL and owner==OWNER,'unreviewed qualified original name')
        members.append((name,address,size))
    require({n for n,_,_ in members}==GLOBAL_NAMES|LOCAL_NAMES and
            len({a for _,a,_ in members})==20 and sum(s for _,_,s in members)==9420,'invalid complete owner identity set')
    require([(a,s) for n,a,s in members if n==ALIAS_ORIGINAL]==[(0x7100a8b990,88)],
            'qualified alias original address or extent differs')
    extents=sorted((a,a+s) for _,a,s in members)
    require(all(a[1]<=b[0] for a,b in zip(extents,extents[1:])), 'overlapping original function extents')
    return members


def build_context(root,owner,expected_pins,object_path,object_sha256):
    """Authenticate exact owner identity and candidate extents from pinned bytes.

    This does not compare candidate instructions, prove relocations or imply
    source/receipt/link acceptance.  It rejects caller-provided alias maps.
    """
    require(owner==OWNER and isinstance(expected_pins,dict) and set(expected_pins)==set(PINS),'unsupported owner or pin set')
    require(all(isinstance(v,str) and re.fullmatch(r'[0-9a-f]{64}',v) for v in expected_pins.values()) and
            isinstance(object_sha256,str) and re.fullmatch(r'[0-9a-f]{64}',object_sha256),'invalid SHA-256')
    root=Path(root).resolve();obj=Path(os.path.abspath(object_path))
    require(obj.is_relative_to(root) and obj.resolve()==obj,'candidate path must be unaliased and inside repository')
    inputs={key:_snapshot(_input_path(root,path),LIMITS[key],expected_pins[key]) for key,path in PINS.items() if key!='nss_sha256'}
    candidate=_snapshot(obj,4*1024**2,object_sha256)
    with _pinned_reader(_input_path(root,PINS['nss_sha256']),LIMITS['nss_sha256'],expected_pins['nss_sha256']) as nss:
        return _build(root,owner,expected_pins,obj,object_sha256,inputs,candidate,nss)


def _build(root,owner,expected_pins,obj,object_sha256,inputs,candidate,nss):
    original=inputs['original_sha256']
    original_sections=_table(original,3);nss_sections=_file_table(nss);candidate_sections=_table(candidate,1)
    strict._object_layout(SimpleNamespace(d=candidate,sections=candidate_sections))
    members=_inventory(inputs['functions_sha256'],inputs['function_files_sha256'],owner)
    original_symbols=_file_symbols(nss,nss_sections);candidate_symbols=_symbol_rows(candidate,candidate_sections)
    functions=[s for s in candidate_symbols if s[1]==2 and s[4]!=0]
    require(not any(s[1]==10 and s[4]!=0 for s in candidate_symbols),'candidate IFUNC is outside scope')
    require(len(functions)==20 and len({s[0] for s in functions})==20,'candidate function symbols are incomplete or ambiguous')
    candidate_names={ALIAS_CANDIDATE if name==ALIAS_ORIGINAL else name for name,_,_ in members}
    require(len(candidate_names)==20 and {s[0] for s in functions}==candidate_names,'candidate membership differs')
    require(not any(s[4]!=0 and s[1]!=2 and s[0] in candidate_names for s in candidate_symbols),
            'candidate function name collides with another definition')
    result=[];extents=[]
    for name,address,size in members:
        candidate_name=ALIAS_CANDIDATE if name==ALIAS_ORIGINAL else name
        matches=[s for s in original_symbols if s[1] in (2,10) and s[5]==(address&0xffffffff)]
        require(len(matches)==1,'ambiguous original NSS function entry')
        ns=matches[0]
        require(ns[1]==2 and ns[0]==candidate_name and ns[2]==0 and ns[6]==size,
                'original NSS symbol identity, local binding or size differs')
        require(ns[3]&~3==0,'unsupported NSS symbol visibility flags')
        require(0<ns[4]<len(nss_sections) and ns[4]<0xff00,'invalid NSS function section')
        sec=nss_sections[ns[4]];low=address&0xffffffff
        require(sec[1]==1 and sec[2]&7==6 and sec[3]<=low and low+size<=sec[3]+sec[5],
                'invalid NSS executable function extent')
        nss_code=_file_mapped_code(nss,low,size)
        require(nss.read(sec[4]+low-sec[3],size)==nss_code and
                nss_code==_mapped_code(original,low,size),'NSS section and original function bytes differ')
        selected=[s for s in functions if s[0]==candidate_name]
        require(len(selected)==1,'ambiguous candidate function identity')
        cs=selected[0];section=cs[4];start=cs[5];length=cs[6]
        require(cs[3]&~3==0,'unsupported candidate symbol visibility flags')
        require(0<section<len(candidate_sections) and section<0xff00 and start%4==0 and length>0 and length%4==0,
                'invalid candidate function section or extent')
        csec=candidate_sections[section]
        require(csec[1]==1 and csec[2]&7==6 and start+length<=csec[5], 'candidate function not file-backed executable code')
        require(cs[2]==(1 if name in GLOBAL_NAMES else 0),'candidate function binding differs')
        extents.append((section,start,start+length))
        result.append(Member(name,address,size,candidate_name,section,start,length,cs[2],cs[3]&3,ns[2],ns[3]&3))
    for i,a in enumerate(extents):
        require(not any(a[0]==b[0] and a[1]<b[2] and b[1]<a[2] for b in extents[i+1:]),'candidate functions overlap')
    # Reject any other NSS function extent overlapping one of the claimed owner functions.
    wanted={(a&0xffffffff,s) for _,a,s in members}
    for symbol in original_symbols:
        if symbol[1] not in (2,10) or symbol[4]==0:continue
        start,end=symbol[5],symbol[5]+symbol[6]
        if any((start<(address&0xffffffff)+size and (address&0xffffffff)<end) or
               (symbol[6]==0 and start==(address&0xffffffff)) for _,address,size in members):
            require(symbol[1]==2 and (start,symbol[6]) in wanted and
                    sum(1 for x in original_symbols if x[1] in (2,10) and x[5]==start)==1,
                    'overlapping or aliased original NSS function')
    # Re-read authenticated inputs so a caller cannot use a stale context.
    for key,path in PINS.items():
        if key!='nss_sha256':_snapshot(_input_path(root,path),LIMITS[key],expected_pins[key])
    _snapshot(obj,4*1024**2,object_sha256)
    return QualifiedOwnerContext(str(root),owner,tuple(sorted(expected_pins.items())),str(obj),object_sha256,
                                 tuple(sorted(result,key=lambda member:member.original_address)))


def revalidate_context(context):
    """Rebuild an immutable context before a consumer uses its identity claims."""
    require(type(context) is QualifiedOwnerContext,'qualified owner context required')
    rebuilt=build_context(context.root,context.owner,dict(context.input_pins),context.object_path,context.object_sha256)
    require(rebuilt==context,'qualified owner context differs from authenticated inputs')
    return rebuilt
