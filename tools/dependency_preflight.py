#!/usr/bin/env python3
"""Read-only vtable dependency hints; never an acceptance or equivalence test."""
import argparse
import csv
from collections import Counter
import json
import sys
from pathlib import Path

from data_elf import inspect_elf
from data_relocations import RelocationImage
from recovery_data import vtable_section
from recovery_mapping import inspect as inspect_object
from validation.elf import Elf
from validation.common import Halt
from verify_original_inputs import fingerprint, InputError


def require(condition, message):
    if not condition:
        raise ValueError(message)


def pin(item):
    require(isinstance(item, dict) and set(item) == {'path', 'sha256'}, 'invalid pin')
    require(isinstance(item['path'], str) and isinstance(item['sha256'], str) and len(item['sha256']) == 64, 'invalid pin fields')
    data = fingerprint(Path(item['path']))
    require(data['sha256'] == item['sha256'], 'input hash mismatch: ' + item['path'])
    return data


def unique_rows(rows, fields):
    return [dict(zip(fields, row)) for row in sorted({tuple(x[k] for k in fields) for x in rows})]


def slot_report(index, entry, original, baseline, candidate, inventory):
    """All names are observations. No provider choice or semantic inference."""
    reasons = []
    name = entry['symbol']['name']
    aliases = unique_rows([s for s in original if s['address'] == entry['original_target'] and s['type'] == 2 and s['size'] > 0], ('name','address','size','binding','visibility'))
    if not aliases:
        reasons.append('original_target_identity_missing')
    extents = {(x['address'], x['size']) for x in aliases}
    if len(extents) != 1:
        reasons.append('original_target_extent_ambiguous')
    if name not in {a['name'] for a in aliases}:
        reasons.append('original_candidate_alias_unresolved')
    baseline_named = [s for s in baseline if s['name'] == name and s['defined']]
    if any(s['type'] != 2 or not s.get('executable', True) or s['binding'] not in (1, 2) or s['visibility'] != 0 for s in baseline_named):
        reasons.append('baseline_symbol_metadata_unsupported')
    base = unique_rows([s for s in baseline_named if s['type'] == 2], ('name','address','size','binding','visibility'))
    candidate_named = [s for s in candidate if s['name'] == name]
    if any(s['type'] != 2 or not s.get('executable', True) or s['binding'] not in (1, 2) or s['other'] != 0 for s in candidate_named):
        reasons.append('candidate_symbol_metadata_unsupported')
    copies = unique_rows([s for s in candidate_named if s['type'] == 2], ('name','section','offset','size','binding','other'))
    if not base and not copies:
        reasons.append('provider_missing')
    if copies:
        reasons.append('candidate_provider_selection_pending')
    if len(base) > 1:
        reasons.append('baseline_provider_ambiguous')
    if len(copies) > 1:
        reasons.append('candidate_provider_ambiguous')
    oldsize = next(iter(extents))[1] if len(extents) == 1 else None
    if oldsize is not None and any(s['size'] != oldsize for s in base):
        reasons.append('baseline_extent_differs')
    if oldsize is not None and any(s['size'] != oldsize for s in copies):
        reasons.append('candidate_extent_differs')
    rows = [x for x in inventory if x['name'] in {a['name'] for a in aliases} and x['address'] == entry['original_target']]
    return {'slot': index, 'candidate_symbol': name, 'original_target': entry['original_target'], 'original_aliases': aliases,
            'original_inventory': rows, 'baseline_providers': base, 'candidate_providers': copies,
            'baseline_extent_compatible': bool(base) and oldsize is not None and all(s['size'] == oldsize for s in base),
            'reasons': reasons, 'semantic_equivalence': False, 'provider_selection_proven': False}


