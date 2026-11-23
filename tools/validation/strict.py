"""Strict object admission: resolve calls/named data; authenticate literal pools. Fail closed."""
from pathlib import Path
import sys,csv,json,struct,collections,hashlib,re
from dataclasses import dataclass
# Promoted from build/deepseek-harvest-20261020/verify.py (2026-10-20).
# Default relocation policy is retained; owner-local proofs are explicit opt-in.
from .elf import Elf, MASKS

@dataclass(frozen=True)
class _OwnerContext:
 owner: str
 original_sha256: str
 members: tuple
 root: str
 pins: tuple

def _elf_header(data, expected_type):
 if len(data)<64 or data[:7]!=b'\x7fELF\x02\x01\x01':raise ValueError('invalid ELF class or encoding')
 typ,machine,version=struct.unpack_from('<HHI',data,16)
 if typ!=expected_type or machine!=183 or version!=1:raise ValueError('invalid ELF type or machine')
 if struct.unpack_from('<H',data,52)[0]!=64:raise ValueError('invalid ELF header size')

def _object_layout(e):
 """Validate the symbol/relocation tables before accepting local proofs."""
 d=e.d;_elf_header(d,1)
 shoff=struct.unpack_from('<Q',d,0x28)[0];shentsize,shnum,shstr=struct.unpack_from('<HHH',d,0x3a)
 if shentsize!=64 or not shnum or shstr>=shnum or shoff<64 or shoff+shentsize*shnum>len(d):raise ValueError('invalid section table')
 tables=[i for i,s in enumerate(e.sections) if s[1]==2]
 if len(tables)!=1:raise ValueError('ambiguous symbol tables')
 symidx=tables[0];sym=e.sections[symidx]
 for s in e.sections:
  if s[1]!=8 and (s[4]>len(d) or s[5]>len(d)-s[4]):raise ValueError('section exceeds file')
  if s[1] in (9,19):raise ValueError('unmodeled relocation section type')
 if sym[9]!=24 or sym[5]%24 or sym[6]>=len(e.sections) or e.sections[sym[6]][1]!=3:raise ValueError('invalid symbol table link or extent')
 strings=e.sections[sym[6]]
 for off in range(sym[4],sym[4]+sym[5],24):
  name=struct.unpack_from('<I',d,off)[0]
  if name>=strings[5] or d.find(b'\0',strings[4]+name,strings[4]+strings[5])<0:raise ValueError('invalid symbol name')
 for s in e.sections:
  if s[1]!=4:continue
  if s[9]!=24 or s[5]%24 or s[6]!=symidx or s[7]<=0 or s[7]>=len(e.sections):raise ValueError('invalid relocation table link or extent')
  for off in range(s[4],s[4]+s[5],24):
   pos,info,_=struct.unpack_from('<QQq',d,off)
   if info>>32>=sym[5]//24 or pos>=e.sections[s[7]][5]:raise ValueError('invalid relocation symbol or offset')

