// Original file: X/Server/SWorld.cpp
// Partly written: functions sit on their original lines (see the reference .cpp).
#include "SGameConstants.h"  // enum-only header: SGAMEGLOBALS2_* bits used by SWorldDungeonFinderSpawnBoss
#include "SWorld.h"
#include "SGame.h"
#include "SGameManager.h"
#include "NoSpawnTree.h"
#include "Timers.h"
#include "PlayerList.h"
#include "Definitions/DungeonFinderDefinition.h"
#include "System/SetEncounter.h"
#include "System/LevelArea.h"
#include "SScene.h"
#include "../Common/World.h"
#include "../Common/SceneCommon.h"
#include "../Common/GlobalSNO.h"
#include "../Common/ActorCommonData.h"
#include "Actors/Actors.h"
#include "../Common/Shell/CmdLine.h"
#include "../Common/GameConstants.h"
#include "../Common/ActorCommonDataSwig.h"
#include "../Common/GameCommonData.h"
#include "Map/Minimap.h"
#include "../../Contrib/Contrib/blz/include/blz/list.h"
#include "../Common/Target.h"
#include "../Common/Powers/PowersCommon.h"
#include "Powers/Powers.h"
#include "../../Contrib/Contrib/Domino/Include/dmStructures.h"
#include "../Common/Definitions/WorldDefinition.h"
#include "../Common/Game.h"
#include "LuaScript/LuaScript.h"
#include "../Common/Nav/NavigationMesh.h"
#include "../Common/Nav/NavCellHandle.h"
#include "../Client/SNOPreload.h"
#include "../Client/CGame.h"
#include "../Common/Player.h"
#include "../Common/NephalemRiftCommon.h"
#include "SPlayer.h"
#include "../Common/Networking/GameMessage.h"
#include "../Sigma/ErrorManager/ErrorManager.h"
#include "../Sigma/DataStructures/XPooledList.h"
#include "../Sigma/DataStructures/XPooledMap.h"
#include "../Sigma/DataStructures/XVar.h"
#include "../Common/TagMaps/ActorTags.h"
#include "../Common/InactiveMarker.h"
#include "../Common/TeamCommon.h"
#include "../Common/Definitions/ActorDefinition.h"
#include "../Common/Protobuf/Leaderboard.pb.h"
#include <float.h>
#include <limits.h>
#include "../Common/Definitions/BossEncounterDefinition.h"
#include "../Common/Powers/PowerAPISwig.h"
#include "../Common/Networking/Network.h"
int32 WorldGetDungeonFinderLevel(const SNO snoWorld);  // X/Common/World.cpp:828
void SWorldRunDungeonFinderEngine();
void SWorldRemovePlayersAndFlagForDeletion(SWorldID idSWorld);
void SWorldCreateUpdate(BOOL bFlushing);
uint32 SWorldGetDungeonFinderWorldSeed(SNO snoWorld);
ACDID SPlayerGetStartACD(const SWorldID idSWorld, const SSceneID idSScene, const Team eTeam, const uint32 uEntranceGUID);  // X/Server/SPlayer.cpp:7782
void SceneCommonDataActivate(const SSceneID idSScene);  // X/Common/SceneCommon.cpp:729
SWorldID WorldCommonFindBySNO(SNO snoWorld, BOOL bIgnorePendingDelete);  // X/Common/World.cpp:605
void SPlayerClearAllReturnPoints(Player & tPlayer, SNO snoWorld);  // X/Server/SPlayer.cpp:10981
void SPortalMovePlayerToActSpawnPoint(const Player & tPlayer);  // X/Common/PortalCommon.cpp:196
void PowersForceStopForWorld(SWorldID idSWorld);  // X/Server/Powers/Powers.cpp:937
void LevelAreaWorldFlagForDeletion(SWorldID idSWorld, uint32 dwDeleteTime);  // X/Server/System/LevelArea.cpp:654
void NPCHandleDismissal(ActorCommonData & tACDHireling, BOOL bPlayerDismissed, BOOL bDisplayChatMessage);  // X/Server/SNPC.cpp:747
ACDID GetLastHeadstoneACD(const ActorCommonData & tACD);  // X/Server/AI/BrainComponents.cpp
void DeleteLastHeadstone(ActorCommonData & tACD);  // X/Server/AI/BrainComponents.cpp
void SQuestsOnWorldMonstersRemainingModified(SNO snoWorld, int32 nMonstersRemaining);  // X/Server/Quests/SQuests.cpp:4644
int32 SWorldGetMonsterCount(SWorldID idSWorld);
void sInitializeWorld(SNO snoWorld, SWorld * ptSWorld);
void ActorCollisionInitializeForGame();
void SceneSpawnBitMaskInitialize(SceneCommonData & tSCD, const PassabilityType ePassabilityType, BOOL bIncludeEliteMask);  // X/Common/SceneCommon.cpp
void SceneSpawnBitMaskDispose(SceneCommonData & tSCD);  // X/Common/SceneCommon.cpp
void MarkersInstantiateFailedDistribution(SceneCommonData & tSCD, BOOL bEnforceEliteSeparation);  // X/Server/Marker/
void WorldCacheSceneValues(const WorldDefinition * pDefWorld);  // X/Common/World.cpp:240
void WorldCommonDataInitialize(WorldCommonData & tWCD, SNO snoWorld, SWorldID idSWorld, int32 nOriginX, int32 nOriginY, int32 nStitchSizeInFeetX, int32 nStitchSizeInFeetY, int32 nWorldSizeInFeetX, int32 nWorldSizeInFeetY, SNO snoDungeonFinderSourceWorld);
BOOL SNOCheckSNOReference(SNOGroup snoGroupSrc, SNO snoHandleSrc, SNOGroup snoGroupDst, SNO snoHandleDst, LPCSTR szExtraMsg);
void ACDCollInitializeForGame();
void NavMeshInitializeForGame();
void ActorCollisionDisposeForGame();
void ACDCollDisposeForGame();
void NavMeshDisposeForGame();
void SWorldDispose(SWorldID idSWorld);
void ActorsDeleteForWorld(SWorldID idSWorld);
void NodePathsDeleteForWorld(SWorldID idSWorld);
void SScenesDisposeForWorld(SWorldID idSWorld);
void WorldCommonDataDispose(WorldCommonData & tWCD);
enum KnownEntryType { KET_WORLD_REVEAL = 2 };  // X/Server/Network/KnownList.h (subset)
void LuaScriptDetach(ScriptAttachmentType eType, DataID idAttachment, SNO snoScript);  // X/Server/LuaScript/LuaScript.cpp:1665
void KnownListItemDeleted(KnownEntryType eType, DataID idObject);  // X/Server/Network/KnownList.cpp:1166
uint32 NavMeshGetDynamicFlagsForPassability(PassabilityType ePassability);  // X/Common/Nav/NavigationMesh.cpp:3187
BOOL WorldCollGetArbitraryPointInScene(const SSceneID idSScene, uint16 uOneRequiredStaticFlags, uint32 uObjectNoOverlapFlags, Vector3D & wpResult, NavCellHandle & hNavCellResult);  // X/Common/WorldCommonCollision.cpp:624
BOOL WorldCollSearchForLocation(const WorldPlace & placeStart, uint16 uOneRequiredStaticFlags, uint32 uObjectNoOverlapFlags, float flLocationRadius, float flSearchRadius, float flSearchRadiusMax, ACDID idACDIgnore, Vector3D & wpEnd, NavCellHandle & hNavCellEnd, Team eTeam, uint16 uObjectNoPassthroughFlags, ACDID idOwner);  // X/Common/WorldCommonCollision.cpp:241
void SScenesFlagWorldScenesForDeletion(SWorldID idSWorld);  // X/Server/SScene.cpp
void SWorldRemoveAllHirelings(SWorldID idSWorld);
void DungeonFinderCreate(SNO snoDungeonFinder, RNG & rng, DungeonFinderLevelInstance * ptLevels, uint32 & nNumLevels, const DungeonFinderOverrides * ptOverrides);  // X/Server/DungeonFinder.cpp
namespace NephalemRiftManager {
BOOL IsTieredRiftActive();  // X/Server/System/SNephalemRift.cpp:1853
int32 GetActiveRiftLevel();  // X/Server/System/SNephalemRift.cpp:1893
}
SNO WorldGetDungeonFinderWorldFromLevel(int32 nLevel);  // X/Common/World.cpp:847
static const int32 MAX_DUNGEON_FINDER_LEVELS = 10;  // X/Server/Definitions/DungeonFinderDefinition.h:18 (not in src's copy of that header)
namespace BossEncounterManager { BOOL IsAnyEncounterActive(); SNO GetActiveEncounterSNO(); }  // X/Server/System/BossEncounter.cpp:1453, 1467
static const int32 NOSPAWN_CIRCLE_BLOCK_COUNT = 128;
static const int32 NOSPAWN_NODE_BLOCK_COUNT = 512;
static const int32 WORLD_CREATE_POOL_BLOCK_COUNT = 32;
static const int32 MINIMAP_DISCOVERY_POOL_BLOCK_COUNT = 32;

static const int32 WORLD_TRACKED_SNO_POOL_BLOCK_COUNT = 128;
static const int32 WORLD_TRACKED_UNIQUE_MONSTER_ACTOR_ENTRY_POOL_BLOCK_COUNT = 64;
static const int32 MIN_LIVE_WORLDS = 8;

static const int32 BOUNTY_TYPE_CLEAR_DUNGEON_SHOW_REMAINING_THRESHOLD = 15;
namespace NephalemRiftManager { void Cleanup(); }  // X/Server/System/SNephalemRift.cpp:1837
void PowerAPI_BuffFlagAllFromPowerForDeletion(ActorCommonData & tACD, SNO snoPower, int32 nBuffIndex, BuffRemovalReason eRemovalReason, ACDID idActorSource);  // X/Server/Powers/PowerAPI.cpp:10989
void ActorChangeWorld(const ActorID idActor, const WorldPlace & tPlace, const Quaternion & q);  // X/Server/Actors/Actors.cpp:6093
ACDID SWorldGetPortalDestinationACD(const SWorldID idSWorld, const uint32 uEntranceGUID);  // X/Server/SWorld.cpp:1375
static XVarScope s_scopeSWorld("SWorld", NULL);
XVarUint32 s_varMaxLiveWorlds(s_scopeSWorld, "MaxLiveWorlds", 256);
XVarUint32 s_varActorThresholdForDeletingWorlds(s_scopeSWorld, "ActorThresholdForDeletingWorlds", 10304);

static BOOL sfWorldPreloadingSuspended;
struct WorldCreateData {  // size 0x1c
    SWorldID idSWorld;  // 0x00
    ACDID idACDPortalOperator;  // 0x04
    ACDID idACDPortal;  // 0x08
    uint32 uEntranceGUID;  // 0x0c
    PlayerWarpReason eWarpReason;  // 0x10
    uint32 unk_14 = 0;  // 0x14 (ORed into SWorld::dwFlags by SWorldCreateUpdate; the target zeroes it when a list node is built)
    Blizzard::Time::Millisecond uTimeFinalized;  // 0x18
};
static_assert(sizeof(WorldCreateData) == 0x1c, "WorldCreateData");

struct SWorldGlobals {  // size 0x5f0
    NoSpawnCirclePool tNoSpawnCirclePool;  // 0x00
    NoSpawnNodePool tNoSpawnNodePool;  // 0x30
    XListMemoryPool<WorldCreateData> poolWorldCreateData;  // 0x60
    XPooledList<WorldCreateData, WorldCreateData> listWorldsToCreate;  // 0x90
    XListMemoryPool<int> poolTrackedSNOs;  // 0xb8
    XListMemoryPool<UniqueMonsterActorEntry> poolUniqueMonsterActorEntries;  // 0xe8
    XMapMemoryPool<unsigned int, Minimap::DiscoveryInfo> poolMinimapDiscoveryInfo;  // 0x118
    TGizmoLocSpawnMap mapGizmoLocSpawns;  // 0x148
    BOOL bDungeonFinderDataInitialized;  // 0x1c8
    DungeonFinderLevelInstance tDungeonFinderLevelData[10];  // 0x1cc
    uint32 nNumDungeonFinderLevels;  // 0x58c
    SNO snoDungeonFinderBossEncounter;  // 0x590
    SNO snoDungeonFinderEliteEncounter;  // 0x594
    DungeonFinderPylonInstance tDungeonFinderPylonData[5];  // 0x598
    uint32 nNumDungeonFinderPylons;  // 0x5e8
};
static_assert(sizeof(SWorldGlobals) == 0x5f0, "SWorldGlobals");

BOOL EncounterResolveToActorSpawnInfo(SNO snoEncounter, RNG & rng, SNOName & tSNOName, Team & eTeam);  // X/Common/Definitions/EncounterDefinition.cpp:384
ActorID GBAddWithEncounterAddStruct(ActorAddStruct & tActorAdd, EncounterAddStruct & tEncounterAdd);  // X/Server/Actors/Actors.cpp:5915
uint32 MarkerStringHash(LPCSTR pszName);  // X/Common/Marker/Marker.cpp:204
GBHandlePool * GBGetHandlePool();  // X/Common/Definitions/GameBalanceDefinition.cpp:3510
void RandSetSeed_Internal(RNG & rng, const uint32 dwNewSeed);  // X/Sigma/Random.cpp:91






enum WarpToSceneMethod {
    WTSM_WAYPOINT = 0,
    WTSM_CHECKPOINT = 1,
    WTSM_SAVEDPOINT = 2,
};




static BOOL sSWorldPlayerWarpToScene(const ACDID idACDPlayer, SSceneID idSScene, const WarpToSceneMethod eWTSM, const ACDID idACDWarpGizmo, const void * ptWarpData);
SWorld::SWorld()
    : pfCreatedCallback(NULL)
{
    dmWorldDef def;
    def.m_gravity = (dm_vec4){ 0.0f, 0.0f, -32.2f, 0.0f };
    def.m_bodyCapacity = 128;
    def.m_fixtureCapacity = 128;

    def.m_phantomCapacity = 0;
    def.m_convexContactCapacity = 0;
    def.m_meshContactCapacity = 0;
    def.m_distanceJointCapacity = 0;
    def.m_mouseJointCapacity = 0;
    def.m_prismaticJointCapacity = 0;
    def.m_revoluteJointCapacity = 0;
    def.m_shoulderJointCapacity = 0;
    def.m_sphericalJointCapacity = 0;
    def.m_weldJointCapacity = 0;
    def.m_stackCapacity = 0;

    worldId = SGameManager::CreateDominoWorld(&def);
}








SWorld::~SWorld()
{
    if (worldId != dm_nullWorldId)
    {


        SGameManager::DestroyDominoWorld(worldId);
        worldId = dm_nullWorldId;
    }
}














void SWorldsInitializeForGame()
{

    ActorCollisionInitializeForGame();
    ACDCollInitializeForGame();
    NavMeshInitializeForGame();

    SGame & tSGame = SGameManager::GameGet();
    tSGame.ptSWorlds = TDataArrayCreateTracked<SWorld, SWorldID>("SWorlds", tSGame.ptMemoryInterface, 256);

    SWorldGlobals *& ptSWorldGlobals = tSGame.ptSWorldGlobals;

    ptSWorldGlobals = (SWorldGlobals *)SigmaMemoryNew(sizeof(SWorldGlobals), 4, tSGame.ptMemoryInterface, TRUE);

    ptSWorldGlobals->tNoSpawnCirclePool.Initialize();
    ptSWorldGlobals->tNoSpawnCirclePool.SetMemoryInterface(tSGame.ptMemoryInterface);
    ptSWorldGlobals->tNoSpawnCirclePool.SetBlockCount_Tracked(NOSPAWN_CIRCLE_BLOCK_COUNT);

    ptSWorldGlobals->tNoSpawnNodePool.Initialize();
    ptSWorldGlobals->tNoSpawnNodePool.SetMemoryInterface(tSGame.ptMemoryInterface);
    ptSWorldGlobals->tNoSpawnNodePool.SetBlockCount_Tracked(NOSPAWN_NODE_BLOCK_COUNT);

    ptSWorldGlobals->poolWorldCreateData.Initialize();
    ptSWorldGlobals->poolWorldCreateData.SetMemoryInterface(tSGame.ptMemoryInterface);
    ptSWorldGlobals->poolWorldCreateData.SetBlockCount_Tracked(WORLD_CREATE_POOL_BLOCK_COUNT);

    ptSWorldGlobals->poolTrackedSNOs.Initialize();
    ptSWorldGlobals->poolTrackedSNOs.SetMemoryInterface(tSGame.ptMemoryInterface);
    ptSWorldGlobals->poolTrackedSNOs.SetBlockCount_Tracked(WORLD_TRACKED_SNO_POOL_BLOCK_COUNT);

    ptSWorldGlobals->poolUniqueMonsterActorEntries.Initialize();
    ptSWorldGlobals->poolUniqueMonsterActorEntries.SetMemoryInterface(tSGame.ptMemoryInterface);
    ptSWorldGlobals->poolUniqueMonsterActorEntries.SetBlockCount_Tracked(WORLD_TRACKED_UNIQUE_MONSTER_ACTOR_ENTRY_POOL_BLOCK_COUNT);

    ptSWorldGlobals->listWorldsToCreate.Initialize(&ptSWorldGlobals->poolWorldCreateData);

    ptSWorldGlobals->poolMinimapDiscoveryInfo.Initialize();
    ptSWorldGlobals->poolMinimapDiscoveryInfo.SetMemoryInterface(tSGame.ptMemoryInterface);
    ptSWorldGlobals->poolMinimapDiscoveryInfo.SetBlockCount_Tracked(MINIMAP_DISCOVERY_POOL_BLOCK_COUNT);

    ptSWorldGlobals->mapGizmoLocSpawns.Initialize(16, FALSE, NULL);

    ptSWorldGlobals->bDungeonFinderDataInitialized = FALSE;

    SigmaMemoryClear(ptSWorldGlobals->tDungeonFinderLevelData, sizeof(ptSWorldGlobals->tDungeonFinderLevelData));
    ptSWorldGlobals->nNumDungeonFinderLevels = 0;
    ptSWorldGlobals->snoDungeonFinderBossEncounter = SNO_NONE;
    ptSWorldGlobals->snoDungeonFinderEliteEncounter = SNO_NONE;

    SigmaMemoryClear(ptSWorldGlobals->tDungeonFinderPylonData, sizeof(ptSWorldGlobals->tDungeonFinderPylonData));
    ptSWorldGlobals->nNumDungeonFinderPylons = 0;
}



