// Original file: BlizzardCore/Include/Blizzard/Util/Byteswap.h
// Only the two swizzle specializations the DWARF shows (lines 300 and 303). Their bodies are
// not in the DWARF; the callers show a single rev, which is what the bswap builtins give.

#pragma once

#include "../Types.h"

template <size_t SIZE>
void Macro_Endian_SwizzleT(void * lvalue);

template <>
inline void Macro_Endian_SwizzleT<4>(void * lvalue) {
    *(uint32 *)lvalue = __builtin_bswap32(*(uint32 *)lvalue);
}

template <>
inline void Macro_Endian_SwizzleT<8>(void * lvalue) {
    *(uint64 *)lvalue = __builtin_bswap64(*(uint64 *)lvalue);
}
