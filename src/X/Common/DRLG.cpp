#include "DRLG.h"
#include "DRLGEncounters.h"
#include "DRLGTypes.h"
#include "ThreadGlobals.h"
#include "../Sigma/MemoryPools.h"
#include "../Sigma/Memory/SigmaMemory.h"
#include "../Sigma/Random.h"
void SceneSpecInitialize(SceneSpecification & tSceneSpec);  // X/Common/Definitions/WorldDefinition.cpp:622
#include "GameTemplate.h"
#include "Definitions/ActorDefinition.h"
#include "Marker/Marker.h"
#include "TagMaps/ActorTags.h"
#include "TagMaps/MarkerTags.h"
#include "TagMaps/Tags.h"
BOOL StartingLocationGUIDIsValid(const uint32 uGUID);  // X/Common/Definitions/GlobalDefinition.cpp:1511
#include "Gizmo.h"
#include "GizmoSwig.h"
#include "SceneCommon.h"
#include "../Server/Map/Minimap.h"
void SceneGetSceneToWorldTransform(SSceneID idSScene, PRTransform & transformSceneToWorld);  // X/Common/SceneCommon.cpp:1620
#include "Definitions/AdventureDefinition.h"
#include "Definitions/MarkerSetDefinition.h"
#include "../Sigma/ErrorManager/ErrorManager.h"
#include "../Sigma/Math/MathGeneral.h"
#include "../Sigma/Math/Rect.h"
#include "../Sigma/RefString.h"
void DRLGGetSceneSize(SNO snoScene, const SceneCachedValues & tCachedValues, IVector2D & pointSizeInFeet);  // X/Common/DRLGWorld.cpp:571
void DRLGGetSceneOrigin(const SceneChunk & tSceneChunk, IVector2D & pointOriginInFeet);  // X/Common/DRLGWorld.cpp:506
#include "Game.h"
#include "World.h"
#include "../Server/SGame.h"
BOOL DRLGGenerateDungeon(RNG & tRNG, SWorldID idSWorld);  // X/Server/DRLGDungeon.cpp:311
BOOL DRLGGenerateWorld(RNG & tRng, SWorldID idSWorld);  // X/Common/DRLGWorld.cpp:306
#include "Clusters.h"
void WorldCacheSceneValues(SNO snoScene, SceneCachedValues & tCachedValues);  // X/Common/World.cpp
#include "Definitions/SceneDefinition.h"
#include "Definitions/SceneGroupDefinition.h"
#include "../Server/SScene.h"
void SWorldRegisterEvent(SWorldID idSWorld, SNO snoEvent);  // X/Server/SWorld.cpp:3799
BOOL WorldIsDungeonFinderWorld(const SNO snoWorld);  // X/Common/World.cpp:886
namespace Minimap { void AddWaypointMarker(const SScene & tSScene); }  // X/Server/Map/Minimap.cpp:761
struct LooteeInfo;
BOOL ConditionEvaluate(const SNO snoCondition, const Player * ptPlayer, const ACDID idACD, const LooteeInfo * ptLooteeInfo);  // X/Common/Definitions/ConditionDefinition.cpp:640
BOOL SQuestsWorldSceneAndLabelAreRequiredByBounty(SNO snoWorld, SNO snoScene, GBID gbidLabel);  // X/Server/Quests/SQuests.cpp:5290










struct DRLGGlobals
{
    DRLGGlobals();
    ~DRLGGlobals();

    TSpawnGroupExactInstancePool tSpawnGroupExactPool;
    TPlacedScenePool tPlacedScenePool;
    TPlacedScenePtrPool tPlacedScenePtrPool;
    TDRLGSceneGroupPool tSceneGroupPool;
    TDRLGGizmoPlacePool tGizmoPlacePool;
};



DRLGGlobals::DRLGGlobals()
{

    tSpawnGroupExactPool.Initialize();
    tSpawnGroupExactPool.SetBlockCount_Tracked(128);

    tPlacedScenePool.Initialize();
    tPlacedScenePool.SetBlockCount_Tracked(128);
    tPlacedScenePool.SetGrow(XALLOC_GROW_LINEAR, 128);
    tPlacedScenePool.SetShrinkable(TRUE);

    tPlacedScenePtrPool.Initialize();
    tPlacedScenePtrPool.SetBlockCount_Tracked(128);

    tSceneGroupPool.Initialize();
    tSceneGroupPool.SetBlockCount_Tracked(128);

    tGizmoPlacePool.Initialize();
    tGizmoPlacePool.SetBlockCount_Tracked(128);


    tGizmoPlacePool.SetGrow(XALLOC_GROW_LINEAR, 128);
    tGizmoPlacePool.SetShrinkable(TRUE);
}






DRLGGlobals::~DRLGGlobals()
{
}




void DRLGDisposeGlobals()

{
    ThreadGlobals & tThreadGlobals = ThreadGlobals::Get();
    delete tThreadGlobals.m_ptDRLGGlobals; tThreadGlobals.m_ptDRLGGlobals = NULL;
}




BOOL DRLGGenerate(
    SWorldID idSWorld)
{
    WorldCommonData & tWCD = WorldGetCommonData(idSWorld);

    BOOL bUseNewDRLG;
    int32 nSeed;
    {
        ScopedWorldDefinitionLockR pdefWorld(tWCD.snoWorld);


        bUseNewDRLG = pdefWorld->bDRLG;

        if (AppIsDRLGSeedOverridden())
        {
            nSeed = AppGetOverriddenDRLGSeed();
        }
        else
        {
            nSeed = pdefWorld->ptServerData->nSeed;
        }
    }


    if (SGameIsValid())
    {
        nSeed = SWorldGet(idSWorld)->nSeed;
    }


    RNG tRNG;
    RandSetSeed_Internal(tRNG, nSeed);


    if (bUseNewDRLG)
    {
        return DRLGGenerateDungeon(tRNG, idSWorld);
    }
    else
    {

        return DRLGGenerateWorld(tRNG, idSWorld);
    }
}




