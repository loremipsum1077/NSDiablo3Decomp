# SRC-805bf196050a: standard library initialization

Baseline HEAD verified: `4e7bfdfcd8361b19fee4f11bd13493bf5f5b1dd8`.
Original owner: `Contrib/Contrib/lua/src/linit.c`. No prior claim or source
implementation existed. Claimed explicitly with tools/claim.py take and worked
only in the resulting worktree. Scope is the single luaL_openlibs function.
Before: 0/1 O, 0/144 exact original bytes; quality U.

## Source and dependency review

Recovered the sibling Lua 5.1.1 source and retained its license. Original DWARF
records parameter L and local const luaL_Reg *lib, no inlined functions, and
static const luaL_Reg lualibs[9]. The function walks until lib->func is null,
pushes a closure with zero upvalues, pushes the library name, and invokes it
with one argument and zero results. The lua_pushcfunction macro expands to
lua_pushcclosure exactly as original evidence requires.

Original table at 0x71010dbed0 is 144 bytes: name/function pairs for empty string
and luaopen_base, package/luaopen_package, table/luaopen_table, io/luaopen_io,
os/luaopen_os, string/luaopen_string, math/luaopen_math, debug/luaopen_debug,
then two null pointers. All 16 relocated pointers were reviewed; the empty
name address 0x7100d6a4a3 contains a zero byte, not a missing string. Candidate
relocations preserve all eight initializer symbols and the original ordering.
Original direct callees are lua_pushcclosure at 0x7100a60b80, lua_pushstring at
0x7100a60950, and lua_call at 0x7100a61a80, all owned by lapi.c. Their DWARF
signatures and emitted external C relocation names agree.

Existing public lua.h/lauxlib.h have C++-only syntax. Minimal C declarations
from lua.h, lauxlib.h and lualib.h are documented locally, using existing
llimits.h typedefs. luaL_Reg retains the original two-pointer layout and a
compile-time size assertion. No existing shared header/helper was changed.
The other library owner bodies are not required to compile this owner and
were not reconstructed or substituted.

## Local trials and nonmatch

Original code is unoptimized. Owner-scoped CMake flags use
`-O0 -mllvm -fast-isel=false`, without changing any other owner's flags.
Local compilation used the existing d3decomp Compose network (Docker address
pool exhaustion was already established in preceding Lua jobs).

| Configuration | Candidate bytes | Original bytes | Exact |
|---|---:|---:|---:|
| O0, standard fast instruction selection | 128 | 144 | no |
| O0, fast instruction selection disabled (retained) | 136 | 144 | no |

Retained code omits original `mov x1,x0` and `str x1,[sp,#8]`, shifting branch
targets and the zero-argument spill from sp+4 to sp+12. This is consistent with
the existing ltm/lmem parameter-copy issue. Source semantics, table values and
call symbols match the evidence, but byte exactness is not established.
No artificial locals, register pinning, assembly, padding, or arbitrary body
rewrites were introduced. luaL_openlibs is explicitly NON_MATCHING.

## Concrete prerequisite jobs

- **SRC-805bf196050a-P01 — C declarations and library table**: resolved locally.
  Path: `src/Contrib/Contrib/lua/src/linit.c` (declaration owners lua.h,
  lauxlib.h, lualib.h). Acceptance: compile with the recorded registration
  layout and C signatures; verify eight name/function pairs plus the null
  sentinel against original relocated data. Source/table review and compilation
  satisfy this bounded prerequisite, not whole-owner exactness.
- **SRC-805bf196050a-P02 — original Lua parameter-copy code generation**: open.
  Paths: `CMakeLists.txt`, `toolchain/`, `src/Contrib/Contrib/lua/src/linit.c`.
  Acceptance: reproduce the original 144-byte luaL_openlibs without source
  tricks, verify its table/callee relocations through the full serial gate,
  and preserve all prior O functions. Related investigations:
  SRC-4be2d8042a96-P02 and SRC-6fa67182d24a-P02. A compiler-version cause remains
  a hypothesis, not a confirmed diagnosis.

## Acceptance

Local exact count is unchanged at 0/1 O and 0/144 bytes. Source job remains open;
no curated milestone status was changed. Full gate outcome is recorded below; generated source status follows committed inventory.


### Serial gate result

Landed as `5c5b6b896646d1d228b514bb115dd194abada94f` using
`COMPOSE_PROJECT_NAME=d3decomp python3 tools/land.py Contrib/Contrib/lua/src/linit.c`.
Full build and relocation-aware checker passed with zero prior O regressions.
luaL_openlibs changed U -> M; owner score remains 0/1 -> 0/1 O and
0/144 -> 0/144 exact original bytes. Global exact totals stayed 13,662 functions
and 3,188,172 bytes. The size mismatch prevents claiming full relocation-verified
exactness for luaL_openlibs. It remains open under SRC-805bf196050a-P02.

No coordinator or competing lander was active. All 36 unrelated preexisting
working-file contents were restored and verified by SHA-256 after the gate;
the temporary stash was removed. No automation was started.

Changed files: `src/Contrib/Contrib/lua/src/linit.c`, owner-specific flags in
`CMakeLists.txt`, generated `data/2.6.2/functions.csv`, this evidence document,
and `docs/linit_evidence/`. No shared helpers or curated status were changed.
