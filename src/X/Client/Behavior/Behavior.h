// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/Behavior/Behavior.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Common/CommonTypes.h"

struct RActor;
struct ExplosionImpulse;

enum {
    BT_NOTHING = -1,
    BT_INVALID = 0,
    BT_RADIUS_TRIGGER = 1,
    BT_CRITTER = 2,
    BT_WALL_ZOMBIE = 3,
    BT_NUM_TYPES = 4,
};

typedef int32 BehaviorType;

enum {
    BEHAVIOR_STATUS_INVALID = 0,
    BEHAVIOR_STATUS_OK = 1,
    BEHAVIOR_STATUS_DELETE = 2,
    BEHAVIOR_STATUS_NUM_STATUS = 3,
};

typedef int32 BehaviorStatus;

typedef void (*BehaviorInitFunc)(RActor *);

typedef BehaviorStatus (*BehaviorUpdateFunc)(RActor *);

typedef void (*BehaviorDisposeFunc)(RActor *);

typedef void (*BehaviorExplosionFunc)(RActorID, const ExplosionImpulse &);

struct Behavior {  // size 0x30
    BehaviorType eType;  // 0x00
    void * ptBehaviorData;  // 0x08
    BehaviorInitFunc pFuncInit;  // 0x10
    BehaviorUpdateFunc pFuncUpdate;  // 0x18
    BehaviorDisposeFunc pFuncDispose;  // 0x20
    BehaviorExplosionFunc pFuncExplosion;  // 0x28
};
static_assert(sizeof(Behavior) == 0x30, "Behavior");

void BehaviorInit(RActor * ptRActor);
void BehaviorDispose(RActor * ptRActor);
BehaviorStatus BehaviorUpdate(RActor * ptRActor);
Behavior * BehaviorGet(RActor * ptRActor);
void BehaviorApplyExplosion(const RActorID idRActor, const ExplosionImpulse & tExplosion);
