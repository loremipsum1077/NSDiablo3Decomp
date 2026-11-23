// Generated from the DWARF by tools/gen_headers.py -- reference only.
// Original file: Contrib/Contrib/DemonWare/bdConnection/bdWindow/bdUnreliableReceiveWindow.h

#pragma once

#include "bdUnreliableSendWindow.h"
#include "../../bdCore/bdContainers/bdQueue.h"
#include "../../bdCore/bdContainers/bdSequenceNumber.h"
#include "../../bdCore/bdReference/bdReference.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdDataChunk;
typedef bdReference<bdDataChunk> bdDataChunkRef;

class bdUnreliableReceiveWindow {  // size 0x40
    // vtable pointer
public:
    BD_CLASS_ALLOCATOR(bdUnreliableReceiveWindow);
    bdUnreliableReceiveWindow();
    virtual ~bdUnreliableReceiveWindow();  // vtable slot 0
    bdBool add(bdDataChunkRef chunk, bdBool acceptOutOfOrder);
    bdDataChunkRef getNextToRead();
    bdDataChunkRef getNextOutOfOrderToRead();
    void reset();
protected:
    bdSequenceNumber m_seqNumber;  // 0x08
    bdQueue<bdReference<bdDataChunk> > m_recvQueue;  // 0x10
    bdQueue<bdReference<bdDataChunk> > m_recvOutOfOrderQueue;  // 0x28
};
static_assert(sizeof(bdUnreliableReceiveWindow) == 0x40, "bdUnreliableReceiveWindow");


// Functions defined in this file (bodies are not in the DWARF):
//   line    23  void bdUnreliableReceiveWindow::operator delete(void * p)  [inlined into 1 functions]
