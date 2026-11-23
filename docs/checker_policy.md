# Checker trust policy and P19 audit

Audit baseline: `dd66601b04b24776de7de23a64f5c00f5571bb3d`. This is a source audit
of the checked-out Viking implementation and its reproducible patches, not a fresh
full-game validation. No inventory, ELF or vendored source was changed for this audit.
Any later fixes must retain negative tests and update this document with their evidence.

## What a match currently means

The project completion contract counts only inventory quality `O`. The checker uses
relocation-aware instruction equivalence, including an explicit trap substitution;
this is not literal equality of every byte in independently linked ELF files. A local
`try_file.py` result is provisional and cannot establish relocation target identity.

| Layer | Actual policy | Evidence |
|---|---|---|
| Local object comparison | Equal function size; compare four-byte words after masking relocation-filled bits. CALL26/JUMP26 mask 26 immediate bits, ADR/ADRP mask page/address bits, and supported ADD/LDST relocations mask lo12 bits. Relocation symbol identity is not compared. | `tools/validation/elf.py`: `MASKS`; `tools/validation/scoring.py`: `score`; `tools/try_file.py` |
| Local trap substitution | Original `0xe7ffdefe` accepts a rebuilt BRK of any immediate using mask `0xffe0001f == 0xd4200000`. | `tools/validation/scoring.py`: `score` |
| Full instruction comparison | Equal byte length, then Capstone instruction IDs and usually raw instruction bytes. Branches outside the original function are compared by target name/address; ADRP pairs use data lookup. | `tools/common/viking/src/checks.rs`: `check`, `check_range` |
| Calls and tail branches | Original target must resolve to an inventory/PLT function. Named target must exist in rebuilt symbols or PLT, and rebuilt target address must match it. Unnamed or unknown original targets fail. | `checks.rs`: `check_function_call` |
| Data | Only data represented by the loaded known-data map and recognized addressing forms gets target validation. Unknown references can pass. Data contents are not compared. | `checks.rs`: `KnownDataSymbolMap`, `check_data_symbol`, `check_data_symbol_ptr` |
| Function identity | Bare names use first retained ELF symbol with that name. `name@file` resolves a unique link-map suffix and retrieves the function symbol at its address. | `elf.rs`: `make_symbol_map_by_name`, `resolve_qualified_in_map`, `add_qualified_names` |
| Batch selection | Exact requested original address and name must agree with CSV; no fuzzy matching. Missing/invalid identities report `not-checkable`. Comparisons still use the same FunctionChecker. | `tools/check.rs`: `check_batch` |
| Quality mutation | A successful comparison can promote a row to `O`; a missing row is not a successful comparison. Default full mode can nevertheless exit successfully on missing built symbols. | `tools/check.rs`: `check_function`, `check_all`, `check_batch` |

The configuration `tools/config.toml` does not override
`check_unimplemented_references`, so its effective value is `true`. If set to false,
PLT targets are intentionally skipped; a missing PLT section also returns success
through `?` in that branch. Such a configuration must not be presented as strict target
verification.

## Two-tier O

`O` is **tier 1**. Every selected identity is checker `ok` in both normal and Lua
candidate results; `integration.comparison(before, after, targets)` reports
`regressions == 0` and `owner_not_ok == 0`; no candidate row is `error`; and a
recorded, hash-pinned independent source review exists. The review stays a
non-automated gate. **Tier 2** adds data/relocation target identity, unwind and
build-lineage proof. Tier-2 gaps are tracked per function, never hidden or waived.

Evidence for the split: all 1,525 existing O rows failing the current checker fail
only on proof gaps (unknown original GOT targets, rebuilt undefined GOT targets,
ADRP pairs under indirect flow, unproven jump tables), not on differing instructions.

`data/2.6.2/proof_debt.csv` (`Address,Name,Mode,Category,Reason`) lists every O
identity and mode that is not `ok`. `tools/proof_debt.py` regenerates it from both
modes' checker `result.json` and `functions.csv`; `--check` reports staleness.
Categories are stable slugs of the reason's first line; unknown reasons are `other`.

`tools/tier1.py promote` re-verifies retained receipts (bindings, attempt inputs,
file hashes, complete batch requests) and both modes' results, then changes only the
selected U/M rows to O and replaces their ledger rows in one local commit. It
fails closed on missing/changed receipts, review pins, scope or lineage and never
rebuilds. The commit message and retained `build/tier1-land/` record name the
pending tier-2 proof and review obligations accepted as `safe-uncertified`.
`--dry-run` and `scan` write nothing. `--allow-pruned` admits receipts
whose ELF/map/object copies were deleted, recording a build-lineage gap;
`--descendant` admits a later HEAD that changed no owner, header, toolchain or
checker input. Both are explicit opt-ins. It is a separate tool because recovery
receipts pin the hashes of the recovery validators.

## Findings requiring closure

These findings describe baseline behavior. They do not establish that a specific existing
O row is false; that requires a controlled fixture or revalidation of the affected row.

### F1 — incomplete disassembly can return success

