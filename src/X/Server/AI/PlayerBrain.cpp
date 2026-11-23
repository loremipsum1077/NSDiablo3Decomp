// Original file: X/Server/AI/PlayerBrain.cpp
#include "../Actors/Actors.h"
#include "./ActorAI.h"
#include "./PlayerBrain.h"
#include "./BrainStance.h"
#include "./BrainComponents.h"
#include "../SWorld.h"
#include "../Actors/ActorPowers.h"
#include "../../Common/GizmoSwig.h"
#include "../../Common/Items.h"
#include "../../Common/TagMaps/PowerTags.h"
#include "../SGame.h"
#include "../PlayerList.h"
#include "../Timers.h"
#include "../Crafting/SItemCrafting.h"
#include "../Powers/Powers.h"
#include "../../Common/ActorCommonData.h"
#include "../../Common/ActorInventory.h"
#include "../../Common/GameCommonData.h"
#include "../../Common/HotbarButton.h"
#include "../../Common/Attributes/FastAttribConstantsSwig.h"
#include "../../Common/Attributes/FastAttributeStructs.h"
#include "../../Common/Attributes/FastAttribute.h"
#include "../../Common/Definitions/PowerDefinition.h"
#include "../../Common/Networking/GameMessage.h"
#include "../../Common/Player.h"
#include "../../Common/Powers/PowersCommon.h"
#include "../../Common/Powers/PowersStruct.h"
#include "../../Common/Target.h"
#include <string.h>

void PowerAbortChannelled(ACDID idACDPlayer, SNO snoPower);  // X/Server/Powers/Powers.cpp:5100
BOOL SACDInventoryProcessMoveRequest(ActorCommonData & tACDPlayer, ActorCommonData & tACDItem, const InventoryLocation & tInvLocDest);  // ActorInventory.cpp:7927
BOOL SACDInventoryProcessUseRequest(const ACDID idPlayerACD, const ACDID idACDItemToUse, const Target * ptTarget);  // ActorInventory.cpp:8197
BOOL ACDInventoryFindNonFullStackInSlot(const ActorCommonData & tACDItem, const ActorCommonData & tACDContainer, const InventorySlotType eCheckSlot, InventoryLocation & tNewLocation, const int32 * pnSlotYMinMax);  // ActorInventory.cpp:2289
BOOL ACDInventoryCanHold(const ActorCommonData & tACD, const ActorCommonData & tACDItem, const InventorySlotType eSlot, IVector2D & tWhereCanHold, const int32 * pnSlotYMinMax);  // ActorInventory.cpp:869
void ACDInventoryLocationSet(InventoryLocation & tInvLoc, const ACDID idACDContainer, const InventorySlotType eSlot, const int32 nSlotX, const int32 nSlotY);  // ActorInventory.cpp:9743
BOOL ACDInventoryStackItems(ActorCommonData & tACDSrcItem, ActorCommonData & tACDDestItem);  // ActorInventory.cpp:2355
void SQuestsOnPlayerAcquireItem(ACDID idACDPlayer, ACDID idACDItem);  // X/Server/Quests/SQuests.cpp:4685
void PlayerSetHotbarBtnAssignment(Player * ptPlayer, const SkillButton eButton, const HotbarButtonData & tButtonData);  // Player.cpp:1778
void SPlayerSkillSlotMarkAsAssigned(Player & tPlayer, const SkillButton eButton);  // X/Server/SPlayer.cpp:6441
EBrainPowerResult BrainStanceFailureReason(ActorID idActor, SNO snoPower, const Target & tTarget, BOOL bHoldPosition);  // BrainStance.cpp:217
PowerID BrainStanceDoPower(ActorID idActor, const Target & tTarget, SNO snoPower, EBrainPowerResult eReason, void * ptExtraPowerStartData, const PreplayedAnimData * ptPreplayedAnimData, int32 nComboLevel, uint32 dwTargetMessageFlags);  // BrainStance.cpp
BOOL ActorPowersPrimaryIsNotInterruptable(ActorID idActor);  // X/Server/Actors/ActorPowers.cpp:253
void ActorPowersInterruptPrimary(ActorID idActor, BOOL bIgnoreContactFrame);  // X/Server/Actors/ActorPowers.cpp:392
ACDID ACDInventoryGetSecondaryWeaponOrShield(ACDID idACD);  // ActorInventory.cpp:5373
BOOL ItemIsOneHandedWeapon(const ActorCommonData & tACDItem);  // X/Common/Items.cpp:3422
void SendPlayerCancelWarp(const ACDID idACDPlayer);  // X/Server/Network/ServerMessageSend.cpp:1403
SNO PowerGetNormalAttackReplacement(ACDID idACD);  // X/Common/Powers/PowersCommon.cpp:2206
void PowerAPI_SetNormalAttackReplacement(ACDID idACD, SNO snoPower);  // X/Server/Powers/PowerAPI.cpp:12663
BOOL ACDInSameWorld(const ActorCommonData & tACD1, const ActorCommonData & tACD2);  // X/Common/ActorCommonData.cpp:2503
BOOL SACDInventoryDeleteItemAfterUse(ActorCommonData & tACDItem);  // ActorInventory.cpp:7193




