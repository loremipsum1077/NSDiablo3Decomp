// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: NintendoSDK/Include/nn/account/account_Types.h

#pragma once

#include "../nn_BitTypes.h"
#include <stddef.h>
#include <stdint.h>

namespace nn {
namespace account {
struct Uid {  // size 0x10
    nn::Bit64 _data[2];  // 0x00
    operator bool() const;
};
static_assert(sizeof(Uid) == 0x10, "Uid");

struct UserHandle {  // size 0x18
    nn::Bit64 _data[2];  // 0x00
    uintptr_t _context;  // 0x10
};
static_assert(sizeof(UserHandle) == 0x18, "UserHandle");

struct Nickname {  // size 0x21
    char name[33];  // 0x00
};
static_assert(sizeof(Nickname) == 0x21, "Nickname");

struct NetworkServiceAccountId {  // size 0x8
    uint64_t id;  // 0x00
};
static_assert(sizeof(NetworkServiceAccountId) == 0x8, "NetworkServiceAccountId");

// account_Types.h:83 and 163-172.
static const Uid InvalidUid = {{0, 0}};
inline bool operator==(const Uid & lhs, const Uid & rhs) {
    return lhs._data[0] == rhs._data[0] && lhs._data[1] == rhs._data[1];
}
inline bool operator!=(const Uid & lhs, const Uid & rhs) {
    return !(lhs == rhs);
}

}  // namespace account
}  // namespace nn

