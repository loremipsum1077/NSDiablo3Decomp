// Original file: X/Server/STrade.cpp
#include "./Crafting/SItemCrafting.h"
#include "./PlayerList.h"
#include "./Powers/PowerAPI.h"
#include "./SGameStats.h"
#include "./SGameStatsSwig.h"
#include "./Network/KnownList.h"
#include "../Common/GameCommonData.h"
#include "../Common/ActorCommonData.h"
#include "../Common/ActorInventory.h"
#include "../Common/Attributes/FastAttribConstantsSwig.h"
#include "../Common/Attributes/FastAttributeStructs.h"
#include "../Common/Game.h"
#include "../Common/Items.h"
#include "../Common/ItemsSwig.h"
#include "../Common/Networking/GameMessage.h"
#include "../Common/Player.h"
#include "../Common/Trade.h"
#include "../Common/WorldPlace.h"
#include "../Sigma/DataStructures/XVar.h"
#include "../Sigma/Memory/SigmaMemory.h"
#include "../Sigma/MemoryPools.h"

void PlayerClearPotionBtnForMissingItem(Player * ptPlayer, const ACDID idACDItem);  // X/Common/Player.cpp:1735
void ACDInventoryRemoveToLimbo(const ACDID idACDContainer, ActorCommonData & tACDItem, BOOL bRemoveFromKnownList);  // X/Common/ActorInventory.cpp:3228
Player * TradeGetTradingTarget(const Player & tPlayer);  // X/Common/Trade.cpp:98
void ResetTradeSessionData(Player * ptPlayer);  // X/Common/Trade.cpp:225
BOOL CValidateTradeContentsStillInInventory(const Player & tPlayer);  // X/Server/Network/ClientValidation.cpp:1189
void TradeOfferGetList(const Player & tPlayer, XPooledDataIDList & listOffer);  // X/Common/Trade.cpp:112
TradeResponse TradeCanTradeWith(const Player & tFirst, const Player * ptSecond);  // X/Common/Trade.cpp (_Z17TradeCanTradeWithRK6PlayerPS0_)
BOOL TradeOfferCanAccept(const Player & tPlayer, const XPooledDataIDList & listOtherOffer);  // X/Common/Trade.cpp:164
static XVarScope s_scopeTrade("Trade", NULL);
static XVarUint64 s_varTradeGoldLogThreshold(s_scopeTrade, "GoldLogThreshold", 0);
static XVarBool s_varGoldTradeDisabled(s_scopeTrade, "GoldTradeDisabled", false);
static XVarBool s_varTradeDisabled(s_scopeTrade, "TradeDisabled", false);

enum TradeCompleteResult {
    TRADE_COMPLETE_NOT_READY = 0,
    TRADE_COMPLETE_MISMATCH = 1,
    TRADE_COMPLETE_SUCCESS = 2,
    TRADE_DISABLED = 3,
    TRADE_OF_GOLD_DISABLED = 4,
    TRADE_COMPLETE_COUNT = 5,
};

BOOL CValidateItemIsInPlayerInventory(const Player & tPlayer, const ACDID idACDItem, BOOL bExcludeHirelingSlots);  // X/Server/Network/ClientValidation.cpp:873
void ServerQueueClientUpdate();  // X/Server/Network/Server.cpp:438
BOOL ItemIsTradeable(const Player & tOwner, const Player * ptPlayerDest, const ActorCommonData & tACDItem, const BOOL bCheckSockets);  // X/Common/Items.cpp:9280
void sCompleteTrade(Player & tPlayerOne, Player & tPlayerTwo);
void sSendTradeState(Player & tPlayerFirst);
void sServerSendTradeMessage(Player & tPlayerTo, TradeMessage & tMessage);
void STradeTriggerUpdate(Player & tPlayer, BOOL bCancelAccepts);
void STradeCancel(ACDID idACDPlayer);


















void sSocketTradeDirtyIterateCB(ACDID idACDSquare, InventorySlotType, void * pUserData)
{




    ActorCommonData * ptACDItem = ACDTryToGet(idACDSquare);
    if (ptACDItem == NULL)
    {
        return;
    }
    SACDInventoryAddToDirtyList(*ptACDItem, AIDA_UPDATE_LIMBO, SLOT_ITEM_SOCKETS, -1);
}

