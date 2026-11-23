"""Read-only verification of the link rule's ELF/map provenance record.

The record binds artifact bytes, not a source revision, owner, or build recipe.
Only the successful link rule may produce it; this module never repairs records.
"""
import hashlib
import json
from pathlib import Path
import re


_HASH = re.compile(r'[0-9a-f]{64}\Z')
_FIELDS = {'format', 'elf_sha256', 'map_sha256'}


def _unique_object(pairs):
    result = {}
    for key, value in pairs:
        if key in result:
            raise ValueError(f'duplicate provenance field: {key}')
        result[key] = value
    return result


def _read(path, label):
    try:
        return Path(path).read_bytes()
    except OSError as exc:
        raise ValueError(f'cannot read {label} {path}: {exc}') from exc


def verify_link_provenance(rebuilt_path, map_path=None, provenance_path=None):
    """Verify format 1 provenance and return hashes of the exact input bytes.

    Defaults follow the link rule: ELF.map and ELF.map.provenance. Explicit
    paths support archived artifacts; format 1 contains no path/name fields.
    Unknown fields are rejected instead of silently implying their validation.
    Missing, malformed, or stale evidence raises ValueError. No input is written.
    Callers must bind their parsed artifacts to these same hashes if files can
    change between verification and parsing.
    """
    rebuilt_path = Path(rebuilt_path)
    map_path = Path(map_path) if map_path is not None else Path(str(rebuilt_path) + '.map')
    provenance_path = (Path(provenance_path) if provenance_path is not None
                       else Path(str(map_path) + '.provenance'))
    record_bytes = _read(provenance_path, 'link provenance')
    try:
        record = json.loads(record_bytes.decode('utf-8'), object_pairs_hook=_unique_object)
    except (UnicodeError, ValueError) as exc:
        raise ValueError(f'malformed link provenance {provenance_path}: {exc}') from exc
    if not isinstance(record, dict) or set(record) != _FIELDS:
        raise ValueError('link provenance must contain exactly format, elf_sha256 and map_sha256')
    if type(record['format']) is not int or record['format'] != 1:
        raise ValueError('unsupported link provenance format (expected integer 1)')
    for key in ('elf_sha256', 'map_sha256'):
        if not isinstance(record[key], str) or _HASH.fullmatch(record[key]) is None:
            raise ValueError(f'invalid link provenance {key}: expected lowercase SHA256')
    actual = {
        'elf_sha256': hashlib.sha256(_read(rebuilt_path, 'rebuilt ELF')).hexdigest(),
        'map_sha256': hashlib.sha256(_read(map_path, 'link map')).hexdigest(),
    }
    for key, digest in actual.items():
        if digest != record[key]:
            raise ValueError(f'stale link provenance: {key} does not match artifact bytes; relink to regenerate')
    return dict(status='verified_link_artifacts', format=1, **actual,
                record_sha256=hashlib.sha256(record_bytes).hexdigest())