static void sMarkerRecordPortalEntranceLocation(
    const SNO snoMarkerSet,
    const Marker & tMarker,
    const SSceneID idSScene,
    SWorld * ptSWorld)
{


    ScopedActorDefinitionLockR pdefActor(tMarker.snoname.snoHandle);

    uint32 uStartingLocationGUID = TagMapGetInheritedValueInt(pdefActor->hTagMap, tMarker.hTagMap, TAG_ACTOR_GIZMO_PORTAL_DEST_ENTRANCE_NUMBER, TAG_MARKER_PORTAL_DEST_ENTRANCE_NUMBER);


    if (!StartingLocationGUIDIsValid(uStartingLocationGUID))
    {
        // INFERENCE (DRLG.cpp:180): the original calls TagMapGetDWORD here and drops the result; the call survives in the line table at 180, so it is written as a bare statement.
        TagMapGetDWORD(tMarker.hTagMap, TAG_MARKER_PORTAL_DEST_DYNAMIC);
        return;
    }






    // INFERENCE (DRLG.cpp:194): GameTypeIsPvP() is called and its result unused; call survives at line 194 (probably an assert argument evaluated in the shipped build).
    // INFERENCE (DRLG.cpp:194-213): the lookup-hit path ends in the epilogue while the miss path's stores sit at 209-213, so the hit path returns early instead of an else.
    EntranceLocation tEntranceLocation;
    if (ptSWorld->tEntranceMap.Lookup(uStartingLocationGUID, tEntranceLocation))
    {
        GameTypeIsPvP();
        return;
    }












    tEntranceLocation.idSScene = idSScene;
    tEntranceLocation.snoMarker = snoMarkerSet;
    tEntranceLocation.tName = tMarker.dwHash;

    ptSWorld->SetEntranceLocation(uStartingLocationGUID, tEntranceLocation);
}




static void sMarkerRecordPortalEntrancesFromAdventure(
    const SNO snoAdventure,
    const GizmoLocPlacement & tPlacement,
    const SSceneID idSScene,
    SWorld & tSWorld)
{
    ScopedAdventureDefinitionLockR pdefAdventure(snoAdventure);
    ScopedMarkerSetDefinitionLockR ptMarkerSet(pdefAdventure->snoMarkerSet);
    int nCount = ptMarkerSet->tMarkerSet.GetArraySize();
    for (int32 i = 0; i < nCount; i++)
    {
        const Marker & tMarker = ptMarkerSet->tMarkerSet[i];
        if (tMarker.snoname.snoGroup == ACTOR_SNO)
        {
            ScopedActorDefinitionLockR pdefActor(tMarker.snoname.snoHandle);
            ActorType eActorType = pdefActor->eType;
            if (eActorType == ACTORTYPE_GIZMO)
            {
                GizmoType eGizmoType = (GizmoType)TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_GIZMO_TYPE);
                if (eGizmoType == ACTORGIZMO_SAVEPOINT_PORTAL || eGizmoType == ACTORGIZMO_PORTAL_DESTINATION)
                {

                    sMarkerRecordPortalEntranceLocation(pdefAdventure->snoMarkerSet, tMarker, idSScene, &tSWorld);
                }



                if (GameIsOpenWorld())
                {
                    ScopedActorDefinitionLockR pdefActor2(tMarker.snoname.snoHandle);
                    SNO snoChallenge = TagMapGetSNO(pdefActor2->hTagMap, TAG_ACTOR_GIZMO_CHALLENGE_SNO);
                    if (snoChallenge != -1)
                    {
                        SWorldRegisterEvent(tSWorld.id, snoChallenge);

                        PRTransform transformSceneToWorld;
                        SceneGetSceneToWorldTransform(idSScene, transformSceneToWorld);

                        PRTransform transform = tPlacement.tPlace.transform;
                        transform = transformSceneToWorld * transform;

                        WorldPlace tPlace;
                        tPlace.idSWorld = tSWorld.id;
                        tPlace.wp = transform.wp;

                        Minimap::AddSNOLocationMarker(tMarker.snoname, tPlace, idSScene);
                    }
                }
            }

        }
    }
}



void MarkerStartMinimapMarkersFromAdventure(
    const SNO snoAdventure,
    const SSceneID idSScene,
    const PRTransform & transform)
{
    SceneCommonData * ptSCD = SceneCommonDataTryToGet(idSScene);
    if (!ptSCD)
    {
        return;
    }




    ScopedAdventureDefinitionLockR pdefAdventure(snoAdventure);
    ScopedMarkerSetDefinitionLockR ptMarkerSet(pdefAdventure->snoMarkerSet);
    int nCount = ptMarkerSet->tMarkerSet.GetArraySize();
    for (int32 i = 0; i < nCount; i++)
    {
        const Marker & tMarker = ptMarkerSet->tMarkerSet[i];
        if (TagMapGetInt(tMarker.hTagMap, TAG_MARKER_MINIMAP_FORCE_START_WITH_WORLD))
        {
            PRTransform transMarker = transform * tMarker.transform;
            Minimap::AddMarker(tMarker, ptSCD, &transMarker);
        }
    }
}


static void sMarkerRecordSavePortalLocation(
    const SNO snoMarkerSet,
    const Marker & tMarker,
    const SSceneID idSScene,
    SWorld * ptSWorld)
{









    int32 nSavePointNum = TagMapGetDWORD(tMarker.hTagMap, TAG_MARKER_SAVEPOINT_NUMBER);

    SavePointLocation tLocation;
    if (!ptSWorld->tSavePortalMap.Lookup(nSavePointNum, tLocation))
    {














        tLocation.snoMarker = snoMarkerSet;
        tLocation.tName = tMarker.dwHash;

        PRTransform transformSceneToWorld;
        SceneGetSceneToWorldTransform(idSScene, transformSceneToWorld);
        tLocation.placeSavePoint.wp = transformSceneToWorld * tMarker.transform.wp;
        tLocation.placeSavePoint.idSWorld = ptSWorld->id;

        ptSWorld->tSavePortalMap.SetAt_Tracked(nSavePointNum, tLocation);
    }
}




static void sMarkerRecordPortalLocation(
    const SNO snoMarkerSet,
    const Marker & tMarker,
    const SceneCommonData * ptSCD,
    SWorld * ptSWorld)
{
    if (tMarker.eType != MARKERTYPE_ACTOR)
    {
        return;
    }


    GizmoType eGizmoType = ActorGetGizmoType(tMarker.snoname.snoHandle);
    switch (eGizmoType)
    {
    case ACTORGIZMO_PORTAL_DESTINATION:
    case ACTORGIZMO_SAVEPOINT_PORTAL:
        sMarkerRecordPortalEntranceLocation(snoMarkerSet, tMarker, ptSCD->idSScene, ptSWorld);
        break;

    case ACTORGIZMO_PORTAL:
    case ACTORGIZMO_PAGE_OF_FATE_PORTAL:
    case ACTORGIZMO_SECRET_PORTAL:
    case ACTORGIZMO_BOSS_PORTAL:
    case ACTORGIZMO_DUNGEON_PORTAL:
    case ACTORGIZMO_RETURN_PORTAL:
        Minimap::AddPortalMarker(snoMarkerSet, tMarker, ptSCD, NULL);
        break;
    case ACTORGIZMO_SAVEPOINT:

        sMarkerRecordSavePortalLocation(snoMarkerSet, tMarker, ptSCD->idSScene, ptSWorld);
        break;
    default: break; }
}

