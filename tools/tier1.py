#!/usr/bin/env python3
"""Tier-1 O promotion from completed, retained both-mode receipts.

  tools/tier1.py promote --transaction build/recovery-land/<id> --dry-run
  tools/tier1.py promote --receipts DIR --review FILE --review-sha256 SHA [--dry-run]
  tools/tier1.py scan build/recovery-land            # dry-run every transaction

Tier 1 (docs/checker_policy.md): every selected identity is checker `ok` in both
normal and Lua candidate results, `integration.comparison` reports no regressions
and no unfinished selected identity, no candidate row is `error`, and a recorded
independent review is pinned by SHA256. Nothing is rebuilt or rechecked: the
retained receipts are re-verified (bindings, attempt inputs, every retained file
hash, link provenance, request completeness) and their results re-evaluated.

Lineage: by default the receipt candidate's committed source and checker support
must equal HEAD's (functions.csv may differ only in quality). `--descendant`
admits a later HEAD whose changes since the candidate touch no owner source,
header, include/toolchain/CMake file or checker configuration/ownership/data
ledger; it trusts that unrelated link layout does not flip checker status.

Promotion (without --dry-run) holds build/land.lock, requires a clean checkout
at the evaluated HEAD, rewrites only the selected rows' Quality to O plus their
data/2.6.2/proof_debt.csv rows (both atomically), and commits locally. Any
failure leaves both files as they were. Tier-2 gaps (data/relocation target
identity, unwind, build lineage) are recorded in the commit and the retained
build/tier1-land/<id>/promotion.json, never treated as proven.
"""
import argparse
import csv
import fcntl
import io
import json
import os
from pathlib import Path
import re
import subprocess
import sys
import uuid

sys.path.insert(0, str(Path(__file__).resolve().parent))
from validation import common as c, integration
import proof_debt
import receipts as r

ROOT = Path(__file__).resolve().parent.parent
TABLE = 'data/2.6.2/functions.csv'
LEDGER = 'data/2.6.2/proof_debt.csv'
REVIEW_KIND = 'independent-tier1-review-v1'
RECOVERY_REVIEW_KIND = 'independent-recovery-review-v1'
TIER2 = ('data-relocation-target-identity', 'unwind', 'build-lineage')
PROMOTABLE = ('U', 'M')
# Bulky retained build artifacts that may be deleted after landing (--allow-pruned).
PRUNABLE = ('build/2.6.2/d3', 'build/2.6.2/d3.map', 'data/2.6.2/main.elf')


def halt(message):
    raise c.Halt(message)


def git(repo, *words):
    return r.git(repo, *words).decode().strip()


def prunable(name):
    return name in PRUNABLE or (name.startswith('objects/') and name.endswith('.obj'))


