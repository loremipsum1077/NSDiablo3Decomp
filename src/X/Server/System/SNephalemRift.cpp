#include "SNephalemRift.h"
#include "../ConversationMgr.h"
#include "EncounterTypes.h"
#include "../../../Contrib/Contrib/blz/include/blz/unordered_map.h"
#include "../../../Contrib/Contrib/blz/include/blz/unordered_set.h"
#include "../../Common/ActorCommonData.h"
#include "../../Common/Attributes/FastAttributeStructs.h"
#include "../../Common/CommonTypes.h"
#include "../../Common/Game.h"
#include "../../Common/Networking/GameMessage.h"
#include "../../Common/Player.h"
#include "../LuaScript/ScriptEvent.h"
#include "../PlayerList.h"
#include "../SGame.h"
#include "../SGameManager.h"
#include "../Timers.h"
#include "../Map/Minimap.h"
#include "../SWorld.h"
#include "../../Common/GameCommonData.h"
#include "../../Common/GizmoSwig.h"
#include "../../Common/Items.h"
#include "../../Common/Definitions/GlobalDefinition.h"
#include "../../Sigma/MemoryPools.h"
#include "../../Sigma/DataStructures/DataArraysTemplated.h"
#include "SChallengeRift.h"
#include "../Actors/ActorPowers.h"
#include "../Powers/PowerAPI.h"
#include "../Powers/Powers.h"
#include "../../Common/GameSwig.h"
#include "../../Common/World.h"
#include "../../Common/GlobalSNO.h"
#include "../../Common/Powers/PowerAPISwig.h"
#include "../../Common/TagMaps/AnimTagsSwig.h"
#include "../../Sigma/DataStructures/DataStructuresExternal.h"
#include "../../Common/Attributes/FastAttribute.h"
#include "../../Sigma/BlzExt/BlzUnorderedMapHelper.h"
#include "../LuaScript/ScriptUtil.h"
#include "../../Common/Shell/CmdLine.h"
#include "../../Common/Protobuf/GameMessage.pb.h"
#include "../../Common/VisualInventory.h"
#include "../../Common/Protobuf/Quest.pb.h"
#include "../../Common/Definitions/GameBalanceDefinition.h"
void PlayerListGetPrimaryPlayers(PlayerList & listPlayers);  // X/Server/PlayerList.cpp:345. Declared locally (here and below) until their headers are adopted; the comment names the defining file.
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
void NPCHandleDismissal(ActorCommonData & tACDHireling, BOOL bPlayerDismissed, BOOL bDisplayChatMessage);  // X/Server/SNPC.cpp:747
int32 MarkerGetACDsLinkedByActor(const ActorID idActor, XPooledDataIDList & listACDs, LPCSTR pszGroupName);  // X/Server/Marker/SMarker.cpp:1231
void SGizmoOperateSwitch(ActorCommonData & tACD, const BOOL bSet);  // X/Server/SGizmo.cpp:696
void SGizmoOperated(ActorCommonData & tACDGizmo, ActorCommonData * ptACDOperator);  // X/Server/SGizmo.cpp:2489
void SACDInventoryDeleteItem(ActorCommonData & tACDItem, uint64 uCount, BOOL bMarkAsDirty, BOOL bFlagForDeletion);  // X/Common/ActorInventory.cpp:6800
int32 WorldGetDungeonFinderLevel(const SNO snoWorld);  // X/Common/World.cpp:828
ACDID SWorldGetPortalDestinationACD(const SWorldID idSWorld, const uint32 uEntranceGUID);  // X/Server/SWorld.cpp:1375
uint64 SPlayerGetGearAndSkillsHash(Player & tPlayer);  // X/Server/SPlayer.cpp:14744
Player * PlayerGetByGameAccount(const OnlineService::GameAccountHandle & tGameAccount);  // X/Common/Player.cpp:1571
BOOL ACDTrySetHighestUnlockedRiftLevel(ActorCommonData & tACD, const int32 nCompletedRiftLevel);  // X/Common/ActorCommonData.cpp:9390
int32 ACDGetTieredLootRunLevel(const ActorCommonData & tACD);  // X/Common/ActorCommonData.cpp:9307; global, original bl 0x402ad0 _Z24ACDGetTieredLootRunLevelRK15ActorCommonData
namespace NephalemRiftManager {
class NephalemRiftState : public EncounterState
{
public:
    NephalemRiftState() : nRiftTier(-1), idObeliskGizmo(ACDID_INVALID), idPortalGizmo(ACDID_INVALID), idACDJewelUpgradeVendor(ACDID_INVALID), dwStartTime(0), dwArtificiallyElapsedTime(0), idTimerInviteStateUpdate(TIMERID_INVALID)
    {
    }










    BOOL IsTieredRift()
    {
        return nRiftTier >= 0;
    }


    virtual void ClearFlags(Player * ptPlayer)
    {
        ptPlayer->dwNephalemRiftFlags = 0;

        ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
        if (ptACDPlayer)
        {
            ptACDPlayer->AttributesSetInt(ATTRIBUTE_PARTICIPATING_IN_TIERED_LOOT_RUN, 0);
            ptACDPlayer->AttributesSetInt(ATTRIBUTE_TIERED_LOOT_RUN_DEATH_COUNT, 0);
            ptACDPlayer->AttributesSetInt(ATTRIBUTE_TIERED_LOOT_RUN_CORPSE_RESURRECTION_ALLOWED_GAMETIME, 0);
        }
    }

    virtual void ResetState()
    {
        EncounterState::ResetState();
        nRiftTier = -1;
        idObeliskGizmo = ACDID_INVALID;
        idPortalGizmo = ACDID_INVALID;
        idACDJewelUpgradeVendor = ACDID_INVALID;



        dwStartTime = 0;
        dwArtificiallyElapsedTime = 0;


        setPaidPlayers.clear();
        mapPlayerRewardStates.clear();
        mapPlayerStates.clear();


        PlayerList listPlayers(PlayerListGetElementPool());
        PlayerListGetAllConnected(listPlayers, -1, FALSE);
        POS posPlayer = listPlayers.GetHeadPosition();
        while (posPlayer != NULL)
        {
            Player * ptPlayerToCheck = listPlayers.GetNext(posPlayer);
            ClearFlags(ptPlayerToCheck);

            ActorCommonData * ptACD = ACDTryToGet(ptPlayerToCheck->idPlayerACD);
            if (ptACD == NULL)
            {
                continue;
            }

            ptACD->AttributesSetInt(ATTRIBUTE_JEWEL_UPGRADES_MAX, 0);
            ptACD->AttributesSetInt(ATTRIBUTE_JEWEL_UPGRADES_USED, 0);
            ptACD->AttributesSetInt(ATTRIBUTE_JEWEL_UPGRADES_BONUS, 0);
            ptACD->AttributesSetInt(ATTRIBUTE_TIERED_LOOT_RUN_REWARD_CHOICE_STATE, -1);
            ptACD->AttributesSetInt(ATTRIBUTE_TIERED_LOOT_RUN_REWARD_RECEIVES_KEY, 0);
        }


        GameSyncedData & tSycnedData = GameGetActiveGameCommonData().tGameSyncedData;
        for (int32 i = 0; i < 4; i++)
        {

            tSycnedData.arTieredRiftParticipatingHeroID[i] = 0;
        }
    }