def build_owner_context(root, owner, expected_pins):
 """Build opt-in local-reference evidence from explicitly pinned original inputs."""
 try:
  from verify_original_inputs import fingerprint
 except ModuleNotFoundError:
  from ..verify_original_inputs import fingerprint
 if not isinstance(owner,str) or not owner or not isinstance(expected_pins,dict) or set(expected_pins)!={'original_sha256','functions_sha256','function_files_sha256'}:
  raise ValueError('invalid owner or original-input pins')
 if any(not isinstance(v,str) or not re.fullmatch('[0-9a-f]{64}',v) for v in expected_pins.values()):raise ValueError('invalid original-input pin')
 root=Path(root);base=root/'data/2.6.2'
 paths={'original_sha256':base/'main.elf','functions_sha256':base/'functions.csv','function_files_sha256':base/'function_files.csv'}
 before={k:fingerprint(p)['sha256'] for k,p in paths.items()}
 if before!=expected_pins:raise ValueError('original-input pin mismatch')
 with paths['functions_sha256'].open(newline='') as f: rows=list(csv.DictReader(f))
 with paths['function_files_sha256'].open(newline='') as f: files=list(csv.DictReader(f))
 chosen=[x for x in files if x['File']==owner]
 addresses=[int(x['Address'],16) for x in chosen]
 if not addresses or len(set(addresses))!=len(addresses):raise ValueError('ambiguous or empty owner membership')
 # A single original address must have exactly one owning file and function row.
 members=[]
 for address in addresses:
  owned=[x for x in files if int(x['Address'],16)==address]
  match=[x for x in rows if int(x['Address'],16)==address]
  if len(owned)!=1 or len(match)!=1:raise ValueError('ambiguous owner or original address')
  row=match[0];name=row['Name'];size=int(row['Size'])
  if not name or '@' in name or size<=0 or len([x for x in rows if x['Name']==name])!=1:raise ValueError('ambiguous original function identity')
  members.append((name,address,size))
 if len({m[0] for m in members})!=len(members):raise ValueError('duplicate owner name')
 image=Elf(paths['original_sha256']);_elf_header(image.d,3)
 phoff=struct.unpack_from('<Q',image.d,0x20)[0];entsize,num=struct.unpack_from('<HH',image.d,0x36)
 if entsize!=56 or not num or phoff<64 or phoff+entsize*num>len(image.d):raise ValueError('invalid original program headers')
 mappings=[]
 for i in range(num):
  typ,flags,offset,va,_,filesz,memsz,_=struct.unpack_from('<IIQQQQQQ',image.d,phoff+i*entsize)
  if typ==1:
   if filesz>memsz or offset>len(image.d) or filesz>len(image.d)-offset:raise ValueError('invalid original load mapping')
   mappings.append((va,va+filesz,va+memsz,flags))
 extents=sorted(((a&0xffffffff),(a&0xffffffff)+size) for _,a,size in members)
 for start,end in extents:
  intersect=[m for m in mappings if m[0]<end and start<m[2]]
  if start%4 or end%4 or len(intersect)!=1 or not (intersect[0][0]<=start<end<=intersect[0][1]) or intersect[0][3]&3!=1:raise ValueError('invalid original function mapping')
 if any(x[1]>y[0] for x,y in zip(extents,extents[1:])):raise ValueError('invalid or overlapping original function extent')
 if hashlib.sha256(image.d).hexdigest()!=expected_pins['original_sha256'] or {k:fingerprint(p)['sha256'] for k,p in paths.items()}!=before:raise ValueError('original input changed during authentication')
 return _OwnerContext(owner,expected_pins['original_sha256'],tuple(sorted(members)),str(root.resolve()),tuple(sorted(expected_pins.items())))

def _validate_owner(e,syms,targets,context,rels=None):
 if not isinstance(context,_OwnerContext) or hashlib.sha256(orig.d).hexdigest()!=context.original_sha256:raise ValueError('owner context does not bind configured original')
 if build_owner_context(context.root,context.owner,dict(context.pins))!=context:raise ValueError('owner context failed authentication')
 actual=tuple(sorted((r['Name'],int(r['Address'],16),int(r['Size'])) for r in targets))
 if actual!=context.members:raise ValueError('targets do not match complete owner')
 if sum(sec[1]==2 for sec in e.sections)!=1:raise ValueError('ambiguous symbol tables')
 funcs=[x for x in syms.values() if x['type']==2 and x['section']]
 if len(funcs)!=len(context.members) or {x['name'] for x in funcs}!={x[0] for x in context.members}:raise ValueError('candidate owner symbols are incomplete or ambiguous')
 for f in funcs:
  idx=f['section'];start=f['value'];end=start+f['size']
  if idx>=len(e.sections) or idx>=0xff00 or f['size']<=0 or start<0 or start%4 or end%4:raise ValueError('invalid candidate function')
  sec=e.sections[idx]
  if sec[1]!=1 or sec[2]&7!=6 or end>sec[5]:raise ValueError('invalid candidate executable extent')
 for i,f in enumerate(funcs):
  for g in funcs[i+1:]:
   if f['section']==g['section'] and f['value']<g['value']+g['size'] and g['value']<f['value']+f['size']:raise ValueError('overlapping candidate functions')
 if rels is not None:
  widths={257:8,258:4,259:2,260:8,261:4,262:2,**{kind:4 for kind in MASKS}}
  for rel in rels:
   sec=e.sections[rel['section']]
   if not sec[2]&4:continue
   width=widths.get(rel['type'])
   if width is None:raise ValueError('unknown executable relocation width')
   start=rel['offset'];end=start+width
   if end>sec[5]:raise ValueError('executable relocation exceeds section')
   for f in funcs:
    if f['section']!=rel['section']:continue
    lo=f['value'];hi=lo+f['size']
    if start<hi and lo<end and not (lo<=start and end<=hi):raise ValueError('relocation crosses function boundary')
  for i,a in enumerate(rels):
   if not e.sections[a['section']][2]&4:continue
   for b in rels[i+1:]:
    if a['section']==b['section'] and a['offset']<b['offset']+widths[b['type']] and b['offset']<a['offset']+widths[a['type']]:raise ValueError('overlapping executable relocations')
 return funcs

