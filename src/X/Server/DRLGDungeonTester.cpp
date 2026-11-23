#include "X/Server/DRLGDungeonTester.h"


DRLGDungeonTester::DRLGDungeonTester(SNO snoWorld) : m_snoWorld(snoWorld), m_pResults(NULL) { RandInit(m_tRNG); }
#include "DRLGDungeonInternal.h"
#include "DRLGResults.h"
#include "../Sigma/SigmaTiming.h"
#include "../Sigma/Math/MathGeneral.h"
#include <limits.h>
// DRLGDungeon.cpp declarations recovered from DWARF.
DRLGDungeonGenerator* sDRLGDungeonGeneratorAlloc();
void sDRLGDungeonGeneratorFree(DRLGDungeonGenerator*& ptDungeonGenerator);
// DRLGResults.h getters/setters: original caller loads/stores these exact members.
int32 DRLGTestResults::GetWarningCount() const
{
    return m_WarningCount;
}
DRLGTileUsage* DRLGTestResult::GetTileUsage()
{
    return &m_TileUsage;
}
inline void DRLGTestResult::SetDungeonLength(DungeonLength& tDungeonLength)  // inline: DWARF abstract DIE only, no concrete DIE (r06 test)
{
    m_tDungeonLength = tDungeonLength;
}
inline void DRLGTestResult::SetHasSwastikaConfiguration(BOOL bSwastika)  // inline: DWARF abstract DIE only, no concrete DIE (r06 test)
{
    m_bHasSwastikaConfiguration = bSwastika;
}
inline BOOL DRLGTestResult::HasSwastikaConfiguration() const  // inline: DWARF abstract DIE only, no concrete DIE (r06 test)
{
    return m_bHasSwastikaConfiguration;
}
#line 33 "X/Server/DRLGDungeonTester.cpp"
DRLGDungeonTester::~DRLGDungeonTester()
{
    delete m_pResults;
    m_pResults = 0;
}
#line 39 "X/Server/DRLGDungeonTester.cpp"
const DRLGTestResults& DRLGDungeonTester::TestDungeons(int32 nNumTests)
{
    uint32 dwSeed = SigmaGetGameClockTicks();
    delete m_pResults;
    m_pResults = 0;
    m_pResults = new DRLGTestResults(m_snoWorld);
    int32 nLastPercentDone = -1;
    for (int32 i = 0; i < nNumTests; ++i) {
        DRLGTestResult* pResult = TestDungeon(dwSeed);
        m_pResults->RecordResult(pResult);
        int32 nPercentDone = i * 100 / nNumTests;
        if (nPercentDone != nLastPercentDone) {
            CRefString sStatusString;
            sStatusString.Format("DRLG Test %d / %d", i, nNumTests);
            nLastPercentDone = nPercentDone;
        }
        dwSeed = Rand_Internal(m_tRNG, 0x7fffffe);
    }
    CRefString sStatusMsg;
    int32 nErrorCount = m_pResults->GetErrorCount();
    int32 nWarningCount = m_pResults->GetWarningCount();
    sStatusMsg.Format("Generated %d Dungeons, %d Errors, %d Warnings (%.2f%%%% error rate)\n", nNumTests, nErrorCount, nWarningCount, static_cast<float>(nErrorCount * 100) / nNumTests);
    return *m_pResults;
}
#line 199 "X/Server/DRLGDungeonTester.cpp"
int32 DRLGDungeonTester::FindShortestDistanceBetweenEntranceAndExits(TDRLGTileList& tTileList) const
{
    XGrowableList<DRLGTile*, DRLGTile*> listExits;
    XGrowableList<DRLGTile*, DRLGTile*> listEntrances;
    POS pos = tTileList.GetHeadPosition();
    while (pos) {
        DRLGTile& tTile = tTileList.GetNext(pos);
        if (tTile.UsesEntranceTileSet()) listEntrances.AddTail_Tracked(&tTile);
        else if (tTile.UsesExitTileSet()) listExits.AddTail_Tracked(&tTile);
    }
    if (listExits.IsEmpty() || listEntrances.IsEmpty()) return 0;
    int32 nMinDistance = INT_MAX;
    POS posEntrance = listEntrances.GetHeadPosition();
    while (posEntrance) {
        DRLGTile* tEntranceTile = listEntrances.GetNext(posEntrance);
        POS posExit = listExits.GetHeadPosition();
        while (posExit) {
            DRLGTile* tExitTile = listExits.GetNext(posExit);
            DRLGDungeonGenerator::ComputeRoomDistances(tTileList, tEntranceTile, FALSE);
            nMinDistance = Math::Min(nMinDistance, tExitTile->nDistance);
        }
    }
    return nMinDistance;
}
#line 294 "X/Server/DRLGDungeonTester.cpp"
DRLGTestResult* DRLGDungeonTester::TestDungeon(uint32 dwSeed)
{
    RandSetSeed_Internal(m_tRNG, dwSeed);
    DRLGDungeonGenerator* ptDungeonGen = sDRLGDungeonGeneratorAlloc();
    ptDungeonGen->Initialize(m_tRNG, m_snoWorld, -1);
    BOOL bErrors = ptDungeonGen->GenerateDungeon();
    BOOL bWarnings = ptDungeonGen->m_bWarnings;
    DRLGTestResult* pResult = new DRLGTestResult(dwSeed);
    if (bWarnings) pResult->SetWarnings(ptDungeonGen->m_sCreationLog);
    if (bErrors) pResult->SetErrors(ptDungeonGen->m_sCreationLog);
    for (uint i = 0; i < ptDungeonGen->m_DRLGEvents.size(); ++i)
        pResult->LogTestEvent(ptDungeonGen->m_DRLGEvents[i]);
    DungeonLength tDungeonLength;
    tDungeonLength.Initialize();
    tDungeonLength.nMinDistanceFromEntranceToExit = FindShortestDistanceBetweenEntranceAndExits(ptDungeonGen->m_listTiles);
    ScopedWorldDefinitionLockR ptWorldDef(m_snoWorld);
    if (ptWorldDef->ptServerData->ptDRLG) {
        POS pos = ptDungeonGen->m_listTiles.GetHeadPosition();
        while (pos) {
            const DRLGTile& tTile = ptDungeonGen->m_listTiles.GetNext(pos);
            pResult->GetTileUsage()->RecordTile(tTile);
            if (!tTile.IsBorderTile()) {
                ++tDungeonLength.nTotalRoomCount;
                if (!pResult->HasSwastikaConfiguration())
                    pResult->SetHasSwastikaConfiguration(tTile.FormsSwastikaPivotPoint());
            }
        }
        pos = ptDungeonGen->m_listPlacedScenes.GetHeadPosition();
        while (pos) {
            const DRLGPlacedScene& tPlacedScene = ptDungeonGen->m_listPlacedScenes.GetNext(pos);
            pResult->GetTileUsage()->RecordScene(tPlacedScene);
        }
    }
    pResult->SetDungeonLength(tDungeonLength);
    sDRLGDungeonGeneratorFree(ptDungeonGen);
    return pResult;
}
