// Original file: Contrib/Contrib/DemonWare/bdCore/bdUtilities/bdBitOperations.cpp

#include "bdBitOperations.h"

// line 18
bdUInt bdBitOperations::nextPowerOf2(const bdUInt v)
{
    bdUInt v2 = v;
    v2 |= v2 >> 1;
    v2 |= v2 >> 2;
    v2 |= v2 >> 4;
    v2 |= v2 >> 8;
    v2 |= v2 >> 16;
    v2 &= ~(v2 >> 1);
    if (v2 != v)
    {
        v2 <<= 1;
    }
    return v2;
}

// line 39
bdUInt bdBitOperations::highBitNumber(bdUInt v)
{
    bdUInt i = (v & 0xFFFF0000) ? 16 : 0;
    v >>= i;
    if (v & 0xFF00)
    {
        i |= 8;
        v >>= 8;
    }
    if (v & 0xF0)
    {
        i |= 4;
        v >>= 4;
    }
    if (v & 0xC)
    {
        i |= 2;
        v >>= 2;
    }
    i |= v >> 1;
    return i;
}