// X/Common/Networking/GameMessage.h






//@ 59
static const int32 MAX_QUEUED_POWER_TIME = 45;

//@ 68
uint32 INTERRUPT_SLOP_TICKS = 5;

//@ 70
static BOOL sPlayerBrainAttack(Player * ptPlayer, ActorCommonData & tACDPlayer, const Target & tTarget, const SNO snoPower, const ACDID idACDItemBeingUsed, const PreplayedAnimData & tPreplayedAnimData, const int32 nComboLevel, const uint32 dwTargetMessageFlags)
{
    ActorID idActorPlayer = (ActorID)ACDGetOwner(tACDPlayer.id);

    EBrainPowerResult eResult = BrainStanceFailureReason(idActorPlayer, snoPower, tTarget, TRUE);

    if (eResult == BRAIN_POWER_START || eResult == BRAIN_POWER_RETARGET)
    {
        uint32 dwNow = GameGetServerTime();

        if (ptPlayer->tQueuedPower.dwPowerLastEndTime > dwNow + INTERRUPT_SLOP_TICKS &&
            !PowerShouldUseClientWalking(tACDPlayer, AutoPowerDefinitionR(snoPower)))
        {
            eResult = BRAIN_POWER_CANT_INTERRUPT;
        }
    }

    if (eResult != BRAIN_POWER_START && eResult != BRAIN_POWER_RETARGET)
    {
        if (tPreplayedAnimData.dwPrePlayedAnimFlags & 1)
        {
            AITrace(tACDPlayer.id, "player swished because BrainStanceFailureReason\n");
        }

        if (eResult == BRAIN_POWER_CANT_INTERRUPT)
        {
            SNO snoCurrentPower = ActorPowersGetPrimaryPower(idActorPlayer);
            if (snoCurrentPower == SNO_NONE)
            {
                return TRUE;
            }
            if (PowerIsChannelled(tACDPlayer, AutoPowerDefinitionR(snoCurrentPower)))
            {
                if (snoCurrentPower == snoPower)
                {
                    PowerID idPowerPrimary = ActorPowersGetPrimary(idActorPlayer);
                    Power * ptPrimaryPower = PowerTryToGet(idPowerPrimary);
                    if (ptPrimaryPower)
                    {
                        ptPrimaryPower->tTarget = tTarget;
                        ptPrimaryPower->dwLastContactTick = GameGetServerTime();
                    }
                    return FALSE;
                }
                return TRUE;
            }
            else if (PowerIsCancellableByAnyPower(AutoPowerDefinitionR(snoCurrentPower)))
            {
                ActorPowersInterruptPrimary(idActorPlayer, FALSE);
                eResult = BRAIN_POWER_START;
            }
            else if (PowerIsCancellable(AutoPowerDefinitionR(snoCurrentPower)))
            {
                if (!PowerIsUninterruptableDuring(&tACDPlayer, AutoPowerDefinitionR(snoCurrentPower)))
                {
                    if (snoCurrentPower != snoPower)
                    {
                        if (PowerCancelsOtherPowers(AutoPowerDefinitionR(snoPower)))
                        {
                            if (PowerCancelsOnInterruptFrame(AutoPowerDefinitionR(snoPower)) &&
                                ActorPowersPrimaryIsNotInterruptable(idActorPlayer))
                            {
                                return TRUE;
                            }
                            ActorPowersInterruptPrimary(idActorPlayer, FALSE);
                            eResult = BRAIN_POWER_START;
                        }
                        else
                        {
                            return TRUE;
                        }
                    }
                    else
                    {
                        return TRUE;
                    }
                }
                else
                {
                    return TRUE;
                }
            }
            else
            {
                return TRUE;
            }
        }
        else
        {
            return TRUE;
        }
    }

    uint8 bytePowerTemplateData[512];
    int32 nPowerDataSize = 0;

    if (idACDItemBeingUsed != ACDID_INVALID)
    {
        ACDID * ptPowerData = (ACDID *)bytePowerTemplateData;
        *ptPowerData = idACDItemBeingUsed;
        nPowerDataSize = sizeof(ACDID);
    }

    if (ACDIsDualWielding(tACDPlayer, TRUE))
    {
        if (PowerGeneralTagGetBool(AutoPowerDefinitionR(snoPower), TAG_POWER_USES_MAINHAND_ONLY))
        {
            tACDPlayer.AttributesSetInt(ATTRIBUTE_DUALWIELD_HAND, 0);

            ACDID idACDPrimaryWeapon = ACDInventoryGetPrimaryWeapon(tACDPlayer.id);
            ACDID idACDSecondaryWeapon = ACDInventoryGetSecondaryWeaponOrShield(tACDPlayer.id);

            ActorCommonData * ptACDPrimaryWeapon = ACDTryToGet(idACDPrimaryWeapon);
            ActorCommonData * ptACDSecondaryWeapon = ACDTryToGet(idACDSecondaryWeapon);

            if (!ptACDPrimaryWeapon && ptACDSecondaryWeapon && ItemIsOneHandedWeapon(*ptACDSecondaryWeapon))
            {
                tACDPlayer.AttributesSetInt(ATTRIBUTE_DUALWIELD_HAND, 1);
            }
        }
        else if (PowerAffectedByDualWield(AutoPowerDefinitionR(snoPower)))
        {
            tACDPlayer.AttributesSetInt(ATTRIBUTE_DUALWIELD_HAND, tACDPlayer.AttributesGetInt(ATTRIBUTE_DUALWIELD_HAND_NEXT));
            PlayerDetermineNextDualWieldHand(tACDPlayer.id);
        }
    }

    BrainStanceDoPower(idActorPlayer, tTarget, snoPower, eResult, bytePowerTemplateData, &tPreplayedAnimData, nComboLevel, dwTargetMessageFlags);
    return FALSE;
}