def pruned_checkpoint(output, binding, binding_sha256, label, mode, entry):
    """receipts.checkpoint for receipts whose bulky build artifacts were deleted.

    Every retained file must still hash as bound; only PRUNABLE artifacts may be
    absent. The result/request chain (binding -> run -> done -> result) stays
    exact. Link provenance cannot be re-verified without the ELF: a tier-2 gap.
    """
    if not isinstance(entry, dict) or set(entry) != {'root', 'receipt_sha256', 'attempt_binding_sha256'} or \
            not isinstance(entry['root'], str) or not re.fullmatch(
                re.escape(label+'/'+mode) + r'/attempt-[0-9]{4,}/receipt', entry['root']):
        halt('Invalid mode checkpoint binding')
    receipt = r.inside(output, entry['root'])
    attempt = receipt.parent
    proof = r.regular(attempt/'binding.json')
    if c.sha(proof) != entry['attempt_binding_sha256'] or \
            r.read_json(proof) != r.attempt_binding(binding, binding_sha256, label, mode):
        halt('Attempt version/mode/source/input/image binding changed')
    version = binding['versions'][label]
    r.source_check(r.inside(output, attempt.relative_to(output).as_posix()+'/source'), version['source'])
    support = r.inside(output, attempt.relative_to(output).as_posix()+'/support')
    expected = {n: i['sha256'] for n, i in version['support'].items()}
    expected.update({'tools/check': binding['checker_sha256'], 'data/2.6.2/main.elf': binding['original_sha256']})
    paths = list(support.rglob('*'))
    actual = {p.relative_to(support).as_posix(): c.sha(p) for p in paths if p.is_file() and not p.is_symlink()}
    if any(p.is_symlink() for p in paths) or any(expected.get(n) != v for n, v in actual.items()) or \
            set(expected) - set(actual) - {'data/2.6.2/main.elf'} or set(version['support']) - set(actual):
        halt('Attempt support inputs changed')
    done = r.regular(receipt/'done.json')
    if c.sha(done) != entry['receipt_sha256']:
        halt('Completed checkpoint changed')
    receipt_value = r.read_json(done)
    if binding.get('policy', 'strict') == 'recovery-only':
        if set(receipt_value) != {'kind', 'mode', 'full_check_executed', 'files'} or \
                receipt_value['kind'] != 'recovery-only-mode-v1' or receipt_value['mode'] != mode:
            halt('Wrong or incomplete recovery receipt')
    elif set(receipt_value) != {'full_check_rc', 'files'} or receipt_value['full_check_rc'] != 0:
        halt('Incomplete strict mode receipt')
    pruned = sorted(n for n in receipt_value['files'] if not r.inside(receipt, n).exists())
    if any(not prunable(n) for n in pruned):
        halt('Retained receipt file missing: ' + next(n for n in pruned if not prunable(n)))
    for name, value in receipt_value['files'].items():
        if name not in pruned and (not r.HEX.fullmatch(value) or c.sha(r.regular(r.inside(receipt, name))) != value):
            halt('Retained artifact changed')
    for name in ('result.json', 'request.json', 'configure.command.json', 'build.command.json'):
        if name not in receipt_value['files'] or name in pruned:
            halt('Missing bound check artifact: ' + name)
    for name in ('configure.command.json', 'build.command.json'):
        command = r.read_json(receipt/name)
        mount = 'type=bind,src=' + str(attempt/'source') + ',dst=/work,readonly'
        if not isinstance(command, list) or command[:2] != ['docker', 'run'] or mount not in command or \
                binding['images'][mode] not in command:
            halt('Attempt command image/source identity changed')
    if not pruned:
        r.verify_link_provenance(receipt/'build/2.6.2/d3')
    return receipt, pruned


def verify_receipts(output, allow_pruned=False):
    """receipts.verify without re-deriving the recovery audit: tier 1 needs results.

    The recovery audit binds the full validator closure; tier 1 must stay usable
    after unrelated tool edits, so the audit is only bound by hash here.
    """
    output = Path(output).resolve()
    binding = r.read_json(r.regular(output/'binding.json'))
    state = r.read_json(r.regular(output/'run.json'))
    policy = binding.get('policy', 'strict')
    if policy not in ('strict', 'recovery-only'):
        halt('Unknown receipt policy')
    if state.get('binding_sha256') != c.sha(output/'binding.json'):
        halt('Run binding changed')
    if state.get('status') != ('recovery_evidence_complete' if policy == 'recovery-only' else 'complete'):
        halt('Receipts incomplete')
    if set(binding.get('versions', {})) != {'baseline', 'candidate'} or set(state.get('modes', {})) != {
            label+'_'+mode for label in ('baseline', 'candidate') for mode in proof_debt.MODES}:
        halt('Missing or stale retained mode')
    expected, pruned = {'format': 1, 'toolchain': {}}, {}
    if policy == 'recovery-only':
        expected['kind'] = 'recovery-only-evidence-v1'
    for label, version in binding['versions'].items():
        r.source_check(r.inside(output, label+'/source'), version['source'])
        manifest = r.regular(output/label/'source.json')
        if r.read_json(manifest) != {'files': {n: i['sha256'] for n, i in version['source'].items()}}:
            halt('Source manifest changed')
        expected[label] = {'commit': version['commit'], 'source': {
            'root': str(output/label/'source'), 'manifest': str(manifest), 'sha256': c.sha(manifest)}}
        for mode in proof_debt.MODES:
            entry = state['modes'][label+'_'+mode]
            if allow_pruned:
                root, missing = pruned_checkpoint(output, binding, state['binding_sha256'], label, mode, entry)
                if missing:
                    pruned[label+'_'+mode] = len(missing)
            else:
                root = r.checkpoint(output, binding, state['binding_sha256'], label, mode, entry)
            expected[label][mode] = {'root': str(root), 'receipt_sha256': entry['receipt_sha256']}
    evidence = r.regular(output/'evidence.json')
    if state.get('evidence_sha256') != c.sha(evidence) or r.read_json(evidence) != expected:
        halt('Evidence manifest does not match bound source/mode identities')
    if policy == 'recovery-only' and state.get('audit_sha256') != c.sha(r.regular(output/'recovery-audit.json')):
        halt('Recovery audit changed')
    return binding, r.read_json(evidence), pruned


