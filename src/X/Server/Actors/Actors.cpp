// Original file: X/Server/Actors/Actors.cpp
#include "../../Common/BBEUtil.h"
#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Common/ActorCommon.h"
#include "../../Common/ActorCommonData.h"
#include "../../Common/Attributes/FastAttribute.h"
#include "../../Common/Definitions/LevelAreaDefinition.h"
#include "../../Common/Definitions/QuestRangeDefinition.h"
#include "../../Common/Game.h"
#include "../../Common/GameCommonData.h"
#include "../../Common/Monsters.h"
#include "../../Common/Networking/GameMessage.h"
#include "../../Common/Player.h"
#include "../../Common/TeamCommon.h"
#include "../../Common/Nav/NavigationMesh.h"
#include "../../Common/Protobuf/OnlineService.pb.h"
#include "../../Common/SceneCommon.h"
#include "../Movement/SActorMovement.h"
#include "../SGameManager.h"
#include "../Timers.h"
#include "Actors.h"
#include "../../Sigma/DataStructures/ChainedDataArrayTemplated.h"
#include "../../Sigma/MemoryPools.h"
#include "../../Sigma/DataStructures/XVar.h"
#include "../../Client/Gfx/GfxVB.h"
#include "../../Common/Definitions/GlobalDefinition.h"
#include "../../Common/Definitions/GameBalanceDefinition.h"
#include "../PlayerList.h"
#include "../AI/ActorAI.h"
#include <float.h>
#include "../../Sigma/ErrorManager/ErrorManager.h"
#include "../SGameConstants.h"
#include "../../Common/TagMaps/Tags.h"
#include "../../Common/TagMaps/MarkerTags.h"
#include "../../Common/Definitions/MarkerSetDefinition.h"
#include "../../Common/Marker/Marker.h"
#include "../Powers/PowerHelper.h"
// X/Common/SceneCommon.h
void SceneCommonDataActivate(const SSceneID idSScene);  // SceneCommon.cpp:729
// X/Common/Nav/NavigationMesh.h
void NavMeshGetCenterPlace(const NavMeshHandle & tNavMeshHandle, WorldPlace & tPlaceCenter);  // NavigationMesh.cpp:992
BOOL NavMeshSearchForLocation(const WorldPlace & placeStart, const PassabilitySettings & tPassability, const float flMinSearchRadius, const float flMaxSearchRadius, const uint32 dwFindLocationFlags, NavMeshHandle & tNavMeshHandleLanding, WorldPlace & placeLanding, BOOL bSearchOverImpassable, Team eTeam, float flRadiusOverride);  // NavigationMesh.cpp:3421
// X/Server/Marker/SMarker.h
enum DeactivateStatus {
    DEACTIVATE_YES = 0,
    DEACTIVATE_ONLY_WITH_SCENE = 1,
    DEACTIVATE_NO_PLAYER_SEEN = 2,
    DEACTIVATE_NO_TOWN_PORTAL = 3,
};
DeactivateStatus MarkerCanDeactivateActor(const ActorID idActor);  // SMarker.cpp:384
void ActorDeactivate(ActorID idActor, BOOL bDeleteActor);  // Actors.cpp:6524
// X/Server/Network/KnownList.h
#include "../Network/KnownList.h"
#include "../Powers/PowerAPI.h"
#include "../Actors/ActorPowers.h"
#include "../STalk.h"
// X/Server/AI/ActorAI.h
BOOL ActorIsUsingAIQueue(ActorID idActor);  // ActorAI.cpp:2187
// X/Server/AI/BrainComponents.h
void BrainAPIDoIdle(ActorID idActor);
BOOL ActorGetSpawnParams(ActorAddStruct & tActorAdd);  // Actors.cpp (defined later in this file) static BOOL sActorGetSpawnPosition(ActorAddStruct & tActorAdd, PassabilitySettings * ptPassability);
// X/Common/ActorCommonData.h
void LocationSetBounds(SNO snoActor, WorldLocation & tLocation, const ActorDefinition * pdefActor);  // ActorCommonData.cpp:4326
// X/Common/Actors/ActorCollision.h
void ACDSetBoundingSphere(ActorCommonData & tACD, const Sphere & wsOld);  // ActorCollision.cpp:249
void ActorInternalSetPosition(Actor * ptActor, const WorldPlace & tPlace);  // Actors.cpp:4663
// X/Server/System/LevelArea.h
SNO LevelAreaGetSNO(LevelAreaID idLevelArea);  // LevelArea.cpp:518
// X/Server/SPlayer.h
void SPlayerOnChangeLevelArea(ACDID idACDPlayer, LevelAreaID idLevelAreaOld, LevelAreaID idLevelAreaNew);  // SPlayer.cpp:7960
// X/Server/AI/BrainAPI.h
typedef int32 DeathType;
void BrainAPIDoScriptedDeath(ActorCommonData & tACD, DeathType eDeathType, BOOL bCrit, BOOL bDropLoot, ActorID idKiller);  // BrainAPI.cpp:11213
// X/Common/ActorInventory.h
void ACDInventoryEquipmentOrSkillChanged(ActorCommonData & tACD);  // ActorInventory.cpp:9033
// X/Server/SPlayer.h
void SPlayerMarkSceneAsEntered(const SSceneID idSScene);  // SPlayer.cpp:4297
// X/Server/Quests/SQuests.h
void SQuestsOnPlayerEnterScene(ACDID idACDPlayer, SNO snoScene);  // SQuests.cpp:4715
// X/Common/Networking/GameMessage.h (kept local, like the other message structs)
#include "../../Common/BBE.h"
#include "../../Common/GizmoSwig.h"
#include "../../Common/Networking/Network.h"
#include "../../Common/TagMaps/ActorTags.h"
void InactiveMarkerCreateFromActor(ActorID idActor);  // X/Common/InactiveMarker.cpp:2275
#include "../SWorld.h"
// X/Common/Nav/NavigationMesh.h
SSceneID NavMeshHandleToSubSceneSceneID(const NavMeshHandle & tNavMeshHandle);  // NavigationMesh.cpp:3892
uint32 NavMeshHandleToStaticFlags(const NavMeshHandle & tNavMeshHandle, BOOL fSpecialOnly);  // NavigationMesh.cpp:2348
// X/Server/SWorld.h
void SWorldRunSpecialNavMeshFunc(SWorldID idSWorld, ActorID idActor, int32 eSpecialType, int32 eFuncType);  // SWorld.cpp:2170
// X/Common/Player.h
int32 PlayerGetNumPlayersForDifficulty();  // Player.cpp:2159
// X/Common/GameDebug.h
void GameDebugSetCurrentUpdatingActor(ACDID idACD);  // GameDebug.cpp:368
// X/Server/SGizmo.h
void GizmoUpdate(ActorCommonData & tACDGizmo);  // SGizmo.cpp:2572
// X/Common/SceneCommon.h
SNO SceneCommonGetSceneSNO(SSceneID idSScene);  // SceneCommon.cpp:1881
// X/Common/ActorCommonData.h
LevelAreaID ACDGetLevelAreaID(const ActorCommonData & tACD);  // ActorCommonData.cpp:4399
int32 ActorGlobalsGetNewAttributeUpdateBucketIndex();  // Actors.cpp:7201
// X/Server/Timers.h
uint32 TimerGetTime(const TimerID idTimer);  // Timers.cpp:458
#include "../Map/Minimap.h"
#include "../Crafting/SItemCrafting.h"
#include "../../Client/SNOPreload.h"
void LuaScriptDetach(int32 eType, DataID idAttachment, SNO snoScript); void OnSavePointPortalReturnDeleted(ActorCommonData & tACDPortal); void SpawnBrainActorDeleting(Actor & tActor); BOOL GizmoIsWaypointActivated(ActorCommonData & tACDWaypoint, BOOL bRequirePair, SNO * psnoLevelArea, int32 * pnWaypointNumber);  // SGizmo.cpp:1029, LuaScript.cpp:1665, SGizmo.cpp:3460, SpawnerBrain.cpp:1419
BOOL STrickleIsActorTrickled(const ActorID idActor); void STrickleRemoveFromAll(const ACDID idACD);  // X/Server/Network/STrickle.cpp:105, 87
void BrainAPIActorReleaseTarget(ActorID idActor); void BrainAPIReleaseAllTargets(ActorID idActor);  // BrainComponents.cpp:8709, 9705
void ActorPowersDispose(Actor * ptActor); void ProjectileDispose(const ACDID idACDProjectile); void ActorAIDispose(const ActorID idActor);  // ActorPowers.cpp:153, Projectile.cpp:170, ActorAI.cpp:1050
void PlayerRemovePet(ACDID idACDPlayer, ACDID idACDPet, BOOL bDisplayChatMessage); void ActorCollisionDispose(ActorCommonData & tACD); void ItemDeleteAltAttribGroup(const ActorCommonData & tACDItem);  // Player.cpp:2374, ActorCollision.cpp:205, Items.cpp:7854
void ACDInventoryDispose(ActorInventory *& ptInventory); void ACDVisualInventoryDispose(VisualInventory *& ptVisualInventory);  // ActorInventory.cpp:1525, VisualInventory.cpp:254
void ActorTimerEffectsDispose(const ActorID idActor); BOOL ActorHasQuestLifespan(const Actor & tActor); SNO ActorGetQuestLifespan(const Actor & tActor); void ActorRemoveWaypointFromInactiveList(const ActorID idActor); void ActorGlobalsDecrementAttributeUpdateBucketValue(const int32 nBucket); void ActorGlobalsDecrementAreaDamageUpdateBucketValue(const int32 nBucket);  // Actors.cpp
#include "../../Common/Definitions/MonsterDefinition.h"
#include "../../Common/Hyperlink.h"
static const uint32 DEFAULT_ATTRIBUTE_UPDATE_INTERVAL = 20;
static const uint32 GIZMO_UPDATE_RATE = 6;
static const float DEFAULT_GARBAGE_COLLECTION_THRESHOLD_PCT = 0.75f;
static const float DEFAULT_GARBAGE_COLLECTION_SEVERE_THRESHOLD_PCT = 0.9f;
static const uint32 DEFAULT_GARBAGE_COLLECTION_RARE_EXPLOIT_CHECK_THRESHOLD = 100;
static const uint32 DEFAULT_GARBAGE_COLLECTION_RARE_EXPLOIT_CHECK_FOLLOWUP_THRESHOLD = 500;
static const uint32 DEFAULT_GARBAGE_COLLECTION_RARES_IN_SCENE_THRESHOLD = 75;
static const float DEFAULT_THROTTLE_SCRIPT_ACTORS_THRESHOLD_PCT = 0.85f;
static const uint32 DEFAULT_TICKS_AREA_DAMAGE_BUCKETING_RATE = 20;
#include "../../Common/GlobalSNO.h"
CRefString GBIDToString(const GBHandle & hGB, const BOOL fUsedForXMLFields); CRefString GetBlameError(const SNOName & tSNOName);  // GameBalanceDefinition.cpp:3467, X/Common/SNO/SNOML_X.cpp:385
static const float ELITE_SPAWN_MIN_DISTANCE = 60.0f;
static void sReportSpawnFailure(LPCSTR sReason, const ActorAddStruct & tActorAdd); static void sActorDelete(Actor * ptActor, BOOL bWorldDeleted);
void NavMeshGetActorPassabilitySettings(SNO snoActor, float flScale, PassabilitySettings & tPassability); void ActorGetSpawnSkipDistance(int32 & nSkipDistMin, int32 & nSkipDistDelta);  // NavigationMesh.cpp:3007, Actors.cpp:6679
#include "../../Common/Gizmo.h"
ActorID ActorAdd(const ActorAddStruct & tActorAdd); void sInitFadeAttribute(Actor & tActor); void sAttributesInitForGizmo(Actor * ptActor, ActorCommonData & tACD, const ActorAddStruct & tActorAdd, const int32 nLevel);  // Actors.cpp:2948, 1799, 1822
static XVarScope s_scopeActors("Actors", NULL);
XVarUint32 s_varAttributeUpdateTicks(s_scopeActors, "AttributeUpdateTicks", DEFAULT_ATTRIBUTE_UPDATE_INTERVAL);
XVarFloat s_varGarbageCollectionThresholdPercent(s_scopeActors, "GarbageCollectionThresholdPercent", DEFAULT_GARBAGE_COLLECTION_THRESHOLD_PCT);
XVarFloat s_varGarbageCollectionSevereThresholdPercent(s_scopeActors, "GarbageCollectionSevereThresholdPercent", DEFAULT_GARBAGE_COLLECTION_SEVERE_THRESHOLD_PCT);
XVarUint32 s_varGarbageCollectionEmergencyCleanupAmount(s_scopeActors, "GarbageCollectionEmergencyCleanupAmount", 11776);
XVarUint32 s_varGarbageCollectionRaresInSceneThreshold(s_scopeActors, "GarbageCollectionRaresInSceneThreshold", DEFAULT_GARBAGE_COLLECTION_RARES_IN_SCENE_THRESHOLD);
XVarFloat s_varThrottleScriptActorsThresholdPercent(s_scopeActors, "ThrottleScriptActorsThresholdPercent", DEFAULT_THROTTLE_SCRIPT_ACTORS_THRESHOLD_PCT);
XVarUint32 s_varMaxActorSoftCap(s_scopeActors, "MaxActorSoftCap", 11264);
#include "../../Common/ActorCommonDataSwig.h"
#include "../../Common/ActorCommonCollision.h"
struct ActorSpawnSkipDistance {  // size 0x8
    int32 nSkipDistanceMin;  // 0x00
    int32 nSkipDistanceDelta;  // 0x04
};
void ACDCollGetSettings(SNO snoActor, ACDCollSettings & tCollSettings); uint32 NavMeshHandleGetStaticFlags(const NavMeshHandle & tNavMeshHandle); BOOL NavMeshPlaceCanEnterForSpawning(const WorldPlace & placeSpawn, const PassabilityType ePassabilityType);  // NavigationMesh.cpp
struct ActorGarbageCollection {  // size 0x18
    uint32 dwIndex;  // 0x00
    SceneCommonDataIterator tSceneIter;  // 0x08
};
float AdjustHPForPlayerCount(const float flHP, const int32 nOldPlayerMult, const int32 nNewPlayerMult); namespace BossEncounterManager { SNO GetActiveEncounterSNO(); }  // GlobalDefinition.cpp:1543, X/Server/System/BossEncounter.cpp:1467
struct ActorGlobals {  // size 0x230
    uint32 dwActorNumberCount;  // 0x00
    uint32 dwActorHighCount;  // 0x04
    TChainedDataArray<Actor, ActorID> * ptActors;  // 0x08
    ActorIDListPooled listActorsFlaggedForDeletion;  // 0x10
    ActorIDListPooled listActorsSpawned;  // 0x38
    ActorIDListPooled listActorsActivated;  // 0x60
    ActorIDListPooled listActorsWithQuestLifespan;  // 0x88
    ActorIDListPooled listActorsWithHirelingClass;  // 0xb0
    ActorIDListPooled listInactiveWaypoints;  // 0xd8
    int32 nActorSpawnSkipDistanceNum;  // 0x100
    ActorSpawnSkipDistance arActorSpawnSkipDistanceStack[8];  // 0x104
    ActorGarbageCollection tGarbageCollection;  // 0x148
    uint32 uAttributeUpdateInterval;  // 0x160
    uint32 uMaxActorSoftCap;  // 0x164
    float flGarbageCollectionThresholdPct;  // 0x168
    float flGarbageCollectionSevereThresholdPct;  // 0x16c
    uint32 uGarbageCollectionEmergencyCleanupAmount;  // 0x170
    uint32 uGarbageCollectionRareExploitCheckThreshold;  // 0x174
    uint32 uGarbageCollectedWreckables;  // 0x178
    uint32 uGarbageCollectedGold;  // 0x17c
    uint32 uGarbageCollectedItems;  // 0x180
    uint32 uGarbageCollectedRareItems;  // 0x184
    float flThrottleScriptActorsThresholdPct;  // 0x188
    uint32 arAttributeUpdateBuckets[20];  // 0x18c
    uint32 arAreaDamageUpdateBuckets[20];  // 0x1dc
};
static_assert(sizeof(ActorGlobals) == 0x230, "ActorGlobals");
const GameBalanceDefinition * MonsterLevelTableLockR(); void MonsterLevelTableUnlockR(const GameBalanceDefinition * lpDef);  // X/Common/Definitions/GameBalanceDefinition.cpp:2281, 2290
struct TimedEffects {  // size 0x34
    AttribGroupID arFastAttribs[13];  // 0x00
};
ActorID sMonsterAddByIndex(const ActorAddStruct & tActorAddOriginal, EncounterAddStruct & tEncounterAdd); ActorID SceneryAdd(const ActorAddStruct & tActorAddOriginal); ActorID PropAddBySNO(ActorAddStruct & tActorAdd); ActorID GizmoAddBySNO(int32 nLevel, ActorAddStruct & tActorAdd);  // Actors.cpp:5475, 5865, 5847, 5803
struct TimedEffectsGlobals {  // size 0x30, Actors.h
    XChainedFixedBlockAllocator<TimedEffects> poolTimedEffects;  // 0x00
};
static_assert(sizeof(TimedEffectsGlobals) == 0x30, "TimedEffectsGlobals");
#include "../../Common/Items.h"
void ActorAIInitForGame();  // X/Server/AI/ActorAI.cpp:225
void SpawnBrainInitForGame();  // X/Server/AI/SpawnerBrain.cpp:108
void ActorProjectileInitializeForGame();  // X/Server/Actors/Projectile.cpp:84
void ActorTimerEffectsInitializeForGame();  // Actors.cpp:6178
void ActorTimerEffectsDisposeForGame();  // Actors.cpp:6193
void ActorAIDisposeForGame();  // X/Server/AI/ActorAI.cpp
void SpawnBrainDisposeForGame();  // X/Server/AI/SpawnerBrain.cpp
void ActorProjectileDisposeForGame();  // X/Server/Actors/Projectile.cpp
#include "../../Common/Protobuf/Items.pb.h"
int32 ActorsGetCount(const SGame & tSGame);
static const int32 sgcSkipDistanceMin = 0;
static const int32 sgcSkipDistanceDelta = 4;
BOOL EncounterResolveToActorSpawnInfo(SNO snoEncounter, RNG & rng, SNOName & tSNOName, int32 & eTeam); LevelAreaID ScenesCommonLevelAreaIDFromPlace(SSceneID idSScene, Vector3D wp); LPCSTR SNOGroupGetName(const SNOGroup snoGroup); ActorID GBAdd(ActorAddStruct & tActorAdd);  // EncounterDefinition.cpp, SceneCommon.cpp:1316, SNOGroups.cpp:78, Actors.cpp:6041
#include "../SScene.h"
#include "../System/LevelArea.h"
#include "../../Common/Actors/ActorCollision.h"
void ActorsIntersectFrustum(SWorldID idSWorld, const Frustum & tFrustum, XPooledDataIDList & listACDs, XBitArray<11776> * ptActorsInListBitArray, uint32 dwSphereTreeFlags); BOOL NPCShouldBeHiddenForClient(const ActorCommonData & tACD, const Player * ptPlayer);  // ActorCollision.cpp:576, X/Server/SNPC.cpp:1308
BOOL SceneCommonIsExcluded(SSceneID idSScene, int32 nPlayerIndex); void SceneCommonPlayerSees(SSceneID idSScene, int32 nPlayerIndex, BOOL bSeen, BOOL bSeenPrefetch);  // SceneCommon.cpp:2742, 2807
BOOL ReplayIsUsingRecordedAVFrustum();  // X/Server/Replay.cpp:849
#include "../System/SNephalemRift.h"
int32 SWorldGetTieredDungeonFinderLevel(SWorldID idSWorld); void PVPPlayerRemoveShrineBuffs(ActorCommonData & tACDPlayer);  // SWorld.cpp:3357, X/Server/PVP.cpp:1433
void ActorCheckChangeLevelArea(ActorID idActor); void ActorCheckNavMeshTypeChange(const ActorID idActor, const uint32 uPrevNavMeshFlags, const SWorldID idSWorldPrev);  // Actors.cpp:2804, 2833
BOOL CanSpawnActorByScrollOfCalling(SNO snoActor); struct LooteeInfo; BOOL ConditionEvaluate(const SNO snoCondition, const Player * ptPlayer, const ACDID idACD, const LooteeInfo * ptLooteeInfo); LPCSTR SWorldGetDungeonFinderPopulation(SNO snoWorld); BOOL SWorldIsDungeonFinder(SWorldID idSWorld);  // PowerAPI.cpp:1039, Condition.cpp:640, SWorld.cpp:2622, SWorld.cpp:3304
#include "../../Common/Definitions/EncounterDefinition.h"
ItemQualityLevel ItemGetDisplayedQualityLevel(const ActorCommonData & tACD); SWorldID SceneCommonGetSWorldID(SSceneID idSScene); void BrainAPIDoDeath(ActorID idDeadActor, ActorID idKillerActor, const BOOL fNoExp, const BOOL fNoLoot, const BOOL fNoHealthDrop, PowerAPIDamage * ptDamage, float * pflDamageDone);  // Items.cpp:2208, SceneCommon.cpp:1891, X/Server/AI/BrainComponents.h
#include "../../Common/Achievements/AchievementDefines.h"
#include "../LuaScript/ScriptEvent.h"
#include "../AI/ProximityDetectorBrain.h"
ProximityDetectorBrainData * ProximityDetectorTryGetData(ProximityDetectorDataID idData); void ProximityDetectorBrainOnCreate(ActorID idTrigger); void SQuestsCheckForBountyActorSpawned(Actor & tActor); void SGizmoAddStaticGizmoPing(ActorCommonData & tACDGizmo);  // SGame.cpp:4719, ProximityDetectorBrain.cpp:736, SQuests.cpp:5695, SGizmo.cpp:3540
ItemQualityLevel ItemGetDisplayedQualityLevel(const ActorCommonData & tACD); SWorldID SceneCommonGetSWorldID(SSceneID idSScene); void BrainAPIDoDeath(ActorID idDeadActor, ActorID idKillerActor, const BOOL fNoExp, const BOOL fNoLoot, const BOOL fNoHealthDrop, PowerAPIDamage * ptDamage, float * pflDamageDone);  // X/Server/AI/BrainComponents.h, Items.cpp:2208, SceneCommon.cpp:1891
#include "../../Common/InactiveMarker.h"
#include "../../Common/GameTemplate.h"
#include "../LuaScript/LuaScript.h"
void ActorsDumpList(); void ScenePrintDeactivationReasonsString(); void SGameForcePlayersQuit(QuitReason eReason); ACDID ACDCreate(); void ActorInventorySetSeed(ActorInventory * pInv, uint32 dwSeed);  // Actors.cpp:5036, SceneCommon.cpp:3175, SGame.cpp:7555, ActorCommonData.cpp:518, ActorInventory.cpp:12876
BOOL SNOCheckSNOReference(SNOGroup snoGroupSrc, SNO snoHandleSrc, SNOGroup snoGroupDst, SNO snoHandleDst, LPCSTR szExtraMsg); void PlayerSetACD(Player & tPlayer, const ACDID idACD); void ActorCollisionInitialize(ActorCommonData & tACD);  // SNO.cpp:10239, Player.cpp:1438, ActorCollision.cpp:185
void SceneClearSpawnArea(SceneCommonData & tSCD, int nSquareIndex, float flRadius, BOOL bUseEliteMask); void SScenePutACDInScene(SSceneID idSScene, ACDID idACD, BOOL bAdd); void ActorPowersInitialize(ActorID idActor);  // SceneCommon.cpp:3092, SScene.cpp:513, ActorPowers.cpp:134
void LuaScriptRestoreForActivatingActor(ActorID idActor, SavedDataKey * ptSavedDataKey); void STrickleTryTrickleToPlayers(const ACDID idACD); void ActorSetBrain(const ActorID idActor, ActorBrain nBrain, ActorSavedAI * ptSavedAI);  // LuaScript.cpp:5288, STrickle.cpp:273, ActorAI.cpp:449

#include "../System/Adventure.h"
static void sActorSetScale(const ActorID idActor, const float flScale); static void sActorAddGetPassabilitySettings(const ActorAddStruct & tActorAdd, PassabilitySettings & tPassability, const ActorDefinition * pdefActor); void ActorTimerEffectsInit(const ActorID idActor);
#include "../../Common/HirelingSwig.h"
static void sAttributesInitForMonster(ActorCommonData & tACD, const ActorAddStruct & tActorAdd); void sMonsterSpawnMinions(ActorCommonData & tACD, const ActorAddStruct & tActorAddOriginal, EncounterAddStruct & tEncounterAdd);  // Actors.cpp:2357, 5386
BOOL MonsterExists(SNO snoMonster); Team TeamGetForActorSNO(SNO snoActor); void MonsterAddAffixes(ActorID idActor, const ActorAddStruct & tActorAdd, EncounterAddStruct & tEncounterAdd); void MonsterDoOnCreatePowers(ACDID idACD);  // Monsters.cpp:67, TeamCommon.cpp:394, SMonsters.cpp:1197, 1260
ACDPack & ACDPackGet(const ACDPackID idACDPack); ACDPackID ACDPackNew(); SNO ACDGetInventory(const ActorCommonData * ptACD, BOOL bSecondary); void SACDInventoryLoad(ACDID idACDContainer, SNO snoInventory);  // ActorCommonData.cpp:342, 490, ActorInventory.cpp:12508, 9594
#include "../../Sigma/Stopwatch.h"
#include "../SGameStatsSwig.h"
#include "../../Common/Hireling.h"

PowerID PowerAPI_StartSpawnAnimPower(const ACDID idACD, uint32 dwAnimTag); void SGameStatsCounterIncrement(StatCounterType eType, int32 nParam0, int32 nParam1, float flValue);  // PowerAPI.cpp:12673, SGameStats.cpp:1504
void ActorTriggerAttributeUpdate(const ActorID idActor); void SPortalSpecifierInitialize(Actor * ptActor, const ResolvedPortalDestination & tDest); void GizmoRestoreState(ActorCommonData & tACD); void GizmoReturnPointPortalInitialize(ActorCommonData & tACD); void GizmoCreate(ActorCommonData & tACD); void SetCurrentSavePointPortalReturn(const ActorCommonData & tACDDestination);  // SPortal.cpp:154, SGizmo.cpp:2833, 3373, 2793, 3410
#include "../../Common/PortalCommon.h"
void ActGetCurrentStartLocation(ResolvedPortalDestination & tDest); SWorldID WorldCommonFindBySNO(SNO snoWorld, BOOL bIgnorePendingDelete); int32 WorldGetDungeonFinderLevel(const SNO snoWorld); BOOL StartingLocationGUIDIsValid(const uint32 uGUID);  // ActDefinition.cpp:260, World.cpp:605, 828, WorldDefinition.cpp:1511
#include "../../Common/Definitions/BossEncounterDefinition.h"
void GameDebugSetCurrentUpdatingActor(ACDID idACD); BOOL ACDIsProjectile(ACDID idACD); void ServerUpdateWasCalled(UpdateData & tUpdateData); void PowerAPI_DoHitpointRegen(ActorCommonData & tACD, float flHitpointsPerSecond, uint32 dwTimeStep); void PowerAPI_BuffDealDOTDamage(PowerAPIBuffInstance & tBuff, uint32 uInterval);  // Game.cpp:368, ActorCommonData.cpp:5494, SGame.cpp:2072, PowerAPI.cpp:773, 12342
void PowerAPI_DealStaggeredDamage(Actor * ptActor); void PowerAPI_DealFrailtyDamage(ActorCommonData & tACD, const float flHP); void ACDInventoryEquipmentOrSkillChanged(ActorCommonData & tACD); void SPlayerUpdate(ActorCommonData & tACDPlayer); BOOL BrainAPIResurrectToCheckpoint(ACDID idACD);  // PowerAPI.cpp:12423, 12498, ActorInventory.cpp:9033, SPlayer.cpp:6828, BrainAPI.cpp:3134
void ServerMessageSendAcceptedDeathAck(const ACDID idACDPlayer); ACDID NPCIsAnyPlayerTalkingTo(ACDID idACDTalkingTo); void SACDInventoryReroll(ActorCommonData & tACDContainer); ACDID ACDGetControllingActor(ACDID idACDPet); uint32 AnimPlayerGetCurrentAnimTag(ActorCommonData & tACD);  // ServerMessageSend.cpp:1652, NPC.cpp:246, ActorInventory.cpp:9697, ActorCommonData.cpp:1690, AnimPlayer.cpp:1856
void PowerAPI_UpdateSummonedActorAttributes(ActorCommonData & tACDSummoned, ActorCommonData & tACDOwner, Power * ptPower); void FastAttribGroupApplyDirty(AttribGroupID idAttribGroup); void sUpdateQueuedFloatingNumberMessages(ActorCommonData & tACD);  // PowerAPI.cpp:13626, FastAttribute.cpp:3250, Actors.cpp:3908
#include "../../Common/TagMaps/AnimTagsSwig.h"
InventoryId::InventoryId()
{
    m_uHeroId = OnlineService::INVALID_HERO_ID;
}

InventoryId & InventoryId::operator=(const OnlineService::GameAccountHandle & tHandle)
{
    SetInvalid();
    m_tGameAccount = tHandle;
    return *this;
}


InventoryId & InventoryId::operator=(OnlineService::HeroId uHeroId)
{
    SetInvalid();
    m_uHeroId = uHeroId;
    return *this;
}
#include "../System/SetEncounter.h"
BOOL SWorldIsSetDungeon(SWorldID idSWorld);  // X/Server/SWorld.cpp:3338
void AIStateMerge(ActorID idActor, SNO snoAIState, uint32 uLifetime, BuffID idBuff);  // X/Server/AI/AIStates.cpp:6207
#include "../../Common/TagMaps/MonsterTags.h"










bool InventoryId::IsInvalid() const
{
    return m_uHeroId == OnlineService::INVALID_HERO_ID && m_tGameAccount.IsInvalid();
}


void InventoryId::SetInvalid()
{
    m_uHeroId = OnlineService::INVALID_HERO_ID;
    m_tGameAccount.SetInvalid();
}


void InventoryId::Import(const D3::OnlineService::EntityId & tEntityId)
{
    if (tEntityId.id_high() == 0)
    {
        m_uHeroId = tEntityId.id_low();
        m_tGameAccount.SetInvalid();
    }
    else
    {
        m_tGameAccount.Import(tEntityId);
        m_uHeroId = OnlineService::INVALID_HERO_ID;
    }
}


void InventoryId::Export(D3::OnlineService::EntityId & tEntityId) const
{
    if (m_uHeroId != OnlineService::INVALID_HERO_ID)
    {
        tEntityId.set_id_high(0);
        tEntityId.set_id_low(m_uHeroId);
    }
    else
    {
        m_tGameAccount.Export(tEntityId);
    }
}



















ActorAddStruct::ActorAddStruct() { Clear(); }




ActorAddStruct::ActorAddStruct(const ActorAddStruct & other)
{
    idMarkersSScene = other.idMarkersSScene;
    snoActor = other.snoActor;
    dwActorAddFlags = other.dwActorAddFlags;
    dwMarkersetCreateOptions = other.dwMarkersetCreateOptions;
    tPlace = other.tPlace;
    qOrientation = other.qOrientation;
    flScale = other.flScale;
    tMarkersName = other.tMarkersName;
    tNavMeshHandle = other.tNavMeshHandle;
    tNavMeshHandlePathingSpace = other.tNavMeshHandlePathingSpace;
    hGB = other.hGB;
    tSNOName = other.tSNOName;
    eTeam = other.eTeam;
    ptSavedAI = other.ptSavedAI;
    ptLuaSavedDataKey = other.ptLuaSavedDataKey;
    eFastAttribType = other.eFastAttribType;
    prngActorSeed = other.prngActorSeed;
    eMonsterRarity = other.eMonsterRarity;
    idLevelArea = other.idLevelArea;
    snoEncounterSource = other.snoEncounterSource;
    nLevel = other.nLevel;
    dwInventorySeed = other.dwInventorySeed;
    for (int32 i = 0; i < 2; i++)
    {
        pRareNameGBIDs[i] = other.pRareNameGBIDs[i];
    }
    for (int32 i = 0; i < 8; i++)
    {
        pMonsterAffixGBIDs[i] = other.pMonsterAffixGBIDs[i];
        pMonsterSpawnerAffixGBIDs[i] = other.pMonsterSpawnerAffixGBIDs[i];
    }
    nSpawnerAdditionalAffixes = other.nSpawnerAdditionalAffixes;

    plistActors = other.plistActors;

    listSNOScripts.SetPool(MemoryPoolsGetDATAIDListPool());
    POS posScript = other.listSNOScripts.GetHeadPosition();
    while (posScript != NULL)
    {
        listSNOScripts.AddTail_Tracked(other.listSNOScripts.GetNext(posScript));
    }

    snoConvList = other.snoConvList;
    tMarkerOwner = other.tMarkerOwner;
    ptPlayer = other.ptPlayer;
    dwSpawnAnimTag = other.dwSpawnAnimTag;
    uPrimaryGroup = other.uPrimaryGroup;
    uSecondaryGroup = other.uSecondaryGroup;
    bAvenger = other.bAvenger;
    bIsSummon = other.bIsSummon;
}



