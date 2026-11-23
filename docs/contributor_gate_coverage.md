# What the contributor command verifies

`tools/contributor_check.py --run-all` aggregates deterministic checks from the
existing trusted tools against retained baseline/candidate normal/Lua artifacts.
It does not construct those artifacts. It returns **failed (1)** or **incomplete
(2)** and always leaves accepted/landed false. [Command and private manifest
format](contributor_ci.md#one-local-report-command).

| Gate | Implemented | Explicit remaining evidence |
| --- | --- | --- |
| Commit/scope identity | Exact commits; replacement/lazy-fetch disabled, grafts rejected; whole-owner and selected-target hashes; reject changed oracle/config | Authenticate agreed scope and changed source/header impacts; reconcile earlier checks outside baseline non-U plus owner |
| Original inputs | NSO/converted ELF SHA-256 pins; NSS hash recorded | NSS build/DWARF authenticity and distribution rights are not inferred |
| Link/checker/artifact binding | Copied checker/original/link/map/provenance pins before/after; four mode/version receipt and object bindings | Hash consistency is not trusted construction or approval |
| Linked bytes/relocations/regression | Existing checker on baseline non-U plus every owner identity including U; `integration.compare` preservation; selected gains allowed | Unchanged unknowns remain partial; outside-target status changes fail even if improvements; selected missing proof remains partial |
| Data/pointers | Existing relocation-aware data validator both modes; per-identity verified-equal preservation and missing-row detection | Unrecorded pointers, owner data/source ownership, different/unverified payloads remain partial |
| Original/object | `strict.configure/verify` for every owner member, both modes; named relocations; prior object status preservation | Unsupported relocations and qualified identities without a proven mapping remain partial |
| Emissions/extras | Whole-owner symbol preservation; candidate extras hashed/counted; executable range coverage; malformed/overlapping ranges rejected; allocated data counted | Unknown extras, unnamed executable bytes/padding, missing owner members and non-EH allocated data need independent audit |
| Unwind | Existing selected-target FDE/range/program audit for each object and both links; adapter additionally requires exact original/object/both-link CIE and FDE programs | Absent/ambiguous/unsupported FDEs or differing CIE bodies remain pending; no trailing-zero stripping in the added program guard |
| Source snapshots | Existing source-manifest file hashes equal exact committed source/headers/recipe/toolchain files; rechecked afterward | Snapshot equality does not prove that those files produced supplied objects/links |
| Toolchain/build lineage | Trusted validator/support/settings hashes, checker SHA, optional compiler/backend/linker file hashes and numeric version claims | Actual runtime versions and a trusted fresh reproducible build are pending; supplied receipts never upgrade this gate |
| Independent source review / integration | Separate explicit pending gate, no auto-approval | Narrow independent source/evidence review, declared shared-header callers, authorized named integration and postintegration CPU validation |

The adapter in `tools/contributor_adapter.py` reads the existing `done.json` and
source manifests and invokes pure existing validators. It never fabricates a
live batch or calls the integration build/campaign. It copies required artifacts
into a disposable temporary directory, rejects traversal/symlinks/identity drift,
and exports fixed statuses/counts/hashes. Raw validator output can contain
original hex/CIE data, names and paths and is never exported.

A legitimate baseline may lack the owner object only when the exact baseline
has no owner source and all owner members are U. Candidate objects are mandatory.
A source manifest/receipt claiming a commit is not trusted merely because its
JSON says so. Contributors can forge the entire report; maintainers reproduce
CPU checks and perform targeted independent review instead of rerunning recovery.

## Retained-batch exercise

A read-only run used the genuine retained `2026-10-20-cpu-productive` baseline
and candidate2 receipts, matching all four checked-in archived receipt hashes.
Source snapshots matched exact commits `46df258157d9a73e519b7487cd0f5aae716cb03c`
and `fa59d0321278b01fd6cccb6da183e3d19ceb1aa3`. This is an earlier accepted-batch
compatibility test, **not a fresh contributor build or new approval**.
For one ten-function owner, the command exercised both-mode checker/data/object/
emission/unwind paths in about 77 seconds:

- All ten selected functions became linked OK in each mode; strict object proof
  passed ten in each mode. The absent baseline objects were authenticated against
  the baseline source/U inventory rather than silently accepted.
- Source snapshot and whole-owner emission checks passed, with no named extras,
  uncovered executable bytes, lost functions or non-EH allocated payload sections.
- Four other functions also improved in each mode outside this owner scope.
  The strict preservation gate correctly failed these unexpected changes; it did
  not silently accept the entire multi-owner batch under one claim.
- Per link: 387 data identities verified equal, 12 different, 10,664 unverified;
  no verified-equal loss or missing inventory rows. Data remains **partial**.
- Fresh source-to-build lineage, actual toolchain versions and independent review
  stayed pending. Overall report **failed**, accepted/landed false.

The final strengthened CIE guard was separately rerun against the same genuine
objects and links: all 20 candidate unwind checks stayed **pending** under exact
CIE comparison; object/emission/source checks passed.
All measured main inputs remained unchanged. No binaries, full maps, raw logs or
private evidence contents were copied into these docs.

An earlier same-commit cached linked/data probe had 50 not-checkable entries in
each mode with zero deltas. The initial implementation called that failed; the
current policy records unchanged unknowns as **partial**, never a new regression
or a pass. Focused tests verify this behavior and still fail degraded statuses
and withhold missing selected-target proof.

## What is not yet verified

No fresh clean-clone setup, dependency provisioning, dual-mode build or compiler
construction was performed. The isolated worktree lacks originals, initialized
`tools/common` and Cargo/Rust on PATH; Lua/compiler images and caches were not
provisioned or authenticated there. Existing owner-machine artifacts are cached
compatibility evidence only. Existing `build.py` links alone do not supply retained
per-mode object/source receipts; obtaining those through trusted foreground builds
is still a preparation step, not automated by this command.

For hostile fork code, trusted disposable compilation **without originals or
credentials** and a separate disposable comparison environment remain necessary.
The [private CI design](contributor_ci.md#private-maintainer-mvp-design-only) describes
that boundary; this script does not create it. No service was provisioned. Source
review and postintegration validation are irreducible explicit pending gates.
