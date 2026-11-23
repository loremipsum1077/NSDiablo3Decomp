# Execution plan: full exact decompilation first

**Latest owner decision, 2026-11-01:** finish complete exact binary matching before
desktop recompilation. This supersedes the native-first direction below. The
[exact-decompilation operating plan](exact_decompilation.md) is the current
priority order and AI job contract. M/source preparation remains useful work in
progress; it is not completion. Existing O labels still require current proof.
New prerequisite jobs P26-P31 are in `config/project_jobs.json`; P22 desktop work
is explicitly deferred. Existing checker/promotion policies are unchanged until
their separately reviewed replacements are implemented.

## Earlier subsystem-first plan and retained work

The earlier direction was approved by the project owner on 2026-11-01. It superseded the former
policy that runnable integration belonged to a separate project. The reference
remains Switch 2.6.2; the eventual runnable target is **native desktop first**.
Desktop OS support and graphics/audio providers are not selected yet.

## Separate results, shared evidence

- **Source recovery:** original owners, layouts, helpers, data and lifecycle paths
  reconstructed with review. M remains uncertified, not a behavioral guarantee.
- **Exact certification:** retain existing O policy, independent review and both-mode
  checks. Recovery U→M and later exact promotion are separate operations.
- **Runtime completeness:** account for missing code, globals, initialization, TLS,
  imports and platform implementations. Function percentages cannot measure it.
- **Desktop behavior:** a separate native target exercises recovered subsystems,
  then real asset loading, startup and eventually an offline gameplay slice.

The current source-job `done` status in project_plan.py still means the exact
inventory criterion. Do not reinterpret it as recovered-subsystem or runtime
completion. Future runtime metrics need explicit evidence, not relabelled O/M.

## First implementation increment

The shared-helper scope now admits `.inl` under `src/` and `include/`, with the same
source binding, independent review and unchanged unrelated runtime-object checks
as `.h`/`.hpp`. Rename screening checks both old and new paths. This removes a
file-type restriction, not the need to prove a changed helper or its consumers.
Existing receipts bind validator hashes: obtain fresh receipts after tool changes.
The tooling change alone accepts no parked source. A subsequent fresh bdStorage
pilot landed at `bd325be`: seven functions / 1,344 bytes U→M after independent
review, four review builds and four integration builds. No exact credit was granted.

`python3 tools/runtime_readiness.py --summary` inventories undefined symbols in an
existing comparison ELF and binds the inspected inputs. The initial local snapshot
has 3,885 distinct name/binding/type entries: 3,157 function-inventory candidates,
549 data-inventory candidates and 179 unclassified. It is not a fresh build.
Exact-name hits are leads, not identity, provider, reachability or linkability proof;
weak imports and unclassified symbols are not automatically missing game code.
The full command without `--summary` retains per-symbol candidate evidence. Keep
raw reports private under ignored build/; regenerate when the ELF changes.

The separate [desktop harness](../desktop/README.md) now compiles recovered Lua
object/string/allocation code with the native host compiler and runs two existing
behavior fixtures. Its allocation/GC doubles and fail-fast VM stand-ins are
explicitly documented. A native release build passed both tests on the current
macOS host; this is not a complete VM, desktop game executable or cross-platform
validation. The root matching build is unchanged.

## Ordered work packages and exit criteria

| Package | Work | Exit evidence |
| --- | --- | --- |
| 1. Shared helper pilot | Reinspect retained Domino/DemonWare candidates and current claims; select one complete owner whose helper changes preserve unrelated runtime objects | Fresh normal/Lua baseline/candidate receipts, independent source review and integration checks; otherwise record the exact remaining blocker |
| 2. Data witnesses | Admit owner-bound zero-fill/globals, constants and strings in separate small changes, then relocation-bearing data and jump tables | Negative tests for wrong extent/alignment/owner/target; original and linked evidence; a real data-bearing pilot |
| 3. Dependency groups | Permit a small explicit group of owners and shared helper definitions | All group identities and affected consumers protected; whole-owner extras/data/unwind accounting; one review bound to exact sources |
| 4. Large owner subsets | Select bounded address/name sets while protecting the full owner | Every unselected identity checked, including U; no qualified-name ambiguity or omitted emissions; preserved exact evidence |
| 5. Validation cost | Reuse immutable compiled baselines and validate compatible groups | Separate compilation/result cache keys, pinned requests/tools/images/backend/input/support hashes, retained bytes revalidated; no use of pruned receipts |
| 6. Desktop runtime | Separate native harness, file/data loading, platform boundaries, startup and gameplay | Explicit dependency closure and observable checks at each stage, without changing the Switch comparison recipe |

