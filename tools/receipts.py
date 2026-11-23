#!/usr/bin/env python3
"""Produce private, provider-free source/both-mode build receipts from trusted commits.

Build is read-only preflight unless --run is supplied. Existing images/cache/checker
are required; nothing is installed/downloaded. Verify only rechecks retained bytes
and never claims a fresh build, source acceptance, independent review or landing.
"""
import argparse
import fcntl
import json
import os
from pathlib import Path, PurePosixPath
import re
import shutil
import subprocess
import time
import resource_admission
import docker_admission

from validation import common as c, integration
from data_provenance import verify_link_provenance

from game_data_manifest import MANIFEST, generated_files

SOURCE = ('src', 'include', 'toolchain', 'CMakeLists.txt', MANIFEST)
SUPPORT = ('tools/config.toml', 'data/2.6.2/functions.csv',
           'data/2.6.2/function_files.csv', 'data/2.6.2/data_symbols.csv')
HEX = re.compile(r'[0-9a-f]{64}\Z')
PENDING = ['independent_source_review', 'object_relocations', 'data', 'emitted_extras',
           'unwind', 'postintegration_validation']


def regular(path):
    path = Path(path)
    if path.is_symlink() or not path.is_file():
        raise c.Halt('Regular input file required: ' + str(path))
    return path


def inside(root, name):
    if not isinstance(name, str) or not name or '\\' in name or ':' in name or '\0' in name:
        raise c.Halt('Invalid retained path')
    path = PurePosixPath(name)
    if path.is_absolute() or any(part in ('', '.', '..') for part in name.split('/')):
        raise c.Halt('Retained path escapes output')
    result = Path(root)
    for part in path.parts:
        result = result / part
        if result.is_symlink():
            raise c.Halt('Retained symlink refused')
    return result


def read_json(path):
    path = regular(path)
    if path.stat().st_size > 8 * 1024 * 1024:
        raise c.Halt('Metadata exceeds the bounded reader limit')
    def unique(pairs):
        value = {}
        for name, item in pairs:
            if name in value:
                raise c.Halt('Duplicate metadata field')
            value[name] = item
        return value
    return json.loads(path.read_text(), object_pairs_hook=unique)


def git(repo, *args):
    return subprocess.check_output(['git', '--no-replace-objects', '-C', str(repo), *args])


def committed_files(repo, revision, paths):
    if not re.fullmatch(r'[0-9a-f]{40}', revision) or git(repo, 'cat-file', '-t', revision).strip() != b'commit':
        raise c.Halt('Exact local forty-character commit required')
    files = {}
    for item in git(repo, 'ls-tree', '-rz', revision, '--', *paths).split(b'\0'):
        if not item:
            continue
        metadata, raw_name = item.split(b'\t', 1)
        mode, kind, oid = metadata.split()
        name = raw_name.decode()
        if mode not in (b'100644', b'100755') or kind != b'blob':
            raise c.Halt('Only committed regular source files supported: ' + name)
        p = PurePosixPath(name)
        if p.is_absolute() or '..' in p.parts:
            raise c.Halt('Invalid committed path')
        files[name] = {'object': oid.decode(), 'sha256': __import__('hashlib').sha256(git(repo, 'cat-file', 'blob', oid.decode())).hexdigest(),
                       'executable': mode == b'100755'}
    if paths == SOURCE and MANIFEST in files:
        pins = json.loads(git(repo, 'cat-file', 'blob', files[MANIFEST]['object']))
        for name, expected in generated_files(pins).items():
            if name in files:
                raise c.Halt('Private generated input is tracked: ' + name)
            files[name] = {'generated': True, 'sha256': expected, 'executable': False}
    if not files:
        raise c.Halt('Empty committed source/input set')
    return files