def committed_inventory(path):
    with Path(path).open(newline='') as stream:
        return list(csv.DictReader(stream))


def mode_results(root, targets):
    """Results of one retained mode, checked against its exact batch request."""
    root = Path(root)
    raw = r.read_json(root/'result.json')
    keyed = proof_debt.load_results(root/'result.json')
    request = r.read_json(root/'request.json')
    inventory = committed_inventory(root.parent/'support'/TABLE)
    selected = {(t['address'], t['name']) for t in targets}
    wanted = {(x['Address'], x['Name']) for x in inventory if x['Quality'] != 'U' or (x['Address'], x['Name']) in selected}
    asked = integration.identities(request.get('functions'))
    if asked != wanted or set(keyed) != wanted:
        halt('Checker request/result omitted or added identities: ' + str(root))
    return raw, keyed, inventory


def tier1(binding, evidence):
    targets = binding['targets']['functions']
    if not isinstance(targets, list) or not 1 <= len(targets) <= 64:
        halt('Bounded target selection required')
    pairs = [{'Address': t['address'], 'Name': t['name']} for t in targets]
    selected = [(t['address'], t['name']) for t in targets]
    modes, results, inventory = {}, {}, None
    for mode in proof_debt.MODES:
        before, _, _ = mode_results(evidence['baseline'][mode]['root'], targets)
        after, keyed, table = mode_results(evidence['candidate'][mode]['root'], targets)
        try:
            summary = integration.comparison(before, after, pairs)
        except (KeyError, TypeError, ValueError) as error:
            halt(mode + ': comparison scope/identity mismatch: ' + str(error))
        errors = sum(row['status'] == 'error' for row in after['results'])
        not_ok = sorted(k[1] for k in selected if keyed[k]['status'] != 'ok')
        modes[mode] = dict(summary, candidate_errors=errors, selected_not_ok=not_ok)
        results[mode] = keyed
        if inventory is None:
            inventory = table
        elif table != inventory:
            halt('Normal and Lua candidate inventories differ')
    failed = [m for m, s in modes.items() if s['regressions'] or s['owner_not_ok'] or s['candidate_errors']]
    return modes, failed, proof_debt.debt(selected, results), inventory


