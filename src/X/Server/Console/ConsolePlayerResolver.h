// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Server/Console/ConsolePlayerResolver.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Common/Console/Networking/Connection.h"
#include "../OnlineService/PlayerResolver.h"

namespace Console {
class PlayerResolver : public OnlineService::PlayerResolver {  // size 0xcb8
public:
    PlayerResolver();
    virtual ~PlayerResolver();  // vtable slot 0
    BOOL LoadFromParty(Console::GameNetwork::ConnectionRef ptConnection);
private:
    virtual void ResolvePlayerData(uint32 uIndex);  // vtable slot 2
};
static_assert(sizeof(PlayerResolver) == 0xcb8, "PlayerResolver");

}  // namespace Console
