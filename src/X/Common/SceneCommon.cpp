// Original file: X/Common/SceneCommon.cpp
// Functions sit on their original lines (see the reference .cpp). All of the file's functions are written; SceneGetDeactivationReasonsString (line 3213) is a linker-removed function reconstructed from its DWARF record (see the note above it).

#include "SceneCommon.h"
#include "WorldPlace.h"
#include "Nav/NavigationMesh.h"
#include "../Sigma/DataStructures/XChainedFixedBlockAllocator.h"
#include "Game.h"
#include "World.h"
#include "Nav/NavMeshStitch.h"
#include "Nav/NavZone.h"
#include "ActorCommonData.h"
#include "Player.h"
#include "Definitions/LevelAreaDefinition.h"
#include "Definitions/SceneDefinition.h"
#include "../Client/Scene.h"
#include "../Server/SScene.h"
#include "../Sigma/Math/Intersect.h"
#include "../Sigma/Math/Frustum.h"

// Declared here from their definitions elsewhere (headers not adopted yet).
SScene * SSceneGet(SSceneID id);  // X/Server/SScene.cpp:393
BOOL SSceneDataArrayIterate(TDataArrayIterator<SScene> & tIterator);  // X/Server/SScene.cpp:380
CScene * CSceneGetFromSSceneID(SSceneID idSScene);  // X/Client/Scene.cpp:3568
BOOL CSceneDataArrayIterate(TDataArrayIterator<CScene> & tIterator, const CGame & tCGame, CSceneDataArrayIterateMode eIterateMode);  // X/Client/Scene.cpp:3538
SSceneID NavMeshHandleToSubSceneSceneID(const NavMeshHandle & tNavMeshHandle);  // X/Common/Nav/NavigationMesh.cpp:3892
void SceneCommonDataDeactivate(SSceneID idSScene);  // SceneCommon.cpp:1900s, defined later in this file
void NavMeshUnstitchScene(SSceneID idSScene);  void NavMeshStitchScene(SSceneID idSScene);  // X/Common/Nav/NavMeshStitch.cpp
void InactiveMarkerDisposeForScene(SSceneID idSScene);  void InactiveMarkerInitializeForScene(SSceneID idSScene);  // X/Common/InactiveMarker.cpp
static void sSceneCommonDataDisposeNavMesh(SSceneID idSScene);  // defined later in the original file
int32 NavMeshAtPointSceneSpace(const NavMesh * ptNavMesh, const Vector3D & wpLocation);  void NavMeshInitInst(SSceneID idSScene);  void NavMeshFreeInst(SceneCommonData & tSCD);  BOOL NavMeshStitchVerifyTileSize(const SceneCommonData & tSCD);  BOOL NavMeshCanEnterForSpawning(const NavMeshDef * ptNavMeshDef, int32 nSquareIndex, const PassabilityType ePassabilityType);  // X/Common/Nav/NavigationMesh.cpp:1040, :3296
void WorldCacheSceneValues(SNO snoScene, SceneCachedValues & tCachedValues);  // X/Common/World.cpp
void SceneCommonDataActivate(SSceneID idSScene);  // defined later in this file
void SceneObjectAABBToWorldAABB(const PRTransform & transformSceneToWorld, const AABB & aabbToTransform, AABB & aabbWorldSpace);  // line 1084, defined later in this file
BOOL SceneCommonIsExcluded(SSceneID idSScene, const SceneDefinition * ptSceneDef);  // SceneCommon.cpp:2787, defined below
void InactiveMarkerActivateInFrustrum(SSceneID idSScene, const Frustum & tFrustum, const Vector3D & wpFocalPoint);  // X/Common/InactiveMarker.cpp
void ActivateInactiveMarkerList(SWorldID idSWorld, SSceneID idSScene, InactiveMarkerList * ptList);  // X/Common/InactiveMarker.cpp
CSceneID CSceneAdd(const SNO snoScene, const SSceneID idSScene, const SceneSpecification & tSceneSpec, const SWorldID idSWorld, const PRTransform & transform, const SSceneID idSSceneParent, const SNO snoSceneGroup, BOOL bIsAttachment, BOOL bIsVista, const RenderContext eContext, const SNO * snoActors, uint32 uActorCount);  // X/Client/Scene.cpp:491
SSceneID SSceneAdd(const SNO snoScene, const SceneSpecification & tSceneSpec, SWorldID idSWorld, const PRTransform & transform, const SSceneID idSSceneParent, const SNO snoSceneGroup, const SNO snoMarkerSetSrc, int32 nMarkerSetSrcIndex, LabelListPooled & listAppliedLabels, BOOL bIsVista);  // X/Server/SScene.cpp
void WorldCommonPlayerSees(SWorldID idSWorld);  // X/Common/World.cpp
BOOL NavMeshHandleIntersectsLine(const NavMeshHandle & tNavMeshHandle, const LineSeg3D & wline);  // X/Common/Nav/NavigationMesh.cpp:901
BOOL NavZoneCollideSweptPoint3D(SWorldID idSWorld, const Vector3D & wpStart, const Vector3D & wvDirection, uint16 uOneRequiredFlag, uint16 uPreventFlags, uint32 uCollFlags, float & flISectT, Vector3D & wpCollision);  // X/Common/Nav/NavZoneIntersect.cpp:2173
BOOL WorldIsRotated(const SNO snoWorld);  // X/Common/World.cpp:775
#include "Definitions/ObserverDefinition.h"
#include "GlobalSNO.h"
void NavMeshGetPositionByIndex(const NavMesh * ptNavMesh, int32 nSquareIndex, int32 & nSquareX, int32 & nSquareY);  // X/Common/Nav/NavigationMesh.cpp:390
// X/Server/Marker/SMarker.h is not in src yet; same enum as the local copy in X/Server/Marker/SMarker.cpp.
enum DeactivateStatus { DEACTIVATE_YES = 0, DEACTIVATE_ONLY_WITH_SCENE = 1, DEACTIVATE_NO_PLAYER_SEEN = 2, DEACTIVATE_NO_TOWN_PORTAL = 3, DEACTIVATE_NO_PET = 4, DEACTIVATE_NO_ESCORT = 5, DEACTIVATE_NO_DOESNT_DEACTIVATE = 6, DEACTIVATE_NO_DELETING = 7, DEACTIVATE_NO_DYING = 8, DEACTIVATE_NO_COMPLEX_EFFECT = 9, DEACTIVATE_NO_HAS_TIMER = 10, DEACTIVATE_NO_UNDEFINED_GIZMO = 11, DEACTIVATE_NO_SCRIPT = 12, DEACTIVATE_NO_QUEUED_ACTIONS = 13, DEACTIVATE_NO_TALKING = 14, DEACTIVATE_NO_NOT_IN_SCENE = 15, DEACTIVATE_NO_UNINTERRUPTABLE_POWER = 16, DEACTIVATE_NO_JUST_CREATED = 17, DEACTIVATE_NO_ATTRIBUTE = 18 };
DeactivateStatus MarkerCanDeactivateActor(const ActorID idActor);  // X/Server/Marker/SMarker.cpp:384
BOOL WorldTreatAsOneScene(SWorldID idSWorld);  // X/Common/World.cpp:802
#include "../Client/CGame.h"

void InactiveMarkerActivateInFrustrum(SSceneID idSScene, const Frustum & tFrustum);  // X/Common/InactiveMarker.cpp
void SceneCommonPlayerSees(SSceneID idSScene, int32 nPlayerIndex, BOOL bSeen, BOOL bSeenPrefetch);  // line 2807, defined later in this file
#include "../Sigma/MemoryPools.h"
#include "../Server/SWorld.h"
#include "../../BlizzardCore/Include/Blizzard/Debug.h"
#include "../Server/SGame.h"
#include "../Server/Network/KnownList.h"
#include "../Client/SNOPreload.h"
#include "../Server/System/LevelArea.h"
namespace AmbientSounds { void DeactivateInScene(SSceneID idSScene); }  // X/Client/Sound/AmbientSound.cpp; the namespace is not declared in src/X/Client/Sound/AmbientSound.h yet
#include "../Client/Lights.h"
bool operator!=(dmBodyId id1, dmBodyId id2);  bool operator==(dmBodyId id1, dmBodyId id2);  // Contrib/Contrib/Domino/Dynamics/dmBody.cpp:25, :31
void DominoRemoveBody(dmBodyId id);  void RActorDeactivateInScene(SSceneID idSScene);  void NavZoneInstanceDispose(SSceneID idSScene);  // not in src yet; signatures from the mangled callee names
void GameDebugSceneDeActivating(SSceneID idSScene);  void ParticleDeactivateInScene(SSceneID idSScene);  void ActorDeactivateInScene(SSceneID idSScene);  void LuaScriptDeactivateInScene(const SSceneID idSScene);  // GameDebug.cpp, Client/Particle.cpp, Server/Actors/Actors.cpp, Server/LuaScript/LuaScript.cpp
void LevelAreaActivate(const LevelAreaID idLevelArea);  dmBodyId DominoCreateBodyFromScene(const SceneCommonData & tSCD);  dmBodyId DominoCreateBodyFromActor(SWorldID idSWorld, SNO snoPhysMesh, const PRTransform & tTransform, float flScale);  void AdventureSceneInitialActivation(const SSceneID idSScene);  void GizmoLocPlaceGizmos(SceneCommonData & tSCD);  void MarkersCreateInstancesFromScene(SSceneID idSScene);  void MarkersCreateEnvAppList(SceneCommonData & tSCD);  void LuaScriptReActivateInScene(const SSceneID idSScene);  void InactiveMarkerActivateSceneMarkers(SSceneID idSScene);  // LevelArea.cpp:618, Physics/Domino.cpp:210 and :242, Adventure.cpp:364, GizmoLocs.cpp:491, Marker.cpp:2651 and :3040, LuaScript.cpp:1776, InactiveMarker.cpp
SWorldID SceneCommonGetSWorldID(SSceneID idSScene);  void SceneSpawnBitMaskDispose(SceneCommonData & tSCD);  void SceneSpawnBitMaskInitialize(SceneCommonData & tSCD, const PassabilityType ePassabilityType, BOOL bIncludeEliteMask);  // defined later in this file
static BOOL sSceneGetDeactivationReasonsString(SceneCommonData * ptSCD, int32 & nScenesActive, CRefString & strReasons, int32 nChildLevel);  // line 3253, defined later in this file
struct Actor;  Actor * ActorTryToGet(ActorID idActor);  const CHAR * MarkerNameGetString(uint32 dwHash);  // X/Server/Actors/Actors.cpp:4890, X/Common/Marker/Marker.cpp:176





























typedef XChainedFixedBlockAllocator<NavMeshInst> TNavMeshInstPool;
typedef XChainedFixedBlockAllocator<NavZoneInstance> TNavZoneInstPool;

struct SceneCommonGlobals
{
    TNavMeshInstPool tNavMeshInstPool;
    TNavZoneInstPool tNavZoneInstPool;
    TEnvAppDataPool tEnvAppDataPool;
    TGizmoLocPlacementDataPool tGizmoLocPlacementPool;
    int32 nSceneInstanceUID;
};

















