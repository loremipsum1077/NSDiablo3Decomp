"""Tier-1 promotion: synthetic Git repository and retained receipts, no private inputs."""
import contextlib
import csv
import hashlib
import io
import json
from pathlib import Path
import shutil
import subprocess
import tempfile
import unittest

import proof_debt
import receipts as r
import tier1
from validation import common as c

OWNER = 'X/Owner.cpp'
FIELDS = ['Address', 'Quality', 'Size', 'Name']
ROWS = [('0x0000007100000010', 'O', '000016', 'prior'),
        ('0x0000007100000020', 'U', '000016', 'target'),
        ('0x0000007100000030', 'U', '000016', 'other'),
        ('0x0000007100000040', 'O', '000016', 'debt')]
TARGETS = {'format': 1, 'functions': [{'address': '0x0000007100000020', 'name': 'target'}]}
IMAGES = {'normal': 'sha256:' + '1'*64, 'lua': 'sha256:' + '2'*64}
DEBT = 'mismatch at 0x44: unverified GOT reference: rebuilt undefined GOT target symbol'


def inventory(changes=()):
    rows = [dict(zip(FIELDS, row)) for row in ROWS]
    for row in rows:
        row['Quality'] = dict(changes).get(row['Name'], row['Quality'])
    return tier1.render_inventory(rows, FIELDS)


