#!/usr/bin/env python3
"""Export revision-pinned, O-only helper evidence without changing data/ or caches.

The cached DWARF index groups template instantiations. Caller evidence is scoped
to observed addresses, never a proof of all specializations or consumer readiness.
"""
import argparse
from collections import Counter
import hashlib
import json
from pathlib import Path
import pickle

import helper_ledger as ledger
import project_plan

ROOT = Path(__file__).resolve().parents[1]


def summarize(index, functions, owners, source_paths, top=25):
    inventory = {ledger.low(f['Address']): f for f in functions}
    file_of = {ledger.low(f['Address']): f['File'] for f in owners}
    rows = []
    all_pending = set()
    old_only = 0

    def examples(addresses):
        result = []
        for address in sorted(addresses, key=lambda a: (int(inventory.get(a, {}).get('Size', 1 << 60)), a))[:3]:
            f = inventory.get(address)
            owner = file_of.get(address, '')
            result.append(dict(address=f['Address'] if f else hex(address),
                               name=f['Name'] if f else None,
                               quality=f['Quality'] if f else None,
                               bytes=int(f['Size']) if f else None,
                               owner=owner, source_job=project_plan.job_id(owner) if owner else None))
        return result

    for key, (callers, sites) in sorted(index['inlined'].items()):
        rec = index['funcs'].get(key)
        if rec is None:
            continue
        callers = {a & 0xFFFFFFFF for a in callers}
        exact = {a for a in callers if inventory.get(a, {}).get('Quality') == 'O'}
        pending = {a for a in callers if a in inventory and inventory[a]['Quality'] != 'O'}
        unknown = set(callers) - inventory.keys()
        accepted = {a for a in callers if inventory.get(a, {}).get('Quality') in ('M', 'm')}
        all_pending.update(pending)
        old_only += bool(accepted and not exact)
        rows.append(dict(owner=key[0], line=key[1], name=key[2],
                         representative_signature=ledger.signature(rec),
                         source_present='src/' + key[0] in source_paths,
                         source_defined=key[0].endswith(ledger.SOURCE_EXT),
                         callers=len(callers), sites=sites, exact_callers=len(exact),
                         pending_callers=len(pending), accepted_nonmatch_callers=len(accepted),
                         unknown_callers=len(unknown), exact_examples=examples(exact),
                         pending_examples=examples(pending), unknown_examples=examples(unknown)))
    rank = lambda r: (-r['pending_callers'], -r['callers'], r['owner'], r['line'], r['name'])
    unproved = sorted((r for r in rows if not r['exact_callers']), key=rank)
    static = sorted((r for r in unproved if r['source_defined']), key=rank)
    proved = sorted((r for r in rows if r['exact_callers']), key=rank)
    quality = Counter(f['Quality'] for f in functions)
    return dict(
        policy='Only committed Quality=O supplies exact caller evidence. M/m/W/U remain open. '
               'Rows group definitions by file/line/template-stripped name; the representative '
               'signature cannot identify each caller specialization. Source presence is not '
               'body, inclusion, or readiness proof. Source-defined does not prove C++ static '
               'linkage. Counts overlap and must not be added as expected gains.',
        inventory=dict(functions=len(functions), exact=quality['O'], quality=dict(sorted(quality.items())),
                       bytes=sum(int(f['Size']) for f in functions),
                       exact_bytes=sum(int(f['Size']) for f in functions if f['Quality'] == 'O')),
        summary=dict(helper_families=len(rows), families_with_exact_callers=len(proved),
                     families_without_exact_callers=len(unproved),
                     families_previously_supported_only_by_accepted_nonmatches=old_only,
                     source_defined_families_without_exact_callers=len(static),
                     distinct_pending_inventory_callers=len(all_pending),
                     skipped_missing_definition_records=len(index['inlined']) - len(rows)),
        selection=dict(top_per_queue=top, example_order='smallest original function bytes, then address',
                       ranking='pending inventory callers descending, total callers descending, owner/line/name'),
        unproved=unproved[:top], source_defined_unproved=static[:top], exact_contexts=proved[:top])


def snapshot(root=ROOT, top=25):
    root = Path(root)
    revision = project_plan.git(root, 'rev-parse', 'HEAD').strip()
    functions, owners = project_plan.inventory(str(root), revision)
    source_paths = set(project_plan.git(root, 'ls-tree', '-r', '--name-only', revision, '--', 'src/').splitlines())
    cache = root / 'build/dwarf/index.pkl'
    raw = cache.read_bytes()  # Read an existing trusted local cache; never rebuild it here.
    index = pickle.loads(raw)
    result = summarize(index, functions, owners, source_paths, top)
    result['provenance'] = dict(revision=revision, index_path='build/dwarf/index.pkl',
                                index_sha256=hashlib.sha256(raw).hexdigest(),
                                index_bytes=len(raw),
                                index_note='Existing local cache; hash pins the input but does not independently validate its DWARF provenance.',
                                inventory_blobs={name: project_plan.git(root, 'rev-parse', f'{revision}:data/2.6.2/{name}.csv').strip()
                                                 for name in ('functions', 'function_files')})
    return result


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--top', type=int, default=25)
    parser.add_argument('--out', type=Path, help='Write JSON here; otherwise print to stdout')
    args = parser.parse_args()
    if args.top < 1:
        parser.error('--top must be positive')
    text = json.dumps(snapshot(top=args.top), indent=2) + '\n'
    if args.out:
        args.out.parent.mkdir(parents=True, exist_ok=True)
        args.out.write_text(text)
    else:
        print(text, end='')


if __name__ == '__main__':
    main()
