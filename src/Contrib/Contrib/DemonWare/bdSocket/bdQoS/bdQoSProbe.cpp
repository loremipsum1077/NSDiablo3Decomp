#include "bdQoSProbe.h"
#include "bdQoSProbeListener.h"
#include "bdPacket/bdQoSRequestPacket.h"
#include "../bdServiceBandwidthArbitrator.h"
#include "../../bdCore/bdContainers/bdQueue.h"
#include "../../bdCore/bdSocket/bdCommonAddrInfo.h"
#include "../../bdCore/bdUtilities/bdBytePacker.h"
#include "../../bdPlatform/bdPlatformLog/bdPlatformLog.h"
#include "../bdNAT/bdNATTravClient.h"

bdQoSProbe::bdQoSProbe() : m_status(BD_QOS_PROBE_UNINITIALIZED), m_probesResolving(4, 0.75f), m_probesProbing(4, 0.75f), m_probesProcessing(4, 0.75f) {}
bdQoSProbe::~bdQoSProbe() { m_socket = 0; m_natTrav = 0; m_bandArb = 0; }
bdBool bdQoSProbe::init(bdDefaultSocket * socket, bdNATTravClient * natTrav, bdServiceBandwidthArbitrator * bandArb)
{
    bdBool result = false;
    if (m_status != BD_QOS_PROBE_UNINITIALIZED) { bdUseVAArgs(); return false; }
    if (socket && natTrav && bandArb) {
        m_maxBandwidth = 16384; m_socket = socket; m_natTrav = natTrav; m_bandArb = bandArb; m_nextProbeId = 0;
        m_secids.clear(); m_statsPerSession.clear(); m_listenState = BD_QOS_OFF;
        m_useMultiplePacketsPerProbe = false; m_maxRetries = 4;
        m_probeTimeout = 0.9f; m_hostSideTimeout = 0.45f; m_maxConcurrentProbes = 8;
        m_probingTimer.start(); m_bandArb->reset();
        m_bandArb->addSliceQuota((bdUInt)((m_maxBandwidth >> 3) * 0.2f));
        m_status = BD_QOS_PROBE_INITIALIZED; result = true;
    } else bdUseVAArgs();
    return result;
}
bdBool bdQoSProbe::quit()
{
    bdBool result = false;
    if (m_status != BD_QOS_PROBE_UNINITIALIZED) {
        cancelProbes(); m_status = BD_QOS_PROBE_UNINITIALIZED;
        m_socket = 0; m_natTrav = 0; m_bandArb = 0; result = true;
    }
    return result;
}
void bdQoSProbe::onNATAddrDiscovery(bdCommonAddrRef remote, const bdAddr & realAddr)
{
    if (m_probesResolving.containsKey(remote)) {
        bdArray<bdQoSProbeEntryWrapper> entries;
        if (m_probesResolving.get(remote, entries)) {
            for (bdUInt i = 0; i < entries.getSize(); ++i) {
                entries[i].m_realAddr = realAddr;
                m_probesProbing.put(m_nextProbeId++, entries[i]);
            }
        }
        m_probesResolving.remove(remote);
    } else bdUseVAArgs();
}
void bdQoSProbe::onNATAddrDiscoveryFailed(bdCommonAddrRef remote)
{
    if (m_probesResolving.containsKey(remote)) {
        bdNChar8 addrStr[1024];
        bdCommonAddrInfo::getBriefInfo(remote, addrStr, 1024); bdUseVAArgs(addrStr);
        bdArray<bdQoSProbeEntryWrapper> entries;
        if (m_probesResolving.get(remote, entries)) {
            for (bdUInt i = 0; i < entries.getSize(); ++i)
                entries[i].m_listener->onQoSProbeFail(entries[i].m_addr);
        }
        m_probesResolving.remove(remote);
    } else bdUseVAArgs();
}
#line 234
// Laid on its DWARF lines 235-281 (tab-indented as the original columns show); status test first per columns 239:35/45.
bdBool bdQoSProbe::acceptPacket(bdDefaultSocket *, bdAddr addr, void * data, const bdUInt size, const bdUByte8 type)
{
	bdBool result = false;

	if ((BD_QOS_PROBE_INITIALIZED == m_status) && // columns: m_status 239:35, && 239:45
		(type >= 40 && type < 50)) // type 240:4; literals: the original names (column 29) are unknown
	{

		const bdUWord srcSize = 22;
		bdNChar8 src[srcSize];
		addr.toString(src, srcSize);

		bdUInt32 typeAsInt = type;
		bdUseVAArgs(src, typeAsInt);
		switch (typeAsInt)
		{
			case 40:
			{
				bdQoSRequestPacket packet;
				bdUInt deserializedSize;
				if (packet.deserialize(data, size, 0, deserializedSize)
					&& deserializedSize == size)
				{
					result = handleRequest(packet, addr, size);
				}
			}
			break;
			case 41:
			{
				bdQoSReplyPacket packet;
				bdUInt deserializedSize;
				if (packet.deserialize(data, size, 0, deserializedSize)
					&& deserializedSize == size)
				{
					result = handleReply(packet, addr, size);
				}
			}
			break;
			default:
			{
				bdUseVAArgs();
				break;
			}
		}
	}
	return result;
}
#line 88
// Rows 494 (status test) and 499 (containsKey) are separate: nested ifs, so the containsKey block dominates both lookups and the remove and &m_probesProbing is computed once (x22 at the first lookup, as in the original).
bdBool bdQoSProbe::handleReply(bdQoSReplyPacket & packet, bdAddr & addr, bdUInt packetSize)
{
    bdBool result = false;
    bdUInt32 numPacketsExpected = m_useMultiplePacketsPerProbe ? 2 : 1;
    if (BD_QOS_PROBE_UNINITIALIZED != m_status) { if (m_probesProbing.containsKey(packet.getId())) {
        auto it = m_probesProbing.getIterator(packet.getId());
        bdQoSProbeEntryWrapper & entry = m_probesProbing.getValue(it);
        if (entry.m_realAddr == addr) {
            ++entry.m_currentProbe.m_numPacketsReceived;
            bdFloat32 latency = packet.getElapsedTime();
            if (entry.m_minLatency == 2147483647.0f || entry.m_minLatency > latency) entry.m_minLatency = latency;
            entry.m_latency += latency;
            entry.m_replies[entry.m_numRepliesReceived] = latency;
            ++entry.m_numRepliesReceived;
            if (entry.m_currentProbe.m_numPacketsReceived >= numPacketsExpected) {
                bdQoSProbeEntryWrapper entryCopy(entry);
                m_probesProbing.remove(it);
                singleProbeComplete(packet, entryCopy, packetSize);
            } else entry.m_currentProbe.m_timer.start();
            result = true;
        } else bdUseVAArgs();
    } else bdUseVAArgs(); } else bdUseVAArgs();
    return result;
}
void bdQoSProbe::cancelProbes()
{
    if (m_status == BD_QOS_PROBE_INITIALIZED) {
        bdArray<bdCommonAddrRef> probesToCancel;
        auto resit = m_probesResolving.getIterator();
        while (resit) { probesToCancel.pushBack(m_probesResolving.getKey(resit)); m_probesResolving.next(resit); }
        for (unsigned int i = 0; i < probesToCancel.getSize(); ++i) m_natTrav->cancelConnect(probesToCancel[i]);
    } else bdUseVAArgs();
    m_probesProbing.clear();
    m_probesResolving.clear();
}
inline bdBool bdQoSProbe::findHostStats(const bdUInt32 & secID, bdUInt32 & index)
{
    bdBool found = false;
    for (index = 0; index < m_statsPerSession.getSize(); ++index) {
        if (m_statsPerSession[index].m_secID == secID) { found = true; break; }
    }


    return found;
}
inline void bdQoSProbe::logProbeRequests(bdUInt32 secID, bdBool requestingData)
{
    bdUInt32 i;
    if (findHostStats(secID, i)) {
        if (requestingData) ++m_statsPerSession[i].m_numDataRequestsReceived;
        else ++m_statsPerSession[i].m_numProbesReceived;
    } else bdUseVAArgs(secID);
}
inline void bdQoSProbe::logProbeReplys(bdUInt32 secID, bdBool sendingData)
{
    bdUInt32 i;
    if (findHostStats(secID, i)) {
        if (sendingData) ++m_statsPerSession[i].m_numDataRepliesSent;
        else ++m_statsPerSession[i].m_numProbeRepliesSent;
        if (sendingData) m_statsPerSession[i].m_numDataReplyBytesSent += m_replyData.getDataSize();
    } else bdUseVAArgs(secID);
}
inline void bdQoSProbe::logProbeDiscards(bdUInt32 secID)
{
    bdUInt32 i;
    if (findHostStats(secID, i)) ++m_statsPerSession[i].m_numSlotsFullDiscards;
    else bdUseVAArgs();
}
inline bdUInt bdQoSProbe::shrinkSecId(const bdSecurityID & id) const
{
    bdUInt tmp = 0, result;
    bdBool status = bdBytePacker::removeBasicType(&id, sizeof(id), 0, tmp, result);
    return status ? result : 0;
}
inline void bdQoSProbe::getPacketSendSize(bdUInt serializedSize, bdUInt & numPackets, bdUInt & totalSize)
{
    numPackets = m_useMultiplePacketsPerProbe ? 2 : 1;
    totalSize = numPackets * (serializedSize + 28);
}
inline bdUInt32 bdQoSProbe::calculateBandwidth(bdFloat32 timeDiffBetweenPackets, bdUInt32 packetSize)
{
    bdUInt32 bandwidth;
    if (timeDiffBetweenPackets == 0 || !m_useMultiplePacketsPerProbe) bandwidth = 0xffffffff;
    else bandwidth = (bdUInt32)((packetSize * 8 + 224) / timeDiffBetweenPackets);
    return bandwidth;
}
void bdQoSProbe::sendRequest(bdBool & stopProcessing, bdQoSProbeEntryWrapper & entry, bdUInt32 id)
{
    bdUByte8 buffer[1288]; bdUInt packetSize = 0; const bdUInt bufferSize = sizeof(buffer);
    bdUInt packetsToSend, totalDataSize; const bdUInt32 secID = shrinkSecId(entry.m_id);
    bdQoSRequestPacket packet(secID, id);
    if (entry.m_numRepliesExpected && entry.m_numRepliesExpected - 1 == entry.m_numRepliesReceived)
        packet.setRequestingData(true);
    const bdBool serializeOk = packet.serialize(buffer, bufferSize, packetSize, packetSize);
    getPacketSendSize(packetSize, packetsToSend, totalDataSize);
    if (serializeOk) {
        if (m_bandArb->allowedSend(totalDataSize)) {
            bdNChar8 addrStr[22]; entry.m_realAddr.toString(addrStr, sizeof(addrStr)); bdUseVAArgs(addrStr);
            for (bdUInt i = 0; i < packetsToSend; ++i) {
                bdInt sendResult = m_socket->sendTo(entry.m_realAddr, buffer, packetSize);
                if (sendResult < 0) { bdUseVAArgs(); break; }
            }
            ++entry.m_retries; entry.m_lastTry.start(); m_bandArb->send(totalDataSize);
        } else { stopProcessing = true; bdUseVAArgs(); }
    } else { bdUseVAArgs(); stopProcessing = true; }
}

