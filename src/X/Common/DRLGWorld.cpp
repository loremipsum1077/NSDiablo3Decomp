#include "DRLG.h"
#include "DRLGEncounters.h"
#include "Definitions/SceneDefinition.h"
#include "Game.h"
#include "GameCommonData.h"
#include "World.h"
#include "../Client/Prefetch.h"
#include "../Sigma/Math/MathGeneral.h"
#include "../../BlizzardCore/Include/Blizzard/Thread.h"

// Declarations recovered from their original DWARF owners.
BOOL CmdLineTestFlag(uint32);
namespace Blizzard {
namespace Thread {
bool IsMainThread();
}
namespace Process {
void Sleep(uint32);
}
}
TDRLGSceneGroupPool * DRLGGetSceneGroupPool();
BOOL LabelsApplyRuleSet(DRLGGenerator &, BOOL);
void MarkerFillPotentialSceneLabels(DRLGPlacedScene &);
void DRLGPlaceTiles(SWorldID, TPlacedSceneList &);
void DRLGCopySpawnGroupExactInstancesToWorld(SWorldID, const TPooledSpawnGroupExactInstanceList &);
BOOL DRLGPlaceSpawnGroupExactInstances(SWorldID, TPlacedSceneList &, TPooledSpawnGroupExactInstanceList &, RNG &);
void DRLGPlaceGizmoLocMarkers(SWorldID, TPlacedSceneList &, RNG &);
void DRLGGetWorldSize(SNO, int32 &, int32 &, int32 &, int32 &);
void DRLGGetWorldOrigin(SNO, IVector2D &);
void DRLGGetSceneOrigin(const SceneChunk &, IVector2D &);
void DRLGGetSceneSize(SNO, const SceneCachedValues &, IVector2D &);
extern const CHAR cgszBadDataName[];  // Definitions.cpp:116

// DRLGWorld.cpp:40, DWARF size 0x170.
class DRLGWorldGenerator : public DRLGGenerator {
public:
    int32 m_nFeetPerTileX;
    int32 m_nFeetPerTileY;
    int32 m_nFeetPerTileZ;
    int32 m_nWorldSizeInFeetX;
    int32 m_nWorldSizeInFeetY;
    int32 m_nTileXSize;
    int32 m_nTileYSize;
    int32 m_nDRLGIndex;
    void GenerateWorldScenes();
protected:
    virtual void InitSpecific();
};
static_assert(sizeof(DRLGWorldGenerator) == 0x170, "DRLGWorldGenerator");

void DRLGWorldGenerator::InitSpecific()
{
    DRLGGetWorldSize(m_snoWorld, m_nFeetPerTileX, m_nFeetPerTileY, m_nWorldSizeInFeetX, m_nWorldSizeInFeetY);
    m_nFeetPerTileZ = 1;
    m_nTileXSize = 1;
    m_nTileYSize = 1;
    m_nDRLGIndex = 0;
}

static void sPrefetchMarkerSets(SceneParams & tScene)
{
    // CMDLINE_USE_ASSETSERVER = 43, original CmdLine.h enumeration.
    if (!CmdLineTestFlag(43))
        return;
    BOOL bNeedsWait = FALSE;
    for (int32 i = 0; i < tScene.nSceneChunkCount; ++i) {
        SceneChunk tSceneChunk = tScene.ptSceneChunks[i];
        if (tSceneChunk.snoname.snoGroup == SCENE_SNO) {
            ScopedSceneDefinitionLockR pdefScene(tSceneChunk.snoname.snoHandle);
            int32 nCount = pdefScene->arMarkerSets.GetArraySize();
            for (int32 j = 0; j < nCount; ++j) {
                SNO snoMarkerSet = pdefScene->arMarkerSets[j];
                if (snoMarkerSet != SNO_NONE)
                    bNeedsWait |= !Prefetch::AddToQueue(SNOName(MARKERSET_SNO, snoMarkerSet), 2);
            }
        }
    }
    BOOL bSleptLastTime;
    if (bNeedsWait) {
        do {
            bSleptLastTime = FALSE;
            for (int32 i = 0; i < tScene.nSceneChunkCount; ++i) {
                SceneChunk tSceneChunk = tScene.ptSceneChunks[i];
                if (tSceneChunk.snoname.snoGroup == SCENE_SNO) {
                    ScopedSceneDefinitionLockR pdefScene(tSceneChunk.snoname.snoHandle);
                    int32 nCount = pdefScene->arMarkerSets.GetArraySize();
                    for (int32 j = 0; j < nCount; ++j) {
                        SNO snoMarkerSet = pdefScene->arMarkerSets[j];
                        if (snoMarkerSet != SNO_NONE && Prefetch::IsPending(SNOName(MARKERSET_SNO, snoMarkerSet))) {
                            if (Blizzard::Thread::IsMainThread())
                                Prefetch::Update(FALSE);
                            Blizzard::Process::Sleep(10);
                            bSleptLastTime = TRUE;
                        }
                    }
                }
            }
        } while (bSleptLastTime);
    }
}

