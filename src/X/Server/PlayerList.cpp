// Original file: X/Server/PlayerList.cpp

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "../Common/ActorCommonData.h"
#include "../Common/CommonTypes.h"
#include "../Common/Player.h"
#include "../Common/WorldPlace.h"
#include "PlayerList.h"
#include "SGameManager.h"
#include "SPlayer.h"
#include "../Sigma/Math/Rect.h"
#include "../Sigma/Memory/SigmaMemory.h"
#include "../Sigma/SigmaTypes.h"

typedef BOOL (*PlayerListFilterFunction)(const Player &, void *);

struct PlayerListGlobals {  // size 0x30
    TPlayerPointerPool tPlayerPointerPool;  // 0x00
};
static_assert(sizeof(PlayerListGlobals) == 0x30, "PlayerListGlobals");

struct AllPlayerFilterData {  // size 0xc
    int32 nPlayerIndexIgnore;  // 0x00
    GameConnectionID idGameConnection;  // 0x04
    BOOL bIgnoreHardcoreDeadPlayers;  // 0x08
};
static_assert(sizeof(AllPlayerFilterData) == 0xc, "AllPlayerFilterData");

struct AllyFilterData {  // size 0x8
    ACDID idACDPlayerAlliesOf;  // 0x00
    ACDID idACDPlayerIgnore;  // 0x04
};
static_assert(sizeof(AllyFilterData) == 0x8, "AllyFilterData");

PlayerList::PlayerList(TPlayerPointerPool * pPool)
    : XPooledList<Player *, Player *>(pPool)
{
}

void PlayerListInitializeForGame()
{
    SGame & tSGame = SGameManager::GameGet();
    PlayerListGlobals *& ptPlayerListGlobals = tSGame.ptPlayerListGlobals;

    ptPlayerListGlobals = (PlayerListGlobals *)SigmaMemoryNew(sizeof(PlayerListGlobals), 4,
                                                              tSGame.ptMemoryInterface, TRUE);

    ptPlayerListGlobals->tPlayerPointerPool.Initialize();
    ptPlayerListGlobals->tPlayerPointerPool.SetMemoryInterface(tSGame.ptMemoryInterface);
    ptPlayerListGlobals->tPlayerPointerPool.SetBlockCount_Tracked(64);
}

void PlayerListDisposeForGame()
{
    SGame & tSGame = SGameManager::GameGet();
    PlayerListGlobals *& ptPlayerListGlobals = tSGame.ptPlayerListGlobals;

    ptPlayerListGlobals->tPlayerPointerPool.Dispose();

    SigmaMemoryFree(ptPlayerListGlobals, tSGame.ptMemoryInterface);
    ptPlayerListGlobals = NULL;
}

TPlayerPointerPool * PlayerListGetElementPool()
{
    PlayerListGlobals & tPlayerListGlobals = *SGameManager::GameGet().ptPlayerListGlobals;
    return &tPlayerListGlobals.tPlayerPointerPool;
}

static void sBuildPlayerList(PlayerList & listPlayers, BOOL fInGamePlayersOnly,
                             PlayerListFilterFunction pfnFilter, void * pUserData)
{
    if (fInGamePlayersOnly)
    {
        for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer != NULL;
             ptPlayer = PlayerGetNextInGame(ptPlayer))
        {
            if (pfnFilter(*ptPlayer, pUserData) == TRUE)
            {
                listPlayers.AddTail_Tracked(ptPlayer);
            }
        }
    }
    else
    {
        for (Player * ptPlayer = PlayerGetFirstAll(); ptPlayer != NULL;
             ptPlayer = PlayerGetNextAll(ptPlayer))
        {
            if (pfnFilter(*ptPlayer, pUserData) == TRUE)
            {
                listPlayers.AddTail_Tracked(ptPlayer);
            }
        }
    }
}

