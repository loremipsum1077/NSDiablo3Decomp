import hashlib
import importlib.util
import io
import json
import os
from pathlib import Path
import subprocess
import tempfile
import unittest
from unittest.mock import patch

SCRIPT=Path(__file__).resolve().parents[1]/'toolchain/lua/owner_compile.py'
spec=importlib.util.spec_from_file_location('lua_owner_compile',SCRIPT)
owner=importlib.util.module_from_spec(spec);spec.loader.exec_module(owner)
real_check_frontend=owner.check_frontend


def sha(data):return hashlib.sha256(data).hexdigest()


class OwnerCompileTests(unittest.TestCase):
    def setUp(self):
        self.temp=tempfile.TemporaryDirectory(prefix='lua owner test ')
        self.addCleanup(self.temp.cleanup)
        self.root=Path(self.temp.name).resolve();self.cwd=self.root/'build/2.6.2';self.cwd.mkdir(parents=True)
        (self.root/'src').mkdir();(self.root/'include').mkdir()
        self.binary=self.root/'llc';self.binary.write_bytes(b'compiler')
        self.llc_sha=sha(b'compiler')
        entries={}
        for path,stem in owner.OWNERS.items():
            source=self.root/'src'/path;source.parent.mkdir(parents=True,exist_ok=True);source.write_bytes(stem.encode())
            files={'src/'+path:{'size':len(stem),'sha256':sha(stem.encode())}}
            for i in range(25):
                name=f'src/Contrib/Contrib/lua/src/h{i}.h';header=self.root/name;header.write_bytes(f'h{i}'.encode())
                files[name]={'size':len(f'h{i}'),'sha256':sha(f'h{i}'.encode())}
            entries[path]={'source_commit':'a'*40,'source_tree':'b'*40,'files':files,
                           'expected_ir_sha256':sha(b'ir'),'expected_object_sha256':sha(b'object'),'expected_object_size':6}
        packet={'format':1,'kind':'experimental-lua-cmake-owner-packet-v1','canonical':False,
                'image':'sha256:8401a52dfd4f02450ca822b65f65595a54a219c8b1ab7c8b63e50fe3ceb36e36',
                'binary_sha256':self.llc_sha,'frontend_sha256':owner.FRONTEND_SHA256,'owners':entries}
        self.packet=self.root/'packet.json';self.packet.write_text(json.dumps(packet))
        self.packet_sha=sha(self.packet.read_bytes())
        self.env={'D3_LUA_OWNER_PACKET':str(self.packet),'D3_LUA_OWNER_LLC':str(self.binary)}
        self.name=next(iter(owner.OWNERS));self.output=self.cwd/'CMakeFiles'/'lstate.c.obj';self.output.parent.mkdir()
        self.depfile=self.cwd/'CMakeFiles'/'lstate.c.obj.d'
        self.argv=['/opt/llvm/bin/clang','--target=aarch64-linux-gnu','-I../../include','-I../../src',
                   '-O3','-mcpu=cortex-a57','-fno-strict-aliasing','-mno-implicit-float','-ffunction-sections',
                   '-g','-fsigned-char','-funwind-tables','-Wall','-Wextra','-fPIC','-D__NO_INLINE__',
                   '-O0','-mllvm','-fast-isel=false','-funsigned-char','-MD','-MT','target with spaces',
                   '-MF','CMakeFiles/lstate.c.obj.d','-o','CMakeFiles/lstate.c.obj','-c',str(self.root/'src'/self.name)]
        self.stack=patch.multiple(owner,PACKET_SHA256=self.packet_sha,LLC_SHA256=self.llc_sha)
        self.stack.start();self.addCleanup(self.stack.stop)
        self.frontendpatch=patch.object(owner,'check_frontend',return_value=Path('/opt/llvm-5.0.1/bin/clang-5.0'))
        self.frontendpatch.start();self.addCleanup(self.frontendpatch.stop)
        self.envpatch=patch.dict(os.environ,self.env,clear=True);self.envpatch.start();self.addCleanup(self.envpatch.stop)

    def fake_compile(self,command,**kwargs):
        if '-emit-llvm' in command:
            output=Path(command[command.index('-o')+1]);output.write_bytes(b'ir')
            dep=Path(command[command.index('-MF')+1])
            source=str(self.root/'src'/self.name).replace(' ','\\ ')
            header=str(self.root/'src/Contrib/Contrib/lua/src/h0.h').replace(' ','\\ ')
            dep.write_text('target: '+source+' '+header+' /usr/include/string.h\n')
        else:Path(command[command.index('-o')+1]).write_bytes(b'object')
        return subprocess.CompletedProcess(command,0)

    def run_compile(self,side_effect=None):
        with patch.object(owner.subprocess,'run',side_effect=side_effect or self.fake_compile) as run:
            owner.compile_owner(self.argv,self.name,self.root,self.cwd)
            return run

    def test_selected_owner_preserves_dependency_target_and_outputs(self):
        run=self.run_compile();self.assertEqual(run.call_count,2)
        frontend=run.call_args_list[0].args[0];backend=run.call_args_list[1].args[0]
        self.assertEqual(frontend[frontend.index('-MT')+1],'target with spaces')
        self.assertEqual(frontend[frontend.index('-MF')+1],str(self.depfile))
        self.assertEqual(frontend[frontend.index('-I/work/src')+1],'-I/work/include')
        self.assertIn('-experiment-preserve-o0-dead-frame-loads=true',backend)
        self.assertEqual(self.output.read_bytes(),b'object')
        self.assertEqual(Path(str(self.output)+'.ll').read_bytes(),b'ir')
        meta=json.loads(Path(str(self.output)+'.commands.json').read_text())
        self.assertEqual(meta['incoming'],self.argv);self.assertFalse(meta['canonical']);self.assertFalse(meta['accepted'])
        self.assertIn('src/'+self.name,meta['dependencies'])

    def test_other_owner_is_exact_passthrough(self):
        argv=['/compiler','-weird-unchanged','-c','/other/unrelated.c']
        with patch.object(owner.os,'execvp',side_effect=RuntimeError('exec')) as call:
            with self.assertRaisesRegex(RuntimeError,'exec'):owner.main(argv)
        call.assert_called_once_with(argv[0],argv)

    def test_lgc_uses_no_experimental_frame_load_flag(self):
        self.name='Contrib/Contrib/lua/src/lgc.c'
        self.output=self.cwd/'CMakeFiles'/'lgc.c.obj'
        self.depfile=self.cwd/'CMakeFiles'/'lgc.c.obj.d'
        self.argv=[x.replace('lstate.c.obj','lgc.c.obj').replace('lstate.c','lgc.c') for x in self.argv]
        run=self.run_compile();backend=run.call_args_list[1].args[0]
        self.assertNotIn('-experiment-preserve-o0-dead-frame-loads=true',backend)
        self.assertEqual(self.output.read_bytes(),b'object')

    def test_rejects_unknown_flags_response_alias_and_duplicate_source(self):
        for extra in ('-funsafe-math-optimizations','@injected.rsp','-O2'):
            with self.subTest(extra=extra), self.assertRaises(ValueError):owner.parse_command(self.argv+[extra],self.name,self.root,self.cwd)
        with self.assertRaises(ValueError):owner.parse_command(self.argv+['-o','other'],self.name,self.root,self.cwd)
        with self.assertRaises(ValueError):owner.owner_for(self.argv+[str(self.root/'src'/self.name)],self.root,self.cwd)
        with self.assertRaises(ValueError):owner.owner_for(['/clang','-c','/tmp/else/src/'+self.name],self.root,self.cwd)
        alias=self.root/'alias.c';alias.symlink_to(self.root/'src'/self.name)
        with self.assertRaisesRegex(ValueError,'alias'):owner.owner_for(['/clang','-c',str(alias)],self.root,self.cwd)
        alias2=self.root/'alias_without_extension';alias2.symlink_to(self.root/'src'/self.name)
        with self.assertRaisesRegex(ValueError,'alias'):owner.owner_for(['/clang','-c',str(alias2)],self.root,self.cwd)
        with self.assertRaisesRegex(ValueError,'routing'):owner.owner_for(['/clang','@hidden.rsp'],self.root,self.cwd)
        with self.assertRaisesRegex(ValueError,'routing'):owner.owner_for(['/clang','-x','c','hidden'],self.root,self.cwd)

    def test_configure_dependency_query_is_packet_authenticated_and_headers_only(self):
        with patch('sys.stdout',new_callable=io.StringIO) as output:
            owner.main(['--dependencies',str(self.packet)])
        lines=output.getvalue().splitlines()
        self.assertEqual(len(lines),25)
        self.assertEqual(lines,sorted(lines))
        self.assertTrue(all(x.endswith('.h') for x in lines))
        self.packet.write_bytes(b'tampered')
        with self.assertRaisesRegex(ValueError,'unreviewed'):owner.main(['--dependencies',str(self.packet)])

    def test_missing_or_tampered_packet_compiler_and_header_stop_before_compile(self):
        for change in ('packet','binary','header','missing_packet'):
            with self.subTest(change=change):
                if change=='packet':self.packet.write_bytes(b'tampered')
                elif change=='binary':self.binary.write_bytes(b'tampered')
                elif change=='header':(self.root/'src/Contrib/Contrib/lua/src/h0.h').write_bytes(b'tampered')
                else:os.environ.pop('D3_LUA_OWNER_PACKET',None)
                with patch.object(owner.subprocess,'run') as run, self.assertRaises((ValueError,OSError)):
                    owner.compile_owner(self.argv,self.name,self.root,self.cwd)
                run.assert_not_called()
                self.setUp()

    def test_failed_backend_removes_stale_object_and_sidecar(self):
        self.output.write_bytes(b'old');side=Path(str(self.output)+'.commands.json');side.write_text('old')
        def fail(command,**kwargs):
            if '-emit-llvm' in command:return self.fake_compile(command,**kwargs)
            raise subprocess.CalledProcessError(1,command)
        with self.assertRaises(subprocess.CalledProcessError):self.run_compile(fail)
        self.assertFalse(self.output.exists());self.assertFalse(side.exists());self.assertFalse(self.depfile.exists())

    def test_failed_frontend_and_mismatched_outputs_do_not_publish(self):
        for change in ('frontend','ir','object'):
            with self.subTest(change=change):
                def fail(command,**kwargs):
                    if change=='frontend' and '-emit-llvm' in command:
                        raise subprocess.CalledProcessError(1,command)
                    result=self.fake_compile(command,**kwargs)
                    if change=='ir' and '-emit-llvm' in command:
                        Path(command[command.index('-o')+1]).write_bytes(b'wrong')
                    if change=='object' and '-filetype=obj' in command:
                        Path(command[command.index('-o')+1]).write_bytes(b'wrong')
                    return result
                with self.assertRaises((ValueError,subprocess.CalledProcessError)):self.run_compile(fail)
                self.assertFalse(self.output.exists())
                self.assertFalse(Path(str(self.output)+'.ll').exists())
                self.assertFalse(Path(str(self.output)+'.commands.json').exists())
                self.assertFalse(self.depfile.exists())

    def test_symlink_source_or_output_is_rejected(self):
        actual=self.root/'src'/self.name
        renamed=actual.with_name('actual.c');actual.rename(renamed);actual.symlink_to(renamed)
        with self.assertRaisesRegex(ValueError,'symlink'):owner.owner_for(self.argv,self.root,self.cwd)
        actual.unlink();renamed.rename(actual)
        self.output.symlink_to(self.root/'llc')
        with self.assertRaisesRegex(ValueError,'symlink'):owner.parse_command(self.argv,self.name,self.root,self.cwd)

    def test_unreviewed_project_dependency_prevents_object_publication(self):
        def unknown(command,**kwargs):
            result=self.fake_compile(command,**kwargs)
            if '-emit-llvm' in command:
                with self.depfile.open('a') as stream:stream.write(str(self.root/'src'/'new.h').replace(' ','\\ '))
            return result
        with self.assertRaisesRegex(ValueError,'unreviewed project dependency'):self.run_compile(unknown)
        self.assertFalse(self.output.exists())

    def test_rejects_environment_contamination(self):
        os.environ['CPATH']='/unreviewed'
        with self.assertRaisesRegex(ValueError,'environment'):owner.parse_command(self.argv,self.name,self.root,self.cwd)

    def test_frontend_symlink_must_resolve_to_pinned_file_and_hash(self):
        compiler=self.root/'clang-real';compiler.write_bytes(b'clang')
        link=self.root/'clang';link.symlink_to(compiler)
        with patch.multiple(owner,FRONTEND_LINK=link,FRONTEND_FILE=compiler,FRONTEND_SHA256=sha(b'clang')):
            self.assertEqual(real_check_frontend(),compiler)
            compiler.write_bytes(b'changed')
            with self.assertRaisesRegex(ValueError,'frontend compiler differs'):real_check_frontend()
            compiler.write_bytes(b'clang');other=self.root/'other';other.write_bytes(b'clang')
            link.unlink();link.symlink_to(other)
            with self.assertRaisesRegex(ValueError,'frontend compiler differs'):real_check_frontend()


if __name__=='__main__':unittest.main()