def export(repo, files, root):
    root.mkdir(parents=True)
    for name, entry in files.items():
        path = root / name
        path.parent.mkdir(parents=True, exist_ok=True)
        if entry.get('generated'):
            local = regular(inside(Path(repo), name))
            if c.sha(local) != entry['sha256']:
                raise c.Halt('Local generated input differs from committed pin: ' + name)
            shutil.copyfile(local, path)
        else:
            path.write_bytes(git(repo, 'cat-file', 'blob', entry['object']))
        if entry['executable']:
            path.chmod(0o755)
        if c.sha(path) != entry['sha256']:
            raise c.Halt('Source changed during export')


def source_check(root, files):
    if root.is_symlink() or not root.is_dir():
        raise c.Halt('Regular source snapshot directory required')
    paths = [p for p in root.rglob('*') if p.relative_to(root).parts[0] != 'build']
    if any(p.is_symlink() for p in paths):
        raise c.Halt('Source snapshot symlink')
    actual = {p.relative_to(root).as_posix(): c.sha(p) for p in paths if p.is_file()}
    if actual != {name: item['sha256'] for name, item in files.items()}:
        raise c.Halt('Source snapshot changed')


def image_ids():
    result = {}
    for mode, tag in (('normal', 'd3-build:5.0.1'), ('lua', 'd3-lua-build:5.0.1')):
        value = subprocess.check_output(['docker', 'image', 'inspect', '--format', '{{.Id}}', tag], text=True).strip()
        if not re.fullmatch(r'sha256:[0-9a-f]{64}', value):
            raise c.Halt('Existing immutable Docker image ID required')
        result[mode] = value
    return result


def context(args):
    for path, expected in ((args.original_elf, args.original_sha256), (args.checker, args.checker_sha256),
                           (args.lua_cache / 'receipt.json', args.lua_receipt_sha256)):
        if not HEX.fullmatch(expected) or c.sha(regular(path)) != expected:
            raise c.Halt('Input pin mismatch: ' + str(path))
    lua_receipt = read_json(args.lua_cache / 'receipt.json')
    binary = regular(args.lua_cache / 'out/bin/llc')
    if c.sha(binary) != lua_receipt.get('binary_sha256'):
        raise c.Halt('Lua backend does not match its existing receipt')
    targets = read_json(args.targets)
    if not isinstance(targets, dict) or set(targets) != {'format', 'functions'} or type(targets['format']) is not int or targets['format'] != 1 or not isinstance(targets['functions'], list) or not 1 <= len(targets['functions']) <= 64:
        raise c.Halt('Bounded target manifest required')
    selected = set()
    for row in targets['functions']:
        if not isinstance(row, dict) or set(row) != {'address', 'name'} or not isinstance(row['address'], str) or not re.fullmatch(r'0x[0-9a-fA-F]+', row['address']) or not isinstance(row['name'], str) or not row['name'].strip():
            raise c.Halt('Exact target address/name required')
        identity = (row['address'], row['name'])
        if identity in selected:
            raise c.Halt('Duplicate target identity')
        selected.add(identity)
    versions = {}
    for label, revision in (('baseline', args.baseline), ('candidate', args.candidate)):
        source = committed_files(args.repo, revision, SOURCE)
        if 'CMakeLists.txt' not in source:
            raise c.Halt('Committed CMake recipe required')
        support = committed_files(args.repo, revision, SUPPORT)
        if set(support) != set(SUPPORT):
            raise c.Halt('Complete committed checker inputs required')
        import csv, io
        inventory = list(csv.DictReader(io.StringIO(git(args.repo, 'show', revision + ':data/2.6.2/functions.csv').decode())))
        try:
            known = integration.identities(inventory, 'Address', 'Name')
        except ValueError as error:
            raise c.Halt('Invalid committed inventory identities') from error
        if not selected <= known:
            raise c.Halt('Target absent from committed inventory')
        # Validate the backend against each exact committed recipe before export/build.
        from backends import backend
        here = 'toolchain/lua/'
        try:
            expected_recipe = {name: source[(here + name).replace('lua/../', '')]['sha256']
                               for name in backend.RECIPE}
            expected_config = json.loads(git(args.repo, 'show', revision + ':toolchain/lua/config.json'))
            if source[here + 'llvm-5.0.1.patch']['sha256'] != expected_config['patch_sha256']:
                raise c.Halt('Committed Lua patch mismatch')
            backend.validate(args.repo, args.lua_cache,
                             {'recipe': expected_recipe, 'config': expected_config})
        except (KeyError, ValueError, backend.BackendError) as error:
            raise c.Halt('Lua backend stale/corrupted for ' + label + ': ' + str(error)) from error
        versions[label] = {'commit': revision, 'source': source, 'support': support}
    folder = Path(__file__).with_name('validation')
    return {'format': 1, 'versions': versions, 'targets': targets,
            'original_sha256': args.original_sha256, 'checker_sha256': args.checker_sha256,
            'lua_receipt_sha256': args.lua_receipt_sha256, 'lua_backend_sha256': c.sha(binary),
            'tools': {p.name: c.sha(p) for p in sorted(folder.glob('*.py'))} | {
                name: c.sha(Path(__file__).with_name(name)) for name in ('receipts.py', 'game_data_manifest.py', 'data_provenance.py', 'resource_admission.py', 'docker_admission.py')}}


