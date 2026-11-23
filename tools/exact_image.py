#!/usr/bin/env python3
"""Read-only pinned NSO byte comparison; byte equality is not source certification.

No relocations, traps, padding or header fields are normalized. NSO segment
diagnostics are separate from whole-container equality. Nothing is executed.
"""
import argparse
import hashlib
import json
import os
from pathlib import Path
import stat
import struct

from verify_original_inputs import load_manifest, stat_identity

ROOT = Path(__file__).resolve().parents[1]
MAX_FILE = 128 * 1024**2
MAX_DECODED = 128 * 1024**2
MAX_RANGES = 1024


class ImageError(ValueError):
    pass


def require(condition, message):
    if not condition:
        raise ImageError(message)


def stable_read(path):
    """Bound memory and bind the bytes to a stable regular path/open descriptor."""
    path = Path(path)
    before = path.lstat()
    require(stat.S_ISREG(before.st_mode), 'regular file required; no symlinks')
    require(0 <= before.st_size <= MAX_FILE, 'file exceeds bounded reader')
    fd = os.open(path, os.O_RDONLY | os.O_NONBLOCK | os.O_NOFOLLOW)
    with os.fdopen(fd, 'rb') as stream:
        require(stat_identity(before) == stat_identity(os.fstat(fd)), 'input changed before read')
        data = stream.read(MAX_FILE + 1)
        require(len(data) == before.st_size, 'input size changed during read')
        require(stat_identity(before) == stat_identity(os.fstat(fd)), 'input changed during read')
    require(stat_identity(before) == stat_identity(path.lstat()), 'input path changed during read')
    return data


def sha(data):
    return hashlib.sha256(data).hexdigest()


def decode_lz4(block, expected):
    """Bounded independent LZ4 block, including overlapping match copies.

    Format: github.com/lz4/lz4/blob/dev/doc/lz4_Block_format.md.
    NSO supplies both the compressed extent and exact decompressed size.
    """
    require(type(expected) is int and 0 <= expected <= MAX_DECODED, 'LZ4 output bound')
    output = bytearray()
    cursor = 0
    last_match = None

    def length(initial):
        nonlocal cursor
        value = initial
        if initial == 15:
            while True:
                require(cursor < len(block), 'truncated LZ4 length')
                extra = block[cursor]
                cursor += 1
                value += extra
                require(value <= expected, 'LZ4 length exceeds output bound')
                if extra != 255:
                    break
        return value

    while cursor < len(block):
        token = block[cursor]
        cursor += 1
        literals = length(token >> 4)
        require(cursor + literals <= len(block), 'truncated LZ4 literals')
        require(len(output) + literals <= expected, 'LZ4 literals exceed output bound')
        output.extend(block[cursor:cursor + literals])
        cursor += literals
        if cursor == len(block):
            require(len(output) == expected, 'LZ4 output size mismatch')
            if last_match is not None:
                require(literals >= 5 and expected - last_match >= 12, 'invalid LZ4 final sequence')
            return bytes(output)
        require(cursor + 2 <= len(block), 'truncated LZ4 offset')
        offset = int.from_bytes(block[cursor:cursor + 2], 'little')
        cursor += 2
        require(0 < offset <= len(output), 'invalid LZ4 match offset')
        count = length(token & 15) + 4
        require(len(output) + count <= expected, 'LZ4 match exceeds output bound')
        last_match = len(output)
        # A match repeats the preceding offset-byte period; cap the copied period
        # by the match length so peak allocation remains bounded by expected.
        begin = len(output) - offset
        period = bytes(output[begin:begin + min(offset, count)])
        repeats, remainder = divmod(count, len(period))
        output.extend(period * repeats)
        output.extend(period[:remainder])
    raise ImageError('LZ4 block has no final literal sequence')


