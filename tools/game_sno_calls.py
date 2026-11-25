#!/usr/bin/env python3
"""Generate generated-game-data/sno_calls.inc and gbid_rows.inc for a version without its .nss.

2.6.2's includes come from tools/game_sno_tables.py, which places each sLookupSNO call on
its original source line from the .nss line table. Later versions ship no .nss, so:

- every sLookupSNO call is decoded from GlobalSNOListRefresh in data/<version>/main.elf
  (tools/global_sno_map.py), in code order;
- a call that 2.6.2 also makes (same platforms, bucket, group, path, flags and extras-ness)
  reuses its 2.6.2 line, with the index renamed to the enumerator that now has that value;
  extras calls (index nExtrasIndex) pair by position, since some name their path through a
  string variable;
- a new call is written from its decoded fields;
- lines are emitted in order, the nExtrasIndex declaration before the first extras call.
  Line numbers are not known (nothing in these calls uses __LINE__).

gbid_rows.inc is rendered from the version's sgtGlobalGBIDs table.

  python3 tools/game_sno_calls.py            # report
  python3 tools/game_sno_calls.py --apply    # write both includes (git-ignored outputs)

Needs the 2.6.2 sno_calls.inc as the template: keep a copy at
build/game-data-2.6.2/sno_calls.inc (made on first --apply from the installed file).
"""
from __future__ import annotations

import argparse
import json
import re
import shutil
import sys
from collections import defaultdict, deque
from pathlib import Path

import global_sno_map as gsm

ROOT = Path(__file__).resolve().parent.parent
OUT = ROOT / 'src/X/Common/generated-game-data'
TEMPLATE = ROOT / 'build/game-data-2.6.2/sno_calls.inc'
PLATFORMS = ['SIGMAPLATFORM_' + s for s in ('PC', 'AXE', 'XBOXONE', 'PS4', 'NX64')]
FLAGS = ['GLOBALSNO_FLAG_' + s + '_BIT' for s in
         ('DO_NOT_PRELOAD', 'DO_NOT_TRAVERSE_IN_COMMON_CPK', 'DO_NOT_TRAVERSE_IN_ACT_CPK_SPLIT')]
BUCKETS = ['BUCKET_' + s for s in ('TOOLS_AND_TESTING', 'PVP', 'MAIN', 'NON_BETA', 'BETA_ONLY')]
CONSOLE = 'SigmaPlatformIsConsole(SigmaPlatformGet()) ? (1 << GLOBALSNO_FLAG_DO_NOT_TRAVERSE_IN_COMMON_CPK_BIT) : 0'
GB_TYPES = {1: 'GB_ITEMTYPES', 2: 'GB_ITEMS', 4: 'GB_HIRELINGS', 7: 'GB_HEROS', 18: 'GB_MONSTER_AFFIXES'}
DECL = '    int32 nExtrasIndex = NUM_GLOBAL_SNOS;'


def enum_names(text: str, pattern: str) -> dict[int, str]:
    return {int(v): n for n, v in re.findall(rf'^\s*({pattern})\s*=\s*(-?\d+),', text, re.MULTILINE)}


def flag_expression(bits: int, names: list[str]) -> str:
    if bits >> len(names):
        raise ValueError(f'unexpected flag bits {bits:#x}')
    return ' | '.join(f'(1 << {n})' for i, n in enumerate(names) if bits & (1 << i)) or '0'


def _key(call: dict) -> tuple:
    return (call['mask'], call['bucket'], call['group'], call['path'], call['flags'], call['index'] is None)


def render_call(call: dict, index_name: str, groups: dict[int, str]) -> str:
    flags = CONSOLE if call['flags'] is None else flag_expression(call['flags'], FLAGS)
    if call['path'] is None:
        raise ValueError(f'new call without a decoded path: {call}')
    text = (f'sLookupSNO({flag_expression(call["mask"], PLATFORMS)}, {BUCKETS[call["bucket"]]}, '
            f'{index_name}, {groups[call["group"]]}, {json.dumps(call["path"])}, {flags})')
    return f'    if ({text}) nExtrasIndex++;' if call['index'] is None else f'    {text};'


