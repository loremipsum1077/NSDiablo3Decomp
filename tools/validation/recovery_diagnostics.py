"""Conservative evidence for layout-only wrong-call diagnostic changes.

Raw checker records are never rewritten. Only named, direct B/BL failures from
Viking's ReferenceDiff display grammar are supported. Each moving address is
resolved in its own retained linked ELF; unchanged caller objects bind relocation
kinds/addends/targets. Unknown formats and ambiguous evidence fail closed.
"""
import hashlib
import re
import struct
from collections import defaultdict

from .elf import Elf
from . import common as c, strict

_CALL = re.compile(
    r'mismatch at (?P<site>0x[0-9a-f]+): wrong function call\n'
    r'incorrect reference; expected to see (?P<original>0x[0-9a-f]+) (?P<expected_name>[^\n]+)\n'
    r'--> decomp source code is referencing (?P<actual>0x[0-9a-f]+) (?P<actual_name>[^\n]+)\n'
    r'(?:--> expected to see (?P<expected>0x[0-9a-f]+) to match original code\n)?')
_NAME = re.compile(r'[A-Za-z_][A-Za-z_0-9.$]*')


def symbol_name(display):
    # ui::format_symbol_name prints either a plain C name or demangled (mangled).
    match = re.fullmatch(r'[^\n]+ \((_[A-Za-z_0-9.$]+)\)', display)
    name = match[1] if match else display
    return name if _NAME.fullmatch(name) and name != 'unknown' else None


def parse(row):
    if row['status'] != 'mismatch':
        return None
    match = _CALL.fullmatch(row['reason'])
    if not match:
        return None
    value = match.groupdict()
    for key in ('actual_name', 'expected_name'):
        value[key + '_symbol'] = symbol_name(value[key])
        if not value[key + '_symbol']:
            return None
    for key in ('site', 'original', 'actual', 'expected'):
        value[key] = int(value[key], 16) if value[key] else None
    return value


