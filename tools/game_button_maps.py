"""Recover the 96 NpadButtonMap assignments in the Switch 2.6.2 InputGfxInit."""
import json
import re
import subprocess

TABLES = ((0x17713a0, "NPAD_JOY_FULL_MAP"), (0x1771aa0, "NPAD_JOY_LEFT_MAP"),
          (0x17721a0, "NPAD_JOY_RIGHT_MAP"))
MEMORY = re.compile(r"\[(\w+)(?:, #(-?(?:0x[0-9a-f]+|\d+)))?\]")


def generate_buttons(image, nss, objdump):
    listing = subprocess.check_output([objdump, "-d", "--start-address=0xa0810",
                                       "--stop-address=0xa2a0c", str(nss)], text=True)
    # Model SP as zero. Only the temporary aggregate and static map stores are read.
    registers = {"sp": 0, "zr": 0}
    memory = {}
    rows = {}
    pending = None

    def key(reg):
        return reg[1:] if reg.startswith(("w", "x")) else reg

    def read(reg):
        return registers[key(reg)]

    def write(reg, value):
        registers[key(reg)] = value & ((1 << (32 if reg.startswith("w") else 64)) - 1)

    def string(address):
        data = image.read_vaddr(address, 128)
        return json.dumps(data[:data.index(0)].decode("ascii"))

    for line in listing.splitlines():
        match = re.match(r"\s*[0-9a-f]+:\s+(?:[0-9a-f]{8}|(?:[0-9a-f]{2}\s+){4})\s*(\w+)\s*(.*)", line)
        if not match:
            continue
        op, operands = match.groups()
        args = re.split(r",\s*(?![^\[]*\])", operands.partition("//")[0].strip())
        dest = args[0]
        if op == "mov":
            write(dest, int(args[1][1:], 0) if args[1].startswith("#") else read(args[1]))
        elif op == "orr" and args[1] in ("wzr", "xzr"):
            write(dest, int(args[2][1:], 0))
        elif op == "movk":
            shift = int(args[2].split("#")[1], 0) if len(args) == 3 else 0
            write(dest, (read(dest) & ~(0xffff << shift)) | (int(args[1][1:], 0) << shift))
        elif op == "adrp":
            write(dest, int(args[1].split()[0], 16))
        elif op == "add":
            write(dest, read(args[1]) + int(args[2][1:], 0))
        elif op in ("str", "stur", "stp", "ldr", "ldp"):
            base, offset = MEMORY.fullmatch(args[-1]).groups()
            address = read(base) + int(offset or "0", 0)
            for reg in args[:-1]:
                if op.startswith("ld"):
                    write(reg, memory[address])
                else:
                    # Callee-saved entry registers may be unknown in the prologue.
                    memory[address] = registers.get(key(reg), 0)
                address += 4 if reg.startswith("w") else 8
        elif op == "bl" and "C2EPKc" in operands:
            pending = (memory[8], memory[16], memory[24], memory[32], read("x1"))
        elif op == "bl" and "aSERKS" in operands:
            dest_addr = read("x0") - 32
            if pending is None or dest_addr in rows:
                raise ValueError("unexpected controller map assignment")
            mask, test, index, label, icon = pending
            if tuple(memory[dest_addr + n] for n in (0, 8, 16, 24)) != pending[:4]:
                raise ValueError("controller map stores disagree with constructor arguments")
            rows[dest_addr] = f'{{ {{ {mask:#x} }}, {{ {test:#x} }}, {index}, {string(label)}, {string(icon)} }}'
            pending = None
        elif op in ("sub", "ret") or (op == "bl" and "D2Ev" in operands):
            continue
        else:
            raise ValueError(f"unsupported instruction in InputGfxInit: {op} {operands}")

    lines = []
    for mode_index, mode in enumerate(("MAP_GAME", "MAP_UI")):
        for table_index, (base, table) in enumerate(TABLES):
            if lines:
                lines.extend([""] * (2 if mode_index and table_index == 0 else 1))
            for index in range(16):
                row = rows.pop(base + 56 * (16 * mode_index + index))
                lines.append(f"    {table}[{mode}][{index}] = {row};")
    if rows or pending:
        raise ValueError("unexpected extra controller map rows")
    return "\n".join(lines) + "\n"