void SceneCommonInitializeForGame()
{
    XMemoryAllocatorInterface * ptMemInterface = AppGetCurrentMemoryInterface();
    GameCommonData & tGCD = GameGetActiveGameCommonData();


    tGCD.ptSceneCommonGlobals = (SceneCommonGlobals *)SigmaMemoryNew(sizeof(SceneCommonGlobals), 4, ptMemInterface, TRUE);
    tGCD.ptSceneCommonGlobals->tNavMeshInstPool.Initialize();
    tGCD.ptSceneCommonGlobals->tNavMeshInstPool.SetMemoryInterface(ptMemInterface);
    tGCD.ptSceneCommonGlobals->tNavMeshInstPool.SetBlockCount_Tracked(100);
    tGCD.ptSceneCommonGlobals->tNavZoneInstPool.Initialize();
    tGCD.ptSceneCommonGlobals->tNavZoneInstPool.SetMemoryInterface(ptMemInterface);
    tGCD.ptSceneCommonGlobals->tNavZoneInstPool.SetBlockCount_Tracked(100);
    tGCD.ptSceneCommonGlobals->tEnvAppDataPool.Initialize();
    tGCD.ptSceneCommonGlobals->tEnvAppDataPool.SetMemoryInterface(ptMemInterface);
    tGCD.ptSceneCommonGlobals->tEnvAppDataPool.SetBlockCount_Tracked(512);
    tGCD.ptSceneCommonGlobals->tGizmoLocPlacementPool.Initialize();
    tGCD.ptSceneCommonGlobals->tGizmoLocPlacementPool.SetMemoryInterface(ptMemInterface);
    tGCD.ptSceneCommonGlobals->tGizmoLocPlacementPool.SetBlockCount_Tracked(32);


    tGCD.ptSceneCommonGlobals->tGizmoLocPlacementPool.SetGrow(XALLOC_GROW_LINEAR, 32);
    tGCD.ptSceneCommonGlobals->tGizmoLocPlacementPool.SetShrinkable(TRUE);

}




void SceneCommonDisposeForGame()
{
    XMemoryAllocatorInterface * ptMemInterface = AppGetCurrentMemoryInterface();
    GameCommonData & tGCD = GameGetActiveGameCommonData();


    tGCD.ptSceneCommonGlobals->tNavMeshInstPool.Dispose();
    tGCD.ptSceneCommonGlobals->tNavZoneInstPool.Dispose();
    tGCD.ptSceneCommonGlobals->tEnvAppDataPool.Dispose();
    tGCD.ptSceneCommonGlobals->tGizmoLocPlacementPool.Dispose();

    SigmaMemoryFree(tGCD.ptSceneCommonGlobals, ptMemInterface); tGCD.ptSceneCommonGlobals = NULL;
}



static void sSceneCalculate2DLogicalWorldBounds(Rect2D & rect2DSceneWorld, int32 nNavMeshSizeX, int32 nNavMeshSizeY, const PRTransform & transformWorld)
{





    Vector3D wpSceneUpperLeft(cgwpNull);

    Vector3D wpSceneBottomRight;
    wpSceneBottomRight.x = (float)nNavMeshSizeX * 2.5f;
    wpSceneBottomRight.y = (float)nNavMeshSizeY * 2.5f;
    wpSceneBottomRight.z = 0.0f;

    AABB aabbBounds;
    aabbBounds.Set(wpSceneUpperLeft, wpSceneBottomRight);


    AABB aabbWorld;
    SceneObjectAABBToWorldAABB(transformWorld, aabbBounds, aabbWorld);


    rect2DSceneWorld.Set(
        aabbWorld.wp.x - aabbWorld.wvExt.x,
        aabbWorld.wp.y - aabbWorld.wvExt.y,
        aabbWorld.wp.x + aabbWorld.wvExt.x,
        aabbWorld.wp.y + aabbWorld.wvExt.y);
}




static void sSceneCommonDataSetLogicalWorldBounds(SceneCommonData & tSCD, int32 nNavMeshSizeX, int32 nNavMeshSizeY)
{




    sSceneCalculate2DLogicalWorldBounds(tSCD.rect2DSceneWorld, nNavMeshSizeX, nNavMeshSizeY, tSCD.transformWorld);


    tSCD.aabbBoundsLocalLogical.wvExt.x = (float)nNavMeshSizeX * 2.5f * 0.5f;
    tSCD.aabbBoundsLocalLogical.wvExt.y = (float)nNavMeshSizeY * 2.5f * 0.5f;


    tSCD.aabbBoundsLocalLogical.wp = tSCD.aabbBoundsLocalLogical.wvExt;


    tSCD.aabbBoundsLocalLogical.wp.z = tSCD.aabbBoundsLocalPhysical.wp.z;
    tSCD.aabbBoundsLocalLogical.wvExt.z = tSCD.aabbBoundsLocalPhysical.wvExt.z;
}




void sSceneCommonDataSetLogicalWorldBounds(SceneCommonData & tSCD, const SNO & snoScene)
{


    ScopedSceneDefinitionLockR ptSceneDef(snoScene);

    sSceneCommonDataSetLogicalWorldBounds(tSCD, ptSceneDef->tNavMeshData.nSizeX, ptSceneDef->tNavMeshData.nSizeY);



}




void sSceneCalculate2DLogicalWorldBounds(Rect2D & rect2DSceneWorld, const SNO & snoScene, const PRTransform & transformWorld)
{



    ScopedSceneDefinitionLockR ptDefScene(snoScene);

    sSceneCalculate2DLogicalWorldBounds(rect2DSceneWorld, ptDefScene->tNavMeshData.nSizeX, ptDefScene->tNavMeshData.nSizeY, transformWorld);




}





void NavZoneInstanceInitialize(SSceneID idSScene)
{

    GameCommonData & tGCD = GameGetActiveGameCommonData();
    SceneCommonGlobals * ptSceneCommonGlobals = tGCD.ptSceneCommonGlobals;
    SceneCommonData & tSCD = SceneCommonDataGet(idSScene);
    ScopedSceneDefinitionLockR ptSceneDef(tSCD.snoScene);

    if (!tSCD.ptNavZoneInst)
    {
        NavZoneInstance * ptNavZoneInst = ptSceneCommonGlobals->tNavZoneInstPool.Alloc_Tracked();
        tSCD.ptNavZoneInst = ptNavZoneInst;



        Vector2D tNavSize((float)ptSceneDef->tNavMeshData.nSizeX * 2.5f, (float)ptSceneDef->tNavMeshData.nSizeY * 2.5f);



        SigmaMemoryClear(ptNavZoneInst, sizeof(NavZoneInstance));
        ptNavZoneInst->idSWorld = tSCD.idSWorld;
        ptNavZoneInst->idSScene = tSCD.idSScene;
        ptNavZoneInst->snoScene = tSCD.snoScene;
        ptNavZoneInst->wpWorldMin3d = tSCD.transformWorld.wp;
        ptNavZoneInst->wpWorldMin = *(const Vector2D *)&tSCD.transformWorld.wp;
        ptNavZoneInst->wpWorldMax = tSCD.ptNavZoneInst->wpWorldMin + tNavSize;
        if (ptSceneDef->tNavZoneDef.uBorderDataCount)
        {

            ptNavZoneInst->uBorderDataInstCount = ptSceneDef->tNavZoneDef.uBorderDataCount;
            ptNavZoneInst->arBorderDataInst = (NavCellBorderDataInst *)SigmaMemoryNew(ptNavZoneInst->uBorderDataInstCount * sizeof(NavCellBorderDataInst), 4, NULL, TRUE);


            ptNavZoneInst->uMaxBorderScenes = ptNavZoneInst->uBorderDataInstCount * 2;
            ptNavZoneInst->uBorderSceneCount = 0;
            ptNavZoneInst->arBorderScenes = (NavCellBorderDataSceneInst *)SigmaMemoryNew(ptNavZoneInst->uMaxBorderScenes * sizeof(NavCellBorderDataSceneInst), 4, NULL, TRUE);


            ptNavZoneInst->uMaxBorderLookups = ptNavZoneInst->uBorderDataInstCount * 2;
            ptNavZoneInst->uBorderLookupCount = 0;
            ptNavZoneInst->arBorderLookups = (NavCellLookup *)SigmaMemoryNew(ptNavZoneInst->uMaxBorderLookups * sizeof(NavCellLookup), 4, NULL, TRUE);
        }


        ptNavZoneInst->Initialize();

        if (tSCD.idSSceneParent != SSCENEID_INVALID)
        {
            SceneCommonData * ptSCDParent = SceneCommonDataTryToGet(tSCD.idSSceneParent);
            if (!ptSCDParent) { DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL); }







            if (!ptSCDParent->ptNavZoneInst)
            {
                NavZoneInstanceInitialize(tSCD.idSSceneParent);
            }





            ptNavZoneInst->SetParentInstance(ptSCDParent->ptNavZoneInst);
        }
    }
}


void NavZoneInstanceDispose(SSceneID idSScene)
{
    SceneCommonData & tSCD = SceneCommonDataGet(idSScene);
    NavZoneInstance * ptNavZoneInst = tSCD.ptNavZoneInst;

    if (ptNavZoneInst)
    {
        ptNavZoneInst->Dispose();

        SigmaMemoryFree(ptNavZoneInst->arBorderScenes, NULL); ptNavZoneInst->arBorderScenes = NULL;
        SigmaMemoryFree(ptNavZoneInst->arBorderDataInst, NULL); ptNavZoneInst->arBorderDataInst = NULL;
        SigmaMemoryFree(ptNavZoneInst->arBorderLookups, NULL); ptNavZoneInst->arBorderLookups = NULL;


        GameCommonData & tGCD = GameGetActiveGameCommonData();

        tGCD.ptSceneCommonGlobals->tNavZoneInstPool.Free(tSCD.ptNavZoneInst);

        tSCD.ptNavZoneInst = NULL;
    }
}



void SceneCommonDataInitializeNavMesh(SSceneID idSScene)

{
    SceneCommonData & tSCD = SceneCommonDataGet(idSScene);
    ScopedSceneDefinitionLockR pdefScene(tSCD.snoScene);

    if (tSCD.idSSceneParent == SSCENEID_INVALID)
    {



        if (!NavMeshStitchVerifyTileSize(tSCD))
        {
            return;
        }



        GameCommonData & tGCD = GameGetActiveGameCommonData();
        SceneCommonGlobals * ptSceneCommonGlobals = tGCD.ptSceneCommonGlobals;

        tSCD.ptNavMeshInst = ptSceneCommonGlobals->tNavMeshInstPool.Alloc_Tracked();


        NavMeshInitInst(idSScene);
        NavZoneInstanceInitialize(idSScene);

        BOOL bClient = GameActiveGameCommonDataIsClient();

        if (!bClient)
        {



            POS pos = tSCD.tChildSceneIDs.GetHeadPosition();
            while (pos)
            {
                SSceneID idSSceneChild = (SSceneID)tSCD.tChildSceneIDs.GetNext(pos);
                SceneCommonData * ptSCDChild = SceneCommonDataTryToGet(idSSceneChild);


                if (ptSCDChild && !(ptSCDChild->dwSceneFlags & (1 << SCENE_ACTIVATED)))
                {
                    SceneCommonDataActivate(idSSceneChild);
                }
            }
        }
    }
    else
    {


        SceneCommonData * ptSCDParent = SceneCommonDataTryToGet(tSCD.idSSceneParent);

        if (ptSCDParent)
        {
















            if (GameActiveGameCommonDataIsClient())
            {


                ptSCDParent->tChildSceneIDs.AddTail_Tracked(idSScene);
            }
        }
        else
        {
            CRefString sScene = SNOToString(SCENE_SNO, tSCD.snoScene, FALSE);

            Blizzard::Debug::AmIBeingDebugged();



        }

        NavZoneInstanceInitialize(idSScene);
    }
}
static void sSceneCommonDataDisposeNavMesh(SSceneID idSScene)

{
    SceneCommonData & tSCD = SceneCommonDataGet(idSScene);

    if (tSCD.ptNavMeshInst)
    {
        NavMeshFreeInst(tSCD);



        GameCommonData & tGCD = GameGetActiveGameCommonData();
        SceneCommonGlobals * ptSceneCommonGlobals = tGCD.ptSceneCommonGlobals;
        ptSceneCommonGlobals->tNavMeshInstPool.Free(tSCD.ptNavMeshInst);
        tSCD.ptNavMeshInst = NULL;


        if (tSCD.idSSceneParent != SSCENEID_INVALID)
        {
            SceneCommonData * ptSCDParent = SceneCommonDataTryToGet(tSCD.idSSceneParent);
            if (ptSCDParent)
            {
                POS pos = ptSCDParent->tChildSceneIDs.GetHeadPosition();
                while (pos)
                {
                    POS posLast = pos;
                    SSceneID idChildScene = (SSceneID)ptSCDParent->tChildSceneIDs.GetNext(pos);
                    if (idChildScene == idSScene)
                    {
                        ptSCDParent->tChildSceneIDs.RemoveAt(posLast);
                    }
                }
            }
        }
    }
}




