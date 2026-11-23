// Generated from the DWARF by tools/gen_headers.py -- reference only.
// Original file: Contrib/Contrib/DemonWare/bdConnection/bdPacket/bdChunk.h

#pragma once

#include "../../bdCore/bdReference/bdReferencable.h"
#include "../../bdCore/bdReference/bdReference.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

enum bdChunkTypes {
    BD_CT_INVALID = 0,
    BD_CT_PADDING = 1,
    BD_CT_DATA = 2,
    BD_CT_INIT = 3,
    BD_CT_INIT_ACK = 4,
    BD_CT_SACK = 5,
    BD_CT_HEARTBEAT = 6,
    BD_CT_HEARTBEAT_ACK = 7,
    BD_CT_ABORT = 8,
    BD_CT_SHUTDOWN = 9,
    BD_CT_SHUTDOWN_ACK = 10,
    BD_CT_SHUTDOWN_COMPLETE = 11,
    BD_CT_ERROR = 12,
    BD_CT_COOKIE_ECHO = 13,
    BD_CT_COOKIE_ACK = 14,
};

class bdChunk;
typedef bdReference<bdChunk> bdChunkRef;

class bdChunk : public bdReferencable {  // size 0x18
public:
    BD_CLASS_ALLOCATOR(bdChunk);
    bdChunk(const bdChunkTypes);
    virtual ~bdChunk();  // vtable slot 0
    virtual bdChunkTypes getType() const;  // vtable slot 2
    static bdChunkTypes getType(const bdUByte8 *const data, const bdUInt size);
    virtual bdUInt serialize(bdUByte8 * data, const bdUInt32 size) const;  // vtable slot 3
    virtual bdBool deserialize(const bdUByte8 *const data, const bdUInt size, bdUInt & offset);  // vtable slot 4
    virtual bdUInt getSerializedSize() const;  // vtable slot 5
    bdBool isControl() const;
    struct bdChunkData {  // size 0x4
        bdUByte8 m_type;  // 0x00
        bdUByte8 m_flags;  // 0x01
        bdUInt16 m_length;  // 0x02
    };
protected:
    bdChunkTypes m_type;  // 0x0c
    bdChunk::bdChunkData m_headerData;  // 0x10
};
static_assert(sizeof(bdChunk) == 0x18, "bdChunk");


// Functions defined in this file (bodies are not in the DWARF):
//   line    54  void bdChunk::operator delete(void * p)  [inlined into 1 functions]
