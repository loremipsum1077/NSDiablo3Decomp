// Generated from the DWARF by tools/gen_headers.py -- reference only.
// Original file: Contrib/Contrib/DemonWare/bdSocket/bdAddrHandleRefWrapper.h

#pragma once

#include "../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include "bdAddrHandle.h"
#include <stddef.h>
#include <stdint.h>

class bdAddrHandleRefWrapper {  // size 0x8
public:
    BD_CLASS_ALLOCATOR(bdAddrHandleRefWrapper);
    bdAddrHandleRefWrapper();
    bdAddrHandleRefWrapper(bdAddrHandleRef);
    bdBool operator==(const bdAddrHandleRefWrapper & other) const;
    bdUInt getHash(const bdAddrHandleRefWrapper & addr) const;
    bdAddrHandleRef getHandle() const;
protected:
    bdAddrHandleRef m_handle;  // 0x00
};
static_assert(sizeof(bdAddrHandleRefWrapper) == 0x8, "bdAddrHandleRefWrapper");


// Functions defined in this file (bodies are not in the DWARF):
//   line    21  bdAddrHandleRefWrapper::bdAddrHandleRefWrapper(const bdAddrHandleRefWrapper &)  [inlined into 1 functions]
//   line    21  bdAddrHandleRefWrapper::~bdAddrHandleRefWrapper()  [inlined into 8 functions]
