#!/usr/bin/env python3
"""Reproducible P17 data inventory evidence, never runtime completion proof.

Extents require exact ELF symbol sizes: adjacent addresses are not size evidence.
CSV duplicates survive; section coverage uses interval unions, not symbol sums.
NSS extents require matching original file bytes (NOBITS remains unresolved).
"""
import argparse
from collections import Counter, defaultdict
import csv
import gzip
import hashlib
import io
import json
from pathlib import Path
import sys

from attribution_binary import BinaryImage, LOAD_BIAS

ROOT = Path(__file__).resolve().parent.parent


def sha(blob):
    return hashlib.sha256(blob).hexdigest()


def identities(symbols):
    """Collapse duplicate table entries only when all identity fields agree."""
    result = {}
    for symbol in symbols:
        key = tuple(symbol[k] for k in ('name', 'address', 'size', 'type', 'bind', 'section_index'))
        result[key] = symbol
    return list(result.values())


def index_symbols(image):
    result = defaultdict(list)
    if image:
        for symbol in identities(image.symbols):
            if symbol['name']:
                result[symbol['name']].append(symbol)
    return result


def containing(image, address, size=1):
    return [s for s in image.sections if s['flags'] & 2
            and s['address'] <= address and address + size <= s['address'] + s['size']]


def union_ranges(ranges):
    merged = []
    for start, end in sorted(ranges):
        if merged and start <= merged[-1][1]:
            merged[-1][1] = max(end, merged[-1][1])
        else:
            merged.append([start, end])
    return merged


def category(name, section):
    if section is None:
        return 'unknown'
    if section['flags'] & 0x400:
        return 'tls'
    if section['flags'] & 4:
        return 'unknown'
    if name.startswith(('_ZTV', '_ZTC')):
        return 'vtable'
    return 'mutable' if section['flags'] & 1 else 'const'


def linkage_report(path):
    from data_elf import inspect_elf
    report = inspect_elf(path)
    report.pop('path', None)
    symbols = report.pop('symbols')
    report['symbol_count'] = len(symbols)
    report['imports'] = [dict(symbol, resolution_status='unresolved') for symbol in symbols
                         if symbol['name'] and not symbol['defined'] and symbol['binding'] != 0]
    report['summary'] = dict(import_entries=len(report['imports']),
        init_fini_slots=sum(len(a['entries']) for a in report['arrays']),
        tls_segments=len(report['tls_segments']),
        tls_sections=sum(bool(s['flags'] & 0x400) for s in report['sections']))
    return report


