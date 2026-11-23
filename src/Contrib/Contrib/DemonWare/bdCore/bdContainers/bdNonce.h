// Hand-written from the DWARF (template; the DWARF only has bdNonce<16>).
// Original file: Contrib/Contrib/DemonWare/bdCore/bdContainers/bdNonce.h

#pragma once

#include "../bdMemory/bdMemory-std.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

template <unsigned int size>
class bdNonce {
public:
    BD_CLASS_ALLOCATOR(bdNonce);
    bdNonce();
    ~bdNonce();
    void ensureCreated();
    const bdUByte8 * getData() { return m_nonce; }
protected:
    bdUByte8 m_nonce[size];
    bdBool m_initialised;
};

#include "bdNonce.inl"
