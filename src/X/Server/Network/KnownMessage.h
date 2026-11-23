// X/Server/Network/KnownMessage.h -- the per-ACD "known message" queue: a
// message stamped with an actor is delivered to exactly the players that
// already know that actor.
#pragma once

#include "../../Common/CommonTypes.h"

class ActorCommonData;
class PlayerList;
struct KnownMessage;

void KnownMessageSend(const PlayerList & listDestPlayers, const KnownMessage & tKnownMessage);  // KnownMessage.cpp:26

void KnownMessageAdd(ActorCommonData & tACD, const GameMessageType eMessageType, const void * ptMessage,
                     const uint32 dwSize, const PlayerList * ptDestPlayers);  // KnownMessage.cpp:43
