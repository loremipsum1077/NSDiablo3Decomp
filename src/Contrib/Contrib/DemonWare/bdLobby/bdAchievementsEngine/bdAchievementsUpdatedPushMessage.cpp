// Original file: Contrib/Contrib/DemonWare/bdLobby/bdAchievementsEngine/bdAchievementsUpdatedPushMessage.cpp
#include "bdAchievementsUpdatedPushMessage.h"
#include "bdAchievementState.h"
#include "../bdLobbyCommon/bdStructBufferDeserializer.h"








bdAchievementsUpdatedPushMessage::bdAchievementsUpdatedPushMessage()
    : m_states()
{
}
// LINKER-REMOVED: DWARF cu_07ad04dd: concrete C1/C2 DIEs at line 18 (low_pc 0, C1 0x68, C2 0x58 bytes); bdArray(0) inlined at line 19, pushBack not inlined; body inferred, no surviving copy found.
bdAchievementsUpdatedPushMessage::bdAchievementsUpdatedPushMessage(const bdAchievementState & state)
    : m_states()
{
    m_states.pushBack(state);
}























bdBool bdAchievementsUpdatedPushMessage::serialize(bdStructBufferSerializer &) const
{
    return false;
}


bdBool bdAchievementsUpdatedPushMessage::deserialize(bdStructBufferDeserializer & deserializer)
{
    bdBool ok = deserializer.readObjectArray(1, m_states);
    return ok;
}
