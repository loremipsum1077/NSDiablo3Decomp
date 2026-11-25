# Porting to game version 2.7.7

This fork targets Diablo III for Switch **2.7.7** (`C2.7.7.92380`, NintendoSDK 8.2.0)
instead of 2.6.2. The repository is named `NSD3Decomp2_7_9` from an early mislabel; the
binary's own version strings say 2.7.7, and every version label and path uses 2.7.7.
Patch-by-patch feature changes are in [the change-tracking guide](changes_2_6_2_to_2_7_7.md).
`tools/config.toml` selects the version (`default_version`) and compiler (`clang_version`);
`tools/build.py`, `tools/try_file.py`, `tools/land.py` and `tools/check` follow it. The
2.6.2 data is kept in `data/2.6.2/` and still checks with `tools/check --version 2.6.2`
(after building 2.6.2 with `clang_version = "5.0.1"`).

## Compiler: clang 7.1.0

2.7.7 was built with a clang 7.x. `tools/compiler_scan.py` recompiles 40 sampled
translation units (661 functions that match 2.6.2) and compares them with 2.7.7:

| compiler | exact vs 2.7.7 | of functions unchanged since 2.6.2 | of changed functions |
|---|---:|---:|---:|
| **7.1.0** | **74%** | **100%** | **60%** |
| 8.0.0 | 53% | 84% | 37% |
| 5.0.1 | 34% | 100% | 0% |
| 9.0.0 | 25% | 49% | 13% |

The close misses under 7.1.0 were game changes (a struct grew from 0x340 to 0x360 bytes,
an ID went from 700 to 731), not codegen. `-g` and the libc++ header version made no
difference; 7.0.1 has not been tried. The 5.0.1 LatePipe plugin and the weak-provider
link are 2.6.2-specific and are off for 7.1.0 (`D3_USE_LATEPIPE`, `D3_WEAK_PROVIDERS`).

## Mixed build: prebuilt libraries keep clang 5.0.1

Lua, BlizzardProtoBuf, DemonWare and OggVorbis are byte-identical between 2.6.2 and 2.7.7:
Blizzard linked them prebuilt, still compiled with the old toolchain. `tools/build.py`
therefore builds 2.7.7 in two phases:

1. **Legacy phase** (`build/2.7.7-legacy`, image `d3-lua-build:5.0.1`): only those four
   libraries, with the full 2.6.2 recipe (clang 5.0.1, LatePipe, the Lua backend);
   `D3_LIBRARY_SET=legacy-only`, objects only, no link.
2. **Main phase** (`build/2.7.7`, image `d3-build:7.1.0`): everything else with clang 7.1.0;
   `D3_LIBRARY_SET=exclude-legacy` links the legacy objects from `D3_LEGACY_OBJECT_DIR`.

The library list is `project_version.LEGACY_DIRS` (mirrored by `D3_LEGACY_REGEX` in
`CMakeLists.txt`; `tools/test_mixed_build.py` keeps them in step). `tools/try_file.py`
compiles a file from those directories the legacy way, and `function_files.csv` labels
their functions `5.0.1`. The mixed build needs the Lua backend
(`python3 tools/backends.py --prepare-lua --allow-downloads`) and the LatePipe plugin, as
the 2.6.2 build does.

| library | O before (all 7.1.0) | O with the mixed build | + mapping repairs | functions |
|---|---:|---:|---:|---:|
| Lua | 0 | 634 | 636 | 683 |
| BlizzardProtoBuf | 216 | 1,012 | 1,012 | 1,058 |
| DemonWare | 789 | 1,636 | 1,935 | 2,226 |
| OggVorbis | 48 | 182 | 185 | 193 |

## Setting up a checkout

1. Convert your 2.7.7 `exefs/main` to `data/2.7.7/main.elf` with the pinned `nx2elf`:
   `nx2elf main --export-elf data/2.7.7/main.elf`. Keep `data/2.6.2/main.elf` too.
