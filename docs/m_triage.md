# M-row behavioural triage

This page answers one question about uncertified (`M`) code: which of it could
behave differently from the original? The row data is in
[`docs/m_triage.csv`](m_triage.csv). The CSV is a snapshot
and nothing reads it. It does not change any quality label or give any credit.

## Inputs

- The inventory at `25f2eaa`: 828 `M` rows in `functions.csv`, with owners from
  `function_files.csv`.
- Checker statuses from the post-landing receipts of transaction
  `c15ac087188f467a97fd91fcf14b9496`, whose `landed.json` names commit
  `25f2eaa`.
- All 359 `// NON_MATCHING:` markers under `src/`. Each marker was bound to the
  definition that follows it, matched by qualified name and parameter count.
  Some markers sit on header or template code; 7 of these were bound by hand.
  18 notes about headers or whole files have no inventory row and are listed
  with an empty `Address`.
- `tools/try_file.py --diff` for six functions where neither the marker nor
  the checker reason settled the question.

## Classes

| Class | Meaning |
|---|---|
| `checker-ok` | The checker reports `ok` in both normal and Lua mode. These are tier-1 promotion candidates; nothing was promoted. |
| `equivalent-encoding` | The difference is a compiler quirk with the same semantics: parameter spills at O0, scheduling, register allocation, block layout, inlining, GlobalOpt/SRA, C1/C2 constructor variants, or the TLS access model. This class also covers Lua sources that match exactly under the pinned Lua backend. |
| `real-difference` | The difference could change behaviour: a different callee, a signed compare where the original is unsigned, a missing initialiser, or a body that is not implemented. |
| `unknown` | The difference is not shown to preserve semantics. Examples: control-flow or CFG changes, large reconstructions, a different blz header version, data references the checker cannot verify, and limits of the checker's jump-table support. |

The `Evidence` column is short. It starts with the definition location
(`def L<n>` in the owner file, `def=<path>:<n>` elsewhere, or
`def=implicit/template`). After that it gives the checker verdict and each
bound marker as `L<marker line> <class>:<reason>`.

## Counts (M rows)

| Class | Functions | Bytes |
|---|---:|---:|
| checker-ok in both modes | 193 | 21,928 |
| equivalent-encoding | 542 | 216,128 |
| — of which exact under the pinned Lua backend | 373 | 80,788 |
| real-difference | 18 | 27,520 |
| unknown | 75 | 106,980 |
| **total** | **828** | **372,556** |

The 75 unknowns split as follows:

- 40 have a marker that describes a control-flow or reconstruction difference.
- 25 have no marker. For these the checker verdict is the only evidence: a
  wrong size or mnemonic, an unverified GOT reference, or a Lua jump table the
  checker cannot prove.
- 10 come from the blz header-version difference in BlizzardProtoBuf.

Marker classes over all 359 markers:

| Marker class | Count | Notes |
|---|---:|---|
| equivalent-encoding | 288 | 1 sits on a row that is now checker-ok, so it is stale. |
| unknown | 52 | 6 are stale and 4 have no inventory row. |
| real-difference | 19 | 2 are stale: the `XGrowableMap_Sub` constructor guard. The constructors in ConsoleIO and UIStoryboard are now `O` and checker-ok. |

Markers on rows that are not `M` add 35 more CSV rows: `O` and `W` rows, plus
notes with no row. One `O` row is a real difference:
`_GLOBAL__sub_I_AdventureDefinition.cpp`, which omits header-global
initialisers. The `ldrsb` marker in `descriptor.pb.cc` is also a real
signedness difference, but it is bound to `O` rows that are checker-ok.

## Top real-difference files (M, by bytes)

| # | File | Bytes | Difference |
|---:|---|---:|---|
| 1 | X/Common/Definitions/WorldDefinition.cpp | 7,248 | `_GLOBAL__sub_I_` omits the GfxVB.h RGBAColor normals. |
| 2 | X/Client/Font/FormattedText.cpp | 5,260 | `sParseText` loads the `{` tag char with `ldrsb`; the original uses `ldrb` (char signedness). |
| 3 | X/Common/Definitions/AttachmentDefinition.cpp | 4,092 | The TU initialiser omits header globals. |
| 4 | X/Common/Definitions/SceneDefinition.cpp | 3,252 | The TU initialiser omits header globals. |
| 5 | X/Common/Definitions/ActorDefinition.cpp | 2,512 | The TU initialiser omits header globals. |
| 6 | X/Common/Definitions/SkillKitDefinition.cpp | 1,292 | The TU initialiser omits header globals. |
| 7 | X/Client/Definitions/SoundBankDefinition.cpp | 672 | The TU initialiser omits header globals. |
| 8 | X/Common/Definitions/FlagSetDefinition.cpp | 604 | The TU initialiser omits cgplaceNull and the RGBAColor normals. |
| 9 | X/Common/HotbarButton.cpp | 480 | The TU initialiser omits header globals. |
| 10 | X/Client/UserInterface/Console/UIConsoleGameSettings.cpp | 464 | The `GameSettingsGlobals` construction is not implemented. |

