"""Explicit historical baseline references for recovery integration only.

Never copy commands, rewrite old bindings, or describe reused modes as fresh.
Candidate modes still build normally. Every reference reauthenticates the old
baseline source, support, commands, objects, link provenance and result seal.
"""
from pathlib import Path
import receipts as r
from validation import common as c

KIND = 'reviewed-recovery-baseline-reference-v1'


def require(value, message):
    if not value:
        raise c.Halt('Baseline reference: ' + message)


def contract(current, previous):
    require(current.get('policy') == previous.get('policy') == 'recovery-only', 'single-owner recovery only')
    require(current.get('post_landing') is True and previous.get('post_landing') is False,
            'integration must refer to pre-landing evidence')
    require('baseline_reference' not in previous, 'reference chains are unsupported')
    excluded = {'versions', 'post_landing', 'changed_paths', 'baseline_reference', 'images'}
    require({k:v for k,v in current.items() if k not in excluded} ==
            {k:v for k,v in previous.items() if k not in excluded}, 'policy, targets, tools or inputs differ')
    require(current['versions']['baseline'] == previous['versions']['baseline'], 'baseline version differs')
    a, b = current['versions']['candidate'], previous['versions']['candidate']
    require(set(a) == set(b) == {'commit', 'source', 'support'} and a['source'] == b['source'],
            'integrated source differs from reviewed source')
    inventory = 'data/2.6.2/functions.csv'
    require({k:v for k,v in a['support'].items() if k != inventory} ==
            {k:v for k,v in b['support'].items() if k != inventory}, 'candidate support differs')
    require('images' in previous, 'reference lacks image pins')
    if 'images' in current:
        require(current['images'] == previous['images'], 'compiler images differ')


def load(binding):
    ref = binding.get('baseline_reference')
    require(isinstance(ref, dict) and set(ref) == {'kind', 'output', 'binding_sha256',
            'run_sha256', 'evidence_sha256'} and ref['kind'] == KIND, 'invalid reference binding')
    output = Path(ref['output'])
    require(output.is_absolute() and output.resolve() == output and output.is_dir(), 'invalid reference directory')
    for name, key in [('binding.json', 'binding_sha256'), ('run.json', 'run_sha256'),
                      ('evidence.json', 'evidence_sha256')]:
        require(r.HEX.fullmatch(ref[key] or '') and c.sha(r.regular(output/name)) == ref[key],
                'historical metadata changed: ' + name)
    old = r.read_json(output/'binding.json'); state = r.read_json(output/'run.json')
    evidence = r.read_json(output/'evidence.json')
    contract(binding, old)
    require(state.get('status') == 'recovery_evidence_complete' and
            state.get('binding_sha256') == ref['binding_sha256'] and
            state.get('evidence_sha256') == ref['evidence_sha256'], 'reference run incomplete')
    require(set(state.get('modes', {})) == {'baseline_normal', 'baseline_lua',
            'candidate_normal', 'candidate_lua'}, 'reference run lacks complete modes')
    return output, old, state, evidence


def checkpoint(binding, label, mode, entry):
    require(label == 'baseline' and mode in ('normal', 'lua'), 'only baseline modes may be reused')
    output, old, state, evidence = load(binding)
    old_entry = state['modes']['baseline_'+mode]
    expected = {'kind': KIND, 'mode': mode, 'receipt_sha256': old_entry['receipt_sha256']}
    require(entry == expected, 'reference checkpoint changed')
    root = r.checkpoint(output, old, state['binding_sha256'], 'baseline', mode, old_entry)
    require(evidence['baseline'][mode] == {'root': str(root), 'receipt_sha256': entry['receipt_sha256']},
            'historical evidence root differs')
    return root


def entry(binding, mode):
    _, _, state, _ = load(binding)
    value = {'kind': KIND, 'mode': mode,
             'receipt_sha256': state['modes']['baseline_'+mode]['receipt_sha256']}
    checkpoint(binding, 'baseline', mode, value)
    return value


def configure(args, binding):
    source = getattr(args, 'reuse_baseline_from', None)
    if source is None:
        return
    source = Path(source).resolve(); destination = Path(args.output).resolve()
    require(not source.is_relative_to(destination) and not destination.is_relative_to(source),
            'reference and new output must be disjoint')
    binding['baseline_reference'] = {'kind': KIND, 'output': str(source),
        'binding_sha256': c.sha(r.regular(source/'binding.json')),
        'run_sha256': c.sha(r.regular(source/'run.json')),
        'evidence_sha256': c.sha(r.regular(source/'evidence.json'))}
    for mode in ('normal', 'lua'):
        entry(binding, mode)
