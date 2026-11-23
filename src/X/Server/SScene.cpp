// Original file: X/Server/SScene.cpp
// Partly written: functions sit on their original lines (see the reference .cpp).

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "../Sigma/DataStructures/DataArraysTemplated.h"
#include "../Sigma/DataStructures/XGrowableMap.h"
#include "../Sigma/DataStructures/XPooledList.h"
#include "SScene.h"
#include "SGame.h"
#include "SGameManager.h"
#include "PlayerList.h"
#include "../Common/SceneCommon.h"
#include "../Common/Player.h"
#include "../Common/ActorCommonData.h"
#include "../Common/InactiveMarker.h"
#include "../Common/WorldPlace.h"
#include "../Common/World.h"
#include "../Sigma/Math/Transform.h"
#include "../Sigma/MemoryPools.h"
#include "../Sigma/Memory/SigmaMemory.h"
#include "../Common/MarkerSet.h"
#include "../Common/Definitions/SceneDefinition.h"
#include "../Common/Definitions/MarkerSetDefinition.h"

void SceneCommonDataDeactivate(SSceneID idSScene);  // X/Common/SceneCommon.cpp
void GizmoPlaceLootForScene(const SceneCommonData & tSCD, const Player & tPlayer);  // X/Server/SGizmo.cpp:3300
void ServerMessageSendSceneSwapToPlayer(const PlayerList & listDestPlayers, const SSceneID idSSceneOld, const SSceneID idSSceneNew);  // X/Server/Network/ServerMessageSend.cpp:930

LevelAreaID LevelAreaFind(const SNO snoLevelArea, const SWorldID idSWorld);  // X/Server/System/LevelArea.cpp:581
SSceneID LevelAreaGetWaypointSSceneID(const LevelAreaID idLevelArea);  // X/Server/System/LevelArea.cpp:635
void sScenePrefetchAroundPlace(Player * ptPlayer, WorldPlace tPlace, SNO snoWorld);
void SScenePrefetchDestination(Player * ptPlayer, WorldPlace tPlace);
void SceneCommonDataInitialize(SceneCommonData & tSCD, const SSceneID idSScene, const SceneSpecification & tSceneSpec, const SWorldID idSWorld, const SNO snoScene, const PRTransform & transform, const SSceneID idSSceneParent, const SNO snoSceneGroup, const SNO snoMarkerSetSrc, int32 nMarkerSetSrcIndex, const BOOL bIsAttachment, const BOOL bIsVista);  // X/Common/SceneCommon.cpp:489
LevelAreaID LevelAreaFindOrCreateBySWorld(const SWorldID idSWorld, const SNO snoLevelArea);  // X/Server/System/LevelArea.cpp
void SSceneListAll(BOOL bGameTestingOutput);
BOOL MarkerSetIterateMarkers(SNO snoMarkerSet, MarkerIterateCb pfuncIterate, void * ptUser);  // X/Common/MarkerSet.cpp:10

void SceneCommonInitializeForGame();  // X/Common/SceneCommon.cpp
void SceneCommonDisposeForGame();  // X/Common/SceneCommon.cpp:157
void SceneCommonDataDispose(SceneCommonData & tSCD);  // X/Common/SceneCommon.cpp:1129
uint32 ActorGroupIndexToHash(int32 nIndex);  // X/Common/Definitions/GlobalDefinition.cpp:205
void SPlayerRemoveSceneFromVisited(SSceneID idSScene);  // X/Server/SPlayer.cpp:4310
static void sSSceneDelete(const SSceneID idSScene);
namespace GameTesting {  // X/Common/GameTesting.h
void OutputInfoEntry(LPCSTR szMsg);  // GameTesting.cpp:1087
}  // namespace GameTesting

typedef XPooledDataIDList SSceneIDListPooled;

struct SSceneGlobals {  // size 0x130
    SSceneIDListPooled listScenesFlaggedForDeletion;  // 0x00
    SSceneIDListPooled listSceneSwapsFrom;  // 0x28
    SSceneIDListPooled listSceneSwapsTo;  // 0x50
    XGrowableMap<int, int, int, int> tPrefetchedSNOs;  // 0x78
    XListMemoryPool<unsigned long> tPlayerIDPool;  // 0xf8
    uint32 uNextDeactivationTime;  // 0x128
};
static_assert(sizeof(SSceneGlobals) == 0x130, "SSceneGlobals");











static const int32 SSCENE_PLAYERID_COUNT = 4;






























void SScenesInitializeForGame()
{

    SGame & tSGame = SGameManager::GameGet();
    tSGame.ptSScenes = TDataArrayCreateTracked<SScene, SSceneID>("SScenes", tSGame.ptMemoryInterface, MAX_SCENES);

    SSceneGlobals *& ptSSceneGlobals = tSGame.ptSSceneGlobals;
    ptSSceneGlobals = (SSceneGlobals *)SigmaMemoryNew(sizeof(SSceneGlobals), 4, tSGame.ptMemoryInterface, TRUE);

    ptSSceneGlobals->listScenesFlaggedForDeletion.Initialize(MemoryPoolsGetDATAIDListPool());
    ptSSceneGlobals->listSceneSwapsFrom.Initialize(MemoryPoolsGetDATAIDListPool());
    ptSSceneGlobals->listSceneSwapsTo.Initialize(MemoryPoolsGetDATAIDListPool());
    ptSSceneGlobals->tPrefetchedSNOs.Initialize(64, FALSE, NULL);
    ptSSceneGlobals->tPrefetchedSNOs.SetHashTableSize_Tracked(64);

    ptSSceneGlobals->tPlayerIDPool.Initialize();
    ptSSceneGlobals->tPlayerIDPool.SetMemoryInterface(tSGame.ptMemoryInterface);
    ptSSceneGlobals->tPlayerIDPool.SetBlockCount_Tracked(MAX_SCENES * SSCENE_PLAYERID_COUNT);

    ptSSceneGlobals->uNextDeactivationTime = 0;

    SceneCommonInitializeForGame();
}




void SScenesDisposeForGame()
{

    SceneCommonDisposeForGame();

    SGame & tSGame = SGameManager::GameGet();
    SSceneGlobals *& ptSSceneGlobals = tSGame.ptSSceneGlobals;
    TDataArrayIterator<SScene> tIt;

    while (tSGame.ptSScenes->Iterate(tIt))
    {
        SScene * ptSScene = tIt.ptElement;

        sSSceneDelete(ptSScene->id);
    }

    TDataArrayDispose(tSGame.ptSScenes);

    ptSSceneGlobals->listScenesFlaggedForDeletion.Dispose();
    ptSSceneGlobals->listSceneSwapsFrom.Dispose();
    ptSSceneGlobals->listSceneSwapsTo.Dispose();
    ptSSceneGlobals->tPrefetchedSNOs.Dispose();

    ptSSceneGlobals->tPlayerIDPool.Dispose();

    SigmaMemoryFree(ptSSceneGlobals, tSGame.ptMemoryInterface); ptSSceneGlobals = NULL;

}




void SScenesFlagWorldScenesForDeletion(SWorldID idSWorld)
{

    SGame & tSGame = SGameManager::GameGet();
    TDataArrayIterator<SScene> tIt;
    while (tSGame.ptSScenes->Iterate(tIt))
    {
        SScene * ptSScene = tIt.ptElement;
        if (ptSScene->tSCD.idSWorld == idSWorld)
        {
            SSceneFlagForRemoval(ptSScene->id);
        }
    }
}




