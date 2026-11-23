// Adopted from the DWARF; field names and layout are original.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdMarketplace/bdMarketplaceCouponInventory.h

#pragma once

#include "../bdLobbyCommon/bdSerializable.h"
#include "../bdLobbyCommon/bdTaskResult.h"
#include "../bdLobbyCommon/bdUserAccountID.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdByteBuffer;

class bdMarketplaceCouponInventory : public bdTaskResult, public bdSerializable {  // size 0x118
public:
    BD_CLASS_ALLOCATOR(bdMarketplaceCouponInventory);
    bdMarketplaceCouponInventory();
    virtual ~bdMarketplaceCouponInventory() {}
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
    bdUInt32 m_customSourceType;  // 0x104
};
static_assert(sizeof(bdMarketplaceCouponInventory) == 0x118, "bdMarketplaceCouponInventory");


// Functions defined in this file (bodies are not in the DWARF):
//   line    35  bdMarketplaceCouponInventory::~bdMarketplaceCouponInventory()  [0x7100b3a210 (+1 more), inlined into 3 functions]
//   line    38  void bdMarketplaceCouponInventory::operator delete(void * p)  [inlined into 1 functions]
