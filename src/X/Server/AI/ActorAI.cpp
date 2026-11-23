// Original file: X/Server/AI/ActorAI.cpp
// Assembled by build/try/r3-actorai/asm.py: functions sit on their original lines (asserts embed __LINE__).

#include "ActorAI.h"
#include "AIStates.h"
#include "BrainStance.h"
#include "ProximityDetectorBrain.h"
#include "../Actors/Actors.h"
#include "../SGame.h"
#include "../SGameManager.h"
#include "../../Common/SNO/SNOName.h"
#include "../../Common/WorldPlace.h"
#include "../../Common/ActorCommonData.h"
#include "../../Common/GameCommonData.h"
#include "../../Common/Game.h"
#include "../../Common/Target.h"
#include "../../Common/InactiveMarker.h"
#include "../../Common/Monsters.h"
#include "../../Common/AnimPlayer.h"
#include "../../Common/Attributes/FastAttribute.h"
#include "../../Common/Definitions/GameBalanceDefinition.h"
#include "../../Common/Definitions/ActorDefinition.h"
#include "../../Common/GlobalSNO.h"
#include "../Powers/PowerAPI.h"
#include "../../Common/Powers/PowersCommon.h"
#include "../../Common/Definitions/PowerDefinition.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/Random.h"
#include "../../Sigma/Math/MathGeneral.h"
#include "../../Common/GameConstants.h"
#include "../../Sigma/DataStructures/XVar.h"
#include "../../Sigma/DataStructures/XPooledList.h"
#include "../../Sigma/DataStructures/XChainedFixedBlockAllocator.h"
#include "../../Sigma/DataStructures/ChainedDataArrayTemplated.h"
#include "../../Sigma/DataStructures/DataArraysTemplated.h"
#include "../../../Contrib/Contrib/blz/include/blz/vector.h"
#include "../../Sigma/BlzExt/BlzVectorHelper.h"
#include "../../../BlizzardCore/Include/Blizzard/String.h"
#include "../LuaScript/ScriptEvent.h"
#include "../../Common/Marker/Marker.h"
#include <stdarg.h>

// Declared here until their own headers are adopted.
void BrainRunUpdates(Actor & tActor);  // X/Server/AI/BrainComponents.h (BrainComponents.cpp)
void GameDebugSetCurrentUpdatingActor(ACDID idACD);  // X/Common/GameDebug.h
void FastAttribGroupApplyDirty(AttribGroupID idAttribGroup);  // X/Common/Attributes/FastAttribute.h
void AnimPlayerUpdate(ActorCommonData & tACD);  // X/Common/AnimPlayer.h
ACDID GetLastHeadstoneACD(const ActorCommonData & tACD);  // X/Server/AI/BrainComponents.h
void BrainUpdate(Actor & tActor);  // ActorAI.cpp:1486
void AIQueueCompleteCurrentAction(ActorID idActor);  // ActorAI.cpp:2653
AIQueuedActionID AIQueueAddAction(ActorID idActor, SNO snoPower, const Target & tTarget, int32 uData, float flData, LPCSTR szData, AIQueuedActionType eType);  // ActorAI.cpp:2209
void BrainStanceReset(ActorID idActor);  // X/Server/AI/BrainStance.h
BOOL NodePathIsOnPath(PathID idPath, PathNodeID idPathNode);  // X/Common/NodePath.h
#include "../../Common/NodePath.h"

#include "SpawnerBrain.h"
#include "../Map/Minimap.h"
#include "../../Common/TagMaps/MarkerTags.h"
#include "../../Common/Gizmo.h"
#include "../../Sigma/TypeDescriptor/SymbolicValue.h"
NodePathNode * NodePathGetNode(PathID idPath, PathNodeID idPathNode);  // X/Common/NodePath.cpp:629
PathNodeID NodePathGetNextNodeAndPosition(PathID idPath, const Vector3D & wpPos, const NodePathNode * ptPathNodeDest, Vector3D & wpPosClosest);  // X/Common/NodePath.cpp:665
PathNodeID NodePathGetNextClosestNode(PathID idPath, PathNodeID idCurrNode, PathNodeID idDestNode);  // X/Common/NodePath.cpp:733
PathNodeID NodePathGetPrevNode(PathID idPath, PathNodeID idCurrNode, PathNodeID idDestNode);  // X/Common/NodePath.cpp:773
BOOL TetherBrainUpdate(ActorID idActor);  // X/Server/AI/TetherBrain.h
void AIStopTetherRetreat(ActorID idActor, ActorAI & tAI);  // ActorAI.cpp:3923
uint32 PowerFailureReasons(const ActorCommonData & tACD, AutoPowerDefinitionR pdefPower, const Target * ptTarget, BOOL bIsAI, BOOL bFreeCast);  // X/Server/Powers/Powers.h
extern const SymbolicEntry gActorBrainNames[]; // DWARF X/Common/SNO/SymbolicValue.cpp:122; declaration placement inferred.
void ActorAIDispose(const ActorID idActor); // This file:1050.
void ActorAIRestoreForActivatingActor(ActorID idActor, ActorSavedAI & tActorSavedAI); // This file:4330.

BOOL GBGetMovementStyleDef(const GBID gbidMovementStyle, MovementStyleDefinition & tMovementStyleDef); // DWARF GameBalanceDefinition.cpp:4223; header ownership inferred GameBalanceDefinition.h.
void AIStateSetOverridePower(ActorID idActor, SNO snoPower, ACDID idACDTarget); // DWARF AIStates.cpp:6086; header ownership inferred AIStates.h.

void BrainTranslateInitForGame(); // DWARF BrainTranslate.cpp:19.
void BrainStanceInitForGame(); // DWARF BrainStance.cpp:111.
void BrainComponentInitForGame(); // DWARF BrainComponents.cpp:341.
#include "Lure.h"
#include "../../Sigma/MemoryPools.h"
void BrainComponentDisposeForGame(); // DWARF BrainComponents.cpp:365.
void BrainStanceDisposeForGame(); // DWARF BrainStance.cpp:124.
void BrainTranslateDisposeForGame(); // DWARF BrainTranslate.cpp:78.
#include "../Actors/ActorPowers.h"
#include "../Movement/SActorMovement.h"
void ActorPowersInterruptPrimary(ActorID idActor, BOOL bIgnoreContactFrame); // DWARF ActorPowers.cpp:392; inferred ActorPowers.h ownership.
#include "../SPlayer.h"
#include "../../Common/Nav/NavigationMesh.h"
#include "../../Sigma/ErrorManager/ErrorManager.h"
#include "BrainComponents.h"
BOOL ACDFindSpawnLocation(ACDID idACDSearchFrom, const PassabilityType ePassabilityType, WorldPlace & placeResult); // DWARF Actors.cpp:3572; inferred Actors.h ownership.
BOOL SPlayerFindGoodSpawnLocation(const WorldPlace & placeStart, WorldPlace & placeResult, float flSearchRadius); // DWARF SPlayer.cpp:2289; inferred SPlayer.h ownership.
void ActorChangeWorld(const ActorID idActor, const WorldPlace & tPlace, const Quaternion & q); // DWARF Actors.cpp:6093; inferred Actors.h ownership.
#include "../../Common/Player.h"
BOOL AIStateSetStateData(ActorID idActor, AIStateType eStateType, SNO snoAIState); // DWARF AIStates.cpp:5560; inferred AIStates.h ownership.
#include "../ConversationMgr.h"
void AISetCalloutCooldown(ActorCommonData & tACD, SNO snoConversation, float flCooldown, float flGlobalCooldown); // This file:2873.
#include "../SNPC.h"
#include "../../Common/TagMaps/ActorTags.h"
#include "../Actors/ActorEffect.h"
void NPCHandleDismissal(ActorCommonData & tACDHireling, BOOL bPlayerDismissed, BOOL bDisplayChatMessage); // DWARF SNPC.cpp:747; header placement inferred.
void NPCHelperDetach(ActorCommonData & tACDPlayer, ActorCommonData & tACDHelper); // DWARF SNPC.cpp:886; header placement inferred.
BOOL EscortCanBePulledThroughWarp(const ActorCommonData & tACDEscort, const ActorCommonData & tACDPlayer); // DWARF Player.cpp:3256; header placement inferred.
void BrainAPIDoResurrect(ActorID idActor, BOOL bForce); // DWARF BrainComponents.cpp:7464; header placement inferred.
#include "../../Common/TagMaps/Tags.h"
#include "../../Common/Actors/ActorEffectCommon.h"
void BrainAPIDoLookAt(ActorID idActor, const Target & tTarget, BOOL bTrack); // DWARF BrainComponents.cpp:3856; header placement inferred.
void BrainAPIDoScriptedDeath(ActorCommonData & tACD, DeathType eDeathType, BOOL bCrit, BOOL bDropLoot, ActorID idKiller); // DWARF BrainComponents.cpp:11213; header placement inferred.
void AIQueueClearList(ActorID idActor); // This file:2603.
Angle ACDGetYaw(ACDID idACD); // DWARF ActorCommonData.cpp:2997; header placement inferred.
enum {
    MAX_ACTOR_AI_EVENT_HANDLERS = 7,
    AI_UPDATE_RATE = 6,
};

static const int32 INITIAL_PROXIMITY_DETECTOR_DATA_COUNT = 128;






static const int32 MAX_SIMULATEOUS_QUEUED_POWERS = 1024;
static const int32 MAX_SIMULATEOUS_QUEUED_CALLOUTS = 64;
void ActorAISetMovementStyle(ActorID idActor, AIStateType eStateType, GBID gbidMovementStyle); // This file:2118; declaration placement inferred.
static XVarScope s_scopeActorAI("ActorAI", 0);
XVarUint32 s_varBrainUpdateAdditionalTicks(s_scopeActorAI, "BrainUpdateAdditionalTicks", 0);
XVarUint32 s_varWarnNearbyAlliesAboutTargetCooldownTicks(s_scopeActorAI, "WarnNearbyAlliesCooldownTicks", 60);
XVarUint32 s_varBrainWasAttackedAddTargetCooldownTicks(s_scopeActorAI, "BrainWasAttackedAddTargetCooldownTicks", 20);

typedef int32 AIQueuedActionID;

struct AIQueuedPower {  // size 0x58
    SNO snoPower;  // 0x00
    Target tTarget;  // 0x04
    uint32 uAnimTag;  // 0x20
    uint32 uNewIdleAnimTag;  // 0x24
    uint32 uTicks;  // 0x28
    int32 uData;  // 0x2c
    float flData;  // 0x30
    char strData[32];  // 0x34
    AIQueuedActionType eType;  // 0x54
};

typedef blz::vector<AIQueuedPower, blz::allocator<AIQueuedPower> > AIQueuedPowerArray;

struct AIQueuedCallout {  // size 0x20
    ACDID idACDPlayer;  // 0x00
    SNO snoConversation;  // 0x04
    int32 nPriority;  // 0x08
    uint32 dwDelayedTime;  // 0x0c
    uint32 dwExpireTime;  // 0x10
    float flCooldown;  // 0x14
    float flGlobalCooldown;  // 0x18
    uint32 dwFlags;  // 0x1c
};

typedef blz::vector<AIQueuedCallout, blz::allocator<AIQueuedCallout> > AIQueuedCalloutArray;

#include "../../Common/Definitions/MonsterDefinition.h"
void ChampionBrainActionUpdate(ActorID idActor); // DWARF ChampionBrains.cpp:92; declaration placement inferred.
BOOL AIStateSetState(ActorID idActor, AIStateType eStateType, BOOL bForceAnimSet); BOOL AIStateSetMovementStyle(ActorID idActor, AIStateType eStateType, GBID gbidMovementStyle); void AIStateClearOverridePower(ActorID idActor); // DWARF AIStates.cpp:5501,5750,6101; declaration placement inferred.
BOOL GizmoSupportsProximityDetection(const GizmoType eGizmoType); // DWARF Gizmo.cpp:1145; declaration placement inferred.
void AIBroadcastSetActive(Actor & tActor, BOOL bOn); // DWARF AIBroadcast.cpp:46; declaration placement inferred.
void BrainStanceInitializeForActor(ActorID idActor); void BrainStanceChangeBrain(const ActorID idActor, ActorBrain nBrain); // DWARF BrainStance.cpp:1044,1313; declaration placement inferred.
void GizmoPressurePlateCountChanged(const ActorID idActor, const int32 nDelta, const int32 nCount, const BOOL fPassingThrough); // DWARF SGizmo.cpp:3182; declaration placement inferred.
static const int32 INACTIVE_SAVE_BLOCK_NUM_DWORDS = 4;
static const int32 INITIAL_INACTIVE_SAVE_COUNT = 32;

struct AIInactiveSaveBlock {  // size 0x18
    AIInactiveSaveBlockID id;  // 0x00
    uint32 ardwData[INACTIVE_SAVE_BLOCK_NUM_DWORDS];  // 0x04
    AIInactiveSaveBlockID idNextBlock;  // 0x14
};

typedef XListMemoryPool<ThreatListEntry> ThreatListEntryPool;

struct ActorAIGlobals {  // size 0x270
    XChainedFixedBlockAllocator<ActorAI> poolActorAIs;  // 0x00
    XChainedFixedBlockAllocator<AIStateInstanceData> poolAIStateInstanceData;  // 0x30
    XChainedFixedBlockAllocator<AINodePathData> poolNodePathData;  // 0x60
    XChainedFixedBlockAllocator<AILureData> poolAILureData;  // 0x90
    ActorID idTraceActor;  // 0xc0
    ACDID idTraceACD;  // 0xc4
    AIQueuedPowerArray tQueuedPowerArray;  // 0xc8
    int32 nQueuedPowerArraySize;  // 0xe0
    int32 nQueuedPowersUsed;  // 0xe4
    AIQueuedIDListPooled listFreeQueuedPowerIndexes;  // 0xe8
    AIQueuedCalloutArray tQueuedCalloutArray;  // 0x110
    int32 nQueuedCalloutArraySize;  // 0x128
    int32 nQueuedCalloutsUsed;  // 0x12c
    AIQueuedIDListPooled listFreeQueuedCalloutIndexes;  // 0x130
    TChainedDataArray<AIInactiveSaveBlock, int> * ptInactiveSaveBlockArray;  // 0x158
    TChainedDataArray<ProximityDetectorBrainData, int> * ptProximityDataArray;  // 0x160
    XPooledDataIDList listProximityDetectors;  // 0x168
    TAIMergeDataPool tAIMergeDataPool;  // 0x190
    CoolDownMapPool poolCoolDownMap;  // 0x1c0
    ThreatListEntryPool tThreatListEntryPool;  // 0x1f0
    uint32 uAdditionalBrainUpdateTicks;  // 0x220
    uint32 uWarnNearbyAlliesCooldownTicks;  // 0x224
    uint32 uWasAttackedAddTargetCooldownTicks;  // 0x228
    AffixSelectionList * ptTempAffixList;  // 0x230
    int32 * pnAffixListWeightTotal;  // 0x238
    TAffixSelectionPool tAffixSelectionPool;  // 0x240
};
static_assert(sizeof(ActorAIGlobals) == 0x270, "ActorAIGlobals");

class AIInactiveSave {  // size 0x18
public:
    AIInactiveSave(ActorSavedAI * pActorSavedAI);
    void SaveDWORD(int32 nIndex, uint32 dwData);
    int32 RestoreDWORD(uint32 & dwData, ActorID idActor);
    AIInactiveSaveBlockID AllocNewBlock();
    AIInactiveSaveBlock & GetBlockFromID(AIInactiveSaveBlockID idBlock);
    void IncData();
    void FreeAllBlocks();
private:
    ActorSavedAI * m_pActorSavedAI;  // 0x00
    AIInactiveSaveBlockID * m_pidCurrBlock;  // 0x08
    int32 m_nCurrDataIndex;  // 0x10
    int32 m_nCurrRestoreIndex;  // 0x14
};



