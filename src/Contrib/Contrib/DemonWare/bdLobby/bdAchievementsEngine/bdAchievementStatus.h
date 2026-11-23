// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdAchievementsEngine/bdAchievementStatus.h

#pragma once

#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"

class bdStructBufferDeserializer;

class bdAchievementStatus {  // size 0x1
public:
    enum Enum {
        UNKNOWN = 0,
        IN_PROGRESS = 1,
        CLAIMABLE = 2,
        FINISHED = 3,
        INACTIVE = 4,
    };
public:
    static bdBool deserialize(bdStructBufferDeserializer & deserializer, bdUInt32 tag, bdAchievementStatus::Enum & value);
};
static_assert(sizeof(bdAchievementStatus) == 0x1, "bdAchievementStatus");

