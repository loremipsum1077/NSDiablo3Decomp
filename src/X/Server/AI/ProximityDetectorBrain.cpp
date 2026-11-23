// Original file: X/Server/Ai/ProximityDetectorBrain.cpp
#include "../Actors/Actors.h"
#include "../Actors/ActorEffect.h"
#include "./ActorAI.h"
#include "./BrainComponents.h"
#include "./ProximityDetectorBrain.h"
#include "../SGame.h"
#include "../SGameManager.h"
#include "../Powers/Powers.h"
#include "../LuaScript/ScriptEvent.h"
#include "../../Common/ActorCommonData.h"
#include "../../Common/Actors/ActorEffectCommon.h"
#include "../../Common/Attributes/FastAttribConstantsSwig.h"
#include "../../Common/Attributes/FastAttributeStructs.h"
#include "../../Common/Definitions/ActorDefinition.h"
#include "../../Common/Definitions/MarkerSetDefinition.h"
#include "../../Common/GameCommonData.h"
#include "../../Common/Gizmo.h"
#include "../../Common/GizmoSwig.h"
#include "../../Common/GlobalSNO.h"
#include "../../Common/InactiveMarker.h"
#include "../../Common/MarkerSet.h"
#include "../../Common/Marker/Marker.h"
#include "../../Common/Player.h"
#include "../../Common/SNO/SNOFiles.h"
#include "../../Common/TagMaps/ActorTags.h"
#include "../../Common/TagMaps/MarkerTags.h"
#include "../../Common/TagMaps/Tags.h"
#include "../../Common/Target.h"
#include "../../Common/WorldPlace.h"
#include "../../Sigma/Math/Intersect.h"
#include "../../Sigma/Math/MathGeneral.h"
#include "../../Sigma/Math/Ray.h"
#include "../../Sigma/Math/Rect.h"
#include "../../Sigma/Math/Sphere.h"
#include "../../Sigma/MemoryPools.h"
#include "../../Sigma/Random.h"

// Declared here until their headers are in src: ActorCollision.h, ActorAI.cpp, Player.cpp, ScriptEvent.cpp, SGizmo.cpp, SQuests.cpp, Tutorial.cpp.
enum { ACTOR_SPHERE_TREE_FLAGS_ALL = 7 }; void ACDsIntersectSphere(const WorldPlace & tPlace, const WorldDistance wdRadius, const ACDID idACDIgnore, XPooledDataIDList & listACDs, uint32 dwSphereTreeFlags); void ActorAIRegisterProximityDetector(ActorID idActor); void ActorAIUnregisterProximityDetector(ActorID idActor); ProximityDetectorDataID ProximityDetectorGetNewData();
ProximityDetectorBrainData * ProximityDetectorTryGetData(ProximityDetectorDataID idData); void ProximityDetectorDataDelete(ProximityDetectorDataID idData); void PlayersFindInRadius(const WorldPlace & tPlace, const WorldDistance wdRadius, XPooledDataIDList & tListPlayerACDs); void PlayersFindInOBB(SWorldID idSWorld, const OBB & tOBB, XPooledDataIDList & tListPlayerACDs); void ScriptEventOnTriggerPulseActorsInBounds(const XPooledDataIDList & idACDActorsDetected, ACDID idACDTrigger, BOOL & bTriggeredScriptEvent);
void GizmoSetCheckpoint(Player & tPlayer, ActorCommonData & tACDGizmo); void GizmoActivateWaypoint(ActorCommonData & tACDWaypoint, const ACDID idACDDetected); void GizmoOperate(ActorCommonData & tACDGizmo, ActorCommonData * ptACDOperator, BOOL bForceOperation, BOOL bAlreadyConfirmed); void GizmoSetSavePoint(Player & tPlayer, ActorCommonData & tACDGizmo, const int32 nSavepointNum, BOOL bCreatesPortal); BOOL SQuestsOnTrigger(ACDID idACDTriggeringActor, ACDID idACDTrigger);
namespace Tutorial { BOOL PlayerHasSeen(const Player & tPlayer, const SNO snoTutorial); BOOL SendTutorialToClient(SNO snoTutorial, Player * ptPlayer, const char * szUIAnchorName, const ACDID idACDTarget); } static void sTriggerEntered(ActorID idActorDetector, ProximityDetectorBrainData & tProximityDetectorBrainData, const ACDID idACDDetected, const BOOL fPassingThrough, const BOOL fWarpedInside); static void sTriggerExited(ActorID idActorDetector, ProximityDetectorBrainData & tProximityDetectorBrainData, const ACDID idACDDetected, const BOOL fPassingThrough); void ProximityDetectorGetActorsInBoundsList(ActorID idTrigger, XPooledDataIDList & tListDetectedACDs, const ProximityShapeData & tShape, BOOL bPlayersOnly); void ProximityDetectorBrainOnCreate(ActorID idTrigger);



