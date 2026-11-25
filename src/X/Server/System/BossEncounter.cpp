// X/Server/System/BossEncounter.cpp -- the per-game boss encounter (BossEncounterState on SGame). Partly
// written; every function sits on its original line. FLAG_*/ESF_* bits: EncounterTypes.h.
#include "BossEncounter.h"

#include "EncounterTypes.h"
#include "../../Common/ActorCommonData.h"
#include "../../Common/Attributes/FastAttribConstantsSwig.h"
#include "../../Common/Attributes/FastAttributeStructs.h"
#include "../../Common/CommonTypes.h"
#include "../../Common/Game.h"
#include "../../Common/GameConstants.h"
#include "../../Common/GameSwig.h"
#include "../../Common/Networking/GameMessage.h"
#include "../../Common/Player.h"
#include "../../Common/SceneCommon.h"
#include "../../Sigma/DataStructures/DataStructuresExternal.h"
#include "../LuaScript/LuaScript.h"
#include "../PlayerList.h"
#include "../SGame.h"
#include "../SGameManager.h"
#include "../SWorld.h"
#include "../Timers.h"
#include "SetEncounter.h"
#include "../../Common/Console/Avenger.h"
#include "../../Common/TagMaps/AnimTagsSwig.h"
#include "../Actors/ActorPowers.h"
#include "../Powers/PowerAPI.h"
#include "../Powers/Powers.h"
#include "../../../Contrib/Contrib/lua/src/lua.h"
#include "../../Common/Definitions/BossEncounterDefinition.h"
#include "../../Common/Definitions/QuestRangeDefinition.h"
#include "../LuaScript/LuaScriptUtil.h"
#include "../LuaScript/ScriptEvent.h"
#include "../Actors/Actors.h"
// Declared locally until their headers are adopted; the comment names the defining file.
void STrickleTriggerUpdate(); void SWorldPlayerWarpToLastCheckpoint(const Player & tPlayer); BOOL SPlayersAreAllLocal();  // X/Server/Network/STrickle.cpp:129; X/Server/SWorld.cpp:866; X/Server/SPlayer.h:69
BOOL GameActiveGameCommonDataIsServer();  // X/Common/Game.cpp:923
SWorldID WorldCommonFindBySNO(SNO snoWorld, BOOL bIgnorePendingDelete);  // X/Common/World.cpp:605
int32 LuaSNOScriptCall(SNO snoScript, lua_State * L, int32 nArgumentsCnt, int32 nReturnValCnt, LPCSTR szFuncName); lua_State * LuaScriptGetLuaStateWithSNO(SNO snoScript);  // X/Server/LuaScript/LuaScript.cpp:1084, :3353
void SGameStatsTimedEventFinish(int32 eType, int32 nParam0, int32 nParam1); void SGameStatsCounterIncrement(int32 eType, int32 nParam0, int32 nParam1, float flValue); void SGameStatsTimedEventStart(int32 eType, int32 nParam0, int32 nParam1, BOOL bReportIfUnfinished);  // X/Server/SGameStats.cpp:1484, :1504, :1463
void SWorldRemovePlayersAndFlagForDeletion(SWorldID idSWorld); BOOL SQuestsAdvanceTo(SNO snoQuest, int32 nStepUID, BOOL bAllowRollback); void QuestsAddOpenWorldTrackedSNO(SNO sno);  // X/Server/SWorld.cpp; X/Server/Quests/SQuests.cpp:3575; X/Common/Quests/Quests.cpp
enum { STAT_COUNTER_BOSS_ENCOUNTER_STARTED = 3, STAT_TIMED_EVENT_BOSS_ENCOUNTER = 1, STAT_TIMED_EVENT_BOSS_ENCOUNTER_FIGHT = 2 };  // invented names; X/Server/SGameStatsSwig.h
void BrainAPIDoDeath(ActorID idDeadActor, ActorID idKillerActor, const BOOL fNoExp, const BOOL fNoLoot, const BOOL fNoHealthDrop, PowerAPIDamage * ptDamage, float * pflDamageDone);  // X/Server/AI/BrainComponents.h:63
namespace NephalemRift { BOOL IsTieredRiftActive(); }  // X/Common/NephalemRiftCommon.cpp:161
namespace ConversationMgr { void CancelConversationForPlayer(const Player & tPlayer); }  // X/Server/ConversationMgr.cpp:3100
BOOL ACDIsParticipatingInTieredLootRun(const ActorCommonData & tACD);  // X/Common/ActorCommonData.cpp:9315
void SceneCommonDataActivate(const SSceneID idSScene);  // X/Common/SceneCommon.cpp:729
uint32 AnimPlayerPlayAnimation(ActorCommonData & tACD, uint32 dwAnimationTag, uint32 dwLength, BOOL bUseAnimationTranslation, int32 eAnimReason, float flUnitAnimStartTime, const BOOL bDontSendToClient, float flSpeedScalar);  // X/Common/AnimPlayer.cpp:1224
void BrainAPIDoResurrect(ActorID idActor, BOOL bForce);  // X/Server/AI/BrainComponents.cpp:7464
void BrainAPIDoIdle(ActorID idActor);  // X/Server/AI/BrainComponents.cpp:3448
void AIQueueCompleteCurrentAction(ActorID idActor);  // X/Server/AI/ActorAI.cpp:2653
void AIQueueClearList(ActorID idActor);  // X/Server/AI/ActorAI.cpp:2603
void SPlayerSetLastWaypointLevelArea(Player * ptPlayer, const SNO snoLevelArea);  // X/Server/SPlayer.cpp:12427
void SGameACDChangeWorld(const ACDID idACD, const SNO snoWorld, const uint32 uEntranceGUID, PlayerWarpReason eReason, const float flWarpFadeInSecs, const Player * ptReferencePlayer);  // X/Server/SGame.cpp:7358
PlayerAvengerData * GetPlayerAvengerData(Player * ptPlayer, AvengerSource eAvengerSource);  // X/Common/Player.cpp:1634
enum {
    ANIM_REASON_SPAWN = 4,  // X/Common/AnimPlayer.h
}; 
namespace BossEncounterManager {

void StartEncounter(BOOL bDelayedEntry); void PortAllPlayers(); void OnTick(TimerID, DataID, void *); void FinishEncounterSetup(TimerID, DataID, void *);
void TriggerCleanup(); void TriggerComplete(); void PromptAllOtherPlayers(const Player * ptInstigator); BOOL IsAnyEncounterActive();
void PlayerVoteCancel(Player * ptPlayer); void PlayerDeclineInvite(Player * ptPlayer); void PlayerDeclineStart(Player * ptPlayer);















class BossEncounterState : public EncounterState
{
public:
    BossEncounterState()
    {
        m_snoEncounter = SNO_NONE;
        dwLastCompleteTime = 0;
        idSWorldOrigin = SWORLDID_INVALID;
    }


