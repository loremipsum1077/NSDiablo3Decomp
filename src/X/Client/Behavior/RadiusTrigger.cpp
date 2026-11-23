// Reconstructed from the original RadiusTrigger.cpp DWARF and AArch64 code.
#include "Behavior.h"
#include "../CGame.h"
#include "../RActors/RActors.h"
#include "../../Common/Definitions/ActorDefinition.h"
#include "../../Common/TagMaps/ActorTags.h"
#include "../../Common/TagMaps/Tags.h"
#include "../../Common/WorldPlace.h"
#include "../../Sigma/DataStructures/DataArraysTemplated.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/MemoryPools.h"

// Original declarations: RActors.cpp:6737 and :10707, Player.cpp:3109.
WorldPlace RActorGetWorldPlace(const RActor * ptRActor);
void RActorUnlinkFromMarker(RActor * ptRActor);
void PlayersFindInRadius(const WorldPlace & tPlace, const WorldDistance wdRadius,
                         XPooledDataIDList & tListPlayerACDs);

static const WorldDistance DEFAULT_RADIUS = 15.0f;
static const uint32 DEFAULT_SCAN_INTERVAL = 60;

struct RadiusTriggerBehaviorData {
    WorldDistance wdRadius;
    float flNextScanTime;
    BOOL bDelete;
};
static_assert(sizeof(RadiusTriggerBehaviorData) == 0x0c, "RadiusTriggerBehaviorData");

static void sRadiusTriggerDataInit(RadiusTriggerBehaviorData & tRadiusTriggerData, RActor * ptRActor)
{
    tRadiusTriggerData.wdRadius = DEFAULT_RADIUS;
    tRadiusTriggerData.flNextScanTime = 0.0f;
    tRadiusTriggerData.bDelete = FALSE;

    ScopedActorDefinitionLockR ptDefActor(ptRActor->snoActor);
    const CTagMap * hTagMap = ptDefActor->hTagMap;
    if (TagMapTagExists(hTagMap, TAG_ACTOR_BEHAVIOR_RADIUS_TRIGGER_RADIUS)) {
        const WorldDistance wdRadius = TagMapGetFloat(hTagMap, TAG_ACTOR_BEHAVIOR_RADIUS_TRIGGER_RADIUS);
        tRadiusTriggerData.wdRadius = wdRadius;
    }
}

static RadiusTriggerBehaviorData & sRadiusTriggerDataGet(RActor * ptRActor)
{
    Behavior * ptBehavior = BehaviorGet(ptRActor);
    return *(RadiusTriggerBehaviorData *)ptBehavior->ptBehaviorData;
}

void BehaviorRadiusTriggerInit(RActor * ptRActor)
{
    Behavior * ptBehavior = BehaviorGet(ptRActor);
    RadiusTriggerBehaviorData * ptRadiusTriggerData =
        (RadiusTriggerBehaviorData *)SigmaMemoryNew(sizeof(RadiusTriggerBehaviorData), 4, NULL, TRUE);
    sRadiusTriggerDataInit(*ptRadiusTriggerData, ptRActor);
    ptBehavior->ptBehaviorData = ptRadiusTriggerData;
}

void BehaviorRadiusTriggerDispose(RActor * ptRActor)
{
    Behavior * ptBehavior = BehaviorGet(ptRActor);
    SigmaMemoryFree(ptBehavior->ptBehaviorData, NULL);
    ptBehavior->ptBehaviorData = NULL;
}

static uint32 sRadiusTriggerGetScanDelay(RActor * ptRActor)
{
    uint32 dwScanDelay = DEFAULT_SCAN_INTERVAL;
    ScopedActorDefinitionLockR ptDefActor(ptRActor->snoActor);
    const CTagMap * hTagMap = ptDefActor->hTagMap;
    if (TagMapTagExists(hTagMap, TAG_ACTOR_BEHAVIOR_RADIUS_TRIGGER_SCAN_INTERVAL))
        dwScanDelay = TagMapGetInt(hTagMap, TAG_ACTOR_BEHAVIOR_RADIUS_TRIGGER_SCAN_INTERVAL);
    if (TagMapTagExists(hTagMap, TAG_ACTOR_BEHAVIOR_RADIUS_TRIGGER_SCAN_INTERVAL_DELTA)) {
        const uint32 dwDelta = TagMapGetInt(hTagMap, TAG_ACTOR_BEHAVIOR_RADIUS_TRIGGER_SCAN_INTERVAL_DELTA);
        dwScanDelay += ClientRandRangeInt(0, dwDelta);
    }
    return dwScanDelay;
}

static BOOL sRadiusTriggerIsOneShot(RActor * ptRActor)
{
    BOOL bOneShot = FALSE;
    ScopedActorDefinitionLockR ptDefActor(ptRActor->snoActor);
    const CTagMap * hTagMap = ptDefActor->hTagMap;
    if (TagMapTagExists(hTagMap, TAG_ACTOR_BEHAVIOR_RADIUS_TRIGGER_ONE_SHOT))
        bOneShot = TagMapGetInt(hTagMap, TAG_ACTOR_BEHAVIOR_RADIUS_TRIGGER_ONE_SHOT);
    return bOneShot;
}

BehaviorStatus BehaviorRadiusTriggerUpdate(RActor * ptRActor)
{
    RadiusTriggerBehaviorData & tRadiusTriggerData = sRadiusTriggerDataGet(ptRActor);
    const float flNow = CGameGetClientTime();
    if (tRadiusTriggerData.bDelete == TRUE) {
        RActorUnlinkFromMarker(ptRActor);
        return BEHAVIOR_STATUS_DELETE;
    }

    if (flNow >= tRadiusTriggerData.flNextScanTime) {
        XPooledDataIDList tListPlayerACDs(MemoryPoolsGetDATAIDListPool());
        PlayersFindInRadius(RActorGetWorldPlace(ptRActor), tRadiusTriggerData.wdRadius, tListPlayerACDs);
        const BOOL bTargetsFound = !tListPlayerACDs.IsEmpty();
        const float flScanDelay = (float)sRadiusTriggerGetScanDelay(ptRActor) * (1.0f / 60.0f);
        tRadiusTriggerData.flNextScanTime = flNow + flScanDelay;
        if (bTargetsFound && sRadiusTriggerIsOneShot(ptRActor))
            tRadiusTriggerData.bDelete = TRUE;
    }
    return BEHAVIOR_STATUS_OK;
}
