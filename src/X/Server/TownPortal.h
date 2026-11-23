// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Server/TownPortal.h

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"

typedef int32 PlayerWarpReason;

struct Player;
void TownPortalsDeleteForPlayer(Player & tPlayer);  // TownPortal.cpp:450
