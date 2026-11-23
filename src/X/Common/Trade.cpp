// Original file: X/Common/Trade.cpp

#include "Trade.h"
#include "ActorCommonData.h"
#include "ActorInventory.h"
#include "Game.h"
#include "GameCommonData.h"
#include "Definitions/GameBalanceDefinition.h"
#include "ItemsSwig.h"
#include "Items.h"
#include "Player.h"
#include "WorldPlace.h"
#include "../Sigma/DataStructures/DataArraysTemplated.h"
#include "../Sigma/Math/Vector.h"
#include "../Sigma/MemoryPools.h"

BOOL ACDInventoryCanHold(const ActorCommonData & tACD, const ActorCommonData & tACDItem, const InventorySlotType eSlot, IVector2D & tWhereCanHold, const int32 * pnSlotYMinMax);  // X/Common/ActorInventory.cpp:869
BOOL PlayerPendingRiftJoin(const Player * ptPlayer);  // X/Common/Player.cpp:2080
TradeResponse TradeCanTradeWith(const Player & tFirst, const Player * ptSecond);

namespace {




void sResetTrade(Player & tPlayer)
{

    tPlayer.tTradeData.bTradeAccepted = FALSE;
    tPlayer.tTradeData.idACDPlayerTradingWith = ACDID_INVALID;
    tPlayer.tTradeData.idACDPlayerWantToTradeWith = ACDID_INVALID;
    tPlayer.tTradeData.uMyGoldOffer = 0;
    tPlayer.tTradeData.uAcceptedHashValue = 0;

    for (int32 i = 0; i < 4; i++)
    {
        tPlayer.tTradeData.arrItems[i] = ANN_INVALID;
    }
}








BOOL sTradeCheckOfferSlot(const ActorCommonData & tACDItem, XPooledDataIDList * listOffer)
{

    BOOL bIsLargeItem = ItemIsLargeSlotItem(tACDItem.hGB.gbid);
    POS pos = listOffer->GetHeadPosition();
    while (pos)
    {
        POS oldPos = pos;
        ACDID idACDItem = (ACDID)listOffer->GetNext(pos);
        ActorCommonData * ptACDItem = ACDTryToGet(idACDItem);
        if (!ptACDItem)
        {
            continue;
        }


        if (ACDInventoryItemAllowedInSlot(tACDItem, ptACDItem->GetInventoryLocation(), FALSE, FALSE) != GAMEERROR_NONE)
        {
            continue;
        }

        if (ItemGetBindingLevel(tACDItem) != IBL_NONE &&
            ItemIsEquipped(*ptACDItem))
        {
            continue;
        }

        BOOL bOtherIsLarge = ItemIsLargeSlotItem(ptACDItem->hGB.gbid);

        if ((bIsLargeItem && bOtherIsLarge) ||
            (!bIsLargeItem && !bOtherIsLarge))
        {
            listOffer->RemoveAt(oldPos);
            return TRUE;
        }
    }

    if (!bIsLargeItem && listOffer->GetHeadPosition())
    {
        listOffer->RemoveHead();
        return TRUE;
    }

    return FALSE;
}

}  // namespace



Player * TradeGetTradingTarget(const Player & tPlayer)
{

    Player * ptPlayer = PlayerGetByACD(tPlayer.tTradeData.idACDPlayerTradingWith);
    if (ptPlayer == NULL)
    {
        ptPlayer = PlayerGetByACD(tPlayer.tTradeData.idACDPlayerWantToTradeWith);
    }

    return ptPlayer;
}



void TradeOfferGetList(const Player & tPlayer, XPooledDataIDList & listOffer)
{



    for (int32 i = 0; i < 4; i++)
    {
        ACDID idACDItem = ACDFromANN(tPlayer.tTradeData.arrItems[i]);
        if (ACDTryToGet(idACDItem))
        {
            listOffer.AddTail_Tracked(idACDItem);
        }
    }
}





BOOL TradeCanTakeItem(ActorCommonData & tACDPlayer, ActorCommonData & tACDItem, ACDID & idACDContainer, InventorySlotType & eSlot, IVector2D & ptWhere, XPooledDataIDList * listOfferToConsiderForSwaps)
{







    idACDContainer = tACDPlayer.id;
    ptWhere.Set(0, 0);


    eSlot = SLOT_PLAYER_BACKPACK;
    BOOL bCanHold = ACDInventoryCanHold(tACDPlayer, tACDItem, eSlot, ptWhere, NULL);
    if (!bCanHold)
    {
        if (listOfferToConsiderForSwaps)
        {
            bCanHold = sTradeCheckOfferSlot(tACDItem, listOfferToConsiderForSwaps);
        }

        idACDContainer = ACDID_INVALID;
        eSlot = SLOT_NONE;
        ptWhere.Set(0, 0);
    }


    return bCanHold;
}



