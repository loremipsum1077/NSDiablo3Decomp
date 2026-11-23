// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdCore/bdCrypto/bdHashSHA256.h

#pragma once

#include "bdHash.h"
#include "../bdMemory/bdMemory-std.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdHashSHA256 : public bdHash {  // size 0x10
public:




    BD_CLASS_ALLOCATOR(bdHashSHA256)  // line 18
    bdHashSHA256();
    virtual ~bdHashSHA256();  // vtable slot 0
    virtual bdBool hash(const bdUByte8 * data, const bdUInt dataSize, bdUByte8 * result, bdUInt & resultSize);  // vtable slot 2
protected:
    static bdInt s_hash;
};
static_assert(sizeof(bdHashSHA256) == 0x10, "bdHashSHA256");
