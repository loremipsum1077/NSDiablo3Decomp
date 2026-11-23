#!/usr/bin/env python3
"""Inventory undefined symbols in an existing AArch64 ELF; never builds or links.

Exact-name inventory hits are leads, not proof of identity, reachability, or a
provider. Both symbol tables can repeat an import, so each row is a name plus
binding and type, with its table occurrences retained.
"""
import argparse
from collections import Counter, defaultdict
import csv
import hashlib
import io
import json
from pathlib import Path

from data_elf import inspect_elf

ROOT = Path(__file__).resolve().parent.parent


def read_inventory(path, required):
    blob = Path(path).read_bytes()
    with io.StringIO(blob.decode('utf-8-sig'), newline='') as stream:
        reader = csv.DictReader(stream)
        if not set(required) <= set(reader.fieldnames or []):
            raise ValueError(f'{path}: missing required columns')
        rows = list(reader)
    return rows, hashlib.sha256(blob).hexdigest()


def classify(symbols, functions, data, owners):
    """Return exact-name evidence without conflating ambiguity or weak imports."""
    owner_index = defaultdict(set)
    for row in owners:
        owner_index[row['Address']].add(row['File'])
    indexes = [defaultdict(list), defaultdict(list)]
    for row in functions:
        indexes[0][row['Name']].append({'address': row['Address'],
            'owners': sorted(owner_index[row['Address']])})
    for row in data:
        indexes[1][row['Name']].append({'address': row['Address']})
    imports = {}
    for sym in symbols:
        if not sym['name'] or sym['defined'] or sym['binding'] == 0:
            continue
        key = (sym['name'], sym['binding'], sym['type'])
        if key not in imports:
            f, d = (idx.get(sym['name'], []) for idx in indexes)
            category = ('function_and_data_inventory' if f and d else
                        'function_inventory' if f else 'data_inventory' if d else 'unclassified')
            imports[key] = {'name': sym['name'], 'binding': sym['binding'],
                'elf_type': sym['type'], 'category': category,
                'function_candidates': f, 'data_candidates': d, 'occurrences': []}
        imports[key]['occurrences'].append({'table_section': sym['table_section'], 'index': sym['index']})
    return [imports[k] for k in sorted(imports)]


def audit(elf, functions, data, owners):
    parsed = inspect_elf(elf)
    metadata = {'elf': {'path': str(elf), 'sha256': parsed['sha256']}}
    inventories = []
    for name, path, columns in (('functions', functions, ('Name', 'Address')),
                                ('data', data, ('Name', 'Address')),
                                ('owners', owners, ('Address', 'File'))):
        inventory, sha = read_inventory(path, columns)
        inventories.append(inventory)
        metadata[name] = {'path': str(path), 'sha256': sha}
    rows = classify(parsed['symbols'], *inventories)
    return {'inputs': metadata, 'summary': {'distinct_name_binding_type': len(rows),
        'categories': dict(sorted(Counter(row['category'] for row in rows).items())),
        'bindings': dict(sorted(Counter(str(row['binding']) for row in rows).items()))},
        'limitations': ['Snapshot of the supplied ELF; freshness relative to source is not established.',
          'Exact-name inventory hits are candidates, not proof of identity or linkability.',
          'Undefined weak symbols may legally remain unresolved; unclassified does not mean external.',
          'No reachability, runtime behavior, platform choice, assets, or executable readiness is established.'],
        'imports': rows}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--elf', type=Path, default=ROOT / 'build/2.6.2/d3')
    parser.add_argument('--functions', type=Path, default=ROOT / 'data/2.6.2/functions.csv')
    parser.add_argument('--data', type=Path, default=ROOT / 'data/2.6.2/data_symbols.csv')
    parser.add_argument('--owners', type=Path, default=ROOT / 'data/2.6.2/function_files.csv')
    parser.add_argument('--summary', action='store_true', help='omit individual imports')
    args = parser.parse_args()
    result = audit(args.elf, args.functions, args.data, args.owners)
    if args.summary:
        result.pop('imports')
    print(json.dumps(result, indent=2))


if __name__ == '__main__':
    main()
