// X/Server/System/EncounterTypes.cpp
//
// 2.6.2 translation unit. Owns EncounterState's constructor, destructor and
// ResetState. Reconstructed from the Ghidra decompilation of this file and the
// DWARF lines/constants recorded for it:
//   line    17  EncounterState::EncounterState()  [0x7100960fa0]
//   line    24  EncounterState::~EncounterState() [0x7100961030, +1 more]
//   line    29  void EncounterState::ResetState() [0x71009610d0]
#include "X/Server/System/EncounterTypes.h"  // EncounterState
#include "X/Common/Player.h"  // PlayerGetFirstInGame, PlayerGetNextInGame
#include "X/Server/Timers.h"  // TimerExists, TimerUnregister, TIMERID_INVALID

EncounterState::EncounterState()
{
    idTickTimer = TIMERID_INVALID;
    snoLastWaypointLevelArea = -1;
    ResetState();
}

EncounterState::~EncounterState()
{
    ResetState();
}

void EncounterState::ResetState()
{
    if (TimerExists(idTickTimer))
    {
        TimerUnregister(idTickTimer);
    }

    dwFlags = 0;
    nStartPromptsOut = 0;
    nDeclinedInvites = 0;
    idTickTimer = TIMERID_INVALID;
    snoLastWaypointLevelArea = -1;

    for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer != NULL; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {
        ClearFlags(ptPlayer);
    }
}
