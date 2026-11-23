# Two-owner recovery validation

This opt-in path shares a complete set of builds across exactly two complete
function owners. Review uses four fresh builds (baseline/candidate × normal/Lua),
and integration uses four more for the pair. Two separate single-owner landings
would require sixteen builds; a group requires eight. This is a build-count
reduction, not a measured guarantee of half the total elapsed time. Source work,
per-owner proof checks and independent review remain necessary.

## Scope

- Exactly two distinct original owner paths, sorted lexicographically, each with
  nonempty complete inventory membership; at most 64 function identities combined.
- Targets are the exact address/name union, including existing O/M identities.
  Qualified names, partial owners and data-only owners are unsupported.
- The candidate contains both owner sources. Only those sources, shared
  `.h`/`.hpp`/`.inl` files under src/include, tests and docs may change.
- Non-group runtime objects and existing linked diagnostics must be preserved.
  Each owner retains its own object, emitted-function, mapping, relocation and
  unwind checks. A symbol cannot be supplied by the other owner to hide a missing
  definition. Duplicate global definitions are rejected conservatively.
- This first group version admits no new data sections or data-status transitions,
  including new vtables. The single-owner policy remains separate.
- Only selected U→M transitions can land. No exact certification or O promotion.

Group bindings, audits and independent reviews have distinct policy/kind names.
Single-owner receipts/reviews cannot approve a group. Both group owner claims must
exist and point at clean worktrees containing the exact reviewed candidate commit.
The landing checks both claims under the coordination lock before preparation and
again before fast-forward. Keep one worktree/branch per existing claim; two branches
may point at the same combined candidate. Preserve older source before replacing
any retained branch state.

## Prepare and validate

Create an owners manifest in private build output, for example:

```json
{"format":1,"owners":["Original/First.cpp","Original/Second.cpp"]}
```

Create the existing format-1 targets manifest containing *every* address/name pair
belonging to these owners at the immutable baseline. Derive membership from
functions.csv and function_files.csv; do not select only new functions. Confirm
scope, claims and shared-header consumers before expensive builds. The Domino
preflight below illustrates why earlier match counts are insufficient.

Use the same original/checker/NSS/Lua pins and fresh-output rules as
[recovery-only validation](recovery_only.md), replacing the mode and owner option:

```sh
python3 tools/receipts.py build --mode recovery-group --run \
  --repo "$PRIMARY" --baseline "$BASE" --candidate "$CANDIDATE" \
  --owners "$OWNERS" --targets "$TARGETS" --output "$RECEIPTS" \
  --original-elf "$ELF" --original-sha256 "$ELF_SHA" \
  --checker "$CHECKER" --checker-sha256 "$CHECKER_SHA" \
  --lua-cache "$LUA_CACHE" --lua-receipt-sha256 "$LUA_RECEIPT_SHA" \
  --nss "$NSS" --nss-sha256 "$NSS_SHA" --seconds 3600
python3 tools/receipts.py verify --mode recovery-group --output "$RECEIPTS"
```

Build completion and verification are not independent approval. A separate reviewer
must inspect every per-owner/per-mode and group-global obligation, the source diff,
all shared consumers, original evidence and retained artifacts. They complete the
generated review-template.json into a separately pinned review file. Never reuse
an individual owner's approval or mark unresolved behavior verified merely because
an object compiled. Missing proof remains explicit.

## Atomic integration

Name both owners in canonical manifest order. This is one reviewed patch and one
atomic integration commit, with fresh baseline/candidate normal/Lua artifacts that
must reproduce the reviewed artifacts:

```sh
python3 tools/land.py "$OWNER_A" "$OWNER_B" --recovery-group \
  --recovery-receipts "$RECEIPTS" --review "$REVIEW" --review-sha256 "$REVIEW_SHA" \
  --original-elf "$ELF" --checker "$CHECKER" --lua-cache "$LUA_CACHE" \
  --nss "$NSS" --seconds 3600
```

The primary must be clean at the exact baseline; both claims must remain unchanged.
A changed claim, primary edit, missing owner or differing integration artifact blocks
fast-forward and preserves the transaction. No push or claim/worktree deletion is
part of this path. After an error inspect retained state before retrying.

## Checkpoints and limits

Existing explicit --resume behavior authenticates completed attempts against the
entire binding, including both owners, partition, source, support, tools, images,
inputs and target union. It is not a new cross-run baseline cache. Normal and Lua
remain separate; old single-owner or pruned receipts cannot seed group acceptance.
Verification recomputes the complete group audit and global data ledgers from
retained artifacts rather than trusting the recorded obligations alone.

The retained dmSphericalJoint + dmDistanceJoint patch is **not ready** for this
policy. Fresh offline consumer compilation against baseline 403f0b8 found identical
runtime content in Common/dmMath.cpp and Collision/dmCollision.cpp, but changed
runtime code, relocations, unwind and symbols in Collision/Functions/dmDistance.cpp.
That owner is outside the proposed pair. Preserve this blocker; neither shared
builds nor independent review waive it.


## Verified implementation smoke (2026-11-01)

At tool commit `c655027d668cbcd6d71bcbe376978b1f1f2ecb0e`, a real offline run used
bdRandom.cpp (three O identities) and bdStorage.cpp (seven M identities), with
identical baseline/candidate commits. Exactly four fresh attempts built both
owners together. Official verification replay returned `retained_bytes_verified`
for policy `recovery-group`; strict and single-owner verification rejected the
same receipts. Independent inspection confirmed:

- 23 recomputed obligations, including each owner's mapping, emissions, relocation,
  unwind and object-path evidence in both modes.
- 733 identical object/link/map/provenance/result artifacts per mode.
- All 11,063 data identities preserved per mode and all ten selected functions'
  unwind checks passing.

The independent review is a tooling smoke, not source or landing
approval: no candidate source changed, no U→M or O promotion occurred, and the
review template remains pending for that smoke. At that point atomic two-claim
landing had only synthetic transaction coverage using real Git. The subsequent
source landing below supplies the first real grouped integration evidence.

The pre-refactor regression run passed 226 recovery cases and 25 strict receipt /
legacy landing cases. Removing duplicate inherited/imported discovery in the new
group tests reduced recovery discovery to 163 cases without removing unique checks;
all 16 group-specific cases passed again. Synthetic audit tests replay real ELF
fixtures with controlled checker/unwind/data responses, including misrouted owner
functions, lost exact evidence, global-data differences, forged/rehashed scope and
omitted obligations. Transaction tests cover claim/source races, failed builds,
artifact differences and preservation of the full reviewed patch.

## First source landing (2026-11-01)

Commit `78fb712a0eb3dc8576d94d84cbf199b1633bcf27` lands curl hostip4.c and
share.c (four functions / 324 original bytes) as U→M. All four selected functions
passed both comparison modes. The 729 existing objects were byte-identical in
each mode; only the two owner objects were added. Independent review accepted
21 verified obligations and two safe-uncertified global-data obligations with
existing debt unchanged. This is recovery credit, not exact certification.

Four review builds and four fresh integration builds passed, followed by retained
replay and exact reproduction of the reviewed object/link/result artifacts.
Both claims and their clean source worktrees were preserved.
The independent review SHA256 is
`0523c7136579705e5588893268b7f9bcbac235c977ea3d2afcdfcec2f0eb1963`.
Review binding-to-audit elapsed approximately 6m24s; integration measured 584.13s.
These measurements include validation beyond compilation. Eight builds replace
sixteen for two serial owners; no measured twofold elapsed-time claim is implied.
