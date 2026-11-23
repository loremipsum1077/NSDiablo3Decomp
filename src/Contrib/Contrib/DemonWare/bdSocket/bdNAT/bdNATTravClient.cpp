#include "bdNATTravClient.h"
#include "../../bdCore/bdContainers/bdQueue.h"
#include "../../bdCore/bdContainers/bdSingleton.h"
#include "../../bdCore/bdCrypto/bdHMacSHA256.h"
#include "../../bdCore/bdSocket/bdCommonAddr.h"
#include "../../bdCore/bdSocket/bdCommonAddrInfo.h"
#include "../../bdCore/bdUtilities/bdBytePacker.h"
#include "../../bdCore/bdUtilities/bdTrulyRandom.h"
#include "../../bdPlatform/bdPlatformLog/bdPlatformLog.h"
#include "../bdServiceBandwidthArbitrator.h"
#include "bdNATTravClientData.h"
#include "bdNATTravListener.h"
#include "bdPacket/bdNATTraversalPacket.h"
#include <cstring>

bdNATTravClient::CachedTraversal bdNATTravClient::m_cachedTraversals[50];
bdUInt bdNATTravClient::m_nextCachedTraversal = 0;
bdBool bdNATTravClient::m_cacheFull = false;
bdMutex bdNATTravClient::m_cacheLock;

bdNATTravClient::bdNATTravClient()
    : m_callbacks(4, 0.75f), m_status(BD_NAT_TRAV_UNINITIALIZED) {}


