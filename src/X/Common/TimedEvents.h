// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/TimedEvents.h

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "CommonTypes.h"
#include "SNO/SNOFiles.h"
#include "../Sigma/DataStructures/XGrowableList.h"

struct ActiveEvent {  // size 0x10
    SNO snoTimedEvent;  // 0x00
    uint32 dwStartTime;  // 0x04
    uint32 dwExpirationTime;  // 0x08
    uint32 dwArtificiallyElapsedTime;  // 0x0c
};
static_assert(sizeof(ActiveEvent) == 0x10, "ActiveEvent");

typedef XGrowableList<ActiveEvent, ActiveEvent &> ActiveEventList;

struct TimedEventStartedMessage {  // size 0x18
    RequiredMessageHeader tHeader;  // 0x00
    ActiveEvent tEvent;  // 0x08
};
static_assert(sizeof(TimedEventStartedMessage) == 0x18, "TimedEventStartedMessage");

