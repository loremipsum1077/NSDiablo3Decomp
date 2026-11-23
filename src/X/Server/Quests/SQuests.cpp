// Original file: X/Server/Quests/SQuests.cpp
#include "../../Common/Quests/Quests.h"
#include "SQuests.h"
#include "../../Common/Game.h"
#include "../../Common/GameCommonData.h"
#include "../../Common/Player.h"
#include "../../Common/ActorCommonData.h"
#include "../../Common/Definitions/QuestDefinition.h"
#include "../../Common/Networking/GameMessage.h"
#include "../PlayerList.h"
#include "../Map/Minimap.h"
#include "../../Common/SceneCommon.h"
#include "../../Sigma/ErrorManager/ErrorManager.h"
#include "../../Sigma/Math/MathGeneral.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../SGame.h"
#include "../SGameConstants.h"
#include "../../Common/Achievements/AchievementDefines.h"
#include "../../Common/Protobuf/Quest.pb.h"
#include "../../Common/Definitions/GlobalDefinition.h"
#include "../Actors/Actors.h"
#include "../../Common/World.h"
#include "../../Common/Items.h"
#include "../SGameStats.h"
#include "../SWorld.h"
#include "../SScene.h"
#include "../System/LevelArea.h"
#include "../../Sigma/DataStructures/XOptional.h"

struct QuestNotificationMessage;
BOOL SQuestsProcessNotification(QuestNotificationMessage & tMessage);  // SQuests.cpp:4144
void SQuestsSendQuestStateToPlayer(QuestInstance & tQuest, const Player & tPlayer, const BOOL bDisplayButton);  // SQuests.cpp:5201
void SQuestsSendUpdateToAllPlayers(SNO snoQuest);  // SQuests.cpp:5177
void QuestsRefreshQuestDialogs(int32 nPlayerIndex);  // Quests.cpp
void QuestsForceSystemUpdate();  // Quests.cpp
void SPlayerRefreshWaypointDestination(ACDID idACDPlayer);  // SPlayer.cpp
QuestInstance * GetLeadingMainLineQuest();  // Quests.cpp:1020
CRefString QuestStepGetName(SNO snoQuest, int32 nStepUID);  // Quests.cpp:1300
BOOL QuestStepTrackPerPlayer(SNO snoQuest, int32 nStepUID);  // Quests.cpp:1226
void ScriptEventOnQuestObjectiveCompleted(LPCSTR szQuest, LPCSTR szStep, int32 nObjectiveIndex);  // ScriptEvent.cpp
void SQuestsCompleteMainlineUpToQuest(SNO snoQuest);  // SQuests.cpp:940
void SQuestHandleQuestAdvancement(QuestInstance & tQuestInstance, int32 nCompletedStepUID, int32 nStepUID, BOOL bStartup);  // SQuests.cpp:3012
void SQuestsSyncPlayerState(QuestInstance & tQuestInstance, Player * ptPlayerToSync);  // SQuests.cpp:4356
BOOL QuestIsComplete(const QuestInstance & tQuest);  // Quests.cpp:995
BOOL QuestStepIsSelectable(SNO snoQuest, int32 nStepUID);  // Quests.cpp:1167
BOOL sSQuestsInstanceIsTargetingActor(QuestInstance & tQuestInstance, ACDID idActor);  // SQuests.cpp:1007
BOOL sSQuestsInstanceHasItem(QuestInstance & tQuestInstance, GBID gbidItemRequired);  // SQuests.cpp:1168

#include "../Achievements/AchievementModifierEvaluationLogic.h"
namespace Achievement {
typedef int32 EventID;











}  // namespace Achievement
PlayerSavedQuest * PlayerGetSavedQuest(Player & tPlayer, SNO snoQuest);  // Player.cpp:2885
void PlayerSavedQuestInitialize(PlayerSavedQuest & tSavedQuest);  // Player.cpp:3291
void QuestsSyncOpenWorldTrackedSNOToPlayer(Player * ptPlayer);  // Quests.cpp:2993
void SPlayerUpdateQuestHistory(Player & tPlayer, SNO snoQuest, int32 nStepUID);  // SPlayer.cpp:10376
static void sSQuestsSendDynamicQuestObjectiveUpdate(int32 nObjective, PlayerList & listDestPlayers);  // SQuests.cpp:635
void SQuestsSyncPlayerSavedQuests(Player & tPlayer);  // SQuests.cpp:4476
void SQuestsResendCounters(int32 nPlayerIndex);  // SQuests.cpp:4854
void SQuestsClearConversationIconsForEnteringPlayer(Player & tPlayer);  // SQuests.cpp:325
void sSQuestsClearFinishedConversationsForPlayer(const QuestInstance & tQuestInstance, Player & tPlayer);  // SQuests.cpp:253
void sSQuestsSetGameFlagsForQuest(const QuestInstance & tQuestInstance);  // SQuests.cpp:407
void SQuestsSendQuestMeter(const QuestInstance & tQuest, Player * ptPlayer);  // SQuests.cpp:823
void SQuestsSendFlags(const QuestInstance & tQuest, Player * ptPlayer);  // SQuests.cpp:853
BOOL ExperiencePlayersCloseEnough(const ActorCommonData & tACDVictim, ACDID idACDOtherPlayer);  // ExperienceCommon.cpp:50
int32 SGameStatsEncodeSNODataToID(SNO sno, int32 nData);  // SGameStats.cpp:4
void SPlayerMarkConversationAsHeard(Player * ptPlayer, const SNO snoConversation);  // SPlayer.cpp:5816
BOOL QuestHasStepBeenReached(const QuestInstance & tQuest, int32 nStepUID);  // Quests.cpp:1500
BOOL QuestHasStepBeenPassed(const QuestInstance & tQuest, int32 nStepUID);  // Quests.cpp:1509
PlayerSavedQuest * PlayerAddSavedQuest(Player & tPlayer, SNO snoQuest);  // Player.cpp:2906
BOOL QuestStepSavePartialCompletion(SNO snoQuest, int32 nStepUID);  // Quests.cpp:1195
int32 QuestStepGetNumObjectives(const QuestStep & tQuestStep);  // Quests.cpp:1373
void GameFlagSet(LPCSTR szFlagLabel, BOOL bValue);  // ScriptUtil.cpp:6215
BOOL PlayerGetSavedQuest(const D3::Hero::SavedDefinition & tHeroDefinition, SNO snoQuest, PlayerSavedQuest & tPlayerSavedQuest);  // Player.cpp:2865
void QuestGetBonusAward(const Player * ptPlayer, SNO snoQuest, const QuestStepBonusObjectiveSet & tBonusObjectiveSet, int32 & nBonusXPGranted, int32 & nBonusGoldGranted, int32 nTimesCompleted);  // Quests.cpp:2871
BOOL ExperienceGrantDirectly(ActorCommonData & tACDPlayer, const uint64 nExpGranted, const BOOL bNotifyClient, const BOOL bGrantToAllPlayersIfLocal);  // Experience.cpp
#include "../../Common/Definitions/MarkerSetDefinition.h"
#include "../../Common/Marker/Marker.h"
#include "../../Common/Definitions/QuestRangeDefinition.h"
const uint32 BOUNTY_SCENE_DEACTIVATION_UPDATE_RATE = 5;  // DWARF SQuests.cpp:91
#include "../../Common/TagMaps/Tags.h"
#include "../LuaScript/ScriptEvent.h"
#include "../System/SNephalemRift.h"
#include "../SPlayer.h"
extern const uint32 CREATOR_APP_TICKS_TO_DISCONNECT_GAME;  // SPlayer.cpp:261
extern const uint32 gdwQuestUpdateRate;  // Quests.cpp:73
extern const uint32 gdwBountySceneActivationUpdateRate;  // Quests.cpp:74
void SceneCommonDataActivate(const SSceneID idSScene);  // SceneCommon.cpp:729
void SSceneForceDeactivationCheckThisTick();  // SScene.cpp:1457
void NPCUpdateConversationIconAndInteraction(ActorCommonData & tACDNPC, const Player & tPlayer);  // SNPC.h (SNPC.cpp:438)
void SQuestsOnPlayerEnterWorld(ACDID idACDPlayer, SNO snoWorld);  // SQuests.cpp:4729
void SQuestsOnPlayerEnterLevelArea(ACDID idACDPlayer, SNO snoLevelArea);  // SQuests.cpp:4743
enum { FLOATING_NUMBER_TYPE_XP = 33, FLOATING_NUMBER_TYPE_BONUS_QUEST_REWARD = 34 };
#include "../../Common/TagMaps/MarkerTags.h"
void SQuestsSendCounterToPlayer(QuestInstance & tQuest, int32 nObjectiveIndex, BOOL bCompleted, const Player * ptPlayer);  // SQuests.cpp:5233
void SQuestsUpdateQuestObjectiveCompleted(QuestInstance & tQuest, int32 nObjectiveIndex, BOOL bCompleted);  // SQuests.cpp:5271
static void sSQuestsHandleBountyObjectiveCompletion(const QuestStepObjective & tObjective);  // SQuests.cpp:4304
void SQuestsHandleDynamicObjectiveCompletion(int32 nObjectiveIndex);  // SQuests.cpp:4329


void ScriptEventOnQuestTimerRunningOut(LPCSTR szQuest, LPCSTR szStep, uint32 dwTimeRemaining);  // ScriptEvent.h:109
int32 FlagSetLookup(SNO snoFlagSet, LPCSTR szFlagLabel);  // FlagSetDefinition.h:63
SNO LevelAreaGetParentLevelArea(SNO snoLevelArea);  // LevelAreaDefinition.h:164 (LevelAreaDefinition.cpp:587)
DataIDMemoryPool* MemoryPoolsGetDATAIDListPool();  // MemoryPools.h:16
uint32 MarkerStringHash(LPCSTR pszName);  // Marker.h:58 (Marker.cpp:204)
const CHAR * MarkerNameGetString(uint32 dwHash);  // Marker.h:72 (Marker.cpp:176)
CRefString AdventureGetParentName(const uint16 nAdventureParentIndex);  // Adventure.h:9 (Adventure.cpp:232)
void ScriptEventOnQuestObjectiveProgressed(LPCSTR szQuest, LPCSTR szStep, ACDID idACDPlayer, int32 nCountLeft);  // ScriptEvent.h:106
SNO ProximityDetectorBrainDataGetTriggerActorSNO(ActorID idTrigger);  // X/Server/Ai/ProximityDetectorBrain.cpp:994
void ACDsIntersectSphere(const WorldPlace & tPlace, const WorldDistance wdRadius, const ACDID idACDIgnore, XPooledDataIDList & listACDs, uint32 dwSphereTreeFlags);  // X/Common/Actors/ActorCollision.cpp:317
void InactiveMarkerGetMarkerName(InactiveBaseMarker * ptInactiveMarker, uint32 & tMarkerName);  // X/Common/InactiveMarker.cpp:673
void ActorDeleteQuestLifespanActors();  // X/Server/Actors/Actors.cpp:6984
namespace Minimap { void UpdateMarkersForQuestState(); }  // X/Server/Map/Minimap.cpp:1461
void ServerQueueClientUpdate();  // X/Server/Network/Server.cpp:438
void SGameStatsTimedEventFinish(StatCounterType eType, int32 nParam0, int32 nParam1);  // X/Server/SGameStats.cpp:1484
void SGameStatsTimedEventStart(StatCounterType eType, int32 nParam0, int32 nParam1, BOOL bReportIfUnfinished);  // X/Server/SGameStats.cpp:1463
void ActorPowersRemoveGlobalPower(SNO snoPower, int32 nPlayerIndex);  // X/Server/Actors/ActorPowers.cpp:541
void ActorPowersGrantGlobalPower(SNO snoPower, int32 nPlayerIndex);  // X/Server/Actors/ActorPowers.cpp:493
SWorldID WorldCommonFindBySNO(SNO snoWorld, BOOL bIgnorePendingDelete);  // X/Common/World.cpp:605
void SQuestsPlayerJoinedGame(Player &)
{
    QuestsRefreshQuestDialogs(-1);
}
void SGameStatsPlayerCompletedQuest(ACDID idACDPlayer, SNO snoQuest, QuestType eQuestType, BOOL bGetAchievementCredit);  // X/Server/SGameStats.cpp:1214
void SGameStatsPlayerStartedQuest(ACDID idACDPlayer, SNO snoQuest, QuestType eQuestType);  // X/Server/SGameStats.cpp:1180
void SQuestsOnQuestCompleted(SNO snoQuest, int32 nPlayerIndex);  // X/Server/Quests/SQuests.cpp:4561
void SPlayerClearSavePoint(Player & tPlayer);  // X/Server/SPlayer.cpp:11027
void SGameStatsCounterSet(StatCounterType eType, int32 nParam0, int32 nParam1, float flValue);  // X/Server/SGameStats.cpp:1525
#include "../Crafting/SItemCrafting.h"
#include "../Loot/Loot.h"
uint64 ExperienceGetRestXPToGrant(const ActorCommonData & tACDPlayer, const uint64 nXPGranted);  // X/Common/ExperienceCommon.cpp:117
void SPlayerReceivedQuestStepReward(Player & tPlayer, SNO snoQuest, int32 nStepUID, DifficultyLevel eDifficulty);  // X/Server/SPlayer.cpp:10451
void SQuestsPlayerJoinAct(Player & tPlayer)
{
    QuestInstance * ptQuestInstance;

    GameCommonData & tGCD = GameGetActiveGameCommonData();
    QuestGlobals *& ptQuestGlobals = tGCD.ptQuestGlobals;

    POS pos = ptQuestGlobals->tQuestInstanceList.GetHeadPosition();
    while (pos)
    {
        QuestInstance & tQuestInstance = ptQuestGlobals->tQuestInstanceList.GetNext(pos);
        if (QuestStepTrackPerPlayer(tQuestInstance.snoQuest, tQuestInstance.nCurrentStepUID))
        {
            QuestInstance tPlayerInstance;
            QuestsQuestInstanceInitialize(tPlayerInstance, tQuestInstance.snoQuest, SNO_NONE);
            tPlayerInstance.nPlayerIndex = tPlayer.nPlayerIndex;
            QuestsAdvanceToStep(tPlayerInstance, tQuestInstance.nCurrentStepUID, FALSE);

            PlayerSavedQuest * ptSavedQuest = PlayerGetSavedQuest(tPlayer, tQuestInstance.snoQuest);
            if (ptSavedQuest)
            {
                QuestsAdvanceToStep(tPlayerInstance, ptSavedQuest->nCurrentStepUID, FALSE);
            }

            ptQuestGlobals->tQuestPerPlayerInstanceList.AddTail_Tracked(tPlayerInstance);
            SQuestsSendQuestStateToPlayer(tPlayerInstance, tPlayer, FALSE);
        }
        else
        {

            SQuestsSendQuestStateToPlayer(tQuestInstance, tPlayer, FALSE);
        }

        SQuestsSendQuestMeter(tQuestInstance, &tPlayer);
        if (tQuestInstance.dwFlags)
        {
            SQuestsSendFlags(tQuestInstance, &tPlayer);
        }
    }

    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetSingle(listDestPlayers, tPlayer);

    for (int32 i = 0; i < 10; i++)
    {
        sSQuestsSendDynamicQuestObjectiveUpdate(i, listDestPlayers);
    }

    SQuestsSyncPlayerSavedQuests(tPlayer);

    QuestsSyncOpenWorldTrackedSNOToPlayer(&tPlayer);
    tPlayer.uConsoleSyncFlags |= (1 << PCSF_SAVED_QUESTS);
    ptQuestInstance = GetLeadingMainLineQuest();
    if (ptQuestInstance)
    {
        SPlayerUpdateQuestHistory(tPlayer, ptQuestInstance->snoQuest, ptQuestInstance->nCurrentStepUID);
    }

    SQuestsResendCounters(tPlayer.nPlayerIndex);

    SQuestsClearConversationIconsForEnteringPlayer(tPlayer);
}
void SPlayerAddNephalemBuffStack(ACDID idACDPlayer);  // X/Server/SPlayer.cpp:12334
















void sSQuestClearConversationsForQuestObjective(
    const QuestStepObjectiveSet & tObjectiveSet,
    Player & tPlayer)
{
    int32 nNumObjectives = tObjectiveSet.serStepObjectives.dwSize / sizeof(QuestStepObjective);
    for (int32 k = 0; k < nNumObjectives; k++)
    {
        QuestStepObjective & tObjective = tObjectiveSet.arStepObjectives[k];
        if (tObjective.eObjectiveType == OBJECTIVE_HAD_CONVERSATION)
        {
            SPlayerMarkConversationAsHeard(&tPlayer, tObjective.snoname1.snoHandle);
        }
    }
}