void sTradeMoveListToLimbo(XPooledDataIDList & tListItems)
{

    POS pos = tListItems.GetHeadPosition();
    while (pos)
    {
        ACDID idACDItem = (ACDID)tListItems.GetNext(pos);

        ActorCommonData & tACDItem = ACDGet(idACDItem);
        SACDInventoryAddToDirtyList(tACDItem, AIDA_UPDATE_LIMBO, tACDItem.tLocation.tInvLocation.eSlot, -1);
        if (tACDItem.HasInventory() && ItemHasSockets(tACDItem))
        {
            ACDInventoryIterateItems(tACDItem, SLOT_ITEM_SOCKETS, 0, sSocketTradeDirtyIterateCB, NULL);
        }

        const InventoryLocation * ptInvLocation = tACDItem.TryToGetInventoryLocation();
        if (ptInvLocation)
        {
            InventorySlotType eSlot = ptInvLocation->eSlot;

            Player * ptPlayer = PlayerGetByACD(ptInvLocation->idACDContainer);
            if (ptPlayer)
            {
                PlayerClearPotionBtnForMissingItem(ptPlayer, tACDItem.id);
            }
            if ((eSlot >= SLOT_PLAYER_HEAD && eSlot < SLOT_PLAYER_SHARED_STASH) ||
                (eSlot >= SLOT_HIRELING_START && eSlot <= SLOT_HIRELING_END))
            {

                PowerAPI_BuffApplyOnEquipmentChangeEffects(ptInvLocation->idACDContainer, eSlot, 1);
            }

            ACDInventoryRemoveToLimbo(ptInvLocation->idACDContainer, tACDItem, TRUE);
        }
    }
}


TradeCompleteResult sTradeTryComplete(Player & tPlayer)
{
    if (!tPlayer.tTradeData.bTradeAccepted)
    {
        return TRADE_COMPLETE_NOT_READY;
    }


    Player * ptOther = TradeGetTradingTarget(tPlayer);
    if (ptOther == NULL || !ptOther->tTradeData.bTradeAccepted)
    {
        return TRADE_COMPLETE_NOT_READY;
    }

    if (s_varTradeDisabled.Get())
    {
        return TRADE_DISABLED;
    }


    if (GameIsSeasonal() || s_varGoldTradeDisabled.Get())
    {
        if (tPlayer.tTradeData.uMyGoldOffer != 0 ||
            ptOther->tTradeData.uMyGoldOffer != 0)
        {
            return TRADE_OF_GOLD_DISABLED;
        }
    }
    if (tPlayer.tTradeData.uAcceptedHashValue != ptOther->tTradeData.uAcceptedHashValue)
    {
        return TRADE_COMPLETE_MISMATCH;
    }


    sCompleteTrade(tPlayer, *ptOther);
    return TRADE_COMPLETE_SUCCESS;
}













void sTransferItems(XPooledDataIDList & tListItems, Player & tReceivingPlayer)
{


    if (tListItems.IsEmpty())
    {
        return;
    }

    ActorCommonData & tACDPlayer = ACDGet(tReceivingPlayer.idPlayerACD);

    POS pos = tListItems.GetHeadPosition();
    while (pos)
    {

        ACDID idACDItem = (ACDID)tListItems.GetNext(pos);
        ActorCommonData * ptACDItem = ACDTryToGet(idACDItem);
        if (ptACDItem)
        {
            SGameStatsPlayerCounterIncrement(&tReceivingPlayer, STAT_COUNTER_PLAYER_ITEM_RECEIVED_IN_TRADE, ptACDItem->hGB.gbid, 0, 1.0f);

            ptACDItem->AttributesSetInt(FastAttribKey(ATTRIBUTE_ITEM_FLAGGED_AS_JUNK), 0);
            SACDInventoryPickupOrSpillOntoGround(tACDPlayer, *ptACDItem, FALSE, NULL, TRUE);
        }
    }
}


