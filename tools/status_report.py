#!/usr/bin/env python3
"""Read-only status snapshot; inventory labels and retained receipts are not acceptance."""
import argparse
from collections import Counter
from datetime import datetime, timezone
import hashlib
import json
from pathlib import Path
import subprocess

import project_plan

ROOT = Path(__file__).resolve().parents[1]
LIMITATIONS = [
    'O labels are retained function-matching claims, not a fresh verification.',
    'Byte totals cover original function sizes only; they do not measure complete executable or asset equivalence.',
    'Acceptance still requires independent source review, relocation/data targets, emitted extras, unwind and normal/Lua linked checks.',
    'This snapshot runs no build or checker and does not establish a runnable game.',
]


def totals(rows):
    identities = [(r['Address'], r['Name']) for r in rows]
    if len(set(identities)) != len(identities):
        raise ValueError('Duplicate function identity')
    if any(r['Quality'] not in {'O', 'U', 'M', 'm', 'W', 'L'} or int(r['Size']) < 0 for r in rows):
        raise ValueError('Invalid function inventory')
    # L = library code excluded from the project (docs/library_functions.md):
    # reported, but not part of the denominators.
    library = [r for r in rows if r['Quality'] == 'L']
    rows = [r for r in rows if r['Quality'] != 'L']
    matched = [r for r in rows if r['Quality'] == 'O']
    count, size = len(rows), sum(int(r['Size']) for r in rows)
    exact_size = sum(int(r['Size']) for r in matched)
    return {'functions': count, 'matching_functions': len(matched),
            'function_percent': 100 * len(matched) / count if count else 0,
            'original_function_bytes': size, 'matching_function_bytes': exact_size,
            'byte_percent': 100 * exact_size / size if size else 0,
            'excluded_library_functions': len(library),
            'excluded_library_bytes': sum(int(r['Size']) for r in library),
            'qualities': dict(Counter(r['Quality'] for r in rows + library))}


def excluded_by_category(root, revision, rows):
    """L rows by reason. config/excluded_functions.csv names the category and reason of
    every L row that is not closed-source library code; unlisted L rows are 'library'."""
    import csv
    import io
    import subprocess
    listed = {}
    # Read at the same revision as the inventory, so an uncommitted edit cannot disagree with it.
    shown = subprocess.run(['git', 'show', f'{revision}:config/excluded_functions.csv'],
                           cwd=root, capture_output=True, text=True)
    if shown.returncode == 0:
        for r in csv.DictReader(io.StringIO(shown.stdout, newline='')):
            if r['Category'] not in {'sdk-runtime', 'skipped'} or not r['Reason'].strip():
                raise ValueError(f"Invalid exclusion entry: {r['Address']}")
            listed[r['Address']] = r['Category']
    labels = {r['Address']: r['Quality'] for r in rows}
    wrong = sorted(a for a in listed if labels.get(a) != 'L')
    if wrong:
        raise ValueError(f'excluded_functions.csv lists functions not labelled L: {wrong[:5]}')
    out = {}
    for r in rows:
        if r['Quality'] == 'L':
            c = out.setdefault(listed.get(r['Address'], 'library'), {'functions': 0, 'bytes': 0})
            c['functions'] += 1
            c['bytes'] += int(r['Size'])
    return out


