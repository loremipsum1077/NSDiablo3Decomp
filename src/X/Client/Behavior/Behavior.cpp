// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/Behavior/Behavior.cpp

#include "Behavior.h"
#include "../RActors/RActors.h"

#include "../../Common/Definitions/ActorDefinition.h"
#include "../../Common/TagMaps/ActorTags.h"
#include "../../Common/TagMaps/Tags.h"
#include "../CGame.h"
#include "../Explosions.h"
#include "../../Sigma/ErrorManager/ErrorManager.h"
#include "../../Sigma/Memory/SigmaMemory.h"

// BehaviorInit installs one of these callbacks. The critter ones are defined in Critter.cpp
// and the radius-trigger ones in RadiusTrigger.cpp; neither header is adopted yet, so they
// are declared here.
void BehaviorCritterInit(RActor * ptRActor);
BehaviorStatus BehaviorCritterUpdate(RActor * ptRActor);
void BehaviorCritterDispose(RActor * ptRActor);
void BehaviorCritterExplosion(const RActorID idRActor, const ExplosionImpulse & tExplosion);
void BehaviorRadiusTriggerInit(RActor * ptRActor);
BehaviorStatus BehaviorRadiusTriggerUpdate(RActor * ptRActor);
void BehaviorRadiusTriggerDispose(RActor * ptRActor);

static void sBehaviorSet(RActor * ptRActor, const BehaviorType eBehaviorType)
{
    Behavior * ptBehavior = (Behavior *)SigmaMemoryNew(sizeof(Behavior), 4, NULL, TRUE);

    ptBehavior->eType = eBehaviorType;
    ptBehavior->ptBehaviorData = NULL;
    ptBehavior->pFuncInit = NULL;
    ptBehavior->pFuncUpdate = NULL;
    ptBehavior->pFuncDispose = NULL;
    ptBehavior->pFuncExplosion = NULL;

    switch (eBehaviorType)
    {
    case BT_RADIUS_TRIGGER:
        ptBehavior->pFuncInit = BehaviorRadiusTriggerInit;
        ptBehavior->pFuncUpdate = BehaviorRadiusTriggerUpdate;
        ptBehavior->pFuncDispose = BehaviorRadiusTriggerDispose;
        break;
    case BT_CRITTER:
        ptBehavior->pFuncInit = BehaviorCritterInit;
        ptBehavior->pFuncUpdate = BehaviorCritterUpdate;
        ptBehavior->pFuncDispose = BehaviorCritterDispose;
        ptBehavior->pFuncExplosion = BehaviorCritterExplosion;
        break;
    case BT_WALL_ZOMBIE:
        break;
    // Any other behavior type is corrupt data. BehaviorInit filters BT_NOTHING before it gets
    // here, so only BT_INVALID or an out-of-range value reaches this arm.
    default:
        // The original diagnostic records Behavior.cpp:75 (0x4b in the original call).
        DisplayInternalError(NULL, "", 75, ERROR_INTERNAL_FATAL);
        ExitProgram(ERROR_INTERNAL_FATAL);
        return;
    }

    ptRActor->ptBehavior = ptBehavior;

    if (ptBehavior->pFuncInit)
        ptBehavior->pFuncInit(ptRActor);
}

void BehaviorInit(RActor * ptRActor)
{
    BehaviorType eBehaviorType;

    ScopedActorDefinitionLockR ptDefActor(ptRActor->snoActor);
    const CTagMap * hTagMap = ptDefActor->hTagMap;

    if (TagMapTagExists(hTagMap, TAG_ACTOR_BEHAVIOR_TYPE))
    {
        eBehaviorType = TagMapGetInt(hTagMap, TAG_ACTOR_BEHAVIOR_TYPE);

        if (eBehaviorType != BT_NOTHING)
        {
            sBehaviorSet(ptRActor, eBehaviorType);
        }
    }
}

void BehaviorDispose(RActor * ptRActor)
{
    const Behavior * ptBehavior = BehaviorGet(ptRActor);

    if (ptBehavior)
    {
        if (ptBehavior->pFuncDispose)
        {
            ptBehavior->pFuncDispose(ptRActor);
        }

        SigmaMemoryFree(ptRActor->ptBehavior, NULL);
        ptRActor->ptBehavior = NULL;
    }
}

BehaviorStatus BehaviorUpdate(RActor * ptRActor)
{
    const Behavior * ptBehavior = BehaviorGet(ptRActor);

    if (ptBehavior)
    {
        if (ptBehavior->pFuncUpdate)
        {
            return ptBehavior->pFuncUpdate(ptRActor);
        }
    }

    return BEHAVIOR_STATUS_OK;
}

Behavior * BehaviorGet(RActor * ptRActor)
{
    return ptRActor->ptBehavior;
}

void BehaviorApplyExplosion(const RActorID idRActor, const ExplosionImpulse & tExplosion)
{
    const RActor * ptRActor = RActorTryToGet(idRActor);

    if (ptRActor && ptRActor->ptBehavior && ptRActor->ptBehavior->pFuncExplosion)
    {
        ptRActor->ptBehavior->pFuncExplosion(idRActor, tExplosion);
    }
}