void sSendTradeState(Player & tPlayerFirst)
{

    PlayerList listDestPlayers(PlayerListGetElementPool());
    listDestPlayers.AddTail_Tracked(&tPlayerFirst);

    Player * ptOther = TradeGetTradingTarget(tPlayerFirst);
    if (ptOther)
    {
        listDestPlayers.AddTail_Tracked(ptOther);
    }
    {


        TradeMessage tMessage;
        tMessage.eMessageType = TRADE_MSG_STATE;
        tMessage.ann = ANNFromACD(tPlayerFirst.idPlayerACD);
        SigmaMemoryCopy(tMessage.arrItems, tPlayerFirst.tTradeData.arrItems, sizeof(tMessage.arrItems));
        tMessage.uData64 = tPlayerFirst.tTradeData.uMyGoldOffer;
        tMessage.dwData2 = tPlayerFirst.tTradeData.bTradeAccepted;
        MessageSendToClient(listDestPlayers, SMSG_TRADE, &tMessage, sizeof(tMessage));
    }
    if (ptOther)
    {



        TradeMessage tMessage;
        tMessage.eMessageType = TRADE_MSG_STATE;
        tMessage.ann = ANNFromACD(ptOther->idPlayerACD);
        SigmaMemoryCopy(tMessage.arrItems, ptOther->tTradeData.arrItems, sizeof(tMessage.arrItems));
        tMessage.uData64 = ptOther->tTradeData.uMyGoldOffer;
        tMessage.dwData2 = ptOther->tTradeData.bTradeAccepted;
        MessageSendToClient(listDestPlayers, SMSG_TRADE, &tMessage, sizeof(tMessage));
    }
}


void sServerSendTradeMessage(Player & tPlayerTo, TradeMessage & tMessage)
{


    PlayerList listDestPlayers(PlayerListGetElementPool());
    listDestPlayers.AddTail_Tracked(&tPlayerTo);
    MessageSendToClient(listDestPlayers, SMSG_TRADE, &tMessage, sizeof(tMessage));
}


void sCompleteTrade(Player & tPlayerOne, Player & tPlayerTwo)
{


    ActorCommonData * ptACDPlayerOne = ACDTryToGet(tPlayerOne.idPlayerACD);
    ActorCommonData * ptACDPlayerTwo = ACDTryToGet(tPlayerTwo.idPlayerACD);
    if (ptACDPlayerOne == NULL || ptACDPlayerTwo == NULL)
    {
        return;
    }

    if (!CValidateTradeContentsStillInInventory(tPlayerOne))
    {
        STradeCancel(tPlayerOne.idPlayerACD);
        return;
    }
    if (!CValidateTradeContentsStillInInventory(tPlayerTwo))
    {
        STradeCancel(tPlayerTwo.idPlayerACD);
        return;
    }




    XPooledDataIDList listPlayerOneOffer(MemoryPoolsGetDATAIDListPool());
    TradeOfferGetList(tPlayerOne, listPlayerOneOffer);

    XPooledDataIDList listPlayerTwoOffer(MemoryPoolsGetDATAIDListPool());
    TradeOfferGetList(tPlayerTwo, listPlayerTwoOffer);


    int64 nGoldOfferOne = tPlayerOne.tTradeData.uMyGoldOffer;
    CurrencyType eCurrencyType = CURRENCY_GOLD;
    int64 nGoldOfferTwo = tPlayerTwo.tTradeData.uMyGoldOffer;
    uint64 uOrigAmountOne = ptACDPlayerOne->GetCurrencyAmount(eCurrencyType);
    uint64 uOrigAmountTwo = ptACDPlayerTwo->GetCurrencyAmount(eCurrencyType);

    BOOL bGoldTradeSuccess =
        ptACDPlayerOne->ModifyCurrencyAmount(-nGoldOfferOne, eCurrencyType, GMR_TRADE, FALSE) &&
        ptACDPlayerTwo->ModifyCurrencyAmount(-nGoldOfferTwo, eCurrencyType, GMR_TRADE, FALSE) &&
        ptACDPlayerOne->ModifyCurrencyAmount(nGoldOfferTwo, eCurrencyType, GMR_TRADE, FALSE) &&
        ptACDPlayerTwo->ModifyCurrencyAmount(nGoldOfferOne, eCurrencyType, GMR_TRADE, FALSE);
    if (!bGoldTradeSuccess)
    {





        ptACDPlayerOne->SetCurrencyAmount(uOrigAmountOne, eCurrencyType, GMR_TRADE_FAILED);
        ptACDPlayerTwo->SetCurrencyAmount(uOrigAmountTwo, eCurrencyType, GMR_TRADE_FAILED);
        STradeCancel(ptACDPlayerOne->id);
        STradeCancel(ptACDPlayerTwo->id);
        return;
    }


    sTradeMoveListToLimbo(listPlayerOneOffer);
    sTradeMoveListToLimbo(listPlayerTwoOffer);


    SGameStatsPlayerCounterIncrement(&tPlayerOne, STAT_COUNTER_PLAYER_CURRENCY_SPENT_TRADING, 0, 0, (float)nGoldOfferOne);
    SGameStatsPlayerCounterIncrement(&tPlayerTwo, STAT_COUNTER_PLAYER_CURRENCY_SPENT_TRADING, 0, 0, (float)nGoldOfferTwo);


    if (!listPlayerTwoOffer.IsEmpty() || !listPlayerOneOffer.IsEmpty())
    {
        sTransferItems(listPlayerOneOffer, tPlayerTwo);
        sTransferItems(listPlayerTwoOffer, tPlayerOne);
    }



















































    ResetTradeSessionData(&tPlayerOne);
    ResetTradeSessionData(&tPlayerTwo);

    TradeMessage tMessage;
    tMessage.eMessageType = TRADE_MSG_COMPLETE;
    tMessage.dwData = ANNFromACD(ptACDPlayerOne->id);
    tMessage.dwData2 = tPlayerTwo.nPlayerIndex;
    sServerSendTradeMessage(tPlayerOne, tMessage);
    tMessage.dwData = ANNFromACD(ptACDPlayerTwo->id);
    tMessage.dwData2 = tPlayerOne.nPlayerIndex;
    sServerSendTradeMessage(tPlayerTwo, tMessage);
}
static const int32 RESET_ACCEPT_MIN_TICKS = 60;


