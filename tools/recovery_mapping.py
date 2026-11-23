"""Authenticate local AArch64 mapping markers; never erase real symbol identities.

Only assembler ordinal suffixes are normalized. Existing section mapping sequences
must be identical. New executable sections must be complete named functions; extras require review.
"""
import re
import hashlib
import struct
from collections import Counter
from pathlib import Path
from validation import common as c
from validation.elf import Elf

MARKER = re.compile(r'^\$(x|d)(?:\.[0-9]+)?$')
LOOKING = re.compile(r'^\$[xd](?:\.|$)')


def inspect(path):
    if path is None:
        return {'markers': [], 'real_emissions': [], 'sections': {}, 'functions': [], 'groups': [], 'relocations': [], 'symbols': []}
    try:
        e = Elf(path)
        if e.d[:7] != b'\x7fELF\x02\x01\x01' or struct.unpack_from('<HH', e.d, 16) != (1, 183):
            raise c.Halt('Mapping witness requires ELF64 little-endian AArch64 ET_REL')
        header_size, _, _, section_size, section_count, _ = struct.unpack_from('<HHHHHH', e.d, 0x34)
        section_offset = struct.unpack_from('<Q', e.d, 0x28)[0]
        if (header_size != 64 or section_size != 64 or section_offset < 64
                or section_offset + section_size * section_count > len(e.d)):
            raise c.Halt('Invalid ELF section table extent')
        if any(s[1] != 8 and s[4] + s[5] > len(e.d) for s in e.sections):
            raise c.Halt('Section file extent exceeds retained ELF')
        shstr = e.sections[struct.unpack_from('<H', e.d, 0x3e)[0]]
        def string(table, offset):
            if offset >= table[5]: raise c.Halt('Mapping witness invalid string offset')
            at = table[4] + offset
            end = e.d.index(b'\0', at, table[4]+table[5])
            return e.d[at:end].decode()
        sections = {}
        names = {}
        for i, s in enumerate(e.sections):
            name = string(shstr, s[0])
            names[i] = name
            if s[1] == 17:  # SHT_GROUP names need not be unique; authenticate below.
                if name != '.group': raise c.Halt('Unsupported group section name')
                continue
            if name == '.group': raise c.Halt('Unauthenticated group section name')
            if name in sections: raise c.Halt('Mapping witness ambiguous section name')
            payload = e.d[s[4]:s[4]+s[5]] if s[1] != 8 else b''
            sections[name] = {'index': i, 'type': s[1], 'flags': s[2], 'size': s[5],
                              'sha256': hashlib.sha256(payload).hexdigest(), 'alignment': s[8], 'entry_size': s[9]}
        tables = [(i,s) for i,s in enumerate(e.sections) if s[1] == 2]
        if len(tables) != 1: raise c.Halt('Mapping witness requires unique symbol table')
        ti, table = tables[0]
        if table[9] != 24 or table[5] % 24: raise c.Halt('Mapping witness invalid symbol table')
        markers, real, functions = [], [], []
        marker_ids = set()
        raw_symbols = []
        for i, at in enumerate(range(table[4], table[4]+table[5], 24)):
            n, info, other, idx, value, size = struct.unpack_from('<IBBHQQ', e.d, at)
            name = string(e.sections[table[6]], n)
            raw_symbols.append({'name': name, 'binding': info >> 4, 'type': info & 15,
                                'other': other, 'section_index': idx, 'offset': value,
                                'size': size, 'symbol_index': i})
            match = MARKER.fullmatch(name)
            if LOOKING.match(name) and not match:
                raise c.Halt('Unsupported mapping-symbol spelling: '+name)
            if match:
                if info != 0 or other != 0 or size != 0 or not 0 < idx < len(e.sections) or idx >= 0xff00:
                    raise c.Halt('Invalid mapping-symbol metadata: '+name)
                sec = e.sections[idx]
                if sec[1] == 17 or value >= sec[5] or (match[1] == 'x' and (not sec[2] & 4 or value % 4)):
                    raise c.Halt('Invalid mapping-symbol position/kind: '+name)
                markers.append({'name': name, 'symbol_index': i, 'section': names[idx],
                                'kind': match[1], 'offset': value, 'size': size,
                                'binding': info >> 4, 'type': info & 15, 'other': other})
                marker_ids.add(i)
            elif idx and name and info & 15 not in (3,4):
                real.append({'name': name, 'type': info & 15})
            if info & 15 == 2 and 0 < idx < len(e.sections):
                functions.append(dict(raw_symbols[-1], section=names[idx]))
        if len({f['name'] for f in functions}) != len(functions):
            raise c.Halt('Ambiguous function name in mapping witness')
        positions = [(v['section'],v['offset']) for v in markers]
        if len(set(positions)) != len(positions):
            raise c.Halt('Duplicate/conflicting mapping markers')
        groups = []
        memberships = {}
        signatures = set()
        for group_index, s in enumerate(e.sections):
            if s[1] != 17: continue
            if s[2] != 0 or s[6] != ti or s[9] != 4 or s[5] < 8 or s[5] % 4:
                raise c.Halt('Invalid COMDAT group metadata')
            if not 0 < s[7] < len(raw_symbols): raise c.Halt('Invalid COMDAT signature index')
            signature = raw_symbols[s[7]]
            if (not signature['name'] or signature['name'] in signatures
                    or signature['type'] != 2 or signature['binding'] != 2 or signature['other'] != 0):
                raise c.Halt('Unsupported or duplicate COMDAT signature')
            payload = struct.unpack_from('<'+'I'*(s[5]//4), e.d, s[4])
            if payload[0] != 1: raise c.Halt('Unsupported COMDAT group flags')
            members = payload[1:]
            if len(set(members)) != len(members): raise c.Halt('Duplicate COMDAT member')
            for member in members:
                if (not 0 < member < len(e.sections) or e.sections[member][1] == 17
                        or not e.sections[member][2] & 0x200 or member in memberships):
                    raise c.Halt('Invalid/conflicting COMDAT membership')
                memberships[member] = signature['name']
            if signature['section_index'] not in members:
                raise c.Halt('COMDAT signature is not defined in its group')
            defining = e.sections[signature['section_index']]
            if (defining[1] != 1 or not defining[2] & 4 or not signature['size']
                    or signature['offset'] + signature['size'] > defining[5]):
                raise c.Halt('COMDAT signature lacks valid function extent')
            for member in members:
                sec = e.sections[member]
                if sec[1] in (4,9) and (sec[6] != ti or sec[7] not in members):
                    raise c.Halt('COMDAT relocation targets another group')
            signatures.add(signature['name'])
            groups.append({'index': group_index, 'name': names[group_index],
                           'signature': signature, 'flags': payload[0],
                           'members': [{'index': i, 'name': names[i]} for i in members]})
        if any(s[2] & 0x200 and i not in memberships for i,s in enumerate(e.sections)):
            raise c.Halt('Unowned SHF_GROUP section')
        relocations = []
        for index, s in enumerate(e.sections):
            if s[1] not in (4,9): continue
            width = 24 if s[1] == 4 else 16
            if s[6] != ti or s[9] != width or s[5] % width or not 0 < s[7] < len(e.sections):
                raise c.Halt('Unsupported relocation table in mapping witness')
            for at in range(s[4],s[4]+s[5],width):
                info = struct.unpack_from('<Q',e.d,at+8)[0]
                if info >> 32 in marker_ids:
                    raise c.Halt('Relocation references mapping marker')
                if info >> 32 >= len(raw_symbols): raise c.Halt('Invalid relocation symbol index')
                relocations.append({'table_index': index, 'section': names[s[7]],
                                    'offset': struct.unpack_from('<Q', e.d, at)[0],
                                    'type': info & 0xffffffff, 'symbol': raw_symbols[info >> 32],
                                    'addend': struct.unpack_from('<q', e.d, at+16)[0] if width == 24 else None})
        return {'markers': markers, 'real_emissions': real, 'sections': sections, 'functions': functions,
                'groups': groups, 'relocations': relocations, 'symbols': raw_symbols}
    except (IndexError, ValueError, UnicodeError, struct.error) as exc:
        raise c.Halt('Malformed ELF mapping evidence: '+str(exc)) from exc


# A previously absent owner object necessarily starts the per-object metadata
# sections every compiled object has, each with one data marker at offset 0.
# Exact (sh_type, sh_flags); .eh_frame content stays covered by unwind obligations.
NEW_OBJECT_METADATA = {
    '.comment': (1, 0x30), '.debug_str': (1, 0x30), '.debug_abbrev': (1, 0),
    '.debug_info': (1, 0), '.debug_line': (1, 0), '.debug_macinfo': (1, 0),
    '.debug_pubnames': (1, 0), '.debug_pubtypes': (1, 0), '.eh_frame': (1, 2),
}


def compare(before, after, selected, *, scalar_context=None, named_data=False):
    """Return raw witness; fail closed on lost real symbols or changed old mappings."""
    a, b = inspect(before), inspect(after)
    def canonical(snapshot):
        return Counter((x['section'],x['kind'],x['offset']) for x in snapshot['markers'])
    ac, bc = canonical(a), canonical(b)
    if ac - bc: raise c.Halt('Existing mapping semantics changed or disappeared')
    for section, _, _ in ac:
        old, new = a['sections'][section], b['sections'][section]
        if (old['type'], old['flags']) != (new['type'], new['flags']):
            raise c.Halt('Existing mapping section metadata changed')
    additions = bc - ac
    def group_id(g):
        sig = g['signature']
        defining = next(m['name'] for m in g['members'] if m['index'] == sig['section_index'])
        return (sig['name'], sig['type'], sig['binding'], sig['other'], defining, sig['offset'],
                g['flags'], tuple(sorted(m['name'] for m in g['members'])))
    if Counter(map(group_id,a['groups'])) - Counter(map(group_id,b['groups'])):
        raise c.Halt('Existing COMDAT group identity/membership changed')
    new_debug = []
    new_object_metadata = []
    extras = []
    old_functions = {f['name'] for f in a['functions']}
    for name, sec in b['sections'].items():
        prior = a['sections'].get(name)
        if sec['flags'] & 4 and sec['size']:
            if prior is not None and (prior['type'],prior['flags']) != (sec['type'],sec['flags']):
                raise c.Halt('Existing section became or changed executable metadata')
            if (prior is None or prior['size'] == 0) and (name, 'x', 0) not in bc:
                raise c.Halt('New executable extent lacks x-at-zero marker')
    selected_sections = []
    data_sections = []
    named_sections = []
    scalar_proof = None
    for section, kind, offset in additions:
        sec = b['sections'][section]
        prior_section = a['sections'].get(section)
        # The observed new DWARF location list is non-runtime metadata.
        # Do not admit arbitrary .debug-like names, allocation flags or interior markers.
        if (section in ('.debug_loc', '.debug_ranges') and kind == 'd' and offset == 0
                and sec['type'] == 1 and sec['flags'] == 0
                and (prior_section is None or (prior_section['type'], prior_section['flags'], prior_section['size']) == (1, 0, 0))):
            new_debug.append(section)
            continue
        if (before is None and (kind, offset) == ('d', 0)
                and NEW_OBJECT_METADATA.get(section) == (sec['type'], sec['flags'])):
            new_object_metadata.append(section)
            continue
        if prior_section is not None:
            raise c.Halt('New mapping marker in existing section')
        if section == '.data.rel.ro' and kind == 'd' and offset == 0:
            from recovery_data import vtable_section
            data_sections.append(vtable_section(b, section, after))
            continue
        if (named_data and (section == '.data' or section.startswith('.data.'))
                and (kind, offset) == ('d', 0)):
            # Structural eligibility only. Recovery must separately regenerate
            # the original/linked/consumer proof before admitting this storage.
            from recovery_named_data import named_section
            named_sections.append(named_section(b, section, after))
            continue
        if (section == '.rodata.cst4' and (kind, offset) == ('d', 0)
                and before is None and scalar_context is not None):
            # This is raw-input proof regeneration, never a caller-supplied waiver
            # or a hash-repinned witness JSON. Legacy/default callers still halt.
            from recovery_scalar import prove
            keys = {'original', 'nss', 'before_receipt', 'after_receipt',
                    'owner_object', 'selected_rows', 'owner'}
            if not isinstance(scalar_context, dict) or set(scalar_context) != keys:
                raise c.Halt('Incomplete scalar raw-input context')
            if (Path(scalar_context['owner_object']).resolve() != Path(after).resolve()
                    or {x['Name'] for x in scalar_context['selected_rows']} != set(selected)):
                raise c.Halt('Scalar proof context does not identify this complete owner')
            scalar_proof = prove(**scalar_context)
            if (scalar_proof['section'] != section
                    or scalar_proof['owner_object_sha256'] != c.sha(after)
                    or scalar_proof['exact_complete'] is not False):
                raise c.Halt('Scalar proof does not bind the owner data section')
            continue
        if section in ('.rodata.cst4', '.data'):
            raise c.Halt('Data mapping lacks authenticated original identity and consumer proof: '+section)
        fs = [f for f in b['functions'] if f['section'] == section]
        # Complete named bodies may be original stripped/COMDAT extras. Never approve them here.
        if (kind != 'x' or offset != 0 or sec['type'] != 1 or not sec['flags'] & 4 or sec['size'] % 4 or not fs
                or any(not f['name'] or f['offset'] != 0 or not f['size'] or f['size'] != sec['size']
                       or f['binding'] not in (0,1,2) or f['other'] != 0 for f in fs)):
            raise c.Halt('Unsupported new mapping section: '+section)
        if any(f['name'] in selected for f in fs):
            selected_sections.append(section)
    # Data need not carry a mapping marker at all (in particular, SHT_NOBITS
    # often does not). Account for runtime data independently of marker deltas;
    # New scalar storage requires the separately regenerated scalar witness.
    witnessed_data = {x['section'] for x in data_sections}
    witnessed_data.update(x['section'] for x in named_sections)
    if scalar_proof is not None:
        witnessed_data.add(scalar_proof['section'])
    protected_data = set()
    for name in a['sections'].keys() | b['sections'].keys():
        prior, sec = a['sections'].get(name), b['sections'].get(name)
        def runtime_data(value):
            return value is not None and value['flags'] & 2 and not value['flags'] & 4 and value['size'] > 0
        if not runtime_data(prior) and not runtime_data(sec):
            continue
        if name == '.eh_frame' and sec is not None and (sec['type'], sec['flags']) == (1, 2):
            if ((prior is not None and (prior['type'], prior['flags']) == (1, 2))
                    or name in new_object_metadata):
                continue  # Independently covered by unwind obligations.
        if name in witnessed_data:
            continue
        protected_data.add(name)
        fields = ('type', 'flags', 'size', 'sha256', 'alignment', 'entry_size')
        if (prior is None or sec is None or any(prior[k] != sec[k] for k in fields)):
            raise c.Halt('New or changed allocated data section lacks authenticated proof: '+name)

    def data_relocations(snapshot):
        names = {s['index']: name for name, s in snapshot['sections'].items()}
        result = Counter()
        for r in snapshot['relocations']:
            if r['section'] not in protected_data:
                continue
            symbol = r['symbol']
            index = symbol['section_index']
            definition = names.get(index, f'<special:{index}>')
            result[(r['section'], r['offset'], r['type'], r['addend'], symbol['name'],
                    symbol['type'], symbol['binding'], symbol['other'], definition,
                    symbol['offset'], symbol['size'])] += 1
        return result

    if data_relocations(a) != data_relocations(b):
        raise c.Halt('Allocated data relocation expression changed without authenticated proof')

    def data_symbols(snapshot):
        sections = {s['index']: (name, s) for name, s in snapshot['sections'].items()}
        markers = {m['symbol_index'] for m in snapshot['markers']}
        result = Counter()
        for s in snapshot['symbols']:
            entry = sections.get(s['section_index'])
            if s['symbol_index'] in markers:
                continue
            if s['section_index'] >= 0xff00:
                # COMMON storage and other special-index definitions have no
                # section to inspect. Never mistake them for undefined imports.
                if s['type'] in (0, 1, 5, 6):
                    result[(f"<special:{s['section_index']}>", s['name'], s['type'],
                            s['binding'], s['other'], s['offset'], s['size'])] += 1
                continue
            if not entry or not entry[1]['flags'] & 2:
                continue
            name, sec = entry
            # NOTYPE definitions in data, and OBJECT/COMMON/TLS even in code,
            # are data identities. Section/file symbols and mapping markers are not.
            if s['type'] not in (1, 5, 6) and not (s['type'] == 0 and not sec['flags'] & 4):
                continue
            result[(name, s['name'], s['type'], s['binding'], s['other'], s['offset'], s['size'])] += 1
        return result

    old_data_symbols, new_data_symbols = data_symbols(a), data_symbols(b)
    for symbol in new_data_symbols - old_data_symbols:
        if symbol[0] not in witnessed_data:
            raise c.Halt('New allocated data symbol lacks authenticated proof: '+symbol[1])
    for symbol in old_data_symbols - new_data_symbols:
        if symbol[0] not in witnessed_data:
            raise c.Halt('Lost allocated data symbol lacks authenticated proof: '+symbol[1])
    # Enumerate named additions independently of marker additions, including aliases.
    extra_names = {f['name'] for f in b['functions']} - old_functions - set(selected)
    for section in sorted({f['section'] for f in b['functions'] if f['name'] in extra_names}):
        sec = b['sections'][section]
        fs = [f for f in b['functions'] if f['section'] == section]
        if (sec['type'] != 1 or not sec['flags'] & 4 or sec['size'] % 4 or (section, 'x', 0) not in bc
                or any(not f['name'] or f['offset'] != 0 or not f['size'] or f['size'] != sec['size']
                       or f['binding'] not in (0,1,2) or f['other'] != 0 for f in fs)
                or any(m['section'] == section and (m['kind'],m['offset']) != ('x',0) for m in b['markers'])):
            raise c.Halt('Unsupported named extra function extent/mapping')
        extras.append({'classification': 'unreviewed_named_extra', 'section': section,
                       'section_metadata': sec, 'functions': fs,
                       'extra_names': sorted(f['name'] for f in fs if f['name'] in extra_names),
                       'markers': [m for m in b['markers'] if m['section'] == section],
                       'relocations': [r for r in b['relocations'] if r['section'] == section],
                       'groups': [g for g in b['groups'] if any(m['name'] == section for m in g['members'])],
                       'unwind_section': b['sections'].get('.eh_frame'),
                       'unwind_relocations': [r for r in b['relocations'] if r['section'] == '.eh_frame'
                                              and r['symbol']['section_index'] == sec['index']]})
    old_real = Counter((x['name'],x['type']) for x in a['real_emissions'])
    new_real = Counter((x['name'],x['type']) for x in b['real_emissions'])
    if old_real - new_real: raise c.Halt('Lost owner data/symbol emission')
    witness = {'kind': 'authenticated-aarch64-mapping-v1', 'baseline': a, 'candidate': b,
               'new_selected_sections': sorted(selected_sections),
               'new_extra_sections': extras, 'new_data_sections': data_sections,
               'new_nonallocated_debug_sections': sorted(new_debug),
               'existing_mapping_sequences_identical': True}
    # Only present for a new object, so existing-object witnesses stay unchanged.
    if new_object_metadata:
        witness['new_object_metadata_sections'] = sorted(new_object_metadata)
    if scalar_proof is not None:
        witness['scalar_proof'] = scalar_proof
    if named_sections:
        witness['new_named_data_sections'] = named_sections
    return witness


def extra_obligations(witness):
    # Each complete extra section requires its own decision; never fold into a generic pass.
    obligations = {'unreviewed-extra:'+x['section']: x for x in witness['new_extra_sections']}
    if 'scalar_proof' in witness:
        obligations['new-scalar-data-proof'] = witness['scalar_proof']
    if witness.get('new_named_data_sections'):
        obligations['new-named-data-structure'] = witness['new_named_data_sections']
    return obligations


def verify_obligations(obligations, mode, before, after, selected, *, scalar_context=None, named_data=False):
    witness = compare(before, after, selected, scalar_context=scalar_context, named_data=named_data)
    required = {'owner-mapping-symbols': witness, **extra_obligations(witness)}
    actual_extra = {k for k in obligations if k.startswith(mode+':unreviewed-extra:')}
    expected_extra = {mode+':'+k for k in required if k.startswith('unreviewed-extra:')}
    if actual_extra != expected_extra:
        raise c.Halt('Named-extra obligation omitted or invented')
    if (mode+':new-scalar-data-proof' in obligations) != ('scalar_proof' in witness):
        raise c.Halt('Scalar proof obligation omitted or invented')
    if (mode+':new-named-data-structure' in obligations) != bool(witness.get('new_named_data_sections')):
        raise c.Halt('Named-data structure obligation omitted or invented')
    for key, value in required.items():
        entry = obligations.get(mode+':'+key)
        if entry != {'evidence': value, 'evidence_sha256': c.digest(value)}:
            raise c.Halt('Mapping/extra witness changed or omitted')
    return witness