class Image:
    def __init__(self, path):
        self.elf = e = Elf(path)
        if e.d[:6] != b'\x7fELF\x02\x01' or struct.unpack_from('<H', e.d, 18)[0] != 183:
            raise ValueError('not ELF64 little-endian AArch64')
        self.functions = defaultdict(list)
        self.defined = set()
        tables = [s for s in e.sections if s[1] == 2]
        if len(tables) != 1 or tables[0][9] != 24 or tables[0][5] % 24:
            raise ValueError('unique supported symbol table required')
        table = tables[0]; names = e.sections[table[6]]
        self.definitions = defaultdict(list)
        for off in range(table[4], table[4] + table[5], 24):
            n, info, other, idx, value, size = struct.unpack_from('<IBBHQQ', e.d, off)
            at = names[4] + n; name = e.d[at:e.d.index(b'\0', at)].decode()
            if idx:
                self.defined.add(name)
                symbol = {'name': name, 'value': value, 'size': size, 'section': idx,
                          'type': info & 15, 'binding': info >> 4, 'visibility': other}
                self.definitions[name].append(symbol)
                if info & 15 == 2 and idx < len(e.sections):
                    self.functions[name].append(symbol)
        strings = e.sections[struct.unpack_from('<H', e.d, 0x3e)[0]]
        sections = {}
        for s in e.sections:
            at = strings[4] + s[0]
            name = e.d[at:e.d.index(b'\0', at)].decode()
            if name in sections:
                raise ValueError('duplicate section')
            sections[name] = s
        self.plt = defaultdict(list)
        if '.rela.plt' not in sections:
            return
        rel = sections['.rela.plt']; plt = sections['.plt']
        dyn = e.sections[rel[6]]; ds = e.sections[dyn[6]]
        if rel[1] != 4 or rel[9] != 24 or rel[5] % 24 or dyn[1] != 11 or dyn[9] != 24:
            raise ValueError('unsupported PLT relocation table')
        for i, off in enumerate(range(rel[4], rel[4] + rel[5], 24)):
            got, info, addend = struct.unpack_from('<QQq', e.d, off)
            symbol_index = info >> 32
            if symbol_index * 24 >= dyn[5]:
                raise ValueError('PLT symbol out of bounds')
            n, si, other, idx, val, size = struct.unpack_from('<IBBHQQ', e.d, dyn[4] + symbol_index * 24)
            at = ds[4] + n; name = e.d[at:e.d.index(b'\0', at)].decode()
            addr = plt[3] + 32 + 16 * i
            if 32 + 16 * (i + 1) > plt[5] or info & 0xffffffff != 1026 or addend != 0 or idx != 0:
                raise ValueError('unsupported PLT entry')
            code = e.read_vaddr(addr, 16)
            if len(code) != 16:
                raise ValueError('missing PLT code')
            a, l, d, b = struct.unpack('<IIII', code)
            # AArch64 ABI PLT: adrp x16; ldr x17,[x16,#lo]; add x16,x16,#lo; br x17.
            if a & 0x9f00001f != 0x90000010 or l & 0xffc003ff != 0xf9400211 or d & 0xffc003ff != 0x91000210 or b != 0xd61f0220:
                raise ValueError('unsupported PLT instructions')
            imm = ((a >> 29) & 3) | (((a >> 5) & 0x7ffff) << 2)
            imm -= (1 << 21) if imm & (1 << 20) else 0
            page = (addr & ~4095) + (imm << 12)
            if page + ((l >> 10) & 4095) * 8 != got or page + ((d >> 10) & 4095) != got:
                raise ValueError('PLT slot does not resolve relocation')
            # LLD imports may be STT_NOTYPE; the JUMP_SLOT and stub authenticate
            # their call use. Preserve the exact type/binding in the witness.
            if si & 15 not in (0, 2) or si >> 4 not in (1, 2) or other != 0 or val != 0 or size != 0:
                raise ValueError('unsupported PLT symbol binding/type/visibility')
            self.plt[name].append({'address': addr, 'binding': si >> 4, 'type': si & 15, 'visibility': other})

    def endpoint(self, name, address):
        funcs = self.functions.get(name, [])
        if name in self.defined:
            if len(self.definitions[name]) != 1 or len(funcs) != 1 or funcs[0]['value'] != address or funcs[0]['binding'] not in (0, 1, 2) or funcs[0]['visibility'] not in (0, 2):
                return None
            return {'kind': 'defined-function', 'symbol': name, **{k: funcs[0][k] for k in ('size', 'type', 'binding', 'visibility')}}
        slots = self.plt.get(name, [])
        if len(slots) == 1 and slots[0]['address'] == address:
            return {'kind': 'plt', 'symbol': name, 'relocation_type': 1026, 'addend': 0, **{k: slots[0][k] for k in ('type', 'binding', 'visibility')}}
        return None

    def absent(self, name):
        return name not in self.defined and not self.plt.get(name)

    def call(self, name, offset, actual):
        funcs = self.functions.get(name, [])
        if len(self.definitions.get(name, [])) != 1 or len(funcs) != 1 or offset < 0 or offset % 4 or offset + 4 > funcs[0]['size']:
            return None
        at = funcs[0]['value'] + offset
        code = self.elf.read_vaddr(at, 4)
        if len(code) != 4:
            return None
        word, = struct.unpack('<I', code)
        if word >> 26 not in (0b000101, 0b100101):
            return None
        delta = word & 0x3ffffff
        delta -= (1 << 26) if delta & (1 << 25) else 0
        if at + delta * 4 != actual:
            return None
        caller = self.endpoint(name, funcs[0]['value'])
        return {'opcode': word >> 26, 'caller': caller} if caller else None


