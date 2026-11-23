#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "../Common/CommonTypes.h"
#include "../Common/GlobalSNO.h"
#include "../Common/Networking/GameMessage.h"
#include "../Common/Player.h"
#include "../Common/Trade.h"
#include "../Sigma/DataStructures/DataArraysTemplated.h"
#include "../Sigma/ErrorManager/ErrorManager.h"
#include "../Sigma/Memory/SigmaMemory.h"
#include "../Sigma/MemoryPools.h"
#include "../Sigma/RefString.h"

// Declared here because their headers are not adopted yet. Each comment names the file that
// defines the function.
// X/Common/Player.h:269 and :280






// X/Common/Player.h



// X/Common/Player.cpp
Player * PlayerGetByANN(const ACDNetworkName ann);
Player * PlayerGetByPlayerIndex(int32 nPlayerIndex);
// X/Common/ActorCommonData.cpp
ACDID ACDFromANN(const ACDNetworkName ann);
// X/Common/Trade.cpp
Player * TradeGetTradingTarget(const Player & tPlayer);
void TradeOfferGetList(const Player & tPlayer, XPooledDataIDList & listOffer);
BOOL TradeOfferCanAccept(const Player & tPlayer, const XPooledDataIDList & listOtherOffer);
void ResetTradeSessionData(Player * ptPlayer);
BOOL TradeIsTrading(const ACDID idACDPlayer);
TradeResponse TradeCanTradeWith(const Player & tFirst, const Player * ptSecond);
uint32 TradeGetHash(const Player & tFirst, const Player & tSecond);
// X/Common/Definitions/StringListDefinition.cpp and X/Common/GlobalSNO.cpp
CRefString StringListFetch(SNO snoStringList, LPCSTR szLabel);
CRefString LocalizedSprintf(LPCSTR szFormat, ...);
SNO GlobalSNOGet(int32 eGlobalSNO);
// X/Client/CPlayer.cpp
CRefString CPlayerGetServiceName(const Player & tPlayer, BOOL fDisplayPound);
BOOL PlayerIsLocal(const Player * ptPlayer);
// X/Client/Network/CGameMessage.cpp
void MessageSendToServer(GameMessageType eType, void * pMessage, uint32 dwMessageSize, const Player * ptPlayer);
// X/Client/Gfx/GfxCommon.h
struct ImageTextureFrame;
extern const ImageTextureFrame cgtImageTextureFrameNull;
// X/Client/UserInterface/Interface/UIGame.cpp
namespace UIGame {
void DisplayError(const GameErrorData & tData, BOOL fDisplayBnetError, const Player * ptPlayer);
}  // namespace UIGame
// X/Client/UserInterface/Interface/UIConversations.cpp
namespace UIConversation {
BOOL ACDIsInConversation(const ACDID idACD, BOOL bConvDialogOnly, BOOL bUnusableOnly);
}  // namespace UIConversation
// X/Client/UserInterface/Console/UIConsoleEventQueue.cpp
namespace UIEventQueue {
namespace Console {
void DisplayLongMessageForPlayer(const CRefString & sMessage, const Player * ptPlayer, const ImageTextureFrame & tImage, BOOL bUseLargeIcon);
}  // namespace Console
}  // namespace UIEventQueue
// X/Client/UserInterface/Console/UIConsoleTrade.cpp
namespace UITrade {
namespace Console {
void StartTrade(int32 nControllerIndex);
void EndTrade();
void ResetAcceptTimer();
BOOL CanTrade(int32 nControllerIndex);
void OtherIsBusy();
void UpdateOfferLists(const BOOL fLocalPlayerTradeDataChanged);
}  // namespace Console
}  // namespace UITrade

