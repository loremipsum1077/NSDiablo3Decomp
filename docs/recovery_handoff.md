# Local source handoff to the recovery queue

`tools/recovery_handoff.py` removes the manual assembly of complete-owner targets,
tool hashes and queue manifests for an existing source candidate. It supports
local/in-chat preparation and makes no provider calls. It neither applies source
nor creates claims, reviews, builds or landing decisions.

Prepare and commit source in the owner's claimed worktree first. The primary
checkout must be clean at the explicit baseline; the candidate must descend from
it and change exactly the owner source and explicitly listed headers. The helper
uses the same original Git objects as receipt generation, ignoring replacement
refs. It checks live claims and coordination, both clean worktrees, input pins and
the Lua backend. An unrelated owner's claim change does not stale this handoff.
An owner marked `stuck` cannot be exported.

## Preparation request

The JSON request has `kind: "local-recovery-handoff-request-v1"` and these fields:

- `repo`, `baseline`, `candidate`, `owner`, `worktree`: canonical primary/worktree
  paths, full commits, and the original source owner without the `src/` prefix.
- `actor_label`: the source author's declared identity. This is self-reported,
  explicitly not authenticated identity or proof of independent review.
- `allowed_headers`: the sorted, exact list of changed repository-relative
  headers; use `[]` when the source needs no header edits.
- `evidence`: nonempty list of `{path, sha256}` pins for source preparation
  evidence. These pins authenticate bytes, not the correctness of their claims.
- `id`, `profile`, `integration_profile`, `output`, `seconds`: the queue job ID,
  `standard` or `named-writable` profile, `fresh-all` or
  `reuse-reviewed-baseline` integration, fresh receipt directory and time bound.
- `original_elf`, `checker`, `nss`: canonical `{path, sha256}` input pins.
- `lua_cache`, `lua_receipt_sha256`: the existing canonical backend directory and
  receipt hash; the helper also checks its recorded compiler binary hash.
- `dwarfdump`: canonical `{path, sha256}` reader pin, only for `named-writable`.

The helper derives targets from the baseline's real owner/function inventory.
It binds all tracked regular tools and records the changed source blobs. Export
and receipt directories must be outside inputs, backend caches, protected source
trees and registered non-primary worktrees. The export's parent must exist.

```sh
# Read-only identity/scope/input preflight; no output directory is created.
python3 tools/recovery_handoff.py prepare --request "$REQUEST" --output "$HANDOFF"

# Publish a new, read-only targets.json, manifest.json and handoff.json.
python3 tools/recovery_handoff.py prepare --request "$REQUEST" --output "$HANDOFF" --write
```

This preflight does not compile source or establish that an object's data,
initialization, emitted helpers or instruction bytes are admissible. Resolve
known source/object blockers before dispatching full builds. A source worker
must preserve a blocked draft and record the blocker instead of treating a
successfully assembled manifest as a successful decompilation. Failed or partial
exports are retained; do not overwrite them or silently repin old evidence.

For a candidate that introduces a vtable, use the available single-owner object
and a pinned baseline link to run `tools/dependency_preflight.py` before dispatch.
It can expose missing methods and existing function-extent debts without another
build. Candidate weak definitions remain selection-pending; the diagnostic is
not an acceptance gate and is not invoked automatically by this helper. See
[the actor-tag example and request format](ui_actor_recovery.md).

## Independent review handoff

The existing [queue](recovery_queue.md) remains the only persistent validation and
landing controller. Run its exported manifest through validation. Once its job
is `awaiting_review`, supply the hash-pinned handoff to the index command:

```sh
python3 tools/recovery_handoff.py review-index \
  --manifest "$HANDOFF/manifest.json" --state "$STATE" \
  --handoff "$HANDOFF/handoff.json" --handoff-sha256 "$HANDOFF_SHA" --job "$JOB"
```

The read-only index checks queue state and receipt metadata pins, compares the
candidate source blobs with the actual receipt binding, and exposes the author,
source evidence and review-template location. It emits `review_required` and
`receipt_verification: required_by_independent_reviewer`, with no decisions or
acceptance. It deliberately does not repeat the full raw audit just to route a
review request.

Assign a distinct reviewer to replay the actual receipts and complete every
existing review obligation. Submit that normal review and its hash to the queue.
Neither a preparation review nor an actor label substitutes for receipt review.
The existing landing gate still replays evidence and requires final artifact
reproduction. A changed baseline requires new preparation and review; no silent
rebasing or continuing under stale hashes occurs.

## Verified scope and remaining work

The initial implementation passed 11 isolated Git/CLI tests independently, plus
the 17 existing queue tests in the parent run. Tests cover explicit header scope,
stale or dirty source, input changes, claims/coordination, output isolation,
replacement refs, receipt/source mismatches, altered review metadata and false
approval claims. Fixture review indices test routing, not actual binary proof.

Automatic source-worker launch, reviewer assignment, dependency repair and
fresh-baseline preparation remain outside this helper. External campaign
artifacts are not accepted as local-session provenance; the exhausted external
pilot is unchanged. Completion still means exact original binary reproduction.
