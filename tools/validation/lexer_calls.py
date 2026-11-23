"""Resolve every lexer CALL26 against authenticated original identities.

Object instruction correspondence only. No candidate linkage or provider proof.
"""
from collections import Counter, defaultdict
import csv
import io
from pathlib import Path
import struct
from types import SimpleNamespace

from . import qualified_owner as qualified
from . import strict
from .imports import prove_original_call


def require(condition, message):
    if not condition:
        raise ValueError(message)


def patch_call(candidate_word, original_word, address, target):
    require(candidate_word == 0x94000000 and original_word >> 26 == 0b100101,
            'CALL26 needs zero-immediate RELA BL and original BL')
    delta = target - address
    require(delta % 4 == 0 and -(1 << 27) <= delta < (1 << 27), 'CALL26 target out of range')
    patched = candidate_word | ((delta >> 2) & 0x3ffffff)
    require(patched == original_word, 'original call target differs from candidate identity')
    return patched


def defined_target(symbol, addend, members):
    """A defined section/function reference must name one exact member entry."""
    require(symbol['type'] in (2, 3), 'defined call does not reference code identity')
    matches = [m for m in members if m.candidate_section == symbol['section'] and
               m.candidate_offset == symbol['value'] + addend]
    require(len(matches) == 1, 'defined call target is not a unique function entry')
    member = matches[0]
    if symbol['type'] == 2:
        require(addend == 0 and symbol['name'] == member.candidate_name and
                symbol['size'] == member.candidate_size, 'named function call identity differs')
    else:
        require(not symbol['name'] and symbol['value'] == 0 and symbol['size'] == 0,
                'noncanonical section symbol')
    return member


def executable_relocations(elf, members):
    syms = strict.symbols(elf)
    rels = strict.relocs(elf, syms)
    found = []
    occupied = set()
    for rel in rels:
        if not elf.sections[rel['section']][2] & 4:
            continue
        require(rel['type'] in (283, 275, 277) and rel['offset'] % 4 == 0,
                'unsupported executable relocation')
        matches = [m for m in members if m.candidate_section == rel['section'] and
                   m.candidate_offset <= rel['offset'] and
                   rel['offset'] + 4 <= m.candidate_offset + m.candidate_size]
        require(len(matches) == 1, 'executable relocation outside unique member')
        key = rel['section'], rel['offset']
        require(key not in occupied, 'overlapping executable relocations')
        occupied.add(key)
        found.append((matches[0], rel))
    return syms, found


def external_target(name, address, size, symbols, sections, reader, original):
    require(address % 4 == 0 and size > 0 and size % 4 == 0, 'invalid external function extent')
    at_entry = [s for s in symbols if s[1] in (2, 10) and s[4] and s[5] == address]
    require(len(at_entry) == 1 and at_entry[0][0] == name and at_entry[0][1] == 2 and
            at_entry[0][6] == size, 'external function NSS identity differs')
    symbol = at_entry[0]
    index = symbol[4]
    require(0 < index < len(sections) and index < 0xff00, 'invalid external NSS symbol section')
    section = sections[index]
    require(section[1] == 1 and section[2] & 7 == 6 and section[3] <= address and
            address + size <= section[3] + section[5], 'external NSS function not executable/file-backed')
    others = [s for s in symbols if s is not symbol and s[1] in (2, 10) and s[4] and
              s[5] < address + size and address < s[5] + s[6]]
    require(not others, 'overlapping external NSS function identities')
    code = qualified._file_mapped_code(reader, address, size)
    require(code == reader.read(section[4] + address - section[3], size) ==
            qualified._mapped_code(original, address, size), 'external NSS section/original code differs')
    return address


