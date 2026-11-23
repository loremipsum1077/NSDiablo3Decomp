// Reconstructed from original DWARF layouts and DRLGResults.cpp caller evidence.
// Original file: X/Server/DRLGResults.h

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Contrib/Contrib/blz/include/blz/vector.h"
#include "../Common/DRLGTypes.h"
#include "../Common/Definitions/WorldDefinition.h"
#include "../Common/SNO/SNOFiles.h"
#include "../Sigma/DataStructures/XGrowableList.h"
#include "../Sigma/DataStructures/XGrowableMap.h"
#include "../Sigma/DataStructures/XTemplate.h"
#include "../Sigma/RefString.h"
#include "../Sigma/SigmaTypes.h"
#include "../Sigma/Unicode.h"

struct DRLGTile;
class DRLGTestResult;

typedef int32 DRLGCommandType;

typedef int32 DRLGTileType;

typedef int32 DRLGErrorResolution;

typedef int32 DRLGStage;

class DRLGTileUsage {  // size 0x138
    typedef XGrowableMap<int, const int &, int, int> LogicalTileTally;
    typedef XGrowableMap<int, int, int, int> PhysicalTileTally;
public:
    DRLGTileUsage(DRLGTestResult *);
    int32 GetSceneUsageCount(SNO snoScene) const;
    int32 GetTileTypeUsageCount(DRLGTileType eTileType);
    int32 GetTileCount() const;
    void RecordTile(const DRLGTile & tTile);
    void RecordScene(const DRLGPlacedScene & tPlacedScene);
    void GetSceneList(SNOList &) const;
    const TPlacedSceneList & GetPlacedSceneList() const;
private:
    DRLGTileUsage::LogicalTileTally m_LogicalTileTally;  // 0x00
    DRLGTileUsage::PhysicalTileTally m_PhysicalTileTally;  // 0x80
    TPlacedSceneList m_PlacedScenes;  // 0x100
    BOOL m_bTileCountDirty;  // 0x128
    int32 m_nTileCount;  // 0x12c
    DRLGTestResult * m_pTestResult;  // 0x130
};
static_assert(sizeof(DRLGTileUsage) == 0x138, "DRLGTileUsage");

struct DRLGTestEvent {  // size 0x38
    enum Status {
        STATUS_NONE = 0,
        STATUS_SUCCEESS = 1,
        STATUS_WARNING = 2,
        STATUS_ERROR = 3,
    };
    DRLGTestEvent();
    DRLGTestEvent(uint32, DRLGTestEvent::Status, CRefString, DRLGCommandType, DRLGErrorResolution, DRLGTileType, int32, DRLGStage);
    uint32 m_dwSeed;  // 0x00
    DRLGCommandType m_eCommandType;  // 0x04
    DRLGTestEvent::Status m_eStatus;  // 0x08
    DRLGErrorResolution m_eResolution;  // 0x0c
    DRLGTileType m_eTileType;  // 0x10
    int32 m_nCommandID;  // 0x14
    CRefString m_sFailureReason;  // 0x18
    DRLGStage m_eDRLGStage;  // 0x30
};
static_assert(sizeof(DRLGTestEvent) == 0x38, "DRLGTestEvent");

struct DungeonLength {  // size 0x8
    int32 nTotalRoomCount;  // 0x00
    int32 nMinDistanceFromEntranceToExit;  // 0x04
    int32 GetMinExplorationRequired() const;
    void Initialize() { nTotalRoomCount = 0; nMinDistanceFromEntranceToExit = 0; }
};
static_assert(sizeof(DungeonLength) == 0x8, "DungeonLength");

