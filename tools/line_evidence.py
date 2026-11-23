#!/usr/bin/env python3
"""One function's original disassembly, resolved: python3 tools/line_evidence.py NAME|0xADDR.

Every instruction carries the source file:line the compiler recorded for it (an
inlined helper shows up as its own file), every branch target is named from
functions.csv, and every adrp/add or adrp/ldr data reference is resolved to
data_symbols.csv with the original bytes (strings and small scalars inline,
tables summarized, GOT slots followed to their target). A final "constants"
section lists DWARF const_value entries (enumerators, static const members and
variables) that the function's compile unit records for its own file, for the
files its instructions come from, for the enum types it uses, and for any
other value that appears as an immediate in the code.

Reads only the original binary, its DWARF and the generated data/ tables; it
never compiles. Bounded to functions of at most 16 KiB like pilot_evidence.py.
"""
import argparse
import bisect
import csv
import posixpath
import re
import shutil
import struct
import subprocess
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import data as data_tool
import dwarf_fn
import dwarf_index
from validation import elf as elf_reader

ROOT = Path(__file__).resolve().parents[1]
DATA = ROOT / 'data/2.6.2'
NSS = ROOT / 'build/romfs/DiabloIIINX64r.nss'
BASE = 0x7100000000
LIMIT = 16384
MAX_DATA = 64          # resolved data references listed in full
MAX_CONSTANT_LINES = 300
MAX_ENUMERATORS = 48   # per enum before the list is elided
MAX_VALUE_MATCHES = 40 # constants listed only because their value is an immediate
INSN = re.compile(r'^\s*([0-9a-f]+):\s*(\S+)\s*(.*?)\s*$')
LOCATION = re.compile(r'^; (.+?):(\d+)(?::\d+)?$')
BRANCHES = ('b', 'bl', 'cbz', 'cbnz', 'tbz', 'tbnz')
SHIFTS = ('lsl', 'lsr', 'asr', 'ror', 'ubfx', 'sbfx', 'bfi', 'bfxil', 'ubfiz', 'sbfiz', 'extr', 'ccmp', 'ccmn')
MEMORY = ('ldr', 'ldrb', 'ldrh', 'ldrsb', 'ldrsh', 'ldrsw', 'ldur', 'ldp', 'ldnp',
          'str', 'strb', 'strh', 'stur', 'stp', 'stnp', 'prfm')
RELOC_RELATIVE = 1027


# ---------------------------------------------------------------- inventory

def inventory():
    funcs = {}
    for r in csv.DictReader((DATA / 'functions.csv').open()):
        funcs[int(r['Address'], 16) & 0xFFFFFFFF] = r
    files = {}
    for r in csv.DictReader((DATA / 'function_files.csv').open()):
        files[int(r['Address'], 16) & 0xFFFFFFFF] = r['File']
    return funcs, files


def resolve(arg, funcs):
    """The functions.csv row for a mangled name or a 0x address."""
    if arg.lower().startswith('0x'):
        row = funcs.get(int(arg, 16) & 0xFFFFFFFF)
        if not row:
            sys.exit(f'{arg}: no function starts there (functions.csv)')
        return row
    rows = [r for r in funcs.values() if r['Name'] == arg]
    if len(rows) != 1:
        sys.exit(f'{arg}: {"not in functions.csv" if not rows else "ambiguous name; give the 0x address"}')
    return rows[0]


class Symbols:
    """Name lookup over data_symbols.csv + functions.csv, as tools/data.py does it."""

    def __init__(self, funcs):
        self.syms = data_tool.symbols()
        self.addrs = [a for a, _ in self.syms]
        self.funcs = funcs

    def at(self, a):
        """(symbol name, offset) for an address, or (None, 0) outside every symbol.
        Past the end of a function there is no symbol: string pools are not part of it."""
        i = bisect.bisect_right(self.addrs, a) - 1
        if i < 0:
            return None, 0
        base = self.addrs[i]
        func = self.funcs.get(base)
        if func and a - base >= int(func['Size']):
            return None, 0
        return self.syms[i][1], a - base

    def name(self, a):
        name, off = self.at(a)
        if name is None:
            return '(no symbol)'
        return name + (f'+0x{off:x}' if off else '')

    def extent(self, a):
        """Bytes from a to the next known symbol, at most 256 (tools/data.py's default)."""
        i = bisect.bisect_right(self.addrs, a)
        return min(self.addrs[i] - a, 256) if i < len(self.addrs) else 64


