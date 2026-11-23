// Original file: Contrib/Contrib/DemonWare/bdLobby/bdLobbyConnection.cpp
#include "bdLobbyConnection.h"
#include "../bdCore/bdContainers/bdSingleton.h"
#include "../bdCore/bdCrypto/bdCypherAES.h"
#include "../bdCore/bdCrypto/bdHMacSHA1.h"
#include "../bdCore/bdCrypto/bdHashSHA1.h"
#include "../bdCore/bdCrypto/bdCryptoUtils.h"
#include "bdLobbyCommon/bdHTTPAuthCommon.h"
#include "../bdCore/bdUtilities/bdBytePacker.h"
#include "../bdCore/bdUtilities/bdTrulyRandom.h"
#include "../bdPlatform/bdPlatformLog/bdPlatformLog.h"
#include "bdLobbyConnectionListener.h"
#include "../bdSocket/bdSocketRouterAdHoc-switch.h"
// Original line 49.
static const bdUInt g_maxClientAuthMessageSize = 1048;

// Original line 53.
bdLobbyConnection::bdLobbyConnection(bdAddr lobbyAddr, bdLobbyConnectionListener * connectionListener, bdBool isLSGConnection, bdBool isPlaintext, bdUWord sendBufSize, bdUWord recvBufSize)
    : bdReferencable(), m_lobbyAddr(lobbyAddr), m_isLSGConnection(isLSGConnection), m_isPlaintext(isPlaintext),
      m_maxSendMessageSize(sendBufSize), m_maxRecvMessageSize(recvBufSize), m_recvState(BD_READ_INIT),
      m_recvCount(0), m_recvEncryptType(0), m_messageSize(0), m_recvMessage(), m_recvTransfer(),
      m_socket(), m_status(BD_NOT_CONNECTED), m_connectionListener(connectionListener),
      m_encryptionType(0), m_cypher(NULL), m_messageCount(0), m_keepAliveTimer(), m_lastReceivedTimer(),
      m_asyncConnectTimer(), m_throttleBucket(40), m_tasksAreThrottled(false), m_authInfo(),
      m_cryptoState(BD_CRYPTO210_INVALID), m_cypher210Client(NULL), m_cypher210Server(NULL),
      m_serverSelectedProto(0), m_migrationState(BD_MIGRATION220_IDLE), m_lobbyMigrationAddr(lobbyAddr),
      m_migrationTicketLen(0), m_migrationReconnectTimeout(30.0f)
{
    m_socket.create(false);
}

// Original line 97.
bdLobbyConnection::~bdLobbyConnection()
{
    m_socket.close();
    if (m_cypher) {
        delete m_cypher;
        m_cypher = NULL;
    }
    if (m_cypher210Client) {
        delete m_cypher210Client;
        m_cypher210Client = NULL;
    }
    if (m_cypher210Server) {
        delete m_cypher210Server;
        m_cypher210Server = NULL;
    }
}

// Original line 170.
bdLobbyConnection::Status bdLobbyConnection::getStatus() const
{
    if (m_migrationState != BD_MIGRATION220_IDLE) {
        if (m_status == BD_CONNECTING || m_status == BD_CONNECTED) {
            return BD_CONNECTED;
        }
        return m_status;
    }
    if (m_status == BD_CONNECTED) {
        return m_cryptoState == BD_CRYPTO210_READY ? BD_CONNECTED : BD_CONNECTING;
    }
    return m_status;
}

// Original line 220.
bdBool bdLobbyConnection::sendTask(bdTaskByteBufferRef message, bdUInt messageSize, const bdBool encrypt)
{
    return sendTask210(message, messageSize, false, 0x86);
}

// Original line 262.
bdUInt bdLobbyConnection::getMaxSendMessageSizeWithoutOverhead() const
{
    return m_maxSendMessageSize;
}

// Original line 272.
// NON_MATCHING: 1476 bytes both; frameSize lives in w25 instead of w8, and three uses of
// plainPayloadTotal add it with `uxtw` where the original reuses the zero-extended x21 (the
// original's zext is formed in the entry block). Tried: recordSize/frameSize/recordTotalSize
// chains, pad-check operand order, 64-bit `headerPtr + plainPayloadTotal + 14` (worse), and
// `& ~static_cast<bdUWord>(15)` in plainPayloadTotal (cols 36 suggest an alignment macro; no change).
bdBool bdLobbyConnection::sendTask210(bdTaskByteBufferRef message, bdUInt messageSize, const bdBool, const bdUByte8 payloadType)
{
    bdBool result;
    if (messageSize > getMaxSendMessageSizeWithoutOverhead()) {
        bdLogError("lobby connection", "Message too large");
        result = false;
    } else if (m_migrationState != BD_MIGRATION220_IDLE) {
        m_migrationOutgoingTaskQueue.enqueue(bdLobbyConnectionMigrationHeldBackTask(message, messageSize));
        result = true;
    } else if (m_status == BD_CONNECTED && m_cryptoState == BD_CRYPTO210_READY) {
        // Record layout constants: DWARF const values (plainPayloadPrefix 5, payloadPrefixSize 31,
        // headerSize 31); the others are inferred from the arithmetic (total+14, +30, +34).
        const bdUInt plainPayloadPrefix = 5;
        const bdUInt plainPayloadTotal = (messageSize + plainPayloadPrefix + 15) & ~15u;
        const bdUInt plainPayloadPad = plainPayloadTotal - (messageSize + plainPayloadPrefix);
        const bdUInt recordSize = plainPayloadTotal + 14;
        const bdUInt frameSize = plainPayloadTotal + 30;
        const bdUInt payloadPrefixSize = 31;
        const bdUInt recordTotalSize = plainPayloadTotal + 34;
        bdBool ok;
        message->setHeaderSize(payloadPrefixSize);
        bdUByte8 * headerPtr = message->getHeaderStart();
        const bdUInt headerSize = payloadPrefixSize;
        bdUInt headerOffset = 0;
        ok = bdBytePacker::appendBasicType<bdUInt32>(headerPtr, headerSize, headerOffset, headerOffset, frameSize);
        // 0xab / 0x85: frame type and record type bytes (same values as send210ClientHeader / process210ExpectedRecord).
        ok = ok && bdBytePacker::appendBasicType<bdUByte8>(headerPtr, headerSize, headerOffset, headerOffset, 0xab);
        ok = ok && bdBytePacker::appendBasicType<bdUByte8>(headerPtr, headerSize, headerOffset, headerOffset, 0x85);
        ok = ok && bdBytePacker::appendBasicType<bdUInt32>(headerPtr, headerSize, headerOffset, headerOffset, m_cypher210SendCounter);
        bdUByte8 iv[16];
        bdSingleton<bdTrulyRandomImpl>::getInstance()->getRandomUByte8(iv, sizeof(iv));
        ok = ok && bdBytePacker::appendBuffer(headerPtr, headerSize, headerOffset, headerOffset, iv, sizeof(iv));
        const bdUInt payloadStartOffset = headerOffset;
        bdUByte8 * payloadStartPtr = headerPtr + payloadStartOffset;
        ok = ok && bdBytePacker::appendBasicType<bdUInt32>(headerPtr, headerSize, headerOffset, headerOffset, messageSize);
        ok = ok && bdBytePacker::appendBasicType<bdUByte8>(headerPtr, headerSize, headerOffset, headerOffset, payloadType);
        ok = ok && (headerPtr + headerOffset == message->getData())
            && (payloadStartPtr + plainPayloadTotal + 8 <= message->getStartOfTaskDataBuffer() + message->getSizeOfTaskDataBuffer());
        bdMemset(payloadStartPtr + plainPayloadTotal - plainPayloadPad, 0, plainPayloadPad);
        ok = plainPayloadPad <= plainPayloadTotal && plainPayloadPad < 16 && ok;
        ok = ok && m_cypher210Client->encrypt(iv, payloadStartPtr, payloadStartPtr, plainPayloadTotal);
        {
            bdHMacSHA1 hmac(m_cypher210ClientMacKey, sizeof(m_cypher210ClientMacKey));
            ok = ok && (headerPtr + recordSize < message->getStartOfTaskDataBuffer() + message->getSizeOfTaskDataBuffer());
            ok = ok && hmac.process(headerPtr, recordTotalSize - 8);
            bdUInt hmacSize = 8;
            ok = ok && hmac.getData(payloadStartPtr + plainPayloadTotal, hmacSize);
        }
        if (ok) {
            m_cypher210SendCounter++;
            bdPendingBufferTransfer tx(message, recordTotalSize, m_tasksAreThrottled);
            m_outgoingBuffers.enqueue(tx);
            result = true;
        } else {
            bdLogError("lobby connection", "Failed to build record");
            result = false;
        }
    } else {
        result = false;
    }
    pump();
    return result;
}