    int32 nRiftTier;
    ACDID idObeliskGizmo;
    ACDID idPortalGizmo;
    ACDID idACDJewelUpgradeVendor;
    uint32 dwStartTime;
    uint32 dwArtificiallyElapsedTime;
    blz::unordered_set<unsigned long> setPaidPlayers;
    blz::unordered_map<unsigned long, GreaterRiftRewardState> mapPlayerRewardStates;
    blz::unordered_map<unsigned long, GreaterRiftPlayerState> mapPlayerStates;
    RiftJoinMessage tCachedJoinMessage;
    TimerID idTimerInviteStateUpdate;
};

namespace {








NephalemRiftState * GetGameState()
{

    NephalemRiftState * pState = SGameManager::GameGet().ptNephalemRiftState;
    return pState;
}
void QueueInviteStateUpdate();
void OnTick(TimerID, DataID, void *);
void TryStartRift();  // defined below at DWARF line 1090; PromptPlayers calls it
static const LPCSTR OBELISK_NORMAL_RIFT_PORTAL_GROUPNAME = "NephalemRift";
static const LPCSTR OBELISK_TIERED_RIFT_PORTAL_GROUPNAME = "TieredRift";

ACDID GetPortalGizmoForObelisk(ACDID idACDObelisk)
{


    ActorCommonData * ptACDObelisk = ACDTryToGet(idACDObelisk);
    if (ptACDObelisk == NULL)
    {
        return ACDID_INVALID;
    }



    int32 nKeyLevel = GetActiveRiftLevel();
    LPCSTR sGroupName = nKeyLevel >= 0 ? OBELISK_TIERED_RIFT_PORTAL_GROUPNAME : OBELISK_NORMAL_RIFT_PORTAL_GROUPNAME;





    ActorID idObeliskActor = (ActorID)ACDGetOwner(idACDObelisk);

    XPooledDataIDList listACDs(MemoryPoolsGetDATAIDListPool());
    MarkerGetACDsLinkedByActor(idObeliskActor, listACDs, sGroupName);

    POS pos = listACDs.GetHeadPosition();
    while (pos != NULL)
    {
        ACDID idACD = (ACDID)listACDs.GetNext(pos);
        ActorCommonData * ptPotentialPortal = ACDTryToGet(idACD);
        if (ptPotentialPortal != NULL && ptPotentialPortal->IsGizmo())
        {
            GizmoType eGizmoType = ptPotentialPortal->GetGizmoType();
            if (eGizmoType == ACTORGIZMO_PORTAL || eGizmoType == ACTORGIZMO_PORTAL_DESTINATION || eGizmoType == ACTORGIZMO_DUNGEON_PORTAL)
            {
                return idACD;
            }
        }
    }

    return ACDID_INVALID;
}

void PortPlayer(Player * ptPlayer, PlayerWarpReason eReason)
{


    NephalemRiftState * pState = GetGameState();


    if (PlayerIsHardcoreAndDead(*ptPlayer))
    {
        XASSERT(FALSE);
        return;
    }
    ptPlayer->dwNephalemRiftFlags &= ~(1 << ESF_ACCEPTED);
    ptPlayer->dwNephalemRiftFlags |= (1 << ESF_ACTIVE);

    ActorCommonData & tACDPlayer = ACDGet(ptPlayer->idPlayerACD);

    if (!tACDPlayer.IsAlive())
    {

        BrainAPIDoResurrect((ActorID)tACDPlayer.idOwner, TRUE);
    }

    if (pState->nRiftTier >= 0)
    {
        PVPResetPowersForDueling(*ptPlayer, FALSE, TRUE);
        PowerAPI_ResetCooldowns(tACDPlayer);

        SNO snoNephalemGlory = GlobalSNOGet(GLOBALSNO_POWERS_NEPHALEM_GLORY_X1);
        POS pos = tACDPlayer.listBuffs.GetHeadPosition();
        while (pos != NULL)
        {
            BuffID idBuff = tACDPlayer.listBuffs.GetNext(pos);

            PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
            if (ptBuff == NULL)
                continue;

            if (ptBuff->bDeleted)
                continue;

            if (ptBuff->snoPower != snoNephalemGlory)
                continue;

            PowerAPI_BuffFlagForDeletion(ptBuff, FALSE, TRUE, BUFF_REMOVAL_REASON_DEFAULT, TRUE);
        }

        int32 nParticipants = 0;
        for (Player * ptCheckPlayer = PlayerGetFirstInGame(); ptCheckPlayer != NULL; ptCheckPlayer = PlayerGetNextInGame(ptCheckPlayer))
        {
            if (PlayerPaidForRift(ptCheckPlayer))
            {
                nParticipants++;
            }
        }

        if (nParticipants > 1)
        {
            ACDID idACDHireling = PlayerGetHireling(ptPlayer->idPlayerACD);
            if (idACDHireling != ACDID_INVALID)
            {
                ActorCommonData & tACDHireling = ACDGet(idACDHireling);
                if (ACDIsHireling(tACDHireling))
                {
                    NPCHandleDismissal(tACDHireling, FALSE, TRUE);
                }
            }
        }
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

    PortalUse(pState->idPortalGizmo, tACDPlayer, eReason);
}


inline void PortAllPlayers()
{

    for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer != NULL; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {
        if (ptPlayer->dwNephalemRiftFlags & (1 << ESF_ACCEPTED))
        {



            PortPlayer(ptPlayer, WARP_TIERED_RIFT);
        }
    }
}


void SendRiftInviteStateMessage(Player * ptPlayerTarget)
{
    EncounterInviteStateMessage tMessage;

    tMessage.Clear();

    PlayerList listPlayersInGame(PlayerListGetElementPool());
    PlayerListGetAllInGame(listPlayersInGame, -1, FALSE, -1);

    POS pos = listPlayersInGame.GetHeadPosition();
    while (pos != NULL)
    {
        POS oldPos = pos;
        const Player * ptPlayer = listPlayersInGame.GetNext(pos);
        if (ptPlayer->dwNephalemRiftFlags == 0)
        {
            listPlayersInGame.RemoveAt(oldPos);
            continue;
        }

        if (ptPlayer->dwNephalemRiftFlags & (1 << ESF_DECLINED)) tMessage.m_DeclinedPlayers |= (1 << ptPlayer->nPlayerIndex); else tMessage.m_DeclinedPlayers &= ~(1 << ptPlayer->nPlayerIndex);
        if (ptPlayer->dwNephalemRiftFlags & (1 << ESF_ACCEPTED)) tMessage.m_AcceptedPlayers |= (1 << ptPlayer->nPlayerIndex); else tMessage.m_AcceptedPlayers &= ~(1 << ptPlayer->nPlayerIndex);
        if (ptPlayer->dwNephalemRiftFlags & (1 << ESF_PENDING_ACCEPT)) tMessage.m_PendingPlayers |= (1 << ptPlayer->nPlayerIndex); else tMessage.m_PendingPlayers &= ~(1 << ptPlayer->nPlayerIndex);
    }

    PlayerList listPlayersMessage(PlayerListGetElementPool());
    if (ptPlayerTarget)
    {
        PlayerListGetSingle(listPlayersMessage, *ptPlayerTarget);
    }
    else
    {
        listPlayersMessage.Copy(listPlayersInGame);
        QueueInviteStateUpdate();
    }

    MessageSendToClient(listPlayersMessage, SMSG_RIFT_INVITE_STATE, &tMessage, sizeof(tMessage));
}


void InviteStateUpdate(TimerID, DataID, void *)
{

    NephalemRiftState * pState = GetGameState();
    if (pState->dwFlags & (1 << FLAG_INVITE_PHASE))
    {
        SendRiftInviteStateMessage(NULL);
    }
}








void QueueInviteStateUpdate()
{


    NephalemRiftState * pState = GetGameState();
    if ((pState->dwFlags & (1 << FLAG_INVITE_PHASE)) == 0)
    {
        return;
    }


    if (TimerExists(pState->idTimerInviteStateUpdate))
    {
        TimerUnregister(pState->idTimerInviteStateUpdate);
    }


    pState->idTimerInviteStateUpdate = TimerRegister(DATAID_INVALID, TIMER_OWNER_NONE, 180, NULL, InviteStateUpdate);
}



inline void CancelStart()
{

    NephalemRiftState * pState = GetGameState();

    SendRiftInviteStateMessage(NULL);

    {
        PlayerList listPlayers(PlayerListGetElementPool());
        PlayerListGetAllInGame(listPlayers, -1, FALSE, -1);
        SimpleMessage tMessage;
        MessageSendToClient(listPlayers, SMSG_RIFT_CANCELED, &tMessage, sizeof(tMessage));
    }




    pState->ResetState();
}



void OnVoteTimeOut(TimerID, DataID, void *)
{

    NephalemRiftState * pState = GetGameState();
    for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer != NULL; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {
        if (ptPlayer->dwNephalemRiftFlags & (1 << ESF_PENDING_ACCEPT))
        {
            PlayerVoteCancel(ptPlayer);
        }
        if (pState->dwFlags & (1 << FLAG_IN_PROGRESS))
        {
            return;
        }
    }

    CancelStart();
}














void HandleConditionalStart(
    void)
{
    NephalemRiftState * pState = GetGameState();
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
        if (!(ptPlayer->dwNephalemRiftFlags & (1 << ESF_ACCEPTED)))
        {
            if (ptPlayer->dwNephalemRiftFlags & (1 << ESF_DECLINED))
            {
                tMessage.nData |= (1 << ptPlayer->nPlayerIndex);
            }





            listPlayers.RemoveAt(oldPos);
        }
        else
        {



            if (ptPlayer->dwNephalemRiftFlags & (1 << ESF_INSTIGATOR))
            {



                ptPlayer->dwNephalemRiftFlags |= (1 << ESF_PENDING_ACCEPT) | (1 << ESF_CONTINUE_WITHOUT);
            }
        }
    }


    MessageSendToClient(listPlayers, SMSG_RIFT_ONE_DECLINED, &tMessage, sizeof(tMessage));

    if (TimerExists(pState->idTickTimer))
    {
        TimerUnregister(pState->idTickTimer);
    }




    pState->idTickTimer = TimerRegister(DATAID_INVALID, TIMER_OWNER_NONE, 900, NULL, OnVoteTimeOut);

}


void InviteTimeOutCallback(TimerID, DataID, void *)
{


    for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer != NULL; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {

        if (ptPlayer->dwNephalemRiftFlags & (1 << ESF_PENDING_ACCEPT))
        {
            PlayerDeclineInvite(ptPlayer);
        }
    }
}

BOOL PlayerIsEligible(
    int32 nRiftTier,
    Player * ptPlayer)
{
    if (nRiftTier < 0)
    {
        return TRUE;
    }

    ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
    if (ptACDPlayer == NULL)
    {
        return FALSE;
    }

    BOOL bTooLowForTieredRift = ptACDPlayer != NULL && ptACDPlayer->AttributesGetInt(ATTRIBUTE_LEVEL) != HERO_LEVEL_MAX;
    BOOL bInBossFight = ptACDPlayer != NULL && ptACDPlayer->AttributesGetInt(ATTRIBUTE_IN_BOSS_ENCOUNTER) != 0;
    BOOL bInSetEncounter = ptACDPlayer != NULL && ptACDPlayer->AttributesGetInt(ATTRIBUTE_PARTICIPATING_IN_SET_DUNGEON) != 0;

    return NephalemRift::HasKeyForGreaterRift(ptPlayer) && !bTooLowForTieredRift && !bInBossFight && !bInSetEncounter;
}


void FilterEligiblePlayerList(
    int32 nRiftTier,
    const PlayerList & listAllPlayers,
    PlayerList & listEligiblePlayers,
    PlayerList & listIneligiblePlayers)
{
    POS pos = listAllPlayers.GetHeadPosition();
    while (pos != NULL)
    {
        Player * ptPlayer = listAllPlayers.GetNext(pos);

        ConversationMgr::CancelConversationForPlayer(*ptPlayer);

        ptPlayer->dwNephalemRiftFlags = (1 << ESF_PENDING_ACCEPT);

        if (PlayerIsEligible(nRiftTier, ptPlayer))
        {

            listEligiblePlayers.AddTail_Tracked(ptPlayer);
        }
        else
        {

            listIneligiblePlayers.AddTail_Tracked(ptPlayer);
        }
    }
}


void ClearPlayerFlags(
    const PlayerList & listPlayers,
    NephalemRiftState * pState)
{
    POS pos = listPlayers.GetHeadPosition();
    while (pos != NULL)
    {
        Player * ptPlayer = listPlayers.GetNext(pos);
        pState->ClearFlags(ptPlayer);
    }
}


uint32 GetPlayerKeysMap(
    int32 nRiftTier,
    const PlayerList & listEligiblePlayers)
{

    uint8 nPlayerKeys[4];
    SigmaMemoryFill(nPlayerKeys, 0xff, sizeof(nPlayerKeys));

    POS pos = listEligiblePlayers.GetHeadPosition();
    while (pos != NULL)
    {
        Player * ptPlayer = listEligiblePlayers.GetNext(pos);

        int32 nCount = 0;
        ACDID idKey = NephalemRift::GetKeyForGreaterRift(ptPlayer, nCount);
        if (idKey != ACDID_INVALID)
        {
            nPlayerKeys[ptPlayer->nPlayerIndex] = (uint8)ACDGet(ptPlayer->idPlayerACD).AttributesGetInt(ATTRIBUTE_HIGHEST_SOLO_RIFT_LEVEL);
        }
    }

    uint32 nKeysMap;
    // Inferred: the original reads the four key bytes with one 32-bit load (line 644, ldur w8).

    nKeysMap = *(uint32 *)nPlayerKeys;
    return nKeysMap;
}




void PromptPlayers(
    const Player * ptInstigator)
{
    NephalemRiftState * pState = GetGameState();


    SigmaMemoryClear(&pState->tCachedJoinMessage, sizeof(pState->tCachedJoinMessage));


    if (!pState->IsTieredRift())
    {
        return;
    }

    PlayerList listPlayers(PlayerListGetElementPool());


    PlayerListGetAllInGame(listPlayers, ptInstigator->nPlayerIndex, TRUE, -1);
    if (listPlayers.Count() == 0)
    {
        return;
    }

    pState->dwFlags |= (1 << FLAG_INVITE_PHASE);


    ClearPlayerFlags(listPlayers, pState);

    PlayerList listEligiblePlayers(PlayerListGetElementPool());
    PlayerList listIneligiblePlayers(PlayerListGetElementPool());
    FilterEligiblePlayerList(pState->nRiftTier, listPlayers, listEligiblePlayers, listIneligiblePlayers);




    Player * ptInitiator = PlayerGetByPlayerIndex(ptInstigator->nPlayerIndex);
    listEligiblePlayers.AddTail_Tracked(ptInitiator);


    POS pos = listIneligiblePlayers.GetHeadPosition();
    while (pos != NULL)
    {
        Player * ptPlayer = listIneligiblePlayers.GetNext(pos);


        ptPlayer->dwNephalemRiftFlags = (1 << ESF_DECLINED);
        pState->nDeclinedInvites++;


        ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
        if (ptACDPlayer != NULL && ptACDPlayer->AttributesGetInt(ATTRIBUTE_LEVEL) != HERO_LEVEL_MAX)
        {
            PlayerDisplayErrorMessage(ptPlayer->idPlayerACD, GAMEERROR_LOOTRUN_TIERED_NOT_MAX_LEVEL);
        }
        else if (!NephalemRift::HasKeyForGreaterRift(ptPlayer))
        {
            PlayerDisplayErrorMessage(ptPlayer->idPlayerACD, GAMEERROR_LOOTRUN_TIERED_NEED_KEY);
        }
    }


    if (listIneligiblePlayers.Count() > 0)
    {
        pos = listEligiblePlayers.GetHeadPosition();
        while (pos != NULL)
        {
            Player * ptPlayer = listEligiblePlayers.GetNext(pos);

            POS posIneligible = listIneligiblePlayers.GetHeadPosition();
            while (posIneligible != NULL)
            {
                Player * ptPlayerIneligible = listIneligiblePlayers.GetNext(posIneligible);
                if (PlayersAreLocalToEachOther(*ptPlayer, *ptPlayerIneligible))
                {
                    ptPlayer->dwNephalemRiftFlags = (1 << ESF_DECLINED);
                    pState->nDeclinedInvites++;
                    break;
                }
            }
        }
    }


    PlayerList listTargetPlayers(PlayerListGetElementPool());
    PlayerListGetPrimaryPlayers(listTargetPlayers);




    if (listTargetPlayers.Count() > 0)
    {
        RiftJoinMessage tMessage;
        tMessage.nPlayerIndex = ptInstigator->nPlayerIndex;
        tMessage.nRiftTier = pState->nRiftTier;
        tMessage.uRiftStartServerTime = GameGetServerTime() + 3600;  // 3600 ticks (TICKS_PER_MINUTE, X/Common/GameConstants.h, not included here)

        tMessage.nPlayerKeyTiers = GetPlayerKeysMap(pState->nRiftTier, listEligiblePlayers);

        MessageSendToClient(listTargetPlayers, SMSG_RIFT_JOIN_ENCOUNTER, &tMessage, sizeof(tMessage));

        if (TimerExists(pState->idTickTimer))
        {
            TimerUnregister(pState->idTickTimer);
        }




        pState->tCachedJoinMessage = tMessage;


        pState->idTickTimer = TimerRegister(DATAID_INVALID, TIMER_OWNER_NONE, 3600, NULL, InviteTimeOutCallback);
        SendRiftInviteStateMessage(NULL);
    }
    else
    {
        SendRiftInviteStateMessage(NULL);
        TryStartRift();
    }
}


void OnTick(TimerID, DataID, void *)
{
    const int32 SAVE_POINT_PORTAL_PING_TICKS = 30 * 60;


    NephalemRiftState * pState = GetGameState();

    for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer != NULL; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {
        ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
        if (ptACDPlayer == NULL || ptACDPlayer->AttributesGetInt(ATTRIBUTE_PARTICIPATING_IN_TIERED_LOOT_RUN) == 0)
            continue;

        const GameSyncedData & tGSD = GameGetActiveGameCommonData().tGameSyncedData;
        if (tGSD.bLootRunBossActive || tGSD.bLootRunBossDead)
        {
            SWorldID idSWorld = ptACDPlayer->GetSWorldID();
            SNO snoWorld = WorldCommonGetWorldSNO(idSWorld);
            if (snoWorld != SNO_NONE)
            {
                int32 nPlayerFloor = WorldGetDungeonFinderLevel(snoWorld);
                SGameGlobals & tSGameGlobals = SGameGlobalsGet();
                if (nPlayerFloor != -1 && tSGameGlobals.tDungeonFinderData.nBossSpawnFloor != -1 && nPlayerFloor != tSGameGlobals.tDungeonFinderData.nBossSpawnFloor)
                {
                    ACDID idPortal = SWorldGetPortalDestinationACD(idSWorld, StartingLocationNameToGUID(nPlayerFloor > tSGameGlobals.tDungeonFinderData.nBossSpawnFloor ? "Entrance" : "Exit"));
                    ActorCommonData * ptACDPortal = ACDTryToGet(idPortal);
                    if (ptACDPortal != NULL)
                    {
                        PingID idPing = ptACDPortal->AttributesGetInt(ATTRIBUTE_PING);
                        if (!Minimap::IsValid(idPing))
                        {
                            PingID idPing = Minimap::PingLocation(ptACDPortal->GetPlace(), SAVE_POINT_PORTAL_PING_TICKS, ptPlayer->uHeroId, SNO_NONE, TRUE, (uint32)-1, SNO_NONE, SNO_NONE, 0.0f, SNO_NONE);
                            ptACDPortal->AttributesSetInt(ATTRIBUTE_PING, idPing);
                        }
                    }
                }
            }
        }
    }

    if (pState->dwFlags & (1 << FLAG_IN_PROGRESS))
    {
        BOOL bAnyoneAlive = FALSE;
        for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer != NULL; ptPlayer = PlayerGetNextInGame(ptPlayer))
        {
            const ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
            if (ptACDPlayer != NULL && ptACDPlayer->IsAlive())
            {
                bAnyoneAlive = TRUE;
                break;
            }
        }

        if (!bAnyoneAlive)
        {
            for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer != NULL; ptPlayer = PlayerGetNextInGame(ptPlayer))
            {
                ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
                if (ptACDPlayer != NULL)
                {
                    ptACDPlayer->AttributesSetInt(ATTRIBUTE_CHECKPOINT_RESURRECTION_ALLOWED_GAMETIME, 0);
                }
            }
        }
    }