void SScenesDisposeForWorld(SWorldID idSWorld)
{

    SGame & tSGame = SGameManager::GameGet();
    TDataArrayIterator<SScene> tIt;
    while (tSGame.ptSScenes->Iterate(tIt))
    {
        SScene * ptSScene = tIt.ptElement;
        if (ptSScene->tSCD.idSWorld == idSWorld)
        {
            sSSceneDelete(ptSScene->id);
        }
    }
}






SSceneID SSceneAdd(const SNO snoScene, const SceneSpecification & tSceneSpec, const SWorldID idSWorld, const PRTransform & transform, const SSceneID idSSceneParent, const SNO snoSceneGroup, const SNO snoMarkerSetSrc, int32 nMarkerSetSrcIndex, LabelListPooled & listAppliedLabels, BOOL bIsVista)
{
    SGame & tSGame = SGameManager::GameGet();

    SSceneID idSScene = tSGame.ptSScenes->ElementNew();
    if (idSScene == SSCENEID_INVALID)
    {
        SSceneListAll(FALSE);
        return SSCENEID_INVALID;
    }

    SScene * ptSScene = SSceneGet(idSScene);

    SceneCommonDataInitialize(ptSScene->tSCD, idSScene, tSceneSpec, idSWorld, snoScene, transform, idSSceneParent, snoSceneGroup, snoMarkerSetSrc, nMarkerSetSrcIndex, FALSE, bIsVista);

    for (int32 i = 0; i < 4; i++)
    {
        if (tSceneSpec.arSnoLevelAreas[i] == SNO_NONE)
        {
            ptSScene->arLevelAreaIds[i] = -1;
        }
        else
        {
            ptSScene->arLevelAreaIds[i] = LevelAreaFindOrCreateBySWorld(idSWorld, tSceneSpec.arSnoLevelAreas[i]);
        }
    }

    ptSScene->listACDInScene.SetPool(MemoryPoolsGetDATAIDListPool());

    if (idSSceneParent != SSCENEID_INVALID)
    {
        SceneCommonData & tSCDParent = SceneCommonDataGet(idSSceneParent);
        tSCDParent.tChildSceneIDs.AddTail_Tracked(idSScene);
    }

    POS pos = listAppliedLabels.GetHeadPosition();
    while (pos)
    {
        GBID gbidLabel = listAppliedLabels.GetNext(pos);
        ptSScene->tSCD.listAppliedLabels.AddTail_Tracked(gbidLabel);
    }

    ptSScene->bodyId = dm_nullBodyId;
    ptSScene->bHasWaypoint = FALSE;
    ptSScene->bIsVista = bIsVista;

    SSceneGlobals *& ptSSceneGlobals = tSGame.ptSSceneGlobals;
    ptSScene->listSeenPlayerIDs.SetPool(&ptSSceneGlobals->tPlayerIDPool);

    return idSScene;
}




















void SSceneFlagForRemoval(SSceneID idSScene)
{

    SScene * ptSScene = SSceneGet(idSScene);

    ptSScene->tSCD.dwSceneFlags |= (1 << SCENE_FLAGGED_FOR_DELETE);

    if (ptSScene->tSCD.idSSceneParent != SSCENEID_INVALID)
    {
        SScene * ptSSceneParent = SSceneGet(ptSScene->tSCD.idSSceneParent);

        POS pos = ptSSceneParent->tSCD.tChildSceneIDs.Find(ptSScene->id);
        ptSSceneParent->tSCD.tChildSceneIDs.RemoveAt(pos);
    }


    SceneCommonDataDeactivate(idSScene);


    SGame & tSGame = SGameManager::GameGet();

    SSceneGlobals *& ptSSceneGlobals = tSGame.ptSSceneGlobals;


    ptSSceneGlobals->listScenesFlaggedForDeletion.AddTail_Tracked(idSScene);
}



void SSceneDeleteAllFlagged()
{

    SGame & tSGame = SGameManager::GameGet();
    SSceneGlobals *& ptSSceneGlobals = tSGame.ptSSceneGlobals;

    while (!ptSSceneGlobals->listScenesFlaggedForDeletion.IsEmpty())
    {
        SSceneID idSScene = ptSSceneGlobals->listScenesFlaggedForDeletion.GetHead();

        ptSSceneGlobals->listScenesFlaggedForDeletion.RemoveHead();

        sSSceneDelete(idSScene);
    }
}




static void sSSceneDelete(const SSceneID idSScene)
{

    SGame & tSGame = SGameManager::GameGet();
    SScene * ptSScene = SSceneTryToGet(idSScene);
    if (ptSScene)
    {

        GameCommonData & tGCD = GameGetActiveGameCommonData();
        InactiveMarkerList * ptMarkerScene = ptSScene->tSCD.ptMarkerScene;

        if (ptMarkerScene->ptInactiveMarkerList)
        {

            for (POS pos = ptMarkerScene->ptInactiveMarkerList->GetHeadPosition(); pos; )
            {

                InactiveBaseMarker * ptInactiveMarker = ptMarkerScene->ptInactiveMarkerList->GetNext(pos);
                ACDPackID idACDPack;
                ACDGroup tACDGroup;

                if (ptInactiveMarker->nPrimaryGroupIndex != -1)
                {
                    uint32 uACDGroupCur = ActorGroupIndexToHash(ptInactiveMarker->nPrimaryGroupIndex);
                    if (tGCD.ptACDGlobals->tACDGroups.tMapACDGroups.Lookup(uACDGroupCur, tACDGroup))
                    {
                        tACDGroup.nCount--;
                        tGCD.ptACDGlobals->tACDGroups.tMapACDGroups.SetAt_Tracked(uACDGroupCur, tACDGroup);
                    }
                }

                if (ptInactiveMarker->nSecondaryGroupIndex != -1)
                {
                    uint32 uACDGroupCur = ActorGroupIndexToHash(ptInactiveMarker->nSecondaryGroupIndex);
                    if (tGCD.ptACDGlobals->tACDGroups.tMapACDGroups.Lookup(uACDGroupCur, tACDGroup))
                    {
                        tACDGroup.nCount--;
                        tGCD.ptACDGlobals->tACDGroups.tMapACDGroups.SetAt_Tracked(uACDGroupCur, tACDGroup);
                    }
                }


                idACDPack = InactiveMarkerGetPack(ptInactiveMarker);
                if (idACDPack != ACDPACKID_INVALID)
                {
                    ACDPack * ptACDPack = ACDPackTryToGet(idACDPack);
                    if (ptACDPack)
                    {
                        ptACDPack->RemoveDeactivated();
                    }
                }
            }
        }

        SceneCommonDataDispose(ptSScene->tSCD);
        tSGame.ptSScenes->ElementDeleteByID(idSScene);

        SPlayerRemoveSceneFromVisited(idSScene);

        ptSScene->listSeenPlayerIDs.RemoveAll();
    }
}

BOOL SSceneDataArrayIterate(TDataArrayIterator<SScene> & tIterator)
{

    SGame & tSGame = SGameManager::GameGet();

    SScene * ptScene = tSGame.ptSScenes->Iterate(tIterator);

    return ptScene != NULL;
}




