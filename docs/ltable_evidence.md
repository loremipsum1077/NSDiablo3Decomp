# Lua table recovery (SRC-8bdff7ce8c56)

Recovered all 26 functions owned by Contrib/Contrib/lua/src/ltable.c: numeric,
string and general lookup/insertion; Brent collision relocation; array/hash
rehashing and resizing; traversal; length search; allocation and release.
This is source recovery with **0/26 exact instruction matches** (7,888 original
bytes), not a completed exact-match source job. No bodies remain missing.

The Lua 5.1.1 source in sibling Diablo3Decomp/src/Externals/lua/5.1.1/ltable.c
provided the source skeleton. Every function's original DWARF parameter/local
names and line ownership were read (docs/evidence/ltable/dwarf.txt), then
compared against the original Ghidra control flow. The important local change
from upstream is external unsigned-int `twoto` in lobject.c:26, rather than
upstream's signed shift macro. It controls node counts and signed comparisons.
`dummynode_` is the original 40-byte zero object at 0x7100e809a8.

Using -O0 -mllvm -fast-isel=false and the repository's clang 5.0.1, all functions
compile. Original argument-copy spills and corresponding frame sizes differ;
getstr.diff and resize.diff preserve representative full differences. No inline
assembly, register pinning, artificial source spills or altered optimization
attributes were added. The nil-object address linkage can also differ while
its defining owner is recovered. Relocations are not certified by try_file.

| Function | Compiled bytes | Original bytes |
|---|---:|---:|
| luaH_next | 560 | 584 |
| findindex | 396 | 416 |
| luaH_resizearray | 128 | 152 |
| resize | 688 | 732 |
| luaH_new | 192 | 220 |
| setarrayvector | 228 | 252 |
| setnodevector | 424 | 448 |
| luaH_free | 212 | 228 |
| luaH_getnum | 248 | 264 |
| hashnum | 196 | 212 |
| luaH_getstr | 212 | 228 |
| luaH_get | 328 | 348 |
| mainposition | 400 | 416 |
| luaH_set | 236 | 260 |
| newkey | 488 | 512 |
| luaH_setnum | 168 | 192 |
| luaH_setstr | 152 | 176 |
| luaH_getn | 284 | 292 |
| unbound_search | 308 | 332 |
| arrayindex | 128 | 140 |
| getfreepos | 116 | 124 |
| rehash | 272 | 296 |
| numusearray | 332 | 348 |
| numusehash | 208 | 232 |
| countint | 164 | 180 |
| computesizes | 288 | 304 |

## Behavior test

Run from the repository root:

```sh
cc -std=c11 -fsanitize=address,undefined -fno-sanitize=float-cast-overflow -g -Isrc/Contrib/Contrib/lua/src tests/lua_tables/behavior.c src/Contrib/Contrib/lua/src/ltable.c -o /tmp/d3-ltable-behavior
/tmp/d3-ltable-behavior
```

Passed numeric dense/sparse insertion and automatic rehash, explicit array
shrink with adequate hash capacity, 64 distinct strings sharing one hash,
GC barrier invocation, fractional/pointer/boolean keys, full traversal,
deleted-key continuation, length after deletion, nil/NaN insertion and invalid
iteration errors, and zero live allocation bytes after release. The harness
uses narrow allocation/GC/error/value-equality/logarithm stubs, so this is not
full interpreter integration. ASan and UBSan pass with float-cast-overflow
excluded: the original Lua conversion attempts the number-to-int conversion
before detecting NaN; the Switch AArch64 conversion has target behavior that
portable C's floating-conversion sanitizer flags. The original source remains
unchanged. Likewise internal luaH_resizearray requires sufficient hash capacity
for displaced nonnil values; arbitrary shrink of a full hash table is outside
its caller contract and the original upstream implementation can invalidate a
source pointer through recursive rehash. The test explicitly provides capacity.

Remaining work is original compiler/code-generation recovery and the full
relocation-aware integration gate. Claim done denotes useful handoff, not exact
completion. Shared headers are supplied by the parallel Lua API/object owners.
