#!/usr/bin/env python3
"""Experimental, packet-bound launcher for exactly two Lua owners.

All other commands are passed to the original compiler unchanged. This does not
approve a canonical recipe, promote quality, or establish linked equivalence.
"""
import hashlib
import json
import os
from pathlib import Path
import re
import shlex
import stat
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[2]
PACKET_SHA256 = 'bd1ad1c0419c5d6e04ce8d413e7a8504e5b1c28452c721042d881c76b4329d57'
LLC_SHA256 = 'a775d990eea27372178c2d180caaaf75fefefc2b2a8b2f829797850cdec32342'
FRONTEND_SHA256 = 'ab49fde0eca84179311ef10b9aa0a330cda7d98585e9ae7da1e29d9952a07170'
FRONTEND_LINK = Path('/opt/llvm/bin/clang')
FRONTEND_FILE = Path('/opt/llvm-5.0.1/bin/clang-5.0')
OWNERS = {'Contrib/Contrib/lua/src/lstate.c': 'lstate', 'Contrib/Contrib/lua/src/lgc.c': 'lgc'}
BACKEND_FLAGS = ['-O0','-mcpu=cortex-a57','-relocation-model=pic','-verify-machineinstrs',
                 '-fast-isel=true','-aarch64-experiment-null-fast-isel=true',
                 '-experiment-disable-o0-load-chain=true','-experiment-disable-o0-indexed-loads=true',
                 '-experiment-defer-o0-trunc-load-store=true','-experiment-fold-o0-subreg-roundtrip=true',
                 '-experiment-sign-extend-o0-byte-store=true']
FRONTEND_FLAGS = ['-emit-llvm','-S','--target=aarch64-linux-gnu','-O0','-mcpu=cortex-a57',
                  '-fno-strict-aliasing','-mno-implicit-float','-ffunction-sections','-g','-funwind-tables',
                  '-fPIC','-D__NO_INLINE__','-funsigned-char','-mllvm','-fast-isel=false',
                  '-I/work/src','-I/work/include']
ALLOWED = {'--target=aarch64-linux-gnu','-O3','-O0','-mcpu=cortex-a57','-fno-strict-aliasing',
           '-mno-implicit-float','-ffunction-sections','-g','-fsigned-char','-funsigned-char',
           '-funwind-tables','-Wall','-Wextra','-fPIC','-D__NO_INLINE__'}
BLOCKED_ENV = ('CPATH','C_INCLUDE_PATH','CPLUS_INCLUDE_PATH','OBJC_INCLUDE_PATH','CCC_OVERRIDE_OPTIONS',
               'COMPILER_PATH','LIBRARY_PATH','SDKROOT','LD_PRELOAD','LD_LIBRARY_PATH',
               'CLANG_CONFIG_FILE_SYSTEM_DIR','CLANG_CONFIG_FILE_USER_DIR')


def require(condition, message):
    if not condition: raise ValueError(message)


def relative_to(path, base):
    try:path.relative_to(base);return True
    except ValueError:return False


def safe_path(path):
    path=Path(os.path.abspath(path))
    for part in (path,*path.parents):
        require(not part.is_symlink(), 'symlink in input path')
    return path


def read_regular(path, limit):
    path=safe_path(path)
    fd=os.open(path,os.O_RDONLY|os.O_NOFOLLOW|os.O_NONBLOCK)
    try:
        before=os.fstat(fd)
        require(stat.S_ISREG(before.st_mode) and 0<=before.st_size<=limit,'bounded regular file required')
        data=bytearray()
        while len(data)<=limit:
            block=os.read(fd,min(1024*1024,limit+1-len(data)))
            if not block:break
            data.extend(block)
        ident=lambda x:(x.st_dev,x.st_ino,x.st_mode,x.st_size,x.st_mtime_ns,x.st_ctime_ns)
        require(len(data)==before.st_size and ident(before)==ident(os.fstat(fd))==ident(path.lstat()),'input changed during read')
        return bytes(data)
    finally:os.close(fd)


def fingerprint(path, limit):
    data=read_regular(path,limit)
    return {'size':len(data),'sha256':hashlib.sha256(data).hexdigest()}


def packet_at(path):
    data=read_regular(path,1024*1024)
    require(hashlib.sha256(data).hexdigest()==PACKET_SHA256,'unreviewed owner recipe packet')
    def pairs(values):
        result={}
        for key,value in values:
            require(key not in result,'duplicate packet key');result[key]=value
        return result
    packet=json.loads(data,object_pairs_hook=pairs)
    require(set(packet)=={'format','kind','canonical','image','binary_sha256','frontend_sha256','owners'} and
            type(packet['format']) is int and packet['format']==1 and packet['canonical'] is False and
            packet['kind']=='experimental-lua-cmake-owner-packet-v1' and
            packet['image']=='sha256:8401a52dfd4f02450ca822b65f65595a54a219c8b1ab7c8b63e50fe3ceb36e36' and
            packet['binary_sha256']==LLC_SHA256 and packet['frontend_sha256']==FRONTEND_SHA256 and
            set(packet['owners'])==set(OWNERS),'invalid recipe packet')
    return packet