SScene * SSceneGet(SSceneID id)
{

    return SGameManager::GameGet().ptSScenes->ElementGet(id);
}




SScene * SSceneTryToGet(SSceneID id)
{

    return SGameManager::GameGet().ptSScenes->ElementTryToGet(id);
}
// Dependencies declared by the owner CU; existing canonical header definitions are reused.
#include "SWorld.h"
#include "../Common/Movement/ActorMovement.h"
#include "../Common/Quests/Quests.h"
#include "../Sigma/Math/Frustum.h"
// X/Common/SceneCommon.cpp:2155,2226.
void SceneCreatePlayerPrefetchFrustum(Player * ptPlayer, Frustum & tFrustum);
void SceneCreateRecordedAVFrustum(Player * ptPlayer, Frustum & tFrustum);
// X/Server/Replay.cpp:849.
BOOL ReplayIsUsingRecordedAVFrustum();
// X/Common/InactiveMarker.cpp:3175.
void ActivateInactiveMarkerList(SWorldID idSWorld, SSceneID idSScene, InactiveMarkerList * ptList);
// X/Server/Actors/Actors.cpp:6286,6384,6439,6544.
void ActorsMakeVisibleList(SWorldID idSWorld, const Frustum & tFrustum, const Frustum * ptAVFrustum, const Vector3D & vCenterPos, Player ** pptPlayer, int32 nPlayerCount);
void ActorsAddFastMoversToVisibleList(SWorldID idSWorld, const Frustum & tFrustum, Player ** pptPlayer, int32 nPlayerCount);
void ActorsMakePrefetchList(SWorldID idSWorld, const Frustum & tFrustum, Player ** pptPlayer, int32 nPlayerCount);
void ActorDeactivateUnseen();

// X/Server/PVP.cpp:1708,917.
uint32 PVPGetDuelingEntranceGUID(const Player & tPlayer, SWorldID idSWorld, BOOL bPickFar);
uint32 PVPGetPlayerEntranceGUID(const Player & tPlayer, SWorldID idSWorld);
// X/Common/SceneCommon.cpp:1623.
void SceneGetSceneToWorldTransform(SSceneID idSScene, PRTransform & transformSceneToWorld);
// X/Common/SceneCommon.cpp:2178,2280 and X/Common/World.cpp:793.
void SceneCreatePlacePrefetchFrustum(WorldPlace tPlace, Frustum & tFrustum);
BOOL SceneIntersectsFrustum(SceneCommonData & tSCD, const Frustum & tFrustum);
BOOL WorldTreatAsOneSceneBySNO(const SNO snoWorld);
// X/Server/Network/ServerMessageSend.cpp:960,970,980.
void ServerMessageSendPrefetchWorldToPlayer(const PlayerList & listDestPlayers, const SNO snoWorld);
void ServerMessageSendPrefetchSceneToPlayer(const PlayerList & listDestPlayers, const SNO snoScene);
void ServerMessageSendPrefetchLevelAreaToPlayer(const PlayerList & listDestPlayers, const SNO snoLevelArea);
// X/Client/SNOPreload.cpp:3370.
namespace SNOPreload { void PrefetchServerScene(const SNO snoScene, const SNO * ptSNOLevelAreas); }

#include "Actors/Actors.h"
#include "../Common/Nav/NavigationMesh.h"
// X/Common/Nav/NavigationMesh.cpp:3665,3011,3421.
BOOL NavMeshHandleIsInRegion(const NavMeshHandle & tNavMeshHandle, const IVector2D & tOffset, const IVector2D & tSize);
void NavMeshGetActorPassabilitySettings(SNO snoActor, float flScale, PassabilitySettings & tPassability);
BOOL NavMeshSearchForLocation(const WorldPlace & placeStart, const PassabilitySettings & tPassability, const float flMinSearchRadius, const float flMaxSearchRadius, const uint32 dwFindLocationFlags, NavMeshHandle & tNavMeshHandleLanding, WorldPlace & placeLanding, BOOL bSearchOverImpassable, Team eTeam, float flRadiusOverride);
// X/Server/Actors/Actors.cpp:6093.
void ActorChangeWorld(const ActorID idActor, const WorldPlace & tPlace, const Quaternion & q);
// X/Common/SceneCommon.cpp:729.
void SceneCommonDataActivate(SSceneID idSScene);
#include "../Common/ActorCommonDataSwig.h"





























































void SScenePutACDInScene(SSceneID idSScene, ACDID idACD, BOOL bAdd)
{


    SScene * ptSScene = SSceneGet(idSScene);

    if (bAdd)
    {
        if (ptSScene->listACDInScene.Find(idACD) == NULL)
        {
            ptSScene->listACDInScene.AddTail_Tracked(idACD);
        }
    }
    else
    {
        POS pos = ptSScene->listACDInScene.Find(idACD);
        if (pos != NULL)
        {
            ptSScene->listACDInScene.RemoveAt(pos);
        }
    }
}





void SScenePutScriptInScene(const SSceneID idSScene, const SNO snoScript, const uint16 nAdventureParentIndex)
{


    SScene * ptSScene = SSceneGet(idSScene);

    SceneScript tSceneScript;
    tSceneScript.snoScript = snoScript;
    tSceneScript.nAdventureParentIndex = nAdventureParentIndex;

    ptSScene->listScriptsInScene.AddTail_Tracked(tSceneScript);
}



















ACDID SSceneFindFirstGizmo(const SSceneID idSScene, const GizmoType eGizmoTypeToFind)
{


    SScene * ptSScene = SSceneGet(idSScene);
    XPooledDataIDList & tListACDInScene = ptSScene->listACDInScene;

    POS pos = tListACDInScene.GetHeadPosition();
    while (pos != NULL)
    {
        POS posOld = pos;
        const ACDID idACD = (ACDID)tListACDInScene.GetNext(pos);

        ActorCommonData * ptACD = ACDTryToGet(idACD);
        if (ptACD != NULL)
        {

            GizmoType eGizmoType = ptACD->GetGizmoType();
            if (eGizmoType == eGizmoTypeToFind)
            {
                return idACD;
            }
        }
        else
        {
            tListACDInScene.RemoveAt(posOld);
        }
    }


    pos = ptSScene->tSCD.tChildSceneIDs.GetHeadPosition();
    while (pos != NULL)
    {
        SSceneID idChildScene = ptSScene->tSCD.tChildSceneIDs.GetNext(pos);
        ACDID idACD = SSceneFindFirstGizmo(idChildScene, eGizmoTypeToFind);
        if (idACD != ACDID_INVALID)
        {
            return idACD;
        }
    }

    return ACDID_INVALID;
}