// Original line 554.
void bdLobbyConnection::sendFramedMessage(const void * buffer, const bdUInt32 bufferSize)
{
    bdByteBufferRef message = new bdByteBuffer(const_cast<void *>(buffer), bufferSize, false);
    bdPendingBufferTransfer tx(message, bufferSize, false);
    m_outgoingBuffers.enqueue(tx);
    pump();
}

// Original line 613.
bdBool bdLobbyConnection::getMessageToDispatch(bdUByte8 & type, bdByteBufferRef & payload)
{
    return getMessageToDispatch210(type, payload);
}

// Original line 633.
bdBool bdLobbyConnection::process220MigrateInit(bdUByte8 *const readPtr, const bdUInt validBytes, bdUInt & offset)
{
    bdUByte8 migrateInitPacketVersion;
    bdBool ok = bdBytePacker::removeBasicType<bdUByte8>(readPtr, validBytes, offset, offset, migrateInitPacketVersion);
    if (ok && migrateInitPacketVersion == 1) {
        if (m_migrationState == BD_MIGRATION220_IDLE) {
            if (send220MigrateInitAck()) {
                m_migrationState = BD_MIGRATION220_INIT_ACK_SENT;
                bdLogInfo("lobby connection", "Sent migrate init ack");
                return true;
            }
            bdLogError("lobby connection", "Failed to send migrate init ack");
            close();
            return false;
        }
    } else {
        ok = false;
    }
    bdLogError("lobby connection", "Unexpected migrate init");
    close();
    return ok;
}

// Original line 672.
bdBool bdLobbyConnection::send220MigrateInitAck()
{
    const bdUWord initAckPayloadSize = 1;
    bdTaskByteBufferRef buffer = new bdTaskByteBuffer(initAckPayloadSize, false);
    bdBool ok = false;
    if (buffer.notNull()) {
        ok = buffer->writeUByte8(1);
        if (ok) {
            ok = sendTask210(buffer, buffer->getDataSize(), false, 0x88);
        }
    }
    return ok;
}

// Original line 685.
bdBool bdLobbyConnection::parse220MigrateAck(bdUByte8 *const readPtr, const bdUInt validBytes, bdUInt & offset)
{
    bdBool ok = m_migrationState == BD_MIGRATION220_INIT_ACK_SENT;
    bdUByte8 migrateAckPacketVersion;
    bdUByte8 reconnectAddrType;
    bdUByte8 reconnectAddrLen;
    bdUInt16 reconnectTicketLen;
    bdUInt32 reconnectTimeout;
    ok = ok && bdBytePacker::removeBasicType<bdUByte8>(readPtr, validBytes, offset, offset, migrateAckPacketVersion);
    ok = ok && migrateAckPacketVersion == 1;
    ok = ok && bdBytePacker::removeBasicType<bdUByte8>(readPtr, validBytes, offset, offset, reconnectAddrType);
    ok = ok && (reconnectAddrType == 1 || reconnectAddrType == 2);
    ok = ok && bdBytePacker::removeBasicType<bdUByte8>(readPtr, validBytes, offset, offset, reconnectAddrLen);
    if (ok) {
        ok = reconnectAddrType == 2 || reconnectAddrLen == 0;
        if (reconnectAddrType == 2) {
            bdAddr newAddr;
            bdUInt tmpOffset = offset;
            ok = ok && newAddr.deserialize(readPtr, validBytes, tmpOffset, tmpOffset) && tmpOffset - offset == reconnectAddrLen;
            if (ok) {
                offset = tmpOffset;
                const bdUInt maxAddrStrLen = 32;
                bdNChar8 newAddrStr[maxAddrStrLen];
                bdNChar8 oldAddrStr[maxAddrStrLen];
                newAddrStr[0] = '\0';
                newAddr.toString(newAddrStr, maxAddrStrLen);
                oldAddrStr[0] = '\0';
                m_lobbyMigrationAddr.toString(oldAddrStr, maxAddrStrLen);
                bdLogInfo("lobby connection", "Migrating from %s to %s", newAddrStr, oldAddrStr);
                m_lobbyMigrationAddr = newAddr;
            }
        }
    }
    ok = ok && bdBytePacker::removeBasicType<bdUInt16>(readPtr, validBytes, offset, offset, reconnectTicketLen);
    ok = ok && reconnectTicketLen <= sizeof(m_migrationTicket);
    ok = ok && bdBytePacker::removeBuffer(readPtr, validBytes, offset, offset, m_migrationTicket, reconnectTicketLen);
    if (ok) {
        m_migrationTicketLen = reconnectTicketLen;
    }
    ok = ok && bdBytePacker::removeBasicType<bdUInt32>(readPtr, validBytes, offset, offset, reconnectTimeout);
    if (ok) {
        const bdFloat32 timeout = reconnectTimeout / 1000.0f;
        m_migrationReconnectTimeout = timeout > 30.0f ? 30.0f : timeout;
    } else {
        bdLogWarn("lobby connection", "Failed to parse migrate ack at offset %u", offset);
    }
    return ok;
}