static void sProximityDetectorBrainDataInitialize(ProximityDetectorBrainData & tBrainData)
{

    tBrainData.dwNextPulseTime = 0;
    tBrainData.dwPulseInterval = 0;

    tBrainData.dwFlags = 0;
    tBrainData.dwPulseIntervalDelta = 0; tBrainData.pOnPlayerEnteredCallback = NULL; tBrainData.pOnPlayerExitedCallback = NULL; tBrainData.pOnCountChangedCallback = NULL;



    tBrainData.snoTriggerActor = SNO_NONE;

    tBrainData.listACDs.SetPool(MemoryPoolsGetDATAIDListPool());
}



static ProximityDetectorBrainData * sProximityDetectorGetBrainData(ActorID idActor)
{

    ActorAI & tAI = ActorGetAI(idActor);
    return ProximityDetectorTryGetData(tAI.idProximityData);
}



void ProximityDetectorGetOBB(OBB & obb, const PRTransform & transform, const CTagMap * ptTagMapDef, const CTagMap * ptTagMapMarker)
{




    obb.transform = transform;

    obb.wvExt.x = TagMapGetInheritedValueFloat(ptTagMapDef, ptTagMapMarker, TAG_ACTOR_AI_PROXIMITY_DETECTOR_OBB_X, TAG_MARKER_OBB_RADIUS_X);
    obb.wvExt.y = TagMapGetInheritedValueFloat(ptTagMapDef, ptTagMapMarker, TAG_ACTOR_AI_PROXIMITY_DETECTOR_OBB_Y, TAG_MARKER_OBB_RADIUS_Y);
    obb.wvExt.z = TagMapGetInheritedValueFloat(ptTagMapDef, ptTagMapMarker, TAG_ACTOR_AI_PROXIMITY_DETECTOR_OBB_Z, TAG_MARKER_OBB_RADIUS_Z);
}



void ProximityInitializeShapeData(ActorID idActor, ProximityShapeData & tShape)


{
    Actor * ptActor = ActorGet(idActor);
    ScopedActorDefinitionLockR pdefActor(ptActor->snoActor);
    ActorCommonData & tACD = ACDGet(ptActor->idACD);
    const WorldLocation & tLocation = tACD.GetWorldLocation();

    tShape.eShape = ActorGetInheritedTagMapValueInt(ptActor, TAG_ACTOR_AI_PROXIMITY_DETECTOR_TYPE, TAG_MARKER_PROXIMITY_DETECTOR_TYPE);
    tShape.flRadius = ActorGetInheritedTagMapValueFloat(ptActor, TAG_ACTOR_AI_PROXIMITY_DETECTOR_RADIUS, TAG_MARKER_PROXIMITY_DETECTOR_RADIUS);

    CTagMap * ptTagMapMarker = NULL;
    if (ptActor->tMarkerOwner.snoMarkerSet != SNO_NONE)
    {
        ScopedMarkerSetDefinitionLockR ptMarkerSet(ptActor->tMarkerOwner.snoMarkerSet);

        if (ptActor->tMarkerOwner.nMarkerIndex >= 0 && ptActor->tMarkerOwner.nMarkerIndex < ptMarkerSet->tMarkerSet.GetArraySize())
        {
            const Marker & tMarker = ptMarkerSet->tMarkerSet[ptActor->tMarkerOwner.nMarkerIndex];
            ptTagMapMarker = tMarker.hTagMap;
        }
    }

    ProximityDetectorGetOBB(tShape.wobb, tLocation.transform, pdefActor->hTagMap, ptTagMapMarker);
}



void ProximityDetectorBrainInitialize(const ActorID idActor)

