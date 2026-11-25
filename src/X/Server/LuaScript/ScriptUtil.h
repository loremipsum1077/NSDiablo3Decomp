// Adopted from the DWARF reference; declarations below come from the owning source file.
// Original file: X/Server/LuaScript/ScriptUtil.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Common/CommonTypes.h"

typedef int32 SNO;

typedef int32 HirelingClass;

typedef int32 AIStateType;

typedef int32 Team;

typedef int32 AIQueuedActionID;

typedef int32 Act;

typedef int32 ActorAnimationTag;

typedef int32 AIFollowType;

typedef int32 AIBehaviorType;

typedef int32 DeathType;

typedef int32 KnockbackStyle;

typedef int32 RuneType;

typedef int32 CalloutEventType;

typedef int32 WeaponClass;

typedef int32 CrafterType;

typedef int32 ItemLabel;

typedef int32 SpecialEventFlag;

typedef int32 QuestStepObjectiveType;

typedef int32 QuestType;

typedef ACDID VACDID;

typedef ACDID VPACDID;

enum {
    STRINGLIST_DEFAULT_INT_PARAM = -16843010,
};




// Lua binding native declarations, recovered from the owning .cpp DWARF.
#include "../../Sigma/Unicode.h"
#include "../../Common/GameSwig.h"
#include "../../Common/ActorCommonDataSwig.h"
#include "../../Common/Powers/PowerAPISwig.h"

