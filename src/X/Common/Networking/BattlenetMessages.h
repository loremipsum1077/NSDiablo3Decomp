// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Networking/BattlenetMessages.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../CommonTypes.h"
#include "../Game.h"
#include "../OnlineService/OnlineService.h"

struct JoinBNetGameMessage {  // size 0x30
    JoinBNetGameMessage();
    RequiredMessageHeader tHeader;  // 0x00
    OnlineService::HeroId uHeroId;  // 0x08
    SGameID idSGame;  // 0x10
    uint64 uAuthToken;  // 0x18
    XLocale eXLocale;  // 0x20
    uint32 uProtocolHash;  // 0x24
    uint32 dwSNOCoreDataHash;  // 0x28
};
static_assert(sizeof(JoinBNetGameMessage) == 0x30, "JoinBNetGameMessage");
