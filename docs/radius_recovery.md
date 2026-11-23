# Radius-trigger gameplay recovery

## Landed recovery, 2026-11-02

Commit `ff48d8f85f3d231dbcef5cee576bdf30a94f8354` lands all three functions
(1,028 original instruction bytes) and changes exactly their three inventory rows
from U to M. All three pass the linked checker in normal and Lua modes. This is
reviewed source recovery, **not exact certification**; no O row was added.
The source remains byte-identical to the earlier candidate documented below.

Production support is committed in `6fb8722`, with the preliminary receipt
comparison wiring corrected in `5c6ccc4`. Independent review and **209 passing
regression tests** cover the frozen tool baseline. The immutable scalar witness
checks 99 input pool sections, 850 modeled reference endpoints and two stationary
aliases per mode. The function-GOT witness checks all three callback bindings in
`BehaviorInit`. Both witnesses regenerate from the raw artifacts; no
checker diagnostic or data obligation is waived.

The final source candidate `76591ef930c2bcd4e0cdfbfccfed85b0a769401c` passed four
fresh baseline/candidate normal/Lua builds in **401.815 seconds**. Independent
review completed all 17 obligations (10 verified, seven safe-uncertified), with
no blocking findings. The existing integration transaction then ran four more
fresh builds and required the linked ELF, map/provenance, results and every
retained object to reproduce the reviewed artifacts. The integration transaction
took **541.830 seconds**, including its receipt replays. All 732 previously
compiled objects remain byte-identical in each mode; the new object contains
exactly the three functions, with no uncovered executable bytes or extra functions.

### Final review

The independent review SHA-256 is
`9c8085aefb1e23949a83c6d19da0089869bc1df723490e1b10c48a4bb5dec180`.
The final transaction's `landed.json` recorded the commit and `strict_certified: false`.

The prior v3 run failed: all four builds completed, but its
preliminary comparison omitted the function-GOT witness and stopped before the
actual audit. The correction has production-entry positive and negative tests;
v4 uses new builds and bindings. The abandoned Docker session was reconciled
only after reviewing that failure and checking that the process exited and the original
daemon had no active project containers. No old receipt was repinned or relabeled as accepted.

### Remaining exact work and next source unit

Six GOT operands in Init/Update remain unsupported by the strict object proof;
Dispose's strict object body/call comparison passes. The scalar witness relies
on the explicitly trusted frozen compiler/linker relocation contract and does
not establish unique original allocation identity or global indirect flow.
All named-data proof outcomes remain unchanged, including preexisting debt.
The ultimate completion criterion is still the exact original whole-game binary.

Touch recovery has since landed through the six-build queue; see
[Touch-input recovery](touch_recovery.md) for its actual results and remaining
exact-match debt. The following describes its earlier preparation checkpoint.

The next concrete source draft covered all four `TouchGestureTracker.cpp`
functions (712 original bytes). At that checkpoint it was uncompiled and
unaccepted. Two work items were recorded:

1. Implement and independently test a reusable proof for named initialized
   writable objects. Original `s_nTouchDelay` is a four-byte global initialized
   to 50; the prior 732-object census found no existing references. Re-census the
   post-Radius baseline before admission.
2. Review an inline constructor reconstruction by observable equivalence at
   authenticated original uses, including all-byte overwrite before read/escape,
   side effects, initialization/type-trait behavior, ABI and unchanged old
   runtime objects. The original `SwipeGesture` body is not known; absence of a
   call alone is insufficient. Require exact new function checks and no extra
   constructor symbol; do not invent source facts.

No external provider calls or pushes were made for this recovery.

## Preparation before landing

The isolated candidate `b6efc80fa1901091a68a1b9841109c93032f2a68` reconstructs
all three original `X/Client/Behavior/RadiusTrigger.cpp` functions on the landed
XP baseline `d052c6e`. The claim is `codex-radius`. Only that source file changes;
no shared header or production tool was edited. At this checkpoint, all three inventory rows remained U.

| Function | Original bytes | Object observation | Retained-input linked check |
| --- | ---: | --- | --- |
| `BehaviorRadiusTriggerInit` | 188 | Shape matches; two GOT words unsupported by strict object check | OK in normal and Lua modes |
| `BehaviorRadiusTriggerDispose` | 48 | Strict object body/call comparison passes | OK in normal and Lua modes |
| `BehaviorRadiusTriggerUpdate` | 792 | Shape matches; four GOT words unsupported; literal bytes correspond | OK in normal and Lua modes |

## Reconstructed behavior and validation

Original DWARF and instructions identify the 12-byte behavior data structure,
radius/tag defaults, randomized unsigned scan interval, player search, and
deferred one-shot deletion. The source uses the original existing allocation,
actor/definition lookup, tag-map and pooled-list interfaces. Missing free-function
declarations were authenticated against their original debug signatures; no
stub implementation or guessed object offset was introduced.

Independent source review checked all three functions and callback signatures,
including the `>=` scan comparison, exact `bDelete == TRUE` behavior, unsigned
delay and temporary lifetimes. The first single-file compile matched Init and
Dispose; Update had the correct size but six scratch-stack instruction words
differed. Original DWARF had no named WorldPlace local: passing its temporary
directly to `PlayersFindInRadius` restored the original lifetime and scratch-slot
reuse. The second compile emitted exactly three functions and matched all
**1,028 instruction bytes after relocation masking**, with no extras, in
**1.220 seconds**.

The second object SHA-256 is
`4937613d50d927040c333af97749b8059944410162c58797ca4e36ec07fe9175`.
The committed source SHA-256 is
`d052374c3c23fc007664d75cfbe1cfd1b3b256cf50250f3bff2968405e4add13`.
Independent object replay confirms 28 resolved direct calls and two literal
operands; six GOT words remain unsupported by the existing strict checker.

