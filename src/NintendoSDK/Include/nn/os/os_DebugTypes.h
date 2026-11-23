// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: NintendoSDK/Include/nn/os/os_DebugTypes.h

#pragma once

#include <stddef.h>
#include <stdint.h>

namespace nn {
namespace os {
struct MemoryInfo {  // size 0x38
    uint64_t totalAvailableMemorySize;  // 0x00
    size_t totalUsedMemorySize;  // 0x08
    size_t totalMemoryHeapSize;  // 0x10
    size_t allocatedMemoryHeapSize;  // 0x18
    size_t programSize;  // 0x20
    size_t totalThreadStackSize;  // 0x28
    int threadCount;  // 0x30
};
static_assert(sizeof(MemoryInfo) == 0x38, "MemoryInfo");

}  // namespace os
}  // namespace nn