void ActorAIInitForGame()
{

    SGame & tSGame = SGameManager::GameGet();
    ActorAIGlobals * & ptActorAIGlobals = tSGame.ptActorAIGlobals;
    ptActorAIGlobals = (ActorAIGlobals *)SigmaMemoryNew(sizeof(ActorAIGlobals), 4, tSGame.ptMemoryInterface, TRUE);

    ptActorAIGlobals->poolActorAIs.Initialize();
    ptActorAIGlobals->poolActorAIs.SetMemoryInterface(tSGame.ptMemoryInterface);
    ptActorAIGlobals->poolActorAIs.SetBlockCount_Tracked(150);

    ptActorAIGlobals->poolAIStateInstanceData.Initialize();
    ptActorAIGlobals->poolAIStateInstanceData.SetMemoryInterface(tSGame.ptMemoryInterface);
    ptActorAIGlobals->poolAIStateInstanceData.SetBlockCount_Tracked(150);

    ptActorAIGlobals->poolNodePathData.Initialize();
    ptActorAIGlobals->poolNodePathData.SetMemoryInterface(tSGame.ptMemoryInterface);
    ptActorAIGlobals->poolNodePathData.SetBlockCount_Tracked(127);

    ptActorAIGlobals->poolAILureData.Initialize();
    ptActorAIGlobals->poolAILureData.SetMemoryInterface(tSGame.ptMemoryInterface);
    ptActorAIGlobals->poolAILureData.SetBlockCount_Tracked(64);

    ptActorAIGlobals->idTraceActor = ACTORID_INVALID;
    ptActorAIGlobals->idTraceACD = ACDID_INVALID;

    new (&ptActorAIGlobals->tQueuedPowerArray) AIQueuedPowerArray;
    ptActorAIGlobals->nQueuedPowerArraySize = 0;
    ptActorAIGlobals->nQueuedPowersUsed = 0;
    ptActorAIGlobals->listFreeQueuedPowerIndexes.Initialize(MemoryPoolsGetDATAIDListPool());

    new (&ptActorAIGlobals->tQueuedCalloutArray) AIQueuedCalloutArray;
    ptActorAIGlobals->nQueuedCalloutArraySize = 0;
    ptActorAIGlobals->nQueuedCalloutsUsed = 0;
    ptActorAIGlobals->listFreeQueuedCalloutIndexes.Initialize(MemoryPoolsGetDATAIDListPool());

    ptActorAIGlobals->ptInactiveSaveBlockArray = TChainedDataArrayCreateTracked<AIInactiveSaveBlock, int>("AIInactiveSaves", tSGame.ptMemoryInterface, INITIAL_INACTIVE_SAVE_COUNT);
    ptActorAIGlobals->ptProximityDataArray = TChainedDataArrayCreateTracked<ProximityDetectorBrainData, int>("ProximityDetectorData", tSGame.ptMemoryInterface, INITIAL_PROXIMITY_DETECTOR_DATA_COUNT);

    ptActorAIGlobals->listProximityDetectors.Initialize(MemoryPoolsGetDATAIDListPool());

    ptActorAIGlobals->tAIMergeDataPool.Initialize();
    ptActorAIGlobals->tAIMergeDataPool.SetBlockCount_Tracked(128);

    ptActorAIGlobals->poolCoolDownMap.Initialize();
    ptActorAIGlobals->poolCoolDownMap.SetMemoryInterface(tSGame.ptMemoryInterface);
    ptActorAIGlobals->poolCoolDownMap.SetBlockCount_Tracked(256);

    ptActorAIGlobals->tThreatListEntryPool.Initialize();
    ptActorAIGlobals->tThreatListEntryPool.SetBlockCount_Tracked(128);

    ptActorAIGlobals->uAdditionalBrainUpdateTicks = 0;
    ptActorAIGlobals->uWarnNearbyAlliesCooldownTicks = 0;
    ptActorAIGlobals->uWasAttackedAddTargetCooldownTicks = 0;
    ptActorAIGlobals->ptTempAffixList = NULL;
    ptActorAIGlobals->pnAffixListWeightTotal = NULL;

    ptActorAIGlobals->tAffixSelectionPool.Initialize();
    ptActorAIGlobals->tAffixSelectionPool.SetBlockCount_Tracked(128);

    BrainTranslateInitForGame();
    BrainStanceInitForGame();
    BrainComponentInitForGame();

    int32 nCount;
}





void ActorAIDisposeForGame()

{
    BrainComponentDisposeForGame();
    BrainStanceDisposeForGame();
    BrainTranslateDisposeForGame();

    SGame & tSGame = SGameManager::GameGet();
    ActorAIGlobals * & ptActorAIGlobals = tSGame.ptActorAIGlobals;

    if (ptActorAIGlobals)
    {
        TChainedDataArrayDispose(ptActorAIGlobals->ptInactiveSaveBlockArray);
        TChainedDataArrayDispose(ptActorAIGlobals->ptProximityDataArray);

        ptActorAIGlobals->tQueuedPowerArray.~AIQueuedPowerArray();
        ptActorAIGlobals->listFreeQueuedPowerIndexes.Dispose();

        ptActorAIGlobals->tQueuedCalloutArray.~AIQueuedCalloutArray();
        ptActorAIGlobals->listFreeQueuedCalloutIndexes.Dispose();

        ptActorAIGlobals->poolAILureData.Dispose();
        ptActorAIGlobals->poolNodePathData.Dispose();
        ptActorAIGlobals->poolAIStateInstanceData.Dispose();
        ptActorAIGlobals->poolActorAIs.Dispose();
        ptActorAIGlobals->listProximityDetectors.Dispose();

        ptActorAIGlobals->tAIMergeDataPool.Dispose();
        ptActorAIGlobals->poolCoolDownMap.Dispose();
        ptActorAIGlobals->tThreatListEntryPool.Dispose();
        ptActorAIGlobals->tAffixSelectionPool.Dispose();

        SigmaMemoryFree(ptActorAIGlobals, tSGame.ptMemoryInterface); ptActorAIGlobals = NULL;
    }
}
void SimpleBrainActionUpdate(ActorID idActor); void ZombieBrainInitialize(const ActorID idActor); void ZombieBrainDispose(const ActorID idActor); void ZombieBrainActionUpdate(ActorID idActor); void ZombieBrainDeadUpdate(ActorID idActor); BOOL ZombieBrainOnDeath(const ActorID idActor, const ActorID idKillerActor); void SkeletonBrainInitialize(const ActorID idActor); void SkeletonBrainDispose(const ActorID idActor); void SkeletonBrainDeadUpdate(ActorID idActor); void SkeletonShieldBearerBrainUpdate(ActorID idActor); void SkeletonSummonerBrainInitialize(ActorID idActor); void SkeletonSummonerBrainUpdate(ActorID idActor); void FallenShamanBrainInitialize(ActorID idActor); void FallenShamanBrainUpdate(ActorID idActor); // DWARF SanctuaryBrains.cpp:80,260,288,335,404,439,509,529,539,623,685,695,793,803; declaration placement inferred.
void SpawnBrainInitialize(ActorID idActor); void SpawnBrainDispose(ActorID idActor); void SpawnBrainUpdate(ActorID idActor); // DWARF SpawnerBrain.cpp:946,1036,1168; declaration placement inferred.
void TriuneSummonerBrainInitialize(const ActorID idActor); void TriuneSummonerBrainDispose(const ActorID idActor); void TriuneSummonerBrainUpdate(ActorID idActor); void TriuneSummonerBrainActionUpdate(ActorID idActor); BOOL TriuneSummonerBrainOnDeath(const ActorID idActor, const ActorID idKillerActor); void TriuneVesselBrainInitialize(const ActorID idActor); void TriuneVesselBrainDispose(const ActorID idActor); void TriuneVesselBrainActionUpdate(ActorID idActor); void TriuneBerserkerBrainInitialize(ActorID idActor); void TriuneBerserkerBrainDispose(ActorID idActor); void TriuneBerserkerBrainActionUpdate(ActorID idActor); // DWARF TriuneBrains.cpp:47,80,89,218,373,394,409,418,526,539,577; declaration placement inferred.
void ProximityDetectorBrainInitialize(const ActorID idActor); void ProximityDetectorBrainInitializeForGizmo(const ActorID idActor, float flRadius); void ProximityDetectorBrainInitializeForAutoLootGizmo(const ActorID idActor); BOOL LureInitialize(ActorCommonData & tACD, AILureData & tLureData); // DWARF ProximityDetectorBrain.cpp:118/182/212, Lure.cpp:35; placement inferred.
void sTryAddChampionBrain(
    ActorID idActor)
{
    ACDID idACD = ACDFindByActor(idActor);
    if (idACD != ACDID_INVALID)
    {
        ActorCommonData * ptACD = ACDTryToGet(idACD);
        if (ptACD && ptACD->eMonsterRarity != 0)
        {
            ActorAI & tAI = ActorGetAI(idActor);

            tAI.pOriginalActionUpdateFunc = tAI.pBrainActionUpdateFunc;
            tAI.pBrainActionUpdateFunc = ChampionBrainActionUpdate;
        }
    }
}



static BOOL sBrainIsValidForGizmos(ActorBrain eBrain)
{
    switch (eBrain)
    {
    case ACTORBRAIN_SPAWNER:
    case ACTORBRAIN_DO_NOTHING:
    case ACTORBRAIN_SHRINE_RESURRECTION:
        return TRUE;
    default:
        return FALSE;
    }
}







static void sForceImmediateAction(
    Actor & tActor,
    uint32 uOffset)
{
    if (tActor.ptAI)
    {
        uint32 dwNow = GameGetServerTime();
        uint32 dwTime = dwNow + uOffset;
        tActor.ptAI->dwDelayBrainActionTime = dwTime;
        tActor.ptAI->dwDelayBrainUpdateTime = dwTime;
        tActor.ptAI->dwNextBrainUpdateTime = dwTime;
        ServerQueueUpdateFunction(uOffset, tActor.ptAI->tAIUpdateData);
    }
}






static BOOL sMovementBlockedCallback(
    ActorID idActor,
    ACDID idACDBlocker,
    const Vector2D & vDistUntilColl)
{
    if (vDistUntilColl.MagnitudeSquared() <= 100.0f)
    {
        Actor * ptActor = ActorTryToGet(idActor);
        if (ptActor && ptActor->ptAI)
        {
            ACDID idACD = ACDFindByActor(idActor);
            ActorCommonData * ptACD = ACDTryToGet(idACD);
            ActorCommonData * ptACDBlocker = ACDTryToGet(idACDBlocker);
            if (ptACD && ptACDBlocker && ptACDBlocker->IsWreckableObject())
            {
                Team eTeam = ptACD->GetTeam();
                Team eTeamBlocker = ptACDBlocker->GetTeam();
                if (eTeam != eTeamBlocker)
                {


                    MovementStyleDefinition tMoveStyle;
                    if (GBGetMovementStyleDef(ptActor->ptAI->gbidMovementStyle, tMoveStyle) && tMoveStyle.snoPowerToBreakObjects != SNO_NONE)
                    {
                        AIStateSetOverridePower(idActor, tMoveStyle.snoPowerToBreakObjects, idACDBlocker);
                        return TRUE;
                    }
                }
            }
        }
    }
    return FALSE;
}
void HirelingBrainInitialize(const ActorID idHireling); void HirelingBrainDispose(const ActorID idActor); BOOL HirelingBrainOnDeath(const ActorID idActor, const ActorID idKillerActor); void HirelingProxyBrainUpdate(ActorID idActor); // DWARF HirelingBrain.cpp:134,161,1789,1898; declaration placement inferred.
void MirrorImageBrainActionUpdate(ActorID idActor); void MirrorImageBrainDeadUpdate(const ActorID idActor); // DWARF MirrorImageBrain.cpp:28,141; declaration placement inferred.
void PigCagedBrainInitialize(const ActorID idActor); void PigCagedBrainDispose(const ActorID idActor); void PigCagedBrainActionUpdate(ActorID idActor); // DWARF AnimalBrains.cpp:35,49,161; declaration placement inferred.
void PlayerBrainInitialize(ActorID idActor); void PlayerBrainDispose(ActorID idActor); // DWARF PlayerBrain.cpp:790,800; declaration placement inferred.
void PowerSetCoolDown(ActorCommonData & tACD, const uint32 dwCooldown); // DWARF Powers.cpp:1715; declaration placement inferred.
void ResurrectionShrineBrainUpdate(ActorID idActor); // DWARF ShrineBrain.cpp:130; declaration placement inferred.
void SandTornadoBrainInitialize(const ActorID idActor); void SandTornadoBrainUpdate(ActorID idActor); // DWARF SandTornadoBrain.cpp:198,207; declaration placement inferred.
void SiegebreakerDemonBrainInitialize(const ActorID idActor); void SiegebreakerDemonBrainDispose(const ActorID idActor); void SiegebreakerDemonBrainUpdateWWI(ActorID idActor); // DWARF SiegebreakerDemonBrain.cpp:436,459,469; declaration placement inferred.
static AIBehaviorType sGetStartingBehavior(
    const MonsterDefinition * ptDefMonster)
{
    AIBehaviorType eBehavior = AIBEHAVIOR_TYPE_NORMAL;
    if (ptDefMonster->arAIBehaviors[eBehavior] == SNO_NONE)
    {
        eBehavior = AIBEHAVIOR_TYPE_NONE;
    }
    return eBehavior;
}
void WizardTornadoBrainInitialize(const ActorID idActor); void WizardTornadoBrainDispose(const ActorID idActor); void WizardTornadoBrainUpdate(ActorID idActor); void SentryBrainInitialize(ActorID idActor); void SentryBrainUpdate(ActorID idActor); BOOL SentryBrainOnDeath(const ActorID idActor, const ActorID idKillerActor); // DWARF PowerBrains.cpp:146,157,176,339,352,446; declaration placement inferred.
void WoodWraithBrainInitialize(const ActorID idActor); void WoodWraithBrainDispose(const ActorID idActor); void WoodWraithBrainActionUpdate(ActorID idActor); void SporePodBrainInitFunc(ActorID idActor); // DWARF WildlifeBrains.cpp:55,90,175,243; declaration placement inferred.

#include "../../Common/GizmoSwig.h"