static BOOL sIsInsideKnowRegion(const Player & tPlayer, const WorldPlace & tPlace)
{
    ActorCommonData * ptACDPlayer = ACDTryToGet(tPlayer.idPlayerACD);
    if (ptACDPlayer == NULL)
        return FALSE;

    // Assigned rather than initialised: the extra copy keeps this function over the inline
    // threshold, so ACDTryToGet is inlined into it and it stays out of line, as in the original.
    WorldPlace wpPlayerLocation;
    wpPlayerLocation = ptACDPlayer->GetPlace();
    Rect2D rectWorld;
    if (wpPlayerLocation.idSWorld != tPlace.idSWorld)
        return FALSE;

    rectWorld.left = wpPlayerLocation.wp.x - 175.0f;
    rectWorld.top = wpPlayerLocation.wp.y - 175.0f;
    rectWorld.right = wpPlayerLocation.wp.x + 175.0f;
    rectWorld.bottom = wpPlayerLocation.wp.y + 175.0f;

    return rectWorld.ContainsPoint(tPlace.wp.x, tPlace.wp.y);
}

static BOOL sPlayerFilterContentRestrictions(const Player & tPlayer, void * pUserData)
{
    uint32 uPlayerContentFlags = SPlayerGetContentFlags(tPlayer);
    uint32 uRequiredContentFlags = *(uint32 *)pUserData;
    return (uRequiredContentFlags & ~uPlayerContentFlags) != 0;
}

void PlayerListGetAllContentFlagRestrictedPlayers(PlayerList & listPlayers, uint32 uContentFlagsRequired)
{
    sBuildPlayerList(listPlayers, FALSE, sPlayerFilterContentRestrictions, &uContentFlagsRequired);
}

static BOOL sPlayerFilterPlayersLocalToPlayer(const Player & tPlayer, void * pUserData)
{
    return PlayersAreLocalToEachOther(tPlayer, *(const Player *)pUserData);
}

static BOOL sPlayerFilterAll(const Player & tPlayer, void * pUserData)
{
    AllPlayerFilterData * ptData = (AllPlayerFilterData *)pUserData;

    if (ptData->nPlayerIndexIgnore != -1 && tPlayer.nPlayerIndex == ptData->nPlayerIndexIgnore)
        return FALSE;

    if (ptData->bIgnoreHardcoreDeadPlayers && PlayerIsHardcoreAndDead(tPlayer))
        return FALSE;

    return TRUE;
}

void PlayerListGetAllConnected(PlayerList & listPlayers, const int32 nPlayerIndexIgnore,
                               BOOL bIgnoreHardcoreDeadPlayers)
{
    AllPlayerFilterData tAllPlayerFilterData;
    tAllPlayerFilterData.nPlayerIndexIgnore = nPlayerIndexIgnore;
    tAllPlayerFilterData.bIgnoreHardcoreDeadPlayers = bIgnoreHardcoreDeadPlayers;
    sBuildPlayerList(listPlayers, FALSE, sPlayerFilterAll, &tAllPlayerFilterData);
}

static BOOL sPlayerFilterAllExceptGC(const Player & tPlayer, void * pUserData)
{
    AllPlayerFilterData * ptData = (AllPlayerFilterData *)pUserData;

    if (ptData->idGameConnection != -1 && tPlayer.idGameConnection == ptData->idGameConnection)
        return FALSE;

    if (ptData->bIgnoreHardcoreDeadPlayers && PlayerIsHardcoreAndDead(tPlayer))
        return FALSE;

    return TRUE;
}

void PlayerListGetAllConnectedExceptConnection(PlayerList & listPlayers, const int32 idGameConnection,
                                               BOOL bIgnoreHardcoreDeadPlayers)
{
    AllPlayerFilterData tAllPlayerFilterData;
    tAllPlayerFilterData.idGameConnection = idGameConnection;
    tAllPlayerFilterData.bIgnoreHardcoreDeadPlayers = bIgnoreHardcoreDeadPlayers;
    sBuildPlayerList(listPlayers, FALSE, sPlayerFilterAllExceptGC, &tAllPlayerFilterData);
}

