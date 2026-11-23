"""Opt-in Lua object comparison with internally regenerated data witnesses.

This is a diagnostic wrapper, not a strict-verifier or receipt policy change.
Call strict.configure(root) first. The outer pilot authenticates source lineage
and tool dependencies; this helper only binds the supplied source/IR bytes.
"""
import copy
import hashlib
from pathlib import Path
import struct
import tempfile

import recovery_mapping
from . import literals, strict
from .elf import Elf, MASKS
from .imports import _require


def _apply_pair(result, function, section_index, witness):
    """Apply a helper-generated witness to exactly two unresolved relocations."""
    offset = witness['object_function_offset']
    base = int(result['original_address'], 16) & 0xffffffff
    size = result['original_size']
    _require(witness['function'] == result['symbol'] == function['name'] and
             witness['original_function_address'] == base and
             witness['original_function_size'] == size == function['size'] and
             type(offset) is int and offset >= 0 and offset % 4 == 0 and offset + 8 <= size and
             witness['original_consumer'] == base + offset, 'data witness function/site differs')
    wanted_sites = [(function['value'] + offset, 275), (function['value'] + offset + 4, 277)]
    selected = []
    for position, kind in wanted_sites:
        matches = [r for r in result['unsupported'] if r.get('section') == function['section']
                   and r.get('offset') == position and r.get('type') == kind]
        _require(len(matches) == 1, 'data witness requires unique unresolved pair')
        rel = matches[0]
        symbol = rel['symbol']
        _require(symbol['section'] == section_index and symbol['type'] == 3 and
                 symbol['value'] == symbol['size'] == rel['addend'] == 0,
                 'data witness relocation identity differs')
        selected.append(rel)
    _require(selected[0]['symbol'] == selected[1]['symbol'], 'data pair targets differ')
    _require(not any(offset <= r['offset'] < offset + 8 for r in result['relocations']),
             'data pair overlaps previously resolved relocation')
    code = bytes.fromhex(result['candidate_bytes_hex'])
    want = bytes.fromhex(result['original_bytes_hex'])
    relocated = bytearray.fromhex(result['relocated_bytes_hex'])
    _require(len(code) == len(want) == len(relocated) == size, 'data comparison extent differs')
    pair = struct.unpack_from('<II', code, offset)
    original_pair = struct.unpack_from('<II', want, offset)
    _require(list(pair) == witness['object_instructions'] and
             list(original_pair) == witness['original_instructions'], 'data witness instruction bytes differ')
    for high, low in (pair, original_pair):
        _require(high & 0x9f000000 == 0x90000000 and low & 0xffc00000 == 0x91000000 and
                 high & 31 != 31 and high & 31 == (low >> 5) & 31 == low & 31,
                 'unsupported data address instruction/register flow')
    _require(pair[0] & MASKS[275] == pair[1] & MASKS[277] == 0 and
             pair[0] & ~MASKS[275] == original_pair[0] & ~MASKS[275] and
             pair[1] & ~MASKS[277] == original_pair[1] & ~MASKS[277],
             'data address opcode/immediate differs')
    target = witness['original_address']
    _require(type(target) is int and 0 <= target < 1 << 64, 'invalid data target')
    delta = ((target & ~4095) - ((base + offset) & ~4095)) >> 12
    _require(-(1 << 20) <= delta < 1 << 20, 'data target outside ADRP range')
    reconstructed = ((pair[0] & ~MASKS[275]) | ((delta & 3) << 29) | (((delta >> 2) & 0x7ffff) << 5),
                     (pair[1] & ~MASKS[277]) | ((target & 4095) << 10))
    _require(reconstructed == original_pair, 'reconstructed data reference differs')
    struct.pack_into('<II', relocated, offset, *reconstructed)
    # Remove only the exact two dictionaries found above. Other unsupported
    # references, including non-relocation control-flow obligations, survive.
    result['unsupported'] = [r for r in result['unsupported'] if all(r is not x for x in selected)]
    for step, rel in enumerate(selected):
        result['relocations'].append({'offset': offset + 4 * step, 'kind': rel['type'],
            'symbol': rel['symbol']['name'], 'addend': rel['addend'], 'resolved_target': hex(target),
            'proof': 'bounded original/object data witness; candidate link not proved',
            'data_section': witness['object_section'], 'data_extent_bytes': witness['extent_bytes']})
    result['relocations'].sort(key=lambda r: r['offset'])
    result['relocated_bytes_hex'] = relocated.hex()
    result['differing_offsets'] = [i for i in range(0, size, 4) if relocated[i:i+4] != want[i:i+4]]
    result['exact'] = not result['unsupported'] and bytes(relocated) == want
    result['reason'] = ('exact' if result['exact'] else 'unsupported_relocation'
                        if result['unsupported'] else 'instructions')