int32 ActorGroupGetCount(LPCSTR szGroupName);  // ScriptUtil.cpp:734
BOOL ActorIsPlayerTeam(ACDID idACDActor);  // ScriptUtil.cpp:3343
BOOL ActorIsMonsterTeam(ACDID idACDActor);  // ScriptUtil.cpp:3356
void ActorSetCanActivate(LPCSTR szMarkerName);  // ScriptUtil.cpp:3720
BOOL IsMonster(ACDID idACDMonster);  // ScriptUtil.cpp:9891
SNO ActorGetSNO(ACDID idACD);  // ScriptUtil.cpp:3410
BOOL ActorIsAlive(ACDID idActor);  // ScriptUtil.cpp:942
BOOL ActorIsValid(ACDID idActor);  // ScriptUtil.cpp:1214
BOOL ActorIsPet(ACDID idActor);  // ScriptUtil.cpp:1236
BOOL ActorIsHireling(ACDID idActor);  // ScriptUtil.cpp:1249
BOOL ActorIsAvenger(ACDID idActor);  // ScriptUtil.cpp:1262
BOOL ActorIsLegendaryItem(ACDID idActor);  // ScriptUtil.cpp:1277
BOOL ActorIsResplendentChest(ACDID idActor);  // ScriptUtil.cpp:1290
BOOL ActorIsInTown(ACDID idActor);  // ScriptUtil.cpp:1309
BOOL ActorIsInCombat(ACDID idActor);  // ScriptUtil.cpp:1322
BOOL ActorIsKnockbackable(ACDID idActor);  // ScriptUtil.cpp:1338
BOOL ActorIsInNoBanterArea(ACDID idActor);  // ScriptUtil.cpp:1354
BOOL PlayerIsListeningToLore(ACDID idACDPlayer);  // ScriptUtil.cpp:1367
BOOL ActorDropsNothing(ACDID idActor);  // ScriptUtil.cpp:1380
void ActorSetLeader(ACDID idActor, BOOL bValue);  // ScriptUtil.cpp:2866
void ActorSetOperable(ACDID idActor, BOOL bValue);  // ScriptUtil.cpp:2880
void ActorSetCannotBeTargetedByAI(ACDID idACDActor, BOOL bValue);  // ScriptUtil.cpp:1813
float ActorGetHitPointsMax(ACDID idACD);  // ScriptUtil.cpp:8461
float ActorGetSinglePlayerHitpointsMax(ACDID idACD);  // ScriptUtil.cpp:8475
float ActorGetHitPointsPercent(ACDID idActor);  // ScriptUtil.cpp:8519
float ActorGetHitPoints(ACDID idActor);  // ScriptUtil.cpp:8532
void ActorScaleHPAndDamageToTieredRiftLevel(ACDID idActor, int32 nTieredRiftLevel);  // ScriptUtil.cpp:8582
MonsterType ActorGetMonsterType(ACDID idACD);  // ScriptUtil.cpp:10816
MonsterFamily ActorGetMonsterFamily(ACDID idACD);  // ScriptUtil.cpp:10834
int32 AvengerGetPlayerKills(ACDID idACD);  // ScriptUtil.cpp:10852
BOOL ActorIsAIEnabled(ACDID idACDActor);  // ScriptUtil.cpp:1796
AIQueuedActionID QActorEventSendToActor(ACDID idActor, LPCSTR szEventName, ACDID idActorTarget);  // ScriptUtil.cpp:8397
BOOL ShouldThrottleScriptActors();  // ScriptUtil.cpp:8805
void ActorInterruptPrimaryPower(ACDID idActor);  // ScriptUtil.cpp:5870
void ActorTriggerBrainStancePowerAtTarget(ACDID idACD, ACDID idACDTarget, LPCSTR szPower);  // ScriptUtil.cpp:9104
void ActorSnapFaceActor(ACDID idActor, ACDID idTarget);  // ScriptUtil.cpp:6634
void ActorSnapFaceAbsolute(ACDID idActor, int32 nDegrees);  // ScriptUtil.cpp:6660
void ClientPlayEffectGroup(ACDID idActor, ACDID idClient, LPCSTR szEffectGroupName);  // ScriptUtil.cpp:1494
void ClientStopEffectGroup(ACDID idActor, ACDID idClient, LPCSTR szEffectGroupName);  // ScriptUtil.cpp:1544
void ActorSheathWeapons(ACDID idActor, BOOL bSheath);  // ScriptUtil.cpp:8999
void TestingKillAllMonsters();  // ScriptUtil.cpp:9679
void ActorSpecialDeathEnable(ACDID idACDActor, BOOL bValue);  // ScriptUtil.cpp:1825
void ActorClearBuffsFlaggedForGreaterRiftEntry(ACDID idActor);  // ScriptUtil.cpp:2810
int32 PlayerGetNumInGame();  // ScriptUtil.cpp:8512
BOOL PlayerIsRangedClass(ACDID idACDPlayer);  // ScriptUtil.cpp:5417
void PlayerAssignSkillToSlot(ACDID idPlayer, LPCSTR szPower, RuneType eRuneType, int32 nSlot);  // ScriptUtil.cpp:5623
void PlayerAssignTraitToSlot(ACDID idPlayer, LPCSTR szTrait, int32 nSlot);  // ScriptUtil.cpp:5691
void PlayerAssignItemToPotionSlot(ACDID idPlayer, LPCSTR szItemName);  // ScriptUtil.cpp:5724
void AllPlayersLearnLore(LPCSTR szLore);  // ScriptUtil.cpp:5797
void WaypointActivate(LPCSTR szLevelAreaName);  // ScriptUtil.cpp:5922
int32 PlayerCount();  // ScriptUtil.cpp:6047
void PlayerClearReturnPointsInWorld(LPCSTR szWorld);  // ScriptUtil.cpp:6054
BOOL PlayerIsPrimary(ACDID idPlayer);  // ScriptUtil.cpp:12117
ACDID GetPrimaryPlayer();  // ScriptUtil.cpp:12134
void PlayerSetNephalemGloryActive(ACDID idPlayer, BOOL bActive);  // ScriptUtil.cpp:12150
void PlayerSpendParagonPoints(ACDID idPlayer, int32 nCategory, int32 nAttribute, int32 nNumPoints);  // ScriptUtil.cpp:12164
void PlayerSetLevel(ACDID idPlayer, int32 nLevel);  // ScriptUtil.cpp:12202
void PlayerEnableAutoEquip(ACDID idPlayer, BOOL bEnabled);  // ScriptUtil.cpp:12218
void CameraScriptedSequenceStop();  // ScriptUtil.cpp:11146
void CameraTriggerFade();  // ScriptUtil.cpp:10983
void CameraTriggerInstantFadeToBlack();  // ScriptUtil.cpp:11007
void CameraTriggerFadeFromBlack();  // ScriptUtil.cpp:11018
void SetCameraOrbit(ACDID idACDPlayer, ACDID idACDToOrbit);  // ScriptUtil.cpp:11303
void SetCameraDefaults(ACDID idACDPlayer);  // ScriptUtil.cpp:11330
void Console_SetCameraDefaults(ACDID idACDPlayer, BOOL bForce);  // ScriptUtil.cpp:11348
void PlayCutsceneForPlayer(const int32 nIndex, VACDID idACDPlayer);  // ScriptUtil.cpp:6287
void WeatherOverride(LPCSTR szWorld, LPCSTR szWeather);  // ScriptUtil.cpp:9726
void ClearWeatherOverride(LPCSTR szWorld);  // ScriptUtil.cpp:9756
void MusicPlayForPlayer(LPCSTR szMusic, ACDID idActorPlayer);  // ScriptUtil.cpp:6381
void MusicStopOverlays();  // ScriptUtil.cpp:6407
void MusicStopOverlaysForPlayer(ACDID idActorPlayer);  // ScriptUtil.cpp:6421
void CancelConversation(LPCSTR szConversation);  // ScriptUtil.cpp:2273
BOOL PlayerHasUnlockedConversation(ACDID idACDPlayer, LPCSTR szConversation);  // ScriptUtil.cpp:2560
void PlayerUnlockConversation(ACDID idACDPlayer, LPCSTR szConversation);  // ScriptUtil.cpp:2583
void PlayerSaveConversation(ACDID idACDPlayer, LPCSTR szConversation);  // ScriptUtil.cpp:2610
BOOL PlayerHasHeardConversation(ACDID idACDPlayer, LPCSTR szConversation);  // ScriptUtil.cpp:2637
void CalloutSendToPlayer(CalloutEventType eType, ACDID idACDPlayer);  // ScriptUtil.cpp:6727
void UnlockCrafter(VACDID idActorPlayer, CrafterType eCrafterType);  // ScriptUtil.cpp:11686
BOOL IsCrafterUnlocked(ACDID idActorPlayer, CrafterType eCrafterType);  // ScriptUtil.cpp:11700
void PlayerEnableFreeEnchant(ACDID idActorPlayer);  // ScriptUtil.cpp:11714
void ConductorStart(LPCSTR szSNOName);  // ScriptUtil.cpp:10000
void ConductorAdvancePhase();  // ScriptUtil.cpp:10012
void ConductorSkipToPhase(LPCSTR szPhaseName);  // ScriptUtil.cpp:10039
void ConductorStop();  // ScriptUtil.cpp:10067
BOOL QuestIsComplete(ACDID idACDPlayer, LPCSTR szQuestName);  // ScriptUtil.cpp:10603
void RegisterQuestActor(ACDID idActor, BOOL bEnable);  // ScriptUtil.cpp:10705
BOOL QuestRangeValid(LPCSTR szQuestRangeName);  // ScriptUtil.cpp:10767
void TimedEventStart(LPCSTR szSNOName);  // ScriptUtil.cpp:10094
void StartChallengeRiftTimer();  // ScriptUtil.cpp:10112
BOOL TimedEventHasExpired(LPCSTR szSNOName);  // ScriptUtil.cpp:10146
void TimedEventReset(LPCSTR szSNOName);  // ScriptUtil.cpp:10164
void QuestReset(LPCSTR szQuestName);  // ScriptUtil.cpp:10242
QuestType QuestGetQuestType(LPCSTR szQuest);  // ScriptUtil.cpp:10272
Act QuestGetBountyAct(LPCSTR szQuest);  // ScriptUtil.cpp:10288
void QuestHideCompletedDynamicObjectives();  // ScriptUtil.cpp:10443
void ChallengeKillAllMonsters(LPCSTR szQuestName);  // ScriptUtil.cpp:10482
BOOL ItemGiveQuestItem(LPCSTR szItemName);  // ScriptUtil.cpp:4258
BOOL ItemIsAncient(ACDID idACDItem);  // ScriptUtil.cpp:4337
SNO TreasureClassSNOFromName(LPCSTR szTreasureClass);  // ScriptUtil.cpp:6472
SNO LevelAreaSNOFromName(LPCSTR szLevelArea);  // ScriptUtil.cpp:6484
BOOL GameFlagIsSet(LPCSTR szFlagLabel);  // ScriptUtil.cpp:6208
void GameFlagSet(LPCSTR szFlagLabel, BOOL bValue);  // ScriptUtil.cpp:6215
void GameFlagSendSyncedFlags();  // ScriptUtil.cpp:6249
BOOL GameIsBlizzcon();  // ScriptUtil.cpp:6843
BOOL GameIsPTR();  // ScriptUtil.cpp:6851
BOOL GameIsBattleNet();  // ScriptUtil.cpp:6863
BOOL GameSetOpenWorldMode(ACDID idActorPlayer, OpenWorldMode eOpenWorldMode, Act eOpenWorldModeAct, int32 nOpenWorldParam, LPCSTR szHubWorld, LPCSTR szHubEntranceName);  // ScriptUtil.cpp:6872
BOOL GameSetOpenWorldModeIdle();  // ScriptUtil.cpp:6921
void GameSetOpenWorldModeParam(int32 nParam);  // ScriptUtil.cpp:6928
int32 GameGetDifficulty();  // ScriptUtil.cpp:7081
int32 ConversationVarGet(LPCSTR szVar);  // ScriptUtil.cpp:12437
void ConversationVarSet(LPCSTR szVar, int32 nValue);  // ScriptUtil.cpp:12466
BOOL TriggerBetaEnd();  // ScriptUtil.cpp:7740
BOOL TriggerDemoEnd();  // ScriptUtil.cpp:7769
double TimeGetGameTime();  // ScriptUtil.cpp:7792
int32 TimeGetDayOfWeek();  // ScriptUtil.cpp:7802
int32 TimeGetHour();  // ScriptUtil.cpp:7814
int32 TimeGetDay();  // ScriptUtil.cpp:7826
int32 TimeGetMonth();  // ScriptUtil.cpp:7838
void ChangeActs(Act eAct);  // ScriptUtil.cpp:9868
Act GetCurrentAct();  // ScriptUtil.cpp:9876
BOOL SceneIsInLevelArea(SSceneID idSScene, LPCSTR szLevelAreaName);  // ScriptUtil.cpp:6440
SSceneID SceneGroupFind(LPCSTR szSceneGroup);  // ScriptUtil.cpp:6496
SSceneID SceneGroupFindByName(LPCSTR szMarkerName);  // ScriptUtil.cpp:6520
void SceneGroupSwap(SSceneID idSScene, int32 nSubSceneIndex);  // ScriptUtil.cpp:6554
int32 SceneGroupGetSubSceneCount(SSceneID idSScene);  // ScriptUtil.cpp:6585
int32 SceneGroupGetCurrentSubScene(SSceneID idSScene);  // ScriptUtil.cpp:6606
BOOL PVPRoundsEnabled();  // ScriptUtil.cpp:6774
BOOL PVPIsSkirmishMode();  // ScriptUtil.cpp:6782
void PVPSetObserver(ACDID idActor);  // ScriptUtil.cpp:6789
BOOL ActorIsObserver(ACDID idActor);  // ScriptUtil.cpp:994
BOOL ActorIsStealthed(ACDID idActor);  // ScriptUtil.cpp:1007
BOOL ActorIsStunned(ACDID idActor);  // ScriptUtil.cpp:1020
BOOL ActorIsHidden(ACDID idActor);  // ScriptUtil.cpp:1033
BOOL ActorIsGhost(ACDID idActor);  // ScriptUtil.cpp:1046
BOOL ActorIsElite(ACDID idActor);  // ScriptUtil.cpp:1059
BOOL ActorGrantsExperience(ACDID idActor);  // ScriptUtil.cpp:1072
BOOL ActorIsRareOrChampion(ACDID idActor);  // ScriptUtil.cpp:1095
BOOL ActorIsBoss(ACDID idActor);  // ScriptUtil.cpp:1116
BOOL ActorIsLargeMonster(ACDID idActor);  // ScriptUtil.cpp:1137
BOOL ActorIsOnSpecial0(ACDID idActor);  // ScriptUtil.cpp:1150
BOOL ActorIsOnSpecial1(ACDID idActor);  // ScriptUtil.cpp:1172
BOOL ActorCanGoOnFlyerNavMesh(ACDID idActor);  // ScriptUtil.cpp:1194
BOOL GameIsPvP();  // ScriptUtil.cpp:6825
BOOL GameIsConsole();  // ScriptUtil.cpp:6800
BOOL GameIsLastGenConsole();  // ScriptUtil.cpp:6811
BOOL GameHasHighScriptInstanceCount();  // ScriptUtil.cpp:6818
void TriggerPVPTeamDeathmatchGameEnd();  // ScriptUtil.cpp:7088
int32 PVPTeamGetScore(Team eTeam);  // ScriptUtil.cpp:7096
int32 PVPPlayerGetKills(ACDID idACDPlayer);  // ScriptUtil.cpp:7167
int32 PVPPlayerGetAssists(ACDID idACDPlayer);  // ScriptUtil.cpp:7182
int32 PVPPlayerGetDeaths(ACDID idACDPlayer);  // ScriptUtil.cpp:7197
BOOL PVPGameIsOver();  // ScriptUtil.cpp:7212
int32 PVPPlayerGetRank(ACDID idACDPlayer);  // ScriptUtil.cpp:7280
void PVPSetAwardsAssigned();  // ScriptUtil.cpp:7292
void PVPAwardPVPXP(ACDID idACDPlayer, int32 nAmount);  // ScriptUtil.cpp:7319
void PVPAwardNormalXP(ACDID idACDPlayer, int32 nAmount);  // ScriptUtil.cpp:7301
void PVPAwardGold(ACDID idACDPlayer, int32 nAmount);  // ScriptUtil.cpp:7337
void PVPAwardTokens(ACDID idACDPlayer, int32 nAmount);  // ScriptUtil.cpp:7355
int32 PVPGetGoldAwardAmount(uint32 nLevel, PVPGameResult ePVPGameResult);  // ScriptUtil.cpp:7374
int32 PVPGetGoldAwardAmountForRank(uint32 nRank, PVPGameResult ePVPGameResult);  // ScriptUtil.cpp:7398
int32 PVPGetNormalXPAwardAmount(uint32 nLevel, PVPGameResult ePVPGameResult);  // ScriptUtil.cpp:7447
int32 PVPGetAltXPAwardAmount(uint32 nLevel, PVPGameResult ePVPGameResult);  // ScriptUtil.cpp:7471
int32 PVPGetPVPXPAwardAmount(uint32 nLevel, PVPGameResult ePVPGameResult);  // ScriptUtil.cpp:7422
int32 PVPGetTokensAwardAmount(uint32 nRank, PVPGameResult ePVPGameResult);  // ScriptUtil.cpp:7495
double PVPPlayerGetTeamRating(ACDID idACDPlayer);  // ScriptUtil.cpp:7519
double PVPPlayerGetTeamVariance(ACDID idACDPlayer);  // ScriptUtil.cpp:7537
double PVPPlayerGetIndividualRating(ACDID idACDPlayer);  // ScriptUtil.cpp:7556
double PVPPlayerGetIndividualVariance(ACDID idACDPlayer);  // ScriptUtil.cpp:7575
uint32 PVPPlayerGetTeamID(ACDID idACDPlayer);  // ScriptUtil.cpp:7593
PVPGameResult PVPPlayerGetGameResult(ACDID idACDPlayer);  // ScriptUtil.cpp:7687
int32 GetRandomInt(int32 nMin, int32 nMax);  // ScriptUtil.cpp:9806
float GetRandomFloat();  // ScriptUtil.cpp:9822
int32 GetRandomIntForDate(int32 nMin, int32 nMax, LPCSTR szKey);  // ScriptUtil.cpp:9839
void WorldCreate(LPCSTR szWorldName);  // ScriptUtil.cpp:5027
BOOL WorldIsCreated(LPCSTR szWorldName);  // ScriptUtil.cpp:5046
BOOL WorldHasEvent(LPCSTR szWorldName, LPCSTR szEventName);  // ScriptUtil.cpp:5071
BOOL WorldHasChallenge(LPCSTR szWorldName, LPCSTR szChallengeName);  // ScriptUtil.cpp:5186
void PrefetchClient(LPCSTR szGroup, LPCSTR szName);  // ScriptUtil.cpp:9791
void PreFetchSNO(LPCSTR szGroupName, LPCSTR szSNOName);  // ScriptUtil.cpp:5278
float MonsterGetExperienceScalar(ACDID idACDMonster);  // ScriptUtil.cpp:12083
BOOL BossEncounterIsComplete(LPCSTR szBossEncounter);  // ScriptUtil.cpp:12102
void MinimapRemovePing(int32 nPingID);  // ScriptUtil.cpp:11468
void MinimapAddActorPing(ACDID idActor);  // ScriptUtil.cpp:11475
void MinimapRemoveActorPing(ACDID idActor);  // ScriptUtil.cpp:11482
BOOL PlayerHasPlayedAdventureModeTutorial(ACDID idACDPlayer);  // ScriptUtil.cpp:11519
void PlayerPlayAdventureModeTutorial(ACDID idACDPlayer);  // ScriptUtil.cpp:11533
void TriggerAssert();  // ScriptUtil.cpp:9408
ACDID ResetHero(const ACDID idACDPlayer, const char * szClassName, int32 nGender);  // ScriptUtil.cpp:9417
void SetGameMonsterLevel(const int32 nLevel);  // ScriptUtil.cpp:9459
void SetSpecialEventFlag(ACDID idPlayer, SpecialEventFlag eEventFlag, BOOL bValue);  // ScriptUtil.cpp:9474
BOOL GetSpecialEventFlag(ACDID idPlayer, SpecialEventFlag eEventFlag);  // ScriptUtil.cpp:9510
void TestingAddDungeonFinder(LPCSTR szDungeonFinder, Act eAct);  // ScriptUtil.cpp:9572
void TestingAdvance();  // ScriptUtil.cpp:9590
void TestingTriggerSkill();  // ScriptUtil.cpp:9605
void TestingSetupPerfSkillTest();  // ScriptUtil.cpp:9617
void TestingStartSampling();  // ScriptUtil.cpp:9665
void TestingStopSampling();  // ScriptUtil.cpp:9672
uint32 TestingGetSampleTime();  // ScriptUtil.cpp:9629
uint32 TestingGetWorldAdvanceWaitTime();  // ScriptUtil.cpp:9636
BOOL TestingLoadingScreenIsEnabled();  // ScriptUtil.cpp:9644
int32 PlayerGetIndex(ACDID idPlayer);  // ScriptUtil.cpp:12493
float ActorGetRareScaleFactor(ACDID idACD);  // ScriptUtil.cpp:12507
void DungeonFinderSetTimedEvent(LPCSTR szSNOName);  // ScriptUtil.cpp:12592
void DungeonFinderSetBossPortal(LPCSTR szBossPortalActor);  // ScriptUtil.cpp:12613
BOOL DungeonFinderIsEmpty();  // ScriptUtil.cpp:12642
void DungeonFinderReset();  // ScriptUtil.cpp:12691
int32 DungeonFinderGetTieredLevel();  // ScriptUtil.cpp:12699
int32 DungeonFingerGetNumParticipants();  // ScriptUtil.cpp:12707
BOOL DungeonFinderIsComplete();  // ScriptUtil.cpp:12747
void DungeonFinderPickEntrancePortalLevelArea();  // ScriptUtil.cpp:12755
SNO DungeonFinderEngineGetSNO();  // ScriptUtil.cpp:12779
ACDID DungeonFinderSpawnBoss(ACDID idACDPlayer);  // ScriptUtil.cpp:12803
void DungeonFinderGivePlayersRewardChoices(BOOL bSuccess);  // ScriptUtil.cpp:12834
void SetDungeonReset();  // ScriptUtil.cpp:12842
BOOL ActorIsGold(ACDID idACDItem);  // ScriptUtil.cpp:12915
BOOL ActorIsCorpse(ACDID idACDCorpse);  // ScriptUtil.cpp:12924
BOOL ActorCanFreelyCastCorpseSkills(ACDID idACDActor);  // ScriptUtil.cpp:12938
BOOL ActorIsGizmo(ACDID idACD);  // ScriptUtil.cpp:12952
BOOL PlayerPaidForLootRun(ACDID idACDPlayer);  // ScriptUtil.cpp:12989
BOOL PlayerIsInChallengeRift(ACDID idACDPlayer);  // ScriptUtil.cpp:13003
BOOL ActorIsInLootRunWorld(ACDID idActor);  // ScriptUtil.cpp:13031
uint32 GameGetChallengeRiftTargetTime();  // ScriptUtil.cpp:13085
void GamePostChallengeRiftTime(int32 nMillisecondsElapsed);  // ScriptUtil.cpp:13097
void GamePostRiftTime(int riftLevel, int millisecondsElapsed);  // ScriptUtil.cpp:13105
void GameUnlockRiftLevelsForParticipants(int32 nRiftLevel);  // ScriptUtil.cpp:13188
void GameSetPlayersLastStartedRiftLevel(int32 nRiftLevel);  // ScriptUtil.cpp:13196
int32 GameGetArtificiallyElapsedRiftTimeMS();  // ScriptUtil.cpp:13180
BOOL GameRiftIsOrphaned();  // ScriptUtil.cpp:13587
BOOL GameRiftIsDynamicPylonSpawningDisabled();  // ScriptUtil.cpp:13568
int32 TimeGetMachineTicks();  // ScriptUtil.cpp:13594
void GameSetLootRunBossKilled(BOOL bKilled);  // ScriptUtil.cpp:13607
void GameSetLootRunChallengeCompleted(BOOL bCompleted);  // ScriptUtil.cpp:13626
void GameLootRunNotifyClientsPreloadLeaderboardData(int32 nTotalTimeMS);  // ScriptUtil.cpp:13641
void QuestShowTieredRiftEndScreen(BOOL bFinishedInTime, uint32 nRiftDepth, int32 nTotalTimeMS, uint32 nGoldTier, uint32 nXPTier);  // ScriptUtil.cpp:13768
void GameChallengeRiftHandleCompletion(BOOL bFinishedInTime, int32 nTotalTimeMS);  // ScriptUtil.cpp:13802
void GameShowWeeklyChallengeLeaderboard();  // ScriptUtil.cpp:13837
float GameGetLootRunProgressPercent();  // ScriptUtil.cpp:13856
void GameSetLootRunBossActive(BOOL bActive);  // ScriptUtil.cpp:13873
void GameKillAllTieredLootRunMonsters();  // ScriptUtil.cpp:13888
void GameKillAllMonstersInWorld(ACDID idACDActorWorld);  // ScriptUtil.cpp:13920
void GameClearLootRunState();  // ScriptUtil.cpp:13949
void GameSetLootRunJewelUpgradeVendor(ACDID idACDJewelUpgradeVendor);  // ScriptUtil.cpp:13957
ACDID GameGetLootRunJewelUpgradeVendor();  // ScriptUtil.cpp:13966
void GameDestroyDeathPortalWorld(LPCSTR szWorld);  // ScriptUtil.cpp:14054
void GameBlizzconShowGreaterRiftEndScreen();  // ScriptUtil.cpp:14128
void GameBlizzconOpenGreaterRift(ACDID idObelisk, ACDID idPlayer, int32 nTier);  // ScriptUtil.cpp:14236
void GameBlizzconManuallyTeleportPlayerToGreaterRift(ACDID idPlayer);  // ScriptUtil.cpp:14251
void PlayerBlizzconUnlockCube(ACDID idPlayer);  // ScriptUtil.cpp:14273
void PlayerBlizzconAssignCubePassive(ACDID idPlayer, LPCSTR szItemName, int32 nSlot);  // ScriptUtil.cpp:14311
void PlayerBlizzconGrantExperience(ACDID idPlayer);  // ScriptUtil.cpp:14334
void StartBountyConductors();  // ScriptUtil.cpp:14352
ACDID ActorSpawnActorInWorldOnRandomAdventure(LPCSTR szActorName, LPCSTR szWorldName, LPCSTR szAdventureName);  // ScriptUtil.cpp:14375
void HQCursedRealmShowRewardDialog(int32 nActIndex);  // ScriptUtil.cpp:14882
ACDID GetSetDungeonOwner();  // ScriptUtil.cpp:14933
void SetDungeonStatsRecordFailure(LPCTSTR szQuestKill, int32 nQuestKillMonsterCount, int32 nTimeTaken, int32 nTargetTime);  // ScriptUtil.cpp:15030
ACDID ActorGetPackLeader(VACDID idActor);  // ScriptUtil.cpp:10905
void ActorAddToPack(VACDID idPackMember, VACDID idActor);  // ScriptUtil.cpp:10928
void ActorRemoveFromPack(VACDID idActor);  // ScriptUtil.cpp:10957
void ActorSetPrimaryGroup(VACDID idActor, LPCSTR szGroupName);  // ScriptUtil.cpp:797
void ActorSetSecondaryGroup(VACDID idActor, LPCSTR szGroupName);  // ScriptUtil.cpp:809
void ActorSetTeam(ACDID idActor, Team eTeam);  // ScriptUtil.cpp:3319
Team ActorGetTeam(ACDID idActor);  // ScriptUtil.cpp:3336
void ActorEnable(VACDID idActor);  // ScriptUtil.cpp:3740
void ActorDisable(VACDID idActor);  // ScriptUtil.cpp:3747
BOOL ActorIsEnabled(VACDID idActor);  // ScriptUtil.cpp:3754
BOOL IsPlayer(VACDID idACD);  // ScriptUtil.cpp:9884
BOOL IsWreckableObject(VACDID idACD);  // ScriptUtil.cpp:9904
BOOL IsBurrowed(VACDID idACD);  // ScriptUtil.cpp:10536
BOOL ActorsInSameWorld(VACDID idACD1, VACDID idACD2);  // ScriptUtil.cpp:894
BOOL ActorsInSameLevelArea(VACDID idACD1, VACDID idACD2);  // ScriptUtil.cpp:916
LPCSTR ActorGetGender(VACDID idACD);  // ScriptUtil.cpp:5438
void ActorSetInvulnerable(VACDID idACD, BOOL bValue);  // ScriptUtil.cpp:2836
void ActorSetInvisible(VACDID idACD, BOOL bValue);  // ScriptUtil.cpp:2848
void ActorSetTargetable(VACDID idACD, BOOL bValue);  // ScriptUtil.cpp:2899
void ActorSetCollidable(VACDID idACD, BOOL bValue);  // ScriptUtil.cpp:2916
void ActorSetDamageState(VACDID idACD, int32 nValue);  // ScriptUtil.cpp:2924
void ActorIncrementDamageState(VACDID idACD);  // ScriptUtil.cpp:2932
void ActorRemoveStaggeredDamage(VACDID idActor);  // ScriptUtil.cpp:2939
void ActorDelete(VACDID idACD);  // ScriptUtil.cpp:2153
void ActorDeleteInventoryItem(VACDID idACDOwner, VACDID idACDItem);  // ScriptUtil.cpp:2174
int32 ActorGetLevel(ACDID idACD);  // ScriptUtil.cpp:3301
BOOL ActorIsAtLeastPlayerLevelCap(ACDID idACD, ACDID idACDPlayer);  // ScriptUtil.cpp:3308
void ActorCrossGamePowerOn(VACDID idActor, SNO snoPower);  // ScriptUtil.cpp:8920
void ActorCrossGamePowerOff(VACDID idActor, SNO snoPower);  // ScriptUtil.cpp:8931
int32 ActorGetPrimaryBonusAttribute(VACDID idActor);  // ScriptUtil.cpp:8942
int32 ActorGetPrimaryAttributeTotal(VACDID idActor);  // ScriptUtil.cpp:8962
void ActorSetWeaponClass(VACDID idActor, WeaponClass eWeaponClass);  // ScriptUtil.cpp:8982
void ActorWarpToActor(VACDID idACD, VACDID idACDTarget);  // ScriptUtil.cpp:3933
float ActorGetRadius(VACDID idACDActor);  // ScriptUtil.cpp:8453
void ActorSetQuestRange(VACDID idACD, LPCSTR szQuestRangeName);  // ScriptUtil.cpp:10784
void ActorSetDeletesInsteadOfInactive(VACDID idActor, BOOL bValue);  // ScriptUtil.cpp:10969
void ActorEnableAI(ACDID idACDActor, BOOL bValue);  // ScriptUtil.cpp:1775
void ActorSetAIState(VACDID idACDActor, AIStateType eState);  // ScriptUtil.cpp:3612
AIStateType ActorGetAIState(VACDID idACDActor);  // ScriptUtil.cpp:3620
void ActorSetAIBehavior(VACDID idACDActor, AIBehaviorType eBehavior);  // ScriptUtil.cpp:3627
void ActorClearAITether(VACDID idACDActor);  // ScriptUtil.cpp:3693
int32 ActorGetControlPointIndex(VACDID idActor);  // ScriptUtil.cpp:3835
void WarnActorAboutTarget(ACDID idACD, ACDID idACDTarget);  // ScriptUtil.cpp:9911
void ActorTryToFlee(VACDID idACD);  // ScriptUtil.cpp:9389
void ActorSetGuardObject(VACDID idACD, VACDID idACDGuardObject);  // ScriptUtil.cpp:9123
void ActorSetEnemyTarget(VACDID idACD, VACDID idACDEnemyTarget);  // ScriptUtil.cpp:9132
void ActorClearEnemyTarget(VACDID idACD);  // ScriptUtil.cpp:9151
BOOL ActorHasAITarget(VACDID idACD, VACDID idACDTarget);  // ScriptUtil.cpp:9159
void ActorDeactivateLure(VACDID idACD, BOOL bWantDeactivate);  // ScriptUtil.cpp:9142
BOOL ActorHasQueuedActions(VACDID idACD);  // ScriptUtil.cpp:7929
AIQueuedActionID QActorWalkToLocation(ACDID idACD, ACDID idLocACD);  // ScriptUtil.cpp:7994
AIQueuedActionID QActorTownWalkToLocation(ACDID idACD, ACDID idLocACD);  // ScriptUtil.cpp:8005
AIQueuedActionID QActorRunToLocation(ACDID idACD, ACDID idLocACD);  // ScriptUtil.cpp:8016
AIQueuedActionID QActorRunToLocationSpider(ACDID idACD, ACDID idLocACD);  // ScriptUtil.cpp:8027
AIQueuedActionID QActorSprintToLocation(ACDID idACD, ACDID idLocACD);  // ScriptUtil.cpp:8038
AIQueuedActionID QActorWalkToFrontOf(ACDID idACD, ACDID idTargetACD);  // ScriptUtil.cpp:8139
AIQueuedActionID QActorRunToFrontOf(ACDID idACD, ACDID idTargetACD);  // ScriptUtil.cpp:8150
AIQueuedActionID QActorSprintToFrontOf(ACDID idACD, ACDID idTargetACD);  // ScriptUtil.cpp:8161
AIQueuedActionID QActorInteract(VACDID idACD, VACDID idTargetACD);  // ScriptUtil.cpp:8172
AIQueuedActionID QActorInteractCrouching(VACDID idACD, VACDID idTargetACD);  // ScriptUtil.cpp:8183
AIQueuedActionID QActorAnim(VACDID idACD, ActorAnimationTag eAnimTag);  // ScriptUtil.cpp:8194
AIQueuedActionID QActorAnimAndNewIdle(VACDID idACD, ActorAnimationTag eAnimTag, ActorAnimationTag eIdleAnimTag);  // ScriptUtil.cpp:8212
AIQueuedActionID QActorFaceActor(VACDID idACD, VACDID idTargetACD);  // ScriptUtil.cpp:8269
AIQueuedActionID QActorFaceSameAsActor(VACDID idACD, VACDID idTargetACD);  // ScriptUtil.cpp:8277
AIQueuedActionID QActorDelete(VACDID idACD);  // ScriptUtil.cpp:8296
AIQueuedActionID QActorWaitForConversationFinished(VACDID idActor, LPCSTR szConversation);  // ScriptUtil.cpp:8350
AIQueuedActionID QActorEventSend(VACDID idActor, LPCSTR szEventName);  // ScriptUtil.cpp:8368
void QActorStopAllQueuedActions(ACDID idACD);  // ScriptUtil.cpp:8412
void ActorTriggerPowerAtFeet(ACDID idACD, LPCSTR szPower);  // ScriptUtil.cpp:9011
void ActorTriggerPowerAtTargetsFeet(ACDID idACD, ACDID idACDTarget, LPCSTR szPower);  // ScriptUtil.cpp:9034
void ActorTriggerBrainStancePowerAtTargetsFeet(ACDID idACD, ACDID idACDTarget, LPCSTR szPower);  // ScriptUtil.cpp:9059
void ActorCancelPowerBuff(VACDID idACD, LPCSTR szPower, int32 nBuffIndex);  // ScriptUtil.cpp:9365
void ActorLookAtActor(ACDID idACDSrc, ACDID idACDTarget);  // ScriptUtil.cpp:8422
void ActorSetIdleAnim(VACDID idACD, ActorAnimationTag eAnimTag);  // ScriptUtil.cpp:3701
void ActorPlaySound(ACDID idActor, LPCSTR szSoundName);  // ScriptUtil.cpp:1406
BOOL ActorPlayResourceGainedEffectGroup(VACDID idActor, ResourceType eResourceType);  // ScriptUtil.cpp:1594
void ActorRemoveComplexEffect(VACDID idActor, int32 idComplexEffect);  // ScriptUtil.cpp:1759
BOOL ActorComplexEffectIsValid(VACDID idActor, int32 idComplexEffect);  // ScriptUtil.cpp:1767
BOOL ActorResurrect(VACDID idCorpse, VACDID idSource);  // ScriptUtil.cpp:2018
void ActorSetNoDamage(ACDID idACD, BOOL bValue);  // ScriptUtil.cpp:2760
BOOL ActorGizmoSwitchIsOn(ACDID idACDGizmo);  // ScriptUtil.cpp:3563
BOOL ActorGizmoIsDoorOpen(ACDID idACDGizmo);  // ScriptUtil.cpp:3553
BOOL ActorGizmoIsDisabled(ACDID idACDGizmo);  // ScriptUtil.cpp:3573
void ActorGizmoSetDisabled(ACDID idACDGizmo, BOOL bDisabled);  // ScriptUtil.cpp:3583
void GateIncrementTriggeringCount(VACDID idACD);  // ScriptUtil.cpp:9943
void GateDecrementTriggeringCount(VACDID idACD);  // ScriptUtil.cpp:9950
double GizmoPlayAnim(VACDID idACDGizmo, ActorAnimationTag eAnimTag);  // ScriptUtil.cpp:9958
ACDID ActorGetFollowTarget(VACDID idACD);  // ScriptUtil.cpp:3011
void ActorSetFollowTarget(VACDID idACD, VACDID idACDToFollow, AIFollowType eFollowType);  // ScriptUtil.cpp:3025
void ActorResetFollowTarget(VACDID idACD);  // ScriptUtil.cpp:3043
void ActorDismissHireling(VACDID idHireling);  // ScriptUtil.cpp:3070
ACDID ActorGetPetOwner(VACDID idActor);  // ScriptUtil.cpp:3080
void ActorMakeFollowerPet(VACDID idACDPet, VACDID idACDPlayer, BOOL bForceSwitch);  // ScriptUtil.cpp:3087
void ActorRemoveFollowerPet(VACDID idACDPet);  // ScriptUtil.cpp:3131
void ActorMakeFollowerProxyPet(VACDID idACDPet, VACDID idACDPlayer);  // ScriptUtil.cpp:3145
BOOL PlayerHasUnlockedHireling(VACDID idPlayer, HirelingClass eHirelingClass);  // ScriptUtil.cpp:5474
void PlayerUnlockHireling(VACDID idPlayer, HirelingClass eHirelingClass);  // ScriptUtil.cpp:5482
ACDID PlayerGetCurrentHireling(VACDID idPlayer);  // ScriptUtil.cpp:5507
HirelingClass PlayerGetCurrentHirelingClass(VACDID idPlayer);  // ScriptUtil.cpp:5515
BOOL PlayerHirelingHasUnspentSkillPoints(VACDID idPlayer);  // ScriptUtil.cpp:5578
void AllPlayersLearnMonsterLore(VACDID idACDMonster);  // ScriptUtil.cpp:5816
int32 PlayerCountItemsInStash(VACDID idACDPlayer);  // ScriptUtil.cpp:5854
void PlayerSetCheckpoint(VACDID idACDPlayer, VACDID idACDTarget);  // ScriptUtil.cpp:6072
void PlayerAddNephalemBuffStack(VACDID idACDPlayer);  // ScriptUtil.cpp:6089
void PlayAnniversaryCutsceneForPlayer(VACDID idACDPlayer);  // ScriptUtil.cpp:6301
void WeatherTriggerLightning(VACDID idACDPlayer);  // ScriptUtil.cpp:9780
void CancelConversationsForPlayer(VACDID idACDPlayer);  // ScriptUtil.cpp:2288
void ActorSetConversationList(VACDID idACD, LPCSTR szConvList);  // ScriptUtil.cpp:3872
void ActorSetConversationStatus(ACDID idACD, BOOL bValue);  // ScriptUtil.cpp:3899
BOOL ActorGetConversationStatus(VACDID idACD);  // ScriptUtil.cpp:3915
void ActorClearCalloutCooldown(VACDID idACDNPC, LPCSTR szConversation);  // ScriptUtil.cpp:2542
BOOL ActorPlayRandomConversationFromList(LPCSTR szConversationList, ACDID idACDPlayer, ACDID idACDPrimary);  // ScriptUtil.cpp:2453
void SetBossBarTarget(VACDID idACD);  // ScriptUtil.cpp:10544
void ActorSetScriptActor(VACDID idACD, int32 nIndex, VACDID idACDTarget);  // ScriptUtil.cpp:9176
void ActorClearScriptActor(VACDID idACD, int32 nIndex);  // ScriptUtil.cpp:9187
ACDID ActorGetScriptActor(VACDID idACD, int32 nIndex);  // ScriptUtil.cpp:9197
void RegisterBountyActorAtPortalDestination(LPCSTR szActorName, VACDID idACDPortal);  // ScriptUtil.cpp:10713
void ActorGrantGold(VACDID idACD, int32 nAmount);  // ScriptUtil.cpp:3381
BOOL ActorHasBloodShards(VACDID idACD);  // ScriptUtil.cpp:3402
void ItemApplyDye(VACDID idItem, int32 nDye);  // ScriptUtil.cpp:4307
void UpgradeJewel(VACDID idJewel, int32 nUpgradeRanks);  // ScriptUtil.cpp:4355
void ItemSocketGem(VACDID idItem, VACDID idGem);  // ScriptUtil.cpp:4384
BOOL ItemMoveRuneToSkillSlot(VACDID idACDPlayer, VACDID idACDRune, LPCSTR szPower);  // ScriptUtil.cpp:4803
BOOL ItemMoveToStash(VACDID idACDPlayer, VACDID idACDItem);  // ScriptUtil.cpp:4831
BOOL ItemIsBroken(VACDID idACDItem);  // ScriptUtil.cpp:4866
BOOL WorldContainsLabel(VACDID idACD, LPCSTR szLabelName);  // ScriptUtil.cpp:6101
int32 WorldGetNumberOfHostileMonstersLeft(VACDID idACD);  // ScriptUtil.cpp:6168
void AchievementSendCursedChestCompletedEvent(VACDID idACDPlayer, int32 nNumKilled);  // ScriptUtil.cpp:11811
void SetKeyWardenKilled(VACDID idACDKeyWarden);  // ScriptUtil.cpp:12850
BOOL ActorIsInLootRun(VACDID idActor);  // ScriptUtil.cpp:13017
BOOL ActorIsInTieredLootRun(VACDID idActor);  // ScriptUtil.cpp:13046
BOOL ActorIsInPostBossTieredLootRun(VACDID idActor);  // ScriptUtil.cpp:13060
void GameRiftRecordDynamicPylonSpawn(VACDID idACDPylon);  // ScriptUtil.cpp:13578
BOOL GameEvaluateTieredLootRunBossFirstKill(VACDID idACDBoss, LPCSTR szTreasureClass, VACDID idACDPlayer);  // ScriptUtil.cpp:13974
void TreasureGoblinEscaped(VACDID idACDTreasureGoblin);  // ScriptUtil.cpp:14034
ACDID ActorSpawnActorInWorldOnClosestAdventure(LPCSTR szActorName, LPCSTR szWorldName, LPCSTR szAdventureName, VACDID idActorOrigin);  // ScriptUtil.cpp:14484
ACDID ActorSpawnActorInWorldOnFarthestAdventure(LPCSTR szActorName, LPCSTR szWorldName, LPCSTR szAdventureName, VACDID idActorOrigin);  // ScriptUtil.cpp:14608
int32 ActorGetHQCursedRealmActIndex(VACDID idActor);  // ScriptUtil.cpp:14744
int32 ActorGetPlayerIndex(VACDID idACDPlayer);  // ScriptUtil.cpp:14808
void PlayerModifyHQCursedRealmReagentsCollected(VACDID idPlayerActor, int32 nChange);  // ScriptUtil.cpp:14836
BOOL PlayerFlagIsSet(ACDID idACDPlayer, LPCSTR szFlagLabel);  // ScriptUtil.cpp:5303
void PlayerFlagSet(ACDID idACDPlayer, LPCSTR szFlagLabel, BOOL bValue);  // ScriptUtil.cpp:5318
void PlayerSetDungeonLoreFlagSet(VPACDID idACDPlayer, int32 nSetDugneonLoreIndex, BOOL bValue);  // ScriptUtil.cpp:5339
BOOL PlayerSetDungeonLoreFlagGet(VPACDID idACDPlayer, int32 nSetDugneonLoreIndex);  // ScriptUtil.cpp:5364
LPCSTR PlayerGetClass(ACDID idACDPlayer);  // ScriptUtil.cpp:5391
PlayerClass PlayerGetClassEnum(ACDID idACDPlayer);  // ScriptUtil.cpp:5402
LPCSTR PlayerGetHeroName(ACDID idACDPlayer);  // ScriptUtil.cpp:5409
LPCSTR PlayerGetGender(VPACDID idACDPlayer);  // ScriptUtil.cpp:5424
int32 PlayerGetPlayerKillCount(VPACDID idACDPlayer);  // ScriptUtil.cpp:5461
void PlayerLearnSkill(VPACDID idACDPlayer, LPCSTR szPower);  // ScriptUtil.cpp:5587
void PlayerLearnLore(VPACDID idACDPlayer, LPCSTR szLore);  // ScriptUtil.cpp:5756
BOOL PlayerHasLearnedLore(VPACDID idACDPlayer, LPCSTR szLore);  // ScriptUtil.cpp:5777
void PlayerGrantExperience(ACDID idACDPlayer, uint32 nXPAmount);  // ScriptUtil.cpp:5885
void PlayerTagMonsterForXP(VPACDID idACDPlayer, ACDID idACDMonster);  // ScriptUtil.cpp:5897
void PlayerWarpToWaypoint(VPACDID idACDPlayer, int nWayPointNumber);  // ScriptUtil.cpp:5914
void PlayerUseItem(VPACDID idACDPlayer, VACDID idACDItem);  // ScriptUtil.cpp:5996
float PlayerGetAFKTime(VPACDID idACDPlayer);  // ScriptUtil.cpp:6011
float PlayerGetInactionTime(VPACDID idACDPlayer);  // ScriptUtil.cpp:6018
int32 PlayerGetLevelCap(VPACDID idACDPlayer);  // ScriptUtil.cpp:6025
void ItemRemoveFromPlayer(ACDID idACDPlayer, LPCSTR szItemName);  // ScriptUtil.cpp:3953
void ItemRemoveMultipleFromPlayer(VACDID idACDPlayer, LPCSTR szItemName, int32 nCount);  // ScriptUtil.cpp:3961
int32 ItemGetCount(VACDID idACDPlayer, LPCSTR szItemName);  // ScriptUtil.cpp:4006
void ItemGiveTierToPlayer(VPACDID idACDPlayer, int32 nTier);  // ScriptUtil.cpp:4132
void PVPPlayerSetDueling(VPACDID idACDPlayer);  // ScriptUtil.cpp:6766
BOOL ActorsAreFriendly(VACDID idActor1, VACDID idActor2, BOOL bCharmedAlliesAreEnemies);  // ScriptUtil.cpp:3369
double ActorDistanceBetweenActors(VACDID idACDFrom, VACDID idACDTo, BOOL bWantAs2D);  // ScriptUtil.cpp:517
double ActorDistanceSquaredBetweenActors(VACDID idACDFrom, VACDID idACDTo, BOOL bWantAs2D);  // ScriptUtil.cpp:540
void ActorSetAttribute(VACDID idActor, double dValue, int32 eAttrib, int32 nParam);  // ScriptUtil.cpp:8891
void ActorAddAITarget(VACDID idACDActor, VACDID idACDTarget, BOOL bForcedTarget);  // ScriptUtil.cpp:3650
ACDID ActorGetAIPrimaryTarget(VACDID idACDActor, BOOL bActiveEnemiesOnly);  // ScriptUtil.cpp:3679
AIQueuedActionID QActorKill(VACDID idACD, DeathType eDeathType, BOOL bCrit);  // ScriptUtil.cpp:8311
AIQueuedActionID QActorPower(VACDID idACD, LPCSTR szPower, VACDID idACDTarget);  // ScriptUtil.cpp:8320
ACDID ActorSpawnMonster(LPCSTR szMonsterName, ACDID idACDLocation, LPCSTR szSpawnAnim, BOOL bRandomOrientation, int nSkipDistMin, int nSkipDistDelta, BOOL bUseContactOffset);  // ScriptUtil.cpp:8702
void ActorTriggerPowerAtTarget(ACDID idACD, ACDID idACDTarget, LPCSTR szPower, BOOL bForceTarget);  // ScriptUtil.cpp:9080
int32 ActorPlayEffectGroup(VACDID idActor, LPCSTR szEffectGroupName, BOOL bUseComplexEffect, ACDID idDestinationPlayer);  // ScriptUtil.cpp:1442
int32 ActorPlayEffectGroupActorToActor(VACDID idActorFrom, VACDID idActorTo, LPCSTR szEffectGroupName, BOOL bUseComplexEffect);  // ScriptUtil.cpp:1665
void ActorKill(ACDID idACD, DeathType eDeathType, BOOL bCrit, BOOL bDropLoot, ACDID idACDKiller);  // ScriptUtil.cpp:1980
void ActorGizmoOperate(ACDID idACDGizmo, ACDID idACDOperator);  // ScriptUtil.cpp:3527
ACDID ActorMakeHireling(VACDID idPlayer, VACDID idActorPlace, HirelingClass eHirelingClass, BOOL bShowPrompt);  // ScriptUtil.cpp:3051
void CameraScriptedSequenceStart(ACDID idACDFocusActor, BOOL bHidePlayers, BOOL bDisableAutoCancellation);  // ScriptUtil.cpp:11030
void SetCameraObserver(ACDID idACDPlayer, LPCSTR szObserverName, ACDID idACDToOrbit);  // ScriptUtil.cpp:11269
BOOL ActorPlayConversation(LPCSTR szConversation, ACDID idACDPlayer, ACDID idACDPrimary, ACDID idACDAlt1, ACDID idACDAlt2, ACDID idACDAlt3, ACDID idACDAlt4, BOOL bForceInterrupt);  // ScriptUtil.cpp:2217
BOOL ActorPlaySimpleConversation(LPCSTR szConversation, ACDID idACDPrimary, ACDID idACDAlt1, ACDID idACDAlt2, ACDID idACDAlt3, ACDID idACDAlt4, BOOL bForceInterrupt);  // ScriptUtil.cpp:2260
void PlayerConversationSetHeard(LPCSTR szConversation, BOOL bHeard, ACDID idACDPlayer);  // ScriptUtil.cpp:2660
void PlayerConversationListSetHeard(LPCSTR szConversationList, BOOL bHeard, ACDID idACDPlayer);  // ScriptUtil.cpp:2704
void EventSend(LPCSTR szEventName, ACDID idACD, int32 nUserParam);  // ScriptUtil.cpp:6681
void EventSendToActor(LPCSTR szEventName, ACDID idACDSource, ACDID idACDTarget, int32 nUserParam);  // ScriptUtil.cpp:6700
BOOL QuestIsAssigned(LPCSTR szQuestName, ACDID idPlayer);  // ScriptUtil.cpp:10553
BOOL QuestIsActive(LPCSTR szQuestName, ACDID idPlayer);  // ScriptUtil.cpp:10578
BOOL QuestStepIsCurrent(LPCSTR szQuestName, LPCSTR szStepName_VariableArray, ACDID idPlayer);  // ScriptUtil.cpp:10628
void QuestLoad(LPCSTR szQuestName, LPCSTR szLevelArea);  // ScriptUtil.cpp:10199
void QuestSendDynamicObjectiveReward(uint32 nGoldTier, uint32 nXPTier, LPCSTR szItemName);  // ScriptUtil.cpp:10404
BOOL ActorHasItemByName(VACDID idACDOwner, LPCSTR szItemName, BOOL bAllowStash, BOOL bEquippedOnly);  // ScriptUtil.cpp:3487
void ActorRemoveItemByName(ACDID idActor, LPCSTR szItemName, BOOL bPreferEquipped);  // ScriptUtil.cpp:3514
ACDID ItemGiveToPlayer(ACDID idACDPlayer, LPCSTR szItemName, LPCSTR szAffix1, LPCSTR szAffix2, LPCSTR szAffix3, LPCSTR szAffix4, LPCSTR szAffix5, LPCSTR szAffix6);  // ScriptUtil.cpp:4159
ACDID ItemGiveToPlayerBlizzcon(ACDID idACDPlayer, LPCSTR szItemName, LPCSTR szAffix1, LPCSTR szAffix2, LPCSTR szAffix3, LPCSTR szAffix4, LPCSTR szAffix5, LPCSTR szAffix6);  // ScriptUtil.cpp:4173
ACDID ItemGiveRewardToPlayer(ACDID idACDPlayer, LPCSTR szItemName, LPCSTR szAffix1, LPCSTR szAffix2, LPCSTR szAffix3, LPCSTR szAffix4, LPCSTR szAffix5, LPCSTR szAffix6);  // ScriptUtil.cpp:4187
ACDID ItemGiveToPlayerStash(ACDID idACDPlayer, LPCSTR szItemName, LPCSTR szAffix1, LPCSTR szAffix2, LPCSTR szAffix3, LPCSTR szAffix4, LPCSTR szAffix5, LPCSTR szAffix6);  // ScriptUtil.cpp:4201
VACDID ItemGiveBestTreasureToPlayer(VPACDID idACDPlayer, LPCSTR szTreasureClass, int nGenerations, int32 nItemLevel, LPCSTR szFuncGetItemWeight, LPCSTR szParam1_VariableArray, LPCSTR szParam2_VariableArray);  // ScriptUtil.cpp:4673
void DisplayGameMessage(VACDID idACDPlayer, LPCSTR szStringDecoratedLabel, int nParam1, int nParam2);  // ScriptUtil.cpp:7876
void DisplayGameMessageForAllPlayers(LPCSTR szStringDecoratedLabel, int nParam1, int nParam2);  // ScriptUtil.cpp:7892
void DisplayChatGameMessage(VACDID idACDPlayer, LPCSTR szStringDecoratedLabel, int nParam1, int nParam2);  // ScriptUtil.cpp:7904
void DisplayChatGameMessageForAllPlayers(LPCSTR szStringDecoratedLabel, int nParam1, int nParam2);  // ScriptUtil.cpp:7917
void PVPTeamAddScore(Team eTeam, uint32 points, ACDID idACDScorer);  // ScriptUtil.cpp:7116
BOOL WorldHasScene(LPCSTR szWorldName, LPCSTR szSceneName, LPCSTR szLabel);  // ScriptUtil.cpp:5110
void PreLoadSNO(LPCSTR szGroupName, LPCSTR szSNOName, BOOL bWriteable);  // ScriptUtil.cpp:5225
void AchievementSendEvent(VACDID idActorPlayer, LPCSTR szAchievementEvent, int32 uQuantity);  // ScriptUtil.cpp:11751
void AchievementSendSetDungeonEvent(VACDID idActorPlayer, LPCSTR szAchievementEvent, int32 uQuantity);  // ScriptUtil.cpp:11784
BOOL ShowTutorial(const char * szTutorialName, const ACDID idACDPlayer, const char * szUIAnchorName, const ACDID idACDTarget);  // ScriptUtil.cpp:11494
void TestingAddWorld(LPCSTR szWorld, Act eAct, LPCSTR szQuest, LPCSTR szStep);  // ScriptUtil.cpp:9545
BOOL PlayerPickUpItem(ACDID idACDPlayer, ACDID idACDItem, BOOL bPlayEffects);  // ScriptUtil.cpp:12966
void QuestShowTieredRiftReward(BOOL bFinishedInTime, uint32 nRiftDepth, uint32 nGoldTier, uint32 nXPTier, LPCSTR szItemName, ACDID idACDSpecificPlayer, int32 nItemStackCount);  // ScriptUtil.cpp:13700
void SetDungeonComplete(LPCSTR szQuestKill, int32 nQuestKillMonsterCount, LPCSTR szQuestBonus1, BOOL bQuestBonus1Success, LPCSTR szQuestBonus2, BOOL bQuestBonus2Success, LPCSTR szQuestMastery, BOOL bQuestMasterySuccess, int32 nTimeTaken, int32 nTargetTime, BOOL bQuestKillSuccess, BOOL bDisplayTotalTime);  // ScriptUtil.cpp:14951

