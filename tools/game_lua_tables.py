"""Reconstruct the Switch SWIG tables from ELF relocations and public symbol inventories.

  python3 tools/game_lua_tables.py --version 2.7.7            # write generated-game-data/lua_tables.inc
  python3 tools/game_lua_tables.py --version 2.7.7 --check    # only compare with the file on disk

2.6.2 names every table in data_symbols.csv. Later versions name only swig_types (and
swig_cast_initial, 8 bytes off in 2.7.7), so the rest are located from swig_types and the table layout
(see LuaTableImage.locate); the reconstructed counts come from the tables themselves.
"""
import argparse
import csv
import hashlib
import json
import re
import struct
from pathlib import Path

from data import relative_relocs

ROOT = Path(__file__).resolve().parent.parent
CLASSES = (("LARGE_INTEGER", 2, 1), ("RequiredMessageHeader", 5, 1), ("NavMeshHandle", 3, 2))
CONST_STRIDE, COMMAND_STRIDE, TYPE_STRIDE, CAST_STRIDE = 48, 16, 48, 32
TYPE_COUNT = 12


def plain(symbol):
    symbol = symbol.partition("@")[0]
    match = re.match(r"_ZL\d+(.+?)(?:P9lua_State)?$", symbol)
    return match[1] if match else symbol


