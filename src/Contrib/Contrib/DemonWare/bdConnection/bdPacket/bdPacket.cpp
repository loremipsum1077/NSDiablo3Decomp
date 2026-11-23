// Original file: Contrib/Contrib/DemonWare/bdConnection/bdPacket/bdPacket.cpp
#include "bdPacket.h"
#include "../../bdCore/bdSocket/bdCommonAddr.h"
#include "bdDataChunk.h"
#include "bdSAckChunk.h"
#include "bdInitChunk.h"
#include "bdInitAckChunk.h"
#include "bdCookieEchoChunk.h"
#include "bdCookieAckChunk.h"
#include "bdHeartbeatChunk.h"
#include "bdHeartbeatAckChunk.h"
#include "bdShutdownChunk.h"
#include "bdShutdownAckChunk.h"
#include "bdShutdownCompleteChunk.h"
#include "../../bdCore/bdUtilities/bdBytePacker.h"
#include "../../bdPlatform/bdPlatformLog/bdPlatformLog.h"












bdPacket::bdPacket()
    : m_tag(0),
      m_bufferSize(0),
      m_currentSize(0)
{
}

bdPacket::bdPacket(const bdUInt32 tag,
                   const bdUInt bufferSize)
    : m_tag(tag),
      m_bufferSize(bufferSize - 6),
      m_currentSize(6)
{
}


bdPacket::~bdPacket()
{
}

bdUInt bdPacket::serialize(bdUByte8 *outBuffer,
                           const bdUInt outSize)
{

    bdUInt outOffset = 2;


    bdBool ok = bdBytePacker::appendBasicType(outBuffer, outSize, outOffset, outOffset, m_tag);


    bdLinkedList<bdChunkRef>::Position chunkPos = m_chunks.getHeadPosition();
    bdUInt bytesRemaining = outSize - outOffset;
    bdUInt encDataSize = outOffset - 2;

    while (chunkPos && ok)
    {
        const bdChunkRef chunk = m_chunks.forward(chunkPos);



        const bdUInt serializedNumBytes = chunk->getSerializedSize();
        if (serializedNumBytes <= bytesRemaining)
        {


            const bdUInt bytesWritten = chunk->serialize(outBuffer + outOffset, outSize - outOffset);
            encDataSize += bytesWritten;
            outOffset += bytesWritten;
            bytesRemaining -= serializedNumBytes;
        }
        else
        {
            bdUseVAArgs();
            ok = false;
        }
    }


    chunkPos = m_chunks.getHeadPosition();
    while (chunkPos && ok)
    {
        const bdChunkRef chunk = m_chunks.forward(chunkPos);

        if (chunk->getType() == BD_CT_DATA)
        {
            bdDataChunkRef dataChunk = reference_cast<bdDataChunk>(chunk);


            const bdUInt bytesWritten = dataChunk->serializeUnencrypted(outBuffer + outOffset, outSize - outOffset);
            outOffset += bytesWritten;
        }
    }

    bdUInt totalBytesWritten = 0;
    if (ok)
    {

        const bdUInt16 encDataSize16 = static_cast<bdUInt16>(encDataSize);
        if (encDataSize16 == encDataSize)
        {
            bdUInt newOffset = 0;
            ok = bdBytePacker::appendBasicType(outBuffer, outSize, 0, newOffset, encDataSize16);
            if (ok)
            {
                totalBytesWritten = outOffset;
            }
        }
        else
        {
            bdUseVAArgs();
        }
    }



    return totalBytesWritten;
}

bdBool bdPacket::deserialize(const bdUByte8 *inData, // NON_MATCHING: 1296 vs 1308; block placement only (one shared failure log at 242)
                             const bdUInt inSize)
{
    bdBool ok = false;



    bdUInt16 encSize = 0;
    bdUInt encOffset = 0;

    ok = inData && inSize > 6 && bdBytePacker::removeBasicType(inData, inSize, encOffset, encOffset, encSize);
    if (encSize > inSize - encOffset)
    {
        bdUseVAArgs(encSize, inSize);
        return false;
    }



    const bdUByte8 *const encData = inData + encOffset;


    const bdUByte8 *const unencData = encData + encSize;
    const bdUInt unencSize = inSize - encSize - encOffset;
    bdUInt unencOffset = 0;

    encOffset = 0;



    ok = ok && bdBytePacker::removeBasicType(encData, encSize, encOffset, encOffset, m_tag);


    const bdUInt maxIt = 100;
    bdUInt iterations = 0;


    while (encOffset < encSize && ok && iterations < maxIt)
    {


        const bdChunkTypes type = bdChunk::getType(encData + encOffset, encSize - encOffset);


        bdChunkRef chunk;
        switch (type)
        {
        case BD_CT_PADDING:
            ++encOffset;
            break;
        case BD_CT_DATA:
            chunk = new bdDataChunk();
            break;
        case BD_CT_SACK:
            chunk = new bdSAckChunk();
            break;
        case BD_CT_HEARTBEAT:
            chunk = new bdHeartbeatChunk();
            break;
        case BD_CT_HEARTBEAT_ACK:
            chunk = new bdHeartbeatAckChunk();
            break;
        case BD_CT_INIT:
            chunk = new bdInitChunk();
            break;
        case BD_CT_INIT_ACK:
            chunk = new bdInitAckChunk();
            break;
        case BD_CT_ABORT:
            bdUseVAArgs();
            break;

        case BD_CT_COOKIE_ECHO:
            chunk = new bdCookieEchoChunk();
            break;
        case BD_CT_COOKIE_ACK:
            chunk = new bdCookieAckChunk();
            break;
        case BD_CT_ERROR:
            bdUseVAArgs();
            break;

        case BD_CT_SHUTDOWN:
            chunk = new bdShutdownChunk();
            break;
        case BD_CT_SHUTDOWN_ACK:
            chunk = new bdShutdownAckChunk();
            break;
        case BD_CT_SHUTDOWN_COMPLETE:
            chunk = new bdShutdownCompleteChunk();
            break;
        default:

            bdUseVAArgs();
            ok = false;
            break;
        }
        if (chunk.notNull())
        {
            if (type == BD_CT_DATA)
            {
                const bdDataChunkRef dataChunk = reference_cast<bdDataChunk>(chunk);
                ok = dataChunk->deserialize(encData, encSize, encOffset, unencData, unencSize, unencOffset);
            }
            else
            {
                ok = chunk->deserialize(encData, encSize, encOffset);
            }

            if (ok)
            {
                m_chunks.addTail(chunk);
            }
            else
            {
                bdUseVAArgs();
            }
        }
        iterations++;
    }

    return ok;
}

bdUInt32 bdPacket::getVerificationTag() const
{
    return m_tag;
}


bdBool bdPacket::addChunk(bdChunkRef chunk)
{



    bdBool added = false;


    const bdUInt chunkSize = chunk->getSerializedSize();

    const bdUInt newPacketSize = m_currentSize + chunkSize;
    if (newPacketSize <= m_bufferSize)
    {

        m_chunks.addTail(chunk);
        m_currentSize = newPacketSize;
        added = true;
    }
    return added;
}

bdBool bdPacket::getNextChunk(bdChunkRef &chunk)
{
    bdBool result = false;

    if (!m_chunks.isEmpty())
    {
        chunk = m_chunks.getHead();
        m_chunks.removeHead();
        result = true;
    }


    return result;
}

bdBool bdPacket::isEmpty() const
{
    return m_chunks.isEmpty();
}
