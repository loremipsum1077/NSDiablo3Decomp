# Recovery restart: targets, first batch and multi-owner validation

Planning and design only, dated 2026-10-30 against `25f2eaa`. Nothing here was built,
claimed, landed or relabelled, and `tools/recovery.py` is unchanged. The full ranking is
[`docs/recovery_targets.csv`](recovery_targets.csv).

## Finding that drives the plan

The recovery v1 mapping gate stops on any new data marker except a witnessed vtable
(`recovery_mapping.compare`: `.rodata*`, `.data`, `.bss`, `.rodata.cst4` and jump tables
all halt). An original-ELF scan of each owner's code finds ADRP references to anonymous
`.rodata`/`.data` (string literals, constants, jump tables) and to named data used by
that owner alone (likely file-local statics). 42 of the 45 owners landed through
recovery-only have neither. The other three have two `BR` instructions or two rodata
references each, so the proxy is conservative.

| Class (CSV `class`) | Owners | U functions | U bytes |
|---|---:|---:|---:|
| `landable-v1`: ≤64 identities, no qualified names, no data blockers | 92 | 460 | 99,616 |
| `data-gate`: otherwise eligible, needs an anonymous/private data proof | 500 | 8,194 | 3,766,580 |
| `needs-subset`: >64 identities or `@file` names | 74 | 8,222 | 3,073,708 |
| `lua-recipe`: 5.0.0-snapshot Lua at `-O0`, needs CMake recipe edits; normal mode cannot match | 9 | 284 | 86,728 |
| `blocked-template`: `next.py` tier `blocked` | 7 | 215 | 63,592 |

Batching cuts the cost of each owner but does not create yield. Under today's data
rule the reachable pool is about 100 KB, or 1.2% of U bytes. The yield lever after
this design is a reviewed proof path for string/constant `.rodata` and jump tables
(the `data-gate` class, led by protobuf `text_format.cc` 37.7 KB, `wire_format.cc`,
`io/tokenizer.cc`, `jpge.cpp`, curl `http.c`/`transfer.c`/`cookie.c`). The second lever
is named-subset support (`ScriptUtil.cpp` 121 KB, `extension_set.cc`, `LuaScript.cpp`).

## Ranking method

Inputs are `functions.csv`, `function_files.csv`, `helper_ledger.csv`, `tools/next.py`
tiers/missing headers, the original `main.elf`, and the main checkout's `claims.csv`.
The ranking excludes the 17 claimed or stuck owners, the parked owners
(`SQuests.cpp`, `PowerAPI.cpp`, `BrainComponents.cpp`, party InspectSkillsScreen),
P02/P06 follow-up owners and non-source owners.
`p_match` = 0.25 + 0.45 × sibling O/M share in the directory, plus 0.2 for upstream
middleware source, −0.1/−0.3 for `headers`/`blocked` tier, −0.03 per unrecovered helper
header (capped at −0.25), −0.1 if the largest U function exceeds 4 KiB, and it is pinned
to 0.03 above 16 KiB (the `line_evidence` bound). Lua owners are halved.
`expected_bytes` = U bytes × `p_match` × eligibility × data-gate factor. That factor is
1, or 0.6 for indirect branches only, or 0.12 when blocked.
These are heuristics for ordering work, not evidence for review. "Helper deps" are
`helper_ledger` headers that are not in `src/` or have no body. Adding a body to an
existing shared header (`XPtr64.h`, `MathGeneral.inl`, `dmMath.inl`, `bdMemory.inl`) is
the PlayerAccount/bdStorage failure mode. Such an owner needs a solo slot or a
dependency group.

## Top 40 (`landable-v1`, by expected bytes)

