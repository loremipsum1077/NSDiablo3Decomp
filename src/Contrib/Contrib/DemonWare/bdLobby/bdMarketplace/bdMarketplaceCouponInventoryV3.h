// Adopted from the DWARF; field names and layout are original.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdMarketplace/bdMarketplaceCouponInventoryV3.h

#pragma once

#include "bdMarketplaceCouponInventoryV2.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"

class bdByteBuffer;

class bdMarketplaceCouponInventoryV3 : public bdMarketplaceCouponInventoryV2 {  // size 0x118
public:
    bdMarketplaceCouponInventoryV3();
    virtual ~bdMarketplaceCouponInventoryV3() {}
    virtual bdBool serialize(bdByteBuffer & buffer) const;  // vtable slot 4
    virtual bdBool deserialize(bdByteBufferRef buffer);  // vtable slot 2
    virtual bdUInt sizeOf();  // vtable slot 3
    void reset();
    bdUInt32 m_startDateTime;  // 0x104
};
static_assert(sizeof(bdMarketplaceCouponInventoryV3) == 0x118, "bdMarketplaceCouponInventoryV3");


// Functions defined in this file (bodies are not in the DWARF):
//   line    17  bdMarketplaceCouponInventoryV3::~bdMarketplaceCouponInventoryV3()  [0x7100b39150 (+1 more), inlined into 3 functions]