void ActorSetBrain(
    const ActorID idActor,
    ActorBrain nBrain,
    ActorSavedAI * ptSavedAI)
{
    Actor * ptActor = ActorTryToGet(idActor);
    if (!ptActor)
    {
        return;
    }


    if (nBrain == ACTORBRAIN_NOBRAIN)
    {
        ActorAIDispose(idActor);
        return;
    }


    ACDID idACD = ACDFindByActor(idActor);
    ActorCommonData * ptACD = ACDTryToGet(idACD);


    if (!ptACD->IsInWorld())
    {
        ActorAIDispose(idActor);
        return;
    }


    if (ptACD->IsGizmo() && !sBrainIsValidForGizmos(nBrain))
    {
        nBrain = ACTORBRAIN_DO_NOTHING;
    }




    ScopedActorDefinitionLockR pdefActor(ptActor->snoActor);
    if (pdefActor->eType == ACTORTYPE_SERVER_PROP)
    {
        nBrain = ACTORBRAIN_DO_NOTHING;
    }

    if (ptActor->ptAI && ptActor->ptAI->nBrainType == nBrain)
    {
        return;
    }


    if (ActorHasBeenKilled(idActor))
    {
        return;
    }


    if (ActorHasBrain(idActor))
    {
        BrainStanceChangeBrain(idActor, nBrain);
    }

    ActorAIDispose(idActor);

    ActorAIGlobals * ptActorAIGlobals = SGameManager::GameGet().ptActorAIGlobals;
    ptActor->ptAI = ptActorAIGlobals->poolActorAIs.Alloc_Tracked();
    SigmaMemoryClear(ptActor->ptAI, sizeof(ActorAI));

    ActorAI & tAI = *ptActor->ptAI;

    tAI.listAIQueuedIDs.SetPool(MemoryPoolsGetDATAIDListPool());
    tAI.listAIQueuedCalloutIDs.SetPool(MemoryPoolsGetDATAIDListPool());
    tAI.tThreatList.SetPool(&ptActorAIGlobals->tThreatListEntryPool);

    UpdateFunctionInitialize(UPDATE_ACTOR_AI, idActor, tAI.tAIUpdateData);
    ServerQueueUpdateFunction(0, tAI.tAIUpdateData);

    tAI.nBrainType = nBrain;


    SigmaMemoryClear(tAI.pbNewBrainDataBuf, sizeof(tAI.pbNewBrainDataBuf));
    tAI.eBrainCondition = -1;
    tAI.dwConditionTimeout = 0;
    tAI.ptBrainComponentData = NULL;
    tAI.sBrainName = "-";
    tAI.sBrainStatus = "-";
    tAI.sBrainComponentStatus = "-";
    tAI.sBrainStance = "-";
    tAI.nUpdateInterval = 30;
    tAI.nRandomNoiseInterval = 20;
    tAI.dwNextBrainUpdateTime = 0;
    tAI.pdwCurrentDelayTimer = NULL; tAI.dwDelayBrainUpdateTime = tAI.dwDelayBrainDeadUpdateTime = tAI.dwDelayBrainActionTime = 0;



    tAI.dwTimeOfDeath = 0;
    tAI.dwTimeOfDead = 0;
    tAI.dwPersistentFlags = 0;
    tAI.idLastDamageSource = ACTORID_INVALID;
    tAI.dwLastAttackTime = 0;
    tAI.dwStatusFlags = 0;
    tAI.idACDTauntTarget = ACDID_INVALID;
    tAI.idProximityData = -1;
    tAI.idLeader = ACTORID_INVALID;
    tAI.idACDKiller = ACDID_INVALID;
    tAI.dwLastLeaderFollowTime = 0;
    tAI.bMaintainThreatList = FALSE;
    tAI.dwNextAllowedWarnNearbyAlliesTime = 0;

    if (MonsterGetTargetStyle(SNOName(ACTOR_SNO, ptActor->snoActor), ptACD->eMonsterRarity) == AITARGET_STYLE_HIGHEST_THREAT)
    {
        tAI.bMaintainThreatList = TRUE;
    }

    PowerSetCoolDown(*ptACD, 0);

    GameCommonData & tGCD = SGameGetGameCommonData();
    GameMode eGameMode = tGCD.GameModeGet();




    tAI.sBrainName = SymbolToName(nBrain, gActorBrainNames);

    switch (nBrain)
    {
    case ACTORBRAIN_MONSTER:
        tAI.pBrainActionUpdateFunc = SimpleBrainActionUpdate;
        break;

    case ACTORBRAIN_PLAYER:
        tAI.pBrainInitFunc = PlayerBrainInitialize;
        tAI.pBrainDisposeFunc = PlayerBrainDispose;
        break;

    case ACTORBRAIN_HIRELING:
        tAI.pBrainInitFunc = HirelingBrainInitialize;
        tAI.pBrainActionUpdateFunc = SimpleBrainActionUpdate;
        tAI.pBrainDisposeFunc = HirelingBrainDispose;
        tAI.pBrainOnDeathCallback = HirelingBrainOnDeath;
        tAI.nRandomNoiseInterval = 0;
        break;

    case ACTORBRAIN_HIRELING_PROXY:
        tAI.pBrainActionUpdateFunc = HirelingProxyBrainUpdate;
        break;

    case ACTORBRAIN_DO_NOTHING:

        break;
    case ACTORBRAIN_SPAWNER:
        tAI.pBrainInitFunc = SpawnBrainInitialize;
        tAI.pBrainDisposeFunc = SpawnBrainDispose;
        tAI.pBrainUpdateFunc = SpawnBrainUpdate;
        tAI.nRandomNoiseInterval = 0;
        break;

    case ACTORBRAIN_SHRINE_RESURRECTION:
        tAI.pBrainUpdateFunc = ResurrectionShrineBrainUpdate;
        break;

    case ACTORBRAIN_ZOMBIE:
        tAI.pBrainInitFunc = ZombieBrainInitialize;
        tAI.pBrainActionUpdateFunc = ZombieBrainActionUpdate;
        tAI.pBrainDisposeFunc = ZombieBrainDispose;
        tAI.pBrainDeadUpdateFunc = ZombieBrainDeadUpdate;
        tAI.pBrainOnDeathCallback = ZombieBrainOnDeath;
        break;

    case ACTORBRAIN_SKELETON_SUMMONER:
        tAI.pBrainInitFunc = SkeletonSummonerBrainInitialize;
        tAI.pBrainUpdateFunc = SkeletonSummonerBrainUpdate;
        break;

    case ACTORBRAIN_SIEGEBREAKER_DEMON:
        tAI.pBrainInitFunc = SiegebreakerDemonBrainInitialize;
        tAI.pBrainUpdateFunc = SiegebreakerDemonBrainUpdateWWI;
        tAI.pBrainDisposeFunc = SiegebreakerDemonBrainDispose;
        break;

    case ACTORBRAIN_SKELETON_SHIELDBEARER:
        tAI.pBrainInitFunc = SkeletonBrainInitialize;
        tAI.pBrainActionUpdateFunc = SkeletonShieldBearerBrainUpdate;
        tAI.pBrainDeadUpdateFunc = SkeletonBrainDeadUpdate;
        tAI.pBrainDisposeFunc = SkeletonBrainDispose;
        break;

    case ACTORBRAIN_TRIUNE_SUMMONER:
        tAI.pBrainInitFunc = TriuneSummonerBrainInitialize;
        tAI.pBrainDisposeFunc = TriuneSummonerBrainDispose;
        tAI.pBrainUpdateFunc = TriuneSummonerBrainUpdate;
        tAI.pBrainActionUpdateFunc = TriuneSummonerBrainActionUpdate;
        tAI.pBrainOnDeathCallback = TriuneSummonerBrainOnDeath;
        break;

    case ACTORBRAIN_PIG_CAGED:
        tAI.pBrainInitFunc = PigCagedBrainInitialize;
        tAI.pBrainDisposeFunc = PigCagedBrainDispose;
        tAI.pBrainActionUpdateFunc = PigCagedBrainActionUpdate;
        break;

    case ACTORBRAIN_WOODWRAITH:
        tAI.pBrainInitFunc = WoodWraithBrainInitialize;
        tAI.pBrainDisposeFunc = WoodWraithBrainDispose;
        tAI.pBrainActionUpdateFunc = WoodWraithBrainActionUpdate;
        break;

    case ACTORBRAIN_SPORE_POD:
        tAI.pBrainInitFunc = SporePodBrainInitFunc;
        break;

    case ACTORBRAIN_TRIUNEVESSEL:
        tAI.pBrainInitFunc = TriuneVesselBrainInitialize;
        tAI.pBrainDisposeFunc = TriuneVesselBrainDispose;
        tAI.pBrainActionUpdateFunc = TriuneVesselBrainActionUpdate;
        break;

    case ACTORBRAIN_TRIUNE_BERSERKER:
        tAI.pBrainInitFunc = TriuneBerserkerBrainInitialize;
        tAI.pBrainDisposeFunc = TriuneBerserkerBrainDispose;
        tAI.pBrainActionUpdateFunc = TriuneBerserkerBrainActionUpdate;
        break;

    case ACTORBRAIN_MIRROR_IMAGE:
        tAI.pBrainActionUpdateFunc = MirrorImageBrainActionUpdate;
        tAI.pBrainDeadUpdateFunc = MirrorImageBrainDeadUpdate;
        break;

    case ACTORBRAIN_WIZARD_TORNADO:
        tAI.pBrainInitFunc = WizardTornadoBrainInitialize;
        tAI.pBrainDisposeFunc = WizardTornadoBrainDispose;
        tAI.pBrainUpdateFunc = WizardTornadoBrainUpdate;
        break;

    case ACTORBRAIN_FALLEN_SHAMAN:
        tAI.pBrainInitFunc = FallenShamanBrainInitialize;
        tAI.pBrainUpdateFunc = FallenShamanBrainUpdate;
        break;

    case ACTORBRAIN_SAND_TORNADO:
        tAI.pBrainInitFunc = SandTornadoBrainInitialize;
        tAI.pBrainUpdateFunc = SandTornadoBrainUpdate;
        break;

    case ACTORBRAIN_SENTRY:
        tAI.pBrainInitFunc = SentryBrainInitialize;
        tAI.pBrainUpdateFunc = SentryBrainUpdate;
        tAI.pBrainOnDeathCallback = SentryBrainOnDeath;
        break;

    case ACTORBRAIN_VENDOR_WITH_ATTRACT:
        tAI.pBrainActionUpdateFunc = SimpleBrainActionUpdate;
        tAI.nRandomNoiseInterval = 0;
        break;


    default:
        tAI.sBrainName = "Do Nothing (missing)";
        break;
    }




    tAI.tAITetherData.placeTetherPoint = ptACD->GetPlace();


    float flRadius = ActorGetInheritedTagMapValueFloat(ptActor, TAG_ACTOR_AI_TETHER_RADIUS, TAG_MARKER_AI_TETHER_RADIUS);
    if (flRadius > 0.0f)
    {
        tAI.pBrainTetherUpdateFunc = TetherBrainUpdate;
        tAI.tAITetherData.flRadius = flRadius;
        tAI.tAITetherData.eTetherType = (TetherType)ActorGetInheritedTagMapValueInt(ptActor, TAG_ACTOR_AI_TETHER_TYPE, TAG_MARKER_AI_TETHER_TYPE);
    }
    else
    {
        tAI.pBrainTetherUpdateFunc = NULL;
    }


    ActorAISetStatusFlag(idActor, ACTOR_AI_HAS_BRAIN_BIT, TRUE);

    sTryAddChampionBrain(idActor);


    BrainStanceInitializeForActor(idActor);


    tAI.gbidMovementStyle = GBID_NONE;
    ActorAISetMovementStyle(idActor, 0, GBID_NONE);


    tAI.ptAIStateInstData = NULL;


    {
        SNO snoMonster = ptACD->GetMonsterSNO();
        if (snoMonster != SNO_NONE)
        {
            ScopedMonsterDefinitionLockR ptDefMonster(snoMonster);
            if (ptDefMonster)
            {
                AIBehaviorType eStartBehavior = sGetStartingBehavior(ptDefMonster);
                if (eStartBehavior != AIBEHAVIOR_TYPE_NONE)
                {
                    tAI.ptAIStateInstData = ptActorAIGlobals->poolAIStateInstanceData.Alloc_Tracked();

                    SigmaMemoryClear(tAI.ptAIStateInstData, sizeof(AIStateInstanceData));
                    tAI.ptAIStateInstData->snoLastPower = SNO_NONE;
                    tAI.ptAIStateInstData->idACDLastTarget = ACDID_INVALID;
                    tAI.ptAIStateInstData->eBehaviorType = AIBEHAVIOR_TYPE_NONE;
                    tAI.ptAIStateInstData->listMergeData.Initialize(&ptActorAIGlobals->tAIMergeDataPool);
                    tAI.ptAIStateInstData->mapCoolDown.Initialize(&ptActorAIGlobals->poolCoolDownMap);

                    AIStateClearOverridePower(idActor);

                    if (AIStateSetStateData(idActor, eStartBehavior))
                    {
                        for (int32 nStateType = 0; nStateType < 8; ++nStateType)
                        {
                            AIStateSetMovementStyle(idActor, (AIStateType)nStateType, ptDefMonster->gbidMovementStyles[nStateType]);
                        }


                        if (!AIStateSetState(idActor, 7, TRUE))
                        {
                            AIStateSetState(idActor, 0, TRUE);
                        }
                    }
                }
            }










        }
    }




    GizmoType eGizmoType = ptACD->GetGizmoType();
    if (GizmoSupportsProximityDetection(eGizmoType))
    {
        ProximityShape eProxShape = ActorGetInheritedTagMapValueInt(ptActor, TAG_ACTOR_AI_PROXIMITY_DETECTOR_TYPE, TAG_MARKER_PROXIMITY_DETECTOR_TYPE);
        if (eProxShape != -1)
        {
            ProximityDetectorBrainInitialize(idActor);
            if (eGizmoType == ACTORGIZMO_PRESSUREPLATE)
            {
                ActorAI & _tAI = ActorGetAI(idActor);
                ProximityDetectorBrainData * ptProxData = ProximityDetectorTryGetData(_tAI.idProximityData);
                if (ptProxData)
                {
                    ptProxData->pOnCountChangedCallback = GizmoPressurePlateCountChanged;
                }
            }
        }
    }

    else if (eGizmoType == ACTORGIZMO_CHEST || eGizmoType == ACTORGIZMO_LORE_CHEST)
    {

        ScopedActorDefinitionLockR _pdefActor(ptACD->snoActor);
        if (TagMapGetInt(_pdefActor->hTagMap, TAG_ACTOR_GIZMO_CHEST_OPERATES_ON_PROXIMITY))
        {
            ProximityDetectorBrainInitializeForAutoLootGizmo(idActor);
        }
    }
    else if (eGizmoType == ACTORGIZMO_BANNER)
    {


        const float _flRadius = 20.0f;
        ProximityDetectorBrainInitializeForGizmo(idActor, _flRadius);
    }
    else if (eGizmoType == ACTORGIZMO_PORTAL)
    {

        ProximityDetectorBrainInitializeForAutoLootGizmo(idActor);
    }



    tAI.ptLureData = NULL;
    if (TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_LURE_ENABLED))
    {
        if (!TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_LURE_ONLY_IF_LURER_IS_NORMAL) ||
            ptACD->eMonsterRarity == 0)
        {
            tAI.ptLureData = ptActorAIGlobals->poolAILureData.Alloc_Tracked();
            if (tAI.ptLureData)
            {
                LureInitialize(*ptACD, *tAI.ptLureData);
            }
        }
    }

    AIBroadcastSetActive(*ptActor, TRUE);

    if (ptSavedAI)
    {







        ActorAIRestoreForActivatingActor(idActor, *ptSavedAI);
    }

    if (ptActor->ptSActorMovement)
    {

        ptActor->ptSActorMovement->SetMovementBlockedCallback(sMovementBlockedCallback);
    }

    if (tAI.pBrainInitFunc)
    {
        tAI.pBrainInitFunc(idActor);
    }
}




BOOL ActorHasBrain(ActorID idActor)
{
    Actor * ptActor = ActorTryToGet(idActor);
    if (ptActor == NULL) {
        return FALSE;
    }
    if (ptActor->ptAI == NULL) {
        return FALSE;
    }
    return ActorAIIsStatusFlagSet(idActor, ACTOR_AI_HAS_BRAIN_BIT);
}

















AIQueuedPower * sGetQueuedPowerFromID(AIQueuedActionID idQActionID)
{
    ActorAIGlobals * ptActorAIGlobals = SGameManager::GameGet().ptActorAIGlobals;
    return &ptActorAIGlobals->tQueuedPowerArray[idQActionID & 0xffff];
}












AIQueuedActionID sAllocQueuedAction(
    void)
{
    ActorAIGlobals * ptActorAIGlobals = SGameManager::GameGet().ptActorAIGlobals;


    ptActorAIGlobals->nQueuedPowersUsed++;

    uint32 dwArrayIndex;
    if (ptActorAIGlobals->listFreeQueuedPowerIndexes.Count() > 0)
    {
        dwArrayIndex = ptActorAIGlobals->listFreeQueuedPowerIndexes.GetHead();
        ptActorAIGlobals->listFreeQueuedPowerIndexes.RemoveHead();
    }
    else
    {
        if (ptActorAIGlobals->nQueuedPowerArraySize >= MAX_SIMULATEOUS_QUEUED_POWERS)
        {
            return AIQID_INVALID;
        }

        AIQueuedPower tPower;
        tPower.snoPower = SNO_NONE;
        tPower.tTarget = cgTargetNone;
        tPower.uAnimTag = (uint32)-1;
        tPower.uNewIdleAnimTag = (uint32)-1;
        tPower.eType = QUEUED_ACTION_INVALID;

        dwArrayIndex = ptActorAIGlobals->nQueuedPowerArraySize;
        blzext::vector_set_at_grow(ptActorAIGlobals->tQueuedPowerArray, ptActorAIGlobals->nQueuedPowerArraySize++, tPower);
    }


    uint32 dwID = (ptActorAIGlobals->nQueuedPowersUsed << 16) | dwArrayIndex;

    return dwID;
}



void sFreeQueuedAction(AIQueuedActionID idQActionID)
{
    ActorAIGlobals * ptActorAIGlobals = SGameManager::GameGet().ptActorAIGlobals;
    uint32 dwArrayIdx = idQActionID & 0xffff;

    ptActorAIGlobals->listFreeQueuedPowerIndexes.AddTail_Tracked(dwArrayIdx);
    ptActorAIGlobals->tQueuedPowerArray[dwArrayIdx].snoPower = SNO_NONE;
    ptActorAIGlobals->tQueuedPowerArray[dwArrayIdx].uNewIdleAnimTag = (uint32)-1;
}






AIQueuedCallout * sGetQueuedCalloutFromID(
    AIQueuedCalloutID idQCalloutID)
{
    ActorAIGlobals * ptActorAIGlobals = SGameManager::GameGet().ptActorAIGlobals;


    uint32 dwArrayIndex = idQCalloutID & 0xffff;



    AIQueuedCallout * ptCalloutInfo = &ptActorAIGlobals->tQueuedCalloutArray[dwArrayIndex];

    return ptCalloutInfo;
}



AIQueuedCalloutID sAllocQueuedCallout()
{
    ActorAIGlobals * ptActorAIGlobals = SGameManager::GameGet().ptActorAIGlobals;
    uint32 dwArrayIndex;

    ptActorAIGlobals->nQueuedCalloutsUsed++;

    if (ptActorAIGlobals->listFreeQueuedCalloutIndexes.Count() > 0) {
        dwArrayIndex = ptActorAIGlobals->listFreeQueuedCalloutIndexes.GetHead();
        ptActorAIGlobals->listFreeQueuedCalloutIndexes.RemoveHead();
    } else {
        if (ptActorAIGlobals->nQueuedCalloutArraySize >= MAX_SIMULATEOUS_QUEUED_CALLOUTS) {
            return AIQCID_INVALID;
        }

        AIQueuedCallout tCallout;
        SigmaMemoryClear(&tCallout, sizeof(tCallout));
        tCallout.snoConversation = SNO_NONE;
        dwArrayIndex = ptActorAIGlobals->nQueuedCalloutArraySize++;
        blzext::vector_set_at_grow(ptActorAIGlobals->tQueuedCalloutArray, dwArrayIndex, tCallout);
    }

    uint32 dwID = dwArrayIndex | (ptActorAIGlobals->nQueuedCalloutsUsed << 16);
    return dwID;
}












void sFreeQueuedCallout(AIQueuedCalloutID idQCalloutID)
{
    ActorAIGlobals * ptActorAIGlobals = SGameManager::GameGet().ptActorAIGlobals;
    uint32 dwArrayIdx = idQCalloutID & 0xffff;

    ptActorAIGlobals->listFreeQueuedCalloutIndexes.AddTail_Tracked(dwArrayIdx);
    ptActorAIGlobals->tQueuedCalloutArray[dwArrayIdx].snoConversation = SNO_NONE;
}
void AIQueueCalloutClearList(ActorID idActor); // This file:3323.
void BrainStanceDisposeForActor(ActorID idActor); // DWARF BrainStance.cpp:1071; placement inferred.
void ProximityDetectorBrainDispose(const ActorID idActor); // DWARF ProximityDetectorBrain.cpp:226; placement inferred.
void ServerRemoveUpdateFunction(UpdateData & tUpdateData); // DWARF SGame.cpp:1829; placement inferred.



void ActorAIDispose(
    const ActorID idActor)
{
    Actor * ptActor = ActorGet(idActor);

    if (!ptActor->ptAI)
    {
        return;
    }

    ActorAI & tAI = *ptActor->ptAI;

    AIQueueClearList(idActor);
    tAI.listAIQueuedIDs.Dispose();

    AIQueueCalloutClearList(idActor);
    tAI.listAIQueuedCalloutIDs.Dispose();

    ProximityDetectorBrainData * ptProxData = ProximityDetectorTryGetData(tAI.idProximityData);
    if (ptProxData)
    {
        ProximityDetectorBrainDispose(idActor);
        tAI.idProximityData = -1;
    }

    if (tAI.pBrainDisposeFunc)
    {
        tAI.pBrainDisposeFunc(idActor);
    }

    if (ActorHasBrain(idActor))
    {
        BrainStanceDisposeForActor(idActor);
    }

    ServerRemoveUpdateFunction(tAI.tAIUpdateData);

    SigmaMemoryFree(tAI.pBrainData, NULL); tAI.pBrainData = NULL;

    ActorAIGlobals * ptActorAIGlobals = SGameManager::GameGet().ptActorAIGlobals;

    if (tAI.ptNodePathData)
    {
        ptActorAIGlobals->poolNodePathData.Free(tAI.ptNodePathData);
    }

    if (tAI.ptLureData)
    {
        ptActorAIGlobals->poolAILureData.Free(tAI.ptLureData);
    }

    if (tAI.ptAIStateInstData)
    {
        tAI.ptAIStateInstData->listMergeData.Dispose();
        tAI.ptAIStateInstData->mapCoolDown.Dispose();
        ptActorAIGlobals->poolAIStateInstanceData.Free(tAI.ptAIStateInstData);
    }

    tAI.tThreatList.Dispose();

    ptActorAIGlobals->poolActorAIs.Free(ptActor->ptAI);
}




