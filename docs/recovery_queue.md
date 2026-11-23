# Queue prepared recovery work

`tools/recovery_queue.py` connects prepared source commits to the existing receipt,
independent-review and landing stages. It makes no provider calls, authors no
reviews, edits no source and creates no claims. Source workers still prepare the
candidate and an independent reviewer still supplies the bound review. This is a
durable dispatcher, not a claim that all remaining game code can run unattended.

For local/in-chat source workers, the [handoff helper](recovery_handoff.md) derives
complete-owner targets and pinned manifests from a clean claimed candidate, and
provides a separate read-only index for actual receipt review.

The first real completed job recovered four Touch input functions through this
queue, using four fresh review builds and two fresh integration builds. See
[Touch-input recovery](touch_recovery.md) for the landed commit, retained evidence
and explicit remaining exact-match debt.

The immutable format-1 manifest pins the primary repository, queue implementation,
tracked regular tool closure, original inputs, checker, targets, Lua backend
receipt, source commits and output directories. The known non-runtime
`tools/common` Git link is excluded from that regular-file closure; the executed
checker binary is pinned separately. Unsupported Git links and tracked symlinks
are rejected. Each job selects single-owner or grouped recovery and a standard
or single-owner named-writable profile. The latter also pins the DWARF reader.
An optional `integration_profile: "reuse-reviewed-baseline"` selects the
[six-build integration policy](recovery_only.md); the default is `"fresh-all"`.

The tool's help/docstring describes the manifest fields. Keep the manifest outside
the state directory, and keep state and receipt directories disjoint. State and
receipt outputs must also stay outside each pinned Lua cache. Start with
a clean primary checkout and the claimed candidate worktree(s) required by the
existing landing tools.

```sh
# Read-only readiness check; no state directory or process is created.
python3 tools/recovery_queue.py --manifest "$MANIFEST" --state "$STATE"

# Dispatch one ready validation action, then stop awaiting independent review.
python3 tools/recovery_queue.py --manifest "$MANIFEST" --state "$STATE" --run

# Supply the separate review and dispatch the existing guarded landing workflow.
python3 tools/recovery_queue.py --manifest "$MANIFEST" --state "$STATE" --run \
  --job "$JOB" --review "$REVIEW" --review-sha256 "$REVIEW_SHA"
```

Validation produces `awaiting_review`, never acceptance. The queue pins completed
receipt metadata and rechecks it before invoking landing. Landing itself verifies
all existing source, artifact, review, claim and integration gates. Only its real
landed record and matching primary HEAD can produce queue state `landed`.
Recovery remains uncertified and cannot grant O credit.

State is atomically written under an exclusive lock, with bounded subprocess
time, logs and actions. An interrupted validation or landing becomes `uncertain`;
failed validation is terminal. Neither is automatically retried. Inspect retained
process/container/transaction evidence before preparing replacement work. A stale
primary baseline becomes `needs_rebase`; the queue cannot rebase or reinterpret
an old review. In particular, a landing may have advanced HEAD before a later
command error, so an uncertain landing must be reconciled before any retry.

Multiple prepared jobs can be listed and `--max-actions` bounds dispatch. After
one lands, jobs pinned to the old baseline are parked rather than silently
rebased. Compatible source files can share a grouped job within the current
group policy. Broader new-data batching, automatic source preparation and model
budget authorization are separate capabilities.
