"""Verify all D3 descriptor objects, public pointers, and vtables against main.elf.

Usage: python3 tests/d3_descriptors/audit_tables.py <compiled obj.o>
Checks ordinary bytes and every absolute relocation, including string contents.
"""
import csv
import json
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'tools'))
from validation.elf import Elf
from data import relative_relocs

original = Elf(ROOT / 'data/2.6.2/main.elf')
obj = Elf(Path(sys.argv[1]))
addresses = {}
for table in ('data_symbols.csv', 'functions.csv'):
    for row in csv.DictReader((ROOT / 'data/2.6.2' / table).open()):
        addresses[row['Name'].split('@')[0]] = int(row['Address'], 16) & 0xffffffff
original_relocs = relative_relocs(original)

def cstring(raw, off):
    return raw[off:raw.index(b'\0', off)]

symbols = []
for section in obj.sections:
    if section[1] == 2:
        strings = obj.sections[section[6]][4]
        for pos in range(section[4], section[4] + section[5], 24):
            name, info, other, index, value, size = struct.unpack_from('<IBBHQQ', obj.d, pos)
            symbols.append((cstring(obj.d, strings + name).decode(), index, value, size, info))
relocations = {}
for section in obj.sections:
    if section[1] == 4:
        for pos in range(section[4], section[4] + section[5], 24):
            off, info, addend = struct.unpack_from('<QQq', obj.d, pos)
            relocations[(section[7], off)] = (symbols[info >> 32], addend, info & 0xffffffff)
by_name = {s[0]: s for s in symbols if s[1] and s[1] < len(obj.sections)}

def compare_pointer(original_pointer, symbol, addend):
    name, sec, value, size, info = symbol
    if name in addresses:
        return original_pointer == addresses[name] + addend
    target = value + addend
    for n, s, v, z, i in symbols:
        if s == sec and z and v <= target < v + z and n in addresses:
            return original_pointer == addresses[n] + target - v
    # Section/string-label references have no source-level symbol.
    assert sec and sec < len(obj.sections), ('unknown target', symbol, addend)
    assert obj.sections[sec][2] & 0x20, ('non-string anonymous target', symbol, addend)
    actual = cstring(obj.d, obj.sections[sec][4] + target)
    expected = original.read_vaddr(original_pointer, len(actual) + 1)
    return expected == actual + b'\0'

names = [n for n in addresses if n.startswith('_ZL') and 'TypeDescriptor_DT_' in n]
# Restrict to the 20 objects owned by this source, using its DWARF variable list.
import re
reference = (ROOT / 'build/dwarf/headers/X/Common/D3TypeDescriptor/D3DescribedType.cpp').read_text()
families = re.findall(r'static const TypeDescriptorClass_(DT_\w+) TypeDescriptor_', reference)
assert len(families) == 20
report = []
for family in families:
    local = 'TypeDescriptor_' + family
    cls = 'TypeDescriptorClass_' + family
    names = ['_ZL' + str(len(local)) + local, family, '_ZTV' + str(len(cls)) + cls]
    sizes = [56, 8, 352 if family in ('DT_SNO', 'DT_SNONAME_HANDLE') else 344]
    for name, size in zip(names, sizes):
        assert name in by_name, ('not emitted', name)
        _, sec, off, actual_size, _ = by_name[name]
        assert actual_size == size, (name, actual_size, size)
        address = addresses[name]
        expected = bytearray(original.read_vaddr(address, size))
        raw = bytearray(obj.d[obj.sections[sec][4] + off:obj.sections[sec][4] + off + size])
        count = 0
        assert all((r_off-off) % 8 == 0 for (r_sec,r_off) in relocations if r_sec == sec and off <= r_off < off+size), (name, 'unaligned relocation')
        for i in range(0, size, 8):
            target = original_relocs.get(address + i)
            relocation = relocations.get((sec, off + i))
            if target is None:
                assert relocation is None, (name, i, 'unexpected relocation')
                continue
            assert relocation, (name, i, 'missing relocation')
            symbol, addend, kind = relocation
            assert kind == 257, (name, i, kind)
            assert compare_pointer(target, symbol, addend), (name, i, hex(target), symbol, addend)
            raw[i:i+8] = expected[i:i+8] = bytes(8)
            count += 1
        assert raw == expected, (name, 'non-relocated bytes differ')
        report.append({'name': name, 'size': size, 'verified_pointers': count})
output = ROOT / 'build/try/d3_evidence/tables.audit.json'
output.parent.mkdir(parents=True, exist_ok=True)
output.write_text(json.dumps(report, indent=2) + '\n')
print(len(report), 'objects/pointers/vtables;', sum(r['verified_pointers'] for r in report), 'pointer targets verified')