// Original line 764.
bdBool bdLobbyConnection::process220MigrateAck(bdUByte8 *const readPtr, const bdUInt validBytes, bdUInt & offset)
{
    if (m_migrationState == BD_MIGRATION220_INIT_ACK_SENT) {
        const bdBool ok = parse220MigrateAck(readPtr, validBytes, offset);
        bdLogInfo("lobby connection", "Migrate ack");
        if (!ok) {
            close();
            return false;
        }
        m_status = BD_CONNECTING;
        m_migrationState = BD_MIGRATION220_RECONNECTING;
        m_cryptoState = BD_CRYPTO210_INVALID;
        m_socket.close();
        m_socket.create(false);
        bdSingleton<bdTrulyRandomImpl>::getInstance()->getRandomUByte8(m_clientNonce, sizeof(m_clientNonce));
        m_lobbyAddr = m_lobbyMigrationAddr;
        bdSocketStatusCode status = m_socket.connect(m_lobbyMigrationAddr);
        switch (status) {
        case BD_NET_SUCCESS:
            m_status = BD_CONNECTED;
            onTcpConnect(true);
            return true;
        case BD_NET_WOULD_BLOCK:
            m_asyncConnectTimer.reset();
            m_asyncConnectTimer.start();
            return true;
        default:
            break;
        }
    } else {
        bdLogWarn("lobby connection", "Unexpected migrate ack");
    }
    bdLogError("lobby connection", "Migration failed");
    close();
    return false;
}

// Original line 829.
bdBool bdLobbyConnection::process210ExpectedRecord(bdUByte8 *const readPtr, const bdUInt readOffset, const bdUInt validBytes, bdUByte8 & returnMsgType, bdByteBufferRef & returnPayload)
{
    bdBool ok;
    bdUInt offset = readOffset;
    // Strings are the record's failure reasons (rodata 0x7100d7c4ef, 0x7100e2faad..0x7100e2fae6).
    const bdNChar8 * whyFailed = "???";
    // Field sizes: inferred from the removeBuffer sizes (16, 8) and the `& 0xf` block check.
    const bdUInt ivFieldSize = 16;
    const bdUInt hmacFieldSize = 8;
    const bdUInt aesBlockSize = 16;
    // Initialiser inferred: the original zeroes recvCounter when its read fails (+0x78).
    bdUInt32 recvCounter = 0;
    bdUByte8 iv[ivFieldSize];
    bdUByte8 recvHmac[hmacFieldSize];
    ok = bdBytePacker::removeBasicType<bdUInt32>(readPtr, validBytes, offset, offset, recvCounter);
    ok = ok && bdBytePacker::removeBuffer(readPtr, validBytes, offset, offset, iv, ivFieldSize);
    bdUByte8 * payloadStart = readPtr + offset;
    const bdUInt payloadStartOffset = offset;
    bdMemcpy(recvHmac, readPtr + validBytes - hmacFieldSize, hmacFieldSize);
    const bdUByte8 * payloadEnd = readPtr + validBytes - hmacFieldSize;
    if (validBytes <= hmacFieldSize) {
        ok = false;
    }
    if (ok) {
        ok = payloadEnd > payloadStart && (payloadEnd - payloadStart) % aesBlockSize == 0
            && (payloadEnd - payloadStart) < 0xFFFFFF;
        if (!ok) {
            whyFailed = "Bad frame/payload size";
        }
    }
    const bdUInt payloadLength = static_cast<bdUInt>(payloadEnd - payloadStart);
    if (ok) {
        ok = recvCounter == m_cypher210RecvCounter++;
        if (!ok) {
            whyFailed = "Bad recv counter";
        }
    }
    // 20 = SHA-1 output size (hmac getData length).
    const bdUInt hmacFullResultSize = 20;
    bdUByte8 expectedHmac[hmacFullResultSize];
    if (ok) {
        // 5 = the frame's size and type bytes in front of the header start.
        bdUByte8 * bufferFrameStart = m_recvMessage->getHeaderStart() - 5;
        ok = bufferFrameStart >= m_recvMessage->getStartOfTaskDataBuffer();
        const bdBool endPtrInRange = m_messageSize - 4 <= m_recvMessage->getSizeOfTaskDataBuffer();
        bdHMacSHA1 hmac(m_cypher210ServerMacKey, sizeof(m_cypher210ServerMacKey));
        ok = ok && endPtrInRange && hmac.process(bufferFrameStart, m_messageSize - 4);
        bdUInt sz = hmacFullResultSize;
        ok = ok && hmac.getData(expectedHmac, sz);
        if (!ok) {
            whyFailed = "HMAC calculation";
        }
    }
    if (ok) {
        ok = bdCryptoUtils::constTimeCompare(expectedHmac, recvHmac, hmacFieldSize);
        if (!ok) {
            whyFailed = "HMAC mismatch";
        }
    }
    if (ok) {
        ok = m_cypher210Server->decrypt(iv, payloadStart, payloadStart, payloadLength);
        bdUInt offset2 = payloadStartOffset;
        bdUInt32 plainSize;
        ok = ok && bdBytePacker::removeBasicType<bdUInt32>(readPtr, validBytes, offset2, offset2, plainSize);
        bdUByte8 msgType;
        ok = ok && plainSize <= payloadLength && bdBytePacker::removeBasicType<bdUByte8>(readPtr, validBytes, offset2, offset2, msgType);
        const bdUInt payloadEndOffset = offset2 + plainSize;
        ok = ok && payloadEndOffset <= validBytes;
        // 0x87 / 0x89: migrate init / migrate ack record types; 220 is the migration protocol.
        if (ok && msgType == 0x87 && m_serverSelectedProto >= 220) {
            process220MigrateInit(readPtr, payloadEndOffset, offset2);
            ok = false;
        } else if (ok && msgType == 0x89 && m_serverSelectedProto >= 220) {
            process220MigrateAck(readPtr, payloadEndOffset, offset2);
            ok = false;
        } else if (ok) {
            m_recvMessage->adjustPayload(readPtr + offset2, plainSize);
            returnPayload = m_recvMessage;
            returnMsgType = msgType;
        } else {
            bdLogError("lobby connection", "Bad record payload");
            close();
        }
    } else {
        bdLogError("lobby connection", "Record failed: %s", whyFailed);
        close();
    }
    return ok;
}