void SceneCommonDataInitialize(SceneCommonData & tSCD,
                               const SSceneID idSScene,
                               const SceneSpecification & tSceneSpec,
                               const SWorldID idSWorld,
                               const SNO snoScene,
                               const PRTransform & transform,
                               const SSceneID idSSceneParent,
                               const SNO snoSceneGroup,
                               const SNO snoMarkerSetSrc,
                               int32 nMarkerSetSrcIndex,
                               const BOOL bIsAttachment,
                               const BOOL bIsVista)

{
    GameCommonData & tGCD = GameGetActiveGameCommonData();

    uint32 dwNow = GameGetServerTime();

    tSCD.idSScene = idSScene;
    tSCD.idSWorld = idSWorld;
    tSCD.snoScene = snoScene;
    tSCD.transformWorld = transform;
    tSCD.transformInvWorld = transform.Inverse();
    tSCD.tSceneSpec = tSceneSpec;
    tSCD.dwSceneFlags = 0;
    tSCD.nLockReferenceCount = 0;
    tSCD.dwPlayerLastSeenTime = dwNow;
    tSCD.dwScenePlayerSeenBits = 0;
    tSCD.dwPrefetchScenePlayerSeenBits = 0;
    tSCD.snoSceneGroup = snoSceneGroup;
    tSCD.snoMarkerSetSrc = snoMarkerSetSrc;
    tSCD.nMarkerSetSrcIndex = nMarkerSetSrcIndex;
    tSCD.idSSceneParent = idSSceneParent;
    tSCD.ptSpawnBitMask = NULL;
    tSCD.ptSpawnBitMaskElites = NULL;
    tSCD.uSpawnBitMaskDWordSize = 0;

    SceneCommonGlobals * ptSceneCommonGlobals = tGCD.ptSceneCommonGlobals;

    tSCD.nInstanceUID = ptSceneCommonGlobals->nSceneInstanceUID++;

    tSCD.tNavMeshOffset.x = Math::FloatRoundToInt(transform.wp.x / 2.5f);
    tSCD.tNavMeshOffset.y = Math::FloatRoundToInt(transform.wp.y / 2.5f);
    tSCD.flNavMeshOffsetZ = transform.wp.z;

    if (tSceneSpec.tCachedValues.dwCachedValuesValid == 0x3f)  // all six cached-value bits; no enumerator known
    {
        tSCD.aabbBoundsLocalPhysical = tSceneSpec.tCachedValues.aabbBounds;
        tSCD.aabbMarkerSetBoundsLocalPhysical = tSceneSpec.tCachedValues.aabbMarkerSetBounds;
        sSceneCommonDataSetLogicalWorldBounds(tSCD, tSceneSpec.tCachedValues.nNavmeshSizeX, tSceneSpec.tCachedValues.nNavmeshSizeY);
    }
    else
    {




        ScopedSceneDefinitionLockR ptSceneDef(snoScene);
        tSCD.aabbBoundsLocalPhysical = ptSceneDef->aabbBounds;
        tSCD.aabbMarkerSetBoundsLocalPhysical = ptSceneDef->aabbMarkerSetBounds;
        sSceneCommonDataSetLogicalWorldBounds(tSCD, snoScene);
    }

    InactiveMarkerInitializeForScene(idSScene);

    tSCD.tChildSceneIDs.SetPool(MemoryPoolsGetDATAIDListPool());
    tSCD.listAppliedLabels.SetPool(MemoryPoolsGetDATAIDListPool());
    tSCD.listEnvApps.SetPool(&ptSceneCommonGlobals->tEnvAppDataPool);
    tSCD.listGizmoLocPlacements.SetPool(&ptSceneCommonGlobals->tGizmoLocPlacementPool);
    tSCD.listGizmoLocLoot.SetPool(&ptSceneCommonGlobals->tGizmoLocPlacementPool);


    if (GameActiveGameCommonDataIsServer())
    {

        AABB aabbScene;

        SceneGetLogicalAABBInWorldSpace(tSCD, aabbScene);
        SWorld * ptSWorld = SWorldGet(idSWorld);

        Vector3D vSceneMin, vSceneMax;
        aabbScene.ComputeBounds(vSceneMin, vSceneMax);
        ptSWorld->vExtMin.x = Math::Min(ptSWorld->vExtMin.x, vSceneMin.x);
        ptSWorld->vExtMin.y = Math::Min(ptSWorld->vExtMin.y, vSceneMin.y);
        ptSWorld->vExtMax.x = Math::Max(ptSWorld->vExtMax.x, vSceneMax.x);
        ptSWorld->vExtMax.y = Math::Max(ptSWorld->vExtMax.y, vSceneMax.y);
    }




    if (idSSceneParent == SSCENEID_INVALID && !bIsVista)
    {
        NavMeshStitchScene(idSScene);
    }

    GameMode nGameMode = tGCD.GameModeGet();
    if ((nGameMode == GAMEMODE_EDITUNITLAYOUT ||
         nGameMode == GAMEMODE_HOLODECK) && !bIsAttachment)
    {

        SceneCommonDataActivate(idSScene);
    }
}


static void sActivateEnvAppList(SceneCommonData & tSCD)
{



    POS pos = tSCD.listEnvApps.GetHeadPosition();
    while (pos)
    {
        EnvAppData & tAppData = tSCD.listEnvApps.GetNext(pos);


        tAppData.bodyId = DominoCreateBodyFromActor(tSCD.idSWorld, tAppData.snoPhysMesh, tAppData.transform, tAppData.flScale);
    }
}





static void sDeactivateEnvAppList(SceneCommonData & tSCD)
{



    POS pos = tSCD.listEnvApps.GetHeadPosition();
    while (pos)
    {
        EnvAppData & tAppData = tSCD.listEnvApps.GetNext(pos);
        if (tAppData.bodyId != dm_nullBodyId)
        {
            DominoRemoveBody(tAppData.bodyId);
            tAppData.bodyId = dm_nullBodyId;
        }
    }
}




static void sPaintCircleOnScene(SceneCommonData & tSCD, const Circle & tCircle)


{
    if (tSCD.ptSpawnBitMask == NULL || tSCD.ptNavMeshInst == NULL)
    {
        return;
    }

    int32 nRadiusInCells = Math::FloatRoundToInt(tCircle.wdRadius / cflGridCellWidth);
    int32 nRadiusInCellsSqr = nRadiusInCells * nRadiusInCells;

    Vector2D vCenterScene = tSCD.transformInvWorld * tCircle.wpCenter;
    IVector2D tCenter(
        Math::FloatRoundToInt(vCenterScene.x / cflGridCellWidth),
        Math::FloatRoundToInt(vCenterScene.y / cflGridCellWidth));




    IVector2D tRadiusInCells(nRadiusInCells, nRadiusInCells);
    IVector2D tStart = tCenter - tRadiusInCells;
    IVector2D tEnd = tCenter + tRadiusInCells;

    int32 nMaxX = tSCD.ptNavMeshInst->nSizeX - 1;
    int32 nMaxY = tSCD.ptNavMeshInst->nSizeY - 1;


    tStart.y = Math::Pin(tStart.y, 0, nMaxY);

    tEnd.y = Math::Pin(tEnd.y, 0, nMaxY);


    for (int y = tStart.y; y <= tEnd.y; y++)
    {
        int32 dy = y - tCenter.y;
        int32 dySqr = dy * dy;

        int32 dx = Math::FloatFloorToInt(Math::Sqrt((float)(nRadiusInCellsSqr - dySqr)));


        int32 x1 = tCenter.x - dx;
        int32 x2 = tCenter.x + dx;

        x1 = Math::Pin(x1, 0, nMaxX);
        x2 = Math::Pin(x2, 0, nMaxX);


        int32 nCellIndex = x1 + y * tSCD.ptNavMeshInst->nSizeX;
        int32 nDWordIndex = nCellIndex / 32;
        int32 nBitIndex = nCellIndex % 32;

        for (int x = x1; x <= x2; x++)
        {

            tSCD.ptSpawnBitMask[nDWordIndex] &= ~(1 << nBitIndex);

            if (tSCD.ptSpawnBitMaskElites)
            {
                tSCD.ptSpawnBitMaskElites[nDWordIndex] &= ~(1 << nBitIndex);
            }

            nBitIndex++;

            if (nBitIndex >= 32)
            {
                nBitIndex = 0;
                nDWordIndex++;
            }
        }
    }
}




static void sPaintNoSpawnActors(SceneCommonData & tSCD)
{

    AABB aabb;
    SceneGetLogicalAABBInWorldSpace(tSCD, aabb);

    Rect2D tRect;
    Vector3D vWorldMin, vWorldMax;
    aabb.ComputeBounds(vWorldMin, vWorldMax);
    tRect.Set(vWorldMin.x, vWorldMin.y, vWorldMax.x, vWorldMax.y);

    SWorld * ptSWorld = SWorldGet(tSCD.idSWorld);
    ptSWorld->tNoSpawnTree.GetCirclesInRect(tRect, tSCD, sPaintCircleOnScene);
}





void SceneCommonDataActivate(const SSceneID idSScene)

{
    SceneCommonData * ptSCD = SceneCommonDataTryToGet(idSScene);
    if (ptSCD == NULL)
    {
        return;
    }

    SceneCommonData * ptSCDParent = SceneCommonDataTryToGet(ptSCD->idSSceneParent);

    if (ptSCDParent)
    {
        SceneCommonDataActivate(ptSCDParent->idSScene);
    }

    if (ptSCD->dwSceneFlags & (1 << SCENE_ACTIVATED))
    {
        return;
    }


    if (GameActiveGameCommonDataIsServer())
    {
        SNOPreload::AddServerSceneBySNO(ptSCD->snoScene);

        SScene * ptSScene = SSceneGet(idSScene);

        if (ptSScene->tSCD.snoSceneGroup == SNO_NONE)
        {
            for (int32 i = 0; i < 4; i++)
            {
                if (ptSScene->arLevelAreaIds[i] != -1)
                {
                    SNOPreload::AddServerLevelArea(LevelAreaGetSNO(ptSScene->arLevelAreaIds[i]));
                }
            }
        }
    }






















    ptSCD->dwSceneFlags |= (1 << SCENE_ACTIVATED);


    BOOL bClient = GameActiveGameCommonDataIsClient();
    if (!bClient)
    {

        if (ptSCDParent)
        {
            KnownListItemAdded(KET_SUBSCENE, idSScene);
        }
        else
        {
            KnownListItemAdded(KET_SCENE, idSScene);
        }



        SceneCommonDataInitializeNavMesh(idSScene);

        SScene * ptSScene = SSceneGet(idSScene);


        if (ptSScene->tSCD.snoSceneGroup == SNO_NONE)
        {
            for (int32 i = 0; i < 4; i++)
            {
                if (ptSScene->arLevelAreaIds[i] != -1)
                {
                    LevelAreaActivate(ptSScene->arLevelAreaIds[i]);
                }
            }
        }


        if (ptSScene->bodyId == dm_nullBodyId)
        {
            ptSScene->bodyId = DominoCreateBodyFromScene(ptSScene->tSCD);
        }
    }


    BOOL bFirstActivation = FALSE;

    if (!(ptSCD->dwSceneFlags & (1 << SCENE_WAS_ACTIVATED_BEFORE)))
    {
        bFirstActivation = TRUE;
        ptSCD->dwSceneFlags |= (1 << SCENE_WAS_ACTIVATED_BEFORE);
    }

    if (bClient)
    {

        SWorldID idSWorld = SceneCommonGetSWorldID(idSScene);
        (void)idSWorld;
    }









    if (bFirstActivation)
    {
        BOOL bAllocateSpawnData = (ptSCD->ptSpawnBitMask == NULL);

        if (!bClient)
        {
            if (bAllocateSpawnData)
            {
                SceneSpawnBitMaskInitialize(*ptSCD, 10, FALSE);
            }


            AdventureSceneInitialActivation(idSScene);
            GizmoLocPlaceGizmos(*ptSCD);
            sPaintNoSpawnActors(*ptSCD);
        }


        MarkersCreateInstancesFromScene(idSScene);


        if (GameActiveGameCommonDataIsServer())
        {
            MarkersCreateEnvAppList(*ptSCD);
        }


        if (bAllocateSpawnData)
        {
            SceneSpawnBitMaskDispose(*ptSCD);
        }
    }
    else
    {

        LuaScriptReActivateInScene(idSScene);

        InactiveMarkerActivateSceneMarkers(idSScene);
        ActivateInactiveMarkerList(ptSCD->idSWorld, idSScene, ptSCD->ptMarkersToBeActivated);
    }


    if (GameActiveGameCommonDataIsServer())
    {
        SGameGlobalsGet().tPerformanceTrackingInfo.m_uScenesActivatedThisIteration++;

        sActivateEnvAppList(*ptSCD);
    }

}




