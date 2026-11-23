#include "Contrib/Contrib/DemonWare/bdLobby/bdReward/bdRewardEvent.h"
#include "Contrib/Contrib/DemonWare/bdPlatform/bdPlatformLog/bdPlatformLog.h"
#include "Contrib/Contrib/DemonWare/bdCore/bdContainers/bdByteBuffer.h"
// Blank lines keep the original line numbers










bdRewardEvent::bdRewardEvent(bdRewardEvent::bdRewardEventTypes type)
    : m_type(type)
{
}

bdRewardEvent::~bdRewardEvent()
{
}

bdBool bdRewardEvent::serialize(bdByteBuffer & buffer) const
{
    return buffer.writeInt32(m_type);
}

bdBool bdRewardEvent::deserialize(bdByteBufferRef buffer)
{
    bdInt32 type = 0;
    bdBool ok = buffer->readInt32(type);
    m_type = static_cast<bdRewardEvent::bdRewardEventTypes>(type);
    return ok;
}