## Measured admission blocker

The actual object emits a four-byte anonymous `.rodata.cst4` allocation with
bytes `89 88 88 3c`, the same `1/60f` value read at original address
`0x7100e3bef4`. Update uses its ADRP/LDR address pair. Correct bytes and a matching
load do not by themselves establish original allocation identity, shared
consumers or the candidate linked storage.

Independent replay of the unchanged mapping gate on the final object stops with:

> Data mapping lacks authenticated original identity and consumer proof: .rodata.cst4

No full receipt build was started for this known blocker. Do not rewrite the
faithful float expression to hide the literal, waive the data gate, or treat
the matching shapes as source acceptance. The diagnostic work below supplies
concrete evidence for a reviewed recovery-only admission extension; the existing
linked, unwind, independent-review and integration checks still follow it.

The final independent artifact review SHA-256 is
`98d66c5c6dbb5b1f5d1633f80a95ff151bd6015710e8b1a58f9aa8153f64fbc4`.
No landing or exact credit is granted by this preparation.

## Retained-input linked experiment, 2026-11-02

At primary HEAD `db866584333a95970acc1510af5dcccecb0bd23a`, a reviewed private
driver reused the 732 sealed XP candidate objects in each mode. It reproduced
both baseline ELFs byte-for-byte, then added exactly the frozen Radius object
in the canonical game-object order. The compiler images, link flags, input
objects, maps, source and commands are pinned. Inputs were mounted read-only;
the resource/Docker guards serialized the four offline, bounded links.

All four links took **5.587 seconds**. Four isolated full-checker runs took
**27.529 seconds**, reproduced every one of the 15,842 prior baseline outcomes
per mode, and checked the three new functions. All three pass in both modes.
Checker inventory writes were confined to private copies. This reused one
previously compiled Radius object: it is **not a fresh two-mode compilation,
recovery receipt, landing approval or exact-match certificate**.

The experiment also found two concrete acceptance issues:

1. **The merged float pool reorders.** Its 176 unique four-byte values and
   704-byte extent are preserved, but 43 values move. The `1/60f` cell moves
   from `0x5dc` to `0x534`; its 19 old loads and the new Radius load resolve
   there. The initial unchanged-address/unchanged-pool test correctly failed.
   Do not relabel that test as passing.
2. **Existing callback validation changes.** `BehaviorInit` now resolves the
   three Radius callback functions. Its first failure changes from an undefined
   rebuilt GOT target to missing original *data* identity, because the pinned
   checker applies its data-symbol lookup to this function pointer. The current
   precise-debt gate rejects that changed diagnostic. The other 25 changed
   diagnostics per mode reproduce the existing call-failure equivalence proof;
   no unselected checker status changes.

## Finite evidence for the next policy change

The broader scalar census reads all 732 sealed input objects per mode and selects
compatible immutable MERGE4 sections by ELF properties. It covers 99 sections,
669 input entries and 1,696 references per mode, including named aliases and
address-taking forms. The 43 moved values have 275 ADRP producers and 276
LDR-S consumers. Decoded producer pages remain zero; the linked loads preserve
their four-byte values. Of 56 nonadjacent loads, 54 have no intervening GPR write
or control transfer. A bounded whole-function branch check resolves the two
`SetMipMapBias` cases under normal function entry. Loader-table checks find no
potentially overlapping pool relocations. Two named formatting-table aliases,
their address-producing ADD references and debug references refer to unmoved
cells at `0x518` and `0x51c`.

Independent comparison covers all 94 affected function contributions (67,528
bytes per mode), preserving non-relocation bits, including load destinations.
A whole-function ingress census finds no direct branch that skips the chosen
ADRP into any of the 276 intervals. It conservatively leaves 45 intervals in
14 functions unresolved because those functions contain indirect calls or
branches, all outside the intervals. Production recovery policy must either
classify the relevant local edges or explicitly rely on the frozen trusted
compiler/linker relocation contract. The diagnostics do not independently
prove global control flow or arbitrary interior entry behavior.

The callback evidence covers all three interleaved address/load pairs and stores
in `BehaviorInit`, not just the first reported failure. Original NSS function
identities and RELATIVE slots `0x10e9328`, `0x10e9330`, `0x10e9338` identify Init,
Update and Dispose. Both baselines have undefined named GLOB_DAT bindings; both
candidates have defined function targets and RELATIVE bindings. The caller
object is unchanged, and caller/provider map addresses and sizes are checked.

These are bounded diagnostics, not production admission proofs. Implement two
explicit, independently reviewed cases before spending on full receipts:

- A regenerated immutable-scalar-read witness that checks the complete affected
  pool/reference census, raw input and linked function contributions, relevant
  register flow, loader exclusions, and unchanged address-taking aliases.
  Unknown forms must fail closed; do not invent unique original allocation
  ownership or a complete original consumer census.
- A regenerated function-GOT refinement witness for an unchanged caller whose
  newly resolved targets are the selected recovered functions. Preserve the raw
  checker error and prove original/object/baseline/candidate bindings; do not
  accept a generic mismatch-to-mismatch transition.

Both cases need adversarial mutation tests and regeneration during receipt
verification and final landing. Preserve the tool versions used by
earlier receipts when changing production tooling. Freeze the new tools, rebase
the unchanged Radius source onto that baseline, then run the existing four
review builds and four integration builds. All three inventory rows remain U
until the recovery transaction succeeds; ultimate completion still requires
the exact original whole-game binary.

The link driver is intentionally single-use; its source/HEAD
pins and output-exists guard must not be weakened for a new run. Production
tools, source inventory and claimed Radius source were unchanged by this
experiment. No provider worker calls or pushes were made.