    virtual void ClearFlags(Player * ptPlayer)
    {

        ptPlayer->dwBossEncounterFlags = 0;

        ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
        if (ptACDPlayer != NULL)
        {
            ptACDPlayer->AttributesSetInt(ATTRIBUTE_IN_BOSS_ENCOUNTER, 0);
        }
    }


    virtual void ResetState()
    {
        EncounterState::ResetState();
        m_snoEncounter = SNO_NONE;
        dwLastCompleteTime = 0;
        idSWorldOrigin = SWORLDID_INVALID;
    }

    SNO m_snoEncounter;
    uint32 dwLastCompleteTime;
    SWorldID idSWorldOrigin;
};





inline void lua_pushplayers(lua_State * L, uint32 dwEncounterMask)
{
    lua_newtable(L);
    int32 playerIndex = 1;
    for (const Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer != NULL; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {
        if (ptPlayer->dwBossEncounterFlags & dwEncounterMask)
        {
            lua_pushnumber(L, playerIndex);
            lua_pushnumber(L, ptPlayer->idPlayerACD);
            lua_rawset(L, -3);
        }
    }
}







inline BossEncounterState * GetGameState()
{
    BossEncounterState * pState = SGameManager::GameGet().ptBossEncounterState;
    return pState;
}



uint32 GetLastCompleteTime()
{
    return GetGameState()->dwLastCompleteTime;
}



void SendEncounterInviteStateMessage()
{
    EncounterInviteStateMessage tMessage;

    tMessage.Clear();

    PlayerList listPlayers(PlayerListGetElementPool());
    PlayerListGetAllInGame(listPlayers, -1, FALSE, -1);

    POS pos = listPlayers.GetHeadPosition();
    while (pos != NULL)
    {
        POS oldPos = pos;
        const Player * ptPlayer = listPlayers.GetNext(pos);
        if (ptPlayer->dwBossEncounterFlags == 0)
        {
            listPlayers.RemoveAt(oldPos);
            continue;
        }
        if (ptPlayer->dwBossEncounterFlags & (1 << ESF_DECLINED)) tMessage.m_DeclinedPlayers |= (1 << ptPlayer->nPlayerIndex); else tMessage.m_DeclinedPlayers &= ~(1 << ptPlayer->nPlayerIndex);
        if (ptPlayer->dwBossEncounterFlags & (1 << ESF_ACCEPTED)) tMessage.m_AcceptedPlayers |= (1 << ptPlayer->nPlayerIndex); else tMessage.m_AcceptedPlayers &= ~(1 << ptPlayer->nPlayerIndex);
        if (ptPlayer->dwBossEncounterFlags & (1 << ESF_PENDING_ACCEPT)) tMessage.m_PendingPlayers |= (1 << ptPlayer->nPlayerIndex); else tMessage.m_PendingPlayers &= ~(1 << ptPlayer->nPlayerIndex);
    }

    MessageSendToClient(listPlayers, SMSG_BOSS_ENCOUNTER_INVITE_STATE, &tMessage, sizeof(tMessage));
}




void SendCancelMessage(TimerID, DataID, void *)
{
    PlayerList listPlayers(PlayerListGetElementPool());
    PlayerListGetAllInGame(listPlayers, -1, FALSE, -1);
    SimpleMessage tMessage;
    MessageSendToClient(listPlayers, SMSG_BOSS_ENCOUNTER_CANCELED, &tMessage, sizeof(tMessage));
}





inline void CancelStart(int32 nMessageDelay)
{
    BossEncounterState * pState = GetGameState();
    SendEncounterInviteStateMessage();
    TimerRegister(DATAID_INVALID, TIMER_OWNER_NONE, nMessageDelay, NULL, SendCancelMessage);
    pState->ResetState();
}








void OnVoteTimeOut(TimerID, DataID, void *)
{
    BossEncounterState * pState = GetGameState();
    for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer != NULL; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {
        if (ptPlayer->dwBossEncounterFlags & (1 << ESF_PENDING_ACCEPT))
        {
            PlayerVoteCancel(ptPlayer);
        }
        if (pState->dwFlags & (1 << FLAG_IN_PROGRESS))
        {
            return;
        }
    }
    CancelStart(2 * TICKS_PER_SECOND);
}









void HandleConditionalStart()
{
    BossEncounterState * pState = GetGameState();
    pState->dwFlags = (1 << FLAG_PROMPT_CONTINUE_WITHOUT);
    pState->nDeclinedInvites = 0;
    PlayerList listPlayers(PlayerListGetElementPool());
    PlayerListGetAllInGame(listPlayers, -1, FALSE, -1);
    IntDataMessage tMessage;
    tMessage.nData = 0;
    POS pos = listPlayers.GetHeadPosition();
    while (pos != NULL)
    {
        POS oldPos = pos;
        Player * ptPlayer = listPlayers.GetNext(pos);
        if (ptPlayer->dwBossEncounterFlags & (1 << ESF_ACCEPTED))
        {
            ptPlayer->dwBossEncounterFlags &= ~(1 << ESF_INSTIGATOR);
            ptPlayer->dwBossEncounterFlags |= (1 << ESF_PENDING_ACCEPT) | (1 << ESF_CONTINUE_WITHOUT);
            continue;
        }
        if (ptPlayer->dwBossEncounterFlags & (1 << ESF_DECLINED))
        {
            tMessage.nData |= (1 << ptPlayer->nPlayerIndex);
        }
        pState->ClearFlags(ptPlayer);
        listPlayers.RemoveAt(oldPos);
    }

    MessageSendToClient(listPlayers, SMSG_BOSS_ENCOUNTER_ONE_DECLINED, &tMessage, sizeof(tMessage));

    if (TimerExists(pState->idTickTimer))
    {
        TimerUnregister(pState->idTickTimer);
    }
    pState->idTickTimer = TimerRegister(DATAID_INVALID, TIMER_OWNER_NONE, 15 * TICKS_PER_SECOND, NULL, OnVoteTimeOut);
}








inline void TryStartEncounter()
{
    for (const Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer != NULL; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {
        if (ptPlayer->dwBossEncounterFlags & ((1 << ESF_PENDING_ACCEPT) | (1 << ESF_CONTINUE_WITHOUT)))
        {
            return;
        }
    }
    StartEncounter(FALSE);
}





void PortAllPlayers()
{

    BossEncounterState * pState = GetGameState();

    ScopedBossEncounterDefinitionLockR pdefEncounter(pState->m_snoEncounter);
    SNO snoTargetWorld = pdefEncounter->arrWorlds[0];
    if (snoTargetWorld == SNO_NONE)
    {
        return;
    }

    SWorldID idSWorld = WorldCommonFindBySNO(snoTargetWorld, TRUE);
    if (idSWorld == SWORLDID_INVALID)
    {
        return;
    }





    SWorld * ptSWorld = SWorldTryToGet(idSWorld);
    if (ptSWorld != NULL)
    {
        ptSWorld->snoBossEncounter = pState->m_snoEncounter;
    }

    for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer != NULL; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {
        if (ptPlayer->dwBossEncounterFlags & (1 << ESF_ACCEPTED))
        {
            if (PlayerIsHardcoreAndDead(*ptPlayer))
            {




                XASSERT(FALSE);
                return;
            }
            ptPlayer->dwBossEncounterFlags &= ~(1 << ESF_ACCEPTED);
            ptPlayer->dwBossEncounterFlags |= (1 << ESF_ACTIVE);

            ActorCommonData & tACDPlayer = ACDGet(ptPlayer->idPlayerACD);
            tACDPlayer.AttributesSetInt(ATTRIBUTE_IN_BOSS_ENCOUNTER, 1);

            if (tACDPlayer.GetPlace().idSWorld != idSWorld)
            {
                if (!tACDPlayer.IsAlive())
                {
                    BrainAPIDoResurrect((ActorID)tACDPlayer.idOwner, TRUE);
                }

                PowerAPI_BuffRemoveAllNegativeBuffs(tACDPlayer);

                ActorID idActor = (ActorID)tACDPlayer.idOwner;
                PowerID idPowerPrimary = ActorPowersGetPrimary(idActor);
                if (idPowerPrimary != POWERID_INVALID)
                {
                    PowerForceStop(idPowerPrimary, TRUE);
                }

                AIQueueCompleteCurrentAction(idActor);
                AIQueueClearList(idActor);
                BrainAPIDoIdle(idActor);

                AnimPlayerPlayAnimation(tACDPlayer, TAG_ACTOR_ANIMATION_NEUTRAL, (uint32)-1, FALSE, ANIM_REASON_SPAWN, 0.0f, FALSE, 1.0f);

                if (pState->snoLastWaypointLevelArea != SNO_NONE)
                {
                    SPlayerSetLastWaypointLevelArea(ptPlayer, pState->snoLastWaypointLevelArea);
                }

                uint32 uEntranceGUID = pdefEncounter->uEntranceGUID;
                SGameACDChangeWorld(tACDPlayer.id, snoTargetWorld, uEntranceGUID, WARP_BOSS_FIGHT, pdefEncounter->flFadeInTime, NULL);

                SNO snoBossLevelArea = ACDGetLevelArea(tACDPlayer);
                PlayerAvengerData * ptPlayerAvengerData = GetPlayerAvengerData(ptPlayer, ptPlayer->eAvengerSource);
                ptPlayerAvengerData->snoBossLevelArea = snoBossLevelArea;
            }
        }
    }
}








static BOOL sIsBossEncounterWorld(
    const BossEncounterDefinition * pdefEncounter,
    SWorldID idSWorld)
{
    if (idSWorld == SWORLDID_INVALID)
    {
        return FALSE;
    }

    for (int32 i = 0; i < NUM_BOSS_WORLDS; i++)
    {
        SNO snoWorldEncounter = pdefEncounter->arrWorlds[i];
        SWorldID idSWorldEncounter = WorldCommonFindBySNO(snoWorldEncounter, TRUE);
        if (idSWorldEncounter == idSWorld)
        {
            return TRUE;
        }
    }

    return FALSE;
}


    void TriggerCleanup()

    {
        BossEncounterState * pState = GetGameState();
        ScopedBossEncounterDefinitionLockR pdefEncounter(pState->m_snoEncounter);


        Player * ptPlayer = PlayerGetFirstInGame();
        while (ptPlayer != NULL)
        {

            ActorCommonData & tACDPlayer = ACDGet(ptPlayer->idPlayerACD);
            if (tACDPlayer.IsAlive() && tACDPlayer.GetPlace().idSWorld)  // inferred: cbz on WorldPlace+0xc (idSWorld) in the original
            {
                float flMaxHP = tACDPlayer.GetHitPointsMaxTotal();
                tACDPlayer.SetHitPoints(flMaxHP, TRUE, FALSE);
            }

            ptPlayer = PlayerGetNextInGame(ptPlayer);
        }


        ActorIterator tIt;
        while (ActorDataArrayIterate(tIt))
        {
            Actor * ptActor = tIt.ptElement;
            ActorCommonData * ptACD = ACDTryToGet(ptActor->idACD);

            if (ptACD != NULL &&
                ptACD->IsMonster() &&
                ptACD->IsAlive() &&
                !ptACD->IsNeverTargetable() &&
                TeamIsMonsterTeam(ptACD->GetNaturalTeam()) &&
                sIsBossEncounterWorld(pdefEncounter, ptACD->GetLocation().tWorldLocation.idSWorld))
            {
                ActorID idActor = (ActorID)ACDGetOwner(ptACD->id);
                Actor * ptActorScoped = ActorGet(idActor);
                if (ptActorScoped->ptAI)
                {
                    BrainAPIDoDeath(idActor, ACTORID_INVALID, TRUE, TRUE, TRUE, NULL, NULL);
                }
            }
        }


        PowerGlobals * ptPowerGlobals = SGameManager::GameGet().ptPowerGlobals;
        TDataArrayIterator<Power> tItPower;

        while (ptPowerGlobals->pArrayPowers->Iterate(tItPower))
        {
            Power * ptPower = tItPower.ptElement;
            if (sIsBossEncounterWorld(pdefEncounter, ptPower->wpStartLocation.idSWorld) && TeamIsMonsterTeam(ptPower->eTeam))
            {
                FastAttribSetValueInt(ptPower->idFastAttrib, ATTRIBUTE_DOES_NO_DAMAGE, TRUE);

                SpecificPowerSendMessage(ptPower->id, POWER_BOSS_ENCOUNTER_CLEANUP, NULL);

                POS posBuffs = ptPower->listBuffInstances.GetHeadPosition();
                while (posBuffs)
                {
                    BuffID idBuff = ptPower->listBuffInstances.GetNext(posBuffs);
                    PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
                    if (ptBuff &&
                        (!ptBuff->bPositiveEffect || ptBuff->flDamagePerSecond > EPSILON))
                    {
                        PowerAPI_BuffFlagForDeletion(ptBuff, FALSE, TRUE, FALSE, TRUE);
                    }
                }
            }
        }
    }


void TriggerComplete()
{

    BossEncounterState * pState = GetGameState();
    if (pState->dwFlags & (1 << FLAG_COMPLETING))
    {
        return;
    }

    pState->dwFlags |= (1 << FLAG_COMPLETING);

    ScopedBossEncounterDefinitionLockR pdefEncounter(pState->m_snoEncounter);

    SNO snoScript = pdefEncounter->arrScripts[BOSS_SCRIPT_COMPLETE];
    if (snoScript != SNO_NONE)
    {
        lua_State * L = LuaScriptGetLuaStateWithSNO(snoScript);
        if (LuaScriptGetFunction(L, "OnComplete"))
        {


            lua_pushplayers(L, (1 << ESF_ACTIVE));
            lua_pushplayers(L, (1 << ESF_FORMER_PARTICIPANT));
            LuaSNOScriptCall(snoScript, L, 2, 0, "OnComplete");
        }
    }

    if (TimerExists(pState->idTickTimer))
    {
        TimerUnregister(pState->idTickTimer);
    }

    SGameStatsTimedEventFinish(STAT_TIMED_EVENT_BOSS_ENCOUNTER, pState->m_snoEncounter, 0);
    SGameStatsTimedEventFinish(STAT_TIMED_EVENT_BOSS_ENCOUNTER_FIGHT, pState->m_snoEncounter, 0);

    if (pdefEncounter->bCleanupMonstersAndPowers)
    {
        TriggerCleanup();
    }

    SGameGlobals & tSGameGlobals = SGameGlobalsGet();
    tSGameGlobals.listCompletedBossEncounters.AddHead_Tracked(pState->m_snoEncounter);
    if (GameIsOpenWorld())
    {
        QuestsAddOpenWorldTrackedSNO(pState->m_snoEncounter);
    }

    pState->ResetState();
    pState->dwLastCompleteTime = GameGetServerTime();
}





void TriggerReset()
{

    BossEncounterState * pState = GetGameState();
    SNO snoEncounter = pState->m_snoEncounter;
    ScopedBossEncounterDefinitionLockR pdefEncounter(snoEncounter);

    SNO snoScript = pdefEncounter->arrScripts[BOSS_SCRIPT_RESET];
    if (snoScript != SNO_NONE)
    {
        lua_State * L = LuaScriptGetLuaStateWithSNO(snoScript);
        if (LuaScriptGetFunction(L, "OnReset"))
        {


            lua_pushplayers(L, (1 << ESF_FORMER_PARTICIPANT));
            LuaSNOScriptCall(snoScript, L, 1, 0, "OnReset");
        }
    }

    if (TimerExists(pState->idTickTimer))
    {
        TimerUnregister(pState->idTickTimer);
    }

    SGameStatsTimedEventFinish(STAT_TIMED_EVENT_BOSS_ENCOUNTER, pState->m_snoEncounter, 0);

    ScriptEventOnBossEncounterReset(snoEncounter);

    pState->ResetState();

    if (pdefEncounter->bResetOnFail)
    {
        for (int32 i = 0; i < NUM_BOSS_WORLDS; i++)
        {
            SNO snoWorld = pdefEncounter->arrWorlds[i];
            SWorldID idSWorld = WorldCommonFindBySNO(snoWorld, TRUE);
            if (idSWorld != SWORLDID_INVALID)
            {
                SWorldRemovePlayersAndFlagForDeletion(idSWorld);
            }
        }

        if (pdefEncounter->bResetQuestStep && pdefEncounter->snoQuestRange != SNO_NONE)
        {
            ScopedQuestRangeDefinitionLockR pdefQuestRange(pdefEncounter->snoQuestRange);
            if (pdefQuestRange->nQuestRangeCount > 0)
            {
                SQuestsAdvanceTo(pdefQuestRange->arQuestRanges[0].tStart.snoQuest, pdefQuestRange->arQuestRanges[0].tStart.nStepUID, TRUE);
            }
        }
    }
}






















    void OnTick(
        TimerID,
        DataID,
        void *)
    {
        BossEncounterState * pState = GetGameState();
        if (!(pState->dwFlags & (1 << FLAG_IN_PROGRESS)))
        {
            return;
        }

        ScopedBossEncounterDefinitionLockR pdefEncounter(pState->m_snoEncounter);


        if (!GameIsOpenWorld())
        {
            QuestRangeResult eResult;
            QuestRangeEvaluate(pdefEncounter->snoQuestRange, &eResult);


            if (eResult == QRR_EXPIRED)
            {
                TriggerComplete();
                return;
            }
        }

        SWorldID arrWorlds[NUM_BOSS_WORLDS] = { SWORLDID_INVALID };
        int32 nActiveWorlds = 0;

        for (int32 i = 0; i < NUM_BOSS_WORLDS; i++)
        {
            SWorldID idSWorld = WorldCommonFindBySNO(pdefEncounter->arrWorlds[i], TRUE);
            if (idSWorld != SWORLDID_INVALID)
            {
                SWorld * ptSWorld = SWorldTryToGet(idSWorld);
                if (ptSWorld != NULL)
                {
                    ptSWorld->snoBossEncounter = pState->m_snoEncounter;

                    arrWorlds[nActiveWorlds++] = idSWorld;
                }
            }
        }

        int32 nActivePlayers = 0;
        int32 nAlivePlayers = 0;

        for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
        {
            const ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
            if (ptACDPlayer == NULL)
            {
                pState->ClearFlags(ptPlayer);
                continue;
            }

            SWorldID idSWorldPlayer = ptACDPlayer->GetPlace().idSWorld;
            int32 j;
            for (j = 0; j < nActiveWorlds; j++)
            {
                if (arrWorlds[j] == idSWorldPlayer)
                {
                    nActivePlayers++;
                    if (ptACDPlayer->IsAlive())
                    {
                        nAlivePlayers++;
                    }

                    if (!(ptPlayer->dwBossEncounterFlags & (1 << ESF_ACTIVE)))
                    {



                        SWorldPlayerWarpToLastCheckpoint(*ptPlayer);
                    }

                    break;
                }
            }
            if (j == nActiveWorlds)
            {
                pState->ClearFlags(ptPlayer);
                ptPlayer->dwBossEncounterFlags &= ~(1 << ESF_INSTIGATOR);
                ptPlayer->dwBossEncounterFlags |= (1 << ESF_FORMER_PARTICIPANT);
            }
        }

        if (nActivePlayers == 0)
        {
            TriggerReset();
        }
        if (nActivePlayers != 0 && nAlivePlayers == 0 &&  // inferred: line-0 tests; != 0 (not > 0) matches the original branches
            !(pState->dwFlags & (1 << FLAG_ALL_DEAD)))
        {
            ScopedGlobalDefinitionLockR pdefGlobal;

            pState->dwFlags |= (1 << FLAG_ALL_DEAD);
            uint32 dwNow = GameGetServerTime();
            uint32 dwDelay = 0;  // inferred: DWARF local with no location; its initializer left no code, value unknown
            for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
            {
                if (!(ptPlayer->dwBossEncounterFlags & (1 << ESF_ACTIVE)))
                {
                    continue;
                }

                ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
                if (ptACDPlayer == NULL)
                {
                    continue;
                }






                ptACDPlayer->AttributesSetInt(ATTRIBUTE_CHECKPOINT_RESURRECTION_ALLOWED_GAMETIME, 0);


                TimerUnregister(ptPlayer->idTimerCorpseResurrect);
                ptPlayer->idTimerCorpseResurrect = TIMERID_INVALID;
            }
            STrickleTriggerUpdate();  // inferred: line-0 call shared by both all-dead branches (tail-merged)
        }

        else if (nAlivePlayers > 0 && pState->dwFlags & (1 << FLAG_ALL_DEAD))
        {
            pState->dwFlags &= ~(1 << FLAG_ALL_DEAD);


            for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
            {
                if (!(ptPlayer->dwBossEncounterFlags & (1 << ESF_ACTIVE)))
                {
                    continue;
                }

                ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
                if (ptACDPlayer == NULL)
                {
                    continue;
                }

                TimerUnregister(ptPlayer->idTimerForcedResurrect);
                ptPlayer->idTimerForcedResurrect = TIMERID_INVALID;
                ptACDPlayer->AttributesSetInt(ATTRIBUTE_CHECKPOINT_RESURRECTION_FORCED_GAMETIME, 0);
            }

            STrickleTriggerUpdate();  // inferred: line-0 call shared by both all-dead branches (tail-merged)
        }


        pState->idTickTimer = TimerRegister(DATAID_INVALID, TIMER_OWNER_NONE, TICKS_PER_SECOND / 2, NULL, OnTick);
    }


void FinishEncounterSetup(TimerID, DataID, void *)
{

    BOOL bAbort = TRUE;
    for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer != NULL; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {
        if (PlayerIsHardcoreAndDead(*ptPlayer))
        {
            ptPlayer->dwBossEncounterFlags &= ~(1 << ESF_ACCEPTED);
        }

        if (ptPlayer->dwBossEncounterFlags & (1 << ESF_ACCEPTED))
        {
            bAbort = FALSE;
        }
    }







    if (bAbort)
    {
        CancelStart(2 * TICKS_PER_SECOND);
        return;
    }

    PortAllPlayers();


    BossEncounterState * pState = GetGameState();
    ScopedBossEncounterDefinitionLockR pdefEncounter(pState->m_snoEncounter);

    SNO snoScript = pdefEncounter->arrScripts[BOSS_SCRIPT_START];
    if (snoScript != SNO_NONE)
    {
        lua_State * L = LuaScriptGetLuaStateWithSNO(snoScript);
        if (LuaScriptGetFunction(L, "OnStart"))
        {


            lua_pushplayers(L, (1 << ESF_ACTIVE));
            LuaSNOScriptCall(snoScript, L, 1, 0, "OnStart");
        }
    }

    if (TimerExists(pState->idTickTimer))
    {
        TimerUnregister(pState->idTickTimer);
    }
    pState->idTickTimer = TimerRegister(DATAID_INVALID, TIMER_OWNER_NONE, TICKS_PER_SECOND / 2, NULL, OnTick);

    SGameStatsCounterIncrement(STAT_COUNTER_BOSS_ENCOUNTER_STARTED, pState->m_snoEncounter, 0, 1.0f);
    SGameStatsTimedEventStart(STAT_TIMED_EVENT_BOSS_ENCOUNTER, pState->m_snoEncounter, 0, TRUE);
    SGameStatsTimedEventStart(STAT_TIMED_EVENT_BOSS_ENCOUNTER_FIGHT, pState->m_snoEncounter, 0, FALSE);
}


void StartEncounter(BOOL bDelayedEntry)
{

    BossEncounterState * pState = GetGameState();
    pState->dwFlags = 0;
    if (pState->nDeclinedInvites != 0)
    {
        HandleConditionalStart();
        return;
    }

    pState->dwFlags = (1 << FLAG_IN_PROGRESS);

    PlayerList listPlayers(PlayerListGetElementPool());
    PlayerListGetAllInGame(listPlayers, -1, FALSE, -1);
    SimpleMessage tMessage;
    MessageSendToClient(listPlayers, SMSG_BOSS_ENCOUNTER_STARTED, &tMessage, sizeof(tMessage));

    if (TimerExists(pState->idTickTimer))
    {
        TimerUnregister(pState->idTickTimer);
    }

    if (!bDelayedEntry)
    {
        TimerID idTimer;
        DataID idData;
        void * pData;
        FinishEncounterSetup(idTimer, idData, pData);
    }
    else
    {
        pState->idTickTimer = TimerRegister(DATAID_INVALID, TIMER_OWNER_NONE, 1, NULL, FinishEncounterSetup);
    }
}





void InviteTimeOutCallback(TimerID, DataID, void *)
{
    for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer != NULL; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {
        if (ptPlayer->dwBossEncounterFlags & (1 << ESF_PENDING_ACCEPT))
        {
            PlayerDeclineInvite(ptPlayer);
        }
    }
}





void PromptAllOtherPlayers(const Player * ptInstigator)
{
    PlayerList listPlayers(PlayerListGetElementPool());
    PlayerListGetAllInGameRemote(listPlayers, ptInstigator->idGameConnection, ptInstigator->nPlayerIndex, TRUE);

    if (listPlayers.Count() == 0)
    {
        return;
    }

    BossEncounterState * pState = GetGameState();
    SWorld * ptSWorldOrigin = SWorldTryToGet(pState->idSWorldOrigin);

    POS pos = listPlayers.GetHeadPosition();
    while (pos != NULL)
    {
        POS posPlayer = pos;
        Player * ptPlayer = listPlayers.GetNext(pos);

        pState->ClearFlags(ptPlayer);

        if (ptSWorldOrigin != NULL && !SWorldPlayerAllowed(pState->idSWorldOrigin, *ptPlayer))
        {
            listPlayers.RemoveAt(posPlayer);
            continue;
        }

        if (NephalemRift::IsTieredRiftActive() || SetEncounterManager::IsAnyEncounterActive())
        {
            ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
            if (ptACDPlayer != NULL && (ACDIsParticipatingInTieredLootRun(*ptACDPlayer) || ACDIsParticipatingInSetDungeon(*ptACDPlayer)))
            {
                listPlayers.RemoveAt(posPlayer);
                continue;
            }
        }

        ConversationMgr::CancelConversationForPlayer(*ptPlayer);

        ptPlayer->dwBossEncounterFlags = (1 << ESF_PENDING_ACCEPT);
    }

    pState->dwFlags |= (1 << FLAG_INVITE_PHASE);

    BossEncounterMessage tMessage;
    tMessage.nPlayerIndex = ptInstigator->nPlayerIndex;
    tMessage.snoEncounter = pState->m_snoEncounter;
    MessageSendToClient(listPlayers, SMSG_BOSS_JOIN_ENCOUNTER, &tMessage, sizeof(tMessage));

    if (TimerExists(pState->idTickTimer))
    {
        TimerUnregister(pState->idTickTimer);
    }

    pState->idTickTimer = TimerRegister(DATAID_INVALID, TIMER_OWNER_NONE, 90 * TICKS_PER_SECOND, NULL, InviteTimeOutCallback);

    SendEncounterInviteStateMessage();
}



















BOOL IsEncounterCompleted(SNO snoEncounter)
{
    SGameGlobals & tSGameGlobals = SGameGlobalsGet();
    POS pos = tSGameGlobals.listCompletedBossEncounters.GetHeadPosition();
    while (pos != NULL)
    {
        SNO snoCompletedBossEncounter = tSGameGlobals.listCompletedBossEncounters.GetNext(pos);
        if (snoCompletedBossEncounter == snoEncounter)
        {
            return TRUE;
        }
    }
    return FALSE;
}



BOOL CanTriggerEncounter(SNO snoEncounter)
{
    BossEncounterState * pState;

    if (snoEncounter == SNO_NONE)
    {
        return FALSE;
    }



    ScopedBossEncounterDefinitionLockR pdefEncounter(snoEncounter);
    if (GameIsOpenWorld())
    {
        if (pdefEncounter->bAlwaysWorkAsPortalInOpenWorldGames)
        {
            return TRUE;
        }

        SGameGlobals & tSGameGlobals = SGameGlobalsGet();
        POS pos = tSGameGlobals.listCompletedBossEncounters.GetHeadPosition();
        while (pos != NULL)
        {
            SNO snoCompletedBossEncounter = tSGameGlobals.listCompletedBossEncounters.GetNext(pos);
            if (snoCompletedBossEncounter == snoEncounter)
            {
                return TRUE;
            }
        }
    }
    else
    {
        QuestRangeResult eResult;
        if (!QuestRangeEvaluate(pdefEncounter->snoQuestRange, &eResult))
        {
            if (eResult == QRR_EXPIRED)
            {
                return TRUE;
            }
            if (eResult == QRR_NOT_REACHED_YET && pdefEncounter->bWorkAsPortalBeforeQuestRange)
            {
                return TRUE;
            }
            return FALSE;
        }
    }


    pState = GetGameState();
    if (!IsAnyEncounterActive() || ((pState->dwFlags & (1 << FLAG_START_QUEUED)) && pState->m_snoEncounter == snoEncounter))
    {





        const GameSyncedData & tGameSyncedData = GameGetActiveGameCommonData().tGameSyncedData;
        if (!GameIsOpenWorld() || tGameSyncedData.dwOpenWorldTransitionTime == (uint32)-1)
        {
            return TRUE;
        }
    }
    return FALSE;
}
BOOL BossWorldCreate(SNO snoWorld)
{
    SWorldID idSWorld = WorldCommonFindBySNO(snoWorld, TRUE);
    if (idSWorld == SWORLDID_INVALID)
    {
        idSWorld = SWorldCreate(snoWorld, TRUE, FALSE, NULL, ACDID_INVALID, ACDID_INVALID, 0, 0, 0, 0);
        SceneCommonData * ptSCD = NULL;
        SceneCommonDataIterator tIterator;
        while (SceneCommonDataArrayIterate(tIterator, ptSCD, SkipHolodeckScenes))
        {
            if (ptSCD->idSWorld == idSWorld)
            {
                SceneCommonDataActivate(ptSCD->idSScene);
            }
        }
        return TRUE;
    }
    return FALSE;
}









void StartEncounterTimeOut(TimerID, DataID, void *)
{
    for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer != NULL; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {
        PlayerDeclineStart(ptPlayer);
    }
}





    void TriggerEncounter(
        SNO snoEncounter,
        Player * ptPlayer,
        SWorldID idSWorldOrigin,
        BOOL bForceImmediateStart)
    {
        if (snoEncounter == SNO_NONE || ptPlayer == NULL)
        {
            return;
        }


        BossEncounterState * pState = GetGameState();

        ScopedBossEncounterDefinitionLockR pdefEncounter(snoEncounter);
        SNO snoTargetWorld = pdefEncounter->arrWorlds[0];

        BOOL bFoundCompletedEncounter = FALSE;
        SGameGlobals & tSGameGlobals = SGameGlobalsGet();
        POS pos = tSGameGlobals.listCompletedBossEncounters.GetHeadPosition();
        while (pos != NULL)
        {
            SNO snoCompletedBossEncounter = tSGameGlobals.listCompletedBossEncounters.GetNext(pos);
            if (snoCompletedBossEncounter == snoEncounter)
            {
                bFoundCompletedEncounter = TRUE;
                break;
            }
        }


        if (!GameIsOpenWorld())
        {
            QuestRangeResult eResult;
            if (!QuestRangeEvaluate(pdefEncounter->snoQuestRange, &eResult) || bFoundCompletedEncounter)
            {

                if (bFoundCompletedEncounter ||
                    eResult == QRR_EXPIRED ||
                    (eResult == QRR_NOT_REACHED_YET && pdefEncounter->bWorkAsPortalBeforeQuestRange))
                {

                    for (Player * ptOtherPlayer = PlayerGetFirstInGame(); ptOtherPlayer; ptOtherPlayer = PlayerGetNextInGame(ptOtherPlayer))
                    {
                        if (PlayersAreLocalToEachOther(*ptPlayer, *ptOtherPlayer))
                        {

                            SGameACDChangeWorld(ptOtherPlayer->idPlayerACD,
                                pdefEncounter->arrWorlds[0],
                                pdefEncounter->uEntranceGUID, 0, 0.0f, NULL);  // inferred: eReason 0 and flWarpFadeInSecs 0.0f are constants in the original
                        }
                    }
                }

                return;
            }
        }









        else
        {
            if (pdefEncounter->bAlwaysWorkAsPortalInOpenWorldGames || bFoundCompletedEncounter)
            {


                for (Player * ptOtherPlayer = PlayerGetFirstInGame(); ptOtherPlayer; ptOtherPlayer = PlayerGetNextInGame(ptOtherPlayer))
                {
                    if (PlayersAreLocalToEachOther(*ptPlayer, *ptOtherPlayer))
                    {

                        SGameACDChangeWorld(ptOtherPlayer->idPlayerACD,
                            pdefEncounter->arrWorlds[0],
                            pdefEncounter->uEntranceGUID, 0, 0.0f, NULL);  // inferred: eReason 0 and flWarpFadeInSecs 0.0f are constants in the original
                    }
                }


                return;
            }
        }







        if (IsAnyEncounterActive() &&
            !((pState->dwFlags & (1 << FLAG_START_QUEUED)) &&
              pState->m_snoEncounter == snoEncounter))
        {
            return;
        }

        pState->ClearFlags(ptPlayer);
        ptPlayer->dwBossEncounterFlags = (1 << ESF_INSTIGATOR);


        for (Player * ptPlayerOther = PlayerGetFirstInGame(); ptPlayerOther; ptPlayerOther = PlayerGetNextInGame(ptPlayerOther))
        {
            if (ptPlayerOther != ptPlayer && PlayersAreLocalToEachOther(*ptPlayer, *ptPlayerOther))
            {
                pState->ClearFlags(ptPlayerOther);
                ptPlayerOther->dwBossEncounterFlags |= (1 << ESF_ACCEPTED);
            }
        }




        BOOL bDelayCreation = BossWorldCreate(snoTargetWorld);

        pState->m_snoEncounter = snoEncounter;
        pState->idSWorldOrigin = idSWorldOrigin;
        pState->snoLastWaypointLevelArea = PlayerGetLastWaypointLevelArea(ptPlayer);
        pState->dwFlags |= (1 << FLAG_START_QUEUED);


        int32 nEligiblePlayers = PlayerGetNumPlayers();
        SWorld * ptSWorldOrigin = SWorldTryToGet(idSWorldOrigin);
        if (ptSWorldOrigin != NULL)
        {
            PlayerList listPlayersInGame(PlayerListGetElementPool());
            PlayerListGetAllInGame(listPlayersInGame, -1, FALSE, -1);

            nEligiblePlayers = 0;  // inferred: line-0 reset of the counter in the original
            POS posPlayers = listPlayersInGame.GetHeadPosition();
            while (posPlayers != NULL)
            {
                Player * ptPlayerScoped = listPlayersInGame.GetNext(posPlayers);
                if (SWorldPlayerAllowed(idSWorldOrigin, *ptPlayerScoped))
                {
                    nEligiblePlayers++;
                }
            }


            SWorldID idSWorldBossEncounter = WorldCommonFindBySNO(snoTargetWorld, TRUE);
            SWorld * ptSWorldBossEncounter = SWorldTryToGet(idSWorldBossEncounter);
            if (ptSWorldBossEncounter != NULL)
            {
                BOOL bFromDeathPortal = ptSWorldOrigin->dwFlags & (1 << FLAG_SWORLD_FROM_DEATH_PORTAL);
                if (bFromDeathPortal) ptSWorldBossEncounter->dwFlags |= (1 << FLAG_SWORLD_FROM_DEATH_PORTAL); else ptSWorldBossEncounter->dwFlags &= ~(1 << FLAG_SWORLD_FROM_DEATH_PORTAL);
            }
        }

        if (!pdefEncounter->bShowUIInSinglePlayer &&
            (nEligiblePlayers < 2 || SPlayersAreAllLocal()))
        {
            ptPlayer->dwBossEncounterFlags |= (1 << ESF_ACCEPTED);
            StartEncounter(bDelayCreation);
            return;
        }

        pState->nStartPromptsOut++;

        BossEncounterMessage tMessage;
        tMessage.nPlayerIndex = ptPlayer->nPlayerIndex;
        tMessage.snoEncounter = snoEncounter;

        if (bForceImmediateStart)
        {
            pState->dwFlags &= ~(1 << FLAG_START_QUEUED);

            SWorld * ptSWorldOrigin = SWorldTryToGet(pState->idSWorldOrigin);

            PlayerList listPlayers(PlayerListGetElementPool());
            PlayerListGetAllInGame(listPlayers, -1, TRUE, -1);

            POS pos = listPlayers.GetHeadPosition();
            while (pos != NULL)
            {
                Player * ptPlayer = listPlayers.GetNext(pos);

                pState->ClearFlags(ptPlayer);

                if (ptSWorldOrigin && !SWorldPlayerAllowed(pState->idSWorldOrigin, *ptPlayer))
                {
                    continue;
                }

                if (NephalemRift::IsTieredRiftActive() || SetEncounterManager::IsAnyEncounterActive())
                {
                    ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
                    if (ptACDPlayer && (ACDIsParticipatingInTieredLootRun(*ptACDPlayer) || ACDIsParticipatingInSetDungeon(*ptACDPlayer)))
                    {
                        continue;
                    }
                }

                ConversationMgr::CancelConversationForPlayer(*ptPlayer);

                ptPlayer->dwBossEncounterFlags = (1 << ESF_ACCEPTED);
            }

            StartEncounter(bDelayCreation);
        }
        else
        {
            PlayerList listDestPlayers(PlayerListGetElementPool());
            PlayerListGetSingle(listDestPlayers, ptPlayer->idPlayerACD);
            MessageSendToClient(listDestPlayers, SMSG_BOSS_START_ENCOUNTER, &tMessage, sizeof(tMessage));


            if (TimerExists(pState->idTickTimer))
            {
                TimerUnregister(pState->idTickTimer);
            }

            pState->idTickTimer = TimerRegister(DATAID_INVALID, TIMER_OWNER_NONE, 90 * TICKS_PER_SECOND, NULL, StartEncounterTimeOut);
        }
    }


void PlayerAcceptStart(Player * ptPlayer)
{
    BossEncounterState * pState = GetGameState();
    if (pState->dwFlags & (1 << FLAG_START_QUEUED))
    {
        if (ptPlayer->dwBossEncounterFlags & (1 << ESF_INSTIGATOR))
        {
            ptPlayer->dwBossEncounterFlags |= (1 << ESF_ACCEPTED);
            pState->dwFlags &= ~(1 << FLAG_START_QUEUED);
            PromptAllOtherPlayers(ptPlayer);
            TryStartEncounter();
        }
    }
}










void PlayerDeclineStart(Player * ptPlayer)
{
    BossEncounterState * pState = GetGameState();
    if (pState->dwFlags & (1 << FLAG_START_QUEUED))
    {
        if (ptPlayer->dwBossEncounterFlags & (1 << ESF_INSTIGATOR))
        {
            ptPlayer->dwBossEncounterFlags &= ~(1 << ESF_INSTIGATOR);
            if (--pState->nStartPromptsOut == 0)
            {
                pState->ResetState();
            }
        }
    }
}








void PlayerVoteCancel(Player * ptPlayer)
{
    BossEncounterState * pState = GetGameState();
    if (pState->dwFlags & (1 << FLAG_PROMPT_CONTINUE_WITHOUT))
    {
        if (ptPlayer->dwBossEncounterFlags & (1 << ESF_CONTINUE_WITHOUT))
        {
            pState->ClearFlags(ptPlayer);
            TryStartEncounter();
        }
    }
}







void PlayerVoteContinue(Player * ptPlayer)
{
    BossEncounterState * pState = GetGameState();
    if (pState->dwFlags & (1 << FLAG_PROMPT_CONTINUE_WITHOUT))
    {
        if (ptPlayer->dwBossEncounterFlags & (1 << ESF_CONTINUE_WITHOUT))
        {
            pState->ClearFlags(ptPlayer);
            ptPlayer->dwBossEncounterFlags = (1 << ESF_ACCEPTED);
            TryStartEncounter();
        }
    }
}








void PlayerRemoved(Player * ptPlayer, BOOL bPlayerWasDeleted)
{
    SGameGlobals & tSGameGlobals = SGameGlobalsGet();
    if (tSGameGlobals.dwFlags & 0x2)
    {
        return;
    }

    uint32 dwOldFlags = ptPlayer->dwBossEncounterFlags;
    ptPlayer->dwBossEncounterFlags = 0;

    BossEncounterState * pState = GetGameState();

    if (dwOldFlags & ((1 << ESF_ACCEPTED) | (1 << ESF_PENDING_ACCEPT) | (1 << ESF_CONTINUE_WITHOUT)))
    {
        ptPlayer->dwBossEncounterFlags = (1 << ESF_PENDING_ACCEPT);

        BOOL bContinue = (pState->dwFlags >> FLAG_PROMPT_CONTINUE_WITHOUT) & 1;
        ptPlayer->dwBossEncounterFlags |= (bContinue << ESF_CONTINUE_WITHOUT);

        if (bContinue)
        {
            PlayerVoteCancel(ptPlayer);
        }

        PlayerDeclineInvite(ptPlayer);
    }

    if (!IsAnyEncounterActive())
    {
        pState->ClearFlags(ptPlayer);
        return;
    }

    if (!bPlayerWasDeleted)
    {
        ptPlayer->dwBossEncounterFlags = dwOldFlags;
    }

    if (!(ptPlayer->dwBossEncounterFlags & (1 << ESF_ACTIVE)))
    {
        pState->ClearFlags(ptPlayer);
    }

    if (pState->dwFlags & ((1 << FLAG_INVITE_PHASE) | (1 << FLAG_PROMPT_CONTINUE_WITHOUT)))
    {
        TryStartEncounter();
    }
}









BOOL IsAnyEncounterActive()
{
    return GetGameState()->m_snoEncounter != SNO_NONE;
}



BOOL IsInProgress()
{
    return IsAnyEncounterActive() && (GetGameState()->dwFlags & (1 << FLAG_IN_PROGRESS));
}



SNO GetActiveEncounterSNO()
{
    return GetGameState()->m_snoEncounter;
}



void PlayerAcceptInvite(Player * ptPlayer)
{
    BossEncounterState * pState = GetGameState();
    if (!(pState->dwFlags & (1 << FLAG_INVITE_PHASE)) || !(ptPlayer->dwBossEncounterFlags & (1 << ESF_PENDING_ACCEPT)))
    {
        return;
    }

    ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
    if (ptACDPlayer != NULL && ptACDPlayer->AttributesGetInt(ATTRIBUTE_CANT_START_DISPLAYED_POWERS))
    {
        return;
    }

    pState->ClearFlags(ptPlayer);

    ptPlayer->dwBossEncounterFlags = (1 << ESF_ACCEPTED);

    SendEncounterInviteStateMessage();

    TryStartEncounter();
}








void PlayerDeclineInvite(Player * ptPlayer)
{
    BossEncounterState * pState = GetGameState();
    if ((pState->dwFlags & (1 << FLAG_INVITE_PHASE)) && ptPlayer != NULL && (ptPlayer->dwBossEncounterFlags & (1 << ESF_PENDING_ACCEPT)))
    {
        pState->ClearFlags(ptPlayer);
        ptPlayer->dwBossEncounterFlags = (1 << ESF_DECLINED);

        SendEncounterInviteStateMessage();

        pState->nDeclinedInvites++;

        TryStartEncounter();
    }
}









void EncounterStateDestroyForGame()
{
    if (SGameManager::GameGet().ptBossEncounterState != NULL)
    {
        ::delete SGameManager::GameGet().ptBossEncounterState;
        SGameManager::GameGet().ptBossEncounterState = NULL;
    }
}


void EncounterStateCreateForGame()
{
    if (GameActiveGameCommonDataIsServer())
    {
        SGameManager::GameGet().ptBossEncounterState = new BossEncounterState();
    }
}






ActorIDArray * BossEncounterGetPlayers()
{
    ActorIDArray & tArrACDs = LuaScriptGetThreadSafeActorIDArray();
    tArrACDs.nNumActorIDs = 0;
    tArrACDs.bWantAddRef = TRUE;

    const BossEncounterState * pState = GetGameState();
    if (pState != NULL && (pState->dwFlags & (1 << FLAG_IN_PROGRESS)))
    {
        for (const Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer != NULL; ptPlayer = PlayerGetNextInGame(ptPlayer))
        {
            if (ptPlayer->dwBossEncounterFlags & (1 << ESF_ACTIVE))
            {
                ACDID idACD = ptPlayer->idPlayerACD;
                LuaScriptInstanceAddRefACD(LuaScriptGetExecutingScriptInstance(), idACD);
                tArrACDs.arActorIDs[tArrACDs.nNumActorIDs++] = idACD;
            }
        }
    }
    return &tArrACDs;
}







void BossEncounterFlagEncounterComplete()
{
    BossEncounterState * pState = GetGameState();
    if (pState != NULL && (pState->dwFlags & ((1 << FLAG_IN_PROGRESS) | (1 << FLAG_COMPLETING))) == (1 << FLAG_IN_PROGRESS))
    {
        TriggerComplete();
    }
}








void BossEncounterTriggerCleanup()
{
    BossEncounterState * pState = GetGameState();
    if (pState != NULL && (pState->dwFlags & (1 << FLAG_IN_PROGRESS)))
    {
        TriggerCleanup();
    }
}

}  // namespace BossEncounterManager
