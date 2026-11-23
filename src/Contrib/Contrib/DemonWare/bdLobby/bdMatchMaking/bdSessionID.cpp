#include "bdSessionID.h"
#include "../../bdCore/bdContainers/bdByteBuffer.h"
#include "../../bdPlatform/bdPlatformMemory/bdPlatformMemory.inl"







bdSessionID::bdSessionID() { bdMemset(m_sessionID.ab, 0, sizeof(m_sessionID.ab)); }





bdSessionID::bdSessionID(const bdSessionID & other) { m_sessionID = other.m_sessionID; }




bdBool bdSessionID::deserialize(bdByteBufferRef buffer)
{
    bdUInt32 blobLength = sizeof(m_sessionID.ab);
    bdBool ok = buffer->readBlob(m_sessionID.ab, blobLength);
    return ok;
}




bdBool bdSessionID::serialize(bdByteBuffer & buffer) const
{
    return buffer.writeBlob(m_sessionID.ab, sizeof(m_sessionID.ab));
}


bdUInt bdSessionID::sizeOf() { return sizeof(bdSessionID); }
