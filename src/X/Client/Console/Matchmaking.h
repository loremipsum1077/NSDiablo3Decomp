// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/Console/Matchmaking.h
// The original includes ScreenManager.h and UIUtil.h (not in src/ yet); GameType and Act come
// from GameTemplate.h and GameSwig.h here instead.

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Contrib/Contrib/DemonWare/bdCore/bdSocket/bdCommonAddr.h"
#include "../../../Contrib/Contrib/DemonWare/bdCore/bdSocket/bdSecurityID.h"
#include "../../../Contrib/Contrib/DemonWare/bdCore/bdSocket/bdSecurityKey.h"
#include "../../Common/GameSwig.h"
#include "../../Common/GameTemplate.h"
#include "../../Common/OnlineService/Callback/Callback_Impl.h"
#include "Online.h"

class bdDMLInfo;

namespace Console {
namespace Online {
enum MatchmakerRegion {
    REGION_DEFAULT = 0,
    REGION_NORTH_AMERICA = 1,
    REGION_SOUTH_AMERICA = 2,
    REGION_EUROPE = 3,
    REGION_RUSSIA = 4,
    REGION_ASIA = 5,
    REGION_AUSTRALIA = 6,
    REGION_AFRICA = 7,
    NUM_REGIONS = 8,
};

struct MatchParameters {  // size 0x34
    MatchParameters();
    BOOL bIsPublic;  // 0x00
    GameType eGameType;  // 0x04
    int32 nLevel;  // 0x08
    BOOL bHardcore;  // 0x0c
    BOOL bSeasonal;  // 0x10
    Act eAct;  // 0x14
    SNO snoQuest;  // 0x18
    int32 nHandicap;  // 0x1c
    uint32 uNumPlayers;  // 0x20
    Console::Online::MatchmakerRegion eRegion;  // 0x24
    int32 nLanguage;  // 0x28
    int32 nTag;  // 0x2c
    BOOL bBadReputation;  // 0x30
};
static_assert(sizeof(MatchParameters) == 0x34, "MatchParameters");

// line 64: the implicit constructor and destructor are inlined into GetSearchResults' caller.
struct MatchmakerResults {  // size 0x58
    bdCommonAddrRef ptAddress;  // 0x00
    bdSecurityID tSecID;  // 0x08
    bdSecurityKey tSecKey;  // 0x10
    Console::Online::MatchParameters tParams;  // 0x20
};
static_assert(sizeof(MatchmakerResults) == 0x58, "MatchmakerResults");

typedef OnlineService::Callback<void (Console::Online::MMSearchResult)> SearchCallback;

typedef OnlineService::Callback<void (bdDMLInfo &)> DMLCallback;

class Matchmaker {  // size 0x8
    // vtable pointer
public:
    static void Create();
    static Console::Online::Matchmaker * Get();
    static void Idle();
    virtual BOOL IsRegistered() = 0;  // vtable slot 0
    virtual void Search(Console::Online::MatchParameters &, Console::Online::SearchCallback) = 0;  // vtable slot 1
    virtual BOOL IsSearching() = 0;  // vtable slot 2
    virtual uint32 GetSearchResults(Console::Online::MatchmakerResults *, uint32) = 0;  // vtable slot 3
    virtual void Cancel() = 0;  // vtable slot 4
    virtual void RegisterGame(const bdSecurityID &, const bdSecurityKey &, const Console::Online::MatchParameters &) = 0;  // vtable slot 5
    virtual void UnregisterGame() = 0;  // vtable slot 6
    virtual void UpdateGame(const bdSecurityID &, const bdSecurityKey &, const Console::Online::MatchParameters &) = 0;  // vtable slot 7
    virtual Console::Online::MatchmakerRegion GetCurrentRegion(Console::Online::MatchmakerRegion) = 0;  // vtable slot 8
};
static_assert(sizeof(Matchmaker) == 0x8, "Matchmaker");

}  // namespace Online
}  // namespace Console