    pState->idTickTimer = TimerRegister(DATAID_INVALID, TIMER_OWNER_NONE, 60, NULL, OnTick);
}




































void StartRift(
    BOOL bBlizzcon)
{
    NephalemRiftState * pState = GetGameState();
    pState->dwFlags = 0;
    pState->dwFlags |= (1 << FLAG_IN_PROGRESS);

    pState->dwStartTime = GameGetServerTime();
    pState->dwArtificiallyElapsedTime = 0;

    ActorCommonData * ptACDObelisk = ACDTryToGet(pState->idObeliskGizmo);
    if (ptACDObelisk == NULL)
    {
        CancelStart();
        return;
    }

    uint64 nEmpowerCost = NephalemRift::GetEmpoweredRiftCost(pState->nRiftTier);

    BOOL bAbort = TRUE;
    Player * ptOperator = NULL;
    BOOL bHasInstigator = FALSE;

    for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer != NULL; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {
        if (PlayerIsHardcoreAndDead(*ptPlayer))
        {

            ptPlayer->dwNephalemRiftFlags &= ~(1 << ESF_ACCEPTED);
        }

        if (!bBlizzcon && pState->nRiftTier >= 0 && !NephalemRift::HasKeyForGreaterRift(ptPlayer))
        {

            ptPlayer->dwNephalemRiftFlags &= ~(1 << ESF_ACCEPTED);
        }

        if (ptPlayer->dwNephalemRiftFlags & (1 << ESF_ACCEPTED))
        {
            bAbort = FALSE;
            ptOperator = ptPlayer;
        }

        if (ptPlayer->dwNephalemRiftFlags & (1 << ESF_INSTIGATOR))
        {
            bHasInstigator = TRUE;
        }

        if (ptPlayer->dwNephalemRiftFlags & (1 << ESF_EMPOWERED))
        {
            ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);

            if (ptACDPlayer->GetCurrencyAmount(CURRENCY_GOLD) < nEmpowerCost)
            {
                ptPlayer->dwNephalemRiftFlags &= ~(1 << ESF_EMPOWERED);
                PlayerDisplayErrorMessage(ptPlayer->idPlayerACD, GAMEERROR_EMPOWERED_RIFT_CANNOT_AFFORD);
            }
        }
    }


    ActorCommonData * ptACDOperator = ptOperator ? ACDTryToGet(ptOperator->idPlayerACD) : NULL;

    if (!bHasInstigator || bAbort || ptACDOperator == NULL)
    {
        CancelStart();
        return;
    }



    PlayerList listPlayers(PlayerListGetElementPool());
    PlayerListGetAllInGame(listPlayers, -1, FALSE, -1);
    SimpleMessage tMessage;
    MessageSendToClient(listPlayers, SMSG_RIFT_STARTED, &tMessage, sizeof(tMessage));

    int32 nEntryCost = NephalemRift::GetKeyEntryCost(pState->nRiftTier);

    for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer != NULL; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {
        if (bBlizzcon)
        {
            PlayerSetPaidForRift(ptPlayer, TRUE, TRUE, FALSE);
        }
        else if (ptPlayer->dwNephalemRiftFlags & (1 << ESF_ACCEPTED))
        {


            int32 nCount = 0;
            if (nEntryCost > 0)
            {
                ACDID idACDKey = NephalemRift::GetKeyForGreaterRift(ptPlayer, nCount);
                ActorCommonData * ptACDItem = ACDTryToGet(idACDKey);

                SACDInventoryDeleteItem(*ptACDItem, nEntryCost, TRUE, FALSE);
            }

            if (ptPlayer->dwNephalemRiftFlags & (1 << ESF_EMPOWERED))
            {
                ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
                ptACDPlayer->ModifyCurrencyAmount(-(int64)nEmpowerCost, CURRENCY_GOLD, 18, FALSE);  // GMR_EMPOWERED_RIFT
            }


            PlayerSetPaidForRift(ptPlayer, TRUE, TRUE, (ptPlayer->dwNephalemRiftFlags >> ESF_EMPOWERED) & 1);
        }
    }

    SGizmoOperateSwitch(*ptACDObelisk, TRUE);
    SGizmoOperated(*ptACDObelisk, ptACDOperator);
    pState->idPortalGizmo = GetPortalGizmoForObelisk(pState->idObeliskGizmo);

    if (GameIsWeeklyChallenge())
    {
        ChallengeRift::OpenChallengeRift();
    }


    if (pState->IsTieredRift())
    {
        PortAllPlayers();

        GameSyncedData & tSycnedData = GameGetActiveGameCommonData().tGameSyncedData;
        int32 nIndex = 0;
        NephalemRiftState * ptRiftState = GetGameState();
        for (blz::unordered_set<unsigned long>::const_iterator iter = ptRiftState->setPaidPlayers.begin(), end = ptRiftState->setPaidPlayers.end(); iter != end; ++iter)
        {
            if (nIndex < 4)
            {
                tSycnedData.arTieredRiftParticipatingHeroID[nIndex++] = *iter;
            }
        }

        SGameSyncData(NULL);

        pState->idTickTimer = TimerRegister(DATAID_INVALID, TIMER_OWNER_NONE, 60, NULL, OnTick);

    }
}