static void sResetAcceptTime(Player & tPlayer)
{
    tPlayer.tTradeData.uNextAcceptTime = GameGetServerTime() + RESET_ACCEPT_MIN_TICKS;
}



void STradeStart(const ACDID idACDPlayer1, const ACDID idACDPlayer2)
{
    Player * ptPlayer1 = PlayerGetByACD(idACDPlayer1);
    Player * ptPlayer2 = PlayerGetByACD(idACDPlayer2);
    if (ptPlayer1 == NULL || ptPlayer2 == NULL)
    {
        return;
    }
    ResetTradeSessionData(ptPlayer1);
    ResetTradeSessionData(ptPlayer2);
    ptPlayer2->tTradeData.idACDPlayerTradingWith = idACDPlayer1;
    ptPlayer1->tTradeData.idACDPlayerTradingWith = idACDPlayer2;
    ptPlayer1->tTradeData.bTradeAccepted = FALSE;
    ptPlayer1->tTradeData.uMyGoldOffer = 0;
    ptPlayer2->tTradeData.bTradeAccepted = FALSE;
    ptPlayer2->tTradeData.uMyGoldOffer = 0;
    sResetAcceptTime(*ptPlayer1);
    sResetAcceptTime(*ptPlayer2);
}












static void sCompactItemList(ACDNetworkName * arrItems)
{
    ACDNetworkName arrTemp[4];
    int32 nIndex = 0;
    for (int32 i = 0; i < 4; i++)
    {
        if (arrItems[i] != ANN_INVALID)
        {
            arrTemp[nIndex++] = arrItems[i];
        }
    }
    for (; nIndex < 4; nIndex++)
    {
        arrTemp[nIndex] = ANN_INVALID;
    }
    SigmaMemoryCopy(arrItems, arrTemp, sizeof(arrTemp));
}
void STradeOtherPlayerBusy(ACDID idACDPlayer);





// Case order from the DWARF: STradeOtherPlayerBusy is inlined at 605 (between CANCEL and ACCEPT), not after GOLD;
// with the case there the TRY_START tail no longer merges with the inlined Busy call.