def _owner_target(e,funcs,rels,rel,s,code,want,context):
 """Resolve one STT_SECTION reference only with a unique entry and safe opcode."""
 t=rel['symbol'];kind=rel['type'];idx=t['section'];pos=t['value']+rel['addend']
 if t['type']!=3 or idx<=0 or idx>=len(e.sections) or idx>=0xff00 or pos<0:return None
 sec=e.sections[idx]
 if sec[1]!=1 or sec[2]&7!=6 or pos>=sec[5]:return None
 hits=[f for f in funcs if f['section']==idx and f['value']==pos]
 if len(hits)!=1:return None
 match=[m for m in context.members if m[0]==hits[0]['name']]
 if len(match)!=1:return None
 o=rel['offset']-s['value']
 if o<0 or o%4 or o+4>len(code) or o+4>len(want):return None
 def unique_at(offset):
  return sum(x['section']==s['section'] and x['offset']<offset+4 and offset<x['offset']+4 for x in rels)==1
 if not unique_at(rel['offset']):return None
 cw=struct.unpack_from('<I',code,o)[0];ow=struct.unpack_from('<I',want,o)[0]
 if kind==283:
  if cw>>26!=0b100101 or ow>>26!=0b100101:return None
 elif kind in (275,277):
  pair=o if kind==275 else o-4
  if pair<0 or pair+8>len(code) or pair+8>len(want):return None
  first=s['value']+pair;second=first+4
  paired=[x for x in rels if x['section']==s['section'] and x['offset'] in (first,second)]
  if len(paired)!=2 or not unique_at(first) or not unique_at(second):return None
  hi=next((x for x in paired if x['offset']==first and x['type']==275),None)
  lo=next((x for x in paired if x['offset']==second and x['type']==277),None)
  if not hi or not lo or hi['symbol']!=t or lo['symbol']!=t or hi['addend']!=rel['addend'] or lo['addend']!=rel['addend']:return None
  for data in (code,want):
   aw,lw=struct.unpack_from('<II',data,pair)
   if aw&0x9f000000!=0x90000000 or lw&0xffc00000!=0x91000000 or aw&31==31 or aw&31!=(lw>>5)&31:return None
 else:return None
 return match[0][1]&0xffffffff
def configure(root):
 global orig,known
 orig=Elf(Path(root)/'data/2.6.2/main.elf');addrs=collections.defaultdict(set)
 for file in ['functions.csv','data_symbols.csv']:
  with (Path(root)/'data/2.6.2'/file).open() as stream:
   for r in csv.DictReader(stream):addrs[r['Name']].add(int(r['Address'],16)&0xffffffff)
 known={k:next(iter(v)) for k,v in addrs.items() if len(v)==1}
def symbols(e):
 out={}
 for s in e.sections:
  if s[1]!=2:continue
  ss=e.sections[s[6]]
  for i,o in enumerate(range(s[4],s[4]+s[5],24)):
   n,info,other,idx,val,size=struct.unpack_from('<IBBHQQ',e.d,o);p=ss[4]+n;name=e.d[p:e.d.index(b'\0',p)].decode();out[i]={'name':name,'section':idx,'value':val,'size':size,'type':info&15}
 return out