static BOOL sPlayerFilterAllInGame(const Player & tPlayer, void * pUserData)
{
    AllPlayerFilterData * ptData = (AllPlayerFilterData *)pUserData;

    if (ptData->nPlayerIndexIgnore != -1 && tPlayer.nPlayerIndex == ptData->nPlayerIndexIgnore)
        return FALSE;

    if (ptData->idGameConnection != -1 && tPlayer.idGameConnection != ptData->idGameConnection)
        return FALSE;

    if (tPlayer.idPlayerACD == ACDID_INVALID)
        return FALSE;

    if (ptData->bIgnoreHardcoreDeadPlayers && PlayerIsHardcoreAndDead(tPlayer))
        return FALSE;

    return TRUE;
}

void PlayerListGetAllInGame(PlayerList & listPlayers, const int32 nPlayerIndexIgnore,
                            const BOOL bIgnoreHardcoreDeadPlayers,
                            const GameConnectionID idGameConnectionToMatch)
{
    AllPlayerFilterData tAllPlayerFilterData;
    tAllPlayerFilterData.nPlayerIndexIgnore = nPlayerIndexIgnore;
    tAllPlayerFilterData.idGameConnection = idGameConnectionToMatch;
    tAllPlayerFilterData.bIgnoreHardcoreDeadPlayers = bIgnoreHardcoreDeadPlayers;
    sBuildPlayerList(listPlayers, TRUE, sPlayerFilterAllInGame, &tAllPlayerFilterData);
}

static BOOL sPlayerFilterAllInGameRemote(const Player & tPlayer, void * pUserData)
{
    AllPlayerFilterData * ptData = (AllPlayerFilterData *)pUserData;

    if (ptData->nPlayerIndexIgnore != -1 && tPlayer.nPlayerIndex == ptData->nPlayerIndexIgnore)
        return FALSE;

    if (ptData->idGameConnection != -1 && tPlayer.idGameConnection == ptData->idGameConnection)
        return FALSE;

    if (tPlayer.idPlayerACD == ACDID_INVALID)
        return FALSE;

    if (ptData->bIgnoreHardcoreDeadPlayers && PlayerIsHardcoreAndDead(tPlayer))
        return FALSE;

    return TRUE;
}

void PlayerListGetAllPlayersLocalToPlayer(PlayerList & listPlayers, const Player * ptPlayer)
{
    sBuildPlayerList(listPlayers, TRUE, sPlayerFilterPlayersLocalToPlayer, (void *)ptPlayer);
}

void PlayerListGetAllInGameRemote(PlayerList & listPlayers, const GameConnectionID idGameConnectionToExclude,
                                  const int32 nPlayerIndexIgnore, const BOOL bIgnoreHardcoreDeadPlayers)
{
    AllPlayerFilterData tAllPlayerFilterData;
    tAllPlayerFilterData.nPlayerIndexIgnore = nPlayerIndexIgnore;
    tAllPlayerFilterData.idGameConnection = idGameConnectionToExclude;
    tAllPlayerFilterData.bIgnoreHardcoreDeadPlayers = bIgnoreHardcoreDeadPlayers;
    sBuildPlayerList(listPlayers, TRUE, sPlayerFilterAllInGameRemote, &tAllPlayerFilterData);
}

static BOOL sPlayerFilterExact(const Player & tPlayer, void * pUserData)
{
    const Player * tPlayerToMatch = (const Player *)pUserData;
    return tPlayerToMatch->nPlayerIndex == tPlayer.nPlayerIndex;
}

void PlayerListGetSingle(PlayerList & listPlayers, const Player & tPlayer)
{
    sBuildPlayerList(listPlayers, FALSE, sPlayerFilterExact, (void *)&tPlayer);
}

void PlayerListGetSingle(PlayerList & listPlayers, const ACDID idACDPlayer)
{
    const Player * ptPlayer = PlayerGetByACD(idACDPlayer);
    if (ptPlayer != NULL)
        PlayerListGetSingle(listPlayers, *ptPlayer);
}