`check_range` iterates `while let (Some(i1), Some(i2)) = (...)` and returns `Ok(None)`
after either iterator stops. It does not assert that both streams consumed every byte.
Equal function lengths alone cannot protect against an unsupported word followed by
changed instructions. The existing trap segmentation fixes the particular NX trap word,
but other undecodable words and asymmetric decode termination remain unguarded.

Required regression: place an undecodable word before a changed suffix, both symmetrically
and in only one stream; neither case may receive an exact result. Also cover non-word-sized
ranges. The comparator must distinguish complete comparison from unsupported decoding.

### F2 — trap equivalence exceeds exact encoding and is inconsistent

`check` accepts NX `0xe7ffdefe` against rebuilt bytes when byte 3 is `0xd4` and the top
three bits of byte 2 are `001`. That predicate does not check the fixed low five bits
required by BRK. For example `0xd4200001` satisfies it but fails the local BRK mask.
Even a valid BRK substitution is an intentional nonidentical encoding, including any
BRK immediate. Retain this as a named policy exception, rather than describing it as
literal byte identity. Tighten the predicate to the same valid BRK mask used locally.

Required regression: valid BRK, identical NX trap, invalid low bits, nontrap replacement,
and changed instructions after a trap. The last case must remain a mismatch.

### F3 — absent data evidence is treated as success

`KnownDataSymbolMap::load` omits CSV data names absent from the rebuilt symbol map.
`check_data_symbol` returns `None` for unknown original addresses; its caller then sets
`diff_ok = true`. `check_data_symbol_ptr` similarly returns `None` for a missing original
GOT section, non-GOT address, unreadable original pointer, unknown symbol, or missing
rebuilt GLOB_DAT entry. Those paths accept a relocated difference without resolving the
target. Plain ADRP accepts a changed page without requiring any subsequent verified use.

Required regression: wrong known data target must fail; unknown/missing data, missing
GOT mapping, direct load/store outside GOT and unconsumed ADRP must be distinguished
from verified equivalence. Closing this comprehensively requires P17 data evidence;
silently counting unchecked references as exact is not a closure strategy.

### F4 — data interior offsets are not preserved

Known-data ranges use rebuilt `st_size` at the original base. `get_symbol` can therefore
identify an original interior address, but `check_data_symbol_ex` compares the rebuilt
address against the symbol base, not base plus original offset. Original `symbol + 8`
versus rebuilt `symbol` can pass, while correct `symbol + 8` can fail. Zero-sized data
never covers even its base; overlapping ranges lack an ambiguity check.

Required regression: equal interior offsets pass; changed/dropped offsets fail; explicitly
cover zero-sized and overlapping symbols. Original range evidence must not be inferred
solely from a potentially incorrect rebuilt definition.

### F5 — normalized instruction paths do not compare every semantic field

The external `B`/`BL` path compares targets but not condition codes. Capstone represents
conditional B instructions under the B instruction ID, so equal targets with changed
branch conditions need explicit coverage. The ADD-pair path compares register IDs and
immediate values but does not explicitly compare operand shifts; the load/store pair
path skips entire memory operands and later compares only their base and displacement.
Index registers, addressing/writeback modes and other encoding bits need validation
before a relocation difference is excused. These are source-inspection risks requiring
executable negative fixtures, not claims about a measured frequency in the game.

The ADRP state tracks forward edges, not a fixed-point control-flow graph. Calls do not
explicitly clear ABI-clobbered pair registers; backward edges are not merged. Retain
forward-join tests and add clobber/backedge cases before claiming complete flow analysis.

### F6 — missing formerly matching symbols need a hard failure

`check_function` prints a warning and returns `CheckResult::Ok` on a missing rebuilt
symbol unless `--warnings-as-errors` is supplied. A missing U row and Library row are
intentionally skipped earlier. Missing O rows remain O. `tools/land.py` invokes the
checker without the flag and then detects lost O rows by CSV status, so that protection
does not independently catch disappearance of a formerly matching symbol.

Required regression: a missing O symbol, including unresolved qualified name, must fail
the acceptance gate. A missing U symbol should remain open. Batch callers must reject
`not-checkable` for every required comparison, not rely on process success alone.

### F7 — duplicate/static ownership and aliases remain evidence-dependent

Qualified suffix resolution rejects zero or multiple matching objects and never falls
back to the bare name. This is a useful protection. However, unresolved qualifiers are
skipped by `add_qualified_names`; default full checking then has the F6 warning behavior.
Bare duplicate names retain the first ELF symbol, with no ambiguity rejection. A stale
map pointing at another existing function address can satisfy the qualified lookup:
the code checks the address's type, not matching ELF name or a map/ELF generation hash.

ELF symbols sharing an address can act as aliases: call checks compare addresses, so
same-address aliases pass. That can be intentional constructor/destructor aliasing, but
it is not proof of original source ownership. Address maps keep the first ELF entry;
the original known-function map keeps the last row per original address. Neither creates
an explicit alias proof or rejects contradictory duplicate metadata.

Required regression: two same-named static functions with different bodies, ambiguous
suffixes, wrong-owner suffix, missing map, stale map, valid same-address alias, and
conflicting alias sizes. Retain source ownership review even after code equality passes.

