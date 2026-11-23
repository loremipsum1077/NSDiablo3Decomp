// Original file: X/Common/Callouts.h

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "../Sigma/SigmaTypes.h"

struct Player;
typedef int32 KillCounterType;

namespace Callouts {
struct MassacreDef {  // size 0x10
    uint32 nLevel;  // 0x00
    uint32 nMinKillCount;  // 0x04
    uint32 nMinKillCountMultiplayer;  // 0x08
    float flNephGlobeDropChance;  // 0x0c
};
static_assert(sizeof(MassacreDef) == 0x10, "MassacreDef");

// X/Common/Callouts.cpp:57-200.
MassacreDef GetMassacreDef(const uint32 nKills);
float GetMassacreXPMultiplier(const uint32 nKills);
BOOL IsSharedCounterType(const KillCounterType eType);
uint32 GetMassacreMinCount();
int32 ApplyEnvironmentKillBuff(Player & tPlayer, uint32 nBonus);
void ApplyDestructionBuff(Player & tPlayer, uint32 nCount);
}  // namespace Callouts
