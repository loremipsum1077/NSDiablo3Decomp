"""Reconstruct the Switch SWIG tables from ELF relocations and public symbol inventories."""
import csv
import hashlib
import json
import re
import struct

from data import relative_relocs


def plain(symbol):
    symbol = symbol.partition("@")[0]
    match = re.match(r"_ZL\d+(.+?)(?:P9lua_State)?$", symbol)
    return match[1] if match else symbol


class LuaTableImage:
    def __init__(self, root, image):
        self.elf = image
        self.relocations = relative_relocs(image)
        self.syms = {}
        for file in ("data_symbols.csv", "functions.csv"):
            with (root / "data/2.6.2" / file).open() as stream:
                for row in csv.DictReader(stream):
                    self.syms[int(row["Address"], 16) & 0xffffffff] = plain(row["Name"])
        self.addr = {name: address for address, name in self.syms.items()}

    def raw(self, address, size):
        return self.elf.read_vaddr(address, size) or bytes(size)

    def ptr(self, address):
        if address in self.relocations:
            return self.relocations[address]
        return struct.unpack("<Q", self.raw(address, 8))[0]

    def text(self, address):
        if not address:
            return "0"
        data = self.elf.read_vaddr(address, 1024)
        return json.dumps(data[:data.index(0)].decode("utf8")) if data[0] else "0"

    def ref(self, address):
        target = self.ptr(address)
        if not target:
            return "0"
        if target in self.syms:
            return "&" + self.syms[target]
        offset = target - self.addr["swig_types"]
        if 0 <= offset < 13 * 8:
            return f"&swig_types[{offset // 8}]"
        raise ValueError(f"unresolved SWIG pointer at {address:#x}")


def generate_lua_tables(source):
    elf, rel = source.elf, source.relocations
    addr, syms = source.addr, source.syms
    raw, ptr, text, ref = source.raw, source.ptr, source.text, source.ref
    out = []

    def emit(line=""):
        out.append(line)

    emit('// Original initializers recovered from ELF bytes and R_AARCH64_RELATIVE relocations.')
    emit()  # swig_types is declared earlier in the wrapper.
    emit('static swig_module_info swig_module = {swig_types,12,0,0,0,0};')
    classes = (("LARGE_INTEGER", 2, 1), ("RequiredMessageHeader", 5, 1), ("NavMeshHandle", 3, 2))
    commands = "swig_commands"
    constants = "swig_constants"
    counts = {
        commands: (1116, 16), constants: (3330, 48), "swig_types": (13, 8),
        "swig_module": (1, 48), "swig_type_initial": (12, 8), "swig_cast_initial": (12, 8),
    }
    entry_points = set()

    def addresses(name):
        count, stride = counts[name]
        return range(addr[name], addr[name] + count * stride, stride)

    for cls, attributes, methods in classes:
        prefix = "swig_" + cls
        counts[prefix + "_methods"] = methods, 16
        counts[prefix + "_attributes"] = attributes, 24
        counts[prefix + "_bases"] = 1, 8
        counts["_wrap_class_" + cls] = 1, 56

    for name, (_, stride) in counts.items():
        if name == commands or name.endswith(("_methods", "_attributes")):
            for address in addresses(name):
                for offset in range(8, stride, 8):
                    target = ptr(address + offset)
                    if target:
                        entry_points.add(syms[target])
    for function in sorted(entry_points):
        emit(f"static int {function}(lua_State *L);")

    for cls, attributes, methods in classes:
        prefix = "swig_" + cls
        emit(f"static swig_lua_method {prefix}_methods[{methods}] = {{")
        for a in addresses(prefix + "_methods"):
            emit("    {%s,%s}," % (text(ptr(a)), ref(a + 8)))
        emit("};")
        emit(f"static swig_lua_attribute {prefix}_attributes[{attributes}] = {{")
        for a in addresses(prefix + "_attributes"):
            emit("    {%s,%s,%s}," % (text(ptr(a)), ref(a + 8), ref(a + 16)))
        emit("};")
        emit(f"static swig_lua_class *{prefix}_bases[1] = {{0}};")
        a = addr["_wrap_class_" + cls]
        members = [text(ptr(a)), ref(a + 8), ref(a + 16), ref(a + 24),
                   prefix + "_methods", prefix + "_attributes", prefix + "_bases"]
        emit(f"swig_lua_class _wrap_class_{cls} = {{" + ",".join(members) + "};")

    emit("static const luaL_Reg swig_commands[1116] = {")
    for a in addresses(commands):
        emit("    {%s,%s}," % (text(ptr(a)), ref(a + 8)))
    emit("};")
    emit("static swig_lua_var_info swig_variables[5] = {")
    # This table lost its symbol. The surviving Hydra_Init calls identify the getters.
    for name in ("SGAMEID_INVALID", "NAVMESH_HANDLE_NONE", "MAX_AFFIX_ALLOWED_TYPES", "MAX_AFFIX_EXCLUDED_CATEGORIES"):
        emit('    {"%s",_wrap_%s_get,0},' % (name, name))
    emit("    {0,0,0}\n};")
    emit("static swig_lua_const_info swig_constants[3330] = {")
    for a in addresses(constants):
        kind, = struct.unpack("<i", raw(a, 4))
        integer, floating = struct.unpack("<qd", raw(a + 16, 16))
        label = "(char *)" + text(ptr(a + 8)) if ptr(a + 8) else "0"
        emit("    {%d,%s,%d,%s,%s,%s}," % (kind, label, integer, repr(floating), ref(a + 32), ref(a + 40)))
    emit("};")

    types = [syms[ptr(a)] for a in addresses("swig_type_initial")]
    for name in types:
        counts[name] = 1, 48
        a = addr[name]
        tag, = struct.unpack("<i", raw(a + 40, 4))
        emit("static swig_type_info %s = {%s,%s,%s,%s,%s,%d};" %
             (name, text(ptr(a)), text(ptr(a + 8)), ref(a + 16), ref(a + 24), ref(a + 32), tag))
    emit("static swig_type_info *swig_type_initial[12] = {" + ",".join("&" + name for name in types) + "};")
    casts = [syms[ptr(a)] for a in addresses("swig_cast_initial")]
    for name in casts:
        counts[name] = 2, 32
        emit(f"static swig_cast_info {name}[2] = {{")
        for a in addresses(name):
            emit("    {%s,%s,%s,%s}," % (ref(a), ref(a + 8), ref(a + 16), ref(a + 24)))
        emit("};")
    emit("static swig_cast_info *swig_cast_initial[12] = {" + ",".join(casts) + "};")

    tables = []
    relocations = sorted(rel.items())
    for name, (count, stride) in counts.items():
        a = addr[name]
        size = count * stride
        tables.append({
            "name": name, "address": hex(0x7100000000 + a), "count": count, "stride": stride,
            "size": size, "storage": "file" if elf.read_vaddr(a, size) else "bss",
            "sha256_original_bytes": hashlib.sha256(raw(a, size)).hexdigest(),
            "relocations": [{"offset": off - a, "target": hex(0x7100000000 + target)}
                            for off, target in relocations if a <= off < a + size],
        })
    evidence = {
        "source": "data/2.6.2/main.elf", "tables": tables,
        "swig_variables": "No surviving symbol; four entries recovered from Hydra_Init named call/data operands, followed by null terminator.",
    }
    return "\n".join(out) + "\n", json.dumps(evidence, indent=2) + "\n"