void ActorAddStruct::Clear()
{
    idMarkersSScene = SSCENEID_INVALID;
    snoActor = SNO_NONE;
    dwActorAddFlags = 0;
    dwMarkersetCreateOptions = 0;
    tPlace = cgplaceNull;
    qOrientation = cgqIdentity;
    flScale = 0.0f;
    tMarkersName = 0;
    tNavMeshHandle.nSquareIndex = -1;
    tNavMeshHandle.idSScene = SSCENEID_INVALID;
    tNavMeshHandlePathingSpace.nSquareIndex = -1;
    tNavMeshHandlePathingSpace.idSScene = SSCENEID_INVALID;
    hGB = cghgbNone;
    tSNOName = SNONAME_NONE;
    eTeam = -1;
    ptSavedAI = NULL;
    ptLuaSavedDataKey = NULL;
    eFastAttribType = 1;
    prngActorSeed = NULL;
    eMonsterRarity = 0;
    idLevelArea = -1;
    plistActors = NULL;
    snoEncounterSource = SNO_NONE;
    nLevel = -1;
    listSNOScripts.SetPool(MemoryPoolsGetDATAIDListPool());
    listSNOScripts.RemoveAll();
    snoConvList = SNO_NONE;
    tMarkerOwner.Initialize();
    ptPlayer = NULL;
    dwSpawnAnimTag = 0xffffffff;
    uPrimaryGroup = 0xffffffff;
    uSecondaryGroup = 0xffffffff;
    dwInventorySeed = 0;
    nSpawnerAdditionalAffixes = 0;
    bAvenger = FALSE;
    bIsSummon = FALSE;
    SigmaMemoryFill(pRareNameGBIDs, 0xff, sizeof(pRareNameGBIDs));
    SigmaMemoryFill(pMonsterAffixGBIDs, 0xff, sizeof(pMonsterAffixGBIDs));
    SigmaMemoryFill(pMonsterSpawnerAffixGBIDs, 0xff, sizeof(pMonsterSpawnerAffixGBIDs));
}
void ActorsInitializeForGame()
{

    SGame & tSGame = SGameManager::GameGet();
    tSGame.ptActorGlobals = (ActorGlobals *)SigmaMemoryNew(sizeof(ActorGlobals), 4, tSGame.ptMemoryInterface, TRUE);
    tSGame.ptActorGlobals->ptActors = TChainedDataArrayCreateTracked<Actor, ActorID>("Actors", tSGame.ptMemoryInterface, 512);
    tSGame.ptActorGlobals->ptActors->SetGrowCap(0x2e00);

    tSGame.ptActorGlobals->listActorsFlaggedForDeletion.Initialize(MemoryPoolsGetDATAIDListPool());
    tSGame.ptActorGlobals->listActorsSpawned.Initialize(MemoryPoolsGetDATAIDListPool());
    tSGame.ptActorGlobals->listActorsActivated.Initialize(MemoryPoolsGetDATAIDListPool());
    tSGame.ptActorGlobals->listActorsWithQuestLifespan.Initialize(MemoryPoolsGetDATAIDListPool());
    tSGame.ptActorGlobals->listActorsWithHirelingClass.Initialize(MemoryPoolsGetDATAIDListPool());
    tSGame.ptActorGlobals->listInactiveWaypoints.Initialize(MemoryPoolsGetDATAIDListPool());
    tSGame.ptActorGlobals->nActorSpawnSkipDistanceNum = 0;
    tSGame.ptActorGlobals->uAttributeUpdateInterval = DEFAULT_ATTRIBUTE_UPDATE_INTERVAL;
    tSGame.ptActorGlobals->uMaxActorSoftCap = 11264;
    tSGame.ptActorGlobals->flGarbageCollectionThresholdPct = DEFAULT_GARBAGE_COLLECTION_THRESHOLD_PCT;
    tSGame.ptActorGlobals->flGarbageCollectionSevereThresholdPct = DEFAULT_GARBAGE_COLLECTION_SEVERE_THRESHOLD_PCT;
    tSGame.ptActorGlobals->uGarbageCollectionEmergencyCleanupAmount = 11776;
    tSGame.ptActorGlobals->uGarbageCollectionRareExploitCheckThreshold = 100;
    tSGame.ptActorGlobals->uGarbageCollectedWreckables = 0;
    tSGame.ptActorGlobals->uGarbageCollectedGold = 0;
    tSGame.ptActorGlobals->uGarbageCollectedItems = 0;
    tSGame.ptActorGlobals->uGarbageCollectedRareItems = 0;
    tSGame.ptActorGlobals->flThrottleScriptActorsThresholdPct = DEFAULT_THROTTLE_SCRIPT_ACTORS_THRESHOLD_PCT;

    SigmaMemoryClear(tSGame.ptActorGlobals->arAttributeUpdateBuckets, sizeof(tSGame.ptActorGlobals->arAttributeUpdateBuckets));

    ActorAIInitForGame();
    SpawnBrainInitForGame();
    ActorProjectileInitializeForGame();
    ActorTimerEffectsInitializeForGame();


















    tSGame.ptActorGlobals->tGarbageCollection.tSceneIter = SceneCommonDataIterator();



    tSGame.ptActorGlobals->tGarbageCollection.dwIndex = 0;
}







































































void ActorsDisposeForGame()
{

    SGame & tSGame = SGameManager::GameGet();
    ActorGlobals *& ptActorGlobals = tSGame.ptActorGlobals;



























    ActorTimerEffectsDisposeForGame();
    ActorProjectileDisposeForGame();
    SpawnBrainDisposeForGame();
    ActorAIDisposeForGame();

    ptActorGlobals->listActorsFlaggedForDeletion.Dispose();
    ptActorGlobals->listActorsSpawned.Dispose();
    ptActorGlobals->listActorsActivated.Dispose();
    ptActorGlobals->listActorsWithQuestLifespan.Dispose();
    ptActorGlobals->listActorsWithHirelingClass.Dispose();
    ptActorGlobals->listInactiveWaypoints.Dispose();

    if (ptActorGlobals->ptActors)
    {
        TChainedDataArrayDispose(ptActorGlobals->ptActors);
    }


    SigmaMemoryFree(ptActorGlobals, tSGame.ptMemoryInterface); ptActorGlobals = NULL;


}


static void sPvPPerformGarbageCollectionOverACDs()
{


    uint32 dwNow = GameGetServerTime();

    ActorGlobals * ptActorGlobals = SGameManager::GameGet().ptActorGlobals;

    uint32 dwBase = ptActorGlobals->tGarbageCollection.dwIndex;
    uint32 dwStride = 368;



    for (uint32 dw = dwBase; dw < ptActorGlobals->ptActors->dwMaximumCount; dw += dwStride)
    {
        Actor * ptActor = ptActorGlobals->ptActors->ElementGetFromBaseIndex(dw);


        if (ptActor->id == ACTORID_INVALID)
        {
            continue;
        }


        ACDID idACD = ptActor->idACD;
        ActorCommonData & tACD = ACDGet(idACD);
        if (!tACD.IsItem())
        {
            continue;
        }


        if (tACD.tLocation.eActorLocation != AL_WORLD)
        {
            continue;
        }


        if (ItemHasLabel(tACD.hGB.gbid, ITL_GLYPH) && GameGetGameType() == 3)
        {
            continue;
        }


        uint32 dwHitGroundTime = tACD.GetHitGroundTime();
        if (dwNow > dwHitGroundTime + 3600)
        {
            ActorDelete(ptActor->id);
        }
    }


    ptActorGlobals->tGarbageCollection.dwIndex++;
    if (ptActorGlobals->tGarbageCollection.dwIndex == dwStride) ptActorGlobals->tGarbageCollection.dwIndex = 0;
}






static void sCheckExploitRareItems(SGame & tSGame)
{


    ActorGlobals * ptActorGlobals = tSGame.ptActorGlobals;

    XGrowableMap<SSceneID, SSceneID, uint32, uint32> tRareItemsInScene;
    for (uint32 i = 0; i < ptActorGlobals->ptActors->dwMaximumCount; i++)
    {

        if (ptActorGlobals->ptActors->ElementGetFromBaseIndex(i)->id == ACTORID_INVALID)
        {
            continue;
        }


        ACDID idACD = ptActorGlobals->ptActors->ElementGetFromBaseIndex(i)->idACD;
        ActorCommonData * ptACD = ACDTryToGet(idACD);
        if (!ptACD ||
            !ptACD->IsItem() ||
            !ptACD->IsInWorld())
        {
            continue;
        }


        ItemQualityLevel eQualityLevel = ItemGetDisplayedQualityLevel(*ptACD);
        if (eQualityLevel < IQL_RARE_AFFIX_START)
        {
            continue;
        }


        SSceneID idSScene = ACDGetCurrentSSceneID(*ptACD);
        if (idSScene == SSCENEID_INVALID)
        {
            continue;
        }

        SScene * ptSScene = SSceneTryToGet(idSScene);
        if (!ptSScene)
        {
            continue;
        }



        uint32 dwRareItemsInScene = 0;
        tRareItemsInScene.Lookup(idSScene, dwRareItemsInScene);
        tRareItemsInScene.SetAt_Tracked(idSScene, dwRareItemsInScene + 1);
    }


    uint32 dwRaresInSceneThreshold = s_varGarbageCollectionRaresInSceneThreshold.Get();
    POS pos = tRareItemsInScene.GetStartAssoc();
    while (pos)
    {
        SSceneID idSScene;
        uint32 dwRareItemsInScene;
        tRareItemsInScene.GetNextAssoc(pos, idSScene, dwRareItemsInScene);

        if (dwRareItemsInScene >= dwRaresInSceneThreshold)
        {
            SNO snoScene = SceneCommonGetSceneSNO(idSScene);
            CRefString szSceneName = SNOToString(SCENE_SNO, snoScene, FALSE);

            SWorldID idSWorld = SceneCommonGetSWorldID(idSScene);
            SNO snoWorld = WorldCommonGetWorldSNO(idSWorld);
            CRefString szWorldName = SNOToString(WORLD_SNO, snoWorld, FALSE);






        }
    }

    tSGame.ptActorGlobals->uGarbageCollectionRareExploitCheckThreshold = DEFAULT_GARBAGE_COLLECTION_RARE_EXPLOIT_CHECK_FOLLOWUP_THRESHOLD;
}


static void sGarbageCollectItem(
    SGame & tSGame,
    ActorID idItemToDestroy,
    ItemQualityLevel eIQL,
    BOOL bItemIsCraftingReagent)
{
    ActorDelete(idItemToDestroy);
    tSGame.ptActorGlobals->uGarbageCollectedItems++;

    if (eIQL >= IQL_RARE_AFFIX_START)
    {
        if (!bItemIsCraftingReagent)
        {
            tSGame.ptActorGlobals->uGarbageCollectedRareItems++;
        }

        if (tSGame.ptActorGlobals->uGarbageCollectedRareItems % tSGame.ptActorGlobals->uGarbageCollectionRareExploitCheckThreshold == 0)
        {
            sCheckExploitRareItems(tSGame);
        }
    }
}



void ActorForceCleanupItemsOnGround(int32 nPlayerIndex)
{

    SGame & tSGame = SGameManager::GameGet();
    ActorGlobals * ptActorGlobals = tSGame.ptActorGlobals;


    for (uint32 dw = 0; dw < ptActorGlobals->ptActors->dwMaximumCount; dw++)
    {

        Actor * ptActor = ptActorGlobals->ptActors->ElementGetFromBaseIndex(dw);

        if (ptActor->id == ACTORID_INVALID)
        {
            continue;
        }

        ACDID idACD = ptActor->idACD;
        ActorCommonData * ptACD = ACDTryToGet(idACD);
        if (!ptACD)
        {
            continue;
        }

        if (ptACD->IsItem())
        {
            int32 nPlayerRecipient = ptACD->AttributesGetInt(FastAttribKey(ATTRIBUTE_ITEM_PLAYER_RECIPIENT));

            if (nPlayerRecipient != -1)
            {
                Player * ptPlayer = PlayerGetByPlayerIndex(nPlayerRecipient);
                if (!ptPlayer || ptPlayer->idPlayerACD == ACDID_INVALID || nPlayerRecipient == nPlayerIndex)
                {
                    ActorDelete(ptActor->id);
                }
            }
        }
    }
}






static void sPerformGarbageCollectionOverACDs(
    uint32 nForcedActorCleanupAmount)
{

    uint32 dwNow = GameGetServerTime();

    SGame & tSGame = SGameManager::GameGet();
    ActorGlobals * ptActorGlobals = tSGame.ptActorGlobals;

    uint32 dwBase = ptActorGlobals->tGarbageCollection.dwIndex;
    uint32 dwStride = 184;


    BOOL bForceDeletion = FALSE;
    uint32 uActorCount = ptActorGlobals->ptActors->GetTotalCount();
    uint32 uGarbageCollectionThreshold = (uint32)(ptActorGlobals->flGarbageCollectionThresholdPct * ptActorGlobals->uMaxActorSoftCap);
    uint32 uGarbageCollectionSevereThreshold = (uint32)(ptActorGlobals->flGarbageCollectionSevereThresholdPct * ptActorGlobals->uMaxActorSoftCap);
    if (nForcedActorCleanupAmount > 0 || uActorCount >= uGarbageCollectionThreshold)
    {
        bForceDeletion = TRUE;
        dwBase = 0;
        dwStride = 1;
    }

    uint32 nWreckablesDeletedThisFrame = 0;
    uint32 nGoldDeletedThisFrame = 0;
    uint32 nCrappyItemsDeletedThisFrame = 0;
    uint32 nTotalActorsDeletedThisFrame = 0;

    uint32 nMaxWreckablesDeletedThisFrame = 16;
    uint32 nMaxGoldDeletedThisFrame = 16;
    uint32 nMaxCrappyItemsDeletedThisFrame = 16;

    uint32 uOldCrappyItemDeletionTickThreshold = 0;
    BOOL bDeleteWhiteItems = FALSE;
    if (nForcedActorCleanupAmount > 0)
    {
        nMaxWreckablesDeletedThisFrame = nForcedActorCleanupAmount;
        nMaxGoldDeletedThisFrame = nForcedActorCleanupAmount;
        nMaxCrappyItemsDeletedThisFrame = nForcedActorCleanupAmount;
    }

    else if (uActorCount >= uGarbageCollectionSevereThreshold)
    {
        nMaxWreckablesDeletedThisFrame = 64;
        nMaxGoldDeletedThisFrame = 64;
        nMaxCrappyItemsDeletedThisFrame = 32;


        const uint32 OLD_CRAPPY_ITEM_DELETION_TIMER = 54000;
        uOldCrappyItemDeletionTickThreshold = dwNow > OLD_CRAPPY_ITEM_DELETION_TIMER ? dwNow - OLD_CRAPPY_ITEM_DELETION_TIMER : 0;

        bDeleteWhiteItems = TRUE;
    }


    uint32 dwOldestItemCreation = 0;
    ItemQualityLevel eOldestItemQuality = IQL_RARE_6_AFFIX;
    BOOL bOldestItemIsCraftingReagent = FALSE;
    ActorID idItemToDestroy = ACTORID_INVALID;


    for (uint32 dw = dwBase; dw < ptActorGlobals->ptActors->dwMaximumCount; dw += dwStride)
    {
        if (nForcedActorCleanupAmount > 0 && nTotalActorsDeletedThisFrame >= nForcedActorCleanupAmount)
        {
            return;
        }

        Actor * ptActor = ptActorGlobals->ptActors->ElementGetFromBaseIndex(dw);


        if (ptActor->id == ACTORID_INVALID)
        {
            continue;
        }

        ACDID idACD = ptActor->idACD;
        ActorCommonData * ptACD = ACDTryToGet(idACD);
        if (!ptACD)
        {
            continue;
        }




        if (ptACD->IsItem() && ItemHasLabel(ptACD->hGB.gbid, ITL_SHRINE_GLYPH))
        {
            uint32 dwHitGroundTime = ptACD->GetHitGroundTime();
            if (dwNow > dwHitGroundTime + 600)
            {
                ActorDelete(ptActor->id);
                nTotalActorsDeletedThisFrame++;
            }
            continue;
        }
        if (ptACD->IsItem())
        {
            int32 nPlayerRecipient = ptACD->AttributesGetInt(FastAttribKey(ATTRIBUTE_ITEM_PLAYER_RECIPIENT));
            if (nPlayerRecipient != -1)
            {
                Player * ptPlayer = PlayerGetByPlayerIndex(nPlayerRecipient);
                if (!ptPlayer || ptPlayer->idPlayerACD == ACDID_INVALID)
                {
                    ActorDelete(ptActor->id);
                    nTotalActorsDeletedThisFrame++;
                }
            }
        }




        if (ptACD->IsItem() && ItemGetUseType(*ptACD) == ITEM_USE_LOOT_RUN_PROGRESS_GLYPH)
        {
            uint32 dwHitGroundTime = ptACD->GetHitGroundTime();
            if (dwNow > dwHitGroundTime + 18000)
            {
                ActorDelete(ptActor->id);
                nTotalActorsDeletedThisFrame++;
            }
            continue;
        }


        if (ptACD->IsItem() && ItemHasLabel(ptACD->hGB.gbid, ITL_GLYPH))
        {
            uint32 dwHitGroundTime = ptACD->GetHitGroundTime();
            if (dwNow > dwHitGroundTime + 3600)
            {
                ActorDelete(ptActor->id);
                nTotalActorsDeletedThisFrame++;
            }
            continue;
        }


        if (ptACD->IsNecroCorpse())
        {
            uint32 dwHitGroundTime = ptACD->GetHitGroundTime();
            if (dwNow > dwHitGroundTime + 7200)
            {
                ActorDelete(ptActor->id);
                nTotalActorsDeletedThisFrame++;
            }
            continue;
        }


        if (!bForceDeletion)
        {
            continue;
        }

        if (ptACD->IsWreckableObject())
        {
            if (nWreckablesDeletedThisFrame >= nMaxWreckablesDeletedThisFrame)
            {
                continue;
            }


            GizmoType eGizmoType = ptACD->GetGizmoType();
            if (eGizmoType == ACTORGIZMO_REFORMING_DESTROYABLE_OBJECT)
            {
                continue;
            }


            uint32 dwTimeSinceCreation = dwNow - ptActor->dwTimeCreated;
            if (dwTimeSinceCreation > 3600) {
            BrainAPIDoDeath(ptActor->id, ACTORID_INVALID, TRUE, TRUE, TRUE, NULL, NULL);
            tSGame.ptActorGlobals->uGarbageCollectedWreckables++;
            nWreckablesDeletedThisFrame++;
            nTotalActorsDeletedThisFrame++;
            }
        }

        else if (ptACD->IsItem() &&


                 ptACD->tLocation.eActorLocation == AL_WORLD)
        {




            if (ItemIsGold(*ptACD))
            {
                if (nGoldDeletedThisFrame < nMaxGoldDeletedThisFrame)
                {
                    ActorDelete(ptActor->id);
                    tSGame.ptActorGlobals->uGarbageCollectedGold++;
                    nGoldDeletedThisFrame++;
                    nTotalActorsDeletedThisFrame++;
                }
            }
            else
            {
                BOOL bItemIsCraftingReagent = ItemIsCraftingReagent(*ptACD);
                ItemQualityLevel eQualityLevel = ItemGetDisplayedQualityLevel(*ptACD);
                if (ItemGetMaxStackSize(ptACD->hGB, NULL, NULL) > 1)
                {
                    if (ItemHasLabel(ptACD->hGB.gbid, ITL_BLOODSHARD))
                    {
                        eQualityLevel = IQL_RARE_6_AFFIX;
                    }
                    else if (ItemHasLabel(ptACD->hGB.gbid, ITL_PLATINUM))
                    {
                        eQualityLevel = IQL_AFFIX_COUNT_END;
                    }
                    else
                    {
                        eQualityLevel = Math::Min<ItemQualityLevel>(eQualityLevel, IQL_RARE_AFFIX_START);
                    }
                }

                if (bDeleteWhiteItems && eQualityLevel <= IQL_NORMAL &&

                    nCrappyItemsDeletedThisFrame < nMaxCrappyItemsDeletedThisFrame)
                {
                    sGarbageCollectItem(tSGame, ptActor->id, eQualityLevel, bItemIsCraftingReagent);
                    nCrappyItemsDeletedThisFrame++;
                    nTotalActorsDeletedThisFrame++;
                }



                else if (eQualityLevel <= eOldestItemQuality)
                {
                    if (nForcedActorCleanupAmount > 0)
                    {
                        sGarbageCollectItem(tSGame, ptActor->id, eQualityLevel, bItemIsCraftingReagent);
                        nCrappyItemsDeletedThisFrame++;
                        nTotalActorsDeletedThisFrame++;
                        continue;
                    }

                    uint32 dwHitGroundTime = ptACD->GetHitGroundTime();
                    if (nCrappyItemsDeletedThisFrame < nMaxCrappyItemsDeletedThisFrame && dwHitGroundTime < uOldCrappyItemDeletionTickThreshold)
                    {
                        sGarbageCollectItem(tSGame, ptActor->id, eQualityLevel, bItemIsCraftingReagent);
                        nCrappyItemsDeletedThisFrame++;
                        nTotalActorsDeletedThisFrame++;
                        continue;
                    }




                    uint32 dwTimeSinceCreation = dwNow - ptActor->dwTimeCreated;
                    if (eQualityLevel < eOldestItemQuality || dwTimeSinceCreation > dwOldestItemCreation)
                    {
                        if (idItemToDestroy == ACTORID_INVALID)
                        {
                            nCrappyItemsDeletedThisFrame++;
                            nTotalActorsDeletedThisFrame++;
                        }

                        dwOldestItemCreation = dwTimeSinceCreation;
                        eOldestItemQuality = eQualityLevel;
                        bOldestItemIsCraftingReagent = bItemIsCraftingReagent;
                        idItemToDestroy = ptActor->id;
                    }
                }
            }
        }
    }
    if (idItemToDestroy != ACTORID_INVALID)
    {
        sGarbageCollectItem(tSGame, idItemToDestroy, eOldestItemQuality, bOldestItemIsCraftingReagent);
    }


    ptActorGlobals->tGarbageCollection.dwIndex++;
    if (ptActorGlobals->tGarbageCollection.dwIndex >= dwStride) ptActorGlobals->tGarbageCollection.dwIndex = 0;



}




static void sPerformGarbageCollectionOverInactiveList()
{


    uint32 dwNow = GameGetServerTime();

    ActorGlobals * ptActorGlobals = SGameManager::GameGet().ptActorGlobals;

    SceneCommonData * ptSCD = NULL;


    uint32 dwMaxScenesPerUpdate = 2;
    uint32 dwScenesScanned = 0;


    while (SceneCommonDataArrayIterate(ptActorGlobals->tGarbageCollection.tSceneIter, ptSCD, SkipHolodeckScenes) && dwScenesScanned < dwMaxScenesPerUpdate)
    {

        dwScenesScanned++;

        if (ptSCD->ptMarkerScene &&
            ptSCD->ptMarkerScene->ptInactiveItemMarkerList)
        {




            POS pos = ptSCD->ptMarkerScene->ptInactiveItemMarkerList->GetHeadPosition();

            while (pos)
            {

                POS posCur = pos;
                InactiveItemMarker * ptInactiveItem = ptSCD->ptMarkerScene->ptInactiveItemMarkerList->GetNext(pos);

                uint32 dwDeactivateTime = ptInactiveItem->dwCreateTime;


                if (ItemHasLabel(ptInactiveItem->tItemGenerator.gb_handle().gbid(), ITL_QUEST))
                {
                    continue;
                }


                if (ACDTryToGet(ptInactiveItem->idACDBoundTo))
                {
                    continue;
                }



                if (ItemHasLabel(ptInactiveItem->tItemGenerator.gb_handle().gbid(), ITL_GLYPH))
                {
                    if (dwNow > dwDeactivateTime + 3600)
                    {




                        ptSCD->ptMarkerScene->ptInactiveItemMarkerList->RemoveAt(posCur);
                    }
                    continue;
                }
                uint32 dwTimeFrame;

                ItemQualityLevel eIQL = ptInactiveItem->tItemGenerator.item_quality_level();

                switch (eIQL)
                {
                case IQL_INFERIOR:
                case IQL_NORMAL:
                case IQL_SUPERIOR:
                    dwTimeFrame = 18000;
                    break;
                case IQL_MAGIC_1_AFFIX:
                case IQL_MAGIC_2_AFFIX:
                case IQL_MAGIC_3_AFFIX:
                case IQL_SPECIAL:
                    dwTimeFrame = 36000;
                    break;
                case IQL_AFFIX_COUNT_END:
                    dwTimeFrame = 144000;
                    break;
                default:
                    if (eIQL >= IQL_RARE_AFFIX_START && eIQL <= IQL_RARE_AFFIX_END) dwTimeFrame = 144000;
                    else
                    {




                        DisplayInternalError(NULL, "", 1219, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
                        return;
                    }
                }



                if (dwNow > dwTimeFrame &&


                    dwDeactivateTime + dwTimeFrame < dwNow)
                {

                    Minimap::RemovePing(ptInactiveItem->idPing);
                    ptSCD->ptMarkerScene->ptInactiveItemMarkerList->RemoveAt(posCur);
                }
            }
        }
    }


    if (!ptSCD)
    {


        ptActorGlobals->tGarbageCollection.tSceneIter = SceneCommonDataIterator();
    }
}




void ActorsPerformGarbageCollection()
{


    if (GameTypeIsPvP())
    {
        sPvPPerformGarbageCollectionOverACDs();
    }
    else
    {

        sPerformGarbageCollectionOverACDs(0);
        sPerformGarbageCollectionOverInactiveList();
    }
}




static void sActorDelete(Actor * ptActor, BOOL bWorldDeleted)
{


    if (!ptActor)
    {
        return;
    }

    ActorCommonData & tACD = ACDGet(ptActor->idACD);


    if (ptActor->dwActorFlags & (1 << ACTOR_IS_PLAYER))
    {
        const GameBalanceDefinition * pdefGB;
        const HeroData * pHeroData = (const HeroData *)GBLockR(tACD.hGB, pdefGB, TRUE);
        if (pHeroData)
        {
            SNOPreload::RemoveServerSkillKit(pHeroData->arSkillKit[0]);
            GBUnlockR(pdefGB);
        }
        SNOPreload::RemoveServerActorBySNO(ptActor->snoActor);
    }
    else if (ptActor->dwActorFlags & (1 << ACTOR_SUMMON_PRELOADED))
    {
        ScopedMonsterDefinitionLockR pdefMonster(ptActor->snoMonster);
        for (int32 i = 0; i < MAX_SUMMON_ACTORS; i++)
        {
            if (pdefMonster->snoSummonActor[i] != SNO_NONE)
            {

                SNOPreload::RemoveServerActorBySNO(pdefMonster->snoSummonActor[i]);
            }
        }
    }


    ptActor->dwActorFlags |= (1 << ACTOR_DELETING_BIT);

    POS posScript = ptActor->listSNOScripts.GetHeadPosition();
    while (posScript)
    {
        LuaScriptDetach(SCRIPT_ATTACH_ACTOR, ptActor->idACD, ptActor->listSNOScripts.GetNext(posScript));
    }

    ActorGlobals * ptActorGlobals = SGameManager::GameGet().ptActorGlobals;



    if (ActorHasQuestLifespan(*ptActor))
    {
        POS pos = ptActorGlobals->listActorsWithQuestLifespan.Find(ptActor->id);
        if (pos)
        {
            ptActorGlobals->listActorsWithQuestLifespan.RemoveAt(pos);
        }
    }

    if (ACDGetHirelingClass(tACD) != 0 && !ACDIsHireling(tACD))
    {
        POS pos = ptActorGlobals->listActorsWithHirelingClass.Find(ptActor->id);
        if (pos)
        {
            ptActorGlobals->listActorsWithHirelingClass.RemoveAt(pos);
        }
    }

    if (tACD.IsNPC())
    {
        STalkForcePlayersDoneTalkingToNPC(ptActor->idACD);
    }

    switch (tACD.GetGizmoType())
    {
    case ACTORGIZMO_TOWN_PORTAL:
        OnSavePointPortalReturnDeleted(tACD);
        break;


    case ACTORGIZMO_WAYPOINT:
        ActorRemoveWaypointFromInactiveList(ptActor->id);
        break;
    }





    BOOL bIsDeactivating = ptActor->dwActorFlags & (1 << ACTOR_DEACTIVATING);
    if (!bIsDeactivating)
    {
        PingID idPing = tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_PING));
        if (idPing != -1)
        {
            Minimap::RemovePing(idPing);
        }
    }

    if (ptActor->ptProjectile)
    {
        Power * ptPower = PowerTryToGet(ActorPowersGetOwnerPower(ptActor->id));
        if (ptPower)
        {
            if (ptPower->listAffectedACDs.Count() > 0)
            {
                POS posAffected = ptPower->listAffectedACDs.GetHeadPosition();
                while (posAffected)
                {
                    POS posCur = posAffected;
                    AffectedACD tAffectedACD = ptPower->listAffectedACDs.GetNext(posAffected);
                    if (tAffectedACD.idACDSource == ptActor->idACD)
                    {
                        ptPower->listAffectedACDs.RemoveAt(posCur);
                    }
                }
            }
        }
    }

    if (ptActor->nAttributeUpdateBucketIndex != -1)
    {
        ActorGlobalsDecrementAttributeUpdateBucketValue(ptActor->nAttributeUpdateBucketIndex);
        ptActor->nAttributeUpdateBucketIndex = -1;
    }

    if (ptActor->nAreaDamageBucketIndex != -1)
    {
        ActorGlobalsDecrementAreaDamageUpdateBucketValue(ptActor->nAreaDamageBucketIndex);
        ptActor->nAreaDamageBucketIndex = -1;
    }

    SpawnBrainActorDeleting(*ptActor);







    if (STrickleIsActorTrickled(ptActor->id))
    {
        STrickleRemoveFromAll(tACD.id);
    }

    BrainAPIActorReleaseTarget(ptActor->id);
    BrainAPIReleaseAllTargets(ptActor->id);
    PowerAPI_BuffFlagAllForDeletion(tACD);
    ActorPowersDispose(ptActor);
    ProjectileDispose(tACD.id);
    ActorAIDispose(ptActor->id);
    ActorTimerEffectsDispose(ptActor->id);
    delete ptActor->ptStaggerDamageInfo; ptActor->ptStaggerDamageInfo = NULL;


    if (tACD.IsPet())
    {
        ACDID idACDOwner = ACDGetPetOwner(tACD.id);
        if (idACDOwner != ACDID_INVALID)
        {
            PlayerRemovePet(idACDOwner, tACD.id, TRUE);
        }
    }

    if (tACD.IsInWorld())
    {
        ActorCollisionDispose(tACD);
    }

    if (ptActor->ptSActorMovement)
    {
        SActorMovementDispose(ptActor);
    }



    if (tACD.IsItem())
    {
        if (tACD.IsInWorld() && !ptActor->tItemId.IsInvalid() && bWorldDeleted)
        {
            if (
                GameIsBattleNetServer() &&



                ptActor->tItemId != OnlineService::ItemId::INVALID)
            {
                SACDInventoryAddToDirtyList(tACD, AIDA_REMOVE, tACD.GetLocationSlot(), SLOT_NONE);
            }
        }


        ItemDeleteAltAttribGroup(tACD);
    }

    ACDInventoryDispose(tACD.ptInventory);
    ACDVisualInventoryDispose(tACD.ptVisualInventory);

    KnownListItemDeleted(KET_ACD, tACD.id);
    ACDDelete(tACD.id, bWorldDeleted);

    ptActorGlobals->ptActors->ElementDelete(ptActor);


}




void ActorsDeleteForWorld(SWorldID idSWorld)
{

    ActorIterator tIt;
    while (Actor * ptActor = ActorDataArrayIterate(tIt))
    {


        ActorCommonData * ptACD = ACDTryToGet(ptActor->idACD);
        if (ptACD &&
            ptACD->IsInSWorldID(idSWorld))
        {
            sActorDelete(ptActor, TRUE);
        }
    }
}



