void SendStartRift(TimerID, DataID, void *)
{
    StartRift(FALSE);
}






BOOL NeedsConditionalStart()
{
    BOOL bNeedsConditionalStart = FALSE;






    for (const Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer != NULL; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {
        if ((ptPlayer->dwNephalemRiftFlags & (1 << ESF_INSTIGATOR)) == 0)
        {
            continue;
        }











        if (ptPlayer->dwNephalemRiftFlags & (1 << ESF_DECLINED))
        {
            return FALSE;
        }






        for (const Player * ptOtherPlayer = PlayerGetFirstInGame(); ptOtherPlayer != NULL; ptOtherPlayer = PlayerGetNextInGame(ptOtherPlayer))
        {
            if (ptOtherPlayer == ptPlayer)
            {
                continue;
            }


            if (ptOtherPlayer->dwNephalemRiftFlags & (1 << ESF_PENDING_ACCEPT))
            {
                return FALSE;
            }



            if (!PlayersAreLocalToEachOther(*ptPlayer, *ptOtherPlayer) && (ptOtherPlayer->dwNephalemRiftFlags & (1 << ESF_DECLINED)))
            {
                bNeedsConditionalStart = TRUE;
            }
        }
        break;
    }
    return bNeedsConditionalStart;
}

void TryStartRift(
    void)
{
    for (const Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer != NULL; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {
        if (ptPlayer->dwNephalemRiftFlags & ((1 << ESF_PENDING_ACCEPT) | (1 << ESF_CONTINUE_WITHOUT)))
        {
            return;
        }
    }


    NephalemRiftState * pState = GetGameState();


    if (TimerExists(pState->idTickTimer))
    {
        TimerUnregister(pState->idTickTimer);
    }



    if (pState->nDeclinedInvites == PlayerGetNumPlayers())
    {
        CancelStart();
        return;
    }


    if (pState->nDeclinedInvites != 0)
    {
        if (NeedsConditionalStart())
        {

            HandleConditionalStart();
            return;
        }
    }


    if (PlayerGetNumPlayers() >= 2)
    {

        TimerRegister(DATAID_INVALID, TIMER_OWNER_NONE, NephalemRift::GetStartRiftDelaySeconds() * 60, NULL, SendStartRift);
        pState->dwFlags |= (1 << FLAG_START_COUNTDOWN_ACTIVE);
    }
    else
    {
        StartRift(FALSE);
    }



}


}  // namespace

void InitiateRift(ACDID idObeliskGizmo, Player * ptPlayer)
{
    if (ptPlayer == NULL)
    {
        return;
    }



    NephalemRiftState * ptState = GetGameState();
    if (ptState->dwFlags & ~(1 << FLAG_START_QUEUED))
    {
        return;
    }

    ptState->idObeliskGizmo = idObeliskGizmo;
    ptState->snoLastWaypointLevelArea = PlayerGetLastWaypointLevelArea(ptPlayer);
    ptState->dwFlags |= (1 << FLAG_START_QUEUED);
    ptState->nStartPromptsOut++;
    ptState->ClearFlags(ptPlayer);
    ptPlayer->dwNephalemRiftFlags = (1 << ESF_INSTIGATOR);

    ANNDataMessage tMessage;
    tMessage.ann = ANNFromACD(idObeliskGizmo);
    PlayerList listPlayers(PlayerListGetElementPool());
    PlayerListGetSingle(listPlayers, *ptPlayer);
    MessageSendToClient(listPlayers, SMSG_RIFT_START_ENCOUNTER, &tMessage, sizeof(tMessage));
}






void TriggerRift(
    int32 nRiftTier,
    Player * ptPlayer)
{
    if (nRiftTier >= 0)
    {

        BOOL bAllEligible = TRUE;


        for (Player * ptOtherPlayer = PlayerGetFirstInGame(); ptOtherPlayer != NULL; ptOtherPlayer = PlayerGetNextInGame(ptOtherPlayer))
        {
            if (PlayersAreLocalToEachOther(*ptPlayer, *ptOtherPlayer) &&
                !PlayerIsEligible(nRiftTier, ptOtherPlayer))
            {
                ActorCommonData * ptACDOtherPlayer = ACDTryToGet(ptOtherPlayer->idPlayerACD);
                if (ptACDOtherPlayer != NULL)
                {
                    if (ACDIsParticipatingInSetDungeon(*ptACDOtherPlayer))
                    {
                        PlayerDisplayErrorMessage(ptPlayer->idPlayerACD, GAMEERROR_LOOTRUN_TIERED_CANNOT_BE_IN_SET_DUNGEON);
                    }
                    else
                    {
                        BOOL bTooLowForTieredRift = ptACDOtherPlayer->AttributesGetInt(ATTRIBUTE_LEVEL) != HERO_LEVEL_MAX;
                        PlayerDisplayErrorMessage(ptOtherPlayer->idPlayerACD, bTooLowForTieredRift ? GAMEERROR_LOOTRUN_TIERED_NOT_MAX_LEVEL : GAMEERROR_LOOTRUN_TIERED_NEED_KEY);
                    }
                }
                else
                {
                    PlayerDisplayErrorMessage(ptPlayer->idPlayerACD, GAMEERROR_LOOTRUN_TIERED_NEED_KEY);
                }
                bAllEligible = FALSE;
            }
        }

        if (!bAllEligible)
        {
            return;
        }
    }











    NephalemRiftState * pState = GetGameState();
    pState->dwFlags &= ~(1 << FLAG_START_QUEUED);
    pState->nRiftTier = nRiftTier;

    PromptPlayers(ptPlayer);

    TryStartRift();
}


BOOL EnterRift(ACDID idPortalGizmo, Player * ptPlayer)
{



    ActorCommonData * ptACDPortal = ACDTryToGet(idPortalGizmo);
    if (ptACDPortal == NULL)
    {
        return FALSE;
    }

    int32 nRiftTier = GetActiveRiftLevel();

    BOOL bHasRiftKey = PlayerPaidForRift(ptPlayer);
    if (bHasRiftKey)
    {
        if (nRiftTier < 0)
        {
            return TRUE;
        }

        BOOL bNonPayerEntering = FALSE;
        for (Player * ptCheckPlayer = PlayerGetFirstInGame(); ptCheckPlayer != NULL; ptCheckPlayer = PlayerGetNextInGame(ptCheckPlayer))
        {
            if (!PlayersAreLocalToEachOther(*ptPlayer, *ptCheckPlayer))
            {
                continue;
            }




            if (!PlayerPaidForRift(ptCheckPlayer) && IsTieredRiftActive() && !GameGetActiveGameCommonData().tGameSyncedData.bLootRunBossDead)
            {
                PlayerDisplayErrorMessage(ptCheckPlayer->idPlayerACD, GAMEERROR_LOOTRUN_TIERED_ALREADY_STARTED);
                bNonPayerEntering = TRUE;
            }
        }

        return !bNonPayerEntering;
    }







    if (IsTieredRiftActive() && GameGetActiveGameCommonData().tGameSyncedData.bLootRunBossDead)
    {
        return TRUE;
    }





    NephalemRiftState * pState = GetGameState();
    if (pState->dwFlags & (1 << FLAG_IN_PROGRESS))
    {
        if (nRiftTier < 0)
        {
            if (!PlayerIsHardcoreAndDead(*ptPlayer))
            {
                pState->idPortalGizmo = idPortalGizmo;
                ptPlayer->dwNephalemRiftFlags |= (1 << ESF_ACCEPTED);
                pState->ClearFlags(ptPlayer);
                pState->dwStartTime = GameGetServerTime();
                PlayerSetPaidForRift(ptPlayer, TRUE, TRUE, (ptPlayer->dwNephalemRiftFlags >> ESF_EMPOWERED) & 1);
                return TRUE;
            }
        }
        else
        {
            if (GameIsWeeklyChallenge())
                PlayerDisplayErrorMessage(ptPlayer->idPlayerACD, GAMEERROR_CHALLENGE_RIFT_ALREADY_STARTED);
            else
                PlayerDisplayErrorMessage(ptPlayer->idPlayerACD, GAMEERROR_LOOTRUN_TIERED_ALREADY_STARTED);
        }
    }

    return FALSE;
}


















































BOOL PortToPlayerInStandardRiftPossible(Player * ptTargetPlayer, Player * ptPlayer)
{



    if (PlayerPaidForRift(ptPlayer))
    {
        return TRUE;
    }

    ActorCommonData * ptACDDest = ACDTryToGet(ptTargetPlayer->idPlayerACD);
    if (ptACDDest == NULL)
    {
        return FALSE;
    }

    return !ACDIsInTieredLootRun(*ptACDDest);
}







void StartWeeklyChallengeRift(ACDID idObeliskGizmo, int32 nRiftTier, Player * ptPlayer)
{
    if (nRiftTier < 0 || ptPlayer == NULL)
    {
        return;
    }

    NephalemRiftState * ptState = GetGameState();
    if (ptState->dwFlags & ~(1 << FLAG_START_QUEUED))
    {
        return;
    }

    ptState->idObeliskGizmo = idObeliskGizmo;
    ptState->snoLastWaypointLevelArea = PlayerGetLastWaypointLevelArea(ptPlayer);

    for (Player * ptPlayerIter = PlayerGetFirstInGame(); ptPlayerIter != NULL; ptPlayerIter = PlayerGetNextInGame(ptPlayerIter))
    {
        ptState->ClearFlags(ptPlayerIter);
        ptPlayerIter->dwNephalemRiftFlags |= (1 << ESF_ACCEPTED);
    }
    ptPlayer->dwNephalemRiftFlags |= (1 << ESF_INSTIGATOR);

    SGameGlobals & tSGameGlobals = SGameGlobalsGet();
    DungeonFinderOverrides & tDungeonFinderOverrides = tSGameGlobals.tChallengeRift.tDungeonFinderOverrides;
    ptState->nRiftTier = tDungeonFinderOverrides.nWeeklyChallengeRiftLevel >= 0 ? tDungeonFinderOverrides.nWeeklyChallengeRiftLevel : nRiftTier;

    StartRift(TRUE);
}
























void TryStartBlizzonRift(ACDID idObeliskGizmo, int32 nRiftTier, ACDID idACDPlayer)
{




    if (!CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
    {
        return;
    }

    Player * ptPlayer = PlayerGetByACD(idACDPlayer);
    if (ptPlayer == NULL)
    {
        return;
    }

    ptPlayer->dwNephalemRiftFlags = (1 << ESF_ACCEPTED);
    if (nRiftTier < 0)
    {
        return;
    }

    BOOL bHasInstigator = FALSE;
    int32 nPlayersAccepted = 0;
    PlayerList listPlayers(PlayerListGetElementPool());
    PlayerListGetAllConnected(listPlayers, -1, FALSE);


    POS posPlayer = listPlayers.GetHeadPosition();
    while (posPlayer != NULL)
    {
        Player * ptPlayerToCheck = listPlayers.GetNext(posPlayer);
        if (ptPlayerToCheck != NULL)
        {
            if (ptPlayerToCheck->dwNephalemRiftFlags & (1 << ESF_INSTIGATOR))
            {
                bHasInstigator = TRUE;
            }
            if (ptPlayerToCheck->dwNephalemRiftFlags & (1 << ESF_ACCEPTED))
            {
                nPlayersAccepted++;
            }
        }
    }



    NephalemRiftState * ptState = GetGameState();
    if (!bHasInstigator)
    {
        ptState->idObeliskGizmo = idObeliskGizmo;
        ptState->snoLastWaypointLevelArea = PlayerGetLastWaypointLevelArea(ptPlayer);
        ptPlayer->dwNephalemRiftFlags |= (1 << ESF_INSTIGATOR);
        ptState->nRiftTier = nRiftTier;
    }


    if (listPlayers.Count() == nPlayersAccepted)
    {
        StartRift(TRUE);
        GameFlagSet("AutoRiftBlizzcon", FALSE);
    }
}


BOOL PlayerPaidForRift(const Player * ptPlayer)
{
    if (ptPlayer == NULL)
    {
        return FALSE;
    }


    NephalemRiftState * ptState = GetGameState();
    if (ptState == NULL)
        return FALSE;

    if (ptState->nRiftTier == -1)
    {
        return TRUE;
    }


    return ptState->setPaidPlayers.find(ptPlayer->uHeroId) != ptState->setPaidPlayers.end();
}


GreaterRiftPlayerState * GetGreaterRiftPlayerState(const Player * ptPlayer)
{
    if (ptPlayer == NULL)
    {
        return NULL;
    }


    NephalemRiftState * ptState = GetGameState();
    if (ptState == NULL)
    {
        return NULL;
    }

    return blzext::unordered_map_find_ptr(ptState->mapPlayerStates, ptPlayer->uHeroId);
}

BOOL IsTieredRiftOrphaned(
    Player * ptPlayerToIgnore)
{
    NephalemRiftState * ptState = GetGameState();
    if (ptState == NULL)
    {
        return FALSE;
    }

    if (NephalemRift::GetActiveTieredRiftLevel() < 0)
    {
        return FALSE;
    }

    int32 nPlayersParticipatingInGame = 0;
    for (Player * ptPlayerIter = PlayerGetFirstInGame(); ptPlayerIter != NULL; ptPlayerIter = PlayerGetNextInGame(ptPlayerIter))
    {
        if (ptPlayerToIgnore != NULL &&
            ptPlayerToIgnore->idPlayerACD == ptPlayerIter->idPlayerACD)
        {
            continue;
        }

        ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayerIter->idPlayerACD);
        if (ptACDPlayer != NULL &&
            NephalemRift::IsTieredRiftActive() &&
            FastAttribGetValueInt(ptACDPlayer->idFastAttrib, ATTRIBUTE_PARTICIPATING_IN_TIERED_LOOT_RUN))
        {
            nPlayersParticipatingInGame++;
        }
    }

    if (nPlayersParticipatingInGame == 0 &&
        !ptState->setPaidPlayers.empty())
    {
        return TRUE;
    }



    return FALSE;
}


void SendTieredRiftOrphanedEvent(ACDID idACDPlayer)
{
    ScriptEventOnUserEvent(LOOT_RUN_ORPHANED_EVENT, idACDPlayer, 0);
}
































int32 RiftPlayerCount()
{

    NephalemRiftState * ptState = GetGameState();
    if (ptState)
        return (int32)ptState->setPaidPlayers.size();
    return -1;
}




BOOL PlayerGetEmpoweredRift(const Player * ptPlayer)
{
    if (ptPlayer == NULL)
    {
        return FALSE;
    }


    NephalemRiftState * ptState = GetGameState();
    if (ptState == NULL)
    {
        return FALSE;
    }







    GreaterRiftPlayerState * ptPlayerState = blzext::unordered_map_find_ptr(ptState->mapPlayerStates, ptPlayer->uHeroId); return ptPlayerState != NULL && ptPlayerState->bEmpoweredRift;
}

































void PlayerSetLeaverHash(Player * ptPlayer)
{
    if (ptPlayer == NULL)
    {
        return;
    }



    NephalemRiftState * ptState = GetGameState();
    if (ptState == NULL)
    {
        return;
    }

    GreaterRiftPlayerState * ptPlayerState = blzext::unordered_map_find_ptr(ptState->mapPlayerStates, ptPlayer->uHeroId);
    if (ptPlayerState != NULL && ptPlayerState->uLeaverGearAndSkillHash == 0)
    {
        uint64 uHashCurrentPlayerGearAndSkills = SPlayerGetGearAndSkillsHash(*ptPlayer);
        ptPlayerState->uLeaverGearAndSkillHash = uHashCurrentPlayerGearAndSkills;
    }
}










void PlayerSetPaidForRift(Player * ptPlayer, BOOL bPaid, BOOL bCopyToLocalPlayers, BOOL bEmpowered)
{
    if (ptPlayer == NULL)
    {
        return;
    }





    NephalemRiftState * ptState = GetGameState();
    if (ptState == NULL)
    {
        return;
    }

    if (bPaid && !PlayerPaidForRift(ptPlayer))
    {

        ptState->setPaidPlayers.insert(ptPlayer->uHeroId);

        GreaterRiftPlayerState tRiftPlayerState;
        tRiftPlayerState.nDeathCount = 0;
        tRiftPlayerState.uLeaverGearAndSkillHash = 0;
        tRiftPlayerState.bHasNewSoloRecord = FALSE;
        tRiftPlayerState.bEmpoweredRift = bEmpowered;
        tRiftPlayerState.tGameAccount = ptPlayer->tAccount.tHandle;

        ptState->mapPlayerStates[ptPlayer->uHeroId] = tRiftPlayerState;

    }
    else if (!bPaid && PlayerPaidForRift(ptPlayer))
    {
        ptState->setPaidPlayers.erase(ptPlayer->uHeroId);
    }

    if (IsTieredRiftActive())
    {
        ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
        if (ptACDPlayer != NULL)
        {
            ptACDPlayer->AttributesSetInt(ATTRIBUTE_PARTICIPATING_IN_TIERED_LOOT_RUN, bPaid);
            ptACDPlayer->AttributesSetInt(ATTRIBUTE_TIERED_LOOT_RUN_DEATH_COUNT, 0);
            ptACDPlayer->AttributesSetInt(ATTRIBUTE_TIERED_LOOT_RUN_CORPSE_RESURRECTION_ALLOWED_GAMETIME, 0);
        }
    }
    else if (bCopyToLocalPlayers)
    {
        for (Player * ptOtherPlayer = PlayerGetFirstInGame(); ptOtherPlayer != NULL; ptOtherPlayer = PlayerGetNextInGame(ptOtherPlayer))
        {
            if (ptOtherPlayer != ptPlayer)
            {
                PlayerSetPaidForRift(ptOtherPlayer, bPaid, FALSE, (ptPlayer->dwNephalemRiftFlags >> ESF_EMPOWERED) & 1);
            }
        }
    }
}








void UnlockRiftLevelsForParticipants(
    int32 nRiftTierCompleted)
{
    NephalemRiftState * ptState = GetGameState();
    if (ptState == NULL)
    {
        return;
    }

    PlayerList listPlayers(PlayerListGetElementPool());
    PlayerListGetAllConnected(listPlayers, -1, FALSE);

    POS posPlayer = listPlayers.GetHeadPosition();
    while (posPlayer != NULL)
    {
        Player * ptPlayerToCheck = listPlayers.GetNext(posPlayer);
        if (ptState->nRiftTier >= 0 && !PlayerPaidForRift(ptPlayerToCheck))
        {
            continue;
        }

        ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayerToCheck->idPlayerACD);
        if (ptACDPlayer == NULL)
        {
            continue;
        }

        ACDTrySetHighestUnlockedRiftLevel(*ptACDPlayer, nRiftTierCompleted);
    }
}