void STradeHandleMessage(Player & tPlayerFrom, TradeMessage & tMessage)
{


    TradeMessage tForwardMessage;
    switch (tMessage.eMessageType)
    {
    case TRADE_MSG_TRY_START:
        {
            tForwardMessage.eMessageType = TRADE_MSG_TRY_START;


            tForwardMessage.dwData = ANNFromACD(tPlayerFrom.idPlayerACD);




            ACDNetworkName annTargetPlayer = (ACDNetworkName)tMessage.dwData;
            Player * ptPlayer = PlayerGetByACD(ACDFromANN(annTargetPlayer));
            if (ptPlayer == NULL)
            {
                STradeCancel(tPlayerFrom.idPlayerACD);
                return;
            }





            TradeResponse eResponse = TradeCanTradeWith(tPlayerFrom, ptPlayer);
            if (eResponse != TRADE_RESPONSE_OK)
            {
                STradeCancel(tPlayerFrom.idPlayerACD);
                if (eResponse == TRADE_RESPONSE_BUSY)
                {

                    PlayerDisplayErrorMessage(tPlayerFrom.idPlayerACD, GAMEERROR_TRADE_OTHER_BUSY);
                }
                return;
            }


            ResetTradeSessionData(&tPlayerFrom);
            ResetTradeSessionData(ptPlayer);
            ptPlayer->tTradeData.idACDPlayerWantToTradeWith = tPlayerFrom.idPlayerACD;
            tPlayerFrom.tTradeData.idACDPlayerWantToTradeWith = ptPlayer->idPlayerACD;

            sServerSendTradeMessage(*ptPlayer, tForwardMessage);
        }
        break;
    case TRADE_MSG_ITEM:
        {


            uint32 nItemIndex = tMessage.dwData;
            ACDNetworkName annItem = (ACDNetworkName)tMessage.dwData2;
            ActorCommonData * ptACDItem = ACDTryToGet(ACDFromANN(annItem));
            if (ptACDItem == NULL)
            {
                STradeCancel(tPlayerFrom.idPlayerACD);
                return;
            }

            Player * ptPlayer = TradeGetTradingTarget(tPlayerFrom);
            if (ptPlayer == NULL)
            {
                STradeCancel(tPlayerFrom.idPlayerACD);
                return;
            }
            if (nItemIndex >= 4)
            {
                STradeCancel(tPlayerFrom.idPlayerACD);
                return;
            }
            for (int32 i = 0; i < 4; i++)
            {
                if (tPlayerFrom.tTradeData.arrItems[i] == annItem)
                {
                    tPlayerFrom.tTradeData.arrItems[i] = ANN_INVALID;
                }
            }

            Player * ptPlayerTo = PlayerGetByACD(tPlayerFrom.tTradeData.idACDPlayerTradingWith);

            if (!ItemIsTradeable(tPlayerFrom, ptPlayerTo, *ptACDItem, TRUE) || !CValidateItemIsInPlayerInventory(tPlayerFrom, ptACDItem->id, TRUE))
            {
                STradeCancel(tPlayerFrom.idPlayerACD);
                return;
            }
            sResetAcceptTime(tPlayerFrom);
            tPlayerFrom.tTradeData.arrItems[nItemIndex] = annItem;
            tPlayerFrom.tTradeData.bTradeAccepted = FALSE;
            ptPlayer->tTradeData.bTradeAccepted = FALSE;
            KnownListItemAdded(KET_ACD, ptACDItem->id);
            ServerQueueClientUpdate();
            sSendTradeState(tPlayerFrom);
        }
        break;
    case TRADE_MSG_ITEM_REMOVED:
        {













            Player * ptPlayer = TradeGetTradingTarget(tPlayerFrom);
            if (ptPlayer == NULL)
            {
                STradeCancel(tPlayerFrom.idPlayerACD);
                return;
            }

            ACDNetworkName annItem = tMessage.ann;






            for (int32 i = 0; i < 4; i++)
            {
                if (tPlayerFrom.tTradeData.arrItems[i] == annItem)
                {
                    tPlayerFrom.tTradeData.arrItems[i] = ANN_INVALID;


                    sCompactItemList(tPlayerFrom.tTradeData.arrItems);
                    tPlayerFrom.tTradeData.bTradeAccepted = FALSE;
                    ptPlayer->tTradeData.bTradeAccepted = FALSE;

                    sResetAcceptTime(tPlayerFrom);
                }
            }
            sSendTradeState(tPlayerFrom);
        }
        break;
    case TRADE_MSG_CANCEL:
        {
            STradeCancel(tPlayerFrom.idPlayerACD);
        }
        break;



    case TRADE_MSG_OTHER_BUSY:

        STradeOtherPlayerBusy(tPlayerFrom.idPlayerACD);
        break;

    case TRADE_MSG_ACCEPT:
        {

            Player * ptPlayer = TradeGetTradingTarget(tPlayerFrom);
            if (ptPlayer == NULL)
            {
                STradeCancel(tPlayerFrom.idPlayerACD);
                return;
            }
            if (!CValidateTradeContentsStillInInventory(*ptPlayer))
            {
                STradeCancel(tPlayerFrom.idPlayerACD);
                return;
            }
            if (GameGetServerTime() < tPlayerFrom.tTradeData.uNextAcceptTime)
            {
                sSendTradeState(tPlayerFrom);
                return;
            }





            XPooledDataIDList listOffer(MemoryPoolsGetDATAIDListPool());
            TradeOfferGetList(*ptPlayer, listOffer);
            tPlayerFrom.tTradeData.bTradeAccepted = TradeOfferCanAccept(tPlayerFrom, listOffer);
            tPlayerFrom.tTradeData.uAcceptedHashValue = tMessage.dwData;

            TradeCompleteResult eResult = sTradeTryComplete(tPlayerFrom);
            switch (eResult)
            {
            case TRADE_DISABLED:
                STradeCancel(tPlayerFrom.idPlayerACD);
                PlayerDisplayErrorMessage(tPlayerFrom.idPlayerACD, GAMEERROR_TRADE_DISABLED);
                PlayerDisplayErrorMessage(ptPlayer->idPlayerACD, GAMEERROR_TRADE_DISABLED);
                break;
            case TRADE_OF_GOLD_DISABLED:



                STradeCancel(tPlayerFrom.idPlayerACD);
                PlayerDisplayErrorMessage(tPlayerFrom.idPlayerACD, GAMEERROR_TRADE_OF_GOLD_DISABLED);
                PlayerDisplayErrorMessage(ptPlayer->idPlayerACD, GAMEERROR_TRADE_OF_GOLD_DISABLED);
                break;
            case TRADE_COMPLETE_MISMATCH:








                STradeCancel(tPlayerFrom.idPlayerACD);
                break;
            case TRADE_COMPLETE_SUCCESS:
                break;
            default:

















                sSendTradeState(*ptPlayer);
                sSendTradeState(tPlayerFrom);
                break;
            }
        }
        break;
    case TRADE_MSG_START:
        {
            Player * ptPlayer = PlayerGetByACD(tPlayerFrom.tTradeData.idACDPlayerWantToTradeWith);
            if (ptPlayer == NULL)
            {
                STradeCancel(tPlayerFrom.idPlayerACD);
                return;
            }

            STradeStart(tPlayerFrom.idPlayerACD, ptPlayer->idPlayerACD);
            sSendTradeState(tPlayerFrom);
        }
        break;
    case TRADE_MSG_GOLD:
        {






            ActorCommonData & tACDPlayerFrom = ACDGet(tPlayerFrom.idPlayerACD);










            int64 nGoldOfferSigned = tPlayerFrom.tTradeData.uMyGoldOffer = tMessage.uData64;
            if (nGoldOfferSigned > tACDPlayerFrom.GetCurrencyAmount(CURRENCY_GOLD))
            {
                STradeCancel(tPlayerFrom.idPlayerACD);
                return;
            }
            if ((int64)tPlayerFrom.tTradeData.uMyGoldOffer < 0)
            {
                STradeCancel(tPlayerFrom.idPlayerACD);
                return;
            }
            tPlayerFrom.tTradeData.bTradeAccepted = FALSE;
            Player * ptPlayer = TradeGetTradingTarget(tPlayerFrom);
            if (ptPlayer == NULL)
            {
                STradeCancel(tPlayerFrom.idPlayerACD);
                return;
            }
            ptPlayer->tTradeData.bTradeAccepted = FALSE;





            sResetAcceptTime(tPlayerFrom);
            sSendTradeState(tPlayerFrom);
        }
        break;
    default:
        break;
    }
}


