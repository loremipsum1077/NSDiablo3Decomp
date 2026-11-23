// Generated from the DWARF by tools/gen_headers.py -- reference only.
// Original file: Contrib/Contrib/DemonWare/bdConnection/bdPacket/bdHeartbeatChunk.h

#pragma once

#include "bdChunk.h"
#include "../../bdCore/bdReference/bdReference.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include "../../bdCore/bdMemory/bdMemory-std.h"
#include <stddef.h>
#include <stdint.h>

class bdHeartbeatChunk;
typedef bdReference<bdHeartbeatChunk> bdHeartbeatChunkRef;

class bdHeartbeatChunk : public bdChunk {  // size 0x18
public:
    BD_CLASS_ALLOCATOR(bdHeartbeatChunk);
    enum bdHeartbeatFlags {
        BD_CA_NO_FLAGS = 0,
    };
    bdHeartbeatChunk();
    virtual ~bdHeartbeatChunk();  // vtable slot 0
    bdHeartbeatChunk::bdHeartbeatFlags getFlags() const;
protected:
    virtual bdUInt serialize(bdUByte8 * data, const bdUInt size) const;  // vtable slot 3
    virtual bdBool deserialize(const bdUByte8 *const data, const bdUInt size, bdUInt & offset);  // vtable slot 4
    bdHeartbeatChunk::bdHeartbeatFlags m_flags;  // 0x14
};
static_assert(sizeof(bdHeartbeatChunk) == 0x18, "bdHeartbeatChunk");


// Functions defined in this file (bodies are not in the DWARF):
//   line    29  void bdHeartbeatChunk::operator delete(void * p)  [inlined into 1 functions]
//   line    29  void * bdHeartbeatChunk::operator new(size_t nbytes)  [inlined into 2 functions]
