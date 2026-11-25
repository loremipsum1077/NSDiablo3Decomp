#!/usr/bin/env python3
"""Find the functions that use given strings, with their callers and callees.

A string that exists only in the new build (docs/changes_2_6_2_to_2_7_7.md lists
some) is a way into its new code: the functions that load its address, and the
functions around them, are where the feature lives. For each string this finds:

  occurrences   every place the bytes appear outside executable code, as file
                offset and virtual address. An occurrence inside a longer
                string (the linker shares string tails) names that string.
  pointers      data slots that hold the string's address (.rela.dyn
                RELATIVE relocations): string tables, registration arrays.
  references    functions whose code computes the string's address (ADRP +
                ADD/LDR, ADR, literal loads, loads through GOT slots) or the
                address of a pointer slot, with their name and Quality from
                functions.csv, callers and callees. A slot no code names
                directly is credited to the functions that reference the
                nearest address below it (within TABLE_REACH): the table's
                start. Those are marked as table leads.

  python3 tools/string_anchors.py                       # the guide's anchors
  python3 tools/string_anchors.py "Primordial Ashes" ConsoleDarkAlchemy
  python3 tools/string_anchors.py --new-strings          # strings not in 2.6.2
  python3 tools/string_anchors.py --owners               # source files for U functions

--new-strings lists every printable string of the new build that the old build
lacks and that code references, ranked by how many unnamed (U) functions use
it (source-path strings differ only by branch name and are ignored). --owners
proposes a source file for each unnamed function, from a source-path string it
references or from link order (the nearest functions on both sides with a known
file agree). Output is Markdown on stdout, or JSON with --json. Feature attributions
remain hypotheses until the code is checked.
"""
from __future__ import annotations

import argparse
import bisect
import csv
import json
import re
import sys
from collections import defaultdict
from pathlib import Path

import port_version
import project_version
import version_diff as vd

ROOT = Path(__file__).resolve().parent.parent

# Strings docs/changes_2_6_2_to_2_7_7.md found only in the 2.7.7 binary.
DEFAULT_ANCHORS = {
    'ConsoleDarkAlchemy': 'Altar of Rites UI',
    'CommunityEventNestingPortals': 'Visions of Enmity',
    'CommunityBuffEtherealItems': 'Ethereal items',
    'ConsoleInventory:AddSoulShardRanksButton': 'Soul Shard upgrades',
    'CommunityBuffSanctifiedItems': 'Sanctified items',
    'Primordial Ashes': 'Primordial Ashes crafting currency',
    'ConsoleHeroCreate:SoloSeasonalHardcoreModalWarningDescription': 'Solo Self Found UI',
}

SOURCE_PATH = re.compile(r'[\\/]branches[\\/][^\\/]+[\\/](.+)$')   # D:/BuildServer/.../branches/2_7_7/<path>
TABLE_REACH = 0x2000     # how far below a pointer slot its table may start
PRINTABLE = re.compile(rb'[\x20-\x7e]{%d,}\x00')