void sSQuestClearConversationForAllPlayers(
    SNO snoConversation)
{
    for (Player *ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {

        SPlayerMarkConversationAsHeard(ptPlayer, snoConversation);
    }
}



void sSQuestsClearFinishedConversationsForPlayer(
    const QuestInstance & tQuestInstance,
    Player & tPlayer)
{
    if (tQuestInstance.eInstanceState != QUEST_INSTANCE_UNASSIGNED)
    {
        ScopedQuestDefinitionLockR ptQuestDef(tQuestInstance.snoQuest);


        const QuestUnassignedStep & tQuestStep = ptQuestDef->tQuestUnassignedStep;
        int32 nNumObjectiveSets = tQuestStep.serStepObjectiveSets.dwSize / sizeof(QuestStepObjectiveSet);

        for (int32 nObjSet = 0; nObjSet < nNumObjectiveSets; nObjSet++)
        {
            const QuestStepObjectiveSet & tObjectiveSet = tQuestStep.arStepObjectiveSets[nObjSet];

            BOOL bPassed;
            if (tObjectiveSet.nNextStepUID < 0)
            {
                bPassed = QuestHasStepBeenPassed(tQuestInstance, -1);
            }
            else
            {
                bPassed = QuestHasStepBeenReached(tQuestInstance, tObjectiveSet.nNextStepUID);
            }

            if (bPassed)
            {

                sSQuestClearConversationsForQuestObjective(tObjectiveSet, tPlayer);
            }
        }


        for (int32 nStep = 0; nStep < ptQuestDef->nNumQuestSteps; nStep++)
        {
            const QuestStep & tQuestStepScoped = ptQuestDef->arQuestSteps[nStep];


            if (tQuestStepScoped.dwFlags & (1 << QSF_ORPHANED_STEP))
            {
                continue;
            }

            nNumObjectiveSets = tQuestStepScoped.serStepObjectiveSets.dwSize / sizeof(QuestStepObjectiveSet);

            for (int32 nObjSet = 0; nObjSet < nNumObjectiveSets; nObjSet++)
            {
                const QuestStepObjectiveSet & tObjectiveSet = tQuestStepScoped.arStepObjectiveSets[nObjSet];

                BOOL bPassed;
                if (tObjectiveSet.nNextStepUID < 0)
                {
                    bPassed = QuestHasStepBeenPassed(tQuestInstance, tQuestStepScoped.nUID);
                }
                else
                {
                    bPassed = QuestHasStepBeenReached(tQuestInstance, tObjectiveSet.nNextStepUID);
                }

                if (bPassed)
                {

                    sSQuestClearConversationsForQuestObjective(tObjectiveSet, tPlayer);
                }
            }
        }
    }
}



void SQuestsClearConversationIconsForEnteringPlayer(Player & tPlayer)
{





    QuestGlobals * ptQuestGlobals = GameGetActiveGameCommonData().ptQuestGlobals;
    POS pos = ptQuestGlobals->tQuestInstanceList.GetHeadPosition();
    while (pos)
    {
        QuestInstance & tQuestInstance = ptQuestGlobals->tQuestInstanceList.GetNext(pos);
        if (QuestGetQuestType(tQuestInstance.snoQuest) == QUEST_TYPE_MAIN_QUEST)
        {
            sSQuestsClearFinishedConversationsForPlayer(tQuestInstance, tPlayer);
        }
    }
}


void sSQuestSetGameFlagsForCompletedQuestStep(
    const QuestStepObjectiveSet & tObjectiveSet)
{
    int32 nNumObjectives = tObjectiveSet.serStepObjectives.dwSize / sizeof(QuestStepObjective);
    for (int32 k = 0; k < nNumObjectives; k++)
    {
        QuestStepObjective & tObjective = tObjectiveSet.arStepObjectives[k];
        if (tObjective.eObjectiveType == OBJECTIVE_GAME_FLAG_SET)
        {
            GameFlagSet(tObjective.szParam1_VariableArray, TRUE);
        }
    }
}



void sSQuestSetGameFlagsForCompletedQuestObjectives(
    const QuestInstance & tQuestInstance,
    const QuestStepObjectiveSet & tObjectiveSet,
    int32 & nCurrentObjectiveIndex)
{
    int32 nNumObjectives = tObjectiveSet.serStepObjectives.dwSize / sizeof(QuestStepObjective);
    for (int32 k = 0; k < nNumObjectives; k++, nCurrentObjectiveIndex++)
    {
        QuestStepObjective & tObjective = tObjectiveSet.arStepObjectives[k];
        if (tObjective.eObjectiveType == OBJECTIVE_GAME_FLAG_SET)
        {
            if (tQuestInstance.arObjectiveState[nCurrentObjectiveIndex] > 0)
            {
                GameFlagSet(tObjective.szParam1_VariableArray, TRUE);
            }
        }
    }


}



void sSQuestSetGameFlagsForCompletedQuestBonusObjectives(
    const QuestInstance & tQuestInstance,
    const QuestStepBonusObjectiveSet & tBonusObjectiveSet,
    int32 & nCurrentBonusObjectiveIndex)
{
    int32 nNumBonusObjectives = tBonusObjectiveSet.serStepBonusObjectives.dwSize / sizeof(QuestStepObjective);
    for (int32 k = 0; k < nNumBonusObjectives; k++, nCurrentBonusObjectiveIndex++)
    {
        QuestStepObjective & tBonusObjective = tBonusObjectiveSet.arStepBonusObjectives[k];
        if (tBonusObjective.eObjectiveType == OBJECTIVE_GAME_FLAG_SET)
        {
            if (tQuestInstance.arObjectiveState[nCurrentBonusObjectiveIndex] > 0)
            {
                GameFlagSet(tBonusObjective.szParam1_VariableArray, TRUE);
            }
        }
    }


}



void sSQuestsSetGameFlagsForQuest(
    const QuestInstance & tQuestInstance)
{
    if (tQuestInstance.eInstanceState != QUEST_INSTANCE_UNASSIGNED)
    {
        ScopedQuestDefinitionLockR ptQuestDef(tQuestInstance.snoQuest);


        const QuestUnassignedStep & tQuestStep = ptQuestDef->tQuestUnassignedStep;
        int32 nNumObjectiveSets = tQuestStep.serStepObjectiveSets.dwSize / sizeof(QuestStepObjectiveSet);

        for (int32 nObjSet = 0; nObjSet < nNumObjectiveSets; nObjSet++)
        {
            const QuestStepObjectiveSet & tObjectiveSet = tQuestStep.arStepObjectiveSets[nObjSet];

            BOOL bPassed;
            if (tObjectiveSet.nNextStepUID < 0)
            {
                bPassed = QuestHasStepBeenPassed(tQuestInstance, -1);
            }
            else
            {
                bPassed = QuestHasStepBeenReached(tQuestInstance, tObjectiveSet.nNextStepUID);
            }

            if (bPassed)
            {

                sSQuestSetGameFlagsForCompletedQuestStep(tObjectiveSet);
            }
        }


        for (int32 nStep = 0; nStep < ptQuestDef->nNumQuestSteps; nStep++)
        {
            const QuestStep & tQuestStepScoped = ptQuestDef->arQuestSteps[nStep];


            if (tQuestStepScoped.dwFlags & (1 << QSF_ORPHANED_STEP))
            {
                continue;
            }

            nNumObjectiveSets = tQuestStepScoped.serStepObjectiveSets.dwSize / sizeof(QuestStepObjectiveSet);

            for (int32 nObjSet = 0; nObjSet < nNumObjectiveSets; nObjSet++)
            {
                const QuestStepObjectiveSet & tObjectiveSet = tQuestStepScoped.arStepObjectiveSets[nObjSet];

                BOOL bPassed;
                if (tObjectiveSet.nNextStepUID < 0)
                {
                    bPassed = QuestHasStepBeenPassed(tQuestInstance, tQuestStepScoped.nUID);
                }
                else
                {
                    bPassed = QuestHasStepBeenReached(tQuestInstance, tObjectiveSet.nNextStepUID);
                }

                if (bPassed)
                {

                    sSQuestSetGameFlagsForCompletedQuestStep(tObjectiveSet);
                }
            }

            if (tQuestInstance.nCurrentStepUID == tQuestStepScoped.nUID)
            {


                nNumObjectiveSets = tQuestStepScoped.serStepObjectiveSets.dwSize / sizeof(QuestStepObjectiveSet);
                int32 nCurrentObjectiveIndex = 0;

                for (int32 nObjSet = 0; nObjSet < nNumObjectiveSets; nObjSet++)
                {
                    const QuestStepObjectiveSet & tObjectiveSet = tQuestStepScoped.arStepObjectiveSets[nObjSet];
                    sSQuestSetGameFlagsForCompletedQuestObjectives(tQuestInstance, tObjectiveSet, nCurrentObjectiveIndex);
                }


                int32 nNumBonusObjectiveSets = tQuestStepScoped.serStepBonusObjectiveSets.dwSize / sizeof(QuestStepBonusObjectiveSet);
                int32 nCurrentBonusObjectiveIndex = QuestStepGetNumObjectives(tQuestStepScoped);

                for (int32 nBonusObjSet = 0; nBonusObjSet < nNumBonusObjectiveSets; nBonusObjSet++)
                {
                    const QuestStepBonusObjectiveSet & tBonusObjectiveSet = tQuestStepScoped.arStepBonusObjectiveSets[nBonusObjSet];
                    sSQuestSetGameFlagsForCompletedQuestBonusObjectives(tQuestInstance, tBonusObjectiveSet, nCurrentBonusObjectiveIndex);
                }
            }
        }
    }
}



void SQuestsSetGameFlagsForCompletedQuests()
{




    QuestGlobals * ptQuestGlobals = GameGetActiveGameCommonData().ptQuestGlobals;
    POS pos = ptQuestGlobals->tQuestInstanceList.GetHeadPosition();
    while (pos)
    {

        QuestInstance & tQuestInstance = ptQuestGlobals->tQuestInstanceList.GetNext(pos);
        if (QuestGetQuestType(tQuestInstance.snoQuest) == QUEST_TYPE_MAIN_QUEST)
        {
            sSQuestsSetGameFlagsForQuest(tQuestInstance);
        }
    }
}

void SQuestsPlayerLeaveGame(Player & tPlayer)
{



    QuestGlobals * ptQuestGlobals = GameGetActiveGameCommonData().ptQuestGlobals;
    POS pos = ptQuestGlobals->tQuestPerPlayerInstanceList.GetHeadPosition();
    while (pos)
    {
        POS posCurr = pos;
        QuestInstance & tQuestInstance = ptQuestGlobals->tQuestPerPlayerInstanceList.GetNext(pos);
        if (tQuestInstance.nPlayerIndex == tPlayer.nPlayerIndex)
        {
            ptQuestGlobals->tQuestPerPlayerInstanceList.RemoveAt(posCurr);
        }
    }

    QuestsRefreshQuestDialogs(-1);
}
















































































static void sQuestsPlayerEnterTownUpdate()
{



    for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {
        SSceneID idSSceneCurrent = ACDGetCurrentSSceneID(ACDGet(ptPlayer->idPlayerACD));


        ptPlayer->tQuestData.idSSceneLast = idSSceneCurrent;
    }
}


static void sSQuestsSendDynamicQuestObjectiveUpdate(int32 nObjective, PlayerList & listDestPlayers)
{


    QuestGlobals * ptQuestGlobals = GameGetActiveGameCommonData().ptQuestGlobals;

    if (ptQuestGlobals->tDynamicQuestObjectives[nObjective].eObjectiveType == OBJECTIVE_INVALID)
        return;

    BOOL bCompleted = ptQuestGlobals->arDynamicQuestObjectiveCompleted[nObjective];
    BOOL bHidden = ptQuestGlobals->arDynamicQuestObjectiveHidden[nObjective];
    int32 nState = ptQuestGlobals->arDynamicQuestObjectiveState[nObjective];

    DynamicQuestObjectiveMessage tMessage;
    QuestStepObjective & tObjective = ptQuestGlobals->tDynamicQuestObjectives[nObjective];
    tMessage.eObjectiveType = tObjective.eObjectiveType;
    tMessage.nUID = tObjective.nUID;
    tMessage.nParam1 = tObjective.nParam1;
    tMessage.nParam2 = tObjective.nParam2;
    tMessage.snoname1 = tObjective.snoname1;
    tMessage.snoname2 = tObjective.snoname2;
    tMessage.gbidParam1 = tObjective.gbidParam1;
    tMessage.gbidParam2 = tObjective.gbidParam2;
    tMessage.bHiddenUntilComplete = tObjective.bHiddenUntilComplete;
    tMessage.dwFlags = tObjective.dwFlags;
    tMessage.gbidItemToShow = tObjective.gbidItemToShow;
    tMessage.nState = nState;
    tMessage.bCompleted = bCompleted;
    tMessage.bHidden = bHidden;
    MessageSendToClient(listDestPlayers, SMSG_DYNAMIC_QUEST_OBJECTIVE_UPDATE, &tMessage, sizeof(tMessage));
}




void SQuestSystemUpdate()
{



    QuestGlobals * ptQuestGlobals = GameGetActiveGameCommonData().ptQuestGlobals;
    ServerUpdateWasCalled(ptQuestGlobals->tQuestsUpdateData);





    sQuestsPlayerEnterTownUpdate();




    if (ptQuestGlobals->bResendEnterEvents)
    {
        ptQuestGlobals->bResendEnterEvents = FALSE;

        for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
        {
            ACDID idACDPlayer = ptPlayer->idPlayerACD;
            ActorCommonData * ptACDPlayer = ACDTryToGet(idACDPlayer);



            if (ptACDPlayer)
            {
                WorldPlace placeCurr = ptACDPlayer->GetPlace();
                SNO snoWorld = WorldCommonGetWorldSNO(placeCurr.idSWorld);
                SQuestsOnPlayerEnterWorld(idACDPlayer, snoWorld);


                LevelAreaID idLevelArea = ACDGetLevelAreaID(*ptACDPlayer);
                SNO snoLevelAreaNew = LevelAreaGetSNO(idLevelArea);
                SQuestsOnPlayerEnterLevelArea(idACDPlayer, snoLevelAreaNew);
            }
        }
    }






    for (int32 i = 0; i < 10; i++)
    {
        if (ptQuestGlobals->arDynamicQuestObjectiveDirty[i])
        {
            ptQuestGlobals->arDynamicQuestObjectiveDirty[i] = FALSE;
            PlayerList listDestPlayers(PlayerListGetElementPool());
            PlayerListGetAllInGame(listDestPlayers, -1, TRUE, -1);
            sSQuestsSendDynamicQuestObjectiveUpdate(i, listDestPlayers);
        }
    }


    ACDIterator tIt;
    while (ActorCommonDataIterate(tIt))
    {

        ActorCommonData * ptACD = tIt.ptElement;
        if (ptACD->IsNPC())
        {

            for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
            {
                NPCUpdateConversationIconAndInteraction(*ptACD, *ptPlayer);
            }
        }
    }



    ServerQueueUpdateFunction(gdwQuestUpdateRate, ptQuestGlobals->tQuestsUpdateData);

}























void SQuestsSendQuestReward(const Player & tDestPlayer, const BOOL bIsComplete, const D3::Quests::QuestReward & tReward, const BOOL bEquipped)
{




    if (PlayerIsHardcoreAndDead(tDestPlayer))
        return;





    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetSingle(listDestPlayers, tDestPlayer.idPlayerACD);

    D3::Quests::QuestStepComplete tQuestStepComplete;
    tQuestStepComplete.set_is_quest_complete(bIsComplete != FALSE);
    tQuestStepComplete.set_was_reward_autoequipped(bEquipped != FALSE);
    *tQuestStepComplete.mutable_reward() = tReward;

    MessageSendToClient(listDestPlayers, SMSG_QUEST_REWARD, tQuestStepComplete);

    SPlayerRefreshWaypointDestination(tDestPlayer.idPlayerACD);
}

void SQuestsSendTieredRiftReward(const Player & tDestPlayer, const BOOL bFinishedInTime, const int32 nRiftDepth, const D3::Quests::QuestReward & tReward)
{




    if (PlayerIsHardcoreAndDead(tDestPlayer))
        return;





    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetSingle(listDestPlayers, tDestPlayer.idPlayerACD);

    D3::Quests::QuestTieredRiftComplete tTieredRiftComplete;
    tTieredRiftComplete.set_is_finished_in_time(bFinishedInTime != FALSE);
    tTieredRiftComplete.set_rift_depth(nRiftDepth);
    *tTieredRiftComplete.mutable_reward() = tReward;



    MessageSendToClient(listDestPlayers, SMSG_TIERED_RIFT_REWARD, tTieredRiftComplete);
}
void SQuestsSendQuestMeter(const QuestInstance & tQuest, Player * ptPlayer)
{
    QuestMeterMessage tMessage;
    tMessage.snoQuest = tQuest.snoQuest;
    tMessage.annMeter = ANNFromACD(tQuest.idACDMeter);
    tMessage.flMeter = tQuest.flMeter;

    PlayerList listDestPlayers(PlayerListGetElementPool());
    if (ptPlayer)
    {
        if (PlayerIsHardcoreAndDead(*ptPlayer))
            return;
        listDestPlayers.AddTail_Tracked(ptPlayer);
    }
    else
    {
        PlayerListGetAllConnected(listDestPlayers, -1, TRUE);
    }
    MessageSendToClient(listDestPlayers, SMSG_QUEST_METER, &tMessage, sizeof(tMessage));
}










void SQuestsSendFlags(const QuestInstance & tQuest, Player * ptPlayer)
{
    QuestFlagsMessage tMessage;
    tMessage.snoQuest = tQuest.snoQuest;
    tMessage.dwFlags = tQuest.dwFlags;

    PlayerList listDestPlayers(PlayerListGetElementPool());
    if (ptPlayer)
    {
        if (PlayerIsHardcoreAndDead(*ptPlayer))
            return;
        listDestPlayers.AddTail_Tracked(ptPlayer);
    }
    else
    {
        PlayerListGetAllConnected(listDestPlayers, -1, TRUE);
    }
    MessageSendToClient(listDestPlayers, SMSG_QUEST_FLAGS, &tMessage, sizeof(tMessage));
}










void SQuestsResetQuest(QuestInstance & tQuestInstance)
{
    tQuestInstance.eInstanceState = QUEST_INSTANCE_UNASSIGNED;
    tQuestInstance.nCurrentStepUID = -1;
    tQuestInstance.dwStarted = GameGetServerTime();

    for (int32 i = 0; i < 20; i++)
    {
        tQuestInstance.arFailureConditionState[i] = 0;
    }

    for (int32 i = 0; i < 20; i++)
    {
        tQuestInstance.arObjectiveState[i] = 0;
    }

    for (int32 i = 0; i < 20; i++)
    {
        tQuestInstance.arBonusRewards[i] = 0;
    }

    SQuestsSendUpdateToAllPlayers(tQuestInstance.snoQuest);
}







void SQuestsForceCompleteQuest(QuestInstance & tQuest, BOOL bStartup)
{


    if (tQuest.eInstanceState == QUEST_INSTANCE_COMPLETED)
        return;
    ScopedQuestDefinitionLockR pdefQuest(tQuest.snoQuest);
    if (pdefQuest->nNumQuestCompletionSteps > 0)
    {

        QuestCompletionStep * ptQuestCompletionStep = pdefQuest->arQuestCompletionSteps;
        int32 nStepUID = ptQuestCompletionStep->nUID;
        int32 nCurrentStepUID = tQuest.nCurrentStepUID;


        QuestsAdvanceToStep(tQuest, nStepUID, FALSE);


        SQuestHandleQuestAdvancement(tQuest, nCurrentStepUID, nStepUID, bStartup);


        SQuestsSyncPlayerState(tQuest, NULL);
    }
}




void SQuestsCompleteMainlineUpToQuest(SNO snoQuest)
{
    BOOL bFound = FALSE;
    QuestGlobals * ptQuestGlobals = GameGetActiveGameCommonData().ptQuestGlobals;



    POS pos = ptQuestGlobals->tQuestInstanceList.GetHeadPosition();
    while (pos)
    {
        QuestInstance & tQuestInstance = ptQuestGlobals->tQuestInstanceList.GetNext(pos);
        if (QuestGetQuestType(tQuestInstance.snoQuest) == QUEST_TYPE_MAIN_QUEST && tQuestInstance.snoQuest == snoQuest)
            bFound = TRUE;
    }

    if (!bFound)
        return;

    pos = ptQuestGlobals->tQuestInstanceList.GetHeadPosition();
    while (pos)
    {
        QuestInstance & tQuestInstance = ptQuestGlobals->tQuestInstanceList.GetNext(pos);
        if (tQuestInstance.snoQuest == snoQuest)
            break;



        if (QuestGetQuestType(tQuestInstance.snoQuest) == QUEST_TYPE_MAIN_QUEST)
        {
            SQuestsForceCompleteQuest(tQuestInstance, TRUE);
        }
    }
}














BOOL sSQuestsObjectiveSetIsTargetingActor(QuestStepObjectiveSet & tObjectiveSet, ActorCommonData * ptACD)
{
    for (int32 k = 0; k < (int32)tObjectiveSet.serStepObjectives.ElementCount(sizeof(QuestStepObjective)); k++)
    {
        if (tObjectiveSet.arStepObjectives[k].eObjectiveType == OBJECTIVE_ENTER_TRIGGER)
        {
            if (ptACD->snoActor == tObjectiveSet.arStepObjectives[k].snoname2.snoHandle)
                return TRUE;
        }
    }
    return FALSE;
}








BOOL sSQuestsInstanceIsTargetingActor(QuestInstance & tQuestInstance, ACDID idActor)
{
    ScopedQuestDefinitionLockR ptQuestDef(tQuestInstance.snoQuest);
    if (tQuestInstance.eInstanceState == QUEST_INSTANCE_COMPLETED || tQuestInstance.eInstanceState == QUEST_INSTANCE_FAILED)
        return FALSE;
    ActorCommonData * ptACD = ACDTryToGet(idActor);
    if (tQuestInstance.eInstanceState == QUEST_INSTANCE_UNASSIGNED)
    {
        int32 nNumObjectiveSets = ptQuestDef->tQuestUnassignedStep.serStepObjectiveSets.ElementCount(sizeof(QuestStepObjectiveSet));
        for (int32 j = 0; j < nNumObjectiveSets; j++)
        {
            QuestStepObjectiveSet & tObjectiveSet = ptQuestDef->tQuestUnassignedStep.arStepObjectiveSets[j];
            if (sSQuestsObjectiveSetIsTargetingActor(tObjectiveSet, ptACD))
            {
                return TRUE;
            }
        }
    }
    else
    {
        int32 nStepIndex = QuestStepUIDToIndex(ptQuestDef, tQuestInstance.nCurrentStepUID);
        if (nStepIndex == -1)
            return FALSE;
        QuestStep & tQuestStep = ptQuestDef->arQuestSteps[nStepIndex];
        int32 nNumObjectiveSets = tQuestStep.serStepObjectiveSets.ElementCount(sizeof(QuestStepObjectiveSet));
        for (int32 j = 0; j < nNumObjectiveSets; j++)
        {
            QuestStepObjectiveSet & tObjectiveSet = tQuestStep.arStepObjectiveSets[j];
            if (sSQuestsObjectiveSetIsTargetingActor(tObjectiveSet, ptACD))
            {
                return TRUE;
            }
        }
    }
    return FALSE;
}


















BOOL SQuestsIsQuestTriggerTargetingActor(ACDID idActor)
{

    GameCommonData & tGCD = GameGetActiveGameCommonData();
    QuestGlobals *& ptQuestGlobals = tGCD.ptQuestGlobals;


    POS pos = ptQuestGlobals->tQuestInstanceList.GetHeadPosition();
    while (pos)
    {
        QuestInstance & tQuestInstance = ptQuestGlobals->tQuestInstanceList.GetNext(pos);
        if (sSQuestsInstanceIsTargetingActor(tQuestInstance, idActor))
            return TRUE;
    }



    pos = ptQuestGlobals->tQuestPerPlayerInstanceList.GetHeadPosition();
    while (pos)
    {
        QuestInstance & tQuestInstance = ptQuestGlobals->tQuestPerPlayerInstanceList.GetNext(pos);
        if (sSQuestsInstanceIsTargetingActor(tQuestInstance, idActor))
            return TRUE;
    }
    return FALSE;
}







BOOL sSQuestsObjectiveSetRequiresItem(
    const QuestStepObjectiveSet & tObjectiveSet,
    GBID gbidItemRequired)
{
    int32 nNumObjectives = tObjectiveSet.serStepObjectives.dwSize / sizeof(QuestStepObjective);
    for (int32 k = 0; k < nNumObjectives; k++)
    {
        QuestStepObjective & tObjective = tObjectiveSet.arStepObjectives[k];
        if (tObjective.eObjectiveType == OBJECTIVE_POSSESS_ITEM &&
            tObjective.gbidParam1 == gbidItemRequired)
        {
            return TRUE;
        }
    }

    return FALSE;
}



BOOL sSQuestsObjectiveSetHasItem(
    const QuestInstance & tQuestInstance,
    const QuestStepObjectiveSet & tObjectiveSet,
    int32 & nCurrentObjectiveIndex,
    GBID gbidItemRequired)
{
    int32 nNumObjectives = tObjectiveSet.serStepObjectives.dwSize / sizeof(QuestStepObjective);
    for (int32 k = 0; k < nNumObjectives; k++, nCurrentObjectiveIndex++)
    {
        QuestStepObjective & tObjective = tObjectiveSet.arStepObjectives[k];
        if (tObjective.eObjectiveType == OBJECTIVE_POSSESS_ITEM &&
            tObjective.gbidParam1 == gbidItemRequired)
        {
            if (tQuestInstance.arObjectiveState[nCurrentObjectiveIndex] > 0)
            {
                return TRUE;
            }
        }
    }


    return FALSE;
}




BOOL sSQuestsBonusObjectiveSetHasItem(
    const QuestInstance & tQuestInstance,
    const QuestStepBonusObjectiveSet & tBonusObjectiveSet,
    int32 & nCurrentBonusObjectiveIndex,
    GBID gbidItemRequired)
{
    int32 nNumBonusObjectives = tBonusObjectiveSet.serStepBonusObjectives.dwSize / sizeof(QuestStepObjective);
    for (int32 k = 0; k < nNumBonusObjectives; k++, nCurrentBonusObjectiveIndex++)
    {
        QuestStepObjective & tBonusObjective = tBonusObjectiveSet.arStepBonusObjectives[k];
        if (tBonusObjective.eObjectiveType == OBJECTIVE_POSSESS_ITEM &&
            tBonusObjective.gbidParam1 == gbidItemRequired)
        {
            if (tQuestInstance.arObjectiveState[nCurrentBonusObjectiveIndex] > 0)
            {
                return TRUE;
            }
        }
    }


    return FALSE;
}




BOOL sSQuestsInstanceHasItem(
    QuestInstance & tQuestInstance,
    GBID gbidItemRequired)
{

    if (tQuestInstance.eInstanceState == QUEST_INSTANCE_UNASSIGNED)
        return FALSE;

    SNO snoQuest = tQuestInstance.snoQuest;
    ScopedQuestDefinitionLockR pdefQuest(snoQuest);




    {
        const QuestUnassignedStep & tQuestStep = pdefQuest->tQuestUnassignedStep;
        int32 nNumObjectiveSets = tQuestStep.serStepObjectiveSets.dwSize / sizeof(QuestStepObjectiveSet);

        for (int32 nObjSet = 0; nObjSet < nNumObjectiveSets; nObjSet++)
        {
            const QuestStepObjectiveSet & tObjectiveSet = tQuestStep.arStepObjectiveSets[nObjSet];

            if (sSQuestsObjectiveSetRequiresItem(tObjectiveSet, gbidItemRequired))
            {


                BOOL bPassed;
                if (tObjectiveSet.nNextStepUID < 0)
                {
                    bPassed = QuestHasStepBeenPassed(tQuestInstance, -1);
                }
                else
                {
                    bPassed = QuestHasStepBeenReached(tQuestInstance, tObjectiveSet.nNextStepUID);
                }

                if (bPassed)
                {
                    return TRUE;
                }

                break;
            }
        }
    }

    for (int32 nStep = 0; nStep < pdefQuest->nNumQuestSteps; nStep++)
    {
        const QuestStep & tQuestStep = pdefQuest->arQuestSteps[nStep];
        int32 nNumObjectiveSets = tQuestStep.serStepObjectiveSets.dwSize / sizeof(QuestStepObjectiveSet);

        for (int32 nObjSet = 0; nObjSet < nNumObjectiveSets; nObjSet++)
        {
            const QuestStepObjectiveSet & tObjectiveSet = tQuestStep.arStepObjectiveSets[nObjSet];

            if (sSQuestsObjectiveSetRequiresItem(tObjectiveSet, gbidItemRequired))
            {


                BOOL bPassed;
                if (tObjectiveSet.nNextStepUID < 0)
                {
                    bPassed = QuestHasStepBeenPassed(tQuestInstance, tQuestStep.nUID);
                }
                else
                {
                    bPassed = QuestHasStepBeenReached(tQuestInstance, tObjectiveSet.nNextStepUID);
                }

                if (bPassed)
                {
                    return TRUE;
                }

                break;
            }
        }
        if (tQuestInstance.nCurrentStepUID == tQuestStep.nUID)
        {


            nNumObjectiveSets = tQuestStep.serStepObjectiveSets.dwSize / sizeof(QuestStepObjectiveSet);
            int32 nCurrentObjectiveIndex = 0;

            for (int32 nObjSet = 0; nObjSet < nNumObjectiveSets; nObjSet++)
            {
                const QuestStepObjectiveSet & tObjectiveSet = tQuestStep.arStepObjectiveSets[nObjSet];
                if (sSQuestsObjectiveSetHasItem(tQuestInstance, tObjectiveSet, nCurrentObjectiveIndex, gbidItemRequired))
                {
                    return TRUE;
                }
            }


            int32 nNumBonusObjectiveSets = tQuestStep.serStepBonusObjectiveSets.dwSize / sizeof(QuestStepBonusObjectiveSet);
            int32 nCurrentBonusObjectiveIndex = QuestStepGetNumObjectives(tQuestStep);

            for (int32 nBonusObjSet = 0; nBonusObjSet < nNumBonusObjectiveSets; nBonusObjSet++)
            {
                const QuestStepBonusObjectiveSet & tBonusObjectiveSet = tQuestStep.arStepBonusObjectiveSets[nBonusObjSet];
                if (sSQuestsBonusObjectiveSetHasItem(tQuestInstance, tBonusObjectiveSet, nCurrentBonusObjectiveIndex, gbidItemRequired))
                {
                    return TRUE;
                }
            }
        }
    }

    return FALSE;
}



BOOL SQuestsHasQuestItemBeenFound(GBID gbidItemRequired)
{

    GameCommonData & tGCD = GameGetActiveGameCommonData();
    QuestGlobals *& ptQuestGlobals = tGCD.ptQuestGlobals;


    POS pos = ptQuestGlobals->tQuestInstanceList.GetHeadPosition();
    while (pos)
    {
        QuestInstance & tQuestInstance = ptQuestGlobals->tQuestInstanceList.GetNext(pos);
        if (sSQuestsInstanceHasItem(tQuestInstance, gbidItemRequired))
            return TRUE;
    }



    pos = ptQuestGlobals->tQuestPerPlayerInstanceList.GetHeadPosition();
    while (pos)
    {
        QuestInstance & tQuestInstance = ptQuestGlobals->tQuestPerPlayerInstanceList.GetNext(pos);
        if (sSQuestsInstanceHasItem(tQuestInstance, gbidItemRequired))
            return TRUE;
    }
    return FALSE;
}







enum QuestNotificationMessageType {
    QUEST_NOTIFICATION_HAD_CONVERSATION = 0,
    QUEST_NOTIFICATION_POSSESS_ITEM,
    QUEST_NOTIFICATION_ACTOR_KILLED,
    QUEST_NOTIFICATION_INTERACT_WITH_GIZMO,
    QUEST_NOTIFICATION_ENTER_WORLD,
    QUEST_NOTIFICATION_ENTER_SCENE,
    QUEST_NOTIFICATION_ENTER_LEVEL_AREA,
    QUEST_NOTIFICATION_TRIGGER,
    QUEST_NOTIFICATION_COMPLETE_QUEST,
    QUEST_NOTIFICATION_ITEM_USED,
    QUEST_NOTIFICATION_PLAYER_FLAG_SET,
    QUEST_NOTIFICATION_GAME_FLAG_SET,
    QUEST_NOTIFICATION_GROUP_KILLED,
    QUEST_NOTIFICATION_GROUP_MODIFIED,
    QUEST_NOTIFICATION_TIMED_EVENT_EXPIRED,
    QUEST_NOTIFICATION_TIMED_EVENT_ABOUT_TO_EXPIRE,
    QUEST_NOTIFICATION_EVENT_RECEIVED,
    QUEST_NOTIFICATION_RESEND_TO_CLIENTS,
    QUEST_NOTIFICATION_WORLD_MONSTERS_REMAINING_MODIFIED,
    QUEST_NOTIFICATION_ACTOR_REMOVED,
    QUEST_NOTIFICATION_HQ_REAGENT_COLLECTED,
};


static const int32 MAX_NOTIFICATION_STRING_SIZE = 256;

struct QuestNotificationMessage {
    QuestNotificationMessage()
    {
        sno1 = SNO_NONE;
        sno2 = SNO_NONE;
        idACD1 = ACDID_INVALID;
        idACD2 = ACDID_INVALID;
        nValue1 = 0;
        nValue2 = 0;
        szValue1[0] = '\0';
    }

    QuestNotificationMessageType eMsgType;
    SNO sno1;
    SNO sno2;
    ACDID idACD1;
    ACDID idACD2;
    int32 nValue1;
    int32 nValue2;
    CHAR szValue1[MAX_NOTIFICATION_STRING_SIZE];
};



static BOOL sSQuestFailureConditionProcessMessage(
    QuestStepFailureCondition & tFailureCondition,
    QuestInstance & tQuestInstance,
    QuestNotificationMessage & tMsg,
    int32 nFailureConditionIndex)
{






    if (tMsg.eMsgType == QUEST_NOTIFICATION_RESEND_TO_CLIENTS)
        return TRUE;



    switch (tFailureCondition.eFailureConditionType)
    {
        case FAILURE_CONDITION_ACTOR_DIED:
        {
            if (tMsg.eMsgType == QUEST_NOTIFICATION_ACTOR_KILLED)
            {




                ActorCommonData & tACD = ACDGet(tMsg.idACD2);
                if (tFailureCondition.snoname1.snoHandle == tACD.tSNOName.snoHandle)
                {
                    tQuestInstance.arFailureConditionState[nFailureConditionIndex]++;
                    return TRUE;
                }
            }
            break;
        }







        case FAILURE_CONDITION_MONSTER_DIED:
        {
            if (tMsg.eMsgType == QUEST_NOTIFICATION_ACTOR_KILLED)
            {




                ActorCommonData & tACD = ACDGet(tMsg.idACD2);
                if (!tACD.IsMonster())
                {
                    return FALSE;
                }

                if (tFailureCondition.snoname1.snoHandle == tACD.tSNOName.snoHandle)
                {
                    tQuestInstance.arFailureConditionState[nFailureConditionIndex]++;
                    return TRUE;
                }
            }
            break;
        }





        case FAILURE_CONDITION_PLAYER_DIED:
        {
            if (tMsg.eMsgType == QUEST_NOTIFICATION_ACTOR_KILLED)
            {




                ActorCommonData & tACD = ACDGet(tMsg.idACD2);
                if (!ACDIsPlayer(tMsg.idACD2))
                {
                    return FALSE;
                }

                if (tQuestInstance.nPlayerIndex != -1)
                {

                    Player *ptPlayer = PlayerGetByPlayerIndex(tQuestInstance.nPlayerIndex);
                    if (ptPlayer && ptPlayer->idPlayerACD != tMsg.idACD2)
                    {
                        return FALSE;
                    }
                }

                if (tFailureCondition.snoname1.snoHandle == tACD.tSNOName.snoHandle)
                {
                    tQuestInstance.arFailureConditionState[nFailureConditionIndex]++;
                    return TRUE;
                }
            }
            break;
        }





        case FAILURE_CONDITION_ITEM_USED:
        {
            if (tMsg.eMsgType == QUEST_NOTIFICATION_ITEM_USED)
            {




                if (tQuestInstance.nPlayerIndex != -1)
                {

                    Player *ptPlayer = PlayerGetByPlayerIndex(tQuestInstance.nPlayerIndex);
                    if (ptPlayer && ptPlayer->idPlayerACD != tMsg.idACD1)
                    {
                        return FALSE;
                    }
                }

                ActorCommonData & tACDItem = ACDGet(tMsg.idACD2);
                if (tFailureCondition.snoname1.snoHandle == tACDItem.tSNOName.snoHandle)
                {
                    tQuestInstance.arFailureConditionState[nFailureConditionIndex]++;
                    return TRUE;
                }
            }
            break;
        }

        case FAILURE_CONDITION_TIMED_EVENT_EXPIRED:
        {






            if (tMsg.eMsgType == QUEST_NOTIFICATION_TIMED_EVENT_EXPIRED || tMsg.eMsgType == QUEST_NOTIFICATION_TIMED_EVENT_ABOUT_TO_EXPIRE)
            {





                if (tFailureCondition.snoname1.snoHandle == tMsg.sno1)
                {
                    if (tMsg.eMsgType == QUEST_NOTIFICATION_TIMED_EVENT_EXPIRED)
                    {
                        tQuestInstance.arFailureConditionState[nFailureConditionIndex] = 1;
                        return TRUE;

                    }
                    else
                    {
                        ScriptEventOnQuestTimerRunningOut(QuestGetName(tQuestInstance.snoQuest).str(),
                            QuestStepGetName(tQuestInstance.snoQuest, tQuestInstance.nCurrentStepUID).str(),
                            tMsg.nValue1);
                    }
                }
            }
            break;
        }


        case FAILURE_CONDITION_GAME_FLAG_SET:
        {
            if (tMsg.eMsgType == QUEST_NOTIFICATION_GAME_FLAG_SET)
            {




                int32 nIndex;
                SNO snoGameFlags = GlobalSNOGet(GLOBALSNO_FLAGSET_GAME_FLAGS);
                nIndex = FlagSetLookup(snoGameFlags, tFailureCondition.szParam1_VariableArray);

                if (nIndex == tMsg.nValue1 && tMsg.nValue2 == TRUE)
                {
                    tQuestInstance.arFailureConditionState[nFailureConditionIndex]++;
                    return TRUE;
                }
            }
            break;
        }





        case FAILURE_CONDITION_PLAYER_FLAG_SET:
        {
            if (tMsg.eMsgType == QUEST_NOTIFICATION_PLAYER_FLAG_SET)
            {




                if (tQuestInstance.nPlayerIndex != -1)
                {

                    Player *ptPlayer = PlayerGetByPlayerIndex(tQuestInstance.nPlayerIndex);
                    if (ptPlayer && ptPlayer->idPlayerACD != tMsg.idACD1)
                    {
                        return FALSE;
                    }
                }

                int32 nIndex;
                SNO snoPlayerFlags = GlobalSNOGet(GLOBALSNO_FLAGSET_PLAYER_FLAGS);
                nIndex = FlagSetLookup(snoPlayerFlags, tFailureCondition.szParam1_VariableArray);

                if (nIndex == tMsg.nValue1 && tMsg.nValue2 == TRUE)
                {
                    tQuestInstance.arFailureConditionState[nFailureConditionIndex] = 1;
                    return TRUE;
                }
            }
            break;
        }






        case FAILURE_CONDITION_EVENT_RECEIVED:
        {
            if (tMsg.eMsgType == QUEST_NOTIFICATION_EVENT_RECEIVED)
            {




                if (!strcmp(tFailureCondition.szParam1_VariableArray, tMsg.szValue1))
                {
                    tQuestInstance.arFailureConditionState[nFailureConditionIndex]++;
                    return TRUE;
                }
            }
            break;
        }
    }

    return FALSE;
}













static BOOL sSQuestObjectiveProcessMessage(
    QuestStepObjective & tObjective,
    SNO snoQuest,
    SNO snoLevelArea,
    int32 nCurrentStepUID,
    int32 nPlayerIndex,
    QuestNotificationMessage & tMsg,
    int32 & nObjectiveCounter,
    BOOL bCap,
    BOOL bBonusObjective)
{


    if (tMsg.eMsgType == QUEST_NOTIFICATION_RESEND_TO_CLIENTS)
        return TRUE;



    int32 nObjectiveCounterMax = (bCap ? Math::Max(tObjective.nParam2, 1) : Math::Max(tObjective.nParam2, nObjectiveCounter+1));

    switch (tObjective.eObjectiveType)
    {
        case OBJECTIVE_HAD_CONVERSATION:
        {
            if (tMsg.eMsgType == QUEST_NOTIFICATION_HAD_CONVERSATION)
            {




                if (nPlayerIndex != -1)
                {

                    Player *ptPlayer = PlayerGetByPlayerIndex(nPlayerIndex);
                    if (ptPlayer && ptPlayer->idPlayerACD != tMsg.idACD1)
                    {
                        return FALSE;
                    }
                }

                if (tObjective.snoname1.snoHandle == tMsg.sno1)
                {
                    nObjectiveCounter = 1;



                    if (nPlayerIndex == -1)
                    {
                        sSQuestClearConversationForAllPlayers(tObjective.snoname1.snoHandle);
                    }

                    return TRUE;
                }
            }
            return FALSE;
        }




        case OBJECTIVE_POSSESS_ITEM:
        {
            if (tMsg.eMsgType == QUEST_NOTIFICATION_POSSESS_ITEM)
            {




                if (nPlayerIndex != -1)
                {

                    Player *ptPlayer = PlayerGetByPlayerIndex(nPlayerIndex);
                    if (ptPlayer && ptPlayer->idPlayerACD != tMsg.idACD1)
                    {
                        return FALSE;
                    }
                }

                ActorCommonData *ptACD = ACDTryToGet(tMsg.idACD2);
                if (!ptACD)
                {
                    return FALSE;
                }


                if (tObjective.gbidParam1 == ptACD->hGB.gbid)
                {


                    // the call is kept but nothing depends on it (column 26 = after '!'); 0x2c as in SGizmo.cpp
                    if (!ItemHasLabel(tObjective.gbidParam1, (ItemLabel)0x2c))
                    {
                    }



                    nObjectiveCounter = Math::Min(nObjectiveCounter+1, nObjectiveCounterMax);
                    return TRUE;
                }
            }
            return FALSE;
        }




        case OBJECTIVE_KILL_ALL_MONSTERS_IN_WORLD:
        {
            if (tMsg.eMsgType == QUEST_NOTIFICATION_ACTOR_KILLED || tMsg.eMsgType == QUEST_NOTIFICATION_ACTOR_REMOVED)
            {

                ActorCommonData &tACD = ACDGet(tMsg.idACD2);
                SWorldID idSWorld = tACD.GetSWorldID();
                if (idSWorld != SWORLDID_INVALID)
                {
                    SNO snoWorld = WorldCommonGetWorldSNO(idSWorld);
                    if (snoWorld == tObjective.snoname1.snoHandle)
                    {

                        SWorld *ptSWorld = SWorldGet(idSWorld);
                        ScopedWorldDefinitionLockR pdefWorld(snoWorld);
                        if (pdefWorld->uFlags & (1 << WDF_TREAT_AS_ONE_SCENE))
                        {




                            ptSWorld->bCheckMonstersRemaining = TRUE;
                        }
                    }
                }


            }
            else if (tMsg.eMsgType == QUEST_NOTIFICATION_WORLD_MONSTERS_REMAINING_MODIFIED &&
                tObjective.snoname1.snoHandle == tMsg.sno1)
            {
                int32 nMonsterCount = tMsg.nValue1;
                if (nMonsterCount == 0x7fffffff)
                {
                    nMonsterCount = 0;
                }
                else if (nMonsterCount == 0)
                {
                    nMonsterCount = 0x7fffffff;
                }
                nObjectiveCounter = nMonsterCount;
                return TRUE;
            }
            return FALSE;
        }









        case OBJECTIVE_KILL_MONSTER:
        case OBJECTIVE_KILL_MONSTER_FROM_GROUP:
        case OBJECTIVE_KILL_MONSTER_FROM_FAMILY:
        case OBJECTIVE_KILL_ELITE_PACK_IN_LEVEL_AREA:
        case OBJECTIVE_KILL_ANY_MONSTER_IN_LEVEL_AREA:
        {
            if (tMsg.eMsgType == QUEST_NOTIFICATION_ACTOR_KILLED)
            {




                ActorCommonData &tACD = ACDGet(tMsg.idACD2);







                if (tACD.IsPlayerTeam())
                {
                    return FALSE;
                }

                if (tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_ILLUSION)))
                {
                    return FALSE;
                }

                BOOL bPassed = FALSE;
                if (tObjective.eObjectiveType == OBJECTIVE_KILL_MONSTER_FROM_GROUP)
                {

                    if (tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_SUMMONED_BY_ACDID)) == ACDID_INVALID)
                    {
                        uint32 uHash = ActorGroupNameToHash(tObjective.szParam1_VariableArray);
                        bPassed = (uHash == (uint32)tMsg.nValue1) || (uHash == (uint32)tMsg.nValue2);
                    }
                }
                else if (tObjective.eObjectiveType == OBJECTIVE_KILL_MONSTER_FROM_FAMILY)
                {
                    if (tACD.IsMonster() && MonsterGetMonsterFamily(tACD.tSNOName) == tObjective.nParam1 && tObjective.nParam1 != -1)
                    {
                        bPassed = TRUE;
                    }
                }
                else if (tObjective.eObjectiveType == OBJECTIVE_KILL_ELITE_PACK_IN_LEVEL_AREA)
                {
                    SNO snoActorLevelArea = ACDGetLevelArea(tACD);
                    if (snoActorLevelArea == tObjective.snoname1.snoHandle ||
                        LevelAreaGetParentLevelArea(snoActorLevelArea) == tObjective.snoname1.snoHandle ||
                        (tObjective.snoname1.snoHandle == SNO_NONE && snoActorLevelArea == snoLevelArea))
                    {
                        if (tACD.eMonsterRarity == 1)  // MonsterRarity 1 and 2: no enumerator names in the DWARF
                        {




                            ACDPack *ptACDPack = ACDPackTryToGet(tACD.idACDPack);
                            if (ptACDPack)
                            {
                                int32 nACDsLeft = ptACDPack->GetTotalACDCount(0);
                                bPassed = (nACDsLeft == 0);
                            }
                        }
                        else if (tACD.eMonsterRarity == 2)
                            bPassed = TRUE;
                    }
                }

                else if (tObjective.eObjectiveType == OBJECTIVE_KILL_ANY_MONSTER_IN_LEVEL_AREA && tACD.IsMonster())
                {
                    if (!ActorSNOIsUntargetable(tACD.GetActorSNO()))
                    {
                        SNO snoActorLevelArea = ACDGetLevelArea(tACD);
                        if (snoActorLevelArea == tObjective.snoname1.snoHandle ||
                            LevelAreaGetParentLevelArea(snoActorLevelArea) == tObjective.snoname1.snoHandle)
                        {
                            bPassed = TRUE;
                        }
                    }
                }
                else
                {
                    bPassed = (tObjective.snoname1.snoHandle == tACD.tSNOName.snoHandle);
                }

                if (bPassed)
                {

                    nObjectiveCounter = Math::Min(nObjectiveCounter+1, nObjectiveCounterMax);

                    if (bCap)
                    {


                        ScriptEventOnQuestObjectiveProgressed(QuestGetName(snoQuest).str(),
                            QuestStepGetName(snoQuest, nCurrentStepUID).str(),
                            tMsg.idACD1, nObjectiveCounterMax - nObjectiveCounter);
                    }

                    return TRUE;
                }
            }
            return FALSE;
        }




        case OBJECTIVE_INTERACT_WITH_GIZMO:
        {
            if (tMsg.eMsgType == QUEST_NOTIFICATION_INTERACT_WITH_GIZMO)
            {




                if (nPlayerIndex != -1)
                {

                    Player *ptPlayer = PlayerGetByPlayerIndex(nPlayerIndex);
                    if (ptPlayer && ptPlayer->idPlayerACD != tMsg.idACD1)
                    {
                        return FALSE;
                    }
                }

                ActorCommonData *ptACD = ACDTryToGet(tMsg.idACD2);
                if (!ptACD)
                {
                    return FALSE;
                }

                if (tObjective.snoname1.snoHandle == ptACD->snoActor)
                {

                    nObjectiveCounter = Math::Min(nObjectiveCounter+1, nObjectiveCounterMax);

                    if (bCap)
                    {


                        ScriptEventOnQuestObjectiveProgressed(QuestGetName(snoQuest).str(),
                            QuestStepGetName(snoQuest, nCurrentStepUID).str(),
                            tMsg.idACD1, nObjectiveCounterMax - nObjectiveCounter);
                    }

                    return TRUE;
                }
            }
            return FALSE;
        }




        case OBJECTIVE_ENTER_LEVEL_AREA:
        {
            if (tMsg.eMsgType == QUEST_NOTIFICATION_ENTER_LEVEL_AREA)
            {




                if (nPlayerIndex != -1)
                {

                    Player *ptPlayer = PlayerGetByPlayerIndex(nPlayerIndex);
                    if (ptPlayer && ptPlayer->idPlayerACD != tMsg.idACD1)
                    {
                        return FALSE;
                    }
                }

                if (tObjective.snoname1.snoHandle == tMsg.sno1 ||
                    (tObjective.snoname1.snoHandle == SNO_NONE && tMsg.sno1 == snoLevelArea))
                {
                    nObjectiveCounter = 1;
                    return TRUE;
                }
            }
            return FALSE;
        }





        case OBJECTIVE_ENTER_SCENE:
        {
            if (tMsg.eMsgType == QUEST_NOTIFICATION_ENTER_SCENE)
            {




                if (nPlayerIndex != -1)
                {

                    Player *ptPlayer = PlayerGetByPlayerIndex(nPlayerIndex);
                    if (ptPlayer && ptPlayer->idPlayerACD != tMsg.idACD1)
                    {
                        return FALSE;
                    }
                }

                if (tObjective.snoname1.snoHandle == tMsg.sno1)
                {
                    nObjectiveCounter = 1;
                    return TRUE;
                }
            }
            return FALSE;
        }





        case OBJECTIVE_ENTER_WORLD:
        {
            if (tMsg.eMsgType == QUEST_NOTIFICATION_ENTER_WORLD)
            {




                if (nPlayerIndex != -1)
                {

                    Player *ptPlayer = PlayerGetByPlayerIndex(nPlayerIndex);
                    if (ptPlayer && ptPlayer->idPlayerACD != tMsg.idACD1)
                    {
                        return FALSE;
                    }
                }

                if (tObjective.snoname1.snoHandle == tMsg.sno1)
                {
                    nObjectiveCounter = 1;
                    return TRUE;
                }
            }
            return FALSE;
        }






        case OBJECTIVE_ENTER_TRIGGER:
        {
            if (tMsg.eMsgType == QUEST_NOTIFICATION_TRIGGER)
            {




                ActorCommonData &tACDTrigger = ACDGet(tMsg.idACD2);
                SNO snoTriggerActor = ProximityDetectorBrainDataGetTriggerActorSNO((ActorID)ACDGetOwner(tMsg.idACD2));

                ActorCommonData &tACDTriggerer = ACDGet(tMsg.idACD1);

                if (!ACDIsPlayer(tMsg.idACD1) && tACDTriggerer.snoActor != snoTriggerActor)
                {
                    return FALSE;
                }

                if (nPlayerIndex != -1)
                {

                    Player *ptPlayer = PlayerGetByPlayerIndex(nPlayerIndex);
                    if (ptPlayer && ptPlayer->idPlayerACD != tMsg.idACD1)
                    {
                        return FALSE;
                    }
                }

                if (tObjective.snoname2.snoHandle != SNO_NONE)
                {
                    XPooledDataIDList listACDsInRadius(MemoryPoolsGetDATAIDListPool());

                    BOOL bFound = FALSE;
                    const float PLAYER_FOLLOW_RADIUS = 30.0f;
                    ACDsIntersectSphere(tACDTriggerer.GetPlace(), PLAYER_FOLLOW_RADIUS, ACDID_INVALID, listACDsInRadius, 7);
                    POS pos = listACDsInRadius.GetHeadPosition();
                    while (pos)
                    {
                        ACDID idACDCurrent = (ACDID)listACDsInRadius.GetNext(pos);
                        ActorCommonData * ptACDCurrent = ACDTryToGet(idACDCurrent);
                        if (ptACDCurrent && ptACDCurrent->snoActor == tObjective.snoname2.snoHandle)
                        {
                            bFound = TRUE;
                            break;
                        }
                    }

                    if (!bFound)
                    {
                        return FALSE;

                    }
                }
                uint32 tObjectiveName = MarkerStringHash(tObjective.szParam1_VariableArray);

                if (tObjectiveName != tACDTrigger.tName)
                {







                    Actor *ptActorTrigger = ActorGet((ActorID)tACDTrigger.idOwner);
                    if (ptActorTrigger && ptActorTrigger->tMarkerOwner.nAdventureParentIndex != 0xFFFF)
                    {






                        uint32 tFullMarkerName;
                        tFullMarkerName = MarkerStringHash(CRefString().Format("%s:%s", AdventureGetParentName(ptActorTrigger->tMarkerOwner.nAdventureParentIndex).str(), MarkerNameGetString(tObjectiveName)));


                        if (tFullMarkerName != tACDTrigger.tName)
                        {
                            return FALSE;
                        }
                    }
                    else
                        return FALSE;
                }
                nObjectiveCounter = 1;
                return TRUE;
            }
            return FALSE;
        }
        case OBJECTIVE_COMPLETE_QUEST:
        {
            if (tMsg.eMsgType == QUEST_NOTIFICATION_COMPLETE_QUEST)
            {





                if (tMsg.nValue1 == -1 || tMsg.nValue1 == nPlayerIndex)
                {
                    if (tObjective.snoname1.snoHandle == tMsg.sno1)
                    {
                        nObjectiveCounter = 1;
                        return TRUE;
                    }
                }
            }
            return FALSE;
        }







        case OBJECTIVE_PLAYER_FLAG_SET:
        {
            if (tMsg.eMsgType == QUEST_NOTIFICATION_PLAYER_FLAG_SET)
            {




                if (nPlayerIndex != -1)
                {

                    Player *ptPlayer = PlayerGetByPlayerIndex(nPlayerIndex);
                    if (ptPlayer && ptPlayer->idPlayerACD != tMsg.idACD1)
                    {
                        return FALSE;
                    }
                }

                int32 nIndex;
                SNO snoPlayerFlags = GlobalSNOGet(GLOBALSNO_FLAGSET_PLAYER_FLAGS);
                nIndex = FlagSetLookup(snoPlayerFlags, tObjective.szParam1_VariableArray);

                if (nIndex == tMsg.nValue1 && tMsg.nValue2 == TRUE)
                {
                    nObjectiveCounter = 1;
                    return TRUE;
                }
            }
            return FALSE;
        }





        case OBJECTIVE_GAME_FLAG_SET:
        {
            if (tMsg.eMsgType == QUEST_NOTIFICATION_GAME_FLAG_SET)
            {




                int32 nIndex;
                SNO snoGameFlags = GlobalSNOGet(GLOBALSNO_FLAGSET_GAME_FLAGS);
                nIndex = FlagSetLookup(snoGameFlags, tObjective.szParam1_VariableArray);

                if (nIndex == tMsg.nValue1 && tMsg.nValue2 == TRUE)
                {
                    nObjectiveCounter = 1;
                    return TRUE;
                }
            }
            return FALSE;
        }





        case OBJECTIVE_KILL_GROUP:
        {
            if (tMsg.eMsgType != QUEST_NOTIFICATION_GROUP_KILLED && tMsg.eMsgType != QUEST_NOTIFICATION_GROUP_MODIFIED)
            {
                return FALSE;
            }


            uint32 uHash = ActorGroupNameToHash(tObjective.szParam1_VariableArray);

            if (uHash == (uint32)tMsg.nValue1)
            {
                if (tMsg.eMsgType == QUEST_NOTIFICATION_GROUP_KILLED)
                {
                    nObjectiveCounter = 1;
                    return TRUE;
                }
                else
                {
                    ScriptEventOnQuestObjectiveProgressed(QuestGetName(snoQuest).str(),
                        QuestStepGetName(snoQuest, nCurrentStepUID).str(),
                        ACDID_INVALID, tMsg.nValue2);
                }
            }
            return FALSE;
        }



        case OBJECTIVE_TIMED_EVENT_EXPIRED:
        {
            if (tMsg.eMsgType == QUEST_NOTIFICATION_TIMED_EVENT_EXPIRED || tMsg.eMsgType == QUEST_NOTIFICATION_TIMED_EVENT_ABOUT_TO_EXPIRE)
            {





                if (tObjective.snoname1.snoHandle == tMsg.sno1)
                {
                    if (tMsg.eMsgType == QUEST_NOTIFICATION_TIMED_EVENT_EXPIRED)
                    {
                        nObjectiveCounter = 1;
                        return TRUE;

                    }
                    else
                    {
                        ScriptEventOnQuestTimerRunningOut(QuestGetName(snoQuest).str(),
                            QuestStepGetName(snoQuest, nCurrentStepUID).str(),
                            tMsg.nValue1);
                    }
                }
            }
            return FALSE;
        }


        case OBJECTIVE_EVENT_RECEIVED:
        {
            if (tMsg.eMsgType == QUEST_NOTIFICATION_EVENT_RECEIVED)
            {




                if (!strcmp(tObjective.szParam1_VariableArray, tMsg.szValue1))
                {

                    nObjectiveCounter = Math::Min(nObjectiveCounter+1, nObjectiveCounterMax);

                    if (bCap)
                    {


                        ScriptEventOnQuestObjectiveProgressed(QuestGetName(snoQuest).str(),
                            QuestStepGetName(snoQuest, nCurrentStepUID).str(),
                            ACDID_INVALID, nObjectiveCounterMax - nObjectiveCounter);
                    }

                    return TRUE;
                }
            }
            return FALSE;
        }





        case OBJECTIVE_HQ_ACT_REAGENTS_COLLECTED:
        {
            if (tMsg.eMsgType == QUEST_NOTIFICATION_HQ_REAGENT_COLLECTED && nPlayerIndex != -1)
            {




                Player *ptPlayer = PlayerGetByPlayerIndex(nPlayerIndex);
                if (!ptPlayer || ptPlayer->idPlayerACD != tMsg.idACD1)
                {
                    return FALSE;
                }

                ActorCommonData *ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
                if (!ptACDPlayer)
                {
                    return FALSE;
                }

                int32 nHQActReagentsCollected = ptACDPlayer->AttributesGetInt(MakeAttribKey(ATTRIBUTE_HQ_CURSED_REALM_REAGENTS_COLLECTED, tObjective.nParam1));
                int32 nHQActReagentsDropped = ptACDPlayer->AttributesGetInt(MakeAttribKey(ATTRIBUTE_HQ_CURSED_REALM_REAGENTS_DROPPED, tObjective.nParam1));
                nObjectiveCounter = Math::Pin(nHQActReagentsCollected - nHQActReagentsDropped, 0, nObjectiveCounterMax);
                return TRUE;
            }
            return FALSE;
        }
        default:
            DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }

    return FALSE;
}