static void sReportSpawnFailure(LPCSTR sReason, const ActorAddStruct & tActorAdd)
{

    if (tActorAdd.dwActorAddFlags & (1 << ACTORADD_DONT_REPORT_SPAWN_ERROR))
    {
        return;
    }


    SNO snoBadMarker = GlobalSNOGet(GLOBALSNO_ACTOR_TOD_BAD_MARKER);
    SNO snoBadActor = GlobalSNOGet(GLOBALSNO_ACTOR_TOD_BAD_ACTOR);
    if (tActorAdd.snoActor == snoBadMarker || tActorAdd.snoActor == snoBadActor)
    {
        return;
    }

    CRefString sMessage;

    sMessage.Format("Spawn Failed: %s", sReason);

    if (tActorAdd.tMarkersName != 0)
    {

        sMessage += CRefString().Format("  Marker Name: %s.", MarkerNameGetString(tActorAdd.tMarkersName));
    }

    if (tActorAdd.snoActor != SNO_NONE)
    {
        CRefString sActor = HyperlinkCreateFromSNO(SNOName(ACTOR_SNO, tActorAdd.snoActor), NULL);
        sMessage += CRefString().Format("  Actor SNO: %s.", sActor.str());
    }

    if (tActorAdd.hGB.gbid != GBID_NONE)
    {
        CRefString sGamebalance = GBIDToString(tActorAdd.hGB, FALSE);
        sMessage += CRefString().Format("  GBID: %s.", sGamebalance.str());
    }

    SWorld * ptSWorld = SWorldTryToGet(tActorAdd.tPlace.idSWorld);
    if (ptSWorld)
    {
        sMessage += CRefString().Format("  Location: [%0.2f, %0.2f, %0.2f] in %s.",
            tActorAdd.tPlace.wp.x,
            tActorAdd.tPlace.wp.y,
            tActorAdd.tPlace.wp.z,
            HyperlinkCreateFromSNO(SNOName(WORLD_SNO, ptSWorld->tWCD.snoWorld), NULL).str());
    }

    if (tActorAdd.idMarkersSScene != SSCENEID_INVALID)
    {
        SceneCommonData & tSCD = SceneCommonDataGet(tActorAdd.idMarkersSScene);

        sMessage += CRefString().Format("  Scene: %s.", HyperlinkCreateFromSNO(SNOName(SCENE_SNO, tSCD.snoScene), NULL).str());
    }

    if (tActorAdd.tMarkerOwner.snoMarkerSet != SNO_NONE)
    {
        sMessage += CRefString().Format("  Marker Set: %s. %s.",
            HyperlinkCreateFromSNO(SNOName(MARKERSET_SNO, tActorAdd.tMarkerOwner.snoMarkerSet), NULL).str(),
            GetBlameError(SNOName(MARKERSET_SNO, tActorAdd.tMarkerOwner.snoMarkerSet)).str());
    }

    sMessage += "\n";












}



// DWARF: both overloads are external (_Z28...f / _Z28...i, DW_AT_external) and have only inlined copies.
void sSetBaseAttributeIfDifferent(
    ActorCommonData & tACD,
    FastAttribKey tKey,
    float flValue)
{
    float flOldValue = tACD.AttributesGetFloat(tKey);
    if (flOldValue != flValue)
    {
        tACD.AttributesSetFloat(tKey, flValue);
    }
}

void sSetBaseAttributeIfDifferent(
    ActorCommonData & tACD,
    FastAttribKey tKey,
    int32 nValue)
{
    int32 nOldValue = tACD.AttributesGetInt(tKey);
    if (nOldValue != nValue)
    {
        tACD.AttributesSetInt(tKey, nValue);
    }
}




PortalResolveMethod ActorResolvePortalDestination(
    const Actor * ptActor,
    ResolvedPortalDestination & tResolvedDest)
{
    GizmoType eType = ActorGetGizmoType(ptActor->snoActor);
    if (!GizmoIsPortal(eType))
    {
        tResolvedDest.snoWorld = SNO_NONE;
        return PRM_INVALID;
    }

    if (eType == ACTORGIZMO_BOSS_PORTAL)
    {
        SNO snoBossEncounter = ActorDefinitionGetTagMapValueSNO(TAG_ACTOR_GIZMO_BOSS_ENCOUNTER_SNO, ptActor->snoActor);
        if (snoBossEncounter != SNO_NONE)
        {
            ScopedBossEncounterDefinitionLockR pdefBoss(snoBossEncounter);
            tResolvedDest.snoWorld = pdefBoss->arrWorlds[0];
            tResolvedDest.uEntranceGUID = pdefBoss->uEntranceGUID;
            tResolvedDest.snoDestLevelArea = ActorGetInheritedTagMapValueSNO(ptActor, TAG_ACTOR_GIZMO_PORTAL_DEST_LEVEL_AREA, TAG_MARKER_PORTAL_DEST_LEVEL_AREA);
            return PRM_BOSS_ENCOUNTER;
        }
    }
    else if (eType == ACTORGIZMO_PAGE_OF_FATE_PORTAL)
    {
        ActorCommonData * ptACDGizmo = ACDTryToGet(ACDFindByActor(ptActor->id));
        if (ptACDGizmo)
        {
            GBID gbidItem = ptACDGizmo->AttributesGetInt(FastAttribKey(ATTRIBUTE_PAGE_OF_FATE_ITEM));
            if (gbidItem != GBID_NONE)
            {




                ScopedItemDefinitionLockR pdefItem(gbidItem);
                tResolvedDest.snoWorld = pdefItem->snoWorldPageOfFate[0];
                tResolvedDest.uEntranceGUID = ActorGetInheritedTagMapValueDWORD(ptActor, TAG_ACTOR_GIZMO_PORTAL_ENTRANCE_NUMBER, TAG_MARKER_PORTAL_ENTRANCE_NUMBER);

                tResolvedDest.snoDestLevelArea = pdefItem->snoLevelAreaPageOfFatePortal;
            }
        }
        return PRM_PAGE_OF_FATE;
    }


    SSceneID idSSceneSCD = ACDGetCurrentSSceneID(ACDGet(ptActor->idACD));
    SScene * ptSScene = SSceneTryToGet(idSSceneSCD);
    if (!ptSScene)
    {
        tResolvedDest.snoWorld = SNO_NONE;
        return PRM_INVALID;
    }

    SceneCommonData & tSCD = SceneCommonDataGet(idSSceneSCD);

    PortalResolveMethod eMethod = ActorGetInheritedTagMapValueInt(ptActor, TAG_ACTOR_GIZMO_PORTAL_RESOLVE_METHOD, TAG_MARKER_PORTAL_RESOLVE_METHOD);


    if (eMethod == PRM_PREV_WORLD &&
        WorldGetDungeonFinderLevel(WorldCommonGetWorldSNO(tSCD.idSWorld)) == 1)
    {
        eMethod = PRM_DYNAMIC_RETURN_LOCATION;
    }


    if (eType == ACTORGIZMO_DUNGEON_PORTAL &&
        WorldIsDungeonFinderPlaceHolder(WorldCommonGetWorldSNO(tSCD.idSWorld)))
    {

        SWorldID idSWorldFirstFloor = WorldCommonFindBySNO(GlobalSNOGet(GLOBALSNO_WORLD_DUNGEON_FINDER_LEVEL_01), TRUE);
        SWorld * ptSWorld = SWorldTryToGet(idSWorldFirstFloor);
        if (ptSWorld)
        {
            tResolvedDest = ptSWorld->tDynamicReturnLocation;
            return PRM_DYNAMIC_RETURN_LOCATION;
        }
    }

    switch (eMethod)
    {
    case PRM_SPECIFIC_WORLD:
        tResolvedDest.snoWorld = ActorGetInheritedTagMapValueSNO(ptActor, TAG_ACTOR_GIZMO_PORTAL_SPECIFIED_WORLD, TAG_MARKER_PORTAL_SPECIFIED_WORLD);

        tResolvedDest.uEntranceGUID = ActorGetInheritedTagMapValueDWORD(ptActor, TAG_ACTOR_GIZMO_PORTAL_ENTRANCE_NUMBER, TAG_MARKER_PORTAL_ENTRANCE_NUMBER);

        tResolvedDest.snoDestLevelArea = ActorGetInheritedTagMapValueSNO(ptActor, TAG_ACTOR_GIZMO_PORTAL_DEST_LEVEL_AREA, TAG_MARKER_PORTAL_DEST_LEVEL_AREA);
        break;


    case PRM_CURRENT_WORLD:
        tResolvedDest.snoWorld = WorldCommonGetWorldSNO(tSCD.idSWorld);
        tResolvedDest.uEntranceGUID = ActorGetInheritedTagMapValueDWORD(ptActor, TAG_ACTOR_GIZMO_PORTAL_ENTRANCE_NUMBER, TAG_MARKER_PORTAL_ENTRANCE_NUMBER);

        tResolvedDest.snoDestLevelArea = (tSCD.tSceneSpec.snoNextLevelArea != SNO_NONE) ? tSCD.tSceneSpec.snoNextLevelArea : ActorGetInheritedTagMapValueSNO(ptActor, TAG_ACTOR_GIZMO_PORTAL_DEST_LEVEL_AREA, TAG_MARKER_PORTAL_DEST_LEVEL_AREA);
        break;



    case PRM_PREV_WORLD:
        tResolvedDest.snoWorld = tSCD.tSceneSpec.snoPrevWorld;
        tResolvedDest.uEntranceGUID = tSCD.tSceneSpec.uPrevEntranceGUID;
        tResolvedDest.snoDestLevelArea = tSCD.tSceneSpec.snoPrevLevelArea;
        break;

    case PRM_NEXT_WORLD:
        tResolvedDest.snoWorld = tSCD.tSceneSpec.snoNextWorld;
        tResolvedDest.uEntranceGUID = tSCD.tSceneSpec.uNextEntranceGUID;
        tResolvedDest.snoDestLevelArea = tSCD.tSceneSpec.snoNextLevelArea;
        break;

    case PRM_SAVE_POINT_PORTAL:
        {
            ActGetCurrentStartLocation(tResolvedDest);


            uint32 uEntranceGUID = ActorGetInheritedTagMapValueDWORD(ptActor, TAG_ACTOR_GIZMO_PORTAL_ENTRANCE_NUMBER, TAG_MARKER_PORTAL_ENTRANCE_NUMBER);


            if (uEntranceGUID != (uint32)-1)
            {
                tResolvedDest.uEntranceGUID = uEntranceGUID;
            }
        }
        break;






    case PRM_DYNAMIC_RETURN_LOCATION:
        {

            SWorld * ptSWorld = SWorldTryToGet(tSCD.idSWorld);
            if (ptSWorld && ptSWorld->tDynamicReturnLocation.IsValid())
            {
                tResolvedDest.snoWorld = ptSWorld->tDynamicReturnLocation.snoWorld;
                tResolvedDest.snoDestLevelArea = ptSWorld->tDynamicReturnLocation.snoDestLevelArea;
                tResolvedDest.uEntranceGUID = ptSWorld->tDynamicReturnLocation.uEntranceGUID;
            }
        }
        break;
    }












    StartingLocationGUIDIsValid(tResolvedDest.uEntranceGUID);











    return eMethod;
}



void sInitFadeAttribute(Actor & tActor)
{

    if (tActor.tMarkerOwner.snoMarkerSet != SNO_NONE && tActor.tMarkerOwner.nMarkerIndex != -1)
    {

        ScopedMarkerSetDefinitionLockR pdefMarkerSet(tActor.tMarkerOwner.snoMarkerSet);

        int32 nIndex = tActor.tMarkerOwner.nMarkerIndex;

        const Marker & tMarker = pdefMarkerSet->tMarkerSet[nIndex];
        int32 nFadeGroup = TagMapGetInt(tMarker.hTagMap, TAG_MARKER_FADE_GROUP);
        if (nFadeGroup != -1)
        {
            ACDID idACD = ACDFindByActor(tActor.id);
            ActorCommonData & tACD = ACDGet(idACD);
            tACD.AttributesSetInt(MakeAttribKey(ATTRIBUTE_RACTOR_FADE_GROUP, -1), nFadeGroup);
        }
    }
}



void sAttributesInitForGizmo(
    Actor * ptActor,
    ActorCommonData & tACD,
    const ActorAddStruct & tActorAdd,
    const int32 nLevel)
{
    ScopedActorDefinitionLockR ptActorDef(tActorAdd.snoActor);





    sSetBaseAttributeIfDifferent(tACD, FastAttribKey(ATTRIBUTE_LEVEL), nLevel);

    SNO snoPower = TagMapGetSNO(ptActorDef->hTagMap, TAG_ACTOR_GIZMO_POWER_ON_OPERATE);

    if (snoPower != SNO_NONE)
    {
        tACD.AttributesSetInt(MakeAttribKey(ATTRIBUTE_SKILL, snoPower), 1);
    }


    tACD.SetHitPointsMax(0.001f);
    tACD.SetHitPoints(0.001f, TRUE, FALSE);

    GizmoType nGizmoType = (GizmoType)TagMapGetInt(ptActorDef->hTagMap, TAG_ACTOR_GIZMO_TYPE);
    switch (nGizmoType)
    {
    case ACTORGIZMO_GATE:
        UpdateFunctionInitialize(UPDATE_GIZMOS, ptActor->id, ptActor->tGizmoUpdateData);
        break;

    case ACTORGIZMO_HEALING_WELL:
        {
            int32 nNumCharges = TagMapGetInt(ptActorDef->hTagMap, TAG_ACTOR_GIZMO_NUM_CHARGES);
            tACD.AttributesSetInt(FastAttribKey(ATTRIBUTE_GIZMO_CHARGES), nNumCharges);
            tACD.AttributesSetInt(FastAttribKey(ATTRIBUTE_GIZMO_STATE), 0);

            UpdateFunctionInitialize(UPDATE_GIZMOS, ptActor->id, ptActor->tGizmoUpdateData);
            ServerQueueUpdateFunction(6, ptActor->tGizmoUpdateData);
        }
        break;

    case ACTORGIZMO_PORTAL:
    case ACTORGIZMO_DUNGEON_PORTAL:
        {
            ResolvedPortalDestination tResolvedDest;
            ActorResolvePortalDestination(ptActor, tResolvedDest);
            SPortalSpecifierInitialize(ptActor, tResolvedDest);

            SNO snoChallenge = TagMapGetSNO(ptActorDef->hTagMap, TAG_ACTOR_GIZMO_CHALLENGE_SNO);
            if (snoChallenge != SNO_NONE)
            {

                SNO snoScript = GlobalSNOGet(GLOBALSNO_SCRIPT_CHALLENGE_TUTORIAL);
                LuaScriptAttach(SCRIPT_ATTACH_ACTOR, ptActor->idACD, snoScript, tActorAdd.tMarkerOwner.nAdventureParentIndex);
                ptActor->listSNOScripts.AddTail_Tracked(snoScript);
            }
        }
        break;







    case ACTORGIZMO_TOWN_PORTAL:
        {
            ResolvedPortalDestination tResolvedDest;
            PortalResolveMethod eMethod = ActorResolvePortalDestination(ptActor, tResolvedDest);

            SPortalSpecifierInitialize(ptActor, tResolvedDest);


            if (!(tActorAdd.dwActorAddFlags & (1 << ACTORADD_IS_FROM_INACTIVE)))
            {

                int32 nLifetime = ActorDefinitionGetTagMapValueInt(TAG_ACTOR_GIZMO_PORTAL_LIFETIME, ptActor->snoActor);
                if (nLifetime > 0)
                {
                    tACD.AttributesSetInt(FastAttribKey(ATTRIBUTE_SUMMON_EXPIRATION_TICK), GameGetServerTime() + nLifetime);
                    ActorTriggerAttributeUpdate(ptActor->id);
                }


                if (eMethod == PRM_SAVE_POINT_PORTAL)
                {
                    SetCurrentSavePointPortalReturn(tACD);
                }
            }
        }
        break;


    case ACTORGIZMO_BOSS_PORTAL:
        {
            UpdateFunctionInitialize(UPDATE_GIZMOS, ptActor->id, ptActor->tGizmoUpdateData);
            ServerQueueUpdateFunction(6, ptActor->tGizmoUpdateData);
            SNO snoEncounter = ActorDefinitionGetTagMapValueSNO(TAG_ACTOR_GIZMO_BOSS_ENCOUNTER_SNO, tACD.snoActor);
            if (snoEncounter != SNO_NONE &&

                SNOFileExists(BOSS_ENCOUNTER_SNO, snoEncounter))
            {




                ResolvedPortalDestination tResolvedDest;
                ActorResolvePortalDestination(ptActor, tResolvedDest);
                SPortalSpecifierInitialize(ptActor, tResolvedDest);
            }
            tACD.AttributesSetInt(FastAttribKey(ATTRIBUTE_GIZMO_STATE), 0);
        }
        break;



    case ACTORGIZMO_SECRET_PORTAL:
        {
            ResolvedPortalDestination tResolvedDest;
            ActorResolvePortalDestination(ptActor, tResolvedDest);

            SPortalSpecifierInitialize(ptActor, tResolvedDest);
        }
        break;

    case ACTORGIZMO_DOOR:
        {
            tACD.AttributesSetInt(FastAttribKey(ATTRIBUTE_DOOR_LOCKED), 0);
            if (tActorAdd.dwActorAddFlags & (1 << ACTORADD_GIZMO_DOOR_OPEN))
            {
                tACD.AttributesSetInt(FastAttribKey(ATTRIBUTE_GIZMO_STATE), 1);
                GizmoRestoreState(tACD);
            }
            else
            {
                tACD.AttributesSetInt(FastAttribKey(ATTRIBUTE_GIZMO_STATE), 0);
            }
        }
        break;

    case ACTORGIZMO_BREAKABLEDOOR:
        {
            int32 nMaxDamageStates = ActorDefinitionGetTagMapValueInt(TAG_ACTOR_GIZMO_NUM_DAMAGE_STATES, ptActor->snoActor);
            tACD.AttributesSetInt(FastAttribKey(ATTRIBUTE_DAMAGE_STATE_MAX), nMaxDamageStates);
        }
        break;

    case ACTORGIZMO_SAVEPOINT_PORTAL:
        {
            GizmoReturnPointPortalInitialize(tACD);
        }
        break;


    case ACTORGIZMO_WAYPOINT:
        {
            if (!GizmoIsWaypointActivated(tACD, TRUE, NULL, NULL))
            {
                ActorGlobals * ptActorGlobals = SGameManager::GameGet().ptActorGlobals;
                ptActorGlobals->listInactiveWaypoints.AddTail_Tracked(ptActor->id);
            }
        }
        break;

    case ACTORGIZMO_BANNER:
        {
            UpdateFunctionInitialize(UPDATE_GIZMOS, ptActor->id, ptActor->tGizmoUpdateData);
            ServerQueueUpdateFunction(60, ptActor->tGizmoUpdateData);
        }
        break;







    case ACTORGIZMO_POOL_OF_REFLECTION:
    case ACTORGIZMO_OBJECTIVE_FINDER:
        {

            int32 nNumCharges = TagMapGetInt(ptActorDef->hTagMap, TAG_ACTOR_GIZMO_NUM_CHARGES);
            tACD.AttributesSetInt(FastAttribKey(ATTRIBUTE_GIZMO_CHARGES), nNumCharges);
            tACD.AttributesSetInt(FastAttribKey(ATTRIBUTE_GIZMO_STATE), 0);
        }
        break;

    case ACTORGIZMO_MULTICLICK:
        tACD.AttributesSetInt(FastAttribKey(ATTRIBUTE_GIZMO_STATE), 0);
        break;
    }



    sInitFadeAttribute(*ptActor);

    BOOL bDisableNonFriendlyOperation = ActorDefinitionGetTagMapValueInt(TAG_ACTOR_DISABLE_NONFRIENDLY_OPERATION, tACD.snoActor);
    if (!bDisableNonFriendlyOperation &&
        nGizmoType != ACTORGIZMO_ITEM_INTERNAL &&
        nGizmoType != ACTORGIZMO_SPAWNER &&
        nGizmoType != ACTORGIZMO_DESTROYABLE_OBJECT &&
        nGizmoType != ACTORGIZMO_REFORMING_DESTROYABLE_OBJECT &&
        nGizmoType != ACTORGIZMO_BOSS_PORTAL &&
        nGizmoType != -1 &&
        !(GameTypeIsPvP() && nGizmoType == ACTORGIZMO_PORTAL_DESTINATION))
    {

        tACD.SetTeam(TEAM_NEUTRAL);
    }



    tACD.AttributesSetFloat(FastAttribKey(ATTRIBUTE_MOVEMENT_SCALAR), 1.0f);
    tACD.AttributesSetFloat(FastAttribKey(ATTRIBUTE_ATTACKS_PER_SECOND), 1.0f);
    tACD.AttributesSetFloat(FastAttribKey(ATTRIBUTE_CASTING_SPEED), 1.0f);


    if (tActorAdd.dwActorAddFlags & (1 << ACTORADD_NO_DAMAGE))
    {
        tACD.SetTakesNoDamage(TRUE);
    }

    if (tActorAdd.dwActorAddFlags & (1 << ACTORADD_DOES_FAKE_DAMAGE))
    {
        tACD.SetDoesFakeDamage(TRUE);
    }

    int32 nTieredLootRunLevel = SWorldGetTieredDungeonFinderLevel(tActorAdd.tPlace.idSWorld);
    if (nTieredLootRunLevel > 0)
    {
        tACD.AttributesSetInt(FastAttribKey(ATTRIBUTE_DROPS_NO_LOOT), TRUE);
    }

    GizmoCreate(tACD);
}




int32 ActorGetCorrectLevel(
    const SNOName & tActorSNOName,
    int32 nOverrideLevel)
{
    int32 nLevel = nOverrideLevel;

    if (nLevel != -1)
    {
        return nLevel;
    }

    if (tActorSNOName.snoHandle != SNO_NONE)
    {
        nLevel = MonsterGetLevel(tActorSNOName);
    }

    if (nLevel == -1)
    {
        nLevel = GameGetMonsterLevel();
        nLevel = Math::Pin(nLevel, 1, 99);
    }

    return nLevel;
}


// Body follows the affix-attribute loop in RandomMonster.cpp; the columns of 2095-2122 fit that
// spelling.
static BOOL sApplyMonsterAttribute(
    ActorCommonData & tACD,
    const AttributeSpecifier * ptAttribSpec)
{
    if (ptAttribSpec->eAttribute == ATTRIBUTE_NONE)
    {
        return FALSE;
    }

    BBECallbackData tCallback;
    InitBBECallbackData(&tCallback);
    tCallback.nLevel = tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_LEVEL));
    tCallback.idFastAttrib = tACD.idFastAttrib;

    float flRetval;
    BOOL bSuccess = BBEEvaluate(
        ptAttribSpec->pbAttributeFormula,
        ptAttribSpec->serAttributeFormula.dwSize,
        BBEIdentifierEvaluate,
        BBEFunctions,
        NUM_BBEFUNCTIONS,
        NULL,
        &tCallback,
        flRetval);






    FastAttribKey keyAttribute = MakeAttribKey(ptAttribSpec->eAttribute, ptAttribSpec->nParam);
    AttributeDataType eAttribType = KeyGetDataType(keyAttribute);

    if (eAttribType == ATTRIBUTEDT_FLOAT)
    {
        sSetBaseAttributeIfDifferent(tACD, keyAttribute, flRetval);
    }
    else
    {
        sSetBaseAttributeIfDifferent(tACD, keyAttribute, Math::FloatToInt(flRetval));
    }

    // Inferred: bSuccess (2106) has no location and no use in the inlined copy.
    return TRUE;
}

// Inferred loop bound: MonsterDefinition::ptAttributes has 1 element; the inlined copy runs once.

static void sApplyMonsterAttributesFromSpreadSheet(
    ActorCommonData & tACD,
    const MonsterDefinition * pdefMonster)
{
    for (int32 i = 0; i < 1; i++)
    {
        const AttributeSpecifier * ptAttribSpec = &pdefMonster->ptAttributes[i];
        BOOL bContinue = sApplyMonsterAttribute(tACD, ptAttribSpec);
        if (!bContinue)
        {
            break;
        }
    }
}




BOOL sub_71007d5020(SWorldID idSWorld);  // unnamed 2.7.7 function (0x71007d5020)
void ActorGetMonsterHPData(SNO snoMonster, int32 nLevel, MonsterRarity eMonsterRarity, float & flHP, float & flHPDelta, const BOOL bAvenger, const BOOL bIsTieredLootRunMonster, const BOOL bIsSetDungeon, const BOOL bUnknown9)
{









    if (snoMonster == SNO_NONE)
    {
        flHP = 0.0f;
        flHPDelta = 0.0f;
        return;
    }
    if (bAvenger)
    {
        eMonsterRarity = MR_RARE;
    }

    if (bIsSetDungeon || bUnknown9)
    {
        nLevel = 70;
    }

    switch (eMonsterRarity)
    {
    case MR_CHAMPION:
    case MR_RARE:
    case MR_RARE_MINION:
        {
            const MonsterDefinition * pdefMonster = (const MonsterDefinition *)gptGroupDefMonster->LockR(snoMonster, FALSE);

            if (eMonsterRarity == MR_RARE)
            {
                flHP = pdefMonster->flHPRare;
                flHPDelta = pdefMonster->flHPDeltaRare;
            }
            else if (eMonsterRarity == MR_CHAMPION)
            {
                flHP = pdefMonster->flHPChampion;
                flHPDelta = pdefMonster->flHPDeltaChampion;
            }
            else
            {
                flHP = pdefMonster->flHPMinion;
                flHPDelta = pdefMonster->flHPDeltaMinion;
            }

            const GameBalanceDefinition * ptMonsterLevelGBDef = MonsterLevelTableLockR();
            if (pdefMonster->dwFlags & (1 << MONSTERDEF_HIRELING_OR_PET_BIT))
            {
                flHP *= ptMonsterLevelGBDef->tMonsterLevelTable.ptMonsterLevels[nLevel - 1].flAttributesHirelingHitPoints;
                flHPDelta *= ptMonsterLevelGBDef->tMonsterLevelTable.ptMonsterLevels[nLevel - 1].flAttributesHirelingHPRange;
            }
            else
            {
                flHP *= ptMonsterLevelGBDef->tMonsterLevelTable.ptMonsterLevels[nLevel - 1].flAttributesHitPoints;
                flHPDelta *= ptMonsterLevelGBDef->tMonsterLevelTable.ptMonsterLevels[nLevel - 1].flAttributesHPRange;
            }

            if (!(bIsSetDungeon || bUnknown9))
            {
                ScopedGlobalDefinitionLockR pdefGlobal;
                const DifficultyTuningParams & tTuningParams = pdefGlobal->ptServerData->tDifficultyParams[GameGetDifficultyParamsLevel(bIsTieredLootRunMonster)];
                flHP *= tTuningParams.flEliteMonsterHPScalar;
                flHPDelta *= tTuningParams.flEliteMonsterHPScalar;
            }

            MonsterLevelTableUnlockR(ptMonsterLevelGBDef);

            gptGroupDefMonster->UnlockR((void *&)pdefMonster);
        }
        break;

    case MR_UNIQUE:
    case MR_BOSS:
        flHP = GetHirelingMonsterLevelAttribute(nLevel, snoMonster, GBA_HITPOINTS, GBA_HIRELING_HITPOINTS);
        flHPDelta = GetHirelingMonsterLevelAttribute(nLevel, snoMonster, GBA_HPRANGE, GBA_HIRELING_HPRANGE);
        if (!(bIsSetDungeon || bUnknown9))
        {
            ScopedGlobalDefinitionLockR pdefGlobal;
            const DifficultyTuningParams & tTuningParams = pdefGlobal->ptServerData->tDifficultyParams[GameGetDifficultyParamsLevel(bIsTieredLootRunMonster)];
            flHP *= tTuningParams.flUniqueAndBossMonsterHPScalar;
            flHPDelta *= tTuningParams.flUniqueAndBossMonsterHPScalar;
        }
        break;

    default:

        flHP = GetHirelingMonsterLevelAttribute(nLevel, snoMonster, GBA_HITPOINTS, GBA_HIRELING_HITPOINTS);
        flHPDelta = GetHirelingMonsterLevelAttribute(nLevel, snoMonster, GBA_HPRANGE, GBA_HIRELING_HPRANGE);
        break;
    }

    if (!(bIsSetDungeon || bUnknown9))
    {
        ScopedMonsterDefinitionLockR pdefMonster(snoMonster);
        if (!(pdefMonster->dwFlags & (1 << MONSTERDEF_HIRELING_OR_PET_BIT)))
        {
            float flHandicapScalar = bIsTieredLootRunMonster ? 1.0f : ActorGetHandicapHealthScalar();
            flHandicapScalar *= GetMonsterLevelAttribute(nLevel, SNO_NONE, GBA_CONSOLE_HEALTH_SCALAR);
            flHP *= flHandicapScalar;
            flHPDelta *= flHandicapScalar;
        }
    }
}




























































uint64 ActorCalculateXPForMonster(const ActorCommonData & tACDVictim, int32 nPlayerLevel)
{
    SNO snoMonster = tACDVictim.GetMonsterSNO();


    uint64 nExpGranted = (uint64)GetMonsterLevelAttribute(nPlayerLevel, snoMonster, GBA_EXPERIENCE);


    if (tACDVictim.eMonsterRarity == MR_CHAMPION)
    {

        nExpGranted *= 3;
    }
    else if (tACDVictim.eMonsterRarity == MR_RARE)
    {
        nExpGranted *= 5;
    }



    int32 nNumPlayers = PlayerGetNumPlayersForDifficulty();

    int32 nPlayerMult = Math::Max(nNumPlayers - 1, 0);

    ScopedGlobalDefinitionLockR pdefGlobal;

    const DifficultyTuningParams & tTuningParams = pdefGlobal->ptServerData->tDifficultyParams[GameGetDifficultyParamsLevel(FALSE)];

    nExpGranted += (int64)((float)nExpGranted * nPlayerMult * tTuningParams.flMonsterXPPercentPerPlayer);


    return nExpGranted;
}


