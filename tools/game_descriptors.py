"""Find the serialized FileDescriptorProtos a build registers with protobuf.

Every generated protobuf_AddDesc_<Name>_2eproto() calls
DescriptorPool::InternalAddGeneratedFile(data, size). The data address comes
from ADRP + ADD and the size from MOVZ/MOVK into w1, so both are read from the
function's code in main.elf; no debug symbols are needed.
"""
from __future__ import annotations

import csv
import re
from pathlib import Path

import version_diff as vd

ADD_FILE = '_ZN6google8protobuf14DescriptorPool24InternalAddGeneratedFileEPKvi'
ADD_DESC = re.compile(r'protobuf_AddDesc_(\w+)_2eprotoEv$')


def _sext(v, bits):
    return v - (1 << bits) if v & (1 << (bits - 1)) else v


def registered_file(elf: vd.ELF, start: int, size: int, add_file: int) -> tuple[int, int] | None:
    """(data address, length) passed to InternalAddGeneratedFile, or None."""
    regs: dict[int, int] = {}
    for i, w in enumerate(elf.words(start, size)):
        pc = start + 4 * i
        rd = w & 31
        if (w & 0x9F000000) == 0x90000000:                          # ADRP
            imm = ((w >> 29) & 3) | (((w >> 5) & 0x7FFFF) << 2)
            regs[rd] = (pc & ~0xFFF) + (_sext(imm, 21) << 12)
        elif (w & 0xFF800000) == 0x91000000:                        # ADD x, imm
            rn = (w >> 5) & 31
            if rn in regs:
                shift = 12 if (w >> 22) & 1 else 0
                regs[rd] = regs[rn] + (((w >> 10) & 0xFFF) << shift)
        elif (w & 0x7F800000) == 0x52800000:                        # MOVZ
            regs[rd] = ((w >> 5) & 0xFFFF) << (16 * ((w >> 21) & 3))
        elif (w & 0x7F800000) == 0x72800000 and rd in regs:         # MOVK
            hw = 16 * ((w >> 21) & 3)
            regs[rd] = (regs[rd] & ~(0xFFFF << hw)) | (((w >> 5) & 0xFFFF) << hw)
        elif (w & 0xFC000000) == 0x94000000:                        # BL
            if pc + 4 * _sext(w & 0x3FFFFFF, 26) == add_file:
                if 0 in regs and 1 in regs:
                    return regs[0], regs[1] & 0xFFFFFFFF
                return None
            regs.clear()
    return None


def _proto_name(data: bytes) -> str:
    """Field 1 (name) of a serialized FileDescriptorProto, without '.proto'."""
    if data[0] != 0x0A:
        raise ValueError('descriptor does not start with its name')
    length, i, shift = 0, 1, 0
    while True:
        byte = data[i]; i += 1
        length |= (byte & 0x7F) << shift; shift += 7
        if byte < 0x80:
            break
    return data[i:i + length].decode().removesuffix('.proto')


def descriptors(root: Path, version: str) -> dict[str, tuple[int, bytes]]:
    """{proto name: (vaddr, serialized FileDescriptorProto)} for a version.

    Every function that calls InternalAddGeneratedFile is read (named or not), and
    each descriptor is keyed by the file name it carries.
    """
    elf = vd.ELF(root / 'data' / version / 'main.elf')
    funcs = {}
    with (root / 'data' / version / 'functions.csv').open(newline='') as fh:
        for r in csv.DictReader(fh):
            funcs[r['Name']] = (int(r['Address'], 16) - vd.CSV_BASE, int(r['Size']))
    add_file = funcs[ADD_FILE][0]
    out = {}
    for name, (addr, size) in funcs.items():
        if not size or not elf.is_exec(addr):
            continue
        found = registered_file(elf, addr, size & ~3, add_file)
        if found is None:
            if ADD_DESC.search(name):
                raise ValueError(f'{name}: no InternalAddGeneratedFile(data, size) call found')
            continue
        data_addr, length = found
        off = elf.offset(data_addr)
        data = elf.data[off:off + length]
        key = _proto_name(data)
        if key in out and out[key][1] != data:
            raise ValueError(f'{key}: two different descriptors')
        out[key] = (data_addr, data)
    return out
