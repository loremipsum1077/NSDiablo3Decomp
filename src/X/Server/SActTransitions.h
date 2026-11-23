// X/Server/SActTransitions.h -- declarations for X/Server/SActTransitions.cpp. The DWARF records
// no types for this header, so only the functions the .cpp defines are declared (signatures from
// the DWARF of their definitions).

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "../Sigma/SigmaTypes.h"

struct Player;

void SActTransitionsInitializeForGame();  // SActTransitions.cpp:62
void SActTransitionsDisposeForGame();  // SActTransitions.cpp:77
void SActTransitionsUpdate();  // SActTransitions.cpp:87
void SActTransitionStart(Player * ptPlayer, BOOL bRecreateGameWithParty);  // SActTransitions.cpp:133
void SActTransitionEnd();  // SActTransitions.cpp:184
void SActTransitionOptInsUpdated();  // SActTransitions.cpp:208
void SActTransitionSetPreventActTransition(BOOL bPreventActTransition);  // SActTransitions.cpp:235