def recovery_review(transaction, output, binding, pin):
    """The landing's recorded independent review, bound through its transaction."""
    transaction = Path(transaction).resolve()
    record = r.read_json(transaction/'transaction.json')
    landed = r.read_json(transaction/'landed.json')
    path = r.regular(transaction/'independent-review.json')
    actual = c.sha(path)
    if (record.get('review', {}).get('review_sha256') != actual or landed.get('review_sha256') != actual or
            (pin is not None and pin != actual)):
        halt('Independent review pin mismatch')
    if landed.get('status') != 'recovery_landed' or landed.get('commit') != record.get('integration'):
        halt('Transaction did not land')
    if Path(landed.get('post_receipts', '')).resolve() != Path(output).resolve():
        halt('Transaction does not name these receipts')
    if (binding.get('policy') != 'recovery-only' or binding.get('post_landing') is not True or
            binding['versions']['candidate']['commit'] != record['integration'] or
            binding['versions']['baseline']['commit'] != record['baseline']):
        halt('Receipts are not this transaction\'s post-landing receipts')
    if r.read_json(transaction/'targets.json') != binding['targets']:
        halt('Transaction targets differ from receipt targets')
    value = r.read_json(path)
    if (value.get('kind') != RECOVERY_REVIEW_KIND or not isinstance(value.get('reviewer'), str) or
            not value['reviewer'].strip() or value.get('independent') is not True or
            value.get('blocking_findings') != [] or not value.get('obligations')):
        halt('Independent review missing or blocking findings unresolved')
    for key, row in value['obligations'].items():
        if (row.get('decision') not in ('verified', 'safe-uncertified') or
                any(not isinstance(row.get(k), str) or not row[k].strip() for k in ('rationale', 'evidence_reference'))):
            halt('Unreviewed obligation: ' + key)
    return {'kind': value['kind'], 'reviewer': value['reviewer'], 'review_sha256': actual,
            'reviewed_candidate': record['reviewed_candidate'],
            'safe_uncertified': sorted(k for k, v in value['obligations'].items() if v['decision'] == 'safe-uncertified')}


def tier1_review(path, pin, output, binding):
    if pin is None or not r.HEX.fullmatch(pin) or c.sha(r.regular(path)) != pin:
        halt('Independent review pin mismatch')
    value = r.read_json(path)
    if set(value) != {'kind', 'binding_sha256', 'targets', 'reviewer', 'independent', 'blocking_findings', 'rationale'}:
        halt('Malformed tier-1 review')
    if value['kind'] != REVIEW_KIND or value['binding_sha256'] != c.sha(Path(output)/'binding.json') or \
            value['targets'] != binding['targets']:
        halt('Review source/evidence binding mismatch')
    if (not isinstance(value['reviewer'], str) or not value['reviewer'].strip() or value['independent'] is not True or
            value['blocking_findings'] != [] or not isinstance(value['rationale'], str) or not value['rationale'].strip()):
        halt('Independent review missing or blocking findings unresolved')
    return {'kind': value['kind'], 'reviewer': value['reviewer'], 'review_sha256': pin, 'safe_uncertified': []}


INCLUDE = re.compile(r'^[ \t]*#[ \t]*include(_next)?[ \t]*([<"])([^>"]+)[>"]', re.M)
MACRO_INCLUDE = re.compile(r'^[ \t]*#[ \t]*include(?:_next)?[ \t]+[A-Za-z_]', re.M)


def includes(repo, commit, path):
    """Textual include closure of an owner at a commit, resolved like the build.

    Every #include in every branch is followed (a superset of the preprocessed
    closure). Search order: including directory (quoted only), include/, src/.
    Unresolved angle includes are toolchain/system headers (toolchain changes are
    refused separately), as is #include_next. Unresolved quoted or macro includes
    fail closed.
    """
    tree = set(git(repo, 'ls-tree', '-r', '--name-only', commit, '--', 'src', 'include').splitlines())
    seen, queue = set(), [path]
    while queue:
        name = queue.pop()
        if name in seen:
            continue
        seen.add(name)
        text = r.git(repo, 'show', commit+':'+name).decode('utf-8', 'replace')
        if MACRO_INCLUDE.search(text):
            halt('Macro #include in owner closure: ' + name)
        for following, kind, target in INCLUDE.findall(text):
            if following:
                continue  # include_next continues into toolchain headers
            bases = ([str(Path(name).parent)] if kind == '"' else []) + ['include', 'src']
            found = next((os.path.normpath(b+'/'+target) for b in bases
                          if os.path.normpath(b+'/'+target) in tree), None)
            if found:
                queue.append(found)
            elif kind == '"':
                halt('Unresolved quoted #include in owner closure: ' + name + ': ' + target)
    return seen