NoSpawnCirclePool * SWorldGetNoSpawnCirclePool()
{
    SGame & tSGame = SGameManager::GameGet();
    return &tSGame.ptSWorldGlobals->tNoSpawnCirclePool;
}




NoSpawnNodePool * SWorldGetNoSpawnNodePool()
{
    SGame & tSGame = SGameManager::GameGet();
    return &tSGame.ptSWorldGlobals->tNoSpawnNodePool;
}




TGizmoLocSpawnMap & SWorldGetGizmoLocSpawnMap()
{
    SGame & tSGame = SGameManager::GameGet();
    return tSGame.ptSWorldGlobals->mapGizmoLocSpawns;
}





void SWorldsDisposeForGame()
{

    SGame & tSGame = SGameManager::GameGet();
    TDataArrayIterator<SWorld> tIt;
    while (tSGame.ptSWorlds->Iterate(tIt))
    {
        SWorldDispose(tIt.ptElement->id);
    }

    TDataArrayDispose(tSGame.ptSWorlds); tSGame.ptSWorlds = NULL;

    SWorldGlobals *& ptSWorldGlobals = tSGame.ptSWorldGlobals;


    ptSWorldGlobals->mapGizmoLocSpawns.Dispose();
    ptSWorldGlobals->listWorldsToCreate.Dispose();
    ptSWorldGlobals->poolWorldCreateData.Dispose();
    ptSWorldGlobals->poolTrackedSNOs.Dispose();
    ptSWorldGlobals->poolUniqueMonsterActorEntries.Dispose();
    ptSWorldGlobals->poolMinimapDiscoveryInfo.Dispose();
    ptSWorldGlobals->tNoSpawnCirclePool.Dispose();
    ptSWorldGlobals->tNoSpawnNodePool.Dispose();


    SigmaMemoryFree(ptSWorldGlobals, tSGame.ptMemoryInterface); ptSWorldGlobals = NULL;

    ActorCollisionDisposeForGame();
    ACDCollDisposeForGame();
    NavMeshDisposeForGame();
}



BOOL SWorldIsFlaggedForDeletion(SWorldID idSWorld)
{
    SWorld * ptSWorld = SWorldTryToGet(idSWorld);
    if (ptSWorld && ptSWorld->dwDeleteTime)
        return TRUE;
    return FALSE;
}





void SWorldFlagForDeletion(SWorldID idSWorld, uint32 nTicks, BOOL bCheckForPlayers)
{
    SWorld * ptSWorld = SWorldTryToGet(idSWorld);
    if (!ptSWorld)
        return;

    {
        SGame & tSGame = SGameManager::GameGet();
        SWorldGlobals *& ptSWorldGlobals = tSGame.ptSWorldGlobals;























        POS pos = ptSWorldGlobals->listWorldsToCreate.GetHeadPosition();
        while (pos)
        {
            POS posLast = pos;
            WorldCreateData & tData = ptSWorldGlobals->listWorldsToCreate.GetNext(pos);
            if (tData.idSWorld == idSWorld)
            {
                ptSWorldGlobals->listWorldsToCreate.RemoveAt(posLast);

                if (ptSWorld->eCreateStage == SWORLD_CREATE_PRELOADING)
                {
                    sInitializeWorld(ptSWorld->tWCD.snoWorld, ptSWorld);
                }

                if (ptSWorld->fPreloadingStarted)
                {
                    SNOPreload::RemoveServerWorldLoading(ptSWorld->tWCD.snoWorld, FALSE);
                }

                SNOPreload::AddServerWorld(ptSWorld->tWCD.snoWorld);

                if (ptSWorld->tWCD.snoDungeonFinderSourceWorld != SNO_NONE)
                {
                    if (ptSWorld->fPreloadingDungeonFinderStarted)
                    {
                        SNOPreload::RemoveServerWorldLoading(ptSWorld->tWCD.snoDungeonFinderSourceWorld, TRUE);
                    }
                    SNOPreload::AddServerWorld(ptSWorld->tWCD.snoDungeonFinderSourceWorld);
                }
            }
        }
    }

    SScenesFlagWorldScenesForDeletion(idSWorld);

    ptSWorld->dwDeleteTime = GameGetServerTime() + nTicks;
}

void ScriptEventOnWorldDeleted(SNO snoWorld);  // X/Server/LuaScript/ScriptEvent.cpp:1613
void ActGetCurrentStartLocation(ResolvedPortalDestination & tDest);  // X/Common/Act.cpp:260

static void sDeleteWorld(
    SWorld * ptSWorld)
{
    WorldDeletedMessage tMessage;
    tMessage.idSWorld = ptSWorld->id;

    GameCommonData& tGCD = SGameGetGameCommonData();
    PlayerGlobals* ptPlayerGlobals = tGCD.ptPlayerGlobals;
    for (int32 i = 0; i < 4; i++)
    {
        Player & tPlayer = ptPlayerGlobals->tPlayerArray[i];
        ActorCommonData* ptACDPlayer = ACDTryToGet(tPlayer.idPlayerACD);
        if (ptACDPlayer)
        {
            WorldPlace placePlayer = ptACDPlayer->GetPlace();  // only this call survives: what used placePlayer (an assert?) is compiled out
        }
    }




    ScriptEventOnWorldDeleted(ptSWorld->tWCD.snoWorld);

    for (int32 i = 0; i < 4; i++)
    {
        Player & tPlayer = ptPlayerGlobals->tPlayerArray[i];
        ActorCommonData* ptACDPlayer = ACDTryToGet(tPlayer.idPlayerACD);
        if (ptACDPlayer)
        {
            WorldPlace placePlayer = ptACDPlayer->GetPlace();  // as above (rows 457/460 repeat 442/445)
        }
    }




    SWorldDispose(ptSWorld->id);

    PlayerList listDestPlayers( PlayerListGetElementPool() );
    PlayerListGetAllInGame(listDestPlayers, -1, FALSE, -1);

    MessageSendToClient(listDestPlayers, SMSG_WORLD_DELETED, &tMessage, sizeof(tMessage));
}



struct TimeSortableWorld
{
    SWorldID id;
    uint32 dwTimeWithoutBeingVisited;
};




static int sTimeWorldSort(
    const void * pLeft,
    const void * pRight)
{
    const TimeSortableWorld * pLeftEntry = (const TimeSortableWorld *)pLeft;
    const TimeSortableWorld * pRightEntry = (const TimeSortableWorld *)pRight;

    return (int32) pRightEntry->dwTimeWithoutBeingVisited - pLeftEntry->dwTimeWithoutBeingVisited;
}




static void sUpdateWorldTimers()
{


    SGame & tSGame = SGameManager::GameGet();
    TDataArrayIterator<SWorld> tIt;
    while (tSGame.ptSWorlds->Iterate(tIt))
    {

        if (!(tIt->dwFlags & (1 << FLAG_SWORLD_IS_HUB)))
        {
            tIt->dwTimeWithoutBeingVisited++;
        }
    }
}




static uint32 sFillWorldTimerArray(
    TimeSortableWorld * arWorlds)
{

    SGame & tSGame = SGameManager::GameGet();
    uint32 dwTotalWorldCount = tSGame.ptSWorlds->GetTotalCount();
    uint32 dwCurrentWorld = 0;
    TDataArrayIterator<SWorld> tIt;
    while (tSGame.ptSWorlds->Iterate(tIt))
    {
        TimeSortableWorld& info = arWorlds[dwCurrentWorld++];
        info.id = tIt->id;
        info.dwTimeWithoutBeingVisited = tIt->dwTimeWithoutBeingVisited;
    }

    return dwTotalWorldCount;
}





static void sResetTimerForWorldWithPlayers()
{

    PlayerList listPlayers( PlayerListGetElementPool() );
    PlayerListGetAllInGame(listPlayers, -1, FALSE, -1);
    POS pos = listPlayers.GetHeadPosition();
    while (pos)
    {
        Player * ptPlayer = listPlayers.GetNext(pos);
        ACDID idACDPlayer = ptPlayer->idPlayerACD;
        ActorCommonData * ptACDPlayer = ACDTryToGet(idACDPlayer);
        if (ptACDPlayer)
        {
            ActorLocation& loc = ptACDPlayer->GetLocation();
            if (loc.eActorLocation == AL_WORLD)
            {
                SWorldID id = loc.tWorldLocation.idSWorld;
                SWorld * ptSWorld = SWorldTryToGet(id);
                if (ptSWorld)
                {
                    ptSWorld->dwTimeWithoutBeingVisited = 0;
                }
            }
        }
    }
}




static void sResetTimerForTownWorld()
{

    ResolvedPortalDestination tDest;
    ActGetCurrentStartLocation(tDest);


    SWorldID idSWorld = WorldCommonFindBySNO(tDest.snoWorld, TRUE);
    SWorld * ptSWorld = SWorldTryToGet(idSWorld);
    if (ptSWorld)
    {
        ptSWorld->dwTimeWithoutBeingVisited = 0;
    }
}




void SWorldsDeleteFlagged()

{



    // bare block inferred: DWARF lexical block 596-645 holds arWorlds..dwMaxLiveWorlds, and 608 is indented 8 (column 53)
    {
        sUpdateWorldTimers();

        sResetTimerForWorldWithPlayers();

        sResetTimerForTownWorld();


        TimeSortableWorld arWorlds[256];
        uint32 dwTotalWorldCount = sFillWorldTimerArray(arWorlds);

        int32 nNumWorldsToDelete = 0;

        uint32 dwMaxLiveWorlds = s_varMaxLiveWorlds.Get();
        if (dwTotalWorldCount > dwMaxLiveWorlds)
        {
            nNumWorldsToDelete = dwTotalWorldCount - dwMaxLiveWorlds;
        }


        else if (dwTotalWorldCount > MIN_LIVE_WORLDS)
        {
            uint32 dwActorThreshold = s_varActorThresholdForDeletingWorlds.Get();
            uint32 dwActorCount = ActorsGetCount();
            if (dwActorCount > dwActorThreshold)
            {
                nNumWorldsToDelete = 1;
            }
        }






        if (nNumWorldsToDelete > 0)
        {

            qsort(arWorlds, dwTotalWorldCount, sizeof(TimeSortableWorld), sTimeWorldSort);
            for (int32 i = 0; i < nNumWorldsToDelete; i++)
            {
                SWorldID idSWorld = arWorlds[i].id;
                SWorld * ptSWorld = SWorldGet(idSWorld);



                PowersForceStopForWorld(idSWorld);
                SWorldFlagForDeletion(idSWorld, 0, FALSE);
                LevelAreaWorldFlagForDeletion(idSWorld, GameGetServerTime());
                SWorldRemoveAllHirelings(idSWorld);
                Minimap::RemoveAllForWorld(idSWorld);
            }
        }
    }

    SGame & tSGame = SGameManager::GameGet();
    uint32 nTime = GameGetServerTime();
    TDataArrayIterator<SWorld> tIt;
    while (tSGame.ptSWorlds->Iterate(tIt))
    {
        if (tIt->dwDeleteTime != 0 && tIt->dwDeleteTime <= nTime)
        {
            sDeleteWorld(tIt.ptElement);
        }
    }
}



void SWorldUpdateMonstersRemaining()
{
    SGame & tSGame = SGameManager::GameGet();
    TDataArrayIterator<SWorld> tIt;
    while (tSGame.ptSWorlds->Iterate(tIt))
    {
        SWorld * ptSWorld = tIt.ptElement;
        if (ptSWorld->bCheckMonstersRemaining)
        {
            int32 nMonstersRemaining = SWorldGetMonsterCount(ptSWorld->id);
            if (nMonstersRemaining != ptSWorld->nMonstersRemaining)
            {
                ptSWorld->nMonstersRemaining = nMonstersRemaining;
                SQuestsOnWorldMonstersRemainingModified(ptSWorld->tWCD.snoWorld, nMonstersRemaining);
            }
            ptSWorld->bCheckMonstersRemaining = FALSE;
        }
    }
}







struct CachedWaypoint {  // size 0x2c (X/Common/Act.h:13)
    Act eSourceAct;  // 0x00
    WaypointInfo tInfo;  // 0x04
};
typedef blz::vector<CachedWaypoint, blz::allocator<CachedWaypoint> > CachedWaypointArray;  // X/Common/Act.h:19
const CachedWaypointArray & ActGetCachedWaypointArray();  // X/Common/Act.cpp:146
int32 ActGetWaypointByLevelArea(const SNO snoLevelArea, BOOL bCheckActivationState);  // X/Common/Act.cpp:156
void SPlayerWorldCreatingBuffApplyToLocalPlayers(ActorCommonData & tACD);  // X/Server/SPlayer.cpp
void SPlayerWorldCreatingBuffRemoveFromLocalPlayers(ActorCommonData & tACD);  // X/Server/SPlayer.cpp
LevelAreaID LevelAreaFind(const SNO snoLevelArea, const SWorldID idSWorld);  // X/Server/System/LevelArea.cpp
SSceneID LevelAreaGetWaypointSSceneID(LevelAreaID idLevelArea);  // X/Server/System/LevelArea.cpp
void SendPlayerCancelWarp(ACDID idACDPlayer);  // X/Server/Network/ServerMessageSend.cpp
ActorID GBAdd(ActorAddStruct & tActorAdd);  // X/Server/Actors/Actors.cpp
WorldDistance NavMeshGetHeightWorldSpace(const NavMeshHandle & tNavMeshHandle);  // X/Common/Nav/NavigationMesh.cpp:771
DungeonFinderPylonInstance * SWorldGlobalsGetDungeonFinderPylonData();
void MarkersCreateInstancesFromWorld(SWorldID idSWorld, SSceneID idSScene);  // X/Common/Marker/Marker.cpp:2685
BOOL DRLGGenerate(SWorldID idSWorld);  // X/Common/DRLG.cpp:116
void SWorldHandleSpecialMarkersInWorld(SWorldID idSWorld);  // X/Common/DRLG.cpp:845
void SPlayerMarkSceneAsEntered(const SSceneID idSScene);  // X/Server/SPlayer.cpp:4297
void InactiveMarkerActivateAllInScene(SSceneID idSScene);  // X/Common/InactiveMarker.cpp:518
void KnownListItemAdded(KnownEntryType eType, DataID idObject);  // X/Server/Network/KnownList.cpp:728
void SSceneForceDeactivationCheckThisTick();  // X/Server/SScene.cpp:1457
void ACDInventoryHandleEquipmentOrSkillChanges();  // X/Common/ActorInventory.cpp:9222
void SScenePrefetchPortalDestination(SWorldID idSWorld, Player * ptPlayer, uint32 uEntranceGUID);  // X/Server/SScene.cpp:1568
void SScenePrefetchWaypointDestination(SWorldID idSWorld, Player * ptPlayer, SNO snoLevelArea);  // X/Server/SScene.cpp:1643
void SGamePause(BOOL bPaused);  // X/Server/SGame.cpp:7972
void SWorldActivateSceneForUniqueMonsterActor(SWorld * ptSWorld);
struct lua_State;
void DungeonFinderOverridesInit(DungeonFinderOverrides & tDungeonFinderOverrides);  // X/Server/Definitions/DungeonFinderDefinition.cpp:639
int32 LuaSNOScriptCall(SNO snoScript, lua_State * L, int32 nArgumentsCnt, int32 nReturnValCnt, LPCSTR szFuncName);  // X/Server/LuaScript/LuaScript.cpp:1084
lua_State * LuaScriptGetLuaStateWithSNO(SNO snoScript);  // X/Server/LuaScript/LuaScript.cpp:3353
BOOL LuaScriptGetFunction(lua_State * L, LPCSTR szFuncName);  // LuaScriptUtil.cpp:1186
void SGameSendDungeonFinderProgressToPlayer(Player & tPlayer);  // X/Server/SGame.cpp:8491
void SGameSendDungeonFinderTimedEventToPlayer(Player & tPlayer);  // X/Server/SGame.cpp:8506
void SGameSendDungeonFinderSeedToPlayer(Player & tPlayer);  // X/Server/SGame.cpp:8521
void SGameSendDungeonFinderParticipatingPlayerCountToPlayer(Player & tPlayer);  // X/Server/SGame.cpp:8536
void SGameSendDungeonFinderCompletionTimeToPlayer(Player & tPlayer);  // X/Server/SGame.cpp:8549
void SWorldActivateScenesForRift(SWorld * ptSWorld);
void SWorldSpawnPylonOverridesForRift(SWorld * ptSWorld);
void SGameSoftPause(BOOL bPaused);  // X/Server/SGame.cpp:7932
ACDID SPortalGetLinkedReturnLocation(ACDID idACDPortal);  // X/Server/SPortal.cpp:543
void SPortalResolveDynamicReturnLocation(ACDID idACDPortalOrigin, ResolvedPortalDestination & tPortalReturnLocation);  // X/Server/SPortal.cpp:568