BOOL SSceneIterateMarkers(SSceneID idSScene, MarkerIterateCb pfuncIterate, void * ptUser, BOOL bIncludeSubscenes)
{
    BOOL bContinueSearch = TRUE;

    SScene * ptSScene = SSceneGet(idSScene);
    SNO snoScene = ptSScene->tSCD.snoScene;
    ScopedSceneDefinitionLockR ptSceneDef(snoScene);

    for (int32 i = 0; i < ptSceneDef->arMarkerSets.GetArraySize(); i++)
    {
        ScopedMarkerSetDefinitionLockR ptMarkerSetDef(ptSceneDef->arMarkerSets[i]);
        GBID gbidLabel = ptMarkerSetDef->nLabel;
        if (gbidLabel != -1 && !ptSScene->tSCD.listAppliedLabels.Find(gbidLabel))
        {
            bContinueSearch = TRUE;
        }
        else
        {
            bContinueSearch = MarkerSetIterateMarkers(ptSceneDef->arMarkerSets[i], pfuncIterate, ptUser);
        }
        if (!bContinueSearch)
        {
            break;
        }
    }

    if (bIncludeSubscenes && bContinueSearch)
    {
        POS pos = ptSScene->tSCD.tChildSceneIDs.GetHeadPosition();
        while (pos)
        {
            SSceneID idChildScene = ptSScene->tSCD.tChildSceneIDs.GetNext(pos);
            bContinueSearch = SSceneIterateMarkers(idChildScene, pfuncIterate, ptUser, FALSE);
            if (!bContinueSearch)
            {
                break;
            }
        }
    }

    return bContinueSearch;
}





















static BOOL sIsWorldPosInSubScene(
    const SceneCommonData & tSCD,
    Vector3D wp)
{
    BOOL bInsideSubScene = FALSE;

    // Concrete DWARF bInsideSubScene is constu0 throughout; only transform, bounds and lock survive.
    Vector3D wpSceneSpace;

    wpSceneSpace = tSCD.transformInvWorld * wp;


    // Inference: original compares each horizontal coordinate against 0 and 1000; no named bound recovered.
    if ((wpSceneSpace.x >= 0.0f && wpSceneSpace.x < 1000.0f) &&
        (wpSceneSpace.y >= 0.0f && wpSceneSpace.y < 1000.0f))
    { // DWARF-only dead grid locals: no initializer/use survives, so none is invented.
        int32 nSquareX;
        int32 nSquareY;

        ScopedSceneDefinitionLockR ptSceneDef(tSCD.snoScene);










    }



    return bInsideSubScene;

}

static void sPrepareActorsForSubSceneSwap(
    SSceneID idSScene,
    SSceneID idSSceneNew)
{
    SceneCommonData & tSCD = SceneCommonDataGet(idSScene);
    ActorIterator tIter;
    Actor * ptActor = ActorDataArrayIterate(tIter);

    Vector3D wpOffset = tSCD.transformWorld.wp;

    if (tSCD.idSSceneParent != SSCENEID_INVALID)
    {
        SceneCommonData & tSCDParent = SceneCommonDataGet(tSCD.idSSceneParent);

        wpOffset -= tSCDParent.transformWorld.wp;
    }

    IVector2D tSize;
    IVector2D tOffset;

    tOffset.x = (int32)(wpOffset.x / cflGridCellWidth);
    tOffset.y = (int32)(wpOffset.y / cflGridCellWidth);


    {
        ScopedSceneDefinitionLockR ptSceneDef(tSCD.snoScene);
        tSize.x = ptSceneDef->tNavMeshData.nSizeX;
        tSize.y = ptSceneDef->tNavMeshData.nSizeY;
    }



    while (ptActor)
    {
        ActorCommonData * ptACD = &ACDGet(ptActor->idACD);
        if (ptActor->nSourceSceneUID == tSCD.nInstanceUID)
        {

            ActorFlagForDeletion(ptActor->id);
        }
        else if (ptACD->tLocation.eActorLocation == AL_WORLD)
        {



            // Inference: parent validity and current-scene equality share the original branch at763.
            if (tSCD.idSSceneParent != SSCENEID_INVALID && ptActor->idCurrentScene == tSCD.idSSceneParent)
            {
                WorldPlace placeCurr = ptACD->GetPlace();
                NavMeshHandle tNavMeshHandle;
                ACDGetNavMeshHandle(*ptACD, tNavMeshHandle);



                if (!NavMeshHandleIsNone(tNavMeshHandle) &&
                    NavMeshHandleIsInRegion(tNavMeshHandle, tOffset, tSize))
                {
                    NavMeshHandle tGoodNavMesh;
                    PassabilitySettings tPassability;
                    WorldPlace wpResult;

                    NavMeshGetActorPassabilitySettings(ptACD->snoActor, ptACD->tLocation.tWorldLocation.flScale, tPassability);

                    if (tPassability.ePassabilityType == PASSABILITY_UNDEFINED)
                    {
                        tPassability.ePassabilityType = PASSABILITY_MONSTER;
                    }
                    // Inference: original search arguments are radius0..50, flags0, over-impassableTRUE, TEAM_NEUTRAL, radius override0.
                    BOOL bFoundGoodLoc = NavMeshSearchForLocation(placeCurr, tPassability, 0.0f, 50.0f, 0, tGoodNavMesh, wpResult, TRUE, TEAM_NEUTRAL, 0.0f);

                    if (bFoundGoodLoc)
                    {

                        ActorSnapToPosition(ptActor->id, wpResult, TRUE, TRUE);
                    }
                }
            }




            else if (tSCD.idSScene == ptActor->idCurrentScene)
            {
                WorldPlace placeCurr = ptACD->GetPlace();
                NavMeshHandle tNavMeshHandle;
                ACDGetNavMeshHandle(*ptACD, tNavMeshHandle);

                if (!NavMeshHandleIsNone(tNavMeshHandle))
                {
                    NavMeshHandle tGoodNavMesh;
                    PassabilitySettings tPassability;
                    WorldPlace wpResult;

                    NavMeshGetActorPassabilitySettings(ptACD->snoActor, ptACD->tLocation.tWorldLocation.flScale, tPassability);

                    if (tPassability.ePassabilityType == PASSABILITY_UNDEFINED)
                    {
                        tPassability.ePassabilityType = PASSABILITY_MONSTER;
                    }

                    ptActor->idCurrentScene = idSSceneNew;




                    tNavMeshHandle.idSScene = idSSceneNew;
                    // Inference: same proven search arguments as785; original ignores the returned boolean here.
                    BOOL bFoundLoc = NavMeshSearchForLocation(placeCurr, tPassability, 0.0f, 50.0f, 0, tGoodNavMesh, wpResult, TRUE, TEAM_NEUTRAL, 0.0f);






                    ActorChangeWorld(ptActor->id, wpResult, cgqIdentity);
                }
            }
        }
        ptActor = ActorDataArrayIterate(tIter);
    }




    TDataArrayIterator<SScene> tIterator;
    while (SSceneDataArrayIterate(tIterator))
    {
        SScene * ptSScene = tIterator.ptElement;

        if (tSCD.idSWorld == ptSScene->tSCD.idSWorld)
        {
            InactiveMarkerList * ptMarkerScene = ptSScene->tSCD.ptMarkerScene;

            if (!ptMarkerScene->ptInactiveMarkerList)
            { continue; }



            POS pos = ptMarkerScene->ptInactiveMarkerList->GetHeadPosition();
            while (pos)
            {
                POS posCur = pos;
                InactiveBaseMarker * ptInactiveMarker = ptMarkerScene->ptInactiveMarkerList->GetNext(pos);
                // Inference: unknown marker kinds use source UID-1, as the original default path.
                int32 nMarkerSourceSceneUID = -1;
                switch (ptInactiveMarker->eInactiveMarkerType)
                { case INACTIVE_MONSTER: {
                    InactiveMonsterMarker * ptInactiveMonster = static_cast<InactiveMonsterMarker *>(ptInactiveMarker);

                    nMarkerSourceSceneUID = ptInactiveMonster->nSourceSceneUID;
                    break; }

                  case INACTIVE_GIZMO: {
                    InactiveGizmoMarker * ptInactiveGizmo = static_cast<InactiveGizmoMarker *>(ptInactiveMarker);

                    nMarkerSourceSceneUID = ptInactiveGizmo->nSourceSceneUID;
                    break; }
                }
                if (nMarkerSourceSceneUID == tSCD.nInstanceUID)
                {

                    ptMarkerScene->ptInactiveMarkerList->RemoveAt(posCur);
                } else // Inference: helper result is proven FALSE; no dead dependent body is invented.
                    sIsWorldPosInSubScene(tSCD, ptInactiveMarker->transform.wp);
            }
        }
    }





}

