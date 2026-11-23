// Generated from the DWARF by tools/gen_headers.py -- reference only.
// Original file: Contrib/Contrib/DemonWare/bdConnection/bdConnectionStatistics.h

#pragma once

#include "../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include "../bdCore/bdMemory/bdMemory-std.h"
#include <stddef.h>
#include <stdint.h>

class bdConnectionStatistics {  // size 0x40
public:
    BD_CLASS_ALLOCATOR(bdConnectionStatistics);
    bdConnectionStatistics();
    void reset();
    void addBytesSent(const bdUInt bytes);
    void addBytesRecv(const bdUInt bytes);
    void addPacketSizeSent(const bdUInt bytes);
    void addPacketSizeRecv(const bdUInt bytes);
    void addPacketsSent(const bdUInt numPackets);
    void addPacketsRecv(const bdUInt numPackets);
    void setLastRTT(const bdFloat32 time);
    bdUInt getBytesSent() const;
    bdUInt getBytesSentPerSecond() const;
    bdUInt getBytesRecv() const;
    bdUInt getPacketsSent() const;
    bdUInt getPacketsRecv() const;
    bdFloat32 getAvgRTT() const;
    void update(const bdFloat32);
protected:
    bdUInt m_bytesSent;  // 0x00
    bdUInt m_bytesSentPerSecond;  // 0x04
    bdUInt m_avgBytesSent;  // 0x08
    bdUInt m_lastBytesSent;  // 0x0c
    bdUInt m_bytesRecv;  // 0x10
    bdUInt m_avgPacketSentSize;  // 0x14
    bdUInt m_avgPacketRecvSize;  // 0x18
    bdUInt m_maxPacketSizeSent;  // 0x1c
    bdUInt m_minPacketSizeSent;  // 0x20
    bdUInt m_maxPacketSizeRecv;  // 0x24
    bdUInt m_minPacketSizeRecv;  // 0x28
    bdUInt m_packetsSent;  // 0x2c
    bdUInt m_packetsRecv;  // 0x30
    bdFloat32 m_maxRTT;  // 0x34
    bdFloat32 m_minRTT;  // 0x38
    bdFloat32 m_avgRTT;  // 0x3c
};
static_assert(sizeof(bdConnectionStatistics) == 0x40, "bdConnectionStatistics");