bdBool bdQoSProbe::handleRequest(bdQoSRequestPacket & packet, bdAddr & addr, bdInt32 packetSize)
{
    bdUInt dummy;
    bdBool result = processRequestPackets(packet, addr, packetSize);
    if (m_listenState == BD_QOS_OFF) result = true;
    else if (m_status == BD_QOS_PROBE_UNINITIALIZED) { bdUseVAArgs(); result = false; }
    else {
        if (m_bandArb->allowedSend(28) && m_secids.findFirst(packet.getSecId(), dummy)) {
            if (m_bandArb->allowedSend(m_replyData.getSerializedSize() + 28) && !m_useMultiplePacketsPerProbe) {
                bdQosProbeHost probe;
                probe.m_id = packet.getId(); probe.m_secid = packet.getSecId();
                probe.m_probeTimestamp = packet.getTimestamp(); probe.m_needToSendData = packet.getRequestingData();
                result = sendReply(addr, probe);
                logProbeRequests(packet.getSecId(), packet.getRequestingData());
                return result;
            }
        }
        if (!m_secids.findFirst(packet.getSecId(), dummy)) bdUseVAArgs();
    }
    return result;
}

bdBool bdQoSProbe::sendReply(const bdAddr & addr, const bdQosProbeHost & probe)
{
    bdUByte8 bufferMsg1[1288], bufferMsg2[1288];
    bdUInt totalDataSize = 0; bdBool result = false; bdUInt dataSizeMsg1;
    const bdUInt bufferSize = 1288; bdUInt dataSizeMsg2;
    m_replyData.setId(probe.m_id); m_replyData.setTimestamp(probe.m_probeTimestamp);
    m_replyData.setBandwidth(probe.m_bandwidth);
    m_replyData.setHostProcessingTime(probe.m_holdTimePacket1.getElapsedTimeInSeconds());
    m_replyData.setHasData(probe.m_needToSendData);
    dataSizeMsg1 = dataSizeMsg2 = 0;
    bdBool serializeResult = m_replyData.serialize(bufferMsg1, bufferSize, 0, dataSizeMsg1);
    totalDataSize = dataSizeMsg1;
    if (m_useMultiplePacketsPerProbe && serializeResult) {
        m_replyData.setHostProcessingTime(probe.m_holdTimePacket2.getElapsedTimeInSeconds());
        serializeResult = m_replyData.serialize(bufferMsg2, bufferSize, 0, dataSizeMsg2);
        totalDataSize += dataSizeMsg2;
    }
    if (serializeResult) {
        if (!m_bandArb->allowedSend(totalDataSize)) return false;
        logProbeReplys(probe.m_secid, probe.m_needToSendData);
        bdInt socketSendResult = m_socket->sendTo(addr, bufferMsg1, dataSizeMsg1);
        if (m_useMultiplePacketsPerProbe && socketSendResult > 0) {
            logProbeReplys(probe.m_secid, probe.m_needToSendData);
            socketSendResult = m_socket->sendTo(addr, bufferMsg2, dataSizeMsg2);
        }
        if (socketSendResult > 0) { m_bandArb->send(totalDataSize); result = true; }
        else bdUseVAArgs();
    } else bdUseVAArgs();
    return result;
}

