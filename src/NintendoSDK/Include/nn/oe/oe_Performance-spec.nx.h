// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: NintendoSDK/Include/nn/oe/oe_Performance-spec.nx.h

#pragma once

namespace nn {
namespace oe {
enum PerformanceMode {
    PerformanceMode_Invalid = -1,
    PerformanceMode_Normal = 0,
    PerformanceMode_Boost = 1,
};

typedef int PerformanceConfiguration;

// Values are not in the DWARF; taken from ShellInitialize (boost mode gets the faster GPU).
static const PerformanceConfiguration PerformanceConfiguration_Cpu1020MhzGpu768MhzEmc1600Mhz = 0x00010001;
static const PerformanceConfiguration PerformanceConfiguration_Cpu1020MhzGpu384MhzEmc1331Mhz = 0x00020004;

// Not in the DWARF.
void SetPerformanceConfiguration(PerformanceMode ePerformanceMode, PerformanceConfiguration eConfiguration);
PerformanceMode GetPerformanceMode();

}  // namespace oe
}  // namespace nn

// Variables defined in this file:
//   line    59  static const nn::oe::PerformanceConfiguration nn::oe::PerformanceConfiguration_Cpu1020MhzGpu768MhzEmc1600Mhz
//   line    72  static const nn::oe::PerformanceConfiguration nn::oe::PerformanceConfiguration_Cpu1020MhzGpu384MhzEmc1331Mhz
