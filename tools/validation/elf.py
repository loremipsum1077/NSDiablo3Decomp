"""Shared ELF reader and AArch64 relocation masks; policy unchanged from flag_sweep."""
from pathlib import Path
import struct

# AArch64 relocation type -> instruction bits the linker fills in
MASKS = {
    282: 0x03FFFFFF, 283: 0x03FFFFFF,                     # JUMP26, CALL26
    274: 0x60FFFFE0, 275: 0x60FFFFE0, 276: 0x60FFFFE0,    # ADR / ADRP
    311: 0x60FFFFE0,                                      # ADR_GOT_PAGE
    277: 0x003FFC00, 278: 0x003FFC00, 284: 0x003FFC00,    # ADD / LDST lo12
    285: 0x003FFC00, 286: 0x003FFC00, 299: 0x003FFC00,
    312: 0x003FFC00,                                      # LD64_GOT_LO12_NC
}


class Elf:
    def __init__(self, path):
        self.d = d = Path(path).read_bytes()
        shoff, = struct.unpack_from("<Q", d, 0x28)
        shentsize, shnum = struct.unpack_from("<HH", d, 0x3A)
        self.sections = [struct.unpack_from("<IIQQQQIIQQ", d, shoff + i * shentsize) for i in range(shnum)]
        self.phdrs = []
        phoff, = struct.unpack_from("<Q", d, 0x20)
        phentsize, phnum = struct.unpack_from("<HH", d, 0x36)
        for i in range(phnum):
            p = struct.unpack_from("<IIQQQQQQ", d, phoff + i * phentsize)
            if p[0] == 1:
                self.phdrs.append((p[3], p[2], p[5]))  # vaddr, offset, filesz

    def read_vaddr(self, vaddr, size):
        for va, off, filesz in self.phdrs:
            if va <= vaddr and vaddr + size <= va + filesz:
                return self.d[off + vaddr - va: off + vaddr - va + size]
        return b""

    def functions(self):
        """{name: (bytes, {offset: mask})} for every defined function"""
        relocs = {}
        for s in self.sections:
            if s[1] == 4:  # SHT_RELA
                tbl = relocs.setdefault(s[7], {})
                for o in range(s[4], s[4] + s[5], 24):
                    r_off, r_info, _ = struct.unpack_from("<QQq", self.d, o)
                    tbl[r_off] = MASKS.get(r_info & 0xFFFFFFFF, 0xFFFFFFFF)
        out = {}
        for s in self.sections:
            if s[1] != 2:  # SHT_SYMTAB
                continue
            stroff = self.sections[s[6]][4]
            for o in range(s[4], s[4] + s[5], 24):
                name, info, _, shndx, value, size = struct.unpack_from("<IBBHQQ", self.d, o)
                if info & 0xF != 2 or shndx == 0 or shndx >= len(self.sections):
                    continue
                end = self.d.index(b"\0", stroff + name)
                sec = self.sections[shndx]
                code = self.d[sec[4] + value: sec[4] + value + size]
                masks = {k - value: m for k, m in relocs.get(shndx, {}).items() if value <= k < value + size}
                out[self.d[stroff + name:end].decode()] = (code, masks)
        return out