def lineage(repo, binding, descendant):
    head = git(repo, 'rev-parse', 'HEAD')
    version = binding['versions']['candidate']
    candidate = version['commit']
    source, support = r.committed_files(repo, head, r.SOURCE), r.committed_files(repo, head, r.SUPPORT)
    fixed = [n for n in r.SUPPORT if n != TABLE]
    if source == version['source'] and all(support[n] == version['support'][n] for n in fixed):
        return head, 'exact'
    if not descendant:
        halt('HEAD source/support differs from the receipt candidate (stale receipts; see --descendant)')
    owner = binding.get('owner')
    if binding.get('policy') != 'recovery-only' or not owner:
        halt('--descendant requires an owner-scoped recovery receipt')
    if subprocess.run(['git', '-C', str(repo), 'merge-base', '--is-ancestor', candidate, head]).returncode:
        halt('Receipt candidate is not an ancestor of HEAD')
    changed = set(git(repo, 'diff', '--name-only', candidate, head).splitlines())
    closure = includes(repo, candidate, 'src/'+owner)
    if closure != includes(repo, head, 'src/'+owner):
        halt('Owner include resolution changed since candidate')
    hit = sorted(p for p in changed if p in closure or p.startswith('toolchain/') or
                 p == 'CMakeLists.txt' or p in fixed)
    if hit:
        halt('Changes since candidate touch owner/include-closure/toolchain/checker inputs: ' + ', '.join(hit[:5]))
    return head, 'descendant'


def head_inventory(repo, head):
    return list(csv.DictReader(io.StringIO(git(repo, 'show', head+':'+TABLE))))


def selection(live, candidate, targets):
    """Selected live U/M rows: same membership and quality as the receipt.

    Already-O selected rows (whole-owner receipts) are verified but not promoted.
    """
    key = lambda x: (x['Address'], x['Name'], x['Size'])
    if [key(x) for x in live] != [key(x) for x in candidate]:
        halt('Inventory membership/sizes changed since the receipt candidate')
    index = {(x['Address'], x['Name']): i for i, x in enumerate(live)}
    rows = []
    for t in targets:
        i = index.get((t['address'], t['name']))
        if i is None:
            halt('Selected identity absent from inventory: ' + t['name'])
        if live[i]['Quality'] != candidate[i]['Quality']:
            halt('Selected quality changed since the receipt: ' + t['name'])
        if live[i]['Quality'] in PROMOTABLE:
            rows.append(i)
        elif live[i]['Quality'] != 'O':
            halt('Selected identity is ' + live[i]['Quality'] + ', not U/M/O: ' + t['name'])
    if not rows:
        halt('No selected U/M identity to promote')
    return rows


def evaluate(repo, receipts, review=None, review_sha256=None, transaction=None, descendant=False, allow_pruned=False):
    """Every gate; raises Halt on the first failure. Never writes."""
    repo, receipts = Path(repo).resolve(), Path(receipts).resolve()
    binding, evidence, pruned = verify_receipts(receipts, allow_pruned)
    if transaction is not None:
        approved = recovery_review(transaction, receipts, binding, review_sha256)
        if approved['reviewed_candidate'] != binding['versions']['candidate']['commit'] and \
                r.committed_files(repo, approved['reviewed_candidate'], r.SOURCE) != binding['versions']['candidate']['source']:
            halt('Integrated source differs from independently reviewed source')
    elif review is not None:
        approved = tier1_review(review, review_sha256, receipts, binding)
    else:
        halt('A recorded independent review is required')
    modes, failed, debt, candidate_inventory = tier1(binding, evidence)
    if failed:
        detail = '; '.join(m+': '+json.dumps({k: modes[m][k] for k in ('regressions', 'owner_not_ok', 'candidate_errors')})
                           + (' not ok: '+', '.join(modes[m]['selected_not_ok'][:4]) if modes[m]['selected_not_ok'] else '')
                           for m in failed)
        halt('Tier-1 criteria fail: ' + detail)
    head, kind = lineage(repo, binding, descendant)
    live = head_inventory(repo, head)
    indices = selection(live, candidate_inventory, binding['targets']['functions'])
    return {'status': 'tier1_verified', 'head': head, 'lineage': kind, 'receipts': str(receipts),
            'binding_sha256': c.sha(receipts/'binding.json'), 'owner': binding.get('owner'),
            'functions': [{'address': live[i]['Address'], 'name': live[i]['Name'], 'from': live[i]['Quality']} for i in indices],
            'modes': modes, 'review': approved, 'proof_debt_rows': debt,
            'tier2_pending': list(TIER2), 'pruned_artifacts': pruned, 'strict_certified': False}