def dynamic_symbols(elf):
    """Names of the dynamic symbol table by index, for non-relative relocations."""
    out = []
    for s in elf.sections:
        if s[1] != 11:  # SHT_DYNSYM
            continue
        stroff = elf.sections[s[6]][4]
        for o in range(s[4], s[4] + s[5], 24):
            name = struct.unpack_from('<I', elf.d, o)[0]
            end = elf.d.index(b'\0', stroff + name)
            out.append(elf.d[stroff + name:end].decode(errors='replace'))
    return out


def relocations(elf):
    """{offset: (type, target address or symbol index)} for every dynamic relocation."""
    out = {}
    for s in elf.sections:
        if s[1] == 4:  # SHT_RELA
            for o in range(s[4], s[4] + s[5], 24):
                r_off, r_info, addend = struct.unpack_from('<QQq', elf.d, o)
                kind = r_info & 0xFFFFFFFF
                out[r_off & 0xFFFFFFFF] = (kind, addend & 0xFFFFFFFF if kind == RELOC_RELATIVE else r_info >> 32)
    return out


# ---------------------------------------------------------------- disassembly

def objdump(start, end):
    exe = shutil.which('llvm-objdump') or '/opt/homebrew/opt/llvm/bin/llvm-objdump'
    p = subprocess.run([exe, '-d', '-l', '--no-show-raw-insn', f'--start-address={start}',
                        f'--stop-address={end}', str(NSS)], capture_output=True, text=True)
    if p.returncode:
        sys.exit(p.stderr.strip() or 'llvm-objdump failed')
    return p.stdout


def norm_file(raw):
    return posixpath.normpath(dwarf_index.norm_path(raw))


def parse(text):
    """[{address, mnemonic, operands, file, line}] from `llvm-objdump -d -l` output."""
    out, where = [], (None, 0)
    for line in text.splitlines():
        m = LOCATION.match(line)
        if m:
            where = (norm_file(m.group(1)), int(m.group(2)))
            continue
        m = INSN.match(line)
        if m and not line.endswith(':'):
            ops = re.sub(r'\s*<[^>]*>', '', m.group(3))
            ops = re.sub(r'\s*//.*$', '', ops)
            out.append({'address': int(m.group(1), 16), 'mnemonic': m.group(2), 'operands': ops,
                        'file': where[0], 'line': where[1]})
    return out


def register(op):
    m = re.fullmatch(r'([wx])(\d+|zr)', op.strip())
    return m.group(2) if m else None


def operands(insn):
    return [o.strip() for o in re.split(r',(?![^\[]*\])', insn['operands']) if o.strip()]


def branch_target(insn):
    if insn['mnemonic'].split('.')[0] not in BRANCHES:
        return None
    m = re.search(r'0x([0-9a-f]+)\s*$', insn['operands'])
    return int(m.group(1), 16) if m else None


def data_references(insns):
    """{index: address} for every adrp paired with a following add or memory access."""
    out = {}
    for i, insn in enumerate(insns):
        if insn['mnemonic'] != 'adrp':
            continue
        ops = operands(insn)
        if len(ops) != 2 or not ops[1].startswith('0x'):
            continue
        reg, page = register(ops[0]), int(ops[1], 16)
        for j in range(i + 1, min(i + 8, len(insns))):
            use = insns[j]
            uops = operands(use)
            if not uops:
                continue
            if use['mnemonic'] == 'add' and len(uops) == 3 and register(uops[1]) == reg and uops[2].startswith('#'):
                out[j] = page + int(uops[2][1:], 0)
                break
            if use['mnemonic'] in MEMORY:
                m = re.search(r'\[([wx]\d+)(?:,\s*#(-?0x[0-9a-f]+|-?\d+))?\]', use['operands'])
                if m and register(m.group(1)) == reg:
                    out[j] = page + (int(m.group(2), 0) if m.group(2) else 0)
                    break
            mnem = use['mnemonic'].split('.')[0]
            if register(uops[0]) == reg and not (mnem.startswith('st') or mnem in BRANCHES or mnem in ('cmp', 'cmn', 'tst', 'ccmp', 'ccmn')):
                break  # the page register was overwritten before any use
    return out