{
    ActorAI & tAI = ActorGetAI(idActor);

    ActorAIRegisterProximityDetector(idActor);


    tAI.idProximityData = ProximityDetectorGetNewData();
    ProximityDetectorBrainData * ptBrainData = ProximityDetectorTryGetData(tAI.idProximityData);
    if (ptBrainData == NULL)
    {
        return;
    }


    sProximityDetectorBrainDataInitialize(*ptBrainData);

    Actor * ptActor = ActorGet(idActor);
    ScopedActorDefinitionLockR pdefActor(ptActor->snoActor);

    ProximityInitializeShapeData(idActor, ptBrainData->tShape);

    ActorCommonData & tACD = ACDGet(ptActor->idACD);

    GizmoType eGizmoType = tACD.GetGizmoType();
    if (eGizmoType != ACTORGIZMO_CHECKPOINT && eGizmoType != ACTORGIZMO_WAYPOINT && eGizmoType != ACTORGIZMO_SAVEPOINT && eGizmoType != ACTORGIZMO_DUNGEON_PORTAL && eGizmoType != ACTORGIZMO_RETURN_PORTAL)
    {
        BOOL bDoEffect;
        BOOL bOneShot;


        if (eGizmoType == ACTORGIZMO_TRIGGER || eGizmoType == ACTORGIZMO_DESTROY_ON_PROXIMITY)
        {
            BOOL fPulse = ActorGetInheritedTagMapValueInt(ptActor, TAG_ACTOR_AI_PROXIMITY_DETECTOR_PULSE, TAG_MARKER_PROXIMITY_DETECTOR_PULSE);
            if (fPulse) { ptBrainData->dwFlags |= (1 << PROX_FLAG_PULSE); } else { ptBrainData->dwFlags &= ~(1 << PROX_FLAG_PULSE); }

            if (fPulse)
            {
                ptBrainData->dwPulseInterval = ActorGetInheritedTagMapValueInt(ptActor, TAG_ACTOR_AI_PROXIMITY_DETECTOR_INTERVAL, TAG_MARKER_PROXIMITY_DETECTOR_INTERVAL);
                ptBrainData->dwPulseIntervalDelta = ActorGetInheritedTagMapValueInt(ptActor, TAG_ACTOR_AI_PROXIMITY_DETECTOR_INTERVAL_DELTA, TAG_MARKER_PROXIMITY_DETECTOR_INTERVAL_DELTA);
            }
        }

        bDoEffect = ActorGetInheritedTagMapValueInt(ptActor, TAG_ACTOR_AI_PROXIMITY_DETECTOR_EFFECT, TAG_MARKER_PROXIMITY_DETECTOR_EFFECT);
        bOneShot = ActorGetInheritedTagMapValueInt(ptActor, TAG_ACTOR_AI_PROXIMITY_DETECTOR_ONE_SHOT, TAG_MARKER_PROXIMITY_DETECTOR_ONE_SHOT);

        if (bDoEffect) { ptBrainData->dwFlags |= (1 << PROX_FLAG_DO_EFFECT); } else { ptBrainData->dwFlags &= ~(1 << PROX_FLAG_DO_EFFECT); }
        if (bOneShot) { ptBrainData->dwFlags |= (1 << PROX_FLAG_ONE_SHOT); } else { ptBrainData->dwFlags &= ~(1 << PROX_FLAG_ONE_SHOT); }
    }

    if (ptActor->tMarkerOwner.snoMarkerSet != SNO_NONE)
    {
        ScopedMarkerSetDefinitionLockR ptMarkerSet(ptActor->tMarkerOwner.snoMarkerSet);
        if (ptActor->tMarkerOwner.nMarkerIndex >= 0 && ptActor->tMarkerOwner.nMarkerIndex < ptMarkerSet->tMarkerSet.GetArraySize())
        {
            const Marker & tMarker = ptMarkerSet->tMarkerSet[ptActor->tMarkerOwner.nMarkerIndex];
            ptBrainData->snoTriggerActor = TagMapGetSNO(tMarker.hTagMap, TAG_MARKER_TRIGGERED_BY_ACTOR_SNO);
        }
    }
}



void ProximityDetectorBrainInitializeForGizmo(const ActorID idActor, float flRadius)


{
    ActorAI & tAI = ActorGetAI(idActor);

    ActorAIRegisterProximityDetector(idActor);


    tAI.idProximityData = ProximityDetectorGetNewData();
    ProximityDetectorBrainData * ptBrainData = ProximityDetectorTryGetData(tAI.idProximityData);
    if (ptBrainData == NULL)
    {
        return;
    }


    sProximityDetectorBrainDataInitialize(*ptBrainData);

    Actor * ptActor = ActorGet(idActor);
    ActorCommonData & tACD = ACDGet(ptActor->idACD);
    const WorldLocation & tLocation = tACD.GetWorldLocation();

    ptBrainData->tShape.eShape = AI_PROXIMITY_SHAPE_SPHERE;
    ptBrainData->tShape.flRadius = flRadius;
    ptBrainData->tShape.wobb.transform = tLocation.transform;
}



void ProximityDetectorBrainInitializeForAutoLootGizmo(const ActorID idActor)
{
    Actor * ptActor = ActorGet(idActor);
    ActorCommonData & tACD = ACDGet(ptActor->idACD);

    float flRadius;
    GizmoGetOperationRadius(tACD, NULL, flRadius);

    ProximityDetectorBrainInitializeForGizmo(idActor, flRadius);
}




void ProximityDetectorBrainDispose(const ActorID idActor)

{
    ProximityDetectorBrainData * ptProxDetBrainData = sProximityDetectorGetBrainData(idActor);
    if (ptProxDetBrainData)
    {
        ptProxDetBrainData->listACDs.RemoveAll();
    }

    ActorAIUnregisterProximityDetector(idActor);

    if (ptProxDetBrainData)
    {
        ActorAI & tAI = ActorGetAI(idActor);
        ProximityDetectorDataDelete(tAI.idProximityData);
    }
}



static uint32 sGetNextPulseDelay(const ActorID idActor)
{

    ProximityDetectorBrainData * ptProxDetBrainData = sProximityDetectorGetBrainData(idActor);
    if (ptProxDetBrainData == NULL) return 0;

    return ServerRandRangeInt(ptProxDetBrainData->dwPulseInterval, ptProxDetBrainData->dwPulseInterval + ptProxDetBrainData->dwPulseIntervalDelta);
}







static void sStopTrigger(ActorID idActor, ProximityDetectorBrainData & tProximityDetectorBrainData)
{









    if (tProximityDetectorBrainData.dwFlags & (1 << PROX_FLAG_DO_EFFECT))
    {
        ActorDeleteSoon(idActor, 15);
    }
    else
    {
        ActorFlagForDeletion(idActor);
    }
}