#include "../../Common/ActorCommonData.h"

#include "../../Common/GameBalance.h"
ACDID ActorGetItemInInventorySlot(ACDID idActor, InventorySlotType eSlot);  // ScriptUtil.cpp:3389
BOOL ItemHasLabel(ACDID idACD, InventorySlotType eSlotType, int32 eLabel);  // ScriptUtil.cpp:4997
MonsterRarity ActorGetMonsterRarity(ACDID idActor);  // ScriptUtil.cpp:1124
GBID GetGBIDFromString(LPCSTR szString);  // ScriptUtil.cpp:11971
void MonsterRemoveAffixFromSelection(GBID gbidAffix);  // ScriptUtil.cpp:11989
void MonsterSetAffixFrequency(GBID gbidAffix, int32 nNewFrequencyWeight);  // ScriptUtil.cpp:12022
int32 MonsterGetAffixFrequency(GBID gbidAffix);  // ScriptUtil.cpp:12058
void ActorFadeAndDelete(VACDID idACD, float flFadeTime);  // ScriptUtil.cpp:2160
void ActorSetHitPointsPercent(ACDID idActor, float flPercent);  // ScriptUtil.cpp:8545
void ActorSetHitPoints(ACDID idActor, float flHP);  // ScriptUtil.cpp:8558
void ActorSetHitPointsMax(ACDID idActor, float flHPMax);  // ScriptUtil.cpp:8571
void ActorMergeAIState(VACDID idACDSource, LPCSTR szAIStateSNOToMerge, VACDID idACDToMergeOnto, float flDurationInSeconds);  // ScriptUtil.cpp:9343
AIQueuedActionID QActorMoveToLocationWithinTime(ACDID idACD, ACDID idLocACD, ActorAnimationTag eAnimTag, float flSeconds);  // ScriptUtil.cpp:8049
AIQueuedActionID QActorMoveToLocation(ACDID idACD, ACDID idLocACD, ActorAnimationTag eAnimTag, float flSpeed);  // ScriptUtil.cpp:8080
AIQueuedActionID QActorMoveToFrontOf(ACDID idACD, ACDID idLocACD, ActorAnimationTag eAnimTag, float flSpeed, float flDistInFrontOfTarget);  // ScriptUtil.cpp:8107
AIQueuedActionID QActorAnimLoop(VACDID idACD, ActorAnimationTag eAnimTag, float flTime);  // ScriptUtil.cpp:8241
AIQueuedActionID QActorFadeAndDelete(VACDID idACD, float flFadeTime);  // ScriptUtil.cpp:8285
AIQueuedActionID QActorWalkForward(VACDID idACD, float flDistance);  // ScriptUtil.cpp:8303
AIQueuedActionID QActorWait(VACDID idActor, float flTime);  // ScriptUtil.cpp:8342
AIQueuedActionID QActorEventSendInRange(VACDID idActor, LPCSTR szEventName, float flRange);  // ScriptUtil.cpp:8382
void ActorBroadcastLookAt(ACDID idACDSrc, float flIntensity, float flRadius);  // ScriptUtil.cpp:8430
void ActorPlaySink(ACDID idActor, float flTimeToSink);  // ScriptUtil.cpp:1426
void ActorDamageByPercent(VACDID idACD, float flPercent);  // ScriptUtil.cpp:1958
void ActorSetDamageCap(ACDID idACD, float flUnitPercent);  // ScriptUtil.cpp:2802
ACDID ActorGetNearbyFollower(VACDID idACDOrigin, float flRangeSq);  // ScriptUtil.cpp:3003
void BossZoom(float flZoom, ACDID idACDPlayer, float flZoomSpeed);  // ScriptUtil.cpp:11377
void WeatherIntensityOverride(LPCSTR szWorld, const float flIntensity, const float flOverrideDuration);  // ScriptUtil.cpp:9698
void ActorSetCalloutCooldown(VACDID idACDNPC, LPCSTR szConversation, float flCooldown);  // ScriptUtil.cpp:2523
void ActorSilenceCallouts(VACDID idACDNPC, float flDuration);  // ScriptUtil.cpp:2493
void ActorSilenceBanter(VACDID idACDPlayer, float flDuration);  // ScriptUtil.cpp:2508
void TimerModifyExpirationTime(LPCSTR szSNOName, float flSeconds);  // ScriptUtil.cpp:10181
BOOL TriggerTrialEnd(float flDelayInSeconds);  // ScriptUtil.cpp:7733
void PVPPlayerSetOnDeathAccolade(ACDID idACDPlayer, LPCSTR szAccolade, float flValue);  // ScriptUtil.cpp:7219
void PVPPlayerSetTeamRating(ACDID idACDPlayer, float flValue);  // ScriptUtil.cpp:7611
void PVPPlayerSetTeamVariance(ACDID idACDPlayer, float flValue);  // ScriptUtil.cpp:7630
void PVPPlayerSetIndividualRating(ACDID idACDPlayer, float flValue);  // ScriptUtil.cpp:7649
void PVPPlayerSetIndividualVariance(ACDID idACDPlayer, float flValue);  // ScriptUtil.cpp:7668
void DungeonFinderClosingTriggered(float flSeconds);  // ScriptUtil.cpp:12650
void SetDungeonClosingTriggered(float flSeconds);  // ScriptUtil.cpp:12663
void GreedRealmClosingTriggered(float flSeconds);  // ScriptUtil.cpp:12677
void GameUpdateRiftStats(int32 nRiftLevel, float flTimeElapsed, BOOL bRiftSuccess);  // ScriptUtil.cpp:13235
ACDID ActorFindNearestActor(VACDID idACD, float flRadius, BOOL bIgnoreActorsWithAIMergesByOthers, BOOL bIgnoreLOS, LPCSTR szActorSNOFilter0, LPCSTR szActorSNOFilter1, LPCSTR szActorSNOFilter2, LPCSTR szActorSNOFilter3);  // ScriptUtil.cpp:9214
ACDID ActorGetClosestPlayer(VACDID idACD, float flRangeSq);  // ScriptUtil.cpp:2950
void ActorAddResource(VACDID idACDActor, float flAmount, BOOL bPrimary, BOOL bApplyOnHitBonuses);  // ScriptUtil.cpp:8441
void ActorSetAITether(VACDID idACDActor, float flRadius, ACDID idACDTether);  // ScriptUtil.cpp:3659
void ActorHeal(VACDID idACD, float flValue, HealType eHealType, SNO snoSource, ACDID idACDSource, BOOL bPlayEffect);  // ScriptUtil.cpp:1837
double ActorGetAnimContactFrameTime(VACDID idACDActor, ActorAnimationTag eAnimTag, int32 nContactFrame, float flSpeedScale);  // ScriptUtil.cpp:9980
void CameraFocusOnActor(VACDID idActor, BOOL bSnap, float flDuration);  // ScriptUtil.cpp:11203
void CameraSetYaw(float flAngle, BOOL bSnap, float flDuration);  // ScriptUtil.cpp:11399
void CameraSetZoom(float flZoom, BOOL bSnap, float flDuration);  // ScriptUtil.cpp:11252
void MusicPlay(LPCSTR szMusic, ACDID idActor, float flMaxDistance);  // ScriptUtil.cpp:6314
void ActorPlayCallout(LPCSTR szConversation, ACDID idACDPlayer, VACDID idACDNPC, int32 nPriority, float flDelay, float flMaxQueuedTime, float flCooldown, float flGlobalCooldown, BOOL bIgnoreGCD);  // ScriptUtil.cpp:2299
BOOL ActorPlayRandomBanterFromList(LPCSTR szConversationList, ACDID idACDPlayer, ACDID idACDPrimary, int32 nPriority, float flCooldown, float flGlobalCooldown);  // ScriptUtil.cpp:2341
void EventSendInRange(LPCSTR szEventName, ACDID idACD, float flRange, int32 nUserParam);  // ScriptUtil.cpp:6690
void ItemDropAtActor(VACDID idACD, LPCSTR szItemName, BOOL bDropForActor, ACDID idACDDropLocation, float flSpawnYawAngle, float flSpawnDist);  // ScriptUtil.cpp:4608
void PVPPlayerSetAccolade(ACDID idACDPlayer, int32 nIndex, LPCSTR szAccolade, float flValue);  // ScriptUtil.cpp:7238
int32 MinimapAddPing(ACDID idActor, float flDuration, LPCSTR szTexture, ACDID idPlayer, BOOL bCanBecomeArrow);  // ScriptUtil.cpp:11415
void DungeonFinderAddLevel(LPCSTR szWorld, LPCSTR szPopulation, float flSpawnGroupDensityMultiplier, float flEliteSpawnGroupDensityMultipler, float flGizmoDensityMultiplier, LPCSTR szWeather);  // ScriptUtil.cpp:12547
CRefString ActorGetPrimaryGroup(VACDID idActor);  // ScriptUtil.cpp:847
CRefString ActorGetSecondaryGroup(VACDID idActor);  // ScriptUtil.cpp:855
CRefString ActorGetScene(VACDID idActor);  // ScriptUtil.cpp:866
CRefString ActorGetWorld(VACDID idActor);  // ScriptUtil.cpp:877
CRefString ActorGetLevelArea(VACDID idActor);  // ScriptUtil.cpp:886
CRefString ActorGetSNOName(ACDID idACD);  // ScriptUtil.cpp:3423
CRefString ActorGetRandomSummonActor(VACDID idACDSummoner);  // ScriptUtil.cpp:8786
CRefString PlayerGetSkillInSlot(ACDID idPlayer, int32 nSlot);  // ScriptUtil.cpp:5662
CRefString ItemGBIDToString(int32 nGBID);  // ScriptUtil.cpp:11979
CRefString DungeonFinderGetWorldTilesetForActor(ACDID idActor);  // ScriptUtil.cpp:12715
CRefString DungeonFinderGetEntrancePortalLevelArea();  // ScriptUtil.cpp:12763
CRefString ActorGetNameFromSNO(SNO snoActor);  // ScriptUtil.cpp:14732