BOOL ACDCanAttackTarget(const ActorCommonData & tACD, const Target & tTarget, AutoPowerDefinitionR pdefPower, uint32 * pdwFailureReasons)
{
    uint32 dwFailureReasons;
    if (pdwFailureReasons) {
        dwFailureReasons = *pdwFailureReasons;
    } else {
        dwFailureReasons = PowerFailureReasons(tACD, AutoPowerDefinitionR(pdefPower), &tTarget, TRUE, FALSE);
    }

    if (dwFailureReasons & (1 << PS_INVALID_SOURCE)) {
        return FALSE;
    }

    if (dwFailureReasons & (1 << PS_INVALID_TARGET)) {
        return FALSE;
    }

    return TRUE;
}












BOOL ACDCanBeAffectedByPowers(const ACDID idACD)
{
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD && ptACD->IsGizmo()) {
        return TRUE;
    }
    ActorID idActor = (ActorID)ACDGetOwner(idACD);
    if (ActorHasBrain(idActor)) {
        return TRUE;
    }
    return FALSE;
}










BOOL ActorHasBeenKilled(const ActorID idActor)
{
    if (ActorHasBrain(idActor)) {
        EStanceType nStanceType = BrainGetStanceType(idActor);
        return nStanceType == STANCE_DYING || nStanceType == STANCE_DEAD;
    }
    return FALSE;
}
































































































void ActorAIWasAttacked(ActorID idTargetActor, ActorID idSourceActor)
{
    ActorAI & tTargetAI = ActorGetAI(idTargetActor);
    uint32 dwNow = GameGetServerTime();
    tTargetAI.idLastDamageSource = idSourceActor;
    tTargetAI.dwLastDamageTime = dwNow;
    if (idSourceActor != ACTORID_INVALID) {
        ActorAI * ptSourceAI = ActorTryToGetAI(idSourceActor);
        if (ptSourceAI) {
            ptSourceAI->dwLastAttackTime = dwNow;
        }
    }
}




























ActorAI & ActorGetAI(const ActorID idActor)
{
    Actor * ptActor = ActorGet(idActor);
    return *ptActor->ptAI;
}





ActorAI & ActorGetAI(Actor & tActor)
{
    return *tActor.ptAI;
}






ActorAI * ActorTryToGetAI(const ActorID idActor)
{
    Actor * ptActor = ActorTryToGet(idActor);
    if (ptActor) {
        return ptActor->ptAI;
    }
    return NULL;
}













ActorBrain ActorGetBrainType(const ActorID idActor)
{
    const Actor * ptActor = ActorTryToGet(idActor);
    if (ptActor == NULL) {
        return ACTORBRAIN_UNDEFINED;
    }
    if (ptActor->ptAI == NULL) {
        return ACTORBRAIN_NOBRAIN;
    }
    ActorAI & tAI = ActorGetAI(idActor);
    return tAI.nBrainType;
}










void ActorSetPowerState(const ActorID idActor, LPCSTR sBrainState)
{
    Actor * ptActor = ActorTryToGet(idActor);
    if (ptActor) {
        Blizzard::String::Copy(ptActor->szPowerState, sBrainState, sizeof(ptActor->szPowerState));
    }
}







void BrainSetStatus(ActorID idActor, LPCSTR sBrainStatus)
{
    ActorAI & tAI = ActorGetAI(idActor);
    tAI.sBrainStatus = sBrainStatus;
}






void BrainDelayNextUpdate(ActorID idActor, int32 nMinTime, int32 nDeltaTime)
{
    ActorAI & tAI = ActorGetAI(idActor);
    uint32 dwNow = GameGetServerTime();
    int32 nRandom = RandRange_Internal(SGameGlobalsGet().rng, 0, nDeltaTime);
    *tAI.pdwCurrentDelayTimer = dwNow + tAI.nUpdateInterval + nMinTime + nRandom;
}


void AIQueueCalloutClearList(ActorID idActor); // This file:3323.
void AIQueueCalloutUpdate(ActorID idActor); // This file:3202.
void AIQueueActionUpdate(ActorID idActor); // This file:2414.
BrainCondition BrainConditionGetCondition(ActorID idActor); // DWARF BrainComponents.cpp:663; header placement inferred.
BOOL PowerActorCanDoAnything(ActorID idActor); // DWARF ActorIsAffected.cpp:169; header placement inferred.
BOOL ACDWarpToACD(ActorCommonData & tACDToWarp, const ActorCommonData & tACDTarget, const PlayerWarpReason eReason, float flMinWarpRadius); // This file:1802.
#include "../SGameConstants.h"
void ProximityDetectorBrainUpdate(const ActorID idActor); // DWARF ProximityDetectorBrain.cpp:715; header placement inferred.
void LureUpdate(ActorCommonData & tACD, AILureData & tLureData); // DWARF Lure.cpp:217; header placement inferred.
static BOOL sFollowLeader(
    ActorID idActor)
{
    Actor * ptActor = ActorGet(idActor);
    ActorAI & tAI = ActorGetAI(idActor);

    Actor * ptLeader = ActorTryToGet(tAI.idLeader);
    if (ptLeader == NULL)
        return FALSE;




    ActorCommonData & tACD = ACDGet(ptActor->idACD);
    ActorCommonData & tACDLeader = ACDGet(ptLeader->idACD);

    if (ACDWarpToACD(tACD, tACDLeader, 0, 80.0f))
        return TRUE;



    WorldPlace placeActor = tACD.GetPlace();
    WorldPlace placeLeader = tACDLeader.GetPlace();

    float flLagRadius = 15.0f;

    int32 nTargets = BrainAPIGetNumEnemyTargets(idActor);
    if (nTargets > 0)
        flLagRadius = 60.0f;





    float flTime = 120.0f;
    BOOL fRecentlyStartedRunningTowardLeader = (float)GameGetServerTime() < tAI.dwLastLeaderFollowTime + flTime;
    if (fRecentlyStartedRunningTowardLeader)
        flLagRadius = 5.0f;



    WorldDistance wdToLeaderSquared = WorldPlaceDistanceSquared(placeActor, placeLeader);

    flLagRadius += tACD.GetRadius();

    if (wdToLeaderSquared < flLagRadius * flLagRadius)
        return FALSE;




    tAI.dwLastLeaderFollowTime = GameGetServerTime();

    BrainAPIDoWalk(
        ptActor->id, placeLeader, 0.0f, 2, 0x11060);


    return TRUE;
}







void BrainUpdate(
    Actor & tActor)
{
    ActorAI & tAI = ActorGetAI(tActor);
    ActorID idActor = tActor.id;
    BOOL bUsingQueue = ActorIsUsingAIQueue(idActor);
    ACDID idACD = ACDFindByActor(idActor);
    ActorCommonData * ptACD = ACDTryToGet(idACD);

    if (ptACD && !ptACD->IsAlive())
    {




        AIQueueCalloutClearList(idActor);

        if (bUsingQueue)
        {
            AIQueueClearList(idActor);
            bUsingQueue = FALSE;
        }
    }

    if (!tAI.pBrainUpdateFunc && !tAI.pBrainDeadUpdateFunc && !tAI.pBrainActionUpdateFunc &&
        !ProximityDetectorTryGetData(tAI.idProximityData) && !bUsingQueue && !tAI.ptLureData)
    {
        tAI.dwNextBrainUpdateTime = (uint32)-1;
        return;
    }


    uint32 dwNow = GameGetServerTime();



    {

        int32 nRandom = 0;
        if (tAI.nRandomNoiseInterval > 0)

            nRandom = RandRange_Internal(SGameGlobalsGet().rng, 0, tAI.nRandomNoiseInterval);


        ActorAIGlobals * ptActorAIGlobals = SGameManager::GameGet().ptActorAIGlobals;


        tAI.dwNextBrainUpdateTime = dwNow + tAI.nUpdateInterval + nRandom + ptActorAIGlobals->uAdditionalBrainUpdateTicks;
    }



    if ((SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_AI_DISABLED_BIT)) ||
        (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_PVP_GAME_OVER_BIT)))
        return;




    if (!bUsingQueue && ActorAIIsStatusFlagSet(idActor, ACTOR_AI_DISABLE_UPDATES_BIT))
        return;



    if (!ptACD->IsEnabled())
        return;






    AIQueueCalloutUpdate(idActor);

    BOOL bFollowed = FALSE;
    if (!bUsingQueue)
    {
        if (ActorAIIsStatusFlagSet(idActor, ACTOR_AI_FOLLOW_LEADER_BIT))



            bFollowed = sFollowLeader(idActor);




        if (tAI.pBrainUpdateFunc &&
            tAI.dwDelayBrainUpdateTime <= dwNow &&
            !ActorHasBeenKilled(idActor))
        {
            tAI.pdwCurrentDelayTimer = &tAI.dwDelayBrainUpdateTime;
            if (!bFollowed)
                tAI.pBrainUpdateFunc(idActor);
        }
    }






    ProximityDetectorBrainData * ptProxData = ProximityDetectorTryGetData(tAI.idProximityData);
    if (ptProxData)
    {


        ProximityDetectorBrainUpdate(idActor);
    }



    if (tAI.ptLureData)
    {
        if (ptACD->AttributesGetInt(FastAttribKey(ATTRIBUTE_IS_LURE_ENABLED)))

            LureUpdate(*ptACD, *tAI.ptLureData);
    }




    if (tAI.pBrainDeadUpdateFunc &&
        tAI.dwDelayBrainDeadUpdateTime <= dwNow &&
        ActorHasBeenKilled(idActor))
    {
        tAI.pdwCurrentDelayTimer = &tAI.dwDelayBrainDeadUpdateTime;
        tAI.pBrainDeadUpdateFunc(idActor);
    }






    bUsingQueue = ActorIsUsingAIQueue(idActor);



    if ((tAI.pBrainActionUpdateFunc || bUsingQueue)
      && (tAI.dwDelayBrainActionTime <= dwNow
       || BrainConditionGetCondition(idActor) == BRAIN_CONDITION_ABORTED)
      && PowerActorCanDoAnything(idActor)
      && BrainConditionIsReady(idActor))
    {
        tAI.pdwCurrentDelayTimer = &tAI.dwDelayBrainActionTime;

        if (bUsingQueue)
        {


            AIQueueActionUpdate(idActor);
        }
        else if ((!tAI.pBrainTetherUpdateFunc || !tAI.pBrainTetherUpdateFunc(idActor)) && !bFollowed)
        {



            tAI.pBrainActionUpdateFunc(idActor);
        }
    }
}











void ActorAIRunUpdateNow(Actor & tActor)
{
    if (ActorAIIsStatusFlagSet(tActor, ACTOR_AI_HAS_BRAIN_BIT)) {
        BrainRunUpdates(tActor);
    } else {
        BrainUpdate(tActor);
    }
}












void ActorAIUpdate(const ActorID idActor)
{
    Actor * ptActor = ActorTryToGet(idActor);
    if (ptActor == NULL) {
        return;
    }
    ActorCommonData & tACD = ACDGet(ptActor->idACD);
    GameDebugSetCurrentUpdatingActor(ptActor->idACD);
    ServerUpdateWasCalled(ptActor->ptAI->tAIUpdateData);
    ServerQueueUpdateFunction(AI_UPDATE_RATE, ptActor->ptAI->tAIUpdateData);
    AnimPlayerUpdate(tACD);
    ActorAIRunUpdateNow(*ptActor);
    FastAttribGroupApplyDirty(tACD.AttributesGetAttributeGroup());
}






























BOOL ACDIsSummoned(const ACDID idACD)
{
    return ACDGetSummoner(idACD) != ACDID_INVALID;
}





BOOL ACDPetCanTarget(ActorCommonData & tACDPet, ActorCommonData & tACDTarget)
{
    if (tACDTarget.AttributesGetInt(FastAttribKey(ATTRIBUTE_UNTARGETABLE_BY_PETS)) == 1) {
        return FALSE;
    }
    return TRUE;
}


























































void AITrace(DataID, LPCSTR, ...) {}





BOOL ACDWarpToACD(ActorCommonData & tACDToWarp, const ActorCommonData & tACDTarget, const PlayerWarpReason eReason, float flMinWarpRadius)
{
    Target tWarpTarget;
    tWarpTarget.InitializeFromACD(tACDTarget.id, FALSE, FALSE);
    return ACDWarpToTarget(tACDToWarp, tWarpTarget, eReason, flMinWarpRadius, FALSE);
}









void ACDWarpLastHeadStone(const ActorCommonData & tACD)
{
    ACDID idACDHeadStone = GetLastHeadstoneACD(tACD);
    ActorCommonData * ptACDHeadstone = ACDTryToGet(idACDHeadStone);
    if (ptACDHeadstone) {
        ACDWarpToACD(*ptACDHeadstone, tACD, 11, 5.0f);
    }
}





BOOL ACDWarpToTarget(
    ActorCommonData & tACDToWarp,
    const Target & tWarpTarget,
    const PlayerWarpReason eReason,
    float flMinWarpRadius,
    BOOL bFailOnBadLocation)
{
    ActorID idActor = (ActorID)ACDGetOwner(tACDToWarp.id);
    const Actor * ptActor = ActorTryToGet(idActor);
    if (!ptActor)
    {
        DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }


    WorldPlace placeACD = tACDToWarp.GetPlace();

    ACDID idACDTarget = tWarpTarget.GetACD();
    WorldPlace placeTarget;
    tWarpTarget.GetPlace(placeTarget);

    BOOL bWarped = FALSE;
    BOOL bResult = TRUE;

    if (!WorldPlaceInSameWorld(placeACD, placeTarget))
    {
        WorldPlace placeWarp;
        if (idACDTarget != ACDID_INVALID)
        {
            bResult = ACDFindSpawnLocation(idACDTarget, PASSABILITY_MONSTER, placeWarp);
        }
        else
        {
            tWarpTarget.GetPlace(placeWarp);
        }

        if (!bFailOnBadLocation || bResult)
        {
            ActorChangeWorld(idActor, placeWarp, cgqIdentity);
            bWarped = TRUE;
            BrainAPISetWanderCenter(ptActor->id, placeWarp);
        }
    }
    else
    {
        WorldDistance wdToLeader = WorldPlaceDistance(placeACD, placeTarget);

        if (wdToLeader > flMinWarpRadius)
        {
            WorldPlace placeWarp;
            if (idACDTarget != ACDID_INVALID)
            {
                if (!ACDIsPlayer(tACDToWarp.id))
                {
                    bResult = ACDFindSpawnLocation(idACDTarget, PASSABILITY_MONSTER, placeWarp);
                }
                else
                {
                    SPlayerFindGoodSpawnLocation(placeTarget, placeWarp, 50.0f);
                }
            }
            else
            {
                tWarpTarget.GetPlace(placeWarp);
            }

            if (!bFailOnBadLocation || bResult)
            {
                ActorSnapToPosition(ptActor->id, placeWarp, TRUE, TRUE);
                bWarped = TRUE;
                BrainAPISetWanderCenter(ptActor->id, placeWarp);
            }
        }
    }


    BOOL bSendClientWarped = TRUE;
    if (idACDTarget != ACDID_INVALID)
    {
        ActorCommonData & tACDTarget = ACDGet(idACDTarget);
        bSendClientWarped = ACDGetCurrentSSceneID(tACDTarget) != ACDGetCurrentSSceneID(tACDToWarp);
    }




    if (eReason == 15)
    {
        bSendClientWarped = FALSE;
    }



    if (bWarped && ACDIsPlayer(tACDToWarp.id))
    {
        SPlayerWarped(tACDToWarp, eReason, NULL, ACDID_INVALID, 0.0f, bSendClientWarped);
    }

    return bWarped;
}



