// Generated from the DWARF by tools/gen_headers.py -- reference only.
// Original file: Contrib/Contrib/DemonWare/bdConnection/bdPacket/bdHeartbeatAckChunk.h

#pragma once

#include "bdChunk.h"
#include "../../bdCore/bdReference/bdReference.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include "../../bdCore/bdMemory/bdMemory-std.h"
#include <stddef.h>
#include <stdint.h>

class bdHeartbeatAckChunk;
typedef bdReference<bdHeartbeatAckChunk> bdHeartbeatAckChunkRef;

class bdHeartbeatAckChunk : public bdChunk {  // size 0x18
public:
    BD_CLASS_ALLOCATOR(bdHeartbeatAckChunk);
    enum bdHeartbeatAckFlags {
        BD_CA_NO_FLAGS = 0,
    };
    bdHeartbeatAckChunk();
    virtual ~bdHeartbeatAckChunk();  // vtable slot 0
    bdHeartbeatAckChunk::bdHeartbeatAckFlags getFlags() const;
protected:
    virtual bdUInt serialize(bdUByte8 * data, const bdUInt32 size) const;  // vtable slot 3
    virtual bdBool deserialize(const bdUByte8 *const data, const bdUInt size, bdUInt & offset);  // vtable slot 4
    bdHeartbeatAckChunk::bdHeartbeatAckFlags m_flags;  // 0x14
};
static_assert(sizeof(bdHeartbeatAckChunk) == 0x18, "bdHeartbeatAckChunk");


// Functions defined in this file (bodies are not in the DWARF):
//   line    29  void bdHeartbeatAckChunk::operator delete(void * p)  [inlined into 1 functions]
//   line    29  void * bdHeartbeatAckChunk::operator new(size_t nbytes)  [inlined into 2 functions]
