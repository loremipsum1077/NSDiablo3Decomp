// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: NintendoSDK/Include/nn/fs/fs_Directory.h

#pragma once

#include "../nn_Result.h"

#include <stddef.h>
#include <stdint.h>

namespace nn {
namespace fs {
struct DirectoryHandle {  // size 0x8
    void * handle;  // 0x00
};
static_assert(sizeof(DirectoryHandle) == 0x8, "DirectoryHandle");

struct DirectoryEntry {  // size 0x310
    char name[769];  // 0x00
    char reserved1[3];  // 0x301
    int8_t directoryEntryType;  // 0x304
    char reserved2[3];  // 0x305
    int64_t fileSize;  // 0x308
};
static_assert(sizeof(DirectoryEntry) == 0x310, "DirectoryEntry");

// Prototypes from the SDK; the functions are imports, so nothing in the image checks them.
nn::Result CreateDirectory(const char * path);
nn::Result DeleteDirectory(const char * path);
nn::Result DeleteDirectoryRecursively(const char * path);
nn::Result OpenDirectory(nn::fs::DirectoryHandle * outValue, const char * path, int mode);
void CloseDirectory(nn::fs::DirectoryHandle handle);
nn::Result GetDirectoryEntryCount(int64_t * outValue, nn::fs::DirectoryHandle handle);
nn::Result ReadDirectory(int64_t * outValue, nn::fs::DirectoryEntry * entryBuffer, nn::fs::DirectoryHandle handle, int64_t count);

}  // namespace fs
}  // namespace nn
