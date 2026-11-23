// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/ActTransitions.h

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "CommonTypes.h"
#include "../Sigma/SigmaTypes.h"

struct ActTransitionStartedMessage {  // size 0x14
    RequiredMessageHeader tHeader;  // 0x00
    uint32 dwActTransitionTime;  // 0x08
    BOOL bRecreateGameWithParty;  // 0x0c
    int32 nPlayerIndex;  // 0x10
};
static_assert(sizeof(ActTransitionStartedMessage) == 0x14, "ActTransitionStartedMessage");

// The act-transition sentinel (DWARF: ActTransitions.h:15, const_value 4294967295).
static const uint32 NO_ACT_TRANSITION_TIME = 0xFFFFFFFF;  // line 15

// Variables defined in this file:
//   line    15  static const uint32 NO_ACT_TRANSITION_TIME