// INFERENCE (DRLG.cpp:432-438): both inlined copies loop from line 432 back to the tag tests at 404 after SSceneGet at 435,
// which is this function calling itself on the parent scene (a tail call turned into a loop); 438 is reached only for a root scene.
// INFERENCE (DRLG.cpp:460): the third TagMapGetFloat result is stored to fWaypointMinRange (0x3a8) again, not fWaypointDiscoveryRange.
static void sAddWaypointToScene(
    SScene & tSScene,
    const PRTransform & transWaypoint,
    const Marker * ptMarker)
{






    if (ptMarker)
    {
        if (TagMapGetInt(ptMarker->hTagMap, TAG_MARKER_DISABLED_IN_DUNGEON_FINDER) &&
            WorldIsDungeonFinderPlaceHolder(WorldCommonGetWorldSNO(tSScene.tSCD.idSWorld)))
        {
            return;
        }


        if (TagMapGetInt(ptMarker->hTagMap, TAG_MARKER_ENABLED_IN_DUNGEON_FINDER) &&
            !WorldIsDungeonFinderPlaceHolder(WorldCommonGetWorldSNO(tSScene.tSCD.idSWorld)))
        {
            return;
        }


        if (TagMapGetInt(ptMarker->hTagMap, TAG_MARKER_DISABLED_IN_OPEN_WORLD) &&
            GameIsOpenWorld())
        {
            return;
        }

        if (TagMapGetInt(ptMarker->hTagMap, TAG_MARKER_DISABLED_IN_STORY_MODE) &&
            GameIsStoryMode())
        {
            return;
        }
    }


    if (tSScene.tSCD.idSSceneParent != -1)
    {

        SScene * ptSSceneParent = SSceneGet(tSScene.tSCD.idSSceneParent);
        sAddWaypointToScene(*ptSSceneParent, transWaypoint, ptMarker);
    }
    else if (!tSScene.bHasWaypoint)
    {






        tSScene.bHasWaypoint = TRUE;
        tSScene.transWaypoint = transWaypoint;
        tSScene.snoWaypointTexture = -1;

        if (ptMarker)
        {
            SNO snoOverride = TagMapGetSNO(ptMarker->hTagMap, TAG_MARKER_MINIMAP_ICON);
            if (SNOFileExists(TEXTURE_SNO, snoOverride) && snoOverride != GlobalSNOGet(GLOBALSNO_TEXURE_MARKER_DEST_ICON))
            {
                tSScene.snoWaypointTexture = snoOverride;
            }

            tSScene.fWaypointMaxRange = TagMapGetFloat(ptMarker->hTagMap, TAG_MARKER_MAX_MINIMAP_RANGE);
            tSScene.fWaypointMinRange = TagMapGetFloat(ptMarker->hTagMap, TAG_MARKER_MIN_MINIMAP_RANGE);
            tSScene.fWaypointMinRange = TagMapGetFloat(ptMarker->hTagMap, TAG_MARKER_MINIMAP_DISCOVERY_RADIUS);
        }
        else
        {
            tSScene.fWaypointMaxRange = 0.0f;
            tSScene.fWaypointMinRange = 0.0f;
            tSScene.fWaypointDiscoveryRange = 0.0f;
        }

        Minimap::AddWaypointMarker(tSScene);
    }
}



// INFERENCE (DRLG.cpp:504): the "Error: scene chunk" path formats sError and never uses it; DisplayInternalError gets "" like the other fatal paths.

BOOL SWorldPresetMarkerIsInScene(
    SSceneID idSScene,
    const Vector3D & wpPosition)
{
    BOOL bInScene = FALSE;
    SceneCommonData * ptSCD = &SceneCommonDataGet(idSScene);

    IRect2D rectSceneBounds;

    int32 nSceneChunk = ptSCD->tSceneSpec.nSceneChunk;
    SNO snoPresetWorld = ptSCD->tSceneSpec.snoPresetWorld;

    ScopedWorldDefinitionLockR pdefWorld(snoPresetWorld);


    const SceneParams & tScene = *pdefWorld->ptServerData->ptScene;

    if (nSceneChunk < 0 || nSceneChunk >= tScene.nSceneChunkCount)
    {
        CRefString sError, sScene, sWorld;

        sScene = SNOToString(SCENE_SNO, ptSCD->snoScene, FALSE);
        sWorld = SNOToString(WORLD_SNO, snoPresetWorld, FALSE);

        sError.Format("Error: scene chunk (%d) out of bounds 0 to %d for scene '%s' in world '%s'\n", nSceneChunk, tScene.nSceneChunkCount, sScene.str(), sWorld.str());


        DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }
    const SceneChunk & tSceneChunk = tScene.ptSceneChunks[nSceneChunk];

    IVector2D pointSize;
    DRLGGetSceneSize(ptSCD->snoScene, tSceneChunk.tSceneSpec.tCachedValues, pointSize);

    IVector2D pointOrigin;
    DRLGGetSceneOrigin(tSceneChunk, pointOrigin);


    rectSceneBounds.left = pointOrigin.x;
    rectSceneBounds.top = pointOrigin.y;
    rectSceneBounds.right = pointOrigin.x + pointSize.x;
    rectSceneBounds.bottom = pointOrigin.y + pointSize.y;

    IVector2D pointMarker;
    pointMarker.x = Math::FloatRoundToInt(wpPosition.x);
    pointMarker.y = Math::FloatRoundToInt(wpPosition.y);

    if (rectSceneBounds.ContainsPoint(pointMarker))
    {
        bInScene = TRUE;
    }


    return bInScene;
}





static void sSWorldFindMapMarkersInPresetMarkerSet(
    SWorld *,
    SceneCommonData * ptSCD,
    SNO snoMarkerSet)
{
    ScopedMarkerSetDefinitionLockR pdefMarkerSet(snoMarkerSet);












    int32 nCountMarkers = pdefMarkerSet->tMarkerSet.GetArraySize();

    for (int32 i = 0; i < nCountMarkers; i++)
    {
        const Marker & tMarker = pdefMarkerSet->tMarkerSet[i];

        if (tMarker.eType == MARKERTYPE_MINIMAP)
        {



            if (SWorldPresetMarkerIsInScene(ptSCD->idSScene, gtransformMarkerToScene * tMarker.transform.wp))
            {



                Minimap::AddMarker(tMarker, ptSCD, NULL);
            }
        }
    }
}