void SSceneSwapSubScene(
    SSceneID idSScene,
    SNO snoNewScene)
{


    SWorldID idSWorld;
    SceneSpecification tSceneSpec;
    SSceneID idSSceneParent;
    PRTransform transform;
    SNO snoSceneGroup;
    SNO snoMarkerSet;
    int nMarkerSetIndex;

    SceneCommonData & tSCD = SceneCommonDataGet(idSScene);

    idSWorld = tSCD.idSWorld;
    tSceneSpec = tSCD.tSceneSpec;
    idSSceneParent = tSCD.idSSceneParent;
    transform = tSCD.transformWorld;
    snoSceneGroup = tSCD.snoSceneGroup;
    snoMarkerSet = tSCD.snoMarkerSetSrc;
    nMarkerSetIndex = tSCD.nMarkerSetSrcIndex;

    tSceneSpec.tCachedValues.dwCachedValuesValid = FALSE;

    SSceneFlagForRemoval(idSScene);

    SceneCommonData * ptSCDParent = SceneCommonDataTryToGet(idSSceneParent);


    // Inference: final bIsAttachment argument is FALSE in original call.
    SSceneID idSSceneNew = SSceneAdd(snoNewScene,
        tSceneSpec,
        idSWorld,
        transform,
        idSSceneParent,
        snoSceneGroup,
        snoMarkerSet,
        nMarkerSetIndex,
        ptSCDParent->listAppliedLabels,
        FALSE);

    SceneCommonDataActivate(idSSceneNew);

    sPrepareActorsForSubSceneSwap(idSScene, idSSceneNew);

    SGame & tSGame = SGameManager::GameGet();
    SSceneGlobals * ptSSceneGlobals = tSGame.ptSSceneGlobals;

    ptSSceneGlobals->listSceneSwapsFrom.AddTail_Tracked(idSScene);
    ptSSceneGlobals->listSceneSwapsTo.AddTail_Tracked(idSSceneNew);
}




void SSceneSendSceneSwapMessages(const PlayerList & listDestPlayers)
{

    SGame & tSGame = SGameManager::GameGet();

    SSceneGlobals * ptSSceneGlobals = tSGame.ptSSceneGlobals;


    if (!ptSSceneGlobals->listSceneSwapsFrom.IsEmpty())
    {

        POS posFrom = ptSSceneGlobals->listSceneSwapsFrom.GetHeadPosition();
        POS posTo = ptSSceneGlobals->listSceneSwapsTo.GetHeadPosition();

        while (posFrom != NULL)
        {

            SSceneID idSScene = ptSSceneGlobals->listSceneSwapsFrom.GetNext(posFrom);
            SSceneID idSSceneNew = ptSSceneGlobals->listSceneSwapsTo.GetNext(posTo);

            ServerMessageSendSceneSwapToPlayer(listDestPlayers, idSScene, idSSceneNew);
        }
    }
}



void SSceneFinalizeSubSceneSwaps()
{

    SGame & tSGame = SGameManager::GameGet();

    SSceneGlobals * ptSSceneGlobals = tSGame.ptSSceneGlobals;


    while (!ptSSceneGlobals->listSceneSwapsFrom.IsEmpty())
    {
        ptSSceneGlobals->listSceneSwapsFrom.RemoveHead();
        ptSSceneGlobals->listSceneSwapsTo.RemoveHead();
    }

}


void SSceneListAll(BOOL bGameTestingOutput)
{

    CRefString sMsg;
    sMsg.Format("Count\tSceneId\tWorldSNO\tSceneSNO\tX\tY\tZ\n");
    if (bGameTestingOutput)
    {
        GameTesting::OutputInfoEntry(sMsg);
    }



    uint32 count = 0;
    TDataArrayIterator<SScene> tIterator;
    while (SSceneDataArrayIterate(tIterator))
    {
        SScene * ptSScene = tIterator.ptElement;
        SNO snoWorld = WorldCommonGetWorldSNO(ptSScene->tSCD.idSWorld);
        sMsg.Format("%d\t%d\t%s\t%s\t%.1f\t%.1f\t%.1f\n", count++, ptSScene->tSCD.idSScene, SNOToString(WORLD_SNO, snoWorld, FALSE).str(), SNOToString(SCENE_SNO, ptSScene->tSCD.snoScene, FALSE).str(), ptSScene->tSCD.transformWorld.wp.x, ptSScene->tSCD.transformWorld.wp.y, ptSScene->tSCD.transformWorld.wp.z);
        if (bGameTestingOutput)
        {
            GameTesting::OutputInfoEntry(sMsg);
        }
    }
}










void SSceneSetKnownByPlayer(SScene & tSScene, const Player & tPlayer)
{


    if (tSScene.listSeenPlayerIDs.Find(tPlayer.uHeroId) != NULL)
    {
        return;
    }

    if (!(tSScene.tSCD.dwSceneFlags & (1 << SCENE_ACTIVATED)))
    {
        return;
    }

    tSScene.listSeenPlayerIDs.AddTail_Tracked(tPlayer.uHeroId);



    GizmoPlaceLootForScene(tSScene.tSCD, tPlayer);
}


