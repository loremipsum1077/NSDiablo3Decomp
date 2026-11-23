// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/Console/PresenceManager.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Contrib/Contrib/DemonWare/bdLobby/bdLobbyCommon/bdLobbyServicesCommon.h"
#include "../../../Contrib/Contrib/blz/include/blz/embedded_string.h"
#include "../../../Contrib/Contrib/blz/include/blz/map.h"
#include "../../../Contrib/Contrib/blz/include/blz/vector.h"
#include "Online.h"

#include "../../Common/OnlineService/Callback/Callback_Impl.h"

class bdRichPresenceData;

namespace Console {
namespace Online {
// Online.h:289-291.
typedef blz::map<unsigned long, blz::basic_embedded_string<char, 32, blz::char_traits<char>, blz::allocator<char> >, blz::less<unsigned long>, blz::allocator<blz::pair<const unsigned long, blz::basic_embedded_string<char, 32, blz::char_traits<char>, blz::allocator<char> > > > > PresenceProfileNameMap;
typedef OnlineService::Callback<void (int, const Console::Online::PresenceProfileNameMap &)> PresenceGetProfileNamesCallback;

class PresenceManager {  // size 0x8
    // vtable pointer
public:
    static void Create();
    static Console::Online::PresenceManager * Get(int32 nUserIdx);
    static void Idle();
    static void SetListingEnabled(BOOL bEnabled);
    static void SetSessionAndGamePresence(const Console::Online::PartyConnectionInfo & tConnectionInfo, const Console::Online::GamePresence & tGamePresence);
    static void SetPresenceDirty();
    virtual uint32 GetAllFriends(Console::Online::Friend *, uint32) = 0;  // vtable slot 0
    virtual BOOL HasAnyFriends() = 0;  // vtable slot 1
    virtual uint32 GetOnlineFriends(Console::Online::Friend *, uint32) = 0;  // vtable slot 2
    virtual blz::vector<unsigned long, blz::allocator<unsigned long> > PresenceGetFriendIDs() = 0;  // vtable slot 3
    virtual BOOL GetRandomFriend(Console::Online::Friend &, bdOnlineUserID) = 0;  // vtable slot 4
    virtual Console::Online::Friend * GetFriend(OnlineUserId) = 0;  // vtable slot 5
    virtual BOOL HasFriendByDemonWareID(bdOnlineUserID) = 0;  // vtable slot 6
    virtual void GetProfileNames(const blz::vector<unsigned long, blz::allocator<unsigned long> > &, Console::Online::PresenceGetProfileNamesCallback) = 0;  // vtable slot 7
    virtual void OnNX64UserSignedIn() = 0;  // vtable slot 8
    virtual void OnNX64UserSignedOut() = 0;  // vtable slot 9
    virtual const Console::Online::PartyConnectionInfo & GetConnectionInfo() = 0;  // vtable slot 10
    virtual void OnRichPresenceUpdateReceived(const bdRichPresenceData &) = 0;  // vtable slot 11
};
static_assert(sizeof(PresenceManager) == 0x8, "PresenceManager");

}  // namespace Online
}  // namespace Console
