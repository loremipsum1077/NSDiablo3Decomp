// Original file: Contrib/Contrib/DemonWare/bdSocket/bdDTLS/bdDTLSAssociation.cpp
// Reconstructed from original DWARF, source-line and instruction evidence.

#include "bdDTLSAssociation.h"
#include "../../bdCore/bdCrypto/bdCryptoUtils.h"
#include "../../bdCore/bdCrypto/bdECCKey.h"
#include "../../bdCore/bdCrypto/bdHMac.h"
#include "../../bdCore/bdUtilities/bdBytePacker.h"
#include "../../bdCore/bdSocket/bdCommonAddrInfo.h"
#include "../../bdCore/bdSocket/bdSocket.h"
#include "../../bdPlatform/bdPlatformLog/bdPlatformLog.h"
#include "../../bdPlatform/bdPlatformTiming/bdPlatformTiming.h"
#include "../bdAddressMap.h"
#include "../bdSecurityKeyMap.h"
#include "bdPacket/bdDTLSCookieAck.h"
#include "bdPacket/bdDTLSCookieEcho.h"
#include "bdPacket/bdDTLSData.h"
#include "bdPacket/bdDTLSInit.h"
#include "bdDTLSUtils.h"

bdNonce<16> bdDTLSAssociation::m_cookieKey;

bdDTLSAssociation::bdDTLSAssociation(bdDefaultSocket * socket, bdSecurityKeyMap * keyMap, bdECCKey * ECCkey, const bdAddr & addr, bdAddrHandleRef addrHandle, bdCommonAddrRef localCommonAddr, bdAddressMap * addrMap, bdFloat32 receiveTimeout, const bdCypherSuite cypherSuite)
    : m_socket(socket),
      m_keyMap(keyMap),
      m_ECCKey(ECCkey),
      m_cypherSuite(cypherSuite),
      m_cypherAlgorithm(bdCypherSuites::getCypherFromSuite(cypherSuite)),
      m_sendingCypher(bdCypherSuites::getCypherFromSuite(cypherSuite)),
      m_receivingCypher(bdCypherSuites::getCypherFromSuite(cypherSuite)),
      m_hashAlgorithm(bdCypherSuites::getHashFromSuite(cypherSuite)),
      m_hash(bdCypherSuites::getHashFromSuite(cypherSuite)),
      m_addr(addr),
      m_addrHandle(addrHandle),
      m_state(BD_DTLS_CLOSED),
      m_lastReceived(),
      m_initTimer(),
      m_initResends(0),
      m_cookieTimer(),
      m_cookieResends(0),
      m_localRandom(),
      m_peerRandom(),
      m_seqNum(-1),
      m_incomingSeqNums(bdSequenceNumber(-1)),
      m_initAck(),
      m_localId(),
      m_localCommonAddr(localCommonAddr),
      m_addrMap(addrMap),
      m_receiveTimeout(receiveTimeout)
{
    reset();
    m_cookieKey.ensureCreated();
}

bdDTLSAssociation::~bdDTLSAssociation()
{
    bdCryptoUtils::zeroBuffer(m_sendingIVMaterial, sizeof(m_sendingIVMaterial));
    bdCryptoUtils::zeroBuffer(m_receivingIVMaterial, sizeof(m_receivingIVMaterial));
    m_socket = NULL;
    m_keyMap = NULL;
    m_ECCKey = NULL;
}

void bdDTLSAssociation::connect()
{
    sendInit();
    m_state = BD_DTLS_COOKIE_WAIT;
}

bdDTLSAssociationStatus bdDTLSAssociation::getStatus()
{
    bdDTLSAssociationStatus status;
    switch (m_state)
    {
    case BD_DTLS_CLOSED:
        status = BD_SOCKET_LOST;
        break;
    case BD_DTLS_COOKIE_WAIT:
    case BD_DTLS_COOKIE_ECHOED:
        status = BD_SOCKET_PENDING;
        break;
    case BD_DTLS_ESTABLISHED:
        status = BD_SOCKET_CONNECTED;
        break;
    default:
    {
        bdLogError("dtls", "Invalid DTLS state.");
        bdNChar8 commonAddrInfo[1024];
        bdCommonAddrInfo::getBriefInfo(m_addrHandle->m_endpoint.getCommonAddr(), commonAddrInfo, sizeof(commonAddrInfo));
        bdLogError("dtls", "Common address info follows.");
        status = BD_SOCKET_LOST;
        break;
    }
    }
    return status;
}

bdInt bdDTLSAssociation::sendTo(const bdAddr & addr, const void * data, const bdUInt length)
{
    bdInt result = -1;
    switch (m_state)
    {
    case BD_DTLS_CLOSED:
    case BD_DTLS_COOKIE_WAIT:
    case BD_DTLS_COOKIE_ECHOED:
        bdLogWarn("dtls", "Cannot send, association not established.");
        break;
    case BD_DTLS_ESTABLISHED:
        result = sendData(addr, data, length);
        break;
    }
    return result;
}

