// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdCore/bdUtilities/bdTrulyRandom.h

#pragma once

#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include "../bdMemory/bdMemory-std.h"
#include <stddef.h>
#include <stdint.h>

template <class T> class bdSingleton;
class bdTrulyRandomImpl {  // size 0x1
public:
    BD_CLASS_ALLOCATOR(bdTrulyRandomImpl);
    bdUInt getRandomUInt() const;
    bdUInt64 getRandomUInt64() const;
    void getRandomUByte8(bdUByte8 * in, const bdUInt length) const;
protected:
    template<class T> friend class bdSingleton;
    bdTrulyRandomImpl();
};
static_assert(sizeof(bdTrulyRandomImpl) == 0x1, "bdTrulyRandomImpl");