class LuaTableImage:
    def __init__(self, root, image, version="2.6.2"):
        self.elf = image
        self.version = version
        self.relocations = relative_relocs(image)
        self.syms = {}
        for file in ("data_symbols.csv", "functions.csv"):
            with (root / "data" / version / file).open() as stream:
                for row in csv.DictReader(stream):
                    self.syms[int(row["Address"], 16) & 0xffffffff] = plain(row["Name"])
        self.addr = {name: address for address, name in self.syms.items()}
        self.counts = {"swig_commands": 1116, "swig_constants": 3330}
        self.misnamed = {}
        self.inferred = {}
        if "swig_commands" not in self.addr:
            self.locate()

    def is_command(self, entry):
        """A luaL_Reg: a relocated pointer to an identifier and one into the code segment."""
        rel = self.relocations
        if entry not in rel or entry + 8 not in rel:
            return False
        code_start, _, code_size = self.elf.phdrs[0]
        if not code_start <= rel[entry + 8] < code_start + code_size:
            return False
        name = self.raw(rel[entry], 128)
        name = name[:name.find(b"\0")]
        return bool(re.fullmatch(rb"[A-Za-z_][A-Za-z0-9_]*", name))

    def name(self, address, name):
        """Record a located table; refuse if the address already carries another name."""
        known = self.syms.get(address)
        if known is not None and known != name:
            raise ValueError(f"{name} at {address:#x} is already {known}")
        self.syms[address] = name
        self.addr[name] = address

    def locate(self):
        """Find the SWIG tables of a version whose data symbols do not name them.

        Layout (identical in 2.6.2, where every step can be checked against the symbols):
        _wrap_class_* blocks, swig_commands {name, func}[] ending in {0,0}, swig_constants
        (48-byte entries ending in a zero entry), the 12 _swigt_ type records, the 12 _swigc_
        cast arrays, swig_module, swig_type_initial[12], swig_cast_initial[12]. swig_types and
        swig_cast_initial are named in every version."""
        rel, addr = self.relocations, self.addr
        modules = [a for a, target in rel.items() if target == addr["swig_types"]]
        if len(modules) != 1:
            raise ValueError(f"{len(modules)} pointers to swig_types; expected only swig_module")
        module = modules[0]
        if struct.unpack("<i", self.raw(module + 8, 4))[0] != TYPE_COUNT:
            raise ValueError("swig_module.size is not 12")
        type_initial = module + 48
        cast_initial = type_initial + TYPE_COUNT * 8
        named = addr.get("swig_cast_initial")
        if named != cast_initial:
            # 2.7.7's data_symbols.csv puts the name 8 bytes too far (0x1129bd0, table at 0x1129bc8).
            self.misnamed = {"swig_cast_initial": (named, cast_initial)}
            self.syms.pop(named, None)
        self.name(module, "swig_module")
        self.name(type_initial, "swig_type_initial")
        self.name(cast_initial, "swig_cast_initial")
        types = [rel[type_initial + 8 * i] for i in range(TYPE_COUNT)]
        for t in types:
            self.name(t, "_swigt_" + json.loads(self.text(rel[t])))
        for i in range(TYPE_COUNT):
            cast = rel[cast_initial + 8 * i]
            self.name(cast, self.syms[rel[cast]].replace("_swigt_", "_swigc_", 1))
        # Constants: walk back from the first type record over 48-byte entries whose name is a
        # relocated pointer and whose kind is a SWIG constant kind; the last entry is all zero.
        end = min(types)
        if any(self.raw(end - CONST_STRIDE, CONST_STRIDE)):
            raise ValueError("swig_constants does not end in a zero entry")
        start = end - CONST_STRIDE
        while True:
            entry = start - CONST_STRIDE
            kind, = struct.unpack("<i", self.raw(entry, 4))
            if not (1 <= kind <= 4 and entry + 8 in rel):
                break
            start = entry
        self.name(start, "swig_constants")
        self.counts["swig_constants"] = (end - start) // CONST_STRIDE
        # Commands: {name, func} pairs, both relocated, ending in {0,0} just before the constants.
        end = start
        if any(self.raw(end - COMMAND_STRIDE, COMMAND_STRIDE)) or end - COMMAND_STRIDE in rel:
            raise ValueError("swig_commands does not end in a zero entry")
        start = end - COMMAND_STRIDE
        while self.is_command(start - COMMAND_STRIDE):
            start -= COMMAND_STRIDE
        self.name(start, "swig_commands")
        self.counts["swig_commands"] = (end - start) // COMMAND_STRIDE
        # SWIG names every command wrapper _wrap_<command>; functions.csv leaves a few new ones
        # unnamed (sub_...), so they take that name here and are listed in self.inferred.
        self.inferred = {}
        for entry in range(start, end - COMMAND_STRIDE, COMMAND_STRIDE):
            function = rel[entry + 8]
            known = self.syms.get(function, "")
            if not known or known.startswith("sub_"):
                command = json.loads(self.text(rel[entry]))
                self.inferred[function] = (known, "_wrap_" + command)
                self.syms[function] = "_wrap_" + command
                self.addr["_wrap_" + command] = function
        # Classes: the 56-byte swig_lua_class blocks just before swig_commands, found by their
        # name pointer; each points at its methods, attributes and bases arrays.
        for cls, _, _ in CLASSES:
            hits = [a for a in range(start - 0x200, start, 8)
                    if a in rel and self.raw(rel[a], len(cls) + 1) == cls.encode() + b"\0"]
            if len(hits) != 1:
                raise ValueError(f"_wrap_class_{cls}: {len(hits)} candidates")
            a = hits[0]
            self.name(a, "_wrap_class_" + cls)
            for offset, part in ((32, "methods"), (40, "attributes"), (48, "bases")):
                self.name(rel[a + offset], f"swig_{cls}_{part}")

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
    classes = CLASSES
    commands = "swig_commands"
    constants = "swig_constants"
    counts = {
        commands: (source.counts[commands], COMMAND_STRIDE), constants: (source.counts[constants], CONST_STRIDE),
        "swig_types": (13, 8),
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

    emit(f"static const luaL_Reg swig_commands[{counts[commands][0]}] = {{")
    for a in addresses(commands):
        emit("    {%s,%s}," % (text(ptr(a)), ref(a + 8)))
    emit("};")
    emit("static swig_lua_var_info swig_variables[5] = {")
    # This table lost its symbol. The surviving Hydra_Init calls identify the getters.
    for name in ("SGAMEID_INVALID", "NAVMESH_HANDLE_NONE", "MAX_AFFIX_ALLOWED_TYPES", "MAX_AFFIX_EXCLUDED_CATEGORIES"):
        emit('    {"%s",_wrap_%s_get,0},' % (name, name))
    emit("    {0,0,0}\n};")
    emit(f"static swig_lua_const_info swig_constants[{counts[constants][0]}] = {{")
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
        "source": f"data/{source.version}/main.elf", "tables": tables,
        "swig_variables": "No surviving symbol; four entries recovered from Hydra_Init named call/data operands, followed by null terminator.",
    }
    if source.inferred:
        evidence["inferred_wrappers"] = {hex(0x7100000000 + a): {"csv": old, "name": new}
                                         for a, (old, new) in sorted(source.inferred.items())}
    if source.misnamed:
        evidence["misnamed_symbols"] = {name: {"csv": hex(0x7100000000 + old) if old else None,
                                               "table": hex(0x7100000000 + new)}
                                        for name, (old, new) in source.misnamed.items()}
    return "\n".join(out) + "\n", json.dumps(evidence, indent=2) + "\n"


def main(argv=None):
    from validation.elf import Elf
    ap = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    ap.add_argument("--version", default="2.7.7", help="data/<version>/main.elf and its symbol inventories")
    ap.add_argument("--output", type=Path, default=ROOT / "src/X/Common/generated-game-data/lua_tables.inc")
    ap.add_argument("--evidence", type=Path, help="also write the table evidence JSON here")
    ap.add_argument("--check", action="store_true", help="compare with --output instead of writing it")
    args = ap.parse_args(argv)
    image = LuaTableImage(ROOT, Elf(ROOT / "data" / args.version / "main.elf"), args.version)
    text, evidence = generate_lua_tables(image)
    if args.evidence:
        args.evidence.write_text(evidence)
    if args.check:
        same = args.output.is_file() and args.output.read_text() == text
        print(f"{args.output}: {'up to date' if same else 'differs'}")
        return 0 if same else 1
    args.output.write_text(text)
    print(f"wrote {args.output} ({image.counts['swig_commands']} commands, {image.counts['swig_constants']} constants)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
