# Console touch-handler recovery

## Blocked source draft, 2026-11-02

`X/Client/UserInterface/Console/UIConsoleTouch.cpp` has one unrecovered function,
`Console::Touch::Update`, containing 208 original instruction bytes. Its isolated
source draft is commit `2132a5d4e2e0595be0a4a955d252b5f77182adf7`, based on the
reviewed handoff tools at `0e31339`. It is byte-identical to the compiled draft
`e781d46f41e89aee16ad6bb05c290c554eebba2c`, preserved on
`codex/ui-console-touch-before-handoff`. No header changes were made.

The draft implements the startup/screen guards, gesture collection, player and
conversation checks, right/left swipe predicates and objective refresh from the
original instructions and debug information. It calls the tracker functions
recovered in [the preceding Touch unit](touch_recovery.md).

Independent inspection of the actual object reproduced the mapping rejection
`Unsupported new mapping section: .init_array`. It contains 72 bytes of local BSS
(the two header objects plus the 48-byte tracker), an eight-byte init-array entry
and the extra 44-byte initializer.

The single-file compile succeeds, but the candidate is **not ready for recovery
admission or exact matching**. Its inventory row remains U. No full receipt build,
queue dispatch, source landing or exact promotion was performed for this unit.

## Concrete blockers

1. The original owner defines a 48-byte local zero-initialized state object,
   `_ZN7Console5TouchL14stTouchGlobalsE`, at original offset `0x1810718`.
   The current named writable-data proof accepts a bounded global initialized
   scalar, not this local NOBITS object or its address-passing consumers.
2. The compiled draft emits `_GLOBAL__sub_I_UIConsoleTouch.cpp` and an init-array
   entry. The original census found 350 init-array entries and none attributed
   to this owner. This is a real discrepancy; the census does not by itself prove
   all original runtime initialization or writer behavior.
3. `Update` is 212 bytes, four larger than the original. The extra instruction
   adds an offset to the aggregate BSS base because two header-local objects
   precede the touch state. The subsequent instruction/control-flow sequence has
   the original shape shifted by that instruction. Reference endpoints and exact
   linked behavior still require proof.

These findings do not justify deleting initializers, inventing field offsets,
forcing constant initialization or accepting the candidate with a weaker gate.

## Isolated initialization experiment

Six small controls use the same pinned normal compiler and flags as the draft:
three extracted current constructors, each unused or with its address exposed.
They change liveness only for diagnosis and are not proposed game source.

| Extracted current definition | Unused | Address exposed |
| --- | --- | --- |
| `SNOName` | No storage or startup initializer | Read-only values, no initializer |
| `WorldPlace` / `Vector3D` | No storage or startup initializer | Read-only values, no initializer |
| `TouchGestureTracker` / `TrackingState` | 48-byte BSS and initializer | 48-byte BSS and initializer |

The tracker control writes four zero start-time fields at startup. Its current
constructor alone is sufficient to produce the unwanted initializer. The two
header constants do not independently require initialization in these extracted
controls; their behavior in the full translation unit still needs explanation.
This experiment does not establish a compiler mismatch or the original
constructor body.

Original nonconstant inlined call sites support the reconstructed SNO and vector
field-value behavior, but optimized inlining cannot establish universal absence
of other effects. The original tracker-constructor declaration likewise does not
establish its body or constant-initialization behavior. Resolve that evidence gap
before editing shared constructors, then validate affected consumers.

The first probe launch stopped before starting a container because it named the
wrong existing compiler image. Its journal was reconciled after confirming that its process exited,
the original Docker daemon was unchanged and no project containers were active.
The six successful controls use the correct pinned normal image.

## Handoff exercise

The new [local handoff helper](recovery_handoff.md) passed its real read-only
preflight against this clean rebased candidate and derived exactly the original
owner target. That checks source identity, scope and pinned inputs; it does not
make the source build-ready. The known blockers are recorded separately, and no
manifest or queue job was published for this candidate. After parking its claim
as `stuck`, a real `--write` attempt was rejected before creating the export,
receipt directory or queue state.

