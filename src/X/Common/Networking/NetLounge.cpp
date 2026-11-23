// Original file: X/Common/Networking/NetLounge.cpp

#include "../CommonTypes.h"
#include "../../../Contrib/Contrib/blz/include/blz/tuple.h"
#include "../../Common/SNO/SNOName.h"
#include "../../Common/WorldPlace.h"
#include "../../Client/Gfx/GfxVB.h"
#include "../../Common/D3TypeDescriptor/D3TypeDescriptor.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptorField.h"

// Declared here because their headers do not carry them yet.
extern const XTypeDescriptor * DT_SERIALIZEDATA;  // X/Sigma/Files/XFilesCommon.cpp:20
extern const XTypeDescriptor * DT_SNO;  // X/Common/D3TypeDescriptor/D3DescribedType.cpp

#include "../../Sigma/DataStructures/XGrowableList.h"
#include "../Act.h"
#include "../Game.h"
#include "../OnlineService/OnlineService.h"
#include "XSocket.h"

// X/Common/Networking/NetLounge.h, which is not adopted yet.
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

struct NetGameAdvertItem;  // defined in a header that is not adopted yet

struct NetHeroAdvert;  // defined in a header that is not adopted yet

typedef XGrowableList<NetGameAdvertItem, NetGameAdvertItem &> TNetGameList;

typedef XGrowableList<NetHeroAdvert, NetHeroAdvert &> TNetHeroList;

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

extern const XTypeDescriptor * DT_NET_GAME_ADVERT;

extern const uint32 ADVERTIZED_SIG = 0x58414456;  // line 50
extern const uint32 ADVERTIZED_VER = 1;  // line 51
extern const uint32 ADVERTIZING_SEND_RATE = 60;  // line 53

// line 272
void NetLoungeExit()
{

    NetLoungeGlobals * ptNetLoungeGlobals = sgtAppGlobals.ptNetLoungeGlobals;

    XSocket & tSocket = ptNetLoungeGlobals->tListenerSocket;
    XSocketClose(tSocket);

    ptNetLoungeGlobals->bInLounge = FALSE;
    ptNetLoungeGlobals->bRequestGameLists = FALSE;
}

// The field names are "" in the binary.
static const TypeDescriptorField AdvertizePacket_Fields[4] = {
    TypeDescriptorField("", DT_INT, offsetof(AdvertizePacket, dwAdvertizeSig), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(AdvertizePacket, dwAdvertizeVersion), NULL, 1),
    TypeDescriptorField("", DT_NET_GAME_ADVERT, offsetof(AdvertizePacket, tNetGameAdvert), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(AdvertizePacket), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_AdvertizePacket("AdvertizePacket", 0, AdvertizePacket_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_ADVERTIZE_PACKET = &TypeDescriptor_AdvertizePacket;

static const TypeDescriptorField NetGameAdvert_Fields[14] = {
    TypeDescriptorField("", DT_INT, offsetof(NetGameAdvert, dwHostIP), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(NetGameAdvert, idSGame), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(NetGameAdvert, dwMaxPlayers), NULL, NULL, (const void *)4),
    BoundedTypeDescriptorField("", DT_INT, offsetof(NetGameAdvert, dwCurPlayers), NULL, NULL, (const void *)4),
    CharArrayTypeDescriptorField("", offsetof(NetGameAdvert, sBuildVersion), "", 1, 0x30, -1),
    TypeDescriptorField("", DT_INT, offsetof(NetGameAdvert, dwSnoCoreDataHash), NULL, 1),
    CharArrayTypeDescriptorField("", offsetof(NetGameAdvert, sGameName), "", 1, 0x21, -1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(NetGameAdvert, eGameType), NULL, (const void *)-1, (const void *)0x6),
    TypeDescriptorField("", DT_INT, offsetof(NetGameAdvert, eAct), NULL, 1),
    TypeDescriptorField("", DT_SNO, offsetof(NetGameAdvert, snoQuest), NULL, 1),
    TypeDescriptorField("", DT_SNO, offsetof(NetGameAdvert, snoInitialWorld), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(NetGameAdvert, uGameCreationFlags), NULL, NULL, (const void *)0xfffff),
    BoundedTypeDescriptorField("", DT_INT, offsetof(NetGameAdvert, bJoinAllowed), NULL, NULL, (const void *)0x1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(NetGameAdvert), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_NetGameAdvert("NetGameAdvert", 0, NetGameAdvert_Fields, 14, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_NET_GAME_ADVERT = &TypeDescriptor_NetGameAdvert;

