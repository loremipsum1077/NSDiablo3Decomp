// X/Server/System/SetEncounter.cpp -- the per-game set-dungeon encounter: one SetEncounterState
// hangs off the current SGame, and players are invited, vote and are ported into the set
// dungeon through it. Every function sits on its original line.
//
// EncounterState::dwFlags holds the FLAG_* bits and Player::dwSetDungeonFlags the ESF_* bits
// (EncounterTypes.h).

#include "SetEncounter.h"

#include "EncounterTypes.h"
#include "../../Common/ActorCommonData.h"
#include "../../Common/Attributes/FastAttributeStructs.h"
#include "../../Common/CommonTypes.h"
#include "../../Common/Definitions/ActorDefinition.h"
#include "../../Common/Game.h"
#include "../../Common/GameConstants.h"
#include "../../Common/GameSwig.h"
#include "../../Common/GizmoSwig.h"
#include "../../Common/Networking/GameMessage.h"
#include "../../Common/Player.h"
#include "../../Common/Powers/PowersStruct.h"
#include "../../Common/TagMaps/ActorTags.h"
#include "../../Common/TagMaps/AnimTagsSwig.h"
#include "../../Common/TagMaps/Tags.h"
#include "../../Sigma/DataStructures/DataStructuresExternal.h"
#include "../Actors/ActorPowers.h"
#include "../PlayerList.h"
#include "../Powers/Powers.h"
#include "../SGame.h"
#include "../SGameManager.h"
#include "../SWorld.h"
#include "../Timers.h"

// Declared locally until their headers are adopted; the comment names the defining file.
typedef int32 AnimReason;  // X/Common/AnimPlayer.h
enum {
    ANIM_REASON_SPAWN = 4,  // X/Common/AnimPlayer.h
};
uint32 AnimPlayerPlayAnimation(ActorCommonData & tACD, uint32 dwAnimationTag, uint32 dwLength,
                               BOOL bUseAnimationTranslation, AnimReason eAnimReason,
                               float flUnitAnimStartTime, const BOOL bDontSendToClient,
                               float flSpeedScalar);  // X/Common/AnimPlayer.cpp:1224
void BrainAPIDoResurrect(ActorID idActor, BOOL bForce);  // X/Server/AI/BrainComponents.cpp:7464
void BrainAPIDoIdle(ActorID idActor);  // X/Server/AI/BrainComponents.cpp:3448
void AIQueueCompleteCurrentAction(ActorID idActor);  // X/Server/AI/ActorAI.cpp:2653
void AIQueueClearList(ActorID idActor);  // X/Server/AI/ActorAI.cpp:2603
void SPlayerSetLastWaypointLevelArea(Player * ptPlayer, const SNO snoLevelArea);  // X/Server/SPlayer.cpp:12427
void PVPResetPowersForDueling(Player & tPlayer, BOOL bEnteringDuel, BOOL bIgnoreCosmeticPets);  // X/Server/PVP.cpp:1529
void PowerAPI_ResetCooldowns(ActorCommonData & tACD);  // X/Server/Powers/PowerAPI.cpp:2119
BOOL PortalUse(ACDID idACDPortal, ActorCommonData & tACDOperator, const PlayerWarpReason eWarpReason);  // X/Server/TownPortal.cpp:296
BOOL ACDIsParticipatingInTieredLootRun(const ActorCommonData & tACD);  // X/Common/ActorCommonData.cpp:9315
BOOL GameActiveGameCommonDataIsServer();  // X/Common/Game.cpp:923
namespace NephalemRift { BOOL IsTieredRiftActive(); }  // X/Common/NephalemRiftCommon.cpp:161
namespace ConversationMgr { void CancelConversationForPlayer(const Player & tPlayer); }  // X/Server/ConversationMgr.cpp:3100

namespace SetEncounterManager {

static void sSetParticipationAttribute(Player * ptPlayer, bool bSet);


















class SetEncounterState : public EncounterState
{
public:

    SetEncounterState()
    {
        m_snoEncounter = SNO_NONE;
        dwLastCompleteTime = 0;
        idSWorldOrigin = SWORLDID_INVALID;
        idPortalGizmo = ACDID_INVALID;
        idOwner = ACDID_INVALID;
        bInviteInProgress = FALSE;
        bDungeonClosing = FALSE;
    }

    virtual void ClearFlags(Player * ptPlayer)

    {

        ptPlayer->dwSetDungeonFlags = 0;
        ptPlayer->bSetDungeonActive = FALSE;

        sSetParticipationAttribute(ptPlayer, false);

        bDungeonClosing = FALSE;
    }

    virtual void ResetState()
    {
        EncounterState::ResetState();
        m_snoEncounter = SNO_NONE;
        dwLastCompleteTime = 0;
        idSWorldOrigin = SWORLDID_INVALID;
        idPortalGizmo = ACDID_INVALID;
        dwFlags = 0;
        idOwner = ACDID_INVALID;
        bInviteInProgress = FALSE;
        bDungeonClosing = FALSE;
    }

    SNO m_snoEncounter;
    uint32 dwLastCompleteTime;
    SWorldID idSWorldOrigin;
    ACDID idPortalGizmo;
    ACDID idOwner;
    BOOL bInviteInProgress;
    BOOL bDungeonClosing;
};



inline SetEncounterState * GetGameState()
{
    SetEncounterState * pState = SGameManager::GameGet().ptSetEncounterState;
    return pState;
}









ACDID GetOwner()
{

    return GetGameState()->idOwner;
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
        if (ptPlayer->dwSetDungeonFlags == 0)
        {
            listPlayers.RemoveAt(oldPos);
            continue;
        }
        if (ptPlayer->dwSetDungeonFlags & (1 << ESF_DECLINED)) tMessage.m_DeclinedPlayers |= (1 << ptPlayer->nPlayerIndex); else tMessage.m_DeclinedPlayers &= ~(1 << ptPlayer->nPlayerIndex);
        if (ptPlayer->dwSetDungeonFlags & (1 << ESF_ACCEPTED)) tMessage.m_AcceptedPlayers |= (1 << ptPlayer->nPlayerIndex); else tMessage.m_AcceptedPlayers &= ~(1 << ptPlayer->nPlayerIndex);
        if (ptPlayer->dwSetDungeonFlags & (1 << ESF_PENDING_ACCEPT)) tMessage.m_PendingPlayers |= (1 << ptPlayer->nPlayerIndex); else tMessage.m_PendingPlayers &= ~(1 << ptPlayer->nPlayerIndex);
    }



    MessageSendToClient(listPlayers, SMSG_SET_DUNGEON_ENCOUNTER_INVITE_STATE, &tMessage, sizeof(tMessage));
}


BOOL IsInviteInProgress()
{

    return GetGameState()->bInviteInProgress;
}









void SendCancelMessage(TimerID, DataID, void *)
{

    PlayerList listPlayers(PlayerListGetElementPool());
    PlayerListGetAllInGame(listPlayers, -1, FALSE, -1);
    SimpleMessage tMessage;

    MessageSendToClient(listPlayers, SMSG_SET_DUNGEON_CANCELED, &tMessage, sizeof(tMessage));

    SetEncounterState * pState = GetGameState();
    pState->ResetState();
}




inline void CancelStart(int32 nMessageDelay)
{


    TimerRegister(DATAID_INVALID, TIMER_OWNER_NONE, nMessageDelay, NULL, SendCancelMessage);
}




