## Original initialization and compiler investigation

Subsequent original-code inspection found four zero start-time writes in each
of three live tracker construction sites: `RingWrapper` construction at
`0x29e4d0`, `PlayerSkillsGlobals` construction at `0x295ab0`, and the banner
startup initializer at `0x118030` (tracker stores at `0x118130..0x11813c`).
This supports preserving the current field initialization. The outer constructor
is marked artificial in DWARF, but testing implicit versus explicit outer
construction did not remove the unwanted owner initializer. The original startup
census also includes `DT_INIT` and external dispatch; 350 enumerated array entries
are not a complete transitive writer proof.

A private late LLVM GlobalOpt experiment produces a 208-byte `Update` and exactly
48 bytes of BSS with no initializer. Its non-relocation instruction words match;
the two local-BSS address relocations remain unsupported. An observable-constructor
control retained its external call and volatile write. Neither result establishes
original compiler policy, local-data ownership, linked endpoints or exact matching.

The IR serialization route was rejected because its control changed five unrelated
`UISkillsConsole` functions. A direct clang extension avoided that confound after
using the compiler's matching C++ ABI and keeping the extension loaded until exit.
Three fresh serial normal-mode builds then tested stock, no-op extension and late
GlobalOpt on source snapshot `e3f2d1c`, which does **not** include this draft:

- All 734 stock/no-op objects, ELF, map and provenance matched byte for byte.
  All 36,559 checker records also matched.
- Late GlobalOpt changed 10 objects; 724 remained byte-identical. It improved
  two M rows but regressed five previously checker-OK O rows: the two RefString
  app initialization/disposal functions and three SigmaTiming functions.
- The regressions include removed initialization stores, narrowed storage and
  eliminated control flow. Other changes include data placement, initializers,
  local symbol splitting and unwind records. They are not merely address shifts.

The blanket pass is **rejected**. No production compiler change or Lua-mode
calibration was warranted after this normal-mode failure. The baseline already
had 1,525 O rows reporting mismatch; those are separate from the five new failures.
No inventory labels or acceptance gates changed.

The independent normal review SHA-256 is
`58939abcaf02bd2dbaea5c03bf9ff22903df87f350f2b6460c75f2935ddc7f01`.
The full object/checker analysis SHA-256 is
`09354f98189c14c111f16a305cf4982deabedc50258e0e18147bf4e676aab4b4`.

The local 48-byte zero-storage witness remains separate work. Keep its existing
ledger state unverified until owner identity, consumers, aliases, initialization
and linked storage are proved; do not fabricate global-object identity. Preserve
this blocked draft. Actual game-source work continues with
`UIActorTagControl` and its independently identified
[weak-provider integration issue](weak_providers.md).

## Accidental cleanup

A later status-note update used the legacy `claim.py stuck` command. After updating
the claim, that command treated a tracked-clean worktree as disposable and invoked
`git worktree remove --force`, deleting the worktree and its ignored diagnostic
files. Git metadata removal failed, leaving the source commit and registration
available. This was an unintended destructive action, not evidence retention.

The committed source was restored at the original path from `2132a5d4`; its
SHA-256 still equals
`8fb57ad09cbab689909e52af45ba59951f0b7c2a6dfa19d980b34453ead6bfe0`.
Shared original-input links were restored, and the worktree was locked against
automatic removal. The copied compile database is from the current primary
checkout, not the lost copy. Some ignored UI objects, IR, probe scripts
and custom plugin binaries have no surviving copy; independent reports cannot
recreate their original bytes.

Commit `b9acf2f` fixes `stuck` to retain worktrees and ignored evidence. Its isolated
real-Git-worktree regression test passes on the fix and fails on the prior code.
`done` already retained worktrees; explicit `release` remains a destructive
operation and was not used for recovery.
Do not treat recreated probes as earlier artifacts or repin old manifests.
Returning to this owner requires fresh compilation evidence for missing artifacts.