static void sTriggerPulse(ActorID idActor, ProximityDetectorBrainData & tProximityDetectorBrainData)
{


    Actor * ptActor = ActorGet(idActor);

    ACDID idACDDetector = ptActor->idACD;
    uint32 dwNow = GameGetServerTime();


    tProximityDetectorBrainData.dwNextPulseTime = dwNow + sGetNextPulseDelay(idActor);

    if (tProximityDetectorBrainData.listACDs.Count() > 0)
    {



        BOOL bScriptEventTriggered = FALSE;
        ScriptEventOnTriggerPulseActorsInBounds(tProximityDetectorBrainData.listACDs, idACDDetector, bScriptEventTriggered);
    }
}



static void sDetectorAddACD(const ActorID idActorDetector, ProximityDetectorBrainData & tProximityDetectorBrainData, const ACDID idACD, const BOOL fPassingThrough)
{




    if (tProximityDetectorBrainData.listACDs.Find(idACD) == NULL)
    {

        tProximityDetectorBrainData.listACDs.AddTail_Tracked(idACD);
        if (tProximityDetectorBrainData.pOnCountChangedCallback)
        {
            tProximityDetectorBrainData.pOnCountChangedCallback(idActorDetector, 1, tProximityDetectorBrainData.listACDs.Count(), fPassingThrough);
        }
    }
}




static void sDetectorRemoveACD(const ActorID idActorDetector, ProximityDetectorBrainData & tProximityDetectorBrainData, const ACDID idACD, const BOOL fPassingThrough)
{




    POS pos = tProximityDetectorBrainData.listACDs.Find(idACD);
    if (pos)
    {
        tProximityDetectorBrainData.listACDs.RemoveAt(pos);
        if (tProximityDetectorBrainData.pOnCountChangedCallback)
        {
            tProximityDetectorBrainData.pOnCountChangedCallback(idActorDetector, -1, tProximityDetectorBrainData.listACDs.Count(), fPassingThrough);
        }
    }
}




static void sSetCheckpoint(ActorCommonData & tACDDetector, const ACDID idACDDetected)
{


    if (ACDIsPlayer(idACDDetected))
    {
        Player * ptPlayer = PlayerGetByACD(idACDDetected);
        if (ptPlayer)
        {
            GizmoSetCheckpoint(*ptPlayer, tACDDetector);
        }
    }
}




static void sSetSavePoint(ActorCommonData & tACDDetector, const ACDID idACDDetected, const int32 nSavepointNum, BOOL bCreatesPortal)
{




    if (ACDIsPlayer(idACDDetected))
    {
        Player * ptOriginalPlayer = PlayerGetByACD(idACDDetected);

        if (ptOriginalPlayer)
        {
            GameConnectionID idOriginalGameConnection = ptOriginalPlayer->idGameConnection;
            for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer != NULL; ptPlayer = PlayerGetNextInGame(ptPlayer))
            {
                if (ptPlayer->idGameConnection == idOriginalGameConnection)
                {

                    GizmoSetSavePoint(*ptPlayer, tACDDetector, nSavepointNum, bCreatesPortal);
                }
            }
        }
    }
}



