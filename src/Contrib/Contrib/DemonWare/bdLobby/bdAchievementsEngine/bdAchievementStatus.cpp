// Original file: Contrib/Contrib/DemonWare/bdLobby/bdAchievementsEngine/bdAchievementStatus.cpp
#include "bdAchievementStatus.h"
#include "../bdLobbyCommon/bdStructBufferDeserializer.h"
#include "../../bdPlatform/bdPlatformLog/bdPlatformLog.h"






bdBool bdAchievementStatus::deserialize(bdStructBufferDeserializer & deserializer, bdUInt32 tag, bdAchievementStatus::Enum & value)
{
    bdUInt16 rawValue = 0;
    bdBool ok = deserializer.readUInt16(tag, rawValue);
    if (ok)
    {
        value = static_cast<bdAchievementStatus::Enum>(rawValue);

        switch (value)
        {
        case UNKNOWN: case IN_PROGRESS: case CLAIMABLE: case FINISHED: case INACTIVE:
            break;
        default:

            bdLogWarn("bdAchievementStatus", "Unknown achievement status %u", value);
            ok = false;
        }
    }
    return ok;
}