bdInt bdDTLSAssociation::receiveFrom(bdAddr & addr, const void * data, const bdUInt size, bdAddrHandleRef & addrHandle, void * buffer, const bdUInt bufferSize)
{
    bdInt result = -2;
    bdDTLSHeader header;
    bdUInt bytesRead;
    if (header.deserialize(data, size, 0, bytesRead))
    {
        const bdDTLSPacketTypes type = static_cast<bdDTLSPacketTypes>(header.getType());
        switch (type)
        {
        case BD_DTLS_INIT:
            handleInit(addr, data, size);
            break;
        case BD_DTLS_INIT_ACK:
            handleInitAck(addr, data, size);
            break;
        case BD_DTLS_COOKIE_ECHO:
            handleCookieEcho(addr, data, size);
            break;
        case BD_DTLS_COOKIE_ACK:
            handleCookieAck(data, size);
            break;
        case BD_DTLS_ERROR:
            handleError(addr, data, size);
            break;
        case BD_DTLS_DATA:
            result = handleData(addr, static_cast<const bdUByte8 *>(data), size, addrHandle, header.getVtag(), static_cast<bdUByte8 *>(buffer), bufferSize);
            break;
        }
    }
    return result;
}

void bdDTLSAssociation::pump()
{
    switch (m_addrHandle->getStatus())
    {
    case bdAddrHandle::BD_ADDR_NOT_CONSTRUCTED:
    {
        bdNChar8 addrString[22];
        m_addrHandle->getRealAddr().toString(addrString, sizeof(addrString));
        bdLogWarn("dtls", "Address handle for %s is not constructed.", addrString);
        m_state = BD_DTLS_CLOSED;
        break;
    }
    case bdAddrHandle::BD_ADDR_NOT_RESOLVED:
    {
        bdNChar8 addrString[22];
        m_addrHandle->getRealAddr().toString(addrString, sizeof(addrString));
        bdLogWarn("dtls", "Address handle for %s is not resolved.", addrString);
        m_state = BD_DTLS_CLOSED;
        break;
    }
    case bdAddrHandle::BD_ADDR_ERROR:
        bdLogWarn("dtls", "Address handle error.");
        m_state = BD_DTLS_CLOSED;
        break;
    case bdAddrHandle::BD_ADDR_RESOLVED:
        break;
    case bdAddrHandle::BD_ADDR_UNRESOLVED:
        bdLogWarn("dtls", "Address handle failed to resolve.");
        m_state = BD_DTLS_CLOSED;
        break;
    }
    if (m_lastReceived.getElapsedTimeInSeconds() > m_receiveTimeout)
    {
        bdNChar8 addrString[22];
        m_addrHandle->getRealAddr().toString(addrString, sizeof(addrString));
        bdLogInfo("dtls", "Connection to %s timed out.", addrString);
        m_state = BD_DTLS_CLOSED;
    }
    else
    {
        switch (m_state)
        {
        case BD_DTLS_COOKIE_WAIT:
            if (m_initTimer.getElapsedTimeInSeconds() > 1.0f)
            {
                sendInit();
            }
            break;
        case BD_DTLS_COOKIE_ECHOED:
            if (m_cookieTimer.getElapsedTimeInSeconds() > 1.0f)
            {
                sendCookieEcho(m_addr);
            }
            break;
        default:
            break;
        }
    }
}

bdAddrHandleRef bdDTLSAssociation::getAddrHandle()
{
    return m_addrHandle;
}

const bdSecurityID * bdDTLSAssociation::getLocalSecurityId()
{
    return &m_localId;
}

bdInt bdDTLSAssociation::handleInit(const bdAddr & addr, const void * data, const bdUInt size)
{
    bdDTLSInit init;
    bdUInt bytesRead;
    if (init.deserialize(data, size, 0, bytesRead, m_cypherSuite))
    {
        bdLogInfo("dtls", "<- INIT (localTag: %u, initTag: %u)", m_localTag, init.getInitTag());
        sendInitAck(addr, init);
        m_lastReceived.start();
    }
    else
    {
        bdLogWarn("dtls", "Failed to deserialize INIT.");
    }
    return -2;
}

bdInt bdDTLSAssociation::handleInitAck(const bdAddr & addr, const void * data, const bdUInt size)
{
    if (m_state == BD_DTLS_COOKIE_WAIT)
    {
        bdUInt bytesRead;
        bdDTLSInitAck initAck;
        if (initAck.deserialize(data, size, 0, bytesRead, m_cypherSuite))
        {
            m_initAck = initAck;
            if (m_initAck.getVtag() == m_localTag)
            {
                bdBool ok = true;
                if (BD_DTLS_VERSION > 2)
                {
                    if (m_localRandom != m_initAck.getPeerRandom())
                    {
                        ok = false;
                    }
                }
                if (ok)
                {
                    m_peerTag = m_initAck.getInitTag();
                    m_peerRandom = m_initAck.getLocalRandom();
                    bdLogInfo("dtls", "<- INIT ACK (localTag: %u, peerTag: %u)", m_localTag, m_peerTag);
                    sendCookieEcho(addr);
                    m_lastReceived.start();
                    m_state = BD_DTLS_COOKIE_ECHOED;
                }
                else
                {
                    bdLogWarn("dtls", "INIT ACK random mismatch.");
                }
            }
            else
            {
                bdLogWarn("dtls", "INIT ACK bad vtag.");
            }
        }
        else
        {
            bdLogWarn("dtls", "Failed to deserialize INIT ACK.");
        }
    }
    return -2;
}

