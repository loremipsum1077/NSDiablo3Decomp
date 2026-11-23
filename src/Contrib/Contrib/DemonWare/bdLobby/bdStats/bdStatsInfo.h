// Adopted from the DWARF; virtual order checked against original vtables.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdStats/bdStatsInfo.h

#pragma once

#include "../../bdCore/bdSocket/bdThreadedStreamSocket.h"
#include "../bdLobbyCommon/bdLobbyServicesCommon.h"
#include "../bdLobbyCommon/bdSerializable.h"
#include "../bdLobbyCommon/bdTaskResult.h"
#include "bdStats.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdByteBuffer;

class bdStatsInfo : public bdTaskResult, public bdSerializable {  // size 0x90
public:
    BD_CLASS_ALLOCATOR(bdStatsInfo);
    bdStatsInfo();
    virtual ~bdStatsInfo();  // vtable slot 0
    virtual bdBool deserialize(bdByteBufferRef buffer);  // vtable slot 2
    virtual bdUInt sizeOf();  // vtable slot 3
    virtual bdBool serialize(bdByteBuffer & buffer) const;  // vtable slot 4
    bdLeaderBoardID m_leaderboardID;  // 0x10
    bdEntityID m_entityID;  // 0x18
    bdStats::bdWriteType m_writeType;  // 0x20
    bdInt64 m_rating;  // 0x28
    bdUInt64 m_rank;  // 0x30
    bdNChar8 m_entityName[65];  // 0x38
    bdUInt32 m_secondsSinceUpdate;  // 0x7c
};
static_assert(sizeof(bdStatsInfo) == 0x90, "bdStatsInfo");

class bdUserGeneratedStatsInfo : public bdStatsInfo {  // size 0x98
public:
    BD_CLASS_ALLOCATOR(bdUserGeneratedStatsInfo);
    bdUserGeneratedStatsInfo();
    virtual ~bdUserGeneratedStatsInfo();  // vtable slot 0
    virtual bdBool deserialize(bdByteBufferRef);  // vtable slot 2
    virtual bdUInt sizeOf();  // vtable slot 3
    virtual bdBool serialize(bdByteBuffer &) const;  // vtable slot 4
    bdEntityID m_subLeaderboardID;  // 0x80
};
static_assert(sizeof(bdUserGeneratedStatsInfo) == 0x98, "bdUserGeneratedStatsInfo");

class bdStatsInfoEx : public bdTaskResult {  // size 0x30
public:
    BD_CLASS_ALLOCATOR(bdStatsInfoEx);
    bdStatsInfoEx();
    bdStatsInfoEx(bdStatsInfo *);
    virtual ~bdStatsInfoEx();  // vtable slot 0
    virtual bdBool deserialize(bdByteBufferRef buffer);  // vtable slot 2
    virtual bdUInt sizeOf();  // vtable slot 3
    bdLeaderBoardID m_leaderboardID;  // 0x08
    bdUInt32 m_error;  // 0x0c
    bdEntityID m_entityID;  // 0x10
    bdStatsInfo * m_statsInfo;  // 0x18
};
static_assert(sizeof(bdStatsInfoEx) == 0x30, "bdStatsInfoEx");


// Functions defined in this file (bodies are not in the DWARF):
//   line    19  bdStatsInfo::bdStatsInfo(const bdStatsInfo &)  [inlined into 4 functions]
//   line    19  bdStatsInfo & bdStatsInfo::operator=(const bdStatsInfo &)  [inlined into 3 functions]
//   line    23  void bdStatsInfo::operator delete(void * p)  [inlined into 2 functions]
//   line    72  void bdUserGeneratedStatsInfo::operator delete(void * p)  [inline]
//   line    94  bdStatsInfoEx::bdStatsInfoEx(const bdStatsInfoEx &)  [inlined into 1 functions]
//   line    98  void bdStatsInfoEx::operator delete(void * p)  [inlined into 1 functions]
