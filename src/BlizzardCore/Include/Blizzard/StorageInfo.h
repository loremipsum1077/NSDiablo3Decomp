// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: BlizzardCore/Include/Blizzard/StorageInfo.h

#pragma once

#include "File.h"

namespace Blizzard { namespace Storage { struct StorageUnit; } }

namespace Blizzard {
namespace Storage {
class StorageUnitHandle {  // size 0x10
public:
    StorageUnitHandle();
    StorageUnitHandle(Blizzard::File::Stream);
    StorageUnitHandle(Blizzard::File::INODE, bool);
    StorageUnitHandle(const Blizzard::Storage::StorageUnitHandle &);
    Blizzard::Storage::StorageUnitHandle & operator=(const Blizzard::Storage::StorageUnitHandle &);
    ~StorageUnitHandle();
    void Set(Blizzard::File::INODE, bool);
    void Release();
    operator Blizzard::Storage::StorageUnit *() const { return mUnit; }
    Blizzard::Storage::StorageUnit * operator->() const;
    Blizzard::Storage::StorageUnit & operator*() const;
private:
    Blizzard::File::INODE mNode;  // 0x00
    Blizzard::Storage::StorageUnit * mUnit;  // 0x08
};
static_assert(sizeof(StorageUnitHandle) == 0x10, "StorageUnitHandle");

}  // namespace Storage
}  // namespace Blizzard

// Functions defined in this file (bodies are not in the DWARF):
//   line   122  Blizzard::Storage::StorageUnitHandle::operator Blizzard::Storage::StorageUnit *() const  [inlined into 2 functions]
