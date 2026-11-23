// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: BlizzardCore/Source/System/File/FilePriv.h

#pragma once

#include "../../../Include/Blizzard/File.h"
#include "../../../Include/Blizzard/StorageInfo.h"
#include "../../../Include/Blizzard/Types.h"
#include "../../../../NintendoSDK/Include/nn/fs/fs_File.h"
#include <stddef.h>
#include <stdint.h>

namespace Blizzard { namespace File { struct StreamingInfo; } }

namespace Blizzard {
namespace File {
struct StreamRecord {  // size 0x60
    nn::fs::FileHandle fileHandle;  // 0x00
    int mode;  // 0x08
    bool infoValid;  // 0x0c
    Blizzard::File::FileInfo info;  // 0x10
    Blizzard::File::StreamingInfo * streaming;  // 0x50
    char * pathname;  // 0x58
    void printInfo(char *, size_t) const;
};
static_assert(sizeof(StreamRecord) == 0x60, "StreamRecord");

struct StreamingInfo {  // size 0x58
    Blizzard::Storage::StorageUnitHandle storagenode;  // 0x00
    Blizzard::File::INODE inode;  // 0x10
    int32 streamRefCount;  // 0x14
    Blizzard::File::FileInfo streaminginfo;  // 0x18
};
static_assert(sizeof(StreamingInfo) == 0x58, "StreamingInfo");

}  // namespace File
}  // namespace Blizzard

// Functions defined in this file (bodies are not in the DWARF):
//   line   105  Blizzard::File::FileSystemPtr Blizzard::File::GetFilesystem<false>()  [inlined into 21 functions]