def retained_mode(root, policy="strict", mode=None):
    done = regular(root / 'done.json')
    receipt = read_json(done)
    if policy in ('recovery-only', 'recovery-group'):
        if (set(receipt) != {'kind', 'mode', 'full_check_executed', 'files'} or
                receipt['kind'] != 'recovery-only-mode-v1' or receipt['mode'] != mode or
                receipt['full_check_executed'] is not False or not receipt['files']):
            raise c.Halt('Wrong or incomplete recovery receipt')
    elif set(receipt) != {'full_check_rc', 'files'} or type(receipt['full_check_rc']) is not int or receipt['full_check_rc'] != 0 or not receipt['files']:
        raise c.Halt('Incomplete strict mode receipt')
    for name, expected in receipt['files'].items():
        if not HEX.fullmatch(expected) or c.sha(regular(inside(root, name))) != expected:
            raise c.Halt('Retained artifact changed')
    if any(name not in receipt['files'] for name in ('result.json', 'build/2.6.2/d3',
               'build/2.6.2/d3.map', 'build/2.6.2/d3.map.provenance')):
        raise c.Halt('Missing bound build/check artifacts')
    if not any(name.startswith('objects/') and name.endswith('.obj') for name in receipt['files']):
        raise c.Halt('Missing retained object evidence')
    verify_link_provenance(root / 'build/2.6.2/d3')
    return c.sha(done)


def support_check(root, binding, version):
    expected = {name: item['sha256'] for name, item in version['support'].items()}
    if set(expected) != set(SUPPORT):
        raise c.Halt('Incomplete attempt support binding')
    expected.update({'tools/check': binding['checker_sha256'],
                     'data/2.6.2/main.elf': binding['original_sha256']})
    if root.is_symlink() or not root.is_dir():
        raise c.Halt('Regular attempt support directory required')
    paths = list(root.rglob('*'))
    if any(path.is_symlink() for path in paths):
        raise c.Halt('Attempt support symlink')
    actual = {path.relative_to(root).as_posix(): c.sha(path) for path in paths if path.is_file()}
    if actual != expected:
        raise c.Halt('Attempt support inputs changed')


def attempt_binding(binding, binding_sha256, label, mode):
    version = binding['versions'][label]
    image = binding['images'][mode]
    if not re.fullmatch(r'sha256:[0-9a-f]{64}', image):
        raise c.Halt('Invalid immutable attempt image')
    return {'format': 1, 'run_sha256': binding_sha256, 'version': label, 'mode': mode,
            'commit': version['commit'], 'image_id': image,
            'source_files_sha256': c.digest(version['source']),
            'support_files_sha256': c.digest(version['support']),
            'checker_sha256': binding['checker_sha256'],
            'original_sha256': binding['original_sha256'],
            'lua_receipt_sha256': binding['lua_receipt_sha256'],
            'lua_backend_sha256': binding['lua_backend_sha256']}