def immediates(insns):
    """Immediate values that could be named constants: operands of compares, moves and
    arithmetic/logic, not addresses, memory offsets, stack adjustments, shifts or bit numbers."""
    out = set()
    for insn in insns:
        mnem = insn['mnemonic'].split('.')[0]
        if mnem in BRANCHES or mnem in MEMORY or mnem in ('adrp', 'adr') or re.search(r'\bsp\b', insn['operands']):
            continue
        ops = operands(insn)
        if mnem in SHIFTS:
            ops = ops[1:2] if mnem in ('ccmp', 'ccmn') else []  # shift amounts and flag masks are not constants
        for op in ops:
            if op.startswith('[') or re.match(r'(lsl|lsr|asr|ror|uxt\w|sxt\w)\b', op):
                continue
            m = re.fullmatch(r'#(-?0x[0-9a-f]+|-?\d+)', op)
            if not m:
                continue
            value = int(m.group(1), 0)
            if mnem in ('cmn', 'sub', 'subs'):
                out.add(-value)
            if mnem == 'movn':
                out.add(~value)
            if value not in (0, 1):
                out.add(value)
    return out


# ---------------------------------------------------------------- data values

class Values:
    def __init__(self, syms, funcs):
        self.syms, self.funcs = syms, funcs
        self.elf = elf_reader.Elf(DATA / 'main.elf')
        self.relocs = relocations(self.elf)
        self.dynsyms = dynamic_symbols(self.elf)

    def pointer_text(self, slot):
        reloc = self.relocs.get(slot)
        if not reloc:
            return None
        kind, value = reloc
        if kind == RELOC_RELATIVE:
            text = data_tool.text_at(self.elf, value)
            return f'-> 0x{BASE + value:x} ' + (f'"{text}"' if text else self.syms.name(value))
        name = self.dynsyms[value] if value < len(self.dynsyms) else ''
        return f'-> {name or "dynamic symbol #" + str(value)} (relocation type {kind})'

    def describe(self, addr, via_load):
        """(short note, detail lines) for one data reference, in tools/data.py's style."""
        name, off = self.syms.at(addr)
        label = self.syms.name(addr)
        pointer = self.pointer_text(addr) if via_load else None
        if pointer:
            detail = [f'0x{BASE + addr:x}  {label}  pointer slot {pointer}']
            reloc = self.relocs.get(addr)
            if reloc[0] == RELOC_RELATIVE and reloc[1] not in self.funcs:
                detail += ['  ' + line for line in self.describe(reloc[1], False)[1]]
            return f'{label} {pointer}', detail
        if addr in self.funcs:
            row = self.funcs[addr]
            return f'function {row["Name"]}', [f'0x{BASE + addr:x}  function {row["Name"]} ({row["Size"]} bytes)']
        size = self.syms.extent(addr)
        raw = self.elf.read_vaddr(addr, size)
        if not raw:
            return f'{label} (.bss, no file bytes)', [f'0x{BASE + addr:x}  {label}  no file contents (.bss)']
        text = data_tool.text_at(self.elf, addr, 200)
        if text is not None and len(text) >= 2:
            return f'{label} "{text}"', [f'0x{BASE + addr:x}  {label}  "{text}"']
        inner = sum(1 for o in range(0, size - 7, 8) if addr + o in self.relocs)
        if size <= 16 and not inner:
            words = [struct.unpack_from('<i', raw, i)[0] for i in range(0, len(raw) - 3, 4)]
            floats = [struct.unpack_from('<f', raw, i)[0] for i in range(0, len(raw) - 3, 4)]
            note = f'{label} = {raw.hex(" ", 4)}'
            if words:
                note += f'  int {" ".join(str(w) for w in words)}  float {" ".join(f"{f:.6g}" for f in floats)}'
            return note, [f'0x{BASE + addr:x}  {label}  ({size} bytes)'] + data_tool.rows(raw)
        head = [f'0x{BASE + addr:x}  {label}  table, {size} bytes to the next symbol'
                + (' (256-byte cap)' if size == 256 else '') + (f', {inner} relocated pointers' if inner else '')]
        if inner:
            for o in range(0, min(size, 64) - 7, 8):
                if addr + o in self.relocs:
                    head.append(f'  +0x{o:03x}  {self.pointer_text(addr + o)}')
            if size > 64:
                head.append('  ... python3 tools/data.py ' + (name or f'0x{BASE + addr:x}') + ' --ptr for the rest')
        else:
            head += data_tool.rows(raw[:32])
            if size > 32:
                head.append('  ... python3 tools/data.py ' + (name or f'0x{BASE + addr:x}') + ' for the rest')
        return f'{label} table ({size} bytes)', head