static void sWaypointWorldCreated(SWorldID idSWorld, ACDID idACDPlayer, ACDID idACDWaypoint, uint32 snoLevelArea, PlayerWarpReason)
{
    // Find the waypoint scene

    // Warp the player there
    ActorCommonData * ptACDPlayer = ACDTryToGet(idACDPlayer);
    if (!ptACDPlayer)
        return;

    SPlayerWorldCreatingBuffRemoveFromLocalPlayers(*ptACDPlayer);

    LevelAreaID idLevelArea = LevelAreaFind(snoLevelArea, idSWorld);
    if (idLevelArea != LEVELAREAID_INVALID)
    {
        SSceneID idSScene = LevelAreaGetWaypointSSceneID(idLevelArea);

        if (idSScene != SSCENEID_INVALID)
        {
            if (sSWorldPlayerWarpToScene(idACDPlayer, idSScene, WTSM_WAYPOINT, idACDWaypoint, NULL))
            {
                Player * ptPlayer = PlayerGetByACD(idACDPlayer);
                if (!ptPlayer)
                    return;

                // the hunter does not tell everybody
                if (GameCommonDataExists() && GameGetActiveGameCommonData().tGameSyncedData.nHunterPlayerIdx >= 0 &&
                    GameGetActiveGameCommonData().tGameSyncedData.nHunterPlayerIdx == ptPlayer->nPlayerIndex)
                {
                    // nothing



                    return;
                }

                PortedToWaypointMessage tMessage;
                tMessage.nPlayerIndex = ptPlayer->nPlayerIndex;
                tMessage.snoLevelArea = snoLevelArea;
                PlayerList listDestPlayers(PlayerListGetElementPool());
                PlayerListGetAllInGame(listDestPlayers, -1, FALSE, -1);
                MessageSendToClient(listDestPlayers, SMSG_PORTED_TO_WAYPOINT, &tMessage, sizeof(tMessage));
                return;
            }
            SendPlayerCancelWarp(idACDPlayer);
            return;
        }
    }

    {












        const SWorld * ptSWorld = SWorldTryToGet(idSWorld);
        int32 nWaypointNumber = ActGetWaypointByLevelArea(snoLevelArea, FALSE);
        const CachedWaypointArray & tWaypoints = ActGetCachedWaypointArray();
        SNO snoAct = ActSNOFromActEnum(tWaypoints[nWaypointNumber].eSourceAct);
    }

    SendPlayerCancelWarp(idACDPlayer);
}











BOOL SWorldPlayerWarpToWaypoint(const ACDID idACDPlayer, const ACDID idACDWaypoint, const int32 nWaypointNumber)
{
    // get the waypoint info



    const CachedWaypointArray & tWaypoints = ActGetCachedWaypointArray();


    const WaypointInfo & tInfo = tWaypoints[nWaypointNumber].tInfo;

    if (tInfo.snoWorld == SNO_NONE || !SNOFileExists(WORLD_SNO, tInfo.snoWorld))
        return FALSE;

    // find the world


    SWorldID idSWorld = WorldCommonFindBySNO(tInfo.snoWorld, TRUE);
    if (idSWorld == SWORLDID_INVALID)
    {
        ActorCommonData & tACDPlayer = ACDGet(idACDPlayer);
        SPlayerWorldCreatingBuffApplyToLocalPlayers(tACDPlayer);

        Player * ptPlayer = PlayerGetByACD(idACDPlayer);
        SWorldCreate(tInfo.snoWorld, FALSE, ptPlayer ? SPlayerIsLocal(*ptPlayer) : FALSE, sWaypointWorldCreated, idACDPlayer, idACDWaypoint, tInfo.snoLevelArea, 2, FALSE, 0);
    }
    else
    {

        SWorld * ptSWorld = SWorldTryToGet(idSWorld);
        if (ptSWorld)
        {
            if (SWorldIsCreated(*ptSWorld))
            {

                sWaypointWorldCreated(idSWorld, idACDPlayer, idACDWaypoint, tInfo.snoLevelArea, 2);
            }
            else
            {
                ptSWorld->AddCreateCallback(sWaypointWorldCreated, idACDPlayer, idACDWaypoint, tInfo.snoLevelArea, 2);
            }
        }
    }

    return TRUE;
}




void SWorldPlayerWarpToLastCheckpoint(
    const Player & tPlayer)
{
    BOOL fRespawnInTown = TRUE;
    // inferred: fRespawnInTown (DWARF line 869, no location) guards the act-spawn fallback at line 977

    if (GameCommonDataExists() && GameGetActiveGameCommonData().tGameSyncedData.nHunterPlayerIdx >= 0)
    {
        if (GameGetActiveGameCommonData().tGameSyncedData.nHunterPlayerIdx == tPlayer.nPlayerIndex)
        {
            int32 nNumPlayers = 0;
            int32 arPlayerIdx[MAX_PLAYERS];
            for (Player* ptCurrPlayer = PlayerGetFirstInGame(); ptCurrPlayer; ptCurrPlayer = PlayerGetNextInGame(ptCurrPlayer))
            {
                if (ptCurrPlayer->nPlayerIndex != tPlayer.nPlayerIndex)
                {
                    arPlayerIdx[nNumPlayers] = ptCurrPlayer->nPlayerIndex;
                    nNumPlayers++;
                }
            }

            int32 nTargetPlayerIdx = tPlayer.nPlayerIndex;
            if (nNumPlayers > 0)
            {
                int32 nRandomArrayIdx = Rand_Internal(SGameGlobalsGet().rng, nNumPlayers - 1);



                nTargetPlayerIdx = arPlayerIdx[nRandomArrayIdx];
            }

            // the hunter warps to a random other player


            Player* ptPlayerTarget = PlayerGetByPlayerIndex(nTargetPlayerIdx);
            if (ptPlayerTarget)
            {
                ActorCommonData* ptACDTarget = ACDTryToGet(ptPlayerTarget->idPlayerACD);
                if (ptACDTarget)
                {
                    if (ACDIsInTieredLootRun(*ptACDTarget))
                    {
                        SWorldID idSWorldTarget = ptACDTarget->GetSWorldID();
                        SWorld * ptSWorldTarget = SWorldTryToGet(idSWorldTarget);
                        if (ptSWorldTarget)
                        {
                            float flWinningDistSqr = 0.0f;
                            WorldPlace placeTarget = ptACDTarget->GetPlace();
                            WorldPlace placeWinner = placeTarget;
                            POS pos = ptSWorldTarget->tEntranceMap.GetStartAssoc();
                            while (pos)
                            {
                                uint32 uPortalGUID;
                                EntranceLocation tEntranceLocation;
                                ptSWorldTarget->tEntranceMap.GetNextAssoc(pos, uPortalGUID, tEntranceLocation);
                                ACDID idACDDest = SWorldGetPortalDestinationACD(ptSWorldTarget->id, uPortalGUID);
                                ActorCommonData * ptACDDest = ACDTryToGet(idACDDest);
                                if (ptACDDest)
                                {
                                    WorldPlace placeDest = ptACDDest->GetPlace();
                                    Vector2D vDiff = (placeDest.wp - placeTarget.wp).As2D();
                                    float flDiffMagSq = vDiff.MagnitudeSquared();
                                    if (flDiffMagSq > flWinningDistSqr)
                                    {
                                        flWinningDistSqr = flDiffMagSq;
                                        placeWinner = placeDest;
                                    }
                                }
                            }

                            if (flWinningDistSqr > EPSILON)
                            {
                                ActorCommonData* ptACDPlayer = ACDTryToGet(tPlayer.idPlayerACD);
                                if (ptACDPlayer)
                                {
                                    ActorID idActorPlayer = (ActorID)ACDGetOwner(ptACDPlayer->id);
                                    WorldPlace placeCurr = ptACDPlayer->GetPlace();
                                    if (WorldPlaceInSameWorld(placeCurr, placeWinner))
                                    {
                                        SPlayerApplyLoadingBuff(*ptACDPlayer, *ptACDPlayer, FALSE, FALSE);
                                        ActorSnapToPosition(idActorPlayer, placeWinner, TRUE, TRUE);
                                    }
                                    else
                                    {
                                        ActorChangeWorld(idActorPlayer, placeWinner, cgqIdentity);
                                    }

                                    SPlayerWarped(*ptACDPlayer, WARP_CHECKPOINT, &placeCurr, ACDID_INVALID, 0.0f, TRUE);
                                    return;
                                }
                            }
                        }
                    }
                }
            }
        }
    }



    if (tPlayer.tLastCheckpoint.IsValid())
    {
        BOOL bResult = sSWorldPlayerWarpToScene(tPlayer.idPlayerACD, SSCENEID_INVALID, WTSM_CHECKPOINT, ACDID_INVALID, &tPlayer.tLastCheckpoint);
        if (bResult)
        {
            fRespawnInTown = FALSE;
        }
    }

    if (fRespawnInTown)
    {


        SPortalMovePlayerToActSpawnPoint(tPlayer);
    }
}

void SWorldPlayerWarpToLastSavepoint(const Player & tPlayer)
{
    const SavePointData & tData = tPlayer.tHeroStateData.tSavedData.tLastSavePoint;
    if (tData.IsValid())
        sSWorldPlayerWarpToScene(tPlayer.idPlayerACD, SSCENEID_INVALID, WTSM_SAVEDPOINT, ACDID_INVALID, &tData);
}







void SWorldPlayerWarpToReturnPoint(const ACDID idACDPlayer)
{
    SGameGlobals & tSGameGlobals = SGameGlobalsGet();
    if (tSGameGlobals.tReturnPoint.idACDReturnPortal != ACDID_INVALID)
    {
        BOOL bResult = sSWorldPlayerWarpToScene(idACDPlayer, SSCENEID_INVALID, WTSM_SAVEDPOINT, ACDID_INVALID, &tSGameGlobals.tReturnPoint);
    }
}







static void sPrintOutOfWorlds()
{




    SGame & tSGame = SGameManager::GameGet();
    TDataArrayIterator<SWorld> tItSWorldForMarkers;
    int32 nCount = 0;
    while (tSGame.ptSWorlds->Iterate(tItSWorldForMarkers))
    {
        SWorld * ptSWorld = tItSWorldForMarkers.ptElement;
        // nCount++ and the loop body are inferred: the original keeps only the line 1020 rows (body compiled out)
        nCount++;
    }
}








void SWorldFlushWorldCreationList()
{
    SGame & tSGame = SGameManager::GameGet();
    while (tSGame.ptSWorldGlobals->listWorldsToCreate.Count() > 0)
        SWorldCreateUpdate(TRUE);
}











void sInitializeWorld(SNO snoWorld, SWorld * ptSWorld)
{

    SWorldID idSWorld = ptSWorld->id;




    {
        ScopedWorldDefinitionLockR pdefWorld(snoWorld);
        WorldCacheSceneValues(pdefWorld);
    }


    WorldCommonDataInitialize(ptSWorld->tWCD, snoWorld, idSWorld, 0, 0, 0, 0, 0, 0, SNO_NONE);

    ScopedWorldDefinitionLockR ptWorld(snoWorld);

    ptSWorld->snoWorldScript = ptWorld->ptServerData->snoScript;
    if (SNOCheckSNOReference(WORLD_SNO, snoWorld, SCRIPT_SNO, ptSWorld->snoWorldScript, NULL))
    {
        LuaScriptAttach(SCRIPT_ATTACH_WORLD, idSWorld, ptSWorld->snoWorldScript, 0xFFFF);
    }


    ptSWorld->snoBossEncounter = SNO_NONE;
    ptSWorld->snoChallenge = SNO_NONE;

    ptSWorld->bChallengeMonstersCleared = FALSE;
    ptSWorld->tChallengeCheckpointPlace.idSWorld = SWORLDID_INVALID;

    ptSWorld->dwFlags = 0;

    ptSWorld->dwFlags = (ptWorld->uFlags >> 8) & (1 << FLAG_SWORLD_IS_HUB);


    ptSWorld->nMonstersRemaining = 0x7FFFFFFF;
    ptSWorld->bCheckMonstersRemaining = FALSE;

    GameType eGameType = GameGetGameType();








    if (WorldIsDungeonFinderPlaceHolder(snoWorld))
    {
        ptSWorld->nSeed = SWorldGetDungeonFinderWorldSeed(snoWorld);
    }
    else if (AppIsDRLGSeedOverridden())
    {
        ptSWorld->nSeed = AppGetOverriddenDRLGSeed();
    }

    else if (CmdLineGetParams()->tBitArray.Get(CMDLINE_DRLG_SEED_BIT))
    {
        ptSWorld->nSeed = CmdLineGetParams()->nDRLGSeed;
    }


    else if (eGameType == GT_DIABLO3)
    {
        SGameGlobals & tSGameGlobals = SGameGlobalsGet();

        ptSWorld->nSeed = Rand_Internal(tSGameGlobals.rng, 0xFFFFFFFD);
    }
    else
    {

        ScopedWorldDefinitionLockR pdefWorld(snoWorld);
        ptSWorld->nSeed = pdefWorld->ptServerData->nSeed;

    }
}




