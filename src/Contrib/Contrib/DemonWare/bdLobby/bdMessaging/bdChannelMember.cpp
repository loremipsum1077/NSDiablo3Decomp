#include "bdChannelMember.h"
#include "../../bdCore/bdContainers/bdByteBuffer.h"










bdChannelMember::bdChannelMember() : m_userID(0), m_subscribedTimestamp(0)
{
    m_userName[0] = 0;
}



bdChannelMember::~bdChannelMember() {}



bdBool bdChannelMember::deserialize(bdByteBufferRef buffer)
{
    bdBool ok = buffer->readUInt64(m_userID);
    ok = ok && buffer->readString(m_userName, sizeof(m_userName))
        && buffer->readUInt32(m_subscribedTimestamp);
    return ok;
}




bdUInt bdChannelMember::sizeOf() { return sizeof(bdChannelMember); }
