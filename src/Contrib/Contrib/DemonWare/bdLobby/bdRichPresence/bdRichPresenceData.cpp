// Original file: Contrib/Contrib/DemonWare/bdLobby/bdRichPresence/bdRichPresenceData.cpp
// Blank lines keep the original line numbers.
#include "bdRichPresenceData.h"
#include "../../bdCore/bdContainers/bdByteBuffer.h"
#include "../../bdPlatform/bdPlatformMemory/bdPlatformMemory.inl"







bdRichPresenceData::bdRichPresenceData()
    : m_onlineStatus(0), m_presenceDataSize(0)
{
    bdMemset(m_presenceData, 0, sizeof(m_presenceData));
}






bdRichPresenceData::bdRichPresenceData(const bdRichPresenceStatus onlineStatus, const bdUByte8 *const presenceData, const bdUInt size)
{
    setRichPresenceData(NULL, onlineStatus, presenceData, size);
}











void bdRichPresenceData::setRichPresenceData(const bdUserAccountID * userAccountID, const bdRichPresenceStatus onlineStatus, const bdUByte8 *const presenceData, const bdUInt size)
{
    if (userAccountID) {
        m_userAccountID = *userAccountID;
    }
    m_onlineStatus = onlineStatus;
    if (presenceData) {
        m_presenceDataSize = size > BD_MAX_USER_RICH_PRESENCE_DATA_LENGTH ? BD_MAX_USER_RICH_PRESENCE_DATA_LENGTH : size;
        bdMemcpy(m_presenceData, presenceData, m_presenceDataSize);
    } else {
        m_presenceDataSize = 0;
        bdMemset(m_presenceData, 0, sizeof(m_presenceData));
    }
}









const bdUserAccountID & bdRichPresenceData::getUserAccountID() const { return m_userAccountID; }




const bdUByte8 * bdRichPresenceData::getPresenceData() const { return m_presenceData; }




bdUInt bdRichPresenceData::getPresenceDataSize() const { return m_presenceDataSize; }




bdRichPresenceStatus bdRichPresenceData::getOnlineStatus() const { return m_onlineStatus; }




bdUInt bdRichPresenceData::sizeOf() { return sizeof(bdRichPresenceData); }




bdBool bdRichPresenceData::serialize(bdByteBuffer & buffer) const
{
    bdBool ok;
    bdUByte8 onlineStatus = m_onlineStatus;
    ok = m_userAccountID.serialize(buffer);
    ok = ok && buffer.writeUByte8(onlineStatus);
    ok = ok && buffer.writeBlob(m_presenceData, m_presenceDataSize);
    return ok;
}


bdBool bdRichPresenceData::deserialize(bdByteBufferRef buffer)
{
    bdUByte8 onlineStatus = 0;
    m_presenceDataSize = BD_MAX_USER_RICH_PRESENCE_DATA_LENGTH;
    bdBool ok = m_userAccountID.deserialize(buffer);
    ok = ok && buffer->readUByte8(onlineStatus);
    ok = ok && buffer->readBlob(m_presenceData, m_presenceDataSize);
    if (ok) {
        m_onlineStatus = onlineStatus;
    }
    return ok;
}
