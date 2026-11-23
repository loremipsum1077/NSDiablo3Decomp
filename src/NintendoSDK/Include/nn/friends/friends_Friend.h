// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: NintendoSDK/Include/nn/friends/friends_Friend.h

#pragma once

#include "../util/util_TypedStorage.h"
#include <stddef.h>
#include <stdint.h>

namespace nn { class Result; }
namespace nn { namespace account { struct NetworkServiceAccountId; } }
namespace nn { namespace account { struct Nickname; } }
namespace nn { namespace friends { class FriendPresence; } }

namespace nn {
namespace friends {
class Friend {  // size 0x200
public:
    Friend();
    nn::account::NetworkServiceAccountId GetAccountId() const;
    const nn::account::Nickname & GetNickname() const;
    nn::Result GetProfileImage(size_t *, void *, size_t) const;
    const nn::friends::FriendPresence & GetPresence() const;
    bool IsFavorite() const;
    bool IsNewly() const;
    nn::Result Update();
    bool IsValid() const;
private:
    nn::util::TypedStorage<unsigned long, 512, 8> m_Storage;  // 0x00
};
static_assert(sizeof(Friend) == 0x200, "Friend");

}  // namespace friends
}  // namespace nn
