// Original file: Contrib/Contrib/DemonWare/bdCore/bdUtilities/bdBitOperations.h
// Hand-written from the DWARF.

#pragma once

#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include "../bdMemory/bdMemory-std.h"

class bdBitOperations {  // size 0x1
public:
    BD_CLASS_ALLOCATOR(bdBitOperations);

    static bdUInt nextPowerOf2(const bdUInt v);
    static bdUInt highBitNumber(bdUInt v);

    // line 70. Body invented: the Switch is little-endian, so this is a plain copy.
    template<typename T>
    static void endianSwap(const T& src, T& dest)
    {
        dest = src;
    }
};
static_assert(sizeof(bdBitOperations) == 0x1, "bdBitOperations");
