# Plan: finishing the 2.7.7 decompilation

Status on 2026-11-24 (branch `port-2.7.7`, commit `25ca2d8`): **23,786 O, 8,134 M, 2,906 U,
2,175 L, 4 W** of 37,005 functions. This plan splits the remaining work into tasks an AI
agent can take on one at a time, with size, order, and a definition of done for each.
Background: [port_2_7_7.md](port_2_7_7.md) (tools, compiler, mixed build) and
[changes_2_6_2_to_2_7_7.md](changes_2_6_2_to_2_7_7.md) (what the game changed).

## Two finish lines

1. **Recomp-ready:** every function in the binary has source that does what 2.7.7 does.
   The recomp project builds 2.6.2 from the 2.6.2 tree; it cannot build 2.7.7 from this
   tree yet:
   - the 2,906 `U` functions have no source, so the link fails;
   - most `M` functions have 2.6.2 source compiled against 2.6.2 struct layouts and
     constants, so they would run with wrong offsets.

   Recomp does not need byte-exact code, but this project has no other reliable test of
   semantics.
2. **Matching:** every function is `O`. This is how finish line 1 gets proven, so the plan
   aims at `O`. A function left `M` with a documented, non-semantic difference (for
   example a checker proof limit) is acceptable for recomp.

## Where the work is

| Area | M | U | Notes |
|---|---:|---:|---|
| Game server (`X/Server`) | 2,813 | ~250 placed | Largest changed area; struct layouts dominate. |
| Game client (`X/Client`), mostly UI | 2,575 | ~700 placed | Console UI for new features. |
| Game common (`X/Common`) | 1,243 | ~100 placed | Items, ActorCommonData, inventory, GlobalSNO users. |
| Generated protobuf (`*.pb.cc`) | 801 | 234 | Regenerable from descriptors embedded in the binary (task A1). |
| DemonWare / Lua / BlizzardProtoBuf / OggVorbis | 390 | 0 | Prebuilt in 2.7.7; mostly checker limits and code that never matched in 2.6.2 (see port guide). |
| libcurl (7.42.1 → 7.55.1) | 137 | 69 | Upstream library upgrade (task A3). |
| Domino physics | 44 | 62 | Blizzard library; small changes plus new hull-building code. |
| Unplaced new code | — | 1,301 | 610 KB, no file known yet; mostly whole new compile units (task C1). |

Size of the 2,906 `U` functions:

| Size (bytes) | Functions |
|---|---:|
| ≤ 16 | 388 |
| 17–64 | 383 |
| 65–256 | 913 |
| 257–1,024 | 965 |
| > 1,024 | 257 |

In total, 1.38 MB of code. 1,605 `U` functions already have a proposed file
(`tools/string_anchors.py --owners`).

What is blocking the remaining `M` functions (first failure, from
`build/analysis/2.7.7/triage/m_triage.csv`; one function can have several causes):

| Cause | Functions |
|---|---:|
| `ActorCommonData` grew 0x340 → 0x360 | ~2,061 |
| Function size differs (needs subdividing) | ~2,028 |
| Checker cannot prove an ADRP pair or jump table | ~1,160 |
| `UserRuntime` grew 0x1e10 → 0x1e98; HUD `DisplayInfo` 0x4d0 → 0x4d8 | ~63 |
| Changed constants (`0xc6ec → 0xdb74`, `0xb1d0 → 0xb208`, …) | ~100 |
| Wrong callee names (mapping errors) | ~130 |
| Unknown data references | ~66 |

## Sizing

- Units are **agent sessions**: one focused session of a capable coding agent, with a build
  and check at the end (a few hours of wall time).
- **S** = 1 session, **M** = 2–5, **L** = 5–15, **XL** = 15+.
- Rough per-function rates once phase B is done:

  | Kind of function | Functions per session |
  |---|---:|
  | Near-identical changed function | 20–40 |
  | Moderately changed function | 5–15 |
  | Small new function | 10–30 |
  | Large new function (> 1 KB) | 1–3 |

These are planning numbers, not promises. Re-estimate after the first session of each task.

## Phase A: mechanical and tooling (do first; mostly serial)