void SceneCommonDataDeactivate(const SSceneID idSScene)

{
    SceneCommonData & tSCD = SceneCommonDataGet(idSScene);


    if (!(tSCD.dwSceneFlags & (1 << SCENE_ACTIVATED)))
    {
        return;
    }


    if (GameActiveGameCommonDataIsServer())
    {
        SGameGlobals & tSGameGlobals = SGameGlobalsGet();
        if (!(tSGameGlobals.dwFlags & 0x2))  // bit 1; no enumerator known
        {
            SScene * ptSScene = SSceneGet(idSScene);

            if (tSCD.snoSceneGroup == SNO_NONE)
            {
                for (int32 i = 0; i < 4; i++)
                {
                    if (ptSScene->arLevelAreaIds[i] != -1)
                    {
                        SNOPreload::RemoveServerLevelArea(LevelAreaGetSNO(ptSScene->arLevelAreaIds[i]));
                    }
                }
            }

            SNOPreload::RemoveServerSceneBySNO(tSCD.snoScene);
        }
    }




    POS pos = tSCD.tChildSceneIDs.GetHeadPosition();
    while (pos)
    {
        SSceneID idSSceneChild = (SSceneID)tSCD.tChildSceneIDs.GetNext(pos);
        SceneCommonData * ptSCDChild = SceneCommonDataTryToGet(idSSceneChild);
        if (ptSCDChild)
        {
            SceneCommonDataDeactivate(idSSceneChild);
        }
    }

    GameDebugSceneDeActivating(idSScene);



    if (GameActiveGameCommonDataIsClient())
    {
    }








    tSCD.dwSceneFlags &= ~(1 << SCENE_ACTIVATED);


    tSCD.dwScenePlayerSeenBits = 0;


    if (GameActiveGameCommonDataIsClient())
    {
        RActorDeactivateInScene(idSScene);
        ParticleDeactivateInScene(idSScene);
        AmbientSounds::DeactivateInScene(idSScene);
        LightsDeactivateInScene(idSScene);
    }
    else
    {


        ActorDeactivateInScene(idSScene);
        LuaScriptDeactivateInScene(idSScene);
    }



    if (GameActiveGameCommonDataIsServer())
    {

        SScene * ptSScene = SSceneGet(idSScene);
        if (ptSScene->bodyId != dm_nullBodyId)
        {
            DominoRemoveBody(ptSScene->bodyId);
            ptSScene->bodyId = dm_nullBodyId;
        }


        sSceneCommonDataDisposeNavMesh(idSScene);

        NavZoneInstanceDispose(idSScene);

        sDeactivateEnvAppList(tSCD);

        if (ptSScene->tSCD.idSSceneParent == SSCENEID_INVALID)
        {
            KnownListItemDeleted(KET_SCENE, idSScene);
        }
        else
        {

            KnownListItemDeleted(KET_SUBSCENE, idSScene);
        }

        SGameGlobalsGet().tPerformanceTrackingInfo.m_uScenesDeactivatedThisIteration++;
    }
}





























































void SceneObjectAABBToWorldAABB(const PRTransform & transformSceneToWorld, const AABB & aabbToTransform, AABB & aabbWorldSpace)
{
    aabbWorldSpace.wp = transformSceneToWorld * aabbToTransform.wp;
    aabbWorldSpace.wvExt = transformSceneToWorld.q.Rotate(aabbToTransform.wvExt);

}





void SceneGetMarkerSetAABBInWorldSpace(const SceneCommonData & tSCD, AABB & aabb)
{
    SceneObjectAABBToWorldAABB(tSCD.transformWorld, tSCD.aabbMarkerSetBoundsLocalPhysical, aabb);
}






void SceneGetPhysicalAABBInWorldSpace(const SceneCommonData & tSCD, AABB & aabb)
{
    SceneObjectAABBToWorldAABB(tSCD.transformWorld, tSCD.aabbBoundsLocalPhysical, aabb);
}






void SceneGetLogicalAABBInWorldSpace(const SceneCommonData & tSCD, AABB & aabb)
{
    SceneObjectAABBToWorldAABB(tSCD.transformWorld, tSCD.aabbBoundsLocalLogical, aabb);
}








void SceneCommonDataDispose(SceneCommonData & tSCD)
{
    SceneCommonDataDeactivate(tSCD.idSScene);
    tSCD.listEnvApps.RemoveAll();


    NavZoneInstanceDispose(tSCD.idSScene);

    if (tSCD.ptNavMeshInst)
    {
        NavMeshUnstitchScene(tSCD.idSScene);


        sSceneCommonDataDisposeNavMesh(tSCD.idSScene);
    }

    InactiveMarkerDisposeForScene(tSCD.idSScene);
}


SSceneID SceneCommonGetSceneIDFromWorldPlace(const WorldPlace & tPlace, BOOL bIncludeInactiveScenes)
{
    NavMeshHandle tNavMeshHandle;
    if (ScenesCommonNavMeshNodeFromPlace(tPlace, &tNavMeshHandle, bIncludeInactiveScenes, FALSE))
        return NavMeshHandleToSubSceneSceneID(tNavMeshHandle);
    return SSCENEID_INVALID;
}









static BOOL sSceneCommonDataNavMeshAtPoint(SceneCommonData & tSCD, const Vector3D & wp, NavMeshHandle * ptNavMeshHandle, const BOOL fCanActivateScenes, const BOOL fProcessInactive)





{


    Vector3D wpSceneSpace = tSCD.transformInvWorld * wp;

    if (!tSCD.ptNavMeshInst)
    {
        if (fCanActivateScenes)
        {

            if (tSCD.tSceneSpec.tCachedValues.dwCachedValuesValid != 0x3f)
            {
                WorldCacheSceneValues(tSCD.snoScene, tSCD.tSceneSpec.tCachedValues);
            }

            if (tSCD.tSceneSpec.tCachedValues.dwCachedValuesValid != 0x3f)
            {
                CRefString sScene = SNOToString(SCENE_SNO, tSCD.snoScene, FALSE);
                CRefString sError;

                sError.Format("Unable to cache values for scene '%s', id: 0x%08X, idParent: 0x%08X\n", sScene.str(), tSCD.idSScene, tSCD.idSSceneParent);

                DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
            }

            float flMaxX = (float)tSCD.tSceneSpec.tCachedValues.nNavmeshSizeX * 2.5f;
            float flMaxY = (float)tSCD.tSceneSpec.tCachedValues.nNavmeshSizeY * 2.5f;
            if (wpSceneSpace.x >= 0.0f &&
                wpSceneSpace.x <= flMaxX &&
                wpSceneSpace.y >= 0.0f &&
                wpSceneSpace.y <= flMaxY)
            {
                SceneCommonDataActivate(tSCD.idSScene);
            }
        }
        if (!tSCD.ptNavMeshInst)
        {
            if (fProcessInactive)
            {
                ScopedSceneDefinitionLockR pdefScene(tSCD.snoScene);
                int32 nNavMeshNodeIndex = NavMeshAtPointSceneSpace(&pdefScene->tNavMeshData, wpSceneSpace);
                if (nNavMeshNodeIndex != -1)
                {
                    ptNavMeshHandle->nSquareIndex = nNavMeshNodeIndex;
                    ptNavMeshHandle->idSScene = tSCD.idSScene;
                    return TRUE;
                }
            }
            return FALSE;
        }
    }

    int32 nNavMeshNodeIndex = NavMeshAtPointSceneSpace(tSCD.ptNavMeshInst, wpSceneSpace);
    if (nNavMeshNodeIndex != -1)
    {
        ptNavMeshHandle->nSquareIndex = nNavMeshNodeIndex;
        ptNavMeshHandle->idSScene = tSCD.idSScene;
        return TRUE;
    }

    return FALSE;
}







BOOL ScenesCommonNavMeshNodeFromPlace(const WorldPlace & tPlace, NavMeshHandle * ptNavMeshHandle, const BOOL fCanActivateScenes, const BOOL fProcessInactive)
{





    WorldCommonData * ptWCD = WorldTryGetCommonData(tPlace.idSWorld);
    SceneCommonData * ptSCD;
    if (ptWCD)
    {
        {
            SSceneID idSScene = NavMeshStitchWorldPlaceToScene(*ptWCD, tPlace, TRUE);

            ptSCD = SceneCommonDataTryToGet(idSScene);

            if (ptSCD && (ptSCD->ptNavMeshInst || fCanActivateScenes))
            {
                if (sSceneCommonDataNavMeshAtPoint(*ptSCD, tPlace.wp, ptNavMeshHandle, fCanActivateScenes, fProcessInactive))
                    return TRUE;
            }
        }





        SceneCommonDataIterator tIterator;
        while (SceneCommonDataArrayIterate(tIterator, ptSCD, SkipHolodeckScenes))
        {
            if (ptSCD->idSWorld == tPlace.idSWorld)
            {



                if (sSceneCommonDataNavMeshAtPoint(*ptSCD, tPlace.wp, ptNavMeshHandle, fCanActivateScenes, fProcessInactive))
                    return TRUE;
            }
        }
    }



    *ptNavMeshHandle = NAVMESH_HANDLE_NONE;
    return FALSE;

}








SNO ScenesCommonLevelAreaFromPlace(SSceneID idSScene, Vector3D vPosition, BOOL fProcessInactive)
{
    SceneCommonData * ptSCD = SceneCommonDataTryToGet(idSScene);
    if (ptSCD)
    {
        NavMeshHandle tNavMeshHandle;
        if (sSceneCommonDataNavMeshAtPoint(*ptSCD, vPosition, &tNavMeshHandle, FALSE, fProcessInactive))
            return NavMeshHandleGetLevelArea(tNavMeshHandle);
    }
    return SNO_NONE;
}










LevelAreaID ScenesCommonLevelAreaIDFromPlace(SSceneID idSScene, Vector3D wp)
{
    LevelAreaID idLevelArea = -1;

    SceneCommonData * ptSCD = SceneCommonDataTryToGet(idSScene);
    if (ptSCD)
    {
        NavMeshHandle tNavMeshHandle;

        if (sSceneCommonDataNavMeshAtPoint(*ptSCD, wp, &tNavMeshHandle, FALSE, FALSE))
        {
            idLevelArea = NavMeshHandleGetLevelAreaID(tNavMeshHandle, FALSE);
        }
    }

    return idLevelArea;
}







