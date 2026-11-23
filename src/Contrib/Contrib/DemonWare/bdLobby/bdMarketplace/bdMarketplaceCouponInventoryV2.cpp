// Original file: Contrib/Contrib/DemonWare/bdLobby/bdMarketplace/bdMarketplaceCouponInventoryV2.cpp
#include "bdMarketplaceCouponInventoryV2.h"
#include "../../bdCore/bdContainers/bdByteBuffer.h"
#include "../../bdPlatform/bdPlatformMemory/bdPlatformMemory.inl"

bdMarketplaceCouponInventoryV2::bdMarketplaceCouponInventoryV2() { reset(); }

bdBool bdMarketplaceCouponInventoryV2::serialize(bdByteBuffer & buffer) const
{
    bdUInt32 couponIdSize = sizeof(m_couponId);
    bdBool writeOk = buffer.writeString(m_couponId, couponIdSize);
    writeOk = writeOk && ContextSerialization::writeUserID(&buffer, m_ownerPlayerId);
    writeOk = writeOk && buffer.writeBool(m_claimed);
    writeOk = writeOk && buffer.writeUInt32(m_productId);
    writeOk = writeOk && ContextSerialization::writeUserID(&buffer, m_buyerPlayerId);
    writeOk = writeOk && buffer.writeUInt32(m_skuId);
    writeOk = writeOk && buffer.writeUInt32(m_rentalDuration);
    writeOk = writeOk && buffer.writeUByte8(m_rentalDurationUnit);
    writeOk = writeOk && buffer.writeUInt32(m_expireDateTime);
    writeOk = writeOk && buffer.writeInt32(m_penalty_percentage);
    writeOk = writeOk && buffer.writeUInt32(m_productExpireDateTime);
    writeOk = writeOk && buffer.writeBlob(m_metadata, m_metadataSize);
    writeOk = writeOk && buffer.writeUByte8(m_customSourceType);
    return writeOk;
}

bdBool bdMarketplaceCouponInventoryV2::deserialize(bdByteBufferRef buffer)
{
    bdUInt32 couponIdSize = sizeof(m_couponId);
    bdBool ok = buffer->readString(m_couponId, couponIdSize);
    ok = ok && ContextSerialization::readUserID(buffer, m_ownerPlayerId);
    ok = ok && buffer->readBool(m_claimed);
    ok = ok && buffer->readUInt32(m_productId);
    ok = ok && ContextSerialization::readUserID(buffer, m_buyerPlayerId);
    ok = ok && buffer->readUInt32(m_skuId);
    ok = ok && buffer->readUInt32(m_rentalDuration);
    ok = ok && buffer->readUByte8(m_rentalDurationUnit);
    ok = ok && buffer->readUInt32(m_expireDateTime);
    ok = ok && buffer->readInt32(m_penalty_percentage);
    ok = ok && buffer->readUInt32(m_productExpireDateTime);
    bdUInt32 metadataSize = sizeof(m_metadata);
    ok = ok && buffer->readBlob(m_metadata, metadataSize);
    m_metadataSize = metadataSize;
    ok = ok && buffer->readUByte8(m_customSourceType);
    return ok;
}

bdUInt bdMarketplaceCouponInventoryV2::sizeOf() { return sizeof(*this); }

void bdMarketplaceCouponInventoryV2::reset()
{
    // The owner temporary survives through the claimed assignment (original line evidence).
    m_ownerPlayerId = bdOnlineUserID(-1),
    m_claimed = false;
    m_productId = 0;
    m_buyerPlayerId = bdOnlineUserID(0);
    m_skuId = -1;
    m_rentalDuration = -1;
    m_rentalDurationUnit = 5;
    m_expireDateTime = -1;
    bdMemset(m_couponId, 0, sizeof(m_couponId));
    m_penalty_percentage = 0;
    bdMemset(m_metadata, 0, sizeof(m_metadata));
    m_metadataSize = 0;
    m_customSourceType = 0;
}

bdInt bdMarketplaceCouponInventoryV2::serializedSizeOf() const
{
    return ContextSerialization::getUserIDSize(m_ownerPlayerId)
        + ContextSerialization::getUserIDSize(m_buyerPlayerId) + m_metadataSize + 0x43;
}
