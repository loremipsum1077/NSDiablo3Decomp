// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/PortalCommon.h
// Subset: PortalResolveMethod and PortalSpecifierMessage.

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "CommonTypes.h"
#include "Definitions/ActDefinition.h"

enum {
    PRM_SPECIFIC_WORLD = 0,
    PRM_CURRENT_WORLD = 1,
    PRM_PREV_WORLD = 2,
    PRM_NEXT_WORLD = 3,
    PRM_SAVE_POINT_PORTAL = 4,
    PRM_BOSS_ENCOUNTER = 5,
    PRM_PAGE_OF_FATE = 6,
    PRM_DYNAMIC_RETURN_LOCATION = 7,
    PRM_INVALID = -1,
};

typedef int32 PortalResolveMethod;

struct PortalSpecifierMessage {  // size 0x18
    RequiredMessageHeader tHeader;  // 0x00
    ACDNetworkName ann;  // 0x08
    ResolvedPortalDestination tResolvedDest;  // 0x0c
};
static_assert(sizeof(PortalSpecifierMessage) == 0x18, "PortalSpecifierMessage");