def checkpoint(output, binding, binding_sha256, label, mode, entry):
    if isinstance(entry, dict) and 'kind' in entry:
        import recovery_baseline_reference
        return recovery_baseline_reference.checkpoint(binding, label, mode, entry)
    if not isinstance(entry, dict) or set(entry) != {'root', 'receipt_sha256', 'attempt_binding_sha256'}:
        raise c.Halt('Invalid mode checkpoint binding')
    if not isinstance(entry['root'], str) or not re.fullmatch(
            re.escape(label + '/' + mode) + r'/attempt-[0-9]{4,}/receipt', entry['root']):
        raise c.Halt('Mode checkpoint path does not match version/mode')
    receipt = inside(output, entry['root'])
    attempt = receipt.parent
    if int(attempt.name.split('-')[1]) < 1:
        raise c.Halt('Invalid attempt identity')
    proof = regular(attempt / 'binding.json')
    if (c.sha(proof) != entry['attempt_binding_sha256'] or
            read_json(proof) != attempt_binding(binding, binding_sha256, label, mode)):
        raise c.Halt('Attempt version/mode/source/input/image binding changed')
    version = binding['versions'][label]
    source = inside(output, attempt.relative_to(output).as_posix() + '/source')
    source_check(source, version['source'])
    support_check(inside(output, attempt.relative_to(output).as_posix() + '/support'), binding, version)
    if retained_mode(receipt, binding.get('policy', 'strict'), mode) != entry['receipt_sha256']:
        raise c.Halt('Completed checkpoint changed')
    done = read_json(receipt / 'done.json')
    for name in ('configure.command.json', 'build.command.json'):
        if name not in done['files']:
            raise c.Halt('Missing bound attempt command')
        command = read_json(regular(receipt / name))
        if not isinstance(command, list) or not all(isinstance(word, str) for word in command):
            raise c.Halt('Invalid attempt command')
        try:
            cmake = command.index('cmake')
        except ValueError as error:
            raise c.Halt('Missing CMake attempt command') from error
        mount = 'type=bind,src=' + str(source) + ',dst=/work,readonly'
        if command[:2] != ['docker', 'run'] or cmake < 3 or command[cmake - 1] != binding['images'][mode] or mount not in command:
            raise c.Halt('Attempt command image/source identity changed')
        if name == 'configure.command.json':
            selector = '-DD3_LUA_COMPILER=' + ('ON' if mode == 'lua' else 'OFF')
            selectors = [word for word in command if word.startswith('-DD3_LUA_COMPILER=')]
            if selectors != [selector]:
                raise c.Halt('Attempt command compiler mode changed')
        elif '--build' not in command[cmake + 1:]:
            raise c.Halt('Missing attempt build command')
    return receipt


