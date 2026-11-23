// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdCore/bdCrypto/bdHashSHA1.h

#pragma once

#include "bdHash.h"
#include "../bdMemory/bdMemory-std.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdHashSHA1 : public bdHash {  // size 0x10
public:




    BD_CLASS_ALLOCATOR(bdHashSHA1)  // line 18
    bdHashSHA1();
    virtual ~bdHashSHA1();  // vtable slot 0
    virtual bdBool hash(const bdUByte8 * data, const bdUInt dataSize, bdUByte8 * result, bdUInt & resultSize);  // vtable slot 2
protected:
    static bdInt s_hash;
};
static_assert(sizeof(bdHashSHA1) == 0x10, "bdHashSHA1");
