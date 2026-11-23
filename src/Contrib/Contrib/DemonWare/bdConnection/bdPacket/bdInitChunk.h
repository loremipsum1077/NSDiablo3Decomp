// Generated from the DWARF by tools/gen_headers.py -- reference only.
// Original file: Contrib/Contrib/DemonWare/bdConnection/bdPacket/bdInitChunk.h

#pragma once

#include "bdChunk.h"
#include "../../bdCore/bdSocket/bdSecurityID.h"
#include "../../bdCore/bdContainers/bdByteBuffer.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include "../../bdCore/bdMemory/bdMemory-std.h"
#include <stddef.h>
#include <stdint.h>

class bdInitChunk : public bdChunk {  // size 0x38
public:
    BD_CLASS_ALLOCATOR(bdInitChunk);
    enum bdInitChunkFlags {
        BD_IC_NO_FLAGS = 0,
    };
    bdInitChunk();
    bdInitChunk(bdUInt32, const bdWord);
    virtual ~bdInitChunk();  // vtable slot 0
    bdUInt32 getInitTag() const;
    bdInitChunk::bdInitChunkFlags getFlags() const;
    bdWord getWindowCredit() const;
    virtual bdUInt serialize(bdUByte8 * data, const bdUInt size) const;  // vtable slot 3
    virtual bdBool deserialize(const bdUByte8 *const data, const bdUInt size, bdUInt & offset);  // vtable slot 4
protected:
    bdUInt32 m_initTag;  // 0x14
    bdInitChunk::bdInitChunkFlags m_flags;  // 0x18
    bdSecurityID m_id;  // 0x1c
    bdByteBufferRef m_theirKey;  // 0x28
    bdWord m_windowCredit;  // 0x30
};
static_assert(sizeof(bdInitChunk) == 0x38, "bdInitChunk");


// Functions defined in this file (bodies are not in the DWARF):
//   line    52  void bdInitChunk::operator delete(void * p)  [inlined into 1 functions]
//   line    52  void * bdInitChunk::operator new(size_t nbytes)  [inlined into 2 functions]