//@ 226
void PlayerBrainTarget(const ACDID idACDPlayer,
                       const Target & tTarget,
                       const SNO snoPower,
                       const ACDID idACDItemBeingUsed,
                       const PreplayedAnimData & tPreplayedAnimData,
                       const int32 nComboLevel,
                       const uint32 dwTargetMessageFlags)
{
    Player * ptPlayer = PlayerGetByACD(idACDPlayer);
    if (ptPlayer == NULL)
    {
        return;
    }

    SNO snoQueuedPower = ptPlayer->tQueuedPower.snoQueuedPower;
    ptPlayer->tQueuedPower.snoQueuedPower = SNO_NONE;

    ActorCommonData & tACDPlayer = ACDGet(idACDPlayer);
    if (!tACDPlayer.IsAlive())
    {

        ActorCommonData * ptACDTarget = ACDTryToGet(tTarget.idACDTarget);
        if (ptACDTarget)
        {
            if (ptACDTarget->GetActorType() == ACTORTYPE_GIZMO)
            {
                GizmoType eGizmoType = ptACDTarget->GetGizmoType();
                if (eGizmoType == ACTORGIZMO_PORTAL || eGizmoType == ACTORGIZMO_HEARTH_PORTAL)
                {
                    SendPlayerCancelWarp(idACDPlayer);
                }
            }
        }

        if (tPreplayedAnimData.dwPrePlayedAnimFlags & 1)
        {
            AITrace(idACDPlayer, "player swished because player not alive\n");
        }
        return;
    }

    SNO snoPowerToUse = snoPower;

    SNO snoReplacementPower = PowerGetNormalAttackReplacement(idACDPlayer);
    if (snoReplacementPower == snoPowerToUse)
    {
        PowerAPI_SetNormalAttackReplacement(idACDPlayer, SNO_NONE);
    }

    ActorID idActorPlayer = (ActorID)ACDGetOwner(idACDPlayer);
    switch (tTarget.eType)
    {
    case TT_ACTOR:
        {
            ActorCommonData * ptACDTarget = ACDTryToGet(tTarget.idACDTarget);
            if (!ptACDTarget ||
                !ptACDTarget->IsInWorld() ||
                !ACDInSameWorld(tACDPlayer, *ptACDTarget))
            {
                if (tPreplayedAnimData.dwPrePlayedAnimFlags & 1)
                {
                    AITrace(idACDPlayer, "player swished because target invalid\n");
                }
                BrainAPIDoIdle(idActorPlayer);
                return;
            }
        }
        break;

    case TT_POINT:
    case TT_ACTOR_WITH_FALLBACK_POINT:
        if (!SWorldTryToGet(tTarget.tPlace.idSWorld))
        {
            BrainAPIDoIdle(idActorPlayer);
            return;
        }
        break;
    }

    if (snoPowerToUse == SNO_NONE)
    {
        return;
    }

    ActorCommonData * ptACDItemBeingUsed = ACDTryToGet(idACDItemBeingUsed);
    SNO snoActorItem = PowerConsumedItem(AutoPowerDefinitionR(snoPowerToUse));
    if (snoActorItem != SNO_NONE)
    {
        if (!ptACDItemBeingUsed || !SACDInventoryDeleteItemAfterUse(*ptACDItemBeingUsed))
        {
            return;
        }
    }

    BOOL bQueuePower = sPlayerBrainAttack(ptPlayer, tACDPlayer, tTarget, snoPowerToUse, idACDItemBeingUsed, tPreplayedAnimData, nComboLevel, dwTargetMessageFlags);

    BOOL bIsClientWalkPower = PowerShouldUseClientWalking(tACDPlayer, AutoPowerDefinitionR(snoPowerToUse));

    if (PowerIsChannelled(tACDPlayer, AutoPowerDefinitionR(snoPowerToUse)) && !bIsClientWalkPower)
    {
        bQueuePower = FALSE;
    }

    if (bQueuePower)
    {
        if (!bIsClientWalkPower)
        {
            BOOL bIsInterrupt = PowerCancelsOtherPowers(AutoPowerDefinitionR(snoPowerToUse)) ? TRUE : FALSE;
            uint32 dwEarliestQueueTime = bIsInterrupt ? ptPlayer->tQueuedPower.dwPowerLastInterruptTime : ptPlayer->tQueuedPower.dwPowerLastEndTime;
            if (dwEarliestQueueTime >= MAX_QUEUED_POWER_TIME)
            {
                dwEarliestQueueTime -= MAX_QUEUED_POWER_TIME;
            }

            uint32 dwNow = GameGetServerTime();
            if (dwEarliestQueueTime > dwNow)
            {
                return;
            }

            if (snoQueuedPower != SNO_NONE)
            {

                BOOL bQueuedIsInterrupt = PowerCancelsOtherPowers(AutoPowerDefinitionR(snoQueuedPower)) ? TRUE : FALSE;
                if (!bIsInterrupt && bQueuedIsInterrupt)
                {
                    return;
                }
            }
        }
    }
    else
    {
        ptPlayer->tQueuedPower.Clear();
        return;
    }

    if (snoQueuedPower != snoPowerToUse)
    {
        ptPlayer->tQueuedPower.dwQueuedTime = GameGetServerTime();
    }

    ptPlayer->tQueuedPower.snoQueuedPower = snoPowerToUse;
    ptPlayer->tQueuedPower.tQueuedTarget = tTarget;
    ptPlayer->tQueuedPower.nQueuedComboLevel = nComboLevel;
    ptPlayer->tQueuedPower.idACDQueuedItemBeingUsed = idACDItemBeingUsed;
    ptPlayer->tQueuedPower.tPreplayedAnimData = tPreplayedAnimData;
    ptPlayer->tQueuedPower.dwQueuedTargetMessageFlags = dwTargetMessageFlags;
}

