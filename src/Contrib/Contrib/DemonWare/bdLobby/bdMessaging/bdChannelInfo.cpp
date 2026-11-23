#include "bdChannelInfo.h"
#include "../../bdCore/bdContainers/bdByteBuffer.h"










bdChannelInfo::bdChannelInfo() : m_channelID(0), m_numSubscribers(0), m_maxSubscribers(0), m_adminID(0), m_isPasswordRequired(false), m_isPublicChannel(false), m_dataVersion(0) {}










bdChannelInfo::~bdChannelInfo() {}



bdBool bdChannelInfo::deserialize(bdByteBufferRef buffer)
{
    bdBool ok = buffer->readUInt64(m_channelID);

    ok = ok && buffer->readUInt32(m_numSubscribers)
        && buffer->readUInt32(m_maxSubscribers)

        && buffer->readUInt64(m_adminID)
        && buffer->readBool(m_isPasswordRequired)
        && buffer->readBool(m_isPublicChannel)
        && buffer->readUInt32(m_dataVersion);
    return ok;
}


bdUInt bdChannelInfo::sizeOf() { return sizeof(bdChannelInfo); }
