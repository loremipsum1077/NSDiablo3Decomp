#pragma once

#include "SigmaTypes.h"
#include "Unicode.h"

struct GamePerfThreadFile;

typedef CHAR PerfTableEntryName[64];

class GamePerfFile {
public:
    GamePerfFile(const char* pPath);
    ~GamePerfFile();
    void Write(const void* pData, const size_t uSize);
    void Printf(const char* pFormat, ...);
    void Flush();
    BOOL IsOpen();

private:
    GamePerfFile(const GamePerfFile&);
    const GamePerfFile& operator=(const GamePerfFile&);

    GamePerfThreadFile* m_pFile;
};

struct PerfCtrParams {
    int32 nEnumIndex;
    BOOL fIsCounter;
    PerfTableEntryName szName;
};

struct GamePerfStats {
    BOOL fCollectPerfCounters;
    BOOL fCollectPerfCountersThisFrame;
    uint32 dwFrameNum;
    int32 nFrameSampleCount;
    float flInstantaneousFPS;
    float flCurrentLowestFPS;
    float flCurrentHighestFPS;
    float flAvgFPS;
    float flRecentLowestFPS;
    float flRecentHighestFPS;
    GamePerfFile* fileDumpFPS;
    uint32 nDumpFPSFreq;
    uint32 nDumpFPSCount;
    BOOL bFPSCollectionStart;
    BOOL bDumpFPSStart;
    BOOL bDumpFPSFormatJSON;
    float flMinAvgFPS;
};

struct GamePerfValues {
    float flAvgTime;
    float flAvgTimeWithSubs;
    float flMaxTime;
    float flMaxTimeWithSubs;
    float flAvgPct;
    float flMaxPct;
    float flCurrentTime;
    float flAvgFPS;
};

void GamePerfInitialize();
void GamePerfDispose();
GamePerfStats& GamePerfStatsGet();
void GamePerfNewFrame();
void GamePerfCollectAverages();

// Layout checks generated from the DWARF by tools/layout_asserts.py.
#include <stddef.h>
static_assert(sizeof(GamePerfFile) == 0x8, "GamePerfFile");
static_assert(sizeof(PerfCtrParams) == 0x48, "PerfCtrParams");
static_assert(offsetof(PerfCtrParams, nEnumIndex) == 0x0, "PerfCtrParams");
static_assert(offsetof(PerfCtrParams, fIsCounter) == 0x4, "PerfCtrParams");
static_assert(offsetof(PerfCtrParams, szName) == 0x8, "PerfCtrParams");
static_assert(sizeof(GamePerfStats) == 0x48, "GamePerfStats");
static_assert(offsetof(GamePerfStats, fCollectPerfCounters) == 0x0, "GamePerfStats");
static_assert(offsetof(GamePerfStats, fCollectPerfCountersThisFrame) == 0x4, "GamePerfStats");
static_assert(offsetof(GamePerfStats, dwFrameNum) == 0x8, "GamePerfStats");
static_assert(offsetof(GamePerfStats, nFrameSampleCount) == 0xc, "GamePerfStats");
static_assert(offsetof(GamePerfStats, flInstantaneousFPS) == 0x10, "GamePerfStats");
static_assert(offsetof(GamePerfStats, flCurrentLowestFPS) == 0x14, "GamePerfStats");
static_assert(offsetof(GamePerfStats, flCurrentHighestFPS) == 0x18, "GamePerfStats");
static_assert(offsetof(GamePerfStats, flAvgFPS) == 0x1c, "GamePerfStats");
static_assert(offsetof(GamePerfStats, flRecentLowestFPS) == 0x20, "GamePerfStats");
static_assert(offsetof(GamePerfStats, flRecentHighestFPS) == 0x24, "GamePerfStats");
static_assert(offsetof(GamePerfStats, fileDumpFPS) == 0x28, "GamePerfStats");
static_assert(offsetof(GamePerfStats, nDumpFPSFreq) == 0x30, "GamePerfStats");
static_assert(offsetof(GamePerfStats, nDumpFPSCount) == 0x34, "GamePerfStats");
static_assert(offsetof(GamePerfStats, bFPSCollectionStart) == 0x38, "GamePerfStats");
static_assert(offsetof(GamePerfStats, bDumpFPSStart) == 0x3c, "GamePerfStats");
static_assert(offsetof(GamePerfStats, bDumpFPSFormatJSON) == 0x40, "GamePerfStats");
static_assert(offsetof(GamePerfStats, flMinAvgFPS) == 0x44, "GamePerfStats");
static_assert(sizeof(GamePerfValues) == 0x20, "GamePerfValues");
static_assert(offsetof(GamePerfValues, flAvgTime) == 0x0, "GamePerfValues");
static_assert(offsetof(GamePerfValues, flAvgTimeWithSubs) == 0x4, "GamePerfValues");
static_assert(offsetof(GamePerfValues, flMaxTime) == 0x8, "GamePerfValues");
static_assert(offsetof(GamePerfValues, flMaxTimeWithSubs) == 0xc, "GamePerfValues");
static_assert(offsetof(GamePerfValues, flAvgPct) == 0x10, "GamePerfValues");
static_assert(offsetof(GamePerfValues, flMaxPct) == 0x14, "GamePerfValues");
static_assert(offsetof(GamePerfValues, flCurrentTime) == 0x18, "GamePerfValues");
static_assert(offsetof(GamePerfValues, flAvgFPS) == 0x1c, "GamePerfValues");
