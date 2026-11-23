// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: NintendoSDK/Include/nn/friends/friends_Profile.h

#pragma once

#include "friends_Types.h"
#include "../util/util_TypedStorage.h"

namespace nn { class Result; }
namespace nn { namespace account { struct NetworkServiceAccountId; } }
namespace nn { namespace account { struct Nickname; } }
namespace nn { namespace friends { struct Url; } }

namespace nn {
namespace friends {
class Profile {  // size 0x100
public:
    Profile();
    nn::account::NetworkServiceAccountId GetAccountId() const;
    const nn::account::Nickname & GetNickname() const;
    nn::Result GetProfileImageUrl(nn::friends::Url *, nn::friends::ImageSize) const;
    bool IsValid() const;
private:
    nn::util::TypedStorage<unsigned long, 256, 8> m_Storage;  // 0x00
};
static_assert(sizeof(Profile) == 0x100, "Profile");

}  // namespace friends
}  // namespace nn