// Forward declarations; their definitions follow below.
void CTradeCancel(const Player * ptLocalPlayer, BOOL bForce, BOOL bSendMessage);
inline void sClientSendTradeMessage(TradeMessage & tMessage, const Player * ptLocalPlayer);
inline BOOL sCanTrade(const Player & tLocalPlayer)
{
    // The original passes the player's ACD id where UITrade expects a controller index.
    BOOL bCanTrade = UITrade::Console::CanTrade(tLocalPlayer.idPlayerACD);
    if (bCanTrade) {
        bCanTrade = !UIConversation::ACDIsInConversation(tLocalPlayer.idPlayerACD, FALSE, FALSE) &&
                    TradeCanTradeWith(tLocalPlayer, NULL) == TRADE_RESPONSE_OK;
    }

    return bCanTrade;
}
inline void sClientStartTradeSession(Player & tLocalPlayer, ACDNetworkName annOtherPlayer)
{
    UITrade::Console::StartTrade(tLocalPlayer.nControllerIndex);
    Player * ptTradePartner = PlayerGetByANN(annOtherPlayer);
    if (ptTradePartner == NULL) {
        return;
    }
    tLocalPlayer.tTradeData.idACDPlayerTradingWith = ACDFromANN(annOtherPlayer);
    if (!PlayerIsLocal(ptTradePartner)) {
        // a remote partner never gets told who it is trading with, so fill it in here
        ptTradePartner->tTradeData.idACDPlayerTradingWith = tLocalPlayer.idPlayerACD;
    }
}
inline void sClientOnPlayerBusy(const Player & tLocalPlayer)
{
    TradeMessage tMessage;
    tMessage.eMessageType = TRADE_MSG_OTHER_BUSY;
    tMessage.dwData = tLocalPlayer.tClientInfo.ann;
    sClientSendTradeMessage(tMessage, &tLocalPlayer);
}
inline void sClientEndTradeSession()
{

    UITrade::Console::EndTrade();














    for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer != NULL; ptPlayer = PlayerGetNextInGame(ptPlayer)) {

        ResetTradeSessionData(ptPlayer);
    }
}



inline BOOL sEitherPlayerInConversation(const Player & tFirst, const Player & tSecond)
{


    return UIConversation::ACDIsInConversation(tSecond.idPlayerACD, FALSE, FALSE) ||
           UIConversation::ACDIsInConversation(tFirst.idPlayerACD, FALSE, FALSE);
}


inline void sClientSendTradeMessage(TradeMessage & tMessage, const Player * ptLocalPlayer)
{


    MessageSendToServer(CMSG_TRADE, &tMessage, sizeof(tMessage), ptLocalPlayer);
}


void CTradeAccept(
    const Player & tLocalPlayer)
{
    Player * ptOther = TradeGetTradingTarget(tLocalPlayer);
    if (ptOther == NULL) {

        CTradeCancel(&tLocalPlayer, TRUE, TRUE);
        return;
    }

    XPooledDataIDList listOffer(MemoryPoolsGetDATAIDListPool());
    TradeOfferGetList(*ptOther, listOffer);

    BOOL bCanAccept = TradeOfferCanAccept(tLocalPlayer, listOffer);
    if (!bCanAccept) {


        UIGame::DisplayError(GameErrorData(GAMEERROR_IARESULT_CANNOT_ACCEPT_TRADE), FALSE, &tLocalPlayer);
    } else {
        TradeMessage tMsg;


        tMsg.eMessageType = TRADE_MSG_ACCEPT;
        tMsg.dwData = TradeGetHash(tLocalPlayer, *ptOther);
        sClientSendTradeMessage(tMsg, &tLocalPlayer);

        if (ptOther->tTradeData.bTradeAccepted) {
            // both sides have accepted
            sClientEndTradeSession();
        }
    }
}



void CTradeCancel(
    const Player * ptLocalPlayer,
    BOOL bForce,
    BOOL bSendMessage)
{
    if ((ptLocalPlayer != NULL && TradeIsTrading(ptLocalPlayer->idPlayerACD)) || bForce) {

        if (bSendMessage) {
            TradeMessage tMessage;

            tMessage.eMessageType = TRADE_MSG_CANCEL;
            sClientSendTradeMessage(tMessage, ptLocalPlayer);
        }
        sClientEndTradeSession();
    }
}


void CTradeTryStart(
    Player & tLocalPlayer,
    Player & tPlayer)
{

    if (!sCanTrade(tLocalPlayer)) {
        return;
    }


    if (TradeCanTradeWith(tLocalPlayer, &tPlayer) != TRADE_RESPONSE_OK ||
        sEitherPlayerInConversation(tLocalPlayer, tPlayer)) {
        // the other player is busy

        UITrade::Console::OtherIsBusy();
        return;
    }



    TradeMessage tMessage;

    tMessage.eMessageType = TRADE_MSG_TRY_START;
    tMessage.dwData = tPlayer.tClientInfo.ann;
    sClientSendTradeMessage(tMessage, &tLocalPlayer);
    sClientStartTradeSession(tLocalPlayer, tPlayer.tClientInfo.ann);
}


