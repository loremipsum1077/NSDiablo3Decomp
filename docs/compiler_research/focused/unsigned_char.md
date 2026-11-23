# Lua plain-char semantics

Current CMake appends `-funsigned-char` to all Lua C source owners after their
scoped options. This preserves the global signed-char policy for game code while
matching the unsigned plain-char semantics recorded in all 28 saved Lua CUs.

The dated investigation at core source commit
`080f190041417429f3fe42c7773ff6c967898cf6` predates that override: its references to
"current signed char" described older saved commands and are not today's policy.
Original DWARF records unsigned language semantics, not the precise original flag.
The saved dump/index identity limitations remain; no new NSS extraction or build
was performed for this documentation correction.

See [compile flags](../../compile_flags.md) and `toolchain/lua/README.md` for current
source-scoped flags and the pinned optional backend.