static void sTriggerEntered(ActorID idActorDetector, ProximityDetectorBrainData & tProximityDetectorBrainData, const ACDID idACDDetected, const BOOL fPassingThrough, const BOOL fWarpedInside)
{







    Actor * ptActorDetector = ActorGet(idActorDetector);
    ACDID idACDDetector = ptActorDetector->idACD;
    ActorCommonData & tACDDetector = ACDGet(idACDDetector);
    Player * ptPlayer = PlayerGetByACD(idACDDetected);

    sDetectorAddACD(idActorDetector, tProximityDetectorBrainData, idACDDetected, fPassingThrough);

    if (tProximityDetectorBrainData.pOnPlayerEnteredCallback)
    {
        tProximityDetectorBrainData.pOnPlayerEnteredCallback(idActorDetector, idACDDetected);
    }

    GizmoType eGizmoType = tACDDetector.GetGizmoType();
    if (ptPlayer)
    {
        switch (eGizmoType)
        {
        case ACTORGIZMO_CHECKPOINT:
        case ACTORGIZMO_DUNGEON_PORTAL:
        case ACTORGIZMO_RETURN_PORTAL:
            sSetCheckpoint(tACDDetector, idACDDetected);
            break;
        case ACTORGIZMO_WAYPOINT:
            sSetCheckpoint(tACDDetector, idACDDetected);
            GizmoActivateWaypoint(tACDDetector, idACDDetected);
            break;
        case ACTORGIZMO_SAVEPOINT:
            {
                int32 nSavepointNum = 0;
                BOOL bCreatesPortal = FALSE;
                if (ptActorDetector->tMarkerOwner.snoMarkerSet != SNO_NONE)
                {
                    ScopedMarkerSetDefinitionLockR ptMarkerSet(ptActorDetector->tMarkerOwner.snoMarkerSet);
                    if (ptActorDetector->tMarkerOwner.nMarkerIndex >= 0 && ptActorDetector->tMarkerOwner.nMarkerIndex < ptMarkerSet->tMarkerSet.GetArraySize())
                    {
                        const Marker & tMarker = ptMarkerSet->tMarkerSet[ptActorDetector->tMarkerOwner.nMarkerIndex];
                        nSavepointNum = TagMapGetInt(tMarker.hTagMap, TAG_MARKER_SAVEPOINT_NUMBER);
                        bCreatesPortal = TagMapGetInt(tMarker.hTagMap, TAG_MARKER_SAVEPOINT_DOESNT_CREATE_PORTAL) == 0;
                    }
                }
                sSetSavePoint(tACDDetector, idACDDetected, nSavepointNum, bCreatesPortal);
            }
            break;
        case ACTORGIZMO_CHEST:
        case ACTORGIZMO_LORE_CHEST:
            {

                ActorCommonData * ptACDDetected = ACDTryToGet(idACDDetected);
                GizmoOperate(tACDDetector, ptACDDetected, FALSE, FALSE);
            }
            break;




        case ACTORGIZMO_BANNER:
            {
                SNO snoTutorial = GlobalSNOGet(GLOBALSNO_TUTORIAL_BANNER);
                if (!Tutorial::PlayerHasSeen(*ptPlayer, snoTutorial))
                {
                    int32 nPlayerDest = ActorDefinitionGetTagMapValueInt(TAG_ACTOR_GIZMO_BANNER_PLAYER_INDEX, tACDDetector.snoActor);
                    Player * ptPlayerDest = PlayerGetByPlayerIndex(nPlayerDest);

                    if (ptPlayerDest && PlayerIsInWorld(*ptPlayerDest) && !PlayersAreLocalToEachOther(*ptPlayer, *ptPlayerDest))
                    {
                        Tutorial::SendTutorialToClient(snoTutorial, ptPlayer, NULL, idACDDetector);
                    }
                }
            }
            break;

        case ACTORGIZMO_PORTAL:
            {
                if (fWarpedInside)
                {

                    ActorCommonData & tACDDetected = ACDGet(idACDDetected);
                    float flDistance = WorldPlaceDistance(tACDDetector.GetPlace(), tACDDetected.GetPlace());

                    const float CONSOLE_PORTAL_OPERATION_PADDING = 4.0f;
                    const float MIN_CONSOLE_PORTAL_OPERATIONAL_RADIUS = 4.0f;


                    float flOperateRadius = Math::Max(flDistance - CONSOLE_PORTAL_OPERATION_PADDING, MIN_CONSOLE_PORTAL_OPERATIONAL_RADIUS);
                    tACDDetector.AttributesSetFloat(ATTRIBUTE_GIZMO_OPERATION_RADIUS_OVERRIDE, flOperateRadius);
                }
            }
            break;
        }
    }

    BOOL bScriptEventTriggered = FALSE;
    if (eGizmoType != ACTORGIZMO_DESTROY_ON_PROXIMITY)
    {
        ScriptEventOnActorEnteredTrigger(idACDDetected, idACDDetector, bScriptEventTriggered);
    }
    else if (ptPlayer)
    {
        BrainAPIDoDeath(idActorDetector, ACTORID_INVALID, TRUE, TRUE, TRUE, NULL, NULL);
        tACDDetector.AttributesSetInt(ATTRIBUTE_GIZMO_STATE, 1);
    }

    BOOL bQuestEventTriggered = SQuestsOnTrigger(idACDDetected, idACDDetector);

    if (ptPlayer)
    {

        if (tProximityDetectorBrainData.dwFlags & (1 << PROX_FLAG_DO_EFFECT))
        {
            ACDPlayEffect(idACDDetector, EFFECT_PLAYER_DETECTED, 0, ACDID_INVALID);
        }

        SNO snoPowerEntered; ScopedActorDefinitionLockR pdefActor(tACDDetector.snoActor);
        snoPowerEntered = TagMapGetSNO(pdefActor->hTagMap, TAG_ACTOR_GIZMO_TRIGGER_ENTERED_POWER);
        if (snoPowerEntered != SNO_NONE)
        {
            Target tTarget;
            tTarget.InitializeFromACD(idACDDetector, FALSE, FALSE);
            PowerStart(idACDDetected, snoPowerEntered, tTarget);
        }
    }


    BOOL bStopTrigger = (tProximityDetectorBrainData.dwFlags & (1 << PROX_FLAG_ONE_SHOT)) &&
                        (bScriptEventTriggered || bQuestEventTriggered);
    if (bStopTrigger)
    {
        sStopTrigger(idActorDetector, tProximityDetectorBrainData);
    }
}