The remaining real differences are small:

- `WorldPlace.cpp`, `Definitions.cpp`, `SNOName.cpp`, `Target.cpp` and
  `ThreadGlobals.cpp` have the same initialiser gap.
- `UIControlGroup::ForwardMessage` (248 B): try_file confirms the original
  tests `msg <= 2` unsigned, while ours tests `msg < 3` signed.
- `UIConsoleListBox::~UIConsoleListBox` (200 B) calls
  `UI_DeleteControl(UIID const&, int)`. The original calls the non-const
  `UIID&` overload.
- `SigmaMemoryMove` (4 B): the source's `MoveMemory` becomes `memmove`. The
  original calls `Blizzard::String::MemCopy`.

The largest unknown files are:

| File | Functions | Bytes | Why unknown |
|---|---:|---:|---|
| blz/src/format.cpp | 7 | 36,928 | `core_printf` is reconstructed and about 60% of its instructions differ. |
| protobuf descriptor.cc | 11 | 23,236 | The blz header version differs. |
| CGame.cpp | 1 | 5,904 | `sRenderDrawDefault` reloads the inverse matrix through a selected pointer. |
| DemonWare bdNAT/bdQoS/bdReliableSendWindow | 7 | 9,092 | The control flow differs. |
| lua lcode.c and lstrlib.c | 12 | 5,852 | The checker cannot prove the jump tables. |

## Recommended fix order

1. **Promote the 193 checker-ok rows** through the separate promotion path.
2. **Header-global initialisers (13 M TUs plus 1 O, about 25 KB).** One shared
   fix covers all of them: adopt `X/Client/Gfx/GfxVB.h:351-354` (the
   RGBAColor normals and the `RGBAColor(r,g,b,a)` constructor), `cgplaceNull`
   (`WorldPlace.h:23`) and `SNONAME_NONE`. Then recheck every `_GLOBAL__sub_I_`
   consumer.
3. **Signedness fixes.** These are cheap, local and genuinely behavioural:
   - `UIControlGroup::ForwardMessage`: make the `msg` type unsigned.
   - The `sParseText` tag char, and the BlizzardProtoBuf `format[i+1]` load
     (calibrate `-funsigned-char`).
4. **Callee identity fixes:**
   - Call the `UI_DeleteControl(UIID&, int)` overload.
   - `SigmaMemoryMove` should call `Blizzard::String::MemCopy`.
   - Implement the `GameSettingsGlobals` initialiser.
5. **Reduce the unknowns, largest first:**
   - Review `format.cpp` `core_printf` semantically against the binary.
   - Align the protobuf tree with its blz headers.
   - Check the DemonWare control-flow markers with try_file and line_evidence.
   - Separate the Lua jump-table checker limits from real codegen differences.
6. **Leave the equivalent-encoding rows** until certification work is done.
   They need compiler or checker support, not source changes.

## Unverified

- Classes come mostly from the authors' own marker text and the checker's
  first mismatch. The checker stops at the first mismatch, so a later
  difference in the same function can be hidden.
- Only six functions were diffed with try_file:
  - `ListenerSetMicrophoneLocation`: instructions are identical, but the GOT
    data identity is unproven, so it stays `unknown`.
  - The three TLS functions in `Definitions.cpp`.
  - `UIControlGroup::ForwardMessage`.
  - `SigmaMemoryMove`, through the checker's call reason.
- Lua `equivalent-encoding` rows rely on an exact match under the pinned
  patched backend. Normal mode still mismatches them.
- C1/C2 and D1/D2 callee swaps are treated as equivalent. This assumes no
  virtual bases; layouts were not rechecked.
- Marker binding and definition lines come from a text heuristic. 12 M rows
  are implicit or template members whose definitions were not located.
- The regeneration scripts are not committed.