def verify(output, repo=None, candidate=None, policy="strict"):
    output = Path(output).resolve()
    binding = read_json(regular(output / 'binding.json'))
    state = read_json(regular(output / 'run.json'))
    if binding.get('policy', 'strict') != policy:
        raise c.Halt('Receipt policy mismatch; recovery is not exact certification')
    if state['binding_sha256'] != c.sha(output / 'binding.json'):
        raise c.Halt('Run binding changed')
    if set(binding['versions']) != {'baseline', 'candidate'} or set(state.get('modes', {})) != {
            'baseline_normal', 'baseline_lua', 'candidate_normal', 'candidate_lua'}:
        raise c.Halt('Missing or stale retained mode')
    if (repo is None) != (candidate is None):
        raise c.Halt('--repo and --candidate must be supplied together')
    if repo is not None:
        commit = git(repo, 'rev-parse', '--verify', candidate + '^{commit}').decode().strip()
        version = binding['versions']['candidate']
        if (version['commit'] != commit or version['source'] != committed_files(repo, commit, SOURCE)
                or version['support'] != committed_files(repo, commit, SUPPORT)):
            raise c.Halt('Retained candidate does not match requested landed commit')
    expected_evidence = {'format': 1, 'toolchain': {}}
    if policy in ('recovery-only', 'recovery-group'):
        expected_evidence['kind'] = policy + '-evidence-v1'
    for label, version in binding['versions'].items():
        source_check(inside(output, label + '/source'), version['source'])
        source_manifest = regular(output / label / 'source.json')
        if read_json(source_manifest) != {'files': {name: item['sha256'] for name, item in version['source'].items()}}:
            raise c.Halt('Source manifest changed')
        expected_evidence[label] = {'commit': version['commit'], 'source': {
            'root': str(output / label / 'source'), 'manifest': str(source_manifest), 'sha256': c.sha(source_manifest)}}
        for mode in ('normal', 'lua'):
            entry = state.get('modes', {}).get(label + '_' + mode)
            if not entry:
                raise c.Halt('Missing or stale retained mode')
            root = checkpoint(output, binding, state['binding_sha256'], label, mode, entry)
            expected_evidence[label][mode] = {'root': str(root), 'receipt_sha256': entry['receipt_sha256']}
    evidence = regular(output / 'evidence.json')
    if state.get('evidence_sha256') != c.sha(evidence) or state.get('status') != ('recovery_evidence_complete' if policy in ('recovery-only', 'recovery-group') else 'complete'):
        raise c.Halt('Incomplete or changed evidence manifest')
    if read_json(evidence) != expected_evidence:
        raise c.Halt('Evidence manifest does not match bound source/mode identities')
    if policy == 'recovery-only':
        import recovery
        recovery.verify_audit(output, binding, state)
    elif policy == 'recovery-group':
        import recovery_group
        recovery_group.verify_audit(output, binding, state)
    return {'status': 'retained_bytes_verified', 'policy': policy, 'fresh_build_executed': False,
            'accepted': False, 'pending_gates': PENDING, 'provider_calls': 0}


def build(args, builder=None):
    """Serialize cooperating build sessions; read-only preflight takes no lease."""
    mode = getattr(args, 'mode', 'strict')
    if ((getattr(args, 'dwarfdump', None) is not None or getattr(args, 'dwarfdump_sha256', None) is not None)
            and mode != 'recovery-only'):
        raise c.Halt('Pinned DWARF reader requires single-owner recovery')
    if getattr(args, 'reuse_baseline_from', None) is not None and mode != 'recovery-only':
        raise c.Halt('Baseline references require single-owner recovery integration')
    if not args.run:
        return _build(args, builder)
    try:
        with resource_admission.lease():
            resource_admission.check(str(args.output.absolute()))
            with docker_admission.session('receipt build'):
                return _build(args, builder)
    except resource_admission.AdmissionError as error:
        raise c.Halt(str(error)) from error