void DRLGWorldGenerator::GenerateWorldScenes()
{
    ScopedWorldDefinitionLockR pdefWorld(m_snoWorld);
    TDRLGSceneGroupList listSceneGroups(DRLGGetSceneGroupPool());
    IVector2D pointWorldOrigin;
    DRLGGetWorldOrigin(m_snoWorld, pointWorldOrigin);
    int32 nFeetPerGridCellX = m_nTileXSize * m_nFeetPerTileX;
    int32 nFeetPerGridCellY = m_nTileYSize * m_nFeetPerTileY;
    if (nFeetPerGridCellX <= 0 || nFeetPerGridCellY <= 0)
        return;
    SceneParams & tScene = *pdefWorld->ptServerData->ptScene;
    sPrefetchMarkerSets(tScene);
    for (int32 i = 0; i < tScene.nSceneChunkCount; ++i) {
        SceneChunk tSceneChunk = tScene.ptSceneChunks[i];
        IVector2D pointOriginSceneSpace;
        DRLGGetSceneOrigin(tSceneChunk, pointOriginSceneSpace);
        int32 nOffsetInFeetZ = Math::FloatRoundToInt(tSceneChunk.transform.wp.z);
        IVector2D pointSceneOrigin = pointOriginSceneSpace - pointWorldOrigin;
        IVector2D pointGridLocation(pointSceneOrigin.x / nFeetPerGridCellX, pointSceneOrigin.y / nFeetPerGridCellY);
        Vector3D wp(pointGridLocation.x * m_nFeetPerTileX, pointGridLocation.y * m_nFeetPerTileY, nOffsetInFeetZ * m_nFeetPerTileZ);
        DRLGPlacedScene tNewPlacedScene;
        tNewPlacedScene.tSceneSpec = tSceneChunk.tSceneSpec;
        tNewPlacedScene.tSceneSpec.dHeight = nOffsetInFeetZ;
        tNewPlacedScene.tSceneSpec.nSceneChunk = i;
        tNewPlacedScene.tSceneSpec.pointGridLocation = pointGridLocation;
        tNewPlacedScene.tSceneSpec.nDRLGIndex = m_nDRLGIndex++;
        tNewPlacedScene.snoname = tSceneChunk.snoname;
        tNewPlacedScene.wp = wp;
        if (tNewPlacedScene.tSceneSpec.snoMusic == SNO_NONE || tNewPlacedScene.tSceneSpec.snoMusic == 0)
            tNewPlacedScene.tSceneSpec.snoMusic = m_tDefaultSceneSpec.snoMusic;
        if (tNewPlacedScene.tSceneSpec.snoAmbient == SNO_NONE || tNewPlacedScene.tSceneSpec.snoAmbient == 0)
            tNewPlacedScene.tSceneSpec.snoAmbient = m_tDefaultSceneSpec.snoAmbient;
        if (tNewPlacedScene.tSceneSpec.snoReverb == SNO_NONE || tNewPlacedScene.tSceneSpec.snoReverb == 0)
            tNewPlacedScene.tSceneSpec.snoReverb = m_tDefaultSceneSpec.snoReverb;
        if (tNewPlacedScene.tSceneSpec.snoWeather == SNO_NONE || tNewPlacedScene.tSceneSpec.snoWeather == 0)
            tNewPlacedScene.tSceneSpec.snoWeather = m_tDefaultSceneSpec.snoWeather;
        if (tNewPlacedScene.tSceneSpec.arSnoLevelAreas[0] == SNOFileLookupGUID(LEVELAREA_SNO, cgszBadDataName))
            tNewPlacedScene.tSceneSpec.arSnoLevelAreas[0] = SNO_NONE;
        tNewPlacedScene.tSceneSpec.snoPresetWorld = m_snoWorld;
        tNewPlacedScene.tSceneSpec.dwOnPathBits = 0;
        tNewPlacedScene.tSceneSpec.nSceneChunk = i;
        if (tSceneChunk.snoname.snoGroup != SCENEGROUP_SNO) {
            MarkerFillPotentialSceneLabels(tNewPlacedScene);
            m_listPlacedScenes.AddTail_Tracked(tNewPlacedScene);
        }
    }
}

