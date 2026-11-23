// Adopted from the DWARF reference; hand-maintained.
// Original file: NintendoSDK/Include/nn/ldn/ldn_NetworkId.h

#pragma once

#include "../nn_BitTypes.h"

namespace nn {
namespace ldn {
struct IntentId {  // size 0x10
    nn::Bit64 localCommunicationId;  // 0x00
    nn::Bit8 _reserved1[2];  // 0x08
    nn::Bit16 sceneId;  // 0x0a
    nn::Bit8 _reserved2[4];  // 0x0c
};
static_assert(sizeof(IntentId) == 0x10, "IntentId");

struct SessionId {  // size 0x10
    nn::Bit8 random[16];  // 0x00
};
static_assert(sizeof(SessionId) == 0x10, "SessionId");

struct NetworkId {  // size 0x20
    nn::ldn::IntentId intentId;  // 0x00
    nn::ldn::SessionId sessionId;  // 0x10
};
static_assert(sizeof(NetworkId) == 0x20, "NetworkId");

}  // namespace ldn
}  // namespace nn