void ACDWarpAllPetsToPlayer(
    ActorCommonData & tACDPlayer,
    const PlayerWarpReason eReason)
{
    int32 nPos;
    SNO snoLevelArea = ACDGetLevelArea(tACDPlayer);


    ACDID idACDPet = PlayerGetFirstPet(tACDPlayer.id, PETTYPE_NONE, nPos);
    while (idACDPet != ACDID_INVALID)
    {

        ActorCommonData * ptACDPet = ACDTryToGet(idACDPet);
        if (ptACDPet)
        {
            PetType eType = ptACDPet->AttributesGetInt(FastAttribKey(ATTRIBUTE_PET_TYPE));
            if (eType == PETTYPE_WITCHDOCTOR_DEATH_STATUE_TOTEM ||
                eType == PETTYPE_WITCHDOCTOR_FIRE_STATUE_TOTEM ||
                eType == PETTYPE_NECROMANCER_SKELETON_MAGE_DECAY ||
                eType == PETTYPE_GENERIC_STATIC_PET_0 ||
                eType == PETTYPE_GENERIC_STATIC_PET_1 ||
                eType == PETTYPE_GENERIC_STATIC_PET_2 ||
                eType == PETTYPE_GENERIC_STATIC_PET_3 ||
                eType == PETTYPE_GENERIC_STATIC_PET_4 ||
                eType == PETTYPE_GENERIC_STATIC_PET_5 ||
                eType == PETTYPE_GENERIC_STATIC_PET_6)
            {
                BrainAPIDoDeath((ActorID)ACDGetOwner(idACDPet), ACTORID_INVALID, TRUE, TRUE, TRUE, NULL, NULL);
            }
            else if (eType == PETTYPE_HIRELING_PROXY)
            {

                NPCHandleDismissal(*ptACDPet, FALSE, TRUE);
            }
            else if (ptACDPet->IsAlive() || eType == PETTYPE_HIRELING)
            {



                if (eType == PETTYPE_HELPER &&
                    !HelperIsLevelAreaAllowed(*ptACDPet, snoLevelArea))
                {

                    NPCHelperDetach(tACDPlayer, *ptACDPet);
                }
                else if (eType != PETTYPE_FOLLOWER ||
                         EscortCanBePulledThroughWarp(*ptACDPet, tACDPlayer))
                {






                    if (eType == PETTYPE_COMPANION)
                    {
                        SNO snoProxyActor = GlobalSNOGet(GLOBALSNO_ACTOR_GENERIC_PROXY);

                        ActorAddStruct tActorAdd;
                        tActorAdd.snoActor = snoProxyActor;
                        tActorAdd.dwActorAddFlags = (1 << ACTORADD_BRAINLESS_BIT) | (1 << ACTORADD_DOESNT_WALK_BIT) | (1 << ACTORADD_DOESNT_DEACTIVATE);
                        tActorAdd.eFastAttribType = 0;
                        tActorAdd.eTeam = ptACDPet->GetTeam();
                        tActorAdd.tPlace = ptACDPet->GetPlace();
                        tActorAdd.qOrientation = ptACDPet->GetOrientation();

                        {
                            ScopedActorDefinitionLockR pdefActor(snoProxyActor);
                            tActorAdd.flScale = TagMapGetFloat(pdefActor->hTagMap, TAG_ACTOR_SCALE_MIN);
                            tActorAdd.tSNOName = SNOName(ACTOR_SNO, snoProxyActor);
                        }


                        ActorID idActorProxy = ActorAdd(tActorAdd);
                        if (idActorProxy != ACTORID_INVALID)
                        {
                            ACDID idACDProxy = ACDFindByActor(idActorProxy);
                            if (idACDProxy != ACDID_INVALID)
                            {
                                ActorCommonData & tACDProxy = ACDGet(idACDProxy);
                                tACDProxy.AttributesSetInt(FastAttribKey(ATTRIBUTE_DROPS_NO_LOOT), TRUE);
                                tACDProxy.AttributesSetInt(FastAttribKey(ATTRIBUTE_GRANTS_NO_XP), TRUE);
                                tACDProxy.AttributesSetInt(FastAttribKey(ATTRIBUTE_INVULNERABLE), TRUE);
                                ActorFlagSet(idActorProxy, ACTOR_DOESNT_DEACTIVATE_BIT, TRUE);
                                tACDProxy.AttributesSetInt(FastAttribKey(ATTRIBUTE_REMOVES_BODY_ON_DEATH), TRUE);
                                tACDProxy.AttributesSetInt(FastAttribKey(ATTRIBUTE_IS_POWER_PROXY), TRUE);

                                uint32 dwDuration = 120;

                                ActorDeleteSoon(idActorProxy, dwDuration);

                                ACDPlayEffect(idACDProxy, EFFECT_COSMETIC_PET_WARP_OUT, 0, ACDID_INVALID);
                            }
                        }
                    }



                    float flWarpRadius = 40.0f;
                    if (eReason == 10)
                        flWarpRadius = 0.0f;


                    ACDWarpToACD(*ptACDPet, tACDPlayer, eReason, flWarpRadius);


                    ActorID idActorPet = (ActorID)ptACDPet->idOwner;

                    if (!ptACDPet->IsAlive())
                    {
                        BrainAPIDoResurrect(idActorPet, TRUE);
                    }

                    else
                    {
                        BrainAPIDoIdle(idActorPet);


                        AnimPlayerSendIdle(*ptACDPet);
                    }
                }
            }
        }
        else
            { DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL); }



        idACDPet = PlayerGetNextPet(tACDPlayer.id, PETTYPE_NONE, nPos);
    }
}





void ActorAISetStatusFlag(const ActorID idActor, const ActorAIStatusFlag eFlag, const BOOL bValue)
{
    ActorAI * ptAI = ActorTryToGetAI(idActor);
    if (ptAI) {
        if (bValue) {
            ptAI->dwStatusFlags |= (1 << eFlag);
        } else {
            ptAI->dwStatusFlags &= ~(1 << eFlag);
        }
    }
}



BOOL ActorAIIsStatusFlagSet(const ActorID idActor, const ActorAIStatusFlag eFlag)
{
    ActorAI & tAI = ActorGetAI(idActor);
    return (tAI.dwStatusFlags >> eFlag) & 1;
}





BOOL ActorAIIsStatusFlagSet(Actor & tActor, const ActorAIStatusFlag eFlag)
{
    if (tActor.ptAI == NULL) {
        return FALSE;
    }
    return (tActor.ptAI->dwStatusFlags >> eFlag) & 1;
}







void ActorAISetTauntTarget(ActorID idActorTauntee, ACDID idACDTaunter)
{
    ActorAI & tAI = ActorGetAI(idActorTauntee);
    tAI.idACDTauntTarget = idACDTaunter;
}






void ActorAISetMovementStyle(ActorID idActor, AIStateType eStateType, GBID gbidMovementStyle)
{
    ActorAI * ptAI = ActorTryToGetAI(idActor);
    if (ptAI == NULL) {
        return;
    }
    if (gbidMovementStyle != GBID_NONE) {
        ptAI->gbidMovementStyle = gbidMovementStyle;
    } else {
        ACDID idACD = ACDFindByActor(idActor);
        ActorCommonData * ptACD = ACDTryToGet(idACD);
        if (ptACD) {
            GBID gbidNewStyle = ptAI->ptAIStateInstData ? ptAI->ptAIStateInstData->arAIMovementStyles[eStateType] : GBID_NONE;
            if (eStateType != 0 && gbidNewStyle == GBID_NONE) {
                gbidNewStyle = MonsterGetMovementStyle(ptACD->tSNOName, 0);
            }
            if (gbidNewStyle != GBID_NONE) {
                ptAI->gbidMovementStyle = gbidNewStyle;
            }
        }
    }
}















































BOOL ActorIsUsingAIQueue(ActorID idActor)
{
    return AIQueueGetCount(idActor) > 0;
}




int32 AIQueueGetCount(ActorID idActor)
{
    ActorAI * ptAI = ActorTryToGetAI(idActor);
    if (ptAI == NULL) {
        return 0;
    }
    return ptAI->listAIQueuedIDs.Count();
}






AIQueuedActionID AIQueueAddAction(
    ActorID idActor,
    SNO snoPower,
    const Target & tTarget,
    int32 uData,
    float flData,
    LPCSTR szData,
    AIQueuedActionType eType)
{
    Actor * ptActor = ActorTryToGet(idActor);
    if (!ptActor)
    {
        return AIQID_INVALID;
    }



    ActorAI * ptAI = ActorTryToGetAI(idActor);
    if (!ptAI)
    {
        return AIQID_INVALID;
    }




    if (snoPower == SNO_NONE && eType == QUEUED_ACTION_POWER)
    {
        return AIQID_INVALID;
    }






    ACDID idACD = ACDFindByActor(idActor);
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (!ptACD || !ptACD->IsAlive() || !ptACD->IsEnabled())
    {
        return AIQID_INVALID;
    }




    AIQueuedActionID idQueuedAction = sAllocQueuedAction();
    if (idQueuedAction == AIQID_INVALID)
    {
        return AIQID_INVALID;
    }





    PowerAPI_FreeFromStun(idACD);
    PowerAPI_FreeFromRoot(idACD);
    PowerAPI_FreeFromFear(idACD);
    PowerAPI_FreeFromSnare(idACD);

    AIQueuedPower * ptPowerInfo = sGetQueuedPowerFromID(idQueuedAction);
    if (!ptPowerInfo)
    {
        return AIQID_INVALID;
    }


    if (!ActorIsUsingAIQueue(idActor))
    {

        sForceImmediateAction(*ptActor, 0);
    }

    ptPowerInfo->snoPower = snoPower;
    ptPowerInfo->tTarget = tTarget;
    ptPowerInfo->eType = eType;
    ptPowerInfo->uData = uData;
    ptPowerInfo->flData = flData;
    ptPowerInfo->strData[0] = 0;
    if (szData && strlen(szData) < sizeof(ptPowerInfo->strData))
    {
        Blizzard::String::Copy(ptPowerInfo->strData, szData, sizeof(ptPowerInfo->strData));
    }

    ptAI->listAIQueuedIDs.AddTail_Tracked(idQueuedAction);
    return idQueuedAction;
}




AIQueuedActionID AIQueueAddAnim(ActorID idActor, uint32 uAnimTag, uint32 uTicks, uint32 uNewIdleAnimTag)
{
    Actor * ptActor = ActorTryToGet(idActor);
    if (ptActor == NULL) {
        return AIQID_INVALID;
    }
    ActorAI * ptAI = ActorTryToGetAI(idActor);
    if (ptAI == NULL) {
        return AIQID_INVALID;
    }
    ACDID idACD = ACDFindByActor(idActor);
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD == NULL || !ptACD->IsAlive() || !ptACD->IsEnabled()) {
        return AIQID_INVALID;
    }
    if (!ActorIsUsingAIQueue(idActor)) {
        BrainStanceReset(idActor);
    }
    AIQueuedActionID idQueuedAction = AIQueueAddAction(idActor, SNO_NONE, cgTargetNone, -1, 0.0f, NULL, QUEUED_ACTION_ANIMATION);
    if (idQueuedAction == AIQID_INVALID) {
        return AIQID_INVALID;
    }
    AIQueuedPower * ptPowerInfo = sGetQueuedPowerFromID(idQueuedAction);
    if (ptPowerInfo == NULL) {
        return AIQID_INVALID;
    }
    ptPowerInfo->uAnimTag = uAnimTag;
    ptPowerInfo->uTicks = uTicks;
    ptPowerInfo->uNewIdleAnimTag = uNewIdleAnimTag;
    return idQueuedAction;
}

























AIQueuedActionID AIQueueMoveToLocation(ACDID idACD, const Target & tTarget, ActorAnimationTag eAnimTag, float flSpeed)
{
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD == NULL) {
        return AIQID_INVALID;
    }
    ActorID idActor = (ActorID)ACDGetOwner(idACD);
    Actor * ptActor = ActorTryToGet(idActor);
    if (ptActor == NULL) {
        return AIQID_INVALID;
    }
    AIQueuedActionID idQueuedAction = AIQueueAddAction(idActor, SNO_NONE, tTarget, -1, flSpeed, NULL, QUEUED_ACTION_MOVE_TO_LOCATION);
    if (idQueuedAction == AIQID_INVALID) {
        return AIQID_INVALID;
    }
    AIQueuedPower * ptPowerInfo = sGetQueuedPowerFromID(idQueuedAction);
    if (ptPowerInfo == NULL) {
        return AIQID_INVALID;
    }
    ptPowerInfo->uAnimTag = eAnimTag;
    return idQueuedAction;
}


















static AIQueuedPower * sGetCurrentQueueAction(ActorID idActor)
{
    ActorAI * ptAI = ActorTryToGetAI(idActor);
    if (ptAI == NULL) {
        return NULL;
    }
    AIQueuedActionID idQAction = ptAI->listAIQueuedIDs.GetHead();
    return sGetQueuedPowerFromID(idQAction);
}








void AIQueueActionUpdate(
    ActorID idActor)
{
    ActorAI * ptAI = ActorTryToGetAI(idActor);






    ACDID idACD = ACDFindByActor(idActor);
    ActorCommonData & tACD = ACDGet(idACD);

    if (ACDIsPlayer(idACD))
    {




        AIQueueClearList(idActor);
        return;
    }


    AIQueuedPower * ptQueuedPower = sGetCurrentQueueAction(idActor);
    switch (ptQueuedPower->eType)
    {
    case QUEUED_ACTION_ANIMATION:
    {
        uint32 uLength = ptQueuedPower->uTicks;

        if (uLength == 0)
            uLength = (uint32)-1;



        BrainAPIDoPlayAnimation(idActor, ptQueuedPower->uAnimTag, uLength, FALSE, FALSE, 0.0f, FALSE, 1.0f);

        if (ptQueuedPower->uNewIdleAnimTag != (uint32)-1)
        {
            ConversationMgr::UpdateIdleForActor(idACD, ptQueuedPower->uNewIdleAnimTag);
            BrainStanceSetIdleAnimation((ActorID)ACDGetOwner(idACD), ptQueuedPower->uNewIdleAnimTag);
        }
        return;
    }


    case QUEUED_ACTION_FACE_ACTOR:
    {
        ACDID idACDTarget = ptQueuedPower->tTarget.GetACD();
        ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
        if (ptACDTarget)
        {
            WorldPlace placeTarget = ptACDTarget->GetPlace();
            Target tTarget;
            tTarget.InitializeFromPlace(placeTarget);
            BrainAPIDoLookAt(idActor, tTarget, FALSE);
        }
        AIQueueCompleteCurrentAction(idActor);
        return;
    }


    case QUEUED_ACTION_FACE_SAME_AS_ACTOR:
    {
        ACDID idACDTarget = ptQueuedPower->tTarget.GetACD();
        ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
        if (ptACDTarget)
        {
            WorldPlace placeTarget = ptACDTarget->GetPlace();
            Angle aYaw = ACDGetYaw(idACDTarget);
            Vector3D vFacing(Math::Cos(aYaw), Math::Sin(aYaw), 0.0f);
            placeTarget.wp += vFacing;

            Target tTarget;
            tTarget.InitializeFromPlace(placeTarget);
            BrainAPIDoLookAt(idActor, tTarget, FALSE);
        }
        AIQueueCompleteCurrentAction(idActor);
        return;
    }

    case QUEUED_ACTION_DELETE:

        ActorFlagForDeletion(idActor);
        return;

    case QUEUED_ACTION_KILL:
    {
        ActorCommonData * ptACD = ACDTryToGet(idACD);
        if (ptACD)
        {
            if (ptACD->IsAlive())
            {
                DeathType eDeathType = ptQueuedPower->uData;
                BOOL bCrit = ptQueuedPower->flData > 0.000001f;
                BrainAPIDoScriptedDeath(*ptACD, eDeathType, bCrit, FALSE, ACTORID_INVALID);
            }
        }
        return;
    }

    case QUEUED_ACTION_WALK_FORWARD:
    {
        ptQueuedPower->snoPower = GlobalSNOGet(GLOBALSNO_POWERS_AI_WALK_TO_GUARANTEED);
        float flDistance; memcpy(&flDistance, &ptQueuedPower->uData, sizeof(flDistance));
        WorldPlace placeCurr = tACD.GetPlace();
        Angle aYaw = ACDGetYaw(idACD);

        WorldPlace placeDest;
        placeDest.wp.x = Math::Cos(aYaw);
        placeDest.wp.y = Math::Sin(aYaw);
        placeDest.wp.z = 0.0f;
        placeDest.idSWorld = placeCurr.idSWorld;

        placeDest.wp *= flDistance;
        placeDest.wp += placeCurr.wp;

        ptQueuedPower->tTarget.InitializeFromPlace(placeDest);
        ptQueuedPower->eType = QUEUED_ACTION_POWER;
        break;
    }

    case QUEUED_ACTION_SEND_EVENT:
    {
        ACDID idACDTarget = ptQueuedPower->tTarget.GetACD();
        if (ptQueuedPower->flData > 0.0f)
        {
            ScriptEventOnUserEventInRange(ptQueuedPower->strData, idACD, ptQueuedPower->flData, FALSE);
        }
        else if (idACDTarget != ACDID_INVALID)
        {
            ScriptEventOnUserEventSentToActor(ptQueuedPower->strData, idACD, idACDTarget, FALSE);
        }
        else
        {
            ScriptEventOnUserEvent(ptQueuedPower->strData, idACD, FALSE);
        }

        AIQueueCompleteCurrentAction(idActor);
        return;
    }





    case QUEUED_ACTION_WAIT_FOR_CONVERSATION:

        if (!ConversationMgr::ConversationIsActive(ptQueuedPower->uData))
            AIQueueCompleteCurrentAction(idActor);
        return;

    default:
        break;
    }





    BrainAttackSettings tAttackSettings;
    BrainAPIGetAttackSettings(idActor, tAttackSettings);
    tAttackSettings.snoPower = ptQueuedPower->snoPower;
    tAttackSettings.tTarget = ptQueuedPower->tTarget;

    if (ptQueuedPower->eType == QUEUED_ACTION_MOVE_TO_LOCATION)
    {
        tAttackSettings.snoPower = GlobalSNOGet(GLOBALSNO_POWERS_AI_WALK_TO_GUARANTEED);
        tAttackSettings.eAnimTag = ptQueuedPower->uAnimTag;
        tAttackSettings.flSpeed = ptQueuedPower->flData;
    }




    if (tAttackSettings.tTarget.IsInvalidForSource(&tACD)) AIQueueCompleteCurrentAction(idActor);
    else
        BrainAPIDoAttack(idActor, tAttackSettings);
}









void AIQueueClearList(ActorID idActor)
{
    Actor * ptActor = ActorGet(idActor);
    if (ptActor->ptAI == NULL) {
        return;
    }

    POS pos = ptActor->ptAI->listAIQueuedIDs.GetHeadPosition();
    while (pos) {
        AIQueuedActionID idQActionID = ptActor->ptAI->listAIQueuedIDs.GetNext(pos);
        sFreeQueuedAction(idQActionID);
    }

    ptActor->ptAI->listAIQueuedIDs.RemoveAll();
}






void AIQueueCheckActionCompleted(ActorID idActor, SNO snoPower, AIQueuedActionType eType)
{
    if (!ActorIsUsingAIQueue(idActor)) {
        return;
    }
    Actor * ptActor = ActorGet(idActor);
    if (ptActor->ptAI == NULL) {
        return;
    }
    AIQueuedPower * ptQueuedPower = sGetCurrentQueueAction(idActor);
    if (ptQueuedPower && ptQueuedPower->snoPower == snoPower && ptQueuedPower->eType == eType) {
        AIQueueCompleteCurrentAction(idActor);
    }
}















