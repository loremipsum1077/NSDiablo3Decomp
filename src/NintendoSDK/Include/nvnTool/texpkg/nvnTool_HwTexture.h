// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: NintendoSDK/Include/nvnTool/texpkg/nvnTool_HwTexture.h

#pragma once

// NVNpackagedTextureLayout and NVNboolean come from the including file (NX64NVNCore.cpp:16
// declares the typedef before including this header).
#include <stddef.h>
#include <stdint.h>

namespace nvnTool {
namespace texpkg {
struct NVNHWTextureHeader {  // size 0x78
    uint64_t dataSize;  // 0x00
    uint32_t align;  // 0x08
    uint32_t width;  // 0x0c
    uint32_t height;  // 0x10
    uint32_t depth;  // 0x14
    uint32_t target;  // 0x18
    uint32_t format;  // 0x1c
    uint32_t mipmapLevels;  // 0x20
    uint32_t subImagePitch;  // 0x24
    uint32_t mipmapLevelOffset[17];  // 0x28
    NVNpackagedTextureLayout layout;  // 0x6c
    NVNboolean sparse;  // 0x74
};
static_assert(sizeof(NVNHWTextureHeader) == 0x78, "NVNHWTextureHeader");

}  // namespace texpkg
}  // namespace nvnTool