def run(request):
    require(isinstance(request, dict) and set(request) == {'inputs','vtable'}, 'invalid request')
    inputs = request['inputs']
    require(isinstance(inputs, dict) and set(inputs) == {'owner','original','nss','baseline','functions'}, 'invalid inputs')
    before = {k: pin(v) for k,v in inputs.items()}
    paths = {k: v['path'] for k,v in inputs.items()}
    require(isinstance(request['vtable'], str) and request['vtable'].startswith('_ZTV'), 'invalid vtable')
    snap = inspect_object(paths['owner'])
    tables = [s for s in snap['symbols'] if s['name'] == request['vtable'] and s['section_index'] != 0]
    require(len(tables) == 1 and tables[0]['section_index'] in [v['index'] for v in snap['sections'].values()], 'candidate table ambiguous')
    table = tables[0]
    section_name = next(k for k,v in snap['sections'].items() if v['index'] == table['section_index'])
    parsed = vtable_section(snap, section_name, paths['owner'])
    owner_relocs = sorted([r for r in parsed['relocations'] if table['offset'] <= r['offset'] < table['offset']+table['size']], key=lambda r:r['offset'])
    nss = inspect_elf(paths['nss'])
    orig_tables = [s for s in nss['symbols'] if s['name'] == request['vtable'] and s['defined'] and s['type'] == 1]
    orig_tables = unique_rows(orig_tables, ('name','address','size','binding','visibility','section_index'))
    require(len(orig_tables) == 1 and orig_tables[0]['size'] == table['size'], 'original table ambiguous or extent differs')
    address, size = orig_tables[0]['address'], table['size']
    main_image = RelocationImage(paths['original'])
    old, kind = main_image.object_bytes(address, size)
    require(kind == 'file' and old[:16] == bytes(16), 'unsupported original ABI prefix')
    main = Elf(paths['original']); debug = Elf(paths['nss'])
    require(main.read_vaddr(address, size) == debug.read_vaddr(address, size), 'NSS vtable bytes differ')
    old_relocs = main_image.object_relocations(address, size)
    require(len(old_relocs) == len(owner_relocs), 'original relocation coverage differs')
    for i, (left, right) in enumerate(zip(old_relocs, owner_relocs)):
        require(left['offset'] == address+16+8*i and left['type'] == 1027 and left['symbol_index'] == 0 and right['offset'] == table['offset']+16+8*i, 'unsupported original slot expression')
        right['original_target'] = left['addend']
    for r in owner_relocs:
        aliases = [s for s in nss['symbols'] if s['address'] == r['original_target'] and s['type'] == 2 and s['defined'] and s['size'] > 0]
        extents = {s['size'] for s in aliases}
        if len(extents) == 1:
            extent = next(iter(extents))
            require(all(nss['sections'][s['section_index']]['flags'] & 4 for s in aliases), 'original target is not executable')
            require(main.read_vaddr(r['original_target'], extent) == debug.read_vaddr(r['original_target'], extent), 'original target bytes differ')
    baseline_data = inspect_elf(paths['baseline'])
    baseline = [dict(s, executable=(s['section_index'] < len(baseline_data['sections']) and bool(baseline_data['sections'][s['section_index']]['flags'] & 4))) for s in baseline_data['symbols']]
    section_by_index = {v['index']:(k,v) for k,v in snap['sections'].items()}
    candidates = [dict(s, section=section_by_index[s['section_index']][0], executable=bool(section_by_index[s['section_index']][1]['flags'] & 4)) for s in snap['symbols'] if s['section_index'] != 0 and s['section_index'] in section_by_index and s['name']]
    with open(paths['functions'], newline='', encoding='utf-8') as stream:
        inv = [{'name':r['Name'], 'address':int(r['Address'],16)-0x7100000000, 'size':int(r['Size']), 'quality':r['Quality']} for r in csv.DictReader(stream)]
    rows = [slot_report(i, r, nss['symbols'], baseline, candidates, inv) for i,r in enumerate(owner_relocs)]
    after = {k: pin(v) for k,v in inputs.items()}
    require(before == after, 'input changed during analysis')
    return {'kind':'vtable-dependency-preflight-v1', 'inputs':inputs, 'original_vtable':orig_tables[0], 'slots':rows,
            'status':'diagnostic_only', 'summary':dict(sorted(Counter(reason for row in rows for reason in row['reasons']).items())),
            'accepted':False, 'exact_complete':False, 'semantic_equivalence':False, 'provider_selection_proven':False,
            'scope':'diagnostic only; linked provider and function bodies require independent proof'}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('request', help='JSON request with explicit input SHA-256 pins')
    args = parser.parse_args()
    with open(args.request, encoding='utf-8') as f:
        request = json.load(f)
    try:
        result = run(request)
    except (ValueError, Halt, InputError, OSError, KeyError, IndexError) as exc:
        result = {'kind':'vtable-dependency-preflight-v1', 'status':'unknown', 'reason':str(exc),
                  'accepted':False, 'exact_complete':False, 'semantic_equivalence':False, 'provider_selection_proven':False}
        print(json.dumps(result, indent=2, sort_keys=True))
        return 2
    print(json.dumps(result, indent=2, sort_keys=True))
    return 0


if __name__ == '__main__':
    sys.exit(main())
