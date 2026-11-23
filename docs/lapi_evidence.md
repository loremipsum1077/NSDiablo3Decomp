# Lua API reconstruction evidence

Recovered all 68 function owners in `Contrib/Contrib/lua/src/lapi.c`, including
stack manipulation, value conversion, closures, table access, metatables,
protected calls, loading/dumping, GC control, upvalues and Blizzard's abort hook.

The reference is sibling `Diablo3Decomp/src/Externals/lua/5.1.1/lapi.c` (Lua
5.1.1). Original DWARF records the same source locations and local names; all
68 functions were inspected with `tools/dwarf_fn.py`. The original Ghidra file
supplies the implementation evidence. `lua_abortaddress` returns `&L->abort`,
which DWARF places at offset 0xb8. Upstream-only lua_atpanic, lua_equal,
lua_cpcall/f_Ccall, lua_getallocf and lua_setallocf are excluded because the
original owner list does not contain them.

Assembly comparison of index2adr verifies positive and negative stack indices,
registry/environment/global pseudo-indices, upvalue selection and nil fallback.
lua_checkstack verifies the 2048-element bound and luaD_growstack call.
lua_call verifies luaD_call and LUA_MULTRET adjustment. lua_gc verifies each
operation and MAX_LUMEM = ~0 minus 2 (the original uses 0xfffffffffffffffd).
The recovered GC barriers and TValue copies use the original owner macros.

Validation: `tools/try_file.py` with `-O0 -mllvm -fast-isel=false` compiles all
68 functions, with zero missing and zero exact byte matches. Every definition
is explicitly NON_MATCHING. The systematic difference is the original Nintendo
compiler's redundant incoming-argument copies and spills, accompanied by
register allocation, local-slot and occasional load differences. No assembly,
register pinning or source padding was used to imitate that compiler behavior.
These are source reconstructions, not claims of byte-exact completion.

The shared Lua declarations now support both C and C++ compilation, retaining
DWARF layouts. Newly required internal declaration headers come from the same
Lua 5.1.1 reference. The full landing build/check must verify relocation targets
and preserve already-matching functions; per-file comparison does not do so.

## Follow-up behavioral review

Ghidra review of `lua_xmove`, `lua_gc`, `lua_error`, `lua_call`, `lua_pcall`,
`lua_load`, `lua_dump`, `aux_upvalue`, `lua_getupvalue`, and `lua_setupvalue`
found no additional Blizzard-specific behavior changes. In particular,
`lua_xmove` preserves element order and exits for identical states; protected
calls save byte offsets relative to `L->stack`; failed upvalue lookups leave the
stack unchanged; successful upvalue writes pop once and perform the GC barrier.
`lua_error` calls `luaG_errormsg` and retains its unreachable return-zero path.
The GC stop sentinel was corrected to `~(lu_mem)0 - 2` using original assembly.

`tests/lua/lapi_native.c` passed natively, and again with AddressSanitizer and
UndefinedBehaviorSanitizer. It covers stack/index/conversion/xmove behavior,
C upvalue bounds and updates, the abort hook and all GC selector branches.
The collector and exceptional dependency stubs are explicit; this is not a
substitute for running the complete Lua VM.
