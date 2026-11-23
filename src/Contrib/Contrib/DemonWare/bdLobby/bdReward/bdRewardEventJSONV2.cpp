// Original file: Contrib/Contrib/DemonWare/bdLobby/bdReward/bdRewardEventJSONV2.cpp

#include "bdRewardEventJSONV2.h"
#include "../../bdPlatform/bdPlatformLog/bdPlatformLog.h"
// The other constructors (after the default one) have no copy in the binary.







bdRewardEventJSONV2::bdRewardEventJSONV2()
    : bdRewardEvent(BD_REWARD_EVENT_JSON),
      m_jsonData()
{
}












bdRewardEventJSONV2::~bdRewardEventJSONV2()
{
}

bdBool bdRewardEventJSONV2::serialize(bdByteBuffer & buffer) const
{
    bdBool writeOk = bdRewardEvent::serialize(buffer);
    writeOk = writeOk && buffer.writeString(m_jsonData);
    return writeOk;
}

bdBool bdRewardEventJSONV2::deserialize(bdByteBufferRef buffer)
{
    bdBool ok = bdRewardEvent::deserialize(buffer);
    ok = ok && m_type == BD_REWARD_EVENT_JSON;
    ok = ok && buffer->readString(m_jsonData);
    return ok;
}


bdUInt bdRewardEventJSONV2::sizeOf()
{
    return sizeof(bdRewardEventJSONV2);
}

const bdNChar8 * bdRewardEventJSONV2::getJsonData() const
{
    return m_jsonData.getBuffer();
}
