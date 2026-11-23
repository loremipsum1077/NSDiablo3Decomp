#include "bdQoSReplyPacket.h"
#include "../../../bdCore/bdUtilities/bdBytePacker.h"
#include "../../../bdCore/bdMemory/bdMemory.inl"
#include "../../../bdPlatform/bdPlatformTiming/bdPlatformTiming.h"
extern void bdUseVAArgs(...);

bdQoSReplyPacket::bdQoSReplyPacket() : m_type(41), m_id(0), m_timestamp(0), m_enabledMode(true), m_bandwidth(0), m_hostProcessingTime(0), m_data(0), m_dataSize(0), m_serializedSize(0), m_hasData(false) {}
bdQoSReplyPacket::~bdQoSReplyPacket() { if (m_data) { bdDeallocate(m_data); m_data = 0; m_dataSize = 0; } }
void bdQoSReplyPacket::setId(bdUInt32 id) { m_id = id; }
void bdQoSReplyPacket::setTimestamp(bdUInt64 timestamp) { m_timestamp = timestamp; }
bdFloat32 bdQoSReplyPacket::getElapsedTime() const { bdFloat32 time = bdPlatformTiming::getElapsedTime(m_timestamp, bdPlatformTiming::getHiResTimeStamp()); return time - m_hostProcessingTime; }
void bdQoSReplyPacket::setHostProcessingTime(bdFloat32 hostProcessingTime) { m_hostProcessingTime = hostProcessingTime; }
void bdQoSReplyPacket::setHasData(bdBool hasData) { m_hasData = hasData; }
bdUInt32 bdQoSReplyPacket::getId() const { return m_id; }
bdUByte8 * bdQoSReplyPacket::getData() { return m_data; }
bdUInt32 bdQoSReplyPacket::getDataSize() const { return m_dataSize; }
bdBool bdQoSReplyPacket::getEnabled() const { return m_enabledMode; }
bdUInt32 bdQoSReplyPacket::getBandwidth() const { return m_bandwidth; }
void bdQoSReplyPacket::setBandwidth(bdUInt bandwidth) { m_bandwidth = bandwidth; }
bdBool bdQoSReplyPacket::serialize(void * data, const bdUInt size, const bdUInt offset, bdUInt & newOffset) const
{
    newOffset = offset;
    bdBool status = bdBytePacker::appendBasicType(data, size, newOffset, newOffset, m_type)
        && bdBytePacker::appendBasicType(data, size, newOffset, newOffset, m_id)
        && bdBytePacker::appendBasicType(data, size, newOffset, newOffset, m_timestamp)
        && bdBytePacker::appendBasicType(data, size, newOffset, newOffset, m_enabledMode);
    if (m_hasData) status = status && bdBytePacker::appendBasicType(data, size, newOffset, newOffset, m_dataSize);
    else status = status && bdBytePacker::appendBasicType(data, size, newOffset, newOffset, 0);
    status = status && bdBytePacker::appendBasicType(data, size, newOffset, newOffset, m_bandwidth)
        && bdBytePacker::appendBasicType(data, size, newOffset, newOffset, m_hostProcessingTime)
        && bdBytePacker::appendBasicType(data, size, newOffset, newOffset, m_hasData);
    if (m_dataSize && m_hasData) status = status && bdBytePacker::appendBuffer(data, size, newOffset, newOffset, m_data, m_dataSize);
    if (!status) newOffset = offset;
    return status;
}
bdBool bdQoSReplyPacket::deserialize(const void * data, const bdUInt size, const bdUInt offset, bdUInt & newOffset)
{
    newOffset = offset;
    bdBool status = bdBytePacker::removeBasicType(data, size, newOffset, newOffset, m_type) &&
        bdBytePacker::removeBasicType(data, size, newOffset, newOffset, m_id) &&
        bdBytePacker::removeBasicType(data, size, newOffset, newOffset, m_timestamp) &&
        bdBytePacker::removeBasicType(data, size, newOffset, newOffset, m_enabledMode) &&
        bdBytePacker::removeBasicType(data, size, newOffset, newOffset, m_dataSize) &&
        bdBytePacker::removeBasicType(data, size, newOffset, newOffset, m_bandwidth) &&
        bdBytePacker::removeBasicType(data, size, newOffset, newOffset, m_hostProcessingTime) &&
        bdBytePacker::removeBasicType(data, size, newOffset, newOffset, m_hasData);
    if (size - newOffset != m_dataSize) { bdUseVAArgs(); status = false; }
    else if (status) {
        if (m_data) { bdDeallocate(m_data); m_data = 0; }
        if (m_dataSize) {
            m_data = bdAllocate<bdUByte8>(m_dataSize);
            if (m_data) status = bdBytePacker::removeBuffer(data, size, newOffset, newOffset, m_data, m_dataSize);
            else { bdUseVAArgs(); status = false; }
        }
    }
    if (!status) newOffset = offset;
    return status;
}
bdUInt bdQoSReplyPacket::getSerializedSize() { serialize(0, 65535, 0, m_serializedSize); return m_serializedSize; }
