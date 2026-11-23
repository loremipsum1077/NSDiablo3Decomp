#include "bdConnectionStatistics.h"








// NON_MATCHING: 32 vs 32 bytes, same 8 stores; only the order of the [x0] and [x0,#0x10] stp and the #0x3c store differs (scheduling). ~430 statement orders and chain forms of reset tried.
bdConnectionStatistics::bdConnectionStatistics()
{
    reset();
}

void bdConnectionStatistics::reset()
{
    m_bytesSent = 0;
    m_bytesSentPerSecond = 0;
    m_avgBytesSent = 0;
    m_lastBytesSent = 0;

    m_bytesRecv = 0;
    m_avgPacketSentSize = 0;
    m_avgPacketRecvSize = 0;
    m_maxPacketSizeSent = 0;
    m_maxPacketSizeRecv = 0;
    m_packetsSent = 0;
    m_packetsRecv = 0;



    m_minPacketSizeSent = 0xffffffffu;
    m_minPacketSizeRecv = 0xffffffffu;


    m_maxRTT = 0.0f;
    m_minRTT = 0.0f;
    m_avgRTT = 0.0f;
}

void bdConnectionStatistics::addBytesSent(bdUInt bytes)
{
    m_bytesSent += bytes;
}

void bdConnectionStatistics::addBytesRecv(bdUInt bytes)
{
    m_bytesRecv += bytes;
}

void bdConnectionStatistics::addPacketSizeSent(bdUInt bytes)
{
    m_maxPacketSizeSent = m_maxPacketSizeSent > bytes ? m_maxPacketSizeSent : bytes;
    m_minPacketSizeSent = m_minPacketSizeSent < bytes ? m_minPacketSizeSent : bytes;

    m_avgPacketSentSize = m_avgPacketSentSize ? (m_avgPacketSentSize + bytes) / 2 : bytes;







}

void bdConnectionStatistics::addPacketSizeRecv(bdUInt bytes)
{
    m_maxPacketSizeRecv = m_maxPacketSizeRecv > bytes ? m_maxPacketSizeRecv : bytes;
    m_minPacketSizeRecv = m_minPacketSizeRecv < bytes ? m_minPacketSizeRecv : bytes;

    m_avgPacketRecvSize = m_avgPacketRecvSize ? (m_avgPacketRecvSize + bytes) / 2 : bytes;







}

void bdConnectionStatistics::addPacketsSent(bdUInt numPackets)
{
    m_packetsSent += numPackets;
}

void bdConnectionStatistics::addPacketsRecv(bdUInt numPackets)
{
    m_packetsRecv += numPackets;
}

void bdConnectionStatistics::setLastRTT(bdFloat32 time)
{

    if (m_maxRTT == 0.0f && m_minRTT == 0.0f)
    {
        m_maxRTT = time;
        m_minRTT = time;
    }

    m_maxRTT = m_maxRTT > time ? m_maxRTT : time;
    m_minRTT = m_minRTT < time ? m_minRTT : time;

    m_avgRTT = time;
}

// getters (107-134) and update (137) are linker-removed in the original (DWARF low_pc 0); not written.