bdBool bdQoSProbe::processRequestPackets(const bdQoSRequestPacket & packet, const bdAddr & addr, bdInt32 packetSize)
{
    bdBool result = false; bdUInt32 dummy;
    if (m_listenState != BD_QOS_OFF && m_status != BD_QOS_PROBE_UNINITIALIZED && m_useMultiplePacketsPerProbe && m_secids.findFirst(packet.getSecId(), dummy)) {
        auto iter = m_probesProcessing.getIterator(addr.getHash());
        if (iter) {
            bdQosProbeHost * probe = &m_probesProcessing.getValue(iter);
            if (!probe->m_completed) {
                probe->m_holdTimePacket2.start(); logProbeRequests(packet.getSecId(), packet.getRequestingData());
                probe->m_bandwidth = calculateBandwidth(probe->m_timer.getElapsedTimeInSeconds(), packetSize);
                probe->m_completed = true; ++probe->m_numPacketsReceived;
                probe->m_needToSendData = packet.getRequestingData(); result = true;
            }
        } else if (m_probesProcessing.getSize() < (bdInt)m_maxConcurrentProbes) {
            bdQosProbeHost probe;
            probe.m_holdTimePacket1.start(); logProbeRequests(packet.getSecId(), packet.getRequestingData());
            probe.m_realAddr = addr; probe.m_secid = packet.getSecId(); ++probe.m_numPacketsReceived;
            probe.m_timer.start(); probe.m_id = packet.getId(); probe.m_probeTimestamp = packet.getTimestamp();
            m_probesProcessing.put(addr.getHash(), probe); result = true;
        } else {
            bdNChar8 addrStr[22]; logProbeDiscards(packet.getSecId()); addr.toString(addrStr, sizeof(addrStr));
            bdUseVAArgs(addrStr, m_maxConcurrentProbes);
        }
    }
    return result;
}