// Original line 980.
inline bdBool bdLobbyConnection::extract210ExpectedServerError(const bdUByte8 *const readPtr, const bdUInt readOffset, const bdUInt validBytes, bdUInt32 & returnErrorReason)
{
    bdUInt offset = readOffset;
    bdUInt32 errorReason;
    bdBool ok = bdBytePacker::removeBasicType<bdUInt32>(readPtr, validBytes, offset, offset, errorReason);
    if (!ok) {
        bdLogError("lobby connection", "Failed to read server error");
        errorReason = 0;
    }
    returnErrorReason = errorReason;
    return ok;
}

// Original line 1006.
bdBool bdLobbyConnection::process210ExpectedServerAuthDone(const bdUByte8 *const readPtr, const bdUInt readOffset, const bdUInt validBytes)
{
    bdBool ok = false;
    bdUInt offset = readOffset;
    bdUByte8 recvVerify[8];
    ok = bdBytePacker::removeBuffer(readPtr, validBytes, offset, offset, recvVerify, sizeof(recvVerify));
    if (ok) {
        // DWARF: bdMemcmp inlined at 1020, the branch on 1021 (col 7 = `ok`).
        ok = bdMemcmp(m_expectedServerVerify, recvVerify, sizeof(recvVerify)) == 0;
        if (ok) {
            if (m_migrationState == BD_MIGRATION220_IDLE) {
                bdLogInfo("lobby connection", "Connected with protocol %u", m_serverSelectedProto);
            }
            m_cryptoState = BD_CRYPTO210_READY;
            m_cypher210RecvCounter = 1;
            m_cypher210SendCounter = 1;
        } else {
            bdLogError("lobby connection", "Server verify failed");
            close();
        }
    } else {
        // The two failure paths share one line-0 tail in the original (b3ceac); log text inferred (strings are compiled out).
        bdLogError("lobby connection", "Server verify failed");
        close();
    }
    return ok;
}