static void sSWorldFindPortalsInPresetMarkerSet(
    SWorld * ptSWorld,
    SceneCommonData * ptSCD,
    SNO snoMarkerSet)
{
    ScopedMarkerSetDefinitionLockR pdefMarkerSet(snoMarkerSet);










    int32 nCountMarkers = pdefMarkerSet->tMarkerSet.GetArraySize();

    for (int32 i = 0; i < nCountMarkers; i++)
    {
        const Marker & tMarker = pdefMarkerSet->tMarkerSet[i];

        if (SWorldPresetMarkerIsInScene(ptSCD->idSScene, gtransformMarkerToScene * tMarker.transform.wp))
        {




            sMarkerRecordPortalLocation(snoMarkerSet, tMarker, ptSCD, ptSWorld);
        }
    }
}



static void sWorldFindMapMarkersInPreset(
    SWorld * ptSWorld,
    SceneCommonData * ptSCD)
{
    SNO snoPresetWorld = ptSCD->tSceneSpec.snoPresetWorld;

    if (snoPresetWorld == -1)
    {
        return;
    }

    ScopedWorldDefinitionLockR pdefWorld(snoPresetWorld);

    int32 nCount = pdefWorld->arMarkerSets.GetArraySize();

    for (int32 i = 0; i < nCount; i++)
    {
        SNO snoMarkerSet = pdefWorld->arMarkerSets[i];

        sSWorldFindMapMarkersInPresetMarkerSet(ptSWorld, ptSCD, snoMarkerSet);
    }
}





static void sSWorldFindPortalsInPreset(
    SWorld * ptSWorld,
    SceneCommonData * ptSCD)
{
    SNO snoPresetWorld = ptSCD->tSceneSpec.snoPresetWorld;

    if (snoPresetWorld == -1)
    {
        return;
    }

    ScopedWorldDefinitionLockR pdefWorld(snoPresetWorld);

    int32 nCount = pdefWorld->arMarkerSets.GetArraySize();

    for (int32 i = 0; i < nCount; i++)
    {
        SNO snoMarkerSet = pdefWorld->arMarkerSets[i];

        sSWorldFindPortalsInPresetMarkerSet(ptSWorld, ptSCD, snoMarkerSet);
    }
}





static void sProcessSpecialMarkersInScene(
    SWorld & tSWorld,
    SceneCommonData & tSCD)
{
    SScene * ptSScene = SSceneGet(tSCD.idSScene);
    PRTransform transformSceneToWorld;
    SceneGetSceneToWorldTransform(tSCD.idSScene, transformSceneToWorld);

    ScopedSceneDefinitionLockR ptSceneDef(tSCD.snoScene);
    int32 nMarkerSets = ptSceneDef->arMarkerSets.GetArraySize();
    for (int32 i = 0; i < nMarkerSets; i++)
    {
        SNO snoMarkerSet = ptSceneDef->arMarkerSets[i];
        ScopedMarkerSetDefinitionLockR pdefMarkerSet(snoMarkerSet);
        if (pdefMarkerSet->nSpecialIndexCount > 0)
        {
            if (pdefMarkerSet->nLabel == -1 ||
                tSCD.listAppliedLabels.Find(pdefMarkerSet->nLabel))
            {













                for (int32 j = 0; j < pdefMarkerSet->nSpecialIndexCount; j++)
                {

                    int32 nIndex = pdefMarkerSet->pwSpecialIndexList[j];

                    const Marker & tMarker = pdefMarkerSet->tMarkerSet[nIndex];





                    if (TagMapGetInt(tMarker.hTagMap, TAG_MARKER_ENABLED_IN_DUNGEON_FINDER) &&
                        !WorldIsDungeonFinderWorld(tSWorld.tWCD.snoWorld))
                    {
                        continue;
                    }

                    if (TagMapGetInt(tMarker.hTagMap, TAG_MARKER_DISABLED_IN_DUNGEON_FINDER) &&
                        WorldIsDungeonFinderWorld(tSWorld.tWCD.snoWorld))
                    {
                        continue;
                    }

                    if (TagMapGetInt(tMarker.hTagMap, TAG_MARKER_DISABLED_IN_OPEN_WORLD) &&
                        GameIsOpenWorld())
                    {
                        continue;
                    }

                    if (TagMapGetInt(tMarker.hTagMap, TAG_MARKER_DISABLED_IN_STORY_MODE) &&
                        GameIsStoryMode())
                    {
                        continue;
                    }


                    sMarkerRecordPortalLocation(snoMarkerSet, tMarker, &tSCD, &tSWorld);
                    if (tMarker.eType == MARKERTYPE_MINIMAP)
                    {
                        Minimap::AddMarker(tMarker, &tSCD, NULL);
                    }

                    if (tMarker.eType == MARKERTYPE_EVENT)
                    {
                        SWorldRegisterEvent(tSWorld.id, tMarker.snoname.snoHandle);

                        PRTransform transformSceneToWorld2;
                        SceneGetSceneToWorldTransform(tSCD.idSScene, transformSceneToWorld2);

                        PRTransform finalMarkerTransform;
                        finalMarkerTransform = transformSceneToWorld2 * tMarker.transform;

                        WorldPlace tPlace;
                        tPlace.idSWorld = tSCD.idSWorld;
                        tPlace.wp = finalMarkerTransform.wp;

                        Minimap::AddSNOLocationMarker(tMarker.snoname, tPlace, tSCD.idSScene);
                    }

                    if (tMarker.eType == MARKERTYPE_ACTOR)
                    {

                        if (GameIsOpenWorld())
                        {
                            ScopedActorDefinitionLockR pdefActor(tMarker.snoname.snoHandle);
                            SNO snoChallenge = TagMapGetSNO(pdefActor->hTagMap, TAG_ACTOR_GIZMO_CHALLENGE_SNO);

                            if (snoChallenge != -1)
                            {
                                SWorldRegisterEvent(tSWorld.id, snoChallenge);

                                PRTransform transformSceneToWorld2;
                                SceneGetSceneToWorldTransform(tSCD.idSScene, transformSceneToWorld2);

                                PRTransform finalMarkerTransform;
                                finalMarkerTransform = transformSceneToWorld2 * tMarker.transform;

                                WorldPlace tPlace;
                                tPlace.idSWorld = tSCD.idSWorld;
                                tPlace.wp = finalMarkerTransform.wp;

                                Minimap::AddSNOLocationMarker(tMarker.snoname, tPlace, tSCD.idSScene);
                            }
                        }
                    }

                    if (tMarker.snoname.snoGroup == ACTOR_SNO)
                    {
                        GizmoType eType = ActorGetGizmoType(tMarker.snoname.snoHandle);
                        if (eType == ACTORGIZMO_WAYPOINT)
                        {
                            PRTransform finalMarkerTransform;
                            finalMarkerTransform = transformSceneToWorld * tMarker.transform;

                            sAddWaypointToScene(*ptSScene, finalMarkerTransform, &tMarker);
                        }
                    }

                    if (tMarker.snoname.snoGroup == ACTOR_SNO &&
                        ActorGetGizmoType(tMarker.snoname.snoHandle) == ACTORGIZMO_HEARTH_PORTAL)
                    {
                        PRTransform transformSceneToWorld2;
                        SceneGetSceneToWorldTransform(tSCD.idSScene, transformSceneToWorld2);

                        PRTransform finalMarkerTransform;
                        finalMarkerTransform = transformSceneToWorld2 * tMarker.transform;

                        tSWorld.tTownPortalLocation.snoScene = tSCD.snoScene;
                        tSWorld.tTownPortalLocation.wpPortal.idSWorld = tSCD.idSWorld;
                        tSWorld.tTownPortalLocation.wpPortal.wp = finalMarkerTransform.wp;
                    }
                }
            }
        }
    }

    POS pos = tSCD.listGizmoLocPlacements.GetHeadPosition();
    while (pos)
    {
        GizmoLocPlacement & tPlacement = tSCD.listGizmoLocPlacements.GetNext(pos);
        const SNOName & snoname = tPlacement.snoname;
        switch (snoname.snoGroup) {
        case ACTOR_SNO: {
            ScopedActorDefinitionLockR pdefActor(snoname.snoHandle);
            ActorType eActorType = pdefActor->eType;
            if (eActorType == ACTORTYPE_GIZMO)
            {
                GizmoType eGizmoType = (GizmoType)TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_GIZMO_TYPE);
                if (eGizmoType == ACTORGIZMO_WAYPOINT)
                {
                    PRTransform transform = transformSceneToWorld * tPlacement.tPlace.transform;
                    sAddWaypointToScene(*ptSScene, transform, NULL);
                }
            }
        }
            break;
        case ADVENTURE_SNO:
            sMarkerRecordPortalEntrancesFromAdventure(snoname.snoHandle, tPlacement, tSCD.idSScene, tSWorld);
            break;
        }
    }
}