static void sTriggerExited(ActorID idActorDetector, ProximityDetectorBrainData & tProximityDetectorBrainData, const ACDID idACDDetected, const BOOL fPassingThrough)
{




    Actor * ptActorDetector = ActorGet(idActorDetector);
    ACDID idACDDetector = ptActorDetector->idACD;
    Player * ptPlayer = PlayerGetByACD(idACDDetected);

    sDetectorRemoveACD(idActorDetector, tProximityDetectorBrainData, idACDDetected, fPassingThrough);

    if (tProximityDetectorBrainData.pOnPlayerExitedCallback)
    {
        tProximityDetectorBrainData.pOnPlayerExitedCallback(idActorDetector, idACDDetected);
    }

    if (ptPlayer)
    {

        ActorCommonData & tACDDetector = ACDGet(idACDDetector);
        GizmoType eGizmoType = tACDDetector.GetGizmoType();

        if (eGizmoType == ACTORGIZMO_PORTAL)
        {


            tACDDetector.AttributesSetFloat(ATTRIBUTE_GIZMO_OPERATION_RADIUS_OVERRIDE, 0.0f);
        }
    }



    BOOL bScriptEventTriggered = FALSE;
    ScriptEventOnActorExitedTrigger(idACDDetected, idACDDetector, bScriptEventTriggered);


    SNO snoPowerExited;
    ScopedActorDefinitionLockR pdefActor(ptActorDetector->snoActor);
    snoPowerExited = TagMapGetSNO(pdefActor->hTagMap, TAG_ACTOR_GIZMO_TRIGGER_EXITED_POWER);
    if (snoPowerExited != SNO_NONE)
    {
        Target tTarget;
        tTarget.InitializeFromACD(idACDDetector, FALSE, FALSE);
        PowerStart(idACDDetected, snoPowerExited, tTarget);
    }


    BOOL bStopTrigger = bScriptEventTriggered && (tProximityDetectorBrainData.dwFlags & (1 << PROX_FLAG_ONE_SHOT));
    if (bStopTrigger)
    {
        sStopTrigger(idActorDetector, tProximityDetectorBrainData);
    }
}




void ProximityDetectorGetActorsInBoundsList(ActorID idTrigger, XPooledDataIDList & tListDetectedACDs, BOOL bPlayersOnly)



{
    ProximityDetectorBrainData * ptProximityDetectorBrainData = sProximityDetectorGetBrainData(idTrigger);
    if (ptProximityDetectorBrainData)
    {
        ProximityDetectorGetActorsInBoundsList(idTrigger, tListDetectedACDs, ptProximityDetectorBrainData->tShape, bPlayersOnly);
    }
}




void ProximityDetectorGetActorsInBoundsList(ActorID idTrigger, XPooledDataIDList & tListDetectedACDs, const ProximityShapeData & tShape, BOOL bPlayersOnly)




{
    Actor * ptActor = ActorGet(idTrigger);
    ACDID idACDDetector = ptActor->idACD;
    ActorCommonData & tACDDetector = ACDGet(idACDDetector);
    const WorldLocation & tWorldLocation = tACDDetector.GetWorldLocation();
    WorldPlace wpCenter = tACDDetector.GetWorldPlaceCenter();


    if (tShape.eShape == AI_PROXIMITY_SHAPE_SPHERE)
    {


        if (!bPlayersOnly)
        {
            ACDsIntersectSphere(wpCenter, tShape.flRadius, ACDID_INVALID, tListDetectedACDs, ACTOR_SPHERE_TREE_FLAGS_ALL);
        }
        else
        {
            POS pos;
            WorldPlace tWorldPlace;
            tWorldPlace.idSWorld = tWorldLocation.idSWorld;
            tWorldPlace.wp = tWorldLocation.transform.wp;
            XPooledDataIDList tListPlayerACDs(MemoryPoolsGetDATAIDListPool());
            PlayersFindInRadius(tWorldPlace, tShape.flRadius, tListPlayerACDs);
            pos = tListPlayerACDs.GetHeadPosition();
            while (pos)
            {
                tListDetectedACDs.AddTail_Tracked(tListPlayerACDs.GetNext(pos));
            }
        }
    }
    else if (tShape.eShape == AI_PROXIMITY_SHAPE_BOX)
    {

        OBB wobb;
        wobb.wvExt = tShape.wobb.wvExt;
        wobb.transform = tWorldLocation.transform;

        if (!bPlayersOnly)
        {
            float flMaxRadius = Math::Max(wobb.wvExt.x, Math::Max(wobb.wvExt.y, wobb.wvExt.z));
            ACDsIntersectSphere(wpCenter, flMaxRadius, ACDID_INVALID, tListDetectedACDs, ACTOR_SPHERE_TREE_FLAGS_ALL);

            POS pos = tListDetectedACDs.GetHeadPosition();
            while (pos)
            {
                POS posCurrent = pos;
                ACDID idACDDetected = (ACDID)tListDetectedACDs.GetNext(pos);
                WorldPlace wpDetected = ACDGet(idACDDetected).GetWorldPlaceCenter();
                Sphere ws;
                ws.wpCenter = wpDetected.wp;
                ws.wdRadius = 0.25f;

                if (!Intersect::SphereAndOBB(ws, tShape.wobb, NULL))
                {
                    tListDetectedACDs.RemoveAt(posCurrent);
                }
            }
        }
        else
        {
            POS pos;
            XPooledDataIDList tListPlayerACDs(MemoryPoolsGetDATAIDListPool());
            PlayersFindInOBB(tWorldLocation.idSWorld, wobb, tListPlayerACDs);
            pos = tListPlayerACDs.GetHeadPosition();
            while (pos)
            {
                tListDetectedACDs.AddTail_Tracked(tListPlayerACDs.GetNext(pos));
            }
        }
    }
}