void AIQueueCompleteCurrentAction(
    ActorID idActor)
{
    if (!ActorIsUsingAIQueue(idActor))
    {
        return;
    }

    Actor * ptActor = ActorGet(idActor);
    if (ptActor->ptAI)
    {
        SNO snoPrimaryPower = ActorPowersGetPrimaryPower(idActor);
        ActorCommonData * ptACD = ACDTryToGet(ptActor->idACD);
        if (snoPrimaryPower != SNO_NONE && ptACD &&

            PowerHasInfiniteAnimationTiming(*ptACD, AutoPowerDefinitionR(snoPrimaryPower)))
        {
            ActorPowersInterruptPrimary(idActor, TRUE);
        }

        AIQueuedActionID idQActionID = ptActor->ptAI->listAIQueuedIDs.GetHead();

        AIQueuedPower * ptQueuedPower = sGetQueuedPowerFromID(idQActionID);

        if (ptQueuedPower->eType == QUEUED_ACTION_ANIMATION)
        {
            BrainStanceReset(idActor);
            sForceImmediateAction(*ptActor, 0);
        }

        if (ptActor->ptSActorMovement)
        {
            ptActor->ptSActorMovement->StopSoon();
        }


        ScriptEventOnAIQueuedActionComplete(ptActor->idACD, idQActionID);

        sFreeQueuedAction(idQActionID);

        ptActor->ptAI->listAIQueuedIDs.RemoveHead();

        if (ptActor->ptAI->listAIQueuedIDs.Count() <= 0)
        {
            ScriptEventOnAIQueueEmpty(ptActor->idACD);



            if (!ActorIsQuestLifespanValid(*ptActor))
            {
                ActorDeactivateFromQuestRange(idActor);
            }
        }


    }
}



















































































































int32 AIQueueCalloutGetCount(
    ActorID idActor)
{
    ActorAI * ptAI = ActorTryToGetAI(idActor);
    if (!ptAI)
    {
        return 0;
    }

    return ptAI->listAIQueuedCalloutIDs.Count();
}



BOOL sPlayCallout(
    ActorCommonData & tACD,
    Player * ptPlayer,
    SNO snoConversation,
    int32 nPriority,
    float flCooldown,
    float flGlobalCooldown)
{
    ACDID idACDNPC = tACD.id;
    if (idACDNPC == ptPlayer->idPlayerACD)
    {
        idACDNPC = ACDID_INVALID;
    }

    BOOL bSuccess = ConversationMgr::StartConversation(
        ptPlayer->nPlayerIndex, snoConversation, idACDNPC,
        ACDID_INVALID, ACDID_INVALID, ACDID_INVALID, ACDID_INVALID,
        nPriority, FALSE, FALSE);






    if (bSuccess)
    {
        AISetCalloutCooldown(tACD, snoConversation, flCooldown, flGlobalCooldown);
    }

    return bSuccess;
}



void AISetCalloutCooldown(
    ActorCommonData & tACD,
    SNO snoConversation,
    float flCooldown,
    float flGlobalCooldown)
{
    if (flCooldown > 0.0f || flGlobalCooldown > 0.0f)
    {
        uint32 dwCooldownTicks = Math::FloatToInt(flCooldown * TICKS_PER_SECOND);
        uint32 dwGlobalCooldownTicks = Math::FloatToInt(flGlobalCooldown * TICKS_PER_SECOND);
        uint32 dwDurationMin = Math::Max(dwCooldownTicks, dwGlobalCooldownTicks);

        uint32 dwNow = GameGetServerTime();
        const int32 nMaxCooldownStackCount = 1024;

        PowerAPIBuffStartData tBuffStartData(POWERID_INVALID);
        tBuffStartData.idACDSource = tACD.id;
        tBuffStartData.snoPower = GlobalSNOGet(GLOBALSNO_POWERS_CALLOUT_COOLDOWN);
        tBuffStartData.dwDurationMin = dwDurationMin;
        tBuffStartData.bRemoveOnDeath = false;
        tBuffStartData.eStackType = BUFF_STACKTYPE_ALWAYS;
        tBuffStartData.nMaxStackCount = nMaxCooldownStackCount;

        tBuffStartData.bUndispellable = true;
        FastAttribSetValueInt(tBuffStartData.idFastAttrib, MakeAttribKey(ATTRIBUTE_CALLOUT_COOLDOWN, SNO_NONE), dwNow + dwGlobalCooldownTicks);

        if (snoConversation != SNO_NONE)
        {
            FastAttribSetValueInt(tBuffStartData.idFastAttrib, MakeAttribKey(ATTRIBUTE_CALLOUT_COOLDOWN, snoConversation), dwNow + dwCooldownTicks);
        }

        PowerAPI_BuffAddToACD(tBuffStartData, tACD);
    }
}



void AIClearCalloutCooldown(ActorCommonData & tACD, SNO snoConversation)
{
    if (snoConversation == SNO_NONE) {
        return;
    }
    SNO snoPowerCalloutCooldown = GlobalSNOGet(GLOBALSNO_POWERS_CALLOUT_COOLDOWN);
    uint32 dwNow = GameGetServerTime();
    POS pos = tACD.listBuffs.GetHeadPosition();
    while (pos) {
        BuffID idBuff = tACD.listBuffs.GetNext(pos);
        PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
        if (ptBuff && !ptBuff->bDeleted && ptBuff->snoPower == snoPowerCalloutCooldown) {
            uint32 dwCooldownEndTick = FastAttribGetValueInt(ptBuff->idFastAttrib, MakeAttribKey(ATTRIBUTE_CALLOUT_COOLDOWN, snoConversation));
            if (dwCooldownEndTick != (uint32)-1) {
                uint32 dwGCDEndTick = FastAttribGetValueInt(ptBuff->idFastAttrib, MakeAttribKey(ATTRIBUTE_CALLOUT_COOLDOWN, SNO_NONE));
                if (dwGCDEndTick == (uint32)-1 || dwGCDEndTick <= dwNow) {
                    PowerAPI_BuffFlagForDeletion(ptBuff, FALSE, TRUE, 0, TRUE);
                } else {
                    FastAttribSetValueInt(ptBuff->idFastAttrib, MakeAttribKey(ATTRIBUTE_CALLOUT_COOLDOWN, snoConversation), 0);
                }
            }
        }
    }
}
















void AISetBanterCooldown(
    ActorCommonData & tACD,
    SNO snoConversation,
    float flCooldown,
    float flGlobalCooldown)
{
    if (flCooldown > 0.0f || flGlobalCooldown > 0.0f)
    {
        uint32 dwCooldownTicks = Math::FloatToInt(flCooldown * TICKS_PER_SECOND);
        uint32 dwGlobalCooldownTicks = Math::FloatToInt(flGlobalCooldown * TICKS_PER_SECOND);
        uint32 dwDurationMin = Math::Max(dwCooldownTicks, dwGlobalCooldownTicks);

        uint32 dwNow = GameGetServerTime();
        const int32 nMaxCooldownStackCount = 1024;

        PowerAPIBuffStartData tBuffStartData(POWERID_INVALID);
        tBuffStartData.idACDSource = tACD.id;
        tBuffStartData.snoPower = GlobalSNOGet(GLOBALSNO_POWERS_BANTER_COOLDOWN);
        tBuffStartData.dwDurationMin = dwDurationMin;
        tBuffStartData.bRemoveOnDeath = false;
        tBuffStartData.eStackType = BUFF_STACKTYPE_ALWAYS;
        tBuffStartData.nMaxStackCount = nMaxCooldownStackCount;

        tBuffStartData.bUndispellable = true;
        FastAttribSetValueInt(tBuffStartData.idFastAttrib, MakeAttribKey(ATTRIBUTE_BANTER_COOLDOWN, SNO_NONE), dwNow + dwGlobalCooldownTicks);

        if (snoConversation != SNO_NONE)
        {
            FastAttribSetValueInt(tBuffStartData.idFastAttrib, MakeAttribKey(ATTRIBUTE_BANTER_COOLDOWN, snoConversation), dwNow + dwCooldownTicks);
        }

        PowerAPI_BuffAddToACD(tBuffStartData, tACD);
    }
}


// NON_MATCHING: 1072 vs 1072 bytes, 115 words differ; 3055 now tests the cooldown first (columns 3055:20 != / 36 && / 45 <, flDelay after); the original computes both cooldown compares as values (cset/cset/and) before the flDelay branch and reuses the != at 3147; ours still branches. Tried flDelay-first, `&` forms, empty-then/else.
void AIQueueAddCallout(
    ActorID idActor,
    ACDID idACDPlayer,
    SNO snoConversation,
    int32 nPriority,
    float flDelay,
    float flMaxQueuedTime,
    float flCooldown,
    float flGlobalCooldown,
    uint32 dwFlags)
{

    Actor * ptActor = ActorTryToGet(idActor);
    if (!ptActor)
    {
        return;
    }


    ActorAI * ptAI = ActorTryToGetAI(idActor);
    if (!ptAI || snoConversation == SNO_NONE)
    {
        return;
    }











    if (ptAI->dwStatusFlags & (1 << ACTOR_AI_DISABLE_UPDATES_BIT))
    {
        return;
    }


    Player * ptPlayer = PlayerGetByACD(idACDPlayer);
    if (!ptPlayer)
    {
        return;
    }


    BOOL bQueueCallout = TRUE;









    uint32 dwNow = GameGetServerTime();
    ActorCommonData & tACD = ACDGet(ptActor->idACD);
    int32 nCoolDown = tACD.AttributesGetInt(MakeAttribKey(ATTRIBUTE_CALLOUT_COOLDOWN, snoConversation));
    int32 nGlobalCoolDown = 0;
    if (!(dwFlags & 1))
    {
        nGlobalCoolDown = tACD.AttributesGetInt(MakeAttribKey(ATTRIBUTE_CALLOUT_COOLDOWN, SNO_NONE));
    }

    uint32 dwCoolDown = Math::Max(nCoolDown, nGlobalCoolDown);
    if (!((dwCoolDown != (uint32)-1 && dwNow < dwCoolDown) || flDelay > 0.0f))
    {





























        bQueueCallout = !sPlayCallout(tACD, ptPlayer, snoConversation, nPriority, flCooldown, flGlobalCooldown);
    }






















    uint32 dwExpireTime = dwNow + Math::FloatToInt(Math::Max(flMaxQueuedTime, flDelay) * TICKS_PER_SECOND);
    POS pos = ptAI->listAIQueuedCalloutIDs.GetHeadPosition();
    while (pos)
    {
        POS posCurrent = pos;
        AIQueuedCalloutID idQCalloutID = ptAI->listAIQueuedCalloutIDs.GetNext(pos);
        AIQueuedCallout * ptCalloutInfo = sGetQueuedCalloutFromID(idQCalloutID);

        if (ptCalloutInfo->snoConversation == snoConversation)
        {
            if (bQueueCallout)
            {

                ptCalloutInfo->dwExpireTime = dwExpireTime;
            }









            else
            {

                ptAI->listAIQueuedCalloutIDs.RemoveAt(posCurrent);
            }

            return;
        }
    }




    if (!(dwCoolDown != (uint32)-1 && dwExpireTime < dwCoolDown) && dwExpireTime && bQueueCallout)
    {











        AIQueuedCalloutID idQueuedCallout = sAllocQueuedCallout();
        if (idQueuedCallout == AIQCID_INVALID)
        {
            return;
        }




        AIQueuedCallout * ptCalloutInfo = sGetQueuedCalloutFromID(idQueuedCallout);
        if (!ptCalloutInfo)
        {
            return;
        }


        ptCalloutInfo->idACDPlayer = idACDPlayer;
        ptCalloutInfo->snoConversation = snoConversation;
        ptCalloutInfo->nPriority = nPriority;
        ptCalloutInfo->dwDelayedTime = dwNow + Math::FloatToInt(flDelay * TICKS_PER_SECOND);
        ptCalloutInfo->dwExpireTime = dwExpireTime;
        ptCalloutInfo->flCooldown = flCooldown;
        ptCalloutInfo->flGlobalCooldown = flGlobalCooldown;
        ptCalloutInfo->dwFlags = dwFlags;

        ptAI->listAIQueuedCalloutIDs.AddTail_Tracked(idQueuedCallout);











    }
}



void AIQueueCalloutUpdate(
    ActorID idActor)
{
    ActorAI * ptAI = ActorTryToGetAI(idActor);



    if (AIQueueCalloutGetCount(idActor) == 0)
    {
        return;
    }

    ACDID idACD = ACDFindByActor(idActor);
    ActorCommonData & tACD = ACDGet(idACD);
    uint32 dwNow = GameGetServerTime();


    AIQueuedCalloutID idBestCalloutID = AIQCID_INVALID;
    int32 nHighestPriority = -1;

    POS pos = ptAI->listAIQueuedCalloutIDs.GetHeadPosition();
    while (pos)
    {
        AIQueuedCalloutID idQCalloutID = ptAI->listAIQueuedCalloutIDs.GetNext(pos);
        AIQueuedCallout * ptCalloutInfo = sGetQueuedCalloutFromID(idQCalloutID);

        if (dwNow < ptCalloutInfo->dwDelayedTime)
        {
            continue;
        }

        if (ptCalloutInfo->nPriority <= nHighestPriority)
        {
            continue;
        }


        if (!(ptCalloutInfo->dwFlags & 1))
        {
            uint32 dwGlobalCoolDown = tACD.AttributesGetInt(MakeAttribKey(ATTRIBUTE_CALLOUT_COOLDOWN, SNO_NONE));
            if (dwGlobalCoolDown != (uint32)-1 && dwNow < dwGlobalCoolDown)
            {
                continue;
            }
        }


        uint32 dwCoolDown = tACD.AttributesGetInt(MakeAttribKey(ATTRIBUTE_CALLOUT_COOLDOWN, ptCalloutInfo->snoConversation));
        if (dwCoolDown != (uint32)-1 && dwNow < dwCoolDown)
        {
            continue;
        }

        idBestCalloutID = idQCalloutID;
        nHighestPriority = ptCalloutInfo->nPriority;
    }

    if (idBestCalloutID != AIQCID_INVALID)
    {

        AIQueuedCallout * ptCalloutInfo = sGetQueuedCalloutFromID(idBestCalloutID);
        Player * ptPlayer = PlayerGetByACD(ptCalloutInfo->idACDPlayer);
        if (ptPlayer)
        {

            if (sPlayCallout(tACD, ptPlayer, ptCalloutInfo->snoConversation, ptCalloutInfo->nPriority, ptCalloutInfo->flCooldown, ptCalloutInfo->flGlobalCooldown))
            {

                ptCalloutInfo->dwExpireTime = dwNow;
            }

            AppDrawFlagIsSet(APP_DRAW_DEBUG_CALLOUTS_BIT);


























        }
    }


    pos = ptAI->listAIQueuedCalloutIDs.GetHeadPosition();
    while (pos)
    {
        POS posCurrent = pos;
        AIQueuedCalloutID idQCalloutID = ptAI->listAIQueuedCalloutIDs.GetNext(pos);
        AIQueuedCallout * ptCalloutInfo = sGetQueuedCalloutFromID(idQCalloutID);

        if (ptCalloutInfo->dwExpireTime <= dwNow)
        {



            sFreeQueuedCallout(idQCalloutID);
            ptAI->listAIQueuedCalloutIDs.RemoveAt(posCurrent);
        }
    }
}


void AIQueueCalloutClearList(ActorID idActor)
{
    Actor * ptActor = ActorGet(idActor);
    if (ptActor->ptAI == NULL) {
        return;
    }

    POS pos = ptActor->ptAI->listAIQueuedCalloutIDs.GetHeadPosition();
    while (pos) {
        AIQueuedCalloutID idQCalloutID = ptActor->ptAI->listAIQueuedCalloutIDs.GetNext(pos);
        sFreeQueuedCallout(idQCalloutID);
    }

    ptActor->ptAI->listAIQueuedCalloutIDs.RemoveAll();
}






BOOL AISetNodePath(ActorID idActor, PathID idPath, PathNodeID idPathNodeDest)
{
    ActorAI * ptAI = ActorTryToGetAI(idActor);
    if (ptAI == NULL) {
        return FALSE;
    }
    if (ptAI->ptNodePathData == NULL) {
        ActorAIGlobals * ptActorAIGlobals = SGameManager::GameGet().ptActorAIGlobals;
        ptAI->ptNodePathData = ptActorAIGlobals->poolNodePathData.Alloc_Tracked();
        if (ptAI->ptNodePathData == NULL) {
            return FALSE;
        }
        SigmaMemoryClear(ptAI->ptNodePathData, sizeof(AINodePathData));
        ptAI->ptNodePathData->idPath = PATHID_INVALID;
        ptAI->ptNodePathData->idPathNodeCurr = PATHNODEID_INVALID;
        ptAI->ptNodePathData->idPathNodeDest = PATHNODEID_INVALID;
    }
    if (idPath == PATHID_INVALID) {
        return FALSE;
    }
    ACDID idACD = ACDFindByActor(idActor);
    if (idPathNodeDest == PATHNODEID_INVALID) {
        return FALSE;
    }
    if (idACD == ACDID_INVALID) {
        return FALSE;
    }
    if (!NodePathIsOnPath(idPath, idPathNodeDest)) {
        return FALSE;
    }
    if (ptAI->ptNodePathData->idPath != idPath || ptAI->ptNodePathData->idPathNodeDest != idPathNodeDest) {
        ptAI->ptNodePathData->idPath = idPath;
        ptAI->ptNodePathData->idPathNodeCurr = PATHNODEID_INVALID;
        ptAI->ptNodePathData->idPathNodeDest = idPathNodeDest;
    }
    return TRUE;
}






















BOOL AIHasNodePath(ActorID idActor)
{
    ActorAI * ptAI = ActorTryToGetAI(idActor);
    if (ptAI && ptAI->ptNodePathData && ptAI->ptNodePathData->idPath != PATHID_INVALID && ptAI->ptNodePathData->idPathNodeDest != PATHNODEID_INVALID) {
        return TRUE;
    }
    return FALSE;
}