void Cleanup()
{

    GetGameState()->ResetState();
}



BOOL IsAnyRiftActive()
{

    NephalemRiftState * pState = GetGameState();
    return pState != NULL && pState->dwFlags != 0;
}


BOOL IsTieredRiftActive()
{
    if (!GameActiveGameCommonDataIsServer())
    {
        return FALSE;
    }


    NephalemRiftState * pState = GetGameState();
    return pState->IsTieredRift();
}


BOOL IsNonTieredRiftActive()
{

    NephalemRiftState * pState = GetGameState();
    return pState != NULL && pState->dwFlags != 0 && !pState->IsTieredRift();
}


void SetRiftActive(BOOL bIsActive)
{

    NephalemRiftState * pState = GetGameState();
    if (bIsActive)
        pState->dwFlags |= (1 << FLAG_IN_PROGRESS);
    else
        pState->dwFlags &= ~(1 << FLAG_IN_PROGRESS);
}










int32 GetActiveRiftLevel()
{

    NephalemRiftState * pState = GetGameState();
    return pState->nRiftTier;
}


void PlayerAcceptInvite(Player * ptPlayer, BOOL bEmpowered)
{

    NephalemRiftState * pState = GetGameState();
    if (!(pState->dwFlags & (1 << FLAG_INVITE_PHASE)) || !(ptPlayer->dwNephalemRiftFlags & (1 << ESF_PENDING_ACCEPT)))
    {
        return;
    }

    ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
    if (ptACDPlayer != NULL)
    {
        if (ptACDPlayer->AttributesGetInt(ATTRIBUTE_CANT_START_DISPLAYED_POWERS))
        {
            return;
        }

        if (bEmpowered)
        {
            pState->dwFlags &= ~(1 << FLAG_START_QUEUED);
            int32 nRiftLevel = pState->nRiftTier;
            uint64 nEmpowerCost = NephalemRift::GetEmpoweredRiftCost(nRiftLevel);
            if (nEmpowerCost > ptACDPlayer->GetCurrencyAmount(CURRENCY_GOLD))
            {
                PlayerDisplayErrorMessage(ptACDPlayer->id, GAMEERROR_EMPOWERED_RIFT_CANNOT_AFFORD);
                bEmpowered = FALSE;
            }
        }
    }

    pState->ClearFlags(ptPlayer);
    ptPlayer->dwNephalemRiftFlags = bEmpowered ? ((1 << ESF_ACCEPTED) | (1 << ESF_EMPOWERED)) : (1 << ESF_ACCEPTED);

    SendRiftInviteStateMessage(NULL);

    TryStartRift();
}









