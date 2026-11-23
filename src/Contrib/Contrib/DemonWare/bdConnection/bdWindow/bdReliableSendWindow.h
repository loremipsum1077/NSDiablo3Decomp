// Generated from the DWARF by tools/gen_headers.py -- reference only.
// Original file: Contrib/Contrib/DemonWare/bdConnection/bdWindow/bdReliableSendWindow.h

#pragma once

#include "bdReliableReceiveWindow.h"
#include "../bdPacket/bdDataChunk.h"
#include "bdUnreliableSendWindow.h"
#include "../../bdCore/bdContainers/bdSequenceNumber.h"
#include "../../bdCore/bdReference/bdReference.h"
#include "../../bdCore/bdTiming/bdStopwatch.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdPacket;

class bdSAckChunk;
typedef bdReference<bdSAckChunk> bdSAckChunkRef;

class bdReliableSendWindow {  // size 0xc40
public:
    BD_CLASS_ALLOCATOR(bdReliableSendWindow);
    bdReliableSendWindow();
    ~bdReliableSendWindow();
    void setTimeoutPeriod(const bdFloat32 secs);
    bdFloat32 getTimeoutPeriod() const;
    bdBool add(bdDataChunkRef chunk);
    void getDataToSend(bdPacket & packet);
    bdBool handleAck(bdSAckChunkRef chunk, bdFloat32 & rtt);
    bdBool isEmpty() const;
    class bdMessageFrame {  // size 0x18
    public:
        BD_CLASS_ALLOCATOR(bdMessageFrame);
        bdMessageFrame() : m_sendCount(0), m_missingCount(0), m_gapAcked(false) {}
        bdMessageFrame(bdDataChunkRef chunk) : m_chunk(chunk), m_sendCount(0), m_missingCount(0), m_gapAcked(false) {}
        ~bdMessageFrame() {}
        bdDataChunkRef m_chunk;  // 0x00
        bdStopwatch m_timer;  // 0x08
        bdUByte8 m_sendCount;  // 0x10
        bdUByte8 m_missingCount;  // 0x11
        bdBool m_gapAcked;  // 0x12
    };
    enum bdCongestionWindowDecreaseReason {
        BD_CWDR_PACKET_LOSS_DETECTED = 0,
        BD_CWDR_RESEND_TIMER_EXPIRED = 1,
        BD_CWDR_INACTIVE = 2,
    };
protected:
    void increaseCongestionWindow(const bdUWord bytesAcked);
    void decreaseCongestionWindow(const bdReliableSendWindow::bdCongestionWindowDecreaseReason reason);
    bdSequenceNumber m_lastAcked;  // 0x00
    bdSequenceNumber m_nextFree;  // 0x04
    bdFloat32 m_timeoutPeriod;  // 0x08
    bdUByte8 m_retransmitCountThreshold;  // 0x0c
    bdReliableSendWindow::bdMessageFrame m_frame[128];  // 0x10
    bdWord m_remoteReceiveWindowCredit;  // 0xc10
    bdWord m_flightSize;  // 0xc18
    bdWord m_partialBytesAcked;  // 0xc20
    bdWord m_slowStartThresh;  // 0xc28
    bdWord m_congestionWindow;  // 0xc30
    bdStopwatch m_lastSent;  // 0xc38
};
static_assert(sizeof(bdReliableSendWindow) == 0xc40, "bdReliableSendWindow");


// Functions defined in this file (bodies are not in the DWARF):
//   line    26  void bdReliableSendWindow::operator delete(void * p)  [inlined into 2 functions]
//   line    26  void * bdReliableSendWindow::operator new(size_t nbytes)  [inlined into 1 functions]
//   line    76  bdReliableSendWindow::bdMessageFrame & bdReliableSendWindow::bdMessageFrame::operator=(const bdReliableSendWindow::bdMessageFrame &)  [inlined into 1 functions]
//   line    82  bdReliableSendWindow::bdMessageFrame::bdMessageFrame()  [inlined into 1 functions]
//   line    89  bdReliableSendWindow::bdMessageFrame::bdMessageFrame(bdDataChunkRef chunk)  [inlined into 1 functions]
//   line    97  bdReliableSendWindow::bdMessageFrame::~bdMessageFrame()  [inlined into 2 functions]
