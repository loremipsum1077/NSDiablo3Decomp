// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: NintendoSDK/Include/nn/fs/fs_FileSystem.h

#pragma once

namespace nn {
namespace fs {
enum OpenMode {
    OpenMode_Read = 1,
    OpenMode_Write = 2,
    OpenMode_AllowAppend = 4,
};

enum OpenDirectoryMode {
    OpenDirectoryMode_Directory = 1,
    OpenDirectoryMode_File = 2,
    OpenDirectoryMode_All = 3,
};

enum DirectoryEntryType {
    DirectoryEntryType_Directory = 0,
    DirectoryEntryType_File = 1,
};

}  // namespace fs
}  // namespace nn
