// Original file: Contrib/Contrib/DemonWare/bdLobby/bdMail/bdMailInfo.cpp
#include "bdMailInfo.h"
#include "../../bdCore/bdContainers/bdByteBuffer.h"

bdMailInfo::bdMailInfo() {}
bdMailInfo::~bdMailInfo() {}
bdBool bdMailInfo::deserialize(bdByteBufferRef buffer)
{
    bdBool ok = ContextSerialization::readUserID(buffer, m_senderID);
    ok = ok && buffer->readString(m_senderName, sizeof(m_senderName))
        && buffer->readUInt64(m_mailID)
        && buffer->readUInt32(m_timeStamp)
        && buffer->readUInt32(m_size)
        && buffer->readUInt16(m_mailCategory)
        && buffer->readBool(m_isRead)
        && buffer->readBool(m_isUndelivered)
        && buffer->readUInt32(m_lastUpdated)
        && buffer->readUInt32(m_messageDuration)
        && buffer->readUInt32(m_secondsToExpiration);
    return ok;
}
bdUInt32 bdMailInfo::sizeOf() { return sizeof(bdMailInfo); }