void STradeCancel(ACDID idACDPlayer)
{

    Player * ptPlayer = PlayerGetByACD(idACDPlayer);
    if (ptPlayer == NULL)
    {
        return;
    }

    Player * ptOtherPlayer = TradeGetTradingTarget(*ptPlayer);

    TradeMessage tMessage;
    tMessage.eMessageType = TRADE_MSG_CANCEL;
    tMessage.dwData = ANNFromACD(ptPlayer->idPlayerACD);
    sServerSendTradeMessage(*ptPlayer, tMessage);

    ResetTradeSessionData(ptPlayer);
    if (ptOtherPlayer)
    {








        ResetTradeSessionData(ptOtherPlayer);

        tMessage.dwData = ANNFromACD(ptOtherPlayer->idPlayerACD);
        sServerSendTradeMessage(*ptOtherPlayer, tMessage);
    }
}

void STradeOtherPlayerBusy(ACDID idACDPlayer)
{
    Player * ptPlayer = PlayerGetByACD(idACDPlayer);
    if (ptPlayer == NULL)
    {
        return;
    }
    Player * ptOtherPlayer = TradeGetTradingTarget(*ptPlayer);
    TradeMessage tMessage;
    tMessage.eMessageType = TRADE_MSG_OTHER_BUSY;
    ResetTradeSessionData(ptPlayer);
    if (ptOtherPlayer)
    {
        ResetTradeSessionData(ptOtherPlayer);
        tMessage.dwData = ANNFromACD(ptOtherPlayer->idPlayerACD);
        sServerSendTradeMessage(*ptOtherPlayer, tMessage);
    }
}