def relocs(e,syms):
 out=[]
 for s in e.sections:
  if s[1]!=4:continue
  for o in range(s[4],s[4]+s[5],24):
   pos,info,add=struct.unpack_from('<QQq',e.d,o);out.append({'section':s[7],'offset':pos,'type':info&0xffffffff,'symbol':syms[info>>32],'addend':add})
 return out

def verify(path,targets,owner_context=None,original_imports=False):
 if type(original_imports) is not bool or (original_imports and owner_context is None):raise ValueError('original import proofs require an owner context')
 e=Elf(path)
 if owner_context is not None:_object_layout(e)
 syms=symbols(e);rels=relocs(e,syms);functions=e.functions();byname={s['name']:s for s in syms.values() if s['section']};results=[]
 owner_funcs=_validate_owner(e,syms,targets,owner_context,rels) if owner_context is not None else None
 if owner_funcs is not None:byname={f['name']:f for f in owner_funcs}
 if original_imports:
  from .imports import prove_original_call
  table=next(sec for sec in e.sections if sec[1]==2)
  attributes={id(syms[i]):struct.unpack_from('<IBBHQQ',e.d,table[4]+i*24)[1:3] for i in syms}
 for row in targets:
  name=row['Name'];res={'symbol':name,'original_address':row['Address'],'original_size':int(row['Size']),'exact':False};results.append(res)
  if name not in functions:res['reason']='missing';continue
  s=byname[name];code,masks=functions[name];base=int(row['Address'],16)&0xffffffff;want=orig.read_vaddr(base,int(row['Size']));res['candidate_size']=len(code);res['raw_exact']=code==want
  if len(code)!=len(want):res['reason']='size';continue
  b=bytearray(code);audit=[];unsupported=[]
  for rel in rels:
   if rel['section']!=s['section'] or not s['value']<=rel['offset']<s['value']+s['size']:continue
   o=rel['offset']-s['value'];kind=rel['type'];t=rel['symbol'];add=rel['addend'];cw=struct.unpack_from('<I',b,o)[0];ow=struct.unpack_from('<I',want,o)[0]
   target=known.get(t['name']);proof='original named identity';witness=None
   if owner_funcs is not None and t['type']==3 and any(f['section']==t['section'] for f in owner_funcs):
    resolved=_owner_target(e,owner_funcs,rels,rel,s,code,want,owner_context)
    if resolved is None:unsupported.append(dict(rel,reason='unproved owner-local section reference'));continue
    target=resolved-add;proof='authenticated owner-local function entry'
   if original_imports and target is None and kind==283 and add==0 and o%4==0 and t['section']==0 and t['value']==0 and t['size']==0 and t['name'] and sum(x['name']==t['name'] for x in syms.values())==1:
    info,other=attributes[id(t)]
    if info>>4==1 and info&15 in (0,2) and other==0 and cw>>26==0b100101:
     try:
      witness=prove_original_call(Path(owner_context.root)/'data/2.6.2/main.elf',owner_context.original_sha256,base+o,t['name'])
     except ValueError:
      witness=None
     if witness is not None:target=witness['plt_address'];proof='original import PLT witness; candidate link not proved'
   # If this object emits referenced data, its bytes/relocations need separate audit.
   if target is not None and t['section'] and t['type']!=2 and proof!='authenticated owner-local function entry':
    unsupported.append(dict(rel,reason='defined data needs independent audit'));continue
   if target is None and kind in (275,277,278,284,285,286,299) and t['section']:
    # Only adjacent ADRP+load immutable literal pairs. Reject tables and address-taking.
    pair=o if kind==275 else o-4
    if pair<0 or pair+8>len(want):unsupported.append(rel);continue
    aw,lw=struct.unpack_from('<II',want,pair);lcw=struct.unpack_from('<I',code,pair+4)[0]
    lowrel=next((x for x in rels if x['section']==s['section'] and x['offset']==s['value']+pair+4 and x['type'] in (278,284,285,286,299) and x['symbol']==t and x['addend']==add),None)
    if aw&0x9f000000!=0x90000000 or not lowrel or aw&31!=(lw>>5)&31:unsupported.append(rel);continue
    width={278:1,284:2,285:4,286:8,299:16}[lowrel['type']]
    if lcw&0x3b000000!=0x39000000 or not lcw&(1<<22):unsupported.append(rel);continue
    imm=((aw>>29)&3)|(((aw>>5)&0x7ffff)<<2);imm=imm-(1<<21) if imm&(1<<20) else imm
    target=((base+pair)&~4095)+(imm<<12)+((lw>>10)&0xfff)*width
    sec=e.sections[t['section']];pos=t['value']+add
    literal=e.d[sec[4]+pos:sec[4]+pos+width]
    hasrel=any(x['section']==t['section'] and pos<=x['offset']<pos+width for x in rels)
    if hasrel or len(literal)!=width or literal!=orig.read_vaddr(target,width):unsupported.append(dict(rel,reason='literal mismatch or relocation'));continue
    proof='literal bytes '+literal.hex();target-=add
   if target is None:unsupported.append(rel);continue
   target+=add;address=base+o
   if kind in (282,283):
    delta=target-address
    if delta%4 or not -(1<<27)<=delta<(1<<27) or cw>>26!=(0b100101 if kind==283 else 0b000101):unsupported.append(rel);continue
    nw=(cw&0xfc000000)|((delta>>2)&0x3ffffff)
   elif kind in (275,276):
    delta=((target&~4095)-(address&~4095))>>12
    if not -(1<<20)<=delta<(1<<20) or cw&0x9f000000!=0x90000000:unsupported.append(rel);continue
    nw=(cw&~MASKS[kind])|((delta&3)<<29)|(((delta>>2)&0x7ffff)<<5)
   elif kind in (277,278,284,285,286,299):
    width={277:1,278:1,284:2,285:4,286:8,299:16}[kind]
    if (target&4095)%width:unsupported.append(rel);continue
    nw=(cw&~MASKS[kind])|(((target&4095)//width)<<10)
   else:unsupported.append(rel);continue
   struct.pack_into('<I',b,o,nw);record={'offset':o,'kind':kind,'symbol':t['name'],'addend':add,'resolved_target':hex(target),'proof':proof}
   if witness is not None:record['original_import_witness']=witness
   audit.append(record)
  # A raw pc-relative operand must stay inside this function unless audited above.
  def signed(n,bits):return n-(1<<bits) if n&(1<<(bits-1)) else n
  for off in range(0,len(code),4):
   if off in masks:continue
   word=struct.unpack_from('<I',code,off)[0];dest=None
   if word&0x7c000000==0x14000000:dest=off+signed(word&0x3ffffff,26)*4
   elif word&0xff000010==0x54000000 or word&0x7e000000==0x34000000:dest=off+signed((word>>5)&0x7ffff,19)*4
   elif word&0x7e000000==0x36000000:dest=off+signed((word>>5)&0x3fff,14)*4
   elif word&0x1f000000==0x10000000 or word&0x3b000000==0x18000000:unsupported.append({'offset':off,'reason':'unrelocated address/literal reference'})
   if dest is not None and not 0<=dest<len(code):unsupported.append({'offset':off,'reason':'unrelocated branch leaves function','target_offset':dest})
  res.update(original_bytes_hex=want.hex(),candidate_bytes_hex=code.hex(),relocated_bytes_hex=b.hex())
  res.update(relocations=audit,unsupported=unsupported,exact=not unsupported and bytes(b)==want,differing_offsets=[i for i in range(0,len(want),4) if b[i:i+4]!=want[i:i+4]])
  res['reason']='exact' if res['exact'] else 'unsupported_relocation' if unsupported else 'instructions'
 return {'object':str(path),'symbols':sorted(functions),'results':results}
if __name__=='__main__':
 configure(sys.argv[1]);p=Path(sys.argv[2]);q=json.loads(Path(sys.argv[3]).read_text());print(json.dumps(verify(p,q['targets']),indent=2))