def _prove(context, nss):
    root = Path(context.root)
    pins = dict(context.input_pins)
    original = qualified._snapshot(root / qualified.PINS['original_sha256'],
                                   qualified.LIMITS['original_sha256'], pins['original_sha256'])
    inventory = qualified._snapshot(root / qualified.PINS['functions_sha256'],
                                    qualified.LIMITS['functions_sha256'], pins['functions_sha256'])
    candidate = qualified._snapshot(context.object_path, 4 * 1024**2, context.object_sha256)
    elf = SimpleNamespace(d=candidate, sections=qualified._table(candidate, 1))
    strict._object_layout(elf)
    symbols, rels = executable_relocations(elf, context.members)
    table = next(s for s in elf.sections if s[1] == 2)
    attrs = {id(symbols[i]): struct.unpack_from('<IBBHQQ', candidate, table[4] + i * 24)[1:3]
             for i in symbols}
    names = defaultdict(list)
    for row in csv.DictReader(io.StringIO(inventory.decode(), newline='')):
        names[row['Name']].append(row)
    nss_sections = qualified._file_table(nss)
    nss_symbols = qualified._file_symbols(nss, nss_sections)
    witnesses = []
    for member, rel in rels:
        if rel['type'] != 283:
            continue
        require(member.candidate_size == member.original_size, 'call-site function size differs')
        offset = rel['offset'] - member.candidate_offset
        pc = (member.original_address & 0xffffffff) + offset
        word = struct.unpack_from('<I', candidate, elf.sections[rel['section']][4] + rel['offset'])[0]
        original_word = struct.unpack('<I', qualified._mapped_code(original, pc, 4))[0]
        symbol = rel['symbol']
        import_witness = None
        if symbol['section']:
            if symbol['type'] == 3:
                require(attrs[id(symbol)] == (3, 0), 'section call symbol must be local/default')
            target_member = defined_target(symbol, rel['addend'], context.members)
            target = target_member.original_address & 0xffffffff
            name = target_member.original_name
            category = 'owner_section' if symbol['type'] == 3 else 'owner_named'
        else:
            info, other = attrs[id(symbol)]
            require(info >> 4 == 1 and info & 15 in (0, 2) and other == 0 and
                    symbol['value'] == symbol['size'] == rel['addend'] == 0 and symbol['name'] and
                    sum(s['name'] == symbol['name'] for s in symbols.values()) == 1,
                    'ambiguous or unsupported undefined call symbol')
            name = symbol['name']
            known = names.get(name, [])
            require(len(known) <= 1 and '@' not in name, 'ambiguous external function identity')
            if known:
                row = known[0]
                address = int(row['Address'], 16)
                require(address >> 32 == 0x71, 'external function address base differs')
                target = address & 0xffffffff
                size = int(row['Size'])
                external_target(name, target, size, nss_symbols, nss_sections, nss, original)
                category = 'original_named'
            else:
                import_witness = prove_original_call(root / qualified.PINS['original_sha256'],
                                                      pins['original_sha256'], pc, name)
                target = import_witness['plt_address']
                category = 'original_import'
        patched = patch_call(word, original_word, pc, target)
        record = {'function': member.original_name, 'offset': offset,
                  'candidate_section': member.candidate_section, 'candidate_offset': rel['offset'],
                  'original_call_address': pc, 'target_identity': name, 'target_address': target,
                  'category': category, 'candidate_word': word, 'patched_word': patched}
        if import_witness is not None:
            record['import_witness'] = import_witness
        witnesses.append(record)
    require(len(witnesses) == 120, 'unexpected complete lexer call membership')
    return {'kind': 'lua-lexer-call-correspondence-v1', 'accepted': False,
            'candidate_linked_imports_proven': False, 'provider_bodies_proven': False,
            'object_sha256': context.object_sha256, 'original_sha256': pins['original_sha256'],
            'nss_sha256': pins['nss_sha256'], 'counts': dict(Counter(w['category'] for w in witnesses)),
            'witnesses': witnesses}


def prove(context):
    qualified.revalidate_context(context)
    pins = dict(context.input_pins)
    with qualified._pinned_reader(Path(context.root) / qualified.PINS['nss_sha256'],
                                  qualified.LIMITS['nss_sha256'], pins['nss_sha256']) as nss:
        result = _prove(context, nss)
    qualified.revalidate_context(context)
    return result
