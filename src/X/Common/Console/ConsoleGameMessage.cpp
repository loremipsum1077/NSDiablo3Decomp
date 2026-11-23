// Original file: X/Common/Console/ConsoleGameMessage.cpp

#include "../CommonTypes.h"
#include "../Networking/GameMessage.h"
#include "../Player.h"

namespace Console {
namespace Server {

BOOL MessageRecieve(
    int32 nPlayerIndex,
    void * pMessage)
{
    Player & tPlayerFrom = *PlayerGetByPlayerIndex(nPlayerIndex);

    const RequiredMessageHeader * ptHeader = (const RequiredMessageHeader *)pMessage;
    return ptHeader->eType == SMSG_SET_KEEP_TOC_LOADED;
}

}  // namespace Server

namespace Client {

BOOL MessageRecieve(
    void * pMessage)
{
    const RequiredMessageHeader * ptHeader = (const RequiredMessageHeader *)pMessage;
    return ptHeader->eType == SMSG_SET_KEEP_TOC_LOADED;
}

}  // namespace Client
}  // namespace Console
