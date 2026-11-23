// Hand-written from the DWARF of the one instantiation, bdAligned<symmetric_CBC, 16>.
// Original file: Contrib/Contrib/DemonWare/bdCore/bdContainers/bdAligned.h

#pragma once

#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include "../bdMemory/bdMemory-std.h"

template <typename T, bdUWord Align>
struct bdAligned {
    BD_CLASS_ALLOCATOR(bdAligned);
    bdAligned() {}
    // Original lines 45-49.
    T * operator&()
    {
        bdUWord offset = reinterpret_cast<bdUWord>(m_data) & (Align - 1);
        offset = (offset != 0) ? (Align - offset) : 0;
        return reinterpret_cast<T *>(m_data + offset);
    }
protected:
    union {
        bdUByte8 m_data[sizeof(T) + Align];
        T dummy;
    };
    bdAligned(const bdAligned &);
    void operator=(const bdAligned &);
    void operator==(const bdAligned &);
};