static void sAttributesInitForMonster(
    ActorCommonData & tACD,
    const ActorAddStruct & tActorAdd)
{
    ActorID idActor = (ActorID)ACDGetOwner(tACD.id);
    Actor * ptActor = ActorTryToGet(idActor);
    if (!ptActor)
    {
        return;
    }

    RNG tOrigRNG = SGameGlobalsGet().rng;
    SGameGlobalsGet().rng = ptActor->rndOriginal;

	int32 nLevel = ActorGetCorrectLevel(tActorAdd.tSNOName, tActorAdd.nLevel);

    nLevel = Math::Pin(nLevel, 1, 99);




















    SNOName tMonsterSNOName;

    BOOL bHasMonsterSNO = MonsterGetSNONameFromActor(tActorAdd.tSNOName.snoHandle, tMonsterSNOName);

    SNO snoMonster = tMonsterSNOName.snoHandle;

	int64 nExpGranted = (int64)GetMonsterLevelAttribute(nLevel, snoMonster, GBA_EXPERIENCE);
    if (nExpGranted <= 0)
    {
        sSetBaseAttributeIfDifferent(tACD, FastAttribKey(ATTRIBUTE_GRANTS_NO_XP), TRUE);
    }

    int32 nGoldGranted = MonsterGetGoldGranted(tActorAdd.tSNOName);


    switch (tActorAdd.eMonsterRarity)
    {
        case MR_CHAMPION:
            nExpGranted *= 3;
            break;

        case MR_RARE:
            nExpGranted *= 5;
            break;
    }

    int32 nTieredLootRunLevel = SWorldGetTieredDungeonFinderLevel(tActorAdd.tPlace.idSWorld);
    BOOL bIsTieredRift = nTieredLootRunLevel >= 0;
    BOOL bIsSetDungeon = SWorldIsSetDungeon(tActorAdd.tPlace.idSWorld);
    BOOL bUnknown9 = sub_71007d5020(tActorAdd.tPlace.idSWorld);
    float flHPMin;
    float flHPDelta;
    ActorGetMonsterHPData(snoMonster, nLevel, tActorAdd.eMonsterRarity, flHPMin, flHPDelta, tActorAdd.bAvenger, bIsTieredRift, bIsSetDungeon, bUnknown9);

    float flHitPoints = RandRangeFloat_Internal(SGameGlobalsGet().rng, flHPMin, flHPDelta);  // all rows of 2428 are at column 25: a macro in the original






    int32 nNumPlayers = PlayerGetNumPlayersForDifficulty();

    ScopedGlobalDefinitionLockR pdefGlobal;

    const DifficultyTuningParams &tTuningParams = pdefGlobal->ptServerData->tDifficultyParams[GameGetDifficultyParamsLevel(bIsTieredRift)];

    int32 nPlayerMult = Math::Max(nNumPlayers - 1, 0);

    float flHandicapScalar = 1.0f;
    ScopedMonsterDefinitionLockR pdefMonster(snoMonster);
    if (pdefMonster->dwFlags & (1 << MONSTERDEF_HIRELING_OR_PET_BIT))
    {
        // Inferred: the hireling path reaches line 2506 with nPlayerMult 0 (mov w25, wzr at +0x38c, no row of its own).
        nPlayerMult = 0;
    }
    else
    {
        flHandicapScalar = GameGetHandicapDamageScalar();



        flHandicapScalar *= GetMonsterLevelAttribute(nLevel, SNO_NONE, GBA_CONSOLE_DAMAGE_SCALAR);


        if (bIsTieredRift)
        {
            float flDungeonFinderHPScalar = 1.0f;
            float flDungeonFinderDamageScalar = 1.0f;
            float flDungeonFinderXPScalar = 1.0f;


            {
                ScopedGameBalanceDefinitionLockR pdefGB(GlobalSNOGet(GLOBALSNO_GB_TIERED_LOOT_RUN_LEVELS));

                int32 nGameBalanceIndex = Math::Max(nTieredLootRunLevel, 0);  // no code: nTieredLootRunLevel >= 0 here (idiom of Loot.cpp:2465)
                nGameBalanceIndex = Math::Min(nGameBalanceIndex, (int32)pdefGB->tTieredLootRunLevelTable.serTieredLootRunLevels.ElementCount(sizeof(TieredLootRunLevel)) - 1);
                // the branch sits on the && (column 44): the >= 0 test folds away after the Max above
                if (nGameBalanceIndex >= 0 && nGameBalanceIndex < (int32)pdefGB->tTieredLootRunLevelTable.serTieredLootRunLevels.ElementCount(sizeof(TieredLootRunLevel)))
                {
                    const TieredLootRunLevel & tTieredLootRunLevel = pdefGB->tTieredLootRunLevelTable.ptTieredLootRunLevels[nGameBalanceIndex];
                    flDungeonFinderHPScalar = tTieredLootRunLevel.tLootRunValues.flHealthScalar;
                    flDungeonFinderDamageScalar = tTieredLootRunLevel.tLootRunValues.flDamageScalar;
                    flDungeonFinderXPScalar = tTieredLootRunLevel.tLootRunValues.flXPBonus;
                }
            }






            flHitPoints *= flDungeonFinderHPScalar;
            nExpGranted = (int64)(nExpGranted * Math::Max(0.0f, flDungeonFinderXPScalar));
            flHandicapScalar = flDungeonFinderDamageScalar;
            FastAttribSetValueInt(tACD.idFastAttrib, ATTRIBUTE_IN_TIERED_LOOT_RUN_LEVEL, nTieredLootRunLevel);


            nNumPlayers = NephalemRiftManager::RiftPlayerCount();
            nPlayerMult = Math::Max(nNumPlayers - 1, 0);
        }
        else if (bIsSetDungeon)
        {

            nNumPlayers = SetEncounterManager::GetNumberOfParticipatingPlayers();
            nPlayerMult = Math::Max(nNumPlayers - 1, 0);
            flHandicapScalar = 1.0f;
            FastAttribSetValueInt(tACD.idFastAttrib, ATTRIBUTE_IN_SET_DUNGEON_WORLD, TRUE);
        }
        else if (bUnknown9)
        {
            FastAttribSetValueInt(tACD.idFastAttrib, ATTRIBUTE_IN_SWARM_RIFT_WORLD, TRUE);
            flHandicapScalar = 1.0f;
        }
    }


    if(bIsTieredRift)
    {

        ScopedGameBalanceDefinitionLockR pdefGB(GlobalSNOGet(GLOBALSNO_GB_TIERED_LOOT_RUN_LEVELS));
        const TieredLootRunLevel & tTieredLootRunLevel = pdefGB->tTieredLootRunLevelTable.ptTieredLootRunLevels[nTieredLootRunLevel];
        float flHPScalar = tTieredLootRunLevel.tLootRunValues.flMonsterHPPerAdditionalPlayer;

        flHitPoints += flHitPoints * (nPlayerMult * flHPScalar);
    }
    else
    {
        flHitPoints += flHitPoints * (nPlayerMult * tTuningParams.flMonsterHPPercentPerPlayer);
    }


    nExpGranted += (int64)(nExpGranted * nPlayerMult * tTuningParams.flMonsterXPPercentPerPlayer);

    const GameBalanceDefinition* ptMonsterLevelGBDef = MonsterLevelTableLockR();

    sSetBaseAttributeIfDifferent(tACD, FastAttribKey(ATTRIBUTE_LEVEL), nLevel);

    tACD.SetExperienceGranted(nExpGranted);

    sSetBaseAttributeIfDifferent(tACD, FastAttribKey(ATTRIBUTE_GOLD_GRANTED), nGoldGranted);
	sSetBaseAttributeIfDifferent(tACD, ATTRIBUTE_VITALITY, GetMonsterLevelAttribute(nLevel, pdefMonster, GBA_VITALITY, ptMonsterLevelGBDef));
	sSetBaseAttributeIfDifferent(tACD, ATTRIBUTE_HITPOINTS_MAX, flHitPoints);

	float flDmgMult = flHandicapScalar + nPlayerMult * tTuningParams.flMonsterDamagePercentPerPlayer;

	sSetBaseAttributeIfDifferent(tACD, MakeAttribKey(ATTRIBUTE_DAMAGE_MIN, DAMAGE_TYPE_PHYSICAL), flDmgMult * GetHirelingMonsterLevelAttribute(nLevel, pdefMonster, GBA_DAMAGE, GBA_HIRELING_DAMAGE, ptMonsterLevelGBDef));
    sSetBaseAttributeIfDifferent(tACD, MakeAttribKey(ATTRIBUTE_DAMAGE_DELTA, DAMAGE_TYPE_PHYSICAL), flDmgMult * GetHirelingMonsterLevelAttribute(nLevel, pdefMonster, GBA_DAMAGERANGE, GBA_HIRELING_DAMAGE_RANGE, ptMonsterLevelGBDef));
	sSetBaseAttributeIfDifferent(tACD, MakeAttribKey(ATTRIBUTE_DAMAGE_MIN, DAMAGE_TYPE_FIRE), flDmgMult * GetMonsterLevelAttribute(nLevel, pdefMonster, GBA_DAMAGE_FIRE, ptMonsterLevelGBDef));
	sSetBaseAttributeIfDifferent(tACD, MakeAttribKey(ATTRIBUTE_DAMAGE_DELTA, DAMAGE_TYPE_FIRE), flDmgMult * GetMonsterLevelAttribute(nLevel, pdefMonster, GBA_DAMAGERANGE_FIRE, ptMonsterLevelGBDef));
	sSetBaseAttributeIfDifferent(tACD, MakeAttribKey(ATTRIBUTE_DAMAGE_MIN, DAMAGE_TYPE_LIGHTNING), flDmgMult * GetMonsterLevelAttribute(nLevel, pdefMonster, GBA_DAMAGE_LIGHTNING, ptMonsterLevelGBDef));
	sSetBaseAttributeIfDifferent(tACD, MakeAttribKey(ATTRIBUTE_DAMAGE_DELTA, DAMAGE_TYPE_LIGHTNING), flDmgMult * GetMonsterLevelAttribute(nLevel, pdefMonster, GBA_DAMAGERANGE_LIGHTNING, ptMonsterLevelGBDef));
	sSetBaseAttributeIfDifferent(tACD, MakeAttribKey(ATTRIBUTE_DAMAGE_MIN, DAMAGE_TYPE_COLD), flDmgMult * GetMonsterLevelAttribute(nLevel, pdefMonster, GBA_DAMAGE_COLD, ptMonsterLevelGBDef));
	sSetBaseAttributeIfDifferent(tACD, MakeAttribKey(ATTRIBUTE_DAMAGE_DELTA, DAMAGE_TYPE_COLD), flDmgMult * GetMonsterLevelAttribute(nLevel, pdefMonster, GBA_DAMAGERANGE_COLD, ptMonsterLevelGBDef));
	sSetBaseAttributeIfDifferent(tACD, MakeAttribKey(ATTRIBUTE_DAMAGE_MIN, DAMAGE_TYPE_POISON), flDmgMult * GetMonsterLevelAttribute(nLevel, pdefMonster, GBA_DAMAGE_POISON, ptMonsterLevelGBDef));
	sSetBaseAttributeIfDifferent(tACD, MakeAttribKey(ATTRIBUTE_DAMAGE_DELTA, DAMAGE_TYPE_POISON), flDmgMult * GetMonsterLevelAttribute(nLevel, pdefMonster, GBA_DAMAGERANGE_POISON, ptMonsterLevelGBDef));
	sSetBaseAttributeIfDifferent(tACD, MakeAttribKey(ATTRIBUTE_DAMAGE_MIN, DAMAGE_TYPE_ARCANE), flDmgMult * GetMonsterLevelAttribute(nLevel, pdefMonster, GBA_DAMAGE_ARCANE, ptMonsterLevelGBDef));
	sSetBaseAttributeIfDifferent(tACD, MakeAttribKey(ATTRIBUTE_DAMAGE_DELTA, DAMAGE_TYPE_ARCANE), flDmgMult * GetMonsterLevelAttribute(nLevel, pdefMonster, GBA_DAMAGERANGE_ARCANE, ptMonsterLevelGBDef));
	sSetBaseAttributeIfDifferent(tACD, MakeAttribKey(ATTRIBUTE_DAMAGE_MIN, DAMAGE_TYPE_HOLY), flDmgMult * GetMonsterLevelAttribute(nLevel, pdefMonster, GBA_DAMAGE_HOLY, ptMonsterLevelGBDef));
	sSetBaseAttributeIfDifferent(tACD, MakeAttribKey(ATTRIBUTE_DAMAGE_DELTA, DAMAGE_TYPE_HOLY), flDmgMult * GetMonsterLevelAttribute(nLevel, pdefMonster, GBA_DAMAGERANGE_HOLY, ptMonsterLevelGBDef));

	sSetBaseAttributeIfDifferent(tACD, ATTRIBUTE_GETHIT_CURRENT, 0.0f);
	sSetBaseAttributeIfDifferent(tACD, ATTRIBUTE_GETHIT_MAX, GetMonsterLevelAttribute(nLevel, pdefMonster, GBA_GETHIT_MAX, ptMonsterLevelGBDef));
	sSetBaseAttributeIfDifferent(tACD, ATTRIBUTE_GETHIT_RECOVERY, GetMonsterLevelAttribute(nLevel, pdefMonster, GBA_GETHIT_RECOVERY, ptMonsterLevelGBDef));
	sSetBaseAttributeIfDifferent(tACD, ATTRIBUTE_GETHIT_DAMAGE, GetMonsterLevelAttribute(nLevel, pdefMonster, GBA_GETHIT_DAMAGE, ptMonsterLevelGBDef));
	sSetBaseAttributeIfDifferent(tACD, ATTRIBUTE_GETHIT_DAMAGE_SCALAR, GetMonsterLevelAttribute(nLevel, pdefMonster, GBA_GETHIT_DAMAGE_SCALAR, ptMonsterLevelGBDef));

	sSetBaseAttributeIfDifferent(tACD, ATTRIBUTE_WALKING_RATE, GetMonsterLevelAttribute(nLevel, pdefMonster, GBA_WALKSPEED, ptMonsterLevelGBDef));
	sSetBaseAttributeIfDifferent(tACD, ATTRIBUTE_RUNNING_RATE, GetMonsterLevelAttribute(nLevel, pdefMonster, GBA_RUNSPEED, ptMonsterLevelGBDef));
    sSetBaseAttributeIfDifferent(tACD, ATTRIBUTE_SPRINTING_RATE, GetMonsterLevelAttribute(nLevel, pdefMonster, GBA_SPRINTSPEED, ptMonsterLevelGBDef));
    sSetBaseAttributeIfDifferent(tACD, ATTRIBUTE_STRAFING_RATE, GetMonsterLevelAttribute(nLevel, pdefMonster, GBA_STRAFESPEED, ptMonsterLevelGBDef));
	sSetBaseAttributeIfDifferent(tACD, ATTRIBUTE_PROJECTILE_SPEED, GetMonsterLevelAttribute(nLevel, pdefMonster, GBA_PROJSPEED, ptMonsterLevelGBDef));

    ResourceType eResourceType = GetMonsterResourceType(pdefMonster);
    if (eResourceType != RESOURCE_TYPE_INVALID)
    {
        tACD.AttributesSetInt(ATTRIBUTE_RESOURCE_TYPE_PRIMARY, eResourceType);
        sSetBaseAttributeIfDifferent(tACD, MakeAttribKey(ATTRIBUTE_RESOURCE_MAX, eResourceType), GetMonsterLevelAttribute(nLevel, pdefMonster, GBA_PRIMARY_RESOURCE_BASE, ptMonsterLevelGBDef));
        sSetBaseAttributeIfDifferent(tACD, MakeAttribKey(ATTRIBUTE_RESOURCE_REGEN_PER_SECOND, eResourceType), GetMonsterLevelAttribute(nLevel, pdefMonster, GBA_PRIMARY_RESOURCE_REGEN, ptMonsterLevelGBDef));
    }

    sSetBaseAttributeIfDifferent(tACD, ATTRIBUTE_HITPOINTS_REGEN_PER_SECOND, GetHirelingMonsterLevelAttribute(nLevel, pdefMonster, GBA_HPREGEN, GBA_HIRELING_HPREGEN, ptMonsterLevelGBDef));
    sSetBaseAttributeIfDifferent(tACD, ATTRIBUTE_OOC_HEALTH_REGEN_PERCENT, GetMonsterAttribute(pdefMonster, GBA_OOC_HEALTH_REGEN_PERCENT));
	sSetBaseAttributeIfDifferent(tACD, MakeAttribKey(ATTRIBUTE_RESISTANCE, DAMAGE_TYPE_PHYSICAL), GetMonsterLevelAttribute(nLevel, pdefMonster, GBA_RESIST_PHYSICAL, ptMonsterLevelGBDef));
	sSetBaseAttributeIfDifferent(tACD, MakeAttribKey(ATTRIBUTE_RESISTANCE, DAMAGE_TYPE_FIRE), GetMonsterLevelAttribute(nLevel, pdefMonster, GBA_RESIST_FIRE, ptMonsterLevelGBDef));
	sSetBaseAttributeIfDifferent(tACD, MakeAttribKey(ATTRIBUTE_RESISTANCE, DAMAGE_TYPE_LIGHTNING), GetMonsterLevelAttribute(nLevel, pdefMonster, GBA_RESIST_LIGHTNING, ptMonsterLevelGBDef));
	sSetBaseAttributeIfDifferent(tACD, MakeAttribKey(ATTRIBUTE_RESISTANCE, DAMAGE_TYPE_COLD), GetMonsterLevelAttribute(nLevel, pdefMonster, GBA_RESIST_COLD, ptMonsterLevelGBDef));
	sSetBaseAttributeIfDifferent(tACD, MakeAttribKey(ATTRIBUTE_RESISTANCE, DAMAGE_TYPE_POISON), GetMonsterLevelAttribute(nLevel, pdefMonster, GBA_RESIST_POISON, ptMonsterLevelGBDef));
	sSetBaseAttributeIfDifferent(tACD, MakeAttribKey(ATTRIBUTE_RESISTANCE, DAMAGE_TYPE_ARCANE), GetMonsterLevelAttribute(nLevel, pdefMonster, GBA_RESIST_ARCANE, ptMonsterLevelGBDef));
	sSetBaseAttributeIfDifferent(tACD, MakeAttribKey(ATTRIBUTE_RESISTANCE, DAMAGE_TYPE_HOLY), GetMonsterLevelAttribute(nLevel, pdefMonster, GBA_RESIST_ARCANE, ptMonsterLevelGBDef));  // GBA_RESIST_ARCANE for holy, as in the original (0x47 at +0xf20)
    sSetBaseAttributeIfDifferent(tACD, MakeAttribKey(ATTRIBUTE_RESISTANCE, ATTRIBUTE_MOVEMENT_SCALAR_REDUCTION_RESISTANCE), GetMonsterLevelAttribute(nLevel, pdefMonster, GBA_RESIST_CHILL, ptMonsterLevelGBDef));
	sSetBaseAttributeIfDifferent(tACD, MakeAttribKey(ATTRIBUTE_RESISTANCE, ATTRIBUTE_STUNNED), GetMonsterLevelAttribute(nLevel, pdefMonster, GBA_RESIST_STUN, ptMonsterLevelGBDef));

	sSetBaseAttributeIfDifferent(tACD, MakeAttribKey(ATTRIBUTE_IMMUNITY, DAMAGE_TYPE_PHYSICAL), (int32)GetMonsterAttribute(pdefMonster, GBA_IMMUNITY_PHYSICAL));
	sSetBaseAttributeIfDifferent(tACD, MakeAttribKey(ATTRIBUTE_IMMUNITY, DAMAGE_TYPE_FIRE), (int32)GetMonsterAttribute(pdefMonster, GBA_IMMUNITY_FIRE));
	sSetBaseAttributeIfDifferent(tACD, MakeAttribKey(ATTRIBUTE_IMMUNITY, DAMAGE_TYPE_LIGHTNING), (int32)GetMonsterAttribute(pdefMonster, GBA_IMMUNITY_LIGHTNING));
	sSetBaseAttributeIfDifferent(tACD, MakeAttribKey(ATTRIBUTE_IMMUNITY, DAMAGE_TYPE_COLD), (int32)GetMonsterAttribute(pdefMonster, GBA_IMMUNITY_COLD));
	sSetBaseAttributeIfDifferent(tACD, MakeAttribKey(ATTRIBUTE_IMMUNITY, DAMAGE_TYPE_POISON), (int32)GetMonsterAttribute(pdefMonster, GBA_IMMUNITY_POISON));
	sSetBaseAttributeIfDifferent(tACD, MakeAttribKey(ATTRIBUTE_IMMUNITY, DAMAGE_TYPE_ARCANE), (int32)GetMonsterAttribute(pdefMonster, GBA_IMMUNITY_ARCANE));
	sSetBaseAttributeIfDifferent(tACD, MakeAttribKey(ATTRIBUTE_IMMUNITY, DAMAGE_TYPE_HOLY), (int32)GetMonsterAttribute(pdefMonster, GBA_IMMUNITY_ARCANE));  // GBA_IMMUNITY_ARCANE for holy, as in the original


	sSetBaseAttributeIfDifferent(tACD, ATTRIBUTE_IMMUNE_TO_KNOCKBACK, (int32)GetMonsterAttribute(pdefMonster, GBA_IMMUNITY_KNOCKBACK));
    sSetBaseAttributeIfDifferent(tACD, ATTRIBUTE_FEAR_IMMUNE, (int32)GetMonsterAttribute(pdefMonster, GBA_IMMUNITY_FEAR));
    sSetBaseAttributeIfDifferent(tACD, ATTRIBUTE_SLOWDOWN_IMMUNE, (int32)GetMonsterAttribute(pdefMonster, GBA_IMMUNITY_SLOW));
    sSetBaseAttributeIfDifferent(tACD, ATTRIBUTE_CHARM_IMMUNE, (int32)GetMonsterAttribute(pdefMonster, GBA_IMMUNITY_CHARM));
    sSetBaseAttributeIfDifferent(tACD, ATTRIBUTE_BLIND_IMMUNE, (int32)GetMonsterAttribute(pdefMonster, GBA_IMMUNITY_BLIND));
    sSetBaseAttributeIfDifferent(tACD, ATTRIBUTE_KNOCKBACK_WEIGHT, GetMonsterAttribute(pdefMonster, GBA_KNOCKBACK_WEIGHT));
    sSetBaseAttributeIfDifferent(tACD, ATTRIBUTE_FREEZE_CAPACITY, GetMonsterAttribute(pdefMonster, GBA_FREEZE_CAPACITY));
    sSetBaseAttributeIfDifferent(tACD, ATTRIBUTE_THAW_RATE, GetMonsterAttribute(pdefMonster, GBA_THAW_RATE));
    sSetBaseAttributeIfDifferent(tACD, ATTRIBUTE_FREEZE_IMMUNE, (int32)GetMonsterAttribute(pdefMonster, GBA_IMMUNITY_FREEZE));
    sSetBaseAttributeIfDifferent(tACD, ATTRIBUTE_STUN_IMMUNE, (int32)GetMonsterAttribute(pdefMonster, GBA_IMMUNITY_STUN));
    sSetBaseAttributeIfDifferent(tACD, ATTRIBUTE_ROOT_IMMUNE, (int32)GetMonsterAttribute(pdefMonster, GBA_IMMUNITY_ROOT));

	sSetBaseAttributeIfDifferent(tACD, ATTRIBUTE_INITIAL_COOLDOWN_MIN, (int32)(tTuningParams.flInitialCooldownScale * GetMonsterAttribute(pdefMonster, GBA_INITIAL_COOLDOWN_MIN)));
	sSetBaseAttributeIfDifferent(tACD, ATTRIBUTE_INITIAL_COOLDOWN_DELTA, (int32)(tTuningParams.flInitialCooldownScale * GetMonsterAttribute(pdefMonster, GBA_INITIAL_COOLDOWN_DELTA)));
	sSetBaseAttributeIfDifferent(tACD, ATTRIBUTE_ATTACK_COOLDOWN_MIN, (int32)(tTuningParams.flAttackCooldownScale * GetMonsterAttribute(pdefMonster, GBA_ATTACK_COOLDOWN_MIN)));
	sSetBaseAttributeIfDifferent(tACD, ATTRIBUTE_ATTACK_COOLDOWN_DELTA, (int32)(tTuningParams.flAttackCooldownScale * GetMonsterAttribute(pdefMonster, GBA_ATTACK_COOLDOWN_DELTA)));
    sSetBaseAttributeIfDifferent(tACD, ATTRIBUTE_CLOSING_COOLDOWN_MIN_TOTAL, (int32)(tTuningParams.flClosingCooldownScale * GetMonsterAttribute(pdefMonster, GBA_CLOSING_COOLDOWN_MIN)));
    sSetBaseAttributeIfDifferent(tACD, ATTRIBUTE_CLOSING_COOLDOWN_DELTA_TOTAL, (int32)(tTuningParams.flClosingCooldownScale * GetMonsterAttribute(pdefMonster, GBA_CLOSING_COOLDOWN_DELTA)));

    MonsterLevelTableUnlockR(ptMonsterLevelGBDef);



	tACD.AttributesSetFloat(ATTRIBUTE_MOVEMENT_SCALAR, 1.0f);
    tACD.AttributesSetFloat(ATTRIBUTE_MOVEMENT_SCALAR_REDUCTION_PERCENT, 0.0f);
	tACD.AttributesSetFloat(ATTRIBUTE_ATTACKS_PER_SECOND, 1.0f);
	tACD.AttributesSetFloat(ATTRIBUTE_CASTING_SPEED, 1.0f);


    ScopedActorDefinitionLockR pdefActor(tActorAdd.snoActor);
    tACD.AttributesSetInt(ATTRIBUTE_DAMAGE_STATE_MAX, TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_MAX_MONSTER_DAMAGE_STATES));

    float flHitChance = pdefGlobal->ptServerData->flHitChance;
    tACD.AttributesSetFloat(ATTRIBUTE_HIT_PERCENT, flHitChance);
    tACD.AttributesSetInt(ATTRIBUTE_BASE_ELEMENT, MonsterGetBaseElement(tActorAdd.tSNOName));

    float flAttackSpeedCap = pdefGlobal->ptServerData->flAttackSpeedItemPercentCap;
    tACD.AttributesSetFloat(ATTRIBUTE_ATTACKS_PER_SECOND_PERCENT_CAP, flAttackSpeedCap / 100.0f);

    float flCooldownMinTime = pdefGlobal->ptServerData->flCooldownMinimumTime;
    tACD.AttributesSetFloat(ATTRIBUTE_POWER_COOLDOWN_MIN_TIME, flCooldownMinTime);

    float flMovementScalarCap = Math::Max(1.0f, pdefGlobal->ptServerData->flMovementScalarCap);
    tACD.AttributesSetFloat(ATTRIBUTE_MOVEMENT_SCALAR_CAP, flMovementScalarCap);


    if (tActorAdd.eMonsterRarity != MR_NONE &&
        tActorAdd.eMonsterRarity != MR_NORMAL &&
        tActorAdd.eMonsterRarity != MR_HIRELING)
    {
        tACD.AttributesSetInt(ATTRIBUTE_GETHIT_IMMUNE, TRUE);
    }


    if (tActorAdd.dwActorAddFlags & (1 << ACTORADD_IS_FROM_MARKER))
    {
        if (tActorAdd.tMarkerOwner.snoMarkerSet != SNO_NONE)
        {
            ScopedMarkerSetDefinitionLockR pdefMarkerSet(tActorAdd.tMarkerOwner.snoMarkerSet);
            const Marker & tMarker = pdefMarkerSet->tMarkerSet[tActorAdd.tMarkerOwner.nMarkerIndex];
            BOOL bStartInSpecial = TagMapGetInt(tMarker.hTagMap, TAG_MARKER_AI_STARTS_IN_SPECIAL_STATE);
            if (bStartInSpecial)
            {
                tACD.AttributesSetInt(ATTRIBUTE_AI_IN_SPECIAL_STATE, TRUE);
            }


            BOOL bWeaponsSheathed = TagMapGetInt(tMarker.hTagMap, TAG_MARKER_WEAPONS_SHEATHED);
            if (bWeaponsSheathed)
            {
                tACD.AttributesSetInt(ATTRIBUTE_WEAPONS_SHEATHED, TRUE);
            }
        }
    }















    for (int32 i = 0; i < MAX_MONSTER_SKILLS; i++)
    {
        SkillDeclaration tSkill = MonsterGetSkill(tActorAdd.tSNOName, i);

		if (tSkill.snoPower == SNO_NONE)
		{
			break;
		}

		sSetBaseAttributeIfDifferent(tACD, MakeAttribKey(ATTRIBUTE_SKILL, tSkill.snoPower), nLevel + tSkill.nLevelMod);
    }

    sApplyMonsterAttributesFromSpreadSheet(tACD, pdefMonster);

    SGameGlobalsGet().rng = tOrigRNG;




    {
        CTagMap * pTagMap = pdefMonster->hTagMap;
        static const uint32 tMonsterAIStateTags[] =
        {
            TAG_MONSTER_GENERIC_PROJECTILE_AI_STATE,
            TAG_MONSTER_GENERIC_PROJECTILE2_AI_STATE,
            TAG_MONSTER_GENERIC_PIE_SHOCKWAVE_AI_STATE,
            TAG_MONSTER_GENERIC_PIE_BREATH_AI_STATE,
            TAG_MONSTER_GENERIC_AOE_NOVA_AI_STATE,
            TAG_MONSTER_GENERIC_AOE_TARGETED_AI_STATE,
            TAG_MONSTER_GENERIC_SUMMON_AI_STATE,
            TAG_MONSTER_GENERIC_SUMMON2_AI_STATE,
        };

        int32 nMonsterAIStateTagsCount = sizeof(tMonsterAIStateTags) / sizeof(tMonsterAIStateTags[0]);
        for (int32 i = 0; i < nMonsterAIStateTagsCount; i++)
        {
            SNO snoAIState = TagMapGetSNO(pTagMap, tMonsterAIStateTags[i]);
            if (snoAIState != SNO_NONE)
            {
                AIStateMerge(idActor, snoAIState, 0, BUFFID_INVALID);
            }
        }
    }
}




void sActorOnChangeLevelArea(ActorCommonData & tACD, LevelAreaID idLevelAreaOld, LevelAreaID idLevelAreaNew)
{
    if (ACDIsPlayer(tACD.id))
    {
        SPlayerOnChangeLevelArea(tACD.id, idLevelAreaOld, idLevelAreaNew);
    }
    else
    {
        SNO snoLevelArea = LevelAreaGetSNO(idLevelAreaNew);
        if (LevelAreaKillsEnemyMonsters(snoLevelArea) && tACD.IsMonster() && tACD.IsAlive() && !tACD.IsNPC() && !TeamsAreFriendly(tACD.GetTeam(), 2))
        {
            BrainAPIDoScriptedDeath(tACD, 0, FALSE, FALSE, ACTORID_INVALID);
        }
    }

    ACDInventoryEquipmentOrSkillChanged(tACD);
}































void sActorOnChangeScene(Actor * ptActor, SSceneID, SSceneID idSSceneNew, LevelAreaID, LevelAreaID)
{
    if (idSSceneNew != SSCENEID_INVALID)
    {
        Player * ptPlayer = PlayerGetByACD(ptActor->idACD);
        if (ptPlayer)
        {
            ACDID idACDPlayer = ptPlayer->idPlayerACD;
            SPlayerMarkSceneAsEntered(idSSceneNew);
            SQuestsOnPlayerEnterScene(idACDPlayer, SceneCommonGetSceneSNO(idSSceneNew));
        }
    }
}















void ActorCheckChangeLevelArea(ActorID idActor)
{
    Actor * ptActor = ActorGet(idActor);
    ActorCommonData & tACD = ACDGet(ptActor->idACD);
    SSceneID idSScene = ACDGetCurrentSSceneID(tACD);
    LevelAreaID idLevelArea = ACDGetLevelAreaID(tACD);

    BOOL bScenesDiffer = idSScene != ptActor->idCurrentScene;
    BOOL bLevelAreasDiffer = idLevelArea != ptActor->idCurrentLevelArea;
    if (bScenesDiffer || bLevelAreasDiffer)
    {
        if (bScenesDiffer)
        {
            sActorOnChangeScene(ptActor, ptActor->idCurrentScene, idSScene, ptActor->idCurrentLevelArea, idLevelArea);
            ptActor->idCurrentScene = idSScene;
        }

        if (bLevelAreasDiffer)
        {
            sActorOnChangeLevelArea(tACD, ptActor->idCurrentLevelArea, idLevelArea);
            ptActor->idCurrentLevelArea = idLevelArea;
        }
    }
}





void ActorCheckNavMeshTypeChange(const ActorID idActor, const uint32 uPrevNavMeshFlags, const SWorldID idSWorldPrev)
{



    Actor * ptActor = ActorGet(idActor);
    ActorCommonData & tACD = ACDGet(ptActor->idACD);
    if (tACD.tLocation.eActorLocation != AL_WORLD)
    {
        return;
    }

    WorldLocation & tLoc = tACD.GetWorldLocation();
    SWorldID idSWorld = tLoc.idSWorld;
    NavMeshHandle tNavMeshHandle;
    ACDGetNavMeshHandle(tACD, tNavMeshHandle);

    NavMeshInst * ptNavMeshInst = NavMeshInstGetByScene(tNavMeshHandle.idSScene);
    if (ptNavMeshInst == NULL || tNavMeshHandle.nSquareIndex < 0)
    {
        return;
    }

    uint32 uSquareFlags = NavMeshHandleToStaticFlags(tNavMeshHandle, FALSE);
    if (uSquareFlags & (1 << CELL_STATIC_SPECIAL_0))
    {
        if (!(ptActor->uCurrNavMeshFlags & (1 << CELL_STATIC_SPECIAL_0)))
        {
            SWorldRunSpecialNavMeshFunc(idSWorld, idActor, NAVMESH_SPECIAL_0, NAVMESH_FUNC_TYPE_IN);
        }
    }
    else if (ptActor->uCurrNavMeshFlags & (1 << CELL_STATIC_SPECIAL_0))
    {
        SWorldRunSpecialNavMeshFunc(idSWorld, idActor, NAVMESH_SPECIAL_0, NAVMESH_FUNC_TYPE_OUT);
    }

    else if ((uPrevNavMeshFlags & (1 << CELL_STATIC_SPECIAL_0)) && idSWorldPrev != SWORLDID_INVALID)
    {
        SWorldRunSpecialNavMeshFunc(idSWorldPrev, idActor, NAVMESH_SPECIAL_0, NAVMESH_FUNC_TYPE_OUT);
    }

    if (uSquareFlags & (1 << CELL_STATIC_SPECIAL_1))
    {
        if (!(ptActor->uCurrNavMeshFlags & (1 << CELL_STATIC_SPECIAL_1)))
        {
            SWorldRunSpecialNavMeshFunc(idSWorld, idActor, NAVMESH_SPECIAL_1, NAVMESH_FUNC_TYPE_IN);
        }
    }
    else if (ptActor->uCurrNavMeshFlags & (1 << CELL_STATIC_SPECIAL_1))
    {
        SWorldRunSpecialNavMeshFunc(idSWorld, idActor, NAVMESH_SPECIAL_1, NAVMESH_FUNC_TYPE_OUT);
    }

    else if ((uPrevNavMeshFlags & (1 << CELL_STATIC_SPECIAL_1)) && idSWorldPrev != SWORLDID_INVALID)
    {
        SWorldRunSpecialNavMeshFunc(idSWorldPrev, idActor, NAVMESH_SPECIAL_1, NAVMESH_FUNC_TYPE_OUT);
    }

    ptActor->uCurrNavMeshFlags = uSquareFlags;
}



static void sPrintOutOfActors()
{




    ActorsDumpList();


    ScenePrintDeactivationReasonsString();
}




static ActorBrain sGetActorBrain(
    ActorCommonData & tACD,
    const ActorAddStruct & tActorAdd,
    const ActorDefinition * pdefActor)
{
    if (tACD.IsGizmo())
    {
        GizmoType eGizmoType = tACD.GetGizmoType();
        switch (eGizmoType)
        {
        case ACTORGIZMO_SPAWNER:
            return ACTORBRAIN_SPAWNER;
        case ACTORGIZMO_ITEM_INTERNAL:
            return ACTORBRAIN_NOBRAIN;
        default:
            return ACTORBRAIN_DO_NOTHING;
        }
    }


    if (tActorAdd.ptPlayer)
        return ACTORBRAIN_PLAYER;


    if (tActorAdd.dwActorAddFlags & (1 << ACTORADD_BRAINLESS_BIT))
        return ACTORBRAIN_NOBRAIN;
    return (ActorBrain)TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_DEFAULT_BRAIN);
}








