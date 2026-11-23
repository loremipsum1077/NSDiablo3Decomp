// Adopted from the DWARF; field names and layout are original.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdMarketplace/bdMarketplaceCouponInventoryV2.h

#pragma once

#include "../bdLobbyCommon/bdSerializable.h"
#include "../bdLobbyCommon/bdTaskResult.h"
#include "../bdLobbyCommon/bdUserAccountID.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdByteBuffer;

class bdMarketplaceCouponInventoryV2 : public bdTaskResult, public bdSerializable {  // size 0x118
public:
    BD_CLASS_ALLOCATOR(bdMarketplaceCouponInventoryV2);
    bdMarketplaceCouponInventoryV2();
    virtual ~bdMarketplaceCouponInventoryV2() {}
    virtual bdBool serialize(bdByteBuffer & buffer) const;  // vtable slot 4
    virtual bdBool deserialize(bdByteBufferRef buffer);  // vtable slot 2
    virtual bdUInt sizeOf();  // vtable slot 3
    virtual bdInt serializedSizeOf() const;  // vtable slot 5
    void reset();
    bdNChar8 m_couponId[25];  // 0x10
    bdUserID m_ownerPlayerId;  // 0x30
    bdBool m_claimed;  // 0x68
    bdUInt32 m_productId;  // 0x6c
    bdUserID m_buyerPlayerId;  // 0x70
    bdUInt32 m_skuId;  // 0xa8
    bdUInt32 m_rentalDuration;  // 0xac
    bdUByte8 m_rentalDurationUnit;  // 0xb0
    bdUInt32 m_expireDateTime;  // 0xb4
    bdUInt32 m_productExpireDateTime;  // 0xb8
    bdInt32 m_penalty_percentage;  // 0xbc
    bdUByte8 m_metadata[64];  // 0xc0
    bdUByte8 m_metadataSize;  // 0x100
    bdUByte8 m_customSourceType;  // 0x101
};
static_assert(sizeof(bdMarketplaceCouponInventoryV2) == 0x118, "bdMarketplaceCouponInventoryV2");


// Functions defined in this file (bodies are not in the DWARF):
//   line    30  bdMarketplaceCouponInventoryV2::~bdMarketplaceCouponInventoryV2()  [0x7100b38df0 (+1 more), inlined into 6 functions]
//   line    33  void bdMarketplaceCouponInventoryV2::operator delete(void * p)  [inlined into 2 functions]
