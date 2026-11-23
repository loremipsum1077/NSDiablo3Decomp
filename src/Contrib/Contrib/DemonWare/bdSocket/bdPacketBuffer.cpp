// Original file: Contrib/Contrib/DemonWare/bdSocket/bdPacketBuffer.cpp

#include "bdPacketBuffer.h"
#include "bdDTLS/bdDTLSAssociation.h"
#include "../bdCore/bdSocket/bdAddr.h"
#include "../bdCore/bdUtilities/bdBytePacker.h"
#include "../bdPlatform/bdPlatformLog/bdPlatformLog.h"

bdBool bdPacketBuffer::bufferPacket(bdUInt key, void * data, bdUInt length)
{
    bdBool success = false;
    bdPacketBufferInternal * availBuffer = findBuffer(key, true);
    if (availBuffer)
    {
        if (!availBuffer->getKey())
        {
            availBuffer->setKey(key);
            ++m_numActiveBuffers;
        }
        success = availBuffer->bufferPacket(data, length);
    }
    else
    {
        bdLogWarn("packetbuffer", "No free buffers available.");
    }
    return success;
}

void bdPacketBuffer::clearBuffer(bdUInt key)
{
    bdPacketBufferInternal * buffer = findBuffer(key, false);
    if (buffer)
    {
        buffer->clearBuffer();
        --m_numActiveBuffers;
    }
}

void bdPacketBuffer::clearAll()
{
    for (int i = 0; i < 10; ++i)
    {
        m_buffers[i].clearBuffer();
    }
    m_numActiveBuffers = 0;
}

bdInt bdPacketBuffer::flushBuffer(bdUInt key, bdDTLSAssociation * dtlsSocket, const bdAddr & realAddr)
{
    bdInt bytesSent = 0;
    if (m_numActiveBuffers)
    {
        bdPacketBufferInternal * buffer = findBuffer(key, false);
        if (buffer)
        {
            bytesSent = buffer->flushBuffer(dtlsSocket, realAddr);
            --m_numActiveBuffers;
        }
    }
    return bytesSent;
}

bdPacketBufferInternal * bdPacketBuffer::findBuffer(bdUInt key, bdBool findEmpty)
{
    bdPacketBufferInternal * availBuffer = NULL;
    for (int i = 0; i < 10; ++i)
    {
        bdUInt cachedHash = m_buffers[i].getKey();
        if (findEmpty && availBuffer == NULL && cachedHash == 0)
        {
            availBuffer = &m_buffers[i];
        }
        else if (cachedHash != 0 && cachedHash == key)
        {
            availBuffer = &m_buffers[i];
            break;
        }
    }
    return availBuffer;
}

inline bdBool bdPacketBufferInternal::bufferPacket(void * data, bdUInt length)
{
    bdBool success = false;
    if (m_offset + length + sizeof(bdUInt) < sizeof(m_buffer))
    {
        success = bdBytePacker::appendBasicType<bdUInt>(m_buffer, sizeof(m_buffer), m_offset, m_offset, length);
        success = success && bdBytePacker::appendBuffer(m_buffer, sizeof(m_buffer), m_offset, m_offset, data, length);
    }
    else
    {
        bdLogWarn("packetbuffer", "Packet too big for buffer.");
    }
    return success;
}

inline void bdPacketBufferInternal::clearBuffer()
{
    m_offset = 0;
    m_key = 0;
}

inline bdInt bdPacketBufferInternal::flushBuffer(bdDTLSAssociation * dtlsSocket, const bdAddr & realAddr)
{
    bdUInt totalBytes = 0;
    bdUInt offset = 0;
    while (offset < m_offset)
    {
        bdUInt length;
        if (!bdBytePacker::removeBasicType<bdUInt>(m_buffer, m_offset, offset, offset, length))
        {
            break;
        }
        const bdInt result = dtlsSocket->sendTo(realAddr, m_buffer + offset, length);
        if (result < 0)
        {
            bdNChar8 addrStr[22];
            realAddr.toString(addrStr, sizeof(addrStr));
            bdLogWarn("packetbuffer", "Failed to send to %s: %i", addrStr, result);
            break;
        }
        totalBytes += result;
        offset += length;
    }
    clearBuffer();
    return totalBytes;
}

inline bdUInt bdPacketBufferInternal::getKey()
{
    return m_key;
}

inline void bdPacketBufferInternal::setKey(bdUInt key)
{
    m_key = key;
}
