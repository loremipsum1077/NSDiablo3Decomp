// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdCore/bdCrypto/bdAuthenticationTag.h

#pragma once

#include "../bdMemory/bdMemory-std.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdAuthenticationTag {  // size 0x20
    // vtable pointer
public:








    BD_CLASS_ALLOCATOR(bdAuthenticationTag)
    enum bdAuthenticationTagLimits {
        BD_TAG_SIZE_MIN = 12,
        BD_TAG_SIZE_MAX = 16,
    };
    bdAuthenticationTag();
    bdAuthenticationTag(const bdUByte8 *const, const bdUByte8);
    virtual ~bdAuthenticationTag();  // vtable slot 0
    bdBool setTag(const bdUByte8 * tag, const bdUByte8 tagLength);
    bdUByte8 * getTag();
    bdUByte8 getTagLength() const;
protected:
    bdUByte8 m_tag[16];  // 0x08
    bdUByte8 m_tagLength;  // 0x18
};
static_assert(sizeof(bdAuthenticationTag) == 0x20, "bdAuthenticationTag");