void CTradeUpdateOffer(
    Player & tLocalPlayer,
    uint64 uGoldOffer)
{
    TradeMessage tMessage;
    Player * ptOther;




    tMessage.uData64 = uGoldOffer;
    tMessage.eMessageType = TRADE_MSG_GOLD;
    sClientSendTradeMessage(tMessage, &tLocalPlayer);

    tLocalPlayer.tTradeData.bTradeAccepted = FALSE;
    tLocalPlayer.tTradeData.uMyGoldOffer = uGoldOffer;
    ptOther = TradeGetTradingTarget(tLocalPlayer);

    if (ptOther != NULL) {
        // changing the offer clears both acceptances
        ptOther->tTradeData.bTradeAccepted = FALSE;
    }
}



void CTradeHandleMessage(
    Player * ptLocalPlayer,
    TradeMessage & tMessage)
{
    switch (tMessage.eMessageType) {



        case TRADE_MSG_OTHER_BUSY:
            UITrade::Console::OtherIsBusy();



            sClientEndTradeSession();
            break;



        case TRADE_MSG_CANCEL:
            sClientEndTradeSession();
            break;


        case TRADE_MSG_TRY_START: {
            BOOL fIsBusy;

            Player * ptOther = PlayerGetByANN((ACDNetworkName)tMessage.dwData);
            fIsBusy = !sCanTrade(*ptLocalPlayer) || TradeCanTradeWith(*ptLocalPlayer, ptOther) != TRADE_RESPONSE_OK;














            if (fIsBusy) {
                sClientOnPlayerBusy(*ptLocalPlayer);
            } else {
                TradeMessage tMsg;

                sClientStartTradeSession(*ptLocalPlayer, (ACDNetworkName)tMessage.dwData);


                tMsg.eMessageType = TRADE_MSG_START;
                tMsg.dwData = ptLocalPlayer->tClientInfo.ann;
                sClientSendTradeMessage(tMsg, ptLocalPlayer);
            }
            break;
        }

        case TRADE_MSG_ACCEPT: {

            ACDNetworkName annPlayer = tMessage.ann;
            Player * ptPlayer = PlayerGetByANN(annPlayer);
            if (ptPlayer == NULL) {
                break;
            }


            ptPlayer->tTradeData.bTradeAccepted = tMessage.dwData;
            Player * ptPartner = TradeGetTradingTarget(*ptPlayer);
            if (ptPartner == NULL) {
                break;
            }

            if (ptPlayer->tTradeData.bTradeAccepted && ptPartner->tTradeData.bTradeAccepted) {
                // both sides have accepted

                sClientEndTradeSession();
            }
            break;
        }


        case TRADE_MSG_START:

            sClientStartTradeSession(*ptLocalPlayer, (ACDNetworkName)tMessage.dwData);
            break;



        case TRADE_MSG_COMPLETE: {

            Player * ptOther = PlayerGetByPlayerIndex(tMessage.dwData2);
            if (ptOther != NULL) {

                CRefString usPlayer = CPlayerGetServiceName(*ptOther, FALSE);
                CRefString sMsg = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_TRADE), "TradeComplete"), usPlayer.str(), NULL);



                UIEventQueue::Console::DisplayLongMessageForPlayer(sMsg, ptLocalPlayer, cgtImageTextureFrameNull, FALSE);
            }






            for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer != NULL; ptPlayer = PlayerGetNextInGame(ptPlayer)) {

                ResetTradeSessionData(ptPlayer);
            }

            UITrade::Console::EndTrade();
            break;
        }




        case TRADE_MSG_STATE: {

            Player * ptPlayerData = PlayerGetByANN(tMessage.ann);
            BOOL fLocalPlayerTradeDataChanged = ptPlayerData == ptLocalPlayer;
            if (!fLocalPlayerTradeDataChanged) {
                // the server sent the partner's offer, so link the two players
                ptLocalPlayer->tTradeData.idACDPlayerTradingWith = ptPlayerData->idPlayerACD;
                ptPlayerData->tTradeData.idACDPlayerTradingWith = ptLocalPlayer->idPlayerACD;
            }


            SigmaMemoryCopy(ptPlayerData->tTradeData.arrItems, tMessage.arrItems, sizeof(ptPlayerData->tTradeData.arrItems));
            ptPlayerData->tTradeData.bTradeAccepted = tMessage.dwData2;
            ptPlayerData->tTradeData.uMyGoldOffer = tMessage.uData64;


            UITrade::Console::UpdateOfferLists(fLocalPlayerTradeDataChanged);
            UITrade::Console::ResetAcceptTimer();
            break;
        }



        case TRADE_MSG_GOLD:
        case TRADE_MSG_ITEM:
            // client to server only

            DisplayInternalError(NULL, "", 407, ERROR_INTERNAL_FATAL);
            ExitProgram(ERROR_INTERNAL_FATAL);
            break;
    }
}
