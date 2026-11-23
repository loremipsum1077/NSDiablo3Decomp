"""Reproduce the private SWIG table include and evidence from the prepared local ELF."""
import hashlib
import json
from pathlib import Path
import sys

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "tools"))
from game_lua_tables import LuaTableImage, generate_lua_tables
from validation.elf import Elf

OUT = ROOT / "build/try/lua_wrapper_evidence"


def main():
    source = LuaTableImage(ROOT, Elf(ROOT / "data/2.6.2/main.elf"))
    include, evidence = generate_lua_tables(source)
    manifest = json.loads((ROOT / "config/local_game_data.json").read_text())
    for file, content, pin in (
        ("tables.cpp.inc", include, manifest["includes"]["lua_tables.inc"]),
        ("tables.evidence.json", evidence, manifest["lua_evidence_sha256"]),
    ):
        if hashlib.sha256(content.encode()).hexdigest() != pin:
            raise ValueError(f"SWIG reconstruction differs from reviewed pin: {file}")
    OUT.mkdir(parents=True, exist_ok=True)
    (OUT / "tables.evidence.json").write_text(evidence)
    (OUT / "lua_tables.inc").write_text(include)
    # Keep the original standalone extractor output for external probes. In
    # the adopted wrapper, swig_types must be visible before these tables.
    comment, rest = include.split("\n\n", 1)
    (OUT / "tables.cpp.inc").write_text(
        comment + "\nstatic swig_type_info *swig_types[13] = {};\n" + rest
    )


if __name__ == "__main__":
    main()