# ---------------------------------------------------------------- constants

def die_name(dies, die):
    return dwarf_fn.attr(dies, die, 'name').strip('"')


def die_file(dies, die):
    raw = dwarf_fn.attr(dies, die, 'decl_file').strip('"')
    return norm_file(raw) if raw else ''


def parse_value(text):
    m = re.fullmatch(r'-?\d+', text.strip())
    return int(text) if m else None


def value_text(text, type_name=''):
    value = parse_value(text)
    if value is None:
        return text
    if 'float' in type_name and 0 <= value < 1 << 32:
        return f'{struct.unpack("<f", struct.pack("<I", value))[0]:g}f (0x{value:08x})'
    if 'double' in type_name and 0 <= value < 1 << 64:
        return f'{struct.unpack("<d", struct.pack("<Q", value))[0]:g} (0x{value:016x})'
    out = str(value)
    if value >= 1 << 63:
        out += f' (signed {value - (1 << 64)})'
    elif 0xFFFF < value < 1 << 32 or value < 0:
        out += f' (0x{value & 0xFFFFFFFF:x})'
    elif value > 1 << 32:
        out += f' (0x{value:x})'
    return out


def subtree_types(dies, die, out):
    for kid in die['kids']:
        if kid['tag'] in ('formal_parameter', 'variable', 'inlined_subroutine', 'lexical_block'):
            if kid['tag'] in ('formal_parameter', 'variable'):
                out.update(re.findall(r'[A-Za-z_]\w*', dwarf_fn.type_name(dies, kid)))
            subtree_types(dies, kid, out)
    return out


def subtree_offsets(die, out):
    out.add(die['off'])
    for kid in die['kids']:
        subtree_offsets(kid, out)
    return out


