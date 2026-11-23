// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: NintendoSDK/Include/nn/friends/friends_Presence.h

#pragma once

#include "friends_Types.h"
#include "../util/util_TypedStorage.h"

namespace nn { namespace friends { struct ApplicationInfo; } }
namespace nn { namespace time { struct PosixTime; } }

namespace nn {
namespace friends {
class FriendPresence {  // size 0xe0
public:
    FriendPresence();
    nn::friends::PresenceStatus GetStatus() const;
    const nn::friends::ApplicationInfo & GetLastPlayedApplication() const;
    const nn::time::PosixTime & GetLastOnlineTime() const;
    const nn::time::PosixTime & GetLastUpdateTime() const;
    const char * GetDescription() const;
    const char * GetAppValue(const char *) const;
    bool IsSamePresenceGroupApplication() const;
private:
    nn::util::TypedStorage<unsigned long, 224, 8> m_Storage;  // 0x00
};
static_assert(sizeof(FriendPresence) == 0xe0, "FriendPresence");

}  // namespace friends
}  // namespace nn
