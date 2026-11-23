// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: NintendoSDK/Include/nn/fs/fs_File.h

#pragma once

#include "../nn_Result.h"

#include <stddef.h>
#include <stdint.h>

namespace nn {
namespace fs {
enum WriteOptionFlag {
    WriteOptionFlag_Flush = 1,
};

struct WriteOption {  // size 0x4
    int flags;  // 0x00
    static nn::fs::WriteOption MakeValue(int flags)
    {
        nn::fs::WriteOption option = { flags };
        return option;
    }
};
static_assert(sizeof(WriteOption) == 0x4, "WriteOption");

struct FileHandle {  // size 0x8
    void * handle;  // 0x00
};
static_assert(sizeof(FileHandle) == 0x8, "FileHandle");

// Prototypes from the SDK; the functions are imports, so nothing in the image checks them.
nn::Result CreateFile(const char * path, int64_t size);
nn::Result DeleteFile(const char * path);
nn::Result RenameFile(const char * currentPath, const char * newPath);
void CloseFile(nn::fs::FileHandle handle);
nn::Result OpenFile(nn::fs::FileHandle * outValue, const char * path, int mode);
nn::Result GetFileSize(int64_t * outValue, nn::fs::FileHandle handle);
nn::Result ReadFile(size_t * outValue, nn::fs::FileHandle handle, int64_t offset, void * buffer, size_t size);
nn::Result WriteFile(nn::fs::FileHandle handle, int64_t offset, const void * buffer, size_t size, const nn::fs::WriteOption & option);

// fs_Result.public.h: module 2, description 7. The class is not in the DWARF; the test is the
// one inlined into System_File::sIsFile.
struct ResultTargetLocked {
    static bool Includes(const nn::Result & result) { return (result.GetInnerValueForDebug() & 0x3fffff) == 0xe02; }
};

}  // namespace fs
}  // namespace nn
