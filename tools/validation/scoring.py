"""CPU-only provisional masked instruction scoring; acceptance gates are separate."""
from pathlib import Path
import struct
from .elf import Elf


def score(objdir, targets, orig):
    """(exact matches, same-size count, matching-instruction fraction, per-function detail)"""
    exact = same_size = 0
    insn_ok = insn_total = 0
    detail = {}
    for src, fns in targets.items():
        obj = objdir / (Path(src).name + ".o")
        built = Elf(obj).functions() if obj.is_file() else {}
        for name, (vaddr, size) in fns.items():
            if name not in built:
                detail[name] = "missing"
                continue
            insn_total += size // 4
            code, masks = built[name]
            if len(code) != size:
                detail[name] = f"size {len(code)} != {size}"
                continue
            same_size += 1
            want = orig.read_vaddr(vaddr, size)
            bad = 0
            for i in range(0, size, 4):
                keep = ~masks.get(i, 0) & 0xFFFFFFFF
                a, = struct.unpack_from("<I", code, i)
                b, = struct.unpack_from("<I", want, i)
                # the NX compiler writes traps as udf #0xdefe; ours are brk (tools/check agrees)
                if b == 0xE7FFDEFE and a & 0xFFE0001F == 0xD4200000:
                    continue
                bad += (a & keep) != (b & keep)
            insn_ok += size // 4 - bad
            exact += bad == 0
            detail[name] = "OK" if bad == 0 else f"{bad}/{size // 4} insns differ"
    return exact, same_size, insn_ok / max(insn_total, 1), detail