def check_frontend():
    resolved=FRONTEND_LINK.resolve(strict=True)
    require(resolved==FRONTEND_FILE and safe_path(resolved)==resolved and
            fingerprint(resolved,320*1024*1024)['sha256']==FRONTEND_SHA256,
            'frontend compiler differs')
    return resolved


def owner_for(command, root=ROOT, cwd=None):
    cwd=Path.cwd() if cwd is None else Path(cwd)
    require(not any(value.startswith('@') or value in ('-x','--') for value in command[1:]),
            'response files or language overrides prevent safe owner routing')
    selected=[]
    for value in command[1:]:
        if value.startswith('-'):continue
        for owner in OWNERS:
            candidate=Path(os.path.abspath(cwd/value))
            expected=Path(root)/'src'/owner
            if candidate!=expected and candidate.exists() and expected.exists() and os.path.samefile(candidate,expected):
                raise ValueError('selected source alias is unsupported')
            if value.endswith('.c') and (value=='src/'+owner or value.endswith('/src/'+owner)):
                selected.append((owner,candidate))
    if not selected:return None
    require(len(selected)==1,'ambiguous selected source')
    owner,path=selected[0]
    require(path==Path(root)/'src'/owner and safe_path(path)==path,'selected source outside root')
    return owner


def parse_command(command, owner, root=ROOT, cwd=None):
    root=Path(root);cwd=Path.cwd() if cwd is None else Path(cwd)
    require(command and command[0]=='/opt/llvm/bin/clang','unreviewed frontend executable')
    require(relative_to(cwd,root/'build'),'selected compile must run from build directory')
    flags=[];includes=[];values={};i=1
    while i<len(command):
        item=command[i]
        require(not item.startswith('@'),'response files unsupported')
        if item in ('-o','-c','-MF','-MT','-mllvm'):
            require(i+1<len(command) and item not in values,'missing or duplicate option')
            values[item]=command[i+1];i+=2;continue
        if item=='-I':
            require(i+1<len(command),'missing include path');includes.append(command[i+1]);i+=2;continue
        if item.startswith('-I'):
            includes.append(item[2:]);i+=1;continue
        if item=='-MD':flags.append(item);i+=1;continue
        require(item in ALLOWED,'unsupported or unexpected compiler argument: '+item)
        flags.append(item);i+=1
    require(set(values)=={'-o','-c','-MF','-MT','-mllvm'} and values['-mllvm']=='-fast-isel=false','unsupported compiler option set')
    require(owner_for(command,root,cwd)==owner and Path(os.path.abspath(cwd/values['-c']))==root/'src'/owner,'source option differs')
    require(len(includes)==2 and {Path(os.path.abspath(cwd/p)) for p in includes}=={root/'src',root/'include'},'unexpected include roots')
    require(set(flags)==ALLOWED|{'-MD'} and len(flags)==len(ALLOWED)+1,'missing or duplicate compiler flags')
    require(flags.index('-O3')<flags.index('-O0') and flags.index('-fsigned-char')<flags.index('-funsigned-char'), 'conflicting compiler flag order')
    output=Path(os.path.abspath(cwd/values['-o']));depfile=Path(os.path.abspath(cwd/values['-MF']))
    require(relative_to(output,root/'build') and relative_to(depfile,root/'build') and
            output.name==OWNERS[owner]+'.c.obj' and depfile==Path(str(output)+'.d') and
            safe_path(output)==output and safe_path(depfile)==depfile,'unsafe selected output paths')
    require(values['-MT'] and '\n' not in values['-MT'] and '\r' not in values['-MT'],'invalid dependency target')
    require(not any(os.environ.get(key) for key in BLOCKED_ENV),'uncontrolled compiler environment')
    return {'output':output,'depfile':depfile,'target':values['-MT']}


def check_closure(root, owner, packet):
    entry=packet['owners'][owner];files=entry['files']
    require(isinstance(files,dict) and len(files)==26 and 'src/'+owner in files,'invalid owner source closure')
    for name,pin in files.items():
        require(isinstance(name,str) and not name.startswith('/') and all(x not in ('','.','..') for x in name.split('/')) and
                (name.endswith('.h') or name=='src/'+owner) and isinstance(pin,dict) and set(pin)=={'size','sha256'},'invalid closure entry')
        require(fingerprint(Path(root)/name,2*1024*1024)==pin,'source/header differs: '+name)
    return entry


def verify_dependencies(depfile, root, files, source):
    text=read_regular(depfile,4*1024*1024).decode('utf-8').replace('\\\n','')
    require(': ' in text,'unsupported dependency file')
    _,body=text.split(': ',1)
    dependencies=shlex.split(body,comments=False,posix=True)
    require(dependencies,'empty dependency file')
    work_paths=set()
    for item in dependencies:
        path=Path(os.path.abspath(Path(root)/item))
        if relative_to(path,root):
            rel=path.relative_to(root).as_posix();require(rel in files,'unreviewed project dependency: '+rel)
            safe_path(path);work_paths.add(rel)
    require('src/'+source in work_paths,'source missing from dependency file')
    return sorted(work_paths)