#include "../../Common/NodePathSwig.h"
#include "LuaScript.h"
CRefString ActorGetNodePathEndNodeName(VACDID idActor, PathClass ePathClass);  // ScriptUtil.cpp:3863
ActorIDArray * ActorsLinkedByActor(ACDID idACD);  // ScriptUtil.cpp:369
ActorIDArray * ActorsLinkedByActorInGroup(ACDID idACD, const char * szGroupName);  // ScriptUtil.cpp:428
ActorIDArray * PlayerActorsOnTeam(Team eTeam);  // ScriptUtil.cpp:560
ActorIDArray * PlayerActors();  // ScriptUtil.cpp:592
ActorIDArray * PlayerActorsLocalToPlayer(VACDID idActor);  // ScriptUtil.cpp:624
ActorIDArray * PlayersInTieredRift();  // ScriptUtil.cpp:664
ActorIDArray * ActorsInGroup(LPCSTR szGroupName);  // ScriptUtil.cpp:697
ActorIDArray * ActorGetPackMembers(VACDID idActor);  // ScriptUtil.cpp:10869
ActorIDArray * ActorsEnableLinkedByActor(ACDID idActor);  // ScriptUtil.cpp:376
ActorIDArray * ActorsEnableLinkedByActorInGroup(ACDID idACD, const char * szGroupName);  // ScriptUtil.cpp:436
ActorIDArray * PlayerGetFollowers(VACDID idPlayer);  // ScriptUtil.cpp:5530
ActorIDArray * ItemGiveTreasureToPlayer(VPACDID idACDPlayer, LPCSTR szTreasureClass);  // ScriptUtil.cpp:4215
ActorIDArray * ActorsNearActor(VACDID idACD, float flRadius, LPCSTR szActorSNOFilter0, LPCSTR szActorSNOFilter1, LPCSTR szActorSNOFilter2, LPCSTR szActorSNOFilter3);  // ScriptUtil.cpp:444
ActorIDArray * ActorsInTriggerBounds(ACDID idTrigger, BOOL bPowerTargetsOnly);  // ScriptUtil.cpp:751
ActorIDArray * TreasureClassDropAtActor(VACDID idACD, LPCSTR szTreasureClass, BOOL bEnforceLootRules, float flSpawnYawAngle, float flSpawnDist);  // ScriptUtil.cpp:4420
ActorIDArray * TreasureClassDropAtActorForPlayer(VACDID idACD, LPCSTR szTreasureClass, VACDID idACDPlayer, float flSpawnYawAngle, float flSpawnDist);  // ScriptUtil.cpp:4526
ActorIDArray * GameGetOpenWorldBossSpawnMarkerLocationsNearActor(ACDID idACDRef, int32 nNumLocations, float flMinDist, float flMaxDist, LPCSTR szLevelArea);  // ScriptUtil.cpp:6938
ACDID ActorItemGetOwner(VACDID idItemActor, BOOL bUnk); // ScriptUtil.cpp:2203 (2.7.7: second argument passed through to ItemGetAssignedACDID)
blz::vector<CRefString, blz::allocator<CRefString> > ActorFindMonsterSNOsInSameLevelArea(VACDID idACD); // ScriptUtil.cpp:9301
void DungeonFinderCreate(LPCSTR szDungeonFinder); // ScriptUtil.cpp:12525
void DungeonFinderCreate(SNO snoDungeonFinderEngine); // ScriptUtil.cpp:12539

