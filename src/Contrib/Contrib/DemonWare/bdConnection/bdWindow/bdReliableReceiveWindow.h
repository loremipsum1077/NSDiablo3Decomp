// Generated from the DWARF by tools/gen_headers.py -- reference only.
// Original file: Contrib/Contrib/DemonWare/bdConnection/bdWindow/bdReliableReceiveWindow.h

#pragma once

#include "bdUnreliableSendWindow.h"
#include "../../bdCore/bdContainers/bdSequenceNumber.h"
#include "../../bdCore/bdReference/bdReference.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdPacket;

class bdDataChunk;
typedef bdReference<bdDataChunk> bdDataChunkRef;

class bdSAckChunk;
typedef bdReference<bdSAckChunk> bdSAckChunkRef;

class bdReliableReceiveWindow {  // size 0x430
public:
    BD_CLASS_ALLOCATOR(bdReliableReceiveWindow);
    bdReliableReceiveWindow();
    ~bdReliableReceiveWindow();
    bdBool add(bdDataChunkRef chunk);
    void getDataToSend(bdPacket & packet);
    bdDataChunkRef getNextToRead();
protected:
    void calculateAck();
    bdSequenceNumber m_newest;  // 0x00
    bdSequenceNumber m_lastCumulative;  // 0x04
    bdSequenceNumber m_lastDispatched;  // 0x08
    bdDataChunkRef m_frame[128];  // 0x10
    bdBool m_shouldAck;  // 0x410
    bdWord m_recvWindowCredit;  // 0x418
    bdWord m_recvWindowUsedCredit;  // 0x420
    bdSAckChunkRef m_sack;  // 0x428
};
static_assert(sizeof(bdReliableReceiveWindow) == 0x430, "bdReliableReceiveWindow");


// Functions defined in this file (bodies are not in the DWARF):
//   line    26  void bdReliableReceiveWindow::operator delete(void * p)  [inlined into 2 functions]
//   line    26  void * bdReliableReceiveWindow::operator new(size_t nbytes)  [inlined into 1 functions]
