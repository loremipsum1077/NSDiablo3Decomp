"""Recover the 2.6.2 registration calls using LLVM's annotated AArch64 disassembly."""
import csv
import json
import re
import struct
import subprocess

from data import relative_relocs


def enum_names(header, pattern):
    return {int(value): name for name, value in re.findall(
        rf"^\s*({pattern})\s*=\s*(-?\d+),", header.read_text(), re.MULTILINE)}


def flag_expression(bits, names):
    if bits >> len(names):
        raise ValueError("unexpected flag bits")
    return " | ".join(f"(1 << {name})" for i, name in enumerate(names) if bits & (1 << i)) or "0"


def lookup_calls(image, nss, objdump):
    listing = subprocess.check_output([
        objdump, "-dl", "--start-address=0x440fc0", "--stop-address=0x450c60", str(nss)
    ], text=True)
    relocations = relative_relocs(image)
    registers = {"zr": 0}
    source_line = None
    for line in listing.splitlines():
        location = re.search(r"GlobalSNO\.cpp:(\d+)", line)
        if location:
            source_line = int(location[1])
        instruction = re.match(r"\s*[0-9a-f]+:\s+(?:[0-9a-f]{8}|(?:[0-9a-f]{2} ){4})\s+(\S+)\s*(.*)", line)
        if not instruction:
            continue
        opcode, arguments = instruction.groups()
        args = arguments.partition("//")[0].strip().split(", ")
        if opcode in ("b", "bl"):
            if "sLookupSNO" in arguments:
                yield source_line, [registers.get(str(i)) for i in range(6)]
            if opcode == "bl":
                for i in range(19):
                    registers.pop(str(i), None)
            continue
        if not re.fullmatch(r"[wx]\d+", args[0]):
            continue
        dest = args[0][1:]
        if opcode == "mov":
            operand = args[1]
            registers[dest] = int(operand[1:], 0) if operand.startswith("#") else registers.get(operand[1:])
        elif opcode == "orr" and args[1] in ("xzr", "wzr") and args[2].startswith("#"):
            registers[dest] = int(args[2][1:], 0)
        elif opcode == "adrp":
            registers[dest] = int(args[1].split()[0], 16)
        elif opcode == "add" and args[2].startswith("#") and isinstance(registers.get(args[1][1:]), int):
            registers[dest] = registers[args[1][1:]] + int(args[2][1:], 0)
        elif opcode == "ldr" and args[0].startswith("x"):
            base = registers.get(args[1].strip("[]")[1:])
            if len(args) == 3 and isinstance(base, int) and args[2].startswith("#"):
                registers[dest] = ("got", relocations.get(base + int(args[2][1:].rstrip("]"), 0)))
            elif len(args) == 2 and isinstance(base, tuple) and base[0] == "got":
                registers[dest] = ("symbol", base[1])
            else:
                registers.pop(dest, None)
        elif opcode == "csel" and args == ["w5", "wzr", "w21", "ne"]:
            registers[dest] = "console"
        elif opcode not in ("stp", "str", "cmp", "cmn", "tst", "cbz", "cbnz", "tbz", "tbnz", "ccmp"):
            registers.pop(dest, None)


def generate(root, image, nss, objdump):
    common = root / "src/X/Common"
    global_names = enum_names(common / "GlobalSNO.h", r"GLOBALSNO_(?!FLAG_)\w+")
    gbid_names = enum_names(common / "GlobalSNO.h", r"GLOBALGBID_\w+")
    groups = enum_names(common / "SNO/SNOFiles.h", r"(?!SNOENTRY_)[A-Z0-9_]+_SNO")
    with (root / "data/2.6.2/data_symbols.csv").open() as fp:
        symbols = {int(row["Address"], 16) & 0xffffffff: row["Name"] for row in csv.DictReader(fp)}
    platform = ["SIGMAPLATFORM_" + s for s in ("PC", "AXE", "XBOXONE", "PS4", "NX64")]
    flags = ["GLOBALSNO_FLAG_" + s + "_BIT" for s in
             ("DO_NOT_PRELOAD", "DO_NOT_TRAVERSE_IN_COMMON_CPK", "DO_NOT_TRAVERSE_IN_ACT_CPK_SPLIT")]
    buckets = ["BUCKET_" + s for s in ("TOOLS_AND_TESTING", "PVP", "MAIN", "NON_BETA", "BETA_ONLY")]
    lines = [""] * (2639 - 123 + 1)
    lines[2277 - 123] = "    int32 nExtrasIndex = NUM_GLOBAL_SNOS;"
    for line, (mask, bucket, index, group, string, flag) in lookup_calls(image, nss, objdump):
        if not 123 <= line <= 2639 or lines[line - 123]:
            raise ValueError("unexpected GlobalSNO call location")
        extras = 2279 <= line <= 2544
        index_name = "nExtrasIndex" if extras else global_names[index]
        if isinstance(string, tuple) and string[0] == "symbol":
            name = symbols[string[1]]
        else:
            raw = image.read_vaddr(string, 256)
            name = json.dumps(raw[:raw.index(0)].decode("ascii"))
        flag_text = ("SigmaPlatformIsConsole(SigmaPlatformGet()) ? (1 << GLOBALSNO_FLAG_DO_NOT_TRAVERSE_IN_COMMON_CPK_BIT) : 0"
                     if flag == "console" else flag_expression(flag, flags))
        call = f"sLookupSNO({flag_expression(mask, platform)}, {buckets[bucket]}, {index_name}, {groups[group]}, {name}, {flag_text})"
        lines[line - 123] = f"    if ({call}) nExtrasIndex++;" if extras else f"    {call};"

    types = {1: "GB_ITEMTYPES", 2: "GB_ITEMS", 4: "GB_HIRELINGS", 7: "GB_HEROS", 18: "GB_MONSTER_AFFIXES"}
    rows = []
    for index, name, kind in struct.iter_unpack("<i64si", image.read_vaddr(0x1048a28, 119 * 72)):
        string = json.dumps(name[:name.index(0)].decode("ascii"))
        rows.append(f"    {{ {gbid_names[index]}, {string}, {types[kind]} }},")
    return {"sno_calls.inc": "\n".join(lines) + "\n", "gbid_rows.inc": "\n".join(rows) + "\n"}
