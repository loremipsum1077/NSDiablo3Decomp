# Lua runtime source recovery

This P10 middleware batch recovers five previously absent original source owners
from baseline `0367d145`. Three subagents implemented the API, tables, and
string/closure owners; the coordinator recovered object helpers, consolidated
headers, and performs the serial integration gate. Claims were made by original
owner. Interdependent headers are integrated together through the coordinator's
`lobject.c` claim so no intermediate commit requires missing headers.

| Original owner | Functions recovered | Original code bytes | New exact functions |
| --- | ---: | ---: | ---: |
| lapi.c | 68 | 12,872 | 0 |
| ltable.c | 26 | 7,888 | 0 |
| lstring.c | 4 | 1,628 | 0 |
| lfunc.c | 11 | 2,468 | 0 |
| lobject.c | 11 | 3,516 | 0 |
| Total | 120 | 28,372 | 0 |

These are actual C implementations: stack and value APIs, table lookup/insertion,
collision relocation and resizing, string interning, userdata allocation, closure
and upvalue lifecycle, prototype allocation, number parsing, raw equality, object
formatting, and chunk identifiers. Blizzard's `lua_abortaddress` and out-of-line
unsigned `twoto` are retained. The 16-byte zero nil object and 256-byte logarithm
table were checked against original data. The SDK whitespace predicate is kept
in its DWARF header owner and emitted by the original lobject.c owner.

The source reference is Lua 5.1.1 in the sibling repository, reviewed against the
2.6.2 DWARF names/layouts, Ghidra control flow, and original disassembly. Existing
source layouts are preserved. The recovered headers remain usable from C and C++.
License notices are retained in each recovered upstream source file.

## Exactness boundary

All 120 original bodies are emitted, with zero missing functions and zero local
exact matches. Every body is marked NON_MATCHING. Owner-only compilation uses
`-O0 -mllvm -fast-isel=false`, consistent with the original unoptimized code and
previous Lua recovery. The original Nintendo compiler retains argument copies
and spills absent from clang 5.0.1, with associated instruction/register/frame
differences. This is a known unresolved prerequisite, not evidence for a new
exactness claim. No artificial spills, register pinning, or assembly were added.

[API evidence](lapi_evidence.md) and [table evidence](ltable_evidence.md)
describe the reconstruction. Object helpers remain 8–36 bytes short;
formatting also has register and frame-layout differences. Recovered data
contents do not by themselves prove every caller's data-address linkage.

## Behavior checks

Native tests exercise the recovered code with explicit narrow VM/GC/allocator
stubs, not a running game or complete interpreter:

- `tests/lua/lapi_native.c`: stack operations, indices, conversions, cross-thread
  moves, upvalues, abort pointer, and GC controls.
- `tests/lua_tables/behavior.c`: dense/sparse keys, collision chains, rehash,
  shrink, traversal, length, error paths, barriers, and allocation balance.
- `tests/lua/objects_test.c`: string identity, embedded NULs, resurrection,
  hash growth, userdata, closures, open upvalue ordering/reuse/close, prototypes.
- `tests/lua_runtime/object_test.c`: all 65,535 small integer encodings/logarithms,
  raw equality, number parsing, whitespace, and chunk identifier formatting.

AddressSanitizer and UndefinedBehaviorSanitizer pass. The table harness excludes
float-cast-overflow for original NaN-to-int conversion behavior; the allocation
harness excludes null checks for Lua's offset-zero union conversion.
These exceptions and stub boundaries are documented beside the tests. The object
helper harness uses both sanitizers without exclusions; VM-dependent variadic
formatting is not exercised by that harness.

## Serial landing

The baseline full build and installed `tools/check --warnings-as-errors` pass.
The integrated `tools/land.py` gate passed the full build and installed
`tools/check --warnings-as-errors`, then committed all five owners. It changed
exactly 120 generated inventory rows from U to M (28,372 original bytes), with
no changes outside this batch. Global counts moved from 21,915 U / 976 M to
21,795 U / 1,096 M. All 13,662 O rows and their 3,188,172 original bytes were
preserved; the existing six W rows were unchanged. No new O functions are claimed.

A passing regression gate is not proof of relocation correctness for these
size-mismatching new functions. The strict exact-match source jobs remain open. Exact project totals
must remain distinct from the amount of recovered source; none of these owners
is complete under the project's byte-matching completion contract. Compiler
configuration/version recovery and full interpreter integration remain open.

## Library owners: uncertified source

Commit `2a785f2` adds ten function owners (ldump, lundump, lmathlib, loslib,
ldblib, ltablib, lauxlib, lcode, ldo, lstrlib), lopcodes data and their headers and
shims: 267 previously U identities. It is independently reviewed source ported from
D3Decomp `62a0c1a..d8bc909` without history. The normal-mode checker changed exactly
those 267 rows from U to M. No O row is claimed.

This is recovered source, not a matching certification. The strict full check that
`tools/land.py` and `tools/receipts.py` require fails on the unchanged baseline
(1,525 existing O rows; P19-F04-ROLLOUT), so no formal receipt exists. For this
landing only, the maintainer accepted a same-baseline differential instead. Fresh
normal and Lua builds of `b99073c` and `2a785f2` reproduced byte for byte with
verified link provenance. Under checker patch 0010 no prior status is lost; the only
changes are the Lua-mode gains below. There are no checker errors and all 394
verified data objects remain verified. This is not a precedent for other candidates.

| Checker result | Normal | Lua backend |
| --- | ---: | ---: |
| New identities OK | 0 / 267 | 250 / 267 |
| Previously recovered Lua M rows now OK | 0 | 15 |

Lua-backend results earn no normal-inventory credit. The 17 Lua-mode mismatches
are 13 unproven `-O0` switch shapes (P19-F04-SWITCH): DumpConstants, LoadConstants,
str_format, `match@lstrlib.c`, match_class, luaK_dischargevars, luaK_storevar,
luaK_goiftrue, codenot, luaK_goiffalse, luaK_posfix, discharge2reg and constfolding.
Two are undefined libc `stdin`/`stderr` GOT targets (P19-F01): luaL_loadfile and
db_debug. f_parser and writer keep their NON_MATCHING code differences.

The new headers also change code in the earlier lapi, ltable, ltm and lobject owners
(hidden `LUAI_DATA`, `api_check`); no status is lost. `LuaScriptInterface_wrap.cpp`
changes only debug information and its table/call audits pass. Emitted extras are
`luaP_opnames` and the original-stripped lauxlib helpers luaL_ref, luaL_unref,
luaL_loadstring, luaL_newstate, panic and l_alloc. l_alloc is the upstream 0x80-byte
body, whereas DWARF records 0x10c; the original body is unknown. loslib retains three
original-source-unknown regions (os_execute 39–41, os_tmpname 60–70, os_getenv
75–80). Strict object relocations (192 Lua-mode rows), unwind programs, owner data,
both qualified lstrlib identities and setjmp/libc ABI provenance remain pending.
Upstream trailing whitespace is kept, so `git show --check` flags the source commit.
