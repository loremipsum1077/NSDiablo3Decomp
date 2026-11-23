// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Server/Powers/PowerScript.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"

typedef int32 PickupAction;

typedef int32 PetType;

typedef int32 CrowdControlType;

typedef int32 BuffRemovalReason;

typedef int32 EquipmentChangeReason;

typedef int32 LootContext;


struct Power;
typedef int32 PowerMessage;

void ScriptedAnimationPowerMessageHandler(Power * ptPower, PowerMessage eMessage, void * pParam);
void ScriptedNonAnimationPowerMessageHandler(Power * ptPower, PowerMessage eMessage, void * pParam);
void ScriptedSpecialWalkPowerMessageHandler(Power * ptPower, PowerMessage eMessage, void * pParam);
void ScriptedLoopingAnimationPowerMessageHandler(Power * ptPower, PowerMessage eMessage, void * pParam);