// NON_MATCHING: 2512 bytes both; 9 of 628 instructions differ: at the setup-collision test (original line 551) ours hoists and w25,w24,#0xffff and reuses it for the log argument, the original compares cmp w1,w24,uxth first then tst w24; tried peerTag/m_peerTag operand order and m_peerTag == 0.
bdInt bdDTLSAssociation::handleCookieEcho(const bdAddr & addr, const void * data, const bdUInt size)
{
    bdBool result = false;
    bdDTLSCookieEcho cookieEcho;
    bdUInt tmpUInt;
    if (cookieEcho.deserialize(data, size, 0, tmpUInt, m_cypherSuite))
    {
        if (!checkCookieValidity(addr, cookieEcho))
        {
            // Inferred: early exit; the original branches from the log at line 377 straight to ~bdDTLSCookieEcho, and localTag (381) shares the lexical block that starts at line 375.
            bdLogWarn("dtls", "Invalid COOKIE ECHO.");
            return -2;
        }
        const bdUInt16 localTag = cookieEcho.getCookie().getLocalTag();
        const bdUInt16 peerTag = cookieEcho.getCookie().getPeerTag();
        const bdUInt16 localTieTag = cookieEcho.getCookie().getLocalTieTag();
        const bdUInt16 peerTieTag = cookieEcho.getCookie().getPeerTieTag();
        const bdDTLSRandom & localRandom = cookieEcho.getCookie().getLocalRandom();
        const bdDTLSRandom & peerRandom = cookieEcho.getCookie().getPeerRandom();
        bdSecurityID secID;
        bdCommonAddrRef commonAddr = new bdCommonAddr();
        bdMemcpy(secID.ab, cookieEcho.getSecID(), sizeof(secID.ab));
        result = commonAddr->deserialize(m_localCommonAddr, cookieEcho.getCa());
        if (result)
        {
            switch (m_state)
            {
            case BD_DTLS_CLOSED:
                m_localTag = localTag;
                m_peerTag = peerTag;
                if (BD_DTLS_VERSION > 2)
                {
                    m_localRandom = localRandom;
                    m_peerRandom = peerRandom;
                }
                if (!m_keyMap->contains(secID))
                {
                    bdLogWarn("dtls", "Unknown security ID in COOKIE ECHO.");
                    sendError(addr, secID, bdDTLSError::BD_DTLS_ERROR_BAD_SECID);
                    // Inferred: the four sendError sites are tail-merged in the original; m_state = BD_DTLS_CLOSED follows ~bdDTLSError there.
                    m_state = BD_DTLS_CLOSED;
                }
                else if (calculateSharedKey(cookieEcho.getECCKey(), 100, secID))
                {
                    result = m_addrMap->getAddrHandle(commonAddr, secID, m_addrHandle);
                    if (result)
                    {
                        m_addrHandle->setRealAddr(addr);
                        m_addr = addr;
                        bdLogInfo("dtls", "<- COOKIE ECHO (localTag: %u, peerTag: %u)", m_localTag, m_peerTag);
                        sendCookieAck(addr, cookieEcho);
                        m_state = BD_DTLS_ESTABLISHED;
                        bdLogInfo("dtls", "Association established.");
                        m_localId = secID;
                        // Inferred: an explicit result = true on the success path; the original threads these paths straight to m_lastReceived.start() (DWARF line 703) while the sendError/discard paths keep result in a register.
                        result = true;
                    }
                }
                else
                {
                    bdLogWarn("dtls", "Failed to calculate shared key.");
                    result = false;
                }
                break;
            default:
                if (m_localTag != localTag && m_peerTag != peerTag && m_localTag == localTieTag && m_peerTag == peerTieTag)
                {
                    if (!m_keyMap->contains(secID))
                    {
                        bdLogWarn("dtls", "Unknown security ID in COOKIE ECHO.");
                        sendError(addr, secID, bdDTLSError::BD_DTLS_ERROR_BAD_SECID);
                        m_state = BD_DTLS_CLOSED;
                    }
                    else
                    {
                        reset();
                        m_localTag = localTag;
                        m_peerTag = peerTag;
                        if (BD_DTLS_VERSION > 2)
                        {
                            m_localRandom = localRandom;
                            m_peerRandom = peerRandom;
                        }
                        if (calculateSharedKey(cookieEcho.getECCKey(), 100, secID))
                        {
                            m_addrMap->getAddrHandle(commonAddr, secID, m_addrHandle);
                            m_addrHandle->setRealAddr(addr);
                            m_addr = addr;
                            sendCookieAck(addr, cookieEcho);
                            bdLogInfo("dtls", "Peer restarted (localTag: %u, peerTag: %u)", m_localTag, m_peerTag);
                            m_state = BD_DTLS_ESTABLISHED;
                            bdLogInfo("dtls", "Association established.");
                            m_localId = secID;
                            // Inferred: an explicit result = true on the success path; the original threads these paths straight to m_lastReceived.start() (DWARF line 703) while the sendError/discard paths keep result in a register.
                            result = true;
                        }
                        else
                        {
                            bdLogWarn("dtls", "Failed to calculate shared key.");
                            result = false;
                        }
                    }
                }
                // Columns at original line 551 (|| at col 64) show the parenthesised (a || b) form; the second operand is inferred from tst of the cookie peerTag.
                else if (m_localTag == localTag && (m_peerTag != peerTag || peerTag == 0))
                {
                    if (m_addrHandle->getStatus() != bdAddrHandle::BD_ADDR_RESOLVED)
                    {
                        bdLogWarn("dtls", "Address handle not resolved.");
                    }
                    m_peerTag = peerTag;
                    bdLogInfo("dtls", "Setup collision (localTag: %u, peerTag: %u)", m_localTag, m_peerTag);
                    if (BD_DTLS_VERSION > 2)
                    {
                        m_localRandom = localRandom;
                        m_peerRandom = peerRandom;
                    }
                    if (!m_keyMap->contains(secID))
                    {
                        bdLogWarn("dtls", "Unknown security ID in COOKIE ECHO.");
                        sendError(addr, secID, bdDTLSError::BD_DTLS_ERROR_BAD_SECID);
                        m_state = BD_DTLS_CLOSED;
                    }
                    else if (calculateSharedKey(cookieEcho.getECCKey(), 100, secID))
                    {
                        m_addrMap->getAddrHandle(commonAddr, secID, m_addrHandle);
                        m_addrHandle->setRealAddr(addr);
                        m_addr = addr;
                        sendCookieAck(addr, cookieEcho);
                        m_state = BD_DTLS_ESTABLISHED;
                        bdLogInfo("dtls", "Association established.");
                        m_localId = secID;
                        // Inferred: an explicit result = true on the success path; the original threads these paths straight to m_lastReceived.start() (DWARF line 703) while the sendError/discard paths keep result in a register.
                        result = true;
                    }
                    else
                    {
                        bdLogWarn("dtls", "Failed to calculate shared key.");
                        result = false;
                    }
                }
                else if (m_localTag != localTag && m_peerTag == peerTag)
                {
                    bdLogInfo("dtls", "Discarding COOKIE ECHO (localTag: %u, peerTag: %u)", m_localTag, m_peerTag);
                }
                else if (m_localTag == localTag && m_peerTag == peerTag)
                {
                    if (!m_keyMap->contains(secID))
                    {
                        bdLogWarn("dtls", "Unknown security ID in COOKIE ECHO.");
                        sendError(addr, secID, bdDTLSError::BD_DTLS_ERROR_BAD_SECID);
                        m_state = BD_DTLS_CLOSED;
                    }
                    else
                    {
                        if (BD_DTLS_VERSION > 2)
                        {
                            m_localRandom = localRandom;
                            m_peerRandom = peerRandom;
                        }
                        if (calculateSharedKey(cookieEcho.getECCKey(), 100, secID))
                        {
                            m_addrMap->getAddrHandle(commonAddr, secID, m_addrHandle);
                            m_addrHandle->setRealAddr(addr);
                            m_addr = addr;
                            sendCookieAck(addr, cookieEcho);
                            m_state = BD_DTLS_ESTABLISHED;
                            m_localId = secID;
                            bdLogInfo("dtls", "Duplicate COOKIE ECHO (localTag: %u, peerTag: %u)", m_localTag, m_peerTag);
                            // Inferred: an explicit result = true on the success path; the original threads these paths straight to m_lastReceived.start() (DWARF line 703) while the sendError/discard paths keep result in a register.
                            result = true;
                        }
                        else
                        {
                            bdLogWarn("dtls", "Failed to calculate shared key.");
                            result = false;
                        }
                    }
                }
                else
                {
                    bdLogWarn("dtls", "Unexpected COOKIE ECHO.");
                    result = false;
                }
                break;
            }
        }
    }
    if (result)
    {
        m_lastReceived.start();
    }
    return -2;
}

