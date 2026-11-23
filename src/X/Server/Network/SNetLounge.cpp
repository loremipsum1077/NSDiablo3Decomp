#include "../../Common/Networking/NetLounge.h"
#include "../SGameManager.h"
#include "../../Common/Definitions/GlobalDefinition.h"
#include "../../Common/Networking/GameMessage.h"
#include "../../Common/Player.h"
#include "../../Sigma/Networking/XEncoder.h"
#include "../../Sigma/Networking/XNetAddress.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptorField.h"
#include "../../Sigma/buildver.h"
#include "../../../BlizzardCore/Include/Blizzard/String.h"

// Original declarations: Networking/NetLounge.h, Networking/Network.h, SNO/SNO.h, Network/Server.h.
extern const uint32 ADVERTIZED_SIG;
extern const uint32 ADVERTIZED_VER;
extern const uint32 ADVERTIZING_SEND_RATE;
extern const XTypeDescriptor * DT_ADVERTIZE_PACKET;
void NetLoungeExit();
void NetworkGetIP(XNetAddress & tAddress);
uint8 * NetworkGetEncodeBuffer();
void NetGetPlayerName(LPSTR sPlayerName, uint32 uLength);
uint32 SNOGetCoreDataHash();
void ServerSendOutOfGameMessage(ServerSocketID idServerSocket, void * pMessage, GameMessageType eType, uint32 dwSize);







































































static void sNetLoungeError()
{

    NetLoungeExit();

    GameActiveGameCommonDataIsClient();
}











static void sNetLoungePutOnWire(const char * pbBuffer, uint32 dwBufferSize)
{


    AppGlobals & tAppGlobals = sgtAppGlobals;
    NetLoungeGlobals * ptNetLoungeGlobals = tAppGlobals.ptNetLoungeGlobals;



    XSocket & tSocket = ptNetLoungeGlobals->tAdvertSocket;



    XNetAddress tBroadcastAddress;
    tBroadcastAddress.SetBroadcast(LOUNGE_PORT_ADDRESS);

    uint32 uSentSize;
    XSocketBase::Result eResult = tSocket.tBaseSocket.SendTo(pbBuffer, dwBufferSize, tBroadcastAddress, uSentSize);

    if (eResult != XSocketBase::RESULT_SUCCESS)
    {
        sNetLoungeError();
    }
}





static void sNetLoungeSendAdvert(NetGameAdvert & tNetGameAdvert)
{

    AdvertizePacket tAdvertPacket;

    tAdvertPacket.dwAdvertizeSig = ADVERTIZED_SIG;
    tAdvertPacket.dwAdvertizeVersion = ADVERTIZED_VER;

    SigmaMemoryCopy(&tAdvertPacket.tNetGameAdvert, &tNetGameAdvert, sizeof(tNetGameAdvert));

    XEncoder tEncoder;
    tEncoder.SetData(NetworkGetEncodeBuffer(), SOCKET_SEND_BUFFER_SIZE);

    TypeDescriptorField field(DT_ADVERTIZE_PACKET, NULL);
    DT_ADVERTIZE_PACKET->Encode(&tAdvertPacket, field, tEncoder);
    tEncoder.ZeroUnusedByte();

    sNetLoungePutOnWire(static_cast<const char *>(tEncoder.Data()), tEncoder.UsedBytes());
}




static void sNetLoungeInitializeAdvert(NetGameAdvert & tAdvert)
{

    SigmaMemoryClear(&tAdvert, sizeof(tAdvert));

    DefinitionLockR<GlobalDefinition>();
    XNetAddress tAddress;
    NetworkGetIP(tAddress);
    tAdvert.dwHostIP = tAddress.Node();

    tAdvert.snoQuest = tAdvert.snoInitialWorld = SNO_NONE;
    tAdvert.eAct = 0;
    tAdvert.uGameCreationFlags = 0;
    tAdvert.bJoinAllowed = TRUE;
    tAdvert.idSGame = SGAMEID_INVALID;
    tAdvert.dwMaxPlayers = SGameGetMaxPlayers();
    tAdvert.dwCurPlayers = 0;
    Blizzard::String::Copy(tAdvert.sBuildVersion, BuildVersion(), sizeof(tAdvert.sBuildVersion));
    tAdvert.dwSnoCoreDataHash = SNOGetCoreDataHash();
}






static void sNetLoungeAdvertizeGame(ServerSocketID idServerSocket)

