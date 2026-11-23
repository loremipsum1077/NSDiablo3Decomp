# Recover source without claiming exact certification

Use this explicit path when useful, independently reviewed source recovery is
blocked by existing strict checker debt. Ordinary certification is unchanged:
`receipts.py` defaults to strict mode and rejects recovery receipts. Recovery
never grants O credit. Existing O rows stay unchanged; selected U rows become M
only after a successful landing. M means recovered but uncertified.

For exactly two complete owners, the separate [grouped recovery path](grouped_recovery.md)
shares builds while preserving per-owner evidence and atomic integration. Its
receipts and reviews cannot be used interchangeably with this single-owner policy.

## Scope and evidence

Version 1 supports one complete owner containing 1–64 original identities. Select
all its address/name pairs, including already recovered functions, in a JSON file:
`{"format":1,"functions":[{"address":"0x...","name":"exact_name"}]}`.
Qualified/ambiguous original names are unsupported. Source changes may touch the
selected owner and shared `.h`, `.hpp` and `.inl` files under `src/` or `include/`,
plus tests/docs. Both paths of a rename are checked against this scope. Shared
helper changes still require unchanged unrelated runtime objects and full review;
allowing a file extension does not admit changed consumer behavior. Recipe, checker, ownership,
data-ledger and other owner changes need a separate reviewed integration.

The gate freshly builds immutable baseline and candidate commits in normal and
Lua modes using identical pinned checker, original ELF, NSS, Docker images and
receipted Lua backend. It retains exact failure identities and diagnostic text.
Unselected proof results and unrelated runtime objects must stay unchanged. Data
proof outcomes must stay unchanged except missing named vtable or initialized
writable storage becoming fully verified through the bounded witnesses below. One bounded diagnostic exception resolves address-only changes in
named direct-call failures against each retained linked ELF: the original failure
site and target, displayed symbol identities, call opcode, and resolved endpoint
kind must agree. The caller object must be runtime-identical: same path, ELF
header (except `e_shoff`), section table (except `sh_offset`), every
`SHF_ALLOC` section's bytes, every non-debug relocation table, `.symtab`,
`.strtab`, groups and any other non-debug section. Only non-allocated
`.debug_*` sections and the `.rela.debug_*` tables patching them may differ in
size or bytes, so a faithful header edit that only changes unrelated callers'
DWARF no longer halts the gate. The witness records `caller_object_runtime_sha256`.
Defined function addresses and AArch64 PLT entries are authenticated; missing
expected targets must remain absent. The audit retains both raw records and a
structured witness. Unknown/unparsed changes, changed targets, relocation kinds,
proof classes, or ambiguous evidence still block, apart from the explicitly
authenticated Radius callback refinement below; no diagnostic text is erased.
Selected existing failures cannot change category or worsen;
previously absent U functions must become emitted. Previously proven instruction,
relocation or unwind evidence must not regress. Missing/error identities block.

Retained obligations expose owner instruction/relocation proof, defined data and
function extras, unwind, linked failures and source provenance/semantics. An
independent reviewer must inspect the actual diff and artifacts, explicitly rule
out unsafe ABI/data/relocation/unwind changes, and explain each unresolved exact
proof obligation. `safe-uncertified` records a justified recovery judgment; it is
not permission to waive a detected bug. Unsupported mechanical checks stop the
run. Do not alter diagnostic records, inventories or checker policy to pass.

AArch64 compiler mapping symbols are authenticated from their ELF metadata before
comparing their section, code/data kind and offset instead of assembler ordinal
suffixes. Raw symbols and the structured mapping witness remain independent review
obligations. Existing mapping sequences must remain identical; new mappings require
complete named function sections, with narrowly recorded non-allocated DWARF
location/range metadata exceptions. A previously absent owner object (every selected
identity U) also starts the per-object metadata every compiled object has: exactly one
data marker at offset 0 in `.comment` and `.debug_str` (PROGBITS, merge/strings),
`.debug_abbrev`, `.debug_info`, `.debug_line`, `.debug_macinfo`, `.debug_pubnames`,
`.debug_pubtypes` (PROGBITS, no flags) and `.eh_frame` (PROGBITS, alloc). Only those exact
section types/flags are admitted, only without a baseline object, and they are listed in
the witness as `new_object_metadata_sections`; `.eh_frame` contents remain covered by the
unwind obligations. Every other new data marker, including `.rodata*`, `.bss*`,
`.init_array`, `.data` and other `.data.*` (except the bounded proofs below),
still stops the gate. Nonselected new function names, including
aliases in existing sections, receive separate unreviewed-extra obligations; each
requires an independent decision on provenance, semantics, ownership, relocations,
data and unwind. Complete coverage is evidence for review, not source acceptance
or inventory credit. COMDAT groups are authenticated by signature and membership;
duplicate metadata names never excuse ambiguous runtime sections. Verification
reparses retained owner objects and rejects omitted or changed extra obligations. Malformed markers, references to markers and
changed existing mappings stop the gate. Ordinary named symbols and all instruction,
relocation, data, unwind and executable-coverage checks remain enforced.

