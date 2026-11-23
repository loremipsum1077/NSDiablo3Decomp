// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Server/PlayerList.h

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "../Sigma/DataStructures/XPooledList.h"
#include "../Common/CommonTypes.h"

struct Player;
struct WorldPlace;

typedef int32 GameConnectionID;

typedef XListMemoryPool<Player *> TPlayerPointerPool;

class PlayerList : public XPooledList<Player *, Player *> {  // size 0x28
public:
    PlayerList(TPlayerPointerPool *);
};
static_assert(sizeof(PlayerList) == 0x28, "PlayerList");

// Declared here from their definitions in PlayerList.cpp.
TPlayerPointerPool * PlayerListGetElementPool();  // PlayerList.cpp:95
void PlayerListGetAllInGame(PlayerList & listPlayers, const int32 nPlayerIndexIgnore,
                            const BOOL bIgnoreHardcoreDeadPlayers,
                            const GameConnectionID idGameConnectionToMatch);  // PlayerList.cpp:343
void PlayerListGetAllInGameRemote(PlayerList & listPlayers,
                                  const GameConnectionID idGameConnectionToExclude,
                                  const int32 nPlayerIndexIgnore,
                                  const BOOL bIgnoreHardcoreDeadPlayers);  // PlayerList.cpp:413
void PlayerListGetSingle(PlayerList & listPlayers, const Player & tPlayer);  // PlayerList.cpp:454
void PlayerListGetSingle(PlayerList & listPlayers, const ACDID idACDPlayer);  // PlayerList.cpp
void PlayerListGetThatKnowACD(PlayerList & listPlayers, const ACDID idACD, BOOL bIgnoreACD);  // PlayerList.cpp:554
void PlayerListGetInKnowRegion(PlayerList & listPlayers, const WorldPlace & tPlace);  // PlayerList.cpp:516

void PlayerListGetAllConnected(PlayerList & listPlayers, const int32 nPlayerIndexIgnore,
                               BOOL bIgnoreHardcoreDeadPlayers);  // PlayerList.cpp:246

// Declaration placement inferred from the owning API; DWARF definition: PlayerList.cpp:402.
void PlayerListGetAllPlayersLocalToPlayer(PlayerList & listPlayers, const Player * ptPlayer);