def render_inventory(rows, fields):
    stream = io.StringIO()
    writer = csv.DictWriter(stream, fieldnames=fields, lineterminator='\n')
    writer.writeheader()
    writer.writerows(rows)
    return stream.getvalue().encode()


def replace(path, data):
    temporary = path.with_name(path.name + '.' + uuid.uuid4().hex + '.tmp')
    with temporary.open('xb') as stream:
        stream.write(data)
        stream.flush()
        os.fsync(stream.fileno())
    os.replace(temporary, path)


def promote(repo, outcome):
    """Apply an evaluated outcome. Caller re-evaluates under the lock first."""
    repo = Path(repo).resolve()
    if git(repo, 'status', '--porcelain') or git(repo, 'rev-parse', 'HEAD') != outcome['head']:
        halt('Checkout must be clean at the evaluated HEAD')
    table, ledger = repo/TABLE, repo/LEDGER
    if table.is_symlink() or ledger.is_symlink():
        halt('Symlinked inventory/ledger refused')
    subprocess.run(['git', '-C', str(repo), 'ls-files', '--error-unmatch', TABLE, LEDGER],
                   check=True, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    original_table, original_ledger = table.read_bytes(), ledger.read_bytes()
    rows = list(csv.DictReader(io.StringIO(original_table.decode())))
    fields = list(rows[0])
    if render_inventory(rows, fields) != original_table:
        halt('functions.csv is not in canonical form; refusing to rewrite it')
    debt = proof_debt.read(ledger)
    if proof_debt.render(debt) != original_ledger:
        halt('proof_debt.csv is not in canonical form; regenerate it first')
    chosen = {(f['address'], f['name']) for f in outcome['functions']}
    for row in rows:
        if (row['Address'], row['Name']) in chosen:
            if row['Quality'] not in PROMOTABLE:
                halt('Selected row changed during promotion')
            row['Quality'] = 'O'
    debt = [x for x in debt if (x['Address'], x['Name']) not in chosen] + outcome['proof_debt_rows']
    stage = repo/'build/tier1-land'/uuid.uuid4().hex
    stage.mkdir(parents=True)
    c.write(stage/'outcome.json', outcome, True)
    message = ['Promote ' + (outcome['owner'] or 'selected functions') + ' to O (tier 1)',
               'Tier 1: both-mode checker ok, no regressions or errors; independent review SHA256 '
               + outcome['review']['review_sha256'] + '.\nReceipt binding SHA256 ' + outcome['binding_sha256']
               + '; lineage ' + outcome['lineage'] + '.\nTier-2 pending: ' + ', '.join(outcome['tier2_pending'])
               + '.\nReview safe-uncertified: ' + (', '.join(outcome['review']['safe_uncertified']) or 'none')
               + ('.\nPruned receipt artifacts: ' + ', '.join(sorted(outcome['pruned_artifacts'])) if outcome['pruned_artifacts'] else '')
               + '.\nFunctions: ' + ', '.join(f['name'] for f in outcome['functions'])]
    try:
        replace(table, render_inventory(rows, fields))
        replace(ledger, proof_debt.render(debt))
        r.git(repo, 'add', '-u', '--', TABLE, LEDGER)
        args = ['commit', '-q']
        for paragraph in message:
            args += ['-m', paragraph]
        r.git(repo, *args)
    except BaseException:
        replace(table, original_table)
        replace(ledger, original_ledger)
        subprocess.run(['git', '-C', str(repo), 'reset', '-q', '--', TABLE, LEDGER])
        raise
    commit = git(repo, 'rev-parse', 'HEAD')
    c.write(stage/'promotion.json', {'status': 'tier1_promoted', 'commit': commit, 'outcome_sha256': c.sha(stage/'outcome.json')}, True)
    return dict(outcome, status='tier1_promoted', commit=commit, retained=str(stage))


def run_one(args, transaction=None):
    if transaction is not None:
        receipts, review, pin = Path(transaction)/'post-receipts', None, args.review_sha256
    else:
        receipts, review, pin = args.receipts, args.review, args.review_sha256
    outcome = evaluate(args.repo, receipts, review, pin, transaction, args.descendant, args.allow_pruned)
    if args.dry_run:
        return dict(outcome, status='tier1_dry_run')
    (Path(args.repo)/'build').mkdir(exist_ok=True)
    with (Path(args.repo)/'build/land.lock').open('w') as lock:
        fcntl.flock(lock, fcntl.LOCK_EX)
        # Re-evaluate under the lock: HEAD or receipts may have changed meanwhile.
        outcome = evaluate(args.repo, receipts, review, pin, transaction, args.descendant, args.allow_pruned)
        return promote(args.repo, outcome)


def scan(args):
    table = []
    for directory in sorted(p for p in Path(args.root).iterdir() if p.is_dir()):
        entry = {'transaction': directory.name}
        try:
            entry['owner'] = r.read_json(directory/'post-receipts/binding.json').get('owner')
        except Exception:
            entry['owner'] = None
        try:
            outcome = run_one(args, directory)
            entry.update(promote=True, lineage=outcome['lineage'], functions=[f['name'] for f in outcome['functions']])
        except Exception as error:
            entry.update(promote=False, reason=type(error).__name__ + ': ' + str(error))
        table.append(entry)
        print(json.dumps(entry, sort_keys=True), flush=True)
    return table


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = ap.add_subparsers(dest='command', required=True)
    one = sub.add_parser('promote', help='verify tier 1 and promote selected rows to O')
    one.add_argument('--receipts', type=Path, help='completed receipt output directory')
    one.add_argument('--review', type=Path, help='independent-tier1-review-v1 file')
    one.add_argument('--transaction', type=Path, help='build/recovery-land/<id>: post-receipts + recorded review')
    one.add_argument('--review-sha256')
    one.add_argument('--dry-run', action='store_true')
    many = sub.add_parser('scan', help='dry-run every transaction under a recovery-land directory')
    many.add_argument('root', type=Path)
    for p in (one, many):
        p.add_argument('--repo', type=Path, default=ROOT)
        p.add_argument('--descendant', action='store_true', help='admit HEAD descending from the candidate (see docstring)')
        p.add_argument('--allow-pruned', action='store_true', help='admit receipts whose ELF/map/objects/original copies were deleted; tier-2 build-lineage gap')
    args = ap.parse_args(argv)
    if args.command == 'scan':
        args.dry_run, args.review_sha256 = True, None
        scan(args)
        return 0
    if (args.transaction is None) == (args.receipts is None) or (args.receipts and not args.review):
        ap.error('give either --transaction, or --receipts with --review and --review-sha256')
    try:
        print(json.dumps(run_one(args, args.transaction), sort_keys=True))
        return 0
    except (c.Halt, OSError, ValueError, TypeError, KeyError, subprocess.CalledProcessError, proof_debt.Error) as error:
        print(json.dumps({'status': 'tier1_blocked', 'error': str(error)}))
        return 1


if __name__ == '__main__':
    raise SystemExit(main())