def build_ledger(csv_path, elf_path, nss_path=None, rebuilt_path=None,
                 load_bias=LOAD_BIAS, rebuilt_bias=0):
    csv_blob = Path(csv_path).read_bytes()
    reader = csv.DictReader(io.StringIO(csv_blob.decode('utf-8-sig')))
    if not reader.fieldnames or not {'Address', 'Name'} <= set(reader.fieldnames):
        raise ValueError('data CSV requires Address and Name columns')
    original = BinaryImage(elf_path, 'original', load_bias)
    nss = BinaryImage(nss_path, 'nss', load_bias) if nss_path else None
    rebuilt = BinaryImage(rebuilt_path, 'rebuilt', rebuilt_bias) if rebuilt_path else None
    indexes = [index_symbols(original), index_symbols(nss)]
    rebuilt_index = index_symbols(rebuilt)
    rows, ranges = [], defaultdict(list)
    for row_id, source in enumerate(reader, 1):
        try:
            address = int(source['Address'], 16)
            if address < 0 or not source['Name']:
                raise ValueError()
        except (ValueError, TypeError):
            raise ValueError(f'invalid data CSV row {row_id}') from None
        name = source['Name']
        sections = containing(original, address)
        section = sections[0] if len(sections) == 1 else None
        row = dict(row_id=row_id, source=source, name=name, address=address,
                   category=category(name, section), section=section['name'] if section else None,
                   owner=None, owner_status='unresolved', size=None, size_status='missing_symbol',
                   size_evidence=[], comparison=dict(status='not_requested', raw_equal=None,
                                                     target_verified=False))
        candidates = []
        for image, index in zip((original, nss), indexes):
            if image:
                for symbol in index.get(name, []):
                    if symbol['address'] == address and symbol['type'] in (0, 1, 6):
                        candidates.append((image, symbol))
                        row['size_evidence'].append(dict(image=image.label, size=symbol['size'],
                            type=symbol['type'], bind=symbol['bind'], section=symbol['section']))
        if any(symbol['type'] == 6 for _, symbol in candidates):
            row['category'] = 'tls'
            row['section'] = None
            row['address_basis'] = 'TLS offset; CSV address is not a virtual-address proof'
        sizes = {symbol['size'] for _, symbol in candidates}
        valid = []
        if len(sizes) > 1:
            row['size_status'] = 'conflicting_sizes'
        elif sizes == {0}:
            row['size_status'] = 'zero_size'
        elif sizes:
            size = next(iter(sizes))
            if len(containing(original, address, size)) != 1:
                row['size_status'] = 'extent_outside_unique_section'
            else:
                for image, symbol in candidates:
                    # ELF section index is part of the evidence; TLS values are offsets,
                    # not ordinary virtual addresses, and are never range-guessed here.
                    symbol_sections = containing(image, address, size)
                    if (len(symbol_sections) != 1 or symbol['section_index'] != symbol_sections[0]['index']
                            or symbol['type'] == 6):
                        continue
                    if image is original:
                        valid.append((image, symbol))
                    else:
                        a = original.function_bytes(address, size)
                        b = image.function_bytes(address, size)
                        if a is not None and b is not None and a == b:
                            valid.append((image, symbol))
                if valid:
                    row['size'], row['size_status'] = size, 'exact_symbol'
                    ranges[section['index']].append((address, address + size))
                else:
                    row['size_status'] = 'uncorroborated_symbol_extent'
        if row['size'] is not None:
            raw = original.function_bytes(address, row['size'])
            row['original_sha256'] = sha(raw) if raw is not None else None
            row['storage'] = 'nobits' if section['type'] == 8 else 'file_bytes'
        if rebuilt:
            compare = row['comparison']
            compare['status'] = 'original_extent_unresolved'
            if row['size'] is not None:
                # A name reused at multiple original addresses cannot identify the rebuilt object.
                original_ids = {(s['address'], s['type'], s['bind'])
                                for index in indexes for s in index.get(name, [])}
                matches = rebuilt_index.get(name, [])
                if len(original_ids) != 1:
                    compare['status'] = 'ambiguous_original_identity'
                elif not matches:
                    compare['status'] = 'missing_rebuilt_symbol'
                elif len(matches) != 1:
                    compare['status'] = 'ambiguous_rebuilt_identity'
                else:
                    match = matches[0]
                    compare['rebuilt_address'] = match['address']
                    rebuilt_sections = containing(rebuilt, match['address'], match['size'])
                    if (len(rebuilt_sections) != 1
                            or match['section_index'] != rebuilt_sections[0]['index']):
                        compare['status'] = 'invalid_rebuilt_section_extent'
                    elif (match['type'], match['bind']) != next(iter(original_ids))[1:]:
                        compare['status'] = 'incompatible_symbol_identity'
                    elif match['size'] != row['size']:
                        compare['status'] = 'size_mismatch'
                    else:
                        a = original.function_bytes(address, row['size'])
                        b = rebuilt.function_bytes(match['address'], match['size'])
                        if a is None or b is None:
                            compare['status'] = 'file_bytes_unavailable'
                        else:
                            compare['rebuilt_sha256'] = sha(b)
                            compare['raw_equal'] = a == b
                            compare['status'] = 'raw_equal_targets_unverified' if a == b else 'raw_different'
        rows.append(row)
    section_rows = []
    for section in original.sections:
        if not section['flags'] & 2 or section['flags'] & 4:
            continue
        merged = union_ranges(ranges[section['index']])
        cursor, gaps = section['address'], []
        for start, end in merged:
            if start > cursor:
                gaps.append(dict(address=cursor, size=start - cursor))
            cursor = end
        end = section['address'] + section['size']
        if cursor < end:
            gaps.append(dict(address=cursor, size=end - cursor))
        section_rows.append(dict(index=section['index'], name=section['name'], address=section['address'],
            size=section['size'], flags=section['flags'], type=section['type'],
            covered_bytes=sum(end - start for start, end in merged),
            gaps=gaps, gap_bytes=sum(g['size'] for g in gaps),
            coverage_meaning='CSV exact-sized symbol extents; not verified reconstruction'))
    inputs = {'data_symbols': {'sha256': sha(csv_blob)}, 'original': original.metadata()}
    for key, image in [('nss', nss), ('rebuilt', rebuilt)]:
        inputs[key] = image.metadata() if image else None
    linkage = {'original': linkage_report(elf_path)}
    if nss:
        linkage['nss'] = linkage_report(nss_path)
    if rebuilt:
        linkage['rebuilt'] = linkage_report(rebuilt_path)
    return dict(schema_version=1, inputs=inputs, rows=rows, sections=section_rows, linkage=linkage,
        provenance={'rebuilt_source_revision': None, 'rebuilt_build_recipe': None,
                    'status': 'artifact_hash_only'},
        summary=dict(row_count=len(rows), unique_address_name_count=len({(r['address'], r['name']) for r in rows}),
            categories=dict(sorted(Counter(r['category'] for r in rows).items())),
            size_status=dict(sorted(Counter(r['size_status'] for r in rows).items())),
            comparison_status=dict(sorted(Counter(r['comparison']['status'] for r in rows).items())),
            owned_count=0, target_verified_count=0, runtime_verified_count=0,
            covered_bytes=sum(s['covered_bytes'] for s in section_rows),
            gap_bytes=sum(s['gap_bytes'] for s in section_rows)),
        limitations=['Raw byte equality does not verify relocation targets, linkage or runtime behavior.',
            'No source owners inferred from symbol names; all ownership remains unresolved.',
            'TLS symbols use TLS offsets; no guessed address conversion or runtime TLS proof.',
            'Section gaps include padding and linker metadata; they are inventory gaps, not proven missing source.',
            'NSS sizes require same-name same-address symbols and identical file bytes; NOBITS cannot corroborate NSS.',
            'Import binding and init/fini runtime order remain unverified; ELF tables are static evidence.'],
        followups=[{'id': 'P17-F01', 'scope': 'Resolve source ownership for exact-sized data and ambiguous extents.'},
            {'id': 'P17-F02', 'scope': 'Prove rebuilt data relocation targets, alias identities and zero-fill storage.'},
            {'id': 'P17-F03', 'scope': 'Assign import providers and validate initialization/finalization ordering.'},
            {'id': 'P17-F04', 'scope': 'Recover TLS layout/initialization and account for section inventory gaps.'}])


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--csv', '--data-symbols', dest='csv_path', type=Path,
                        default=ROOT / 'data/2.6.2/data_symbols.csv')
    parser.add_argument('--elf', type=Path, default=ROOT / 'data/2.6.2/main.elf')
    parser.add_argument('--nss', type=Path)
    parser.add_argument('--rebuilt', '--built-elf', dest='rebuilt', type=Path)
    parser.add_argument('--load-bias', type=lambda s: int(s, 0), default=LOAD_BIAS)
    parser.add_argument('--rebuilt-bias', type=lambda s: int(s, 0), default=0)
    parser.add_argument('--output', type=Path, help='JSON or deterministic .json.gz; default stdout')
    args = parser.parse_args(argv)
    try:
        if args.output:
            for source_path in (args.csv_path, args.elf, args.nss, args.rebuilt):
                if source_path and (args.output.resolve() == source_path.resolve()
                                    or (args.output.exists() and args.output.samefile(source_path))):
                    raise ValueError('output must not overwrite an audit input')
        ledger = build_ledger(args.csv_path, args.elf, args.nss, args.rebuilt, args.load_bias, args.rebuilt_bias)
        payload = (json.dumps(ledger, sort_keys=True, indent=2) + '\n').encode()
        if args.output:
            args.output.parent.mkdir(parents=True, exist_ok=True)
            args.output.write_bytes(gzip.compress(payload, mtime=0) if args.output.suffix == '.gz' else payload)
            print(json.dumps(ledger['summary'], sort_keys=True))
        else:
            sys.stdout.write(payload.decode())
    except (OSError, ValueError, UnicodeError) as exc:
        parser.error(str(exc))
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