bdNATTravClient::~bdNATTravClient() {
  bdHashMap<unsigned int, bdNATTravClientData, bdHashingClass>::Iterator it =
      m_callbacks.getIterator();
  while (it) {
    bdNATTravClientData data(m_callbacks.getValue(it));
    bdNChar8 tmp[1024];
    const bdUInt tmpSize = 1024;
    bdCommonAddrInfo::getBriefInfo(data.m_remote, tmp, tmpSize);
    bdUseVAArgs(tmp);
    data.callOnNATAddrDiscoveryFailed(data.m_remote);
    m_callbacks.next(it);
  }
  m_callbacks.clear();
  m_nextCachedTraversal = 0;
  m_cacheFull = false;
}
bdBool bdNATTravClient::init(bdDefaultSocket *socket,
                             bdServiceBandwidthArbitrator *bandArb,
                             bdCommonAddrRef localCommonAddr) {
  bdBool result = false;
  if (m_status != BD_NAT_TRAV_UNINITIALIZED) {
    bdLogError("bdNATTrav", "Already initialized.");
  } else if (!socket) {
    bdLogError("bdNATTrav", "Socket is NULL.");
  } else if (!bandArb) {
    bdLogError("bdNATTrav", "Bandwidth arbitrator is NULL.");
  } else if (localCommonAddr.isNull()) {
    bdLogError("bdNATTrav", "Local common addr is NULL.");
  } else {
    m_socket = socket;
    m_bandArb = bandArb;
    m_localCommonAddr = localCommonAddr;
    result = sendKeepAlive();
    bdSingleton<bdTrulyRandomImpl>::getInstance()->getRandomUByte8(
        m_secretKey, sizeof(m_secretKey));
    m_status = BD_NAT_TRAV_INITIALIZED;
    resetUsageStats();
    m_natTravTimeout = 0.9f;
    m_interleaveStages = true;
  }
  return result;
}
void bdNATTravClient::pump() {
  if (m_status != BD_NAT_TRAV_UNINITIALIZED) {
    if (m_keepAliveTimer.getElapsedTimeInSeconds() > 15.0f) {
      sendKeepAlive();
    }

    bdQueue<bdUInt32> removeQueue;
    bdHashMap<unsigned int, bdNATTravClientData, bdHashingClass>::Iterator it =
        m_callbacks.getIterator();
    while (it) {
      const bdUInt32 &identifier = m_callbacks.getKey(it);
      bdNATTravClientData &data = m_callbacks.getValue(it);
      if (data.m_throttled && data.m_age.getElapsedTimeInSeconds() > 20.0f) {
        const bdUInt bufferSize = 100;
        bdNChar8 buffer[bufferSize];
        data.m_remote->getPublicAddr().toString(buffer, bufferSize);
        bdLogWarn("bdNATTrav",
                  "Traversal to %s throttled for %f seconds, giving up.",
                  buffer, data.m_age.getElapsedTimeInSeconds());
        data.callOnNATAddrDiscoveryFailed(data.m_remote);
        removeQueue.enqueue(identifier);
      } else if (data.m_lastSent.getElapsedTimeInSeconds() > m_natTravTimeout) {
        bdLogInfo("bdNATTrav", "Resending after %f seconds.",
                  data.m_lastSent.getElapsedTimeInSeconds());
        switch (data.m_state) {
        case bdNATTravClientData::BD_NTCDS_STAGE_1:
          if (data.m_tries >= 4) {
            if (data.m_remote->getPublicAddr().getAddress().isValid() &&
                !m_introducers.isEmpty()) {
              bdLogInfo("bdNATTrav", "Stage 1 failed, trying stage 2.");
              bdNChar8 commonAddrInfo[1024];
              bdCommonAddrInfo::getBriefInfo(data.m_remote, commonAddrInfo,
                                             sizeof(commonAddrInfo));
              bdLogInfo("bdNATTrav", "Common address info follows.");
              data.m_tries = 0;
              data.m_state = bdNATTravClientData::BD_NTCDS_STAGE_2;
              sendStage2(data);
            } else {
              bdLogWarn("bdNATTrav", "Stage 1 failed, no stage 2 possible.");
              bdNChar8 commonAddrInfo[1024];
              bdCommonAddrInfo::getBriefInfo(data.m_remote, commonAddrInfo,
                                             sizeof(commonAddrInfo));
              bdLogWarn("bdNATTrav", "Common address info follows.");
              data.callOnNATAddrDiscoveryFailed(data.m_remote);
              removeQueue.enqueue(identifier);
            }
          } else {
            bdLogInfo("bdNATTrav", "Resending stage 1.");
            bdNChar8 commonAddrInfo[1024];
            bdCommonAddrInfo::getBriefInfo(data.m_remote, commonAddrInfo,
                                           sizeof(commonAddrInfo));
            bdLogInfo("bdNATTrav", "Common address info follows.");
            sendStage1(data);
          }
          break;
        case bdNATTravClientData::BD_NTCDS_STAGE_2:
          if (data.m_tries++ >= 5) {
            bdLogInfo("bdNATTrav", "Stage 2 failed, trying stage 3.");
            bdNChar8 commonAddrInfo[1024];
            bdCommonAddrInfo::getBriefInfo(data.m_remote, commonAddrInfo,
                                           sizeof(commonAddrInfo));
            bdLogInfo("bdNATTrav", "Common address info follows.");
            data.m_tries = 0;
            data.m_state = bdNATTravClientData::BD_NTCDS_STAGE_3;
            sendStage1(data);
          } else {
            bdLogInfo("bdNATTrav", "Resending stage 2.");
            bdNChar8 commonAddrInfo[1024];
            bdCommonAddrInfo::getBriefInfo(data.m_remote, commonAddrInfo,
                                           sizeof(commonAddrInfo));
            bdLogInfo("bdNATTrav", "Common address info follows.");
            sendStage2(data);
          }
          break;
        case bdNATTravClientData::BD_NTCDS_STAGE_3:
          if (data.m_tries >= 4) {
            bdLogWarn("bdNATTrav", "Stage 3 failed.");
            bdNChar8 commonAddrInfo[1024];
            bdCommonAddrInfo::getBriefInfo(data.m_remote, commonAddrInfo,
                                           sizeof(commonAddrInfo));
            bdLogWarn("bdNATTrav", "Common address info follows.");
            bdNATType NATType = data.m_remote->getNATType();
            if (NATType >= BD_NAT_UNKNOWN && NATType < BD_NAT_COUNT) {
              ++m_traversalFailureCount[NATType];
            }
            data.callOnNATAddrDiscoveryFailed(data.m_remote);
            removeQueue.enqueue(identifier);
          } else {
            bdLogInfo("bdNATTrav", "Resending stage 3.");
            bdNChar8 commonAddrInfo[1024];
            bdCommonAddrInfo::getBriefInfo(data.m_remote, commonAddrInfo,
                                           sizeof(commonAddrInfo));
            bdLogInfo("bdNATTrav", "Common address info follows.");
            sendStage1(data);
          }
          break;
        default:
          bdLogError("bdNATTrav", "Unknown state.");
          break;
        }
      }
      m_callbacks.next(it);
    }

    while (removeQueue.getSize() > 0) {
      m_callbacks.remove(removeQueue.peek());
      removeQueue.dequeue();
    }
  }

  if (m_callbacks.getSize() == 0) {
    m_status = BD_NAT_TRAV_INITIALIZED;
  }
}
bdBool bdNATTravClient::quit() {
  bdBool result = false;
  switch (m_status) {
  case BD_NAT_TRAV_INITIALIZED:
    m_socket = NULL;
    m_bandArb = NULL;
    m_status = BD_NAT_TRAV_UNINITIALIZED;
    m_introducers.clear();
    result = true;
    break;
  case BD_NAT_TRAV_RUNNING: {
    bdHashMap<unsigned int, bdNATTravClientData, bdHashingClass>::Iterator it =
        m_callbacks.getIterator();
    while (it) {
      bdNATTravClientData data(m_callbacks.getValue(it));
      bdNChar8 tmp[1024];
      bdCommonAddrInfo::getBriefInfo(data.m_remote, tmp, sizeof(tmp));
      bdLogInfo("bdNATTrav", "Cancelling pending traversal to %s", tmp);
      m_callbacks.next(it);
    }
    m_introducers.clear();
    m_socket = NULL;
    m_bandArb = NULL;
    m_status = BD_NAT_TRAV_UNINITIALIZED;
    result = true;
    break;
  }
  default:
    break;
  }
  return result;
}
bdBool bdNATTravClient::setupIntroducers(const bdArray<bdAddr> &introducers) {
  bdBool result = false;
  if (m_status != BD_NAT_TRAV_UNINITIALIZED) {
    if (m_introducers.getSize()) {
      bdLogWarn("bdNATTrav", "Introducers already set, overwriting.");
    }
    m_introducers = introducers;
    result = true;
  }
  return result;
}
bdBool bdNATTravClient::updateLocalCommonAddr(
    const bdCommonAddrRef newLocalCommonAddr) {
  bdBool result = false;
  if (m_status != BD_NAT_TRAV_UNINITIALIZED && !newLocalCommonAddr.isNull()) {
    bdHashMap<unsigned int, bdNATTravClientData, bdHashingClass>::Iterator it =
        m_callbacks.getIterator();
    while (it) {
      bdNATTravClientData data(m_callbacks.getValue(it));
      data.m_local = newLocalCommonAddr;
      m_callbacks.next(it);
    }
    m_localCommonAddr = newLocalCommonAddr;
    result = true;
  } else {
    bdLogWarn("bdNATTrav", "Cannot update local common addr.");
  }
  return result;
}
bdBool
bdNATTravClient::isConnectInProgress(const bdCommonAddrRef remote) const {
  const bdUInt32 identifier = remote->getHash();
  return m_callbacks.containsKey(identifier);
}

