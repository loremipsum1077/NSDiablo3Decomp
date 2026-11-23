// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: NintendoSDK/Include/nn/ldn/ldn_MacAddress.h

#pragma once

#include "../nn_BitTypes.h"

namespace nn {
namespace ldn {
struct MacAddress {  // size 0x6
    nn::Bit8 raw[6];  // 0x00
};
static_assert(sizeof(MacAddress) == 0x6, "MacAddress");

// Invented helper: the SDK builds these two constants at run time (the binary stores them from
// every initializer that includes this header); its own spelling is not in the DWARF.
inline MacAddress MakeMacAddress(Bit8 a, Bit8 b, Bit8 c, Bit8 d, Bit8 e, Bit8 f) {
    MacAddress tAddress = {{a, b, c, d, e, f}};
    return tAddress;
}

static const MacAddress ZeroMacAddress = MakeMacAddress(0, 0, 0, 0, 0, 0);  // line 101
static const MacAddress BroadcastMacAddress = MakeMacAddress(0xff, 0xff, 0xff, 0xff, 0xff, 0xff);  // line 106
}  // namespace ldn
}  // namespace nn
