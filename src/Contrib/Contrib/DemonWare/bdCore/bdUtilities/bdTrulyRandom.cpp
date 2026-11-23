// Original file: Contrib/Contrib/DemonWare/bdCore/bdUtilities/bdTrulyRandom.cpp

#include "bdTrulyRandom.h"
#include "../bdContainers/bdSingleton.h"
// Defined in bdPlatform/bdPlatformRandom/bdPlatformTrulyRandom-switch.cpp; its header is not
// in the DWARF, so it is declared here.
void bdGetRandomUChar8(bdUByte8 * in, const bdUInt32 length);
template <> bdTrulyRandomImpl * bdSingleton<bdTrulyRandomImpl>::m_instance = NULL;  // line 13
// line 17
bdUInt bdTrulyRandomImpl::getRandomUInt() const
{
    bdUInt result = 0;
    getRandomUByte8(reinterpret_cast<bdUByte8 *>(&result), sizeof(result));
    return result;
}

// line 24
bdUInt64 bdTrulyRandomImpl::getRandomUInt64() const
{
    bdUInt64 result = 0;
    getRandomUByte8(reinterpret_cast<bdUByte8 *>(&result), sizeof(result));
    return result;
}

// line 31
void bdTrulyRandomImpl::getRandomUByte8(bdUByte8 * in, const bdUInt length) const
{
    bdGetRandomUChar8(in, length);
}

// line 39
bdTrulyRandomImpl::bdTrulyRandomImpl()
{
}
