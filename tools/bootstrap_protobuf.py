#!/usr/bin/env python3
"""Generate the protobuf bindings (src/X/Common/Protobuf/*.pb.{cc,h}) for a game version.

The descriptors come straight from data/<version>/main.elf (tools/game_descriptors.py),
so no .nss is needed. They are rendered to .proto, compiled with the pinned
protoc 2.6.0, and adapted exactly as tools/bootstrap_game_data.py does for
2.6.2 (Hydra ABI, original diagnostic lines). The #line directive names the
version's build-server path. A header keeps its layout/<Name>.inc size checks
(measured on 2.6.2) only when its descriptor is unchanged from 2.6.2.

  python3 tools/bootstrap_protobuf.py --protoc /path/to/protoc-2.6.0
  python3 tools/bootstrap_protobuf.py --check     # compare with the installed files
  python3 tools/bootstrap_protobuf.py --names out.csv   # prove function names (below)

The outputs are derived from the game and stay ignored by git, like 2.6.2's.

--names proves which main.elf function each generated method is, without
symbols, and writes the functions.csv entries that disagree (address,
current_name, proposed_name, confidence, evidence, file: the columns
build/agents/apply_mapping.py --map reads; a displaced name becomes sub_<addr>). It reads the objects tools/try_file.py leaves in
build/try/, so run try_file.py on every .pb.cc first. Evidence, strongest first:
  vtable   protobuf_AddDesc_<Name>() allocates every default instance in message
           order and stores its vtable (adrp/ldr GOT/add #0x10, inline or in the
           constructor); vtable slot k of the binary is slot k of ours.
  call     a proven function with the original size calls, at the same offset,
           the function our object calls there.
  ctor     a function of our size that loads the class's vtable and matches
           ours byte for byte (masked relocations); one candidate only.
  bytes    a name functions.csv places on a function that does not match goes
           to the one byte-identical function near the class's proven methods.
"""
from __future__ import annotations

import argparse
import csv
import re
import shutil
import struct
import subprocess
import sys
from pathlib import Path
from tempfile import TemporaryDirectory

import project_version
from bootstrap_game_data import BINDINGS, adapt
from game_descriptors import ADD_FILE, _proto_name, descriptors, registered_file
from game_proto import schema

ROOT = Path(__file__).resolve().parent.parent
SOURCE_DIRS = {
    '2.6.2': 'D:/BuildServer/2/work/code/branches/2_6_2/X/Common/Protobuf/',
    '2.7.7': 'D:/BuildServer/1/work/code/branches/2_7_7/X/Common/Protobuf/',
}


def default_protoc() -> str:
    for candidate in (ROOT / 'build/game-tools/protoc-2.6.0',
                      ROOT.parent / 'NSDiablo3Decomp/build/game-tools/protoc-2.6.0'):
        if candidate.is_file():
            return str(candidate)
    return 'protoc'


def generate(version: str, protoc: str) -> dict[Path, bytes]:
    found = descriptors(ROOT, version)
    baseline = descriptors(ROOT, '2.6.2') if version != '2.6.2' else found
    raw = {name: data for name, (_, data) in found.items() if (BINDINGS / 'layout' / f'{name}.inc').is_file()}
    out_version = subprocess.check_output([protoc, '--version'], text=True).strip()
    if out_version != 'libprotoc 2.6.0':
        raise SystemExit(f'expected libprotoc 2.6.0, found {out_version} ({protoc})')
    outputs = {}
    with TemporaryDirectory() as tmp:
        stage = Path(tmp)
        for name, data in raw.items():
            (stage / f'{name}.proto').write_text(schema(data))
        subprocess.run([protoc, '-I.', '--cpp_out=.', *[f'{n}.proto' for n in raw]], cwd=stage, check=True)
        for name in raw:
            unchanged = baseline.get(name, (0, b''))[1] == raw[name]
            for ext in ('cc', 'h'):
                path = BINDINGS / f'{name}.pb.{ext}'
                text = adapt(path, (stage / path.name).read_text(), SOURCE_DIRS[version],
                             layout_checks=unchanged)
                outputs[path] = text.encode()
    return outputs


# --- --names: prove function names from vtables ------------------------------

CSV_BASE = 0x7100000000


def _sext(value: int, bits: int) -> int:
    return value - (1 << bits) if value & (1 << (bits - 1)) else value