void SWorldHandleSpecialMarkersInWorld(
    SWorldID idSWorld)
{

    SWorld * ptSWorld = SWorldGet(idSWorld);


    ptSWorld->tNoSpawnTree.Populate(*ptSWorld);

    SceneCommonData * ptSCD = NULL;
    SceneCommonDataIterator tIterator;
    while (SceneCommonDataArrayIterate(tIterator, ptSCD, SkipHolodeckScenes))
    {
        if (ptSCD->idSWorld != idSWorld)
        {
            continue;
        }

        sProcessSpecialMarkersInScene(*ptSWorld, *ptSCD);

        sSWorldFindPortalsInPreset(ptSWorld, ptSCD);
        sWorldFindMapMarkersInPreset(ptSWorld, ptSCD);
    }
}





TSpawnGroupExactInstancePool * DRLGGetSpawnGroupExactPool()

{
    ThreadGlobals & tThreadGlobals = ThreadGlobals::Get();
    if (!tThreadGlobals.m_ptDRLGGlobals)
    {

        tThreadGlobals.m_ptDRLGGlobals = new DRLGGlobals;
    }

    return &tThreadGlobals.m_ptDRLGGlobals->tSpawnGroupExactPool;
}



TDRLGSceneGroupPool * DRLGGetSceneGroupPool()

{
    ThreadGlobals & tThreadGlobals = ThreadGlobals::Get();
    if (!tThreadGlobals.m_ptDRLGGlobals)
    {

        tThreadGlobals.m_ptDRLGGlobals = new DRLGGlobals;
    }

    return &tThreadGlobals.m_ptDRLGGlobals->tSceneGroupPool;
}



TDRLGGizmoPlacePool * DRLGGetGizmoPlacePool()

{
    ThreadGlobals & tThreadGlobals = ThreadGlobals::Get();
    if (!tThreadGlobals.m_ptDRLGGlobals)
    {

        tThreadGlobals.m_ptDRLGGlobals = new DRLGGlobals;
    }

    return &tThreadGlobals.m_ptDRLGGlobals->tGizmoPlacePool;
}



TPlacedScenePool * DRLGGetPlacedScenePool()

{
    ThreadGlobals & tThreadGlobals = ThreadGlobals::Get();
    if (!tThreadGlobals.m_ptDRLGGlobals)
    {

        tThreadGlobals.m_ptDRLGGlobals = new DRLGGlobals;
    }

    return &tThreadGlobals.m_ptDRLGGlobals->tPlacedScenePool;
}



TPlacedScenePtrPool * DRLGGetPlacedScenePtrPool()

{
    ThreadGlobals & tThreadGlobals = ThreadGlobals::Get();
    if (!tThreadGlobals.m_ptDRLGGlobals)
    {

        tThreadGlobals.m_ptDRLGGlobals = new DRLGGlobals;
    }

    return &tThreadGlobals.m_ptDRLGGlobals->tPlacedScenePtrPool;
}



DRLGPlacedScene::DRLGPlacedScene()
{
    SigmaMemoryClear(this, sizeof(DRLGPlacedScene));

    snoname.Set(SCENE_SNO, -1);
    nTileType = 0;
    nTileSet = 0;
    posParentScene = 0;

    snoSceneGroup = -1;
    idSScene = -1;
    snoMarkerSetSrc = -1;
    nMarkerSetSrcIndex = -1;

    nSpecialTypesPlaced = 0;

    listAppliedLabels.SetPool(MemoryPoolsGetDATAIDListPool());
    listPotentialLabels.SetPool(MemoryPoolsGetDATAIDListPool());
}

const DRLGPlacedScene & DRLGPlacedScene::operator=(
    const DRLGPlacedScene & tSrc)
{
    tSceneSpec = tSrc.tSceneSpec;
    wp = tSrc.wp;
    snoname = tSrc.snoname;
    nTileType = tSrc.nTileType;
    nTileSet = tSrc.nTileSet;
    posParentScene = tSrc.posParentScene;
    snoSceneGroup = tSrc.snoSceneGroup;
    idSScene = tSrc.idSScene;
    snoMarkerSetSrc = tSrc.snoMarkerSetSrc;
    nMarkerSetSrcIndex = tSrc.nMarkerSetSrcIndex;

    listAppliedLabels.SetPool(MemoryPoolsGetDATAIDListPool());
    POS pos = tSrc.listAppliedLabels.GetHeadPosition();
    while (pos)
    {
        GBID gbidLabel = tSrc.listAppliedLabels.GetNext(pos);
        listAppliedLabels.AddTail_Tracked(gbidLabel);
    }

    listPotentialLabels.SetPool(MemoryPoolsGetDATAIDListPool());
    pos = tSrc.listPotentialLabels.GetHeadPosition();
    while (pos)
    {
        GBID gbidLabel = tSrc.listPotentialLabels.GetNext(pos);
        listPotentialLabels.AddTail_Tracked(gbidLabel);
    }

    return *this;
}