bdInt bdDTLSAssociation::handleCookieAck(const void * data, const bdUInt size)
{
    bdDTLSCookieAck cookieAck;
    bdUInt tmpUInt;
    if (cookieAck.deserialize(data, size, 0, tmpUInt) && cookieAck.getVtag() == m_localTag)
    {
        bdSecurityID secID;
        cookieAck.getSecID(secID);
        switch (m_state)
        {
        case BD_DTLS_CLOSED:
        case BD_DTLS_COOKIE_WAIT:
        {
            const bdUInt tempState = m_state;
            bdLogWarn("dtls", "Unexpected COOKIE ACK in state %u.", tempState);
            break;
        }
        case BD_DTLS_COOKIE_ECHOED:
            if (calculateSharedKey(cookieAck.getECCKey(), 100, secID))
            {
                m_lastReceived.start();
                m_state = BD_DTLS_ESTABLISHED;
                bdLogInfo("dtls", "Association established.");
                m_localId = secID;
            }
            else
            {
                bdNChar8 addrBuffer[22];
                m_addr.toString(addrBuffer, sizeof(addrBuffer));
                bdLogWarn("dtls", "Failed to calculate shared key with %s.", addrBuffer);
            }
            break;
        case BD_DTLS_ESTABLISHED:
            bdLogInfo("dtls", "Duplicate COOKIE ACK.");
            break;
        }
    }
    else
    {
        bdLogWarn("dtls", "Discarding COOKIE ACK.");
    }
    return -2;
}