def _branch(word: int, pc: int) -> int | None:
    """Target of B/BL, else None."""
    if word & 0x7C000000 == 0x14000000:
        return pc + 4 * _sext(word & 0x3FFFFFF, 26)
    return None


def vtable_got_loads(words, start: int) -> list[tuple[int, int]]:
    """[(index, GOT slot)] for each 'adrp xA; ldr xA, [xA, #lo]; ... add xA, xA, #0x10':
    the address point of a vtable loaded through the GOT (a vtable store)."""
    out, pages = [], {}
    for i, w in enumerate(words):
        rd = w & 31
        if w & 0x9F000000 == 0x90000000:                                  # ADRP
            imm = ((w >> 29) & 3) | (((w >> 5) & 0x7FFFF) << 2)
            pages[rd] = ((start + 4 * i) & ~0xFFF) + (_sext(imm, 21) << 12)
            continue
        if w & 0xFFC00000 == 0xF9400000 and (w >> 5) & 31 in pages:       # LDR x, [x, #imm]
            got = pages[(w >> 5) & 31] + ((w >> 10) & 0xFFF) * 8
            if any(n & 0xFFFFFC00 == 0x91004000 and n & 31 == rd and (n >> 5) & 31 == rd
                   for n in words[i + 1:i + 5]):                          # ADD xA, xA, #0x10
                out.append((i, got))
        pages.pop(rd, None)
    return out


def default_instance_vtables(words, start, new_fn, slot, ctor_words) -> list[int]:
    """Vtable of each 'new Message()' in protobuf_AddDesc, in order. A 'new' with
    no vtable store before the next call (a string default) is skipped; a call
    before the store is the out-of-line constructor, which then holds it."""
    out = []
    for i, w in enumerate(words):
        if _branch(w, start + 4 * i) != new_fn:
            continue
        j = i + 1
        while j < len(words) and _branch(words[j], start + 4 * j) is None:
            j += 1
        loads = [g for k, g in vtable_got_loads(words[i + 1:j + 1], start + 4 * (i + 1))]
        if not loads and j < len(words) and _branch(words[j], start + 4 * j) != new_fn:
            callee = _branch(words[j], start + 4 * j)
            loads = [g for _, g in vtable_got_loads(ctor_words(callee), callee)][:1]
        vt = slot(loads[0]) if loads else None
        if vt is not None:
            out.append(vt)
    return out


def _same(code: bytes, masks: dict, want: bytes) -> bool:
    if len(code) != len(want):
        return False
    for i in range(0, len(code), 4):
        keep = ~masks.get(i, 0) & 0xFFFFFFFF
        a, = struct.unpack_from('<I', code, i)
        b, = struct.unpack_from('<I', want, i)
        if not (b == 0xE7FFDEFE and a & 0xFFE0001F == 0xD4200000) and (a & keep) != (b & keep):
            return False
    return True


def _readelf(*args) -> str:
    exe = shutil.which('llvm-readelf') or '/opt/homebrew/opt/llvm/bin/llvm-readelf'
    return subprocess.run([exe, *args], capture_output=True, text=True, check=True).stdout


def our_object(obj: Path):
    """(vtable symbols {name: (offset, size)}, vtable slots {offset: symbol},
    calls {function: [(offset, symbol)]}) of a compiled .pb.cc object."""
    vtables, slots, calls, section = {}, {}, {}, ''
    for line in _readelf('-sW', str(obj)).splitlines():
        p = line.split()
        if len(p) >= 8 and p[3] == 'OBJECT' and p[7].startswith('_ZTV'):
            vtables[p[7]] = (int(p[1], 16), int(p[2]))
    for line in _readelf('-rW', str(obj)).splitlines():
        m = re.match(r"Relocation section '([^']+)'", line)
        if m:
            section = m.group(1)
            continue
        p = line.split()
        if len(p) < 5 or not p[2].startswith('R_AARCH64'):
            continue
        if section == '.rela.data.rel.ro':
            slots[int(p[0], 16)] = p[4]
        elif section.startswith('.rela.text.') and p[2] in ('R_AARCH64_CALL26', 'R_AARCH64_JUMP26'):
            calls.setdefault(section[len('.rela.text.'):], []).append((int(p[0], 16), p[4]))
    return vtables, slots, calls


