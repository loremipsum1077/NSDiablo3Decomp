// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: NintendoSDK/Include/nvnTool/texpkg/nvnTool_Fd.h

#pragma once

#include <stddef.h>
#include <stdint.h>

namespace nvnTool {
namespace texpkg {
struct ArchiveHeader {  // size 0x10
    uint32_t magic;  // 0x00
    uint32_t headerSize;  // 0x04
    uint32_t majorVersion;  // 0x08
    uint32_t minorVersion;  // 0x0c
};
static_assert(sizeof(ArchiveHeader) == 0x10, "ArchiveHeader");

struct BlockHeader {  // size 0x28
    uint32_t magic;  // 0x00
    uint32_t blockHeaderSize;  // 0x04
    uint64_t blockSize;  // 0x08
    uint64_t offset;  // 0x10
    int32_t blockType;  // 0x18
    uint32_t index;  // 0x1c
    uint32_t typeIndex;  // 0x20
};
static_assert(sizeof(BlockHeader) == 0x28, "BlockHeader");

}  // namespace texpkg
}  // namespace nvnTool
