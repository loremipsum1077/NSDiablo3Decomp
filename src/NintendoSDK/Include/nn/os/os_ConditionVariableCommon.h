// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: NintendoSDK/Include/nn/os/os_ConditionVariableCommon.h

#pragma once

namespace nn {
namespace os {
enum ConditionVariableStatus {
    ConditionVariableStatus_Timeout = 0,
    ConditionVariableStatus_NoTimeout = 1,
};

}  // namespace os
}  // namespace nn
