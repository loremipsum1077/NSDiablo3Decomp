// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Trade.h

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "CommonTypes.h"
#include "../Sigma/SigmaTypes.h"

struct PlayerTradeData {  // size 0x30
    ACDID idACDPlayerTradingWith;  // 0x00
    BOOL bTradeAccepted;  // 0x04
    ACDID idACDPlayerWantToTradeWith;  // 0x08
    uint32 uNextAcceptTime;  // 0x0c
    uint64 uMyGoldOffer;  // 0x10
    uint32 uAcceptedHashValue;  // 0x18
    ACDNetworkName arrItems[4];  // 0x1c
};
static_assert(sizeof(PlayerTradeData) == 0x30, "PlayerTradeData");

enum {
    TRADE_RESPONSE_OK = 0,
    TRADE_RESPONSE_BUSY = 1,
    TRADE_RESPONSE_NOT_ALIVE = 2,
    TRADE_RESPONSE_TOO_FAR = 3,
    TRADE_RESPONSE_RESTRICTED = 4,
    TRADE_RESPONSE_ERROR = 5,
};

typedef int32 TradeResponse;

