// Original file: Contrib/Contrib/DemonWare/bdLobby/bdReward/bdRewardEventJSON.cpp
// The DWARF puts every function of this file on line 13.

#include "bdRewardEventJSON.h"
#include "../../bdPlatform/bdPlatformLog/bdPlatformLog.h"
#include "../../bdPlatform/bdPlatformMemory/bdPlatformMemory.inl"
#include "../../bdPlatform/bdPlatformString/bdPlatformString.inl"

bdRewardEventJSON::bdRewardEventJSON()
    : bdRewardEvent(BD_REWARD_EVENT_JSON)
{
    bdMemset(m_jsonData, 0, sizeof(m_jsonData));
}

bdRewardEventJSON::~bdRewardEventJSON()
{
}

bdBool bdRewardEventJSON::serialize(bdByteBuffer & buffer) const
{
    bdBool writeOk = bdRewardEvent::serialize(buffer);
    writeOk = writeOk && buffer.writeString(m_jsonData, sizeof(m_jsonData));
    return writeOk;
}

bdBool bdRewardEventJSON::deserialize(bdByteBufferRef buffer)
{
    bdBool ok = bdRewardEvent::deserialize(buffer);
    ok = ok && m_type == BD_REWARD_EVENT_JSON;
    ok = ok && buffer->readString(m_jsonData, sizeof(m_jsonData));
    return ok;
}

bdUInt bdRewardEventJSON::sizeOf()
{
    return sizeof(bdRewardEventJSON);
}

bdBool bdRewardEventJSON::set(const bdNChar8 * jsonData)
{
    bdBool ok = false;
    if (bdStrnlen(jsonData, sizeof(m_jsonData)) < sizeof(m_jsonData))
    {
        bdStrlcpy(m_jsonData, jsonData, sizeof(m_jsonData));
        ok = true;
    }
    else
    {
        bdLogWarn("reward", "JSON data too long");
    }
    return ok;
}
