#!/usr/bin/env python3
"""Versioned read-only technical metadata for optional external workflows."""
import argparse
import hashlib
import json
from pathlib import Path
import subprocess

import project_plan

FORMAT = 1
ROOT = Path(__file__).resolve().parents[1]


def snapshot(root):
    root = Path(root).resolve()
    revision = project_plan.git(root, 'rev-parse', 'HEAD').strip()
    config_path = root / project_plan.CONFIG
    before = hashlib.sha256(config_path.read_bytes()).hexdigest()
    functions, owners = project_plan.inventory(str(root), revision)
    plan = project_plan.snapshot(root)
    if plan['revision'] != revision:
        raise ValueError('Core revision changed during snapshot')
    config = project_plan.load_config(root)
    if (hashlib.sha256(config_path.read_bytes()).hexdigest() != before
            or project_plan.git(root, 'rev-parse', 'HEAD').strip() != revision):
        raise ValueError('Core inputs changed during snapshot')
    return {'format': FORMAT, 'operation': 'snapshot', 'revision': revision,
            'config': config, 'functions': functions, 'owners': owners, 'plan': plan,
            'bindings': {'config_sha256': before,
                         'inventory_blobs': {name: project_plan.git(root, 'rev-parse',
                            f'{revision}:data/2.6.2/{name}.csv').strip()
                            for name in ('functions', 'function_files')}},
            'acceptance': 'Metadata only; no checker/build/source review performed. Inventory O labels are not fresh verification.'}


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--repo', type=Path, default=ROOT)
    parser.add_argument('--format', type=int, choices=[FORMAT], default=FORMAT)
    parser.add_argument('operation', choices=['snapshot'])
    args = parser.parse_args(argv)
    try:
        print(json.dumps(snapshot(args.repo), sort_keys=True))
    except (OSError, ValueError, KeyError, subprocess.CalledProcessError) as exc:
        parser.exit(2, str(exc) + '\n')


if __name__ == '__main__':
    main()