bdInt bdDTLSAssociation::handleError(const bdAddr & addr, const void * data, const bdUInt size)
{
    bdDTLSError error;
    bdUInt tmpUInt;
    if (error.deserialize(data, size, 0, tmpUInt) && error.getVtag() == m_localTag)
    {
        bdNChar8 addrStr[22];
        const bdUInt maxAddrLength = sizeof(addrStr);
        addr.toString(addrStr, maxAddrLength);
        switch (error.getEtype())
        {
        case bdDTLSError::BD_DTLS_ERROR_BAD_SECID:
            bdLogWarn("dtls", "<- ERROR (bad security ID) from %s", addrStr);
            m_state = BD_DTLS_CLOSED;
            break;
        default:
            bdLogWarn("dtls", "<- ERROR from %s", addrStr);
            break;
        }
    }
    return -2;
}

bdInt bdDTLSAssociation::handleData(bdAddr & addr, const bdUByte8 * data, const bdUInt size, bdAddrHandleRef & addrHandle, const bdUInt16 vtag, bdUByte8 * buffer, const bdUInt bufferSize)
{
    bdInt val = -2;
    if (m_state == BD_DTLS_ESTABLISHED)
    {
        if (m_addrHandle->m_endpoint.getCommonAddr().notNull())
        {
            if (m_localTag == vtag)
            {
                bdUByte8 safetyBuffer[1288];
                bdDTLSData dtlsData;
                bdUInt dtlsDataSize;
                bdUInt payloadSize;
                if (dtlsData.deserialize(data, size, 0, dtlsDataSize, m_incomingSeqNums.getLastSequenceNumber(), m_receivingIVMaterial, safetyBuffer, sizeof(safetyBuffer), payloadSize, &m_receivingCypher.getCypher(), &m_hash.getHash(), m_cypherSuite) && verify(dtlsData))
                {
                    if (payloadSize <= bufferSize)
                    {
                        val = payloadSize;
                        addr = m_addr;
                        addrHandle = m_addrHandle;
                        m_lastReceived.start();
                        bdMemcpy(buffer, safetyBuffer, payloadSize);
                    }
                    else
                    {
                        bdLogWarn("dtls", "Buffer too small.");
                    }
                }
            }
            else
            {
                bdLogWarn("dtls", "Discarding DATA with bad vtag.");
            }
        }
        else
        {
            bdLogWarn("dtls", "Discarding DATA, no common address.");
        }
    }
    return val;
}

void bdDTLSAssociation::sendInit()
{
    if (m_initResends++ < 6)
    {
        bdSecurityID secID(m_addrHandle->m_endpoint.getSecID());
        bdDTLSInit init(m_localTag, secID, m_localRandom, m_cypherSuite);
        bdUInt bytesWritten;
        bdUByte8 buffer[1288];
        init.serialize(buffer, sizeof(buffer), 0, bytesWritten);
        const bdInt sendResult = m_socket->sendTo(m_addr, buffer, bytesWritten);
        if (sendResult != bytesWritten)
        {
            bdLogWarn("dtls", "Failed to send INIT.");
        }
        m_initTimer.start();
        bdLogInfo("dtls", "-> INIT (localTag: %u)", m_localTag);
    }
    else
    {
        bdLogInfo("dtls", "Too many INIT resends.");
        m_state = BD_DTLS_CLOSED;
    }
}

void bdDTLSAssociation::sendInitAck(const bdAddr & addr, const bdDTLSInit & init)
{
    bdBool result = true;
    bdUInt16 localTieTag;
    bdUInt16 peerTieTag;
    bdUInt16 localTag;
    const bdUInt16 peerTag = init.getInitTag();
    bdDTLSRandom peerRandom(init.getRandomData());
    bdDTLSRandom localRandom(m_localRandom);
    switch (m_state)
    {
    case BD_DTLS_CLOSED:
        localTag = m_localTag;
        localTieTag = 0;
        peerTieTag = 0;
        break;
    case BD_DTLS_COOKIE_WAIT:
        localTag = m_localTag;
        localTieTag = 0;
        peerTieTag = 0;
        break;
    case BD_DTLS_COOKIE_ECHOED:
        localTag = m_localTag;
        peerTieTag = m_peerTag;
        localTieTag = m_localTag;
        break;
    case BD_DTLS_ESTABLISHED:
        localTag = makeTag();
        localRandom.generate();
        peerTieTag = m_peerTag;
        localTieTag = m_localTag;
        break;
    default:
        return;
    }
    if (result)
    {
        bdSecurityID secID;
        init.getSecID(secID);
        bdDTLSInitAck initAck(peerTag, localTag, localTag, peerTag, localTieTag, peerTieTag, m_cypherSuite, localRandom, peerRandom, bdPlatformTiming::getLoResTimeStamp(), addr, secID);
        bdDynamicHMAC hmacObj(m_hashAlgorithm, m_cookieKey.getData(), 16);
        if (initAck.sign(hmacObj.getHMAC()))
        {
            bdUInt bytesWritten;
            bdUByte8 buffer[1288];
            initAck.serialize(buffer, sizeof(buffer), 0, bytesWritten);
            const bdInt sendResult = m_socket->sendTo(addr, buffer, bytesWritten);
            if (sendResult != bytesWritten)
            {
                bdLogWarn("dtls", "Failed to send INIT ACK.");
            }
            bdLogInfo("dtls", "-> INIT ACK (localTag: %u, tag: %u, peerTag: %u)", m_localTag, localTag, peerTag);
        }
        else
        {
            bdLogWarn("dtls", "Failed to sign INIT ACK.");
        }
    }
}

