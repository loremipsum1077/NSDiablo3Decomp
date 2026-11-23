// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Server/ActorScript.h
// Subset: the boost options and the two boost entry points.

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "../Sigma/SigmaTypes.h"
#include "../Sigma/Unicode.h"

class ActorCommonData;

enum PlayerBoostOptionBits {
    PLAYER_BOOST_PICKUP_ITEMS_BIT = 0,
    PLAYER_BOOST_NOTIFY_CLIENT_BIT = 1,
    PLAYER_BOOST_GIVE_WAYPOINTS_BIT = 2,
    PLAYER_BOOST_REMOVE_EQUIPMENT_FIRST_BIT = 3,
    PLAYER_BOOST_AUTO_ALLOC_ATTRIBUTES = 4,
    PLAYER_BOOST_SHOW_LEVEL_UP_UI_BIT = 5,
};

void ActorBoostPvP(ActorCommonData & tACDPlayer, const int32 nLevel, LPCSTR szTemplate, LPCSTR szTemplateParam, const uint32 dwPlayerBoostOptionBits);  // ActorScript.cpp:78
void ActorBoostToLevel(ActorCommonData & tACDPlayer, const int32 nLevel, LPCSTR szTemplate, LPCSTR szTemplateParam, const uint32 dwPlayerBoostOptionBits);  // ActorScript.cpp:148