#include "../../Common/Actors/ActorEffectSwig.h"
#include "../../Common/World.h"
#include "../SGameStatsSwig.h"
int32 ActorGetGemCount(VACDID idACD, GemType eType, InventorySlotType eSlot);  // ScriptUtil.cpp:12284
int32 ActorGetSpecificGemCount(VACDID idACD, LPCSTR szGemName, InventorySlotType eSlot);  // ScriptUtil.cpp:12407
int32 ActorGetLegendaryGemCount(VACDID idACD, InventorySlotType eSlot);  // ScriptUtil.cpp:12344
ACDID ActorSpawnActor(LPCSTR szActorName, ACDID idACDLocation, LPCSTR szSpawnAnim, BOOL bRandomOrientation, int nSkipDistMin, int nSkipDistMax, BOOL bUseContactOffset, MonsterRarity eMonsterRarity);  // ScriptUtil.cpp:8715
int32 ActorPlayEffectGroupRopeActorToActor(VACDID idActorFrom, RopePosition ePositionFrom, VACDID idActorTo, RopePosition ePositionTo, LPCSTR szRopeName, BOOL bUseComplexEffect, BOOL bIgnoreOwnerAlpha);  // ScriptUtil.cpp:1707
void PlayerWarpToWorld(VPACDID idActorPlayer, LPCSTR szWorldName, LPCSTR szEntranceName, float flWarpFadeInSecs, PlayerWarpReason eReason);  // ScriptUtil.cpp:5948
void CameraTriggerFadeToBlack(BOOL bStayFaded);  // ScriptUtil.cpp:10995
void CameraFocusClear(BOOL bInstant, float flDuration);  // ScriptUtil.cpp:11237
CRefString ReferenceEffectGroup(LPCSTR szEffectGroup, ScriptReferenceType eRefType);  // ScriptUtil.cpp:11595
CRefString ReferenceActor(LPCSTR szActor, ScriptReferenceType eRefType);  // ScriptUtil.cpp:11550
CRefString ReferenceParticle(LPCSTR szParticle, ScriptReferenceType eRefType);  // ScriptUtil.cpp:11559
CRefString ReferencePower(LPCSTR szPower, ScriptReferenceType eRefType);  // ScriptUtil.cpp:11568
CRefString ReferenceRecipe(LPCSTR szRecipe, ScriptReferenceType eRefType);  // ScriptUtil.cpp:11577
CRefString ReferenceRope(LPCSTR szRope, ScriptReferenceType eRefType);  // ScriptUtil.cpp:11586
CRefString ReferenceConversation(LPCSTR szConversation, ScriptReferenceType eRefType);  // ScriptUtil.cpp:11604
CRefString ReferenceConversationList(LPCSTR szConversationList, ScriptReferenceType eRefType);  // ScriptUtil.cpp:11613
CRefString ReferenceTreasureClass(LPCSTR szTreasureClassName, ScriptReferenceType eRefType);  // ScriptUtil.cpp:11622
CRefString ReferenceQuestRange(LPCSTR szQuestRangeName, ScriptReferenceType eRefType);  // ScriptUtil.cpp:11649
CRefString ReferenceAccolade(LPCSTR szAccoladeName, ScriptReferenceType eRefType);  // ScriptUtil.cpp:11631
CRefString ReferenceConductor(LPCSTR szConductorName, ScriptReferenceType eRefType);  // ScriptUtil.cpp:11640
CRefString ReferenceQuest(LPCSTR szQuestName, ScriptReferenceType eRefType);  // ScriptUtil.cpp:11658
CRefString ReferenceWorld(LPCSTR szWorldName, ScriptReferenceType eRefType);  // ScriptUtil.cpp:11667
CRefString ReferenceDungeonFinder(LPCSTR szDungeonFinderName, ScriptReferenceType eRefType);  // ScriptUtil.cpp:11676
double PlayerStatCounterGet(ACDID idACDPlayer, StatCounterType eType, int32 nParam0, int32 nParam1);  // ScriptUtil.cpp:11837
double PlayerCurrentLifeStatCounterGet(ACDID idACDPlayer, StatCounterType eType, int32 nParam0, int32 nParam1);  // ScriptUtil.cpp:11860
ActorIDArray * PlayerStatCounterGetHighestPlayers(StatCounterType eType, int32 nParam0, int32 nParam1, Team eTeam);  // ScriptUtil.cpp:11883
ActorIDArray * PlayerStatCounterGetLowestPlayers(StatCounterType eType, int32 nParam0, int32 nParam1, Team eTeam);  // ScriptUtil.cpp:11927
void ActorSetNodePath(VACDID idACD, PathClass ePathClass, LPCSTR szTargetNodeName); // ScriptUtil.cpp:3767
PathClass ActorGetNodePathClass(VACDID idActor); // ScriptUtil.cpp:3807

ACDID ActorIDFromName(LPCSTR szInstanceName);  // X/Server/LuaScript/ScriptUtil.cpp:178

ACDID ActorIDFromName(LPCSTR szInstanceName, VACDID idACDNeighbor);  // X/Server/LuaScript/ScriptUtil.cpp:253

double ActorGetAttribute(VACDID idActor, LPCSTR szAttributeName, LPCSTR szAttribParamName);  // X/Server/LuaScript/ScriptUtil.cpp:8812

double ActorGetAttribute(VACDID idActor, int32 eAttrib, int32 nParam);  // X/Server/LuaScript/ScriptUtil.cpp:8859

double ItemGetAttributeFromInventorySlot(ACDID idACD, InventorySlotType eSlotType, LPCSTR szAttributeName, LPCSTR szAttribParamName);  // X/Server/LuaScript/ScriptUtil.cpp:4882

double ItemGetAttributeFromInventorySlot(ACDID idACD, InventorySlotType eSlotType, int32 eAttrib, int32 nParam);  // X/Server/LuaScript/ScriptUtil.cpp:4914
