// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: NintendoSDK/Include/nn/os/os_ThreadCommon.h

#pragma once

namespace nn {
namespace os {
typedef void (*ThreadFunction)(void *);

}  // namespace os
}  // namespace nn

// Variables defined in this file:
//   line    33  static const int nn::os::ThreadNameLengthMax
//   line    47  static const int nn::os::DefaultThreadPriority
//   line    52  static const int nn::os::LowestThreadPriority
//   line    89  static const size_t nn::os::ThreadStackAlignment
