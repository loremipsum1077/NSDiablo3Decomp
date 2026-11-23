import hashlib
import struct
import types
import unittest
import tempfile
from pathlib import Path
from unittest.mock import patch

from validation import strict


def section(kind=1, flags=6, size=64):
    return (0, kind, flags, 0, 0, size, 0, 0, 0, 0)


def function(name, offset, size=16):
    return {'name': name, 'section': 1, 'value': offset, 'size': size, 'type': 2}


def relocation(offset, kind, addend=0, symbol=None):
    return {'section': 2, 'offset': offset, 'type': kind,
            'symbol': symbol or {'name': '', 'section': 1, 'value': 0, 'size': 0, 'type': 3},
            'addend': addend}


def object_fixture(collision=False, crossing=False, import_symbol=None):
    """Small valid AArch64 relocatable with two functions and one local BL."""
    data = bytearray(64)
    data[:7] = b'\x7fELF\x02\x01\x01'
    struct.pack_into('<HHI', data, 16, 1, 183, 1)
    struct.pack_into('<H', data, 52, 64)
    sections = [(0,0,0,0,0,0,0,0,0,0)]
    def append(blob, kind, flags=0, link=0, info=0, entsize=0):
        while len(data)%8:data.append(0)
        offset=len(data);data.extend(blob)
        sections.append((0,kind,flags,0,offset,len(blob),link,info,8,entsize))
    text = bytearray(32);struct.pack_into('<I',text,16,0x94000000)
    append(text,1,6)
    append(bytes(8),1,0)
    append(b'\0target\0caller\0external\0',3)
    entries=[(0,0,0,0,0,0),(1,2,0,1,0,16),(8,2,0,1,16,16),(0,3,0,1,0,0)]
    if collision:entries.append((8,1,0,2,0,4))
    if import_symbol is not None:entries.append((15,*import_symbol))
    append(b''.join(struct.pack('<IBBHQQ',*e) for e in entries),2,0,3,0,24)
    rels=[(16,((4 if import_symbol is not None else 3)<<32)|283,0)]
    if crossing:rels.insert(0,(12,(3<<32)|257,0))
    append(b''.join(struct.pack('<QQq',*r) for r in rels),4,0,4,1,24)
    while len(data)%8:data.append(0)
    shoff=len(data)
    data.extend(b''.join(struct.pack('<IIQQQQIIQQ',*s) for s in sections))
    struct.pack_into('<Q',data,0x28,shoff)
    struct.pack_into('<HHH',data,0x3a,64,len(sections),3)
    return bytes(data)