DRLGGenerator::DRLGGenerator()
{
    m_ptRNG = NULL;
    m_snoWorld = -1;
    m_idSWorld = -1;
    SigmaMemoryClear(&m_tDefaultSceneSpec, sizeof(SceneSpecification));
    m_dwSeed = 0;
}


void DRLGGenerator::Initialize(
    RNG & tRNG,
    SNO snoWorld,
    SWorldID idSWorld)
{
    m_ptRNG = &tRNG;
    m_dwSeed = RandGetSeed(tRNG);

    m_snoWorld = snoWorld;
    m_idSWorld = idSWorld;

    m_listPlacedScenes.SetPool(DRLGGetPlacedScenePool());
    m_listSpawnGroupExact.SetPool(DRLGGetSpawnGroupExactPool());

    {
        ScopedWorldDefinitionLockR pdefWorld(snoWorld);
        const Environment * ptEnvironment = &pdefWorld->tEnvironment;

        SceneSpecInitialize(m_tDefaultSceneSpec);
        m_tDefaultSceneSpec.snoMusic = ptEnvironment->snoMusic;
        m_tDefaultSceneSpec.snoAmbient = ptEnvironment->snoAmbient;
        m_tDefaultSceneSpec.snoReverb = ptEnvironment->snoReverb;
        m_tDefaultSceneSpec.snoWeather = ptEnvironment->snoWeather;
    }

    InitSpecific();
}
// INFERENCE (DRLG.cpp:1055): the original compares i with nRandChoose using cmp/b.lt at 1055, so the test is i >= nRandChoose rather than ==.
BOOL DRLGGenerator::ApplyClusterSubScene(SubSceneEntry & tSubSceneEntry, TPlacedSceneList & listSubScenes, GBID gbidForceLabel)
{
    int32 nTotalCount = listSubScenes.Count();
    if (nTotalCount <= 0)
    {
        return FALSE;
    }

    int32 nRandChoose = RandRange_Internal(*m_ptRNG, 0, nTotalCount - 1);
    POS pos = listSubScenes.GetHeadPosition();
    for (int32 i = 0; i <= nRandChoose; i++)
    {
        POS posLast = pos;
        DRLGPlacedScene & tPlacedScene = listSubScenes.GetNext(pos);
        if (i >= nRandChoose)
        {
            WorldCacheSceneValues(tSubSceneEntry.snoScene, tPlacedScene.tSceneSpec.tCachedValues);
            tPlacedScene.snoname.snoHandle = tSubSceneEntry.snoScene;


            if (gbidForceLabel == -1)
            {
                int32 nTotalWeight = 0;
                for (int32 nLabel = 0; nLabel < tSubSceneEntry.nNumLabels; nLabel++)
                {
                    SubSceneLabel & tLabel = tSubSceneEntry.arLabels[nLabel];
                    int32 nWeight = Math::Max(0, tLabel.nWeight);
                    nTotalWeight += nWeight;
                }

                int32 nRandWeight = RandRange_Internal(*m_ptRNG, 0, nTotalWeight);
                nTotalWeight = 0;
                for (int32 nLabel = 0; nLabel < tSubSceneEntry.nNumLabels; nLabel++)
                {
                    SubSceneLabel & tLabel = tSubSceneEntry.arLabels[nLabel];
                    int32 nWeight = Math::Max(0, tLabel.nWeight);
                    nTotalWeight += nWeight;
                    if (nRandWeight <= nTotalWeight)
                    {
                        tPlacedScene.listAppliedLabels.AddTail_Tracked(tLabel.gbidLabel);
                        break;
                    }
                }
            }
            else
            {
                tPlacedScene.listAppliedLabels.AddTail_Tracked(gbidForceLabel);
            }

            m_listPlacedScenes.AddTail_Tracked(tPlacedScene);
            listSubScenes.RemoveAt(posLast);
        }
    }

    return TRUE;
}


void DRLGGenerator::SelectDebugOverrideSubScenes(
    SceneCluster & tCluster,
    TPlacedSceneList & listSubScenes,
    int32 nOverrideIndex)
{

    SubSceneEntry * ptSubSceneEntry = NULL;
    int32 nCurrentIndexCount = 0;
    for (int32 nSubSceneGroup = 0; nSubSceneGroup < tCluster.nNumSubSceneGroups; nSubSceneGroup++)
    {
        SubSceneGroup & tSubSceneGroup = tCluster.arSubSceneGroups[nSubSceneGroup];
        int32 nLocalIndex = nOverrideIndex - nCurrentIndexCount;
        if (nLocalIndex < tSubSceneGroup.nNumSubScenes)
        {
            ptSubSceneEntry = &tSubSceneGroup.arSubScenes[nLocalIndex];
            break;
        }

        nCurrentIndexCount += tSubSceneGroup.nNumSubScenes;
    }


    if (!ptSubSceneEntry)
    {
        int32 nLocalIndex = nOverrideIndex - nCurrentIndexCount;
        if (nLocalIndex < tCluster.tDefaultSubSceneGroup.nNumSubScenes)
        {
            ptSubSceneEntry = &tCluster.tDefaultSubSceneGroup.arSubScenes[nLocalIndex];
        }
    }

    if (!ptSubSceneEntry)
    {
        return;
    }




    while (listSubScenes.Count() > 0)
    {
        BOOL bSuccess = ApplyClusterSubScene(*ptSubSceneEntry, listSubScenes, -1);
        if (!bSuccess)
        {
            break;
        }
    }
}






