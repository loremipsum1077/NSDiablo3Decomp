// Original file: X/Client/Console/GamerProfile.h
// Subset: profile access and save entry points used by the game-settings screen.
#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Common/OnlineService/Callback/Callback_Impl.h"

namespace D3 { namespace Account { class SavedDefinition; } }
namespace D3 { namespace Hero { class SavedDefinition; } }
namespace nn { namespace account { struct NetworkServiceAccountId; } }

namespace Console { namespace GamerProfile {
typedef OnlineService::Callback<void()> SaveCallback;
D3::Account::SavedDefinition & GetAccount(int32 nUserIdx);
D3::Hero::SavedDefinition & GetHero(int32 nUserIdx);
void Save(int32 nUserIdx, SaveCallback pfnCallback);
int32 GetPrimaryProfileUserIndex();
const char * GetGamerTag(int32 nUserIdx);  // GamerProfileNX64.cpp:1116
BOOL IsSignedIn(int32 nUserIdx);  // GamerProfileNX64.cpp:679
BOOL IsActive(int32 nUserIdx);  // GamerProfileNX64.cpp
BOOL GetUserNetworkServiceAccountId(int32 nUserIndex, nn::account::NetworkServiceAccountId & idOutUserNSA);  // GamerProfileNX64.cpp:745
} }

// GamerProfileNX64.cpp: the app subsystem entry points and the per-frame update (X/Common/Game.cpp).
namespace Console { namespace GamerProfile {
void InitializeForApp();
void DisposeForApp();
void Process();
} }

namespace Console { namespace GamerProfile {
BOOL IsHeroLoaded(int32 nUserIdx);  // GamerProfileNX64.cpp:1358
} }

namespace Console { namespace GamerProfile {
BOOL IsSignedInOnline(int32 nUserIdx);  // GamerProfileNX64.cpp:692
BOOL IsGuest(int32 nUserIdx);  // GamerProfileNX64.cpp:926
} }

namespace D3 { namespace Console { class Profile; } }
namespace Console { namespace GamerProfile {
D3::Console::Profile & GetProfile(int32 nUserIdx);  // GamerProfileNX64.cpp:1192
} }



#include "../../../NintendoSDK/Include/nn/friends/friends_Types.h"
#include "../../Sigma/Memory/SigmaMemory.h"





























































































































































































































namespace Console { namespace GamerProfile {

struct PlayerHistoryRecord {

    nn::friends::PlayHistoryRegistrationKey m_tRegistrationKey;
    char m_szName[32];

    PlayerHistoryRecord()
    {
        SigmaMemoryClear(&m_tRegistrationKey, sizeof(m_tRegistrationKey));
        SigmaMemoryClear(m_szName, sizeof(m_szName));
    }

    PlayerHistoryRecord(nn::friends::PlayHistoryRegistrationKey & tRegistrationKey, char * szName)
    {
        uint32 SizeOfRegistrationValue = sizeof(m_tRegistrationKey);
        SigmaMemoryCopy(&m_tRegistrationKey, &tRegistrationKey, SizeOfRegistrationValue);
        SigmaMemoryCopy(m_szName, szName, sizeof(m_szName));
    }

    PlayerHistoryRecord(PlayerHistoryRecord & other)
    {
        uint32 SizeOfRegistrationValue = sizeof(m_tRegistrationKey);
        SigmaMemoryCopy(&m_tRegistrationKey, &other.m_tRegistrationKey, SizeOfRegistrationValue);
        SigmaMemoryCopy(m_szName, other.m_szName, sizeof(m_szName));
    }

    PlayerHistoryRecord & operator=(const PlayerHistoryRecord & from)
    {
        uint32 SizeOfRegistrationValue = sizeof(m_tRegistrationKey);
        SigmaMemoryCopy(&m_tRegistrationKey, &from.m_tRegistrationKey, SizeOfRegistrationValue);
        SigmaMemoryCopy(m_szName, from.m_szName, sizeof(m_szName));
        return *this;
    }


    inline BOOL operator==(const PlayerHistoryRecord & rhs);
};






enum OnlinePlayPrivilege {
    ONLINE_PLAY_ALLOWED = 0,
    ONLINE_PLAY_BLOCKED_NOT_SIGNED_IN_LOCALLY = 1,
    ONLINE_PLAY_BLOCKED_NOT_SIGNED_IN_ONLINE = 2,
    ONLINE_PLAY_BLOCKED_NO_PRIVILEGE = 3,
    ONLINE_PLAY_BLOCKED_PARENTAL_CONTROLS = 4,
    ONLINE_PLAY_BLOCKED_BANNED = 5,
    ONLINE_PLAY_BLOCKED_REQUIRES_PREMIUM = 6,
    ONLINE_PLAY_BLOCKED_UPDATE_REQUIRED = 7,
    ONLINE_PLAY_BLOCKED_OTHER = 8,
};
OnlinePlayPrivilege GetOnlinePlayPrivilege(int32 nUserIdx);  // defined GamerProfileNX64.cpp:1025; header line not in the DWARF

BOOL UpdatePlayHistoryRecord(int32 nUserIndex, PlayerHistoryRecord * atPlayerHistoryRecords, uint32 uNumPlayerHistoryRecords);
} }
namespace Console { namespace GamerProfile {
inline BOOL CanPlayOnline(
    int32 nUserIdx)
{
    return GetOnlinePlayPrivilege(nUserIdx) == ONLINE_PLAY_ALLOWED;
}
} }

// Layout checks generated from the DWARF by tools/layout_asserts.py.
#include <stddef.h>
static_assert(sizeof(Console::GamerProfile::PlayerHistoryRecord) == 0x60, "Console::GamerProfile::PlayerHistoryRecord");
static_assert(offsetof(Console::GamerProfile::PlayerHistoryRecord, m_tRegistrationKey) == 0x0, "Console::GamerProfile::PlayerHistoryRecord");
static_assert(offsetof(Console::GamerProfile::PlayerHistoryRecord, m_szName) == 0x40, "Console::GamerProfile::PlayerHistoryRecord");