def inspect_nso(data):
    """Inspect the supported NSO0 shape; never omit bytes from raw comparison.

    Header layout corroborated against hactool nso.h/nso.c. Unknown header
    payload fields are hashed as part of the complete 256-byte header.
    """
    require(len(data) >= 256 and data[:4] == b'NSO0', 'not a supported NSO0 container')
    version, reserved, flags = struct.unpack_from('<III', data, 4)
    require(version == 0 and reserved == 0 and not flags & ~0x3f, 'unsupported NSO header version/flags')
    descriptors = []
    for i, name in enumerate(('text', 'rodata', 'data')):
        offset, address, size, extra = struct.unpack_from('<IIII', data, 16 + 16 * i)
        compressed = bool(flags & (1 << i))
        stored = struct.unpack_from('<I', data, 96 + i * 4)[0] if compressed else size
        require(size > 0 and stored > 0 and offset >= 256 and offset + stored <= len(data), 'NSO segment file extent')
        require(address + size <= 2**32, 'NSO segment address overflow')
        descriptors.append(dict(name=name, file_offset=offset, address=address,
                                size=size, stored_size=stored, compressed=compressed, extra=extra))
    require(sum(x['size'] for x in descriptors) <= MAX_DECODED, 'aggregate NSO decoded bound')
    for key, length_key in (('file_offset', 'stored_size'), ('address', 'size')):
        ordered = sorted(descriptors, key=lambda x: x[key])
        require(all(a[key] + a[length_key] <= b[key] for a, b in zip(ordered, ordered[1:])),
                'overlapping NSO segment extents')
    require([x['address'] for x in descriptors] == sorted(x['address'] for x in descriptors),
            'unsupported NSO segment memory order')
    module_offset, module_size = descriptors[0]['extra'], descriptors[1]['extra']
    if module_size:
        require(module_offset >= 256 and module_offset + module_size <= len(data), 'NSO module-name extent')
        require(all(module_offset + module_size <= x['file_offset'] or
                    x['file_offset'] + x['stored_size'] <= module_offset for x in descriptors),
                'NSO module name overlaps segment')
    bss_address = descriptors[2]['address'] + descriptors[2]['size']
    bss_size = descriptors[2]['extra']
    require(bss_address + bss_size <= 2**32, 'NSO BSS address overflow')
    segments = []
    for i, desc in enumerate(descriptors):
        content = data[desc['file_offset']:desc['file_offset'] + desc['stored_size']]
        if desc['compressed']:
            content = decode_lz4(content, desc['size'])
        digest = sha(content)
        hash_enabled = bool(flags & (1 << (i + 3)))
        if hash_enabled:
            require(digest == data[160 + 32 * i:192 + 32 * i].hex(), 'NSO declared segment hash mismatch')
        segments.append({k: v for k, v in desc.items() if k != 'extra'} |
                        {'sha256': digest, 'declared_hash_checked': hash_enabled})
    return {'header_sha256': sha(data[:256]), 'flags': flags,
            'build_id': data[64:96].hex(), 'segments': segments,
            'bss': {'address': bss_address, 'size': bss_size},
            'module_name': {'file_offset': module_offset, 'size': module_size}}


def differences(original, candidate, limit):
    require(type(limit) is int and 1 <= limit <= MAX_RANGES, 'invalid mismatch-range limit')
    if original == candidate:
        return {'differing_bytes': 0, 'range_count': 0, 'ranges': [], 'ranges_truncated': False}
    ranges, count, differing, start = [], 0, 0, None
    common = min(len(original), len(candidate))

    def close(end):
        nonlocal start, count
        if start is not None:
            count += 1
            if len(ranges) < limit:
                ranges.append({'start': start, 'end_exclusive': end})
            start = None

    for base in range(0, common, 1024 * 1024):
        left, right = original[base:min(common, base + 1024 * 1024)], candidate[base:min(common, base + 1024 * 1024)]
        if left == right:
            close(base)
            continue
        for i, (a, b) in enumerate(zip(left, right), base):
            if a != b:
                differing += 1
                if start is None:
                    start = i
            else:
                close(i)
    if len(original) != len(candidate):
        differing += abs(len(original) - len(candidate))
        if start is None:
            start = common
    close(max(len(original), len(candidate)))
    return {'differing_bytes': differing, 'range_count': count, 'ranges': ranges,
            'ranges_truncated': count > limit}


def compare(reference, candidate, pin, max_ranges=128):
    """The caller supplies an authenticated original pin, not a generated hash."""
    original = stable_read(reference)
    require(len(original) == pin['size'] and sha(original) == pin['sha256'], 'original NSO pin mismatch')
    old = inspect_nso(original)
    new_bytes = stable_read(candidate)
    result = {'kind': 'exact-nso-byte-comparison-v1', 'accepted': False,
              'source_and_build_lineage': 'not_assessed',
              'reference': {'size': len(original), 'sha256': sha(original)},
              'candidate': {'size': len(new_bytes), 'sha256': sha(new_bytes)},
              'container_bytes_equal': original == new_bytes,
              'normalization': 'none', 'differences': differences(original, new_bytes, max_ranges),
              'original_nso': old}
    try:
        new = inspect_nso(new_bytes)
    except ImageError as error:
        result.update(candidate_nso={'supported': False, 'error': str(error)},
                      declared_segment_bytes_equal=False, declared_memory_layout_equal=False,
                      bss_equal=False, header_bytes_equal=False)
    else:
        result.update(candidate_nso={'supported': True, **new},
                      declared_segment_bytes_equal=all(a['size'] == b['size'] and a['sha256'] == b['sha256']
                          for a, b in zip(old['segments'], new['segments'])),
                      declared_memory_layout_equal=all((a['address'], a['size']) == (b['address'], b['size'])
                          for a, b in zip(old['segments'], new['segments'])),
                      bss_equal=old['bss'] == new['bss'], header_bytes_equal=original[:256] == new_bytes[:256])
    return result


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--original', type=Path, default=ROOT / 'build/exefs/main')
    parser.add_argument('--candidate', type=Path, required=True)
    parser.add_argument('--max-ranges', type=int, default=128)
    args = parser.parse_args(argv)
    try:
        pin = next(x for x in load_manifest(ROOT)['inputs'] if x['id'] == 'nso')
        result = compare(args.original, args.candidate, pin, args.max_ranges)
    except (OSError, ValueError, RuntimeError) as error:
        print(json.dumps({'kind': 'exact-nso-byte-comparison-v1', 'accepted': False,
                          'status': 'unverified', 'error': str(error)}))
        return 2
    print(json.dumps(result, indent=2))
    return 0 if result['container_bytes_equal'] else 1


if __name__ == '__main__':
    raise SystemExit(main())
