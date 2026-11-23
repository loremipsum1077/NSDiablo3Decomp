// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Server/Network/ServerMessageSend.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Common/CommonTypes.h"

typedef int32 UIElement;

typedef int32 UIElementAction;

typedef int32 KnownEntryListID;

// Declared here from its definition in X/Server/Network/ServerMessageSend.cpp.
void ServerMessageSendForceInterfaceClose(const ACDID idACDPlayer);  // ServerMessageSend.cpp:1450

#include "../../Common/Networking/GameMessage.h"
class PlayerList;
// Declaration placement inferred from the owning API; DWARF definition: ServerMessageSend.cpp:1414.
void SendPlayerWarped(const PlayerList & listDestPlayers, const PlayerWarpReason eReason, const float flWarpFadeInSeconds);

// No header declaration recovered; owning-header placement inferred from ServerMessageSend.cpp:694.
void ServerMessageSendWorldRevealToPlayer(const PlayerList & listDestPlayers, const SWorldID idSWorld);