void PlayerListGetInKnowRegionByID(PlayerList & listPlayers, OnlineService::HeroId idPlayer,
                                   const WorldPlace & tPlace)
{
    if (idPlayer == OnlineService::INVALID_HERO_ID)
    {
        PlayerListGetInKnowRegion(listPlayers, tPlace);
        return;
    }

    for (Player * ptPlayer = PlayerGetFirstAll(); ptPlayer != NULL; ptPlayer = PlayerGetNextAll(ptPlayer))
    {
        if (ptPlayer->uHeroId == idPlayer && sIsInsideKnowRegion(*ptPlayer, tPlace))
            listPlayers.AddTail_Tracked(ptPlayer);
    }
}

static BOOL sPlayerFilterInKnowRegion(const Player & tPlayer, void * pUserData)
{
    const WorldPlace & tPlace = *(const WorldPlace *)pUserData;
    return sIsInsideKnowRegion(tPlayer, tPlace);
}

void PlayerListGetInKnowRegion(PlayerList & listPlayers, const WorldPlace & tPlace)
{
    sBuildPlayerList(listPlayers, TRUE, sPlayerFilterInKnowRegion, (void *)&tPlace);
}

static BOOL sPlayerFilterKnowsACD(const Player & tPlayer, void * pUserData)
{
    return ACDGet(*(ACDID *)pUserData).IsKnownByPlayer(tPlayer);
}

static BOOL sPlayerFilterKnowsACDIgnoreACD(const Player & tPlayer, void * pUserData)
{
    ACDID idACD = *(ACDID *)pUserData;
    if (tPlayer.idPlayerACD == idACD)
        return FALSE;

    return ACDGet(idACD).IsKnownByPlayer(tPlayer);
}

void PlayerListGetThatKnowACD(PlayerList & listPlayers, const ACDID idACD, BOOL bIgnoreACD)
{
    if (bIgnoreACD)
        sBuildPlayerList(listPlayers, TRUE, sPlayerFilterKnowsACDIgnoreACD, (void *)&idACD);
    else
        sBuildPlayerList(listPlayers, TRUE, sPlayerFilterKnowsACD, (void *)&idACD);
}

static BOOL sPlayerFilterAllies(const Player & tPlayer, void * pUserData)
{
    AllyFilterData * ptData = (AllyFilterData *)pUserData;

    if (ptData->idACDPlayerIgnore != ACDID_INVALID && tPlayer.idPlayerACD == ptData->idACDPlayerIgnore)
        return FALSE;

    ActorCommonData * ptACDPlayer = ACDTryToGet(tPlayer.idPlayerACD);
    ActorCommonData * ptACDAlly = ACDTryToGet(ptData->idACDPlayerAlliesOf);
    if (ptACDPlayer != NULL && ptACDAlly != NULL && ACDIsFriendly(*ptACDPlayer, *ptACDAlly, FALSE))
        return TRUE;

    return FALSE;
}

void PlayerListGetAllies(PlayerList & listPlayers, const ACDID idACDPlayerAlliesOf, const ACDID idACDPlayerIgnore)
{
    AllyFilterData tAllyFilterData;
    tAllyFilterData.idACDPlayerAlliesOf = idACDPlayerAlliesOf;
    tAllyFilterData.idACDPlayerIgnore = idACDPlayerIgnore;
    sBuildPlayerList(listPlayers, TRUE, sPlayerFilterAllies, &tAllyFilterData);
}

static BOOL sPlayerFilterPrimaryPlayers(const Player & tPlayer, void * pUserData)
{
    if (tPlayer.idPlayerACD == ACDID_INVALID)
        return FALSE;

    if (!tPlayer.bIsPrimaryPlayerServerOnly)
        return FALSE;

    return TRUE;
}

void PlayerListGetPrimaryPlayers(PlayerList & listPlayers)
{
    sBuildPlayerList(listPlayers, TRUE, sPlayerFilterPrimaryPlayers, NULL);
}
