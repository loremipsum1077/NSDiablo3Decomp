# SRC-b0d1517a03a2: buffered input stream reconstruction

Baseline and claim base: `5f98acf9ab8b6d3e70ab261de7c6f690c722c26f`.
Owner: `Contrib/Contrib/lua/src/lzio.c`. No preexisting claim or implementation.
The initial take recorded our claim but sandbox restrictions prevented worktree
creation; repaired only our own `codex-lzio` claim with claim.make_worktree.
One bounded batch: stream refill, lookahead, initialization, read and buffer growth.
Before: 0/5 O, 0/980 exact original bytes; all five U.

## Source review

Recovered the sibling repository's Lua 5.1.1 source, checked against every original
DWARF record and instruction listing. Preserved parameter/local names, function
order and upstream bodies, retaining the Lua license. No inline functions appear
in the DWARF records. Lock/unlock emit no instructions. Existing Mbuffer layout
is unchanged. Added Zio's five fields at offsets 0, 8, 16, 24 and 32 to its owner
header with a size check. Minimal lua_Reader declaration is documented as lua.h
owned because that existing public subset requires C++; no public header rewrite.

fill invokes the reader with L, data and &size, returns -1 for NULL/empty input,
then consumes one unsigned byte. lookahead puts that byte back. init stores all
five fields. read copies min(n,z->n), advances the cursor and returns missing
bytes on EOF. openspace grows to at least 32 and preserves the existing
luaM_reallocv overflow guard `(size_t)(n+1) <= SIZE_MAX-2` for char elements;
original adds 3 and branches on carry. Size updates follow allocation.

Original direct calls and emitted relocations agree: luaZ_fill, luaZ_lookahead,
luaM_realloc_ at 0x7100a6ed40 and luaM_toobig at 0x7100a6ee10. Original read's
otherwise unnamed 0x7100c1d410 target is memcpy@plt (objdump evidence).
The reader is an indirect call through offset 16. No data relocations are needed.
Allocation callees already exist; their own nonexactness does not prevent this
owner compiling, and no unrelated helper implementation was changed.

## Local validation

Original assembly shows unoptimized code. Added only owner-scoped CMake options
`-O0 -mllvm -fast-isel=false`, following existing Lua owners.
Two instruction selector trials through tools/try_file.py:

| Function | Original bytes | Fast selector | Retained selector disabled |
|---|---:|---:|---:|
| luaZ_fill | 188 | 168 | 164 |
| luaZ_lookahead | 152 | 124 | 144 |
| luaZ_init | 116 | 84 | 84 |
| luaZ_read | 292 | 228 | 264 |
| luaZ_openspace | 232 | 180 | 208 |

Both trials 0/5 exact. Retained selection better follows the original instruction
structure. init lacks four parameter copies and four spills (32 bytes); lookahead
lacks one copy/spill pair (8 bytes); openspace lacks three pairs (24 bytes).
read lacks three pairs plus a spill of memcpy's unused return (28 bytes).
fill additionally differs in reader argument movement/load selection (24 bytes).
Frame offsets change accordingly. All five remain explicitly NON_MATCHING.
No padding, register pinning, fabricated locals or assembly was used. Further
source rewrites are not justified by these compiler-shaped differences; related
ltm/lmem evidence already records unsuccessful compiler-option hypotheses.

Docker trials used COMPOSE_PROJECT_NAME=d3decomp; no automated runs were started.

## Concrete prerequisites

- **SRC-b0d1517a03a2-P01 — stream layout and C callback declarations**, resolved.
  Path: `src/Contrib/Contrib/lua/src/lzio.h` (callback owned by `lua.h`).
  Acceptance: original Zio layout and reader signature compile in the original C
  owner; offsets, unsigned byte conversion and growth calls agree with original
  DWARF/assembly. Local compilation and source review satisfy this, not exactness.
- **SRC-b0d1517a03a2-P02 — reproduce original Lua code generation**, open.
  Paths: `CMakeLists.txt`, `toolchain/`, `src/Contrib/Contrib/lua/src/lzio.c`.
  Acceptance: all five original instruction streams including parameter copies
  and spills match without source tricks, then pass the full relocation-aware
  gate without prior O regression. Related: SRC-4be2d8042a96-P02 and
  SRC-6fa67182d24a-P02. Compiler-version causation remains a hypothesis.

## Acceptance

Source job stays open: local O counts 0/5 -> 0/5, exact bytes 0/980 -> 0/980.
No curated milestone status changed. Generated inventory follows the landing
checker. Serial gate result will be recorded after execution.

### Serial gate result

Landed as `0de8af7de01be0b274f4561f424ff4bce0afb5f7` with
`COMPOSE_PROJECT_NAME=d3decomp python3 tools/land.py Contrib/Contrib/lua/src/lzio.c`.
The full build and `tools/check --warnings-as-errors` succeeded. All five owner
rows changed U -> M, with 0/5 -> 0/5 O and 0/980 -> 0/980 exact original bytes.
Global O count remained 13,662 and exact bytes remained 3,188,172; independently
compared committed inventories confirm zero prior O regressions. No other lander
or coordinator was active; only the independent autopilot server was observed
and left alone. The successful gate removed this job's claim and worktree.

Changed files: `src/Contrib/Contrib/lua/src/lzio.c`, additive stream declarations
and macros in `lzio.h`, owner-only options in `CMakeLists.txt`, generated
`data/2.6.2/functions.csv`, and this evidence document/directory. No curated
status change: every listed function remains open under SRC-b0d1517a03a2-P02.