// Original line 1049.
// NON_MATCHING: 1808 vs 1800 bytes, same calls and control flow. serverHeaderAckReadPtr stays in
// x25 (spilled later) instead of being spilled at entry; the original keeps both
// replyPacketOffset - origReplyPacketOffset and replyPacketSize live (sub w5, w28, #8 at the last
// inSalt append) where ours keeps only replyPacketSize. Tried `replyPacketSize + 4 - 8` and a
// sizeof(bdUInt32) operand in replyPacketSize, in the 1128 bound and in the 1197 `replyPacketSize - 4`; no change.
bdBool bdLobbyConnection::prepare2x0ClientAuthAndDeriveCrypto(bdUByte8 *const replyPacketPtr, bdUInt & replyPacketOffset, const bdUInt replyPacketMaxSize, const bdUByte8 *const serverHeaderAckReadPtr, const bdUInt serverHeaderAckReadOffset, const bdUInt serverHeaderAckEndOffset)
{
    const bdUInt origReplyPacketOffset = replyPacketOffset;
    bdBool ok;
    // Placeholder for the frame size, patched below at origReplyPacketOffset.
    ok = bdBytePacker::appendBasicType<bdUInt32>(replyPacketPtr, replyPacketMaxSize, replyPacketOffset, replyPacketOffset, 0);
    // 0xab frame type, 0x90 migration reconnect, 0x82 client auth (values from the binary).
    ok = ok && bdBytePacker::appendBasicType<bdUByte8>(replyPacketPtr, replyPacketMaxSize, replyPacketOffset, replyPacketOffset, 0xab);
    if (m_migrationState == BD_MIGRATION220_RECONNECTED) {
        ok = ok && bdBytePacker::appendBasicType<bdUByte8>(replyPacketPtr, replyPacketMaxSize, replyPacketOffset, replyPacketOffset, 0x90);
        ok = ok && m_migrationTicketLen <= sizeof(m_migrationTicket) - 1;
        ok = ok && bdBytePacker::appendBasicType<bdUInt16>(replyPacketPtr, replyPacketMaxSize, replyPacketOffset, replyPacketOffset, static_cast<bdUInt16>(m_migrationTicketLen));
        ok = ok && bdBytePacker::appendBuffer(replyPacketPtr, replyPacketMaxSize, replyPacketOffset, replyPacketOffset, m_migrationTicket, m_migrationTicketLen);
        // 8 bytes reserved for the challenge response written below.
        ok = ok && bdBytePacker::skipBytes(replyPacketPtr, replyPacketMaxSize, replyPacketOffset, replyPacketOffset, 8);
    } else if (ok) {
        bdBitBufferRef authBlob;
        bdUByte8 serviceID = 7;
        authBlob = new bdBitBuffer(&serviceID, 8, true);
        // Inferred: the original skips everything below when the allocation fails (cbz after the ctor).
        ok = authBlob.notNull();
        if (ok) {
            authBlob->setTypeCheck(false);
            authBlob->writeBool(true);
            authBlob->setTypeCheck(true);
            authBlob->writeUInt32(m_authInfo.m_titleID);
            authBlob->writeUInt32(m_authInfo.m_IVSeed);
            authBlob->writeBits(m_authInfo.m_data, sizeof(m_authInfo.m_data) * 8);
        }
        ok = ok && bdBytePacker::appendBasicType<bdUByte8>(replyPacketPtr, replyPacketMaxSize, replyPacketOffset, replyPacketOffset, 0x82);
        ok = ok && bdBytePacker::appendBuffer(replyPacketPtr, replyPacketMaxSize, replyPacketOffset, replyPacketOffset, authBlob->getData(), authBlob->getDataSize());
        ok = ok && bdBytePacker::skipBytes(replyPacketPtr, replyPacketMaxSize, replyPacketOffset, replyPacketOffset, 8);
    }
    ok = ok && (replyPacketOffset > origReplyPacketOffset + 4);
    const bdUInt replyPacketSize = replyPacketOffset - origReplyPacketOffset - 4;
    bdUInt tmpOffset = origReplyPacketOffset;
    ok = ok && bdBytePacker::appendBasicType<bdUInt32>(replyPacketPtr, replyPacketMaxSize, tmpOffset, tmpOffset, replyPacketSize);
    const bdUInt extractResultSize = 20;
    bdUByte8 extractResult[extractResultSize];
    {
        // DWARF const value 1098.
        const bdUInt inSaltSize = 1098;
        bdUByte8 inSalt[inSaltSize];
        bdUInt saltOffset = 0;
        // 210 / 220: the supported protocol range (as in process210ExpectedServerHeader).
        ok = ok && bdBytePacker::appendBasicType<bdUInt32>(inSalt, inSaltSize, saltOffset, saltOffset, 210);
        ok = ok && bdBytePacker::appendBasicType<bdUInt32>(inSalt, inSaltSize, saltOffset, saltOffset, 220);
        ok = ok && bdBytePacker::appendBasicType<bdUInt32>(inSalt, inSaltSize, saltOffset, saltOffset, getReceiveBufferSize());
        ok = ok && bdBytePacker::appendBuffer(inSalt, inSaltSize, saltOffset, saltOffset, m_clientNonce, sizeof(m_clientNonce));
        ok = ok && (serverHeaderAckEndOffset > serverHeaderAckReadOffset);
        const bdUInt32 headerAckPayloadSize = serverHeaderAckEndOffset - serverHeaderAckReadOffset;
        ok = ok && bdBytePacker::appendBasicType<bdUInt32>(inSalt, inSaltSize, saltOffset, saltOffset, headerAckPayloadSize + 2);
        ok = ok && bdBytePacker::appendBasicType<bdUByte8>(inSalt, inSaltSize, saltOffset, saltOffset, 0xab);
        ok = ok && bdBytePacker::appendBasicType<bdUByte8>(inSalt, inSaltSize, saltOffset, saltOffset, 0x81);
        ok = ok && bdBytePacker::appendBuffer(inSalt, inSaltSize, saltOffset, saltOffset, serverHeaderAckReadPtr + serverHeaderAckReadOffset, headerAckPayloadSize);
        ok = ok && replyPacketSize > 12;
        ok = ok && bdBytePacker::appendBuffer(inSalt, inSaltSize, saltOffset, saltOffset, replyPacketPtr + origReplyPacketOffset, replyPacketSize - 4);
        bdHashSHA1 sha1;
        bdUByte8 inSaltHashed[20];
        bdUInt hashSize = sizeof(inSaltHashed);
        ok = ok && sha1.hash(inSalt, saltOffset, inSaltHashed, hashSize);
        if (m_authInfo.m_authSessionKeyKDF) {
            bdUByte8 sessionKeyActual[24];
            ok = ok && bdCryptoUtils::hkdfExpand(m_sessionKey, sizeof(m_sessionKey), BD_AUTH_TRAFFIC_SIGNING_KEY, sizeof(BD_AUTH_TRAFFIC_SIGNING_KEY), sessionKeyActual, sizeof(sessionKeyActual), BD_HASH_SHA1);
            ok = ok && bdCryptoUtils::hkdfExtractSHA1(inSaltHashed, sessionKeyActual, sizeof(sessionKeyActual), extractResult);
        } else {
            ok = ok && bdCryptoUtils::hkdfExtractSHA1(inSaltHashed, m_sessionKey, sizeof(m_sessionKey), extractResult);
        }
        bdCryptoUtils::zeroBuffer(inSaltHashed, sizeof(inSaltHashed));
    }
    {
        bdUByte8 chResponse[16];
        ok = ok && bdCryptoUtils::hkdfExpand(extractResult, extractResultSize, "CLIENTCHAL", 10, chResponse, sizeof(chResponse), BD_HASH_SHA1);
        ok = ok && bdBytePacker::rewindBytes(replyPacketPtr, replyPacketMaxSize, replyPacketOffset, replyPacketOffset, 8);
        ok = ok && bdBytePacker::appendBuffer(replyPacketPtr, replyPacketMaxSize, replyPacketOffset, replyPacketOffset, chResponse, 8);
        bdMemcpy(m_expectedServerVerify, chResponse + 8, sizeof(m_expectedServerVerify));
        bdCryptoUtils::zeroBuffer(chResponse, sizeof(chResponse));
    }
    const bdUInt keyMaterialSize = 72;
    bdUByte8 keyMaterial[keyMaterialSize];
    ok = ok && bdCryptoUtils::hkdfExpand(extractResult, extractResultSize, "BDDATA", 6, keyMaterial, keyMaterialSize, BD_HASH_SHA1);
    if (ok) {
        bdMemcpy(m_cypher210ClientMacKey, keyMaterial, sizeof(m_cypher210ClientMacKey));
        bdMemcpy(m_cypher210ServerMacKey, keyMaterial + 20, sizeof(m_cypher210ServerMacKey));
        if (m_cypher210Client) {
            delete m_cypher210Client;
            m_cypher210Client = NULL;
        }
        m_cypher210Client = new bdCypherAES();
        ok = ok && m_cypher210Client->init(keyMaterial + 40, 16);
        if (m_cypher210Server) {
            delete m_cypher210Server;
            m_cypher210Server = NULL;
        }
        m_cypher210Server = new bdCypherAES();
        ok = ok && m_cypher210Server->init(keyMaterial + 56, 16);
    }
    bdCryptoUtils::zeroBuffer(keyMaterial, sizeof(keyMaterial));
    return ok;
}

// Original line 1295.
bdBool bdLobbyConnection::process210ExpectedServerHeader(const bdUByte8 *const readPtr, const bdUInt readOffset, const bdUInt validBytes)
{
    bdUInt offset = readOffset;
    bdUInt32 serverSelectedProto;
    bdBool ok = bdBytePacker::removeBasicType<bdUInt32>(readPtr, validBytes, offset, offset, serverSelectedProto);
    ok = ok && bdBytePacker::removeBasicType<bdUInt64>(readPtr, validBytes, offset, offset, m_cypher210ConnID);
    ok = ok && bdBytePacker::skipBytes(readPtr, validBytes, offset, offset, 8);
    if (ok) {
        if (serverSelectedProto >= 210 && serverSelectedProto <= 220) {
            bdUByte8 clientAuthPacket[1048];
            bdUInt clientAuthPacketOffset = 0;
            m_serverSelectedProto = serverSelectedProto;
            if (prepare2x0ClientAuthAndDeriveCrypto(clientAuthPacket, clientAuthPacketOffset, sizeof(clientAuthPacket), readPtr, readOffset, offset)) {
                sendFramedMessage(clientAuthPacket, clientAuthPacketOffset);
                m_cryptoState = BD_CRYPTO210_WAIT_SERVER_AUTH_DONE;
                return true;
            }
        } else {
            bdLogError("lobby connection", "Unsupported protocol");
            bdLogError("lobby connection", "Server selected %u, supported %u-%u", serverSelectedProto, 210, 220);
        }
    }
    bdLogError("lobby connection", "Bad server header");
    close();
    return false;
}

