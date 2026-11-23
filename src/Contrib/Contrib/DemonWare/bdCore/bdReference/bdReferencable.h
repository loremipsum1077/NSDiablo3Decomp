// Original file: Contrib/Contrib/DemonWare/bdCore/bdReference/bdReferencable.h
// Hand-written from the DWARF (member names, order and declaration lines).

#pragma once

#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include "../bdMemory/bdMemory-std.h"
#include <atomic>

// line 30
class bdReferencable {  // size 0x10
public:
    // line 34
    BD_CLASS_ALLOCATOR(bdReferencable);

    bdReferencable();
    bdReferencable(const bdReferencable&);
    virtual ~bdReferencable();
    const bdReferencable& operator=(const bdReferencable&);
    // line 48
    bdInt addRef();
    // line 52
    bdInt releaseRef();
    bdInt getRefCount() const;

protected:
    // line 64
    std::atomic<int> m_refCount;  // 0x08
};
static_assert(sizeof(bdReferencable) == 0x10, "bdReferencable");

#include "bdReferencable.inl"