void PlayerDeclineInvite(
    Player * ptPlayer)
{
    NephalemRiftState * pState = GetGameState();
    if (!(pState->dwFlags & (1 << FLAG_INVITE_PHASE)) || ptPlayer == NULL ||
        !(ptPlayer->dwNephalemRiftFlags & (1 << ESF_PENDING_ACCEPT)))
    {
        return;
    }


    int32 nNumDeclined = 1;

    pState->ClearFlags(ptPlayer);
    ptPlayer->dwNephalemRiftFlags = (1 << ESF_DECLINED);



    if (ptPlayer->idGameConnection == -1)
    {
        ptPlayer->dwNephalemRiftFlags = (1 << ESF_PENDING_ACCEPT);
    }
    else
    {

        PlayerList listPlayers(PlayerListGetElementPool());
        PlayerListGetAllInGame(listPlayers, ptPlayer->nPlayerIndex, TRUE, -1);
        POS pos = listPlayers.GetHeadPosition();

        BOOL bInstigatorCancelled = FALSE;

        while (pos != NULL)
        {
            Player * ptOtherPlayer = listPlayers.GetNext(pos);
            if (PlayersAreLocalToEachOther(*ptPlayer, *ptOtherPlayer))
            {

                BOOL bWasInstigator = (ptOtherPlayer->dwNephalemRiftFlags & (1 << ESF_INSTIGATOR)) != 0;

                pState->ClearFlags(ptOtherPlayer);
                ptOtherPlayer->dwNephalemRiftFlags = (1 << ESF_DECLINED);
                nNumDeclined++;

                if (bWasInstigator)
                {
                    bInstigatorCancelled = TRUE;
                }
            }
        }

        if (bInstigatorCancelled)
        {
            SendRiftInviteStateMessage(NULL);
            CancelStart();
        }
    }


    SendRiftInviteStateMessage(NULL);

    pState->nDeclinedInvites += nNumDeclined;

    TryStartRift();
}


void PlayerAcceptStart(int32 nRiftTier, Player * ptPlayer, BOOL bEmpowered)
{

    NephalemRiftState * pState = GetGameState();
    if ((pState->dwFlags & (1 << FLAG_START_QUEUED)) == 0)
    {
        return;
    }

    if ((ptPlayer->dwNephalemRiftFlags & (1 << ESF_INSTIGATOR)) == 0)
    {
        return;
    }

    if (nRiftTier != -1 && !NephalemRift::HasKeyForGreaterRift(ptPlayer))
    {
        PlayerDisplayErrorMessage(ptPlayer->idPlayerACD, GAMEERROR_LOOTRUN_TIERED_NEED_KEY);
        PlayerDeclineStart(ptPlayer);
        return;
    }

    if (bEmpowered)
        ptPlayer->dwNephalemRiftFlags |= (1 << ESF_EMPOWERED);
    else
        ptPlayer->dwNephalemRiftFlags &= ~(1 << ESF_EMPOWERED);
    ptPlayer->dwNephalemRiftFlags |= (1 << ESF_ACCEPTED);
    TriggerRift(nRiftTier, ptPlayer);
}




void PlayerDeclineStart(Player * ptPlayer)
{

    NephalemRiftState * pState = GetGameState();
    if ((pState->dwFlags & (1 << FLAG_START_QUEUED)) == 0)
    {
        return;
    }

    if ((ptPlayer->dwNephalemRiftFlags & (1 << ESF_INSTIGATOR)) == 0)
    {
        return;
    }

    ptPlayer->dwNephalemRiftFlags &= ~(1 << ESF_INSTIGATOR);
    if (--pState->nStartPromptsOut == 0)
    {
        pState->ResetState();
    }
}



void PlayerRequestedJoinInfo(Player * ptPlayer)
{
    uint32 dwJoinUIValidFlags = (1 << ESF_ACCEPTED) | (1 << ESF_PENDING_ACCEPT) | (1 << ESF_DECLINED);
    NephalemRiftState * pState = GetGameState();
    if (ptPlayer != NULL && (pState->dwFlags & (1 << FLAG_INVITE_PHASE)) && (ptPlayer->dwNephalemRiftFlags & dwJoinUIValidFlags) && pState->tCachedJoinMessage.uRiftStartServerTime != 0)
    {
        PlayerList listPlayers(PlayerListGetElementPool());
        PlayerListGetSingle(listPlayers, *ptPlayer);
        MessageSendToClient(listPlayers, SMSG_RIFT_JOIN_ENCOUNTER, &pState->tCachedJoinMessage, sizeof(pState->tCachedJoinMessage));
        SendRiftInviteStateMessage(ptPlayer);
    }
}

