BOOL AIWalkNodePath(ActorID idActor, WorldPlace & placeGotoPoint)
{
    if (!AIHasNodePath(idActor))
        return FALSE;






    ACDID idACD = ACDFindByActor(idActor);
    ActorCommonData & tACD = ACDGet(idACD);
    if (!tACD.IsAlive())
        return FALSE;





    ActorAI * ptAI = ActorTryToGetAI(idActor);


    AINodePathData * ptPathData = ptAI->ptNodePathData;


    WorldPlace tPlace = tACD.GetPlace();
    if (ptPathData->idPathNodeCurr == PATHNODEID_INVALID)
    {
        Vector3D wpPosClosest;
        const NodePathNode * pDestNode = NodePathGetNode(ptPathData->idPath, ptPathData->idPathNodeDest);
        ptPathData->idPathNodeCurr = NodePathGetNextNodeAndPosition(ptPathData->idPath, tPlace.wp, pDestNode, wpPosClosest);
        if (ptPathData->idPathNodeCurr == PATHNODEID_INVALID)
            return FALSE;
    }









    NodePathNode * pCurrNode = NodePathGetNode(ptPathData->idPath, ptPathData->idPathNodeCurr);


    float flDistSqr = Vector3D::DistanceSquared(tPlace.wp, pCurrNode->wpPos);
    if (flDistSqr <= pCurrNode->flRadius * pCurrNode->flRadius)
    {
        CRefString strName(MarkerNameGetString(pCurrNode->tName));
        ScriptEventOnActorReachedPathNode(idACD, strName.str());
        if (ptPathData->idPathNodeCurr == ptPathData->idPathNodeDest)
        {
            ptPathData->idPath = PATHID_INVALID;
            return FALSE;
        }
        ptPathData->idPathNodeCurr = NodePathGetNextClosestNode(ptPathData->idPath, ptPathData->idPathNodeCurr, ptPathData->idPathNodeDest);
        pCurrNode = NodePathGetNode(ptPathData->idPath, ptPathData->idPathNodeCurr);
    }

    if (pCurrNode)
    {
        placeGotoPoint.idSWorld = tPlace.idSWorld;
        placeGotoPoint.wp = pCurrNode->wpPos;
        return TRUE;
    }
    return FALSE;
}











































































BOOL AIIsTargetOnPath(ActorID idActor, const WorldPlace & placeTarget, float flBonusRadius)
{
    if (!AIHasNodePath(idActor))
        return FALSE;







    ActorAI * ptAI = ActorTryToGetAI(idActor);
    if (ptAI)
    {

        AINodePathData * ptPathData = ptAI->ptNodePathData;
        if (ptPathData->idPathNodeCurr == PATHNODEID_INVALID)
            return FALSE;







        NodePathNode * ptNodeCurr = NodePathGetNode(ptPathData->idPath, ptPathData->idPathNodeCurr);
        PathNodeID idPrevNode = NodePathGetPrevNode(ptPathData->idPath, ptPathData->idPathNodeCurr, ptPathData->idPathNodeDest);
        NodePathNode * ptNodePrev = NodePathGetNode(ptPathData->idPath, idPrevNode);
        if (ptNodeCurr == NULL || ptNodePrev == NULL)
            return FALSE;




        float flPrevRadius = ptNodePrev->flRadius + flBonusRadius;
        float flCurrRadius = ptNodeCurr->flRadius + flBonusRadius;

        Vector3D vToTarg = ptNodeCurr->wpPos - ptNodePrev->wpPos;
        if (vToTarg.MagnitudeSquared() >= 1e-12f)
        {
            float flLength = vToTarg.Normalize();

            Vector3D vToAI = placeTarget.wp - ptNodePrev->wpPos;
            float flProjLength = Vector3D::DotProduct(vToTarg, vToAI);
            if (flProjLength < -flPrevRadius || flProjLength > flCurrRadius + flLength)
                return FALSE;








            float flUnitLength = flProjLength / flLength;
            if (flUnitLength < 0.0f)
            {
                if (vToAI.MagnitudeSquared() >= flPrevRadius * flPrevRadius)
                    return FALSE;
            }
            else if (flUnitLength > 1.0f)
            {

                Vector3D vCurrToAI = placeTarget.wp - ptNodeCurr->wpPos;
                if (vCurrToAI.MagnitudeSquared() >= flCurrRadius * flCurrRadius)
                    return FALSE;
            }
            else
            {




                Vector3D vProjPoint = vToTarg * flProjLength;
                Vector3D vProjPointToAI = vToAI - vProjPoint;
                float flPathRadiusAtProjPoint = flPrevRadius + (flCurrRadius - flPrevRadius) * flUnitLength;
                if (vProjPointToAI.MagnitudeSquared() >= flPathRadiusAtProjPoint * flPathRadiusAtProjPoint)
                    return FALSE;
            }
        }
    }
    return TRUE;
}











































































BOOL AITargetIsAheadOnPath(ActorID idActor, const WorldPlace & placeTarget, float flDist)
{
    if (!AIHasNodePath(idActor))
        return FALSE;








    ACDID idACD = ACDFindByActor(idActor);
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD == NULL)
        return FALSE;




    ActorAI * ptAI = ActorTryToGetAI(idActor);
    if (ptAI && ptAI->ptNodePathData)
    {
        AINodePathData * ptPathData = ptAI->ptNodePathData;
        if (ptPathData->idPathNodeCurr == PATHNODEID_INVALID)
            return FALSE;





        NodePathNode * ptNodeCurr = NodePathGetNode(ptPathData->idPath, ptPathData->idPathNodeCurr);
        PathNodeID idPrevNode = NodePathGetPrevNode(ptPathData->idPath, ptPathData->idPathNodeCurr, ptPathData->idPathNodeDest);
        NodePathNode * ptNodePrev = NodePathGetNode(ptPathData->idPath, idPrevNode);
        if (ptNodeCurr == NULL || ptNodePrev == NULL)
            return FALSE;




        WorldPlace placeACD = ptACD->GetPlace();
        Vector2D vToNextNode(ptNodeCurr->wpPos.x - ptNodePrev->wpPos.x, ptNodeCurr->wpPos.y - ptNodePrev->wpPos.y);
        if (vToNextNode.MagnitudeSquared() >= 1e-12f)
        {
            vToNextNode.Normalize();
            Vector2D vToTarget(placeTarget.wp.x - ptNodePrev->wpPos.x, placeTarget.wp.y - ptNodePrev->wpPos.y);
            float flProjLengthTarget = Vector2D::DotProduct(vToNextNode, vToTarget);
            Vector2D vToAI(placeACD.wp.x - ptNodePrev->wpPos.x, placeACD.wp.y - ptNodePrev->wpPos.y);
            float flProjLengthAI = Vector2D::DotProduct(vToNextNode, vToAI);
            if (flProjLengthTarget >= flProjLengthAI + flDist)
                return TRUE;
        }
    }
    return FALSE;
}



BOOL AIIsDerailedFromPath(ActorID idActor, float flBonusRadius)
{
    if (AIHasNodePath(idActor)) {
        ActorAI * ptAI = ActorTryToGetAI(idActor);
        if (ptAI) {
            ACDID idACD = ACDFindByActor(idActor);
            ActorCommonData & tACD = ACDGet(idACD);
            WorldPlace tPlace = tACD.GetPlace();
            AINodePathData * ptPathData = ptAI->ptNodePathData;
            if (ptPathData->idPathNodeCurr == PATHNODEID_INVALID) {
                return TRUE;
            }
            if (!AIIsTargetOnPath(idActor, tPlace, flBonusRadius)) {
            }
        }
    }
    return FALSE;
}


















































































void AISetTether(ActorID idActor, TetherType eTetherType, float flRadius, const WorldPlace & placeTetherPoint)
{
    ActorAI * ptAI = ActorTryToGetAI(idActor);
    if (ptAI) {
        ptAI->pBrainTetherUpdateFunc = TetherBrainUpdate;
        TetherData & tTetherData = ptAI->tAITetherData;
        tTetherData.eTetherType = eTetherType;
        tTetherData.flRadius = flRadius;
        tTetherData.placeTetherPoint = placeTetherPoint;
    }
}









void AIClearTether(ActorID idActor)
{
    ActorAI * ptAI = ActorTryToGetAI(idActor);
    if (ptAI) {
        ptAI->pBrainTetherUpdateFunc = NULL;
        ptAI->tAITetherData.flRadius = 0.0f;
        AIStopTetherRetreat(idActor, *ptAI);
    }
}










void AIStopTetherRetreat(ActorID idActor, ActorAI & tAI)
{
    TetherData & tTetherData = tAI.tAITetherData;
    tTetherData.bRetreating = FALSE;
    ACDID idACD = ACDFindByActor(idActor);
    ActorCommonData & tACD = ACDGet(idACD);
    SNO snoBuff = GlobalSNOGet(GLOBALSNO_POWERS_AI_EVADE_BUFF);
    BuffID idBuff = PowerAPI_BuffFindOnACD(snoBuff, tACD, ACDID_INVALID);
    PowerAPI_BuffFlagForDeletion(idBuff, FALSE, TRUE, 0, TRUE);
    ScopedActorDefinitionLockR ptActorDef(tACD.snoActor);
    tACD.tCollSettings.uDynamicFlagsNoOverlap = ptActorDef->tCollData.tCollFlags.uDynamicFlagsNoOverlap;
    tACD.ModifiedMark(15);
}










enum InactiveSaveIndex {
    ISI_PERSISTENT_FLAGS = 0,
    ISI_SUMMONED_BY_ACDID = 1,
    ISI_SUMMONER_ID = 2,
    ISI_SUMMON_EXPIRATION_TICK = 3,
    ISI_SUMMON_COUNT = 4,
    ISI_BEHAVIOR_TYPE = 5,
    ISI_AISTATE_GBID_START = 6,
    ISI_AISTATE_GBID_LAST = 13,
    ISI_CURRENT_PATH = 14,
    ISI_PATH_DESTINATION_NODE = 15,
    ISI_GUARD_OBJECT_ACDID = 16,
    ISI_FORCED_ENEMY_ACDID = 17,
    ISI_FOLLOW_TARGET_ACDID = 18,
    ISI_FOLLOW_TARGET_TYPE = 19,
    ISI_SPAWNER_CONCURRENT_COUNT_ID = 20,
    ISI_TETHER_LOCATION_X = 21,
    ISI_TETHER_LOCATION_Y = 22,
    ISI_TETHER_LOCATION_Z = 23,
    ISI_POWER_SAVED_ATTRIBUTE = 24,
    ISI_EXPENSIVE_PROC_COUNT = 25,
    ISI_PING = 26,
    ISI_ELITE_ENGAGED = 27,
    ISI_SPECIAL_INVENTORY_HAS_SOLD = 28,
    ISI_INVENTORY_REROLL_TIME = 29,
    ISI_COUNT = 30,
};








inline AIInactiveSave::AIInactiveSave(ActorSavedAI * pActorSavedAI)
{
    m_pActorSavedAI = pActorSavedAI;
    m_pidCurrBlock = &pActorSavedAI->idFirstBlock;
    m_nCurrDataIndex = 0;
    m_nCurrRestoreIndex = 0;
}




























AIInactiveSaveBlockID AIInactiveSave::AllocNewBlock()
{
    ActorAIGlobals * ptActorAIGlobals = SGameManager::GameGet().ptActorAIGlobals;
    AIInactiveSaveBlockID idBlock = ptActorAIGlobals->ptInactiveSaveBlockArray->ElementNew();
    GetBlockFromID(idBlock).idNextBlock = AISAVEID_INVALID;
    return idBlock;
}











AIInactiveSaveBlock & AIInactiveSave::GetBlockFromID(AIInactiveSaveBlockID idBlock)
{
    ActorAIGlobals * ptActorAIGlobals = SGameManager::GameGet().ptActorAIGlobals;
    return *ptActorAIGlobals->ptInactiveSaveBlockArray->ElementGet(idBlock);
}










void AIInactiveSave::IncData()
{
    m_nCurrDataIndex++;
    if (m_nCurrDataIndex >= INACTIVE_SAVE_BLOCK_NUM_DWORDS) {
        m_pidCurrBlock = &GetBlockFromID(*m_pidCurrBlock).idNextBlock;
        m_nCurrDataIndex = 0;
    }
}









void AIInactiveSave::FreeAllBlocks()
{
    ActorAIGlobals * ptActorAIGlobals = SGameManager::GameGet().ptActorAIGlobals;
    m_pidCurrBlock = &m_pActorSavedAI->idFirstBlock;
    while (*m_pidCurrBlock != AISAVEID_INVALID) {
        AIInactiveSaveBlockID idLastBlock = *m_pidCurrBlock;
        AIInactiveSaveBlock & tBlock = GetBlockFromID(idLastBlock);
        *m_pidCurrBlock = tBlock.idNextBlock;
        ptActorAIGlobals->ptInactiveSaveBlockArray->ElementDeleteByID(idLastBlock);
    }
    m_pActorSavedAI->idFirstBlock = AISAVEID_INVALID;
}











void AIInactiveSave::SaveDWORD(int32 nIndex, uint32 dwData)
{
    uint32 dwFlagIndex = 1 << nIndex;
    m_pActorSavedAI->dwIndexUsedFlags |= dwFlagIndex;

    if (*m_pidCurrBlock == AISAVEID_INVALID) {
        *m_pidCurrBlock = AllocNewBlock();
        m_nCurrDataIndex = 0;
    }

    AIInactiveSaveBlock & tBlock = GetBlockFromID(*m_pidCurrBlock);
    tBlock.ardwData[m_nCurrDataIndex] = dwData;

    IncData();
}

















