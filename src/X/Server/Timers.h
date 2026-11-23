// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Server/Timers.h
// Subset: the timer types and TimerRegister.

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "../Sigma/DataStructures/DataArraysTemplated.h"

enum {
    TIMERID_INVALID = -1,
};

typedef int32 TimerID;

typedef void (*TimerFunc)(TimerID, DataID, void *);

enum TimerOwnerType {
    TIMER_OWNER_NONE = 0,
    TIMER_OWNER_POWER = 1,
    TIMER_OWNER_BUFF = 2,
    TIMER_OWNER_ACTOR = 3,
    TIMER_OWNER_ACD = 4,
};

TimerID TimerRegister(DataID idOwner, TimerOwnerType eOwnerType, uint32 dwDelay, void * lpData,
                      TimerFunc pfunc);  // Timers.cpp:170
BOOL TimerExists(TimerID idTimer);  // Timers.cpp:252
void TimerUnregister(TimerID idTimer);  // Timers.cpp:276
