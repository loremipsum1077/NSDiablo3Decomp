# P06 shared-helper prerequisite map

Tooling note (2026-10-19): this map records the P06 audit as performed
with the experimental `tools/swarm` verifier and test suite. That workflow has
since been removed; the commands below cover retained tools.

Current completion update (2026-10-19): P06-F04, F05 and F07 have landed original consumer proofs with full linked checking and no prior-O regressions.

P06 audits the evidence used to choose foundation work. It does not complete the
foundations themselves. The audit uses committed inventory at
`4cd7918255a0f4b66958bdb8ae52ae3fdd7bad18`; P02's claim reconciliation is complete
at this baseline. Existing ownership remains in force for all follow-up work.

## Findings

The complete cached index contains **22,298 recorded helper families** with
definition records. **12,316** have at least one strictly O-quality caller;
**9,982** have none. **904** families previously appeared evidenced solely because
the ledger accepted M/m nonmatches. Those families now remain open. Four families
with at least 100 callers lack exact evidence: current-power lookup and three
protobuf repeated-field operations. Their caller sets overlap: the union contains
552 functions, not the sum of the four fan-ins and not an expected matching gain.

The ranking is an exhaustive index scan with bounded, inspectable examples, not
an exhaustive source-body review. Fourteen indexed keys lack definition records
and are explicitly counted as omitted. The versioned
JSON snapshot recorded the inventory revision,
inventory Git blob IDs, cache SHA-256, ranking rules, named caller witnesses and
the highest 25 entries in each queue. The hash identified the existing cache;
it did not independently validate its original DWARF provenance.

The consumer audit inspected the highest-impact
unproved groups against actual reconstructed and reference source. The
independent proof audit reviewed 11 existing caller
witnesses, definition locations and limits. Together they distinguished missing
context fields, declarations without bodies, existing generic bodies, original
SDK ownership aliases and unresolved specialization evidence.

## Scoped recovery jobs

Each job below names original definitions, concrete caller symbols and addresses,
linked source jobs, ownership constraints and acceptance criteria in
`config/project_jobs.json`.

| Job | Definition / consumer scope | Required result |
|---|---|---|
| P06-F01 | PowersSwig current-power context | Recover original context access and prove a bounded real wrapper caller. |
| P06-F02 | Buff indexed access and `PowerAPIBuffStartData::IsInitialized` | Recover buff-list layout and initialization semantics with a real caller. |
| P06-F03 | Cursor indexed access | Recover cursor-list storage and bounds behavior with a real caller. |
| P06-F04 | XPtr64 buff-instance specialization | Inspect the existing generic conversion and prove the actual consumer specialization. |
| P06-F05 | XPooledList `RemoveAll` / NoSpawnTree | Recover the real destruction/allocator path and validate the selected consumer. |
| P06-F06 | Protobuf repeated-pointer destruction | Resolve original SDK/header provenance and validate a generated-message destructor. |
| P06-F07 | BrainComponentData access | Recover the field currently hidden by ActorAI padding and validate its consumer. |
| P06-F08 | Protobuf descriptor index | Establish the original index layout/operation and prove the selected descriptor consumer. |

These jobs are bounded starting points, not claims that the callers are ready.
Dependencies between them reflect inspected requirements; unrelated subsystem
milestones are not blanket prerequisites. Existing claimed owners require the
normal reconciliation process before source edits. Every implementation must
preserve prior O callers through the full relocation-aware landing gate.

## Evidence policy and implementation

Only O supplies exact caller evidence. The index collapses template arguments
and can merge same-line overloads; its representative signature does not name
every specialization. A caller may observe a helper through a nested inline
chain. Neither an exact caller nor a present header independently proves all
source definitions, overloads, layouts or consumer readiness.

`tools/helper_ledger.py` now uses strict O, exports deterministic caller-address
sets and states the scope limitation. Queue totals use set unions. Its
`Static` column remains compatible but means source-file location only, not C++
linkage. Body detection remains a heuristic: the audit found an existing XPtr64
conversion body that it labels absent.

The earlier verifier correction removed helper-definition exemptions based on
grouped or stale ledger counts. That verifier was removed with the swarm in
`71cd76b`. Current recovery still requires specific source evidence and the
strict validators; earlier helper counts do not discharge review obligations.

The new `tools/helper_audit.py` exports revision-pinned evidence without changing
the generated inventory or rebuilding the cache. Refresh reports explicitly:

```sh
python3 tools/helper_audit.py --out /tmp/helper_audit.json
python3 tools/helper_ledger.py --out /tmp/helper_ledger.csv --top 25
python3 -m unittest discover -s tools -p 'test_*.py'
```

The committed legacy `data/2.6.2/helper_ledger.csv` was not rewritten. Readers of
that file, including `next.py`, can still display its earlier counts until it
is regenerated. Use a freshly generated ledger for
strict evidence; legacy counts do not supply an exemption.

## Validation and completion boundary

Before and after this audit: **13,662 / 36,559 O functions**, covering
**3,188,172 / 12,467,556 original bytes**. Delta: **0 functions / 0 bytes**.
The remaining inventory is 21,915 U, 976 M and 6 W. No source owner or nonmatch
was declared complete and no game source, claims or generated inventory changed.

The audit is complete when its map and scoped jobs are recorded and the tooling
regressions pass. Implementation of P06-F01 through P06-F08 remains open. No fresh
game build/full checker run is claimed for this tooling-and-evidence change.