{
    SGame & tSGame = SGameManager::GameGet();
    if (!(tSGame.tStartParams.dwCreationFlags & 0x100)) // Original tests bit 8 of dwCreationFlags; no named flag in this CU.
    {
        return;
    }


    NetGameAdvert tAdvert;

    sNetLoungeInitializeAdvert(tAdvert);

    tAdvert.idSGame = tSGame.id;
    tAdvert.eGameType = tSGame.tStartParams.eGameType;
    tAdvert.eAct = tSGame.tStartParams.eAct;
    tAdvert.snoQuest = tSGame.tStartParams.snoInitialQuest;
    tAdvert.snoInitialWorld = tSGame.tStartParams.snoInitialWorld;
    tAdvert.uGameCreationFlags = tSGame.tStartParams.dwCreationFlags;
    tAdvert.bJoinAllowed = SGameIsJoinAllowed(tSGame.id);

    tAdvert.dwCurPlayers = PlayersGetCount();

    CHAR usAccountName[128];
    NetGetPlayerName(usAccountName, sizeof(usAccountName));

    if (SGameManager::GetGameCount() && tSGame.ptSGameGlobals)
    {





        Blizzard::String::Format(tAdvert.sGameName, sizeof(tAdvert.sGameName), "%s", tSGame.ptSGameGlobals->szGameName);
    }
    else
    {
        Blizzard::String::Format(tAdvert.sGameName, sizeof(tAdvert.sGameName), "%s's Local Game", usAccountName);
    }

    if (idServerSocket == SERVERSOCKETID_INVALID)
    {

        sNetLoungeSendAdvert(tAdvert);
    }
    else
    {
        LANParamsMessage tMessage;
        SigmaMemoryCopy(&tMessage.tLANGameAdvert, &tAdvert, sizeof(tAdvert));


        ServerSendOutOfGameMessage(idServerSocket, &tMessage, SMSG_LAN_GAME_PARAMS, sizeof(tMessage));
    }
}














































void SNetLoungeUpdate()
{



    AppGlobals & tAppGlobals = sgtAppGlobals;
    NetLoungeGlobals * ptNetLoungeGlobals = tAppGlobals.ptNetLoungeGlobals;

    uint32 dwNow = AppGetTicks();

    if (ptNetLoungeGlobals && ptNetLoungeGlobals->bAdvertizing)
    {



        uint32 nTimeSinceLastAdvert = dwNow - ptNetLoungeGlobals->dwTimeLastAdvert;
        if (nTimeSinceLastAdvert >= ADVERTIZING_SEND_RATE)
        {
            ptNetLoungeGlobals->dwTimeLastAdvert = dwNow;


            SGameIterator tIt;
            while (SGameManager::GameIterate(tIt))
            {
                SGame * ptSGame = tIt.ptElement;

                BOOL bReturnToClient = GameServerCodeEnter_Track() || _GameServerCodeEnter(ptSGame->id);
                sNetLoungeAdvertizeGame(SERVERSOCKETID_INVALID);
                GameServerCodeLeave_Tracked(bReturnToClient);
            }
        }
    }




}

// LINKER-REMOVED: DWARF external, low_pc 0, 0xb8 bytes, SNetLounge.cpp:332; its out-of-line call to sNetLoungeAdvertizeGame is why the original emits that static separately; conditions at 343/352 and the break at 358 are inferred (see draft SNetLounge.txt).
// Its out-of-line call to sNetLoungeAdvertizeGame is why the original emits that static separately.

void SNetLoungeSendLANGameParams(
    ServerSocketID idServerSocket,
    SGameID idSGame)
{




    AppGlobals & tAppGlobals = sgtAppGlobals;
    NetLoungeGlobals * ptNetLoungeGlobals = tAppGlobals.ptNetLoungeGlobals;
    // Inferred from line table 343 (member load col 27, branch col 7 = 'ptNetLoungeGlobals->bAdvertizing' at 20 cols).
    if (ptNetLoungeGlobals->bAdvertizing)
    {


        SGameIterator tIt;
        while (SGameManager::GameIterate(tIt))
        {
            SGame * ptSGame = tIt.ptElement;
            // Inferred: one load (col 62) + compare at 352; idSGame is live only up to the call, so the match path leaves the loop.
            if (ptSGame->id == idSGame)
            {
                BOOL bReturnToClient = GameServerCodeEnter_Track() || _GameServerCodeEnter(ptSGame->id);
                sNetLoungeAdvertizeGame(idServerSocket);
                GameServerCodeLeave_Tracked(bReturnToClient);
                // Inferred: no back edge after line 356 in the line table.
                break;
            }
        }
    }

}
