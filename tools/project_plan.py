#!/usr/bin/env python3
"""Read-only, exhaustive project plan from committed inventories and curated job definitions."""
import argparse
import csv
import hashlib
import io
import json
import subprocess
from collections import Counter, defaultdict
from functools import lru_cache
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
CONFIG = 'config/project_jobs.json'
METRIC_EVIDENCE = (
    'O is a retained matching-quality claim for a function. Counts use committed normal inventory O labels and original function sizes; no checker is run. '
    'Inventory labels are not fresh verification. Object matches are provisional until '
    'relocation, data, unwind, source review and fresh normal/Lua linked checks pass. '
    'These totals do not prove a fully linked byte-identical rebuilt executable or a runnable game.'
)


def actionable(job):
    """Queue eligibility only; this does not assert dependency readiness."""
    return job['status'] != 'done' and not job.get('deferred')


def job_order(job):
    group = 2 if job['status'] == 'done' else 1 if job.get('deferred') else 0
    return group, job['id']


def git(root, *args):
    return subprocess.check_output(['git', *args], cwd=root, text=True)


def job_id(path):
    return 'SRC-' + hashlib.sha256(path.encode()).hexdigest()[:12]


def area(path):
    if not path:
        return 'Unattributed code'
    parts = path.split('/')
    return '/'.join(parts[:3]) if len(parts) > 3 else '/'.join(parts[:-1])


def source_job(path, functions, config):
    counts = Counter(f['Quality'] for f in functions)
    pending = [f for f in functions if f['Quality'] != 'O']
    remaining_bytes = sum(int(f['Size']) for f in pending)
    largest = max((int(f['Size']) for f in pending), default=0)
    nonmatches = sum(counts[q] for q in ('M', 'm', 'W'))
    dependencies = []
    for rule in config['source_rules']:
        if path.startswith(rule['prefix']):
            dependencies.extend(rule.get('depends_on', []))
    return dict(id=job_id(path), kind='source', title=path, area=area(path), file=path,
                status='done' if not pending else 'partial' if counts['O'] else 'todo',
                depends_on=sorted(set(dependencies)),
                functions=len(functions), exact=counts['O'], nonmatches=nonmatches,
                remaining=len(pending), bytes=sum(int(f['Size']) for f in functions),
                remaining_bytes=remaining_bytes, largest=largest,
                acceptance='Every original function assigned to this owner is O after source review and the full relocation-aware checker; no prior O regression. Nonmatches stay open.',
                scope='Reconstruct the remaining functions in the original owner; preserve existing exact implementations. Large jobs must be split into named function batches before implementation.',
                evidence=['data/2.6.2/functions.csv', 'data/2.6.2/function_files.csv', 'docs/headers.md'],
                remaining_functions=[dict(address=f['Address'], name=f['Name'], quality=f['Quality'], bytes=int(f['Size'])) for f in pending])


@lru_cache(maxsize=2)
def inventory(root, revision):
    def read(name):
        return list(csv.DictReader(io.StringIO(git(root, 'show', f'{revision}:data/2.6.2/{name}.csv'))))
    return read('functions'), read('function_files')


def references(root, values):
    promoted = {'docs/project_jobs.json': CONFIG,
                'docs/p03_evidence/component_rules.json': 'config/component_rules.json',
                'docs/p19_f02_evidence/reconciliation.json': 'evidence/checker/reconciliation.json'}
    resolved = (promoted.get(value, value) for value in values)
    return [value for value in resolved if (Path(root) / value).exists()]


def load_config(root):
    config = json.loads((Path(root) / CONFIG).read_text())
    config.pop('estimates', None)
    for row in config['source_rules'] + config['jobs']:
        row.pop('priority', None)
        row.pop('effort', None)
    for job in config['jobs']:
        if 'evidence' in job:
            job['evidence'] = references(root, job['evidence'])
    return config


