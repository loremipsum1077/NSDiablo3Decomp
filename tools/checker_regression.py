#!/usr/bin/env python3
"""Check every non-U inventory row in an isolated snapshot; never update live CSVs."""
import argparse
from collections import Counter
import csv
import hashlib
import json
from pathlib import Path
import shutil
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]


def sha256(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--checker', type=Path, default=ROOT / 'tools/check')
    parser.add_argument('--output', type=Path, required=True, help='Evidence JSON destination')
    parser.add_argument('--build-dir', type=Path, default=ROOT / 'build/2.6.2',
                        help='existing linked output directory; copied into an isolated checker snapshot')
    args = parser.parse_args()
    checker = args.checker.resolve()
    build_dir = args.build_dir.resolve()
    paths = ['tools/config.toml', 'data/2.6.2/functions.csv',
             'data/2.6.2/data_symbols.csv', 'data/2.6.2/main.elf',
             'build/2.6.2/d3', 'build/2.6.2/d3.map']
    # Copy only pre-existing link evidence. Stamping the snapshot here would
    # incorrectly bless a potentially stale ELF/map pair.
    provenance = 'build/2.6.2/d3.map.provenance'
    if (build_dir / 'd3.map.provenance').is_file():
        paths.append(provenance)
    sources = {p: (build_dir / Path(p).name if p.startswith('build/2.6.2/') else ROOT / p)
               for p in paths}
    with tempfile.TemporaryDirectory(prefix='d3-checker-regression-') as temp:
        snapshot = Path(temp)
        (snapshot / 'src').mkdir()
        for relative in paths:
            target = snapshot / relative
            target.parent.mkdir(parents=True, exist_ok=True)
            shutil.copy2(sources[relative], target)
        shutil.copy2(checker, snapshot / 'tools/check')
        hashes = {p: sha256(snapshot / p) for p in paths}
        with (snapshot / 'data/2.6.2/functions.csv').open() as stream:
            rows = list(csv.DictReader(stream))
        selected = [r for r in rows if r['Quality'] != 'U']
        request = {'functions': [{'address': r['Address'], 'name': r['Name']} for r in selected]}
        (snapshot / 'request.json').write_text(json.dumps(request))
        run = subprocess.run([str(snapshot / 'tools/check'), '--batch', 'request.json',
                              '--batch-output', 'result.json'], cwd=snapshot,
                             capture_output=True, text=True)
        if run.returncode or not (snapshot / 'result.json').exists():
            raise RuntimeError(f'Checker failed ({run.returncode}): {run.stdout}\n{run.stderr}')
        response = json.loads((snapshot / 'result.json').read_text())
        results = response['results']
        if response['format'] != 1 or [(r['address'], r['name']) for r in results] != [
                (r['Address'], r['Name']) for r in selected]:
            raise RuntimeError('Checker returned incomplete or different identities')
        failures = [result for row, result in zip(selected, results)
                    if (row['Quality'] == 'O' and result['status'] != 'ok')
                    or result['status'] not in ('ok', 'mismatch')]
        verified_exact = [row for row, result in zip(selected, results)
                          if row['Quality'] == 'O' and result['status'] == 'ok']
        unverified_exact = [row for row, result in zip(selected, results)
                            if row['Quality'] == 'O' and result['status'] != 'ok']
        changed = [p for p in paths if sha256(sources[p]) != hashes[p]]
        if changed:
            raise RuntimeError(f'Inputs changed while checking; rerun after writer finishes: {changed}')
        report = {
            'revision': subprocess.check_output(['git', 'rev-parse', 'HEAD'], cwd=ROOT, text=True).strip(),
            'source_diff': subprocess.check_output(['git', 'diff', 'HEAD', '--', 'src', 'include',
                                                    'CMakeLists.txt', 'toolchain'], cwd=ROOT, text=True),
            'inputs_sha256': hashes,
            'checker_sha256': sha256(snapshot / 'tools/check'),
            'patches_sha256': {p.name: sha256(p) for p in sorted((ROOT / 'tools/patches').glob('*.patch'))},
            'checked': len(results),
            'status_counts': dict(Counter(r['status'] for r in results)),
            'strict_O_count': sum(r['Quality'] == 'O' for r in rows),
            'strict_O_bytes': sum(int(r['Size']) for r in rows if r['Quality'] == 'O'),
            # Inventory labels are inputs, not proof that this checker accepted them.
            'verified_O_count': len(verified_exact),
            'verified_O_bytes': sum(int(r['Size']) for r in verified_exact),
            'unverified_O_count': len(unverified_exact),
            'unverified_O_bytes': sum(int(r['Size']) for r in unverified_exact),
            'failures': failures,
            'note': 'Existing built ELF; no fresh source build. Live inventory unchanged.',
        }
        args.output.parent.mkdir(parents=True, exist_ok=True)
        args.output.write_text(json.dumps(report, indent=2) + '\n')
        print(f"Checked {len(results)} rows: {report['status_counts']}; failures: {len(failures)}")
        return int(bool(failures))


if __name__ == '__main__':
    raise SystemExit(main())
