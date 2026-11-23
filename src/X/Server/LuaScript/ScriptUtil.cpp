// Reconstructed from original 2.6.2 DWARF and instruction listings.
// Functions are in their original order; line positions are not kept (none of the
// functions written so far embeds __LINE__).
#include "ScriptUtil.h"
#include "LuaScript.h"
#include "ScriptEvent.h"
#include "../../Common/ActorCommonData.h"
#include "../../Common/Attributes/FastAttributeStructs.h"
#include "../../Common/Game.h"
#include "../../Common/GameBalance.h"
#include "../../Common/Player.h"
#include "../../Common/Marker/Marker.h"
#include "../../Common/Shell/CmdLine.h"
#include "../../Client/CGame.h"
#include "../Map/Minimap.h"
#include "../SGame.h"
#include "../../Common/ActorInventory.h"
#include "../../Common/Gizmo.h"
#include "../../Common/Definitions/GlobalDefinition.h"
#include "../../Common/SNO/SNOName.h"
#include "../../Sigma/Math/Vector.h"
#include "../../Sigma/Random.h"
#include "../../Common/Target.h"
#include "../AI/ActorAI.h"
#include <string.h>
#include "../../Sigma/Math/MathGeneral.h"
#include "../SGameConstants.h"
#include "../../Common/SNO/SNOFiles.h"
#include "../../Common/Definitions/QuestDefinition.h"
#include "../../Common/GlobalSNO.h"
#include "../../Sigma/MemoryPools.h"
#include "../PlayerList.h"
#include "../../Common/Networking/GameMessage.h"
#include "../../Client/Prefetch.h"
#include "../SGameManager.h"
#include "../SScene.h"
#include "../SConductor.h"
#include "../System/SetEncounter.h"
#include "../System/EncounterTypes.h"
#include "../../Common/ItemsSwig.h"
#include "../AI/AIStates.h"
#include "../../Common/Items.h"
#include "../SWorld.h"
#include "../SPlayer.h"
#include "../Actors/ActorPowers.h"
#include "../../Common/Definitions/GameBalanceDefinition.h"
#include "../../Common/Definitions/SceneGroupDefinition.h"
#include "../../Sigma/DataStructures/XOptional.h"
#include "../../Common/Nav/NavigationMesh.h"
#include "../../Common/TagMaps/ActorTags.h"
#include "../../Common/TagMaps/Tags.h"
#include "../../Common/Actors/ActorEffectCommon.h"
#include "../Actors/ActorEffect.h"
#include "../Network/KnownMessage.h"
#include "../../Common/TagMaps/AnimTagsSwig.h"
#include "../../Common/GizmoSwig.h"
#include "../../Common/HirelingSwig.h"
#include "../Actors/Actors.h"
#include "../../Common/Powers/PowersCommon.h"
#include "../../Common/Definitions/MonsterDefinition.h"
#include "../Powers/PowerAPI.h"
#include "../../Common/AnimPlayer.h"
#include "../../Common/GameConstants.h"
#include "../../Common/Definitions/MarkerSetDefinition.h"
#include "../../Common/TagMaps/MarkerTags.h"
#include "../Achievements/AchievementModifierEvaluationLogic.h"
#include "../../Common/Achievements/AchievementDefines.h"
#include "../AI/BrainStance.h"
#include "../../../NintendoSDK/Include/nn/ldn/ldn_Types.h"  // Inference: header statics run by the original static initializer (ldn_Ipv4Address.h:153-163, ldn_MacAddress.h:101-106; _GLOBAL__sub_I_ScriptUtil.cpp DWARF).
// ---- Declarations whose headers are not in src/ yet (or lack them) ----
void SComplexEffectRemove(const ACDID idACD, const int32 idSComplexEffect);       // X/Server/Actors/SComplexEffect.h
BOOL SComplexEffectIsValid(const ACDID idACD, const int32 idSComplexEffect);      // X/Server/Actors/SComplexEffect.h
BOOL HeroIsRanged(ACDID idACDPlayer);                                             // X/Common/Hero.h
BOOL HirelingTestTypeAvailable(ACDID idACDPlayer, HirelingClass eClass);          // X/Common/Hireling.h
float SPlayerGetAFKTime(const ACDID idACDPlayer);                                 // X/Server/SPlayer.h
float SPlayerGetInactionTime(const ACDID idACDPlayer);                            // X/Server/SPlayer.h
BOOL GameCommonGameFlagIsSet(LPCSTR szFlagLabel);                                 // X/Common/GameCommonData.h
BOOL LuaScriptHasHighScriptInstanceCount();                                       // X/Server/LuaScript/LuaScript.h
void LuaScriptErrorHandlerEnter();                                                // X/Server/LuaScript/LuaScript.h
void LuaScriptErrorHandlerExit();                                                 // X/Server/LuaScript/LuaScript.h
void PvPTeamDeathmatchGameEnd();                                                  // X/Server/PVP.h
void PVPApplyDuelingBuff(Player & tPlayer);                                       // X/Server/PVP.h
void ACDLookAtACD(const ACDID idACDSrc, const ACDID idACDTarget);                 // X/Common/ActorCommonData.h
BOOL ActorShouldThrottleScriptActors();                                           // X/Server/Actors/Actors.h
void SWorldDungeonFinderCreate(SNO snoDungeonFinder);                             // X/Server/SWorld.h
BOOL SWorldDungeonFinderIsEmpty();                                                // X/Server/SWorld.h
void SWorldResetDungeonFinder();                                                  // X/Server/SWorld.h
SNOGroup SNOMLGetSNOGroupTypeFromClassName(LPCSTR szClassName);                    // X/Common/SNO/SNOML.h
void BrainAPITryToFlee(ACDID idACD);                                              // X/Server/AI/BrainComponents.h
void BrainAPIGetFrontOfPlace(const ActorCommonData & tACDTarget, float flDist, WorldPlace & placeTarget);  // X/Server/AI/BrainComponents.h
void AIQueueCompleteCurrentAction(ActorID idActor);                               // X/Server/AI/AIQueue.h
void AIQueueClearList(ActorID idActor);                                           // X/Server/AI/AIQueue.h
BOOL SItemApplyDye(ActorCommonData & tACDItem, ActorCommonData & tACDOwner, DyeType eDyeType);  // X/Server/SItems.h
void SItemUpgradeJewel(ActorCommonData & tACDItem, int32 nUpgradeRanks);          // X/Server/SItems.h
enum { MF_TREASURE_GOBLIN = 11 };                                                  // X/Common/ActorCommonDataSwig.h:41
AIQueuedActionID AIQueueMoveToLocation(ACDID idACD, const Target & tTarget, ActorAnimationTag eAnimTag, float flSpeed);  // X/Server/AI/AIQueue.h
void ActorAddToQuestLifespanActors(ActorID idActor);                              // X/Server/Actors/Actors.h
void ServerMessageSendTreasureGoblinEscapedMsg(const ActorCommonData & tACDMonster);  // X/Server/Network/ServerMessages.h
namespace Tutorial {  // X/Server/Tutorial.h
BOOL SendTutorialToClient(SNO snoTutorial, Player * ptPlayer, const char * szUIAnchorName, const ACDID idACDTarget);
}
BOOL BrainAPIActorHasTarget(ActorID idActor, ActorID idTarget);                   // X/Server/AI/BrainComponents.h
void BrainAPIDoDeath(ActorID idDeadActor, ActorID idKillerActor, const BOOL fNoExp, const BOOL fNoLoot, const BOOL fNoHealthDrop, PowerAPIDamage * ptDamage, float * pflDamageDone);  // X/Server/AI/BrainComponents.h
void SWorldResetSetDungeon();                                                     // X/Server/SWorld.h
BOOL SWorldPlayerWarpToWaypoint(const ACDID idACDPlayer, const ACDID idACDWaypoint,
                                const int32 nWaypointNumber);                     // X/Server/SWorld.h
SNO SGamePickDungeonFinderLevelArea();                                            // X/Server/SGame.h
void SGameChangeActs(const Act eAct, SNO snoForceToWorld, BOOL bMovePlayers);     // X/Server/SGame.h
BOOL QuestDungeonFinderIsComplete();                                              // X/Common/Quests/Quests.h
ACDID SHirelingGetNearby(ACDID idACDOrigin, float flRangeSq, HirelingClass eHirelingClass);  // X/Server/SHireling.h
PlayerClass PlayerGetPlayerClass(const ACDID idACDPlayer);                         // X/Common/Player.h
CRefString SNOFileGetName(const SNOName & snoname);                               // X/Common/SNO/SNOFiles.h
SNO ACDGetLevelArea(const ActorCommonData & tACD);                                // X/Common/ActorCommonData.h
BOOL ACDIsInTown(const ActorCommonData & tACD);                                   // X/Common/ActorCommonData.h
BOOL ACDIsInNoBanterArea(const ActorCommonData & tACD);                           // X/Common/ActorCommonData.h
BOOL ItemIsLegendary(GBID gbidItem);                                              // X/Common/Items.h
ACDID ItemGetAssignedACDID(const ActorCommonData & tACDItem);                     // X/Common/Items.h
BOOL ItemIsAncientRank(const ActorCommonData & tACDItem);                         // X/Common/Items.h
BOOL ItemIsBroken(const ActorCommonData & tACDItem);                              // X/Common/Items.h
int32 ActorDeleteSoon(ActorID idActor, int32 nDuration);                          // X/Server/Actors/Actors.h
void NPCHandleDismissal(ActorCommonData & tACDHireling, BOOL bPlayerDismissed,
                        BOOL bDisplayChatMessage);                                // X/Server/SNPC.h
BOOL AIStateSetState(ActorID idActor, AIStateType eStateType, BOOL bForceAnimSet); // X/Server/AI/AIState.h
struct ActiveConversation;
namespace ConversationMgr {  // X/Server/ConversationMgr.h
void CancelConversation(const SNO snoConv);
void UpdateIdleForActor(ACDID idACD, const uint32 dwAnimTag);
BOOL StartConversation(const int32 nPlayerTalkingIndex, const SNO snoConversation, const ACDID idACDTalkingTo, const ACDID idACDAlt1, const ACDID idACDAlt2, const ACDID idACDAlt3, const ACDID idACDAlt4, const int32 nPriority, const BOOL bForceInterrupt, const BOOL bPlayerInitiated);
BOOL PlayerOptOutOfAllConversations(const Player & tPlayer, ActiveConversation * ptConv);
}  // namespace ConversationMgr
BOOL PlayerIsPlayerGameFlagSet(const Player * ptPlayer, LPCSTR szFlagLabel);       // X/Common/Player.h
BOOL PlayerSetPlayerGameFlag(Player * ptPlayer, LPCSTR szFlagLabel, BOOL bValue);   // X/Server/SPlayer.h
void SPlayerAddNephalemBuffStack(ACDID idACDPlayer);                              // X/Server/SPlayer.h
BOOL SGameIsPvPFlagSet();                                                         // X/Server/SGame.h
BOOL SGameSetOpenWorldMode(Player * ptPlayerInitiating, int32 eOpenWorldMode, Act eOpenWorldModeAct,
                           int32 nOpenWorldModeParam, SNO snoHubWorld, uint32 hubEntranceGUID);  // X/Server/SGame.h
int32 GameGetPVPGameState();  // returns PVPGameState                                               // X/Common/GameCommonData.h
PVPGameResult GameGetPVPGameResultForTeam(Team eTeam);                            // X/Common/GameCommonData.h
void SConductorInstanceStart(SNO snoConductor);                                   // X/Server/SConductor.h
int32 GetCraftLevel(const CrafterType eType, const Player & tPlayer);             // X/Common/Crafting/Crafting.h
void SCrafterUnlock(Player & tPlayer, const CrafterType eType);                   // X/Server/Crafting/SCrafting.h
void ExperienceSetLevel(const ACDID idACD, int32 nLevels, const BOOL fUpdateClient);  // X/Server/Experience.h
AIQueuedActionID AIQueueAddAnim(ActorID idActor, uint32 uAnimTag, uint32 uTicks, uint32 uNewIdleAnimTag);  // X/Server/AI/AIQueue.h
void LuaScriptInstanceSetLastAIQAction(ScriptInstanceID idScriptInstance, AIQueuedActionID idQAction);  // X/Server/LuaScript/LuaScript.h
void SGameSyncData(const Player * ptPlayer);                                      // X/Server/SGame.h
void sActorDeleteFromScript(VACDID idACD);                                        // defined below (line 2101)
void GateTriggeringACDCountChanged(ActorCommonData & tACDGate, const int32 nDelta); // X/Server/SGizmo.h
void STimedEventStart(const SNO snoTimedEvent, const int32 nOverrideDuration);    // X/Server/STimedEvents.h
BOOL STimedEventHasExpired(const SNO snoTimedEvent);                              // X/Server/STimedEvents.h
void STimedEventReset(const SNO snoTimedEvent);                                   // X/Server/STimedEvents.h
void SPlayerLearnLore(const ACDID idACDPlayer, const SNO snoLore);                 // X/Server/SPlayer.h
BOOL SPlayerHasLearnedLore(const ACDID idACDPlayer, const SNO snoLore);           // X/Server/SPlayer.h
void SPlayerAllPlayersLearnLore(const SNO snoLore);                               // X/Server/SPlayer.h
BOOL HirelingHasUnspentSkillPoints(const ActorCommonData & tACDHireling);         // X/Common/Hireling.h
AIQueuedActionID AIQueueAddAction(ActorID idActor, SNO snoPower, const Target & tTarget, int32 uData, float flData,
                                  LPCSTR szData, AIQueuedActionType eType);      // X/Server/AI/AIQueue.h
void InactiveMarkerSetCanActivate(uint32 tMarkerName, BOOL bValue);              // X/Common/InactiveMarker.h

namespace NephalemRiftManager {  // X/Server/System/SNephalemRift.h
int32 RiftPlayerCount();
void PlayerStateSetNewSoloRecord(Player * ptPlayer, BOOL bHasNewSoloRecord);
void UnlockRiftLevelsForParticipants(int32 nRiftTierCompleted);
int32 GetActiveRiftLevel();
void PlayersGiveAllRiftRewardChoices(BOOL bSuccess);
int32 GetArtificiallyElapsedTimeMS();
ACDID GetJewelUpgradeVendor();
void SetJewelUpgradeVendor(ACDID idACDJewelUpgradeVendor);
void Cleanup();
BOOL PlayerPaidForRift(const Player * ptPlayer);
BOOL IsTieredRiftOrphaned(Player * ptPlayerToIgnore);
BOOL IsTieredRiftActive();
BOOL IsAnyRiftActive();
void TryStartBlizzonRift(ACDID idObeliskGizmo, int32 nRiftTier, ACDID idACDPlayer);
void PlayerManuallyPortIntoRift(Player * ptPlayer);
}  // namespace NephalemRiftManager

namespace GameTesting {  // X/Common/GameTesting.h
uint32 GetSampleTime();
uint32 GetWorldAdvanceWaitTime();
void StartSampling();
void StopSampling();
void Advance();
void TriggerSkill();
void SetupPerfSkillTest();
void AddDungeonFinder(LPCSTR szDungeonFinder, const Act eAct);
void AddWorld(LPCSTR szWorld, const Act eAct, SNO snoQuest, int32 nStepUID);
void KillAllMonsters(ActorID idActor, BOOL bIncludeElites, BOOL bIncludeBosses, float flRadius);
}  // namespace GameTesting

void FadeOut(VACDID idACD, int32 nFadeTime);
static AIQueuedActionID sQueueAIAction(SNO snoPower, ACDID idACDSource, ACDID idACDTarget, int32 uData);
static AIQueuedActionID sQueueAIActionCustomType(AIQueuedActionType eType, ACDID idACD, ACDID idACDTarget, int32 nParam, float flParam, LPCSTR szParam);

ACDID sCreateItemInInventory(ACDID idACDPlayer, LPCSTR szItemName, BOOL bTryStashFirst, LPCSTR szAffix1,
                             LPCSTR szAffix2, LPCSTR szAffix3, LPCSTR szAffix4, LPCSTR szAffix5, LPCSTR szAffix6,
                             BOOL bRewardUI, BOOL bIgnoreInheritedAffixes);  // defined below (line 4026)
SNOGroup SNOMLGetSNOGroupTypeFromClassName(LPCSTR szClassName);                 // X/Common/SNO/SNOML.h
void PlayerBrainTryInventoryUseItem(const Player & tPlayer, const ACDID idACDItem, const Target & tTarget);  // X/Server/AI/PlayerBrain.h
void SPlayerClearAllReturnPoints(Player & tPlayer, SNO snoWorld);                 // X/Server/SPlayer.h
// ---- Game message types and ids this file sends (X/Common/Networking/GameMessage.h) ----



































































enum { FLOATING_NUMBER_TYPE_PVP_KILL = 35 };























































void SWorldResetGreedRealm(float flDelay);
enum {  // X/Common/Actors/ComplexEffect.h
    COMPLEXEFFECT_TYPE_NORMAL = 0,
    COMPLEXEFFECT_TYPE_BEAM_TARGET = 1,
    COMPLEXEFFECT_TYPE_SPECIAL_ROPE = 2,
    COMPLEXEFFECT_TYPE_COUNT = 3,
};
typedef int32 ComplexEffectType;                                                  // X/Server/Actors/SComplexEffect.h
int32 SComplexEffectAdd(ActorCommonData & tACD, const SNO snoEffectGroup, const ACDID idACDDestParam, ComplexEffectType eType,
                        uint32 dwParam1, uint32 dwParam2, BOOL bIgnoreOwnerAlpha);  // X/Server/Actors/SComplexEffect.h
BOOL ACDInSameWorld(const ActorCommonData & tACD1, const ActorCommonData & tACD2);  // X/Common/ActorCommonData.h
void BrainAPIDoIdle(ActorID idActor);
BOOL ExperienceGrantDirectly(ActorCommonData & tACDPlayer, const uint64 nExpGranted, const BOOL bNotifyClient, const BOOL bGrantToAllPlayersIfLocal);  // Experience.h (Experience.cpp:699)
void PVPExperienceGrantDirectly(ActorCommonData & tACDPlayer, const uint64 nPVPExpGranted, const BOOL bNotifyClient);  // Experience.h (Experience.cpp:764)
void SPlayerMarkConversationAsHeard(Player * ptPlayer, const SNO snoConversation);  // SPlayer.h (SPlayer.cpp:5816)
void SPlayerClearHeardConversation(Player & tPlayer, const SNO snoConv);  // SPlayer.h (SPlayer.cpp:5847)
void BrainAPIActorAddTarget(ActorID idActor, ActorID idTarget, BOOL bRequireLOS, BOOL bAddSummoner, uint32 uDelayTicks, BOOL bForceTarget);  // BrainComponents.h (BrainComponents.cpp:9426)
void SPlayerAssignToEmptyActiveSkillSlot(Player & tPlayer, const SNO snoPower);  // SPlayer.h (SPlayer.cpp:6483)
void SPlayerAssignToEmptyTraitSlot(Player & tPlayer, const SNO snoPower);  // SPlayer.h (SPlayer.cpp:6535)
void SPlayerAllPlayersLearnLore(const SNO snoLore);  // SPlayer.h (SPlayer.cpp:7245)
void AISetTether(ActorID idActor, TetherType eTetherType, float flRadius, const WorldPlace & placeTetherPoint);  // ActorAI.h (ActorAI.cpp:3884)
BOOL ACDInventoryCanHold(const ActorCommonData & tACD, const ActorCommonData & tACDItem, const InventorySlotType eSlot, IVector2D & tWhereCanHold, const int32 * pnSlotYMinMax);  // ActorCommonData.h (ActorCommonData.cpp:869)
BOOL ACDInventoryMove(ActorCommonData & tACDItem, const InventoryLocation & tDstLocation);  // ActorCommonData.h (ActorCommonData.cpp:4018)
void ACDInventoryLocationSet(InventoryLocation & tInvLoc, const ACDID idACDContainer, const InventorySlotType eSlot, const int32 nSlotX, const int32 nSlotY);  // ActorCommonData.h (ActorCommonData.cpp:9743)
void AIClearTether(ActorID idActor);  // ActorAI.h (ActorAI.cpp:3904)
BOOL ACDWarpToACD(ActorCommonData & tACDToWarp, const ActorCommonData & tACDTarget, const PlayerWarpReason eReason, float flMinWarpRadius);  // ActorAI.h (ActorAI.cpp:1802)
void NPCUpdateConversationIconAndInteraction(ActorCommonData & tACDNPC, const Player & tPlayer);  // SNPC.h (SNPC.cpp:438)
ACDID SHirelingCreate(ActorCommonData & tACDPlayer, ActorCommonData & tACDNextTo, const HirelingClass eHirelingClass, BOOL bForceCreate, HirelingType eType);  // SHireling.h (SHireling.cpp:642)
BOOL GizmoRequiresOperator(const GizmoType eGizmoType);  // SGizmo.h (SGizmo.cpp:3039)
void GizmoOperate(ActorCommonData & tACDGizmo, ActorCommonData * ptACDOperator, BOOL bForceOperation, BOOL bAlreadyConfirmed);  // SGizmo.h (SGizmo.cpp:2007)
BOOL LabelAlreadySetInWorld(GBID gbidLabel, const WorldCommonData & tWcd);  // Labels.h (Labels.cpp:787)
void SSceneSwapSubScene(SSceneID idSScene, SNO snoNewScene);  // SScene.h
void ACDSetYaw(ACDID idACD, const Angle aYaw);  // ActorCommonData.h (ActorCommonData.cpp:3027)
SNO ActorDefinitionGetTagMapValueSNO(const ActorTag eTag, const SNO snoActor);   // X/Common/Definitions/ActorDefinition.h
void SWorldSetDungeonFinderBossEncounter(SNO snoBossEncounter);                   // X/Server/SWorld.h
void SWorldRecordPylonSpawnForRift(ActorCommonData & tACDPylon);                  // X/Server/SWorld.h
struct QuestRangeResult;
BOOL QuestRangeEvaluate(const SNO snoQuestRange, QuestRangeResult * pResult);     // X/Common/Quests/QuestRange.h
int32 * ActorAIGetTempAffixWeightTotal();                                         // X/Server/AI/ActorAI.h
struct HirelingSwapMessage {  // size 0xc  (X/Common/Hireling.h)
    RequiredMessageHeader tHeader;  // 0x00
    HirelingClass eNewClass;  // 0x08
};
static_assert(sizeof(HirelingSwapMessage) == 0xc, "HirelingSwapMessage");





void HirelingSetTypeAvailable(ACDID idACDPlayer, HirelingClass eClass, BOOL fAvailable);  // X/Common/Hireling.h
Player * PlayerGetNextInTieredRift(const Player * ptPlayer);                     // X/Common/Player.h
void BrainAPIDoScriptedDeath(ActorCommonData & tACD, DeathType eDeathType, BOOL bCrit, BOOL bDropLoot, ActorID idKiller);  // X/Server/AI/BrainComponents.h
void BrainAPIDoResurrect(ActorID idActor, BOOL bForce);                           // X/Server/AI/BrainComponents.h
void BrainAPIDoPlayAnimation(const ActorID idActor, const uint32 dwAnimTag, const uint32 dwDuration,
                             const BOOL bAllowPlayingAnimInterrupt, const BOOL bInterruptableAnim, float flUnitStartTime,
                             const BOOL bDontSendToClient, const float flSpeedScalar);  // X/Server/AI/BrainComponents.h
BOOL PowerActorCanResurrect(ActorID idActor);                                     // X/Server/ActorIsAffected.h
void LuaScriptInstanceRemoveRefACD(ScriptInstanceID idScriptInstance, ACDID idACD);  // X/Server/LuaScript/LuaScript.h
extern const int32 MAX_FADE_TIME;                                                 // X/Common/Networking/GameMessage.h
void ACDCollDisposeSphereNode(ActorCommonData & tACD);                            // X/Common/ActorCommonCollision.h
void PlayerListGetThatKnowACD(PlayerList & listPlayers, const ACDID idACD, BOOL bIgnoreACD);  // X/Server/PlayerList.h
void PlayerListGetAllies(PlayerList & listPlayers, ACDID idACD, ACDID idACDIgnore);  // X/Server/PlayerList.h
void STimedEventModifyExpirationTime(const SNO snoTimedEvent, int32 nTicks);      // X/Server/STimedEvents.h
void QuestsLoad(SNO snoQuest, SNO snoLevelArea);                                  // X/Common/Quests/Quests.h
struct QuestInstance;
void SQuestsResetQuest(QuestInstance & tQuestInstance);                           // X/Server/Quests/SQuests.h
void SQuestsSendUpdateToAllPlayers(SNO snoQuest);                                 // X/Server/Quests/SQuests.h                                             // X/Server/AI/BrainComponents.h
BOOL FastAttribGroupIsValid(AttribGroupID idGroup);                               // X/Common/Attributes/FastAttribute.h
BOOL PowerActorCanBeKnockedBack(ActorID idActor, PowerID idPower);                // X/Server/ActorIsAffected.h
SNO ACDGetTreasureClass(const ActorCommonData & tACD, const ACDID idACDLooter);   // X/Server/Loot/Loot.h                                        // X/Server/SWorld.h

struct PlayMusicMessage {  // size 0xc  (X/Common/Actors/ActorEffectCommon.h)
    RequiredMessageHeader tHeader;  // 0x00
    SNO snoMusic;  // 0x08
};
static_assert(sizeof(PlayMusicMessage) == 0xc, "PlayMusicMessage");















void PlayerListGetAllContentFlagRestrictedPlayers(PlayerList & listPlayers, uint32 uContentFlagsRequired);  // X/Server/PlayerList.h
void QuestSetGameOver();                                                          // X/Common/Quests/Quests.h
void SPlayerUnlockAdventureMode(Player & tPlayer);                                // X/Server/SPlayer.h
namespace OnlineService {  // X/Common/OnlineService/OnlineService.h
BOOL IsPTR();
}













// X/Common/Quests/Quests.h






#include "../../Common/Quests/Quests.h"











static_assert(sizeof(QuestInstance) == 0x168, "QuestInstance");
QuestInstance * QuestsGetQuestInstance(SNO snoQuest, ACDID idACDPlayer);         // X/Common/Quests/Quests.h
SWorldID WorldCommonFindBySNO(SNO snoWorld, BOOL bIgnorePendingDelete);         // X/Common/World.h
void SWorldSyncData(SWorldID idSWorld, const Player * ptPlayer);                  // X/Server/SWorld.h
void SConductorFlagForAdvancement(ConductorInstance & tConductorInstance);        // X/Server/SConductor.h
void SConductorFlagForPhaseSkip(ConductorInstance & tConductorInstance, LPCSTR szPhaseName);  // X/Server/SConductor.h
float SGameStatsPlayerCounterFetch(const Player * ptPlayer, StatCounterType eType, int32 nParam0, int32 nParam1);  // X/Server/SGameStats.h
float SGameStatsPlayerCurrentLifeCounterFetch(const Player * ptPlayer, StatCounterType eType, int32 nParam0,
                                              int32 nParam1);                     // X/Server/SGameStats.h
CRefString GBIDToString(const GBHandle & hGB, const BOOL fUsedForXMLFields);      // X/Common/Definitions/GameBalanceDefinition.h
AffixSelectionList * ActorAIGetTempAffixList();
Player * PlayerGetFirstInTieredRift();                                            // X/Common/Player.h
namespace BossEncounterManager {  // X/Server/System/BossEncounter.h
BOOL IsEncounterCompleted(SNO snoEncounter);
}
namespace SLeaderboard {  // X/Server/SLeaderboard.h
void PostRiftTime(int riftLevel, int millisecondsElapsed, int playerCount, const Player * firstPlayerInRift, bool cheated);
}                                  // X/Server/AI/ActorAI.h
void AISetCalloutCooldown(ActorCommonData & tACD, SNO snoConversation, float flCooldown, float flGlobalCooldown);  // X/Server/AI/AIConversation.h
void AISetBanterCooldown(ActorCommonData & tACD, SNO snoConversation, float flCooldown, float flGlobalCooldown);   // X/Server/AI/AIConversation.h
void AIClearCalloutCooldown(ActorCommonData & tACD, SNO snoConversation);         // X/Server/AI/AIConversation.h
BOOL SPlayerHasHeardConversation(const Player * ptPlayer, const SNO snoConv);     // X/Server/SPlayer.h
void PlayerRemovePet(Player & tPlayer, ACDID idACDPet, BOOL bDisplayChatMessage);  // X/Common/Player.h
BOOL SQuestsHasQuestItemBeenFound(GBID gbidItemRequired);                         // X/Server/Quests/SQuests.h
BOOL ACDInventoryDelete(ActorCommonData & tACDItem, BOOL bMarkItemAsDirty);       // X/Common/ActorInventory.h
CRefString NodePathGetEndNodeForActor(ActorCommonData & tACD, PathClass ePathClass);  // X/Server/NodePath.h
ACDID BrainAPIChoosePrimaryTarget(ActorCommonData & tACD, BOOL bActiveEnemiesOnly);  // X/Server/AI/BrainAPI.h
int32 ACDInventoryGetItemCount(const ActorCommonData & tACD, const GBHandle & hGBItem, const InventorySlotType * peSlots,
                               BOOL bWhiteEquippableOnly, BOOL bCheckSockets);  // X/Common/ActorInventory.h
extern const InventorySlotType cgtDefaultPlayerInventorySlots[15];               // X/Common/ActorInventory.h
extern const InventorySlotType cgtPlayerInventorySlotsPreferEquipped[15];        // X/Common/ActorInventory.h
extern const InventorySlotType cgtPlayerInventorySlotsEquippedOnly[14];          // X/Common/ActorInventory.h
extern const InventorySlotType cgtDefaultPlayerInventorySlotsIncludingStash[16]; // X/Common/ActorInventory.h
extern const InventorySlotType cgtPlayerInventorySlotsPreferEquippedIncludingStash[16];  // X/Common/ActorInventory.h
void SPlayerSwitchActiveSkills(Player & tPlayer, const SNO snoPower, const RuneType eRuneType, const int32 nSlot,
                               const BOOL bApplyCooldown);                        // X/Server/SPlayer.h
SNO SPlayerGetActiveSkillInSlot(const Player & tPlayer, const int32 nSlot);       // X/Server/SPlayer.h
void SPlayerSwitchTraits(Player & tPlayer, const SNO snoPower, const int32 nSlot, const BOOL bApplyCooldown);  // X/Server/SPlayer.h
void PlayerSetPotionBtnAssignment(Player * ptPlayer, const HotbarButtonData & tButtonData);  // X/Common/Player.h
BOOL ExperienceGrantDirectly(ActorCommonData & tACDPlayer, const uint64 nExpGranted, const BOOL bNotifyClient,
                             const BOOL bGrantToAllPlayersIfLocal);               // X/Server/Experience.h
int32 ActGetWaypointByLevelArea(const SNO snoLevelArea, BOOL bCheckActivationState);  // X/Common/Act.h
ActorIDArray & LuaScriptGetThreadSafeActorIDArray();                                   // X/Server/LuaScript/LuaScript.h
int32 MarkerGetACDsLinkedByActor(const ActorID idActor, XPooledDataIDList & listACDs, LPCSTR pszGroupName);  // X/Server/Marker/SMarker.h
int32 QuestStepGetUIDByName(SNO snoQuest, LPCSTR szStepName);  // X/Common/Quests/Quests.cpp
void SQuestsOnGameFlagSet(int32 nGameFlagIndex, BOOL bValue);  // X/Server/Quests/SQuest.cpp
int32 FlagSetLookup(SNO snoFlagSet, LPCSTR szFlagLabel);  // X/Common/Definitions/FlagSetDefinition.h
BOOL SPlayerRecordHighestSoloRiftLevel(Player * ptPlayer, int32 nRiftLevel);  // X/Server/SPlayer.cpp
uint32 SWorldGetNumDungeonFinderLevels();  // X/Server/SWorld.cpp
void SGameACDChangeWorld(ACDID idACD, SNO snoWorld, uint32 uEntranceGUID, PlayerWarpReason eReason, float flWarpFadeInSecs, const Player * ptLeadPlayer);  // X/Server/SGame.h
void AIQueueAddCallout(ActorID idActor, ACDID idACDPlayer, SNO snoConversation, int32 nPriority, float flDelay, float flMaxQueuedTime, float flCooldown, float flGlobalCooldown, uint32 bIgnoreGCD);  // X/Server/AI/AIQueue.h
BOOL SPlayerInScriptedSequence(const Player & tPlayer);  // X/Server/SPlayer.h (reference overload, SPlayer.cpp:412)
ACDPackID ACDPackNew();  // X/Common/ActorCommonData.cpp
struct PreplayedAnimData;
void BrainStanceDoPower(ActorID idActor, const Target & tTarget, SNO snoPower, EBrainPowerResult eResult, void * pPowerData, const PreplayedAnimData * ptPreplayedAnim, int32 nFlags, uint32 dwSeed);  // X/Server/AI/BrainStance.cpp
void AIStateMerge(ActorID idActor, SNO snoAIState, uint32 dwDurationTicks, ACDID idACDSource);  // X/Server/AI/AIStates.cpp
void BrainStanceSetIdleAnimation(ActorID idActor, uint32 dwIdleAnimationTag);  // X/Server/AI/BrainStance.h
void SWorldAddDungeonFinderLevel(SNO snoWorld, LPCSTR szPopulation, float flSpawnGroupDensityMultiplier, float flEliteSpawnGroupDensityMultipler, float flGizmoDensityMultiplier, SNO snoWeather);  // X/Server/SWorld.cpp

//----------------------------------------------------------------------------
static ActorIDArray * sActorsLinkedByActor(ACDID idACD, BOOL bEnable)
{
    ActorIDArray & tArrACDs = LuaScriptGetThreadSafeActorIDArray();
    tArrACDs.nNumActorIDs = 0;
    tArrACDs.bWantAddRef = TRUE;

    ActorID idActor = (ActorID)ACDGetOwner(idACD);

    XPooledDataIDList listACDs(MemoryPoolsGetDATAIDListPool());
    MarkerGetACDsLinkedByActor(idActor, listACDs, NULL);
    if (listACDs.Count())
    {
        POS pos = listACDs.GetHeadPosition();
        while (pos)
        {
            ACDID idACDScoped = (ACDID)listACDs.GetNext(pos);
            if (bEnable)
            {
                ActorCommonData * ptACD = ACDTryToGet(idACDScoped);
                if (ptACD)
                    ptACD->SetEnabled(TRUE);
            }
            tArrACDs.arActorIDs[tArrACDs.nNumActorIDs] = idACDScoped;
            if (++tArrACDs.nNumActorIDs == 64)
                break;
        }
    }
    return &tArrACDs;
}

static ActorIDArray * sActorsLinkedByActorInGroup(ACDID idACD, const char * szGroupName, BOOL bEnable)
{
    ActorIDArray & tArrACDs = LuaScriptGetThreadSafeActorIDArray();
    tArrACDs.nNumActorIDs = 0;
    tArrACDs.bWantAddRef = TRUE;

    ActorID idActor = (ActorID)ACDGetOwner(idACD);

    XPooledDataIDList listACDs(MemoryPoolsGetDATAIDListPool());
    MarkerGetACDsLinkedByActor(idActor, listACDs, szGroupName);
    if (listACDs.Count())
    {
        POS pos = listACDs.GetHeadPosition();
        while (pos)
        {
            ACDID idACDScoped = (ACDID)listACDs.GetNext(pos);
            if (bEnable)
            {
                ActorCommonData * ptACD = ACDTryToGet(idACDScoped);
                if (ptACD)
                    ptACD->SetEnabled(TRUE);
            }
            tArrACDs.arActorIDs[tArrACDs.nNumActorIDs] = idACDScoped;
            if (++tArrACDs.nNumActorIDs == 64)
                break;
        }
    }
    return &tArrACDs;
}

ActorIDArray * ActorsLinkedByActor(ACDID idACD)
{
    return sActorsLinkedByActor(idACD, FALSE);
}

ActorIDArray * ActorsEnableLinkedByActor(ACDID idActor)
{
    return sActorsLinkedByActor(idActor, TRUE);
}

ActorIDArray * ActorsLinkedByActorInGroup(ACDID idACD, const char * szGroupName)
{
    return sActorsLinkedByActorInGroup(idACD, szGroupName, FALSE);
}

ActorIDArray * ActorsEnableLinkedByActorInGroup(ACDID idACD, const char * szGroupName)
{
    return sActorsLinkedByActorInGroup(idACD, szGroupName, TRUE);
}

double ActorDistanceBetweenActors(VACDID idACDFrom, VACDID idACDTo, BOOL bWantAs2D)
{
    ActorCommonData & tACDFrom = ACDGet(idACDFrom);
    ActorCommonData & tACDTo = ACDGet(idACDTo);
    float fDist = MAX_WORLDDISTANCE;
    if (tACDFrom.IsInWorld() && tACDTo.IsInWorld() && ACDInSameWorld(tACDFrom, tACDTo))
        fDist = Math::Sqrt(ACDGetDistanceSquared(tACDFrom, tACDTo, bWantAs2D));
    return fDist;
}

double ActorDistanceSquaredBetweenActors(VACDID idACDFrom, VACDID idACDTo, BOOL bWantAs2D)
{
    ActorCommonData * ptACDFrom = ACDTryToGet(idACDFrom);
    ActorCommonData * ptACDTo = ACDTryToGet(idACDTo);
    if (ptACDFrom && ptACDTo)
    {
        const WorldLocation & tWorldLocFrom = ptACDFrom->GetWorldLocation();
        const WorldLocation & tWorldLocTo = ptACDTo->GetWorldLocation();
        if (tWorldLocFrom.idSWorld == tWorldLocTo.idSWorld)
            return ACDGetDistanceSquared(*ptACDFrom, *ptACDTo, bWantAs2D);
    }
    return MAX_WORLDDISTANCE;
}

ActorIDArray * PlayerActorsOnTeam(Team eTeam)
{
    ActorIDArray & tArrACDs = LuaScriptGetThreadSafeActorIDArray();
    tArrACDs.nNumActorIDs = 0;
    tArrACDs.bWantAddRef = TRUE;
    PlayerGlobals * ptPlayerGlobals = GameGetActiveGameCommonData().ptPlayerGlobals;
    for (int32 i = 0; i < MAX_PLAYERS; i++)
    {
        const Player & tPlayer = ptPlayerGlobals->tPlayerArray[i];
        if (tPlayer.idPlayerACD != ACDID_INVALID && tPlayer.eTeam == eTeam)
        {
            tArrACDs.arActorIDs[tArrACDs.nNumActorIDs] = tPlayer.idPlayerACD;
            if (++tArrACDs.nNumActorIDs == 64)
                break;
        }
    }
    return &tArrACDs;
}

ActorIDArray * PlayerActors()
{
    ActorIDArray & tArrACDs = LuaScriptGetThreadSafeActorIDArray();
    tArrACDs.nNumActorIDs = 0;
    tArrACDs.bWantAddRef = TRUE;
    GameCommonData & tGCD = GameGetActiveGameCommonData();
    PlayerGlobals * ptPlayerGlobals = tGCD.ptPlayerGlobals;
    for (int32 i = 0; i < MAX_PLAYERS; i++)
    {
        const Player & tPlayer = ptPlayerGlobals->tPlayerArray[i];
        if (tPlayer.idPlayerACD != ACDID_INVALID)
        {
            tArrACDs.arActorIDs[tArrACDs.nNumActorIDs] = tPlayer.idPlayerACD;
            if (++tArrACDs.nNumActorIDs == 64)
                break;
        }
    }
    return &tArrACDs;
}

ActorIDArray * PlayerActorsLocalToPlayer(VACDID idActor)
{
    ActorIDArray & tArrACDs = LuaScriptGetThreadSafeActorIDArray();
    tArrACDs.nNumActorIDs = 0;
    tArrACDs.bWantAddRef = TRUE;
    ActorCommonData * ptACD = ACDTryToGet(idActor);
    if (ptACD)
    {
        Player * ptPlayer = PlayerGetByACD(idActor);
        if (ptPlayer)
        {
            PlayerGlobals * ptPlayerGlobals = GameGetActiveGameCommonData().ptPlayerGlobals;
            for (int32 i = 0; i < MAX_PLAYERS; i++)
            {
                const Player & tPlayer = ptPlayerGlobals->tPlayerArray[i];
                if (tPlayer.idPlayerACD != ACDID_INVALID && PlayersAreLocalToEachOther(*ptPlayer, tPlayer))
                {
                    tArrACDs.arActorIDs[tArrACDs.nNumActorIDs] = tPlayer.idPlayerACD;
                    if (++tArrACDs.nNumActorIDs == 64)
                        break;
                }
            }
        }
    }
    return &tArrACDs;
}

CRefString ActorGetScene(VACDID idActor)
{
    ActorCommonData & tACD = ACDGet(idActor);
    const WorldLocation & loc = tACD.GetWorldLocation();
    SceneCommonData & tSCD = SceneCommonDataGet(loc.idSScene);
    return SNOFileGetName(SNOName(SCENE_SNO, tSCD.snoScene));
}

CRefString ActorGetWorld(VACDID idActor)
{
    ActorCommonData & tACD = ACDGet(idActor);
    WorldPlace tPlace = tACD.GetPlace();
    return SNOFileGetName(SNOName(WORLD_SNO, WorldCommonGetWorldSNO(tPlace.idSWorld)));
}

BOOL ActorsInSameWorld(VACDID idACD1, VACDID idACD2)
{
    ActorCommonData * ptACD1 = ACDTryToGet(idACD1);
    if (!ptACD1)
        return FALSE;
    ActorCommonData * ptACD2 = ACDTryToGet(idACD2);
    if (!ptACD2)
        return FALSE;
    return ACDInSameWorld(*ptACD1, *ptACD2);
}

BOOL ActorsInSameLevelArea(VACDID idACD1, VACDID idACD2)
{
    ActorCommonData * ptACD1 = ACDTryToGet(idACD1);
    if (!ptACD1)
        return FALSE;
    ActorCommonData * ptACD2 = ACDTryToGet(idACD2);
    if (!ptACD2)
        return FALSE;
    SNO snoLevelArea1 = ACDGetLevelArea(*ptACD1);
    if (snoLevelArea1 == SNO_NONE)
        return FALSE;
    return snoLevelArea1 == ACDGetLevelArea(*ptACD2);
}

// Inference: separate monster and player IsAlive tails (original tail-merges them, both rows at 953); DWARF bAlive is const 0, so TRUE/IsAlive are early returns
BOOL ActorIsAlive(ACDID idActor)
{
    BOOL bAlive = FALSE;
    {
        ActorCommonData * ptACD = ACDTryToGet(idActor);
        if (ptACD)
        {
            if (ptACD->IsMonster()) { if (FastAttribGroupIsValid(ptACD->idFastAttrib)) return ptACD->IsAlive(); }
            else if (ptACD->IsGizmo()) return TRUE;
            else if (ACDIsPlayer(idActor)) { if (FastAttribGroupIsValid(ptACD->idFastAttrib)) return ptACD->IsAlive(); }
            else return TRUE;
        }
    }
    return bAlive;
}

BOOL ActorIsObserver(ACDID idActor)
{
    ActorCommonData * ptACD = ACDTryToGet(idActor);
    if (ptACD && ptACD->IsObserver())
        return TRUE;
    return FALSE;
}

BOOL ActorIsStealthed(ACDID idActor)
{
    ActorCommonData * ptACD = ACDTryToGet(idActor);
    if (ptACD && ptACD->IsStealthed())
        return TRUE;
    return FALSE;
}

BOOL ActorIsStunned(ACDID idActor)
{
    ActorCommonData * ptACD = ACDTryToGet(idActor);
    if (ptACD && ptACD->IsStunned())
        return TRUE;
    return FALSE;
}

BOOL ActorIsHidden(ACDID idActor)
{
    ActorCommonData * ptACD = ACDTryToGet(idActor);
    if (ptACD && ptACD->IsHidden())
        return TRUE;
    return FALSE;
}

BOOL ActorIsGhost(ACDID idActor)
{
    ActorCommonData * ptACD = ACDTryToGet(idActor);
    if (ptACD && ptACD->AttributesGetInt(ATTRIBUTE_GHOSTED))
        return TRUE;
    return FALSE;
}

BOOL ActorIsElite(ACDID idActor)
{
    ActorCommonData * ptACD = ACDTryToGet(idActor);
    if (ptACD && ptACD->IsElite())
        return TRUE;
    return FALSE;
}

BOOL ActorGrantsExperience(ACDID idActor)
{
    ActorCommonData * ptACD = ACDTryToGet(idActor);
    if (ptACD && ptACD->IsMonster())
    {
        if (ptACD->AttributesGetInt(ATTRIBUTE_GRANTS_NO_XP))
            return FALSE;
        return ptACD->GetExperienceGranted() != 0;
    }
    return FALSE;
}

BOOL ActorIsLargeMonster(ACDID idActor)
{
    ActorCommonData * ptACD = ACDTryToGet(idActor);
    if (ptACD == NULL)
        return FALSE;
    return ptACD->IsLargeMonster();
}

BOOL ActorIsOnSpecial0(ACDID idActor)
{
    ActorCommonData * ptACD = ACDTryToGet(idActor);
    if (!ptACD)
        return FALSE;
    NavMeshHandle tNavMeshHandle;
    ACDGetNavMeshHandle(*ptACD, tNavMeshHandle);
    if (NavMeshHandleIsNone(tNavMeshHandle))
        return FALSE;
    return NavMeshHandleGetStaticFlag(tNavMeshHandle, CELL_STATIC_SPECIAL_0);
}

BOOL ActorIsOnSpecial1(ACDID idActor)
{
    ActorCommonData * ptACD = ACDTryToGet(idActor);
    if (!ptACD)
        return FALSE;
    NavMeshHandle tNavMeshHandle;
    ACDGetNavMeshHandle(*ptACD, tNavMeshHandle);
    if (NavMeshHandleIsNone(tNavMeshHandle))
        return FALSE;
    return NavMeshHandleGetStaticFlag(tNavMeshHandle, CELL_STATIC_SPECIAL_1);
}

BOOL ActorCanGoOnFlyerNavMesh(ACDID idActor)
{
    ActorCommonData * ptACD = ACDTryToGet(idActor);
    if (!ptACD)
        return FALSE;
    uint32 uStaticOneRequiredFlags = 0;
    {
        ScopedActorDefinitionLockR pdefActor(ptACD->snoActor);
        if (pdefActor)
            uStaticOneRequiredFlags = pdefActor->tCollData.tCollFlags.uStaticOneRequiredFlags;
    }
    return (uStaticOneRequiredFlags & (1 << CELL_STATIC_ALLOW_FLIER)) != 0;
}

BOOL ActorIsValid(ACDID idActor)
{
    if (!ACDTryToGet(idActor))
        return FALSE;
    if (ACDIsPlayer(idActor))
    {
        Player * ptPlayer = PlayerGetByACD(idActor);
        if (ptPlayer && ptPlayer->bIsBeingDeleted)
            return FALSE;
    }
    return TRUE;
}

BOOL ActorIsPet(ACDID idActor)
{
    ActorCommonData * ptACD = ACDTryToGet(idActor);
    if (ptACD && ptACD->GetPetOwnerPlayer())
        return TRUE;
    return FALSE;
}

BOOL ActorIsHireling(ACDID idActor)
{
    ActorCommonData * ptACD = ACDTryToGet(idActor);
    if (ptACD && ACDIsHireling(*ptACD))
        return TRUE;
    return FALSE;
}

BOOL ActorIsAvenger(ACDID idActor)
{
    ActorCommonData * ptACD = ACDTryToGet(idActor);
    if (ptACD && ptACD->IsAvenger())
        return TRUE;
    return FALSE;
}

BOOL ActorIsResplendentChest(ACDID idActor)
{
    ActorCommonData * ptACD = ACDTryToGet(idActor);
    if (ptACD && ptACD->IsChest() && ptACD->IsOperatableGizmo(ACDID_INVALID, FALSE))
    {
        ScopedActorDefinitionLockR pdefActor(ptACD->snoActor);
        SNO snoTreasureClass = TagMapGetSNO(pdefActor->hTagMap, TAG_ACTOR_GIZMO_TREASURE_CLASS);
        if (snoTreasureClass == GlobalSNOGet(GLOBALSNO_TREASURECLASS_RESPLENDENT_CHEST))
            return TRUE;
    }
    return FALSE;
}

BOOL ActorIsInCombat(ACDID idActor)
{
    ActorCommonData * ptACD = ACDTryToGet(idActor);
    if (ptACD && ptACD->IsInCombat())
        return TRUE;
    return FALSE;
}

BOOL ActorIsKnockbackable(ACDID idActor)
{
    ActorID idTargetActor = (ActorID)ACDGetOwner(idActor);
    ActorCommonData * ptACDTarget = ACDTryToGet(idActor);
    if (idTargetActor == ACTORID_INVALID || !ptACDTarget)
        return FALSE;
    if (PowerActorCanBeKnockedBack(idTargetActor, -1))
        return !ptACDTarget->AttributesGetInt(ATTRIBUTE_IMMUNE_TO_KNOCKBACK);
    return FALSE;
}

BOOL ActorDropsNothing(ACDID idActor)
{
    ActorCommonData * ptACD = ACDTryToGet(idActor);
    if (ptACD)
    {
        SNO snoTreasureClass = ACDGetTreasureClass(*ptACD, ACDID_INVALID);
        if (snoTreasureClass != GlobalSNOGet(GLOBALSNO_TREASURECLASS_DROPNOTHING))
            return ptACD->AttributesGetInt(ATTRIBUTE_DROPS_NO_LOOT) != 0;
    }
    return TRUE;
}

void ActorPlaySound(ACDID idActor, LPCSTR szSoundName)
{
    ActorCommonData & tACD = ACDGet(idActor);
    SNO snoSound = SNOFileLookupGUID(SOUND_SNO, szSoundName);
    LuaVerifySnosAreReferenced(snoSound, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE);
    PlayEffectMessage tMessage;
    tMessage.ann = ANNFromACD(tACD.id);
    tMessage.eEffectType = EFFECT_PLAY_SOUND;
    tMessage.nEffectParameter.Set(snoSound);
    KnownMessageAdd(tACD, SMSG_PLAY_EFFECT, &tMessage, sizeof(tMessage), NULL);
}

void ActorPlaySink(ACDID idActor, float flTimeToSink)
{
    ActorCommonData & tACD = ACDGet(idActor);
    int32 nTicksToSink = Math::FloatRoundToInt(flTimeToSink * 60.0f);
    PlayEffectMessage tMessage;
    tMessage.ann = ANNFromACD(tACD.id);
    tMessage.eEffectType = EFFECT_SINK;
    tMessage.nEffectParameter.Set(nTicksToSink);
    KnownMessageAdd(tACD, SMSG_PLAY_EFFECT, &tMessage, sizeof(tMessage), NULL);
}

int32 ActorPlayEffectGroup(VACDID idActor, LPCSTR szEffectGroupName, BOOL bUseComplexEffect, ACDID idDestinationPlayer)
{
    SNO snoEffectGroup = SNOFileLookupGUID(EFFECTGROUP_SNO, szEffectGroupName);
    BOOL bExists = snoEffectGroup != SNO_NONE;
    if (bExists)
    {
        if (bUseComplexEffect)
            return SComplexEffectAdd(ACDGet(idActor), snoEffectGroup, ACDID_INVALID, COMPLEXEFFECT_TYPE_NORMAL, 0, 0, FALSE);
        if (idDestinationPlayer != ACDID_INVALID)
            ACDPlayEffectForPlayer(idDestinationPlayer, idActor, EFFECT_PLAY_EFFECT_GROUP, snoEffectGroup);
        else
            ACDPlayEffect(idActor, EFFECT_PLAY_EFFECT_GROUP, snoEffectGroup, idActor);
    }
    return -1;
}

void ClientPlayEffectGroup(ACDID idActor, ACDID idClient, LPCSTR szEffectGroupName)
{
    SNO snoEffectGroup = SNOFileLookupGUID(EFFECTGROUP_SNO, szEffectGroupName);
    BOOL bExists = snoEffectGroup != SNO_NONE;
    ActorCommonData * ptACDTarget = ACDTryToGet(idActor);
    if (!ptACDTarget)
        return;
    ActorCommonData * ptACDClient = ACDTryToGet(idClient);
    if (!ptACDClient)
        return;
    Player * ptPlayer = PlayerGetByACD(idClient);
    if (bExists && ptPlayer)
    {
        FastAttribKey tKey = MakeAttribKey(ATTRIBUTE_CLIENT_EFFECT, ptPlayer->nPlayerIndex);
        ptACDTarget->AttributesSetInt(tKey, snoEffectGroup);
    }
}

void ClientStopEffectGroup(ACDID idActor, ACDID idClient, LPCSTR szEffectGroupName)
{
    SNO snoEffectGroup = SNOFileLookupGUID(EFFECTGROUP_SNO, szEffectGroupName);
    BOOL bExists = snoEffectGroup != SNO_NONE;
    ActorCommonData * ptACDTarget = ACDTryToGet(idActor);
    if (!ptACDTarget)
        return;
    ActorCommonData * ptACDClient = ACDTryToGet(idClient);
    if (!ptACDClient)
        return;
    Player * ptPlayer = PlayerGetByACD(idClient);
    if (bExists && ptPlayer)
    {
        FastAttribKey tKey = MakeAttribKey(ATTRIBUTE_CLIENT_EFFECT, ptPlayer->nPlayerIndex);
        ptACDTarget->AttributesSetInt(tKey, 0);
    }
}

BOOL ActorPlayResourceGainedEffectGroup(VACDID idActor, ResourceType eResourceType)
{
    ActorCommonData * ptACDTarget = ACDTryToGet(idActor);
    if (!ptACDTarget || !ptACDTarget->IsAlive() || (uint32)eResourceType >= RESOURCE_TYPE_COUNT)
        return FALSE;
    const uint32 RESOURCE_MINIMUM_EFFECT_INTERVAL = 60;
    uint32 dwLastEffectTick = ptACDTarget->AttributesGetInt(MakeAttribKey(ATTRIBUTE_RESOURCE_EFFECT_LAST_PLAYED_TICK, eResourceType));
    uint32 dwNow = GameGetServerTime();
    if (dwNow - dwLastEffectTick > RESOURCE_MINIMUM_EFFECT_INTERVAL)
    {
        SNO snoResourceGainedEffect = SNO_NONE;
        switch (eResourceType)
        {
        case RESOURCE_TYPE_MANA:
            snoResourceGainedEffect = GlobalSNOGet(GLOBALSNO_EFFECTGROUP_ACTOR_GAINED_MANA);
            break;
        case RESOURCE_TYPE_ARCANUM:
            snoResourceGainedEffect = GlobalSNOGet(GLOBALSNO_EFFECTGROUP_ACTOR_GAINED_ARCANUM);
            break;
        case RESOURCE_TYPE_FURY:
            snoResourceGainedEffect = GlobalSNOGet(GLOBALSNO_EFFECTGROUP_ACTOR_GAINED_FURY);
            break;
        case RESOURCE_TYPE_SPIRIT:
            snoResourceGainedEffect = GlobalSNOGet(GLOBALSNO_EFFECTGROUP_ACTOR_GAINED_SPIRIT);
            break;
        case RESOURCE_TYPE_HATRED:
            snoResourceGainedEffect = GlobalSNOGet(GLOBALSNO_EFFECTGROUP_ACTOR_GAINED_HATRED);
            break;
        case RESOURCE_TYPE_DISCIPLINE:
            snoResourceGainedEffect = GlobalSNOGet(GLOBALSNO_EFFECTGROUP_ACTOR_GAINED_DISCIPLINE);
            break;
        case RESOURCE_TYPE_FAITH:
            snoResourceGainedEffect = GlobalSNOGet(GLOBALSNO_EFFECTGROUP_ACTOR_GAINED_FAITH);
            break;
        case RESOURCE_TYPE_ESSENCE:
            snoResourceGainedEffect = GlobalSNOGet(GLOBALSNO_EFFECTGROUP_ACTOR_GAINED_ESSENCE);
            break;
        default:
            break;
        }
        if (snoResourceGainedEffect != SNO_NONE)
        {
            ACDPlayEffect(ptACDTarget->id, EFFECT_PLAY_EFFECT_GROUP, snoResourceGainedEffect, ptACDTarget->id);
            ptACDTarget->AttributesSetInt(MakeAttribKey(ATTRIBUTE_RESOURCE_EFFECT_LAST_PLAYED_TICK, eResourceType), dwNow);
            return TRUE;
        }
    }
    return FALSE;
}

int32 ActorPlayEffectGroupActorToActor(VACDID idActorFrom, VACDID idActorTo, LPCSTR szEffectGroupName, BOOL bUseComplexEffect)
{
    SNO snoEffectGroup = SNOFileLookupGUID(EFFECTGROUP_SNO, szEffectGroupName);
    BOOL bExists = snoEffectGroup != SNO_NONE;
    if (bExists)
    {
        if (bUseComplexEffect)
            return SComplexEffectAdd(ACDGet(idActorFrom), snoEffectGroup, idActorTo, COMPLEXEFFECT_TYPE_NORMAL, 0, 0, FALSE);
        ACDPlayEffectGroupACDToACD(snoEffectGroup, idActorFrom, idActorTo);
    }
    return -1;
}

int32 ActorPlayEffectGroupRopeActorToActor(VACDID idActorFrom, RopePosition ePositionFrom, VACDID idActorTo, RopePosition ePositionTo, LPCSTR szRopeName, BOOL bUseComplexEffect, BOOL bIgnoreOwnerAlpha)
{
    if (idActorFrom == idActorTo)
        return -1;
    SNO snoRope = SNOFileLookupGUID(ROPE_SNO, szRopeName);
    BOOL bExists = snoRope != SNO_NONE;
    if (bExists)
    {
        if (bUseComplexEffect)
            return SComplexEffectAdd(ACDGet(idActorFrom), snoRope, idActorTo, COMPLEXEFFECT_TYPE_SPECIAL_ROPE, ePositionFrom, ePositionTo, bIgnoreOwnerAlpha);
        ACDPlayEffectRopeACDToACD(snoRope, idActorFrom, ePositionFrom, idActorTo, ePositionTo, bIgnoreOwnerAlpha);
    }
    return -1;
}

void ActorRemoveComplexEffect(VACDID idActor, int32 idComplexEffect)
{
    SComplexEffectRemove(idActor, idComplexEffect);
}

BOOL ActorComplexEffectIsValid(VACDID idActor, int32 idComplexEffect)
{
    return SComplexEffectIsValid(idActor, idComplexEffect);
}

void ActorEnableAI(ACDID idACDActor, BOOL bValue)
{
    ActorID idActor = (ActorID)ACDGetOwner(idACDActor);
    if (idActor != ACTORID_INVALID && ActorTryToGetAI(idActor))
    {
        if (!bValue && !ActorAIIsStatusFlagSet(idActor, ACTOR_AI_DISABLE_UPDATES_BIT))
            BrainAPIDoIdle(idActor);
        ActorAISetStatusFlag(idActor, ACTOR_AI_DISABLE_UPDATES_BIT, !bValue);
    }
}

BOOL ActorIsAIEnabled(ACDID idACDActor)
{
    ActorID idActor = (ActorID)ACDGetOwner(idACDActor);
    if (idActor != ACTORID_INVALID && ActorTryToGetAI(idActor))
        return !ActorAIIsStatusFlagSet(idActor, ACTOR_AI_DISABLE_UPDATES_BIT);
    return FALSE;
}

void ActorSetCannotBeTargetedByAI(ACDID idACDActor, BOOL bValue)
{
    ActorCommonData * ptACD = ACDTryToGet(idACDActor);
    if (ptACD)
        ptACD->SetCannotBeAddedToAITargetList(bValue);
}

void ActorSpecialDeathEnable(ACDID idACDActor, BOOL bValue)
{
    ActorCommonData * ptACD = ACDTryToGet(idACDActor);
    if (ptACD)
        ptACD->AttributesSetInt(ATTRIBUTE_HAS_SPECIAL_DEATH_ANIMTAG, bValue);
}

void ActorKill(ACDID idACD, DeathType eDeathType, BOOL bCrit, BOOL bDropLoot, ACDID idACDKiller)
{
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (eDeathType >= 0 && eDeathType < NUM_DEATH_TYPES && ptACD && !ACDIsPlayer(idACD) && ptACD->IsAlive())
    {
        ActorID idKiller = (ActorID)ACDGetOwner(idACDKiller);
        BrainAPIDoScriptedDeath(*ptACD, eDeathType, bCrit, bDropLoot, idKiller);
        LuaScriptInstanceRemoveRefACD(LuaScriptGetExecutingScriptInstance(), idACD);
    }
}

BOOL ActorResurrect(VACDID idCorpse, VACDID idSource)
{
    ActorCommonData * ptACDSource = ACDTryToGet(idSource);
    ActorCommonData * ptACDTarget = ACDTryToGet(idCorpse);
    if (!ptACDSource || !ptACDTarget)
        return FALSE;
    if (ptACDSource->GetPlace().idSWorld != ptACDTarget->GetPlace().idSWorld)
        return FALSE;
    ActorID idTarget = (ActorID)ptACDTarget->idOwner;
    if (!PowerActorCanResurrect(idTarget))
        return FALSE;
    BrainAPIDoResurrect(idTarget, FALSE);
    BrainAPIDoPlayAnimation(idTarget, TAG_ACTOR_ANIMATION_RESURRECTION, (uint32)-1, FALSE, FALSE, 0.0f, FALSE, 1.0f);
    ptACDTarget->AttributesSetInt(ATTRIBUTE_DROPS_NO_LOOT, TRUE);
    ptACDTarget->AttributesSetInt(ATTRIBUTE_GRANTS_NO_XP, TRUE);
    return TRUE;
}

// One early-return condition: listDestPlayers is at function scope (no lexical block); operand rows 2058 and 2069 (ccmp)
void FadeOut(VACDID idACD, int32 nFadeTime)
{
    ActorCommonData * ptACDActor = ACDTryToGet(idACD);
    if (nFadeTime <= 0 || ptACDActor == NULL || nFadeTime >= MAX_FADE_TIME) return;

    ptACDActor->AttributesSetInt(ATTRIBUTE_DELETED_ON_SERVER, TRUE);
    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetAllInGame(listDestPlayers, -1, FALSE, -1);
    ptACDActor->AttributesSetInt(ATTRIBUTE_SCRIPTED_FADE_TIME, nFadeTime);
    ACDCollDisposeSphereNode(*ptACDActor);
    listDestPlayers.RemoveAll();
    PlayerListGetThatKnowACD(listDestPlayers, idACD, FALSE);
    ANNDataMessage tMessage;
    tMessage.ann = ANNFromACD(idACD);
    MessageSendToClient(listDestPlayers, SMSG_REMOVE_FROM_SPHERE_LIST, &tMessage, sizeof(tMessage));

}

BOOL ActorPlayConversation(LPCSTR szConversation, VACDID idACDPlayer, VACDID idACDPrimary, VACDID idACDAlt1, VACDID idACDAlt2, VACDID idACDAlt3, VACDID idACDAlt4, BOOL bForceInterrupt)
{
    SNO snoConversation = SNOFileLookupGUID(CONVERSATION_SNO, szConversation);
    if (snoConversation == SNO_NONE)
        return FALSE;

    int32 nPlayerTalkingIndex = -1;
    LuaVerifySnosAreReferenced(snoConversation, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE);
    Player * ptPlayer = PlayerGetByACD(idACDPlayer);
    if (ptPlayer)
        nPlayerTalkingIndex = ptPlayer->nPlayerIndex;
    ConversationMgr::StartConversation(nPlayerTalkingIndex, snoConversation, idACDPrimary, idACDAlt1, idACDAlt2, idACDAlt3, idACDAlt4, 0, bForceInterrupt, FALSE);
    return TRUE;
}

BOOL ActorPlaySimpleConversation(LPCSTR szConversation, VACDID idACDPrimary, VACDID idACDAlt1, VACDID idACDAlt2, VACDID idACDAlt3, VACDID idACDAlt4, BOOL bForceInterrupt)
{
    return ActorPlayConversation(szConversation, ACDID_INVALID, idACDPrimary, idACDAlt1, idACDAlt2, idACDAlt3, idACDAlt4, bForceInterrupt);
}

void ActorPlayCallout(LPCSTR szConversation, VACDID idACDPlayer, VACDID idACDNPC, int32 nPriority, float flDelay, float flMaxQueuedTime, float flCooldown, float flGlobalCooldown, BOOL bIgnoreGCD)
{
    SNO snoConversation = SNOFileLookupGUID(CONVERSATION_SNO, szConversation);
    if (snoConversation == SNO_NONE)
        return;

    Player * ptPlayer = PlayerGetByACD(idACDPlayer);
    if (!ptPlayer || SPlayerInScriptedSequence(*ptPlayer))  // original calls _Z25SPlayerInScriptedSequenceRK6Player (0x874b44)
        return;

    AIQueueAddCallout((ActorID)ACDGet(idACDNPC).idOwner, idACDPlayer, snoConversation, nPriority, flDelay, flMaxQueuedTime, flCooldown, flGlobalCooldown, bIgnoreGCD != FALSE);
}

void ActorSilenceCallouts(VACDID idACDNPC, float flDuration)
{
    ActorCommonData & tACD = ACDGet(idACDNPC);
    AISetCalloutCooldown(tACD, SNO_NONE, 0.0f, flDuration);
    if (AppDrawFlagIsSet(APP_DRAW_DEBUG_CALLOUTS_BIT))
    {
    }
}

void ActorSilenceBanter(VACDID idACDPlayer, float flDuration)
{
    ActorCommonData & tACD = ACDGet(idACDPlayer);
    AISetBanterCooldown(tACD, SNO_NONE, 0.0f, flDuration);
    if (AppDrawFlagIsSet(APP_DRAW_DEBUG_CALLOUTS_BIT))
    {
    }
}

void ActorSetCalloutCooldown(VACDID idACDNPC, LPCSTR szConversation, float flCooldown)
{
    SNO snoConv = SNOFileLookupGUID(CONVERSATION_SNO, szConversation);
    if (snoConv != SNO_NONE)
    {
        LuaVerifySnosAreReferenced(snoConv, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE);
        ActorCommonData & tACD = ACDGet(idACDNPC);
        AISetCalloutCooldown(tACD, snoConv, flCooldown, 0.0f);
    }
}

void ActorClearCalloutCooldown(VACDID idACDNPC, LPCSTR szConversation)
{
    SNO snoConv = SNOFileLookupGUID(CONVERSATION_SNO, szConversation);
    if (snoConv != SNO_NONE)
    {
        LuaVerifySnosAreReferenced(snoConv, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE);
        ActorCommonData & tACD = ACDGet(idACDNPC);
        AIClearCalloutCooldown(tACD, snoConv);
    }
}

BOOL PlayerHasUnlockedConversation(ACDID idACDPlayer, LPCSTR szConversation)
{
    Player * ptPlayer = PlayerGetByACD(idACDPlayer);
    if (!ptPlayer)
        return FALSE;
    SNO snoConv = SNOFileLookupGUID(CONVERSATION_SNO, szConversation);
    if (snoConv == SNO_NONE)
        return FALSE;
    LuaVerifySnosAreReferenced(snoConv, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE);
    return ptPlayer->tHeroStateData.tSavedData.tSavedConversations.IsUnlocked(snoConv);
}

void PlayerUnlockConversation(ACDID idACDPlayer, LPCSTR szConversation)
{
    Player * ptPlayer = PlayerGetByACD(idACDPlayer);
    if (!ptPlayer)
        return;
    SNO snoConv = SNOFileLookupGUID(CONVERSATION_SNO, szConversation);
    if (snoConv == SNO_NONE)
        return;
    LuaVerifySnosAreReferenced(snoConv, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE);
    ptPlayer->tHeroStateData.tSavedData.tSavedConversations.Save(snoConv, FALSE);
    ptPlayer->uConsoleSyncFlags |= (1 << PCSF_SAVED_CONVERSATIONS);
}

void PlayerSaveConversation(ACDID idACDPlayer, LPCSTR szConversation)
{
    Player * ptPlayer = PlayerGetByACD(idACDPlayer);
    if (!ptPlayer)
        return;
    SNO snoConv = SNOFileLookupGUID(CONVERSATION_SNO, szConversation);
    if (snoConv == SNO_NONE)
        return;
    LuaVerifySnosAreReferenced(snoConv, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE);
    ptPlayer->tHeroStateData.tSavedData.tSavedConversations.Save(snoConv, TRUE);
    ptPlayer->uConsoleSyncFlags |= (1 << PCSF_SAVED_CONVERSATIONS);
}

BOOL PlayerHasHeardConversation(ACDID idACDPlayer, LPCSTR szConversation)
{
    Player * ptPlayer = PlayerGetByACD(idACDPlayer);
    if (!ptPlayer)
        return FALSE;
    SNO snoConv = SNOFileLookupGUID(CONVERSATION_SNO, szConversation);
    if (snoConv == SNO_NONE)
        return FALSE;
    LuaVerifySnosAreReferenced(snoConv, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE);
    return SPlayerHasHeardConversation(ptPlayer, snoConv);
}

void PlayerConversationSetHeard(LPCSTR szConversation, BOOL bHeard, ACDID idACDPlayer)
{
    SNO snoConv = SNOFileLookupGUID(CONVERSATION_SNO, szConversation);
    if (snoConv == SNO_NONE)
        return;
    LuaVerifySnosAreReferenced(snoConv, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE);
    Player * ptPlayer = PlayerGetByACD(idACDPlayer);
    if (ptPlayer)
    {
        if (bHeard)
            SPlayerMarkConversationAsHeard(ptPlayer, snoConv);
        else
            SPlayerClearHeardConversation(*ptPlayer, snoConv);
    }
    else
    {
        for (ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
        {
            if (bHeard)
                SPlayerMarkConversationAsHeard(ptPlayer, snoConv);
            else
                SPlayerClearHeardConversation(*ptPlayer, snoConv);
        }
    }
}

void ActorSetNoDamage(ACDID idACD, BOOL bValue)
{
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD)
        ptACD->SetTakesNoDamage(bValue);
}

static void sAttribAddBuff(SNO snoBuff, ACDID idACD, Attrib eAttrib)
{
    PowerAPIBuffStartData tBuffStartData(POWERID_INVALID);
    tBuffStartData.bInfinite = true;
    tBuffStartData.snoPower = snoBuff; // store order from the line table: 2779 snoPower, 2780 eStackType
    tBuffStartData.eStackType = BUFF_STACKTYPE_ALWAYS;
    FastAttribSetValueInt(tBuffStartData.idFastAttrib, eAttrib, 1);
    PowerAPI_BuffAddToACD(tBuffStartData, ACDGet(idACD));
}

static void sAttribRemoveBuff(SNO snoBuff, ACDID idACD)
{
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD)
    {
        BuffID idBuff = PowerAPI_BuffFindOnACD(snoBuff, ptACD, ACDID_INVALID);
        if (idBuff != BUFFID_INVALID)
            PowerAPI_BuffFlagForDeletion(idBuff, FALSE, TRUE, BUFF_REMOVAL_REASON_DEFAULT, TRUE);
    }
}

void ActorClearBuffsFlaggedForGreaterRiftEntry(ACDID idActor)
{
    ActorCommonData * ptACD = ACDTryToGet(idActor);
    if (ptACD)
    {
        POS pos = ptACD->listBuffs.GetHeadPosition();
        while (pos)
        {
            BuffID idBuff = ptACD->listBuffs.GetNext(pos);
            PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
            if (ptBuff && !ptBuff->bDeleted && PowerIsPassive(ptBuff->snoPower) && ptBuff->bRemoveOnGreaterRiftEntry)
                PowerAPI_BuffFlagForDeletion(ptBuff, FALSE, TRUE, 0, TRUE);
        }
    }
}

void ActorSetInvulnerable(VACDID idACD, BOOL bValue)
{
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD)
        ptACD->SetInvulnerable(bValue);
}


void ActorSetInvisible(VACDID idACD, BOOL bValue)
{
    SNO snoPower = GlobalSNOGet(GLOBALSNO_POWERS_INVISIBLE_BUFF);
    if (bValue)
        sAttribAddBuff(snoPower, idACD, ATTRIBUTE_HIDDEN);
    else
        sAttribRemoveBuff(snoPower, idACD);
}

void ActorSetOperable(ACDID idActor, BOOL bValue)
{
    ActorCommonData * ptACD = ACDTryToGet(idActor);
    if (ptACD)
    {
        ptACD->AttributesSetInt(ATTRIBUTE_NPC_DISABLE_OPERATION, !bValue);
        for (const Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
            NPCUpdateConversationIconAndInteraction(*ptACD, *ptPlayer);
    }
}


void ActorSetTargetable(VACDID idACD, BOOL bValue)
{
    SNO snoPower = GlobalSNOGet(GLOBALSNO_POWERS_UNTARGETABLE_BUFF);
    if (!bValue)
        sAttribAddBuff(snoPower, idACD, ATTRIBUTE_UNTARGETABLE);
    else
        sAttribRemoveBuff(snoPower, idACD);
}

void ActorSetCollidable(VACDID idACD, BOOL bValue)
{
    ACDGet(idACD).EnableCollision(bValue);
}

void ActorSetDamageState(VACDID idACD, int32 nValue)
{
    ACDGet(idACD).ChangeDamageState(nValue);
}

void ActorIncrementDamageState(VACDID idACD)
{
    ACDGet(idACD).IncDamageState();
}

void ActorRemoveStaggeredDamage(VACDID idActor)
{
    Actor * ptActor = ActorTryToGet((ActorID)ACDGetOwner(idActor));
    if (ptActor)
        ActorClearStaggeredDamage(ptActor->id);
}

ACDID ActorGetClosestPlayer(VACDID idACD, float flRangeSq)
{
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (!ptACD || !ptACD->IsInWorld())
        return ACDID_INVALID;

    const WorldLocation & tWorldLoc = ptACD->GetWorldLocation();
    ACDID idACDClosest = ACDID_INVALID;
    flRangeSq = flRangeSq > EPSILON ? flRangeSq : MAX_WORLDDISTANCE;

    for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {
        ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
        if (ptACDPlayer && ptACDPlayer->IsInWorld())
        {
            const WorldLocation & tPlayerLoc = ACDGet(ptPlayer->idPlayerACD).GetWorldLocation();
            if (tPlayerLoc.idSWorld == tWorldLoc.idSWorld)
            {
                float flDistSq = Vector3D::DistanceSquared(tWorldLoc.transform.wp, tPlayerLoc.transform.wp);
                if (flDistSq < flRangeSq)
                {
                    idACDClosest = ptPlayer->idPlayerACD;
                    flRangeSq = flDistSq;
                }
            }
        }
    }
    return idACDClosest;
}

ACDID ActorGetNearbyFollower(VACDID idACD, float flRangeSq)
{
    return SHirelingGetNearby(idACD, flRangeSq, 0);
}

ACDID ActorMakeHireling(VACDID idPlayer, VACDID idActorPlace, HirelingClass eHirelingClass, BOOL bShowPrompt)
{
    ActorCommonData & tACDPlayer = ACDGet(idPlayer);
    ActorCommonData & tACDActorPlace = ACDGet(idActorPlace);
    ACDID idACDHireling = SHirelingCreate(tACDPlayer, tACDActorPlace, eHirelingClass, !bShowPrompt, HT_NORMAL);
    if (idACDHireling != ACDID_INVALID)
        LuaScriptInstanceAddRefACD(LuaScriptGetExecutingScriptInstance(), idACDHireling);
    return idACDHireling;
}

ACDID ActorGetPetOwner(VACDID idACDPet)
{
    return ACDGetPetOwner(idACDPet);
}

void ActorRemoveFollowerPet(VACDID idACDPet)
{
    ActorCommonData & tACDPet = ACDGet(idACDPet);
    Player * ptPlayer = tACDPet.GetPetOwnerPlayer();
    if (ptPlayer)
        PlayerRemovePet(*ptPlayer, idACDPet, TRUE);
    AIStateSetStateData((ActorID)tACDPet.idOwner, AIBEHAVIOR_TYPE_NORMAL);
}

Team ActorGetTeam(ACDID idActor)
{
    return ACDGet(idActor).GetTeam();
}

BOOL ActorIsPlayerTeam(ACDID idACDActor)
{
    ActorCommonData * ptACD = ACDTryToGet(idACDActor);
    if (ptACD == NULL)
        return FALSE;
    return ptACD->IsPlayerTeam();
}

BOOL ActorIsMonsterTeam(ACDID idACDActor)
{
    ActorCommonData * ptACD = ACDTryToGet(idACDActor);
    if (ptACD == NULL)
        return FALSE;
    return ptACD->IsMonsterTeam();
}

BOOL ActorsAreFriendly(VACDID idActor1, VACDID idActor2, BOOL bCharmedAlliesAreEnemies)
{
    const ActorCommonData & tACD1 = ACDGet(idActor1);
    const ActorCommonData & tACD2 = ACDGet(idActor2);
    return ACDIsFriendly(tACD1, tACD2, bCharmedAlliesAreEnemies);
}

SNO ActorGetSNO(ACDID idACD)
{
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD == NULL)
        return SNO_NONE;
    return ptACD->GetActorSNO();
}

CRefString ActorGetSNOName(ACDID idACD)
{
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (!ptACD)
        return "UNKNOWN";
    return SNOToString(ACTOR_SNO, ptACD->GetActorSNO(), FALSE);
}

static ACDID sActorGetItemByName(VACDID idACDOwner, LPCSTR szItemName, BOOL bPreferEquipped, BOOL bAllowStash,
                                 BOOL bEquippedOnly)
{
    GBID gbidItem = StringToGBID(szItemName);
    int32 nItemCount;
    ACDID idACDItem;
    GBHandle hGBItem(GB_ITEMS, gbidItem);

    ActorCommonData & tACDOwner = ACDGet(idACDOwner);
    if (bEquippedOnly)
    {
        ACDInventoryGetItem(tACDOwner, hGBItem, cgtPlayerInventorySlotsEquippedOnly, idACDItem, nItemCount, FALSE);
    }
    else if (bPreferEquipped)
    {
        if (bAllowStash)
        {
            ACDInventoryGetItem(tACDOwner, hGBItem, cgtPlayerInventorySlotsPreferEquippedIncludingStash, idACDItem,
                                nItemCount, FALSE);
        }
        else
        {
            ACDInventoryGetItem(tACDOwner, hGBItem, cgtPlayerInventorySlotsPreferEquipped, idACDItem, nItemCount,
                                FALSE);
        }
    }
    else
    {
        if (bAllowStash)
        {
            ACDInventoryGetItem(tACDOwner, hGBItem, cgtDefaultPlayerInventorySlotsIncludingStash, idACDItem,
                                nItemCount, FALSE);
        }
        else
        {
            ACDInventoryGetItem(tACDOwner, hGBItem, cgtDefaultPlayerInventorySlots, idACDItem, nItemCount, FALSE);
        }
    }

    if (idACDItem != ACDID_INVALID)
    {
        LuaScriptInstanceAddRefACD(LuaScriptGetExecutingScriptInstance(), idACDItem);
    }
    return idACDItem;
}

BOOL ActorHasItemByName(VACDID idACDOwner, LPCSTR szItemName, BOOL bAllowStash, BOOL bEquippedOnly)
{
    GBID gbidItem = StringToGBID(szItemName);
    if (ItemHasLabel(gbidItem, ITL_QUEST))
        return SQuestsHasQuestItemBeenFound(gbidItem);

    ACDID idACDItem = sActorGetItemByName(idACDOwner, szItemName, FALSE, bAllowStash, bEquippedOnly);
    return idACDItem != ACDID_INVALID;
}

void ActorRemoveItemByName(ACDID idActor, LPCSTR szItemName, BOOL bPreferEquipped)
{
    ACDID idACDItem = sActorGetItemByName(idActor, szItemName, bPreferEquipped, FALSE, FALSE);
    ActorCommonData * ptACDItem = ACDTryToGet(idACDItem);
    if (ptACDItem)
        ACDInventoryDelete(*ptACDItem, TRUE);
}

void ActorGizmoOperate(ACDID idACDGizmo, ACDID idACDOperator)
{
    ActorCommonData & tACDGizmo = ACDGet(idACDGizmo);
    ActorCommonData * ptACDOperator = ACDTryToGet(idACDOperator);
    if (tACDGizmo.GetGizmoType() == ACTORGIZMO_BREAKABLEDOOR)
        return;
    if (GizmoRequiresOperator(tACDGizmo.GetGizmoType()) && !ptACDOperator)
        return;
    GizmoOperate(tACDGizmo, ptACDOperator, TRUE, FALSE);
}

AIStateType ActorGetAIState(VACDID idACDActor)
{
    return AIStateGetCurrState((ActorID)ACDGetOwner(idACDActor));
}

void ActorSetAIBehavior(VACDID idACDActor, AIBehaviorType eBehavior)
{
    if (eBehavior < 0 || eBehavior >= AIBEHAVIOR_TYPE_COUNT)
        return;
    AIStateSetStateData((ActorID)ACDGetOwner(idACDActor), eBehavior);
}

void ActorAddAITarget(VACDID idACDActor, VACDID idACDTarget, BOOL bForcedTarget)
{
    BrainAPIActorAddTarget((ActorID)ACDGetOwner(idACDActor), (ActorID)ACDGetOwner(idACDTarget), FALSE, FALSE, 0, bForcedTarget);
}

void ActorSetAITether(VACDID idACDActor, float flRadius, ACDID idACDTether)
{
    ActorCommonData & tACD = ACDGet(idACDActor);
    ActorID idActor = (ActorID)tACD.idOwner;
    ActorCommonData * ptACDTether = ACDTryToGet(idACDTether);
    if (ptACDTether)
        AISetTether(idActor, TETHER_ALWAYS, flRadius, ptACDTether->GetPlace());
    else
        AISetTether(idActor, TETHER_ALWAYS, flRadius, tACD.GetPlace());
}

ACDID ActorGetAIPrimaryTarget(VACDID idACDActor, BOOL bActiveEnemiesOnly)
{
    ActorCommonData * ptACDActor = ACDTryToGet(idACDActor);
    if (!ptACDActor)
        return ACDID_INVALID;
    return BrainAPIChoosePrimaryTarget(*ptACDActor, bActiveEnemiesOnly);
}

void ActorClearAITether(VACDID idACDActor)
{
    AIClearTether((ActorID)ACDGetOwner(idACDActor));
}

void ActorSetIdleAnim(VACDID idACD, ActorAnimationTag eAnimTag)
{
    if (eAnimTag == 0 || eAnimTag == -1)
        return;

    ConversationMgr::UpdateIdleForActor(idACD, eAnimTag);
    BrainStanceSetIdleAnimation((ActorID)ACDGetOwner(idACD), eAnimTag);
}

void ActorSetCanActivate(LPCSTR szMarkerName)
{
    InactiveMarkerSetCanActivate(MarkerStringHash(szMarkerName), TRUE);
}

void ActorEnable(VACDID idActor)
{
    ActorCommonData * ptACD = ACDTryToGet(idActor);
    if (ptACD)
        ptACD->SetEnabled(TRUE);
}

void ActorDisable(VACDID idActor)
{
    ActorCommonData * ptACD = ACDTryToGet(idActor);
    if (ptACD)
        ptACD->SetEnabled(FALSE);
}

BOOL ActorIsEnabled(VACDID idActor)
{
    ActorCommonData * ptACD = ACDTryToGet(idActor);
    if (ptACD == NULL)
        return FALSE;
    return ptACD->IsEnabled();
}

PathClass ActorGetNodePathClass(VACDID idActor)
{
    Actor * ptActor = ActorTryToGet((ActorID)ACDGetOwner(idActor));
    if (ptActor == NULL || ptActor->tMarkerOwner.snoMarkerSet == SNO_NONE)
        return (PathClass)-1;

    ScopedMarkerSetDefinitionLockR ptMarkerSet(ptActor->tMarkerOwner.snoMarkerSet);
    if (ptActor->tMarkerOwner.nMarkerIndex < ptMarkerSet->tMarkerSet.GetArraySize())
    {
        const Marker & tMarker = ptMarkerSet->tMarkerSet[ptActor->tMarkerOwner.nMarkerIndex];
        return (PathClass)TagMapGetInt(tMarker.hTagMap, TAG_MARKER_PATH_ID);
    }
    return (PathClass)-1;
}

int32 ActorGetControlPointIndex(VACDID idActor)
{
    Actor * ptActor = ActorTryToGet((ActorID)ACDGetOwner(idActor));
    if (ptActor == NULL || ptActor->tMarkerOwner.snoMarkerSet == SNO_NONE)
        return -1;

    ScopedMarkerSetDefinitionLockR ptMarkerSet(ptActor->tMarkerOwner.snoMarkerSet);
    if (ptActor->tMarkerOwner.nMarkerIndex < ptMarkerSet->tMarkerSet.GetArraySize())
    {
        const Marker & tMarker = ptMarkerSet->tMarkerSet[ptActor->tMarkerOwner.nMarkerIndex];
        return TagMapGetInt(tMarker.hTagMap, TAG_MARKER_CONTROLPOINT_INDEX);
    }
    return -1;
}

CRefString ActorGetNodePathEndNodeName(VACDID idActor, PathClass ePathClass)
{
    ActorCommonData & tACD = ACDGet(idActor);
    return NodePathGetEndNodeForActor(tACD, ePathClass);
}

void ActorSetConversationList(VACDID idACD, LPCSTR szConvList)
{
    SNO snoConvList = SNOFileLookupGUID(CONVERSATIONLIST_SNO, szConvList);
    if (snoConvList == SNO_NONE && szConvList[0])
        return;

    LuaVerifySnosAreReferenced(snoConvList, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE);
    ActorGet((ActorID)ACDGetOwner(idACD))->snoConvList = snoConvList;
    ActorCommonData & tACD = ACDGet(idACD);
    for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {
        NPCUpdateConversationIconAndInteraction(tACD, *ptPlayer);
    }
}

void ActorWarpToActor(VACDID idACD, VACDID idACDTarget)
{
    ActorCommonData & tACD = ACDGet(idACD);
    ActorCommonData & tACDTarget = ACDGet(idACDTarget);
    if (ACDIsFriendly(tACD, tACDTarget, FALSE))
        ACDWarpToACD(tACD, tACDTarget, WARP_SCRIPT, 0.0f);
}

void ItemRemoveFromPlayer(VACDID idACDPlayer, LPCSTR szItemName)
{
    ItemRemoveMultipleFromPlayer(idACDPlayer, szItemName, 1);
}

void ItemRemoveMultipleFromPlayer(VACDID idACDPlayer, LPCSTR szItemName, int32 nCount)
{
    GBHandle hGBItem(GB_ITEMS, StringToGBID(szItemName));
    ActorCommonData & tACDPlayer = ACDGet(idACDPlayer);
    int32 nRemoved = 0;
    while (nRemoved < nCount)
    {
        ACDID idACDItem;
        int32 nItemCount;
        ACDInventoryGetItem(tACDPlayer, hGBItem, cgtDefaultPlayerInventorySlots, idACDItem, nItemCount, FALSE);
        if (idACDItem == ACDID_INVALID)
            return;

        ActorCommonData & tACDItem = ACDGet(idACDItem);
        if (nCount - nRemoved < nItemCount)
        {
            SACDInventoryDeleteItem(tACDItem, nCount - nRemoved, TRUE, FALSE);
            return;
        }
        SACDInventoryDeleteItem(tACDItem, nItemCount, TRUE, FALSE);
        nRemoved += nItemCount;
    }
}

int32 ItemGetCount(VACDID idACDPlayer, LPCSTR szItemName)
{
    int32 nCount = 0;
    GBID gbidItem = StringToGBID(szItemName);
    GBHandle gbHandle(GB_ITEMS, gbidItem);
    ActorCommonData & tACDPlayer = ACDGet(idACDPlayer);
    nCount = ACDInventoryGetItemCount(tACDPlayer, gbHandle, cgtDefaultPlayerInventorySlots, FALSE, FALSE);
    return nCount;
}

ACDID ItemGiveToPlayer(ACDID idACDPlayer, LPCSTR szItemName, LPCSTR szAffix1, LPCSTR szAffix2, LPCSTR szAffix3,
                       LPCSTR szAffix4, LPCSTR szAffix5, LPCSTR szAffix6)
{
    return sCreateItemInInventory(idACDPlayer, szItemName, FALSE, szAffix1, szAffix2, szAffix3, szAffix4, szAffix5,
                                  szAffix6, FALSE, FALSE);
}

ACDID ItemGiveToPlayerBlizzcon(ACDID idACDPlayer, LPCSTR szItemName, LPCSTR szAffix1, LPCSTR szAffix2,
                               LPCSTR szAffix3, LPCSTR szAffix4, LPCSTR szAffix5, LPCSTR szAffix6)
{
    return sCreateItemInInventory(idACDPlayer, szItemName, FALSE, szAffix1, szAffix2, szAffix3, szAffix4, szAffix5,
                                  szAffix6, FALSE, TRUE);
}

ACDID ItemGiveRewardToPlayer(ACDID idACDPlayer, LPCSTR szItemName, LPCSTR szAffix1, LPCSTR szAffix2,
                             LPCSTR szAffix3, LPCSTR szAffix4, LPCSTR szAffix5, LPCSTR szAffix6)
{
    return sCreateItemInInventory(idACDPlayer, szItemName, FALSE, szAffix1, szAffix2, szAffix3, szAffix4, szAffix5,
                                  szAffix6, TRUE, FALSE);
}

ACDID ItemGiveToPlayerStash(ACDID idACDPlayer, LPCSTR szItemName, LPCSTR szAffix1, LPCSTR szAffix2,
                            LPCSTR szAffix3, LPCSTR szAffix4, LPCSTR szAffix5, LPCSTR szAffix6)
{
    return sCreateItemInInventory(idACDPlayer, szItemName, TRUE, szAffix1, szAffix2, szAffix3, szAffix4, szAffix5,
                                  szAffix6, FALSE, FALSE);
}

void ItemApplyDye(VACDID idItem, int32 nDye)
{
    ActorCommonData * ptACDItem = ACDTryToGet(idItem);
    if (ptACDItem == NULL || !ptACDItem->IsItem() || !ItemIsDyeable(*ptACDItem))
        return;
    ACDID idACDOwner = ptACDItem->GetInventoryContainerOwner();
    ActorCommonData * ptACDOwner = ACDTryToGet(idACDOwner);
    if (ptACDOwner == NULL)
        return;
    SItemApplyDye(*ptACDItem, *ptACDOwner, (DyeType)Math::Pin(nDye, 0, 22));
}

void UpgradeJewel(VACDID idJewel, int32 nUpgradeRanks)
{
    ActorCommonData * ptACDJewel = ACDTryToGet(idJewel);
    if (ptACDJewel == NULL || !ptACDJewel->IsItem() || !ItemIsUpgradeableJewel(*ptACDJewel))
        return;
    ACDID idACDOwner = ptACDJewel->GetInventoryContainerOwner();
    ActorCommonData * ptACDOwner = ACDTryToGet(idACDOwner);
    if (ptACDOwner == NULL)
        return;
    SItemUpgradeJewel(*ptACDJewel, nUpgradeRanks);
}

BOOL ItemMoveRuneToSkillSlot(VACDID idACDPlayer, VACDID idACDRune, LPCSTR szPower)
{
    SNO snoPower = SNOFileLookupGUID(POWER_SNO, szPower);
    if (snoPower == SNO_NONE)
        return FALSE;

    LuaVerifySnosAreReferenced(snoPower, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE);

    if (ACDIsPlayer(idACDPlayer))
        return TRUE;
    return FALSE;
}

// DWARF decl lines: tACDPlayer 4841, tACDItem 4842, eSlot 4851, tInvLoc 4852, tPoint 4854
BOOL ItemMoveToStash(VACDID idACDPlayer, VACDID idACDItem)
{
    if (!ACDIsPlayer(idACDPlayer))
        return FALSE;
    ActorCommonData & tACDPlayer = ACDGet(idACDPlayer);
    ActorCommonData & tACDItem = ACDGet(idACDItem);
    if (!tACDItem.IsItem())
        return FALSE;
    InventorySlotType eSlot = SLOT_PLAYER_SHARED_STASH;
    InventoryLocation tInvLoc;
    IVector2D tPoint(0, 0);
    if (!ACDInventoryCanHold(tACDPlayer, tACDItem, eSlot, tPoint, NULL))
        return FALSE;
    ACDInventoryLocationSet(tInvLoc, tACDPlayer.id, eSlot, tPoint.x, tPoint.y);
    return ACDInventoryMove(tACDItem, tInvLoc);
}

double ItemGetAttributeFromInventorySlot(ACDID idACD, InventorySlotType eSlotType, LPCSTR szAttributeName,
                                         LPCSTR szAttribParamName)
{
    Attrib eAttrib = (Attrib)StringToAttrib(szAttributeName);
    if (eAttrib > AXEBADDATA && eAttrib < NUM_ATTRIBUTES)
    {
        int32 nParam = (szAttribParamName && szAttribParamName[0]) ? AttribStringToParam(eAttrib, szAttribParamName)
                                                                   : -1;
        return ItemGetAttributeFromInventorySlot(idACD, eSlotType, eAttrib, nParam);
    }
    return 0.0;
}

BOOL ItemHasLabel(ACDID idACD, InventorySlotType eSlotType, int32 eLabel)
{
    if ((eSlotType < SLOT_PLAYER_BODYSLOT_START || eSlotType >= SLOT_PLAYER_BODYSLOT_END) &&
        (eSlotType < SLOT_HIRELING_START || eSlotType >= SLOT_HIRELING_END))
        return FALSE;
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD == NULL)
        return FALSE;
    IVector2D tPoint(0, 0);
    ACDID idACDItem = ACDInventoryGetSquareItem(*ptACD, eSlotType, tPoint);
    ActorCommonData * ptACDItem = ACDTryToGet(idACDItem);
    if (ptACDItem == NULL)
        return FALSE;
    return ItemHasLabel(ptACDItem->hGB.gbid, eLabel);
}

void WorldCreate(LPCSTR szWorldName)
{
    SNO snoWorld = SNOFileLookupGUID(WORLD_SNO, szWorldName);
    if (snoWorld == SNO_NONE)
        return;

    SWorldID idSWorld = WorldCommonFindBySNO(snoWorld, TRUE);
    if (idSWorld == SWORLDID_INVALID)
        SWorldCreate(snoWorld, FALSE, FALSE, NULL, ACDID_INVALID, ACDID_INVALID, 0, 0, FALSE);
}

BOOL WorldIsCreated(LPCSTR szWorldName)
{
    SNO snoWorld = SNOFileLookupGUID(WORLD_SNO, szWorldName);
    if (snoWorld == SNO_NONE)
        return FALSE;

    SWorldID idSWorld = WorldCommonFindBySNO(snoWorld, TRUE);
    if (idSWorld != SWORLDID_INVALID)
    {
        SWorld * ptSWorld = SWorldGet(idSWorld);
        if (ptSWorld->eCreateStage > 6)
            return TRUE;
    }
    return FALSE;
}

BOOL WorldHasEvent(LPCSTR szWorldName, LPCSTR szEventName)
{
    SNO snoWorld = SNOFileLookupGUID(WORLD_SNO, szWorldName);
    if (snoWorld == SNO_NONE)
        return FALSE;
    SNO snoQuest = SNOFileLookupGUID(QUEST_SNO, szEventName);
    if (snoQuest == SNO_NONE)
        return FALSE;

    SWorldID idSWorld = WorldCommonFindBySNO(snoWorld, TRUE);
    if (idSWorld != SWORLDID_INVALID)
    {
        SWorld * ptSWorld = SWorldGet(idSWorld);
        POS pos = ptSWorld->listEventSNOs.GetHeadPosition();
        while (pos)
        {
            SNO snoEvent = ptSWorld->listEventSNOs.GetNext(pos);
            if (snoEvent == snoQuest)
                return TRUE;
        }
    }
    return FALSE;
}

BOOL WorldHasChallenge(LPCSTR szWorldName, LPCSTR szChallengeName)
{
    SNO snoWorld = SNOFileLookupGUID(WORLD_SNO, szWorldName);
    if (snoWorld == SNO_NONE)
        return FALSE;
    SNO snoQuest = SNOFileLookupGUID(QUEST_SNO, szChallengeName);
    if (snoQuest == SNO_NONE)
        return FALSE;

    SWorldID idSWorld = WorldCommonFindBySNO(snoWorld, TRUE);
    if (idSWorld != SWORLDID_INVALID)
    {
        SWorld * ptSWorld = SWorldGet(idSWorld);
        POS pos = ptSWorld->listEventSNOs.GetHeadPosition();
        while (pos)
        {
            SNO snoEvent = ptSWorld->listEventSNOs.GetNext(pos);
            if (snoEvent == snoQuest)
                return TRUE;
        }
    }
    return FALSE;
}

void PreLoadSNO(LPCSTR szGroupName, LPCSTR szSNOName, BOOL bWriteable)
{
    SNOGroup snoGroup = SNOMLGetSNOGroupTypeFromClassName(szGroupName);
    if (snoGroup != SNOGROUP_NONE)
    {
        SNO snoHandle = SNOFileLookupGUID(snoGroup, szSNOName);
        if (snoHandle != SNO_NONE)
        {
            GroupDefinition * pGroupDef = GameDefinitionGet(snoGroup);
            SGameID idSGame = GameServerCodeLeave_Tracked(TRUE);
            LuaScriptErrorHandlerEnter();
            if (bWriteable)
            {
                void * pDef = pGroupDef->LockW(snoHandle, FALSE);
                pGroupDef->UnlockW(pDef);
            }
            else
            {
                const void * pDef = pGroupDef->LockR(snoHandle, FALSE);
                pGroupDef->UnlockR((void *&)pDef);
            }
            LuaScriptErrorHandlerExit();
            if (!GameServerCodeEnter_Track())
                _GameServerCodeEnter(idSGame);
        }
    }
}

void PreFetchSNO(LPCSTR szGroupName, LPCSTR szSNOName)
{
    SNOGroup snoGroup = SNOMLGetSNOGroupTypeFromClassName(szGroupName);
    if (snoGroup != SNOGROUP_NONE)
        Prefetch::AddToQueue(SNOName(snoGroup, szSNOName), 2);
}

LPCSTR PlayerGetClass(VPACDID idACDPlayer)
{
    return PlayerGetShortClassName(PlayerGetPlayerClass(idACDPlayer));
}

PlayerClass PlayerGetClassEnum(VPACDID idACDPlayer)
{
    return PlayerGetPlayerClass(idACDPlayer);
}

BOOL PlayerIsRangedClass(VPACDID idACDPlayer)
{
    return HeroIsRanged(idACDPlayer);
}

LPCSTR ActorGetGender(VACDID idACD)
{
    if (ACDIsPlayer(idACD))
        return PlayerGetGender(idACD);

    ActorCommonData & tACD = ACDGet(idACD);
    ActorGender eGender = (ActorGender)ActorDefinitionGetTagMapValueDWORD(TAG_ACTOR_NPC_GENDER, tACD.snoActor);
    return eGender == ACTOR_GENDER_MALE ? "M" : eGender == ACTOR_GENDER_FEMALE ? "F" : "N";
}

BOOL PlayerHasUnlockedHireling(VACDID idPlayer, HirelingClass eHirelingClass)
{
    return HirelingTestTypeAvailable(idPlayer, eHirelingClass);
}

void PlayerUnlockHireling(VACDID idPlayer, HirelingClass eHirelingClass)
{
    Player * ptPlayer = PlayerGetByACD(idPlayer);
    if (ptPlayer)
    {
        if (!HirelingTestTypeAvailable(idPlayer, eHirelingClass))
        {
            HirelingSwapMessage tMessage;
            tMessage.eNewClass = eHirelingClass;
            PlayerList listDestPlayers(PlayerListGetElementPool());
            listDestPlayers.AddTail_Tracked(ptPlayer);
            MessageSendToClient(listDestPlayers, SMSG_HIRELING_UNLOCKED, &tMessage, sizeof(tMessage));
        }
        HirelingSetTypeAvailable(idPlayer, eHirelingClass, TRUE);
    }
}

ACDID PlayerGetCurrentHireling(VACDID idPlayer)
{
    return PlayerGetHireling(idPlayer);
}

HirelingClass PlayerGetCurrentHirelingClass(VACDID idPlayer)
{
    ACDID idACDHireling = PlayerGetHireling(idPlayer);
    ActorCommonData * ptACDHireling = ACDTryToGet(idACDHireling);
    if (!ptACDHireling)
        return (HirelingClass)0;
    return ACDGetHirelingClass(*ptACDHireling);
}

void PlayerLearnSkill(VPACDID idACDPlayer, LPCSTR szPower)
{
    SNO snoPower = SNOFileLookupGUID(POWER_SNO, szPower);
    if (snoPower == SNO_NONE)
        return;
    LuaVerifySnosAreReferenced(snoPower, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE);
    {
        ActorCommonData & tACDPlayer = ACDGet(idACDPlayer);
        tACDPlayer.AttributesSetInt(MakeAttribKey(ATTRIBUTE_SKILL, snoPower), 1);
        tACDPlayer.ApplyPassive(snoPower, FALSE);
        Player * ptPlayer = PlayerGetByACD(idACDPlayer);
        if (ptPlayer)
        {
            if (PowerIsPassive(snoPower))
                SPlayerAssignToEmptyTraitSlot(*ptPlayer, snoPower);
            else
                SPlayerAssignToEmptyActiveSkillSlot(*ptPlayer, snoPower);
        }
    }
}

void PlayerAssignSkillToSlot(ACDID idPlayer, LPCSTR szPower, RuneType eRuneType, int32 nSlot)
{
    if (nSlot < 0 || nSlot >= NUM_SKILL_SLOTS)
        return;

    if (eRuneType < RUNE_TYPE_INVALID || eRuneType >= RUNE_TYPE_COUNT)
        return;

    SNO snoPower = SNOFileLookupGUID(POWER_SNO, szPower);

    if (snoPower != SNO_NONE)
    {
        Player * ptPlayer = PlayerGetByACD(idPlayer);
        if (ptPlayer)
            SPlayerSwitchActiveSkills(*ptPlayer, snoPower, eRuneType, nSlot, FALSE);
    }
}

CRefString PlayerGetSkillInSlot(ACDID idPlayer, int32 nSlot)
{
    CRefString sSkill("");
    if (nSlot < 0 || nSlot >= NUM_SKILL_SLOTS)
        return sSkill;

    Player * ptPlayer = PlayerGetByACD(idPlayer);
    if (ptPlayer)
    {
        SNO snoSkill = SPlayerGetActiveSkillInSlot(*ptPlayer, nSlot);
        if (snoSkill != SNO_NONE)
            sSkill = SNOToString(POWER_SNO, snoSkill, FALSE);
    }
    return sSkill;
}

void PlayerAssignTraitToSlot(ACDID idPlayer, LPCSTR szTrait, int32 nSlot)
{
    if (nSlot < 0 || nSlot >= NUM_TRAIT_SLOTS)
        return;

    SNO snoPower = SNOFileLookupGUID(POWER_SNO, szTrait);

    if (snoPower != SNO_NONE)
    {
        Player * ptPlayer = PlayerGetByACD(idPlayer);
        if (ptPlayer)
            SPlayerSwitchTraits(*ptPlayer, snoPower, nSlot, FALSE);
    }
}

void PlayerAssignItemToPotionSlot(ACDID idPlayer, LPCSTR szItemName)
{
    GBID gbidItem = StringToGBID(szItemName);
    if (gbidItem == GBID_NONE)
        return;
    if (!ItemHasLabel(gbidItem, ITL_POTION))
        return;

    Player * ptPlayer = PlayerGetByACD(idPlayer);
    if (ptPlayer)
    {
        HotbarButtonData tButtonData;
        tButtonData.SetItemGBID(gbidItem);
        PlayerSetPotionBtnAssignment(ptPlayer, tButtonData);
    }
}


void AllPlayersLearnMonsterLore(VACDID idACDMonster)
{
    ActorCommonData * ptACDMonster = ACDTryToGet(idACDMonster);
    if (ptACDMonster && ptACDMonster->IsMonster())
    {
        ScopedMonsterDefinitionLockR pdefMonster(ptACDMonster->GetMonsterSNO());
        if (pdefMonster->snoLore != SNO_NONE)
            SPlayerAllPlayersLearnLore(pdefMonster->snoLore);
    }
}

static void sCountItemsCallBack(ACDID idACDSquare, InventorySlotType, void * pUserData)
{
    int32 * pnItemCount = (int32 *)pUserData;
    const ActorCommonData & tACD = ACDGet(idACDSquare);
    uint64 uCount = *pnItemCount;
    uCount += Math::Max<uint64>(1, tACD.GetStackCount());
    if (uCount > INT32_MAX)
        uCount = INT32_MAX;
    *pnItemCount = (int32)uCount;
}

int32 PlayerCountItemsInStash(VACDID idACDPlayer)
{
    ActorCommonData * ptACDPlayer = ACDTryToGet(idACDPlayer);
    if (!ptACDPlayer)
        return 0;

    int32 nItemCount = 0;
    ACDInventoryIterateItems(*ptACDPlayer, SLOT_PLAYER_SHARED_STASH, 0, sCountItemsCallBack, &nItemCount);
    return nItemCount;
}

void ActorInterruptPrimaryPower(ACDID idActor)
{
    const ActorCommonData * ptACD = ACDTryToGet(idActor);
    if (ptACD)
    {
        PowerID idPrimary = ActorPowersGetPrimary((ActorID)ptACD->idOwner);
        if (idPrimary != POWERID_INVALID)
            PowerInterrupt(idPrimary, TRUE);
    }
}

void PlayerGrantExperience(ACDID idACDPlayer, uint32 nXPAmount)
{
    ActorCommonData * ptACDPlayer = ACDTryToGet(idACDPlayer);
    if (ptACDPlayer)
        ExperienceGrantDirectly(*ptACDPlayer, nXPAmount, TRUE, FALSE);
}

void PlayerWarpToWaypoint(VPACDID idACDPlayer, int32 nWaypointNumber)
{
    SWorldPlayerWarpToWaypoint(idACDPlayer, ACDID_INVALID, nWaypointNumber);
}

void WaypointActivate(LPCSTR szLevelAreaName)
{
    SNO snoLevelArea = SNOFileLookupGUID(LEVELAREA_SNO, szLevelAreaName);
    if (snoLevelArea == SNO_NONE)
        return;

    LuaVerifySnosAreReferenced(snoLevelArea, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE);

    int32 nWaypointNumber = ActGetWaypointByLevelArea(snoLevelArea, FALSE);
    if (nWaypointNumber >= 0)
    {
        Minimap::ActivateWaypointMarker(snoLevelArea);
        SPlayerActivateWaypoint(nWaypointNumber, TRUE);
    }
}

void PlayerWarpToWorld(VPACDID idActorPlayer, LPCSTR szWorldName, LPCSTR szEntranceName, float flWarpFadeInSecs, PlayerWarpReason eReason)
{
    SNO snoWorld = SNOFileLookupGUID(WORLD_SNO, szWorldName);
    uint32 uEntranceGUID = StartingLocationNameToGUID(szEntranceName);
    if (snoWorld == SNO_NONE || uEntranceGUID == (uint32)-1)
        return;

    LuaVerifySnosAreReferenced(snoWorld, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE);
    Player * ptPlayer = PlayerGetByACD(idActorPlayer);
    if (!ptPlayer)
    {
        SGameACDChangeWorld(idActorPlayer, snoWorld, uEntranceGUID, eReason, flWarpFadeInSecs, NULL);
        return;
    }

    SGameACDChangeWorld(ptPlayer->idPlayerACD, snoWorld, uEntranceGUID, eReason, flWarpFadeInSecs, NULL);
    for (Player * ptOtherPlayer = PlayerGetFirstInGame(); ptOtherPlayer; ptOtherPlayer = PlayerGetNextInGame(ptOtherPlayer))
    {
        BOOL bLocal = PlayersAreLocalToEachOther(*ptPlayer, *ptOtherPlayer);
        if (ptOtherPlayer != ptPlayer && bLocal)
        {
            SGameACDChangeWorld(ptOtherPlayer->idPlayerACD, snoWorld, uEntranceGUID, eReason, flWarpFadeInSecs, ptPlayer);
        }
    }
}

void PlayerUseItem(VPACDID idACDPlayer, VACDID idACDItem)
{
    Player * ptPlayer = PlayerGetByACD(idACDPlayer);
    if (ptPlayer)
    {
        Target tTarget;
        tTarget.InitializeFromACD(ACDID_INVALID, FALSE, FALSE);
        PlayerBrainTryInventoryUseItem(*ptPlayer, idACDItem, tTarget);
    }
}

float PlayerGetAFKTime(VPACDID idACDPlayer)
{
    return SPlayerGetAFKTime(idACDPlayer);
}

float PlayerGetInactionTime(VPACDID idACDPlayer)
{
    return SPlayerGetInactionTime(idACDPlayer);
}

int32 PlayerGetLevelCap(VPACDID idACDPlayer)
{
    int32 nLevelCap = 0;
    ActorCommonData * ptACDPlayer = ACDTryToGet(idACDPlayer);
    if (ptACDPlayer && ACDIsPlayer(idACDPlayer))
        nLevelCap = ptACDPlayer->AttributesGetInt(ATTRIBUTE_LEVEL_CAP);
    return nLevelCap;
}

int32 PlayerCount()
{
    return PlayersGetCount();
}

void PlayerClearReturnPointsInWorld(LPCSTR szWorld)
{
    SNO snoWorld = SNOFileLookupGUID(WORLD_SNO, szWorld);
    if (snoWorld != SNO_NONE)
    {
        for (Player * ptPlayer = PlayerGetFirstAll(); ptPlayer; ptPlayer = PlayerGetNextAll(ptPlayer))
            SPlayerClearAllReturnPoints(*ptPlayer, snoWorld);
    }
}

BOOL WorldContainsLabel(VACDID idACD, LPCSTR szLabelName)
{
    ActorCommonData & tACD = ACDGet(idACD);
    if (!tACD.IsInWorld())
        return FALSE;
    const WorldLocation & tWorldLoc = tACD.GetWorldLocation();
    const WorldCommonData & tWCD = WorldGetCommonData(tWorldLoc.idSWorld);
    return LabelAlreadySetInWorld(StringToGBID(szLabelName), tWCD);
}

BOOL GameFlagIsSet(LPCSTR szFlagLabel)
{
    return GameCommonGameFlagIsSet(szFlagLabel);
}

void GameFlagSet(LPCSTR szFlagLabel, BOOL bValue)
{
    SNO snoFlagSet = GlobalSNOGet(GLOBALSNO_FLAGSET_GAME_FLAGS);
    GameCommonData & tGCD = GameGetActiveGameCommonData();
    int32 nGameFlagIndex = FlagSetLookup(snoFlagSet, szFlagLabel);

    if (tGCD.ptQuestGlobals)
    {
        SQuestsOnGameFlagSet(nGameFlagIndex, bValue);
    }

    if (nGameFlagIndex >= 0 && nGameFlagIndex < tGCD.nGameFlagsSize * 8)
    {
        uint32 dwBit = nGameFlagIndex & 7;
        uint32 dwByte = (uint32)nGameFlagIndex >> 3;
        if (bValue)
            tGCD.arGameFlags[dwByte] |= (1 << dwBit);
        else
            tGCD.arGameFlags[dwByte] &= ~(1 << dwBit);
        SGameGlobalsGet().dwFlags2 |= (1 << SGAMEGLOBALS2_GAME_FLAGS_CHANGED);
    }
}

void GameFlagSendSyncedFlags()
{
    SGameGlobals & tSGameGlobals = SGameGlobalsGet();
    GameCommonData & tGCD = SGameGetGameCommonData();
    if ((tSGameGlobals.dwFlags2 >> SGAMEGLOBALS2_GAME_FLAGS_CHANGED) & 1)
    {
        BOOL bLootRunOpen = GameCommonGameFlagIsSet("LootRunOpen");
        BOOL bLootRunBossDead = (tSGameGlobals.dwFlags2 >> SGAMEGLOBALS2_LOOT_RUN_BOSS_KILLED) & 1;
        BOOL bTieredLootRunFailed = GameCommonGameFlagIsSet("LootRunTieredRiftFailed");
        BOOL bLootRunBossActive = (tSGameGlobals.dwFlags2 >> SGAMEGLOBALS2_LOOT_RUN_BOSS_ACTIVE) & 1;
        BOOL bLootRunChallengeCompleted = (tSGameGlobals.dwFlags2 >> SGAMEGLOBALS2_LOOT_RUN_CHALLENGE_COMPLETED) & 1;
        BOOL bSetDungeonActive = GameCommonGameFlagIsSet("Sets_Generic_DungeonInProgress");
        if (tGCD.tGameSyncedData.bLootRunOpen != bLootRunOpen ||
            tGCD.tGameSyncedData.bLootRunBossDead != bLootRunBossDead ||
            tGCD.tGameSyncedData.bTieredLootRunFailed != bTieredLootRunFailed ||
            tGCD.tGameSyncedData.bLootRunBossActive != bLootRunBossActive ||
            tGCD.tGameSyncedData.bLootRunChallengeCompleted != bLootRunChallengeCompleted ||
            tGCD.tGameSyncedData.bSetDungeonActive != bSetDungeonActive)
        {
            tGCD.tGameSyncedData.bLootRunOpen = bLootRunOpen;
            tGCD.tGameSyncedData.nOpenLootRunLevel = NephalemRiftManager::GetActiveRiftLevel();
            tGCD.tGameSyncedData.bLootRunBossDead = bLootRunBossDead;
            tGCD.tGameSyncedData.bTieredLootRunFailed = bTieredLootRunFailed;
            tGCD.tGameSyncedData.bLootRunBossActive = bLootRunBossActive;
            tGCD.tGameSyncedData.bLootRunChallengeCompleted = bLootRunChallengeCompleted;
            tGCD.tGameSyncedData.bSetDungeonActive = bSetDungeonActive;
            SGameSyncData(NULL);
        }
        tSGameGlobals.dwFlags2 &= ~(1 << SGAMEGLOBALS2_GAME_FLAGS_CHANGED);
    }
}


void PlayCutsceneForPlayer(const int32 nIndex, VACDID idACDPlayer)
{
    PlayCutsceneMessage tMessage;
    tMessage.nIndex = nIndex;
    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetSingle(listDestPlayers, idACDPlayer);
    MessageSendToClient(listDestPlayers, SMSG_PLAY_CUTSCENE, &tMessage, sizeof(tMessage));
}


void PlayAnniversaryCutsceneForPlayer(VACDID idACDPlayer)
{
    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetSingle(listDestPlayers, idACDPlayer);
    SimpleMessage tMessage;
    MessageSendToClient(listDestPlayers, SMSG_PLAY_ANNIVERSARY_CUTSCENE, &tMessage, sizeof(PlayCutsceneMessage));
}

void MusicPlayForPlayer(LPCSTR szMusic, ACDID idActorPlayer)
{
    SNO snoMusic = SNOFileLookupGUID(MUSIC_SNO, szMusic);
    LuaVerifySnosAreReferenced(snoMusic, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE);
    if (ACDIsPlayer(idActorPlayer) && GameActiveGameCommonDataIsServer())
    {
        PlayMusicMessage tMessage;
        tMessage.snoMusic = snoMusic;
        PlayerList listDestPlayers(PlayerListGetElementPool());
        PlayerListGetSingle(listDestPlayers, idActorPlayer);
        MessageSendToClient(listDestPlayers, SMSG_PLAY_MUSIC, &tMessage, sizeof(tMessage));
    }
}


void MusicStopOverlays()
{
    if (GameActiveGameCommonDataIsServer())
    {
        SimpleMessage tMessage;
        PlayerList listDestPlayers(PlayerListGetElementPool());
        PlayerListGetAllConnected(listDestPlayers, -1, FALSE);
        MessageSendToClient(listDestPlayers, SMSG_STOP_MUSIC_OVERLAY, &tMessage, sizeof(tMessage));
    }
}

void MusicStopOverlaysForPlayer(ACDID idActorPlayer)
{
    if (ACDIsPlayer(idActorPlayer) && GameActiveGameCommonDataIsServer())
    {
        PlayerList listDestPlayers(PlayerListGetElementPool());
        PlayerListGetSingle(listDestPlayers, idActorPlayer);
        SimpleMessage tMessage;
        MessageSendToClient(listDestPlayers, SMSG_STOP_MUSIC_OVERLAY, &tMessage, sizeof(tMessage));
    }
}

SSceneID SceneGroupFind(LPCSTR szSceneGroup)
{
    SSceneID idSScene = SSCENEID_INVALID;
    SNO snoSceneGroup = SNOFileLookupGUID(SCENEGROUP_SNO, szSceneGroup);
    LuaVerifySnosAreReferenced(snoSceneGroup, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE);
    SceneCommonData * ptSCD = NULL;
    SceneCommonDataIterator tIterator;
    while (SceneCommonDataArrayIterate(tIterator, ptSCD, SkipHolodeckScenes))
    {
        if (ptSCD->snoSceneGroup == snoSceneGroup)
        {
            idSScene = ptSCD->idSScene;
            break;
        }
    }
    return idSScene;
}

void SceneGroupSwap(SSceneID idSScene, int32 nSubSceneIndex)
{
    if (idSScene != SSCENEID_INVALID)
    {
        SScene * ptSScene = SSceneTryToGet(idSScene);
        if (ptSScene && ptSScene->tSCD.snoSceneGroup != SNO_NONE)
        {
            ScopedSceneGroupDefinitionLockR ptSceneGroupDef(ptSScene->tSCD.snoSceneGroup);
            int32 nIndex = 0;
            if (nSubSceneIndex > 0)
                nIndex = (nSubSceneIndex > ptSceneGroupDef->nSceneCnt ? ptSceneGroupDef->nSceneCnt : nSubSceneIndex) - 1;
            SNO snoSceneSwapTo = ptSceneGroupDef->arScenes[nIndex].snoScene;
            SSceneSwapSubScene(idSScene, snoSceneSwapTo);
        }
    }
}

int32 SceneGroupGetSubSceneCount(SSceneID idSScene)
{
    int32 nSubSceneCount = 0;
    if (idSScene != SSCENEID_INVALID)
    {
        SScene * ptSScene = SSceneTryToGet(idSScene);
        if (ptSScene && ptSScene->tSCD.snoSceneGroup != SNO_NONE)
        {
            ScopedSceneGroupDefinitionLockR pdefSceneGroup(ptSScene->tSCD.snoSceneGroup);
            nSubSceneCount = pdefSceneGroup->nSceneCnt;
        }
    }
    return nSubSceneCount;
}

int32 SceneGroupGetCurrentSubScene(SSceneID idSScene)
{
    int32 nSceneIndex = 0;
    if (idSScene != SSCENEID_INVALID)
    {
        SScene * ptSScene = SSceneTryToGet(idSScene);
        if (ptSScene && ptSScene->tSCD.snoSceneGroup != SNO_NONE)
        {
            ScopedSceneGroupDefinitionLockR ptSceneGroupDef(ptSScene->tSCD.snoSceneGroup);
            for (int32 i = 0; i < ptSceneGroupDef->nSceneCnt; i++)
            {
                if (ptSceneGroupDef->arScenes[i].snoScene == ptSScene->tSCD.snoScene)
                {
                    nSceneIndex = i + 1;
                    break;
                }
            }
        }
    }
    return nSceneIndex;
}

void ActorSnapFaceActor(ACDID idActor, ACDID idTarget)
{
    ActorCommonData * ptACD = ACDTryToGet(idActor);
    const ActorCommonData * ptACDTarget = ACDTryToGet(idTarget);
    if (ptACD && ptACDTarget && ptACDTarget->IsInWorld() && ptACD->IsInWorld() && ACDInSameWorld(*ptACD, *ptACDTarget))
        ptACD->SetFacing(ptACDTarget->GetPlace(), TRUE);
}

void ActorSnapFaceAbsolute(ACDID idActor, int32 nDegrees)
{
    ActorCommonData * ptACD = ACDTryToGet(idActor);
    if (ptACD && ptACD->IsInWorld())
    {
        Angle aNewYaw = Math::DegToAngle((float)nDegrees);
        ACDSetYaw(idActor, aNewYaw);
    }
}

void EventSend(LPCSTR szEventName, ACDID idACD, int32 nParam)
{
    ScriptEventOnUserEvent(szEventName, idACD, nParam);
}

void EventSendInRange(LPCSTR szEventName, ACDID idACD, float flRange, int32 nParam)
{
    ScriptEventOnUserEventInRange(szEventName, idACD, flRange, nParam);
}

void EventSendToActor(LPCSTR szEventName, ACDID idACDSender, ACDID idACDReceiver, int32 nParam)
{
    ScriptEventOnUserEventSentToActor(szEventName, idACDSender, idACDReceiver, nParam);
}

BOOL GameIsConsole()
{
    return TRUE;
}

BOOL GameIsLastGenConsole()
{
    return FALSE;
}

BOOL GameHasHighScriptInstanceCount()
{
    return LuaScriptHasHighScriptInstanceCount();
}

BOOL GameIsPTR()
{
    return FALSE;
}

BOOL GameIsBattleNet()
{
    return GameIsBattleNetServer();
}

int32 GameGetDifficulty()
{
    return GameGetHandicapLevel();
}

void TriggerPVPTeamDeathmatchGameEnd()
{
    PvPTeamDeathmatchGameEnd();
}

void PVPTeamAddScore(Team eTeam, uint32 points, ACDID idACDScorer)
{
    GameCommonData & tGCD = GameGetActiveGameCommonData();
    const uint32 MAX_POINTS = 300;

    if (eTeam == TEAM_PLAYER_0)
        tGCD.tGameSyncedData.nTeamScore[0] = Math::Min(tGCD.tGameSyncedData.nTeamScore[0] + points, MAX_POINTS);
    else if (eTeam == TEAM_PLAYER_1)
        tGCD.tGameSyncedData.nTeamScore[1] = Math::Min(tGCD.tGameSyncedData.nTeamScore[1] + points, MAX_POINTS);

    if (idACDScorer != ACDID_INVALID)
    {
        Player * ptPlayer = ACDGetControllingPlayer(idACDScorer);
        if (ptPlayer)
        {
            PlayerList listDestPlayers(PlayerListGetElementPool());
            PlayerListGetSingle(listDestPlayers, ptPlayer->idPlayerACD);

            FloatingNumberMessage tKillMessage;
            tKillMessage.eType = FLOATING_NUMBER_TYPE_PVP_KILL;
            tKillMessage.flDamage = (float)points;
            tKillMessage.annTarget = ANNFromACD(ptPlayer->idPlayerACD);
            MessageSendToClient(listDestPlayers, SMSG_FLOATING_NUMBER, &tKillMessage, sizeof(tKillMessage));
        }
        else if (ACDTryToGet(idACDScorer))
        {
            PlayerList listDestPlayers(PlayerListGetElementPool());
            PlayerListGetAllies(listDestPlayers, idACDScorer, ACDID_INVALID);

            FloatingNumberMessage tKillMessage;
            tKillMessage.eType = FLOATING_NUMBER_TYPE_PVP_KILL;
            tKillMessage.flDamage = (float)points;
            tKillMessage.annTarget = ANNFromACD(idACDScorer);
            MessageSendToClient(listDestPlayers, SMSG_FLOATING_NUMBER, &tKillMessage, sizeof(tKillMessage));
        }
    }

    SGameSyncData(NULL);
}

void PVPPlayerSetOnDeathAccolade(ACDID idACDPlayer, LPCSTR szAccolade, float flValue)
{
    SNO snoAccolade = SNOFileLookupGUID(ACCOLADE_SNO, szAccolade);
    if (idACDPlayer != ACDID_INVALID && snoAccolade != SNO_NONE)
    {
        ActorCommonData * ptACD = ACDTryToGet(idACDPlayer);
        if (ptACD)
        {
            ptACD->AttributesSetInt(ATTRIBUTE_ON_DEATH_ACCOLADE_0, snoAccolade);
            ptACD->AttributesSetFloat(ATTRIBUTE_ON_DEATH_ACCOLADE_0_VALUE, flValue);
        }
    }
}

void PVPPlayerSetAccolade(ACDID idACDPlayer, int32 nIndex, LPCSTR szAccolade, float flValue)
{
    SNO snoAccolade = SNOFileLookupGUID(ACCOLADE_SNO, szAccolade);
    if (idACDPlayer != ACDID_INVALID && snoAccolade != SNO_NONE)
    {
        ActorCommonData * ptACD = ACDTryToGet(idACDPlayer);
        if (ptACD)
        {
            switch (nIndex)
            {
            case 0:
                ptACD->AttributesSetInt(ATTRIBUTE_ACCOLADE_0, snoAccolade);
                ptACD->AttributesSetFloat(ATTRIBUTE_ACCOLADE_0_VALUE, flValue);
                break;
            case 1:
                ptACD->AttributesSetInt(ATTRIBUTE_ACCOLADE_1, snoAccolade);
                ptACD->AttributesSetFloat(ATTRIBUTE_ACCOLADE_1_VALUE, flValue);
                break;
            case 2:
                ptACD->AttributesSetInt(ATTRIBUTE_ACCOLADE_2, snoAccolade);
                ptACD->AttributesSetFloat(ATTRIBUTE_ACCOLADE_2_VALUE, flValue);
                break;
            case 3:
                ptACD->AttributesSetInt(ATTRIBUTE_ACCOLADE_3, snoAccolade);
                ptACD->AttributesSetFloat(ATTRIBUTE_ACCOLADE_3_VALUE, flValue);
                break;
            case 4:
                ptACD->AttributesSetInt(ATTRIBUTE_ACCOLADE_4, snoAccolade);
                ptACD->AttributesSetFloat(ATTRIBUTE_ACCOLADE_4_VALUE, flValue);
                break;
            }
        }
    }
}

int32 PVPPlayerGetRank(ACDID idACDPlayer)
{
    if (ACDIsPlayer(idACDPlayer))
        return ACDGet(idACDPlayer).AttributesGetInt(ATTRIBUTE_PVP_RANK);
    return 0;
}

void PVPAwardNormalXP(ACDID idACDPlayer, int32 nAmount)
{
    if (!ACDIsPlayer(idACDPlayer))
        return;
    ActorCommonData & tACD = ACDGet(idACDPlayer);
    uint64 nPrevValue = tACD.PvPGetNormalExperienceGainedThisGame();
    tACD.PvPSetNormalExperienceGainedThisGame(nPrevValue + nAmount);
    ExperienceGrantDirectly(tACD, nAmount, TRUE, FALSE);
}

void PVPAwardPVPXP(ACDID idACDPlayer, int32 nAmount)
{
    if (!ACDIsPlayer(idACDPlayer))
        return;
    ActorCommonData & tACD = ACDGet(idACDPlayer);
    uint64 nPrevValue = tACD.PvPGetPvPExperienceGainedThisGame();
    tACD.PvPSetPvPExperienceGainedThisGame(nPrevValue + nAmount);
    PVPExperienceGrantDirectly(tACD, nAmount, TRUE);
}

void PVPAwardGold(ACDID idACDPlayer, int32 nAmount)
{
    if (ACDIsPlayer(idACDPlayer))
    {
        ActorCommonData & tACD = ACDGet(idACDPlayer);
        int32 nPrevValue = tACD.AttributesGetInt(ATTRIBUTE_PVP_GOLD_GAINED_THIS_GAME);
        tACD.AttributesSetInt(ATTRIBUTE_PVP_GOLD_GAINED_THIS_GAME, nPrevValue + nAmount);
        tACD.ModifyCurrencyAmount(nAmount, CURRENCY_GOLD, GMR_SCRIPT, FALSE);
    }
}

void PVPAwardTokens(ACDID idACDPlayer, int32 nAmount)
{
    if (ACDIsPlayer(idACDPlayer))
    {
        ActorCommonData & tACD = ACDGet(idACDPlayer);
        int32 nPrevValue = tACD.AttributesGetInt(ATTRIBUTE_PVP_TOKENS_GAINED_THIS_GAME);
        tACD.AttributesSetInt(ATTRIBUTE_PVP_TOKENS_GAINED_THIS_GAME, nPrevValue + nAmount);
        int32 nCurrentAmount = tACD.AttributesGetInt(ATTRIBUTE_PVP_TOKENS);
        tACD.AttributesSetInt(ATTRIBUTE_PVP_TOKENS, nCurrentAmount + nAmount);
    }
}

int32 PVPGetGoldAwardAmount(uint32 nLevel, PVPGameResult ePVPGameResult)
{
    int32 nGoldAwardAmount = 0;
    if (nLevel < HERO_LEVEL_MAX && ePVPGameResult >= 0 && ePVPGameResult < NUM_PVP_GAME_RESULTS)
    {
        ScopedGameBalanceDefinitionLockR ptExpTableDef(GlobalSNOGet(GLOBALSNO_GB_EXPERIENCE));
        const ExperienceTable & tExpTable = ptExpTableDef->tExperienceTable;
        nGoldAwardAmount = tExpTable.ptExperienceLevels[nLevel].tPVPAward[ePVPGameResult].nGold;
    }
    return nGoldAwardAmount;
}

int32 PVPGetGoldAwardAmountForRank(uint32 nRank, PVPGameResult ePVPGameResult)
{
    int32 nGoldAwardAmount = 0;
    if (nRank <= PVP_RANK_MAX && ePVPGameResult >= 0 && ePVPGameResult < NUM_PVP_GAME_RESULTS)
    {
        ScopedGameBalanceDefinitionLockR ptExpTableDef(GlobalSNOGet(GLOBALSNO_GB_EXPERIENCE));
        const ExperienceTable & tExpTable = ptExpTableDef->tExperienceTable;
        nGoldAwardAmount = tExpTable.ptExperienceLevels[nRank].tPVPAward[ePVPGameResult].nGoldByRank;
    }
    return nGoldAwardAmount;
}

int32 PVPGetPVPXPAwardAmount(uint32 nLevel, PVPGameResult ePVPGameResult)
{
    int32 nPVPXPAwardAmount = 0;
    if (nLevel < HERO_LEVEL_MAX && ePVPGameResult >= 0 && ePVPGameResult < NUM_PVP_GAME_RESULTS)
    {
        ScopedGameBalanceDefinitionLockR ptExpTableDef(GlobalSNOGet(GLOBALSNO_GB_EXPERIENCE));
        const ExperienceTable & tExpTable = ptExpTableDef->tExperienceTable;
        nPVPXPAwardAmount = tExpTable.ptExperienceLevels[nLevel].tPVPAward[ePVPGameResult].nPVPXP;
    }
    return nPVPXPAwardAmount;
}

int32 PVPGetNormalXPAwardAmount(uint32 nLevel, PVPGameResult ePVPGameResult)
{
    int32 nXPAwardAmount = 0;
    if (nLevel < HERO_LEVEL_MAX && ePVPGameResult >= 0 && ePVPGameResult < NUM_PVP_GAME_RESULTS)
    {
        ScopedGameBalanceDefinitionLockR ptExpTableDef(GlobalSNOGet(GLOBALSNO_GB_EXPERIENCE));
        const ExperienceTable & tExpTable = ptExpTableDef->tExperienceTable;
        nXPAwardAmount = tExpTable.ptExperienceLevels[nLevel].tPVPAward[ePVPGameResult].nXP;
    }
    return nXPAwardAmount;
}

int32 PVPGetAltXPAwardAmount(uint32 nLevel, PVPGameResult ePVPGameResult)
{
    int32 nAltXPAwardAmount = 0;
    if (nLevel < HERO_LEVEL_MAX && ePVPGameResult >= 0 && ePVPGameResult < NUM_PVP_GAME_RESULTS)
    {
        ScopedGameBalanceDefinitionLockR ptExpTableDef(GlobalSNOGet(GLOBALSNO_GB_EXPERIENCE));
        const ExperienceTable & tExpTable = ptExpTableDef->tExperienceTable;
        nAltXPAwardAmount = tExpTable.ptExperienceLevels[nLevel].tPVPAward[ePVPGameResult].nAltXP;
    }
    return nAltXPAwardAmount;
}

int32 PVPGetTokensAwardAmount(uint32 nRank, PVPGameResult ePVPGameResult)
{
    int32 nTokensAwardAmount = 0;
    if (nRank <= PVP_RANK_MAX && ePVPGameResult >= 0 && ePVPGameResult < NUM_PVP_GAME_RESULTS)
    {
        ScopedGameBalanceDefinitionLockR ptExpTableDef(GlobalSNOGet(GLOBALSNO_GB_EXPERIENCE));
        const ExperienceTable & tExpTable = ptExpTableDef->tExperienceTable;
        nTokensAwardAmount = tExpTable.ptExperienceLevels[nRank].tPVPAward[ePVPGameResult].nTokens;
    }
    return nTokensAwardAmount;
}

inline BOOL sTriggerTrialEndInternal(float flDelayInSeconds, uint32 uFlags)
{
    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetAllContentFlagRestrictedPlayers(listDestPlayers, uFlags);
    if (listDestPlayers.Count() > 0)
    {
        QuestSetGameOver();
        IntDataMessage tMessage;
        tMessage.nData = Math::FloatToInt(flDelayInSeconds * 60.0f);
        MessageSendToClient(listDestPlayers, SMSG_TRIAL_OVER, &tMessage, sizeof(tMessage));
        return TRUE;
    }
    return FALSE;
}

BOOL TriggerTrialEnd(float flDelayInSeconds)
{
    return sTriggerTrialEndInternal(flDelayInSeconds, 1);
}

BOOL TriggerBetaEnd()
{
    if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
        return FALSE;
    if (!OnlineService::IsPTR())
        return FALSE;
    {
        PlayerList listDestPlayers(PlayerListGetElementPool());
        PlayerListGetAllInGame(listDestPlayers, -1, FALSE, -1);
        if (listDestPlayers.Count() > 0)
        {
            SimpleMessage tMessage;
            MessageSendToClient(listDestPlayers, SMSG_BETA_OVER, &tMessage, sizeof(tMessage));
            POS pos = listDestPlayers.GetHeadPosition();
            while (pos)
                SPlayerUnlockAdventureMode(*listDestPlayers.GetNext(pos));
        }
    }
    return TRUE;
}

BOOL TriggerDemoEnd()
{
    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetAllInGame(listDestPlayers, -1, FALSE, -1);
    if (listDestPlayers.Count() > 0)
    {
        SimpleMessage tMessage;
        MessageSendToClient(listDestPlayers, SMSG_DEMO_OVER, &tMessage, sizeof(tMessage));
    }
    if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
        QuestSetGameOver();
    return TRUE;
}

int32 TimeGetDayOfWeek()
{
    SGame & tSGame = SGameManager::GameGet();
    Blizzard::Time::Timestamp tStartTime = Blizzard::Time::LocalBias(tSGame.ptSGameGlobals->tUTCGameStartTime);
    Blizzard::Time::TimeRec tTimeRecord;
    Blizzard::Time::BreakTime(tStartTime, tTimeRecord);
    return tTimeRecord.dayOfWeek;
}

int32 TimeGetHour()
{
    SGame & tSGame = SGameManager::GameGet();
    Blizzard::Time::Timestamp tStartTime = Blizzard::Time::LocalBias(tSGame.ptSGameGlobals->tUTCGameStartTime);
    Blizzard::Time::TimeRec tTimeRecord;
    Blizzard::Time::BreakTime(tStartTime, tTimeRecord);
    return tTimeRecord.hour;
}

int32 TimeGetDay()
{
    SGame & tSGame = SGameManager::GameGet();
    Blizzard::Time::Timestamp tStartTime = Blizzard::Time::LocalBias(tSGame.ptSGameGlobals->tUTCGameStartTime);
    Blizzard::Time::TimeRec tTimeRecord;
    Blizzard::Time::BreakTime(tStartTime, tTimeRecord);
    return tTimeRecord.day;
}

int32 TimeGetMonth()
{
    SGame & tSGame = SGameManager::GameGet();
    Blizzard::Time::Timestamp tStartTime = Blizzard::Time::LocalBias(tSGame.ptSGameGlobals->tUTCGameStartTime);
    Blizzard::Time::TimeRec tTimeRecord;
    Blizzard::Time::BreakTime(tStartTime, tTimeRecord);
    return tTimeRecord.month;
}

static void sDisplayGameMessageForPlayerList(GameMessageType eType, PlayerList & listDestPlayers, LPCSTR szStringDecoratedLabel, int nParam1, int nParam2)
{
    DisplayGameTextMessage tDisplayMessage;
    Blizzard::String::Copy(tDisplayMessage.szMsg, szStringDecoratedLabel, sizeof(tDisplayMessage.szMsg));
    if (nParam1 != STRINGLIST_DEFAULT_INT_PARAM)
        tDisplayMessage.nParam1.Set(nParam1);
    if (nParam2 != STRINGLIST_DEFAULT_INT_PARAM)
        tDisplayMessage.nParam2.Set(nParam2);
    MessageSendToClient(listDestPlayers, eType, &tDisplayMessage, sizeof(tDisplayMessage));
}

void DisplayGameMessage(VACDID idACDPlayer, LPCSTR szStringDecoratedLabel, int nParam1, int nParam2)
{
    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetSingle(listDestPlayers, idACDPlayer);
    if (PlayerIsPrimary(idACDPlayer))
        sDisplayGameMessageForPlayerList(SMSG_DISPLAY_GAME_TEXT, listDestPlayers, szStringDecoratedLabel, nParam1, nParam2);
}

void DisplayGameMessageForAllPlayers(LPCSTR szStringDecoratedLabel, int nParam1, int nParam2)
{
    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetAllConnected(listDestPlayers, -1, FALSE);
    sDisplayGameMessageForPlayerList(SMSG_DISPLAY_GAME_TEXT, listDestPlayers, szStringDecoratedLabel, nParam1, nParam2);
}

void DisplayChatGameMessage(VACDID idACDPlayer, LPCSTR szStringDecoratedLabel, int nParam1, int nParam2)
{
    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetSingle(listDestPlayers, idACDPlayer);
    sDisplayGameMessageForPlayerList(SMSG_DISPLAY_GAME_CHAT_TEXT, listDestPlayers, szStringDecoratedLabel, nParam1, nParam2);
}

void DisplayChatGameMessageForAllPlayers(LPCSTR szStringDecoratedLabel, int nParam1, int nParam2)
{
    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetAllConnected(listDestPlayers, -1, FALSE);
    sDisplayGameMessageForPlayerList(SMSG_DISPLAY_GAME_CHAT_TEXT, listDestPlayers, szStringDecoratedLabel, nParam1, nParam2);
}

BOOL ActorHasQueuedActions(VACDID idACD)
{
    ActorID idActor = (ActorID)ACDGetOwner(idACD);
    return ActorIsUsingAIQueue(idActor);
}

AIQueuedActionID QActorMoveToLocationWithinTime(ACDID idACD, ACDID idLocACD, ActorAnimationTag eAnimTag, float flSeconds)
{
    ActorCommonData & tACD = ACDGet(idACD);
    ActorCommonData & tACDLocation = ACDGet(idLocACD);
    float flTicks = flSeconds * TICKS_PER_SECOND;
    if (flTicks <= 1.0f)
        flTicks = 1.0f;
    WorldPlace placeStart = tACD.GetPlace();
    WorldPlace placeEnd = tACDLocation.GetPlace();
    float flSpeed = Vector3D::Distance(placeStart.wp, placeEnd.wp) / flTicks;
    Target tTarget;
    tTarget.InitializeFromACD(idLocACD, FALSE, FALSE);
    AIQueuedActionID idQAction = AIQueueMoveToLocation(idACD, tTarget, eAnimTag, flSpeed);
    LuaScriptInstanceSetLastAIQAction(LuaScriptGetExecutingScriptInstance(), idQAction);
    return idQAction;
}

AIQueuedActionID QActorMoveToLocation(ACDID idACD, ACDID idLocACD, ActorAnimationTag eAnimTag, float flSpeed)
{
    Target tTarget;
    tTarget.InitializeFromACD(idLocACD, FALSE, FALSE);
    AIQueuedActionID idQAction = AIQueueMoveToLocation(idACD, tTarget, eAnimTag, (flSpeed > 20.0f) ? 20.0f : flSpeed);
    LuaScriptInstanceSetLastAIQAction(LuaScriptGetExecutingScriptInstance(), idQAction);
    return idQAction;
}

AIQueuedActionID QActorMoveToFrontOf(ACDID idACD, ACDID idLocACD, ActorAnimationTag eAnimTag, float flSpeed, float flDistInFrontOfTarget)
{
    ActorCommonData & tACDLocation = ACDGet(idLocACD);
    WorldPlace placeTarget;
    BrainAPIGetFrontOfPlace(tACDLocation, flDistInFrontOfTarget, placeTarget);
    Target tTarget;
    tTarget.InitializeFromPlace(placeTarget);
    AIQueuedActionID idQAction = AIQueueMoveToLocation(idACD, tTarget, eAnimTag, (flSpeed > 20.0f) ? 20.0f : flSpeed);
    LuaScriptInstanceSetLastAIQAction(LuaScriptGetExecutingScriptInstance(), idQAction);
    return idQAction;
}

void QActorStopAllQueuedActions(ACDID idACD)
{
    AIQueueCompleteCurrentAction((ActorID)ACDGetOwner(idACD));
    AIQueueClearList((ActorID)ACDGetOwner(idACD));
}

void ActorLookAtActor(VACDID idACD, VACDID idACDTarget)
{
    ACDLookAtACD(idACD, idACDTarget);
}

void ActorBroadcastLookAt(ACDID idACDSrc, float flIntensity, float flRadius)
{
    ActorCommonData & tACD = ACDGet(idACDSrc);
    tACD.AttributesSetFloat(ATTRIBUTE_LOOK_TARGET_BROADCAST_INTENSITY, flIntensity);
    tACD.AttributesSetFloat(ATTRIBUTE_LOOK_TARGET_BROADCAST_RADIUS, flRadius);
}

int32 PlayerGetNumInGame()
{
    return PlayerGetNumPlayers();
}

void ActorSetHitPointsPercent(ACDID idActor, float flPercent)
{
    ActorCommonData * ptACD = ACDTryToGet(idActor);
    if (ptACD)
        ptACD->SetHitPoints(ptACD->GetHitPointsMaxTotal() * flPercent, TRUE, FALSE);
}

void ActorSetHitPoints(ACDID idActor, float flHP)
{
    ActorCommonData * ptACD = ACDTryToGet(idActor);
    if (ptACD)
        ptACD->SetHitPoints(flHP, TRUE, FALSE);
}

void ActorScaleHPAndDamageToTieredRiftLevel(ACDID idActor, int32 nTieredRiftLevel)
{
    ActorCommonData * ptACDActor = ACDTryToGet(idActor);
    if (ptACDActor == NULL || !ptACDActor->IsMonster())
        return;

    float flDungeonFinderHPScalar = 1.0f;
    float flDungeonFinderDamageScalar = 1.0f;
    float flDungeonFinderXPScalar = 1.0f;

    ScopedGameBalanceDefinitionLockR pdefGB(GlobalSNOGet(GLOBALSNO_GB_TIERED_LOOT_RUN_LEVELS));
    int32 nGameBalanceIndex = Math::Max(nTieredRiftLevel, 0);
    nGameBalanceIndex = Math::Min(nGameBalanceIndex, (int32)pdefGB->tTieredLootRunLevelTable.serTieredLootRunLevels.ElementCount(sizeof(TieredLootRunLevel)) - 1);
    if (nGameBalanceIndex >= 0 && nGameBalanceIndex < (int32)pdefGB->tTieredLootRunLevelTable.serTieredLootRunLevels.ElementCount(sizeof(TieredLootRunLevel)))
    {
        const TieredLootRunLevel & tTieredLootRunLevel = pdefGB->tTieredLootRunLevelTable.ptTieredLootRunLevels[nGameBalanceIndex];
        flDungeonFinderHPScalar = tTieredLootRunLevel.tLootRunValues.flHealthScalar;
        flDungeonFinderDamageScalar = tTieredLootRunLevel.tLootRunValues.flDamageScalar;
        flDungeonFinderXPScalar = tTieredLootRunLevel.tLootRunValues.flXPBonus;
    }

    FastAttribSetValueFloat(ptACDActor->idFastAttrib, ATTRIBUTE_EXPERIENCE_BONUS_PCT, flDungeonFinderXPScalar);
    FastAttribSetValueFloat(ptACDActor->idFastAttrib, ATTRIBUTE_DAMAGE_PERCENT_ALL_FROM_SKILLS, flDungeonFinderDamageScalar);
    FastAttribSetValueFloat(ptACDActor->idFastAttrib, ATTRIBUTE_HITPOINTS_MAX_PERCENT_BONUS, flDungeonFinderHPScalar);
    FastAttribSetValueFloat(ptACDActor->idFastAttrib, ATTRIBUTE_DROPS_NO_LOOT, 1.0f);
    float fMaxHp = ptACDActor->GetHitPointsMaxTotal();
    ptACDActor->SetHitPoints(fMaxHp, TRUE, FALSE);
}

CRefString ActorGetRandomSummonActor(VACDID idACDSummoner)
{
    ActorCommonData * ptACDSummoner = ACDTryToGet(idACDSummoner);
    if (ptACDSummoner)
    {
        SNO snoMonsterActor = MonsterGetRandomSummonActor(ptACDSummoner->tSNOName, SGameGlobalsGet().rng);
        if (snoMonsterActor != SNO_NONE)
            return SNOToString(ACTOR_SNO, snoMonsterActor, FALSE);
    }
    return "";
}

BOOL ShouldThrottleScriptActors()
{
    return ActorShouldThrottleScriptActors();
}

double ActorGetAttribute(VACDID idActor, LPCSTR szAttributeName, LPCSTR szAttribParamName)
{
    double flValue = 0.0;
    Attrib eAttrib = (Attrib)StringToAttrib(szAttributeName);
    if (eAttrib > AXEBADDATA && eAttrib < NUM_ATTRIBUTES)
    {
        ActorCommonData & tACD = ACDGet(idActor);
        AttributeDataType eAttribType = KeyGetDataType(FastAttribKey(eAttrib));
        int32 nParam = (szAttribParamName && szAttribParamName[0]) ? AttribStringToParam(eAttrib, szAttribParamName) : -1;
        FastAttribKey tKey = MakeAttribKey(eAttrib, nParam);
        switch (eAttribType)
        {
        case ATTRIBUTEDT_FLOAT:
            flValue = tACD.AttributesGetFloat(tKey);
            break;
        case ATTRIBUTEDT_INT:
            flValue = tACD.AttributesGetInt(tKey);
            break;
        }
    }
    return flValue;
}

double ActorGetAttribute(VACDID idActor, int32 eAttrib, int32 nParam)
{
    double flValue = 0.0;
    if (eAttrib > AXEBADDATA && eAttrib < NUM_ATTRIBUTES)
    {
        ActorCommonData & tACD = ACDGet(idActor);
        FastAttribKey tKey = MakeAttribKey(eAttrib, nParam);
        AttributeDataType eAttribType = KeyGetDataType(tKey);
        switch (eAttribType)
        {
        case ATTRIBUTEDT_FLOAT:
            flValue = tACD.AttributesGetFloat(tKey);
            break;
        case ATTRIBUTEDT_INT:
            flValue = tACD.AttributesGetInt(tKey);
            break;
        }
    }
    return flValue;
}

void ActorSetAttribute(VACDID idActor, double dValue, int32 eAttrib, int32 nParam)
{
    if (eAttrib > AXEBADDATA && eAttrib < NUM_ATTRIBUTES)
    {
        ActorCommonData & tACD = ACDGet(idActor);
        FastAttribKey tKey = MakeAttribKey(eAttrib, nParam);
        AttributeDataType eAttribType = KeyGetDataType(tKey);
        switch (eAttribType)
        {
        case ATTRIBUTEDT_FLOAT:
            tACD.AttributesSetFloat(tKey, (float)dValue);
            break;
        case ATTRIBUTEDT_INT:
            tACD.AttributesSetInt(tKey, (int32)dValue);
            break;
        }

    }
}

void ActorTriggerPowerAtFeet(ACDID idACD, LPCSTR szPower)
{
    SNO snoPower = SNOFileLookupGUID(POWER_SNO, szPower);
    if (snoPower != SNO_NONE)
    {
        LuaVerifySnosAreReferenced(snoPower, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE);
        Target tTarget;
        ActorCommonData & tACD = ACDGet(idACD);
        WorldPlace placeACD = tACD.GetPlace();
        tTarget.InitializeFromPlace(placeACD);
        PowerStart(idACD, snoPower, tTarget);
    }
}

void ActorTriggerPowerAtTargetsFeet(ACDID idACD, ACDID idACDTarget, LPCSTR szPower)
{
    SNO snoPower = SNOFileLookupGUID(POWER_SNO, szPower);
    if (snoPower != SNO_NONE)
    {
        LuaVerifySnosAreReferenced(snoPower, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE);
        Target tTarget;
        ActorCommonData & tACD = ACDGet(idACDTarget);
        WorldPlace placeACD = tACD.GetPlace();
        tTarget.InitializeFromPlace(placeACD);
        PowerStart(idACD, snoPower, tTarget);
    }
}

void ActorTriggerBrainStancePowerAtTargetsFeet(ACDID idACD, ACDID idACDTarget, LPCSTR szPower)
{
    SNO snoPower = SNOFileLookupGUID(POWER_SNO, szPower);
    if (snoPower == SNO_NONE)
        return;

    LuaVerifySnosAreReferenced(snoPower, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE);
    Target tTarget;
    tTarget.InitializeFromPlace(ACDGet(idACDTarget).GetPlace());
    BrainStanceDoPower((ActorID)ACDGetOwner(idACD), tTarget, snoPower, BRAIN_POWER_START, NULL, NULL, 0, 0);
}

void ActorTriggerPowerAtTarget(ACDID idACD, ACDID idACDTarget, LPCSTR szPower, BOOL bForceTarget)
{
    SNO snoPower = SNOFileLookupGUID(POWER_SNO, szPower);
    if (snoPower != SNO_NONE)
    {
        LuaVerifySnosAreReferenced(snoPower, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE);
        Target tTarget;
        tTarget.InitializeFromACD(idACDTarget, FALSE, FALSE);
        tTarget.bForcedActorTarget = bForceTarget;
        PowerStart(idACD, snoPower, tTarget);
    }
}

void ActorTriggerBrainStancePowerAtTarget(ACDID idACD, ACDID idACDTarget, LPCSTR szPower)
{
    SNO snoPower = SNOFileLookupGUID(POWER_SNO, szPower);
    if (snoPower == SNO_NONE)
        return;

    LuaVerifySnosAreReferenced(snoPower, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE);
    Target tTarget;
    tTarget.InitializeFromACD(idACDTarget, FALSE, FALSE);
    BrainStanceDoPower((ActorID)ACDGetOwner(idACD), tTarget, snoPower, BRAIN_POWER_START, NULL, NULL, 0, 0);
}

BOOL ActorHasAITarget(VACDID idACD, VACDID idACDTarget)
{
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
    if (ptACD == NULL || ptACDTarget == NULL)
        return FALSE;
    ActorID idActor = (ActorID)ptACD->idOwner;
    ActorID idTarget = (ActorID)ptACDTarget->idOwner;
    return BrainAPIActorHasTarget(idActor, idTarget);
}

void ActorSetScriptActor(VACDID idACD, int32 nIndex, VACDID idACDTarget)
{
    ActorCommonData & tACD = ACDGet(idACD);
    FastAttribKey tKey = MakeAttribKey(ATTRIBUTE_SCRIPT_ACTOR_ACDID, nIndex);
    tACD.AttributesSetInt(tKey, idACDTarget);
}

void ActorClearScriptActor(VACDID idACD, int32 nIndex)
{
    ActorCommonData & tACD = ACDGet(idACD);
    FastAttribKey tKey = MakeAttribKey(ATTRIBUTE_SCRIPT_ACTOR_ACDID, nIndex);
    tACD.AttributesSetInt(tKey, ACDID_INVALID);
}

ACDID ActorGetScriptActor(VACDID idACD, int32 nIndex)
{
    ActorCommonData & tACD = ACDGet(idACD);
    ACDID idACDTarget = (ACDID)tACD.AttributesGetInt(MakeAttribKey(ATTRIBUTE_SCRIPT_ACTOR_ACDID, nIndex));
    ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
    if (ptACDTarget)
        return ptACDTarget->id;
    return ACDID_INVALID;
}

void ActorMergeAIState(VACDID idACDSource, LPCSTR szAIStateSNOToMerge, VACDID idACDToMergeOnto, float flDurationInSeconds)
{
    ActorID idActor = (ActorID)ACDGetOwner(idACDToMergeOnto);
    SNO snoAIState = SNOFileLookupGUID(AISTATE_SNO, szAIStateSNOToMerge);
    if (snoAIState == SNO_NONE)
        return;
    uint32 dwDurationTicks = (int32)(flDurationInSeconds * TICKS_PER_SECOND) + 1;
    LuaVerifySnosAreReferenced(snoAIState, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE);
    AIStateMerge(idActor, snoAIState, dwDurationTicks, idACDSource);
}

void ActorCancelPowerBuff(VACDID idACD, LPCSTR szPower, int32 nBuffIndex)
{
    SNO snoPower = SNOFileLookupGUID(POWER_SNO, szPower);
    if (snoPower != SNO_NONE)
    {
        LuaVerifySnosAreReferenced(snoPower, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE);
        BuffID idBuff = PowerAPI_BuffFindOnACD(snoPower, nBuffIndex, ACDGet(idACD), ACDID_INVALID, FALSE);
        if (idBuff != BUFFID_INVALID)
            PowerAPI_BuffFlagForDeletion(idBuff, FALSE, TRUE, 0, TRUE);
    }
}

void ActorTryToFlee(VACDID idACD)
{
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD && ptACD->tSNOName.snoGroup == ACTOR_SNO)
        BrainAPITryToFlee(idACD);
}

void SetSpecialEventFlag(ACDID idPlayer, SpecialEventFlag eEventFlag, BOOL bValue)
{
    ActorCommonData * ptACD = ACDTryToGet(idPlayer);
    if (ptACD == NULL)
        return;
    Player * ptPlayer = PlayerGetByACD(idPlayer);
    if (eEventFlag >= 0 && eEventFlag < SPECIAL_EVENT_FLAG_COUNT && ptPlayer)
    {
        if (bValue)
            ptPlayer->tHeroStateData.tSavedData.uEventFlags |= (1 << eEventFlag);
        else
            ptPlayer->tHeroStateData.tSavedData.uEventFlags &= ~(1 << eEventFlag);
    }
}

BOOL GetSpecialEventFlag(ACDID idPlayer, SpecialEventFlag eEventFlag)
{
    ActorCommonData * ptACD = ACDTryToGet(idPlayer);
    if (ptACD == NULL)
        return FALSE;
    Player * ptPlayer = PlayerGetByACD(idPlayer);
    if (eEventFlag >= 0 && eEventFlag < SPECIAL_EVENT_FLAG_COUNT && ptPlayer)
        return (ptPlayer->tHeroStateData.tSavedData.uEventFlags >> eEventFlag) & 1;
    return FALSE;
}

void TestingAddWorld(LPCSTR szWorld, Act eAct, LPCSTR szQuest, LPCSTR szStep)
{
    LuaScriptErrorHandlerEnter();
    SNO snoQuest = SNOFileLookupGUID(QUEST_SNO, szQuest);
    SNO snoWorld = SNOFileLookupGUID(WORLD_SNO, szWorld);
    int32 nStepUID = -1;
    LuaVerifySnosAreReferenced(snoQuest, snoWorld, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE);
    if (snoQuest != SNO_NONE)
    {
        nStepUID = QuestStepGetUIDByName(snoQuest, szStep);
    }
    GameTesting::AddWorld(szWorld, eAct, snoQuest, nStepUID);
    LuaScriptErrorHandlerExit();
}

void TestingAddDungeonFinder(LPCSTR szDungeonFinder, Act eAct)
{
    LuaScriptErrorHandlerEnter();
    SNO snoDungeonFinder = SNOFileLookupGUID(DUNGEONFINDER_SNO, szDungeonFinder);
    LuaVerifySnosAreReferenced(snoDungeonFinder, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE);
    GameTesting::AddDungeonFinder(szDungeonFinder, eAct);
    LuaScriptErrorHandlerExit();
}

void TestingAdvance()
{
    LuaScriptErrorHandlerEnter();
    GameTesting::Advance();
    LuaScriptErrorHandlerExit();
}

void TestingTriggerSkill()
{
    LuaScriptErrorHandlerEnter();
    GameTesting::TriggerSkill();
    LuaScriptErrorHandlerExit();
}

void TestingSetupPerfSkillTest()
{
    LuaScriptErrorHandlerEnter();
    GameTesting::SetupPerfSkillTest();
    LuaScriptErrorHandlerExit();
}

uint32 TestingGetSampleTime()
{
    return GameTesting::GetSampleTime();
}

uint32 TestingGetWorldAdvanceWaitTime()
{
    return GameTesting::GetWorldAdvanceWaitTime();
}

BOOL TestingLoadingScreenIsEnabled()
{
    return CGameLoadingScreenIsEnabled();
}

void TestingStartSampling()
{
    GameTesting::StartSampling();
}

void TestingStopSampling()
{
    GameTesting::StopSampling();
}

void TestingKillAllMonsters()
{
    Player * ptPlayer = PlayerGetByPlayerIndex(0);
    Actor * ptActor = ActorTryToGet((ActorID)ACDGetOwner(ptPlayer->idPlayerACD));
    if (ptActor)
    {
        LuaScriptErrorHandlerEnter();
        GameTesting::KillAllMonsters(ptActor->id, TRUE, TRUE, 0.0f);
        LuaScriptErrorHandlerExit();
    }
}

void WeatherIntensityOverride(LPCSTR szWorld, const float flIntensity, const float flOverrideDuration)
{
    SNO snoWorld = SNOFileLookupGUID(WORLD_SNO, szWorld);
    if (snoWorld == SNO_NONE)
        return;
    SWorldID idSWorld = WorldCommonFindBySNO(snoWorld, TRUE);
    if (idSWorld == SWORLDID_INVALID)
        return;
    WorldCommonData & tWCD = WorldGetCommonData(idSWorld);
    tWCD.tWorldSyncedData.flWeatherIntensityOverride = flIntensity;
    tWCD.tWorldSyncedData.dwWeatherIntensityOverrideEnd = GameGetServerTime() + Math::FloatToInt(flOverrideDuration * 60.0f);
    SWorldSyncData(idSWorld, NULL);
}

void WeatherOverride(LPCSTR szWorld, LPCSTR szWeather)
{
    SNO snoWorld = SNOFileLookupGUID(WORLD_SNO, szWorld);
    SNO snoWeather = SNOFileLookupGUID(WEATHER_SNO, szWeather);
    SWorldID idSWorld = WorldCommonFindBySNO(snoWorld, TRUE);
    if (idSWorld != SWORLDID_INVALID)
    {
        WorldCommonData & tWCD = WorldGetCommonData(idSWorld);
        tWCD.tWorldSyncedData.snoWeatherOverride = snoWeather;
        SWorldSyncData(idSWorld, NULL);
    }
}

void ClearWeatherOverride(LPCSTR szWorld)
{
    SNO snoWorld = SNOFileLookupGUID(WORLD_SNO, szWorld);
    SWorldID idSWorld = WorldCommonFindBySNO(snoWorld, TRUE);
    if (idSWorld != SWORLDID_INVALID)
    {
        WorldCommonData & tWCD = WorldGetCommonData(idSWorld);
        tWCD.tWorldSyncedData.snoWeatherOverride = SNO_NONE;
        SWorldSyncData(idSWorld, NULL);
    }
}


void WeatherTriggerLightning(VACDID idACDPlayer)
{
    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetSingle(listDestPlayers, idACDPlayer);
    SimpleMessage tMessage;
    MessageSendToClient(listDestPlayers, SMSG_TRIGGER_LIGHTNING, &tMessage, sizeof(tMessage));
}

void PrefetchClient(LPCSTR szGroup, LPCSTR szName)
{
    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetAllConnected(listDestPlayers, -1, FALSE);
    SNONameDataMessage tMessage;
    SNOGroup snoGroup = StringToSNOGroup(szGroup);
    tMessage.snoname.Set(snoGroup, szName);
    MessageSendToClient(listDestPlayers, SMSG_PREFETCH, &tMessage, sizeof(tMessage));
}

int32 GetRandomIntForDate(int32 nMin, int32 nMax, LPCSTR szKey)
{
    if (nMax < nMin)
        return nMin;

    Blizzard::Time::Timestamp tLocalTime = Blizzard::Time::LocalBias(SGameManager::GameGet().ptSGameGlobals->tUTCGameStartTime);
    Blizzard::Time::TimeRec tTimeRec;
    Blizzard::Time::BreakTime(tLocalTime, tTimeRec);

    uint32 dwSeed = tTimeRec.day + tTimeRec.year * 1000 + XHashKey<LPCSTR>(szKey);
    RNG rng;
    RandSetSeed_Internal(rng, dwSeed);
    return RandRange_Internal(rng, nMin, nMax - nMin);
}

void ChangeActs(Act eAct)
{
    SGameChangeActs(eAct, SNO_NONE, TRUE);
}

BOOL IsPlayer(ACDID idACDPlayer)
{
    return ACDIsPlayer(idACDPlayer);
}

BOOL IsMonster(ACDID idACDMonster)
{
    ActorCommonData * ptACDMonster = ACDTryToGet(idACDMonster);
    if (ptACDMonster)
        return ptACDMonster->IsMonster();
    return FALSE;
}

double GizmoPlayAnim(VACDID idACDGizmo, ActorAnimationTag eAnimTag)
{
    ActorCommonData & tACDGizmo = ACDGet(idACDGizmo);
    ActorType eType = tACDGizmo.GetActorType();
    double flTime = 0.0;
    if (eAnimTag != 0 && eAnimTag != -1 && eType == ACTORTYPE_GIZMO)
    {
        uint32 uTicks = AnimPlayerPlayAnimation(tACDGizmo, eAnimTag, -1, FALSE, ANIM_REASON_GIZMO, 0.0f, FALSE, 1.0f);
        flTime = uTicks * (double)OO_TICKS_PER_SECOND;
    }
    return flTime;
}

double ActorGetAnimContactFrameTime(VACDID idACDActor, ActorAnimationTag eAnimTag, int32 nContactFrame, float flSpeedScale)
{
    AnimationTiming tAnimationTiming;
    ActorCommonData & tACDActor = ACDGet(idACDActor);
    if (nContactFrame < 0 || nContactFrame >= 4)
        return 0.0;
    AnimPlayerGetAnimationTiming(tACDActor, eAnimTag, -1, flSpeedScale, tAnimationTiming);
    return tAnimationTiming.dwExecuteTime[nContactFrame] * (double)OO_TICKS_PER_SECOND;
}

void ConductorAdvancePhase()
{
    ScriptInstanceID idScriptInstance = LuaScriptGetExecutingScriptInstance();
    ScriptID idScript = LuaScriptGetInstanceParent(idScriptInstance);
    ConductorInstance * ptConductor = SConductorFindConductorByScriptID(idScript);
    if (ptConductor)
    {
        SGameGlobalsGet();
        SConductorFlagForAdvancement(*ptConductor);
    }
}

void ConductorSkipToPhase(LPCSTR szPhaseName)
{
    ScriptInstanceID idScriptInstance = LuaScriptGetExecutingScriptInstance();
    ScriptID idScript = LuaScriptGetInstanceParent(idScriptInstance);
    ConductorInstance * ptConductor = SConductorFindConductorByScriptID(idScript);
    if (ptConductor)
    {
        SGameGlobalsGet();
        SConductorFlagForPhaseSkip(*ptConductor, szPhaseName);
    }
}

void ConductorStop()
{
    ScriptInstanceID idScriptInstance = LuaScriptGetExecutingScriptInstance();
    ScriptID idScript = LuaScriptGetInstanceParent(idScriptInstance);
    ConductorInstance * ptConductor = SConductorFindConductorByScriptID(idScript);
    if (ptConductor)
    {
        SGameGlobalsGet();
        ptConductor->bFlaggedForDeletion = TRUE;
    }
}

void StartChallengeRiftTimer()
{
    if (GameIsWeeklyChallenge())
    {
        SNO snoTimer = GlobalSNOGet(GLOBALSNO_TIMEDEVENT_TIERED_RIFT_TIMER);
        if (snoTimer != SNO_NONE)
        {
            SGameGlobals & tSGameGlobals = SGameGlobalsGet();
            DungeonFinderOverrides * ptOverrides = &tSGameGlobals.tChallengeRift.tDungeonFinderOverrides;
            STimedEventStart(snoTimer, (uint32)ptOverrides->nWeeklyChallengeTargetMilliseconds / 1000 * 60);
            tSGameGlobals.tDungeonFinderData.snoDungeonFinderTimedEvent = snoTimer;
            SNODataMessage tMessage;
            tMessage.snoData = snoTimer;
            PlayerList listDestPlayers(PlayerListGetElementPool());
            PlayerListGetAllConnected(listDestPlayers, -1, TRUE);
            MessageSendToClient(listDestPlayers, SMSG_DUNGEON_FINDER_SET_TIMEDEVENT, &tMessage, sizeof(tMessage));
        }
    }
}

void TimerModifyExpirationTime(LPCSTR szSNOName, float flSeconds)
{
    SNO snoTimer = SNOFileLookupGUID(TIMEDEVENT_SNO, szSNOName);
    if (snoTimer != SNO_NONE)
    {
        LuaVerifySnosAreReferenced(snoTimer, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE);
        STimedEventModifyExpirationTime(snoTimer, Math::FloatToInt(flSeconds * 60.0f));
    }
}

void QuestLoad(LPCSTR szQuestName, LPCSTR szLevelArea)
{
    SNO snoQuest = SNOFileLookupGUID(QUEST_SNO, szQuestName);
    SNO snoLevelArea = SNO_NONE;
    if (snoQuest == SNO_NONE)
        return;
    {
        ScopedQuestDefinitionLockR pdefQuest(snoQuest);
        if (pdefQuest->eQuestType == QUEST_TYPE_MAIN_QUEST)
            return;
    }
    if (szLevelArea && szLevelArea[0])
    {
        snoLevelArea = SNOFileLookupGUID(LEVELAREA_SNO, szLevelArea);
        if (snoLevelArea == SNO_NONE)
            return;
    }
    QuestsLoad(snoQuest, snoLevelArea);
    SQuestsSendUpdateToAllPlayers(snoQuest);
}

void QuestReset(LPCSTR szQuestName)
{
    SNO snoQuest = SNOFileLookupGUID(QUEST_SNO, szQuestName);
    if (snoQuest == SNO_NONE)
        return;
    {
        ScopedQuestDefinitionLockR pdefQuest(snoQuest);
        if (pdefQuest->eQuestType == QUEST_TYPE_MAIN_QUEST)
            return;
    }
    QuestInstance * ptQuestInstance = QuestsGetQuestInstance(snoQuest, ACDID_INVALID);
    if (ptQuestInstance)
        SQuestsResetQuest(*ptQuestInstance);
}

BOOL QuestIsAssigned(LPCSTR szQuestName, ACDID idPlayer)
{
    SNO snoQuest = SNOFileLookupGUID(QUEST_SNO, szQuestName);
    QuestInstance * ptQuestInstance = QuestsGetQuestInstance(snoQuest, idPlayer);
    if (ptQuestInstance && ptQuestInstance->nCurrentStepUID != -1)
        return TRUE;

    if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
    {
    }
    return FALSE;
}

BOOL QuestIsActive(LPCSTR szQuestName, ACDID idPlayer)
{
    SNO snoQuest = SNOFileLookupGUID(QUEST_SNO, szQuestName);
    QuestInstance * ptQuestInstance = QuestsGetQuestInstance(snoQuest, idPlayer);
    if (ptQuestInstance && ptQuestInstance->eInstanceState == QUEST_INSTANCE_ACTIVE)
        return TRUE;

    if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
    {
    }
    return FALSE;
}

BOOL QuestIsComplete(ACDID idACDPlayer, LPCSTR szQuestName)
{
    SNO snoQuest = SNOFileLookupGUID(QUEST_SNO, szQuestName);
    QuestInstance * ptQuestInstance = QuestsGetQuestInstance(snoQuest, idACDPlayer);
    if (ptQuestInstance && ptQuestInstance->eInstanceState == QUEST_INSTANCE_COMPLETED)
        return TRUE;

    if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
    {
    }
    return FALSE;
}

void RegisterQuestActor(ACDID idActor, BOOL bEnable)
{
    ActorCommonData * ptACD = ACDTryToGet(idActor);
    if (ptACD)
        ptACD->AttributesSetInt(ATTRIBUTE_QUEST_MONSTER, bEnable);
}

BOOL QuestRangeValid(LPCSTR szQuestRangeName)
{
    SNO snoQuestRange = SNOFileLookupGUID(QUESTRANGE_SNO, szQuestRangeName);
    if (snoQuestRange != SNO_NONE)
    {
        LuaVerifySnosAreReferenced(snoQuestRange, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE);
        return QuestRangeEvaluate(snoQuestRange, NULL);
    }
    return FALSE;
}

void ActorSetQuestRange(VACDID idACD, LPCSTR szQuestRangeName)
{
    SNO snoQuestRange = SNOFileLookupGUID(QUESTRANGE_SNO, szQuestRangeName);
    if (snoQuestRange != SNO_NONE)
    {
        LuaVerifySnosAreReferenced(snoQuestRange, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE);
        ActorCommonData & tACD = ACDGet(idACD);
        if (tACD.GetGizmoType() == ACTORGIZMO_TOWN_PORTAL)
        {
            tACD.AttributesSetInt(ATTRIBUTE_QUEST_RANGE, snoQuestRange);
            ActorID idActor = (ActorID)tACD.idOwner;
            ActorAddToQuestLifespanActors(idActor);
        }
    }
}

MonsterType ActorGetMonsterType(ACDID idACD)
{
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD && ptACD->IsMonster())
        return MonsterGetMonsterType(ptACD->tSNOName);
    return -1;
}

MonsterFamily ActorGetMonsterFamily(ACDID idACD)
{
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD && ptACD->IsMonster())
        return MonsterGetMonsterFamily(ptACD->tSNOName);
    return -1;
}

ActorIDArray * ActorGetPackMembers(VACDID idActor)
{
    ActorIDArray & tArrACDs = LuaScriptGetThreadSafeActorIDArray();
    tArrACDs.nNumActorIDs = 0;
    tArrACDs.bWantAddRef = TRUE;
    ActorCommonData & tACD = ACDGet(idActor);
    ACDPack * ptACDPack = ACDPackTryToGet(tACD.idACDPack);
    if (ptACDPack)
    {
        const ACDIDListPooled * ptMembers = ptACDPack->GetMemberList();
        POS pos = ptMembers->GetHeadPosition();
        while (pos)
        {
            ACDID idACD = (ACDID)ptMembers->GetNext(pos);
            if (ACDTryToGet(idACD))
            {
                tArrACDs.arActorIDs[tArrACDs.nNumActorIDs] = idACD;
                if (++tArrACDs.nNumActorIDs == 64)
                    break;
            }
        }
    }
    return &tArrACDs;
}

ACDID ActorGetPackLeader(VACDID idActor)
{
    ActorCommonData & tACD = ACDGet(idActor);
    ACDPack * ptACDPack = ACDPackTryToGet(tACD.idACDPack);
    if (ptACDPack == NULL)
        return ACDID_INVALID;
    ACDID idACDLeader = ptACDPack->GetLeader();
    ActorCommonData * ptACDLeader = ACDTryToGet(idACDLeader);
    if (ptACDLeader == NULL)
        return ACDID_INVALID;
    LuaScriptInstanceAddRefACD(LuaScriptGetExecutingScriptInstance(), idACDLeader);
    return idACDLeader;
}

void ActorAddToPack(VACDID idPackMember, VACDID idActor)
{
    ActorCommonData & tACDPackMember = ACDGet(idPackMember);
    ActorCommonData & tACDActor = ACDGet(idActor);
    ACDPack * ptACDPack = ACDPackTryToGet(tACDPackMember.idACDPack);
    if (!ptACDPack)
    {
        ptACDPack = ACDPackTryToGet(ACDPackNew());
        ptACDPack->AddMember(tACDPackMember);
    }
    ACDPack * ptACDOldPack = ACDPackTryToGet(tACDActor.idACDPack);
    if (ptACDOldPack)
        ptACDOldPack->RemoveMember(tACDActor);
    ptACDPack->AddMember(tACDActor);
}

void ActorRemoveFromPack(VACDID idActor)
{
    ActorCommonData & tACDActor = ACDGet(idActor);
    ACDPack * ptACDPack = ACDPackTryToGet(tACDActor.idACDPack);
    if (ptACDPack)
        ptACDPack->RemoveMember(tACDActor);
}

void ActorSetDeletesInsteadOfInactive(VACDID idActor, BOOL bValue)
{
    ActorCommonData * ptACD = ACDTryToGet(idActor);
    if (ptACD)
        ActorFlagSet((ActorID)ptACD->idOwner, ACTOR_DELETES_INSTEAD_OF_INACTIVE_BIT, bValue);
}


void CameraTriggerFade()
{
    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetAllConnected(listDestPlayers, -1, FALSE);
    SimpleMessage tMessage;
    MessageSendToClient(listDestPlayers, SMSG_CAMERA_TRIGGER_FADE, &tMessage, sizeof(tMessage));
}


void CameraTriggerFadeToBlack(BOOL bStayFaded)
{
    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetAllConnected(listDestPlayers, -1, FALSE);
    BoolDataMessage tMessage;
    tMessage.bVal = bStayFaded;
    MessageSendToClient(listDestPlayers, SMSG_CAMERA_TRIGGER_FADE_TO_BLACK, &tMessage, sizeof(tMessage));
}


void CameraTriggerInstantFadeToBlack()
{
    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetAllConnected(listDestPlayers, -1, FALSE);
    SimpleMessage tMessage;
    MessageSendToClient(listDestPlayers, SMSG_CAMERA_TRIGGER_INSTANT_FADE_TO_BLACK, &tMessage, sizeof(tMessage));
}


void CameraTriggerFadeFromBlack()
{
    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetAllConnected(listDestPlayers, -1, FALSE);
    SimpleMessage tMessage;
    MessageSendToClient(listDestPlayers, SMSG_CAMERA_TRIGGER_FADE, &tMessage, sizeof(tMessage));
}

void CameraFocusClear(BOOL bInstant, float flDuration)
{
    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetAllInGame(listDestPlayers, -1, FALSE, -1);
    CameraFocusMessage tFocusMessage;
    tFocusMessage.ann = ANN_INVALID;
    tFocusMessage.bSnap = bInstant;
    tFocusMessage.flDuration = flDuration;
    MessageSendToClient(listDestPlayers, SMSG_CAMERA_FOCUS_ACD, &tFocusMessage, sizeof(tFocusMessage));
}

void CameraSetZoom(float flZoom, BOOL bSnap, float flDuration)
{
    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetAllInGame(listDestPlayers, -1, FALSE, -1);
    CameraZoomMessage tCameraZoomMessage;
    tCameraZoomMessage.flZoom = flZoom < -1.0f ? -1.0f : (flZoom > 1.0f ? 1.0f : flZoom);
    tCameraZoomMessage.flDuration = flDuration;
    tCameraZoomMessage.bSnap = bSnap;
    MessageSendToClient(listDestPlayers, SMSG_CAMERA_ZOOM, &tCameraZoomMessage, sizeof(tCameraZoomMessage));
}

void SetCameraOrbit(ACDID idACDPlayer, ACDID idACDToOrbit)
{
    ActorCommonData * ptACDToOrbit = ACDTryToGet(idACDToOrbit);
    if (!ptACDToOrbit)
        return;
    Player * ptPlayer = PlayerGetByACD(idACDPlayer);
    if (!ptPlayer)
        return;
    PlayerList listDestPlayers(PlayerListGetElementPool());
    listDestPlayers.AddTail_Tracked(ptPlayer);
    PlayerSetCameraOrbitMessage tCameraOrbitMessage;
    tCameraOrbitMessage.wpToOrbit = ptACDToOrbit->GetPlace();
    MessageSendToClient(listDestPlayers, SMSG_SET_CAMERA_ORBIT, &tCameraOrbitMessage, sizeof(tCameraOrbitMessage));
}

void SetCameraDefaults(ACDID idACDPlayer)
{
    Player * ptPlayer = PlayerGetByACD(idACDPlayer);
    if (ptPlayer)
    {
        PlayerList listDestPlayers(PlayerListGetElementPool());
        listDestPlayers.AddTail_Tracked(ptPlayer);
        PlayerSetCameraDefaultsMessage tCameraSetDefaultsMessage;
        MessageSendToClient(listDestPlayers, SMSG_SET_CAMERA_DEFAULTS, &tCameraSetDefaultsMessage, sizeof(tCameraSetDefaultsMessage));
    }
}

void Console_SetCameraDefaults(ACDID idACDPlayer, BOOL bForce)
{
    Player * ptPlayer = PlayerGetByACD(idACDPlayer);
    if (ptPlayer)
    {
        PlayerList listDestPlayers(PlayerListGetElementPool());
        listDestPlayers.AddTail_Tracked(ptPlayer);
        PlayerConsoleSetCameraDefaultsMessage tCameraSetDefaultsMessage;
        tCameraSetDefaultsMessage.bForce = bForce;
        MessageSendToClient(listDestPlayers, SMSG_CONSOLE_SET_CAMERA_DEFAULTS, &tCameraSetDefaultsMessage, sizeof(tCameraSetDefaultsMessage));
    }
}

void BossZoom(float flZoom, ACDID idACDPlayer, float flZoomSpeed)
{
    PlayerList listDestPlayers(PlayerListGetElementPool());
    if (idACDPlayer != ACDID_INVALID)
        PlayerListGetSingle(listDestPlayers, idACDPlayer);
    else
        PlayerListGetAllInGame(listDestPlayers, -1, FALSE, -1);
    BossZoomMessage tBossZoomMessage;
    tBossZoomMessage.flZoom = flZoom;
    tBossZoomMessage.flZoomSpeed = flZoomSpeed;
    MessageSendToClient(listDestPlayers, SMSG_BOSS_ZOOM, &tBossZoomMessage, sizeof(tBossZoomMessage));
}

void CameraSetYaw(float flAngle, BOOL bSnap, float flDuration)
{
    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetAllInGame(listDestPlayers, -1, FALSE, -1);
    CameraYawMessage tCameraYawMessage;
    tCameraYawMessage.aAngle = Math::DegToAngle(flAngle);
    tCameraYawMessage.bSnap = bSnap;
    tCameraYawMessage.flDuration = flDuration;
    MessageSendToClient(listDestPlayers, SMSG_CAMERA_YAW, &tCameraYawMessage, sizeof(tCameraYawMessage));
}


int32 MinimapAddPing(ACDID idActor, float flDuration, LPCSTR szTexture, ACDID idPlayer, BOOL bCanBecomeArrow)
{
    ActorCommonData * ptACD = ACDTryToGet(idActor);
    if (ptACD == NULL || !ptACD->IsInWorld())
        return PINGID_INVALID;
    uint32 dwDuration = Math::FloatToInt(flDuration * 60.0f); // DWARF decl 11435, before snoTexture 11437
    SNO snoTexture = SNO_NONE;
    if (szTexture && szTexture[0])
    {
        snoTexture = SNOFileLookupGUID(TEXTURE_SNO, szTexture);
        if (snoTexture == SNO_NONE)
            return PINGID_INVALID;
    }

    OnlineService::HeroId idPlayerHeroId = OnlineService::INVALID_HERO_ID;
    if (idPlayer != ACDID_INVALID)
    {
        Player * ptPlayer = PlayerGetByACD(idPlayer);
        if (ptPlayer == NULL)
            return PINGID_INVALID;
        idPlayerHeroId = ptPlayer->uHeroId;
    }


    return Minimap::PingLocation(ptACD->GetPlace(), dwDuration, idPlayerHeroId, snoTexture, bCanBecomeArrow, (uint32)-1, SNO_NONE, SNO_NONE, 0.0f, SNO_NONE);
}

void MinimapRemovePing(int32 idPing)
{
    Minimap::RemovePing(idPing);
}

void MinimapAddActorPing(VACDID idACD)
{
    Minimap::ActorAutoPing(idACD, -1);
}

void MinimapRemoveActorPing(ACDID idActor)
{
    ActorCommonData * ptACD = ACDTryToGet(idActor);
    if (ptACD)
    {
        PingID idPing = Minimap::SNOGetAutoPing(ptACD->tSNOName.snoHandle);
        Minimap::RemovePing(idPing);
    }
}

BOOL ShowTutorial(LPCSTR szTutorialName, const ACDID idACDPlayer, LPCSTR szUIAnchorName, const ACDID idACDTarget)
{
    SNO snoTutorial = SNOFileLookupGUID(TUTORIAL_SNO, szTutorialName);
    if (SNOFileExists(TUTORIAL_SNO, snoTutorial) && ACDIsPlayer(idACDPlayer))
    {
        Player * ptPlayer = PlayerGetByACD(idACDPlayer);
        return Tutorial::SendTutorialToClient(snoTutorial, ptPlayer, szUIAnchorName, idACDTarget);
    }
    return FALSE;
}

BOOL PlayerHasPlayedAdventureModeTutorial(ACDID idACDPlayer)
{
    Player * ptPlayer = PlayerGetByACD(idACDPlayer);
    if (ptPlayer)
        return (ptPlayer->tAccount.uDigestFlags & (1 << D3::Account::Digest_Flags_ADVENTURE_MODE_TUTORIAL_PLAYED)) != 0;
    return FALSE;
}


void PlayerPlayAdventureModeTutorial(ACDID idACDPlayer)
{
    Player * ptPlayer = PlayerGetByACD(idACDPlayer);
    if (ptPlayer)
    {
        PlayerList listDestPlayers(PlayerListGetElementPool());
        PlayerListGetSingle(listDestPlayers, *ptPlayer);
        SimpleMessage tMessage;
        MessageSendToClient(listDestPlayers, SMSG_PLAY_OPEN_WORLD_TUTORIAL, &tMessage, sizeof(tMessage));
    }
}

void PlayerEnableFreeEnchant(ACDID idActorPlayer)
{
    Player * ptPlayer = PlayerGetByACD(idActorPlayer);
    ActorCommonData * ptACDPlayer = ACDTryToGet(idActorPlayer);
    if (ptPlayer && ptACDPlayer)
    {
        ptPlayer->tHeroStateData.uPlayerFlags |= (1u << PLAYERFLAG_FREE_ENCHANT);
        ptACDPlayer->MarkHeroStatePlayerFlagsChanged();
    }
}

void AchievementSendSetDungeonEvent(VACDID idActorPlayer, LPCSTR szAchievementEvent, int32 uQuantity)
{
    Player * ptPlayer = PlayerGetByACD(idActorPlayer);
    if (ptPlayer)
        AchievementSendEvent(idActorPlayer, szAchievementEvent, uQuantity);
}

void AchievementSendCursedChestCompletedEvent(VACDID idACDPlayer, int32 nNumKilled)
{
    Player * ptPlayer = PlayerGetByACD(idACDPlayer);
    if (ptPlayer)
    {
        Achievement::EvaluationData tEvalData;
        tEvalData.SetPlayer(ptPlayer);
        tEvalData.SetCursedChestKillCount(nNumKilled);
        tEvalData.EvaluateEvent(ptPlayer, Achievement::ACH_EVENT_CURSED_CHEST_EVENT_COMPLETED, 0, 1);
    }
}

double PlayerStatCounterGet(ACDID idACDPlayer, StatCounterType eType, int32 nParam0, int32 nParam1)
{
    Player * ptPlayer = PlayerGetByACD(idACDPlayer);
    if (eType < STAT_COUNTER_BEGIN_PLAYER_STAT_COUNTERS && ptPlayer)
        return SGameStatsPlayerCounterFetch(ptPlayer, eType, nParam0, nParam1);
    return 0.0;
}

double PlayerCurrentLifeStatCounterGet(ACDID idACDPlayer, StatCounterType eType, int32 nParam0, int32 nParam1)
{
    Player * ptPlayer = PlayerGetByACD(idACDPlayer);
    if (eType < STAT_COUNTER_BEGIN_PLAYER_STAT_COUNTERS && ptPlayer)
        return SGameStatsPlayerCurrentLifeCounterFetch(ptPlayer, eType, nParam0, nParam1);
    return 0.0;
}

ActorIDArray * PlayerStatCounterGetHighestPlayers(StatCounterType eType, int32 nParam0, int32 nParam1, Team eTeam)
{
    int32 nCurrentMax = 0;
    ActorIDArray & tArrACDs = LuaScriptGetThreadSafeActorIDArray();
    tArrACDs.nNumActorIDs = 0;
    tArrACDs.bWantAddRef = FALSE;
    if (eType < STAT_COUNTER_BEGIN_PLAYER_STAT_COUNTERS)
    {
        for (const Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
        {
            if (eTeam != TEAM_INVALID && ptPlayer->eTeam != eTeam)
                continue;
            int32 nPlayerValue = (int32)SGameStatsPlayerCounterFetch(ptPlayer, eType, nParam0, nParam1);
            if (nPlayerValue > nCurrentMax)
            {
                tArrACDs.arActorIDs[0] = ptPlayer->idPlayerACD;
                tArrACDs.nNumActorIDs = 1;
                nCurrentMax = nPlayerValue;
            }
            else if (nPlayerValue == nCurrentMax)
            {
                tArrACDs.arActorIDs[tArrACDs.nNumActorIDs] = ptPlayer->idPlayerACD;
                tArrACDs.nNumActorIDs++;
            }
        }
    }
    return &tArrACDs;
}

ActorIDArray * PlayerStatCounterGetLowestPlayers(StatCounterType eType, int32 nParam0, int32 nParam1, Team eTeam)
{
    int32 nCurrentMin = INT32_MAX;
    ActorIDArray & tArrACDs = LuaScriptGetThreadSafeActorIDArray();
    tArrACDs.nNumActorIDs = 0;
    tArrACDs.bWantAddRef = FALSE;
    if (eType < STAT_COUNTER_BEGIN_PLAYER_STAT_COUNTERS)
    {
        for (const Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
        {
            if (eTeam != TEAM_INVALID && ptPlayer->eTeam != eTeam)
                continue;
            int32 nPlayerValue = (int32)SGameStatsPlayerCounterFetch(ptPlayer, eType, nParam0, nParam1);
            if (nPlayerValue < nCurrentMin)
            {
                tArrACDs.arActorIDs[0] = ptPlayer->idPlayerACD;
                tArrACDs.nNumActorIDs = 1;
                nCurrentMin = nPlayerValue;
            }
            else if (nPlayerValue == nCurrentMin)
            {
                tArrACDs.arActorIDs[tArrACDs.nNumActorIDs] = ptPlayer->idPlayerACD;
                tArrACDs.nNumActorIDs++;
            }
        }
    }
    return &tArrACDs;
}

GBID GetGBIDFromString(LPCSTR szString)
{
    return StringToGBID(szString);
}

CRefString ItemGBIDToString(int32 nGBID)
{
    GBHandle hGB(GB_ITEMS, nGBID);
    return GBIDToString(hGB, FALSE);
}

void MonsterRemoveAffixFromSelection(GBID gbidAffix)
{
    AffixSelectionList * ptList = ActorAIGetTempAffixList();
    if (ptList == NULL)
        return;
    int32 * pnAffixWeightTotal = ActorAIGetTempAffixWeightTotal();
    if (pnAffixWeightTotal == NULL)
        return;
    POS pos = ptList->GetHeadPosition();
    while (pos)
    {
        POS posLast = pos;
        AffixSelectionEntry & tEntry = ptList->GetNext(pos);
        if (tEntry.gbidAffix == gbidAffix)
        {
            *pnAffixWeightTotal -= tEntry.nFrequencyWeight;
            ptList->RemoveAt(posLast);
        }
    }
}

void MonsterSetAffixFrequency(GBID gbidAffix, int32 nNewFrequencyWeight)
{
    AffixSelectionList * ptList = ActorAIGetTempAffixList();
    if (!ptList)
        return;
    int32 * pnAffixWeightTotal = ActorAIGetTempAffixWeightTotal();
    if (!pnAffixWeightTotal)
        return;
    POS pos = ptList->GetHeadPosition();
    while (pos)
    {
        AffixSelectionEntry & tEntry = ptList->GetNext(pos);
        if (tEntry.gbidAffix == gbidAffix)
        {
            int32 nDelta = nNewFrequencyWeight - tEntry.nFrequencyWeight;
            tEntry.nFrequencyWeight = nNewFrequencyWeight;
            *pnAffixWeightTotal += nDelta;
            return;
        }
    }
}

int32 MonsterGetAffixFrequency(GBID gbidAffix)
{
    AffixSelectionList * ptList = ActorAIGetTempAffixList();
    if (ptList)
    {
        POS pos = ptList->GetHeadPosition();
        while (pos)
        {
            AffixSelectionEntry & tEntry = ptList->GetNext(pos);
            if (tEntry.gbidAffix == gbidAffix)
                return tEntry.nFrequencyWeight;
        }
    }
    return 0;
}

float MonsterGetExperienceScalar(ACDID idACDMonster)
{
    ActorCommonData * ptACDMonster = ACDTryToGet(idACDMonster);
    if (ptACDMonster == NULL || !ptACDMonster->IsMonster())
        return 0.0f;
    ScopedMonsterDefinitionLockR pdefMonster(ptACDMonster->GetMonsterSNO());
    return pdefMonster->pctAttributeModifiers[GBA_EXPERIENCE];
}

BOOL BossEncounterIsComplete(LPCSTR szBossEncounter)
{
    SNO snoBossEncounter = SNOFileLookupGUID(BOSS_ENCOUNTER_SNO, szBossEncounter);
    if (snoBossEncounter != SNO_NONE)
        return BossEncounterManager::IsEncounterCompleted(snoBossEncounter);
    return FALSE;
}

ACDID GetPrimaryPlayer()
{
    for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {
        if (ptPlayer->bIsPrimaryPlayerServerOnly)
            return ptPlayer->idPlayerACD;
    }
    return ACDID_INVALID;
}

struct GemCountInfo {
    GemType eType;
    int32 * pCount;
};
//
static void sGemCountCallback(ACDID idACD, InventorySlotType eSlot, void * pUserData)
{
    GemCountInfo * ptInfo = (GemCountInfo *)pUserData;
    const ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD == NULL)
        return;
    if (eSlot != SLOT_NONE && eSlot != SLOT_ITEM_SOCKETS && (eSlot < SLOT_PLAYER_BODYSLOT_START || eSlot >= SLOT_PLAYER_BODYSLOT_END))
        return;
    if (ItemIsGem(*ptACD))
    {
        ScopedItemDefinitionLockR pdefGem(ptACD->hGB.gbid);
        if (pdefGem->eGemType == ptInfo->eType || ptInfo->eType == 0)
            (*ptInfo->pCount)++;
    }
    if (ptACD->HasInventory())
        ACDInventoryIterateItems(*ptACD, SLOT_NONE, 0, sGemCountCallback, pUserData);
}

int32 ActorGetGemCount(VACDID idACD, GemType eType, InventorySlotType eSlot)
{
    int32 nCount = 0;
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD && (eSlot == SLOT_NONE || eSlot == SLOT_ITEM_SOCKETS || (eSlot >= SLOT_PLAYER_BODYSLOT_START && eSlot < SLOT_PLAYER_BODYSLOT_END)))
    {
        GemCountInfo tInfo;
        tInfo.eType = eType;
        tInfo.pCount = &nCount;
        ACDInventoryIterateItems(*ptACD, eSlot, 0, sGemCountCallback, &tInfo);
    }
    return nCount;
}

static void sLegendaryGemCountCallback(ACDID idACD, InventorySlotType eSlot, void * pUserData)
{
    const ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD == NULL)
        return;
    if (eSlot != SLOT_NONE && eSlot != SLOT_ITEM_SOCKETS && (eSlot < SLOT_PLAYER_BODYSLOT_START || eSlot >= SLOT_PLAYER_BODYSLOT_END))
        return;
    int32 * pCount = (int32 *)pUserData;
    if (ItemIsUpgradeableJewel(*ptACD))
        (*pCount)++;
    if (ptACD->HasInventory())
        ACDInventoryIterateItems(*ptACD, SLOT_NONE, 0, sLegendaryGemCountCallback, pUserData);
}

int32 ActorGetLegendaryGemCount(VACDID idACD, InventorySlotType eSlot)
{
    int32 nCount = 0;
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD && (eSlot == SLOT_NONE || eSlot == SLOT_ITEM_SOCKETS || (eSlot >= SLOT_PLAYER_BODYSLOT_START && eSlot < SLOT_PLAYER_BODYSLOT_END)))
        ACDInventoryIterateItems(*ptACD, eSlot, 0, sLegendaryGemCountCallback, &nCount);
    return nCount;
}

struct SpecificGemCountInfo {
    GBID gbidGem;
    int32 * pCount;
};
//
static void sSpecificGemCountCallback(ACDID idACD, InventorySlotType eSlot, void * pUserData)
{
    SpecificGemCountInfo * ptInfo = (SpecificGemCountInfo *)pUserData;
    const ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD == NULL)
        return;
    if (eSlot != SLOT_NONE && eSlot != SLOT_ITEM_SOCKETS && (eSlot < SLOT_PLAYER_BODYSLOT_START || eSlot >= SLOT_PLAYER_BODYSLOT_END))
        return;
    if (ptACD->hGB.gbid == ptInfo->gbidGem)
        (*ptInfo->pCount)++;
    if (ptACD->HasInventory())
        ACDInventoryIterateItems(*ptACD, SLOT_NONE, 0, sSpecificGemCountCallback, pUserData);
}

int32 ActorGetSpecificGemCount(VACDID idACD, LPCSTR szGemName, InventorySlotType eSlot)
{
    int32 nCount = 0;
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD && (eSlot == SLOT_NONE || eSlot == SLOT_ITEM_SOCKETS || (eSlot >= SLOT_PLAYER_BODYSLOT_START && eSlot < SLOT_PLAYER_BODYSLOT_END)))
    {
        SpecificGemCountInfo tInfo;
        tInfo.gbidGem = StringToGBID(szGemName);
        tInfo.pCount = &nCount;
        ACDInventoryIterateItems(*ptACD, eSlot, 0, sSpecificGemCountCallback, &tInfo);
    }
    return nCount;
}

int32 ConversationVarGet(LPCSTR szVar)
{
    if (!szVar)
        return 0;

    GBID gbidVar = StringToGBID(szVar);
    SGame & tSGame = SGameManager::GameGet();
    if (gbidVar == GBID_NONE || !tSGame.ptSGameGlobals)
        return 0;

    int32 nValue;
    if (tSGame.ptSGameGlobals->tConversationVarMap.Lookup(gbidVar, nValue))
        return nValue;
    return 0;
}

void ConversationVarSet(LPCSTR szVar, int32 nValue)
{
    if (!szVar)
        return;

    GBID gbidVar = StringToGBID(szVar);
    SGame & tSGame = SGameManager::GameGet();
    if (gbidVar == GBID_NONE || !tSGame.ptSGameGlobals)
        return;

    tSGame.ptSGameGlobals->tConversationVarMap.SetAt_Tracked(gbidVar, nValue);
}

float ActorGetRareScaleFactor(ACDID idACD)
{
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD)
    {
        ScopedActorDefinitionLockR pdefActor(ptACD->snoActor);
        if (pdefActor)
            return TagMapGetFloat(pdefActor->hTagMap, TAG_ACTOR_RARE_SCALE_FACTOR);
    }
    return 1.0f;
}

void DungeonFinderCreate(SNO snoDungeonFinder)
{
    SWorldDungeonFinderCreate(snoDungeonFinder);
}

void DungeonFinderCreate(LPCSTR szDungeonFinder)
{
    SNO snoDungeonFinder = SNOFileLookupGUID(DUNGEONFINDER_SNO, szDungeonFinder);
    if (snoDungeonFinder != SNO_NONE)
        SWorldDungeonFinderCreate(snoDungeonFinder);
}

void DungeonFinderAddLevel(LPCSTR szWorld, LPCSTR szPopulation, float flSpawnGroupDensityMultiplier, float flEliteSpawnGroupDensityMultipler, float flGizmoDensityMultiplier, LPCSTR szWeather)
{
    SNO snoWorld = SNOFileLookupGUID(WORLD_SNO, szWorld);
    if (snoWorld == SNO_NONE)
        return;

    SNO snoWeather = SNO_NONE;
    if (szWeather && szWeather[0])
    {
        snoWeather = SNOFileLookupGUID(WEATHER_SNO, szWeather);
        if (snoWeather == SNO_NONE)
            return;
    }

    const WorldDefinition * ptWorldDef = DefinitionLockR<WorldDefinition>(snoWorld);
    if ((ptWorldDef->uFlags & (1 << 8)) && SWorldGetNumDungeonFinderLevels() < 10)
    {
        SWorldAddDungeonFinderLevel(snoWorld, szPopulation, flSpawnGroupDensityMultiplier, flEliteSpawnGroupDensityMultipler, flGizmoDensityMultiplier, snoWeather);
    }
    DefinitionUnlockR<WorldDefinition>(ptWorldDef);
}

void DungeonFinderSetTimedEvent(LPCSTR szSNOName)
{
    SNO snoTimer = SNOFileLookupGUID(TIMEDEVENT_SNO, szSNOName);
    SGameGlobals & tSGameGlobals = SGameGlobalsGet();
    tSGameGlobals.tDungeonFinderData.snoDungeonFinderTimedEvent = snoTimer;
    SNODataMessage tMessage;
    tMessage.snoData = snoTimer;
    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetAllConnected(listDestPlayers, -1, TRUE);
    MessageSendToClient(listDestPlayers, SMSG_DUNGEON_FINDER_SET_TIMEDEVENT, &tMessage, sizeof(tMessage));
}

void DungeonFinderSetBossPortal(LPCSTR szBossPortalActor)
{
    SNO snoActor = SNOFileLookupGUID(ACTOR_SNO, szBossPortalActor);
    if (snoActor != SNO_NONE && ActorGetGizmoType(snoActor) == ACTORGIZMO_BOSS_PORTAL)
    {
        SNO snoBossEncounter = ActorDefinitionGetTagMapValueSNO(TAG_ACTOR_GIZMO_BOSS_ENCOUNTER_SNO, snoActor);
        if (snoBossEncounter != SNO_NONE)
            SWorldSetDungeonFinderBossEncounter(snoBossEncounter);
    }
}

BOOL DungeonFinderIsEmpty()
{
    return SWorldDungeonFinderIsEmpty();
}

void DungeonFinderClosingTriggered(float flSeconds)
{
    DungeonFinderClosingMessage tMessage;
    tMessage.nTickClosing = GameGetServerTime() + Math::FloatToInt(flSeconds * 60.0f);
    tMessage.nRiftLevel = NephalemRiftManager::GetActiveRiftLevel();
    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetAllConnected(listDestPlayers, -1, FALSE);
    MessageSendToClient(listDestPlayers, SMSG_DUNGEON_FINDER_CLOSING, &tMessage, sizeof(tMessage));
}

void SetDungeonClosingTriggered(float flSeconds)
{
    IntDataMessage tMessage;
    tMessage.nData = GameGetServerTime() + Math::FloatToInt(flSeconds * 60.0f);
    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetAllConnected(listDestPlayers, -1, FALSE);
    MessageSendToClient(listDestPlayers, SMSG_SET_DUNGEON_CLOSING, &tMessage, sizeof(tMessage));
    SetEncounterManager::SetDungeonClosing(TRUE);
}

void GreedRealmClosingTriggered(float flSeconds)
{
    IntDataMessage tMessage;
    tMessage.nData = GameGetServerTime() + Math::FloatToInt(flSeconds * 60.0f);
    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetAllConnected(listDestPlayers, -1, FALSE);
    MessageSendToClient(listDestPlayers, SMSG_GREED_REALM_CLOSING, &tMessage, sizeof(tMessage));
    SWorldResetGreedRealm(flSeconds);
}

void DungeonFinderReset()
{
    SWorldResetDungeonFinder();
}

int32 DungeonFinderGetTieredLevel()
{
    return NephalemRiftManager::GetActiveRiftLevel();
}

int32 DungeonFingerGetNumParticipants()
{
    return NephalemRiftManager::RiftPlayerCount();
}

BOOL DungeonFinderIsComplete()
{
    return QuestDungeonFinderIsComplete();
}

void DungeonFinderPickEntrancePortalLevelArea()
{
    SGamePickDungeonFinderLevelArea();
}

CRefString DungeonFinderGetEntrancePortalLevelArea()
{
    GameCommonData & tGCD = GameGetActiveGameCommonData();
    if (tGCD.tGameSyncedData.snoDungeonFinderLevelArea != SNO_NONE)
        return SNOToString(LEVELAREA_SNO, tGCD.tGameSyncedData.snoDungeonFinderLevelArea, FALSE);
    return "";
}

SNO DungeonFinderEngineGetSNO()
{
    if (GameIsOpenWorld() || GameIsWeeklyChallenge())
    {
        if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
            return GlobalSNOGet(GLOBALSNO_DUNGEON_FINDER_ENGINE_BLIZZCON);
        if (GameCommonGameFlagIsSet("LootRunIsTieredRift"))
            return GlobalSNOGet(GLOBALSNO_DUNGEON_FINDER_ENGINE_GREATERRIFT);
        return GlobalSNOGet(GLOBALSNO_DUNGEON_FINDER_ENGINE);
    }
    return SNO_NONE;
}

void DungeonFinderGivePlayersRewardChoices(BOOL bSuccess)
{
    NephalemRiftManager::PlayersGiveAllRiftRewardChoices(bSuccess);
}

void SetDungeonReset()
{
    SWorldResetSetDungeon();
}

BOOL ActorIsGold(ACDID idACDItem)
{
    ActorCommonData * ptACDItem = ACDTryToGet(idACDItem);
    if (ptACDItem == NULL)
        return FALSE;
    return ptACDItem->IsItem() && ItemIsGold(*ptACDItem);
}

BOOL ActorIsCorpse(ACDID idACDCorpse)
{
    ActorCommonData * ptACD = ACDTryToGet(idACDCorpse);
    if (ptACD)
        return ptACD->IsCorpse();
    return FALSE;
}

BOOL ActorCanFreelyCastCorpseSkills(ACDID idACDActor)
{
    ActorCommonData * ptACD = ACDTryToGet(idACDActor);
    if (ptACD)
        return ptACD->AttributesGetInt(ATTRIBUTE_NECROMANCER_CORPSE_FREE_CASTING);
    return FALSE;
}

BOOL ActorIsGizmo(ACDID idACD)
{
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD)
        return ptACD->IsGizmo();
    return FALSE;
}

BOOL PlayerPickUpItem(ACDID idACDPlayer, ACDID idACDItem, BOOL bPlayEffects)
{
    ActorCommonData * ptACDPlayer = ACDTryToGet(idACDPlayer);
    if (ptACDPlayer == NULL)
        return FALSE;
    if (PlayerGetByACD(idACDPlayer) == NULL)
        return FALSE;
    ActorCommonData * ptACDItem = ACDTryToGet(idACDItem);
    if (ptACDItem == NULL || !ptACDItem->IsItem())
        return FALSE;
    if (!ptACDItem->IsInSWorldID(ptACDPlayer->GetWorldLocation().idSWorld))
        return FALSE;
    PickupResponse ePickupResponse = SACDInventoryPickup(idACDPlayer, idACDItem, EFFECT_INVALID, TRUE, FALSE, NULL, bPlayEffects, FALSE, FALSE, TRUE);
    return ePickupResponse == PICKUP_RESPONSE_OK;
}

BOOL PlayerIsInChallengeRift(ACDID idACDPlayer)
{
    ActorCommonData * ptACDPlayer = ACDTryToGet(idACDPlayer);
    if (ptACDPlayer == NULL)
        return FALSE;
    return ptACDPlayer->AttributesGetInt(ATTRIBUTE_IN_CHALLENGE_RIFT) != 0;
}

BOOL ActorIsInLootRun(VACDID idActor)
{
    ActorCommonData * ptACD = ACDTryToGet(idActor);
    if (ptACD)
        return ACDIsInLootRun(*ptACD);
    return FALSE;
}

BOOL ActorIsInLootRunWorld(ACDID idActor)
{
    ActorCommonData * ptACD = ACDTryToGet(idActor);
    if (ptACD)
    {
        SWorldID idSWorld = ptACD->GetWorldLocation().idSWorld;
        return SWorldIsTieredDungeonFinder(idSWorld);
    }
    return FALSE;
}

BOOL ActorIsInTieredLootRun(VACDID idActor)
{
    ActorCommonData * ptACD = ACDTryToGet(idActor);
    if (ptACD)
        return ACDIsInTieredLootRun(*ptACD);
    return FALSE;
}

BOOL ActorIsInPostBossTieredLootRun(VACDID idActor)
{
    ActorCommonData * ptACD = ACDTryToGet(idActor);
    if (ptACD == NULL)
        return FALSE;

    if (GameGetActiveGameCommonData().tGameSyncedData.bLootRunBossActive)
        return TRUE;

    return GameGetActiveGameCommonData().tGameSyncedData.bLootRunBossDead != FALSE;
}

void GamePostChallengeRiftTime(int32 nMillisecondsElapsed)
{
    SLeaderboard::PostRiftTime(NephalemRiftManager::GetActiveRiftLevel() + 1, nMillisecondsElapsed,
                               NephalemRiftManager::RiftPlayerCount(), PlayerGetFirstInTieredRift(), false);
}

void GamePostRiftTime(int riftLevel, int millisecondsElapsed)
{
    {
        Achievement::EvaluationData tEvalData;
        for (Player * ptPlayer = PlayerGetFirstInTieredRift(); ptPlayer; ptPlayer = PlayerGetNextInTieredRift(ptPlayer))
        {
            tEvalData.SetPlayer(ptPlayer);
            tEvalData.EvaluateEvent(ptPlayer, Achievement::ACH_EVENT_PLAYER_COMPLETED_TIERED_RIFT, 1);
        }
    }

    SLeaderboard::PostRiftTime(riftLevel, millisecondsElapsed, NephalemRiftManager::RiftPlayerCount(), PlayerGetFirstInTieredRift(), false);

    if (NephalemRiftManager::RiftPlayerCount() == 1)
    {
        Player * ptPlayer = PlayerGetFirstInTieredRift();
        BOOL bHasNewSoloRecord = SPlayerRecordHighestSoloRiftLevel(ptPlayer, riftLevel);
        NephalemRiftManager::PlayerStateSetNewSoloRecord(ptPlayer, bHasNewSoloRecord);
    }
}

int32 GameGetArtificiallyElapsedRiftTimeMS()
{
    return NephalemRiftManager::GetArtificiallyElapsedTimeMS();
}

void GameUnlockRiftLevelsForParticipants(int32 nRiftTierCompleted)
{
    NephalemRiftManager::UnlockRiftLevelsForParticipants(nRiftTierCompleted);
}

void GameSetPlayersLastStartedRiftLevel(int32 nRiftLevel)
{
    if (nRiftLevel < GREATERRIFTLEVEL_MINIMUM || nRiftLevel > GREATERRIFTLEVEL_MAXIMUM)
        return;
    PlayerList listPlayers(PlayerListGetElementPool());
    PlayerListGetAllConnected(listPlayers, -1, FALSE);
    POS posPlayer = listPlayers.GetHeadPosition();
    while (posPlayer)
    {
        Player * ptPlayerToCheck = listPlayers.GetNext(posPlayer);
        ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayerToCheck->idPlayerACD);
        if (ptACDPlayer && ACDIsParticipatingInTieredLootRun(*ptACDPlayer))
            ptACDPlayer->AttributesSetInt(ATTRIBUTE_LAST_RIFT_STARTED_LEVEL, nRiftLevel);
    }
}

void GameRiftRecordDynamicPylonSpawn(VACDID idACDPylon)
{
    ActorCommonData & tACDPylon = ACDGet(idACDPylon);
    SWorldRecordPylonSpawnForRift(tACDPylon);
}

void GameSetLootRunBossKilled(BOOL bKilled)
{
    SGame & tSGame = SGameManager::GameGet();
    if (tSGame.ptSGameGlobals)
    {
        BOOL bSame = ((tSGame.ptSGameGlobals->dwFlags2 >> SGAMEGLOBALS2_LOOT_RUN_BOSS_KILLED) & 1) == bKilled;
        if (!bSame)
        {
            if (bKilled)
                tSGame.ptSGameGlobals->dwFlags2 |= (1 << SGAMEGLOBALS2_LOOT_RUN_BOSS_KILLED);
            else
                tSGame.ptSGameGlobals->dwFlags2 &= ~(1 << SGAMEGLOBALS2_LOOT_RUN_BOSS_KILLED);
            tSGame.ptSGameGlobals->dwFlags2 |= (1 << SGAMEGLOBALS2_GAME_FLAGS_CHANGED);
        }
    }
}

void GameSetLootRunChallengeCompleted(BOOL bCompleted)
{
    SGame & tSGame = SGameManager::GameGet();
    if (tSGame.ptSGameGlobals)
    {
        if (bCompleted)
            tSGame.ptSGameGlobals->dwFlags2 |= (1 << SGAMEGLOBALS2_LOOT_RUN_CHALLENGE_COMPLETED);
        else
            tSGame.ptSGameGlobals->dwFlags2 &= ~(1 << SGAMEGLOBALS2_LOOT_RUN_CHALLENGE_COMPLETED);
        tSGame.ptSGameGlobals->dwFlags2 |= (1 << SGAMEGLOBALS2_GAME_FLAGS_CHANGED);
    }
}

void GameShowWeeklyChallengeLeaderboard()
{
    if (GameIsWeeklyChallenge())
    {
        PlayerList listWeeklyChallengePlayers(PlayerListGetElementPool());
        for (Player * ptPlayer = PlayerGetFirstInTieredRift(); ptPlayer; ptPlayer = PlayerGetNextInTieredRift(ptPlayer))
            listWeeklyChallengePlayers.AddTail_Tracked(ptPlayer);
        SimpleMessage tMessage;
        MessageSendToClient(listWeeklyChallengePlayers, SMSG_WEEKLY_CHALLENGE_COMPLETED, &tMessage, sizeof(tMessage));
    }
}

float GameGetLootRunProgressPercent()
{
    SGameGlobals & tSGameGlobals = SGameGlobalsGet();
    ScopedGlobalDefinitionLockR pdefGlobals;
    float flMaxXPPercent = pdefGlobals->flLootRunXPPercentToComplete;
    if (NephalemRiftManager::IsTieredRiftActive())
        flMaxXPPercent = pdefGlobals->flTieredLootRunXPPercentToComplete;
    return tSGameGlobals.tDungeonFinderData.flDungeonFinderProgress / flMaxXPPercent;
}

void GameSetLootRunBossActive(BOOL bActive)
{
    SGame & tSGame = SGameManager::GameGet();
    if (tSGame.ptSGameGlobals)
    {
        if (bActive)
            tSGame.ptSGameGlobals->dwFlags2 |= (1 << SGAMEGLOBALS2_LOOT_RUN_BOSS_ACTIVE);
        else
            tSGame.ptSGameGlobals->dwFlags2 &= ~(1 << SGAMEGLOBALS2_LOOT_RUN_BOSS_ACTIVE);
        tSGame.ptSGameGlobals->dwFlags2 |= (1 << SGAMEGLOBALS2_GAME_FLAGS_CHANGED);
    }
}

void GameKillAllTieredLootRunMonsters()
{
    SGame & tSGame = SGameManager::GameGet();
    if (tSGame.ptSGameGlobals == NULL || !((tSGame.ptSGameGlobals->dwFlags2 >> SGAMEGLOBALS2_LOOT_RUN_BOSS_ACTIVE) & 1))
        return;

    ACDIterator tIt;
    while (ActorCommonDataIterate(tIt))
    {
        ActorCommonData * ptACD = tIt.ptElement;
        if (ptACD && ACDIsInTieredLootRun(*ptACD) && ptACD->IsMonster() && ptACD->IsMonsterTeam() &&
            !ptACD->AttributesGetInt(ATTRIBUTE_IS_LOOT_RUN_BOSS))
        {
            ActorID idActor = (ActorID)ACDGetOwner(ptACD->id);
            Actor * ptActor = ActorGet(idActor);
            if (ptActor->ptAI)
                BrainAPIDoDeath(idActor, ACTORID_INVALID, TRUE, TRUE, TRUE, NULL, NULL);
        }
    }
}

void GameKillAllMonstersInWorld(ACDID idACDActorWorld)
{
    ActorCommonData * ptACDActorWorld = ACDTryToGet(idACDActorWorld);
    if (!ptACDActorWorld)
        return;

    ACDIterator tIt;
    while (ActorCommonDataIterate(tIt))
    {
        ActorCommonData * ptACD = tIt.ptElement;
        if (ptACD && ptACD->IsMonster() && ptACD->IsMonsterTeam() &&
            ptACD->GetPlace().idSWorld == ptACDActorWorld->GetPlace().idSWorld)
        {
            ActorID idActor = (ActorID)ACDGetOwner(ptACD->id);
            Actor * ptActor = ActorGet(idActor);
            if (ptActor->ptAI)
                BrainAPIDoDeath(idActor, ACTORID_INVALID, TRUE, TRUE, TRUE, NULL, NULL);
        }
    }
}

void GameClearLootRunState()
{
    NephalemRiftManager::Cleanup();
}

void GameSetLootRunJewelUpgradeVendor(VACDID idACDVendor)
{
    NephalemRiftManager::SetJewelUpgradeVendor(idACDVendor);
}

ACDID GameGetLootRunJewelUpgradeVendor()
{
    return NephalemRiftManager::GetJewelUpgradeVendor();
}

// ---- batch 2 ----
void ActorSetPrimaryGroup(VACDID idActor, LPCSTR szGroupName)
{
    ActorCommonData & tACD = ACDGet(idActor);
    tACD.SetPrimaryGroup(ActorGroupNameToHash(szGroupName), TRUE, ACDID_INVALID);
}

void ActorSetSecondaryGroup(VACDID idActor, LPCSTR szGroupName)
{
    ActorCommonData & tACD = ACDGet(idActor);
    tACD.SetSecondaryGroup(ActorGroupNameToHash(szGroupName), TRUE, ACDID_INVALID);
}

CRefString ActorGetLevelArea(VACDID idActor)
{
    ActorCommonData & tACD = ACDGet(idActor);
    return SNOFileGetName(SNOName(LEVELAREA_SNO, ACDGetLevelArea(tACD)));
}

BOOL ActorIsRareOrChampion(ACDID idActor)
{
    ActorCommonData * ptACD = ACDTryToGet(idActor);
    if (ptACD)
    {
        switch (ptACD->GetEliteClass())
        {
        case ELITECLASS_CHAMPION:
        case ELITECLASS_RARE:
            return TRUE;
        default:
            break;
        }
    }
    return FALSE;
}

BOOL ActorIsBoss(ACDID idActor)
{
    ActorCommonData * ptACD = ACDTryToGet(idActor);
    if (ptACD == NULL)
        return FALSE;
    return ptACD->eMonsterRarity == 7;  // MonsterRarity: boss
}

MonsterRarity ActorGetMonsterRarity(ACDID idActor)
{
    ActorCommonData * ptACD = ACDTryToGet(idActor);
    if (ptACD == NULL)
        return -1;
    return ptACD->eMonsterRarity;
}

BOOL ActorIsLegendaryItem(ACDID idActor)
{
    ActorCommonData * ptACD = ACDTryToGet(idActor);
    if (ptACD && ptACD->IsItem() && ItemIsLegendary(ptACD->hGB.gbid))
        return TRUE;
    return FALSE;
}

BOOL ActorIsInTown(ACDID idActor)
{
    ActorCommonData * ptACD = ACDTryToGet(idActor);
    if (ptACD && ACDIsInTown(*ptACD))
        return TRUE;
    return FALSE;
}

BOOL ActorIsInNoBanterArea(ACDID idActor)
{
    ActorCommonData * ptACD = ACDTryToGet(idActor);
    if (ptACD && ACDIsInNoBanterArea(*ptACD))
        return TRUE;
    return FALSE;
}

BOOL PlayerIsListeningToLore(ACDID idACDPlayer)
{
    ActorCommonData * ptACD = ACDTryToGet(idACDPlayer);
    if (ptACD && ptACD->AttributesGetInt(ATTRIBUTE_LORE_PLAYING))
        return TRUE;
    return FALSE;
}

void ActorFadeAndDelete(VACDID idACD, float flFadeTime)
{
    FadeOut(idACD, (int32)(flFadeTime * 60.0f));
    ActorDeleteSoon((ActorID)ACDGetOwner(idACD), 2);
}

ACDID ActorItemGetOwner(VACDID idItemActor)
{
    ActorCommonData * ptACD = ACDTryToGet(idItemActor);
    if (ptACD == NULL)
        return ACDID_INVALID;
    return ItemGetAssignedACDID(*ptACD);
}

void ActorSetDamageCap(ACDID idACD, float flUnitPercent)
{
    ACDGet(idACD).AttributesSetFloat(ATTRIBUTE_DAMAGE_CAP_PERCENT, flUnitPercent);
}

void ActorSetLeader(ACDID idActor, BOOL bValue)
{
    ActorCommonData * ptACD = ACDTryToGet(idActor);
    if (ptACD)
        ptACD->AttributesSetInt(ATTRIBUTE_LEADER, bValue);
}

ACDID ActorGetFollowTarget(VACDID idACD)
{
    ACDID idACDTarget = (ACDID)ACDGet(idACD).AttributesGetInt(ATTRIBUTE_FOLLOW_TARGET_ACDID);
    if (ACDTryToGet(idACDTarget) == NULL)
        return ACDID_INVALID;
    return idACDTarget;
}

void ActorSetFollowTarget(VACDID idACD, VACDID idACDToFollow, AIFollowType eFollowType)
{
    if (eFollowType != 0 && eFollowType != 1)
        return;
    ActorCommonData & tACD = ACDGet(idACD);
    tACD.AttributesSetInt(ATTRIBUTE_FOLLOW_TARGET_ACDID, idACDToFollow);
    tACD.AttributesSetInt(ATTRIBUTE_FOLLOW_TARGET_TYPE, eFollowType);
}

void ActorResetFollowTarget(VACDID idACD)
{
    ACDGet(idACD).AttributesSetInt(ATTRIBUTE_FOLLOW_TARGET_ACDID, ACDID_INVALID);
}

void ActorDismissHireling(VACDID idHireling)
{
    NPCHandleDismissal(ACDGet(idHireling), FALSE, TRUE);
}

int32 ActorGetLevel(ACDID idACD)
{
    return ACDGet(idACD).AttributesGetInt(ATTRIBUTE_LEVEL);
}

BOOL ActorIsAtLeastPlayerLevelCap(ACDID idACD, ACDID idACDPlayer)
{
    int32 nLevel = ACDGet(idACD).AttributesGetInt(ATTRIBUTE_LEVEL);
    return nLevel >= ACDGet(idACDPlayer).AttributesGetInt(ATTRIBUTE_LEVEL_CAP);
}

void ActorSetTeam(ACDID idActor, Team eTeam)
{
    ActorCommonData & tACD = ACDGet(idActor);
    if (eTeam < 0 || eTeam > 22)
        return;
    tACD.SetTeam(eTeam);
}

void ActorGrantGold(VACDID idACD, int32 nAmount)
{
    ACDGet(idACD).ModifyCurrencyAmount(nAmount, 0, 4 /* GMR_SCRIPT */, FALSE);
}

BOOL ActorHasBloodShards(VACDID idACD)
{
    return ACDGet(idACD).GetCurrencyAmount(1) > 0;
}

ACDID ActorGetItemInInventorySlot(ACDID idActor, InventorySlotType eSlot)
{
    InventoryLocation tInvLoc;
    tInvLoc.eSlot = eSlot;
    tInvLoc.pt = cgptNull;
    tInvLoc.idACDContainer = idActor;
    return ACDInventoryGetItemInSlot(tInvLoc);
}

BOOL ActorGizmoIsDoorOpen(ACDID idACDGizmo)
{
    return GizmoDoorIsOpen(ACDGet(idACDGizmo));
}

BOOL ActorGizmoSwitchIsOn(ACDID idACDGizmo)
{
    return GizmoSwitchIsOn(ACDGet(idACDGizmo));
}

BOOL ActorGizmoIsDisabled(ACDID idACDGizmo)
{
    return GizmoIsDisabled(ACDGet(idACDGizmo));
}

void ActorSetAIState(VACDID idACDActor, AIStateType eState)
{
    AIStateSetState((ActorID)ACDGetOwner(idACDActor), eState, FALSE);
}

void ActorSetConversationStatus(ACDID idACD, BOOL bValue)
{
    ActorCommonData & tACD = ACDGet(idACD);
    if (tACD.IsNPC())
        tACD.AttributesSetInt(ATTRIBUTE_NPC_IS_OPERATABLE, bValue);
}

BOOL ActorGetConversationStatus(VACDID idACD)
{
    ActorCommonData & tACD = ACDGet(idACD);
    if (tACD.IsNPC())
        return tACD.AttributesGetInt(ATTRIBUTE_NPC_IS_OPERATABLE);
    return TRUE;
}

BOOL ItemIsAncient(ACDID idACDItem)
{
    ActorCommonData * ptACD = ACDTryToGet(idACDItem);
    if (ptACD && ptACD->IsItem())
        return ItemIsAncientRank(*ptACD);
    return FALSE;
}

BOOL ItemIsBroken(VACDID idACDItem)
{
    ActorCommonData & tACD = ACDGet(idACDItem);
    if (tACD.IsItem())
        return ItemIsBroken(tACD);
    return FALSE;
}

void PlayerTagMonsterForXP(VPACDID idACDPlayer, ACDID idACDMonster)
{
    ActorCommonData * ptACD = ACDTryToGet(idACDMonster);
    if (ptACD)
        ptACD->TagForExp(idACDPlayer);
}

void PVPSetObserver(ACDID idActor)
{
    ActorCommonData * ptACD = ACDTryToGet(idActor);
    if (ptACD)
        ptACD->SetObserver(TRUE);
}

void ActorAddResource(VACDID idACDActor, float flAmount, BOOL bPrimary, BOOL bApplyOnHitBonuses)
{
    ActorCommonData & tACD = ACDGet(idACDActor);
    tACD.AddResource(bPrimary ? tACD.GetPrimaryResourceType() : tACD.GetSecondaryResourceType(), flAmount,
                     bApplyOnHitBonuses);
}

float ActorGetRadius(VACDID idACDActor)
{
    return ACDGet(idACDActor).GetRadius();
}

float ActorGetHitPointsMax(ACDID idACD)
{
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD == NULL)
        return 0.0f;
    return ptACD->GetHitPointsMaxTotal();
}

float ActorGetHitPointsPercent(ACDID idActor)
{
    ActorCommonData * ptACD = ACDTryToGet(idActor);
    if (ptACD == NULL)
        return 0.0f;
    return ptACD->GetHitpointPercent();
}

float ActorGetHitPoints(ACDID idActor)
{
    ActorCommonData * ptACD = ACDTryToGet(idActor);
    if (ptACD == NULL)
        return 0.0f;
    return ptACD->GetHitPointsCurrent();
}

void ActorSetHitPointsMax(ACDID idActor, float flHPMax)
{
    ActorCommonData * ptACD = ACDTryToGet(idActor);
    if (ptACD)
        ptACD->SetHitPointsMax(flHPMax);
}

void ActorSetWeaponClass(VACDID idActor, WeaponClass eWeaponClass)
{
    if (eWeaponClass < 0 || eWeaponClass > 27)
        return;
    ACDGet(idActor).AttributesSetInt(ATTRIBUTE_CURRENT_WEAPONCLASS, eWeaponClass);
}

void ActorSheathWeapons(ACDID idActor, BOOL bSheath)
{
    ActorCommonData * ptACD = ACDTryToGet(idActor);
    if (ptACD)
        ptACD->AttributesSetInt(ATTRIBUTE_WEAPONS_SHEATHED, bSheath);
}

AIQueuedActionID QActorFadeAndDelete(VACDID idACD, float flFadeTime)
{
    FadeOut(idACD, (int32)(flFadeTime * 60.0f));
    return sQueueAIActionCustomType(4, idACD, ACDID_INVALID, -1, 0.0f, NULL);
}

// ---- batch 3 ----
void CancelConversation(LPCSTR szConversation)
{
    SNO snoConversation = SNOFileLookupGUID(CONVERSATION_SNO, szConversation);
    if (snoConversation != SNO_NONE)
        ConversationMgr::CancelConversation(snoConversation);
}

void CancelConversationsForPlayer(VACDID idACDPlayer)
{
    Player * ptPlayer = PlayerGetByACD(idACDPlayer);
    if (ptPlayer)
        ConversationMgr::PlayerOptOutOfAllConversations(*ptPlayer, NULL);
}

BOOL PlayerFlagIsSet(ACDID idACDPlayer, LPCSTR szFlagLabel)
{
    Player * ptPlayer = PlayerGetByACD(idACDPlayer);
    if (ptPlayer == NULL)
        return FALSE;
    return PlayerIsPlayerGameFlagSet(ptPlayer, szFlagLabel);
}

void PlayerFlagSet(ACDID idACDPlayer, LPCSTR szFlagLabel, BOOL bValue)
{
    Player * ptPlayer = PlayerGetByACD(idACDPlayer);
    if (ptPlayer)
        PlayerSetPlayerGameFlag(ptPlayer, szFlagLabel, bValue);
}

void PlayerSetDungeonLoreFlagSet(VPACDID idACDPlayer, int32 nSetDugneonLoreIndex, BOOL bValue)
{
    Player * ptPlayer = PlayerGetByACD(idACDPlayer);
    if (ptPlayer)
    {
        int32 nFlagToSet = Math::Pin(nSetDugneonLoreIndex + PARTITION_FLAG_SET_DUNGEON_LORE_1,
                                     PARTITION_FLAG_SET_DUNGEON_LORE_1, PARTITION_FLAG_SET_DUNGEON_LORE_5);
        if (bValue)
            ptPlayer->tAccount.uPartitionFlags |= (1 << nFlagToSet);
        else
            ptPlayer->tAccount.uPartitionFlags &= ~(1 << nFlagToSet);
    }
}

BOOL PlayerSetDungeonLoreFlagGet(VPACDID idACDPlayer, int32 nSetDugneonLoreIndex)
{
    Player * ptPlayer = PlayerGetByACD(idACDPlayer);
    if (ptPlayer == NULL)
        return FALSE;
    int32 nFlagToCheck = Math::Pin(nSetDugneonLoreIndex + PARTITION_FLAG_SET_DUNGEON_LORE_1,
                                   PARTITION_FLAG_SET_DUNGEON_LORE_1, PARTITION_FLAG_SET_DUNGEON_LORE_5);
    return (ptPlayer->tAccount.uPartitionFlags >> nFlagToCheck) & 1;
}

LPCSTR PlayerGetHeroName(ACDID idACDPlayer)
{
    return PlayerGetByACD(idACDPlayer)->usHeroName;
}

int32 PlayerGetPlayerKillCount(VPACDID idACDPlayer)
{
    Player * ptPlayer = PlayerGetByACD(idACDPlayer);
    if (ptPlayer == NULL)
        return 0;
    return ptPlayer->nPlayerKills;
}

void PlayerAddNephalemBuffStack(VACDID idACDPlayer)
{
    if (ACDIsPlayer(idACDPlayer))
        SPlayerAddNephalemBuffStack(idACDPlayer);
}

SNO TreasureClassSNOFromName(LPCSTR szTreasureClass)
{
    SNO snoTreasureClass = SNOFileLookupGUID(TREASURECLASS_SNO, szTreasureClass);
    LuaVerifySnosAreReferenced(snoTreasureClass, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE);
    return snoTreasureClass;
}

SNO LevelAreaSNOFromName(LPCSTR szLevelArea)
{
    SNO snoLevelArea = SNOFileLookupGUID(LEVELAREA_SNO, szLevelArea);
    LuaVerifySnosAreReferenced(snoLevelArea, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE);
    return snoLevelArea;
}

void PVPPlayerSetDueling(VPACDID idACDPlayer)
{
    PVPApplyDuelingBuff(*PlayerGetByACD(idACDPlayer));
}

BOOL PVPRoundsEnabled()
{
    return GameGetGameType() == 3;
}

BOOL PVPIsSkirmishMode()
{
    return GameGetPVPGameState() == 1;
}

// Inference: if/return TRUE + return FALSE (row 6840) gives the original orr operand order; a plain return of the || swaps them
BOOL GameIsPvP()
{
    GameType eGameType = GameGetGameType();
    if (SGameIsPvPFlagSet() || (eGameType >= GT_PVP_GAME_FIRST && eGameType <= GT_PVP_GAME_LAST)) return TRUE; return FALSE;
}

BOOL GameIsBlizzcon()
{
    return CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT);
}

BOOL GameSetOpenWorldModeIdle()
{
    return SGameSetOpenWorldMode(NULL, 0, -1, -1, -1, 0);
}

int32 PVPPlayerGetKills(ACDID idACDPlayer)
{
    Player * ptPlayer = PlayerGetByACD(idACDPlayer);
    if (ptPlayer == NULL)
        return 0;
    return ptPlayer->nPlayerKills;
}

int32 PVPPlayerGetAssists(ACDID idACDPlayer)
{
    Player * ptPlayer = PlayerGetByACD(idACDPlayer);
    if (ptPlayer == NULL)
        return 0;
    return ptPlayer->nAssists;
}

int32 PVPPlayerGetDeaths(ACDID idACDPlayer)
{
    Player * ptPlayer = PlayerGetByACD(idACDPlayer);
    if (ptPlayer == NULL)
        return 0;
    return ptPlayer->nDeaths;
}

BOOL PVPGameIsOver()
{
    return (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_PVP_GAME_OVER_BIT)) != 0;
}

double PVPPlayerGetTeamRating(ACDID idACDPlayer)
{
    if (ACDIsPlayer(idACDPlayer))
    {
        Player * ptPlayer = PlayerGetByACD(idACDPlayer);
        if (ptPlayer)
            return ptPlayer->teamRating;
    }
    return 0.0;
}

double PVPPlayerGetTeamVariance(ACDID idACDPlayer)
{
    if (ACDIsPlayer(idACDPlayer))
    {
        Player * ptPlayer = PlayerGetByACD(idACDPlayer);
        if (ptPlayer)
            return ptPlayer->teamVariance;
    }
    return 0.0;
}

double PVPPlayerGetIndividualRating(ACDID idACDPlayer)
{
    if (ACDIsPlayer(idACDPlayer))
    {
        Player * ptPlayer = PlayerGetByACD(idACDPlayer);
        if (ptPlayer)
            return ptPlayer->individualRating;
    }
    return 0.0;
}

double PVPPlayerGetIndividualVariance(ACDID idACDPlayer)
{
    if (ACDIsPlayer(idACDPlayer))
    {
        Player * ptPlayer = PlayerGetByACD(idACDPlayer);
        if (ptPlayer)
            return ptPlayer->individualVariance;
    }
    return 0.0;
}

uint32 PVPPlayerGetTeamID(ACDID idACDPlayer)
{
    if (ACDIsPlayer(idACDPlayer))
    {
        Player * ptPlayer = PlayerGetByACD(idACDPlayer);
        if (ptPlayer)
            return (uint32)ptPlayer->nTeamId;
    }
    return 0;
}

void PVPPlayerSetTeamRating(ACDID idACDPlayer, float flValue)
{
    if (ACDIsPlayer(idACDPlayer))
    {
        Player * ptPlayer = PlayerGetByACD(idACDPlayer);
        if (ptPlayer)
            ptPlayer->teamRating = flValue;
    }
}

void PVPPlayerSetTeamVariance(ACDID idACDPlayer, float flValue)
{
    if (ACDIsPlayer(idACDPlayer))
    {
        Player * ptPlayer = PlayerGetByACD(idACDPlayer);
        if (ptPlayer)
            ptPlayer->teamVariance = flValue;
    }
}

void PVPPlayerSetIndividualRating(ACDID idACDPlayer, float flValue)
{
    if (ACDIsPlayer(idACDPlayer))
    {
        Player * ptPlayer = PlayerGetByACD(idACDPlayer);
        if (ptPlayer)
            ptPlayer->individualRating = flValue;
    }
}

void PVPPlayerSetIndividualVariance(ACDID idACDPlayer, float flValue)
{
    if (ACDIsPlayer(idACDPlayer))
    {
        Player * ptPlayer = PlayerGetByACD(idACDPlayer);
        if (ptPlayer)
            ptPlayer->individualVariance = flValue;
    }
}

PVPGameResult PVPPlayerGetGameResult(ACDID idACDPlayer)
{
    if (ACDIsPlayer(idACDPlayer))
    {
        Player * ptPlayer = PlayerGetByACD(idACDPlayer);
        if (ptPlayer)
            return GameGetPVPGameResultForTeam(ptPlayer->eTeam);
    }
    return (PVPGameResult)-1;
}

static AIQueuedActionID sQueueAIAnim(ACDID idACD, uint32 uAnimTag, float flTime, uint32 uNewIdleAnimTag)
{
    uint32 uTicks = flTime > 0.0f ? (uint32)(flTime * 60.0f) : 0;
    AIQueuedActionID idQAction = AIQueueAddAnim((ActorID)ACDGetOwner(idACD), uAnimTag, uTicks, uNewIdleAnimTag);
    LuaScriptInstanceSetLastAIQAction(LuaScriptGetExecutingScriptInstance(), idQAction);
    return idQAction;
}

static AIQueuedActionID sQueueAIAction(SNO snoPower, ACDID idACDSource, ACDID idACDTarget, int32 uData)
{
    Target tTarget;
    tTarget.InitializeFromACD(idACDTarget, FALSE, FALSE);
    AIQueuedActionID idQAction = AIQueueAddAction((ActorID)ACDGetOwner(idACDSource), snoPower, tTarget, -1, 0.0f, NULL, 0);
    LuaScriptInstanceSetLastAIQAction(LuaScriptGetExecutingScriptInstance(), idQAction);
    return idQAction;
}

static AIQueuedActionID sQueueAIActionCustomType(AIQueuedActionType eType, ACDID idACDSource, ACDID idACDTarget,
                                                 int32 uData, float flData, LPCSTR szData)
{
    Target tTarget;
    tTarget.InitializeFromACD(idACDTarget, FALSE, FALSE);
    AIQueuedActionID idQAction = AIQueueAddAction((ActorID)ACDGetOwner(idACDSource), SNO_NONE, tTarget, uData, flData,
                                                  szData, eType);
    LuaScriptInstanceSetLastAIQAction(LuaScriptGetExecutingScriptInstance(), idQAction);
    return idQAction;
}

AIQueuedActionID QActorWalkToLocation(ACDID idACD, ACDID idLocACD)
{
    return sQueueAIAction(GlobalSNOGet(GLOBALSNO_POWERS_AI_WALK_TO_GUARANTEED), idACD, idLocACD, 0);
}

AIQueuedActionID QActorAnim(VACDID idACD, ActorAnimationTag eAnimTag)
{
    if (eAnimTag == 0 || eAnimTag == -1)
        return -1;
    return sQueueAIAnim(idACD, eAnimTag, 0.0f, -1);
}

AIQueuedActionID QActorAnimAndNewIdle(VACDID idACD, ActorAnimationTag eAnimTag, ActorAnimationTag eIdleAnimTag)
{
    if (eAnimTag == 0 || eAnimTag == -1 || eIdleAnimTag == 0 || eIdleAnimTag == -1)
        return -1;
    return sQueueAIAnim(idACD, eAnimTag, 0.0f, eIdleAnimTag);
}

AIQueuedActionID QActorFaceActor(VACDID idACD, VACDID idTargetACD)
{
    return sQueueAIActionCustomType(2, idACD, idTargetACD, -1, 0.0f, NULL);
}

AIQueuedActionID QActorFaceSameAsActor(VACDID idACD, VACDID idTargetACD)
{
    return sQueueAIActionCustomType(3, idACD, idTargetACD, -1, 0.0f, NULL);
}

AIQueuedActionID QActorWalkForward(VACDID idACD, float flDistance)
{
    return sQueueAIActionCustomType(6, idACD, ACDID_INVALID, *(int32 *)&flDistance, 0.0f, NULL);
}

AIQueuedActionID QActorKill(VACDID idACD, DeathType eDeathType, BOOL bCrit)
{
    return sQueueAIActionCustomType(5, idACD, ACDID_INVALID, eDeathType, bCrit ? 1.0f : 0.0f, NULL);
}

AIQueuedActionID QActorWait(VACDID idActor, float flTime)
{
    return sQueueAIAnim(idActor, 0x11000, flTime, -1);
}

ACDID ActorSpawnMonster(LPCSTR szMonsterName, ACDID idACDLocation, LPCSTR szSpawnAnim, BOOL bRandomOrientation,
                        int nSkipDistMin, int nSkipDistDelta, BOOL bUseContactOffset)
{
    return ActorSpawnActor(szMonsterName, idACDLocation, szSpawnAnim, bRandomOrientation, nSkipDistMin,
                           nSkipDistDelta, bUseContactOffset, FALSE);
}

ACDID ResetHero(const ACDID idACDPlayer, const char * szClassName, int32 nGender)
{
    return ACDID_INVALID;
}

int32 GetRandomInt(int32 nMin, int32 nMax)
{
    int32 nResult = nMin;
    if (nMin <= nMax)
        nResult = RandRange_Internal(SGameGlobalsGet().rng, nMin, nMax - nMin);
    return nResult;
}

float GetRandomFloat()
{
    float fResult = RandFloat_Internal(SGameGlobalsGet().rng);
    return fResult;
}

void ConductorStart(LPCSTR szSNOName)
{
    SNO snoConductor = SNOFileLookupGUID(CONDUCTOR_SNO, szSNOName);
    LuaVerifySnosAreReferenced(snoConductor, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE);
    SConductorInstanceStart(snoConductor);
}

QuestType QuestGetQuestType(LPCSTR szQuest)
{
    SNO snoQuest = SNOFileLookupGUID(QUEST_SNO, szQuest);
    if (snoQuest == SNO_NONE)
        return -1;
    return QuestGetQuestType(snoQuest);
}

Act QuestGetBountyAct(LPCSTR szQuest)
{
    SNO snoQuest = SNOFileLookupGUID(QUEST_SNO, szQuest);
    if (snoQuest == SNO_NONE)
        return -1;
    return QuestGetBountyAct(snoQuest);
}

void UnlockCrafter(VACDID idActorPlayer, CrafterType eCrafterType)
{
    Player * ptPlayer = PlayerGetByACD(idActorPlayer);
    if (ptPlayer)
        SCrafterUnlock(*ptPlayer, eCrafterType);
}

BOOL IsCrafterUnlocked(ACDID idActorPlayer, CrafterType eCrafterType)
{
    Player * ptPlayer = PlayerGetByACD(idActorPlayer);
    if (ptPlayer == NULL)
        return FALSE;
    return GetCraftLevel(eCrafterType, *ptPlayer) != 0;
}

void PlayerSetLevel(ACDID idPlayer, int32 nLevel)
{
    if (PlayerGetByACD(idPlayer))
        ExperienceSetLevel(idPlayer, nLevel, TRUE);
}

int32 PlayerGetIndex(ACDID idPlayer)
{
    Player * ptPlayer = PlayerGetByACD(idPlayer);
    if (ptPlayer == NULL)
        return -1;
    return ptPlayer->nPlayerIndex + 1;
}

void TreasureGoblinEscaped(VACDID idACDTreasureGoblin)
{
    ActorCommonData * ptACDTreasureGoblin = ACDTryToGet(idACDTreasureGoblin);
    if (ptACDTreasureGoblin && ptACDTreasureGoblin->IsMonster() &&
        MonsterGetMonsterFamily(ptACDTreasureGoblin->tSNOName) == MF_TREASURE_GOBLIN)
        ServerMessageSendTreasureGoblinEscapedMsg(*ptACDTreasureGoblin);
}

void GameBlizzconShowGreaterRiftEndScreen()
{
    if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
    {
        PlayerList listPlayersInGame(PlayerListGetElementPool());
        PlayerListGetAllInGame(listPlayersInGame, -1, FALSE, -1);
        BOOL bSentReward = FALSE;
        if (!bSentReward)
        {
            BlizzconEndScreenMessage tMessage;
            tMessage.nRewardTier = 0;
            tMessage.ann = ANN_INVALID;
            MessageSendToClient(listPlayersInGame, SMGS_BLIZZCON_RIFT_ENDED, &tMessage, sizeof(tMessage));
        }
    }
}

void GameBlizzconOpenGreaterRift(ACDID idObelisk, ACDID idPlayer, int32 nTier)
{
    if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
        NephalemRiftManager::TryStartBlizzonRift(idObelisk, nTier, idPlayer);
}

void GameBlizzconManuallyTeleportPlayerToGreaterRift(ACDID idPlayer)
{
    if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
    {
        Player * ptPlayer = PlayerGetByACD(idPlayer);
        if (ptPlayer && NephalemRiftManager::IsAnyRiftActive())
            NephalemRiftManager::PlayerManuallyPortIntoRift(ptPlayer);
    }
}

CRefString ActorGetNameFromSNO(SNO snoActor)
{
    return SNOToString(SNOName(ACTOR_SNO, snoActor), FALSE);
}

CRefString ReferenceActor(LPCSTR szActor, ScriptReferenceType eRefType)
{
    return CRefString(szActor);
}

CRefString ReferenceParticle(LPCSTR szParticle, ScriptReferenceType eRefType)
{
    return CRefString(szParticle);
}

CRefString ReferencePower(LPCSTR szPower, ScriptReferenceType eRefType)
{
    return CRefString(szPower);
}

CRefString ReferenceRecipe(LPCSTR szRecipe, ScriptReferenceType eRefType)
{
    return CRefString(szRecipe);
}

CRefString ReferenceRope(LPCSTR szRope, ScriptReferenceType eRefType)
{
    return CRefString(szRope);
}

CRefString ReferenceEffectGroup(LPCSTR szEffectGroup, ScriptReferenceType eRefType)
{
    return CRefString(szEffectGroup);
}

CRefString ReferenceConversation(LPCSTR szConversation, ScriptReferenceType eRefType)
{
    return CRefString(szConversation);
}

CRefString ReferenceConversationList(LPCSTR szConversationList, ScriptReferenceType eRefType)
{
    return CRefString(szConversationList);
}

CRefString ReferenceTreasureClass(LPCSTR szTreasureClassName, ScriptReferenceType eRefType)
{
    return CRefString(szTreasureClassName);
}

CRefString ReferenceAccolade(LPCSTR szAccoladeName, ScriptReferenceType eRefType)
{
    return CRefString(szAccoladeName);
}

CRefString ReferenceConductor(LPCSTR szConductorName, ScriptReferenceType eRefType)
{
    return CRefString(szConductorName);
}

CRefString ReferenceQuestRange(LPCSTR szQuestRangeName, ScriptReferenceType eRefType)
{
    return CRefString(szQuestRangeName);
}

CRefString ReferenceQuest(LPCSTR szQuestName, ScriptReferenceType eRefType)
{
    return CRefString(szQuestName);
}

CRefString ReferenceWorld(LPCSTR szWorldName, ScriptReferenceType eRefType)
{
    return CRefString(szWorldName);
}

CRefString ReferenceDungeonFinder(LPCSTR szDungeonFinderName, ScriptReferenceType eRefType)
{
    return CRefString(szDungeonFinderName);
}

// ---- batch 4 ----
void ActorDelete(VACDID idACD)
{
    sActorDeleteFromScript(idACD);
}

LPCSTR PlayerGetGender(VPACDID idACDPlayer)
{
    return PlayerIsMale(*PlayerGetByACD(idACDPlayer)) ? "M" : "F";
}

BOOL PlayerIsPrimary(VPACDID idACDPlayer)
{
    Player * ptPlayer = PlayerGetByACD(idACDPlayer);
    if (ptPlayer == NULL)
        return FALSE;
    return ptPlayer->bIsPrimaryPlayerServerOnly;
}

AIQueuedActionID QActorDelete(VACDID idACD)
{
    return sQueueAIActionCustomType(4, idACD, ACDID_INVALID, -1, 0.0f, NULL);
}

BOOL PlayerPaidForLootRun(ACDID idACDPlayer)
{
    Player * ptPlayer = PlayerGetByACD(idACDPlayer);
    if (ptPlayer == NULL)
        return FALSE;
    return NephalemRiftManager::PlayerPaidForRift(ptPlayer);
}

uint32 GameGetChallengeRiftTargetTime()
{
    return SGameGlobalsGet().tChallengeRift.tDungeonFinderOverrides.nWeeklyChallengeTargetMilliseconds;
}

BOOL GameRiftIsDynamicPylonSpawningDisabled()
{
    return SGameGlobalsGet().tChallengeRift.tDungeonFinderOverrides.bUsePylonOverrides;
}

BOOL GameRiftIsOrphaned()
{
    return NephalemRiftManager::IsTieredRiftOrphaned(NULL);
}

int32 TimeGetMachineTicks()
{
    return 0;
}

void ActorCrossGamePowerOn(VACDID idActor, SNO snoPower)
{
}

void ActorCrossGamePowerOff(VACDID idActor, SNO snoPower)
{
}

void TriggerAssert()
{
}

void SetGameMonsterLevel(const int32 nLevel)
{
}

void PlayerEnableAutoEquip(ACDID idPlayer, BOOL bEnabled)
{
}

void PlayerSetNephalemGloryActive(ACDID idPlayer, BOOL bActive)
{
    Player * ptPlayer = PlayerGetByACD(idPlayer);
}

void PlayerBlizzconUnlockCube(ACDID idPlayer)
{
}

void PlayerBlizzconAssignCubePassive(ACDID idPlayer, LPCSTR szItemName, int32 nSlot)
{
}

void PlayerBlizzconGrantExperience(ACDID idPlayer)
{
}

void GameSetOpenWorldModeParam(int32 nParam)
{
    GameGetActiveGameCommonData().tGameSyncedData.nOpenWorldModeParam = nParam;
    SGameSyncData(NULL);
}

void PVPSetAwardsAssigned()
{
    GameGetActiveGameCommonData().tGameSyncedData.bPVPAwardsAssigned = TRUE;
    SGameSyncData(NULL);
}

int32 PVPTeamGetScore(Team eTeam)
{
    GameCommonData & tGCD = GameGetActiveGameCommonData();
    if (eTeam == 2)
        return tGCD.tGameSyncedData.nTeamScore[0];
    if (eTeam == 3)
        return tGCD.tGameSyncedData.nTeamScore[1];
    return 0;
}

double TimeGetGameTime()
{
    return (double)GameGetActiveGameCommonData().dwServerTime / 60.0;
}

Act GetCurrentAct()
{
    return GameGetActiveGameCommonData().tGameSyncedData.eAct;
}

BOOL IsWreckableObject(VACDID idACD)
{
    return ACDGet(idACD).IsWreckableObject();
}

BOOL IsBurrowed(VACDID idACD)
{
    return ACDGet(idACD).IsBurrowed();
}

void SetBossBarTarget(VACDID idACD)
{
    ACDGet(idACD).SetBossBar(TRUE);
}

BOOL PlayerHirelingHasUnspentSkillPoints(VACDID idPlayer)
{
    return HirelingHasUnspentSkillPoints(ACDGet(PlayerGetHireling(idPlayer)));
}

void PlayerLearnLore(VPACDID idACDPlayer, LPCSTR szLore)
{
    SNO snoLore = SNOFileLookupGUID(LORE_SNO, szLore);
    if (snoLore == SNO_NONE)
        return;
    LuaVerifySnosAreReferenced(snoLore, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE);
    SPlayerLearnLore(idACDPlayer, snoLore);
}

BOOL PlayerHasLearnedLore(VPACDID idACDPlayer, LPCSTR szLore)
{
    SNO snoLore = SNOFileLookupGUID(LORE_SNO, szLore);
    if (snoLore == SNO_NONE)
        return FALSE;
    LuaVerifySnosAreReferenced(snoLore, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE);
    return SPlayerHasLearnedLore(idACDPlayer, snoLore);
}

void AllPlayersLearnLore(LPCSTR szLore)
{
    SNO snoLore = SNOFileLookupGUID(LORE_SNO, szLore);
    if (snoLore == SNO_NONE)
        return;
    LuaVerifySnosAreReferenced(snoLore, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE);
    SPlayerAllPlayersLearnLore(snoLore);
}

void TimedEventStart(LPCSTR szSNOName)
{
    SNO snoTimedEvent = SNOFileLookupGUID(TIMEDEVENT_SNO, szSNOName);
    if (snoTimedEvent == SNO_NONE)
        return;
    LuaVerifySnosAreReferenced(snoTimedEvent, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE);
    STimedEventStart(snoTimedEvent, 0);
}

BOOL TimedEventHasExpired(LPCSTR szSNOName)
{
    SNO snoTimedEvent = SNOFileLookupGUID(TIMEDEVENT_SNO, szSNOName);
    if (snoTimedEvent == SNO_NONE)
        return FALSE;
    LuaVerifySnosAreReferenced(snoTimedEvent, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE);
    return STimedEventHasExpired(snoTimedEvent);
}

void TimedEventReset(LPCSTR szSNOName)
{
    SNO snoTimedEvent = SNOFileLookupGUID(TIMEDEVENT_SNO, szSNOName);
    if (snoTimedEvent == SNO_NONE)
        return;
    LuaVerifySnosAreReferenced(snoTimedEvent, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE);
    STimedEventReset(snoTimedEvent);
}

void GateIncrementTriggeringCount(VACDID idACD)
{
    GateTriggeringACDCountChanged(ACDGet(idACD), 1);
}

void GateDecrementTriggeringCount(VACDID idACD)
{
    GateTriggeringACDCountChanged(ACDGet(idACD), -1);
}

int32 ActorGetPrimaryBonusAttribute(VACDID idActor)
{
    switch (ACDGet(idActor).AttributesGetInt(ATTRIBUTE_PRIMARY_DAMAGE_ATTRIBUTE))
    {
    case 1:
        return ATTRIBUTE_DEXTERITY_BONUS;
    case 2:
        return ATTRIBUTE_INTELLIGENCE_BONUS;
    default:
        return ATTRIBUTE_STRENGTH_BONUS;
    }
}

int32 ActorGetPrimaryAttributeTotal(VACDID idActor)
{
    switch (ACDGet(idActor).AttributesGetInt(ATTRIBUTE_PRIMARY_DAMAGE_ATTRIBUTE))
    {
    case 1:
        return ATTRIBUTE_DEXTERITY_TOTAL;
    case 2:
        return ATTRIBUTE_INTELLIGENCE_TOTAL;
    default:
        return ATTRIBUTE_STRENGTH_TOTAL;
    }
}

void ActorSetGuardObject(VACDID idACD, VACDID idACDGuardObject)
{
    ACDGet(idACD).AttributesSetInt(ATTRIBUTE_GUARD_OBJECT_ACDID, idACDGuardObject);
}

void ActorSetEnemyTarget(VACDID idACD, VACDID idACDEnemyTarget)
{
    ACDGet(idACD).AttributesSetInt(ATTRIBUTE_FORCED_ENEMY_ACDID, idACDEnemyTarget);
}

void ActorDeactivateLure(VACDID idACD, BOOL bWantDeactivate)
{
    ACDGet(idACD).AttributesSetInt(ATTRIBUTE_DEACTIVATE_LURE, bWantDeactivate);
}

void ActorClearEnemyTarget(VACDID idACD)
{
    ACDGet(idACD).AttributesSetInt(ATTRIBUTE_FORCED_ENEMY_ACDID, ACDID_INVALID);
}

// ---- batch 5 ----
AIQueuedActionID QActorTownWalkToLocation(ACDID idACD, ACDID idLocACD)
{
    return sQueueAIAction(GlobalSNOGet(GLOBALSNO_POWERS_AI_TOWNWALK_TO_GUARANTEED), idACD, idLocACD, 0);
}

AIQueuedActionID QActorRunToLocation(ACDID idACD, ACDID idLocACD)
{
    return sQueueAIAction(GlobalSNOGet(GLOBALSNO_POWERS_AI_RUN_TO_GUARANTEED), idACD, idLocACD, 0);
}

AIQueuedActionID QActorRunToLocationSpider(ACDID idACD, ACDID idLocACD)
{
    return sQueueAIAction(GlobalSNOGet(GLOBALSNO_POWERS_AI_RUN_TO_GUARANTEED_SPIDER), idACD, idLocACD, 0);
}

AIQueuedActionID QActorSprintToLocation(ACDID idACD, ACDID idLocACD)
{
    return sQueueAIAction(GlobalSNOGet(GLOBALSNO_POWERS_AI_SPRINT_TO_GUARANTEED), idACD, idLocACD, 0);
}

AIQueuedActionID QActorWalkToFrontOf(ACDID idACD, ACDID idTargetACD)
{
    return sQueueAIAction(GlobalSNOGet(GLOBALSNO_POWERS_AI_WALK_IN_FRONT_GUARANTEED), idACD, idTargetACD, 0);
}

AIQueuedActionID QActorRunToFrontOf(ACDID idACD, ACDID idTargetACD)
{
    return sQueueAIAction(GlobalSNOGet(GLOBALSNO_POWERS_AI_RUN_IN_FRONT_GUARANTEED), idACD, idTargetACD, 0);
}

AIQueuedActionID QActorSprintToFrontOf(ACDID idACD, ACDID idTargetACD)
{
    return sQueueAIAction(GlobalSNOGet(GLOBALSNO_POWERS_AI_SPRINT_IN_FRONT_GUARANTEED), idACD, idTargetACD, 0);
}

AIQueuedActionID QActorInteract(VACDID idACD, VACDID idTargetACD)
{
    return sQueueAIAction(GlobalSNOGet(GLOBALSNO_POWERS_INTERACT_NORMAL), idACD, idTargetACD, 0);
}

AIQueuedActionID QActorInteractCrouching(VACDID idACD, VACDID idTargetACD)
{
    return sQueueAIAction(GlobalSNOGet(GLOBALSNO_POWERS_INTERACT_CROUCHING), idACD, idTargetACD, 0);
}

AIQueuedActionID QActorAnimLoop(VACDID idACD, ActorAnimationTag eAnimTag, float flTime)
{
    if (eAnimTag == 0 || eAnimTag == -1)
        return -1;
    if (flTime > 3600.0f || flTime < 0.000001f)
        return -1;
    return sQueueAIAnim(idACD, eAnimTag, flTime, -1);
}

AIQueuedActionID QActorPower(VACDID idACD, LPCSTR szPower, VACDID idACDTarget)
{
    SNO snoPower = SNOFileLookupGUID(POWER_SNO, szPower);
    if (snoPower == SNO_NONE)
        return -1;
    LuaVerifySnosAreReferenced(snoPower, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE);
    return sQueueAIAction(snoPower, idACD, idACDTarget, 0);
}

AIQueuedActionID QActorWaitForConversationFinished(VACDID idActor, LPCSTR szConversation)
{
    SNO snoConversation = SNOFileLookupGUID(CONVERSATION_SNO, szConversation);
    if (snoConversation == SNO_NONE)
        return -1;
    LuaVerifySnosAreReferenced(snoConversation, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE);
    return sQueueAIActionCustomType(9, idActor, ACDID_INVALID, snoConversation, 0.0f, NULL);
}

AIQueuedActionID QActorEventSend(VACDID idActor, LPCSTR szEventName)
{
    if (strlen(szEventName) >= 32)
        return -1;
    return sQueueAIActionCustomType(8, idActor, ACDID_INVALID, -1, 0.0f, szEventName);
}

AIQueuedActionID QActorEventSendInRange(VACDID idActor, LPCSTR szEventName, float flRange)
{
    if (strlen(szEventName) >= 32)
        return -1;
    return sQueueAIActionCustomType(8, idActor, ACDID_INVALID, -1, flRange, szEventName);
}

AIQueuedActionID QActorEventSendToActor(ACDID idActor, LPCSTR szEventName, ACDID idActorTarget)
{
    if (strlen(szEventName) >= 32)
        return -1;
    return sQueueAIActionCustomType(8, idActor, idActorTarget, -1, 0.0f, szEventName);
}

int32 ActorGetPlayerIndex(VACDID idACDPlayer)
{
    ActorCommonData * ptACD = ACDTryToGet(idACDPlayer);
    if (ptACD && ACDIsPlayer(idACDPlayer))
    {
        Player * ptPlayer = PlayerGetByACD(idACDPlayer);
        if (ptPlayer)
            return ptPlayer->nPlayerIndex;
    }
    return -1;
}

void HQCursedRealmShowRewardDialog(int32 nActIndex)
{
    if (nActIndex < 1 || nActIndex > 5)
        return;

    FastAttribKey tCursedRealmReagentsCollectedKey = MakeAttribKey(ATTRIBUTE_HQ_CURSED_REALM_REAGENTS_COLLECTED, nActIndex);
    FastAttribKey tCursedRealmReagentsDroppedKey = MakeAttribKey(ATTRIBUTE_HQ_CURSED_REALM_REAGENTS_DROPPED, nActIndex);
    int32 nPlayerBest = 0;
    PlayerList listDestPlayers(PlayerListGetElementPool());
    for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {
        ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
        if (ptACDPlayer)
        {
            int32 nPlayerCollectedCount = ptACDPlayer->AttributesGetInt(tCursedRealmReagentsCollectedKey);
            int32 nPlayerDroppedCount = ptACDPlayer->AttributesGetInt(tCursedRealmReagentsDroppedKey);
            if (nPlayerCollectedCount > 0 || nPlayerDroppedCount > 0)
            {
                listDestPlayers.AddTail_Tracked(ptPlayer);
                nPlayerBest = Math::Max(nPlayerBest, nPlayerCollectedCount - nPlayerDroppedCount);
            }
        }
    }

    if (listDestPlayers.Count() <= 0)
        return;

    HoradricQuestCursedRealmResults tMessage;
    tMessage.nActIndex = nActIndex;
    tMessage.nRewardAmount = nPlayerBest;
    MessageSendToClient(listDestPlayers, SMSG_CURSED_REALM_RESULTS, &tMessage, sizeof(tMessage));
}

ACDID GetSetDungeonOwner()
{
    if (SetEncounterManager::IsAnyEncounterActive())
    {
        for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
        {
            if (ptPlayer->dwSetDungeonFlags & (1 << ESF_INSTIGATOR))
                return ptPlayer->idPlayerACD;
        }
    }
    return ACDID_INVALID;
}

void SetDungeonComplete(LPCSTR szQuestKill, int32 nQuestKillMonsterCount, LPCSTR szQuestBonus1, BOOL bQuestBonus1Success, LPCSTR szQuestBonus2, BOOL bQuestBonus2Success, LPCSTR szQuestMastery, BOOL bQuestMasterySuccess, int32 nTimeTaken, int32 nTargetTime, BOOL bQuestKillSuccess, BOOL bDisplayTotalTime)
{
    ACDID idACDPlayer = SetEncounterManager::GetOwner();
    if (idACDPlayer == ACDID_INVALID)
        return;

    SetDungeonResultsMessage tResultsMessage;
    tResultsMessage.snoQuestKill = SNOFileLookupGUID(QUEST_SNO, szQuestKill);
    tResultsMessage.snoQuestBonus1 = SNOFileLookupGUID(QUEST_SNO, szQuestBonus1);
    tResultsMessage.snoQuestBonus2 = SNOFileLookupGUID(QUEST_SNO, szQuestBonus2);
    tResultsMessage.snoQuestMastery = SNOFileLookupGUID(QUEST_SNO, szQuestMastery);
    tResultsMessage.nQuestKillMonsterCount = nQuestKillMonsterCount;
    tResultsMessage.bQuestBonus1Success = bQuestBonus1Success;
    tResultsMessage.bQuestBonus2Success = bQuestBonus2Success;
    tResultsMessage.bQuestMasterySuccess = bQuestMasterySuccess;
    tResultsMessage.bQuestKillSuccess = bQuestKillSuccess;
    tResultsMessage.bShowTotalTime = bDisplayTotalTime;
    tResultsMessage.nTimeTaken = nTimeTaken;
    tResultsMessage.nTargetTime = nTargetTime;

    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetSingle(listDestPlayers, idACDPlayer);
    MessageSendToClient(listDestPlayers, SMSG_SET_DUNGEON_RESULTS_MESSAGE, &tResultsMessage, sizeof(tResultsMessage));

    ActorCommonData * ptACDPlayer = ACDTryToGet(idACDPlayer);
    if (ptACDPlayer)
    {
        Player * ptPlayer = PlayerGetByACD(idACDPlayer);
        if (ptPlayer)
            ptPlayer->bSetDungeonActive = FALSE;
    }
}

void SetDungeonStatsRecordFailure(LPCTSTR szQuestKill, int32 nQuestKillMonsterCount, int32 nTimeTaken,
                                  int32 nTargetTime)
{
}

#line 734 "X/Server/LuaScript/ScriptUtil.cpp"
int32 ActorGroupGetCount(
    LPCSTR szGroupName)
{
    ACDGlobals * ptACDGlobals = GameGetActiveGameCommonData().ptACDGlobals;

    uint32 uHash = ActorGroupNameToHash(szGroupName);
    ACDGroup tACDGroup;
    if (ptACDGlobals->tACDGroups.tMapACDGroups.Lookup(uHash, tACDGroup))
        return tACDGroup.nCount;




    return 0;
}

#line 820 "X/Server/LuaScript/ScriptUtil.cpp"
static CRefString sFindGroupName(
    uint32 uHash)
{
    CRefString sGroup;

    if (uHash != (uint32)-1)
    {
        ScopedGlobalDefinitionLockR ptGlobals;


        int nGroupCount = ptGlobals->ptServerData->serActorGroups.dwSize / sizeof(ActorGroup);

        for (int i = 0; i < nGroupCount; ++i)
        {
            ActorGroup & tActorGroup = ptGlobals->ptServerData->arActorGroups[i];
            if (tActorGroup.uHash == uHash)
            {
                sGroup = tActorGroup.szName;
                break;
            }
        }
    }

    return sGroup;
}


CRefString ActorGetPrimaryGroup(
    VACDID idActor)
{
    ActorCommonData & tACD = ACDGet(idActor);

    return sFindGroupName(tACD.uGroupPrimary);
}

CRefString ActorGetSecondaryGroup(
    VACDID idActor)
{
    ActorCommonData & tACD = ACDGet(idActor);

    return sFindGroupName(tACD.uGroupSecondary);
}

#line 10852 "X/Server/LuaScript/ScriptUtil.cpp"
int32 AvengerGetPlayerKills(
    ACDID idACD)
{




    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (!ptACD || !ptACD->IsAvenger())
        return 0;


    return ptACD->ptAvengerData->arVictims.size();

}

#line 6072 "X/Server/LuaScript/ScriptUtil.cpp"
void PlayerSetCheckpoint(
    VACDID idACDPlayer,
    VACDID idACDTarget)
{
    Player * ptPlayer = PlayerGetByACD(idACDPlayer);
    if (ptPlayer)
    {
        ActorCommonData & tACDTarget = ACDGet(idACDTarget);
        CheckpointData tCheckpoint;
        tCheckpoint.place = tACDTarget.GetPlace();
        tCheckpoint.qOrientation = tACDTarget.GetOrientation();

        SPlayerSetCheckpoint(*ptPlayer, tCheckpoint);
    }
}

#include "../SGizmo.h"
#include "../AI/BrainComponents.h"
#include "../../Common/Definitions/ActorDefinition.h"

#line 3583 "X/Server/LuaScript/ScriptUtil.cpp"
void ActorGizmoSetDisabled(
    ACDID idACDGizmo,
    BOOL bDisabled)
{
    ActorCommonData & tACDGizmo = ACDGet(idACDGizmo);
    GizmoSetDisabled(tACDGizmo, bDisabled);

    if (!bDisabled)
    {

        ActorID idActor = (ActorID)ACDGetOwner(idACDGizmo);
        Actor * ptActor = ActorTryToGet(idActor);
        if (ptActor)
        {
            SNO snoMarkerSet = ptActor->tMarkerOwner.snoMarkerSet;
            int32 nMarkerIndex = ptActor->tMarkerOwner.nMarkerIndex;
            if (nMarkerIndex >= 0)
            {
                ScopedMarkerSetDefinitionLockR ptMarkerSet(snoMarkerSet);
                if (nMarkerIndex < ptMarkerSet->tMarkerSet.GetArraySize())
                {
                    InactiveMarkerSetCanActivate(ptMarkerSet->tMarkerSet[nMarkerIndex].dwHash, TRUE);
                }
            }
        }
    }
}

#line 8475 "X/Server/LuaScript/ScriptUtil.cpp"
float ActorGetSinglePlayerHitpointsMax(
    ACDID idACD)
{
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (!ptACD)
        return 0.0f;




    float flMaxHP = ptACD->GetHitPointsMaxTotal();
    if (ptACD->IsMonster())
    {
        ScopedActorDefinitionLockR pdefActor(ptACD->tSNOName.snoHandle);
        ScopedMonsterDefinitionLockR pdefMonster(pdefActor->snoMonster);
        if (pdefMonster->dwFlags & (1 << MONSTERDEF_HIRELING_OR_PET_BIT))
            return flMaxHP;



        int32 nNumPlayers = PlayerGetNumPlayersForDifficulty();
        if (nNumPlayers < 2)
            return flMaxHP;



        ScopedGlobalDefinitionLockR pdefGlobal;
        const DifficultyTuningParams & tTuningParams = pdefGlobal->ptServerData->tDifficultyParams[GameGetDifficultyParamsLevel(FALSE)];

        float flMultiplayerScalar = 1.0f + (nNumPlayers - 1) * tTuningParams.flMonsterHPPercentPerPlayer;
        flMaxHP /= flMultiplayerScalar;
    }

    return flMaxHP;
}

#line 9911 "X/Server/LuaScript/ScriptUtil.cpp"
void WarnActorAboutTarget(
    ACDID idACD,
    ACDID idACDTarget)
{
    ActorCommonData & tACD = ACDGet(idACD);
    ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
    if (!ptACDTarget)
        return;



    Team eActorTeam = tACD.GetTeam();

    if (!ACDIsHostile(eActorTeam, ptACDTarget->GetTeam()))
        return;






    if (tACD.IsWreckableObject())
        return;






    BrainAPIWarnAllyAboutTarget((ActorID)ACDGetOwner(idACD), tACD, eActorTeam, (ActorID)ACDGetOwner(idACDTarget), FALSE);
}

#line 697 "X/Server/LuaScript/ScriptUtil.cpp"
ActorIDArray * ActorsInGroup(
    LPCSTR szGroupName)
{
    ACDGlobals * ptACDGlobals = GameGetActiveGameCommonData().ptACDGlobals;
    ActorIDArray & tArrACDs = LuaScriptGetThreadSafeActorIDArray();
    tArrACDs.nNumActorIDs = 0;
    tArrACDs.bWantAddRef = TRUE;

    uint32 uHash = ActorGroupNameToHash(szGroupName);
    ACDGroup tACDGroup;
    if (ptACDGlobals->tACDGroups.tMapACDGroups.Lookup(uHash, tACDGroup))
    {


        if (tACDGroup.ptList)
        {

            POS pos = tACDGroup.ptList->GetHeadPosition();
            while (pos)
            {
                ACDID idACD = tACDGroup.ptList->GetNext(pos);

                tArrACDs.arActorIDs[tArrACDs.nNumActorIDs] = idACD;
                if (++tArrACDs.nNumActorIDs == ACTOR_ID_ARRAY_MAX_ACTORS)
                    break;
            }
        }
    }




    return &tArrACDs;
}

#line 6520 "X/Server/LuaScript/ScriptUtil.cpp"
SSceneID SceneGroupFindByName(
    LPCSTR szMarkerName)
{
    SSceneID idSScene = SSCENEID_INVALID;
    uint32 tName = MarkerStringHash(szMarkerName);
    uint32 uHash = tName;

    SceneCommonData * ptSCD = NULL;
    SceneCommonDataIterator tIterator;
    while (SceneCommonDataArrayIterate(tIterator, ptSCD, SkipHolodeckScenes))
    {
        if (ptSCD->snoSceneGroup != SNO_NONE &&
            ptSCD->snoMarkerSetSrc != SNO_NONE &&
            ptSCD->nMarkerSetSrcIndex != -1)
        {
            ScopedMarkerSetDefinitionLockR ptMarkerSetDef(ptSCD->snoMarkerSetSrc);

            if (ptSCD->nMarkerSetSrcIndex >= 0 && ptSCD->nMarkerSetSrcIndex < ptMarkerSetDef->tMarkerSet.GetArraySize())
            {
                const Marker & tMarker = ptMarkerSetDef->tMarkerSet[ptSCD->nMarkerSetSrcIndex];
                uint32 uNameHash = tMarker.dwHash;
                if (uNameHash == uHash)
                {
                    idSScene = ptSCD->idSScene;
                    break;
                }
            }
        }
    }



    return idSScene;

}

#line 5530 "X/Server/LuaScript/ScriptUtil.cpp"
ActorIDArray * PlayerGetFollowers(
    VACDID idPlayer)
{
    ActorIDArray & tArrACDs = LuaScriptGetThreadSafeActorIDArray();
    tArrACDs.nNumActorIDs = 0;
    tArrACDs.bWantAddRef = TRUE;

    if (!ACDIsPlayer(idPlayer))
        return &tArrACDs;



    ActorCommonData * ptACDOwner = ACDTryToGet(idPlayer);
    if (!ptACDOwner)
        return &tArrACDs;



    int32 nPos;
    ACDID idACDPet = PlayerGetFirstPet(idPlayer, PETTYPE_NONE, nPos);

    while (idACDPet != ACDID_INVALID)
    {

        ActorCommonData * ptACDPet = ACDTryToGet(idACDPet);
        if (ptACDPet)
        {
            PetType ePetType = ptACDPet->AttributesGetInt(ATTRIBUTE_PET_TYPE);
            if (ePetType == PETTYPE_FOLLOWER_PROXY || ePetType == PETTYPE_FOLLOWER)
            {
                if (tArrACDs.nNumActorIDs < ACTOR_ID_ARRAY_MAX_ACTORS)
                {
                    tArrACDs.arActorIDs[tArrACDs.nNumActorIDs] = ptACDPet->id;
                    ++tArrACDs.nNumActorIDs;
                }
            }
        }




        idACDPet = PlayerGetNextPet(ptACDOwner->id, PETTYPE_NONE, nPos);
    }

    return &tArrACDs;
}

#line 4384 "X/Server/LuaScript/ScriptUtil.cpp"
void ItemSocketGem(
    VACDID idItem,
    VACDID idGem)
{
    ActorCommonData * ptACDItem = ACDTryToGet(idItem);
    ActorCommonData * ptACDGem = ACDTryToGet(idGem);

    if (!ptACDItem || !ptACDItem->IsItem() || !ptACDGem)
    {
        return;
    }


    if (!ItemIsSocketable(*ptACDGem))
    {
        return;
    }


    ActorCommonData * ptACDOwner = ACDTryToGet(ptACDItem->tLocation.tInvLocation.idACDContainer);
    if (!ptACDOwner)
    {
        return;
    }


    if (ptACDGem->tLocation.eActorLocation != AL_INVENTORY || ptACDItem->tLocation.eActorLocation != AL_INVENTORY)
    {
        return;
    }


    SACDInventoryProcessSocketRequest(*ptACDOwner, *ptACDGem, *ptACDItem);
}

#include "../Powers/PowerHelper.h"
#line 1958 "X/Server/LuaScript/ScriptUtil.cpp"
void ActorDamageByPercent(
    VACDID idACD,
    float flPercent)
{
    ActorCommonData * ptACD = ACDTryToGet(idACD);


    if (ptACD && ptACD->IsAlive())
    {
        float flHPMax = ptACD->GetHitPointsMaxTotal();
        flPercent /= 100.0f;
        float flDamage = Math::Min(flPercent * flHPMax, flHPMax);

        PowerAPIDamage tModifier;
        PowerHelper_DamageInit(tModifier, POWERID_INVALID, idACD);
        PowerHelper_DamageAdd(tModifier, FastAttribKey(ATTRIBUTE_HITPOINTS_CUR), -flDamage, 0);
        PowerAPI_DoDamage(tModifier, FALSE, NULL);
    }

}

#include "../System/LevelArea.h"
#line 6440 "X/Server/LuaScript/ScriptUtil.cpp"
BOOL SceneIsInLevelArea(
    SSceneID idSScene,
    LPCSTR szLevelAreaName)
{
    CRefString sLevelAreaName;
    SceneCommonData * ptSCD = SceneCommonDataTryToGet(idSScene);

    if (ptSCD)
    {
        for (int32 i = 0; i < 4; ++i)
        {
            SNO snoLevelArea = ptSCD->tSceneSpec.arSnoLevelAreas[i];

            if (snoLevelArea == SNO_NONE)
                continue;

            LuaVerifySnosAreReferenced(snoLevelArea, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE);

            LevelAreaGetName(snoLevelArea, sLevelAreaName);

            if (sLevelAreaName == szLevelAreaName)
            {
                return TRUE;
            }
        }
    }

    return FALSE;

}

#include "../SNPC.h"
#include <limits.h>
// NON_MATCHING: Rand_Internal and RandFast inline AND operands are reversed in two instructions.
#line 2453 "X/Server/LuaScript/ScriptUtil.cpp"
BOOL ActorPlayRandomConversationFromList(
    LPCSTR szConversationList,
    ACDID idACDPlayer,
    ACDID idACDPrimary)
{
    SNO snoConvList = SNOFileLookupGUID(CONVERSATIONLIST_SNO, szConversationList);
    if (snoConvList == SNO_NONE)
    {
        return FALSE;
    }


    LuaVerifySnosAreReferenced(snoConvList, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE);

    ConversationArray tConvArray;
    NPCGetConversationsFromList(snoConvList, idACDPlayer, idACDPrimary, &tConvArray);
    if (tConvArray.size() == 0)
    {
        return FALSE;
    }

    int32 nIndex = Rand_Internal(SGameGlobalsGet().rng, tConvArray.size() - 1);
    SNO snoConv = tConvArray[nIndex];
    if (snoConv == SNO_NONE)
    {
        return FALSE;
    }

    Player * ptPlayer = PlayerGetByACD(idACDPlayer);
    int32 nPlayerIndex = ptPlayer ? ptPlayer->nPlayerIndex : -1;

    BOOL bSuccess = ConversationMgr::StartConversation(nPlayerIndex, snoConv, idACDPrimary, ACDID_INVALID, ACDID_INVALID, ACDID_INVALID, ACDID_INVALID, INT_MAX, FALSE, FALSE);




    return bSuccess;
}

#line 12715 "X/Server/LuaScript/ScriptUtil.cpp"
CRefString DungeonFinderGetWorldTilesetForActor(
    ACDID idActor)
{
    ActorCommonData * ptACD = ACDTryToGet(idActor);
    if (!ptACD)
    {
        return "";
    }

    if (!ptACD->IsInWorld())
    {
        return "";
    }

    SWorldID idSWorld = ptACD->GetSWorldID();
    SNO snoWorld = WorldCommonGetWorldSNO(idSWorld);

    int32 nLevel = WorldGetDungeonFinderLevel(snoWorld);
    if (nLevel != -1)
    {
        snoWorld = SWorldGetDungeonFinderTileset(snoWorld);
        if (snoWorld != SNO_NONE)
        {
            return SNOToString(WORLD_SNO, snoWorld, FALSE);
        }
    }
    return "";
}

#line 12803 "X/Server/LuaScript/ScriptUtil.cpp"
ACDID DungeonFinderSpawnBoss(
    ACDID idACDPlayer)
{
    ActorCommonData * ptACDPlayer = ACDTryToGet(idACDPlayer);
    if (!ptACDPlayer)
    {
        return ACDID_INVALID;
    }


    ScriptEventOnUserEvent("LRALLMONSTERSKILLED", idACDPlayer, 0);

    ACDID idACDBoss = SWorldDungeonFinderSpawnBoss(ptACDPlayer->GetPlace());

    ActorCommonData * ptACDBoss = ACDTryToGet(idACDBoss);
    if (ptACDBoss)
    {
        ptACDBoss->AttributesSetInt(ATTRIBUTE_IS_LOOT_RUN_BOSS, TRUE);
        ptACDBoss->AttributesSetInt(ATTRIBUTE_NEVER_DEACTIVATES, TRUE);

        SGameGlobals & tSGameGlobals = SGameGlobalsGet();
        tSGameGlobals.tDungeonFinderData.snoBossSpawned = ptACDBoss->snoActor;

        return ptACDBoss->id;
    }

    return ACDID_INVALID;
}

#line 2174 "X/Server/LuaScript/ScriptUtil.cpp"
void ActorDeleteInventoryItem(
    VACDID idACDOwner,
    VACDID idACDItem)
{
    ActorCommonData * ptACDOwner = ACDTryToGet(idACDOwner);
    if (!ptACDOwner || (!ACDIsPlayer(idACDOwner) && !ptACDOwner->IsNPC()))
    {
        return;
    }


    ActorCommonData * ptACDItem = ACDTryToGet(idACDItem);
    if (!ptACDItem || !ptACDItem->IsItem())
    {
        return;
    }


    ActorLocation tItemLocation = ptACDItem->GetLocation();
    if (tItemLocation.eActorLocation != AL_INVENTORY || tItemLocation.tInvLocation.idACDContainer != idACDOwner)
    {
        return;
    }


    SACDInventoryDeleteItemAfterUse(*ptACDItem);
}

#line 10628 "X/Server/LuaScript/ScriptUtil.cpp"
BOOL QuestStepIsCurrent(
    LPCSTR szQuestName,
    LPCSTR szStepName_VariableArray,
    ACDID idPlayer)
{
    SNO snoQuest = SNOFileLookupGUID(QUEST_SNO, szQuestName);
    QuestInstance * ptQuestInstance = QuestsGetQuestInstance(snoQuest, idPlayer);

    if (ptQuestInstance)
    {
        if (ptQuestInstance->eInstanceState == QUEST_INSTANCE_FAILED)
        {
            return FALSE;
        }

        ScopedQuestDefinitionLockR pdefQuest(snoQuest);

        for (int32 i = 0; i < pdefQuest->nNumQuestSteps; ++i)
        {
            if (strcmp(pdefQuest->arQuestSteps[i].szStepName_VariableArray, szStepName_VariableArray) == 0)
            {
                return ptQuestInstance->nCurrentStepUID == pdefQuest->arQuestSteps[i].nUID;
            }
        }

#line 10660 "X/Server/LuaScript/ScriptUtil.cpp"
        for (int32 i = 0; i < pdefQuest->nNumQuestCompletionSteps; ++i)
        {
            if (strcmp(pdefQuest->arQuestCompletionSteps[i].szStepName_VariableArray, szStepName_VariableArray) == 0)
            {
                return ptQuestInstance->nCurrentStepUID == pdefQuest->arQuestCompletionSteps[i].nUID;
            }
        }
    }
    else
    {
#line 10679 "X/Server/LuaScript/ScriptUtil.cpp"
        CmdLineGetParams();
    }
    return FALSE;
#line 10688 "X/Server/LuaScript/ScriptUtil.cpp"
}

#line 14352 "X/Server/LuaScript/ScriptUtil.cpp"
void StartBountyConductors(
    )
{
    QuestGlobals * ptQuestGlobals = GameGetActiveGameCommonData().ptQuestGlobals;
    POS pos = ptQuestGlobals->tQuestInstanceList.GetHeadPosition();
    while (pos)
    {
        QuestInstance & tQuestInstance = ptQuestGlobals->tQuestInstanceList.GetNext(pos);
        ScopedQuestDefinitionLockR pdefQuest(tQuestInstance.snoQuest);

        if (QuestIsAnyBountyType(pdefQuest) && pdefQuest->tBountyData.snoGlobalConductor != SNO_NONE)
        {
            SNO snoGlobalConductor = pdefQuest->tBountyData.snoGlobalConductor;
            if (!SConductorActive(snoGlobalConductor))
            {
                SConductorInstanceStart(pdefQuest->tBountyData.snoGlobalConductor);
            }
        }
    }
}

#include "../Loot/Loot.h"
#include "../SItems.h"
#include "../Quests/SQuests.h"
// NON_MATCHING: cgplaceNull uses a standalone constant address instead of the original dynamically initialized merged global.
#line 4258 "X/Server/LuaScript/ScriptUtil.cpp"
BOOL ItemGiveQuestItem(
    LPCSTR szItemName)
{

    GBHandle gbhItem;
    gbhItem.Set(GB_ITEMS, StringToGBID(szItemName));

    if (!GBIDExists(gbhItem))
    {
        return FALSE;
    }


    ACDID idACDItem = ACDID_INVALID;

    {
        LootSpecifier tSpecifier;

        tSpecifier.hGBItem = gbhItem;
        tSpecifier.snoActorForFlippy = GameBalanceGetActorSNO(tSpecifier.hGBItem);
        tSpecifier.nGoldAmount = 10;
        tSpecifier.fAutoIdentify = TRUE;
        tSpecifier.nAffixLevel = 1;

        LootDetermineInherentAffixes(tSpecifier);
        int32 nInherentAffixes = tSpecifier.nNumAffixes;

        tSpecifier.eIQL = ItemDetermineQualityLevel("Script ItemGiveQuestItem", tSpecifier, nInherentAffixes);

        idACDItem = LootCreateItem(tSpecifier, cgplaceNull, NULL);
    }

    if (idACDItem != ACDID_INVALID)
    {

        SQuestsOnPlayerAcquireItem(ACDID_INVALID, idACDItem);


        ActorID idActorItem = (ActorID)ACDGetOwner(idACDItem);

        ActorDelete(idActorItem);
        return TRUE;
    }

    return FALSE;

}

#line 3087 "X/Server/LuaScript/ScriptUtil.cpp"
void ActorMakeFollowerPet(
    VACDID idACDPet,
    VACDID idACDPlayer,
    BOOL bForceSwitch)
{
    Player * ptPlayer = PlayerGetByACD(idACDPlayer);
    if (ptPlayer)
    {
        ActorCommonData & tACDPlayer = ACDGet(idACDPlayer);
        ActorCommonData & tACDPet = ACDGet(idACDPet);
        Player * ptPlayerPreviousOwner = tACDPet.GetPetOwnerPlayer();

        if (tACDPlayer.GetSWorldID() != tACDPet.GetSWorldID())
        {
            return;
        }
#line 3108 "X/Server/LuaScript/ScriptUtil.cpp"
        if (ptPlayerPreviousOwner)
        {
            if (bForceSwitch)
            {
                PlayerRemovePet(*ptPlayerPreviousOwner, idACDPet, TRUE);
            }
            else
            {
                return;
            }
        }

        SPlayerAddFollower(tACDPet, *ptPlayer);
    }
    else
    {
        LPCSTR szName = MarkerNameGetString(ACDGet(idACDPlayer).tName);
    }

}

#line 3145 "X/Server/LuaScript/ScriptUtil.cpp"
void ActorMakeFollowerProxyPet(
    VACDID idACDPet,
    VACDID idACDPlayer)
{
    Player * ptPlayer = PlayerGetByACD(idACDPlayer);
    if (ptPlayer)
    {
        ActorCommonData & tACDPlayer = ACDGet(idACDPlayer);
        ActorCommonData & tACDPet = ACDGet(idACDPet);
        Player * ptPlayerPreviousOwner = tACDPet.GetPetOwnerPlayer();

        if (tACDPlayer.GetSWorldID() != tACDPet.GetSWorldID())
        {
            return;
        }
#line 3165 "X/Server/LuaScript/ScriptUtil.cpp"
        if (ptPlayerPreviousOwner)
        {
            PlayerRemovePet(*ptPlayerPreviousOwner, idACDPet, TRUE);
        }

        SPlayerAddFollowerProxy(tACDPet, *ptPlayer);
    }
    else
    {
        LPCSTR szName = MarkerNameGetString(ACDGet(idACDPlayer).tName);
    }

}

#include "../../Common/NodePath.h"
#include "../../Common/SNO/SymbolicValue.h"
#line 3767 "X/Server/LuaScript/ScriptUtil.cpp"
void ActorSetNodePath(
    VACDID idACD,
    PathClass ePathClass,
    LPCSTR szTargetNodeName)
{
    PathID idPath = NodePathFind(ePathClass, ACDGet(idACD).GetSWorldID());

    if (idPath != PATHID_INVALID)
    {
        PathNodeID idPathNode = NodePathGetNodeByName(idPath, MarkerStringHash(szTargetNodeName));

        if (idPathNode != PATHNODEID_INVALID)
        {
            ActorID idActor = (ActorID)ACDGetOwner(idACD);
            BOOL bPathSet = AISetNodePath(idActor, idPath, idPathNode);
            if (bPathSet)
            {
                return;
            }
        }
    }
#line 3804 "X/Server/LuaScript/ScriptUtil.cpp"
    SymbolToName(ePathClass, gPathClasses);
}

#include <strings.h>

#line 11727 "X/Server/LuaScript/ScriptUtil.cpp"
GBID sFindAchievementEventByName(
    const LPCSTR szAchievementEvent)
{
    const GameBalanceDefinition * lpGBDef = ScriptedAchievementEventsTableLockR();
    const ScriptedAchievementEventsTable & tScriptedAchEventsTable = lpGBDef->tScriptedAchievementEventsTable;

    GBID gbidScriptEvent = INVALID_GBID;

    int32 nNumScriptedEvents = tScriptedAchEventsTable.serScriptedAchievementEvents.ElementCount(sizeof(ScriptedAchievementEvent));
    for (int32 i = 0; i < nNumScriptedEvents; ++i)
    {
        const ScriptedAchievementEvent & tScriptedAchEvent = tScriptedAchEventsTable.ptScriptedAchievementEvents[i];
        if (strcasecmp(tScriptedAchEvent.tHeader.szName, szAchievementEvent) == 0)
        {
            return tScriptedAchEvent.tHeader.gbid;
        }
    }

    ScriptedAchievementEventsTableUnlockR(lpGBDef);

    return gbidScriptEvent;
}



#line 11751 "X/Server/LuaScript/ScriptUtil.cpp"
void AchievementSendEvent(
    VACDID idActorPlayer,
    LPCSTR szAchievementEvent,
    int32 uQuantity)
{
    Player * ptPlayer = PlayerGetByACD(idActorPlayer);
    if (ptPlayer)
    {
#line 11767 "X/Server/LuaScript/ScriptUtil.cpp"
        GBID gbidScriptEvent = sFindAchievementEventByName(szAchievementEvent);

        if (gbidScriptEvent != INVALID_GBID)
        {
            Achievement::EvaluationData tEvaluationData;
            tEvaluationData.SetPlayer(ptPlayer);

            tEvaluationData.EvaluateEvent(ptPlayer, Achievement::ACH_EVENT_SCRIPTED, gbidScriptEvent, uQuantity);
#line 11779 "X/Server/LuaScript/ScriptUtil.cpp"
        }
    }
}

#include "../SGameStats.h"
#include "../../Common/PlayerSwig.h"
#line 6711 "X/Server/LuaScript/ScriptUtil.cpp"
static Achievement::EventID sCalloutTypeToAchievementEvent(
    CalloutEventType eType)
{

    switch (eType)
    {
    case CET_PVP_FIRST_KILL: return Achievement::ACH_EVENT_PVP_CALLOUT_FIRSTBLOOD;
    case CET_PVP_AVENGER_KILL: return Achievement::ACH_EVENT_PVP_CALLOUT_AVENGER;
    case CET_PVP_KILLSTREAK_3: return Achievement::ACH_EVENT_PVP_CALLOUT_KILLSTREAK_3;
    case CET_PVP_KILLSTREAK_5: return Achievement::ACH_EVENT_PVP_CALLOUT_KILLSTREAK_5;
    case CET_PVP_KILLSTREAK_7: return Achievement::ACH_EVENT_PVP_CALLOUT_KILLSTREAK_7;
    case CET_PVP_KILLSTREAK_10: return Achievement::ACH_EVENT_PVP_CALLOUT_KILLSTREAK_10;
    default: return Achievement::ACH_EVENT_NONE;
    }
}

#line 6727 "X/Server/LuaScript/ScriptUtil.cpp"
void CalloutSendToPlayer(
    CalloutEventType eType,
    ACDID idACDPlayer)
{
    if (!ACDIsPlayer(idACDPlayer))
    {
        return;
    }

    SGameStatsPlayerCounterIncrement(PlayerGetByACD(idACDPlayer), STAT_COUNTER_PLAYER_PVP_CALLOUTS, eType, 0, 1.0f);

    {
#line 6744 "X/Server/LuaScript/ScriptUtil.cpp"
        Player * ptPlayer = PlayerGetByACD(idACDPlayer);

        Achievement::EventID achEvent = sCalloutTypeToAchievementEvent(eType);
        if (achEvent != Achievement::ACH_EVENT_NONE)
        {
            Achievement::EvaluationData tEvaluationData;
            tEvaluationData.SetPlayer(ptPlayer);
            tEvaluationData.EvaluateEvent(ptPlayer, achEvent, 1);
#line 6755 "X/Server/LuaScript/ScriptUtil.cpp"
        }
    }

    CalloutMessage tMessage;
    tMessage.eType = eType;
    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetSingle(listDestPlayers, idACDPlayer);
    MessageSendToClient(listDestPlayers, SMSG_CALLOUT_EVENT, &tMessage, sizeof(tMessage));
}

#include "../../Common/Protobuf/GameMessage.pb.h"
#line 13641 "X/Server/LuaScript/ScriptUtil.cpp"
void GameLootRunNotifyClientsPreloadLeaderboardData(
    int32 nTotalTimeMS)
{
    if (!NephalemRiftManager::IsTieredRiftActive())
    {
        return;
    }

    D3::GameMessage::RiftStartPreloadingLeaderBoardData tPreloadData;
    tPreloadData.set_num_participants(NephalemRiftManager::RiftPlayerCount());
    tPreloadData.set_rift_tier(NephalemRiftManager::GetActiveRiftLevel());
    tPreloadData.set_completion_time_ms(nTotalTimeMS);

    PlayerList listPlayersInGame(PlayerListGetElementPool());
    PlayerListGetAllInGame(listPlayersInGame, -1, FALSE, -1);

    POS pos = listPlayersInGame.GetHeadPosition();
    while (pos)
    {
        Player * ptPlayer = listPlayersInGame.GetNext(pos);
        if (NephalemRiftManager::PlayerPaidForRift(ptPlayer))
        {

            PlayerList listDestPlayers(PlayerListGetElementPool());
            PlayerListGetSingle(listDestPlayers, *ptPlayer);
            MessageSendToClient(listDestPlayers, SMSG_PRELOAD_LOOT_RUN_LEADERBOARD_INFO, tPreloadData);
        }
    }
}

#include "../System/SNephalemRift.h"
#include "../System/SChallengeRift.h"
#include "../../Common/Protobuf/Quest.pb.h"
#line 13802 "X/Server/LuaScript/ScriptUtil.cpp"
void GameChallengeRiftHandleCompletion(
    BOOL bFinishedInTime,
    int32 nTotalTimeMS)
{


    for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {
        if (NephalemRiftManager::PlayerPaidForRift(ptPlayer))
        {
            D3::Quests::QuestReward tReward;
            ChallengeRift::RewardScreenInfo tInfo;
            ChallengeRift::HandleChallengeRewards(ptPlayer, nTotalTimeMS, tInfo);

            NephalemRiftManager::PlayerSendRiftEndScreenInfo(*ptPlayer, TRUE, nTotalTimeMS, NephalemRiftManager::GetActiveRiftLevel(), tReward, tInfo);
        }
    }


    uint32 nNumPlayers = NephalemRiftManager::RiftPlayerCount();
    ChallengeRift::ChallengeRiftPersonalBest best = ChallengeRift::NumPlayersToChallengeRiftPersonalBest(nNumPlayers);
    for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {
        if (NephalemRiftManager::PlayerPaidForRift(ptPlayer))
        {
            if (ptPlayer->nChallengeBestTimesMilliseconds[best] == 0 || ptPlayer->nChallengeBestTimesMilliseconds[best] > nTotalTimeMS)
            {
                ptPlayer->nChallengeBestTimesMilliseconds[best] = nTotalTimeMS;
            }
        }
    }
}

#include "../../Common/Definitions/LevelAreaDefinition.h"
#line 14744 "X/Server/LuaScript/ScriptUtil.cpp"
int32 ActorGetHQCursedRealmActIndex(
    VACDID idActor)
{
    ActorCommonData * ptACD = ACDTryToGet(idActor);
    if (!ptACD)
    {
        return -1;
    }



    if (!ptACD->IsInWorld())
    {
        return -1;
    }



    SWorld * ptSWorld = SWorldTryToGet(ptACD->GetSWorldID());
    if (!ptSWorld ||
        !(ptSWorld->dwFlags & (1 << FLAG_SWORLD_FROM_DEATH_PORTAL)) ||
        !ptSWorld->tDynamicReturnLocation.IsValid())
    {
        return -1;
    }



    int32 nCursedRealmAct = -1;
    Act eCursedRealmAct = LevelAreaGetAct(ptSWorld->tDynamicReturnLocation.snoDestLevelArea);
    switch (eCursedRealmAct)
    {
    case ACT_1:
        nCursedRealmAct = 1;
        break;
    case ACT_2:
        nCursedRealmAct = 2;
        break;
    case ACT_3:
        nCursedRealmAct = 3;
        break;
    case ACT_4:
        nCursedRealmAct = 4;
        break;
    case ACT_5:
        nCursedRealmAct = 5;
        break;
    }
#line 14803 "X/Server/LuaScript/ScriptUtil.cpp"
    return nCursedRealmAct;
}

#line 14836 "X/Server/LuaScript/ScriptUtil.cpp"
void PlayerModifyHQCursedRealmReagentsCollected(
    VACDID idPlayerActor,
    int32 nChange)
{
    ActorCommonData * ptACDPlayer = ACDTryToGet(idPlayerActor);
    if (!ptACDPlayer || !ACDIsPlayer(idPlayerActor))
    {
        return;
    }


    int32 nCursedRealmAct = ActorGetHQCursedRealmActIndex(idPlayerActor);
    if (nChange == 0 || nCursedRealmAct == -1)
    {
        return;
    }
#line 14860 "X/Server/LuaScript/ScriptUtil.cpp"
    if (nChange < 0)
    {
        FastAttribKey tCursedRealmReagentsDroppedKey = MakeAttribKey(ATTRIBUTE_HQ_CURSED_REALM_REAGENTS_DROPPED, nCursedRealmAct);

        int32 nDroppedCount = ptACDPlayer->AttributesGetInt(tCursedRealmReagentsDroppedKey);
        ptACDPlayer->AttributesSetInt(tCursedRealmReagentsDroppedKey, nDroppedCount - nChange);
    }
    else
    {

        FastAttribKey tCursedRealmReagentsCollectedKey = MakeAttribKey(ATTRIBUTE_HQ_CURSED_REALM_REAGENTS_COLLECTED, nCursedRealmAct);

        int32 nCollectedCount = ptACDPlayer->AttributesGetInt(tCursedRealmReagentsCollectedKey);
        ptACDPlayer->AttributesSetInt(tCursedRealmReagentsCollectedKey, nCollectedCount + nChange);
    }


    SQuestsOnHQCursedRealmReagentCollected(ptACDPlayer->id, nCursedRealmAct);
}

#include "../../Common/InactiveMarker.h"
#line 10713 "X/Server/LuaScript/ScriptUtil.cpp"
void RegisterBountyActorAtPortalDestination(
    LPCSTR szActorName,
    VACDID idACDPortal)
{
    SNO snoActor = SNOFileLookupGUID(ACTOR_SNO, szActorName);
    if (snoActor == SNO_NONE)
    {
        return;
    }



    SNO snoQuest = SQuestGetBountyQuestSNOWithActorObjective(snoActor);
    if (snoQuest == SNO_NONE)
    {
        return;
    }



    ActorCommonData & tACDPortal = ACDGet(idACDPortal);
    if (tACDPortal.GetGizmoType() != ACTORGIZMO_PORTAL)
    {
        return;
    }




    SNO snoWorld = ActorDefinitionGetTagMapValueSNO(TAG_ACTOR_GIZMO_PORTAL_SPECIFIED_WORLD, tACDPortal.snoActor);
    SNO snoLevelArea = ActorDefinitionGetTagMapValueSNO(TAG_ACTOR_GIZMO_PORTAL_DEST_LEVEL_AREA, tACDPortal.snoActor);
    if (snoWorld == SNO_NONE || snoLevelArea == SNO_NONE)
    {
        return;
    }





    ACDID idACDActor = ActivateActorMarkerInWorld(snoActor, snoWorld, snoLevelArea);

    if (idACDActor != ACDID_INVALID)
    {
        Minimap::ActorAutoPing(idACDActor, snoQuest);
    }
    else
    {

        SQuestsRegisterBountyActor(snoActor, snoWorld, snoLevelArea, snoQuest);
    }
}

#include "../../Common/ActorInventory.h"
uint64 ExperienceGetRestXPToGrant(const ActorCommonData & tACDPlayer, const uint64 nXPGranted);  // X/Common/ExperienceCommon.cpp:117
// NON_MATCHING: tiered reward float registers differ; cgplaceNull lacks the original dynamically initialized merged-global address.
#line 10305 "X/Server/LuaScript/ScriptUtil.cpp"
static void sGetQuestReward(
    D3::Quests::QuestReward & tReward,
    const ExperienceTable & tExpTable,
    ACDID idACDPlayer,
    uint32 nXPTier,
    uint32 nGoldTier,
    GBHandle gbhItem,
    int32 nTieredRiftDepth,
    int32 nItemStackCount)
{
    ActorCommonData * ptACDPlayer = ACDTryToGet(idACDPlayer);
    if (!ptACDPlayer)
    {
        return;
    }

    int32 nPlayerLevel = ptACDPlayer->AttributesGetInt(FastAttribKey(ATTRIBUTE_LEVEL));

    int32 nTreasureLevel = GameGetMonsterLevel();
    nTreasureLevel = Math::Min(nTreasureLevel, nPlayerLevel);
    nTreasureLevel = Math::Max(nTreasureLevel, 1);

    uint64 nXPGranted = nXPTier ? tExpTable.ptExperienceLevels[nTreasureLevel - 1].nQuestTierXP[nXPTier - 1] : 0;
    int32 nGoldGranted = nGoldTier ? tExpTable.ptExperienceLevels[nTreasureLevel - 1].nQuestTierGold[nGoldTier - 1] : 0;


    float flMagicFindFromHandicap;
    float flGoldFindFromHandicap;
    float flXPBonusFromHandicap;
    GameGetPlayerHandicapValues(GameGetHandicapLevel(), flMagicFindFromHandicap, flGoldFindFromHandicap, flXPBonusFromHandicap);

    if (nTieredRiftDepth == -1)
    {
        nXPGranted = Math::FloatToInt(nXPGranted * (1.0f + flXPBonusFromHandicap));
        nGoldGranted = Math::FloatToInt(nGoldGranted * (1.0f + flGoldFindFromHandicap));
    }

    else if (nTieredRiftDepth >= 0)
    {
        ScopedGameBalanceDefinitionLockR pdefGB(GlobalSNOGet(GLOBALSNO_GB_TIERED_LOOT_RUN_LEVELS));

        int32 nGameBalanceIndex = nTieredRiftDepth;
        nGameBalanceIndex = Math::Min(nGameBalanceIndex, (int32)pdefGB->tTieredLootRunLevelTable.serTieredLootRunLevels.ElementCount(sizeof(TieredLootRunLevel)) - 1);

        if (nGameBalanceIndex < pdefGB->tTieredLootRunLevelTable.serTieredLootRunLevels.dwSize / (uint32)sizeof(TieredLootRunLevel))
        {
            const TieredLootRunLevel & tTieredLootRunLevel = pdefGB->tTieredLootRunLevelTable.ptTieredLootRunLevels[nGameBalanceIndex];

            nXPGranted = tTieredLootRunLevel.tLootRunValues.flXPBonus * nXPGranted;
            nGoldGranted = tTieredLootRunLevel.tLootRunValues.flGoldFind * nGoldGranted;

            if (ItemHasLabel(gbhItem.gbid, ITL_BLOODSHARD) && nItemStackCount > 0)
            {

                nItemStackCount = Math::FloatFloorToInt(nItemStackCount * tTieredLootRunLevel.tLootRunValues.flBloodShardBonus);
            }
        }
    }

    uint64 nXP;
    int32 nGold;

    tReward.Clear();
    tReward.set_sno_quest(GlobalSNOGet(GLOBALSNO_QUEST_DYNAMIC));
    tReward.set_gold_granted(nGoldGranted);
    tReward.set_xp_granted(nXPGranted);
    tReward.set_bonus_xp_granted(ExperienceGetRestXPToGrant(*ptACDPlayer, nXPGranted));

    ACDID idACDItem = ACDID_INVALID;
    if (GBIDExists(gbhItem) && gbhItem.eType == GB_ITEMS)
    {
        LootSpecifier tSpecifier;
        tSpecifier.hGBItem = gbhItem;
        tSpecifier.snoActorForFlippy = GameBalanceGetActorSNO(tSpecifier.hGBItem);
        tSpecifier.eIQL = IQL_NORMAL;
        tSpecifier.fAutoIdentify = TRUE;
        tSpecifier.nGoldAmount = 0;

        if (ItemGetMaxStackSize(tSpecifier.hGBItem, NULL, NULL) > 1 && nItemStackCount != 0)
        {

            tSpecifier.uDesiredStackCount = nItemStackCount;
        }

        idACDItem = LootCreateItem(tSpecifier, cgplaceNull, NULL);
    }

    if (idACDItem != ACDID_INVALID)
    {
        ActorCommonData & tACDItem = ACDGet(idACDItem);
        ItemGeneratorToDefaults(*tReward.mutable_item());
        ItemGeneratorPopulateFromACDID(tACDItem, *tReward.mutable_item(), FALSE);

        SACDInventoryPickupOrSpillOntoGround(*ptACDPlayer, tACDItem, FALSE, NULL, TRUE);
        LuaScriptInstanceAddRefACD(LuaScriptGetExecutingScriptInstance(), idACDItem);
    }
}

#line 13673 "X/Server/LuaScript/ScriptUtil.cpp"
static void sGrantXPAndGoldForTieredRiftReward(
    Player * ptPlayer,
    D3::Quests::QuestReward & tReward)
{
    ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
    if (ptACDPlayer)
    {
        if (tReward.has_xp_granted())
        {
            ExperienceGrantDirectly(*ptACDPlayer, tReward.xp_granted(), TRUE, FALSE);
        }

        if (tReward.has_bonus_xp_granted())
        {
            ExperienceGrantDirectly(*ptACDPlayer, tReward.bonus_xp_granted(), TRUE, FALSE);
        }

        if (tReward.has_gold_granted())
        {
            ptACDPlayer->ModifyCurrencyAmount(tReward.gold_granted(), CURRENCY_GOLD, GMR_QUEST, FALSE);
            SGameStatsPlayerCounterIncrement(ptPlayer, STAT_COUNTER_PLAYER_CURRENCY_PICKED_UP, CURRENCY_GOLD, 0, tReward.gold_granted());
            SGameStatsPlayerCounterIncrement(ptPlayer, STAT_COUNTER_PLAYER_CURRENCY_RECEIVED_FROM_QUESTS, CURRENCY_GOLD, 0, tReward.gold_granted());
            SQuestEvaluateGoldRewardedAchievementEvent(*ptPlayer, tReward.gold_granted());
        }
    }
}

#line 13768 "X/Server/LuaScript/ScriptUtil.cpp"
void QuestShowTieredRiftEndScreen(
    BOOL bFinishedInTime,
    uint32 nRiftDepth,
    int32 nTotalTimeMS,
    uint32 nGoldTier,
    uint32 nXPTier)
{


    if (nGoldTier > MAX_QUEST_REWARD_GOLD_TIER || nXPTier > MAX_QUEST_REWARD_XP_TIER)
    {
        return;
    }


    ScopedGameBalanceDefinitionLockR ptGameBalDef(GlobalSNOGet(GLOBALSNO_GB_EXPERIENCE));
    const ExperienceTable & tExpTable = ptGameBalDef->tExperienceTable;


    for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {
        if (NephalemRiftManager::PlayerPaidForRift(ptPlayer))
        {
            D3::Quests::QuestReward tReward;
            sGetQuestReward(tReward, tExpTable, ptPlayer->idPlayerACD, nXPTier, nGoldTier, cghgbNone, nRiftDepth, 0);
            sGrantXPAndGoldForTieredRiftReward(ptPlayer, tReward);

            NephalemRiftManager::PlayerSendRiftEndScreenInfo(*ptPlayer, TRUE, nTotalTimeMS, nRiftDepth, tReward, ChallengeRift::RewardScreenInfo());
        }
    }
}

#line 13700 "X/Server/LuaScript/ScriptUtil.cpp"
void QuestShowTieredRiftReward(
    BOOL bFinishedInTime,
    uint32 nRiftDepth,
    uint32 nGoldTier,
    uint32 nXPTier,
    LPCSTR szItemName,
    ACDID idACDSpecificPlayer,
    int32 nItemStackCount)
{



    if (nGoldTier > MAX_QUEST_REWARD_GOLD_TIER || nXPTier > MAX_QUEST_REWARD_XP_TIER)
    {
        return;
    }



    GBHandle gbhItem = cghgbNone;

    if (szItemName && szItemName[0])
    {
        gbhItem.Set(GB_ITEMS, StringToGBID(szItemName));
        if (!GBIDExists(gbhItem))
        {
            return;
        }
    }



    ScopedGameBalanceDefinitionLockR ptGameBalDef(GlobalSNOGet(GLOBALSNO_GB_EXPERIENCE));
    const ExperienceTable & tExpTable = ptGameBalDef->tExperienceTable;

    if (idACDSpecificPlayer == ACDID_INVALID)
    {
        for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
        {
            if (NephalemRiftManager::PlayerPaidForRift(ptPlayer))
            {
                D3::Quests::QuestReward tReward;
                sGetQuestReward(tReward, tExpTable, ptPlayer->idPlayerACD, nXPTier, nGoldTier, gbhItem, nRiftDepth, nItemStackCount);
                SQuestsSendTieredRiftReward(*ptPlayer, bFinishedInTime, nRiftDepth, tReward);


                sGrantXPAndGoldForTieredRiftReward(ptPlayer, tReward);
            }
        }
    }
    else
    {
        Player * ptPlayer = PlayerGetByACD(idACDSpecificPlayer);
        if (ptPlayer)
        {
            D3::Quests::QuestReward tReward;
            sGetQuestReward(tReward, tExpTable, ptPlayer->idPlayerACD, nXPTier, nGoldTier, gbhItem, nRiftDepth, nItemStackCount);
            SQuestsSendTieredRiftReward(*ptPlayer, bFinishedInTime, nRiftDepth, tReward);


            sGrantXPAndGoldForTieredRiftReward(ptPlayer, tReward);
        }
    }

}

#line 10404 "X/Server/LuaScript/ScriptUtil.cpp"
void QuestSendDynamicObjectiveReward(
    uint32 nGoldTier,
    uint32 nXPTier,
    LPCSTR szItemName)
{
    if (nGoldTier >= MAX_QUEST_REWARD_GOLD_TIER || nXPTier >= MAX_QUEST_REWARD_XP_TIER)
    {
        return;
    }




    GBHandle gbhItem = cghgbNone;

    if (szItemName && szItemName[0])
    {
        gbhItem.Set(GB_ITEMS, StringToGBID(szItemName));

        if (!GBIDExists(gbhItem))
        {
            return;
        }
    }



    ScopedGameBalanceDefinitionLockR ptGameBalDef(GlobalSNOGet(GLOBALSNO_GB_EXPERIENCE));
    const ExperienceTable & tExpTable = ptGameBalDef->tExperienceTable;

    for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {
        D3::Quests::QuestReward tReward;
        sGetQuestReward(tReward, tExpTable, ptPlayer->idPlayerACD, nXPTier, nGoldTier, gbhItem, -1, 0);
        SQuestsSendQuestReward(*ptPlayer, FALSE, tReward, FALSE);
    }
}

#include "../../Sigma/Memory/SigmaMemory.h"
#line 10443 "X/Server/LuaScript/ScriptUtil.cpp"
void QuestHideCompletedDynamicObjectives()

{
    GameCommonData & tGCD = GameGetActiveGameCommonData();
    QuestGlobals * ptQuestGlobals = tGCD.ptQuestGlobals;

    for (int32 i = 0; i < 10; i++)
    {
        if (ptQuestGlobals->tDynamicQuestObjectives[i].eObjectiveType != OBJECTIVE_INVALID)
        {
            if (!ptQuestGlobals->arDynamicQuestObjectiveHidden[i] && ptQuestGlobals->arDynamicQuestObjectiveCompleted[i])
            {
                ptQuestGlobals->arDynamicQuestObjectiveHidden[i] = TRUE;
                ptQuestGlobals->arDynamicQuestObjectiveDirty[i] = TRUE;
            }
        }
    }

    SigmaMemoryClear(ptQuestGlobals->tDynamicQuestObjectives, sizeof(ptQuestGlobals->tDynamicQuestObjectives));
    for (int32 i = 0; i < 10; i++)
    {
        ptQuestGlobals->tDynamicQuestObjectives[i].eObjectiveType = OBJECTIVE_INVALID;
    }
    for (int32 i = 0; i < 10; i++)
    {
        ptQuestGlobals->arDynamicQuestObjectiveState[i] = 0;
        ptQuestGlobals->arDynamicQuestObjectiveCompleted[i] = FALSE;
        ptQuestGlobals->arDynamicQuestObjectiveHidden[i] = FALSE;
    }

    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetAllConnected(listDestPlayers, -1, FALSE);

    SimpleMessage tMessage;
    MessageSendToClient(listDestPlayers, SMSG_DYNAMIC_QUEST_RESET, &tMessage, sizeof(tMessage));

}

#line 10482 "X/Server/LuaScript/ScriptUtil.cpp"
void ChallengeKillAllMonsters(
    LPCSTR szQuestName)
{
    SNO snoQuest = SNOFileLookupGUID(QUEST_SNO, szQuestName);
    if (snoQuest == SNO_NONE)
    {
        return;
    }


    ScopedQuestDefinitionLockR pdefQuest(snoQuest);
    if (pdefQuest->eQuestType != QUEST_TYPE_CHALLENGE)
    {
        return;
    }


    if (pdefQuest->snoChallengeWorld == SNO_NONE)
    {
        return;
    }


    SWorldID idSWorld = SWorldFindBySNO(pdefQuest->snoChallengeWorld);
    SWorld * ptSWorld = SWorldTryToGet(idSWorld);
    if (!ptSWorld)
    {
        return;
    }

    ACDIterator tIt;
    while (ActorCommonDataIterate(tIt))
    {
        ActorCommonData * ptACD = tIt.ptElement;

        if (ptACD->GetSWorldID() == idSWorld &&
            ptACD->IsMonster() &&
            ptACD->IsMonsterTeam() &&
            ptACD->IsAlive())
        {
            ActorID idActor = (ActorID)ACDGetOwner(ptACD->id);
            Actor * ptActor = ActorGet(idActor);
            if (ptActor->ptAI)
            {
                BrainAPIDoDeath(idActor, ACTORID_INVALID, TRUE, TRUE, TRUE, NULL, NULL);
            }
        }
    }


    ptSWorld->bChallengeMonstersCleared = TRUE;
}

#line 2101 "X/Server/LuaScript/ScriptUtil.cpp"
void sActorDeleteFromScript(
    VACDID idACD)
{
    if (ACDIsPlayer(idACD))
    {
        return;
    }


    ActorID idActor = (ActorID)ACDGetOwner(idACD);


    ActorCommonData & tACD = ACDGet(idACD);
    tACD.RemoveFromGroups(ACDID_INVALID);

    if (tACD.IsItem() &&
        LuaScriptExecutingScriptIsConductor() &&
        ActorInventoryGetUltimateOwner(&tACD) != ACDID_INVALID)
    {
        return;
    }






    LuaScriptInstanceRemoveRefACD(LuaScriptGetExecutingScriptInstance(), idACD);

    if (tACD.IsMonster() && tACD.IsAlive())
    {
        SQuestsOnActorRemoved(tACD);
    }
    else if (tACD.GetActorType() == ACTORTYPE_GIZMO ||
             tACD.GetActorType() == ACTORTYPE_SERVER_PROP ||
             tACD.GetActorType() == ACTORTYPE_PROJECTILE)
    {


        PlayEffectMessage tMessage;
        tMessage.ann = ANNFromACD(tACD.id);
        tMessage.eEffectType = EFFECT_DEATH;

        PlayerList listDestPlayers(PlayerListGetElementPool());
        PlayerListGetThatKnowACD(listDestPlayers, tACD.id, FALSE);
        MessageSendToClient(listDestPlayers, SMSG_PLAY_EFFECT, &tMessage, sizeof(tMessage));
    }

    ActorFlagForDeletion(idActor);
}

#include "../../Common/SceneCommon.h"
#line 12850
void SetKeyWardenKilled(
    VACDID idACDKeyWarden)
{
    SGame & tSGame = SGameManager::GameGet();
    if (idACDKeyWarden == ACDID_INVALID || !tSGame.ptSGameGlobals)
        return;



    ActorCommonData * ptACDKeyWarden = ACDTryToGet(idACDKeyWarden);
    if (!ptACDKeyWarden || !ptACDKeyWarden->IsInWorld())
        return;




    WorldLocation tKeyWardenLocation = ptACDKeyWarden->GetWorldLocation();
    SNO snoKeyWardenLevelArea = ScenesCommonLevelAreaFromPlace(tKeyWardenLocation.idSScene, tKeyWardenLocation.transform.wp, TRUE);
    if (snoKeyWardenLevelArea == SNO_NONE)
        return;



    Act eAct = LevelAreaGetAct(snoKeyWardenLevelArea);
    int32 nAct = -1;
    switch (eAct)
    {
    case ACT_1: nAct = 1; break;
    case ACT_2: nAct = 2; break;
    case ACT_3: nAct = 3; break;
    case ACT_4: nAct = 4; break;
    default: return;
    }
#line 12895
    if (tSGame.ptSGameGlobals->dwKeyWardenKilledFlags & (1 << (nAct - 1)))
        return;




    tSGame.ptSGameGlobals->dwKeyWardenKilledFlags |= (1 << (nAct - 1));

    if (GameIsOpenWorld())
    {
        SNO snoBossEncounter = QuestsOpenWorldGetKeyWardenBossEncounter(nAct);
        if (snoBossEncounter != SNO_NONE)
        {
            QuestsAddOpenWorldTrackedSNO(snoBossEncounter);
        }
    }
}

#include "../../Common/TeamCommon.h"
#line 6119
static int32 nGetHostileActorsInScene(
    SceneCommonData & tSCD,
    Team eTeam)
{
    int32 nCount = 0;

    SScene * ptSScene = SSceneTryToGet(tSCD.idSScene);
    if (ptSScene)
    {
        POS pos = ptSScene->listACDInScene.GetHeadPosition();
        while (pos)
        {
            ACDID idACD = (ACDID)ptSScene->listACDInScene.GetNext(pos);
            ActorCommonData * ptACD = ACDTryToGet(idACD);
            if (ptACD)
            {
                if (ptACD->IsAlive() &&
                    !TeamsAreFriendly(ptACD->GetNaturalTeam(), eTeam))
                    ++nCount;
            }
        }



        InactiveMarkerList * ptMarkerScene = tSCD.ptMarkerScene;
        if (ptMarkerScene && ptMarkerScene->ptInactiveMarkerList)
        {
            pos = ptMarkerScene->ptInactiveMarkerList->GetHeadPosition();
            while (pos)
            {
                InactiveBaseMarker * ptInactiveMarker = ptMarkerScene->ptInactiveMarkerList->GetNext(pos);
                if (ptInactiveMarker->eInactiveMarkerType == INACTIVE_MONSTER)
                {
                    InactiveMonsterMarker * ptInactiveMonster = (InactiveMonsterMarker *)ptInactiveMarker;
                    Team eCurrTeam = ptInactiveMonster->tSavedAttributes.nTeam;
                    if (ptInactiveMonster->tSavedAttributes.flHitPoints > EPSILON &&
                        !TeamsAreFriendly(eCurrTeam, eTeam))
                        ++nCount;
                }
            }
        }
    }
    return nCount;
}
#line 6168
int32 WorldGetNumberOfHostileMonstersLeft(
    VACDID idACD)
{
    int32 nCount = 0;

    ActorCommonData & tACD = ACDGet(idACD);
    if (tACD.IsInWorld())
    {




        const WorldLocation & tWorldLoc = tACD.GetWorldLocation();
        const WorldCommonData * ptWCD = WorldTryGetCommonData(tWorldLoc.idSWorld);
        if (ptWCD)
        {




            Team eTeam = tACD.GetTeam();
            SceneCommonData * ptSCD = NULL;
            SceneCommonDataIterator tIterator;
            while (SceneCommonDataArrayIterate(tIterator, ptSCD, SkipHolodeckScenes))
            {
                if (ptSCD->idSWorld == ptWCD->idSWorld)
                {
                    nCount += nGetHostileActorsInScene(*ptSCD, eTeam);
                }
            }
        }
    }
    return nCount;
}

#line 664
ActorIDArray * PlayersInTieredRift()

{
    ActorIDArray & tArrACDs = LuaScriptGetThreadSafeActorIDArray();
    tArrACDs.nNumActorIDs = 0;
    tArrACDs.bWantAddRef = FALSE;

    GameCommonData & tGCD = GameGetActiveGameCommonData();
    PlayerGlobals * ptPlayerGlobals = tGCD.ptPlayerGlobals;
    for (int32 i = 0; i < MAX_PLAYERS; ++i)
    {
        const Player & tPlayer = ptPlayerGlobals->tPlayerArray[i];
        ActorCommonData * ptACDPlayer = ACDTryToGet(tPlayer.idPlayerACD);

        if (ptACDPlayer && ACDIsInTieredLootRun(*ptACDPlayer))
        {
            tArrACDs.arActorIDs[tArrACDs.nNumActorIDs] = tPlayer.idPlayerACD;
            ++tArrACDs.nNumActorIDs;

            if (tArrACDs.nNumActorIDs == ACTOR_ID_ARRAY_MAX_ACTORS)
                break;
        }
    }
    return &tArrACDs;
}

#include "../System/Adventure.h"
#line 178
ACDID ActorIDFromName(
    LPCSTR szInstanceName)
{
    ACDID idACD = ACDID_INVALID;

    CRefString sNameToFind(szInstanceName);
    uint32 nTokenLength = strlen(LOCAL_MARKER_SCOPE_TOKEN);
    if (!strncmp(LOCAL_MARKER_SCOPE_TOKEN, szInstanceName, nTokenLength))
    {
        BOOL fStripToken = TRUE;

        uint16 nAdventureParentIndex = LuaScriptGetExecutingScriptAdventureParentIndex();
        if (nAdventureParentIndex != 0xFFFF)
        {
            LPCSTR pszScriptAdvParentName = AdventureGetParentName(nAdventureParentIndex);
            sNameToFind.Format("%s:%s", pszScriptAdvParentName, szInstanceName + nTokenLength);
            fStripToken = FALSE;
        }

        if (fStripToken)
        {

            sNameToFind = szInstanceName + nTokenLength;
        }
    }

    uint32 tNameToFind = MarkerStringHash(sNameToFind.str());

    ActorIterator tIt;
    while (ActorDataArrayIterate(tIt))
    {
        const Actor * ptActor = tIt.ptElement;

        if (ptActor->tName == tNameToFind)
        {
#line 227
            idACD = ptActor->idACD;
            break;
        }
    }


    if (idACD == ACDID_INVALID)
    {

        idACD = ActivateMarkerName(sNameToFind);
    }


    if (idACD != ACDID_INVALID)
    {
        LuaScriptInstanceAddRefACD(LuaScriptGetExecutingScriptInstance(), idACD);
    }
#line 249
    return idACD;
}

#include "../../Common/Definitions/ConversationListDefinition.h"
#line 2704
void PlayerConversationListSetHeard(
    LPCSTR szConversationList,
    BOOL bHeard,
    ACDID idACDPlayer)
{
    SNO snoConvList = SNOFileLookupGUID(CONVERSATIONLIST_SNO, szConversationList);
    if (snoConvList == SNO_NONE)
        return;




    LuaVerifySnosAreReferenced(snoConvList, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE);

    ScopedConversationListDefinitionLockR ptConvListDef(snoConvList);
    int nCount = ptConvListDef->serConvList.dwSize / sizeof(ConversationListEntry);

    Player * ptPlayer = PlayerGetByACD(idACDPlayer);
    if (ptPlayer)
    {
        for (int32 i = 0; i < nCount; ++i)
        {
            ConversationListEntry & tEntry = ptConvListDef->ptConvList[i];
            if (bHeard)
            {
                SPlayerMarkConversationAsHeard(ptPlayer, tEntry.snoConv);
            }
            else
            {
                SPlayerClearHeardConversation(*ptPlayer, tEntry.snoConv);
            }
        }
    }
    else
    {

        for (ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
        {
            for (int32 i = 0; i < nCount; ++i)
            {
                ConversationListEntry & tEntry = ptConvListDef->ptConvList[i];

                if (bHeard)
                {
                    SPlayerMarkConversationAsHeard(ptPlayer, tEntry.snoConv);
                }
                else
                {
                    SPlayerClearHeardConversation(*ptPlayer, tEntry.snoConv);
                }
            }
        }
    }
}

#line 4914
double ItemGetAttributeFromInventorySlot(
    ACDID idACD,
    InventorySlotType eSlotType,
    int32 eAttrib,
    int32 nParam)
{
    double flValue = 0.0;

    if ((eSlotType < SLOT_PLAYER_BODYSLOT_START || eSlotType >= SLOT_PLAYER_BODYSLOT_END) &&
        (eSlotType < SLOT_HIRELING_START || eSlotType >= SLOT_HIRELING_END))
        return flValue;



    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (!ptACD)
        return flValue;




    if (!ACDIsPlayer(idACD) && eSlotType >= SLOT_PLAYER_BODYSLOT_START && eSlotType < SLOT_PLAYER_BODYSLOT_END)
    {
        switch (eSlotType)
        {
        case SLOT_PLAYER_LEFT_HAND: eSlotType = SLOT_HIRELING_LEFT_HAND; break;
        case SLOT_PLAYER_RIGHT_HAND: eSlotType = SLOT_HIRELING_RIGHT_HAND; break;
        case SLOT_PLAYER_RIGHT_FINGER: eSlotType = SLOT_HIRELING_RIGHT_FINGER; break;
        case SLOT_PLAYER_LEFT_FINGER: eSlotType = SLOT_HIRELING_LEFT_FINGER; break;
        case SLOT_PLAYER_NECK: eSlotType = SLOT_HIRELING_NECK; break;
        default: return flValue;
        }
    }
#line 4965
    IVector2D tPoint(0, 0);
    ACDID idACDItem = ACDInventoryGetSquareItem(*ptACD, eSlotType, tPoint);
    ActorCommonData * ptACDItem = ACDTryToGet(idACDItem);
    if (ptACDItem)
    {



        if (eAttrib > AXEBADDATA && eAttrib < NUM_ATTRIBUTES)
        {
            FastAttribKey tKey = MakeAttribKey(eAttrib, nParam);
            AttributeDataType eAttribType = KeyGetDataType(tKey);

            switch (eAttribType)
            {
            case ATTRIBUTEDT_FLOAT:
                flValue = ptACDItem->AttributesGetFloat(tKey);
                break;
            case ATTRIBUTEDT_INT:
                flValue = ptACDItem->AttributesGetInt(tKey);
                break;
            }
        }
    }
#line 4993
    return flValue;
}

#line 6872
BOOL GameSetOpenWorldMode(
    ACDID idActorPlayer,
    OpenWorldMode eOpenWorldMode,
    Act eOpenWorldModeAct,
    int32 nOpenWorldParam,
    LPCSTR szHubWorld,
    LPCSTR szHubEntranceName)
{
    SNO snoHubWorld = SNOFileLookupGUID(WORLD_SNO, szHubWorld);
    if (snoHubWorld == SNO_NONE)
        return FALSE;
#line 6887
    uint32 uGUID = StartingLocationNameToGUID(szHubEntranceName);
    if (uGUID == (uint32)-1)
        return FALSE;
#line 6894
    SWorldID idSWorld = SWorldFindBySNO(snoHubWorld);
    SWorld * ptSWorld = SWorldTryToGet(idSWorld);
    if (!ptSWorld)
        return FALSE;
#line 6902
    EntranceLocation tEntranceLocation;
    if (!ptSWorld->tEntranceMap.Lookup(uGUID, tEntranceLocation))
        return FALSE;
#line 6909
    Player * ptPlayerInitiating = PlayerGetByACD(idActorPlayer);

    BOOL bSuccess = SGameSetOpenWorldMode(ptPlayerInitiating, eOpenWorldMode, eOpenWorldModeAct, nOpenWorldParam, snoHubWorld, uGUID);
    return bSuccess;
}

#line 11269
void SetCameraObserver(
    ACDID idACDPlayer,
    LPCSTR szObserverName,
    ACDID idACDToOrbit)
{
    SNO snoObserver = SNOFileLookupGUID(OBSERVER_SNO, szObserverName);
    if (!SNOFileExists(OBSERVER_SNO, snoObserver))
        return;
#line 11281
    Player * ptPlayer = PlayerGetByACD(idACDPlayer);
    if (!ptPlayer)
        return;
#line 11287
    PlayerList listDestPlayers(PlayerListGetElementPool());
    listDestPlayers.AddTail_Tracked(ptPlayer);

    PlayerSetCameraObserverMessage tCameraObserverMessage;
    tCameraObserverMessage.snoObserver = snoObserver;

    ActorCommonData * ptACDToOrbit = ACDTryToGet(idACDToOrbit);
    if (ptACDToOrbit)
    {
        tCameraObserverMessage.wpToOrbit = ptACDToOrbit->GetPlace();
    }

    MessageSendToClient(listDestPlayers, SMSG_SET_CAMERA_OBSERVER, &tCameraObserverMessage, sizeof(tCameraObserverMessage));
}

#include "../../Common/Networking/Network.h"
#line 11203
void CameraFocusOnActor(
    VACDID idActor,
    BOOL bSnap,
    float flDuration)
{
    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetAllInGame(listDestPlayers, -1, FALSE, GAMECONNECTIONID_INVALID);

    ActorCommonData * ptACD = ACDTryToGet(idActor);
    if (!ptACD)
        return;
#line 11217
    POS pos = listDestPlayers.GetHeadPosition();
    POS posOld;
    while (pos)
    {
        posOld = pos;
        Player * ptPlayer = listDestPlayers.GetNext(pos);
        if (!ptACD->IsKnownByPlayer(*ptPlayer))
        {
            listDestPlayers.RemoveAt(posOld);
        }
    }

    CameraFocusMessage tFocusMessage;
    tFocusMessage.ann = ANNFromACD(idActor);
    tFocusMessage.bSnap = bSnap;
    tFocusMessage.flDuration = flDuration;
    MessageSendToClient(listDestPlayers, SMSG_CAMERA_FOCUS_ACD, &tFocusMessage, sizeof(tFocusMessage));
}

#line 12164
void PlayerSpendParagonPoints(
    ACDID idPlayer,
    int32 nCategory,
    int32 nAttribute,
    int32 nNumPoints)
{
    ActorCommonData * ptACDPlayer = ACDTryToGet(idPlayer);
    Player * ptPlayer = PlayerGetByACD(idPlayer);
    if (!ptACDPlayer || !ptPlayer)
        return;
#line 12177
    PlayerClass ePlayerClass = PlayerGetPlayerClass(ptACDPlayer->id);

    GBHandleList listBonuses(GBGetHandlePool());
    GBEnumerate(GB_PARAGON_BONUSES, listBonuses);
    POS pos = listBonuses.GetHeadPosition();
    while (pos)
    {
        const GBHandle & hGBBonus = listBonuses.GetNext(pos);
        ScopedParagonBonusLockR pdefParagonBonus(hGBBonus.gbid);

        if (pdefParagonBonus->ePlayerClass == PLAYER_CLASS_NONE || pdefParagonBonus->ePlayerClass == ePlayerClass)
        {
            int32 nCurrentCategory = pdefParagonBonus->nCategory;
            int32 nCurrentAttribute = pdefParagonBonus->nIndex - 1;

            if (nCurrentCategory == nCategory && nCurrentAttribute == nAttribute)
            {
                SPlayerSpendParagonPoints(*ptPlayer, hGBBonus.gbid, nNumPoints);
                break;
            }
        }
    }
}

#line 14054
void GameDestroyDeathPortalWorld(
    LPCSTR szWorld)
{

    SNO snoWorld = SNOFileLookupGUID(WORLD_SNO, szWorld);
    if (snoWorld == SNO_NONE)
        return;
#line 14065
    SWorldID idSWorld = WorldCommonFindBySNO(snoWorld, TRUE);
    SWorld * ptSWorld = SWorldTryToGet(idSWorld);
    if (!ptSWorld)
        return;
#line 14073
    if (!(ptSWorld->dwFlags & (1 << FLAG_SWORLD_FROM_DEATH_PORTAL)))
        return;
#line 14080
    SWorldRemovePlayersAndFlagForDeletion(idSWorld);


    uint32 uDynamicReturnLocationGUID = (uint32)-1;
    if (ptSWorld->tDynamicReturnLocation.snoWorld != SNO_NONE)
    {
        uDynamicReturnLocationGUID = ptSWorld->tDynamicReturnLocation.uEntranceGUID;
    }


    ActorIterator tIt;
    while (ActorDataArrayIterate(tIt))
    {
        Actor * ptActor = tIt.ptElement;
        if (!ptActor)
            continue;
#line 14099
        ActorCommonData * ptACD = ACDTryToGet(ptActor->idACD);
        if (!ptACD)
            continue;
#line 14105
        uint32 uActorDynamicEntranceGUID = ptACD->AttributesGetInt(ATTRIBUTE_DYNAMIC_ENTRANCE_GUID);
        if (uDynamicReturnLocationGUID != (uint32)-1 && uActorDynamicEntranceGUID == uDynamicReturnLocationGUID)
        {

            ActorFlagForDeletion(ptActor->id);
            continue;
        }

        GizmoType eGizmoType = ptACD->GetGizmoType();
        if (eGizmoType == ACTORGIZMO_DUNGEON_PORTAL || eGizmoType == ACTORGIZMO_PORTAL)
        {
            SNO snoPortalWorld = ActorDefinitionGetTagMapValueSNO(TAG_ACTOR_GIZMO_PORTAL_SPECIFIED_WORLD, ptACD->snoActor);
            if (snoPortalWorld == snoWorld)
            {
                FadeOut(ptActor->idACD, 60);
                ActorDeleteSoon(ptActor->id, 2);
            }
        }
    }
}

#line 11146
void CameraScriptedSequenceStop()

{

    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetAllConnected(listDestPlayers, -1, FALSE);

    SNO snoPower = GlobalSNOGet(GLOBALSNO_POWERS_CAMERA_FOCUS_BUFF);
    SNO snoPetPower = GlobalSNOGet(GLOBALSNO_POWERS_CAMERA_FOCUS_PET_BUFF);

    POS pos = listDestPlayers.GetHeadPosition();
    while (pos)
    {
        Player * ptPlayer = listDestPlayers.GetNext(pos);
        BuffID idBuff = PowerAPI_BuffFindOnACD(snoPower, ACDGet(ptPlayer->idPlayerACD), ACDID_INVALID);
        if (idBuff != BUFFID_INVALID)
        {
            PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
            if (ptBuff)
            {
                PowerAPI_BuffFlagForDeletion(ptBuff, FALSE, TRUE, BUFF_REMOVAL_REASON_DEFAULT, TRUE);

                int32 nPos;
                ACDID idACDPet = PlayerGetFirstPet(ptPlayer->idPlayerACD, PETTYPE_NONE, nPos);
                while (idACDPet != ACDID_INVALID)
                {
                    ActorCommonData * ptACDPet = ACDTryToGet(idACDPet);

                    if (ptACDPet && ptACDPet->IsAlive())
                    {
                        BuffID idPetBuff = PowerAPI_BuffFindOnACD(snoPetPower, *ptACDPet, ACDID_INVALID);
                        if (idPetBuff != BUFFID_INVALID)
                        {
                            PowerAPIBuffInstance * ptPetBuff = PowerAPI_BuffTryToGet(idPetBuff);
                            if (ptPetBuff)
                            {
                                PowerAPI_BuffFlagForDeletion(ptPetBuff, FALSE, TRUE, BUFF_REMOVAL_REASON_DEFAULT, TRUE);
                                ActorID idActorPet = (ActorID)ACDGetOwner(idACDPet);
                                if (ActorTryToGetAI(idActorPet))
                                {
                                    ActorAISetStatusFlag(idActorPet, ACTOR_AI_DISABLE_UPDATES_BIT, FALSE);
                                }
                            }
                        }
                    }

                    idACDPet = PlayerGetNextPet(ptPlayer->idPlayerACD, PETTYPE_NONE, nPos);
                }
            }
        }
    }

    SimpleMessage tMessage;
    MessageSendToClient(listDestPlayers, SMSG_CAMERA_SCRIPTED_SEQUENCE_STOP, &tMessage, sizeof(tMessage));
}

#include "../../Common/Movement/ActorMovement.h"

#line 11030
void CameraScriptedSequenceStart(
    ACDID idACDFocusActor,
    BOOL bHidePlayers,
    BOOL bDisableAutoCancellation)
{
    ActorCommonData * ptACD = ACDTryToGet(idACDFocusActor);
    if (!ptACD)
        return;
#line 11057
    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetAllConnected(listDestPlayers, -1, FALSE);

    PowerAPIBuffStartData tBuffStartData(POWERID_INVALID);
    tBuffStartData.bInfinite = true;
    tBuffStartData.bUndispellable = true;
    tBuffStartData.bPositiveEffect = true;
    tBuffStartData.snoPower = GlobalSNOGet(GLOBALSNO_POWERS_CAMERA_FOCUS_BUFF);
    tBuffStartData.eStackType = BUFF_STACKTYPE_NEVER;
    tBuffStartData.bImmobilize = true;
    tBuffStartData.bFailsIfImmobilizeImmune = false;
    FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_INVULNERABLE, TRUE);
    FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_CANT_START_DISPLAYED_POWERS, TRUE);
    FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_RESOURCE_DEGENERATION_STOPPED, TRUE);

    if (bHidePlayers)
    {
        FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_HIDDEN, TRUE);
    }

    PowerAPIBuffStartData tPetBuffStartData(POWERID_INVALID);
    tPetBuffStartData.bInfinite = true;
    tPetBuffStartData.bUndispellable = true;
    tPetBuffStartData.bPositiveEffect = true;
    tPetBuffStartData.snoPower = GlobalSNOGet(GLOBALSNO_POWERS_CAMERA_FOCUS_PET_BUFF);
    tPetBuffStartData.eStackType = BUFF_STACKTYPE_NEVER;
    tPetBuffStartData.bImmobilize = true;
    tPetBuffStartData.bFailsIfImmobilizeImmune = false;
    FastAttribSetValueInt(tPetBuffStartData.idFastAttrib, ATTRIBUTE_HIDDEN, TRUE);
    FastAttribSetValueInt(tPetBuffStartData.idFastAttrib, ATTRIBUTE_IMMOBOLIZED, TRUE);
    FastAttribSetValueInt(tPetBuffStartData.idFastAttrib, ATTRIBUTE_INVULNERABLE, TRUE);
    FastAttribSetValueInt(tPetBuffStartData.idFastAttrib, ATTRIBUTE_CANT_START_DISPLAYED_POWERS, TRUE);

    POS pos = listDestPlayers.GetHeadPosition();
    POS posOld;
    while (pos)
    {
        posOld = pos;
        Player * ptPlayer = listDestPlayers.GetNext(pos);
        if (!ptACD->IsKnownByPlayer(*ptPlayer))
        {
            listDestPlayers.RemoveAt(posOld);
        }
        else
        {
            ActorCommonData & tACDPlayer = ACDGet(ptPlayer->idPlayerACD);

            ActorMovement * ptActorMovement = tACDPlayer.GetActorMovement();

            if (!(ptActorMovement->m_uMoveFlags & (1 << MOVEBIT_FROM_POWER)) && !ptActorMovement->IsInArcingMovement())
            {
                ptActorMovement->StopNow();
            }


            ConversationMgr::PlayerOptOutOfAllConversations(*ptPlayer, NULL);

            PowerAPI_BuffAddToACD(tBuffStartData, tACDPlayer);

            int32 nPos;
            ACDID idACDPet = PlayerGetFirstPet(tACDPlayer.id, PETTYPE_NONE, nPos);
            while (idACDPet != ACDID_INVALID)
            {
                ActorCommonData * ptACDPet = ACDTryToGet(idACDPet);

                if (ptACDPet && ptACDPet->IsAlive())
                {
                    PowerAPI_BuffAddToACD(tPetBuffStartData, *ptACDPet);

                    ActorID idActorPet = (ActorID)ACDGetOwner(idACDPet);
                    if (ActorTryToGetAI(idActorPet))
                    {
                        BrainAPIDoIdle(idActorPet);

                        ActorAISetStatusFlag(idActorPet, ACTOR_AI_DISABLE_UPDATES_BIT, TRUE);
                    }
                }

                idACDPet = PlayerGetNextPet(tACDPlayer.id, PETTYPE_NONE, nPos);
            }
        }
    }

    BoolDataMessage tMessage;
    tMessage.bVal = bDisableAutoCancellation;
    MessageSendToClient(listDestPlayers, SMSG_CAMERA_SCRIPTED_SEQUENCE_START, &tMessage, sizeof(tMessage));
}

extern void FlippiesCreateOnActor(const ActorCommonData & tACDSpawningActor, const ACDID idACDAssigned, const LootSpecifierArray & arLootSpecifiersToSpawn, const uint32 dwFlippyFlags, uint32 dwGameEventNumber, float flCameraBiasDist, const ACDID idACDKiller, const LooteeInfo * ptLootee, const Vector3D * pvForcedPositionBias, const LootContext eLootContext, XPooledDataIDList * plistSpawnedItemACDs);  // DWARF: Flippies.cpp:958; no header declaration recorded.

#line 13974
BOOL GameEvaluateTieredLootRunBossFirstKill(
    VACDID idACDBoss,
    LPCSTR szTreasureClass,
    VACDID idACDPlayer)
{

    SNO snoTreasureClass = SNOFileLookupGUID(TREASURECLASS_SNO, szTreasureClass);
    if (snoTreasureClass == SNO_NONE)
        return FALSE;
#line 13988
    Player * ptPlayer = PlayerGetByACD(idACDPlayer);
    if (!ptPlayer)
        return FALSE;
#line 13996
    if (ptPlayer->tAccount.uPartitionFlags & (1 << PARTITION_FLAG_TIERED_LOOT_RUN_BOSS_KILLED))
        return FALSE;
#line 14002
    LuaVerifySnosAreReferenced(snoTreasureClass, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE);


    ActorCommonData & tACDLootee = ACDGet(idACDBoss);

    LooteeInfo tLootee;
    tLootee.InitializeFromACD(tACDLootee, ACDID_INVALID);
    tLootee.snoTreasureClass = snoTreasureClass;


    LootSpecifierArray arLootSpecifiers;
    if (!LootGetLootSpecifiers(tLootee, ptPlayer->idPlayerACD, arLootSpecifiers, NULL, FALSE, 1, NULL))
        return FALSE;
#line 14019
    LootModifyBeforeSpawning(arLootSpecifiers, tACDLootee, ACDID_INVALID);
    FlippiesCreateOnActor(tACDLootee, ptPlayer->idPlayerACD, arLootSpecifiers, 0, SGameGetEventNumber(), 0.0f, ACDID_INVALID, NULL, NULL, LC_UNKNOWN, NULL);


    ptPlayer->tAccount.uPartitionFlags |= (1 << PARTITION_FLAG_TIERED_LOOT_RUN_BOSS_KILLED);


    ptPlayer->uConsoleSyncFlags |= (1 << PCSF_PARTITION_FLAGS);

    return TRUE;

}

#include "../AI/ProximityDetectorBrain.h"
#include "../Powers/PowersSwig.h"

#line 751
ActorIDArray * ActorsInTriggerBounds(
    ACDID idTrigger,
    BOOL bPowerTargetsOnly)
{
    ActorIDArray & tArrACDs = LuaScriptGetThreadSafeActorIDArray();
    tArrACDs.nNumActorIDs = 0;
    tArrACDs.bWantAddRef = TRUE;

    ActorCommonData * ptACDTrigger = ACDTryToGet(idTrigger);
    if (ptACDTrigger)
    {
        ActorID idActorTrigger = (ActorID)ptACDTrigger->idOwner;
        ActorAI * ptAI = ActorTryToGetAI(idActorTrigger);
        if (ptAI && ProximityDetectorTryGetData(ptAI->idProximityData))
        {
            XPooledDataIDList tListACDs(MemoryPoolsGetDATAIDListPool());
            ProximityDetectorGetActorsInBoundsList(idActorTrigger, tListACDs, FALSE);
            POS pos = tListACDs.GetHeadPosition();
            while (pos)
            {
                ACDID idACDInTrigger = (ACDID)tListACDs.GetNext(pos);

                if (bPowerTargetsOnly && !PowerCanTarget(idACDInTrigger))
                {
                    continue;
                }

                tArrACDs.arActorIDs[tArrACDs.nNumActorIDs] = idACDInTrigger;
                ++tArrACDs.nNumActorIDs;

                if (tArrACDs.nNumActorIDs == ACTOR_ID_ARRAY_MAX_ACTORS)
                {
                    const Vector3D & wp = ptACDTrigger->GetPlace().wp;

                    break;
                }
            }
        }
    }

    return &tArrACDs;
}

#include "../../Common/Actors/ActorCollision.h"

#line 444
ActorIDArray * ActorsNearActor(
    VACDID idACD,
    float flRadius,
    LPCSTR szActorSNOFilter0,
    LPCSTR szActorSNOFilter1,
    LPCSTR szActorSNOFilter2,
    LPCSTR szActorSNOFilter3)
{
    ActorIDArray & tArrACDs = LuaScriptGetThreadSafeActorIDArray();
    tArrACDs.nNumActorIDs = 0;
    tArrACDs.bWantAddRef = TRUE;

    ActorCommonData & tACD = ACDGet(idACD);
    if (!tACD.IsInWorld())
    {
        return &tArrACDs;
    }

    XPooledDataIDList listACDsInRadius(MemoryPoolsGetDATAIDListPool());
    GetACDsInRadius(tACD, flRadius, listACDsInRadius);

    BOOL bFilter = TRUE;

    SNO snoActor0 = SNOFileLookupGUID(ACTOR_SNO, szActorSNOFilter0);
    SNO snoActor1 = SNOFileLookupGUID(ACTOR_SNO, szActorSNOFilter1);
    SNO snoActor2 = SNOFileLookupGUID(ACTOR_SNO, szActorSNOFilter2);
    SNO snoActor3 = SNOFileLookupGUID(ACTOR_SNO, szActorSNOFilter3);

    if (snoActor0 == SNO_NONE && snoActor1 == SNO_NONE && snoActor2 == SNO_NONE && snoActor3 == SNO_NONE)
    {
        bFilter = FALSE;
    }
#line 480
    POS pos = listACDsInRadius.GetHeadPosition();
    while (pos)
    {
        ACDID idACDPos = (ACDID)listACDsInRadius.GetNext(pos);

        if (bFilter)
        {
            ActorCommonData * ptACDTarget = ACDTryToGet(idACDPos);

            if (ptACDTarget->snoActor != snoActor0 && ptACDTarget->snoActor != snoActor1 && ptACDTarget->snoActor != snoActor2 && ptACDTarget->snoActor != snoActor3)
            {
                continue;
            }
        }
#line 498
        tArrACDs.arActorIDs[tArrACDs.nNumActorIDs] = idACDPos;

        ++tArrACDs.nNumActorIDs;

        if (tArrACDs.nNumActorIDs == ACTOR_ID_ARRAY_MAX_ACTORS)
        {
            Vector3D wp = tACD.GetPlace().wp;

            break;
        }
    }
#line 513
    return &tArrACDs;
}

#include "../../Common/CombatLog.h"
#include <float.h>

#line 1837
void ActorHeal(
    VACDID idACD,
    float flValue,
    HealType eHealType,
    SNO snoSource,
    ACDID idACDSource,
    BOOL bPlayEffect)
{
    if (idACD == ACDID_INVALID || flValue < FLT_EPSILON)
        return;
#line 1853
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (!ptACD)
        return;
#line 1862
    if (!ptACD->IsAlive())
        return;
#line 1867
    if (ptACD->IsNPC() && !ptACD->IsPet() && ptACD->AttributesGetInt(ATTRIBUTE_INVULNERABLE))
        return;
#line 1873
    if (flValue > 0.0f && ptACD->AttributesGetInt(ATTRIBUTE_HEALING_SUPPRESSED))
        return;
#line 1878
    if (eHealType == HEAL_TYPE_LIFE_STEAL)
    {

        ActorCommonData * ptACDController = ACDTryToGet(ACDGetControllingActor(idACD));
        if (!ptACDController || !ptACDController->IsDueling())
        {
            int32 nLevel = (ptACDController ? ptACDController : ptACD)->AttributesGetInt(ATTRIBUTE_LEVEL);
            if (nLevel >= 1 && nLevel <= HERO_LEVEL_MAX)
            {
                ScopedGameBalanceDefinitionLockR ptExpTableDef(GlobalSNOGet(GLOBALSNO_GB_EXPERIENCE));
                const ExperienceTable & tExpTable = ptExpTableDef->tExperienceTable;


                float flLifeStealMultiplier = tExpTable.ptExperienceLevels[nLevel - 1].flLifeStealMultiplier;
                flValue *= flLifeStealMultiplier;
            }
        }
    }

    if (bPlayEffect)
    {
        const uint32 MINIMUM_EFFECT_INTERVAL = 60;
        uint32 dwLastEffectTick = ptACD->AttributesGetInt(ATTRIBUTE_HEAL_EFFECT_LAST_PLAYED_TICK);
        uint32 dwNow = GameGetServerTime();
        if (dwNow - dwLastEffectTick > MINIMUM_EFFECT_INTERVAL)
        {
            ACDPlayEffect(idACD, EFFECT_PLAY_EFFECT_GROUP, GlobalSNOGet(GLOBALSNO_EFFECTGROUP_ACTOR_GAINED_HEALTH), idACD);
            ptACD->AttributesSetInt(ATTRIBUTE_HEAL_EFFECT_LAST_PLAYED_TICK, dwNow);
        }
    }

    flValue *= 1.0f + ptACD->AttributesGetFloat(ATTRIBUTE_BONUS_HEALING_RECEIVED_PERCENT) - ptACD->AttributesGetFloat(ATTRIBUTE_REDUCED_HEALING_RECEIVED_PERCENT);

    CombatLog::LogEvent(CombatLog::LOG_EVENT_HEALING_DONE, idACDSource, idACD, snoSource, flValue, 0.0f, eHealType);

    float flOldHP = ptACD->GetHitPointsCurrent();
    float flNewHP = flOldHP + flValue;

    flNewHP = Math::Min(flNewHP, ptACD->GetHitPointsMaxTotal());

    Player * ptPlayer = ACDGetControllingPlayer(idACD);
    if (ptPlayer)
        if (flNewHP - flOldHP > 1.0f)
        {
#line 1927
            SPlayerQueueFloatingNumberHealing(*ptACD, flValue);
        }
#line 1953
    ptACD->SetHitPoints(flNewHP, TRUE, TRUE);
}

#line 13235
void GameUpdateRiftStats(
    int32 nRiftLevel,
    float flTimeElapsed,
    BOOL bRiftSuccess)
{


    int32 nClassCount[NUM_PLAYER_CLASSES];
    for (int32 i = 0; i < NUM_PLAYER_CLASSES; ++i)
    {
        nClassCount[i] = 0;
    }

    int32 nPlayerCount = 0;
    for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {
        if (!NephalemRiftManager::PlayerPaidForRift(ptPlayer))
            continue;
#line 13256
        PlayerClass ePlayerClass = PlayerGetPlayerClass(ptPlayer);
        if (ePlayerClass == PLAYER_CLASS_NONE)
            continue;
#line 13263
        ++nClassCount[ePlayerClass];
    }

    SGameGlobals & tSGameGlobals = SGameGlobalsGet();
    tSGameGlobals.tChallengeRift.nCompletionMilliseconds = (int32)(flTimeElapsed * 1000.0f);
    DungeonFinderData & tDungeonFinderData = tSGameGlobals.tDungeonFinderData;


    int32 nDeepestRiftFloor = INT_MIN;
    for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {
        if (!NephalemRiftManager::PlayerPaidForRift(ptPlayer))
            continue;
#line 13279
        ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
        if (ptACDPlayer)
        {
            SWorldID idSWorld = ptACDPlayer->GetSWorldID();
            SNO snoWorld = WorldCommonGetWorldSNO(idSWorld);
            if (snoWorld != SNO_NONE)
            {
                int32 nFloor = WorldGetDungeonFinderLevel(snoWorld);
            }
        }
    }
#line 13565
}

#include "../../Common/Definitions/MonsterDefinition.h"
#include "../../Common/TagMaps/MonsterTags.h"
#include "../../Common/TagMaps/Tags.h"

#line 9301
blz::vector<CRefString> ActorFindMonsterSNOsInSameLevelArea(
    VACDID idACD)
{
    blz::vector<CRefString> listMonsterSNONames;
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD)
    {

        SNOArray listActors;
        blz::vector<int> listAllowedMonsterRarities;
        listAllowedMonsterRarities.push_back(MR_NORMAL);
        ActorFindMonsterForPlayersLevelArea(*ptACD, listActors, listAllowedMonsterRarities);

        for (int32 i = 0; i < listActors.GetArraySize(); ++i)
        {
            SNO snoMonster = ActorGetMonsterSNO(listActors[i]);

            if (snoMonster != SNO_NONE)
            {
                ScopedMonsterDefinitionLockR pdefMonster(snoMonster);


                const CTagMap * pTagMap = pdefMonster->hTagMap;


                SNO snoValue = TagMapGetSNO(pTagMap, TAG_MONSTER_NECRO_REVIVED_MONSTER_SNO);

                if (snoValue != SNO_NONE)
                {
                    const AxeTagInfo * ptInfo = TagMapGetTagInfo(pTagMap, TAG_MONSTER_NECRO_REVIVED_MONSTER_SNO);
                    SNOGroup snoGroup = AxeTagTypeToSNOGroup(ptInfo->eType);

                    listMonsterSNONames.push_back(SNOToString(snoGroup, snoValue, TRUE));
                }
            }
        }
    }
    return listMonsterSNONames;
}

#line 4215
ActorIDArray * ItemGiveTreasureToPlayer(
    VPACDID idACDPlayer,
    LPCSTR szTreasureClass)
{
    ActorIDArray & tArrACDs = LuaScriptGetThreadSafeActorIDArray();
    tArrACDs.nNumActorIDs = 0;
    tArrACDs.bWantAddRef = TRUE;

    SNO snoTreasureClass = SNOFileLookupGUID(TREASURECLASS_SNO, szTreasureClass);
    if (snoTreasureClass != SNO_NONE)
    {

        LuaVerifySnosAreReferenced(snoTreasureClass, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE);

        ActorCommonData & tACDPlayer = ACDGet(idACDPlayer);
        LooteeInfo tLootee;
        tLootee.InitializeFromACD(tACDPlayer, ACDID_INVALID);
        tLootee.snoTreasureClass = snoTreasureClass;

        XPooledDataIDList listLootACDs(MemoryPoolsGetDATAIDListPool());
        LootGiveTreasureToPlayer(tLootee, tACDPlayer, &listLootACDs, LC_UNKNOWN, FALSE, FALSE);

        POS pos = listLootACDs.GetHeadPosition();
        while (pos)
        {
            ACDID idACDItem = (ACDID)listLootACDs.GetNext(pos);

            if (tArrACDs.nNumActorIDs < ACTOR_ID_ARRAY_MAX_ACTORS)
            {
                tArrACDs.arActorIDs[tArrACDs.nNumActorIDs] = idACDItem;
                ++tArrACDs.nNumActorIDs;
            }
        }
    }
    return &tArrACDs;
}

#line 4608
void ItemDropAtActor(
    VACDID idACD,
    LPCSTR szItemName,
    BOOL bDropForActor,
    ACDID idACDDropLocation,
    float flSpawnYawAngle,
    float flSpawnDist)
{
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (!ptACD)
        return;
#line 4623
    GBID gbidItem = GBID_NONE;
    if (szItemName)
    {
        gbidItem = StringToGBID(szItemName);
    }
    if (gbidItem == GBID_NONE)
        return;
#line 4635
    LootSpecifierArray arLootSpecifiersToSpawn;
    LootSpecifier & tSpecifier = arLootSpecifiersToSpawn.AppendElement_Tracked();


    tSpecifier.hGBItem.eType = GB_ITEMS;
    tSpecifier.hGBItem.gbid = gbidItem;
#line 4643
    tSpecifier.snoActorForFlippy = GameBalanceGetActorSNO(tSpecifier.hGBItem);
    if (tSpecifier.snoActorForFlippy == SNO_NONE)
        return;
#line 4650
    Vector3D * pvForcedPositionBias = NULL;
    Vector3D vForcedPositionBias;
    if (flSpawnDist > 0.0f)
    {
        vForcedPositionBias = cgwvXAxis;
        Vector3D::Rotate(cgwvZAxis, flSpawnYawAngle, vForcedPositionBias);
        vForcedPositionBias *= flSpawnDist;
        pvForcedPositionBias = &vForcedPositionBias;
    }
#line 4662
    tSpecifier.eIQL = IQL_NORMAL; tSpecifier.nAffixLevel = 1;
    tSpecifier.fAutoIdentify = TRUE;
    tSpecifier.fAccountBound = bDropForActor;
#line 4666
    ActorCommonData * ptACDDropLocation = ACDTryToGet(idACDDropLocation);

    uint32 dwGameEventNumber = SGameGetEventNumber();
    FlippiesCreateOnActor(ptACDDropLocation ? *ptACDDropLocation : *ptACD, bDropForActor ? idACD : ACDID_INVALID, arLootSpecifiersToSpawn, 0, dwGameEventNumber, 0.0f, ACDID_INVALID, NULL, pvForcedPositionBias, LC_UNKNOWN, NULL);
}

#include "LuaScriptUtil.h"
#include "../../../Contrib/Contrib/lua/src/lua.h"
SNO LuaScriptGetExecutingScriptSNO();  // X/Server/LuaScript/LuaScript.cpp:1957
lua_State * LuaScriptGetExecutingScriptState();  // X/Server/LuaScript/LuaScript.cpp:1971
int32 LuaSNOScriptCall(SNO snoScript, lua_State * L, int32 nArgumentsCnt, int32 nReturnValCnt, LPCSTR szFuncName);  // X/Server/LuaScript/LuaScriptUtil.cpp:1084
BOOL LuaCheckReturnValue(SNO snoScript, lua_State * L, int32 nExpectedLuaType, LPCSTR szFuncName);  // X/Server/LuaScript/LuaScriptUtil.cpp:1130
void ItemsValidateCachedAffixTableEntries();  // X/Common/Items.cpp:10229

#line 4672

VACDID ItemGiveBestTreasureToPlayer(
    VPACDID idACDPlayer,
    LPCSTR szTreasureClass,
    int nGenerations,
    int32 nItemLevel,
    LPCSTR szFuncGetItemWeight,
    LPCSTR szParam1_VariableArray,
    LPCSTR szParam2_VariableArray)
{
    LuaScriptErrorHandlerEnter();

    // 70: literal; no named level-cap constant found in src or the owner CU DWARF
    nItemLevel = Math::Pin(nItemLevel, 1, 70);

    double flBestScore = -FLT_MAX;
    ACDID idACDBestItem = ACDID_INVALID;

    SNO snoScript = LuaScriptGetExecutingScriptSNO();
    lua_State * ptLuaState = LuaScriptGetExecutingScriptState();

    if (!ptLuaState)
    {

        LuaScriptErrorHandlerExit();
        return ACDID_INVALID;
    }

    ActorCommonData & tACDPlayer = ACDGet(idACDPlayer);
    SNO snoTreasureClass = SNOFileLookupGUID(TREASURECLASS_SNO, szTreasureClass);
    if (snoTreasureClass != SNO_NONE)
    {

        LuaVerifySnosAreReferenced(snoTreasureClass, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE);

        ScopedTreasureClassDefinitionLockR pdefTreasureClass(snoTreasureClass);
        LooteeInfo tLootee;
        tLootee.InitializeFromACD(tACDPlayer, ACDID_INVALID);
        tLootee.tLooteeParams.nLevel = nItemLevel;


        ItemsValidateCachedAffixTableEntries();

        for (int j = 0; j < nGenerations; ++j)
        {
            LootSpecifierArray arLootSpecifiers;
            if (LootGetLootSpecifiers(tLootee, tACDPlayer.id, arLootSpecifiers, pdefTreasureClass, FALSE, 1, NULL))
            {

                int32 nNumItems = arLootSpecifiers.GetArraySize();
                for (int32 i = 0; i < nNumItems; ++i)
                {
                    arLootSpecifiers[i].fAutoIdentify = TRUE;


                    ACDID idACDItem = LootCreateItem(arLootSpecifiers[i], cgplaceNull, NULL);
                    if (idACDItem == ACDID_INVALID)
                        continue;



                    if (LuaScriptGetFunction(ptLuaState, szFuncGetItemWeight))
                    {
                        lua_pushinteger(ptLuaState, tACDPlayer.id);
                        lua_pushinteger(ptLuaState, idACDItem);
                        lua_pushstring(ptLuaState, szParam1_VariableArray);
                        lua_pushstring(ptLuaState, szParam2_VariableArray);

                        int32 nError = LuaSNOScriptCall(snoScript, ptLuaState, 4, 1, szFuncGetItemWeight);

                        if (nError == 0)
                        {
                            if (LuaCheckReturnValue(snoScript, ptLuaState, LUA_TNUMBER, szFuncGetItemWeight))
                            {

                                double flScore = lua_tonumber(ptLuaState, -1);
                                if (flScore > flBestScore)
                                {
                                    flBestScore = flScore;
                                    ACDID idACDPrevBest = idACDBestItem;  // invented name: swap temp; its block emits no code so DWARF keeps no DIE for it
                                    idACDBestItem = idACDItem;
                                    idACDItem = idACDPrevBest;  // inference: line 4770 tests the previous best on this path
                                }
                            }


                            lua_pop(ptLuaState, 1);
                        }
                    }
                    else
                    {
                        ActorDelete(idACDItem);

                        LuaScriptErrorHandlerExit();
                        return ACDID_INVALID;
                    }


                    if (idACDItem != ACDID_INVALID)
                    {
                        ActorDelete((ActorID)ACDGetOwner(idACDItem));
                    }
                }
            }




        }
    }





    ActorCommonData * ptACDItem = ACDTryToGet(idACDBestItem);
    if (ptACDItem)
    {
        if (!SACDInventoryPickupOrSpillOntoGround(tACDPlayer, *ptACDItem, FALSE, NULL, TRUE))
            idACDBestItem = ACDID_INVALID;
    }





    LuaScriptErrorHandlerExit();
    return idACDBestItem;
}

#include "../../Common/GameTemplate.h"

#line 4526
ActorIDArray * TreasureClassDropAtActorForPlayer(
    VACDID idACD,
    LPCSTR szTreasureClass,
    VACDID idACDPlayer,
    float flSpawnYawAngle,
    float flSpawnDist)
{
    ActorIDArray & tArrACDs = LuaScriptGetThreadSafeActorIDArray();
    tArrACDs.nNumActorIDs = 0;
#line 4537
    if (GameTypeIsPvP())
        return &tArrACDs;
#line 4543
    SNO snoTreasureClass = SNOFileLookupGUID(TREASURECLASS_SNO, szTreasureClass);
    if (snoTreasureClass == SNO_NONE)
        return &tArrACDs;
#line 4551
    ActorCommonData & tACDLootee = ACDGet(idACD);
    Player * ptPlayer = PlayerGetByACD(idACDPlayer);
    if (!ptPlayer)
        return &tArrACDs;
#line 4560
    LuaVerifySnosAreReferenced(snoTreasureClass, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE);
#line 4563
    LooteeInfo tLootee;
    tLootee.InitializeFromACD(tACDLootee, ACDID_INVALID);
    tLootee.snoTreasureClass = snoTreasureClass;
#line 4568
    LootSpecifierArray arLootSpecifiers;
    if (!LootGetLootSpecifiers(tLootee, ptPlayer->idPlayerACD, arLootSpecifiers, NULL, FALSE, 1, NULL))
        return &tArrACDs;
#line 4574
    Vector3D * pvForcedPositionBias = NULL;
    Vector3D vForcedPositionBias;
    if (flSpawnDist > 0.0f)
    {
        vForcedPositionBias = cgwvXAxis;
        Vector3D::Rotate(cgwvZAxis, flSpawnYawAngle, vForcedPositionBias);
        vForcedPositionBias *= flSpawnDist;
        pvForcedPositionBias = &vForcedPositionBias;
    }
#line 4585
    LootModifyBeforeSpawning(arLootSpecifiers, tACDLootee, ACDID_INVALID);
    XPooledDataIDList listSpawnedItemACDs(MemoryPoolsGetDATAIDListPool());
    FlippiesCreateOnActor(tACDLootee, ptPlayer->idPlayerACD, arLootSpecifiers, 0, SGameGetEventNumber(), 0.0f, ACDID_INVALID, &tLootee, pvForcedPositionBias, LC_UNKNOWN, &listSpawnedItemACDs);
#line 4590
    tArrACDs.bWantAddRef = FALSE;
    POS pos = listSpawnedItemACDs.GetHeadPosition();
    while (pos)
    {
        ACDID idACDLoc = (ACDID)listSpawnedItemACDs.GetNext(pos);
        tArrACDs.arActorIDs[tArrACDs.nNumActorIDs] = idACDLoc;
        ++tArrACDs.nNumActorIDs;
        if (tArrACDs.nNumActorIDs == ACTOR_ID_ARRAY_MAX_ACTORS)
            break;
    }
#line 4604
    return &tArrACDs;
}

int32 ExperienceGetPlayersEligibleForKill(ACDID idACDVictim, PlayerList & listPlayersToEarnXP);  // Experience.cpp:882; no header declaration in DWARF.

#line 4420
ActorIDArray * TreasureClassDropAtActor(
    VACDID idACD,
    LPCSTR szTreasureClass,
    BOOL bEnforceLootRules,
    float flSpawnYawAngle,
    float flSpawnDist)
{
    ActorIDArray & tArrACDs = LuaScriptGetThreadSafeActorIDArray();
    tArrACDs.nNumActorIDs = 0;
#line 4431
    if (GameTypeIsPvP())
        return &tArrACDs;
#line 4436
    ActorCommonData & tACDLootee = ACDGet(idACD);

    XPooledDataIDList listSpawnedItemACDs(MemoryPoolsGetDATAIDListPool());
    SNO snoTreasureClass = SNOFileLookupGUID(TREASURECLASS_SNO, szTreasureClass);
    if (snoTreasureClass != SNO_NONE)
    {
#line 4443
        LuaVerifySnosAreReferenced(snoTreasureClass, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE);

        LooteeInfo tLootee;
        tLootee.InitializeFromACD(tACDLootee, ACDID_INVALID);
        tLootee.snoTreasureClass = snoTreasureClass;

        SharedLootManager tSharedLoot;
        tSharedLoot.ptACDLootee = &tACDLootee;
        tSharedLoot.dwGameEventNumber = SGameGetEventNumber();

        Vector3D * pvForcedPositionBias = NULL;
        Vector3D vForcedPositionBias;
        if (flSpawnDist > 0.0f)
        {
            vForcedPositionBias = cgwvXAxis;
            Vector3D::Rotate(cgwvZAxis, flSpawnYawAngle, vForcedPositionBias);
            vForcedPositionBias *= flSpawnDist;
            pvForcedPositionBias = &vForcedPositionBias;
        }
#line 4463
        if (!bEnforceLootRules)
        {
            for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
            {
                LootSpecifierArray arLootSpecifiersToSpawn;
                BOOL fResult = LootGetLootSpecifiers(tLootee, ptPlayer->idPlayerACD, arLootSpecifiersToSpawn, NULL, FALSE, 1, &tSharedLoot);
                if (!fResult)
                    continue;
#line 4474
                LootModifyBeforeSpawning(arLootSpecifiersToSpawn, tACDLootee, ACDID_INVALID);

                FlippiesCreateOnActor(tACDLootee, ptPlayer->idPlayerACD, arLootSpecifiersToSpawn, 0, SGameGetEventNumber(), 0.0f, ACDID_INVALID, &tLootee, pvForcedPositionBias, LC_UNKNOWN, &listSpawnedItemACDs);
            }
        }
        else
        {
            PlayerList listPlayers(PlayerListGetElementPool());
            ExperienceGetPlayersEligibleForKill(idACD, listPlayers);

            POS pos = listPlayers.GetHeadPosition();
            while (pos)
            {
                Player * ptPlayer = listPlayers.GetNext(pos);

                LootSpecifierArray arLootSpecifiersToSpawn;
                BOOL fResult = LootGetLootSpecifiers(tLootee, ptPlayer->idPlayerACD, arLootSpecifiersToSpawn, NULL, FALSE, 1, &tSharedLoot);
                if (!fResult)
                    continue;
#line 4496
                LootModifyBeforeSpawning(arLootSpecifiersToSpawn, tACDLootee, ACDID_INVALID);

                FlippiesCreateOnActor(tACDLootee, ptPlayer->idPlayerACD, arLootSpecifiersToSpawn, 0, SGameGetEventNumber(), 0.0f, ACDID_INVALID, &tLootee, pvForcedPositionBias, LC_UNKNOWN, &listSpawnedItemACDs);
            }
        }
    }
#line 4508
    tArrACDs.bWantAddRef = FALSE;
    POS pos = listSpawnedItemACDs.GetHeadPosition();
    while (pos)
    {
        ACDID idACDLocal = (ACDID)listSpawnedItemACDs.GetNext(pos);
        tArrACDs.arActorIDs[tArrACDs.nNumActorIDs] = idACDLocal;
        ++tArrACDs.nNumActorIDs;
        if (tArrACDs.nNumActorIDs == ACTOR_ID_ARRAY_MAX_ACTORS)
            break;
    }
#line 4522
    return &tArrACDs;
}

#line 253
ACDID ActorIDFromName(
    LPCSTR szInstanceName,
    VACDID idACDNeighbor)
{
    ACDID idACD = ACDID_INVALID;
    ActorIterator tIt;
    float flBestDist = FLT_MAX;

    ActorCommonData * ptACD = ACDTryToGet(idACDNeighbor);
    if (!ptACD || !ptACD->IsInWorld())
    {
        uint32 tName;
        if (!ptACD)
        {
#line 273
            tName = MarkerStringHash("Unknown");
        }
        return ACDID_INVALID;
    }
#line 280
    uint32 tInstanceName = MarkerStringHash(szInstanceName);

    const WorldLocation & tWorldLocNeighbor = ACDGet(idACDNeighbor).GetWorldLocation();
    const Vector3D & wpNeighbor = tWorldLocNeighbor.transform.wp;

    while (ActorDataArrayIterate(tIt))
    {
        const Actor * ptActor = tIt.ptElement;

        ActorCommonData & tACD = ACDGet(ptActor->idACD);
        if (!tACD.IsInWorld())
        {
            continue;
        }
#line 295
        const WorldLocation & tWorldLoc = tACD.GetWorldLocation();

        if (tWorldLoc.idSWorld == tWorldLocNeighbor.idSWorld &&
            ptActor->tName == tInstanceName)
        {
            const Vector3D & wpActor = tWorldLoc.transform.wp;

            float dist = Vector3D::DistanceSquared(wpNeighbor, wpActor);

            if (dist < flBestDist)
            {
                idACD = ptActor->idACD;
                flBestDist = dist;
            }
        }
    }
#line 312
    if (idACD != ACDID_INVALID)
    {
        LuaScriptInstanceAddRefACD(LuaScriptGetExecutingScriptInstance(), idACD);
    }
#line 322
    return idACD;
}

#include "../../Common/Nav/NavigationMesh.h"
#include "../../Common/Nav/NavZone.h"
#include "../AI/AIStates.h"

#line 9214
ACDID ActorFindNearestActor(
    VACDID idACD,
    float flRadius,
    BOOL bIgnoreActorsWithAIMergesByOthers,
    BOOL bIgnoreLOS,
    LPCSTR szActorSNOFilter0,
    LPCSTR szActorSNOFilter1,
    LPCSTR szActorSNOFilter2,
    LPCSTR szActorSNOFilter3)
{
    SNO snoActor0 = SNOFileLookupGUID(ACTOR_SNO, szActorSNOFilter0);
    SNO snoActor1 = SNOFileLookupGUID(ACTOR_SNO, szActorSNOFilter1);
    SNO snoActor2 = SNOFileLookupGUID(ACTOR_SNO, szActorSNOFilter2);
    SNO snoActor3 = SNOFileLookupGUID(ACTOR_SNO, szActorSNOFilter3);


    LuaVerifySnosAreReferenced(snoActor0, snoActor1, snoActor2, snoActor3, SNO_NONE, SNO_NONE, SNO_NONE);
#line 9236
    ActorCommonData & tACD = ACDGet(idACD);
    WorldPlace placeCenter = tACD.GetPlace();
    XPooledDataIDList listTargets(MemoryPoolsGetDATAIDListPool());
    ACDsCylinderIntersectSphere(placeCenter, flRadius, idACD, listTargets, ACTOR_SPHERE_TREE_FLAGS_ALL);

    ACDID idACDWinner = ACDID_INVALID;
    float flWinningDistSqr = FLT_MAX;
    NavCellHandle hNavCellStart = { SSCENEID_INVALID };
    BOOL fNavCellInitialized = FALSE;
    POS pos = listTargets.GetHeadPosition();
    while (pos)
    {
        ACDID idACDTarget = (ACDID)listTargets.GetNext(pos);
        ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
        if (ptACDTarget)
        {
            if (ptACDTarget->snoActor == snoActor0 || ptACDTarget->snoActor == snoActor1 || ptACDTarget->snoActor == snoActor2 || ptACDTarget->snoActor == snoActor3)
            {
#line 9257
                ActorID idActorTarget = (ActorID)ACDGetOwner(idACDTarget);
                if (bIgnoreActorsWithAIMergesByOthers &&
                    AIStateHasExistingMerge(idActorTarget, idACD))
                {
                    continue;
                }
#line 9264
                if (!bIgnoreLOS)
                {
                    if (!fNavCellInitialized)
                    {
                        NavCellHandleFromWorldPlace(placeCenter, hNavCellStart);
                        fNavCellInitialized = TRUE;
                    }
                    BOOL bLOS = NavMeshCheckLOSToACD(hNavCellStart, placeCenter, idACDTarget, PASSABILITY_MONSTER, FALSE);
                    if (!bLOS)
                    {
                        continue;
                    }
                }
#line 9278
                WorldPlace placeTarget = ptACDTarget->GetPlace();
                float flDistSqr = Vector2D::DistanceSquared((const Vector2D &)placeCenter.wp, (const Vector2D &)placeTarget.wp);
                if (flDistSqr < flWinningDistSqr)
                {
                    flWinningDistSqr = flDistSqr;
                    idACDWinner = idACDTarget;
                }
            }
        }
    }
#line 9289
    SGameGlobalsGet();
#line 9297
    return idACDWinner;
}

#include "../../Common/TagMaps/AnimTags.h"
#line 8621
ACDID sActorSpawn(
    SNO snoHandle,
    SNOGroup snoGroup,
    GameBalanceType eType,
    ACDID idACDLocation,
    LPCSTR szSpawnAnim,
    BOOL bRandomOrientation,
    int nSkipDistMin,
    int nSkipDistDelta,
    BOOL bUseContactOffset,
    MonsterRarity eMonsterRarity)
{
    ActorCommonData & tACDLoc = ACDGet(idACDLocation);

    ActorAddStruct tActorAdd;

    tActorAdd.tSNOName.snoGroup = snoGroup;
    tActorAdd.tSNOName.snoHandle = snoHandle;
    if (snoGroup == ACTOR_SNO)
    {
#line 8641
        tActorAdd.snoActor = snoHandle;
        tActorAdd.hGB.gbid = snoHandle;
        tActorAdd.eMonsterRarity = eMonsterRarity;
        tActorAdd.bIsSummon = TRUE;
    }
#line 8651
    tActorAdd.tPlace.idSWorld = tACDLoc.tLocation.tWorldLocation.idSWorld;
    tActorAdd.hGB.eType = eType;

    if (bUseContactOffset)
    {
        tACDLoc.GetContactOffset(tActorAdd.tPlace.wp, 0);
    }
    else
    {
        tActorAdd.tPlace.wp = tACDLoc.tLocation.tWorldLocation.transform.wp;
    }

    if (bRandomOrientation)
    {
        tActorAdd.qOrientation.FromYaw(RandAngle(SGameGlobalsGet().rng));
    }
    else
    {
        tActorAdd.qOrientation = tACDLoc.tLocation.tWorldLocation.transform.q;
    }

    if (szSpawnAnim && *szSpawnAnim)
    {
        tActorAdd.dwSpawnAnimTag = AnimTagGetID(szSpawnAnim);
    }

    if (!bRandomOrientation && nSkipDistMin == 0 && nSkipDistDelta == 0)
    {
        tActorAdd.dwActorAddFlags |= 1 << ACTORADD_DONT_PERTURB;
        tActorAdd.dwActorAddFlags |= 1 << ACTORADD_DONT_SEARCH_FOR_LOCATION;
    }

    ScenesCommonNavMeshNodeFromPlace(tActorAdd.tPlace, &tActorAdd.tNavMeshHandle, FALSE, FALSE);

    ActorPushSpawnSkipDistance(nSkipDistMin, nSkipDistDelta);

    ACDID idACDSpawned = ACDID_INVALID;
    ActorID idActor = GBAdd(tActorAdd);


    if (idActor != ACTORID_INVALID)
    {
        idACDSpawned = ACDFindByActor(idActor);
    }

    ActorPopSpawnSkipDistance();

    return idACDSpawned;
}

// Raw DWARF records only GameBalanceDefinition.cpp:2319/2325; no recovered header declarations.
const GameBalanceDefinition *ItemTiersTableLockR();
void ItemTiersTableUnlockR(const GameBalanceDefinition *lpDef);

#line 4132
void ItemGiveTierToPlayer(
    VPACDID idACDPlayer,
    int32 nTier)
{
    const GameBalanceDefinition *pdefItemTiers = ItemTiersTableLockR();
    const ItemTierTable &tItemTierTable = pdefItemTiers->tItemTierTable;
    int32 nItems = tItemTierTable.serTiers.dwSize / sizeof(ItemTierData);

    if (nTier >= 1 && nTier <= nItems)
    {
        const ItemTierData &tData = tItemTierTable.ptTiers[nTier - 1];

        sCreateItemInInventory(idACDPlayer, GBIDToString(tData.gbidHead, GB_ITEMS, FALSE).str(), FALSE, NULL, NULL, NULL, NULL, NULL, NULL, FALSE, FALSE);
        sCreateItemInInventory(idACDPlayer, GBIDToString(tData.gbidTorso, GB_ITEMS, FALSE).str(), FALSE, NULL, NULL, NULL, NULL, NULL, NULL, FALSE, FALSE);
        sCreateItemInInventory(idACDPlayer, GBIDToString(tData.gbidFeet, GB_ITEMS, FALSE).str(), FALSE, NULL, NULL, NULL, NULL, NULL, NULL, FALSE, FALSE);
        sCreateItemInInventory(idACDPlayer, GBIDToString(tData.gbidHands, GB_ITEMS, FALSE).str(), FALSE, NULL, NULL, NULL, NULL, NULL, NULL, FALSE, FALSE);
        sCreateItemInInventory(idACDPlayer, GBIDToString(tData.gbidShoulders, GB_ITEMS, FALSE).str(), FALSE, NULL, NULL, NULL, NULL, NULL, NULL, FALSE, FALSE);
        sCreateItemInInventory(idACDPlayer, GBIDToString(tData.gbidLegs, GB_ITEMS, FALSE).str(), FALSE, NULL, NULL, NULL, NULL, NULL, NULL, FALSE, FALSE);
        sCreateItemInInventory(idACDPlayer, GBIDToString(tData.gbidBracers, GB_ITEMS, FALSE).str(), FALSE, NULL, NULL, NULL, NULL, NULL, NULL, FALSE, FALSE);
        sCreateItemInInventory(idACDPlayer, GBIDToString(tData.gbidBelt, GB_ITEMS, FALSE).str(), FALSE, NULL, NULL, NULL, NULL, NULL, NULL, FALSE, FALSE);
    }

    ItemTiersTableUnlockR(pdefItemTiers);
}

// Raw DWARF records only SPlayer.cpp:10849; no recovered header declaration for this reference overload.
BOOL SPlayerInScriptedSequence(const Player &tPlayer);

#line 2341
BOOL ActorPlayRandomBanterFromList(
    LPCSTR szConversationList,
    ACDID idACDPlayer,
    ACDID idACDPrimary,
    int32 nPriority,
    float flCooldown,
    float flGlobalCooldown)
{
    SNO snoConvList = SNOFileLookupGUID(CONVERSATIONLIST_SNO, szConversationList);
    if (snoConvList == SNO_NONE)
    {
        return FALSE;
    }


    LuaVerifySnosAreReferenced(snoConvList, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE);

    Player *ptPlayer = PlayerGetByACD(idACDPlayer);
    if (!ptPlayer)
    {
        return FALSE;
    }


    uint32 dwNow = GameGetServerTime();
    ActorCommonData &tACDPlayer = ACDGet(idACDPlayer);
    int32 nGlobalCoolDown = tACDPlayer.AttributesGetInt(MakeAttribKey(ATTRIBUTE_BANTER_COOLDOWN, -1));
    if (nGlobalCoolDown != -1 && dwNow < nGlobalCoolDown)
    {
        return FALSE;
    }


    if (ACDIsInNoBanterArea(tACDPlayer))
    {
        return FALSE;
    }


    if (tACDPlayer.AttributesGetInt(ATTRIBUTE_LORE_PLAYING))
    {
        return FALSE;
    }


    if (SPlayerInScriptedSequence(*ptPlayer))
    {
        return FALSE;
    }

    ConversationArray tConvArray;
    NPCGetConversationsFromList(snoConvList, idACDPlayer, idACDPrimary, &tConvArray);
    if (tConvArray.size() == 0)
    {
        return FALSE;
    }


    int32 nCount = 0;
    uint32 nFewestTimesHeard = UINT_MAX;
    for (uint32 i = 0; i < tConvArray.size(); ++i)
    {
        SNO snoConv = tConvArray[i];
        uint32 nTimesHeard = tACDPlayer.AttributesGetInt(MakeAttribKey(ATTRIBUTE_CONVERSATION_HEARD_COUNT, snoConv));

        if (nTimesHeard <= nFewestTimesHeard)
        {
            if (nTimesHeard < nFewestTimesHeard)
            {
                nCount = 0;
                nFewestTimesHeard = nTimesHeard;
            }



            tConvArray[nCount] = snoConv;
            ++nCount;
        }
    }


    int32 nIndex = Rand_Internal(SGameGlobalsGet().rng, nCount - 1);
    SNO snoConv = tConvArray[nIndex];
    if (snoConv == SNO_NONE)
    {
        return FALSE;
    }

    BOOL bSuccess = ConversationMgr::StartConversation(
        ptPlayer->nPlayerIndex,
        snoConv,
        idACDPrimary,
        ACDID_INVALID,
        ACDID_INVALID,
        ACDID_INVALID,
        ACDID_INVALID,
        nPriority,
        FALSE, FALSE);

    if (bSuccess)
    {

        tACDPlayer.AttributesSetInt(MakeAttribKey(ATTRIBUTE_CONVERSATION_HEARD_COUNT, snoConv), nFewestTimesHeard + 1);


        AISetBanterCooldown(tACDPlayer, snoConv, flCooldown, flGlobalCooldown);
    }

    return bSuccess;
}

#include "../../Common/Definitions/SceneDefinition.h"
#include "../../Common/Marker/SmartMarker.h"
#include "../../Sigma/BlzExt/BlzVectorHelper.h"
// Cached raw DWARF records only SceneCommon.cpp:729; no header declaration was found.
void SceneCommonDataActivate(const SSceneID idSScene);

#line 6938 "X/Server/LuaScript/ScriptUtil.cpp"
ActorIDArray * GameGetOpenWorldBossSpawnMarkerLocationsNearActor(
    ACDID idACDRef,
    int32 nNumLocations,
    float flMinDist,
    float flMaxDist,
    LPCSTR szLevelArea)
{
    ActorIDArray & tArrACDs = LuaScriptGetThreadSafeActorIDArray();
    tArrACDs.nNumActorIDs = 0;
    tArrACDs.bWantAddRef = TRUE;

    ActorCommonData * ptACDRef = ACDTryToGet(idACDRef);
    if (!ptACDRef)
    {
        return &tArrACDs;
    }
    const WorldPlace & tPlaceOrigin = ptACDRef->GetPlace();

    nNumLocations = Math::Min(nNumLocations, (int32)ACTOR_ID_ARRAY_MAX_ACTORS);
    if (nNumLocations < 1)
    {
        return &tArrACDs;
    }

    SNO snoLevelArea = SNO_NONE;
    if (szLevelArea && szLevelArea[0])
    {
        snoLevelArea = SNOFileLookupGUID(LEVELAREA_SNO, szLevelArea);
    }

    if (snoLevelArea == SNO_NONE)
    {
        snoLevelArea = ACDGetLevelArea(*ptACDRef);
    }

    blz::vector<WorldPlace, blz::allocator<WorldPlace> > arPlaces;

    SceneCommonData * ptSCD = NULL;
    SceneCommonDataIterator tIterator;

    while (SceneCommonDataArrayIterate(tIterator, ptSCD, SkipHolodeckScenes))
    {
        if (ptSCD->idSWorld != tPlaceOrigin.idSWorld)
        {
            continue;
        }

        ScopedSceneDefinitionLockR pdefScene(ptSCD->snoScene);
        for (int32 nMarkerSet = 0; nMarkerSet < pdefScene->arMarkerSets.GetArraySize(); ++nMarkerSet)
        {
            SNO snoMarkerSet = pdefScene->arMarkerSets[nMarkerSet];
            ScopedMarkerSetDefinitionLockR pdefMarkerSet(snoMarkerSet);

            GBID gbidLabel = pdefMarkerSet->nLabel;
            if (gbidLabel != GBID_NONE &&
                !ptSCD->listAppliedLabels.Find(gbidLabel))
            {
                continue;
            }


            if (pdefMarkerSet->nSpecialIndexCount > 0)
            {
                for (int32 j = 0; j < pdefMarkerSet->nSpecialIndexCount; ++j)
                {

                    int32 nIndex = pdefMarkerSet->pwSpecialIndexList[j];


                    SmartMarker sMarker(pdefMarkerSet, nIndex);


                    if (sMarker.IsGizmoLocationMarker() && sMarker.GetMarkerType() == MARKERTYPE_GIZMOLOC_U)
                    {
                        PRTransform transform = ptSCD->transformWorld * sMarker.GetTransform();
                        WorldDistance wdDistanceSquared = Vector3D::Distance2DSquared(tPlaceOrigin.wp, transform.wp);
                        if (wdDistanceSquared < flMinDist * flMinDist)
                        {
                            continue;
                        }

                        if (flMaxDist > EPSILON && wdDistanceSquared > flMaxDist * flMaxDist)
                        {
                            continue;
                        }


                        SNO snoLevelAreaScene = ScenesCommonLevelAreaFromPlace(ptSCD->idSScene, transform.wp, TRUE);
                        if (snoLevelAreaScene == snoLevelArea)
                        {
                            WorldPlace tPlaceGizmo;
                            tPlaceGizmo.idSWorld = tPlaceOrigin.idSWorld;
                            tPlaceGizmo.wp = transform.wp;

                            arPlaces.push_back(tPlaceGizmo);
                        }
                    }
                }
            }
        }
    }



    SNO snoMarkerLocation = GlobalSNOGet(GLOBALSNO_ACTOR_MARKER_LOCATION);

    ActorAddStruct tActorAdd;
    tActorAdd.tSNOName.snoHandle = snoMarkerLocation;
    tActorAdd.tSNOName.snoGroup = ACTOR_SNO;
    tActorAdd.snoActor = snoMarkerLocation;
    tActorAdd.qOrientation = ptACDRef->tLocation.tWorldLocation.transform.q;
    tActorAdd.bIsSummon = TRUE;
    tActorAdd.dwActorAddFlags |= 1 << ACTORADD_DONT_PERTURB;
    tActorAdd.dwActorAddFlags |= 1 << ACTORADD_DONT_SEARCH_FOR_LOCATION;

    uint32 nSize = arPlaces.size();
    while (nSize > 0 && tArrACDs.nNumActorIDs < nNumLocations)
    {

        int32 nIndex = Rand_Internal(SGameGlobalsGet().rng, nSize - 1);
        tActorAdd.tPlace = arPlaces[nIndex];
        ScenesCommonNavMeshNodeFromPlace(tActorAdd.tPlace, &tActorAdd.tNavMeshHandle, TRUE, FALSE);


        SceneCommonDataActivate(tActorAdd.tNavMeshHandle.idSScene);

        ActorID idActor = GBAdd(tActorAdd);

        if (idActor != ACTORID_INVALID)
        {
            ACDID idACDSpawned = ACDFindByActor(idActor);
            tArrACDs.arActorIDs[tArrACDs.nNumActorIDs++] = idACDSpawned;
        }


        blzext::vector_erase_reorder(arPlaces, nIndex);
        --nSize;
    }

    return &tArrACDs;
}

#line 8714 "X/Server/LuaScript/ScriptUtil.cpp"
// NON_MATCHING: 396/400 bytes; failure return shares the earlier tail and epilogue restores are scheduled differently; one faithful spelling.
ACDID ActorSpawnActor(
    LPCSTR szActorName,
    ACDID idACDLocation,
    LPCSTR szSpawnAnim,
    BOOL bRandomOrientation,
    int nSkipDistMin,
    int nSkipDistMax,
    BOOL bUseContactOffset,
    MonsterRarity eMonsterRarity)
{
    if (GameActiveGameCommonDataIsClient())
        return ACDID_INVALID;


    // Inferred clamp spelling: original clears negative minimum, then raises maximum to minimum.
    if (nSkipDistMin < 0)
        nSkipDistMin = 0;



    if (nSkipDistMax < nSkipDistMin)
        nSkipDistMax = nSkipDistMin;



    SNO sno = SNOFileLookupGUID(ACTOR_SNO, szActorName);

    if (sno == SNO_NONE)
        return ACDID_INVALID;





    LuaVerifySnosAreReferenced(sno, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE);

    int32 nSkipDistDelta = nSkipDistMax - nSkipDistMin;
    ACDID idACD = sActorSpawn(sno, ACTOR_SNO, GBA_INVALID, idACDLocation, szSpawnAnim, bRandomOrientation, nSkipDistMin, nSkipDistDelta, bUseContactOffset, eMonsterRarity);

    if (idACD != ACDID_INVALID)
    {
        LuaScriptInstanceAddRefACD(LuaScriptGetExecutingScriptInstance(), idACD);
    }
    else
    {
        ActorCommonData * ptACDLocation = ACDTryToGet(idACDLocation);
        if (ptACDLocation)
        {
            WorldPlace wpLocation = ptACDLocation->GetPlace();
            SWorld * ptSWorld = SWorldTryToGet(wpLocation.idSWorld);
            // DWARF records these diagnostic locals; only GetPlace and TLS side effects survive.












        }
    }


    return idACD;
}

#line 5110 "X/Server/LuaScript/ScriptUtil.cpp"
BOOL WorldHasScene(
    LPCSTR szWorldName,
    LPCSTR szSceneName,
    LPCSTR szLabel)
{
    SNO snoWorld = SNOFileLookupGUID(WORLD_SNO, szWorldName);
    if (snoWorld == SNO_NONE)
        return FALSE;





    SNO snoScene = SNOFileLookupGUID(SCENE_SNO, szSceneName);

    if (snoScene == SNO_NONE)
        return FALSE;



    // DWARF initial values: bCheckLabel is zero and gbidLabel is -1 before lookup.
    BOOL bCheckLabel = FALSE;
    GBID gbidLabel = GBID_NONE;
    if (szLabel && *szLabel)
    {
        gbidLabel = GBIDExists(szLabel, 11); // GB_LABELS, GameBalanceDefinition.h:34.
        if (gbidLabel == GBID_NONE)
            return FALSE;



        // Inferred assignment location: original sets the flag after successful lookup.
        bCheckLabel = TRUE;
    }



    SWorldID idSWorld = WorldCommonFindBySNO(snoWorld, TRUE);
    SWorld * ptSWorld = SWorldTryToGet(idSWorld);
    if (ptSWorld)
    {
        SceneCommonData * ptSCD = NULL;
        SceneCommonDataIterator tIterator;
        while (SceneCommonDataArrayIterate(tIterator, ptSCD, SkipHolodeckScenes))
        {
            if (ptSCD->idSWorld != idSWorld)
            {
                continue;
            }

            if (ptSCD->snoScene != snoScene)
            {
                continue;
            }
            // Inferred early return: original unswitches this flag and returns true without a label scan.
            if (!bCheckLabel)
                return TRUE;
            // Inferred list iteration spelling from the head load and next-node traversal.
            POS posLabelList = ptSCD->listAppliedLabels.GetHeadPosition();
            while (posLabelList)
            {
                GBID gbidThisLabel = ptSCD->listAppliedLabels.GetNext(posLabelList);
                // Inferred comparison location: original returns true on equal label IDs.
                if (gbidThisLabel == gbidLabel)
                    return TRUE;
            }
        }
    }



    // Inferred shared failure return: original merges lookup failures and exhausted scans.
    return FALSE;
}

#line 4025 "X/Server/LuaScript/ScriptUtil.cpp"

ACDID sCreateItemInInventory(
    ACDID idACDPlayer,
    LPCSTR szItemName,
    BOOL bTryStashFirst,
    LPCSTR szAffix1,
    LPCSTR szAffix2,
    LPCSTR szAffix3,
    LPCSTR szAffix4,
    LPCSTR szAffix5,
    LPCSTR szAffix6,
    BOOL bRewardUI,
    BOOL bIgnoreInheritedAffixes)
{

    GBHandle gbhItem;

    gbhItem.Set(GB_ITEMS, StringToGBID(szItemName));

    if (!GBIDExists(gbhItem))
    {
        return ACDID_INVALID;
    }

    // Inferred initial invalid ID: every successful path assigns LootCreateItem before use.
    ACDID idACDItem = ACDID_INVALID;
    ActorCommonData & tACDPlayer = ACDGet(idACDPlayer);

    {
        LootSpecifier tSpecifier;

        tSpecifier.hGBItem = gbhItem;
        tSpecifier.snoActorForFlippy = GameBalanceGetActorSNO(tSpecifier.hGBItem);
        tSpecifier.nGoldAmount = 10;
        tSpecifier.fAutoIdentify = TRUE;
        tSpecifier.nAffixLevel = tACDPlayer.AttributesGetInt(ATTRIBUTE_LEVEL);

        if (!bIgnoreInheritedAffixes)
        {
            LootDetermineInherentAffixes(tSpecifier);
        }
        int32 nInherentAffixes = tSpecifier.nNumAffixes;

        // Inferred aggregate spelling: original stores the six parameter pointers in this order.
        LPCSTR arAffixes[6] = {szAffix1, szAffix2, szAffix3, szAffix4, szAffix5, szAffix6};
        // Inferred zero initialization: original counts the non-null prefix, bounded by six.
        int nAffixCount = 0;


        // Inferred loop spelling: original unrolls six consecutive null-pointer tests.
        while (nAffixCount < 6 && arAffixes[nAffixCount])
        {
            ++nAffixCount;
        }


        for (int i = 0; i < nAffixCount; ++i)
        {
            GBID gbidAffix = StringToGBID(arAffixes[i]);
            if (GBIDExists(gbidAffix, 8)) // GB_AFFIXES, GameBalanceDefinition.h:34.
            {
                tSpecifier.AddAffix(gbidAffix);
            }
        }





        tSpecifier.eIQL = ItemDetermineQualityLevel("Script sCreateItemInInventory", tSpecifier, nInherentAffixes);
        tSpecifier.nLegendaryItemLevel = tSpecifier.nAffixLevel;

        idACDItem = LootCreateItem(tSpecifier, cgplaceNull, NULL);
    }

    if (idACDItem != ACDID_INVALID)
    {
        ActorCommonData & tACDItem = ACDGet(idACDItem);
        Player * ptPlayer = PlayerGetByACD(idACDPlayer);


        if (!CmdLineGetParams()->tBitArray.Test(CMDLINE_BLIZZCON_BIT))
        {
            if (bRewardUI && ptPlayer)
            {

                D3::Quests::QuestReward tReward;
                tReward.Clear();

                ItemGeneratorToDefaults(*tReward.mutable_item());
                ItemGeneratorPopulateFromACDID(tACDItem, *tReward.mutable_item(), FALSE);

                SQuestsSendQuestReward(*ptPlayer, 0, tReward, FALSE);
            }
        }

        SACDInventoryPickupOrSpillOntoGround(tACDPlayer, tACDItem, bTryStashFirst, NULL, TRUE);

        LuaScriptInstanceAddRefACD(LuaScriptGetExecutingScriptInstance(), idACDItem);
    }

    return idACDItem;
}

// DWARF: X/Client/Sound/Sound.cpp:2712; declaration absent from the adopted Sound.h subset.
namespace SoundManager { void PlayMusic(const SNO snoMusic, const int32 nFirstIndexToPlay, const BOOL fDisableShuffle); }
#line 6314 "X/Server/LuaScript/ScriptUtil.cpp"
void MusicPlay(
    LPCSTR szMusic,
    ACDID idActor,
    float flMaxDistance)
{
    SNO snoMusic = SNOFileLookupGUID(MUSIC_SNO, szMusic);


    LuaVerifySnosAreReferenced(snoMusic, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE);

    if (GameActiveGameCommonDataIsServer())
    {
        PlayMusicMessage tMessage;
        tMessage.snoMusic = snoMusic;

        PlayerList listDestPlayers(PlayerListGetElementPool());
        PlayerListGetAllConnected(listDestPlayers, -1, FALSE);

        // Inferred null guard placement: original bypasses filtering when ACD lookup fails.
        ActorCommonData * ptACD = ACDTryToGet(idActor);
        if (ptACD)
        {
            if (ptACD->IsInWorld())
            {
                WorldPlace tPlaceCenter = ptACD->GetPlace();
                float flDistSqr = flMaxDistance * flMaxDistance;

                POS pos = listDestPlayers.GetHeadPosition();
                while (pos)
                {
                    POS posOld = pos;
                    Player * ptPlayer = listDestPlayers.GetNext(pos);
                    ActorCommonData & tACDPlayer = ACDGet(ptPlayer->idPlayerACD);
                    WorldPlace tPlace = tACDPlayer.GetWorldPlaceCenter();
                    // DWARF loclist records false before the world/distance acceptance tests.
                    BOOL fCloseEnough = FALSE;
                    if (tPlace.idSWorld == tPlaceCenter.idSWorld) {
                        // Inferred operand order from original center-minus-player loads; threshold bits are 0x358637bd.
                        if (flDistSqr < EPSILON || Vector3D::Distance2DSquared(tPlace.wp, tPlaceCenter.wp) < flDistSqr)
                        {
                            // Inferred flag assignment: accepted players branch around removal.
                            fCloseEnough = TRUE;
                        }
                    }
                    if (!fCloseEnough)
                    {
                        listDestPlayers.RemoveAt(posOld);
                    }
                }
            }
        }





        MessageSendToClient(listDestPlayers, SMSG_PLAY_MUSIC, &tMessage, sizeof(tMessage));
    }
    else
    {
        // Original tail call passes zero first index and false shuffle-disable flag.
        SoundManager::PlayMusic(snoMusic, 0, FALSE);
    }

}

// DWARF: X/Server/Actors/Actors.cpp:5915, absent from the adopted Actors.h subset.
ActorID GBAddWithEncounterAddStruct(ActorAddStruct & tActorAdd, EncounterAddStruct & tEncounterAdd);
#line 14375 "X/Server/LuaScript/ScriptUtil.cpp"
ACDID ActorSpawnActorInWorldOnRandomAdventure(
    LPCSTR szActorName,
    LPCSTR szWorldName,
    LPCSTR szAdventureName)
{

    SNO snoActor = SNOFileLookupGUID(ACTOR_SNO, szActorName);
    if (snoActor == SNO_NONE)
        return ACDID_INVALID;




    SNO snoWorld = SNOFileLookupGUID(WORLD_SNO, szWorldName);
    if (snoWorld == SNO_NONE)
        return ACDID_INVALID;




    SWorldID idSWorld = WorldCommonFindBySNO(snoWorld, TRUE);
    if (idSWorld == SWORLDID_INVALID)
        return ACDID_INVALID;




    SNO snoAdventure = SNOFileLookupGUID(ADVENTURE_SNO, szAdventureName);
    if (snoAdventure == SNO_NONE)
        return ACDID_INVALID;




    // Inferred world initialization: the selected position is paired with idSWorld at actor creation.
    WorldPlace tPlaceRandom;
    tPlaceRandom.idSWorld = idSWorld;
    SSceneID idSSceneRandom = SSCENEID_INVALID;
    // Initial zero count and null iterator target are evidenced by the original stores.
    int32 nFoundAdventures = 0;
    SceneCommonData * ptSCD = NULL;
    SceneCommonDataIterator tIterator;
    while (SceneCommonDataArrayIterate(tIterator, ptSCD, SkipHolodeckScenes))
    {
        if (ptSCD->idSWorld != idSWorld)
        {
            continue;
        }

        POS pos = ptSCD->listGizmoLocPlacements.GetHeadPosition();
        while (pos)
        {
            GizmoLocPlacement & tPlacement = ptSCD->listGizmoLocPlacements.GetNext(pos);
            if (tPlacement.snoname.snoHandle == snoAdventure)
            {
                ++nFoundAdventures;
                if (nFoundAdventures == 1 ||
                    RandFloat_Internal(SGameGlobalsGet().rng) <= 1.0f / nFoundAdventures)
                {
                    PRTransform transform = ptSCD->transformWorld * tPlacement.tPlace.transform;
                    tPlaceRandom.wp = transform.wp;
                    idSSceneRandom = ptSCD->idSScene;
                }
            }
        }
    }


    SceneCommonData * ptSCDRandom = SceneCommonDataTryToGet(idSSceneRandom);
    if (!ptSCDRandom)
        return ACDID_INVALID;




    // Original bit-zero test skips activation for scenes already marked activated.
    if (!(ptSCDRandom->dwSceneFlags & (1 << SCENE_ACTIVATED)))
    {
        SceneCommonDataActivate(ptSCDRandom->idSScene);
    }


    ActorAddStruct tActorAdd;
    tActorAdd.tSNOName.snoGroup = ACTOR_SNO;
    tActorAdd.tSNOName.snoHandle = snoActor;
    tActorAdd.tPlace = tPlaceRandom;
    tActorAdd.snoActor = snoActor;
    tActorAdd.tMarkersName = MarkerStringHash("RandomAdventure_SpawnProxy");
    tActorAdd.dwActorAddFlags = (1 << ACTORADD_EVEN_WHEN_STUCK) | (1 << ACTORADD_DONT_PERTURB) | (1 << ACTORADD_DONT_SEARCH_FOR_LOCATION) | (1 << ACTORADD_DOESNT_DEACTIVATE);




    EncounterAddStruct tEncounterAdd;
    GBHandleList listGroupAffixes(GBGetHandlePool());
    EncounterAddInitialize(tEncounterAdd, listGroupAffixes);

    ActorID idActor = GBAddWithEncounterAddStruct(tActorAdd, tEncounterAdd);
    if (idActor == ACTORID_INVALID)
        return ACDID_INVALID;




    return ACDFindByActor(idActor);
}

#line 14484 "X/Server/LuaScript/ScriptUtil.cpp"
ACDID ActorSpawnActorInWorldOnClosestAdventure(
    LPCSTR szActorName,
    LPCSTR szWorldName,
    LPCSTR szAdventureName,
    VACDID idActorOrigin)
{

    SNO snoActor = SNOFileLookupGUID(ACTOR_SNO, szActorName);
    if (snoActor == SNO_NONE)
        return ACDID_INVALID;




    SNO snoWorld = SNOFileLookupGUID(WORLD_SNO, szWorldName);
    if (snoWorld == SNO_NONE)
        return ACDID_INVALID;




    SWorldID idSWorld = WorldCommonFindBySNO(snoWorld, TRUE);
    if (idSWorld == SWORLDID_INVALID)
        return ACDID_INVALID;




    SNO snoAdventure = SNOFileLookupGUID(ADVENTURE_SNO, szAdventureName);
    if (snoAdventure == SNO_NONE)
        return ACDID_INVALID;




    ActorCommonData * ptACDOrigin = ACDTryToGet(idActorOrigin);
    if (!ptACDOrigin)
        return ACDID_INVALID;




    WorldPlace wpOrigin = ptACDOrigin->GetWorldPlaceCenter();
    if (wpOrigin.idSWorld != idSWorld)
        return ACDID_INVALID;




    // Inferred world initialization, as in ActorSpawnActorInWorldOnRandomAdventure: the original stores idSWorld at actor creation.
    WorldPlace tPlaceClosest;
    tPlaceClosest.idSWorld = idSWorld;
    float flClosestDistSq = FLT_MAX;
    SSceneID idSSceneClosest = SSCENEID_INVALID;

    SceneCommonData * ptSCD = NULL;
    SceneCommonDataIterator tIterator;
    while (SceneCommonDataArrayIterate(tIterator, ptSCD, SkipHolodeckScenes))
    {
        if (ptSCD->idSWorld != idSWorld)
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
                float flDistSq = Vector3D::Distance2DSquared(transform.wp, wpOrigin.wp);
                if (flDistSq < flClosestDistSq)
                {
                    flClosestDistSq = flDistSq;
                    tPlaceClosest.wp = transform.wp;
                    idSSceneClosest = ptSCD->idSScene;
                }
            }
        }
    }


    SceneCommonData * ptSCDClosest = SceneCommonDataTryToGet(idSSceneClosest);
    if (!ptSCDClosest)
        return ACDID_INVALID;




    // Original bit-zero test skips activation for scenes already marked activated.
    if (!(ptSCDClosest->dwSceneFlags & (1 << SCENE_ACTIVATED)))
    {
        SceneCommonDataActivate(ptSCDClosest->idSScene);
    }


    ActorAddStruct tActorAdd;
    tActorAdd.tSNOName.snoGroup = ACTOR_SNO;
    tActorAdd.tSNOName.snoHandle = snoActor;
    tActorAdd.tPlace = tPlaceClosest;
    tActorAdd.snoActor = snoActor;
    tActorAdd.tMarkersName = MarkerStringHash("ClosestAdventure_SpawnProxy");
    tActorAdd.dwActorAddFlags = (1 << ACTORADD_EVEN_WHEN_STUCK) | (1 << ACTORADD_DONT_PERTURB) | (1 << ACTORADD_DONT_SEARCH_FOR_LOCATION) | (1 << ACTORADD_DOESNT_DEACTIVATE);




    EncounterAddStruct tEncounterAdd;
    GBHandleList listGroupAffixes(GBGetHandlePool());
    EncounterAddInitialize(tEncounterAdd, listGroupAffixes);

    ActorID idActor = GBAddWithEncounterAddStruct(tActorAdd, tEncounterAdd);
    if (idActor == ACTORID_INVALID)
        return ACDID_INVALID;




    return ACDFindByActor(idActor);
}

#line 14608 "X/Server/LuaScript/ScriptUtil.cpp"
ACDID ActorSpawnActorInWorldOnFarthestAdventure(
    LPCSTR szActorName,
    LPCSTR szWorldName,
    LPCSTR szAdventureName,
    VACDID idActorOrigin)
{

    SNO snoActor = SNOFileLookupGUID(ACTOR_SNO, szActorName);
    if (snoActor == SNO_NONE)
        return ACDID_INVALID;




    SNO snoWorld = SNOFileLookupGUID(WORLD_SNO, szWorldName);
    if (snoWorld == SNO_NONE)
        return ACDID_INVALID;




    SWorldID idSWorld = WorldCommonFindBySNO(snoWorld, TRUE);
    if (idSWorld == SWORLDID_INVALID)
        return ACDID_INVALID;




    SNO snoAdventure = SNOFileLookupGUID(ADVENTURE_SNO, szAdventureName);
    if (snoAdventure == SNO_NONE)
        return ACDID_INVALID;




    ActorCommonData * ptACDOrigin = ACDTryToGet(idActorOrigin);
    if (!ptACDOrigin)
        return ACDID_INVALID;




    WorldPlace wpOrigin = ptACDOrigin->GetWorldPlaceCenter();
    if (wpOrigin.idSWorld != idSWorld)
        return ACDID_INVALID;




    // Inferred world initialization, as in ActorSpawnActorInWorldOnRandomAdventure: the original stores idSWorld at actor creation.
    WorldPlace tPlaceFarthest;
    tPlaceFarthest.idSWorld = idSWorld;
    float flFarthestDistSq = 0.0f;
    SSceneID idSSceneFarthest = SSCENEID_INVALID;

    SceneCommonData * ptSCD = NULL;
    SceneCommonDataIterator tIterator;
    while (SceneCommonDataArrayIterate(tIterator, ptSCD, SkipHolodeckScenes))
    {
        if (ptSCD->idSWorld != idSWorld)
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
                float flDistSq = Vector3D::Distance2DSquared(transform.wp, wpOrigin.wp);
                if (flDistSq > flFarthestDistSq)
                {
                    flFarthestDistSq = flDistSq;
                    tPlaceFarthest.wp = transform.wp;
                    idSSceneFarthest = ptSCD->idSScene;
                }
            }
        }
    }


    SceneCommonData * ptSCDFarthest = SceneCommonDataTryToGet(idSSceneFarthest);
    if (!ptSCDFarthest)
        return ACDID_INVALID;




    // Original bit-zero test skips activation for scenes already marked activated.
    if (!(ptSCDFarthest->dwSceneFlags & (1 << SCENE_ACTIVATED)))
    {
        SceneCommonDataActivate(ptSCDFarthest->idSScene);
    }


    ActorAddStruct tActorAdd;
    tActorAdd.tSNOName.snoGroup = ACTOR_SNO;
    tActorAdd.tSNOName.snoHandle = snoActor;
    tActorAdd.tPlace = tPlaceFarthest;
    tActorAdd.snoActor = snoActor;
    tActorAdd.tMarkersName = MarkerStringHash("FarthestAdventure_SpawnProxy");
    tActorAdd.dwActorAddFlags = (1 << ACTORADD_EVEN_WHEN_STUCK) | (1 << ACTORADD_DONT_PERTURB) | (1 << ACTORADD_DONT_SEARCH_FOR_LOCATION) | (1 << ACTORADD_DOESNT_DEACTIVATE);




    EncounterAddStruct tEncounterAdd;
    GBHandleList listGroupAffixes(GBGetHandlePool());
    EncounterAddInitialize(tEncounterAdd, listGroupAffixes);

    ActorID idActor = GBAddWithEncounterAddStruct(tActorAdd, tEncounterAdd);
    if (idActor == ACTORID_INVALID)
        return ACDID_INVALID;




    return ACDFindByActor(idActor);
}