def _build(args, builder=None):
    builder = builder or integration.build_check
    args.repo = args.repo.resolve()
    if args.lua_cache is None:
        from backends import backend
        args.lua_cache = backend.resolve(args.repo)
    for field in ('original_elf', 'checker', 'targets', 'lua_cache'):
        setattr(args, field, getattr(args, field).absolute())
    binding = context(args)
    mode_policy = getattr(args, 'mode', 'strict')
    recovery_mode = mode_policy in ('recovery-only', 'recovery-group')
    if getattr(args, 'reuse_baseline_from', None) is not None and mode_policy != 'recovery-only':
        raise c.Halt('Baseline references require single-owner recovery integration')
    if recovery_mode:
        import recovery
        if mode_policy == 'recovery-group':
            import recovery_group as recovery_policy
        else:
            recovery_policy = recovery
        recovery_policy.configure(args, binding)
    if not args.run:
        if args.resume:
            raise c.Halt('--resume requires --run')
        try:
            assessment = resource_admission.assess(
                [{'path': str(args.output.absolute()), 'bytes': resource_admission.DEFAULT_GROWTH}],
                resource_admission.DEFAULT_RESERVE)
        except (OSError, RuntimeError, ValueError) as error:
            raise c.Halt('Cannot determine resource availability: ' + str(error)) from error
        return {'status': 'preflight', 'writes': 0, 'provider_calls': 0,
                'binding_sha256': c.digest(binding), 'accepted': False,
                'resource_assessment': assessment}
    from backends import check
    errors = check(args.repo, cache=args.lua_cache)
    if errors:
        raise c.Halt('\n'.join(errors))
    binding['images'] = image_ids()
    output = args.output.resolve()
    if output == args.repo or any(output.is_relative_to(args.repo / name) for name in ('src', 'include', 'toolchain', 'data', 'tools')) or output.is_relative_to(args.lua_cache.resolve()):
        raise c.Halt('Output must be isolated from source/tool/input/cache trees')
    if output.exists() and not args.resume:
        raise c.Halt('Output exists; inspect it or use explicit --resume')
    if not output.exists() and args.resume:
        raise c.Halt('Resume output does not exist')
    output.mkdir(parents=True, exist_ok=True)
    with inside(output, '.lock').open('a') as lock:
        fcntl.flock(lock, fcntl.LOCK_EX | fcntl.LOCK_NB)
        if (output / 'binding.json').exists():
            if read_json(output / 'binding.json') != binding:
                raise c.Halt('Source/tool/input/image binding changed; use a new output')
        else:
            c.write(output / 'binding.json', binding, True)
        state = read_json(output / 'run.json') if (output / 'run.json').exists() else {
            'format': 1, 'status': 'in_progress', 'accepted': False, 'provider_calls': 0,
            'binding_sha256': c.sha(output / 'binding.json'), 'modes': {}, 'pending_gates': PENDING}
        if state['binding_sha256'] != c.sha(output / 'binding.json'):
            raise c.Halt('Run binding changed')
        allowed = {label + '_' + mode: (label, mode) for label in ('baseline', 'candidate') for mode in ('normal', 'lua')}
        if not isinstance(state.get('modes'), dict) or set(state['modes']) - set(allowed):
            raise c.Halt('Invalid saved mode checkpoints')
        # Authenticate every completed checkpoint before writing state or starting
        # any new build. Never reinterpret Lua outputs as the normal mode.
        for key, entry in state['modes'].items():
            label, mode = allowed[key]
            checkpoint(output, binding, state['binding_sha256'], label, mode, entry)
        c.write(output / 'run.json', state)
        deadline = time.monotonic() + args.seconds
        built_this_invocation = 0
        evidence = {'format': 1, 'toolchain': {}}
        if recovery_mode:
            evidence['kind'] = mode_policy + '-evidence-v1'
        for label, version in binding['versions'].items():
            source = inside(output, label + '/source')
            manifest = output / label / 'source.json'
            if not source.exists():
                export(args.repo, version['source'], source)
                c.write(manifest, {'files': {name: item['sha256'] for name, item in version['source'].items()}}, True)
            source_check(source, version['source'])
            if read_json(manifest) != {'files': {name: item['sha256'] for name, item in version['source'].items()}}:
                raise c.Halt('Source manifest changed')
            item = {'commit': version['commit'], 'source': {'root': str(source), 'manifest': str(manifest), 'sha256': c.sha(manifest)}}
            for mode in ('normal', 'lua'):
                key = label + '_' + mode
                if key in state['modes']:
                    entry = state['modes'][key]
                elif label == 'baseline' and 'baseline_reference' in binding:
                    import recovery_baseline_reference
                    entry = recovery_baseline_reference.entry(binding, mode)
                    state['modes'][key] = entry
                    c.write(output/'run.json', state)
                else:
                    # An estimate cannot reserve capacity against external writers.
                    # Recheck available space before each new retained attempt.
                    resource_admission.check(str(output), growth_bytes=resource_admission.ATTEMPT_GROWTH)
                    parent = output / label / mode
                    parent.mkdir(parents=True, exist_ok=True)
                    # Partial attempts are retained, never resumed as successful outputs.
                    attempt = parent / ('attempt-%04d' % (1 + len(list(parent.glob('attempt-*')))))
                    attempt.mkdir()
                    build_source, support, receipt = attempt / 'source', attempt / 'support', attempt / 'receipt'
                    shutil.copytree(source, build_source)
                    export(args.repo, version['support'], support)
                    (support / 'tools').mkdir(exist_ok=True)
                    shutil.copy2(args.checker, support / 'tools/check')
                    shutil.copy2(args.original_elf, support / 'data/2.6.2/main.elf')
                    c.write(attempt / 'binding.json', attempt_binding(binding, state['binding_sha256'], label, mode), True)
                    source_check(build_source, version['source'])
                    support_check(support, binding, version)
                    options = {'recovery_targets': binding['targets']['functions']} if recovery_mode else {}
                    builder(build_source, support, receipt, mode, deadline,
                            'd3-receipt-' + c.digest({'binding': binding, 'output': str(output)})[:12]
                            + '-' + label + '-' + mode + '-' + attempt.name.rsplit('-', 1)[-1],
                            images=binding['images'], lua_cache=args.lua_cache, **options)
                    built_this_invocation += 1
                    source_check(build_source, version['source'])
                    current = context(args)
                    if recovery_mode:
                        recovery_policy.configure(args, current)
                    if current != {k: v for k, v in binding.items() if k != 'images'}:
                        raise c.Halt('Inputs/tools changed during build')
                    entry = {'root': receipt.relative_to(output).as_posix(), 'receipt_sha256': retained_mode(receipt, binding.get('policy', 'strict'), mode),
                             'attempt_binding_sha256': c.sha(attempt / 'binding.json')}
                    checkpoint(output, binding, state['binding_sha256'], label, mode, entry)
                    state['modes'][key] = entry
                    c.write(output / 'run.json', state)
                root = checkpoint(output, binding, state['binding_sha256'], label, mode, entry)
                item[mode] = {'root': str(root), 'receipt_sha256': entry['receipt_sha256']}
            evidence[label] = item
        targets = [{'Address': row['address'], 'Name': row['name']} for row in binding['targets']['functions']]
        for mode in ('normal', 'lua'):
            old = read_json(Path(evidence['baseline'][mode]['root']) / 'result.json')
            new = read_json(Path(evidence['candidate'][mode]['root']) / 'result.json')
            if recovery_mode:
                from validation.recovery_diagnostics import CallFailureEvidence
                roots = {label: Path(evidence[label][mode]['root']) for label in ('baseline', 'candidate')}
                inventory = recovery.rows(args.repo, args.baseline)
                function_got = None
                if mode_policy == 'recovery-only':
                    from validation.recovery_function_got import FunctionGOTEvidence
                    selected = {(x['address'], x['name']) for x in binding['targets']['functions']}
                    selected_rows = [x for x in inventory if (x['Address'], x['Name']) in selected]
                    function_got = FunctionGOTEvidence(roots, args.original_elf, args.nss,
                                                      selected_rows, args.owner)
                recovery.compare(old, new, binding['targets']['functions'],
                    {(x['Address'],x['Name']) for x in inventory if x['Quality']=='U'},
                    CallFailureEvidence(roots), function_got)
            elif not integration.compare(old, new, targets):
                raise c.Halt('Both-mode identity preservation or selected target gate failed')
        c.write(output / 'evidence.json', evidence)
        if recovery_mode:
            recovery_policy.audit(args, output, binding, evidence)
            audit_name = 'recovery-group-audit.json' if mode_policy == 'recovery-group' else 'recovery-audit.json'
            state['audit_sha256'] = c.sha(output / audit_name)
        state.update(status='recovery_evidence_complete' if recovery_mode else 'complete', evidence_sha256=c.sha(output / 'evidence.json'),
                     construction='trusted_commands_observed; external reviewers must independently reproduce')
        if 'baseline_reference' in binding:
            state['construction'] = 'fresh candidate commands observed; historical baseline references reverified'
            state['reused_baseline_modes'] = ['normal', 'lua']
        c.write(output / 'run.json', state)
        return {'status': state['status'], 'policy': binding.get('policy', 'strict'), 'fresh_build_executed': built_this_invocation > 0,
                'accepted': False, 'pending_gates': PENDING, 'provider_calls': 0,
                'evidence': str(output / 'evidence.json'), 'evidence_sha256': state['evidence_sha256'],
                'fresh_build_count': built_this_invocation,
                'reused_baseline_count': 2 if 'baseline_reference' in binding else 0}


