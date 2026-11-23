// Generated from the DWARF by tools/gen_headers.py -- reference only.
// Original file: Contrib/Contrib/DemonWare/bdConnection/bdPacket/bdPacket.h

#pragma once

#include "bdChunk.h"
#include "../../bdCore/bdContainers/bdLinkedList.h"
#include "../../bdCore/bdReference/bdReference.h"
#include "../../bdCore/bdSocket/bdEndpoint.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

typedef bdReference<bdChunk> bdChunkRef;

class bdPacket {  // size 0x38
    // vtable pointer
public:
    BD_CLASS_ALLOCATOR(bdPacket);
    bdPacket();
    bdPacket(const bdUInt32, const bdUInt);
    virtual ~bdPacket();  // vtable slot 0
    bdUInt serialize(bdUByte8 * outBuffer, const bdUInt outSize);
    bdBool deserialize(const bdUByte8 * inData, const bdUInt inSize);
    bdUInt32 getVerificationTag() const;
    bdBool addChunk(bdChunkRef chunk);
    bdBool getNextChunk(bdChunkRef & chunk);
    bdBool isEmpty() const;
protected:
    bdLinkedList<bdReference<bdChunk> > m_chunks;  // 0x08
    bdCommonAddrRef m_me;  // 0x20
    bdUInt32 m_tag;  // 0x28
    bdUInt m_bufferSize;  // 0x2c
    bdUInt m_currentSize;  // 0x30
};
static_assert(sizeof(bdPacket) == 0x38, "bdPacket");


// Functions defined in this file (bodies are not in the DWARF):
//   line    41  void bdPacket::operator delete(void * p)  [inlined into 1 functions]
