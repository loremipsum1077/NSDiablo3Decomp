# SRC-6fa67182d24a: memory manager reconstruction

Baseline: `7f6608dc532d3bb35f88b68ee8305ae098609986`. Owner:
`Contrib/Contrib/lua/src/lmem.c`. No prior claim or source implementation existed.
The explicit take recorded our claim before a sandbox failure creating the worktree;
we reconciled that same claim and completed its worktree, without replacing another worker.
One bounded batch: allocation growth, overflow error, and allocator accounting.
Before: 0/3 O, 0/640 exact original bytes. All three were U.

## Source review

Recovered the sibling repository's Lua 5.1.1 source, preserving upstream bodies,
DWARF parameter/local names and original function order. Retained the Lua license.
No inlined functions occur in these three DWARF records. Existing lstate.h,
lobject.h and llimits.h provide the original layouts with compile-time size checks.
No existing shared helper or exact implementation was changed.

Growth doubles capacity with a minimum of four, saturates at the limit, and raises
luaG_runerror when already at the limit. luaM_reallocv checks `(size_t)(n+1)`
against `(SIZE_MAX-2)/element_size`, matching the original unsigned division and
comparison; it only updates size after allocation. The new lmem.h contains this
upstream macro and the three owner declarations. Minimal external declarations
for ldebug.h and ldo.h remain local because those headers are not adopted.

Reallocation calls G(L)->frealloc at offset 0x10 with ud at 0x18, throws error 4
on nonzero-size allocation failure, and updates totalbytes at 0x78 by subtracting
the old size then adding the new. luaM_toobig reports the exact original string
at 0x7100e2a43b and returns NULL.

Reviewed direct callees against original assembly and candidate relocations:
luaG_runerror (0x7100a65da0, ldebug.c), luaD_throw (0x7100a66b70, ldo.c),
and the two internal lmem calls. Declarations preserve external C linkage and
returning control flow present in the original. No replacement callee bodies
are required to compile this owner. The full checker cannot establish exactness
for size-mismatching functions merely because their relocation names are right.

## Local validation and open functions

Original unoptimized instructions justify owner-scoped CMake options
`-O0 -mllvm -fast-isel=false`, consistent with the prior ltm.c investigation.
No global flags were changed. Local trials use the existing d3decomp Compose
network because Docker's address pools are exhausted; no automation was started.

| Function | Original bytes | O0 default fast selection | Retained selection disabled |
|---|---:|---:|---:|
| luaM_growaux_ | 372 | 288 | 324 |
| luaM_realloc_ | 208 | 180 | 176 |
| luaM_toobig | 60 | 48 | 48 |

Both trials: 0/3 exact. Retained configuration has the original instruction
selection structure but omits six parameter copies/spills in growaux_ and four
in realloc_, with corresponding frame differences. toobig omits one copy/spill
and addresses the string directly through x1 instead of x8 followed by a move.
All three bodies are explicitly NON_MATCHING. No fabricated locals, register
pinning, assembly, or padding were used. Further arbitrary source rewrites are
not justified by this compiler-shaped evidence. Previous ltm trials of optnone,
machine DCE and IR pass settings were also unsuccessful (see ltm_evidence.md).

## Concrete prerequisite discoveries

- **SRC-6fa67182d24a-P01: allocation declarations and macro** — resolved locally.
  Paths: `src/Contrib/Contrib/lua/src/lmem.h`, `lmem.c`, existing `lstate.h`.
  Acceptance: all three functions compile using original layouts and the original
  overflow guard/callee signatures; reviewed original instructions and emitted
  relocations support those accesses. This does not imply byte exactness.
- **SRC-6fa67182d24a-P02: reproduce original Lua code generation** — open.
  Paths: `CMakeLists.txt`, `toolchain/`, `src/Contrib/Contrib/lua/src/lmem.c`.
  Acceptance: reproduce all three original instruction streams including parameter
  copies/spills without source tricks, then pass the serial relocation-aware gate
  with no prior O regression. Related existing investigation:
  SRC-4be2d8042a96-P02. Compiler-version causation remains a hypothesis.

## Acceptance and landing

Local counts remain 0/3 O and 0/640 exact bytes; source job remains open.
No curated job or milestone status is changed. Generated status must follow the
committed full-check inventory. Serial gate outcome is recorded below.


### Serial gate result

Landed as `882b777cd6b294d3a16b0b273f80a7024825be0c` using
`COMPOSE_PROJECT_NAME=d3decomp python3 tools/land.py Contrib/Contrib/lua/src/lmem.c`.
Full build and full relocation-aware checker succeeded; zero prior O regressions.
All three owner rows changed U -> M, with 0/3 -> 0/3 O and 0/640 -> 0/640
exact original bytes. Global exact count stayed 13,662 and exact original bytes
stayed 3,188,172. No nonmatch is accepted as exact; SRC-6fa67182d24a stays open.

The gate had no active coordinator/other lander. Unrelated tracked and untracked
work was temporarily stashed, restored, and all 36 preexisting file contents
verified by SHA-256 before the temporary stash was dropped.

Changed files: `src/Contrib/Contrib/lua/src/lmem.c`, its new `lmem.h`, owner-only
flags in `CMakeLists.txt`, generated `data/2.6.2/functions.csv`, and this evidence
document/directory. Curated milestone status and unrelated shared helpers were
not changed. The claim was removed by successful landing of the partial result.
