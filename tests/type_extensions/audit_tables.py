"""Verify TypeExtensions vtable bytes and relocated method targets against main.elf.

Usage: python3 tests/type_extensions/audit_tables.py <compiled obj.o>
Requires all 18 vtables emitted by this source. Checks bytes and every absolute relocation.
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

# Fixed source inventory: do not discover this from the object under audit, since
# that would silently accept a missing vtable. DT_ENUM and ComplexTypeDescriptor
# have key functions outside this source and must not be emitted here.
FAMILIES = (
    'DT_INT', 'DT_INT64', 'DT_BYTE', 'DT_WORD', 'DT_DATAID', 'DT_FLOAT',
    'DT_ANGLE', 'DT_PERCENT', 'DT_HIGHPRECISIONPERCENT', 'DT_FLAG',
    'DT_FIXEDARRAY', 'DT_VARIABLEARRAY', 'DT_OPTIONAL', 'DT_STRING',
    'DT_CHARARRAY', 'DT_TRANSLATEABLE', 'DT_CSTRING',
)
CLASSES = tuple('TypeDescriptorClass_' + family for family in FAMILIES) + ('VariantTypeDescriptor',)
EXPECTED = {'_ZTV' + str(len(cls)) + cls for cls in CLASSES}
assert len(EXPECTED) == 18
emitted = {name for name in by_name if name.startswith('_ZTV')}
assert emitted == EXPECTED, ('vtable inventory differs', 'missing', sorted(EXPECTED-emitted), 'extra', sorted(emitted-EXPECTED))
report = []
for name in sorted(EXPECTED):
    size = 344  # Two ABI prefix words and 41 virtual function pointers.
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
output = ROOT / 'build/try/type_evidence/tables.audit.json'
output.parent.mkdir(parents=True, exist_ok=True)
output.write_text(json.dumps(report, indent=2) + '\n')
print(len(report), 'vtables;', sum(r['verified_pointers'] for r in report), 'pointer targets verified')
