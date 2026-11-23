"""Compare compiled SWIG table bytes and pointer targets against the original ELF."""
import json,struct,sys
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2]
sys.path.insert(0,str(ROOT/'tools'))
from validation.elf import Elf
OUT=ROOT/'build/try/lua_wrapper_evidence'
OUT.mkdir(parents=True,exist_ok=True)
from game_lua_tables import LuaTableImage, plain
e = LuaTableImage(ROOT, Elf(ROOT / "data/2.6.2/main.elf"))
obj=Elf(Path(sys.argv[1]))
symbols=[]; relocations={}
def string(d,o):return d[o:d.index(b'\0',o)].decode()
for s in obj.sections:
    if s[1]==2:
        names=obj.sections[s[6]][4]
        for o in range(s[4],s[4]+s[5],24):
            n,info,other,sec,value,size=struct.unpack_from('<IBBHQQ',obj.d,o)
            symbols.append((string(obj.d,names+n),sec,value,size,info))
for s in obj.sections:
    if s[1]==4:
        for o in range(s[4],s[4]+s[5],24):
            off,info,addend=struct.unpack_from('<QQq',obj.d,o)
            relocations[(s[7],off)]=(symbols[info>>32],addend)
by_name={plain(n):(n,sec,value,size,info) for n,sec,value,size,info in symbols if sec and sec<len(obj.sections) and n}
def original_target(p):
    if p in e.syms:return ('symbol',e.syms[p],0)
    if e.addr['swig_types']<=p<e.addr['swig_types']+104:return ('symbol','swig_types',p-e.addr['swig_types'])
    return ('text',e.text(p))
def object_target(sym,add):
    n,sec,value,size,info=sym
    if not sec:return ('symbol',plain(n),add)
    target=value+add
    if n and (info&15)!=3:return ('symbol',plain(n),add)
    for sn,ss,sv,sz,si in symbols:
        if sn and ss==sec and sz and sv<=target<sv+sz and (si&15) in (1,2):return ('symbol',plain(sn),target-sv)
    return ('text',json.dumps(string(obj.d,obj.sections[sec][4]+target)))
results=[]
for row in json.loads((OUT / 'tables.evidence.json').read_text())['tables']:
    n=row['name'];a=e.addr[n];count=row['size']
    if n not in by_name:
        results.append({'name':n,'result':'not emitted'});continue
    _,sec,off,size,_=by_name[n]
    assert size==count,(n,size,count)
    raw=bytearray(obj.d[obj.sections[sec][4]+off:obj.sections[sec][4]+off+size]) if obj.sections[sec][1]!=8 else bytearray(size)
    expected=bytearray(e.raw(a,size));targets=0
    for r in row['relocations']:
        i=r['offset'];sym,add=relocations[(sec,off+i)]
        left=original_target(e.ptr(a+i));right=object_target(sym,add)
        assert left==right,(n,i,left,right)
        raw[i:i+8]=bytes(8);expected[i:i+8]=bytes(8);targets+=1
    assert raw==expected,(n,'non-relocated bytes differ')
    results.append({'name':n,'result':'exact bytes and semantic pointer targets','size':size,'pointers':targets})
report={'object':sys.argv[1],'tables':results,'checked_tables':sum(x['result'].startswith('exact') for x in results),'checked_pointers':sum(x.get('pointers',0) for x in results)}
(OUT/'compiled-tables.audit.json').write_text(json.dumps(report,indent=2)+'\n')
print(report['checked_tables'],'tables and',report['checked_pointers'],'pointer targets verified')