static void sRemoveInvalidACDsFromList(ActorID idActorDetector, ProximityDetectorBrainData & tProximityDetectorBrainData)
{



    POS pos = tProximityDetectorBrainData.listACDs.GetHeadPosition();
    while (pos)
    {
        POS posCurrent = pos;
        ACDID idACD = (ACDID)tProximityDetectorBrainData.listACDs.GetNext(pos);
        if (ACDTryToGet(idACD) == NULL)
        {
            tProximityDetectorBrainData.listACDs.RemoveAt(posCurrent);
            if (tProximityDetectorBrainData.pOnCountChangedCallback)
            {
                tProximityDetectorBrainData.pOnCountChangedCallback(idActorDetector, -1, tProximityDetectorBrainData.listACDs.Count(), FALSE);
            }
        }
    }
}




void ProximityDetectorBrainUpdate(const ActorID idActor)

{
    ProximityDetectorBrainData * ptProximityDetectorBrainData = sProximityDetectorGetBrainData(idActor);
    if (ptProximityDetectorBrainData)
    {
        sRemoveInvalidACDsFromList(idActor, *ptProximityDetectorBrainData);

        if (ptProximityDetectorBrainData->dwFlags & (1 << PROX_FLAG_PULSE))
        {
            uint32 dwNow = GameGetServerTime();
            if (dwNow >= ptProximityDetectorBrainData->dwNextPulseTime)
            {
                sTriggerPulse(idActor, *ptProximityDetectorBrainData);
            }
        }
    }
}



void ProximityDetectorBrainOnCreate(ActorID idTrigger)

{
    Actor * ptTrigger = ActorGet(idTrigger);
    if (ptTrigger == NULL)
    {
        return;
    }

    ActorCommonData * ptACDTrigger = ACDTryToGet(ptTrigger->idACD);
    if (ptACDTrigger == NULL)
    {
        return;
    }

    if (!ptACDTrigger->IsEnabled())
    {
        return;
    }

    ProximityDetectorBrainData * ptProximityDetectorBrainData = sProximityDetectorGetBrainData(idTrigger);
    if (ptProximityDetectorBrainData == NULL)
    {
        return;
    }



    POS pos;
    XPooledDataIDList listACDs(MemoryPoolsGetDATAIDListPool());
    ProximityDetectorGetActorsInBoundsList(idTrigger, listACDs, ptProximityDetectorBrainData->tShape, TRUE);

    pos = listACDs.GetHeadPosition();
    while (pos)
    {
        ACDID idACDMover = (ACDID)listACDs.GetNext(pos);

        sTriggerEntered(idTrigger, *ptProximityDetectorBrainData, idACDMover, FALSE, TRUE);
    }
}



void ProximityDetectorCheckActivation(ActorID idTrigger, ActorCommonData & tACDMover, const LineSeg2D & segMove)



{
    Actor * ptTrigger = ActorTryToGet(idTrigger);
    if (ptTrigger == NULL)
    {
        return;
    }

    ActorCommonData * ptACDTrigger = ACDTryToGet(ptTrigger->idACD);
    if (ptACDTrigger == NULL)
    {
        return;
    }

    if (!ptACDTrigger->IsEnabled())
    {
        return;
    }

    ProximityDetectorBrainData * ptProximityDetectorBrainData = sProximityDetectorGetBrainData(idTrigger);
    if (ptProximityDetectorBrainData == NULL)
    {
        return;
    }
    ProximityShapeData & tShape = ptProximityDetectorBrainData->tShape;
    const WorldLocation & tTriggerLoc = ptACDTrigger->GetWorldLocation();
    const WorldLocation & tMoverLoc = tACDMover.GetWorldLocation();
    if (tTriggerLoc.idSWorld != tMoverLoc.idSWorld)
    {
        return;
    }

    BOOL fStartInside = FALSE;
    BOOL fEndInside = FALSE;



    if (tShape.eShape == AI_PROXIMITY_SHAPE_SPHERE)
    {
        Vector2D vIntersect2D;
        float flT;
        Circle circleTrigger;
        circleTrigger.Set(tTriggerLoc.transform.wp.x, tTriggerLoc.transform.wp.y, tShape.flRadius);



        if (!Intersect::LineSeg2DAndCircle(segMove, circleTrigger, flT, &vIntersect2D))
        {
            return;
        }

        Vector2D wpEnd;
        segMove.wr.GetEndpoint(segMove.wdLength, wpEnd);

        if (circleTrigger.ContainsPoint(segMove.wr.wpOrigin))
        {
            fStartInside = TRUE;
        }
        if (circleTrigger.ContainsPoint(wpEnd))
        {
            fEndInside = TRUE;
        }
    }
    else if (tShape.eShape == AI_PROXIMITY_SHAPE_BOX)
    {
        Vector2D vIntersect2D;
        float flT;
        tShape.wobb.transform = tTriggerLoc.transform;



        if (!Intersect::LineSeg2DAndOBB(segMove, tShape.wobb, flT, &vIntersect2D))
        {
            return;
        }

        Vector3D wpStart(
            segMove.wr.wpOrigin.x,
            segMove.wr.wpOrigin.y,
            tShape.wobb.transform.wp.z);
        Vector3D wpEnd(
            segMove.wr.wpOrigin.x + segMove.wr.wvDirection.x * segMove.wdLength,
            segMove.wr.wpOrigin.y + segMove.wr.wvDirection.y * segMove.wdLength,
            tShape.wobb.transform.wp.z);

        if (tShape.wobb.ContainsPoint(wpStart))
        {
            fStartInside = TRUE;
        }
        if (tShape.wobb.ContainsPoint(wpEnd))
        {
            fEndInside = TRUE;
        }
    }






    if (fStartInside && fEndInside)
    {
        return;
    }

    if (!fStartInside && !fEndInside)
    {

        sTriggerEntered(idTrigger, *ptProximityDetectorBrainData, tACDMover.id, TRUE, FALSE);
        sTriggerExited(idTrigger, *ptProximityDetectorBrainData, tACDMover.id, TRUE);
    }

    else if (!fStartInside)
    {

        sTriggerEntered(idTrigger, *ptProximityDetectorBrainData, tACDMover.id, FALSE, FALSE);
    }
    else
    {


        sTriggerExited(idTrigger, *ptProximityDetectorBrainData, tACDMover.id, FALSE);
    }
}