// NON_MATCHING: 1/327 insns: the original tests sfWorldPreloadingSuspended with ldrb+cbnz, ours ldrb+tbnz #0 (the BOOL static is shrunk to i1 in both; looks like pass order). Tried `== FALSE` (worse), `!(a || b)` (same). Paren on 1192 from columns 66/69.
SWorldID SWorldCreate(
    const SNO snoWorld,
    BOOL bCreateImmediately,
    BOOL bCausedByLocalPlayer,
    WorldCreatedCallback pfCreatedCallback,
    ACDID idACDPortalOperator,
    ACDID idACDPortal,
    uint32 uEntranceGUID,
    PlayerWarpReason eWarpReason,
    BOOL fCreateWhenLoadingScreenIsDone, uint32 unk_14)
{



    SGamePause(FALSE);
    SGameSoftPause(FALSE);




    SGame & tSGame = SGameManager::GameGet();
    SWorldID idSWorld = tSGame.ptSWorlds->ElementNew();

    if (idSWorld == SWORLDID_INVALID)
    {
        sPrintOutOfWorlds();
        return SWORLDID_INVALID;
    }



    SWorld * ptSWorld = SWorldGet(idSWorld);

    ptSWorld->listSpawnGroupExact.SetPool(tSGame.ptSpawnGroupExactPool);
    ptSWorld->listSpawnGroupFailed.SetPool(tSGame.ptSpawnGroupExactPool);
    ptSWorld->idActorEntrancePortal = ACTORID_INVALID;
    ptSWorld->dwModifiedBits = 0;
    ptSWorld->dwDeleteTime = 0;
    ptSWorld->vExtMin.Set(FLT_MAX, FLT_MAX);
    ptSWorld->vExtMax.Set(-FLT_MAX, -FLT_MAX);

    ptSWorld->tTownPortalLocation.snoScene = SNO_NONE;
    ptSWorld->tTownPortalLocation.wpPortal = cgplaceNull;
    ptSWorld->tReturnPortalLocation.placeReturn = cgplaceNull;
    ptSWorld->tReturnPortalLocation.idSScene = SSCENEID_INVALID;

    ptSWorld->mapUndiscoveredMinimapMarkers.SetPool(&tSGame.ptSWorldGlobals->poolMinimapDiscoveryInfo);
    ptSWorld->listEventSNOs.SetPool(&tSGame.ptSWorldGlobals->poolTrackedSNOs);
    ptSWorld->listUniqueMonsterActorEntries.SetPool(&tSGame.ptSWorldGlobals->poolUniqueMonsterActorEntries);
    ptSWorld->tDynamicReturnLocation.snoWorld = SNO_NONE;

    ptSWorld->snoWorldMonsterPower = SNO_NONE;
    ptSWorld->snoWorldPlayerPower = SNO_NONE;
    ptSWorld->hWorldQuestDescriptionLabel = (StringLabelHandle)-1;


    ptSWorld->tWCD.snoWorld = snoWorld;
    ptSWorld->tWCD.idSWorld = idSWorld;
    BOOL fWaitForLoadingScreen = (fCreateWhenLoadingScreenIsDone && CGameLoadingScreenIsEnabled());
    if (!fWaitForLoadingScreen && !sfWorldPreloadingSuspended)
    {
        // Inferred: the three flag stores are written in both branches. The original stores them once after the join with no line rows of
        // their own (merged stores), and stores a constant 0 to fWaitForLoadingScreen on this path (mov w8, wzr at 0x765cb8).




        SNOPreload::AddServerWorldLoading(snoWorld, FALSE);
        ptSWorld->dwPreloadingStart = Blizzard::Time::Milliseconds();
        ptSWorld->fPreloadingStarted = TRUE;
        ptSWorld->fPreloadingDungeonFinderStarted = FALSE;
        ptSWorld->fWaitForLoadingScreen = FALSE;
    }
    else
    {
        ptSWorld->fPreloadingStarted = FALSE; ptSWorld->fPreloadingDungeonFinderStarted = FALSE; ptSWorld->fWaitForLoadingScreen = fWaitForLoadingScreen;
    }
    if (idACDPortal != ACDID_INVALID)
    {
        SPortalResolveDynamicReturnLocation(idACDPortal, ptSWorld->tDynamicReturnLocation);





        if (eWarpReason == 13)
        {
            ActorCommonData * ptACDPortal = ACDTryToGet(idACDPortal);
            if (ptACDPortal)
            {
                ptSWorld->snoChallenge = ActorDefinitionGetTagMapValueSNO(TAG_ACTOR_GIZMO_CHALLENGE_SNO, ptACDPortal->snoActor);
                ACDID idACDReturnLocation = SPortalGetLinkedReturnLocation(idACDPortal);
                ActorCommonData * ptACDReturnLocation = ACDTryToGet(idACDReturnLocation);
                if (!ptACDReturnLocation)
                {

                    ptSWorld->tChallengeCheckpointPlace = ptACDPortal->GetPlace();
                    ptSWorld->qChallengeCheckpointOrientation = ptACDPortal->GetOrientation();
                }
                else
                {
                    ptSWorld->tChallengeCheckpointPlace = ptACDReturnLocation->GetPlace();
                    ptSWorld->qChallengeCheckpointOrientation = ptACDReturnLocation->GetOrientation();
                }
            }
        }
    }
    WorldCreateData tWorldCreateData;
    tWorldCreateData.idSWorld = ptSWorld->id;
    tWorldCreateData.idACDPortalOperator = idACDPortalOperator;
    tWorldCreateData.idACDPortal = idACDPortal;
    tWorldCreateData.uEntranceGUID = uEntranceGUID;
    tWorldCreateData.eWarpReason = eWarpReason; tWorldCreateData.unk_14 = unk_14;
    tSGame.ptSWorldGlobals->listWorldsToCreate.AddTail_Tracked(tWorldCreateData);

    ptSWorld->eCreateStage = SWORLD_CREATE_PRELOADING;



    ptSWorld->pfCreatedCallback = pfCreatedCallback;

    if (bCreateImmediately)
    {


        if (bCausedByLocalPlayer)
        {
            CGameServerLoadingPartRequired(LSC_SNO_MANAGER);
        }

        SWorldFlushWorldCreationList();

        if (bCausedByLocalPlayer)
        {
            CGameServerLoadingPartCompleted(LSC_SNO_MANAGER);
        }
    }


    for (int32 i = 0; i < 4; i++)
    {
        ptSWorld->arAllowedPlayers[i] = 0;
    }


    return idSWorld;
}



void SWorldDispose(SWorldID idSWorld)
{

    SGame & tSGame = SGameManager::GameGet();
    BOOL fLoading = FALSE;

    SWorld * ptSWorld = SWorldGet(idSWorld);

    {
        SWorldGlobals *& ptSWorldGlobals = tSGame.ptSWorldGlobals;


        POS pos = ptSWorldGlobals->listWorldsToCreate.GetHeadPosition();
        while (pos)
        {
            POS posLast = pos;
            WorldCreateData & tData = ptSWorldGlobals->listWorldsToCreate.GetNext(pos);
            if (tData.idSWorld == idSWorld)
            {
                fLoading = TRUE;


                ptSWorldGlobals->listWorldsToCreate.RemoveAt(posLast);
            }
        }
    }


    if (fLoading)
    {
        if (ptSWorld->eCreateStage == SWORLD_CREATE_PRELOADING)
        {
            sInitializeWorld(ptSWorld->tWCD.snoWorld, ptSWorld);
        }

        if (ptSWorld->fPreloadingStarted)
        {
            SNOPreload::RemoveServerWorldLoading(ptSWorld->tWCD.snoWorld, FALSE);
        }

        if (ptSWorld->tWCD.snoDungeonFinderSourceWorld != SNO_NONE)
        {
            if (ptSWorld->fPreloadingDungeonFinderStarted)
            {
                SNOPreload::RemoveServerWorldLoading(ptSWorld->tWCD.snoDungeonFinderSourceWorld, TRUE);
            }
        }
    }
    else
    {

        SNOPreload::RemoveServerWorld(ptSWorld->tWCD.snoWorld);
        if (ptSWorld->tWCD.snoDungeonFinderSourceWorld != SNO_NONE)
        {
            SNOPreload::RemoveServerWorld(ptSWorld->tWCD.snoDungeonFinderSourceWorld);
        }
    }




    ActorsDeleteForWorld(idSWorld);
    NodePathsDeleteForWorld(idSWorld);

    ptSWorld->tNoSpawnTree.Dispose();

    SScenesDisposeForWorld(idSWorld);

    if (ptSWorld->snoWorldScript != SNO_NONE)
    {
        LuaScriptDetach(SCRIPT_ATTACH_WORLD, idSWorld, ptSWorld->snoWorldScript);
    }


    WorldCommonDataDispose(ptSWorld->tWCD);

    tSGame.ptSWorlds->ElementDeleteByID(idSWorld);

    KnownListItemDeleted(KET_WORLD_REVEAL, idSWorld);
}











ACDID SWorldGetPortalDestinationACD(const SWorldID idSWorld, const uint32 uEntranceGUID)
{
    SWorld * ptSWorld = SWorldGet(idSWorld);
    EntranceLocation tEntranceLocation;
    if (ptSWorld->tEntranceMap.Lookup(uEntranceGUID, tEntranceLocation))
    {
        SceneCommonDataActivate(tEntranceLocation.idSScene);
        ACDID idACD = SPlayerGetStartACD(idSWorld, tEntranceLocation.idSScene, TEAM_INVALID, uEntranceGUID);
        return idACD;
    }
    return ACDID_INVALID;
}



















SWorldID SWorldFindBySNO(const SNO & snoWorld)
{
    SGame & tSGame = SGameManager::GameGet();
    TDataArrayIterator<SWorld> tIt;
    while (tSGame.ptSWorlds->Iterate(tIt))
    {
        const SWorld & tSWorld = *tIt.ptElement;
        if (tSWorld.tWCD.snoWorld == snoWorld && !tSWorld.dwDeleteTime)
            return tSWorld.id;
    }
    return SWORLDID_INVALID;
}













static BOOL sWorldGetArbitraryStartLocationInScene(
    SWorldID idSWorld,
    SSceneID idSScene,
    WorldPlace & placeEntrance)
{
    SceneCommonDataActivate(idSScene);

    WorldPlace tPlace;
    NavCellHandle hNavCell;
    uint16 uOneRequiredStaticFlags = 1;
    uint32 uDynamicFlagsNoOverlap = NavMeshGetDynamicFlagsForPassability(PASSABILITY_MONSTER);

    BOOL bResult = WorldCollGetArbitraryPointInScene(idSScene, uOneRequiredStaticFlags, uDynamicFlagsNoOverlap, tPlace.wp, hNavCell);

    if (bResult)
    {
        // inferred: the inlined copy in sSWorldPlayerWarpToScene sets the result to the constants 1 / 0
        // (orr w21,#1 / mov w21,wzr), so the search result is not returned directly
        tPlace.idSWorld = idSWorld;

        NavCellHandle hNavCellSearch;
        Vector3D wpSearch;

        if (WorldCollSearchForLocation(tPlace, uOneRequiredStaticFlags, uDynamicFlagsNoOverlap, 3.0f, 0.0f, 20.0f, ACDID_INVALID, wpSearch, hNavCellSearch, TEAM_NEUTRAL, 0, ACDID_INVALID))
        {
            placeEntrance.idSWorld = idSWorld;
            placeEntrance.wp = wpSearch;
            bResult = TRUE;
        }
        else bResult = FALSE;
    }
    return bResult;
}



SSceneID SWorldGetArbitraryStartLocation(SWorldID idSWorld, WorldPlace & placeEntrance)
{
    SceneCommonData * ptSCD = NULL;
    SceneCommonDataIterator tIterator;
    while (SceneCommonDataArrayIterate(tIterator, ptSCD, SkipHolodeckScenes))
    {
        if (ptSCD->idSWorld != idSWorld)
            continue;
        BOOL bResult = sWorldGetArbitraryStartLocationInScene(idSWorld, ptSCD->idSScene, placeEntrance);
        if (bResult)
            return ptSCD->idSScene;
    }

    placeEntrance.idSWorld = idSWorld;
    placeEntrance.wp = cgwpNull;
    return SSCENEID_INVALID;
}
BOOL SPlayerGetStartLocation(const SWorldID idSWorld, const SSceneID idSScene, const Team eTeam, const uint32 uEntranceGUID, WorldPlace & tPlace, Quaternion & q);  // X/Server/SPlayer.cpp:7938
BOOL WorldGetStartLocationOverride(const SWorldID idSWorld, WorldPlace & placeEntrance, Quaternion & qEntrance);  // X/Common/World.cpp:708
void SPlayerGhostedBuffApply(ActorCommonData & tACDPlayer, BOOL bInfiniteDuration);  // X/Server/SPlayer.cpp:8309
uint32 PVPGetDuelingEntranceGUID(const Player & tPlayer, SWorldID idSWorld, BOOL bPickFar);  // X/Server/PVP.cpp:1708
uint32 PVPGetPlayerEntranceGUID(const Player & tPlayer, SWorldID idSWorld);  // X/Server/PVP.cpp:917
BOOL SGameStatsUsedCheats();  // X/Server/SGameStats.cpp:1808
BOOL SPlayerFindGoodSpawnLocation(const WorldPlace & placeStart, WorldPlace & placeResult, float flSearchRadius);  // X/Server/SPlayer.cpp:2289
ACDID PlayerActorCreate(Player & tPlayer, const uint32 dwPlayerFlags, const uint32 dwPlayerFlags2, const WorldPlace & tPlace, const Quaternion & q, const BOOL bResetLifeStats, const D3::Hero::SavedDefinition & tHeroDefinition, const D3::Account::SavedDefinition & tAccountDefinition, const D3::Items::Mails & tMails, const uint32 dwSaveOptionBits);  // X/Server/SPlayer.cpp:2352
void ActorPowersInterruptPrimary(ActorID idActor, BOOL bIgnoreContactFrame);  // X/Server/Actors/ActorPowers.cpp:392




ACDID SWorldPlayerEnter(
    SWorldID idSWorld,
    const ACDID idACDPlayer,
    Player * ptPlayer,
    const uint32 uEntranceGUID,
    const BOOL fAllowStartLocationOverride,
    const D3::Hero::SavedDefinition & tHeroDefinition,
    const D3::Account::SavedDefinition & tAccountDefinition,
    const D3::Items::Mails & tMails,

    const Player * ptReferencePlayer)


{
    Player * ptPlayerToUse = ptPlayer;
    if (idACDPlayer != ACDID_INVALID)
    {
        if (GameIsStartUp())
        {
            ptPlayerToUse = PlayerGetByPlayerIndex(0);
        }
        else
        {
            ptPlayerToUse = ACDGetControllingPlayer(idACDPlayer);
        }
    }


    WorldPlace placeEntrance = cgplaceNull;
    Quaternion qEntrance = cgqIdentity;

    SWorld * ptSWorld = SWorldGet(idSWorld);




    ptSWorld->dwFlags |= (1 << FLAG_SWORLD_HAS_BEEN_ENTERED);







    ScopedWorldDefinitionLockR ptWorldDef(ptSWorld->tWCD.snoWorld);

    if ((ptWorldDef->uFlags >> WDF_DUELING_ARENA) & 1)
    {
        SPlayerGhostedBuffApply(ACDGet(ptPlayerToUse->idPlayerACD), FALSE);
    }

    if (ptReferencePlayer && ptPlayerToUse && ((ptWorldDef->uFlags >> WDF_DUELING_ARENA) & 1) && PlayersAreLocalToEachOther(*ptPlayerToUse, *ptReferencePlayer))
    {
        ActorCommonData& tACD = ACDGet(ptReferencePlayer->idPlayerACD);

        placeEntrance = tACD.GetPlace();
    }
    else
    {



        BOOL bFound = FALSE;
        if (fAllowStartLocationOverride)
        {
            bFound = WorldGetStartLocationOverride(idSWorld, placeEntrance, qEntrance);
        }

        if (!bFound)
        {
            EntranceLocation tEntranceLocation;
            uint32 uGUIDToUse = uEntranceGUID;

            if (uGUIDToUse == (uint32)-1)
            {

                POS pos = ptSWorld->tEntranceMap.GetStartAssoc();
                if (pos)
                {
                    ptSWorld->tEntranceMap.GetNextAssoc(pos, uGUIDToUse, tEntranceLocation);
                }
            }

            Team eTeam = TEAM_INVALID;

            if ((ptWorldDef->uFlags >> WDF_DUELING_ARENA) & 1)
            {
                uGUIDToUse = PVPGetDuelingEntranceGUID(*ptPlayerToUse, idSWorld, FALSE);
            }

            if (GameTypeIsPvP())
            {
                eTeam = ptPlayerToUse->eTeam;
                uGUIDToUse = PVPGetPlayerEntranceGUID(*ptPlayerToUse, idSWorld);
            }

            if (ptSWorld->tEntranceMap.Lookup(uGUIDToUse, tEntranceLocation))
            {
                SceneCommonDataActivate(tEntranceLocation.idSScene);




                bFound = SPlayerGetStartLocation(idSWorld, SSCENEID_INVALID, eTeam, uGUIDToUse, placeEntrance, qEntrance);
            }
        }

        if (!bFound)
        {
            if (ptSWorld->tWCD.snoWorld == GlobalSNOGet(GLOBALSNO_WORLD_MARKERSET_TOOL))
            {
                SceneCommonData *ptSCD = NULL;
                SceneCommonDataIterator tIterator;
                while (SceneCommonDataArrayIterate(tIterator, ptSCD, SkipHolodeckScenes))
                {
                    SceneCommonDataActivate(ptSCD->idSScene);
                }

                placeEntrance.idSWorld = idSWorld;
                placeEntrance.wp.Set(100.0f, 100.0f, 0.0f);
            }
            else
            {
                SWorldGetArbitraryStartLocation(idSWorld, placeEntrance);
            }
            // result unused (line 1623, column 18: probably inside a debug macro)
            SGameStatsUsedCheats();
        }
    }









    ACDID idACDEnteredPlayer = idACDPlayer;

    SPlayerFindGoodSpawnLocation(placeEntrance, placeEntrance, 50.0f);
    ActorCommonData * ptACDGivenPlayer = ACDTryToGet(idACDPlayer);
    if (!ptACDGivenPlayer)
    {
        idACDEnteredPlayer = PlayerActorCreate(
            *ptPlayerToUse,
            ptPlayerToUse->tHeroStateData.uPlayerFlags, ptPlayerToUse->tHeroStateData.uPlayerFlags2,
            placeEntrance,
            qEntrance,
            TRUE,
            tHeroDefinition,
            tAccountDefinition,
            tMails, 0);
    }
    else
    {
        ActorID idActor = (ActorID)ACDGetOwner(idACDPlayer);
        ActorPowersInterruptPrimary(idActor, TRUE);
        ActorChangeWorld(idActor, placeEntrance, qEntrance);
    }










    SSceneForceDeactivationCheckThisTick();

    return idACDEnteredPlayer;
}
SWorldID SceneCommonGetSWorldID(SSceneID idSScene);  // X/Common/SceneCommon.cpp:1891
BOOL NavMeshSearchForLocation(const WorldPlace & placeStart, const PassabilitySettings & tPassability, const float flMinSearchRadius, const float flMaxSearchRadius, const uint32 dwFindLocationFlags, NavMeshHandle & tNavMeshHandleLanding, WorldPlace & placeLanding, BOOL bSearchOverImpassable, Team eTeam, float flRadiusOverride);  // X/Common/Nav/NavigationMesh.cpp:3421
BOOL BrainAPIResurrectByType(ACDID idACD, RevivalType eRevivalType);  // X/Server/AI/BrainComponents.cpp:2981
void ServerMessageSendForceInterfaceClose(ACDID idACD);  // X/Server/Network/ServerMessageSend.cpp


