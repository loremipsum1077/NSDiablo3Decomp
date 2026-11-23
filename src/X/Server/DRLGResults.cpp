#include "DRLGResults.h"
#include "DRLGDungeon.h"

// Signature and definition: original X/Common/DRLG.cpp:919.
// The exact original declaration header is not recovered; keep this narrow prototype
// under the missing-header convention in docs/decomp_workflow.md.
TPlacedScenePool* DRLGGetPlacedScenePool();

DRLGTestResult::DRLGTestResult(uint32 dwSeed)
    : m_TileUsage(this), m_dwSeed(dwSeed), m_bWarnings(FALSE), m_bErrors(FALSE),
      m_bHasSwastikaConfiguration(FALSE) {
    m_tDungeonLength.Initialize();
}

DRLGTileUsage::DRLGTileUsage(DRLGTestResult* pTestResult)
    : m_LogicalTileTally(16, NULL), m_PhysicalTileTally(16, NULL),
      m_PlacedScenes(DRLGGetPlacedScenePool()), m_bTileCountDirty(FALSE),
      m_nTileCount(0), m_pTestResult(pTestResult) {}

void DRLGTileUsage::RecordTile(const DRLGTile& tTile) {
    if (tTile.IsBorderTile() || tTile.nTileType == DRLG_TILE_NONE)
        return;
    int32 nUsageCount = 0;
    m_LogicalTileTally.Lookup(tTile.nTileType, nUsageCount);
    m_LogicalTileTally.SetAt_Tracked(tTile.nTileType, nUsageCount + 1);
    m_bTileCountDirty = TRUE;
}

void DRLGTileUsage::RecordScene(const DRLGPlacedScene& tPlacedScene) {
    SNO snoScene = tPlacedScene.snoname.snoHandle;
    if (tPlacedScene.nTileType != DRLG_TILE_NONE && snoScene != -1) {
        int32 nUsageCount = 0;
        m_PhysicalTileTally.Lookup(snoScene, nUsageCount);
        m_PhysicalTileTally.SetAt_Tracked(snoScene, nUsageCount + 1);
        m_PlacedScenes.AddTail_Tracked(tPlacedScene);
        m_bTileCountDirty = TRUE;
    }
}

void DRLGTestResult::SetWarnings(const CRefString& sCreationLog) {
    m_bWarnings = TRUE;
    m_sErrorLog = sCreationLog;
}

void DRLGTestResult::SetErrors(const CRefString& sCreationLog) {
    m_bErrors = TRUE;
    m_sErrorLog = sCreationLog;
}

void DRLGTestResult::LogTestEvent(const DRLGTestEvent& tTestEvent) {
    m_arTestEvents.push_back(tTestEvent);
}

DRLGTestResults::DRLGTestResults(SNO snoWorld)
    : m_ErrorCount(0), m_WarningCount(0), m_snoWorld(snoWorld) {}

DRLGTestResults::~DRLGTestResults() {
    POS pos = m_ResultsList.GetHeadPosition();
    while (pos) {
        DRLGTestResult* pResult = m_ResultsList.GetNext(pos);
        delete pResult;
    }
    m_ResultsList.RemoveAll();
}

void DRLGTestResults::RecordResult(DRLGTestResult* pResult) {
    if (pResult->HasErrors())
        ++m_ErrorCount;
    if (pResult->HasWarnings())
        ++m_WarningCount;
    m_ResultsList.AddTail_Tracked(pResult);
}

DRLGTestEvent::DRLGTestEvent(uint32 dwSeed, DRLGTestEvent::Status eStatus,
                           CRefString sFailureReason, DRLGCommandType eCommandType,
                           DRLGErrorResolution eResolution, DRLGTileType eTileType,
                           int32 nCommandID, DRLGStage eDRLGStage)
    : m_dwSeed(dwSeed), m_eCommandType(eCommandType), m_eStatus(eStatus),
      m_eResolution(eResolution), m_eTileType(eTileType), m_nCommandID(nCommandID),
      m_sFailureReason(sFailureReason), m_eDRLGStage(eDRLGStage) {}