class Tier1Tests(unittest.TestCase):
    def setUp(self):
        self.temp = Path(tempfile.mkdtemp(prefix='d3-tier1-')).resolve()
        self.addCleanup(shutil.rmtree, self.temp)
        self.repo = self.temp/'repo'
        repo = self.repo
        for name, data in {'src/'+OWNER: '#include "Owner.h"\nint target() { return 1; }\n',
                           'src/X/Owner.h': 'int target();\n', 'src/X/Other.cpp': 'int other() { return 2; }\n',
                           'src/X/Unrelated.h': 'int unrelated();\n', 'include/Common.h': '\n',
                           'toolchain/recipe.txt': 'r\n', 'CMakeLists.txt': 'project(x)\n',
                           'tools/config.toml': '[x]\n', 'data/2.6.2/function_files.csv': 'Address,File\n',
                           'data/2.6.2/data_symbols.csv': 'Address,Name\n'}.items():
            (repo/name).parent.mkdir(parents=True, exist_ok=True)
            (repo/name).write_text(data)
        (repo/tier1.TABLE).write_bytes(inventory())
        proof_debt.write([{'Address': '0x0000007100000040', 'Name': 'debt', 'Mode': 'normal',
                           'Category': 'got-rebuilt-undefined', 'Reason': DEBT}], repo/tier1.LEDGER)
        self.git('init', '-q')
        for key, value in (('user.name', 'Tier-1 fixture'), ('user.email', 'fixture@example.invalid'),
                           ('commit.gpgsign', 'false')):
            self.git('config', key, value)
        (repo/'.git/info/exclude').write_text('/build/\n')
        self.git('add', '-A')
        self.git('commit', '-q', '-m', 'baseline')
        self.baseline = self.head()
        (repo/tier1.TABLE).write_bytes(inventory({'target': 'M'}))
        (repo/('src/'+OWNER)).write_text('#include "Owner.h"\nint target() { return 3; }\n')
        self.git('commit', '-q', '-am', 'candidate')
        self.candidate = self.head()
        self.output = self.temp/'receipts'
        self.make_receipts()
        self.review = self.temp/'review.json'
        c.write(self.review, {'kind': tier1.REVIEW_KIND, 'binding_sha256': c.sha(self.output/'binding.json'),
                              'targets': TARGETS, 'reviewer': 'Independent fixture reviewer', 'independent': True,
                              'blocking_findings': [], 'rationale': 'Synthetic source review'})

    def git(self, *words):
        return subprocess.run(['git', '-C', str(self.repo), *words], check=True,
                              capture_output=True, text=True).stdout.strip()

    def head(self):
        return self.git('rev-parse', 'HEAD')

    def make_receipts(self, statuses=None, prune=False):
        """Mirror receipts.build output: binding -> run -> attempts -> done -> results."""
        statuses = statuses or {}
        output = self.output
        if output.exists():
            shutil.rmtree(output)
        binding = {'format': 1, 'policy': 'recovery-only', 'owner': OWNER, 'post_landing': True, 'targets': TARGETS,
                   'checker_sha256': hashlib.sha256(b'checker').hexdigest(),
                   'original_sha256': hashlib.sha256(b'original').hexdigest(), 'lua_receipt_sha256': '5'*64,
                   'lua_backend_sha256': '6'*64, 'images': IMAGES, 'versions': {
                       label: {'commit': commit, 'source': r.committed_files(self.repo, commit, r.SOURCE),
                               'support': r.committed_files(self.repo, commit, r.SUPPORT)}
                       for label, commit in (('baseline', self.baseline), ('candidate', self.candidate))}}
        c.write(output/'binding.json', binding)
        binding_sha = c.sha(output/'binding.json')
        state = {'format': 1, 'status': 'recovery_evidence_complete', 'binding_sha256': binding_sha, 'modes': {}}
        evidence = {'format': 1, 'toolchain': {}, 'kind': 'recovery-only-evidence-v1'}
        for label, version in binding['versions'].items():
            r.export(self.repo, version['source'], output/label/'source')
            c.write(output/label/'source.json', {'files': {n: i['sha256'] for n, i in version['source'].items()}})
            evidence[label] = {'commit': version['commit'], 'source': {
                'root': str(output/label/'source'), 'manifest': str(output/label/'source.json'),
                'sha256': c.sha(output/label/'source.json')}}
            table = list(csv.DictReader(io.StringIO(r.git(self.repo, 'show', version['commit']+':'+tier1.TABLE).decode())))
            for mode in ('normal', 'lua'):
                attempt = output/label/mode/'attempt-0001'
                r.export(self.repo, version['source'], attempt/'source')
                r.export(self.repo, version['support'], attempt/'support')
                (attempt/'support/tools/check').write_bytes(b'checker')
                (attempt/'support/data/2.6.2/main.elf').write_bytes(b'original')
                c.write(attempt/'binding.json', r.attempt_binding(binding, binding_sha, label, mode), True)
                receipt = attempt/'receipt'
                request = [{'address': x['Address'], 'name': x['Name']} for x in table
                           if x['Quality'] != 'U' or x['Name'] == 'target']
                rows = []
                for item in request:
                    status = statuses.get((label, mode, item['name']),
                                          'mismatch' if item['name'] in ('debt',) or
                                          (item['name'] == 'target' and label == 'baseline') else 'ok')
                    rows.append(dict(item, status=status, reason='' if status == 'ok' else DEBT))
                c.write(receipt/'request.json', {'functions': request})
                c.write(receipt/'result.json', {'format': 1, 'results': rows})
                mount = 'type=bind,src=' + str(attempt/'source') + ',dst=/work,readonly'
                c.write(receipt/'configure.command.json', ['docker', 'run', '--mount', mount, IMAGES[mode], 'cmake',
                        '-DD3_LUA_COMPILER=' + ('ON' if mode == 'lua' else 'OFF')])
                c.write(receipt/'build.command.json', ['docker', 'run', '--mount', mount, IMAGES[mode], 'cmake', '--build', 'b'])
                build = receipt/'build/2.6.2'
                build.mkdir(parents=True)
                (build/'d3').write_bytes(b'linked '+mode.encode())
                (build/'d3.map').write_bytes(b'map')
                c.write(build/'d3.map.provenance', {'format': 1, 'elf_sha256': c.sha(build/'d3'),
                                                     'map_sha256': c.sha(build/'d3.map')})
                (receipt/'objects/src/X').mkdir(parents=True)
                (receipt/'objects/src/X/Owner.cpp.obj').write_bytes(b'object')
                files = {p.relative_to(receipt).as_posix(): c.sha(p) for p in receipt.rglob('*') if p.is_file()}
                c.write(receipt/'done.json', {'kind': 'recovery-only-mode-v1', 'mode': mode,
                                              'full_check_executed': False, 'files': files}, True)
                if prune:
                    for name in ('build/2.6.2/d3', 'build/2.6.2/d3.map', 'objects/src/X/Owner.cpp.obj'):
                        (receipt/name).unlink()
                    (attempt/'support/data/2.6.2/main.elf').unlink()
                state['modes'][label+'_'+mode] = {'root': receipt.relative_to(output).as_posix(),
                    'receipt_sha256': c.sha(receipt/'done.json'), 'attempt_binding_sha256': c.sha(attempt/'binding.json')}
                evidence[label][mode] = {'root': str(receipt), 'receipt_sha256': state['modes'][label+'_'+mode]['receipt_sha256']}
        c.write(output/'evidence.json', evidence)
        c.write(output/'recovery-audit.json', {'kind': 'recovery-only-v1'})
        state.update(evidence_sha256=c.sha(output/'evidence.json'), audit_sha256=c.sha(output/'recovery-audit.json'))
        c.write(output/'run.json', state)

    def evaluate(self, **options):
        return tier1.evaluate(self.repo, self.output, self.review, c.sha(self.review), **options)

    def blocked(self, message, **options):
        with self.assertRaisesRegex(c.Halt, message):
            self.evaluate(**options)

    def test_tier1_passes_with_full_receipts(self):
        outcome = self.evaluate()
        self.assertEqual(outcome['functions'], [{'address': '0x0000007100000020', 'name': 'target', 'from': 'M'}])
        self.assertEqual((outcome['lineage'], outcome['proof_debt_rows'], outcome['pruned_artifacts']), ('exact', [], {}))
        self.assertEqual(outcome['modes']['lua']['regressions'], 0)

    def test_selected_not_ok_in_either_mode_blocks(self):
        for mode in ('normal', 'lua'):
            self.make_receipts({('candidate', mode, 'target'): 'mismatch'})
            self.blocked('Tier-1 criteria fail: ' + mode)

    def test_regression_and_error_rows_block(self):
        self.make_receipts({('candidate', 'lua', 'prior'): 'mismatch'})
        self.blocked('Tier-1 criteria fail: lua')
        self.make_receipts({('candidate', 'normal', 'prior'): 'error'})
        self.blocked('Tier-1 criteria fail: normal')

    def test_pruned_artifacts_require_explicit_opt_in(self):
        self.make_receipts(prune=True)
        self.blocked('Attempt support inputs changed|Retained')
        outcome = self.evaluate(allow_pruned=True)
        self.assertEqual(set(outcome['pruned_artifacts']), {'baseline_normal', 'baseline_lua', 'candidate_normal', 'candidate_lua'})
        # Results, requests and commands are never prunable.
        (self.output/'candidate/lua/attempt-0001/receipt/result.json').unlink()
        self.blocked('Retained receipt file missing: result.json', allow_pruned=True)

    def test_tampered_receipts_fail_closed(self):
        for allow in (False, True):
            self.make_receipts()
            path = self.output/'candidate/normal/attempt-0001/receipt/result.json'
            value = c.read(path)
            value['results'][-1]['status'] = 'ok'
            c.write(path, value)
            self.blocked('changed', allow_pruned=allow)
        self.make_receipts()
        c.write(self.output/'binding.json', dict(c.read(self.output/'binding.json'), owner='X/Else.cpp'))
        self.blocked('Run binding changed')
        self.make_receipts()
        (self.output/'run.json').unlink()
        with self.assertRaises(c.Halt):
            self.evaluate(allow_pruned=True)

    def test_incomplete_request_blocks(self):
        receipt = self.output/'candidate/normal/attempt-0001/receipt'
        for name in ('request.json', 'result.json'):
            value = c.read(receipt/name)
            key = 'functions' if name == 'request.json' else 'results'
            value[key] = [x for x in value[key] if x['name'] != 'debt']
            c.write(receipt/name, value)
        done = c.read(receipt/'done.json')
        done['files'].update({n: c.sha(receipt/n) for n in ('request.json', 'result.json')})
        (receipt/'done.json').unlink()
        c.write(receipt/'done.json', done, True)
        state = c.read(self.output/'run.json')
        state['modes']['candidate_normal']['receipt_sha256'] = c.sha(receipt/'done.json')
        evidence = c.read(self.output/'evidence.json')
        evidence['candidate']['normal']['receipt_sha256'] = c.sha(receipt/'done.json')
        c.write(self.output/'evidence.json', evidence)
        state['evidence_sha256'] = c.sha(self.output/'evidence.json')
        c.write(self.output/'run.json', state)
        self.blocked('omitted or added identities', allow_pruned=True)

    def test_review_is_required_pinned_and_bound(self):
        with self.assertRaisesRegex(c.Halt, 'review is required'):
            tier1.evaluate(self.repo, self.output)
        with self.assertRaisesRegex(c.Halt, 'pin mismatch'):
            tier1.evaluate(self.repo, self.output, self.review, '0'*64)
        for field, value in (('independent', False), ('reviewer', ' '), ('blocking_findings', ['x']),
                             ('binding_sha256', '0'*64), ('targets', {'format': 1, 'functions': []})):
            original = c.read(self.review)
            c.write(self.review, dict(original, **{field: value}))
            self.blocked('Review|review')
            c.write(self.review, original)

    def test_recovery_transaction_review(self):
        transaction = self.temp/'transaction'
        transaction.mkdir()
        self.output = transaction/'post-receipts'
        self.make_receipts()
        review = {'kind': tier1.RECOVERY_REVIEW_KIND, 'reviewer': 'Independent', 'independent': True,
                  'blocking_findings': [], 'obligations': {'normal:abi-unwind': {
                      'decision': 'safe-uncertified', 'rationale': 'r', 'evidence_reference': 'e'}}}
        c.write(transaction/'independent-review.json', review)
        pin = c.sha(transaction/'independent-review.json')
        c.write(transaction/'targets.json', TARGETS)
        c.write(transaction/'transaction.json', {'baseline': self.baseline, 'integration': self.candidate,
                                                 'reviewed_candidate': self.candidate, 'review': {'review_sha256': pin}})
        c.write(transaction/'landed.json', {'status': 'recovery_landed', 'commit': self.candidate,
                                            'review_sha256': pin, 'post_receipts': str(self.output)})
        outcome = tier1.evaluate(self.repo, self.output, transaction=transaction)
        self.assertEqual(outcome['review']['safe_uncertified'], ['normal:abi-unwind'])
        review['obligations']['normal:abi-unwind']['decision'] = 'pending'
        c.write(transaction/'independent-review.json', review)
        with self.assertRaisesRegex(c.Halt, 'pin mismatch'):
            tier1.evaluate(self.repo, self.output, transaction=transaction)

    def test_lineage_exact_descendant_and_stale(self):
        (self.repo/'src/X/Other.cpp').write_text('int other() { return 9; }\n')
        (self.repo/'src/X/Unrelated.h').write_text('int unrelated(int);\n')
        self.git('commit', '-q', '-am', 'later unrelated owner')
        self.blocked('stale receipts')
        self.assertEqual(self.evaluate(descendant=True)['lineage'], 'descendant')
        for path, text in (('src/X/Owner.h', 'int target(void);\n'), ('src/'+OWNER, '#include "Owner.h"\n'),
                           ('tools/config.toml', '[y]\n'), ('src/X/Owner.h', '#include "Shadow.h"\n')):
            (self.repo/path).write_text(text)
            self.git('commit', '-q', '-am', 'touch ' + path)
            self.blocked('include-closure|include resolution|Unresolved', descendant=True)
            self.git('reset', '-q', '--hard', 'HEAD~1')

    def test_inventory_drift_and_already_o_block(self):
        (self.repo/tier1.TABLE).write_bytes(inventory({'target': 'O'}))
        self.git('commit', '-q', '-am', 'already promoted')
        self.blocked('quality changed', descendant=True)

    def test_already_o_targets_are_verified_but_not_promoted(self):
        global TARGETS
        saved = TARGETS
        TARGETS = {'format': 1, 'functions': saved['functions'] + [{'address': '0x0000007100000010', 'name': 'prior'}]}
        try:
            self.make_receipts()
            c.write(self.review, dict(c.read(self.review), targets=TARGETS, binding_sha256=c.sha(self.output/'binding.json')))
            self.assertEqual([f['name'] for f in self.evaluate()['functions']], ['target'])
        finally:
            TARGETS = saved

    def test_include_next_is_toolchain_and_macro_include_blocks(self):
        (self.repo/'include/Common.h').write_text('#include_next <Common.h>\n')
        (self.repo/'src/X/Owner.h').write_text('#include <Common.h>\nint target();\n')
        self.git('commit', '-q', '-am', 'shim header')
        self.assertIn('include/Common.h', tier1.includes(self.repo, 'HEAD', 'src/X/Owner.h'))
        (self.repo/'src/X/Owner.h').write_text('#include HEADER\n')
        self.git('commit', '-q', '-am', 'macro include')
        with self.assertRaisesRegex(c.Halt, 'Macro'):
            tier1.includes(self.repo, 'HEAD', 'src/X/Owner.h')

    def test_promote_rewrites_rows_and_ledger_in_one_commit(self):
        (self.repo/'build').mkdir()
        ledger_before = proof_debt.read(self.repo/tier1.LEDGER)
        self.make_receipts({('candidate', 'normal', 'target'): 'ok'})
        outcome = tier1.promote(self.repo, self.evaluate())
        self.assertEqual(outcome['status'], 'tier1_promoted')
        self.assertEqual(self.head(), outcome['commit'])
        self.assertEqual((self.repo/tier1.TABLE).read_bytes(), inventory({'target': 'O'}))
        self.assertEqual(proof_debt.read(self.repo/tier1.LEDGER), ledger_before)
        self.assertEqual(self.git('status', '--porcelain'), '')
        self.assertIn('Tier-2 pending', self.git('log', '-1', '--format=%B'))
        # No candidate debt for a tier-1 target: the ledger is rewritten byte-identically.
        self.assertEqual(self.git('diff', '--name-only', 'HEAD~1', 'HEAD').split(), [tier1.TABLE])

    def test_promote_writes_debt_rows_and_restores_on_failure(self):
        outcome = self.evaluate()
        outcome['proof_debt_rows'] = [{'Address': '0x0000007100000020', 'Name': 'target', 'Mode': 'lua',
                                       'Category': 'other', 'Reason': 'synthetic'}]
        table, ledger = (self.repo/tier1.TABLE).read_bytes(), (self.repo/tier1.LEDGER).read_bytes()
        hooks = self.repo/'.git/hooks/pre-commit'
        hooks.write_text('#!/bin/sh\nexit 1\n')
        hooks.chmod(0o755)
        with self.assertRaises(subprocess.CalledProcessError):
            tier1.promote(self.repo, outcome)
        self.assertEqual(((self.repo/tier1.TABLE).read_bytes(), (self.repo/tier1.LEDGER).read_bytes()), (table, ledger))
        self.assertEqual(self.git('status', '--porcelain'), '')
        hooks.unlink()
        tier1.promote(self.repo, outcome)
        self.assertIn('target', [x['Name'] for x in proof_debt.read(self.repo/tier1.LEDGER)])

    def test_promote_requires_clean_evaluated_head(self):
        outcome = self.evaluate()
        (self.repo/'src/X/Other.cpp').write_text('dirty\n')
        with self.assertRaisesRegex(c.Halt, 'clean'):
            tier1.promote(self.repo, outcome)

    def test_cli_dry_run_writes_nothing(self):
        before = self.git('status', '--porcelain', '--ignored')
        out = io.StringIO()
        with contextlib.redirect_stdout(out):
            code = tier1.main(['promote', '--repo', str(self.repo), '--receipts', str(self.output), '--review',
                               str(self.review), '--review-sha256', c.sha(self.review), '--dry-run'])
        self.assertEqual((code, json.loads(out.getvalue())['status']), (0, 'tier1_dry_run'))
        self.assertEqual((self.head(), self.git('status', '--porcelain', '--ignored')), (self.candidate, before))


if __name__ == '__main__':
    unittest.main()
