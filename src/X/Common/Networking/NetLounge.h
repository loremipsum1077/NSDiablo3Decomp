// Adopted DWARF layouts for the server lounge; unrelated include dependencies omitted.
// Original file: X/Common/Networking/NetLounge.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../CommonTypes.h"
#include "XSocket.h"
#include "../OnlineService/OnlineService.h"
#include "../../Sigma/DataStructures/XGrowableList.h"
#include "../../Sigma/SigmaTypes.h"

#include "../Act.h"
#include "../SNO/SNOFiles.h"

struct NetGameAdvertItem;
struct NetHeroAdvert;
typedef int32 GameType;

struct NetGameAdvert {  // size 0xa0
    uint32 dwHostIP;  // 0x00
    uint16 uHostPort;  // 0x04
    SGameID idSGame;  // 0x08
    OnlineService::GameId tGameId;  // 0x10
    uint32 dwMaxPlayers;  // 0x28
    uint32 dwCurPlayers;  // 0x2c
    CHAR sBuildVersion[48];  // 0x30
    uint32 dwSnoCoreDataHash;  // 0x60
    CHAR sGameName[33];  // 0x64
    GameType eGameType;  // 0x88
    uint32 uGameCreationFlags;  // 0x8c
    Act eAct;  // 0x90
    SNO snoQuest;  // 0x94
    SNO snoInitialWorld;  // 0x98
    BOOL bJoinAllowed;  // 0x9c
};
static_assert(sizeof(NetGameAdvert) == 0xa0, "NetGameAdvert");

struct AdvertizePacket {  // size 0xa8
    uint32 dwAdvertizeSig;  // 0x00
    uint32 dwAdvertizeVersion;  // 0x04
    NetGameAdvert tNetGameAdvert;  // 0x08
};
static_assert(sizeof(AdvertizePacket) == 0xa8, "AdvertizePacket");

typedef XGrowableList<NetGameAdvertItem, NetGameAdvertItem &> TNetGameList;

typedef XGrowableList<NetHeroAdvert, NetHeroAdvert &> TNetHeroList;

struct LANParamsMessage {  // size 0xa8
    RequiredMessageHeader tHeader;  // 0x00
    NetGameAdvert tLANGameAdvert;  // 0x08
};
static_assert(sizeof(LANParamsMessage) == 0xa8, "LANParamsMessage");

struct NetLoungeGlobals {  // size 0x1c8
    TNetGameList listGames;  // 0x00
    TNetGameList listGamesSnapshot;  // 0x58
    TNetHeroList listHeros;  // 0xb0
    XSocket tListenerSocket;  // 0x108
    XSocket tAdvertSocket;  // 0x158
    BOOL bInLounge;  // 0x1a8
    BOOL bAdvertizing;  // 0x1ac
    uint32 dwTimeLastAdvert;  // 0x1b0
    BOOL bRequestGameLists;  // 0x1b4
    uint32 dwTimeLastPolledGameList;  // 0x1b8
    uint32 dwTimeLastPolledBattleNetInfo;  // 0x1bc
    SGameID idSLANGameToJoin;  // 0x1c0
};
static_assert(sizeof(NetLoungeGlobals) == 0x1c8, "NetLoungeGlobals");


// Functions defined in this file (bodies are not in the DWARF):
//   line    22  NetGameAdvert::NetGameAdvert()  [inlined into 1 functions]
//   line    49  AdvertizePacket::AdvertizePacket()  [inlined into 1 functions]
//   line    81  LANParamsMessage::LANParamsMessage()  [inlined into 1 functions]