BOOL ScenesCommonLineIntersectNavMeshNode(
    SWorldID idSWorld,
    const LineSeg3D & wline,
    NavMeshHandle * ptNavMeshHandle,
    WorldPlace * ptPlaceCollision)
{

    if (NavMeshHandleIsNone(*ptNavMeshHandle) ||
        !NavMeshHandleIntersectsLine(*ptNavMeshHandle, wline))
    {





        Vector3D wpOrigin, wvDirection;

        wpOrigin = wline.wr.wpOrigin;
        wvDirection = wline.wr.wvDirection * wline.wdLength;

        float flT;
        Vector3D wpCollision;

        uint32 uCollFlags = 0xc;
        if (NavZoneCollideSweptPoint3D(idSWorld, wpOrigin, wvDirection, 0xc07, 0, uCollFlags, flT, wpCollision))
        {
            WorldPlace tPlace;

            tPlace.wp = wpCollision;
            tPlace.idSWorld = idSWorld;

            ScenesCommonNavMeshNodeFromPlace(tPlace, ptNavMeshHandle, FALSE, FALSE);

            if (ptNavMeshHandle->nSquareIndex == -1)
            {


                float flMod;
                BOOL bRetry = FALSE;

                flMod = Math::FloatMod(tPlace.wp.x, 2.5f);

                if (flMod < 0.000001f)
                {
                    tPlace.wp.x -= 1.25f;
                    bRetry = TRUE;
                }

                flMod = Math::FloatMod(tPlace.wp.y, 2.5f);

                if (flMod < 0.000001f)
                {
                    tPlace.wp.y -= 1.25f;
                    bRetry = TRUE;
                }

                if (bRetry)
                {
                    ScenesCommonNavMeshNodeFromPlace(tPlace, ptNavMeshHandle, FALSE, FALSE);
                }
            }

            if (ptNavMeshHandle->nSquareIndex != -1)
            {
                if (ptPlaceCollision)
                {
                    *ptPlaceCollision = tPlace;
                }
                return TRUE;
            }
        }



        *ptNavMeshHandle = NAVMESH_HANDLE_NONE;
        return FALSE;
    }
    return TRUE;
}



SceneCommonData & SceneCommonDataGet(SSceneID idSScene)
{
    if (GameActiveGameCommonDataIsClient())
    {
        CScene * ptCScene = CSceneGetFromSSceneID(idSScene);
        return ptCScene->tSCD;
    }
    else
    {
        SScene * ptSScene = SSceneGet(idSScene);
        return ptSScene->tSCD;
    }
}


















SceneCommonData * SceneCommonDataTryToGet(SSceneID idSScene)
{
    if (idSScene == SSCENEID_INVALID)
        return NULL;
    if (GameActiveGameCommonDataIsClient())
    {
        CScene * ptCScene = CSceneGetFromSSceneID(idSScene);
        if (ptCScene)
            return &ptCScene->tSCD;
    }
    else
    {
        SScene * ptSScene = SSceneTryToGet(idSScene);
        if (ptSScene)
            return &ptSScene->tSCD;
    }
    return NULL;
}
















































































SceneCommonData * SceneCommonDataArrayIterate(SceneCommonDataIterator & tSCDIterator, SceneCommonData *& ptSCD, CSceneDataArrayIterateMode eIterateMode)
{













    if (GameActiveGameCommonDataIsClient())
    {
        TDataArrayIterator<CScene> & tIterator = (TDataArrayIterator<CScene> &)tSCDIterator;
        ptSCD = CSceneDataArrayIterate(tIterator, *sgtAppGlobals.ptCGameActive, eIterateMode) ? &tIterator->tSCD : NULL;
    }
    else
    {
        TDataArrayIterator<SScene> & tIterator = (TDataArrayIterator<SScene> &)tSCDIterator;












        ptSCD = SSceneDataArrayIterate(tIterator) ? &tIterator->tSCD : NULL;
    }
    return ptSCD;
}

































void SceneGetSceneToWorldTransform(SSceneID idSScene, PRTransform & transformSceneToWorld)
{
    if (idSScene == SSCENEID_INVALID)
    {
        transformSceneToWorld = cgtPRTransformIdentity;
        return;
    }
    SceneCommonData & tSCD = SceneCommonDataGet(idSScene);
    transformSceneToWorld = tSCD.transformWorld;
}









































































































































































































DataID SceneCommonAdd(const SNO snoScene, const SceneSpecification & tSceneSpec, SWorldID idSWorld, const PRTransform & transform, const SSceneID idSSceneParent, const SNO snoSceneGroup, const SNO snoMarkerSetSrc, int32 nMarkerSetSrcIndex, LabelListPooled & listAppliedLabels, BOOL bIsVista)
{
















    {
        ScopedSceneDefinitionLockR pdefScene(snoScene);
        bIsVista |= (pdefScene->dwFlags >> SCENE_IGNORE_NAVMESH) & 1;
    }





    if (GameActiveGameCommonDataIsClient())
    {

        RenderContext eContext = GameGetActiveGameCommonData().GameModeGet() == GAMEMODE_HOLODECK ? RENDERCONTEXT_HOLODECK : RENDERCONTEXT_DEFAULT;



        return CSceneAdd(snoScene, SSCENEID_INVALID, tSceneSpec, idSWorld, transform, idSSceneParent, snoSceneGroup, FALSE, bIsVista, eContext, NULL, 0);
    }
    else
    {

        return SSceneAdd(snoScene, tSceneSpec, idSWorld, transform, idSSceneParent, snoSceneGroup, snoMarkerSetSrc, nMarkerSetSrcIndex, listAppliedLabels, bIsVista);
    }
}





SNO SceneCommonGetSceneSNO(SSceneID idSScene)
{
    SceneCommonData & tSCD = SceneCommonDataGet(idSScene);
    return tSCD.snoScene;
}





SWorldID SceneCommonGetSWorldID(SSceneID idSScene)
{
    SceneCommonData & tSCD = SceneCommonDataGet(idSScene);
    return tSCD.idSWorld;
}




Vector3D GetFrustrumFocalPoint(const WorldPlace & tPlace, float & flWideFactor, const Player * ptBasePlayer)



