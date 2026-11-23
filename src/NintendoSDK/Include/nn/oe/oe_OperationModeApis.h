// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: NintendoSDK/Include/nn/oe/oe_OperationModeApis.h

#pragma once

namespace nn {
namespace oe {
enum OperationMode {
    OperationMode_Handheld = 0,
    OperationMode_Console = 1,
};

// Not in the DWARF.
OperationMode GetOperationMode();

}  // namespace oe
}  // namespace nn