//@ 407
void PlayerBrainTryQueuedPower(ACDID idACDPlayer)
{
    Player * ptPlayer = PlayerGetByACD(idACDPlayer);
    if (ptPlayer == NULL)
    {
        return;
    }

    SNO snoPower = ptPlayer->tQueuedPower.snoQueuedPower;
    if (snoPower == SNO_NONE)
    {
        return;
    }

    uint32 dwNow = GameGetServerTime();

    if (ptPlayer->tQueuedPower.dwQueuedTime + MAX_QUEUED_POWER_TIME < dwNow)
    {
        ActorCommonData * ptACDPlayer = ACDTryToGet(idACDPlayer);
        if (ptACDPlayer && PowerShouldUseClientWalking(*ptACDPlayer, AutoPowerDefinitionR(snoPower)))
        {
            PlayerClearClientWalkPowerMessage tMessage;
            tMessage.snoPower = snoPower;
            tMessage.ann = ANNFromACD(idACDPlayer);

            PlayerList listDestPlayers(PlayerListGetElementPool());
            PlayerListGetSingle(listDestPlayers, idACDPlayer);

            MessageSendToClient(listDestPlayers, SMSG_PLAYER_CLEAR_CLIENT_WALK_POWER, &tMessage, sizeof(tMessage));
        }


        ptPlayer->tQueuedPower.Clear();
        return;
    }

    ptPlayer->tQueuedPower.tPreplayedAnimData.dwServerTimeAnimStarted = dwNow;


    ptPlayer->tQueuedPower.tQueuedTarget.ValidateTargetACD();

    PlayerBrainTarget(idACDPlayer,
        ptPlayer->tQueuedPower.tQueuedTarget,
        snoPower,
        ptPlayer->tQueuedPower.idACDQueuedItemBeingUsed,
        ptPlayer->tQueuedPower.tPreplayedAnimData,
        ptPlayer->tQueuedPower.nQueuedComboLevel,
        ptPlayer->tQueuedPower.dwQueuedTargetMessageFlags);
}