void ProximityDetectorActivateAtPlace(ActorID idTrigger, ActorCommonData & tACDMover, BOOL bExit)



{
    Actor * ptTrigger = ActorTryToGet(idTrigger);
    if (ptTrigger == NULL)
    {
        return;
    }

    ActorCommonData * ptACDTrigger = ACDTryToGet(ptTrigger->idACD);
    if (ptACDTrigger == NULL)
    {
        return;
    }

    if (!ptACDTrigger->IsEnabled())
    {
        return;
    }

    ProximityDetectorBrainData * ptProximityDetectorBrainData = sProximityDetectorGetBrainData(idTrigger);
    if (ptProximityDetectorBrainData == NULL)
    {
        return;
    }
    ProximityShapeData & tShape = ptProximityDetectorBrainData->tShape;
    const WorldLocation & tTriggerLoc = ptACDTrigger->GetWorldLocation();
    const WorldLocation & tMoverLoc = tACDMover.GetWorldLocation();
    if (tTriggerLoc.idSWorld != tMoverLoc.idSWorld)
    {
        return;
    }

    BOOL bCollide = FALSE;


    if (tShape.eShape == AI_PROXIMITY_SHAPE_SPHERE)
    {


        Vector2D vDiff = Vector2D(tMoverLoc.transform.wp.x, tMoverLoc.transform.wp.y) - Vector2D(tTriggerLoc.transform.wp.x, tTriggerLoc.transform.wp.y);
        if (vDiff.MagnitudeSquared() <= tShape.flRadius * tShape.flRadius)
        {
            bCollide = TRUE;
        }
    }
    else if (tShape.eShape == AI_PROXIMITY_SHAPE_BOX)
    {

        Sphere ws;
        tShape.wobb.transform = tTriggerLoc.transform;


        ws.wpCenter = tMoverLoc.transform.wp;
        ws.wdRadius = 0.25f;
        if (Intersect::SphereAndOBB(ws, tShape.wobb, NULL))
        {
            bCollide = TRUE;
        }
    }







    if (bCollide)
    {
        if (bExit)
        {
            sTriggerExited(idTrigger, *ptProximityDetectorBrainData, tACDMover.id, FALSE);
        }
        else
        {
            sTriggerEntered(idTrigger, *ptProximityDetectorBrainData, tACDMover.id, FALSE, TRUE);
        }
    }
}



SNO ProximityDetectorBrainDataGetTriggerActorSNO(ActorID idTrigger)
{

    Actor * ptTrigger = ActorTryToGet(idTrigger);
    if (ptTrigger == NULL)
    {
        return SNO_NONE;
    }

    ActorCommonData * ptACDTrigger = ACDTryToGet(ptTrigger->idACD);
    if (ptACDTrigger == NULL)
    {
        return SNO_NONE;
    }

    ProximityDetectorBrainData * ptProximityDetectorBrainData = sProximityDetectorGetBrainData(idTrigger);
    if (ptProximityDetectorBrainData == NULL)
    {
        return SNO_NONE;
    }

    return ptProximityDetectorBrainData->snoTriggerActor;
}



void ProximityDetectorEnabledStatusChanged(ActorID idActor, BOOL bEnabled)


{

    ActorAI * ptAI = ActorTryToGetAI(idActor);
    if (ptAI)
    {
        ProximityDetectorBrainData * ptProxDetBrainData = ProximityDetectorTryGetData(ptAI->idProximityData);
        if (ptProxDetBrainData)
        {
            if (bEnabled)
            {


                ProximityDetectorBrainOnCreate(idActor);
            }
            else
            {

                ptProxDetBrainData->listACDs.RemoveAll();
            }
        }
    }
}
