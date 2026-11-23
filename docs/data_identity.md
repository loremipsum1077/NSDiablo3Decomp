# Data identity for checker proof gaps

Phase-1 analysis for P19-F01 (F3/F4). Nothing here changes the checker, its policy,
`data_symbols.csv`, inventories or quality labels. It covers two things: deriving
original identities for unverified data references from pinned original inputs,
and a design for proving anonymous read-only references by content.

## Reproduction

```
python3 tools/data_identity.py \
  --result "$NORMAL_RESULT" \
  --result "$LUA_RESULT" \
  --f3 "$F3_CLASSIFICATION" \
  --proposed data/2.6.2/data_symbols.proposed.csv \
  --rows build/data-identity/rows.json --report build/data-identity/report.json
PYTHONPATH=tools python3 -m unittest tools.test_data_identity
```

The tool reads `data/2.6.2/main.elf` (the checker's original), the NSS `.symtab`
and DWARF (`llvm-dwarfdump`, cached under `build/data-identity/`, keyed by the NSS
SHA-256), `data_symbols.csv`, `functions.csv` and `function_files.csv`. The report
records every input's SHA-256, and two runs give byte-identical output. Proposed
rows use the columns `Address,Name,Evidence`. `data/*` is gitignored apart from the
six reviewed inventories, so the proposal stays an untracked file that can be
regenerated.

For each checker GOT row, the GOT slot is recovered from the reported instruction
(an LDR is paired with the nearest preceding ADRP of its base register, and an ADRP
with the next LDR from it). The target then comes from the slot's RELATIVE or
GLOB_DAT relocation in `main.elf`. All 1,215 O-labelled GOT rows resolved this way,
and every one of them is a RELATIVE slot. The scan is linear, not flow-sensitive.
Its results are consistent with the checker's diagnoses: every "lacks identity"
target except one explained case is absent from the CSV, and every
"rebuilt-undefined" target has an original identity. Consistency is not proof.

Identity derivation, in priority order: an existing CSV base; a named NSS
`STT_OBJECT/FUNC/TLS` symbol whose **original** extent covers the target (mapping
symbols never count); a DWARF `DW_OP_addr` variable (this includes function-local
statics and `_MergedGlobals` members via `DW_OP_plus_uconst`); a `functions.csv`
entry for function-pointer slots. Names carried by several NSS or DWARF copies are
qualified as `name@TU-suffix`. The qualifier comes from the defining compile unit
(`DW_AT_name` joined with `DW_AT_comp_dir`), never from `decl_file`: all 278 NSS
copies of `blz::ignore` declare `blz/tuple.h`. A copy without a distinct owning TU,
distinct names at one extent, and overlapping extents are reported as ambiguous and
never proposed.

## Results (identical in normal and Lua modes)

**GOT target lacks identity: 759 O rows, 150 distinct targets.** 149 targets
(758 rows) are function pointers into `.text`. They are callbacks, `__cxa_atexit`
destructors and protobuf init/shutdown hooks. Their identity is the NSS `STT_FUNC`
symbol, equal to the `functions.csv` name. All 149 names are bare, and 4 targets
(7 rows) are same-extent D1/D2 destructor aliases. One target (1 row) is already in
the CSV: `cgwvInitCameraForward` (`.rodata`, `X/Client/Camera/Camera.cpp`). The
checker dropped it because the rebuilt map lacks that bare name. A plausible cause
is that our namespace-scope `const` has internal linkage (`_ZL…`). None of the 759
needs DWARF-only or `.bss/.data` evidence. The gap is structural: GOT verification
consults only the data CSV, never function identity.

**Rebuilt undefined GOT target: 456 O rows, 155 distinct original targets.** 151
are data globals already named in the CSV (116 `.bss`, 20 `.data`, 15 `.rodata`),
and 4 are `.text` functions. These need source definitions, not identities. Grouped
by defining TU, there are 50 owners. 29 owners whose file is absent from `src/`
account for 313 rows. 17 present files that lack the definition account for 136.
The top owners:

| Owner (defining TU) | O rows | Globals | In `src/` |
| --- | ---: | ---: | --- |
| `X/Client/UserInterface/UIGlobals.cpp` | 181 | 78 `UIDLG_*`/`UIWND_*` IDs | no |
| `X/Sigma/TypeDescriptor/TypeDescriptor.cpp` | 51 | `ComplexTypeDescriptor::pRegistrationHead` (42), 2 counters | yes |
| `X/Common/OnlineService/OnlineService.cpp` | 39 | `OnlineService::INVALID_HERO_ID` | yes |
| `X/Client/Gfx/Gfx.cpp` | 28 | `gptGfxData` (22), `cgrectViewportDefault` | no |
| `X/Client/Sound/Sound.cpp` | 27 | `cgtPSODefaults` | no |
| `X/Client/Gfx/NX64/NX64NVNCore.cpp` | 13 | `GFXNX64NVN::globals`, 8 `nvn::pfncpp_*` | no |

The full ranked list is in `report.json` under `modes.*.got_undefined_owners`.

**F3 held-candidate rejections: 2,371 normal / 2,450 Lua rows (598 / 664 distinct).**

| Class (normal rows) | Rows | Distinct |
| --- | ---: | ---: |
| Anonymous `.rodata`, NUL-terminated text | 2,003 | 541 |
| Anonymous `.rodata`, target byte is NUL | 224 | 9 |
| Anonymous `.rodata`, other bytes | 11 | 10 |
| Named NSS local, qualified (`blz::ignore`, `vwin`, `mask`, `FLOOR1_fromdB_LOOKUP`, …) | 122 | 28 |
| DWARF-only `_ZN6bdJSONL15BD_JSON_NULLSTRE@bdJSONValue.cpp` | 1 | 1 |
| Anonymous `.data` (≤1,928 bytes after a DWARF variable of unmodelled size) | 9 | 8 |
| CSV-known `quant_look`, absent from rebuilt names | 1 | 1 |

Lua adds 70 text and 9 NUL rows. 216 of the 224 NUL rows reference a single address,
`0xd6a4a3`, a lone NUL after a terminator, which is the deduplicated `""` literal.
The other 8 NUL targets and the 10 "other" targets sit in aligned constant tables.
No text target sits in a string-tail position.

`data/2.6.2/data_symbols.proposed.csv` has 182 rows. 153 are functions, 149 of them
GOT-unknown and 4 rebuilt-undefined; adding the undefined 4 would not help. 28 are
qualified NSS locals and 1 is a DWARF variable. All 29 qualified names need checker
support before they could take effect. `KnownDataSymbolMap::load` does a bare-name
lookup, and `add_qualified_names` resolves only `functions.csv` names.

## Content-anchored identity proof (design)

**Claim.** An original reference R in a verified function F, to address T, and the
rebuilt reference R′ at the same instruction in F′, to address T′, are equivalent
when every one of the following holds:

1. **Read-only, unnamed original target.** T lies in a non-writable, non-executable
   allocated section. No named NSS symbol and no DWARF variable covers T. Mapping
   symbols are ignored. A covering identity means the identity proof applies
   instead, and content equality is never accepted as a substitute for it.
2. **Mergeable rebuilt target.** T′ lies in a linker merge-synthetic section. In
   ld.lld's map these are `<internal>:(.rodata)` input sections, bound to the ELF by
   the existing map provenance. No rebuilt symbol covers T′. This is the evidence
   that the compiler treated the object as having content-only identity. The
   original image has a single merged `RO` segment and cannot show this.
3. **Defined extent.** For a string, the extent is T up to and including the first
   NUL, and the bytes before the NUL must be text. For a constant, the extent is the
   access width of every use of the materialized address in F and F′: `LDR q`, `s`,
   `d` or `x` of the pair, or an `LD1` element count. The extent must be at least 1
   and is bounded, with no "read to end of section".
4. **Byte equality without relocations.** original[T, T+n) == rebuilt[T′, T′+n), and
   neither range overlaps any relocation record (`relocated_bytes` excludes them).
5. **Anchor.** F/F′ are the identities the checker already matched (same name, same
   instruction index, same register semantics). The proof is per-reference. It never
   produces a reusable address mapping for other functions.

**Why this is sound under those conditions.** C and C++ leave the distinctness of
string literals and compiler constants unspecified, so a conforming program can
observe only their bytes. A merge-section object has no address identity the
linker must preserve, so equal bytes over the whole accessed extent give equal
observable behaviour.

**When it is not sound (must stay rejected):**

- A writable target (`.data`/`.bss`) is never covered: it has state and address
  identity. This applies to all 125 F3 writable rows.
- Named objects, even `const` ones, have distinct-address semantics and may be
  compared by pointer or modified through `const_cast`. They need the identity proof.
- An unknown extent. A NUL-target or constant-table reference materialized by
  ADRP+ADD and passed out of F (to a call, a store, or a return) has no local width.
  Equal first bytes then prove nothing, for example `0.0f` against `""` or an
  unrelated zero table. The 224 NUL and 11 other rows are therefore only covered
  when condition 3 fixes the extent. For the `""` literal, that requires condition 2
  plus string-shape on both sides.
- Interior and tail references, where T is not an object start. Tail merging can
  alias `"bar"` into `"foobar"`. If F does pointer arithmetic below T or reads past
  the NUL (fixed-size `memcpy`, string tables indexed past their end), the extent is
  not the C string, so the proof must reject. None of the measured targets are tails.
- Address-as-key uses: a literal used as a unique tag compared by pointer. The proof
  covers content only and cannot detect this. If F compares R against another
  address, the check must fall back to the identity proof.
- A different instruction position or register, or a reference reached under
  unmodelled flow (P19-F04). The anchor would not be established.

**Coverage, original side measured, rebuilt side not measurable now.** The rebuilt
links behind the F3 replay have been pruned, so
conditions 2 and 4 cannot be evaluated. Conditions 1 and 3
(text shape) hold for **2,003 / 2,371** normal rows and 2,073 / 2,450 Lua rows. The
`""` literal adds 216 rows and constant tables add 19, but only with use-width
evidence. These are upper bounds that hold only if the rebuilt bytes match.

## Estimate (upper bounds, first-mismatch only)

Checker results report only the first mismatch per function, so clearing it can
expose later failures.

| Population | Verifiable after phase 2 | Remaining |
| --- | ---: | --- |
| 1,525 failing O rows | ≤758: the 149 function identities (bare names, existing mechanism, or a checker GOT→`functions.csv` lookup) | 1 renamed const; 456 need source definitions; 160 ADRP-under-indirect-flow and 147 jump tables need P19-F04 flow proof; 3 other |
| 2,371 F3 rows | ≤2,126 strict (2,003 content + 123 qualified identities, the latter only after qualified-data-name checker support); ≤2,361 if NUL and constant uses get width proof | ≥10: 9 anonymous `.data` (needs DWARF type sizes) and `quant_look` |

Unverified here: the rebuilt side of every estimate (no matching rebuilt ELF is
retained for either result set); flow-sensitive GOT pairing; DWARF type sizes, so
DWARF variables never cover interior offsets; and whether `@`-qualified data names
resolve uniquely in the rebuilt link map.


## Original zero-fill evidence

`tools/original_zerofill_evidence.py` is a read-only prerequisite diagnostic. It
requires explicit original-ELF and NSS SHA-256 pins, an exact symbol name and the
unrebased ELF virtual address (for example `0x19b41f0` for `Curl_ack_eintr`, not
`0x71019b41f0`). Obtain pins from independently verified inputs; supplying a hash
does not establish the provenance of the metadata.

```
python3 tools/original_zerofill_evidence.py --help
python3 -m unittest discover -s tools -p test_original_zerofill_evidence.py
```

The output corroborates equal file-backed load segments and the selected symbol's
size/range inside writable zero-fill storage. It rejects partial section overlaps,
TLS intersections, conflicting symbol extents and unsupported observed relocation
forms. Coverage fields explicitly leave dynamic loader writes, st_other upper
bits, per-object alignment and source ownership unknown. It does not change the
data ledger, recovery mapping gate, or any inventory quality. No acceptance gate
consumes this output.

Direct original-input investigation found these definitions:

| Symbol | Unrebased address | NSS size | Defining DWARF compilation unit |
| --- | --- | --- | --- |
| Curl_ack_eintr | 0x19b41f0 | 4 | Externals/curl/lib/select.c |
| bdPlatformStreamSocket::m_totalBytesSent | 0x1a357d0 | 8 | Contrib/Contrib/DemonWare/bdPlatform/bdPlatformSocket/bdPlatformStreamSocket.cpp |
| bdPlatformStreamSocket::m_totalBytesRecvd | 0x1a357d8 | 8 | Contrib/Contrib/DemonWare/bdPlatform/bdPlatformSocket/bdPlatformStreamSocket.cpp |

The owner observations come from separate DWARF inspection, not the new CLI.
The stream definition DIEs are 0x07e5d135 and 0x07e5d3f9 in CU 0x07e5d116;
the switch consumer CU is 0x07e59f38. Curl's definition DIE is 0x07c31188 in
CU 0x07c31169. This corrects the earlier claim that the counters were private
definitions of the switch-specific file. The known-input NSS pin is recorded in
config/original_inputs.json.

Before any recovery admission, independently authenticate defining owner, object
alignment, complete original/linked loader-write behavior, source initialization,
consumer targets and map provenance. NOBITS contains no stored bytes: an empty
payload hash is never evidence of correct runtime initialization.
