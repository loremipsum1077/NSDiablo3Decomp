// Generated from the DWARF by tools/gen_headers.py -- reference only.
// Original file: Contrib/Contrib/DemonWare/bdConnection/bdPacket/bdShutdownChunk.h

#pragma once

#include "bdChunk.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include "../../bdCore/bdMemory/bdMemory-std.h"
#include <stddef.h>
#include <stdint.h>

class bdShutdownChunk : public bdChunk {  // size 0x18
public:
    BD_CLASS_ALLOCATOR(bdShutdownChunk);
    enum bdShutdownFlags {
        BD_SC_NO_FLAGS = 0,
    };
    bdShutdownChunk();
    virtual ~bdShutdownChunk();  // vtable slot 0
    bdShutdownChunk::bdShutdownFlags getFlags() const;
protected:
    virtual bdUInt serialize(bdUByte8 * data, const bdUInt32 size) const;  // vtable slot 3
    virtual bdBool deserialize(const bdUByte8 *const data, const bdUInt size, bdUInt & offset);  // vtable slot 4
    bdShutdownChunk::bdShutdownFlags m_flags;  // 0x14
};
static_assert(sizeof(bdShutdownChunk) == 0x18, "bdShutdownChunk");


// Functions defined in this file (bodies are not in the DWARF):
//   line    29  void bdShutdownChunk::operator delete(void * p)  [inlined into 1 functions]
//   line    29  void * bdShutdownChunk::operator new(size_t nbytes)  [inlined into 2 functions]