2. Download the official `clang+llvm-7.1.0-aarch64-linux-gnu.tar.xz` (274,473,812 bytes,
   [llvmorg-7.1.0 release](https://github.com/llvm/llvm-project/releases/tag/llvmorg-7.1.0))
   into `build/toolchains/downloads/`, then `python3 tools/build.py --setup-image`. The
   image (`toolchain/Dockerfile.clang7`) refuses an archive with a different SHA-256.
3. Generate the game-data sources as for 2.6.2 (`tools/bootstrap_game_data.py`), and
   prepare the 2.6.2 toolchain for the legacy phase: the `d3-build:5.0.1` /
   `d3-lua-build:5.0.1` images and the Lua backend (`python3 tools/backends.py --check`).
4. `python3 tools/build.py --jobs 8`, then `tools/check`.

## How data/2.7.7 was made

- `tools/version_diff.py` matches every 2.6.2 function to 2.7.7 (eh-frame boundaries,
  address-masked instruction hashes, link order, call graph) and writes
  `build/analysis/2.7.7/version_diff/{mapping.csv,summary.md}`.
- `tools/port_version.py` writes `functions.csv` (2.6.2 names; every function that had
  source starts as `M`, new functions are `U` / `sub_<address>`), `function_files.csv`,
  `duplicate_names.csv`, and `data_symbols.csv` (6,303 of 11,063 data symbols, located by
  lining up address computations, through GOT slots, in matched functions).
- `tools/check` then promoted every `M` that matches. All-7.1.0 build: 20,222 O. Mixed
  build: 22,632 O. Mixed build with the mapping repairs below: 23,427 O. After the first
  shared-cause fixes and the GlobalSNO renumbering: 23,786 O. After the protobuf
  regeneration: 24,292 O. After the first struct-layout fixes (branch `structs/2.7.7`: ActorCommonData, HeroInfo/UserRuntime, HUD): **25,790 O**, 6,130 M, 2,906 U, 2,175 L, 4 W (of 37,005 functions).

### Mapping repairs (look-alike functions)

Tiny functions with identical masked code (empty destructors, `ret`/`b` stubs, SWIG
wrappers) can only be paired by link order, which guesses wrong where the layout shifted.
A wrong name on one of them breaks every caller: one mislabelled destructor
(`bdReferencable::~bdReferencable` named `bdAuthenticationTag`) alone failed 189
DemonWare functions with `wrong function call`. `version_diff.py` now repairs pairings
after the order and call-graph passes:

- **Callers** (`repair_by_callers`): callers whose code is identical in both builds (and
  are hash-matched or at least 32 bytes) vote, call by call and tail call by tail call,
  for which new function each callee is; a 75% majority overrides the pairing.
- **Neighbours** (`repair_by_neighbours`): a look-alike sitting next to a strongly matched
  function takes the identical function next to that function's partner, if that gives
  strictly more neighbour agreement than both its current pairing and the pairing it
  would displace.
- Unique-hash pairs are never moved; freed functions get a final order/call-graph pass.

Against the pre-repair mapping: 797 functions gained `O`, and two 4-byte functions
(`Blizzard::Lock::RefCounted` D0, `UIInventoryViewItemsProvider::OnPlayerChanged`) lost
their name to the function their hash-matched neighbours on both sides point to; those
addresses are still `O` under the corrected names.
- Two `blz::` string/vector templates matched only by accident in the all-7.1.0 build (the
  linker took a 7.1.0 BlizzardProtoBuf copy); their owners are game files
  (`OnlineServiceClientBase.cpp`, `OnlineService.cpp`), which do not emit them yet. They
  are `M` until those files do.

## GlobalSNO / GlobalGBID renumbering: `tools/global_sno_map.py`

2.7.7 inserted new global SNOs and GBIDs, so most enum values in `src/X/Common/GlobalSNO.h`
moved (by +1 up to +97). `global_sno_map.py` reads both builds' registrations straight from
`main.elf` (the `sLookupSNO` calls in `GlobalSNOListRefresh`, the `sgtGlobalGBIDs` rows) and
pairs entries by asset group and path, or by GBID name and type. Entries that share a path,
and the names neither build registers, pair in order. On 2.6.2 it reproduces all 1,980
registrations of `sno_calls.inc` exactly.

- 1,911 / 1,911 GlobalSNOs and 119 / 119 GBIDs mapped; 1,710 and 89 values renumbered.
- 106 GlobalSNOs and 4 GBIDs exist only in 2.7.7; they are `GLOBALSNO_UNK_<n>` /
  `GLOBALGBID_UNK_<n>`, commented with their asset path or name (for example
  `Crafting_Legendary_Primal_01`, `SwarmRiftKey`).
- `NUM_GLOBAL_SNOS` 1928 → 2034, `NUM_GLOBAL_GBIDS` 119 → 123.
- `python3 tools/global_sno_map.py` reports; `--apply` rewrites the two enums (run it on the
  2.6.2 values; it refuses any name it cannot place).

Result: **+301 O** (23,485 → 23,786), none lost. `generated-game-data/sno_calls.inc` and
`gbid_rows.inc` still hold the 2.6.2 registrations, so `GlobalSNOListRefresh` and the GBID
table do not match yet.

## Protobuf bindings: `tools/bootstrap_protobuf.py`

The `.pb.cc`/`.pb.h` files under `src/X/Common/Protobuf/` are generated from the game, so
they stay ignored by git. Each checkout must generate them once:

    python3 tools/bootstrap_protobuf.py             # writes the 2.7.7 bindings
    python3 tools/bootstrap_protobuf.py --check     # 0 of 48 differ when up to date

- `tools/game_descriptors.py` finds every function that calls
  `DescriptorPool::InternalAddGeneratedFile(data, size)`, named or not, decodes the
  ADRP/ADD and MOVZ/MOVK arguments, and reads each serialized FileDescriptorProto from
  `main.elf`. No `.nss` is needed.
- The descriptors are compiled with the pinned protoc 2.6.0 and adapted as
  `bootstrap_game_data.py` does for 2.6.2 (`adapt()`, now with the 2.7.7 build-server path
  in `#line`). `--version 2.6.2` reproduces the 2.6.2 bindings exactly.
- A header keeps its 2.6.2 `layout/<Name>.inc` size checks only if its descriptor is
  unchanged. 35 of 48 files changed. Ten messages grew, which changes the layout of the
  game structs that embed them:

  | Message | 2.6.2 | 2.7.7 |
  |---|---|---|
  | `Account::ConsolePartitionData` | 0x48 | 0x58 |
  | `Account::DeliveredAchievementReward` | 0x20 | 0x28 |
  | `GameMessage::AchievementsCriteriaUpdateConsole` | 0x30 | 0x38 |
  | `Hero::Digest` | 0xa0 | 0xa8 |
  | `Hero::LoadoutItem` | 0x30 | 0x38 |
  | `Items::Generator` | 0xd8 | 0xf0 |
  | `Leaderboard::Member` | 0x78 | 0x80 |
  | `OnlineService::ContentHandle` | 0x30 | 0x38 |
  | `Profile::HeroMiniProfile` | 0x38 | 0x40 |
  | `Client::GameAccountSettings` | 0xe8 | 0xf8 |

- Fixes needed to build: `InactiveItemMarker` is 0x1b8 (holds a `Generator`),
  `PartyMember` 0x1d8 (holds a `Digest`), `PartyDisplayData` 0xdef0. Some `Digest_Flags_*`
  values were renamed `*_DEPRECATED` (same values).
- The mapping had put `Items::Generator::{Get,Set}CachedSize` on another message's
  accessors (`_cached_size_` at 0xd4). They now sit at 0x60d4a0 and 0x619240 (offset 0xec).

Result: **+506 O** (23,786 → 24,292), 489 of them protobuf functions; no function lost.

## Finding new code: `tools/string_anchors.py`

2.7.7 ships no `.nss`, so its new functions are `U` / `sub_<address>`. `string_anchors.py`
finds functions through the strings they use (ADRP/ADD/LDR, ADR, literal and GOT loads,
pointer slots from `.rela.dyn`, and pointer tables whose start code references):

- `python3 tools/string_anchors.py [STRING ...]`: the functions using each string, with
  callers and callees. With no arguments it uses the seven anchors of
  [the change-tracking guide](changes_2_6_2_to_2_7_7.md).
- `--new-strings`: strings of 2.7.7 that 2.6.2 lacks and code uses, ranked by unnamed users
  (686 today; source-path strings are compared without the build server and branch).
- `--owners`: a proposed source file for unnamed functions, from a source-path string they
  reference (58) or because the nearest functions on both sides with a known file agree
  (1,547): 1,605 of 2,906.

Results are leads: confirm the function's behaviour before naming it. `--json` gives
machine-readable output.

## Known gaps (next work)

- **DemonWare** has 291 functions that are not `O`: 273 were never `O` in 2.6.2 either and
  2 are `L`. Of the 16 that were `O` in 2.6.2, 7 are in 2.6.2's `proof_debt.csv` (the
  checker never proved them), 7 fail checker proofs in 2.7.7 (unproven jump tables or
  ADRP pairs, one unlocated data symbol `bdJSON::BD_JSON_TRUE`), and the two
  `bdDataChunk::deserialize` overloads really changed size (44→48, 576→588 bytes).
- `data/2.6.2/{proof_debt,helper_ledger,inline_helpers}.csv` have no 2.7.7 counterparts yet.
- **Changed game code:** 7,389 functions that matched in 2.6.2 now differ (struct layouts,
  constants, logic). The diff's `shape_similarity` ranks the easiest first.
- 3,576 new functions have no names (no `.nss` debug file ships with 2.7.7).
- 4,760 2.6.2 data symbols are not located yet, so the checker cannot verify references
  to them.
- The evidence/receipt/recovery tooling (`tools/recovery*.py`, `receipts.py`,
  `contributor_check.py`, ...) still assumes 2.6.2 and the 2.6.2 `.nss`.