New `.data.rel.ro` mappings are supported only for complete nonoverlapping named
vtable extents with a zero ABI header, zero RELA storage and complete ABS64 function
slots. The audit authenticates opcode-backed owner consumers, exact retained-object
link-map provenance, original/object/linked extents, aliases, contents and pointer
targets. The ordinary data validator must report `verified_equal`; unresolved weak
targets fail closed. Only `missing_rebuilt_symbol` to this fully witnessed proof may
change a data outcome. Retained raw data records and the structured witness are
independent review obligations, and verification recomputes the original/linked
proof from pinned inputs. Local static `.data` and anonymous constants outside
the following narrow case remain unsupported; names or byte equality alone
never admit them.

### Radius immutable-scalar and callback witnesses

The single-owner path also supports the previously absent
`X/Client/Behavior/RadiusTrigger.cpp` owner with its exact three original
identities and one four-byte `1/60f` scalar. `recovery_scalar.py` regenerates
the proof from the original ELF/NSS, complete sealed baseline/candidate object
inventories and authenticated linked images/maps. All old objects must be
byte-identical for this case. This additional restriction does not alter the
existing debug-only allowance for other recoveries.

The scalar proof checks the original and candidate load, all compatible
immutable MERGE4 sections and their references, the complete pool value set,
input/linked non-relocation instructions, relocated values and producer pages,
loader write exclusions, and stationary named/address-taking aliases. Unknown
forms, interior references, moved named aliases, extra objects or changed
consumers stop the gate. It explicitly relies on the pinned compiler/object
and authenticated linker relocation contract; it does not independently prove
global control flow or arbitrary interior entry. Unique original allocation
ownership, complete original consumer enumeration, runtime bindings and
whole-image exactness remain unproved. The `new-scalar-data-proof` review
obligation is mandatory and regenerated during verification. Ordinary mapping
calls without raw proof context still reject this new scalar.

The associated function-GOT exception is limited to the exact observed
`BehaviorInit` diagnostic change from an undefined rebuilt GOT target to
missing original data identity. `validation/recovery_function_got.py` proves
the unchanged caller, its original instructions/stores, all three original
NSS function identities, original RELATIVE slots, baseline undefined GLOB_DAT
bindings, and candidate defined function/RELATIVE bindings. All three callback
providers must be the selected Radius functions and have authenticated owner
object/map/code attribution. This is not a general mismatch-to-mismatch waiver.
Raw checker records remain unchanged; the `function-got-refinements` obligation
is separately reviewed and recomputed, including callbacks hidden behind the
first reported checker failure.

These witnesses do not admit named-data outcome changes or apply to grouped
recovery. They grant no O credit and do not replace source review, unwind checks,
the four fresh review builds or the four fresh integration builds. See the
[Radius evidence and handoff](radius_recovery.md).

Validator hashes are part of every receipt. Preserve the exact earlier tool
tree when changing policy; never repin old reports to new code.

### Named initialized writable storage

This opt-in single-owner profile supports one newly introduced global/default
four-byte object, with one authenticated adjacent GOT-pointer/load consumer.
It has no owner or variable-name whitelist. Other sizes, multiple objects,
preexisting owner objects or consumers, aliases, relocation-bearing initializers,
COMMON, TLS, RELRO and unmodeled consumers remain unsupported.

Supply `--dwarfdump PATH --dwarfdump-sha256 SHA` to the receipt build. The executable
is pinned alongside the original inputs and is automatically forwarded during
landing. These options are rejected in strict and grouped modes. Without this
profile, new ordinary writable data still fails the mapping gate.

Mapping only records pending structure. Admission requires a separate regenerated
proof: exact original ELF/NSS identity, DWARF defining owner, size, permissions and
initializer; sealed object-set and prior-consumer census; unchanged old runtime
objects; unique object-to-linked map attribution; genuine original and linked
GOT targets and consumer instructions; and the ordinary data ledger reporting
`verified_equal`. A missing data symbol may transition only under that witness.
Both the pending structure and final proof are mandatory review obligations,
regenerated during replay. A supplied JSON record or review cannot substitute for
raw proof. Unknown dynamic consumers and unseen-client semantics are not certified.

## Commands

Start from a clean primary checkout and an isolated claimed source worktree.
Commit only the intended candidate source locally. Keep the primary at the exact
baseline commit. Supply absolute paths and independently verified hashes below;
never reuse a hash from a different toolchain recipe or original dump.

