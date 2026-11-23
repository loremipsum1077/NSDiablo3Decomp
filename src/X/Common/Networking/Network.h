// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Networking/Network.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Sigma/DataStructures/XPooledList.h"

enum {
    GAMECONNECTIONID_INVALID = -1,
};

enum {
    MAX_SINGLE_MESSAGE_SIZE = 32768,
};

enum {
    NETWORK_COMPRESSION_SIZE_MASK = 16777215,
    NETWORK_COMPRESSION_PACKET_COMPRESSED_BIT = 31,
    NETWORK_COMPRESSION_PACKET_SYNC_FLUSH_BIT = 30,
    NETWORK_COMPRESSION_PACKET_RESET_STREAM_BIT = 29,
};

typedef BOOL (*NetworkGameDataBlockHandlerProc)(intptr, uint8 *, uint32, BOOL &);

struct PacketBufferHeader {  // size 0x8
    uint32 dwPoolHandle;  // 0x00
    DataID idSocket;  // 0x04
};
static_assert(sizeof(PacketBufferHeader) == 0x8, "PacketBufferHeader");

struct PacketBuffer {  // size 0xc
    PacketBufferHeader tHeader;  // 0x00
    uint8 byteData[1];  // 0x08
};
static_assert(sizeof(PacketBuffer) == 0xc, "PacketBuffer");

typedef XListMemoryPool<PacketBuffer *> TPacketListPool;

typedef XPooledList<PacketBuffer *, PacketBuffer *> TPacketBufferList;

