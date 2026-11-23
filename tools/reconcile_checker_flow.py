#!/usr/bin/env python3
"""Audit checker regression failures without modifying inventory or game assets.

Reads a checker_regression.py report and verifies its inventory/ELF SHA-256 pins.
Prints JSON by default; --output explicitly saves evidence outside data/. Branch
classification scans original words, not inferred source correctness or a CFG.
"""
import argparse
from collections import Counter
import csv
import hashlib
import io
import json
from pathlib import Path
import re
import struct

ROOT = Path(__file__).resolve().parents[1]
INVENTORY = 'data/2.6.2/functions.csv'
ELF = 'data/2.6.2/main.elf'
OWNERS = 'data/2.6.2/function_files.csv'
BASE = 0x7100000000


def digest(data):
    return hashlib.sha256(data).hexdigest()


def load_segments(blob):
    if blob[:6] != b'\x7fELF\x02\x01' or struct.unpack_from('<H', blob, 18)[0] != 183:
        raise ValueError('Expected little-endian ELF64/AArch64 original image')
    offset = struct.unpack_from('<Q', blob, 32)[0]
    size, count = struct.unpack_from('<HH', blob, 54)
    if size < 56:
        raise ValueError('Invalid ELF program-header size')
    return [struct.unpack_from('<IIQQQQQQ', blob, offset + i * size)
            for i in range(count)]


def extract(blob, segments, address, size):
    matches = [s for s in segments if s[0] == 1
               and s[3] <= address and address + size <= s[3] + s[5]]
    if len(matches) != 1 or size <= 0 or size % 4:
        raise ValueError(f'Invalid or ambiguous function range {address:#x}+{size:#x}')
    start = matches[0][2] + address - matches[0][3]
    code = blob[start:start + size]
    if len(code) != size:
        raise ValueError('Truncated ELF function bytes')
    return code


def reconcile(report, inputs):
    for relative in (INVENTORY, ELF):
        expected = report['inputs_sha256'].get(relative)
        if expected != digest(inputs[relative]):
            raise ValueError(f'Regression input hash does not match: {relative}')
    rows = list(csv.DictReader(io.StringIO(inputs[INVENTORY].decode())))
    inventory = {}
    for row in rows:
        key = (int(row['Address'], 16), row['Name'])
        if key in inventory:
            raise ValueError(f'Duplicate inventory identity: {key}')
        inventory[key] = row
    owners = {}
    for row in csv.DictReader(io.StringIO(inputs[OWNERS].decode())):
        owners.setdefault(int(row['Address'], 16), set()).add(row['File'])
    segments = load_segments(inputs[ELF])
    evidence = []
    seen = set()
    for failure in report['failures']:
        identity = (int(failure['address'], 16), failure['name'])
        if identity in seen:
            raise ValueError(f'Duplicate failure: {identity}')
        seen.add(identity)
        row = inventory[identity]
        if failure['status'] == 'ok':
            raise ValueError('Failure list contains an ok result')
        address = identity[0] - BASE
        size = int(row['Size'])
        code = extract(inputs[ELF], segments, address, size)
        branches = []
        for offset in range(0, size, 4):
            word = struct.unpack_from('<I', code, offset)[0]
            at = address + offset
            branch = {'address': hex(at), 'word': f'0x{word:08x}'}
            if word & 0xfffffc1f == 0xd61f0000:
                branch.update(kind='indirect_br', register=f'x{(word >> 5) & 31}')
            elif word & 0xfc000000 == 0x94000000:
                immediate = word & 0x3ffffff
                if immediate & (1 << 25):
                    immediate -= 1 << 26
                target = at + immediate * 4
                if not address <= target < address + size:
                    continue
                branch.update(kind='recursive_entry_bl' if target == address else 'interior_bl',
                              target=hex(target))
            else:
                continue
            branches.append(branch)
        kinds = {b['kind'] for b in branches}
        category = next((kind for kind in ('indirect_br', 'interior_bl', 'recursive_entry_bl')
                         if kind in kinds), 'other')
        reason = failure.get('reason', '')
        mismatch = re.search(r'mismatch at (0x[0-9a-fA-F]+)', reason)
        mismatch_word = None
        if mismatch:
            offset = int(mismatch[1], 16) - address
            if 0 <= offset <= size - 4 and offset % 4 == 0:
                mismatch_word = f'0x{struct.unpack_from("<I", code, offset)[0]:08x}'
        evidence.append({**failure, 'prior_quality': row['Quality'], 'original_bytes': size,
                         'original_owner_candidates': sorted(owners.get(identity[0], [])),
                         'category': category, 'branches': branches,
                         'mismatch_instruction_word': mismatch_word})
    prior_o = [entry for entry in evidence if entry['prior_quality'] == 'O']
    return {
        'format': 1,
        'note': 'Read-only reconciliation. Branch presence does not prove reachability or incorrect source. '
                'Original owner metadata is recorded as evidence, not independently authenticated.',
        'inputs_sha256': {path: digest(data) for path, data in inputs.items()},
        'candidate_checker_sha256': report['checker_sha256'],
        'failure_count': len(evidence),
        'prior_O_failure_count': len(prior_o),
        'prior_O_failure_bytes': sum(entry['original_bytes'] for entry in prior_o),
        'categories': dict(Counter(entry['category'] for entry in evidence)),
        'category_original_bytes': dict(Counter({kind: sum(e['original_bytes'] for e in evidence
                                                           if e['category'] == kind)
                                               for kind in {e['category'] for e in evidence}})),
        'functions': evidence,
    }


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('report', type=Path)
    parser.add_argument('--root', type=Path, default=ROOT, help='Repository or preserved snapshot')
    parser.add_argument('--output', type=Path, help='Evidence destination; default stdout')
    args = parser.parse_args()
    root = args.root.resolve()
    report_bytes = args.report.read_bytes()
    inputs = {relative: (root / relative).read_bytes() for relative in (INVENTORY, ELF, OWNERS)}
    result = reconcile(json.loads(report_bytes), inputs)
    result['candidate_report_sha256'] = digest(report_bytes)
    for relative, data in inputs.items():
        if digest((root / relative).read_bytes()) != digest(data):
            raise ValueError(f'Input changed during audit: {relative}')
    rendered = json.dumps(result, indent=2, sort_keys=True) + '\n'
    if args.output:
        output = args.output.resolve()
        if output == args.report.resolve() or output.is_relative_to(root / 'data'):
            raise ValueError('Output must not overwrite the candidate report or data/')
        output.parent.mkdir(parents=True, exist_ok=True)
        output.write_text(rendered)
    else:
        print(rendered, end='')
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
