// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Server/System/EncounterTypes.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Common/SNO/SNOFiles.h"

struct Player;

typedef int32 TimerID;

enum {
    ESF_ACCEPTED = 0,
    ESF_PENDING_ACCEPT = 1,
    ESF_DECLINED = 2,
    ESF_FORMER_PARTICIPANT = 3,
    ESF_ACTIVE = 4,
    ESF_INSTIGATOR = 5,
    ESF_CONTINUE_WITHOUT = 6,
    ESF_EMPOWERED = 7,
};

enum {
    FLAG_START_QUEUED = 0,
    FLAG_START_COUNTDOWN_ACTIVE = 1,
    FLAG_INVITE_PHASE = 2,
    FLAG_PROMPT_CONTINUE_WITHOUT = 3,
    FLAG_IN_PROGRESS = 4,
    FLAG_COMPLETING = 5,
    FLAG_ALL_DEAD = 6,
    NUM_FLAGS = 7,
    MAX_FLAG = 20,
};

class EncounterState {  // size 0x18
    // vtable pointer
public:
    EncounterState();
    virtual ~EncounterState();  // vtable slot 0
    virtual void ClearFlags(Player *) = 0;  // vtable slot 2
    virtual void ResetState();  // vtable slot 3
    uint32 dwFlags : 7;  // 0x08
    uint32 nStartPromptsOut : 8;  // 0x08
    uint32 nDeclinedInvites : 4;  // 0x08
    TimerID idTickTimer;  // 0x0c
    SNO snoLastWaypointLevelArea;  // 0x10
};
static_assert(sizeof(EncounterState) == 0x18, "EncounterState");

