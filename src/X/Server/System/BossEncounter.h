#pragma once

// Reconstructed companion declarations from BossEncounter.cpp DWARF.
// No original BossEncounter.h declaration is present in the available DWARF.
namespace BossEncounterManager {
void BossEncounterFlagEncounterComplete();  // BossEncounter.cpp:1579
void BossEncounterTriggerCleanup();  // BossEncounter.cpp:1595
}

#include "../../Sigma/SigmaTypes.h"
struct Player;
namespace BossEncounterManager {
void PlayerRemoved(Player * ptPlayer, BOOL bPlayerWasDeleted);  // BossEncounter.cpp:1395
}

struct ActorIDArray;
namespace BossEncounterManager {
ActorIDArray * BossEncounterGetPlayers(); // BossEncounter.cpp:1551
}