uint32 SSceneGetAllSceneActorsSNO(SScene & tSScene, SNO * snoActors, uint32 uActorsCapacity)
{



    XGrowableMap<int, int, int, int> tSNOToBoolMap(16, NULL);
    uint32 uActorCount = 0;

    POS pos = tSScene.listACDInScene.GetHeadPosition();
    while (pos)
    {
        POS posOld = pos;
        const ACDID idACD = (ACDID)tSScene.listACDInScene.GetNext(pos);

        ActorCommonData * ptACD = ACDTryToGet(idACD);
        if (ptACD == NULL)
        {
            tSScene.listACDInScene.RemoveAt(posOld);
        }
        else
        {
            SNO snoActor = ptACD->snoActor;
            BOOL fDummy;
            if (!tSNOToBoolMap.Lookup(snoActor, fDummy))
            {
                snoActors[uActorCount++] = snoActor;
                tSNOToBoolMap.SetAt_Tracked(snoActor, TRUE);
                if (uActorCount == uActorsCapacity)
                {
                    break;
                }
            }
        }
    }

    if (uActorCount < uActorsCapacity && tSScene.tSCD.ptMarkerScene != NULL)
    {
        InactiveMarkerList * ptInactiveMarkerList = tSScene.tSCD.ptMarkerScene;
        if (ptInactiveMarkerList->ptInactiveMarkerList != NULL)
        {
            POS posInactiveMarker = ptInactiveMarkerList->ptInactiveMarkerList->GetHeadPosition();
            while (posInactiveMarker)
            {
                InactiveBaseMarker * ptInactiveBaseMarker = ptInactiveMarkerList->ptInactiveMarkerList->GetNext(posInactiveMarker);
                if (ptInactiveBaseMarker->eInactiveMarkerType == INACTIVE_MONSTER)
                {
                    SNO snoActor = ((InactiveMonsterMarker *)ptInactiveBaseMarker)->snoMonster;
                    BOOL fDummy;
                    if (!tSNOToBoolMap.Lookup(snoActor, fDummy))
                    {
                        snoActors[uActorCount++] = snoActor;
                        tSNOToBoolMap.SetAt_Tracked(snoActor, TRUE);
                        if (uActorCount == uActorsCapacity)
                        {
                            break;
                        }
                    }
                }
            }
        }
    }

    if (uActorCount < uActorsCapacity && tSScene.tSCD.ptMarkersToBeActivated != NULL)
    {
        InactiveMarkerList * ptInactiveMarkerList = tSScene.tSCD.ptMarkersToBeActivated;
        if (ptInactiveMarkerList->ptInactiveMarkerList != NULL)
        {
            POS posInactiveMarker = ptInactiveMarkerList->ptInactiveMarkerList->GetHeadPosition();
            while (posInactiveMarker)
            {
                InactiveBaseMarker * ptInactiveBaseMarker = ptInactiveMarkerList->ptInactiveMarkerList->GetNext(posInactiveMarker);
                if (ptInactiveBaseMarker->eInactiveMarkerType == INACTIVE_MONSTER)
                {
                    SNO snoActor = ((InactiveMonsterMarker *)ptInactiveBaseMarker)->snoMonster;
                    BOOL fDummy;
                    if (!tSNOToBoolMap.Lookup(snoActor, fDummy))
                    {
                        snoActors[uActorCount++] = snoActor;
                        tSNOToBoolMap.SetAt_Tracked(snoActor, TRUE);
                        if (uActorCount == uActorsCapacity)
                        {
                            break;
                        }
                    }
                }
            }
        }
    }

    return uActorCount;
}