bdBool bdNATTravClient::connect(bdCommonAddrRef remote,
                                bdNATTravListener *listener, bdBool throttle) {
  bdBool result = false;
  const bdAddr *cached_addr = getFromCache(remote->getHash());
  if (cached_addr) {
    bdUseVAArgs();
    listener->onNATAddrDiscovery(remote, *cached_addr);
    return true;
  }
  if (m_status == BD_NAT_TRAV_UNINITIALIZED) {
    bdUseVAArgs();
    return false;
  }
  const bdUInt32 identifier = remote->getHash();
  if (!m_callbacks.containsKey(identifier)) {
    bdNATTravClientData data(m_localCommonAddr, remote, listener);
    data.m_throttled = throttle;
    data.m_age.start();
    bdNChar8 destAddr[1024];
    bdCommonAddrInfo::getInfo(remote, destAddr, sizeof(destAddr));
    bdUseVAArgs(destAddr);
    bdBool sent;
    if (m_interleaveStages && remote->getNATType() == BD_NAT_MODERATE &&
        !m_introducers.isEmpty()) {
      data.m_state = bdNATTravClientData::BD_NTCDS_STAGE_2;
      bdUseVAArgs();
      sent = sendStage2(data);
    } else
      sent = sendStage1(data);
    if (!sent)
      bdUseVAArgs();
    if (!m_callbacks.put(identifier, data)) {
      bdUseVAArgs();
      bdNChar8 commonAddrInfo[1024];
      bdCommonAddrInfo::getBriefInfo(remote, commonAddrInfo,
                                     sizeof(commonAddrInfo));
      bdUseVAArgs();
    } else {
      result = true;
      m_status = BD_NAT_TRAV_RUNNING;
    }
  } else {
    bdHashMap<unsigned int, bdNATTravClientData, bdHashingClass>::Iterator it =
        m_callbacks.getIterator(identifier);
    bdNATTravClientData *data = &m_callbacks.getValue(it);
    if (data->m_secondaryListener) {
      bdNChar8 commonAddrInfo[1024];
      bdCommonAddrInfo::getBriefInfo(remote, commonAddrInfo,
                                     sizeof(commonAddrInfo));
      bdUseVAArgs(commonAddrInfo);
      return false;
    } else {
      data->m_secondaryListener = listener;
      result = true;
    }
  }
  return result;
}
void bdNATTravClient::cancelConnect(bdCommonAddrRef remote) {
  if (!remote.isNull()) {
    const bdUInt32 identifier = remote->getHash();
    bdNATTravClientData callback;
    if (m_callbacks.remove(identifier, callback))
      callback.callOnNATAddrDiscoveryFailed(remote);
    else {
      bdNChar8 commonAddrInfo[1024];
      bdCommonAddrInfo::getBriefInfo(remote, commonAddrInfo,
                                     sizeof(commonAddrInfo));
      bdUseVAArgs(commonAddrInfo);
    }
  } else
    bdUseVAArgs();
}
// NON_MATCHING: register allocation differs (original keeps result live in w21 from
// entry and data/size/packet in x22-x24; ours reuses the size register).
#line 580
bdBool bdNATTravClient::acceptPacket(bdDefaultSocket *, bdAddr addr, void *data, const bdUInt size, const bdUByte8 type)
{
  bdBool result = false;


  if (type >= 10 && type < 20 && m_status != BD_NAT_TRAV_UNINITIALIZED) {



    bdNATTraversalPacket packet;
    bdUInt newOffset;

    if (packet.deserialize(data, size, 0, newOffset) &&
        packet.getType() >= 10 &&
        packet.getType() < 20) {

      receiveFrom(addr, packet);
      result = true;
    }

  }


  return result;
}
#line 338
bdBool bdNATTravClient::sendKeepAlive() {
  bdBool result = true;
  m_keepAliveTimer.start();
  if (!m_introducers.isEmpty()) {
    bdNATTraversalPacket packet(static_cast<bdUByte8>(14), 0, bdAddr(),
                                bdAddr());
    bdUInt newOffset;
    const bdUInt bufferSize = 1288;
    bdUByte8 buffer[bufferSize];
    if (!packet.serialize(buffer, bufferSize, 0, newOffset)) {
      bdUseVAArgs();
      return false;
    }
    for (bdUInt i = 0; i < m_introducers.getSize(); ++i)
      if (m_socket->sendTo(m_introducers[i], buffer, newOffset) !=
          newOffset) {
        bdUseVAArgs();
        result = false;
      }
  }
  return result;
}
bdBool bdNATTravClient::sendStage1(bdNATTravClientData &data) {
  data.m_lastSent.start();
  if (data.m_throttled && !m_bandArb->allowedSend(28))
    return false;
  ++data.m_tries;
  bdCommonAddrRef local = data.m_local, remote = data.m_remote;
  bdBool sent = true;
  const bdUInt32 identifier = remote->getHash();
  bdAddr src(local->getLocalAddrByIndex(0));
  bdArray<bdAddr> destinations(remote->getLocalAddrs());
  if (remote->getPublicAddr().getAddress().isValid())
    destinations.pushBack(remote->getPublicAddr());
  for (bdUInt i = 0; i < destinations.getSize(); ++i) {
    bdAddr dst(destinations[i]);
    bdNATTraversalPacket packet(static_cast<bdUByte8>(13), identifier, src,
                                dst);
    const bdUInt hmacLength = 10;
    bdUByte8 hmacTemp[hmacLength];
    if (doHMac(identifier, packet.getAddrSrc(), packet.getAddrDest(),
               hmacTemp)) {
      packet.setHMAC(hmacTemp, hmacLength);
      const bdUInt bufferSize = 1288;
      bdUByte8 buffer[bufferSize];
      bdUInt serializedSize = 0;
      packet.serialize(buffer, bufferSize, 0, serializedSize);
      if (data.m_throttled)
        m_bandArb->send(packet.getSize() + 28);
      if (m_socket->sendTo(dst, buffer, serializedSize) == serializedSize) {
        const bdUWord strSize = 22;
        bdNChar8 dstStr[strSize];
        dst.toString(dstStr, strSize);
        bdUseVAArgs(dstStr);
      } else {
        bdUseVAArgs();
        sent = false;
      }
    } else
      bdUseVAArgs();
  }
  return sent;
}
bdBool bdNATTravClient::sendStage2(bdNATTravClientData &data) {
  if (m_interleaveStages && data.m_remote->getNATType() == BD_NAT_MODERATE) {
    sendStage1(data);
    --data.m_tries;
  }
  if (data.m_throttled && !m_bandArb->allowedSend(28))
    return false;
  data.m_lastSent.start();
  ++data.m_tries;
  bdCommonAddrRef local = data.m_local, remote = data.m_remote;
  bdBool sent = false;
  const bdUInt32 identifier = remote->getHash();
  bdAddr src(local->getPublicAddr()), dst(remote->getPublicAddr());
  bdNATTraversalPacket packet(static_cast<bdUByte8>(10), identifier, src, dst);
  const bdUInt hmacLength = 10;
  bdUByte8 hmacTemp[hmacLength];
  const bdUInt bufferSize = 1288;
  bdUByte8 buffer[bufferSize];
  if (doHMac(identifier, packet.getAddrSrc(), packet.getAddrDest(), hmacTemp)) {
    packet.setHMAC(hmacTemp, hmacLength);
    bdUInt serializedSize = 0;
    packet.serialize(buffer, bufferSize, 0, serializedSize);
    if (data.m_throttled)
      m_bandArb->send(packet.getSize() + 28);
    if (!m_introducers.isEmpty() &&
        m_socket->sendTo(m_introducers[0], buffer, serializedSize) ==
            serializedSize) {
      const bdUWord strSize = 22;
      bdNChar8 intr[strSize], dstString[strSize], srcString[strSize];
      m_introducers[0].toString(intr, strSize);
      packet.getAddrDest().toString(dstString, strSize);
      packet.getAddrSrc().toString(srcString, strSize);
      bdUseVAArgs(intr, dstString, srcString);
      sent = true;
    } else
      bdUseVAArgs();
  }
  return sent;
}
bdBool bdNATTravClient::doHMac(const bdUInt32 identifier, const bdAddr &src,
                               const bdAddr &dest, bdUByte8 *data) const {
  bdUInt offset = 0;
  bdUByte8 addrBuffer[136];
  const bdInt identSize = 4;
  bdUByte8 ident[identSize];
  if (!bdBytePacker::appendBasicType(ident, identSize, 0, offset, identifier))
    return false;
  bdHMacSHA256 hmac(m_secretKey, 28);
  bdBool result = hmac.process(ident, identSize);
  if (result) {
    offset = 0;
    result = src.serialize(addrBuffer, sizeof(addrBuffer), 0, offset) &&
             hmac.process(addrBuffer, offset);
  }
  if (result) {
    offset = 0;
    result = dest.serialize(addrBuffer, sizeof(addrBuffer), 0, offset) &&
             hmac.process(addrBuffer, offset);
  }
  if (result) {
    bdUInt hmacLength = 10;
    result = hmac.getData(data, hmacLength);
  }
  return result;
}