// Original line 1366.
bdBool bdLobbyConnection::process220MigrationServerAuthDone()
{
    bdBool ok = true;
    bdLogInfo("lobby connection", "Sending %u held back tasks", m_migrationOutgoingTaskQueue.getSize());
    m_migrationState = BD_MIGRATION220_IDLE;
    while (ok && !m_migrationOutgoingTaskQueue.isEmpty()) {
        bdLobbyConnectionMigrationHeldBackTask item = m_migrationOutgoingTaskQueue.peek();
        m_migrationOutgoingTaskQueue.dequeue();
        ok = sendTask210(item.m_task, item.m_messageSize, false, 0x86);
        if (!ok) {
            bdLogError("lobby connection", "Failed to send held back task");
            close();
        }
    }
    if (ok) {
        bdLogInfo("lobby connection", "Migration complete");
    }
    return ok;
}

// Original line 1399.
bdBool bdLobbyConnection::getMessageToDispatch210(bdUByte8 & type, bdByteBufferRef & payload)
{
    if (m_status == BD_DISCONNECTED) {
        return false;
    }
    pump();
    bdBool result = false;
    if (m_status == BD_CONNECTED && recvMessageData() && receivedFullMessage()) {
        bdUByte8 * readPtr = m_recvMessage->getHeaderStart();
        bdUInt validBytes = m_messageSize - 1;
        bdBool ok;
        bdUByte8 msgType;
        bdUInt offset = 0;
        ok = bdBytePacker::removeBasicType<bdUByte8>(readPtr, validBytes, offset, offset, msgType);
        if (ok) {
            switch (m_cryptoState) {
            case BD_CRYPTO210_WAIT_SERVER_ACK:
                if (msgType == 0x81) {
                    process210ExpectedServerHeader(readPtr, offset, validBytes);
                } else {
                    bdLogError("lobby connection", "Expected server header");
                    if (msgType == 4) {
                        bdLogError("lobby connection", "Server rejected the connection");
                    }
                    close();
                }
                break;
            case BD_CRYPTO210_WAIT_SERVER_AUTH_DONE:
                if (msgType == 0x83) {
                    ok = process210ExpectedServerAuthDone(readPtr, offset, validBytes);
                    if (ok && m_migrationState == BD_MIGRATION220_IDLE) {
                        payload = new bdByteBuffer(8, false);
                        payload->writeUInt64(m_cypher210ConnID);
                        type = 4;
                        result = true;
                    } else if (ok && m_migrationState == BD_MIGRATION220_RECONNECTED) {
                        process220MigrationServerAuthDone();
                    } else {
                        // Inferred: the original returns 0 here without resetting m_recvState (b3c034/b3c044 -> b3c0ec).
                        return false;
                    }
                } else if (msgType == 0x84) {
                bdUInt32 errorReason;
                const bdBool ok = extract210ExpectedServerError(readPtr, offset, validBytes, errorReason);
                bdLogError("lobby connection", "Server error %u", errorReason);
                close();
                if (ok) {
                    payload = new bdByteBuffer(8, true);
                    payload->writeUInt32(errorReason);
                    type = 3;
                    result = true;
                }
                } else {
                    bdLogError("lobby connection", "Unexpected message");
                    close();
                }
                break;
            case BD_CRYPTO210_READY:
                if (msgType == 0x85) {
                    result = process210ExpectedRecord(readPtr, offset, validBytes, type, payload);
                } else if (msgType == 0x84) {
                bdUInt32 errorReason;
                const bdBool ok = extract210ExpectedServerError(readPtr, offset, validBytes, errorReason);
                bdLogError("lobby connection", "Server error %u", errorReason);
                close();
                if (ok) {
                    payload = new bdByteBuffer(8, true);
                    payload->writeUInt32(errorReason);
                    type = 3;
                    result = true;
                }
                } else {
                    bdLogError("lobby connection", "Unexpected message");
                    close();
                }
                break;
            default:
                bdLogError("lobby connection", "Unexpected message");
                close();
                break;
            }
        } else {
            bdLogError("lobby connection", "Unexpected message");
            close();
        }
        m_recvState = BD_READ_INIT;
    }
    return result;
}

// Original line 1665.


// Tail-recursive form, as the columns suggest (1667 col 6 is an `if`, single `return` at 1759 col 9).
#line 1664
// NON_MATCHING: 524 vs 552; laid on DWARF lines 1665-1760 (tabs, case order from rows). The original compares m_recvState at 1719 with per-case constants (phi 0/1/2/3), ours with the loaded state; switch(oldState) and a separate INIT call tried.
bdBool bdLobbyConnection::recvMessageData()
{
	if (m_status != BD_CONNECTED)
	{
		return false;
	}

	RecvState oldState = m_recvState;
	bdInt status;

	switch (m_recvState)
	{
	case BD_READ_INIT:
		m_recvCount = 0;
		m_recvEncryptType = 0;
		m_messageSize = 0;
		m_recvMessage = NULL;
		m_recvTransfer = NULL;

		m_recvState = BD_READ_SIZE;



	case BD_READ_SIZE:
		status = recvMessageSize();
		break;

	case BD_READ_ENCRYPT:
		status = recvEncryptType();
		break;


	case BD_READ_MESSAGE:
		status = m_socket.recv(m_recvTransfer->getData(), m_recvTransfer->getAvail());

		if (status > 0 && m_recvTransfer->updateTransfer(status) == 0)
		{
			m_recvState = BD_READ_COMPLETE;
		}
		break;

	default:
		return true;
	}



	if (status > 0)
	{
		m_lastReceivedTimer.reset();
		m_lastReceivedTimer.start();
	}


	if (m_recvState != oldState)
	{
		return recvMessageData();
	}


	if (status <= 0) // 1725:18 unexplained
	{

		switch (status)
		{
		case BD_NET_CONNECTION_CLOSED:
			m_status = BD_DISCONNECTING;
			close();
			break;

		case BD_NET_CONNECTION_RESET:
			bdLogWarn("lobby connection", "Connection reset");
			close();
			break;

		case BD_NET_MSG_SIZE:
			bdLogWarn("lobby connection", "Message size error");
			break;

		case BD_NET_WOULD_BLOCK:
		case BD_NET_BLOCKING_CALL_CANCELED:
		case BD_NET_NOT_BOUND:
		case BD_NET_NOT_CONNECTED:
			break;



		default:

			bdLogWarn("lobby connection", "Receive error %i", status);
			break;
		}
	}

	return m_status == BD_CONNECTED;
}
#line 737