static BOOL sResolveSSceneFromCheckpointData(
    const CheckpointData * ptData,
    SSceneID & idSScene)
{
    if (ptData)
    {
        NavMeshHandle tNavMeshHandle;
        ScenesCommonNavMeshNodeFromPlace(ptData->place, &tNavMeshHandle, TRUE, FALSE);
        idSScene = tNavMeshHandle.idSScene;
        if (SSceneTryToGet(idSScene) && SWorldTryToGet(ptData->place.idSWorld))
        {
            return TRUE;
        }
    }

    return FALSE;
}



BOOL SWorldResolveLocationFromSavePointData(const SavePointData * ptData, WorldPlace & placeLocation)
{

    if (ptData != NULL)
    {
        if (ptData->snoWorld != SNO_NONE)
        {
            SWorldID idSWorld = WorldCommonFindBySNO(ptData->snoWorld, TRUE);
            if (idSWorld == SWORLDID_INVALID)
                return FALSE;




            SWorld * ptSWorld = SWorldTryToGet(idSWorld);
            if (ptSWorld)
            {
                if (ptData->nNum != -1)
                {
                    SavePointLocation tLocation;

                    if (ptSWorld->tSavePortalMap.Lookup(ptData->nNum, tLocation))
                    {
                        placeLocation = tLocation.placeSavePoint;
                        return TRUE;
                    }
                }
                else

                if (ptData->idACDReturnPortal != ACDID_INVALID)
                {

                    ActorCommonData * ptACDDest = ACDTryToGet(ptData->idACDReturnPortal);
                    if (ptACDDest)
                    {
                        placeLocation = ptACDDest->GetPlace();
                        return TRUE;
                    }
                }
            }
        }
    }
    return FALSE;
}