```sh
python3 tools/receipts.py build --mode recovery-only --run \
  --repo "$PRIMARY" --baseline "$BASE" --candidate "$CANDIDATE" \
  --owner "$OWNER" --targets "$TARGETS" --output "$RECEIPTS" \
  --original-elf "$ELF" --original-sha256 "$ELF_SHA" \
  --checker "$CHECKER" --checker-sha256 "$CHECKER_SHA" \
  --lua-cache "$LUA_CACHE" --lua-receipt-sha256 "$LUA_RECEIPT_SHA" \
  --nss "$NSS" --nss-sha256 "$NSS_SHA" --seconds 3600
python3 tools/receipts.py verify --mode recovery-only --output "$RECEIPTS"
```

A completed build reports `recovery_evidence_complete`, not acceptance. Give an
independent reviewer the diff, binding/evidence/audit files, retained data records
and four build directories. They copy `review-template.json` to a separate review
file, identify themselves, set `independent` true, record any blocking findings,
and complete **every** obligation with a decision, rationale and evidence
reference. Pin that completed file's SHA256. The template is only an editing aid;
the gate derives requirements from the bound audit. Independence is an accountable
human/agent attestation, not a cryptographic identity guarantee.

```sh
python3 tools/land.py "$OWNER" --recovery-only \
  --recovery-receipts "$RECEIPTS" --review "$REVIEW" --review-sha256 "$REVIEW_SHA" \
  --original-elf "$ELF" --checker "$CHECKER" --lua-cache "$LUA_CACHE" \
  --nss "$NSS" --seconds 3600
```

Landing creates a separate integration worktree, applies the reviewed diff,
changes only selected U to M, commits locally, then freshly rebuilds all four
baseline/candidate modes. Actual integration artifacts must reproduce the reviewed
artifacts. Source, primary commit/branch, claims and input pins are checked again
before a fast-forward. Reserve exclusive Git access to the primary checkout during
landing: project locks coordinate claim/land tools, but arbitrary external Git
commands or direct file writers can race the final check. Do not switch branches
or edit the primary while landing runs. Failures before fast-forward preserve the
primary; every attempt preserves its transaction
under `build/recovery-land/`; do not delete evidence/worktrees before inspection.
If the final command reports an error, inspect HEAD and the retained transaction
before retrying: a fast-forward may have succeeded before writing its final receipt.
Successful `landed.json` explicitly says `recovery_landed` and
`strict_certified: false`. The worktree, source patch, review, receipts and prior
commit reference remain retained. No push occurs. Release the completed claim
only after checking retained evidence and preserving any wanted work.

An explicit single-owner integration optimization is available with
`land.py --reuse-reviewed-baseline`. Review still runs four fresh builds. Integration
reauthenticates the reviewed baseline's two modes and freshly builds
both candidate modes, reducing the total from eight builds to six. Default and
grouped landing retain their existing fresh-build policy.

Earlier checkpoints remain references to their original directories and command
records; no old receipt is copied, repinned or called fresh. Baseline source,
support, inputs, tools, targets and compiler images must agree, and integrated
source must equal reviewed source. Reference chains and candidate-mode references
are rejected. Every replay checks the original seals and command/source provenance.
Final candidate artifacts must still reproduce the reviewed artifacts before merge.
The landing record reports two fresh integration builds and two reused baseline
modes. Keep the referenced review directory for as long as its integration receipts.

The [prepared recovery queue](recovery_queue.md) can dispatch these existing stages
but does not supply its own independent review.

An optional contributor report exposes the reviewed recovery state without claiming
acceptance. It returns exit **2** (`incomplete`) even after review, because landing
and its fresh integration checks are still required:

```sh
python3 tools/contributor_check.py --repo "$PRIMARY" --baseline "$BASE" \
  --candidate "$CANDIDATE" --owner "$OWNER" --nso "$NSO" --nss "$NSS" \
  --original-elf "$ELF" --checker "$CHECKER" --output "$REPORT" \
  --recovery-receipts "$RECEIPTS" --review "$REVIEW" --review-sha256 "$REVIEW_SHA"
```

The shared parser requires `--nso`, but this recovery report does not re-verify NSO
conversion; it verifies the pinned original ELF/NSS/checker and retained evidence.
The report is not an exact-match certification receipt.

Strict consumers continue to reject these receipts. Exact certification and
baseline proof debt remain separate work. Recovery evidence cannot be
used to declare the entire game correct or promote any target to O. A later,
separate tier-1 promotion ([checker policy](checker_policy.md#two-tier-o)) re-evaluates
retained post-landing receipts and review; recovery landing itself never does.
