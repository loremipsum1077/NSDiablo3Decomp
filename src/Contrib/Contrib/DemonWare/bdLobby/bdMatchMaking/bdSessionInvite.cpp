// Original file: Contrib/Contrib/DemonWare/bdLobby/bdMatchMaking/bdSessionInvite.cpp

#include "bdSessionInvite.h"
#include "../../bdPlatform/bdPlatformLog/bdPlatformLog.h"








bdSessionInvite::bdSessionInvite()
    : bdTaskResult(), m_senderID(0), m_sessionID(), m_attachmentSize(0)
{
}


bdSessionInvite::~bdSessionInvite()
{
}

bdBool bdSessionInvite::deserialize(bdByteBufferRef buffer)
{
    bdBool ok;

    ok = buffer->readUInt64(m_senderID);
    ok = ok && buffer->readString(m_senderName, sizeof(m_senderName));


    bdUInt length = sizeof(m_sessionID.m_sessionID.ab);
    ok = ok && buffer->readBlob(m_sessionID.m_sessionID.ab, length);

    m_attachmentSize = sizeof(m_attachment);
    ok = ok && buffer->readBlob(m_attachment, m_attachmentSize);

    if (!ok)
    {
        bdLogError("session invite", "Failed to deserialize");
    }
    return ok;
}

bdUInt bdSessionInvite::sizeOf()
{
    return sizeof(bdSessionInvite);
}