void PlayerRemoved(
    Player * ptPlayer,
    BOOL bPlayerWasDeleted)
{

    SGameGlobals & tSGameGlobals = SGameGlobalsGet();
    if (tSGameGlobals.dwFlags & 0x2)
    {
        return;
    }

    uint32 dwOldFlags = ptPlayer->dwNephalemRiftFlags;
    ptPlayer->dwNephalemRiftFlags = 0;

    NephalemRiftState * pState = GetGameState();


    if (dwOldFlags & ((1 << ESF_ACCEPTED) | (1 << ESF_PENDING_ACCEPT) | (1 << ESF_CONTINUE_WITHOUT)))
    {

        ptPlayer->dwNephalemRiftFlags = (1 << ESF_PENDING_ACCEPT);

        BOOL bContinue = (pState->dwFlags >> FLAG_PROMPT_CONTINUE_WITHOUT) & 1;
        ptPlayer->dwNephalemRiftFlags |= (bContinue << ESF_CONTINUE_WITHOUT);
        if (bContinue)
        {
            PlayerVoteCancel(ptPlayer);
        }

        PlayerDeclineInvite(ptPlayer);
    }


    if (!IsAnyRiftActive())
    {
        pState->ClearFlags(ptPlayer);
        return;
    }

    if (!bPlayerWasDeleted)
    {

        ptPlayer->dwNephalemRiftFlags = dwOldFlags;
    }

    if (!(ptPlayer->dwNephalemRiftFlags & (1 << ESF_ACTIVE)))
    {
        pState->ClearFlags(ptPlayer);
    }

    if (!(pState->dwFlags & (1 << FLAG_START_COUNTDOWN_ACTIVE)) &&
        (pState->dwFlags & ((1 << FLAG_INVITE_PHASE) | (1 << FLAG_PROMPT_CONTINUE_WITHOUT))))
    {

        if (ptPlayer->dwNephalemRiftFlags & (1 << ESF_INSTIGATOR))
        {
            SendRiftInviteStateMessage(NULL);
            CancelStart();
        }
        else
        {
            TryStartRift();
        }
    }

    BOOL bHasRewardStates = pState->mapPlayerRewardStates.size() != 0;
    if (bHasRewardStates && !PlayersAreStillChoosingRewards(ptPlayer))
    {
        SendPlayersDoneChoosingRewardsEvent(pState->idObeliskGizmo);
    }
}



















void PlayerVoteCancel(Player * ptPlayer)
{

    NephalemRiftState * pState = GetGameState();
    if ((pState->dwFlags & (1 << FLAG_PROMPT_CONTINUE_WITHOUT)) == 0)
    {
        return;
    }




    if ((ptPlayer->dwNephalemRiftFlags & (1 << ESF_CONTINUE_WITHOUT)) == 0)
    {
        return;
    }

    ptPlayer->dwNephalemRiftFlags = (1 << ESF_DECLINED);
    CancelStart();
}




void PlayerVoteContinue(Player * ptPlayer, BOOL bEmpowered)
{

    NephalemRiftState * pState = GetGameState();
    if (!(pState->dwFlags & (1 << FLAG_PROMPT_CONTINUE_WITHOUT)))
    {
        return;
    }

    if (!(ptPlayer->dwNephalemRiftFlags & (1 << ESF_CONTINUE_WITHOUT)))
    {
        return;
    }

    uint32 dwInstigatorFlag = ptPlayer->dwNephalemRiftFlags & (1 << ESF_INSTIGATOR);
    pState->ClearFlags(ptPlayer);

    if (bEmpowered)
    {
        ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
        uint64 nEmpowerCost = NephalemRift::GetEmpoweredRiftCost(pState->nRiftTier);
        if (ptACDPlayer == NULL || nEmpowerCost > ptACDPlayer->GetCurrencyAmount(CURRENCY_GOLD))
        {
            return;
        }
    }

    ptPlayer->dwNephalemRiftFlags = dwInstigatorFlag | (bEmpowered ? ((1 << ESF_ACCEPTED) | (1 << ESF_EMPOWERED)) : (1 << ESF_ACCEPTED));

    TryStartRift();
}











void PlayerPaymentWarningAccepted(Player * ptPlayer, BOOL bEnterPortal)
{

    NephalemRiftState * pState = GetGameState();
    if (pState->dwFlags & (1 << FLAG_IN_PROGRESS))
    {
        pState->ClearFlags(ptPlayer);
        ptPlayer->dwNephalemRiftFlags = (1 << ESF_ACCEPTED);
        if (EnterRift(pState->idPortalGizmo, ptPlayer) && bEnterPortal)
        {
            PortPlayer(ptPlayer, WARP_PORTAL);
        }
    }
}






void PlayerSetJewelUpgradesUsed(Player * ptPlayer, int32 nUsed)
{
    if (ptPlayer == NULL)
    {
        return;
    }



    NephalemRiftState * pState = GetGameState();
    if (pState == NULL)
    {
        return;
    }

    GreaterRiftRewardState * ptRewardState = blzext::unordered_map_find_ptr(pState->mapPlayerRewardStates, ptPlayer->uHeroId);
    if (ptRewardState != NULL)
    {
        ptRewardState->nJewelUpgradesUsed = nUsed;
    }

    ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
    if (ptACDPlayer != NULL)
    {
        ptACDPlayer->AttributesSetInt(ATTRIBUTE_JEWEL_UPGRADES_USED, nUsed);
    }
}



void PlayerSetJewelBonusUpgradesLeft(Player * ptPlayer, int32 nRemaining)
{
    if (ptPlayer == NULL)
    {
        return;
    }



    NephalemRiftState * pState = GetGameState();
    if (pState == NULL)
    {
        return;
    }

    GreaterRiftRewardState * ptRewardState = blzext::unordered_map_find_ptr(pState->mapPlayerRewardStates, ptPlayer->uHeroId);
    if (ptRewardState != NULL)
    {
        ptRewardState->nJewelUpgradesBonus = nRemaining;
    }

    ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
    if (ptACDPlayer != NULL)
    {
        ptACDPlayer->AttributesSetInt(ATTRIBUTE_JEWEL_UPGRADES_BONUS, nRemaining);
    }
}



void NephalemRiftStateCreateForGame()
{

    if (GameActiveGameCommonDataIsServer())
    {
        SGameManager::GameGet().ptNephalemRiftState = new NephalemRiftState();
    }
}


void NephalemRiftStateDestroyForGame()
{

    if (GameActiveGameCommonDataIsServer())
    {


        ::delete SGameManager::GameGet().ptNephalemRiftState; SGameManager::GameGet().ptNephalemRiftState = NULL;
    }
}

void sAddRewardStateForPlayer(const uint64 uHeroID, const BOOL bSuccess, const int32 nNumBonusUpgrades)
{



    NephalemRiftState * ptRiftState = GetGameState();
    if (ptRiftState == NULL)
    {
        return;
    }

    GreaterRiftRewardState tRewardState;
    tRewardState.eChoiceState = bSuccess ? NephalemRift::RIFT_GEM_UPGRADE : NephalemRift::RIFT_FAILURE_NO_REWARD;
    tRewardState.bSuccess = bSuccess;
    tRewardState.nJewelUpgradesUsed = 0;
    tRewardState.nJewelUpgradesBonus = nNumBonusUpgrades;










    ScopedGameBalanceDefinitionLockR pdefGB(GlobalSNOGet(GLOBALSNO_GB_TIERED_LOOT_RUN_LEVELS));

    int32 nGameBalanceIndex = Math::Max(ptRiftState->nRiftTier, 0);
    nGameBalanceIndex = Math::Min(nGameBalanceIndex, (int32)pdefGB->tTieredLootRunLevelTable.serTieredLootRunLevels.ElementCount(sizeof(TieredLootRunLevel)) - 1);

    if (nGameBalanceIndex >= 0 && nGameBalanceIndex < (int32)pdefGB->tTieredLootRunLevelTable.serTieredLootRunLevels.ElementCount(sizeof(TieredLootRunLevel)))
    {
        tRewardState.nJewelUpgradesMax = pdefGB->tTieredLootRunLevelTable.ptTieredLootRunLevels[nGameBalanceIndex].tLootRunValues.nJewelUpgradeCharges;
    }
    else tRewardState.nJewelUpgradesMax = 0;


    ptRiftState->mapPlayerRewardStates[uHeroID] = tRewardState;
}


static int32 sGetNumBonusUpgrades(const Player & tPlayer)
{
    int32 nNumBonusUpgrades = 0;

    if (PlayerGetEmpoweredRift(&tPlayer))
    {
        nNumBonusUpgrades++;
    }
    BOOL bNoDeath = NephalemRift::PlayerHasNoRiftDeaths(&tPlayer);
    if (bNoDeath)
    {
        nNumBonusUpgrades++;
    }

    return nNumBonusUpgrades;
}