BOOL TradeOfferCanAccept(const Player & tPlayer, const XPooledDataIDList & listOtherOffer)
{


    ActorCommonData & tACDPlayer = ACDGet(tPlayer.idPlayerACD);

    XPooledDataIDList listPlayerOffer(MemoryPoolsGetDATAIDListPool());
    TradeOfferGetList(tPlayer, listPlayerOffer);


    POS pos = listOtherOffer.GetHeadPosition();
    while (pos)
    {
        ACDID idACDItem = (ACDID)listOtherOffer.GetNext(pos);
        ActorCommonData & tACDItem = ACDGet(idACDItem);

        ACDID idACDContainer;
        InventorySlotType eSlot;
        IVector2D ptWhere;

        BOOL bCanHold = TradeCanTakeItem(tACDPlayer, tACDItem, idACDContainer, eSlot, ptWhere, &listPlayerOffer);
        if (!bCanHold)
        {
            return FALSE;
        }
    }

    return TRUE;





























}



void ResetTradeSessionData(Player * ptPlayer)
{

    if (ptPlayer)
    {



        Player * ptOther = TradeGetTradingTarget(*ptPlayer);

        sResetTrade(*ptPlayer);
        if (ptOther)
        {
            sResetTrade(*ptOther);
        }
    }
}



BOOL TradeItemIsBeingOffered(const Player & tPlayer, const ACDID idACDItem)
{

    if (TradeGetTradingTarget(tPlayer))
    {
        XPooledDataIDList listOffer(MemoryPoolsGetDATAIDListPool());
        TradeOfferGetList(tPlayer, listOffer);

        POS pos = listOffer.GetHeadPosition();
        while (pos)
        {
            ACDID idACD = (ACDID)listOffer.GetNext(pos);
            if (idACD == idACDItem)
            {
                return TRUE;
            }
        }

    }

    return FALSE;
}



BOOL TradeIsTrading(const ACDID idACDPlayer)
{


    Player * ptPlayer = PlayerGetByACD(idACDPlayer);
    if (ptPlayer)
    {
        return TradeGetTradingTarget(*ptPlayer) != NULL;
    }

    return FALSE;
}





BOOL TradeIsTradingWith(const ACDID idACDPlayer, const ACDID idACDTradingWith)
{



    const Player * ptPlayer = PlayerGetByACD(idACDPlayer);
    if (ptPlayer)
    {
        const Player * ptPlayerOther = PlayerGetByACD(idACDTradingWith);
        if (ptPlayerOther)
            return ptPlayerOther->idPlayerACD == ptPlayer->tTradeData.idACDPlayerTradingWith;
    }

    return FALSE;
}






static const float MAX_TRADING_DISTANCE_SQR = 3600.0f;
static BOOL sIsCloseEnoughForTrade(const ActorCommonData & tACDSource, const ActorCommonData & tACDTarget)
{


    WorldPlace placeFirst = tACDSource.GetPlace();
    WorldPlace placeSecond = tACDTarget.GetPlace();
    if (placeFirst.idSWorld != placeSecond.idSWorld)
    {
        return FALSE;
    }

    Vector3D vDiff = placeSecond.wp - placeFirst.wp;
    if (vDiff.MagnitudeSquared() > MAX_TRADING_DISTANCE_SQR)
    {
        return FALSE;
    }

    return TRUE;
}



TradeResponse TradeCanTradeWith(const Player & tFirst, const ACDID idACDTarget)
{


    ActorCommonData * ptACDSource = ACDTryToGet(tFirst.idPlayerACD);
    if (ptACDSource)
    {
        ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
        if (ptACDTarget && ACDIsPlayer(idACDTarget) && ptACDSource != ptACDTarget)
        {





            return TradeCanTradeWith(tFirst, PlayerGetByACD(idACDTarget));
        }
    }


    return TRADE_RESPONSE_ERROR;
}