// 1215-1223: columns (1215:28, 1217:59/63 one level deeper than 1222:59 and 1223:13/49/56) put the primary-player stores after a `continue` block, not in an else.
void SActivationUpdate(
    BOOL bUpdateLists)
{


    // Inference: hidden-server branch exits before all activation work, as the original shared epilogue.
    if (GameGetParts() == GAME_PARTS_HIDDEN_SERVER)
    {
        return;
    }

    uint32 uNow = GameGetServerTime();
    SGame & tSGame = SGameManager::GameGet();
    SSceneGlobals * ptSSceneGlobals = tSGame.ptSSceneGlobals;
    BOOL bDeactivate = FALSE;
    if (uNow >= ptSSceneGlobals->uNextDeactivationTime)
    { bUpdateLists = TRUE; bDeactivate = TRUE; }




    if (!bUpdateLists)
    {
        for (Player * ptPlayer = PlayerGetFirstAll(); ptPlayer; ptPlayer = PlayerGetNextAll(ptPlayer))
        {

            if (!ptPlayer->bIsPrimaryPlayerServerOnly)
            { continue; }



            if (ptPlayer->idPlayerACD == ACDID_INVALID)
            { continue; }



            if (!PlayerIsInWorld(*ptPlayer))
            { continue; }


            // Inference: a forced update sets bUpdateLists and leaves this scan (original shared tail).
            if (ptPlayer->bForceKnownListUpdate)
            { bUpdateLists = TRUE; break; }
        }
    }




    Player * ptPrimaryPlayers[4][4];
    int32 nPrimaryLocalPlayerCount[4];
    int32 nPrimaryPlayerCount = 0;


    Player * ptNonPrimaryPlayers[4];
    int32 nNonPrimaryPlayerCount = 0;



    for (Player * ptPlayer = PlayerGetFirstAll(); ptPlayer; ptPlayer = PlayerGetNextAll(ptPlayer))
    {
        if (ptPlayer->idPlayerACD == ACDID_INVALID)
        { continue; }



        if (!PlayerIsInWorld(*ptPlayer))
        { continue; }




        if (!ptPlayer->bIsPrimaryPlayerServerOnly)
        {
            ptNonPrimaryPlayers[nNonPrimaryPlayerCount++] = ptPlayer;
            continue;
        }


        nPrimaryLocalPlayerCount[nPrimaryPlayerCount] = 1;
        ptPrimaryPlayers[nPrimaryPlayerCount++][0] = ptPlayer;

    }

    for (int32 i = 0; i < nNonPrimaryPlayerCount; i++)
    {
        Player * ptPlayer = ptNonPrimaryPlayers[i];
        // Inference: collect each secondary player under the first matching connection, then stop scanning.
        for (int32 j = 0; j < nPrimaryPlayerCount; j++)
        {
            if (ptPrimaryPlayers[j][0]->idGameConnection == ptPlayer->idGameConnection)
            {
                ptPrimaryPlayers[j][nPrimaryLocalPlayerCount[j]++] = ptPlayer;
                break;
            }
        }
    }


    // Inference: the update flag selects full visibility rebuild versus fast-mover-only updates.
    if (bUpdateLists)
    {
        Frustum atFrustum[4];
        Frustum atPrefetchFrustum[4];
        Frustum atAVFrustum[4];
        Vector3D avCenterPos[4];








        for (int32 nIndex = 0; nIndex < nPrimaryPlayerCount; nIndex++)
        {
            Player * ptPlayer = ptPrimaryPlayers[nIndex][0];


            Frustum & tFrustumToUse = atFrustum[ptPlayer->nPlayerIndex];
            Frustum & tPrefetchFrustumToUse = atPrefetchFrustum[ptPlayer->nPlayerIndex];
            Vector3D & vCenterPosToUse = avCenterPos[ptPlayer->nPlayerIndex];

            SceneCreatePlayerFrustum(ptPlayer, tFrustumToUse, vCenterPosToUse);

            SceneCreatePlayerPrefetchFrustum(ptPlayer, tPrefetchFrustumToUse);

            ActorCommonData & tACDPlayer = ACDGet(ptPlayer->idPlayerACD);
            SWorldID idSWorld = tACDPlayer.GetSWorldID();

            SceneCheckInactiveMarkers(idSWorld, tFrustumToUse, vCenterPosToUse);
            InactiveMarkerActivateInFrustrumForWorld(idSWorld, tFrustumToUse, vCenterPosToUse);

            if (ReplayIsUsingRecordedAVFrustum())
            {

                Frustum & tAVFrustumToUse = atAVFrustum[ptPlayer->nPlayerIndex];
                SceneCreateRecordedAVFrustum(ptPlayer, tAVFrustumToUse);
                SceneCheckInactiveMarkers(idSWorld, tAVFrustumToUse, vCenterPosToUse);
                InactiveMarkerActivateInFrustrumForWorld(idSWorld, tAVFrustumToUse, vCenterPosToUse);
            }


            else if (ptPlayer->uServerFlags & (1 << PSF_IN_AV_CAMERA_MODE))
            {
                Frustum & tAVFrustumToUse = atAVFrustum[ptPlayer->nPlayerIndex];
                SceneCreateAVFrustum(ptPlayer, tAVFrustumToUse);

                Vector3D & vAVCenterPos = ptPlayer->tAVCameraTransform.wp;
                SceneCheckInactiveMarkers(idSWorld, tAVFrustumToUse, vAVCenterPos);
                InactiveMarkerActivateInFrustrumForWorld(idSWorld, tAVFrustumToUse, vAVCenterPos);
            }
            // Inference: absent world skips marker activation; no additional fallback is emitted.
            SWorld * ptSWorld = SWorldTryToGet(idSWorld);
            if (ptSWorld)
            {
                ActivateInactiveMarkerList(idSWorld, SSCENEID_INVALID, ptSWorld->tWCD.ptMarkersToActivate);
            }
        }








        for (int32 nIndex = 0; nIndex < nPrimaryPlayerCount; nIndex++)
        {
            Player * ptPlayer = ptPrimaryPlayers[nIndex][0];

            int32 nPlayerIndex = ptPlayer->nPlayerIndex;


            Frustum & tFrustumToUse = atFrustum[nPlayerIndex];
            Frustum & tFarFrustumToUse = atPrefetchFrustum[nPlayerIndex]; // Inference: AV frustum defaults to NULL unless a recorded/current camera replaces it.
            Frustum * ptAVFrustum = NULL;
            Vector3D & vCenterPosToUse = avCenterPos[nPlayerIndex];

            if (ReplayIsUsingRecordedAVFrustum())
            {



                ptPlayer->listVisibleACDs.RemoveAll();
                for (int32 nAVIndex = 0; nAVIndex < nPrimaryPlayerCount; nAVIndex++)
                {
                    Player * ptAVPlayer = ptPrimaryPlayers[nAVIndex][0];

                    tFrustumToUse = atFrustum[ptAVPlayer->nPlayerIndex];
                    tFarFrustumToUse = atPrefetchFrustum[ptAVPlayer->nPlayerIndex];

                    ptAVFrustum = &atAVFrustum[ptAVPlayer->nPlayerIndex];

                    ActorCommonData & tACDAVPlayer = ACDGet(ptAVPlayer->idPlayerACD);
                    SWorldID idSWorld = tACDAVPlayer.GetSWorldID();

                    Player * ptPlayerLoc = PlayerGetByPlayerIndex(nPlayerIndex);

                    SceneActivateVisible(idSWorld, tFrustumToUse, tFarFrustumToUse, ptAVFrustum, vCenterPosToUse, ptPrimaryPlayers[nIndex], nPrimaryLocalPlayerCount[nIndex]);

                    ActorsMakeVisibleList(idSWorld, tFrustumToUse, ptAVFrustum, vCenterPosToUse, ptPrimaryPlayers[nIndex], nPrimaryLocalPlayerCount[nIndex]);
                    // Inference: only the local AV player clears and regenerates the prefetch list.
                    if (ptPlayerLoc == ptAVPlayer)
                    {
                        for (int32 i = 0; i < nPrimaryLocalPlayerCount[nIndex]; i++)
                        {
                            ptPrimaryPlayers[nIndex][i]->listPrefetchableACDs.RemoveAll();
                        }
                        ActorsMakePrefetchList(idSWorld, tFarFrustumToUse, ptPrimaryPlayers[nIndex], nPrimaryLocalPlayerCount[nIndex]);
                    }
                }
            }
            else
            {
                if (ptPlayer->uServerFlags & (1 << PSF_IN_AV_CAMERA_MODE))
                {
                    ptAVFrustum = &atAVFrustum[ptPlayer->nPlayerIndex];
                }

                ActorCommonData & tACDPlayer = ACDGet(ptPlayer->idPlayerACD);
                SWorldID idSWorld = tACDPlayer.GetSWorldID();



                SceneActivateVisible(idSWorld, tFrustumToUse, tFarFrustumToUse, ptAVFrustum, vCenterPosToUse, ptPrimaryPlayers[nIndex], nPrimaryLocalPlayerCount[nIndex]);




                for (int32 i = 0; i < nPrimaryLocalPlayerCount[nIndex]; i++)
                {
                    ptPrimaryPlayers[nIndex][i]->listVisibleACDs.RemoveAll();
                }
                ActorsMakeVisibleList(idSWorld, tFrustumToUse, ptAVFrustum, vCenterPosToUse, ptPrimaryPlayers[nIndex], nPrimaryLocalPlayerCount[nIndex]);




                for (int32 i = 0; i < nPrimaryLocalPlayerCount[nIndex]; i++)
                {
                    ptPrimaryPlayers[nIndex][i]->listPrefetchableACDs.RemoveAll();
                }
                ActorsMakePrefetchList(idSWorld, tFarFrustumToUse, ptPrimaryPlayers[nIndex], nPrimaryLocalPlayerCount[nIndex]);
            }
        }


    }

    else
    {

        for (int32 nIndex = 0; nIndex < nPrimaryPlayerCount; nIndex++)
        {
            Player * ptPlayer = ptPrimaryPlayers[nIndex][0];

            ActorCommonData & tACDPlayer = ACDGet(ptPlayer->idPlayerACD);
            Frustum tFrustum;
            Vector3D vCenterPos;
            SceneCreatePlayerFrustum(ptPlayer, tFrustum, vCenterPos);
            ActorsAddFastMoversToVisibleList(tACDPlayer.GetSWorldID(), tFrustum, ptPrimaryPlayers[nIndex], nPrimaryLocalPlayerCount[nIndex]);
        }
    }



    // Inference: KEEP_ALL_SCENES_ACTIVE skips both deactivation and forced-location trigger processing.
    if (GameIsParamFlagSet(GPF_KEEP_ALL_SCENES_ACTIVE))
    { return; }




    if (bDeactivate)
    {




        SceneDeactivateUnseen();
        ActorDeactivateUnseen();
        // DWARF gives DEACTIVATION_INTERVAL the constant value 120 (DIE05d727a7).
        const int32 DEACTIVATION_INTERVAL = 120;
        ptSSceneGlobals->uNextDeactivationTime = uNow + DEACTIVATION_INTERVAL;
    }

    GameCommonData & tGCD = GameGetActiveGameCommonData();
    QuestGlobals *& ptQuestGlobals = tGCD.ptQuestGlobals;
    if (ptQuestGlobals->bForcePlayerLocationTriggerEventAfterActivationUpdate)
    {
        ptQuestGlobals->bForcePlayerLocationTriggerEventAfterActivationUpdate = FALSE;
        // Inference: dispatch forced triggers only for valid player ACDs and existing movement objects.
        for (Player * ptPlayer = PlayerGetFirstAll(); ptPlayer; ptPlayer = PlayerGetNextAll(ptPlayer))
        {
            if (ptPlayer->idPlayerACD != ACDID_INVALID)
            {



                ActorCommonData & tACDPlayer = ACDGet(ptPlayer->idPlayerACD);
                ActorMovement * ptMove = tACDPlayer.GetActorMovement();
                if (ptMove)
                {
                    ptMove->ActivateTriggersAtLocation(FALSE);
                }
            }
        }
    }

}



void SSceneForceDeactivationCheckThisTick()
{

    SGame & tSGame = SGameManager::GameGet();

    SSceneGlobals * ptSSceneGlobals = tSGame.ptSSceneGlobals;

    ptSSceneGlobals->uNextDeactivationTime = 0;
}