// NON_MATCHING: register allocation only (same instructions and block layout): the original keeps bWalking in w26 and spills bIgnoredByAI, ours does the reverse, which also moves spill slots and constant rematerialisation points
ActorID ActorAdd(
    const ActorAddStruct & tActorAdd)
{














    SNO snoActor = tActorAdd.snoActor;

    const ActorDefinition * pdefActor = (const ActorDefinition *)gptGroupDefActor->LockR(snoActor, FALSE);













    uint32 tNameToUse;

    ActorGlobals * ptActorGlobals = SGameManager::GameGet().ptActorGlobals;

    ptActorGlobals->dwActorNumberCount++;

    if (tActorAdd.dwActorAddFlags & (1 << ACTORADD_KEEP_MARKER_NAME))
    {
        tNameToUse = tActorAdd.tMarkersName;
    }
    else
    {
        tNameToUse = MarkerStringHash(CRefString().Format("%s-%d", SNOToString(ACTOR_SNO, snoActor, FALSE).str(), ptActorGlobals->dwActorNumberCount));
    }



















    float flScaleToUse = tActorAdd.flScale;
    if (flScaleToUse <= EPSILON)
    {
        float flMinScale = TagMapGetFloat(pdefActor->hTagMap, TAG_ACTOR_SCALE_MIN);
        float flDeltaScale = TagMapGetFloat(pdefActor->hTagMap, TAG_ACTOR_SCALE_DELTA);
        flScaleToUse = RandRangeFloat_Internal(SGameGlobalsGet().rng, flMinScale, flDeltaScale);
    }

    SNO snoConvList = tActorAdd.snoConvList;
    BOOL bNPC = TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_NPC_STARTS_AS_NPC);
    BOOL bHelper = TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_IS_HELPER);
    BOOL bGlobalFollower = TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_IS_GLOBAL_FOLLOWER);
    HirelingClass eHirelingClass = (HirelingClass)TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_HIRELING_CLASS);

    AttribGroupID idFastAttributeGroup = FastAttribGroupCreate(tActorAdd.eFastAttribType, -1);


    if (ptActorGlobals->ptActors->AvailableCount() <= 0)
    {
        uint32 uNewSize = ptActorGlobals->ptActors->dwMaximumCount + ptActorGlobals->ptActors->dwGrowSize;
        SGameGlobals & tSGameGlobals = SGameGlobalsGet();

        if (uNewSize > ptActorGlobals->uMaxActorSoftCap && !(tSGameGlobals.dwFlags & (1 << SGAMEGLOBALS_REQUEST_SHUT_DOWN)))
        {

            if (ptActorGlobals->uGarbageCollectionEmergencyCleanupAmount != 0)
            {

                sPrintOutOfActors();


                sPerformGarbageCollectionOverACDs(ptActorGlobals->uGarbageCollectionEmergencyCleanupAmount);

                // DWARF local nActorsCleanedUp (int32, 3048, no location): whatever used it was compiled out.
                int32 nActorsCleanedUp;



                ActorsDumpGarbageCollectionInfo();
            }


            if (ptActorGlobals->ptActors->AvailableCount() <= 0)
            {




                if (ptActorGlobals->uGarbageCollectionEmergencyCleanupAmount == 0)
                {
                    sPrintOutOfActors();
                }


                SGameID idSGame = SGameManager::GetCurrentGame();

                tSGameGlobals.dwFlags |= (1 << SGAMEGLOBALS_REQUEST_SHUT_DOWN);
                SGameForcePlayersQuit(QUIT_REASON_SHUTDOWN_GAME);
            }
        }
    }

    ActorID id = ptActorGlobals->ptActors->ElementNew();
    if (id == ACTORID_INVALID)
    {
        sPrintOutOfActors();
        gptGroupDefActor->UnlockR((void *&)pdefActor);
        return ACTORID_INVALID;
    }





    if (ptActorGlobals->ptActors->GetTotalCount() > ptActorGlobals->dwActorHighCount)
    {
        ptActorGlobals->dwActorHighCount = ptActorGlobals->ptActors->GetTotalCount();
    }

    Actor * ptActor = ptActorGlobals->ptActors->ElementGet(id);

    uint32 dwNow = GameGetServerTime();


    RNG tOrigRNG = SGameGlobalsGet().rng;
    if (tActorAdd.prngActorSeed)
    {
        SGameGlobalsGet().rng = *tActorAdd.prngActorSeed;
    }

    if (tActorAdd.tMarkerOwner.nAdventureParentIndex != 0xffff)
    {
        tNameToUse = MarkerStringHash(CRefString().Format("%s:%s", AdventureGetParentName(tActorAdd.tMarkerOwner.nAdventureParentIndex).str(), MarkerNameGetString(tNameToUse)));
    }

    ptActor->rndOriginal = SGameGlobalsGet().rng;
    ptActor->dwActorFlags = 0;
    ptActor->tName = tNameToUse;
    ptActor->snoCurrentAnim = SNO_NONE;
    ptActor->nCurrentAnimPermutationIndex = 0;
    ptActor->flCurrentAnimSpeedScale = 0.0f;
    ptActor->wvCurrentAnimVelocity = cgwvNull;
    ptActor->idCurrentScene = SSCENEID_INVALID;
    ptActor->idCurrentLevelArea = -1;
    ptActor->uCurrNavMeshFlags = 0;
    ptActor->dwTimeCreated = dwNow;
    ptActor->dwLastAttributeUpdateTime = dwNow;
    ptActor->nAttributeUpdateBucketIndex = -1;
    ptActor->nAreaDamageBucketIndex = -1;
    ptActor->dwLastAreaDamageUpdateTime = dwNow;
    ptActor->dwTimeSinceLastRecoilUpdate = (uint32)-1;
    ptActor->idDeathTimer = -1;
    ptActor->dwTrickleBits = 0;
    ptActor->nSourceSceneUID = -1;
    ptActor->tMarkerOwner = tActorAdd.tMarkerOwner;
    ptActor->ptSActorMovement = NULL;
    ptActor->ptStaggerDamageInfo = NULL;
    ptActor->listSNOScripts.SetPool(MemoryPoolsGetDATAIDListPool());
    ptActor->nAttackerCount = 0;
    ptActor->nAttackerSlotsUsed = 0;
    ptActor->nAttackerLargeSlotsUsed = 0;

    if (tActorAdd.idMarkersSScene != SSCENEID_INVALID)
    {
        SceneCommonData & tSCD = SceneCommonDataGet(tActorAdd.idMarkersSScene);

        ptActor->nSourceSceneUID = tSCD.nInstanceUID;
    }

    if (tActorAdd.hGB.eType != GB_ITEMS)
    {
        UpdateFunctionInitialize(UPDATE_ACTOR_ATTRIBUTES, ptActor->id, ptActor->tAttrUpdateData);
        UpdateFunctionInitialize(UPDATE_ACTOR_AREA_DAMAGE, ptActor->id, ptActor->tAreaDamageUpdateData);
    }

    ptActor->idACD = ACDCreate();
    ACDNetworkName ann = ANNFromACD(ptActor->idACD);


    ActorLocation tLocation;
    ActorLocationInit(
        tLocation,
        tActorAdd.tPlace,
        tActorAdd.qOrientation,
        flScaleToUse);

    ActorCommonData & tACD = ACDGet(ptActor->idACD);
    tACD.Initialize(
        ann,
        ptActor->id,
        tActorAdd.snoActor,
        tActorAdd.hGB,
        tActorAdd.tSNOName,
        tNameToUse,
        tLocation,
        idFastAttributeGroup,
        (tActorAdd.dwActorAddFlags & (1 << ACTORADD_FORCE_INVENTORY_CREATE)) != 0,
        TRUE);

    if (tActorAdd.dwActorAddFlags & (1 << ACTORADD_FAST_MOVER))
    {
        tACD.dwFlags |= (1 << ACD_FASTMOVER_BIT);
    }

    tACD.SetTeam(tActorAdd.eTeam);
    if (tActorAdd.dwInventorySeed != 0)
    {
        ActorInventorySetSeed(tACD.ptInventory, tActorAdd.dwInventorySeed);
    }


    BOOL fModifyLiveActorCount = !(tActorAdd.dwActorAddFlags & (1 << ACTORADD_IS_FROM_INACTIVE));
    tACD.SetPrimaryGroup(tActorAdd.uPrimaryGroup, fModifyLiveActorCount, ACDID_INVALID);
    tACD.SetSecondaryGroup(tActorAdd.uSecondaryGroup, fModifyLiveActorCount, ACDID_INVALID);

    ptActor->snoActor = snoActor;
    ptActor->snoMonster = (pdefActor->eType == ACTORTYPE_MONSTER) ? pdefActor->snoMonster : SNO_NONE;


    int32 nScripts = tActorAdd.listSNOScripts.Count();
    if (nScripts > 0)
    {
        POS posScript = tActorAdd.listSNOScripts.GetHeadPosition();
        while (posScript)
        {
            ptActor->listSNOScripts.AddTail_Tracked(tActorAdd.listSNOScripts.GetNext(posScript));
        }
    }
    else
    {
        SNO snoScript = TagMapGetSNO(pdefActor->hTagMap, TAG_ACTOR_SCRIPT);
        if (snoScript != SNO_NONE &&
            SNOCheckSNOReference(ACTOR_SNO, snoActor, 35, snoScript, NULL))
        {
            ptActor->listSNOScripts.AddTail_Tracked(snoScript);
        }
    }

    ptActor->snoConvList = snoConvList;

    POS posScript = ptActor->listSNOScripts.GetHeadPosition();
    while (posScript)
    {
        LuaScriptAttach(SCRIPT_ATTACH_ACTOR, ptActor->idACD, ptActor->listSNOScripts.GetNext(posScript), tActorAdd.tMarkerOwner.nAdventureParentIndex);
    }

    if (tActorAdd.ptPlayer)
    {

        PlayerSetACD(*tActorAdd.ptPlayer, ptActor->idACD);
    }

    BOOL bDoesNotDeactivate = tActorAdd.dwActorAddFlags & (1 << ACTORADD_DOESNT_DEACTIVATE);
    ActorFlagSet(ptActor->id, ACTOR_DOESNT_DEACTIVATE_BIT, bDoesNotDeactivate);

    ptActor->wvVelocity = cgwvNull;

    ptActor->aYaw = tActorAdd.qOrientation.ToYaw();

    ptActor->eBrain = sGetActorBrain(tACD, tActorAdd, pdefActor);

    BOOL bWalking = FALSE;
    BOOL bSpawnedOnNavMesh = FALSE;

    if (WorldTryGetCommonData(tActorAdd.tPlace.idSWorld))
    {
        ActorCollisionInitialize(tACD);


        sActorSetScale(ptActor->id, tACD.GetScale());

        if (!(tActorAdd.dwActorAddFlags & (1 << ACTORADD_DOESNT_WALK_BIT)))
        {
            bWalking = TRUE;


            SActorMovementInitialize(ptActor, tActorAdd.tPlace);

            NavMeshHandle tNavMeshHandle;
            ACDGetNavMeshHandle(tACD, tNavMeshHandle);

            if (!NavMeshHandleIsNone(tNavMeshHandle))
            {
                bSpawnedOnNavMesh = TRUE;
                PassabilitySettings tPassability;

                sActorAddGetPassabilitySettings(tActorAdd, tPassability, pdefActor);


                if (tPassability.ePassabilityType != PASSABILITY_UNDEFINED && tPassability.ePassabilityType != PASSABILITY_UNKNOWN_OBJECT)
                {

                    SceneCommonData * ptSCD = SceneCommonDataTryToGet(tNavMeshHandle.idSScene);

                    if (ptSCD)
                    {
                        SceneClearSpawnArea(*ptSCD, tActorAdd.tNavMeshHandle.nSquareIndex, tACD.GetLocation().tWorldLocation.wsBounds.wdRadius, FALSE);


                        if (tActorAdd.eMonsterRarity == MR_CHAMPION || tActorAdd.eMonsterRarity == MR_RARE || tActorAdd.eMonsterRarity == MR_UNIQUE)
                        {


                            SceneClearSpawnArea(*ptSCD, tActorAdd.tNavMeshHandle.nSquareIndex, 60.0f, TRUE);
                        }
                    }
                }
            }
        }

        if (!ptActor->ptSActorMovement && tLocation.eActorLocation == AL_WORLD)
        {

            SSceneID idSScene = SceneCommonGetSceneIDFromWorldPlace(tActorAdd.tPlace, FALSE);
            if (idSScene != SSCENEID_INVALID)
            {
                SScenePutACDInScene(idSScene, tACD.id, TRUE);
            }
        }
    }

    ActorPowersInitialize(ptActor->id);

    ActorTimerEffectsInit(ptActor->id);

    tACD.eMonsterRarity = tActorAdd.eMonsterRarity;
    tACD.nLookLinkIndex = ActorChooseLookLink(pdefActor, SGameGlobalsGet().rng);


    if (tActorAdd.dwActorAddFlags & (1 << ACTORADD_IS_FROM_DISTRIBUTION))
    {
        tACD.dwFlags |= (1 << ACD_IS_FROM_DISTRIBUTION_BIT);
    }

    ActorSetBrain(ptActor->id, ptActor->eBrain, tActorAdd.ptSavedAI);

    if ((tActorAdd.dwActorAddFlags & (1 << ACTORADD_DISABLE_BRAIN_BIT)) &&
        ActorHasBrain(ptActor->id))
    {
        ActorAISetStatusFlag(ptActor->id, ACTOR_AI_DISABLE_UPDATES_BIT, TRUE);
    }

    if (tActorAdd.ptLuaSavedDataKey)
    {
        LuaScriptRestoreForActivatingActor(ptActor->id, tActorAdd.ptLuaSavedDataKey);
    }
    BOOL bInvulnerable = TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_BEHAVIOR_INVULNERABLE);
    BOOL bCannotDie = TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_BEHAVIOR_CANNOT_DIE);
    BOOL bNoDamage = TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_BEHAVIOR_NO_DAMAGE);
    BOOL bIgnoredByAI = TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_CANNOT_BE_TARGETED_BY_AI);

    SNO snoDeathReplacementPower = TagMapGetSNO(pdefActor->hTagMap, TAG_ACTOR_POWER_INSTEAD_OF_DEATH);
    if (snoDeathReplacementPower != SNO_NONE)
    {
        float flPowerChance = TagMapGetFloat(pdefActor->hTagMap, TAG_ACTOR_POWER_INSTEAD_OF_DEATH_CHANCE);
        BOOL bUsePower = TRUE;
        if (flPowerChance < 1.0f)
        {
            float flRoll = RandFloat_Internal(SGameGlobalsGet().rng);
            if (flRoll > flPowerChance)
            {
                bUsePower = FALSE;
            }
        }
        if (bUsePower)
        {

            tACD.AttributesSetInt(FastAttribKey(ATTRIBUTE_DEATH_REPLACEMENT_POWER_SNO), snoDeathReplacementPower);
        }
    }


    if (tActorAdd.ptPlayer)
    {
        ptActor->dwActorFlags |= (1 << ACTOR_IS_PLAYER);
        SNOPreload::AddServerActorBySNO(tActorAdd.snoActor);
        const GameBalanceDefinition * pdefGB;
        const HeroData * pHeroData = (const HeroData *)GBLockR(tActorAdd.hGB, pdefGB, TRUE);
        if (pHeroData)
        {
            SNOPreload::AddServerSkillKit(pHeroData->arSkillKit[0]);
            GBUnlockR(pdefGB);
        }
    }
    else
    {
        BOOL fIsNPC = TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_NPC_STARTS_AS_NPC);
        if (!fIsNPC && ptActor->snoMonster != SNO_NONE)
        {
            ptActor->dwActorFlags |= (1 << ACTOR_SUMMON_PRELOADED);
            ScopedMonsterDefinitionLockR pdefMonster(ptActor->snoMonster);
            for (int32 i = 0; i < MAX_SUMMON_ACTORS; i++)
            {
                SNO snoActorLoc = pdefMonster->snoSummonActor[i];
                if (snoActorLoc != SNO_NONE)
                {
                    SNOPreload::AddServerActorBySNO(snoActorLoc);


                }
            }
        }
    }
    if (ActorGetInheritedTagMapValueInt(ptActor, TAG_ACTOR_KNOWN_WITH_SCENE, TAG_MARKER_KNOWN_WITH_SCENE))
    {
        tACD.dwFlags |= (1 << ACD_KNOWN_WITH_SCENE);
    }

    gptGroupDefActor->UnlockR((void *&)pdefActor);
    if (bWalking)
    {



        SGame & tSGame = SGameManager::GameGet();
        bSpawnedOnNavMesh |= tSGame.tStartParams.dwCreationFlags & (1 << GAMECREATEPARAM_UTILITY);





        if (!bSpawnedOnNavMesh)
        {
            if (tActorAdd.ptPlayer)
            {
                sReportSpawnFailure("Player NOT ON NAV MESH (adding anyway)", tActorAdd);
            }
            else
            {
                sReportSpawnFailure("NOT ON NAV MESH AFTER CREATED", tActorAdd);
                ActorDelete(ptActor->id);

                if (tActorAdd.prngActorSeed)
                {
                    SGameGlobalsGet().rng = tOrigRNG;
                }
                return ACTORID_INVALID;
            }
        }
    }
    if (bInvulnerable)
    {


        tACD.SetInvulnerable(TRUE);
    }
    if (bCannotDie)
    {

        tACD.SetCannotDie(TRUE);
    }
    if (bNoDamage)
    {

        tACD.SetTakesNoDamage(TRUE);
    }

    if (bIgnoredByAI)
    {
        tACD.SetCannotBeAddedToAITargetList(TRUE);
    }

    if (bNPC)
    {
        tACD.AttributesSetInt(FastAttribKey(ATTRIBUTE_IS_NPC), TRUE);
        tACD.AttributesSetInt(FastAttribKey(ATTRIBUTE_NPC_IS_OPERATABLE), TRUE);
    }

    if (bHelper)
    {
        tACD.AttributesSetInt(FastAttribKey(ATTRIBUTE_IS_HELPER), TRUE);
    }

    if (bGlobalFollower)
    {
        tACD.AttributesSetInt(FastAttribKey(ATTRIBUTE_PET_TYPE), PETTYPE_FOLLOWER);
    }

    if (eHirelingClass != HC_NONE)
    {
        tACD.AttributesSetInt(FastAttribKey(ATTRIBUTE_HIRELING_CLASS), eHirelingClass);
    }

    if (GameTypeIsPvP() &&
        tACD.GetTeam() != -1 &&
        tACD.IsPlayerTeam())
    {
        tACD.AttributesSetInt(FastAttribKey(ATTRIBUTE_DISPLAYS_TEAM_EFFECT), TRUE);
    }

    tACD.AttributesSetInt(FastAttribKey(ATTRIBUTE_MINIMAP_ICON_ACTIVE),
        ActorGetInheritedTagMapValueInt(ptActor, TAG_ACTOR_MINIMAP_ACTIVE_BY_DEFAULT, TAG_MARKER_MINIMAP_ACTIVE_BY_DEFAULT));


    if (!tActorAdd.ptPlayer)
    {
        ActorCheckChangeLevelArea(ptActor->id);
        ActorCheckNavMeshTypeChange(ptActor->id, 0, -1);
    }



    if (tActorAdd.plistActors)
    {
        tActorAdd.plistActors->AddTail_Tracked(ptActor->id);
    }

    if (ActorHasQuestLifespan(*ptActor))
    {
        ptActorGlobals->listActorsWithQuestLifespan.AddTail_Tracked(ptActor->id);
    }




    if (ACDGetHirelingClass(tACD) != HC_NONE && !ACDIsHireling(tACD))
    {
        ptActorGlobals->listActorsWithHirelingClass.AddTail_Tracked(ptActor->id);
    }

    if (tActorAdd.dwActorAddFlags & (1 << ACTORADD_IS_FROM_INACTIVE))
    {
        ptActorGlobals->listActorsActivated.AddTail_Tracked(ptActor->id);


        tACD.AttributesSetInt(FastAttribKey(ATTRIBUTE_SPAWNED_BY_INACTIVE_MARKER), TRUE);
    }
    else
    {
        ptActorGlobals->listActorsSpawned.AddTail_Tracked(ptActor->id);
    }


    STrickleTryTrickleToPlayers(ptActor->idACD);

    KnownListItemAdded(KET_ACD, tACD.id);

    tACD.InitAttributeSyncMask();

    if (tActorAdd.prngActorSeed)
    {
        SGameGlobalsGet().rng = tOrigRNG;
    }

    return ptActor->id;
}




static void sActorBrainToggleDebug(Actor * ptActor, const BOOL fEnabled)
{


    ActorAI * pAI = ActorTryToGetAI(ptActor->id);
    if (pAI && ActorAIIsStatusFlagSet(ptActor->id, ACTOR_AI_HAS_BRAIN_BIT))
    {
        if (fEnabled)
        {
        }
        else
        {




            BrainAPIDoIdle(ptActor->id);
        }
    }
}







void ActorsBrainsToggle(BOOL fEnabled)
{
    ActorIterator tIt;

    while (Actor * ptActor = ActorDataArrayIterate(tIt))
    {



        if (!ACDIsPlayer(ptActor->idACD) &&
            ptActor->eBrain != 5)
        {



            sActorBrainToggleDebug(ptActor, fEnabled);
        }
    }
}


BOOL ACDFindSpawnLocation(ACDID idACDSearchFrom, const PassabilityType ePassabilityType, WorldPlace & placeResult)
{


    BOOL bResult = FALSE;
    ActorCommonData & tACDSearchFrom = ACDGet(idACDSearchFrom);







    if (SceneCommonDataTryToGet(tACDSearchFrom.tLocation.tWorldLocation.idSScene) != NULL)
    {











        NavMeshHandle tNavMeshHandle;
        ACDGetNavMeshHandle(tACDSearchFrom, tNavMeshHandle);
        if (!NavMeshHandleIsNone(tNavMeshHandle))
        {
            NavMeshHandle tNavMeshHandleResult;









            bResult = FindSpawnLocation(tACDSearchFrom.snoActor, tNavMeshHandle, ePassabilityType, 0, placeResult, tNavMeshHandleResult);
        }
    }








    if (!bResult)
    {

        placeResult = tACDSearchFrom.GetPlace();
    }

    return bResult;
}

void ActorFindAddLocation(const ActorCommonData & tACDSearchFrom, const PassabilityType ePassabilityType, ActorAddStruct & tActorAdd)
{
    NavMeshHandle tNavMeshHandle;
    ACDGetNavMeshHandle(tACDSearchFrom, tNavMeshHandle);

    WorldPlace tPlace;
    NavMeshHandle tNavMeshHandleResult;
    FindSpawnLocation(tACDSearchFrom.snoActor, tNavMeshHandle, ePassabilityType, 0, tPlace, tNavMeshHandleResult);

    tActorAdd.idMarkersSScene = tNavMeshHandleResult.idSScene;
    tActorAdd.tPlace = tPlace;
    tActorAdd.tNavMeshHandle = tNavMeshHandleResult;
}















BOOL FindPerturbedSpawnLocation(SNO snoActor, const NavMeshHandle & tNavMeshHandle, const PassabilitySettings & tPassability, const float flMinSearchRadius, const uint32 dwFindLocationFlags, WorldPlace & placeResult, NavMeshHandle & tNavMeshHandleResult, Team eTeam)
{
    PassabilitySettings tPassabilitySettingsToUse = tPassability;
    tPassabilitySettingsToUse.snoActor = snoActor;

    SceneCommonDataActivate(tNavMeshHandle.idSScene);

    WorldPlace placeStart;
    NavMeshGetCenterPlace(tNavMeshHandle, placeStart);

    float flMaxSearchRadius = Math::Pin(flMinSearchRadius + 20.0f, 50.0f, 150.0f);

    return NavMeshSearchForLocation(placeStart, tPassabilitySettingsToUse, flMinSearchRadius, flMaxSearchRadius, dwFindLocationFlags, tNavMeshHandleResult, placeResult, FALSE, eTeam, 0.0f);
}
























BOOL FindSpawnLocation(SNO snoActor, const NavMeshHandle & tNavMeshHandle, const PassabilityType ePassabilityType, const uint32 dwFindLocationFlags, WorldPlace & placeResult, NavMeshHandle & tNavMeshHandleResult)
{
    PassabilitySettings tPassability;
    PassabilitySettingsInit(tPassability);
    tPassability.ePassabilityType = ePassabilityType;

    return FindPerturbedSpawnLocation(snoActor, tNavMeshHandle, tPassability, 0.0f, dwFindLocationFlags, placeResult, tNavMeshHandleResult, 1);
}






























void ActorSetOrientation(const ActorID idActor, const Quaternion & qOrientation, const BOOL bSendToClient)
{
    Actor * ptActor = ActorGet(idActor);
    ACDID idACD = ptActor->idACD;
    ActorCommonData & tACD = ACDGet(idACD);
    WorldLocation & loc = tACD.GetWorldLocation();
    loc.transform.q = qOrientation;
    ptActor->aYaw = qOrientation.ToYaw();
    ActorUpdateOrientation(idActor, bSendToClient);
}









void ActorInternalSetCurrentFacing(ActorID idActor, Angle aYaw)
{
    Actor * ptActor = ActorGet(idActor);
    ActorCommonData & tACD = ACDGet(ptActor->idACD);
    WorldLocation & loc = tACD.GetWorldLocation();

    ptActor->aYaw = aYaw;
    loc.transform.q.SetRotation(cgwvZAxis, aYaw);
}






void ActorTriggerAttributeUpdate(const ActorID idActor)
{

    Actor * ptActor = ActorGet(idActor);

    ActorCommonData & tACD = ACDGet(ptActor->idACD);
    if (tACD.IsItem())
    {
        return;
    }



    uint32 dwNow = GameGetServerTime();
    uint32 dwLastUpdateTime = ptActor->dwLastAttributeUpdateTime;
    uint32 dwEarliestNextUpdate = dwLastUpdateTime + DEFAULT_ATTRIBUTE_UPDATE_INTERVAL;
    uint32 dwStartingBucketTime = (dwNow / DEFAULT_ATTRIBUTE_UPDATE_INTERVAL) * DEFAULT_ATTRIBUTE_UPDATE_INTERVAL;
    if (ptActor->nAttributeUpdateBucketIndex == -1)
    {
        ptActor->nAttributeUpdateBucketIndex = ActorGlobalsGetNewAttributeUpdateBucketIndex();
    }
    int32 nCurrentFrameBucketIndex = ptActor->nAttributeUpdateBucketIndex;

    uint32 dwNextUpdateTime = dwStartingBucketTime + nCurrentFrameBucketIndex;


    if (dwNextUpdateTime < dwEarliestNextUpdate || dwNextUpdateTime <= dwNow)
    {
        dwNextUpdateTime += DEFAULT_ATTRIBUTE_UPDATE_INTERVAL;
    }


    uint32 dwTicksUntilNextUpdate = dwNextUpdateTime - dwNow;

    if (SGameManager::GameGet().ptSGameGlobals->dwFlags2 & (1 << SGAMEGLOBALS2_USE_FIXED_ATTRIBUTE_UPDATE_RATE))
    {
        dwTicksUntilNextUpdate = DEFAULT_ATTRIBUTE_UPDATE_INTERVAL;
    }


    ServerQueueUpdateFunction(dwTicksUntilNextUpdate, ptActor->tAttrUpdateData);
}
void ActorClearStaggeredDamage(const ActorID idActor)
{
    Actor * ptActor = ActorTryToGet(idActor);
    if (ptActor && ptActor->ptStaggerDamageInfo)
    {
        for (uint32 i = 0; i < ptActor->ptStaggerDamageInfo->arStaggeredDamageBuckets.size(); i++)
        {
            ptActor->ptStaggerDamageInfo->arStaggeredDamageBuckets[i] = 0.0f;
        }
    }
}




void sRegenResourceForACD(ActorCommonData & tACD, ResourceType eResourceType, uint32 dwTimeStep)
{


    float flManaMaxTotal = tACD.ResourceAttributeGetFloat(ATTRIBUTE_RESOURCE_EFFECTIVE_MAX, eResourceType);

    float flManaCur = tACD.ResourceAttributeGetFloat(ATTRIBUTE_RESOURCE_CUR, eResourceType);
    if (flManaCur > flManaMaxTotal)
    {
        tACD.ResourceAttributeSetFloat(ATTRIBUTE_RESOURCE_CUR, flManaMaxTotal, eResourceType);
    }


    if (tACD.AttributesAttributeExists(MakeAttribKey(ATTRIBUTE_RESOURCE_REGEN_TOTAL, eResourceType)) &&
        !tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_RESOURCE_REGEN_STOP_REGEN)))
    {
        flManaCur = tACD.ResourceAttributeGetFloat(ATTRIBUTE_RESOURCE_CUR, eResourceType);
        if (flManaCur < flManaMaxTotal)
        {

            float flRegenPerSecond = tACD.ResourceAttributeGetFloat(ATTRIBUTE_RESOURCE_REGEN_TOTAL, eResourceType);

            float flRegenAmount = flRegenPerSecond * dwTimeStep / 60.0f;

            if (flRegenAmount < FLT_EPSILON && tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_RESOURCE_DEGENERATION_STOPPED)))
            {
                flRegenAmount = 0.0f;
            }

            float flNewManaCur = flManaCur + flRegenAmount;





            tACD.ResourceAttributeSetFloat(ATTRIBUTE_RESOURCE_CUR, Math::Max(Math::Min(flNewManaCur, flManaMaxTotal), 0.0f), eResourceType);
        }
    }



    if (!tACD.IsInCombat())
    {
        flManaCur = tACD.ResourceAttributeGetFloat(ATTRIBUTE_RESOURCE_CUR, eResourceType);
        if (flManaCur < flManaMaxTotal)
        {
            float flManaRegenPerSecond = tACD.AttributesGetFloat(FastAttribKey(ATTRIBUTE_OOC_MANA_REGEN_PERCENT));
            float flManaRegenAmount = flManaMaxTotal * dwTimeStep * flManaRegenPerSecond / 60.0f;


            if (flManaRegenAmount < FLT_EPSILON && tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_RESOURCE_DEGENERATION_STOPPED)))
            {
                flManaRegenAmount = 0.0f;
            }





            tACD.ResourceAttributeSetFloat(ATTRIBUTE_RESOURCE_CUR, flManaCur + flManaRegenAmount, eResourceType);
        }
    }
}


















void sUpdateQueuedFloatingNumberMessages(
    ActorCommonData & tACD)
{
    uint32 dwNow = GameGetServerTime();


    for (int32 i = 0; i < MAX_PLAYERS; i++)
    {
        float flDOTDamageTaken = tACD.AttributesGetFloat(MakeAttribKey(ATTRIBUTE_DOT_DAMAGE_TAKEN_QUEUED, i));


        Player * ptPlayer = PlayerGetByPlayerIndex(i);
        if (!ptPlayer)
        {
            if (flDOTDamageTaken > 0.0f)
            {
                tACD.AttributesSetFloat(MakeAttribKey(ATTRIBUTE_DOT_DAMAGE_TAKEN_QUEUED, i), 0.0f);
            }
            continue;
        }

        if ((int32)dwNow >= tACD.AttributesGetInt(MakeAttribKey(ATTRIBUTE_DOT_DAMAGE_TAKEN_QUEUED_TICK, i)) && flDOTDamageTaken >= EPSILON)
        {

            FloatingNumberMessage tMessage;
            tMessage.annTarget = ANNFromACD(tACD.id);
            tMessage.flDamage = flDOTDamageTaken;
            tMessage.eType = FLOATING_NUMBER_TYPE_DOT_DAMAGE_DONE;

            PlayerList listDestPlayers(PlayerListGetElementPool());
            listDestPlayers.AddTail_Tracked(ptPlayer);
            MessageSendToClient(listDestPlayers, SMSG_FLOATING_NUMBER, &tMessage, sizeof(tMessage));

            tACD.AttributesSetFloat(MakeAttribKey(ATTRIBUTE_DOT_DAMAGE_TAKEN_QUEUED, i), 0.0f);
        }
    }


    Player * ptPlayer = PlayerGetByACD(tACD.id);
    if (ptPlayer)
    {
        float flDOTDamageTaken = tACD.AttributesGetFloat(FastAttribKey(ATTRIBUTE_DOT_DAMAGE_TAKEN_QUEUED_PLAYER));
        if ((int32)dwNow >= tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_DOT_DAMAGE_TAKEN_QUEUED_PLAYER_TICK)) && flDOTDamageTaken >= EPSILON)
        {

            FloatingNumberMessage tMessage;
            tMessage.annTarget = ANNFromACD(tACD.id);
            tMessage.flDamage = flDOTDamageTaken;
            tMessage.eType = FLOATING_NUMBER_TYPE_DOT_DAMAGE_TAKEN;

            PlayerList listDestPlayers(PlayerListGetElementPool());
            listDestPlayers.AddTail_Tracked(ptPlayer);
            MessageSendToClient(listDestPlayers, SMSG_FLOATING_NUMBER, &tMessage, sizeof(tMessage));

            tACD.AttributesSetFloat(FastAttribKey(ATTRIBUTE_DOT_DAMAGE_TAKEN_QUEUED_PLAYER), 0.0f);
        }
    }


    Player * ptControllingPlayer = ACDGetControllingPlayer(tACD.id);
    float flHealingReceived = tACD.AttributesGetFloat(FastAttribKey(ATTRIBUTE_HEALING_RECEIVED_QUEUED));
    if ((int32)dwNow >= tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_HEALING_RECEIVED_QUEUED_TICK)) && flHealingReceived >= EPSILON && ptControllingPlayer)
    {


        FloatingNumberMessage tMessage;
        tMessage.annTarget = ANNFromACD(tACD.id);
        tMessage.flDamage = flHealingReceived;
        tMessage.eType = FLOATING_NUMBER_TYPE_HEAL;

        PlayerList listDestPlayers(PlayerListGetElementPool());
        if (!GameTypeIsPvP())
        {
            listDestPlayers.AddTail_Tracked(ptControllingPlayer);
        }
        else
        {

            PlayerListGetAllConnected(listDestPlayers, -1, FALSE);
        }

        MessageSendToClient(listDestPlayers, SMSG_FLOATING_NUMBER, &tMessage, sizeof(tMessage));

        tACD.AttributesSetFloat(FastAttribKey(ATTRIBUTE_HEALING_RECEIVED_QUEUED), 0.0f);
    }
}