BOOL DRLGGenerateWorld(RNG & tRng, SWorldID idSWorld)
{
    SNO snoWorld = WorldCommonGetWorldSNO(idSWorld);
    BOOL bSuccess = FALSE;
    int32 nTry = 0;
    for (;;) {
        DRLGWorldGenerator tWorldGenerator;
        tWorldGenerator.Initialize(tRng, snoWorld, SWORLDID_INVALID);
        tWorldGenerator.GenerateWorldScenes();
        bSuccess = LabelsApplyRuleSet(tWorldGenerator, TRUE);
        if (bSuccess) {
            bSuccess = tWorldGenerator.PickSubScenes();
            if (bSuccess) {
                bSuccess = DRLGPlaceSpawnGroupExactInstances(idSWorld, tWorldGenerator.m_listPlacedScenes, tWorldGenerator.m_listSpawnGroupExact, *tWorldGenerator.m_ptRNG);
                if (!bSuccess) {
                    SWorld * ptSWorld = SWorldGet(idSWorld);
                    ptSWorld->listUniqueMonsterActorEntries.RemoveAll();
                }
            }
        }
        if (bSuccess || nTry >= 3) {
            DRLGCopySpawnGroupExactInstancesToWorld(idSWorld, tWorldGenerator.m_listSpawnGroupExact);
            DRLGPlaceTiles(idSWorld, tWorldGenerator.m_listPlacedScenes);
            DRLGPlaceGizmoLocMarkers(idSWorld, tWorldGenerator.m_listPlacedScenes, tRng);
            return bSuccess;
        }
        ++nTry;
    }
}

void DRLGGetWorldSize(SNO snoWorld, int32 & nMinTileSizeInFeetX, int32 & nMinTileSizeInFeetY, int32 & nWorldSizeInFeetX, int32 & nWorldSizeInFeetY)
{
    ScopedWorldDefinitionLockR pdefWorld(snoWorld);
    const SceneParams * ptScene = pdefWorld->ptServerData->ptScene;
    IVector2D pointWorldMin = cgptNull;
    IVector2D pointWorldMax = cgptNull;
    for (int32 i = 0; i < ptScene->nSceneChunkCount; ++i) {
        const SceneChunk & tSceneChunk = ptScene->ptSceneChunks[i];
        SNO snoScene = tSceneChunk.snoname.snoHandle;
        IVector2D pointSceneSize;
        DRLGGetSceneSize(snoScene, tSceneChunk.tSceneSpec.tCachedValues, pointSceneSize);
        IVector2D pointSceneMin;
        DRLGGetSceneOrigin(tSceneChunk, pointSceneMin);
        IVector2D pointSceneMax = pointSceneMin + pointSceneSize;
        if (i == 0) {
            pointWorldMin = pointSceneMin;
            pointWorldMax = pointSceneMax;
            nMinTileSizeInFeetX = pointSceneSize.x;
            nMinTileSizeInFeetY = pointSceneSize.y;
        } else {
            pointWorldMin.x = Math::Min(pointWorldMin.x, pointSceneMin.x);
            pointWorldMin.y = Math::Min(pointWorldMin.y, pointSceneMin.y);
            pointWorldMax.x = Math::Max(pointWorldMax.x, pointSceneMax.x);
            pointWorldMax.y = Math::Max(pointWorldMax.y, pointSceneMax.y);
            nMinTileSizeInFeetX = Math::Min(nMinTileSizeInFeetX, pointSceneSize.x);
            nMinTileSizeInFeetY = Math::Min(nMinTileSizeInFeetY, pointSceneSize.y);
        }
    }
    if (pdefWorld->uFlags & (1 << WDF_WORLD_USES_NON_UNIFORM_SCENE_SIZES)) {
        nMinTileSizeInFeetX = 10;
        nMinTileSizeInFeetY = 10;
    } else {
        for (int32 i = 0; i < ptScene->nSceneChunkCount; ++i) {
            const SceneChunk & tSceneChunk = ptScene->ptSceneChunks[i];
            IVector2D pointSceneMin;
            DRLGGetSceneOrigin(tSceneChunk, pointSceneMin);
            int32 nOffsetX = pointSceneMin.x - pointWorldMin.x;
            int32 nOffsetY = pointSceneMin.y - pointWorldMin.y;
            if (nMinTileSizeInFeetX && nMinTileSizeInFeetY && (nOffsetX % nMinTileSizeInFeetX || nOffsetY % nMinTileSizeInFeetY)) {
                nMinTileSizeInFeetX >>= 1;
                nMinTileSizeInFeetY >>= 1;
                if (nOffsetX % nMinTileSizeInFeetX || nOffsetY % nMinTileSizeInFeetY)
                {
                    nMinTileSizeInFeetX = 10;
                    nMinTileSizeInFeetY = 10;
                }
            }
        }
    }
    nWorldSizeInFeetX = pointWorldMax.x - pointWorldMin.x;
    nWorldSizeInFeetY = pointWorldMax.y - pointWorldMin.y;
}

