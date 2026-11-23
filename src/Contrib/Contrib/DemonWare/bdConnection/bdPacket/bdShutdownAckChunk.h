// Generated from the DWARF by tools/gen_headers.py -- reference only.
// Original file: Contrib/Contrib/DemonWare/bdConnection/bdPacket/bdShutdownAckChunk.h

#pragma once

#include "bdChunk.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include "../../bdCore/bdMemory/bdMemory-std.h"
#include <stddef.h>
#include <stdint.h>

class bdShutdownAckChunk : public bdChunk {  // size 0x18
public:
    BD_CLASS_ALLOCATOR(bdShutdownAckChunk);
    enum bdShutdownAckFlags {
        BD_SA_NO_FLAGS = 0,
    };
    bdShutdownAckChunk();
    virtual ~bdShutdownAckChunk();  // vtable slot 0
    bdShutdownAckChunk::bdShutdownAckFlags getFlags() const;
protected:
    virtual bdUInt serialize(bdUByte8 * data, const bdUInt size) const;  // vtable slot 3
    virtual bdBool deserialize(const bdUByte8 *const data, const bdUInt size, bdUInt & offset);  // vtable slot 4
    bdShutdownAckChunk::bdShutdownAckFlags m_flags;  // 0x14
};
static_assert(sizeof(bdShutdownAckChunk) == 0x18, "bdShutdownAckChunk");


// Functions defined in this file (bodies are not in the DWARF):
//   line    29  void bdShutdownAckChunk::operator delete(void * p)  [inlined into 1 functions]
//   line    29  void * bdShutdownAckChunk::operator new(size_t nbytes)  [inlined into 2 functions]