// NON_MATCHING: 2836/1492 bytes; level-area loop unrolls and visibility loop does not unswitch; tried negative/positive visibility guards and nested/sentinel-continue level-area guards.
void sScenePrefetchAroundPlace(
    Player * ptPlayer,
    WorldPlace tPlace,
    SNO snoWorld)
{
    PlayerList listDestPlayers(PlayerListGetElementPool());

    if (ptPlayer)
    {
        PlayerListGetSingle(listDestPlayers, *ptPlayer);
        ServerMessageSendPrefetchWorldToPlayer(listDestPlayers, snoWorld);
    }

    SGame & tSGame = SGameManager::GameGet();
    SSceneGlobals *& ptSSceneGlobals = tSGame.ptSSceneGlobals;

    Frustum tFrustum;
    SceneCreatePlacePrefetchFrustum(tPlace, tFrustum);
    BOOL bWorldTreatAsOneScene = WorldTreatAsOneSceneBySNO(snoWorld);

    SceneCommonDataIterator tIterator;
    SceneCommonData * ptSCD = NULL;
    int32 nSceneCount = 0;
    int32 nLevelAreaCount = 0;
    while (SceneCommonDataArrayIterate(tIterator, ptSCD, SkipHolodeckScenes))
    {
        if (ptSCD->idSWorld != tPlace.idSWorld)
        { continue; }

        // Inference: unmatched worlds are skipped; visibility uses the highest surviving parent scene.

        SceneCommonData * ptSCDUsedForVisibility = ptSCD;

        while (ptSCDUsedForVisibility->idSSceneParent != SSCENEID_INVALID)
        {
            SceneCommonData * ptSCDTemp = SceneCommonDataTryToGet(ptSCDUsedForVisibility->idSSceneParent);
            if (!ptSCDTemp)
            { break; }

            ptSCDUsedForVisibility = ptSCDTemp;
        }


        // Inference: whole-world visibility bypasses the frustum test, matching the original short circuit.
        if (bWorldTreatAsOneScene || SceneIntersectsFrustum(*ptSCDUsedForVisibility, tFrustum))
        {



            if (!ptSSceneGlobals->tPrefetchedSNOs.LookupPtr(ptSCD->snoScene))
            {
                if (ptPlayer)
                {
                    ServerMessageSendPrefetchSceneToPlayer(listDestPlayers, ptSCD->snoScene);
                }

                else
                {
                    SNOPreload::PrefetchServerScene(ptSCD->snoScene, ptSCD->tSceneSpec.arSnoLevelAreas);
                }


                ptSSceneGlobals->tPrefetchedSNOs.SetAt_Tracked(ptSCD->snoScene, TRUE);
            }
            // Inference: all 4 original level-area slots are visited; SNO_NONE entries are skipped.
            for (int32 i = 0; i < 4; i++)
            {
                SNO snoLevelArea = ptSCD->tSceneSpec.arSnoLevelAreas[i];
                if (snoLevelArea == SNO_NONE)
                { continue; }
                if (!ptSSceneGlobals->tPrefetchedSNOs.LookupPtr(snoLevelArea))
                {
                    if (ptPlayer)
                    {
                        ServerMessageSendPrefetchLevelAreaToPlayer(listDestPlayers, snoLevelArea);
                    }

                    ptSSceneGlobals->tPrefetchedSNOs.SetAt_Tracked(snoLevelArea, TRUE);
                }

            }
        }
    }
    ptSSceneGlobals->tPrefetchedSNOs.RemoveAll();

}




void SScenePrefetchDestination(Player * ptPlayer, WorldPlace tPlace)
{



    SNO snoWorld = WorldCommonGetWorldSNO(tPlace.idSWorld);
    sScenePrefetchAroundPlace(ptPlayer, tPlace, snoWorld);
}


void SScenePrefetchPortalDestination(
    SWorldID idSWorld,
    Player * ptPlayer,
    uint32 uEntranceGUID)
{
    EntranceLocation tEntranceLocation;
    uint32 uGUIDToUse = uEntranceGUID;
    // Inference: missing destination world returns before locking its definition.
    SWorld * ptSWorld = SWorldTryToGet(idSWorld);
    if (!ptSWorld)
    { return; }



    SNO snoWorld = ptSWorld->tWCD.snoWorld;
    ScopedWorldDefinitionLockR ptWorldDef(snoWorld);
    // Inference: UINT32_MAX entrance GUID falls back to the first existing entrance-map association.
    if (uGUIDToUse == (uint32)-1)
    {
        POS pos = ptSWorld->tEntranceMap.GetStartAssoc();
        if (pos)
        {
            ptSWorld->tEntranceMap.GetNextAssoc(pos, uGUIDToUse, tEntranceLocation);
        }
    }
    // Inference: unused DWARF team local has no recoverable initializer; preserve its declaration.
    Team eTeam;

    if (ptPlayer)
    {
        if (ptWorldDef->uFlags & (1 << WDF_DUELING_ARENA))
        {
            uGUIDToUse = PVPGetDuelingEntranceGUID(*ptPlayer, idSWorld, FALSE);
        }

        if (GameTypeIsPvP())
        { // Inference: dead team assignment follows the parallel SWorld.cpp1589 player-team branch.
            eTeam = ptPlayer->eTeam;
            uGUIDToUse = PVPGetPlayerEntranceGUID(*ptPlayer, idSWorld);
        }
    }

    if (ptSWorld->tEntranceMap.Lookup(uGUIDToUse, tEntranceLocation))
    {
        SScene * ptSScene = SSceneTryToGet(tEntranceLocation.idSScene);
        // Inference: both valid scene and non-SNO_NONE marker-set are required by the original branches.
        if (ptSScene && tEntranceLocation.snoMarker != SNO_NONE)
        {

            ScopedMarkerSetDefinitionLockR pdefMarkerSet(tEntranceLocation.snoMarker);
            int32 nCountMarkers = pdefMarkerSet->tMarkerSet.GetArraySize();

            PRTransform transformSceneToWorld;
            SceneGetSceneToWorldTransform(ptSScene->id, transformSceneToWorld);

            for (int32 i = 0; i < nCountMarkers; i++)
            {
                const Marker & tMarker = pdefMarkerSet->tMarkerSet[i];
                if (tMarker.dwHash == tEntranceLocation.tName)
                {
                    PRTransform transform = transformSceneToWorld * tMarker.transform;
                    WorldPlace placeEntrance;
                    placeEntrance.wp = transform.wp;
                    placeEntrance.idSWorld = idSWorld;
                    SScenePrefetchDestination(ptPlayer, placeEntrance); // Inference: first matching marker ends this search in the original.
                    break;
                }
            }
        }
    }

}



void SScenePrefetchWaypointDestination(SWorldID idSWorld, Player * ptPlayer, SNO snoLevelArea)
{



    LevelAreaID idLevelArea = LevelAreaFind(snoLevelArea, idSWorld);
    if (idLevelArea != -1)
    {
        SSceneID idSScene = LevelAreaGetWaypointSSceneID(idLevelArea);

        if (idSScene != SSCENEID_INVALID)
        {

            SScene * ptSScene = SSceneTryToGet(idSScene);
            if (ptSScene != NULL && ptSScene->bHasWaypoint)
            {
                WorldPlace placeEntrance;
                placeEntrance.wp = ptSScene->transWaypoint.wp;
                placeEntrance.idSWorld = idSWorld;

                SScenePrefetchDestination(ptPlayer, placeEntrance);
            }
        }
    }
}