BOOL DRLGGenerator::PickSubScenesFromClusters()
{
    AppGlobals & tAppGlobals = sgtAppGlobals;
    ScopedWorldDefinitionLockR pdefWorld(m_snoWorld);


    for (int32 nCluster = 0; nCluster < pdefWorld->ptServerData->tSceneClusterSet.nNumClusters; nCluster++)
    {
        TPlacedSceneList listSubScenes;
        listSubScenes.SetPool(DRLGGetPlacedScenePool());

        SceneCluster & tCluster = pdefWorld->ptServerData->tSceneClusterSet.arClusters[nCluster];

        POS pos = m_listPlacedScenes.GetHeadPosition();
        while (pos)
        {
            POS posLast = pos;
            DRLGPlacedScene & tPlacedScene = m_listPlacedScenes.GetNext(pos);
            if (tPlacedScene.snoMarkerSetSrc != -1)
            {
                continue;
            }


            if (tPlacedScene.tSceneSpec.uClusterID == tCluster.uClusterID)
            {

                SNO snoScene = tPlacedScene.snoname.snoHandle;
                ScopedSceneDefinitionLockR pdefScene(snoScene);
                for (int32 nMarkerSet = 0; nMarkerSet < pdefScene->arMarkerSets.GetArraySize(); nMarkerSet++)
                {
                    SNO snoMarkerSet = pdefScene->arMarkerSets[nMarkerSet];
                    ScopedMarkerSetDefinitionLockR pdefMarkerSet(snoMarkerSet);
                    if (pdefMarkerSet->nSpecialIndexCount > 0)
                    {
                        for (int32 j = 0; j < pdefMarkerSet->nSpecialIndexCount; j++)
                        {

                            int32 nIndex = pdefMarkerSet->pwSpecialIndexList[j];

                            const Marker & tMarker = pdefMarkerSet->tMarkerSet[nIndex];
                            if (tMarker.eType == MARKERTYPE_SCENE_LOCATION)
                            {
                                DRLGPlacedScene tSubScene;

                                tSubScene.tSceneSpec = tPlacedScene.tSceneSpec;

                                tSubScene.wp = tPlacedScene.wp + tMarker.transform.wp;


                                tSubScene.wp.x -= Math::FloatMod(tSubScene.wp.x, 2.5f);
                                tSubScene.wp.y -= Math::FloatMod(tSubScene.wp.y, 2.5f);

                                tSubScene.posParentScene = posLast;
                                tSubScene.snoSceneGroup = tMarker.snoname.snoHandle;

                                tSubScene.snoMarkerSetSrc = snoMarkerSet;
                                tSubScene.nMarkerSetSrcIndex = nMarkerSet;


                                POS posLabel = tPlacedScene.listAppliedLabels.GetHeadPosition();
                                while (posLabel)
                                {
                                    GBID gbidLabel = tPlacedScene.listAppliedLabels.GetNext(posLabel);
                                    tSubScene.listAppliedLabels.AddTail_Tracked(gbidLabel);
                                }

                                listSubScenes.AddTail_Tracked(tSubScene);
                            }
                        }
                    }
                }
            }
        }

        if (tAppGlobals.nSubSceneOverride >= 0)
        {
            SelectDebugOverrideSubScenes(tCluster, listSubScenes, tAppGlobals.nSubSceneOverride);
            continue;
        }




        for (int32 nSubSceneGroup = 0; nSubSceneGroup < tCluster.nNumSubSceneGroups; nSubSceneGroup++)
        {
            SubSceneGroup & tSubSceneGroup = tCluster.arSubSceneGroups[nSubSceneGroup];

            if (tSubSceneGroup.snoCondition != -1)
            {
                if (!ConditionEvaluate(tSubSceneGroup.snoCondition, NULL, ACDID_INVALID, NULL))
                {
                    continue;
                }
            }

            XPooledDataIDList listIndexes(MemoryPoolsGetDATAIDListPool());
            for (int32 nScene = 0; nScene < tSubSceneGroup.nNumSubScenes; nScene++)
            {
                listIndexes.AddTail_Tracked(nScene);
            }

            int32 nNumToChoose = Math::Min(tSubSceneGroup.nNumScenesToChoose, tSubSceneGroup.nNumSubScenes);
            for (int32 nChoice = 0; nChoice < nNumToChoose; nChoice++)
            {
                int32 nTotalWeight = 0;


                BOOL bBountyRequirement = FALSE;


                pos = listIndexes.GetHeadPosition();
                while (pos)
                {
                    POS posLast = pos;

                    int32 nIndex = listIndexes.GetNext(pos);
                    SubSceneEntry & tSubSceneEntry = tSubSceneGroup.arSubScenes[nIndex];


                    if (GameIsOpenWorld())
                    {
                        GBID gbidBountyLabel = -1;

                        if (SQuestsWorldSceneAndLabelAreRequiredByBounty(m_snoWorld,
                            tSubSceneEntry.snoScene, gbidBountyLabel))
                        {
                            bBountyRequirement = TRUE;
                        }
                        else
                        {

                            for (int32 i = 0; i < tSubSceneEntry.nNumLabels; i++)
                            {
                                if (SQuestsWorldSceneAndLabelAreRequiredByBounty(m_snoWorld,
                                    tSubSceneEntry.snoScene,
                                    tSubSceneEntry.arLabels[i].gbidLabel))
                                {
                                    bBountyRequirement = TRUE;

                                    gbidBountyLabel = tSubSceneEntry.arLabels[i].gbidLabel;
                                    break;
                                }
                            }
                        }

                        if (bBountyRequirement)
                        {
                            ApplyClusterSubScene(tSubSceneEntry, listSubScenes, gbidBountyLabel);
                            listIndexes.RemoveAt(posLast);
                            break;
                        }
                    }

                    int32 nWeight = Math::Max(0, tSubSceneEntry.nWeight);
                    nTotalWeight += nWeight;
                }

                if (bBountyRequirement)
                {
                    continue;
                }



                int32 nRandWeight = RandRange_Internal(*m_ptRNG, 0, nTotalWeight);
                nTotalWeight = 0;
                pos = listIndexes.GetHeadPosition();
                while (pos)
                {
                    POS posLast = pos;
                    int32 nIndex = listIndexes.GetNext(pos);
                    SubSceneEntry & tSubSceneEntry = tSubSceneGroup.arSubScenes[nIndex];
                    int32 nWeight = Math::Max(0, tSubSceneEntry.nWeight);
                    nTotalWeight += nWeight;
                    if (nRandWeight <= nTotalWeight)
                    {
                        listIndexes.RemoveAt(posLast);
                        BOOL bSuccess = ApplyClusterSubScene(tSubSceneEntry, listSubScenes, -1);
                        if (!bSuccess)
                        {
                            if (listSubScenes.Count() < 1)
                            {
                                CRefString strClusterName;
                                tCluster.GetName(strClusterName);
                                // INFERENCE (DRLG.cpp:1338-1340): no instruction is attributed to these lines; whatever reported
                                // strClusterName produces no code in this build, only the CRefString ctor, GetName and dtor survive.

                            }
                        }
                        break;
                    }
                }
            }


        }


        if (tCluster.tDefaultSubSceneGroup.nNumSubScenes > 0)
        {


            while (listSubScenes.Count() > 0)
            {
                int32 nTotalWeight = 0;
                for (int32 i = 0; i < tCluster.tDefaultSubSceneGroup.nNumSubScenes; i++)
                {
                    SubSceneEntry & tSubSceneEntry = tCluster.tDefaultSubSceneGroup.arSubScenes[i];
                    int32 nWeight = Math::Max(0, tSubSceneEntry.nWeight);
                    nTotalWeight += nWeight;
                }

                int32 nRandWeight = RandRange_Internal(*m_ptRNG, 0, nTotalWeight);
                nTotalWeight = 0;
                for (int32 i = 0; i < tCluster.tDefaultSubSceneGroup.nNumSubScenes; i++)
                {
                    SubSceneEntry & tSubSceneEntry = tCluster.tDefaultSubSceneGroup.arSubScenes[i];
                    int32 nWeight = Math::Max(0, tSubSceneEntry.nWeight);
                    nTotalWeight += nWeight;
                    if (nRandWeight <= nTotalWeight)
                    {
                        BOOL bSuccess = ApplyClusterSubScene(tSubSceneEntry, listSubScenes, -1);
                        if (!bSuccess)
                        {
                            return FALSE;
                        }
                        break;
                    }
                }
            }
        }







    }

    return TRUE;
}
// INFERENCE (DRLG.cpp:1404): bHasOverrides is folded to the constant 0 over the whole function (DWARF location list) and no instruction
// uses it, so it is declared FALSE and never read.
BOOL DRLGGenerator::PickSubScenes()
{




    BOOL bHasOverrides = FALSE;










    BOOL bSuccess = PickSubScenesFromClusters();
    if (!bSuccess)
    {
        return FALSE;
    }

    POS pos = m_listPlacedScenes.GetHeadPosition();
    while (pos)
    {

        if (!PickSubSceneForPlacedScene(pos, TRUE))
        {
            return FALSE;
        }
        m_listPlacedScenes.GetNext(pos);
    }






    return TRUE;
}

