"""Narrow recovery witnesses for newly emitted, fully proved vtable storage.

This policy does not infer anonymous constants, pointers, or missing identities.
The ordinary data validator remains authoritative. All raw records are retained.
"""
import hashlib
import json
import re
import struct
from pathlib import Path
from validation import common as c
from validation.elf import Elf
from data_provenance import verify_link_provenance
from data_relocations import RelocationImage


def require(ok, message):
    if not ok:
        raise c.Halt('Recovery data: ' + message)


def vtable_section(snapshot, section, path):
    sec = snapshot['sections'][section]
    require(section == '.data.rel.ro' and sec['type'] == 1 and sec['flags'] == 3
            and sec['alignment'] == 8 and sec['entry_size'] == 0 and sec['size'] > 0,
            'unsupported vtable section metadata')
    markers = [m for m in snapshot['markers'] if m['section'] == section]
    require(len(markers) == 1 and (markers[0]['kind'], markers[0]['offset']) == ('d', 0),
            'vtable mapping must be one data marker at zero')
    section_symbols = [s for s in snapshot['symbols'] if s['section_index'] == sec['index'] and s['type'] == 3]
    require(all(s['name'] in ('', section) and s['binding'] == 0 and s['other'] == 0
                and s['offset'] == 0 and s['size'] == 0 for s in section_symbols),
            'malformed vtable section symbol')
    symbols = [s for s in snapshot['symbols'] if s['section_index'] == sec['index']
               and s['type'] != 3 and s['symbol_index'] not in {m['symbol_index'] for m in markers}]
    require(bool(symbols), 'vtable section has no named objects')
    end = 0
    for s in sorted(symbols, key=lambda s: s['offset']):
        require(s['name'].startswith('_ZTV') and s['type'] == 1 and s['binding'] in (1, 2)
                and s['other'] == 0 and s['offset'] == end and s['size'] >= 24
                and s['size'] % 8 == 0, 'ambiguous vtable identity/alias/extent')
        end += s['size']
    require(end == sec['size'], 'vtable symbols do not cover exact section extent')
    require(len({s['name'] for s in symbols}) == len(symbols), 'duplicate vtable names')
    e = Elf(path)
    raw = e.sections[sec['index']]
    payload = e.d[raw[4]:raw[4]+raw[5]]
    require(hashlib.sha256(payload).hexdigest() == sec['sha256'], 'object changed during witness')
    relocs = [r for r in snapshot['relocations'] if r['section'] == section]
    slots = {s['offset']+off for s in symbols for off in range(16, s['size'], 8)}
    require(len(relocs) == len(slots) and {r['offset'] for r in relocs} == slots,
            'vtable relocation layout incomplete or overlapping')
    for r in relocs:
        s = r['symbol']
        require(r['type'] == 257 and r['addend'] == 0 and s['name'] and s['type'] in (0, 2)
                and s['binding'] in (1, 2) and s['other'] == 0,
                'unsupported vtable pointer expression')
    # This bounded ABI case has offset-to-top=0, no RTTI and zero RELA storage.
    require(not any(payload), 'unsupported nonzero vtable object storage')
    consumers = [r for r in snapshot['relocations'] if r['symbol']['section_index'] == sec['index']
                 or r['symbol']['name'] in {s['name'] for s in symbols}]
    require(bool(consumers), 'vtable has no owner consumer evidence')
    by_section = {}
    for r in consumers:
        by_section.setdefault(r['section'], []).append(r)
    for name, records in by_section.items():
        source = snapshot['sections'][name]
        require(source['type'] == 1 and source['flags'] & 4, 'non-code vtable consumer')
        raw_section = e.sections[source['index']]
        records.sort(key=lambda r:r['offset'])
        require(len(records) % 2 == 0, 'incomplete vtable consumer pair')
        for a, b in zip(records[::2], records[1::2]):
            require(a['symbol'] in symbols and b['symbol'] == a['symbol']
                    and ((a['type'],b['type'],a['addend'],b['addend']) in ((311,312,0,0),(275,277,16,16)))
                    and a['offset'] % 4 == 0 and b['offset'] == a['offset']+4
                    and b['offset']+4 <= source['size']
                    and any(f['section'] == name and f['offset'] <= a['offset']
                            and b['offset']+4 <= f['offset']+f['size'] for f in snapshot['functions']),
                    'unsupported vtable consumer identity/extent/pair')
            first, second = struct.unpack_from('<II', e.d, raw_section[4]+a['offset'])
            require(first & 0x9f000000 == 0x90000000
                    and second & 0xffc00000 == (0xf9400000 if b['type'] == 312 else 0x91000000)
                    and first & 31 == (second >> 5) & 31 == second & 31,
                    'vtable consumer opcode/register flow differs')
    return dict(section=section, metadata=sec, symbols=symbols, relocations=relocs,
                consumers=consumers, classification='requires-original-linked-data-proof')