def snapshot(root=ROOT):
    root = Path(root)
    revision = git(root, 'rev-parse', 'HEAD').strip()
    functions, owners = inventory(str(root), revision)
    config = load_config(root)
    owner_of = {int(r['Address'], 16): r['File'] for r in owners}
    grouped = defaultdict(list)
    for f in functions:
        grouped[owner_of.get(int(f['Address'], 16), '')].append(f)
    # Recompute classification against these same committed rows. A retained
    # binary audit may refer to an older revision and must not supply live totals.
    attribution = {}
    rules_path = root / 'config/component_rules.json'
    if rules_path.is_file():
        import attribution_audit
        audit = attribution_audit.reconcile(functions, owners, json.loads(rules_path.read_text()))
        for record in audit['records']:
            attribution.setdefault(record['bin_id'], []).append(record)
    jobs = []
    for path, rows in sorted(grouped.items()):
        if path:
            jobs.append(source_job(path, rows, config))
        else:
            # Fixed address buckets keep IDs stable as functions become exact or acquire owners.
            buckets = defaultdict(list)
            for row in rows:
                buckets[int(row['Address'], 16) // 0x10000].append(row)
            for bucket, batch in sorted(buckets.items()):
                j = source_job('', batch, config)
                j.update(id=f'BIN-{bucket:x}', title=f'Unattributed code at 0x{bucket * 0x10000:x}',
                         kind='binary', depends_on=['P03'],
                         status='partial' if j['exact'] else 'todo',
                         scope='Attribute every function in this fixed 64 KiB address range to an original owner or documented SDK/runtime component, then reconstruct or explicitly disposition it. Preserve inventory addresses; use generation tools for inventory changes.',
                         acceptance='Every function is attributed and tracked under an owner, or individually dispositioned under the approved scope policy. Exact reconstruction is tracked separately from exclusions.')
                if j['id'] in attribution:
                    j['attribution_records'] = attribution[j['id']]
                    counts = Counter(r['component'] for r in j['attribution_records'])
                    j['component_summary'] = dict(sorted(counts.items()))
                    j['evidence'] += references(root, ['docs/p03_evidence/research.md',
                                      'docs/p03_evidence/component_rules.json',
                                      'docs/p03_evidence/audit.json'])
                jobs.append(j)
    source_jobs = jobs[:]
    for item in config['jobs']:
        if 'deferred' in item and (not isinstance(item['deferred'], dict) or not all(
                isinstance(item['deferred'].get(k), str) and item['deferred'][k].strip()
                for k in ('reason', 'revisit_when', 'do_not_repeat'))):
            raise ValueError(f"Deferred job {item['id']} needs reason, revisit_when and do_not_repeat")
        jobs.append({'kind':'milestone', 'confidence':'medium', 'area':'Project milestones', 'depends_on':[], 'evidence':[], **item})
    jobs.sort(key=job_order)
    by_area = defaultdict(lambda: dict(functions=0, exact=0, bytes=0, remaining_bytes=0, files=0, done=0))
    for job in source_jobs:
        a = by_area[job['area']]
        for key in ('functions', 'exact', 'bytes', 'remaining_bytes'):
            a[key] += job[key]
        a['files'] += 1
        a['done'] += job['status'] == 'done'
    counts = Counter(f['Quality'] for f in functions)
    total_bytes = sum(int(f['Size']) for f in functions)
    exact_bytes = sum(int(f['Size']) for f in functions if f['Quality'] == 'O')
    return dict(format=1, revision=revision, scope=config['scope'],
                metric_evidence=METRIC_EVIDENCE,
                planning_source='config/project_jobs.json technical facts (working tree); function evidence is committed HEAD. No live claims, queue or personal priorities are read.',
                totals=dict(functions=len(functions), exact=counts['O'], quality=dict(counts), bytes=total_bytes,
                            exact_bytes=exact_bytes, percent=100*counts['O']/len(functions) if functions else 0,
                            bytes_percent=100*exact_bytes/total_bytes if total_bytes else 0,
                            unattributed=len(grouped.get('', [])), source_files=len([p for p in grouped if p]),
                            complete_files=sum(j['status']=='done' for j in source_jobs if j['kind']=='source')),
                areas=[dict(name=k, **v) for k,v in sorted(by_area.items(), key=lambda kv:-kv[1]['remaining_bytes'])], jobs=jobs)


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--repo', type=Path, default=ROOT, help='Core checkout to inspect read-only')
    parser.add_argument('--json', action='store_true')
    parser.add_argument('--job', help='Inspect retained metadata for one stable job ID as JSON')
    parser.add_argument('--actionable', action='store_true', help='Exclude completed and deferred jobs; readiness still needs inspection')
    args=parser.parse_args()
    plan=snapshot(args.repo)
    if args.actionable and args.job:
        parser.error('--actionable cannot be combined with --job; inspect a deferred job explicitly')
    if args.actionable:
        plan['jobs'] = [j for j in plan['jobs'] if actionable(j)]
    if args.job:
        job=next((j for j in plan['jobs'] if j['id']==args.job),None)
        if not job:
            parser.error('Unknown job ID')
        print(json.dumps(job, indent=2))
    elif args.json:
        print(json.dumps(plan,indent=2))
    else:
        print(f"{plan['totals']['exact']}/{plan['totals']['functions']} inventory O; {plan['totals']['bytes_percent']:.2f}% of original function bytes. HEAD {plan['revision'][:8]}")
        print(plan['metric_evidence'])
        for j in plan['jobs']:
            hold = ' [DEFERRED]' if j.get('deferred') else ''
            print(f"{j['id']:16} {j['status']:8} {j['title']}{hold}")


if __name__=='__main__':
    main()