// Original line 1762.
bdInt bdLobbyConnection::recvMessageSize()
{
    bdInt sockStatus = m_socket.recv(m_msgSizeBuffer + m_recvCount, sizeof(m_msgSizeBuffer) - m_recvCount);
    if (sockStatus > 0) {
        m_recvCount += sockStatus;
        if (m_recvCount == sizeof(m_msgSizeBuffer)) {
            bdUInt offset = 0;
            bdBytePacker::removeBasicType<bdUInt32>(m_msgSizeBuffer, sizeof(m_msgSizeBuffer), offset, offset, m_messageSize);
            if (m_messageSize == 0) {
                m_recvState = BD_READ_INIT;
            } else if (m_messageSize > m_maxRecvMessageSize) {
                bdLogWarn("lobby connection", "Message too big");
                close();
                sockStatus = BD_NET_NOT_CONNECTED;
            } else {
                m_recvState = BD_READ_ENCRYPT;
            }
        }
    }
    return sockStatus;
}

// Original line 1798.
bdInt bdLobbyConnection::recvEncryptType()
{
    bdInt sockStatus = m_socket.recv(&m_recvEncryptType, sizeof(m_recvEncryptType));
    if (sockStatus > 0) {
        bdUInt headerSize;
        switch (m_recvEncryptType) {
        case 1:
        case 2:
            headerSize = 9;
            break;
        default:
            headerSize = 1;
            break;
        }
        m_recvMessage = new bdTaskByteBuffer(m_messageSize - 1 - headerSize, true);
        const bdUInt preHeaderSpaceSize = sizeof(bdUInt32) + sizeof(bdUByte8);
        m_recvMessage->setHeaderSize(headerSize + sizeof(bdUInt32) + sizeof(bdUByte8));
        bdUByte8 * preHeaderSpace = m_recvMessage->getHeaderStart();
        bdUInt preHeaderOffset = 0;
        bdBool ok = bdBytePacker::appendBasicType<bdUInt32>(preHeaderSpace, preHeaderSpaceSize, preHeaderOffset, preHeaderOffset, m_messageSize);
        ok = ok && bdBytePacker::appendBasicType<bdUByte8>(preHeaderSpace, preHeaderSpaceSize, preHeaderOffset, preHeaderOffset, m_recvEncryptType);
        m_recvMessage->setHeaderSize(headerSize);
        m_recvTransfer = new bdPendingBufferTransfer(m_recvMessage, m_messageSize - 1, false);
        m_recvState = BD_READ_MESSAGE;
    }
    return sockStatus;
}

// Original line 1863.
inline bdUInt32 bdLobbyConnection::getReceiveBufferSize() const
{
    return m_maxRecvMessageSize;
}

// Original line 1868.
bdBool bdLobbyConnection::connect(bdAuthInfo & authInfo)
{
    bdBool result;
    m_status = BD_CONNECTING;
    m_cryptoState = BD_CRYPTO210_INVALID;
    m_authInfo = authInfo;
    bdSingleton<bdTrulyRandomImpl>::getInstance()->getRandomUByte8(m_clientNonce, sizeof(m_clientNonce));
    bdMemcpy(m_sessionKey, authInfo.m_sessionKey, sizeof(m_sessionKey));
    bdSocketStatusCode status = m_socket.connect(m_lobbyAddr);
    switch (status) {
    case BD_NET_SUCCESS:
        m_status = BD_CONNECTED;
        onTcpConnect(true);
        result = true;
        break;
    case BD_NET_WOULD_BLOCK:
        m_asyncConnectTimer.reset();
        m_asyncConnectTimer.start();
        result = true;
        break;
    default:
        close();
        result = false;
        break;
    }
    return result;
}

// Original line 1913.
void bdLobbyConnection::disconnect()
{
    close();
}

// Original line 1918.
void bdLobbyConnection::close()
{
    if (m_status != BD_DISCONNECTED) {
        if (m_status == BD_CONNECTING && m_migrationState == BD_MIGRATION220_IDLE) {
            onTcpConnect(false);
        } else {
            callListenersDisconnect();
        }
    }
    m_status = BD_DISCONNECTED;
    m_cryptoState = BD_CRYPTO210_INVALID;
    m_migrationState = BD_MIGRATION220_IDLE;
    m_socket.close();
    m_outgoingBuffers = bdQueue<bdPendingBufferTransfer>();
    m_migrationOutgoingTaskQueue = bdQueue<bdLobbyConnectionMigrationHeldBackTask>();
}

// Original line 2014.
bdBool bdLobbyConnection::send210ClientHeader()
{
    bdByteBufferRef byteBuffer = new bdByteBuffer(28, false);
    bdBool ok = byteBuffer.notNull();
    ok = ok && byteBuffer->write<bdUInt32>(200);
    ok = ok && byteBuffer->write<bdUInt32>(200);
    ok = ok && byteBuffer->write<bdUInt32>(210);
    ok = ok && byteBuffer->write<bdUInt32>(220);
    ok = ok && byteBuffer->write<bdUInt32>(getReceiveBufferSize());
    ok = ok && byteBuffer->write(m_clientNonce, sizeof(m_clientNonce));
    sendFramedMessage(byteBuffer->getData(), byteBuffer->getDataSize());
    if (m_status == BD_CONNECTED) {
        m_cryptoState = BD_CRYPTO210_WAIT_SERVER_ACK;
        if (m_migrationState != BD_MIGRATION220_IDLE) {
            m_migrationState = BD_MIGRATION220_RECONNECTED;
        }
    }
    return ok;
}

// Original line 2053.
void bdLobbyConnection::onTcpConnect(const bdBool success)
{
    if (success) {
        m_keepAliveTimer.reset();
        m_keepAliveTimer.start();
        m_lastReceivedTimer.reset();
        m_lastReceivedTimer.start();
        if (m_connectionListener && m_migrationState == BD_MIGRATION220_IDLE) {
            m_connectionListener->onConnect(this);
        }
        if (m_isLSGConnection && !send210ClientHeader()) {
            bdLogError("lobby connection", "Failed to send client header");
        }
    } else if (m_connectionListener) {
        m_connectionListener->onConnectFailed(this);
    }
}

