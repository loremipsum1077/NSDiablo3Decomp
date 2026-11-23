// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Server/AI/PlayerBrain.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"

typedef int32 SkillButton;


#include "../../Common/CommonTypes.h"

class ActorCommonData;
class Target;
struct InventoryLocation;
struct PreplayedAnimData;

// X/Server/AI/PlayerBrain.cpp:226 and :694
void PlayerBrainTarget(const ACDID idACDPlayer, const Target & tTarget, const SNO snoPower, const ACDID idACDItemBeingUsed, const PreplayedAnimData & tPreplayedAnimData, const int32 nComboLevel, const uint32 dwTargetMessageFlags);
void PlayerBrainTryQueuedPower(ACDID idACDPlayer);
BOOL PlayerBrainTryInventoryStackItem(ActorCommonData & tACDPlayer, ActorCommonData & tACDItem, const InventoryLocation & tInvLocDest);