static uint64 nGetModifiedQuestXP(ActorCommonData & tACDPlayer, uint64 nBaseQuestXP)
{

    uint64 nXP = nBaseQuestXP;




    int32 nNumNearbyPlayers = 0;
    for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {
        if (tACDPlayer.id != ptPlayer->idPlayerACD && ExperiencePlayersCloseEnough(tACDPlayer, ptPlayer->idPlayerACD))
            nNumNearbyPlayers++;
    }





    if (nNumNearbyPlayers > 0)
    {

        ScopedGlobalDefinitionLockR pdefGlobal;

        float flMPBonusMult = 1.0f + pdefGlobal->ptServerData->flMultiplayerQuestXPMultiplier * (nNumNearbyPlayers - 1);
        nXP = Math::FloatRoundToInt(nBaseQuestXP * flMPBonusMult);
    }

    return nXP;
}
static void sSQuestsResetRewardData(
    QuestInstance & tQuestInstance)
{

    for (int32 i = 0; i < 20; i++)  // 20 = the length of QuestInstance::arBonusRewards (Quests.h:46)
    {
        tQuestInstance.arBonusRewards[i] = 0;
    }


    if (tQuestInstance.eInstanceState == QUEST_INSTANCE_ACTIVE)
    {
        ScopedQuestDefinitionLockR ptQuestDef(tQuestInstance.snoQuest);

        int32 nStepIndex = QuestStepUIDToIndex(ptQuestDef, tQuestInstance.nCurrentStepUID);
        if (nStepIndex >= 0)
        {
            const QuestStep & tQuestStep = ptQuestDef->arQuestSteps[nStepIndex];
            int32 nNumBonusObjectiveSets = tQuestStep.serStepBonusObjectiveSets.dwSize / sizeof(QuestStepBonusObjectiveSet);
            for (int32 i = 0; i < nNumBonusObjectiveSets; i++)
            {
                const QuestStepBonusObjectiveSet & tBonusObjectiveSet = tQuestStep.arStepBonusObjectiveSets[i];
                if (tBonusObjectiveSet.bInverse)
                {
                    if (tBonusObjectiveSet.bRepeatable)
                    {

                        tQuestInstance.arBonusRewards[i] = tBonusObjectiveSet.arStepBonusObjectives[0].nParam2;
                    }
                    else
                    {
                        tQuestInstance.arBonusRewards[i] = 1;
                    }
                }
            }
        }
    }
}



