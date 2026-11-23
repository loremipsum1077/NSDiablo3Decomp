#!/usr/bin/env python3
"""Per-function proof debt of tier-1/historical O rows: checker failures per mode.

  python3 tools/proof_debt.py --normal N/result.json --lua L/result.json
  python3 tools/proof_debt.py --normal ... --lua ... --check   # exit 1 if stale

Reads checker batch results (`{"format":1,"results":[{"address","name","status",
"reason"}]}`) for both modes plus data/2.6.2/functions.csv, and writes one row per
Quality O identity and mode whose status is not `ok`. An O identity absent from a
result is debt too (`no-result`). Results must come from a complete batch request;
the ledger records gaps, it never proves the absence of other gaps.

Columns
  Address, Name   exact inventory identity
  Mode            normal or lua
  Category        stable kebab-case slug of the reason's first line (CATEGORIES;
                  non-mismatch statuses use the status; anything else is `other`)
  Reason          first line of the checker reason, addresses kept

Rows sort by Address, Name, then normal before lua. Output is deterministic.
"""
import argparse
import csv
import io
import json
import os
from pathlib import Path
import re
import sys
import uuid

ROOT = Path(__file__).resolve().parent.parent
FUNCTIONS = ROOT / 'data/2.6.2/functions.csv'
LEDGER = ROOT / 'data/2.6.2/proof_debt.csv'
COLUMNS = ['Address', 'Name', 'Mode', 'Category', 'Reason']
MODES = ('normal', 'lua')
STATUSES = ('ok', 'mismatch', 'not-checkable', 'missing', 'error')
# Ordered: the first prefix of the reason (after "mismatch at 0x...: ") wins.
CATEGORIES = (
    ('unverified GOT reference: original GOT target lacks known data identity', 'got-original-unknown'),
    ('unverified GOT reference: rebuilt undefined GOT target symbol', 'got-rebuilt-undefined'),
    ('unverified GOT reference: original undefined GOT target symbol', 'got-original-undefined'),
    ('unverified GOT reference: GOT target or interior offset differs', 'got-target-differs'),
    ('unverified GOT reference', 'got-unverified'),
    ('unproven ADRP pair: reachable indirect jump', 'adrp-indirect-flow'),
    ('unproven ADRP pair: no common live page pair', 'adrp-conflicting-flow'),
    ('unproven ADRP pair', 'adrp-unproven'),
    ('unproven jump table: jump-table guard cannot be proved with unresolved indirect', 'jump-table-indirect-guard'),
    ('unproven jump table', 'jump-table-unproven'),
    ('wrong function size', 'wrong-size'),
    ('wrong function call', 'wrong-call'),
    ('wrong branch target', 'wrong-branch-target'),
    ('wrong mnemonic', 'wrong-mnemonic'),
    ('instruction differs outside a supported relocation immediate', 'instruction-differs'),
    ('unknown reason', 'unknown-reason'),
)
SITE = re.compile(r'mismatch at 0x[0-9a-fA-F]+: ')


class Error(ValueError):
    pass


def first_line(reason):
    return reason.split('\n', 1)[0].strip()


def category(status, reason):
    if status != 'mismatch':
        return status
    text = SITE.sub('', first_line(reason), count=1)
    for prefix, slug in CATEGORIES:
        if text.startswith(prefix):
            return slug
    return 'other'


def load_results(path):
    """Exact identity -> row of one checker batch result; malformed input fails."""
    value = json.loads(Path(path).read_text())
    if not isinstance(value, dict) or set(value) != {'format', 'results'} or value['format'] != 1 \
            or not isinstance(value['results'], list):
        raise Error('Malformed checker result: ' + str(path))
    out = {}
    for row in value['results']:
        if (not isinstance(row, dict) or set(row) != {'address', 'name', 'status', 'reason'} or
                not all(isinstance(row[k], str) for k in row) or row['status'] not in STATUSES or
                not re.fullmatch(r'0x[0-9a-fA-F]+', row['address']) or not row['name']):
            raise Error('Malformed checker row in ' + str(path))
        identity = row['address'], row['name']
        if identity in out:
            raise Error('Duplicate checker identity in ' + str(path))
        out[identity] = row
    return out


def debt(identities, results):
    """Rows for the given (address, name) identities; results maps mode -> load_results."""
    if set(results) != set(MODES):
        raise Error('Both normal and lua results are required')
    rows = []
    for identity in identities:
        for mode in MODES:
            row = results[mode].get(identity)
            if row is None:
                rows.append(dict(zip(COLUMNS, (*identity, mode, 'no-result', 'no checker result'))))
            elif row['status'] != 'ok':
                rows.append(dict(zip(COLUMNS, (*identity, mode, category(row['status'], row['reason']),
                                                first_line(row['reason'])))))
    return ordered(rows)


def ordered(rows):
    return sorted(rows, key=lambda r: (r['Address'], r['Name'], MODES.index(r['Mode'])))


def build(functions, results):
    with Path(functions).open(newline='') as stream:
        inventory = list(csv.DictReader(stream))
    ids = [(r['Address'], r['Name']) for r in inventory if r['Quality'] == 'O']
    if len(ids) != len(set(ids)):
        raise Error('Duplicate O identity in ' + str(functions))
    return debt(ids, results)


def render(rows):
    stream = io.StringIO()
    writer = csv.DictWriter(stream, COLUMNS, lineterminator='\n')
    writer.writeheader()
    writer.writerows(ordered(rows))
    return stream.getvalue().encode()


def read(path=LEDGER):
    with Path(path).open(newline='') as stream:
        reader = csv.DictReader(stream)
        if reader.fieldnames != COLUMNS:
            raise Error('Unexpected proof-debt columns: ' + str(path))
        return list(reader)


def write(rows, path=LEDGER):
    """Atomic replace: readers see the old or the new ledger, never a partial one."""
    path = Path(path)
    if path.is_symlink():
        raise Error('Symlink output refused: ' + str(path))
    temporary = path.with_name(path.name + '.' + uuid.uuid4().hex + '.tmp')
    with temporary.open('xb') as stream:
        stream.write(render(rows))
        stream.flush()
        os.fsync(stream.fileno())
    os.replace(temporary, path)


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('--normal', type=Path, required=True, help='normal-mode checker result.json')
    ap.add_argument('--lua', type=Path, required=True, help='Lua-mode checker result.json')
    ap.add_argument('--functions', type=Path, default=FUNCTIONS)
    ap.add_argument('--output', type=Path, default=LEDGER)
    ap.add_argument('--check', action='store_true', help='compare only; exit 1 when the ledger differs')
    args = ap.parse_args(argv)
    try:
        rows = build(args.functions, {'normal': load_results(args.normal), 'lua': load_results(args.lua)})
        if args.check:
            current = args.output.read_bytes() if args.output.is_file() else b''
            if current != render(rows):
                print(f'{args.output}: stale ({len(rows)} expected rows)')
                return 1
            print(f'{args.output}: current ({len(rows)} rows)')
            return 0
        write(rows, args.output)
    except (Error, OSError, json.JSONDecodeError, KeyError) as error:
        print('proof_debt: ' + str(error), file=sys.stderr)
        return 1
    counts = {}
    for row in rows:
        counts[row['Category']] = counts.get(row['Category'], 0) + 1
    print(f'{args.output}: {len(rows)} rows, {len({(r["Address"], r["Name"]) for r in rows})} functions')
    for name, count in sorted(counts.items(), key=lambda x: (-x[1], x[0])):
        print(f'  {count:6d}  {name}')
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