class LocalReferenceTests(unittest.TestCase):
    def setUp(self):
        self.elf = types.SimpleNamespace(sections=[section(kind=0), section(), section(), section(kind=2)])
        self.funcs = [function('target', 0), function('caller', 16)]
        self.ctx = strict._OwnerContext('owner', 'a' * 64, (('caller', 0x1020, 16), ('target', 0x1000, 16)), '/', ())
        self.caller = dict(self.funcs[1], section=2, value=16)
        self.call = struct.pack('<I', 0x94000000) + bytes(12)

    def resolve(self, rel, rels, code=None, want=None, funcs=None, elf=None, context=None):
        code = self.call if code is None else code
        return strict._owner_target(elf or self.elf, funcs or self.funcs, rels, rel, self.caller,
                                    code, code if want is None else want, context or self.ctx)

    def test_section_call_resolves_exact_entry_only(self):
        rel = relocation(16, 283)
        self.assertEqual(self.resolve(rel, [rel]), 0x1000)
        for invalid in (relocation(16, 283, addend=4), relocation(16, 283, addend=-1),
                        relocation(16, 283, addend=64), relocation(16, 282),
                        relocation(17, 283), relocation(16, 283, symbol={'name':'','section':0xfff1,'value':0,'size':0,'type':3})):
            with self.subTest(invalid=invalid):
                self.assertIsNone(self.resolve(invalid, [invalid]))
        self.assertIsNone(self.resolve(rel, [rel, rel]))
        self.assertIsNone(self.resolve(rel, [rel, relocation(18, 1)]))
        self.assertIsNone(self.resolve(rel, [rel], code=struct.pack('<I', 0x14000000)+bytes(12)))
        self.assertIsNone(self.resolve(rel, [rel], want=struct.pack('<I', 0x14000000)+bytes(12)))
        self.assertIsNone(self.resolve(rel, [rel], funcs=[function('target', 0), function('alias', 0)]))
        self.assertIsNone(self.resolve(rel, [rel], context=strict._OwnerContext('wrong','a'*64,(('other',0x1000,16),),'/',())))
        self.assertIsNone(self.resolve(rel, [rel], elf=types.SimpleNamespace(sections=[section(kind=0), section(flags=0), section()])))

    def test_address_pair_requires_matching_adjacent_pair_and_registers(self):
        code = struct.pack('<II', 0x90000001, 0x91000021) + bytes(8)
        hi, lo = relocation(16, 275), relocation(20, 277)
        for r in (hi, lo):
            self.assertEqual(self.resolve(r, [hi, lo], code=code), 0x1000)
        self.assertIsNone(self.resolve(hi, [hi], code=code))
        self.assertIsNone(self.resolve(hi, [hi, relocation(20, 277, addend=1)], code=code))
        self.assertIsNone(self.resolve(hi, [hi, lo, relocation(18, 1)], code=code))
        wrong_register = struct.pack('<II', 0x90000001, 0x91000041) + bytes(8)
        self.assertIsNone(self.resolve(hi, [hi, lo], code=wrong_register))
        shifted_add = struct.pack('<II', 0x90000001, 0x91400021) + bytes(8)
        self.assertIsNone(self.resolve(hi, [hi, lo], code=shifted_add))
        self.assertIsNone(self.resolve(hi, [hi, lo], code=code, want=shifted_add))
        zero_to_stack = struct.pack('<II', 0x9000001f, 0x910003e1) + bytes(8)
        self.assertIsNone(self.resolve(hi, [hi, lo], code=zero_to_stack))
        self.assertIsNone(self.resolve(hi, [hi, lo], code=code, want=zero_to_stack))
        self.assertIsNone(self.resolve(relocation(28,275), [relocation(28,275), relocation(32,277)], code=code))

    def test_complete_owner_context_and_candidate_extents(self):
        data = b'original'; digest = hashlib.sha256(data).hexdigest()
        context = strict._OwnerContext('owner', digest, (('caller', 0x1020, 16), ('target', 0x1000, 16)), '/', ())
        targets = [{'Name':'target','Address':'0x1000','Size':'16'}, {'Name':'caller','Address':'0x1020','Size':'16'}]
        with patch.object(strict, 'orig', types.SimpleNamespace(d=data), create=True), \
             patch.object(strict, 'build_owner_context', return_value=context):
            strict._validate_owner(self.elf, {0:self.funcs[0],1:self.funcs[1]}, targets, context)
            with self.assertRaisesRegex(ValueError, 'complete owner'):
                strict._validate_owner(self.elf, {0:self.funcs[0],1:self.funcs[1]}, targets[:1], context)
            with self.assertRaisesRegex(ValueError, 'overlapping'):
                strict._validate_owner(self.elf, {0:self.funcs[0],1:function('caller', 8)}, targets, context)
            with self.assertRaisesRegex(ValueError, 'incomplete or ambiguous'):
                strict._validate_owner(self.elf, {0:self.funcs[0],1:function('target', 16)}, targets, context)
            with self.assertRaisesRegex(ValueError, 'executable extent'):
                strict._validate_owner(self.elf, {0:self.funcs[0],1:function('caller', 60)}, targets, context)
            with self.assertRaisesRegex(ValueError, 'configured original'):
                strict._validate_owner(self.elf, {0:self.funcs[0],1:self.funcs[1]}, targets, strict._OwnerContext('x','b'*64,context.members,'/',()))


class OwnerInventoryTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)
        self.data = self.root / 'data/2.6.2'
        self.data.mkdir(parents=True)
        elf = bytearray(4096)
        elf[:7] = b'\x7fELF\x02\x01\x01'
        struct.pack_into('<HHI', elf, 16, 3, 183, 1)
        struct.pack_into('<H', elf, 52, 64)
        struct.pack_into('<Q', elf, 0x20, 64)
        struct.pack_into('<HH', elf, 0x36, 56, 1)
        struct.pack_into('<IIQQQQQQ', elf, 64, 1, 5, 0, 0x1000, 0, 0x1000, 0x1000, 4)
        (self.data/'main.elf').write_bytes(elf)
        (self.data/'functions.csv').write_text('Address,Name,Size\n0x1000,target,16\n0x1020,caller,16\n')
        (self.data/'function_files.csv').write_text('Address,File,Compiler\n0x1000,owner,5\n0x1020,owner,5\n')

    def pins(self):
        return {key: hashlib.sha256((self.data/name).read_bytes()).hexdigest() for key, name in (
            ('original_sha256','main.elf'), ('functions_sha256','functions.csv'),
            ('function_files_sha256','function_files.csv'))}

    def test_factory_derives_complete_pinned_membership(self):
        pins = self.pins()
        context = strict.build_owner_context(self.root, 'owner', pins)
        self.assertEqual(context.members, (('caller',0x1020,16),('target',0x1000,16)))
        with self.assertRaisesRegex(ValueError, 'pin mismatch'):
            strict.build_owner_context(self.root, 'owner', dict(pins, functions_sha256='0'*64))
        with self.assertRaisesRegex(ValueError, 'empty'):
            strict.build_owner_context(self.root, 'wrong', pins)
        (self.data/'functions.csv').write_text('Address,Name,Size\n0x1000,target,16\n0x1020,caller,16\n0x1040,target,16\n')
        with self.assertRaisesRegex(ValueError, 'ambiguous original function'):
            strict.build_owner_context(self.root, 'owner', self.pins())

    def test_factory_rejects_wrong_owner_and_overlapping_originals(self):
        (self.data/'function_files.csv').write_text('Address,File,Compiler\n0x1000,owner,5\n0x1000,other,5\n0x1020,owner,5\n')
        with self.assertRaisesRegex(ValueError, 'ambiguous owner'):
            strict.build_owner_context(self.root, 'owner', self.pins())
        (self.data/'function_files.csv').write_text('Address,File,Compiler\n0x1000,owner,5\n0x1008,owner,5\n')
        (self.data/'functions.csv').write_text('Address,Name,Size\n0x1000,target,16\n0x1008,caller,16\n')
        with self.assertRaisesRegex(ValueError, 'overlapping original'):
            strict.build_owner_context(self.root, 'owner', self.pins())

    def test_original_partial_overlapping_load_mapping_is_rejected(self):
        elf=bytearray((self.data/'main.elf').read_bytes())
        struct.pack_into('<H',elf,0x38,2)
        struct.pack_into('<IIQQQQQQ',elf,120,1,4,0,0x1008,0,1,1,1)
        (self.data/'main.elf').write_bytes(elf)
        with self.assertRaisesRegex(ValueError,'original function mapping'):
            strict.build_owner_context(self.root,'owner',self.pins())


class EndToEndTests(unittest.TestCase):
    def test_same_name_data_symbol_does_not_hide_real_relocation(self):
        self.run_fixture(collision=True)

    def test_wide_relocation_crossing_function_boundary_fails_closed(self):
        self.run_fixture(crossing=True)

    def test_opt_in_rejects_malformed_elf_and_table_links(self):
        original=object_fixture()
        shoff=struct.unpack_from('<Q',original,0x28)[0]
        for name,offset,fmt,value in (
            ('encoding',5,'B',2),('machine',18,'H',62),
            ('rela link',shoff+5*64+40,'I',3),
            ('symtab link',shoff+4*64+40,'I',1),
            ('rela extent',shoff+5*64+32,'Q',2**32),
            ('REL section',shoff+5*64+4,'I',9),
            ('RELR section',shoff+5*64+4,'I',19),
        ):
            with self.subTest(name=name), tempfile.TemporaryDirectory() as temp:
                data=bytearray(original);struct.pack_into('<'+fmt,data,offset,value)
                path=Path(temp)/'bad.o';path.write_bytes(data)
                with self.assertRaises(ValueError):strict._object_layout(strict.Elf(path))

    def run_fixture(self, collision=False, crossing=False):
        with tempfile.TemporaryDirectory() as temp:
            path=Path(temp)/'test.o';path.write_bytes(object_fixture(collision, crossing))
            original=bytes(16)+struct.pack('<I',0x94000000)+bytes(12)
            orig=types.SimpleNamespace(d=b'original',read_vaddr=lambda addr,size: original[addr-0x1000:addr-0x1000+size])
            digest=hashlib.sha256(orig.d).hexdigest()
            ctx=strict._OwnerContext('owner',digest,(('caller',0x1010,16),('target',0x1000,16)),'/',())
            targets=[{'Name':'target','Address':'0x1000','Size':'16'}, {'Name':'caller','Address':'0x1010','Size':'16'}]
            with patch.object(strict,'orig',orig,create=True), patch.object(strict,'known',{},create=True), \
                 patch.object(strict,'build_owner_context',return_value=ctx):
                if crossing:
                    with self.assertRaisesRegex(ValueError,'crosses function boundary'):
                        strict.verify(path,targets,ctx)
                else:
                    report=strict.verify(path,targets,ctx)
                    caller=next(r for r in report['results'] if r['symbol']=='caller')
                    self.assertFalse(caller['exact'])
                    self.assertEqual(caller['reason'],'instructions')


