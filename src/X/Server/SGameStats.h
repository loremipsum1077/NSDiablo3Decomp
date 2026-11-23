// Generated from the DWARF by tools/gen_headers.py -- reference only.
// Original file: X/Server/SGameStats.h

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Time.h"
#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "SGame.h"
#include "SGameStatsSwig.h"
#include "../Sigma/DataStructures/XPooledMap.h"
#include "../Sigma/SigmaTypes.h"

typedef int32 StatCounterType;
struct Player;
void SGameStatsPlayerCounterIncrement(Player * ptPlayer, StatCounterType eType, int32 nParam0, int32 nParam1, float flValue);  // SGameStats.cpp:1626

float SGameStatsPlayerCounterFetch(const Player * ptPlayer, StatCounterType eType, int32 nParam0, int32 nParam1);  // SGameStats.cpp:1690
void SGameStatsDumpPlayerToLog(ACDID idACDPlayer);  // SGameStats.cpp:1170
void SGameStatsRecordPlayerStats(const Player & tPlayer);  // SGameStats.cpp:1110
void SGameStatsClearForPlayer(Player & tPlayer);  // SGameStats.cpp:1736
struct ActorCommonData;
void SGameStatsRecordGoldSpent(ActorCommonData & tACDPlayer, uint64 uGoldSpent);  // SGameStats.cpp:833

typedef int32 PayloadDeliveryMechanism;

typedef int32 LootContext;

struct StatCounterKey {  // size 0xc
    StatCounterType eType;  // 0x00
    int32 nParam0;  // 0x04
    int32 nParam1;  // 0x08
    StatCounterKey();
    StatCounterKey(const StatCounterType, const int32, const int32);
    BOOL operator==(const StatCounterKey & tKey) const;
};
static_assert(sizeof(StatCounterKey) == 0xc, "StatCounterKey");

struct StatTimedEventData {  // size 0xc
    uint32 dwStart;  // 0x00
    uint32 dwRunningTotal;  // 0x04
    BOOL bReportIfUnfinished;  // 0x08
};
static_assert(sizeof(StatTimedEventData) == 0xc, "StatTimedEventData");

typedef XPooledMap<StatCounterKey, StatCounterKey, StatTimedEventData, StatTimedEventData, 16> StatTimedEventMap;

typedef XMapMemoryPool<StatCounterKey, StatTimedEventData> StatTimedEventMapPool;

typedef XPooledMap<StatCounterKey, StatCounterKey, float, float, 1024> StatCounterMapFloat;

typedef XMapMemoryPool<StatCounterKey, float> StatCounterMapPoolFloat;

typedef int32 PowerID;

struct SGameStatGlobals {  // size 0x12830
    UpdateData tStatsUpdateData;  // 0x00
    BOOL bInitialized;  // 0x20
    StatTimedEventMapPool poolStatTimedEventMap;  // 0x28
    StatCounterMapPoolFloat poolStatCounterMap;  // 0x58
    StatTimedEventMap mapStatTimedEvents;  // 0x88
    StatCounterMapFloat mapStatCounters;  // 0x130
    StatTimedEventMap mapPlayerStatTimedEvents[4];  // 0x2158
    StatCounterMapFloat mapPlayerStatCounters[4];  // 0x23f8
    StatTimedEventMap mapPlayerCurrentLifeStatTimedEvents[4];  // 0xa498
    StatCounterMapFloat mapPlayerCurrentLifeStatCounters[4];  // 0xa738
    BOOL bCheatUsed;  // 0x127d8
    uint32 uMaxNumberOfPlayers;  // 0x127dc
    uint32 uTotalGoldDropped;  // 0x127e0
    uint32 uMaxGoldDroppedAtOnce;  // 0x127e4
    Blizzard::Time::Timestamp tStartTime;  // 0x127e8
    Blizzard::Time::Timestamp tEndTime;  // 0x127f0
    uint32 uTotalNumberOfPlayersByClass[7];  // 0x127f8
    float flTotalPlayTimeByClass[7];  // 0x12814
};
static_assert(sizeof(SGameStatGlobals) == 0x12830, "SGameStatGlobals");


// Functions defined in this file (bodies are not in the DWARF):
//   line    61  BOOL StatCounterKey::operator==(const StatCounterKey & tKey) const  [inlined into 14 functions]