void sSQuestGrantImmediateBonusAward(
    Player & tPlayer,
    SNO snoQuest,
    const QuestStepBonusObjectiveSet & tBonusObjectiveSet,
    int32 nTimesCompleted)
{
    int32 nBonusXPGranted = 0;
    int32 nBonusGoldGranted = 0;
    QuestGetBonusAward(&tPlayer, snoQuest, tBonusObjectiveSet, nBonusXPGranted, nBonusGoldGranted, nTimesCompleted);

    ActorCommonData & tACDPlayer = ACDGet(tPlayer.idPlayerACD);
    BOOL bAwarded = FALSE;
    BOOL fAwardedXPToPlayer = FALSE;


    if (nBonusXPGranted > 0)
    {

        uint64 nXP = nGetModifiedQuestXP(tACDPlayer, nBonusXPGranted);
        if (nXP > 0)
        {
            fAwardedXPToPlayer = ExperienceGrantDirectly(tACDPlayer, nXP, TRUE, FALSE);
        }
        else
        {
            nBonusXPGranted = 0;
        }
        bAwarded = TRUE;
    }

    if (nBonusGoldGranted > 0)
    {

        tACDPlayer.ModifyCurrencyAmount(nBonusGoldGranted, CURRENCY_GOLD, 3, FALSE);
        bAwarded = TRUE;
    }

    if (!bAwarded || QuestGetQuestType(snoQuest) == QUEST_TYPE_CHALLENGE) return;  // both tests on line 2453 in the original
    {
        FloatingAmountMessage tMessage;
        tMessage.wpPlace = tACDPlayer.GetWorldPlaceCenter();

        PlayerList listDestPlayers(PlayerListGetElementPool());
        listDestPlayers.AddTail_Tracked(&tPlayer);


        if (nBonusXPGranted > 0 && nBonusGoldGranted > 0)
        {
            tMessage.eType = FLOATING_NUMBER_TYPE_BONUS_QUEST_REWARD;
            tMessage.nAmount = nBonusXPGranted;
            tMessage.nAmount2 = nBonusGoldGranted;
        }

        else if (fAwardedXPToPlayer && nBonusXPGranted > 0)
        {
            tMessage.nAmount = nBonusXPGranted;
            tMessage.eType = FLOATING_NUMBER_TYPE_XP;
        }

        else if (nBonusGoldGranted > 0)
        {
            tMessage.nAmount = nBonusGoldGranted;
            tMessage.eType = FLOATING_NUMBER_TYPE_GOLD;
        } else { return; }
        MessageSendToClient(listDestPlayers, SMSG_FLOATING_AMOUNT, &tMessage, sizeof(tMessage));
    }
}


void SQuestEvaluateGoldRewardedAchievementEvent(
    const Player & tPlayer,
    uint32 nGoldGranted)
{



    // Columns 2494:31/2495:19/2501:2 show this inner block is tab-indented in the original.
	{
		Achievement::EvaluationData tEvaluationData;
		tEvaluationData.SetPlayer(&tPlayer);

		tEvaluationData.EvaluateEvent(&tPlayer, Achievement::ACH_EVENT_REWARDED_GOLD, nGoldGranted);



	}
}



void sSQuestCheckForBonusReward(
    Player & tPlayer,
    const QuestInstance & tQuestInstance,
    const QuestStep & tBonusStep)
{
    BOOL bAwarded = FALSE;


    int32 nBonusXPGranted = 0;
    int32 nBonusGoldGranted = 0;

    int32 nNumBonusObjectiveSets = tBonusStep.serStepBonusObjectiveSets.ElementCount(sizeof(QuestStepBonusObjectiveSet));
    for (int32 i = 0; i < nNumBonusObjectiveSets; i++)
    {
        const QuestStepBonusObjectiveSet & tBonusObjectiveSet = tBonusStep.arStepBonusObjectiveSets[i];


        if (!tBonusObjectiveSet.bInverse)
            continue;



        int32 nTimesCompleted = tQuestInstance.arBonusRewards[i];
        if (nTimesCompleted > 0)
        {
            QuestGetBonusAward(&tPlayer, tQuestInstance.snoQuest, tBonusObjectiveSet, nBonusXPGranted, nBonusGoldGranted, nTimesCompleted);
        }
    }

    ActorCommonData & tACDPlayer = ACDGet(tPlayer.idPlayerACD);
    BOOL fAwardedXPToPlayer = FALSE;


    if (nBonusXPGranted > 0)
    {

        uint64 nXP = nGetModifiedQuestXP(tACDPlayer, nBonusXPGranted);
        if (nXP > 0)
        {
            fAwardedXPToPlayer = ExperienceGrantDirectly(tACDPlayer, nXP, TRUE, FALSE);
        }
        else
        {
            nBonusXPGranted = 0;
        }
        bAwarded = TRUE;
    }

    if (nBonusGoldGranted > 0)
    {

        tACDPlayer.ModifyCurrencyAmount(nBonusGoldGranted, CURRENCY_GOLD, 3, FALSE);
        SGameStatsPlayerCounterIncrement(&tPlayer, STAT_COUNTER_PLAYER_CURRENCY_PICKED_UP, 0, 0, (float)nBonusGoldGranted);
        SGameStatsPlayerCounterIncrement(&tPlayer, STAT_COUNTER_PLAYER_CURRENCY_RECEIVED_FROM_QUESTS, 0, 0, (float)nBonusGoldGranted);
        SGameStatsPlayerCounterIncrement(&tPlayer, STAT_COUNTER_PLAYER_CURRENCY_RECEIVED_FROM_QUESTS_BONUS, 0, 0, (float)nBonusGoldGranted);
        SQuestEvaluateGoldRewardedAchievementEvent(tPlayer, nBonusGoldGranted);


        tPlayer.tConsoleCounters.uGoldPickedUp += nBonusGoldGranted;
        bAwarded = TRUE;
    }

    if (!bAwarded) return;
    {
        FloatingAmountMessage tMessage;
        tMessage.wpPlace = tACDPlayer.GetWorldPlaceCenter();

        PlayerList listDestPlayers(PlayerListGetElementPool());
        listDestPlayers.AddTail_Tracked(&tPlayer);


        if (nBonusXPGranted > 0 && nBonusGoldGranted > 0)
        {
            tMessage.eType = FLOATING_NUMBER_TYPE_BONUS_QUEST_REWARD;
            tMessage.nAmount = nBonusXPGranted;
            tMessage.nAmount2 = nBonusGoldGranted;
        }

        else if (fAwardedXPToPlayer && nBonusXPGranted > 0)
        {
            tMessage.nAmount = nBonusXPGranted;
            tMessage.eType = FLOATING_NUMBER_TYPE_XP;
        }

        else if (nBonusGoldGranted > 0)
        {
            tMessage.nAmount = nBonusGoldGranted;
            tMessage.eType = FLOATING_NUMBER_TYPE_GOLD;
        } else { return; }
        MessageSendToClient(listDestPlayers, SMSG_FLOATING_AMOUNT, &tMessage, sizeof(tMessage));
    }
}

// DWARF SQuests.cpp:2600, static (_ZL). Locals and lines from the DWARF; structure inferred from the line table.
static void sSQuestsStepRewardPlayer(
    Player *ptPlayer,
    QuestInstance &tQuestInstance,
    int32 nCompletedStepUID)
{
    SGameGlobals &tSGameGlobals = SGameGlobalsGet();
    if (tSGameGlobals.dwFlags2 & (1 << SGAMEGLOBALS2_EXPLOIT_GAME_SHUT_DOWN))
    {
        return;
    }

    ActorCommonData &tACDPlayer = ACDGet(ptPlayer->idPlayerACD);














    // 2629/2735/2809: the branch sits at column 10, one past the 4-space indent; 5-space indent assumed (as at 3226/3325).
    ScopedQuestDefinitionLockR ptQuestDef(tQuestInstance.snoQuest);
    int32 nStepIndex = QuestStepUIDToIndex(ptQuestDef, nCompletedStepUID);
     if (nStepIndex < 0)
    {
        return;
    }
    QuestStep &tQuestStep = ptQuestDef->arQuestSteps[nStepIndex];

    uint64 nXPGranted;
    int32 nGoldGranted;
    SNO snoRecipe;
    BOOL bReplayReward;
    BOOL bAddNVStack;

    LooteeInfo tLootee;
    tLootee.flNoDropScalar = 1.0f;
    tLootee.tDebugName = MarkerStringHash("Quest");
    tLootee.hGBDebug = cghgbNone;
    tLootee.eQuestAct = ACT_INVALID;

    QuestType eQuestType = QuestGetQuestType(tQuestInstance.snoQuest);
    if (eQuestType == QUEST_TYPE_BOUNTY)
    {
        tLootee.eQuestAct = QuestGetBountyAct(tQuestInstance.snoQuest);
    }
    else
    {
        tLootee.eQuestAct = QuestGetAct(tQuestInstance.snoQuest);
    }

    if (!QuestStepGetReward(*ptPlayer, tQuestInstance.snoQuest, tQuestStep, nXPGranted, nGoldGranted, snoRecipe, tLootee.snoTreasureClass, tLootee.tLooteeParams.nLevel, bReplayReward, bAddNVStack))
    {
        return;
    }

    D3::Quests::QuestReward tReward;
    tReward.Clear();

    BOOL bSendRewardMsg = FALSE;
    ACDID idACDItemSingle = ACDID_INVALID;

    if (snoRecipe != SNO_NONE)
    {
        idACDItemSingle = SItemCrafting_GenerateItemFromRecipe(snoRecipe, ptPlayer);
    }
    else if (tLootee.snoTreasureClass != SNO_NONE)
    {
        LootSpecifierArray arLootSpecifiers;
        if (LootGetLootSpecifiers(tLootee, tACDPlayer.id, arLootSpecifiers, NULL, FALSE, 1, NULL))
        {
            int32 nNumItems = arLootSpecifiers.GetArraySize();
            if (nNumItems > 0)
            {

                if (!GameIsOpenWorld())
                {








                    arLootSpecifiers[0].fAutoIdentify = TRUE;


                    idACDItemSingle = LootCreateItem(arLootSpecifiers[0], cgplaceNull, NULL);
                }
                else
                {
                    for (int32 i = 0; i < nNumItems; i++)
                    {
                        arLootSpecifiers[i].fAutoIdentify = TRUE;

                        ACDID idACDItemLocal = LootCreateItem(arLootSpecifiers[i], cgplaceNull, NULL);
                        if (idACDItemLocal != ACDID_INVALID)
                        {
                            ActorCommonData &tACDItem = ACDGet(idACDItemLocal);

                            if (ItemGetUseType(tACDItem) == ITEM_USE_TREASURE_BAG)
                            {

                                tACDItem.AttributesSetInt(ATTRIBUTE_ITEM_LEGENDARY_ITEM_LEVEL_OVERRIDE, tLootee.tLooteeParams.nLevel);
                            }

                            if (ItemIsCurrency(tACDItem.hGB.gbid))
                            {
                                SGameStatsPlayerCounterIncrement(ptPlayer, STAT_COUNTER_PLAYER_SPECIAL_CURRENCY_RECEIVED_FROM_QUEST, tQuestInstance.snoQuest, tACDItem.hGB.gbid, (float)tACDItem.GetStackCount());
                            }
                            // 2719: the sub sits at column 99, ours 97; spelling not found.
                            tReward.add_items();
                            ItemGeneratorToDefaults(*tReward.mutable_items(tReward.items_size() - 1));
                            ItemGeneratorPopulateFromACDID(tACDItem, *tReward.mutable_items(tReward.items_size() - 1), FALSE);

                            bSendRewardMsg = TRUE;
                            SACDInventoryPickupOrSpillOntoGround(tACDPlayer, tACDItem, FALSE, NULL, TRUE);
                        }
                    }
                }
            }
        }

    }

    BOOL bEquipped = FALSE;


     if (idACDItemSingle != ACDID_INVALID)
    {


        ActorCommonData &tACDItem = ACDGet(idACDItemSingle);
        ItemGeneratorToDefaults(*tReward.mutable_item());
        ItemGeneratorPopulateFromACDID(tACDItem, *tReward.mutable_item(), FALSE);

        bSendRewardMsg = TRUE;
        SACDInventoryPickupOrSpillOntoGround(tACDPlayer, tACDItem, FALSE, NULL, TRUE);

        const InventoryLocation *ptInvLoc = tACDItem.TryToGetInventoryLocation();
        if (ptInvLoc &&
            ptInvLoc->eSlot >= SLOT_PLAYER_BODYSLOT_START && ptInvLoc->eSlot <= SLOT_PLAYER_BODYSLOT_END)
        {
            bEquipped = TRUE;
        }
    }




    if (nXPGranted)
    {
        uint64 nXP = nGetModifiedQuestXP(tACDPlayer, nXPGranted);
        ExperienceGrantDirectly(tACDPlayer, nXP, TRUE, FALSE);
        tReward.set_xp_granted(nXP);
        tReward.set_bonus_xp_granted(ExperienceGetRestXPToGrant(tACDPlayer, nXP));
        bSendRewardMsg = TRUE;
    }

    if (nGoldGranted > 0)
    {
        tACDPlayer.ModifyCurrencyAmount(nGoldGranted, CURRENCY_GOLD, 3, FALSE);
        tReward.set_gold_granted(nGoldGranted);
        SQuestEvaluateGoldRewardedAchievementEvent(*ptPlayer, nGoldGranted);

        // Columns 117/128 fit CURRENCY_GOLD (not 0) as the third argument.
        SGameStatsPlayerCounterIncrement(ptPlayer, STAT_COUNTER_PLAYER_CURRENCY_PICKED_UP, CURRENCY_GOLD, 0, (float)nGoldGranted);
        SGameStatsPlayerCounterIncrement(ptPlayer, STAT_COUNTER_PLAYER_CURRENCY_RECEIVED_FROM_QUESTS, CURRENCY_GOLD, 0, (float)nGoldGranted);
        if (!bReplayReward)
        {
            SGameStatsPlayerCounterIncrement(ptPlayer, STAT_COUNTER_PLAYER_CURRENCY_RECEIVED_FROM_QUESTS_FIRST_TIME, CURRENCY_GOLD, 0, (float)nGoldGranted);
        }
        else
        {
            SGameStatsPlayerCounterIncrement(ptPlayer, STAT_COUNTER_PLAYER_CURRENCY_RECEIVED_FROM_QUESTS_REPEAT, CURRENCY_GOLD, 0, (float)nGoldGranted);
        }


        ptPlayer->tConsoleCounters.uGoldPickedUp += nGoldGranted;
        bSendRewardMsg = TRUE;
    }





















     if (bSendRewardMsg)
    {
        tReward.set_sno_quest(tQuestInstance.snoQuest);
        SQuestsSendQuestReward(*ptPlayer, (tQuestInstance.eInstanceState == QUEST_INSTANCE_COMPLETED), tReward, bEquipped);



        SPlayerReceivedQuestStepReward(*ptPlayer, tQuestInstance.snoQuest, nCompletedStepUID, GameGetDifficultyLevel());
    }


    sSQuestCheckForBonusReward(*ptPlayer, tQuestInstance, tQuestStep);

    if (bAddNVStack)
    {
        SPlayerAddNephalemBuffStack(ptPlayer->idPlayerACD);
    }
}


static BOOL sSQuestsIsOpenWorldBountyActComplete(
    SNO snoQuest)
{
    static const GlobalSNO sctBountyTurninQuests[] =
    {
        GLOBALSNO_QUEST_BOUNTY_TURNIN_A1,
        GLOBALSNO_QUEST_BOUNTY_TURNIN_A2,
        GLOBALSNO_QUEST_BOUNTY_TURNIN_A3,
        GLOBALSNO_QUEST_BOUNTY_TURNIN_A4,
        GLOBALSNO_QUEST_BOUNTY_TURNIN_A5,
    };
    if (!GameIsOpenWorld())
        return FALSE;



    for (int32 i = 0; i < (int32)(sizeof(sctBountyTurninQuests) / sizeof(sctBountyTurninQuests[0])); i++)
    {
        if (snoQuest == GlobalSNOGet(sctBountyTurninQuests[i]))
            return TRUE;
    }

    return FALSE;
}