//@ 462
void PlayerBrainClearQueuedPower(ACDID idACDPlayer)
{
    Player * ptPlayer = PlayerGetByACD(idACDPlayer);
    if (ptPlayer == NULL)
    {
        return;
    }

    ptPlayer->tQueuedPower.Clear();
}

//@ 476
SNO PlayerBrainGetQueuedPower(const ACDID idACDPlayer)
{
    Player * ptPlayer = PlayerGetByACD(idACDPlayer);
    if (ptPlayer == NULL)
    {
        return SNO_NONE;
    }

    return ptPlayer->tQueuedPower.snoQueuedPower;
}

void PlayerBrainRequestAbortChannelled(ACDID idACDPlayer, SNO snoPower);

//@ 515
void sPlayerBrainProcessQueuedAbortChannelled(TimerID, DataID idOwner, void * pData)
{
    SNO snoPower = (SNO)(intptr_t)pData;

    PlayerBrainRequestAbortChannelled((ACDID)idOwner, snoPower);
}

//@ 527
void PlayerBrainRequestAbortChannelled(ACDID idACDPlayer, SNO snoPower)
{
    Player * ptPlayer = PlayerGetByACD(idACDPlayer);
    if (ptPlayer == NULL)
    {
        return;
    }




    uint32 dwNow = GameGetServerTime();
    if (ptPlayer->tQueuedPower.dwPowerLastInterruptTime > dwNow)
    {

        TimerRegister(idACDPlayer, TIMER_OWNER_ACD, ptPlayer->tQueuedPower.dwPowerLastInterruptTime - dwNow, (void *)(intptr_t)snoPower, sPlayerBrainProcessQueuedAbortChannelled);
    }
    else
    {
        PowerAbortChannelled(idACDPlayer, snoPower);


        PlayerBrainTryQueuedPower(idACDPlayer);
    }
}