def runtime_digest(path):
    """sha256 of a relocatable object's link-relevant content.

    Every section keeps its index, name and header (except sh_offset). Bytes are
    bound for all sections except non-allocated .debug_* PROGBITS sections and
    the .rela.debug_* tables that patch only them; those may change size/bytes.
    So SHF_ALLOC bytes, runtime relocations, .symtab/.strtab, groups and any
    other non-debug section stay exact. Unknown shapes raise ValueError.
    """
    elf = Elf(path); d = elf.d
    if d[:6] != b'\x7fELF\x02\x01' or struct.unpack_from('<HHIQQ', d, 16)[0] != 1 or struct.unpack_from('<H', d, 0x38)[0]:
        raise ValueError('relocatable ELF64 little-endian object without program headers required')
    strings = elf.sections[struct.unpack_from('<H', d, 0x3e)[0]]
    names = [d[strings[4] + s[0]:d.index(b'\0', strings[4] + s[0])].decode() for s in elf.sections]
    tables = [i for i, s in enumerate(elf.sections) if s[1] == 2]
    def debug(i):
        name, s = names[i], elf.sections[i]
        if s[2] & 2:  # SHF_ALLOC is always runtime content
            return False
        if s[1] == 1 and name.startswith('.debug_'):
            return True
        return (s[1] == 4 and name == '.rela' + names[s[7]] and s[7] != i and s[6] in tables
                and not elf.sections[s[7]][2] & 2 and elf.sections[s[7]][1] == 1 and names[s[7]].startswith('.debug_'))
    sections = []
    for i, s in enumerate(elf.sections):
        if s[1] != 8 and s[4] + s[5] > len(d):
            raise ValueError('section extends past end of object')
        header = {'name': names[i], 'type': s[1], 'flags': s[2], 'addr': s[3], 'link': s[6],
                  'info': s[7], 'align': s[8], 'entsize': s[9]}
        if debug(i):
            header['debug'] = True
        else:
            header['size'] = s[5]
            header['sha256'] = hashlib.sha256(d[s[4]:s[4] + s[5]] if s[1] != 8 else b'').hexdigest()
        sections.append(header)
    # ELF header without e_shoff, which moves when debug sections grow.
    return c.digest({'header': (d[:0x28] + d[0x30:0x40]).hex(), 'sections': sections})


class CallFailureEvidence:
    def __init__(self, roots):
        self.roots = roots
        self.images = None
        self.objects = None
        self.witnesses = []

    def prepare(self):
        if self.images is not None:
            return
        self.images = {label: Image(root/'build/2.6.2/d3') for label, root in self.roots.items()}
        self.objects = {}
        for label, root in self.roots.items():
            found = defaultdict(list)
            for path in (root/'objects').rglob('*.obj'):
                # Bind runtime content only; debug-only header edits may differ.
                digest = runtime_digest(path)
                elf = Elf(path)
                if sum(s[1] == 2 for s in elf.sections) != 1:
                    raise ValueError('ambiguous object symbol tables')
                for symbol in strict.symbols(elf).values():
                    if not symbol['section']:
                        continue
                    name = symbol['name']
                    found[name].append((str(path.relative_to(root/'objects')), digest))
            self.objects[label] = found

    def equivalent(self, before, after):
        a, b = parse(before), parse(after)
        if not a or not b or (before['address'], before['name']) != (after['address'], after['name']):
            return False
        stable = ('site', 'original', 'expected_name', 'actual_name', 'expected_name_symbol', 'actual_name_symbol')
        if any(a[k] != b[k] for k in stable) or (a['expected'] is None) != (b['expected'] is None):
            return False
        try:
            self.prepare()
            caller = before['name']; old = self.objects['baseline'].get(caller, [])
            if len(old) != 1 or old != self.objects['candidate'].get(caller, []):
                return False
            offset = a['site'] - (int(before['address'], 16) & 0xffffffff)
            proofs = []
            for label, diagnostic in (('baseline', a), ('candidate', b)):
                image = self.images[label]
                actual = image.endpoint(diagnostic['actual_name_symbol'], diagnostic['actual'])
                if actual is None:
                    return False
                expected_name = diagnostic['expected_name_symbol']
                expected = image.endpoint(expected_name, diagnostic['expected']) if diagnostic['expected'] is not None else None
                if (diagnostic['expected'] is not None and expected is None) or (diagnostic['expected'] is None and not image.absent(expected_name)):
                    return False
                opcode = image.call(caller, offset, diagnostic['actual'])
                if opcode is None:
                    return False
                proofs.append({'actual': actual, 'expected': expected, 'opcode': opcode})
            if proofs[0] != proofs[1]:
                return False
        except (ValueError, KeyError, IndexError, OSError, struct.error):
            return False
        self.witnesses.append({'address': before['address'], 'name': before['name'],
            'kind': 'unchanged-direct-call-failure', 'original_site': a['site'],
            'original_target': a['original'], 'caller_object': old[0][0],
            'caller_object_runtime_sha256': old[0][1], 'resolved': proofs[0],
            'before': before, 'after': after})
        return True
