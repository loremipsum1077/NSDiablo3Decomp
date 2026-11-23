// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Server/STalk.h

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "../Common/CommonTypes.h"

class ActorCommonData;
struct Player;

typedef int32 NPCInteractionType;

void STalkInitializeForGame();  // STalk.cpp:30
void STalkDisposeForGame();  // STalk.cpp:43
BOOL STalkIsTalking(const ACDID idACDPlayer);  // STalk.cpp:56
void STalkSetTalkingTo(ActorCommonData & tACDPlayer, ActorCommonData & tACDTalkingTo);  // STalk.cpp:73
void STalkResetTalkingTo(Player & tPlayer);  // STalk.cpp:94
void STalkDoneTalking(const ACDID idACDPlayer);  // STalk.cpp:121
void STalkForceDoneTalking(const ACDID idACDPlayer);  // STalk.cpp:132
void STalkForcePlayersDoneTalkingToNPC(const ACDID idACDNPC);  // STalk.cpp:158