static BOOL sSWorldPlayerWarpToScene(
    const ACDID idACDPlayer,
    SSceneID idSScene,
    const WarpToSceneMethod eWTSM,
    const ACDID idACDWarpGizmo,
    const void * ptWarpData)
{
    ActorCommonData * ptACDPlayer = &ACDGet(idACDPlayer);
    WorldPlace placeEntrance = cgplaceNull;
    Quaternion qEntrance = cgqIdentity;
    // inferred: initial values of eWarpReason/bFound (DWARF 1756/1757; the default case leaves WARP_UNKNOWN)
    PlayerWarpReason eWarpReason = WARP_UNKNOWN;
    BOOL bFound = FALSE;

    switch (eWTSM)
    {
        // case order from the line table: waypoint 1765, checkpoint 1784, saved point 1806, default 1833

        case WTSM_WAYPOINT:
        {
            SScene* ptSScene = SSceneTryToGet(idSScene);
            if (ptSScene && ptSScene->bHasWaypoint)
            {
                placeEntrance.wp = ptSScene->transWaypoint.wp;
                placeEntrance.idSWorld = ptSScene->tSCD.idSWorld;
                qEntrance = ptSScene->transWaypoint.q;
                eWarpReason = WARP_WAYPOINT;
                bFound = TRUE;
            }
        }
        break;






        case WTSM_CHECKPOINT:
        {
            if (!sResolveSSceneFromCheckpointData((const CheckpointData *)ptWarpData, idSScene))
            {
                SendPlayerCancelWarp(idACDPlayer);
                return FALSE;
            }








            const CheckpointData * ptData = (const CheckpointData *)ptWarpData;
            placeEntrance = ptData->place;
            qEntrance = ptData->qOrientation;
            eWarpReason = WARP_CHECKPOINT;
            bFound = TRUE;
        }
        break;
        case WTSM_SAVEDPOINT:
        {
            WorldPlace placeSavePoint;
            const SavePointData * ptData = (const SavePointData *)ptWarpData;
            if (!SWorldResolveLocationFromSavePointData(ptData, placeSavePoint))
            {
                SendPlayerCancelWarp(idACDPlayer);
                return FALSE;
            }







            NavMeshHandle tNavMeshHandle;
            ScenesCommonNavMeshNodeFromPlace(placeSavePoint, &tNavMeshHandle, TRUE, FALSE);
            idSScene = tNavMeshHandle.idSScene;

            placeEntrance = placeSavePoint;
            qEntrance = ptACDPlayer->GetOrientation();
            eWarpReason = WARP_CHECKPOINT;
            bFound = TRUE;
        }
        break;

        default:
        {
            SWorldID idSWorld = SceneCommonGetSWorldID(idSScene);
            bFound = SPlayerGetStartLocation(idSWorld, idSScene, TEAM_INVALID, (uint32)-1, placeEntrance, qEntrance);
            if (!bFound)
            {
                bFound = sWorldGetArbitraryStartLocationInScene(idSWorld, idSScene, placeEntrance);
            }
        }
        break;
    }

    SceneCommonDataActivate(idSScene);

    if (bFound)
    {




        if (eWTSM == WTSM_CHECKPOINT)
        {
            PassabilitySettings tPassability;
            PassabilitySettingsInit(tPassability);
            tPassability.ePassabilityType = PASSABILITY_PLAYER_AI_OFF;
            tPassability.snoActor = ptACDPlayer->snoActor;

            NavMeshHandle tNavMeshHandleResult;
            NavMeshSearchForLocation(placeEntrance, tPassability, 0.0f, 50.0f, 0, tNavMeshHandleResult, placeEntrance, FALSE, TEAM_NEUTRAL, 0.0f);
        }















        Player* ptOriginalPlayer = PlayerGetByACD(idACDPlayer);
        for (Player* ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
        {
            if (!PlayersAreLocalToEachOther(*ptPlayer, *ptOriginalPlayer))
            {
                continue;
            }

            ptACDPlayer = &ACDGet(ptPlayer->idPlayerACD);

            if (!ptACDPlayer->IsAlive())
            {
                BrainAPIResurrectByType(ptPlayer->idPlayerACD, REVIVAL_TYPE_AT_CORPSE_NOW);
            }


            ServerMessageSendForceInterfaceClose(ptACDPlayer->id);



            WorldPlace tPlace = ptACDPlayer->GetPlace();

            ActorID idActor = (ActorID)ACDGetOwner(ptACDPlayer->id);

            if (WorldPlaceInSameWorld(tPlace, placeEntrance))
            {
                SPlayerApplyLoadingBuff(*ptACDPlayer, *ptACDPlayer, FALSE, FALSE);
                ActorSnapToPosition(idActor, placeEntrance, TRUE, TRUE);
                ActorSetOrientation(idActor, qEntrance, TRUE);
            }
            else
            {
                if (!SPlayerCanSwitchToNewWorld(ptACDPlayer->id))
                {
                    return FALSE;
                }


                ActorChangeWorld(idActor, placeEntrance, qEntrance);
            }

            SPlayerWarped(*ptACDPlayer, eWarpReason, &tPlace, idACDWarpGizmo, 0.0f, TRUE);
        }
    }




    return bFound;
}



BOOL SWorld::IsModifiedAny() const
{
    return dwModifiedBits != 0;
}




BOOL SWorld::IsModifiedSpecific(const SWorldModifiedType eModifiedType) const
{
    return (dwModifiedBits & (1 << eModifiedType)) != 0;
}



void SWorld::ModifiedClear()
{
    dwModifiedBits = 0;
}


















void SWorld::SendSegmentIfModified(const PlayerList & listDestPlayers, const SWorldModifiedType eModifiedType)
{
    if (!IsModifiedSpecific(eModifiedType))
        return;

    switch (eModifiedType)
    {
    case SWORLDMODIFIED_WORLD_STATUS:
        SendStatus(listDestPlayers);
        break;
    }
}













void SWorld::SendServerUpdateMessage(const PlayerList & listDestPlayers)
{
    if (!IsModifiedAny())
        return;

    for (int32 i = 0; i < SWORLDMODIFIED_NUM_TYPES; i++)
    {
        SendSegmentIfModified(listDestPlayers, i);
    }
}





void SWorld::SendStatus(const PlayerList & listDestPlayers)
{
    WorldStatusMessage tMessage;
    tMessage.idSWorld = id;
    tMessage.dwWorldStatusBits = tWCD.dwWorldStatusBits;
    MessageSendToClient(listDestPlayers, SMSG_WORLD_STATUS, &tMessage, sizeof(tMessage));
}






void SWorld::SetEntranceLocation(const uint32 uGUID, EntranceLocation & tLocation)
{
    tEntranceMap.SetAt_Tracked(uGUID, tLocation);
}







void SWorld::AddCreateCallback(WorldCreatedCallback _pfCreatedCallback, ACDID idACDPortalOperator, ACDID idACDPortal, uint32 uEntranceGUID, PlayerWarpReason eWarpReason)
{
    WorldCreatedCallbackWrapper tWrapper;
    tWrapper.pfCreatedCallback = _pfCreatedCallback;
    tWrapper.idACDPortalOperator = idACDPortalOperator;
    tWrapper.idACDPortal = idACDPortal;
    tWrapper.uEntranceGUID = uEntranceGUID;
    tWrapper.eWarpReason = eWarpReason;
    tExtraWorldCreatedCallbacks.AddTail_Tracked(tWrapper);
}





























int32 SWorldGetRandSeedFromHash(SWorldID idSWorld, const int32 nHash)
{
    int32 nSeed = nHash;
    SWorld * ptSWorld = SWorldTryToGet(idSWorld);
    if (ptSWorld)
        nSeed += ptSWorld->nSeed;
    return nSeed;
}









int32 SWorldGetRandSeedFromSSceneID(SWorldID idSWorld, SSceneID idSScene)
{
    int32 nSeed = 0;
    SceneCommonData * ptSCD;
    SWorld * ptSWorld = SWorldTryToGet(idSWorld);
    ptSCD = SceneCommonDataTryToGet(idSScene);
    if (ptSWorld && ptSCD)
    {
        nSeed = ptSCD->snoScene;
        nSeed += Math::FloatToInt(ptSCD->transformWorld.wp.x);
        nSeed += Math::FloatToInt(ptSCD->transformWorld.wp.y);
        nSeed += ptSWorld->nSeed;
    }
    return nSeed;
}









































SSceneID SWorldFindFirstSceneInWorld(SWorldID idSWorld, SNO snoScene)
{
    SWorld * ptSWorld = SWorldTryToGet(idSWorld);
    if (ptSWorld)
    {
        SceneCommonData * ptSCD = NULL;
        SceneCommonDataIterator tIterator;
        while (SceneCommonDataArrayIterate(tIterator, ptSCD, SkipHolodeckScenes))
        {
            if (ptSCD->idSWorld == idSWorld && ptSCD->snoScene == snoScene)
                return ptSCD->idSScene;
        }
    }
    return SSCENEID_INVALID;
}













void SWorldRunSpecialNavMeshFunc(SWorldID idSWorld, ActorID idActor, NavMeshSpecialType eSpecialType, NavMeshFuncType eFuncType)
{

    Actor * ptActor = ActorTryToGet(idActor);
    if (ptActor)
    {

        ActorCommonData & tACD = ACDGet(ptActor->idACD);
        WorldCommonData * ptWCD = WorldTryGetCommonData(idSWorld);
        if (ptWCD && ptWCD->snoWorld != SNO_NONE)
        {

            ScopedWorldDefinitionLockR pdefWorld(ptWCD->snoWorld);



            SNO snoPower = pdefWorld->ptServerData->arNavMeshFuncs[eSpecialType * NAVMESH_FUNC_TYPE_COUNT + eFuncType];
            if (snoPower != SNO_NONE)
            {

                Target tTarget;
                tTarget.InitializeFromPlace(tACD.GetPlace());
                if (PowerCanTarget(AutoPowerDefinitionR(snoPower), tACD.id, &tTarget))
                {
                    PowerStart(ptActor->idACD, snoPower, tTarget);
                }
            }
        }
    }
}






BOOL SWorldSpawnGroupExactInstanceIsInScene(SSceneID idSScene, LevelAreaID idLevelArea, const SpawnGroupExactInstance & tSpawnGroupExactInstance)
{
    SceneCommonData * ptSCD = &SceneCommonDataGet(idSScene);
    SNO snoLevelArea = LevelAreaGetSNO(idLevelArea);
    if (tSpawnGroupExactInstance.tLocation.nDRLGIndex == ptSCD->tSceneSpec.nDRLGIndex && snoLevelArea == tSpawnGroupExactInstance.snoLevelAreaOrig)
        return TRUE;
    return FALSE;
}














BOOL SWorldIsCreated(const SWorld & tSWorld)
{
    return tSWorld.eCreateStage == SWORLD_CREATE_COMPLETED;
}





void SWorldsSuspendPreload()
{
    sfWorldPreloadingSuspended = TRUE;
}




void SWorldsResumePreload()
{
    sfWorldPreloadingSuspended = FALSE;
}




// NON_MATCHING: same code, but the shared return block is laid out at the end instead of after the SNOPreload tail (4 bytes larger)
void SWorldCreateUpdate(
    BOOL bFlushing)
{

    SGame & tSGame = SGameManager::GameGet();
    if (tSGame.ptSWorldGlobals->listWorldsToCreate.Count() <= 0)
        return;



    Blizzard::Time::Millisecond tTimeStart = Blizzard::Time::Milliseconds();

    WorldCreateData & tData = tSGame.ptSWorldGlobals->listWorldsToCreate.GetHead();
    SWorldID idSWorld = tData.idSWorld;
    SWorld * ptSWorld = SWorldTryToGet(idSWorld);
    if (ptSWorld)
    {
        switch (ptSWorld->eCreateStage)
        {
        case SWORLD_CREATE_PRELOADING:


            if (!ptSWorld->fPreloadingStarted)
            {
                if (sfWorldPreloadingSuspended)
                    return;



                if (ptSWorld->fWaitForLoadingScreen)
                {
                    if (CGameLoadingScreenIsEnabled())
                        return;
                }



                SNOPreload::AddServerWorldLoading(ptSWorld->tWCD.snoWorld, FALSE);
                ptSWorld->dwPreloadingStart = Blizzard::Time::Milliseconds();
                ptSWorld->fPreloadingStarted = TRUE;
            }

            if (!ptSWorld->fPreloadingDungeonFinderStarted)
            {
                if (!SNOPreload::IsServerWorldLoadingDone(ptSWorld->tWCD.snoWorld, FALSE))
                    return;





                sInitializeWorld(ptSWorld->tWCD.snoWorld, ptSWorld);

                if (ptSWorld->tWCD.snoDungeonFinderSourceWorld != SNO_NONE)
                {
                    SNOPreload::AddServerWorldLoading(ptSWorld->tWCD.snoDungeonFinderSourceWorld, TRUE);
                }
                ptSWorld->dwFlags |= tData.unk_14;
                ptSWorld->fPreloadingDungeonFinderStarted = TRUE;
            }

            if (ptSWorld->tWCD.snoDungeonFinderSourceWorld != SNO_NONE)
            {
                if (!SNOPreload::IsServerWorldLoadingDone(ptSWorld->tWCD.snoDungeonFinderSourceWorld, TRUE))
                    return;
            }
            break;




        case SWORLD_CREATE_START:

            ptSWorld->tNoSpawnTree.Initialize();
            break;

        case SWORLD_CREATE_WORLD_INSTANCES:

            MarkersCreateInstancesFromWorld(idSWorld, SSCENEID_INVALID);
            break;

        case SWORLD_CREATE_DRLG:

            DRLGGenerate(idSWorld);
            break;

        case SWORLD_CREATE_SPECIAL_MARKERS:
            SWorldHandleSpecialMarkersInWorld(idSWorld);
            break;

        case SWORLD_CREATE_FINALIZE:
            {
                SWorldActivateSceneForUniqueMonsterActor(ptSWorld);



                SWorldActivateScenesForRift(ptSWorld);


                SWorldSpawnPylonOverridesForRift(ptSWorld);

                if (GameIsParamFlagSet(GPF_KEEP_ALL_SCENES_ACTIVE))
                {




                    if (GameActiveGameCommonDataIsServer())
                    {
                        SceneCommonData * ptSCD = NULL;
                        SceneCommonDataIterator tIterator;
                        while (SceneCommonDataArrayIterate(tIterator, ptSCD, SkipHolodeckScenes))
                        {
                            SPlayerMarkSceneAsEntered(ptSCD->idSScene);
                            SceneCommonDataActivate(ptSCD->idSScene);
                            InactiveMarkerActivateAllInScene(ptSCD->idSScene);
                        }
                    }
                }

                KnownListItemAdded(KET_WORLD_REVEAL, idSWorld);
                SSceneForceDeactivationCheckThisTick();
                ACDInventoryHandleEquipmentOrSkillChanges();


                if (tData.eWarpReason == 2)
                {
                    SScenePrefetchWaypointDestination(idSWorld, NULL, tData.uEntranceGUID);
                }
                else if (tData.uEntranceGUID != (uint32)-1)
                {
                    SScenePrefetchPortalDestination(idSWorld, NULL, tData.uEntranceGUID);
                }


                tData.uTimeFinalized = Blizzard::Time::Milliseconds();
            }
            break;

        case SWORLD_CREATE_WAIT_PREFETCH:
            {


                {

                    const Blizzard::Time::Millisecond uWaitTime = 2000;
                    const Blizzard::Time::Millisecond uNow = Blizzard::Time::Milliseconds();
                    Blizzard::Time::Millisecond uElapsed = uNow - tData.uTimeFinalized;
                    if (!bFlushing && uElapsed < uWaitTime)
                        return;
                }
            }
            break;
        }






        Blizzard::Time::Millisecond tTimeElapsed = Blizzard::Time::Milliseconds() - tTimeStart;
        if (OnlineService::IsPTR())
        {
        }













        ptSWorld->eCreateStage++;


        if (ptSWorld->eCreateStage < SWORLD_CREATE_COMPLETED)
            return;
        if (ptSWorld->pfCreatedCallback)
        {
            ptSWorld->pfCreatedCallback(ptSWorld->id, tData.idACDPortalOperator, tData.idACDPortal, tData.uEntranceGUID, tData.eWarpReason);
        }

        while (!ptSWorld->tExtraWorldCreatedCallbacks.IsEmpty())
        {
            SWorld::WorldCreatedCallbackWrapper & tWrapper = ptSWorld->tExtraWorldCreatedCallbacks.GetHead();
            tWrapper.pfCreatedCallback(ptSWorld->id, tWrapper.idACDPortalOperator, tWrapper.idACDPortal, tWrapper.uEntranceGUID, tWrapper.eWarpReason);
            ptSWorld->tExtraWorldCreatedCallbacks.RemoveHead();
        }


        if (ptSWorld->eCreateStage < SWORLD_CREATE_COMPLETED) return;
    }
    if (tSGame.ptSWorldGlobals->listWorldsToCreate.Count() > 0)
    {
        tSGame.ptSWorldGlobals->listWorldsToCreate.RemoveHead();
    }


    if (!ptSWorld)
        return;
    SNOPreload::RemoveServerWorldLoading(ptSWorld->tWCD.snoWorld, FALSE);
    SNOPreload::AddServerWorld(ptSWorld->tWCD.snoWorld);

    if (ptSWorld->tWCD.snoDungeonFinderSourceWorld != SNO_NONE)
    {
        SNOPreload::RemoveServerWorldLoading(ptSWorld->tWCD.snoDungeonFinderSourceWorld, TRUE);
        SNOPreload::AddServerWorld(ptSWorld->tWCD.snoDungeonFinderSourceWorld);
    }



}










SNO SWorldGetDungeonFinderTileset(SNO snoWorld)
{
    SNO snoDungeonFinderSourceWorld;
    int32 nLevel = WorldGetDungeonFinderLevel(snoWorld);
    if (nLevel > 0)
    {
        SGame & tSGame = SGameManager::GameGet();
        SWorldGlobals *& ptSWorldGlobals = tSGame.ptSWorldGlobals;
        if (!ptSWorldGlobals->bDungeonFinderDataInitialized)
            SWorldRunDungeonFinderEngine();
        return ptSWorldGlobals->tDungeonFinderLevelData[nLevel - 1].snoTilesetWorld;
    }












































































    DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
}





float SWorldGetDungeonFinderSpawnGroupDensityMultiplier(SNO snoWorld)
{
    int32 nLevel = WorldGetDungeonFinderLevel(snoWorld);
    if (nLevel > 0)
    {
        SGame & tSGame = SGameManager::GameGet();
        SWorldGlobals *& ptSWorldGlobals = tSGame.ptSWorldGlobals;
        return ptSWorldGlobals->tDungeonFinderLevelData[nLevel - 1].flSpawnGroupDensityMultiplier;
    }
    return 1.0f;
}







float SWorldGetDungeonFinderEliteSpawnGroupDensityMultiplier(SNO snoWorld)
{
    int32 nLevel = WorldGetDungeonFinderLevel(snoWorld);
    if (nLevel > 0)
    {
        SGame & tSGame = SGameManager::GameGet();
        SWorldGlobals *& ptSWorldGlobals = tSGame.ptSWorldGlobals;
        return ptSWorldGlobals->tDungeonFinderLevelData[nLevel - 1].flEliteSpawnGroupDensityMultiplier;
    }
    return 1.0f;
}






float SWorldGetDungeonFinderGizmoDensityMultiplier(SNO snoWorld)
{
    int32 nLevel = WorldGetDungeonFinderLevel(snoWorld);
    if (nLevel > 0)
    {
        SGame & tSGame = SGameManager::GameGet();
        SWorldGlobals *& ptSWorldGlobals = tSGame.ptSWorldGlobals;
        return ptSWorldGlobals->tDungeonFinderLevelData[nLevel - 1].flGizmoDensityMultiplier;
    }
    return 1.0f;
}







LPCSTR SWorldGetDungeonFinderPopulation(SNO snoWorld)
{
    int32 nLevel = WorldGetDungeonFinderLevel(snoWorld);
    if (nLevel > 0)
    {
        SGame & tSGame = SGameManager::GameGet();
        SWorldGlobals *& ptSWorldGlobals = tSGame.ptSWorldGlobals;
        return ptSWorldGlobals->tDungeonFinderLevelData[nLevel - 1].szPopulationName;
    }
    return NULL;
}







SNO SWorldGetDungeonFinderBossEncounter(
    SNO snoWorld)
{

    int32 nLevel = WorldGetDungeonFinderLevel(snoWorld);
    if (nLevel > 0)
    {
        SGame & tSGame = SGameManager::GameGet();
        SWorldGlobals *& ptSWorldGlobals = tSGame.ptSWorldGlobals;
        return ptSWorldGlobals->tDungeonFinderLevelData[nLevel - 1].snoBossEncounter;
    }
    // no row of its own: the SNO_NONE result shares the caller's fallback (SpawnBoss +0xb4)
    return SNO_NONE;
}




SNO SWorldGetDungeonFinderWeather(SNO snoWorld)
{
    int32 nLevel = WorldGetDungeonFinderLevel(snoWorld);
    if (nLevel > 0)
    {
        SGame & tSGame = SGameManager::GameGet();
        SWorldGlobals *& ptSWorldGlobals = tSGame.ptSWorldGlobals;
        return ptSWorldGlobals->tDungeonFinderLevelData[nLevel - 1].snoWeather;
    }
    return SNO_NONE;
}






uint32 SWorldGetDungeonFinderWorldSeed(SNO snoWorld)
{
    int32 nLevel = WorldGetDungeonFinderLevel(snoWorld);
    if (nLevel > 0)
    {
        SGame & tSGame = SGameManager::GameGet();
        SWorldGlobals *& ptSWorldGlobals = tSGame.ptSWorldGlobals;
        return ptSWorldGlobals->tDungeonFinderLevelData[nLevel - 1].dwWorldSeed;
    }
    return 0;
}















BOOL SWorldAnyLoading()
{
    SGame & tSGame = SGameManager::GameGet();
    return tSGame.ptSWorldGlobals->listWorldsToCreate.Count() > 0;
}





void SWorldAddDungeonFinderLevel(SNO snoWorld, LPCSTR szPopulation, float flSpawnGroupDensityMultiplier, float flEliteSpawnGroupDensityMultiplier, float flGizmoDensityMultiplier, SNO snoWeather)
{
    SGame & tSGame = SGameManager::GameGet();
    SWorldGlobals *& ptSWorldGlobals = tSGame.ptSWorldGlobals;
    if (ptSWorldGlobals->nNumDungeonFinderLevels < 10)
    {
        DungeonFinderLevelInstance & tDungeonFinderLevel = ptSWorldGlobals->tDungeonFinderLevelData[ptSWorldGlobals->nNumDungeonFinderLevels];
        tDungeonFinderLevel.snoTilesetWorld = snoWorld;
        tDungeonFinderLevel.flSpawnGroupDensityMultiplier = flSpawnGroupDensityMultiplier;
        tDungeonFinderLevel.flEliteSpawnGroupDensityMultiplier = flEliteSpawnGroupDensityMultiplier;
        tDungeonFinderLevel.flGizmoDensityMultiplier = flGizmoDensityMultiplier;
        strncpy(tDungeonFinderLevel.szPopulationName, szPopulation, sizeof(tDungeonFinderLevel.szPopulationName) - 1);
        tDungeonFinderLevel.snoWeather = snoWeather;
        ptSWorldGlobals->nNumDungeonFinderLevels++;
    }
}














BOOL SWorldDungeonFinderIsInitialized()
{
    SGame & tSGame = SGameManager::GameGet();
    SWorldGlobals *& ptSWorldGlobals = tSGame.ptSWorldGlobals;
    return ptSWorldGlobals->bDungeonFinderDataInitialized;
}




void SWorldDungeonFinderCreate(SNO snoDungeonFinder)
{
    SGame & tSGame = SGameManager::GameGet();
    DungeonFinderData & tDungeonFinderData = tSGame.ptSGameGlobals->tDungeonFinderData;
    RNG & rng = tDungeonFinderData.tDungeonFinderRNG;
    rng.dwCarry = 0;
    tDungeonFinderData.dwInitialSeed = rng.dwSeed;
    DungeonFinderOverrides * ptOverrides = &SGameGlobalsGet().tChallengeRift.tDungeonFinderOverrides;
    if (ptOverrides->snoDungeonFinderEngine != SNO_NONE)
        snoDungeonFinder = ptOverrides->snoDungeonFinderEngine;
    tDungeonFinderData.snoDungeonFinderEngine = snoDungeonFinder;
    SWorldGlobals *& ptSWorldGlobals = tSGame.ptSWorldGlobals;
    DungeonFinderCreate(snoDungeonFinder, rng, ptSWorldGlobals->tDungeonFinderLevelData, ptSWorldGlobals->nNumDungeonFinderLevels, ptOverrides);
}












void SWorldSetDungeonFinderBossEncounter(SNO snoBossEncounter)
{
    SGame & tSGame = SGameManager::GameGet();
    SWorldGlobals *& ptSWorldGlobals = tSGame.ptSWorldGlobals;
    ptSWorldGlobals->snoDungeonFinderBossEncounter = snoBossEncounter;
}





void SWorldRunDungeonFinderEngine()
{

    SGame & tSGame = SGameManager::GameGet();
    SWorldGlobals *& ptSWorldGlobals = tSGame.ptSWorldGlobals;  // DWARF declares ptSWorldGlobals on 2792 (moved up from the loops)

    tSGame.ptSGameGlobals->tDungeonFinderData.Initialize();


    tSGame.ptSWorldGlobals->bDungeonFinderDataInitialized = TRUE;

    tSGame.ptSGameGlobals->tDungeonFinderData.tDungeonFinderRNG.dwSeed = RandGetDWord(tSGame.ptSGameGlobals->rng);
    tSGame.ptSGameGlobals->tDungeonFinderData.tDungeonFinderRNG.dwCarry = 0;

    if (GameIsWeeklyChallenge())
    {




        Blizzard::Time::Timestamp tStartTime = Blizzard::Time::GetTimestamp();
        Blizzard::Time::TimeRec tTimeRecord;
        Blizzard::Time::BreakTime(tStartTime, tTimeRecord);
        int32 nSeed = (tTimeRecord.dayOfYear + 6 - tTimeRecord.dayOfWeek) / 7 + 1684;

        tSGame.ptSGameGlobals->tDungeonFinderData.tDungeonFinderRNG.dwSeed = nSeed;
    }















    DungeonFinderOverrides & tDungeonFinderOverrides = tSGame.ptSGameGlobals->tChallengeRift.tDungeonFinderOverrides;
    if (tDungeonFinderOverrides.nSeed)
    {
        tSGame.ptSGameGlobals->tDungeonFinderData.tDungeonFinderRNG.dwSeed = tDungeonFinderOverrides.nSeed;
    }

    if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
    {
        tSGame.ptSGameGlobals->tDungeonFinderData.tDungeonFinderRNG.dwSeed = 0x9606b5d2;
    }


    for (int32 i = 0; i < 10; i++)
    {
        ptSWorldGlobals->tDungeonFinderLevelData[i].snoTilesetWorld = SNO_NONE;
    }
    ptSWorldGlobals->nNumDungeonFinderLevels = 0;
    ptSWorldGlobals->snoDungeonFinderBossEncounter = SNO_NONE;
    ptSWorldGlobals->snoDungeonFinderEliteEncounter = SNO_NONE;

    for (int32 i = 0; i < 5; i++)
    {
        ptSWorldGlobals->tDungeonFinderPylonData[i].snoActorPylon = SNO_NONE;
    }
    ptSWorldGlobals->nNumDungeonFinderPylons = 0;

    SNO snoScript = GlobalSNOGet(GLOBALSNO_SCRIPT_DUNGEON_FINDER_ENGINE);

    if (snoScript != SNO_NONE)
    {
        lua_State * L = LuaScriptGetLuaStateWithSNO(snoScript);

        if (LuaScriptGetFunction(L, "LoadDungeonFinder"))
        {
            LuaSNOScriptCall(snoScript, L, 0, 0, "LoadDungeonFinder");
        }
    }


    PlayerList listPlayers(PlayerListGetElementPool());
    PlayerListGetAllInGame(listPlayers, -1, FALSE, -1);
    POS pos = listPlayers.GetHeadPosition();
    while (pos)
    {
        Player * ptPlayer = listPlayers.GetNext(pos);
        SGameSendDungeonFinderSeedToPlayer(*ptPlayer);
        SGameSendDungeonFinderParticipatingPlayerCountToPlayer(*ptPlayer);
        SGameSendDungeonFinderProgressToPlayer(*ptPlayer);
        SGameSendDungeonFinderTimedEventToPlayer(*ptPlayer);
        SGameSendDungeonFinderCompletionTimeToPlayer(*ptPlayer);
    }
}



ACDID SWorldDungeonFinderSpawnBoss(
    WorldPlace tPlaceOrigin)
{

    if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
    {
        return ACDID_INVALID;
    }

    SGameGlobals& tSGameGlobals = SGameGlobalsGet();
    BOOL bLootRunBossDead = (tSGameGlobals.dwFlags2 >> SGAMEGLOBALS2_LOOT_RUN_BOSS_KILLED) & 1;
    BOOL bLootRunBossActive = (tSGameGlobals.dwFlags2 >> SGAMEGLOBALS2_LOOT_RUN_BOSS_ACTIVE) & 1;
    if (bLootRunBossDead || bLootRunBossActive)
    {
        return ACDID_INVALID;
    }


    SceneCommonData * ptSCD = NULL;
    SceneCommonDataIterator tIterator;

    SNO snoAdventure = GlobalSNOGet(GLOBALSNO_ADVENTURE_DUNGEON_FINDER_BOSS_LOCATION);
    SNO snoEncounter = GlobalSNOGet(GLOBALSNO_ENCOUNTER_DUNGEON_FINDER_BOSS);

    SNO snoEncounterOverride = SWorldGetDungeonFinderBossEncounter(WorldCommonGetWorldSNO(tPlaceOrigin.idSWorld));
    if (snoEncounterOverride != SNO_NONE)
    {
        snoEncounter = snoEncounterOverride;
    }

    Team eTeam;
    SNOName tSNONameActor;


    RNG encounterRNG;
    RandSetSeed_Internal(encounterRNG, tSGameGlobals.tDungeonFinderData.tDungeonFinderRNG.dwSeed);
    EncounterResolveToActorSpawnInfo(snoEncounter, encounterRNG, tSNONameActor, eTeam);

    if (tSGameGlobals.tChallengeRift.tDungeonFinderOverrides.snoDungeonFinderBossOverride != SNO_NONE)
    {
        tSNONameActor.snoHandle = tSGameGlobals.tChallengeRift.tDungeonFinderOverrides.snoDungeonFinderBossOverride;
    }


    WorldDistance wdMinDistanceSquared = 100000.0f;  // literal: 0x7100e3c104 holds 100000.0f, no named constant found
    WorldPlace tPlaceClosest;
    tPlaceClosest.wp = cgwpNull;
    tPlaceClosest.idSWorld = tPlaceOrigin.idSWorld;
    SSceneID idSScene = SSCENEID_INVALID;

    while (SceneCommonDataArrayIterate(tIterator, ptSCD, SkipHolodeckScenes))
    {
        if (ptSCD->idSWorld != tPlaceOrigin.idSWorld)  // `continue` form: 2943's while has column 9, the same indent as this if
        {
            continue;
        }

        POS pos = ptSCD->listGizmoLocPlacements.GetHeadPosition();
        while (pos)
        {
            GizmoLocPlacement & tPlacement = ptSCD->listGizmoLocPlacements.GetNext(pos);
            if (tPlacement.snoname.snoHandle == snoAdventure)
            {

                PRTransform transform = ptSCD->transformWorld * tPlacement.tPlace.transform;
                WorldDistance wdDistanceSquared = Vector3D::Distance2DSquared(tPlaceOrigin.wp, transform.wp);
                if (wdDistanceSquared < wdMinDistanceSquared)
                {
                    wdMinDistanceSquared = wdDistanceSquared;
                    tPlaceClosest.wp = transform.wp;
                    idSScene = ptSCD->idSScene;
                }
            }
        }
    }


    ptSCD = SceneCommonDataTryToGet(idSScene);

    if (!ptSCD)
    {




        BOOL bFoundPlayerInRift = FALSE;
        PlayerList listPlayersInGame(PlayerListGetElementPool());
        PlayerListGetAllInGame(listPlayersInGame, -1, FALSE, -1);
        POS pos = listPlayersInGame.GetHeadPosition();
        while (pos)
        {
            Player * ptPlayer = listPlayersInGame.GetNext(pos);
            if (ptPlayer)
            {
                ActorCommonData* ptACD = ACDTryToGet(ptPlayer->idPlayerACD);

                if (ptACD &&
                    ptACD->GetPlace().idSWorld == tPlaceOrigin.idSWorld)
                {
                    tPlaceClosest = ptACD->GetPlace();
                    bFoundPlayerInRift = TRUE;
                    break;
                }
            }
        }

        // no row: the flag test is jump-threaded away (found path skips +0x35c)
        if (!bFoundPlayerInRift)
        {
            tPlaceClosest = tPlaceOrigin;
        }
    }
    else
    {
        if (!(ptSCD->dwSceneFlags & (1 << SCENE_ACTIVATED)))
        {
            SceneCommonDataActivate(ptSCD->idSScene);
        }
    }

    if (tPlaceClosest.wp != cgwpNull)
    {

        ActorAddStruct tActorAdd;
        tActorAdd.tSNOName = tSNONameActor;
        tActorAdd.tPlace = tPlaceClosest;
        tActorAdd.snoActor = tSNONameActor.snoHandle;
        tActorAdd.eMonsterRarity = MR_BOSS;
        tActorAdd.eTeam = eTeam;
        tActorAdd.tMarkersName = MarkerStringHash("DungeonFinderBoss");
        // 3016 has the load and 3019 the or/store of 0x800d0 (bits 4, 6, 7, 19): four |= lines inferred
        tActorAdd.dwActorAddFlags |= (1 << ACTORADD_EVEN_WHEN_STUCK);
        tActorAdd.dwActorAddFlags |= (1 << ACTORADD_DONT_PERTURB);
        tActorAdd.dwActorAddFlags |= (1 << ACTORADD_DONT_SEARCH_FOR_LOCATION);
        tActorAdd.dwActorAddFlags |= (1 << ACTORADD_INVULNERABLE);

        GBHandleList listGroupAffixes(GBGetHandlePool());
        EncounterAddStruct tEncounterAdd;
        EncounterAddInitialize(tEncounterAdd, listGroupAffixes);
        ActorID idActor = GBAddWithEncounterAddStruct(tActorAdd, tEncounterAdd);
        if (idActor != ACTORID_INVALID)
        {






            PlayerList listPlayersInGame(PlayerListGetElementPool());
            PlayerListGetAllInGame(listPlayersInGame, -1, FALSE, -1);
            POS pos = listPlayersInGame.GetHeadPosition();
            while (pos)
            {
                Player * ptPlayer = listPlayersInGame.GetNext(pos);
                if (ptPlayer)
                {
                    ActorCommonData* ptACD = ACDTryToGet(ptPlayer->idPlayerACD);
                    if (ptACD && ACDIsParticipatingInTieredLootRun(*ptACD))
                    {
                        SPlayerTrickleAdd(*ptPlayer, ACDFindByActor(idActor));
                    }
                }
            }

            SGameGlobals& tSGameGlobals = SGameGlobalsGet();
            tSGameGlobals.tDungeonFinderData.nBossSpawnFloor = WorldGetDungeonFinderLevel(WorldCommonGetWorldSNO(tPlaceClosest.idSWorld));


            return ACDFindByActor(idActor);
        }
        return ACDID_INVALID; }  // inferred: the original sets -1 before the shared cleanup (+0x758), so this exit is a return inside the scope
    // no row of its own: the ACDID_INVALID result is set in the shared tail (+0x728)
    return ACDID_INVALID;

}



uint32 SWorldGetNumDungeonFinderLevels()
{
    SGame & tSGame = SGameManager::GameGet();
    return tSGame.ptSWorldGlobals->nNumDungeonFinderLevels;
}
























BOOL SWorldDungeonFinderIsEmpty()
{

    for (int32 i = 1; i <= MAX_DUNGEON_FINDER_LEVELS; i++)
    {
        SNO snoWorld = WorldGetDungeonFinderWorldFromLevel(i);
        if (snoWorld != SNO_NONE)
        {
            SWorldID idSWorld = WorldCommonFindBySNO(snoWorld, TRUE);
            if (idSWorld != SWORLDID_INVALID)
            {
                for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
                {
                    const ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
                    if (ptACDPlayer)
                    {
                        if (idSWorld == ptACDPlayer->GetSWorldID())
                        {
                            return FALSE;
                        }
                    }
                }
            }
        }
    }

    SGame & tSGame = SGameManager::GameGet();
    SWorldGlobals *& ptSWorldGlobals = tSGame.ptSWorldGlobals;

    if (ptSWorldGlobals->snoDungeonFinderBossEncounter != SNO_NONE)
    {
        ScopedBossEncounterDefinitionLockR pdefEncounter(ptSWorldGlobals->snoDungeonFinderBossEncounter);
        for (int32 i = 0; i < NUM_BOSS_WORLDS; i++)
        {
            SNO snoWorld = pdefEncounter->arrWorlds[i];
            if (snoWorld != SNO_NONE)
            {
                SWorldID idSWorld = WorldCommonFindBySNO(snoWorld, TRUE);
                if (idSWorld != SWORLDID_INVALID)
                {
                    for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
                    {
                        const ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
                        if (ptACDPlayer)
                        {
                            if (idSWorld == ptACDPlayer->GetSWorldID())
                            {
                                return FALSE;
                            }
                        }
                    }
                }
            }
        }
    }

    return TRUE;
}



void SWorldResetDungeonFinder()
{

    SNO snoWorlds[MAX_DUNGEON_FINDER_LEVELS + NUM_BOSS_WORLDS];

    for (int32 i = 0; i < MAX_DUNGEON_FINDER_LEVELS + NUM_BOSS_WORLDS; i++)
    {
        snoWorlds[i] = SNO_NONE;
    }

    int32 nNumWorlds = 0;

    for (int32 i = 1; i <= MAX_DUNGEON_FINDER_LEVELS; i++)
    {
        SNO snoWorld = WorldGetDungeonFinderWorldFromLevel(i);
        if (snoWorld != SNO_NONE)
        {
            snoWorlds[nNumWorlds] = snoWorld;
            nNumWorlds++;
        }
    }

    SGame & tSGame = SGameManager::GameGet();
    tSGame.ptSGameGlobals->tDungeonFinderData.flDungeonFinderProgress = 0.0f;
    tSGame.ptSGameGlobals->tDungeonFinderData.tDungeonFinderRNG.dwSeed = 0;
    tSGame.ptSGameGlobals->tDungeonFinderData.tDungeonFinderRNG.dwCarry = 0;

    SWorldGlobals *& ptSWorldGlobals = tSGame.ptSWorldGlobals;

    if (ptSWorldGlobals->snoDungeonFinderBossEncounter != SNO_NONE)
    {
        ScopedBossEncounterDefinitionLockR pdefEncounter(ptSWorldGlobals->snoDungeonFinderBossEncounter);

        if (BossEncounterManager::IsAnyEncounterActive() && BossEncounterManager::GetActiveEncounterSNO() == ptSWorldGlobals->snoDungeonFinderBossEncounter)
        {
            // an assert-style fatal error (DisplayInternalError + ExitProgram on one line)
            DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
            return;
        }

        for (int32 i = 0; i < NUM_BOSS_WORLDS; i++)
        {
            SNO snoWorld = pdefEncounter->arrWorlds[i];
            if (snoWorld != SNO_NONE)
            {
                snoWorlds[nNumWorlds] = snoWorld;
                nNumWorlds++;
            }
        }
    }

    for (int32 i = 0; i < nNumWorlds; i++)
    {
        SNO snoWorld = snoWorlds[i];
        SWorldRemovePlayersAndFlagForDeletion(WorldCommonFindBySNO(snoWorld, TRUE));
    }



    SNO snoPower = GlobalSNOGet(GLOBALSNO_POWERS_DUNGEONFINDER_BUFF);
    for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {
        ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
        if (ptACDPlayer)
        {
            PowerAPI_BuffFlagAllFromPowerForDeletion(*ptACDPlayer, snoPower, -1, BUFF_REMOVAL_REASON_DEFAULT, ACDID_INVALID);


            ptACDPlayer->ApplyAllPassives(FALSE);
            ptACDPlayer->AttributesSetInt(FastAttribKey(ATTRIBUTE_IN_CHALLENGE_RIFT), 0);
        }

        SGameSendDungeonFinderSeedToPlayer(*ptPlayer);
        SGameSendDungeonFinderProgressToPlayer(*ptPlayer);
        SGameSendDungeonFinderTimedEventToPlayer(*ptPlayer);
    }


    if (ptSWorldGlobals->snoDungeonFinderBossEncounter != SNO_NONE)
    {
        POS pos = tSGame.ptSGameGlobals->listCompletedBossEncounters.Find(ptSWorldGlobals->snoDungeonFinderBossEncounter);
        if (pos)
        {
            tSGame.ptSGameGlobals->listCompletedBossEncounters.RemoveAt(pos);
        }

        ptSWorldGlobals->snoDungeonFinderBossEncounter = SNO_NONE;
    }

    ptSWorldGlobals->bDungeonFinderDataInitialized = FALSE;
    NephalemRiftManager::Cleanup();

    for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {
        SGameSendDungeonFinderParticipatingPlayerCountToPlayer(*ptPlayer);
    }
}





void SWorldResetSetDungeon()
{
    if (SetEncounterManager::IsAnyEncounterActive())
    {
        SWorldID idWorld = SetEncounterManager::GetActiveSetDungeonWorld();
        SWorldRemovePlayersAndFlagForDeletion(idWorld);
        SetEncounterManager::Cleanup();
    }
}









void sDeleteGreedRealm(TimerID, DataID idOwner, void *)
{
    SWorldRemovePlayersAndFlagForDeletion(WorldCommonFindBySNO(GlobalSNOGet(GLOBALSNO_WORLD_GREED_REALM), TRUE));
    SWorldRemovePlayersAndFlagForDeletion(WorldCommonFindBySNO(GlobalSNOGet(GLOBALSNO_WORLD_GREED_REALM_BOSS), TRUE));
    SWorldRemovePlayersAndFlagForDeletion(WorldCommonFindBySNO(GlobalSNOGet(GLOBALSNO_UNK_686), TRUE));
    SWorldRemovePlayersAndFlagForDeletion(WorldCommonFindBySNO(GlobalSNOGet(GLOBALSNO_UNK_687), TRUE));

    ACDID idACDPortal = (ACDID)idOwner;
    ActorFlagForDeletion((ActorID)ACDGetOwner(idACDPortal));

    SGameGlobals & tSGameGlobals = SGameGlobalsGet();
    POS pos = tSGameGlobals.listCompletedBossEncounters.Find(GlobalSNOGet(GLOBALSNO_BOSS_ENCOUNTER_GREED));
    if (pos)
        tSGameGlobals.listCompletedBossEncounters.RemoveAt(pos);
    pos = tSGameGlobals.listCompletedBossEncounters.Find(GlobalSNOGet(GLOBALSNO_UNK_2009));
    if (pos)
        tSGameGlobals.listCompletedBossEncounters.RemoveAt(pos);
}








void SWorldResetGreedRealm(float flDelay)
{
    SGameGlobals & tSGameGlobals = SGameGlobalsGet();
    int32 dwDelay = (int32)flDelay * TICKS_PER_SECOND;
    TimerRegister(tSGameGlobals.idGreedPortal, TIMER_OWNER_ACD, dwDelay, NULL, sDeleteGreedRealm);
}




BOOL SWorldIsDungeonFinder(SWorldID idSWorld)
{
    SWorld * ptSWorld = SWorldTryToGet(idSWorld);
    if (ptSWorld)
    {
        if (WorldIsDungeonFinderPlaceHolder(WorldCommonGetWorldSNO(idSWorld)))
            return TRUE;
    }
    return FALSE;
}







BOOL SWorldIsTieredDungeonFinder(SWorldID idSWorld)
{
    SWorld * ptSWorld = SWorldTryToGet(idSWorld);
    if (ptSWorld)
    {
        if (SWorldIsDungeonFinder(idSWorld) && NephalemRiftManager::IsTieredRiftActive())
            return TRUE;
    }
    return FALSE;
}







BOOL SWorldIsSetDungeon(SWorldID idSWorld)
{
    SWorld * ptSWorld = SWorldTryToGet(idSWorld);
    if (ptSWorld && ptSWorld->hWorldQuestDescriptionLabel != (StringLabelHandle)-1)
        return TRUE;
    return FALSE;
}












int32 SWorldGetTieredDungeonFinderLevel(SWorldID idSWorld)
{
    SWorld * ptSWorld = SWorldTryToGet(idSWorld);
    if (ptSWorld)
    {
        if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT) || WorldIsDungeonFinderPlaceHolder(WorldCommonGetWorldSNO(idSWorld)))
            return NephalemRiftManager::GetActiveRiftLevel();
        return -1;
    }
    return -1;
}