class Image:
    """One ELF with its functions, address references, pointers and call graph."""

    def __init__(self, elf_path: Path, functions_csv: Path | None = None):
        self.elf = vd.ELF(elf_path)
        bounds = vd.eh_functions(self.elf)
        self.starts = sorted(a for a, s in bounds.items() if s and self.elf.is_exec(a))
        self.sizes = {a: bounds[a] & ~3 for a in self.starts}
        self.names: dict[int, tuple[str, str]] = {}      # vaddr -> (quality, name)
        if functions_csv and functions_csv.is_file():
            with functions_csv.open(newline='') as fh:
                for r in csv.DictReader(fh):
                    self.names[int(r['Address'], 16) - vd.CSV_BASE] = (r['Quality'], r['Name'])
        got = port_version.got_targets(self.elf)
        self.refs: dict[int, set[int]] = defaultdict(set)      # target -> functions
        self.callers: dict[int, set[int]] = defaultdict(set)
        self.callees: dict[int, list[int]] = {}
        for a in self.starts:
            words = self.elf.words(a, self.sizes[a])
            for _, target in port_version.address_refs(words, a, got):
                self.refs[target].add(a)
            out = []
            for t in vd.branch_targets(words, a):
                if t in self.sizes and t != a and t not in out:
                    out.append(t)
                    self.callers[t].add(a)
            self.callees[a] = out
        self.ref_targets = sorted(self.refs)
        # Data slots holding an address (RELATIVE relocations only: string tables).
        self.pointers: dict[int, list[int]] = defaultdict(list)   # target -> slots
        _, off, size = self.elf.sections['.rela.dyn']
        for i in range(size // 24):
            r_offset, r_info, r_addend = vd.struct.unpack_from('<QQq', self.elf.data, off + 24 * i)
            if r_info & 0xffffffff == 1027:
                self.pointers[r_addend].append(r_offset)

    # -- addresses -------------------------------------------------------
    def vaddr(self, file_offset: int) -> int | None:
        for start, size, off, _ in self.elf.segments:
            if off <= file_offset < off + size:
                return start + file_offset - off
        return None

    def data_ranges(self):
        """(file offset, size) of every non-executable loaded segment."""
        return [(off, size) for _, size, off, flags in self.elf.segments if not flags & 1]

    def function_at(self, vaddr: int) -> int | None:
        k = bisect.bisect_right(self.starts, vaddr) - 1
        if k >= 0 and vaddr < self.starts[k] + self.sizes[self.starts[k]]:
            return self.starts[k]
        return None

    def label(self, a: int) -> dict:
        q, n = self.names.get(a, ('?', f'sub_{vd.CSV_BASE + a:x}'))
        return {'address': f'{vd.CSV_BASE + a:#018x}', 'quality': q, 'name': n, 'size': self.sizes.get(a, 0)}

    # -- strings ---------------------------------------------------------
    def occurrences(self, text: str):
        """[(file offset, vaddr, containing string)] outside executable code."""
        needle = text.encode()
        d = self.elf.data
        out = []
        for off, size in self.data_ranges():
            pos = d.find(needle, off, off + size)
            while pos != -1:
                start = d.rfind(b'\0', off, pos) + 1 or off
                end = d.find(b'\0', pos)
                whole = d[start:end].decode('latin-1')
                out.append((pos, self.vaddr(pos), whole))
                pos = d.find(needle, pos + 1, off + size)
        return out

    def strings(self, min_length: int) -> dict[str, int]:
        """{printable NUL-terminated string: first vaddr} outside executable code."""
        pattern = re.compile(PRINTABLE.pattern % min_length)
        out = {}
        for off, size in self.data_ranges():
            for m in pattern.finditer(self.elf.data, off, off + size):
                s = m.group()[:-1].decode()
                out.setdefault(s, self.vaddr(m.start()))
        return out

    def table_base(self, slot: int) -> int | None:
        """Highest code-referenced address in [slot - TABLE_REACH, slot): the
        start of the table a pointer slot probably belongs to."""
        k = bisect.bisect_left(self.ref_targets, slot) - 1
        if k >= 0 and slot - self.ref_targets[k] <= TABLE_REACH:
            return self.ref_targets[k]
        return None

    def users(self, vaddr: int) -> tuple[set[int], list[int], dict[int, int]]:
        """(functions referencing vaddr or one of its pointer slots, the slots,
        {function: table base} for slots only reached through a table)."""
        funcs = set(self.refs.get(vaddr, ()))
        slots = self.pointers.get(vaddr, [])
        tables = {}
        for slot in slots:
            direct = self.refs.get(slot, set())
            funcs |= direct
            base = None if direct else self.table_base(slot)
            if base is not None:
                for f in self.refs[base]:
                    tables.setdefault(f, base)
        return funcs, slots, tables


def anchor_report(img: Image, text: str, note: str = '', neighbours: int = 8) -> dict:
    occ = []
    functions: dict[int, list[str]] = defaultdict(list)
    for off, va, whole in img.occurrences(text):
        funcs, slots, tables = img.users(va)
        occ.append({'file_offset': f'{off:#x}', 'vaddr': f'{va:#x}', 'string': whole,
                    'exact': whole == text, 'pointer_slots': [f'{s:#x}' for s in slots],
                    'functions': len(funcs), 'table_functions': len(tables)})
        for f in funcs:
            functions[f].append(f'{va:#x}')
        for f, base in tables.items():
            if f not in funcs:
                functions[f].append(f'table at {base:#x}')
    rows = []
    for f in sorted(functions):
        row = img.label(f)
        row['via'] = functions[f]
        row['callers'] = [img.label(c) for c in sorted(img.callers.get(f, ()))[:neighbours]]
        row['caller_count'] = len(img.callers.get(f, ()))
        row['callees'] = [img.label(c) for c in img.callees.get(f, [])[:neighbours]]
        row['callee_count'] = len(img.callees.get(f, []))
        rows.append(row)
    return {'string': text, 'note': note, 'occurrences': occ, 'functions': rows}


def _unbranched(s: str) -> str:
    """Source-path strings carry the build server and branch (.../BuildServer/2/
    .../branches/2_6_2/...): compare only the path below the branch."""
    m = SOURCE_PATH.search(s)
    return 'source:' + m.group(1).replace('\\', '/') if m else s


def new_strings(img: Image, old: Image, min_length: int) -> list[dict]:
    old_set = {_unbranched(s) for s in old.strings(min_length)}
    out = []
    for s, va in img.strings(min_length).items():
        if _unbranched(s) in old_set:
            continue
        funcs, _, _ = img.users(va)
        if not funcs:
            continue
        labels = [img.label(f) for f in sorted(funcs)]
        out.append({'string': s, 'vaddr': f'{va:#x}', 'functions': labels,
                    'unnamed': sum(l['quality'] == 'U' for l in labels)})
    out.sort(key=lambda r: (-r['unnamed'], -len(r['functions']), r['string']))
    return out


def owners(img: Image, files_csv: Path) -> list[dict]:
    """Proposed source file for each unnamed (U) function.

    path-string  the function references a build-server source path string
                 (asserts, logging) naming exactly one file.
    neighbours   link order keeps a compile unit's functions together: the
                 nearest functions on both sides that have a file agree.
    """
    files: dict[int, str] = {}
    if files_csv.is_file():
        with files_csv.open(newline='') as fh:
            for r in csv.DictReader(fh):
                if r['File']:
                    files[int(r['Address'], 16) - vd.CSV_BASE] = r['File']
    by_path: dict[int, set[str]] = defaultdict(set)
    for s, va in img.strings(6).items():
        m = SOURCE_PATH.search(s)
        if m:
            path = m.group(1).replace('\\', '/')
            for f in img.users(va)[0]:
                by_path[f].add(path)
    out = []
    starts = img.starts
    for i, a in enumerate(starts):
        if img.names.get(a, ('?', ''))[0] != 'U':
            continue
        row = img.label(a)
        paths = by_path.get(a, set())
        if len(paths) == 1:
            row.update(file=next(iter(paths)), evidence='path-string')
        else:
            before = next((files[starts[j]] for j in range(i - 1, -1, -1) if starts[j] in files), None)
            after = next((files[starts[j]] for j in range(i + 1, len(starts)) if starts[j] in files), None)
            if before is None or before != after:
                continue
            row.update(file=before, evidence='neighbours')
        out.append(row)
    return out


def _fn(l: dict) -> str:
    return f"`{l['address']}` {l['quality']} `{l['name']}`"


def markdown_anchors(reports: list[dict]) -> str:
    lines = []
    for r in reports:
        lines.append(f"## `{r['string']}`" + (f" ({r['note']})" if r['note'] else ''))
        if not r['occurrences']:
            lines += ['', 'Not found in this ELF.', '']
            continue
        lines += ['', '| file offset | vaddr | in string | pointer slots | functions | via a table |',
                  '|---|---|---|---|---:|---:|']
        for o in r['occurrences']:
            inside = 'exact' if o['exact'] else f"`{o['string'][:80]}`"
            lines.append(f"| {o['file_offset']} | {o['vaddr']} | {inside} | "
                         f"{', '.join(o['pointer_slots']) or '-'} | {o['functions']} | {o['table_functions']} |")
        lines.append('')
        if not r['functions']:
            lines += ['No code references found (it may be reached through a table of offsets).', '']
        for f in r['functions']:
            tables = [v for v in f['via'] if v.startswith('table')]
            lines.append(f"- {_fn(f)}, {f['size']} bytes" + (f" (reads the {tables[0]}; a lead, not proof)" if tables else ''))
            if f['callers']:
                more = f" (+{f['caller_count'] - len(f['callers'])})" if f['caller_count'] > len(f['callers']) else ''
                lines.append('  - called by: ' + '; '.join(_fn(c) for c in f['callers']) + more)
            if f['callees']:
                more = f" (+{f['callee_count'] - len(f['callees'])})" if f['callee_count'] > len(f['callees']) else ''
                lines.append('  - calls: ' + '; '.join(_fn(c) for c in f['callees']) + more)
        lines.append('')
    return '\n'.join(lines)


def markdown_new(rows: list[dict], limit: int) -> str:
    lines = [f'{len(rows)} new strings are referenced by code.', '',
             '| string | unnamed users | users | first users |', '|---|---:|---:|---|']
    for r in rows[:limit]:
        s = r['string'].replace('|', '\\|')[:90]
        first = '; '.join(f"{l['quality']} `{l['name']}`" for l in r['functions'][:3])
        lines.append(f"| `{s}` | {r['unnamed']} | {len(r['functions'])} | {first} |")
    return '\n'.join(lines) + '\n'


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__.split('\n\n')[0])
    ap.add_argument('strings', nargs='*', help='strings to look up (default: the guide anchors)')
    ap.add_argument('--version', default=project_version.VERSION)
    ap.add_argument('--old-version', default='2.6.2')
    ap.add_argument('--elf', type=Path)
    ap.add_argument('--functions', type=Path)
    ap.add_argument('--new-strings', action='store_true', help='list strings absent from the old build')
    ap.add_argument('--owners', action='store_true', help='propose source files for unnamed functions')
    ap.add_argument('--min-length', type=int, default=6)
    ap.add_argument('--limit', type=int, default=200, help='rows of --new-strings Markdown output')
    ap.add_argument('--neighbours', type=int, default=8, help='callers/callees listed per function')
    ap.add_argument('--json', action='store_true')
    args = ap.parse_args(argv)

    data = ROOT / 'data' / args.version
    elf = args.elf or data / 'main.elf'
    if not elf.is_file():
        print(f'missing input: {elf}', file=sys.stderr)
        return 2
    img = Image(elf, args.functions or data / 'functions.csv')

    if args.owners:
        rows = owners(img, data / 'function_files.csv')
        if args.json:
            print(json.dumps(rows, indent=1))
        else:
            ev = defaultdict(int)
            for r in rows:
                ev[r['evidence']] += 1
            print(f"{len(rows)} unnamed functions placed ({', '.join(f'{k} {v}' for k, v in sorted(ev.items()))}).\n")
            print('| address | size | proposed file | evidence |\n|---|---:|---|---|')
            for r in rows:
                print(f"| `{r['address']}` | {r['size']} | `{r['file']}` | {r['evidence']} |")
        return 0

    if args.new_strings:
        old_elf = ROOT / 'data' / args.old_version / 'main.elf'
        if not old_elf.is_file():
            print(f'missing input: {old_elf}', file=sys.stderr)
            return 2
        rows = new_strings(img, Image(old_elf), args.min_length)
        print(json.dumps(rows, indent=1) if args.json else markdown_new(rows, args.limit))
        return 0

    anchors = {s: '' for s in args.strings} if args.strings else DEFAULT_ANCHORS
    reports = [anchor_report(img, s, note, args.neighbours) for s, note in anchors.items()]
    print(json.dumps(reports, indent=1) if args.json else markdown_anchors(reports))
    return 0


if __name__ == '__main__':
    sys.exit(main())