void HandleConditionalStart()
{
    SetEncounterState * pState = GetGameState();
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
        if (ptPlayer->dwSetDungeonFlags & (1 << ESF_ACCEPTED))
        {
            ptPlayer->dwSetDungeonFlags |= (1 << ESF_PENDING_ACCEPT) | (1 << ESF_CONTINUE_WITHOUT);
            continue;
        }
        BOOL bDeclined = FALSE;
        if (ptPlayer->dwSetDungeonFlags & (1 << ESF_DECLINED))
        {
            bDeclined = TRUE;
            tMessage.nData |= (1 << ptPlayer->nPlayerIndex);
        }
        pState->ClearFlags(ptPlayer);
        if (bDeclined)
            ptPlayer->dwSetDungeonFlags |= (1 << ESF_DECLINED);
        else
            ptPlayer->dwSetDungeonFlags &= ~(1 << ESF_DECLINED);
        listPlayers.RemoveAt(oldPos);
    }





    MessageSendToClient(listPlayers, SMSG_SET_DUNGEON_ONE_DECLINED, &tMessage, sizeof(tMessage));

    if (TimerExists(pState->idTickTimer))
    {
        TimerUnregister(pState->idTickTimer);
    }
}





void TryStartSetDungeon()
{

    for (const Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer != NULL; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {

        if (ptPlayer->dwSetDungeonFlags & ((1 << ESF_PENDING_ACCEPT) | (1 << ESF_CONTINUE_WITHOUT)))
        {
            return;
        }
    }

    SetEncounterState * pState = GetGameState();

    if (TimerExists(pState->idTickTimer))
    {
        TimerUnregister(pState->idTickTimer);
    }

    if (pState->nDeclinedInvites == PlayerGetNumPlayers())
    {
        CancelStart(2 * TICKS_PER_SECOND);
    }
    else if (pState->nDeclinedInvites != 0)
    {
        HandleConditionalStart();
    }
    else
    {
        StartSetDungeon();
    }
}





















void PortPlayer(Player * ptPlayer, PlayerWarpReason eReason)
{

    SetEncounterState * pState = GetGameState();


    if (PlayerIsHardcoreAndDead(*ptPlayer))
    {

        XASSERT(FALSE);
        return;
    }
    ptPlayer->dwSetDungeonFlags &= ~(1 << ESF_ACCEPTED);
    ptPlayer->dwSetDungeonFlags |= (1 << ESF_ACTIVE);

    ActorCommonData & tACDPlayer = ACDGet(ptPlayer->idPlayerACD);

    if (!tACDPlayer.IsAlive())
    {

        BrainAPIDoResurrect((ActorID)tACDPlayer.idOwner, TRUE);
    }

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

    PVPResetPowersForDueling(*ptPlayer, FALSE, TRUE);
    PowerAPI_ResetCooldowns(tACDPlayer);

    PortalUse(pState->idPortalGizmo, tACDPlayer, eReason);
}


inline void PortAllPlayers()
{

    for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer != NULL; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {
        if (ptPlayer->dwSetDungeonFlags & (1 << ESF_ACCEPTED))
        {



            PortPlayer(ptPlayer, WARP_SET_DUNGEON);
        }
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

    SetEncounterState * pState = GetGameState();
    SWorld * ptSWorldOrigin = SWorldTryToGet(pState->idSWorldOrigin);
    if (ptSWorldOrigin == NULL)
    {
        return;
    }


    POS pos = listPlayers.GetHeadPosition();
    while (pos != NULL)
    {

        POS posPlayer = pos;
        Player * ptPlayer = listPlayers.GetNext(pos);

        pState->ClearFlags(ptPlayer);



        if (!SWorldPlayerAllowed(pState->idSWorldOrigin, *ptPlayer))
        {
            listPlayers.RemoveAt(posPlayer);
            continue;
        }

        if (NephalemRift::IsTieredRiftActive())
        {
            ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
            if (ptACDPlayer != NULL && ACDIsParticipatingInTieredLootRun(*ptACDPlayer))
            {
                listPlayers.RemoveAt(posPlayer);
                continue;
            }
        }


        ConversationMgr::CancelConversationForPlayer(*ptPlayer);

        ptPlayer->dwSetDungeonFlags = (1 << ESF_PENDING_ACCEPT);
    }

    pState->dwFlags |= (1 << FLAG_INVITE_PHASE);

    SetDungeonDialogMessage tMessage;
    tMessage.nPlayerIndex = ptInstigator->nPlayerIndex;
    tMessage.nLabelDescription = ptSWorldOrigin->hWorldQuestDescriptionLabel;
    tMessage.nLabelTitle = ptSWorldOrigin->hWorldQuestTitleLabel;
    MessageSendToClient(listPlayers, SMSG_SET_DUNGEON_JOIN, &tMessage, sizeof(tMessage));

    if (TimerExists(pState->idTickTimer))
    {
        TimerUnregister(pState->idTickTimer);
    }

    pState->idTickTimer = TimerRegister(DATAID_INVALID, TIMER_OWNER_NONE, 90 * TICKS_PER_SECOND, NULL, InviteTimeOutCallback);

    SendEncounterInviteStateMessage();
}



































































































void StartSetDungeon()
{
    SetEncounterState * pState = GetGameState();

    pState->dwFlags = (1 << FLAG_IN_PROGRESS);
    BOOL bAbort = TRUE;
    Player * ptOperator = NULL;
    BOOL bHasInstigator = FALSE;

    for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer != NULL; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {
        if (PlayerIsHardcoreAndDead(*ptPlayer))
        {
            ptPlayer->dwSetDungeonFlags &= ~(1 << ESF_ACCEPTED);
        }

        if (ptPlayer->dwSetDungeonFlags & (1 << ESF_ACCEPTED))
        {
            bAbort = FALSE;
            ptOperator = ptPlayer;
        }

        if (ptPlayer->dwSetDungeonFlags & (1 << ESF_INSTIGATOR))
        {
            bHasInstigator = TRUE;
        }

        ptPlayer->bSetDungeonActive = TRUE;
    }






    ActorCommonData * ptACDOperator = ptOperator != NULL ? ACDTryToGet(ptOperator->idPlayerACD) : NULL;

    if (bAbort || !bHasInstigator || ptACDOperator == NULL)
    {
        CancelStart(2 * TICKS_PER_SECOND);
        return;
    }

    ActorCommonData * ptACDPortal = ACDTryToGet(pState->idPortalGizmo);
    if (ptACDPortal == NULL)
    {
        CancelStart(2 * TICKS_PER_SECOND);
        return;
    }

    PlayerList listPlayers(PlayerListGetElementPool());
    PlayerListGetAllInGame(listPlayers, -1, FALSE, -1);
    SimpleMessage tMessage;
    MessageSendToClient(listPlayers, SMSG_SET_DUNGEON_STARTED, &tMessage, sizeof(tMessage));

    PortAllPlayers();
}























void PlayerAcceptStart(Player * ptPlayer)
{

    if (IsInviteInProgress())
    {
        return;
    }



    SetEncounterState * pState = GetGameState();

    pState->ResetState();

    pState->bInviteInProgress = TRUE;

    ACDID idACDPortal = ptPlayer->idACDActiveChallengePortal;
    ActorCommonData * ptACDPortal = ACDTryToGet(idACDPortal);
    if (ptACDPortal != NULL && ptACDPortal->GetGizmoType() == ACTORGIZMO_PORTAL)
    {
        pState->idPortalGizmo = idACDPortal;
        ScopedActorDefinitionLockR pdefActorPortal(ptACDPortal->snoActor);
        SNO snoWorld = TagMapGetSNO(pdefActorPortal->hTagMap, TAG_ACTOR_GIZMO_PORTAL_SPECIFIED_WORLD);
        if (snoWorld != SNO_NONE)
        {
            SWorldID idSWorld = SWorldFindBySNO(snoWorld);
            if (idSWorld == SWORLDID_INVALID)
            {
                return;
            }

            pState->m_snoEncounter = snoWorld;
            pState->idSWorldOrigin = idSWorld;
        }
    }


    ptPlayer->dwSetDungeonFlags = (1 << ESF_ACCEPTED) | (1 << ESF_INSTIGATOR);

    pState->idOwner = ptPlayer->idPlayerACD;

    sSetParticipationAttribute(ptPlayer, true);

    PromptAllOtherPlayers(ptPlayer);

    TryStartSetDungeon();
}




// PlayerDeclineStart (inline) is declared here in the original but never used, so no copy of
// its body survives; it is left out.





















void PlayerVoteCancel(Player * ptPlayer)
{

    SetEncounterState * pState = GetGameState();
    if (pState->dwFlags & (1 << FLAG_PROMPT_CONTINUE_WITHOUT))
    {




        if (ptPlayer->dwSetDungeonFlags & (1 << ESF_CONTINUE_WITHOUT))
        {




            CancelStart(2 * TICKS_PER_SECOND);
        }
    }
}
void PlayerVoteContinue(Player * ptPlayer)
{

    SetEncounterState * pState = GetGameState();
    if (pState->dwFlags & (1 << FLAG_PROMPT_CONTINUE_WITHOUT))
    {




        if (ptPlayer->dwSetDungeonFlags & (1 << ESF_CONTINUE_WITHOUT))
        {
            BOOL bPlayerIsInstigator = (ptPlayer->dwSetDungeonFlags & (1 << ESF_INSTIGATOR)) != 0;

            pState->ClearFlags(ptPlayer);

            ptPlayer->dwSetDungeonFlags = (1 << ESF_ACCEPTED);

            sSetParticipationAttribute(ptPlayer, true);

            if (bPlayerIsInstigator)
            {
                ptPlayer->dwSetDungeonFlags |= (1 << ESF_INSTIGATOR);
            }

            TryStartSetDungeon();
        }
    }
}



void PlayerRemoved(Player * ptPlayer, BOOL bPlayerWasDeleted)
{

    SGameGlobals & tSGameGlobals = SGameGlobalsGet();

    // The flag's name is not in the DWARF; bit 1 of SGameGlobals::dwFlags.
    if (tSGameGlobals.dwFlags & 0x2)
    {
        return;
    }

    uint32 dwOldFlags = ptPlayer->dwSetDungeonFlags;
    ptPlayer->dwSetDungeonFlags = 0;

    SetEncounterState * pState = GetGameState();

    if (dwOldFlags & ((1 << ESF_ACCEPTED) | (1 << ESF_PENDING_ACCEPT) | (1 << ESF_CONTINUE_WITHOUT)))
    {

        ptPlayer->dwSetDungeonFlags = (1 << ESF_PENDING_ACCEPT);

        BOOL bContinue = (pState->dwFlags >> FLAG_PROMPT_CONTINUE_WITHOUT) & 1;
        ptPlayer->dwSetDungeonFlags |= (bContinue << ESF_CONTINUE_WITHOUT);

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
        ptPlayer->dwSetDungeonFlags = dwOldFlags;
    }

    if (!(ptPlayer->dwSetDungeonFlags & (1 << ESF_ACTIVE)))
    {
        pState->ClearFlags(ptPlayer);
    }

    if (pState->dwFlags & ((1 << FLAG_INVITE_PHASE) | (1 << FLAG_PROMPT_CONTINUE_WITHOUT)))
    {
        TryStartSetDungeon();
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









SWorldID GetActiveSetDungeonWorld()
{

    return GetGameState()->idSWorldOrigin;
}








static void sSetParticipationAttribute(Player * ptPlayer, bool bSet)
{



    if (ptPlayer != NULL)
    {



        for (Player * ptPlayerInGame = PlayerGetFirstInGame(); ptPlayerInGame != NULL; ptPlayerInGame = PlayerGetNextInGame(ptPlayerInGame))
        {
            if (PlayersAreLocalToEachOther(*ptPlayer, *ptPlayerInGame))
            {
                ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayerInGame->idPlayerACD);
                if (ptACDPlayer != NULL)
                {
                    ptACDPlayer->AttributesSetInt(ATTRIBUTE_PARTICIPATING_IN_SET_DUNGEON, bSet);
                }
            }
        }
    }
}
void PlayerAcceptInvite(Player * ptPlayer)
{

    SetEncounterState * pState = GetGameState();
    if (!(pState->dwFlags & (1 << FLAG_INVITE_PHASE)) || !(ptPlayer->dwSetDungeonFlags & (1 << ESF_PENDING_ACCEPT)))
    {
        return;
    }

    ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
    if (ptACDPlayer != NULL && ptACDPlayer->AttributesGetInt(ATTRIBUTE_CANT_START_DISPLAYED_POWERS))
    {
        return;
    }

    pState->ClearFlags(ptPlayer);

    sSetParticipationAttribute(ptPlayer, true);

    ptPlayer->dwSetDungeonFlags = (1 << ESF_ACCEPTED);

    SendEncounterInviteStateMessage();

    TryStartSetDungeon();
}








void PlayerDeclineInvite(Player * ptPlayer)
{

    SetEncounterState * pState = GetGameState();
    if ((pState->dwFlags & (1 << FLAG_INVITE_PHASE)) && ptPlayer != NULL && (ptPlayer->dwSetDungeonFlags & (1 << ESF_PENDING_ACCEPT)))
    {
        pState->ClearFlags(ptPlayer);
        ptPlayer->dwSetDungeonFlags = (1 << ESF_DECLINED);

        SendEncounterInviteStateMessage();

        pState->nDeclinedInvites++;

        TryStartSetDungeon();
    }
}








BOOL CanEnterSetDungeon(Player * ptPlayer)
{

    if (!IsAnyEncounterActive())
    {
        return FALSE;
    }

    if (!(ptPlayer->dwSetDungeonFlags & (1 << ESF_ACTIVE)))
    {
        return FALSE;
    }

    return !IsDungeonClosing();
}







void SetDungeonClosing(BOOL bClosing)
{

    GetGameState()->bDungeonClosing = bClosing;
}


BOOL IsDungeonClosing()
{

    return GetGameState()->bDungeonClosing;
}





void EncounterStateDestroyForGame()
{

    if (SGameManager::GameGet().ptSetEncounterState != NULL)
    {
        ::delete SGameManager::GameGet().ptSetEncounterState;
        SGameManager::GameGet().ptSetEncounterState = NULL;
    }
}

void EncounterStateCreateForGame()
{

    if (GameActiveGameCommonDataIsServer())
    {
        SGameManager::GameGet().ptSetEncounterState = new SetEncounterState();
    }
}


void Cleanup()
{

    SetEncounterState * ptState = GetGameState();
    ptState->ResetState();
    for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer != NULL; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {
        ptState->ClearFlags(ptPlayer);
    }
}



int32 GetNumberOfParticipatingPlayers()
{
    int32 nCount = 0;

    for (const Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer != NULL; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {
        ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
        if (ptACDPlayer != NULL && ptACDPlayer->AttributesGetInt(ATTRIBUTE_PARTICIPATING_IN_SET_DUNGEON))
        {
            nCount++;
        }
    }

    return nCount;
}


BOOL IsWaitingForPlayerToVote(Player * ptPlayer)
{

    SetEncounterState * pState = GetGameState();
    if (ptPlayer != NULL && pState != NULL && (pState->dwFlags & (1 << FLAG_INVITE_PHASE)) && (ptPlayer->dwSetDungeonFlags & (1 << ESF_PENDING_ACCEPT)))
    {
        return TRUE;
    }

    return FALSE;
}

}  // namespace SetEncounterManager
