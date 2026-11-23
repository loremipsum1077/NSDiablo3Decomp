// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Server/AI/AIStates.h
// Subset.

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Common/CommonTypes.h"
#include "../../Common/AI/ActorAIStatesSwig.h"

struct Power;
class ActorCommonData;

void AIStateActivateCooldown(Power & tPower);  // AIStates.cpp:6281

// AIStates.cpp: original definition used by BrainComponents.cpp:3539.
AIStateType AIStateGetCurrState(ActorID idActor);

BOOL AIStateSetState(ActorID idActor, AIStateType eStateType, BOOL bForceAnimSet);  // AIStates.cpp:5501
BOOL AIStateSetStateData(ActorID idActor, AIBehaviorType eBehaviorType);  // AIStates.cpp:5624
void AIStateActivateCooldown(ActorID idActor, uint32 uAICoolDownKey, uint32 uAICoolDownTicks, ActorCommonData * ptACD);  // AIStates.cpp:6294

struct BrainAttackSettings;
BOOL AIStateChooseSkill(ActorID idActor, BrainAttackSettings & tAttackSettings);  // AIStates.cpp:4915

BOOL AIStateHasExistingMerge(ActorID idActor, ACDID idACDSourceIgnore);  // AIStates.cpp:6233