//@ 557
void sUpdatePlayerDataIfNecessary(const ACDID idACD)
{

    Player * ptPlayer = PlayerGetByACD(idACD);

    if (ptPlayer)
    {



        ACDGet(idACD).MarkHeroStateSavedDataChanged();
    }
}

//@ 572
void PlayerBrainSpendActiveSkillPoint(ActorCommonData & tACDPlayer, const SNO snoPower)
{




    tACDPlayer.AttributesSetInt(MakeAttribKey(ATTRIBUTE_SKILL, snoPower), 1);

    ACDInventoryEquipmentOrSkillChanged(tACDPlayer);
    sUpdatePlayerDataIfNecessary(tACDPlayer.id);
}

//@ 586
void PlayerBrainSpendTraitPoint(ActorCommonData & tACDPlayer, const SNO snoPower)
{




    tACDPlayer.AttributesSetInt(MakeAttribKey(ATTRIBUTE_SKILL, snoPower), 1);

    ACDInventoryEquipmentOrSkillChanged(tACDPlayer);
    sUpdatePlayerDataIfNecessary(tACDPlayer.id);
}

//@ 600
BOOL PlayerBrainTryInventoryMoveItem(ActorCommonData & tACDPlayer, ActorCommonData & tACDItem, const InventoryLocation & tInvLocDest)



{
    if (tACDItem.IsInInventory())
    {
        InventoryLocation & tInvLocCurrent = tACDItem.GetInventoryLocation();
        if (memcmp(&tInvLocCurrent, &tInvLocDest, sizeof(InventoryLocation)) == 0)
        {
            return TRUE;
        }
    }



    BOOL bSuccess = SACDInventoryProcessMoveRequest(tACDPlayer, tACDItem, tInvLocDest);
    if (bSuccess)
    {
        sUpdatePlayerDataIfNecessary(tACDPlayer.id);
        SQuestsOnPlayerAcquireItem(tACDPlayer.id, tACDItem.id);
    }

    return bSuccess;
}

//@ 627
BOOL PlayerBrainTryInventoryQuickMoveItem(ActorCommonData & tACDPlayer, ActorCommonData & tACDItem, InventoryLocation & tInvLocDest, const int32 * pnSlotYMinMax)




