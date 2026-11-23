# SRC-4be2d8042a96: ltm.c reconstruction

Baseline: `26116f68d046cc2b285adda013671f463fafd080`. Original owner:
`Contrib/Contrib/lua/src/ltm.c`. No claim existed; claimed explicitly.
Scope is one named batch, **tag-method initialization and lookup**:
`luaT_init`, `luaT_gettm`, `luaT_gettmbyobj`. No existing owner implementation or O
function was replaced. Baseline owner score: 0/3 O, 0/612 exact original bytes.

## Source review

Bodies recovered from the sibling's Lua 5.1.1 source and compared against original
DWARF parameter/local names and line evidence. No inlined functions are recorded.
Initialization interns the 17 original event names and sets marked bit 5; the
lookup caches missing methods in Table.flags; object lookup selects table,
userdata, or global metatables and returns the shared nil object when absent.
Both static pointer tables agree with original relocated strings.

Callees are `strlen@plt` (original 0x7100c1d300), `luaS_newlstr`
(0x7100a770f0, lstring.c), and `luaH_getstr` (0x7100a7bad0, ltable.c).
`luaO_nilobject_` is the lobject.c-owned 16-byte zero object at 0x7100e805a0.
Declarations preserve external C symbols. This job does not reconstruct those
other owners. Their unresolved bodies do not prevent compiling this owner.

Full original DWARF layouts were adopted for lobject.h and lstate.h, with
C-compatible forward declarations and size checks. Smaller llimits.h, lzio.h,
ltm.h, lstring.h, ltable.h and lgc.h subsets provide only required declarations
and macros. Existing public lua.h is a C++ subset, so minimal public typedefs
are declared in llimits.h and the two public type constants locally in ltm.c;
no existing Lua header or unrelated helper was modified. Upstream license is
retained in ltm.c.

## Local experiments and remaining nonmatches

The current pinned compiler is clang 5.0.1; the original Lua CU records a Nintendo
5.0.0 snapshot. Original code is unoptimized. Using the existing global optimized
flags would therefore be inappropriate. An owner-only CMake setting now selects
`-O0 -mllvm -fast-isel=false`. This is a best partial configuration, not proof of
the original complete flag set.

| Trial | init bytes | gettm bytes | gettmbyobj bytes | exact |
|---|---:|---:|---:|---:|
| Original | 200 | 156 | 256 | — |
| Standard -O0 | 228 | 120 | 248 | 0/3 |
| -O0, fast instruction selection disabled | 192 | 132 | 232 | 0/3 |
| Also disable O0 optnone | 192 | 132 | 232 | 0/3 |
| Also disable machine dead-code elimination | 192 | 132 | 232 | 0/3 |
| Also disable LLVM IR passes | 192 | 132 | 232 | 0/3 |

The last three switches were tested individually and discarded. All functions
remain explicitly NON_MATCHING. The original retains one additional parameter
copy/spill in init and three in each lookup, with corresponding stack/frame and
register differences. Object lookup also currently uses a GOT load for external
`luaO_nilobject_`, whereas the original uses a direct address. No register pinning,
assembly, fabricated locals, or padding was introduced to force these differences.

Docker's address pools were exhausted; local checks reuse the existing
`d3decomp` Compose network, without starting any automation.

## Concrete prerequisite jobs

- **SRC-4be2d8042a96-P01 — internal C declarations** (resolved for this owner).
  Paths: `src/Contrib/Contrib/lua/src/{llimits,lobject,lstate,lzio,ltm,lstring,ltable,lgc}.h`.
  Acceptance: this owner compiles against original DWARF layouts, size assertions
  pass, and macros reproduce the recorded accesses. Local compilation satisfies
  this bounded declaration prerequisite; it does not establish full Lua recovery.
- **SRC-4be2d8042a96-P02 — recover Lua compiler configuration** (open).
  Paths: `CMakeLists.txt`, `toolchain/`, `src/Contrib/Contrib/lua/src/ltm.c`.
  Acceptance: establish a reproducible compiler/flag configuration producing all
  three original functions without source tricks, then pass the serial full
  relocation-aware gate with no prior O regression. Parameter copies/spills are
  concrete remaining evidence; a compiler-version cause is a hypothesis.
- **SRC-4be2d8042a96-P03 — recover nil-object definition/linkage** (open).
  Paths: `src/Contrib/Contrib/lua/src/lobject.c` and `lobject.h`.
  Acceptance: define the original 16-byte zero `luaO_nilobject_` in its original
  owner, establish its internal linkage/visibility contract from original evidence,
  and verify the direct-address reference in luaT_gettmbyobj with the full checker.
  That owner is outside this job; no substitute definition was added here.

## Acceptance

The source job remains open; no curated milestone status is changed. Local exact
score remains 0/3 and 0/612 bytes. The full gate result is recorded below for
`tools/land.py Contrib/Contrib/lua/src/ltm.c`. Generated status must follow committed
inventory, not the claim's handoff state.


### Serial gate result

Landed as `dd5233877e5565cf3941352dc3eb033479beb57d`. `tools/land.py` returned
success after the full build and full relocation-aware checker; zero prior O
regressions and zero new O functions. All three owner rows changed U -> M.
Owner exact counts remain **0/3 -> 0/3**, exact original bytes **0/612 -> 0/612**.
Global exact counts remain **13,662 -> 13,662**, exact original bytes
**3,188,172 -> 3,188,172**. The full build's generated compile command confirms
`-O0 -mllvm -fast-isel=false` for this owner only.

The gate validates regression safety; it does not establish relocation correctness
for functions already failing on instruction size. All three remain open under
P02/P03 above. The existing unrelated tracked and untracked working changes were
temporarily stashed for the clean-checkout gate, restored successfully, and their
file contents verified by SHA-256. The temporary stash was then removed.

Changed files: ltm.c, the eight required internal headers listed above,
CMakeLists.txt (owner-only flags), generated functions.csv (three U -> M rows),
and this evidence document plus its raw-evidence directory. Curated jobs were
not changed; the generated source job remains incomplete.