| ID | Task | Covers | Size | Done when |
|---|---|---|---|---|
| A1 | **Port `tools/bootstrap_game_data.py` to 2.7.7.** It regenerates the protobuf `.pb.cc`/`.pb.h` from the FileDescriptorProtos embedded in the binary, and the game-data includes (`sno_calls.inc`, `gbid_rows.inc`, button maps and other tables). Today it reads the 2.6.2 `.nss` and hard-coded 2.6.2 addresses; make it locate everything in `data/2.7.7/main.elf` through symbols in `functions.csv`/`data_symbols.csv`, as `tools/global_sno_map.py` does. **Status:** protobuf done (`tools/bootstrap_protobuf.py`, +506 O); `sno_calls.inc`, `gbid_rows.inc` and the other tables remain. | ~801 M + 234 U protobuf functions, `GlobalSNOListRefresh`, GBID table | M | Regenerated files build; protobuf functions promote; no O lost. |
| A2 | **Fix the suspected mapping errors** the triage found: inventory `GetPrimaryView`/`GetFocusedView`, `XDecoder::Get`/`XEncoder::Put`, `CTargetCache::GetTarget` and `ActorCommonData::IsAlive` mapped to unnamed callees. Extend `version_diff.py`'s repairs, with tests. | ~130 "wrong function call" | S | Those groups gone from a re-triage; no O lost. |
| A3 | **Upgrade libcurl to 7.55.1.** Bring in upstream curl 7.55.1 source (MIT-style license; **downloading needs the user's approval**) with the existing build configuration (`curl_config.h` and so on), replacing 7.42.1 under `Externals/curl`. | 137 M + 69 U | M | curl functions promote; upstream source unmodified except configuration. |
| A4 | **Locate more data symbols.** 4,742 of the 11,063 2.6.2 data symbols are not in `data/2.7.7/data_symbols.csv`. Add a pass that names data through matched functions after the struct fixes (rerun `port_version.py` logic on newly matched functions), plus new globals referenced only by new code. | "unknown data reference" failures; many checks become possible | S | More symbols located; re-check shows no new conflicts. |
| A5 | **Checker proof limits** (unproven ADRP pairs and jump tables, ~1,160). `tools/check` is a binary, so do not change it. Batch these with `--always-diff`, confirm the code is otherwise identical, and record them like 2.6.2's `data/2.6.2/proof_debt.csv`. | ~1,160 | S | `data/2.7.7/proof_debt.csv` exists; nothing hand-promoted to O. |

## Phase B: shared struct layouts (highest leverage; one agent per struct)

These must be serial per header: two agents must not edit the same struct.

| ID | Task | Covers | Size |
|---|---|---|---|
| B1 | **`ActorCommonData` 0x340 → 0x360.** A first attempt (see `build/analysis/2.7.7/triage/fix_log.md`) helped in isolation but broke 4 existing O functions. Start from those 4, find which fields moved and where the 0x20 bytes went, then verify across the whole group. | ~2,000 | L |
| B2 | **`UserRuntime` 0x1e10 → 0x1e98** and the player-related constant `0xc6ec → 0xdb74` (probably a containing struct's size). | ~110 | M |
| B3 | **HUD `DisplayInfo` 0x4d0 → 0x4d8**, constant `0xb1d0 → 0xb208`, and the remaining stride/constant groups from the triage. | ~100 | M |
| B4 | **Re-triage** after B1–B3 and split "function size differs" (~2,028) into real causes. Repeat phase B on any new shared cause covering ≥ 20 functions. | — | S each |

Method for every struct task:

- Collect the offset deltas from many functions (same field, same delta).
- Add `unk_<offset>` fields rather than guessing names.
- Keep `static_assert(sizeof(...))`.
- Run `try_file.py` on representative files, then a full build and `tools/check`.

## Phase C: new 2.7.7 code (parallel by file after phase B)

| ID | Task | Covers | Size |
|---|---|---|---|
| C1 | **Assign files to the 1,301 unplaced functions.** Most sit in runs of consecutive `U` functions (51 runs of ≥ 10), which are whole new compile units. Use `string_anchors.py` (strings, callers, callees), the 106 new `GLOBALSNO_UNK_*` asset paths, `XVar` registration names, UI template names and diagnostic strings to give each run a file and class name. RTTI is compiled out (only 53 type names exist), so names come from strings and call context. Output: a reviewed `data/2.7.7/new_functions_plan.csv` (address, proposed file, proposed name, evidence). Do not rename in `functions.csv` until the code exists. | 1,301 | M |
| C2 | **Feature clusters** (one agent each, after C1). Starting points from the change guide and `string_anchors.py`; each run is a likely new `.cpp`. See the list below. | ~600 | L each |
| C3 | **New functions in existing files** (1,605 placed by `--owners`): UI (567), console (119), Lua script bindings (55), Items, Powers, SGame, SPlayer and others. Split by file; one agent per file group. | 1,605 | XL total, parallel |
| C4 | **Domino new hull code** (`dmHullBuilder.cpp`, `dmSimpleHull.cpp`, …), 62 U + 44 M. Path strings in asserts confirm the files. | 106 | M |

C2 feature clusters:

- **Altar of Rites UI:** the run at `0x7100177fc0` (27 functions) and `0x710017a700` (template `ConsoleDarkAlchemy:DarkAlchemyUI_main`).
- **Soul Shard / Sanctify inventory UI:** the run at `0x71003739a0` (30 functions; `AddSoulShardRanksButton`, `SanctifyLegendaryItemConfirmation`).
- **Community buffs (Ethereal, Sanctified, …):** the run at `0x71004a5ac0` (38 functions, `0x71004a61d0`).
- **Visions / nesting-portal community events:** `0x71004a7bb0` (`XVar` registration).
- **Set-dungeon clues and item table:** the run at `0x71004b1bb0` (45 functions; `p4_SetDung_Clue_*`, `class_clues*`).
- **Solo Self Found:** changes in `UIHeroCreate::Console` (an existing function, so phase D).
- **Followers' 14 slots and Emanate:** hireling inventory and UI; find through changed `Hireling*` functions.

## Phase D: changed game functions (parallel by file after phase B)

The remaining `M` functions that matched in 2.6.2, after phases A and B have taken the
shared causes. Work file by file. Order files by count of `M` functions with
`shape_similarity ≥ 0.98` (fastest first). Each agent owns a disjoint set of `.cpp` files.

| ID | Band (shape_similarity) | Functions today | Size |
|---|---|---:|---|
| D1 | ≥ 0.98 | ~1,600 | L |
| D2 | 0.90–0.98 | ~2,700 | XL |
| D3 | < 0.90 (real logic changes) | ~3,100 | XL |

D3 is where most patch-note behaviour lives (followers, Soul Shards, Altar powers, Paragon
and mode gates). Use the change guide's register IDs in commit messages and fix logs.

## Phase E: recomp readiness (later, in the recomp project)

- A non-matching but complete build: every `U` has source, so the link succeeds.
- The recomp project must take the 2.7.7 game data (romfs/CPKs) and the 2.7.7 generated
  sources from A1.
- The change guide's warning applies: 44 of the 91 `Patch2_7_7.cpk` payloads read as zeros
  with the current reader. Resolve that before relying on 2.7.7 assets.
- Platform-gated and season-gated code needs runtime configuration, not just code.

## Order and parallelism

```
A1 ─┬─ A4 ── B1 ── B2/B3 ── B4 ─┬─ D1 → D2 → D3   (parallel by file)
A2 ─┤                           └─ C3              (parallel by file)
A3  │  (independent)
A5  │  (any time)
C1 ─┴───────────────────────────── C2              (one agent per cluster)
C4    (independent)
```

- A1, A2 and A3 can run in parallel: different files.
- C1 is analysis only and can start now.
- B tasks are serial per struct.
- C2, C3 and D tasks parallelise by file ownership once B1 lands.

Rough total: about 120–250 agent sessions. Phase B decides how fast D goes; re-plan after
B1.

## Rules for every agent task

Same as the triage handoff (`build/handoff/triage_prompt.md`):

- Only `tools/check` changes Quality. Never hand-mark `O`.
- Never touch `data/2.6.2/`, `main.elf`, `build/dumps/`, the compiler or mixed-build split, or
  `tools/check`. Change the matching tools only when the task is about them, with tests.
- No guessed struct fields (`unk_<offset>` instead), no version `#ifdef`s, no invented names
  without evidence. `U` functions get names only when their source lands.
- **No regressions:** snapshot `data/2.7.7/functions.csv` first; every O stays O.
- Log every change in `build/analysis/2.7.7/triage/fix_log.md` (or a task log) with the
  evidence and the O count before and after.
- Leave changes uncommitted, or on a branch the user names, for review.

## Validation by the reviewing agent

For each finished task:

1. Diff against the task's starting snapshot. Only the task's files may change.
2. Run a full build and `tools/check` independently and compare O sets: no losses.
3. Read the source changes against the disassembly (`try_file.py --diff`). Check that
   layout changes are consistent across all affected functions, not tuned to one.
4. Re-triage and update this plan's counts.