class OriginalImportTests(unittest.TestCase):
    def check_import(self, attrs=(0x10,0,0,0,0), mutate=None, helper_error=False):
        with tempfile.TemporaryDirectory() as temp:
            data=bytearray(object_fixture(import_symbol=attrs))
            if mutate is not None:mutate(data)
            path=Path(temp)/'import.o';path.write_bytes(data)
            original=bytes(16)+struct.pack('<I',0x94000000)+bytes(12)
            orig=types.SimpleNamespace(d=b'original',read_vaddr=lambda addr,size: original[addr-0x1000:addr-0x1000+size])
            digest=hashlib.sha256(orig.d).hexdigest()
            ctx=strict._OwnerContext('owner',digest,(('caller',0x1010,16),('target',0x1000,16)),'/',())
            targets=[{'Name':'target','Address':'0x1000','Size':'16'}, {'Name':'caller','Address':'0x1010','Size':'16'}]
            witness={'plt_address':0x1010,'scope':'original only'}
            with patch.object(strict,'orig',orig,create=True), patch.object(strict,'known',{},create=True), \
                 patch.object(strict,'build_owner_context',return_value=ctx), \
                 patch('validation.imports.prove_original_call',side_effect=ValueError('unproved') if helper_error else None,return_value=witness) as prove:
                report=strict.verify(path,targets,ctx,original_imports=True)
                caller=next(r for r in report['results'] if r['symbol']=='caller')
                return caller, prove.call_count

    def test_import_proof_requires_context_and_is_explicit(self):
        with self.assertRaisesRegex(ValueError,'require an owner context'):
            strict.verify('not-opened.o',[],original_imports=True)
        caller,calls=self.check_import()
        self.assertTrue(caller['exact'])
        self.assertEqual(calls,1)
        self.assertIn('candidate link not proved',caller['relocations'][0]['proof'])
        self.assertEqual(caller['relocations'][0]['original_import_witness']['scope'],'original only')

    def test_import_rejects_weak_hidden_defined_nonzero_or_wrong_type(self):
        for attrs in ((0x20,0,0,0,0),(0x10,2,0,0,0),(0x10,0,2,0,0),
                      (0x10,0,0,1,0),(0x10,0,0,0,1),(0x11,0,0,0,0)):
            with self.subTest(attrs=attrs):
                caller,calls=self.check_import(attrs)
                self.assertFalse(caller['exact']);self.assertEqual(calls,0)
        caller,calls=self.check_import(helper_error=True)
        self.assertFalse(caller['exact']);self.assertEqual(calls,1)

    def test_import_rejects_addend_and_non_bl_opcode(self):
        def addend(data):
            shoff=struct.unpack_from('<Q',data,0x28)[0]
            rela=struct.unpack_from('<Q',data,shoff+5*64+24)[0]
            struct.pack_into('<q',data,rela+16,4)
        def wrong_opcode(data):
            shoff=struct.unpack_from('<Q',data,0x28)[0]
            text=struct.unpack_from('<Q',data,shoff+64+24)[0]
            struct.pack_into('<I',data,text+16,0x14000000)
        for mutate in (addend,wrong_opcode):
            caller,calls=self.check_import(mutate=mutate)
            self.assertFalse(caller['exact']);self.assertEqual(calls,0)



if __name__ == '__main__':
    unittest.main()