def constants(vaddr, insn_files, values):
    """Lines for the constants section, grouped by how they relate to the function."""
    try:
        dies = dwarf_fn.load_unit(vaddr)
    except SystemExit as exc:
        return [f'  (no DWARF: {exc})']
    unit = next((d for d in dies.values() if d['tag'] == 'compile_unit'), None)
    fn = next((d for d in dies.values() if d['tag'] == 'subprogram' and 'high_pc' in d['attrs']
               and int(d['attrs'].get('low_pc', '0x0'), 16) == vaddr), None)
    if fn is None:
        return ['  (no subprogram starts here in the DWARF)']
    own = {die_file(dies, fn)}
    if unit is not None:
        own.add(norm_file(unit['attrs'].get('name', '').strip('"')))
    own.discard('')
    used = subtree_types(dies, fn, set())
    inside = subtree_offsets(fn, set())
    groups = {'declared in this function\'s file': [], 'enum types the function uses': [],
              'declared in files its instructions come from': [], 'locals the compiler folded to constants': [],
              'values that appear as immediates': []}
    matched = []  # (specificity, line) for the value group, most specific first

    area = next(iter(own)).split('/')[:2]

    def place(file, names, entries):
        """Which group a constant belongs to; None when it is unrelated."""
        if file in own:
            return 'declared in this function\'s file'
        if names & used:
            return 'enum types the function uses'
        if file in insn_files:
            return 'declared in files its instructions come from'
        # Small values and powers of two match everywhere; count them only from the function's own area.
        if any(specific(file, v) for _, v in entries):
            return 'values that appear as immediates'
        return None

    def specific(file, text):
        value = parse_value(text)
        if value is None or value not in values:
            return False
        if file.split('/')[:2] == area:
            return True
        return abs(value) >= 32 and (abs(value) & (abs(value) - 1)) != 0

    parents = {}
    for die in dies.values():
        for kid in die['kids']:
            parents[kid['off']] = die

    def enclosing_function(die):
        p = parents.get(die['off'])
        while p is not None and p['tag'] != 'compile_unit':
            if p['tag'] in ('subprogram', 'inlined_subroutine'):
                return p
            p = parents.get(p['off'])
        return None

    def qualified(die):
        names = []
        p = parents.get(die['off'])
        while p is not None and p['tag'] != 'compile_unit':
            if p['tag'] in ('namespace', 'structure_type', 'class_type', 'union_type'):
                names.append(die_name(dies, p) or '(anonymous)')
            p = parents.get(p['off'])
        return '::'.join(reversed(names))

    aliases = {}
    for die in dies.values():
        if die['tag'] == 'typedef':
            m = re.match(r'0x([0-9a-f]+)', die['attrs'].get('type', ''))
            if m:
                aliases.setdefault(int(m.group(1), 16), set()).add(die_name(dies, die))
    for die in dies.values():
        if die['tag'] == 'enumeration_type':
            entries = [(die_name(dies, k), k['attrs'].get('const_value', '')) for k in die['kids'] if k['tag'] == 'enumerator']
            if not entries:
                continue
            name = die_name(dies, die) or '(anonymous enum)'
            full = '::'.join(filter(None, (qualified(die), name)))
            file = die_file(dies, die)
            group = place(file, {name} | aliases.get(die['off'], set()), entries)
            if group is None:
                continue
            shown = entries
            if group == 'values that appear as immediates':
                shown = [(n, v) for n, v in entries if specific(file, v)]
            elif len(entries) > MAX_ENUMERATORS:
                shown = entries[:MAX_ENUMERATORS]
            line = f'  enum {full}  ({file}:{dwarf_fn.attr(dies, die, "decl_line")})'
            body = ', '.join(f'{n}={value_text(v)}' for n, v in shown)
            if group == 'values that appear as immediates':
                body += f'  (of {len(entries)} enumerators)'
                matched.append((max(abs(parse_value(v)) for _, v in shown), line + '\n    ' + body))
                continue
            if len(shown) < len(entries):
                body += f', ... {len(entries) - len(shown)} more'
            groups[group].append(line + '\n    ' + body)
        elif die['tag'] in ('member', 'variable') and 'const_value' in die['attrs']:
            name = die_name(dies, die)
            if not name:
                continue
            file = die_file(dies, die)
            value = die['attrs']['const_value']
            where = f'({file}:{dwarf_fn.attr(dies, die, "decl_line")})'
            if die['tag'] == 'variable' and enclosing_function(die) is not None:
                # A local the optimizer proved constant; only this function's own are relevant.
                if die['off'] in inside:
                    groups['locals the compiler folded to constants'].append(
                        f'  {dwarf_fn.type_name(dies, die)} {name} = {value_text(value, dwarf_fn.type_name(dies, die))}  {where}')
                continue
            full = '::'.join(filter(None, (qualified(die), name)))
            group = place(file, set(), [(name, value)])
            if group is None:
                continue
            line = f'  {dwarf_fn.type_name(dies, die)} {full} = {value_text(value, dwarf_fn.type_name(dies, die))}  {where}'
            if group == 'values that appear as immediates':
                matched.append((abs(parse_value(value) or 0), line))
            else:
                groups[group].append(line)
    matched.sort(key=lambda item: -item[0])
    groups['values that appear as immediates'] = [line for _, line in matched[:MAX_VALUE_MATCHES]]
    if len(matched) > MAX_VALUE_MATCHES:
        groups['values that appear as immediates'].append(f'  ... {len(matched) - MAX_VALUE_MATCHES} more value matches')
    out = []
    for title, lines in groups.items():
        if lines:
            out.append(f' {title}:')
            out += lines
    if not out:
        out.append('  (none recorded in this compile unit)')
    flat = '\n'.join(out).split('\n')
    if len(flat) > MAX_CONSTANT_LINES:
        flat = flat[:MAX_CONSTANT_LINES] + [f'  ... {len(flat) - MAX_CONSTANT_LINES} more lines; python3 tools/dwarf_fn.py for the full unit']
    return flat


