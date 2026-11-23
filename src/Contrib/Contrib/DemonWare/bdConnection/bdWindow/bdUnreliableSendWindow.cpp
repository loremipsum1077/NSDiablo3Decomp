// Original file: Contrib/Contrib/DemonWare/bdConnection/bdWindow/bdUnreliableSendWindow.cpp
#include "bdUnreliableSendWindow.h"
#include "../bdPacket/bdDataChunk.h"
#include "../bdPacket/bdPacket.h"
bdUnreliableSendWindow::bdUnreliableSendWindow() : m_seqNumber(0) {}
bdUnreliableSendWindow::~bdUnreliableSendWindow() {}
void bdUnreliableSendWindow::add(bdDataChunkRef chunk) { m_sendQueue.enqueue(chunk); }
void bdUnreliableSendWindow::getDataToSend(bdPacket &packet) {
    bdBool addedChunk = true;
    bdDataChunkRef chunk;
    while (!m_sendQueue.isEmpty() && addedChunk) {
        chunk = m_sendQueue.peek();
        chunk->setSequenceNumber(m_seqNumber);
        addedChunk = packet.addChunk(chunk);
        if (addedChunk) { ++m_seqNumber; m_sendQueue.dequeue(); }
    }
}
void bdUnreliableSendWindow::reset() {
    m_seqNumber = 0;
    while (!m_sendQueue.isEmpty()) m_sendQueue.dequeue();
}
