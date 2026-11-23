// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Server/Network/Server.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Sigma/DataStructures/XPooledList.h"

struct PacketBuffer;
struct PacketPool;

typedef int32 TimerID;

typedef int32 GameConnectionID;

struct PacketHistoryEntry {  // size 0x18
    uint32 dwTickReceived;  // 0x00
    int32 nPlayerIndex;  // 0x04
    PacketBuffer * ptPacketBuffer;  // 0x08
    PacketPool * ptPacketPoolAllocator;  // 0x10
};
static_assert(sizeof(PacketHistoryEntry) == 0x18, "PacketHistoryEntry");

typedef XListMemoryPool<PacketHistoryEntry> TPacketHistoryListPool;

typedef XPooledList<PacketHistoryEntry, const PacketHistoryEntry &> TPacketHistoryList;

