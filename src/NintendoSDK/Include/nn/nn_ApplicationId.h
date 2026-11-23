// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: NintendoSDK/Include/nn/nn_ApplicationId.h

#pragma once

#include "nn_BitTypes.h"

namespace nn {
struct ApplicationId {  // size 0x8
    nn::Bit64 value;  // 0x00
    static nn::ApplicationId GetInvalidId();
};
static_assert(sizeof(ApplicationId) == 0x8, "ApplicationId");

}  // namespace nn