def prove(mapping, ledger, link, owner, object_path):
    """Bind each admitted object extent to genuine map provenance and linked proof."""
    sections = mapping.get('new_data_sections', [])
    if not sections:
        return []
    provenance = verify_link_provenance(link)
    require(provenance == ledger['provenance'], 'ledger/link provenance mismatch')
    image = RelocationImage(link, 'rebuilt')
    require(image.metadata['sha256'] == provenance['elf_sha256'], 'linked snapshot changed')
    lines = Path(str(link)+'.map').read_text().splitlines()
    rows = ledger['rows']
    witnesses = []
    for section in sections:
        object_path = Path(object_path)
        require(object_path.is_file(), 'retained owner object missing')
        parts = object_path.parts
        require(parts.count('objects') == 1, 'ambiguous retained object root')
        relative = Path(*parts[parts.index('objects')+1:]).as_posix()
        require(relative.endswith('/src/'+owner+'.obj'), 'object owner path mismatch')
        contribution = '/work/build/2.6.2/'+relative+':('+section['section']+')'
        suffix = '/src/'+owner+'.obj:('+section['section']+')'
        matches = []
        for line in lines:
            if line.rstrip().endswith(suffix):
                fields = line.split()
                require(len(fields) == 4 and re.fullmatch(r'[0-9a-fA-F]+', fields[0])
                        and re.fullmatch(r'[0-9a-fA-F]+', fields[1]) and fields[2].isdigit(),
                        'malformed owner map contribution')
                require(fields[3] == contribution, 'wrong owner object map provenance')
                matches.append((int(fields[0],16), int(fields[1],16), int(fields[2]), line))
        require(len(matches) == 1, 'missing/ambiguous owner map contribution')
        start, size, align, raw_line = matches[0]
        require(size == section['metadata']['size'] and align == section['metadata']['alignment'],
                'owner map extent differs from object')
        for s in section['symbols']:
            found = [row for row in rows if row['name'] == s['name']]
            require(len(found) == 1, 'missing/ambiguous original vtable identity: '+s['name'])
            row = found[0]
            proof = row['validation']
            require(row['size'] == s['size'] and row['size_status'] == 'exact_symbol'
                    and row['comparison'].get('rebuilt_address') == start+s['offset'],
                    'original/linked/object vtable extent mismatch')
            require(proof['status'] == 'verified_equal' and proof['reasons'] == []
                    and proof['storage_verified'] and proof['non_relocated_equal'],
                    'unresolved original vtable proof: '+s['name']+' '+str(proof.get('reasons')))
            linked = [x for x in image.symbols if x['name'] == s['name']]
            require(len({(x['address'],x['size'],x['section_index'],x['type'],x['binding'],x['visibility']) for x in linked}) == 1 and linked[0]['address'] == start+s['offset']
                    and linked[0]['size'] == s['size'] and linked[0]['type'] == s['type']
                    and linked[0]['binding'] == s['binding'] and linked[0]['visibility'] == s['other'],
                    'linked vtable symbol metadata mismatch')
            storage_section = image.sections[linked[0]['section_index']]
            require(storage_section['type'] == 1 and storage_section['flags'] == 3
                    and storage_section['alignment'] >= 8 and linked[0]['address'] % 8 == 0,
                    'unsupported linked vtable section metadata')
            identity = image.target_identity(start+s['offset'])
            require(identity['resolved'] and identity['offset'] == 0
                    and identity['aliases'] == [[s['name'],1,s['binding'],s['other']]],
                    'unreviewed linked vtable aliases')
            body, storage = image.object_bytes(start+s['offset'], s['size'])
            require(storage == 'file' and body[:16] == bytes(16), 'vtable ABI header differs')
            object_slots = [r for r in section['relocations'] if s['offset'] <= r['offset'] < s['offset']+s['size']]
            linked_slots = image.object_relocations(start+s['offset'], s['size'])
            require(len(linked_slots) == len(object_slots), 'linked/object relocation count differs')
            for a, b in zip(sorted(object_slots,key=lambda r:r['offset']), linked_slots):
                require(b['offset'] == start+a['offset'], 'linked/object relocation offset differs')
                expression = image.expression(b)
                require(expression['resolved'] and expression['type'] in (257, 1027)
                        and expression['target']['offset'] == 0
                        and any(alias[0] == a['symbol']['name'] and alias[1] == 2 for alias in expression['target']['aliases'])
                        and (expression['type'] == 1027 or expression['addend'] == 0),
                        'linked/object pointer target differs or unresolved')
                targets = [x for x in image.symbols if x['name'] == a['symbol']['name']]
                require(targets and all(image.sections[x['section_index']]['type'] == 1
                        and image.sections[x['section_index']]['flags'] & 4 for x in targets),
                        'vtable target is not file-backed executable function')
            witnesses.append(dict(identity=[row['address'],row['name']], symbol=s,
                                  original_data_record=row, owner_map_line=raw_line, object_contribution=contribution,
                                  provenance=provenance, object_section=section,
                                  linked_relocations=linked_slots))
    return witnesses