void bdDTLSAssociation::sendCookieEcho(const bdAddr & addr)
{
    if (m_cookieResends++ < 6)
    {
        bdDTLSCookieEcho cookieEcho(m_peerTag, m_initAck, m_localCommonAddr, m_ECCKey);
        bdUInt bytesWritten;
        bdUByte8 buffer[1288];
        cookieEcho.serialize(buffer, sizeof(buffer), 0, bytesWritten);
        const bdInt sendResult = m_socket->sendTo(addr, buffer, bytesWritten);
        if (sendResult == bytesWritten)
        {
            bdLogInfo("dtls", "-> COOKIE ECHO (localTag: %u, peerTag: %u)", m_localTag, m_peerTag);
            m_state = BD_DTLS_COOKIE_ECHOED;
        }
        else
        {
            bdLogWarn("dtls", "Failed to send COOKIE ECHO.");
        }
        m_cookieTimer.start();
    }
    else
    {
        bdLogWarn("dtls", "Too many COOKIE ECHO resends.");
        m_state = BD_DTLS_CLOSED;
    }
}

void bdDTLSAssociation::sendCookieAck(const bdAddr & addr, const bdDTLSCookieEcho &)
{
    bdUInt keyLen = 100;
    bdUByte8 key[100];
    const bdBool exported = m_ECCKey->exportKey(key, keyLen);
    if (keyLen != sizeof(key) || !exported)
    {
        bdLogWarn("dtls", "Failed to export ECC key.");
    }
    const bdSecurityID & secID = m_addrHandle->m_endpoint.getSecID();
    bdDTLSCookieAck cookieAck(m_peerTag, key, secID);
    bdUInt bytesWritten;
    bdUByte8 buffer[1288];
    cookieAck.serialize(buffer, sizeof(buffer), 0, bytesWritten);
    const bdInt sendResult = m_socket->sendTo(addr, buffer, bytesWritten);
    if (sendResult != bytesWritten)
    {
        bdLogWarn("dtls", "Failed to send COOKIE ACK.");
    }
    bdLogInfo("dtls", "-> COOKIE ACK (localTag: %u, peerTag: %u)", m_localTag, m_peerTag);
}

// The original's out-of-line copy was removed by the linker (DWARF concrete DIE has low_pc 0); it survives inlined into handleCookieEcho.
void bdDTLSAssociation::sendError(const bdAddr & addr, const bdSecurityID & secID, const bdDTLSError::bdDTLSErrorType & type)
{
    bdUByte8 buffer[1288];
    bdDTLSError error(m_peerTag, type, secID);
    bdUInt bytesWritten;
    error.serialize(buffer, sizeof(buffer), 0, bytesWritten);
    const bdInt sendResult = m_socket->sendTo(addr, buffer, bytesWritten);
    if (sendResult != static_cast<bdInt>(bytesWritten))
    {
        bdLogWarn("dtls", "Failed to send ERROR.");
    }
    bdLogInfo("dtls", "-> ERROR (type: %u)", type);
}

bdInt bdDTLSAssociation::sendData(const bdAddr & addr, const void * data, const bdUInt length)
{
    bdInt result;
    if (length < 1256)
    {
        bdUByte8 outData[1288];
        m_seqNum++;
        bdDTLSData dataPacket(m_peerTag, m_seqNum.getValue());
        bdUInt packetLength;
        if (dataPacket.serialize(outData, sizeof(outData), 0, packetLength, m_seqNum, m_sendingIVMaterial, static_cast<const bdUByte8 *>(data), length, &m_sendingCypher.getCypher(), &m_hash.getHash(), m_cypherSuite))
        {
            result = m_socket->sendTo(addr, outData, packetLength);
        }
        else
        {
            bdLogWarn("dtls", "Failed to serialize DATA.");
            result = -1;
            return result;
        }
    }
    else
    {
        result = -6;
    }
    return result;
}

inline bdUInt16 bdDTLSAssociation::makeTag() const
{
    bdUInt16 result;
    do
    {
        result = static_cast<bdUInt16>(bdSingleton<bdTrulyRandomImpl>::getInstance()->getRandomUInt());
    } while (result == 0);
    return result;
}

void bdDTLSAssociation::reset()
{
    m_initResends = 0;
    m_cookieResends = 0;
    m_seqNum = bdSequenceNumber(0);
    m_localTag = makeTag();
    m_peerTag = 0;
    m_localRandom.generate();
    m_peerRandom.generate();
    bdCryptoUtils::zeroBuffer(m_sendingIVMaterial, sizeof(m_sendingIVMaterial));
    bdCryptoUtils::zeroBuffer(m_receivingIVMaterial, sizeof(m_receivingIVMaterial));
    m_lastReceived.start();
    m_incomingSeqNums = bdSequenceNumberStore(bdSequenceNumber(-1));
}