static void sUpdateQuestMarkersAndActors()
{



    BOOL bMarkerModified = FALSE;

    SceneCommonData *ptSCD = NULL;
    SceneCommonDataIterator tIterator;
    while (SceneCommonDataArrayIterate(tIterator, ptSCD, SkipHolodeckScenes))
    {
        if (ptSCD->ptMarkerScene && ptSCD->ptMarkerScene->ptInactiveMarkerList)
        {
            POS pos = ptSCD->ptMarkerScene->ptInactiveMarkerList->GetHeadPosition();
            while (pos)
            {
                InactiveBaseMarker *ptInactiveMarker = ptSCD->ptMarkerScene->ptInactiveMarkerList->GetNext(pos);
                // columns from line 2876 on are one level deeper: a scope block with no code of its own
                {
                    SNO snoMarkerSet = ptInactiveMarker->tMarkerOwner.snoMarkerSet;
                    int32 nMarkerIndex = ptInactiveMarker->tMarkerOwner.nMarkerIndex;
                    if (snoMarkerSet != SNO_NONE)
                    {
                        ScopedMarkerSetDefinitionLockR ptMarkerSet(snoMarkerSet);
                        if (nMarkerIndex >= 0 && nMarkerIndex < ptMarkerSet->tMarkerSet.GetArraySize())
                        {
                            const Marker & tMarker = ptMarkerSet->tMarkerSet[nMarkerIndex];
                            SNO snoQuestRange = TagMapGetSNO(tMarker.hTagMap, TAG_MARKER_QUESTRANGE);
                            if (snoQuestRange != SNO_NONE)
                            {
                                QuestRangeResult result;
                                if (QuestRangeEvaluate(snoQuestRange, &result))
                                {
                                    if (ptInactiveMarker->nFlags & (1 << INACTIVE_MARKER_FLAG_WAITING_ON_QUEST))
                                    {
                                        ptInactiveMarker->nFlags &= ~(1 << INACTIVE_MARKER_FLAG_WAITING_ON_QUEST);

                                        if (AppDrawFlagIsSet(APP_DRAW_DEBUG_QUESTS_BIT))
                                        {
                                            // tMarkerName (DWARF 2898) feeds debug output that is compiled out

                                            uint32 tMarkerName;
                                            InactiveMarkerGetMarkerName(ptInactiveMarker, tMarkerName);
                                        }
                                        bMarkerModified = TRUE;
                                    }
                                }
                                else
                                {



                                    if (!(ptInactiveMarker->nFlags & (1 << INACTIVE_MARKER_FLAG_WAITING_ON_QUEST)))
                                    {
                                        ptInactiveMarker->nFlags |= (1 << INACTIVE_MARKER_FLAG_WAITING_ON_QUEST);

                                        if (AppDrawFlagIsSet(APP_DRAW_DEBUG_QUESTS_BIT))
                                        {
                                            // tMarkerName (DWARF 2917) feeds debug output that is compiled out

                                            uint32 tMarkerName;
                                            InactiveMarkerGetMarkerName(ptInactiveMarker, tMarkerName);
                                        }
                                        bMarkerModified = TRUE;
                                    }
                                }
                            }
                        }




                    }
                }
            }
        }
    }

    ActorDeleteQuestLifespanActors();
    Minimap::UpdateMarkersForQuestState();

    if (bMarkerModified)
    {

        ServerQueueClientUpdate();

        GameCommonData & tGCD = GameGetActiveGameCommonData();
        QuestGlobals *& ptQuestGlobals = tGCD.ptQuestGlobals;
        ptQuestGlobals->bForcePlayerLocationTriggerEventAfterActivationUpdate = TRUE;
    }
}


static SNO sGetQuestStepPowerToGrant(
    const QuestDefinition & tDefQuest,
    int32 nStepUID)
{
    SNO snoPower = SNO_NONE;
    int32 nStepIndex = QuestStepUIDToIndex(&tDefQuest, nStepUID);
    if (nStepIndex >= 0)
    {
        // the DWARF has a local int32 nNumSteps here (no location, no code); its initialiser is unknown

        snoPower = tDefQuest.arQuestSteps[nStepIndex].snoPowerGranted;
    }
    return snoPower;
}



void sEvaluateQuestStepCompletedAchievement(const QuestInstance & tQuestInstance, int32 nCompletedStepUID)
{
    {







        PlayerList listDestPlayers(PlayerListGetElementPool());

        if (tQuestInstance.nPlayerIndex == -1)
        {
            for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
            {
                listDestPlayers.AddTail_Tracked(ptPlayer);
            }
        }
        else
        {

            listDestPlayers.AddTail_Tracked(PlayerGetByPlayerIndex(tQuestInstance.nPlayerIndex));
        }

        POS pos = listDestPlayers.GetHeadPosition();
        while (pos)
        {
            Player * ptPlayer = listDestPlayers.GetNext(pos);
            if (ptPlayer)
            {
                Achievement::EvaluationData tEvaluationData;
                tEvaluationData.SetPlayer(ptPlayer);
                tEvaluationData.EvaluateEvent(ptPlayer, Achievement::ACH_EVENT_COMPLETE_QUEST_STEP, SGameStatsEncodeSNODataToID(tQuestInstance.snoQuest, nCompletedStepUID), 1);
            }
        }
    }
}







