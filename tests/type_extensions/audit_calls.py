"""Verify named direct-call targets at original instruction offsets in exact matches."""
import csv,json,struct,sys
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2]
sys.path.insert(0,str(ROOT/'tools'))
from validation.elf import Elf
OUT=ROOT/'build/try/type_evidence'
OUT.mkdir(parents=True,exist_ok=True)
obj=Elf(Path(sys.argv[1]));original=Elf(ROOT/'data/2.6.2/main.elf')
symbols=[];relocations={}
for s in obj.sections:
    if s[1]==2:
        names=obj.sections[s[6]][4]
        for o in range(s[4],s[4]+s[5],24):
            n,info,other,sec,value,size=struct.unpack_from('<IBBHQQ',obj.d,o)
            end=obj.d.index(b'\0',names+n)
            symbols.append((obj.d[names+n:end].decode(),sec,value,size,info))
for s in obj.sections:
    if s[1]==4:
        for o in range(s[4],s[4]+s[5],24):
            off,info,addend=struct.unpack_from('<QQq',obj.d,o)
            if (info&0xffffffff) in (282,283):relocations[(s[7],off)]=(symbols[info>>32],addend)
inventory={r['Name'].split('@')[0]:(int(r['Address'],16)&0xffffffff,int(r['Size'])) for r in csv.DictReader(open(ROOT/'data/2.6.2/functions.csv'))}
addresses={a:n for n,(a,sz) in inventory.items()}
code=obj.functions();checked=[];unknown=[];failures=[]
for name,sec,value,size,info in symbols:
    if name not in inventory or name not in code:continue
    a,sz=inventory[name]
    if sz!=size:continue
    raw,masks=code[name];want=original.read_vaddr(a,sz)
    if any((struct.unpack_from('<I',raw,i)[0]^struct.unpack_from('<I',want,i)[0])&(~masks.get(i,0)&0xffffffff) for i in range(0,sz,4)):continue
    count=0
    for i in range(0,sz,4):
        if (sec,value+i) not in relocations:continue
        sym,add=relocations[(sec,value+i)];n,ss,sv,szz,si=sym
        if (si&15)==3:
            target=sv+add
            matches=[sn for sn,ssec,sval,ssize,sinfo in symbols if ssec==ss and sval==target and (sinfo&15)==2]
            if len(matches)!=1:raise ValueError((name,i,matches))
            n=matches[0]
        insn=struct.unpack_from('<I',want,i)[0];delta=insn&0x3ffffff
        if delta&0x2000000:delta-=0x4000000
        target=a+i+4*delta
        if target not in addresses:
            unknown.append({'function':name,'offset':i,'object_target':n,'original_target':hex(target)});continue
        expected=addresses[target]
        if n!=expected:failures.append({'function':name,'offset':i,'expected':expected,'actual':n})
        else:count+=1
    checked.append({'function':name,'calls':count})
report={'object':sys.argv[1],'exact_functions_audited':len(checked),'named_direct_calls_verified':sum(r['calls'] for r in checked),'unresolved_targets':unknown,'failures':failures,'functions':checked}
(OUT/'compiled-calls.audit.json').write_text(json.dumps(report,indent=2)+'\n')
print(report['exact_functions_audited'],'exact functions;',report['named_direct_calls_verified'],'verified direct-call targets;',len(unknown),'external/unresolved;',len(failures),'failures')
if failures:print(failures)

raise SystemExit(1 if failures else 0)
