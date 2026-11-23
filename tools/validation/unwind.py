"""Conservative EH check promoted from harvest audit_unwind.py/mdct parser.

Unsupported encodings, absent modes, missing or ambiguous FDEs are pending.
No compilation or generated program execution.
"""
import struct
from .strict import Elf, symbols, relocs


def uleb(b, p):
    value = shift = 0
    while p < len(b) and shift < 64:
        c = b[p]
        p += 1
        value |= (c & 127) << shift
        shift += 7
        if not c & 128:
            return value, p
    raise ValueError('Malformed LEB')


def encoding(body):
    end = body.index(0, 1)
    if body[1:end] != b'zR':
        raise ValueError('Unsupported CIE')
    p = end + 1
    _, p = uleb(body, p)
    _, p = uleb(body, p)
    p += 1
    _, p = uleb(body, p)
    return body[p]


def frame(e, linked):
    shstr = struct.unpack_from('<H', e.d, 0x3e)[0]
    strings = e.sections[shstr]
    found = []
    for index, section in enumerate(e.sections):
        start = strings[4] + section[0]
        name = e.d[start:e.d.index(b'\0', start)]
        if name == b'.eh_frame':
            found.append((index, section))
    if len(found) != 1:
        raise ValueError('Unique .eh_frame required')
    index, section = found[0]
    data = e.d[section[4]:section[4] + section[5]]
    fdes, cies, at = [], {}, 0
    while at + 8 <= len(data):
        n, ident = struct.unpack_from('<II', data, at)
        if n == 0:
            break
        if n == 0xffffffff or n < 4 or at + 4 + n > len(data):
            raise ValueError('Unsupported EH record')
        if ident == 0:
            body = data[at+8:at+4+n]
            if encoding(body) != 0x1b:
                raise ValueError('Unsupported pointer encoding')
            cies[at] = body
        else:
            if n < 12:
                raise ValueError('Truncated FDE')
            pc = section[3] + at + 8 + struct.unpack_from('<i', data, at+8)[0] if linked else None
            fdes.append({'at': at, 'cie': at+4-ident, 'pc': pc,
                         'range': struct.unpack_from('<I', data, at+12)[0],
                         'instr': data[at+16:at+4+n]})
        at += 4+n
    return index, fdes, cies


def one(rows):
    if len(rows) != 1:
        raise ValueError('Unique EH identity required')
    return rows[0]


def audit(original, obj, links, target):
    try:
        o, e = Elf(original), Elf(obj)
        _, of, oc = frame(o, True)
        index, fs, cs = frame(e, False)
        syms = symbols(e)
        s = one([v for v in syms.values() if v['name'] == target['Name'] and v['section'] and v['type'] == 2])
        rel = one([r for r in relocs(e, syms) if r['section'] == index and r['type'] == 261 and
                   r['symbol']['section'] == s['section'] and r['symbol']['value']+r['addend'] == s['value']])
        f = one([v for v in fs if v['at']+8 == rel['offset']])
        orig = one([v for v in of if v['pc'] & 0xffffffff == int(target['Address'], 16) & 0xffffffff])
        if not (f['range'] == s['size'] == int(target['Size']) == orig['range'] and
                f['instr'].rstrip(b'\0') == orig['instr'].rstrip(b'\0')):
            raise ValueError('Original EH program/range differs')
        modes = {}
        for mode in ('normal', 'lua'):
            le = Elf(links[mode])
            _, lf, lc = frame(le, True)
            ls = one([v for v in symbols(le).values() if v['name'] == target['Name'] and v['section'] and v['type'] == 2])
            linked = one([v for v in lf if v['pc'] == ls['value']])
            modes[mode] = (linked['range'] == f['range'] and
                           linked['instr'].rstrip(b'\0') == f['instr'].rstrip(b'\0') and
                           lc[linked['cie']].rstrip(b'\0') == cs[f['cie']].rstrip(b'\0'))
        return {'status': 'pass' if all(modes.values()) else 'pending', 'links': modes,
                'original_program_identical': True, 'original_cie': oc[orig['cie']].hex()}
    except (KeyError, ValueError, IndexError, struct.error) as error:
        return {'status': 'pending', 'reason': str(error)}
