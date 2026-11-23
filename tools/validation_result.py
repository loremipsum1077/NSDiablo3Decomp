"""Address-based per-file results. Compilation success is not match success."""

def summarize(rel, inventory, files, targets, detail, built):
    expected = {int(r['Address'], 16) & 0xffffffff: r for r in inventory
                if files.get(r['Address'], '').lower() == rel.lower()}
    emitted = {address for address, size in targets.values()}
    matched = {targets[name][0] for name, result in detail.items() if result == 'OK'}
    rows = []
    for address, original in expected.items():
        rows.append({'address': original['Address'], 'name': original['Name'],
                     'size': int(original['Size']), 'previous_quality': original['Quality'],
                     'status': 'match' if address in matched else 'mismatch' if address in emitted else 'missing'})
    unexpected = []
    for name in sorted(name for name in built if name not in targets or targets[name][0] not in expected):
        alias = name.replace('C2E', 'C1E').replace('D2E', 'D1E')
        # Aliases are only labeled when the paired emitted body is identical.
        constructor_alias = alias != name and alias in built and built[name] == built[alias]
        unexpected.append({'name': name, 'classification': 'constructor_destructor_alias' if constructor_alias else 'needs_review'})
    gained = [r for r in rows if r['status'] == 'match' and r['previous_quality'] not in ('O', 'M', 'm')]
    regressed = [r['address'] for r in rows if r['status'] != 'match' and r['previous_quality'] in ('O', 'M', 'm')]
    return {'schema': 1, 'compiled': True, 'source': rel, 'functions': rows,
            'expected': len(rows), 'emitted': sum(r['status'] != 'missing' for r in rows),
            'matched': sum(r['status'] == 'match' for r in rows),
            'missing': sum(r['status'] == 'missing' for r in rows),
            'mismatched': sum(r['status'] == 'mismatch' for r in rows),
            'new_matched_functions': len(gained), 'new_matched_bytes': sum(r['size'] for r in gained),
            'previous_matches_not_preserved': regressed, 'unexpected_definitions': unexpected,
            'relocation_targets_verified': False}


def outcome(result):
    if not result.get('compiled'): return 'compile_failed'
    if result.get('previous_matches_not_preserved'): return 'regression'
    if result.get('expected', 0) and result.get('matched') == result['expected']: return 'candidate'
    return 'partial' if result.get('matched', 0) else 'no_matches'