def snapshot(root=ROOT, receipt_root=None):
    root = Path(root).resolve()
    revision = project_plan.git(root, 'rev-parse', 'HEAD').strip()
    rows, _ = project_plan.inventory(str(root), revision)
    normal = {'available': True, 'basis': 'Committed inventory O labels; not a mode-specific linked verification',
              'revision': revision, 'metrics': totals(rows)}
    normal['metrics']['excluded_by_category'] = excluded_by_category(root, revision, rows)
    modes = {'normal': normal, 'lua': {'available': False,
             'basis': 'No separate Lua-mode inventory or verified receipt bundle supplied', 'metrics': None}}
    verification = {'status': 'not_supplied', 'source_relation': 'not_assessed',
                    'accepted': False, 'fresh_build_executed': False}
    if receipt_root is not None:
        import csv
        import io
        import receipts
        from validation.common import Halt
        receipt_root = Path(receipt_root).resolve()
        control = ('binding.json', 'run.json', 'evidence.json')
        pins = {name: receipts.c.sha(receipts.regular(receipt_root / name)) for name in control}
        # Existing trusted verifier checks complete source, artifact and mode bindings.
        receipts.verify(receipt_root)
        def pinned_json(name):
            data = receipts.regular(receipt_root / name).read_bytes()
            if hashlib.sha256(data).hexdigest() != pins[name]:
                raise Halt('Receipt control metadata changed during status snapshot')
            return json.loads(data)
        binding = pinned_json('binding.json')
        state = pinned_json('run.json')
        if any(receipts.c.sha(receipt_root / name) != digest for name, digest in pins.items()):
            raise Halt('Receipt control metadata changed during status snapshot')
        candidate = binding['versions']['candidate']['commit']
        for key, selected in [('source', receipts.SOURCE), ('support', receipts.SUPPORT)]:
            if receipts.committed_files(root, candidate, selected) != binding['versions']['candidate'][key]:
                raise Halt('Receipt candidate differs from committed core source')
        for mode in ('normal', 'lua'):
            entry = state['modes']['candidate_' + mode]
            output = receipts.inside(receipt_root, entry['root'])
            path = receipts.regular(output / 'data/2.6.2/functions.csv')
            # Never aggregate an unsealed inventory, even if some other artifacts verify.
            done_path = receipts.regular(output / 'done.json')
            done_bytes = done_path.read_bytes()
            if hashlib.sha256(done_bytes).hexdigest() != entry['receipt_sha256']:
                raise Halt('Retained mode receipt changed during status snapshot')
            done = json.loads(done_bytes)
            inventory_bytes = path.read_bytes()
            if done['files'].get('data/2.6.2/functions.csv') != hashlib.sha256(inventory_bytes).hexdigest():
                raise Halt('Mode inventory is not sealed by its receipt')
            mode_rows = list(csv.DictReader(io.StringIO(inventory_bytes.decode())))
            expected = binding['versions']['candidate']['support']['data/2.6.2/functions.csv']
            source = receipts.inside(output.parent, 'support/data/2.6.2/functions.csv')
            source_bytes = source.read_bytes()
            if hashlib.sha256(source_bytes).hexdigest() != expected['sha256']:
                raise Halt('Candidate inventory source binding changed')
            source_rows = list(csv.DictReader(io.StringIO(source_bytes.decode())))
            identity = lambda rs: sorted((r['Address'], r['Name'], int(r['Size'])) for r in rs)
            if identity(mode_rows) != identity(source_rows):
                raise Halt('Mode inventory membership differs from source')
            modes[mode] = {'available': True, 'basis': 'Retained candidate inventory O labels from a verified receipt bundle',
                           'revision': candidate, 'metrics': totals(mode_rows)}
        verification.update(status='retained_bytes_verified',
            source_relation='same_revision' if candidate == revision else 'different_revision',
            receipt_binding_sha256=state['binding_sha256'], evidence_revision=candidate)
        receipts.verify(receipt_root)
        if any(receipts.c.sha(receipt_root / name) != digest for name, digest in pins.items()):
            raise Halt('Receipt control metadata changed during status snapshot')
    if project_plan.git(root, 'rev-parse', 'HEAD').strip() != revision:
        raise ValueError('Core revision changed during status snapshot')
    return {'format': 1, 'kind': 'overall_status_snapshot', 'game_version': '2.6.2',
            'revision': revision, 'generated_at': datetime.now(timezone.utc).isoformat(),
            'inventory_blob': project_plan.git(root, 'rev-parse', revision + ':data/2.6.2/functions.csv').strip(),
            'modes': modes, 'verification': verification, 'limitations': LIMITATIONS,
            'live': False, 'provider_calls': 0}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--repo', type=Path, default=ROOT)
    parser.add_argument('--format', type=int, choices=[1], default=1)
    parser.add_argument('--receipts', type=Path, help='Explicit private retained bundle; verifies bytes only, never builds')
    args = parser.parse_args()
    try:
        print(json.dumps(snapshot(args.repo, args.receipts), indent=2))
    except (OSError, ValueError, RuntimeError, KeyError, TypeError, subprocess.CalledProcessError) as error:
        parser.exit(2, str(error) + '\n')


if __name__ == '__main__':
    main()