void ActorUpdateAttributes(
    const ActorID idActor)
{
    Actor * ptActor = ActorTryToGet(idActor);
    if (!ptActor)
    {
        return;
    }



    GameDebugSetCurrentUpdatingActor(ptActor->idACD);

    if (ptActor->tAttrUpdateData.dwGameTime != SGameGetGameCommonData().dwServerTime)
    {
        ACDIsProjectile(ptActor->idACD);










        ptActor->tAttrUpdateData.dwGameTime = SGameGetGameCommonData().dwServerTime;
    }

    ServerUpdateWasCalled(ptActor->tAttrUpdateData);


    uint32 dwNow = SGameGetGameCommonData().dwServerTime;
    uint32 dwTimeStep = dwNow - ptActor->dwLastAttributeUpdateTime;

    ptActor->dwLastAttributeUpdateTime = dwNow;

    ActorGlobalsDecrementAttributeUpdateBucketValue(ptActor->nAttributeUpdateBucketIndex);
    ptActor->nAttributeUpdateBucketIndex = -1;

    ACDID idACD = ptActor->idACD;
    ActorCommonData & tACD = ACDGet(idACD);
    if (tACD.IsAlive() && tACD.GetLocationType() == AL_WORLD)
    {
        BOOL bIsGodMode = ActorFlagIsSet(ptActor->id, ACTOR_GOD_MODE_BIT);

        if (!bIsGodMode)
        {
            sRegenResourceForACD(tACD, tACD.GetPrimaryResourceType(), dwTimeStep);
            sRegenResourceForACD(tACD, tACD.GetSecondaryResourceType(), dwTimeStep);

            float flHitpointsPerSecond = tACD.AttributesGetFloat(FastAttribKey(ATTRIBUTE_HITPOINTS_REGEN_PER_SECOND_TOTAL));
            PowerAPI_DoHitpointRegen(tACD, flHitpointsPerSecond, dwTimeStep);


            float flDamageShieldCurTotal = 0.0f;
            POS pos = tACD.listBuffs.GetHeadPosition();
            while (pos)
            {
                BuffID idBuff = tACD.listBuffs.GetNext(pos);

                PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
                if (ptBuff && !ptBuff->bDeleted)
                {
                    flDamageShieldCurTotal += ptBuff->flShieldAmount;
                }
            }

            tACD.AttributesSetFloat(FastAttribKey(ATTRIBUTE_DAMAGE_SHIELD_AMOUNT), flDamageShieldCurTotal);

            float flHPMaxTotal = tACD.AttributesGetFloat(FastAttribKey(ATTRIBUTE_HITPOINTS_MAX_TOTAL));
            float flCurHP = tACD.GetHitPointsCurrent();
            if (flCurHP > flHPMaxTotal)
            {
                tACD.AttributesSetFloat(FastAttribKey(ATTRIBUTE_HITPOINTS_CUR), flHPMaxTotal);
            }


            if (!tACD.IsInCombat())
            {
                float flHealthRegenPerSecond = tACD.AttributesGetFloat(FastAttribKey(ATTRIBUTE_OOC_HEALTH_REGEN_PERCENT));
                float flHPRegenAmount = flHPMaxTotal * dwTimeStep * flHealthRegenPerSecond / 60.0f;
                if (flCurHP < flHPMaxTotal)
                {

                    tACD.Heal(flHPRegenAmount, TRUE);
                }
            }


            if (dwTimeStep != 0)
            {
                pos = tACD.listBuffs.GetHeadPosition();
                while (pos && tACD.listBuffs.Count() > 0 && tACD.IsAlive())
                {
                    BuffID idBuff = tACD.listBuffs.GetNext(pos);
                    PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
                    if (ptBuff &&
                        !ptBuff->bDeleted &&
                        (ptBuff->bInfinite || ptBuff->dwDuration))
                    {

                        PowerAPI_BuffDealDOTDamage(*ptBuff, dwTimeStep);
                    }
                }
            }


            if (ptActor->ptStaggerDamageInfo)
            {
                PowerAPI_DealStaggeredDamage(ptActor);
            }


            float flFrailtyHealthThreshold = tACD.AttributesGetFloat(FastAttribKey(ATTRIBUTE_FRAILTY_HEALTH_THRESHOLD));
            if (flFrailtyHealthThreshold > EPSILON)
            {
                float flHPCurrent = tACD.GetHitPointsCurrent();
                float flHPMaxTotal = tACD.GetHitPointsMaxTotal();
                if (flHPCurrent <= flFrailtyHealthThreshold * flHPMaxTotal)
                {
                    PowerAPI_DealFrailtyDamage(tACD, flHPCurrent);
                }
            }
        }

        for (int32 iTimerType = 0; iTimerType < NUM_ATTRIBUTE_TIMER_TYPES; iTimerType++)
        {
            int32 iTimerDuration = tACD.AttributesGetInt(MakeAttribKey(ATTRIBUTE_ATTRIBUTE_TIMER, iTimerType));

            if (iTimerDuration > 0)
            {
                iTimerDuration = Math::Max<int32>(iTimerDuration - dwTimeStep, 0);

                if (iTimerDuration <= 0)
                {
                    ActorTimerEffectsExpireGroup(idActor, iTimerType);
                    ACDInventoryEquipmentOrSkillChanged(tACD);
                }
                else if (iTimerType == 1 &&
                         tACD.GetHitPointsMaxTotal() - tACD.GetHitPointsCurrent() < EPSILON)
                {
                    ActorTimerEffectsExpireGroup(idActor, iTimerType);
                }
                tACD.AttributesSetInt(MakeAttribKey(ATTRIBUTE_ATTRIBUTE_TIMER, iTimerType), iTimerDuration);
            }
        }

        if (ACDIsPlayer(idACD))
        {

            SPlayerUpdate(tACD);
        }
    }


    if (GameTypeIsPvP())
    {
        if (ACDIsPlayer(idACD) && !tACD.IsAlive())
        {
            if (dwNow >= (uint32)tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_RESPAWN_GAMETIME)))
            {
                BrainAPIResurrectToCheckpoint(tACD.id);
                ServerMessageSendAcceptedDeathAck(idACD);
            }
        }
    }

    if (tACD.AttributesAttributeExists(FastAttribKey(ATTRIBUTE_SUMMON_EXPIRATION_TICK)) &&

        tACD.GetRootTarget() == ACDID_INVALID)
    {
        uint32 dwSummonExpirationTick = tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_SUMMON_EXPIRATION_TICK));
        if (GameGetServerTime() > dwSummonExpirationTick)
        {
            BrainAPIDoDeath(idActor, ACTORID_INVALID, TRUE, TRUE, TRUE, NULL, NULL);
        }
        else
        {


            ActorTriggerAttributeUpdate(idActor);
        }
    }



    if (tACD.AttributesAttributeExists(FastAttribKey(ATTRIBUTE_INVENTORY_REROLL_TIME)))
    {
        uint32 dwRerollTime = tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_INVENTORY_REROLL_TIME));

        if (NPCIsAnyPlayerTalkingTo(tACD.id) != ACDID_INVALID)
        {
            uint32 dwEarliestReroll = dwNow + 3600;
            if (dwRerollTime < dwEarliestReroll)
            {
                tACD.AttributesSetInt(FastAttribKey(ATTRIBUTE_INVENTORY_REROLL_TIME), dwEarliestReroll);
            }
        }
        else if (dwNow > dwRerollTime)
        {
            SACDInventoryReroll(tACD);
        }



        ActorTriggerAttributeUpdate(idActor);
    }

    if (tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_QUEUE_DEATH)) &&
        !tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_CANNOT_DIE_DURING)))
    {
        tACD.AttributesSetInt(FastAttribKey(ATTRIBUTE_QUEUE_DEATH), 0);
        ActorID idLastDamageActor = (ActorID)tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_LAST_DAMAGE_MAINACTOR));
        if (!ActorTryToGet(idLastDamageActor))
        {
            idLastDamageActor = ACTORID_INVALID;
        }
        BrainAPIDoDeath(idActor, idLastDamageActor, FALSE, FALSE, FALSE, NULL, NULL);
    }

    if (tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_ACTOR_UPDATES_ATTRIBUTES_FROM_OWNER)))
    {
        ACDID idACDOwner = ACDGetControllingActor(tACD.id);
        ActorCommonData * ptACDOwner = ACDTryToGet(idACDOwner);
        if (ptACDOwner)
        {
            PowerAPI_UpdateSummonedActorAttributes(tACD, *ptACDOwner, NULL);
            if (ACDIsPlayer(ptACDOwner->id))
            {

                ActorTriggerAttributeUpdate(idActor);
            }
        }
    }


    float flCCResistance = tACD.AttributesGetFloat(FastAttribKey(ATTRIBUTE_CROWD_CONTROL_RESISTANCE));
    if (flCCResistance > EPSILON)
    {
        uint32 nAnimTag = AnimPlayerGetCurrentAnimTag(tACD);


        if (!tACD.IsStunned() &&
            !tACD.IsBlind() &&
            !tACD.IsRooted() &&
            !tACD.IsImmobilized() &&
            !tACD.IsFeared() &&
            !tACD.IsCharmed() &&
            !tACD.IsTaunted() &&
            !tACD.IsInKnockback() &&
            nAnimTag != TAG_ACTOR_ANIMATION_KNOCK_BACK_LAND)
        {
            float flCCResistanceNew = Math::Max(0.0f, flCCResistance + ActorGetAttributeUpdateTicks() * -0.05f / 60.0f);
            tACD.AttributesSetFloat(FastAttribKey(ATTRIBUTE_CROWD_CONTROL_RESISTANCE), flCCResistanceNew);
        }

        ActorTriggerAttributeUpdate(idActor);
    }

    sUpdateQueuedFloatingNumberMessages(tACD);


    FastAttribGroupApplyDirty(tACD.AttributesGetAttributeGroup());
}






void ActorUpdateGizmo(const ActorID idActor)
{

    Actor * ptActor = ActorTryToGet(idActor);
    if (ptActor == NULL)
    {
        return;
    }


    GameDebugSetCurrentUpdatingActor(ptActor->idACD);

    if (ptActor->tGizmoUpdateData.dwGameTime != SGameGetGameCommonData().dwServerTime)
    {



        ptActor->tGizmoUpdateData.dwGameTime = SGameGetGameCommonData().dwServerTime;
    }

    ServerUpdateWasCalled(ptActor->tGizmoUpdateData);

    ActorCommonData & tACD = ACDGet(ptActor->idACD);
    GizmoUpdate(tACD);
}

// X/Server/Powers/Powers.cpp:655 (explicit specialization; must be declared before the map uses it)
template <> uint32 XHashKey<AreaDamageAccumulatorKey>(AreaDamageAccumulatorKey tKey);

void ActorUpdateAreaDamage(
    const ActorID idActor)
{
    Actor * ptActor = ActorTryToGet(idActor);
    if (ptActor == NULL)
    {
        return;
    }

    ActorCommonData * ptACD = ACDTryToGet(ACDFindByActor(idActor));
    if (!ptACD || !ptACD->IsAlive())
    {
        return;
    }

    PowerGlobals * ptPowerGlobals = SGameManager::GameGet().ptPowerGlobals;
    if (ptPowerGlobals == NULL)
    {
        return;
    }

    if (ptActor->tAreaDamageUpdateData.dwGameTime != SGameGetGameCommonData().dwServerTime)
    {









        ptActor->tAreaDamageUpdateData.dwGameTime = SGameGetGameCommonData().dwServerTime;
    }

    ptActor->dwLastAreaDamageUpdateTime = GameGetServerTime();
    ActorGlobalsDecrementAreaDamageUpdateBucketValue(ptActor->nAreaDamageBucketIndex);
    ptActor->nAreaDamageBucketIndex = -1;

    blz::list<AreaDamageAccumulatorKey, blz::allocator<AreaDamageAccumulatorKey> > listKeysToRemove;

    MapAreaDamageAccumulators & tMapAreaDamageAccumulators = ptPowerGlobals->tMapAreaDamageAccumulators;
    if (tMapAreaDamageAccumulators.Count() > 0)
    {
        POS pos = tMapAreaDamageAccumulators.GetStartAssoc();
        while (pos)
        {
            AreaDamageAccumulatorKey tKey;
            float flDamageAccumulated;
            tMapAreaDamageAccumulators.GetNextAssoc(pos, tKey, flDamageAccumulated);

            ActorCommonData * ptACDTarget = ACDTryToGet(tKey.idACDTarget);
            // Inferred: a missing target and a target other than this actor's ACD both skip the push_back on 4382.
            if (!ptACDTarget || ptACD->id != tKey.idACDTarget)
            {
                continue;
            }

            ActorCommonData * ptACDAttacker = ACDTryToGet(tKey.idACDAttacker);
            if (ptACDAttacker && ptACDAttacker->tLocation.eActorLocation == AL_WORLD)
            {
                PowerStartData tPowerStartData;
                PowerStartDataInit(tPowerStartData);
                tPowerStartData.idACD = tPowerStartData.idACDAlternateSource = tKey.idACDAttacker;
                tPowerStartData.snoPower = GlobalSNOGet(GLOBALSNO_POWERS_SPLASH_DAMAGE_PROC);

                PowerID idPower = PowerStartEx(tPowerStartData, -1, 0, 0);
                Power * ptPower = PowerTryToGet(idPower);
                if (ptPower)
                {
                    PowerAPIAttackPayload tPayload;
                    PowerHelper_AttackPayloadInit_None(ptPower->id, tPayload);
                    tPayload.arCombatDamage[DAMAGE_TYPE_PHYSICAL].flMin = flDamageAccumulated;
                    tPayload.bTriggersHitProcs = FALSE;
                    tPayload.bSplashDamage = TRUE;
                    tPayload.bDisableHitEffects = TRUE;
                    tPayload.bCanCrit = FALSE;
                    tPayload.bIgnoreDamageBonusAndReduction = TRUE;
                    tPayload.eDeliveryMechanism = PDM_THORNS;
                    // Inferred: flProcScalar (+0x14) is stored by the stp on line 4369; its own line is unknown.
                    tPayload.flProcScalar = 0.0f;
                    PowerAPI_DoPayload(tPayload, ptACDTarget->id, tKey.idACDAttacker);
                }
            }

            listKeysToRemove.push_back(tKey);
        }
    }

    for (blz::list<AreaDamageAccumulatorKey, blz::allocator<AreaDamageAccumulatorKey> >::iterator iter = listKeysToRemove.begin(); iter != listKeysToRemove.end(); ++iter)
    {
        tMapAreaDamageAccumulators.RemoveAt(*iter);
    }

    ServerUpdateWasCalled(ptActor->tAreaDamageUpdateData);
}




void ActorDelete(ActorID idActor)
{

    Actor * ptActor = ActorTryToGet(idActor);
    sActorDelete(ptActor, FALSE);
}




void ActorFlagForDeletion(ActorID idActor)
{
    Actor * ptActor = ActorTryToGet(idActor);
    if (ptActor)
    {
        if (TimerExists(ptActor->idDeathTimer))
        {
            TimerUnregister(ptActor->idDeathTimer);
            ptActor->idDeathTimer = TIMERID_INVALID;
        }

        ActorFlagSet(idActor, ACTOR_DELETE_AT_END_OF_UPDATE_BIT, TRUE);
        ActorGlobals * ptActorGlobals = SGameManager::GameGet().ptActorGlobals;
        ptActorGlobals->listActorsFlaggedForDeletion.AddTail_Tracked(idActor);
    }
}






void ActorDeactivateFromQuestRange(ActorID idActor)
{
    Actor * ptActor = ActorTryToGet(idActor);
    if (ptActor == NULL)
    {
        return;
    }

    SNO snoQuestRange = ActorGetQuestLifespan(*ptActor);
    if (QuestRangeStillPending(snoQuestRange))
    {
        ActorDeactivate(idActor, FALSE);
    }
    ActorFlagForDeletion(idActor);
}








static void sActorDeleteSoonCallback(TimerID, DataID idOwner, void *)
{
    ActorID idActor = (ActorID)idOwner;
    Actor * ptActor = ActorTryToGet(idActor);
    if (ptActor == NULL)
    {
        return;
    }

    ptActor->idDeathTimer = TIMERID_INVALID;
    ActorFlagForDeletion(idActor);
}






TimerID ActorDeleteSoon(ActorID idActor, int32 nDuration)
{
    ActorFlagSet(idActor, ACTOR_DOESNT_DEACTIVATE_BIT, TRUE);

    Actor * ptActor = ActorGet(idActor);
    if (TimerExists(ptActor->idDeathTimer))
    {
        if (TimerGetTime(ptActor->idDeathTimer) <= GameGetServerTime() + nDuration)
        {
            return TIMERID_INVALID;
        }
        TimerUnregister(ptActor->idDeathTimer);
    }

    ptActor->idDeathTimer = TimerRegister(idActor, TIMER_OWNER_ACTOR, nDuration, NULL, sActorDeleteSoonCallback);
    return ptActor->idDeathTimer;
}

























void ActorsDeleteAllFlagged()
{




    ActorGlobals * ptActorGlobals = SGameManager::GameGet().ptActorGlobals;

    while (!ptActorGlobals->listActorsFlaggedForDeletion.IsEmpty())
    {
        ActorID idActor = (ActorID)ptActorGlobals->listActorsFlaggedForDeletion.GetHead();
        ptActorGlobals->listActorsFlaggedForDeletion.RemoveHead();

        ActorDelete(idActor);
    }
}



void ActorsFireSpawnEvents()
{




    ActorGlobals * ptActorGlobals = SGameManager::GameGet().ptActorGlobals;
    while (!ptActorGlobals->listActorsSpawned.IsEmpty())
    {
        ActorID idActor = (ActorID)ptActorGlobals->listActorsSpawned.GetHead();
        Actor * ptActor = ActorTryToGet(idActor);
        if (ptActor)
        {
            ActorCommonData & tACD = ACDGet(ptActor->idACD);
            ACDID idSpawner = ACDID_INVALID;
            ActorCommonData * ptACDSpawner = tACD.HasSpawner() ? ACDTryToGet((ACDID)tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_SPAWNED_BY_ACDID))) : NULL;
            if (ptACDSpawner)
            {
                idSpawner = ptACDSpawner->id;
            }



            if (!tACD.IsItem())
            {
                ScriptEventOnActorSpawnedBySpawner(idSpawner, tACD.id);
                ScriptEventOnActorActivated(tACD.id);
            }



            if (ptActor->ptAI && ProximityDetectorTryGetData(ptActor->ptAI->idProximityData))
            {
                ProximityDetectorBrainOnCreate(idActor);
            }


            if (tACD.IsMonster())
            {
                for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
                {
                    Achievement::EvaluationData tEvaluationData;
                    tEvaluationData.SetACD(&tACD);
                    tEvaluationData.SetPlayer(ptPlayer);

                    tEvaluationData.EvaluateEvent(ptPlayer,
                        Achievement::ACH_EVENT_MONSTER_SPAWNED,

                        tACD.GetActorSNO(),
                        1, (uint32)sub_71007b5e70());
                }
            }


            SQuestsCheckForBountyActorSpawned(*ptActor);
            SGizmoAddStaticGizmoPing(tACD);
        }

        ptActorGlobals->listActorsSpawned.RemoveHead();
    }

    while (!ptActorGlobals->listActorsActivated.IsEmpty())
    {
        ActorID idActor = (ActorID)ptActorGlobals->listActorsActivated.GetHead();
        Actor * ptActor = ActorTryToGet(idActor);
        if (ptActor)
        {
            ActorCommonData & tACD = ACDGet(ptActor->idACD);


            if (!tACD.IsItem())
            {
                ScriptEventOnActorActivated(tACD.id);
            }



            if (ptActor->ptAI && ProximityDetectorTryGetData(ptActor->ptAI->idProximityData))
            {
                ProximityDetectorBrainOnCreate(idActor);
            }
        }

        ptActorGlobals->listActorsActivated.RemoveHead();
    }
}



BOOL ActorFlaggedForDeletion(ActorID idActor)
{
    Actor * ptActor = ActorTryToGet(idActor);
    if (ptActor == NULL)
    {
        return TRUE;
    }
    return ActorFlagIsSet(idActor, ACTOR_DELETE_AT_END_OF_UPDATE_BIT);
}






void ActorUpdateOrientation(const ActorID idActor, BOOL bSendToClient)
{
    ACDID idACD = ActorGet(idActor)->idACD;
    ActorCommonData & tACD = ACDGet(idACD);
    WorldLocation & tLocation = tACD.GetWorldLocation();
    SNO snoActor = tACD.snoActor;

    Sphere wsOld = tLocation.wsBounds;
    LocationSetBounds(snoActor, tLocation, NULL);
    ACDSetBoundingSphere(tACD, wsOld);
    tACD.UpdateCollision();

    if (bSendToClient)
    {
        KnownListItemChanged(KET_ACD, tACD.id, FALSE);
        tACD.ModifiedMark(ACDMODIFIED_WORLD_POSITION);
    }
}










void ActorInternalSetPosition(Actor * ptActor, const WorldPlace & tPlace)
{








    ActorCommonData & tACD = ACDGet(ptActor->idACD);

    WorldLocation & loc = tACD.GetWorldLocation();
    if (tPlace.idSWorld != loc.idSWorld)
    {

        CRefString sOwner("");
        if (tACD.IsPet())
        {
            ACDID idACDOwner = ACDGetPetOwner(ptActor->idACD);
            Player * ptPlayer = PlayerGetByACD(idACDOwner);
            if (ptPlayer)
            {

                CRefString sAccountName(ptPlayer->tAccount.usName);
                CRefString sHeroName(ptPlayer->usHeroName);
                sOwner.Format("\"%s\":\"%s\"'s pet ", sAccountName.str(), sHeroName.str());
            }
        }

        DisplayInternalError(NULL, "", 4722, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }













    loc.transform.wp = tPlace.wp;


    NavMeshHandle hNavMeshCurr;
    ScenesCommonNavMeshNodeFromPlace(tPlace, &hNavMeshCurr, FALSE, FALSE);
    loc.idSScene = NavMeshHandleToSubSceneSceneID(hNavMeshCurr);

    if (ptActor->ptSActorMovement)
    {
        ptActor->ptSActorMovement->PositionChanged(tPlace, loc.idSScene);
    }
}

void ActorSnapToPosition(const ActorID idActor, const WorldPlace & tPlace, BOOL bSnapToGround, BOOL bSendToClient)
{
    if (Math::Abs(tPlace.wp.x) >= 100000.0f ||
        Math::Abs(tPlace.wp.y) >= 100000.0f ||
        Math::Abs(tPlace.wp.z) >= 100000.0f)
    {
        ActorFlagForDeletion(idActor);
        return;
    }

    Actor * ptActor = ActorGet(idActor);
    if (ptActor->ptSActorMovement)
    {
        ptActor->ptSActorMovement->SnapToNewPlace(tPlace, bSnapToGround, FALSE, bSendToClient, 0);
        return;
    }

    ActorInternalSetPosition(ptActor, tPlace);
    ActorUpdateOrientation(idActor, bSendToClient);
    ActorCheckChangeLevelArea(ptActor->id);
    ActorCheckNavMeshTypeChange(ptActor->id, 0, SWORLDID_INVALID);
}













static void sActorSetScale(
    const ActorID idActor,
    const float flScale)
{
    ACDID idACD = ActorGet(idActor)->idACD;

    ActorLocationSetScale(ACDGet(idACD).tLocation, flScale);
    ActorUpdateOrientation(idActor, TRUE);
}




















































Actor * ActorDataArrayIterate(ActorIterator & tIterator)
{
    ActorGlobals * ptActorGlobals = SGameManager::GameGet().ptActorGlobals;
    Actor * ptActor = ptActorGlobals->ptActors->Iterate(tIterator);
    return ptActor;
}

















void ServerMessageSetPlayerActorInitial(const PlayerList & listDestPlayers, const int32 nPlayerIndex, const ActorCommonData & tACDPlayer)
{
    PlayerActorSetInitialMessage tMessage;
    tMessage.ann = ANNFromACD(tACDPlayer.id);
    tMessage.nPlayerIndex = nPlayerIndex;
    MessageSendToClient(listDestPlayers, SMSG_PLAYER_ACTOR_SET, &tMessage, sizeof(tMessage));
}








void ServerMessageProjectileStick(const ActorCommonData & tACDSource, const ActorCommonData & tACDTarget, const SNO snoActorAttachment)
{


    if (ACDIsPlayer(tACDTarget.id))
    {
        return;
    }

    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetInKnowRegion(listDestPlayers, tACDTarget.GetPlace());
    ProjectileStickMessage tMessage;


    tMessage.wpSourceLoc = tACDSource.GetWorldPlaceCenter().wp;
    tMessage.annTarget = ANNFromACD(tACDTarget.id);
    tMessage.snoActorAttachment = snoActorAttachment;

    MessageSendToClient(listDestPlayers, SMSG_PROJECTILE_STICK, &tMessage, sizeof(tMessage));
}





Actor * ActorGet(ActorID idActor)
{
    ActorGlobals * ptActorGlobals = SGameManager::GameGet().ptActorGlobals;
    return ptActorGlobals->ptActors->ElementGet(idActor);
}





Actor * ActorTryToGet(ActorID idActor)
{
    ActorGlobals * ptActorGlobals = SGameManager::GameGet().ptActorGlobals;
    return ptActorGlobals->ptActors->ElementTryToGet(idActor);
}





int32 ActorsGetCount()
{
    SGame & tSGame = SGameManager::GameGet();
    return ActorsGetCount(tSGame);
}




int32 ActorsGetCount(const SGame & tSGame)
{
    if (tSGame.ptActorGlobals == NULL || tSGame.ptActorGlobals->ptActors == NULL)
    {
        return -1;
    }
    return tSGame.ptActorGlobals->ptActors->dwTotalCount;
}







































































































void ActorsDumpGarbageCollectionInfo()
{
    SGame & tSGame = SGameManager::GameGet();
    ActorGlobals *& ptActorGlobals = tSGame.ptActorGlobals;
}











void ActorsDumpList()
{



    ActorIterator tIt;


    while (Actor * ptActor = ActorDataArrayIterate(tIt))
    {



        ActorCommonData * ptACD = ACDTryToGet(ptActor->idACD);
        if (ptACD)
        {
            if (ptACD->IsMonster())
            {
            }
            else if (ptACD->IsItem())
            {
                ACDID idACDContainedBy = ACDID_INVALID;
                if (ptACD->tLocation.eActorLocation == AL_INVENTORY)
                {
                    const InventoryLocation & tInvLoc = ptACD->GetInventoryLocation();
                    idACDContainedBy = tInvLoc.idACDContainer;
                    const ActorCommonData * ptACDContainedBy = ACDTryToGet(idACDContainedBy);
                }

                else if (ptACD->tLocation.eActorLocation == AL_WORLD)
                {
                    WorldLocation & loc = ptACD->GetWorldLocation();
                    SNO snoWorld = WorldCommonGetWorldSNO(loc.idSWorld);
                }
            }







            else if (ACDIsPlayer(ptActor->idACD))
            {
                const Player & tPlayer = *PlayerGetByACD(ptActor->idACD);
            }





            else if (ptACD->IsWreckableObject())
            {
            }
        }
    }









    ActorsDumpGarbageCollectionInfo();
}




static bool sActorIsNonMover(const ActorDefinition * pdefActor, const SNOName & tSNOName)
{


    if (tSNOName.snoGroup == ACTOR_SNO && pdefActor->snoMonster != SNO_NONE)
    {
        MonsterType eMonsterType = MonsterGetMonsterType(tSNOName);
        if (eMonsterType == MT_SCENERY)
        {
            return TRUE;
        }
    }

    GizmoType nGizmoType = (GizmoType)TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_GIZMO_TYPE);
    return GizmoIsNonMoving(nGizmoType);
}









static void sActorAddGetPassabilitySettings(
    const ActorAddStruct & tActorAdd,
    PassabilitySettings & tPassability,
    const ActorDefinition * pdefActor)
{
    NavMeshGetActorPassabilitySettings(tActorAdd.snoActor, tActorAdd.flScale, tPassability);

    if (tActorAdd.dwActorAddFlags & (1 << ACTORADD_USE_NEW_ENCOUNTER_PASSABILITY))
    {
        tPassability.ePassabilityType = PASSABILITY_NEW_ENCOUNTER;
    }
    else if (tPassability.ePassabilityType == PASSABILITY_UNDEFINED)
    {
        BOOL bNonMover = sActorIsNonMover(pdefActor, tActorAdd.tSNOName);

        if (tActorAdd.dwActorAddFlags & (1 << ACTORADD_PROJECTILE))
            tPassability.ePassabilityType = PASSABILITY_PROJECTILE;
        else

            tPassability.ePassabilityType = bNonMover ? PASSABILITY_NONMOVER : PASSABILITY_MONSTER;
    }
}











static BOOL sActorGetSpawnPosition(ActorAddStruct & tActorAdd, PassabilitySettings * ptPassability)
{

    if (!(tActorAdd.dwActorAddFlags & (1 << ACTORADD_DONT_SEARCH_FOR_LOCATION)))
    {

        PassabilitySettings tPassability;
        if (!ptPassability)
        {
            ScopedActorDefinitionLockR pdefActor(tActorAdd.snoActor);

            sActorAddGetPassabilitySettings(tActorAdd, tPassability, pdefActor);
            ptPassability = &tPassability;

        }

        float flSkipDistance = 0.0f;
        if (!(tActorAdd.dwActorAddFlags & (1 << ACTORADD_DONT_PERTURB)))
        {
            int32 nSkipDistanceMin;
            int32 nSkipDistanceDelta;
            ActorGetSpawnSkipDistance(nSkipDistanceMin, nSkipDistanceDelta);

            flSkipDistance = (float)RandRange_Internal(SGameGlobalsGet().rng, nSkipDistanceMin, nSkipDistanceDelta);
        }

        if (NavMeshHandleIsNone(tActorAdd.tNavMeshHandle))
        {
            ScenesCommonNavMeshNodeFromPlace(tActorAdd.tPlace, &tActorAdd.tNavMeshHandle, FALSE, FALSE);
        }



        RNG tRNGCopy = SGameGlobalsGet().rng;

        BOOL bSuccess = FALSE;
        if (!NavMeshHandleIsNone(tActorAdd.tNavMeshHandle))
        {
            bSuccess = FindPerturbedSpawnLocation(
                tActorAdd.snoActor,
                tActorAdd.tNavMeshHandle,
                *ptPassability,
                flSkipDistance,
                1,
                tActorAdd.tPlace,
                tActorAdd.tNavMeshHandle,
                tActorAdd.eTeam);
        }

        SGameGlobalsGet().rng = tRNGCopy;

        if (!bSuccess &&
            !(tActorAdd.dwActorAddFlags & (1 << ACTORADD_EVEN_WHEN_STUCK)))
        {
            if (tActorAdd.dwActorAddFlags & (1 << ACTORADD_DONT_ADD_IF_LOCATION_SEARCH_FAILS))
            {
                return FALSE;
            }


            if (!(tActorAdd.dwActorAddFlags & (1 << ACTORADD_ADD_AT_SPAWNER_LOCATION_IF_SEARCH_FAILS)))
            {
                sReportSpawnFailure("TOO CROWDED (adding anyway)", tActorAdd);
            }
        }
    }

    return TRUE;
}