class DungeonLengthCollection {  // size 0x118
    typedef blz::vector<DungeonLength, blz::allocator<DungeonLength> > DungeonLengths;
    typedef XGrowableMap<int, int, int, int> DungeonLengthMap;
public:
    void AddDungeonLength(const DungeonLength &);
    int32 GetNumberDungeonsByMinEntranceToExitDistance(int32 nMinDistanceFromEntranceToExit) const;
    int32 GetNumberDungeonsByMinExplorationPercentage(int32 nMinExplorationPercentage) const;
    uint32 Count() const;
    const DungeonLength & GetDungeonLength(int32) const;
    void GetPossibleMinEntranceToExitDistances(XGrowableList<int, int> &) const;
    void GetPossibleMinExplorationPercentages(XGrowableList<int, int> &) const;
private:
    static int32 SortDistances(const void * pLHS, const void * pRHS);
    DungeonLengthCollection::DungeonLengths m_arDungeonLengths;  // 0x00
    DungeonLengthCollection::DungeonLengthMap m_mapMinEntranceToExitDistances;  // 0x18
    DungeonLengthCollection::DungeonLengthMap m_mapMinExplorationPercentages;  // 0x98
};
static_assert(sizeof(DungeonLengthCollection) == 0x118, "DungeonLengthCollection");

class DRLGTestResult {  // size 0x188
public:
    DRLGTestResult(uint32);
    BOOL HasWarnings() const { return m_bWarnings; }
    BOOL HasErrors() const { return m_bErrors; }
    const CRefString & GetErrorLog() const;
    uint32 GetSeed() const;
    void SetWarnings(const CRefString & sCreationLog);
    void SetErrors(const CRefString & sCreationLog);
    DRLGTileUsage * GetTileUsage();
    uint32 GetNumTestEvents() const;
    const DRLGTestEvent & GetTestEvent(int32) const;
    void LogTestEvent(const DRLGTestEvent & tTestEvent);
    void SetDungeonLength(DungeonLength & tDungeonLength);
    const DungeonLength & GetDungeonLength() const;
    void SetHasSwastikaConfiguration(BOOL bSwastika);
    BOOL HasSwastikaConfiguration() const;
private:
    DRLGTileUsage m_TileUsage;  // 0x00
    blz::vector<DRLGTestEvent, blz::allocator<DRLGTestEvent> > m_arTestEvents;  // 0x138
    uint32 m_dwSeed;  // 0x150
    BOOL m_bWarnings;  // 0x154
    BOOL m_bErrors;  // 0x158
    DungeonLength m_tDungeonLength;  // 0x15c
    CRefString m_sErrorLog;  // 0x168
    BOOL m_bHasSwastikaConfiguration;  // 0x180
};
static_assert(sizeof(DRLGTestResult) == 0x188, "DRLGTestResult");

class DRLGTestResults {  // size 0x68
    typedef XGrowableList<DRLGTestResult *, DRLGTestResult *> ResultsList;
public:
    DRLGTestResults(SNO);
    ~DRLGTestResults();
    SNO GetSNOWorld() const;
    void RecordResult(DRLGTestResult * pResult);
    void WriteScenesToFile(LPCSTR) const;
    POS GetHeadPosition() const;
    const DRLGTestResult & GetNext(POS &) const;
    int32 GetResultCount() const;
    int32 GetWarningCount() const;
    int32 GetErrorCount() const { return m_ErrorCount; }  // DWARF DRLGResults.h:254 (inlined into GameTesting.cpp sTestDRLGForWorld)
    int32 GetTotalSceneUsageCount(SNO) const;
    int32 GetTotalTileTypeUsageCount(DRLGTileType) const;
    int32 GetTotalNumTilesCreated() const;
    int32 GetNumberDungeonsSceneAppearedIn(SNO) const;
    int32 GetNumberDungeonsTileTypeAppearedIn(DRLGTileType) const;
    void GetDungeonLengths(DungeonLengthCollection &) const;
    void GetSceneList(SNOList &) const;
    void GetPlacedSceneList(TPlacedSceneList &) const;
    float GetSwastikaPercentageChange() const;
    int32 GetExampleSwastikaSeed() const;
    const DRLGTestResult & GetTestResult(int32) const;
    DRLGTileType GetSceneTileType(SNO) const;
    void WriteWarningsAndErrorsToFile(LPCSTR) const;
    void GetCandidateScenes(XGrowableMap<int, int, int, int> &) const;
private:
    DRLGTestResults::ResultsList m_ResultsList;  // 0x00
    int32 m_ErrorCount;  // 0x58
    int32 m_WarningCount;  // 0x5c
    SNO m_snoWorld;  // 0x60
};
static_assert(sizeof(DRLGTestResults) == 0x68, "DRLGTestResults");


