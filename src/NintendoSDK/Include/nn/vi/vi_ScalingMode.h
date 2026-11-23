// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: NintendoSDK/Include/nn/vi/vi_ScalingMode.h

#pragma once

namespace nn {
namespace vi {
enum ScalingMode {
    ScalingMode_None = 0,
    ScalingMode_Exact = 1,
    ScalingMode_FitToLayer = 2,
    ScalingMode_ScaleAndCrop = 3,
    ScalingMode_PreserveAspectRatio = 4,
};

}  // namespace vi
}  // namespace nn