void STradeTriggerUpdate(Player & tPlayer, BOOL bCancelAccepts)
{
    Player * ptOther = TradeGetTradingTarget(tPlayer);
    if (ptOther == NULL)
    {
        return;
    }
    if (bCancelAccepts)
    {
        ptOther->tTradeData.bTradeAccepted = FALSE;
        tPlayer.tTradeData.bTradeAccepted = FALSE;
    }
    sSendTradeState(tPlayer);
}








void STradeCheckForModified(const ActorCommonData & tACDItem, BOOL bRemoveItem)
{


    ACDID idACDPlayer = tACDItem.GetInventoryContainerOwner();
    ActorCommonData * ptACDPlayer = ACDTryToGet(idACDPlayer);
    if (ptACDPlayer)
    {
        Player * ptPlayer = PlayerGetByACD(idACDPlayer);
        if (ptPlayer)
        {
            for (int32 i = 0; i < 4; i++)
            {
                ACDNetworkName idANNThisItem = ptPlayer->tTradeData.arrItems[i];
                ACDID idACDThisItem = ACDFromANN(idANNThisItem);
                if (idACDThisItem == tACDItem.id)
                {
                    Player * ptPlayerTo = PlayerGetByACD(ptPlayer->tTradeData.idACDPlayerTradingWith);
                    if (bRemoveItem)
                    {
                        ptPlayer->tTradeData.arrItems[i] = ANN_INVALID;


                        sCompactItemList(ptPlayer->tTradeData.arrItems);
                    }
                    else
                    {

                        if (!ItemIsTradeable(*ptPlayer, ptPlayerTo, tACDItem, TRUE))
                        {
                            STradeCancel(idACDPlayer);
                            return;
                        }
                    }
                    STradeTriggerUpdate(*ptPlayer, TRUE);
                    return;
                }
            }
        }
    }
}



void STradeRemoveItemFromTrade(const ActorCommonData & tACDItem)
{

    ACDID idACDContainer = tACDItem.GetInventoryContainerOwner();
    ActorCommonData * ptACDContainer = ACDTryToGet(idACDContainer);
    if (ptACDContainer && idACDContainer != tACDItem.id)
    {
        if (ptACDContainer->IsItem())
        {
            STradeRemoveItemFromTrade(*ptACDContainer);
            return;
        }

        Player * ptPlayer = PlayerGetByACD(idACDContainer);
        if (ptPlayer)
        {
            for (int32 i = 0; i < 4; i++)
            {
                ACDNetworkName idANNThisItem = ptPlayer->tTradeData.arrItems[i];
                ACDID idACDThisItem = ACDFromANN(idANNThisItem);
                if (idACDThisItem == tACDItem.id)
                {
                    ptPlayer->tTradeData.arrItems[i] = ANN_INVALID;

                    sCompactItemList(ptPlayer->tTradeData.arrItems);

                    STradeTriggerUpdate(*ptPlayer, TRUE);
                }
            }
        }
    }
}
