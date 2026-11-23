// Shared waypoint helpers used by the waypoint list and the waypoint map.

#pragma once

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Common/SNO/SNOFiles.h"

namespace UIWaypoints {

// UIWaypointsCommon.cpp:50 -- fills snoWaypointLevelAreas with the level areas the player's
// active quest step points at and writes how many through nLevelAreaCount.
void GetCurrentQuestLevelAreas(int32 & nLevelAreaCount, SNO * snoWaypointLevelAreas);

}  // namespace UIWaypoints
