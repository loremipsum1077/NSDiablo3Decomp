#!/usr/bin/env python3
"""Validate P17 data storage and relocation identities with bound ELF/map evidence.

This supplements the inventory; it never certifies source ownership, runtime
behavior, or absence of pointers that lack ELF relocation records.
"""
import argparse
from collections import Counter
import gzip
import hashlib
import json
from pathlib import Path
import sys

from attribution_binary import LOAD_BIAS
from data_coverage import ROOT, build_ledger
from data_provenance import verify_link_provenance
from data_relocations import RelocationImage, compare_objects


def validate_data(csv_path, elf_path, rebuilt_path, nss_path=None,
                  load_bias=LOAD_BIAS, rebuilt_bias=0, map_path=None,
                  provenance_path=None):
    provenance = verify_link_provenance(rebuilt_path, map_path, provenance_path)
    ledger = build_ledger(csv_path, elf_path, nss_path, rebuilt_path,
                          load_bias, rebuilt_bias)
    original = RelocationImage(elf_path, 'original', load_bias)
    rebuilt = RelocationImage(rebuilt_path, 'rebuilt', rebuilt_bias)
    images = {'original': original, 'rebuilt': rebuilt}
    if nss_path:
        nss = RelocationImage(nss_path, 'nss', load_bias)
        images['nss'] = nss
        original.add_symbol_evidence(nss)
    for key, image in images.items():
        expected = ledger['inputs'][key]['sha256']
        if image.metadata['sha256'] != expected or ledger['linkage'][key]['sha256'] != expected:
            raise ValueError(f'{key} snapshots changed during validation')
    eligible = {'raw_equal_targets_unverified', 'raw_different', 'file_bytes_unavailable'}
    for row in ledger['rows']:
        comparison = row['comparison']
        row['validation'] = dict(status='unverified', reasons=[comparison['status']])
        if any(item['bind'] == 0 for item in row['size_evidence']):
            row['validation']['reasons'] = ['local_object_without_owner_identity']
            continue
        if comparison['status'] in eligible and row['size'] is not None:
            row['validation'] = compare_objects(original, rebuilt, row['address'],
                                                 comparison['rebuilt_address'], row['size'])
    # Detect mutation during the audit instead of associating results with stale hashes.
    if verify_link_provenance(rebuilt_path, map_path, provenance_path) != provenance:
        raise ValueError('rebuilt artifacts changed during validation')
    if ledger['inputs']['rebuilt']['sha256'] != provenance['elf_sha256']:
        raise ValueError('ledger and provenance refer to different rebuilt artifacts')
    for key, path in [('original', elf_path), ('nss', nss_path), ('data_symbols', csv_path)]:
        if path and hashlib.sha256(Path(path).read_bytes()).hexdigest() != ledger['inputs'][key]['sha256']:
            raise ValueError(f'{key} changed during validation')
    ledger['schema_version'] = 2
    ledger['provenance'] = provenance
    ledger['summary']['validation_status'] = dict(sorted(Counter(
        row['validation']['status'] for row in ledger['rows']).items()))
    verified = [row for row in ledger['rows'] if row['validation']['status'] == 'verified_equal']
    ledger['summary']['validated_relocated_objects'] = sum(bool(row['validation']['relocations']) for row in verified)
    ledger['summary']['validated_relocation_slots'] = sum(len(row['validation']['relocations']) for row in verified)
    ledger['summary']['validation_reasons'] = dict(sorted(Counter(
        reason for row in ledger['rows'] for reason in set(row['validation']['reasons'])).items()))
    ledger['limitations'].extend([
        'Validation covers ELF-recorded relocations only; unrecorded absolute pointers are not inferred.',
        'Verified storage/relocations do not certify source ownership or function body equivalence.',
        'Link provenance binds ELF and map hashes; fresh compilation requires separate build evidence.',
    ])
    return ledger


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--csv', type=Path, default=ROOT / 'data/2.6.2/data_symbols.csv')
    parser.add_argument('--elf', type=Path, default=ROOT / 'data/2.6.2/main.elf')
    parser.add_argument('--nss', type=Path)
    parser.add_argument('--rebuilt', type=Path, required=True)
    parser.add_argument('--map', dest='map_path', type=Path)
    parser.add_argument('--provenance', type=Path)
    parser.add_argument('--load-bias', type=lambda s: int(s, 0), default=LOAD_BIAS)
    parser.add_argument('--rebuilt-bias', type=lambda s: int(s, 0), default=0)
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args(argv)
    try:
        paths = [args.csv, args.elf, args.nss, args.rebuilt,
                 args.map_path or Path(str(args.rebuilt) + '.map'),
                 args.provenance or Path(str(args.map_path or Path(str(args.rebuilt) + '.map')) + '.provenance')]
        for path in paths:
            if path and (args.output.resolve() == path.resolve()
                         or (args.output.exists() and args.output.samefile(path))):
                raise ValueError('output must not overwrite a validation input')
        ledger = validate_data(args.csv, args.elf, args.rebuilt, args.nss,
                               args.load_bias, args.rebuilt_bias, args.map_path, args.provenance)
        payload = (json.dumps(ledger, sort_keys=True, indent=2) + '\n').encode()
        args.output.parent.mkdir(parents=True, exist_ok=True)
        args.output.write_bytes(gzip.compress(payload, mtime=0) if args.output.suffix == '.gz' else payload)
        print(json.dumps(ledger['summary'], sort_keys=True))
    except (OSError, ValueError, UnicodeError) as exc:
        parser.error(str(exc))
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