int32 AIInactiveSave::RestoreDWORD(
    uint32 & dwData,
    ActorID idActor)
{




    int32 nOrigRestoreIndex = m_nCurrRestoreIndex;


    for (; m_nCurrRestoreIndex < ISI_COUNT; ++m_nCurrRestoreIndex)
    {
        if (m_pActorSavedAI->dwIndexUsedFlags & (1 << m_nCurrRestoreIndex))
        {
            break;
        }
    }

    int32 nIndexProcessed = m_nCurrRestoreIndex;







    if (m_nCurrRestoreIndex < ISI_COUNT)
    {

        AIInactiveSaveBlock & tBlock = GetBlockFromID(*m_pidCurrBlock);


        dwData = tBlock.ardwData[m_nCurrDataIndex];

        IncData();
        ++m_nCurrRestoreIndex;
    }

    return nIndexProcessed;
}
#include "../../Common/Definitions/MonsterDefinition.h"
#include "../Definitions/AIDefinition.h"
AIBehaviorType AIStateGetBehaviorType(ActorID idActor); SNO AIStateGetSNO(ActorID idActor, AIStateType eStateType); // DWARF AIStates.cpp:6072/4685; header placement inferred.
void ActorAISaveForDeactivatingActor(
    ActorID idActor,
    ActorSavedAI & tActorSavedAI)
{
    SigmaMemoryClear(&tActorSavedAI, sizeof(tActorSavedAI));
    tActorSavedAI.idFirstBlock = AISAVEID_INVALID;


    ActorAI * ptActorAI = ActorTryToGetAI(idActor);
    if (ptActorAI)
    {
        ACDID idACD = ACDFindByActor(idActor);
        ActorCommonData & tACD = ACDGet(idACD);

        AIInactiveSave InactiveSave(&tActorSavedAI);

        if (tACD.AttributesGetInt(ATTRIBUTE_NPC_IS_OPERATABLE)) ptActorAI->dwPersistentFlags |= 1 << 4; else ptActorAI->dwPersistentFlags &= ~(1 << 4);
        if (tACD.AttributesGetInt(ATTRIBUTE_AI_IN_SPECIAL_STATE)) ptActorAI->dwPersistentFlags |= 1 << 5; else ptActorAI->dwPersistentFlags &= ~(1 << 5);
        if (tACD.AttributesGetInt(ATTRIBUTE_AI_USED_SPAWN_ANIM)) ptActorAI->dwPersistentFlags |= 1 << 6; else ptActorAI->dwPersistentFlags &= ~(1 << 6);
        if (ptActorAI->dwPersistentFlags)

            InactiveSave.SaveDWORD(ISI_PERSISTENT_FLAGS, ptActorAI->dwPersistentFlags);


        ACDID idACDSummoner = (ACDID)tACD.AttributesGetInt(ATTRIBUTE_SUMMONED_BY_ACDID);
        if (idACDSummoner != ACDID_INVALID)

            InactiveSave.SaveDWORD(ISI_SUMMONED_BY_ACDID, idACDSummoner);


        idACDSummoner = (ACDID)tACD.AttributesGetInt(ATTRIBUTE_SUMMONER_ID);
        if (idACDSummoner != ACDID_INVALID)

            InactiveSave.SaveDWORD(ISI_SUMMONER_ID, idACDSummoner);


        if (tACD.AttributesAttributeExists(ATTRIBUTE_SUMMON_EXPIRATION_TICK))

            InactiveSave.SaveDWORD(ISI_SUMMON_EXPIRATION_TICK, tACD.AttributesGetInt(ATTRIBUTE_SUMMON_EXPIRATION_TICK));


        if (tACD.AttributesAttributeExists(ATTRIBUTE_SUMMON_COUNT))

            InactiveSave.SaveDWORD(ISI_SUMMON_COUNT, tACD.AttributesGetInt(ATTRIBUTE_SUMMON_COUNT));



        if (tACD.IsMonster())
        {
            SNO snoMonster = tACD.GetMonsterSNO();
            const MonsterDefinition * ptDefMonster = (const MonsterDefinition *)gptGroupDefMonster->LockR(snoMonster, FALSE);
            if (ptDefMonster)
            {
                AIBehaviorType eStartBehavior = 0;
                if (ptDefMonster->arAIBehaviors[eStartBehavior] != SNO_NONE)
                {
                    AIBehaviorType eCurrBehavior = AIStateGetBehaviorType(idActor);



                    if (eCurrBehavior > eStartBehavior)











                        InactiveSave.SaveDWORD(ISI_BEHAVIOR_TYPE, eCurrBehavior);


                    if (eCurrBehavior >= 0)
                    {
                        SNO snoBehavior = ptDefMonster->arAIBehaviors[eCurrBehavior];
                        if (snoBehavior != SNO_NONE)
                        {
                            const AIBehaviorDefinition * ptdefBehavior = (const AIBehaviorDefinition *)gptGroupDefAIBehavior->LockRFast(snoBehavior);
                            if (ptdefBehavior)
                            {
                                for (int i = 0; i < 8; ++i)
                                {
                                    SNO snoCurrStateData = AIStateGetSNO(idActor, i);
                                    if (snoCurrStateData != ptdefBehavior->arAIState[i])

                                        InactiveSave.SaveDWORD(ISI_AISTATE_GBID_START + i, snoCurrStateData);
                                }
                            }
                        }
                    }
                }

                gptGroupDefMonster->UnlockR((void *&)ptDefMonster);
            }
        }





        if (AIHasNodePath(idActor))
        {
            AINodePathData * ptPathData = ptActorAI->ptNodePathData;
            if (ptPathData)
            {
                if (ptPathData->idPath != PATHID_INVALID)

                    InactiveSave.SaveDWORD(ISI_CURRENT_PATH, ptPathData->idPath);


                if (ptPathData->idPathNodeDest != PATHNODEID_INVALID)

                    InactiveSave.SaveDWORD(ISI_PATH_DESTINATION_NODE, ptPathData->idPathNodeDest);
            }
        }



        ACDID idACDGuardObject = (ACDID)tACD.AttributesGetInt(ATTRIBUTE_GUARD_OBJECT_ACDID);
        if (ACDTryToGet(idACDGuardObject))

            InactiveSave.SaveDWORD(ISI_GUARD_OBJECT_ACDID, idACDGuardObject);



        ACDID idACDForcedEnemy = (ACDID)tACD.AttributesGetInt(ATTRIBUTE_FORCED_ENEMY_ACDID);
        if (ACDTryToGet(idACDForcedEnemy))

            InactiveSave.SaveDWORD(ISI_FORCED_ENEMY_ACDID, idACDForcedEnemy);



        ACDID idACDFollowTarget = (ACDID)tACD.AttributesGetInt(ATTRIBUTE_FOLLOW_TARGET_ACDID);
        if (ACDTryToGet(idACDFollowTarget))

            InactiveSave.SaveDWORD(ISI_FOLLOW_TARGET_ACDID, idACDFollowTarget);

        AIFollowType eFollowType = tACD.AttributesGetInt(ATTRIBUTE_FOLLOW_TARGET_TYPE);
        if (eFollowType != 0)

            InactiveSave.SaveDWORD(ISI_FOLLOW_TARGET_TYPE, eFollowType);


        SpawnerCountID idSpawnerCount = (SpawnerCountID)tACD.AttributesGetInt(ATTRIBUTE_SPAWNER_CONCURRENT_COUNT_ID);
        if (idSpawnerCount != SPAWNERCOUNTID_INVALID)

            InactiveSave.SaveDWORD(ISI_SPAWNER_CONCURRENT_COUNT_ID, idSpawnerCount);



        if (ptActorAI->tAITetherData.flRadius > 0.0f)
        {
            InactiveSave.SaveDWORD(ISI_TETHER_LOCATION_X, (int32)ptActorAI->tAITetherData.placeTetherPoint.wp.x);
            InactiveSave.SaveDWORD(ISI_TETHER_LOCATION_Y, (int32)ptActorAI->tAITetherData.placeTetherPoint.wp.y);
            InactiveSave.SaveDWORD(ISI_TETHER_LOCATION_Z, (int32)ptActorAI->tAITetherData.placeTetherPoint.wp.z);
        }

        int32 bHasDoppelgangerCloned = tACD.AttributesGetInt(ATTRIBUTE_POWER_SAVED_ATTRIBUTE);
        if (bHasDoppelgangerCloned)

            InactiveSave.SaveDWORD(ISI_POWER_SAVED_ATTRIBUTE, bHasDoppelgangerCloned);


        int32 nExpensiveProcCount = tACD.AttributesGetInt(ATTRIBUTE_EXPENSIVE_PROC_COUNT);
        if (nExpensiveProcCount > 0)

            InactiveSave.SaveDWORD(ISI_EXPENSIVE_PROC_COUNT, nExpensiveProcCount);


        PingID idPing = (PingID)tACD.AttributesGetInt(ATTRIBUTE_PING);
        if (idPing != PINGID_INVALID)

            InactiveSave.SaveDWORD(ISI_PING, idPing);


        BOOL bEliteEngaged = tACD.AttributesGetInt(ATTRIBUTE_ELITE_ENGAGED);
        if (bEliteEngaged)

            InactiveSave.SaveDWORD(ISI_ELITE_ENGAGED, bEliteEngaged);



        if (tACD.AttributesAttributeExists(ATTRIBUTE_SPECIAL_INVENTORY_HAS_SOLD))

            InactiveSave.SaveDWORD(ISI_SPECIAL_INVENTORY_HAS_SOLD, tACD.AttributesGetInt(ATTRIBUTE_SPECIAL_INVENTORY_HAS_SOLD));



        if (tACD.AttributesAttributeExists(ATTRIBUTE_INVENTORY_REROLL_TIME))

            InactiveSave.SaveDWORD(ISI_INVENTORY_REROLL_TIME, tACD.AttributesGetInt(ATTRIBUTE_INVENTORY_REROLL_TIME));
    }
}




void ActorAIRestoreForActivatingActor(
    ActorID idActor,
    ActorSavedAI & tActorSavedAI)
{
    if (tActorSavedAI.dwIndexUsedFlags == 0)
    {
        return;
    }


    ActorAI * ptActorAI = ActorTryToGetAI(idActor);
    if (ptActorAI)
    {
        ACDID idACD = ACDFindByActor(idActor);
        ActorCommonData & tACD = ACDGet(idACD);
        AIInactiveSave InactiveSave(&tActorSavedAI);
        PathID idPath = PATHID_INVALID;
        PathNodeID idPathNodeDest = PATHNODEID_INVALID;

        for (;;)
        {
            uint32 dwData;
            int32 nIndex = InactiveSave.RestoreDWORD(dwData, idActor);

            if (nIndex >= ISI_COUNT)
            {
                break;
            }

            if (nIndex >= ISI_AISTATE_GBID_START && nIndex <= ISI_AISTATE_GBID_LAST)
            {

                AIStateSetStateData(idActor, nIndex - ISI_AISTATE_GBID_START, dwData);
            }
            else
            {
                switch (nIndex)
                {
                case ISI_PERSISTENT_FLAGS:
                    ptActorAI->dwPersistentFlags = dwData;
                    break;
                case ISI_SUMMONED_BY_ACDID:
                    tACD.AttributesSetInt(ATTRIBUTE_SUMMONED_BY_ACDID, dwData);
                    break;
                case ISI_SUMMONER_ID:
                    tACD.AttributesSetInt(ATTRIBUTE_SUMMONER_ID, dwData);
                    break;
                case ISI_SUMMON_EXPIRATION_TICK:
                    tACD.AttributesSetInt(ATTRIBUTE_SUMMON_EXPIRATION_TICK, dwData); ActorTriggerAttributeUpdate(idActor);
                    break;

                case ISI_SUMMON_COUNT:
                    tACD.AttributesSetInt(ATTRIBUTE_SUMMON_COUNT, dwData);
                    break;
                case ISI_BEHAVIOR_TYPE:
                    AIStateSetStateData(idActor, dwData);
                    break;
                case ISI_CURRENT_PATH:
                    idPath = (PathID)dwData;
                    break;
                case ISI_PATH_DESTINATION_NODE:
                    idPathNodeDest = (PathNodeID)dwData;
                    break;
                case ISI_GUARD_OBJECT_ACDID:
                    tACD.AttributesSetInt(ATTRIBUTE_GUARD_OBJECT_ACDID, dwData);
                    break;
                case ISI_FORCED_ENEMY_ACDID:
                    tACD.AttributesSetInt(ATTRIBUTE_FORCED_ENEMY_ACDID, dwData);
                    break;
                case ISI_FOLLOW_TARGET_ACDID:
                    tACD.AttributesSetInt(ATTRIBUTE_FOLLOW_TARGET_ACDID, dwData);
                    break;
                case ISI_FOLLOW_TARGET_TYPE:
                    tACD.AttributesSetInt(ATTRIBUTE_FOLLOW_TARGET_TYPE, dwData);
                    break;
                case ISI_SPAWNER_CONCURRENT_COUNT_ID:
                    tACD.AttributesSetInt(ATTRIBUTE_SPAWNER_CONCURRENT_COUNT_ID, dwData);
                    break;
                case ISI_TETHER_LOCATION_X:
                    ptActorAI->tAITetherData.placeTetherPoint.wp.x = (float)(int32)dwData;
                    break;
                case ISI_TETHER_LOCATION_Y:
                    ptActorAI->tAITetherData.placeTetherPoint.wp.y = (float)(int32)dwData;
                    break;
                case ISI_TETHER_LOCATION_Z:
                    ptActorAI->tAITetherData.placeTetherPoint.wp.z = (float)(int32)dwData;
                    break;
                case ISI_POWER_SAVED_ATTRIBUTE:
                    tACD.AttributesSetInt(ATTRIBUTE_POWER_SAVED_ATTRIBUTE, dwData);
                    break;
                case ISI_EXPENSIVE_PROC_COUNT:
                    tACD.AttributesSetInt(ATTRIBUTE_EXPENSIVE_PROC_COUNT, dwData);
                    break;
                case ISI_PING:
                    tACD.AttributesSetInt(ATTRIBUTE_PING, dwData);
                    break;
                case ISI_ELITE_ENGAGED:
                    tACD.AttributesSetInt(ATTRIBUTE_ELITE_ENGAGED, dwData);
                    break;
                case ISI_SPECIAL_INVENTORY_HAS_SOLD:
                    tACD.AttributesSetInt(ATTRIBUTE_SPECIAL_INVENTORY_HAS_SOLD, dwData);
                    break;
                case ISI_INVENTORY_REROLL_TIME:
                    tACD.AttributesSetInt(ATTRIBUTE_INVENTORY_REROLL_TIME, dwData); ActorTriggerAttributeUpdate(idActor);
                    break;

                default:
                    DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
                }
            }
        }


        tACD.AttributesSetInt(ATTRIBUTE_NPC_IS_OPERATABLE, (ptActorAI->dwPersistentFlags >> 4) & 1);
        tACD.AttributesSetInt(ATTRIBUTE_AI_IN_SPECIAL_STATE, (ptActorAI->dwPersistentFlags >> 5) & 1);
        tACD.AttributesSetInt(ATTRIBUTE_AI_USED_SPAWN_ANIM, (ptActorAI->dwPersistentFlags >> 6) & 1);

        if (idPath != PATHID_INVALID)
        {
            AISetNodePath(idActor, idPath, idPathNodeDest);
        }

        InactiveSave.FreeAllBlocks();
    }
}



void ActorAIFreeInactiveAI(ActorSavedAI & tActorSavedAI)
{
    AIInactiveSave InactiveSave(&tActorSavedAI);
    InactiveSave.FreeAllBlocks();
}




void ActorAISetState(ActorID idActor, uint32 dwState)
{
    int32 nMaxValue = 7;
    ActorAI * ptActorAI = ActorTryToGetAI(idActor);
    ptActorAI->dwPersistentFlags |= dwState;
}




















uint32 ActorAIGetState(ActorID idActor)
{
    uint32 dwMask = 7;
    ActorAI * ptActorAI = ActorTryToGetAI(idActor);
    uint32 dwState = ptActorAI->dwPersistentFlags & dwMask;
    return dwState;
}







void ActorAIRegisterProximityDetector(ActorID idActor)
{
    ActorAIGlobals * ptActorAIGlobals = SGameManager::GameGet().ptActorAIGlobals;
    ptActorAIGlobals->listProximityDetectors.AddTail_Tracked(idActor);
}




void ActorAIUnregisterProximityDetector(ActorID idActor)
{
    ActorAIGlobals * ptActorAIGlobals = SGameManager::GameGet().ptActorAIGlobals;
    POS pos = ptActorAIGlobals->listProximityDetectors.Find(idActor);
    ptActorAIGlobals->listProximityDetectors.RemoveAt(pos);
}




XPooledDataIDList * ActorAIGetTriggerList()
{
    ActorAIGlobals * ptActorAIGlobals = SGameManager::GameGet().ptActorAIGlobals;
    return &ptActorAIGlobals->listProximityDetectors;
}




void ActorAIUpdateServerVariables()
{
    ActorAIGlobals * ptActorAIGlobals = SGameManager::GameGet().ptActorAIGlobals;
    ptActorAIGlobals->uAdditionalBrainUpdateTicks = s_varBrainUpdateAdditionalTicks.Get();
    ptActorAIGlobals->uWarnNearbyAlliesCooldownTicks = s_varWarnNearbyAlliesAboutTargetCooldownTicks.Get();
    ptActorAIGlobals->uWasAttackedAddTargetCooldownTicks = s_varBrainWasAttackedAddTargetCooldownTicks.Get();
}





BOOL ActorAICanWarnNearbyAlliesAboutTarget(ActorID idActor)
{
    ActorAI * ptActorAI = ActorTryToGetAI(idActor);
    if (ptActorAI == NULL) {
        return FALSE;
    }
    uint32 dwCurTick = GameGetServerTime();
    return dwCurTick >= ptActorAI->dwNextAllowedWarnNearbyAlliesTime;
}












BOOL ActorAIWasAttackedCanAddTarget(ActorID idActor)
{
    ActorAI * ptActorAI = ActorTryToGetAI(idActor);
    if (ptActorAI == NULL) {
        return FALSE;
    }
    uint32 dwCurTick = GameGetServerTime();
    return dwCurTick >= ptActorAI->dwNextAllowedOnAttackedAddTargetTime;
}












void ActorAIResetWarnNearbyAlliesAboutTargetTime(ActorID idActor)
{
    ActorAI * ptActorAI = ActorTryToGetAI(idActor);
    if (ptActorAI == NULL) {
        return;
    }
    ActorAIGlobals * ptActorAIGlobals = SGameManager::GameGet().ptActorAIGlobals;
    ptActorAI->dwNextAllowedWarnNearbyAlliesTime = GameGetServerTime() + ptActorAIGlobals->uWarnNearbyAlliesCooldownTicks;
}








void ActorAIResetWasAttackedAddTargetCooldown(ActorID idActor)
{
    ActorAI * ptActorAI = ActorTryToGetAI(idActor);
    if (ptActorAI == NULL) {
        return;
    }
    ActorAIGlobals * ptActorAIGlobals = SGameManager::GameGet().ptActorAIGlobals;
    ptActorAI->dwNextAllowedOnAttackedAddTargetTime = GameGetServerTime() + ptActorAIGlobals->uWasAttackedAddTargetCooldownTicks;
}








void ActorAISetTempAffixList(AffixSelectionList * ptList, int32 * pnAffixListWeightTotal)
{
    ActorAIGlobals * ptActorAIGlobals = SGameManager::GameGet().ptActorAIGlobals;
    ptActorAIGlobals->ptTempAffixList = ptList;
    ptActorAIGlobals->pnAffixListWeightTotal = pnAffixListWeightTotal;
}







AffixSelectionList * ActorAIGetTempAffixList()
{
    ActorAIGlobals * ptActorAIGlobals = SGameManager::GameGet().ptActorAIGlobals;
    return ptActorAIGlobals->ptTempAffixList;
}






int32 * ActorAIGetTempAffixWeightTotal()
{
    ActorAIGlobals * ptActorAIGlobals = SGameManager::GameGet().ptActorAIGlobals;
    return ptActorAIGlobals->pnAffixListWeightTotal;
}






TAffixSelectionPool & ActorAIGetAffixSelectionPool()
{
    ActorAIGlobals * ptActorAIGlobals = SGameManager::GameGet().ptActorAIGlobals;
    return ptActorAIGlobals->tAffixSelectionPool;
}






ProximityDetectorDataID ProximityDetectorGetNewData()
{
    ActorAIGlobals * ptActorAIGlobals = SGameManager::GameGet().ptActorAIGlobals;
    ProximityDetectorDataID idData = ptActorAIGlobals->ptProximityDataArray->ElementNew();
    return idData;
}









ProximityDetectorBrainData * ProximityDetectorTryGetData(ProximityDetectorDataID idData)
{
    ProximityDetectorBrainData * ptData = NULL;
    if (idData != PROXIMITYDETECTORID_INVALID) {
        ActorAIGlobals * ptActorAIGlobals = SGameManager::GameGet().ptActorAIGlobals;
        ptData = ptActorAIGlobals->ptProximityDataArray->ElementTryToGet(idData);
    }
    return ptData;
}








// NON_MATCHING: the inlined TChainedDataArray::ElementDelete free-index loop (ChainedDataArrayTemplated.h, shared) keeps two index registers; the original walks one.
void ProximityDetectorDataDelete(
    ProximityDetectorDataID idData)
{
    ActorAIGlobals * ptActorAIGlobals = SGameManager::GameGet().ptActorAIGlobals;
    if (idData == PROXIMITYDETECTORID_INVALID) {
        return;
    }
    ptActorAIGlobals->ptProximityDataArray->ElementDeleteByID(idData);
}

