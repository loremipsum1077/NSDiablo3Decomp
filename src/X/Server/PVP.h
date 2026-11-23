// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Server/PVP.h

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "../Common/CommonTypes.h"

typedef int32 Team;

struct Player;
class ActorCommonData;

void PvPArenaGameStartNewRound(uint32 dwRoundDuration);  // PVP.cpp:745
void PvPTeamDeathmatchGameEnd();  // PVP.cpp:464
uint32 PVPGetDuelingEntranceGUID(const Player & tPlayer, SWorldID idSWorld, BOOL bPickFar);  // PVP.cpp:1708
void PVPApplyFollowerDuelingBuff(ActorCommonData & tACD);  // PVP.cpp:1823
void PVPRemoveFollowerDuelingBuff(ActorCommonData & tACD);  // PVP.cpp:1876
void PVPResetPowersForDueling(Player & tPlayer, BOOL bEnteringDuel, BOOL bIgnoreCosmeticPets);  // PVP.cpp:1529
void PVPDuelingHandlePlayerDelete(ACDID idACDPlayer);  // PVP.cpp:1898

// No header declaration recovered; owning-header placement inferred from PVP.cpp:1853.
void PVPRemoveDuelingBuff(Player & tPlayer);