bdBool bdDTLSAssociation::verify(const bdDTLSData & dataPacket)
{
    bdSequenceNumber seqNum(-1);
    bdBool result = false;
    if (dataPacket.getVtag() == m_localTag)
    {
        const bdSequenceNumber last(m_incomingSeqNums.getLastSequenceNumber());
        const bdSequenceNumber newSeqNum(last, dataPacket.getCounter(), 16);
        switch (m_incomingSeqNums.check(newSeqNum))
        {
        case bdSequenceNumberStore::BD_SN_VALID_SMALLER:
        case bdSequenceNumberStore::BD_SN_VALID_LARGER:
        case bdSequenceNumberStore::BD_SN_VALID_MUCH_LARGER:
            seqNum = newSeqNum;
            result = true;
            break;
        case bdSequenceNumberStore::BD_SN_INVALID_SMALLER:
            bdLogWarn("dtls", "Sequence number %d too old (last %d).", newSeqNum.getValue(), last.getValue());
            break;
        default:
            bdLogWarn("dtls", "Invalid sequence number %d (last %d).", newSeqNum.getValue(), last.getValue());
            break;
        }
    }
    else
    {
        bdLogWarn("dtls", "Bad vtag on DATA.");
    }
    return result;
}

bdBool bdDTLSAssociation::checkCookieValidity(const bdAddr & peerAddr, const bdDTLSCookieEcho & cookiePacket) const
{
    const bdUInt age = bdPlatformTiming::getLoResElapsedTime(cookiePacket.getCookie().getTimestamp(), bdPlatformTiming::getLoResTimeStamp());
    if (age < 60)
    {
        if (cookiePacket.getCookie().getPeerAddr() == peerAddr)
        {
            bdDynamicHMAC hmacObj(m_hashAlgorithm, m_cookieKey.getData(), 16);
            const bdBool verifyHMAC = cookiePacket.getCookie().verify(hmacObj.getHMAC());
            if (!verifyHMAC)
            {
                bdLogWarn("dtls", "Cookie HMAC verification failed.");
            }
            return verifyHMAC;
        }
        else
        {
            bdLogWarn("dtls", "Cookie address mismatch.");
        }
    }
    else
    {
        bdLogWarn("dtls", "Cookie expired (age %u).", age);
    }
    return false;
}