def extras_normalized(calls: list) -> list:
    """Mark the first extras call: the compiler folds nExtrasIndex's start (NUM_GLOBAL_SNOS)
    into it, so it is the call right before the first one with a variable index."""
    first = next(i for i, c in enumerate(calls) if c['index'] is None)
    return [dict(c, index=None) if i == first - 1 else c for i, c in enumerate(calls)]


def sno_calls(old_calls: list, old_lines: list[str], new_calls: list, sno_names: dict[int, str],
              groups: dict[int, str]) -> tuple[str, int, int]:
    """(include text, reused lines, new lines). Calls are already extras_normalized."""
    if len(old_calls) != len(old_lines):
        raise ValueError(f'template has {len(old_lines)} calls, 2.6.2 code has {len(old_calls)}')
    pool: dict[tuple, deque] = defaultdict(deque)
    old_extras = deque()
    for call, line in zip(old_calls, old_lines):
        (old_extras if call['index'] is None else pool[_key(call)]).append(line)
    new_extras = sum(1 for c in new_calls if c['index'] is None)
    if new_extras != len(old_extras):
        raise ValueError(f'extras block changed size ({len(old_extras)} -> {new_extras}); pair by hand')
    out, reused, added = [], 0, 0
    for call in new_calls:
        if call['index'] is None:
            if DECL not in out:
                out.append(DECL)
            out.append(old_extras.popleft())
            reused += 1
            continue
        name = sno_names[call['index']]
        bucket = pool.get(_key(call))
        if bucket:
            line = bucket.popleft()
            line = re.sub(r'(BUCKET_\w+, )GLOBALSNO_\w+', lambda m: m[1] + name, line, count=1)
            reused += 1
        else:
            line = render_call(call, name, groups)
            added += 1
        out.append(line)
    return '\n'.join(out) + '\n', reused, added


def gbid_rows(rows: list[tuple[int, str, int]], gbid_names: dict[int, str]) -> str:
    return '\n'.join(f'    {{ {gbid_names[i]}, {json.dumps(name)}, {GB_TYPES[kind]} }},'
                     for i, name, kind in rows) + '\n'


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__.split('\n\n')[0])
    ap.add_argument('--version', default='2.7.7')
    ap.add_argument('--apply', action='store_true')
    args = ap.parse_args(argv)
    if not TEMPLATE.is_file():
        TEMPLATE.parent.mkdir(parents=True, exist_ok=True)
        installed = (OUT / 'sno_calls.inc').read_text()
        if installed.count('sLookupSNO(') != len(gsm.lookup_calls('2.6.2')):
            raise SystemExit(f'{TEMPLATE} missing and the installed sno_calls.inc is not 2.6.2\'s')
        shutil.copyfile(OUT / 'sno_calls.inc', TEMPLATE)
    header = gsm.HEADER.read_text()
    sno_names = enum_names(header, r'GLOBALSNO_(?!FLAG_)\w+')
    gbid_names = enum_names(header, r'GLOBALGBID_\w+')
    groups = enum_names((ROOT / 'src/X/Common/SNO/SNOFiles.h').read_text(), r'(?!SNOENTRY_)[A-Z0-9_]+_SNO')
    old_lines = [l for l in TEMPLATE.read_text().split('\n') if 'sLookupSNO(' in l]
    old_calls = extras_normalized(gsm.lookup_calls('2.6.2'))
    new_calls = extras_normalized(gsm.lookup_calls(args.version))
    text, reused, added = sno_calls(old_calls, old_lines, new_calls, sno_names, groups)
    rows = gsm.gbid_rows(args.version)
    print(f'sno_calls.inc: {reused + added} calls ({reused} from 2.6.2, {added} new); '
          f'gbid_rows.inc: {len(rows)} rows')
    if args.apply:
        (OUT / 'sno_calls.inc').write_text(text)
        (OUT / 'gbid_rows.inc').write_text(gbid_rows(rows, gbid_names))
        print('written')
    return 0


if __name__ == '__main__':
    sys.exit(main())