void PlayersGiveAllRiftRewardChoices(BOOL bSuccess)
{


    NephalemRiftState * ptRiftState = GetGameState();
    if (ptRiftState == NULL)
    {
        return;
    }

    PlayerList listPlayers(PlayerListGetElementPool());
    PlayerListGetAllConnected(listPlayers, -1, FALSE);

    POS posPlayer = listPlayers.GetHeadPosition();




    while (posPlayer && IsTieredRiftActive())
    {
        Player * ptPlayerToCheck = listPlayers.GetNext(posPlayer);
        if (ptPlayerToCheck && PlayerPaidForRift(ptPlayerToCheck))
        {
            sAddRewardStateForPlayer(ptPlayerToCheck->uHeroId, bSuccess, sGetNumBonusUpgrades(*ptPlayerToCheck));
        }
    }












    posPlayer = listPlayers.GetHeadPosition();
    while (posPlayer && IsTieredRiftActive())
    {
        Player * ptPlayerToCheck = listPlayers.GetNext(posPlayer);
        if (ptPlayerToCheck && PlayerPaidForRift(ptPlayerToCheck))
        {



            ActorCommonData * ptACD = ACDTryToGet(ptPlayerToCheck->idPlayerACD);
            if (ptACD)
            {



                if (bSuccess > 0)
                {
                    ptACD->AttributesSetInt(ATTRIBUTE_TIERED_LOOT_RUN_REWARD_CHOICE_STATE, NephalemRift::RIFT_GEM_UPGRADE);
                    ptACD->AttributesSetInt(ATTRIBUTE_TIERED_LOOT_RUN_REWARD_RECEIVES_KEY, FALSE);

                    int32 nTieredLootRunLevel = ACDGetTieredLootRunLevel(*ptACD);
                    if (nTieredLootRunLevel >= 0)
                    {
                        ScopedGameBalanceDefinitionLockR pdefGB(GlobalSNOGet(GLOBALSNO_GB_TIERED_LOOT_RUN_LEVELS));


                        int32 nGameBalanceIndex = Math::Min(nTieredLootRunLevel, (int32)pdefGB->tTieredLootRunLevelTable.serTieredLootRunLevels.ElementCount(sizeof(TieredLootRunLevel)) - 1);

                        if (nGameBalanceIndex >= 0 && nGameBalanceIndex < (int32)pdefGB->tTieredLootRunLevelTable.serTieredLootRunLevels.ElementCount(sizeof(TieredLootRunLevel)))
                        {
                            const TieredLootRunLevel & tTieredLootRunLevel = pdefGB->tTieredLootRunLevelTable.ptTieredLootRunLevels[nGameBalanceIndex];
                            ptACD->AttributesSetInt(ATTRIBUTE_JEWEL_UPGRADES_MAX, tTieredLootRunLevel.tLootRunValues.nJewelUpgradeCharges);
                            ptACD->AttributesSetInt(ATTRIBUTE_JEWEL_UPGRADES_BONUS, sGetNumBonusUpgrades(*ptPlayerToCheck));
                            ptACD->AttributesSetInt(ATTRIBUTE_JEWEL_UPGRADES_USED, 0);
                        }
                    }
                }
                else
                {
                    ptACD->AttributesSetInt(ATTRIBUTE_TIERED_LOOT_RUN_REWARD_CHOICE_STATE, NephalemRift::RIFT_FAILURE_NO_REWARD);
                }
            }
        }
    }
}

BOOL PlayersAreStillChoosingRewards(const Player * ptPlayerToIgnore)
{

    PlayerList listPlayers(PlayerListGetElementPool());
    PlayerListGetAllConnected(listPlayers, -1, FALSE);

    POS posPlayer = listPlayers.GetHeadPosition();
    while (posPlayer != NULL)
    {
        Player * ptPlayerToCheck = listPlayers.GetNext(posPlayer);
        ActorCommonData * ptACD = ACDTryToGet(ptPlayerToCheck->idPlayerACD);

        if (ptPlayerToCheck == ptPlayerToIgnore || ptACD == NULL)
        {
            continue;
        }

        NephalemRift::RIFT_REWARD_CHOICE_STATE eRewardChoiceState = (NephalemRift::RIFT_REWARD_CHOICE_STATE)FastAttribGetValueInt(ptACD->idFastAttrib, ATTRIBUTE_TIERED_LOOT_RUN_REWARD_CHOICE_STATE);
        if (eRewardChoiceState == NephalemRift::RIFT_GEM_UPGRADE)
        {

            int32 nJewelUpgradesUsed = ptACD->AttributesGetInt(ATTRIBUTE_JEWEL_UPGRADES_USED);
            int32 nJewelUpgradesMax = ptACD->AttributesGetInt(ATTRIBUTE_JEWEL_UPGRADES_MAX);
            if (nJewelUpgradesUsed != nJewelUpgradesMax)
            {
                return TRUE;
            }
        }
    }

    return FALSE;
}




void PlayerStateSetNewSoloRecord(Player * ptPlayer, BOOL bHasNewSoloRecord)
{

    if (ptPlayer == NULL)
        return;




    GreaterRiftPlayerState * ptPlayerState = GetGreaterRiftPlayerState(ptPlayer);

    ptPlayerState->bHasNewSoloRecord = bHasNewSoloRecord;
}
void AddArtificiallyElapsedTime(int32 nTicks)
{

    NephalemRiftState * pState = GetGameState();
    pState->dwArtificiallyElapsedTime += nTicks;
}




int32 GetArtificiallyElapsedTimeMS()
{
    return GetGameState()->dwArtificiallyElapsedTime * 1000 / 60;
}



void SendPlayersDoneChoosingRewardsEvent(ACDID idACDPlayer)
{
    ScriptEventOnUserEvent(ALL_PLAYERS_CONSUMED_RIFT_REWARDS_EVENT, idACDPlayer, 0);
}





GreaterRiftRewardState * PlayerGetGreaterRiftRewardState(Player * ptPlayer)
{
    if (ptPlayer == NULL)
    {
        return NULL;
    }


    NephalemRiftState * ptRiftState = GetGameState();
    if (ptRiftState == NULL)
    {
        return NULL;
    }


    return blzext::unordered_map_find_ptr(ptRiftState->mapPlayerRewardStates, ptPlayer->uHeroId);
}


ACDID GetJewelUpgradeVendor()
{

    NephalemRiftState * ptRiftState = GetGameState();
    if (ptRiftState)
        return ptRiftState->idACDJewelUpgradeVendor;
    return ACDID_INVALID;
}






void SetJewelUpgradeVendor(ACDID idACDJewelUpgradeVendor)
{

    NephalemRiftState * ptRiftState = GetGameState();
    if (ptRiftState)
        ptRiftState->idACDJewelUpgradeVendor = idACDJewelUpgradeVendor;
}








void sExportRiftEndScreenPlayerInfo(const Player & tPlayer, D3::GameMessage::RiftEndScreenConsolePlayerInfo * ptPlayerInfo)
{


    ActorCommonData & tACDPlayer = ACDGet(tPlayer.idPlayerACD);

    ptPlayerInfo->set_player_name(tPlayer.tAccount.usName);
    ptPlayerInfo->set_hero_gbid_class(tACDPlayer.hGB.gbid);
    ptPlayerInfo->set_hero_flags(tPlayer.tHeroStateData.uPlayerFlags);
    ptPlayerInfo->set_hero_level(tACDPlayer.AttributesGetInt(ATTRIBUTE_LEVEL));
    ptPlayerInfo->set_alt_level(tACDPlayer.AttributesGetInt(ATTRIBUTE_ALT_LEVEL));
    tACDPlayer.ptVisualInventory->tEquipmentCurrent.Export(*ptPlayerInfo->mutable_hero_visual_equipment());
}




void PlayerSendRiftEndScreenInfo(const Player & tDestPlayer, BOOL bSuccess, int32 nCompletionTimeMS, int32 nRiftTier, const D3::Quests::QuestReward & tReward, ChallengeRift::RewardScreenInfo tChallengeRiftInfo)
{
    if (PlayerIsHardcoreAndDead(tDestPlayer))
    {
        return;
    }









    GreaterRiftPlayerState * ptPlayerState = GetGreaterRiftPlayerState(&tDestPlayer);
    if (ptPlayerState == NULL)
    {
        return;
    }

    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetSingle(listDestPlayers, tDestPlayer.idPlayerACD);

    D3::GameMessage::RiftEndScreenInfo tRiftEndScreenInfo;
    tRiftEndScreenInfo.set_rift_tier(nRiftTier + 1);
    tRiftEndScreenInfo.set_success(bSuccess != FALSE);
    tRiftEndScreenInfo.set_completion_time_ms(nCompletionTimeMS);
    tRiftEndScreenInfo.set_xp_reward(tReward.xp_granted() + tReward.bonus_xp_granted());
    tRiftEndScreenInfo.set_gold_reward(tReward.gold_granted());
    tRiftEndScreenInfo.set_is_from_cheat(false);
    tRiftEndScreenInfo.set_new_personal_best(ptPlayerState->bHasNewSoloRecord != FALSE);

    if (GameIsWeeklyChallenge())
    {
        tRiftEndScreenInfo.set_challenge_rift_reward_gbid(tChallengeRiftInfo.challengeRiftRewardGbid);
        tRiftEndScreenInfo.set_challenge_rift_time_to_beat_ms(tChallengeRiftInfo.challengeRiftTimeToBeatMs);
        tRiftEndScreenInfo.set_challenge_rift_reward_state(tChallengeRiftInfo.challengeRiftRewardState);
        tRiftEndScreenInfo.set_challenge_rift_number(tChallengeRiftInfo.challengeRiftNum);
    }

    D3::Account::BannerConfiguration & tBanner = *tRiftEndScreenInfo.mutable_banner_configuration();
    tDestPlayer.tAccount.tBanner.Export(tBanner);






    sExportRiftEndScreenPlayerInfo(tDestPlayer, tRiftEndScreenInfo.add_console_player_info());
    tRiftEndScreenInfo.set_console_num_participants(RiftPlayerCount());


    NephalemRiftState * ptRiftState = GetGameState();
    if (ptRiftState != NULL)
    {
        for (blz::unordered_map<unsigned long, GreaterRiftPlayerState>::iterator iter = ptRiftState->mapPlayerStates.begin(); iter != ptRiftState->mapPlayerStates.end(); iter++)
        {
            if (iter->second.tGameAccount != ptPlayerState->tGameAccount)
            {
                Player * ptPlayer = PlayerGetByGameAccount(iter->second.tGameAccount);
                if (ptPlayer != NULL)
                {
                    sExportRiftEndScreenPlayerInfo(*ptPlayer, tRiftEndScreenInfo.add_console_player_info());
                }
            }
        }
    }

    MessageSendToClient(listDestPlayers, SMSG_SHOW_RIFT_END_SCREEN, tRiftEndScreenInfo);
}





void PlayerManuallyPortIntoRift(Player * ptPlayer)
{
    if (ptPlayer != NULL)
    {
        PortPlayer(ptPlayer, WARP_TIERED_RIFT);
    }
}



























































}  // namespace NephalemRiftManager
