#!/usr/bin/env python3
"""Export Ghidra's decompiled C for every function to build/ghidra/decomp/,
one file per original source file (build/ghidra/decomp/X/Client/CGame.cpp).

On first use this imports build/romfs/DiabloIIINX64r.nss into a new Ghidra
project with its DWARF applied and the image base at 0x7100000000, so Ghidra
addresses equal those in functions.csv. Later runs reuse the project. The
project must not be open in the Ghidra app while this runs.

Needs a Ghidra install: set GHIDRA_INSTALL_DIR or pass --ghidra.
"""
import argparse
import os
import shutil
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
PROGRAM = "DiabloIIINX64r.nss"


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--ghidra", type=Path, default=os.environ.get("GHIDRA_INSTALL_DIR"),
                    help="Ghidra install directory (default: $GHIDRA_INSTALL_DIR)")
    ap.add_argument("--project-dir", type=Path, default=ROOT / "build/ghidra")
    ap.add_argument("--project-name", default="d3_decomp_262")
    ap.add_argument("--out", type=Path, default=ROOT / "build/ghidra/decomp")
    ap.add_argument("--maxmem", default="7G", help="Java heap for the headless run")
    args = ap.parse_args()

    if not args.ghidra:
        sys.exit("set GHIDRA_INSTALL_DIR or pass --ghidra")
    headless = Path(args.ghidra) / "support/analyzeHeadless"
    if not headless.is_file():
        sys.exit(f"{headless} not found")
    env = dict(os.environ, MAXMEM=args.maxmem)
    project = [str(headless), str(args.project_dir.resolve()), args.project_name]
    args.project_dir.mkdir(parents=True, exist_ok=True)

    if not (args.project_dir / f"{args.project_name}.gpr").is_file():
        nss = ROOT / "build/romfs" / PROGRAM
        if not nss.is_file():
            sys.exit(f"{nss} not found")
        print(f">>> importing {PROGRAM} into a new Ghidra project (about 10 minutes)")
        subprocess.run([*project, "-import", str(nss), "-loader", "ElfLoader",
                        "-loader-imagebase", "7100000000", "-analysisTimeoutPerFile", "14400"],
                       check=True, env=env)

    shutil.rmtree(args.out, ignore_errors=True)
    args.out.mkdir(parents=True)
    print(f">>> decompiling to {args.out}")
    subprocess.run([*project, "-process", PROGRAM, "-noanalysis", "-readOnly",
                    "-scriptPath", str(ROOT / "tools/ghidra"),
                    "-postScript", "ExportDecomp.java", str(ROOT / "data/2.6.2"), str(args.out.resolve())],
                   check=True, env=env)


if __name__ == "__main__":
    main()