def parser():
    p = argparse.ArgumentParser(description=__doc__)
    commands = p.add_subparsers(dest='command', required=True)
    run = commands.add_parser('build', help='read-only preflight; --run explicitly performs trusted CPU builds')
    run.add_argument('--repo', type=Path, required=True)
    for name in ('baseline', 'candidate', 'original-sha256', 'checker-sha256', 'lua-receipt-sha256'):
        run.add_argument('--' + name, required=True)
    run.add_argument('--lua-cache', type=Path, help='exact cache; defaults to current recipe cache')
    for name in ('original-elf', 'checker', 'targets', 'output'):
        run.add_argument('--' + name, type=Path, required=True)
    run.add_argument('--mode', choices=('strict', 'recovery-only', 'recovery-group'), default='strict')
    run.add_argument('--owner', help='Recovery mode: exact bounded whole owner')
    run.add_argument('--owners', type=Path, help='Group mode: exact two-owner JSON manifest')
    run.add_argument('--nss', type=Path)
    run.add_argument('--nss-sha256')
    run.add_argument('--dwarfdump', type=Path, help='Pinned local DWARF reader for named writable-data recovery')
    run.add_argument('--dwarfdump-sha256')
    run.add_argument('--reuse-baseline-from', type=Path, help='Integration only: reverify historical reviewed baseline modes')
    run.add_argument('--post-landing', action='store_true', help='Recovery: permit only selected U to M inventory updates')
    run.add_argument('--run', action='store_true')
    run.add_argument('--resume', action='store_true')
    run.add_argument('--seconds', type=int, default=1800)
    check = commands.add_parser('verify', help='verify retained artifacts only; never a fresh-build claim')
    check.add_argument('--output', type=Path, required=True)
    check.add_argument('--mode', choices=('strict', 'recovery-only', 'recovery-group'), default='strict')
    check.add_argument('--repo', type=Path, help='bind retained post-landing evidence to this repository')
    check.add_argument('--candidate', help='exact landed commit; requires --repo')
    return p


def main(argv=None):
    p = parser()
    args = p.parse_args(argv)
    try:
        if args.command == 'build' and not 1 <= args.seconds <= 86400:
            raise c.Halt('--seconds must be between 1 and 86400')
        result = verify(args.output, args.repo, args.candidate, args.mode) if args.command == 'verify' else build(args)
        print(json.dumps(result, sort_keys=True))
        return 0
    except (c.Halt, OSError, ValueError, TypeError, AttributeError, KeyError, subprocess.CalledProcessError) as error:
        print(json.dumps({'status': 'failed', 'accepted': False, 'error': str(error), 'provider_calls': 0}))
        return 1


if __name__ == '__main__':
    raise SystemExit(main())
