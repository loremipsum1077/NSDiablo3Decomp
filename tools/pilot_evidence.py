#!/usr/bin/env python3
"""Bounded original assembly, without recompiling: python3 tools/pilot_evidence.py NAME.
Use exact mangled names from the assigned packet. Reads only original binary data.
--lines adds the source file:line of every instruction and symbolic operands;
tools/line_evidence.py resolves the same listing further (callees, data, constants).
"""
import argparse
import csv
from pathlib import Path
import subprocess
import shutil

ROOT = Path(__file__).resolve().parents[1]
LIMIT = 16384

def locate(name):
    """(address, size) of one exact, unique inventory name, or None."""
    with (ROOT/'data/2.6.2/functions.csv').open() as stream:
        rows=[r for r in csv.DictReader(stream) if r['Name']==name]
    if len(rows)!=1: return None
    return int(rows[0]['Address'],16)&0xffffffff, int(rows[0]['Size'])

def objdump():
    return shutil.which('llvm-objdump') or '/opt/homebrew/opt/llvm/bin/llvm-objdump'

def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--lines', action='store_true', help='annotate every instruction with its source file:line')
    parser.add_argument('name')
    args=parser.parse_args()
    found=locate(args.name)
    if not found: parser.error('Choose an exact, unique inventory name')
    address,size=found
    if not 0<size<=LIMIT: parser.error('Function exceeds the 16 KiB assembly bound')
    flags=['-l','--symbolize-operands'] if args.lines else []
    subprocess.run([objdump(),'-d','--no-show-raw-insn',*flags,f'--start-address={address}',
                    f'--stop-address={address+size}',str(ROOT/'build/romfs/DiabloIIINX64r.nss')],check=True)

if __name__=='__main__':main()
