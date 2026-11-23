// Generated from the DWARF by tools/gen_headers.py -- reference only.
// Original file: X/Server/DRLGDungeonTester.h

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "../Common/SNO/SNOFiles.h"
#include "DRLGDungeon.h"
#include "../Sigma/DataStructures/XPooledList.h"
#include "../Sigma/Random.h"

class DRLGTestResult;
class DRLGTestResults;

typedef int32 DRLGTileType;

typedef XPooledList<DRLGTile, const DRLGTile &> TDRLGTileList;

class DRLGDungeonTester {  // size 0x18
public:
    DRLGDungeonTester(SNO);
    ~DRLGDungeonTester();
    const DRLGTestResults & TestDungeons(int32 nNumTests);
    const DRLGTestResults & TestDungeonSequence(int32, int32);
    int32 FindShortestDistanceBetweenEntranceAndExits(TDRLGTileList & tTileList) const;
protected:
    DRLGTestResult * TestDungeon(uint32 dwSeed);
private:
    RNG m_tRNG;  // 0x00
    SNO m_snoWorld;  // 0x08
    DRLGTestResults * m_pResults;  // 0x10
};
static_assert(sizeof(DRLGDungeonTester) == 0x18, "DRLGDungeonTester");

