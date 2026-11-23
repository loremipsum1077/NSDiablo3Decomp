# Gameplay experience recovery

On 2026-11-02, the source-first follow-up reconstructed all five original
functions in `X/Common/ExperienceCommon.cpp`. This is Diablo III gameplay code,
separate from the earlier Lua lexer/compiler pipeline work.

**Landed as recovered source:** commit
`d052c6e7a83284b8fcb1b03d05892b67eba0c536` adds the five functions and their
authentic declarations to the main branch. Exactly five inventory rows changed
from U to M. There is no O promotion or full exact-certification claim.

The initial isolated candidate was `b132b7128b71b60165030a0e2953363c1575ac70`, based on
`acd979acca324f38e25c9d55b990212af4c2caa5`, on
`work/X_Common_ExperienceCommon`. Before integration it was rebased onto the
documentation-only `17acc01` baseline as `8cb1b46`, with identical source bytes.
The original candidate is preserved on `codex/experience-source-v1`.

| Reconstructed function | Original bytes | Linked checker, both modes |
| --- | ---: | --- |
| `ExperienceGetCurrentLevelXP` | 244 | Missing linked `gptGroupDefGameBalance` provider |
| `ExperiencePlayersCloseEnough` | 288 | OK |
| `ExperienceGetRestXPToGrant` | 92 | OK |
| `ExperienceGetBonusFromGear` | 344 | Missing linked `gptGroupDefGameBalance` provider |
| `ExperienceGetHirelingBonus` | 252 | OK |

## Source and debug evidence

One subagent reconstructed source, a second recovered the missing declarations,
and a third independently reviewed the original behavior and layouts. Evidence
comes from the original NSS/DWARF and main ELF, not a pasted decompiler body.
The five functions use existing actor lookup, attribute, definition-lock and
distance helpers. No helper stubs, guessed byte offsets, assembly or compiler
flag changes were added.

Two existing headers needed genuine declarations:

- `ActorCommonData.h`: the three original experience accessor declarations,
  each a `const` method returning `uint64`.
- `Definitions/GameBalanceDefinition.h`: six original dependent/experience
  structures, replacing the relevant unadopted storage with the original
  experience tables at offsets `0x38` and `0x48`. Size/offset assertions preserve
  the observed layout, including the `0x238` outer structure size.

The header type additions required consumer validation. In particular,
`XPtr64<T>` has nontrivial special members: replacing opaque padding with its
original typed members is not automatically proof that every existing consumer
emits unchanged runtime code.

## Initial candidate validation

Two single-file offline compiles used the existing guarded `try_file.py` path.
The first emitted all five functions, with four instruction-shape matches. The
remaining comparison operand order was corrected using the original assembly.
The second compile pinned all three changed files before and after execution:
all five functions / 1,220 bytes matched after relocation masking, with no
missing or unexpected definitions. It took **1.42 seconds**. Its object
SHA-256 is `aec0e93a96e6146a2beb9a6548c2302fa579c2922d7d947b193f77190e9e106b`.

The existing strict checker authenticated complete owner membership and resolved
all **32 direct-call relocations**. The rested-XP function passes that object
check. The other four remain unsupported because of **eight GOT address pairs**
(`R_AARCH64_ADR_GOT_PAGE` / `R_AARCH64_LD64_GOT_LO12_NC`); no policy was weakened
to turn those into passes. No linked-provider, full-data or unwind certification
is implied by these results.

Independent replay confirmed those results and found exactly five emitted
functions, no defined OBJECT/COMMON/TLS symbols, and only a 200-byte unwind
section outside executable allocated content. Original/object FDE ranges and
NOP-stripped programs agree for all five functions; raw FDE bytes agree for
four, while CIE/FDE padding differences remain. This unwind observation is
diagnostic, not linked-unwind acceptance.

Attempt 1 is exploratory:
the author corrected source around its launch, so it is not a frozen-source
certification. Attempt 2 has verified unchanged inputs and the candidate commit
contains those exact source/header bytes.

Two existing header consumers were also compiled at baseline and candidate,
producing fresh compiler dependency files. `UIDyeItemCommon.cpp` has identical
runtime digests under the existing comparator (its debug information changes);
`GameBalanceDefinition.cpp` has identical complete object bytes. These four
successful compiles took 4.405 seconds in total. An earlier diagnostic
attempt compiled the first baseline but saved its depfile to the wrong path;
the diagnostic was corrected, and the four comparisons were rerun. No production tool was changed.

This is only a two-consumer smoke check. The estimate was a union of
109 compile-database consumers plus ten other tracked/candidate translation
units. The database covers 678 of 730 tracked source units and predates the new
owner; textual includes and those two fresh depfiles do not prove full coverage.

The candidate worktree plus initial source evidence used about **59 MiB** at
inspection, before the additional consumer objects and full receipts. The guarded compiles completed
without a retained pending Docker journal.
No additional external provider worker calls were made.

## Complete review and source integration

The follow-up completed the existing
recovery-only route without changing production validators or build recipes:

- A private, independently replayed diagnostic authenticates the three original
  globals, their GOT slots and loader RELATIVE relocations. Composed with the
  32 named-call witnesses, all 1,220 original function bytes correspond exactly.
  It includes 16 positive and 71 negative predicate checks. This proves
  original/object correspondence, not a missing candidate global provider.
- Four official baseline/candidate normal/Lua builds and the recovery audit
  completed in **396.994 seconds**. Both modes preserve all **731 existing
  runtime objects**: 632 are whole-file identical and 99 differ only in debug
  information. The candidate adds exactly one owner object.
- Independent replay reproduced the complete audit. All 11,063 data-ledger
  identity/status/reason outcomes are unchanged. Linked address movement remains
  authenticated; existing exactness debt was retained. All five functions pass
  the existing object/both-link unwind checks, which do not imply whole-image
  byte identity.
- The independent review completed all **17 obligations**. The serialized
  integration then ran four more fresh builds, reproduced every reviewed
  object, linked ELF, map, provenance and checker result, and fast-forwarded the
  main branch. Integration took **481.698 seconds** including its verification.
  A separate postlanding review confirmed the exact source and five U-to-M changes.

Review SHA-256:
`e1f1b8bfb5254b527ebe1bc747c0511d6b5802d81c8b91bca0c90e8195dba8a5`.
The final transaction's `landed.json` recorded `recovery_landed`, with
`strict_certified: false`.
The pre-landing receipts used about 2.2 GiB.

## Remaining exactness and next gameplay owner

Two linked XP functions still need the genuine `gptGroupDefGameBalance` provider.
The original debug records assign it to `GameBalanceDefinition.cpp`; the original
also shows zero-fill and a Game.cpp registration/creation path. Its owning file
has 73 functions / 76,064 bytes, including a 63,644-byte initializer; the current
whole-owner recovery gate caps selection at 64 and does not admit this named
NOBITS definition. Recovering its source and lifecycle is real remaining work;
adding a bare definition to make a diagnostic disappear is not certification.
Several other called functions remain unrecovered as well.

During the final rebuilds, the next source owner was reconstructed:
[RadiusTrigger.cpp](radius_recovery.md), three functions / 1,028 bytes. Its
instruction shapes now match, but a measured literal-data gate blocker prevents
receipt admission. The candidate has no inventory credit.

This landing is a concrete test case for the eventual AI recovery loop:
select an owner, read original evidence, restore missing types, reconstruct
complete bodies, compile cheaply, correct a measured difference, review and
integrate the source. Further pipeline work should remove a demonstrated blocker
on this source path or another selected game owner, rather than only replaying
the fixed lexer example.