// Original line 2107.
inline void bdLobbyConnection::callListenersDisconnect()
{
    if (m_connectionListener) {
        m_connectionListener->onDisconnect(this);
    }
}

// Original line 2116.
bdBool bdLobbyConnection::pump()
{
    m_throttleBucket.fillBucket();
    if (m_status == BD_CONNECTING) {
        bdSocketStatusCode sockError = BD_NET_SUCCESS;
        if (m_socket.isWritable(sockError) && sockError == BD_NET_SUCCESS) {
            m_status = BD_CONNECTED;
            onTcpConnect(true);
        } else if (sockError != BD_NET_SUCCESS) {
            bdLogWarn("lobby connection", "Connect failed: %i", sockError);
            close();
            return false;
        } else {
            if ((m_migrationState == BD_MIGRATION220_IDLE && m_asyncConnectTimer.getElapsedTimeInSeconds() > 30.0f)
                || (m_migrationState != BD_MIGRATION220_IDLE && m_migrationReconnectTimeout < m_asyncConnectTimer.getElapsedTimeInSeconds())) {
                bdLogWarn("lobby connection", "Connect timed out");
                close();
                return false;
            }
            return true;
        }
    }
    while (m_status == BD_CONNECTED && !m_outgoingBuffers.isEmpty()) {
        bdPendingBufferTransfer & tx = m_outgoingBuffers.peek();
        bdInt32 bytesToSend = tx.getAvail();
        if (tx.getIsThrottled()) {
            const bdUInt32 availDataWithOverhead = m_throttleBucket.getAvailTokens(bytesToSend + 40);
            bytesToSend = static_cast<bdInt32>(availDataWithOverhead) > 0 ? availDataWithOverhead - 40 : availDataWithOverhead;
        }
        // DWARF 2178: sent is declared with the send call; cols 22/49 put a `bytesToSend > 0 ?` test in its
        // initialiser. The jump table sends the skipped case to the WOULD_BLOCK target (b3b9bc).
        const bdInt sent = bytesToSend > 0 ? m_socket.send(tx.getData(), bytesToSend) : BD_NET_WOULD_BLOCK;
        if (sent > 0) {
            m_keepAliveTimer.start();
            m_throttleBucket.removeTokens(sent + 40);
            if (tx.updateTransfer(sent) == 0) {
                m_outgoingBuffers.dequeue();
            }
        } else {
            switch (sent) {
            case BD_NET_ERROR:
                bdLogWarn("lobby connection", "Send failed: error");
                close();
                break;
            case BD_NET_SUBSYTEM_ERROR:
                bdLogWarn("lobby connection", "Send failed: subsystem error");
                close();
                break;
            case BD_NET_ADDRESS_INVALID:
                bdLogWarn("lobby connection", "Send failed: address invalid");
                close();
                break;
            case BD_NET_INVALID_HANDLE:
                bdLogWarn("lobby connection", "Send failed: invalid handle");
                close();
                break;
            case BD_NET_NOT_CONNECTED:
                bdLogWarn("lobby connection", "Send failed: not connected");
                close();
                break;
            case BD_NET_CONNECTION_RESET:
            case BD_NET_MSG_SIZE:
                close();
                break;
            case BD_NET_WOULD_BLOCK:
            case BD_NET_ADDRESS_IN_USE:
                break;
            default:
                bdLogWarn("lobby connection", "Send failed %i", sent);
                close();
                break;
            }
            return m_status == BD_CONNECTED;
        }
    }
    bdInt socketResult;
    if (m_status == BD_CONNECTED && m_cryptoState == BD_CRYPTO210_READY && m_keepAliveTimer.getElapsedTimeInSeconds() > 40.0f) {
        bdUInt32 keepAlive = 0;
        socketResult = m_socket.send(&keepAlive, sizeof(keepAlive));
        if (socketResult < 0 && socketResult != BD_NET_WOULD_BLOCK) {
            bdLogWarn("lobby connection", "Failed to send keep alive");
            close();
            return false;
        }
        m_keepAliveTimer.start();
    }
    if (m_lastReceivedTimer.getElapsedTimeInSeconds() > 120.0f) {
        bdLogWarn("lobby connection", "Connection timed out");
        close();
    }
    return true;
}

// Original line 2313.
inline bdBool bdLobbyConnection::receivedFullMessage()
{
    return m_recvState == BD_READ_COMPLETE;
}

// Original line 2318.
bdTokenBucket & bdLobbyConnection::getThrottleBucket()
{
    return m_throttleBucket;
}

// Original line 2323.
void bdLobbyConnection::setTasksAreThrottled(bdBool areThrottled)
{
    m_tasksAreThrottled = areThrottled;
}

// Original line 2328.
inline bdPendingBufferTransfer::bdPendingBufferTransfer(bdTaskByteBufferRef buffer, bdUInt totalSize, bdBool isThrottled)
    : bdReferencable(), m_buffer(buffer), m_txPtr(buffer->getHeaderStart()), m_isThrottled(isThrottled)
{
    m_txAvail = m_txPtr ? totalSize : 0;
}

// Original line 2338.
inline bdPendingBufferTransfer::bdPendingBufferTransfer(bdByteBufferRef buffer, bdUInt totalSize, bdBool isThrottled)
    : bdReferencable(), m_buffer(buffer), m_txPtr(buffer->getData()), m_isThrottled(isThrottled)
{
    m_txAvail = m_txPtr ? totalSize : 0;
}

// Original line 2348.
inline bdPendingBufferTransfer::bdPendingBufferTransfer(const bdPendingBufferTransfer & other)
    : bdReferencable(), m_buffer(other.m_buffer), m_txPtr(other.m_txPtr), m_txAvail(other.m_txAvail),
      m_isThrottled(other.m_isThrottled)
{
}

// Original line 2356.
bdPendingBufferTransfer::~bdPendingBufferTransfer()
{
}

// Original line 2360.
inline bdUByte8 * bdPendingBufferTransfer::getData() const
{
    return m_txPtr;
}

// Original line 2365.
inline bdUInt32 bdPendingBufferTransfer::getAvail() const
{
    return m_txAvail;
}

// Original line 2370.
inline bdUInt32 bdPendingBufferTransfer::updateTransfer(bdUInt32 amountTransfered)
{
    const bdUInt32 amount = amountTransfered < m_txAvail ? amountTransfered : m_txAvail;
    m_txPtr += amount;
    m_txAvail -= amount;
    return m_txAvail;
}

// Original line 2380.
inline bdBool bdPendingBufferTransfer::getIsThrottled() const
{
    return m_isThrottled;
}
