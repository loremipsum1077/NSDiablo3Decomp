#!/usr/bin/env python3
"""Cross-check pinned original and NSS zero-fill metadata; never certify ownership.

Reports corroboration only; does not alter the data ledger or recovery gates.
Caller-supplied hashes establish consistency with those pins; their trusted
provenance and the authenticity of NSS metadata must be established separately.
Addresses use the common unrebased ELF virtual address space.
"""
import argparse
import hashlib
import json
import re
from pathlib import Path
from data_elf import inspect_elf

LIMIT = 1 << 64


def require(ok, message):
    if not ok:
        raise ValueError(message)


def checked_end(start, size):
    require(type(start) is int and type(size) is int and 0 <= start < LIMIT
            and 0 <= size < LIMIT and start + size <= LIMIT, 'invalid address extent')
    return start + size


def loads(image):
    values = [x for x in image['segments'] if x['type'] == 1 and x['memory_size']]
    require(values, 'missing load segments')
    for x in values:
        checked_end(x['address'], x['memory_size'])
        checked_end(x['address'], x['file_size'])
        require(x['file_size'] <= x['memory_size'], 'invalid load size')
    ordered = sorted(values, key=lambda x:x['address'])
    require(all(checked_end(a['address'],a['memory_size']) <= b['address']
                for a,b in zip(ordered,ordered[1:])), 'overlapping load segments')
    return ordered


def extract(original_path, nss_path, original_sha256, nss_sha256, name, address):
    for value in (original_sha256, nss_sha256):
        require(isinstance(value,str) and re.fullmatch(r'[0-9a-f]{64}',value), 'expected pinned SHA-256')
    require(isinstance(name,str) and name and type(address) is int, 'invalid identity')
    original, nss = inspect_elf(original_path), inspect_elf(nss_path)
    require(original['sha256'] == original_sha256 and nss['sha256'] == nss_sha256,
            'input hash differs from pin')
    require(original['elf_type'] == nss['elf_type'] and original['machine'] == nss['machine'],
            'ELF identity differs')
    al, bl = loads(original), loads(nss)
    original_bytes, nss_bytes = Path(original_path).read_bytes(), Path(nss_path).read_bytes()
    require(hashlib.sha256(original_bytes).hexdigest() == original_sha256
            and hashlib.sha256(nss_bytes).hexdigest() == nss_sha256, 'input changed while reading')
    a_file = [x for x in al if x['file_size']]
    b_file = [x for x in bl if x['file_size']]
    key = lambda x: (x['address'],x['file_size'],x['flags'])
    require([key(x) for x in a_file] == [key(x) for x in b_file], 'file-backed load layouts differ')
    matched = []
    for a,b in zip(a_file,b_file):
        x = original_bytes[a['offset']:a['offset']+a['file_size']]
        y = nss_bytes[b['offset']:b['offset']+b['file_size']]
        require(len(x)==a['file_size'] and x==y, 'file-backed load bytes differ')
        matched.append(dict(original=a,nss=b,sha256=hashlib.sha256(x).hexdigest()))
    candidates = [s for s in nss['symbols'] if s['name'] == name]
    require(len(candidates)==1, 'missing or ambiguous NSS symbol')
    s = candidates[0]
    require(s['address']==address and s['size']>0 and s['type']==1 and s['binding']==1
            and s['visibility']==0 and s['defined'] and 0<s['section_index']<0xff00
            and any(t['index']==s['table_section'] and t['type']==2 for t in nss['sections']),
            'unsupported NSS object identity')
    end=checked_end(address,s['size'])
    overlaps = [x for x in nss['symbols'] if x is not s and x['defined']
                and x['type'] in (1,2) and x['size']>0 and x['address']<end
                and address<checked_end(x['address'],x['size'])]
    require(not overlaps, 'overlapping NSS object identity')
    def evidence(img, ls):
        sections=[x for x in img['sections'] if x['flags'] & 2 and x['size']
                  and x['address'] < end and address < checked_end(x['address'],x['size'])]
        require(len(sections)==1 and sections[0]['type']==8 and sections[0]['flags']==3
                and sections[0]['address']<=address and end<=checked_end(sections[0]['address'],sections[0]['size']),
                'object not in unique writable NOBITS section')
        require(not any(x['type']==7 and x['address']<end
                        and address<checked_end(x['address'],x['memory_size']) for x in img['segments']),
                'object overlaps TLS segment')
        spans=[x for x in ls if x['address']+x['file_size']<=address
               and end<=checked_end(x['address'],x['memory_size']) and x['flags']==6]
        require(len(spans)==1, 'object not in unique writable zero-fill load')
        require(not any(x['type']==19 for x in img['sections']) and
                not any(x['tag'] in (35,36,37) for x in img['dynamic_tags']), 'packed relocation metadata unsupported')
        # Explicit relocations in or immediately before the object can write into it.
        # Widths of unknown relocations are not inferred; fail if one is anywhere in the image.
        widths={257:8,1025:8,1026:8,1027:8}
        require(all(r['type'] in widths for r in img['relocations']), 'unsupported relocation type')
        writes=[r for r in img['relocations'] if r['offset']<end
                and address<checked_end(r['offset'],widths[r['type']])]
        require(not writes, 'relocation writes to object')
        return dict(section=sections[0],zero_fill_load=spans[0],observed_section_relocation_writes=writes)
    left=evidence(original,al); right=evidence(nss,bl)
    require(right['section']['index']==s['section_index'], 'NSS symbol section mismatch')
    return dict(kind='pinned-original-nss-zero-fill-corroboration-v1',
                limitation='Corroborates matching file-backed loads and target zero-fill coverage only. Section-table relocation checks do not prove absence of dynamic writes; symbol st_other upper bits are not exposed. Does not establish owner, type alignment, runtime state, or recovery admission.',
                coverage=dict(file_backed_loads='matched',target_zero_fill='matched',
                              section_table_relocation_writes='none_observed',
                              dynamic_loader_relocation_writes='unknown',
                              symbol_other_upper_bits='unknown',object_alignment='unknown',owner='unknown'),
                original_sha256=original_sha256,nss_sha256=nss_sha256,identity=dict(name=name,address=address),
                nss_symbol=s,matched_file_backed_loads=matched,original=left,nss=right)


def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--original',required=True);p.add_argument('--nss',required=True)
    p.add_argument('--original-sha256',required=True);p.add_argument('--nss-sha256',required=True)
    p.add_argument('--name',required=True);p.add_argument('--address',required=True,type=lambda x:int(x,0))
    a=p.parse_args()
    print(json.dumps(extract(a.original,a.nss,a.original_sha256,a.nss_sha256,a.name,a.address),indent=2))

if __name__=='__main__':main()
