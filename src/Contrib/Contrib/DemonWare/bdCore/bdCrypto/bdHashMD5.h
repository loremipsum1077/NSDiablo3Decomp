// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdCore/bdCrypto/bdHashMD5.h

#pragma once

#include "bdHash.h"
#include "../bdMemory/bdMemory-std.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdHashMD5 : public bdHash {  // size 0x10
public:




    BD_CLASS_ALLOCATOR(bdHashMD5)  // line 18
    bdHashMD5();
    virtual ~bdHashMD5();  // vtable slot 0
    virtual bdBool hash(const bdUByte8 * data, const bdUInt dataSize, bdUByte8 * result, bdUInt & resultSize);  // vtable slot 2
protected:
    static bdInt s_hash;
};
static_assert(sizeof(bdHashMD5) == 0x10, "bdHashMD5");
