// Generated from the DWARF by tools/gen_headers.py -- reference only.
// Original file: Contrib/Contrib/DemonWare/bdConnection/bdWindow/bdUnreliableSendWindow.h

#pragma once

#include "../../bdCore/bdContainers/bdQueue.h"
#include "../../bdCore/bdReference/bdReference.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdPacket;

class bdDataChunk;
typedef bdReference<bdDataChunk> bdDataChunkRef;

class bdUnreliableSendWindow {  // size 0x28
    // vtable pointer
public:
    BD_CLASS_ALLOCATOR(bdUnreliableSendWindow);
    bdUnreliableSendWindow();
    virtual ~bdUnreliableSendWindow();  // vtable slot 0
    void add(bdDataChunkRef chunk);
    void getDataToSend(bdPacket & packet);
    void reset();
protected:
    bdSeqNumber m_seqNumber;  // 0x08
    bdQueue<bdReference<bdDataChunk> > m_sendQueue;  // 0x10
};
static_assert(sizeof(bdUnreliableSendWindow) == 0x28, "bdUnreliableSendWindow");


// Functions defined in this file (bodies are not in the DWARF):
//   line    24  void bdUnreliableSendWindow::operator delete(void * p)  [inlined into 1 functions]
