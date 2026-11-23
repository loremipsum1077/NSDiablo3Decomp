// Original file: Contrib/Contrib/DemonWare/bdLobby/bdAchievementsEngine/bdAchievementState.cpp
#include "bdAchievementState.h"
#include "../bdLobbyCommon/bdStructBufferDeserializer.h"
#include "../bdLobbyCommon/bdStructBufferDeserializer.inl"








bdAchievementState::bdAchievementState()
    : m_name()
    , m_kind(0)
    , m_progress(0)
    , m_progressTarget(0)
    , m_status(bdAchievementStatus::IN_PROGRESS)
    , m_completionTimestamp(0)
    , m_activationTimestamp(0)
    , m_expirationTimestamp(0)
    , m_completionCount(0)
{
}

bdBool bdAchievementState::serialize(bdStructBufferSerializer &) const
{
    return false;
}

bdBool bdAchievementState::deserialize(bdStructBufferDeserializer & deserializer)
{
    bdBool ok = deserializer.readString(1, m_name);
    ok = ok && deserializer.readUInt16(2, m_kind);
    ok = ok && deserializer.readUInt32(3, m_progress);
    ok = ok && deserializer.readUInt32(4, m_progressTarget);
    ok = ok && bdAchievementStatus::deserialize(deserializer, 5, m_status);
    ok = ok && deserializer.readUInt64(6, m_completionTimestamp);
    ok = ok && deserializer.readUInt64(7, m_activationTimestamp);
    ok = ok && deserializer.readUInt64(8, m_expirationTimestamp);
    ok = ok && deserializer.readUInt32(9, m_completionCount);
    return ok;
}