BOOL ActorGetSpawnParams(ActorAddStruct & tActorAdd)
{
    if (tActorAdd.snoActor == SNO_NONE)
    {
        return FALSE;
    }

    ScopedActorDefinitionLockR pdefActor(tActorAdd.snoActor);



    SGameGlobals & tSGameGlobals = SGameGlobalsGet();

    if (tActorAdd.flScale < EPSILON || (pdefActor->dwFlags & (1 << ACTORDEF_SCALE_COMES_FROM_ACTOR_BIT)))
    {

        float flMinScale = TagMapGetFloat(pdefActor->hTagMap, TAG_ACTOR_SCALE_MIN);
        float flDeltaScale = TagMapGetFloat(pdefActor->hTagMap, TAG_ACTOR_SCALE_DELTA);
        tActorAdd.flScale = RandRangeFloat_Internal(tSGameGlobals.rng, flMinScale, flDeltaScale);

        if (tActorAdd.eMonsterRarity == MR_CHAMPION ||
            tActorAdd.eMonsterRarity == MR_RARE ||
            tActorAdd.eMonsterRarity == MR_UNIQUE ||
            tActorAdd.eMonsterRarity == MR_BOSS)
        {

            float flScaleFactor = TagMapGetFloat(pdefActor->hTagMap, TAG_ACTOR_RARE_SCALE_FACTOR);
            tActorAdd.flScale *= flScaleFactor;
        }
    }



    BOOL bRandomYaw = FALSE;
    if (pdefActor->dwFlags & (1 << ACTORDEF_HAS_RANDOM_FACING_BIT))
    {
        bRandomYaw = TRUE;
    }
    else if (!(tActorAdd.dwActorAddFlags & ((1 << ACTORADD_DONT_PERTURB) | (1 << ACTORADD_DONT_SEARCH_FOR_LOCATION))))
    {
        bRandomYaw = TRUE;
    }




    if (bRandomYaw)
    {

        Angle aYaw = RandAngle(tSGameGlobals.rng);
        tActorAdd.qOrientation.FromYaw(aYaw);
    }

    PassabilitySettings tPassability;
    sActorAddGetPassabilitySettings(tActorAdd, tPassability, pdefActor);

    if (tActorAdd.dwActorAddFlags & (1 << ACTORADD_DOESNT_WALK_BIT))
    {
        return TRUE;
    }


    BOOL bCanSpawnAtPlace = TRUE;
    SGame & tSGame = SGameManager::GameGet();
    if (!(tSGame.tStartParams.dwCreationFlags & (1 << GAMECREATEPARAM_UTILITY)))
    {




        if (tActorAdd.dwActorAddFlags & (1 << ACTORADD_USE_ACTOR_STATIC_FLAGS_FOR_SPAWN))
        {
            ACDCollSettings tCollSettings;
            ACDCollGetSettings(tActorAdd.snoActor, tCollSettings);

            NavMeshHandle tNavMeshHandle;
            if (ScenesCommonNavMeshNodeFromPlace(tActorAdd.tPlace, &tNavMeshHandle, FALSE, FALSE))
            {
                uint32 dwSquareStaticFlags = NavMeshHandleGetStaticFlags(tNavMeshHandle);
                if (!(dwSquareStaticFlags & tCollSettings.uStaticOneRequiredFlags) || (dwSquareStaticFlags & tCollSettings.uStaticPreventFlags))
                {
                    bCanSpawnAtPlace = FALSE;
                }
            }
        }
        else
        {
            bCanSpawnAtPlace = NavMeshPlaceCanEnterForSpawning(tActorAdd.tPlace, tPassability.ePassabilityType);
        }
    }

    if (bCanSpawnAtPlace)
    {
        return sActorGetSpawnPosition(tActorAdd, &tPassability);
    }
    else if (tActorAdd.dwActorAddFlags & (1 << ACTORADD_EVEN_WHEN_STUCK))
    {
        sReportSpawnFailure("NOT ON PASSABLE NAV MESH (adding anyway)", tActorAdd);
    }
    else
    {

        sReportSpawnFailure("NOT ON PASSABLE NAV MESH", tActorAdd);
        return FALSE;
    }


    return TRUE;
}
static void sMonsterPutInPack(
    ActorCommonData & tACD,
    const ActorAddStruct & tActorAdd,
    EncounterAddStruct & tEncounterAdd)
{
    if (!(tActorAdd.dwActorAddFlags & (1 << ACTORADD_IS_FROM_INACTIVE)) && (tActorAdd.eMonsterRarity == MR_CHAMPION || tActorAdd.eMonsterRarity == MR_RARE || tActorAdd.eMonsterRarity == MR_RARE_MINION))
    {











        if (tEncounterAdd.idACDPack == -1)
        {
            tEncounterAdd.idACDPack = ACDPackNew();
        }

        ACDPack & tPack = ACDPackGet(tEncounterAdd.idACDPack);
        tPack.AddMember(tACD);
    }
}

static float sGetMinionSpawnCountMultipler(const ActorCommonData & tACD)
{

    float flMultiplier = 1.0f + tACD.AttributesGetFloat(FastAttribKey(ATTRIBUTE_MINION_COUNT_BONUS_PERCENT));
    return Math::Max(flMultiplier, 0.0f);
}




void sMonsterSpawnMinions(
    ActorCommonData & tACD,
    const ActorAddStruct & tActorAddOriginal,
    EncounterAddStruct & tEncounterAdd)
{
    ScopedActorDefinitionLockR pdefActor(tActorAddOriginal.tSNOName.snoHandle);

    if (!MonsterExists(pdefActor->snoMonster))
    {
        return;
    }

    ScopedMonsterDefinitionLockR pdefMonster(pdefActor->snoMonster);
    if (!pdefMonster)
    {
        return;
    }

    RNG & rng = SGameGetRNG();
    int32 nSpawnGroupCount = pdefMonster->nMinionSpawnGroupCount;


    float dwTotalWeight = 0.0f;
    for (int32 i = 0; i < nSpawnGroupCount; i++)
    {
        const MonsterMinionSpawnGroup & tSpawnGroup = pdefMonster->arMinionSpawnGroups[i];
        dwTotalWeight += tSpawnGroup.nWeight;
    }

    float fTarget = RandFloat_Internal(rng);
    int nSpawnGroupIndex;
    const MonsterMinionSpawnGroup * pSpawnGroup = NULL;
    float fCurrentWeight = 0.0f;
    for (nSpawnGroupIndex = 0; nSpawnGroupIndex < nSpawnGroupCount; nSpawnGroupIndex++)
    {
        pSpawnGroup = &pdefMonster->arMinionSpawnGroups[nSpawnGroupIndex];
        float fGroupWeight = pSpawnGroup->nWeight / dwTotalWeight;
        if (fTarget >= fCurrentWeight && fTarget < fCurrentWeight + fGroupWeight)
        {
            break;
        }
        fCurrentWeight += fGroupWeight;
    }


    if (pSpawnGroup == NULL)
    {
        return;
    }


    for (int32 i = 0; i < pSpawnGroup->nSpawnItemCount; i++)
    {
        const MonsterMinionSpawnItem & tSpawnItem = pSpawnGroup->arSpawnItems[i];
        ActorAddStruct tActorMinionAdd(tActorAddOriginal);
        tActorMinionAdd.ptSavedAI = NULL;
        CRefString sMinionName = CRefString().Format("%s-Minion-%d", MarkerNameGetString(tActorAddOriginal.tMarkersName), i);
        tActorMinionAdd.tMarkersName = MarkerStringHash(sMinionName.str());

        SNO snoSpawn = tSpawnItem.snoSpawn;
        if (snoSpawn == SNO_NONE)
        {
            snoSpawn = tActorAddOriginal.snoActor;
        }

        tActorMinionAdd.snoActor = snoSpawn;
        tActorMinionAdd.tSNOName.Set(ACTOR_SNO, snoSpawn);
        tActorMinionAdd.eMonsterRarity = MR_RARE_MINION;




        tActorMinionAdd.tMarkerOwner.Initialize();


        int32 nNumToSpawn = RandRange_Internal(rng, tSpawnItem.nSpawnCountMin, tSpawnItem.nSpawnCountMax - tSpawnItem.nSpawnCountMin);

        nNumToSpawn = (int32)(nNumToSpawn * sGetMinionSpawnCountMultipler(tACD));
        for (int32 j = 0; j < nNumToSpawn; j++)
        {
            ActorFindAddLocation(tACD, PASSABILITY_MONSTER, tActorMinionAdd);
            sMonsterAddByIndex(tActorMinionAdd, tEncounterAdd);
        }
    }
}




ActorID sMonsterAddByIndex(
    const ActorAddStruct & tActorAddOriginal,
    EncounterAddStruct & tEncounterAdd)
{
    Stopwatch tStopwatch;

    ActorAddStruct tActorAdd(tActorAddOriginal);


    ScopedActorDefinitionLockR pdefActor(tActorAdd.tSNOName.snoHandle);

    if (pdefActor->eType != ACTORTYPE_MONSTER)
    {
        return ACTORID_INVALID;
    }




    if (!MonsterExists(pdefActor->snoMonster))
    {
        return ACTORID_INVALID;
    }





    ScopedMonsterDefinitionLockR pdefMonster(pdefActor->snoMonster);
    if (!pdefMonster)
    {
        return ACTORID_INVALID;
    }

















    if (pdefMonster->dwFlags & (1 << MONSTERDEF_IS_UNIQUE_BIT))
    {

        tActorAdd.eMonsterRarity = MR_UNIQUE;
    }

    if (pdefMonster->dwFlags & (1 << MONSTERDEF_IS_BOSS_BIT))
    {
        tActorAdd.eMonsterRarity = MR_BOSS;
    }

    tActorAdd.snoActor = tActorAdd.tSNOName.snoHandle;

    MonsterType eMonsterType = pdefMonster->eMonsterType;
    if (eMonsterType == MT_SCENERY)
    {
        ActorID idActorScenery = SceneryAdd(tActorAdd);
        return idActorScenery;
    }

    if (tActorAdd.snoActor == SNO_NONE)
    {
        return ACTORID_INVALID;
    }



    ActorBrain nBrain = (ActorBrain)TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_DEFAULT_BRAIN);
    if (nBrain == ACTORBRAIN_NOBRAIN)
    {
        return ACTORID_INVALID;
    }



    BOOL bSuccess = ActorGetSpawnParams(tActorAdd);
    if (!bSuccess)
    {
        return ACTORID_INVALID;
    }

    if (tActorAdd.eTeam == TEAM_INVALID || tActorAdd.eTeam == TEAM_SCENERY)
    {

        switch (eMonsterType)
        {
        case MT_BREAKABLE:
            tActorAdd.eTeam = TEAM_INVALID;
            break;
        case MT_ALLY:
            tActorAdd.eTeam = TEAM_PLAYER_FIRST;
            break;
        default:
            tActorAdd.eTeam = TeamGetForActorSNO(tActorAdd.snoActor);
            break;
        }
    }

    BOOL fIsHireling = FALSE;
    for (int32 i = HC_FIRST; i < NUM_HIRELING_CLASSES; i++)
    {
        if (HirelingGetGBID((HirelingClass)i) == tActorAdd.hGB.gbid)
        {
            fIsHireling = TRUE;
            break;
        }
    }

    tActorAdd.eFastAttribType = fIsHireling ? FAT_COMPLEX_ACTOR : FAT_ACTOR;

    ActorID idActor = ActorAdd(tActorAdd);

    if (idActor == ACTORID_INVALID)
    {
        return ACTORID_INVALID;
    }

    ACDID idACD = ACDFindByActor(idActor);

    ActorCommonData & tACD = ACDGet(idACD);
    tACD.eMonsterRarity = tActorAdd.eMonsterRarity;

    sAttributesInitForMonster(tACD, tActorAdd);

    if (!(tActorAdd.dwActorAddFlags & (1 << ACTORADD_NO_AFFIXES)))
    {
        MonsterAddAffixes(idActor, tActorAdd, tEncounterAdd);
    }




















    sMonsterPutInPack(tACD, tActorAdd, tEncounterAdd);

    tACD.ResetLifeStats(FALSE, FALSE, TRUE, TRUE, FALSE);


    SNO snoInventory = ACDGetInventory(&tACD, FALSE);
    if (snoInventory != SNO_NONE)
    {
        SACDInventoryLoad(idACD, snoInventory);
    }

    snoInventory = ACDGetInventory(&tACD, TRUE);
    if (snoInventory != SNO_NONE)
    {
        SACDInventoryLoad(idACD, snoInventory);
    }

    if (tActorAdd.dwActorAddFlags & (1 << ACTORADD_NO_DAMAGE))
    {
        tACD.SetTakesNoDamage(TRUE);
    }

    if (tActorAdd.dwActorAddFlags & (1 << ACTORADD_INVULNERABLE))
    {
        tACD.SetInvulnerable(TRUE);
    }

    if (tActorAdd.dwActorAddFlags & (1 << ACTORADD_DOES_FAKE_DAMAGE))
    {
        tACD.SetDoesFakeDamage(TRUE);
    }

    MonsterDoOnCreatePowers(idACD);

    if (tActorAdd.dwSpawnAnimTag != (uint32)-1)
    {
        PowerAPI_StartSpawnAnimPower(idACD, tActorAdd.dwSpawnAnimTag);
    }

    if (tActorAdd.eMonsterRarity == MR_RARE && tActorAdd.ptSavedAI == NULL)
    {
        sMonsterSpawnMinions(tACD, tActorAddOriginal, tEncounterAdd);
    }

    if (!(tActorAdd.dwActorAddFlags & (1 << ACTORADD_IS_FROM_INACTIVE)) &&

        !tACD.IsPlayerTeam())
    {
        SGameStatsCounterIncrement(STAT_COUNTER_GAME_MONSTER_SPAWNED, tACD.GetActorSNO(), tACD.eMonsterRarity, 1.0f);

        if (tActorAdd.eMonsterRarity != MR_RARE_MINION)
        {
            for (int32 i = 0; i < 8; i++)
            {
                GBID gbidMonsterAffix = tACD.pMonsterAffixGBIDs[i];
                if (gbidMonsterAffix != GBID_NONE)
                {
                    SGameStatsCounterIncrement(STAT_COUNTER_GAME_MONSTER_AFFIX_SPAWNED, gbidMonsterAffix, tACD.GetActorSNO(), 1.0f);
                }
            }
        }
    }


    SWorld * ptSWorld = SWorldTryToGet(tACD.GetSWorldID());
    if (ptSWorld && tACD.IsMonster() && tACD.IsMonsterTeam())
    {

        if (ptSWorld->bChallengeMonstersCleared)
        {
            tACD.SetEnabled(FALSE);
        }
        if (ptSWorld->snoWorldMonsterPower != SNO_NONE)
        {
            PowerStart(idACD, ptSWorld->snoWorldMonsterPower, cgTargetNone);
        }
    }

    if (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_VERBOSE_POWER_LOGGING))
    {
        float flTime = tStopwatch.Check();
    }

    return idActor;
}





void EncounterAddByLevelArea(SNO snoEncounter, ActorAddStruct & tActorAdd)
{



    SNOName tSNOName;
    Team eTeam;
    BOOL bSpawn = EncounterResolveToActorSpawnInfo(snoEncounter, SGameGlobalsGet().rng, tSNOName, eTeam);

    tActorAdd.eTeam = eTeam;

    if (!bSpawn || tSNOName.snoGroup == SNOGROUP_NONE)
    {
        return;
    }

    if (tActorAdd.idLevelArea == LEVELAREAID_INVALID)
    {
        if (!NavMeshHandleIsNone(tActorAdd.tNavMeshHandle))
        {
            tActorAdd.idLevelArea = NavMeshHandleGetLevelAreaID(tActorAdd.tNavMeshHandle, FALSE);
        }
        else if (tActorAdd.idMarkersSScene != SSCENEID_INVALID)
        {


            tActorAdd.idLevelArea = ScenesCommonLevelAreaIDFromPlace(tActorAdd.idMarkersSScene, Vector3D(tActorAdd.tPlace.wp));
        }
    }






    if (sgtAppGlobals.dwDebugFlags & (1 << APP_GLOBAL_TRACE_SPECIAL_MARKER_CREATION))
    {
        CRefString sEncounter = SNOToString(ENCOUNTER_SNO, snoEncounter, FALSE);
        CRefString sLevelArea("Unknown");
        CRefString sScene("Unknown");

        SScene * ptSScene = SSceneTryToGet(tActorAdd.idMarkersSScene);

        if (ptSScene)
        {
            sScene = HyperlinkCreateFromSNO(SNOName(SCENE_SNO, ptSScene->tSCD.snoScene), NULL);
        }

        SNO snoLevelArea = LevelAreaGetSNO(tActorAdd.idLevelArea);

        if (snoLevelArea != SNO_NONE)
        {
            sLevelArea = HyperlinkCreateFromSNO(SNOName(LEVELAREA_SNO, snoLevelArea), NULL);
        }

        CRefString sGroup(SNOGroupGetName(tSNOName.snoGroup));



    }

    tActorAdd.tSNOName = tSNOName;

    if (tActorAdd.tSNOName.snoGroup == ACTOR_SNO)
    {
        tActorAdd.snoActor = tActorAdd.tSNOName.snoHandle;

        ScopedActorDefinitionLockR pdefActor(tActorAdd.snoActor);
        if (pdefActor->dwFlags & (1 << ACTORDEF_HAS_RANDOM_FACING_BIT))
        {
            tActorAdd.qOrientation.FromYaw(RandAngle(SGameGlobalsGet().rng));
        }
    }

    GBAdd(tActorAdd);
}




ActorID GizmoAddBySNO(int32 nLevel, ActorAddStruct & tActorAdd)
{

    tActorAdd.eFastAttribType = 1;

    sActorGetSpawnPosition(tActorAdd, NULL);

    ActorID idActor = ActorAdd(tActorAdd);

    if (idActor == ACTORID_INVALID)
    {
        return ACTORID_INVALID;
    }

    Actor * ptActor = ActorGet(idActor);
    ActorCommonData & tACD = ACDGet(ptActor->idACD);
    int32 nCorrectLevel = ActorGetCorrectLevel(SNONAME_NONE, nLevel);
    sAttributesInitForGizmo(ptActor, tACD, tActorAdd, nCorrectLevel);





















    return idActor;
}



ActorID PropAddBySNO(ActorAddStruct & tActorAdd)
{

    tActorAdd.eFastAttribType = 1;
    sActorGetSpawnPosition(tActorAdd, NULL);

    ActorID idActor = ActorAdd(tActorAdd);
    Actor * ptActor = ActorTryToGet(idActor);
    if (ptActor)
    {

        sInitFadeAttribute(*ptActor);
    }
    return idActor;
}



ActorID SceneryAdd(const ActorAddStruct & tActorAddOriginal)
{
    ActorAddStruct tActorAdd(tActorAddOriginal);

    tActorAdd.dwActorAddFlags |= (1 << ACTORADD_BRAINLESS_BIT);

    BOOL bSuccess = ActorGetSpawnParams(tActorAdd);

    if (!bSuccess)
    {
        return ACTORID_INVALID;
    }



    tActorAdd.hGB.eType = GB_SCENERY;
    tActorAdd.hGB.gbid = tActorAdd.snoActor;
    tActorAdd.eFastAttribType = 0;

    ActorID idActor = ActorAdd(tActorAdd);

    return idActor;
}


void HeroCheckAreaAfterCreate(ActorID idActor)
{
    ActorCheckChangeLevelArea(idActor);
    ActorCheckNavMeshTypeChange(idActor, 0, SWORLDID_INVALID);
}




















ActorID GBAddWithEncounterAddStruct(ActorAddStruct & tActorAdd, EncounterAddStruct & tEncounterAdd)
{


    ScopedActorDefinitionLockR pdefActor(tActorAdd.snoActor);
    if (pdefActor->eType == ACTORTYPE_MONSTER &&
        !tActorAdd.bIsSummon)
    {
        SGame & tSGame = SGameManager::GameGet();
        if (SWorldIsTieredDungeonFinder(tActorAdd.tPlace.idSWorld) &&
            tSGame.ptSGameGlobals &&
            (tSGame.ptSGameGlobals->dwFlags2 & ((1 << SGAMEGLOBALS2_LOOT_RUN_BOSS_KILLED) | (1 << SGAMEGLOBALS2_LOOT_RUN_BOSS_ACTIVE))))
        {
            return ACTORID_INVALID;
        }
    }

    if (!pdefActor)
    {
        return ACTORID_INVALID;
    }



    ActorID idActor = ACTORID_INVALID;

    switch (pdefActor->eType)
    {
    case ACTORTYPE_MONSTER:
        if (pdefActor->snoMonster != SNO_NONE)
        {







            idActor = sMonsterAddByIndex(tActorAdd, tEncounterAdd);
        }
        break;

    case ACTORTYPE_SERVER_PROP:
        tActorAdd.snoActor = tActorAdd.tSNOName.snoHandle;
        idActor = PropAddBySNO(tActorAdd);
        break;
    default:
        if (tActorAdd.tSNOName.snoGroup == ACTOR_SNO && ActorGetGizmoType(tActorAdd.tSNOName.snoHandle) != ACTORGIZMO_UNDEFINED)
        {





            tActorAdd.snoActor = tActorAdd.tSNOName.snoHandle;
            idActor = GizmoAddBySNO(-1, tActorAdd);
        }


        else if (tActorAdd.hGB.eType != GB_ITEMS)
        {




            tActorAdd.snoActor = GameBalanceGetActorSNO(tActorAdd.hGB);

            GizmoType eGizmoType = (GizmoType)ActorDefinitionGetTagMapValueInt(TAG_ACTOR_GIZMO_TYPE, tActorAdd.snoActor);
            if (eGizmoType != ACTORGIZMO_UNDEFINED)
            {
                idActor = GizmoAddBySNO(-1, tActorAdd);
            }
            else
            {

                idActor = SceneryAdd(tActorAdd);
            }
        }
        else
        {



            D3::Items::Generator tGenerator;
            ItemGeneratorToDefaults(tGenerator);
            tActorAdd.hGB.Export(*tGenerator.mutable_gb_handle());
            tGenerator.set_seed(RandGetSeed(SGameGlobalsGet().rng));

            ACDID idACD = SItemGenerate(OnlineService::ItemId::INVALID,
                tGenerator,

                tActorAdd.tPlace,
                tActorAdd.flScale,
                tActorAdd.tMarkersName);


            ActorCommonData * ptACD = ACDTryToGet(idACD);
            if (ptACD)
            {
                idActor = (ActorID)ptACD->idOwner;
            }
        }
        break;
    }


    return idActor;
}




void EncounterAddInitialize(EncounterAddStruct & tEncounterAdd, GBHandleList & listGroupAffixes)
{
    tEncounterAdd.plistGroupAffixes = &listGroupAffixes;
    tEncounterAdd.eGroupAffixType = -1;
    tEncounterAdd.nRecursionLevel = 0;
    tEncounterAdd.idACDPack = -1;
}







ActorID GBAdd(ActorAddStruct & tActorAdd)
{

    uint32 dwGameFlags = SGameGlobalsGet().dwFlags;
    if ((dwGameFlags & (1 << SGAMEGLOBALS_MONSTER_SPAWNING_DISABLED_BIT)) && TeamIsMonsterTeam(tActorAdd.eTeam))
    {
        return ACTORID_INVALID;
    }

    ScopedActorDefinitionLockR pdefActor(tActorAdd.snoActor);
    if (pdefActor->eType == ACTORTYPE_MONSTER && !tActorAdd.bIsSummon)
    {
        SGame & tSGame = SGameManager::GameGet();
        if (SWorldIsTieredDungeonFinder(tActorAdd.tPlace.idSWorld) &&
            tSGame.ptSGameGlobals &&
            (tSGame.ptSGameGlobals->dwFlags2 & ((1 << SGAMEGLOBALS2_LOOT_RUN_BOSS_KILLED) | (1 << SGAMEGLOBALS2_LOOT_RUN_BOSS_ACTIVE))))
        {
            return ACTORID_INVALID;
        }
    }

    GBHandleList listGroupAffixes(GBGetHandlePool());
    EncounterAddStruct tEncounterAdd;
    EncounterAddInitialize(tEncounterAdd, listGroupAffixes);

    ActorID id = GBAddWithEncounterAddStruct(tActorAdd, tEncounterAdd);

    return id;
}




void ACDSetRareItemName(ActorCommonData & tACD, const RareItemName & tRareItemName)
{


    tACD.tRareItemName = tRareItemName;
}













void ActorChangeWorld(const ActorID idActor, const WorldPlace & tPlace, const Quaternion & q)
{


    Actor * ptActor = ActorGet(idActor);
    ActorCommonData & tACD = ACDGet(ptActor->idACD);

    SWorldID idSWorldPrev = tACD.GetSWorldID();
    uint32 uPrevNavMeshFlags = ptActor->uCurrNavMeshFlags;

    tACD.ChangeWorld(tPlace, q);

    if (ACDIsPlayer(ptActor->idACD))
    {
        Player * ptPlayer = PlayerGetByACD(ptActor->idACD);



        SWorld * ptSWorldPrev = SWorldTryToGet(idSWorldPrev);
        if (ptSWorldPrev && ptSWorldPrev->snoWorldPlayerPower != SNO_NONE)
        {
            PowerAPI_BuffFlagAllFromPowerForDeletion(tACD, ptSWorldPrev->snoWorldPlayerPower, -1, BUFF_REMOVAL_REASON_DEFAULT, ACDID_INVALID);
        }


        SWorld * ptSWorldNew = SWorldTryToGet(tPlace.idSWorld);
        if (ptSWorldNew && ptSWorldNew->snoWorldPlayerPower != SNO_NONE)
        {
            PowerStart(tACD.id, ptSWorldNew->snoWorldPlayerPower, cgTargetNone);
        }
    }

    ActorMovement * ptMovement = tACD.GetActorMovement();
    if (ptMovement)
    {
        ptMovement->ResetCache();
    }

    ActorCheckChangeLevelArea(ptActor->id);
    ActorCheckNavMeshTypeChange(ptActor->id, uPrevNavMeshFlags, idSWorldPrev);

    int32 nDungeonFinderLevel = SWorldGetTieredDungeonFinderLevel(tPlace.idSWorld);
    if (nDungeonFinderLevel >= 0)
    {
        FastAttribSetValueInt(tACD.idFastAttrib, FastAttribKey(ATTRIBUTE_IN_TIERED_LOOT_RUN_LEVEL), nDungeonFinderLevel);
        PVPPlayerRemoveShrineBuffs(tACD);


        PlayerList listPlayer(PlayerListGetElementPool());
        PlayerListGetSingle(listPlayer, tACD.id);
        SimpleMessage tMessage;
        MessageSendToClient(listPlayer, SMSG_FORCE_UI_RESET, &tMessage, sizeof(tMessage));

        Player * ptPlayer = PlayerGetByACD(ptActor->idACD);
        if (ptPlayer)
        {
            NephalemRiftManager::GreaterRiftRewardState * ptRewardState = NephalemRiftManager::PlayerGetGreaterRiftRewardState(ptPlayer);
            if (ptRewardState)
            {
                tACD.AttributesSetInt(FastAttribKey(ATTRIBUTE_TIERED_LOOT_RUN_REWARD_CHOICE_STATE), ptRewardState->eChoiceState);
                tACD.AttributesSetInt(FastAttribKey(ATTRIBUTE_JEWEL_UPGRADES_USED), ptRewardState->nJewelUpgradesUsed);
                tACD.AttributesSetInt(FastAttribKey(ATTRIBUTE_JEWEL_UPGRADES_MAX), ptRewardState->nJewelUpgradesMax);
                tACD.AttributesSetInt(FastAttribKey(ATTRIBUTE_JEWEL_UPGRADES_BONUS), ptRewardState->nJewelUpgradesBonus);
                tACD.AttributesSetInt(FastAttribKey(ATTRIBUTE_TIERED_LOOT_RUN_REWARD_RECEIVES_KEY), ptRewardState->bSuccess);
            }
            else
            {
                tACD.AttributesSetInt(FastAttribKey(ATTRIBUTE_TIERED_LOOT_RUN_REWARD_CHOICE_STATE), -1);
                tACD.AttributesSetInt(FastAttribKey(ATTRIBUTE_JEWEL_UPGRADES_USED), 0);
                tACD.AttributesSetInt(FastAttribKey(ATTRIBUTE_JEWEL_UPGRADES_MAX), 0);
                tACD.AttributesSetInt(FastAttribKey(ATTRIBUTE_JEWEL_UPGRADES_BONUS), 0);
                tACD.AttributesSetInt(FastAttribKey(ATTRIBUTE_TIERED_LOOT_RUN_REWARD_RECEIVES_KEY), 0);
            }
        }
    }
    else
    {
        FastAttribSetValueInt(tACD.idFastAttrib, FastAttribKey(ATTRIBUTE_IN_TIERED_LOOT_RUN_LEVEL), -1);
    }
}





void ActorTimerEffectsInitializeForGame()
{

    SGame & tSGame = SGameManager::GameGet();

    tSGame.ptTimedEffectsGlobals = (TimedEffectsGlobals *)SigmaMemoryNew(sizeof(TimedEffectsGlobals), 4, tSGame.ptMemoryInterface, TRUE);

    tSGame.ptTimedEffectsGlobals->poolTimedEffects.Initialize();
    tSGame.ptTimedEffectsGlobals->poolTimedEffects.SetMemoryInterface(tSGame.ptMemoryInterface);
    tSGame.ptTimedEffectsGlobals->poolTimedEffects.SetBlockCount_Tracked(1000);
}




void ActorTimerEffectsDisposeForGame()
{

    SGame & tSGame = SGameManager::GameGet();

    tSGame.ptTimedEffectsGlobals->poolTimedEffects.Dispose();
    SigmaMemoryFree(tSGame.ptTimedEffectsGlobals, tSGame.ptMemoryInterface); tSGame.ptTimedEffectsGlobals = NULL;
}



void ActorTimerEffectsInit(
    const ActorID idActor)
{
    TimedEffectsGlobals *& ptTimedEffectsGlobals = SGameManager::GameGet().ptTimedEffectsGlobals;

    Actor * ptActor = ActorGet(idActor);


    ptActor->ptTimedEffects = ptTimedEffectsGlobals->poolTimedEffects.Alloc_Tracked();
    for (int32 i = 0; i < NUM_ATTRIBUTE_TIMER_TYPES; i++)
    {

        ptActor->ptTimedEffects->arFastAttribs[i] = -1;
    }
}




void ActorTimerEffectsDispose(const ActorID idActor)
{

    Actor * ptActor = ActorGet(idActor);

    if (ptActor->ptTimedEffects)
    {

        for (int32 i = 0; i < NUM_ATTRIBUTE_TIMER_TYPES; i++)
        {
            ActorTimerEffectsExpireGroup(idActor, i);
        }

        TimedEffectsGlobals *& ptTimedEffectsGlobals = SGameManager::GameGet().ptTimedEffectsGlobals;
        ptTimedEffectsGlobals->poolTimedEffects.Free(ptActor->ptTimedEffects);
        ptActor->ptTimedEffects = NULL;
    }
}



AttribGroupID ActorTimerEffectsGetGroup(const ActorID idActor, AttributeTimerType eTimerType)
{
    const Actor * ptActor = ActorGet(idActor);
    if (ptActor->ptTimedEffects->arFastAttribs[eTimerType] == ATTRIBGROUPID_INVALID)
    {
        AttribGroupID idAttribGroupChild = FastAttribGroupCreate(0, -1);
        ACDGet(ptActor->idACD).AttributesMergeAttributeGroup(idAttribGroupChild);
        ptActor->ptTimedEffects->arFastAttribs[eTimerType] = idAttribGroupChild;
    }
    return ptActor->ptTimedEffects->arFastAttribs[eTimerType];
}









void ActorTimerEffectsExpireGroup(const ActorID idActor, AttributeTimerType eTimerType)
{
    const Actor * ptActor = ActorGet(idActor);
    AttribGroupID idAttribGroupChild = ptActor->ptTimedEffects->arFastAttribs[eTimerType];
    if (idAttribGroupChild != ATTRIBGROUPID_INVALID)
    {
        ActorCommonData & tACD = ACDGet(ptActor->idACD);
        tACD.AttributesUnmergeAttributeGroup(idAttribGroupChild);
        FastAttribGroupDelete(idAttribGroupChild);
        ptActor->ptTimedEffects->arFastAttribs[eTimerType] = ATTRIBGROUPID_INVALID;
    }
}










void ActorsMakeVisibleList(SWorldID idSWorld, const Frustum & tFrustum, const Frustum * ptAVFrustum, const Vector3D & vCenterPos, Player ** pptPlayer, int32 nPlayerCount)
{






    uint32 dwNow = GameGetServerTime();
    BOOL bInEditMode = GameModeIsAnEditMode(GameGetActiveGameCommonData().GameModeGet());

    XPooledDataIDList listACDs(MemoryPoolsGetDATAIDListPool());
    XBitArray<11776> tActorBitArray;
    ActorsIntersectFrustum(idSWorld, tFrustum, listACDs, &tActorBitArray, ACTOR_SPHERE_TREE_FLAGS_ALL);
    if (ptAVFrustum)
    {


        ActorsIntersectFrustum(idSWorld, *ptAVFrustum, listACDs, &tActorBitArray, ACTOR_SPHERE_TREE_FLAGS_ALL);
    }


    BOOL bInAVCameraModes[4];
    for (int32 i = 0; i < nPlayerCount; i++)
    {


        BOOL bInAVCameraMode = (pptPlayer[i]->uServerFlags & (1 << PSF_IN_AV_CAMERA_MODE)) || ReplayIsUsingRecordedAVFrustum();

        bInAVCameraModes[i] = bInAVCameraMode;
    }





    POS pos = listACDs.GetHeadPosition();
    while (pos)
    {
        ACDID idACD = (ACDID)listACDs.GetNext(pos);

        ActorCommonData & tACD = ACDGet(idACD);
        SSceneID idSScene = ACDGetCurrentSSceneID(tACD);

        for (int32 i = 0; i < nPlayerCount; i++)
        {
            Player * ptPlayer = pptPlayer[i];
            int32 nPlayerIndex = ptPlayer->nPlayerIndex;
            BOOL bInAVCameraMode = bInAVCameraModes[i];

            if (idSScene != SSCENEID_INVALID)
            {
                if (SceneCommonIsExcluded(idSScene, nPlayerIndex))
                {
                    continue;
                }


                SceneCommonPlayerSees(idSScene, nPlayerIndex, TRUE, TRUE);
            }

            if (!bInAVCameraMode)
            {

                if (tACD.IsMonster() &&
                    Vector3D::DistanceSquared(vCenterPos, tACD.GetWorldLocation().transform.wp) > 120.0f * 120.0f)
                {
                    continue;
                }
            }

            if (!bInEditMode && (tACD.dwFlags & (1 << ACD_EDIT_MODE_ONLY_BIT)))
            {
                continue;
            }

            tACD.SetTimeStamp(dwNow);

            if (tACD.dwPlayerLastSeenTime != dwNow)
            {
                tACD.dwPlayerSeenBits = 0;
                tACD.dwPlayerLastSeenTime = dwNow;
            }








            ptPlayer->listVisibleACDs.AddTail_Tracked(idACD);
            tACD.dwPlayerSeenBits |= (1 << nPlayerIndex);
        }
    }
}