{






    Vector3D vDeadPlaces[4];
    Vector3D vAlivePlaces[4];

    ActorCommonData & tBaseACD = ACDGet(ptBasePlayer->idPlayerACD);

    float flNumDeadPlayers = 0.0f;
    float flNumAlivePlayers = 0.0f;
    int32 nNumDeadPlayers = 0;
    int32 nNumAlivePlayers = 0;
    for (const Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {
        ActorCommonData & tACD = ACDGet(ptPlayer->idPlayerACD);
        if (PlayersAreLocalToEachOther(*ptPlayer, *ptBasePlayer) && tACD.tLocation.tWorldLocation.idSWorld == tBaseACD.tLocation.tWorldLocation.idSWorld)
        {
            if (tACD.IsAlive())
            {
                vAlivePlaces[nNumAlivePlayers++] = tACD.GetPlace().wp;
                flNumAlivePlayers += 1.0f;
            }
            else
            {
                vDeadPlaces[nNumDeadPlayers++] = tACD.GetPlace().wp;
                flNumDeadPlayers += 1.0f;
            }
        }
    }

    Vector3D vCentroid(0.0f, 0.0f, 0.0f);
    float flUsedPlayers;
    if (nNumAlivePlayers > 0)
    {
        for (int32 i = 0; i < nNumAlivePlayers; i++)
        {
            vCentroid += vAlivePlaces[i];
        }
        flUsedPlayers = flNumAlivePlayers;
    }
    else
    {
        for (int32 i = 0; i < nNumDeadPlayers; i++)
        {
            vCentroid += vDeadPlaces[i];
        }
        flUsedPlayers = flNumDeadPlayers;
    }
    vCentroid = vCentroid * (1.0f / flUsedPlayers);


    if (nNumAlivePlayers > 1)
    {
        float flSqrWorstDistance = 0.0f;
        for (int32 i = 0; i < nNumAlivePlayers; i++)
        {
            for (int32 j = i + 1; j < nNumAlivePlayers; j++)
            {
                float flSqrDistance = Vector3D::DistanceSquared(vAlivePlaces[i], vAlivePlaces[j]);
                if (flSqrDistance > flSqrWorstDistance)
                    flSqrWorstDistance = flSqrDistance;
            }
        }


        flWideFactor = Math::Min(Math::Sqrt(flSqrWorstDistance) * 0.016f, 1.0f);

    }
    else if (nNumDeadPlayers > 1)
    {
        float flSqrWorstDistance = 0.0f;
        for (int32 i = 0; i < nNumDeadPlayers; i++)
        {
            for (int32 j = i + 1; j < nNumDeadPlayers; j++)
            {
                float flSqrDistance = Vector3D::DistanceSquared(vDeadPlaces[i], vDeadPlaces[j]);
                if (flSqrDistance > flSqrWorstDistance)
                    flSqrWorstDistance = flSqrDistance;
            }
        }



        flWideFactor = Math::Min(Math::Sqrt(flSqrWorstDistance) * 0.016f, 1.0f);
    }
    else
    {
        flWideFactor = 0.0f;
    }


    return vCentroid;
}

float sGetFarPlane(SWorldID idSWorld, float flAddedDistance)
{
    float flDefaultFarPlane = 950.0f;







    if (idSWorld != SWORLDID_INVALID)
    {
        SNO snoWorld = WorldCommonGetWorldSNO(idSWorld);
        if (snoWorld != SNO_NONE)
        {
            ScopedWorldDefinitionLockR pdefWorld(snoWorld);
            float flFarPlaneCap = pdefWorld->tEnvironment.flFarPlaneCap;

            if (flFarPlaneCap > 1.0f)
            {
                float flAdjustedFarPlaneCap = flFarPlaneCap + flAddedDistance * 2.0f;
                return Math::Min(flAdjustedFarPlaneCap, flDefaultFarPlane);
            }
        }
    }

    return flDefaultFarPlane;
}



static void sGetFrustum(WorldPlace tPlace, float flWideFactor, float flOffsetMultiplier, Vector3D * wpCorners, BOOL fReverse)





{

    Angle aFOV;
    Vector3D wvCameraOffsetVector;
    Vector3D wvLookAtOffsetVector;
    if (flWideFactor == 0.0f)
    {
        ScopedObserverDefinitionLockR pdefObserver(GlobalSNOGet(GLOBALSNO_OBSERVER_DEFAULT));
        aFOV = pdefObserver->aFOV;
        wvCameraOffsetVector = pdefObserver->wvCameraOffsetVector;
        wvLookAtOffsetVector = pdefObserver->wvLookAtOffsetVector;
    }
    else if (flWideFactor == 1.0f)
    {
        ScopedObserverDefinitionLockR pdefObserver(GlobalSNOGet(GLOBALSNO_OBSERVER_WIDE));
        aFOV = pdefObserver->aFOV;
        wvCameraOffsetVector = pdefObserver->wvCameraOffsetVector;
        wvLookAtOffsetVector = pdefObserver->wvLookAtOffsetVector;
    }
    else
    {
        ScopedObserverDefinitionLockR pdefObserver0(GlobalSNOGet(GLOBALSNO_OBSERVER_DEFAULT));
        ScopedObserverDefinitionLockR pdefObserver1(GlobalSNOGet(GLOBALSNO_OBSERVER_WIDE));

        Math::Lerp(pdefObserver0->aFOV, pdefObserver1->aFOV, flWideFactor, aFOV);
        Math::Lerp(pdefObserver0->wvCameraOffsetVector, pdefObserver1->wvCameraOffsetVector, flWideFactor, wvCameraOffsetVector);
        Math::Lerp(pdefObserver0->wvLookAtOffsetVector, pdefObserver1->wvLookAtOffsetVector, flWideFactor, wvLookAtOffsetVector);
    }









    Vector3D wvCameraOffset = wvCameraOffsetVector * flOffsetMultiplier;
    float flAddedDistance = (wvCameraOffsetVector - wvCameraOffset).Magnitude();

    if (tPlace.idSWorld != SWORLDID_INVALID && WorldIsRotated(WorldCommonGetWorldSNO(tPlace.idSWorld)))
    {
        Vector3D::Rotate(cgwvZAxis, -PI / 4.0f, wvCameraOffset);
        Vector3D::Rotate(cgwvZAxis, -PI / 4.0f, wvLookAtOffsetVector);
    }


    if (fReverse)
    {
        Vector3D::Rotate(cgwvZAxis, PI, wvCameraOffset);
        Vector3D::Rotate(cgwvZAxis, PI, wvLookAtOffsetVector);
    }

    Vector3D wpLookFrom = tPlace.wp + wvCameraOffset;
    Vector3D wpLookTo = tPlace.wp + wvLookAtOffsetVector;

    const float cflAspectRatio = 1366.0f / 768.0f;

    Frustum::ComputeCorners(wpLookFrom, wpLookTo, cgwvZAxis, aFOV, cflAspectRatio, 0.0f, sGetFarPlane(tPlace.idSWorld, flAddedDistance), wpCorners);
}


static void sGetFrustum(const Player * ptPlayer, float flOffsetMultiplier, Vector3D * wpCorners, Vector3D & vCenterPos)




{
    WorldPlace tPlace = ACDGet(ptPlayer->idPlayerACD).GetPlace();
    float flWideFactor;
    tPlace.wp = GetFrustrumFocalPoint(tPlace, flWideFactor, ptPlayer);
    vCenterPos = tPlace.wp;
    sGetFrustum(tPlace, flWideFactor, flOffsetMultiplier, wpCorners, GameIsParamFlagSet(GPF_REVERSE_CAMERA_FOR_TEAM_1) && ptPlayer->eTeam == 3);
}




void SceneGetPlayerFrustumCorners(const Player * ptPlayer, Vector3D * wpCorners, Vector3D & vCenterPos)
{
    float flMultiplier = 2.0f;




    sGetFrustum(ptPlayer, flMultiplier, wpCorners, vCenterPos);
}




void SceneGetPlaceFrustumCorners(WorldPlace tPlace, Vector3D * wpCorners)
{
    float flMultiplier = 2.0f;



    sGetFrustum(tPlace, 0.0f, flMultiplier, wpCorners, FALSE);
}




void SceneGetTightPlayerFrustumCorners(Player * ptPlayer, Vector3D * wpCorners)
{
    // Linker-removed in the original image (DW_AT_low_pc 0); reconstructed from its DWARF DIE
    // (flMultiplier const_value 1.0f). It keeps flOffsetMultiplier of sGetFrustum from being constant-propagated.
    float flMultiplier = 1.0f;

    Vector3D vCenterPos;
    sGetFrustum(ptPlayer, flMultiplier, wpCorners, vCenterPos);
}




void SceneCreatePlayerPrefetchFrustum(Player * ptPlayer, Frustum & tFrustum)


{
    Vector3D wpCorners[NUM_FRUSTUM_CORNERS];
    Vector3D vCenterPos;

    SceneGetPlayerFrustumCorners(ptPlayer, wpCorners, vCenterPos);

    tFrustum.Set(wpCorners);

    const float sflMargin = 50.0f;
    Plane * ptPlanes = (Plane *)tFrustum.GetPlanes();
    for (int i = 0; i < 6; i++)
    {

        ptPlanes[i].wdD -= sflMargin;
    }
}




void SceneCreatePlacePrefetchFrustum(WorldPlace tPlace, Frustum & tFrustum)


{
    Vector3D wpCorners[NUM_FRUSTUM_CORNERS];

    SceneGetPlaceFrustumCorners(tPlace, wpCorners);

    tFrustum.Set(wpCorners);

    const float sflMargin = 50.0f;
    Plane * ptPlanes = (Plane *)tFrustum.GetPlanes();
    for (int i = 0; i < 6; i++)
    {

        ptPlanes[i].wdD -= sflMargin;
    }
}





void SceneCreateAVFrustum(Player * ptPlayer, Frustum & tFrustum)


{
    const float cflNearPlane = 0.1f;
    const float cflFarPlane = 500.0f;
    const float cflAspectRatio = 1366.0f / 768.0f;

    Vector3D wvForward = ptPlayer->tAVCameraTransform.q.Rotate(cgwvZAxis);
    Vector3D wvRight = Vector3D::CrossProduct(wvForward, cgwvZAxis);
    Vector3D wvUp = Vector3D::CrossProduct(wvRight, wvForward);
    Vector3D tLookFrom = ptPlayer->tAVCameraTransform.wp - wvForward * 25.0f;
    Vector3D tLookTo(ptPlayer->tAVCameraTransform.wp);





    ScopedObserverDefinitionLockR pdefObserver(GlobalSNOGet(GLOBALSNO_OBSERVER_DEFAULT));
    tFrustum.Set(tLookFrom, tLookTo, wvUp, pdefObserver->aFOV, cflAspectRatio, cflNearPlane, cflFarPlane);
}




void SceneCreateRecordedAVFrustum(Player * ptPlayer, Frustum & tFrustum)


{
    const float cflNearPlane = 0.1f;
    const float cflFarPlane = 500.0f;
    const float cflAspectRatio = 1.0f;


    ScopedObserverDefinitionLockR pdefObserver(GlobalSNOGet(GLOBALSNO_OBSERVER_DEFAULT));
    float fDistAbovePlayer = 200.0f / Math::Tan(pdefObserver->aFOV);
    WorldPlace tPlace = ACDGet(ptPlayer->idPlayerACD).GetPlace();
    Vector3D tLookTo(tPlace.wp);
    Vector3D tLookFrom = tPlace.wp + cgwvZAxis * fDistAbovePlayer;
    Vector3D wvUp(cgwvXAxis);



    tFrustum.Set(tLookFrom, tLookTo, wvUp, pdefObserver->aFOV, cflAspectRatio, cflNearPlane, cflFarPlane);
}





void SceneCreatePlayerFrustum(const Player * ptPlayer, Frustum & tFrustum, Vector3D & vCenterPos)



{
    Vector3D wpCorners[NUM_FRUSTUM_CORNERS];

    SceneGetPlayerFrustumCorners(ptPlayer, wpCorners, vCenterPos);

    tFrustum.Set(wpCorners);
}


















BOOL SceneIntersectsFrustum(SceneCommonData & tSCD, const Frustum & tFrustum)
{
    AABB aabbBounds;
    SceneGetMarkerSetAABBInWorldSpace(tSCD, aabbBounds);
    BOOL bIntersect = Intersect::FrustumAndAABB(tFrustum, aabbBounds);
    return bIntersect;
}

















void SceneActivateVisible(
    SWorldID idSWorld,
    const Frustum & tFrustum,
    const Frustum & tPrefetchFrustum,
    const Frustum * ptAVFrustum,
    const Vector3D & vCenter,
    Player ** pptPlayers,
    int32 nPlayerCount)
{





    int32 nPlayerIndices[4];
    for (int32 i = 0; i < nPlayerCount; i++)
    {
        nPlayerIndices[i] = pptPlayers[i]->nPlayerIndex;
    }

    SceneCommonData * ptSCD;
    SceneCommonDataIterator tIterator;
    while (SceneCommonDataArrayIterate(tIterator, ptSCD, SkipHolodeckScenes))
    {
        BOOL bFlaggedForDelete = ptSCD->dwSceneFlags & (1 << SCENE_FLAGGED_FOR_DELETE);

        if (ptSCD->idSWorld != idSWorld || bFlaggedForDelete)
        {
            if (!bFlaggedForDelete)
            {
                for (int32 i = 0; i < nPlayerCount; i++)
                {
                    int32 nPlayerIndex = nPlayerIndices[i];
                    BOOL bSeen = ptSCD->dwScenePlayerSeenBits & (1 << nPlayerIndex);



                    if (bSeen)
                    {
                        SceneCommonPlayerSees(ptSCD->idSScene, nPlayerIndex, FALSE, FALSE);
                    }
                }
            }

            continue;
        }

        BOOL bWorldTreatAsOneScene = WorldTreatAsOneScene(idSWorld);





















        SceneCommonData * ptSCDUsedForVisibility = ptSCD;


        while (ptSCDUsedForVisibility->idSSceneParent != SSCENEID_INVALID)
        {
            SceneCommonData * ptSCDTemp = SceneCommonDataTryToGet(ptSCDUsedForVisibility->idSSceneParent);
            if (!ptSCDTemp)
            {
                break;
            }
            ptSCDUsedForVisibility = ptSCDTemp;
        }


        BOOL bIntersect = SceneIntersectsFrustum(*ptSCDUsedForVisibility, tFrustum);
        if (ptAVFrustum)
        {
            bIntersect |= SceneIntersectsFrustum(*ptSCDUsedForVisibility, *ptAVFrustum);
        }

        BOOL bIntersectPrefetch = SceneIntersectsFrustum(*ptSCDUsedForVisibility, tPrefetchFrustum);
        if (ptSCD->idSWorld == idSWorld) { bIntersect |= bWorldTreatAsOneScene; bIntersectPrefetch |= bWorldTreatAsOneScene; }







        for (int32 i = 0; i < nPlayerCount; i++)
        {
            int32 nPlayerIndex = nPlayerIndices[i];
            SceneCommonPlayerSees(ptSCD->idSScene, nPlayerIndex, bIntersect, bIntersectPrefetch);
        }

        if (bIntersect)
        {


            if (GameActiveGameCommonDataIsClient())
            {

                InactiveMarkerActivateInFrustrum(ptSCD->idSScene, tFrustum);
            }
            else







            {
                InactiveMarkerActivateInFrustrum(ptSCD->idSScene, tFrustum, vCenter);
            }
            if (ptAVFrustum)
            {

                InactiveMarkerActivateInFrustrum(ptSCD->idSScene, *ptAVFrustum);
            }
        }
    }
}




static BOOL sSceneCanDeactivateAllActors(SceneCommonData * ptSCD)

{


    if (GameActiveGameCommonDataIsClient())
    {
        return TRUE;
    }

    SScene * ptSScene = SSceneGet(ptSCD->idSScene);
    POS pos = ptSScene->listACDInScene.GetHeadPosition();

    while (pos)
    {
        POS posOld = pos;
        ACDID idACD = (ACDID)ptSScene->listACDInScene.GetNext(pos);

        ActorCommonData * ptACD = ACDTryToGet(idACD);
        if (!ptACD)
        {
            ptSScene->listACDInScene.RemoveAt(posOld);
            continue;
        }

        ActorID idActor = (ActorID)ACDGetOwner(idACD);


        DeactivateStatus nStatus = MarkerCanDeactivateActor(idActor);
        if (nStatus != DEACTIVATE_YES && nStatus != DEACTIVATE_ONLY_WITH_SCENE)
        {
            return FALSE;
        }
    }





    return TRUE;
}



static BOOL sSceneCanDeactivate(SceneCommonData * ptSCD)
{

    if (GameIsParamFlagSet(GPF_KEEP_ALL_SCENES_ACTIVE))
    {
        return FALSE;
    }

    if (GameIsStartUp())
    {
        return FALSE;
    }

    if (ptSCD->dwSceneFlags & (1 << SCENE_LOCKED))
    {
        return FALSE;
    }

    if (!sSceneCanDeactivateAllActors(ptSCD))
    {
        return FALSE;
    }


    POS pos = ptSCD->tChildSceneIDs.GetHeadPosition();
    while (pos)
    {
        SSceneID idSSceneChild = ptSCD->tChildSceneIDs.GetNext(pos);
        SceneCommonData * ptSCDChild = SceneCommonDataTryToGet(idSSceneChild);
        if (ptSCDChild)
        {
            if (!sSceneCanDeactivateAllActors(ptSCDChild))
            {
                return FALSE;
            }
        }
    }

    return TRUE;
}




void SceneDeactivateUnseen()

{


    uint32 dwNow = GameGetServerTime();

    SceneCommonData * ptSCD;
    SceneCommonDataIterator tIterator;
    while (SceneCommonDataArrayIterate(tIterator, ptSCD, SkipHolodeckScenes))
    {
        BOOL bSeenRecently = FALSE;
        BOOL bPrefetchRecently = FALSE;


        if (GameActiveGameCommonDataIsClient())
        {
            if (CGameGetCurrentSWorldID() != ptSCD->idSWorld)
            {
                continue;
            }
        }


        if (ptSCD->idSSceneParent != SSCENEID_INVALID)
        {
            continue;
        }



        if (ptSCD->dwPlayerLastSeenTime < dwNow)
        {
            ptSCD->dwScenePlayerSeenBits = 0;
            ptSCD->dwPrefetchScenePlayerSeenBits = 0;
        }

        if (ptSCD->dwScenePlayerSeenBits != 0)
        {
            bSeenRecently = TRUE;
        }

        if (ptSCD->dwPrefetchScenePlayerSeenBits != 0)
        {
            bPrefetchRecently = TRUE;
        }

        if (!bSeenRecently || !bPrefetchRecently)
        {
            if (WorldTreatAsOneScene(ptSCD->idSWorld))
            {
                WorldCommonData * ptWCD = WorldTryGetCommonData(ptSCD->idSWorld);
                if (ptWCD && ptWCD->dwPlayerLastSeenTime == dwNow)
                {
                    bSeenRecently = TRUE;
                }
            }
        }


        if (bSeenRecently && !(ptSCD->dwSceneFlags & (1 << SCENE_ACTIVATED)))
        {

            SceneCommonDataActivate(ptSCD->idSScene);
        }

        else if (!bSeenRecently && (ptSCD->dwSceneFlags & (1 << SCENE_ACTIVATED)) &&
                 sSceneCanDeactivate(ptSCD))
        {
            SceneCommonDataDeactivate(ptSCD->idSScene);
        }




    }
}




void SceneCheckInactiveMarkers(SWorldID idSWorld, Frustum & tFrustum, Vector3D & wpFocalPoint)
{
    SceneCommonDataIterator tIterator;
    SceneCommonData * ptSCD;
    while (SceneCommonDataArrayIterate(tIterator, ptSCD, SkipHolodeckScenes))
    {


        if (ptSCD->idSWorld != idSWorld)
            continue;




        if (!(ptSCD->dwSceneFlags & (1 << SCENE_ACTIVATED)))
            continue;




        InactiveMarkerActivateInFrustrum(ptSCD->idSScene, tFrustum, wpFocalPoint);

        ActivateInactiveMarkerList(idSWorld, ptSCD->idSScene, ptSCD->ptMarkersToBeActivated);
    }
}



void SceneCommonDeactivateInWorld(SWorldID idSWorld)
{
    SceneCommonDataIterator tIterator;
    SceneCommonData * ptSCD;




    while (SceneCommonDataArrayIterate(tIterator, ptSCD, SkipHolodeckScenes))
    {
        if (ptSCD->idSWorld == idSWorld)
        {



            SceneCommonDataDeactivate(ptSCD->idSScene);
        }
    }
}

















BOOL SceneCommonCanTownPortal(const SSceneID idSScene, const ActorCommonData & tACD)
{
    if (idSScene != SSCENEID_INVALID)
    {
        SNO snoLevelArea = ACDGetLevelArea(tACD);
        return LevelAreaTownPortalAllowed(snoLevelArea);
    }
    return FALSE;
}







BOOL SceneCommonIsActivated(const SSceneID idSScene)
{
    if (idSScene != SSCENEID_INVALID)
    {
        SceneCommonData & tSCD = SceneCommonDataGet(idSScene);
        return tSCD.dwSceneFlags & (1 << SCENE_ACTIVATED);
    }
    return FALSE;
}















































BOOL SceneCommonIsExcluded(SSceneID idSScene, int32 nPlayerIndex)
{
    Player * ptPlayer = PlayerGetByPlayerIndex(nPlayerIndex);
    ActorCommonData & tACD = ACDGet(ptPlayer->idPlayerACD);

    SSceneID idPlayerSScene = ACDGetCurrentSSceneID(tACD);
    if (idPlayerSScene == SSCENEID_INVALID)
        return FALSE;







    SceneCommonData & tPlayerSCD = SceneCommonDataGet(idPlayerSScene);
    uint32 dwFlags = tPlayerSCD.dwSceneFlags;
    if ((dwFlags & ((1 << SCENE_EXCLUSIONS_INITIALIZED) | (1 << SCENE_HAS_ANY_EXCLUSIONS))) == (1 << SCENE_EXCLUSIONS_INITIALIZED))
        return FALSE;








    const SceneDefinition * ptSceneDef = DefinitionLockR<SceneDefinition>(tPlayerSCD.snoScene);


    if (!(dwFlags & (1 << SCENE_EXCLUSIONS_INITIALIZED)))
    {

        if (ptSceneDef->arExclusions.GetArraySize() > 0) tPlayerSCD.dwSceneFlags |= (1 << SCENE_HAS_ANY_EXCLUSIONS); else tPlayerSCD.dwSceneFlags &= ~(1 << SCENE_HAS_ANY_EXCLUSIONS);
        tPlayerSCD.dwSceneFlags |= (1 << SCENE_EXCLUSIONS_INITIALIZED);
    }

    BOOL bExcluded = SceneCommonIsExcluded(idSScene, ptSceneDef);
    DefinitionUnlockR<SceneDefinition>(ptSceneDef);
    return bExcluded;
}

BOOL SceneCommonIsExcluded(SSceneID idSScene, const SceneDefinition * ptSceneDef)
{
    int32 nExclusions = ptSceneDef->arExclusions.GetArraySize();
    if (nExclusions == 0)
        return FALSE;
    SceneCommonData & tSCD = SceneCommonDataGet(idSScene);
    for (int32 i = 0; i < nExclusions; i++)
    {
        if (tSCD.snoScene == ptSceneDef->arExclusions[i])
            return TRUE;
    }
    return FALSE;
}







void SceneCommonPlayerSees(SSceneID idSScene, int32 nPlayerIndex, BOOL fActivate, BOOL fPrefetch)
{
    SceneCommonData * ptSCD = SceneCommonDataTryToGet(idSScene);
    if (!ptSCD)
        return;

    uint32 dwServerTime = GameGetServerTime();

    if (ptSCD->dwPlayerLastSeenTime != dwServerTime)
    {
        ptSCD->dwScenePlayerSeenBits = 0;
        ptSCD->dwPrefetchScenePlayerSeenBits = 0;
        ptSCD->dwPlayerLastSeenTime = dwServerTime;
    }
    else if (ptSCD->dwScenePlayerSeenBits & (1 << nPlayerIndex))
    {
        return;
    }



    WorldCommonPlayerSees(ptSCD->idSWorld);

    if (fActivate) ptSCD->dwScenePlayerSeenBits |= (1 << nPlayerIndex); else ptSCD->dwScenePlayerSeenBits &= ~(1 << nPlayerIndex);
    if (fPrefetch) ptSCD->dwPrefetchScenePlayerSeenBits |= (1 << nPlayerIndex); else ptSCD->dwPrefetchScenePlayerSeenBits &= ~(1 << nPlayerIndex);

    if (fActivate && !(ptSCD->dwSceneFlags & (1 << SCENE_ACTIVATED)))
    {


        SceneCommonDataActivate(idSScene);
    }





    if ((fActivate || fPrefetch) &&
        ptSCD->idSSceneParent != SSCENEID_INVALID &&
        GameActiveGameCommonDataIsServer())
    {

        SceneCommonPlayerSees(ptSCD->idSSceneParent, nPlayerIndex, fActivate, fPrefetch);
    }
}





static void sPaintSubsceneBitMaskOnParent(SceneCommonData & tSCDChild, SceneCommonData & tSCDParent, const PassabilityType ePassabilityType)
{



    ScopedSceneDefinitionLockR ptChildSceneDef(tSCDChild.snoScene);
    const NavMeshDef * ptChildNavMeshData = &ptChildSceneDef->tNavMeshData;
    int32 nChildSizeX = ptChildNavMeshData->nSizeX;
    int32 nChildSizeY = ptChildNavMeshData->nSizeY;











    int32 nOffsetX = tSCDChild.tNavMeshOffset.x - tSCDParent.tNavMeshOffset.x;
    int32 nOffsetY = tSCDChild.tNavMeshOffset.y - tSCDParent.tNavMeshOffset.y;



    ScopedSceneDefinitionLockR ptParentSceneDef(tSCDParent.snoScene);
    int32 nParentSizeX = ptParentSceneDef->tNavMeshData.nSizeX;
    int32 nIndex = 0;

    for (int32 nChildY = 0; nChildY < nChildSizeY; nChildY++)
    {

        for (int32 nChildX = 0; nChildX < nChildSizeX; nChildX++)
        {





            int32 nDWordIndex = ((nOffsetY + nChildY) * nParentSizeX + nOffsetX + nChildX) / 32;


            BOOL bCanEnter = NavMeshCanEnterForSpawning(ptChildNavMeshData, nIndex++, ePassabilityType);
            if ((uint32)nDWordIndex >= tSCDParent.uSpawnBitMaskDWordSize)
            {
                return;
            }






            if (bCanEnter) { tSCDParent.ptSpawnBitMask[nDWordIndex] |= (1 << (((nOffsetY + nChildY) * nParentSizeX + nOffsetX + nChildX) & 31)); } else { tSCDParent.ptSpawnBitMask[nDWordIndex] &= ~(1 << (((nOffsetY + nChildY) * nParentSizeX + nOffsetX + nChildX) & 31)); }
        }
    }



}



void SceneSpawnBitMaskInitialize(SceneCommonData & tSCD,
                                 const PassabilityType ePassabilityType,
                                 BOOL bIncludeEliteMask)

{
    ScopedSceneDefinitionLockR ptSceneDef(tSCD.snoScene);
    const NavMeshDef * ptNavMeshData = &ptSceneDef->tNavMeshData;
    int32 nSizeX = ptNavMeshData->nSizeX;
    int32 nSizeY = ptNavMeshData->nSizeY;

    int32 nSize = nSizeX * nSizeY;
    int32 nSizeBytes = nSize / 8;

    if (nSize % 8)
    {
        nSizeBytes++;
    }

    if (nSizeBytes % 4)
    {
        nSizeBytes += 4 - (nSizeBytes % 4);
    }

    tSCD.ptSpawnBitMask = (uint32 *)SigmaMemoryNew(nSizeBytes, 4, NULL, TRUE);
    tSCD.uSpawnBitMaskDWordSize = nSizeBytes >> 2;

    int32 nBitIndex = 0;
    int32 nDWordIndex = 0;













    for (int i = 0; i < nSize; i++)
    {
        if (NavMeshCanEnterForSpawning(ptNavMeshData, i, ePassabilityType))
        {

            tSCD.ptSpawnBitMask[nDWordIndex] |= (1 << nBitIndex);
        }

        nBitIndex++;
        if (nBitIndex >= 32)
        {
            nBitIndex = 0;
            nDWordIndex++;
        }
    }


    POS pos = tSCD.tChildSceneIDs.GetHeadPosition();
    while (pos)
    {
        SSceneID idSSceneChild = (SSceneID)tSCD.tChildSceneIDs.GetNext(pos);
        SceneCommonData * ptSCDChild = SceneCommonDataTryToGet(idSSceneChild);
        if (ptSCDChild)
        {
            sPaintSubsceneBitMaskOnParent(*ptSCDChild, tSCD, ePassabilityType);
        }
    }


    if (bIncludeEliteMask)
    {
        tSCD.ptSpawnBitMaskElites = (uint32 *)SigmaMemoryNew(nSizeBytes, 4, NULL, TRUE);
        SigmaMemoryCopy(tSCD.ptSpawnBitMaskElites, tSCD.ptSpawnBitMask, nSizeBytes);
    }

}


void SceneSpawnBitMaskDispose(SceneCommonData & tSCD)
{
    SigmaMemoryFree(tSCD.ptSpawnBitMask, NULL);
    tSCD.ptSpawnBitMask = NULL;
    SigmaMemoryFree(tSCD.ptSpawnBitMaskElites, NULL);
    tSCD.ptSpawnBitMaskElites = NULL;
}




BOOL SceneTestSpawnArea(
    SceneCommonData & tSCD,
    int nSquareIndex,
    float flRadius,
    BOOL bUseEliteMask)
{
    uint32 * ptSpawnMaskToUse = tSCD.ptSpawnBitMask;
    if (bUseEliteMask && tSCD.ptSpawnBitMaskElites)
    {
        ptSpawnMaskToUse = tSCD.ptSpawnBitMaskElites;
    }

    int32 nRadiusInCells = Math::FloatRoundToInt(flRadius / 2.5f);
    int32 nRadiusInCellsSqr = nRadiusInCells * nRadiusInCells;

    IVector2D tCenter;
    NavMeshGetPositionByIndex(tSCD.ptNavMeshInst, nSquareIndex, tCenter.x, tCenter.y);

    IVector2D tRadiusInCells(nRadiusInCells, nRadiusInCells);
    IVector2D tStart = tCenter - tRadiusInCells;
    IVector2D tEnd = tCenter + tRadiusInCells;

    if (tStart.x < 0 || tEnd.x >= tSCD.ptNavMeshInst->nSizeX ||
        tStart.y < 0 || tEnd.y >= tSCD.ptNavMeshInst->nSizeY)
    {
        return FALSE;
    }






    BOOL bNavMeshOk = TRUE;


    for (int y = tStart.y; y <= tEnd.y; y++)
    {
        int32 dy = y - tCenter.y;
        int32 dySqr = dy * dy;

        int32 dx = Math::FloatFloorToInt(Math::Sqrt((float)(nRadiusInCellsSqr - dySqr)));


        int32 x1 = tCenter.x - dx;
        int32 x2 = tCenter.x + dx;

        int32 nCellIndex = y * tSCD.ptNavMeshInst->nSizeX + x1;
        int32 nDWordIndex = nCellIndex / 32;
        int32 nBitIndex = nCellIndex % 32;

        for (int x = x1; x <= x2; x++)
        {
            BOOL bCanSpawn = ptSpawnMaskToUse[nDWordIndex] & (1 << nBitIndex);

            if (!bCanSpawn)
            {
                bNavMeshOk = FALSE;
                break;
            }


            nBitIndex++;
            if (nBitIndex >= 32)
            {
                nBitIndex = 0;
                nDWordIndex++;
            }
        }

        if (!bNavMeshOk)
        {
            break;
        }
    }

    return bNavMeshOk;
}




void SceneClearSpawnArea(SceneCommonData & tSCD, int nSquareIndex, float flRadius, BOOL bUseEliteMask)




{
    uint32 * ptSpawnMaskToUse = bUseEliteMask ? tSCD.ptSpawnBitMaskElites : tSCD.ptSpawnBitMask;

    if (!ptSpawnMaskToUse || !tSCD.ptNavMeshInst)
    {
        return;
    }

    int32 nRadiusInCells = Math::FloatRoundToInt(flRadius / 2.5f);
    int32 nRadiusInCellsSqr = nRadiusInCells * nRadiusInCells;

    IVector2D tCenter;
    NavMeshGetPositionByIndex(tSCD.ptNavMeshInst, nSquareIndex, tCenter.x, tCenter.y);

    IVector2D tRadiusInCells(nRadiusInCells, nRadiusInCells);
    IVector2D tStart = tCenter - tRadiusInCells;
    IVector2D tEnd = tCenter + tRadiusInCells;

    int32 nMaxX = tSCD.ptNavMeshInst->nSizeX - 1;
    int32 nMaxY = tSCD.ptNavMeshInst->nSizeY - 1;

    tStart.x = Math::Pin(tStart.x, 0, nMaxX);
    tStart.y = Math::Pin(tStart.y, 0, nMaxY);
    tEnd.x = Math::Pin(tEnd.x, 0, nMaxX);
    tEnd.y = Math::Pin(tEnd.y, 0, nMaxY);


    for (int y = tStart.y; y <= tEnd.y; y++)
    {
        int32 dy = y - tCenter.y;
        int32 dySqr = dy * dy;

        int32 dx = Math::FloatFloorToInt(Math::Sqrt((float)(nRadiusInCellsSqr - dySqr)));


        int32 x1 = tCenter.x - dx;
        int32 x2 = tCenter.x + dx;

        x1 = Math::Pin(x1, 0, nMaxX);
        x2 = Math::Pin(x2, 0, nMaxX);


        int32 nCellIndex = y * tSCD.ptNavMeshInst->nSizeX + x1;
        int32 nDWordIndex = nCellIndex / 32;
        int32 nBitIndex = nCellIndex % 32;

        for (int x = x1; x <= x2; x++)
        {

            ptSpawnMaskToUse[nDWordIndex] &= ~(1 << nBitIndex);


            nBitIndex++;
            if (nBitIndex >= 32)
            {
                nBitIndex = 0;
                nDWordIndex++;
            }
        }
    }
}



TEnvAppDataPool * SceneCommonGetEnvAppDataPool()
{
    GameCommonData & tGCD = GameGetActiveGameCommonData();
    if (tGCD.ptSceneCommonGlobals)
        return &tGCD.ptSceneCommonGlobals->tEnvAppDataPool;
    return NULL;
}







void ScenePrintDeactivationReasonsString()

{


    if (GameIsParamFlagSet(GPF_KEEP_ALL_SCENES_ACTIVE))
    {

    }
    else
    {


        int32 nScenesActive = 0;

        SceneCommonData * ptSCD;
        SceneCommonDataIterator tIterator;

        while (SceneCommonDataArrayIterate(tIterator, ptSCD, SkipHolodeckScenes))
        {
            CRefString strReasons;
            BOOL bBeingKeptActive = sSceneGetDeactivationReasonsString(ptSCD, nScenesActive, strReasons, 0);
            (void)bBeingKeptActive;



        }

        CRefString strNumScenes;
        strNumScenes.Format("%d Active Scenes\n", nScenesActive);


    }

}

// Linker-removed in the original image (DW_AT_low_pc 0, size 0x120); only its prototype, locals and their lines are in the DWARF.
// It is the caller that uses sSceneGetDeactivationReasonsString's result, which keeps that static's return value alive. Body reconstructed, unchecked; the strings it built are not recoverable and are left out.
CRefString SceneGetDeactivationReasonsString()
{

    CRefString strReasons;

    if (!GameIsParamFlagSet(GPF_KEEP_ALL_SCENES_ACTIVE))
    {




        int32 nScenesActive = 0;
        BOOL bNoScenesKeptActive = TRUE;

        SceneCommonData * ptSCD;
        SceneCommonDataIterator tIterator;

        while (SceneCommonDataArrayIterate(tIterator, ptSCD, SkipHolodeckScenes))
        {
            if (sSceneGetDeactivationReasonsString(ptSCD, nScenesActive, strReasons, 0))
            {
                bNoScenesKeptActive = FALSE;
            }
        }



        CRefString strNumScenes;
        (void)bNoScenesKeptActive;




    }

    return strReasons;
}



static BOOL sSceneGetDeactivationReasonsString(SceneCommonData * ptSCD, int32 & nScenesActive, CRefString & strReasons, int32 nChildLevel)




{
    if (!(ptSCD->dwSceneFlags & (1 << SCENE_ACTIVATED)))
    {
        return FALSE;
    }

    nScenesActive++;

    if (nChildLevel == 0 && ptSCD->idSSceneParent != SSCENEID_INVALID)
    {
        return FALSE;
    }

    if (ptSCD->dwScenePlayerSeenBits != 0)
    {
        return FALSE;
    }

    if (ptSCD->dwSceneFlags & (1 << SCENE_FLAGGED_FOR_DELETE))
    {
        return FALSE;
    }

    CRefString sIndent("");
    for (int32 i = 0; i < nChildLevel; i++)
    {
        sIndent += "    ";
    }

    CRefString strScene;
    CRefString strTemp;
    strScene.Format("%s[%s] : ", sIndent.str(), SNOToString(SCENE_SNO, ptSCD->snoScene, FALSE).str());
    strReasons += strScene;

    if (ptSCD->dwSceneFlags & (1 << SCENE_LOCKED))
    {
        strReasons += "SCENE_LOCKED\n";
    }
    else
    {
        CRefString strActors;
        SScene * ptSScene = SSceneGet(ptSCD->idSScene);
        POS pos = ptSScene->listACDInScene.GetHeadPosition();
        BOOL bHasActorsThatCantDeactivate = FALSE;

        while (pos)
        {
            ACDID idACD = (ACDID)ptSScene->listACDInScene.GetNext(pos);

            ActorCommonData * ptACD = ACDTryToGet(idACD);
            ActorID idActor = (ActorID)ACDGetOwner(idACD);
            Actor * ptActor = ActorTryToGet(idActor);
            if (ptACD && ptActor)
            {
                DeactivateStatus nStatus = MarkerCanDeactivateActor(idActor);
                CRefString strActorReason;
                switch (nStatus)
                {
                case DEACTIVATE_NO_PLAYER_SEEN:
                    strActorReason = "Player Seen Recently";
                    break;
                case DEACTIVATE_NO_TOWN_PORTAL:
                    strActorReason = "Is a Town Portal";
                    break;
                case DEACTIVATE_NO_PET:
                    strActorReason = "Is a Pet";
                    break;
                case DEACTIVATE_NO_ESCORT:
                    strActorReason = "Escorting a Player";
                    break;
                case DEACTIVATE_NO_DOESNT_DEACTIVATE:
                    strActorReason = "Doesnt Deactivate";
                    break;
                case DEACTIVATE_NO_DYING:
                    strActorReason = "Is Dying";
                    break;
                case DEACTIVATE_NO_COMPLEX_EFFECT:
                    strActorReason = "Is Complex Effect";
                    break;
                case DEACTIVATE_NO_HAS_TIMER:
                    strActorReason = "Has Active Timer";
                    break;
                case DEACTIVATE_NO_UNDEFINED_GIZMO:
                    strActorReason = "Is Undefined Gizmo";
                    break;
                case DEACTIVATE_NO_SCRIPT:
                    strActorReason = "Referenced By Active Script";
                    break;
                case DEACTIVATE_NO_QUEUED_ACTIONS:
                    strActorReason = "Has Queued Actions";
                    break;
                case DEACTIVATE_NO_TALKING:
                    strActorReason = "Is Talking To Player";
                    break;
                case DEACTIVATE_NO_NOT_IN_SCENE:
                    strActorReason = "Not In A Scene";
                    break;
                case DEACTIVATE_NO_UNINTERRUPTABLE_POWER:
                    strActorReason = "Uninterruptable Power";
                    break;
                case DEACTIVATE_NO_ATTRIBUTE:
                    strActorReason = "Attribute";
                    break;
                default:
                    continue;
                }

                bHasActorsThatCantDeactivate = TRUE;





                strTemp.Format("    %s (%d)-(%s): %s\n", SNOToString(SNOName(ACTOR_SNO, ptACD->snoActor), FALSE).str(), ptACD->snoActor, MarkerNameGetString(ptACD->tName), strActorReason.str());
                strActors += strTemp;
            }
        }

        if (bHasActorsThatCantDeactivate)
        {
            strReasons += "Actor(s) Keeping Scene Alive\n";
            strReasons += sIndent + strActors;
        }
        else
        {
            strReasons += "\n";
        }
    }

    POS pos = ptSCD->tChildSceneIDs.GetHeadPosition();
    while (pos)
    {
        SceneCommonData * ptSCDChild = SceneCommonDataTryToGet((SSceneID)ptSCD->tChildSceneIDs.GetNext(pos));
        if (ptSCDChild)
        {

            sSceneGetDeactivationReasonsString(ptSCDChild, nScenesActive, strReasons, 1);
        }
    }

    return TRUE;
}