inline const bdAddr *bdNATTravClient::getFromCache(bdUInt remoteAddressHash) {
  m_cacheLock.lock();
  bdUInt end = m_cacheFull ? 50 : m_nextCachedTraversal;
  const bdAddr *addr = 0;
  for (bdUInt i = 0; i < end && !addr; ++i)
    if (m_cachedTraversals[i].m_remoteAddressHash == remoteAddressHash)
      addr = &m_cachedTraversals[i].m_realAddress;
  m_cacheLock.unlock();
  return addr;
}
inline void bdNATTravClient::saveInCache(bdUInt remoteAddressHash,
                                         const bdAddr &realAddress) {
  m_cacheLock.lock();
  m_cachedTraversals[m_nextCachedTraversal].m_remoteAddressHash =
      remoteAddressHash;
  m_cachedTraversals[m_nextCachedTraversal].m_realAddress = realAddress;
  m_nextCachedTraversal = (m_nextCachedTraversal + 1) % 50;
  if (!m_nextCachedTraversal && !m_cacheFull)
    m_cacheFull = true;
  m_cacheLock.unlock();
}
inline void bdNATTravClient::resetUsageStats() {
  bdMemset(m_traversalCount, 0, sizeof(m_traversalCount) * 4);
}

void bdNATTravClient::receiveFrom(const bdAddr &addr,
                                  bdNATTraversalPacket &packet) {
  const bdUInt bufferSize = 1288;
  bdUByte8 buffer[bufferSize];
  bdUInt length;
  switch (packet.getType()) {
  case 11: {
    bdAddr dest(packet.getAddrSrc());
    packet.setType(12);
    bdBool serialized = packet.serialize(buffer, bufferSize, 0, length);
    if (serialized) {
      const bdInt sendResult = m_socket->sendTo(dest, buffer, length);
      const bdUWord dstSize = 22;
      bdNChar8 dst[dstSize];
      dest.toString(dst, dstSize);
      if (sendResult > 0)
        bdUseVAArgs(dst);
      else
        bdUseVAArgs(dst, sendResult);
    } else
      bdUseVAArgs();
    break;
  }
  case 12: {
    bdUseVAArgs();
    const bdUInt32 identifier = packet.getIdentifier();
    bdUInt hmacLength = 10;
    bdUByte8 hmacBuffer[10];
    bdBool hmacRes = doHMac(identifier, packet.getAddrSrc(),
                            packet.getAddrDest(), hmacBuffer);
    if (hmacRes && memcmp(hmacBuffer, packet.getHMAC(), hmacLength) == 0) {
      const bdUWord strSize = 22;
      bdNChar8 src[strSize], lookup[strSize];
      addr.toString(src, strSize);
      packet.getAddrDest().toString(lookup, strSize);
      bdUseVAArgs(lookup, src);
      bdNATTravClientData data;
      if (m_callbacks.remove(identifier, data)) {
        bdUInt64 durationMSecs = static_cast<bdUInt64>(
            data.m_age.getElapsedTimeInSeconds() * 1000.0f);
        bdNATType NATType = data.m_remote->getNATType();
        if (NATType >= BD_NAT_UNKNOWN && NATType < BD_NAT_COUNT) {
          ++m_traversalCount[NATType];
          m_traversalDurationAggr[NATType] += durationMSecs;
          m_traversalDurationAggrSq[NATType] += durationMSecs * durationMSecs;
        }
        bdUseVAArgs(durationMSecs, NATType);
        data.callOnNATAddrDiscovery(data.m_remote, addr);
        saveInCache(data.m_remote->getHash(), addr);
      } else
        bdUseVAArgs();
    } else
      bdUseVAArgs();
    break;
  }
  case 13: {
    if (packet.getIdentifier() != m_localCommonAddr->getHash()) {
      const bdUWord srcSize = 22;
      bdNChar8 src[srcSize];
      addr.toString(src, srcSize);
      bdUseVAArgs(src);
      break;
    }
    bdAddr dest(addr);
    packet.setType(12);
    bdBool serialized = packet.serialize(buffer, bufferSize, 0, length);
    if (serialized) {
      const bdInt sendResult = m_socket->sendTo(dest, buffer, length);
      const bdUWord dstSize = 22;
      bdNChar8 dst[dstSize];
      dest.toString(dst, dstSize);
      if (sendResult > 0)
        bdUseVAArgs(dst);
      else
        bdUseVAArgs(dst, sendResult);
    } else
      bdUseVAArgs();
    break;
  }
  case 10:
    bdUseVAArgs();
    break;
  default:
    break;
  }
}
