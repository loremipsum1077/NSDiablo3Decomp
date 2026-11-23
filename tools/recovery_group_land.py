"""Atomic, independently reviewed two-owner recovery integration."""
import csv
from pathlib import Path

import receipts as r
import recovery as single
import recovery_group as group
from validation import common as c

rows = single.rows
inventory = single.inventory


def land(args, main):
    """Hold host-wide build capacity through validation and fast-forward."""
    import resource_admission
    import docker_admission
    try:
        with resource_admission.lease():
            resource_admission.check(str((Path(main).resolve() / 'build' / 'recovery-land')))
            with docker_admission.session('recovery integration'):
                return _land(args, main)
    except resource_admission.AdmissionError as error:
        raise c.Halt(str(error)) from error


def _land(args, main):
    """Prepare and validate an integration commit away from the checked-out branch.

    Caller holds the existing land.lock. No reset, stash or worktree deletion.
    Git's fast-forward checkout checks protect user edits; failed attempts survive.
    """
    import shutil
    import subprocess
    import uuid
    main=Path(main).resolve(); output=Path(args.recovery_receipts).resolve()
    evidence=r.read_json(output/'evidence.json'); binding=r.read_json(output/'binding.json')
    if binding.get('policy') != 'recovery-group' or binding.get('post_landing'):
        raise c.Halt('Pre-landing recovery receipts required')
    owners = binding.get('owners')
    if not isinstance(owners, list) or len(owners) != 2 or len(set(owners)) != 2 or any(not isinstance(x, str) or not x for x in owners):
        raise c.Halt('Exactly two reviewed owners required')
    if len(args.files) != 2 or set(args.files) != set(owners):
        raise c.Halt('Name exactly the reviewed recovery owners')
    approved=group.review(output,args.review,args.review_sha256)
    base=binding['versions']['baseline']['commit']; candidate=binding['versions']['candidate']['commit']
    def git(*words):return r.git(main,*words).decode().strip()
    branch = git('rev-parse','--symbolic-full-name','HEAD')
    def unchanged_main():
        if git('rev-parse','--symbolic-full-name','HEAD')!=branch or git('rev-parse','HEAD')!=base or git('status','--porcelain'):
            raise c.Halt('Primary HEAD/work changed; preserve evidence and rebase/review')
    unchanged_main()
    r.verify(output,main,candidate,policy='recovery-group')
    import claim
    def check_claim_sources(claims):
        for owner in owners:
            if owner not in claims:
                raise c.Halt('Both reviewed owner claims are required: ' + owner)
            wt = claim.worktree(owner)
            if r.git(wt, 'rev-parse', 'HEAD').decode().strip() != candidate or r.git(wt, 'status', '--porcelain').strip():
                raise c.Halt('Claim source changed after review: ' + owner)
    with claim.locked() as claims:
        claim.check_coordination(main)
        original_claims = {owner: dict(claims[owner]) if owner in claims else None for owner in owners}
        check_claim_sources(claims)
    stage=main/'build/recovery-land'/uuid.uuid4().hex
    stage.mkdir(parents=True)
    checkout=stage/'integration'
    targets=stage/'targets.json';c.write(targets,binding['targets'],True)
    manifest=stage/'owners.json';c.write(manifest,{'format':1,'owners':owners},True)
    # Revalidate live input pins/toolchain recipe before any source mutation.
    words=['build','--mode','recovery-group','--repo',str(main),'--baseline',base,'--candidate',candidate,
           '--owners',str(manifest),'--original-elf',str(args.original_elf),'--original-sha256',binding['original_sha256'],
           '--checker',str(args.checker),'--checker-sha256',binding['checker_sha256'],
           '--lua-cache',str(args.lua_cache),'--lua-receipt-sha256',binding['lua_receipt_sha256'],
           '--nss',str(args.nss),'--nss-sha256',binding['nss_sha256'],'--targets',str(targets),
           '--output',str(stage/'post-receipts'),'--seconds',str(args.seconds)]
    build_args=r.parser().parse_args(words)
    current=r.context(build_args);group.configure(build_args,current)
    if current != {k:v for k,v in binding.items() if k!='images'} or r.image_ids()!=binding['images']:
        raise c.Halt('Live input/checker/config/toolchain lineage changed')
    r.git(main,'worktree','add','--detach',str(checkout),base)
    patch=r.git(main,'diff','--binary',base,candidate)
    (stage/'reviewed.patch').write_bytes(patch)
    shutil.copy2(args.review,stage/'independent-review.json')
    c.write(stage/'reviewed-binding.json', binding, True)
    subprocess.run(['git','-C',str(checkout),'apply','--index',str(stage/'reviewed.patch')],check=True)
    before=rows(main,base); after=[dict(x) for x in before]
    selected={(x['address'],x['name']) for x in binding['targets']['functions']}
    for row in after:
        if (row['Address'],row['Name']) in selected and row['Quality']=='U':row['Quality']='M'
    inventory(before,after,binding['targets']['functions'],True)
    table=checkout/'data/2.6.2/functions.csv'
    with table.open('w',newline='') as stream:
        writer=csv.DictWriter(stream,fieldnames=list(before[0]),lineterminator='\n');writer.writeheader();writer.writerows(after)
    r.git(checkout,'add','-u','--','data/2.6.2/functions.csv')
    r.git(checkout,'commit','-m','Recover '+', '.join(owners)+' as uncertified source',
          '-m','Recovery-group; no exact-match credit. Review SHA256: '+args.review_sha256)
    integrated=r.git(checkout,'rev-parse','HEAD').decode().strip()
    remaining = r.git(main, 'diff', '--no-renames', '--name-only', '-z', candidate, integrated).decode().split('\0')[:-1]
    if (r.committed_files(main,integrated,r.SOURCE)!=binding['versions']['candidate']['source'] or
            any(path != 'data/2.6.2/functions.csv' for path in remaining)):
        raise c.Halt('Integration patch differs from reviewed patch')
    c.write(stage/'transaction.json',{'kind':'recovery-group-v1','baseline':base,'reviewed_candidate':candidate,
            'integration':integrated,'review':approved,'claims':original_claims,'state':'postintegration_validation_pending'},True)
    build_args.candidate=integrated;build_args.post_landing=True;build_args.run=True
    r.build(build_args)
    post=stage/'post-receipts';r.verify(post,main,integrated,policy='recovery-group')
    # Fresh final links and every retained object must reproduce reviewed artifacts.
    final=r.read_json(post/'evidence.json')
    for label in ('baseline','candidate'):
        for mode in ('normal','lua'):
            old=Path(evidence[label][mode]['root']);new=Path(final[label][mode]['root'])
            def artifacts(root):
                return {str(p.relative_to(root)):c.sha(p) for p in root.rglob('*') if p.is_file() and
                    (p.relative_to(root).parts[0]=='objects' or str(p.relative_to(root)) in
                     ('build/2.6.2/d3','build/2.6.2/d3.map','build/2.6.2/d3.map.provenance','result.json'))}
            if artifacts(old)!=artifacts(new):raise c.Halt('Fresh integration artifacts differ; new independent review required')
    with claim.locked() as claims:
        claim.check_coordination(main)
        group.review(output,args.review,args.review_sha256)
        build_args.candidate = candidate; build_args.post_landing = False
        current = r.context(build_args); group.configure(build_args, current)
        if current != {k:v for k,v in binding.items() if k != 'images'} or r.image_ids() != binding['images']:
            raise c.Halt('Live input/checker/config/toolchain lineage changed during validation')
        if any(claims.get(owner) != original_claims[owner] for owner in owners):
            raise c.Halt('Claim ownership changed during validation')
        check_claim_sources(claims)
        unchanged_main()
        # Preserve rollback identity before a fast-forward. No destructive rollback on failure.
        r.git(main,'update-ref','refs/recovery-preserved/'+stage.name,base)
        r.git(main,'merge','--ff-only',integrated)
        if git('rev-parse','HEAD')!=integrated:
            raise c.Halt('Unexpected integration HEAD; inspect retained transaction')
    c.write(stage/'landed.json',{'kind':'recovery-group-v1','status':'recovery_group_landed','strict_certified':False,
            'commit':integrated,'review_sha256':args.review_sha256,'post_receipts':str(post)},True)
    return {'status':'recovery_group_landed','strict_certified':False,'commit':integrated,'retained':str(stage)}