A first [two-owner grouped-validation policy](grouped_recovery.md) now implements
shared review/integration builds for exactly two complete function owners (64
identities combined). It admits no new data and keeps both claims and all proof
gates. General dependency groups, new data witnesses, subsets and cross-run cache
reuse remain future work.
The first shared-helper pilot is complete; its eight full builds demonstrate why
the new small-group path should replace serial validation for compatible pairs.
Fresh targeted compilation blocks the retained dmSphericalJoint + dmDistanceJoint
pair: its shared dmMath changes alter runtime content in the unrelated
Collision/Functions/dmDistance.cpp object. Do not spend full receipt builds on this
patch unchanged. Curl share.c + hostip4.c subsequently landed at `78fb712`:
four identities / 324 original bytes U→M, after independent review and eight
fresh builds shared across both owners. No exact promotion was performed.

The socket defining/consumer CU pair is a later dependency test, not a ready first
performance pilot. Its data-defining CU has no indexed functions. Supporting it
requires a separately authenticated data-only owner class; accepting an empty target
list is not sufficient. The [zero-fill diagnostic](data_identity.md#original-zero-fill-evidence) supplies only original-side
corroboration, with owner/loader-write/alignment proof still separate.
Rescan candidates and retained claims before assigning recovery instead of using
the October restart ranking. A group must not silently promote U→O. Neither agent
count nor CPU time replaces missing dependency or source evidence.

## Native desktop boundaries and milestones

Keep root CMake and its AArch64 comparison target as the reference. Desktop-only
adaptations belong in a separate target and should call recovered platform-neutral
logic where possible. Preserve original declarations/source evidence rather than
rewriting all matching code around a desktop library.

| Boundary | Existing evidence | Required desktop work |
| --- | --- | --- |
| Shell/lifecycle | `src/X/Common/Shell/ShellNX64.cpp` uses Nintendo lifecycle and services | Desktop startup/shutdown, focus and lifecycle contracts |
| Graphics | `src/X/Client/Gfx/GfxAPI.cpp` instantiates GFXNX64NVN; `NX64/NX64NVN.h` embeds NVN objects | Explicit rendering boundary and compatible resource/shader behavior; a library substitution alone is insufficient |
| Files/assets | `src/X/Sigma/Files/XFilesCommon.cpp`, `src/X/Common/Console/IO/ConsoleIO_ArchiveReader.cpp` | File-provider implementation and validated CPK/asset behavior |
| Threads/TLS | `src/BlizzardCore/Include/Blizzard/Thread.h` embeds nn::os TLS | Desktop thread/TLS lifetime and synchronization contracts |
| Input | `src/X/Client/Input/Input.h`; original InputNX64 implementation is absent | Desktop event and controller mapping |
| Audio/services | Sound declarations and original FMod implementation references; Nintendo and network service calls | Recover contracts and provider/ABI evidence; explicit offline service behavior, not success-returning placeholders |

Runtime milestones are incremental:

1. Native process runs a closed recovered subsystem with deterministic test-owned
   input and clean shutdown. This is a subsystem harness, not game boot.
2. Desktop file/CPK and threading contracts pass fixtures, followed by a bounded
   real-asset exercise with locally supplied assets.
3. Desktop shell/input and rendering boundary execute a controlled asset scene;
   initialization, allocator and resource lifetimes are checked.
4. Entry-point dependency closure supports startup, then a defined offline gameplay
   slice. Audio/network/platform gaps remain explicit until implemented and tested.

Do not wait for every function to match before investigating desktop boundaries.
Do not use absent imports or stub success returns to claim runtime completion.

## Completion assessment and next executable milestone, 2026-11-01

Snapshot at `763900e`, before the curl source pilot: 36,559 function identities,
including 20,726 U, 755 M, 6 W and 15,072 O. U bodies account for 8,502,496 original
bytes. Of 1,355 indexed owners, 631 have no source file. Seventy owners exceed the
current 64-identity limit and contain 8,812 U functions / 3,406,012 U bytes (about
40% of U code bytes). Grouping two small owners cannot address this portion.
These are workload counts, not a completion percentage or effort estimate.

The grouped smoke ELF at `c655027` has 3,935 undefined name/binding/type
entries: 3,186 function candidates, 549 data candidates and 200 unclassified.
These are inventory leads, not reachable dependency counts. Its data ledger has
402 verified-equal, 12 different and 10,649 unverified identities out of 11,063.
The root build allows undefined symbols and is a comparison artifact, not a
runnable game. Existing O labels also need fresh proof: this smoke requested
15,833 non-U functions, with 13,666 normal-mode and 14,039 Lua-mode checker OKs.
The remaining results were mismatches; do not advertise the label count as a
current exact certification.

The first meaningful desktop subsystem should be the **real Lua interpreter**.
Nine absent Lua owners contain 284 U functions / 86,728 bytes. Start with the
seven core owners lstate, lgc, lvm, ldebug, lbaselib, llex and lparser (221 functions),
then close liolib and loadlib with explicit desktop file/module contracts.
lparser alone has 69 identities and a qualified local identity (`body@lparser.c`).
A separately reviewed complete-owner limit increase is simpler than a subset
workflow for this file, but qualified-name mapping and data support are also
required. Raising the limit from 64 to 96 would make 29 unqualified owners with
2,104 U functions / 960,552 bytes size-eligible; other gates still apply. Constants
and globals require data witnesses. Existing Lua source also has documented ABI,
source-unknown and compiler obligations in [the Lua recovery record](lua_runtime_recovery.md).
Recovering the absent files alone does not close these obligations.

| Milestone | Observable exit |
| --- | --- |
| Real Lua core | Create state, load authenticated bytecode then source, execute deterministic scripts, exercise error unwinding and forced collection, and shut down with balanced allocation; no VM/GC doubles |
| Asset lifecycle | Real local file/CPK to SNO/reflection load and unload; registration order, ownership, malformed-input and failure paths checked |
| Controlled desktop scene | Selected desktop OS and render/input providers; real asset display and input with initialization and teardown exercised |
| Offline gameplay slice | Declared world/actor/inventory/power dependencies; deterministic spawn, move, action and save/load lifecycle |
| Source completion | Every original identity and owner accounted for; U/W and unresolved ownership addressed, M semantics reviewed, shared helpers/data/init/TLS/imports closed; external/platform replacements explicitly recorded and never counted as recovered original bodies |
| Native release | Reproducible clean build with complete reachable providers, real game startup and required offline behavior; no silent success stubs |
| Exact completion | Separate fresh original-platform certification of every in-scope function/data obligation, with existing proof debt resolved; a working desktop port does not establish binary equivalence |

Keep three coordinated work streams: source recovery along the Lua dependency
closure; narrowly reviewed data/subset support needed by that closure; and native
behavior fixtures that replace doubles as real implementations arrive. Reserve an
independent reviewer and integrate serially. Prove the two-owner path with the
small curl pair, then prioritize subsystem closure over unrelated easy functions.
Further build savings should use authenticated immutable baseline reuse, with
content and tool/input verification, after the real grouped landing works.

The mapping guard now accounts for allocated data independently of mapping
markers, including COMMON storage, existing bytes/metadata, data symbol identities
and relocation expressions. This closes an omission; it does not admit new globals.
Prioritize the concrete Lua literal and switch tables below. Zero-fill globals
remain a separate feature requiring checked original loader relocations,
defining-CU ownership and extent/alignment evidence. The current grouped policy
admits no new data: extending a single-owner witness alone will not admit a
data-bearing group.

### First real group and prepared Lua source

At `78fb712`, the curl group completed four review and four integration builds,
retained-artifact replay, independent review and final artifact reproduction.
All four selected functions passed both comparison modes; all 729 prior objects
were byte-identical in each mode. Review accepted 21 verified obligations and
two safe-uncertified global-data obligations preserving existing debt. Only four
U→M rows changed. Inventory is now U 20,722 / M 759 / W 6 / O 15,072.
This uses eight builds for two owners instead of sixteen serial builds; it does
not establish a measured twofold wall-time speedup. Review binding-to-audit took
about 6m24s; integration measured 584.13 seconds, including replay/reproduction.
The frozen data-guard
revision also passed all 170 recovery regression tests.

The next 40 functions are prepared, **not landed**: lstate at `d74c820` (10) and
lgc at `6095561` (30), in their supported claimed worktrees. Reference Lua 5.1.1
was authenticated against its publisher checksum. Original instructions establish
the game's extra `L->abort = 0` initialization, retained in lstate. Shared-header
changes are additive definitions in their original header owners. The two
branches share one header edit and need deliberate combination onto a current
baseline. Their current provisional `-O0` recipe is not an approved CMake change.

Both compiler backends emit all 40 names without extra function definitions.
Preliminary independent review checked 62 local calls and all 24 switch-table
target offsets. Lua-backend object checks find 12 exact functions, 26 equal-sized
functions whose differences occur at unsupported relocations, and two size
differences involving unused-argument/spill handling. These observations grant no
inventory or exact credit. Two narrow sanitizer smoke tests cover state
initialization and collector link/barrier operations; real allocation, protected
cleanup, finalizers, weak tables, threads/upvalues and complete collection remain
untested by those fixtures.

The next landing prerequisites are explicit:

1. Approve and bind the two owners' compiler recipes without putting an unreviewed
   CMake change into a source-only recovery candidate.
2. Authenticate lstate's 18-byte `not enough memory` literal and lgc's four
   relative switch tables (5/7/7/5 entries), including original extents, candidate
   references, dispatch bounds and linked target equivalence. Original addresses
   and object correspondences are leads; no new-data permission is implied.
3. Prove shared-header consumer preservation, complete independent source/artifact
   review and fresh receipts under an explicitly data-capable policy.
4. Expand native behavior coverage through the actual lifecycle, then recover the
   VM/debugger, base/lexer and parser dependencies toward a real interpreter.

Keep the lstate and lgc claims and commits intact;
the prepared source remains U in the primary inventory.

# Inventory metadata and retained technical holds

`tools/project_plan.py` reads committed inventories and core `config/project_jobs.json`.
It reports coverage and stable source/binary/milestone identities without running a
checker, provider or server. `SRC-` IDs derive from the exact original owner path;
functions use address plus qualified name, including any `@file` suffix.

```sh
python3 tools/project_plan.py --actionable
python3 tools/project_plan.py --job SRC-EXACT_ID
```

The core config preserves every curated technical scope, dependency, acceptance
criterion, deferral and coordination constraint from the promoted source
input. `reason`, `revisit_when` and `do_not_repeat` must remain explicit for deferred
jobs. Core sorts technical records by stable ID,
active/deferred/completed status and reads no live claims or queue state.

External tools can consume a versioned read-only snapshot:

```sh
python3 tools/core_api.py --repo /path/to/core --format 1 snapshot
```

Format 1 binds committed function/owner rows and the technical config hash. It
contains no dispatch authorization, fresh checker result or acceptance decision.

Source coverage counts only committed normal inventory O labels and original
function sizes. Nonmatches stay open; Lua-only results do not promote normal labels.
Counts do not establish fresh source/binary equivalence or measure total effort.
Acceptance also requires [independent source review and full gates](validation.md).

`config/component_rules.json` supplies symbol-family attribution leads. These do not
prove original ownership or approve exclusions. `evidence/checker/reconciliation.json`
retains the bounded indirect-flow investigation set; unresolved targets remain open.