// INFERENCE (DRLG.cpp:1486): the original zeroes nTotalWeight on entry to the fallback loop (mov w22,wzr at 0x42e6dc, scheduled under XBaseList.h:635), so the Count() < 1 block resets it.
BOOL DRLGGenerator::PickSubSceneForPlacedScene(POS posParentScene, BOOL bAbortOnError)
{
    DRLGPlacedScene & tPlacedScene = m_listPlacedScenes.GetAt(posParentScene);
    if (tPlacedScene.snoMarkerSetSrc != -1)
    {
        return TRUE;
    }



    ScopedSceneDefinitionLockR ptSceneDef(tPlacedScene.snoname.snoHandle);
    int32 nMarkerSets = ptSceneDef->arMarkerSets.GetArraySize();
    for (int32 i = 0; i < nMarkerSets; i++)
    {
        SNO snoMarkerSet = ptSceneDef->arMarkerSets[i];
        ScopedMarkerSetDefinitionLockR pdefMarkerSet(snoMarkerSet);
        if (pdefMarkerSet->nSpecialIndexCount > 0)
        {
            for (int32 j = 0; j < pdefMarkerSet->nSpecialIndexCount; j++)
            {

                int32 nIndex = pdefMarkerSet->pwSpecialIndexList[j];

                const Marker & tMarker = pdefMarkerSet->tMarkerSet[nIndex];
                if (tMarker.eType == MARKERTYPE_SCENEGROUP)
                {
                    ScopedSceneGroupDefinitionLockR ptSceneGroupDef(tMarker.snoname.snoHandle);


                    XPooledDataIDList listScenes(MemoryPoolsGetDATAIDListPool());
                    int32 nTotalWeight = 0;
                    for (int32 nScene = 0; nScene < ptSceneGroupDef->nSceneCnt; nScene++)
                    {
                        SceneGroupItem & tSceneGroupItem = ptSceneGroupDef->arScenes[nScene];
                        if (tSceneGroupItem.gbidLabel != -1 && tPlacedScene.listAppliedLabels.Find(tSceneGroupItem.gbidLabel))
                        {

                            int32 nWeight = Math::Max(0, tSceneGroupItem.nWeight);
                            nTotalWeight += nWeight;
                            listScenes.AddTail_Tracked(nScene);
                        }
                    }

                    if (listScenes.Count() < 1)
                    {
                        nTotalWeight = 0;

                        for (int32 nScene = 0; nScene < ptSceneGroupDef->nSceneCnt; nScene++)
                        {
                            SceneGroupItem & tSceneGroupItem = ptSceneGroupDef->arScenes[nScene];
                            if (tSceneGroupItem.gbidLabel == -1)
                            {
                                int32 nWeight = Math::Max(0, tSceneGroupItem.nWeight);
                                nTotalWeight += nWeight;
                                listScenes.AddTail_Tracked(nScene);
                            }
                        }
                    }


                    if (bAbortOnError && listScenes.Count() < 1)
                    {
                        return FALSE;
                    }






                    int32 nRandWeight = RandRange_Internal(*m_ptRNG, 0, nTotalWeight);
                    nTotalWeight = 0;
                    POS pos = listScenes.GetHeadPosition();
                    SNO snoSceneChosen = -1;
                    while (pos)
                    {
                        int32 nScene = listScenes.GetNext(pos);


                        SceneGroupItem & tSceneGroupItem = ptSceneGroupDef->arScenes[nScene];
                        nTotalWeight += tSceneGroupItem.nWeight;
                        if (nRandWeight <= nTotalWeight)
                        {
                            snoSceneChosen = tSceneGroupItem.snoScene;
                            break;
                        }
                    }

                    if (snoSceneChosen != -1)
                    {
                        DRLGPlacedScene tSubScene;


                        tSubScene.tSceneSpec = tPlacedScene.tSceneSpec;
                        WorldCacheSceneValues(snoSceneChosen, tSubScene.tSceneSpec.tCachedValues);
                        tSubScene.wp = tPlacedScene.wp + tMarker.transform.wp;


                        tSubScene.wp.x -= Math::FloatMod(tSubScene.wp.x, 2.5f);
                        tSubScene.wp.y -= Math::FloatMod(tSubScene.wp.y, 2.5f);
                        tSubScene.posParentScene = posParentScene;
                        tSubScene.snoSceneGroup = tMarker.snoname.snoHandle;
                        tSubScene.snoname.snoHandle = snoSceneChosen;
                        tSubScene.snoMarkerSetSrc = snoMarkerSet;
                        tSubScene.nMarkerSetSrcIndex = i;



                        POS posLabel = tPlacedScene.listAppliedLabels.GetHeadPosition();
                        while (posLabel)
                        {
                            GBID gbidLabel = tPlacedScene.listAppliedLabels.GetNext(posLabel);
                            tSubScene.listAppliedLabels.AddTail_Tracked(gbidLabel);
                        }


                        m_listPlacedScenes.AddTail_Tracked(tSubScene);




                    }
                    else if (bAbortOnError)
                    {
                        return FALSE;
                    }





                }
            }
        }
    }

    return TRUE;
}