void DRLGGetSceneOrigin(const SceneChunk & tSceneChunk, IVector2D & pointOriginInFeet)
{
    int32 nOffsetInFeetX = Math::FloatRoundToInt(tSceneChunk.transform.wp.x);
    int32 nOffsetInFeetY = Math::FloatRoundToInt(tSceneChunk.transform.wp.y);
    if (nOffsetInFeetX % 10 || nOffsetInFeetY % 10) {
        nOffsetInFeetX += 10 - nOffsetInFeetX % 10;
        nOffsetInFeetY += 10 - nOffsetInFeetY % 10;
    }
    pointOriginInFeet.Set(nOffsetInFeetX, nOffsetInFeetY);
}

void DRLGGetWorldOrigin(SNO snoWorld, IVector2D & pointWorldOrigin)
{
    ScopedWorldDefinitionLockR pdefWorld(snoWorld);
    pointWorldOrigin = cgptNull;
    if (!pdefWorld->ptServerData || !pdefWorld->ptServerData->ptScene)
        return;
    if (GameGetActiveGameCommonData().GameModeGet() == GAMEMODE_HOLODECK)
        return;
    const SceneParams & tScene = *pdefWorld->ptServerData->ptScene;
    for (int32 i = 0; i < tScene.nSceneChunkCount; ++i) {
        const SceneChunk & tSceneChunk = tScene.ptSceneChunks[i];
        IVector2D pointSceneOrigin;
        DRLGGetSceneOrigin(tSceneChunk, pointSceneOrigin);
        if (pointSceneOrigin.x < pointWorldOrigin.x || i == 0)
            pointWorldOrigin.x = pointSceneOrigin.x;
        if (pointSceneOrigin.y < pointWorldOrigin.y || i == 0)
            pointWorldOrigin.y = pointSceneOrigin.y;
    }
}

void DRLGGetSceneSize(SNO snoScene, const SceneCachedValues & tCachedValues, IVector2D & pointSizeInFeet)
{
    static BOOL sbEnableCachedNavmeshSize = TRUE;
    if (!sbEnableCachedNavmeshSize || (tCachedValues.dwCachedValuesValid & SCENE_NAVMESH_SIZE_VALID) != SCENE_NAVMESH_SIZE_VALID) {
        pointSizeInFeet = cgptNull;
        return;
    }
    int32 nNodesX = Math::Max(4, tCachedValues.nNavmeshSizeX);
    int32 nNodesY = Math::Max(4, tCachedValues.nNavmeshSizeY);
    if (nNodesX % 4 || nNodesY % 4) {
        nNodesX += 4 - nNodesX % 4;
        nNodesY += 4 - nNodesY % 4;
    }
    pointSizeInFeet.Set(nNodesX * 5 / 2, nNodesY * 5 / 2);
}