def verify(path, targets, owner_context, object_sha256,
           owner_path, owner_sha256, ir_path, ir_sha256):
    """Recompute witnesses against one pinned snapshot; accept no caller proofs."""
    _require(isinstance(owner_context, strict._OwnerContext), 'authenticated owner context required')
    original_path = Path(owner_context.root) / 'data/2.6.2/main.elf'
    original_sha256 = owner_context.original_sha256
    object_bytes = literals._read(path, object_sha256)
    with tempfile.TemporaryDirectory(prefix='lua-data-object-') as directory:
        snapshot = Path(directory) / 'owner.o'
        snapshot.write_bytes(object_bytes)
        snapshot.chmod(0o400)
        reference = strict.verify(snapshot, targets, owner_context=owner_context, original_imports=True)
        if owner_context.owner == literals.OWNER:
            proof = literals.prove_lstate_literal(original_path, original_sha256, snapshot, object_sha256,
                owner_path, owner_sha256, ir_path, ir_sha256, owner_context)
            witnesses = [proof]
        else:
            from . import tables
            _require(owner_context.owner == tables.OWNER, 'unsupported data witness owner')
            proof = tables.prove_lgc_tables(original_path, original_sha256, snapshot, object_sha256,
                owner_path, owner_sha256, ir_path, ir_sha256, owner_context)
            witnesses = proof['tables']
        _require(proof['original_sha256'] == original_sha256 and proof['object_sha256'] == object_sha256
                 and proof['owner'] == owner_context.owner and proof['receipt_acceptance'] is False and
                 proof['owner_sha256'] == owner_sha256 and proof['ir_sha256'] == ir_sha256 and
                 proof['original_inventory_pins'] == dict(owner_context.pins),
                 'data witness input binding differs')
        elf = Elf(snapshot)
        inspected = recovery_mapping.inspect(snapshot)
        _require(hashlib.sha256(elf.d).hexdigest() == object_sha256, 'private object changed')
        symbols = strict.symbols(elf)
        functions = {s['name']: s for s in symbols.values() if s['type'] == 2 and s['section']}
        reference['object'] = str(path)
        comparison = copy.deepcopy(reference)
        rows = {r['symbol']: r for r in comparison['results']}
        seen = set()
        for witness in witnesses:
            name = witness['function']
            _require(name not in seen and name in rows, 'duplicate or missing data witness function')
            seen.add(name)
            section = inspected['sections'][witness['object_section']]
            _require(section['type'] == 1 and section['flags'] & 7 == 2 and
                     section['size'] == witness['extent_bytes'], 'data witness section extent differs')
            _apply_pair(rows[name], functions[name], section['index'], witness)
        # Proofs and comparison consumed immutable private bytes. Recheck the
        # caller's pinned inputs too, so a changed input cannot be silently saved.
        _require(literals._read(snapshot, object_sha256) == object_bytes, 'private object changed')
    for filename, digest in ((path, object_sha256), (original_path, original_sha256),
                             (owner_path, owner_sha256), (ir_path, ir_sha256)):
        literals._read(filename, digest)
    _require(strict.build_owner_context(owner_context.root, owner_context.owner,
             dict(owner_context.pins)) == owner_context, 'owner context changed')
    return {'kind': 'lua-object-data-comparison-v1', 'accepted': False,
            'scope': 'function instructions with bounded data witnesses; linked layout unproved',
            'source_ir_object_lineage_proven': False, 'original_allocation_identity_proven': False,
            'global_control_flow_proven': False,
            'candidate_linked_data_proven': False, 'shared_consumers_proven': False,
            'receipt_acceptance': False, 'reference_comparison': reference,
            'comparison': comparison, 'data_witness': proof}
