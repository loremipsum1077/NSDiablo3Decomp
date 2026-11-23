// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: NintendoSDK/Include/nn/ldn/ldn_Ipv4Address.h

#pragma once

#include "../nn_BitTypes.h"

namespace nn {
namespace ldn {
struct Ipv4Address {  // size 0x4
    nn::Bit32 raw;  // 0x00
};
static_assert(sizeof(Ipv4Address) == 0x4, "Ipv4Address");

// Defined in the SDK library (called through the PLT).
Ipv4Address MakeIpv4Address(Bit8 a, Bit8 b, Bit8 c, Bit8 d);

static const Ipv4Address LocalLoopbackIpv4Address = MakeIpv4Address(127, 0, 0, 1);  // line 153
static const Ipv4Address BroadcastIpv4Address = MakeIpv4Address(255, 255, 255, 255);  // line 158
static const Ipv4Address ZeroIpv4Address = MakeIpv4Address(0, 0, 0, 0);  // line 163
}  // namespace ldn
}  // namespace nn
