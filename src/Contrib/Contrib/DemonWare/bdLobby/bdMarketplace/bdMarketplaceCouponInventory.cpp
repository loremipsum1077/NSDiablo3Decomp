// Original file: Contrib/Contrib/DemonWare/bdLobby/bdMarketplace/bdMarketplaceCouponInventory.cpp
#include "bdMarketplaceCouponInventory.h"
#include "../../bdCore/bdContainers/bdByteBuffer.h"
#include "../../bdPlatform/bdPlatformMemory/bdPlatformMemory.inl"

bdMarketplaceCouponInventory::bdMarketplaceCouponInventory() { reset(); }

bdBool bdMarketplaceCouponInventory::serialize(bdByteBuffer & buffer) const
{
    bdUInt32 couponIdSize = sizeof(m_couponId);
    bdBool writeOk = buffer.writeString(m_couponId, couponIdSize);
    writeOk = writeOk && ContextSerialization::writeUserID(&buffer, m_ownerPlayerId);
    writeOk = writeOk && buffer.writeBool(m_claimed);
    writeOk = writeOk && buffer.writeUInt32(m_productId);
    writeOk = writeOk && ContextSerialization::writeUserID(&buffer, m_buyerPlayerId);
    writeOk = writeOk && buffer.writeUInt32(m_skuId);
    writeOk = writeOk && buffer.writeUInt32(m_rentalDuration);
    writeOk = writeOk && buffer.writeUInt32(m_expireDateTime);
    writeOk = writeOk && buffer.writeInt32(m_penalty_percentage);
    return writeOk;
}

bdBool bdMarketplaceCouponInventory::deserialize(bdByteBufferRef buffer)
{
    bdUInt32 couponIdSize = sizeof(m_couponId);
    bdBool ok = buffer->readString(m_couponId, couponIdSize);
    ok = ok && ContextSerialization::readUserID(buffer, m_ownerPlayerId);
    ok = ok && buffer->readBool(m_claimed);
    ok = ok && buffer->readUInt32(m_productId);
    ok = ok && ContextSerialization::readUserID(buffer, m_buyerPlayerId);
    ok = ok && buffer->readUInt32(m_skuId);
    ok = ok && buffer->readUInt32(m_rentalDuration);
    ok = ok && buffer->readUInt32(m_expireDateTime);
    ok = ok && buffer->readInt32(m_penalty_percentage);
    return ok;
}

bdUInt bdMarketplaceCouponInventory::sizeOf() { return sizeof(*this); }

void bdMarketplaceCouponInventory::reset()
{
    // The owner temporary survives through the claimed assignment (original line evidence).
    m_ownerPlayerId = bdOnlineUserID(-1),
    m_claimed = false;
    m_productId = 0;
    m_buyerPlayerId = bdOnlineUserID(0);
    m_skuId = -1;
    m_rentalDuration = -1;
    m_expireDateTime = -1;
    bdMemset(m_couponId, 0, sizeof(m_couponId));
    m_penalty_percentage = 0;
    bdMemset(m_metadata, 0, sizeof(m_metadata));
    m_metadataSize = 0;
    m_customSourceType = 0;
}

bdInt bdMarketplaceCouponInventory::serializedSizeOf() const
{
    return ContextSerialization::getUserIDSize(m_ownerPlayerId)
        + ContextSerialization::getUserIDSize(m_buyerPlayerId) + 0x34;
}
