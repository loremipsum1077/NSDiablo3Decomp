#include "bdQoSRequestPacket.h"
#include "../../../bdCore/bdUtilities/bdBytePacker.h"
#include "../../../bdPlatform/bdPlatformTiming/bdPlatformTiming.h"

bdQoSRequestPacket::bdQoSRequestPacket(bdUInt32 secid, bdUInt32 id)
    : m_type(40), m_id(id), m_secid(secid), m_requestingData(false)
{ m_timestamp = bdPlatformTiming::getHiResTimeStamp(); }
bdQoSRequestPacket::bdQoSRequestPacket()
    : m_type(0), m_timestamp(0), m_id(0), m_secid(0), m_requestingData(false) {}
bdUInt64 bdQoSRequestPacket::getTimestamp() const { return m_timestamp; }
bdUInt32 bdQoSRequestPacket::getId() const { return m_id; }
bdUInt32 bdQoSRequestPacket::getSecId() const { return m_secid; }
void bdQoSRequestPacket::setRequestingData(bdBool requestingData) { m_requestingData = requestingData; }
bdBool bdQoSRequestPacket::getRequestingData() const { return m_requestingData; }
bdBool bdQoSRequestPacket::serialize(void * data, const bdUInt size, const bdUInt offset, bdUInt & newOffset) const
{
    newOffset = offset;
    bdBool status = bdBytePacker::appendBasicType(data, size, newOffset, newOffset, m_type) &&
        bdBytePacker::appendBasicType(data, size, newOffset, newOffset, m_timestamp) &&
        bdBytePacker::appendBasicType(data, size, newOffset, newOffset, m_id) &&
        bdBytePacker::appendBasicType(data, size, newOffset, newOffset, m_secid) &&
        bdBytePacker::appendBasicType(data, size, newOffset, newOffset, m_requestingData);
    if (!status) newOffset = offset;
    return status;
}
bdBool bdQoSRequestPacket::deserialize(const void * data, const bdUInt size, const bdUInt offset, bdUInt & newOffset)
{
    newOffset = offset;
    bdBool status = bdBytePacker::removeBasicType(data, size, newOffset, newOffset, m_type) &&
        bdBytePacker::removeBasicType(data, size, newOffset, newOffset, m_timestamp) &&
        bdBytePacker::removeBasicType(data, size, newOffset, newOffset, m_id) &&
        bdBytePacker::removeBasicType(data, size, newOffset, newOffset, m_secid) &&
        bdBytePacker::removeBasicType(data, size, newOffset, newOffset, m_requestingData);
    if (!status) newOffset = offset;
    return status;
}
