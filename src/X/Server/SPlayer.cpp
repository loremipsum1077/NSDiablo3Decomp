// Original file: X/Server/SPlayer.cpp
// Partly written: functions sit on their original lines (see the reference .cpp).

#include "SPlayer.h"
#include "SGame.h"
#include "SGameManager.h"
#include "PlayerList.h"
#include "../Common/Player.h"
#include "../Common/Game.h"
#include "../Common/ActorCommonData.h"
#include "../Common/Networking/GameMessage.h"
#include "../Common/Networking/Network.h"
#include "../Common/OnlineService/OnlineService.h"
#include "OnlineService/OnlineServiceServer.h"
#include "../Sigma/DataStructures/XGrowableList.h"
#include "../Sigma/DataStructures/XRingBuffer.h"
#include "../Sigma/DataStructures/DataArraysTemplated.h"
#include "../../Contrib/Contrib/blz/include/blz/vector.h"
#include "../../Contrib/Contrib/blz/include/blz/unordered_map.h"
#include "../Common/Powers/PowerAPISwig.h"
#include "../Common/Attributes/FastAttribConstantsSwig.h"
#include "../Common/GameCommonData.h"
#include "../Common/GlobalSNO.h"
#include "../Common/GameConstants.h"
#include "../Common/Items.h"
#include "../Common/VisualInventory.h"
#include "../Common/Definitions/FlagSetDefinition.h"
#include "../Sigma/Math/MathGeneral.h"
#include "../Sigma/DataStructures/DataStructuresExternal.h"
#include "../Common/AI/ActorAIStatesSwig.h"
#include "AI/AIStates.h"
#include "Powers/PowerAPI.h"
#include "Map/Minimap.h"
#include "SGameStats.h"
#include "SGameStatsSwig.h"
#include "Actors/Actors.h"
#include "../Common/Quests/QuestsSwig.h"
#include "Actors/ActorPowers.h"
#include "Powers/Powers.h"
#include "../Common/Crafting/ItemCrafting.h"
#include "../Common/Nav/NavigationMesh.h"
#include "../Common/Protobuf/Hero.pb.h"
#include "../Common/Protobuf/Account.pb.h"
#include "../Common/GameTemplate.h"
#include "../Common/GameSwig.h"
#include "System/LevelArea.h"
#include "LuaScript/ScriptEvent.h"
#include "../Common/Definitions/LevelAreaDefinition.h"
#include "../Sigma/Memory/SigmaMemory.h"
#include "../Common/OnlineService/Achievements.h"
#include "../Common/Achievements/Console/ConsoleAchievements.h"
#include "../Common/Achievements/AchievementDefines.h"
#include "../Common/Definitions/LoreDefinition.h"
#include "../Sigma/DataStructures/XOptional.h"
#include "SWorld.h"
#include "Timers.h"
#include "../Common/ActorInventory.h"
#include "../Common/ItemsSwig.h"
#include "../Sigma/DataStructures/XVar.h"
#include "../Sigma/StringManip.h"
#include "../Common/Shell/CmdLine.h"
#include "SGameConstants.h"
#include "../Common/Definitions/QuestDefinition.h"
#include "../Common/Definitions/GlobalDefinition.h"
#include "../Common/Protobuf/GameMessage.pb.h"
#include "../Common/Protobuf/GameBalance.pb.h"
#include "../Common/Definitions/GameBalanceDefinition.h"
#include "../Common/Nav/NavigationMesh.h"
#include "../Common/Nav/NavZone.h"
#include "../Common/ActorCommonDataSwig.h"
#include "../Common/Definitions/ConversationDefinition.h"
#include "../Common/Definitions/ActDefinition.h"
#include "SScene.h"
#include "../Common/Definitions/SkillKitDefinition.h"
#include "../Common/Powers/PowersCommon.h"
#include "../../BlizzardCore/Include/Blizzard/Util/Hash.h"
#include "../Common/TagMaps/PowerTags.h"
#include "../Common/Actors/ActorEffectCommon.h"
#include "../Common/Movement/ActorMovement.h"
#include "OnlineService/SStore.h"
// Declared here from their definitions elsewhere (headers not adopted yet).
namespace Minimap { void UpdateWaypointMarkersFromPlayer(const Player & tPlayer); }  // X/Server/Map/Minimap.cpp:1488
#include "../../NintendoSDK/Include/nn/ldn/ldn_Types.h"  // header statics run by this file's static initializer (ldn_Ipv4Address.h:153-163, ldn_MacAddress.h:101-106)
void SQuestsOnPlayerFlagSet(ACDID idACDPlayer, int32 nPlayerFlagIndex, BOOL bValue);  // X/Server/Quests/SQuests.cpp:4757
void SQuestsPlayerJoinAct(Player & tPlayer);  // X/Server/Quests/SQuests.cpp:144
BOOL SWorldIsSetDungeon(SWorldID idSWorld);  // X/Server/SWorld.cpp:3338
void ServerQueueClientUpdate();  // X/Server/Network/Server.cpp:438
void SendCurrentActMessage(const Player & tPlayer, const BOOL bOnJoin);  // X/Server/Network/ServerMessageSend.cpp:1357
#include "../Common/AnimPlayer.h"
BOOL PlayerEligibleForDailyFirst(const Player & tPlayer, FastAttribKey tKey);  // X/Common/Player.cpp:5387
void BrainAPIReleaseAllTargets(ActorID idActor);  // X/Server/AI/BrainComponents.cpp:9705
BOOL NavMeshSearchForLocation(const WorldPlace & placeStart, const PassabilitySettings & tPassability,
                              const float flMinSearchRadius, const float flMaxSearchRadius,
                              const uint32 dwFindLocationFlags, NavMeshHandle & tNavMeshHandleLanding,
                              WorldPlace & placeLanding, BOOL bSearchOverImpassable, Team eTeam,
                              float flRadiusOverride);  // X/Common/Nav/NavigationMesh.cpp:3421
#include "../Common/TagMaps/ActorTags.h"
uint32 PlayerFlagFromActEnum(const Act eAct);  // X/Common/GameCommonData.cpp:450
void SendInterstitialMessage(const Player & tPlayer);  // X/Server/Network/ServerMessageSend.cpp:1372
#include "Movement/SActorMovement.h"
PlayerAvengerData * GetPlayerAvengerData(Player * ptPlayer, AvengerSource eAvengerSource);  // X/Common/Player.cpp:1634
namespace Avenger { namespace Console { void PlayExitEffect(ActorCommonData & tACDAvenger); } }  // X/Common/Console/Avenger.cpp:776
#include "../Common/TagMaps/Tags.h"
void PlayerBrainSpendTraitPoint(ActorCommonData & tACDPlayer, const SNO snoPower);  // X/Server/AI/PlayerBrain.cpp:586
uint32 NavMeshGetDynamicFlagsForPassability(PassabilityType ePassability);  // X/Common/Nav/NavigationMesh.cpp:3187
struct StashIconStateMessage {  // size 0x18; X/Common/Networking/GameMessage.h
    RequiredMessageHeader tHeader;  // 0x00
    uint8 bStashIcons[13];  // 0x08
};
#include "System/BossEncounter.h"
#include "System/SNephalemRift.h"
#include "System/SetEncounter.h"
#include "System/SVoteKick.h"
#include "Network/ClientValidation.h"
#include "../Common/Console/Avenger.h"
void TraceStat(LPCSTR szStat, ACDID idACD, LPCSTR szFormat, ...);  // X/Server/SGame.cpp:7255
void SGameAchievementRegister(const OnlineService::GameAccountHandle & tHandle); void SGameAchievementUnregister(const OnlineService::GameAccountHandle & tHandle);  // X/Server/SGame.cpp:1628, 1650
void ActorTriggerAttributeUpdate(ActorID idActor);  // X/Server/Actors/Actors.cpp:3770
float SPlayerGetPing(Player & tPlayer); void SPlayerUpdateKillCounters(Player & tPlayer);  // lines 11296, 5138

static void sPollPerkBuffs(Player & tPlayer, ActorCommonData & tACDPlayer); static void sPollLimitedDurationItems(Player & tPlayer, ActorCommonData & tACDPlayer);  // lines 11644, 11699
#include "../Common/TeamCommon.h"
#include "Quests/SQuests.h"
#include "TownPortal.h"
#include "PVP.h"
#include "STalk.h"
#include "../Common/Hireling.h"
#include "SActTransitions.h"
#include "ConversationMgr.h"
BOOL DeleteLastHeadstone(ActorCommonData & tACDVictim); void ActorsAdjustForPlayerChanged(const int32 nOldPlayerCount, const int32 nNewPlayerCount);  // X/Server/AI/BrainComponents.cpp:2878; X/Server/Actors/Actors.cpp:6724
void SItemsDestroyLockedOwned(ActorCommonData & tACDPlayer); void SHirelingRehireAfterPlayerLeft(); int32 PlayerGetNumPlayersForDifficulty();  // X/Server/SItems.cpp:157; X/Server/SHireling.cpp:1080; X/Common/Player.cpp:2159



enum { FLOATING_NUMBER_TYPE_XP = 33 };
BOOL ExperienceGrantDirectly(ActorCommonData & tACDPlayer, const uint64 nExpGranted, const BOOL bNotifyClient, const BOOL bGrantToAllPlayersIfLocal);  // Experience.cpp

void ACDInventoryItemRestoreSocketsFromGenerator(ACDID idACDOwner, ActorCommonData & tACDItem, const D3::Items::Generator & tGenerator, XPooledDataIDList * plistACDs, int32 nNetworkNameBase);  // X/Common/ActorInventory.cpp:6613
namespace Callouts {
BOOL IsSharedCounterType(const KillCounterType eType);  // X/Common/Callouts.cpp:109
uint32 GetMassacreMinCount();  // X/Common/Callouts.cpp:120
void ApplyDestructionBuff(Player & tPlayer, uint32 nCount); float GetMassacreXPMultiplier(const uint32 nKills); int32 ApplyEnvironmentKillBuff(Player & tPlayer, uint32 nBonus);  // X/Common/Callouts.cpp:200, 77, 135
}
void SPlayerUpdateKillCounter(Player & tPlayer, KillCounterType eType); void SPlayerUpdateCurrencyCounter(Player & tPlayer, CurrencyType eCurrencyType); static void sUpdateCurseCounts(); void PowerAPI_RecomputeHitpointHealTarget(ActorCommonData * ptACD); BOOL ACDWithinDistance(const ActorCommonData & tACD1, const ActorCommonData & tACD2, const float flDistance); void STalkForceDoneTalking(const ACDID idACDPlayer); float PlayerGetStreakTimerTolerance(const Player & tPlayer); CalloutEventType CalloutEventFromKillCounter(const KillCounterType eKillCounterType); BuffID PowerAPI_BuffFindOnACD(SNO snoPower, const ActorCommonData * ptACD, ACDID idACDSource); BOOL PowerGetBuffAttributes(int32 nBuffIndex, Attrib & eAttribStart, Attrib & eAttribEnd, Attrib & eAttribCount); BOOL ExperiencePlayersCloseEnough(const ActorCommonData & tACDVictim, ACDID idACDOtherPlayer); uint64 ActorCalculateXPForMonster(const ActorCommonData & tACDVictim, int32 nPlayerLevel); ACDID BrainAPIGetActorAttackTarget(ACDID idACDAttacker); int32 BrainAPIGetNumEnemyTargets(ActorID idActor);  BOOL BrainAPIHasEnemyTargetForInCombatStatus(ActorID idActor); int32 BrainAPIGetNumEnemyPlayerTargets(ActorID idActor); void BrainAPIUpdateTargets(ActorID idActor, BOOL bForceUpdate);    // line 8946, 9207, 9315; X/Server/Powers/PowerAPI.cpp:13170; X/Common/ActorCommonData.cpp:2334; X/Server/STalk.cpp:132; X/Common/Player.cpp:5337, 618; X/Server/Powers/PowerAPI.cpp:10166; X/Common/Powers/PowersCommon.cpp:5977; X/Common/ExperienceCommon.cpp:78; X/Server/Actors/Actors.cpp:2322; X/Server/AI/BrainComponents.cpp:10443, 7974; X/Common/Networking/GameMessage.h (size 0x10)
BOOL SPlayerIsInSetDungeon(const ActorCommonData & tACDPlayer);  // line 14244
void SPlayerEvaluateMassacreForShrineGlobeDrop(uint32 uCurrentScore, Player * ptPlayer, ActorCommonData & tACDPlayer, KillCounter & tCounter);  // line 9415
void SPlayerRefreshWaypointDestination(ACDID idACDPlayer);  // line 13243
void SPlayerSetLastWaypointLevelArea(Player * ptPlayer, const SNO snoLevelArea);  // line 12427
void ActorRefreshActivatedWaypoints();  // X/Server/Actors/Actors.cpp:7047
SNO ActGetLevelAreaByWaypointIndex(int32 nIndex);  // X/Common/Act.cpp:198
namespace NephalemRift { BOOL IsTieredRiftActive(); }  // X/Common/NephalemRiftCommon.cpp
namespace NephalemRiftManager { BOOL PortToPlayerInStandardRiftPossible(Player * ptTargetPlayer, Player * ptPlayer); }  // X/Server/System/SNephalemRift.cpp
namespace SetEncounterManager { BOOL CanEnterSetDungeon(Player * ptPlayer); }  // X/Server/System/SetEncounter.cpp
BOOL SWorldIsDungeonFinder(SWorldID idSWorld);  // X/Server/SWorld.cpp:3304
BOOL ACDInSameWorld(const ActorCommonData & tACD1, const ActorCommonData & tACD2);  // X/Common/ActorCommonData.cpp:2503









#include "../Common/Callouts.h"
BOOL ACDWarpToTarget(ActorCommonData & tACDToWarp, const Target & tWarpTarget, const PlayerWarpReason eReason, float flMinWarpRadius, BOOL bFailOnBadLocation);  // X/Server/AI/ActorAI.cpp
BOOL SPlayerFindValidWalkableSpotNearPlayer(const ActorCommonData & tACDPlayer, WorldPlace & placeSpot);  // line 12152
void ACDInventoryWhereCanThisGo(const ActorCommonData & tACDPlayer, const GBID gbidItem, int32 & nNumSlots, InventorySlotType * eValidSlots);  // X/Common/ActorInventory.cpp:11604
BOOL PlayerBrainTryInventoryMoveItem(ActorCommonData & tACDPlayer, ActorCommonData & tACDItem, const InventoryLocation & tInvLocDest);  // X/Server/AI/PlayerBrain.cpp:600
float PowerGetClientRunSpeed(const Power & tPower);  // X/Server/Powers/Powers.cpp:5945
struct LegacyItemConversion {  // size 0x28; X/Common/Definitions/GameBalanceDefinition.h:1333 (also local in Items.cpp)
    GBHeader tHeader;  // 0x00
    GBID gbidOldItem;  // 0x18
    GBID gbidNewItem;  // 0x1c
    BOOL bConsoleIgnore;  // 0x20
    uint32 dwPad;  // 0x24
};
enum { GB_LEGACY_ITEM_CONVERSIONS = 45 };
typedef ScopedGBLockR<LegacyItemConversion, 45> ScopedLegacyItemConversionLockR;
uint64 ItemGenerateHash64(const ActorCommonData & tACDItem, BOOL bIncludeSockets);  // X/Common/Items.cpp:11239
SNO PlayerGetPowerInTraitSlot(const Player & tPlayer, const int32 nSlot);  // X/Common/Player.cpp:3740
void SGameStatsPlayerTimedEventStart(Player * ptPlayer, StatCounterType eType, int32 nParam0, int32 nParam1, BOOL bReportIfUnfinished);  // X/Server/SGameStats.cpp:1571
void SGameStatsPlayerTimedEventFinish(Player * ptPlayer, StatCounterType eType, int32 nParam0, int32 nParam1);  // X/Server/SGameStats.cpp:1599
int32 ActGetWaypointByLevelArea(const SNO snoLevelArea, BOOL bCheckActivationState);  // X/Common/Act.cpp:156
void SQuestsOnPlayerEnterLevelArea(ACDID idACDPlayer, SNO snoLevelArea);  // X/Server/SQuests.cpp:4743
SNO QuestGetActiveBountyForLevelArea(SNO snoLevelArea, BOOL bCheckAdditionalLevelAreas);  // line 2200
QuestType QuestGetQuestType(SNO snoQuest);  // line 1961
void HelperProcessPlayerLevelAreaChange(ActorCommonData & tACDPlayer, const LevelAreaID idLevelAreaNew);  // line 941
PlayerClass PlayerGetPlayerClass(const Player * ptPlayer);  // X/Common/Player.cpp
namespace Callouts {






MassacreDef GetMassacreDef(uint32 uCount);  // X/Common/Callouts.cpp
}
void LootDropShrineGlobe(const ACDID idACDLootee, uint32 dwGameEventNumber, const ACDID idACDKiller, GBID gbidItem);  // X/Server/Loot/Loot.cpp:1269
BOOL PowerIsValidForSlot(AutoPowerDefinitionR pdefPower, int32 nSlot);  // X/Common/Powers/PowersCommon.cpp
extern const uint32 CREATOR_APP_TICKS_TO_DISCONNECT_GAME = 14400;  // line 261
const int32 PING_INTERVAL = 180;  // line 265
const uint32 PING_BUCKET_MAX_ENTRIES = 8;  // line 266
typedef int32 TrickleInfoType; TrickleInfoType PlayerGetTrickleInfoType(const ActorCommonData & tACDPlayer, const ActorCommonData & tACD);  // X/Common/Player.cpp:3496
BOOL ACDIsFollower(const ActorCommonData & tACD);  // X/Common/ActorCommonData.cpp:8206
BOOL ACDIsHirelingProxy(const ActorCommonData & tACD);  // X/Common/ActorCommonData.cpp:8221
BOOL ACDIsFollowerProxy(const ActorCommonData & tACD);  // X/Common/ActorCommonData.cpp:8236
void NPCHandleDismissal(ActorCommonData & tACDHireling, BOOL bPlayerDismissed, BOOL bDisplayChatMessage);  // X/Server/SNPC.cpp:747
void SendPlayerWarping(const ACDID idACDPlayer); BOOL TradeIsTrading(const ACDID idACDPlayer); void STradeCancel(ACDID idACDPlayer); void ACDWarpLastHeadStone(const ActorCommonData & tACD); namespace Minimap { void OnPlayerWarped(const ActorCommonData & tACDPlayer, PlayerWarpReason eWarpReason, const ActorCommonData * ptACDPortal); } void SComplexEffectRemoveAll(const ACDID idACD); void ACDWarpAllPetsToPlayer(ActorCommonData & tACDPlayer, const PlayerWarpReason eReason); void ACDsIntersectSphere(const WorldPlace & tPlace, const WorldDistance wdRadius, const ACDID idACDIgnore, XPooledDataIDList & listACDs, uint32 dwSphereTreeFlags); AIBehaviorType AIStateGetBehaviorType(ActorID idActor); BOOL EscortCanBePulledThroughWarp(const ActorCommonData & tACDEscort, const ActorCommonData & tACDPlayer); BOOL ACDWarpToACD(ActorCommonData & tACDToWarp, const ActorCommonData & tACDTarget, const PlayerWarpReason eReason, float flMinWarpRadius); void BrainAPIDoIdle(ActorID idActor); DataIDMemoryPool * MemoryPoolsGetDATAIDListPool(); void SSceneForceDeactivationCheckThisTick(); void ActorPowersRemoveBuffsForDueling(ActorCommonData & tACD, const SNO snoPower); int32 WorldGetDungeonFinderLevel(const SNO snoWorld); struct QuestInstance; BOOL QuestIsComplete(const QuestInstance & tQuest); QuestInstance * QuestsGetQuestInstance(SNO snoQuest, ACDID idACDPlayer); void KnownListForceUpdateForAllPlayers(); void PlayerSetDefaults(Player & tPlayer); void ServerMessageSendWorldRevealToPlayer(const PlayerList & listDestPlayers, const SWorldID idSWorld); void ServerMessageSendMapRevealSceneToPlayer(const PlayerList & listDestPlayers, const SSceneID idSScene, BOOL bForceReveal); void PlayerLoadoutItemUpdate(LoadoutItemData & tLoadoutItem, const ActorCommonData & tACDItem); struct PlayerLoadoutEquipmentUpdateMessage { RequiredMessageHeader tHeader; int32 nLoadoutIndex; int32 nEquipmentIndex; LoadoutItemData tNewItem; }; BOOL WorldIsDungeonFinderWorld(const SNO snoWorld); BOOL SWorldDungeonFinderIsInitialized(); void SScenePrefetchDestination(Player * ptPlayer, WorldPlace tPlace); ACDID PlayerGetFirstPet(ACDID idACDPlayer, PetType eType, int32 & nPos); ACDID PlayerGetNextPet(ACDID idACDPlayer, PetType eType, int32 & nPos); Team STeamGetTeamForPlayer(Player & tPlayer); void SPlayerSetCheckpoint(Player & tPlayer, const CheckpointData & tCheckpoint);  // X/Server/Network/ServerMessageSend.cpp:1433, 694, 883; X/Common/Trade.cpp:270; X/Server/STrade.cpp:756; X/Server/AI/ActorAI.cpp:1817, 1933, 1802; X/Server/Map/Minimap.cpp:1511; X/Server/Actors/SComplexEffect.cpp:338; X/Common/Actors/ActorCollision.cpp:317; X/Server/AI/AIStates.cpp:6072; X/Common/Player.cpp:3256; X/Server/AI/BrainComponents.cpp:3448; X/Sigma/MemoryPools.cpp:101; X/Server/SScene.cpp:1457; X/Server/Actors/ActorPowers.cpp:723; X/Common/World.cpp:828, 886; X/Server/SWorld.cpp:2741; X/Server/SScene.cpp:1558; X/Common/Quests/Quests.cpp:995, 872; X/Server/Network/KnownList.cpp:2941; X/Common/Player.cpp:782, 4460, 2589, 2600; X/Common/Networking/GameMessage.h (message, size 0x30; also local in Player.cpp); X/Common/TeamCommon.cpp:479; line 10893
void BrainAPIDoDeath(ActorID idDeadActor, ActorID idKillerActor, const BOOL fNoExp, const BOOL fNoLoot, const BOOL fNoHealthDrop, PowerAPIDamage * ptDamage, float * pflDamageDone);  // X/Server/AI/BrainComponents.h
#include "../Common/GizmoSwig.h"
BOOL SItemCrafting_TryUnlockTransmog(GBID gbidTransmog, Player & tPlayer, BOOL bMustBeInInventory, BOOL bSendFullUpdate);  // X/Server/Crafting/SItemCrafting.cpp:296
BOOL SACDInventoryRewardStashTab(const ACDID idACDPlayerActor);  // X/Common/ActorInventory.cpp:10513
int32 GetSeasonalRewardStashTabsCountFromAttributeValue(int32 nAttribute);  // X/Common/ActorInventory.cpp:13424
void sPlayerGiveRewardCosmetic(Player & player, OnlineService::RewardId rewardId, OnlineService::AchievementId achievementId);  // SPlayer.cpp:14786

SNO ItemGetLegendaryPower(GBID gbidItem);  // X/Common/Items.cpp:10556
float ItemGetLegendaryPowerValue(GBID gbidItem);  // X/Common/Items.cpp:10606

Player * PlayerGetByGameAccount(const OnlineService::GameAccountHandle & tGameAccount);  // X/Common/Player.cpp:1571
BOOL SCosmeticItems_LearnCosmeticItem(const ACDID idACDPlayer, const GBID gbidItem, CosmeticItemType eType, bool bPurchased);  // X/Server/SCosmeticItems.h
BOOL SCosmeticItems_LearnPet(const ACDID idACDPlayer, const GBID gbidItem, bool bPurchased);  // X/Server/SCosmeticItems.h

void SPlayerUpdateSidekickStatsBoost(ActorCommonData & tACDPlayer);  // SPlayer.cpp:13762
ACDID ACDGetPlayerACDOnSameTeam(const ACDID idACD); int32 ACDGetTieredLootRunLevel(const ActorCommonData & tACD); BOOL ACDIsPet(const ACDID idACD);  // X/Common/Player.cpp:1415, 1666
void ExperienceUpdatePerLevelHitpointFactorVitality(ActorCommonData & tACD);  // X/Server/Experience.cpp:1293
void ACDInventoryUpdateOnUpscalingChanged(ActorCommonData & tACDOwner);  // X/Common/ActorInventory.cpp:13406
void ACDPlayEffect(const ACDID idACD, const EffectType eEffectType, const int32 nEffectParameter, const ACDID idACDOwner);  // X/Server/Actors/ActorEffect.cpp:104

BOOL ACDIsLootRunBoss(const ActorCommonData & tACD); BOOL NPCShouldBeHiddenForClient(const ActorCommonData & tACD, const Player * ptPlayer);  // X/Common/ActorInventory.cpp:9331; line 1308 of the file ahead of SPlayer.cpp
int32 PlayerGetParagonPointsTotalForCategory(const ActorCommonData & tACDPlayer, int32 nCategory);  // X/Common/Player.cpp:5280
void SPlayerInitializeParagonPoints(Player & tPlayer);  // SPlayer.cpp:6726
namespace Minimap { void ResolveIconForActor(const ActorCommonData & tACD, const Actor * ptActor, const Player * ptPlayer, SNO & snoRet, CRefString & szRet); } 
#include "Network/KnownList.h"











void KnownListItemChanged(KnownEntryType eType, DataID idObject, BOOL bCheckForDuplicate);  // X/Server/Network/KnownList.cpp:1180
inline void SPlayerQueueFloatingNumberDOTDamageTaken(ActorCommonData & tACDPlayer, float flDamage);  // SPlayer.cpp:4670
void SPlayerQueueFloatingNumberDOTDamageDone(Player * ptPlayerAttacker, ActorCommonData & tACDTarget, float flDamage);  // SPlayer.cpp:4650
void SPlayerOnChangeLevelArea(ACDID idACDPlayer, LevelAreaID idLevelAreaOld, LevelAreaID idLevelAreaNew);  // SPlayer.cpp:7960
void STimedEventOnPlayerEnterWorld(Player & tPlayer);  // X/Server/STimedEvents.cpp:195
void SQuestsOnPlayerEnterWorld(ACDID idACDPlayer, SNO snoWorld);  // X/Server/Quests/SQuests.cpp:4729
void GizmoSetHearthPortal(Player & tPlayer, const WorldPlace & placeReturn, const BOOL bSendToClient);  // X/Server/SGizmo.cpp:3484
// X/Common/Definitions/ConversationDefinition.h. Only the members read here are declared.
enum { CONV_TYPE_TALK_MENU = 10 };
enum { CF_NEVER_REPEAT_BIT = 14 };
struct ConversationDefinition {  // size 0x110
    RequiredDefinitionHeader tHeader;  // 0x00
    int32 eConvType;  // 0x0c (ConversationType)
    uint8 _unadopted0[0x24 - 0x10];  // 0x10 (invented name: eConvIcon .. snoConvUnlocks)
    uint32 dwFlags;  // 0x24
    uint8 _unadopted1[0x110 - 0x28];  // 0x28 (invented name: szSetPlayerFlag .. )
};
template <> inline const ConversationDefinition * DefinitionLockR<ConversationDefinition>(SNO sno) {  // line 462
    return (const ConversationDefinition *)gptGroupDefConversation->LockR(sno, FALSE);
}
template <> inline void DefinitionUnlockR<ConversationDefinition>(const ConversationDefinition * pdef) {  // line 463
    void * lpDef = (void *)pdef;
    gptGroupDefConversation->UnlockR(lpDef);
}
typedef ScopedDefinitionLockR<ConversationDefinition> ScopedConversationDefinitionLockR;

BOOL QuestStepIsReachableByStep(const QuestDefinition * pdefQuest, int32 nStepUID, int32 nFromStepUID);  // X/Common/Definitions/QuestDefinition.cpp

struct CachedWaypoint {  // size 0x2c (X/Common/Act.h:13)
    Act eSourceAct;  // 0x00
    WaypointInfo tInfo;  // 0x04
};
typedef blz::vector<CachedWaypoint, blz::allocator<CachedWaypoint> > CachedWaypointArray;  // X/Common/Act.h:19
const CachedWaypointArray & ActGetCachedWaypointArray();  // X/Common/Act.cpp:146
SSceneID LevelAreaGetWaypointSSceneID(LevelAreaID idLevelArea);  // X/Server/System/LevelArea.cpp
LevelAreaID LevelAreaFind(const SNO snoLevelArea, const SWorldID idSWorld);  // X/Server/System/LevelArea.cpp
void PlayerAssignToLegendaryPowerSlot(Player & tPlayer, const GBID gbidLegendaryItem, const int32 nSlot);  // X/Common/Player.cpp:3750
ACDID SItemGenerate(const OnlineService::ItemId & tItemId, const D3::Items::Generator & tGenerator, const WorldPlace & tPlace, const float flScale, uint32 tName);  // X/Server/SItems.cpp:337
void ItemSetBoundToACD(ActorCommonData & tACD, const ACDID idACDOwner);  // X/Common/Items.cpp:5565
void PlayerAssignToActiveSkillSlot(Player & tPlayer, const SNO snoPower, const RuneType eRune, const int32 nSlot);  // X/Common/Player.cpp:3698
extern const int32 cgnActiveSkillSlotLevelReq[6];  // X/Common/Player.cpp:345
void ActorPowersRemoveBuffsForRespec(ActorCommonData & tACD, const SNO snoPower);  // X/Server/Actors/ActorPowers.cpp
SNO ActorPowersGetPrimaryPower(ActorID idActor);  // X/Server/Actors/ActorPowers.cpp:292
void ActorPowersInterruptPrimary(ActorID idActor, BOOL bIgnoreContactFrame); void BrainAPIDoPlayAnimation(const ActorID idActor, const uint32 dwAnimTag, const uint32 dwDuration, const BOOL bAllowPlayingAnimInterrupt, const BOOL bInterruptableAnim, float flUnitStartTime, const BOOL bDontSendToClient, const float flSpeedScalar);  // X/Server/Actors/ActorPowers.cpp:392; X/Server/AI/BrainComponents.cpp:940
uint32 StringHashI(LPCSTR pszName);  // X/Sigma/TypeDescriptor/SymbolicValue.cpp:63
BOOL WorldCollSearchForLocation(const WorldPlace & placeStart, uint16 uOneRequiredStaticFlags, uint32 uObjectNoOverlapFlags, float flLocationRadius, float flSearchRadius, float flSearchRadiusMax, ACDID idACDIgnore, Vector3D & wpEnd, NavCellHandle & hNavCellEnd, Team eTeam, uint16 uObjectNoPassthroughFlags, ACDID idOwner);  // X/Common/WorldCommonCollision.cpp:241
#include "../Common/Quests/Quests.h"






QuestInstance * GetLeadingMainLineQuest();  // X/Common/Quests/Quests.cpp:1020
BOOL QuestStepIsSelectable(SNO snoQuest, int32 nStepUID);  // X/Common/Quests/Quests.cpp:1167
BOOL QuestStepCanReachReward(SNO snoQuest, int32 nStepUID);  // X/Common/Quests/Quests.cpp:1727
BOOL QuestStepIsEligibleForMatchmaking(SNO snoQuest, int32 nStepUID);  // X/Common/Quests/Quests.cpp:2707

#include "Achievements/AchievementModifierEvaluationLogic.h"
namespace Achievement {
typedef int32 EventID;











}  // namespace Achievement
void PlayerBrainSpendActiveSkillPoint(ActorCommonData & tACDPlayer, const SNO snoPower);  // X/Server/AI/PlayerBrain.cpp:572
BOOL PlayerBrainChangeHotbarButtons(Player & tPlayer, const SkillButton eSkillButton, const HotbarButtonData & tButtonData);  // X/Server/AI/PlayerBrain.cpp:756
void PlayerAssignToTraitSlot(Player & tPlayer, const SNO snoPower, const int32 nSlot);  // X/Common/Player.cpp:3729
BOOL PlayerAlreadyPurchasedActiveSkill(const Player & tPlayer, const SNO snoPower, int32 & nSlotOfExistingSkill);  // X/Common/Player.cpp:3772
void PowerHelper_AddCooldown(ActorCommonData & tACD, SNO snoPower, uint32 uCooldownTicks, BOOL bReapplyPassivesWhenFinished);  // X/Server/Powers/PowerHelper.cpp:1498
void PlayerSetHotbarBtnAssignment(Player * ptPlayer, const SkillButton eButton, const HotbarButtonData & tButtonData); const HotbarButtonData & PlayerGetHotbarBtnAssignment(const Player * ptPlayer, const SkillButton eButton);  // X/Common/Player.cpp:1778, 1846

BOOL SWorldPlayerWarpToWaypoint(const ACDID idACDPlayer, const ACDID idACDWaypoint, const int32 nWaypointNumber);  // X/Server/SWorld.cpp:816













BOOL PlayerLoadoutMatchesCurrentEquipment(const Player & tPlayer, int32 nLoadoutIndex);  // X/Common/Player.cpp:4177
void PlayerLoadoutGetDesiredSocketables(GBIDArray & arGBIDSocketables, const Player & tPlayer, int32 nLoadoutIndex);  // X/Common/Player.cpp:4515
void PlayerLoadoutFindSocketables(XPooledDataIDList & listACDIDSocketables, const ActorCommonData & tACDPlayer, const GBIDArray * arGBIDSocketables);  // X/Common/Player.cpp:4750
BOOL PlayerLoadoutSocketablesMatch(GBID gbidItem1, GBID gbidItem2);  // X/Common/Player.cpp:4832
BOOL PlayerLoadoutItemIsInValidInventorySlot(const ActorCommonData & tACDItem, ACDID idACDPlayer); void PlayerLoadoutUnsocketItemToLimbo(const ActorCommonData & tACDItem, XPooledDataIDList * plistACDIDSocketables);  // X/Common/Player.cpp:4393, 4808
void PlayerSetPotionBtnAssignment(Player * ptPlayer, const HotbarButtonData & tButtonData); extern const InventorySlotType cgarPlayerLoadoutSocketableSlotsOrder[9];  // X/Common/Player.cpp:1761, 173
EInvItemClass GetInventoryItemClass(const GBID gbidItem); BOOL ACDInventoryCanHold(const ActorCommonData & tACD, const ActorCommonData & tACDItem, const InventorySlotType eSlot, IVector2D & tWhereCanHold, const int32 * pnSlotYMinMax);  // X/Common/ActorInventory.cpp:927, 869
BOOL ActorInventorySocketItem(ActorCommonData & tACDItemToSocket, ActorCommonData & tACDItemSocketedInto, const IVector2D & tPoint, BOOL bMarkItemAsDirty); void ACDInventoryLocationSet(InventoryLocation & tInvLoc, const ACDID idACDContainer, const InventorySlotType eSlot, const int32 nSlotX, const int32 nSlotY);  // X/Common/ActorInventory.cpp:7825, 9743
BOOL ACDInventoryAddToLocation(ActorCommonData & tACDContainer, ActorCommonData & tACDItem, const ActorLocation & tNewLocation, BOOL fSkipRequirements, BOOL bMarkAsDirty, BOOL bOnHeroLoad, BOOL bRemoveExistingItems); void ACDInventoryRemoveToLimbo(const ACDID idACDContainer, ActorCommonData & tACDItem, BOOL bRemoveFromKnownList);  // X/Common/ActorInventory.cpp:4504, 3228
BOOL ItemUsesQuiver(const ActorCommonData & tACDItem); BOOL ItemRemoveSocketedItemAttributes(ActorCommonData & tACDItemToSocket, const ActorCommonData & tACDItemSocketedInto);  // X/Common/Items.cpp:3467, 4681
void ActorGetItemsInLimbo(blz::vector<const ActorCommonData *, blz::allocator<const ActorCommonData *> > & vecItemActors); void ActorGetItemsOnGround(blz::vector<const ActorCommonData *, blz::allocator<const ActorCommonData *> > & vecItemActors);  // X/Server/Actors/Actors.cpp:7537, 7508
BOOL SACDInventoryPickupOrSpillOntoGround(ActorCommonData & tACDPlayer, ActorCommonData & tACDItem, const BOOL bTryStashFirst, ACDID * pidACDStackDest, const BOOL bAllowAutoEquip);  // X/Common/ActorInventory.cpp:5218

namespace GameTesting { int32 GetTestType(); }  // X/Common/GameTesting.cpp:625 (returns TestType)
void SScenePrefetchPortalDestination(SWorldID idSWorld, Player * ptPlayer, uint32 uEntranceGUID);  // X/Server/SScene.cpp:1568
void SScenePrefetchWaypointDestination(SWorldID idSWorld, Player * ptPlayer, SNO snoLevelArea);  // X/Server/SScene.cpp:1643
SWorldID WorldCommonFindBySNO(SNO snoWorld, BOOL bIgnorePendingDelete);  // X/Common/World.cpp:605
#include "../Common/TagMaps/MarkerTags.h"
#include "../Common/Definitions/MarkerSetDefinition.h"
#include <float.h>
typedef int32 ConsumablePotionBuffTypes; BOOL QuestIsGameOver();  // X/Common/Quests/Quests.cpp:2693, X/Common/Player.h:422
#include "../Common/HirelingSwig.h"
ACDID PlayerGetPet(ACDID idACDPlayer, int32 nIndex); int32 PlayerGetPetCount(ACDID idACDPlayer); int32 PlayerGetNumPetsOfType(ACDID idACDPlayer, PetType eType); void ServerMessageSendAddPet(const PlayerList & listDestPlayers, const ACDID idACDPet); void SHirelingSetActive(ACDID idACDPlayer, HirelingClass eClass, BOOL bSave); void ActorRefreshHirelingKnownActors(HirelingClass eClass); void PowerAPI_BuffApplyOnPetAddEffects(ACDID idACDSummoner, ACDID idACDPet, PetType ePetType, SNO snoPet); int32 PlayerGetEmptyPetSlot(Player & tPlayer, ACDID idACDPet, PlayerPet ** ptPet); void PlayerPetInit(PlayerPet & tPlayerPet, PetType eType, ActorCommonData & tACDPet, Player & tPlayer); namespace SHero { ActorID CreateActor(uint32 dwPlayerFlags, const WorldPlace & tPlace, const Quaternion & q, Player & tPlayer, const D3::Hero::SavedDefinition & tHeroDefinition, const D3::Account::SavedDefinition & tAccountDefinition, const D3::Items::Mails & tMails, const uint32 dwSaveOptionBits); } void STrickleOnPlayerCreated(Player & tPlayer); ACDID SHirelingCreate(ActorCommonData & tACDPlayer, ActorCommonData & tACDNextTo, const HirelingClass eHirelingClass, BOOL bForceCreate, HirelingType eType); void NPCDismissProxy(ActorCommonData & tACDPlayer, HirelingClass eClass); DataID SACDInventoryAddToDirtyList(const ActorCommonData & tACDItem, ActorInventoryDatabaseAction eAction, InventorySlotType eCameFrom, InventorySlotType eDestination); static void sSetLimitedDurationItemPollTime(ActorCommonData & tACDPlayer);  // X/Common/Player.cpp:2721, 2767, 2672; X/Server/Network/ServerMessageSend.cpp:294; X/Server/SHireling.cpp:891; X/Server/Actors/Actors.cpp:7026; X/Server/Powers/PowerAPI.cpp:6359; lines 3365, 3384; X/Server/SHero.cpp:933; X/Server/Network/STrickle.cpp:288; X/Server/SHireling.cpp:642; X/Server/SNPC.cpp:793; X/Common/ActorInventory.cpp:11891; line 11461
int32 PowerActorGetMaxCharges(const ActorCommonData & tACD, SNO snoPower);  // X/Common/Powers/PowersCommon.cpp:4356
int32 PowerActorGetRechargeTime(const ActorCommonData & tACD, SNO snoPower);  // X/Common/Powers/PowersCommon.cpp:4373
float PowerGetCoolDownReductionPercent(SNO snoPower, const ActorCommonData & tACD);  // X/Common/Powers/PowersCommon.cpp:4541
BOOL SPlayerApplyConsumablePotionBuffs(ActorCommonData & tACDPlayer);  // line 11806
namespace NephalemRiftManager {
BOOL IsTieredRiftActive();  // X/Server/System/SNephalemRift.cpp:1853
BOOL PlayerPaidForRift(const Player * ptPlayer);  // X/Server/System/SNephalemRift.cpp:1521
}
static void sPopulateAchievementHealingList(); void SPlayerDelete(Player & tPlayer);  // lines 3745, 1794
BOOL ReplayIsReplayGame();  // X/Common/Replay.cpp
void PlayerLoadoutPotionUpdate(LoadoutPotionData & tLoadoutPotion, const ActorCommonData & tACDItem);  // X/Common/Player.cpp:4491
static const uint32 PLAYER_SAVE_INTERVAL = 18000;  // line 581
static const uint32 PLAYER_SAVE_WITHIN_5_SECONDS = 300;  // line 582
static const uint32 PLAYER_SAVE_MAX_DIRTY_ITEMS = 150;  // line 583
static const uint32 PLAYER_ITEM_DUPE_SCAN_INTERVAL = 36000;  // line 584
void ServerClientDisconnectGC(GameConnectionID idGameConnection); void ServerPlayerLeftOnConnection(GameConnectionID idGameConnection, const Player & tPlayer); void ServerDisconnectWhenOutgoingQueueEmpty(Player & tPlayer); void SPlayerOffline(Player & tPlayer, BOOL bHonorLogoutTimerRules);  // X/Common/Networking/GameMessage.h:847; X/Server/Network/Server.cpp:772, 796, 661; line 7584
// Defined further down in this file.
void sCheckForCommunityBuffReset(ActorCommonData & tACDPlayer, const GlobalSNO eGlobalPowerSNO);
void SPlayerPurgeSaveList(BOOL bForcedPurge, const OnlineServiceServer::GenericCallback & cb);
void SPlayerClearSavePoint(Player & tPlayer);
void sPowerRechargeCallback(TimerID, DataID idOwner, void * ptData);
uint32 SPlayerGetPowerChargeTime(Player * ptPlayer, SNO snoPower);
void SPlayerKillPetsFromPowerSNO(ACDID idACDPlayer, SNO snoPower, BOOL bDismissOnly);
void SPlayerRegisterPowerForRecharge(Player * ptPlayer, SNO snoPower);
void ACDInventoryEquipmentOrSkillChanged(ActorCommonData & tACD);  // X/Common/ActorInventory.cpp:9033
void SPlayerRemoveBuffsAndPetsForRespec(ActorCommonData & tACDPlayer, const SNO snoPower);
static void sSPlayerRemoveTrait(Player & tPlayer, SNO snoPower, int32 nSlot);
int32 SPlayerGetSlotForActiveSkill(Player & tPlayer, SNO snoPower);
void SPlayerWorldCreatingBuffRemove(ActorCommonData & tACDPlayer);
void SPlayerApplyParagonBuff(ActorCommonData & tACDPlayer);
const OnlineService::ContentLicense * SPlayerGetContentLicense(const Player & tPlayer, OnlineService::ContentLicenseId licenseId);
void SPlayerWorldCreatingBuffApply(ActorCommonData & tACDPlayer);
inline BOOL SPlayerHasValidSavePointForPort(const Player & tPlayer);
void SPlayerResetParagonPoints(Player & tPlayer, int32 nCategory);
void PlayerSendHeroStateCurrencyToClient(Player & tPlayer, BOOL bOnlySendDirtyCurrencies);
BOOL SPlayerCheckCanWarpToWaypoint(Player * ptPlayer, uint32 uWaypointIndex);
void SPlayerRemoveTraitInSlot(Player & tPlayer, const int32 nSlot);
void SPlayerAssignToTraitSlot(Player & tPlayer, const SNO snoPower, const int32 nSlot, BOOL bApplyCooldown);
int32 SPlayerGetEmptyActiveSkillSlotForPower(Player & tPlayer, const SNO snoPower);
void SPlayerAssignToActiveSkillSlot(Player & tPlayer, const SNO snoPower, const RuneType eRune, const int32 nSlot, BOOL bApplyCooldown);
int32 SPlayerGetUpscalingLevel();
void SPlayerSetUpscalingLevel(ActorCommonData & tACDPlayer, int32 nUpscalingLevel);
void SPlayerSetLevelCap(ActorCommonData & tACDPlayer, OnlineService::ContentLicenseId eLicenseId);
void SPlayerKillActiveCosmeticPet(Player & tPlayer);
void SPlayerQueueFloatingNumberHealing(ActorCommonData & tACDTarget, float flHealAmount);
void SPlayerUpdateCombatStatus(Player & tPlayer);
ACDID SPlayerGetStartACD(const SWorldID idSWorld, const SSceneID idSScene, const Team eTeam, const uint32 uEntranceGUID);
void PlayerSendTutorialStashIconsStateToClient(const Player * ptPlayer, const PlayerList & listDestPlayers);
BOOL SPlayerTestAllowedToInspect(ACDID idACDPlayer, ACDID idACDToInspect);
BOOL SPlayerInScriptedSequence(const Player & tPlayer);
void SPlayerLearnLore(const ACDID idACDPlayer, const SNO snoLore);
void SPlayerClearAllReturnPoints(Player & tPlayer, SNO snoWorld);
void SPlayerClearHeardConversations(Player & tPlayer);
static void sSPlayerWarpedInternal(ActorCommonData & tACDPlayer, const PlayerWarpReason eReason,
                                   const ACDID idACDWarpGizmo, const WorldPlace * ptPlaceBeforeWarp,
                                   const float flWarpFadeInSecs, const BOOL bSendClientWarped);
void SPlayerSendBanner(const PlayerList & listDestPlayers, const Player & tPlayer);  // line 2936
static XVarScope s_scopePerk("Perk", NULL);  // line 184
static XVarBool s_varIgrPerkEnabled(s_scopePerk, "IgrEnabled", false);
static XVarBool s_varAnniversaryPerkEnabled(s_scopePerk, "AnniversaryEnabled", false);
static XVarBool s_varCommunityEventPerkEnabled(s_scopePerk, "CommunityEventEnabled", false);
XVarFloat s_varIgrPerkXP(s_scopePerk, "IgrXP", 0.0f);
XVarFloat s_varAnniversaryPerkXP(s_scopePerk, "AnniversaryXP", 0.0f);
XVarFloat s_varCommunityEventPerkXP(s_scopePerk, "CommunityEventXP", 0.0f);
XVarFloat s_varCommunityEventPerkLegendaryFind(s_scopePerk, "CommunityEventLegendaryFind", 0.0f);
XVarFloat s_varCommunityEventPerkGoldFind(s_scopePerk, "CommunityEventGoldFind", 0.0f);
XVarBool s_varCommunityEventPerkDoubleRiftKeystones(s_scopePerk, "CommunityEventDoubleRiftKeystones", false);
XVarBool s_varCommunityEventPerkDoubleBloodShards(s_scopePerk, "CommunityEventDoubleBloodShards", false);
XVarBool s_varCommunityEventPerkDoubleTreasureGoblins(s_scopePerk, "CommunityEventDoubleTreasureGoblins", false);
XVarBool s_varEasterEggWorldPerkEnabled(s_scopePerk, "EasterEggWorldEnabled", false);
XVarBool s_varCommunityEventPerkDoubleBountyBags(s_scopePerk, "CommunityEventDoubleBountyBags", false);
bool sub_71007b7080();  // 0x71007b7080 (unnamed, SGame.cpp): tStartParams.dwCreationFlags bit 20
static XVarScope s_scopeDashingStrikeStuck("DashingStrikeStuck", NULL);  // line 199
XVarBool s_varDashingStrikeStuckMaxDurSpew(s_scopeDashingStrikeStuck, "EnableMaxDurationSpew", false);
BOOL sub_71007d50c0(SNO snoWorld);  // 0x71007d50c0 (unnamed 2.7.7 function, also declared in SpellEffect.cpp)
static XVarScope s_scopeSave("SaveMode", NULL);  // line 202
XVarBool s_varParallelEnabled(s_scopeSave, "ParallelEnabled", false);

struct DPSBucket {  // size 0x8
    uint32 dwTick;  // 0x00
    float flDamage;  // 0x04
};

typedef XGrowableList<DPSBucket, const DPSBucket &> DPSBucketList;

struct SpeedBucket {  // size 0x8
    uint32 dwTick;  // 0x00
    float flSpeed;  // 0x04
};

typedef XGrowableList<SpeedBucket, const SpeedBucket &> SpeedBucketList;

struct PingBucket {  // size 0x8
    uint32 dwTick;  // 0x00
    int32 nResponseTicks;  // 0x04
};

typedef XGrowableList<PingBucket, const PingBucket &> PingBucketList;

typedef blz::vector<unsigned long, blz::allocator<unsigned long> > AchievementList;

struct PerPlayerGlobals {  // size 0x350
    KnownListID tKnownListCurrent;  // 0x00
    KnownListID tKnownListPrevious;  // 0x04
    ServerTrickleList * pListPossibleTrickle;  // 0x08
    ServerTrickleList * pListTricklePrev;  // 0x10
    ServerTrickleList * pListTrickle;  // 0x18
    uint32 uExperienceTagId;  // 0x20
    uint32 uSessionFlags;  // 0x24
    OnlineService::ContentLicenseArray tContentLicenses;  // 0x28
    OnlineServiceServer::LicenseArray tRawLicenses;  // 0x40
    uint32 uContentFlags;  // 0x58
    SPlayerConsumables tConsumables;  // 0x60
    uint32 uConsoleLicenses;  // 0x78
    BOOL bHasFriends;  // 0x7c
    float flDPSDamageTotal;  // 0x80
    DPSBucketList listDPSInfo;  // 0x88
    SpeedBucketList listRecordedSpeedInfo;  // 0xe0
    TickBankBucketList listTickBankBuckets;  // 0x138
    TickRateTracker tTickRateTracker;  // 0x190
    PingBucketList listPings;  // 0x290
    uint32 uPingSentTime;  // 0x2e8
    float flHighestPing;  // 0x2ec
    uint32 tNextAchievementRegisterAttempt;  // 0x2f0
    uint32 tNextAchievementUnregisterAttempt;  // 0x2f4
    AchievementList tRecentAchievementsEarned;  // 0x2f8
    uint32 uLastAchievementEarnedTime;  // 0x310
    int32 nTrickleLoreAchievementIndex;  // 0x314
    int32 nTrickleCosmeticAchievementIndex;  // 0x318
    BOOL bGracefulDisconnect;  // 0x31c
    DeliveredRewardsSavedData tDeliveredRewards;  // 0x320
};
static_assert(sizeof(PerPlayerGlobals) == 0x350, "PerPlayerGlobals");
static_assert(offsetof(PerPlayerGlobals, listDPSInfo) == 0x88, "PerPlayerGlobals");
static_assert(offsetof(PerPlayerGlobals, tTickRateTracker) == 0x190, "PerPlayerGlobals");
static_assert(offsetof(PerPlayerGlobals, tDeliveredRewards) == 0x320, "PerPlayerGlobals");

struct ServerPlayerRegistration {  // size 0x18
    BOOL bUsed;  // 0x00
    OnlineService::HeroId uHeroId;  // 0x08
    Blizzard::Time::Timestamp tLeftTime;  // 0x10
};
struct SkillCooldownData { SNO snoPower; /* 0x00 */ uint32 dwDuration; /* 0x04 */ SkillCooldownData(); };  // size 0x8; original line 402
struct SPlayerSaveRecord {  // size 0xc
    ACDID idACDPlayer;  // 0x00
    uint32 dwHeroSaveOptions;  // 0x04
    PlayerSaveReason reason;  // 0x08
};
struct BuffCooldownData { SNO snoPower; /* 0x00 */ int32 nBuffIndex; /* 0x04 */ uint32 dwDuration; /* 0x08 */ BuffCooldownData() {} /* body not recoverable: the one use overwrites all three fields */ };  // size 0xc; original line 415
typedef blz::unordered_map<unsigned long, SPlayerSaveRecord, blz::hash<unsigned long>, blz::equal_to<unsigned long>, blz::allocator<blz::pair<const unsigned long, SPlayerSaveRecord> > > SPlayerSaveMap;
static const int32 NUM_SAVED_PLAYER_COOLDOWNS = 16; struct PlayerCooldownData { OnlineService::HeroId uHeroId; /* 0x00 */ uint32 dwLeaveTime; /* 0x08 */ SkillCooldownData arSkillCooldownData[6]; /* 0x0c */ blz::vector<BuffCooldownData, blz::allocator<BuffCooldownData> > arBuffCooldownData; /* 0x40 */ PlayerCooldownData(); void Clear() { uHeroId = OnlineService::INVALID_HERO_ID; dwLeaveTime = 0; arBuffCooldownData.clear(); for (int32 i = 0; i < 6; i++) { arSkillCooldownData[i].snoPower = SNO_NONE; arSkillCooldownData[i].dwDuration = 0; } } /* original lines 447-452 */ };  // size 0x58; original lines 458, 430
static const char * s_PlayerSaveReasonNames[33] = {  // line 210
    "Item", "Timer", "Utility", "Delete", "Web", "Clear", "Console", "Reset", "SendBug", "DebugDump",
    "LocalPlayer", "Dead", "DB", "Hardcore", "Diablo", "Deprecated_Consolidated", "Create", "ServerQuit",
    "PlayerQuit", "Checkpoint", "GameEnd", "LevelUp", "QuestComplete", "RareItem", "Adventure", "Store",
    "ChallengeRift", "", "", "", "", "", "",
};
#include "../../Contrib/Contrib/blz/include/blz/deque.h"
struct SessionTrackedData {  // size 0x18
    OnlineService::HeroId uHeroId;  // 0x00
    uint32 dwLastPlayedAmount;  // 0x08
    uint32 nNumNephalemStacks;  // 0x0c
    uint32 dwNephalemBuffDurationRemaining;  // 0x10
    uint32 nNumDungeonFinderProgress;  // 0x14
};
struct PendingGameSave { GameSaveData * ptGameSaveData; /* 0x00 */ OnlineServiceServer::GenericCallback cb; /* 0x08 */ };  static_assert(sizeof(PendingGameSave) == 0x10, "PendingGameSave");  // size 0x10; line 334
struct ServerPlayerGlobals {  // size 0x17d0
    PerPlayerGlobals tPerPlayer[4];  // 0x00
    UpdateData tPlayerUpdateData;  // 0xd40
    XRingBuffer<SessionTrackedData, SessionTrackedData, 8> tPreviousSessions;  // 0xd60
    blz::deque<PendingGameSave, blz::allocator<PendingGameSave> > tPendingSaves;  // 0xe58
    blz::vector<EscrowSaveData, blz::allocator<EscrowSaveData> > tPendingEscrowSaveData;  // 0xe78
    blz::vector<ConsumableSaveRecord, blz::allocator<ConsumableSaveRecord> > tPendingConsumableSaveRecords;  // 0xe90
    XPooledDataIDList listVisitedSScenes;  // 0xea8
    XPooledDataIDList listLoadingACDs;  // 0xed0
    ServerPlayerRegistration tPlayerRegistrationArray[32];  // 0xef8
    SPlayerSaveMap tPlayerSaveMap;  // 0x11f8
    uint32 dwNextAllPlayersSaveTime;  // 0x1218
    int32 nPlayerCooldownIndex;  // 0x121c
    PlayerCooldownData tPlayerCooldownData[NUM_SAVED_PLAYER_COOLDOWNS];  // 0x1220
    OnlineService::RewardIdList tRewardHealingList;  // 0x17a0
    uint32 dwNextItemDupeScan;  // 0x17b8
    uint32 uNextPingTime;  // 0x17bc
    uint32 dwNextAvengerSpawnCheck;  // 0x17c0
    uint64 uGameLeaderAccountId;  // 0x17c8
};
static_assert(sizeof(ServerPlayerGlobals) == 0x17d0, "ServerPlayerGlobals");
static_assert(offsetof(ServerPlayerGlobals, tPendingSaves) == 0xe58, "ServerPlayerGlobals");
static_assert(offsetof(ServerPlayerGlobals, tPendingEscrowSaveData) == 0xe78, "ServerPlayerGlobals");
static_assert(offsetof(ServerPlayerGlobals, tPendingConsumableSaveRecords) == 0xe90, "ServerPlayerGlobals");
static_assert(offsetof(ServerPlayerGlobals, listVisitedSScenes) == 0xea8, "ServerPlayerGlobals");
static_assert(offsetof(ServerPlayerGlobals, tPlayerSaveMap) == 0x11f8, "ServerPlayerGlobals");
static_assert(offsetof(ServerPlayerGlobals, tRewardHealingList) == 0x17a0, "ServerPlayerGlobals");
static_assert(offsetof(ServerPlayerGlobals, dwNextAvengerSpawnCheck) == 0x17c0, "ServerPlayerGlobals");



static BOOL sPlayerTrickleLoreAchievements(Player & tPlayer, PerPlayerGlobals & tPerPlayer); static BOOL sPlayerTrickleCosmeticAchievements(Player & tPlayer, PerPlayerGlobals & tPerPlayer);  // lines 3798, 3835
static void sTickPlayerAchievementNotifications(Player & tPlayer, PerPlayerGlobals & tPerPlayer, BOOL bFlushAllImmediately); void SPlayerRecalculateUpscalingLevels(); BOOL SPlayerRemoveFromSaveList(Player & tPlayer); void SPlayerSaveCooldowns(ActorCommonData & tACDPlayer, Player & tPlayer); void SPlayer_ClientLoadComplete(Player & tPlayer); void SPlayerClearEnemyList(Player & tPlayer); void SPlayerUnregisterExperienceId(const Player & tPlayer); void SPlayerEvaluateMultiplayerBuff(); void SPlayerBootAvenger(Player & tPlayer);  // lines 3709, 13633, 5016, 6048, 8473, 5149, 7498, 12081, 5796























#include "../Client/Console/Online.h"
static void sConsoleSerialize(SGameID idSGame, GameSaveDataCampaignOrAdventureMode * ptGameSaveData, const OnlineServiceServer::GenericCallback & pfnCallback);  // line 1330
void sOnGameSaved(SGameID idSGame, GameSaveData * ptGameSaveData, const OnlineServiceServer::GenericCallback & pfnCallback, DbErrorCode dbErrCode);  // line 1377
static void sSavePlayersInMap(const OnlineServiceServer::GenericCallback & pfnCallback); void SPlayerIssuePendingSave();  // lines 1143, 1203

GameSaveData * GameSaveData::PopulateData(SGame & tSGame)
{
    GameSaveData * gameData = NULL;

    GameType gameType = GameGetGameType();
    if (gameType == GT_CHALLENGE_RIFTS)
    {
        gameData = new GameSaveDataChallengeRifts();
    }
    else
    {
        gameData = new GameSaveDataCampaignOrAdventureMode();
    }


    gameData->idSGame = tSGame.id;
    SPlayerSaveMap & tPlayerSaveMap = tSGame.ptServerPlayerGlobals->tPlayerSaveMap;
    for (SPlayerSaveMap::iterator it = tPlayerSaveMap.begin(); it != tPlayerSaveMap.end(); ++it)
    {
        const SPlayerSaveRecord & tSaveRecord = it->second;
        Player * ptPlayer = PlayerGetByACD(tSaveRecord.idACDPlayer);


        if (tSaveRecord.dwHeroSaveOptions & (1 << SHero::HSO_PLAYER_DELETED_FROM_GAME_BIT))
        {
            gameData->accountsToUnlock.insert(ptPlayer->tAccount.tHandle.m_id);
        }
    }

    if (!gameData->PopulateDataFromGame(tSGame))
    {
        ::delete gameData; gameData = NULL;
    }

    return gameData;
}



void GameSaveDataCampaignOrAdventureMode::IssueSave(const OnlineServiceServer::GenericCallback & pfnCallback)
{
    SGame & tSGame = SGameManager::GameGet();


    sConsoleSerialize(tSGame.id, this, pfnCallback);
}




















void GameSaveDataCampaignOrAdventureMode::PerformPostSaveOperations(DbErrorCode dbErrCode)
{

    for (blz::vector<EscrowSaveData>::const_iterator it = tEscrowData.cbegin(); it != tEscrowData.cend(); ++it)
    {
        it->cb(dbErrCode ? OnlineService::ERROR_GLOBAL_DB_FAILURE : OnlineService::ERROR_GLOBAL_SUCCESS, NULL);
    }


    for (blz::vector<ConsumableSaveRecord>::const_iterator it = tConsumableSaveRecords.cbegin(); it != tConsumableSaveRecords.cend(); ++it)
    {

        SGameManager::AddAccountSignal(idSGame, GAST_CLEAR_PENDING_CONSUMABLE_SAVE, it->account_id, XVariant::MakeUint64(it->transaction_id), OnlineServiceServer::ProtoMessageCallback());


        it->cb(dbErrCode ? OnlineService::ERROR_GLOBAL_DB_FAILURE : OnlineService::ERROR_GLOBAL_SUCCESS, NULL);
    }
















}



bool GameSaveDataCampaignOrAdventureMode::PopulateDataFromGame(SGame & tSGame)
{
    SPlayerSaveMap & tPlayerSaveMap = tSGame.ptServerPlayerGlobals->tPlayerSaveMap;

    BOOL bIsHardcoreGame = (tSGame.tStartParams.dwCreationFlags & (1 << GAMECREATEPARAM_HARDCORE)) != 0;
    BOOL bIsSeasonGame = (tSGame.tStartParams.dwCreationFlags & (1 << GAMECREATEPARAM_SEASON)) != 0;
    eGamePartitionType = GetPartition(bIsHardcoreGame, bIsSeasonGame);


    for (SPlayerSaveMap::iterator it = tPlayerSaveMap.begin(); it != tPlayerSaveMap.end(); ++it)
    {
        const SPlayerSaveRecord & tSaveRecord = it->second;
        Player * ptPlayer = PlayerGetByACD(tSaveRecord.idACDPlayer);







        if (!Console::Online::PartyIsPlayerAndHeroStillPresent(ptPlayer->nPlayerIndex, ptPlayer->uHeroId))
        {
            continue;
        }





        uint32 dwHeroSaveOptions = tSaveRecord.dwHeroSaveOptions;
        dwHeroSaveOptions |= (1 << SHero::HSO_ENCODE_ITEM_DATA);

        tPlayerSaveDataVec.push_back(SHero::PlayerSaveData());
        SHero::PlayerSaveData & tPlayerSaveData = tPlayerSaveDataVec.back();
        if (!SHero::Save(*ptPlayer, tPlayerSaveData, dwHeroSaveOptions))
        {






            tPlayerSaveDataVec.pop_back();
        }
    }





    ACDPurgeGlobalItemLists(&tDirtyInventoryRecords);
    ACDUpdateActorPersistState(tDirtyInventoryRecords);


    MailRecordMap & mailRecords = ACDMailMapGet();
    blz::vector<unsigned long> removeMailIds;
    for (MailRecordMap::iterator iter = mailRecords.begin(); iter != mailRecords.end(); ++iter)
    {
        ActorInventoryMailRecord & tMailRecord = iter->second;
        if (tMailRecord.bDirty)
        {
            tMailRecord.bDirty = false;
            tDirtyMailRecords[iter->first] = iter->second;

            if (tMailRecord.eStatus == MS_DELETED)
            {
                removeMailIds.push_back(tMailRecord.tMailId);
            }
        }
    }


    for (blz::vector<unsigned long>::const_iterator it = removeMailIds.begin(); it != removeMailIds.end(); ++it)
    {
        mailRecords.erase(*it);
    }

    tEscrowData.swap(tSGame.ptServerPlayerGlobals->tPendingEscrowSaveData);
    tConsumableSaveRecords.swap(tSGame.ptServerPlayerGlobals->tPendingConsumableSaveRecords);


    return true;
}

void GameSaveDataChallengeRifts::IssueSave(const OnlineServiceServer::GenericCallback & pfnCallback)
{
    SGame & tSGame = SGameManager::GameGet();

    if (GameIsBattleNetServer())
    {
        return;
    }



















    sOnGameSaved(tSGame.id, this, pfnCallback, 0);

}



void GameSaveDataChallengeRifts::PerformPostSaveOperations(DbErrorCode)
{
}



bool GameSaveDataChallengeRifts::PopulateDataFromGame(SGame & tSGame)
{
    SPlayerSaveMap & tPlayerSaveMap = tSGame.ptServerPlayerGlobals->tPlayerSaveMap;

    for (SPlayerSaveMap::iterator it = tPlayerSaveMap.begin(); it != tPlayerSaveMap.end(); ++it)
    {
        const SPlayerSaveRecord & tSaveRecord = it->second;
        Player * ptPlayer = PlayerGetByACD(tSaveRecord.idACDPlayer);


        tPlayerSaveDataVec.push_back(SHero::ChallengeRiftPlayerSaveData());
        SHero::ChallengeRiftPlayerSaveData & tPlayerSaveData = tPlayerSaveDataVec.back();
        tPlayerSaveData.idGameAccount = ptPlayer->tAccount.tHandle;
        tPlayerSaveData.tSeenTutorials = blz::string((const char *)ptPlayer->tSeenTutorials.GetRawBuffer(), ptPlayer->tSeenTutorials.GetLength());
        tPlayerSaveData.tAccountData.set_last_challenge_reward_earned(ptPlayer->nLastRewardChallengeNumber);
        ptPlayer->tHeroStateData.tSavedData.tSavedConversations.Export(*tPlayerSaveData.tAccountData.mutable_saved_conversations());


        if (ptPlayer->tChallengeMail.mail_id() != 0)
        {
            tPlayerSaveData.tRewardMail = ptPlayer->tChallengeMail;
            ptPlayer->tChallengeMail.Clear();
        }


        if (tSaveRecord.dwHeroSaveOptions & (1 << SHero::HSO_QUIT_AFTER_SAVE_BIT))
        {
            SPlayerDisconnect(*ptPlayer, 0, FALSE);
        }
    }
    return true;
}



static void DoPreSaveWork(SGame & tSGame)
{
    SPlayerSaveMap & tPlayerSaveMap = tSGame.ptServerPlayerGlobals->tPlayerSaveMap;
    SGameGlobals & tSGameGlobals = *tSGame.ptSGameGlobals;


    for (SPlayerSaveMap::iterator it = tPlayerSaveMap.begin(); it != tPlayerSaveMap.end(); ++it)
    {
        const SPlayerSaveRecord & tSaveRecord = it->second;
        Player * ptPlayer = PlayerGetByACD(tSaveRecord.idACDPlayer);



        if (!GameIsBattleNetServer() && tSGameGlobals.ptAchievementsInterface)
        {
            for (SPlayerSaveMap::iterator it = tPlayerSaveMap.begin(); it != tPlayerSaveMap.end(); ++it)
            {
                tSGameGlobals.ptAchievementsInterface->Flush(ptPlayer->tAccount.tHandle);
            }
        }





        if (!GameIsWeeklyChallenge() &&
            !Console::Online::PartyIsPlayerAndHeroStillPresent(ptPlayer->nPlayerIndex, ptPlayer->uHeroId))
        {
            continue;
        }




        SPlayerConsoleSync(*ptPlayer);


        PlayerList listDestPlayer(PlayerListGetElementPool());
        PlayerListGetSingle(listDestPlayer, *ptPlayer);

        DWordDataMessage tMessage;
        tMessage.dwData = tSaveRecord.reason;
        MessageSendToClient(listDestPlayer, SMSG_CONSOLE_SAVE, &tMessage, sizeof(tMessage));

    }

}

void SPlayerConsumable::Import(const D3::Account::Consumable & tConsumable)
{
    uLicenseInstanceId = tConsumable.license_instance_id();
    uLicenseId = tConsumable.license_id();
    uTransactionId = tConsumable.transaction_id();
    uType = tConsumable.type();
    nQuantity = tConsumable.quantity();
    tConsumeTime = tConsumable.has_consume_time() ? tConsumable.consume_time() : 0;
    tRevokeTime = tConsumable.has_revoke_time() ? tConsumable.revoke_time() : 0;
    uDataPersistFlags = tConsumable.data_persist_flags();
}




void SPlayerConsumable::Export(D3::Account::Consumable & tConsumable) const
{
    tConsumable.set_license_instance_id(uLicenseInstanceId);
    tConsumable.set_license_id(uLicenseId);
    tConsumable.set_transaction_id(uTransactionId);
    tConsumable.set_type(uType);
    tConsumable.set_quantity(nQuantity);
    if (tConsumeTime != 0)
    {
        tConsumable.set_consume_time(tConsumeTime);
    }
    if (tRevokeTime != 0)
    {
        tConsumable.set_revoke_time(tRevokeTime);
    }

    tConsumable.set_data_persist_flags(uDataPersistFlags);
}



void SPlayerConsumables::Import(
    const D3::Account::Consumables & tConsumables,
    BOOL bClearOldContents)
{
    if (bClearOldContents)
    {
        arConsumables.clear();
    }

    SPlayerConsumable tempConsumable;
    for (int32 i = 0; i < tConsumables.consumed_licenses_size(); i++)
    {
        tempConsumable.Import(tConsumables.consumed_licenses(i));
        arConsumables.push_back(tempConsumable);
    }
}























void SPlayerConsumables::Export(
    D3::Account::Consumables & tConsumables,
    uint32 uDataPersistFlagsFilter,
    uint32 uDataPersistFlagsToClear,
    uint32 uDataPersistFlagsToSet)
{
    uint32 uClearMask = ~uDataPersistFlagsToClear;

    for (size_t i = 0; i < arConsumables.size(); i++)
    {
        if (uDataPersistFlagsFilter == 0 || (arConsumables[i].uDataPersistFlags & uDataPersistFlagsFilter) != 0)
        {
            D3::Account::Consumable * ptConsumable = tConsumables.add_consumed_licenses();
            arConsumables[i].Export(*ptConsumable);

            arConsumables[i].uDataPersistFlags &= uClearMask;
            arConsumables[i].uDataPersistFlags |= uDataPersistFlagsToSet;
        }
    }
}





































void SPlayerPurgeSaveList(
    BOOL bForcedPurge, const OnlineServiceServer::GenericCallback & cb)
{

    SGameGlobals & tSGameGlobals = SGameGlobalsGet();

    if (tSGameGlobals.dwFlags & (1 << SGAMEGLOBALS_DISABLED_HERO_SAVE))
        return;


    ServerPlayerGlobals * ptServerPlayerGlobals = SGameManager::GameGet().ptServerPlayerGlobals;


    if (!bForcedPurge)
    {   const int numMoves = 0;
        int32 nSize = ACDDirtyListGetSize();
        if (nSize + numMoves > PLAYER_SAVE_MAX_DIRTY_ITEMS)
        {
            bForcedPurge = TRUE;
        }
    }

    uint32 dwCurrentTime = GameGetServerTime();
    if (!bForcedPurge && dwCurrentTime < ptServerPlayerGlobals->dwNextAllPlayersSaveTime)
        return;

    GameCommonData & tGCD = SGameGetGameCommonData();
    PlayerGlobals * ptPlayerGlobals = tGCD.ptPlayerGlobals;
    SPlayerSaveMap & tPlayerSaveMap = ptServerPlayerGlobals->tPlayerSaveMap;

    for (int32 i = 0; i < 4; i++)
    {
        Player & tPlayer = ptPlayerGlobals->tPlayerArray[i];

        if (tPlayer.idPlayerACD != ACDID_INVALID && tPlayerSaveMap.find(tPlayer.tAccount.tHandle.m_id) == tPlayerSaveMap.end())
        {
            SPlayerAddToSaveList(tPlayer, 0, PlayerSaveReason_Timer, PLAYER_SAVE_MODE_LAZY);
        }
    }


    if (tPlayerSaveMap.size() > 0)
    {
        SGame & tSGame = SGameManager::GameGet();
        sSavePlayersInMap(
            cb ? cb : OnlineServiceServer::GenericCallback(OnlineService::internal::function_binder_2<void, SGameID, OnlineService::ErrorCode>::bind<&SGame::OnSavedHeros>(tSGame.id)));


        tPlayerSaveMap.clear();
    }

    ptServerPlayerGlobals->dwNextAllPlayersSaveTime = dwCurrentTime + PLAYER_SAVE_INTERVAL;

}




void sAddToSaveList(
    const Player & tPlayer,
    uint32 dwHeroSaveOptions,
    PlayerSaveReason reason)
{
    ServerPlayerGlobals * ptServerPlayerGlobals = SGameManager::GameGet().ptServerPlayerGlobals;
    SPlayerSaveMap & tPlayerSaveMap = ptServerPlayerGlobals->tPlayerSaveMap;
    SPlayerSaveMap::iterator it = tPlayerSaveMap.find(tPlayer.tAccount.tHandle.m_id);
    if (it == tPlayerSaveMap.end())
    {
        SPlayerSaveRecord tSaveRecord;
        tSaveRecord.idACDPlayer = tPlayer.idPlayerACD;
        tSaveRecord.dwHeroSaveOptions = dwHeroSaveOptions;
        tSaveRecord.reason = reason;
        tPlayerSaveMap[tPlayer.tAccount.tHandle.m_id] = tSaveRecord;
    }
    else
    {
        SPlayerSaveRecord & tSaveRecord = tPlayerSaveMap[tPlayer.tAccount.tHandle.m_id];
        tSaveRecord.dwHeroSaveOptions |= dwHeroSaveOptions;
        if ((tSaveRecord.reason & reason) == 0)
        {
            char reasonStr[256];
            reasonStr[0] = 0;
            bool bFirst = true;
            int32 val = tSaveRecord.reason | reason;
            for (int i = 0; i < 33; i++)
            {
                if (val & (1 << i))
                {
                    if (!bFirst)
                        strncat_safe(reasonStr, sizeof(reasonStr), " | ", 3);
                    strncat_safe(reasonStr, sizeof(reasonStr), s_PlayerSaveReasonNames[i], strlen(s_PlayerSaveReasonNames[i]));
                    bFirst = false;
                }
            }
            tSaveRecord.reason |= reason;
        }
    }
}














static void sSavePlayersInMap(const OnlineServiceServer::GenericCallback & pfnCallback)
{

    SGameGlobals & tSGameGlobals = SGameGlobalsGet();
    if ((tSGameGlobals.dwFlags & (1 << SGAMEGLOBALS_DISABLED_HERO_SAVE)) ||
        !GameIsParamFlagSet(GPF_SAVE_HEROS))
    {
        return;
    }








    SGame & tSGame = SGameManager::GameGet();

    if (tSGame.ptServerPlayerGlobals->tPlayerSaveMap.empty())
    {
        return;
    }



    DoPreSaveWork(tSGame);

    GameSaveData * ptGameSaveData = GameSaveData::PopulateData(tSGame);



















    PendingGameSave tPendingGameSave;
    tPendingGameSave.ptGameSaveData = ptGameSaveData;
    tPendingGameSave.cb = pfnCallback;
    tSGame.ptServerPlayerGlobals->tPendingSaves.push_back(tPendingGameSave);


    SPlayerIssuePendingSave();
}




void SPlayerIssuePendingSave()
{
    SGameGlobals & tSGameGlobals = SGameGlobalsGet();





    if (!s_varParallelEnabled.Get())
    {

        if (tSGameGlobals.dwFlags2 & (1 << SGAMEGLOBALS2_SAVE_IN_PROGRESS))
        {
            return;
        }
    }

    ServerPlayerGlobals * ptServerPlayerGlobals = SGameManager::GameGet().ptServerPlayerGlobals;
    if (ptServerPlayerGlobals->tPendingSaves.empty())
    {
        return;
    }


    if (tSGameGlobals.dwFlags & (1 << SGAMEGLOBALS_DISABLED_HERO_SAVE))
    {

        while (!ptServerPlayerGlobals->tPendingSaves.empty())
        {
            PendingGameSave & tPendingGameSave = ptServerPlayerGlobals->tPendingSaves.front();











            ::delete tPendingGameSave.ptGameSaveData;
            ptServerPlayerGlobals->tPendingSaves.pop_front();
        }
        return;
    }

    PendingGameSave & tPendingSave = ptServerPlayerGlobals->tPendingSaves.front();
    GameSaveData * ptGameSaveData = tPendingSave.ptGameSaveData;
    OnlineServiceServer::GenericCallback pfnCallback = tPendingSave.cb;
    ptServerPlayerGlobals->tPendingSaves.pop_front();


    if (!s_varParallelEnabled.Get())
    {
        tSGameGlobals.dwFlags2 |= (1 << SGAMEGLOBALS2_SAVE_IN_PROGRESS);
    }







    ptGameSaveData->IssueSave(pfnCallback);
}





























































static void sConsoleSerialize(SGameID idSGame, GameSaveDataCampaignOrAdventureMode * ptGameSaveData, const OnlineServiceServer::GenericCallback & pfnCallback)
{





    for (uint32 i = 0; i < ptGameSaveData->tPlayerSaveDataVec.size(); i++)
    {
        SHero::PlayerSaveData & tSaveData = ptGameSaveData->tPlayerSaveDataVec[i];
        Player * ptPlayer = PlayerGetByGameAccount(tSaveData.idGameAccount);

        Player & tPlayer = *ptPlayer;


        Console::Online::PartyUpdateCachedRemotePlayerAccountAndHero(
            tPlayer.nPlayerIndex,
            tPlayer.uHeroId,
            tSaveData.tAccountDefinition,
            tSaveData.tHeroDefinition);
    }





















    sOnGameSaved(idSGame, ptGameSaveData, pfnCallback, 0);
}



void sOnGameSaved(SGameID idSGame, GameSaveData * ptGameSaveData, const OnlineServiceServer::GenericCallback & pfnCallback, DbErrorCode dbErrCode)
{





















    SGameManager::SignalClearSaveInProgress(idSGame);

    ptGameSaveData->PerformPostSaveOperations(dbErrCode);

    ::delete ptGameSaveData;

    pfnCallback(dbErrCode);
}



// NON_MATCHING: same instructions, but the address temporaries of the three XGrowableList::Initialize store groups are materialised in a different order (50 of 300 insns)
void SPlayersInitializeForGame(
    void)
{

    SGame & tSGame = SGameManager::GameGet();
    ServerPlayerGlobals *& ptServerPlayerGlobals = tSGame.ptServerPlayerGlobals;

    ptServerPlayerGlobals = (ServerPlayerGlobals *)SigmaMemoryNew(sizeof(ServerPlayerGlobals), 4, tSGame.ptMemoryInterface, TRUE);

    UpdateFunctionInitialize(UPDATE_PLAYERS, -1, ptServerPlayerGlobals->tPlayerUpdateData);
    ServerQueueUpdateFunction(60, ptServerPlayerGlobals->tPlayerUpdateData);

    new (&ptServerPlayerGlobals->tPreviousSessions) XRingBuffer<SessionTrackedData, SessionTrackedData, 8>(TRUE);
    new (&ptServerPlayerGlobals->tPendingSaves) blz::deque<PendingGameSave>;
    new (&ptServerPlayerGlobals->tPendingEscrowSaveData) blz::vector<EscrowSaveData>;
    new (&ptServerPlayerGlobals->tPendingConsumableSaveRecords) blz::vector<ConsumableSaveRecord>;
    new (&ptServerPlayerGlobals->tRewardHealingList) OnlineService::RewardIdList;


    for (int32 i = 0; i < MAX_PLAYERS; i++)
    {
        PerPlayerGlobals & tPerPlayer = ptServerPlayerGlobals->tPerPlayer[i];


        tPerPlayer.tKnownListCurrent = KnownListAlloc();
        tPerPlayer.tKnownListPrevious = KnownListAlloc();

        {
            void * pMem = SigmaMemoryNew(sizeof(ServerTrickleList), 4, tSGame.ptMemoryInterface, FALSE);
            tPerPlayer.pListPossibleTrickle = new (pMem) ServerTrickleList(MemoryPoolsGetDATAIDListPool());
        }
        {
            void * pMem = SigmaMemoryNew(sizeof(ServerTrickleList), 4, tSGame.ptMemoryInterface, FALSE);
            tPerPlayer.pListTricklePrev = new (pMem) ServerTrickleList(MemoryPoolsGetDATAIDListPool());
        }
        {
            void * pMem = SigmaMemoryNew(sizeof(ServerTrickleList), 4, tSGame.ptMemoryInterface, FALSE);
            tPerPlayer.pListTrickle = new (pMem) ServerTrickleList(MemoryPoolsGetDATAIDListPool());
        }

        tPerPlayer.listDPSInfo.Initialize(tSGame.ptMemoryInterface);
        tPerPlayer.listRecordedSpeedInfo.Initialize(tSGame.ptMemoryInterface);
        tPerPlayer.listTickBankBuckets.Initialize(tSGame.ptMemoryInterface);
        tPerPlayer.tTickRateTracker.Initialize();
        new (&tPerPlayer.tRecentAchievementsEarned) AchievementList;
        new (&tPerPlayer.tContentLicenses) OnlineService::ContentLicenseArray;
        new (&tPerPlayer.tDeliveredRewards) DeliveredRewardsSavedData();
        new (&tPerPlayer.tRawLicenses) OnlineServiceServer::LicenseArray;
        new (&tPerPlayer.tConsumables) SPlayerConsumables;

        tPerPlayer.listPings.Initialize(tSGame.ptMemoryInterface);
        tPerPlayer.uPingSentTime = 0;
        tPerPlayer.flHighestPing = 0.0f;
        tPerPlayer.bGracefulDisconnect = FALSE;
        tPerPlayer.nTrickleLoreAchievementIndex = 0;
        tPerPlayer.nTrickleCosmeticAchievementIndex = 0;
    }


    ptServerPlayerGlobals->listVisitedSScenes.Initialize(MemoryPoolsGetDATAIDListPool());
    ptServerPlayerGlobals->listLoadingACDs.Initialize(MemoryPoolsGetDATAIDListPool());

    ptServerPlayerGlobals->dwNextAllPlayersSaveTime = GameGetServerTime() + PLAYER_SAVE_INTERVAL;
    ptServerPlayerGlobals->dwNextItemDupeScan = GameGetServerTime() + PLAYER_ITEM_DUPE_SCAN_INTERVAL;
    ptServerPlayerGlobals->uNextPingTime = GameGetServerTime() + 180;

    ScopedGlobalDefinitionLockR pdefGlobal;
    ptServerPlayerGlobals->dwNextAvengerSpawnCheck = GameGetServerTime() + pdefGlobal->nAvengerUpdateCheckInterval * 60;

    new (&ptServerPlayerGlobals->tPlayerSaveMap) SPlayerSaveMap;
    ptServerPlayerGlobals->uGameLeaderAccountId = 0;
    ptServerPlayerGlobals->nPlayerCooldownIndex = 0;

    sPopulateAchievementHealingList();
}




void SPlayersDisposeForGame()

{
    SGame & tSGame = SGameManager::GameGet();
    ServerPlayerGlobals *& ptServerPlayerGlobals = tSGame.ptServerPlayerGlobals;


    GameCommonData & tGCD = SGameGetGameCommonData();
    PlayerGlobals * ptPlayerGlobals = tGCD.ptPlayerGlobals;
    for (int32 i = 0; i < 4; i++)
    {
        Player & tPlayer = ptPlayerGlobals->tPlayerArray[i];

        if (tPlayer.idPlayerACD != ACDID_INVALID)
        {
            SPlayerAddToSaveList(tPlayer, 1 << SHero::HSO_PLAYER_DELETED_FROM_GAME_BIT, PlayerSaveReason_Delete, PLAYER_SAVE_MODE_LAZY);
        }
    }







    for (Player * ptPlayer = PlayerGetFirstAll(); ptPlayer; ptPlayer = PlayerGetNextAll(ptPlayer))
    {
        SPlayerDelete(*ptPlayer);
    }



    for (int32 i = 0; i < 4; i++)
    {
        PerPlayerGlobals & tPerPlayer = ptServerPlayerGlobals->tPerPlayer[i];

        tPerPlayer.listPings.RemoveAll();
        tPerPlayer.listTickBankBuckets.RemoveAll();
        tPerPlayer.listRecordedSpeedInfo.RemoveAll();
        tPerPlayer.listDPSInfo.RemoveAll();

        tPerPlayer.listPings.Dispose();
        tPerPlayer.listTickBankBuckets.Dispose();
        tPerPlayer.listRecordedSpeedInfo.Dispose();
        tPerPlayer.listDPSInfo.Dispose();

        KnownListFree(tPerPlayer.tKnownListCurrent);
        KnownListFree(tPerPlayer.tKnownListPrevious);

        tPerPlayer.pListPossibleTrickle->RemoveAll();
        tPerPlayer.pListPossibleTrickle->~ServerTrickleList();
        SigmaMemoryFree(tPerPlayer.pListPossibleTrickle, tSGame.ptMemoryInterface);

        tPerPlayer.pListTricklePrev->RemoveAll();
        tPerPlayer.pListTricklePrev->~ServerTrickleList();
        SigmaMemoryFree(tPerPlayer.pListTricklePrev, tSGame.ptMemoryInterface);

        tPerPlayer.pListTrickle->RemoveAll();
        tPerPlayer.pListTrickle->~ServerTrickleList();
        SigmaMemoryFree(tPerPlayer.pListTrickle, tSGame.ptMemoryInterface);

        tPerPlayer.tRecentAchievementsEarned.~vector();
        tPerPlayer.tDeliveredRewards.~DeliveredRewardsSavedData();
        tPerPlayer.tContentLicenses.~vector();
        tPerPlayer.tRawLicenses.~vector();
        tPerPlayer.tConsumables.~SPlayerConsumables();
    }


    ptServerPlayerGlobals->listVisitedSScenes.Dispose();

    ptServerPlayerGlobals->listLoadingACDs.Dispose();


    ptServerPlayerGlobals->tPlayerSaveMap.clear();
    ptServerPlayerGlobals->tPlayerSaveMap.~SPlayerSaveMap();


    while (!ptServerPlayerGlobals->tPendingSaves.empty())
    {
        PendingGameSave & tPendingGameSave = ptServerPlayerGlobals->tPendingSaves.front();
        ::delete tPendingGameSave.ptGameSaveData;
        ptServerPlayerGlobals->tPendingSaves.pop_front();
    }
    ptServerPlayerGlobals->tPendingSaves.~deque();
    ptServerPlayerGlobals->tPendingEscrowSaveData.~vector();
    ptServerPlayerGlobals->tPendingConsumableSaveRecords.~vector();
    ptServerPlayerGlobals->tRewardHealingList.~vector();


    SigmaMemoryFree(ptServerPlayerGlobals, tSGame.ptMemoryInterface); ptServerPlayerGlobals = NULL;
}



KnownListID PlayerKnownListGetCurrent(const Player & tPlayer); KnownListID PlayerKnownListGetPrevious(const Player & tPlayer);  // lines 1601, 1620
void PlayerClearKnownList(
    const Player & tPlayer)
{


    KnownListID idKnownListCurrent = PlayerKnownListGetCurrent(tPlayer);
    KnownListID idKnownListPrev = PlayerKnownListGetPrevious(tPlayer);

    KnownListClear(idKnownListCurrent, TRUE, &tPlayer);
    KnownListClear(idKnownListPrev, FALSE, NULL);
}



KnownListID PlayerKnownListGetCurrent(
    const Player & tPlayer)
{
    ServerPlayerGlobals & tServerPlayerGlobals = *SGameManager::GameGet().ptServerPlayerGlobals;
    return tServerPlayerGlobals.tPerPlayer[tPlayer.nPlayerIndex].tKnownListCurrent;
}



void PlayerKnownListSetCurrent(
    const Player & tPlayer,
    KnownListID idKnownList)
{
    ServerPlayerGlobals & tServerPlayerGlobals = *SGameManager::GameGet().ptServerPlayerGlobals;
    tServerPlayerGlobals.tPerPlayer[tPlayer.nPlayerIndex].tKnownListCurrent = idKnownList;
}



KnownListID PlayerKnownListGetPrevious(
    const Player & tPlayer)
{
    ServerPlayerGlobals & tServerPlayerGlobals = *SGameManager::GameGet().ptServerPlayerGlobals;
    return tServerPlayerGlobals.tPerPlayer[tPlayer.nPlayerIndex].tKnownListPrevious;
}



void PlayerKnownListSetPrevious(
    const Player & tPlayer,
    KnownListID idKnownList)
{
    ServerPlayerGlobals & tServerPlayerGlobals = *SGameManager::GameGet().ptServerPlayerGlobals;
    tServerPlayerGlobals.tPerPlayer[tPlayer.nPlayerIndex].tKnownListPrevious = idKnownList;
}



static DPSBucketList & PlayerGetDPSBucketList(
    const Player & tPlayer)
{
    ServerPlayerGlobals * ptServerPlayerGlobals = SGameManager::GameGet().ptServerPlayerGlobals;
    return ptServerPlayerGlobals->tPerPlayer[tPlayer.nPlayerIndex].listDPSInfo;
}








static float & PlayerGetDPSDamageTotal(
    const Player & tPlayer)
{
    ServerPlayerGlobals * ptServerPlayerGlobals = SGameManager::GameGet().ptServerPlayerGlobals;
    return ptServerPlayerGlobals->tPerPlayer[tPlayer.nPlayerIndex].flDPSDamageTotal;
}








static SpeedBucketList & PlayerGetSpeedBucketList(
    const Player & tPlayer)
{




    ServerPlayerGlobals * ptServerPlayerGlobals = SGameManager::GameGet().ptServerPlayerGlobals;
    return ptServerPlayerGlobals->tPerPlayer[tPlayer.nPlayerIndex].listRecordedSpeedInfo;
}




TickBankBucketList & SPlayerGetTickBankList(
    const Player & tPlayer)
{
    ServerPlayerGlobals * ptServerPlayerGlobals = SGameManager::GameGet().ptServerPlayerGlobals;
    return ptServerPlayerGlobals->tPerPlayer[tPlayer.nPlayerIndex].listTickBankBuckets;
}








TickRateTracker & SPlayerGetTickRateTracker(
    const Player & tPlayer)
{
    ServerPlayerGlobals * ptServerPlayerGlobals = SGameManager::GameGet().ptServerPlayerGlobals;
    return ptServerPlayerGlobals->tPerPlayer[tPlayer.nPlayerIndex].tTickRateTracker;
}








void SPlayerKillAllPets(
    Player & tPlayer,
    BOOL bDismissFollowers,
    BOOL bKillHirelings,
    BOOL bKillCosmeticPets,
    BOOL bEnteringRift)
{
    if (bKillCosmeticPets)
        SPlayerKillActiveCosmeticPet(tPlayer);

    for (int32 i = 0; i < MAX_PETS; i++)
    {
        ACDID idACDPet = PlayerGetPet(tPlayer.idPlayerACD, i);
        if (idACDPet != ACDID_INVALID)
        {
            ActorID idPet = (ActorID)ACDGetOwner(idACDPet);
            Actor * ptPetActor = ActorTryToGet(idPet);
            if (ptPetActor)
            {
                ActorCommonData & tACDPet = ACDGet(idACDPet);
                if (ACDIsHireling(tACDPet) || ACDIsHirelingProxy(tACDPet))
                {
                    if (bDismissFollowers)
                    {
                        NPCHandleDismissal(tACDPet, FALSE, TRUE);
                    }
                    else if (bKillHirelings)
                    {
                        BrainAPIDoDeath(idPet, (ActorID)ACDGetOwner(tPlayer.idPlayerACD), TRUE, TRUE, TRUE, NULL, NULL);
                    }
                }
                else if (ACDIsFollower(tACDPet))
                {
                    if (bDismissFollowers)
                        PlayerRemovePet(tPlayer, idACDPet, TRUE);
                }
                else if (ACDIsFollowerProxy(tACDPet))
                {
                    if (bDismissFollowers)
                        ActorFlagForDeletion(idPet);
                }
                else if (!bEnteringRift || !tACDPet.AttributesGetInt(ATTRIBUTE_PET_DONT_KILL_WHEN_ENTERING_RIFTS))
                {
                    PlayerRemovePet(tPlayer, idACDPet, TRUE);
                    PowerAPI_BuffFlagAllForDeletion(tACDPet);
                    BrainAPIDoDeath(idPet, (ActorID)ACDGetOwner(tPlayer.idPlayerACD), TRUE, TRUE, TRUE, NULL, NULL);
                }
            }
        }
    }
}


































void SPlayerDelete(
    Player & tPlayer)
{
    ActorCommonData * ptACDPlayer = ACDTryToGet(tPlayer.idPlayerACD);

    if (TradeIsTrading(tPlayer.idPlayerACD))
    {
        STradeCancel(tPlayer.idPlayerACD);
    }


    SPlayerRecalculateUpscalingLevels();

    GameCommonData & tGCD = GameGetActiveGameCommonData();
    int32 nCurrentMonsterLevel = tGCD.tGameSyncedData.nMonsterLevel;
    int32 nNewScalingLevel = 1;
    for (Player * ptPlayer = PlayerGetFirstAll(); ptPlayer != NULL; ptPlayer = PlayerGetNextAll(ptPlayer))
    {
        if (ptPlayer->nPlayerIndex == tPlayer.nPlayerIndex)
            continue;



        ActorCommonData * ptACD = ACDTryToGet(ptPlayer->idPlayerACD);
        if (ptACD != NULL)
        {
            nNewScalingLevel = blz::max(nNewScalingLevel, ptACD->AttributesGetInt(ATTRIBUTE_LEVEL));
        }
    }

    if (nNewScalingLevel < nCurrentMonsterLevel)
    {
        SGameUpdateMonsterLevel(nNewScalingLevel, TRUE);
    }





    ServerPlayerGlobals * ptServerPlayerGlobals = SGameManager::GameGet().ptServerPlayerGlobals;
    PerPlayerGlobals & tPerPlayer = ptServerPlayerGlobals->tPerPlayer[tPlayer.nPlayerIndex];

    sTickPlayerAchievementNotifications(tPlayer, tPerPlayer, TRUE);


    SPlayerRemoveFromSaveList(tPlayer);









    uint64 uPlayerGold = ptACDPlayer ? ptACDPlayer->GetCurrencyAmount(CURRENCY_GOLD) : 0;


    tPlayer.bIsBeingDeleted = TRUE;







    if (ptACDPlayer != NULL) { SPlayerSaveCooldowns(*ptACDPlayer, tPlayer); }







    SActTransitionOptInsUpdated();

    int32 nOldNumPlayers = PlayerGetNumPlayersForDifficulty();


    ACDID idACDPlayer = tPlayer.idPlayerACD;


    float flTimePlayed = (float)(GameGetServerTime() - tPlayer.dwJoinTime) / 60.0f;
    SGameStatsPlayerCounterIncrement(&tPlayer, STAT_COUNTER_PLAYER_TIME_PLAYED, 0, 0, flTimePlayed);


    float flItemsEquippedInCombat = SGameStatsPlayerCounterFetch(&tPlayer, STAT_COUNTER_PLAYER_ITEM_EQUIPPED_IN_COMBAT, 0, 0);
    if (flItemsEquippedInCombat / (flTimePlayed / 60.0f) > 1.0f)
    {
        SGameStatsPlayerCounterIncrement(&tPlayer, STAT_COUNTER_PLAYER_ITEM_EQUIPPED_IN_COMBAT_FREQUENTLY, 0, 0, 1.0f);
    }

    SGameStatsDumpPlayerToLog(idACDPlayer);

    PlayerClass ePlayerClass = PlayerGetPlayerClass(&tPlayer);

    SGameManager::GameGet().ptSGameStatGlobals->flTotalPlayTimeByClass[ePlayerClass] += flTimePlayed;

    BossEncounterManager::PlayerRemoved(&tPlayer, TRUE);
    NephalemRiftManager::PlayerRemoved(&tPlayer, TRUE);
    SetEncounterManager::PlayerRemoved(&tPlayer, TRUE);
    Minimap::OnRemovedPlayer(tPlayer);
    VoteKick::Server::OnRemovedPlayer(tPlayer);

    SGameGlobals & tSGameGlobals = SGameGlobalsGet();


    if (tPlayer.uHeroId == tSGameGlobals.uHeroIdCreator)
    {
        if (SQuestStateIsExploitable())
        {
            tSGameGlobals.dwFlags2 |= 0x40;
        }
    }


    if (ptACDPlayer != NULL && ptACDPlayer->IsInWorld())
    {
        if (ptACDPlayer->IsLoading())
        {

            SPlayer_ClientLoadComplete(tPlayer);
        }


        DeleteLastHeadstone(*ptACDPlayer);




        if (TradeIsTrading(idACDPlayer))
        {
            STradeCancel(idACDPlayer);
        }


        ConversationMgr::PlayerOptOutOfAllConversations(tPlayer, NULL);


        STalkResetTalkingTo(tPlayer);


        TownPortalsDeleteForPlayer(tPlayer);


        SQuestsPlayerLeaveGame(tPlayer);

        ScriptEventOnPlayerLeftGame(tPlayer.idPlayerACD);


        SGameStatsRecordPlayerStats(tPlayer);


        if (ptACDPlayer->IsDueling())
        {
            PVPDuelingHandlePlayerDelete(idACDPlayer);
        }


        SPlayerKillAllPets(tPlayer, TRUE, TRUE, TRUE, FALSE);


        SPlayerClearEnemyList(tPlayer);


        HirelingStableDispose(tPlayer.tHirelingStable);


        PlayerClearKnownList(tPlayer);

        SPlayerTrickleClearAll(tPlayer);


        for (int32 i = 0; i < 4; i++)
        {
            tPlayer.flPVPPlayerDamageTaken[i] = 0.0f;
        }


        tPlayer.idACDStashLastOperated = ACDID_INVALID;
        tPlayer.idACDActiveChallengePortal = ACDID_INVALID;
        tPlayer.bChallengeAccepted = FALSE;

        tPlayer.dwTickLastVerification = 0;
        tPlayer.tPlaceLastVerification = cgplaceNull;
        tPlayer.flMaxSpeedThisVerificationInterval = 0.0f;

        tPlayer.nMessageCostLast300 = tPlayer.nMessageCostLast600 = tPlayer.nMessageCostLast3600 = tPlayer.nMessageCostLastUpdateTick = tPlayer.nMessageCostMax300 = tPlayer.nMessageCostMax600 = tPlayer.nMessageCostMax3600 = 0;








        SNO snoNephalemBuffPower = GlobalSNOGet(GLOBALSNO_POWERS_KILLELITEPACK);
        uint32 nNumNephalemStacks = ptACDPlayer->AttributesGetInt(MakeAttribKey(ATTRIBUTE_SKILL, snoNephalemBuffPower));


        uint32 nNumDungeonFinderProgress = 0;

        uint32 dwNow = GameGetServerTime();
        BuffID idBuff = PowerAPI_BuffFindOnACD(snoNephalemBuffPower, ptACDPlayer, ACDID_INVALID);
        uint32 dwTimeRemaining = 0;
        if (idBuff != BUFFID_INVALID)
        {
            PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
            if (ptBuff != NULL)
            {
                dwTimeRemaining = ptBuff->dwDuration - (dwNow - ptBuff->dwStartTick);
            }
        }

        if (nNumNephalemStacks != 0 && dwTimeRemaining != 0)
        {
            SessionTrackedData tData;
            tData.dwLastPlayedAmount = tPlayer.tHeroStateData.tSavedData.uSecondsPlayed;
            tData.uHeroId = tPlayer.uHeroId;
            tData.nNumNephalemStacks = nNumNephalemStacks;
            tData.dwNephalemBuffDurationRemaining = dwTimeRemaining;
            tData.nNumDungeonFinderProgress = nNumDungeonFinderProgress;

            ptServerPlayerGlobals->tPreviousSessions.Enqueue(tData);
        }


        POS posBuff = ptACDPlayer->listBuffs.GetHeadPosition();

        while (posBuff)
        {
            idBuff = ptACDPlayer->listBuffs.GetNext(posBuff);

            PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
            if (ptBuff != NULL
                && !ptBuff->bDeleted
                && !ptBuff->bKeepForProfileSnapshot)
            {
                PowerAPI_BuffFlagForDeletion(ptBuff->id, FALSE, TRUE, FALSE, TRUE);
            }
        }

        Player * ptPlayer = PlayerGetByACD(idACDPlayer);
        if (ptPlayer != NULL)
        {
            if (!NephalemRiftManager::PlayersAreStillChoosingRewards(ptPlayer))
            {
                NephalemRiftManager::SendPlayersDoneChoosingRewardsEvent(ptPlayer->idPlayerACD);
            }

            if (NephalemRiftManager::IsTieredRiftOrphaned(ptPlayer))
            {
                NephalemRiftManager::SendTieredRiftOrphanedEvent(ptPlayer->idPlayerACD);
            }

            if (NephalemRiftManager::PlayerPaidForRift(&tPlayer) &&
                NephalemRiftManager::GetActiveRiftLevel() >= 0)
            {
                NephalemRiftManager::PlayerSetLeaverHash(&tPlayer);
            }
        }
    }


































































































    tSGameGlobals.tMatchmakingData.dwLastPlayerLeaveTick = GameGetServerTime();

    {
        Achievement::EvaluationData tEvaluationData;
        tEvaluationData.SetPlayer(&tPlayer);
        tEvaluationData.EvaluateEvent(&tPlayer, Achievement::ACH_EVENT_PLAYER_LEAVE_GAME, 1, (uint32)sub_71007b5e70());




        if (tSGameGlobals.ptAchievementsInterface != NULL)
        {
            tSGameGlobals.ptAchievementsInterface->CancelSubscription(
                tPlayer.tAccount.tHandle,
                OnlineService::AchievementsInterface::RegistrationCallback());
        }
    }

















    SGameManager::ClearAccountSGameId(tPlayer.tAccount.tHandle);

    if (ptACDPlayer != NULL)
    {

        SItemsDestroyLockedOwned(*ptACDPlayer);


        ACDInventoryDeleteMail(*ptACDPlayer);
    }

    TickRateTracker & tTickRateTracker = SPlayerGetTickRateTracker(tPlayer);
    tTickRateTracker.Initialize();


    SGameStatsClearForPlayer(tPlayer);

    SPlayerUnregisterExperienceId(tPlayer);


    STeamRemovePlayer(tPlayer);


    PlayerSetDefaults(tPlayer);


    ActorID idActorPlayer = (ActorID)ACDGetOwner(idACDPlayer);
    ActorDelete(idActorPlayer);





    int32 nNewNumPlayers = PlayerGetNumPlayersForDifficulty();
    ActorsAdjustForPlayerChanged(nOldNumPlayers, nNewNumPlayers);


    SHirelingRehireAfterPlayerLeft();








    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetAllConnected(listDestPlayers, -1, 0);

    PlayerIndexMessage tMessage;
    tMessage.nPlayerIndex = tPlayer.nPlayerIndex;

    MessageSendToClient(listDestPlayers, SMSG_PLAYER_DELETE, &tMessage, sizeof(tMessage));






    SPlayerEvaluateMultiplayerBuff();


    SPlayerBootAvenger(tPlayer);






    if (tPlayer.uHeroId == tSGameGlobals.uHeroIdCreator)
    {
        tSGameGlobals.uHeroIdCreator = OnlineService::INVALID_HERO_ID;
    }


    if (tSGameGlobals.dwFlags2 & 0x40)
    {


        QuitGameMessage tQuitMessage;
        tQuitMessage.eReason = QUIT_REASON_GAME_HOST_LEFT_NEWLY_CREATED_GAME;
        PlayerList listDestPlayers2(PlayerListGetElementPool());
        PlayerListGetAllInGame(listDestPlayers2, tPlayer.nPlayerIndex, 0, -1);
        MessageSendToClient(listDestPlayers2, SMSG_QUIT, &tQuitMessage, sizeof(tQuitMessage));


        SGameID idSGame = SGameManager::GetCurrentGame();

        tSGameGlobals.dwFlags |= 4;
    }


    if (tPlayer.idTimerCorpseResurrectGrantCharge != TIMERID_INVALID)
    {
        TimerUnregister(tPlayer.idTimerCorpseResurrectGrantCharge);
        tPlayer.idTimerCorpseResurrectGrantCharge = TIMERID_INVALID;
    }
}



BOOL SPlayerFindGoodSpawnLocation(
    const WorldPlace & placeStart,
    WorldPlace & placeResult,
    float flSearchRadius)
{
    NavMeshHandle tGoodNavMesh;
    PassabilitySettings tPassability;
    PassabilitySettingsInit(tPassability);
    tPassability.ePassabilityType = PASSABILITY_MONSTER;
    BOOL bFoundGoodLoc = TRUE;
    if (!NavMeshSearchForLocation(placeStart, tPassability, 0.0f, flSearchRadius, 0, tGoodNavMesh, placeResult, TRUE, 10, 0.0f))
    {
        placeResult = placeStart;
        bFoundGoodLoc = FALSE;
    }
    return bFoundGoodLoc;
}














































ACDID PlayerActorCreate(
    Player & tPlayer,
    const uint32 dwPlayerFlags,
    const WorldPlace & tPlace,
    const Quaternion & q,
    const BOOL bResetLifeStats,
    const D3::Hero::SavedDefinition & tHeroDefinition,
    const D3::Account::SavedDefinition & tAccountDefinition,
    const D3::Items::Mails & tMails,
    const uint32 dwSaveOptionBits)
{
    SHero::CreateActor(dwPlayerFlags, tPlace, q, tPlayer, tHeroDefinition, tAccountDefinition, tMails, dwSaveOptionBits);









    if (bResetLifeStats)
    {
        ActorCommonData & tACDPlayer = ACDGet(tPlayer.idPlayerACD);
        tACDPlayer.ResetLifeStats(FALSE, TRUE, TRUE, TRUE, FALSE);
        tACDPlayer.ResetRestoredPlayerStats();
    }

    STrickleOnPlayerCreated(tPlayer);

    if (tPlayer.bForceHirelingItemSave)
    {
        ActorCommonData & tACDPlayer = ACDGet(tPlayer.idPlayerACD);
        for (int32 i = HC_FIRST; i < NUM_HIRELING_CLASSES; i++)
        {
            if (HirelingTestTypeAvailable(tPlayer.idPlayerACD, i))
            {
                ACDID idACDHireling = SHirelingCreate(tACDPlayer, tACDPlayer, i, TRUE, HT_PROXY);
                ActorCommonData * ptACDHireling = ACDTryToGet(idACDHireling);
                if (ptACDHireling)
                {
                    for (int32 nSlot = SLOT_HIRELING_START; nSlot < SLOT_HIRELING_END; nSlot++)
                    {
                        InventorySlotType eSlot = (InventorySlotType)nSlot;
                        ACDID idACDItem = ACDInventoryGetSquareItem(*ptACDHireling, eSlot, cgptNull);
                        ActorCommonData * ptACDItem = ACDTryToGet(idACDItem);
                        if (ptACDItem)
                        {



                            SACDInventoryAddToDirtyList(*ptACDItem, AIDA_UPDATE_GEM_REMOVED, eSlot, eSlot);
                        }
                    }

                    NPCDismissProxy(tACDPlayer, i);
                }
            }
        }

        tPlayer.bForceHirelingItemSave = FALSE;
    }






    ActorCommonData & tACDPlayer = ACDGet(tPlayer.idPlayerACD);
    BOOL bActiveBuffs = SPlayerApplyConsumablePotionBuffs(tACDPlayer);
    if (bActiveBuffs || tPlayer.tCosmeticItemSavedData.tCosmeticItemData.UpdateLimitedDurationPets(tACDPlayer) || GameGetDailyPlatinumLimit() != 0)
    {
        sSetLimitedDurationItemPollTime(tACDPlayer);
    }
    return tPlayer.idPlayerACD;
}




void PlayerSendHeroStateToClient(
    const ACDID idACD)
{
    Player * ptPlayer = PlayerGetByACD(idACD);
    HeroStateMessage tMessage;
    SigmaMemoryCopy(&tMessage.tStateData, &ptPlayer->tHeroStateData, sizeof(tMessage.tStateData));
    tMessage.nPlayerIndex = ptPlayer->nPlayerIndex;
    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetSingle(listDestPlayers, *ptPlayer);
    MessageSendToClient(listDestPlayers, SMSG_HERO_STATE, &tMessage, sizeof(tMessage));
    PlayerSendTutorialStashIconsStateToClient(ptPlayer, listDestPlayers);
}










void PlayerSendTutorialStashIconsStateToClient(
    const Player * ptPlayer,
    const PlayerList & listDestPlayers)
{
    StashIconStateMessage tStashIconMessage;

    if (ptPlayer->tSeenTutorials.GetCountOfBitsSet())
    {
        D3::GameMessage::TutorialState tTutorialStateMessage;
        tTutorialStateMessage.set_seen_tutorials(ptPlayer->tSeenTutorials.GetRawBuffer(), ptPlayer->tSeenTutorials.GetLength());
        MessageSendToClient(listDestPlayers, SMSG_TUTORIAL_STATE, tTutorialStateMessage);
    }

    SigmaMemoryCopy(tStashIconMessage.bStashIcons, ptPlayer->bStashIcons, sizeof(tStashIconMessage.bStashIcons));
    MessageSendToClient(listDestPlayers, SMSG_STASH_ICON_STATE, &tStashIconMessage, sizeof(tStashIconMessage));
}




void PlayerSendHeroStateSavedDataToClient(
    const ACDID idACD)
{
    Player * ptPlayer = PlayerGetByACD(idACD);
    PlayerSavedDataMessage tMessage;
    tMessage.tSavedData = ptPlayer->tHeroStateData.tSavedData;
    tMessage.nPlayerIndex = ptPlayer->nPlayerIndex;
    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetSingle(listDestPlayers, *ptPlayer);
    MessageSendToClient(listDestPlayers, SMSG_HERO_STATE_SAVEDDATA, &tMessage, sizeof(tMessage));
}








void PlayerSendHeroStateSkillsToAllOtherClients(
    const ACDID idACD)
{
    Player * ptPlayer = PlayerGetByACD(idACD);
    PlayerSkillsMessage tMessage;
    SigmaMemoryCopy(tMessage.snoTraits, ptPlayer->tHeroStateData.tSavedData.snoTraits, sizeof(tMessage.snoTraits));
    SigmaMemoryCopy(tMessage.tActiveSkills, ptPlayer->tHeroStateData.tSavedData.tActiveSkills, sizeof(tMessage.tActiveSkills));
    SigmaMemoryCopy(tMessage.gbidLegendaryPowers, ptPlayer->tHeroStateData.tSavedData.gbidLegendaryPowers, sizeof(tMessage.gbidLegendaryPowers));
    tMessage.nPlayerIndex = ptPlayer->nPlayerIndex;
    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetAllInGame(listDestPlayers, ptPlayer->nPlayerIndex, FALSE, -1);
    MessageSendToClient(listDestPlayers, SMSG_HERO_STATE_SKILLS, &tMessage, sizeof(tMessage));
}








void PlayerSendHeroStatePlayerFlagsToClient(
    const ACDID idACD)
{
    Player * ptPlayer = PlayerGetByACD(idACD);
    PlayerDWordDataMessage tMessage;
    tMessage.dwData = ptPlayer->tHeroStateData.uPlayerFlags;
    tMessage.nPlayerIndex = ptPlayer->nPlayerIndex;
    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetSingle(listDestPlayers, *ptPlayer);
    MessageSendToClient(listDestPlayers, SMSG_HERO_STATE_PLAYERFLAGS, &tMessage, sizeof(tMessage));
}








void PlayerSendHeroStateWaypointsToClient(
    const ACDID idACD)
{
    Player * ptPlayer = PlayerGetByACD(idACD);
    PlayerDWordDataMessage tMessage;
    tMessage.dwData = ptPlayer->tHeroStateData.tSavedData.m_ActivatedWaypoints;
    tMessage.nPlayerIndex = ptPlayer->nPlayerIndex;
    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetSingle(listDestPlayers, *ptPlayer);
    MessageSendToClient(listDestPlayers, SMSG_HERO_STATE_WAYPOINTS, &tMessage, sizeof(tMessage));
}








void PlayerSendHeroStateLastSavePointToClient(
    const ACDID idACD)
{
    Player * ptPlayer = PlayerGetByACD(idACD);
    SavePointDataMessage tMessage;
    SigmaMemoryCopy(&tMessage.tData, &ptPlayer->tHeroStateData.tSavedData.tLastSavePoint, sizeof(tMessage.tData));
    tMessage.nPlayerIndex = ptPlayer->nPlayerIndex;
    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetSingle(listDestPlayers, *ptPlayer);
    MessageSendToClient(listDestPlayers, SMSG_HERO_STATE_LASTSAVEPOINT, &tMessage, sizeof(tMessage));
}









void PlayerSendHeroStateLastWaypointLevelAreaToAllClients(
    const ACDID idACD)
{
    Player * ptPlayer = PlayerGetByACD(idACD);
    LastWaypointLevelAreaDataMessage tMessage;
    tMessage.nPlayerIndex = ptPlayer->nPlayerIndex;
    tMessage.snoLevelArea = PlayerGetLastWaypointLevelArea(ptPlayer);
    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetAllInGame(listDestPlayers, -1, FALSE, -1);
    MessageSendToClient(listDestPlayers, SMSG_HERO_STATE_LASTWAYPOINTLEVELAREA, &tMessage, sizeof(tMessage));
}








void PlayerSendHeroStateCurrencyToClient(
    const ACDID idACD,
    BOOL bOnlySendDirtyCurrencies)
{
    Player * ptPlayer = PlayerGetByACD(idACD);
    PlayerSendHeroStateCurrencyToClient(*ptPlayer, bOnlySendDirtyCurrencies);
}





void PlayerSendHeroStateCurrencyToClient(
    Player & tPlayer,
    BOOL bOnlySendDirtyCurrencies)
{
    D3::Items::CurrencySavedData tCurrencySavedData;
    CurrencySavedData::ExportDirtyScope eDirtyScope = bOnlySendDirtyCurrencies ? CurrencySavedData::EXPORT_DIRTY_CLIENTSYNC : CurrencySavedData::EXPORT_DIRTY_IGNORE;
    tPlayer.tCurrencySavedData.Export(tCurrencySavedData, CurrencySavedData::EXPORT_PARTITION_IGNORE, eDirtyScope, FALSE, 2);
    PlayerList listPlayers(PlayerListGetElementPool());
    PlayerListGetSingle(listPlayers, tPlayer);
    MessageSendToClient(listPlayers, bOnlySendDirtyCurrencies ? SMSG_CURRENCY_DATA_PARTIAL : SMSG_CURRENCY_DATA_FULL, tCurrencySavedData);
}










void PlayerSendHeroStateLoadoutToClient(
    Player & tPlayer,
    int32 nLoadoutIndex,
    BOOL bOnSave)
{
    if (nLoadoutIndex < 0 || nLoadoutIndex >= 10)
        return;



    PlayerList listPlayers(PlayerListGetElementPool());
    PlayerListGetSingle(listPlayers, tPlayer);

    const LoadoutSavedData & tLoadout = tPlayer.tLoadouts[nLoadoutIndex];
    if (tLoadout.bEmpty)
    {
        PlayerLoadoutTabIconMessage tMessage;
        tMessage.nLoadoutIndex = nLoadoutIndex;
        tMessage.nTabIcon = tLoadout.nTabIcon;
        MessageSendToClient(listPlayers, SMSG_LOADOUT_EMPTY, &tMessage, sizeof(tMessage));
        return;
    }

    PlayerLoadoutDataMessage tMessage;
    tMessage.nLoadoutIndex = nLoadoutIndex;
    SigmaMemoryCopy(tMessage.tEquipment, tLoadout.tEquipment, sizeof(tMessage.tEquipment));
    SigmaMemoryCopy(tMessage.tActiveSkills, tLoadout.tActiveSkills, sizeof(tMessage.tActiveSkills));
    SigmaMemoryCopy(tMessage.snoTraits, tLoadout.snoTraits, sizeof(tMessage.snoTraits));
    SigmaMemoryCopy(tMessage.gbidLegendaryPowers, tLoadout.gbidLegendaryPowers, sizeof(tMessage.gbidLegendaryPowers));
    SigmaMemoryCopy(tMessage.usName, tLoadout.usName, sizeof(tMessage.usName));
    tMessage.tEquippedPotion = tLoadout.tEquippedPotion;
    tMessage.nTabIcon = tLoadout.nTabIcon;

    MessageSendToClient(listPlayers, bOnSave ? SMSG_LOADOUT_SAVED : SMSG_LOADOUT_DATA, &tMessage, sizeof(tMessage));
}




void PlayerSendHeroStateAllLoadoutsToClient(
    Player & tPlayer)
{
    for (int32 i = 0; i < 10; i++)
        PlayerSendHeroStateLoadoutToClient(tPlayer, i, FALSE);
}






BOOL PlayerCheckAndUpdateDailyFirst(
    const Player & tPlayer,
    FastAttribKey tKey)
{
    BOOL bRet = FALSE;
    if (GameGetDailyPlatinumLimit() != 0 && !GameIsWeeklyChallenge())
    {
        bRet = PlayerEligibleForDailyFirst(tPlayer, tKey);
        if (bRet)
        {
            ActorCommonData & tACDPlayer = ACDGet(tPlayer.idPlayerACD);
            uint32 nNow = Blizzard::Time::ToUnixTime(OnlineServiceServer::GetCurrentTime());
            tACDPlayer.AttributesSetInt(tKey, nNow);
        }
    }
    return bRet;
}










































































































uint64 PlayerSubtractFromDailyPlatinum(
    Player & tPlayer,
    uint64 uAmount)
{
    if (uAmount == 0)
        return 0;




    ActorCommonData & tACDPlayer = ACDGet(tPlayer.idPlayerACD);
    if (PlayerCheckAndUpdateDailyFirst(tPlayer, ATTRIBUTE_PLATINUM_CAP_LAST_GAIN))
        tACDPlayer.AttributesSetInt(ATTRIBUTE_PLATINUM_CAP_REMAINING, (int32)GameGetDailyPlatinumLimit());

    uint64 uAmountRemaining = tACDPlayer.AttributesGetInt(ATTRIBUTE_PLATINUM_CAP_REMAINING);
    uint64 uDelta = Math::Min(uAmountRemaining, uAmount);
    if (uDelta != 0)
        tACDPlayer.AttributesSetInt(ATTRIBUTE_PLATINUM_CAP_REMAINING, (int32)(uAmountRemaining - uDelta));
    return uDelta;
}











































void PlayerSendHeroStateExtractedLegendariesToClient(
    const Player & tPlayer)
{
    D3::GameBalance::BitPackedGbidArray tExtractedLegendariesData;
    tPlayer.tCrafterSavedData.tExtractedLegendariesData.Export(tExtractedLegendariesData);
    PlayerList listPlayers(PlayerListGetElementPool());
    PlayerListGetSingle(listPlayers, tPlayer);
    MessageSendToClient(listPlayers, SMSG_EXTRACTED_LEGENDARIES_DATA_FULL, tExtractedLegendariesData);
}





void PlayerAskClientToLoadLegendaryPowerIcons(
    const Player & tPlayer)
{
    SimpleMessage tMessage;
    PlayerList listPlayers(PlayerListGetElementPool());
    PlayerListGetSingle(listPlayers, tPlayer);
    MessageSendToClient(listPlayers, SMSG_LOAD_LEGENDARY_POWER_ICONS, &tMessage, sizeof(tMessage));
}





void SPlayerSendNewPlayerMessage(
    const PlayerList & listDestPlayers,
    const Player & tPlayer,
    BOOL bJustJoined)
{

    NewPlayerMessage tMessage;
    tMessage.uHeroId = tPlayer.uHeroId;
    tMessage.tAccountId = tPlayer.tAccount.tHandle;
    Blizzard::String::Copy(tMessage.usHeroName, tPlayer.usHeroName, sizeof(tMessage.usHeroName));
    tMessage.nPlayerIndex = tPlayer.nPlayerIndex;
    tMessage.eTeam = tPlayer.eTeam;

    ActorCommonData & tACDPlayer = ACDGet(tPlayer.idPlayerACD);


    Blizzard::String::Copy(tMessage.usGamerTag, tPlayer.tAccount.usName, sizeof(tMessage.usGamerTag));
    tMessage.uAcceptedLicenseBitsHero = tPlayer.uAcceptedLicenseBitsHero;
    tMessage.bUpscaled = tACDPlayer.AttributesGetInt(FastAttribKey(ATTRIBUTE_UPSCALED_LEVEL)) > 0;
    tMessage.uPartitionFlags = tPlayer.tAccount.uPartitionFlags;
    tMessage.uSeasonCreated = tPlayer.uSeasonCreated;


    tMessage.eClass = PlayerGetPlayerClass(tPlayer.idPlayerACD);

    tMessage.snoActorPortrait = tACDPlayer.snoActor;
    tMessage.nCurrentLevel = tACDPlayer.AttributesGetInt(FastAttribKey(ATTRIBUTE_LEVEL));
    tMessage.nCurrentAltLevel = tACDPlayer.AttributesGetInt(FastAttribKey(ATTRIBUTE_ALT_LEVEL));
    tMessage.nCurrentHighestHeroSoloRiftLevel = tACDPlayer.AttributesGetInt(FastAttribKey(ATTRIBUTE_HIGHEST_HERO_SOLO_RIFT_LEVEL));

    tMessage.tHeroStateData = tPlayer.tHeroStateData;
    tMessage.bJustJoined = bJustJoined;
    tMessage.ann = ANNFromACD(tPlayer.idPlayerACD);
    tMessage.idGameConnection = tPlayer.idGameConnection;


    tMessage.nNFPDefaultScanTime = tPlayer.tNFPDefaultScanTime;
    tMessage.nNFPGoblinScanTime = tPlayer.tNFPGoblinScanTime;


    MessageSendToClient(listDestPlayers, SMSG_NEW_PLAYER, &tMessage, sizeof(tMessage));


    SPlayerSendBanner(listDestPlayers, tPlayer);
}



void SPlayerSendBanner(
    const PlayerList & listDestPlayers,
    const Player & tPlayer)
{
    D3::GameMessage::PlayerBanner tBannerMessage;
    tBannerMessage.set_player_index(tPlayer.nPlayerIndex);
    tPlayer.tAccount.tBanner.Export(*tBannerMessage.mutable_banner());
    MessageSendToClient(listDestPlayers, SMSG_PLAYER_BANNER, tBannerMessage);
}






void SPlayerUnlockAdventureMode(
    Player & tPlayer)
{
    const OnlineService::ContentLicense * ptLicense = SPlayerGetContentLicense(tPlayer, OnlineService::CONTENT_LICENSE_ADVMODE_ACTV);
    if (ptLicense == NULL || ptLicense->IsExpired())
        return;

    BOOL bUnlocked = FALSE;
    if (!(tPlayer.tHeroStateData.uPlayerFlags & (1 << PLAYERFLAG_HARDCORE)) &&
        !(tPlayer.tAccount.uDigestFlags & (1 << D3::Account::Digest_Flags_ADVENTURE_MODE_UNLOCKED_DEPRECATED)))
    {
        tPlayer.tAccount.uDigestFlags |= (1 << D3::Account::Digest_Flags_ADVENTURE_MODE_UNLOCKED_DEPRECATED);
        SPlayerAddToSaveList(tPlayer, 0, PlayerSaveReason_Adventure, PLAYER_SAVE_MODE_END_OF_FRAME);
        bUnlocked = TRUE;
    }
    if ((tPlayer.tHeroStateData.uPlayerFlags & (1 << PLAYERFLAG_HARDCORE)) &&
        !(tPlayer.tAccount.uDigestFlags & (1 << D3::Account::Digest_Flags_HARDCORE_ADVENTURE_MODE_UNLOCKED_DEPRECATED)))
    {
        tPlayer.tAccount.uDigestFlags |= (1 << D3::Account::Digest_Flags_HARDCORE_ADVENTURE_MODE_UNLOCKED_DEPRECATED);
        SPlayerAddToSaveList(tPlayer, 0, PlayerSaveReason_Adventure, PLAYER_SAVE_MODE_END_OF_FRAME);
        bUnlocked = TRUE;
    }
    if (bUnlocked)
    {
        PlayerList listDestPlayers(PlayerListGetElementPool());
        PlayerListGetSingle(listDestPlayers, tPlayer);
        SimpleMessage tMessage;
        MessageSendToClient(listDestPlayers, SMSG_ADVENTURE_MODE_UNLOCKED, &tMessage, sizeof(tMessage));
    }
}



















































































































int32 SPlayerAddPet(
    ACDID idACDPlayer,
    PetType eType,
    ACDID idACDPet,
    BOOL fSendNow,
    int32 nMaxCount,
    PetComparisonValueFunc pCompareFunc)
{
    Player * ptPlayer = PlayerGetByACD(idACDPlayer);
    if (ptPlayer == NULL)
    {
        DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }



    return SPlayerAddPet(*ptPlayer, eType, idACDPet, fSendNow, nMaxCount, pCompareFunc, FALSE);
}


int32 SPlayerAddPet(
    Player & tPlayer,
    PetType eType,
    ACDID idACDPet,
    BOOL fSendNow,
    int32 nMaxCount,
    PetComparisonValueFunc pCompareFunc,
    BOOL bIgnoreFade)
{
    ActorCommonData * ptACDPlayer = ACDTryToGet(tPlayer.idPlayerACD);
    if (!ptACDPlayer)
    {
        return -1;
    }


    if (tPlayer.bIsBeingDeleted)
    {
        return -1;
    }

    ActorCommonData & tACDPet = ACDGet(idACDPet);



    if (tACDPet.GetPetOwnerPlayer())
    {
        return -1;
    }





    int32 nTotalPets = PlayerGetPetCount(tPlayer.idPlayerACD);
    int32 nTotalPetsOfType = PlayerGetNumPetsOfType(tPlayer.idPlayerACD, eType);
    int32 nSanityCount = 0;
    while ((nMaxCount > 0 && nTotalPetsOfType >= nMaxCount) || nTotalPets >= MAX_PETS)
    {
        ACDID idACDPetToKill = ACDID_INVALID;
        PetType eTypeOfPetToKill = eType;
        if (pCompareFunc)
        {
            float flWinningValue = FLT_MAX;
            for (int32 i = 0; i <= tPlayer.nMaxUsedPetIndex; i++)
            {
                PlayerPet & tPet = tPlayer.tPets[i];

                if (tPet.eType == eType)
                {
                    ActorID idActorPet = (ActorID)ACDGetOwner(tPet.idACD);
                    if (!ActorFlagIsSet(idActorPet, ACTOR_DYING))
                    {



                        float flCurrValue =


                            pCompareFunc(tPet.idACD);

                        if (flCurrValue < flWinningValue)
                        {
                            flWinningValue = flCurrValue;
                            idACDPetToKill = tPet.idACD;
                        }
                    }
                }
            }
        }
        if (nTotalPets >= MAX_PETS && idACDPetToKill == ACDID_INVALID)
        {

            for (int32 i = 0; i <= tPlayer.nMaxUsedPetIndex; i++)
            {
                PlayerPet & tPet = tPlayer.tPets[i];
                if (tPet.eType != PETTYPE_HIRELING && tPet.eType != PETTYPE_FOLLOWER &&
                    tPet.eType != PETTYPE_HIRELING_PROXY && tPet.eType != PETTYPE_FOLLOWER_PROXY)
                {
                    eTypeOfPetToKill = tPet.eType;

                    idACDPetToKill = tPet.idACD;
                    break;
                }
            }
        }






        if (idACDPetToKill != ACDID_INVALID)
        {
            ActorID idOldPet = (ActorID)ACDGetOwner(idACDPetToKill);




            if (eTypeOfPetToKill == PETTYPE_WITCHDOCTOR_WICKERMAN)
            {
                PowerAPI_BuffFlagAllForDeletion(ACDGet(idACDPetToKill));
            }


            BrainAPIDoDeath(idOldPet, (ActorID)ACDGetOwner(tPlayer.idPlayerACD), TRUE, TRUE, TRUE, NULL, NULL);
        }







        else if (!pCompareFunc && nMaxCount >= 0 && nTotalPetsOfType >= nMaxCount)
        {
            return -1;
        }










        nTotalPets = PlayerGetPetCount(tPlayer.idPlayerACD);
        nTotalPetsOfType = PlayerGetNumPetsOfType(tPlayer.idPlayerACD, eType);

        nSanityCount++;


        if (nSanityCount >= 1000)
        {
            for (int32 i = 0; i <= tPlayer.nMaxUsedPetIndex; i++)
            {
                ACDID idACDPetLoc = tPlayer.tPets[i].idACD;
                ActorCommonData * ptACDPet = ACDTryToGet(idACDPetLoc);

            }
        }
    }



    if (nMaxCount > 0)
    {
        nTotalPetsOfType = PlayerGetNumPetsOfType(tPlayer.idPlayerACD, eType);
        if (nTotalPetsOfType >= nMaxCount)
        {
            return -1;
        }
    }




    PlayerPet * ptPlayerPet = NULL;

    int32 nIndex = PlayerGetEmptyPetSlot(tPlayer, idACDPet, &ptPlayerPet);
    if (!ptPlayerPet)
        return -1;

    PlayerPetInit(*ptPlayerPet, eType, tACDPet, tPlayer);


    if (fSendNow)
    {

        PlayerList listDestPlayer(PlayerListGetElementPool());
        PlayerListGetThatKnowACD(listDestPlayer, idACDPet, FALSE);


        ServerMessageSendAddPet(listDestPlayer, idACDPet);
    }


    if (eType == PETTYPE_HIRELING)
    {
        HirelingClass eHirelingClass = HirelingGetClass(tACDPet);
        SHirelingSetActive(tPlayer.idPlayerACD, eHirelingClass, TRUE);


        ActorRefreshHirelingKnownActors(eHirelingClass);

        SGameStatsPlayerTimedEventStart(&tPlayer, STAT_COUNTER_HIRELING_UPTIME, eHirelingClass, 0, TRUE);
    }











    if (Achievement::IsPetTypeEvaluable(eType))
    {
        Achievement::EvaluationData tEvaluationData;
        tEvaluationData.SetPlayer(&tPlayer);
        tEvaluationData.SetACD(&tACDPet);

        tEvaluationData.EvaluateEvent(&tPlayer, Achievement::ACH_EVENT_PET_ACQUIRED, eType, 1, (uint32)sub_71007b5e70());
    }



    PowerAPI_BuffApplyOnPetAddEffects(tPlayer.idPlayerACD, tACDPet.id, eType, tACDPet.GetActorSNO());


    if (eType != PETTYPE_FOLLOWER && !bIgnoreFade)
    {
        tACDPet.AttributesSetInt(FastAttribKey(ATTRIBUTE_ALPHA_FOR_OTHER_PLAYERS), tPlayer.idPlayerACD);
    }












    return nIndex;
}




void SPlayerKillPetsFromPowerSNO(
    const ACDID idACDPlayer,
    const SNO snoPower,
    const BOOL bEnteringRift)
{
    const Player * ptPlayer = PlayerGetByACD(idACDPlayer);
    if (!ptPlayer)
    {
        return;
    }

    const ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
    if (snoPower == SNO_NONE || !ptACDPlayer)
    {
        return;
    }







    SNO snoSynergyPower = PowerGeneralTagGetSNO(AutoPowerDefinitionR(snoPower), TAG_POWER_SYNERGY_POWER);

    int32 nPos;
    ACDID idACDPet = PlayerGetFirstPet(ptACDPlayer->id, -1, nPos);
    while (idACDPet != ACDID_INVALID)
    {

        ActorCommonData * ptACDPet = ACDTryToGet(idACDPet);
        if (ptACDPet)
        {
            SNO snoSummonedBySno = ptACDPet->AttributesGetInt(ATTRIBUTE_SUMMONED_BY_SNO);
            if (snoSummonedBySno != SNO_NONE)
            {
                ActorID idPet = (ActorID)ACDGetOwner(ptACDPet->id);

                if (!bEnteringRift || !ptACDPet->AttributesGetInt(ATTRIBUTE_PET_DONT_KILL_WHEN_ENTERING_RIFTS))
                {
                    if (snoSummonedBySno == snoPower || snoSummonedBySno == snoSynergyPower)
                    {

                        BrainAPIDoDeath(idPet, (ActorID)ACDGetOwner(ptACDPlayer->id), TRUE, TRUE, TRUE, NULL, NULL);
                    }
                    else
                    {


                        SNO snoBaseZombieDogPower = GlobalSNOGet(GLOBALSNO_POWERS_WITCHDOCTOR_SUMMON_ZOMBIEDOG);
                        SNO snoPassiveZombieDogSpawner = GlobalSNOGet(GLOBALSNO_POWERS_WITCHDOCTOR_SUMMON_ZOMBIEDOG_PASSIVE_SPAWNER);
                        if (snoBaseZombieDogPower == snoPower && snoSummonedBySno == snoPassiveZombieDogSpawner)
                        {


                            PowerAPI_BuffFlagAllForDeletion(*ptACDPet);
                            BrainAPIDoDeath(idPet, (ActorID)ACDGetOwner(ptACDPlayer->id), TRUE, TRUE, TRUE, NULL, NULL);
                        }
                    }
                }
            }
        }


        idACDPet = PlayerGetNextPet(ptACDPlayer->id, -1, nPos);
    }
}



void SPlayerRemoveBuffsAndPetsForRespec(
    ActorCommonData & tACDPlayer,
    const SNO snoPower)
{
    if (snoPower != SNO_NONE)
    {
        PowerAPI_BuffApplyOnSkillChangeEffects(tACDPlayer.id);
        ActorPowersRemoveBuffsForRespec(tACDPlayer, snoPower);
        SPlayerKillPetsFromPowerSNO(tACDPlayer.id, snoPower, FALSE);
    }

    ActorCommonData * ptACD = ACDTryToGet(tACDPlayer.id);
    ActorID idActor = ptACD ? (ActorID)ptACD->idOwner : ACTORID_INVALID;
    SNO snoPrimaryPower = ActorPowersGetPrimaryPower(idActor);
    if (snoPrimaryPower != SNO_NONE && snoPrimaryPower == snoPower)
    {
        if (PowerIsChannelled(tACDPlayer, AutoPowerDefinitionR(snoPower)))
            ActorPowersInterruptPrimary(idActor, TRUE);
    }
}






void SPlayerAddFollower(
    ActorCommonData & tACDPet,
    Player & tPlayer)
{

    SPlayerAddPet(tPlayer, PETTYPE_FOLLOWER, tACDPet.id, TRUE, -1, NULL, FALSE);


    AIStateSetStateData((ActorID)tACDPet.idOwner, AIBEHAVIOR_TYPE_ESCORT_FOLLOW);
    tACDPet.AttributesSetInt(ATTRIBUTE_FOLLOW_TARGET_ACDID, tPlayer.idPlayerACD);
}




void SPlayerAddFollowerProxy(
    ActorCommonData & tACDPet,
    Player & tPlayer)
{
    SPlayerAddPet(tPlayer, PETTYPE_FOLLOWER_PROXY, tACDPet.id, TRUE, -1, NULL, FALSE);
}





int32 PlayerGetEmptyPetSlot(
    Player & tPlayer,
    ACDID idACDPet,
    PlayerPet ** ptPet)
{

    for (int32 i = 0; i < MAX_PETS; i++)
    {

        if (tPlayer.tPets[i].idACD == ACDID_INVALID || tPlayer.tPets[i].idACD == idACDPet)
        {
            *ptPet = &tPlayer.tPets[i];
            return i;
        }
    }
    return -1;
}


void PlayerPetInit(
    PlayerPet & tPlayerPet,
    PetType eType,
    ActorCommonData & tACDPet,
    Player & tPlayer)
{


    tPlayerPet.idACD = tACDPet.id;
    tPlayerPet.eType = eType;


    intptr nPetIndex = &tPlayerPet - tPlayer.tPets;
    if (nPetIndex - 1 == tPlayer.nMaxUsedPetIndex)
    {
        tPlayer.nMaxUsedPetIndex = nPetIndex;
    }



    tACDPet.AttributesSetInt(FastAttribKey(ATTRIBUTE_PET_OWNER), tPlayer.nPlayerIndex);
    tACDPet.AttributesSetInt(FastAttribKey(ATTRIBUTE_PET_CREATOR), tPlayer.nPlayerIndex);

    tACDPet.AttributesSetInt(FastAttribKey(ATTRIBUTE_PET_TYPE), eType);
    tACDPet.AttributesSetInt(FastAttribKey(ATTRIBUTE_EFFECT_OWNER_ANN), ANNFromACD(tPlayer.idPlayerACD));
}


static void sPlayerTracePlayerStat(
    Player & tPlayer)
{
    uint32 dwNow = GameGetServerTime();

    int32 nAFKTime = (dwNow - tPlayer.dwLastInputTime) / 60;

    ActorCommonData & tACDPlayer = ACDGet(tPlayer.idPlayerACD);
    int32 nLevel = tACDPlayer.AttributesGetInt(ATTRIBUTE_LEVEL);
    uint64 nExp = tACDPlayer.GetExperienceNext();

    TraceStat("playerupdate", tPlayer.idPlayerACD, "%u\t%llu\t%u", nLevel, nExp, nAFKTime);
}




static uint32 sGetSoonestLogoutTime(
    const Player & tPlayer)
{

    uint32 uSoonestLogoutTime = 0;
    for (uint32 i = 0; i < LOGOUT_CONTEXT_COUNT; i++)
    {
        if (tPlayer.auLogoutTimerData[i].uLogoutTimeTicks != 0)
        {
            if (uSoonestLogoutTime == 0)
            {
                uSoonestLogoutTime = tPlayer.auLogoutTimerData[i].uLogoutTimeTicks;
            }
            else
            {
                uSoonestLogoutTime = Math::Min(uSoonestLogoutTime, tPlayer.auLogoutTimerData[i].uLogoutTimeTicks);
            }
        }
    }
    return uSoonestLogoutTime;
}



























static int32 sSPlayerNumPlayersOnConnection(
    GameConnectionID idGameConnection)
{
    GameCommonData & tGCD = SGameGetGameCommonData();
    PlayerGlobals * ptPlayerGlobals = tGCD.ptPlayerGlobals;
    int32 nCount = 0;

    for (int32 i = 0; i < 4; i++)
    {
        Player & tPlayer = ptPlayerGlobals->tPlayerArray[i];

        if (tPlayer.idPlayerACD == ACDID_INVALID)
            continue;


        if (tPlayer.idGameConnection == idGameConnection)
            nCount++;
    }
    return nCount;
}






void SPlayerQueueFutureAchievementRegistrationAttempt(
    Player * ptPlayer)
{
}












void SPlayerQueueUnregisterAchievements(
    Player * ptPlayer)
{
}


























































































































































































static void sTickPlayerAchievementNotifications(
    Player & tPlayer,
    PerPlayerGlobals & tPerPlayer,
    BOOL bFlushAllImmediately)
{
    if (tPerPlayer.tRecentAchievementsEarned.size() == 0)
        return;



    uint32 dwNow = GameGetServerTime();
    if (!bFlushAllImmediately && dwNow - tPerPlayer.uLastAchievementEarnedTime < 60)
        return;



    SGameGlobals & tSGameGlobals = SGameGlobalsGet();
    if (tSGameGlobals.ptAchievementsInterface != NULL)
    {











        tPerPlayer.tRecentAchievementsEarned.clear();
        tPerPlayer.uLastAchievementEarnedTime = 0;
    }
}


static void sPopulateAchievementHealingList()

{






    Console::Achievements * ptAchievements = sgtAppGlobals.ptAchievements;








    ServerPlayerGlobals * ptServerPlayerGlobals = SGameManager::GameGet().ptServerPlayerGlobals;
    OnlineService::RewardIdList & idList = ptServerPlayerGlobals->tRewardHealingList;


    ptAchievements->GetAllRewardsOfTypes((1 << OnlineService::REWARD_TYPE_ITEM_TRANSMOG) | (1 << OnlineService::REWARD_TYPE_COSMETIC), idList, 0, false);
    if (GameIsSeasonal())
    {
        ptAchievements->GetAllRewardsOfTypes((1 << OnlineService::REWARD_TYPE_ITEM_TRANSMOG) | (1 << OnlineService::REWARD_TYPE_COSMETIC), idList, GameGetCurrentSeason(), false);
    }
}




static void sPlayerTrickleAchievementHealing(
    Player & tPlayer,
    PerPlayerGlobals & tPerPlayer)
{




    if (sPlayerTrickleLoreAchievements(tPlayer, tPerPlayer))
        return;


    sPlayerTrickleCosmeticAchievements(tPlayer, tPerPlayer);
}







static BOOL sPlayerTrickleLoreAchievements(
    Player & tPlayer,
    PerPlayerGlobals & tPerPlayer)
{




    if (tPerPlayer.nTrickleLoreAchievementIndex >= 0)
    {
        if (tPerPlayer.nTrickleLoreAchievementIndex < tPlayer.tHeroStateData.tSavedData.tLearnedLore.m_nLearnedCount)
        {
            SNO snoLore = tPlayer.tHeroStateData.tSavedData.tLearnedLore.m_snoLoreLearned[tPerPlayer.nTrickleLoreAchievementIndex];
            ScopedLoreDefinitionLockR pdefLore(snoLore);

            Achievement::EvaluationData tEvaluationData;
            tEvaluationData.SetPlayer(&tPlayer);
            tEvaluationData.EvaluateEvent(&tPlayer, Achievement::ACH_EVENT_LORE_LEARNED, pdefLore->snoConversation, 1, (uint32)sub_71007b5e70());

            tPerPlayer.nTrickleLoreAchievementIndex++;


            return TRUE;
        }
        else
        {

            tPerPlayer.nTrickleLoreAchievementIndex = -1;
        }
    }

    return FALSE;
}




static BOOL sPlayerTrickleCosmeticAchievements(
    Player & tPlayer,
    PerPlayerGlobals & tPerPlayer)
{




    if (tPerPlayer.nTrickleCosmeticAchievementIndex >= 0)
    {
        ServerPlayerGlobals * ptServerPlayerGlobals = SGameManager::GameGet().ptServerPlayerGlobals;
        const OnlineService::RewardIdList & tList = ptServerPlayerGlobals->tRewardHealingList;

        if (tPerPlayer.nTrickleCosmeticAchievementIndex < (int32)tList.size())
        {


            Console::Achievements & tAchievements = *sgtAppGlobals.ptAchievements;







            OnlineService::RewardId idReward = tList[tPerPlayer.nTrickleCosmeticAchievementIndex];
            tPerPlayer.nTrickleCosmeticAchievementIndex++;

            OnlineService::RewardDefinition tReward;
            if (tAchievements.GetRewardDefinition(idReward, tReward))
            {



                SGameGlobals & tSGameGlobals = SGameGlobalsGet();
                GBID gbid = idReward;

                if (tReward.m_type == OnlineService::REWARD_TYPE_ITEM_TRANSMOG)
                {

                    if (ItemCrafting_IsTransmogUnlocked(gbid, &tPlayer, NULL))
                    {
                        return TRUE;
                    }
                }
                else
                {


                    const CosmeticItemData & tCosmeticItemData = tPlayer.tCosmeticItemSavedData.tCosmeticItemData;
                    if (tCosmeticItemData.HasItem(gbid, 1, 0))
                    {
                        return TRUE;
                    }
                }



                OnlineService::AchievementIdList listParentAchievements;
                tAchievements.GetRewardParentAchievements(idReward, listParentAchievements);

                for (uint32 i = 0; i < listParentAchievements.size(); i++)
                {

                    OnlineService::AchievementRecord tRecord;
                    if (tSGameGlobals.ptAchievementsInterface != NULL && tSGameGlobals.ptAchievementsInterface->FindAwardedAchievement(tPlayer.tAccount.tHandle, sub_71007b7080(), listParentAchievements[i], tRecord))
                    {

                        if (tReward.m_type == OnlineService::REWARD_TYPE_ITEM_TRANSMOG)
                        {
                            SItemCrafting_TryUnlockTransmog(tReward.m_id, tPlayer, FALSE, TRUE);
                        }
                        else
                        {
                            sPlayerGiveRewardCosmetic(tPlayer, tReward.m_id, listParentAchievements[i]);
                        }

                        break;
                    }
                }

                return TRUE;
            }
        }
        else
        {
            tPerPlayer.nTrickleCosmeticAchievementIndex = -1;
        }
    }

    return FALSE;
}




void SPlayersUpdate()

{

    GameCommonData & tGCD = SGameGetGameCommonData();
    PlayerGlobals * ptPlayerGlobals = tGCD.ptPlayerGlobals;
    ServerPlayerGlobals * ptServerPlayerGlobals = SGameManager::GameGet().ptServerPlayerGlobals;

    ServerUpdateWasCalled(ptServerPlayerGlobals->tPlayerUpdateData);
    ServerQueueUpdateFunction(60, ptServerPlayerGlobals->tPlayerUpdateData);

    uint32 dwNow = GameGetServerTime();


    if (dwNow >= ptServerPlayerGlobals->dwNextItemDupeScan)
    {
        CValidateDupeScan();
        ptServerPlayerGlobals->dwNextItemDupeScan = dwNow + 36000;
    }

    BOOL bPing = FALSE;
    if (dwNow >= ptServerPlayerGlobals->uNextPingTime)
    {
        bPing = TRUE;
        ptServerPlayerGlobals->uNextPingTime = dwNow + 180;
    }


    BOOL bCheckAvengerSpawns = FALSE;
    if (dwNow >= ptServerPlayerGlobals->dwNextAvengerSpawnCheck)
    {
        bCheckAvengerSpawns = TRUE;
        ScopedGlobalDefinitionLockR pdefGlobal;
        ptServerPlayerGlobals->dwNextAvengerSpawnCheck = dwNow + pdefGlobal->nAvengerUpdateCheckInterval * 60;
    }



    Player * deletedPlayerArray[4];
    int32 i;
    int32 numDeletedPlayers = 0;

    ScopedGlobalDefinitionLockR pdefGlobals;


    for (i = 0; i < 4; i++)
    {
        Player & tPlayer = ptPlayerGlobals->tPlayerArray[i];

        if (tPlayer.idPlayerACD == ACDID_INVALID)
            continue;




        SPlayerConsoleSync(tPlayer);
        Avenger::Console::UpdateForPlayer(tPlayer);
        if (bCheckAvengerSpawns)
        {
            Avenger::Console::CheckSpawnStartForPlayer(tPlayer);
        }
        SPlayerUpdateKillCounters(tPlayer);


        uint32 uSoonestLogoutTime = sGetSoonestLogoutTime(tPlayer);
        if (uSoonestLogoutTime != 0)
        {
            BOOL bDeletePlayer = FALSE;
            uint32 uCurrentTime = GameGetServerTime();

            if (tPlayer.idGameConnection == GAMECONNECTIONID_INVALID)
            {

                BossEncounterManager::PlayerRemoved(&tPlayer, FALSE);
                NephalemRiftManager::PlayerRemoved(&tPlayer, FALSE);
                SetEncounterManager::PlayerRemoved(&tPlayer, FALSE);
                Minimap::OnRemovedPlayer(tPlayer);
                VoteKick::Server::OnRemovedPlayer(tPlayer);
            }

            if (tPlayer.idGameConnection != GAMECONNECTIONID_INVALID && uCurrentTime >= uSoonestLogoutTime + 300)
            {


                SPlayerAddToSaveList(tPlayer, 2, 0x40000, PLAYER_SAVE_MODE_END_OF_FRAME);
                bDeletePlayer = TRUE;
            }





            if (tPlayer.idGameConnection == GAMECONNECTIONID_INVALID &&
                !SPlayerInScriptedSequence(tPlayer) &&
                (uCurrentTime >= uSoonestLogoutTime || &ACDGet(tPlayer.idPlayerACD) != NULL))  // the ACDGet operand's use folded away in the original; only its lookup (line 4025) survives
            {
                bDeletePlayer = TRUE;
            }


            if (bDeletePlayer)
            {
                deletedPlayerArray[numDeletedPlayers] = &tPlayer;
                numDeletedPlayers++;
                continue;
            }
        }

        PerPlayerGlobals & tPerPlayer = ptServerPlayerGlobals->tPerPlayer[tPlayer.nPlayerIndex];
        if (tPerPlayer.tNextAchievementRegisterAttempt != 0 && tPerPlayer.tNextAchievementRegisterAttempt < dwNow)
        {






            tPerPlayer.tNextAchievementRegisterAttempt = 0;
            SGameAchievementRegister(tPlayer.tAccount.tHandle);
        }

        if (tPerPlayer.tNextAchievementUnregisterAttempt != 0 && tPerPlayer.tNextAchievementUnregisterAttempt < dwNow)
        {

            tPerPlayer.tNextAchievementUnregisterAttempt = 0;
            SGameAchievementUnregister(tPlayer.tAccount.tHandle);
        }


        sTickPlayerAchievementNotifications(tPlayer, tPerPlayer, FALSE);


        sPlayerTrickleAchievementHealing(tPlayer, tPerPlayer);




        tPlayer.tHeroStateData.tSavedData.uSecondsPlayed++;


        if ((tPlayer.tHeroStateData.tSavedData.uSecondsPlayed % 20) == 0)
        {
            PlayerList listPlayers(PlayerListGetElementPool());
            PlayerListGetSingle(listPlayers, tPlayer);

            DWordDataMessage tMsg;
            tMsg.dwData = tPlayer.tHeroStateData.tSavedData.uSecondsPlayed;

            MessageSendToClient(listPlayers, SMSG_CONSOLE_SYNC_HERO_TIME_PLAYED, &tMsg, sizeof(tMsg));
        }


        if (TraceStatEnabled() &&
            dwNow >= tPlayer.dwNextStatTime)
        {
            sPlayerTracePlayerStat(tPlayer);

            const uint32 PLAYER_STAT_INTERVAL = 600;
            tPlayer.dwNextStatTime = dwNow + PLAYER_STAT_INTERVAL;
        }

        if (dwNow >= tPlayer.dwNextAttributeUpdateTime)
        {
            ActorTriggerAttributeUpdate((ActorID)ACDGetOwner(tPlayer.idPlayerACD));

            const uint32 PLAYER_ATTRIBUTE_UPDATE_INTERVAL = 60;
            tPlayer.dwNextAttributeUpdateTime = dwNow + PLAYER_ATTRIBUTE_UPDATE_INTERVAL;
        }

        if (bPing)
        {
            tPerPlayer.flHighestPing = Math::Max(tPerPlayer.flHighestPing, SPlayerGetPing(tPlayer));
        }


        ActorCommonData & tACDPlayer = ACDGet(tPlayer.idPlayerACD);
        uint32 dwAFKTime = (dwNow - tPlayer.dwLastInputTime) / 60;
        tACDPlayer.AttributesSetInt(ATTRIBUTE_AFK, dwAFKTime >= pdefGlobals->ptServerData->nAfkTime);

        sPollPerkBuffs(tPlayer, tACDPlayer);

        sPollLimitedDurationItems(tPlayer, tACDPlayer);
    }

























    if (numDeletedPlayers > 0)
    {
        for (i = 0; i < numDeletedPlayers; i++)
            SPlayerAddToSaveList(*deletedPlayerArray[i], 4, 8, PLAYER_SAVE_MODE_END_OF_FRAME);

        SPlayerPurgeSaveList(TRUE, OnlineServiceServer::GenericCallback());
        for (i = 0; i < numDeletedPlayers; i++)
        {
            Player & tPlayer = *deletedPlayerArray[i];
            if (tPlayer.idGameConnection != GAMECONNECTIONID_INVALID)
            {

                ServerPlayerLeftOnConnection(tPlayer.idGameConnection, tPlayer);
                int32 nCount = sSPlayerNumPlayersOnConnection(tPlayer.idGameConnection);
                if (nCount <= 1)
                {
                    ServerDisconnectWhenOutgoingQueueEmpty(tPlayer);
                }
            }





            SPlayerDelete(tPlayer);
        }
    }
    else SPlayerPurgeSaveList(FALSE, OnlineServiceServer::GenericCallback());

}



BOOL SPlayerTestAllowedToInspect(
    ACDID idACDPlayer,
    ACDID idACDToInspect)
{

    Player * ptPlayer = PlayerGetByACD(idACDPlayer);
    Player * ptInspect = PlayerGetByACD(idACDToInspect);
    if (ptPlayer == NULL || ptInspect == NULL)
    {
        return FALSE;
    }


    if (ptPlayer->nPlayerIndex == ptInspect->nPlayerIndex)
    {
        return FALSE;
    }

    ActorCommonData * ptACDPlayer = ACDTryToGet(idACDPlayer);
    ActorCommonData * ptACDToInspect = ACDTryToGet(idACDToInspect);

    if (ptACDPlayer != NULL && ptACDToInspect != NULL)
    {
        return TRUE;
    }

    return FALSE;
}



static void sSendStartInspect(
    ACDID idACDPlayer,
    ACDID idACDToInspect)
{
    Player * ptPlayer = PlayerGetByACD(idACDPlayer);
    if (ptPlayer == NULL)
        return;
    Player * ptInspect = PlayerGetByACD(idACDToInspect);
    if (ptInspect == NULL)
        return;
    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetSingle(listDestPlayers, *ptPlayer);
    PlayerIndexMessage tMessage;
    SigmaMemoryClear(&tMessage, sizeof(tMessage));
    tMessage.nPlayerIndex = ptInspect->nPlayerIndex;
    MessageSendToClient(listDestPlayers, SMSG_INSPECT, &tMessage, sizeof(tMessage));
}
















void SPlayerInspect(
    ACDID idACDPlayer,
    ACDID idACDToInspect)
{
    if (!SPlayerTestAllowedToInspect(idACDPlayer, idACDToInspect))
        return;
    Player * ptPlayer = PlayerGetByACD(idACDPlayer);
    if (!ptPlayer->bIsPrimaryPlayerServerOnly)
        ptPlayer = PlayerGetPrimaryPlayerForGameConnection(ptPlayer->idGameConnection);
    ptPlayer->idACDPlayerToInspect = idACDToInspect;
    ServerQueueClientUpdate();
    sSendStartInspect(idACDPlayer, idACDToInspect);
}

















void SPlayerInspectEnd(
    ACDID idACDPlayer)
{
    Player * ptPlayer = PlayerGetByACD(idACDPlayer);
    if (!ptPlayer->bIsPrimaryPlayerServerOnly)
        ptPlayer = PlayerGetPrimaryPlayerForGameConnection(ptPlayer->idGameConnection);
    if (ptPlayer != NULL)
        ptPlayer->idACDPlayerToInspect = ACDID_INVALID;
}













inline XPooledDataIDList & sGetVisitedSceneList()
{
    ServerPlayerGlobals & tServerPlayerGlobals = *SGameManager::GameGet().ptServerPlayerGlobals;
    return tServerPlayerGlobals.listVisitedSScenes;
}




void SPlayerMarkSceneAsEntered(
    const SSceneID idSScene)
{
    XPooledDataIDList & listVisitedSScenes = sGetVisitedSceneList();
    if (listVisitedSScenes.Find(idSScene) == NULL)
        listVisitedSScenes.AddTail_Tracked(idSScene);
}






void SPlayerRemoveSceneFromVisited(
    const SSceneID idSScene)
{
    if (SGameManager::GameGet().ptServerPlayerGlobals == NULL)
        return;
    XPooledDataIDList & listVisitedSScenes = sGetVisitedSceneList();
    POS pos = listVisitedSScenes.Find(idSScene);
    if (pos != NULL)
        listVisitedSScenes.RemoveAt(pos);
}








void SPlayerGiveSharedMapVision(
    Player & tPlayerDest)
{

    const XPooledDataIDList & listVisitedSScenes = sGetVisitedSceneList();






    int32 nPlayerCount = PlayerGetNumPlayers();
    if (nPlayerCount == 1 && listVisitedSScenes.Count() == 1)
    {
        return;
    }


    XPooledDataIDList listVisitedWorlds(MemoryPoolsGetDATAIDListPool());
    POS pos = listVisitedSScenes.GetHeadPosition();
    while (pos)
    {
        SSceneID idSScene = listVisitedSScenes.GetNext(pos);


        const SScene * ptSScene = SSceneGet(idSScene);
        SWorldID idSWorld = ptSScene->tSCD.idSWorld;
        if (!listVisitedWorlds.Find(idSWorld))
        {
            listVisitedWorlds.AddTail_Tracked(idSWorld);
        }
    }


    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetSingle(listDestPlayers, tPlayerDest);


    pos = listVisitedWorlds.GetHeadPosition();
    while (pos)
    {
        SWorldID idSWorld = listVisitedWorlds.GetNext(pos);

        ServerMessageSendWorldRevealToPlayer(listDestPlayers, idSWorld);
    }


    pos = listVisitedSScenes.GetHeadPosition();

    while (pos)
    {
        SSceneID idSScene = listVisitedSScenes.GetNext(pos);

        ServerMessageSendMapRevealSceneToPlayer(listDestPlayers, idSScene, FALSE);
    }
}



void sPruneDPSList(
    const ACDID idACDPlayer)
{
    Player * ptPlayer = PlayerGetByACD(idACDPlayer);
    if (ptPlayer)
    {
        DPSBucketList & listDPSInfo = PlayerGetDPSBucketList(*ptPlayer);
        float & flDPSDamageTotal = PlayerGetDPSDamageTotal(*ptPlayer);
        SGameGlobals & tSGameGlobals = SGameGlobalsGet();
        const uint32 & dwDuration = tSGameGlobals.tDPSTrackingInfo.m_uWindowTicks;
        uint32 dwNow = GameGetServerTime();
        uint32 dwMinTime = 0;
        if (dwNow > dwDuration)
        {
            dwMinTime = dwNow - dwDuration;
        }

        POS pos = listDPSInfo.GetHeadPosition();
        while (pos)
        {
            POS posLast = pos;
            DPSBucket & tBucket = listDPSInfo.GetNext(pos);
            if (tBucket.dwTick < dwMinTime)
            {
                flDPSDamageTotal -= tBucket.flDamage;
                listDPSInfo.RemoveAt(posLast);
            }
        }
    }
}




void PlayerRecordDamageDone(
    const ACDID idACDPlayer,
    float flDamage,
    const ACDID idACDTarget,
    const BOOL bCriticalHit,
    const BOOL bDOT)
{
    BOOL bAdded = FALSE;

    if (flDamage <= EPSILON)
        return;

    Player * ptPlayer = PlayerGetByACD(idACDPlayer);
    if (ptPlayer)
    {
        if (!bCriticalHit && !(ptPlayer->dwServerSidePrefFlags & (1 << SSPREF_SHOW_DAMAGE_NUMBERS)))
            return;

        uint32 dwNow = GameGetServerTime();
        sPruneDPSList(idACDPlayer);

        DPSBucketList & listDPSInfo = PlayerGetDPSBucketList(*ptPlayer);
        float & flDPSDamageTotal = PlayerGetDPSDamageTotal(*ptPlayer);

        if (listDPSInfo.Count() > 0)
        {
            DPSBucket & tBucket = listDPSInfo.GetTail();
            if (tBucket.dwTick == dwNow)
            {
                tBucket.flDamage += flDamage;
                bAdded = TRUE;
            }
        }
        else
        {
            flDPSDamageTotal = 0.0f;
        }

        flDPSDamageTotal += flDamage;

        if (!bAdded)
        {
            DPSBucket tNewBucket;
            tNewBucket.dwTick = dwNow;
            tNewBucket.flDamage = flDamage;
            listDPSInfo.AddTail_Tracked(tNewBucket);
        }

        if (ACDIsPlayer(idACDTarget))
        {
            Achievement::EvaluationData tEvaluationData;
            tEvaluationData.SetPlayer(ptPlayer);
            tEvaluationData.EvaluateEvent(ptPlayer, Achievement::ACH_EVENT_PVP_PLAYER_DAMAGED_PLAYER, Math::FloatToInt(flDamage), (uint32)sub_71007b5e70());
        }
    }

    FloatingNumberMessage tMessage;
    tMessage.annTarget = ANNFromACD(idACDTarget);
    tMessage.flDamage = flDamage;
    if (bDOT)
    {
        tMessage.eType = FLOATING_NUMBER_TYPE_DOT_DAMAGE_DONE;
        ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
        if (ptACDTarget)
        {
            SPlayerQueueFloatingNumberDOTDamageDone(ptPlayer, *ptACDTarget, flDamage);
            return;
        }
    }
    else if (bCriticalHit)
    {
        if (!ptPlayer || !(ptPlayer->dwServerSidePrefFlags & (1 << SSPREF_SHOW_CRITICALS)))
            return;
        tMessage.eType = FLOATING_NUMBER_TYPE_CRITICAL_HIT_DONE;
    }
    else
    {
        tMessage.eType = FLOATING_NUMBER_TYPE_DAMAGE_DONE;
    }

    if (ptPlayer)
    {
        PlayerList listDestPlayers(PlayerListGetElementPool());
        listDestPlayers.AddTail_Tracked(ptPlayer);
        MessageSendToClient(listDestPlayers, SMSG_FLOATING_NUMBER, &tMessage, sizeof(tMessage));
    }
}






































void PlayerRecordDamageTaken(
    const ACDID idACDPlayer,
    float flDamage,
    const ACDID idACDAttacker,
    const BOOL bCriticalHit,
    const BOOL bDOT)
{
    Player * ptPlayer = PlayerGetByACD(idACDPlayer);
    if (!bCriticalHit && !(ptPlayer->dwServerSidePrefFlags & (1 << SSPREF_SHOW_DAMAGE_NUMBERS)))
        return;

    FloatingNumberMessage tMessage;
    tMessage.annTarget = ANNFromACD(idACDPlayer);
    tMessage.flDamage = flDamage;
    if (bDOT)
    {
        tMessage.eType = FLOATING_NUMBER_TYPE_DOT_DAMAGE_TAKEN;
        ActorCommonData * ptACDPlayer = ACDTryToGet(idACDPlayer);
        if (ptACDPlayer)
        {
            SPlayerQueueFloatingNumberDOTDamageTaken(*ptACDPlayer, flDamage);
            return;
        }
    }
    else if (bCriticalHit)
    {
        if (!(ptPlayer->dwServerSidePrefFlags & (1 << SSPREF_SHOW_CRITICALS)))
            return;
        tMessage.eType = FLOATING_NUMBER_TYPE_CRITICAL_HIT_TAKEN;
    }
    else
    {
        tMessage.eType = FLOATING_NUMBER_TYPE_DAMAGE_TAKEN;
    }

    PlayerList listDestPlayers(PlayerListGetElementPool());
    listDestPlayers.AddTail_Tracked(ptPlayer);
    MessageSendToClient(listDestPlayers, SMSG_FLOATING_NUMBER, &tMessage, sizeof(tMessage));
}


















void PlayerRecordHeal(
    ActorCommonData & tACDPlayer,
    float flHealAmount)
{
    if (flHealAmount > EPSILON)
    {
        Player * ptPlayer = PlayerGetByACD(tACDPlayer.id);
        if (ptPlayer != NULL)
        {
            SGameStatsPlayerCounterIncrement(ptPlayer, STAT_COUNTER_PLAYER_POWER_HEALING_RECEIVED, -1, -1, flHealAmount);
            SPlayerQueueFloatingNumberHealing(tACDPlayer, flHealAmount);
        }
    }
}

































void SPlayerQueueFloatingNumberDOTDamageDone(
    Player * ptPlayerAttacker,
    ActorCommonData & tACDTarget,
    float flDamage)
{
    if (!(ptPlayerAttacker->dwServerSidePrefFlags & (1 << SSPREF_SHOW_DAMAGE_NUMBERS)))
        return;
    float flDOTDamageTaken = tACDTarget.AttributesGetFloat(MakeAttribKey(ATTRIBUTE_DOT_DAMAGE_TAKEN_QUEUED, ptPlayerAttacker->nPlayerIndex));
    if (flDOTDamageTaken < EPSILON)
        tACDTarget.AttributesSetInt(MakeAttribKey(ATTRIBUTE_DOT_DAMAGE_TAKEN_QUEUED_TICK, ptPlayerAttacker->nPlayerIndex), GameGetServerTime() + 10);
    tACDTarget.AttributesSetFloat(MakeAttribKey(ATTRIBUTE_DOT_DAMAGE_TAKEN_QUEUED, ptPlayerAttacker->nPlayerIndex), flDOTDamageTaken + flDamage);
}








inline void SPlayerQueueFloatingNumberDOTDamageTaken(
    ActorCommonData & tACDPlayer,
    float flDamage)
{
    Player * ptPlayer = PlayerGetByACD(tACDPlayer.id);
    if (ptPlayer && !(ptPlayer->dwServerSidePrefFlags & (1 << SSPREF_SHOW_DAMAGE_NUMBERS)))
        return;
    float flDOTDamageTaken = tACDPlayer.AttributesGetFloat(ATTRIBUTE_DOT_DAMAGE_TAKEN_QUEUED_PLAYER);
    if (flDOTDamageTaken < EPSILON)
        tACDPlayer.AttributesSetInt(ATTRIBUTE_DOT_DAMAGE_TAKEN_QUEUED_PLAYER_TICK, GameGetServerTime() + 10);
    tACDPlayer.AttributesSetFloat(ATTRIBUTE_DOT_DAMAGE_TAKEN_QUEUED_PLAYER, flDOTDamageTaken + flDamage);
}








void SPlayerQueueFloatingNumberHealing(
    ActorCommonData & tACDTarget,
    float flHealAmount)
{
    Player * ptControllingPlayer = ACDGetControllingPlayer(tACDTarget.id);
    if (ptControllingPlayer != NULL && !(ptControllingPlayer->dwServerSidePrefFlags & (1 << SSPREF_SHOW_HEAL_NUMBERS)) && !GameTypeIsPvP())
        return;
    float flHealingReceived = tACDTarget.AttributesGetFloat(ATTRIBUTE_HEALING_RECEIVED_QUEUED);
    if (flHealingReceived < EPSILON)
        tACDTarget.AttributesSetInt(ATTRIBUTE_HEALING_RECEIVED_QUEUED_TICK, GameGetServerTime());
    tACDTarget.AttributesSetFloat(ATTRIBUTE_HEALING_RECEIVED_QUEUED, flHealingReceived + flHealAmount);
}












void SPlayerRecordCurrencyPickedUp(
    const ACDID idACDPlayer,
    const uint64 uCurrencyPickedUp,
    const CurrencyType eCurrencyType)
{
    Player * ptPlayer = PlayerGetByACD(idACDPlayer);
    if (ptPlayer != NULL)
    {
        ptPlayer->tStats.m_uTotalCurrencyPickedUp[eCurrencyType] += uCurrencyPickedUp;
        ptPlayer->tStats.m_uMaxLumpOfCurrencyPickedUp[eCurrencyType] = Math::Max(ptPlayer->tStats.m_uMaxLumpOfCurrencyPickedUp[eCurrencyType], uCurrencyPickedUp);
    }
}



void SPlayerRecordHealthGlobePickedUp(
    const ACDID idACDPlayer,
    const float flAmountHealed)
{
    Player * ptPlayer = PlayerGetByACD(idACDPlayer);
    if (ptPlayer != NULL)
    {
        ptPlayer->tStats.m_flTotalHealthPickedUp += flAmountHealed;
        ptPlayer->tStats.m_flMaxLumpOfHealthPickedUp = Math::Max(ptPlayer->tStats.m_flMaxLumpOfHealthPickedUp, flAmountHealed);
    }
}



















BOOL SPlayerRecordHighestSoloRiftLevel(
    Player * ptPlayer,
    int32 nRiftLevel)
{
    if (!ptPlayer)
        return FALSE;

    ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
    if (!ptACDPlayer)
        return FALSE;

    ScopedGameBalanceDefinitionLockR pdefGB(GlobalSNOGet(GLOBALSNO_GB_TIERED_LOOT_RUN_LEVELS));
    int32 nMaxRiftLevel = pdefGB->tTieredLootRunLevelTable.serTieredLootRunLevels.ElementCount(sizeof(TieredLootRunLevel));
    if (nRiftLevel <= 0 || nRiftLevel > nMaxRiftLevel)
        return FALSE;

    ptPlayer->bCompletedSoloRift = TRUE;

    int32 nCurrentHeroHighestRiftLevel = ptACDPlayer->AttributesGetInt(ATTRIBUTE_HIGHEST_HERO_SOLO_RIFT_LEVEL);
    if (nRiftLevel > nCurrentHeroHighestRiftLevel)
    {
        ptACDPlayer->AttributesSetInt(ATTRIBUTE_HIGHEST_HERO_SOLO_RIFT_LEVEL, nRiftLevel);

        PlayerIntValMessage tMessage;
        SigmaMemoryClear(&tMessage, sizeof(tMessage));
        tMessage.nPlayerIndex = ptPlayer->nPlayerIndex;
        tMessage.nVal = nRiftLevel;

        PlayerList listDestPlayers(PlayerListGetElementPool());
        PlayerListGetAllConnected(listDestPlayers, -1, FALSE);
        MessageSendToClient(listDestPlayers, SMSG_PLAYER_HIGHEST_HERO_SOLO_RIFT_LEVEL, &tMessage, sizeof(tMessage));
    }

    int32 nCurrentHighestRiftLevel = ptACDPlayer->AttributesGetInt(ATTRIBUTE_HIGHEST_SOLO_RIFT_LEVEL);
    if (nRiftLevel > nCurrentHighestRiftLevel)
    {
        ptACDPlayer->AttributesSetInt(ATTRIBUTE_HIGHEST_SOLO_RIFT_LEVEL, nRiftLevel);
        return TRUE;
    }

    return FALSE;
}



















float PlayerGetRecordedDPS(
    const ACDID idACDPlayer)
{
    Player * ptPlayer = PlayerGetByACD(idACDPlayer);
    if (ptPlayer)
    {
        SGameGlobals & tSGameGlobals = SGameGlobalsGet();
        sPruneDPSList(idACDPlayer);

        float & flDPSDamageTotal = PlayerGetDPSDamageTotal(*ptPlayer);
        return flDPSDamageTotal * tSGameGlobals.tDPSTrackingInfo.m_flOOWindowSecs;
    }
    return 0.0f;
}





BOOL SPlayerInSaveList(
    const Player & tPlayer)
{
    ServerPlayerGlobals * ptServerPlayerGlobals = SGameManager::GameGet().ptServerPlayerGlobals;
    SPlayerSaveMap & tPlayerSaveMap = ptServerPlayerGlobals->tPlayerSaveMap;

    return tPlayerSaveMap.find(tPlayer.tAccount.tHandle.m_id) != tPlayerSaveMap.end();
}




void SPlayerAddToSaveList(
    Player & tPlayer,
    uint32 dwHeroSaveOptions,
    PlayerSaveReason reason,
    SPlayerSaveMode eSaveMode)
{
    if (tPlayer.bIsBeingDeleted || tPlayer.idPlayerACD == ACDID_INVALID)
        return;





    if (GameIsBattleNetServer() && CmdLineGetParams()->tBitArray.Test(CMDLINE_BLIZZCON_BIT))
    {


        if (dwHeroSaveOptions & 2)
            SPlayerDisconnect(tPlayer, 0, FALSE);
        return;
    }


















    BOOL fSaveAllowed = TRUE;
    if (!GameIsParamFlagSet(GPF_SAVE_HEROS) || ReplayIsReplayGame()) fSaveAllowed = FALSE;
    if (GameTesting::GetTestType() != 0)
        return;































    if (GameIsStartUp())
        fSaveAllowed = FALSE;





    SGameGlobals & tSGameGlobals = SGameGlobalsGet();
    if (tSGameGlobals.dwFlags & (1 << SGAMEGLOBALS_DISABLED_HERO_SAVE))
        fSaveAllowed = FALSE;









    if (!fSaveAllowed && (dwHeroSaveOptions & 2))
    {








        SPlayerDisconnect(tPlayer, 0, FALSE);
    }






    sAddToSaveList(tPlayer, dwHeroSaveOptions, reason);


    switch (eSaveMode)
    {
    case PLAYER_SAVE_MODE_IMMEDIATE:
        SPlayerPurgeSaveList(TRUE, OnlineServiceServer::GenericCallback());
        break;
    case PLAYER_SAVE_MODE_WITHIN_5_SECONDS:
    {
        ServerPlayerGlobals * ptServerPlayerGlobals = SGameManager::GameGet().ptServerPlayerGlobals;
        uint32 dwNextSaveTime = Math::Min(GameGetServerTime() + PLAYER_SAVE_WITHIN_5_SECONDS, ptServerPlayerGlobals->dwNextAllPlayersSaveTime);
        ptServerPlayerGlobals->dwNextAllPlayersSaveTime = dwNextSaveTime;
        break;
    }
    case PLAYER_SAVE_MODE_END_OF_FRAME:
    {
        ServerPlayerGlobals * ptServerPlayerGlobals = SGameManager::GameGet().ptServerPlayerGlobals;
        ptServerPlayerGlobals->dwNextAllPlayersSaveTime = GameGetServerTime();
        break;
    }
    default:
        break;
    }
}



















SPlayerConsumables & SPlayerGetConsumables(
    Player & tPlayer)
{
    ServerPlayerGlobals * ptServerPlayerGlobals = SGameManager::GameGet().ptServerPlayerGlobals;
    return ptServerPlayerGlobals->tPerPlayer[tPlayer.nPlayerIndex].tConsumables;
}




BOOL SPlayerRemoveFromSaveList(
    Player & tPlayer)
{
    ServerPlayerGlobals * ptServerPlayerGlobals = SGameManager::GameGet().ptServerPlayerGlobals;
    return ptServerPlayerGlobals->tPlayerSaveMap.erase(tPlayer.tAccount.tHandle.m_id) != 0;
}































void SPlayerUpdateCombatStatus(
    Player & tPlayer)
{
    ActorID idActorPlayer = (ActorID)ACDGetOwner(tPlayer.idPlayerACD);

    BrainAPIUpdateTargets(idActorPlayer, TRUE);




    ActorCommonData & tACDPlayer = ACDGet(tPlayer.idPlayerACD);
    if (tACDPlayer.AttributesGetInt(ATTRIBUTE_IN_COMBAT))
    {



        if (!BrainAPIHasEnemyTargetForInCombatStatus(idActorPlayer))
        {


            uint32 uNow = GameGetServerTime();
            uint32 uNextCheck = tACDPlayer.AttributesGetInt(ATTRIBUTE_IN_COMBAT_NEXT_TICK_CHECK);
            if (uNow >= uNextCheck)
            {
                tACDPlayer.SetCombatStatus(FALSE);

                if (tACDPlayer.IsAlive())
                {
                    float flHPPercent = tACDPlayer.GetHitpointPercent();

                    if (flHPPercent <= 0.1f && !GameTypeIsPvP() && flHPPercent > 0.000001f)
                    {

                        if (!(tPlayer.uServerFlags & (1 << PSF_INELIGIBLE_FOR_SURVIVOR_AWARD)))
                        {

                            tPlayer.uServerFlags |= (1 << PSF_INELIGIBLE_FOR_SURVIVOR_AWARD);

                            LowHealthCombatMessage tMessage;
                            tMessage.flHealthPercent = flHPPercent;

                            ScopedGameBalanceDefinitionLockR ptGameBalDef(GlobalSNOGet(GLOBALSNO_GB_EXPERIENCE));
                            const ExperienceTable & tExpTable = ptGameBalDef->tExperienceTable;
                            int32 nLevel = tACDPlayer.AttributesGetInt(ATTRIBUTE_LEVEL);


                            tMessage.uXPRewarded = tExpTable.ptExperienceLevels[nLevel - 1].nKillCounterXP;
                            ExperienceGrantDirectly(tACDPlayer, tMessage.uXPRewarded, TRUE, TRUE);

                            PowerAPI_BuffApplyOnCalloutEventEffects(tPlayer.idPlayerACD, 6);

                            PlayerList listDestPlayers(PlayerListGetElementPool());
                            listDestPlayers.AddTail_Tracked(&tPlayer);
                            MessageSendToClient(listDestPlayers, SMSG_LOW_HEALTH_COMBAT, &tMessage, sizeof(tMessage));





                            {
                                Achievement::EvaluationData tEvaluationData;
                                tEvaluationData.SetPlayer(&tPlayer);

                                tEvaluationData.EvaluateEvent(&tPlayer, Achievement::ACH_EVENT_SURVIVED_COMBAT_WITH_LOW_HEALTH, 1, (uint32)sub_71007b5e70());



                            }
                        }
                    }
                }
            }
        }
    }

    int32 nNumPlayerEnemies = BrainAPIGetNumEnemyPlayerTargets(idActorPlayer);

    if (tACDPlayer.AttributesGetInt(ATTRIBUTE_IN_COMBAT_WITH_PLAYER) && nNumPlayerEnemies == 0)
    {
        tACDPlayer.SetCombatWithPlayerStatus(FALSE);
    }
}



void SPlayerUpdateKillCounters(
    Player & tPlayer)
{
    for (int32 i = 0; i < KILLCOUNTER_COUNT; i++)
    {
        SPlayerUpdateKillCounter(tPlayer, i);
    }
}



void SPlayerClearEnemyList(
    Player & tPlayer)
{
    BrainAPIReleaseAllTargets((ActorID)ACDGetOwner(tPlayer.idPlayerACD));
    SPlayerUpdateCombatStatus(tPlayer);
}



























































ServerTrickleList * SPlayerGetPossibleTrickleList(
    const Player & tPlayer)
{
    ServerPlayerGlobals * ptServerPlayerGlobals = SGameManager::GameGet().ptServerPlayerGlobals;
    if (ptServerPlayerGlobals == NULL)
        return NULL;
    return ptServerPlayerGlobals->tPerPlayer[tPlayer.nPlayerIndex].pListPossibleTrickle;
}























ServerTrickleList * SPlayerGetPrevTrickleList(
    const Player & tPlayer)
{
    ServerPlayerGlobals * ptServerPlayerGlobals = SGameManager::GameGet().ptServerPlayerGlobals;
    if (ptServerPlayerGlobals == NULL)
        return NULL;
    return ptServerPlayerGlobals->tPerPlayer[tPlayer.nPlayerIndex].pListTricklePrev;
}

























void SPlayerTrickleAdd(
    Player & tPlayer,
    const ACDID idACD)
{
    ServerTrickleList * pList = SPlayerGetPossibleTrickleList(tPlayer);
    if (pList->Find(idACD) != NULL)
        return;
    pList->AddTail_Tracked(idACD);

    ActorCommonData * ptACD = ACDTryToGet(idACD);
    Actor * ptActor = ActorGet(ptACD ? (ActorID)ptACD->idOwner : ACTORID_INVALID);
    ptActor->dwTrickleBits |= (1 << tPlayer.nPlayerIndex);
}







void SPlayerTrickleRemove(
    Player & tPlayer,
    const ACDID idACD)
{
    ServerPlayerGlobals * ptServerPlayerGlobals = SGameManager::GameGet().ptServerPlayerGlobals;
    ServerTrickleList * pList = ptServerPlayerGlobals->tPerPlayer[tPlayer.nPlayerIndex].pListPossibleTrickle;
    POS pos = pList->Find(idACD);
    if (pos == NULL)
        return;
    pList->RemoveAt(pos);

    ServerTrickleList * pListPrev = SPlayerGetPrevTrickleList(tPlayer);
    if (pListPrev->Find(idACD) == NULL)
        pListPrev->AddTail_Tracked(idACD);

    ActorCommonData * ptACD = ACDTryToGet(idACD);
    Actor * ptActor = ActorGet(ptACD ? (ActorID)ptACD->idOwner : ACTORID_INVALID);
    ptActor->dwTrickleBits &= ~(1 << tPlayer.nPlayerIndex);
}







void SPlayerTrickleClearAll(
    const Player & tPlayer)
{
    ServerTrickleList * pList = SPlayerGetPossibleTrickleList(tPlayer);
    if (pList)
        pList->RemoveAll();
}




static SNO sgpLevelAreaErrorScene[32];
static int32 sgnLevelAreaErrorNumErrors;



static void sSendTrickleUpdateMessage(
    const PlayerList & listDestPlayers,
    const Player & tPlayerOwner,
    const ACDID idACD)
{
    ActorCommonData & tACD = ACDGet(idACD);
    ActorCommonData & tACDPlayerListOwner = ACDGet(tPlayerOwner.idPlayerACD);


    TrickleMessage tMessage;
    tMessage.ann = ANNFromACD(tACD.id);
    tMessage.snoActor = tACD.GetActorSNO();
    tMessage.tPlace = tACD.GetPlace();

    LevelAreaID idLevelArea = ACDGetLevelAreaID(tACD);
    LevelArea * ptLevelArea = LevelAreaTryToGet(idLevelArea);
    if (ptLevelArea)
    {
        tMessage.snoLevelArea = ptLevelArea->snoLevelArea;
    }
    else
    {
        NavMeshHandle tNavMeshHandle;
        ACDGetNavMeshHandle(tACD, tNavMeshHandle);
        SceneCommonData * ptSCD = SceneCommonDataTryToGet(tNavMeshHandle.idSScene);
        if (ptSCD && sgnLevelAreaErrorNumErrors < 32)
        {
            int32 nError = 0;
            for (; nError < sgnLevelAreaErrorNumErrors; nError++)
            {

                if (sgpLevelAreaErrorScene[nError] == ptSCD->snoScene)
                    break;
            }

            if (nError >= sgnLevelAreaErrorNumErrors)
            {



                sgpLevelAreaErrorScene[sgnLevelAreaErrorNumErrors] = ptSCD->snoScene;
                sgnLevelAreaErrorNumErrors++;
            }
        }






        tMessage.snoLevelArea = SNO_NONE;
    }

    tMessage.dwTrickleFlags = 0;

    if (!tACD.IsAlive())
    {
        tMessage.dwTrickleFlags |= (1 << TRICKLE_FLAG_DEAD_BIT);
    }

    if (tACD.IsObserver())
    {
        tMessage.dwTrickleFlags |= (1 << TRICKLE_FLAG_OBSERVER_BIT);
    }

    if (tACD.AttributesGetInt(ATTRIBUTE_GHOSTED))
    {
        tMessage.dwTrickleFlags |= (1 << TRICKLE_FLAG_GHOSTED_BIT);
    }

    if (tACD.AttributesGetInt(ATTRIBUTE_PVP_PLAYER_REVEALED_ON_MAP))
    {
        tMessage.dwTrickleFlags |= (1 << TRICKLE_FLAG_PVP_PLAYER_REVEALED_ON_MAP);
    }

    if (tACD.AttributesGetInt(ATTRIBUTE_HUNTER))
    {
        tMessage.dwTrickleFlags |= (1 << TRICKLE_FLAG_HUNTER);
    }

    BOOL bSupressIcon = !tACD.AttributesGetInt(ATTRIBUTE_MINIMAP_ICON_ACTIVE);
    BOOL bBecomesArrow = FALSE;

    float flCurrentHP = tACD.GetHitPointsCurrent();
    float flMaxHP = tACD.GetHitPointsMaxTotal();
    tMessage.flHealthPercent = 0.0f;
    if ((flMaxHP < -EPSILON || flMaxHP > EPSILON) && flCurrentHP != 0.0f)
    {
        tMessage.flHealthPercent.Set(flCurrentHP / flMaxHP);
    }

    Player * ptPlayer = PlayerGetByACD(ACDGetPlayerACDOnSameTeam(tACD.id));
    if (ptPlayer)
    {
        tMessage.nPlayerIndexOwner.Set(ptPlayer->nPlayerIndex);
        tMessage.uEngagedWithRareTime = tACD.AttributesGetInt(ATTRIBUTE_ENGAGED_WITH_RARE_TIME);
        tMessage.uEngagedWithGoblinTime = tACD.AttributesGetInt(ATTRIBUTE_ENGAGED_WITH_TREASURE_GOBLIN_TIME);
        tMessage.bInCombat = tACD.IsInCombat();
        tMessage.nTieredRiftRank = ACDGetTieredLootRunLevel(tACD) + 1;
        tMessage.bBrawling = tACD.IsDueling();
        SNO snoWorld = WorldCommonGetWorldSNO(tACD.GetSWorldID());
        tMessage.bInUberFight = snoWorld == GlobalSNOGet(GLOBALSNO_UBER_BOSS_WORLD_04)
            || snoWorld == GlobalSNOGet(GLOBALSNO_UBER_BOSS_WORLD_01)
            || snoWorld == GlobalSNOGet(GLOBALSNO_UBER_BOSS_WORLD_02)
            || snoWorld == GlobalSNOGet(GLOBALSNO_UBER_BOSS_WORLD_03);
        tMessage.unk_b4 = sub_71007d50c0(snoWorld);
        tMessage.nAction = TRICKLE_ACTION_NONE;
        if (tACD.AttributesGetInt(ATTRIBUTE_BUSY))
        {
            tMessage.nAction = TRICKLE_ACTION_BUSY;
        }
        else if (PowerAPI_BuffFindOnACD(GlobalSNOGet(GLOBALSNO_POWERS_USE_STONE_OF_RECALL), &tACD, ACDID_INVALID) != -1)
        {
            tMessage.nAction = TRICKLE_ACTION_TOWN_PORTAL;
        }
        else if (tACD.AttributesGetInt(ATTRIBUTE_IN_BOSS_ENCOUNTER))
        {
            tMessage.nAction = TRICKLE_ACTION_IN_BOSS_FIGHT;
        }
        else if (tMessage.snoLevelArea != SNO_NONE)
        {
            if (NephalemRift::IsTieredRiftActive())
            {
                tMessage.nAction = TRICKLE_ACTION_DUNGEON_FINDER;
            }
            else if (LevelAreaIsTown(tMessage.snoLevelArea))
            {
                tMessage.nAction = TRICKLE_ACTION_IN_TOWN;
            }
        }

        VisualInventory * ptVisualInventorySource = VisualInventoryGet(ptPlayer->idPlayerACD);
        if (ptVisualInventorySource)
        {
            GBID gbidPortraitFrame = ptVisualInventorySource->tEquipmentCurrent.tCosmeticItems[3].gbidItem;
            if (gbidPortraitFrame != GBID_NONE)
            {
                tMessage.gbidPortraitFrame = gbidPortraitFrame;
            }
        }
    }

    int32 dwHeadstoneCorpseReviveTime = tACD.AttributesGetInt(ATTRIBUTE_CORPSE_RESURRECTION_ALLOWED_GAMETIME);
    if (dwHeadstoneCorpseReviveTime > 0)
    {
        tMessage.dwHeadstoneCorpseReviveTime.Set(dwHeadstoneCorpseReviveTime);
    }

    ScopedActorDefinitionLockR pdefActor(tACD.snoActor);
    const Actor * ptActor = ActorTryToGet((ActorID)tACD.idOwner);
    if (ptActor)
    {
        BOOL bRespectsFoW = ActorGetInheritedTagMapValueInt(ptActor, TAG_ACTOR_MINIMAP_RESPECTS_FOW, TAG_MARKER_MINIMAP_HIDE_UNTIL_EXPLORED);
        bBecomesArrow = !ActorGetInheritedTagMapValueInt(ptActor, TAG_ACTOR_MINIMAP_NO_ARROW, TAG_MARKER_MINIMAP_DOES_NOT_BECOME_ARROW);

        if (ACDIsLootRunBoss(tACD))
        {
            bBecomesArrow = TRUE;
            tMessage.dwTrickleFlags |= (1 << TRICKLE_FLAG_LOOT_RUN_BOSS);
        }


        if (bRespectsFoW) tMessage.dwTrickleFlags |= (1 << TRICKLE_FLAG_RESPECTS_FOW); else tMessage.dwTrickleFlags &= ~(1 << TRICKLE_FLAG_RESPECTS_FOW);

        float fDisplayRangeMin = ActorGetInheritedTagMapValueFloat(ptActor, TAG_ACTOR_MINIMAP_MIN_RANGE, TAG_MARKER_MIN_MINIMAP_RANGE);
        float fDisplayRangeMax = ActorGetInheritedTagMapValueFloat(ptActor, TAG_ACTOR_MINIMAP_MAX_RANGE, TAG_MARKER_MAX_MINIMAP_RANGE);

        if (fDisplayRangeMin > FLT_EPSILON)
        {
            tMessage.fMinDisplayRangeSq.Set(fDisplayRangeMin * fDisplayRangeMin);
        }

        if (fDisplayRangeMax > FLT_EPSILON)
        {
            tMessage.fMaxDisplayRangeSq.Set(fDisplayRangeMax * fDisplayRangeMax);
        }

        SNO snoIcon;
        CRefString sRet;
        Minimap::ResolveIconForActor(tACD, ptActor, &tPlayerOwner, snoIcon, sRet);
        if (snoIcon != SNO_NONE)
        {
            tMessage.snoMinimapIcon.Set(snoIcon);
        }

        if (sRet != "")
        {
            tMessage.hMinimapIconLabel.Set(XHashKey(sRet.str()));
            bSupressIcon = FALSE;
        }

        SNO snoStringList;
        uint32 hStringListHandle;
        Minimap::ResolveMouseOverNameForActor(ptActor, hStringListHandle, snoStringList);
        if (snoStringList != SNO_NONE && hStringListHandle != (uint32)-1)
        {
            tMessage.snoStringList.Set(snoStringList);
            tMessage.hMinimapStringLabel.Set(hStringListHandle);
        }
    }

    if (tACD.AttributesGetInt(ATTRIBUTE_MINIMAP_ICON_DISABLE_ARROW))
        bBecomesArrow = FALSE;



    if ((bSupressIcon || NPCShouldBeHiddenForClient(tACD, &tPlayerOwner)) != FALSE)  // the explicit compare keeps the condition as one value, as in the original (plain `if (a || b)` does not match)

        tMessage.dwTrickleFlags |= (1 << TRICKLE_FLAG_NOT_ON_MAP); else tMessage.dwTrickleFlags &= ~(1 << TRICKLE_FLAG_NOT_ON_MAP);
    if (bBecomesArrow) tMessage.dwTrickleFlags |= (1 << TRICKLE_FLAG_BECOMES_ARROW); else tMessage.dwTrickleFlags &= ~(1 << TRICKLE_FLAG_BECOMES_ARROW);


    if (tACD.IsHidden() || !tACD.IsEnabled() || TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_RENDER_ONLY_IN_EDIT_MODE)) tMessage.dwTrickleFlags |= (1 << TRICKLE_FLAG_HIDDEN); else tMessage.dwTrickleFlags &= ~(1 << TRICKLE_FLAG_HIDDEN);



    tMessage.eTrickleInfoType = PlayerGetTrickleInfoType(tACDPlayerListOwner, tACD);


    MessageSendToClient(listDestPlayers, SMSG_TRICKLE_UPDATE, &tMessage, sizeof(tMessage));
}































static float sDistanceSquaredToClosestAlly(
    const Player & tPlayer,
    const WorldPlace & tPlaceCurrent)
{
    float distSquared = 9999.0f;
    ActorCommonData & tACDPlayer = ACDGet(tPlayer.idPlayerACD);

    ServerTrickleList & listTrickle = *SPlayerGetPossibleTrickleList(tPlayer);
    POS posTrickle = listTrickle.GetHeadPosition();
    while (posTrickle)
    {
        ACDID idACD = (ACDID)listTrickle.GetNext(posTrickle);
        ActorCommonData * ptACD = ACDTryToGet(idACD);
        if (ptACD)
        {
            TrickleInfoType eType = PlayerGetTrickleInfoType(tACDPlayer, *ptACD);
            if (eType == TRICKLE_INFO_LOCAL_PLAYER || eType == TRICKLE_INFO_FRIENDLY_PLAYER || eType == TRICKLE_INFO_FRIENDLY_MONSTER)
            {
                float distNew = Vector3D::Distance2DSquared(ptACD->GetPlace().wp, tPlaceCurrent.wp);
                if (distNew < distSquared)
                    distSquared = distNew;
            }
        }
    }




    WorldPlace tPlacePlayer = tACDPlayer.GetPlace();
    float distNew = Vector3D::Distance2DSquared(tPlacePlayer.wp, tPlaceCurrent.wp);
    if (distNew < distSquared)
        distSquared = distNew;
    return distSquared;
}







static BOOL sPlayerShouldSeeTrickledACD(
    const ActorCommonData & tACDPlayer,
    const ActorCommonData & tACD,
    const TrickleInfoType eTrickleType,
    const WorldPlace & tPlaceACD)
{
    if (eTrickleType == TRICKLE_INFO_HOSTILE_PLAYER)
    {


        Player * ptPlayer = PlayerGetByACD(tACDPlayer.id);
        if (!ptPlayer)
            return FALSE;




        if (GameTypeIsPvP())
            return TRUE;












        if (tACDPlayer.IsDueling() || tACD.IsDueling())
            return TRUE;



        float distClosest = sDistanceSquaredToClosestAlly(*ptPlayer, tPlaceACD);
        if (distClosest > 8100.0f)
            return FALSE;
    }







    else if (eTrickleType == TRICKLE_INFO_HOSTILE_MONSTER)
    {
        return tACD.IsAlive();
    }
    return TRUE;
}













// NON_MATCHING: 2 insns; the inlined sSendTrickleUpdateMessage reaches sgpLevelAreaErrorScene at merged-globals offset 0xf0, the target at 0xd0 (file-static layout, not logic)
void SPlayerTrickleUpdate(Player & tPlayer)
{

    ServerPlayerGlobals * ptServerPlayerGlobals = SGameManager::GameGet().ptServerPlayerGlobals;
    if (!ptServerPlayerGlobals)
        return;



    BOOL fSendUpdate = FALSE;

    fSendUpdate = tPlayer.bIsPrimaryPlayerServerOnly;





    PerPlayerGlobals & tPerPlayer = ptServerPlayerGlobals->tPerPlayer[tPlayer.nPlayerIndex];
    ServerTrickleList & listPossible = *tPerPlayer.pListPossibleTrickle;
    ServerTrickleList * pListNewTrickle = tPerPlayer.pListTrickle;
    ServerTrickleList * pListOldTrickle = tPerPlayer.pListTricklePrev;
    ActorCommonData & tACDPlayer = ACDGet(tPlayer.idPlayerACD);


    {
        pListNewTrickle->RemoveAll();
        POS pos = listPossible.GetHeadPosition();
        while (pos)
        {
            ACDID idACD = (ACDID)listPossible.GetNext(pos);
            ActorCommonData * ptACD = ACDTryToGet(idACD);
            if (ptACD)
            {
                TrickleInfoType eTrickleInfoType = PlayerGetTrickleInfoType(tACDPlayer, *ptACD);
                if (ptACD->IsInWorld() && sPlayerShouldSeeTrickledACD(tACDPlayer, *ptACD, eTrickleInfoType, ptACD->GetPlace()))
                {
                    pListNewTrickle->AddTail_Tracked(idACD);
                }
            }
        }
    }

    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetSingle(listDestPlayers, tPlayer);

    {
        POS pos = pListNewTrickle->GetHeadPosition();
        while (pos)
        {
            ACDID idACD = (ACDID)pListNewTrickle->GetNext(pos);

            BOOL bNewTrickle = TRUE;
            POS posOld = pListOldTrickle->Find(idACD);
            if (posOld)
            {






                pListOldTrickle->RemoveAt(posOld);
                bNewTrickle = FALSE;
            }
            if (fSendUpdate && (bNewTrickle || ACDIsPlayer(idACD) || ACDIsPet(idACD) || GameTypeIsPvP() || ACDIsLootRunBoss(ACDGet(idACD))))
            {
                sSendTrickleUpdateMessage(listDestPlayers, tPlayer, idACD);
            }
        }
    }


    if (fSendUpdate)
    {
        PlayerList listDestPlayers2(PlayerListGetElementPool());
        PlayerListGetSingle(listDestPlayers2, tPlayer);

        POS pos = pListOldTrickle->GetHeadPosition();
        while (pos)
        {
            ACDID idACD = (ACDID)pListOldTrickle->GetNext(pos);

            ANNDataMessage tMessage;
            tMessage.ann = ANNFromACD(idACD);
            MessageSendToClient(listDestPlayers2, SMSG_TRICKLE_REMOVE, &tMessage, sizeof(tMessage));
        }
    }


    Math::Swap(tPerPlayer.pListTrickle, tPerPlayer.pListTricklePrev);
}





void SPlayerBootAvenger(
    Player & tPlayer)
{
    PlayerAvengerData * ptAvengerData = GetPlayerAvengerData(&tPlayer, tPlayer.eAvengerSource);
    if (ptAvengerData->bHasAvenger && ptAvengerData->bHasSpawned)
    {
        ACDID idAvenger = ACDFromANN(ptAvengerData->annTarget);
        ActorCommonData * ptACDAvenger = ACDTryToGet(idAvenger);
        if (ptACDAvenger != NULL && ptACDAvenger->IsAlive())
        {
            ptACDAvenger->SetInvulnerable(TRUE);
            Avenger::Console::PlayExitEffect(*ptACDAvenger);
        }
    }
}





void SPlayerMarkConversationAsHeard(
    Player * ptPlayer,
    const SNO snoConversation)
{
    SNOName snoNameConversation(CONVERSATION_SNO, snoConversation);

    if (ptPlayer->listConversations.Find(snoNameConversation) == NULL)
    {
        ptPlayer->listConversations.AddHead_Tracked(snoNameConversation);
    }
}



BOOL SPlayerHasHeardConversation(
    const Player * ptPlayer,
    const SNO snoConv)
{
    BOOL bHeard = ptPlayer->listConversations.Find(SNOName(CONVERSATION_SNO, snoConv)) != NULL;
    return bHeard;
}










void SPlayerClearHeardConversation(
    Player & tPlayer,
    const SNO snoConv)
{
    POS pos = tPlayer.listConversations.Find(SNOName(CONVERSATION_SNO, snoConv));
    if (pos != NULL)
    {
        tPlayer.listConversations.RemoveAt(pos);
    }
}



void SPlayerClearHeardConversations(
    Player & tPlayer)
{
    POS pos = tPlayer.listConversations.GetHeadPosition();
    while (pos != NULL)
    {
        POS posCurrent = pos;
        const SNOName & tConversation = tPlayer.listConversations.GetNext(pos);
        ScopedConversationDefinitionLockR pdefConversation(tConversation.snoHandle);
        if (pdefConversation->eConvType != CONV_TYPE_TALK_MENU && !(pdefConversation->dwFlags & (1 << CF_NEVER_REPEAT_BIT)))
            tPlayer.listConversations.RemoveAt(posCurrent);
    }
}





SNO SPlayerGetActiveSkillInSlot(
    const Player & tPlayer,
    const int32 nSlot)
{
    return tPlayer.tHeroStateData.tSavedData.tActiveSkills[nSlot].snoSkill;
}



int32 SPlayerGetSlotForActiveSkill(
    Player & tPlayer,
    SNO snoPower)
{
    for (int32 i = 0; i < NUM_SKILL_SLOTS; i++)
    {
        ActiveSkillSavedData & tSkillData = tPlayer.tHeroStateData.tSavedData.tActiveSkills[i];
        if (tSkillData.snoSkill == snoPower)
            return i;
    }
    return -1;
}







void SPlayerRemoveActiveSkillInSlot(
    Player & tPlayer,
    const int32 nSlot)
{
    SNO snoPowerInSlot = tPlayer.tHeroStateData.tSavedData.tActiveSkills[nSlot].snoSkill;
    if (snoPowerInSlot != SNO_NONE)
    {
        SPlayerClearRechargeTimer(tPlayer, snoPowerInSlot);
        SPlayerRemoveSkill(tPlayer, snoPowerInSlot, nSlot);
    }
}








































static void sSetRuneForPower(
    ActorCommonData & tACDPlayer,
    const SNO snoPower,
    const RuneType eRune)
{
    tACDPlayer.AttributesSetInt(MakeAttribKey(ATTRIBUTE_RUNE_A, snoPower), eRune == 0);
    tACDPlayer.AttributesSetInt(MakeAttribKey(ATTRIBUTE_RUNE_B, snoPower), eRune == 1);
    tACDPlayer.AttributesSetInt(MakeAttribKey(ATTRIBUTE_RUNE_C, snoPower), eRune == 2);
    tACDPlayer.AttributesSetInt(MakeAttribKey(ATTRIBUTE_RUNE_D, snoPower), eRune == 3);
    tACDPlayer.AttributesSetInt(MakeAttribKey(ATTRIBUTE_RUNE_E, snoPower), eRune == 4);
}



void SPlayerRestoreRunes(
    ActorCommonData & tACDPlayer,
    Player & tPlayer)
{
    for (int32 i = 0; i < 6; i++)
    {
        sSetRuneForPower(tACDPlayer, tPlayer.tHeroStateData.tSavedData.tActiveSkills[i].snoSkill, tPlayer.tHeroStateData.tSavedData.tActiveSkills[i].eRuneType);
    }
}






void SPlayerRestoreCooldowns(
    ActorCommonData & tACDPlayer,
    Player & tPlayer)
{
    ServerPlayerGlobals * ptServerPlayerGlobals = SGameManager::GameGet().ptServerPlayerGlobals;

    uint32 dwNow = GameGetServerTime();

    for (int32 nPlayer = 0; nPlayer < NUM_SAVED_PLAYER_COOLDOWNS; nPlayer++)
    {
        PlayerCooldownData & tCooldownData = ptServerPlayerGlobals->tPlayerCooldownData[nPlayer];

        if (tPlayer.uHeroId == tCooldownData.uHeroId)
        {

            uint32 dwTimePassed = dwNow - tCooldownData.dwLeaveTime;


            for (uint32 nSkill = 0; nSkill < 6; nSkill++)
            {

                SNO snoPower = tCooldownData.arSkillCooldownData[nSkill].snoPower;
                uint32 dwDuration = tCooldownData.arSkillCooldownData[nSkill].dwDuration;
                if (dwTimePassed < dwDuration && PowerIsAssigned(snoPower, &tPlayer))
                {

                    PowerTriggerCooldown(&tACDPlayer, snoPower, 0.0f);


                    uint32 dwReductionTicks = dwTimePassed - dwDuration + PowerGetCastingCooldown(snoPower, tACDPlayer);
                    PowerReduceCooldown(&tACDPlayer, snoPower, dwReductionTicks);
                }
            }


            for (uint32 nBuff = 0; nBuff < tCooldownData.arBuffCooldownData.size(); nBuff++)
            {
                uint32 dwDuration = tCooldownData.arBuffCooldownData[nBuff].dwDuration;
                if (dwDuration > dwTimePassed)
                {
                    SNO snoPower = tCooldownData.arBuffCooldownData[nBuff].snoPower;

                    PowerAPIBuffStartData tBuffData(POWERID_INVALID);
                    tBuffData.dwDurationMin = dwDuration - dwTimePassed;
                    tBuffData.bPositiveEffect = TRUE;
                    tBuffData.bRemoveOnDeath = FALSE;
                    tBuffData.bRemoveOnPassiveApplication = FALSE;
                    tBuffData.bSaveCooldown = TRUE;
                    tBuffData.snoPower = snoPower;
                    tBuffData.nBuffIndex = tCooldownData.arBuffCooldownData[nBuff].nBuffIndex;
                    PowerAPI_BuffAddToACD(tBuffData, tACDPlayer);
                }
            }

            break;
        }
    }
}




void SPlayerSaveCooldowns(
    ActorCommonData & tACDPlayer,
    Player & tPlayer)
{
    ServerPlayerGlobals * ptServerPlayerGlobals = SGameManager::GameGet().ptServerPlayerGlobals;


    ptServerPlayerGlobals->nPlayerCooldownIndex = (ptServerPlayerGlobals->nPlayerCooldownIndex + 1) % NUM_SAVED_PLAYER_COOLDOWNS;



    for (int32 nPlayer = 0; nPlayer < NUM_SAVED_PLAYER_COOLDOWNS; nPlayer++)
    {
        PlayerCooldownData & tCooldownData = ptServerPlayerGlobals->tPlayerCooldownData[nPlayer];
        if (tPlayer.uHeroId == tCooldownData.uHeroId)
        {
            tCooldownData.Clear();
        }
    }

    uint32 dwNow = GameGetServerTime();

    PlayerCooldownData & tCooldownData = ptServerPlayerGlobals->tPlayerCooldownData[ptServerPlayerGlobals->nPlayerCooldownIndex];
    tCooldownData.uHeroId = tPlayer.uHeroId;
    tCooldownData.dwLeaveTime = dwNow;


    for (int32 nSkill = 0; nSkill < 6; nSkill++)
    {
        SNO snoPower = tPlayer.tHeroStateData.tSavedData.tActiveSkills[nSkill].snoSkill;
        tCooldownData.arSkillCooldownData[nSkill].snoPower = snoPower;


        tCooldownData.arSkillCooldownData[nSkill].dwDuration = 0;
        if (snoPower != SNO_NONE)
        {
            uint32 dwPowerCoolDown = tACDPlayer.AttributesGetInt(MakeAttribKey(ATTRIBUTE_POWER_COOLDOWN, snoPower));
            if (dwPowerCoolDown != (uint32)-1 && dwPowerCoolDown > dwNow)
            {
                tCooldownData.arSkillCooldownData[nSkill].dwDuration = dwPowerCoolDown - dwNow;
            }
        }
    }


    tCooldownData.arBuffCooldownData.clear();

    POS posBuff = tACDPlayer.listBuffs.GetHeadPosition();
    while (posBuff)
    {
        BuffID idBuff = tACDPlayer.listBuffs.GetNext(posBuff);

        PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
        if (ptBuff &&

            !ptBuff->bDeleted &&
            ptBuff->bSaveCooldown)
        {
            BuffCooldownData tBuffCooldownData;
            tBuffCooldownData.snoPower = ptBuff->snoPower;
            tBuffCooldownData.nBuffIndex = ptBuff->nBuffIndex;
            tBuffCooldownData.dwDuration = ptBuff->dwDuration - dwNow + ptBuff->dwStartTick;

            tCooldownData.arBuffCooldownData.push_back(tBuffCooldownData);
        }
    }
}



void SPlayerAssignToActiveSkillSlot(
    Player & tPlayer,
    const SNO snoPower,
    const RuneType eRune,
    const int32 nSlot,
    BOOL bApplyCooldown)
{
    PlayerAssignToActiveSkillSlot(tPlayer, snoPower, eRune, nSlot);
    ActorCommonData & tACDPlayer = ACDGet(tPlayer.idPlayerACD);
    sSetRuneForPower(tACDPlayer, snoPower, eRune);
    tACDPlayer.ApplyAllPassives(FALSE);

    if (snoPower != SNO_NONE && bApplyCooldown)
    {
        PowerHelper_AddCooldown(tACDPlayer, snoPower, 0, TRUE);
        SNO snoSynergyPower = PowerGeneralTagGetSNO(AutoPowerDefinitionR(snoPower), TAG_POWER_SYNERGY_POWER);
        if (snoSynergyPower != SNO_NONE)
            PowerHelper_AddCooldown(tACDPlayer, snoSynergyPower, 0, TRUE);
    }

    int32 nMaxCharges = PowerActorGetMaxCharges(tACDPlayer, snoPower);
    int32 nClampedCharges = Math::Min(nMaxCharges, 99);
    if (nMaxCharges > 0)
    {
        SPlayerClearRechargeTimer(tPlayer, snoPower);
        int32 nCharges = tACDPlayer.AttributesGetInt(MakeAttribKey(ATTRIBUTE_SKILL_CHARGES, snoPower));
        if (nClampedCharges > nCharges)
            SPlayerRegisterPowerForRecharge(PlayerGetByACD(tPlayer.idPlayerACD), snoPower);
        else if (nCharges > nClampedCharges)
            tACDPlayer.AttributesSetInt(MakeAttribKey(ATTRIBUTE_SKILL_CHARGES, snoPower), nClampedCharges);
    }

    PlayerList listDestPlayers(PlayerListGetElementPool());
    listDestPlayers.AddTail_Tracked(&tPlayer);
    AssignSkillMessage tMessage;
    SigmaMemoryClear(&tMessage, sizeof(tMessage));
    tMessage.snoPower = snoPower;
    tMessage.nRuneType = eRune;
    tMessage.nSlot = nSlot;
    tMessage.nPlayerIndex = tPlayer.nPlayerIndex;
    MessageSendToClient(listDestPlayers, SMSG_ASSIGNED_SKILL_TO_SLOT, &tMessage, sizeof(tMessage));
}
































void SPlayerSwitchActiveSkills(
    Player & tPlayer,
    const SNO snoPower,
    const RuneType eRuneType,
    const int32 nSlot,
    const BOOL bApplyCooldown)
{
    int32 nSlotOfExistingSkill;
    if (PlayerAlreadyPurchasedActiveSkill(tPlayer, snoPower, nSlotOfExistingSkill))
    {
        if (nSlotOfExistingSkill != nSlot)
            SPlayerRemoveActiveSkillInSlot(tPlayer, nSlotOfExistingSkill);
    }
    SPlayerRemoveActiveSkillInSlot(tPlayer, nSlot);
    if (snoPower != SNO_NONE)
    {
        ActorCommonData & tACDPlayer = ACDGet(tPlayer.idPlayerACD);
        PlayerBrainSpendActiveSkillPoint(tACDPlayer, snoPower);
        SPlayerAssignToActiveSkillSlot(tPlayer, snoPower, eRuneType, nSlot, bApplyCooldown);
        HotbarButtonData tButtonData;
        tButtonData.SetSkill(snoPower, eRuneType);
        PlayerBrainChangeHotbarButtons(tPlayer, (SkillButton)nSlot, tButtonData);
    }
}









void SPlayerSwapActiveSkillSlots(
    Player & tPlayer,
    const int32 nSlot0,
    const int32 nSlot1)
{
    ActiveSkillSavedData & tSkill0 = tPlayer.tHeroStateData.tSavedData.tActiveSkills[nSlot0];
    ActiveSkillSavedData & tSkill1 = tPlayer.tHeroStateData.tSavedData.tActiveSkills[nSlot1];
    ActiveSkillSavedData tTempSkill = tSkill0;
    tSkill0 = tSkill1;
    tSkill1 = tTempSkill;

    HotbarButtonData & tButton0 = tPlayer.tHeroStateData.tSavedData.tHotbarBtnAssignments[nSlot0];
    HotbarButtonData & tButton1 = tPlayer.tHeroStateData.tSavedData.tHotbarBtnAssignments[nSlot1];
    if (tButton0.m_snoPower == SNO_NONE || PowerIsBasicAttack(AutoPowerDefinitionR(tButton0.m_snoPower)))
        tButton0.Empty();
    if (tButton1.m_snoPower == SNO_NONE || PowerIsBasicAttack(AutoPowerDefinitionR(tButton1.m_snoPower)))
        tButton1.Empty();
    HotbarButtonData tTempButton = tButton0;
    tButton0 = tButton1;
    tButton1 = tTempButton;

    PlayerList listDestPlayers(PlayerListGetElementPool());
    listDestPlayers.AddTail_Tracked(&tPlayer);
    AssignSkillMessage tMessage;
    SigmaMemoryClear(&tMessage, sizeof(tMessage));
    tMessage.snoPower = tSkill0.snoSkill;
    tMessage.nRuneType = tPlayer.tHeroStateData.tSavedData.tActiveSkills[nSlot0].eRuneType;
    tMessage.nPlayerIndex = tPlayer.nPlayerIndex;
    tMessage.nSlot = nSlot0;
    MessageSendToClient(listDestPlayers, SMSG_ASSIGNED_SKILL_TO_SLOT, &tMessage, sizeof(tMessage));
    SigmaMemoryClear(&tMessage, sizeof(tMessage));
    tMessage.snoPower = tSkill1.snoSkill;
    tMessage.nRuneType = tPlayer.tHeroStateData.tSavedData.tActiveSkills[nSlot1].eRuneType;
    tMessage.nPlayerIndex = tPlayer.nPlayerIndex;
    tMessage.nSlot = nSlot1;
    MessageSendToClient(listDestPlayers, SMSG_ASSIGNED_SKILL_TO_SLOT, &tMessage, sizeof(tMessage));

    ActorCommonData & tACDPlayer = ACDGet(tPlayer.idPlayerACD);
    tACDPlayer.MarkHeroStateSavedDataChanged();
    ServerQueueClientUpdate();
    ACDInventoryEquipmentOrSkillChanged(tACDPlayer);
}
























void SPlayerRemoveTraitInSlot(Player & tPlayer, const int32 nSlot)
{
    SNO snoPower = tPlayer.tHeroStateData.tSavedData.snoTraits[nSlot];
    if (snoPower == SNO_NONE)
        return;
    ActorCommonData & tACDPlayer = ACDGet(tPlayer.idPlayerACD);
    tACDPlayer.AttributesSetInt(MakeAttribKey(ATTRIBUTE_SKILL, snoPower), 0);
    sSPlayerRemoveTrait(tPlayer, snoPower, nSlot);
    SPlayerRemoveBuffsAndPetsForRespec(tACDPlayer, snoPower);
    ACDInventoryEquipmentOrSkillChanged(tACDPlayer);
}


void SPlayerAssignToTraitSlot(
    Player & tPlayer,
    const SNO snoPower,
    const int32 nSlot,
    BOOL bApplyCooldown)
{
    PlayerAssignToTraitSlot(tPlayer, snoPower, nSlot);
    if (snoPower != SNO_NONE && bApplyCooldown)
    {
        ActorCommonData & tACDPlayer = ACDGet(tPlayer.idPlayerACD);
        PowerHelper_AddCooldown(tACDPlayer, snoPower, 0, TRUE);
    }

    PlayerList listDestPlayers(PlayerListGetElementPool());
    listDestPlayers.AddTail_Tracked(&tPlayer);
    AssignSkillMessage tMessage;
    SigmaMemoryClear(&tMessage, sizeof(tMessage));
    tMessage.snoPower = snoPower;
    tMessage.nSlot = nSlot;
    tMessage.nPlayerIndex = tPlayer.nPlayerIndex;
    MessageSendToClient(listDestPlayers, SMSG_ASSIGNED_TRAIT_TO_SLOT, &tMessage, sizeof(tMessage));
}







void SPlayerRemoveLegendaryPowerInSlot(
    Player & tPlayer,
    const int32 nSlot)
{
    SPlayerAssignToLegendaryPowerSlot(tPlayer, SNO_NONE, nSlot);
}



void SPlayerAssignToLegendaryPowerSlot(
    Player & tPlayer,
    const GBID gbidLegendaryItem,
    const int32 nSlot)
{
    if (tPlayer.tHeroStateData.tSavedData.gbidLegendaryPowers[nSlot] == gbidLegendaryItem)
        return;

    if (gbidLegendaryItem != GBID_NONE)
    {
        for (int32 i = 0; i < 3; i++)
        {
            if (tPlayer.tHeroStateData.tSavedData.gbidLegendaryPowers[i] == gbidLegendaryItem)
                SPlayerRemoveLegendaryPowerInSlot(tPlayer, i);
        }
    }

    PlayerAssignToLegendaryPowerSlot(tPlayer, gbidLegendaryItem, nSlot);
    ActorCommonData & tACDPlayer = ACDGet(tPlayer.idPlayerACD);
    ACDInventoryEquipmentOrSkillChanged(tACDPlayer);
    tACDPlayer.ApplyAllPassives(FALSE);

    AssignLegendaryPowerMessage tMessage;
    SigmaMemoryClear(&tMessage, sizeof(tMessage));
    tMessage.nPlayerIndex = tPlayer.nPlayerIndex;
    tMessage.gbidLegendaryItem = gbidLegendaryItem;
    tMessage.nSlot = nSlot;
    PlayerList listDestPlayers(PlayerListGetElementPool());
    listDestPlayers.AddTail_Tracked(&tPlayer);
    MessageSendToClient(listDestPlayers, SMSG_ASSIGNED_LEGENDARY_POWER_TO_SLOT, &tMessage, sizeof(tMessage));

    if (tPlayer.tHeroStateData.tSavedData.gbidLegendaryPowers[0] != GBID_NONE &&
        tPlayer.tHeroStateData.tSavedData.gbidLegendaryPowers[1] != GBID_NONE &&
        tPlayer.tHeroStateData.tSavedData.gbidLegendaryPowers[2] != GBID_NONE)
    {
        Achievement::EvaluationData tEvaluationData;
        tEvaluationData.SetPlayer(&tPlayer);
        tEvaluationData.EvaluateEvent(&tPlayer, Achievement::ACH_EVENT_EQUIP_THREE_CUBE_POWERS, 1, (uint32)sub_71007b5e70());
    }
}





























void SPlayerSwitchTraits(
    Player & tPlayer,
    const SNO snoPower,
    const int32 nSlot,
    const BOOL bApplyCooldown)
{
    SPlayerRemoveTraitInSlot(tPlayer, nSlot);
    if (snoPower != SNO_NONE)
    {
        ActorCommonData & tACDPlayer = ACDGet(tPlayer.idPlayerACD);
        PlayerBrainSpendTraitPoint(tACDPlayer, snoPower);
        SPlayerAssignToTraitSlot(tPlayer, snoPower, nSlot, bApplyCooldown);
    }
}
















void SPlayerSkillSlotMarkAsAssigned(
    Player & tPlayer,
    const SkillButton eButton)
{
    tPlayer.tHeroStateData.tSavedData.bSkillSlotEverAssigned |= (1 << eButton);
}






int32 SPlayerGetEmptyActiveSkillSlotForPower(
    Player & tPlayer,
    const SNO snoPower)
{

    for (int32 i = 0; i < NUM_SKILL_SLOTS; i++)
    {
        if (tPlayer.tHeroStateData.tSavedData.tActiveSkills[i].snoSkill == snoPower)
            return -1;
    }




    for (int32 i = 0; i < NUM_SKILL_SLOTS; i++)
    {

        if (tPlayer.tHeroStateData.tSavedData.tActiveSkills[i].snoSkill == SNO_NONE &&
            PowerIsValidForSlot(AutoPowerDefinitionR(snoPower), i))
            return i;
    }

    return -1;
}






void SPlayerAssignToEmptyActiveSkillSlot(
    Player & tPlayer,
    const SNO snoPower)
{
    int32 nSlot = SPlayerGetEmptyActiveSkillSlotForPower(tPlayer, snoPower);
    if (nSlot != -1)
    {
        SPlayerAssignToActiveSkillSlot(tPlayer, snoPower, -1, nSlot, FALSE);
        HotbarButtonData tButtonData;
        tButtonData.SetSkill(snoPower, -1);
        PlayerSetHotbarBtnAssignment(&tPlayer, nSlot, tButtonData);
        if (snoPower != SNO_NONE)
            SPlayerSkillSlotMarkAsAssigned(tPlayer, nSlot);
    }
}










inline int32 SPlayerGetEmptyTraitSlotForPower(
    Player & tPlayer,
    const SNO snoPower)
{
    for (int32 i = 0; i < 4; i++)
    {
        if (tPlayer.tHeroStateData.tSavedData.snoTraits[i] == snoPower)
            return -1;
    }
    for (int32 i = 0; i < 4; i++)
    {
        if (tPlayer.tHeroStateData.tSavedData.snoTraits[i] == SNO_NONE)
            return i;
    }
    return -1;
}











void SPlayerAssignToEmptyTraitSlot(
    Player & tPlayer,
    const SNO snoPower)
{
    int32 nSlot = SPlayerGetEmptyTraitSlotForPower(tPlayer, snoPower);
    if (nSlot != -1)
        SPlayerAssignToTraitSlot(tPlayer, snoPower, nSlot, FALSE);
}







void SPlayerAutoPurchaseSkillForNewLevel(
    ActorCommonData & tACD,
    Player & tPlayer,
    int32 nLevel)
{
    BOOL bUnlocksSlot = FALSE;
    for (int32 i = 0; i < NUM_SKILL_SLOTS; i++)
    {
        if (cgnActiveSkillSlotLevelReq[i] == nLevel)
        {
            bUnlocksSlot = TRUE;
            break;
        }
    }
    if (!bUnlocksSlot)
        return;
    SNO snoSkillKit = tACD.GetSkillKit();
    if (snoSkillKit == SNO_NONE)
        return;

    ScopedSkillKitDefinitionLockR pdefSkillKit(snoSkillKit);
    uint32 nCount = pdefSkillKit->serActiveSkillEntries.dwSize / sizeof(ActiveSkillEntry);
    for (uint32 i = 0; i < nCount; i++)
    {
        const ActiveSkillEntry & tEntry = pdefSkillKit->arActiveSkillEntries[i];
        if (tEntry.snoPower == SNO_NONE || tEntry.nMasteryReq != nLevel)
            continue;
        if (PowerIsDisplayed(AutoPowerDefinitionR(tEntry.snoPower)))
        {
            int32 nSlot = SPlayerGetEmptyActiveSkillSlotForPower(tPlayer, tEntry.snoPower);
            if (nSlot != -1)
                SPlayerSwitchActiveSkills(tPlayer, tEntry.snoPower, RUNE_TYPE_INVALID, nSlot, FALSE);
            return;
        }
    }
}




void SPlayerRemoveSkill(
    Player & tPlayer,
    SNO snoPower,
    int32 nSlot)
{
    ActorCommonData & tACDPlayer = ACDGet(tPlayer.idPlayerACD);
    tACDPlayer.AttributesSetInt(MakeAttribKey(ATTRIBUTE_SKILL, snoPower), 0);
    tACDPlayer.AttributesSetInt(MakeAttribKey(ATTRIBUTE_RUNE_A, snoPower), 0);
    tACDPlayer.AttributesSetInt(MakeAttribKey(ATTRIBUTE_RUNE_B, snoPower), 0);
    tACDPlayer.AttributesSetInt(MakeAttribKey(ATTRIBUTE_RUNE_C, snoPower), 0);
    tACDPlayer.AttributesSetInt(MakeAttribKey(ATTRIBUTE_RUNE_D, snoPower), 0);
    tACDPlayer.AttributesSetInt(MakeAttribKey(ATTRIBUTE_RUNE_E, snoPower), 0);

    SNO snoPassivePower = PowerGeneralTagGetSNO(AutoPowerDefinitionR(snoPower), TAG_POWER_SYNERGY_POWER);
    if (snoPassivePower != SNO_NONE)
    {
        tACDPlayer.AttributesSetInt(MakeAttribKey(ATTRIBUTE_SKILL, snoPassivePower), 0);
        tACDPlayer.AttributesSetInt(MakeAttribKey(ATTRIBUTE_RUNE_A, snoPassivePower), 0);
        tACDPlayer.AttributesSetInt(MakeAttribKey(ATTRIBUTE_RUNE_B, snoPassivePower), 0);
        tACDPlayer.AttributesSetInt(MakeAttribKey(ATTRIBUTE_RUNE_C, snoPassivePower), 0);
        tACDPlayer.AttributesSetInt(MakeAttribKey(ATTRIBUTE_RUNE_D, snoPassivePower), 0);
        tACDPlayer.AttributesSetInt(MakeAttribKey(ATTRIBUTE_RUNE_E, snoPassivePower), 0);
    }

    if (nSlot < 0)
    {

        for (int32 i = 0; i < 6; i++)
        {
            if (tPlayer.tHeroStateData.tSavedData.tActiveSkills[i].snoSkill == snoPower)
            {
                SPlayerAssignToActiveSkillSlot(tPlayer, SNO_NONE, -1, i, FALSE);
            }
        }
    }
    else if (nSlot < 6)
    {
        SPlayerClearRechargeTimer(tPlayer, snoPower);

        SPlayerAssignToActiveSkillSlot(tPlayer, SNO_NONE, -1, nSlot, FALSE);
    }


    for (int32 i = 0; i < 7; i++)
    {
        const HotbarButtonData & tButtonData = PlayerGetHotbarBtnAssignment(&tPlayer, i);

        if (tButtonData.m_snoPower == snoPower)
        {
            HotbarButtonData tNewButtonData;
            tNewButtonData.Empty();
            PlayerSetHotbarBtnAssignment(&tPlayer, i, tNewButtonData);
        }
    }

    SPlayerRemoveBuffsAndPetsForRespec(tACDPlayer, snoPower);

    ACDInventoryEquipmentOrSkillChanged(tACDPlayer);
}




static void sSPlayerRemoveTrait(
    Player & tPlayer,
    SNO snoPower,
    int32 nSlot)
{
    if (nSlot < 0)
    {
        for (int32 i = 0; i < NUM_TRAIT_SLOTS; i++)
        {
            if (tPlayer.tHeroStateData.tSavedData.snoTraits[nSlot] == snoPower)
                SPlayerAssignToTraitSlot(tPlayer, SNO_NONE, i, FALSE);
        }
    }
    else if (nSlot < NUM_TRAIT_SLOTS)
    {
        SPlayerAssignToTraitSlot(tPlayer, SNO_NONE, nSlot, FALSE);
    }
}














void SPlayerSpendParagonPoints(
    Player & tPlayer,
    GBID gbidParagonBonus,
    int32 nPoints)
{
    ActorCommonData & tACDPlayer = ACDGet(tPlayer.idPlayerACD);
    if (!GBIDExists(gbidParagonBonus, 40 /* GB_PARAGON_BONUSES */) || GameIsWeeklyChallenge())
        return;

    ScopedParagonBonusLockR ptParagonBonus(gbidParagonBonus);
    int32 nPointsAvailable = tACDPlayer.AttributesGetInt(MakeAttribKey(ATTRIBUTE_PARAGON_BONUS_POINTS_AVAILABLE, ptParagonBonus->nCategory));
    if (nPoints <= nPointsAvailable)
    {
        FastAttribKey tKey = MakeAttribKey(ATTRIBUTE_PARAGON_BONUS, gbidParagonBonus);
        int32 nCurrentPoints = tACDPlayer.AttributesGetInt(tKey);
        if (nCurrentPoints + nPoints <= ptParagonBonus->nPointsMax)
        {
            tACDPlayer.AttributesSetInt(tKey, nCurrentPoints + nPoints);
            tACDPlayer.AttributesSetInt(MakeAttribKey(ATTRIBUTE_PARAGON_BONUS_POINTS_AVAILABLE, ptParagonBonus->nCategory), nPointsAvailable - nPoints);
            SPlayerApplyParagonBuff(tACDPlayer);
        }
    }
}


















void SPlayerInitializeParagonPoints(
    Player & tPlayer)
{
    ActorCommonData & tACDPlayer = ACDGet(tPlayer.idPlayerACD);

    for (int32 i = 0; i < 4; i++)
    {
        int32 nPointsAvailable = PlayerGetParagonPointsTotalForCategory(tACDPlayer, i);

        GBHandleList listBonuses(GBGetHandlePool());
        GBEnumerate(GB_PARAGON_BONUSES, listBonuses);

        POS pos = listBonuses.GetHeadPosition();
        while (pos)
        {
            GBHandle & hGBBonus = listBonuses.GetNext(pos);
            ScopedParagonBonusLockR pdefParagonBonus(hGBBonus.gbid);
            if (pdefParagonBonus->nCategory == i)
            {
                nPointsAvailable -= tACDPlayer.AttributesGetInt(MakeAttribKey(ATTRIBUTE_PARAGON_BONUS, hGBBonus.gbid));
            }
        }

        if (GameIsWeeklyChallenge())
        {
            nPointsAvailable = 0;
        }

        tACDPlayer.AttributesSetInt(MakeAttribKey(ATTRIBUTE_PARAGON_BONUS_POINTS_AVAILABLE, i), nPointsAvailable);
    }
}







void SPlayerResetParagonPoints(
    Player & tPlayer,
    int32 nCategory)
{
    ActorCommonData & tACDPlayer = ACDGet(tPlayer.idPlayerACD);

    GBHandleList listBonuses(GBGetHandlePool());
    GBEnumerate(GB_PARAGON_BONUSES, listBonuses);

    POS pos = listBonuses.GetHeadPosition();
    while (pos)
    {
        GBHandle & hGBBonus = listBonuses.GetNext(pos);
        ScopedParagonBonusLockR pdefParagonBonus(hGBBonus.gbid);
        if (pdefParagonBonus->nCategory == nCategory)
        {
            tACDPlayer.AttributesSetInt(MakeAttribKey(ATTRIBUTE_PARAGON_BONUS, hGBBonus.gbid), 0);
        }
    }

    SPlayerInitializeParagonPoints(tPlayer);
    SPlayerApplyParagonBuff(tACDPlayer);
}








BOOL SPlayerPurchaseParagonPointsReset(
    Player & tPlayer,
    int32 nCategory)
{



    ActorCommonData & tACDPlayer = ACDGet(tPlayer.idPlayerACD);
    CurrencyType eCurrencyType = CURRENCY_GOLD;
    int32 nCost;
    {

        ScopedGlobalDefinitionLockR pdefGlobals;
        nCost = pdefGlobals->nParagonCategoryResetCost;
    }
    if (!tACDPlayer.CanAffordCurrencyCost(nCost, eCurrencyType))
        return FALSE;
    if (!tACDPlayer.ModifyCurrencyAmount(-nCost, eCurrencyType, GMR_RESET_PARAGON_POINTS, FALSE))
        return FALSE;
    SPlayerResetParagonPoints(tPlayer, nCategory);
    return TRUE;
}











void SPlayerUpdate(
    ActorCommonData & tACDPlayer)
{
    Player * ptPlayer = PlayerGetByACD(tACDPlayer.id);
    if (!ptPlayer || !PlayerIsInWorld(*ptPlayer))
    {
        return;
    }


    float flDPS = PlayerGetRecordedDPS(tACDPlayer.id);
    tACDPlayer.AttributesSetFloat(ATTRIBUTE_DPS, flDPS);


    SPlayerUpdateCombatStatus(*ptPlayer);


    float flHPPercent = tACDPlayer.GetHitpointPercent();
    if (flHPPercent > 0.999999f)
    {
        ptPlayer->uServerFlags &= ~(1 << PSF_INELIGIBLE_FOR_SURVIVOR_AWARD);
    }


    SPlayerUpdateKillCounters(*ptPlayer);
    SPlayerUpdateCurrencyCounter(*ptPlayer, CURRENCY_GOLD);

    PowerAPI_RecomputeHitpointHealTarget(&tACDPlayer);



























    PowerID idPowerPrimary = ActorPowersGetPrimary((ActorID)ACDGetOwner(tACDPlayer.id));
    Power * ptPower = PowerTryToGet(idPowerPrimary);
    if (ptPower)
    {



        SNO snoDashingStrike = GlobalSNOGet(GLOBALSNO_POWERS_MONK_DASHING_STRIKE);
        SNO snoFuriousCharge = GlobalSNOGet(GLOBALSNO_POWERS_BARBARIAN_FURIOUS_CHARGE);
        const uint32 dwMaxSpecialWalkDurTicks = 180;

        if (ptPower->snoPower == snoDashingStrike || ptPower->snoPower == snoFuriousCharge)
        {
            uint32 dwNow = GameGetServerTime();
            uint32 dwPowerStart = ptPower->dwStartTick;

            uint32 dwPowerDuration = dwNow - dwPowerStart;
            if (dwPowerDuration >= dwMaxSpecialWalkDurTicks)
            {
                ActorMovement * ptMove = tACDPlayer.GetActorMovement();
                if (ptMove)
                {
                    BOOL bWantSpew = s_varDashingStrikeStuckMaxDurSpew.Get();
                    if (bWantSpew)
                    {
                        WorldPlace wpCur;
                        ptMove->GetPlace(wpCur);
                        WorldPlace wpDest;

                        Vector3D vVelocity = ptMove->GetVelocity();
                    }



















                    ptMove->StopNow();
                }

                PowerInterrupt(ptPower->id, TRUE);
            }
        }
    }



    ActorCommonData * ptACDTalkingTo = ACDTryToGet(ptPlayer->tTalkData.idACDTalkingTo);
    if (ptACDTalkingTo)
    {
        if (!ACDWithinDistance(*ptACDTalkingTo, tACDPlayer, MAX_CONVERSATION_DISTANCE))
        {
            STalkForceDoneTalking(tACDPlayer.id);
        }
    }

    if (ptPlayer->bCosmeticPetMissing)
    {
        ptPlayer->bCosmeticPetMissing = FALSE;
        GBID gbidPet = tACDPlayer.AttributesGetInt(ATTRIBUTE_CURRENT_COSMETIC_PET);
        if (gbidPet != GBID_NONE)
        {
            SPlayerActivatePet(*ptPlayer, tACDPlayer.AttributesGetInt(ATTRIBUTE_CURRENT_COSMETIC_PET));
        }
    }


    sUpdateCurseCounts();
}



void SPlayerClearRechargeTimer(
    Player & tPlayer,
    SNO snoPower)
{
    ActorCommonData * ptACDPlayer = ACDTryToGet(tPlayer.idPlayerACD);
    if (ptACDPlayer == NULL)
        return;
    if (PowerActorGetMaxCharges(*ptACDPlayer, snoPower) <= 0)
        return;

    FastAttribKey tKey = MakeAttribKey(ATTRIBUTE_NEXT_CHARGE_GAINED_TIME, snoPower);
    if (ptACDPlayer->AttributesGetInt(tKey) != 0)
        ptACDPlayer->AttributesSetInt(tKey, 0);

    int32 nSlot = SPlayerGetSlotForActiveSkill(tPlayer, snoPower);
    if (nSlot == -1)
        return;
    TimerUnregister(tPlayer.tHeroStateData.tSavedData.tActiveSkills[nSlot].idChargesTimer);
    tPlayer.tHeroStateData.tSavedData.tActiveSkills[nSlot].idChargesTimer = TIMERID_INVALID;
}














void sPowerRechargeCallback(
    TimerID,
    DataID idOwner,
    void * ptData)
{
    ActorCommonData * ptACD = ACDTryToGet((ACDID)idOwner);
    if (ptACD == NULL)
        return;
    SNO snoPower = (SNO)(intptr_t)ptData;
    int32 nMaxCharges = PowerActorGetMaxCharges(*ptACD, snoPower);
    FastAttribKey tNextChargeKey = MakeAttribKey(ATTRIBUTE_NEXT_CHARGE_GAINED_TIME, snoPower);
    if (nMaxCharges == 0)
    {
        ptACD->AttributesSetInt(tNextChargeKey, 0);
        return;
    }
    if (ptACD->AttributesGetInt(tNextChargeKey) > 0)
    {
        ptACD->AttributesSetInt(tNextChargeKey, 0);
        FastAttribKey tChargesKey = MakeAttribKey(ATTRIBUTE_SKILL_CHARGES, snoPower);
        int32 nCharges = ptACD->AttributesGetInt(tChargesKey);
        if (nCharges < nMaxCharges)
        {
            ptACD->AttributesSetInt(tChargesKey, nCharges + 1);
            if (nCharges + 1 < nMaxCharges)
                SPlayerRegisterPowerForRecharge(PlayerGetByACD(ptACD->id), snoPower);
        }
    }
}














void SPlayerRegisterPowerForRecharge(
    Player * ptPlayer,
    SNO snoPower)
{
    ActorCommonData & tACDPlayer = ACDGet(ptPlayer->idPlayerACD);
    int32 nMaxCharges = PowerActorGetMaxCharges(tACDPlayer, snoPower);
    FastAttribKey tNextChargeKey = MakeAttribKey(ATTRIBUTE_NEXT_CHARGE_GAINED_TIME, snoPower);
    int32 nSlot = SPlayerGetSlotForActiveSkill(*ptPlayer, snoPower);
    if (nMaxCharges <= 0 || nSlot == -1)
        return;

    FastAttribKey tChargesKey = MakeAttribKey(ATTRIBUTE_SKILL_CHARGES, snoPower);
    if (tACDPlayer.AttributesGetInt(tChargesKey) < nMaxCharges)
    {
        if (tACDPlayer.AttributesGetInt(tNextChargeKey) == 0)
        {
            uint32 dwDelay = SPlayerGetPowerChargeTime(ptPlayer, snoPower);
            uint32 dwNow = GameGetServerTime();
            tACDPlayer.AttributesSetInt(tNextChargeKey, dwNow + dwDelay);
            tACDPlayer.AttributesSetInt(MakeAttribKey(ATTRIBUTE_RECHARGE_START_TIME, snoPower), dwNow);
            ptPlayer->tHeroStateData.tSavedData.tActiveSkills[nSlot].idChargesTimer = TimerRegister(ptPlayer->idPlayerACD, TIMER_OWNER_ACD, dwDelay, (void *)(intptr_t)snoPower, sPowerRechargeCallback);
        }
    }
    else
    {
        tACDPlayer.AttributesSetInt(tChargesKey, nMaxCharges);
        tACDPlayer.AttributesSetInt(tNextChargeKey, 0);
        if (ptPlayer->tHeroStateData.tSavedData.tActiveSkills[nSlot].idChargesTimer != TIMERID_INVALID)
        {
            TimerUnregister(ptPlayer->tHeroStateData.tSavedData.tActiveSkills[nSlot].idChargesTimer);
            ptPlayer->tHeroStateData.tSavedData.tActiveSkills[nSlot].idChargesTimer = TIMERID_INVALID;
        }
    }
}























uint32 SPlayerGetPowerChargeTime(
    Player * ptPlayer,
    SNO snoPower)
{
    ActorCommonData & tACDPlayer = ACDGet(ptPlayer->idPlayerACD);

    int32 nMaxCharges = PowerActorGetMaxCharges(tACDPlayer, snoPower);



    if (nMaxCharges <= 0)
        return 0;


    uint32 dwChargeGainTime = PowerActorGetRechargeTime(tACDPlayer, snoPower);


    float flFlatCooldownReduction = tACDPlayer.AttributesGetFloat(MakeAttribKey(ATTRIBUTE_POWER_COOLDOWN_REDUCTION, snoPower)) + tACDPlayer.AttributesGetFloat(ATTRIBUTE_POWER_COOLDOWN_REDUCTION_ALL);
    uint32 dwFlatCooldownReduction = (uint32)(flFlatCooldownReduction * 60.0f);

    dwChargeGainTime -= Math::Min(dwFlatCooldownReduction, dwChargeGainTime);
    float flReductionPercent = PowerGetCoolDownReductionPercent(snoPower, tACDPlayer);


    dwChargeGainTime = (uint32)((float)dwChargeGainTime * (1.0f - flReductionPercent));



    uint32 dwMinCooldownTime = (uint32)(tACDPlayer.AttributesGetFloat(ATTRIBUTE_POWER_COOLDOWN_MIN_TIME) * 60.0f);
    dwChargeGainTime = Math::Max(dwChargeGainTime, dwMinCooldownTime);


    return dwChargeGainTime;
}



void SPlayerUpdateRechargeTimer(
    ACDID idPlayer,
    SNO snoPower,
    float flTimeReduction)
{


    Player * ptPlayer = PlayerGetByACD(idPlayer);

    if (ptPlayer)
    {
        ActorCommonData & tACDPlayer = ACDGet(idPlayer);

        int32 nMaxCharges = PowerActorGetMaxCharges(tACDPlayer, snoPower);

        if (nMaxCharges <= 0)
        {
            return;
        }

        int32 nSlot = SPlayerGetSlotForActiveSkill(*ptPlayer, snoPower);
        if (nSlot == -1)
        {
            return;
        }


        FastAttribKey tNextChargeGainTime = MakeAttribKey(ATTRIBUTE_NEXT_CHARGE_GAINED_TIME, snoPower);
        FastAttribKey tRechargeStartTime = MakeAttribKey(ATTRIBUTE_RECHARGE_START_TIME, snoPower);

        if (tACDPlayer.AttributesGetInt(tNextChargeGainTime) == 0)
        {
            return;
        }

        TimerID idTimer = ptPlayer->tHeroStateData.tSavedData.tActiveSkills[nSlot].idChargesTimer;

        if (idTimer == TIMERID_INVALID)
        {
            return;
        }

        uint32 dwCurrentTime = GameGetServerTime();

        uint32 nTicksRemaining = tACDPlayer.AttributesGetInt(tNextChargeGainTime) - dwCurrentTime;

        TimerUnregister(idTimer);

        uint32 nTimeReductionTicks = (uint32)(flTimeReduction * 60.0f);

        int32 nNewTicksRemaining = nTicksRemaining - nTimeReductionTicks;

        if (nNewTicksRemaining <= 0)
        {
            sPowerRechargeCallback(idTimer, idPlayer, (void *)(intptr_t)snoPower);
            if (nNewTicksRemaining < 0)
            {
                SPlayerUpdateRechargeTimer(idPlayer, snoPower, (float)(-nNewTicksRemaining) / 60.0f);
            }
        }
        else
        {
            tACDPlayer.AttributesSetInt(tNextChargeGainTime, dwCurrentTime + nNewTicksRemaining);

            uint32 dwRechargeStartTime = tACDPlayer.AttributesGetInt(tRechargeStartTime);
            tACDPlayer.AttributesSetInt(tRechargeStartTime, dwRechargeStartTime - nTimeReductionTicks);

            ptPlayer->tHeroStateData.tSavedData.tActiveSkills[nSlot].idChargesTimer = TimerRegister(ptPlayer->idPlayerACD, TIMER_OWNER_ACD, nNewTicksRemaining, (void *)(intptr_t)snoPower,
                sPowerRechargeCallback);
        }
    }
}




void SPlayerInitGameWaypointsFromPlayer(
    const D3::Hero::SavedDefinition & tHeroDefinition)
{
    SGameGlobals & tGameGlobals = SGameGlobalsGet();
    tGameGlobals.dwActivatedWaypoints = tHeroDefinition.saved_data().activated_waypoints();
}



void SPlayerSyncWaypointsToGame(
    Player & tNewPlayer)
{
    tNewPlayer.tHeroStateData.tSavedData.m_ActivatedWaypoints = SGameGlobalsGet().dwActivatedWaypoints;
    Minimap::UpdateWaypointMarkersFromPlayer(tNewPlayer);
}






void SPlayerInitGameSavePointFromPlayer(
    const D3::Hero::SavedDefinition & tHeroDefinition)
{
    SGameGlobals & tSGameGlobals = SGameGlobalsGet();
    tSGameGlobals.tCreatorSavePoint.Import(tHeroDefinition.saved_data().save_point());
}



void SPlayerAllPlayersLearnLore(
    const SNO snoLore)
{
    PlayerList listPlayers(PlayerListGetElementPool());
    PlayerListGetAllConnected(listPlayers, -1, FALSE);
    for (POS pos = listPlayers.GetHeadPosition(); pos != NULL; )
    {
        Player * ptPlayer = listPlayers.GetNext(pos);
        SPlayerLearnLore(ptPlayer->idPlayerACD, snoLore);
    }
}




void SPlayerLearnLore(
    const ACDID idACDPlayer,
    const SNO snoLore)
{
    if (snoLore == SNO_NONE)
        return;

    if (GameIsWeeklyChallenge())
        return;

    Player * ptOriginalPlayer = PlayerGetByACD(idACDPlayer);
    if (!ptOriginalPlayer)
        return;

    PlayerList listDestPlayers(PlayerListGetElementPool());

    for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {
        if (!PlayersAreLocalToEachOther(*ptPlayer, *ptOriginalPlayer))
            continue;

        if (ptPlayer->tHeroStateData.tSavedData.tLearnedLore.Find(snoLore) != -1)
            continue;

        listDestPlayers.AddTail_Tracked(ptPlayer);

        ptPlayer->tHeroStateData.tSavedData.tLearnedLore.Add(snoLore);

        ScopedLoreDefinitionLockR pdefLore(snoLore);
        if (pdefLore->nXPLevel > 0)
        {
            ScopedGameBalanceDefinitionLockR ptGameBalDef(GlobalSNOGet(GLOBALSNO_GB_EXPERIENCE));
            const ExperienceTable & tExpTable = ptGameBalDef->tExperienceTable;
            ActorCommonData & tACDPlayer = ACDGet(ptPlayer->idPlayerACD);
            int32 nXPAmount = tExpTable.ptExperienceLevels[tACDPlayer.AttributesGetInt(ATTRIBUTE_LEVEL) - 1].nLoreTierXP[pdefLore->nXPTier];
            BOOL fAwardedToPlayer = ExperienceGrantDirectly(tACDPlayer, nXPAmount, TRUE, FALSE);
            if (fAwardedToPlayer)
            {
                FloatingAmountMessage tMessage;
                tMessage.wpPlace = tACDPlayer.GetWorldPlaceCenter();
                tMessage.eType = FLOATING_NUMBER_TYPE_XP;
                tMessage.nAmount = nXPAmount;
                MessageSendToClient(listDestPlayers, SMSG_FLOATING_AMOUNT, &tMessage, sizeof(tMessage));
            }
        }

        {
            Achievement::EvaluationData tEvaluationData;
            tEvaluationData.SetPlayer(ptPlayer);
            tEvaluationData.EvaluateEvent(ptPlayer, Achievement::ACH_EVENT_LORE_LEARNED, pdefLore->snoConversation, 1, (uint32)sub_71007b5e70());
        }
    }

    LoreMessage tMessage;
    tMessage.snoLore = snoLore;
    MessageSendToClient(listDestPlayers, SMSG_LORE_LEARNED, &tMessage, sizeof(tMessage));
}





























BOOL SPlayerHasLearnedLore(
    const ACDID idACDPlayer,
    const SNO snoLore)
{
    if (GameIsWeeklyChallenge())
        return TRUE;
    if (snoLore == SNO_NONE)
        return FALSE;
    Player * ptPlayer = PlayerGetByACD(idACDPlayer);
    if (ptPlayer == NULL)
        return FALSE;
    if (ptPlayer->tHeroStateData.tSavedData.tLearnedLore.Find(snoLore) != -1)
        return TRUE;
    return FALSE;
}

















void SPlayerSaveConversation(
    const PlayerList & listDestPlayers,
    const SNO snoConv,
    const BOOL bPlayed)
{
    if (snoConv == SNO_NONE)
        return;

    POS pos = listDestPlayers.GetHeadPosition();
    while (pos != NULL)
    {
        Player * ptPlayer = listDestPlayers.GetNext(pos);
        ptPlayer->tHeroStateData.tSavedData.tSavedConversations.Save(snoConv, bPlayed);
        ptPlayer->uConsoleSyncFlags |= (1 << PCSF_SAVED_CONVERSATIONS);
    }
}










void SPlayerResetInactionTime(
    const ACDID idACDPlayer)
{
    ActorCommonData & tACDPlayer = ACDGet(idACDPlayer);
    uint32 dwNow = GameGetServerTime();
    tACDPlayer.AttributesSetInt(ATTRIBUTE_LAST_ACTION_TIMESTAMP, dwNow);
    tACDPlayer.AttributesSetInt(ATTRIBUTE_BUSY, 0);
}




float SPlayerGetAFKTime(
    const ACDID idACDPlayer)
{
    Player * ptPlayer = PlayerGetByACD(idACDPlayer);
    if (ptPlayer == NULL)
        return 0.0f;
    uint32 dwNow = GameGetServerTime();
    return (float)(dwNow - ptPlayer->dwLastInputTime) * OO_TICKS_PER_SECOND;
}







float SPlayerGetInactionTime(
    const ACDID idACDPlayer)
{
    ActorCommonData & tACDPlayer = ACDGet(idACDPlayer);
    uint32 dwNow = GameGetServerTime();
    uint32 dwLastActionTime = tACDPlayer.AttributesGetInt(ATTRIBUTE_LAST_ACTION_TIMESTAMP);
    return (float)(dwNow - dwLastActionTime) * OO_TICKS_PER_SECOND;
}




BOOL SPlayerCanSwitchToNewWorld(
    const ACDID idACDPlayer)
{
    return TRUE;
}









void SPlayerRegisterForExperienceId(
    const Player & tNewPlayer)
{
    int32 nIndexToUse = -1;
    ServerPlayerGlobals * ptServerPlayerGlobals = SGameManager::GameGet().ptServerPlayerGlobals;
    for (int32 i = 0; i < 32; i++)
    {
        if (ptServerPlayerGlobals->tPlayerRegistrationArray[i].uHeroId == tNewPlayer.uHeroId)
        {
            nIndexToUse = i;
            break;
        }
        if (!ptServerPlayerGlobals->tPlayerRegistrationArray[i].bUsed && nIndexToUse == -1)
            nIndexToUse = i;
    }
    ServerPlayerRegistration & tRegistration = ptServerPlayerGlobals->tPlayerRegistrationArray[nIndexToUse];
    tRegistration.bUsed = TRUE;
    tRegistration.uHeroId = tNewPlayer.uHeroId;
    ptServerPlayerGlobals->tPerPlayer[tNewPlayer.nPlayerIndex].uExperienceTagId = nIndexToUse;
}




















void SPlayerUnregisterExperienceId(
    const Player & tPlayer)
{
    ServerPlayerGlobals * ptServerPlayerGlobals = SGameManager::GameGet().ptServerPlayerGlobals;


    for (int32 i = 0; i < 32; i++)
    {
        ServerPlayerRegistration & tRegistration = ptServerPlayerGlobals->tPlayerRegistrationArray[i];
        if (tRegistration.uHeroId == tPlayer.uHeroId)
        {
            tRegistration.bUsed = FALSE;
            tRegistration.tLeftTime = Blizzard::Time::GetTimestamp();
            return;
        }
    }
}








uint32 SPlayerGetExperienceId(
    const Player & tPlayer)
{
    ServerPlayerGlobals * ptServerPlayerGlobals = SGameManager::GameGet().ptServerPlayerGlobals;
    return ptServerPlayerGlobals->tPerPlayer[tPlayer.nPlayerIndex].uExperienceTagId;
}










void SPlayerDisconnect(
    Player & tPlayer,
    QuitReason eQuitReason,
    BOOL bHonorLogoutTimerRules)
{




    if (eQuitReason != QUIT_REASON_COUNT)
    {
        QuitGameMessage tQuitMessage;
        tQuitMessage.eReason = eQuitReason;

        PlayerList listDestPlayers(PlayerListGetElementPool());
        PlayerListGetSingle(listDestPlayers, tPlayer);
        MessageSendToClient(listDestPlayers, SMSG_QUIT, &tQuitMessage, sizeof(tQuitMessage));

        ServerPlayerGlobals * ptServerPlayerGlobals = SGameManager::GameGet().ptServerPlayerGlobals;
        PerPlayerGlobals & tPerPlayer = ptServerPlayerGlobals->tPerPlayer[tPlayer.nPlayerIndex];
        tPerPlayer.bGracefulDisconnect = TRUE;



        ServerPlayerLeftOnConnection(tPlayer.idGameConnection, tPlayer);
        if (sSPlayerNumPlayersOnConnection(tPlayer.idGameConnection) <= 1)
        {
            ServerDisconnectWhenOutgoingQueueEmpty(tPlayer);
        }
    }




    else
    {
        ServerClientDisconnectGC(tPlayer.idGameConnection);
    }

    SPlayerOffline(tPlayer, bHonorLogoutTimerRules);
}




void SPlayerOffline(
    Player & tPlayer,
    BOOL bHonorLogoutTimerRules)
{
    if (bHonorLogoutTimerRules)
    {
        if (SPlayerInScriptedSequence(tPlayer) && tPlayer.idPlayerACD != ACDID_INVALID && ScriptEventOnScriptedSequenceSkipped(tPlayer.idPlayerACD))
        {
            PlayerIndexMessage tMessage;
            tMessage.nPlayerIndex = tPlayer.nPlayerIndex;
            PlayerList listDestPlayers(PlayerListGetElementPool());
            PlayerListGetAllInGame(listDestPlayers, -1, FALSE, -1);
            MessageSendToClient(listDestPlayers, SMSG_HANDLE_EVENT_SKIP, &tMessage, sizeof(tMessage));
        }
        SPlayerStartLogoutTimer(tPlayer.nPlayerIndex, FALSE, LOGOUT_CONTEXT_SOLO);
    }
    else
    {
        tPlayer.auLogoutTimerData[LOGOUT_CONTEXT_SOLO].uLogoutTimeTicks = GameGetServerTime() - 299;
        tPlayer.auLogoutTimerData[LOGOUT_CONTEXT_SOLO].bAbortable = FALSE;
    }
    tPlayer.idGameConnection = GAMECONNECTIONID_INVALID;
}
































void SPlayerStartLogoutTimer(
    int32 nPlayerIndex,
    BOOL bIssueKickWarning,
    LogoutContext eContext)
{
    BOOL bAbortable = !bIssueKickWarning;

    Player * ptPlayer = PlayerGetByPlayerIndex(nPlayerIndex);
    if (ptPlayer == NULL)
    {
        return;
    }

    BOOL bStartTimer = FALSE;


    if (ptPlayer->auLogoutTimerData[eContext].uLogoutTimeTicks == 0 || bIssueKickWarning)
    {
        bStartTimer = TRUE;
    }













    if (bStartTimer)
    {
        ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
        if (ptACDPlayer != NULL)
        {
            BOOL bCanSkipTimer = !bIssueKickWarning;

            LogoutTickTimeMessage tMessage;
            if (bCanSkipTimer)
            {
                tMessage.uKickTicks = 0;
                tMessage.uMaxWaitTicks = 10 * TICKS_PER_SECOND;
                tMessage.uMinWaitTicks = 0;
            }
            else
            {
                tMessage.uKickTicks = 10 * TICKS_PER_SECOND;
                tMessage.uMaxWaitTicks = 10 * TICKS_PER_SECOND;
                tMessage.uMinWaitTicks = 10 * TICKS_PER_SECOND;
            }
            tMessage.eContext = eContext;
            PlayerList listDestPlayers(PlayerListGetElementPool());
            listDestPlayers.AddTail_Tracked(ptPlayer);
            MessageSendToClient(listDestPlayers, SMSG_LOGOUT_TIMER_STARTED, &tMessage, sizeof(tMessage));

            ptPlayer->auLogoutTimerData[eContext].uLogoutTimeTicks = GameGetServerTime() + tMessage.uMaxWaitTicks;
            ptPlayer->auLogoutTimerData[eContext].bAbortable = bAbortable;

            ptPlayer->auLogoutTimerData[eContext].uGracePeriodTimeTicks = GameGetServerTime() + tMessage.uKickTicks;
        }
    }
}




void SPlayerCancelLogoutTimer(
    int32 nPlayerIndex,
    LogoutContext eLogoutContext)
{
    Player * ptPlayer = PlayerGetByPlayerIndex(nPlayerIndex);
    if (ptPlayer != NULL && ptPlayer->auLogoutTimerData[eLogoutContext].bAbortable)
    {
        ptPlayer->auLogoutTimerData[eLogoutContext].uLogoutTimeTicks = 0;
        ptPlayer->auLogoutTimerData[eLogoutContext].uGracePeriodTimeTicks = 0;
    }
}




void SPlayerAttemptLogout(
    int32 nPlayerIndex)
{
    Player * ptPlayer = PlayerGetByPlayerIndex(nPlayerIndex);
    if (ptPlayer != NULL)
    {

        uint32 uSoonestLogoutTime = sGetSoonestLogoutTime(*ptPlayer);
        if (uSoonestLogoutTime == 0)
            return;



        ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
        if (ptACDPlayer != NULL)
            SPlayerDisconnect(*ptPlayer, 0, TRUE);
    }
}









BOOL SPlayerIsLoggingOut(
    BOOL includeGracePeriod,
    Player & tPlayer)
{
    for (int i = 0; i < 2; i++)
    {
        if (tPlayer.auLogoutTimerData[i].uLogoutTimeTicks != 0)
        {
            BOOL bInGracePeriod = tPlayer.auLogoutTimerData[i].uGracePeriodTimeTicks != 0 && GameGetServerTime() < tPlayer.auLogoutTimerData[i].uGracePeriodTimeTicks;
            if (includeGracePeriod || !bInGracePeriod)
                return TRUE;
        }
    }
    return FALSE;
}

















ACDID SPlayerGetStartACD(
    const SWorldID idSWorld,
    const SSceneID idSScene,
    const Team eTeam,
    const uint32 uEntranceGUID)
{
    XPooledDataIDList listACDs(MemoryPoolsGetDATAIDListPool());
    ACDID idACDEntranceZero = ACDID_INVALID;

    ActorIterator tIt;
    while (ActorDataArrayIterate(tIt))
    {
        Actor * ptActor = tIt.ptElement;

        ActorCommonData & tACD = ACDGet(ptActor->idACD);
        if (!tACD.IsInWorld())
            continue;



        if (tACD.GetSWorldID() != idSWorld)
            continue;



        if (idSScene != SSCENEID_INVALID)
        {
            SSceneID idSSceneActor = ACDGetCurrentSSceneID(tACD);
            if (idSSceneActor != idSScene)
                continue;
        }



        uint32 uDynamicEntranceGUID = tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_DYNAMIC_ENTRANCE_GUID));

        ScopedActorDefinitionLockR pdefActor(ptActor->snoActor);
        GizmoType nGizmoType = (GizmoType)TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_GIZMO_TYPE);

        if (uDynamicEntranceGUID != 0 ||
            nGizmoType == ACTORGIZMO_TOWN_PORTAL ||
            nGizmoType == ACTORGIZMO_HEARTH_PORTAL ||
            nGizmoType == ACTORGIZMO_SAVEPOINT_PORTAL ||
            nGizmoType == ACTORGIZMO_PORTAL_DESTINATION)
        {
            uint32 uStartingLocationGUID = uDynamicEntranceGUID;

            if (uStartingLocationGUID == 0)
            {

                if (nGizmoType == ACTORGIZMO_PORTAL_DESTINATION)
                {


                    if (ptActor->tMarkerOwner.snoMarkerSet == SNO_NONE)
                        continue;



                    ScopedMarkerSetDefinitionLockR pdefMarkerSet(ptActor->tMarkerOwner.snoMarkerSet);
                    const Marker & tMarker = pdefMarkerSet->tMarkerSet[ptActor->tMarkerOwner.nMarkerIndex];

                    uStartingLocationGUID = TagMapGetInt(tMarker.hTagMap, TAG_MARKER_PORTAL_DEST_ENTRANCE_NUMBER);
                }
                else
                {
                    uStartingLocationGUID = ActorGetInheritedTagMapValueDWORD(ptActor, TAG_ACTOR_GIZMO_PORTAL_DEST_ENTRANCE_NUMBER, TAG_MARKER_PORTAL_DEST_ENTRANCE_NUMBER);


                    if (uStartingLocationGUID == (uint32)-1)
                        continue;
                }
            }


            if (uStartingLocationGUID == (uint32)-1)
            {
                idACDEntranceZero = ptActor->idACD;
            }


            if (uEntranceGUID != (uint32)-1 && uStartingLocationGUID != uEntranceGUID)
                continue;






            if (eTeam == TEAM_INVALID)
            {
                listACDs.AddTail_Tracked(ptActor->idACD);
            }
            else
            {
                Team eTeamACD = tACD.GetTeam();
                if (eTeamACD == TEAM_INVALID)
                {
                    listACDs.AddTail_Tracked(ptActor->idACD);
                }


                else if (GameTypeIsPvP() && eTeamACD == TEAM_NEUTRAL)
                {
                    listACDs.AddTail_Tracked(ptActor->idACD);
                }


                else if (eTeamACD == eTeam)
                {
                    listACDs.AddTail_Tracked(ptActor->idACD);
                }
            }
        }
    }

    ACDID idACD = idACDEntranceZero;

    if (listACDs.Count() != 0)
    {





        int32 nRandom = RandRange_Internal(SGameGlobalsGet().rng, 0, listACDs.Count() - 1);

        while (nRandom--)
        {
            listACDs.RemoveHead();
        }

        idACD = (ACDID)listACDs.GetHead();
    }















    return idACD;
}





BOOL SPlayerGetStartLocation(
    const SWorldID idSWorld,
    const SSceneID idSScene,
    const Team eTeam,
    const uint32 uEntranceGUID,
    WorldPlace & tPlace,
    Quaternion & q)
{
    ACDID idACD = SPlayerGetStartACD(idSWorld, idSScene, eTeam, uEntranceGUID);
    if (idACD == ACDID_INVALID)
        return FALSE;
    ACDGet(idACD).GetStartLocation(tPlace, q);
    return TRUE;
}








void SPlayerOnChangeLevelArea(
    ACDID idACDPlayer,
    LevelAreaID idLevelAreaOld,
    LevelAreaID idLevelAreaNew)
{
    ActorCommonData & tACDPlayer = ACDGet(idACDPlayer);

    if (GameActiveGameCommonDataIsServer())
    {

        Player * ptPlayer = PlayerGetByACD(tACDPlayer.id);

        if (idLevelAreaOld != LEVELAREAID_INVALID)
        {
            SNO snoLevelAreaOld = LevelAreaGetSNO(idLevelAreaOld);
            SGameStatsPlayerTimedEventFinish(ptPlayer, STAT_COUNTER_PLAYER_TIME_IN_LEVEL_AREA, snoLevelAreaOld, GameGetDifficultyLevel());
            SGameStatsPlayerTimedEventFinish(ptPlayer, STAT_COUNTER_PLAYER_TIME_IN_LEVEL_AREA_BY_CLASS, snoLevelAreaOld, PlayerGetPlayerClass(ptPlayer));
            SGameStatsPlayerTimedEventFinish(ptPlayer, STAT_COUNTER_PLAYER_TIME_IN_LEVEL_AREA_BY_ALT_LEVEL, snoLevelAreaOld, tACDPlayer.AttributesGetInt(ATTRIBUTE_ALT_LEVEL));

            {
                Achievement::EvaluationData tEvaluationData;
                tEvaluationData.SetPlayer(ptPlayer);

                tEvaluationData.EvaluateEvent(ptPlayer, Achievement::ACH_EVENT_LEAVE_LEVEL_AREA, snoLevelAreaOld, 1, (uint32)sub_71007b5e70());
            }
        }

        if (!tACDPlayer.IsLoading())
        {
            SNO snoLevelAreaNew = LevelAreaGetSNO(idLevelAreaNew);

            SGameStatsPlayerTimedEventStart(ptPlayer, STAT_COUNTER_PLAYER_TIME_IN_LEVEL_AREA, snoLevelAreaNew, GameGetDifficultyLevel(), TRUE);
            SGameStatsPlayerTimedEventStart(ptPlayer, STAT_COUNTER_PLAYER_TIME_IN_LEVEL_AREA_BY_CLASS, snoLevelAreaNew, PlayerGetPlayerClass(ptPlayer), TRUE);
            SGameStatsPlayerTimedEventStart(ptPlayer, STAT_COUNTER_PLAYER_TIME_IN_LEVEL_AREA_BY_ALT_LEVEL, snoLevelAreaNew, tACDPlayer.AttributesGetInt(ATTRIBUTE_ALT_LEVEL), TRUE);

            if (snoLevelAreaNew != SNO_NONE &&
                ActGetWaypointByLevelArea(snoLevelAreaNew, TRUE) != -1)
            {

                SPlayerSetLastWaypointLevelArea(ptPlayer, snoLevelAreaNew);
            }

            SQuestsOnPlayerEnterLevelArea(idACDPlayer, snoLevelAreaNew);
            ScriptEventOnPlayerEnteredLevelArea(idACDPlayer, snoLevelAreaNew);

            SNO snoBountyQuest = QuestGetActiveBountyForLevelArea(snoLevelAreaNew, FALSE);
            if (snoBountyQuest != SNO_NONE && QuestGetQuestType(snoBountyQuest) == QUEST_TYPE_HORADRIC_QUEST)
            {
                ScriptEventOnPlayerEnteredHoradricQuestLevelArea(idACDPlayer, snoLevelAreaNew, snoBountyQuest);
            }

            SNO snoPower = GlobalSNOGet(GLOBALSNO_POWERS_ACTOR_INTOWN_BUFF);
            if (snoPower != SNO_NONE)
            {

                if (LevelAreaIsTown(snoLevelAreaNew))
                {

                    PowerAPIBuffStartData tBuffStartData(POWERID_INVALID);
                    tBuffStartData.snoPower = snoPower;
                    tBuffStartData.eStackType = BUFF_STACKTYPE_NEVER;
                    tBuffStartData.bInfinite = TRUE;
                    tBuffStartData.bPositiveEffect = TRUE;
                    FastAttribSetValueInt(tBuffStartData.idFastAttrib, MakeAttribKey(ATTRIBUTE_POWER_BUFF_0_VISUAL_EFFECT_RUNE_NONE, snoPower), TRUE);
                    PowerAPI_BuffAddToACD(tBuffStartData, tACDPlayer);
                }
                else
                {

                    BuffID idBuff = PowerAPI_BuffFindOnACD(snoPower, &tACDPlayer, ACDID_INVALID);
                    if (idBuff != BUFFID_INVALID)
                    {

                        PowerAPI_BuffFlagForDeletion(idBuff, TRUE, TRUE, FALSE, TRUE);
                    }
                }
            }

            {
                Achievement::EvaluationData tEvaluationData;
                tEvaluationData.SetPlayer(ptPlayer);

                tEvaluationData.EvaluateEvent(ptPlayer, Achievement::ACH_EVENT_ENTER_LEVEL_AREA, snoLevelAreaNew, 1, (uint32)sub_71007b5e70());
            }
        }
    }

    HelperProcessPlayerLevelAreaChange(tACDPlayer, idLevelAreaNew);
}

















static void sLoadingBuffRemoved(
    PowerAPIBuffInstance * ptBuff,
    BuffRemovalReason eReason,
    void *)
{


    SGame & tSGame = SGameManager::GameGet();
    STimedEventGlobals * ptEventGlobals = tSGame.ptSTimedEventGlobals;
    if (ptBuff && ptBuff->idACDAffected != ACDID_INVALID)
    {
        ActorCommonData * ptACD = ACDTryToGet(ptBuff->idACDAffected);
        if (ptACD)
        {


            ptACD->AttributesSetInt(ATTRIBUTE_LOADING, FALSE);
            ptACD->AttributesSetInt(ATTRIBUTE_LOADING_PLAYER_ACD, ACDID_INVALID);
            ptACD->AttributesSetInt(ATTRIBUTE_LOADING_POWER_SNO, SNO_NONE);
            ptACD->AttributesSetInt(ATTRIBUTE_LOADING_ANIM_TAG, -1);
            ptACD->AttributesSetInt(ATTRIBUTE_LOADING_NEW_GAME, FALSE);

            SPlayerWorldCreatingBuffRemove(*ptACD);


            KnownListItemChanged(KET_ACD, ptACD->id, FALSE);

            ServerPlayerGlobals * ptServerPlayerGlobals = SGameManager::GameGet().ptServerPlayerGlobals;

            XPooledDataIDList & listACDs = ptServerPlayerGlobals->listLoadingACDs;
            POS pos = listACDs.GetHeadPosition();
            while (pos)
            {
                POS posLast = pos;
                ACDID idACDCurrent = (ACDID)listACDs.GetNext(pos);
                if (idACDCurrent == ptACD->id)
                {
                    listACDs.RemoveAt(posLast);
                }
            }

            Player * ptPlayer = PlayerGetByACD(ptACD->id);
            if (ptEventGlobals && ptPlayer)
            {
                WorldPlace placeCurr = ptACD->GetPlace();
                SNO snoWorld = WorldCommonGetWorldSNO(placeCurr.idSWorld);

                SQuestsOnPlayerEnterWorld(ptACD->id, snoWorld);
                ScriptEventOnPlayerEnteredWorld(ptACD->id, snoWorld);
                STimedEventOnPlayerEnterWorld(*ptPlayer);

                LevelAreaID idLevelArea = ACDGetLevelAreaID(*ptACD);
                SPlayerOnChangeLevelArea(ptACD->id, LEVELAREAID_INVALID, idLevelArea);



                ActorMovement * ptMove = ptACD->GetActorMovement();
                if (ptMove)
                {
                    ptMove->ActivateTriggersAtLocation(FALSE);
                }

                ptACD->AttributesSetInt(ATTRIBUTE_AUTO_PORTING_TO_SAVE_POINT, FALSE);


                if (!ptPlayer->tHearthPortalInfo.IsValid() && !ptPlayer->bIsPrimaryPlayerServerOnly)
                {
                    Player * ptPrimaryPlayer = PlayerGetPrimaryPlayerForGameConnection(ptPlayer->idGameConnection);
                    if (ptPrimaryPlayer && ptPrimaryPlayer->tHearthPortalInfo.IsValid())
                    {
                        GizmoSetHearthPortal(*ptPlayer, ptPrimaryPlayer->tHearthPortalInfo.place, TRUE);
                    }
                }


                else if (ptPlayer->bIsPrimaryPlayerServerOnly)
                {
                    for (Player * ptNextPlayer = PlayerGetFirstInGame(); ptNextPlayer; ptNextPlayer = PlayerGetNextInGame(ptNextPlayer))
                    {
                        if (PlayersAreLocalToEachOther(*ptNextPlayer, *ptPlayer))
                        {
                            GizmoSetHearthPortal(*ptNextPlayer, ptPlayer->tHearthPortalInfo.place, TRUE);
                        }
                    }
                }


            }
        }
    }
}





void SPlayerApplyInvulBuff(
    ActorCommonData & tACDPlayer)
{
    SNO snoPower = GlobalSNOGet(GLOBALSNO_POWERS_ACTOR_INVUL_BUFF);
    if (PowerAPI_BuffFindOnACD(snoPower, &tACDPlayer, ACDID_INVALID) == BUFFID_INVALID)
    {
        PowerAPIBuffStartData tBuffStartData(POWERID_INVALID);
        tBuffStartData.snoPower = snoPower;
        tBuffStartData.eStackType = BUFF_STACKTYPE_NEVER;
        tBuffStartData.dwDurationMin = 60 * TICKS_PER_SECOND;
        tBuffStartData.bPositiveEffect = TRUE;
        tBuffStartData.bUndispellable = TRUE;
        FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_INVULNERABLE, TRUE);
        FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_CANNOT_BE_ADDED_TO_AI_TARGET_LIST, TRUE);
        PowerAPI_BuffAddToACD(tBuffStartData, tACDPlayer);
    }

    for (int32 i = 0; i < MAX_PETS; i++)
    {
        ActorCommonData * ptACDPet = ACDTryToGet(PlayerGetPet(tACDPlayer.id, i));
        if (ptACDPet != NULL)
        {
            PowerAPIBuffStartData tBuffStartData(POWERID_INVALID);
            tBuffStartData.snoPower = snoPower;
            tBuffStartData.eStackType = BUFF_STACKTYPE_NEVER;
            tBuffStartData.dwDurationMin = 60 * TICKS_PER_SECOND;
            tBuffStartData.bPositiveEffect = TRUE;
            tBuffStartData.bUndispellable = TRUE;
            FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_CANNOT_BE_ADDED_TO_AI_TARGET_LIST, TRUE);
            FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_INVULNERABLE, TRUE);
            FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_IMMOBOLIZED, TRUE);
            FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_CANT_START_DISPLAYED_POWERS, TRUE);
            PowerAPI_BuffAddToACD(tBuffStartData, *ptACDPet);
        }
    }
}











void SPlayerRemoveInvulBuff(
    ActorCommonData & tACDPlayer)
{
    SNO snoPower = GlobalSNOGet(GLOBALSNO_POWERS_ACTOR_INVUL_BUFF);
    BuffID idBuff = PowerAPI_BuffFindOnACD(snoPower, &tACDPlayer, ACDID_INVALID);
    if (idBuff != BUFFID_INVALID)
    {
        PowerAPI_BuffFlagForDeletion(idBuff, TRUE, TRUE, FALSE, TRUE);
        for (int32 i = 0; i < MAX_PETS; i++)
        {
            ACDID idACDPet = PlayerGetPet(tACDPlayer.id, i);
            if (idACDPet != ACDID_INVALID)
            {
                ActorCommonData * ptACDPet = ACDTryToGet(idACDPet);
                if (ptACDPet != NULL)
                {
                    idBuff = PowerAPI_BuffFindOnACD(snoPower, ptACDPet, ACDID_INVALID);
                    if (idBuff != BUFFID_INVALID)
                        PowerAPI_BuffFlagForDeletion(idBuff, TRUE, TRUE, FALSE, TRUE);
                }
            }
        }
    }
}







void SPlayerApplyLoadingBuff(
    ActorCommonData & tACD,
    ActorCommonData & tACDPlayer,
    BOOL bNewGame,
    BOOL bApplyInvulBuff)
{
    uint32 uBuffDuration = 60 * TICKS_PER_SECOND;

    if (tACD.id != tACDPlayer.id && !tACDPlayer.IsLoading())
    {
        SPlayerApplyLoadingBuff(tACDPlayer, tACDPlayer, bNewGame, bApplyInvulBuff);
    }

    if (!tACDPlayer.IsLoading() && bApplyInvulBuff)
    {
        SPlayerApplyInvulBuff(tACDPlayer);
    }

    if (!tACD.IsLoading())
    {
        SNO snoPower = GlobalSNOGet(GLOBALSNO_POWERS_ACTOR_LOADING_BUFF);
        PowerAPIBuffStartData tBuffStartData(POWERID_INVALID);
        tBuffStartData.snoPower = snoPower;
        tBuffStartData.eStackType = BUFF_STACKTYPE_NEVER;
        tBuffStartData.dwDurationMin = uBuffDuration;
        tBuffStartData.bInfinite = FALSE;
        tBuffStartData.bUndispellable = TRUE;
        tBuffStartData.bPositiveEffect = TRUE;
        tBuffStartData.pRemovalEffect = sLoadingBuffRemoved;
        FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_INVULNERABLE, TRUE);
        FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_HIDDEN, TRUE);
        FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_NPC_DISABLE_OPERATION, TRUE);
        FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_UNTARGETABLE, TRUE);
        FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_IMMOBOLIZED, TRUE);
        FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_CANT_START_DISPLAYED_POWERS, TRUE);
        FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_CANNOT_BE_ADDED_TO_AI_TARGET_LIST, TRUE);
        FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_RESOURCE_DEGENERATION_STOP_POINT, TRUE);
        PowerAPI_BuffAddToACD(tBuffStartData, tACD);

        tACD.AttributesSetInt(ATTRIBUTE_LOADING, TRUE);
        tACD.AttributesSetInt(ATTRIBUTE_LOADING_PLAYER_ACD, tACDPlayer.id);
        if (bNewGame && tACD.id == tACDPlayer.id)
        {
            tACD.AttributesSetInt(ATTRIBUTE_LOADING_NEW_GAME, TRUE);
        }

        KnownListItemChanged(KET_ACD, tACD.id, FALSE);

        ServerPlayerGlobals * ptServerPlayerGlobals = SGameManager::GameGet().ptServerPlayerGlobals;
        ptServerPlayerGlobals->listLoadingACDs.AddTail_Tracked(tACD.id);
    }
}

















void SPlayerGhostedBuffApply(
    ActorCommonData & tACDPlayer,
    BOOL bInfiniteDuration)
{
    SNO snoPower = GlobalSNOGet(GLOBALSNO_POWERS_ACTOR_GHOSTED_BUFF);
    PowerAPIBuffStartData tBuffStartData(POWERID_INVALID);
    tBuffStartData.snoPower = snoPower;
    tBuffStartData.eStackType = BUFF_STACKTYPE_NEVER;
    tBuffStartData.bInfinite = FALSE;
    tBuffStartData.bPositiveEffect = TRUE;
    tBuffStartData.bUndispellable = TRUE;
    tBuffStartData.nBuffIndex = 0;
    FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_HAS_LOOK_OVERRIDE, TRUE);
    tBuffStartData.nLookOverrideHash = StringHashI("ghost");
    FastAttribSetValueInt(tBuffStartData.idFastAttrib, MakeAttribKey(ATTRIBUTE_POWER_BUFF_0_VISUAL_EFFECT_RUNE_NONE, snoPower), TRUE);

    ScopedGlobalDefinitionLockR pdefGlobals;
    if (bInfiniteDuration)
    {
        tBuffStartData.bInfinite = TRUE;
    }
    else if (GameTypeIsPvP())
    {
        tBuffStartData.dwDurationMin = (int32)(pdefGlobals->ptServerData->flPVPPlayerGhostDuration * TICKS_PER_SECOND);
    }
    else
    {
        tBuffStartData.dwDurationMin = (int32)pdefGlobals->ptServerData->flPlayerGhostDuration * TICKS_PER_SECOND;
    }

    FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_GHOSTED, TRUE);
    FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_STEALTHED, TRUE);
    FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_WEAPONS_HIDDEN, TRUE);
    FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_INVULNERABLE, TRUE);
    FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_UNTARGETABLE, TRUE);
    FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_CANT_START_DISPLAYED_POWERS, TRUE);
    FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_CANNOT_BE_ADDED_TO_AI_TARGET_LIST, TRUE);
    FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_WALK_PASSABILITY_POWER_SNO, snoPower);
    PowerAPI_BuffAddToACD(tBuffStartData, tACDPlayer);
}















void SPlayerGhostedBuffRemove(
    ActorCommonData & tACDPlayer)
{
    SNO snoPower = GlobalSNOGet(GLOBALSNO_POWERS_ACTOR_GHOSTED_BUFF);
    BuffID idBuff = PowerAPI_BuffFindOnACD(snoPower, &tACDPlayer, ACDID_INVALID);
    if (idBuff != BUFFID_INVALID)
        PowerAPI_BuffFlagForDeletion(idBuff, TRUE, TRUE, FALSE, TRUE);
}





void SPlayerWorldCreatingBuffApplyToLocalPlayers(
    ActorCommonData & tACDPlayer)
{
    Player * ptActivatingPlayer = PlayerGetByACD(tACDPlayer.id);
    if (ptActivatingPlayer != NULL)
    {
        for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer != NULL; ptPlayer = PlayerGetNextInGame(ptPlayer))
        {
            if (ptActivatingPlayer == ptPlayer || PlayersAreLocalToEachOther(*ptActivatingPlayer, *ptPlayer))
            {
                ActorCommonData * ptPlayerACD = ACDTryToGet(ptPlayer->idPlayerACD);
                if (ptPlayerACD != NULL)
                    SPlayerWorldCreatingBuffApply(*ptPlayerACD);
            }
        }
    }
    else
    {
        SPlayerWorldCreatingBuffApply(tACDPlayer);
    }
}










void SPlayerWorldCreatingBuffApply(
    ActorCommonData & tACDPlayer)
{
    SNO snoPower = GlobalSNOGet(GLOBALSNO_POWERS_WORLD_CREATING_BUFF);
    PowerAPIBuffStartData tBuffStartData(POWERID_INVALID);
    tBuffStartData.snoPower = snoPower;
    tBuffStartData.eStackType = BUFF_STACKTYPE_NEVER;
    tBuffStartData.bInfinite = TRUE;
    tBuffStartData.bPositiveEffect = TRUE;
    FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_INVULNERABLE, TRUE);
    FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_HIDDEN, TRUE);
    FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_NPC_DISABLE_OPERATION, TRUE);
    FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_UNTARGETABLE, TRUE);
    FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_IMMOBOLIZED, TRUE);
    FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_CANT_START_DISPLAYED_POWERS, TRUE);
    FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_CANNOT_BE_ADDED_TO_AI_TARGET_LIST, TRUE);
    PowerAPI_BuffAddToACD(tBuffStartData, tACDPlayer);
}



void SPlayerWorldCreatingBuffRemoveFromLocalPlayers(
    ActorCommonData & tACDPlayer)
{
    Player * ptActivatingPlayer = PlayerGetByACD(tACDPlayer.id);
    if (ptActivatingPlayer != NULL)
    {
        for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer != NULL; ptPlayer = PlayerGetNextInGame(ptPlayer))
        {
            if (ptActivatingPlayer == ptPlayer || PlayersAreLocalToEachOther(*ptActivatingPlayer, *ptPlayer))
            {
                ActorCommonData * ptPlayerACD = ACDTryToGet(ptPlayer->idPlayerACD);
                if (ptPlayerACD != NULL)
                    SPlayerWorldCreatingBuffRemove(*ptPlayerACD);
            }
        }
    }
    else
    {
        SPlayerWorldCreatingBuffRemove(tACDPlayer);
    }
}










void SPlayerWorldCreatingBuffRemove(
    ActorCommonData & tACDPlayer)
{
    SNO snoPower = GlobalSNOGet(GLOBALSNO_POWERS_WORLD_CREATING_BUFF);
    BuffID idBuff = PowerAPI_BuffFindOnACD(snoPower, &tACDPlayer, ACDID_INVALID);
    if (idBuff != BUFFID_INVALID)
        PowerAPI_BuffFlagForDeletion(idBuff, TRUE, TRUE, FALSE, TRUE);
}





void SPlayer_ClientLoadComplete(
    Player & tPlayerFrom)
{
    ActorCommonData * ptACDPlayer = ACDTryToGet(tPlayerFrom.idPlayerACD);
    if (ptACDPlayer && ACDIsPlayer(ptACDPlayer->id))
    {
        SNO snoPowerWarpIn = ptACDPlayer->AttributesGetInt(ATTRIBUTE_LOADING_POWER_SNO);
        ActorAnimationTag eAnimTag = ptACDPlayer->AttributesGetInt(ATTRIBUTE_LOADING_ANIM_TAG);

        ServerPlayerGlobals * ptServerPlayerGlobals = SGameManager::GameGet().ptServerPlayerGlobals;

        XPooledDataIDList & listACDs = ptServerPlayerGlobals->listLoadingACDs;
        XPooledDataIDList listRemovals(MemoryPoolsGetDATAIDListPool());
        POS pos = listACDs.GetHeadPosition();
        while (pos)
        {
            ACDID idACDCurrent = (ACDID)listACDs.GetNext(pos);
            ActorCommonData * ptACDCurrent = ACDTryToGet(idACDCurrent);
            if (ptACDCurrent)
            {
                ACDID idACDCurrentPlayer = (ACDID)ptACDCurrent->AttributesGetInt(ATTRIBUTE_LOADING_PLAYER_ACD);
                if (idACDCurrentPlayer == ptACDPlayer->id)
                {
                    listRemovals.AddTail_Tracked(idACDCurrent);
                }
            }
        }

        if (listRemovals.Count() > 0)
        {
            SNO snoPower = GlobalSNOGet(GLOBALSNO_POWERS_ACTOR_LOADING_BUFF);
            pos = listRemovals.GetHeadPosition();
            while (pos)
            {
                ACDID idACDCurrent = (ACDID)listRemovals.GetNext(pos);
                ActorCommonData * ptACDCurrent = ACDTryToGet(idACDCurrent);
                if (ptACDCurrent)
                {
                    BuffID idBuff = PowerAPI_BuffFindOnACD(snoPower, ptACDCurrent, ACDID_INVALID);
                    if (idBuff != BUFFID_INVALID)
                    {
                        PowerAPI_BuffFlagForDeletion(idBuff, TRUE, TRUE, FALSE, TRUE);
                    }
                }
            }
        }

















        if (snoPowerWarpIn != SNO_NONE)
        {

            SPlayerGhostedBuffRemove(*ptACDPlayer);

            Target tTarget;
            tTarget.InitializeFromACD(ptACDPlayer->id, FALSE, FALSE);
            PowerStart(ptACDPlayer->id, snoPowerWarpIn, tTarget);
        }
        else if (eAnimTag != -1)
        {
            ActorID idActorPlayer = (ActorID)ACDGetOwner(ptACDPlayer->id);
            ActorPowersInterruptPrimary(idActorPlayer, TRUE);
            BrainAPIDoPlayAnimation(idActorPlayer, eAnimTag, -1, TRUE, FALSE, 0.0f, FALSE, 1.0f);
        }
    }
}



static void sSPlayerWarpedInternal(
    ActorCommonData & tACDPlayer,
    const PlayerWarpReason eReason,
    const ACDID idACDWarpGizmo,
    const WorldPlace * ptPlaceBeforeWarp,
    const float flWarpFadeInSecs,
    const BOOL bSendClientWarped)
{
    if (bSendClientWarped)
    {
        SendPlayerWarping(tACDPlayer.id);
    }

    ActorCommonData * ptACDWarpGizmo = ACDTryToGet(idACDWarpGizmo);
    if (ptACDWarpGizmo && !ptACDWarpGizmo->IsGizmo())
    {
        ptACDWarpGizmo = NULL;
    }

    Player * ptPlayer = PlayerGetByACD(tACDPlayer.id);
    ptPlayer->eWarpReason = eReason;
    ptPlayer->flWarpFadeInSecs = flWarpFadeInSecs;

    if (TradeIsTrading(ptPlayer->idPlayerACD))
    {
        STradeCancel(ptPlayer->idPlayerACD);
    }

    tACDPlayer.ModifiedMark(ACDMODIFIED_WARPED);

    switch (eReason)
    {
    case WARP_TOWN_PORTAL:
        ACDPlayEffect(tACDPlayer.id, EFFECT_WARP_IN_PORTAL, 0, tACDPlayer.id);
        break;

    case WARP_WAYPOINT:
    case WARP_CHECKPOINT:
        ACDPlayEffect(tACDPlayer.id, EFFECT_WARP_IN_WAYPOINT, 0, tACDPlayer.id);
        break;

    case WARP_COMPANION:
        ACDPlayEffect(tACDPlayer.id, EFFECT_WARP_IN_COMPANION, 0, tACDPlayer.id);
        break;
    }









    ACDWarpLastHeadStone(tACDPlayer);

    Minimap::OnPlayerWarped(tACDPlayer, eReason, ptACDWarpGizmo);



    SComplexEffectRemoveAll(tACDPlayer.id);

    ACDWarpAllPetsToPlayer(tACDPlayer, eReason);


    if (ptPlaceBeforeWarp)
    {
        if (eReason != WARP_TOWN_PORTAL && eReason != WARP_CHANGING_ACTS && eReason != WARP_HEARTH_PORTAL && eReason != WARP_CHECKPOINT)
        {



            XPooledDataIDList listACDsInRadius(MemoryPoolsGetDATAIDListPool());
            ACDsIntersectSphere(*ptPlaceBeforeWarp, 30.0f, tACDPlayer.id, listACDsInRadius, 7);

            POS pos = listACDsInRadius.GetHeadPosition();
            while (pos)
            {
                ACDID idACDCurrent = (ACDID)listACDsInRadius.GetNext(pos);
                ActorCommonData * ptACDCurrent = ACDTryToGet(idACDCurrent);
                if (ptACDCurrent &&
                    ptACDCurrent->IsAlive() &&
                    ptACDCurrent->IsMonster() &&
                    ACDIsFriendly(tACDPlayer, *ptACDCurrent, FALSE))
                {
                    ActorID idActorNPC = (ActorID)ACDGetOwner(idACDCurrent);
                    AIBehaviorType eBehaviorType = AIStateGetBehaviorType(idActorNPC);
                    if (eBehaviorType == AIBEHAVIOR_TYPE_ESCORT_FOLLOW)
                    {

                        if (ACDGetPetOwner(ptACDCurrent->id) == ACDID_INVALID)
                        {
                            if (EscortCanBePulledThroughWarp(*ptACDCurrent, tACDPlayer))
                            {
                                ACDWarpToACD(*ptACDCurrent, tACDPlayer, WARP_UNKNOWN, 80.0f);
                                BrainAPIDoIdle((ActorID)ptACDCurrent->idOwner);
                                AnimPlayerSendIdle(*ptACDCurrent);
                            }
                        }
                    }
                }
            }
        }



        if (ptACDWarpGizmo)
        {
            ScopedActorDefinitionLockR pdefActorGizmo(ptACDWarpGizmo->snoActor);
            SNO snoActorWarpEffect = TagMapGetSNO(pdefActorGizmo->hTagMap, TAG_ACTOR_GIZMO_WARP_OUT_ACTOR);
            if (snoActorWarpEffect != SNO_NONE)
            {
                ActorAddStruct tActorAdd;
                tActorAdd.snoActor = snoActorWarpEffect;
                tActorAdd.tSNOName.Set(ACTOR_SNO, snoActorWarpEffect);
                tActorAdd.dwActorAddFlags =
                    (1 << ACTORADD_BRAINLESS_BIT) | (1 << ACTORADD_DOESNT_WALK_BIT) | (1 << ACTORADD_EVEN_WHEN_STUCK) |
                    (1 << ACTORADD_DONT_REPORT_SPAWN_ERROR) | (1 << ACTORADD_DONT_PERTURB) | (1 << ACTORADD_DONT_SEARCH_FOR_LOCATION);




                tActorAdd.tPlace = *ptPlaceBeforeWarp;
                tActorAdd.qOrientation = cgqIdentity;
                ActorID idActorProxy = ActorAdd(tActorAdd);
                if (idActorProxy != ACTORID_INVALID)
                {
                    ActorDeleteSoon(idActorProxy, 180);
                }
            }
        }
    }


    ActorID idActor = (ActorID)ACDGetOwner(tACDPlayer.id);
    PowerID idPowerPrimary = ActorPowersGetPrimary(idActor);
    Power * ptPowerPrimary = PowerTryToGet(idPowerPrimary);
    if (ptPowerPrimary)
    {
        PowerForceStop(idPowerPrimary, TRUE);
    }


    if (ptACDWarpGizmo)
    {
        ScopedActorDefinitionLockR pdefActorGizmo(ptACDWarpGizmo->snoActor);
        SNO snoPowerWarpIn = TagMapGetSNO(pdefActorGizmo->hTagMap, TAG_ACTOR_GIZMO_WARP_IN_POWER);
        if (snoPowerWarpIn != SNO_NONE)
        {
            if (tACDPlayer.IsLoading())
            {
                tACDPlayer.AttributesSetInt(ATTRIBUTE_LOADING_POWER_SNO, snoPowerWarpIn);
            }
            else
            {

                SPlayerGhostedBuffRemove(tACDPlayer);

                Target tTarget;
                tTarget.InitializeFromACD(tACDPlayer.id, FALSE, FALSE);
                PowerStart(tACDPlayer.id, snoPowerWarpIn, tTarget);
            }
        }
    }


    if (eReason == WARP_CHALLENGE)
    {
        SWorld * ptSWorld = SWorldTryToGet(tACDPlayer.GetSWorldID());
        if (ptSWorld && ptSWorld->tChallengeCheckpointPlace.idSWorld != SWORLDID_INVALID)
        {
            QuestInstance * ptQuestInstance = QuestsGetQuestInstance(ptSWorld->snoChallenge, ACDID_INVALID);
            if (ptQuestInstance && !QuestIsComplete(*ptQuestInstance))
            {

                CheckpointData tChallengeCheckpoint;
                tChallengeCheckpoint.place = ptSWorld->tChallengeCheckpointPlace;
                tChallengeCheckpoint.qOrientation = ptSWorld->qChallengeCheckpointOrientation;
                SPlayerSetCheckpoint(*ptPlayer, tChallengeCheckpoint);
            }
        }
    }

    if (NephalemRiftManager::IsTieredRiftActive() && ptPlaceBeforeWarp)
    {
        SWorld * ptSWorld = SWorldTryToGet(tACDPlayer.GetSWorldID());
        if (ptSWorld)
        {
            if (WorldGetDungeonFinderLevel(ptSWorld->tWCD.snoWorld) != -1 && WorldGetDungeonFinderLevel(WorldCommonGetWorldSNO(ptPlaceBeforeWarp->idSWorld)) == -1)
            {
                ActorPowersRemoveBuffsForDueling(ACDGet(ptPlayer->idPlayerACD), GlobalSNOGet(GLOBALSNO_POWERS_ARCHON));
            }
        }
    }


    SWorld * ptSWorld = SWorldTryToGet(tACDPlayer.GetSWorldID());
    if (ptSWorld)
    {
        ptSWorld->dwFlags |= 1;
    }

    SActorMovement * ptMove = (SActorMovement *)tACDPlayer.GetActorMovement();
    if (ptMove)
    {
        ptMove->ResetValidationToCurrentPlace();
    }


    KnownListForceUpdateForAllPlayers();


    SSceneForceDeactivationCheckThisTick();

    SNO snoWorldBefore = ptPlaceBeforeWarp ? WorldCommonGetWorldSNO(ptPlaceBeforeWarp->idSWorld) : SNO_NONE;
    CRefString szBefore = SNOToString(WORLD_SNO, snoWorldBefore, FALSE);

    WorldPlace tPlaceAfterWarp = tACDPlayer.GetPlace();
    SNO snoWorldAfter = WorldCommonGetWorldSNO(tPlaceAfterWarp.idSWorld);
    CRefString szAfter = SNOToString(WORLD_SNO, snoWorldAfter, FALSE);











}





void SPlayerWarped(
    ActorCommonData & tACDPlayer,
    const PlayerWarpReason eReason,
    const WorldPlace * ptPlaceBeforeWarp,
    const ACDID idACDWarpGizmo,
    const float flWarpFadeInSecs,
    const BOOL bSendClientWarped)
{
    sSPlayerWarpedInternal(tACDPlayer, eReason, idACDWarpGizmo, ptPlaceBeforeWarp, flWarpFadeInSecs, bSendClientWarped);
}




void SPlayerWarpedThroughPortal(
    ActorCommonData & tACDPlayer,
    const ACDID idACDPortalFrom,
    const ACDID idACDPortalTo,
    const PlayerWarpReason eReason,
    const WorldPlace * ptPlaceBeforeWarp)
{
    sSPlayerWarpedInternal(tACDPlayer, eReason, idACDPortalFrom, ptPlaceBeforeWarp, 0.0f, TRUE);
}







inline BOOL PlayerSetPlayerGameFlag(
    Player * ptPlayer,
    int32 nPlayerFlagIndex,
    BOOL bValue)
{
    SQuestsOnPlayerFlagSet(ptPlayer->idPlayerACD, nPlayerFlagIndex, bValue);
    FlagSetSetFlag(ptPlayer->arGameFlags, sizeof(ptPlayer->arGameFlags), nPlayerFlagIndex, bValue);
    return TRUE;
}








BOOL PlayerSetPlayerGameFlag(
    Player * ptPlayer,
    LPCSTR szLabel,
    BOOL bValue)
{
    SNO snoPlayerFlags = GlobalSNOGet(GLOBALSNO_FLAGSET_PLAYER_FLAGS);
    int32 nIndex = FlagSetLookup(snoPlayerFlags, szLabel);
    if (nIndex == -1)
        return FALSE;
    return PlayerSetPlayerGameFlag(ptPlayer, nIndex, bValue);
}






void SPlayerRefreshKillCounterTime(
    ActorCommonData * ptACDPlayer)
{
    if (ptACDPlayer != NULL && ptACDPlayer->IsAlive())
    {
        Player * ptPlayer = PlayerGetByACD(ptACDPlayer->id);
        if (!ptPlayer->bIsPrimaryPlayerServerOnly)
        {
            for (Player * ptOther = PlayerGetFirstInGame(); ptOther != NULL; ptOther = PlayerGetNextInGame(ptOther))
            {
                if (PlayersAreLocalToEachOther(*ptPlayer, *ptOther) && ptOther->bIsPrimaryPlayerServerOnly)
                {
                    ptPlayer = ptOther;
                    break;
                }
            }
        }
        PlayerStats & tStats = ptPlayer->tStats;
        tStats.m_arKillCounter[0].uLastTick = GameGetServerTime();
    }
}










static const uint32 KILL_COUNT_MAX = 0xffffff;
struct KillCounterDef {  // size 0x10
    uint32 uTickTolerance;  // 0x00
    uint32 uMinCount;  // 0x04
    uint32 uOffset;  // 0x08
    float flXPCountMult;  // 0x0c
};











static KillCounterDef gstKillCountDefs[KILLCOUNTER_COUNT] =
{
    { 120, 10, 5, 1.0f },
    { 120, 0, 0, 0.0f },
    { 30, 6, 5, 0.0f },
    { 30, 1, 0, 0.0f },
};





int32 GetXPReward(
    uint32 uCountGrantingXP,
    uint32 uOffset,
    float flXPCountMult,
    int32 nLevel)
{





    if (uCountGrantingXP > uOffset)
    {
        ScopedGameBalanceDefinitionLockR ptGameBalDef(GlobalSNOGet(GLOBALSNO_GB_EXPERIENCE));
        const ExperienceTable & tExpTable = ptGameBalDef->tExperienceTable;


        float flBaseXP = (float)tExpTable.ptExperienceLevels[nLevel - 1].nKillCounterXP;

        float flTotalXP = flXPCountMult * ((uCountGrantingXP - uOffset) * flBaseXP);
        return Math::FloatRoundToInt(flTotalXP);
    }

    return 0;
}



void SPlayerUpdateKillCounter(
    Player & tPlayer,
    KillCounterType eType)
{


    KillCounter & tCounter = tPlayer.tStats.m_arKillCounter[eType];
    KillCounterDef & tDef = gstKillCountDefs[eType];

    uint32 uNow = GameGetServerTime();
    if (uNow <= tCounter.uLastUpdate)
    {
        return;
    }
    tCounter.uLastUpdate = uNow;

    uint32 uExpireTime = tCounter.uLastTick + tDef.uTickTolerance;
    if (eType == KILLCOUNTER_MONSTER)
    {
        uint32 uToleranceInTicks = (uint32)(PlayerGetStreakTimerTolerance(tPlayer) * 60.0f);
        uExpireTime = tCounter.uLastTick + uToleranceInTicks;
    }







    BOOL bExpired = uNow > uExpireTime;

    if (eType == KILLCOUNTER_MONSTER && !tPlayer.bIsPrimaryPlayerServerOnly)
    {
        bExpired = FALSE;
    }



    static const uint32 MASSACRE_SEND_MESSAGE_INTERVAL = 15;
    if (eType == KILLCOUNTER_MONSTER && tCounter.bStartingTimerActive)
    {

        bExpired = FALSE;

        ScopedGlobalDefinitionLockR pdefGlobals;
        int32 nMassacreStartingTimerDuration = (int32)(pdefGlobals->flMassacreStartingTimer * 60.0f);
        uint32 uStartingTimerExpireTime = tCounter.uStartTime + nMassacreStartingTimerDuration;
        if (uNow >= uStartingTimerExpireTime)
        {
            tCounter.bStartingTimerActive = FALSE;

            tCounter.uLastTick = uNow;
        }
    }
    uint32 uMinCount = tDef.uMinCount;
    if (eType == KILLCOUNTER_MONSTER)
    {
        uMinCount = Callouts::GetMassacreMinCount();
        if (tCounter.uLastTick > tCounter.uPreviousLastTick)
        {
            tCounter.uPreviousLastTick = tCounter.uLastTick;
            SimpleMessage tMessage;
            PlayerList listDestPlayers(PlayerListGetElementPool());
            listDestPlayers.AddTail_Tracked(&tPlayer);
            MessageSendToClient(listDestPlayers, SMSG_KILL_COUNTER_REFRESH, &tMessage, sizeof(tMessage));
        }
    }
    else if (eType == KILLCOUNTER_OBJECT_DESTRUCTION)
    {
        ActorCommonData * ptACDPlayer = ACDTryToGet(tPlayer.idPlayerACD);
        if (ptACDPlayer)
        {
            SNO snoDestructionBuffPower = GlobalSNOGet(GLOBALSNO_POWERS_DESTRUCTIONSTREAK_BUFF_RUN_SPEED);
            ScopedPowerDefinitionLockR pdefDestructionBuffPower(snoDestructionBuffPower);
            uMinCount = Math::FloatToInt(PowerGetFormulaValue(ptACDPlayer, pdefDestructionBuffPower, TAG_POWER_SCRIPT_FORMULA_3, -1, -1, 0));
        }
    }
    if (bExpired)
    {
        if (tCounter.uTotalCount >= uMinCount)
        {
            ActorCommonData * ptACDPlayer = ACDTryToGet(tPlayer.idPlayerACD);
            if (!GameTypeIsPvP() && ptACDPlayer)
            {
                KillCounterUpdateMessage tMessage;
                tMessage.eType = eType;
                tMessage.uCount = tCounter.uTotalCount;
                BOOL bAlreadyGrantedXP = FALSE;
                tMessage.uTotalTime = 0;
                tMessage.bExpired = TRUE;
                tMessage.flXPMultiplier = 1.0f;
                uint64 uXPRewarded = 0;
                if (eType == KILLCOUNTER_ENVIRONMENT_ATTACK)
                {
                    tMessage.uTotalTime = Callouts::ApplyEnvironmentKillBuff(tPlayer, tCounter.uTotalCount - tDef.uMinCount);
                }
                if (Callouts::IsSharedCounterType(eType))
                {
                    switch (eType)
                    {
                    case KILLCOUNTER_MONSTER:
                        tMessage.flXPMultiplier = Callouts::GetMassacreXPMultiplier(tCounter.uTotalCount);
                        tCounter.bStartingTimerActive = FALSE;
                        break;
                    case KILLCOUNTER_OBJECT_DESTRUCTION:
                    {
                        tDef.uOffset = uMinCount - 1;
                        SNO snoPower = GlobalSNOGet(GLOBALSNO_POWERS_DESTRUCTIONSTREAK_BUFF_RUN_SPEED);
                        BuffID idBuff = PowerAPI_BuffFindOnACD(snoPower, ptACDPlayer, ACDID_INVALID);
                        PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
                        if (ptBuff)
                        {
                            Attrib eAttribStart;
                            Attrib eAttribEnd;
                            Attrib eAttribCount;
                            PowerGetBuffAttributes(ptBuff->nBuffIndex, eAttribStart, eAttribEnd, eAttribCount);
                            uint32 dwEndTick = ptACDPlayer->AttributesGetInt(MakeAttribKey(eAttribEnd, snoPower));
                            tMessage.uTotalTime = Math::FloatRoundToInt((float)(dwEndTick - uNow + tDef.uTickTolerance) / 60.0f);
                        }
                    }
                    }


                    for (Player * ptPotentialLocalPlayer = PlayerGetFirstInGame(); ptPotentialLocalPlayer; ptPotentialLocalPlayer = PlayerGetNextInGame(ptPotentialLocalPlayer))
                    {
                        if (PlayersAreLocalToEachOther(tPlayer, *ptPotentialLocalPlayer))
                        {
                            ActorCommonData * ptACDLocalPlayer = ACDTryToGet(ptPotentialLocalPlayer->idPlayerACD);
                            KillCounter & tLocalPlayerCounter = ptPotentialLocalPlayer->tStats.m_arKillCounter[eType];



                            if (eType == KILLCOUNTER_MONSTER)
                            {
                                uXPRewarded = (uint32)(tLocalPlayerCounter.nTotalXP * (tMessage.flXPMultiplier - 1.0f));
                                tLocalPlayerCounter.nTotalXP = 0;
                            }

                            ExperienceGrantDirectly(*ptACDLocalPlayer, uXPRewarded, TRUE, FALSE);
                            ScriptEventOnPlayerKillCounter(ptPotentialLocalPlayer->idPlayerACD, eType);
                            PowerAPI_BuffApplyOnCalloutEventEffects(ptPotentialLocalPlayer->idPlayerACD, CalloutEventFromKillCounter(eType));
                        }
                    }
                }
                else
                {


                    if (eType == KILLCOUNTER_MONSTER)
                    {

                        uXPRewarded = (uint32)(tCounter.nTotalXP * (tMessage.flXPMultiplier - 1.0f));
                        tCounter.nTotalXP = 0;
                    }
                    else
                    {
                        uXPRewarded = GetXPReward(tCounter.uCountGrantingXP, tDef.uOffset, tDef.flXPCountMult, ptACDPlayer->AttributesGetInt(ATTRIBUTE_LEVEL));
                    }
                    ExperienceGrantDirectly(*ptACDPlayer, uXPRewarded, TRUE, FALSE);

                    ScriptEventOnPlayerKillCounter(tPlayer.idPlayerACD, eType);
                    PowerAPI_BuffApplyOnCalloutEventEffects(tPlayer.idPlayerACD, CalloutEventFromKillCounter(eType));
                }

                PlayerList listDestPlayers(PlayerListGetElementPool());
                listDestPlayers.AddTail_Tracked(&tPlayer);
                MessageSendToClient(listDestPlayers, SMSG_KILL_COUNTER_UPDATE, &tMessage, sizeof(tMessage));
            }
        }

        if (tCounter.uTotalCount > 0)
        {
            Achievement::EvaluationData tEvaluationData;











            if (Callouts::IsSharedCounterType(eType))
            {
                for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
                {
                    if (PlayersAreLocalToEachOther(tPlayer, *ptPlayer))
                    {
                        tEvaluationData.SetPlayer(ptPlayer);
                        tEvaluationData.EvaluateEvent(ptPlayer, Achievement::ACH_EVENT_KILL_COUNTER_RESET, eType, 1, (uint32)sub_71007b5e70());
                    }
                }
            }




            else
            {
                tEvaluationData.SetPlayer(&tPlayer);
                tEvaluationData.EvaluateEvent(&tPlayer, Achievement::ACH_EVENT_KILL_COUNTER_RESET, eType, 1, (uint32)sub_71007b5e70());
            }





        }

        if (eType == KILLCOUNTER_MONSTER)
        {

            for (Player * ptPotentialLocalPlayer = PlayerGetFirstInGame(); ptPotentialLocalPlayer; ptPotentialLocalPlayer = PlayerGetNextInGame(ptPotentialLocalPlayer))
            {
                if (PlayersAreLocalToEachOther(tPlayer, *ptPotentialLocalPlayer))
                {
                    KillCounter & tLocalPlayerCounter = ptPotentialLocalPlayer->tStats.m_arKillCounter[KILLCOUNTER_MONSTER];
                    tLocalPlayerCounter.Reset();
                }
            }
        }
        else
        {

            tCounter.Reset();
        }
    }


    else if ((eType == KILLCOUNTER_MONSTER || eType == KILLCOUNTER_OBJECT_DESTRUCTION) && uNow >
        tCounter.uLastMessageSentTime + MASSACRE_SEND_MESSAGE_INTERVAL)
    {


        if (tCounter.uTotalCount >= uMinCount)
        {
            ActorCommonData * ptACDPlayer = ACDTryToGet(tPlayer.idPlayerACD);
            if (!GameTypeIsPvP() && ptACDPlayer && tCounter.uLastMessageCount != tCounter.uTotalCount)
            {
                tCounter.uLastMessageSentTime = uNow;
                tCounter.uLastMessageCount = tCounter.uTotalCount;

                KillCounterUpdateMessage tMessage;
                tMessage.eType = eType;
                tMessage.uCount = tCounter.uTotalCount;
                tMessage.flXPMultiplier = Callouts::GetMassacreXPMultiplier(tCounter.uTotalCount);
                tMessage.uTotalTime = 0;
                tMessage.bExpired = FALSE;

                PlayerList listDestPlayers(PlayerListGetElementPool());
                listDestPlayers.AddTail_Tracked(&tPlayer);
                MessageSendToClient(listDestPlayers, SMSG_KILL_COUNTER_UPDATE, &tMessage, sizeof(tMessage));
            }
        }
    }
}



void SPlayerUpdateCurrencyCounter(
    Player & tPlayer,
    CurrencyType eCurrencyType)
{
    uint32 uNow = GameGetServerTime();
    CurrencyCounter & tCounter = (eCurrencyType == CURRENCY_PLATINUM) ? tPlayer.tStats.m_tPlatinumCounter : tPlayer.tStats.m_tGoldCounter;
    if (uNow <= tCounter.uLastUpdate)
        return;
    tCounter.uLastUpdate = uNow;

    if (uNow > tCounter.uLastTick + 5 * TICKS_PER_SECOND)
    {
        if (tCounter.uCount == 0)
            return;
        CurrencyCounterUpdateMessage tMessage;
        tMessage.uAmountCollected = (eCurrencyType == CURRENCY_GOLD) ? tCounter.uCount : tCounter.uCount - tCounter.uLastCount;
        tMessage.eCurrencyType = eCurrencyType;
        tMessage.bExpired = TRUE;
        PlayerList listDestPlayers(PlayerListGetElementPool());
        listDestPlayers.AddTail_Tracked(&tPlayer);
        MessageSendToClient(listDestPlayers, SMSG_CURRENCY_COUNTER_UPDATE, &tMessage, sizeof(tMessage));

        Achievement::EvaluationData tEvaluationData;
        tEvaluationData.SetPlayer(&tPlayer);
        tEvaluationData.EvaluateEvent(&tPlayer, Achievement::ACH_EVENT_GOLD_COUNTER_EXPIRED, 1, (uint32)sub_71007b5e70());
        tCounter.Reset();
    }
    else
    {
        if (tCounter.uCount <= tCounter.uLastCount)
            return;
        CurrencyCounterUpdateMessage tMessage;
        tMessage.uAmountCollected = tCounter.uCount - ((eCurrencyType == CURRENCY_GOLD) ? 0 : tCounter.uLastCount);
        tMessage.eCurrencyType = eCurrencyType;
        tMessage.bExpired = FALSE;
        PlayerList listDestPlayers(PlayerListGetElementPool());
        listDestPlayers.AddTail_Tracked(&tPlayer);
        MessageSendToClient(listDestPlayers, SMSG_CURRENCY_COUNTER_UPDATE, &tMessage, sizeof(tMessage));
        tCounter.uLastCount = tCounter.uCount;
    }
}





























void SPlayerFixUpCurrenciesDiscovered(
    ActorCommonData & tACDPlayer)
{
    GBHandleList listGBHandles(GBGetHandlePool());
    GBEnumerate(GB_CURRENCY_CONVERSIONS, listGBHandles);

    int32 nCurrenciesDiscovered = tACDPlayer.AttributesGetInt(ATTRIBUTE_CURRENCIES_DISCOVERED);

    POS pos = listGBHandles.GetHeadPosition();
    while (pos)
    {
        GBHandle hGB = listGBHandles.GetNext(pos);
        ScopedCurrencyConversionEntryLockR pdefCurrencyConversion(hGB.gbid);
        if (pdefCurrencyConversion && !pdefCurrencyConversion->bHidden)
        {
            CurrencyType eCurrencyType = pdefCurrencyConversion->eCurrency;
            int64 nCurrencyAmount = tACDPlayer.GetCurrencyAmount(eCurrencyType);
            if (nCurrencyAmount > 0)
            {
                nCurrenciesDiscovered |= (1 << eCurrencyType);
            }
        }
    }

    tACDPlayer.AttributesSetInt(ATTRIBUTE_CURRENCIES_DISCOVERED, nCurrenciesDiscovered);
}












static void sUpdateCurseCounts()
{





    static FastAttribKey arNecroCursedAttribKeys[3] = {
        MakeAttribKey(ATTRIBUTE_NECROMANCER_CURSED, GlobalSNOGet(GLOBALSNO_POWERS_NECRO_LEECH_PASSIVE_MANAGER)),
        MakeAttribKey(ATTRIBUTE_NECROMANCER_CURSED, GlobalSNOGet(GLOBALSNO_POWERS_NECRO_DECREPIFY_PASSIVE_MANAGER)),
        MakeAttribKey(ATTRIBUTE_NECROMANCER_CURSED, GlobalSNOGet(GLOBALSNO_POWERS_NECRO_FRAILTY_PASSIVE_MANAGER)),
    };

    static FastAttribKey arNecroCurseCountAttribKeys[3] = {
        MakeAttribKey(ATTRIBUTE_NECROMANCER_CURSE_COUNT, GlobalSNOGet(GLOBALSNO_POWERS_NECRO_LEECH_PASSIVE_MANAGER)),
        MakeAttribKey(ATTRIBUTE_NECROMANCER_CURSE_COUNT, GlobalSNOGet(GLOBALSNO_POWERS_NECRO_DECREPIFY_PASSIVE_MANAGER)),
        MakeAttribKey(ATTRIBUTE_NECROMANCER_CURSE_COUNT, GlobalSNOGet(GLOBALSNO_POWERS_NECRO_FRAILTY_PASSIVE_MANAGER)),
    };


    int32 arCurseCountPerPlayer[4][3];
    SigmaMemoryFill(arCurseCountPerPlayer, 0, sizeof(arCurseCountPerPlayer));

    int32 arUniqueCurseCountPerPlayer[4];
    SigmaMemoryFill(arUniqueCurseCountPerPlayer, 0, sizeof(arUniqueCurseCountPerPlayer));

    SWorldID arWorldIDsPerPlayer[4];
    SigmaMemoryFill(arWorldIDsPerPlayer, 0, sizeof(arWorldIDsPerPlayer));


    for (int32 i = 0; i < 4; i++)
    {
        Player * ptPlayer = PlayerGetByPlayerIndex(i);
        if (ptPlayer)
        {
            ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
            if (ptACDPlayer)
            {
                arWorldIDsPerPlayer[i] = ptACDPlayer->GetSWorldID();
            }
        }
    }

    ACDIterator tIt;
    while (ActorCommonDataIterate(tIt))
    {
        ActorCommonData * ptACD = tIt.ptElement;
        if (!ptACD)
        {
            continue;
        }

        if (!ptACD->IsMonster())
        {
            continue;
        }


        bool bFoundCurse = false;
        for (int32 i = 0; i < 3; i++)
        {
            int32 nOwnerPlayerIndex = ptACD->AttributesGetInt(arNecroCursedAttribKeys[i]);
            if (nOwnerPlayerIndex != -1)
            {
                if (ptACD->GetSWorldID() == arWorldIDsPerPlayer[nOwnerPlayerIndex])
                {
                    arCurseCountPerPlayer[nOwnerPlayerIndex][i]++;

                    if (!bFoundCurse)
                    {
                        arUniqueCurseCountPerPlayer[nOwnerPlayerIndex]++;
                        bFoundCurse = true;
                    }
                }
            }
        }
    }


    for (int32 i = 0; i < 4; i++)
    {
        Player * ptPlayer = PlayerGetByPlayerIndex(i);
        if (ptPlayer)
        {
            ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
            if (ptACDPlayer)
            {
                for (int32 j = 0; j < 3; j++)
                {
                    ptACDPlayer->AttributesSetInt(arNecroCurseCountAttribKeys[j], arCurseCountPerPlayer[i][j]);
                }
                ptACDPlayer->AttributesSetInt(ATTRIBUTE_NECROMANCER_UNIQUE_CURSE_COUNT, arUniqueCurseCountPerPlayer[i]);
            }
        }
    }
}




void SPlayerEvaluateMassacreForShrineGlobeDrop(
    uint32 uCurrentScore,
    Player * ptPlayer,
    ActorCommonData & tACDPlayer,
    KillCounter & tCounter)
{
    ScopedGlobalDefinitionLockR pdefGlobals;
    if (!pdefGlobals->ptServerData || pdefGlobals->ptServerData->nMinMonsterLevelForShrineGlobe > GameGetMonsterLevel())
        return;

    Callouts::MassacreDef tMassacreDefinition = Callouts::GetMassacreDef(uCurrentScore);

    BOOL bRollForNGDrop = TRUE;
    for (Player * ptOtherPlayer = PlayerGetFirstInGame(); ptOtherPlayer; ptOtherPlayer = PlayerGetNextInGame(ptOtherPlayer))
    {
        if (ptOtherPlayer->nPlayerIndex != ptPlayer->nPlayerIndex &&
            ptOtherPlayer->bIsPrimaryPlayerServerOnly)
        {

            ActorCommonData * ptACDOtherPlayer = ACDTryToGet(ptOtherPlayer->idPlayerACD);
            if (ptACDOtherPlayer && ACDInSameWorld(tACDPlayer, *ptACDOtherPlayer))
            {

                float flDistSqr = ACDGetDistanceSquared(tACDPlayer, *ptACDOtherPlayer, FALSE);

                if (flDistSqr < 9999999.0f)
                {

                    KillCounter & tOtherPlayerCounter = ptOtherPlayer->tStats.m_arKillCounter[KILLCOUNTER_MONSTER];
                    if (tOtherPlayerCounter.uTotalCount >= tCounter.uTotalCount)
                        bRollForNGDrop = FALSE;
                }
            }
        }
    }

    if (bRollForNGDrop && (tCounter.uMassacreTier < tMassacreDefinition.nLevel || tCounter.uTotalCount == Callouts::GetMassacreMinCount()))
    {

        float flRoll = RandFloat_Internal(SGameGlobalsGet().rng);
        if (flRoll < tMassacreDefinition.flNephGlobeDropChance)
        {

            uint32 dwGameEventNumber = SGameGetEventNumber();
            LootDropShrineGlobe(tACDPlayer.id, dwGameEventNumber, tACDPlayer.id, GBID_NONE);
        }
    }

    tCounter.uMassacreTier = tMassacreDefinition.nLevel;
}







static void sIncCounter(
    Player & tPlayer,
    KillCounterType eType,
    Attrib eRecordAttribute,
    BOOL bDontCountKill,
    BOOL bKillGrantsXP,
    uint64 nBaseExpGranted,
    BOOL bOnlyAddXP)
{


    Player * ptPlayer = &tPlayer;
    ActorCommonData & tACDPlayer = ACDGet(tPlayer.idPlayerACD);
    if (tACDPlayer.IsDueling())
        return;


    if (bOnlyAddXP)
    {
        KillCounter & tCounter = tPlayer.tStats.m_arKillCounter[eType];
        tCounter.nTotalXP += nBaseExpGranted;
        return;
    }




    if (Callouts::IsSharedCounterType(eType) && !tPlayer.bIsPrimaryPlayerServerOnly)
        return;




    SPlayerUpdateKillCounter(tPlayer, eType);

    KillCounter & tCounter = tPlayer.tStats.m_arKillCounter[eType];

    if (eType == KILLCOUNTER_MONSTER)
    {






        if (SPlayerIsInSetDungeon(tACDPlayer))
            return;



        tCounter.nTotalXP += nBaseExpGranted;
    }

    if (!bDontCountKill)
    {
        tCounter.uTotalCount = Math::Min(tCounter.uTotalCount + 1, KILL_COUNT_MAX);

        if (bKillGrantsXP)
        {
            tCounter.uCountGrantingXP = Math::Min(tCounter.uCountGrantingXP + 1, KILL_COUNT_MAX);
        }

        uint32 uRecord = tACDPlayer.AttributesGetInt(eRecordAttribute);
        uint32 uCurrentScore = tCounter.uTotalCount;

        if (eType == KILLCOUNTER_MONSTER)
        {
            if (uCurrentScore == Callouts::GetMassacreMinCount())
            {
                uint32 uNow = GameGetServerTime();
                tCounter.uStartTime = uNow;
                tCounter.bStartingTimerActive = TRUE;
            }



            SPlayerEvaluateMassacreForShrineGlobeDrop(uCurrentScore, ptPlayer, tACDPlayer, tCounter);
        }






        if (uCurrentScore > uRecord)
        {
            tCounter.bNewRecord = TRUE;
            tACDPlayer.AttributesSetInt(eRecordAttribute, uCurrentScore);
        }

        if (eType == KILLCOUNTER_OBJECT_DESTRUCTION)
        {
            Callouts::ApplyDestructionBuff(tPlayer, tCounter.uTotalCount);
        }
    }

    tCounter.uLastTick = GameGetServerTime();
}




static void sAddAllLocalPlayersToList(
    const Player * ptPlayer,
    PlayerList & listEligiblePlayers)
{
    for (Player * ptOtherPlayer = PlayerGetFirstInGame(); ptOtherPlayer; ptOtherPlayer = PlayerGetNextInGame(ptOtherPlayer))
    {
        if (PlayersAreLocalToEachOther(*ptPlayer, *ptOtherPlayer) && !listEligiblePlayers.Find(ptOtherPlayer))
        {
            listEligiblePlayers.AddHead_Tracked(ptOtherPlayer);
        }
    }
}



static void sEvaluateMonsterKillForMassacreCounter(
    const ActorCommonData & tACDVictim,
    ActorCommonData * ptACDAttacker)
{
    PlayerList listEligiblePlayers(PlayerListGetElementPool());
    BOOL bVictimKilledByEnvironment = ptACDAttacker && ptACDAttacker->IsWreckableObject();
    for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {
        if (tACDVictim.GetExpTagPlayer(*ptPlayer) || ExperiencePlayersCloseEnough(tACDVictim, ptPlayer->idPlayerACD) || bVictimKilledByEnvironment)
        {
            listEligiblePlayers.AddHead_Tracked(ptPlayer);

            for (Player * ptOtherPlayer = PlayerGetFirstInGame(); ptOtherPlayer; ptOtherPlayer = PlayerGetNextInGame(ptOtherPlayer))
            {

                if (ptPlayer->nPlayerIndex != ptOtherPlayer->nPlayerIndex &&
                    !listEligiblePlayers.Find(ptOtherPlayer))
                {










                    if (PlayersAreLocalToEachOther(*ptPlayer, *ptOtherPlayer))
                    {
                        listEligiblePlayers.AddHead_Tracked(ptOtherPlayer);
                    }
                    else if (tACDVictim.GetExpTagPlayer(*ptOtherPlayer) || bVictimKilledByEnvironment)
                    {
                        sAddAllLocalPlayersToList(ptOtherPlayer, listEligiblePlayers);
                    }
                    else
                    {
                        ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
                        if (ExperiencePlayersCloseEnough(*ptACDPlayer, ptOtherPlayer->idPlayerACD))
                        {
                            sAddAllLocalPlayersToList(ptOtherPlayer, listEligiblePlayers);
                        }
                    }
                }
            }
            break;
        }
    }

    BOOL bDontCountKill = tACDVictim.HasResurrectionPower();
    BOOL bGrantsXP = !tACDVictim.AttributesGetInt(ATTRIBUTE_GRANTS_NO_XP);
    POS pos = listEligiblePlayers.GetHeadPosition();
    while (pos)
    {
        Player * ptPlayer = listEligiblePlayers.GetNext(pos);
        ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
        if (!ptACDPlayer)
            break;


        int32 nPlayerLevel = ptACDPlayer->AttributesGetInt(ATTRIBUTE_LEVEL);
        uint64 uExpForPlayer = ActorCalculateXPForMonster(tACDVictim, nPlayerLevel);
        sIncCounter(*ptPlayer, KILLCOUNTER_MONSTER, ATTRIBUTE_KILL_COUNT_RECORD, bDontCountKill, bGrantsXP, uExpForPlayer, !ptPlayer->bIsPrimaryPlayerServerOnly);
    }
}



static void sEvaluateObjectKillForDestructionCounter(
    const ActorCommonData & tACDVictim,
    Achievement::EvaluationData & tEvaluationData)
{
    PlayerList listEligiblePlayers(PlayerListGetElementPool());


    for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {

        if (!listEligiblePlayers.Find(ptPlayer))
        {



            if (ExperiencePlayersCloseEnough(tACDVictim, ptPlayer->idPlayerACD))
            {
                sAddAllLocalPlayersToList(ptPlayer, listEligiblePlayers);
            }
        }
    }

    BOOL bDontCountKill = tACDVictim.HasResurrectionPower();
    BOOL bGrantsXP = !tACDVictim.AttributesGetInt(ATTRIBUTE_GRANTS_NO_XP);
    POS pos = listEligiblePlayers.GetHeadPosition();
    while (pos)
    {
        Player * ptPlayer = listEligiblePlayers.GetNext(pos);
        sIncCounter(*ptPlayer, KILLCOUNTER_OBJECT_DESTRUCTION, ATTRIBUTE_OBJECT_DESTRUCTION_RECORD, bDontCountKill, bGrantsXP, 0, FALSE);
        tEvaluationData.SetPlayer(ptPlayer);
        tEvaluationData.EvaluateEvent(ptPlayer,
            Achievement::ACH_EVENT_OBJECT_DESTROYED_BY_PLAYER,

            tACDVictim.snoActor,
            1, (uint32)sub_71007b5e70());
    }
}







static void sIncAllCounters(
    ActorCommonData * ptACDAttacker,
    Player & tPlayer,
    ActorCommonData & tACDVictim,
    PowerAPIDamage * ptDamage)
{
    BOOL bVictimKilledByEnvironment = ptACDAttacker && ptACDAttacker->IsWreckableObject();





    BOOL bTaggedForExperience = tACDVictim.GetExpTagPlayer(tPlayer);



    BOOL bMonsterDestroyedWreckable = ptACDAttacker && ptACDAttacker->IsMonster() && tACDVictim.IsWreckableObject();

    if (!bTaggedForExperience && !bVictimKilledByEnvironment && !bMonsterDestroyedWreckable)
    {
        return;
    }

    BOOL bDontCountKill = tACDVictim.HasResurrectionPower();
    BOOL bGrantsXP = !tACDVictim.AttributesGetInt(ATTRIBUTE_GRANTS_NO_XP);

    Achievement::EvaluationData tEvaluationData;
    tEvaluationData.SetPlayer(&tPlayer);
    tEvaluationData.SetACD(ptACDAttacker);
    tEvaluationData.SetPower(ptDamage);


    if (tACDVictim.IsMonster())
    {

        if (TeamsAreFriendly(tPlayer.eTeam, tACDVictim.GetTeam()))
        {
            return;
        }



        sIncCounter(tPlayer, KILLCOUNTER_SINGLE_ATTACK, ATTRIBUTE_SINGLE_ATTACK_RECORD, bDontCountKill, bGrantsXP, 0, FALSE);


















        ACDID idACDAttackTarget = BrainAPIGetActorAttackTarget(tACDVictim.id);
        if (ACDIsPlayer(idACDAttackTarget) && ptACDAttacker && ptACDAttacker->id != idACDAttackTarget)
        {
            ActorCommonData & tACDAttackTarget = ACDGet(idACDAttackTarget);
            float flPlayerHPPercent = tACDAttackTarget.GetHitpointPercent();
            if (flPlayerHPPercent <= 0.1f)
            {
                ActorID idActorPlayer = (ActorID)ACDGetOwner(idACDAttackTarget);
                if (BrainAPIGetNumEnemyTargets(idActorPlayer) <= 1)
                {
                    SaviorMessage tMessage;
                    tMessage.annSavedPlayer = ANNFromACD(idACDAttackTarget);

                    if (!GameTypeIsPvP())
                    {
                        ScopedGameBalanceDefinitionLockR ptGameBalDef(GlobalSNOGet(GLOBALSNO_GB_EXPERIENCE));
                        const ExperienceTable & tExpTable = ptGameBalDef->tExperienceTable;
                        ActorCommonData & tACDPlayer = ACDGet(tPlayer.idPlayerACD);
                        int32 nLevel = tACDPlayer.AttributesGetInt(ATTRIBUTE_LEVEL);


                        tMessage.uXPRewarded = tExpTable.ptExperienceLevels[nLevel - 1].nKillCounterXP;
                        ExperienceGrantDirectly(tACDPlayer, tMessage.uXPRewarded, TRUE, TRUE);
                    }

                    PlayerList listDestPlayers(PlayerListGetElementPool());
                    listDestPlayers.AddTail_Tracked(&tPlayer);
                    MessageSendToClient(listDestPlayers, SMSG_SAVIOR, &tMessage, sizeof(tMessage));






                    Player * ptSavedPlayer = PlayerGetByACD(idACDAttackTarget);
                    PlayerClass ePlayerClass = ptSavedPlayer ? PlayerGetPlayerClass(idACDAttackTarget) : PLAYER_CLASS_NONE;

                    tEvaluationData.EvaluateEvent(&tPlayer, Achievement::ACH_EVENT_SAVED_ANOTHER_PLAYER_FROM_DEATH, ePlayerClass, 1, (uint32)sub_71007b5e70());





                }
            }
        }


        if (bVictimKilledByEnvironment &&
            ExperiencePlayersCloseEnough(tACDVictim, tPlayer.idPlayerACD))
        {
            sIncCounter(tPlayer, KILLCOUNTER_ENVIRONMENT_ATTACK, ATTRIBUTE_ENVIRONMENT_ATTACK_RECORD, bDontCountKill, bGrantsXP, 0, FALSE);





            if (tACDVictim.IsMonster())
            {
                tEvaluationData.EvaluateEvent(&tPlayer,
                    Achievement::ACH_EVENT_MONSTER_KILLED_BY_ENVIRONMENT,

                    tACDVictim.snoActor,
                    1, (uint32)sub_71007b5e70());
            }
        }
    }


    else if (tACDVictim.IsWreckableObject())
    {

        if (!bMonsterDestroyedWreckable)
        {

            sEvaluateObjectKillForDestructionCounter(tACDVictim, tEvaluationData);
        }

























        else if (ExperiencePlayersCloseEnough(tACDVictim, tPlayer.idPlayerACD))
        {
            tEvaluationData.EvaluateEvent(&tPlayer,
                Achievement::ACH_EVENT_OBJECT_DESTROYED_BY_MONSTER,

                tACDVictim.snoActor,
                1, (uint32)sub_71007b5e70());
        }
    }


}



void SPlayerIncreaseKillCounter(
    const ACDID idACDAttacker,
    const ACDID idACDVictim,
    PowerAPIDamage * ptDamage)
{
    ActorCommonData * ptACDVictim = ACDTryToGet(idACDVictim);
    if (ptACDVictim)
    {
        ActorCommonData * ptACDAttacker = ACDTryToGet(idACDAttacker);

        for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
        {
            ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
            if (ptACDPlayer &&
                ptACDPlayer->IsAlive())
            {
                sIncAllCounters(ptACDAttacker, *ptPlayer, *ptACDVictim, ptDamage);
            }
        }

        if (ptACDVictim->IsMonster())
        {
            sEvaluateMonsterKillForMassacreCounter(*ptACDVictim, ptACDAttacker);
        }
    }
}






void SPlayerIncreaseCurrencyCounter(
    Player & tPlayer,
    CurrencyType eCurrencyType,
    uint32 nAmountPickedUp)
{
    CurrencyCounter & tCounter = (eCurrencyType == CURRENCY_PLATINUM) ? tPlayer.tStats.m_tPlatinumCounter : tPlayer.tStats.m_tGoldCounter;
    uint32 uNow = GameGetServerTime();
    tCounter.uLastTick = uNow;
    tCounter.uCount += nAmountPickedUp;
}


















































































BOOL SPlayerCheckCanWarpToOtherPlayer(
    Player * ptPlayerWarping,
    Player * ptPlayerDest)
{
    if (!ptPlayerWarping || !ptPlayerDest)
        return FALSE;

    ActorCommonData * ptACDDest = ACDTryToGet(ptPlayerDest->idPlayerACD);
    if (!ptACDDest)
        return FALSE;

    ActorCommonData * ptACDWarping = ACDTryToGet(ptPlayerWarping->idPlayerACD);
    if (!ptACDWarping)
        return FALSE;

    if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
        return TRUE;

    if (PlayersAreLocalToEachOther(*ptPlayerWarping, *ptPlayerDest))
    {

        if (ptACDWarping->id == ptPlayerDest->idPlayerACD)
        {

            PlayerList listDestPlayers(PlayerListGetElementPool());
            PlayerListGetSingle(listDestPlayers, *ptPlayerDest);

            SimpleMessage tMessage;
            MessageSendToClient(listDestPlayers, SMSG_OPEN_BANNER_CUSTOMIZATION, &tMessage, sizeof(tMessage));

            return FALSE;
        }

        PlayerDisplayErrorMessage(ptACDWarping->id, GAMEERROR_BANNER_PLAYER_TOO_CLOSE);
        return FALSE;
    }

    if (ptACDWarping->AttributesGetInt(ATTRIBUTE_IN_BOSS_ENCOUNTER))
    {
        PlayerDisplayErrorMessage(ptACDWarping->id, GAMEERROR_POWER_UNUSABLE_DURING_BOSS_ENCOUNTER);
        return FALSE;
    }

    if (ptACDDest->AttributesGetInt(ATTRIBUTE_IN_BOSS_ENCOUNTER))
    {
        PlayerDisplayErrorMessage(ptACDWarping->id, GAMEERROR_BANNER_PLAYER_IS_IN_BOSS_ENCOUNTER);
        return FALSE;
    }

    if (SPlayerInScriptedSequence(*ptPlayerDest))
    {
        PlayerDisplayErrorMessage(ptACDWarping->id, GAMEERROR_BANNER_PLAYER_IS_IN_BOSS_ENCOUNTER);
        return FALSE;
    }

    if (ptACDDest->IsDueling())
    {

        PlayerDisplayErrorMessage(ptACDWarping->id, GAMEERROR_BANNER_PLAYER_IS_DUELING);
        return FALSE;
    }

    if (NephalemRift::IsTieredRiftActive())
    {

        if (ACDIsInTieredLootRun(*ptACDDest) && (!NephalemRiftManager::PlayerPaidForRift(ptPlayerWarping) || !GameGetActiveGameCommonData().tGameSyncedData.bLootRunBossDead))
        {
            PlayerDisplayErrorMessage(ptACDWarping->id, GAMEERROR_BANNER_PLAYER_IS_IN_TIERED_LOOT_RUN);
            return FALSE;
        }
    }
    else
    {
        if (SWorldIsDungeonFinder(ptACDDest->GetSWorldID()) && !NephalemRiftManager::PortToPlayerInStandardRiftPossible(ptPlayerDest, ptPlayerWarping))
            return FALSE;

        if (SWorldIsSetDungeon(ptACDDest->GetSWorldID()) && !SetEncounterManager::CanEnterSetDungeon(ptPlayerWarping))
        {
            PlayerDisplayErrorMessage(ptACDWarping->id, GAMEERROR_BANNER_PLAYER_IS_IN_SET_DUNGEON);
            return FALSE;
        }
    }

    if (!SWorldPlayerAllowed(ptACDDest->GetSWorldID(), *ptPlayerWarping))
    {
        PlayerDisplayErrorMessage(ptACDWarping->id, GAMEERROR_PLAYER_NOT_ALLOWED_IN_WORLD);
        return FALSE;
    }

    if (ACDInSameWorld(*ptACDWarping, *ptACDDest) &&
        ACDGetDistanceSquared(*ptACDWarping, *ptACDDest, FALSE) < 80.0f * 80.0f)
    {

        PlayerDisplayErrorMessage(ptACDWarping->id, GAMEERROR_BANNER_PLAYER_TOO_CLOSE);
        return FALSE;
    }

    return TRUE;
}




























void SPlayerWarpToOtherPlayer(
    Player * ptPlayerWarping,
    Player * ptPlayerDest)
{
    if (!SPlayerCheckCanWarpToOtherPlayer(ptPlayerWarping, ptPlayerDest))
        return;

    ActorCommonData * ptACDDest = ACDTryToGet(ptPlayerDest->idPlayerACD);
    if (!ptACDDest)
        return;

    ActorCommonData * ptACDWarping = ACDTryToGet(ptPlayerWarping->idPlayerACD);
    if (!ptACDWarping)
        return;

    for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {
        if (!PlayersAreLocalToEachOther(*ptPlayer, *ptPlayerWarping))
            continue;

        ptACDWarping = ACDTryToGet(ptPlayer->idPlayerACD);

        WorldPlace placeSpawn = ptACDDest->GetPlace();
        BOOL bValidSpot = SPlayerFindValidWalkableSpotNearPlayer(*ptACDDest, placeSpawn);
        if (!bValidSpot)
        {
            PlayerDisplayErrorMessage(ptACDWarping->id, GAMEERROR_POWER_UNUSABLE_GENERIC);
            return;
        }

        Target tTarget;
        tTarget.InitializeFromPlace(placeSpawn);
        if (ACDWarpToTarget(*ptACDWarping, tTarget, WARP_COMPANION, 80.0f, FALSE))
        {

            SPlayerSetLastWaypointLevelArea(ptPlayer, PlayerGetLastWaypointLevelArea(ptPlayerDest));

            Achievement::EvaluationData tEvaluationData;
            tEvaluationData.SetPlayer(PlayerGetByACD(ptACDWarping->id));

            if (tEvaluationData.GetPlayer())
            {

                tEvaluationData.EvaluateEvent(tEvaluationData.GetPlayer(), Achievement::ACH_EVENT_WARPED_USING_BANNER, 1, (uint32)sub_71007b5e70());
            }
        }
    }
}


























BOOL SPlayerCheckCanWarpToWaypoint(
    Player * ptPlayer,
    uint32 uWaypointIndex)
{

    ActorCommonData * ptACDPlayer = ptPlayer ? ACDTryToGet(ptPlayer->idPlayerACD) : NULL;
    if (ptACDPlayer == NULL)
    {
        return FALSE;
    }


    if (!ptACDPlayer->IsAlive())
    {
        return FALSE;
    }


    if (ptACDPlayer->AttributesGetInt(ATTRIBUTE_IN_BOSS_ENCOUNTER))
    {
        PlayerDisplayErrorMessage(ptACDPlayer->id, GAMEERROR_POWER_UNUSABLE_DURING_BOSS_ENCOUNTER);
        return FALSE;
    }

    const CachedWaypointArray & tWaypoints = ActGetCachedWaypointArray();
    size_t nCount = tWaypoints.size();


    if ((int32)uWaypointIndex < 0 ||
        (int32)uWaypointIndex >= nCount ||
        !SGameGlobalsGet().WaypointIsActive(uWaypointIndex))
    {
        return FALSE;
    }

    const WaypointInfo & tInfo = tWaypoints[uWaypointIndex].tInfo;


    WorldPlace wpPlayer = ptACDPlayer->GetPlace();
    SWorldID idSWorldWaypoint = WorldCommonFindBySNO(tInfo.snoWorld, TRUE);
    if (idSWorldWaypoint != SWORLDID_INVALID && wpPlayer.idSWorld == idSWorldWaypoint)
    {
        LevelAreaID idLevelAreaWaypoint = LevelAreaFind(tInfo.snoLevelArea, idSWorldWaypoint);
        if (idLevelAreaWaypoint != LEVELAREAID_INVALID)
        {
            SSceneID idSSceneWaypoint = LevelAreaGetWaypointSSceneID(idLevelAreaWaypoint);
            SScene * ptSScene = SSceneTryToGet(idSSceneWaypoint);
            if (ptSScene != NULL && ptSScene->bHasWaypoint)
            {
                if (Vector3D::Distance2DSquared(ptSScene->transWaypoint.wp, wpPlayer.wp) < 6400.0f)
                {
                    PlayerDisplayErrorMessage(ptACDPlayer->id, GAMEERROR_WAYPOINT_TOO_CLOSE);
                    return FALSE;
                }
            }
        }
    }

    return TRUE;
}



void SPlayerWarpToWaypoint(
    Player * ptPlayer,
    uint32 uWaypointIndex)
{
    if (!SPlayerCheckCanWarpToWaypoint(ptPlayer, uWaypointIndex))
        return;









    ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
    if (ptACDPlayer != NULL)
        SWorldPlayerWarpToWaypoint(ptPlayer->idPlayerACD, ACDID_INVALID, uWaypointIndex);
}


























void SPlayerEnterAct(
    Player & tPlayer,
    const Act eAct,
    const BOOL bOnJoin)
{
    tPlayer.tHeroStateData.eLastPlayedAct = eAct;
    tPlayer.tHeroStateData.eHighestUnlockedAct = Math::Max(eAct, tPlayer.tHeroStateData.eHighestUnlockedAct);
    SPlayerSyncWaypointsToGame(tPlayer);
    SPlayerClearAllReturnPoints(tPlayer, SNO_NONE);
    SendCurrentActMessage(tPlayer, bOnJoin);
    SPlayerClearHeardConversations(tPlayer);
    SQuestsPlayerJoinAct(tPlayer);
}

















void SPlayerPlayActInterstitial(
    Player & tPlayer,
    const Act eAct,
    BOOL bForceInterstitial)
{
    uint32 dwFlag = PlayerFlagFromActEnum(eAct);
    BOOL bFirstTimeEnteredAct = FALSE;
    if (dwFlag != 0 && !(tPlayer.tHeroStateData.uPlayerFlags & (1 << dwFlag)))
    {
        tPlayer.tHeroStateData.uPlayerFlags |= (1 << dwFlag);
        ActorCommonData * ptACDPlayer = ACDTryToGet(tPlayer.idPlayerACD);
        if (ptACDPlayer != NULL)
            ptACDPlayer->MarkHeroStatePlayerFlagsChanged();
        bFirstTimeEnteredAct = TRUE;
    }
    if ((bFirstTimeEnteredAct || bForceInterstitial) && tPlayer.bIsPrimaryPlayerServerOnly)
        SendInterstitialMessage(tPlayer);
}















void SPlayerUpdateQuestHistory(
    Player & tPlayer,
    SNO snoQuest,
    int32 nStepUID)
{
    ScopedQuestDefinitionLockR pdefQuest(snoQuest);
    if (pdefQuest->eQuestType != QUEST_TYPE_MAIN_QUEST)
        return;

    BOOL bCompletionStep = QuestStepIsCompletionStep(pdefQuest, nStepUID);
    for (int32 i = 0; i < 200; i++)
    {
        PlayerQuestHistoryEntry & tEntry = tPlayer.tHeroStateData.tQuestHistory[i];
        if (tEntry.snoQuest == snoQuest)
        {
            if (tEntry.nStepUID == -3)
                return;
            if (QuestStepIsReachableByStep(pdefQuest, tEntry.nStepUID, nStepUID))
            {
                if (bCompletionStep)
                    nStepUID = -3;
                tEntry.nStepUID = nStepUID;
            }
            tPlayer.uConsoleSyncFlags |= (1 << PCSF_QUEST_HISTORY);
            if (bCompletionStep)
            {
                ActorCommonData * ptACDPlayer = ACDTryToGet(tPlayer.idPlayerACD);
                if (ptACDPlayer != NULL)
                    SGameStatsPlayerCounterIncrement(&tPlayer, STAT_COUNTER_PLAYER_QUEST_COMPLETION_LEVEL, snoQuest, ptACDPlayer->AttributesGetInt(ATTRIBUTE_LEVEL), 1.0f);
            }
            return;
        }
        if (tEntry.snoQuest == SNO_NONE)
        {
            tEntry.snoQuest = snoQuest;
            // Original line 10424 holds the whole select and store (cmp/csel w21, w9, eq/str at 0x7100750634); the == FALSE form gives that operand order.

            tEntry.nStepUID = (bCompletionStep == FALSE) ? nStepUID : -3;
            tPlayer.uConsoleSyncFlags |= (1 << PCSF_QUEST_HISTORY);
            if (bCompletionStep)
            {
                ActorCommonData * ptACDPlayer = ACDTryToGet(tPlayer.idPlayerACD);
                if (ptACDPlayer != NULL)
                    SGameStatsPlayerCounterIncrement(&tPlayer, STAT_COUNTER_PLAYER_QUEST_COMPLETION_LEVEL, snoQuest, ptACDPlayer->AttributesGetInt(ATTRIBUTE_LEVEL), 1.0f);
            }
            return;
        }
    }
}


























void SPlayerReceivedQuestStepReward(
    Player & tPlayer,
    SNO snoQuest,
    int32 nStepUID,
    DifficultyLevel eDifficulty)
{
    if (eDifficulty == -1)
        return;





    ScopedQuestDefinitionLockR pdefQuest(snoQuest);
    if (pdefQuest->eQuestType != QUEST_TYPE_MAIN_QUEST)
        return;




    for (int32 i = 0; i < 100; i++)
    {
        PlayerQuestRewardHistoryEntry & tEntry = tPlayer.tHeroStateData.tQuestRewardHistory[i];


        if (tEntry.snoQuest == snoQuest &&
            tEntry.nStepUID == nStepUID)
        {

            tEntry.uDifficultyFlags |= (1 << eDifficulty);
            tPlayer.uConsoleSyncFlags |= (1 << PCSF_QUEST_REWARD_HISTORY);
            return;
        }






        if (tEntry.snoQuest == SNO_NONE)
        {
            tEntry.snoQuest = snoQuest;
            tEntry.nStepUID = nStepUID;

            tEntry.uDifficultyFlags = (1 << eDifficulty);


            tPlayer.tHeroStateData.nNumQuestRewardEntries = i + 1;
            tPlayer.uConsoleSyncFlags |= (1 << PCSF_QUEST_REWARD_HISTORY);
            return;
        }
    }
}
void SPlayerActivateWaypoint(
    const int32 nWaypointNumber,
    const BOOL bNotifyClient)
{
    if (nWaypointNumber == -1)
        return;

    if (GameIsOpenWorld())
        return;

    for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {
        SPlayerRefreshWaypointDestination(ptPlayer->idPlayerACD);
    }

    SNO snoWaypointLevelArea = ActGetLevelAreaByWaypointIndex(nWaypointNumber);
    SGameGlobals & tSGameGlobals = SGameGlobalsGet();
    if (tSGameGlobals.dwActivatedWaypoints & (1 << nWaypointNumber))
        return;

    tSGameGlobals.WaypointEnable(nWaypointNumber, TRUE);

    for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {
        ActorCommonData & tACDPlayer = ACDGet(ptPlayer->idPlayerACD);
        ptPlayer->tHeroStateData.tSavedData.m_ActivatedWaypoints |= (1 << nWaypointNumber);
        SNO snoLevelArea = ACDGetLevelArea(tACDPlayer);
        if (snoLevelArea == snoWaypointLevelArea)
        {
            SPlayerSetLastWaypointLevelArea(ptPlayer, snoWaypointLevelArea);
        }
        if (bNotifyClient)
        {
            tACDPlayer.MarkHeroStateWaypointsChanged();
        }
    }

    if (bNotifyClient)
        ActorRefreshActivatedWaypoints();
}



















int32 SPlayerCreate(
    GameConnectionID idGameConnection,
    int32 nConsolePartyIndex)
{


    PlayerGlobals * ptPlayerGlobals = GameGetActiveGameCommonData().ptPlayerGlobals;

    for (int32 i = nConsolePartyIndex; ; i++)  // the loop's condition was compiled away: the body always returns
    {





        Player & tPlayer = ptPlayerGlobals->tPlayerArray[i];












        PlayerSetDefaults(tPlayer);

        ServerPlayerGlobals * ptServerPlayerGlobals = SGameManager::GameGet().ptServerPlayerGlobals;

        PerPlayerGlobals & tPerPlayer = ptServerPlayerGlobals->tPerPlayer[tPlayer.nPlayerIndex];
        tPerPlayer.listDPSInfo.RemoveAll();
        tPerPlayer.listRecordedSpeedInfo.RemoveAll();
        tPerPlayer.flDPSDamageTotal = 0.0f;
        tPerPlayer.tRecentAchievementsEarned.clear();
        tPerPlayer.tDeliveredRewards.Clear();
        tPerPlayer.listPings.RemoveAll();
        tPerPlayer.uPingSentTime = 0;
        tPerPlayer.flHighestPing = 0.0f;
        tPerPlayer.bGracefulDisconnect = FALSE;

        Blizzard::String::Format(tPlayer.usHeroName, sizeof(tPlayer.usHeroName), "Player %d", i);
        tPlayer.eTeam = STeamGetTeamForPlayer(tPlayer);
        tPlayer.idGameConnection = idGameConnection;

        return i;
    }
}





void SPlayerSetLicensesAndSessionFlags(
    const Player & tPlayer,
    const OnlineService::ContentLicenseArray & tContentLicenses,
    const OnlineServiceServer::LicenseArray & tRawLicenses,
    uint32 uSessionFlags)
{
    ServerPlayerGlobals * ptServerPlayerGlobals = SGameManager::GameGet().ptServerPlayerGlobals;
    PerPlayerGlobals & tPerPlayer = ptServerPlayerGlobals->tPerPlayer[tPlayer.nPlayerIndex];
    tPerPlayer.tContentLicenses = tContentLicenses;
    tPerPlayer.tRawLicenses = tRawLicenses;
    tPerPlayer.uContentFlags = OnlineService::DetermineContentFlags(tContentLicenses);
    tPerPlayer.uSessionFlags = uSessionFlags;
}































void SPlayerSetConsoleLicenseFlags(
    const Player & tPlayer,
    uint32 uConsoleLicenses)
{
    ServerPlayerGlobals * ptServerPlayerGlobals = SGameManager::GameGet().ptServerPlayerGlobals;
    ptServerPlayerGlobals->tPerPlayer[tPlayer.nPlayerIndex].uConsoleLicenses = uConsoleLicenses;
}




uint32 SPlayerGetConsoleLicenseFlags(
    const Player & tPlayer)
{
    ServerPlayerGlobals * ptServerPlayerGlobals = SGameManager::GameGet().ptServerPlayerGlobals;
    return ptServerPlayerGlobals->tPerPlayer[tPlayer.nPlayerIndex].uConsoleLicenses;
}





BOOL SPlayerGetHasFriends(
    const Player & tPlayer)
{
    ServerPlayerGlobals * ptServerPlayerGlobals = SGameManager::GameGet().ptServerPlayerGlobals;
    return ptServerPlayerGlobals->tPerPlayer[tPlayer.nPlayerIndex].bHasFriends;
}





void SPlayerSetHasFriends(
    const Player & tPlayer,
    BOOL bHasFriends)
{
    ServerPlayerGlobals * ptServerPlayerGlobals = SGameManager::GameGet().ptServerPlayerGlobals;
    ptServerPlayerGlobals->tPerPlayer[tPlayer.nPlayerIndex].bHasFriends = bHasFriends;
}














BOOL SPlayerHasTradeRestrictions(
    const Player & tPlayer)
{
    if (SGameManager::GameGet().ptServerPlayerGlobals == NULL)
        return TRUE;
    if (tPlayer.nPlayerIndex < 0 || tPlayer.nPlayerIndex >= MAX_PLAYERS)
        return TRUE;
    if (SPlayerGetContentLicense(tPlayer, OnlineService::CONTENT_LICENSE_LEVEL_60) == NULL)
        return TRUE;
    return SPlayerGetContentLicense(tPlayer, OnlineService::CONTENT_LICENSE_PAID_PENDING_REVIEW) != NULL;
}









const OnlineService::ContentLicense * SPlayerGetContentLicense(
    const Player & tPlayer,
    OnlineService::ContentLicenseId licenseId)
{
    if ((OnlineService::GetServerContentFlags() & (1 << licenseId)) == 0)
        return NULL;
    ServerPlayerGlobals * ptServerPlayerGlobals = SGameManager::GameGet().ptServerPlayerGlobals;
    if (ptServerPlayerGlobals != NULL)
    {
        PerPlayerGlobals & tPerPlayer = ptServerPlayerGlobals->tPerPlayer[tPlayer.nPlayerIndex];
        for (const OnlineService::ContentLicense * it = tPerPlayer.tContentLicenses.cbegin(); it != tPerPlayer.tContentLicenses.cend(); ++it)
        {
            if (it->licenseId == licenseId)
                return it;
        }
    }
    return NULL;
}
























uint32 SPlayerGetContentFlags(
    const Player & tPlayer)
{
    ServerPlayerGlobals * ptServerPlayerGlobals = SGameManager::GameGet().ptServerPlayerGlobals;
    if (ptServerPlayerGlobals == NULL)
        return 0;
    return ptServerPlayerGlobals->tPerPlayer[tPlayer.nPlayerIndex].uContentFlags & OnlineService::GetServerContentFlags();
}








void SPlayerUpdateLevelCaps()
{
    if (OnlineService::GetServerContentFlags() & (1 << OnlineService::CONTENT_LICENSE_LEVEL_70))
    {
        for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer != NULL; ptPlayer = PlayerGetNextInGame(ptPlayer))
        {
            if (SPlayerGetContentLicense(*ptPlayer, OnlineService::CONTENT_LICENSE_LEVEL_70) != NULL)
            {
                ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
                if (ptACDPlayer != NULL)
                    SPlayerSetLevelCap(*ptACDPlayer, OnlineService::CONTENT_LICENSE_LEVEL_70);
            }
        }
    }
}









void SPlayerSetLevelCap(
    ActorCommonData & tACDPlayer,
    OnlineService::ContentLicenseId eLicenseId)
{
    if (eLicenseId == OnlineService::CONTENT_LICENSE_LEVEL_70)
        tACDPlayer.AttributesSetInt(ATTRIBUTE_LEVEL_CAP, 70);
    else
        tACDPlayer.AttributesSetInt(ATTRIBUTE_LEVEL_CAP, 60);
}







Act SPlayerGetMaxAct(
    const Player & tPlayer)
{
    Act eMaxAct = ACT_4;
    const OnlineService::ContentLicense * ptLicense = SPlayerGetContentLicense(tPlayer, OnlineService::CONTENT_LICENSE_ADVMODE_ACTV);
    if (ptLicense != NULL && !ptLicense->IsExpired())
        eMaxAct = ACT_5;
    return eMaxAct;
}






BOOL SPlayerInScriptedSequence(
    const Player & tPlayer)
{
    ActorCommonData * ptACDPlayer = ACDTryToGet(tPlayer.idPlayerACD);
    if (ptACDPlayer == NULL)
        return FALSE;




    BuffID idBuff = PowerAPI_BuffFindOnACD(GlobalSNOGet(GLOBALSNO_POWERS_CAMERA_FOCUS_BUFF), ptACDPlayer, ACDID_INVALID);

    if (idBuff != BUFFID_INVALID)
    {
        PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
        if (ptBuff == NULL)
            return FALSE;


        const int32 MAX_SCRIPTED_SEQUENCE_RUN_TIME = 18000;



        if (ptBuff->dwStartTick + MAX_SCRIPTED_SEQUENCE_RUN_TIME > GameGetServerTime())
            return TRUE;





        SGameGlobals & tSGameGlobals = SGameGlobalsGet();
        SGameID idSGame = SGameManager::GetCurrentGame();

        tSGameGlobals.dwFlags |= (1 << SGAMEGLOBALS_REQUEST_SHUT_DOWN);
        return FALSE;
    }




    return FALSE;
}


void SPlayerSetCheckpoint(
    Player & tPlayer,
    const CheckpointData & tCheckpoint)
{
    if (!tCheckpoint.IsValid())
        return;






    if (!CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT) &&
        NephalemRiftManager::IsTieredRiftActive() &&
        NephalemRiftManager::PlayerPaidForRift(&tPlayer) &&
        !SWorldIsTieredDungeonFinder(tCheckpoint.place.idSWorld))
        return;






    if (memcmp(&tPlayer.tLastCheckpoint, &tCheckpoint, sizeof(CheckpointData)) == 0)
        return;











    tPlayer.tLastCheckpoint = tCheckpoint;
}







BOOL SPlayerSetSavePoint(
    Player & tPlayer,
    const SavePointData & tSavePoint)
{
    if (!tSavePoint.HasValidSavePoint() || tPlayer.bIsBeingDeleted || QuestIsGameOver())
        return FALSE;




    SavePointData & tLastSavePoint = tPlayer.tHeroStateData.tSavedData.tLastSavePoint;
    if (tSavePoint.nNum > tLastSavePoint.nNum ||
        (tSavePoint.nNum == tLastSavePoint.nNum && tSavePoint.snoWorld != tLastSavePoint.snoWorld))
    {
        tLastSavePoint = tSavePoint;
        ActorCommonData & tACDPlayer = ACDGet(tPlayer.idPlayerACD);
        tACDPlayer.MarkHeroStateLastSavePointChanged();
        SPlayerAddToSaveList(tPlayer, 0, PlayerSaveReason_Checkpoint, PLAYER_SAVE_MODE_WITHIN_5_SECONDS);
        return TRUE;
    }
    return FALSE;
}





















void SPlayerClearAllReturnPoints(
    Player & tPlayer,
    SNO snoWorld)
{
    SWorldID idSWorld = (snoWorld != SNO_NONE) ? WorldCommonFindBySNO(snoWorld, TRUE) : SWORLDID_INVALID;


    if (snoWorld == SNO_NONE ||
        tPlayer.tLastCheckpoint.place.idSWorld == idSWorld)
    {
        tPlayer.tLastCheckpoint.Initialize();
    }


    if (snoWorld == SNO_NONE ||
        tPlayer.tHearthPortalInfo.place.idSWorld == idSWorld)
    {
        PlayerList listThisPlayer(PlayerListGetElementPool());
        PlayerListGetSingle(listThisPlayer, tPlayer);

        tPlayer.tHearthPortalInfo.Initialize();



        if (tPlayer.bIsPrimaryPlayerServerOnly)
        {
            HearthPortalInfoMessage tHearthPortalMessage;

            tHearthPortalMessage.snoLevelArea = SNO_NONE; tHearthPortalMessage.unk_c = SNO_NONE;
            tHearthPortalMessage.annPortal = ANN_INVALID;
            tHearthPortalMessage.bResetUI = FALSE;
            tHearthPortalMessage.bIsTieredRift = FALSE;
            MessageSendToClient(listThisPlayer, SMSG_HEARTHPORTAL_INFO, &tHearthPortalMessage, sizeof(tHearthPortalMessage));

        }
    }


    if (snoWorld == SNO_NONE ||
        tPlayer.tHeroStateData.tSavedData.tLastSavePoint.snoWorld == snoWorld)
    {
        SPlayerClearSavePoint(tPlayer);
    }
}


void SPlayerClearSavePoint(
    Player & tPlayer)
{


    tPlayer.tHeroStateData.tSavedData.tLastSavePoint.Reset();


    ActorCommonData * ptACD = ACDTryToGet(tPlayer.idPlayerACD);
    if (ptACD != NULL)
        ptACD->MarkHeroStateLastSavePointChanged();

    if (tPlayer.bIsPrimaryPlayerServerOnly)
    {
        SavePointInfoMessage tSavePointMessage;
        tSavePointMessage.snoLevelArea = SNO_NONE;
        PlayerList listThisPlayer(PlayerListGetElementPool());
        PlayerListGetSingle(listThisPlayer, tPlayer);
        MessageSendToClient(listThisPlayer, SMSG_SAVEPOINT_INFO, &tSavePointMessage, sizeof(tSavePointMessage));
    }
}










BOOL SPlayerIsWaitingForSavePointPort(
    const Player & tPlayer)
{
    const ActorCommonData * ptACDPlayer = ACDTryToGet(tPlayer.idPlayerACD);
    if (ptACDPlayer == NULL)
        return FALSE;



    if (ptACDPlayer->AttributesGetInt(ATTRIBUTE_LOADING_NEW_GAME))
        return SPlayerHasValidSavePointForPort(tPlayer);
    return FALSE;
}




inline BOOL SPlayerHasValidSavePointForPort(
    const Player & tPlayer)
{
    return tPlayer.tHeroStateData.tSavedData.tLastSavePoint.IsValid();
}
















static const uint32 SPEEDBUCKET_MAX_BUCKET_SIZE = 48;
void SPlayerRecordSentSpeedValue(
    Player & tPlayer,
    ActorCommonData & tACD,
    float flValue)
{
    SpeedBucketList & listRecordedSpeedInfo = PlayerGetSpeedBucketList(tPlayer);



    if (listRecordedSpeedInfo.Count() < SPEEDBUCKET_MAX_BUCKET_SIZE - 1)
    {
        uint32 uNow = GameGetServerTime();
        SpeedBucket tNewBucket;
        tNewBucket.dwTick = uNow;
        tNewBucket.flSpeed = tACD.AttributesGetFloat(ATTRIBUTE_LAST_RUNNING_RATE);
        listRecordedSpeedInfo.AddTail_Tracked(tNewBucket);


        tNewBucket.flSpeed = flValue;
        listRecordedSpeedInfo.AddTail_Tracked(tNewBucket);

        tACD.AttributesSetFloat(ATTRIBUTE_LAST_RUNNING_RATE, flValue);
    }
}





void SPlayerRecordAcceptedSpeedValue(
    Player & tPlayer,
    float flValue)
{
    SpeedBucketList & listRecordedSpeedInfo = PlayerGetSpeedBucketList(tPlayer);



    if (listRecordedSpeedInfo.Count() < 47u)
    {
        uint32 uNow = GameGetServerTime();
        SpeedBucket tNewBucket;
        tNewBucket.dwTick = uNow;
        tNewBucket.flSpeed = flValue;
        listRecordedSpeedInfo.AddTail_Tracked(tNewBucket);
    }
}


static const uint32 SPEED_BUCKET_MAX_TICKS = 105;


float SPlayerGetHighestSpeedValue(
    Player & tPlayer)
{
    uint32 uMinTime = 0;
    uint32 uNow = GameGetServerTime();
    if (uNow > SPEED_BUCKET_MAX_TICKS)
    {
        uMinTime = uNow - SPEED_BUCKET_MAX_TICKS;
    }

    float flMaxSpeedValue = 0.0f;

    ActorCommonData * ptACD = ACDTryToGet(tPlayer.idPlayerACD);
    if (ptACD)
    {

        SpeedBucketList & listRecordedSpeedInfo = PlayerGetSpeedBucketList(tPlayer);
        flMaxSpeedValue = ptACD->AttributesGetFloat(ATTRIBUTE_RUNNING_RATE_TOTAL);




        flMaxSpeedValue = Math::Max(flMaxSpeedValue, ptACD->AttributesGetFloat(ATTRIBUTE_LAST_RUNNING_RATE));

        POS pos = listRecordedSpeedInfo.GetHeadPosition();
        while (pos)
        {
            POS posLast = pos;
            SpeedBucket & tBucket = listRecordedSpeedInfo.GetNext(pos);
            if (tBucket.dwTick < uMinTime)
            {
                listRecordedSpeedInfo.RemoveAt(posLast);
            }
            else
            {

                flMaxSpeedValue = Math::Max(tBucket.flSpeed, flMaxSpeedValue);
            }
        }



        ActorID idActor = (ActorID)ACDGetOwner(ptACD->id);
        PowerID idPower = ActorPowersGetPrimary(idActor);
        Power * ptPower = PowerTryToGet(idPower);
        if (ptPower)
        {

            float flPowerSpeed = PowerGetClientRunSpeed(*ptPower);
            flMaxSpeedValue = Math::Max(flMaxSpeedValue, flPowerSpeed);
        }
    }


    return flMaxSpeedValue;
}























static void sCullPingList(
    PingBucketList & tList)
{
    uint32 uNow = GameGetServerTime();
    uint32 uMinTime = 0;
    if (uNow > PING_INTERVAL * PING_BUCKET_MAX_ENTRIES)
    {
        uMinTime = uNow - PING_INTERVAL * PING_BUCKET_MAX_ENTRIES;
    }

    POS pos = tList.GetHeadPosition();
    while (pos)
    {
        POS posLast = pos;
        PingBucket & tBucket = tList.GetNext(pos);
        if (tBucket.dwTick < uMinTime)
        {
            tList.RemoveAt(posLast);
        }
        else
        {
            break;
        }
    }
}



void SPlayerReceivePing(
    Player & tPlayer)
{
    if (tPlayer.nPlayerIndex >= 0 && tPlayer.nPlayerIndex < MAX_PLAYERS)
    {
        ServerPlayerGlobals * ptServerPlayerGlobals = SGameManager::GameGet().ptServerPlayerGlobals;
        if (ptServerPlayerGlobals)
        {
            PerPlayerGlobals & tPerPlayer = ptServerPlayerGlobals->tPerPlayer[tPlayer.nPlayerIndex];
            if (tPerPlayer.uPingSentTime)
            {
                sCullPingList(tPerPlayer.listPings);


                if (tPerPlayer.listPings.Count() >= PING_BUCKET_MAX_ENTRIES)
                {
                    tPerPlayer.listPings.RemoveHead();
                }

                if (tPerPlayer.listPings.Count() < PING_BUCKET_MAX_ENTRIES)
                {
                    uint32 uNow = GameGetServerTime();
                    if (uNow >= tPerPlayer.uPingSentTime)
                    {
                        int32 nResponseTicks = uNow - tPerPlayer.uPingSentTime;

                        PingBucket tNewBucket;
                        tNewBucket.dwTick = uNow;
                        tNewBucket.nResponseTicks = nResponseTicks;
                        tPerPlayer.listPings.AddTail_Tracked(tNewBucket);
                    }
                }

                tPerPlayer.uPingSentTime = 0;
            }
        }
    }
}



float SPlayerGetPing(
    Player & tPlayer)
{
    float flAvg = 0.0f;

    if (tPlayer.nPlayerIndex >= 0 && tPlayer.nPlayerIndex < MAX_PLAYERS)
    {
        ServerPlayerGlobals * ptServerPlayerGlobals = SGameManager::GameGet().ptServerPlayerGlobals;
        if (ptServerPlayerGlobals)
        {
            PerPlayerGlobals & tPerPlayer = ptServerPlayerGlobals->tPerPlayer[tPlayer.nPlayerIndex];
            sCullPingList(tPerPlayer.listPings);



            POS pos = tPerPlayer.listPings.GetHeadPosition();
            int32 nCount = 0;
            int32 nTotal = 0;
            while (pos)
            {
                PingBucket & tBucket = tPerPlayer.listPings.GetNext(pos);
                nTotal += tBucket.nResponseTicks;
                nCount++;
            }

            if (nCount > 0)
            {
                flAvg = ((float)nTotal * OO_TICKS_PER_SECOND * 1000.0f) / (float)nCount;
            }
        }
    }
    return flAvg;
}






















BOOL SPlayerIsEligibleForQuestState(
    const Player & tPlayer)
{
    if (GameIsOpenWorld())
        return FALSE;
    SGameGlobals & tSGameGlobals = SGameGlobalsGet();
    if (tPlayer.uHeroId == tSGameGlobals.uHeroIdCreator)
        return TRUE;
    if (tPlayer.uServerFlags & (1 << PSF_ELIGIBLE_FOR_QUEST_STATE))
        return TRUE;
    uint32 dwNow = GameGetServerTime();
    if (tPlayer.dwJoinTime + 14400 < dwNow)
        return TRUE;
    QuestInstance * ptQuest = GetLeadingMainLineQuest();
    if (ptQuest)
    {
        if (QuestStepIsEligibleForMatchmaking(ptQuest->snoQuest, ptQuest->nResumableStepUID))
            return TRUE;
        if (QuestStepIsSelectable(ptQuest->snoQuest, ptQuest->nResumableStepUID))
            return TRUE;
        if (!QuestStepCanReachReward(ptQuest->snoQuest, ptQuest->nResumableStepUID))
            return TRUE;
    }
    return FALSE;
}










































void SPlayerSetKiller(
    Player & tPlayer,
    const GBHandle & hGBKiller,
    SNO snoKiller,
    MonsterRarity eKillerRarity,
    const GBID * pRareNameGBIDs,
    SNO snoKilledIn)
{
    tPlayer.hGBKiller = hGBKiller;
    tPlayer.snoKiller = snoKiller;
    tPlayer.eKillerRarity = eKillerRarity;
    if (eKillerRarity == MR_RARE)
    {
        tPlayer.killerRareNameGBIDs[0] = pRareNameGBIDs[0];
        tPlayer.killerRareNameGBIDs[1] = pRareNameGBIDs[1];
    }
    else
    {
        memset(tPlayer.killerRareNameGBIDs, 0, sizeof(tPlayer.killerRareNameGBIDs));
    }
    tPlayer.snoKilledIn = snoKilledIn;
}

static const uint32 PERK_BUFF_POLL_TICK_INTERVAL = 900;
static void sSetPerkBuffPollTime(
    ActorCommonData & tACDPlayer)
{
    uint32 uPollTime = GameGetServerTime() + PERK_BUFF_POLL_TICK_INTERVAL;
    tACDPlayer.AttributesSetInt(ATTRIBUTE_PERK_BUFF_POLL_NEXT_TIME, uPollTime);
}



static void sResetPerkBuffPollTime(
    ActorCommonData & tACDPlayer)
{
    tACDPlayer.AttributesSetInt(ATTRIBUTE_PERK_BUFF_POLL_NEXT_TIME, 0);
}




static const uint32 LIMITED_DURATION_ITEM_POLL_TICK_INTERVAL = 900;
static void sSetLimitedDurationItemPollTime(
    ActorCommonData & tACDPlayer)
{
    uint32 uPollTime = GameGetServerTime() + LIMITED_DURATION_ITEM_POLL_TICK_INTERVAL;
    tACDPlayer.AttributesSetInt(ATTRIBUTE_LIMITED_DURATION_ITEM_POLL_NEXT_TIME, uPollTime);
}



static void sResetLimitedDurationItemPollTime(
    ActorCommonData & tACDPlayer)
{
    tACDPlayer.AttributesSetInt(ATTRIBUTE_LIMITED_DURATION_ITEM_POLL_NEXT_TIME, 0);
}



static void sSetCommunityFlags(
    ActorCommonData & tACDPlayer,
    BOOL bSetValue)
{
    Player * ptPlayer = PlayerGetByACD(tACDPlayer.id);
    if (ptPlayer != NULL)
    {
        BOOL bDoubleRiftKeystones = s_varCommunityEventPerkDoubleRiftKeystones.Get() && bSetValue;
        PlayerSetPlayerGameFlag(ptPlayer, "DoubleRiftKeystones", bDoubleRiftKeystones);
    }




    BOOL bActive = s_varCommunityEventPerkDoubleTreasureGoblins.Get() && bSetValue;

    SGameSetCommunityBuffDoubleGoblins(bActive);
}




static void sRemovePerkBuff(
    ActorCommonData & tACDPlayer,
    GlobalSNO eGlobalSNOPower,
    BOOL bSetCommunityFlags)
{
    SNO snoPower = GlobalSNOGet(eGlobalSNOPower);
    if (snoPower == SNO_NONE)
        return;




    POS pos = tACDPlayer.listBuffs.GetHeadPosition();
    while (pos)
    {
        BuffID idBuff = tACDPlayer.listBuffs.GetNext(pos);
        PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
        if (ptBuff && ptBuff->snoPower == snoPower)
        {
            PowerAPI_BuffFlagForDeletion(idBuff, FALSE, TRUE, FALSE, TRUE);

            if (bSetCommunityFlags)
            {
                sSetCommunityFlags(tACDPlayer, FALSE);
            }
        }
    }
}



static void sApplyPerkBuff(
    ActorCommonData & tACDPlayer,
    GlobalSNO eGlobalSNOPower,
    BOOL bSetCommunityFlags)
{

    SNO snoPower = GlobalSNOGet(eGlobalSNOPower);
    if (snoPower == SNO_NONE)
        return;





    BuffID idBuff = PowerAPI_BuffFindOnACD(snoPower, &tACDPlayer, ACDID_INVALID);
    if (idBuff != BUFFID_INVALID)
        return;




    PowerStartData tPowerStartData;
    PowerStartDataInit(tPowerStartData);
    tPowerStartData.idACD = tACDPlayer.id;
    tPowerStartData.snoPower = snoPower;
    PowerStartEx(tPowerStartData, ATTRIBGROUPID_INVALID, FALSE, FALSE);

    sSetPerkBuffPollTime(tACDPlayer);


    if (bSetCommunityFlags)
    {
        sSetCommunityFlags(tACDPlayer, TRUE);
    }
}
static void sApplyConsumablePotionBuff(
    ActorCommonData & tACDPlayer,
    ConsumablePotionBuffTypes eType,
    BOOL bBuffOn)
{
    SNO snoPower = GlobalSNOGet(GLOBALSNO_POWERS_CONSUMABLE_POTION_BUFFS);

    if (bBuffOn)
    {
        PowerAPIBuffStartData tBuffStartData(POWERID_INVALID);
        tBuffStartData.snoPower = snoPower;
        tBuffStartData.nBuffIndex = eType;
        tBuffStartData.eStackType = BUFF_STACKTYPE_NEVER;
        tBuffStartData.bInfinite = TRUE;
        tBuffStartData.bPositiveEffect = TRUE;

        PowerAPI_BuffAddToACD(tBuffStartData, tACDPlayer);

    }
    else
    {
        BuffID idBuff = PowerAPI_BuffFindOnACD(snoPower, eType, tACDPlayer, ACDID_INVALID, FALSE);
        if (idBuff != BUFFID_INVALID)
        {
            PowerAPI_BuffFlagForDeletion(idBuff, FALSE, TRUE, FALSE, TRUE);
        }
    }
}




void SPlayerSetPerkBuffs(
    float flXP,
    float flLegendaryFind,
    float flGoldFind,
    BOOL bDoubleTreasureGoblins,
    BOOL bDoubleBloodShards,
    BOOL bDoubleRiftKeystones,
    BOOL bEasterEggWorld,
    BOOL bDoubleBountyBags)
{
    s_varCommunityEventPerkEnabled.Set(flXP > EPSILON || flLegendaryFind > EPSILON || flGoldFind > EPSILON || bDoubleTreasureGoblins || bDoubleBloodShards || bDoubleRiftKeystones || bEasterEggWorld || bDoubleBountyBags, XVarBase::DIRTY_DEFAULT);
    s_varCommunityEventPerkXP.Set(flXP, XVarBase::DIRTY_DEFAULT);
    s_varCommunityEventPerkLegendaryFind.Set(flLegendaryFind, XVarBase::DIRTY_DEFAULT);
    s_varCommunityEventPerkGoldFind.Set(flGoldFind, XVarBase::DIRTY_DEFAULT);
    s_varCommunityEventPerkDoubleTreasureGoblins.Set(bDoubleTreasureGoblins != FALSE, XVarBase::DIRTY_DEFAULT);
    s_varCommunityEventPerkDoubleBloodShards.Set(bDoubleBloodShards != FALSE, XVarBase::DIRTY_DEFAULT);
    s_varCommunityEventPerkDoubleRiftKeystones.Set(bDoubleRiftKeystones != FALSE, XVarBase::DIRTY_DEFAULT);
    s_varEasterEggWorldPerkEnabled.Set(bEasterEggWorld != FALSE, XVarBase::DIRTY_DEFAULT);
    s_varCommunityEventPerkDoubleBountyBags.Set(bDoubleBountyBags != FALSE, XVarBase::DIRTY_DEFAULT);
}

void SPlayerGetCommunityBuffInfo(
    BOOL & bActive,
    float & flXPBuff,
    float & flLegendaryBuff,
    float & flGoldBuff,
    BOOL & bDoubleTreasureGoblins,
    BOOL & bDoubleBloodShards,
    BOOL & bDoubleRiftKeystones,
    BOOL & bEasterEggWorld,
    BOOL & bDoubleBountyBags)
{
    bActive = s_varCommunityEventPerkEnabled.Get();
    flXPBuff = s_varCommunityEventPerkXP.Get();
    flLegendaryBuff = s_varCommunityEventPerkLegendaryFind.Get();
    flGoldBuff = s_varCommunityEventPerkGoldFind.Get();
    bDoubleTreasureGoblins = s_varCommunityEventPerkDoubleTreasureGoblins.Get();
    bDoubleBloodShards = s_varCommunityEventPerkDoubleBloodShards.Get();
    bDoubleRiftKeystones = s_varCommunityEventPerkDoubleRiftKeystones.Get();
    bEasterEggWorld = s_varEasterEggWorldPerkEnabled.Get();
    bDoubleBountyBags = s_varCommunityEventPerkDoubleBountyBags.Get();
}




static void sPollPerkBuffs(
    Player & tPlayer,
    ActorCommonData & tACDPlayer)
{
    uint32 uNextPerkPoll = tACDPlayer.AttributesGetInt(ATTRIBUTE_PERK_BUFF_POLL_NEXT_TIME);
    if (uNextPerkPoll != 0 && GameGetServerTime() >= uNextPerkPoll)
    {
        BOOL bIgrEnabled = FALSE;
        BOOL bAnniversaryEnabled = s_varAnniversaryPerkEnabled.Get();
        BOOL bCommunityEventEnabled = s_varCommunityEventPerkEnabled.Get();
        BOOL bEasterEggWorldEnabled = s_varEasterEggWorldPerkEnabled.Get();

        ServerPlayerGlobals * ptServerPlayerGlobals = SGameManager::GameGet().ptServerPlayerGlobals;
        if (ptServerPlayerGlobals != NULL)
        {
            PerPlayerGlobals & tPerPlayer = ptServerPlayerGlobals->tPerPlayer[tPlayer.nPlayerIndex];
            if (tPerPlayer.uSessionFlags & 1)
            {
                bIgrEnabled = s_varIgrPerkEnabled.Get();
            }
        }

        if (!bIgrEnabled)
        {
            sRemovePerkBuff(tACDPlayer, GLOBALSNO_POWERS_IGR_XP, FALSE);
        }

        if (!bAnniversaryEnabled)
        {
            sRemovePerkBuff(tACDPlayer, GLOBALSNO_POWERS_ANNIVERSARY_XPMF, TRUE);
        }

        if (!bCommunityEventEnabled)
        {
            sRemovePerkBuff(tACDPlayer, GLOBALSNO_POWERS_COMMUNITY_EVENT_XPMF, TRUE);
        }

        if (!bEasterEggWorldEnabled)
        {
            sRemovePerkBuff(tACDPlayer, GLOBALSNO_POWERS_EASTER_EGG_WORLD, FALSE);
        }

        if (bIgrEnabled || bAnniversaryEnabled || bCommunityEventEnabled)
        {
            sSetPerkBuffPollTime(tACDPlayer);
        }
        else
        {
            sResetPerkBuffPollTime(tACDPlayer);
        }
    }
}



static void sPollLimitedDurationItems(
    Player & tPlayer,
    ActorCommonData & tACDPlayer)
{


    uint32 uNextPoll = tACDPlayer.AttributesGetInt(ATTRIBUTE_LIMITED_DURATION_ITEM_POLL_NEXT_TIME);
    if (uNextPoll != 0 && GameGetServerTime() >= uNextPoll)
    {
        BOOL bActiveBuffs = SPlayerApplyConsumablePotionBuffs(tACDPlayer);
        BOOL bActivePets = tPlayer.tCosmeticItemSavedData.tCosmeticItemData.UpdateLimitedDurationPets(tACDPlayer);

        if (bActiveBuffs || bActivePets || GameGetDailyPlatinumLimit() != 0)
        {
            sSetLimitedDurationItemPollTime(tACDPlayer);
        }
        else
        {
            sResetLimitedDurationItemPollTime(tACDPlayer);
        }
    }
}



void sCheckForCommunityBuffReset(
    ActorCommonData & tACDPlayer,
    const GlobalSNO eGlobalPowerSNO)
{
    SNO snoPower = GlobalSNOGet(eGlobalPowerSNO);
    if (snoPower == SNO_NONE)
        return;

    BOOL bBuffXPActive = PowerAPI_BuffFindOnACD(snoPower, 1, tACDPlayer, ACDID_INVALID, FALSE) != BUFFID_INVALID;
    BOOL bBuffLegendaryFindActive = PowerAPI_BuffFindOnACD(snoPower, 2, tACDPlayer, ACDID_INVALID, FALSE) != BUFFID_INVALID;
    BOOL bBuffGoldFindActive = PowerAPI_BuffFindOnACD(snoPower, 3, tACDPlayer, ACDID_INVALID, FALSE) != BUFFID_INVALID;
    BOOL bBuffDoubleRiftKeystones = PowerAPI_BuffFindOnACD(snoPower, 4, tACDPlayer, ACDID_INVALID, FALSE) != BUFFID_INVALID;
    BOOL bBuffDoubleBloodShards = PowerAPI_BuffFindOnACD(snoPower, 5, tACDPlayer, ACDID_INVALID, FALSE) != BUFFID_INVALID;
    BOOL bBuffDoubleTreasureGoblins = PowerAPI_BuffFindOnACD(snoPower, 6, tACDPlayer, ACDID_INVALID, FALSE) != BUFFID_INVALID;
    BOOL bBuffDoubleBountyBags = PowerAPI_BuffFindOnACD(snoPower, 7, tACDPlayer, ACDID_INVALID, FALSE) != BUFFID_INVALID;

    if (bBuffXPActive != (s_varCommunityEventPerkXP.Get() > EPSILON) ||
        bBuffLegendaryFindActive != (s_varCommunityEventPerkLegendaryFind.Get() > EPSILON) ||
        bBuffGoldFindActive != (s_varCommunityEventPerkGoldFind.Get() > EPSILON) ||
        bBuffDoubleRiftKeystones != s_varCommunityEventPerkDoubleRiftKeystones.Get() ||
        bBuffDoubleBloodShards != s_varCommunityEventPerkDoubleBloodShards.Get() ||
        bBuffDoubleTreasureGoblins != s_varCommunityEventPerkDoubleTreasureGoblins.Get() ||
        bBuffDoubleBountyBags != s_varCommunityEventPerkDoubleBountyBags.Get())
    {
        sRemovePerkBuff(tACDPlayer, eGlobalPowerSNO, TRUE);

    }
}



void SPlayerApplyPerkBuffs(
    ActorCommonData & tACDPlayer)
{
    Player * ptPlayer = PlayerGetByACD(tACDPlayer.id);
    if (ptPlayer == NULL)
        return;

    ServerPlayerGlobals * ptServerPlayerGlobals = SGameManager::GameGet().ptServerPlayerGlobals;
    if (ptServerPlayerGlobals == NULL)
        return;

    if (ptPlayer->nPlayerIndex < 0 || ptPlayer->nPlayerIndex >= MAX_PLAYERS)
        return;


    BOOL bAnniversaryEnabled = s_varAnniversaryPerkEnabled.Get();
    BOOL bCommunityEventEnabled = s_varCommunityEventPerkEnabled.Get();
    BOOL bEasterEggWorldEnabled = s_varEasterEggWorldPerkEnabled.Get();

    PerPlayerGlobals & tPerPlayer = ptServerPlayerGlobals->tPerPlayer[ptPlayer->nPlayerIndex];
    BOOL bIgrEnabled = (tPerPlayer.uSessionFlags & 1) &&

        s_varIgrPerkEnabled.Get();


    if (bIgrEnabled)
    {
        sApplyPerkBuff(tACDPlayer, GLOBALSNO_POWERS_IGR_XP, FALSE);
    }

    if (bAnniversaryEnabled)
    {
        sCheckForCommunityBuffReset(tACDPlayer, GLOBALSNO_POWERS_ANNIVERSARY_XPMF);
        sApplyPerkBuff(tACDPlayer, GLOBALSNO_POWERS_ANNIVERSARY_XPMF, TRUE);
    }

    if (bCommunityEventEnabled)
    {
        sCheckForCommunityBuffReset(tACDPlayer, GLOBALSNO_POWERS_COMMUNITY_EVENT_XPMF);
        sApplyPerkBuff(tACDPlayer, GLOBALSNO_POWERS_COMMUNITY_EVENT_XPMF, TRUE);
    }

    if (bEasterEggWorldEnabled)
    {
        sApplyPerkBuff(tACDPlayer, GLOBALSNO_POWERS_EASTER_EGG_WORLD, FALSE);
    }
}




BOOL SPlayerApplyConsumablePotionBuffs(
    ActorCommonData & tACDPlayer)
{
    Player * ptPlayer = PlayerGetByACD(tACDPlayer.id);

    {
        const OnlineService::ContentLicense * ptXPBoostLicense = SPlayerGetContentLicense(*ptPlayer, OnlineService::CONTENT_LICENSE_BOOST_EXPERIENCE);

        if (ptXPBoostLicense)
        {
            tACDPlayer.AttributesSetInt(ATTRIBUTE_XP_POTION_BUFF_EXPIRATION, Blizzard::Time::ToUnixTime(ptXPBoostLicense->expireTime));
        }
        else
        {
            tACDPlayer.AttributesSetInt(ATTRIBUTE_XP_POTION_BUFF_EXPIRATION, 0);
        }
    }

    {
        const OnlineService::ContentLicense * ptBloodShardBoostLicense = SPlayerGetContentLicense(*ptPlayer, OnlineService::CONTENT_LICENSE_BOOST_BLOODSHARD);

        if (ptBloodShardBoostLicense)
        {
            tACDPlayer.AttributesSetInt(ATTRIBUTE_BLOODSHARD_POTION_BUFF_EXPIRATION, Blizzard::Time::ToUnixTime(ptBloodShardBoostLicense->expireTime));
        }
        else
        {
            tACDPlayer.AttributesSetInt(ATTRIBUTE_BLOODSHARD_POTION_BUFF_EXPIRATION, 0);
        }
    }

    {
        const OnlineService::ContentLicense * ptGoldFindBoostLicense = SPlayerGetContentLicense(*ptPlayer, OnlineService::CONTENT_LICENSE_BOOST_GOLDFIND);

        if (ptGoldFindBoostLicense)
        {
            tACDPlayer.AttributesSetInt(ATTRIBUTE_GOLD_FIND_POTION_BUFF_EXPIRATION, Blizzard::Time::ToUnixTime(ptGoldFindBoostLicense->expireTime));
        }
        else
        {
            tACDPlayer.AttributesSetInt(ATTRIBUTE_GOLD_FIND_POTION_BUFF_EXPIRATION, 0);
        }
    }

    uint32 nNow = Blizzard::Time::ToUnixTime(OnlineServiceServer::GetCurrentTime());

    BOOL bXPActive = (uint32)tACDPlayer.AttributesGetInt(ATTRIBUTE_XP_POTION_BUFF_EXPIRATION) > nNow;
    BOOL bGFActive = (uint32)tACDPlayer.AttributesGetInt(ATTRIBUTE_GOLD_FIND_POTION_BUFF_EXPIRATION) > nNow;
    BOOL bBSActive = (uint32)tACDPlayer.AttributesGetInt(ATTRIBUTE_BLOODSHARD_POTION_BUFF_EXPIRATION) > nNow;

    tACDPlayer.AttributesSetFloat(ATTRIBUTE_EXPERIENCE_BONUS_PCT_POTION_BUFF, bXPActive ? 0.25f : 0.0f);
    tACDPlayer.AttributesSetFloat(ATTRIBUTE_GOLD_FIND_POTION_BUFF, bGFActive ? 0.25f : 0.0f);
    tACDPlayer.AttributesSetFloat(ATTRIBUTE_BLOODSHARD_BONUS_PCT_POTION_BUFF, bBSActive ? 0.25f : 0.0f);

    BOOL bAnyActive = bXPActive || bGFActive || bBSActive;

    SNO snoPower = GlobalSNOGet(GLOBALSNO_POWERS_CONSUMABLE_POTION_BUFFS);

    if (bAnyActive)
    {

        BuffID idBuff = PowerAPI_BuffFindOnACD(snoPower, &tACDPlayer, ACDID_INVALID);
        if (idBuff == BUFFID_INVALID)
        {

            PowerAPIBuffStartData tBuffStartData(POWERID_INVALID);
            tBuffStartData.snoPower = snoPower;
            tBuffStartData.nBuffIndex = 0;
            tBuffStartData.eStackType = BUFF_STACKTYPE_NEVER;
            tBuffStartData.bInfinite = TRUE;
            tBuffStartData.bPositiveEffect = TRUE;

            PowerAPI_BuffAddToACD(tBuffStartData, tACDPlayer);
        }
    }

    if (bAnyActive || GameGetDailyPlatinumLimit() != 0)
    {
        sSetLimitedDurationItemPollTime(tACDPlayer);
    }

    sApplyConsumablePotionBuff(tACDPlayer, 1, bXPActive);
    sApplyConsumablePotionBuff(tACDPlayer, 2, bGFActive);
    sApplyConsumablePotionBuff(tACDPlayer, 3, bBSActive);

    if (!bAnyActive)
    {
        BuffID idBuff = PowerAPI_BuffFindOnACD(snoPower, &tACDPlayer, ACDID_INVALID);
        if (idBuff != BUFFID_INVALID)
        {
            PowerAPI_BuffFlagForDeletion(idBuff, FALSE, TRUE, FALSE, TRUE);
        }
    }

    return bAnyActive;
}




void SPlayerApplyParagonBuff(
    ActorCommonData & tACDPlayer)
{
    Player * ptPlayer = PlayerGetByACD(tACDPlayer.id);
    if (!ptPlayer)
        return;

    int32 nAltLevel = tACDPlayer.AttributesGetInt(ATTRIBUTE_ALT_LEVEL);
    if (nAltLevel > 0)
    {
        SNO snoPower = GlobalSNOGet(GLOBALSNO_POWERS_PARAGON_BUFF);

        PowerAPIBuffStartData tBuffStartData(POWERID_INVALID);
        tBuffStartData.snoPower = snoPower;
        tBuffStartData.eStackType = BUFF_STACKTYPE_NEVER;
        tBuffStartData.bPositiveEffect = TRUE;
        tBuffStartData.bKeepForProfileSnapshot = TRUE;
        tBuffStartData.bInfinite = TRUE;

        POS pos;
        GBHandleList listBonuses(GBGetHandlePool());
        GBEnumerate(GB_PARAGON_BONUSES, listBonuses);

        pos = listBonuses.GetHeadPosition();
        while (pos)
        {
            const GBHandle & hGBBonus = listBonuses.GetNext(pos);

            int32 nPoints = tACDPlayer.AttributesGetInt(MakeAttribKey(ATTRIBUTE_PARAGON_BONUS, hGBBonus.gbid));
            if (nPoints)
            {
                ScopedParagonBonusLockR pdefParagonBonus(hGBBonus.gbid);

                for (int32 i = 0; i < 4; i++)
                {
                    const AttributeSpecifier & tSpecifier = pdefParagonBonus->ptAttributes[i];
                    if (tSpecifier.eAttribute > AXEBADDATA && tSpecifier.eAttribute < NUM_ATTRIBUTES)
                    {

                        float flAttributeValue = AffixGetFormulaValue(tSpecifier.pbAttributeFormula,
                            tSpecifier.serAttributeFormula.dwSize,
                            &tACDPlayer, GBID_NONE, NULL, 0, 0);

                        flAttributeValue *= nPoints;

                        FastAttribKey tKey = MakeAttribKey(tSpecifier.eAttribute, tSpecifier.nParam);
                        if (KeyGetDataType(tKey) == ATTRIBUTEDT_FLOAT)
                        {

                            FastAttribSetValueFloat(tBuffStartData.idFastAttrib, tKey, flAttributeValue);
                        }
                        else
                        {

                            FastAttribSetValueInt(tBuffStartData.idFastAttrib, tKey, Math::FloatToInt(flAttributeValue));
                        }
                    }
                }
            }
        }

        PowerAPI_BuffAddToACD(tBuffStartData, tACDPlayer);
    }
}





void SPlayerApplyEquippedLegendaryPowerBuffs(
    ActorCommonData & tACDPlayer)
{
    Player * ptPlayer = PlayerGetByACD(tACDPlayer.id);
    if (!ptPlayer)
        return;

    for (int32 i = 0; i < 3; i++)
    {
        GBID gbidLegendary = ptPlayer->tHeroStateData.tSavedData.gbidLegendaryPowers[i];
        if (gbidLegendary == GBID_NONE)
            continue;

        SNO snoPower = ItemGetLegendaryPower(gbidLegendary);
        if (FastAttribExists(tACDPlayer.idFastAttrib, MakeAttribKey(ATTRIBUTE_ITEM_POWER_PASSIVE, snoPower)))
            continue;

        PowerAPIBuffStartData tBuffStartData(POWERID_INVALID);
        tBuffStartData.idACDSource = tACDPlayer.id;
        tBuffStartData.snoPower = GlobalSNOGet(GLOBALSNO_POWERS_EQUIPPED_LEGENDARY_POWER);
        tBuffStartData.bPositiveEffect = TRUE;
        tBuffStartData.bRemoveOnDeath = FALSE;
        tBuffStartData.bRemoveOnPassiveApplication = TRUE;
        tBuffStartData.eStackType = BUFF_STACKTYPE_NEVER;
        tBuffStartData.nBuffIndex = i;
        tBuffStartData.bInfinite = TRUE;
        float flPowerValue = ItemGetLegendaryPowerValue(gbidLegendary);
        FastAttribSetValueFloat(tBuffStartData.idFastAttrib, MakeAttribKey(ATTRIBUTE_ITEM_POWER_PASSIVE, snoPower), flPowerValue);
        PowerAPI_BuffAddToACD(tBuffStartData, tACDPlayer);
    }
}

















static void sApplyMultiplayerBuff(
    const Player & tPlayer)
{
    ActorCommonData * ptACDPlayer = ACDTryToGet(tPlayer.idPlayerACD);
    if (!ptACDPlayer)
    {
        return;
    }

    SNO snoPower = GlobalSNOGet(GLOBALSNO_POWERS_MULTIPLAYER_BUFF);
    if (snoPower != SNO_NONE)
    {

        PowerStartData tPowerStartData;
        PowerStartDataInit(tPowerStartData);
        tPowerStartData.idACD = ptACDPlayer->id;
        tPowerStartData.snoPower = snoPower;
        PowerStartEx(tPowerStartData, -1, FALSE, FALSE);



        PlayerList listDestPlayers(PlayerListGetElementPool());
        PlayerListGetSingle(listDestPlayers, tPlayer);
        SimpleMessage tMessage;
        MessageSendToClient(listDestPlayers, SMSG_MULTIPLAYER_BUFF_ON, &tMessage, sizeof(tMessage));
    }
}


static void sRemoveMultiplayerBuff(
    const Player & tPlayer)
{
    ActorCommonData * ptACDPlayer = ACDTryToGet(tPlayer.idPlayerACD);
    if (!ptACDPlayer)
    {
        return;
    }


    SNO snoPower = GlobalSNOGet(GLOBALSNO_POWERS_MULTIPLAYER_BUFF);
    if (snoPower != SNO_NONE)
    {
        BuffID idBuff = PowerAPI_BuffFindOnACD(snoPower, ptACDPlayer, ACDID_INVALID);
        if (idBuff != BUFFID_INVALID)
        {
            PowerAPI_BuffFlagForDeletion(idBuff, FALSE, TRUE, FALSE, TRUE);



            PlayerList listDestPlayers(PlayerListGetElementPool());
            PlayerListGetSingle(listDestPlayers, tPlayer);
            SimpleMessage tMessage;
            MessageSendToClient(listDestPlayers, SMSG_MULTIPLAYER_BUFF_OFF, &tMessage, sizeof(tMessage));
        }
    }
}


void SPlayerEvaluateMultiplayerBuff()

{

    SGameGlobals & tSGameGlobals = SGameGlobalsGet();
    if (tSGameGlobals.dwFlags & (1 << SGAMEGLOBALS_SHUTTING_DOWN))
    {
        return;
    }

    if (GameTypeIsPvP())
    {
        return;
    }

    int32 nPlayers = PlayerGetNumPlayers();


    if (nPlayers >= 2)
    {
        for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
        {


            if (PlayerIsHardcoreAndDead(*ptPlayer))
            {
                nPlayers--;
            }
        }
    }



    if (nPlayers >= 2)
    {
        for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
        {


            sApplyMultiplayerBuff(*ptPlayer);
        }
    }
    else
    {
        for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
        {


            sRemoveMultiplayerBuff(*ptPlayer);
        }
    }
}





void SPlayerSendKickTimers(
    Player & tPlayer)
{
    PlayerKickTimerMessage tMessage;
    tMessage.dwLastKickAttempt = tPlayer.dwLastKickAttempt;
    tMessage.dwLastSuccessfulKick = tPlayer.dwLastSuccessfulKick;
    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetSingle(listDestPlayers, tPlayer);
    MessageSendToClient(listDestPlayers, SMSG_PLAYER_KICK_TIMERS, &tMessage, sizeof(tMessage));
}




BOOL SPlayerFindValidWalkableSpotNearPlayer(
    const ActorCommonData & tACDPlayer,
    WorldPlace & placeSpot)
{
    placeSpot = tACDPlayer.GetPlace();
    Vector3D wpFound;
    NavCellHandle hNavCellFound;
    if (WorldCollSearchForLocation(placeSpot, 1, NavMeshGetDynamicFlagsForPassability(PASSABILITY_PLAYER_AI_OFF) | 8, 1.25f, 8.0f, 12.0f, ACDID_INVALID, wpFound, hNavCellFound, TEAM_NEUTRAL, 0x860, ACDID_INVALID))
        placeSpot.wp = wpFound;

    if (NavCellGetStaticFlags(placeSpot) & 1)
        return TRUE;

    NavCellHandle hNavCell;
    if (!NavCellHandleFromWorldPlace(placeSpot, hNavCell))
        return FALSE;
    if (!NavCellModifyForRoundedCorners(NavCellHandleToZoneInstance(hNavCell), hNavCell, (Vector2D &)placeSpot.wp, 1))
        return FALSE;
    return (NavCellGetStaticFlags(placeSpot) & 1);
}



































void SPlayerRestoreBuffs(
    Player & tPlayer)
{
    ServerPlayerGlobals * ptServerPlayerGlobals = SGameManager::GameGet().ptServerPlayerGlobals;

    for (size_t i = 0; i < ptServerPlayerGlobals->tPreviousSessions.Count(); i++)
    {
        SessionTrackedData & tData = ptServerPlayerGlobals->tPreviousSessions[i];
        if (tData.uHeroId == tPlayer.uHeroId &&
            tData.dwLastPlayedAmount == tPlayer.tHeroStateData.tSavedData.uSecondsPlayed)
        {
            SNO snoNephalemBuffPower = GlobalSNOGet(GLOBALSNO_POWERS_KILLELITEPACK);

            for (uint32 j = 0; j < tData.nNumNephalemStacks; j++)
            {
                PowerStart(tPlayer.idPlayerACD, snoNephalemBuffPower, cgTargetNone);
            }

            ActorCommonData & tACDPlayer = ACDGet(tPlayer.idPlayerACD);
            POS pos = tACDPlayer.listBuffs.GetHeadPosition();
            while (pos)
            {
                BuffID idBuff = tACDPlayer.listBuffs.GetNext(pos);

                PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
                if (ptBuff && !ptBuff->bDeleted && ptBuff->snoPower == snoNephalemBuffPower)
                {
                    PowerAPI_BuffBackdate(idBuff, ptBuff->dwDuration - tData.dwNephalemBuffDurationRemaining);
                }
            }
            return;
        }
    }
}





























































































void SPlayerAddNephalemBuffStack(
    ACDID idACDPlayer)
{
}




















BOOL SPlayerTryIdentifyAllWithCast(
    ACDID idACD,
    ACDID idGizmoTarget)
{
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (!ptACD)
        return FALSE;




    SNO snoPowerIdentifyAll = GlobalSNOGet(GLOBALSNO_POWERS_IDENTIFY_ALL_WITH_CAST);


    PowerID idPowerPrimary = ActorPowersGetPrimary((ActorID)ACDGetOwner(idACD));
    Power * ptPower = PowerTryToGet(idPowerPrimary);
    if (ptPower && ptPower->snoPower == snoPowerIdentifyAll)
        return FALSE;





    if (ItemCrafting_GetUnidentifiedItemCount(*ptACD) <= 0)
    {
        PlayerDisplayErrorMessage(idACD, GAMEERROR_IDENTIFY_ALL_NO_ITEMS);
        return FALSE;
    }



    if (!PowerCanUseNow(*ptACD, AutoPowerDefinitionR(snoPowerIdentifyAll), NULL, NULL))
        return FALSE;





    Target tTarget = cgTargetNone;
    ActorCommonData * ptACDGizmoTarget = ACDTryToGet(idGizmoTarget);
    if (ptACDGizmoTarget)
    {

        tTarget.InitializeFromACD(idGizmoTarget, 0, 0);
    }



    PowerID idPower = PowerStart(idACD, snoPowerIdentifyAll, tTarget);
    return idPower != POWERID_INVALID;
}


















void SPlayerSetLastWaypointLevelArea(
    Player * ptPlayer,
    const SNO snoLevelArea)
{
    if (ptPlayer->snoLastWaypointLevelArea == snoLevelArea)
        return;
    ptPlayer->snoLastWaypointLevelArea = snoLevelArea;
    ACDGet(ptPlayer->idPlayerACD).MarkHeroStateLastWaypointLevelAreaChanged();
}






void SPlayerSetGameLeaderAccount(
    const OnlineService::GameAccountHandle & tAccountId)
{
    ServerPlayerGlobals * ptServerPlayerGlobals = SGameManager::GameGet().ptServerPlayerGlobals;
    ptServerPlayerGlobals->uGameLeaderAccountId = tAccountId.m_id;
}
















void sHandlePlayerAchievementScoreUpdate(
    Player * ptPlayer,
    OnlineService::AchievementId achievementId)
{
    if (!GameIsSeasonal())
        return;
    Console::Achievements * ptAchievements = sgtAppGlobals.ptAchievements;
    const OnlineService::AchievementDefinition * ptAchievementDef = ptAchievements->GetAchievementDefinition(achievementId);
    if (ptAchievementDef == NULL)
        return;
    if (!ptAchievements->IsAchievementValidForSeason(ptAchievementDef, GameGetCurrentSeason()))
        return;
    int32 nPoints = ptAchievementDef->m_pointsValue;
    if (nPoints > 0)
    {
        Achievement::EvaluationData tEvaluationData;
        tEvaluationData.SetPlayer(ptPlayer);
        tEvaluationData.EvaluateEvent(ptPlayer, Achievement::ACH_EVENT_PLAYER_EARNED_SEASONAL_ACHIEVEMENT_POINTS, nPoints, (uint32)sub_71007b5e70());
    }
}
























bool sPlayerGiveRewardCurrency(
    OnlineService::AchievementId achievementId,
    CurrencyType currencyType,
    uint32 quantity,
    Player & player)
{
    if (currencyType == CURRENCY_INVALID || currencyType == CURRENCY_PLATINUM)
        return false;
    ActorCommonData * ptACDPlayer = ACDTryToGet(player.idPlayerACD);
    if (ptACDPlayer == NULL)
        return false;

    PlatinumAchievementAwardedMessage tMessage;
    tMessage.uPlatinumIncrement = quantity;
    tMessage.uCurrentPlatinum = ptACDPlayer->GetCurrencyAmount(CURRENCY_PLATINUM);
    tMessage.idAchievement = achievementId;
    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetSingle(listDestPlayers, player);
    MessageSendToClient(listDestPlayers, SMSG_PLATINUM_ACHIEVEMENT_AWARDED, &tMessage, sizeof(tMessage));
    ptACDPlayer->ModifyCurrencyAmount(quantity, currencyType, GMR_ACHIEVEMENT, FALSE);
    SGameStatsPlayerCounterIncrement(&player, STAT_COUNTER_PLAYER_CURRENCY_RECEIVED_ACHIEVEMENT, CURRENCY_PLATINUM, (int32)achievementId, (float)quantity);
    SGameStatsPlayerCounterIncrement(&player, STAT_COUNTER_PLAYER_CURRENCY_PICKED_UP, CURRENCY_PLATINUM, (int32)achievementId, (float)quantity);
    return true;
}










































































































bool sGrantAchievementReward(
    Player * ptPlayer,
    OnlineService::AchievementId achievementId,
    bool onlyGrantNonSeasonally)
{
    int significantItemsGranted = 0;
    Console::Achievements & tAchievements = *sgtAppGlobals.ptAchievements;

    OnlineService::RewardIdList aridRewards;
    tAchievements.GetAchievementRewards(achievementId, aridRewards);

    for (uint32 i = 0; i < aridRewards.size(); i++)
    {
        bool failedToGrant = false;
        OnlineService::RewardDefinition tReward;
        if (!tAchievements.GetRewardDefinition(aridRewards[i], tReward))
            return false;

        switch (tReward.m_type)
        {
        case OnlineService::REWARD_TYPE_ITEM_TRANSMOG:
            SItemCrafting_TryUnlockTransmog(tReward.m_id, *ptPlayer, FALSE, TRUE);
            break;
        case OnlineService::REWARD_TYPE_ITEM_CURRENCY:
            if (!sPlayerGiveRewardCurrency(achievementId, tReward.m_currency, tReward.m_quantity, *ptPlayer))
                failedToGrant = true;
            break;
        case OnlineService::REWARD_TYPE_COSMETIC:
            sPlayerGiveRewardCosmetic(*ptPlayer, tReward.m_id, achievementId);
            break;
        case OnlineService::REWARD_TYPE_ITEM:
            break;
        case OnlineService::REWARD_TYPE_STASH_TAB:
            if (!SACDInventoryRewardStashTab(ptPlayer->idPlayerACD))
            {
                ActorCommonData & tACDPlayer = ACDGet(ptPlayer->idPlayerACD);
                int32 nTabs = GetSeasonalRewardStashTabsCountFromAttributeValue(tACDPlayer.AttributesGetInt(ATTRIBUTE_STASH_TABS_REWARDED_FROM_SEASONS));
                if (nTabs < MAX_STASH_TABS_ACHIEVEMENT_REWARDS)
                    failedToGrant = true;
            }
            break;
        }

        if (failedToGrant)
            return false;
    }

    return true;
}
















































































































void SPlayerReconcileGuaranteedRewards(
    Player & player)
{
}


















































































































void SPlayerAchievementEarned(
    const OnlineService::GameAccountHandle & tAccountId,
    const OnlineService::AchievementId & achievementId)
{
    Player * ptPlayer = PlayerGetByGameAccount(tAccountId);
    if (ptPlayer)
    {
        ServerPlayerGlobals * ptServerPlayerGlobals = SGameManager::GameGet().ptServerPlayerGlobals;
        PerPlayerGlobals & tPerPlayer = ptServerPlayerGlobals->tPerPlayer[ptPlayer->nPlayerIndex];
        tPerPlayer.tRecentAchievementsEarned.push_back(achievementId);
        tPerPlayer.uLastAchievementEarnedTime = GameGetServerTime();
        sHandlePlayerAchievementScoreUpdate(ptPlayer, achievementId);
        sGrantAchievementReward(ptPlayer, achievementId, false);
    }
}











BOOL SPlayerIsLocal(
    const Player & tPlayer)
{
    return tPlayer.bIsLocalToServerServerOnly;
}









BOOL SPlayersAreAllLocal()
{
    for (Player * ptPlayer = PlayerGetFirstAll(); ptPlayer != NULL; ptPlayer = PlayerGetNextAll(ptPlayer))
    {
        if (!SPlayerIsLocal(*ptPlayer))
            return FALSE;
    }
    return TRUE;
}










void TickRateTracker::Initialize()
{
    for (uint32 i = 0; i < TICKRATE_TRACKER_MAX_BUCKETS; i++)
    {
        m_arBuckets[i].Initialize();
    }
    m_nClientTickTotal = 0;
    m_nServerTickTotal = 0;
    m_nCurIndex = 0;
}


void TickRateTracker::AddBucket(
    int32 nClientTickDelta,
    int32 nServerTickDelta)
{
    TickRateBucket & tBucket = m_arBuckets[m_nCurIndex];
    nClientTickDelta = Math::Max(nClientTickDelta, 0);
    nServerTickDelta = Math::Max(nServerTickDelta, 0);
    m_nClientTickTotal -= tBucket.nClientTickDelta;
    m_nServerTickTotal -= tBucket.nServerTickDelta;
    tBucket.nClientTickDelta = nClientTickDelta;
    tBucket.nServerTickDelta = nServerTickDelta;
    m_nClientTickTotal += nClientTickDelta;
    m_nServerTickTotal += nServerTickDelta;
    if (++m_nCurIndex >= sizeof(m_arBuckets) / sizeof(m_arBuckets[0]))
        m_nCurIndex = 0;
}























static void sSendCounterUpdate(
    PlayerList & listPlayers,
    uint32 & uValue,
    StatCounterType eCounter)
{
    if (uValue != 0)
    {
        ConsoleCounterIncrementMessage tMsg;
        tMsg.nCounter = eCounter; tMsg.uValue = uValue;

        MessageSendToClient(listPlayers, SMSG_CONSOLE_COUNTER_INCREMENT, &tMsg, sizeof(tMsg));
        uValue = 0;
    }
}

void SPlayerConsoleSync(
    Player & tPlayer)
{
    PlayerList listPlayers(PlayerListGetElementPool());
    PlayerListGetSingle(listPlayers, tPlayer);

    if (!(tPlayer.uConsoleSyncFlags & (1 << PCSF_SAVED_QUESTS_ELIGIBLE_IN_GAME_LONG_ENOUGH)))
    {

        uint32 dwNow = GameGetServerTime();
        if (tPlayer.dwJoinTime + 4 * 60 * 60 < dwNow)
        {

            tPlayer.uConsoleSyncFlags |= (1 << PCSF_SAVED_QUESTS) | (1 << PCSF_SAVED_QUESTS_ELIGIBLE_IN_GAME_LONG_ENOUGH);
        }
    }

    if (tPlayer.uConsoleSyncFlags & (1 << PCSF_SAVED_QUESTS))
    {
        PlayerSavedQuestUpdateMessage tMsg;
        tMsg.snoLastPlayedQuest = SNO_NONE;
        tMsg.nLastPlayedStepUID = -1;
        tMsg.nHandicapLevel = -1;

        QuestInstance * pCurrentQuest = GetLeadingMainLineQuest();

        tMsg.dwNumQuests = 0;
        for (int32 i = 0; i < 64; i++)
        {
            if (tPlayer.tHeroStateData.tSavedQuest[i].snoQuest != SNO_NONE)
            {





                if (pCurrentQuest && tPlayer.tHeroStateData.tSavedQuest[i].snoQuest == pCurrentQuest->snoQuest &&
                    !SPlayerIsEligibleForQuestState(tPlayer))
                {
                    continue;
                }

                tMsg.tSavedQuest[tMsg.dwNumQuests++] = tPlayer.tHeroStateData.tSavedQuest[i];
            }
        }





        BOOL bGameOver = SPlayerGetMaxAct(tPlayer) < GameGetCurrentAct() || QuestIsGameOver();
        if (bGameOver && !GameTypeIsPvP() && !GameIsOpenWorld())
        {
            tMsg.eLastPlayedAct = ACT_5;
        }
        else if (SPlayerIsEligibleForQuestState(tPlayer) && !GameTypeIsPvP() && !GameIsOpenWorld())
        {
            tMsg.eLastPlayedAct = tPlayer.tHeroStateData.eLastPlayedAct;

            if (pCurrentQuest)
            {
                tMsg.snoLastPlayedQuest = pCurrentQuest->snoQuest;
                tMsg.nLastPlayedStepUID = pCurrentQuest->nResumableStepUID;

                QuestGlobals * ptQuestGlobals = GameGetActiveGameCommonData().ptQuestGlobals;
                if (ptQuestGlobals)
                {

                    tMsg.nHandicapLevel = Math::Min(GameGetHandicapLevel(), ptQuestGlobals->nCurrentMainLineQuestHandicapLevel);
                }
            }
        }
        else
        {


            tMsg.eLastPlayedAct = tPlayer.tPlayerResumeStateUponJoining.eLastPlayedAct;
            tMsg.snoLastPlayedQuest = tPlayer.tPlayerResumeStateUponJoining.snoLastPlayedQuest;
            tMsg.nLastPlayedStepUID = tPlayer.tPlayerResumeStateUponJoining.nLastPlayedQuestStep;
            tMsg.nHandicapLevel = tPlayer.tPlayerResumeStateUponJoining.nHandicapLevelSnapshot;
        }

        MessageSendToClient(listPlayers, SMSG_CONSOLE_SYNC_HERO_SAVED_QUESTS, &tMsg, sizeof(tMsg));
    }

    if (tPlayer.uConsoleSyncFlags & (1 << PCSF_QUEST_HISTORY))
    {
        PlayerQuestHistoryUpdateMessage tMsg;
        tMsg.dwNumEntries = 0;


        for (int32 i = 0; i < 200; i++)
        {
            if (tPlayer.tHeroStateData.tQuestHistory[i].snoQuest != SNO_NONE)
            {
                tMsg.tQuestHistory[tMsg.dwNumEntries++] = tPlayer.tHeroStateData.tQuestHistory[i];
            }
        }

        MessageSendToClient(listPlayers, SMSG_CONSOLE_SYNC_HERO_QUEST_HISTORY, &tMsg, sizeof(tMsg));
    }

    if (tPlayer.uConsoleSyncFlags & (1 << PCSF_QUEST_REWARD_HISTORY))
    {
        PlayerQuestRewardHistoryUpdateMessage tMsg;
        tMsg.nNumQuestRewardEntries = tPlayer.tHeroStateData.nNumQuestRewardEntries;
        for (int32 i = 0; i < tMsg.nNumQuestRewardEntries; i++)
        {
            tMsg.tQuestRewardHistory[i] = tPlayer.tHeroStateData.tQuestRewardHistory[i];
        }

        MessageSendToClient(listPlayers, SMSG_CONSOLE_SYNC_HERO_QUEST_REWARD_HISTORY, &tMsg, sizeof(tMsg));
    }

    if (tPlayer.uConsoleSyncFlags & (1 << PCSF_SAVED_CONVERSATIONS))
    {
        PlayerSavedConversationsUpdateMessage tMsg;
        tMsg.tSavedConversations = tPlayer.tHeroStateData.tSavedData.tSavedConversations;

        MessageSendToClient(listPlayers, SMSG_CONSOLE_SYNC_HERO_SAVED_CONVERSATIONS, &tMsg, sizeof(tMsg));
    }

    if (tPlayer.uConsoleSyncFlags & (1 << PCSF_ACTIVE_HIRELING))
    {
        IntDataMessage tMsg;
        tMsg.nData = tPlayer.tHirelingStable.eActiveHireling;

        MessageSendToClient(listPlayers, SMSG_CONSOLE_SYNC_HERO_ACTIVE_HIRELING, &tMsg, sizeof(tMsg));
    }

    if (tPlayer.uConsoleSyncFlags & (1 << PCSF_PARTITION_FLAGS))
    {
        IntDataMessage tMsg;
        tMsg.nData = tPlayer.tAccount.uPartitionFlags;

        MessageSendToClient(listPlayers, SMSG_CONSOLE_SYNC_HERO_PARTITION_FLAGS, &tMsg, sizeof(tMsg));
    }

    if (tPlayer.uConsoleSyncFlags & (1 << PCSF_RANDOM_SEEDS))
    {
        D3::Hero::ConsoleRandomVendorSeedList tData;

        for (auto & tItemSeedPair : tPlayer.tMapVendorSeeds)
        {
            D3::Hero::ConsoleRandomVendorSeed * ptSeed = tData.add_console_random_vendor_seed();
            ptSeed->set_item_slot_key(tItemSeedPair.first);
            ptSeed->set_random_item_seed(tItemSeedPair.second.first);
            ptSeed->set_random_item_carry(tItemSeedPair.second.second);
        }

        MessageSendToClient(listPlayers, SMSG_UPDATED_VENDOR_RANDOM_SEEDS, tData);
    }

    if (tPlayer.uConsoleSyncFlags & (1 << PCSF_PARTITION_WIDE_CUBE_SEEDS))
    {
        D3::Account::ConsoleRandomTransmuteSeedList tData;

        for (auto & tItemSeedPair : tPlayer.tMapPartitionWideCubeSeeds)
        {
            D3::Account::ConsoleRandomTransmuteSeed * ptSeed = tData.add_console_random_transmute_seed();
            ptSeed->set_item_slot_key(tItemSeedPair.first);
            ptSeed->set_random_item_seed(tItemSeedPair.second.first);
            ptSeed->set_random_item_carry(tItemSeedPair.second.second);
        }

        MessageSendToClient(listPlayers, SMSG_UPDATED_PARTITION_WIDE_CUBE_RANDOM_SEEDS, tData);
    }


    sSendCounterUpdate(listPlayers, tPlayer.tConsoleCounters.uMonstersKilled, STAT_COUNTER_PLAYER_MONSTERS_KILLED);
    sSendCounterUpdate(listPlayers, tPlayer.tConsoleCounters.uElitesKilled, STAT_COUNTER_PLAYER_ELITES_KILLED);
    sSendCounterUpdate(listPlayers, tPlayer.tConsoleCounters.uGoldPickedUp, STAT_COUNTER_PLAYER_CURRENCY_PICKED_UP);
    sSendCounterUpdate(listPlayers, tPlayer.tConsoleCounters.uLootRunsCompleted, STAT_COUNTER_PLAYER_QUEST_COMPLETED);



    if (tPlayer.uConsoleSyncFlags & (1 << PCSF_AMIIBO_SCAN_TIMES))
    {
        PlayerList listDestPlayers(PlayerListGetElementPool());
        listDestPlayers.AddTail_Tracked(&tPlayer);

        PlayerDWordDataMessage tDefaultMessage;
        tDefaultMessage.dwData = 0;
        tDefaultMessage.nPlayerIndex = Blizzard::Time::ToUnixTime(tPlayer.tNFPDefaultScanTime);
        MessageSendToClient(listDestPlayers, SMSG_SYNC_AMIIBO_SCAN_TIME, &tDefaultMessage, sizeof(tDefaultMessage));

        PlayerDWordDataMessage tGoblinMessage;
        tGoblinMessage.dwData = 1;
        tGoblinMessage.nPlayerIndex = Blizzard::Time::ToUnixTime(tPlayer.tNFPGoblinScanTime);
        MessageSendToClient(listDestPlayers, SMSG_SYNC_AMIIBO_SCAN_TIME, &tGoblinMessage, sizeof(tGoblinMessage));
    }



    tPlayer.uConsoleSyncFlags &= (1 << PCSF_SAVED_QUESTS_ELIGIBLE_IN_GAME_LONG_ENOUGH);
}





void SPlayerRefreshWaypointDestination(
    ACDID idACDPlayer)
{
    Player * ptPlayer = PlayerGetByACD(idACDPlayer);
    if (ptPlayer != NULL)
        ptPlayer->fRefreshWaypointDestination = TRUE;
}








void SPlayerAddDestinationToPrefetch(
    ACDID idACDPlayer,
    ACDID idACDGizmo)
{
    Player * ptPlayer = PlayerGetByACD(idACDPlayer);
    if (ptPlayer != NULL)
        ptPlayer->listDestinationsToPrefetch.AddTail_Tracked(idACDGizmo);
}








static void sPrefetchPortalDestinationCallback(
    SWorldID idSWorld,
    ACDID idACDOperator,
    ACDID idACDPortal,
    uint32 uEntranceGUID,
    PlayerWarpReason eWarpReason)
{

    Player * ptPlayer = PlayerGetByACD(idACDOperator);
    if (ptPlayer == NULL)
        return;



    switch (eWarpReason)
    {
    case WARP_PORTAL:
        SScenePrefetchPortalDestination(idSWorld, ptPlayer, uEntranceGUID);
        break;

    case WARP_WAYPOINT:
        SScenePrefetchWaypointDestination(idSWorld, ptPlayer, uEntranceGUID);
        break;
    }
}


static void sPrefetchPortalDestination(
    SNO snoWorld,
    ACDID idACDOperator,
    ACDID idACDPortal,
    uint32 uEntranceGUID,
    PlayerWarpReason eWarpReason)
{








    if (GameTesting::GetTestType() != 0 || snoWorld == SNO_NONE || snoWorld == -2)
        return;









    SWorldID idSWorld = WorldCommonFindBySNO(snoWorld, TRUE);
    if (idSWorld == SWORLDID_INVALID)
    {
        SWorldCreate(snoWorld, FALSE, FALSE, sPrefetchPortalDestinationCallback, idACDOperator, idACDPortal, uEntranceGUID, eWarpReason, FALSE, 0);
    }
    else
    {


        SWorld * ptSWorld = SWorldTryToGet(idSWorld);
        if (ptSWorld == NULL)
            return;
        if (SWorldIsCreated(*ptSWorld))
        {
            sPrefetchPortalDestinationCallback(idSWorld, idACDOperator, idACDPortal, uEntranceGUID, eWarpReason);
        }
        else
        {

            ptSWorld->AddCreateCallback(sPrefetchPortalDestinationCallback, idACDOperator, ACDID_INVALID, uEntranceGUID, eWarpReason);
        }
    }
}


void SPlayerPrefetchDestinations(
    Player & tPlayer)
{









    POS pos = tPlayer.listDestinationsToPrefetch.GetHeadPosition();

    while (pos)
    {
        ACDID idACD = (ACDID)tPlayer.listDestinationsToPrefetch.GetNext(pos);
        ActorCommonData * ptACD = ACDTryToGet(idACD);
        if (!ptACD)
        {
            continue;
        }

        if (ptACD->ptResolvedDest && ptACD->ptResolvedDest->IsValid())
        {

            PlayerWarpReason eWarpReason = WARP_PORTAL;
            SNO snoChallenge = ActorDefinitionGetTagMapValueSNO(TAG_ACTOR_GIZMO_CHALLENGE_SNO, ptACD->snoActor);
            if (snoChallenge != SNO_NONE)
            {
                eWarpReason = WARP_CHALLENGE;
            }

            if (WorldIsDungeonFinderWorld(ptACD->ptResolvedDest->snoWorld))
            {

                if (!SWorldDungeonFinderIsInitialized())
                {
                    continue;
                }
            }

            sPrefetchPortalDestination(ptACD->ptResolvedDest->snoWorld, tPlayer.idPlayerACD, ptACD->id, ptACD->ptResolvedDest->uEntranceGUID, eWarpReason);
        }
        else if (ptACD->GetGizmoType() == ACTORGIZMO_HEARTH_PORTAL && tPlayer.tHearthPortalInfo.IsValid())
        {
            SScenePrefetchDestination(&tPlayer, tPlayer.tHearthPortalInfo.place);
        }
    }

    tPlayer.listDestinationsToPrefetch.RemoveAll();

    if (tPlayer.fRefreshWaypointDestination && GameGetCurrentAct() != ACT_OPEN_WORLD)
    {
        const CachedWaypointArray & tWaypoints = ActGetCachedWaypointArray();
        uint32 nCount = tWaypoints.size();


        BOOL fDestinationFound = FALSE;
        QuestInstance * ptQuestInstance = GetLeadingMainLineQuest();
        if (ptQuestInstance)
        {
            ScopedQuestDefinitionLockR pdefQuest(ptQuestInstance->snoQuest);
            int32 nStepIndex = QuestStepUIDToIndex(pdefQuest, ptQuestInstance->nCurrentStepUID);
            if (nStepIndex != -1)
            {
                QuestStep & tStep = pdefQuest->arQuestSteps[nStepIndex];
                for (uint32 i = 0; i < nCount; i++)
                {
                    if (PlayerHasActivatedWaypoint(tPlayer, i))
                    {



                        const WaypointInfo & tInfo = tWaypoints[i].tInfo;
                        for (int32 nLevelArea = 0; nLevelArea < NUM_WAYPOINT_LEVEL_AREAS; nLevelArea++)

                            if (tStep.snoWaypointLevelAreas[nLevelArea] == tInfo.snoLevelArea)
                            {

                                sPrefetchPortalDestination(tInfo.snoWorld, tPlayer.idPlayerACD, ACDID_INVALID, tInfo.snoLevelArea, WARP_WAYPOINT);
                                fDestinationFound = TRUE;
                            }
                    }
                }
            }
        }
        if (!fDestinationFound && !GameIsOpenWorld())
        {
            for (int32 i = nCount - 1; i >= 0; i--)
            {
                const WaypointInfo & tInfo = tWaypoints[i].tInfo;
                if (PlayerHasActivatedWaypoint(tPlayer, i))
                {



                    sPrefetchPortalDestination(tInfo.snoWorld, tPlayer.idPlayerACD, ACDID_INVALID, tInfo.snoLevelArea, WARP_WAYPOINT);
                    break;
                }
            }
        }
    }
    tPlayer.fRefreshWaypointDestination = FALSE;
}





void SPlayerRedeemSeasonGift(
    Player & tPlayer,
    GBID gbidItem)
{
    D3::Items::Generator tGenerator;
    ItemGeneratorToDefaults(tGenerator);
    tGenerator.mutable_gb_handle()->set_gbid(gbidItem);
    tGenerator.set_flags(tGenerator.flags() | 1);

    ACDID idACD = SItemGenerate(OnlineService::ItemId::INVALID, tGenerator, cgplaceNull, 0.0f, 0);
    ActorCommonData * ptACDItem = ACDTryToGet(idACD);
    if (ptACDItem != NULL)
    {
        ItemSetBoundToACD(*ptACDItem, tPlayer.idPlayerACD);
        BoolDataMessage tMessage;
        tMessage.bVal = SACDInventoryPickup(tPlayer.idPlayerACD, idACD, EFFECT_INVALID, TRUE, FALSE, NULL, FALSE, FALSE, FALSE, TRUE) == PICKUP_RESPONSE_OK;
        PlayerList listDestPlayers(PlayerListGetElementPool());
        PlayerListGetSingle(listDestPlayers, tPlayer);
        MessageSendToClient(listDestPlayers, SMSG_REDEEM_SEASON_REWARD_CONSOLE, &tMessage, sizeof(tMessage));
    }
}








void SPlayerAddMailItemToInventory(
    const Player & tPlayerFrom,
    D3::Items::Generator & tGenerator,
    BOOL bEquip)
{
    GBHandle hGB(tGenerator.gb_handle());
    GBID gbidItem = hGB.gbid;

    if (ItemIsCosmeticItem(gbidItem))
    {
        if (GBIDExists(gbidItem, GB_LEGACY_ITEM_CONVERSIONS))
        {
            ScopedLegacyItemConversionLockR pdefLegacyItemConversion(gbidItem);
            if (pdefLegacyItemConversion &&
                pdefLegacyItemConversion->gbidOldItem == gbidItem)
            {
                tGenerator.mutable_gb_handle()->set_gbid(pdefLegacyItemConversion->gbidNewItem);
            }
        }
    }

    ACDID idACD = SItemGenerate(OnlineService::ItemId::INVALID, tGenerator, cgplaceNull, 0.0f, 0);
    ActorCommonData * ptACDItem = ACDTryToGet(idACD);
    if (ptACDItem)
    {

        ACDInventoryItemRestoreSocketsFromGenerator(tPlayerFrom.idPlayerACD, *ptACDItem, tGenerator, NULL, 0x2e00);
    }

    SACDInventoryPickup(tPlayerFrom.idPlayerACD, idACD, EFFECT_INVALID, TRUE, FALSE, NULL, FALSE, FALSE, FALSE, TRUE);

    if (bEquip && ptACDItem)
    {
        ActorCommonData * ptACDPlayer = ACDTryToGet(tPlayerFrom.idPlayerACD);
        InventorySlotType eValidSlots[4];
        int32 nNumSlots = 0;

        ACDInventoryWhereCanThisGo(*ptACDPlayer, ptACDItem->hGB.gbid, nNumSlots, eValidSlots);

        if (nNumSlots > 0)
        {
            InventoryLocation tLocDest;
            tLocDest.eSlot = eValidSlots[0];
            tLocDest.idACDContainer = tPlayerFrom.idPlayerACD;
            tLocDest.pt.Set(0, 0);

            PlayerBrainTryInventoryMoveItem(*ptACDPlayer, *ptACDItem, tLocDest);
        }
    }
}

void SPlayerRetrieveSeasonalStashItemOrChallengeRiftReward(
    const Player & tPlayerFrom,
    D3::Items::Generator & tGenerator,
    InventorySlotType eInvSlot)
{
    ACDID idACD = SItemGenerate(OnlineService::ItemId::INVALID, tGenerator, cgplaceNull, 0.0f, 0);

    ActorCommonData * ptACDItem = ACDTryToGet(idACD);
    if (ptACDItem)
    {
        ACDInventoryItemRestoreSocketsFromGenerator(tPlayerFrom.idPlayerACD, *ptACDItem, tGenerator, NULL, 0x2e00);
    }

    PickupResponse eResponse = SACDInventoryPickup(tPlayerFrom.idPlayerACD, idACD, EFFECT_INVALID, TRUE, eInvSlot == SLOT_PLAYER_SHARED_STASH, NULL, FALSE, eInvSlot == SLOT_PLAYER_SHARED_STASH, FALSE, TRUE);

    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetSingle(listDestPlayers, tPlayerFrom);

    BoolDataMessage tStashRetrieveMessage;
    tStashRetrieveMessage.bVal = eResponse == PICKUP_RESPONSE_OK;
    MessageSendToClient(listDestPlayers, SMSG_STASH_ITEM_RETRIEVED, &tStashRetrieveMessage, sizeof(tStashRetrieveMessage));
}





































































void SPlayerRecalculateUpscalingLevels()
{
    int32 nUpscaleLevel = SPlayerGetUpscalingLevel();
    for (Player * ptPlayer = PlayerGetFirstAll(); ptPlayer != NULL; ptPlayer = PlayerGetNextAll(ptPlayer))
    {
        ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
        if (ptACDPlayer != NULL)
            SPlayerSetUpscalingLevel(*ptACDPlayer, nUpscaleLevel);
    }
}








int32 SPlayerGetUpscalingLevel()
{
    int32 nHighestPlayerLevel = 1;




    for (Player * ptPlayer = PlayerGetFirstAll(); ptPlayer != NULL; ptPlayer = PlayerGetNextAll(ptPlayer))
    {
        if (ptPlayer->tHeroStateData.uPlayerFlags & (1 << PLAYERFLAG_HARDCORE))
        {
            nHighestPlayerLevel = 1;
            break;
        }
        ActorCommonData * ptACD = ACDTryToGet(ptPlayer->idPlayerACD);
        if (ptACD != NULL &&
            ptPlayer->idGameConnection != GAMECONNECTIONID_INVALID)
        {
            if (ptACD->IsDueling())
            {
                nHighestPlayerLevel = 1;
                break;
            }


            int32 nPlayerLevel = ptACD->AttributesGetInt(ATTRIBUTE_LEVEL);
            nHighestPlayerLevel = Math::Max(nPlayerLevel, nHighestPlayerLevel);
        }
    }






    ScopedGlobalDefinitionLockR pdefGlobals;
    const int32 nApprenticeshipLevelRequirement = pdefGlobals->nApprenticeshipLevelRequirement;
    if (nHighestPlayerLevel < nApprenticeshipLevelRequirement)
        nHighestPlayerLevel = 1;
    return nHighestPlayerLevel;
}
void SPlayerAddUpscaleBaseBuff(
    ActorCommonData & tACDPlayer,
    int32 nUpscalingLevel)
{
    SNO snoPower = GlobalSNOGet(GLOBALSNO_POWERS_UPSCALE_BUFF);
    BuffID idBuff = PowerAPI_BuffFindOnACD(snoPower, &tACDPlayer, ACDID_INVALID);
    if (idBuff != BUFFID_INVALID)
    {
        PowerAPI_BuffFlagForDeletion(idBuff, TRUE, TRUE, FALSE, TRUE);
    }

    if (nUpscalingLevel > 1)
    {
        PowerAPIBuffStartData tBuffStartData(POWERID_INVALID);
        tBuffStartData.snoPower = snoPower;
        tBuffStartData.eStackType = BUFF_STACKTYPE_NEVER;
        tBuffStartData.nBuffIndex = 0;
        tBuffStartData.bInfinite = TRUE;
        tBuffStartData.bUndispellable = TRUE;
        tBuffStartData.bPositiveEffect = TRUE;

        int32 nVitality = 0;
        int32 nStrength = 0;
        int32 nDexterity = 0;
        int32 nIntelligence = 0;

        int32 nLevel = tACDPlayer.AttributesGetInt(ATTRIBUTE_LEVEL);
        PlayerClass ePlayerClass = PlayerGetPlayerClass(tACDPlayer.id);

        SNO snoXPTable = GlobalSNOGet(GLOBALSNO_GB_EXPERIENCE);
        ScopedGameBalanceDefinitionLockR ptGameBalDef(snoXPTable);
        const ExperienceTable * tExpTable = &ptGameBalDef->tExperienceTable;

        for (int32 n = nLevel; n < nUpscalingLevel; n++)
        {
            const AttributeAwardDefinition & tAttribAwardDef = tExpTable->ptExperienceLevels[n].tAttributeAward[ePlayerClass];
            nVitality += tAttribAwardDef.nVitality;
            nStrength += tAttribAwardDef.nStrength;
            nDexterity += tAttribAwardDef.nDexterity;
            nIntelligence += tAttribAwardDef.nIntelligence;
        }

        FastAttribSetValueFloat(tBuffStartData.idFastAttrib, ATTRIBUTE_STRENGTH_BONUS, (float)nStrength);
        FastAttribSetValueFloat(tBuffStartData.idFastAttrib, ATTRIBUTE_DEXTERITY_BONUS, (float)nDexterity);
        FastAttribSetValueFloat(tBuffStartData.idFastAttrib, ATTRIBUTE_INTELLIGENCE_BONUS, (float)nIntelligence);
        FastAttribSetValueFloat(tBuffStartData.idFastAttrib, ATTRIBUTE_VITALITY_BONUS, (float)nVitality);

        ScopedGlobalDefinitionLockR pdefGlobals;
        float flApprenticeXPBoost = pdefGlobals->flApprenticeshipXPBonus;
        FastAttribSetValueFloat(tBuffStartData.idFastAttrib, ATTRIBUTE_EXPERIENCE_BONUS_PCT, flApprenticeXPBoost);

        Attrib eAttrib = PowerRuneTypeAndBuffIndexToAttribute(snoPower, -1, tBuffStartData.nBuffIndex);
        if (eAttrib != ATTRIBUTE_NONE)
        {
            FastAttribSetValueInt(tBuffStartData.idFastAttrib, MakeAttribKey(eAttrib, snoPower), TRUE);
        }

        PowerAPI_BuffAddToACD(tBuffStartData, tACDPlayer);
    }

    SPlayerUpdateSidekickStatsBoost(tACDPlayer);
}








void SPlayerUpdateSidekickStatsBoost(
    ActorCommonData & tACDPlayer)
{
    // (the original declares `AppGlobals & tAppGlobals` on line 13765; it has no surviving code)

    SNO snoPower = GlobalSNOGet(GLOBALSNO_POWERS_SIDEKICK_STATS_BOOST_BUFF);
    BuffID idBuff = PowerAPI_BuffFindOnACD(snoPower, &tACDPlayer, ACDID_INVALID);
    if (idBuff != BUFFID_INVALID)
    {
        PowerAPI_BuffFlagForDeletion(idBuff, TRUE, TRUE, FALSE, TRUE);
    }

    int32 nUpscalingLevel = tACDPlayer.AttributesGetInt(ATTRIBUTE_UPSCALED_LEVEL);

    if (nUpscalingLevel > 1)
    {





        PowerAPIBuffStartData tBuffStartData(POWERID_INVALID);
        tBuffStartData.snoPower = snoPower;
        tBuffStartData.eStackType = BUFF_STACKTYPE_NEVER;
        tBuffStartData.bInfinite = TRUE;
        tBuffStartData.bUndispellable = TRUE;
        tBuffStartData.bPositiveEffect = TRUE;

        BOOL bNeedToApplyBuff = FALSE;

        SNO snoXPTable = GlobalSNOGet(GLOBALSNO_GB_EXPERIENCE);
        ScopedGameBalanceDefinitionLockR ptGameBalDef(snoXPTable);
        const ExperienceTable & tExpTable = ptGameBalDef->tExperienceTable;

        ScopedGlobalDefinitionLockR pdefGlobals;
        const float flBuffMinScalar = pdefGlobals->flMinimumStatScale;

        {
            float flSidekickVitalityIdeal = (float)tExpTable.ptExperienceLevels[nUpscalingLevel - 1].nSidekickVitalityIdeal;
            float flPlayerVitalityCurrent = tACDPlayer.AttributesGetFloat(ATTRIBUTE_VITALITY_TOTAL);






            if (flPlayerVitalityCurrent < flSidekickVitalityIdeal)
            {


                float fAmountToAdd = flBuffMinScalar * (flSidekickVitalityIdeal - flPlayerVitalityCurrent);
                FastAttribSetValueFloat(tBuffStartData.idFastAttrib, ATTRIBUTE_VITALITY_BONUS, fAmountToAdd);
                bNeedToApplyBuff = TRUE;
            }
        }












        {
            float flSidekickTotalArmorIdeal = (float)tExpTable.ptExperienceLevels[nUpscalingLevel - 1].nSidekickTotalArmorIdeal;
            float flPlayerTotalArmorCurrent = tACDPlayer.AttributesGetFloat(ATTRIBUTE_ARMOR_TOTAL);






            if (flPlayerTotalArmorCurrent < flSidekickTotalArmorIdeal)
            {


                float fAmountToAdd = flBuffMinScalar * (flSidekickTotalArmorIdeal - flPlayerTotalArmorCurrent);
                FastAttribSetValueFloat(tBuffStartData.idFastAttrib, ATTRIBUTE_ARMOR, fAmountToAdd);
                bNeedToApplyBuff = TRUE;
            }
        }












        {
            float flSidekickLifeOnHitIdeal = (float)tExpTable.ptExperienceLevels[nUpscalingLevel - 1].nSidekickLifeOnHitIdeal;
            float flPlayerLifeOnHitCurrent = tACDPlayer.AttributesGetFloat(ATTRIBUTE_HITPOINTS_ON_HIT);






            if (flPlayerLifeOnHitCurrent < flSidekickLifeOnHitIdeal)
            {


                float fAmountToAdd = flBuffMinScalar * (flSidekickLifeOnHitIdeal - flPlayerLifeOnHitCurrent);
                FastAttribSetValueFloat(tBuffStartData.idFastAttrib, ATTRIBUTE_HITPOINTS_ON_HIT, fAmountToAdd);
                bNeedToApplyBuff = TRUE;
            }
        }












        {
            float flSidekickTotalResistIdeal = (float)tExpTable.ptExperienceLevels[nUpscalingLevel - 1].nSidekickTotalResistIdeal;

            for (int32 nDamageType = 0; nDamageType < 7; nDamageType++)
            {
                float flSidekickTotalResistCurrent = tACDPlayer.AttributesGetFloat(MakeAttribKey(ATTRIBUTE_RESISTANCE_TOTAL, nDamageType));






                if (flSidekickTotalResistCurrent < flSidekickTotalResistIdeal)
                {


                    float fAmountToAdd = flBuffMinScalar * (flSidekickTotalResistIdeal - flSidekickTotalResistCurrent);
                    FastAttribSetValueFloat(tBuffStartData.idFastAttrib, MakeAttribKey(ATTRIBUTE_RESISTANCE, nDamageType), fAmountToAdd);
                    bNeedToApplyBuff = TRUE;
                }
            }
        }












        {
            Attrib ePrimaryAttribute = ATTRIBUTE_NONE;
            Attrib ePrimaryAttributeBonus = ATTRIBUTE_NONE;

            PrimaryDamageAttribute ePrimaryDamageAttribute = tACDPlayer.AttributesGetInt(ATTRIBUTE_PRIMARY_DAMAGE_ATTRIBUTE);
            switch (ePrimaryDamageAttribute)
            {
            case PRIMARY_DAMAGE_ATTRIBUTE_STRENGTH:
                ePrimaryAttribute = ATTRIBUTE_STRENGTH_TOTAL;
                ePrimaryAttributeBonus = ATTRIBUTE_STRENGTH_BONUS;
                break;
            case PRIMARY_DAMAGE_ATTRIBUTE_DEXTERITY:
                ePrimaryAttribute = ATTRIBUTE_DEXTERITY_TOTAL;
                ePrimaryAttributeBonus = ATTRIBUTE_DEXTERITY_BONUS;
                break;
            case PRIMARY_DAMAGE_ATTRIBUTE_INTELLIGENCE:
                ePrimaryAttribute = ATTRIBUTE_INTELLIGENCE_TOTAL;
                ePrimaryAttributeBonus = ATTRIBUTE_INTELLIGENCE_BONUS;
                break;
            }

            if (ePrimaryAttribute != ATTRIBUTE_NONE)
            {














                float flSidekickPrimaryStatIdeal = (float)tExpTable.ptExperienceLevels[nUpscalingLevel - 1].nSidekickPrimaryStatIdeal;

                float flPlayerPrimaryStatCurrent = tACDPlayer.AttributesGetFloat(ePrimaryAttribute);






                if (flPlayerPrimaryStatCurrent < flSidekickPrimaryStatIdeal)
                {


                    float fAmountToAdd = flBuffMinScalar * (flSidekickPrimaryStatIdeal - flPlayerPrimaryStatCurrent);
                    FastAttribSetValueFloat(tBuffStartData.idFastAttrib, ePrimaryAttributeBonus, fAmountToAdd);
                    bNeedToApplyBuff = TRUE;
                }
            }
        }












        if (bNeedToApplyBuff)
        {
            PowerAPI_BuffAddToACD(tBuffStartData, tACDPlayer);
        }
    }




    SNO snoPowerLoc = GlobalSNOGet(GLOBALSNO_POWERS_SIDEKICK_WEAPON_DAMAGE_BOOST_BUFF);
    BuffID idBuffLoc = PowerAPI_BuffFindOnACD(snoPowerLoc, &tACDPlayer, ACDID_INVALID);

    if (idBuffLoc != BUFFID_INVALID)
    {
        PowerAPI_BuffFlagForDeletion(idBuffLoc, TRUE, TRUE, FALSE, TRUE);
    }

    if (nUpscalingLevel > 1)
    {



        float flMainHandDamage = 0.0f;
        float flMainHandSpeed = 0.0f;
        float flOffHandDamage = 0.0f;
        float flOffHandSpeed = 0.0f;

        BOOL bIsDualWielding = ACDIsDualWielding(tACDPlayer, TRUE);
        tACDPlayer.GetWeaponDamageAndAttackSpeed(bIsDualWielding, flMainHandDamage, flMainHandSpeed, flOffHandDamage, flOffHandSpeed);

        if ((flMainHandDamage != 0.0f || flOffHandDamage != 0.0f) &&
            (flMainHandSpeed != 0.0f || flOffHandSpeed != 0.0f))
        {



            ScopedGlobalDefinitionLockR pdefGlobals;
            const float flBuffMinScalar = pdefGlobals->flMinimumStatScale;

            SNO snoXPTable = GlobalSNOGet(GLOBALSNO_GB_EXPERIENCE);
            ScopedGameBalanceDefinitionLockR ptGameBalDef(snoXPTable);
            const ExperienceTable & tExpTable = ptGameBalDef->tExperienceTable;

            float flDPSCurrent = (flMainHandDamage + flOffHandDamage) * (flMainHandSpeed + flOffHandSpeed);
            float flDPSIdeal = (float)tExpTable.ptExperienceLevels[nUpscalingLevel - 1].nSidekickTargetDPSIdeal;






            if (flDPSCurrent < flDPSIdeal)
            {
                PowerAPIBuffStartData tBuffStartData(POWERID_INVALID);
                tBuffStartData.snoPower = snoPowerLoc;
                tBuffStartData.eStackType = BUFF_STACKTYPE_NEVER;
                tBuffStartData.bInfinite = TRUE;
                tBuffStartData.bUndispellable = TRUE;
                tBuffStartData.bPositiveEffect = TRUE;

                float flDamageCurrent = flDPSCurrent / (flMainHandSpeed + flOffHandSpeed);
                float flDamageIdeal = flDPSIdeal / (flMainHandSpeed + flOffHandSpeed);
                float flDamageExtra = flBuffMinScalar * (flDamageIdeal - flDamageCurrent);

                float flScalar = (flDamageCurrent + flDamageExtra) / flDamageCurrent;








                FastAttribSetValueFloat(tBuffStartData.idFastAttrib, ATTRIBUTE_DAMAGE_SIDEKICK_MULTIPLIER, flScalar);

                PowerAPI_BuffAddToACD(tBuffStartData, tACDPlayer);
            }
        }
    }














}


void SPlayerSetUpscalingLevel(
    ActorCommonData & tACDPlayer,
    int32 nUpscalingLevel)
{
    if (CmdLineGetParams()->tBitArray.Test(CMDLINE_BLIZZCON_BIT))
        return;

    int32 nCurrentUpscalingLevel = tACDPlayer.AttributesGetInt(ATTRIBUTE_UPSCALED_LEVEL);
    int32 nCurrentLevel = tACDPlayer.AttributesGetInt(ATTRIBUTE_LEVEL);
    ScopedGlobalDefinitionLockR pdefGlobals;
    int32 nLevelGap = pdefGlobals->nApprenticeshipLevelGap;
    BOOL bUpscalingStateChanged = FALSE;

    if (nCurrentUpscalingLevel <= 1)
    {
        if (nUpscalingLevel <= nCurrentLevel + nLevelGap)
            return;

        if (!CmdLineGetParams()->tBitArray.Test(CMDLINE_BLIZZCON_BIT))
        {
            SNO snoEffect = GlobalSNOGet(GLOBALSNO_EFFECT_GROUP_UPSCALE_BUFF_ACTIVE);
            if (snoEffect != SNO_NONE && GameTesting::GetTestType() == 0)
                ACDPlayEffect(tACDPlayer.id, EFFECT_PLAY_EFFECT_GROUP, snoEffect, ACDID_INVALID);
        }
        bUpscalingStateChanged = TRUE;
    }
    else
    {
        BOOL bChangeUpscaling;
        if (nCurrentLevel + nLevelGap > nCurrentUpscalingLevel)
            bChangeUpscaling = !CmdLineGetParams()->tBitArray.Test(CMDLINE_BLIZZCON_BIT) || nUpscalingLevel < nCurrentLevel + nLevelGap;
        else
            bChangeUpscaling = nUpscalingLevel < nCurrentLevel + nLevelGap;

        if (bChangeUpscaling)
        {
            SNO snoEffect = GlobalSNOGet(GLOBALSNO_EFFECT_GROUP_UPSCALE_BUFF_REMOVED);
            if (snoEffect != SNO_NONE && GameTesting::GetTestType() == 0)
                ACDPlayEffect(tACDPlayer.id, EFFECT_PLAY_EFFECT_GROUP, snoEffect, ACDID_INVALID);
            bUpscalingStateChanged = TRUE;
        }
    }

    BOOL bResetHealth;
    nUpscalingLevel--;
    if (tACDPlayer.AttributesGetInt(ATTRIBUTE_LEVEL) > nUpscalingLevel - nLevelGap)
    {
        tACDPlayer.AttributesSetInt(ATTRIBUTE_UPSCALED_LEVEL, 0);
        bResetHealth = FALSE;
        nUpscalingLevel = 1;
    }
    else
    {
        bResetHealth = tACDPlayer.AttributesGetInt(ATTRIBUTE_UPSCALED_LEVEL) < 2;
        tACDPlayer.AttributesSetInt(ATTRIBUTE_UPSCALED_LEVEL, nUpscalingLevel);
    }

    if (tACDPlayer.ptInventory)
    {
        ACDInventoryUpdateOnUpscalingChanged(tACDPlayer);
    }

    SPlayerAddUpscaleBaseBuff(tACDPlayer, nUpscalingLevel);
    ExperienceUpdatePerLevelHitpointFactorVitality(tACDPlayer);

    if (bResetHealth && tACDPlayer.IsAlive())
    {
        tACDPlayer.ResetLifeStats(TRUE, TRUE, TRUE, TRUE, FALSE);
    }

    if (bUpscalingStateChanged)
    {
        Player * ptPlayer = PlayerGetByACD(tACDPlayer.id);
        if (ptPlayer)
        {
            PlayerList listDestPlayers(PlayerListGetElementPool());
            PlayerListGetAllConnected(listDestPlayers, -1, FALSE);
            PlayerDWordDataMessage tUpscaleMessage;
            tUpscaleMessage.dwData = tACDPlayer.AttributesGetInt(ATTRIBUTE_UPSCALED_LEVEL);
            tUpscaleMessage.nPlayerIndex = ptPlayer->nPlayerIndex;
            MessageSendToClient(listDestPlayers, SMSG_UPSCALING_LEVEL_CHANGED, &tUpscaleMessage, sizeof(tUpscaleMessage));
        }
    }
}





















































BOOL SPlayerIsInRift(
    const ActorCommonData & tACDPlayer)
{
    if (!ACDIsPlayer(tACDPlayer.id))
        return FALSE;
    LevelArea * ptLevelArea = LevelAreaTryToGet(ACDGetLevelAreaID(tACDPlayer));
    if (ptLevelArea != NULL && LevelAreaGetDungeonFinderLevel(ptLevelArea->snoLevelArea) > 0)
        return TRUE;
    return FALSE;
}










BOOL SPlayerIsInSetDungeon(
    const ActorCommonData & tACDPlayer)
{
    if (!ACDIsPlayer(tACDPlayer.id))
        return FALSE;
    return SWorldIsSetDungeon(tACDPlayer.GetPlace().idSWorld);
}




void SPlayerHealAchievementPointLeaderboard(
    Player & tPlayer)
{
}















































































void SPlayerKillActiveCosmeticPet(
    Player & tPlayer)
{
    ACDID idPlayerACD = tPlayer.idPlayerACD;
    ActorCommonData * ptACDPlayer = ACDTryToGet(idPlayerACD);
    if (ptACDPlayer == NULL)
        return;

    VisualInventorySetCosmeticItemInSlot(idPlayerACD, VCIS_PET, GBID_NONE);
    GBID gbidPet = ptACDPlayer->AttributesGetInt(ATTRIBUTE_CURRENT_COSMETIC_PET);
    if (gbidPet == GBID_NONE)
        return;

    ScopedItemDefinitionLockR pdefItem(gbidPet);
    if (pdefItem == NULL)
        return;
    for (int32 i = 0; i < (int32)(pdefItem->serAttributes_Console.dwSize / sizeof(AttributeSpecifier)); i++)
    {
        const AttributeSpecifier & tSpecifier = pdefItem->ptAttributes[i];
        if (tSpecifier.eAttribute == ATTRIBUTE_COSMETIC_PET_POWER)
        {
            SNO snoPower = tSpecifier.nParam;
            if (snoPower == SNO_NONE)
                break;
            BuffID idBuff = PowerAPI_BuffFindOnACD(snoPower, ptACDPlayer, ACDID_INVALID);
            if (idBuff != BUFFID_INVALID)
            {
                PowerAPI_BuffFlagForDeletion(idBuff, FALSE, TRUE, FALSE, TRUE);
                break;
            }
        }
    }
}












void SPlayerRemoveActivePet(
    Player & tPlayer)
{
    ActorCommonData * ptACDPlayer = ACDTryToGet(tPlayer.idPlayerACD);
    if (ptACDPlayer != NULL)
    {
        SPlayerKillActiveCosmeticPet(tPlayer);
        ptACDPlayer->AttributesSetInt(ATTRIBUTE_CURRENT_COSMETIC_PET, -1);
        ptACDPlayer->AttributesSetInt(ATTRIBUTE_COSMETIC_PET_EXPIRATION, 0);
    }
}







void SPlayerActivatePet(
    Player & tPlayer,
    GBID gbidPet)
{
    ActorCommonData * ptACDPlayer = ACDTryToGet(tPlayer.idPlayerACD);
    if (!ptACDPlayer)
        return;

    if (ptACDPlayer->IsDueling())
        return;

    if (ptACDPlayer->AttributesGetInt(ATTRIBUTE_CURRENT_COSMETIC_PET) != GBID_NONE)
    {
        SPlayerRemoveActivePet(tPlayer);
    }

    if (!GBIDExists(gbidPet, GB_ITEMS))
        return;

    ScopedItemDefinitionLockR pdefPetItem(gbidPet);
    if (!pdefPetItem)
        return;

    {
        SNO snoPetActor = SNO_NONE;
        SNO snoPetPower = SNO_NONE;
        int32 nPetExpiration = 0;

        for (int32 i = 0; i < (int32)(pdefPetItem->serAttributes_Console.dwSize / sizeof(AttributeSpecifier)); i++)
        {
            const AttributeSpecifier & tSpecifier = pdefPetItem->ptAttributes[i];

            if (tSpecifier.eAttribute == ATTRIBUTE_COSMETIC_PET_POWER)
            {
                snoPetPower = tSpecifier.nParam;
                if (snoPetPower == SNO_NONE)
                    return;

                if (!SNOFileExists(POWER_SNO, snoPetPower))
                    return;
            }
            else if (tSpecifier.eAttribute == ATTRIBUTE_COSMETIC_PET_EXPIRATION)
            {
                nPetExpiration = tSpecifier.nParam;
            }
            else if (tSpecifier.eAttribute == ATTRIBUTE_COSMETIC_PET_ACTOR)
            {
                snoPetActor = tSpecifier.nParam;
                if (snoPetActor == SNO_NONE)
                    return;

                if (!SNOFileExists(ACTOR_SNO, snoPetActor))
                    return;
            }
        }

        if (nPetExpiration > 0 && tPlayer.tCosmeticItemSavedData.tCosmeticItemData.GetPetExpiration(gbidPet) == 0)
        {
            snoPetPower = SNO_NONE;
        }

        if (snoPetActor != SNO_NONE && snoPetPower != SNO_NONE)
        {
            ptACDPlayer->AttributesSetInt(ATTRIBUTE_CURRENT_COSMETIC_PET, gbidPet);
            ptACDPlayer->AttributesSetInt(ATTRIBUTE_COSMETIC_PET_EXPIRATION, nPetExpiration);
            ptACDPlayer->AttributesSetInt(ATTRIBUTE_COSMETIC_PET_ACTOR, snoPetActor);

            PowerStartData tPowerStartData;
            PowerStartDataInit(tPowerStartData);
            tPowerStartData.idACD = tPlayer.idPlayerACD;
            tPowerStartData.idACDAlternateSource = tPowerStartData.idACD;
            tPowerStartData.snoPower = snoPetPower;
            tPowerStartData.ptExtraPowerStartData = NULL;
            tPowerStartData.tTarget.Clear();
            PowerID idPower = PowerStartEx(tPowerStartData, ATTRIBGROUPID_INVALID, FALSE, FALSE);

            if (idPower == POWERID_INVALID)
            {
                tPlayer.bCosmeticPetMissing = TRUE;
            }

            VisualInventorySetCosmeticItemInSlot(ptACDPlayer->id, VCIS_PET, gbidPet);
        }
    }
}
















void SPlayerSetCosmeticPortrait(
    Player & tPlayer,
    GBID gbidPortrait)
{
    VisualInventorySetCosmeticItemInSlot(tPlayer.idPlayerACD, VCIS_PORTRAIT, gbidPortrait);
}



void DeliveredRewardsSavedData::Export(
    D3::Account::DeliveredRewards & tDeliveredRewardsSavedData)
{
    for (size_t i = 0; i < Math::Min(tAchievementRewards.size(), (size_t)MAX_RECENT_ACHIEVEMENTS); i++)
    {
        D3::Account::DeliveredAchievementReward * ptReward = tDeliveredRewardsSavedData.add_achievement_reward(); ptReward->set_achievement_id(tAchievementRewards[i].achievement_id); ptReward->set_set_id(tAchievementRewards[i].set_id);
    }

    for (size_t i = 0; i < tLegacyAchievementsToDeliver.size(); i++)
    {
        tDeliveredRewardsSavedData.add_legacy_achievements_to_deliver(tLegacyAchievementsToDeliver[i]);
    }
}


void DeliveredRewardsSavedData::Import(
    const D3::Account::DeliveredRewards & tDeliveredRewardsSavedData)
{
    tAchievementRewards.reserve(tDeliveredRewardsSavedData.achievement_reward_size());

    for (int i = 0; i < tDeliveredRewardsSavedData.achievement_reward_size(); i++)
    {
        const D3::Account::DeliveredAchievementReward & tSaved = tDeliveredRewardsSavedData.achievement_reward(i); DeliveredAchievementRewardEntry tReward = { tSaved.achievement_id(), tSaved.set_id() }; tAchievementRewards.push_back(tReward);
    }

    for (int i = 0; i < tDeliveredRewardsSavedData.legacy_achievements_to_deliver_size(); i++)
    {
        tLegacyAchievementsToDeliver.push_back(tDeliveredRewardsSavedData.legacy_achievements_to_deliver(i));
    }
}
















void DeliveredRewardsSavedData::AddAchievementReward(uint64 achievement, int32 set_id, Player * ptPlayer)
{
    // trim the list down to make room for the new one
    while (tAchievementRewards.size() >= MAX_RECENT_ACHIEVEMENTS)
    {
        OnlineService::AchievementRecord record;
        DeliveredAchievementRewardEntry * earliestAchievementPos = tAchievementRewards.begin();
        Blizzard::Time::Timestamp earliestAchievementTimestamp = INT64_MAX;

        for (DeliveredAchievementRewardEntry * it = tAchievementRewards.begin(); it != tAchievementRewards.end(); ++it)
        {
            if (SGameManager::GameGet().ptSGameGlobals->ptAchievementsInterface->FindAwardedAchievement(ptPlayer->tAccount.tHandle, it->set_id, it->achievement_id, record))
            {
                if (record.m_timestampEarned < earliestAchievementTimestamp)
                {
                    earliestAchievementPos = it;
                    earliestAchievementTimestamp = record.m_timestampEarned;
                }
            }
        }

        if (SGameManager::GameGet().ptSGameGlobals->ptAchievementsInterface->FindAwardedAchievement(ptPlayer->tAccount.tHandle, set_id, achievement, record) && record.m_timestampEarned < earliestAchievementTimestamp)
            return;

        tAchievementRewards.erase(earliestAchievementPos);
    }

    DeliveredAchievementRewardEntry tReward = { achievement, set_id }; tAchievementRewards.push_back(tReward);
}

















inline bool DeliveredRewardsSavedData::ContainsAchievement(
    uint64 achievement, int32 set_id)
{
    for (size_t i = 0; i < tAchievementRewards.size(); i++)
    {
        if (tAchievementRewards[i].achievement_id == achievement && tAchievementRewards[i].set_id == set_id)
            return true;
    }
    return false;
}
































































// No out-of-line copy in the image (the DWARF records it only inlined into SPlayerCreate); body from that inlined copy.
void DeliveredRewardsSavedData::Clear()
{
    tAchievementRewards.clear();
    tLegacyAchievementsToDeliver.clear();
}




void SPlayerImportDeliveredRewards(
    Player & player,
    const D3::Account::DeliveredRewards & rewards)
{
    SGameManager::GameGet().ptServerPlayerGlobals->tPerPlayer[player.nPlayerIndex].tDeliveredRewards.Import(rewards);
    if (rewards.has_entitled_reward_license_bits())
        player.uEntitledRewardBits = rewards.entitled_reward_license_bits();
}















void SPlayerExportDeliveredRewards(
    const Player & player,
    D3::Account::DeliveredRewards * rewards)
{
    SGameManager::GameGet().ptServerPlayerGlobals->tPerPlayer[player.nPlayerIndex].tDeliveredRewards.Export(*rewards);
}









void SPlayerMarkConquestDelivered(
    Player & player,
    OnlineService::AchievementId achievementId)
{
    SGame & game = SGameManager::GameGet();
    DeliveredRewardsSavedData & tDeliveredRewards = game.ptServerPlayerGlobals->tPerPlayer[player.nPlayerIndex].tDeliveredRewards;
    if (!tDeliveredRewards.ContainsAchievement(achievementId, sub_71007b7080()))
        game.ptServerPlayerGlobals->tPerPlayer[player.nPlayerIndex].tDeliveredRewards.AddAchievementReward(achievementId, sub_71007b7080(), &player);
}










uint64 SPlayerGetGearAndSkillsHash(
    Player & tPlayer)
{
    const ActorCommonData * ptACD = ACDTryToGet(tPlayer.idPlayerACD);
    if (!ptACD)
        return 0;

    uint64 uHash = 0;

    for (int32 i = SLOT_PLAYER_BODYSLOT_START; i < SLOT_PLAYER_BODYSLOT_END; i++)
    {
        ACDID idACDItem = ACDInventoryGetSquareItem(*ptACD, i, IVector2D(0, 0));
        ActorCommonData * ptACDItem = ACDTryToGet(idACDItem);
        if (ptACDItem)
        {

            uint64 uItemHash = ItemGenerateHash64(*ptACDItem, TRUE);
            uHash = Blizzard::Util::Hash64More(uItemHash, uHash);
        }
    }

    for (int32 i = 0; i < NUM_SKILL_SLOTS; i++)
    {
        SNO snoSkill = SPlayerGetActiveSkillInSlot(tPlayer, i);
        uHash = Blizzard::Util::Hash64More(snoSkill, uHash);

        RuneType eRuneType = PowerGetRuneTypeForACD(*ptACD, snoSkill);
        uHash = Blizzard::Util::Hash64More(eRuneType, uHash);
    }

    for (int32 i = 0; i < NUM_TRAIT_SLOTS; i++)
    {
        SNO snoTrait = PlayerGetPowerInTraitSlot(tPlayer, i);
        uHash = Blizzard::Util::Hash64More(snoTrait, uHash);
    }

    return uHash;
}




void sPlayerGiveRewardCosmetic(Player & player, OnlineService::RewardId rewardId, OnlineService::AchievementId achievementId)
{
    CosmeticItemType eCosmeticType = CosmeticItems_GetCosmeticItemTypeFromGBID(rewardId);
    switch (eCosmeticType)
    {
    case COSMETIC_ITEM_PET:
        SCosmeticItems_LearnPet(player.idPlayerACD, rewardId, false);
        break;

    case COSMETIC_ITEM_WINGS:
    case COSMETIC_ITEM_PENNANT:
    case COSMETIC_ITEM_FRAME:

        SCosmeticItems_LearnCosmeticItem(player.idPlayerACD, rewardId, eCosmeticType, false);
        break;
    }

}


















static void sPlayerLoadoutEquipGems(
    ActorCommonData & tACDItem,
    XPooledDataIDList & listACDIDSocketables,
    const LoadoutItemData & tLoadoutItem)
{
    if (!ItemHasSockets(tACDItem))
        return;



    int32 nSocketsTotal = tACDItem.AttributesGetInt(ATTRIBUTE_SOCKETS);
    for (int32 nSocket = 0; nSocket < tLoadoutItem.nSocketsCount && nSocket < nSocketsTotal; nSocket++)
    {
        GBID gbidDesiredSocketable = tLoadoutItem.gbidSockets[nSocket];

        POS posSocketables = listACDIDSocketables.GetHeadPosition();
        while (posSocketables)
        {
            ACDID & idACDSocketable = (ACDID &)listACDIDSocketables.GetNext(posSocketables);

            ActorCommonData * ptACDSocketable = ACDTryToGet(idACDSocketable);
            if (!ptACDSocketable)
                continue;




            if (PlayerLoadoutSocketablesMatch(ptACDSocketable->hGB.gbid, gbidDesiredSocketable))
            {
                uint64 nOldStackSize = ptACDSocketable->GetStackCount();

                IVector2D tPoint(0, 0);
                if (ACDInventoryCanHold(tACDItem, *ptACDSocketable, SLOT_ITEM_SOCKETS, tPoint, NULL))
                {
                    ActorCommonData * ptACDOldContainer = NULL;


                    const InventoryLocation * ptOldInvLocation = ptACDSocketable->TryToGetInventoryLocation();
                    if (ptOldInvLocation && ptOldInvLocation->eSlot == SLOT_ITEM_SOCKETS)
                    {

                        ptACDOldContainer = ACDTryToGet(ptOldInvLocation->idACDContainer);
                        if (ptACDOldContainer)
                        {
                            ItemRemoveSocketedItemAttributes(*ptACDSocketable, *ptACDOldContainer);
                            SACDInventoryAddToDirtyList(*ptACDOldContainer, AIDA_UPDATE_GEM_REMOVED, ptACDOldContainer->GetLocationSlot(), ptACDOldContainer->GetLocationSlot());
                        }
                    }


                    if (ActorInventorySocketItem(*ptACDSocketable, tACDItem, tPoint, TRUE))
                    {
                        if (nOldStackSize <= 1)
                        {
                            idACDSocketable = ACDID_INVALID;
                        }
                    }
                    else if (ptACDOldContainer)
                    {

                        ItemAddSocketedItemAttributes(*ptACDSocketable, *ptACDOldContainer);
                    }
                }
                break;
            }
        }
    }
}





























































static ActorCommonData * sPlayerLoadoutCheckForOffHandRemoval(
    ActorCommonData * ptACDPlayer,
    const LoadoutSavedData & tLoadout,
    XPooledDataIDList & listACDIDRemovedItems)
{
    ActorCommonData * ptACDLoadoutMainHandItem = ACDTryToGet(ACDFromANN(tLoadout.tEquipment[SLOT_PLAYER_LEFT_HAND - SLOT_PLAYER_BODYSLOT_START].annItem));
    if (!ptACDLoadoutMainHandItem || !PlayerLoadoutItemIsInValidInventorySlot(*ptACDLoadoutMainHandItem, ptACDPlayer->id))
        return NULL;



    ACDID idACDOffHandItem = ACDInventoryGetSquareItem(*ptACDPlayer, SLOT_PLAYER_RIGHT_HAND, cgptNull);
    ActorCommonData * ptACDOffHandItem = ACDTryToGet(idACDOffHandItem);
    if (!ptACDOffHandItem)
        return NULL;



    EInvItemClass eOffHandItemClass = GetInventoryItemClass(ptACDOffHandItem->hGB.gbid);
    if (eOffHandItemClass == IIC_QUIVER)
    {
        if (!ItemUsesQuiver(*ptACDLoadoutMainHandItem))
        {
            SACDInventoryAddToDirtyList(*ptACDOffHandItem, AIDA_REMOVE, ptACDOffHandItem->GetLocationSlot(), -1);
            ACDInventoryRemoveToLimbo(ptACDPlayer->id, *ptACDOffHandItem, FALSE);
            listACDIDRemovedItems.AddTail_Tracked(ptACDOffHandItem->id);
            return ptACDOffHandItem;
        }
    }
    else
    {




        EInvItemClass eLoadoutMainHandItemClass = GetInventoryItemClass(ptACDLoadoutMainHandItem->hGB.gbid);
        if (eLoadoutMainHandItemClass == IIC_TWOHANDED &&
            !(eOffHandItemClass == IIC_SHIELD && ptACDPlayer->AttributesGetInt(ATTRIBUTE_ALLOW_2H_AND_SHIELD)))
        {
            SACDInventoryAddToDirtyList(*ptACDOffHandItem, AIDA_REMOVE, ptACDOffHandItem->GetLocationSlot(), -1);
            ACDInventoryRemoveToLimbo(ptACDPlayer->id, *ptACDOffHandItem, FALSE);
            listACDIDRemovedItems.AddTail_Tracked(ptACDOffHandItem->id);
            return ptACDOffHandItem;
        }
    }
    return NULL;
}



static BOOL sPlayerLoadoutEquipItemSlot(
    ActorCommonData * ptACDPlayer,
    const LoadoutSavedData & tLoadout,
    InventorySlotType eInvSlot,
    XPooledDataIDList & listACDIDSocketables,
    XPooledDataIDList & listACDIDRemovedItems,
    BOOL bDropToStash,
    InventoryLocation * ptEquippedItemInvLocOld)
{
    ACDID idACDCurrentItem = ACDInventoryGetSquareItem(*ptACDPlayer, eInvSlot, cgptNull);
    ActorCommonData * ptACDCurrentItem = ACDTryToGet(idACDCurrentItem);

    if (ptACDCurrentItem)
    {
        ptACDCurrentItem->AttributesSetInt(ATTRIBUTE_ITEM_WAS_TOUCHED_BY_ARMORY, 1);
        ptACDCurrentItem->AttributesSetInt(ATTRIBUTE_ITEM_WAS_TOUCHED_BY_ARMORY_LAST_GAME, 1);
    }


    ActorCommonData * ptACDItem = ACDTryToGet(ACDFromANN(tLoadout.tEquipment[eInvSlot - SLOT_PLAYER_BODYSLOT_START].annItem));
    if (ptACDItem)
    {
        ptACDItem->AttributesSetInt(ATTRIBUTE_ITEM_WAS_TOUCHED_BY_ARMORY, 1);
        ptACDItem->AttributesSetInt(ATTRIBUTE_ITEM_WAS_TOUCHED_BY_ARMORY_LAST_GAME, 1);
    }


    if (!ptACDItem || !PlayerLoadoutItemIsInValidInventorySlot(*ptACDItem, ptACDPlayer->id))
    {
        if (ptACDCurrentItem)
        {

            ACDID idACDPickedUpItem = ptACDCurrentItem->id;
            LPCSTR szPickedUpItem = GBIDToString(ptACDCurrentItem->hGB, FALSE).str();
            uint64 dwStackCount = ptACDCurrentItem->GetStackCount();

            PowerAPI_BuffApplyOnEquipmentChangeEffects(ptACDPlayer->id, eInvSlot, 1);

            SACDInventoryAddToDirtyList(*ptACDCurrentItem, AIDA_REMOVE, ptACDCurrentItem->GetLocationSlot(), -1);
            ACDInventoryRemoveToLimbo(ptACDPlayer->id, *ptACDCurrentItem, FALSE);

            BOOL fSuccess = SACDInventoryPickupOrSpillOntoGround(*ptACDPlayer, *ptACDCurrentItem, bDropToStash, NULL, FALSE);












            listACDIDRemovedItems.AddTail_Tracked(ptACDCurrentItem->id);
        }
        return FALSE;
    }



    if (ptACDCurrentItem)
    {
        PlayerLoadoutUnsocketItemToLimbo(*ptACDCurrentItem, &listACDIDSocketables);
        listACDIDRemovedItems.AddTail_Tracked(ptACDCurrentItem->id);
    }


    {
        POS pos = listACDIDRemovedItems.GetHeadPosition();
        while (pos)
        {
            ACDID idACDItem = (ACDID)listACDIDRemovedItems.GetNext(pos);
            ActorCommonData * ptACDRemovedItem = ACDTryToGet(idACDItem);
            if (ptACDRemovedItem)
            {
                ptACDRemovedItem->AttributesSetInt(ATTRIBUTE_ITEM_WAS_TOUCHED_BY_ARMORY, 1);
                ptACDRemovedItem->AttributesSetInt(ATTRIBUTE_ITEM_WAS_TOUCHED_BY_ARMORY_LAST_GAME, 1);
            }
        }
    }


    if (ptACDCurrentItem == ptACDItem)
        return TRUE;



    if (ptEquippedItemInvLocOld)
    {
        *ptEquippedItemInvLocOld = ptACDItem->GetInventoryLocation();
    }


    PlayerLoadoutUnsocketItemToLimbo(*ptACDItem, &listACDIDSocketables);


    {
        POS pos = listACDIDSocketables.GetHeadPosition();
        while (pos)
        {
            ACDID idACDItem = (ACDID)listACDIDSocketables.GetNext(pos);
            ActorCommonData * ptACDSocketable = ACDTryToGet(idACDItem);
            if (ptACDSocketable)
            {
                ptACDSocketable->AttributesSetInt(ATTRIBUTE_ITEM_WAS_TOUCHED_BY_ARMORY, 1);
                ptACDSocketable->AttributesSetInt(ATTRIBUTE_ITEM_WAS_TOUCHED_BY_ARMORY_LAST_GAME, 1);
            }
        }
    }

    InventoryLocation tLocDest;
    tLocDest.idACDContainer = ptACDPlayer->id;
    tLocDest.eSlot = eInvSlot;
    tLocDest.pt = cgptNull;

    return PlayerBrainTryInventoryMoveItem(*ptACDPlayer, *ptACDItem, tLocDest);
}





struct CountLegendaryGemsCallbackData {
    int nCount;


    CountLegendaryGemsCallbackData() : nCount(0) {}
};

static void sCountLegendaryGemsCallback(
    ACDID idACDSquare,
    InventorySlotType eSlot,
    void * pUserData)
{


    ActorCommonData * ptACDSocketable = ACDTryToGet(idACDSquare);
    if (!ptACDSocketable || !ItemIsSocketable(*ptACDSocketable) || !ItemIsJewel(ptACDSocketable->hGB.gbid))
        return;




    CountLegendaryGemsCallbackData * ptData = (CountLegendaryGemsCallbackData *)pUserData;
    ptData->nCount++;
}

static int32 sCountAndLogLegendaryGems(
    const ActorCommonData & tACDPlayer)
{
    CountLegendaryGemsCallbackData tData;

    for (int32 i = SLOT_PLAYER_BODYSLOT_START; i < SLOT_PLAYER_BODYSLOT_END; i++)
    {
        ACDInventoryIterateItems(tACDPlayer, i, 2, sCountLegendaryGemsCallback, &tData);
    }

    ACDInventoryIterateItems(tACDPlayer, SLOT_PLAYER_BACKPACK, 2, sCountLegendaryGemsCallback, &tData);

    ACDInventoryIterateItems(tACDPlayer, SLOT_PLAYER_SHARED_STASH, 2, sCountLegendaryGemsCallback, &tData);



    blz::vector<const ActorCommonData *> vecItemActors;
    ActorGetItemsOnGround(vecItemActors);
    for (const ActorCommonData *& ptACDItem : vecItemActors)
    {
        if (ItemIsJewel(ptACDItem->hGB.gbid) && ItemGetBoundToACD(*ptACDItem, TRUE) == tACDPlayer.id)
        {

            tData.nCount++;
        }
    }

    return tData.nCount;
}

static void sDumpLimbo(
    const Player & tPlayer)
{
    blz::vector<const ActorCommonData *> vecItemActors;
    ActorGetItemsInLimbo(vecItemActors);

    if (vecItemActors.size() == 0)
        return;

    for (const ActorCommonData *& ptACDItem : vecItemActors)
    {
        Actor * ptActorItem = ActorTryToGet((ActorID)ptACDItem->idOwner);
    }
}











struct FindMatchingPotionData {
    GBID gbidPotionToFind;
    ACDID idACDFoundPotion;
    InventoryLocation tInvLoc;

    FindMatchingPotionData()
        : gbidPotionToFind(GBID_NONE)
        , idACDFoundPotion(ACDID_INVALID)
    {
    }
    FindMatchingPotionData(GBID gbidPotionToFind)
        : gbidPotionToFind(gbidPotionToFind)
        , idACDFoundPotion(ACDID_INVALID)
    {
    }
};
BOOL sFindMatchingPotionCallback(
    const InventoryLocation & tInvLoc,
    void * pCallBackData)
{
    FindMatchingPotionData * ptPotionData = (FindMatchingPotionData *)pCallBackData;
    ActorCommonData & tACDContainer = ACDGet(tInvLoc.idACDContainer);
    ACDID idACDItem = ACDInventoryGetSquareItem(tACDContainer, tInvLoc.eSlot, tInvLoc.pt);
    if (idACDItem == ACDID_INVALID)
        return TRUE;

    ActorCommonData * ptACDItem = ACDTryToGet(idACDItem);
    if (ptACDItem == NULL)
        return TRUE;

    if (ptACDItem->hGB.gbid == ptPotionData->gbidPotionToFind)
    {
        ptPotionData->idACDFoundPotion = idACDItem;
        ptPotionData->tInvLoc = tInvLoc;
        return FALSE;
    }
    return TRUE;
}

BOOL SPlayerFindMatchingPotion(
    const ActorCommonData & tACDPlayer,
    GBID gbidPotion,
    ActorCommonData *& ptACDMatchingPotion,
    InventoryLocation & tMatchingPotionLoc)
{
    FindMatchingPotionData tFindPotionCallbackData(gbidPotion);
    ACDInventoryIterateSlot(tACDPlayer, SLOT_PLAYER_BACKPACK, NULL, sFindMatchingPotionCallback, &tFindPotionCallbackData, FALSE, FALSE);
    if (tFindPotionCallbackData.idACDFoundPotion == ACDID_INVALID)
        ACDInventoryIterateSlot(tACDPlayer, SLOT_PLAYER_SHARED_STASH, NULL, sFindMatchingPotionCallback, &tFindPotionCallbackData, FALSE, FALSE);

    if (tFindPotionCallbackData.idACDFoundPotion == ACDID_INVALID)
        return FALSE;

    ptACDMatchingPotion = ACDTryToGet(tFindPotionCallbackData.idACDFoundPotion);
    tMatchingPotionLoc = tFindPotionCallbackData.tInvLoc;
    return TRUE;
}











static void sEquipLoadoutPotion(
    Player & tPlayer,
    const LoadoutSavedData & tLoadout)
{
    ActorCommonData * ptACDPlayer = ACDTryToGet(tPlayer.idPlayerACD);
    if (!ptACDPlayer)
        return;




    InventoryLocation tInvLoc;
    ACDInventoryLocationSet(tInvLoc, tPlayer.idPlayerACD, SLOT_PLAYER_BASE_HEALTHPOTION, 0, 0);
    ACDID idDefaultPotion = ACDInventoryGetItemInSlot(tInvLoc);
    ACDNetworkName annDefaultPotion = ANNFromACD(idDefaultPotion);

    if (tLoadout.tEquippedPotion.annPotion == ANN_INVALID || tLoadout.tEquippedPotion.annPotion == annDefaultPotion)
    {


        HotbarButtonData tButtonData;
        tButtonData.SetItemANN(annDefaultPotion);
        PlayerSetPotionBtnAssignment(&tPlayer, tButtonData);
        return;
    }

    ActorCommonData * ptACDNewPotion = ACDTryToGet(ACDFromANN(tLoadout.tEquippedPotion.annPotion));
    if (!ptACDNewPotion)
        return;







    InventoryLocation tNewInvLoc;
    const InventoryLocation * ptNewPotionInvLoc = ptACDNewPotion->TryToGetInventoryLocation();
    if (ptNewPotionInvLoc)
    {

        if (ptNewPotionInvLoc->idACDContainer == tPlayer.idPlayerACD)
        {
            tNewInvLoc = *ptNewPotionInvLoc;
        }
        else
        {
            PlayerDisplayErrorMessage(ptACDPlayer->id, GAMEERROR_EQUIPMENT_MANAGER_CANNOT_EQUIP_POTION);
            return;
        }
    }
    else
    {


        if (!SPlayerFindMatchingPotion(*ptACDPlayer, tLoadout.tEquippedPotion.gbidPotion, ptACDNewPotion, tNewInvLoc))
        {
            PlayerDisplayErrorMessage(ptACDPlayer->id, GAMEERROR_EQUIPMENT_MANAGER_CANNOT_EQUIP_POTION);
            return;
        }
    }


    const HotbarButtonData & tOldButtonData = PlayerGetPotionBtnAssignment(&tPlayer);
    if (tOldButtonData.m_annItem != ANN_INVALID && tOldButtonData.m_annItem != annDefaultPotion)
    {

        ActorCommonData * ptACDOldPotion = ACDTryToGet(ACDFromANN(tOldButtonData.m_annItem));
        if (ptACDOldPotion)
        {








            const InventoryLocation * ptOldPotionInvLoc = ptACDOldPotion->TryToGetInventoryLocation();
            if (ptOldPotionInvLoc)
            {










                if (tNewInvLoc.eSlot == SLOT_PLAYER_SHARED_STASH)
                {

                    InventoryLocation tOldInvLoc = *ptOldPotionInvLoc;


                    SACDInventoryAddToDirtyList(*ptACDOldPotion, AIDA_REMOVE, SLOT_PLAYER_BACKPACK, -1);
                    ACDInventoryRemoveToLimbo(ptACDPlayer->id, *ptACDOldPotion, FALSE);


                    PlayerBrainTryInventoryMoveItem(*ptACDPlayer, *ptACDNewPotion, tOldInvLoc);


                    ActorLocation tLocation;
                    tLocation.eActorLocation = AL_INVENTORY;
                    tLocation.tInvLocation = tNewInvLoc;
                    ACDInventoryAddToLocation(*ptACDPlayer, *ptACDOldPotion, tLocation, FALSE, TRUE, FALSE, FALSE);
                }


                ACDNetworkName annNewPotion = ANNFromACD(ptACDNewPotion->id);
                HotbarButtonData tButtonData;
                tButtonData.SetItemANN(annNewPotion);
                PlayerSetPotionBtnAssignment(&tPlayer, tButtonData);
            }
        }
    }
    else
    {
        if (tNewInvLoc.eSlot == SLOT_PLAYER_SHARED_STASH)
        {
            if (SACDInventoryPickup(ptACDPlayer->id, ptACDNewPotion->id, EFFECT_INVALID, TRUE, FALSE, NULL, FALSE, FALSE, FALSE, TRUE) != PICKUP_RESPONSE_OK)
            {

                PlayerDisplayErrorMessage(tPlayer.idPlayerACD, GAMEERROR_EQUIPMENT_MANAGER_CANNOT_EQUIP_POTION);
                return;
            }
        }


        HotbarButtonData tButtonData;
        tButtonData.SetItemANN(tLoadout.tEquippedPotion.annPotion);
        PlayerSetPotionBtnAssignment(&tPlayer, tButtonData);
    }
}

void SPlayerLoadout_Equip(
    Player & tPlayer,
    int32 nLoadoutIndex)
{
    if (nLoadoutIndex < 0 || nLoadoutIndex >= 10)
        return;




    ActorCommonData * ptACDPlayer = ACDTryToGet(tPlayer.idPlayerACD);
    if (!ptACDPlayer)
        return;




    const LoadoutSavedData & tLoadout = tPlayer.tLoadouts[nLoadoutIndex];
    if (tLoadout.bEmpty)
        return;




    if (PlayerLoadoutMatchesCurrentEquipment(tPlayer, nLoadoutIndex))
        return;








    int32 nNumGemsAtStart = sCountAndLogLegendaryGems(*ptACDPlayer);



    BOOL bDropToStash = (tPlayer.dwServerSidePrefFlags & (1 << SSPREF_EQUIPMENT_MANAGER_DROP_TO_STASH)) != 0;





    ptACDPlayer->dwFlags |= (1 << ACD_SUPPRESS_PASSIVE_APPLICATION);


    GBIDArray arGBIDSocketables;
    PlayerLoadoutGetDesiredSocketables(arGBIDSocketables, tPlayer, nLoadoutIndex);

    XPooledDataIDList listACDIDSocketables(MemoryPoolsGetDATAIDListPool());
    PlayerLoadoutFindSocketables(listACDIDSocketables, *ptACDPlayer, &arGBIDSocketables);

    XPooledDataIDList listACDIDRemovedItems(MemoryPoolsGetDATAIDListPool());


    bool bNeedToForceCrusader2HPassive = false;
    if (PlayerGetPlayerClass(ptACDPlayer) == PLAYER_CLASS_CRUSADER)
    {

        SNO sno2HPower = SNOFileLookupGUID(POWER_SNO, "X1_Crusader_Passive_HeavenlyStrength");
        for (int32 nSlot = 0; nSlot < 4; nSlot++)
        {
            if (tLoadout.snoTraits[nSlot] == sno2HPower)
            {
                bNeedToForceCrusader2HPassive = true;
                break;
            }
        }





        ActorCommonData * ptACDItem = ACDTryToGet(ACDFromANN(tLoadout.tEquipment[SLOT_PLAYER_NECK - SLOT_PLAYER_BODYSLOT_START].annItem));
        if (ptACDItem)
        {
            GBID gbidHellfireAmulet = StringToGBID("x1_Amulet_norm_unique_25");
            if (ptACDItem->hGB.gbid == gbidHellfireAmulet)
            {

                if (ptACDItem->AttributesAttributeExists(MakeAttribKey(ATTRIBUTE_ITEM_POWER_PASSIVE, GlobalSNOGet(GLOBALSNO_POWERS_HELLFIRE_AMULET_HEAVENLY_STRENGTH))))
                {
                    bNeedToForceCrusader2HPassive = true;
                }
            }
        }
    }




    if (bNeedToForceCrusader2HPassive)
    {
        ptACDPlayer->AttributesSetInt(ATTRIBUTE_ALLOW_2H_AND_SHIELD, 1);
    }


    ActorCommonData * ptACDRemovedOffHandItem = sPlayerLoadoutCheckForOffHandRemoval(ptACDPlayer, tLoadout, listACDIDRemovedItems);


    sPlayerLoadoutEquipItemSlot(ptACDPlayer, tLoadout, SLOT_PLAYER_LEFT_HAND, listACDIDSocketables, listACDIDRemovedItems, bDropToStash, NULL);


    InventoryLocation tOffHandItemInvLocOold;
    sPlayerLoadoutEquipItemSlot(ptACDPlayer, tLoadout, SLOT_PLAYER_RIGHT_HAND, listACDIDSocketables, listACDIDRemovedItems, bDropToStash, &tOffHandItemInvLocOold);
    if (ptACDRemovedOffHandItem)
    {
        SACDInventoryPickupOrSpillOntoGround(*ptACDPlayer, *ptACDRemovedOffHandItem, bDropToStash, NULL, TRUE);
    }


    for (int32 nBodySlot = 0; nBodySlot < NUM_PLAYER_BODYSLOTS; nBodySlot++)
    {
        InventorySlotType eInvSlot = SLOT_PLAYER_BODYSLOT_START + nBodySlot;
        if (eInvSlot == SLOT_PLAYER_RIGHT_HAND || eInvSlot == SLOT_PLAYER_LEFT_HAND)
            continue;



        sPlayerLoadoutEquipItemSlot(ptACDPlayer, tLoadout, eInvSlot, listACDIDSocketables, listACDIDRemovedItems, bDropToStash, NULL);
    }


    for (int32 nOrderedSlot = 0; cgarPlayerLoadoutSocketableSlotsOrder[nOrderedSlot] != -1; nOrderedSlot++)
    {
        InventorySlotType eSlot = cgarPlayerLoadoutSocketableSlotsOrder[nOrderedSlot];

        ACDID idACDCurrentItem = ACDInventoryGetSquareItem(*ptACDPlayer, eSlot, cgptNull);
        ActorCommonData * ptACDCurrentItem = ACDTryToGet(idACDCurrentItem);
        if (!ptACDCurrentItem)
            continue;



        int32 nEquipmentSlotIndex = eSlot - SLOT_PLAYER_BODYSLOT_START;
        sPlayerLoadoutEquipGems(*ptACDCurrentItem, listACDIDSocketables, tLoadout.tEquipment[nEquipmentSlotIndex]);
    }


    sEquipLoadoutPotion(tPlayer, tLoadout);


    POS posSocketables = listACDIDSocketables.GetHeadPosition();
    while (posSocketables)
    {
        ACDID idACDSocketable = (ACDID)listACDIDSocketables.GetNext(posSocketables);

        ActorCommonData * ptACDSocketable = ACDTryToGet(idACDSocketable);
        if (!ptACDSocketable)
            continue;



        if (ptACDSocketable->GetLocationType() == AL_NONE)
        {

            ACDID idACDPickedUpItem = ptACDSocketable->id;
            LPCSTR szPickedUpItem = GBIDToString(ptACDSocketable->hGB, FALSE).str();
            uint64 dwStackCount = ptACDSocketable->GetStackCount();


            ACDID idACDSocketableStack = ACDID_INVALID;
            BOOL fSuccess = SACDInventoryPickupOrSpillOntoGround(*ptACDPlayer, *ptACDSocketable, bDropToStash, &idACDSocketableStack, TRUE);













            listACDIDRemovedItems.AddTail_Tracked(idACDSocketableStack != ACDID_INVALID ? idACDSocketableStack : ptACDSocketable->id);
        }
    }


    if (bNeedToForceCrusader2HPassive)
    {
        ptACDPlayer->AttributesSetInt(ATTRIBUTE_ALLOW_2H_AND_SHIELD, 0);
    }





    for (int32 nSlot = 0; nSlot < 6; nSlot++)
    {
        SNO snoPower = tLoadout.tActiveSkills[nSlot].snoPower;

        SPlayerSwitchActiveSkills(tPlayer, snoPower, tLoadout.tActiveSkills[nSlot].eRune, nSlot, FALSE);
    }

















    for (int32 nSlot = 0; nSlot < 4; nSlot++)
    {
        SPlayerRemoveTraitInSlot(tPlayer, nSlot);
    }


    ActorCommonData & tACDPlayer = ACDGet(tPlayer.idPlayerACD);
    for (int32 nSlot = 0; nSlot < 4; nSlot++)
    {
        SNO snoPower = tLoadout.snoTraits[nSlot];

        PlayerBrainSpendTraitPoint(tACDPlayer, snoPower);
        SPlayerAssignToTraitSlot(tPlayer, snoPower, nSlot, FALSE);
    }










    for (int32 nSlot = 0; nSlot < 3; nSlot++)
    {
        SPlayerAssignToLegendaryPowerSlot(tPlayer, tLoadout.gbidLegendaryPowers[nSlot], nSlot);
    }


    ptACDPlayer->dwFlags &= ~(1 << ACD_SUPPRESS_PASSIVE_APPLICATION);
    ptACDPlayer->ApplyAllPassives(FALSE);

    if (TimerExists(tPlayer.idTimerLoadoutEquipApplyPassives))
    {
        TimerUnregister(tPlayer.idTimerLoadoutEquipApplyPassives);
        tPlayer.idTimerLoadoutEquipApplyPassives = TIMERID_INVALID;
    }







    BOOL bSocketsChanged = arGBIDSocketables.size() > 0 && listACDIDSocketables.Count() > 0;

    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetSingle(listDestPlayers, tPlayer);

    PlayerLoadoutEquipResultsMessage tMessage;
    tMessage.bSocketsChanged = bSocketsChanged;
    tMessage.nRemoveItemANNsCount = 0;

    POS posRemovedItems = listACDIDRemovedItems.GetHeadPosition();
    while (posRemovedItems)
    {
        ACDID idACDRemovedItem = (ACDID)listACDIDRemovedItems.GetNext(posRemovedItems);

        ActorCommonData * ptACDRemovedItem = ACDTryToGet(idACDRemovedItem);
        if (ptACDRemovedItem
            && ptACDRemovedItem->IsInInventory()
            && ptACDRemovedItem->GetInventoryLocation().eSlot == SLOT_PLAYER_BACKPACK)
        {




            if (tMessage.nRemoveItemANNsCount >= 35)
                break;






            tMessage.arRemovedItemANNs[tMessage.nRemoveItemANNsCount] = ANNFromACD(idACDRemovedItem);
            tMessage.nRemoveItemANNsCount++;
        }
    }


    int32 nNumGemsAtEnd = sCountAndLogLegendaryGems(*ptACDPlayer);







    sDumpLimbo(tPlayer);


    MessageSendToClient(listDestPlayers, SMSG_LOADOUT_EQUIP_NOTIFICATION, &tMessage, sizeof(tMessage));

    ptACDPlayer->ResetLifeStats(TRUE, TRUE, FALSE, FALSE, FALSE);
}



void SPlayerLoadout_Save(
    Player & tPlayer,
    int32 nLoadoutIndex,
    LPCSTR szName)
{
    if (nLoadoutIndex < 0 || nLoadoutIndex >= 10)
        return;




    ActorCommonData * ptACDPlayer = ACDTryToGet(tPlayer.idPlayerACD);
    if (!ptACDPlayer)
        return;




    LoadoutSavedData & tLoadout = tPlayer.tLoadouts[nLoadoutIndex];
    tLoadout.Initialize();

    for (int32 i = 0; i < 13; i++)
    {
        ACDID idACDItem = ACDInventoryGetSquareItem(*ptACDPlayer, SLOT_PLAYER_BODYSLOT_START + i, cgptNull);
        ActorCommonData * ptACDItem = ACDTryToGet(idACDItem);
        if (ptACDItem)
        {



            PlayerLoadoutItemUpdate(tLoadout.tEquipment[i], *ptACDItem);
        }
    }
    for (int32 i = 0; i < 6; i++)
    {
        tLoadout.tActiveSkills[i].snoPower = tPlayer.tHeroStateData.tSavedData.tActiveSkills[i].snoSkill;
        tLoadout.tActiveSkills[i].eRune = tPlayer.tHeroStateData.tSavedData.tActiveSkills[i].eRuneType;
    }

    SigmaMemoryCopy(tLoadout.snoTraits, tPlayer.tHeroStateData.tSavedData.snoTraits, sizeof(tLoadout.snoTraits));
    SigmaMemoryCopy(tLoadout.gbidLegendaryPowers, tPlayer.tHeroStateData.tSavedData.gbidLegendaryPowers, sizeof(tLoadout.gbidLegendaryPowers));

    if (szName)
    {
        Blizzard::String::Copy(tLoadout.usName, szName, sizeof(tLoadout.usName));
    }
    else
    {
        SigmaMemoryClear(tLoadout.usName, sizeof(tLoadout.usName));
    }

    const HotbarButtonData & tButtonData = PlayerGetPotionBtnAssignment(&tPlayer);
    if (tButtonData.m_annItem != ANN_INVALID)
    {
        ActorCommonData * ptACDPotion = ACDTryToGet(ACDFromANN(tButtonData.m_annItem));
        if (ptACDPotion)
        {
            PlayerLoadoutPotionUpdate(tLoadout.tEquippedPotion, *ptACDPotion);
        }
    }

    tLoadout.bEmpty = FALSE;

    PlayerSendHeroStateLoadoutToClient(tPlayer, nLoadoutIndex, TRUE);
}











































































void SPlayerUpdateLoadoutItemANNs(
    Player & tPlayer,
    const ActorCommonData * ptACDItem,
    ACDNetworkName annItemOld)
{
    if (!ItemIsEquippable(ptACDItem->hGB.gbid))
    {
        return;
    }

    Actor * ptActorItem = ActorTryToGet((ActorID)ptACDItem->idOwner);
    if (!ptActorItem)
    {
        return;
    }

    for (int32 i = 0; i < 10; i++)
    {
        LoadoutSavedData & tLoadout = tPlayer.tLoadouts[i];
        for (int32 j = 0; j < 13; j++)
        {
            LoadoutItemData & tLoadoutItem = tLoadout.tEquipment[j];

            BOOL bSendUpdate = FALSE;
            if (annItemOld != ANN_INVALID)
            {
                if (tLoadoutItem.annItem == annItemOld)
                {
                    PlayerLoadoutItemUpdate(tLoadoutItem, *ptACDItem);
                    bSendUpdate = TRUE;
                }
            }
            else
            {
                if (ptActorItem->tItemId.GetLow() == tLoadoutItem.uOriginalItemIdLow)
                {
                    PlayerLoadoutItemUpdate(tLoadoutItem, *ptACDItem);
                    bSendUpdate = TRUE;
                }
            }

            if (bSendUpdate)
            {
                PlayerLoadoutEquipmentUpdateMessage tMessage;
                tMessage.nLoadoutIndex = i;
                tMessage.nEquipmentIndex = j;
                tMessage.tNewItem = tLoadoutItem;

                PlayerList listPlayers(PlayerListGetElementPool());
                PlayerListGetSingle(listPlayers, tPlayer);
                MessageSendToClient(listPlayers, SMSG_LOADOUT_UPDATE, &tMessage, sizeof(tMessage));
            }
        }
    }
}