{
    if (tACDItem.IsInInventory())
    {
        InventoryLocation & tInvLocCurrent = tACDItem.GetInventoryLocation();



        if (tInvLocCurrent.eSlot == tInvLocDest.eSlot)
        {
            return FALSE;
        }
    }



    BOOL bMovedSomething = FALSE;
    InventoryLocation tInvLoc;


    while (ACDInventoryFindNonFullStackInSlot(tACDItem, tACDPlayer, tInvLocDest.eSlot, tInvLoc, pnSlotYMinMax))
    {
        BOOL bSuccess = PlayerBrainTryInventoryStackItem(tACDPlayer, tACDItem, tInvLoc);

        if (!bSuccess)
        {
            break;
        }

        bMovedSomething = TRUE;



        if (tACDItem.id == ACDID_INVALID)
        {
            return TRUE;
        }
    }



    IVector2D tPoint(0, 0);
    if (!ACDInventoryCanHold(tACDPlayer, tACDItem, tInvLocDest.eSlot, tPoint, pnSlotYMinMax))
    {
        if (!bMovedSomething)
        {

            PlayerDisplayErrorMessage(tACDPlayer.id, GAMEERROR_IARESULT_NOT_ENOUGH_ROOM);
        }
        return bMovedSomething;
    }

    ACDInventoryLocationSet(tInvLocDest, tACDPlayer.id, tInvLocDest.eSlot, tPoint.x, tPoint.y);


    BOOL bSuccess = SACDInventoryProcessMoveRequest(tACDPlayer, tACDItem, tInvLocDest);
    if (bSuccess)
    {
        sUpdatePlayerDataIfNecessary(tACDPlayer.id);
        SQuestsOnPlayerAcquireItem(tACDPlayer.id, tACDItem.id);
    }

    return bSuccess;
}

//@ 694
BOOL PlayerBrainTryInventoryStackItem(ActorCommonData & tACDPlayer, ActorCommonData & tACDItem, const InventoryLocation & tInvLocDest)



{

    ACDID idACDSquareItem = ACDInventoryGetSquareItem(tACDPlayer, tInvLocDest.eSlot, tInvLocDest.pt);
    ActorCommonData * ptACDSquareItem = ACDTryToGet(idACDSquareItem);
    if (ptACDSquareItem == NULL)
    {
        return FALSE;
    }
    BOOL bSuccess = ACDInventoryStackItems(tACDItem, *ptACDSquareItem);
    sUpdatePlayerDataIfNecessary(tACDPlayer.id);
    SQuestsOnPlayerAcquireItem(tACDPlayer.id, tACDItem.id);

    return bSuccess;
}

//@ 716
void PlayerBrainTryInventoryUseItem(const Player & tPlayer, const ACDID idACDItem, const Target & tTarget)



{

    if (ACDTryToGet(idACDItem) == NULL)
    {
        return;
    }



    SACDInventoryProcessUseRequest(tPlayer.idPlayerACD, idACDItem, &tTarget);
}

//@ 740
static void sNotifyPlayerOfAutoAssignment(const ACDID idACDPlayer, const SkillButton eSkillButton)
{


    IntDataMessage tMessage;
    tMessage.nData = eSkillButton;

    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetSingle(listDestPlayers, idACDPlayer);

    MessageSendToClient(listDestPlayers, SMSG_AUTOASSIGNED_TO_BUTTON, &tMessage, sizeof(tMessage));
}

//@ 756
BOOL PlayerBrainChangeHotbarButtons(Player & tPlayer, const SkillButton eSkillButton, const HotbarButtonData & tButtonData)



{
    ActorCommonData * ptACD = ACDTryToGet(tPlayer.idPlayerACD);
    if (ptACD == NULL)
    {
        return FALSE;
    }

    if (tButtonData.m_snoPower != SNO_NONE)
    {

        int32 nPointsInvested = ptACD->AttributesGetInt(MakeAttribKey(ATTRIBUTE_SKILL, tButtonData.m_snoPower));
        if (nPointsInvested < 1)
        {
            return FALSE;
        }
    }

    PlayerSetHotbarBtnAssignment(&tPlayer, eSkillButton, tButtonData);
    if (tButtonData.m_snoPower != SNO_NONE)
    {
        SPlayerSkillSlotMarkAsAssigned(tPlayer, eSkillButton);
    }
    sNotifyPlayerOfAutoAssignment(tPlayer.idPlayerACD, eSkillButton);

    return TRUE;
}

//@ 790
void PlayerBrainInitialize(ActorID idActor)
{

    ActorAI & tAI = ActorGetAI(idActor);
    tAI.nUpdateInterval = 1;
}

//@ 800
void PlayerBrainDispose(ActorID idActor)
{


}
