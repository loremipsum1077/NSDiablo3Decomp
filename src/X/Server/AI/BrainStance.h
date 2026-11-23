// Adopted subset of X/Server/AI/BrainStance.h (DWARF line 58).
#pragma once
#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Common/CommonTypes.h"

enum EStanceType {
    STANCE_IDLE = 0,
    STANCE_GETHIT = 1,
    STANCE_KNOCK_BACK = 2,
    STANCE_STUNNED = 3,
    STANCE_SKILL = 4,
    STANCE_DYING = 5,
    STANCE_DEAD = 6,
    STANCE_WALK_WARM_UP = 7,
    STANCE_WALKING = 8,
    STANCE_WALK_COOL_DOWN = 9,
    STANCE_CHANGING = 10,
    STANCE_PLAY_ANIMATION = 11,
    STANCE_PLAY_INTERRUPTABLE_ANIMATION = 12,
    STANCE_FEARED = 13,
    STANCE_ATTACHED = 14,
    STANCE_FROZEN = 15,
    STANCE_FORCE_GRIPPED = 16,
};

enum EBrainPowerResult {
    BRAIN_POWER_START = 0,
    BRAIN_POWER_RETARGET = 1,
    BRAIN_POWER_GET_CLOSER = 2,
    BRAIN_POWER_QUEUE_AND_STAND = 3,
    BRAIN_POWER_CANT_INTERRUPT = 4,
    BRAIN_POWER_FAIL = 5,
    BRAIN_POWER_CLOSING = 6,
    BRAIN_POWER_NO_LINE_OF_SIGHT = 7,
    BRAIN_POWER_IN_UNCOMPLETED_MOVEMENT = 8,
};

EStanceType BrainGetStanceType(ActorID idActor);  // BrainStance.cpp:2369
void BrainStanceSetIdleAnimation(ActorID idActor, uint32 dwIdleAnimationTag);  // BrainStance.cpp:2589
typedef int32 PowerID;  // X/Common/Powers/PowersStruct.h
void BrainUpCallOnPowerStop(ActorID idActor, PowerID idPower, BOOL bAllowAIUpdate);  // BrainStance.cpp:2191

enum {
    TRANSLATE_TYPE_PLAYER = 0,
    TRANSLATE_TYPE_PLAYER_CLOSING = 1,
    TRANSLATE_TYPE_WALKING = 2,
    TRANSLATE_TYPE_CLOSING = 3,
    TRANSLATE_TYPE_CHEAP = 4,
    TRANSLATE_TYPE_STRAFE = 5,
    TRANSLATE_TYPE_QUEST = 6,
    TRANSLATE_TYPE_CHARGE = 7,
    TRANSLATE_TYPE_BACKPEDAL = 8,
    TRANSLATE_TYPE_COUNT = 9,
};
typedef int32 TranslateType;

class WorldPlace;
// BrainStance.cpp:835; argument order verified against original DWARF.
void BrainStanceDoTranslate(ActorID idActor, const WorldPlace & placeGotoPoint,
                           WorldDistance wdSpeed, float flAccel, TranslateType nTranslateType,
                           uint32 dwAnimTag, WorldDistance wdMaxRangeFromDest,
                           LPCSTR sTraceString, ACDID idACDTarget, SNO snoPowerPassability);
void BrainStanceForceUpdate(ActorID idActor);  // BrainStance.cpp:1334