void bdQoSProbe::singleProbeComplete(bdQoSReplyPacket & packet, bdQoSProbeEntryWrapper & entry, bdUInt packetSize)
{
    entry.m_bandwidthDown += calculateBandwidth(entry.m_currentProbe.m_timer.getElapsedTimeInSeconds(), packetSize);
    entry.m_bandwidthUp += packet.getBandwidth();
    if (entry.m_numRepliesReceived < entry.m_numRepliesExpected) {
        entry.m_retries = 0; entry.m_currentProbe.reset(); entry.m_lastTry.reset();
        m_probesProbing.put(m_nextProbeId++, entry);
    } else {
        bdUInt32 numPacketsExpectedPerProbe = m_useMultiplePacketsPerProbe ? 2 : 1;
        bdUInt32 numProbesReceived = entry.m_numRepliesReceived / numPacketsExpectedPerProbe;
        bdQoSProbeInfo info;
        info.m_realAddr = entry.m_realAddr; info.m_addr = entry.m_addr;
        info.m_data = packet.getData(); info.m_dataSize = packet.getDataSize();
        info.m_disabled = !packet.getEnabled(); info.m_minLatency = entry.m_minLatency;
        info.m_latency = entry.m_latency / entry.m_numRepliesReceived;
        bdMemcpy(info.m_replies, entry.m_replies, sizeof(info.m_replies));
        info.m_numReplies = entry.m_numRepliesReceived;
        if (m_useMultiplePacketsPerProbe) {
            info.m_bandwidthUp = entry.m_bandwidthUp / numProbesReceived;
            info.m_bandwidthDown = entry.m_bandwidthDown / numProbesReceived;
        } else { info.m_bandwidthUp = 0xffffffff; info.m_bandwidthDown = 0xffffffff; }
        entry.m_listener->onQoSProbeSuccess(info);
    }
}
// The retry-limit test is written as the original has it (line table: fail path 310/312/314 before sendRequest at 318). The queue/stopLoop/iterator order follows the line table (296/297/299).
void bdQoSProbe::pump()
{
    if (m_status == BD_QOS_PROBE_INITIALIZED) {
        if (m_probingTimer.getElapsedTimeInSeconds() > 0.2f) {
            m_bandArb->addSliceQuota((bdUInt)((m_maxBandwidth >> 3) * 0.2f)); m_probingTimer.start();
        }
        if (m_probesProbing.getSize() > 0 && m_bandArb->allowedSend(28)) {
            bdQueue<unsigned int> toRemove; bdBool stopLoop = false;
            bdHashMap<unsigned int, bdQoSProbeEntryWrapper, bdHashingClass>::Iterator it = m_probesProbing.getIterator();
            while (it && m_bandArb->allowedSend(28) && !stopLoop) {
                bdUInt32 id = m_probesProbing.getKey(it);
                bdQoSProbeEntryWrapper & entry = m_probesProbing.getValue(it);
                if (entry.m_retries == 0 || entry.m_lastTry.getElapsedTimeInSeconds() > m_probeTimeout) {
                    if (entry.m_retries >= m_maxRetries) {
                        bdNChar8 addrStr[1024]; bdCommonAddrInfo::getBriefInfo(entry.m_addr, addrStr, sizeof(addrStr));
                        bdUseVAArgs(addrStr); toRemove.enqueue(id);
                    } else sendRequest(stopLoop, entry, id);

                }
                m_probesProbing.next(it);
            }
            while (!toRemove.isEmpty()) {
                bdQoSProbeEntryWrapper entry; m_probesProbing.get(toRemove.peek(), entry);
                entry.m_listener->onQoSProbeFail(entry.m_addr);
                m_probesProbing.remove(toRemove.peek()); toRemove.dequeue();
            }
        }
        checkHostProbes();
    } else bdUseVAArgs();
}
void bdQoSProbe::checkHostProbes()
{
    auto iter = m_probesProcessing.getIterator(); bdQueue<unsigned int> removeStaleProbesFromHost;
    while (iter) {
        bdQosProbeHost probe(m_probesProcessing.getValue(iter));
        // Original FCMP/LE gate accepts unordered elapsed times; preserve that condition.
        if (!(probe.m_timer.getElapsedTimeInSeconds() > m_hostSideTimeout) && probe.m_completed) {
            if (m_bandArb->allowedSend(28) && m_bandArb->allowedSend(m_replyData.getSerializedSize() + 28) && m_useMultiplePacketsPerProbe && probe.m_completed) {
                sendReply(probe.m_realAddr, probe); removeStaleProbesFromHost.enqueue(m_probesProcessing.getKey(iter));
            }
        } else {
            bdNChar8 addrStr[22]; probe.m_realAddr.toString(addrStr, sizeof(addrStr)); bdUseVAArgs(addrStr);
            removeStaleProbesFromHost.enqueue(m_probesProcessing.getKey(iter));
        }
        m_probesProcessing.next(iter);
    }
    while (!removeStaleProbesFromHost.isEmpty()) {
        m_probesProcessing.remove(removeStaleProbesFromHost.peek()); removeStaleProbesFromHost.dequeue();
    }
}