void SWorldRecordPylonSpawnForRift(ActorCommonData & tACDPylon)
{
    SWorld * ptSWorld = SWorldTryToGet(tACDPylon.GetSWorldID());
    if (ptSWorld)
    {
        int32 nActiveFloor = WorldGetDungeonFinderLevel(ptSWorld->tWCD.snoWorld);
        if (nActiveFloor > 0)
        {
            SGame & tSGame = SGameManager::GameGet();
            SWorldGlobals & tSWorldGlobals = *tSGame.ptSWorldGlobals;
            if (tSWorldGlobals.nNumDungeonFinderPylons < 5)
            {
                DungeonFinderPylonInstance & tPylon = tSWorldGlobals.tDungeonFinderPylonData[tSWorldGlobals.nNumDungeonFinderPylons++];
                tPylon.snoActorPylon = tACDPylon.snoActor;
                tPylon.wpPos = tACDPylon.GetWorldLocation().transform.wp.As2D();
                tPylon.nRiftFloor = nActiveFloor;
            }
        }
    }
}






void SWorldSpawnPylonOverridesForRift(SWorld * ptSWorld)
{
    DungeonFinderOverrides * ptOverrides = &SGameGlobalsGet().tChallengeRift.tDungeonFinderOverrides;
    if (ptOverrides->bUsePylonOverrides)
    {

        int32 nFloor = WorldGetDungeonFinderLevel(ptSWorld->tWCD.snoWorld);

        for (int32 i = 0; i < 5; i++)
        {
            if (ptOverrides->arPylonOverrides[i].nRiftFloor == nFloor)
            {
                SNO snoPylon = ptOverrides->arPylonOverrides[i].snoActorPylon;
                if (snoPylon != SNO_NONE)
                {
                    ActorAddStruct tActorAdd;
                    tActorAdd.tPlace.idSWorld = ptSWorld->id;
                    tActorAdd.tPlace.wp.As2D() = ptOverrides->arPylonOverrides[i].wpPos;
                    tActorAdd.tPlace.wp.z = 0.0f;


                    if (ScenesCommonNavMeshNodeFromPlace(tActorAdd.tPlace, &tActorAdd.tNavMeshHandle, TRUE, FALSE))
                    {


                        tActorAdd.tPlace.wp.z = NavMeshGetHeightWorldSpace(tActorAdd.tNavMeshHandle);


                        tActorAdd.tSNOName.snoHandle = snoPylon;
                        tActorAdd.tSNOName.snoGroup = ACTOR_SNO;
                        tActorAdd.snoActor = snoPylon;
                        tActorAdd.hGB.gbid = snoPylon;

                        tActorAdd.dwActorAddFlags |= (1 << ACTORADD_DONT_PERTURB);
                        tActorAdd.dwActorAddFlags |= (1 << ACTORADD_DONT_SEARCH_FOR_LOCATION);

                        GBAdd(tActorAdd);

                        DungeonFinderPylonInstance * arPylons = SWorldGlobalsGetDungeonFinderPylonData();
                        arPylons[i] = ptOverrides->arPylonOverrides[i];
                    }
                }
            }
        }
    }
}