void ActorsAddFastMoversToVisibleList(SWorldID idSWorld, const Frustum & tFrustum, Player ** pptPlayer, int32 nPlayerCount)
{




    if (nPlayerCount == 0)
    {
        return;
    }

    uint32 dwNow = GameGetServerTime();

    XPooledDataIDList listACDs(MemoryPoolsGetDATAIDListPool());
    ActorsIntersectFrustum(idSWorld, tFrustum, listACDs, NULL, 1 << ACTOR_SPHERE_TREE_FAST_MOVER);

    POS pos = listACDs.GetHeadPosition();
    while (pos)
    {
        ACDID idACD = (ACDID)listACDs.GetNext(pos);
        ActorCommonData & tACD = ACDGet(idACD);

        SSceneID idSScene = ACDGetCurrentSSceneID(tACD);

        for (int32 i = 0; i < nPlayerCount; i++)
        {
            Player * ptPlayer = pptPlayer[i];
            int32 nPlayerIndex = ptPlayer->nPlayerIndex;

            if (idSScene != SSCENEID_INVALID)
            {
                if (SceneCommonIsExcluded(idSScene, nPlayerIndex))
                {
                    continue;
                }


                SceneCommonPlayerSees(idSScene, nPlayerIndex, TRUE, TRUE);
            }

            tACD.SetTimeStamp(dwNow);
            tACD.dwPlayerLastSeenTime = dwNow;
            if (!(tACD.dwPlayerSeenBits & (1 << nPlayerIndex)))
            {
                ptPlayer->listVisibleACDs.AddTail_Tracked(idACD);
                tACD.dwPlayerSeenBits |= (1 << nPlayerIndex);
                KnownListItemChanged(KET_ACD, tACD.id, TRUE);
            }
        }
    }
}




void ActorsMakePrefetchList(SWorldID idSWorld, const Frustum & tFrustum, Player ** pptPlayer, int32 nPlayerCount)
{




    if (nPlayerCount == 0)
    {
        return;
    }

    XPooledDataIDList listACDs(MemoryPoolsGetDATAIDListPool());
    ActorsIntersectFrustum(idSWorld, tFrustum, listACDs, NULL, ACTOR_SPHERE_TREE_FLAGS_ALL);


    for (int32 i = 0; i < nPlayerCount; i++)
    {
        Player * ptPlayer = pptPlayer[i];
        POS pos = listACDs.GetHeadPosition();
        while (pos)
        {
            ACDID idACD = (ACDID)listACDs.GetNext(pos);
            ActorCommonData * ptACD = ACDTryToGet(idACD);
            if (ptACD && !NPCShouldBeHiddenForClient(*ptACD, ptPlayer))
            {
                ptPlayer->listPrefetchableACDs.AddTail_Tracked(idACD);
            }
        }
    }
}




static void sActorDeactivateSingle(ActorID idActor, BOOL bDelete)
{

    Actor * ptActor = ActorGet(idActor);



    ScopedActorDefinitionLockR pdefActor(ptActor->snoActor);
    if ((ActorFlagIsSet(idActor, ACTOR_DYING) &&
         TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_CORPSE_DELETES_INSTEAD_OF_INACTIVE)) ||
        ActorFlagIsSet(idActor, ACTOR_DELETES_INSTEAD_OF_INACTIVE_BIT))
    {
        bDelete = TRUE;
    }
    else
    {
        ActorFlagSet(idActor, ACTOR_DEACTIVATING, TRUE);

        InactiveMarkerCreateFromActor(idActor);
    }


    if (bDelete)
    {

        ActorDelete(idActor);
    }
}





static void sActorPackDeactivate(ACDPack & tACDPack)
{

    const ACDIDListPooled * listPack = tACDPack.GetMemberList();

    POS pos = listPack->GetHeadPosition();

    while (pos)
    {
        ACDID idACD = (ACDID)listPack->GetNext(pos);
        ActorID idActor = (ActorID)ACDGetOwner(idACD);
        sActorDeactivateSingle(idActor, TRUE);
    }
}




void ActorDeactivate(ActorID idActor, BOOL bDeleteActor)
{

    ACDID idACD = ACDFindByActor(idActor);
    ActorCommonData & tACD = ACDGet(idACD);
    ACDPack * ptACDPack = ACDPackTryToGet(tACD.idACDPack);
    if (ptACDPack)
    {
        sActorPackDeactivate(*ptACDPack);
    }
    else
    {
        sActorDeactivateSingle(idActor, bDeleteActor);
    }
}





void ActorDeactivateUnseen()
{
    uint32 dwNow = GameGetServerTime();
    ACDIterator tIt;

    while (ActorCommonDataIterate(tIt))
    {
        ActorCommonData * ptACD = tIt.ptElement;

        if (ptACD == NULL)
        {
            continue;
        }

        if (!ptACD->IsInWorld())
        {
            continue;
        }

        if (ptACD->dwPlayerLastSeenTime != dwNow)
        {
            ptACD->dwPlayerSeenBits = 0;
        }

        ActorID idActor = (ActorID)ptACD->idOwner;
        DeactivateStatus nStatus = MarkerCanDeactivateActor(idActor);

        if (nStatus == DEACTIVATE_YES)
        {
            ActorDeactivate(idActor, TRUE);
        }
    }
}







void ActorDeactivateInScene(SSceneID idSScene)
{
    ActorIterator tIt;
    while (ActorDataArrayIterate(tIt))
    {
        Actor * ptActor = tIt.ptElement;
        SSceneID idSSceneCurrent = ACDGetCurrentSSceneID(ACDGet(ptActor->idACD));
        if (idSSceneCurrent == idSScene)
        {
            DeactivateStatus nStatus = MarkerCanDeactivateActor(ptActor->id);
            if (nStatus == DEACTIVATE_YES || nStatus == DEACTIVATE_ONLY_WITH_SCENE)
            {
                ActorDeactivate(ptActor->id, TRUE);
            }
        }
    }
}











void ActorFlagSet(const ActorID idActor, const ActorFlag eFlag, const BOOL bValue)
{
    Actor * ptActor = ActorGet(idActor);
    if (bValue)
        ptActor->dwActorFlags |= (1 << eFlag);
    else
        ptActor->dwActorFlags &= ~(1 << eFlag);
}



BOOL ActorFlagIsSet(const ActorID idActor, const ActorFlag eFlag)
{
    Actor * ptActor = ActorGet(idActor);
    if (ptActor == NULL)
    {
        return FALSE;
    }
    return (ptActor->dwActorFlags >> eFlag) & 1;
}














void ActorPushSpawnSkipDistance(int32 nSkipDistMin, int32 nSkipDistDelta)
{
    ActorGlobals * ptActorGlobals = SGameManager::GameGet().ptActorGlobals;
    ActorSpawnSkipDistance & tSpawnSkipDistance = ptActorGlobals->arActorSpawnSkipDistanceStack[ptActorGlobals->nActorSpawnSkipDistanceNum];
    tSpawnSkipDistance.nSkipDistanceMin = nSkipDistMin;
    tSpawnSkipDistance.nSkipDistanceDelta = nSkipDistDelta;
    ptActorGlobals->nActorSpawnSkipDistanceNum++;
}













void ActorPopSpawnSkipDistance()
{
    ActorGlobals * ptActorGlobals = SGameManager::GameGet().ptActorGlobals;
    if (ptActorGlobals->nActorSpawnSkipDistanceNum > 0)
    {
        ptActorGlobals->nActorSpawnSkipDistanceNum--;
    }
}




void ActorGetSpawnSkipDistance(int32 & nSkipDistMin, int32 & nSkipDistDelta)
{

    ActorGlobals * ptActorGlobals = SGameManager::GameGet().ptActorGlobals;

    if (ptActorGlobals->nActorSpawnSkipDistanceNum > 0)
    {
        ActorSpawnSkipDistance & tSpawnSkipDistance = ptActorGlobals->arActorSpawnSkipDistanceStack[ptActorGlobals->nActorSpawnSkipDistanceNum - 1];
        nSkipDistMin = tSpawnSkipDistance.nSkipDistanceMin;
        nSkipDistDelta = tSpawnSkipDistance.nSkipDistanceDelta;
    }
    else
    {
        nSkipDistMin = sgcSkipDistanceMin;
        nSkipDistDelta = sgcSkipDistanceDelta;
    }
}






void PassabilitySettingsInit(PassabilitySettings & tPassability)
{
    tPassability.ePassabilityType = -1;
    tPassability.snoActor = SNO_NONE;
    tPassability.aYaw = 0.0f;
    tPassability.flScale = 1.0f;
}















void ActorsAdjustForPlayerChanged(const int32 nOldPlayerCount, const int32 nNewPlayerCount)
{

    int32 nOldPlayerMult = Math::Max(nOldPlayerCount - 1, 0);
    int32 nNewPlayerMult = Math::Max(nNewPlayerCount - 1, 0);

    if (nOldPlayerMult == nNewPlayerMult)
    {
        return;
    }

    SNO snoBossEncounter = BossEncounterManager::GetActiveEncounterSNO();
    ActorIterator tIt;
    while (Actor * ptActor = ActorDataArrayIterate(tIt))
    {

        ActorCommonData * ptACD = ACDTryToGet(ptActor->idACD);

        if (ptACD &&
            ptACD->IsMonster() &&
            !ACDIsHireling(*ptACD) &&
            !ACDIsInTieredLootRun(*ptACD))
        {
            float flOldHPMax = ptACD->AttributesGetFloat(FastAttribKey(ATTRIBUTE_HITPOINTS_MAX));
            if (flOldHPMax > EPSILON)
            {
                float flOldHP = ptACD->GetHitPointsCurrent();

                if (snoBossEncounter != SNO_NONE)
                {
                    SWorldID idSWorld = ptACD->GetSWorldID();
                    SWorld * ptSWorld = SWorldTryToGet(idSWorld);
                    if (ptSWorld && ptSWorld->snoBossEncounter == snoBossEncounter)
                    {
                        continue;
                    }
                }







                BOOL bFullHealth = flOldHP >= flOldHPMax - EPSILON;




                float flHPMax = AdjustHPForPlayerCount(flOldHPMax, nOldPlayerMult, nNewPlayerMult);
                ptACD->SetHitPointsMax(flHPMax);

                if (bFullHealth)
                {
                    ptACD->SetHitPoints(ptACD->GetHitPointsMaxTotal(), FALSE, FALSE);
                }
                else
                {
                    float flHP = AdjustHPForPlayerCount(flOldHP, nOldPlayerMult, nNewPlayerMult);
                    ptACD->SetHitPoints(flHP, FALSE, FALSE);
                }
            }
        }
    }
}



static float sGetHandicapHealthConversionMult(const int32 nOldDifficulty, const int32 nNewDifficulty)
{
    float flMult = 1.0f;
    if (nOldDifficulty != nNewDifficulty)
    {
        if (nOldDifficulty >= 0 && nOldDifficulty <= 19 && nNewDifficulty >= 0 && nNewDifficulty <= 19)
        {
            float flOld = ActorGetHandicapHealthScalar(nOldDifficulty);
            if (flOld > 1e-06f)
                flMult = ActorGetHandicapHealthScalar(nNewDifficulty) / flOld;
        }
    }
    return flMult;
}












void ActorAdjustHPForDifficultyChanged(const int32 nOldDifficulty, const int32 nNewDifficulty, float & flHP)
{
    float flConversionMult = sGetHandicapHealthConversionMult(nOldDifficulty, nNewDifficulty);
    flHP *= flConversionMult;
}






void ActorsAdjustForDifficultyChanged(const int32 nOldDifficulty, const int32 nNewDifficulty)
{

    float flConversionMult = sGetHandicapHealthConversionMult(nOldDifficulty, nNewDifficulty);

    ActorIterator tIt;
    while (Actor * ptActor = ActorDataArrayIterate(tIt))
    {

        ActorCommonData * ptACD = ACDTryToGet(ptActor->idACD);

        if (ptACD &&
            ptACD->IsMonster() &&
            !ACDIsHireling(*ptACD) &&
            !ACDIsInTieredLootRun(*ptACD))
        {
            float flOldHPMax = ptACD->AttributesGetFloat(FastAttribKey(ATTRIBUTE_HITPOINTS_MAX));
            if (flOldHPMax > 1e-06f)
            {
                float flOldHP = ptACD->GetHitPointsCurrent();





                BOOL bFullHealth = flOldHP >= flOldHPMax - 1e-06f;




                float flHPMax = flConversionMult * flOldHPMax;
                ptACD->SetHitPointsMax(flHPMax);

                if (bFullHealth)
                {
                    ptACD->SetHitPoints(flHPMax, FALSE, FALSE);
                }
                else
                {

                    float flHP = flConversionMult * flOldHP;
                    ptACD->SetHitPoints(flHP, FALSE, FALSE);
                }
            }
        }
    }
}

































BOOL ActorHasQuestLifespan(const Actor & tActor)
{

    return ActorGetQuestLifespan(tActor) != SNO_NONE;
}



BOOL ActorIsQuestLifespanValid(const Actor & tActor)
{
    SNO snoQuestRange = ActorGetQuestLifespan(tActor);
    if (snoQuestRange != SNO_NONE && !QuestRangeEvaluate(snoQuestRange, NULL))
    {
        return FALSE;
    }
    return TRUE;
}





SNO ActorGetQuestLifespan(const Actor & tActor)
{



    ActorCommonData & tACD = ACDGet(tActor.idACD);
    SNO snoQuestRange = tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_QUEST_RANGE));
    if (snoQuestRange != SNO_NONE)
    {
        return snoQuestRange;
    }


    if (tActor.tMarkerOwner.snoMarkerSet != SNO_NONE)
    {

        ScopedMarkerSetDefinitionLockR ptMarkerSetDef(tActor.tMarkerOwner.snoMarkerSet);
        if (tActor.tMarkerOwner.nMarkerIndex >= 0 && tActor.tMarkerOwner.nMarkerIndex < ptMarkerSetDef->tMarkerSet.GetArraySize())
        {

            const Marker & tMarker = ptMarkerSetDef->tMarkerSet[tActor.tMarkerOwner.nMarkerIndex];
            return TagMapGetSNO(tMarker.hTagMap, TAG_MARKER_QUESTRANGE);
        }
    }














    return SNO_NONE;
}
void ActorAddToQuestLifespanActors(ActorID idActor)
{
    ActorGlobals * ptActorGlobals = SGameManager::GameGet().ptActorGlobals;
    POS pos = ptActorGlobals->listActorsWithQuestLifespan.Find(idActor);
    if (pos == NULL)
    {
        ptActorGlobals->listActorsWithQuestLifespan.AddTail_Tracked(idActor);
    }
}




void ActorDeleteQuestLifespanActors()
{
    ActorGlobals * ptActorGlobals = SGameManager::GameGet().ptActorGlobals;
    POS pos = ptActorGlobals->listActorsWithQuestLifespan.GetHeadPosition();
    while (pos)
    {
        ActorID idActor = (ActorID)ptActorGlobals->listActorsWithQuestLifespan.GetNext(pos);
        Actor * ptActor = ActorTryToGet(idActor);
        if (ptActor)
        {
            SNO snoQuestRange = ActorGetQuestLifespan(*ptActor);
            if (!QuestRangeEvaluate(snoQuestRange, NULL))
            {
                if (!ActorIsUsingAIQueue(idActor) && !ActorFlagIsSet(idActor, ACTOR_DYING))
                {
                    ActorDeactivateFromQuestRange(idActor);
                }

                if (AppDrawFlagIsSet(APP_DRAW_DEBUG_QUESTS_BIT))
                {
                }
            }
        }
    }
}

















void ActorRefreshHirelingKnownActors(HirelingClass eClass)
{
    ActorGlobals * ptActorGlobals = SGameManager::GameGet().ptActorGlobals;
    POS pos = ptActorGlobals->listActorsWithHirelingClass.GetHeadPosition();
    while (pos)
    {
        ActorID idActor = (ActorID)ptActorGlobals->listActorsWithHirelingClass.GetNext(pos);
        Actor * ptActor = ActorTryToGet(idActor);
        if (ptActor)
        {
            ActorCommonData & tACD = ACDGet(ptActor->idACD);
            if (ACDGetHirelingClass(tACD) == eClass)
            {
                KnownListItemChanged(KET_ACD, tACD.id, FALSE);
            }
        }
    }
}



void ActorRefreshActivatedWaypoints()
{

    ActorGlobals * ptActorGlobals = SGameManager::GameGet().ptActorGlobals;
    POS pos = ptActorGlobals->listInactiveWaypoints.GetHeadPosition();
    while (pos)
    {
        POS posCur = pos;
        ActorID idActor = (ActorID)ptActorGlobals->listInactiveWaypoints.GetNext(pos);
        Actor * ptActor = ActorTryToGet(idActor);
        if (ptActor)
        {
            ActorCommonData & tACD = ACDGet(ptActor->idACD);
            SNO snoLevelArea;
            if (GizmoIsWaypointActivated(tACD, TRUE, &snoLevelArea, NULL))
            {
                WaypointActivatedMessage tMessage;

                tMessage.annWaypoint = ANNFromACD(tACD.id);
                tMessage.annActivatingPlayer = ANN_INVALID;
                tMessage.snoLevelArea = snoLevelArea;
                tMessage.bAnnounce = FALSE;

                {

                    PlayerList listDestPlayers(PlayerListGetElementPool());
                    PlayerListGetAllInGame(listDestPlayers, -1, FALSE, GAMECONNECTIONID_INVALID);
                    MessageSendToClient(listDestPlayers, SMSG_ACTIVATE_WAYPOINT, &tMessage, sizeof(tMessage));
                }

                ptActorGlobals->listInactiveWaypoints.RemoveAt(posCur);
            }
        }
    }
}



void ActorRemoveWaypointFromInactiveList(const ActorID idActor)
{
    ActorGlobals * ptActorGlobals = SGameManager::GameGet().ptActorGlobals;

    POS pos = ptActorGlobals->listInactiveWaypoints.Find(idActor);
    if (pos != NULL)
    {
        ptActorGlobals->listInactiveWaypoints.RemoveAt(pos);
    }
}



float ActorGetInheritedTagMapValueFloat(const Actor * ptActor, const uint32 dwActorTag, const uint32 dwMarkerTag)
{
    return ActorGetInheritedTagMapValueFloat(ptActor->snoActor, MinimapMarkerSetInfo(ptActor->tMarkerOwner), dwActorTag, dwMarkerTag);
}





int32 ActorGetInheritedTagMapValueInt(const Actor * ptActor, const uint32 dwActorTag, const uint32 dwMarkerTag)
{
    return ActorGetInheritedTagMapValueInt(ptActor->snoActor, MinimapMarkerSetInfo(ptActor->tMarkerOwner), dwActorTag, dwMarkerTag);
}





uint32 ActorGetInheritedTagMapValueDWORD(const Actor * ptActor, const uint32 dwActorTag, const uint32 dwMarkerTag)
{
    return ActorGetInheritedTagMapValueDWORD(ptActor->snoActor, MinimapMarkerSetInfo(ptActor->tMarkerOwner), dwActorTag, dwMarkerTag);
}





SNO ActorGetInheritedTagMapValueSNO(const Actor * ptActor, const uint32 dwActorTag, const uint32 dwMarkerTag)
{
    return ActorGetInheritedTagMapValueSNO(ptActor->snoActor, MinimapMarkerSetInfo(ptActor->tMarkerOwner), dwActorTag, dwMarkerTag);
}





GBID ActorGetInheritedTagMapValueGBID(const Actor * ptActor, const uint32 dwActorTag, const uint32 dwMarkerTag)
{
    return ActorGetInheritedTagMapValueGBID(ptActor->snoActor, MinimapMarkerSetInfo(ptActor->tMarkerOwner), dwActorTag, dwMarkerTag);
}




























void ActorUpdateServerVariables()
{
    ActorGlobals * ptActorGlobals = SGameManager::GameGet().ptActorGlobals;
    if (ptActorGlobals == NULL)
    {
        return;
    }

    ptActorGlobals->uAttributeUpdateInterval = s_varAttributeUpdateTicks.Get();
    ptActorGlobals->uMaxActorSoftCap = s_varMaxActorSoftCap.Get();
    ptActorGlobals->flGarbageCollectionThresholdPct = s_varGarbageCollectionThresholdPercent.Get();
    ptActorGlobals->flGarbageCollectionSevereThresholdPct = s_varGarbageCollectionSevereThresholdPercent.Get();
    ptActorGlobals->uGarbageCollectionEmergencyCleanupAmount = s_varGarbageCollectionEmergencyCleanupAmount.Get();
    ptActorGlobals->flThrottleScriptActorsThresholdPct = s_varThrottleScriptActorsThresholdPercent.Get();
}

uint32 ActorGetAttributeUpdateTicks()
{
    ActorGlobals * ptActorGlobals = SGameManager::GameGet().ptActorGlobals;
    return ptActorGlobals->uAttributeUpdateInterval;
}




BOOL ActorShouldThrottleScriptActors()
{
    ActorGlobals * ptActorGlobals = SGameManager::GameGet().ptActorGlobals;
    uint32 uActorCount = ptActorGlobals->ptActors->dwTotalCount;
    uint32 uThrottleScriptActorsThreshold = (uint32)(ptActorGlobals->uMaxActorSoftCap * ptActorGlobals->flThrottleScriptActorsThresholdPct);
    return uActorCount >= uThrottleScriptActorsThreshold;
}



int32 ActorGlobalsGetNewAttributeUpdateBucketIndex()
{
    ActorGlobals * ptActorGlobals = SGameManager::GameGet().ptActorGlobals;
    if (ptActorGlobals)
    {
        int32 nLowestIndex = -1;
        uint32 uLowestValue = 0xFFFFFFFF;
        for (int32 i = 0; i < 20; i++)
        {
            if (ptActorGlobals->arAttributeUpdateBuckets[i] < uLowestValue)
            {
                nLowestIndex = i;
                uLowestValue = ptActorGlobals->arAttributeUpdateBuckets[i];
            }
        }
        ptActorGlobals->arAttributeUpdateBuckets[nLowestIndex]++;
        return nLowestIndex;
    }
    return -1;
}







void ActorGlobalsDecrementAttributeUpdateBucketValue(const int32 nBucket)
{

    ActorGlobals * ptActorGlobals = SGameManager::GameGet().ptActorGlobals;
    if (nBucket >= 0 && nBucket < 20 && ptActorGlobals)
    {
        if (ptActorGlobals->arAttributeUpdateBuckets[nBucket] > 0)
        {
            ptActorGlobals->arAttributeUpdateBuckets[nBucket]--;
        }
    }
}


































































void ActorGlobalsDecrementAreaDamageUpdateBucketValue(const int32 nBucket)
{

    ActorGlobals * ptActorGlobals = SGameManager::GameGet().ptActorGlobals;
    if (nBucket >= 0 && nBucket < 20 && ptActorGlobals)
    {
        if (ptActorGlobals->arAreaDamageUpdateBuckets[nBucket] > 0)
        {
            ptActorGlobals->arAreaDamageUpdateBuckets[nBucket]--;
        }
    }
}





void ActorAccumulateAreaDamage(
    const ACDID idACDSource,
    const ACDID idACDTarget,
    const float flDamage)
{

    ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
    if (!ptACDTarget || !ptACDTarget->IsAlive())
    {
        return;
    }

    Actor * ptActor = ActorTryToGet((ActorID)ACDGetOwner(idACDTarget));
    if (ptActor == NULL)
    {
        return;
    }

    PowerGlobals * ptPowerGlobals = SGameManager::GameGet().ptPowerGlobals;
    if (ptPowerGlobals == NULL)
    {
        return;
    }
    MapAreaDamageAccumulators & tMapAreaDamageAccumulators = ptPowerGlobals->tMapAreaDamageAccumulators;

    AreaDamageAccumulatorKey tKey(idACDSource, idACDTarget);
    float flValue = 0.0f;

    BOOL bFound = tMapAreaDamageAccumulators.Lookup(tKey, flValue);
    if (!bFound)
    {
        tMapAreaDamageAccumulators.SetAt_Tracked(tKey, flDamage);
    }
    else
    {
        flValue += flDamage;
        tMapAreaDamageAccumulators.SetAt_Tracked(tKey, flValue);
    }


    if (ptActor->nAreaDamageBucketIndex != -1)
    {
        return;
    }
    // Original: attribute-bucket field (+0x220) and allocator (0x7bfa30), not the area-damage ones.
    ptActor->nAttributeUpdateBucketIndex = ActorGlobalsGetNewAttributeUpdateBucketIndex();
    if (ptActor->nAttributeUpdateBucketIndex == -1)
    {
        return;
    }

    uint32 dwNow = GameGetServerTime();
    uint32 dwLastAreaDamageUpdateTime = ptActor->dwLastAreaDamageUpdateTime;
    uint32 dwEarliestNextUpdate = dwLastAreaDamageUpdateTime + DEFAULT_TICKS_AREA_DAMAGE_BUCKETING_RATE;

    int32 nCurrentFrameBucketIndex = dwNow / DEFAULT_TICKS_AREA_DAMAGE_BUCKETING_RATE;
    uint32 dwStartingBucketTime = nCurrentFrameBucketIndex * DEFAULT_TICKS_AREA_DAMAGE_BUCKETING_RATE;

    uint32 dwNextUpdateTime = dwStartingBucketTime + ptActor->nAttributeUpdateBucketIndex;
    if (dwNextUpdateTime < dwEarliestNextUpdate ||
        dwNextUpdateTime <= dwNow)
    {
        dwNextUpdateTime += DEFAULT_TICKS_AREA_DAMAGE_BUCKETING_RATE;
    }

    uint32 dwTicksUntilNextUpdate = dwNextUpdateTime - dwNow;

    ServerQueueUpdateFunction(dwTicksUntilNextUpdate, ptActor->tAreaDamageUpdateData);
}




void sFindMonsterForPlayersLevelAreaPopulation(
    SNOArray & listActors,
    const LevelAreaSpawnPopulation & tSpawnPop,
    blz::vector<int, blz::allocator<int> > & arAllowedMonsterRarities)
{
    for (int32 j = 0; j < tSpawnPop.nSpawnGroupCount; j++)
    {
        LevelAreaSpawnGroup & tSpawnGroup = tSpawnPop.arSpawnGroups[j];


        if (tSpawnGroup.flSpawnChance <= 0.0f ||
            (tSpawnGroup.snoCondition != SNO_NONE && !ConditionEvaluate(tSpawnGroup.snoCondition, NULL, ACDID_INVALID, NULL)))
        {
            continue;
        }

        for (int32 k = 0; k < tSpawnGroup.nSpawnItemCount; k++)
        {
            LevelAreaSpawnItem & tSpawnItem = tSpawnGroup.arSpawnItems[k];

            BOOL bAllowedMonsterRarity = FALSE;
            for (size_t i = 0; i < arAllowedMonsterRarities.size(); i++)
            {
                if (arAllowedMonsterRarities[i] == tSpawnItem.eMonsterRarity)
                {
                    bAllowedMonsterRarity = TRUE;
                    break;
                }
            }

            if (!bAllowedMonsterRarity)
            {
                continue;
            }


            if (tSpawnItem.snonameToSpawn.snoGroup == ACTOR_SNO)
            {
                SNO snoActor = tSpawnItem.snonameToSpawn.snoHandle;
                if (CanSpawnActorByScrollOfCalling(snoActor) &&
                    listActors.Find(snoActor) == -1)
                {
                    listActors.Add_Tracked(snoActor);
                }
            }
            else if (tSpawnItem.snonameToSpawn.snoGroup == ENCOUNTER_SNO)
            {
                ScopedEncounterDefinitionLockR pdefEncounter(tSpawnItem.snonameToSpawn.snoHandle);
                int32 nCount = pdefEncounter->serSpawnOptions.dwSize / sizeof(EncounterSpawnOption);
                for (int32 i = 0; i < nCount; i++)
                {
                    if (pdefEncounter->arSpawnOptions[i].nWeight > 0)
                    {
                        EncounterSpawnOption & tSpawnOpt = pdefEncounter->arSpawnOptions[i];
                        SNO snoActor = tSpawnOpt.snoSpawn;
                        if (CanSpawnActorByScrollOfCalling(snoActor) &&
                            listActors.Find(snoActor) == -1)
                        {
                            listActors.Add_Tracked(snoActor);
                        }
                    }
                }
            }
        }

    }
}

void ActorFindMonsterForPlayersLevelArea(
    ActorCommonData & tACDPlayer,
    SNOArray & listActors,
    blz::vector<int, blz::allocator<int> > & arAllowedMonsterRarities)
{
    WorldPlace placeVictim = tACDPlayer.GetPlace();
    SWorld * ptSWorld = SWorldTryToGet(placeVictim.idSWorld);
    SNO snoLevelArea = ACDGetLevelArea(tACDPlayer);

    if (ptSWorld && SWorldIsDungeonFinder(ptSWorld->id))
    {
        LPCSTR szPopulation = SWorldGetDungeonFinderPopulation(WorldCommonGetWorldSNO(ptSWorld->id));

        SNO scopedSnoLevelArea = GlobalSNOGet(GLOBALSNO_LEVELAREA_DUNGEON_FINDER_POPULATIONS);
        ScopedLevelAreaDefinitionLockR pdefLevelArea(scopedSnoLevelArea);

        for (int32 i = 0; i < pdefLevelArea->ptServerData->nSpawnPopulationCount; i++)
        {
            LevelAreaSpawnPopulation & tSpawnPop = pdefLevelArea->ptServerData->arSpawnPopulations[i];
            if (!szPopulation || strcasecmp(tSpawnPop.szName, szPopulation) == 0)
            {
                sFindMonsterForPlayersLevelAreaPopulation(listActors, tSpawnPop, arAllowedMonsterRarities);
                break;
            }
        }
    }
    else if (snoLevelArea != SNO_NONE)
    {
        ScopedLevelAreaDefinitionLockR pdefLevelArea(snoLevelArea);
        for (int32 i = 0; i < pdefLevelArea->ptServerData->nSpawnPopulationCount; i++)
        {
            LevelAreaSpawnPopulation & tSpawnPop = pdefLevelArea->ptServerData->arSpawnPopulations[i];



            if (tSpawnPop.nWeight > 0)
            {


                sFindMonsterForPlayersLevelAreaPopulation(listActors, tSpawnPop, arAllowedMonsterRarities);
            }
        }
    }
}
void ActorGetItemsOnGround(blz::vector<const ActorCommonData *> & vecItemActors)
{

    SGame & tSGame = SGameManager::GameGet();
    ActorGlobals * ptActorGlobals = tSGame.ptActorGlobals;
    for (uint32 i = 0; i < ptActorGlobals->ptActors->dwMaximumCount; i++)
    {
        Actor * ptActor = ptActorGlobals->ptActors->ElementGetFromBaseIndex((uint16)i);
        if (ptActor->id == ACTORID_INVALID)
        {
            continue;
        }




        ACDID idACD = ptActor->idACD;
        const ActorCommonData * ptACD = ACDTryToGet(idACD);
        if (ptACD &&
            ptACD->IsItem() &&
            ptACD->IsInWorld())
        {




            vecItemActors.push_back(ptACD);
        }
    }
}



void ActorGetItemsInLimbo(blz::vector<const ActorCommonData *> & vecItemActors)
{

    SGame & tSGame = SGameManager::GameGet();
    ActorGlobals * ptActorGlobals = tSGame.ptActorGlobals;
    for (uint32 i = 0; i < ptActorGlobals->ptActors->dwMaximumCount; i++)
    {
        Actor * ptActor = ptActorGlobals->ptActors->ElementGetFromBaseIndex((uint16)i);
        if (ptActor->id == ACTORID_INVALID)
        {
            continue;
        }




        ACDID idACD = ptActor->idACD;
        const ActorCommonData * ptACD = ACDTryToGet(idACD);
        if (ptACD &&
            ptACD->IsItem() &&
            ptACD->tLocation.eActorLocation == AL_NONE)
        {

            vecItemActors.push_back(ptACD);
        }
    }
}