## Patch provenance and safe validation

`tools/common/` is vendored and must not be hand-edited. Existing project changes are
retained as:

- `tools/patches/0001-viking-compare-across-trap-words.patch`: trap segmentation and
  ADRP state support.
- `tools/patches/0002-viking-name-at-file-link-map.patch`: qualified ownership.
- `tools/patches/0003-viking-batch-checker.patch`: exact batch identity selection.
- `tools/patches/0004-viking-flow-sensitive-adrp-pairing.patch`: forward branch pairing.

New fixes belong in reproducible project patches and should be tested in an isolated
copy with synthetic ELF/CSV/map fixtures. Do not run `tools/check` against shared live
inventory merely to probe a failure: full, single and batch paths can update CSV state.
Keep fixture results separate from original-function progress; no synthetic match adds
an O row. A clean build and full check belong to the serial landing workflow.

P19 is not complete solely because this audit exists. Closure requires retained positive
and negative evidence for the policies above, an acceptance gate that fails closed on
required but uncheckable functions, and explicit accounting for any remaining unchecked
reference classes. P20 must also tie the executed checker binary to the patched source
used to build it; inspection of source alone does not prove the installed binary matches.

## P19 implementation outcome

P19 closed the audit's bounded
implementation work: patch 0005 fixes F1/F2 and the external branch condition portion
of F5; the landing gate now uses `--warnings-as-errors` for F6. Synthetic fixtures cover
correct/wrong known targets, qualified owners, missing symbols and same-address aliases.
The full implemented inventory retains every prior O result. Earlier descriptions in
this document are explicitly baseline findings, not a claim these fixed paths persist.

P19 is complete as an audit with negative tests and revision/toolchain evidence. Remaining
remediation is not waived: P19-F01 covers F3/F4, P19-F02 covers residual F5, and P19-F03
covers F7. Their acceptance criteria live in `config/project_jobs.json`. P20 clean-build
reproducibility remains separate.

## P19-F02 follow-up

Patch 0006 closes the residual F5 operand and direct-control-flow work: 27 executable
fixture groups pass, while the isolated full regression conservatively rejects 423
existing O rows with unresolved indirect flow. None is silently credited as freshly verified. The installed checker and
live inventory remain unchanged. P19-F04 tracks indirect-target proof and rollout.
F3/F4 and F7 remain open under P19-F01 and P19-F03.

## P19-F03 follow-up

Patch 0007 closes F7's bounded identity checks: ambiguous bare functions and
conflicting aliases are rejected; qualified lookup requires matching ELF metadata
and link-time SHA256 provenance. It also closes PLT, literal-qualifier and exact-name
fuzzy-selection bypasses.
All 40 fixture groups pass, and the full 14,644-row comparison adds no rejections
relative to patch 0006. The existing 423 indirect-flow failures remain under P19-F04;
the installed checker is not replaced. F3/F4 remain P19-F01. Source ownership review
remains necessary: unsigned link provenance detects stale artifacts, not deliberate
rewriting of the artifacts and record together.

## P19-F04 bounded follow-up and deferrals

Patches 0008 and 0009 add guarded switch-table proof and recognized GOT slot
identity/offset checks. The pinned
full regression distinguishes fresh candidate acceptance from existing
O labels. Unknown indirect targets and unsupported switch shapes remain conservative;
R2's inferred tail calls are not treated as proven external targets. This does not
close all F3/F4 data-reference gaps or deploy a new installed checker.

Residual jobs are explicitly deferred in `config/project_jobs.json` with evidence-based
reopen conditions. Do not repeat the broad classification/audit on unchanged inputs
or infer closure from deferred status. Use another actionable job until a stated
trigger is met.

## P19-F01 follow-up: literal ADRP pages under unmodeled flow

Patch 0010 closes the unsupported-flow part of F3's unconsumed-ADRP finding. A
reachable BR or interior BL makes patch 0006 decline pair normalization, so no
ADRP pair reaches a use. The ADRP arm never compared its page, however: a changed
page passed whenever its lo12 use happened to be raw-equal, including with a wrong
target. Such functions now also require literal ADRP bytes. The rejection is
reported after the rest of the comparison, so existing diagnostics are unchanged.

`UIInventory::Console::FilteredInventoryView::sUpdateItemsForFilter`
(`0x710013c000`) ends in a memory-loaded tail `br`. It passed only because its five
rebuilt GOT loads coincidentally kept the original lo12 offsets. Unrelated link
layout changes flipped it to a mismatch with a byte-identical object. Rerunning
15,813 identities on four fresh links changed exactly this status in each baseline
link and nothing in either candidate link. Fresh setup therefore reports 1,525
unverified existing O rows in each mode instead of 1,524; this identity remains
under P19-F04-TAIL. `tools/test_checker.py` covers BR and interior BL, ADD and LDR
uses, right and wrong targets, trap segments and supported-flow controls.

Supported-flow variants remain open under P19-F01: a pair dropped at a conflicting
join, changed ADRPs reachable only outside the direct CFG (for example landing pads),
uses that are never normalized and ADRPs clobbered before any use.