DungeonFinderLevelInstance * SWorldGlobalsGetDungeonFinderLevelData(int32 nFloor)
{
    if (nFloor < 0 || nFloor > 10)
        return NULL;
    SGame & tSGame = SGameManager::GameGet();
    SWorldGlobals *& ptSWorldGlobals = tSGame.ptSWorldGlobals;
    return &ptSWorldGlobals->tDungeonFinderLevelData[nFloor];
}








DungeonFinderPylonInstance * SWorldGlobalsGetDungeonFinderPylonData()
{

    SGame & tSGame = SGameManager::GameGet();


    return tSGame.ptSWorldGlobals->tDungeonFinderPylonData;
}




























































































void SWorldLoadRiftShapshot(const SGameID idSGame, uint32 nChallengeNum, const D3::Leaderboard::RiftSnapshot & tSnapshot, int32 nEndTime, bool bChallengeOverriden, float flOrbValueMultiplier)
{






    SGame * ptSGame = SGameManager::GameTryToGet(idSGame);
    if (!ptSGame || !ptSGame->ptSGameGlobals)
        return;



    SGameGlobals & tSGameGlobals = *ptSGame->ptSGameGlobals;

    tSGameGlobals.tChallengeRift.nChallengeNumber = nChallengeNum;
    tSGameGlobals.tChallengeRift.bChallengeOverriden = bChallengeOverriden;
    tSGameGlobals.tChallengeRift.nEndTime = nEndTime;

    DungeonFinderOverrides * ptOverrides = &tSGameGlobals.tChallengeRift.tDungeonFinderOverrides;
    DungeonFinderOverridesInit(*ptOverrides);

    ptOverrides->snoDungeonFinderEngine = tSnapshot.sno_dungeon_finder();
    ptOverrides->nSeed = tSnapshot.rift_seed();
    ptOverrides->snoDungeonFinderBossOverride = tSnapshot.sno_boss();
    ptOverrides->nWeeklyChallengeRiftLevel = tSnapshot.rift_tier();
    ptOverrides->flOrbValueMultiplier = flOrbValueMultiplier;
    if (tSnapshot.has_completion_milliseconds() && tSnapshot.completion_milliseconds() != 0)
    {
        ptOverrides->nWeeklyChallengeCompletionMilliseconds = tSnapshot.completion_milliseconds();
    }
    else
    {

        ptOverrides->nWeeklyChallengeCompletionMilliseconds = tSnapshot.deprecated_completion_seconds() * 1000;
    }

    ptOverrides->bUsePylonOverrides = TRUE;

    for (int32 i = 0; i < 10 && i < tSnapshot.floors_size(); i++)
    {
        const D3::Leaderboard::RiftFloor & tFloor = tSnapshot.floors(i);
        ptOverrides->arSnoTilesets[i] = tFloor.sno_world();
        ptOverrides->arPopulationHashes[i] = tFloor.population_hash();
    }

    for (int32 i = 0; i < 5 && i < tSnapshot.pylons_size(); i++)
    {
        const D3::Leaderboard::RiftPylon & tPylon = tSnapshot.pylons(i);
        ptOverrides->arPylonOverrides[i].snoActorPylon = tPylon.sno_pylon();
        ptOverrides->arPylonOverrides[i].nRiftFloor = tPylon.rift_floor();
        ptOverrides->arPylonOverrides[i].wpPos.x = tPylon.position_x();
        ptOverrides->arPylonOverrides[i].wpPos.y = tPylon.position_y();
    }
}

void SWorldRemoveAllHirelings(SWorldID idSWorld)
{
    for (const Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {
        ACDID idACDHireling = PlayerGetHireling(ptPlayer->idPlayerACD);
        if (idACDHireling != ACDID_INVALID)
        {
            ActorCommonData & tACDHireling = ACDGet(idACDHireling);
            if (ACDIsHireling(tACDHireling))
            {
                if (tACDHireling.GetPlace().idSWorld == idSWorld)
                {
                    NPCHandleDismissal(tACDHireling, FALSE, TRUE);
                }
                else
                {
                    ACDID idACDHeadStone = GetLastHeadstoneACD(tACDHireling);
                    ActorCommonData * ptACDHeadStone = ACDTryToGet(idACDHeadStone);
                    if (ptACDHeadStone && ptACDHeadStone->GetPlace().idSWorld == idSWorld)
                        DeleteLastHeadstone(tACDHireling);
                }
            }
        }
    }
}











void SWorldSyncData(SWorldID idSWorld, const Player * ptPlayer)
{

    SWorld * ptSWorld = SWorldTryToGet(idSWorld);
    if (ptSWorld)
    {
        WorldSyncedDataMessage tMessage;
        tMessage.tSyncedData = ptSWorld->tWCD.tWorldSyncedData;
        tMessage.idSWorld = idSWorld;
        PlayerList listDestPlayers(PlayerListGetElementPool());
        if (ptPlayer == NULL)
            PlayerListGetAllConnected(listDestPlayers, -1, FALSE);
        else
            listDestPlayers.AddTail_Tracked((Player *)ptPlayer);
        MessageSendToClient(listDestPlayers, SMSG_WORLD_SYNCED_DATA, &tMessage, sizeof(tMessage));
    }
}










int32 SWorldGetMonsterCount(SWorldID idSWorld)
{
    uint32 nCount = 0;

    SWorld * ptSWorld = SWorldTryToGet(idSWorld);
    if (ptSWorld)
    {
        // count active monsters
        ActorIterator tIt;
        while (ActorDataArrayIterate(tIt))
        {
            Actor * ptActor = tIt.ptElement;

            ActorCommonData & tACD = ACDGet(ptActor->idACD);
            if (!tACD.IsInWorld())
                continue;




            const WorldLocation & tLocation = tACD.GetWorldLocation();

            if (tLocation.idSWorld != idSWorld)
                continue;





            if (ActorSNOIsUntargetable(tACD.GetActorSNO()))
                continue;





            if (tACD.AttributesGetInt(ATTRIBUTE_RESURRECTED))
                continue;





            if (tACD.AttributesGetInt(ATTRIBUTE_GRANTS_NO_XP))
                continue;




            if (tACD.IsMonsterTeam() && tACD.IsAlive())
                nCount++;
            if (nCount > BOUNTY_TYPE_CLEAR_DUNGEON_SHOW_REMAINING_THRESHOLD)
                return INT_MAX;
        }


        // count inactive monsters
        SGame & tSGame = SGameManager::GameGet();
        TDataArrayIterator<SScene> tItSSceneForMarkers;
        while (tSGame.ptSScenes->Iterate(tItSSceneForMarkers))
        {
            SScene * ptSScene = tItSSceneForMarkers.ptElement;
            if (ptSScene->tSCD.idSWorld != idSWorld)
                continue;



            if (!(ptSScene->tSCD.dwSceneFlags & 2))
                return INT_MAX;




            if (ptSScene->tSCD.ptMarkerScene->ptInactiveMarkerList)
            {
                TInactiveMarkerList & listMarkers = *ptSScene->tSCD.ptMarkerScene->ptInactiveMarkerList;
                POS pos = listMarkers.GetHeadPosition();
                while (pos)
                {
                    InactiveBaseMarker * ptMarker = listMarkers.GetNext(pos);
                    if (ptMarker->eInactiveMarkerType == INACTIVE_MONSTER)
                    {
                        InactiveMonsterMarker * ptMonsterMarker = (InactiveMonsterMarker *)ptMarker;
                        Team eTeam = (Team)ptMonsterMarker->tSavedAttributes.nTeam;

                        if (ptMonsterMarker->tSavedAttributes.dwSavedFlagAttributes & ((1 << MSA_FLAG_GRANTS_NO_XP) | (1 << MSA_FLAG_RESURRECTED)))
                            continue;









                        if (TeamIsMonsterTeam(eTeam))
                        {
                            nCount++;

                            if (nCount > BOUNTY_TYPE_CLEAR_DUNGEON_SHOW_REMAINING_THRESHOLD)
                                return INT_MAX;
                        }
                    }
                }
            }
        }

        return nCount;
    }
    return 0;
}


void SWorldRegisterEvent(SWorldID idSWorld, SNO snoEvent)
{
    SWorld * ptSWorld = SWorldTryToGet(idSWorld);
    if (ptSWorld)
    {
        ptSWorld->listEventSNOs.AddTail_Tracked(snoEvent);
    }
}





void SWorldRegisterUniqueMonsterActor(SWorldID idSWorld, SNO snoActor, SNO snoLevelArea, int32 nDRLGIndex)
{
    SWorld * ptSWorld = SWorldTryToGet(idSWorld);
    if (ptSWorld)
    {
        UniqueMonsterActorEntry tEntry;
        tEntry.snoMonsterActor = snoActor;
        tEntry.snoLevelArea = snoLevelArea;
        tEntry.nDRLGIndex = nDRLGIndex;
        ptSWorld->listUniqueMonsterActorEntries.AddTail_Tracked(tEntry);
    }
}







// LINKER-REMOVED: DWARF size 0xfc; DWARF lexical block (tSGame 3837, pos 3839) and rows 3835:9 (one test before GameGet) and 16/20/24-column nesting: body inside `if (ptSWorld)`; iterator name tItSSceneForMarkers (3845) from the DWARF.
void SWorldActivateSceneForUniqueMonsterActor(
    SWorld * ptSWorld)
{
    if (ptSWorld)
    {
        SGame & tSGame = SGameManager::GameGet();

        POS pos = ptSWorld->listUniqueMonsterActorEntries.GetHeadPosition();
        while (pos)
        {
            const UniqueMonsterActorEntry & tEntry = ptSWorld->listUniqueMonsterActorEntries.GetNext(pos);


            TDataArrayIterator<SScene> tItSSceneForMarkers;
            while (tSGame.ptSScenes->Iterate(tItSSceneForMarkers))
            {
                SScene * ptSScene = tItSSceneForMarkers.ptElement;
                if (ptSScene->tSCD.idSWorld == ptSWorld->id &&
                    ptSScene->tSCD.tSceneSpec.nDRLGIndex == tEntry.nDRLGIndex)
                {
                    if (!(ptSScene->tSCD.dwSceneFlags & 2))
                    {
                        SceneCommonDataActivate(ptSScene->id);
                    }
                    break;
                }
            }
        }
    }
}




static int sCompareDRLGIndices(const void * pData0, const void * pData1)
{
    const SceneCommonData * ptScene0 = (const SceneCommonData *)pData0;
    const SceneCommonData * ptScene1 = (const SceneCommonData *)pData1;
    return ptScene0->tSceneSpec.nDRLGIndex < ptScene1->tSceneSpec.nDRLGIndex;
}




void SWorldActivateScenesForRift(SWorld * ptSWorld)
{

    if (ptSWorld && WorldIsDungeonFinderPlaceHolder(ptSWorld->tWCD.snoWorld))
    {

        blz::list<SceneCommonData *> listScenes;
        SceneCommonData * ptSCD = NULL;
        SceneCommonDataIterator tIterator;
        while (SceneCommonDataArrayIterate(tIterator, ptSCD, SkipHolodeckScenes))
        {
            if (ptSCD->idSWorld == ptSWorld->id)
            {




                listScenes.push_back(ptSCD);
            }
        }


        listScenes.sort(sCompareDRLGIndices);




        for (blz::list<SceneCommonData *>::iterator iter = listScenes.begin(); iter != listScenes.end(); ++iter)
        {









            SceneSpawnBitMaskInitialize(**iter, PASSABILITY_NEW_ENCOUNTER, TRUE);
        }





        for (blz::list<SceneCommonData *>::iterator iter = listScenes.begin(); iter != listScenes.end(); ++iter)
        {


            SceneCommonDataActivate((*iter)->idSScene);
        }







        for (blz::list<SceneCommonData *>::iterator iter = listScenes.begin(); iter != listScenes.end(); ++iter)
        {
            if (ptSWorld->listSpawnGroupFailed.IsEmpty())
                break;

            MarkersInstantiateFailedDistribution(**iter, TRUE);
        }






        for (blz::list<SceneCommonData *>::iterator iter = listScenes.begin(); iter != listScenes.end(); ++iter)
        {

            if (!ptSWorld->listSpawnGroupFailed.IsEmpty())
            {
                MarkersInstantiateFailedDistribution(**iter, FALSE);
            }



            SceneSpawnBitMaskDispose(**iter);
        }
    }
}

BOOL SWorldHasBeenEntered(SNO snoWorld)
{
    SWorldID idSWorld = SWorldFindBySNO(snoWorld);
    if (idSWorld == SWORLDID_INVALID)
        return FALSE;
    SWorld * ptSWorld = SWorldTryToGet(idSWorld);
    if (ptSWorld)
        return ptSWorld->dwFlags & (1 << FLAG_SWORLD_HAS_BEEN_ENTERED);
    return FALSE;
}












BOOL SWorldPlayerAllowed(SWorldID idSWorld, const Player & tPlayer)
{
    SWorld * ptSWorld = SWorldTryToGet(idSWorld);
    if (!ptSWorld)
        return FALSE;

    const uint64 idPlayer = tPlayer.tAccount.tHandle.m_id;
    for (int32 i = 0; i < 4; i++)
    {
        uint64 idAllowedPlayer = ptSWorld->arAllowedPlayers[i];
        if (!idAllowedPlayer)
            return i == 0;
        if (idAllowedPlayer == idPlayer)
            return TRUE;
    }
    return FALSE;
}















void SWorldLimitToPlayersInGame(SWorld & tSWorld)
{
    int32 nNumPlayers = 0;
    for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {
        if (nNumPlayers < 4)
            tSWorld.arAllowedPlayers[nNumPlayers] = ptPlayer->tAccount.tHandle.m_id;
        nNumPlayers++;
    }
    for (int32 i = nNumPlayers; i < 4; i++)
        tSWorld.arAllowedPlayers[i] = 0;
    if (nNumPlayers <= 0)
        tSWorld.arAllowedPlayers[0] = (uint64)-1;
}
















void SWorldRemovePlayersAndFlagForDeletion(SWorldID idSWorld)
{
    if (idSWorld == SWORLDID_INVALID)
        return;

    const SWorld * ptSWorld = SWorldTryToGet(idSWorld);
    if (!ptSWorld)
        return;

    for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {
        SPlayerClearAllReturnPoints(*ptPlayer, ptSWorld->tWCD.snoWorld);
        if (ptPlayer->idPlayerACD != ACDID_INVALID)
        {
            const ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
            if (ptACDPlayer && ptACDPlayer->GetSWorldID() == idSWorld)
                SPortalMovePlayerToActSpawnPoint(*ptPlayer);
        }
    }

    PowersForceStopForWorld(idSWorld);
    SWorldFlagForDeletion(idSWorld, 1, FALSE);
    LevelAreaWorldFlagForDeletion(idSWorld, GameGetServerTime() + 1);
    SWorldRemoveAllHirelings(idSWorld);
    Minimap::RemoveAllForWorld(idSWorld);


    for (Player * ptPlayer = PlayerGetFirstAll(); ptPlayer; ptPlayer = PlayerGetNextAll(ptPlayer))
    {
        if (ptPlayer->idGameConnection != GAMECONNECTIONID_INVALID)
        {
            ACDGet(ptPlayer->idPlayerACD).RestorePermanentPowers();
        }
    }
}
