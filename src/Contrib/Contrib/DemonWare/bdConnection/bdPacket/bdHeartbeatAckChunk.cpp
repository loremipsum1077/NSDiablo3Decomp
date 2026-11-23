// Original file: Contrib/Contrib/DemonWare/bdConnection/bdPacket/bdHeartbeatAckChunk.cpp
#include "bdHeartbeatAckChunk.h"
#include "../../bdCore/bdUtilities/bdBytePacker.h"
bdHeartbeatAckChunk::bdHeartbeatAckChunk() : bdChunk(BD_CT_HEARTBEAT_ACK), m_flags((bdHeartbeatAckFlags)0) {}
bdHeartbeatAckChunk::~bdHeartbeatAckChunk() {}
bdUInt bdHeartbeatAckChunk::serialize(bdUByte8* data, const bdUInt size) const
{
    bdUInt offset = bdChunk::serialize(data, size);
    bdUInt16 length = 0;
    const bdUByte8 flags = (bdUByte8)m_flags;
    bdBool ok = bdBytePacker::appendBasicType(data, size, offset, offset, flags);
    ok = ok && bdBytePacker::appendBasicType(data, size, offset, offset, length);
    return offset;
}
bdBool bdHeartbeatAckChunk::deserialize(const bdUByte8* const data, const bdUInt size, bdUInt& offset)
{
    bdUInt bytesRead = offset;
    bdUByte8 flags = 0;
    bdUInt16 length;
    bdBool ok = bdChunk::deserialize(data, size, bytesRead);
    ok = ok && bdBytePacker::removeBasicType(data, size, bytesRead, bytesRead, flags);
    m_flags = (bdHeartbeatAckFlags)flags;
    ok = ok && bdBytePacker::removeBasicType(data, size, bytesRead, bytesRead, length);
    if (ok) offset = bytesRead;
    return ok;
}
