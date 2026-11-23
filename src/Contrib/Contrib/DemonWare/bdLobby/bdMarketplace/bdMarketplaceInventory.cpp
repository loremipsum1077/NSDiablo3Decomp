#include "bdMarketplaceInventory.h"
#include "../../bdCore/bdContainers/bdByteBuffer.h"
#include "../../bdPlatform/bdPlatformMemory/bdPlatformMemory.inl"
#include <limits.h>
bdMarketplaceInventory::bdMarketplaceInventory() { reset(); }
bdBool bdMarketplaceInventory::serialize(bdByteBuffer &buffer) const
{
 bdBool writeOk = ContextSerialization::writeUserID(&buffer, m_playerId);
 writeOk = writeOk && buffer.writeUInt32(m_itemId);
 writeOk = writeOk && buffer.writeUInt32(m_itemQuantity);
 writeOk = writeOk && buffer.writeUInt32(m_itemXp);
 writeOk = writeOk && buffer.writeBlob(m_itemData, m_itemDataSize);
 writeOk = writeOk && buffer.writeUInt32(m_expireDateTime);
 writeOk = writeOk && buffer.writeInt64(m_expiryDuration);
 writeOk = writeOk && buffer.writeUInt16(m_collisionField);
 writeOk = writeOk && buffer.writeUInt32(m_modDateTime);
 return writeOk;
}
bdBool bdMarketplaceInventory::deserialize(bdByteBufferRef buffer)
{
 return deserialize(buffer, bdUserID(bdOnlineUserID(-1)));
}
bdBool bdMarketplaceInventory::deserialize(bdByteBufferRef buffer, const bdUserID playerId)
{
 bdBool ok = true;
 if (playerId == bdUserID(bdOnlineUserID(-1)))
     ok = ContextSerialization::readUserID(buffer, m_playerId);
 else
     m_playerId = playerId;
 ok = ok && buffer->readUInt32(m_itemId);
 ok = ok && buffer->readUInt32(m_itemQuantity);
 ok = ok && buffer->readUInt32(m_itemXp);
 bdUInt32 itemDataSize = sizeof(m_itemData);
 ok = ok && buffer->readBlob(m_itemData, itemDataSize);
 m_itemDataSize = static_cast<bdUByte8>(itemDataSize);
 ok = ok && buffer->readUInt32(m_expireDateTime);
 ok = ok && buffer->readInt64(m_expiryDuration);
 ok = ok && buffer->readUInt16(m_collisionField);
 ok = ok && buffer->readUInt32(m_modDateTime);
 return ok;
}
bdUInt bdMarketplaceInventory::sizeOf() { return sizeof(*this); }
void bdMarketplaceInventory::reset()
{
 m_playerId = bdUserID(bdOnlineUserID(0));
 m_itemId = 0;
 m_itemQuantity = 0;
 m_itemXp = 0;
 m_itemDataSize = 0;
 m_expireDateTime = -1;
 m_expiryDuration = LLONG_MAX;
 m_collisionField = 0;
 m_modDateTime = -1;
 bdMemset(m_itemData, 0, sizeof(m_itemData));
 m_customSourceType = 0;
}
bdInt bdMarketplaceInventory::serializedSizeOf() const
{
 return ContextSerialization::getUserIDSize(m_playerId) + m_itemDataSize + 43;
}
