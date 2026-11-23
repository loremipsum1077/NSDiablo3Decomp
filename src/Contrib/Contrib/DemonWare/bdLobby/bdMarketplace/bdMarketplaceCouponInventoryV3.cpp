// Original file: Contrib/Contrib/DemonWare/bdLobby/bdMarketplace/bdMarketplaceCouponInventoryV3.cpp
#include "bdMarketplaceCouponInventoryV3.h"
#include "../../bdCore/bdContainers/bdByteBuffer.h"

bdMarketplaceCouponInventoryV3::bdMarketplaceCouponInventoryV3() { reset(); }
bdBool bdMarketplaceCouponInventoryV3::serialize(bdByteBuffer & buffer) const
{
    bdBool writeOk = bdMarketplaceCouponInventoryV2::serialize(buffer);
    return writeOk && buffer.writeUInt32(m_startDateTime);
}
bdBool bdMarketplaceCouponInventoryV3::deserialize(bdByteBufferRef buffer)
{
    bdBool ok = bdMarketplaceCouponInventoryV2::deserialize(buffer);
    return ok && buffer->readUInt32(m_startDateTime);
}
bdUInt bdMarketplaceCouponInventoryV3::sizeOf() { return sizeof(*this); }
void bdMarketplaceCouponInventoryV3::reset()
{
    bdMarketplaceCouponInventoryV2::reset();
    m_startDateTime = -1;
}
