// Adopted from DWARF; hand-maintained.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdMatchMaking/bdMatchMakingInfo.h

#pragma once

#include "../../bdCore/bdSocket/bdEndpoint.h"
#include "../../bdCore/bdSocket/bdSecurityID.h"
#include "../../bdCore/bdSocket/bdThreadedStreamSocket.h"
#include "../bdLobbyCommon/bdSerializable.h"
#include "../bdLobbyCommon/bdTaskResult.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdByteBuffer;

class bdMatchMakingInfo : public bdTaskResult, public bdSerializable {  // size 0x138
public:
    static void * operator new(size_t);
    static void operator delete(void * p) { bdMemory::deallocate(p); }
    static void * operator new[](size_t);
    static void operator delete[](void *);
    static void * operator new(size_t, void *);
    static void operator delete(void *, void *);
    static void * operator new[](size_t, void *);
    static void operator delete[](void *, void *);
    bdMatchMakingInfo();
    bdMatchMakingInfo(const bdCommonAddrRef, const bdInt32, const bdUInt32);
    bdMatchMakingInfo(const bdUByte8 *, const bdUInt32, const bdInt32, const bdUInt32);
    virtual ~bdMatchMakingInfo();  // vtable slot 0
    void setHostAddr(bdCommonAddrRef);
    bdCommonAddrRef getHostAddrAsCommonAddr(bdCommonAddrRef localCommonAddr) const;
    virtual bdBool serialize(bdByteBuffer & buffer) const;  // vtable slot 4
    virtual bdBool deserialize(bdByteBufferRef buffer);  // vtable slot 2
    virtual bdUInt sizeOf();  // vtable slot 3
    bdSecurityID m_sessionID;  // 0x10
    bdUByte8 m_hostAddr[255];  // 0x18
    bdUInt m_hostAddrSize;  // 0x118
    bdUInt m_gameType;  // 0x11c
    bdUInt m_maxPlayers;  // 0x120
    bdUInt m_numPlayers;  // 0x124
};
static_assert(sizeof(bdMatchMakingInfo) == 0x138, "bdMatchMakingInfo");


// Functions defined in this file (bodies are not in the DWARF):
//   line    38  bdMatchMakingInfo::bdMatchMakingInfo(const bdMatchMakingInfo &)  [inlined into 2 functions]
//   line    42  void bdMatchMakingInfo::operator delete(void * p)  [inlined into 2 functions]