TradeResponse TradeCanTradeWith(const Player & tFirst, const Player * ptSecond)
{


    ActorCommonData * ptACDFirst = ACDTryToGet(tFirst.idPlayerACD);
    if (!ptACDFirst)
    {
        return TRADE_RESPONSE_ERROR;
    }


    if (TradeIsTrading(ptACDFirst->id))
    {
        return TRADE_RESPONSE_BUSY;
    }

    if (!ptACDFirst->IsAlive())
    {
        return TRADE_RESPONSE_NOT_ALIVE;
    }

    if (tFirst.idACDActiveVendor != ACDID_INVALID)
    {
        return TRADE_RESPONSE_BUSY;
    }

    if (ptACDFirst->IsInCombat())
    {
        return TRADE_RESPONSE_BUSY;
    }

    if (ptACDFirst->IsInConversation())
    {
        return TRADE_RESPONSE_BUSY;
    }

    if (ptACDFirst->AccountIsTradeRestricted())
    {
        return TRADE_RESPONSE_RESTRICTED;
    }

    if (ptACDFirst->IsBusy())
    {
        return TRADE_RESPONSE_BUSY;
    }

    if (PlayerPendingRiftJoin(&tFirst))
    {
        return TRADE_RESPONSE_BUSY;
    }

    if (ptSecond)
    {
        ActorCommonData * ptACDSecond = ACDTryToGet(ptSecond->idPlayerACD);
        if (!ptACDSecond || ptACDFirst == ptACDSecond)
        {
            return TRADE_RESPONSE_ERROR;
        }






        if (!PlayersAreLocalToEachOther(tFirst, *ptSecond))
        {

            if (TradeIsTrading(ptACDSecond->id))
            {
                return TRADE_RESPONSE_BUSY;
            }
        }

        if (ptSecond->idACDActiveVendor != ACDID_INVALID)
        {
            return TRADE_RESPONSE_BUSY;
        }

        if (!ptACDSecond->IsAlive())
        {
            return TRADE_RESPONSE_NOT_ALIVE;
        }

        if (ptACDSecond->IsInCombat())
        {
            return TRADE_RESPONSE_BUSY;
        }

        if (ptACDSecond->IsInConversation())
        {
            return TRADE_RESPONSE_BUSY;
        }

        if (!sIsCloseEnoughForTrade(*ptACDFirst, *ptACDSecond))
        {
            return TRADE_RESPONSE_TOO_FAR;
        }

        if (ptACDSecond->AccountIsTradeRestricted())
        {
            return TRADE_RESPONSE_RESTRICTED;
        }

        if (ptACDSecond->IsBusy())
        {
            return TRADE_RESPONSE_BUSY;
        }

        if (PlayerPendingRiftJoin(ptSecond))
        {
            return TRADE_RESPONSE_BUSY;
        }
    }

    return TRADE_RESPONSE_OK;

}



static const uint32 sguTradeHashStart = 2166136261u;
static const uint32 sguTradeHashFNVPrime = 16777619;
static uint32 sTradeGetHash(const Player & tPlayer)
{

    uint32 uHash = sguTradeHashStart;
    for (int32 i = 0; i < 4; i++)
    {
        ACDNetworkName idANNItem = tPlayer.tTradeData.arrItems[i];
        ACDID idACDItem = ACDFromANN(idANNItem);
        ActorCommonData * ptACDItem = ACDTryToGet(idACDItem);
        if (ptACDItem)
        {

            uint32 dwKey = idANNItem;
            uint8 * bKey = (uint8 *)&dwKey;
            for (int32 j = 0; j < 4; j++)
            {
                uHash ^= bKey[j];
                uHash *= sguTradeHashFNVPrime;
            }


            uint64 uStackCount = ptACDItem->GetStackCount();
            bKey = (uint8 *)&uStackCount;
            for (int32 j = 0; j < 8; j++)
            {
                uHash ^= bKey[j];
                uHash *= sguTradeHashFNVPrime;
            }
        }
    }

    uint8 * bKey = (uint8 *)&tPlayer.tTradeData.uMyGoldOffer;
    for (int32 i = 0; i < 8; i++)
    {
        uHash ^= bKey[i];
        uHash *= sguTradeHashFNVPrime;
    }

    return uHash;
}



uint32 TradeGetHash(const Player & tFirst, const Player & tSecond)
{


    uint32 uFirstHash = sTradeGetHash(tFirst);
    uint32 uSecondHash = sTradeGetHash(tSecond);


    return uFirstHash + uSecondHash;
}