def compare_outcomes(before, after, witnesses):
    allowed = {tuple(w['identity']) for w in witnesses}
    require(before.keys() == after.keys(), 'data identity set changed')
    transitions = []
    for identity, value in before.items():
        new = after[identity]
        if (value['status'],value.get('reasons')) == (new['status'],new.get('reasons')):
            continue
        require(identity in allowed and value == {'status':'unverified','reasons':['missing_rebuilt_symbol']}
                and new['status'] == 'verified_equal' and new['reasons'] == [],
                'data proof status/reason changed without exact new-vtable witness: '+str(identity))
        transitions.append(dict(identity=list(identity), baseline=value, candidate=new))
    return transitions


def read_records(records):
    require(isinstance(records, list) and len(records) <= 65536, 'malformed or excessive retained data records')
    result = {}
    for entry in records:
        require(isinstance(entry, list) and len(entry) == 2, 'malformed retained data entry')
        key, value = entry
        require(isinstance(key, list) and len(key) == 2 and type(key[0]) is int
                and key[0] >= 0 and isinstance(key[1], str) and bool(key[1])
                and isinstance(value, dict), 'malformed retained data identity')
        require(value.get('status') in ('unverified', 'different', 'verified_equal')
                and isinstance(value.get('reasons'), list)
                and all(isinstance(reason, str) for reason in value['reasons']),
                'malformed retained data proof')
        require(tuple(key) not in result, 'duplicate retained data identity')
        result[tuple(key)] = value
    return result


# Data ledgers contain every original identity and relocation proof, and exceed
# the generic 8 MiB control-metadata bound. This bound is dedicated to that data
# format; other receipt metadata continues through receipts.read_json unchanged.
MAX_RETAINED_DATA_BYTES = 32 * 1024 * 1024


def read_retained_data(path):
    from receipts import regular
    path = regular(path)
    require(path.stat().st_size <= MAX_RETAINED_DATA_BYTES, 'retained data exceeds bounded reader limit')
    with path.open('rb') as stream:
        blob = stream.read(MAX_RETAINED_DATA_BYTES + 1)
    require(len(blob) <= MAX_RETAINED_DATA_BYTES, 'retained data exceeds bounded reader limit')
    def unique(pairs):
        result = {}
        for key, value in pairs:
            require(key not in result, 'duplicate retained data field')
            result[key] = value
        return result
    def invalid_constant(value):
        raise c.Halt('Recovery data: non-JSON numeric constant '+value)
    try:
        records = json.loads(blob.decode('utf-8'), object_pairs_hook=unique, parse_constant=invalid_constant)
    except (UnicodeError, ValueError, RecursionError) as exc:
        raise c.Halt('Recovery data: malformed retained data JSON') from exc
    return read_records(records)