def _complete(sym: str) -> str:
    """functions.csv names the complete-object constructor/destructor (C1/D1)."""
    return re.sub(r'([CD])2E', r'\g<1>1E', sym)


def prove_names(version: str) -> list[dict]:
    import version_diff as vd
    from data_elf import inspect_elf
    from validation import elf as elf_reader

    data = ROOT / 'data' / version
    elf = vd.ELF(data / 'main.elf')
    image = elf_reader.Elf(data / 'main.elf')
    relative = {r['offset']: r['addend'] for r in inspect_elf(data / 'main.elf')['relocations']
                if r['type'] == 1027}
    rows = list(csv.DictReader((data / 'functions.csv').open(newline='')))
    files = {r['Address']: r['File'] for r in csv.DictReader((data / 'function_files.csv').open(newline=''))}
    by_name = {r['Name']: int(r['Address'], 16) - CSV_BASE for r in rows}
    by_addr = {int(r['Address'], 16) - CSV_BASE: r for r in rows}
    size_at = {a: int(r['Size']) for a, r in by_addr.items()}
    new_fn, add_file = by_name['_Znwm'], by_name[ADD_FILE]

    def words(addr, count):
        return elf.words(addr, 4 * count)

    def target_matches(code, masks, addr):
        return size_at.get(addr) == len(code) and _same(code, masks, image.read_vaddr(addr, len(code)))

    add_desc = {}
    for a, r in by_addr.items():
        if size_at[a] and elf.is_exec(a) and (r['Name'].endswith('_2eprotoEv') or r['Name'].startswith('sub_')):
            found = registered_file(elf, a, size_at[a] & ~3, add_file)
            if found:
                off = elf.offset(found[0])
                add_desc[_proto_name(elf.data[off:off + found[1]])] = a

    proven: dict[str, tuple[int, str, str]] = {}            # name -> (address, evidence, file)
    vt_of: dict[str, int] = {}                              # our vtable symbol -> target vtable
    units = {}
    for src in sorted(BINDINGS.glob('*.pb.cc')):
        name = src.name[:-len('.pb.cc')]
        obj = ROOT / 'build/try' / f'X_Common_Protobuf_{src.name}' / 'obj.o'
        if not obj.is_file():
            raise SystemExit(f'{obj.relative_to(ROOT)} is missing: run tools/try_file.py {src.relative_to(ROOT)}')
        body = re.search(r'void protobuf_AddDesc_\w+\(\) \{(.*?)\n\}', src.read_text(), re.S).group(1)
        classes = re.findall(r'(\w+)::default_instance_ = new \1\(\);', body)
        start = add_desc[name]
        targets = default_instance_vtables(words(start, size_at[start] // 4), start, new_fn,
                                           relative.get, lambda a: words(a, 40))
        if len(targets) != len(classes):
            raise SystemExit(f'{name}: {len(classes)} messages but {len(targets)} default-instance vtables')
        vtables, slots, calls = our_object(obj)
        built = elf_reader.Elf(obj).functions()
        unit = f'X/Common/Protobuf/{src.name}'
        units[name] = (unit, built, calls)
        for cls, target in zip(classes, targets):
            sym, = [v for v in vtables if v.endswith(f'{len(cls)}{cls}E')]
            vt_of[sym] = target
            base, size = vtables[sym]
            for k in range(2, size // 8):                    # skip offset-to-top, typeinfo
                method = slots.get(base + 8 * k, '')
                fn = relative.get(target + 8 * k)
                if f'{len(cls)}{cls}' in method and fn is not None:
                    proven[_complete(method)] = (fn, 'vtable', unit)

    # Calls at the same offset of a proven function of the original size.
    work = list(proven.items())
    while work:
        sym, (addr, _, unit) = work.pop()
        _, built, calls = units[next(n for n, u in units.items() if u[0] == unit)]
        key = sym if sym in built else re.sub(r'([CD])1E', r'\g<1>2E', sym)
        if key not in built or size_at.get(addr) != len(built[key][0]):
            continue
        code = words(addr, len(built[key][0]) // 4)
        for off, callee in calls.get(key, []):
            target = _branch(code[off // 4], addr + off)
            callee = _complete(callee)
            if target is None or target not in by_addr or callee in proven:
                continue
            if callee.startswith('_ZN6google') and 'RepeatedPtrField' not in callee:
                continue
            proven[callee] = (target, 'call', unit)
            work.append((callee, proven[callee]))

    # Non-virtual methods that store the class vtable (constructors).
    taken = {a for a, _, _ in proven.values()}
    lo, hi = min(a for a, _, _ in proven.values() if a > 0x400000) - 0x10000, max(taken) + 0x10000
    loads = {}
    for a in by_addr:
        if lo <= a < hi and size_at[a] and by_addr[a]['Quality'] != 'L' and elf.is_exec(a):
            for _, got in vtable_got_loads(words(a, size_at[a] // 4), a):
                if got in relative:
                    loads.setdefault(relative[got], set()).add(a)
    for unit, built, _ in units.values():
        found = {}
        for sym, (code, masks) in built.items():
            if sym in proven or re.search(r'[CD]2E', sym):
                continue
            hits = {a for v, t in vt_of.items() if v[4:-1] in sym
                    for a in loads.get(t, ()) if a not in taken and target_matches(code, masks, a)}
            if len(hits) == 1:
                found.setdefault(hits.pop(), []).append(sym)
        for a, syms in found.items():
            if len(syms) == 1:
                proven[syms[0]] = (a, 'ctor', unit)
                taken.add(a)

    # A functions.csv name on a function that does not match: the one byte-identical
    # function near the class's proven methods.
    for unit, built, _ in units.values():
        for sym, (code, masks) in built.items():
            if (sym in proven or sym not in by_name or len(code) < 64 or not sym.startswith(('_ZN2D3', '_ZNK2D3'))
                    or target_matches(code, masks, by_name[sym])):
                continue
            near = [a for v in vt_of if v[4:-1] in sym for s, (a, _, _) in proven.items() if v[4:-1] in s]
            if not near:
                continue
            hits = [a for a in by_addr if min(near) - 0x2000 <= a < max(near) + 0x2000 and a not in taken
                    and by_addr[a]['Quality'] != 'L' and target_matches(code, masks, a)]
            if len(hits) == 1:
                proven[sym] = (hits[0], 'bytes', unit)
                taken.add(hits[0])

    names = {a: r['Name'] for a, r in by_addr.items()}
    evidence, owner = {}, {}
    for sym, (addr, how, unit) in proven.items():
        defined = [u for u, built, _ in units.values() if sym in built]
        unit = defined[0] if len(defined) == 1 else unit
        old = by_name.get(sym)
        if old is not None and old != addr and names.get(old) == sym:
            names[old] = f'sub_{old + CSV_BASE:x}'
        names[addr], evidence[addr], owner[addr] = sym, how, unit
    out = []
    for a in sorted(names):
        if names[a] != by_addr[a]['Name']:
            address = by_addr[a]['Address']
            how = evidence.get(a, 'displaced')
            out.append(dict(address=address, current_name=by_addr[a]['Name'], proposed_name=names[a],
                            confidence='medium' if how == 'bytes' else 'high', evidence=how,
                            file=owner.get(a, files.get(address, ''))))
    return out


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__.split('\n\n')[0])
    ap.add_argument('--version', default=project_version.VERSION)
    ap.add_argument('--protoc', default=None, help='protoc 2.6.0 (default: build/game-tools/protoc-2.6.0)')
    ap.add_argument('--check', action='store_true', help='report differences, write nothing')
    ap.add_argument('--names', metavar='CSV', help='write the proven functions.csv renames, generate nothing')
    args = ap.parse_args(argv)
    if args.names:
        rows = prove_names(args.version)
        with open(args.names, 'w', newline='') as fh:
            writer = csv.DictWriter(fh, list(rows[0]) if rows else ['address'], lineterminator='\n')
            writer.writeheader()
            writer.writerows(rows)
        print(f'{args.version}: {len(rows)} functions.csv names disagree with the binary ({args.names})')
        return 0
    outputs = generate(args.version, args.protoc or default_protoc())
    changed = [p for p, data in outputs.items() if not p.is_file() or p.read_bytes() != data]
    if args.check:
        print(f'{len(changed)} of {len(outputs)} binding files differ from {args.version}')
        for p in changed:
            print(f'  {p.relative_to(ROOT)}')
        return 1 if changed else 0
    for p in changed:
        p.write_bytes(outputs[p])
    print(f'{args.version}: wrote {len(changed)} of {len(outputs)} binding files')
    return 0


if __name__ == '__main__':
    sys.exit(main())
