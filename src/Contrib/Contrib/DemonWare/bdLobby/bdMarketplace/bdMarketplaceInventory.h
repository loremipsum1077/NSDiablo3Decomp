// Adopted from DWARF.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdMarketplace/bdMarketplaceInventory.h

#pragma once

#include "../../bdCore/bdSocket/bdThreadedStreamSocket.h"
#include "../bdLobbyCommon/bdSerializable.h"
#include "../bdLobbyCommon/bdTaskResult.h"
#include "../bdLobbyCommon/bdUserAccountID.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdByteBuffer;

class bdMarketplaceInventory : public bdTaskResult, public bdSerializable {  // size 0xc8
public:
    BD_CLASS_ALLOCATOR(bdMarketplaceInventory);
    bdMarketplaceInventory();
    virtual ~bdMarketplaceInventory() {}
    bdMarketplaceInventory(const bdUserID, const bdUInt32, const bdUInt32, const bdUInt32, const bdUByte8 *, const bdUByte8, const bdUInt32, const bdInt64, const bdUInt16, const bdUInt32, const bdUInt32);
    virtual bdBool serialize(bdByteBuffer & buffer) const;  // vtable slot 4
    virtual bdBool deserialize(bdByteBufferRef buffer);  // vtable slot 2
    virtual bdBool deserialize(bdByteBufferRef buffer, const bdUserID playerId);  // vtable slot 5
    virtual bdUInt sizeOf();  // vtable slot 3
    virtual bdInt serializedSizeOf() const;  // vtable slot 6
    void reset();
    bdUserID m_playerId;  // 0x10
    bdUInt32 m_itemId;  // 0x48
    bdUInt32 m_itemQuantity;  // 0x4c
    bdUInt32 m_itemXp;  // 0x50
    bdUByte8 m_itemData[64];  // 0x54
    bdUByte8 m_itemDataSize;  // 0x94
    bdUInt32 m_expireDateTime;  // 0x98
    bdInt64 m_expiryDuration;  // 0xa0
    bdUInt16 m_collisionField;  // 0xa8
    bdUInt32 m_modDateTime;  // 0xac
    bdUInt32 m_customSourceType;  // 0xb0
};
static_assert(sizeof(bdMarketplaceInventory) == 0xc8, "bdMarketplaceInventory");


// Functions defined in this file (bodies are not in the DWARF):
//   line    20  bdMarketplaceInventory::~bdMarketplaceInventory()  [0x7100b43ee0 (+1 more), inlined into 4 functions]
//   line    23  void bdMarketplaceInventory::operator delete(void * p)  [inlined into 1 functions]