| # | Owner | U fn / bytes | Rationale |
|---:|---|---:|---|
| 1 | X/Client/UserInterface/Interface/UITransmuteCommon.cpp | 25 / 10,160 | Largest clean X owner; needs blz `algo_memory.h`, `repeated_field.h`, `Player.h`, `XPtr64.h` bodies, so solo |
| 2 | Contrib/Contrib/Domino/Dynamics/Contacts/dmContactSolver.cpp | 5 / 7,696 | One 4.5 KB solver; only dep `dmMath.inl` bodies (shared Domino .inl), so solo |
| 3 | X/Client/Camera/Camera.cpp | 8 / 3,172 | Adopt `ScreenManager.h` first; `Quaternion.inl` body |
| 4 | X/Common/CosmeticItems.cpp | 16 / 3,484 | Only dep protobuf `repeated_field.h` templates |
| 5 | Contrib/Contrib/Domino/Dynamics/Joints/dmSphericalJoint.cpp | 4 / 3,776 | Joints family untouched; `dmMath.inl`, batch after #2 |
| 6 | X/Server/STimedEvents.cpp | 10 / 4,932 | Four shared deps incl. `XPtr64.h`/`MathGeneral.inl`; low sibling rate |
| 7 | X/Common/ActorAttributes.cpp | 16 / 2,736 | `ActorDefinition.h`, `XPtr64.h` |
| 8 | Contrib/Contrib/DemonWare/bdLobby/bdAntiCheat/bdAntiCheatChallenge.cpp | 5 / 1,212 | DemonWare sibling rate 0.76; `bdArray.inl`/`bdMemory.inl` bodies (bdStorage-type risk) |
| 9 | X/Server/SCosmeticItems.cpp | 5 / 2,524 | Only `XPtr64.h` |
| 10 | Contrib/Contrib/Domino/Dynamics/Joints/dmDistanceJoint.cpp | 4 / 2,776 | Same `dmMath.inl` set as #2/#5 |
| 11 | Contrib/Contrib/DemonWare/bdLobby/bdMatchMaking/bdSessionInvite.cpp | 7 / 1,008 | bdMatchMaking landed; `bdSessionID.h` body |
| 12 | Contrib/Contrib/DemonWare/bdCore/bdCrypto/bdTomcryptBackport.cpp | 1 / 1,100 | Upstream libtomcrypt `rsa_verify_hash_ex`; calls unattributed crypto (P03-F03) |
| 13 | Externals/curl/lib/conncache.c | 8 / 948 | Upstream curl; calls only landed `hash.c`/`bundles.c`; no deps |
| 14 | X/Client/UserInterface/Interface/UICraftingCommon.cpp | 2 / 2,348 | Four definition/blz deps |
| 15 | X/Client/UserInterface/Controls/UICutsceneControl.cpp | 9 / 1,188 | `Cutscene.h` body |
| 16 | Contrib/Contrib/DemonWare/bdSocket/bdDTLS/bdPacket/bdDTLSInitAck.cpp | 18 / 2,912 | DTLS family: adopt `bdDTLSRandom.h`; dependency group with #36/#37 |
| 17 | X/Server/SActTransitions.cpp | 7 / 1,872 | `GlobalDefinition.h` body |
| 18 | X/Common/Hero.cpp | 5 / 1,924 | Three deps incl. `XPtr64.h` |
| 19 | Externals/curl/lib/select.c | 1 / 832 | Upstream curl, no deps |
| 20 | X/Client/UserInterface/Controls/UIListItemControl.cpp | 15 / 1,400 | `headers` tier (ScreenManager.h); `UITextControl.h` |
| 21 | X/Common/Marker/SmartEncounterMarker.cpp | 5 / 1,772 | `headers` tier; five deps |
| 22 | X/Client/UserInterface/Controls/UIRadioButtonControl.cpp | 16 / 1,928 | One `BR` (tail call or table, check); `headers` tier |
| 23 | X/Client/Physics/PartialRagdoll.cpp | 4 / 1,740 | Six definition deps |
| 24 | X/Common/Marker/SmartAudioVolumeMarker.cpp | 8 / 1,176 | `headers` tier; Smart*Marker family, one per batch |
| 25 | X/Common/ActorCommon.cpp | 12 / 2,340 | `headers` tier; three deps |
| 26 | X/Common/Marker/SmartAmbientSoundMarker.cpp | 8 / 1,380 | Five deps; same family as #21/#24 |
| 27 | Contrib/Contrib/DemonWare/bdLobby/bdAchievementsEngine/bdAchievementsUpdatedPushMessage.cpp | 9 / 1,496 | `bdMemory.inl`, `bdAchievementState.h` |
| 28 | Contrib/Contrib/DemonWare/bdPlatform/bdPlatformSocket/bdPlatformStreamSocket-switch.cpp | 7 / 856 | Header already in `src/`; no deps; calls existing platform socket/log |
| 29 | Contrib/Contrib/DemonWare/bdLobby/bdReward/bdRewardEventJSONV2.cpp | 12 / 1,024 | No deps; sibling `bdRewardEvent.cpp` is claimed, so decide that claim first |
| 30 | Contrib/Contrib/DemonWare/bdCore/bdCrypto/bdCypherAESGCM.cpp | 9 / 564 | bdCrypto sibling rate 0.52; `bdAuthenticationTag.h` |
| 31 | X/Common/ExperienceCommon.cpp | 5 / 1,220 | Four deps incl. `MathGeneral.inl` |
| 32 | Contrib/Contrib/DemonWare/bdLobby/bdReward/bdRewardEventJSON.cpp | 12 / 924 | No deps; shares bdReward headers with #29, so different batches |
| 33 | X/Client/RActors/RActorACDEffects.cpp | 4 / 1,200 | `Rope.h`, `XPtr64.h` |
| 34 | X/Client/CMarker.cpp | 3 / 1,040 | Four deps |
| 35 | X/Server/SConductor.cpp | 20 / 10,944 | Big but low p: `headers` tier, six deps |
| 36 | Contrib/Contrib/DemonWare/bdSocket/bdDTLS/bdPacket/bdDTLSHeader.cpp | 10 / 708 | DTLS base packet; first in the DTLS group |
| 37 | Contrib/Contrib/DemonWare/bdSocket/bdDTLS/bdPacket/bdDTLSCookieEcho.cpp | 10 / 1,136 | Needs `bdDTLSInitAck.h` (#16) |
| 38 | X/Client/UserInterface/Controls/UIThumbButtonControl.cpp | 6 / 384 | Header in `src/`; no deps; own vtable (witnessed `.data.rel.ro` path) |
| 39 | X/Client/Console/TouchGestureTracker.cpp | 4 / 712 | `algo_memory.h` |
| 40 | X/Server/OnlineService/DataHealUtils.cpp | 1 / 996 | Three deps |

Before claiming any owner, preflight it with `try_file.py`. Confirm that no new data
marker appears, that the per-owner recipe is right (DemonWare/protobuf
`-funsigned-char`, curl's corrected recipe) and that owner membership is complete.

## First batch: three compatible owners

**bdPlatformStreamSocket-switch.cpp (#28) + conncache.c (#13) + UIThumbButtonControl.cpp
(#38)**, 21 U functions / 2,188 bytes. This is a pilot of the group mechanics, so it
uses the lowest-risk compatible set rather than the highest yield.

- Each owner sits in a different CMake recipe group (DemonWare `-funsigned-char` glob,
  middleware C, `d3_game_obj` with game defines) and a disjoint `src/` subtree, so no
  header path can be shared.
- No owner has an unrecovered helper header except its own. Two own headers already
  exist; `conncache.h` would be new and consumed only by its owner. None needs a body
  added to a shared header.
- Original-ELF B/BL scan: zero branches between any pair. Callees are existing or
  landed owners: `hash.c`/`bundles.c` (M), platform socket/log, `UIButtonControl`.
- No anonymous or private data, and no indirect branches. Each owner has ≤64
  identities, no qualified names, and is all-U, so each produces exactly one new
  object and failures attribute by object path.
- None is claimed, parked or a follow-up job. UIThumbButton's own vtable exercises
  the per-owner vtable witness inside a group.

Next candidates: #11 + #19 + #36 (DemonWare matchmaking, curl, DTLS base). #2 goes
solo for its `dmMath.inl` bodies. The DTLS chain #36→#16→#37 forms a declared
dependency group (v2, below).

## Multi-owner validation design for `tools/recovery.py`

Scope: a **group** of 1–3 independent owners on one immutable baseline, with one
combined candidate built as a **stack of per-owner commits** (each touching only that
owner's paths). Single-owner receipts keep `KIND='recovery-only-v1'` and verify
unchanged; groups use `recovery-only-v2`. Every existing check stays and runs over the
union. Per-owner checks run once per owner.

**Targets file.** `{"format":2,"owners":{"<path>":{"functions":[...],"headers":[...]}}}`:
per-owner identities plus the header paths that owner may add or edit.

**L23–32 `inventory`.** Take the union of per-owner targets, with no change in meaning.
Under two-tier O, the permitted transition is computed per identity. The candidate
result must be `ok` in both modes with no regression, and the review decision must
be `verified` or `safe-uncertified`. Then U becomes O; otherwise U becomes M. Every O
whose obligations include `safe-uncertified` gets a row in a separate proof-gap ledger
(e.g. `data/2.6.2/recovery_proof_gaps.csv`: identity, obligation key, review SHA). This
is a policy change to `docs/recovery_only.md` ("never O credit") and needs its own
review; it is independent of grouping.

**L35–58 `configure`.** `--owner` becomes repeatable (`args.owners`, sorted, unique,
1–3). The 1–64 complete-owner check (L41–45) runs per owner, with a group cap of
128 identities. Additional checks:
1. Each commit in `baseline..candidate` maps to exactly one owner's path set, in owner
   order (`changed_paths` per commit).
2. The allowed-path lambda (L53) accepts `src/<owner>` for group owners, plus headers
   listed for exactly one owner. A header listed for two owners halts: "shared header,
   split or declare a dependency group".
3. A new `recovery_group.independence()` scans the original ELF for B/BL and
   ADRP+ADD function-address references between group owners. Any hit halts. A v2
   `depends_on` relation is required for those, and then members are ordered and
   validated as a chain.
4. `binding` records `owners`, per-owner targets, per-owner commit SHAs and the
   independence witness. `owner` stays for v1.

**L82–107 `compare`.** Same rules over the union. When an unselected identity changes,
the halt now carries `suspects`. These are the group owners whose candidate objects
newly define a symbol that the changed identity's object relocations reference, or
whose object path matches. No suspect, or more than one, is a group-level failure.

**L131–239 `audit`.** Whole-link work runs once per mode: debt comparison, stable call
failures, `validate_data` ledgers and the unrelated-object check (L171–177 excludes the
set of group owner suffixes; extra objects are allowed only for group owners). Object
work (L164–233) runs in a loop per owner: unique-object check, `strict.verify`,
executable coverage, mapping/extras, unwind, vtable witness and `owner-data-abi`.
Obligation keys become `<mode>:<SRC-id>:<kind>`; linked obligations `<mode>:linked:<addr>`
gain `"owner"`. Data transitions must each be explained by exactly one owner's vtable
witness, or the audit halts with that owner named. `source-semantics-provenance` is
emitted per owner from that owner's commit diff.

**L242–247, L319–337 review.** The template has `group` obligations (debt, call
witnesses, data ledger, independence witness) plus one section per owner, each with
its own `reviewer`, `independent` and `blocking_findings`. Different reviewers may
sign different owners, but one person must review the group section.
`review()` checks every section. A blocking finding blocks only its owner; the group
must then be split (below), never landed partially from the same receipts.

**L250–316 `verify_audit`.** The owner-object glob (L279–287) and vtable recomputation
(L295–312) loop per owner. The trusted-tool hash check stays.

**L340–438 `land`.** L353 becomes `sorted(args.files) == binding['owners']`. Claim
snapshot and recheck (L363–371, L422–429) cover all owners under one `claim.locked()`.
The integration worktree replays the per-owner commits. Each one is followed by an
inventory commit for that owner's rows, so history keeps one revertable commit pair
per owner. Only the stack tip is rebuilt (normal + Lua) and compared with the reviewed
artifacts (L411–420). One `merge --ff-only` lands the whole group atomically, and
`landed.json` lists each owner's commits and review SHA.

**Splitting a failed group.** Retain the transaction. If the failure names exactly
one owner (object path, suspect list, owner review section), drop that owner and
rebuild the candidate as the baseline plus the remaining owners' commits replayed
unchanged. Otherwise split into singletons (k ≤ 3). An owner's existing review
decisions carry into the subset run only for obligations whose `evidence_sha256` is
identical. Any changed digest needs a fresh decision. Group obligations always need
re-review. A passing result from a larger group never counts as acceptance for a
subset whose final link differs.

**Tests** (extend `tools/test_recovery.py`, `test_recovery_land.py`):
- Each existing v1 test still passes with one owner, and v1 receipts still verify.
- Two-owner success: both objects audited, per-owner obligation keys, one ff merge,
  only selected rows change.
- Rejections: a header listed for two owners; a commit touching two owners' paths;
  an original-ELF cross-call (fixture ELF); an extra object outside the group; a
  missing per-owner review section; a non-independent owner section.
- An unselected identity regresses through one owner's new symbol: `suspects` names
  that owner. With two possible causes, a group-level failure.
- Split replay: the subset candidate tree equals baseline + remaining commits, and
  review decisions carry over only on identical digests.
- Claims: an owner's claim changes mid-validation, so the whole group stops and the
  primary is unchanged.
- Two-tier inventory: ok in both modes gives U→O plus a proof-gap row when
  safe-uncertified; a Lua-only ok gives U→M.

## Overhead cuts that keep every check

1. **Content-addressed build cache.** Key: baseline *tree* id, CMake/toolchain file
   hashes, Docker image IDs, checker/ELF/NSS/Lua-receipt SHAs, `recovery_tools` hashes
   and mode. Entries are read-only, with an artifact-hash manifest re-verified before
   reuse. Group N's integration tip tree is group N+1's baseline tree, so on a serial
   chain the baseline builds come from the previous landing's fresh builds.
2. **Integration rebuilds only the candidate.** The baseline is the same immutable
   key, already fresh in this chain. The fresh-candidate artifact reproduction check is
   kept in both modes.
3. **Data ledgers and `runtime_object` digests** are cached with the build key. They are
   deterministic functions of artifacts plus pinned inputs.
4. **Short review packets.** Per-owner diff and obligations only. Whole-link debt is
   shown as "identical to baseline audit `<sha>`" plus the delta rather than 1,525
   rows; the reviewer still records every decision.
5. **One claim/coordination/lock cycle per group**, hardlinked retained artifacts in
   place of copies, and objects pruned only after `landed.json` (as done on 2026-10-30).
6. **Preflight before claiming** (data-marker proxy, recipe, membership; seconds of
   work) so that blocked owners never reach a full build. Of the 9 overnight owners,
   PlayerAccount spent 4 builds before parking.

## Builds per owner

| Workflow | Fresh full builds per group | Per owner |
|---|---:|---:|
| Today (v1, 1 owner): baseline+candidate × 2 modes × (review + integration) | 8 | 8 |
| Design, cold baseline: baseline 2 + candidate 2 + integration candidate 2 | 6 | 3.0 (k=2), 2.0 (k=3) |
| Design, chained baseline from previous landing: candidate 2 + integration 2 | 4 | 2.0 (k=2), 1.3 (k=3) |

A split costs about 2 more builds per subset retried. These are build counts, not a
measured speedup. Run the benchmark in the bulk-validation proposal (serial versus
grouped, same caps, one deliberately rejected dependency case) before relying on them.
