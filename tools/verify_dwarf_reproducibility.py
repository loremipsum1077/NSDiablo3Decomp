#!/usr/bin/env python3
"""Rebuild DWARF independently and compare generated artifacts (no live cache writes).

Run with the private NSS available, for example:
  python3 tools/verify_dwarf_reproducibility.py --workers 1 3 --output build/dwarf-proof
The output directory must not exist. This is generation evidence, not a game check.
"""
import argparse
import hashlib
import json
import os
from pathlib import Path
import pickle
import shutil
import subprocess
import sys

ROOT = Path(__file__).resolve().parent.parent


def sha(path):
    h = hashlib.sha256()
    with Path(path).open('rb') as stream:
        for chunk in iter(lambda: stream.read(1024 * 1024), b''):
            h.update(chunk)
    return h.hexdigest()


def header_manifest(root):
    return {p.relative_to(root).as_posix(): sha(p)
            for p in sorted(root.rglob('*')) if p.is_file()}


def render(cache, output):
    import dwarf_index
    import gen_headers
    import helper_ledger
    import inline_rank
    sys.setrecursionlimit(10000)
    with cache.open('rb') as stream:
        index = pickle.load(stream)
    # Exercise the public generator entry point with the explicitly supplied index.
    dwarf_index.load = lambda: index
    sys.argv = ['inline_rank.py', '--out', str(output / 'inline_helpers.csv'), '--top', '0']
    inline_rank.main()
    gen_headers.Generator(index).run(output / 'headers')
    helper_ledger.write(helper_ledger.build(index=index), output / 'helper_ledger.csv')
    manifest = header_manifest(output / 'headers')
    (output / 'headers.json').write_text(json.dumps(manifest, indent=2, sort_keys=True) + '\n')
    counts = {key: len(index[key]) for key in ('types', 'funcs', 'vars', 'inlined', 'units')}
    (output / 'counts.json').write_text(json.dumps(counts, indent=2, sort_keys=True) + '\n')


def compare(first, second):
    differences = {}
    for name in ('inline_helpers.csv', 'helper_ledger.csv', 'counts.json'):
        if sha(first / name) != sha(second / name):
            differences[name] = {'first': sha(first / name), 'second': sha(second / name)}
    manifests = []
    for label, directory in (('first', first), ('second', second)):
        recorded = json.loads((directory / 'headers.json').read_text())
        actual = header_manifest(directory / 'headers')
        if not (directory / 'headers').is_dir() or recorded != actual:
            differences[f'{label}_header_manifest'] = {
                'error': 'Recorded header manifest does not match generated files'}
        manifests.append(actual)
    a, b = manifests
    changes = [name for name in sorted(a.keys() | b.keys()) if a.get(name) != b.get(name)]
    if changes:
        differences['headers'] = changes
    return differences


def run(args):
    output = args.output.resolve()
    output.mkdir(parents=True, exist_ok=False)
    nss = ROOT / 'build/romfs/DiabloIIINX64r.nss'
    llvm = shutil.which('llvm-dwarfdump') or '/opt/homebrew/opt/llvm/bin/llvm-dwarfdump'
    report = {'schema': 1, 'purpose': 'Uncached DWARF artifact determinism; no game validation',
              'runs': [], 'passed': False}
    try:
        report.update({
            'revision': subprocess.check_output(['git', 'rev-parse', 'HEAD'], cwd=ROOT, text=True).strip(),
            'source_sha256': {name: sha(ROOT / 'tools' / name) for name in
                              ('dwarf_index.py', 'gen_headers.py', 'inline_rank.py',
                               'helper_ledger.py', 'verify_dwarf_reproducibility.py')},
            'python': sys.version, 'nss_sha256': sha(nss),
            'llvm_dwarfdump': {'path': llvm, 'sha256': sha(llvm),
                              'version': subprocess.check_output([llvm, '--version'], text=True).strip()},
        })
        for number, workers in enumerate(args.workers):
            target = output / f'run-{number + 1}-workers-{workers}'
            target.mkdir()
            env = dict(os.environ, PYTHONHASHSEED=str(number + 1))
            record = {'workers': workers, 'hash_seed': number + 1, 'directory': target.name, 'stages': []}
            report['runs'].append(record)
            commands = [
                [sys.executable, str(ROOT / 'tools/dwarf_index.py'), '--workers', str(workers),
                 '--cache', str(target / 'index.pkl')],
                [sys.executable, str(Path(__file__).resolve()), '--render', str(target / 'index.pkl'),
                 '--output', str(target)],
            ]
            for stage, command in zip(('index', 'render'), commands):
                print(f'>>> {target.name}: {stage}', flush=True)
                log = target / f'{stage}.log'
                with log.open('w') as stream:
                    result = subprocess.run(command, cwd=ROOT, env=env, stdout=stream, stderr=subprocess.STDOUT)
                record['stages'].append({'command': command, 'exit_code': result.returncode,
                                         'log': str(log.relative_to(output)), 'log_sha256': sha(log)})
                if result.returncode:
                    raise RuntimeError(f'{stage} failed; see {log}')
            record['artifacts'] = {name: sha(target / name) for name in
                                   ('inline_helpers.csv', 'helper_ledger.csv', 'headers.json', 'counts.json')}
            record['counts'] = json.loads((target / 'counts.json').read_text())
            record['header_count'] = len(json.loads((target / 'headers.json').read_text()))
        first = output / report['runs'][0]['directory']
        report['differences'] = {r['directory']: compare(first, output / r['directory'])
                                 for r in report['runs'][1:]}
        report['passed'] = all(not d for d in report['differences'].values())
    except Exception as error:
        report['error'] = str(error)
    finally:
        (output / 'report.json').write_text(json.dumps(report, indent=2, sort_keys=True) + '\n')
    print(f'>>> {"PASS" if report["passed"] else "FAIL"}: {output / "report.json"}')
    return 0 if report['passed'] else 1


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--workers', type=int, nargs='+', default=[1, 3])
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--render', type=Path, help=argparse.SUPPRESS)
    args = parser.parse_args()
    if args.render:
        render(args.render, args.output)
        return 0
    if len(args.workers) < 2 or any(n < 1 for n in args.workers):
        parser.error('supply at least two positive worker counts')
    return run(args)


if __name__ == '__main__':
    sys.exit(main())
