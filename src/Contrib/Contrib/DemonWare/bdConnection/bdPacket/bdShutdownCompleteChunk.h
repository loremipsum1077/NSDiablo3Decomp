// Generated from the DWARF by tools/gen_headers.py -- reference only.
// Original file: Contrib/Contrib/DemonWare/bdConnection/bdPacket/bdShutdownCompleteChunk.h

#pragma once

#include "bdChunk.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include "../../bdCore/bdMemory/bdMemory-std.h"
#include <stddef.h>
#include <stdint.h>

class bdShutdownCompleteChunk : public bdChunk {  // size 0x18
public:
    BD_CLASS_ALLOCATOR(bdShutdownCompleteChunk);
    enum bdShutdownCompleteFlags {
        BD_SCC_NO_FLAGS = 0,
    };
    bdShutdownCompleteChunk();
    virtual ~bdShutdownCompleteChunk();  // vtable slot 0
    bdShutdownCompleteChunk::bdShutdownCompleteFlags getFlags() const;
protected:
    virtual bdUInt serialize(bdUByte8 * data, const bdUInt size) const;  // vtable slot 3
    virtual bdBool deserialize(const bdUByte8 *const data, const bdUInt size, bdUInt & offset);  // vtable slot 4
    bdShutdownCompleteChunk::bdShutdownCompleteFlags m_flags;  // 0x14
};
static_assert(sizeof(bdShutdownCompleteChunk) == 0x18, "bdShutdownCompleteChunk");


// Functions defined in this file (bodies are not in the DWARF):
//   line    29  void bdShutdownCompleteChunk::operator delete(void * p)  [inlined into 1 functions]
//   line    29  void * bdShutdownCompleteChunk::operator new(size_t nbytes)  [inlined into 2 functions]
