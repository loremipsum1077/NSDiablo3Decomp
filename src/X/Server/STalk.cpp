// X/Server/STalk.cpp
#include "STalk.h"

#include "../Common/ActorCommonData.h"
#include "../Common/CommonTypes.h"
#include "../Common/NPC.h"
#include "../Common/Player.h"
#include "../Sigma/Memory/SigmaMemory.h"
#include "Network/ServerMessageSend.h"
#include "SGame.h"
#include "SGameManager.h"
#include "SNPC.h"




struct STalkGlobals {  // size 0x4
    int32 nUnused;  // 0x00
};
static_assert(sizeof(STalkGlobals) == 0x4, "STalkGlobals");









void STalkInitializeForGame()

{

    SGame & tSGame = SGameManager::GameGet();
    STalkGlobals *& ptSTalkGlobals = tSGame.ptSTalkGlobals;

    ptSTalkGlobals = (STalkGlobals *)SigmaMemoryNew(sizeof(STalkGlobals), 4, tSGame.ptMemoryInterface, TRUE);
}




void STalkDisposeForGame()

{
    SGame & tSGame = SGameManager::GameGet();
    STalkGlobals *& ptSTalkGlobals = tSGame.ptSTalkGlobals;


    SigmaMemoryFree(ptSTalkGlobals, tSGame.ptMemoryInterface); ptSTalkGlobals = NULL;
}




BOOL STalkIsTalking(const ACDID idACDPlayer)

{
    Player * ptPlayer = PlayerGetByACD(idACDPlayer);


    if (ptPlayer == NULL)
    {
        return FALSE;
    }

    return ptPlayer->tTalkData.idACDTalkingTo != ACDID_INVALID;

}



void STalkSetTalkingTo(ActorCommonData & tACDPlayer, ActorCommonData & tACDTalkingTo)


{



    Player * ptPlayer = PlayerGetByACD(tACDPlayer.id);






    ptPlayer->tTalkData.idACDTalkingTo = tACDTalkingTo.id;

    NPCSetTalkTarget(tACDTalkingTo, &tACDPlayer);
}



void STalkResetTalkingTo(Player & tPlayer)

{
    ActorCommonData * ptACDTalkingTo = ACDTryToGet(tPlayer.tTalkData.idACDTalkingTo);


    tPlayer.tTalkData.Reset();

    if (ptACDTalkingTo != NULL)
    {


        ACDID idACDOtherPlayerTalkingToNPC = NPCIsAnyPlayerTalkingTo(ptACDTalkingTo->id);
        if (idACDOtherPlayerTalkingToNPC != ACDID_INVALID)
        {
            ActorCommonData & tACDOtherPlayer = ACDGet(idACDOtherPlayerTalkingToNPC);
            NPCSetTalkTarget(*ptACDTalkingTo, &tACDOtherPlayer);
        }
        else
        {
            NPCSetTalkTarget(*ptACDTalkingTo, NULL);
        }
    }
}



void STalkDoneTalking(const ACDID idACDPlayer)

{
    Player * ptPlayer = PlayerGetByACD(idACDPlayer);


    STalkResetTalkingTo(*ptPlayer);
}



void STalkForceDoneTalking(const ACDID idACDPlayer)

{
    Player * ptPlayer = PlayerGetByACD(idACDPlayer);
    if (ptPlayer->tTalkData.idACDTalkingTo != ACDID_INVALID)
    {

        ServerMessageSendForceInterfaceClose(idACDPlayer);


        if (ptPlayer->idACDActiveVendor != ACDID_INVALID)
        {





            NPCVendorStop(*ptPlayer);
        }

        STalkDoneTalking(idACDPlayer);
    }
}



void STalkForcePlayersDoneTalkingToNPC(const ACDID idACDNPC)

{
    for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer != NULL; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {
        if (ptPlayer->tTalkData.idACDTalkingTo == idACDNPC)
        {
            STalkForceDoneTalking(ptPlayer->idPlayerACD);
        }
    }
}