# ---------------------------------------------------------------- report

def report(arg):
    funcs, files = inventory()
    row = resolve(arg, funcs)
    start = int(row['Address'], 16) & 0xFFFFFFFF
    size = int(row['Size'])
    if not 0 < size <= LIMIT:
        sys.exit('Function exceeds the 16 KiB assembly bound')
    end = start + size
    insns = parse(objdump(start, end))
    syms = Symbols(funcs)
    values = Values(syms, funcs)
    refs = data_references(insns)

    file_ids, order = {}, []
    for insn in insns:
        if insn['file'] and insn['file'] not in file_ids:
            file_ids[insn['file']] = f'F{len(order)}'
            order.append(insn['file'])
    out = [f'{row["Name"]}  0x{BASE + start:x}  {size} bytes  quality {row["Quality"]}',
           f'file: {files.get(start, "?")}']
    out.append('files:')
    for f in order:
        out.append(f'  {file_ids[f]} {f}')
    out.append('instructions:')
    calls, data, seen_data = {}, [], set()
    for i, insn in enumerate(insns):
        loc = f'{file_ids[insn["file"]]}:{insn["line"]}' if insn['file'] else '-'
        note = ''
        target = branch_target(insn)
        if target is not None:
            if start <= target < end:
                note = f'-> +0x{target - start:x}'
            else:
                name, off = syms.at(target)
                callee = funcs.get(target)
                note = '-> ' + (callee['Name'] if callee else (name + (f'+0x{off:x}' if off else '') if name else '?'))
                calls.setdefault(target, callee['Name'] if callee else syms.name(target))
        elif i in refs:
            addr = refs[i]
            short, detail = values.describe(addr, insn['mnemonic'] in ('ldr', 'ldp', 'ldur'))
            note = f'-> 0x{BASE + addr:x} {short}'
            if addr not in seen_data:
                seen_data.add(addr)
                data.append(detail)
        out.append(f'  {insn["address"]:x}  {insn["mnemonic"]:<7} {insn["operands"]:<32} {loc:<8} {note}'.rstrip())
    out.append('calls:')
    for target, name in sorted(calls.items()):
        out.append(f'  0x{BASE + target:x}  {name}  {files.get(target, "")}'.rstrip())
    if not calls:
        out.append('  (none)')
    out.append('data:')
    for detail in data[:MAX_DATA]:
        out += ['  ' + line for line in detail]
    if len(data) > MAX_DATA:
        out.append(f'  ... {len(data) - MAX_DATA} more references; python3 tools/data.py <address>')
    if not data:
        out.append('  (no adrp-based data references)')
    out.append('constants:')
    out += constants(start, set(order), immediates(insns))
    return '\n'.join(out)


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('function', help='mangled name from functions.csv, or 0x<address>')
    args = ap.parse_args()
    print(report(args.function))


if __name__ == '__main__':
    main()