bdBool bdDTLSAssociation::calculateSharedKey(const bdUByte8 *const pubKey, const bdUInt keylen, const bdSecurityID & secID)
{
    bdSecurityKey securityKey;
    if (!m_keyMap->get(secID, securityKey))
    {
        bdLogWarn("dtls", "Failed to find security key.");
        return false;
    }
    const bdUInt BD_DTLS_SHARED_SECRET_LENGTH = 44;
    bdUByte8 sharedSecret[BD_DTLS_SHARED_SECRET_LENGTH];
    if (!m_ECCKey->generateSharedSecret(pubKey, keylen, sharedSecret, BD_DTLS_SHARED_SECRET_LENGTH))
    {
        bdLogWarn("dtls", "Failed to generate shared secret.");
        return false;
    }
    bdUByte8 masterSecret[32] = {0};
    const bdUInt cypherKeySize = bdCryptoDynamic::getCypherAlgorithmKeySize(m_cypherAlgorithm);
    // Inferred: DWARF bdDTLSAssociation.cpp:1208 declares cypherBlockSize with no location; its initialiser left no code.
    const bdUInt cypherBlockSize = sizeof(m_sendingIVMaterial);
    if (BD_DTLS_VERSION == 2)
    {
        // The 16-byte security key goes after the 28-byte ECC secret (copy to sharedSecret+0x1c).
        bdMemcpy(&sharedSecret[28], securityKey.ab, sizeof(securityKey.ab));
        bdUInt hashResultSize = sizeof(masterSecret);
        if (!m_hash.getHash().hash(sharedSecret, BD_DTLS_SHARED_SECRET_LENGTH, masterSecret, hashResultSize))
        {
            bdLogWarn("dtls", "Failed to hash shared secret.");
            return false;
        }
        bdCryptoUtils::zeroBuffer(sharedSecret, BD_DTLS_SHARED_SECRET_LENGTH);
        bdMemcpy(m_sendingIVMaterial, masterSecret, sizeof(masterSecret));
        bdMemcpy(m_receivingIVMaterial, masterSecret, sizeof(masterSecret));
        if (!m_sendingCypher.getCypher().init(masterSecret, cypherKeySize) ||
            !m_receivingCypher.getCypher().init(masterSecret, cypherKeySize))
        {
            bdLogWarn("dtls", "Failed to initialise cyphers.");
            bdCryptoUtils::zeroBuffer(masterSecret, sizeof(masterSecret));
            return false;
        }
    }
    else if (BD_DTLS_VERSION >= 3)
    {
        bdBool ok = true;
        // Inferred: DWARF bdDTLSAssociation.cpp:1276 declares hashSize with no location; initialiser unknown.
        const bdUInt hashSize = 32;
        bdUByte8 allZeros[hashSize] = {0};
        bdUByte8 earlySecret[hashSize];
        ok = ok && bdCryptoUtils::hkdfExtractSHA256(allZeros, securityKey.ab, sizeof(securityKey.ab), earlySecret);
        bdUByte8 handshakeSecret[hashSize];
        ok = ok && bdCryptoUtils::hkdfExtractSHA256(earlySecret, sharedSecret, 28, handshakeSecret);
        bdCryptoUtils::zeroBuffer(sharedSecret, BD_DTLS_SHARED_SECRET_LENGTH);
        ok = ok && bdCryptoUtils::hkdfExtractSHA256(handshakeSecret, allZeros, sizeof(allZeros), masterSecret);

        // 32 = BD_DTLS_RANDOM_SIZE (DWARF bdDTLSRandom.h:17), not declared in src/bdDTLSRandom.h.
        bdUInt sendOffset = 0;
        bdUByte8 sendMessages[68] = {0};
        ok = ok && bdBytePacker::appendBuffer(sendMessages, sizeof(sendMessages), sendOffset, sendOffset, &m_peerRandom, 32);
        ok = ok && bdBytePacker::appendBuffer(sendMessages, sizeof(sendMessages), sendOffset, sendOffset, &m_localRandom, 32);
        ok = ok && bdBytePacker::appendBasicType<bdUInt16>(sendMessages, sizeof(sendMessages), sendOffset, sendOffset, m_peerTag);
        ok = ok && bdBytePacker::appendBasicType<bdUInt16>(sendMessages, sizeof(sendMessages), sendOffset, sendOffset, m_localTag);
        bdUByte8 sendTrafficSecret[hashSize];
        ok = ok && bdDTLSUtils::deriveSecret(masterSecret, bdDTLSUtils::BD_DTLS_LABEL_SERVER_APPLICATION_TRAFFIC_SECRET, sendMessages, sizeof(sendMessages), sendTrafficSecret);
        bdUByte8 sendEncKey[32];
        bdUByte8 sendIV[32];
        ok = ok && bdDTLSUtils::hkdfExpandLabel(sendTrafficSecret, bdDTLSUtils::BD_DTLS_LABEL_KEY, NULL, 0, sendEncKey, cypherKeySize);
        ok = ok && bdDTLSUtils::hkdfExpandLabel(sendTrafficSecret, bdDTLSUtils::BD_DTLS_LABEL_IV, NULL, 0, sendIV, sizeof(sendIV));

        bdUInt recvOffset = 0;
        bdUByte8 recvMessages[68] = {0};
        ok = ok && bdBytePacker::appendBuffer(recvMessages, sizeof(recvMessages), recvOffset, recvOffset, &m_localRandom, 32);
        ok = ok && bdBytePacker::appendBuffer(recvMessages, sizeof(recvMessages), recvOffset, recvOffset, &m_peerRandom, 32);
        ok = ok && bdBytePacker::appendBasicType<bdUInt16>(recvMessages, sizeof(recvMessages), recvOffset, recvOffset, m_localTag);
        ok = ok && bdBytePacker::appendBasicType<bdUInt16>(recvMessages, sizeof(recvMessages), recvOffset, recvOffset, m_peerTag);
        bdUByte8 recvTrafficSecret[hashSize];
        ok = ok && bdDTLSUtils::deriveSecret(masterSecret, bdDTLSUtils::BD_DTLS_LABEL_SERVER_APPLICATION_TRAFFIC_SECRET, recvMessages, sizeof(recvMessages), recvTrafficSecret);
        bdUByte8 recvEncKey[32];
        bdUByte8 recvIV[32];
        ok = ok && bdDTLSUtils::hkdfExpandLabel(recvTrafficSecret, bdDTLSUtils::BD_DTLS_LABEL_KEY, NULL, 0, recvEncKey, cypherKeySize);
        ok = ok && bdDTLSUtils::hkdfExpandLabel(recvTrafficSecret, bdDTLSUtils::BD_DTLS_LABEL_IV, NULL, 0, recvIV, sizeof(recvIV));

        if (ok)
        {
            bdMemcpy(m_sendingIVMaterial, sendIV, sizeof(sendIV));
            bdMemcpy(m_receivingIVMaterial, recvIV, sizeof(recvIV));
        }
        ok = ok && m_sendingCypher.getCypher().init(sendEncKey, cypherKeySize);
        ok = ok && m_receivingCypher.getCypher().init(recvEncKey, cypherKeySize);

        bdCryptoUtils::zeroBuffer(earlySecret, sizeof(earlySecret));
        bdCryptoUtils::zeroBuffer(handshakeSecret, sizeof(handshakeSecret));
        bdCryptoUtils::zeroBuffer(sendMessages, sizeof(sendMessages));
        bdCryptoUtils::zeroBuffer(recvMessages, sizeof(recvMessages));
        bdCryptoUtils::zeroBuffer(sendTrafficSecret, sizeof(sendTrafficSecret));
        bdCryptoUtils::zeroBuffer(recvTrafficSecret, sizeof(recvTrafficSecret));
        bdCryptoUtils::zeroBuffer(sendEncKey, sizeof(sendEncKey));
        bdCryptoUtils::zeroBuffer(recvEncKey, sizeof(recvEncKey));
        bdCryptoUtils::zeroBuffer(sendIV, sizeof(sendIV));
        bdCryptoUtils::zeroBuffer(recvIV, sizeof(recvIV));

        if (!ok)
        {
            bdLogWarn("dtls", "Failed to derive DTLS keys.");
            bdCryptoUtils::zeroBuffer(masterSecret, sizeof(masterSecret));
            return false;
        }
    }
    else
    {
        bdLogWarn("dtls", "Unsupported DTLS version.");
        return false;
    }
    bdCryptoUtils::zeroBuffer(sharedSecret, BD_DTLS_SHARED_SECRET_LENGTH);
    bdCryptoUtils::zeroBuffer(masterSecret, sizeof(masterSecret));
    return true;
}
