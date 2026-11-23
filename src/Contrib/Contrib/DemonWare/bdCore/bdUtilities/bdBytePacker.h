// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdCore/bdUtilities/bdBytePacker.h

#pragma once

#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include "bdBitOperations.h"

class bdBytePacker {  // size 0x1
public:
    // line 47
    template<typename T>
    static bdBool appendBasicType(void* buffer, const bdUInt bufferSize, const bdUInt offset, bdUInt& newOffset, const T& var)
    {
        T nvar;
        bdBitOperations::endianSwap(var, nvar);
        return appendBuffer(buffer, bufferSize, offset, newOffset, &nvar, sizeof(T));
    }
    static bdBool appendBuffer(void * dest, const bdUInt32 destSize, const bdUInt offset, bdUInt & newOffset, const void *const src, const bdUInt writeSize);
    static bdBool appendEncodedUInt16(void * buffer, const bdUInt bufferSize, const bdUInt offset, bdUInt & newOffset, const bdUInt16 value);
    // line 136
    template<typename T>
    static bdBool removeBasicType(const void* buffer, const bdUInt bufferSize, const bdUInt offset, bdUInt& newOffset, T& var)
    {
        T nvar;
        const bdBool result = removeBuffer(buffer, bufferSize, offset, newOffset, &nvar, sizeof(T));
        if (result)
        {
            bdBitOperations::endianSwap(nvar, var);
        }
        return result;
    }
    static bdBool removeBuffer(const void * src, const bdUInt srcSize, const bdUInt offset, bdUInt & newOffset, void *const dest, const bdUInt readSize);
    static bdBool removeEncodedUInt16(const bdUByte8 * buffer, const bdUInt bufferSize, const bdUInt offset, bdUInt & newOffset, bdUInt16 & value);
    static bdBool skipBytes(const void *const buffer, const bdUInt bufferSize, const bdUInt offset, bdUInt & newOffset, const bdUInt bytes);
    static bdBool rewindBytes(const void *const buffer, const bdUInt, const bdUInt offset, bdUInt & newOffset, const bdUInt bytes);
};
static_assert(sizeof(bdBytePacker) == 0x1, "bdBytePacker");