def compile_owner(command, owner, root=ROOT, cwd=None):
    root=Path(root); cwd=Path.cwd() if cwd is None else Path(cwd)
    parsed=parse_command(command,owner,root,cwd)
    packet_path=os.environ.get('D3_LUA_OWNER_PACKET');binary_path=os.environ.get('D3_LUA_OWNER_LLC')
    require(packet_path and binary_path,'explicit recipe packet and compiler required')
    packet=packet_at(packet_path);binary=safe_path(binary_path)
    require(fingerprint(binary,64*1024*1024)['sha256']==LLC_SHA256,'diagnostic compiler differs')
    clang=check_frontend()
    closure=check_closure(root,owner,packet)
    output=parsed['output'];depfile=parsed['depfile'];sidecar=Path(str(output)+'.commands.json')
    ir=Path(str(output)+'.owner-ir.tmp');ir_final=Path(str(output)+'.ll')
    temporary=Path(str(output)+'.owner-object.tmp');side_temp=Path(str(sidecar)+'.tmp')
    outputs={output,sidecar,ir,ir_final,temporary,side_temp,depfile}
    require(len(outputs)==7 and safe_path(packet_path) not in outputs and binary not in outputs,'recipe inputs overlap outputs')
    protected={safe_path(root/name) for name in closure['files']} | {safe_path(__file__),clang}
    require(not protected.intersection(outputs),
            'source or tool input overlaps output')
    for path in (sidecar,ir,ir_final,temporary,side_temp):require(safe_path(path)==path,'symlink in output path')
    for path in outputs:path.unlink(missing_ok=True)
    frontend=[str(clang),*FRONTEND_FLAGS,'-MD','-MF',str(depfile),'-MT',parsed['target'],
              '-c',str(root/'src'/owner),'-o',str(ir)]
    backend=[str(binary),*BACKEND_FLAGS]
    if OWNERS[owner]=='lstate':backend.append('-experiment-preserve-o0-dead-frame-loads=true')
    backend+=['-function-sections','-filetype=obj',str(ir),'-o',str(temporary)]
    environment={'PATH':'/usr/bin:/bin','LANG':'C','LC_ALL':'C'}
    try:
        subprocess.run(frontend,cwd=root,env=environment,check=True)
        dependencies=verify_dependencies(depfile,root,closure['files'],owner)
        irpin=fingerprint(ir,4*1024*1024)
        require(irpin['sha256']==closure['expected_ir_sha256'],'frontend IR differs from reviewed evidence')
        subprocess.run(backend,cwd=root,env=environment,check=True)
        objpin=fingerprint(temporary,4*1024*1024)
        require(objpin['sha256']==closure['expected_object_sha256'] and objpin['size']==closure['expected_object_size'],'backend object differs from reviewed evidence')
        check_closure(root,owner,packet)
        require(fingerprint(binary,64*1024*1024)['sha256']==LLC_SHA256 and check_frontend()==clang and
                packet_at(packet_path)==packet,'recipe input changed during compile')
        metadata={'kind':'experimental-lua-owner-compile-v1','canonical':False,'accepted':False,
                  'owner':owner,'packet_sha256':PACKET_SHA256,'binary_sha256':LLC_SHA256,'frontend_sha256':FRONTEND_SHA256,
                  'incoming':command,'frontend':frontend,'backend':backend,'dependencies':dependencies,
                  'ir_path':str(ir_final),'ir_sha256':irpin['sha256'],'object_sha256':objpin['sha256']}
        side_temp.write_text(json.dumps(metadata,indent=2)+'\n')
        os.replace(ir,ir_final)
        os.replace(side_temp,sidecar)
        os.replace(temporary,output)
    except BaseException:
        for path in (output,sidecar,ir_final,temporary,side_temp,depfile):path.unlink(missing_ok=True)
        raise
    finally:ir.unlink(missing_ok=True)


def main(command=None):
    command=sys.argv[1:] if command is None else command
    if not command:raise ValueError('compiler command required')
    if command[0]=='--dependencies':
        require(len(command)==2,'--dependencies requires exactly one packet path')
        packet=packet_at(command[1]);sets=[]
        for name in OWNERS:
            files=packet['owners'][name]['files']
            require(isinstance(files,dict) and len(files)==26,'invalid owner source closure')
            headers={p for p in files if isinstance(p,str) and p.endswith('.h')}
            require(len(headers)==25 and all(not p.startswith('/') and all(x not in ('','.','..') for x in p.split('/')) for p in headers),'invalid header closure')
            sets.append(headers)
        require(sets[0]==sets[1],'owner header path sets differ')
        print('\n'.join(sorted(sets[0])))
        return
    owner=owner_for(command)
    if owner is None:os.execvp(command[0],command)
    compile_owner(command,owner)


if __name__=='__main__':
    try:main()
    except (ValueError,OSError,subprocess.SubprocessError) as error:
        print('Lua owner recipe: '+str(error),file=sys.stderr);sys.exit(1)