void SQuestHandleQuestAdvancement(
    QuestInstance & tQuestInstance,
    int32 nCompletedStepUID,
    int32 nStepUID,
    BOOL bStartup)
{
    if (QuestIsGameOver())
    {

        nStepUID = -1;
    }
    if (nStepUID == nCompletedStepUID)
        return;




    QuestGlobals *ptQuestGlobals = QuestsGetQuestGlobals();

    BOOL bResetToFalse = bStartup && !ptQuestGlobals->bStartup;
    if (bStartup)
    {
        ptQuestGlobals->bStartup = TRUE;
    }
    else
    {
        bStartup = ptQuestGlobals->bStartup;
    }

    SGameStatsTimedEventFinish(STAT_COUNTER_GAME_QUEST_STEP_DURATION, tQuestInstance.snoQuest, SGameStatsEncodeSNODataToID(tQuestInstance.snoQuest, nCompletedStepUID));

    if (!QuestStepIsCompletionStep(tQuestInstance.snoQuest, nStepUID))
    {
        SGameStatsTimedEventStart(STAT_COUNTER_GAME_QUEST_STEP_DURATION, tQuestInstance.snoQuest, SGameStatsEncodeSNODataToID(tQuestInstance.snoQuest, nStepUID), TRUE);
    }


    if (tQuestInstance.nPlayerIndex == -1)
    {
        SQuestsSendUpdateToAllPlayers(tQuestInstance.snoQuest);
        ScriptEventOnQuestStepCompleted(QuestGetName(tQuestInstance.snoQuest).str(), QuestStepGetName(tQuestInstance.snoQuest, nCompletedStepUID).str(), ACDID_INVALID);
        ScriptEventOnQuestStepAssigned(QuestGetName(tQuestInstance.snoQuest).str(), QuestStepGetName(tQuestInstance.snoQuest, nStepUID).str(), ACDID_INVALID);
        ScriptEventOnQuestRangeChanged(tQuestInstance.snoQuest, nCompletedStepUID, nStepUID);
    }
    else
    {
        Player *ptPlayer = PlayerGetByPlayerIndex(tQuestInstance.nPlayerIndex);
        if (ptPlayer && ptPlayer->idGameConnection != -1)
        {
            SQuestsSendQuestStateToPlayer(tQuestInstance, *ptPlayer, TRUE);
            ScriptEventOnQuestStepCompleted(QuestGetName(tQuestInstance.snoQuest).str(), QuestStepGetName(tQuestInstance.snoQuest, nCompletedStepUID).str(), ptPlayer->idPlayerACD);
            ScriptEventOnQuestStepAssigned(QuestGetName(tQuestInstance.snoQuest).str(), QuestStepGetName(tQuestInstance.snoQuest, nStepUID).str(), ptPlayer->idPlayerACD);
        }
    }

	sEvaluateQuestStepCompletedAchievement(tQuestInstance, nCompletedStepUID);


    ScopedQuestDefinitionLockR ptQuestDef(tQuestInstance.snoQuest);
    SNO snoPowerToGrant = sGetQuestStepPowerToGrant(*ptQuestDef, nStepUID);
    SNO snoPowerToRemove = sGetQuestStepPowerToGrant(*ptQuestDef, nCompletedStepUID);





    if (snoPowerToRemove != snoPowerToGrant && snoPowerToRemove != SNO_NONE)
    {
        ActorPowersRemoveGlobalPower(snoPowerToRemove, tQuestInstance.nPlayerIndex);
    }
    if (snoPowerToGrant != SNO_NONE)
    {
        ActorPowersGrantGlobalPower(snoPowerToGrant, tQuestInstance.nPlayerIndex);
    }

    if (tQuestInstance.nPlayerIndex == -1)
    {

        if (QuestStepTrackPerPlayer(tQuestInstance.snoQuest, nStepUID))
        {

            for (Player *ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
            {
                QuestInstance tPlayerInstance;
                QuestsQuestInstanceInitialize(tPlayerInstance, tQuestInstance.snoQuest, SNO_NONE);
                QuestsAdvanceToStep(tPlayerInstance, nStepUID, FALSE);
                tPlayerInstance.nPlayerIndex = ptPlayer->nPlayerIndex;
                ptQuestGlobals->tQuestPerPlayerInstanceList.AddTail_Tracked(tPlayerInstance);
            }
        }
    }

    if (ptQuestDef->eQuestType == QUEST_TYPE_MAIN_QUEST)
    {
        ptQuestGlobals->snoCurrentMainLineQuest = tQuestInstance.snoQuest;
        ptQuestGlobals->nCurrentMainLineQuestStepUID = nStepUID;



        if (!bStartup &&
            (nCompletedStepUID == QUEST_UNASSIGNED_STEP_UID || QuestStepIsSelectable(tQuestInstance.snoQuest, nStepUID)))
        {
            ptQuestGlobals->nCurrentMainLineQuestHandicapLevel = GameGetHandicapLevel();
        }
    }


    if (ptQuestDef->eQuestType == QUEST_TYPE_BOUNTY)
    {
        ScopedQuestDefinitionLockR pdefQuest(tQuestInstance.snoQuest);

        int32 nStepIndex =
                     QuestStepUIDToIndex(
                         pdefQuest,
            tQuestInstance.nCurrentStepUID);

        if (nStepIndex >= 0)
        {
            const QuestStep & tStep = pdefQuest->arQuestSteps[nStepIndex];
            int32 nNumObjectiveSets = tStep.serStepObjectiveSets.ElementCount(sizeof(QuestStepObjectiveSet));

            for (int32 j = 0; j < nNumObjectiveSets; j++)
            {
                const QuestStepObjectiveSet & tObjectiveSet = tStep.arStepObjectiveSets[j];
                int32 nNumObjectives = tObjectiveSet.serStepObjectives.ElementCount(sizeof(QuestStepObjective));

                BOOL bKillBoss = FALSE;

                for (int32 k = 0; k < nNumObjectives; k++)
                {
                    const QuestStepObjective & tObjective = tObjectiveSet.arStepObjectives[k];
                    if (tObjective.eObjectiveType == OBJECTIVE_KILL_MONSTER)
                    {

                        bKillBoss = TRUE;
                        break;
                    }
                }
                for (int32 k = 0; k < nNumObjectives; k++)
                {
                    const QuestStepObjective & tObjective = tObjectiveSet.arStepObjectives[k];
                    if ((tObjective.eObjectiveType == OBJECTIVE_KILL_MONSTER || tObjective.eObjectiveType == OBJECTIVE_INTERACT_WITH_GIZMO) &&
                        pdefQuest->tBountyData.snoRequiredActorWorld != SNO_NONE)
                    {
                        SNO snoActor = tObjective.snoname1.snoHandle;
                        if (snoActor != SNO_NONE)
                        {
                            SNO snoWorld = pdefQuest->tBountyData.snoRequiredActorWorld;
                            SNO snoLevelArea = pdefQuest->tBountyData.snoRequiredActorLevelArea;

                            ACDID idACD = ActivateActorMarkerInWorld(snoActor, snoWorld, snoLevelArea);

                            if (idACD != ACDID_INVALID)
                            {
                                Minimap::ActorAutoPing(idACD, tQuestInstance.snoQuest);
                            }
                            else
                            {

                                SQuestsRegisterBountyActor(snoActor, snoWorld, snoLevelArea, tQuestInstance.snoQuest);
                            }
                        }
                    }
                    else if (tObjective.eObjectiveType == OBJECTIVE_COMPLETE_QUEST)
                    {
                        if (pdefQuest->tBountyData.snoRequiredQuestWorld != SNO_NONE)
                        {

                            SWorldID idSWorld = WorldCommonFindBySNO(pdefQuest->tBountyData.snoRequiredQuestWorld, TRUE);
                            if (idSWorld == SWORLDID_INVALID)
                            {
                                idSWorld = SWorldCreate(pdefQuest->tBountyData.snoRequiredQuestWorld, TRUE, FALSE, NULL, ACDID_INVALID, ACDID_INVALID, 0, WARP_UNKNOWN, FALSE);
                            }

                            SNO snoQuest = tObjective.snoname1.snoHandle;
                            ScopedQuestDefinitionLockR pdefObjectiveQuest(snoQuest);
                            if (pdefObjectiveQuest->eQuestType == QUEST_TYPE_CHALLENGE)
                            {
                                Minimap::ChallengeAutoPing(snoQuest, tQuestInstance.snoQuest);
                            }
                            else if (pdefObjectiveQuest->eQuestType == QUEST_TYPE_EVENT)
                            {
                                Minimap::EventAutoPing(snoQuest, tQuestInstance.snoQuest);
                            }
                        }
                    }
                    else if (tObjective.eObjectiveType == OBJECTIVE_ENTER_LEVEL_AREA)
                    {
                        SNO snoLevelArea = tObjective.snoname1.snoHandle;
                        if (snoLevelArea != SNO_NONE)
                        {
                            Minimap::SNOAddAutoPingRef(snoLevelArea, LEVELAREA_SNO, tQuestInstance.snoQuest, bKillBoss);
                        }
                    }
                }
            }
        }
    }



    if (nCompletedStepUID == QUEST_UNASSIGNED_STEP_UID && ptQuestDef->eQuestType == QUEST_TYPE_EVENT)
    {
        PingID idPing = Minimap::SNOGetAutoPing(tQuestInstance.snoQuest);
        Minimap::RemovePing(idPing);
    }

    if (!bStartup)
    {


        ptQuestGlobals->bResendEnterEvents = TRUE;

        SGameGlobals &tSGameGlobals = SGameGlobalsGet();
         if (!(tSGameGlobals.dwFlags & (1 << SGAMEGLOBALS_QUEST_ADVANCED_BIT)) &&
             ptQuestDef->eQuestType == QUEST_TYPE_MAIN_QUEST)
        {



            if (QuestIsComplete(tQuestInstance) || QuestStepIsResumable(tQuestInstance.snoQuest, nStepUID))
            {
                tSGameGlobals.dwFlags |= (1 << SGAMEGLOBALS_QUEST_ADVANCED_BIT);
            }
        }

        if (tQuestInstance.nPlayerIndex == -1)
        {
            for (Player *ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
            {
                if (!(ptPlayer->uServerFlags & (1 << PSF_ELIGIBLE_FOR_QUEST_STATE)) &&
                     ptQuestDef->eQuestType == QUEST_TYPE_MAIN_QUEST)
                {



                    if (QuestIsComplete(tQuestInstance) || QuestStepIsResumable(tQuestInstance.snoQuest, nStepUID))
                    {
                        ptPlayer->uServerFlags |= (1 << PSF_ELIGIBLE_FOR_QUEST_STATE);
                    }
                }

                BOOL bGiveReward = TRUE;

                if (ptQuestDef->eQuestType == QUEST_TYPE_HORADRIC_QUEST) {}
                else if (ptQuestDef->eQuestType == QUEST_TYPE_BOUNTY) {
                    ActorCommonData *ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
                    if (ptACDPlayer)
                    {
                        SNO snoLevelArea = ACDGetLevelArea(*ptACDPlayer);
                        if (tQuestInstance.snoLevelArea != snoLevelArea &&
                            !QuestHasAdditionalLevelArea(tQuestInstance.snoQuest, snoLevelArea))
                        {
                            if (!ACDIsInTown(*ptACDPlayer))
                            {
                                bGiveReward = FALSE;
                            }
                            else
                            {
                                SNO snoTownPortalLevelArea = ptPlayer->tHearthPortalInfo.snoPortalLevelArea;
                                if (snoTownPortalLevelArea == SNO_NONE ||
                                    (tQuestInstance.snoLevelArea != snoTownPortalLevelArea &&
                                     !QuestHasAdditionalLevelArea(tQuestInstance.snoQuest, snoTownPortalLevelArea)))
                                {
                                    bGiveReward = FALSE;
                                }
                            }
                        }
                    }
                }






                else if (tQuestInstance.snoQuest == GlobalSNOGet(GLOBALSNO_QUEST_DUNGEONFINDER) ||
                        tQuestInstance.snoQuest == GlobalSNOGet(GLOBALSNO_QUEST_TIERED_RIFT_CHALLENGE))
                {

                    bGiveReward = NephalemRiftManager::PlayerPaidForRift(ptPlayer);


                    if (NephalemRiftManager::IsTieredRiftActive())
                    {
                        bGiveReward = FALSE;
                    }
                }

                if (bGiveReward)
                {
                    sSQuestsStepRewardPlayer(ptPlayer, tQuestInstance, nCompletedStepUID);
                }

                SPlayerUpdateQuestHistory(*ptPlayer, tQuestInstance.snoQuest, tQuestInstance.nCurrentStepUID);

                if (tQuestInstance.eInstanceState == QUEST_INSTANCE_COMPLETED)
                {
                    SGameStatsPlayerCompletedQuest(ptPlayer->idPlayerACD, tQuestInstance.snoQuest, ptQuestDef->eQuestType, bGiveReward);
                }
                else if (nCompletedStepUID == QUEST_UNASSIGNED_STEP_UID)
                {
                    SGameStatsPlayerStartedQuest(ptPlayer->idPlayerACD, tQuestInstance.snoQuest, ptQuestDef->eQuestType);
                }


                if (sSQuestsIsOpenWorldBountyActComplete(tQuestInstance.snoQuest))
                {
                    SGameGlobalsGet().dwFlags2 |= (1 << SGAMEGLOBALS2_ADVENTURE_MODE_BOUNTY_ACT_COMPLETE);
                }
            }


             if (ptQuestDef->eQuestType == QUEST_TYPE_MAIN_QUEST)
            {
                SGameInitWaypointsFromStartData(0, TRUE);
            }



            if (tQuestInstance.eInstanceState == QUEST_INSTANCE_COMPLETED &&
                ptQuestDef->eQuestType == QUEST_TYPE_BOUNTY &&
                !sSQuestsIsOpenWorldBountyActComplete(tQuestInstance.snoQuest))
            {
                int32 nPlayersInBountyLevelArea = 0;
                int32 nPlayersInGame = 0;

                for (Player *ptPlayerIt = PlayerGetFirstInGame(); ptPlayerIt; ptPlayerIt = PlayerGetNextInGame(ptPlayerIt))
                {
                    ActorCommonData *ptACDPlayer = ACDTryToGet(ptPlayerIt->idPlayerACD);
                    if (ptACDPlayer)
                    {
                        BOOL bPlayerIsInBountyLevelArea = FALSE;


                        SNO snoPlayerLevelArea = ACDGetLevelArea(*ptACDPlayer);
                        if (snoPlayerLevelArea == tQuestInstance.snoLevelArea ||
                            QuestHasAdditionalLevelArea(tQuestInstance.snoQuest, snoPlayerLevelArea))
                        {
                            bPlayerIsInBountyLevelArea = TRUE;
                        }


                        SNO snoPlayerHearthPortalLevelArea = ptPlayerIt->tHearthPortalInfo.snoPortalLevelArea;
                        if (ACDIsInTown(*ptACDPlayer) && snoPlayerHearthPortalLevelArea != SNO_NONE)
                        {
                            if (snoPlayerHearthPortalLevelArea == tQuestInstance.snoLevelArea ||
                                QuestHasAdditionalLevelArea(tQuestInstance.snoQuest, snoPlayerHearthPortalLevelArea))
                            {
                                bPlayerIsInBountyLevelArea = TRUE;
                            }
                        }

                        if (bPlayerIsInBountyLevelArea)
                        {
                            nPlayersInBountyLevelArea++;
                        }
                    }

                    nPlayersInGame++;
                }

                SGameStatsCounterSet(STAT_COUNTER_GAME_BOUNTY_COMPLETED, tQuestInstance.snoQuest, nPlayersInGame, (float)nPlayersInBountyLevelArea);
            }
        }
        else
        {
            Player *ptPlayer = PlayerGetByPlayerIndex(tQuestInstance.nPlayerIndex);
            if (ptPlayer)
            {
                sSQuestsStepRewardPlayer(ptPlayer, tQuestInstance, nCompletedStepUID);

                SPlayerUpdateQuestHistory(*ptPlayer, tQuestInstance.snoQuest, tQuestInstance.nCurrentStepUID);

                if (tQuestInstance.eInstanceState == QUEST_INSTANCE_COMPLETED)
                {
                    SGameStatsPlayerCompletedQuest(ptPlayer->idPlayerACD, tQuestInstance.snoQuest, ptQuestDef->eQuestType, TRUE);
                }
                else if (nCompletedStepUID == QUEST_UNASSIGNED_STEP_UID)
                {
                    SGameStatsPlayerStartedQuest(ptPlayer->idPlayerACD, tQuestInstance.snoQuest, ptQuestDef->eQuestType);
                }
            }
        }
    }

    if (tQuestInstance.eInstanceState == QUEST_INSTANCE_COMPLETED)
    {

        SQuestsOnQuestCompleted(tQuestInstance.snoQuest, tQuestInstance.nPlayerIndex);
        ScriptEventOnQuestCompleted(QuestGetName(tQuestInstance.snoQuest).str());


        Player *ptPlayer = PlayerGetByPlayerIndex(tQuestInstance.nPlayerIndex);
        if (ptPlayer)
        {
            SPlayerAddToSaveList(*ptPlayer, 0, PlayerSaveReason_Quest, PLAYER_SAVE_MODE_WITHIN_5_SECONDS);
        }
        else
        {
            for (ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
            {
                SPlayerAddToSaveList(*ptPlayer, 0, PlayerSaveReason_Quest, PLAYER_SAVE_MODE_WITHIN_5_SECONDS);
            }
        }
    }





    sSQuestsResetRewardData(tQuestInstance);



    if (tQuestInstance.eInstanceState == QUEST_INSTANCE_COMPLETED &&
        ptQuestDef->eQuestType == QUEST_TYPE_MAIN_QUEST &&
        tQuestInstance.nPlayerIndex == -1)
    {
        for (Player *ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
        {
            SPlayerClearSavePoint(*ptPlayer);
        }
    }


    sUpdateQuestMarkersAndActors();




















    QuestsRefreshQuestDialogs(tQuestInstance.snoQuest);

    if (bResetToFalse)
    {
        ptQuestGlobals->bStartup = FALSE;
    }
}



void SQuestsGetCurrent(SNO & snoQuest, int32 & nStepUID)
{
    snoQuest = SNO_NONE;
    nStepUID = -2;
    QuestInstance * ptQuestInstance = GetLeadingMainLineQuest();
    if (ptQuestInstance)
    {
        ScopedQuestDefinitionLockR ptQuestDef(ptQuestInstance->snoQuest);
        snoQuest = ptQuestInstance->snoQuest;
        nStepUID = ptQuestInstance->nCurrentStepUID;
    }
}






























































































BOOL SQuestsAdvanceTo(SNO snoQuest, int32 nStepUID, BOOL bAllowRollback)
{
    int32 nNextStepUID = nStepUID;
    QuestInstance * ptQuestInstance = QuestsGetQuestInstance(snoQuest, ACDID_INVALID);
    if (!ptQuestInstance)
        return FALSE;

    ScopedQuestDefinitionLockR ptQuestDef(snoQuest);
    int32 nStepIndex = QuestStepUIDToIndex(ptQuestDef, nNextStepUID);
    if (nStepIndex < 0)
        return FALSE;

    if (bAllowRollback)
    {
        if (ptQuestInstance->eInstanceState == QUEST_INSTANCE_COMPLETED)
            return FALSE;
    }
    else if (!QuestStepIsReachableByStep(snoQuest, ptQuestInstance->nCurrentStepUID, nNextStepUID))
    {
        return FALSE;
    }

    SQuestsCompleteMainlineUpToQuest(snoQuest);
    int32 nCompletedStepUID = ptQuestInstance->nCurrentStepUID;
    QuestsAdvanceToStep(*ptQuestInstance, nNextStepUID, bAllowRollback);
    SQuestHandleQuestAdvancement(*ptQuestInstance, nCompletedStepUID, nNextStepUID, FALSE);
    SQuestsSyncPlayerState(*ptQuestInstance, NULL);
    return TRUE;
}

























































































































void SQuestsCreateMapMarker(const Marker & tMarker, const SSceneID idSScene)
{
    SceneCommonData * ptSCD = SceneCommonDataTryToGet(idSScene);
    Minimap::AddQuestMarker(tMarker, ptSCD);
}







static BOOL sSQuestsObjectiveSetProcessMessage(
    QuestNotificationMessage & tMessage,
    QuestInstance & tQuestInstance,
    QuestStepObjectiveSet & tObjectiveSet,
    int32 & nCurrentObjectiveIndex,
    BOOL & bObjectiveMet)
{


    if (tObjectiveSet.nNextStepUID < 0)
        return FALSE;



    Player *ptPlayer = PlayerGetByPlayerIndex(tQuestInstance.nPlayerIndex);
    if (tMessage.eMsgType == QUEST_NOTIFICATION_RESEND_TO_CLIENTS)
    {
        if (!ptPlayer)
        {
            ptPlayer = PlayerGetByPlayerIndex(tMessage.nValue1);
        }
        else if (ptPlayer->nPlayerIndex != tMessage.nValue1)
        {
            return FALSE;
        }
    }

    BOOL bStopProcessingStep = FALSE;  // no code of its own (DWARF local, SQuests.cpp:3764)

    int32 nNumObjectives = tObjectiveSet.serStepObjectives.dwSize / sizeof(QuestStepObjective);

    int32 nSetObjectiveIndex = nCurrentObjectiveIndex;

    for (int32 k = 0; k < nNumObjectives; k++)
    {
        QuestStepObjective &tObjective = tObjectiveSet.arStepObjectives[k];
        bObjectiveMet |= sSQuestObjectiveProcessMessage(tObjective, tQuestInstance.snoQuest, tQuestInstance.snoLevelArea, tQuestInstance.nCurrentStepUID, tQuestInstance.nPlayerIndex, tMessage, tQuestInstance.arObjectiveState[nCurrentObjectiveIndex], TRUE, FALSE);
        if (bObjectiveMet)
        {
            BOOL bProcessedReqPassed = QuestObjectivePassed(tObjective, tQuestInstance.arObjectiveState[nCurrentObjectiveIndex]);
            SQuestsSendCounterToPlayer(tQuestInstance, nCurrentObjectiveIndex, bProcessedReqPassed, ptPlayer);
            SQuestsUpdateQuestObjectiveCompleted(tQuestInstance, nCurrentObjectiveIndex, bProcessedReqPassed);

            if (bProcessedReqPassed)
            {
                ScopedQuestDefinitionLockR pdefQuest(tQuestInstance.snoQuest);
                if (pdefQuest->eQuestType == QUEST_TYPE_BOUNTY)
                {
                    sSQuestsHandleBountyObjectiveCompletion(tObjective);
                }
            }

            BOOL bPassed = TRUE;
            for (int32 l = 0; l < nNumObjectives; l++)
            {
                QuestStepObjective &tObjectiveToCheck = tObjectiveSet.arStepObjectives[l];
                if (!QuestObjectivePassed(tObjectiveToCheck, tQuestInstance.arObjectiveState[nSetObjectiveIndex + l]))
                {
                    bPassed = FALSE;
                    break;
                }
            }

            if (bPassed)
            {

                int32 nCompletedStepUID = tQuestInstance.nCurrentStepUID;
                int32 nStepUID = tObjectiveSet.nNextStepUID;


                SQuestsCompleteMainlineUpToQuest(tQuestInstance.snoQuest);
                QuestsAdvanceToStep(tQuestInstance, nStepUID, FALSE);

                SQuestHandleQuestAdvancement(tQuestInstance, nCompletedStepUID, nStepUID, FALSE);
                return TRUE;
            }
        }




        nCurrentObjectiveIndex++;
    }

    return FALSE;
}



static void sSQuestsBonusObjectiveSetProcessMessage(
    QuestNotificationMessage & tMessage,
    QuestInstance & tQuestInstance,
    QuestStepBonusObjectiveSet & tBonusObjectiveSet,
    int32 nBonusObjectiveSetIndex,
    int32 & nCurrentBonusObjectiveIndex,
    BOOL & bObjectiveMet)
{
    int32 nNumBonusObjectives = tBonusObjectiveSet.serStepBonusObjectives.dwSize / sizeof(QuestStepObjective);

    int32 nSetBonusObjectiveIndex = nCurrentBonusObjectiveIndex;











    BOOL bCap = !tBonusObjectiveSet.bRepeatable || tBonusObjectiveSet.bInverse;

    for (int32 i = 0; i < nNumBonusObjectives; i++)
    {
        QuestStepObjective &tObjective = tBonusObjectiveSet.arStepBonusObjectives[i];
        BOOL bThisObjectiveMet = sSQuestObjectiveProcessMessage(tObjective, tQuestInstance.snoQuest, tQuestInstance.snoLevelArea, tQuestInstance.nCurrentStepUID, tQuestInstance.nPlayerIndex, tMessage, tQuestInstance.arObjectiveState[nCurrentBonusObjectiveIndex], bCap, TRUE);
        bObjectiveMet |= bThisObjectiveMet;
        if (bThisObjectiveMet)
        {

            Player *ptPlayer = PlayerGetByPlayerIndex(tQuestInstance.nPlayerIndex);
            BOOL bProcessedReqPassed = bCap && QuestObjectivePassed(tObjective, tQuestInstance.arObjectiveState[nCurrentBonusObjectiveIndex]);
            SQuestsSendCounterToPlayer(tQuestInstance, nCurrentBonusObjectiveIndex, bProcessedReqPassed, ptPlayer);
            SQuestsUpdateQuestObjectiveCompleted(tQuestInstance, nCurrentBonusObjectiveIndex, bProcessedReqPassed);
        }

        nCurrentBonusObjectiveIndex++;
    }

    if (bObjectiveMet)
    {
        int32 nImmediateRewards = 0;

        if (tBonusObjectiveSet.bRepeatable)
        {


            if (tBonusObjectiveSet.bInverse)
            {

                tQuestInstance.arBonusRewards[nBonusObjectiveSetIndex] = tBonusObjectiveSet.arStepBonusObjectives[0].nParam2
                 - tQuestInstance.arObjectiveState[nSetBonusObjectiveIndex];
            }
            else
            {

                nImmediateRewards = tQuestInstance.arObjectiveState[nSetBonusObjectiveIndex] - tQuestInstance.arBonusRewards[nBonusObjectiveSetIndex];
                tQuestInstance.arBonusRewards[nBonusObjectiveSetIndex] = tQuestInstance.arObjectiveState[nSetBonusObjectiveIndex];
            }
        }
        else
        {
            BOOL bPassed = TRUE;
            for (int32 i = 0; i < nNumBonusObjectives; i++)
            {
                QuestStepObjective &tObjectiveToCheck = tBonusObjectiveSet.arStepBonusObjectives[i];
                if (!QuestObjectivePassed(tObjectiveToCheck, tQuestInstance.arObjectiveState[nSetBonusObjectiveIndex + i]))
                {
                    bPassed = FALSE;
                    break;
                }
            }

            if (bPassed)
            {


                if (tBonusObjectiveSet.bInverse)
                {
                    tQuestInstance.arBonusRewards[nBonusObjectiveSetIndex] = 0;
                }
                else
                {

                    if (!tQuestInstance.arBonusRewards[nBonusObjectiveSetIndex])
                    {
                        nImmediateRewards = 1;
                        tQuestInstance.arBonusRewards[nBonusObjectiveSetIndex] = nImmediateRewards;
                    }
                }
            }
        }

        if (nImmediateRewards > 0)
        {
            if (tQuestInstance.nPlayerIndex == -1)
            {
                for (Player *ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
                {
                    sSQuestGrantImmediateBonusAward(*ptPlayer, tQuestInstance.snoQuest, tBonusObjectiveSet, nImmediateRewards);
                }
            }
            else
            {
                Player *ptPlayer = PlayerGetByPlayerIndex(tQuestInstance.nPlayerIndex);
                if (ptPlayer)
                {
                    sSQuestGrantImmediateBonusAward(*ptPlayer, tQuestInstance.snoQuest, tBonusObjectiveSet, nImmediateRewards);
                }
            }
        }
    }
}



static BOOL sSQuestsFailureConditionSetProcessMessage(
    QuestNotificationMessage & tMessage,
    QuestInstance & tQuestInstance,
    QuestStepFailureConditionSet & tFailureConditionSet,
    int32 & nCurrentFailureConditionIndex,
    BOOL & bFailureConditionMet)
{
    BOOL bStopProcessingStep = FALSE;  // no code of its own (DWARF local, SQuests.cpp:3951)

    int32 nNumFailureConditions = tFailureConditionSet.serStepFailureConditions.dwSize / sizeof(QuestStepFailureCondition);

    int32 nSetFailureConditionIndex = nCurrentFailureConditionIndex;



    for (int32 k = 0; k < nNumFailureConditions; k++)
    {
        QuestStepFailureCondition & tFailureCondition = tFailureConditionSet.arStepFailureConditions[k];
        bFailureConditionMet = sSQuestFailureConditionProcessMessage(tFailureCondition, tQuestInstance, tMessage, nCurrentFailureConditionIndex);
        if (bFailureConditionMet)
        {
            BOOL bPassed = TRUE;
            for (int32 l = 0; l < nNumFailureConditions; l++)
            {

                QuestStepFailureCondition & tFailureConditionToCheck = tFailureConditionSet.arStepFailureConditions[l];



                if (tFailureConditionToCheck.nParam2 > tQuestInstance.arFailureConditionState[nSetFailureConditionIndex + l])
                {
                    bPassed = FALSE;
                    break;
                }

                if (tFailureConditionToCheck.eFailureConditionType == FAILURE_CONDITION_PLAYER_DIED &&
                    tFailureConditionToCheck.nParam1 > tQuestInstance.arFailureConditionState[nSetFailureConditionIndex + l])
                {
                    bPassed = FALSE;
                    break;
                }

                if (tQuestInstance.arFailureConditionState[nSetFailureConditionIndex + l] == 0)
                {
                    bPassed = FALSE;
                    break;
                }
            }

            if (bPassed)
            {


                tQuestInstance.eInstanceState = QUEST_INSTANCE_FAILED;


                if (tQuestInstance.nPlayerIndex != -1)
                {
                    Player *ptPlayer = PlayerGetByPlayerIndex(tQuestInstance.nPlayerIndex);
                    if (ptPlayer)
                    {
                        SQuestsSendQuestStateToPlayer(tQuestInstance, *ptPlayer, TRUE);
                    }
                }
                else
                {
                    SQuestsSendUpdateToAllPlayers(tQuestInstance.snoQuest);
                }

                return TRUE;
            }
        }



        nCurrentFailureConditionIndex++;
    }

    return FALSE;
}



static BOOL sSQuestsInstanceProcessNotification(
    QuestNotificationMessage & tMessage,
    QuestInstance & tQuestInstance)
{
    BOOL bObjectiveMet = FALSE;

    ScopedQuestDefinitionLockR ptQuestDef(tQuestInstance.snoQuest);

    if (tQuestInstance.eInstanceState == QUEST_INSTANCE_COMPLETED || tQuestInstance.eInstanceState == QUEST_INSTANCE_FAILED)
        return FALSE;




    if (tQuestInstance.eInstanceState == QUEST_INSTANCE_UNASSIGNED)
    {
        int32 nNumObjectiveSets = ptQuestDef->tQuestUnassignedStep.serStepObjectiveSets.dwSize / sizeof(QuestStepObjectiveSet);
        BOOL bStopProcessingStep = FALSE;
        int32 nCurrentObjectiveIndex = 0;
        for (int32 j = 0; j < nNumObjectiveSets; j++)
        {
            QuestStepObjectiveSet &tObjectiveSet = ptQuestDef->tQuestUnassignedStep.arStepObjectiveSets[j];

            BOOL bObjectiveMetThis = FALSE;
            bStopProcessingStep = sSQuestsObjectiveSetProcessMessage(tMessage, tQuestInstance, tObjectiveSet, nCurrentObjectiveIndex, bObjectiveMetThis);
            bObjectiveMet |= bObjectiveMetThis; if (bStopProcessingStep) break;
        }







        if (!bStopProcessingStep)
        {
            int32 nNumFailureConditions = ptQuestDef->tQuestUnassignedStep.serStepFailureConditionSets.dwSize / sizeof(QuestStepFailureConditionSet);
            int32 nCurrentFailureConditionIndex = 0;
            for (int32 j = 0; j < nNumFailureConditions; j++)
            {
                QuestStepFailureConditionSet &tFailureConditionSet = ptQuestDef->tQuestUnassignedStep.arStepFailureConditionSets[j];
                BOOL bObjectiveMetThis = FALSE;
                bStopProcessingStep = sSQuestsFailureConditionSetProcessMessage(tMessage, tQuestInstance, tFailureConditionSet, nCurrentFailureConditionIndex, bObjectiveMetThis);
                bObjectiveMet |= bObjectiveMetThis; if (bStopProcessingStep) break;
            }
        }
    }
    else
    {





        int32 nStepIndex = QuestStepUIDToIndex(ptQuestDef, tQuestInstance.nCurrentStepUID);

        QuestStep &tQuestStep = ptQuestDef->arQuestSteps[nStepIndex];




        int32 nNumBonusObjectiveSets = tQuestStep.serStepBonusObjectiveSets.dwSize / sizeof(QuestStepBonusObjectiveSet);
        int32 nCurrentBonusObjectiveIndex = QuestStepGetNumObjectives(tQuestStep);

        for (int32 j = 0; j < nNumBonusObjectiveSets; j++)
        {
            QuestStepBonusObjectiveSet &tBonusObjectiveSet = tQuestStep.arStepBonusObjectiveSets[j];

            BOOL bObjectiveMetThis = FALSE;
            sSQuestsBonusObjectiveSetProcessMessage(tMessage, tQuestInstance, tBonusObjectiveSet, j, nCurrentBonusObjectiveIndex, bObjectiveMetThis);
            bObjectiveMet |= bObjectiveMetThis;
        }


        int32 nNumObjectiveSets = tQuestStep.serStepObjectiveSets.dwSize / sizeof(QuestStepObjectiveSet);
        BOOL bStopProcessingStep = FALSE;
        int32 nCurrentObjectiveIndex = 0;

        for (int32 j = 0; j < nNumObjectiveSets; j++)
        {
            QuestStepObjectiveSet &tObjectiveSet = tQuestStep.arStepObjectiveSets[j];

            BOOL bObjectiveMetThis = FALSE;
            bStopProcessingStep = sSQuestsObjectiveSetProcessMessage(tMessage, tQuestInstance, tObjectiveSet, nCurrentObjectiveIndex, bObjectiveMetThis);
            bObjectiveMet |= bObjectiveMetThis; if (bStopProcessingStep) break;
        }







        if (!bStopProcessingStep)
        {
            int32 nNumFailureConditions = tQuestStep.serStepFailureConditionSets.dwSize / sizeof(QuestStepFailureConditionSet);
            int32 nCurrentFailureConditionIndex = 0;
            for (int32 j = 0; j < nNumFailureConditions; j++)
            {
                QuestStepFailureConditionSet &tFailureConditionSet = tQuestStep.arStepFailureConditionSets[j];
                BOOL bObjectiveMetThis = FALSE;
                bStopProcessingStep = sSQuestsFailureConditionSetProcessMessage(tMessage, tQuestInstance, tFailureConditionSet, nCurrentFailureConditionIndex, bObjectiveMetThis);
                bObjectiveMet |= bObjectiveMetThis; if (bStopProcessingStep) break;
            }
        }
    }






    return bObjectiveMet;
}


// NON_MATCHING: 1324 B, same size; 2 instructions differ: at line 4221 our build tests bWasUnassigned (cmn w27,#1) before eInstanceState == ACTIVE, the original after. Tried bWasUnassigned == TRUE and parenthesising the last two operands.
BOOL SQuestsProcessNotification(
    QuestNotificationMessage & tMessage)
{
    if (!GameCommonDataExists())
        return FALSE;



    BOOL bHandled = FALSE;

    ACDID idACDPlayer = ACDID_INVALID;
    ActorCommonData * ptACD = ACDTryToGet(tMessage.idACD1);
    if (ptACD)
    {
        if (ACDIsPlayer(tMessage.idACD1))
        {
            idACDPlayer = tMessage.idACD1;
        }
        else
        {
            ACDID idACDControllingActor = ACDGetControllingActor(tMessage.idACD1);
            ACDID idACDFollowTarget = (ACDID)ptACD->AttributesGetInt(FastAttribKey(ATTRIBUTE_FOLLOW_TARGET_ACDID));
            if (ACDIsPlayer(idACDControllingActor))
            {
                idACDPlayer = idACDControllingActor;
            }
            else if (ACDIsPlayer(idACDFollowTarget))
            {
                idACDPlayer = idACDFollowTarget;
            }
        }
    }

    ActorCommonData * ptACDPlayer = ACDTryToGet(idACDPlayer);
    if (ptACDPlayer && ptACDPlayer->AttributesGetInt(FastAttribKey(ATTRIBUTE_AUTO_PORTING_TO_SAVE_POINT)) > 0)
    {
        return FALSE;
    }



    GameCommonData & tGCD = GameGetActiveGameCommonData();
    QuestGlobals *& ptQuestGlobals = tGCD.ptQuestGlobals;


    POS pos = ptQuestGlobals->tQuestInstanceList.GetHeadPosition();
    while (pos)
    {
        QuestInstance & tQuestInstance = ptQuestGlobals->tQuestInstanceList.GetNext(pos);


        if (!QuestStepTrackPerPlayer(tQuestInstance.snoQuest, tQuestInstance.nCurrentStepUID))
        {
            BOOL bWasUnassigned = (tQuestInstance.nCurrentStepUID == QUEST_UNASSIGNED_STEP_UID);
            BOOL bHandledThis = sSQuestsInstanceProcessNotification(tMessage, tQuestInstance);
            bHandled |= bHandledThis;
            if (bHandledThis)
            {


                if (!QuestStepTrackPerPlayer(tQuestInstance.snoQuest, tQuestInstance.nCurrentStepUID) &&
                    idACDPlayer != ACDID_INVALID &&
                    PlayerGetNumPlayers() > 1)
                {
                    if (tQuestInstance.eInstanceState == QUEST_INSTANCE_COMPLETED)
                    {

                        PlayerList listDestPlayers( PlayerListGetElementPool());
                        PlayerListGetAllInGame(listDestPlayers, -1, TRUE, -1);
                        Player *ptPlayer = PlayerGetByACD(idACDPlayer);
                        PlayerQuestMessage tQuestMessage;
                        tQuestMessage.nPlayerIndex = ptPlayer->nPlayerIndex;
                        tQuestMessage.snoQuest = tQuestInstance.snoQuest;
                        MessageSendToClient(listDestPlayers, SMSG_PLAYER_COMPLETED_QUEST, &tQuestMessage, sizeof(tQuestMessage));
                        SPlayerRefreshWaypointDestination(ptPlayer->idPlayerACD);
                    }

                    else if (tQuestInstance.eInstanceState == QUEST_INSTANCE_ACTIVE && bWasUnassigned &&
                        tQuestInstance.nCurrentStepUID != QUEST_UNASSIGNED_STEP_UID)
                    {

                        Player *ptPlayer = PlayerGetByACD(idACDPlayer);


                        PlayerList listDestPlayers( PlayerListGetElementPool());
                        PlayerListGetAllInGame(listDestPlayers, -1, TRUE, -1);

                        PlayerQuestMessage tQuestMessage;
                        tQuestMessage.nPlayerIndex = ptPlayer->nPlayerIndex;
                        tQuestMessage.snoQuest = tQuestInstance.snoQuest;
                        tQuestMessage.snoLevelArea = tQuestInstance.snoLevelArea;
                        MessageSendToClient(listDestPlayers, SMSG_PLAYER_ACTIVATED_QUEST, &tQuestMessage, sizeof(tQuestMessage));
                        SPlayerRefreshWaypointDestination(ptPlayer->idPlayerACD);
                    }
                }



                SQuestsSyncPlayerState(tQuestInstance, NULL);
            }
        }
    }


    pos = ptQuestGlobals->tQuestPerPlayerInstanceList.GetHeadPosition();
    while (pos)
    {
        QuestInstance & tQuestInstance = ptQuestGlobals->tQuestPerPlayerInstanceList.GetNext(pos);
        BOOL bHandledThis = sSQuestsInstanceProcessNotification(tMessage, tQuestInstance);
        bHandled |= bHandledThis;

        if (bHandledThis)
        {
            SQuestsSyncPlayerState(tQuestInstance, NULL);
        }
    }


    {
        SNO snoQuest = GlobalSNOGet(GLOBALSNO_QUEST_DYNAMIC);
        // 10: the length of QuestGlobals::tDynamicQuestObjectives (no named constant in the DWARF)
        for (int32 i = 0; i < 10; i++)
        {
            QuestStepObjective & tObjective = ptQuestGlobals->tDynamicQuestObjectives[i];

            if (tObjective.eObjectiveType != OBJECTIVE_INVALID)
            {
                const int32 DYNAMIC_QUEST_ASSIGNED_STEP = 2;
                BOOL bObjectiveMet = sSQuestObjectiveProcessMessage(tObjective, snoQuest, SNO_NONE, DYNAMIC_QUEST_ASSIGNED_STEP, -1, tMessage, ptQuestGlobals->arDynamicQuestObjectiveState[i], TRUE, FALSE);
                if (bObjectiveMet)
                {

                    if (!ptQuestGlobals->arDynamicQuestObjectiveCompleted[i] &&
                        QuestObjectivePassed(tObjective, ptQuestGlobals->arDynamicQuestObjectiveState[i]))
                    {
                        ptQuestGlobals->arDynamicQuestObjectiveCompleted[i] = TRUE;

                        SQuestsHandleDynamicObjectiveCompletion(i);


                        ScriptEventOnQuestObjectiveCompleted(QuestGetName(snoQuest).str(),
                            QuestStepGetName(snoQuest, DYNAMIC_QUEST_ASSIGNED_STEP).str(), i);


                        ptQuestGlobals->arDynamicQuestObjectiveDirty[i] = TRUE;
                    }


                    if (!ptQuestGlobals->arDynamicQuestObjectiveCompleted[i])

                        ptQuestGlobals->arDynamicQuestObjectiveDirty[i] = TRUE;
                }
            }
        }
    }

    return bHandled;

}

static void sSQuestsHandleBountyObjectiveCompletion(const QuestStepObjective & tObjective)
{

    if (tObjective.eObjectiveType == OBJECTIVE_KILL_MONSTER || tObjective.eObjectiveType == OBJECTIVE_INTERACT_WITH_GIZMO)
    {
        SNO snoActor = tObjective.snoname1.snoHandle;
        int32 idPing = Minimap::SNOGetAutoPing(snoActor);
        Minimap::RemovePing(idPing);
    }
    else if (tObjective.eObjectiveType == OBJECTIVE_COMPLETE_QUEST)
    {
        SNO snoQuest = tObjective.snoname1.snoHandle;
        int32 idPing = Minimap::SNOGetAutoPing(snoQuest);
        Minimap::RemovePing(idPing);
    }
    else if (tObjective.eObjectiveType == OBJECTIVE_ENTER_LEVEL_AREA)
    {
        SNO snoLevelArea = tObjective.snoname1.snoHandle;
        Minimap::SNORemoveAutoPingRef(snoLevelArea);
    }
}




void SQuestsHandleDynamicObjectiveCompletion(int32 nObjectiveIndex)
{

    GameCommonData & tGCD = GameGetActiveGameCommonData();
    QuestGlobals *& ptQuestGlobals = tGCD.ptQuestGlobals;
    QuestStepObjective & tObjective = ptQuestGlobals->tDynamicQuestObjectives[nObjectiveIndex];

    if (tObjective.snoname1.snoGroup == LEVELAREA_SNO)
    {
        Minimap::SNORemoveAutoPingRef(tObjective.snoname1.snoHandle);
    }
    else if (tObjective.snoname2.snoGroup == LEVELAREA_SNO)
    {
        Minimap::SNORemoveAutoPingRef(tObjective.snoname2.snoHandle);
    }


    if (tObjective.eObjectiveType == OBJECTIVE_KILL_MONSTER)


        Minimap::SNORemoveAutoPingRef(tObjective.snoname1.snoHandle);
}





void SQuestsSyncPlayerState(QuestInstance & tQuestInstance, Player * ptPlayerToSync)
{
    if (tQuestInstance.nPlayerIndex == -1)
    {
        for (Player * ptPlayer = PlayerGetFirstAll(); ptPlayer; ptPlayer = PlayerGetNextAll(ptPlayer))
        {
            if (ptPlayerToSync && ptPlayer->nPlayerIndex != ptPlayerToSync->nPlayerIndex)
                continue;

            if (tQuestInstance.eInstanceState == QUEST_INSTANCE_UNASSIGNED || QuestIsComplete(tQuestInstance))
            {
                PlayerSavedQuest * ptSavedQuest = PlayerGetSavedQuest(*ptPlayer, tQuestInstance.snoQuest);
                if (ptSavedQuest)
                {
                    PlayerSavedQuestInitialize(*ptSavedQuest);
                    ptPlayer->uConsoleSyncFlags |= (1 << PCSF_SAVED_QUESTS);
                }
            }
            else if (QuestStepIsResumable(tQuestInstance.snoQuest, tQuestInstance.nCurrentStepUID))
            {
                PlayerSavedQuest * ptSavedQuest = PlayerGetSavedQuest(*ptPlayer, tQuestInstance.snoQuest);
                if (!ptSavedQuest)
                    ptSavedQuest = PlayerAddSavedQuest(*ptPlayer, tQuestInstance.snoQuest);
                ptSavedQuest->nCurrentStepUID = tQuestInstance.nCurrentStepUID;
                if (QuestStepSavePartialCompletion(tQuestInstance.snoQuest, tQuestInstance.nCurrentStepUID))
                    SigmaMemoryCopy(ptSavedQuest->arObjectiveState, tQuestInstance.arObjectiveState, sizeof(ptSavedQuest->arObjectiveState));
                else
                    SigmaMemoryClear(ptSavedQuest->arObjectiveState, sizeof(ptSavedQuest->arObjectiveState));
                ptPlayer->uConsoleSyncFlags |= (1 << PCSF_SAVED_QUESTS);
            }
        }
    }
    else
    {
        Player * ptPlayer = PlayerGetByPlayerIndex(tQuestInstance.nPlayerIndex);
        if (!ptPlayer)
            return;
        if (ptPlayerToSync && ptPlayer->nPlayerIndex != ptPlayerToSync->nPlayerIndex)
            return;

        if (tQuestInstance.eInstanceState == QUEST_INSTANCE_UNASSIGNED || QuestIsComplete(tQuestInstance))
        {
            PlayerSavedQuest * ptSavedQuest = PlayerGetSavedQuest(*ptPlayer, tQuestInstance.snoQuest);
            if (ptSavedQuest)
            {
                PlayerSavedQuestInitialize(*ptSavedQuest);
                ptPlayer->uConsoleSyncFlags |= (1 << PCSF_SAVED_QUESTS);
            }
        }
        else if (QuestStepIsResumable(tQuestInstance.snoQuest, tQuestInstance.nCurrentStepUID))
        {
            PlayerSavedQuest * ptSavedQuest = PlayerGetSavedQuest(*ptPlayer, tQuestInstance.snoQuest);
            if (!ptSavedQuest)
                ptSavedQuest = PlayerAddSavedQuest(*ptPlayer, tQuestInstance.snoQuest);
            if (QuestStepIsReachableByStep(tQuestInstance.snoQuest, tQuestInstance.nCurrentStepUID, ptSavedQuest->nCurrentStepUID) &&
                tQuestInstance.nCurrentStepUID != ptSavedQuest->nCurrentStepUID)
                return;
            ptSavedQuest->nCurrentStepUID = tQuestInstance.nCurrentStepUID;
            if (QuestStepSavePartialCompletion(tQuestInstance.snoQuest, tQuestInstance.nCurrentStepUID))
                SigmaMemoryCopy(ptSavedQuest->arObjectiveState, tQuestInstance.arObjectiveState, sizeof(ptSavedQuest->arObjectiveState));
            else
                SigmaMemoryClear(ptSavedQuest->arObjectiveState, sizeof(ptSavedQuest->arObjectiveState));
            ptPlayer->uConsoleSyncFlags |= (1 << PCSF_SAVED_QUESTS);
        }
    }
}






















































void SQuestsSyncPlayerSavedQuests(Player & tPlayer)
{

    QuestInstance * ptQuestInstance = GetLeadingMainLineQuest();
    SNO snoQuest = ptQuestInstance ? ptQuestInstance->snoQuest : SNO_NONE;




    if (ptQuestInstance)
        SQuestsSyncPlayerState(*ptQuestInstance, &tPlayer);



    for (int32 i = 0; i < 64; i++)
    {
        PlayerSavedQuest & tSavedQuest = tPlayer.tHeroStateData.tSavedQuest[i];
        if (tSavedQuest.snoQuest == SNO_NONE || tSavedQuest.snoQuest == snoQuest)
            continue;











        ScopedQuestDefinitionLockR pdefQuest(tSavedQuest.snoQuest);
        if (pdefQuest->eQuestType == QUEST_TYPE_MAIN_QUEST &&
            !QuestStepTrackPerPlayer(tSavedQuest.snoQuest, tSavedQuest.nCurrentStepUID))
        {

            PlayerSavedQuestInitialize(tSavedQuest);


            tPlayer.uConsoleSyncFlags |= (1 << PCSF_SAVED_QUESTS);
        }
    }
}

BOOL SQuestsOnTrigger(ACDID idACDTriggeringActor, ACDID idACDTrigger)
{
    QuestNotificationMessage tMessage;
    tMessage.eMsgType = QUEST_NOTIFICATION_TRIGGER;
    tMessage.idACD1 = idACDTriggeringActor;
    tMessage.idACD2 = idACDTrigger;
    return SQuestsProcessNotification(tMessage);
}


































void SQuestsOnQuestCompleted(SNO snoQuest, int32 nPlayerIndex)
{
    QuestNotificationMessage tMessage;
    tMessage.eMsgType = QUEST_NOTIFICATION_COMPLETE_QUEST;
    tMessage.sno1 = snoQuest;
    tMessage.nValue1 = nPlayerIndex;
    SQuestsProcessNotification(tMessage);

    ScopedQuestDefinitionLockR pdefQuest(snoQuest);
    if (pdefQuest->dwFlags & (1 << QUEST_FLAG_AUTO_GENERATE_MAP_ICON))
    {
        CRefString questName = SNOToString(QUEST_SNO, snoQuest, FALSE);
        questName += ".QuestMarker";
        Minimap::DeactivateMarker(questName.str(), FALSE);
    }
}








void SQuestsOnActorKilled(ACDID idACDKiller, ActorCommonData & tACDVictim)
{
    QuestNotificationMessage tMessage;
    tMessage.eMsgType = QUEST_NOTIFICATION_ACTOR_KILLED;
    tMessage.idACD1 = ACDGetControllingActor(idACDKiller);
    tMessage.idACD2 = tACDVictim.id;
    tMessage.nValue1 = tACDVictim.uGroupPrimary;
    tMessage.nValue2 = tACDVictim.uGroupSecondary;
    SQuestsProcessNotification(tMessage);
}











void SQuestsOnActorRemoved(ActorCommonData & tACDActor)
{
    QuestNotificationMessage tMessage;
    tMessage.eMsgType = QUEST_NOTIFICATION_ACTOR_KILLED;
    tMessage.idACD2 = tACDActor.id;
    SQuestsProcessNotification(tMessage);
}







void SQuestsOnGroupKilled(int32 nGroup)
{
    QuestNotificationMessage tMessage;
    tMessage.eMsgType = QUEST_NOTIFICATION_GROUP_KILLED;
    tMessage.nValue1 = nGroup;
    SQuestsProcessNotification(tMessage);
}




void SQuestsOnGroupModified(int32 nGroup, int32 nCount)
{
    QuestNotificationMessage tMessage;
    tMessage.eMsgType = QUEST_NOTIFICATION_GROUP_MODIFIED;
    tMessage.nValue1 = nGroup;
    tMessage.nValue2 = nCount;
    SQuestsProcessNotification(tMessage);
}





void SQuestsOnWorldMonstersRemainingModified(SNO snoWorld, int32 nMonstersRemaining)
{
    QuestNotificationMessage tMessage;
    tMessage.eMsgType = QUEST_NOTIFICATION_WORLD_MONSTERS_REMAINING_MODIFIED;
    tMessage.sno1 = snoWorld;
    tMessage.nValue1 = nMonstersRemaining;
    SQuestsProcessNotification(tMessage);
}





void SQuestsOnItemUsed(ACDID idACDPlayer, ACDID idACDItem)
{
    QuestNotificationMessage tMessage;
    tMessage.eMsgType = QUEST_NOTIFICATION_ITEM_USED;
    tMessage.idACD1 = idACDPlayer;
    tMessage.idACD2 = idACDItem;
    SQuestsProcessNotification(tMessage);
}






void SQuestsOnConversationEnded(ACDID idACDPlayer, SNO snoConversation)
{
    QuestNotificationMessage tMessage;
    tMessage.eMsgType = QUEST_NOTIFICATION_HAD_CONVERSATION;
    tMessage.idACD1 = idACDPlayer;
    tMessage.sno1 = snoConversation;
    SQuestsProcessNotification(tMessage);
}






void SQuestsOnPlayerAcquireItem(ACDID idACDPlayer, ACDID idACDItem)
{
    QuestNotificationMessage tMessage;
    tMessage.eMsgType = QUEST_NOTIFICATION_POSSESS_ITEM;
    tMessage.idACD1 = idACDPlayer;
    tMessage.idACD2 = idACDItem;
    SQuestsProcessNotification(tMessage);
}








void SQuestsOnPlayerInteractWithGizmo(ACDID idACDPlayer, ACDID idACDGizmo)
{
    QuestNotificationMessage tMessage;
    tMessage.eMsgType = QUEST_NOTIFICATION_INTERACT_WITH_GIZMO;
    tMessage.idACD1 = idACDPlayer;
    tMessage.idACD2 = idACDGizmo;
    SQuestsProcessNotification(tMessage);
}






void SQuestsOnPlayerEnterScene(ACDID idACDPlayer, SNO snoScene)
{
    QuestNotificationMessage tMessage;
    tMessage.eMsgType = QUEST_NOTIFICATION_ENTER_SCENE;
    tMessage.idACD1 = idACDPlayer;
    tMessage.sno1 = snoScene;
    SQuestsProcessNotification(tMessage);
}






void SQuestsOnPlayerEnterWorld(ACDID idACDPlayer, SNO snoWorld)
{
    QuestNotificationMessage tMessage;
    tMessage.eMsgType = QUEST_NOTIFICATION_ENTER_WORLD;
    tMessage.idACD1 = idACDPlayer;
    tMessage.sno1 = snoWorld;
    SQuestsProcessNotification(tMessage);
}






void SQuestsOnPlayerEnterLevelArea(ACDID idACDPlayer, SNO snoLevelArea)
{
    QuestNotificationMessage tMessage;
    tMessage.eMsgType = QUEST_NOTIFICATION_ENTER_LEVEL_AREA;
    tMessage.idACD1 = idACDPlayer;
    tMessage.sno1 = snoLevelArea;
    SQuestsProcessNotification(tMessage);
}






void SQuestsOnPlayerFlagSet(ACDID idACDPlayer, int32 nPlayerFlagIndex, BOOL bValue)
{
    QuestNotificationMessage tMessage;
    tMessage.eMsgType = QUEST_NOTIFICATION_PLAYER_FLAG_SET;
    tMessage.idACD1 = idACDPlayer;
    tMessage.nValue1 = nPlayerFlagIndex;
    tMessage.nValue2 = bValue;
    SQuestsProcessNotification(tMessage);
}







void SQuestsOnHQCursedRealmReagentCollected(ACDID idACDPlayer, int32 nActIndex)
{
    QuestNotificationMessage tMessage;
    tMessage.eMsgType = QUEST_NOTIFICATION_HQ_REAGENT_COLLECTED;
    tMessage.idACD1 = idACDPlayer;
    tMessage.nValue1 = nActIndex;
    SQuestsProcessNotification(tMessage);
}






void SQuestsOnGameFlagSet(int32 nGameFlagIndex, BOOL bValue)
{
    QuestNotificationMessage tMessage;
    tMessage.eMsgType = QUEST_NOTIFICATION_GAME_FLAG_SET;
    tMessage.nValue1 = nGameFlagIndex;
    tMessage.nValue2 = bValue;
    SQuestsProcessNotification(tMessage);
}






void SQuestsOnTimedEventExpired(SNO snoTimedEvent)
{
    QuestNotificationMessage tMessage;
    tMessage.eMsgType = QUEST_NOTIFICATION_TIMED_EVENT_EXPIRED;
    tMessage.sno1 = snoTimedEvent;
    SQuestsProcessNotification(tMessage);
}




void SQuestsOnTimedEventAboutToExpire(SNO snoTimedEvent, uint32 dwTicksRemaining)
{
    QuestNotificationMessage tMessage;
    tMessage.eMsgType = QUEST_NOTIFICATION_TIMED_EVENT_ABOUT_TO_EXPIRE;
    tMessage.sno1 = snoTimedEvent;
    tMessage.nValue1 = dwTicksRemaining;
    SQuestsProcessNotification(tMessage);
}





void SQuestsOnEventSend(LPCSTR szEventName, ACDID idACD)
{
    QuestNotificationMessage tMessage;
    tMessage.eMsgType = QUEST_NOTIFICATION_EVENT_RECEIVED;
    Blizzard::String::Copy(tMessage.szValue1, szEventName, MAX_NOTIFICATION_STRING_SIZE);
    tMessage.idACD1 = idACD;
    SQuestsProcessNotification(tMessage);
}





void SQuestsOnQuestActivated(const QuestInstance & tQuestInstance)
{
    ScopedQuestDefinitionLockR pdefQuest(tQuestInstance.snoQuest);
    if (pdefQuest->eQuestType == QUEST_TYPE_EVENT && (pdefQuest->dwFlags & (1 << QUEST_FLAG_AUTO_GENERATE_MAP_ICON)))
    {
        CRefString markerName = SNOToString(QUEST_SNO, tQuestInstance.snoQuest, FALSE);
        markerName += ".QuestMarker";
        Minimap::ActivateMarker(markerName);
    }
}






void SQuestsResendCounters(int32 nPlayerIndex)
{


    QuestNotificationMessage tMessage;
    tMessage.eMsgType = QUEST_NOTIFICATION_RESEND_TO_CLIENTS;
    tMessage.nValue1 = nPlayerIndex;

    SQuestsProcessNotification(tMessage);
}




void QuestMeterTrackActorHitpoints(LPCSTR szQuestName, ACDID idActor)
{
    SNO snoQuest = SNOFileLookupGUID(QUEST_SNO, szQuestName);
    QuestInstance * ptQuestInstance = QuestsGetQuestInstance(snoQuest, ACDID_INVALID);
    if (ptQuestInstance)
    {
        ptQuestInstance->idACDMeter = idActor;
        ptQuestInstance->flMeter = -1.0f;
        SQuestsSendQuestMeter(*ptQuestInstance, NULL);
    }
}







void QuestMeterSetAmount(LPCSTR szQuestName, float flPercent)
{
    SNO snoQuest = SNOFileLookupGUID(QUEST_SNO, szQuestName);
    QuestInstance * ptQuestInstance = QuestsGetQuestInstance(snoQuest, ACDID_INVALID);
    if (ptQuestInstance)
    {
        ptQuestInstance->idACDMeter = ACDID_INVALID;
        ptQuestInstance->flMeter = flPercent;
        SQuestsSendQuestMeter(*ptQuestInstance, NULL);
    }
}







void HoradricQuestSetColorOrbs(Act eAct, BOOL bRed, BOOL bGreen, BOOL bBlue)
{






    QuestGlobals * ptQuestGlobals = GameGetActiveGameCommonData().ptQuestGlobals;
    QuestInstance * ptQuestInstance = NULL;
    POS pos = ptQuestGlobals->tQuestInstanceList.GetHeadPosition();
    while (pos)
    {
        QuestInstance & tQuestInstance = ptQuestGlobals->tQuestInstanceList.GetNext(pos);
        ScopedQuestDefinitionLockR pdefQuest(tQuestInstance.snoQuest);
        if (pdefQuest && pdefQuest->tBountyData.eAct == eAct && pdefQuest->eQuestType == QUEST_TYPE_HORADRIC_QUEST && pdefQuest->tBountyData.eBountyType == 5)
        {
            ptQuestInstance = &tQuestInstance;
            break;
        }
    }

    if (!ptQuestInstance)
        return;

    if (bRed)
        ptQuestInstance->dwFlags |= (1 << QUEST_FLAG_COLOR_ORBS_RED);
    else
        ptQuestInstance->dwFlags &= ~(1 << QUEST_FLAG_COLOR_ORBS_RED);

    if (bGreen)
        ptQuestInstance->dwFlags |= (1 << QUEST_FLAG_COLOR_ORBS_GREEN);
    else
        ptQuestInstance->dwFlags &= ~(1 << QUEST_FLAG_COLOR_ORBS_GREEN);

    if (bBlue)
        ptQuestInstance->dwFlags |= (1 << QUEST_FLAG_COLOR_ORBS_BLUE);
    else
        ptQuestInstance->dwFlags &= ~(1 << QUEST_FLAG_COLOR_ORBS_BLUE);

    SQuestsSendFlags(*ptQuestInstance, NULL);
}




























void SQuestsLoadSavedQuestObjectives(const D3::Hero::SavedDefinition & tHeroDefinition)
{
    QuestInstance * ptQuestInstance = GetLeadingMainLineQuest();
    if (ptQuestInstance)
    {
        PlayerSavedQuest tSavedQuest;
        BOOL bFound = PlayerGetSavedQuest(tHeroDefinition, ptQuestInstance->snoQuest, tSavedQuest);
        if (bFound && tSavedQuest.nCurrentStepUID == ptQuestInstance->nCurrentStepUID)
        {
            SigmaMemoryCopy(ptQuestInstance->arObjectiveState, tSavedQuest.arObjectiveState, sizeof(ptQuestInstance->arObjectiveState));
        }
    }
}















void SQuestsGetDifficultySnapshot(const D3::Hero::SavedDefinition & tHeroDefinition)
{
    int32 nHandicapSnapshot = tHeroDefinition.saved_data().main_quest_handicap_snapshot();
    if (nHandicapSnapshot >= 0)
    {
        QuestInstance * ptCurrentQuest = GetLeadingMainLineQuest();
        if (ptCurrentQuest && !QuestStepIsSelectable(ptCurrentQuest->snoQuest, ptCurrentQuest->nCurrentStepUID))
        {
            QuestGlobals * ptQuestGlobals = GameGetActiveGameCommonData().ptQuestGlobals;
            ptQuestGlobals->nCurrentMainLineQuestHandicapLevel = Math::Min(ptQuestGlobals->nCurrentMainLineQuestHandicapLevel, nHandicapSnapshot);
        }
    }
}











void SQuestsLoadFromStartData()
{


    SGameGlobals & tSGameGlobals = SGameGlobalsGet();
    SNO snoInitQuest = tSGameGlobals.snoInitialQuest;
    int32 nStepUID = tSGameGlobals.nInitialQuestStepUID;


    if (snoInitQuest == SNO_NONE)
        return;

    QuestGlobals * ptQuestGlobals = GameGetActiveGameCommonData().ptQuestGlobals;

    POS pos = ptQuestGlobals->tQuestInstanceList.GetHeadPosition();
    while (pos)
    {
        QuestInstance & tQuestInstance = ptQuestGlobals->tQuestInstanceList.GetNext(pos);


        if (QuestGetQuestType(tQuestInstance.snoQuest) != QUEST_TYPE_MAIN_QUEST)
            continue;



        SNO snoQuest = tQuestInstance.snoQuest;

        if (snoQuest == snoInitQuest)
        {


            SQuestsCompleteMainlineUpToQuest(snoQuest);



            if (QuestStepIsResumable(snoQuest, nStepUID))
            {


                int32 nCurrentStepUID = tQuestInstance.nCurrentStepUID;
                if (nCurrentStepUID != nStepUID && QuestStepIsReachableByStep(snoQuest, nCurrentStepUID, nStepUID))
                {
                    QuestsAdvanceToStep(tQuestInstance, nStepUID, FALSE);
                    SQuestHandleQuestAdvancement(tQuestInstance, nCurrentStepUID, nStepUID, TRUE);
                }
            }
            return;
        }
    }

    Act eAct = GameGetCurrentAct();
    SNO snoAct = ActSNOFromActEnum(eAct);
}











BOOL SQuestStateIsExploitable()
{

    SGameGlobals & tSGameGlobals = SGameGlobalsGet();


    if (!tSGameGlobals.bResumeFromSave)
        return FALSE;




    uint32 uCurAppTime = AppGetTicks();

    uint32 uElapsedGameTime = uCurAppTime > tSGameGlobals.dwGameStartAppTime ? uCurAppTime - tSGameGlobals.dwGameStartAppTime : 0;




    if (uElapsedGameTime > CREATOR_APP_TICKS_TO_DISCONNECT_GAME)
        return FALSE;

    if (tSGameGlobals.dwFlags & (1 << SGAMEGLOBALS_QUEST_ADVANCED_BIT))
        return FALSE;

    QuestInstance * ptQuestInstance = GetLeadingMainLineQuest();


    if (ptQuestInstance && ptQuestInstance->nResumableStepUID != -1 &&
        !QuestStepIsEligibleForMatchmaking(ptQuestInstance->snoQuest, ptQuestInstance->nResumableStepUID) &&
        QuestStepCanReachReward(ptQuestInstance->snoQuest, ptQuestInstance->nResumableStepUID))
    {
        return TRUE;
    }

    return FALSE;
}


















































void SQuestsSendUpdateToAllPlayers(SNO snoQuest)
{
    QuestInstance * ptQuestInstance = QuestsGetQuestInstance(snoQuest, ACDID_INVALID);
    if (ptQuestInstance)
    {
        for (int32 i = 0; i < 4; i++)
        {
            Player * ptPlayer = PlayerGetByPlayerIndex(i);
            if (ptPlayer && ptPlayer->idGameConnection != -1)
            {
                SQuestsSendQuestStateToPlayer(*ptQuestInstance, *ptPlayer, TRUE);
            }
        }
    }
}









void SQuestsSendQuestStateToPlayer(QuestInstance & tQuest, const Player & tPlayer, const BOOL bDisplayButton)
{
    if (PlayerIsHardcoreAndDead(tPlayer))
        return;

    QuestUpdateMessage tMessage;
    tMessage.nStepUID = tQuest.nCurrentStepUID;
    tMessage.snoQuest = tQuest.snoQuest;
    tMessage.snoLevelArea = tQuest.snoLevelArea;
    tMessage.bFailed = tQuest.eInstanceState == QUEST_INSTANCE_FAILED;
    tMessage.bDisplayButton = bDisplayButton;
    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetSingle(listDestPlayers, tPlayer);
    MessageSendToClient(listDestPlayers, SMSG_QUEST_UPDATE, &tMessage, sizeof(tMessage));
    SPlayerRefreshWaypointDestination(tPlayer.idPlayerACD);
    QuestsForceSystemUpdate();
}















void SQuestsSendCounterToPlayer(QuestInstance & tQuest, int32 nObjectiveIndex, BOOL bCompleted, const Player * ptPlayer)
{
    QuestCounterMessage tMessage;
    tMessage.snoQuest = tQuest.snoQuest;
    tMessage.snoLevelArea = tQuest.snoLevelArea;
    tMessage.nStepUID = tQuest.nCurrentStepUID;
    tMessage.nObjectiveIndex = nObjectiveIndex;
    tMessage.nValue = tQuest.arObjectiveState[nObjectiveIndex];
    tMessage.bCompleted = bCompleted;

    PlayerList listDestPlayers(PlayerListGetElementPool());
    if (ptPlayer == NULL)
    {
        PlayerListGetAllConnected(listDestPlayers, -1, TRUE);
    }
    else
    {
        if (PlayerIsHardcoreAndDead(*ptPlayer))
            return;
        PlayerListGetSingle(listDestPlayers, *ptPlayer);
    }
    MessageSendToClient(listDestPlayers, SMSG_QUEST_COUNTER, &tMessage, sizeof(tMessage));
}















void SQuestsUpdateQuestObjectiveCompleted(QuestInstance & tQuest, int32 nObjectiveIndex, BOOL bCompleted)
{
    if (!tQuest.arObjectiveCompleted[nObjectiveIndex] && bCompleted)
    {
        ScriptEventOnQuestObjectiveCompleted(QuestGetName(tQuest.snoQuest).str(), QuestStepGetName(tQuest.snoQuest, tQuest.nCurrentStepUID).str(), nObjectiveIndex);
    }
    tQuest.arObjectiveCompleted[nObjectiveIndex] = bCompleted;
}











BOOL SQuestsWorldSceneAndLabelAreRequiredByBounty(SNO snoWorld, SNO snoScene, GBID gbidLabel)
{
    QuestGlobals * ptQuestGlobals = GameGetActiveGameCommonData().ptQuestGlobals;
    POS pos = ptQuestGlobals->tQuestInstanceList.GetHeadPosition();
    while (pos)
    {
        QuestInstance & tQuestInstance = ptQuestGlobals->tQuestInstanceList.GetNext(pos);
        if (QuestIsComplete(tQuestInstance))
            continue;

        ScopedQuestDefinitionLockR pdefQuest(tQuestInstance.snoQuest);
        if (!QuestIsAnyBountyType(pdefQuest))
            continue;

        if (pdefQuest->tBountyData.snoRequiredSceneWorld == snoWorld && pdefQuest->tBountyData.snoRequiredScene == snoScene && pdefQuest->tBountyData.gbidRequiredLabel == gbidLabel)
            return TRUE;
        if (pdefQuest->tBountyData.snoRequiredSceneWorld2 == snoWorld && pdefQuest->tBountyData.snoRequiredScene2 == snoScene && pdefQuest->tBountyData.gbidRequiredLabel2 == gbidLabel)
            return TRUE;
    }
    return FALSE;
}


















BOOL SQuestsWorldAndLabelAreRequiredByBounty(SNO snoWorld, GBID gbidLabel)
{
    QuestGlobals * ptQuestGlobals = GameGetActiveGameCommonData().ptQuestGlobals;
    POS pos = ptQuestGlobals->tQuestInstanceList.GetHeadPosition();
    while (pos)
    {
        QuestInstance & tQuestInstance = ptQuestGlobals->tQuestInstanceList.GetNext(pos);
        if (QuestIsComplete(tQuestInstance))
            continue;

        ScopedQuestDefinitionLockR pdefQuest(tQuestInstance.snoQuest);
        if (!QuestIsAnyBountyType(pdefQuest))
            continue;

        if (pdefQuest->tBountyData.snoWorld == snoWorld && pdefQuest->tBountyData.gbidRequiredLabel == gbidLabel)
            return TRUE;
        if (pdefQuest->tBountyData.snoRequiredSceneWorld == snoWorld && pdefQuest->tBountyData.gbidRequiredLabel == gbidLabel)
            return TRUE;
        if (pdefQuest->tBountyData.snoWorld == snoWorld && pdefQuest->tBountyData.gbidRequiredLabel2 == gbidLabel)
            return TRUE;
        if (pdefQuest->tBountyData.snoRequiredSceneWorld2 == snoWorld && pdefQuest->tBountyData.gbidRequiredLabel2 == gbidLabel)
            return TRUE;
    }
    return FALSE;
}
























BOOL SQuestsWorldIsRequiredByBounty(SNO snoWorld, SNO & snoScene)
{
    QuestGlobals * ptQuestGlobals = GameGetActiveGameCommonData().ptQuestGlobals;
    POS pos = ptQuestGlobals->tQuestInstanceList.GetHeadPosition();
    while (pos)
    {
        QuestInstance & tQuestInstance = ptQuestGlobals->tQuestInstanceList.GetNext(pos);
        if (QuestIsComplete(tQuestInstance))
            continue;

        ScopedQuestDefinitionLockR pdefQuest(tQuestInstance.snoQuest);
        if (QuestIsAnyBountyType(pdefQuest) && pdefQuest->tBountyData.snoRequiredSceneWorld == snoWorld)
        {
            snoScene = pdefQuest->tBountyData.snoRequiredScene;
            return TRUE;
        }
    }
    snoScene = SNO_NONE;
    return FALSE;
}












BOOL SQuestsActorIsRequiredByBounty(SNO snoActor)
{
    if (snoActor == SNO_NONE)
        return FALSE;

    QuestGlobals * ptQuestGlobals = GameGetActiveGameCommonData().ptQuestGlobals;
    POS pos = ptQuestGlobals->tQuestInstanceList.GetHeadPosition();
    while (pos)
    {
        QuestInstance & tQuestInstance = ptQuestGlobals->tQuestInstanceList.GetNext(pos);
        if (QuestIsComplete(tQuestInstance))
            continue;

        ScopedQuestDefinitionLockR pdefQuest(tQuestInstance.snoQuest);
        if (QuestIsAnyBountyType(pdefQuest) && pdefQuest->tBountyData.snoRequiredActor == snoActor)
            return TRUE;
    }
    return FALSE;
}















































BOOL SQuestsAdventureIsRequiredByBounty(SNO snoAdventure)
{
    if (snoAdventure == SNO_NONE)
        return FALSE;

    QuestGlobals * ptQuestGlobals = GameGetActiveGameCommonData().ptQuestGlobals;
    POS pos = ptQuestGlobals->tQuestInstanceList.GetHeadPosition();
    while (pos)
    {
        QuestInstance & tQuestInstance = ptQuestGlobals->tQuestInstanceList.GetNext(pos);
        if (QuestIsComplete(tQuestInstance))
            continue;

        ScopedQuestDefinitionLockR pdefQuest(tQuestInstance.snoQuest);
        if (QuestIsAnyBountyType(pdefQuest) && pdefQuest->tBountyData.snoRequiredAdventure == snoAdventure)
            return TRUE;
    }
    return FALSE;
}















BOOL SQuestsEncounterIsRequiredByBounty(SNO snoEncounter)
{
    if (snoEncounter == SNO_NONE)
        return FALSE;

    QuestGlobals * ptQuestGlobals = GameGetActiveGameCommonData().ptQuestGlobals;
    POS pos = ptQuestGlobals->tQuestInstanceList.GetHeadPosition();
    while (pos)
    {
        QuestInstance & tQuestInstance = ptQuestGlobals->tQuestInstanceList.GetNext(pos);
        if (QuestIsComplete(tQuestInstance))
            continue;

        ScopedQuestDefinitionLockR pdefQuest(tQuestInstance.snoQuest);
        if (QuestIsAnyBountyType(pdefQuest) && pdefQuest->tBountyData.snoRequiredEncounter == snoEncounter)
            return TRUE;
    }
    return FALSE;
}















BOOL SQuestsLevelAreaIsRequiredByKillBounty(SNO snoLevelArea)
{
    if (snoLevelArea == SNO_NONE)
        return FALSE;

    QuestGlobals * ptQuestGlobals = GameGetActiveGameCommonData().ptQuestGlobals;
    POS pos = ptQuestGlobals->tQuestInstanceList.GetHeadPosition();
    while (pos)
    {
        QuestInstance & tQuestInstance = ptQuestGlobals->tQuestInstanceList.GetNext(pos);
        if (QuestIsComplete(tQuestInstance))
            continue;

        ScopedQuestDefinitionLockR pdefQuest(tQuestInstance.snoQuest);
        if (pdefQuest->tBountyData.snoRequiredActorLevelArea == snoLevelArea && QuestIsAnyBountyType(pdefQuest) && pdefQuest->tBountyData.snoRequiredActor != SNO_NONE)
            return TRUE;
    }
    return FALSE;
}




















SNO SQuestGetBountyQuestSNOWithActorObjective(SNO snoActor)
{

    if (snoActor == SNO_NONE)
        return SNO_NONE;

    QuestGlobals * ptQuestGlobals = GameGetActiveGameCommonData().ptQuestGlobals;
    POS pos = ptQuestGlobals->tQuestInstanceList.GetHeadPosition();

    while (pos)
    {
        QuestInstance & tQuestInstance = ptQuestGlobals->tQuestInstanceList.GetNext(pos);

        if (QuestIsComplete(tQuestInstance))
            continue;

        ScopedQuestDefinitionLockR pdefQuest(tQuestInstance.snoQuest);
        if (pdefQuest->eQuestType != QUEST_TYPE_BOUNTY)
            continue;

        int32 nStepIndex = QuestStepUIDToIndex(pdefQuest, tQuestInstance.nCurrentStepUID);

        if (nStepIndex >= 0)
        {

            const QuestStep & tStep = pdefQuest->arQuestSteps[nStepIndex];

            int32 nNumObjectiveSets = tStep.serStepObjectiveSets.ElementCount(sizeof(QuestStepObjectiveSet));

            for (int32 j = 0; j < nNumObjectiveSets; j++)
            {
                const QuestStepObjectiveSet & tObjectiveSet = tStep.arStepObjectiveSets[j];
                int32 nNumObjectives = tObjectiveSet.serStepObjectives.ElementCount(sizeof(QuestStepObjective));

                for (int32 k = 0; k < nNumObjectives; k++)
                {
                    const QuestStepObjective & tObjective = tObjectiveSet.arStepObjectives[k];

                    if ((tObjective.eObjectiveType == OBJECTIVE_KILL_MONSTER || tObjective.eObjectiveType == OBJECTIVE_INTERACT_WITH_GIZMO) && tObjective.snoname1.snoHandle == snoActor)
                    {
                        return tQuestInstance.snoQuest;
                    }
                }
            }
        }
    }

    return SNO_NONE;
}







SNO SQuestGetBountyQuestSNOWithLevelAreaObjective(SNO snoLevelArea)
{

    if (snoLevelArea == SNO_NONE)
        return SNO_NONE;

    QuestGlobals * ptQuestGlobals = GameGetActiveGameCommonData().ptQuestGlobals;
    POS pos = ptQuestGlobals->tQuestInstanceList.GetHeadPosition();

    while (pos)
    {
        QuestInstance & tQuestInstance = ptQuestGlobals->tQuestInstanceList.GetNext(pos);

        if (QuestIsComplete(tQuestInstance))
            continue;

        ScopedQuestDefinitionLockR pdefQuest(tQuestInstance.snoQuest);
        if (pdefQuest->eQuestType != QUEST_TYPE_BOUNTY)
            continue;

        int32 nStepIndex = QuestStepUIDToIndex(pdefQuest, tQuestInstance.nCurrentStepUID);

        if (nStepIndex >= 0)
        {

            const QuestStep & tStep = pdefQuest->arQuestSteps[nStepIndex];

            int32 nNumObjectiveSets = tStep.serStepObjectiveSets.ElementCount(sizeof(QuestStepObjectiveSet));

            for (int32 j = 0; j < nNumObjectiveSets; j++)
            {
                const QuestStepObjectiveSet & tObjectiveSet = tStep.arStepObjectiveSets[j];
                int32 nNumObjectives = tObjectiveSet.serStepObjectives.ElementCount(sizeof(QuestStepObjective));

                for (int32 k = 0; k < nNumObjectives; k++)
                {
                    const QuestStepObjective & tObjective = tObjectiveSet.arStepObjectives[k];

                    if (tObjective.eObjectiveType == OBJECTIVE_ENTER_LEVEL_AREA && tObjective.snoname1.snoHandle == snoLevelArea)
                    {
                        return tQuestInstance.snoQuest;
                    }
                }
            }
        }
    }

    return SNO_NONE;
}







void SQuestsCheckForBountyActorSpawned(Actor & tActor)
{
    if (tActor.snoActor == SNO_NONE)
        return;




    QuestGlobals * ptQuestGlobals = GameGetActiveGameCommonData().ptQuestGlobals;


    POS pos = ptQuestGlobals->tBountyActivationRecordList.GetHeadPosition();
    while (pos)
    {
        BountyActivationRecord & tRecord = ptQuestGlobals->tBountyActivationRecordList.GetNext(pos);

        if (!tRecord.bFlaggedForDeletion &&
            tRecord.eType == BOUNTY_ACTIVATION_ACTOR &&
            tRecord.snoActor == tActor.snoActor)
        {
            ActorCommonData & tACD = ACDGet(tActor.idACD);

            if (tACD.IsInWorld())
            {
                WorldLocation & loc = tACD.GetWorldLocation();
                SNO snoWorld = WorldCommonGetWorldSNO(loc.idSWorld);
                SNO snoLevelArea = ACDGetLevelArea(tACD);

                if (snoWorld == tRecord.snoWorld &&
                    snoLevelArea == tRecord.snoLevelArea)
                {

                    Minimap::ActorAutoPing(tActor.idACD, tRecord.snoQuest);


                    tRecord.bFlaggedForDeletion = TRUE;
                    break;
                }
            }
        }
    }
}


void SQuestsRegisterBountyActor(SNO snoActor, SNO snoWorld, SNO snoLevelArea, SNO snoQuest)
{
    if (snoActor == SNO_NONE || snoWorld == SNO_NONE || snoLevelArea == SNO_NONE || snoQuest == SNO_NONE)
        return;














    QuestGlobals * ptQuestGlobals = GameGetActiveGameCommonData().ptQuestGlobals;
    POS pos = ptQuestGlobals->tBountyActivationRecordList.GetHeadPosition();
    while (pos)
    {
        BountyActivationRecord & tRecord = ptQuestGlobals->tBountyActivationRecordList.GetNext(pos);
        if (!tRecord.bFlaggedForDeletion &&
            tRecord.eType == BOUNTY_ACTIVATION_ACTOR &&
            tRecord.snoActor == snoActor &&
            tRecord.snoWorld == snoWorld &&
            tRecord.snoLevelArea == snoLevelArea &&
            tRecord.snoQuest == snoQuest)
        {
            return;
        }
    }

    POS posNew = ptQuestGlobals->tBountyActivationRecordList.AddTail_NoAssign_Tracked();
    BountyActivationRecord & tRecord = ptQuestGlobals->tBountyActivationRecordList.GetAt(posNew);
    tRecord.eType = BOUNTY_ACTIVATION_ACTOR;
    tRecord.snoActor = snoActor;
    tRecord.snoWorld = snoWorld;
    tRecord.snoLevelArea = snoLevelArea;
    tRecord.snoQuest = snoQuest;
    tRecord.bFlaggedForDeletion = FALSE;
    ServerQueueUpdateFunction(gdwBountySceneActivationUpdateRate, ptQuestGlobals->tBountySceneActivationUpdate);
}








void SQuestsRegisterBountyLevelArea(SNO snoWorld, SNO snoLevelArea, SNO snoQuest)
{
    if (snoWorld == SNO_NONE || snoLevelArea == SNO_NONE || snoQuest == SNO_NONE)
        return;

    QuestGlobals * ptQuestGlobals = GameGetActiveGameCommonData().ptQuestGlobals;
    POS posNew = ptQuestGlobals->tBountyActivationRecordList.AddTail_NoAssign_Tracked();
    BountyActivationRecord & tRecord = ptQuestGlobals->tBountyActivationRecordList.GetAt(posNew);
    tRecord.eType = BOUNTY_ACTIVATION_LEVEL_AREA;
    tRecord.snoActor = SNO_NONE;
    tRecord.snoWorld = snoWorld;
    tRecord.snoLevelArea = snoLevelArea;
    tRecord.snoQuest = snoQuest;
    tRecord.bFlaggedForDeletion = FALSE;
    ServerQueueUpdateFunction(gdwBountySceneActivationUpdateRate, ptQuestGlobals->tBountySceneActivationUpdate);
}















void SQuestsRemoveBountyLevelArea(SNO snoLevelArea, SNO snoQuest)
{
    QuestGlobals * ptQuestGlobals = GameGetActiveGameCommonData().ptQuestGlobals;
    POS pos = ptQuestGlobals->tBountyActivationRecordList.GetHeadPosition();
    while (pos)
    {
        BountyActivationRecord & tRecord = ptQuestGlobals->tBountyActivationRecordList.GetNext(pos);
        if (!tRecord.bFlaggedForDeletion &&
            tRecord.eType == BOUNTY_ACTIVATION_LEVEL_AREA &&
            tRecord.snoLevelArea == snoLevelArea &&
            tRecord.snoQuest == snoQuest)
        {
            tRecord.bFlaggedForDeletion = TRUE;
            return;
        }
    }
}









void SQuestsUpdateBountySceneActivation()

{


    QuestGlobals *ptQuestGlobals = GameGetActiveGameCommonData().ptQuestGlobals;


    ServerUpdateWasCalled(ptQuestGlobals->tBountySceneActivationUpdate);

    SGame & tSGame = SGameManager::GameGet();

    POS pos = ptQuestGlobals->tBountyActivationRecordList.GetHeadPosition();
    while (pos)
    {
        POS posCur = pos;
        BountyActivationRecord & tRecord = ptQuestGlobals->tBountyActivationRecordList.GetNext(pos);
        // Inferred: each exit removes the record through its own RemoveAt call; the original keeps
        // XGrowableList<BountyActivationRecord>::RemoveAt out of line (four call sites tail-merged, line 0 col 13).
        if (tRecord.bFlaggedForDeletion)
        {
            ptQuestGlobals->tBountyActivationRecordList.RemoveAt(posCur);
            continue;
        }



        QuestInstance *ptQuestInstance = QuestsGetQuestInstance(tRecord.snoQuest, ACDID_INVALID);
        if (!ptQuestInstance || QuestIsComplete(*ptQuestInstance))
        {
            ptQuestGlobals->tBountyActivationRecordList.RemoveAt(posCur);
            continue;
        }

        BOOL bActivated = FALSE;
        // Inferred: a created world counts as activated (5893 success jumps straight to 5937) and a
        // failed create skips straight to RemoveAt; the scene loop and actor check sit in the 5893 block
        // (DWARF lexical block holds both tItSSceneForMarkers and pdefQuest). switch at 5904 from columns 25/41.
        SWorldID idSWorld = SWorldFindBySNO(tRecord.snoWorld);
        if (idSWorld == SWORLDID_INVALID)
        {
            idSWorld = SWorldCreate(tRecord.snoWorld, TRUE, FALSE, NULL, ACDID_INVALID, ACDID_INVALID, 0, 0, FALSE);
            bActivated = TRUE;
        }

        if (idSWorld != SWORLDID_INVALID)
        {

            TDataArrayIterator<SScene> tItSSceneForMarkers;
            while (!bActivated && tSGame.ptSScenes->Iterate(tItSSceneForMarkers))
            {
                SScene * ptSScene = tItSSceneForMarkers.ptElement;
                if (ptSScene->tSCD.idSWorld == idSWorld)
                {
                    if (!(ptSScene->tSCD.dwSceneFlags & (1 << SCENE_WAS_ACTIVATED_BEFORE)))
                    {
                        switch (tRecord.eType)
                        {
                        case BOUNTY_ACTIVATION_ACTOR:
                            {
                            for (int32 i = 0; i < 4; i++)
                            {
                                if (ptSScene->tSCD.tSceneSpec.arSnoLevelAreas[i] == tRecord.snoLevelArea)
                                {

                                    SceneCommonDataActivate(ptSScene->id);
                                    bActivated = TRUE;
                                    break;
                                }
                            }
                            }
                            break;
                        default:
                            break;
                        }
                    }
                }



            }






            if (bActivated)
            {
                if (++ptQuestGlobals->dwUpdatesSinceLastDeactivation >= BOUNTY_SCENE_DEACTIVATION_UPDATE_RATE)
                {
                    SSceneForceDeactivationCheckThisTick();
                    ptQuestGlobals->dwUpdatesSinceLastDeactivation = 0;
                }
                break;
            }
            else
            {
                // Inferred: this path ends in its own RemoveAt + continue; with only three sites clang inlines
                // RemoveAt, while the original calls it out of line. nSeed (5955) is unused,
                // only its SWorldTryToGet TLS read survives.
                if (tRecord.eType == BOUNTY_ACTIVATION_ACTOR)
                {
                    ScopedQuestDefinitionLockR pdefQuest(tRecord.snoQuest);
                    if (pdefQuest->tBountyData.snoRequiredActor == tRecord.snoActor)
                    {
                        SWorld * ptSWorld = SWorldTryToGet(idSWorld);
                        int nSeed = ptSWorld ? ptSWorld->nSeed : 0;
                        (void)nSeed;
                    }
                }

                ptQuestGlobals->tBountyActivationRecordList.RemoveAt(posCur);
                continue;
            }
        }

        ptQuestGlobals->tBountyActivationRecordList.RemoveAt(posCur);
    }














    if (ptQuestGlobals->tBountyActivationRecordList.Count() > 0)
    {
        ServerQueueUpdateFunction(gdwBountySceneActivationUpdateRate, ptQuestGlobals->tBountySceneActivationUpdate);
    }
}
