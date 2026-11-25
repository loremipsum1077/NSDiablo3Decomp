#include "DRLGDungeonInternal.h"
#include "../Common/ThreadGlobals.h"
#include "../Common/TagMaps/Tags.h"
#include "../Common/World.h"
#include "../Sigma/Random.h"
#include "../Sigma/Memory/SigmaMemory.h"
#include "../Sigma/BlzExt/BlzVectorHelper.h"
#include "../Sigma/TypeDescriptor/SymbolicValue.h"
extern const int32 gAddExitX[4];  // external linkage in the original (GOT access)
extern const int32 gAddExitY[4];
static void sZeroExitWeightsForTileType(int32 * arDirWeights, DRLGTileType nTileType);
static void sSetExitWeightsForTileType(int32 * arDirWeights, DRLGTileType nTileType);
uint32 SWorldGetNumDungeonFinderLevels();  // X/Server/SWorld.cpp
SNO WorldGetDungeonFinderWorldFromLevel(int32 nLevel);  // X/Common/World.cpp
static DRLGTileType sExitDirectionToOppositeTileType(DRLGExitDirection nDirection);
extern const SymbolicEntry gDRLGTileSets[]; extern const SymbolicEntry gDRLGTileTypes[];  // X/Common/SNO/SymbolicValue.cpp
void WorldCacheSceneValues(SNO snoScene, SceneCachedValues & tCachedValues);  // X/Common/World.cpp
BOOL LabelsApplyRuleSet(DRLGGenerator & tGenerator, BOOL bServer); BOOL DRLGPlaceSpawnGroupExactInstances(SWorldID idSWorld, TPlacedSceneList & listPlacedScenes, TPooledSpawnGroupExactInstanceList & listSpawnGroupExact, RNG & tRNG); void NavmeshStitchingSetSizeForDRLGWorld(WorldCommonData & tWCD, int32 nSizeX, int32 nSizeY);
void DRLGCopySpawnGroupExactInstancesToWorld(SWorldID idSWorld, const TPooledSpawnGroupExactInstanceList & listSpawnGroupExact); void DRLGPlaceTiles(SWorldID idSWorld, TPlacedSceneList & listPlacedScenes); void DRLGPlaceGizmoLocMarkers(SWorldID idSWorld, TPlacedSceneList & listPlacedScenes, RNG & tRNG);
int32 WorldGetDungeonFinderLevel(const SNO snoWorld);  /* X/Common/World.cpp:828 */ SNO ActGetCurrentHubWorldSNO(); SNO LevelAreaFirstInWorld(SWorldID idSWorld);
SNO SWorldGetDungeonFinderWeather(SNO snoWorld); uint32 SWorldGetDungeonFinderWorldSeed(SNO snoWorld);  /* X/Server/SWorld.cpp */ BOOL SQuestsWorldIsRequiredByBounty(SNO snoWorld, SNO & snoScene);  // X/Server/Quests/SQuests.cpp
#include "../Common/GlobalSNO.h"
#include "../Common/Definitions/GlobalDefinition.h"
// Raw DWARF records only Marker.cpp:2775; no header declaration was recovered.
void MarkerFillPotentialSceneLabels(DRLGPlacedScene & tPlacedScene);
#include <float.h>






























struct ValidNubLocation
{
    int32 nOffset;
    int32 nWidth;
    int32 nMaxLength;
};
static_assert(sizeof(ValidNubLocation) == 12 && offsetof(ValidNubLocation, nOffset) == 0 && offsetof(ValidNubLocation, nWidth) == 4 && offsetof(ValidNubLocation, nMaxLength) == 8, "ValidNubLocation layout");
struct TileCoords
{
    int32 x;
    int32 y;
};



const int32 gAddExitX[4] = { -1, 1, 0, 0 };
const int32 gAddExitY[4] = { 0, 0, 1, -1 };

static const int32 gTransitionTags[4] =
{
    TAG_WORLD_TILE_TRANSITION_N,
    TAG_WORLD_TILE_TRANSITION_S,
    TAG_WORLD_TILE_TRANSITION_E,
    TAG_WORLD_TILE_TRANSITION_W,
};

static const int32 gEdgeTags[4] =
{
    TAG_WORLD_TILE_EDGE_TYPE_N,
    TAG_WORLD_TILE_EDGE_TYPE_S,
    TAG_WORLD_TILE_EDGE_TYPE_E,
    TAG_WORLD_TILE_EDGE_TYPE_W,
};























DRLGDungeonGlobals::DRLGDungeonGlobals()
{

    tTilePool.Initialize();
    tTilePool.SetBlockCount_Tracked(128);
    tPTilePool.Initialize();
    tPTilePool.SetBlockCount_Tracked(128);
    tListTilesPool.Initialize();
    tListTilesPool.SetBlockCount_Tracked(128);
    tDeadEndPool.Initialize();
    tDeadEndPool.SetBlockCount_Tracked(128);
    tDRLGDungeonGeneratorPool.Initialize();
    tDRLGDungeonGeneratorPool.SetBlockCount_Tracked(1);
}



DRLGDungeonGlobals::~DRLGDungeonGlobals()
{






}
DRLGDungeonGenerator * sDRLGDungeonGeneratorAlloc()
{

    DRLGDungeonGlobals *& ptDungeonGlobals = ThreadGlobals::Get().m_ptDRLGDungeonGlobals;
    if (!ptDungeonGlobals)
    {
        ptDungeonGlobals = new DRLGDungeonGlobals;
    }






    return ptDungeonGlobals->tDRLGDungeonGeneratorPool.Alloc_Tracked();
}
void sDRLGDungeonGeneratorFree(DRLGDungeonGenerator *& ptDungeonGenerator)
{
    ThreadGlobals::Get().m_ptDRLGDungeonGlobals->tDRLGDungeonGeneratorPool.Free(ptDungeonGenerator);
    ptDungeonGenerator = NULL;
}



static TDRLGTilePool * sDRLGDungeonGetTilePool()
{

	return &ThreadGlobals::Get().m_ptDRLGDungeonGlobals->tTilePool;
}


static TPDRLGTilePool * sDRLGDungeonGetPtrTilePool()
{

	return &ThreadGlobals::Get().m_ptDRLGDungeonGlobals->tPTilePool;
}


static TListTilesPool * sDRLGDungeonGetListTilePool()
{

	return &ThreadGlobals::Get().m_ptDRLGDungeonGlobals->tListTilesPool;
}


static TDRLGDeadEndPool * sDRLGDungeonGetDeadEndPool()
{

	return &ThreadGlobals::Get().m_ptDRLGDungeonGlobals->tDeadEndPool;
}


void DRLGDungeonDisposeGlobals()
{


    delete ThreadGlobals::Get().m_ptDRLGDungeonGlobals; ThreadGlobals::Get().m_ptDRLGDungeonGlobals = NULL;
}

BOOL CreateSuccessfulDRLGDungeonGenerator(RNG & tRNG, SNO snoWorld, SWorldID idSWorld, int32 nMaxTries, int32 & nAttemptedTries, DRLGDungeonGenerator *& pCreatedGenerator)
{
    BOOL bSuccess;
    do
    {
        if (pCreatedGenerator)
            sDRLGDungeonGeneratorFree(pCreatedGenerator);
        pCreatedGenerator = sDRLGDungeonGeneratorAlloc();
        pCreatedGenerator->Initialize(tRNG, snoWorld, idSWorld);
        bSuccess = !pCreatedGenerator->GenerateDungeon();
        if (!bSuccess)
            nAttemptedTries++;
    } while (nAttemptedTries < nMaxTries && !bSuccess);
    return bSuccess;
}


































































































BOOL DRLGGenerateDungeon(
    RNG & tRNG,
    SWorldID idSWorld)
{
    DRLGDungeonGenerator * ptDungeonGen = NULL;
    BOOL bSuccess = FALSE;

    SNO snoWorld = WorldCommonGetWorldSNO(idSWorld);



    WorldCommonData & tWCD = WorldGetCommonData(idSWorld);
    if (WorldIsDungeonFinderPlaceHolder(snoWorld) && tWCD.snoDungeonFinderSourceWorld != SNO_NONE)
    {
        snoWorld = tWCD.snoDungeonFinderSourceWorld;
    }

    int32 nAttemptedDRLGTries = 0;
    int32 nAttemptedRuleTries = 0;

    while (nAttemptedRuleTries < 3 && nAttemptedDRLGTries < 100)
    {
        if (CreateSuccessfulDRLGDungeonGenerator(tRNG, snoWorld, idSWorld, 100, nAttemptedDRLGTries, ptDungeonGen))
        {


            if (LabelsApplyRuleSet(*ptDungeonGen, TRUE) &&

                ptDungeonGen->PickSubScenes())
            {
                if (DRLGPlaceSpawnGroupExactInstances(
                        idSWorld, ptDungeonGen->m_listPlacedScenes,
                        ptDungeonGen->m_listSpawnGroupExact,
                        *ptDungeonGen->m_ptRNG))
                {
                    bSuccess = TRUE;
                    break;
                }
                else
                {

                    SWorld * ptSWorld = SWorldGet(idSWorld);
                    ptSWorld->listUniqueMonsterActorEntries.RemoveAll();
                }
            }

            nAttemptedRuleTries++;
        }
    }










    if (bSuccess)
    {
        if (GameActiveGameCommonDataIsServer())
        {
            IVector2D posMin, posMax;
            ptDungeonGen->GetGridBounds(posMin, posMax);




            IVector2D vSize = posMax - posMin + IVector2D(2, 2);

            SWorld * ptSWorld = SWorldGet(idSWorld);
            NavmeshStitchingSetSizeForDRLGWorld(ptSWorld->tWCD, vSize.x, vSize.y);
        }

        DRLGCopySpawnGroupExactInstancesToWorld(idSWorld, ptDungeonGen->m_listSpawnGroupExact);
        DRLGPlaceTiles(idSWorld, ptDungeonGen->m_listPlacedScenes);
        DRLGPlaceGizmoLocMarkers(idSWorld, ptDungeonGen->m_listPlacedScenes, tRNG);
    }

    sDRLGDungeonGeneratorFree(ptDungeonGen);
    return bSuccess;
}





















static BOOL sIsValidTileType(DRLGTileType nTileType)
{



    int32 nType = nTileType;
    return nType > DRLG_TILE_NONE && nType < DRLG_TILE_NUM_TYPES;
}


static DRLGTileType sExitDirectionToTileType(DRLGExitDirection nDirection)
{
    static const DRLGTileType sTileTypesForDirection[DRLG_EXIT_CNT] = { DRLG_TILE_N, DRLG_TILE_S, DRLG_TILE_E, DRLG_TILE_W };
    DRLGTileType nTileType = DRLG_TILE_NONE;
    if (nDirection < DRLG_EXIT_NORTH || nDirection >= DRLG_EXIT_CNT)
        return nTileType;
    nTileType = sTileTypesForDirection[nDirection];
    return nTileType;
}














static DRLGExitDirection sOppositeDirection(DRLGExitDirection nDirection)
{
    static const DRLGExitDirection sOppositeDirections[DRLG_EXIT_CNT] = { DRLG_EXIT_SOUTH, DRLG_EXIT_NORTH, DRLG_EXIT_WEST, DRLG_EXIT_EAST };
    DRLGExitDirection nOppositeDirection = DRLG_EXIT_NONE;
    if (nDirection < DRLG_EXIT_NORTH || nDirection >= DRLG_EXIT_CNT)
        return nOppositeDirection;
    nOppositeDirection = sOppositeDirections[nDirection];
    return nOppositeDirection;
}














static DRLGTileType sExitDirectionToOppositeTileType(
    DRLGExitDirection nDirection)
{
    DRLGExitDirection eOppositeDirection = sOppositeDirection(nDirection);


    return sExitDirectionToTileType(eOppositeDirection);
}



static DRLGExitDirection sRotateExitDirectionLeft(
    DRLGExitDirection nDirection)
{
    static const DRLGExitDirection sLeftDirections[DRLG_EXIT_CNT] =
    {
        DRLG_EXIT_WEST,
        DRLG_EXIT_EAST,
        DRLG_EXIT_NORTH,
        DRLG_EXIT_SOUTH,
    };

    return sLeftDirections[nDirection];
}




static DRLGExitDirection sRotateExitDirectionRight(
    DRLGExitDirection nDirection)
{
    static const DRLGExitDirection sRightDirections[DRLG_EXIT_CNT] =
    {
        DRLG_EXIT_EAST,
        DRLG_EXIT_WEST,
        DRLG_EXIT_SOUTH,
        DRLG_EXIT_NORTH,
    };

    return sRightDirections[nDirection];
}






static BOOL sIsLinkPossible(DRLGTileType nTileTypeParent, DRLGExitDirection nDirection, DRLGTileType nTileTypeChild)
{
    BOOL bLinkPossible = FALSE;


    if (nDirection < DRLG_EXIT_NORTH || nDirection >= DRLG_EXIT_CNT)
        return bLinkPossible;

    DRLGTileType nParentToChildTile = sExitDirectionToTileType(nDirection);


    if (nTileTypeParent & nParentToChildTile)
        bLinkPossible = TRUE;
    return bLinkPossible;
}










static int32 sExitCountForTileType(
    DRLGTileType nTileType)
{
    int32 nMask = nTileType;
    int nExitCnt = 0;
    while (nMask)
    {
        nMask &= nMask - 1;
        nExitCnt++;
    }
    return nExitCnt;
}






static DRLGTileType GetTileTypeFromWeights(
    int32 * arDirectionWeights)
{
    int32 nType = 0;

    for (int32 i = 0; i < DRLG_EXIT_CNT; i++)
    {
        if (arDirectionWeights[i] > 0)
            nType |= (1 << i);
    }
    return nType;
}




static void sGetTileDimensions(
    const TileInfo & tTileInfo,
    int32 & nSceneSizeX,
    int32 & nSceneSizeY)
{
    nSceneSizeX = TagMapGetInt(tTileInfo.hTileTagMap, TAG_WORLD_TILE_SCENE_SIZE_X);
    nSceneSizeY = TagMapGetInt(tTileInfo.hTileTagMap, TAG_WORLD_TILE_SCENE_SIZE_Y);

    if (tTileInfo.nTileType == DRLG_TILE_CUSTOM)
    {
        nSceneSizeX = tTileInfo.tCustomTileInfo.nSizeX;
        nSceneSizeY = tTileInfo.tCustomTileInfo.nSizeY;
    }
}



static int32 sGetSpecialTileMinSpacing(const TileInfo * ptTileInfo, DRLGSpecialTileType nSpecialType)
{
    if (nSpecialType >= DRLG_SPECIAL_TILE_0 && nSpecialType < DRLG_SPECIAL_TILE_NUM_TYPES)
    {
        return TagMapGetInt(ptTileInfo->hTileTagMap, TAG_WORLD_TILE_SPECIAL_TILE_MIN_SPACING_0 + nSpecialType);
    }
    else
    {
        int32 nMaxSpacing = 0;
        for (int32 i = 0; i < DRLG_SPECIAL_TILE_NUM_TYPES; i++)
        {
            int32 nMinSpacing = TagMapGetInt(ptTileInfo->hTileTagMap, TAG_WORLD_TILE_SPECIAL_TILE_MIN_SPACING_0 + i);
            if (nMinSpacing > nMaxSpacing) nMaxSpacing = nMinSpacing;
        }
        return nMaxSpacing;
    }
}









static WorldDRLGCommandTag sctTileTypeAlternateTags[] =
{
    TAG_WORLD_DRLG_COMMAND_TILE_TYPE_ALT_1,
    TAG_WORLD_DRLG_COMMAND_TILE_TYPE_ALT_2,
    TAG_WORLD_DRLG_COMMAND_TILE_TYPE_ALT_3,
};

static DRLGTileType sGetCommandRandomTileType(const DRLGCommand * const ptCommand, RNG & tRNG)
{
    const int32 nTileTypeAlternateTagsCount = sizeof(sctTileTypeAlternateTags) / sizeof(sctTileTypeAlternateTags[0]);

    DRLGTileType eTileType = TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_TILE_TYPE);


    int32 nTagIndex = 0;
    for (int32 i = 0; i < nTileTypeAlternateTagsCount; i++)
    {
        if (!TagMapTagExists(ptCommand->hCommandTagMap, sctTileTypeAlternateTags[i]))
            break;
        nTagIndex++;
    }




    nTagIndex = RandRange_Internal(tRNG, 0, nTagIndex);
    if (nTagIndex > 0)
    {
        eTileType = TagMapGetInt(ptCommand->hCommandTagMap, sctTileTypeAlternateTags[nTagIndex - 1]);
    }

    return eTileType;
}











inline DRLGTileConstraint::DRLGTileConstraint()
{
    nMaxExitCnt = -1;
    nRoomCreateNum = -1;
    nRoomCreateId = -1;
    nGroupNum = -1;
    nHubLength = 0;
    nHubWidth = 0;
    nNumSpokeRooms = 0;
    nAllowedSpokeDir = DRLG_TILE_NONE;
    uSpokeTileTypeFlags = 0;
    ptFilterCommand = NULL;
    ptOwnerCommand = NULL;
    ptFillHubCommand = NULL;
    eChokepointType = (DRLGChokepointType)-1;
    eChokepointDir = (DRLGExitDirection)-1;
    nChokepointX = 0;
    nChokepointY = 0;
    nChokepointLength = 0;
    nChokepointWidth = 0;
    nChokepointExcludeDist = 0;
    nFilterFlag = -1;
    ptTileInfo = NULL;
}












BOOL DRLGTile::IsDummy() const
{

    return (nTileSet == GRIDDATA_TILE_SET_DUMMY);
}




BOOL DRLGTile::IsBorderTile() const
{

    return dwFlags & (1 << DRLG_TILE_FLAG_BORDER_TILE);
}



BOOL DRLGTile::IsDeadEndTile() const
{

	return (dwFlags & (1 << DRLG_TILE_FLAG_DEADEND_TILE)) != 0;
}



BOOL DRLGTile::IsRoadTile() const
{

    return (dwFlags & (1 << DRLG_TILE_FLAG_ROAD_TILE)) != 0;
}



BOOL DRLGTile::IsPerimeterTile() const
{

    return (dwFlags & (1 << DRLG_TILE_FLAG_PERIMETER_TILE)) != 0;
}



BOOL DRLGTile::IsReplacementTile() const
{

    return (dwFlags & (1 << DRLG_TILE_FLAG_REPLACEMENT_TILE)) != 0;
}



BOOL DRLGTile::UsesEntranceTileSet() const
{

    return nTileSet >= 200 && nTileSet < 220;
}



BOOL DRLGTile::UsesExitTileSet() const
{

    return nTileSet >= 300 && nTileSet < 320;
}




BOOL DRLGTile::IsExitRequired(
    DRLGExitDirection nDirection) const
{
    BOOL bRequired = FALSE;

    if (sIsValidTileType(nTileType))
    {
        bRequired = sIsLinkPossible(nTileType, nDirection, DRLG_TILE_UNDEFINED);
    }

    return bRequired;
}



BOOL DRLGTile::CanRemoveTile() const
{
    BOOL bCanRemove = TRUE;




    if (nTileSet < GRIDDATA_TILE_SET_RAT_MAZE_1 || nTileSet > GRIDDATA_TILE_SET_RAT_MAZE_20)
        return FALSE;








    for (int32 i = 0; i < DRLG_EXIT_CNT; i++)
    {
        if (ExitExists(i))
        {
            DRLGTile * ptNeighbor = arNeighbors[i];



            if (sIsValidTileType(ptNeighbor->nTileType))
            {
                bCanRemove = FALSE;
                break;
            }
        }
    }
    return bCanRemove;
}



BOOL DRLGTile::CanAddTile(DRLGExitDirection nDirection, int32 _nHeight) const
{
    BOOL bCanAdd = FALSE;



    if (arNeighbors[nDirection] == NULL &&
        sIsLinkPossible(nAllowedExits, nDirection, DRLG_TILE_UNDEFINED) &&
        GetCurrentExitsCnt() < nMaxExits &&
        GetExitHeight(nDirection) == _nHeight)
    {
        bCanAdd = TRUE;
    }
    return bCanAdd;
}





BOOL DRLGTile::CanAddExit(
    DRLGExitDirection nDirection,
    BOOL bCheckNeighbor) const
{
    BOOL bCanAdd = FALSE;

    if (arNeighbors[nDirection] != NULL &&
        !ExitExists(nDirection) &&
        sIsLinkPossible(nAllowedExits, nDirection, DRLG_TILE_UNDEFINED) &&
        GetCurrentExitsCnt() < nMaxExits)
    {
        if (bCheckNeighbor)
        {
            DRLGExitDirection nOppDirection = sOppositeDirection(nDirection);


            if (GetExitHeight(nDirection) == arNeighbors[nDirection]->GetExitHeight(nOppDirection))
            {
                bCanAdd = arNeighbors[nDirection]->CanAddExit(nOppDirection, FALSE);
            }
        }
        else
        {
            bCanAdd = TRUE;
        }
    }
    return bCanAdd;
}




int32 DRLGTile::GetCurrentExitsCnt() const
{

    int32 nExitCnt = sExitCountForTileType(nCurrentExits);
    return nExitCnt;
}




BOOL DRLGTile::ExitExists(DRLGExitDirection nDirection) const
{
    BOOL bExists = FALSE;





    bExists = (sExitDirectionToTileType(nDirection) & nCurrentExits) != 0;
    return bExists;
}




DRLGTile * DRLGTile::GetNeighbor(DRLGExitDirection nDirection) const
{
    DRLGTile * ptNeighbor = NULL;
    if (nDirection < DRLG_EXIT_NORTH || nDirection >= DRLG_EXIT_CNT)
        return ptNeighbor;
    ptNeighbor = arNeighbors[nDirection];
    return ptNeighbor;
}







int32 DRLGTile::GetExitHeight(DRLGExitDirection nDirection) const
{
    int32 nElevation = nHeight + arRelativeExitHeights[nDirection];
    return nElevation;
}








DRLGEdgeType DRLGTile::GetEdgeType(DRLGExitDirection nDirection) const
{


    return arEdges[nDirection];
}



BOOL DRLGTile::HasLabel(DRLGLabelType nLabel) const
{
    for (int32 i = 0; i < 4; i++)
    {
        if (arLabels[i] == DRLG_LABEL_NONE)
            return FALSE;
        if (arLabels[i] == nLabel)
            return TRUE;
    }
    return FALSE;
}



















BOOL DRLGTile::FormsSwastikaPivotPoint() const
{

    if (nTileType != DRLG_TILE_NSEW && nCurrentExits != DRLG_TILE_NSEW)
        return FALSE;


    DRLGTile * pNorthNeighbor = GetNeighbor(DRLG_EXIT_NORTH);
    if (!pNorthNeighbor)
        return FALSE;
    if (pNorthNeighbor->nTileType != DRLG_TILE_SE && pNorthNeighbor->nCurrentExits != DRLG_TILE_SE)
        return FALSE;


    DRLGTile * pEastNeighbor = GetNeighbor(DRLG_EXIT_EAST);
    if (!pEastNeighbor)
        return FALSE;
    if (pEastNeighbor->nTileType != DRLG_TILE_SW && pEastNeighbor->nCurrentExits != DRLG_TILE_SW)
        return FALSE;


    DRLGTile * pSouthNeighbor = GetNeighbor(DRLG_EXIT_SOUTH);
    if (!pSouthNeighbor)
        return FALSE;
    if (pSouthNeighbor->nTileType != DRLG_TILE_NW && pSouthNeighbor->nCurrentExits != DRLG_TILE_NW)
        return FALSE;


    DRLGTile * pWestNeighbor = GetNeighbor(DRLG_EXIT_WEST);
    if (!pWestNeighbor)
        return FALSE;
    if (pWestNeighbor->nTileType != DRLG_TILE_NE && pWestNeighbor->nCurrentExits != DRLG_TILE_NE)
        return FALSE;

    return TRUE;
}











DRLGDungeonGenerator::DRLGDungeonGenerator()
    :
    m_nCreateId(0),
    m_nGroupIdStackCnt(0),
    m_nTileConstraintsCnt(0),
    m_nLabelStackCnt(0),
    m_nCommandStackCnt(0),
    m_arCommands(NULL),
    m_nCommandCnt(0),
    m_arCommandParentIndices(NULL),
    m_nDRLGIndex(0),
    m_nFeetPerTileX(0),
    m_nFeetPerTileY(0),
    m_nFeetPerTileZ(0),
    m_nTileSizeX(0),
    m_nTileSizeY(0),
    m_nTileOffsetX(0),
    m_nTileOffsetY(0),
    m_nHeightChangeToNorth(0),
    m_nHeightChangeToWest(0),
    m_sCreationLog(),

    m_bErrors(FALSE),
    m_bWarnings(FALSE),
    m_bWarningsAsErrors(FALSE),
    m_bPatchTileExits(FALSE),
    m_bTryFixRepeatTiles(FALSE),
    m_bCheckForSwastika(FALSE),
    m_bUseEdgeTypes(FALSE),
    m_nMaxCurGenVariationPerTileType(0),
    m_eRepeatTileSet(0),
    m_nCommandTilesCount(0),
    m_TileFrequencies(),

    m_arSpecialTiles(),
    m_snoDungeonFinderWeather(SNO_NONE),
    m_eBountyRequiredTileType(DRLG_TILE_UNDEFINED),
    m_eBountyRequiredTileSet(0),
    m_snoBountyRequiredScene(SNO_NONE),
    m_bBountyRequiredScenePlaced(FALSE)
{
    for (int i = 0; i < MAX_NUM_NESTED_DRLG_COMMANDS; i++)
    {
        m_arGroupIdStack[i] = 0;
    }

    for (int i = 0; i < MAX_NUM_NESTED_DRLG_COMMANDS; i++)
    {
        m_arCommandStack[i] = NULL;
    }

    for (int i = 0; i < 4; i++)
    {
        m_arLabelStack[i] = DRLG_LABEL_NONE;
    }
}


void DRLGDungeonGenerator::InitSpecific()

{
    m_listTiles.SetPool(sDRLGDungeonGetTilePool());

    ScopedWorldDefinitionLockR pdefWorld(m_snoWorld);


    if (pdefWorld->ptServerData->ptDRLG)
    {
        m_nCommandCnt = pdefWorld->ptServerData->ptDRLG->nNumCommands;






        m_nFeetPerTileX = TagMapGetInt(pdefWorld->ptServerData->ptDRLG->hDRLGTagMap, TAG_WORLD_STITCH_SIZE_X);
        m_nFeetPerTileY = TagMapGetInt(pdefWorld->ptServerData->ptDRLG->hDRLGTagMap, TAG_WORLD_STITCH_SIZE_Y);
        m_nFeetPerTileZ = 1;

        m_nTileSizeX = TagMapGetInt(pdefWorld->ptServerData->ptDRLG->hDRLGTagMap, TAG_WORLD_TILE_SIZE_X);
        m_nTileSizeY = TagMapGetInt(pdefWorld->ptServerData->ptDRLG->hDRLGTagMap, TAG_WORLD_TILE_SIZE_Y);

        if (m_nFeetPerTileX > 0)
        {
            m_nTileSizeX /= m_nFeetPerTileX;
        }
        else
        {
            DRLGTestEvent tEvent(
                m_dwSeed,
                m_bWarningsAsErrors ? DRLGTestEvent::STATUS_ERROR : DRLGTestEvent::STATUS_WARNING,
                CRefString("Invalid tile size/stitch size in X dimension.\n(Defaulting to 100).\nFix this in the World DRLG Properties tags."),
                DRLG_COMMAND_NONE,
                DRLG_ERROR_RESOLUTION_NONE,
                DRLG_TILE_NONE,
                -1,
                DRLG_STAGE_PRE_GENERATION);

            m_DRLGEvents.push_back(tEvent);


            if (m_nTileSizeX == 0)
            {
                m_nTileSizeX = 100;
            }

            m_nFeetPerTileX = m_nTileSizeX;
            m_nTileSizeX = 1;
        }

        if (m_nFeetPerTileY > 0)
        {
            m_nTileSizeY /= m_nFeetPerTileY;
        }
        else
        {
            DRLGTestEvent tEvent(
                m_dwSeed,
                m_bWarningsAsErrors ? DRLGTestEvent::STATUS_ERROR : DRLGTestEvent::STATUS_WARNING,
                CRefString("Invalid tile size/stitch size in Y dimension.\n(Defaulting to 100).\nFix this in the World DRLG Properties tags."),
                DRLG_COMMAND_NONE,
                DRLG_ERROR_RESOLUTION_NONE,
                DRLG_TILE_NONE,
                -1,
                DRLG_STAGE_PRE_GENERATION);

            m_DRLGEvents.push_back(tEvent);


            if (m_nTileSizeY == 0)
            {
                m_nTileSizeY = 100;
            }

            m_nFeetPerTileY = m_nTileSizeY;
            m_nTileSizeY = 1;
        }

        m_nHeightChangeToNorth = TagMapGetInt(pdefWorld->ptServerData->ptDRLG->hDRLGTagMap, TAG_WORLD_HEIGHT_CHANGE_TO_NORTH);
        m_nHeightChangeToWest = TagMapGetInt(pdefWorld->ptServerData->ptDRLG->hDRLGTagMap, TAG_WORLD_HEIGHT_CHANGE_TO_WEST);
        m_bPatchTileExits = TagMapGetInt(pdefWorld->ptServerData->ptDRLG->hDRLGTagMap, TAG_WORLD_PATCH_EXITS);
        m_bTryFixRepeatTiles = TagMapGetInt(pdefWorld->ptServerData->ptDRLG->hDRLGTagMap, TAG_WORLD_FIX_REPEATS);
        m_bCheckForSwastika = TagMapGetInt(pdefWorld->ptServerData->ptDRLG->hDRLGTagMap, TAG_WORLD_CHECK_FOR_SWASTIKAS);
        m_bUseEdgeTypes = TagMapGetInt(pdefWorld->ptServerData->ptDRLG->hDRLGTagMap, TAG_WORLD_USE_EDGE_TYPES);
        m_nMaxCurGenVariationPerTileType = TagMapGetInt(pdefWorld->ptServerData->ptDRLG->hDRLGTagMap, TAG_WORLD_MAX_CUR_GEN_VARIATION_PER_TILE_TYPE);
        m_eRepeatTileSet = TagMapGetInt(pdefWorld->ptServerData->ptDRLG->hDRLGTagMap, TAG_WORLD_REPEATS_TILE_SET);
    }


    if (IsDungeonFinder() && GameActiveGameCommonDataIsServer())
    {
        m_snoDungeonFinderWeather = SWorldGetDungeonFinderWeather(SWorldGet(m_idSWorld)->tWCD.snoWorld);

        if (m_snoDungeonFinderWeather == SNO_NONE)
        {
            ScopedWorldDefinitionLockR pdefWorld2(m_snoWorld);
            int nWeatherCount = pdefWorld2->ptServerData->serWeatherSNOs.dwSize / sizeof(SNO);
            if (nWeatherCount > 0)
            {
                RNG tRNG;
                RandInit(tRNG);
                uint32 dwSeed = SWorldGetDungeonFinderWorldSeed(WorldCommonGetWorldSNO(m_idSWorld));
                RandSetSeed_Internal(tRNG, dwSeed);

                int weatherIndex = Rand_Internal(tRNG, nWeatherCount - 1);
                m_snoDungeonFinderWeather = pdefWorld2->ptServerData->arWeatherSNOs[weatherIndex];
            }
        }
    }

    if (GameActiveGameCommonDataIsServer() && SQuestsWorldIsRequiredByBounty(m_snoWorld, m_snoBountyRequiredScene))
    {
        if (m_snoBountyRequiredScene != SNO_NONE)
        {
            for (int32 nTileIndex = 0; nTileIndex < pdefWorld->ptServerData->ptDRLG->arTiles.GetArraySize(); nTileIndex++)
            {
                TileInfo & tTileInfo = pdefWorld->ptServerData->ptDRLG->arTiles[nTileIndex];
                if (tTileInfo.snoScene == m_snoBountyRequiredScene)
                {
                    m_eBountyRequiredTileType = tTileInfo.nTileType;
                    m_eBountyRequiredTileSet = tTileInfo.nTileSet;
                }
            }
        }
    }








}


BOOL DRLGDungeonGenerator::GenerateDungeon()
{


    ScopedWorldDefinitionLockR pdefWorld(m_snoWorld);
    if (pdefWorld && pdefWorld->ptServerData && pdefWorld->ptServerData->ptDRLG)
    {
        m_arCommandParentIndices = pdefWorld->ptServerData->ptDRLG->pParentIndices;
        m_arCommands = pdefWorld->ptServerData->ptDRLG->ptCommands;
    }
    else
    {
        m_arCommandParentIndices = NULL;
        m_arCommands = NULL;
    }

    if (m_nCommandCnt > 0)
    {
        ExecuteCommand(0);
    }

    if (m_bPatchTileExits)
    {
        PatchTileExits();
    }

    if (m_bErrors)
        return m_bErrors;
    {



        SpokesConnectEnds();

        if (m_bTryFixRepeatTiles &&

            m_eRepeatTileSet != 0)
        {




            FixRepeatTiles();
        }



        SetEdgeTypes();


        if (m_bCheckForSwastika)
        {
            POS pos = m_listTiles.GetHeadPosition();
            while (pos)
            {
                DRLGTile & tTile = m_listTiles.GetNext(pos);
                if (tTile.FormsSwastikaPivotPoint())
                {












                    m_bErrors = TRUE;
                }
            }
        }

        IVector2D posMin, posMax;
        GetGridBounds(posMin, posMax);


        m_nTileOffsetX = 1 - posMin.x;
        m_nTileOffsetY = 1 - posMin.y;




        int32 nNumRemaining = m_listTiles.Count();
        while (nNumRemaining > 1)
        {
            int32 nIndex = PickRand(nNumRemaining);
            POS pos = m_listTiles.GetHeadPosition();
            while (nIndex > 0 && pos)
            {
                m_listTiles.GetNext(pos);
                nIndex--;
            }


            m_listTiles.MoveToTail(pos);
            nNumRemaining--;
        }

        PlaceTiles();

        CacheSceneValues();









        if (!m_bWarnings && !m_bErrors)
        {




            DRLGTestEvent tSuccess(
                m_dwSeed,
                DRLGTestEvent::STATUS_SUCCEESS,
                CRefString("N/A"),
                DRLG_COMMAND_NONE,
                DRLG_ERROR_RESOLUTION_NONE,
                DRLG_TILE_NONE,
                -1,
                DRLG_STAGE_NONE);

            m_DRLGEvents.push_back(tSuccess);
        }
    }
    return m_bErrors;
}





void DRLGDungeonGenerator::PlaceTiles()
{
    ScopedWorldDefinitionLockR pdefWorld(m_snoWorld);


    BOOL bReplacingTiles = m_arPendingTileReplacements.size() > 0;

    POS pos = m_listTiles.GetHeadPosition();
    while (pos && !m_bErrors)
    {
        DRLGTile & tTile = m_listTiles.GetAt(pos);


        if (!sIsValidTileType(tTile.nTileType) && !(tTile.dwFlags & (1 << DRLG_TILE_FLAG_BORDER_TILE)))
        {
            if (sIsValidTileType(tTile.nCurrentExits))
                tTile.nTileType = tTile.nCurrentExits;
            else
                tTile.nTileType = DRLG_TILE_NSEW;
        }












        int32 nCommandIndex = (int32)(tTile.ptCommand - m_arCommands);
        PushCommandTree(nCommandIndex);

        PlaceTile(&tTile);

        PlaceBorderTiles(&tTile);


        POS posBeforeAdditions = pos;
        m_listTiles.GetNext(posBeforeAdditions); pos = posBeforeAdditions;

        PopCommandTree(nCommandIndex);


        if (bReplacingTiles && m_arPendingTileReplacements.size() == 0)
        {
            pos = m_listTiles.GetHeadPosition();
            bReplacingTiles = FALSE;
        }
    }


    if (m_arPendingTileReplacements.size() > 0)
    {
        DRLGTestEvent tEvent(
            m_dwSeed,
            DRLGTestEvent::STATUS_ERROR,
            CRefString().Format("[Post-Generation] PlaceTiles(): Failed to do all tile substitutions (%d remaining).  Rerolling Dungeon...", m_arPendingTileReplacements.size()),
            DRLG_COMMAND_NONE,
            DRLG_ERROR_RESOLUTION_TILES_OMITTED,
            DRLG_TILE_NONE,
            -1,
            DRLG_STAGE_POST_GENERATION);

        m_DRLGEvents.push_back(tEvent);





        m_bErrors = TRUE;
    }
}
// NON_MATCHING: 2520 vs 2524 bytes; height-offset arithmetic uses MADD instead of two MULs and ADD, with different register allocation.
void DRLGDungeonGenerator::PlaceTile(
    DRLGTile * ptTile)
{

    if (ptTile->IsDummy())
        return;



    SNONameArray listSceneSNONames;
    listSceneSNONames.reserve(m_nTileSizeX * m_nTileSizeY);




    if (m_arPendingTileReplacements.size() > 0)
    {
        if (SNONAME_NONE == ptTile->snoName)
        {
            ptTile->snoName = PickReplacementTile(ptTile, ptTile->nTileType, ptTile->nTileSet, listSceneSNONames);
        }
        return;
    }

    if (SNONAME_NONE == ptTile->snoName)
    {
        ptTile->snoName = PickTile(ptTile, ptTile->nTileType, ptTile->nTileSet, listSceneSNONames);
    }
    else
    {
        listSceneSNONames.push_back(ptTile->snoName);
    }

    if (SNONAME_NONE == ptTile->snoName)
    {
        const char * szTileSetStr = SymbolToName(ptTile->nTileSet, gDRLGTileSets);

        DRLGTestEvent tEvent(
            m_dwSeed,
            m_bWarningsAsErrors ? DRLGTestEvent::STATUS_ERROR : DRLGTestEvent::STATUS_WARNING,
            CRefString().Format("Unable to place tile.\nCould not find required tile in tile set '%s'.\nThis will leave a gap in the dungeon!", szTileSetStr),
            ptTile->ptCommand ? ptTile->ptCommand->nCommandType : DRLG_COMMAND_NONE,
            DRLG_ERROR_RESOLUTION_TILES_OMITTED,
            ptTile->nTileType,
            ptTile->nCommandIndex,
            DRLG_STAGE_POST_GENERATION);

        m_DRLGEvents.push_back(tEvent);

        CRefString sWarnMsg;
        const char * szTileTypeStr = SymbolToName(ptTile->nTileType, gDRLGTileTypes);
        const char * szCommandName = ptTile->ptCommand ? ptTile->ptCommand->szCommandName : "Unknown";

        sWarnMsg.Format("[Post-Generation] PlaceTile(): Could not find tile of type '%s' in tile set '%s' for tile #%d.  Tile added by command #%d '%s'.  This will leave a gap in your dungeon - you should fix this.",
            szTileTypeStr, szTileSetStr, ptTile->nCreateId + 1, ptTile->nCommandIndex, szCommandName);
        // The original diagnostic consumer is erased; only Format survives.


        return;
    }












































    IVector2D pointLocation;
    IVector2D pointStitchSpace;

    pointLocation.x = ptTile->nXCoord + m_nTileOffsetX;
    pointLocation.y = ptTile->nYCoord + m_nTileOffsetY;




    LayoutPointToStitchPoint(pointLocation, pointStitchSpace);

    int32 nHeightOffsetX = pointLocation.x * m_nHeightChangeToNorth;
    int32 nHeightOffsetY = pointLocation.y * m_nHeightChangeToWest;



    SceneDRLGCreationInfo tDRLGCreationInfo;
    tDRLGCreationInfo.bCreatedFromDRLG = TRUE;
    tDRLGCreationInfo.nAddOrder = ptTile->nCreateId;
    tDRLGCreationInfo.nDRLGCommandIndex = ptTile->nCommandIndex;
    tDRLGCreationInfo.nCommandType = ptTile->ptCommand ? ptTile->ptCommand->nCommandType : DRLG_COMMAND_NONE;
    tDRLGCreationInfo.bIsBorderTile = ptTile->IsBorderTile();
    tDRLGCreationInfo.bIsDeadEndTile = ptTile->IsDeadEndTile();
    tDRLGCreationInfo.bIsRoadTile = ptTile->IsRoadTile();
    tDRLGCreationInfo.bIsPerimeterTile = ptTile->IsPerimeterTile();
    tDRLGCreationInfo.bIsReplacementTile = ptTile->IsReplacementTile();
    tDRLGCreationInfo.eTileSet = ptTile->nTileSet;
    tDRLGCreationInfo.bIgnoreNavmesh = ptTile->bIgnoreNavmesh;





    for (int i = 0; i < m_nTileSizeY; i++)
    {
        for (int j = 0; j < m_nTileSizeX; j++)
        {
            DRLGPlacedScene tNewPlacedScene;

            int32 nIndex = i * m_nTileSizeX + j;
            if (nIndex >= (int32)listSceneSNONames.size())
            {
                return;
            }


            tNewPlacedScene.snoname = listSceneSNONames[nIndex];
            if (tNewPlacedScene.snoname.snoHandle == SNO_NONE)
            {
                continue;
            }





            IVector2D pointStitchSpaceScene;
            pointStitchSpaceScene.x = pointStitchSpace.x + j;
            pointStitchSpaceScene.y = pointStitchSpace.y + i;

            BOOL bValidSceneSpec = GetSceneSpec(tNewPlacedScene.tSceneSpec, ptTile->nGroup);
            if (!bValidSceneSpec)
            {
                DRLGTestEvent tEvent(
                    m_dwSeed,
                    m_bWarningsAsErrors ? DRLGTestEvent::STATUS_ERROR : DRLGTestEvent::STATUS_WARNING,
                    CRefString().Format("Unable to place tile.\nCould not find a valid scene spec for group #%d.\nThis will leave a gap in the dungeon!", ptTile->nGroup),
                    ptTile->ptCommand ? ptTile->ptCommand->nCommandType : DRLG_COMMAND_NONE,
                    DRLG_ERROR_RESOLUTION_TILES_OMITTED,
                    ptTile->nTileType,
                    ptTile->nCommandIndex,
                    DRLG_STAGE_POST_GENERATION);

                m_DRLGEvents.push_back(tEvent);







                m_bErrors = TRUE;
                continue;

            }

            tNewPlacedScene.tSceneSpec.dHeight += ptTile->nHeight - nHeightOffsetX - nHeightOffsetY;
            tNewPlacedScene.tSceneSpec.pointGridLocation.x = pointStitchSpaceScene.x;
            tNewPlacedScene.tSceneSpec.pointGridLocation.y = pointStitchSpaceScene.y;
            tNewPlacedScene.tSceneSpec.nDRLGIndex = m_nDRLGIndex++;

            tNewPlacedScene.tSceneSpec.nTileType = ptTile->nTileType;
            tNewPlacedScene.tSceneSpec.snoPrevWorld = ptTile->snoPrevWorld;
            tNewPlacedScene.tSceneSpec.uPrevEntranceGUID = ptTile->uPrevEntranceGUID;
            tNewPlacedScene.tSceneSpec.snoPrevLevelArea = ptTile->snoPrevLevelArea;
            tNewPlacedScene.tSceneSpec.snoNextWorld = ptTile->snoNextWorld;
            tNewPlacedScene.tSceneSpec.uNextEntranceGUID = ptTile->uNextEntranceGUID;
            tNewPlacedScene.tSceneSpec.snoNextLevelArea = ptTile->snoNextLevelArea;
            tNewPlacedScene.tSceneSpec.tDRLGCreationInfo = tDRLGCreationInfo;


            if (ptTile->nTileIndex >= 0)
            {
                ScopedWorldDefinitionLockR pdefWorld(m_snoWorld);


                TileInfo & tTileInfo = pdefWorld->ptServerData->ptDRLG->arTiles[ptTile->nTileIndex];
                tNewPlacedScene.tSceneSpec.uClusterID = tTileInfo.uClusterID;


                if (tTileInfo.nTileType == DRLG_TILE_CUSTOM)
                {
                    int32 nSceneSizeX = tTileInfo.tCustomTileInfo.nSizeX;
                    int32 nSceneSizeY = tTileInfo.tCustomTileInfo.nSizeY;

                    DRLGTileType & eTileType = tNewPlacedScene.tSceneSpec.nTileType;
                    eTileType = DRLG_TILE_NONE;

                    for (int32 ix = 0; ix < nSceneSizeX; ix++)
                    {
                        for (int32 iy = 0; iy < nSceneSizeY; iy++)
                        {

                            uint32 dwExteriorExits = DRLG_TILE_NSEW;
                            if (ix > 0) dwExteriorExits &= ~DRLG_TILE_N;
                            if (ix < nSceneSizeX - 1) dwExteriorExits &= ~DRLG_TILE_S;
                            if (iy < nSceneSizeY - 1) dwExteriorExits &= ~DRLG_TILE_E;
                            if (iy > 0) dwExteriorExits &= ~DRLG_TILE_W;

                            int32 nCustomTileIndex = iy * nSceneSizeX + ix;
                            int32 nCurCustomTileType = tTileInfo.tCustomTileInfo.arTiles[nCustomTileIndex].nTileType;
                            eTileType |= nCurCustomTileType & dwExteriorExits;
                        }
                    }
                }
            }

            tNewPlacedScene.nTileSet = ptTile->nTileSet;
            tNewPlacedScene.nTileType = ptTile->nTileType;

            tNewPlacedScene.wp.Set(
                pointStitchSpaceScene.x * m_nFeetPerTileX,
                pointStitchSpaceScene.y * m_nFeetPerTileY,
                tNewPlacedScene.tSceneSpec.dHeight * m_nFeetPerTileZ);

            tNewPlacedScene.posParentScene = NULL;
            tNewPlacedScene.snoSceneGroup = SNO_NONE;
            tNewPlacedScene.idSScene = SSCENEID_INVALID;
            tNewPlacedScene.snoMarkerSetSrc = SNO_NONE;
            tNewPlacedScene.nMarkerSetSrcIndex = 0;

            MarkerFillPotentialSceneLabels(tNewPlacedScene);
            m_listPlacedScenes.AddTail_Tracked(tNewPlacedScene);
        }
    }
}


void DRLGDungeonGenerator::PlaceBorderTiles(
    DRLGTile * ptTile)
{
    if ((SNONAME_NONE == ptTile->snoName && !ptTile->IsDummy()) || (ptTile->dwFlags & (1 << DRLG_TILE_FLAG_BORDER_TILE)))
    {
        return;
    }



    if (ptTile->nTileIndex < 0)
    {
        return;
    }

    ScopedWorldDefinitionLockR pdefWorld(m_snoWorld);


    TileInfo * ptTileInfo = &pdefWorld->ptServerData->ptDRLG->arTiles[ptTile->nTileIndex];

    int32 tx, ty;
    tx = ptTile->nXCoord; ty = ptTile->nYCoord;


    int32 arTags[8] =
    {
        TAG_WORLD_TILE_BORDER_N,
        TAG_WORLD_TILE_BORDER_NE,
        TAG_WORLD_TILE_BORDER_E,
        TAG_WORLD_TILE_BORDER_SE,
        TAG_WORLD_TILE_BORDER_S,
        TAG_WORLD_TILE_BORDER_SW,
        TAG_WORLD_TILE_BORDER_W,
        TAG_WORLD_TILE_BORDER_NW,
    };

    for (int32 i = 0; i < 8; i++)
    {
        if (TagMapTagExists(ptTileInfo->hTileTagMap, arTags[i]))
        {


            GridDataTileSets eTileSet = TagMapGetInt(ptTileInfo->hTileTagMap, arTags[i]);
            if (eTileSet)
            {
                int32 x, y;

                switch (arTags[i])
                {
                case TAG_WORLD_TILE_BORDER_N:
                    x = tx - 1;
                    y = ty;
                    break;
                case TAG_WORLD_TILE_BORDER_NE:
                    x = tx - 1;
                    y = ty + 1;
                    break;
                case TAG_WORLD_TILE_BORDER_E:
                    x = tx;
                    y = ty + 1;
                    break;
                case TAG_WORLD_TILE_BORDER_SE:
                    x = tx + 1;
                    y = ty + 1;
                    break;
                case TAG_WORLD_TILE_BORDER_S:
                    x = tx + 1;
                    y = ty;
                    break;
                case TAG_WORLD_TILE_BORDER_SW:
                    x = tx + 1;
                    y = ty - 1;
                    break;
                case TAG_WORLD_TILE_BORDER_W:
                    x = tx;
                    y = ty - 1;
                    break;
                case TAG_WORLD_TILE_BORDER_NW:
                    x = tx - 1;
                    y = ty - 1;
                    break;
                default:
                    {
                        CRefString sError;

                        sError.Format("Invalid border direction: %d\n", i);

                        DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
                    }
                }


                DRLGTile * ptTileAtLoc = GetTileAtLoc(x, y);
                if (!ptTileAtLoc)
                {

                    DRLGTile * ptNewTile = AllocTile(ptTile->ptCommand, ptTile, x, y, DRLG_TILE_NONE, eTileSet, ptTile->nHeight, NULL, TRUE);

                    ptNewTile->dwFlags |= (1 << DRLG_TILE_FLAG_BORDER_TILE);
                }
                else if (ptTileAtLoc->dwFlags & (1 << DRLG_TILE_FLAG_BORDER_TILE))
                {


                    if (ptTileAtLoc->nTileSet > eTileSet)
                    {
                        ptTileAtLoc->nTileSet = eTileSet;
                    }
                }
            }
        }
    }
}

BOOL DRLGDungeonGenerator::DoesTilePassChecks(
    const TileInfo & tTileInfo,
    DRLGTile * ptTile,
    DRLGTileType nTileType,
    int32 nTileIndex,
    DRLGTileFilterType eFilterType,
    int32 nFilterParam,
    uint32 dwAdjacencyMask)
{

    if (ptTile->snoName != SNONAME_NONE)
    {
        return FALSE;
    }


    if (ptTile->IsDummy())
    {
        return FALSE;
    }

    if (tTileInfo.nTileType != DRLG_TILE_CUSTOM && ((tTileInfo.nTileType ^ nTileType) & dwAdjacencyMask))
    {
        return FALSE;
    }

    if (ptTile->nTileIndex >= 0 && ptTile->nTileIndex != nTileIndex)
    {
        return FALSE;
    }



    if (dwAdjacencyMask != 9 && dwAdjacencyMask != 0xf && (ptTile->dwFlags & (1 << DRLG_TILE_FLAG_REPLACEMENT_TILE)))
    {
        return FALSE;
    }

    BOOL bPassedAdjacencyCheck = TRUE;
    for (int32 nExitDir = 0; nExitDir < DRLG_EXIT_CNT; nExitDir++)
    {

        if ((dwAdjacencyMask & (1 << nExitDir)) && (ptTile->nCurrentExits & (1 << nExitDir)))
        {
            DRLGTile * ptNeighbor = ptTile->GetNeighbor(nExitDir);
            if (ptNeighbor)
            {




                GridDataTileSets nTransitionTileSet = TagMapGetInt(tTileInfo.hTileTagMap, gTransitionTags[nExitDir]);
                if (nTransitionTileSet != 0 && nTransitionTileSet != ptNeighbor->nTileSet)
                {
                    bPassedAdjacencyCheck = FALSE;
                    break;
                }


                if (m_bUseEdgeTypes)
                {
                    DRLGEdgeType eEdgeType = TagMapGetInt(tTileInfo.hTileTagMap, gEdgeTags[nExitDir]);
                    DRLGEdgeType eCurEdge = ptTile->GetEdgeType(nExitDir);


                    if (eCurEdge == DRLG_EDGE_UNDEFINED && eFilterType != DRLG_TILE_FILTER_EDGE_TYPES_ONLY)
                    {
                        eCurEdge = DRLG_EDGE_DEFAULT;
                    }

                    if ((eFilterType == DRLG_TILE_FILTER_ROAD_START ||
                         eFilterType == DRLG_TILE_FILTER_ROAD_DESTINATION ||
                         eFilterType == DRLG_TILE_FILTER_ROAD_LINK) &&
                        eEdgeType == nFilterParam)
                    {
                        continue;
                    }



                    if (eCurEdge != DRLG_EDGE_UNDEFINED && eCurEdge != eEdgeType)
                    {
                        bPassedAdjacencyCheck = FALSE;
                        break;
                    }


                    if (!(ptTile->arAllowedEdges[nExitDir] & (1 << eEdgeType)))
                    {
                        bPassedAdjacencyCheck = FALSE;
                        break;
                    }
                }
            }
        }
    }






    return bPassedAdjacencyCheck;
}




BOOL sDetermineInvalidSpecialTileRect(IRect2D & tInvalidRect, int32 nTileX, int32 nTileY, const TileInfo & tTileInfo, const TileInfo & tCompareTileInfo)
{
    DRLGSpecialTileType nSpecialTileType;
    int32 nSpecialTileBorder = 0;
    DRLGSpecialTileType nCompareSpecialTileType;


    nSpecialTileType = TagMapGetInt(tTileInfo.hTileTagMap, TAG_WORLD_TILE_SPECIAL_TILE_TYPE);
    nCompareSpecialTileType = TagMapGetInt(tCompareTileInfo.hTileTagMap, TAG_WORLD_TILE_SPECIAL_TILE_TYPE);




    if (nSpecialTileType != DRLG_SPECIAL_TILE_NONE && nCompareSpecialTileType != DRLG_SPECIAL_TILE_NONE)
    {
        nSpecialTileBorder = Math::Max(sGetSpecialTileMinSpacing(&tCompareTileInfo, nSpecialTileType),
                                       sGetSpecialTileMinSpacing(&tTileInfo, nCompareSpecialTileType));
    }
    else if (nSpecialTileType != DRLG_SPECIAL_TILE_NONE)
    {
        nSpecialTileBorder = sGetSpecialTileMinSpacing(&tCompareTileInfo, nSpecialTileType);
    }
    else if (nCompareSpecialTileType != DRLG_SPECIAL_TILE_NONE)
    {
        nSpecialTileBorder = sGetSpecialTileMinSpacing(&tTileInfo, nCompareSpecialTileType);
    }


    if (nSpecialTileBorder <= 0)
        return FALSE;




    int32 nSceneSizeX;
    int32 nSceneSizeY;
    sGetTileDimensions(tTileInfo, nSceneSizeX, nSceneSizeY);

    tInvalidRect.left = nTileX - nSpecialTileBorder;
    tInvalidRect.right = nTileX + nSpecialTileBorder + nSceneSizeX - 1;
    tInvalidRect.top = nTileY - nSpecialTileBorder;
    tInvalidRect.bottom = nTileY + nSpecialTileBorder + nSceneSizeY - 1;
    return TRUE;
}  // NON_MATCHING (next function): 4 of 165 insns; inside the inlined IRect2D::Intersect the original selects the bottom (Min) before the top (Max)
BOOL DRLGDungeonGenerator::DoesTilePassSpecialTileSpacing(
    const WorldDefinition * pdefWorld,
    int32 nXCoord,
    int32 nYCoord,
    TileInfo * ptTileInfo)
{

    int32 nSpecialTileMaxSpacing = sGetSpecialTileMinSpacing(ptTileInfo, DRLG_SPECIAL_TILE_NONE);
    DRLGSpecialTileType nSpecialTileType = TagMapGetInt(ptTileInfo->hTileTagMap, TAG_WORLD_TILE_SPECIAL_TILE_TYPE);

    if (nSpecialTileMaxSpacing <= 0 && nSpecialTileType == DRLG_SPECIAL_TILE_NONE)
    {
        return TRUE;
    }


    blz::vector<IRect2D> arSpecialTileRects;

    size_t nSpecialTilesCount = m_arSpecialTiles.size();
    for (uint32 i = 0; i < nSpecialTilesCount; i++)
    {
        const DRLGTile * ptCurTile = m_arSpecialTiles[i];

        if (ptCurTile->nTileIndex > 0)
        {
            TileInfo & tNeighborTileInfo = pdefWorld->ptServerData->ptDRLG->arTiles[ptCurTile->nTileIndex];

            IRect2D tSpecialTileRect;
            if (sDetermineInvalidSpecialTileRect(tSpecialTileRect, ptCurTile->nXCoord, ptCurTile->nYCoord, tNeighborTileInfo, *ptTileInfo))
            {
                arSpecialTileRects.push_back(tSpecialTileRect);
            }
        }
    }


    size_t nSpecialTileRectsCount = arSpecialTileRects.size();
    if (nSpecialTileRectsCount > 0)
    {




        int32 nSceneSizeX, nSceneSizeY;
        sGetTileDimensions(*ptTileInfo, nSceneSizeX, nSceneSizeY);

        IRect2D tSourceRect;
        tSourceRect.left = nXCoord;
        tSourceRect.right = nXCoord + nSceneSizeX - 1;
        tSourceRect.top = nYCoord;
        tSourceRect.bottom = nYCoord + nSceneSizeY - 1;


        for (uint32 i = 0; i < nSpecialTileRectsCount; i++)
        {
            if (IRect2D::Intersect(arSpecialTileRects[i], tSourceRect, NULL))
            {
                return FALSE;
            }
        }
    }

    return TRUE;
}


BOOL DRLGDungeonGenerator::DoesTilePassValidCustomTileChecks(
    const TileInfo & tTileInfo,
    DRLGTile * ptTile,
    DRLGTileFilterType eFilterType,
    int32 nBaseTileHeight,
    int32 nTilePosX,
    int32 nTilePosY,
    int32 nCustomTileX,
    int32 nCustomTileY,
    DRLGTileType & nOutTileType)
{
    if (tTileInfo.nTileType == DRLG_TILE_CUSTOM &&
        (eFilterType == DRLG_TILE_FILTER_PLACE_NORMAL_TILES || eFilterType == DRLG_TILE_FILTER_PRE_PLACE_TILES))
    {


        int32 nAnchorIndex = tTileInfo.tCustomTileInfo.pointAnchor.y * tTileInfo.tCustomTileInfo.nSizeX + tTileInfo.tCustomTileInfo.pointAnchor.x;
        CustomTileCell & tAnchorCell = tTileInfo.tCustomTileInfo.arTiles[nAnchorIndex];


        int32 nCustomTileIndex = nCustomTileY * tTileInfo.tCustomTileInfo.nSizeX + nCustomTileX;
        CustomTileCell & tCustomCell = tTileInfo.tCustomTileInfo.arTiles[nCustomTileIndex];
        nOutTileType = (DRLGTileType)tCustomCell.nTileType;


        int32 nBaseHeight = nBaseTileHeight - tAnchorCell.nTileHeight;
        if (ptTile)
        {

            if (ptTile->nHeight != nBaseHeight + tCustomCell.nTileHeight)
            {
                return FALSE;
            }


            if (sIsValidTileType(nOutTileType))
            {
                for (int32 d = 0; d < DRLG_EXIT_CNT; d++)
                {
                    DRLGTileType nExitType = (DRLGTileType)(1 << d);


                    if (nOutTileType & nExitType)
                    {
                        if (!(ptTile->nAllowedExits & nExitType))
                        {
                            return FALSE;
                        }
                    }
                    else
                    {

                        if (ptTile->nCurrentExits & nExitType)
                        {
                            return FALSE;
                        }
                    }
                }
            }
        }
        else
        {

            if (!CheckHardConstraintsXY(nTilePosX, nTilePosY, nOutTileType, nBaseHeight + tCustomCell.nTileHeight, tCustomCell.arExitHeights))
            {
                return FALSE;
            }
        }
    }

    return TRUE;
}


BOOL DRLGDungeonGenerator::ShouldReplaceTileForBounty(
    DRLGTileType eTileType,
    GridDataTileSets eTileSet)
{
    if (m_snoBountyRequiredScene != SNO_NONE &&
        !m_bBountyRequiredScenePlaced &&
        m_eBountyRequiredTileType != DRLG_TILE_UNDEFINED &&
        m_eBountyRequiredTileType == eTileType &&
        m_eBountyRequiredTileSet == eTileSet)
    {
        return TRUE;
    }
    return FALSE;
}


// NON_MATCHING: 2036 vs 2020 bytes, 10 hunks, register allocation only: the original keeps ty (ptTile->nYCoord) in w20 and nSceneSizeY in w0 until the iy loop starts; ours spills both where they are defined.
int32 DRLGDungeonGenerator::GetEligibleListForTile(
    const WorldDefinition * pdefWorld,
    DRLGTile * ptTile,
    DRLGTileType nTileType,
    GridDataTileSets nTileSet,
    TListTiles & listTiles,
    DRLGTileFilterType eFilterType,
    int32 nFilterParam)
{
    uint32 dwTotalWeight = 0;


    if (ptTile->snoName != SNONAME_NONE)
    {
        return 0;
    }


    if (ptTile->dwFlags & (1 << DRLG_TILE_FLAG_DEADEND_TILE))
    {
        PickDeadEndCapTile(ptTile, nTileType, listTiles, dwTotalWeight);
    }

    int32 nAlreadyChosen = listTiles.Count();


    if (nAlreadyChosen == 0)
    {
        for (int32 nTileIndex = 0; nTileIndex < pdefWorld->ptServerData->ptDRLG->arTiles.GetArraySize(); nTileIndex++)
        {
            TileInfo & tTileInfo = pdefWorld->ptServerData->ptDRLG->arTiles[nTileIndex];

            if (nTileSet != 0 && tTileInfo.nTileSet != nTileSet)
            {
                continue;
            }


            if ((eFilterType == DRLG_TILE_FILTER_ROAD_START || eFilterType == DRLG_TILE_FILTER_ROAD_DESTINATION) &&
                CountEdgesOfType(tTileInfo, nFilterParam) != 1)
            {
                continue;
            }
            if (eFilterType == DRLG_TILE_FILTER_ROAD_LINK &&
                CountEdgesOfType(tTileInfo, nFilterParam) != 2)
            {
                continue;
            }


            int32 nMaxFrequency = TagMapGetInt(tTileInfo.hTileTagMap, TAG_WORLD_TILE_MAX_FREQUENCY);
            int32 nActualFrequency = 0;
            m_TileFrequencies.Lookup(nTileIndex, nActualFrequency);
            if (nMaxFrequency != -1 && nActualFrequency >= nMaxFrequency)
            {
                continue;
            }

            BOOL bPassedChecks = TRUE;




            int32 tx = ptTile->nXCoord;
            int32 ty = ptTile->nYCoord;

            int32 nSceneSizeX = TagMapGetInt(tTileInfo.hTileTagMap, TAG_WORLD_TILE_SCENE_SIZE_X);
            int32 nSceneSizeY = TagMapGetInt(tTileInfo.hTileTagMap, TAG_WORLD_TILE_SCENE_SIZE_Y);

            if ((eFilterType == DRLG_TILE_FILTER_PRE_PLACE_TILES || eFilterType == DRLG_TILE_FILTER_PLACE_NORMAL_TILES) && tTileInfo.nTileType == DRLG_TILE_CUSTOM)
            {


                tx -= tTileInfo.tCustomTileInfo.pointAnchor.x;
                ty -= tTileInfo.tCustomTileInfo.pointAnchor.y;

                nSceneSizeX = tTileInfo.tCustomTileInfo.nSizeX;
                nSceneSizeY = tTileInfo.tCustomTileInfo.nSizeY;
            }

            for (int32 iy = 0; iy < nSceneSizeY && bPassedChecks; iy++)
            {
                for (int32 ix = 0; ix < nSceneSizeX && bPassedChecks; ix++)
                {
                    DRLGTile * ptTileAtLoc = GetTileAtLoc(tx + ix, ty + iy);


                    uint32 dwAdjacencyMask = DRLG_TILE_NSEW;
                    if (ix != 0) dwAdjacencyMask &= ~DRLG_TILE_N;
                    if (ix != nSceneSizeX - 1) dwAdjacencyMask &= ~DRLG_TILE_S;
                    if (iy != nSceneSizeY - 1) dwAdjacencyMask &= ~DRLG_TILE_E;
                    if (iy != 0) dwAdjacencyMask &= ~DRLG_TILE_W;

                    DRLGTileType nCurTileType = nTileType;
                    if (!DoesTilePassValidCustomTileChecks(tTileInfo, ptTileAtLoc, eFilterType, ptTile->nHeight, tx + ix, ty + iy, ix, iy, nCurTileType))
                    {
                        bPassedChecks = FALSE;
                    }
                    else if (ptTileAtLoc && !DoesTilePassChecks(tTileInfo, ptTileAtLoc, nCurTileType, nTileIndex, eFilterType, nFilterParam, dwAdjacencyMask))
                    {
                        bPassedChecks = FALSE;
                    }
                }
            }

            if (!bPassedChecks)
            {
                continue;
            }


            if (eFilterType == DRLG_TILE_FILTER_ROAD_START || eFilterType == DRLG_TILE_FILTER_ROAD_DESTINATION)
            {

                for (int32 i = 0; i < DRLG_EXIT_CNT; i++)
                {
                    DRLGEdgeType eEdgeType = TagMapGetInt(tTileInfo.hTileTagMap, gEdgeTags[i]);
                    if (eEdgeType == nFilterParam)
                    {
                        ptTile->nFilterFlags |= (1 << (i + ((eFilterType == DRLG_TILE_FILTER_ROAD_START) ? DRLG_PATH_START : DRLG_PATH_DESTINATION)));
                        break;
                    }
                }
            }
            else if (eFilterType == DRLG_TILE_FILTER_ROAD_LINK)
            {

                for (int32 i = 0; i < DRLG_EXIT_CNT - 1; i++)
                {
                    DRLGEdgeType eEdgeType = TagMapGetInt(tTileInfo.hTileTagMap, gEdgeTags[i]);
                    if (eEdgeType == nFilterParam)
                    {
                        for (int32 j = i + 1; j < DRLG_EXIT_CNT; j++)
                        {
                            DRLGEdgeType eEdgeType2 = TagMapGetInt(tTileInfo.hTileTagMap, gEdgeTags[j]);
                            if (eEdgeType2 == nFilterParam)
                            {
                                ptTile->nFilterFlags |= (1 << GetPathLinkFromDirections(i, j));
                                break;
                            }
                        }
                    }
                }
            }

            else if (eFilterType == DRLG_TILE_FILTER_PRE_PLACE_TILES || eFilterType == DRLG_TILE_FILTER_PLACE_NORMAL_TILES)
            {
                if (!DoesTilePassSpecialTileSpacing(pdefWorld, ptTile->nXCoord, ptTile->nYCoord, &tTileInfo))
                {
                    continue;
                }
            }

            uint32 dwWeight = GetModifiedTileWeight(ptTile, tTileInfo, nTileIndex,
                tTileInfo.dwWeight);

            DungeonTileChooser tTileChooser;
            tTileChooser.ptTileInfo = &tTileInfo;
            tTileChooser.dwWeight = dwWeight;
            tTileChooser.nTileIndex = nTileIndex;
            listTiles.AddTail_Tracked(tTileChooser);


            dwTotalWeight += dwWeight;
        }
    }








    return dwTotalWeight;
}


SNOName DRLGDungeonGenerator::PickTile(
    DRLGTile * ptTile,
    DRLGTileType nTileType,
    GridDataTileSets nTileSet,
    SNONameArray & listSNONames)
{
    ScopedWorldDefinitionLockR pdefWorld(m_snoWorld);


    TListTiles listTiles(sDRLGDungeonGetListTilePool());
    uint32 dwTotalWeight;

    dwTotalWeight = GetEligibleListForTile(pdefWorld, ptTile, nTileType, nTileSet, listTiles, DRLG_TILE_FILTER_PLACE_NORMAL_TILES, 0);

    TileInfo * ptTileInfo = NULL;
    int32 nTileIndex = -1;
    SNOName snoResult = SNONAME_NONE;


    if (dwTotalWeight == 0) return snoResult;

    if (ShouldReplaceTileForBounty(nTileType, nTileSet))
    {
        POS pos = listTiles.GetHeadPosition();

        while (pos)
        {
            DungeonTileChooser tTileChooser = listTiles.GetNext(pos);
            if (tTileChooser.ptTileInfo->snoScene == m_snoBountyRequiredScene)
            {
                ptTileInfo = tTileChooser.ptTileInfo;
                m_bBountyRequiredScenePlaced = TRUE;
                break;
            }
        }
    }

    if (!ptTileInfo)
    {
        uint32 dwWeight = PickRand(dwTotalWeight);
        dwTotalWeight = 0;

        POS pos = listTiles.GetHeadPosition();

        while (pos)
        {
            DungeonTileChooser tTileChooser = listTiles.GetNext(pos);

            if (dwWeight < (dwTotalWeight += tTileChooser.dwWeight))
            {
                ptTileInfo = tTileChooser.ptTileInfo;
                nTileIndex = tTileChooser.nTileIndex;
                break;
            }
        }
    }




    if (ptTileInfo)
    {


        ptTile->nTileSet = ptTileInfo->nTileSet;
        ptTile->nTileIndex = nTileIndex;

        ptTile->bIgnoreNavmesh |= TagMapGetInt(ptTileInfo->hTileTagMap, TAG_WORLD_TILE_IGNORE_NAVMESH);


        int32 nActualFrequency = 0;
        m_TileFrequencies.Lookup(nTileIndex, nActualFrequency);
        m_TileFrequencies.SetAt_Tracked(nTileIndex, nActualFrequency + 1);


        int32 nSpecialTileMaxSpacing = sGetSpecialTileMinSpacing(ptTileInfo, DRLG_SPECIAL_TILE_NONE);
        DRLGSpecialTileType nSpecialTileType = TagMapGetInt(ptTileInfo->hTileTagMap, TAG_WORLD_TILE_SPECIAL_TILE_TYPE);
        if (nSpecialTileMaxSpacing > 0 || nSpecialTileType != DRLG_SPECIAL_TILE_NONE)
        {
            m_arSpecialTiles.push_back(ptTile);
        }



        if (ptTileInfo->tCustomTileInfo.eCustomType != CUSTOM_TYPE_MULTI_SCENE)
        {
            snoResult.Set(SCENE_SNO, ptTileInfo->snoScene);
            listSNONames.push_back(snoResult);



            int32 nSceneSizeX = TagMapGetInt(ptTileInfo->hTileTagMap, TAG_WORLD_TILE_SCENE_SIZE_X);
            int32 nSceneSizeY = TagMapGetInt(ptTileInfo->hTileTagMap, TAG_WORLD_TILE_SCENE_SIZE_Y);

            if (nSceneSizeX > 1 || nSceneSizeY > 1)
            {
                int32 tx, ty;
                tx = ptTile->nXCoord; ty = ptTile->nYCoord;


                for (int32 i = 0; i < nSceneSizeY; i++)
                {
                    for (int32 j = 0; j < nSceneSizeX; j++)
                    {
                        DRLGTile * ptTileAtLoc = GetTileAtLoc(tx + j, ty + i);
                        if (!ptTileAtLoc || ptTileAtLoc == ptTile)
                            continue;






                        if (SNONAME_NONE == ptTileAtLoc->snoName)
                        {
                            ptTileAtLoc->nTileSet = GRIDDATA_TILE_SET_DUMMY;
                            ptTileAtLoc->nTileIndex = nTileIndex;
                        }
                    }
                }
            }
        }
        else
        {


            int32 nSizeX = ptTileInfo->tCustomTileInfo.nSizeX;
            int32 nSizeY = ptTileInfo->tCustomTileInfo.nSizeY;

            for (int32 i = 0; i < m_nTileSizeY; i++)
            {
                for (int32 j = 0; j < m_nTileSizeX; j++)
                {




                    if (j >= nSizeX || i >= nSizeY)
                    {
                        listSNONames.push_back(SNONAME_NONE);
                        continue;
                    }

                    int32 nIndex = i * nSizeX + j;
                    CustomTileCell tTileCell = ptTileInfo->tCustomTileInfo.arTiles[nIndex];
                    SNOName snoNameScene(SCENE_SNO, tTileCell.snoScene);

                    listSNONames.push_back(snoNameScene);
                }
            }

            if (!listSNONames.empty())
            {
                snoResult = listSNONames[0];
            }
        }
    }

    return snoResult;
}



SNOName DRLGDungeonGenerator::PickReplacementTile(DRLGTile * ptTile, DRLGTileType nTileType, GridDataTileSets nTileSet, SNONameArray & listSNONames)
{





    SNOName snoResult = SNONAME_NONE;


    if (ptTile->dwFlags & (1 << DRLG_TILE_FLAG_DEADEND_TILE))
        return snoResult;




    size_t nPendingTileReplacementsCount = m_arPendingTileReplacements.size();
    for (uint32 i = 0; i < nPendingTileReplacementsCount; i++)
    {

        if ((m_arPendingTileReplacements[i].nParentCommandIndex < 0 ||
             m_arPendingTileReplacements[i].nParentCommandIndex == ptTile->nCommandIndex) &&




            (m_arPendingTileReplacements[i].nTileLabel == DRLG_LABEL_NONE ||
             ptTile->HasLabel(m_arPendingTileReplacements[i].nTileLabel)) &&
            m_arPendingTileReplacements[i].eReplacementTileSet != nTileSet)
        {





            listSNONames.clear();
            snoResult = PickTile(ptTile, nTileType, m_arPendingTileReplacements[i].eReplacementTileSet, listSNONames);

            if (snoResult != SNONAME_NONE)
            {

                if (m_arPendingTileReplacements[i].nOverrideGroupId >= 0)
                    ptTile->nGroup = m_arPendingTileReplacements[i].nOverrideGroupId;

                ptTile->dwFlags |= (1 << DRLG_TILE_FLAG_REPLACEMENT_TILE);


                ptTile->nCommandIndex = m_arPendingTileReplacements[i].nCommandIndex; ptTile->ptCommand = &m_arCommands[ptTile->nCommandIndex];

                blzext::vector_erase_at(m_arPendingTileReplacements, i);
                return snoResult;
            }
        }
    }
    return snoResult;
}
uint32 DRLGDungeonGenerator::GetModifiedTileWeight(DRLGTile * ptTile, const TileInfo & tTileInfo, uint32 nTileIndex, uint32 dwOriginalWeight)
{



    if (dwOriginalWeight <= 1 || tTileInfo.snoScene == SNO_NONE)
        return dwOriginalWeight;




    int32 nMinFrequency = TagMapGetInt(tTileInfo.hTileTagMap, TAG_WORLD_TILE_MIN_FREQUENCY);
    int32 nActualFrequency = 0;
    m_TileFrequencies.Lookup(nTileIndex, nActualFrequency);
    if (nMinFrequency != -1 && nActualFrequency < nMinFrequency)
        return dwOriginalWeight + 1000000;





    for (int32 nDirection = 0; nDirection < DRLG_EXIT_CNT; nDirection++)
    {
        DRLGTile * ptNeighbor = ptTile->GetNeighbor(nDirection);
        if (ptNeighbor && ptNeighbor->snoName.snoGroup == SCENE_SNO && ptNeighbor->snoName.snoHandle == tTileInfo.snoScene)
            return 1;
    }

    return dwOriginalWeight;
}















DRLGValidChokepointRotateDir DRLGDungeonGenerator::GetValidChokepointRotateDirections(
    int32 nXCoord,
    int32 nYCoord,
    int32 nChokepointLength,
    int32 nChokepointWidth,
    DRLGExitDirection nChokepointDir,
    GridDataTileSets eTileSet)
{

    IRect2D nPotentialChokepointPos[2];
    switch (nChokepointDir)
    {
    case DRLG_EXIT_NORTH:
        {
            int32 nLeft = nYCoord + 1 - nChokepointWidth;
            nPotentialChokepointPos[0].Set(nLeft, nXCoord - nChokepointLength + 1, nYCoord, nXCoord);


            nPotentialChokepointPos[1].Set(nYCoord, nXCoord - nChokepointLength + 1, nYCoord + nChokepointWidth - 1, nXCoord);
        }
        break;

    case DRLG_EXIT_SOUTH:
        {
            int32 nLeft;
            nPotentialChokepointPos[0].Set(nYCoord, nXCoord, nYCoord + nChokepointWidth - 1, nXCoord + nChokepointLength - 1);

            nLeft = nYCoord + 1 - nChokepointWidth;
            nPotentialChokepointPos[1].Set(nLeft, nXCoord, nYCoord, nXCoord + nChokepointLength - 1);
        }
        break;

    case DRLG_EXIT_EAST:
        {
            int32 nTop = nXCoord + 1 - nChokepointWidth;
            nPotentialChokepointPos[0].Set(nYCoord, nTop, nYCoord + nChokepointLength - 1, nXCoord);


            nPotentialChokepointPos[1].Set(nYCoord, nXCoord, nYCoord + nChokepointLength - 1, nXCoord + nChokepointWidth - 1);
        }
        break;

    case DRLG_EXIT_WEST:
        {
            int32 nTop;
            nPotentialChokepointPos[0].Set(nYCoord - nChokepointLength + 1, nXCoord, nYCoord, nXCoord + nChokepointWidth - 1);

            nTop = nXCoord + 1 - nChokepointWidth;
            nPotentialChokepointPos[1].Set(nYCoord - nChokepointLength + 1, nTop, nYCoord, nXCoord);
        }
        break;
    default:
        return DRLG_CHOKEPOINT_ROTATE_DIR_NONE;
    }



    DRLGValidChokepointRotateDir nChokepointAllowedRotation = (nChokepointWidth > 1) ? DRLG_CHOKEPOINT_ROTATE_DIR_BOTH : DRLG_CHOKEPOINT_ROTATE_DIR_LEFT;
    for (int32 c = 0; c < m_nTileConstraintsCnt && nChokepointAllowedRotation != DRLG_CHOKEPOINT_ROTATE_DIR_NONE; c++)
    {
        IRect2D tChokepointRect;
        switch (m_arTileConstraints[c].eChokepointType)
        {
        case DRLG_CHOKEPOINT_NORTH_SOUTH:

            tChokepointRect.top = m_arTileConstraints[c].nChokepointX;
            tChokepointRect.bottom = m_arTileConstraints[c].nChokepointX + m_arTileConstraints[c].nChokepointLength - 1;
            tChokepointRect.left = -m_nCreateId;
            tChokepointRect.right = m_nCreateId;
            break;


        case DRLG_CHOKEPOINT_EAST_WEST:

            tChokepointRect.top = -m_nCreateId;
            tChokepointRect.bottom = m_nCreateId;
            tChokepointRect.left = m_arTileConstraints[c].nChokepointY;
            tChokepointRect.right = m_arTileConstraints[c].nChokepointY + m_arTileConstraints[c].nChokepointLength - 1;
            break;
        default:
            continue;
        }



        if (tChokepointRect.right != tChokepointRect.left)
        {
            for (int32 nTestRotDir = (nChokepointAllowedRotation & DRLG_CHOKEPOINT_ROTATE_DIR_LEFT) ? 0 : 1; nTestRotDir <= (nChokepointAllowedRotation >> 1);
                 nTestRotDir++)
            {

                if (IRect2D::Intersect(tChokepointRect, nPotentialChokepointPos[nTestRotDir], NULL))
                {
                    nChokepointAllowedRotation = (DRLGValidChokepointRotateDir)(nChokepointAllowedRotation & ~(1 << nTestRotDir));
                }
            }
        }
    }



    ScopedWorldDefinitionLockR pdefWorld(m_snoWorld);

    for (int32 nTileIndex = 0;
         nTileIndex < pdefWorld->ptServerData->ptDRLG->arTiles.GetArraySize() && nChokepointAllowedRotation != DRLG_CHOKEPOINT_ROTATE_DIR_NONE;

         nTileIndex++)
    {
        TileInfo & tTileInfo = pdefWorld->ptServerData->ptDRLG->arTiles[nTileIndex];
        if (eTileSet == 0 || tTileInfo.nTileSet == eTileSet)
        {



            for (int32 nTestRotDir = (nChokepointAllowedRotation & DRLG_CHOKEPOINT_ROTATE_DIR_LEFT) ? 0 : 1; nTestRotDir <= (nChokepointAllowedRotation >> 1);
                 nTestRotDir++)
            {

                if (!DoesTilePassSpecialTileSpacing(pdefWorld, nPotentialChokepointPos[nTestRotDir].top, nPotentialChokepointPos[nTestRotDir].left, &tTileInfo))
                {
                    nChokepointAllowedRotation = (DRLGValidChokepointRotateDir)(nChokepointAllowedRotation & ~(1 << nTestRotDir));
                }
            }
        }
    }

    return nChokepointAllowedRotation;
}












BOOL DRLGDungeonGenerator::PushCommand(DRLGCommand * ptCommand, BOOL bPlacingTiles)
{


    if (m_nCommandStackCnt >= MAX_NUM_NESTED_DRLG_COMMANDS)
    {
        int32 nCommandIndex = (int32)(ptCommand - m_arCommands);

        DRLGTestEvent tEvent(
            m_dwSeed,
            DRLGTestEvent::STATUS_WARNING,
            CRefString("Exceeded maximum nested command limit."),
            ptCommand->nCommandType,
            DRLG_ERROR_RESOLUTION_TILES_OMITTED,
            DRLG_TILE_NONE,
            nCommandIndex,
            DRLG_STAGE_GENERATION);

        m_DRLGEvents.push_back(tEvent);





        return FALSE;
    }



    m_arCommandStack[m_nCommandStackCnt] = ptCommand;
    m_nCommandStackCnt++;

    switch (ptCommand->nCommandType)
    {
    case DRLG_COMMAND_SET_GROUP:
        PushGroupCommand(ptCommand, bPlacingTiles);
        break;
    case DRLG_COMMAND_FILL_HUB:
        {
            DRLGTileConstraint tConstraint;
            tConstraint.ptFillHubCommand = GetParentTileCommand();
            PushConstraint(tConstraint);
        }
        break;
    case DRLG_COMMAND_ADD_CHOKEPOINT:
        {
            DRLGTileConstraint tConstraint;
            PushConstraint(tConstraint);
        }
        break;
    }

    return TRUE;
}
void DRLGDungeonGenerator::PopCommand()
{
    DRLGCommand * ptCommand = m_arCommandStack[--m_nCommandStackCnt];
    switch (ptCommand->nCommandType)
    {
    case DRLG_COMMAND_SET_GROUP:
        PopGroupCommand();
        break;
    case DRLG_COMMAND_FILL_HUB:
        PopConstraint();
        break;
    case DRLG_COMMAND_ADD_CHOKEPOINT:
        PopConstraint();
        break;
    }
    m_arCommandStack[m_nCommandStackCnt] = NULL;

    if (m_nCommandStackCnt > 0 && m_arCommandStack[m_nCommandStackCnt - 1]->nCommandType == DRLG_COMMAND_ADD_CHOKEPOINT && m_nTileConstraintsCnt > 0)
    {
        DRLGTileConstraint & tConstraint = m_arTileConstraints[m_nTileConstraintsCnt - 1];
        tConstraint.eChokepointDir = sOppositeDirection(tConstraint.eChokepointDir);
    }
}












void DRLGDungeonGenerator::PushCommandTree(int32 nCommandIndex)
{

    if (m_arCommandParentIndices[nCommandIndex] != -1)
        PushCommandTree(m_arCommandParentIndices[nCommandIndex]);
    PushCommand(&m_arCommands[nCommandIndex], TRUE);
}







void DRLGDungeonGenerator::PopCommandTree(int32 nCommandIndex)
{

    if (m_arCommandParentIndices[nCommandIndex] != -1)
        PopCommandTree(m_arCommandParentIndices[nCommandIndex]);
    PopCommand();
}





















DRLGCommand * DRLGDungeonGenerator::GetParentTileCommand()
{
    DRLGCommand * ptParentTileCommand = NULL;





    for (int32 i = m_nCommandStackCnt - 2; i >= 0; i--)
    {
        switch (m_arCommandStack[i]->nCommandType)
        {
        case DRLG_COMMAND_ADD_ROOMS:
        case DRLG_COMMAND_ADD_ENTRANCE:
        case DRLG_COMMAND_ADD_EXIT:
        case DRLG_COMMAND_ADD_HUB:
        case DRLG_COMMAND_ADD_SPOKE:
        case DRLG_COMMAND_ADD_ENTRANCE_EXIT:
        case DRLG_COMMAND_ADD_ALL_TILES:
        case DRLG_COMMAND_ADD_CUSTOM_TILE:
        case DRLG_COMMAND_ADD_PERIMETER:
        case DRLG_COMMAND_ADD_FIELDS:
        case DRLG_COMMAND_ADD_ROAD:
        case DRLG_COMMAND_ADD_CHOKEPOINT:
        case DRLG_COMMAND_REPLACE_ENTRANCE_EXIT:
        case DRLG_COMMAND_REPLACE_RANDOM_TILE:
        case DRLG_COMMAND_ADD_NUBS:
        case DRLG_COMMAND_ADD_PERIMETER_NUBS:
            ptParentTileCommand = m_arCommandStack[i];
            break;
        }
        if (ptParentTileCommand)
            break;
    }
    return ptParentTileCommand;
}






void DRLGDungeonGenerator::PushConstraint(const DRLGTileConstraint & tConstraint)
{




    m_arTileConstraints[m_nTileConstraintsCnt] = tConstraint;

    m_nTileConstraintsCnt++;
}

void DRLGDungeonGenerator::PopConstraint()
{
    m_nTileConstraintsCnt--;
}
















void DRLGDungeonGenerator::PushLabel(
    DRLGLabelType nLabel)
{





    if (nLabel < DRLG_LABEL_ALPHA || nLabel >= DRLG_LABEL_NUM_TYPES)
        return;
    m_arLabelStack[m_nLabelStackCnt] = nLabel;

    m_nLabelStackCnt++;
}


void DRLGDungeonGenerator::PopLabel()
{



    m_nLabelStackCnt--;
}













void DRLGDungeonGenerator::AddSceneSpec(
    SceneSpecification & tSceneSpec)
{
    int32 nGroupId = GetGroupId();


    m_arSceneSpecMap.SetAt_Tracked(nGroupId, tSceneSpec);
}


BOOL DRLGDungeonGenerator::GetSceneSpec(SceneSpecification & tSceneSpec, int32 nGroupId)
{




    if (nGroupId < 0)
        nGroupId = GetGroupId();



    if (nGroupId < 0)
        return GetDefaultSceneSpec(tSceneSpec);


    return m_arSceneSpecMap.Lookup(nGroupId, tSceneSpec);
}

BOOL DRLGDungeonGenerator::GetDefaultSceneSpec(SceneSpecification & tSceneSpec)
{


    tSceneSpec = m_tDefaultSceneSpec;
    return TRUE;
}






void DRLGDungeonGenerator::PushGroupId(
    int32 nGroupId)
{

    m_arGroupIdStack[m_nGroupIdStackCnt] = nGroupId;
    m_nGroupIdStackCnt++;
}




void DRLGDungeonGenerator::PopGroupId()
{
    m_nGroupIdStackCnt--;
}





int32 DRLGDungeonGenerator::GetGroupId()
{
    int32 nGroupId = -1;
    if (m_nGroupIdStackCnt > 0)
        nGroupId = m_arGroupIdStack[m_nGroupIdStackCnt - 1];
    return nGroupId;
}











void DRLGDungeonGenerator::PushGroupCommand(
    DRLGCommand * ptCommand,
    BOOL bPlacingTiles)
{


    SceneSpecification tSceneSpec;
    BOOL bValidSceneSpec = GetSceneSpec(tSceneSpec, -1);



    int32 nGroupId = TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_GROUP_NUM);

    SNO snoLevelArea = TagMapGetSNO(ptCommand->hCommandTagMap, TAG_WORLD_LEVEL_AREA);
    if (snoLevelArea != SNO_NONE)
    {
        tSceneSpec.arSnoLevelAreas[0] = snoLevelArea;
    }
    snoLevelArea = TagMapGetSNO(ptCommand->hCommandTagMap, TAG_WORLD_LEVEL_AREA_ALT1);
    if (snoLevelArea != SNO_NONE)
    {
        tSceneSpec.arSnoLevelAreas[1] = snoLevelArea;
    }
    snoLevelArea = TagMapGetSNO(ptCommand->hCommandTagMap, TAG_WORLD_LEVEL_AREA_ALT2);
    if (snoLevelArea != SNO_NONE)
    {
        tSceneSpec.arSnoLevelAreas[2] = snoLevelArea;
    }
    snoLevelArea = TagMapGetSNO(ptCommand->hCommandTagMap, TAG_WORLD_LEVEL_AREA_ALT3);
    if (snoLevelArea != SNO_NONE)
    {
        tSceneSpec.arSnoLevelAreas[3] = snoLevelArea;
    }

    SWorld * ptSWorld = NULL;
    if (IsDungeonFinder() && GameActiveGameCommonDataIsServer())
    {
        ptSWorld = SWorldGet(m_idSWorld);
    }

    if (!IsDungeonFinder() || !GameActiveGameCommonDataIsServer() || (ptSWorld && ptSWorld->tWCD.snoDungeonFinderSourceWorld == SNO_NONE))
    {
        SNO snoPrevWorld = TagMapGetSNO(ptCommand->hCommandTagMap, TAG_WORLD_PREV_WORLD_SNO);
        if (snoPrevWorld != SNO_NONE)
        {
            tSceneSpec.snoPrevWorld = snoPrevWorld;
            tSceneSpec.uPrevEntranceGUID = TagMapGetDWORD(ptCommand->hCommandTagMap, TAG_WORLD_PREV_ENTRANCE);
            tSceneSpec.snoPrevLevelArea = TagMapGetSNO(ptCommand->hCommandTagMap, TAG_WORLD_PREV_LEVEL_AREA);
        }

        SNO snoNextWorld = TagMapGetSNO(ptCommand->hCommandTagMap, TAG_WORLD_NEXT_WORLD_SNO);
        if (snoNextWorld != SNO_NONE)
        {
            tSceneSpec.snoNextWorld = snoNextWorld;
            tSceneSpec.uNextEntranceGUID = TagMapGetDWORD(ptCommand->hCommandTagMap, TAG_WORLD_NEXT_ENTRANCE);
            tSceneSpec.snoNextLevelArea = TagMapGetSNO(ptCommand->hCommandTagMap, TAG_WORLD_NEXT_LEVEL_AREA);
        }
    }
    else
    {



        static const GlobalSNO arWorlds[10] =
        {
            GLOBALSNO_WORLD_DUNGEON_FINDER_LEVEL_01,
            GLOBALSNO_WORLD_DUNGEON_FINDER_LEVEL_02,
            GLOBALSNO_WORLD_DUNGEON_FINDER_LEVEL_03,
            GLOBALSNO_WORLD_DUNGEON_FINDER_LEVEL_04,
            GLOBALSNO_WORLD_DUNGEON_FINDER_LEVEL_05,
            GLOBALSNO_WORLD_DUNGEON_FINDER_LEVEL_06,
            GLOBALSNO_WORLD_DUNGEON_FINDER_LEVEL_07,
            GLOBALSNO_WORLD_DUNGEON_FINDER_LEVEL_08,
            GLOBALSNO_WORLD_DUNGEON_FINDER_LEVEL_09,
            GLOBALSNO_WORLD_DUNGEON_FINDER_LEVEL_10,
        };

        static const GlobalSNO arLevelAreas[10] =
        {
            GLOBALSNO_LEVELAREA_DUNGEON_FINDER_LEVEL_01,
            GLOBALSNO_LEVELAREA_DUNGEON_FINDER_LEVEL_02,
            GLOBALSNO_LEVELAREA_DUNGEON_FINDER_LEVEL_03,
            GLOBALSNO_LEVELAREA_DUNGEON_FINDER_LEVEL_04,
            GLOBALSNO_LEVELAREA_DUNGEON_FINDER_LEVEL_05,
            GLOBALSNO_LEVELAREA_DUNGEON_FINDER_LEVEL_06,
            GLOBALSNO_LEVELAREA_DUNGEON_FINDER_LEVEL_07,
            GLOBALSNO_LEVELAREA_DUNGEON_FINDER_LEVEL_08,
            GLOBALSNO_LEVELAREA_DUNGEON_FINDER_LEVEL_09,
            GLOBALSNO_LEVELAREA_DUNGEON_FINDER_LEVEL_10,
        };



        int32 nCurrentLevel = WorldGetDungeonFinderLevel(ptSWorld->tWCD.snoWorld);


        ScopedGlobalDefinitionLockR pdefGlobal;


        int32 nMaxLevel = SWorldGetNumDungeonFinderLevels();

        if (nCurrentLevel == 1)
        {

            tSceneSpec.snoPrevWorld = ActGetCurrentHubWorldSNO();
            tSceneSpec.snoPrevLevelArea = LevelAreaFirstInWorld(SWorldFindBySNO(tSceneSpec.snoPrevWorld));
            tSceneSpec.uPrevEntranceGUID = StartingLocationNameToGUID("DungeonFinder_HubEntrance");
        }
        else
        {

            tSceneSpec.snoPrevWorld = GlobalSNOGet(arWorlds[nCurrentLevel - 2]);
            tSceneSpec.snoPrevLevelArea = GlobalSNOGet(arLevelAreas[nCurrentLevel - 2]);
            tSceneSpec.uPrevEntranceGUID = TagMapGetDWORD(ptCommand->hCommandTagMap, TAG_WORLD_PREV_ENTRANCE);
        }


        if (nCurrentLevel < nMaxLevel)
        {
            tSceneSpec.snoNextWorld = GlobalSNOGet(arWorlds[nCurrentLevel]);
            tSceneSpec.snoNextLevelArea = GlobalSNOGet(arLevelAreas[nCurrentLevel]);
            tSceneSpec.uNextEntranceGUID = TagMapGetDWORD(ptCommand->hCommandTagMap, TAG_WORLD_NEXT_ENTRANCE);
        }
        else
        {







            tSceneSpec.snoNextWorld = ActGetCurrentHubWorldSNO();
            tSceneSpec.snoNextLevelArea = LevelAreaFirstInWorld(SWorldFindBySNO(tSceneSpec.snoNextWorld));
            tSceneSpec.uNextEntranceGUID = StartingLocationNameToGUID("DungeonFinder_HubEntrance");
        }
    }






    SNO snoMusic = TagMapGetSNO(ptCommand->hCommandTagMap, TAG_WORLD_MUSIC);
    if (snoMusic != SNO_NONE)
    {
        tSceneSpec.snoMusic = snoMusic;
    }

    SNO snoAmbient = TagMapGetSNO(ptCommand->hCommandTagMap, TAG_WORLD_AMBIENT);
    if (snoAmbient != SNO_NONE)
    {
        tSceneSpec.snoAmbient = snoAmbient;
    }

    SNO snoWeather = TagMapGetSNO(ptCommand->hCommandTagMap, TAG_WORLD_WEATHER);

    if (m_snoDungeonFinderWeather != SNO_NONE)
    {
        snoWeather = m_snoDungeonFinderWeather;
    }

    if (snoWeather != SNO_NONE)
    {
        tSceneSpec.snoWeather = snoWeather;
    }
















    PushGroupId(nGroupId);
    AddSceneSpec(tSceneSpec);
}


void DRLGDungeonGenerator::PopGroupCommand()
{
    PopGroupId();
}







DRLGTile * DRLGDungeonGenerator::AllocTile(
    DRLGCommand * ptCommand,
    DRLGTile * ptParentTile,
    int32 nX,
    int32 nY,
    DRLGTileType nTileType,
    GridDataTileSets nTileSet,
    int32 nHeight,
    int32 * arExitHeights,
    BOOL bBorder)
{
    DRLGTile tTile;





    PickTransitionTile(nX, nY, ptParentTile, nTileType, nTileSet);

    BOOL bValidTileType = sIsValidTileType(nTileType);
    int32 nAllowedExits = bValidTileType ? nTileType : DRLG_TILE_NSEW;
    int32 nMaxExits = 4;

    if (!bBorder)
    {
        if (TagMapTagExists(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_MAX_EXIT_CNT))
        {
            int32 nExitsMin, nExitsDelta;

            nExitsMin = TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_MAX_EXIT_CNT);
            nExitsDelta = TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_MAX_EXIT_CNT_DELTA);
            nMaxExits = nExitsMin + PickRandRange(0, nExitsDelta);
        }

        if (!bValidTileType && TagMapTagExists(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_ALLOWABLE_EXITS))
        {
            nAllowedExits = TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_ALLOWABLE_EXITS);
        }
    }

    int32 nOverrideGroupId = TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_OVERRIDE_GROUP_NUM);

    tTile.dwFlags = 0;
    tTile.nTileType = nTileType;
    tTile.nTileSet = nTileSet;
    tTile.snoName = SNONAME_NONE;
    tTile.nTileIndex = -1;
    for (int32 i = 0; i < DRLG_EXIT_CNT; i++)
    {
        tTile.arNeighbors[i] = NULL;
        tTile.arEdges[i] = DRLG_EDGE_UNDEFINED;
        tTile.arAllowedEdges[i] = 0x1f;
    }
    tTile.ptParentTile = ptParentTile;
    tTile.ptCommand = ptCommand;
    tTile.nCommandIndex = ptCommand - m_arCommands;
    tTile.nGroup = (nOverrideGroupId >= 0) ? nOverrideGroupId : GetGroupId();
    tTile.nCreateId = m_nCreateId;
    tTile.nFilterFlags = 0;
    tTile.nDistance = -2;
    tTile.nComputedPaths = 0;
    tTile.nMaxExits = nMaxExits;
    tTile.nAllowedExits = nAllowedExits;
    tTile.nCurrentExits = 0;
    tTile.nXCoord = nX;
    tTile.nYCoord = nY;

    SceneSpecification tSceneSpec;
    BOOL bValidSceneSpec = GetSceneSpec(tSceneSpec, -1);



    tTile.snoPrevWorld = tSceneSpec.snoPrevWorld;
    tTile.uPrevEntranceGUID = tSceneSpec.uPrevEntranceGUID;
    tTile.snoPrevLevelArea = tSceneSpec.snoPrevLevelArea;
    tTile.snoNextWorld = tSceneSpec.snoNextWorld;
    tTile.uNextEntranceGUID = tSceneSpec.uNextEntranceGUID;
    tTile.snoNextLevelArea = tSceneSpec.snoNextLevelArea;
    tTile.bIgnoreNavmesh = FALSE;

    tTile.nHeight = (nHeight != DRLG_INVALID_EXIT_HEIGHT) ? nHeight : 0;








    if (arExitHeights)
    {
        for (int32 i = 0; i < DRLG_EXIT_CNT; i++)
        {
            tTile.arRelativeExitHeights[i] = arExitHeights[i];
        }
    }
    else
    {
        for (int32 i = 0; i < DRLG_EXIT_CNT; i++)
        {
            tTile.arRelativeExitHeights[i] = 0;
        }
    }

    for (int32 i = 0; i < 4; i++)
    {
        if (i < m_nLabelStackCnt)
        {
            tTile.arLabels[i] = m_arLabelStack[i];
        }
        else
        {
            tTile.arLabels[i] = DRLG_LABEL_NONE;
        }
    }

    m_nCreateId++;
    m_nCommandTilesCount++;

    POS pos = m_listTiles.AddTail_Tracked(tTile);

    DRLGTile * ptChildTile = &m_listTiles.GetAt(pos);

    for (int32 i = 0; i < DRLG_EXIT_CNT; i++)
    {
        DRLGExitDirection nDir = i;
        DRLGTile * ptTile = GetTileAtLoc(ptChildTile, nDir);
        if (ptTile)
        {

            DRLGExitDirection nOppositeDir = nDir ^ 1;


            ptChildTile->arNeighbors[nDir] = ptTile;

            ptTile->arNeighbors[nOppositeDir] = ptChildTile;


            if (ptTile == ptParentTile && nHeight == DRLG_INVALID_EXIT_HEIGHT)
            {
                ptChildTile->nHeight = ptParentTile->GetExitHeight(nOppositeDir);
            }

            ptChildTile->arAllowedEdges[nDir] = ptTile->arAllowedEdges[nOppositeDir];
            ptChildTile->arEdges[nDir] = ptTile->arEdges[nOppositeDir];


            if (m_nCommandTilesCount == 1 && ptTile == ptParentTile)
            {
                DRLGEdgeType eParentEdgeType = TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_PARENT_EDGE);
                if (eParentEdgeType != DRLG_EDGE_UNDEFINED)
                {
                    SetEdgeType(ptChildTile, nDir, eParentEdgeType, FALSE);
                }
            }
        }
    }

    if (bValidTileType || (!bBorder && TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_CONNECT_NEIGHBORS)))
    {

        for (int32 i = 0; i < DRLG_EXIT_CNT; i++)
        {
            if (ptChildTile->CanAddExit(i, TRUE))
            {
                DRLGTile * ptTile = ptChildTile->arNeighbors[i];

                ConnectTiles(ptChildTile, ptTile, i, DRLG_EDGE_UNDEFINED);
            }
        }
    }

    return ptChildTile;
}






BOOL DRLGDungeonGenerator::CheckHardConstraintsXY(
    int32 nTileX,
    int32 nTileY,
    DRLGTileType nTileType,
    int32 nHeight,
    int32 * arExitHeights)
{
    BOOL bPasses = TRUE;



    if (sIsValidTileType(nTileType))
    {
        int32 tx = nTileX, ty = nTileY;




        for (int32 i = 0; i < DRLG_EXIT_CNT; i++)
        {
            int32 x, y;

            x = tx + gAddExitX[i];
            y = ty + gAddExitY[i];

            DRLGTile * ptTile = GetTileAtLoc(x, y);

            if (ptTile)
            {
                DRLGExitDirection nDir = i;

                if (nTileType & sExitDirectionToTileType(nDir))
                {
                    int32 nExitHeight = nHeight;

                    if (arExitHeights)
                    {
                        nExitHeight += arExitHeights[nDir];
                    }






                    if (!ptTile->CanAddTile(sOppositeDirection(nDir), nExitHeight))
                    {
                        bPasses = FALSE;
                        break;
                    }
                }
                else if (ptTile->IsExitRequired(sOppositeDirection(nDir)))
                {
                    bPasses = FALSE;
                    break;
                }
            }
        }
    }
    else
    {


        int32 tx = nTileX, ty = nTileY;




        for (int32 i = 0; i < DRLG_EXIT_CNT; i++)
        {
            int32 x, y;

            x = tx + gAddExitX[i];
            y = ty + gAddExitY[i];

            DRLGTile * ptTile = GetTileAtLoc(x, y);

            if (ptTile && sIsValidTileType(ptTile->nTileType) && (ptTile->nTileType & sExitDirectionToOppositeTileType(i)))
            {
                DRLGExitDirection nDir = i;



                int32 nExitHeight = nHeight;

                if (arExitHeights)
                {
                    nExitHeight += arExitHeights[nDir];
                }





                if (!ptTile->CanAddTile(sOppositeDirection(nDir), nExitHeight))
                {

                    bPasses = FALSE;
                    break;
                }
            }
        }
    }
    return bPasses;
}



BOOL DRLGDungeonGenerator::CheckHardConstraints(const DRLGTile * ptTile, DRLGExitDirection nDirection, DRLGTileType nTileType)
{
    BOOL bPasses = FALSE;


    int32 nHeight = ptTile->GetExitHeight(nDirection);
    if (ptTile->CanAddTile(nDirection, nHeight))
    {
        bPasses = CheckHardConstraintsXY(ptTile->nXCoord + gAddExitX[nDirection], ptTile->nYCoord + gAddExitY[nDirection], nTileType, nHeight, NULL);
    }
    return bPasses;
}





static BOOL CheckForSpokeFreeSpace(DRLGDungeonGenerator * ptDRLG, int32 x, int32 y, int32 nNumRooms,
    DRLGTileType nAllowedExits, DRLGCoordArray & aReserved)
{



    if (ptDRLG->GetTileAtLoc(x, y))
        return FALSE;


    DRLGCoord coord(x, y);

    if (blz::find(aReserved.begin(), aReserved.end(), coord) != aReserved.end())
        return FALSE;


    if (nNumRooms > 1)
    {


        aReserved.push_back(coord);
        int32 allowedDirections = nAllowedExits;
        for (int dir = 0; dir < DRLG_EXIT_CNT; dir++)
        {
            if (allowedDirections & (1 << dir))
            {
                if (CheckForSpokeFreeSpace(ptDRLG, x + gAddExitX[dir], y + gAddExitY[dir], nNumRooms - 1, nAllowedExits, aReserved))
                    return TRUE;
            }
        }


        aReserved.pop_back();
        return FALSE;
    }
    return TRUE; }
// NON_MATCHING: 3168 vs 3124 bytes. (1) the inlined blz::vector<TileCoords>::push_back: the original computes m_size + 1 once before the m_size == m_capacity test and grows with csel; ours computes it in both arms (csinc). (2) the hub block (3866) is laid out before the custom-tile block (3939); the original has them the other way round. Everything else is structurally identical.
BOOL DRLGDungeonGenerator::CheckSoftConstraints(
    const DRLGTile * ptTile,
    DRLGExitDirection nDirection,
    BOOL bFilterForError,
    int32 nTileX,
    int32 nTileY)
{
    DRLGTileConstraint * ptConstraint = m_arTileConstraints;

    if (ptTile)
    {
        nTileX = ptTile->nXCoord;
        nTileY = ptTile->nYCoord;
    }


    for (int32 i = 0; i < m_nTileConstraintsCnt; i++, ptConstraint++)
    {
        if (ptTile && ptConstraint->nGroupNum > 0 && ptConstraint->nGroupNum != ptTile->nGroup)
        {
            return FALSE;
        }

        if (ptTile && !bFilterForError && ptConstraint->nMaxExitCnt > 0 && ptConstraint->nMaxExitCnt <= ptTile->GetCurrentExitsCnt())
        {
            return FALSE;
        }

        if (ptTile && ptConstraint->nRoomCreateNum > 0 &&
            GetCreateId() - ptConstraint->nRoomCreateNum > ptTile->nCreateId)
        {
            return FALSE;
        }

        if (ptTile && ptConstraint->nRoomCreateId >= 0 &&
            ptTile->nCreateId != ptConstraint->nRoomCreateId)
        {
            return FALSE;
        }

        if (ptTile && ptConstraint->nFilterFlag > 0 &&
            ptConstraint->nFilterFlag != ptTile->nFilterFlags)
        {
            return FALSE;
        }

        if (ptTile && ptConstraint->ptFilterCommand &&
            ptConstraint->ptFilterCommand != ptTile->ptCommand)
        {
            return FALSE;
        }

        if (ptTile && ptConstraint->ptFillHubCommand &&
            ptConstraint->ptFillHubCommand->nCommandType == DRLG_COMMAND_ADD_HUB &&
            ptConstraint->ptFillHubCommand == ptTile->ptCommand)
        {


            int32 hubX = nTileX;
            int32 hubY = nTileY;
            int32 nextX = hubX + gAddExitX[nDirection];
            int32 nextY = hubY + gAddExitY[nDirection];

            DRLGTile * ptHubTile = GetTileAtLoc(hubX, hubY);
            DRLGTile * ptNextTile = NULL;




            DRLGExitDirection nRightDir = sRotateExitDirectionRight(nDirection);
            while (ptHubTile && ptHubTile->ptCommand == ptConstraint->ptFillHubCommand)
            {
                ptNextTile = GetTileAtLoc(nextX, nextY);
                if (ptNextTile && ptNextTile->ptCommand == ptConstraint->ptFillHubCommand)
                {
                    break;
                }


                hubX += gAddExitX[nRightDir];
                hubY += gAddExitY[nRightDir];
                nextX = hubX + gAddExitX[nDirection];
                nextY = hubY + gAddExitY[nDirection];

                ptHubTile = GetTileAtLoc(hubX, hubY);
            }


            if (!ptHubTile || ptHubTile->ptCommand != ptConstraint->ptFillHubCommand)
            {
                return FALSE;
            }
        }




        if (!bFilterForError && ptConstraint->eChokepointType != DRLG_CHOKEPOINT_NONE)
        {

            if (ptTile && ptConstraint->eChokepointDir != DRLG_EXIT_NONE &&
                ptTile->ptCommand == ptConstraint->ptOwnerCommand &&
                ptTile->ptCommand->nCommandType == DRLG_COMMAND_ADD_CHOKEPOINT && ptConstraint->eChokepointDir != nDirection)
            {
                return FALSE;
            }


            int32 tx = nTileX + gAddExitX[nDirection];
            int32 ty = nTileY + gAddExitY[nDirection];


            if (ptConstraint->eChokepointDir != DRLG_EXIT_NONE)
            {

                blz::vector<TileCoords> arRemainingAttachments;
                for (int32 j = 0; j < ptConstraint->nChokepointWidth; j++)
                {
                    int32 txAttach = tx, tyAttach = ty;
                    switch (ptConstraint->eChokepointDir)
                    {
                    case DRLG_EXIT_NORTH:
                        {
                            txAttach = ptConstraint->nChokepointX - 1;
                            tyAttach = ptConstraint->nChokepointY + j;
                        }
                        break;

                    case DRLG_EXIT_SOUTH:
                        {
                            txAttach = ptConstraint->nChokepointX + ptConstraint->nChokepointLength;
                            tyAttach = ptConstraint->nChokepointY + j;
                        }
                        break;

                    case DRLG_EXIT_EAST:
                        {
                            txAttach = ptConstraint->nChokepointX + j;
                            tyAttach = ptConstraint->nChokepointY + ptConstraint->nChokepointLength;
                        }
                        break;

                    case DRLG_EXIT_WEST:
                        {
                            txAttach = ptConstraint->nChokepointX + j;
                            tyAttach = ptConstraint->nChokepointY - 1;
                        }
                        break;
                    }

                    if (GetTileAtLoc(txAttach, tyAttach) == NULL)
                    {
                        TileCoords tCoord;
                        tCoord.x = txAttach;
                        tCoord.y = tyAttach;
                        arRemainingAttachments.push_back(tCoord);
                    }
                }


                size_t nRemainingAttachmentsCount = arRemainingAttachments.size();
                if (nRemainingAttachmentsCount > 0)
                {
                    uint32 nAttachIndex;
                    for (nAttachIndex = 0; nAttachIndex < nRemainingAttachmentsCount; nAttachIndex++)
                    {
                        if (tx == arRemainingAttachments[nAttachIndex].x &&
                            ty == arRemainingAttachments[nAttachIndex].y)
                        {
                            break;
                        }
                    }

                    if (nAttachIndex >= nRemainingAttachmentsCount)
                    {
                        return FALSE;
                    }
                }
            }

            if (ptConstraint->eChokepointType == DRLG_CHOKEPOINT_PLACEMENT)
            {




















                POS pos = m_listTiles.GetHeadPosition();
                while (pos)
                {
                    DRLGTile & tTile = m_listTiles.GetNext(pos);

                    switch (nDirection)
                    {
                    case DRLG_EXIT_NORTH:
                        {
                            if (tTile.nXCoord <= tx && tTile.nXCoord > tx - ptConstraint->nChokepointLength)
                            {
                                return FALSE;
                            }
                        }
                        break;

                    case DRLG_EXIT_SOUTH:
                        {
                            if (tTile.nXCoord >= tx && tTile.nXCoord < tx + ptConstraint->nChokepointLength)
                            {
                                return FALSE;
                            }
                        }
                        break;

                    case DRLG_EXIT_EAST:
                        {
                            if (tTile.nYCoord >= ty && tTile.nYCoord < ty + ptConstraint->nChokepointLength)
                            {
                                return FALSE;
                            }
                        }
                        break;

                    case DRLG_EXIT_WEST:
                        {
                            if (tTile.nYCoord <= ty && tTile.nYCoord > ty - ptConstraint->nChokepointLength)
                            {
                                return FALSE;
                            }
                        }
                        break;
                    }
                }


                GridDataTileSets eTileSet = TagMapGetInt(m_arCommandStack[m_nCommandStackCnt - 1]->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_TILE_SET);
                if (GetValidChokepointRotateDirections(tx, ty, ptConstraint->nChokepointLength, ptConstraint->nChokepointWidth, nDirection, eTileSet) == DRLG_CHOKEPOINT_ROTATE_DIR_NONE)
                {
                    return FALSE;
                }
            }
            else if (!sValidForChokepointExcludeDist(ptConstraint, tx, ty))
            {
                return FALSE;
            }
        }

        if (!bFilterForError && ptConstraint->nNumSpokeRooms > 1 &&
            ptConstraint->nAllowedSpokeDir)
        {

            int32 tx = nTileX + gAddExitX[nDirection];
            int32 ty = nTileY + gAddExitY[nDirection];

            DRLGCoordArray aReserved;
            aReserved.reserve(ptConstraint->nNumSpokeRooms - 1);






            if (!CheckForSpokeFreeSpace(this, tx, ty, ptConstraint->nNumSpokeRooms, ptConstraint->nAllowedSpokeDir, aReserved))
                return FALSE;
        }

        if (!bFilterForError && ptConstraint->uSpokeTileTypeFlags)
        {

            int32 nResultingTileType = ptTile->nCurrentExits |
                (1 << nDirection);
            if (ptConstraint->uSpokeTileTypeFlags & (1 << nResultingTileType))
            {
                return FALSE;
            }


            if (ptConstraint->nNumSpokeRooms > 1)
            {
                int32 nCheckDir;
                DRLGExitDirection nOppositeDirection = sOppositeDirection(nDirection);







                for (nCheckDir = 0; nCheckDir < DRLG_EXIT_CNT; nCheckDir++)
                {
                    if (nCheckDir == nDirection) continue;


                    int32 nTestTileType = (1 << nOppositeDirection) | (1 << nCheckDir);
                    if (!(ptConstraint->uSpokeTileTypeFlags & (1 << nTestTileType)))
                    {
                        break;
                    }
                }

                if (nCheckDir == DRLG_EXIT_CNT)
                {
                    return FALSE;
                }
            }
        }

        if (!bFilterForError && (ptConstraint->nHubLength > 1 || ptConstraint->nHubWidth > 1))
        {

            BOOL bPlacementOK = TRUE;
            int32 nLength = ptConstraint->nHubLength;
            int32 nWidth = ptConstraint->nHubWidth;
            DRLGExitDirection nRotatedDirection = sRotateExitDirectionLeft(nDirection);


            int32 tx = nTileX + gAddExitX[nDirection];
            int32 ty = nTileY + gAddExitY[nDirection];


            int32 nColumns = nWidth * 2 - 1;
            int32 nFreeColumns = 0;


            int32 columnX = tx - gAddExitX[nRotatedDirection] * (nWidth - 1);
            int32 columnY = ty - gAddExitY[nRotatedDirection] * (nWidth - 1);

            for (int32 j = 0; j < nColumns; j++)
            {

                int32 tileX = columnX;
                int32 tileY = columnY;



                for (int32 k = 0; k < nLength; k++)
                {
                    if (GetTileAtLoc(tileX, tileY))
                    {
                        bPlacementOK = FALSE;
                        break;
                    }

                    tileX += gAddExitX[nDirection];
                    tileY += gAddExitY[nDirection];
                }

                if (bPlacementOK)
                {


                    if (++nFreeColumns == nWidth)
                    {
                        break;
                    }
                }
                else
                {

                    nFreeColumns = 0;


                    if (nColumns - j <= nWidth)
                    {
                        break;
                    }

                    bPlacementOK = TRUE;
                }

                columnX += gAddExitX[nRotatedDirection];
                columnY += gAddExitY[nRotatedDirection];
            }

            if (!bPlacementOK)
            {
                return FALSE;
            }
        }

        if (!bFilterForError && ptConstraint->ptTileInfo)
        {
            CustomTileInfo * ptTileInfo = ptConstraint->ptTileInfo;
            BOOL bPlacementOK = TRUE;
            int32 tx, ty, x, y;

            tx = nTileX + gAddExitX[nDirection];
            ty = nTileY + gAddExitY[nDirection];

            tx -= ptTileInfo->pointAnchor.x;
            ty -= ptTileInfo->pointAnchor.y;

            int32 nAnchorIndex = ptTileInfo->pointAnchor.x + ptTileInfo->pointAnchor.y * ptTileInfo->nSizeX;
            CustomTileCell & tAnchorCell = ptTileInfo->arTiles[nAnchorIndex];

            int32 nBaseHeight = ptTile ? ptTile->GetExitHeight(nDirection) : 0;
            nBaseHeight -= tAnchorCell.nTileHeight;
            DRLGExitDirection eOppositeDirection = sOppositeDirection(nDirection);
            if (eOppositeDirection != DRLG_EXIT_NONE)
            {
                nBaseHeight -= tAnchorCell.arExitHeights[eOppositeDirection];
            }

            for (y = 0; y < ptTileInfo->nSizeY; y++)
            {
                for (x = 0; x < ptTileInfo->nSizeX; x++)
                {
                    int32 nIdx = x + y * ptTileInfo->nSizeX;
                    CustomTileCell & tCustomCell = ptTileInfo->arTiles[nIdx];
                    DRLGTileType nTileType = tCustomCell.nTileType;

                    if (tCustomCell.bEnabled && sIsValidTileType(nTileType) &&
                        (
                         GetTileAtLoc(tx + x, ty + y) != NULL ||
                         !sValidForChokepointExcludeDist(tx + x, ty + y) ||
                         !CheckHardConstraintsXY(tx + x, ty + y, nTileType, tCustomCell.nTileHeight - nBaseHeight, tCustomCell.arExitHeights)))
                    {
                        return FALSE;
                    }
                }
            }
        }
    }














    return TRUE;
}


BOOL DRLGDungeonGenerator::FilterTile(const DRLGTile * ptTile, DRLGExitDirection nExitDirection, DRLGTileType nTileType)
{
    BOOL bPassesFilter = FALSE;




    if (CheckHardConstraints(ptTile, nExitDirection, nTileType) && CheckSoftConstraints(ptTile, nExitDirection, FALSE, 0, 0))
        bPassesFilter = TRUE;
    return bPassesFilter;
}





void DRLGDungeonGenerator::FilterTiles(TPDRLGTileList & tTileList, DRLGExitDirection nExitDirection, DRLGTileType nTileType)
{




    POS pos = m_listTiles.GetHeadPosition();
    while (pos)
    {







        DRLGTile & tTile = m_listTiles.GetNext(pos);

        if (FilterTile(&tTile, nExitDirection, nTileType))

            tTileList.AddTail_Tracked(&tTile);
    }
}










































































DRLGTile * DRLGDungeonGenerator::AddTile(DRLGTile * ptParentTile, DRLGExitDirection nExitDirection, DRLGCommand * ptCommand, DRLGTileType nTileType, GridDataTileSets nTileSet, BOOL bFilter)
{
    if (ptParentTile == NULL)
        return AllocTile(ptCommand, NULL, 0, 0, nTileType, nTileSet, DRLG_INVALID_EXIT_HEIGHT, NULL, FALSE);

    int32 y;
    int32 x;
    x = ptParentTile->nXCoord + gAddExitX[nExitDirection];
    y = ptParentTile->nYCoord + gAddExitY[nExitDirection];





    if (bFilter && !FilterTile(ptParentTile, nExitDirection, nTileType))
        return NULL;




    DRLGTile * ptChildTile = AllocTile(ptCommand, ptParentTile, x, y, nTileType, nTileSet, DRLG_INVALID_EXIT_HEIGHT, NULL, FALSE);







    ConnectTiles(ptParentTile, ptChildTile, nExitDirection, DRLG_EDGE_UNDEFINED);
    return ptChildTile;
}



DRLGTile * DRLGDungeonGenerator::AddTile(
    DRLGExitDirection nExitDirection,
    DRLGCommand * ptCommand,
    DRLGTileType nTileType,
    GridDataTileSets nTileSet)
{
    DRLGTile * ptTile = NULL;

    if (m_listTiles.Count() > 0)
    {
        TPDRLGTileList listTiles(sDRLGDungeonGetPtrTilePool());

        FilterTiles(listTiles, nExitDirection, nTileType);

        if (listTiles.Count() > 0)
        {
            int32 nIndex = PickRand(listTiles.Count());

            POS pos = listTiles.GetHeadPosition();

            while (nIndex >= 0 && pos)
            {
                ptTile = listTiles.GetNext(pos);
                nIndex--;
            }

            ptTile = AddTile(ptTile, nExitDirection, ptCommand, nTileType, nTileSet, FALSE);
        }
    }
    else
    {
        ptTile = AddTile((DRLGTile *)NULL, nExitDirection, ptCommand, nTileType, nTileSet, FALSE);
    }

    return ptTile;
}


DRLGTile * DRLGDungeonGenerator::AddTile(int32 * arDirectionExitWeights, DRLGCommand * ptCommand, DRLGTileType nTileType, GridDataTileSets nTileSet, DRLGExitDirection * pExitDirection)
{
    int32 arExitWeights[DRLG_EXIT_CNT];
    SigmaMemoryCopy(arExitWeights, arDirectionExitWeights, sizeof(arExitWeights));

    int32 nWeightSum = 0;
    for (int i = 0; i < DRLG_EXIT_CNT; i++)
        nWeightSum += arExitWeights[i];

    DRLGTile * ptTile = NULL;
    while (TRUE)
    {
        int32 nExitDirection = PickWeightedRand(arExitWeights, DRLG_EXIT_CNT);
        ptTile = AddTile(nExitDirection, ptCommand, nTileType, nTileSet);
        if (ptTile)
        {
            if (pExitDirection)
                *pExitDirection = nExitDirection;
            break;
        }

        nWeightSum -= arExitWeights[nExitDirection];
        arExitWeights[nExitDirection] = 0;
        if (nWeightSum <= 0)
            break;
    }

    return ptTile;
}













DRLGTile * DRLGDungeonGenerator::AddTileByType(
    int32 * arTileWeightsIn,
    DRLGCommand * ptCommand,
    GridDataTileSets nTileSet)
{
    DRLGTile * ptTile;
    int32 nWeightSum;
    int32 arTileWeights[DRLG_TILE_NUM_TYPES];
    int32 arDirWeights[DRLG_EXIT_CNT];
    int32 i;

    SigmaMemoryCopy(arTileWeights, arTileWeightsIn, sizeof(arTileWeights));

    nWeightSum = 0;
    for (i = 0; i < DRLG_TILE_NUM_TYPES; i++)
        nWeightSum += arTileWeights[i];

    ptTile = NULL;
    while (nWeightSum > 0 && ptTile == NULL)
    {
        DRLGTileType nTileType = PickWeightedRand(arTileWeights, DRLG_TILE_NUM_TYPES);

        sSetExitWeightsForTileType(arDirWeights, nTileType);

        ptTile = AddTile(arDirWeights, ptCommand, nTileType, nTileSet, NULL);

        if (!ptTile)
        {
            nWeightSum -= arTileWeights[nTileType];
            arTileWeights[nTileType] = 0;
        }
    }

    return ptTile;
}

// NON_MATCHING: 2248 vs 2224 bytes; organic-neighbor control flow and table hoisting differ, with changed register allocation.
DRLGTile * DRLGDungeonGenerator::AddTileForShape(
    DRLGCommand * ptCommand,
    DRLGTileType nTileType,
    GridDataTileSets nTileSet,
    DRLGShapeType nShapeType,
    float flShapeParam,
    float flOriginX,
    float flOriginY)
{

    if (nShapeType == DRLG_SHAPE_UNDEFINED)
    {
        int32 arWeights[DRLG_EXIT_CNT] = { 100, 100, 100, 100 };
        return AddTile(arWeights, ptCommand, nTileType, nTileSet, NULL);
    }


    DRLGTile * ptParentTile = NULL;
    DRLGExitDirection nExitDirection = DRLG_EXIT_NONE;
    float flBestScore = FLT_MAX;
    int32 nRunningWeight = 0;

    for (int i = 0; i < DRLG_EXIT_CNT; i++)
    {
        TPDRLGTileList listTiles(sDRLGDungeonGetPtrTilePool());

        FilterTiles(listTiles, i, nTileType);

        POS pos = listTiles.GetHeadPosition();
        while (pos)
        {
            DRLGTile * ptTile = listTiles.GetNext(pos);
            int32 tx = ptTile->nXCoord + gAddExitX[i];
            int32 ty = ptTile->nYCoord + gAddExitY[i];
            float flScore = FLT_MAX;
            int32 nWeight = 1;

            switch (nShapeType)
            {
                case DRLG_SHAPE_CIRCLE:
                    flScore = Math::Pow(tx - flOriginX, 2.0f) + Math::Pow(ty - flOriginY, 2.0f);
                    break;

                case DRLG_SHAPE_SQUARE:
                    flScore = Math::Max(Math::Abs(tx - flOriginX), Math::Abs(ty - flOriginY));
                    break;

                case DRLG_SHAPE_OVAL:
                    flScore = Math::Pow((tx - flOriginX) * flShapeParam, 2.0f) + Math::Pow(ty - flOriginY, 2.0f);
                    break;

                case DRLG_SHAPE_RECTANGLE:
                    flScore = Math::Max(Math::Abs(tx - flOriginX) + flShapeParam, Math::Abs(ty - flOriginY));
                    break;

                case DRLG_SHAPE_ORGANIC:
                {






                    DRLGExitDirection leftDir = sRotateExitDirectionLeft(i);
                    DRLGTile * ptLeftTile = GetTileAtLoc(tx + gAddExitX[leftDir], ty + gAddExitY[leftDir]);
                    if (!ptLeftTile)
                    {
                        DRLGTile * ptNextLeftTile = GetTileAtLoc(tx + 2 * gAddExitX[leftDir], ty + 2 * gAddExitY[leftDir]);
                        if (ptNextLeftTile)
                        {
                            flScore = 500.0f;
                            break;
                        }
                    }

                    DRLGExitDirection rightDir = sRotateExitDirectionRight(i);
                    DRLGTile * ptRightTile = GetTileAtLoc(tx + gAddExitX[rightDir], ty + gAddExitY[rightDir]);
                    if (!ptRightTile)
                    {
                        DRLGTile * ptNextRightTile = GetTileAtLoc(tx + 2 * gAddExitX[rightDir], ty + 2 * gAddExitY[rightDir]);
                        if (ptNextRightTile)
                        {
                            flScore = 500.0f;
                            break;
                        }
                    }

                    DRLGTile * ptStraightTile = GetTileAtLoc(tx + gAddExitX[i], ty + gAddExitY[i]);
                    if (!ptStraightTile)
                    {
                        DRLGTile * ptNextStraightTile = GetTileAtLoc(tx + 2 * gAddExitX[i], ty + 2 * gAddExitY[i]);
                        if (ptNextStraightTile)
                        {
                            flScore = 500.0f;
                            break;
                        }
                    }


                    BOOL bDiagonalConflict = FALSE;
                    int32 nBaseDiagonalX = tx + 2 * gAddExitX[i];
                    int32 nBaseDiagonalY = ty + 2 * gAddExitY[i];
                    for (int32 nLeftOffset = -2; nLeftOffset <= 2 && !bDiagonalConflict; ++nLeftOffset)
                    {
                        if (nLeftOffset == 0)
                            continue;






                        int32 nDiagonalX = nBaseDiagonalX + nLeftOffset * gAddExitX[leftDir];
                        int32 nDiagonalY = nBaseDiagonalY + nLeftOffset * gAddExitY[leftDir];
                        DRLGTile * ptDiagonalTile = GetTileAtLoc(nDiagonalX, nDiagonalY);
                        if (ptDiagonalTile)
                        {
                            BOOL bTileFound = FALSE;
                            int32 nStartCheckOffset = nLeftOffset < 0 ? nLeftOffset : 1;
                            int32 nMaxCheckOffset = nLeftOffset < 0 ? -1 : nLeftOffset;
                            for (int32 nCheckOffset = nStartCheckOffset; nCheckOffset <= nMaxCheckOffset && !bTileFound; ++nCheckOffset)
                            {
                                for (int32 nForwardOffset = 0; nForwardOffset < 2 && !bTileFound; ++nForwardOffset)
                                {
                                    int32 nCheckX = tx + nCheckOffset * gAddExitX[leftDir] + nForwardOffset * gAddExitX[i];
                                    int32 nCheckY = ty + nCheckOffset * gAddExitY[leftDir] + nForwardOffset * gAddExitY[i];
                                    if (GetTileAtLoc(nCheckX, nCheckY))
                                        bTileFound = TRUE;
                                }
                            }

                            if (!bTileFound)
                                bDiagonalConflict = TRUE;
                        }
                    }

                    // Inference: if-forms at 4390/4395, plain if at 4419 (break at 4413) and 4402 placement from the original columns 41/33/36 and rows.

                    if (bDiagonalConflict)
                    {
                        flScore = 500.0f;
                        break;
                    }



                    if ((ptRightTile && ptStraightTile) || (ptLeftTile && (ptRightTile || ptStraightTile)))
                    {
                        flScore = -500.0f;
                        break;
                    }




                    if (ptLeftTile || ptRightTile || ptStraightTile)
                    {
                        flScore = 0.0f;
                        nWeight = 500 - Math::FloatFloorToInt(flShapeParam * 500.0f);
                    }
                    else
                    {
                        flScore = 0.0f;




                        DRLGTile * ptLeftNeighbor = ptTile->GetNeighbor(leftDir);
                        DRLGTile * ptRightNeighbor = ptTile->GetNeighbor(rightDir);
                        if (ptLeftNeighbor || ptRightNeighbor)
                            nWeight = 500;
                    }
                    break;
                }
            }







            if (nWeight > 0)
            {
                if (flScore < flBestScore)
                {
                    flBestScore = flScore;
                    nRunningWeight = nWeight;
                    ptParentTile = ptTile;
                    nExitDirection = i;
                }
                else if (flScore == flBestScore)
                {
                    nRunningWeight += nWeight;



                    if (PickRand(nRunningWeight) < nWeight)
                    {
                        ptParentTile = ptTile;
                        nExitDirection = i;
                    }
                }
            }
        }
    }

    if (ptParentTile)
    {
        return AddTile(ptParentTile, nExitDirection, ptCommand, nTileType, nTileSet, FALSE);
    }

    return NULL;
}


BOOL DRLGDungeonGenerator::PrePlaceTile(
    DRLGCommand * ptCommand,
    TileInfo * ptTileInfo,
    GridDataTileSets nPlacedSceneTileSet,
    int32 nTileIndex,
    int32 nBaseTileX,
    int32 nBaseTileY,
    int32 nBaseTileHeight,
    BOOL bForcePlacedSceneTileSet,
    int32 nOverrideGroupId)
{

    int32 nSceneSizeX, nSceneSizeY;
    sGetTileDimensions(*ptTileInfo, nSceneSizeX, nSceneSizeY);


    int32 nTileGroupId = GetGroupId();


    if (nOverrideGroupId < 0)
    {
        nOverrideGroupId = TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_OVERRIDE_GROUP_NUM);
    }
    if (nOverrideGroupId >= 0)
    {
        nTileGroupId = nOverrideGroupId;
    }
    else
    {

        int32 nExistingGroupId = -1;
        BOOL bMultipleGroups = FALSE;
        for (int32 ix = 0; ix < nSceneSizeX; ix++)
        {
            for (int32 iy = 0; iy < nSceneSizeY; iy++)
            {
                if (ptTileInfo->nTileType == DRLG_TILE_CUSTOM)
                {

                    CustomTileInfo & tCustomTileInfo = ptTileInfo->tCustomTileInfo;

                    int32 nCustomTileIndex = iy * nSceneSizeX + ix;
                    CustomTileCell & tCustomCell = tCustomTileInfo.arTiles[nCustomTileIndex];

                    if (!tCustomCell.bEnabled)
                        continue;
                }



                DRLGTile * ptTileAtLoc = GetTileAtLoc(nBaseTileX + ix, nBaseTileY + iy);
                if (ptTileAtLoc && ptTileAtLoc->nGroup >= 0)
                {

                    if (nExistingGroupId != -1 && ptTileAtLoc->nGroup != nExistingGroupId)
                    {
                        bMultipleGroups = TRUE;
                        break;
                    }
                    nExistingGroupId = ptTileAtLoc->nGroup;
                }
            }
            if (bMultipleGroups)
            { // Inference: original conflict exits both loops straight to the keep-GetGroupId path (0x6f02bc/0x6f0330 -> 0x6f0348); lines 4536-4550 have no line-table rows.
                nExistingGroupId = -1;
                break;
            }
        }



        if (nExistingGroupId != -1)
            nTileGroupId = nExistingGroupId;

    }


    DRLGTile * ptFirstTile = NULL;
    if (ptTileInfo->nTileType == DRLG_TILE_CUSTOM)
    {
        CustomTileInfo & tCustomTileInfo = ptTileInfo->tCustomTileInfo;


        nBaseTileX -= tCustomTileInfo.pointAnchor.x;
        nBaseTileY -= tCustomTileInfo.pointAnchor.y;


        int32 nAnchorIndex = tCustomTileInfo.pointAnchor.y * nSceneSizeX + tCustomTileInfo.pointAnchor.x;
        CustomTileCell & tAnchorCell = tCustomTileInfo.arTiles[nAnchorIndex];
        nBaseTileHeight -= tAnchorCell.nTileHeight;


        for (int32 ix = 0; ix < nSceneSizeX; ix++)
        {
            for (int32 iy = 0; iy < nSceneSizeY; iy++)
            {
                int32 nCustomTileIndex = iy * nSceneSizeX + ix;
                CustomTileCell & tCustomCell = tCustomTileInfo.arTiles[nCustomTileIndex];
                DRLGTileType nCustomTileType = (DRLGTileType)tCustomCell.nTileType;
                GridDataTileSets nCustomTileSet = tCustomCell.nTileSet;

                if (!tCustomCell.bEnabled)
                    continue;




                DRLGTile * ptTileAtLoc = GetTileAtLoc(nBaseTileX + ix, nBaseTileY + iy);
                if (ptTileAtLoc)
                {
                    ptTileAtLoc->nTileType = nCustomTileType;
                    ptTileAtLoc->nTileSet = nCustomTileSet;
                    ptTileAtLoc->nAllowedExits = sIsValidTileType(nCustomTileType) ? nCustomTileType : DRLG_TILE_NSEW;

                    ptTileAtLoc->ptCommand = ptCommand;
                    ptTileAtLoc->nCommandIndex = ptCommand - m_arCommands;
                }
                else
                {

                    ptTileAtLoc = AllocTile(
                        ptCommand,
                        NULL,
                        nBaseTileX + ix,
                        nBaseTileY + iy,
                        nCustomTileType,
                        nCustomTileSet,
                        nBaseTileHeight + tCustomCell.nTileHeight,
                        tCustomCell.arExitHeights,
                        FALSE);
                    if (!ptTileAtLoc)
                    {
                        int32 nCommandIndex = (int32)(ptCommand - m_arCommands);

                        DRLGTestEvent tEvent(
                            m_dwSeed,
                            DRLGTestEvent::STATUS_ERROR,
                            CRefString("Failed to allocate a custom tile in 'PrePlaceTile'."),
                            ptCommand->nCommandType,
                            DRLG_ERROR_RESOLUTION_TILES_OMITTED,
                            DRLG_TILE_NONE,
                            nCommandIndex,
                            DRLG_STAGE_GENERATION);
                        m_DRLGEvents.push_back(tEvent);
                        return FALSE;
                    }
                }











                if (tCustomCell.snoScene != SNO_NONE)
                {
                    ptTileAtLoc->snoName.Set(SCENE_SNO, tCustomCell.snoScene);

                    if (bForcePlacedSceneTileSet || nCustomTileSet == GRIDDATA_TILE_SET_NONE || nCustomTileSet == GRIDDATA_TILE_SET_DUMMY)
                    {
                        ptTileAtLoc->nTileSet = nPlacedSceneTileSet;
                    }
                }

                ptTileAtLoc->nTileIndex = nTileIndex;
                ptTileAtLoc->nGroup = nTileGroupId;

                if (m_bUseEdgeTypes && (tCustomCell.snoScene != SNO_NONE || ptTileAtLoc->IsDummy()))
                {
                    uint32 dwExteriorExits = DRLG_TILE_NSEW;
                    if (ix > 0) dwExteriorExits &= ~DRLG_TILE_N;
                    if (ix < nSceneSizeX - 1) dwExteriorExits &= ~DRLG_TILE_S;
                    if (iy < nSceneSizeY - 1) dwExteriorExits &= ~DRLG_TILE_E;
                    if (iy > 0) dwExteriorExits &= ~DRLG_TILE_W;

                    FinalizeEdgeTypes(ptTileAtLoc, dwExteriorExits, ptTileInfo);
                }

                if (!ptFirstTile)
                    ptFirstTile = ptTileAtLoc;
            }
        }
    }
    else
    {



        for (int32 ix = 0; ix < nSceneSizeX; ix++)
        {
            for (int32 iy = 0; iy < nSceneSizeY; iy++)
            {

                uint32 dwInteriorExits = DRLG_TILE_NSEW;
                if (ix == 0) dwInteriorExits &= ~DRLG_TILE_N;
                if (ix >= nSceneSizeX - 1) dwInteriorExits &= ~DRLG_TILE_S;
                if (iy >= nSceneSizeY - 1) dwInteriorExits &= ~DRLG_TILE_E;
                if (iy == 0) dwInteriorExits &= ~DRLG_TILE_W;


                uint32 dwExteriorExits = DRLG_TILE_NSEW;
                if (ix > 0) dwExteriorExits &= ~DRLG_TILE_N;
                if (ix < nSceneSizeX - 1) dwExteriorExits &= ~DRLG_TILE_S;
                if (iy < nSceneSizeY - 1) dwExteriorExits &= ~DRLG_TILE_E;
                if (iy > 0) dwExteriorExits &= ~DRLG_TILE_W;


                if (sIsValidTileType((DRLGTileType)ptTileInfo->nTileType))
                    dwExteriorExits &= ptTileInfo->nTileType;




                DRLGTileType nTileType = (DRLGTileType)(dwInteriorExits | dwExteriorExits);

                DRLGTile * ptTileAtLoc = GetTileAtLoc(nBaseTileX + ix, nBaseTileY + iy);
                if (ptTileAtLoc)
                {
                    ptTileAtLoc->nTileSet = ptTileInfo->nTileSet;
                    ptTileAtLoc->nTileType = nTileType;
                    ptTileAtLoc->nAllowedExits = nTileType;

                    ptTileAtLoc->ptCommand = ptCommand;
                    ptTileAtLoc->nCommandIndex = ptCommand - m_arCommands;
                }
                else
                {

                    ptTileAtLoc = AllocTile(
                        ptCommand,
                        NULL,
                        nBaseTileX + ix,
                        nBaseTileY + iy,
                        nTileType,
                        ptTileInfo->nTileSet,
                        nBaseTileHeight,
                        NULL, FALSE);
                    if (!ptTileAtLoc)
                    {
                        int32 nCommandIndex = (int32)(ptCommand - m_arCommands);

                        DRLGTestEvent tEvent(
                            m_dwSeed,
                            DRLGTestEvent::STATUS_ERROR,
                            CRefString("Failed to allocate a dummy tile in 'PrePlaceTile'."),
                            ptCommand->nCommandType,
                            DRLG_ERROR_RESOLUTION_TILES_OMITTED,
                            DRLG_TILE_NONE,
                            nCommandIndex,
                            DRLG_STAGE_GENERATION);
                        m_DRLGEvents.push_back(tEvent);
                        return FALSE;
                    }
                }










                if (ix == 0 && iy == 0)
                {
                    ptTileAtLoc->snoName.Set(SCENE_SNO, ptTileInfo->snoScene);
                    ptTileAtLoc->nTileSet = nPlacedSceneTileSet;
                }
                else
                {
                    ptTileAtLoc->nTileSet = GRIDDATA_TILE_SET_DUMMY;
                }


                ptTileAtLoc->nTileIndex = nTileIndex;
                ptTileAtLoc->nGroup = nTileGroupId;

                FinalizeEdgeTypes(ptTileAtLoc, dwExteriorExits, ptTileInfo);

                if (!ptFirstTile)
                    ptFirstTile = ptTileAtLoc;
            }
        }
    }




    int32 nSpecialTileMaxSpacing = sGetSpecialTileMinSpacing(ptTileInfo, DRLG_SPECIAL_TILE_NONE);
    DRLGSpecialTileType nSpecialTileType = (DRLGSpecialTileType)TagMapGetInt(ptTileInfo->hTileTagMap, TAG_WORLD_TILE_SPECIAL_TILE_TYPE);
    if (nSpecialTileMaxSpacing > 0 || nSpecialTileType != DRLG_SPECIAL_TILE_NONE)
    {
        m_arSpecialTiles.push_back(ptFirstTile);
    }

    return TRUE;
}






void DRLGDungeonGenerator::ExecuteCommand(
    int32 nCommandIndex)
{


    DRLGCommand * ptCommand = &m_arCommands[nCommandIndex];

    if (!TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_ENABLED))
        return;



    float flExecuteChance = TagMapGetFloat(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_CHANCE);
    if (flExecuteChance < 1.0f && RandFloat_Internal(*m_ptRNG) > flExecuteChance)
        return;



    if (!PushCommand(ptCommand, FALSE))
        return;




    DRLGLabelType nCommandLabel = TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_LABEL);
    if (nCommandLabel != DRLG_LABEL_NONE)
    {
        PushLabel(nCommandLabel);
    }


    m_nCommandTilesCount = 0;


    switch (ptCommand->nCommandType)
    {
    case DRLG_COMMAND_ADD_ROOMS: CmdAddTiles(ptCommand); break;
    case DRLG_COMMAND_ADD_ENTRANCE: CmdAddEntrance(ptCommand); break;
    case DRLG_COMMAND_ADD_EXIT: CmdAddExit(ptCommand); break;
    case DRLG_COMMAND_ADD_HUB: CmdAddHub(ptCommand); break;
    case DRLG_COMMAND_ADD_SPOKE: CmdAddSpoke(ptCommand); break;
    case DRLG_COMMAND_REMOVE_DEAD_ENDS: CmdRemoveDeadEnds(ptCommand); break;
    case DRLG_COMMAND_ADD_ENTRANCE_EXIT: CmdAddEntranceExit(ptCommand); break;
    case DRLG_COMMAND_ADD_ALL_TILES: CmdAddAllTilesHack(ptCommand); break;
    case DRLG_COMMAND_ADD_CUSTOM_TILE: CmdAddCustomTile(ptCommand); break;
    case DRLG_COMMAND_ADD_PERIMETER: CmdAddPerimeter(ptCommand); break;
    case DRLG_COMMAND_ADD_FIELDS: CmdAddFields(ptCommand); break;
    case DRLG_COMMAND_ADD_ROAD: CmdAddRoad(ptCommand); break;
    case DRLG_COMMAND_ADD_CHOKEPOINT: CmdAddChokepoint(ptCommand); break;
    case DRLG_COMMAND_REPLACE_ENTRANCE_EXIT: CmdReplaceEntranceExit(ptCommand); break;
    case DRLG_COMMAND_REPLACE_RANDOM_TILE: CmdReplaceRandomTile(ptCommand); break;
    case DRLG_COMMAND_ADD_NUBS: CmdAddNubs(ptCommand); break;
    case DRLG_COMMAND_ADD_PERIMETER_NUBS: CmdAddPerimeterNubs(ptCommand); break;
    }






    if (m_bErrors)
    {
        PopCommand();
        return;
    }



    BOOL bLabelPopped = FALSE;
    if (nCommandLabel != DRLG_LABEL_NONE && ptCommand->nCommandType != DRLG_COMMAND_ADD_LABEL)
    {
        PopLabel();
        bLabelPopped = TRUE;
    }


    if (ptCommand->nCommandType == DRLG_COMMAND_SELECT)
    {

        int32 i;
        int32 nChildCnt = 0;

        for (i = 0; i < m_nCommandCnt; i++)
        {

            if (m_arCommandParentIndices[i] == nCommandIndex)
            {
                DRLGCommand * ptCommandLoc = &m_arCommands[i];
                if (TagMapGetInt(ptCommandLoc->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_ENABLED))
                    nChildCnt++;
            }
        }



        if (nChildCnt > 0)
        {
            int32 nNumSelect, nSelectDelta, nSelectMin;
            int32 * arChildWeights = (int32 *)alloca(nChildCnt * sizeof(int32));

            nSelectMin = TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_SELECT_NUM);
            nSelectDelta = TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_SELECT_DELTA);

            nNumSelect = nSelectMin + PickRandRange(0, nSelectDelta);
            nNumSelect = Math::Min(nNumSelect, nChildCnt);


            int32 w = 0;
            int32 nBountyRequiredIndex = -1;
            for (i = 0; i < m_nCommandCnt; i++)
            {
                if (m_arCommandParentIndices[i] == nCommandIndex)
                {
                    DRLGCommand * ptCommandLoc = &m_arCommands[i];
                    if (TagMapGetInt(ptCommandLoc->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_ENABLED))
                    {
                        arChildWeights[w] = TagMapGetInt(ptCommandLoc->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_SELECT_WEIGHT);


                        if (arChildWeights[w] > 0 &&
                            ShouldReplaceTileForBounty(
                                TagMapGetInt(ptCommandLoc->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_TILE_TYPE),
                                TagMapGetInt(ptCommandLoc->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_TILE_SET)))
                        {
                            nBountyRequiredIndex = w;
                        }

                        w++;
                    }
                }
            }





            uint32 dwExecBitmask = 0;
            for (i = 0; i < nNumSelect; i++)
            {
                int32 nChildCmd = PickWeightedRand(arChildWeights, nChildCnt);

                if (nBountyRequiredIndex != -1)
                {
                    nChildCmd = nBountyRequiredIndex;
                    nBountyRequiredIndex = -1;
                }

                dwExecBitmask |= (1 << nChildCmd);


                arChildWeights[nChildCmd] = 0;
            }



            w = 0;
            for (i = 0; i < m_nCommandCnt && !m_bErrors; i++)
            {
                if (m_arCommandParentIndices[i] == nCommandIndex)
                {
                    DRLGCommand * ptCommandLoc = &m_arCommands[i];
                    if (TagMapGetInt(ptCommandLoc->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_ENABLED))
                    {
                        if (dwExecBitmask & (1 << w))
                        {
                            ExecuteCommand(i);
                        }

                        w++;
                    }
                }
            }
        }
    }
    else
    {


        for (int32 i = 0; i < m_nCommandCnt && !m_bErrors; i++)
        {
            if (m_arCommandParentIndices[i] == nCommandIndex)
            {
                ExecuteCommand(i);
            }
        }
    }


    if (nCommandLabel != DRLG_LABEL_NONE && !bLabelPopped)
    {
        PopLabel();
    }


    PopCommand();
}





static void sSetTilePortalWorlds(
    CTagMap * hCommandTagMap,
    DRLGTile * ptTile,
    BOOL bIsDungeonFinder)
{
    if (!bIsDungeonFinder)
    {
        if (TagMapTagExists(hCommandTagMap, TAG_WORLD_PREV_WORLD_SNO))
        {
            SNO snoPrevWorld = TagMapGetSNO(hCommandTagMap, TAG_WORLD_PREV_WORLD_SNO);

            if (snoPrevWorld != SNO_NONE)
            {
                ptTile->snoPrevWorld = snoPrevWorld;
                ptTile->uPrevEntranceGUID = TagMapGetDWORD(hCommandTagMap, TAG_WORLD_PREV_ENTRANCE);
                ptTile->snoPrevLevelArea = TagMapGetSNO(hCommandTagMap, TAG_WORLD_PREV_LEVEL_AREA);
            }
        }
        if (TagMapTagExists(hCommandTagMap, TAG_WORLD_NEXT_WORLD_SNO))
        {
            SNO snoNextWorld = TagMapGetSNO(hCommandTagMap, TAG_WORLD_NEXT_WORLD_SNO);

            if (snoNextWorld != SNO_NONE)
            {
                ptTile->snoNextWorld = snoNextWorld;
                ptTile->uNextEntranceGUID = TagMapGetDWORD(hCommandTagMap, TAG_WORLD_NEXT_ENTRANCE);
                ptTile->snoNextLevelArea = TagMapGetSNO(hCommandTagMap, TAG_WORLD_NEXT_LEVEL_AREA);
            }
        }
    }
    else
    {
        if (TagMapTagExists(hCommandTagMap, TAG_WORLD_PREV_WORLD_SNO))
        {
            SNO snoPrevWorld = TagMapGetSNO(hCommandTagMap, TAG_WORLD_PREV_WORLD_SNO);
            if (snoPrevWorld != SNO_NONE)
            {
                DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
            }
        }
        if (TagMapTagExists(hCommandTagMap, TAG_WORLD_NEXT_WORLD_SNO))
        {
            SNO snoNextWorld = TagMapGetSNO(hCommandTagMap, TAG_WORLD_NEXT_WORLD_SNO);
            if (snoNextWorld != SNO_NONE)
            {
                DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
            }
        }
    }
}


void DRLGDungeonGenerator::CmdAddTiles(
    DRLGCommand * ptCommand)
{
    ScopedDRLGWarningAsErrors warningsAsErrors(this, ptCommand);



    int32 arDirectionWeights[DRLG_EXIT_CNT];
    int32 nRoomCnt = TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_ROOM_CNT);
    int32 nRoomCntDelta = TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_ROOM_CNT_DELTA);
    nRoomCnt += PickRandRange(0, nRoomCntDelta);

    GridDataTileSets nTileSet = TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_TILE_SET);

    DRLGTileConstraint tConstraint;
    tConstraint.ptFilterCommand = GetParentTileCommand();
    PushConstraint(tConstraint);

    for (int32 i = 0; i < nRoomCnt; i++)
    {
        DRLGTileType nTileType = sGetCommandRandomTileType(ptCommand, *m_ptRNG);

        arDirectionWeights[DRLG_EXIT_NORTH] = TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_WEIGHT_NORTH);
        arDirectionWeights[DRLG_EXIT_SOUTH] = TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_WEIGHT_SOUTH);
        arDirectionWeights[DRLG_EXIT_EAST] = TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_WEIGHT_EAST);
        arDirectionWeights[DRLG_EXIT_WEST] = TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_WEIGHT_WEST);
        sZeroExitWeightsForTileType(arDirectionWeights, nTileType);

        DRLGTile * ptTile = AddTile(arDirectionWeights, ptCommand, nTileType, nTileSet, NULL);
        if (!ptTile)
        {
            int32 nCommandIndex = (int32)(ptCommand - m_arCommands);

            DRLGTestEvent tEvent(
                m_dwSeed,
                m_bWarningsAsErrors ? DRLGTestEvent::STATUS_ERROR : DRLGTestEvent::STATUS_WARNING,
                CRefString("Failed to add specified number of rooms."),
                ptCommand->nCommandType,
                DRLG_ERROR_RESOLUTION_TILES_OMITTED,
                nTileType,
                nCommandIndex,
                DRLG_STAGE_GENERATION);

            m_DRLGEvents.push_back(tEvent);









            break;
        }
    }
    PopConstraint();
}


void DRLGDungeonGenerator::CmdAddEntrance(
    DRLGCommand * ptCommand)
{
    ScopedDRLGWarningAsErrors warningsAsErrors(this, ptCommand);

    DRLGTileType eTileType = sGetCommandRandomTileType(ptCommand, *m_ptRNG);
    GridDataTileSets eTileSet = TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_TILE_SET);

    int32 nStartGroup = TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_START_GROUP_NUM);
    int32 bRetryAnywhere = TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_RETRY_ANYWHERE);

    int32 arTileWeights[DRLG_TILE_NUM_TYPES];
    if (!sIsValidTileType(eTileType))
    {
        PickTilesFromSet(arTileWeights, eTileSet, DRLG_TILE_NONE);
    }
    else
    {
        SigmaMemoryClear(arTileWeights, sizeof(arTileWeights));
        arTileWeights[eTileType] = 100;
    }

    DRLGTileConstraint tConstraint;
    tConstraint.nGroupNum = nStartGroup;
    tConstraint.ptFilterCommand = GetParentTileCommand();
    PushConstraint(tConstraint);
    DRLGTile * ptTile = AddTileByType(arTileWeights, ptCommand, eTileSet);
    PopConstraint();

    int32 nCommandIndex = (int32)(ptCommand - m_arCommands);

    if (bRetryAnywhere && !ptTile)
    {
        DRLGTestEvent tEvent(
            m_dwSeed,
            DRLGTestEvent::STATUS_WARNING,
            CRefString("Unable to add entrance at desired position."),
            ptCommand->nCommandType,
            DRLG_ERROR_RESOLUTION_TILES_RELOCATED,
            eTileType,
            nCommandIndex,
            DRLG_STAGE_GENERATION);

        m_DRLGEvents.push_back(tEvent);


        int32 arDirWeights[DRLG_EXIT_CNT];







        for (int32 i = 0; i < DRLG_TILE_NUM_TYPES; i++)
        {
            if (arTileWeights[i] > 0)
            {
                eTileType = i;

                sSetExitWeightsForTileType(arDirWeights, eTileType);


                ptTile = AddTile(arDirWeights, ptCommand, eTileType, eTileSet, NULL);

                if (ptTile)
                    break;
            }
        }


    }

    if (!ptTile)
    {
        DRLGTestEvent tEvent(
            m_dwSeed,
            m_bWarningsAsErrors ? DRLGTestEvent::STATUS_ERROR : DRLGTestEvent::STATUS_WARNING,
            CRefString("Failed to add an entrance tile."),
            ptCommand->nCommandType,
            DRLG_ERROR_RESOLUTION_TILES_OMITTED,
            eTileType,
            nCommandIndex,
            DRLG_STAGE_GENERATION);

        m_DRLGEvents.push_back(tEvent);







        return;
    }

    sSetTilePortalWorlds(ptCommand->hCommandTagMap, ptTile, IsDungeonFinder());

}


void DRLGDungeonGenerator::CmdAddExit(
    DRLGCommand * ptCommand)
{
    ScopedDRLGWarningAsErrors warningsAsErrors(this, ptCommand);

    DRLGTileType eTileType = sGetCommandRandomTileType(ptCommand, *m_ptRNG);
    GridDataTileSets eTileSet = TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_TILE_SET);



    if (IsDungeonFinder() && GameActiveGameCommonDataIsServer())
    {
        if (WorldGetDungeonFinderWorldFromLevel(SWorldGetNumDungeonFinderLevels()) == WorldCommonGetWorldSNO(m_idSWorld))
        {
            eTileSet = GRIDDATA_TILE_SET_EXIT_2;
        }
    }

    int32 nStartGroup = TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_START_GROUP_NUM);
    int32 bRetryAnywhere = TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_RETRY_ANYWHERE);

    int32 arTileWeights[DRLG_TILE_NUM_TYPES];
    if (!sIsValidTileType(eTileType))
    {
        PickTilesFromSet(arTileWeights, eTileSet, DRLG_TILE_NONE);
    }
    else
    {
        SigmaMemoryClear(arTileWeights, sizeof(arTileWeights));
        arTileWeights[eTileType] = 100;
    }

    DRLGTileConstraint tConstraint;
    tConstraint.nGroupNum = nStartGroup;
    tConstraint.ptFilterCommand = GetParentTileCommand();
    PushConstraint(tConstraint);
    DRLGTile * ptTile = AddTileByType(arTileWeights, ptCommand, eTileSet);
    PopConstraint();

    int32 nCommandIndex = (int32)(ptCommand - m_arCommands);

    if (bRetryAnywhere && !ptTile)
    {
        DRLGTestEvent tEvent(
            m_dwSeed,
            DRLGTestEvent::STATUS_WARNING,
            CRefString("Unable to add exit at desired position."),
            ptCommand->nCommandType,
            DRLG_ERROR_RESOLUTION_TILES_RELOCATED,
            eTileType,
            nCommandIndex,
            DRLG_STAGE_GENERATION);

        m_DRLGEvents.push_back(tEvent);


        int32 arDirWeights[DRLG_EXIT_CNT];







        for (int32 i = 0; i < DRLG_TILE_NUM_TYPES; i++)
        {
            if (arTileWeights[i] > 0)
            {
                eTileType = i;

                sSetExitWeightsForTileType(arDirWeights, eTileType);


                ptTile = AddTile(arDirWeights, ptCommand, eTileType, eTileSet, NULL);

                if (ptTile)
                    break;
            }
        }


    }

    if (!ptTile)
    {
        DRLGTestEvent tEvent(
            m_dwSeed,
            m_bWarningsAsErrors ? DRLGTestEvent::STATUS_ERROR : DRLGTestEvent::STATUS_WARNING,
            CRefString("Failed to add an exit tile."),
            ptCommand->nCommandType,
            DRLG_ERROR_RESOLUTION_TILES_OMITTED,
            eTileType,
            nCommandIndex,
            DRLG_STAGE_GENERATION);

        m_DRLGEvents.push_back(tEvent);







        return;
    }

    sSetTilePortalWorlds(ptCommand->hCommandTagMap, ptTile, IsDungeonFinder());

}



void DRLGDungeonGenerator::sConnectToHub(DRLGTile *& ptTileParent, DRLGCommand * ptCommand, const int32 x, const int32 y, const DRLGExitDirection eDir, GridDataTileSets nTileSet, DRLGTileType nTileType)
{







    if (!sIsValidTileType((DRLGTileType)ptTileParent->nTileType) ||
        sIsLinkPossible((DRLGTileType)ptTileParent->nTileType, eDir, nTileType))
    {



        DRLGTile * ptTileNew = GetTileAtLoc(x, y);
        if (ptTileNew == NULL)
        {
            ptTileNew = AllocTile(ptCommand, ptTileParent, x, y, nTileType, nTileSet, DRLG_INVALID_EXIT_HEIGHT, NULL, FALSE);
        }
        else if (ptTileNew->ptCommand != ptCommand)
        {
            int32 nCommandIndex = (int32)(ptCommand - m_arCommands);

            DRLGTestEvent tEvent(
                m_dwSeed,
                m_bWarningsAsErrors ? DRLGTestEvent::STATUS_ERROR : DRLGTestEvent::STATUS_WARNING,
                CRefString("Tried to connect to a room not in the hub."),
                ptCommand->nCommandType,
                DRLG_ERROR_RESOLUTION_TILES_MALFORMED,
                DRLG_TILE_NONE,
                nCommandIndex,
                DRLG_STAGE_GENERATION);

            m_DRLGEvents.push_back(tEvent);

            return;
        }

        ConnectTiles(ptTileParent, ptTileNew, eDir, DRLG_EDGE_UNDEFINED);
        ptTileParent = ptTileNew;
    }
}







int32 DRLGDungeonGenerator::sDetermineOffsetForHub(
    const int32 x,
    const int32 y,
    const int32 nHubLength,
    const int32 nHubWidth,
    const DRLGExitDirection eDir,
    DRLGCommand * ptCommand)
{
    int32 minOffset = -1;
    int32 offsetDelta = 0;


    DRLGExitDirection nRotatedDir = sRotateExitDirectionLeft(eDir);


    int32 nColumns = nHubWidth * 2 - 1;
    int32 nFreeColumns = 0;


    int32 columnX = x - (nHubWidth - 1) * gAddExitX[nRotatedDir];
    int32 columnY = y - (nHubWidth - 1) * gAddExitY[nRotatedDir];

    for (int32 i = 0; i < nColumns; i++)
    {
        BOOL bPlacementOK = TRUE;


        int32 tileX = columnX;
        int32 tileY = columnY;



        for (int32 j = 0; j < nHubLength; j++)
        {
            DRLGTile * pTile = GetTileAtLoc(tileX, tileY);
            if (pTile && pTile->ptCommand != ptCommand)
            {
                bPlacementOK = FALSE;
                break;
            }

            tileX += gAddExitX[eDir];
            tileY += gAddExitY[eDir];
        }

        if (bPlacementOK)
        {
            nFreeColumns++;
            if (nFreeColumns >= nHubWidth)
            {
                if (minOffset < 0)
                    minOffset = i;
                else
                    offsetDelta++;
            }
        }
        else
        {
            if (minOffset >= 0)
                break;
            nFreeColumns = 0;
        }



        columnX += gAddExitX[nRotatedDir];
        columnY += gAddExitY[nRotatedDir];
    }

    if (minOffset < 0)
    {

        int32 nCommandIndex = (int32)(ptCommand - m_arCommands);

        DRLGTestEvent tEvent(
            m_dwSeed,
            m_bWarningsAsErrors ? DRLGTestEvent::STATUS_ERROR : DRLGTestEvent::STATUS_WARNING,
            CRefString("Unable to find a free space for entire hub."),
            ptCommand->nCommandType,
            DRLG_ERROR_RESOLUTION_TILES_OMITTED,
            DRLG_TILE_NONE,
            nCommandIndex,
            DRLG_STAGE_GENERATION);

        m_DRLGEvents.push_back(tEvent);
        return 0;
    }








    int32 offset = minOffset + 2 - nHubWidth * 2 + PickRandRange(0, offsetDelta);
    return offset;
}



void DRLGDungeonGenerator::CmdAddHub(
    DRLGCommand * ptCommand)
{
    ScopedDRLGWarningAsErrors warningsAsErrors(this, ptCommand);

    int32 nHubSize, nHubSizeDelta, nHubLength, nHubWidth;
    int32 arWeights[DRLG_EXIT_CNT] = { 100, 100, 100, 100 };

    GridDataTileSets nTileSet = TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_TILE_SET);
    DRLGTileType nTileType = TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_TILE_TYPE);

    nHubSize = TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_HUB_SIZE);
    nHubSizeDelta = TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_HUB_SIZE_DELTA);
    nHubSize = PickRandRange(nHubSize, nHubSizeDelta);


    nHubLength = TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_HUB_LENGTH);
    if (nHubLength > 0)
    {
        nHubSizeDelta = TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_HUB_LENGTH_DELTA);
        nHubLength = PickRandRange(nHubLength, nHubSizeDelta);
    }
    else
    {
        nHubLength = nHubSize;
    }

    nHubWidth = TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_HUB_WIDTH);
    if (nHubWidth > 0)
    {
        nHubSizeDelta = TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_HUB_WIDTH_DELTA);
        nHubWidth = PickRandRange(nHubWidth, nHubSizeDelta);
    }
    else
    {
        nHubWidth = nHubSize;
    }



    int32 nTotalLengthWidth = nHubLength + nHubWidth;
    blz::vector<int32> aHubLengthArray;

    BOOL bFlexibleDimensions = TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_HUB_FLEXIBLE_DIMENSIONS);
    if (bFlexibleDimensions && nTotalLengthWidth >= 5)
    {
        for (int32 nLength = 2; nLength <= nTotalLengthWidth - 2; nLength++)
        {
            aHubLengthArray.push_back(nLength);
        }
    }
    else
    {
        aHubLengthArray.push_back(nHubLength);



        BOOL bAllowLengthWidthSwap = TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_ALLOW_ROTATION);
        if (bAllowLengthWidthSwap)
        {
            aHubLengthArray.push_back(nHubWidth);
        }
    }


    for (int32 i = (int32)aHubLengthArray.size() - 1; i > 0; i--)
    {
        int32 nIndex = PickRand(i + 1);
        Math::Swap(aHubLengthArray[i], aHubLengthArray[nIndex]);
    }

    DRLGTileConstraint tConstraint;
    BOOL bSuccess = FALSE;

    for (uint32 i = 0; i < aHubLengthArray.size(); i++)
    {
        nHubLength = aHubLengthArray[i];
        nHubWidth = nTotalLengthWidth - nHubLength;
        tConstraint.nHubLength = nHubLength;
        tConstraint.nHubWidth = nHubWidth;
        tConstraint.ptFilterCommand = GetParentTileCommand();


        PushConstraint(tConstraint);
        DRLGExitDirection nExitDirection;
        DRLGTile * ptTile = AddTile(arWeights, ptCommand, nTileType, nTileSet, &nExitDirection);
        PopConstraint();

        if (!ptTile)
            continue;





        if (nHubLength > 1 || nHubWidth > 1)
        {

            int32 hx = ptTile->nXCoord;
            int32 hy = ptTile->nYCoord;


            int32 offset = sDetermineOffsetForHub(hx, hy, nHubLength, nHubWidth, nExitDirection, ptCommand);
            DRLGExitDirection offsetDir = sRotateExitDirectionLeft(nExitDirection);


            int32 newX = hx + offset * gAddExitX[offsetDir];
            int32 newY = hy + offset * gAddExitY[offsetDir];
            hx = newX;
            hy = newY;
            if (offset != 0)
            {
                ptTile = AllocTile(ptCommand, NULL, newX, newY, nTileType, nTileSet, DRLG_INVALID_EXIT_HEIGHT, NULL, FALSE);
            }


            int32 x = 0, y = 0;


            DRLGTile * ptTileParent = ptTile;
            while (x < nHubLength - 1)
            {
                x++;
                hx += gAddExitX[nExitDirection];
                hy += gAddExitY[nExitDirection];

                sConnectToHub(ptTileParent, ptCommand, hx, hy, nExitDirection, nTileSet, nTileType);
            }

            nExitDirection = sRotateExitDirectionLeft(nExitDirection);
            while (y < nHubWidth - 1)
            {
                y++;
                hx += gAddExitX[nExitDirection];
                hy += gAddExitY[nExitDirection];

                sConnectToHub(ptTileParent, ptCommand, hx, hy, nExitDirection, nTileSet, nTileType);
            }

            nExitDirection = sRotateExitDirectionLeft(nExitDirection);
            while (x > 0)
            {
                x--;
                hx += gAddExitX[nExitDirection];
                hy += gAddExitY[nExitDirection];

                sConnectToHub(ptTileParent, ptCommand, hx, hy, nExitDirection, nTileSet, nTileType);
            }

            nExitDirection = sRotateExitDirectionLeft(nExitDirection);
            while (y > 0)
            {
                y--;
                hx += gAddExitX[nExitDirection];
                hy += gAddExitY[nExitDirection];

                sConnectToHub(ptTileParent, ptCommand, hx, hy, nExitDirection, nTileSet, nTileType);
            }
        }


        for (int32 nTag = TAG_WORLD_DRLG_COMMAND_ADD_SPECIAL_TILE_1; nTag <= TAG_WORLD_DRLG_COMMAND_ADD_SPECIAL_TILE_3; nTag++)
        {

            GridDataTileSets nNewTileSet = TagMapGetInt(ptCommand->hCommandTagMap, nTag);
            if (nNewTileSet != GRIDDATA_TILE_SET_RAT_MAZE_1)
            {
                DRLGTile * ptChangedTile = NULL;
                int32 nCount = 0;

                POS pos = m_listTiles.GetHeadPosition();
                while (pos)
                {
                    DRLGTile & tTile = m_listTiles.GetNext(pos);


                    if (tTile.ptCommand == ptCommand)
                    {

                        if (tTile.nTileSet == nTileSet)
                        {


                            DRLGTileType nNewTileType = (DRLGTileType)tTile.nCurrentExits;


                            if (IsTileAvailable(nNewTileSet, nNewTileType))
                            {

                                if (PickRandRange(0, nCount++) == 0)
                                    ptChangedTile = &tTile;
                            }
                        }
                    }
                }

                if (ptChangedTile)
                {

                    ptChangedTile->nTileSet = nNewTileSet;
                    ptChangedTile->nTileType = ptChangedTile->nCurrentExits;
                }
            }
        }
        bSuccess = TRUE;
        break;
    }


    if (!bSuccess)
    {
        int32 nCommandIndex = (int32)(ptCommand - m_arCommands);

        DRLGTestEvent tEvent(
            m_dwSeed,
            m_bWarningsAsErrors ? DRLGTestEvent::STATUS_ERROR : DRLGTestEvent::STATUS_WARNING,
            CRefString("Unable to place initial room in hub."),
            ptCommand->nCommandType,
            DRLG_ERROR_RESOLUTION_TILES_OMITTED,
            DRLG_TILE_NONE,
            nCommandIndex,
            DRLG_STAGE_GENERATION);

        m_DRLGEvents.push_back(tEvent);
    }







}


void DRLGDungeonGenerator::CmdAddSpoke(
    DRLGCommand * ptCommand)
{
    ScopedDRLGWarningAsErrors warningsAsErrors(this, ptCommand);

    int32 arDirectionWeights[DRLG_EXIT_CNT];
    int32 arTypedDirectionWeights[DRLG_EXIT_CNT];


    int32 nRoomCnt = TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_ROOM_CNT);
    int32 nRoomCntDelta = TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_ROOM_CNT_DELTA);
    int32 nStartGroup = TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_START_GROUP_NUM);

    arDirectionWeights[DRLG_EXIT_NORTH] = TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_WEIGHT_NORTH);
    arDirectionWeights[DRLG_EXIT_SOUTH] = TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_WEIGHT_SOUTH);
    arDirectionWeights[DRLG_EXIT_EAST] = TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_WEIGHT_EAST);
    arDirectionWeights[DRLG_EXIT_WEST] = TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_WEIGHT_WEST);

    GridDataTileSets nTileSet = TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_TILE_SET);
    DRLGTileType nTileType = sGetCommandRandomTileType(ptCommand, *m_ptRNG);

    BOOL bNoRepeatTileTypes = TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_SPOKE_NO_REPEAT_TYPES);

    SigmaMemoryCopy(arTypedDirectionWeights, arDirectionWeights, sizeof(arTypedDirectionWeights));
    sZeroExitWeightsForTileType(arTypedDirectionWeights, nTileType);

    nRoomCnt += PickRandRange(0, nRoomCntDelta);

    DRLGCommand * ptCommandParent = GetParentTileCommand();
    DRLGTileConstraint tConstraint;
    tConstraint.ptFilterCommand = ptCommandParent;
    tConstraint.nNumSpokeRooms = nRoomCnt;
    tConstraint.nAllowedSpokeDir = GetTileTypeFromWeights(arDirectionWeights);

    PushConstraint(tConstraint);
    DRLGTile * ptTile = AddTile(arTypedDirectionWeights, ptCommand, nTileType, nTileSet, NULL);
    PopConstraint();

    if (nRoomCnt > 1 && ptTile)
    {
        tConstraint.ptFilterCommand = NULL;
        tConstraint.nRoomCreateNum = 1;
        nRoomCnt--;

        for (; nRoomCnt > 0; nRoomCnt--)
        {
            tConstraint.nNumSpokeRooms = nRoomCnt;
            PushConstraint(tConstraint);
            ptTile = AddTile(arDirectionWeights, ptCommand, DRLG_TILE_NONE, nTileSet, NULL);
            PopConstraint();

            if (!ptTile)
                break;

            if (bNoRepeatTileTypes)
            {

                if (ptTile->ptParentTile)
                {
                    tConstraint.uSpokeTileTypeFlags |= 1 << ptTile->ptParentTile->nCurrentExits;
                }



                if (nRoomCnt <= 1)
                {
                    for (int32 nCheckDir = 0; nCheckDir < DRLG_EXIT_CNT; nCheckDir++)
                    {
                        if (!(ptTile->nCurrentExits & (1 << nCheckDir)))
                        {


                            int32 nTestTileType = ptTile->nCurrentExits | (1 << nCheckDir);
                            if (tConstraint.uSpokeTileTypeFlags & (1 << nTestTileType))
                            {
                                ptTile->nAllowedExits &= ~(1 << nCheckDir);
                            }
                        }
                    }
                }
            }
        }
    }

    if (!ptTile)
    {
        int32 nCommandIndex = (int32)(ptCommand - m_arCommands);

        DRLGTestEvent tEvent(
            m_dwSeed,
            m_bWarningsAsErrors ? DRLGTestEvent::STATUS_ERROR : DRLGTestEvent::STATUS_WARNING,
            CRefString("Failed to add specified number of rooms."),
            ptCommand->nCommandType,
            DRLG_ERROR_RESOLUTION_TILES_OMITTED,
            DRLG_TILE_NONE,
            nCommandIndex,
            DRLG_STAGE_GENERATION);







    }
}


void DRLGDungeonGenerator::CmdRemoveDeadEnds(
    DRLGCommand * ptCommand)
{
    ScopedDRLGWarningAsErrors warningsAsErrors(this, ptCommand);

    BOOL bConnectTiles = TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_DEAD_END_CONNECT_TILES);
    BOOL bConnectFirstTileOnly = TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_DEAD_END_CONNECT_FIRST_ONLY);

    if (bConnectTiles)
    {
        DRLGTileConnectTechnique nTechnique = DRLG_CONNECT_ALL_DEAD_END_TILES;

        if (bConnectFirstTileOnly)
        {
            nTechnique = DRLG_CONNECT_FIRST_DEAD_END_TILE;
        }


        ConnectDRLGTiles(nTechnique, 2);
    }

    {
        TDRLGDeadEndList tDeadEndList(sDRLGDungeonGetDeadEndPool());
        const int32 nTotalTiles = m_listTiles.Count();

        int32 nDeadEndCnt = GetDeadEnds(tDeadEndList);

        if (nDeadEndCnt > 0)
        {

            int32 nMaxDeadEndCnt = TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_DEAD_END_MAX_NUM);
            int32 nMaxDeadEndLen = TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_DEAD_END_MAX_LEN);

            POS pos = tDeadEndList.GetHeadPosition();


            while (pos)
            {
                DRLGDeadEnd & tDeadEnd = tDeadEndList.GetNext(pos);

                DRLGTile * ptTile = tDeadEnd.ptTile;




                DRLGExitDirection nLastDirection = DRLG_EXIT_NONE;

                int32 nDeadEndLength = tDeadEnd.nLength;
                int32 nTotalTilesConsidered = 0;



                while (nDeadEndCnt > nMaxDeadEndCnt || nDeadEndLength > nMaxDeadEndLen)
                {




                    DRLGTile * ptTileNext = GetNextDeadEndTile(ptTile, nLastDirection);

                    RemoveTile(ptTile);

                    if (!ptTileNext)
                        break;
                    nDeadEndLength--;



                    if (nTotalTilesConsidered >= nTotalTiles)
                        break;




                    nTotalTilesConsidered++;
                    ptTile = ptTileNext;
                }





                if (nDeadEndLength == 0)
                    nDeadEndCnt--;
            }
        }


    }
}


void DRLGDungeonGenerator::CmdAddEntranceExit(
    DRLGCommand * ptCommand)
{
    ScopedDRLGWarningAsErrors warningsAsErrors(this, ptCommand);

    IVector2D posMin;
    IVector2D posMax;
    int32 nWidth, nHeight;

    int32 nTileTypeEntrance = TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_TILE_TYPE_ENTRANCE);
    int32 nTileSetEntrance = TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_TILE_SET_ENTRANCE);
    int32 nTileTypeExit = TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_TILE_TYPE_EXIT);
    int32 nTileSetExit = TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_TILE_SET_EXIT);
    int32 bRetryAnywhere = TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_RETRY_ANYWHERE);


    // Inference: the original server/final-dungeon-world checks select exit set 301.
    if (IsDungeonFinder() && GameActiveGameCommonDataIsServer())
    {
        if (WorldGetDungeonFinderWorldFromLevel(SWorldGetNumDungeonFinderLevels()) == WorldCommonGetWorldSNO(m_idSWorld))
        {
            nTileSetExit = 301;
        }
    }

    int32 nCommandIndex = (uint32)(ptCommand - m_arCommands);

    int32 arTileTypesEntrance[16];
    int32 arTileTypesExit[16];

    if (!sIsValidTileType(nTileTypeEntrance))
    {
        PickTilesFromSet(arTileTypesEntrance, nTileSetEntrance, DRLG_TILE_NONE);
    }
    else
    {
        SigmaMemoryClear(arTileTypesEntrance, sizeof(arTileTypesEntrance));
        arTileTypesEntrance[nTileSetEntrance] = 100;
    }

    if (!sIsValidTileType(nTileTypeExit))
    {
        PickTilesFromSet(arTileTypesExit, nTileSetExit, DRLG_TILE_NONE);
    }
    else
    {
        SigmaMemoryClear(arTileTypesExit, sizeof(arTileTypesExit));
        arTileTypesExit[nTileTypeExit] = 100;
    }

    // Inference: original 5937 loads the tile-set local for the entrance array index, unlike exit.
    int32 nMinDistance = TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_MIN_DISTANCE);
    if (nMinDistance > 0)
    {

        int32 arTileExitWeightsEntrance[4];
        SigmaMemoryClear(arTileExitWeightsEntrance, sizeof(arTileExitWeightsEntrance));

        int32 arTileExitWeightsExit[4];
        SigmaMemoryClear(arTileExitWeightsExit, sizeof(arTileExitWeightsExit));

        for (int32 i = 0; i < DRLG_TILE_NUM_TYPES; ++i)
        {
            if (arTileTypesEntrance[i] > 0)
            {
                for (int32 j = 0; j < 4; ++j)
                {
                    if (i & (1 << j))
                    {
                        arTileExitWeightsEntrance[j] += arTileTypesEntrance[i];
                    }
                }
            }

            if (arTileTypesExit[i] > 0)
            {
                for (int32 j = 0; j < 4; ++j)
                {
                    if (i & (1 << j))
                    {
                        arTileExitWeightsExit[j] += arTileTypesExit[i];
                    }
                }
            }
        }

        DRLGLabelType nEntranceLabel = (DRLGLabelType)TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_SUBSTITUTE_ENTRANCE_LABEL);
        DRLGLabelType nExitLabel = (DRLGLabelType)TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_SUBSTITUTE_EXIT_LABEL);

        // Inference: candidate counts start at zero; each successful direction insertion increments them.
        blz::vector<DRLGTile *> arEntranceTiles[4];
        int32 arEntranceTileTotalWeights[4];
        SigmaMemoryClear(arEntranceTileTotalWeights, sizeof(arEntranceTileTotalWeights));
        int32 nEntranceTilesCount = 0;

        blz::vector<DRLGTile *> arExitTiles[4];
        int32 arExitTileTotalWeights[4];
        SigmaMemoryClear(arExitTileTotalWeights, sizeof(arExitTileTotalWeights));
        int32 nExitTilesCount = 0;

        {
            ScopedWorldDefinitionLockR pdefWorld(m_snoWorld);

            POS pos = m_listTiles.GetHeadPosition();
            while (pos)
            {
                DRLGTile & tTile = m_listTiles.GetNext(pos);

                for (int32 i = 0; i < DRLG_EXIT_CNT; ++i)
                { // Inference: eDir aliases the loop index; its conversion has no distinct instruction.
                    DRLGExitDirection eDir = i;
                    if (!tTile.CanAddTile(eDir, 0))
                    {
                        continue;
                    }
                    // Inference: four direction bits, DRLG_LABEL_NONE wildcard and positive weights follow the original branches.
                    DRLGExitDirection eOppositeDir = sOppositeDirection(eDir);

                    if ((nEntranceLabel == DRLG_LABEL_NONE || tTile.HasLabel(nEntranceLabel)) &&
                        arTileExitWeightsEntrance[eOppositeDir] > 0)
                    {
                        arEntranceTiles[eOppositeDir].push_back(&tTile);
                        arEntranceTileTotalWeights[eOppositeDir] += arTileExitWeightsEntrance[eOppositeDir];
                        ++nEntranceTilesCount;
                    }

                    if ((nExitLabel == DRLG_LABEL_NONE || tTile.HasLabel(nExitLabel)) &&
                        arTileExitWeightsExit[eOppositeDir] > 0)
                    {
                        arExitTiles[eOppositeDir].push_back(&tTile);
                        arExitTileTotalWeights[eOppositeDir] += arTileExitWeightsExit[eOppositeDir];
                        ++nExitTilesCount;
                    }
                }
            }
        }

        nMinDistance -= 2;
        int32 nMaxDistance = TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_MAX_DISTANCE);

        // Inference: retry distinct entrance candidates while both candidate sets remain nonempty.
        while (nEntranceTilesCount > 0 && nExitTilesCount > 0)
        {

            DRLGExitDirection eEntranceDir = PickWeightedRand(arEntranceTileTotalWeights, 4);
            int32 nRandEntranceTile = PickRand((int32)arEntranceTiles[eEntranceDir].size());
            DRLGTile *ptEntranceConnectTile = arEntranceTiles[eEntranceDir][nRandEntranceTile];

            DRLGExitDirection eEntranceOppositeDir = sOppositeDirection(eEntranceDir);
            IVector2D vecEntranceCoords(ptEntranceConnectTile->nXCoord + gAddExitX[eEntranceOppositeDir],
                                        ptEntranceConnectTile->nYCoord + gAddExitY[eEntranceOppositeDir]);


            blzext::vector_erase_at(arEntranceTiles[eEntranceDir], nRandEntranceTile);
            arEntranceTileTotalWeights[eEntranceDir] -= arTileExitWeightsEntrance[eEntranceDir];
            --nEntranceTilesCount;


            DRLGDungeonGenerator::ComputeRoomDistances(ptEntranceConnectTile);

            // Inference: possible-exit count starts zero and increments only for distance-qualified candidates.
            blz::vector<DRLGTile *> arPossibleExitTiles[4];
            int32 arPossibleExitTileTotalWeights[4];
            SigmaMemoryClear(arPossibleExitTileTotalWeights, sizeof(arPossibleExitTileTotalWeights));
            int32 nPossibleExitTilesCount = 0;

            for (int32 nDir = 0; nDir < DRLG_EXIT_CNT; ++nDir)
            {
                size_t nExitTilesForExitDir = arExitTiles[nDir].size();
                for (uint32 nTile = 0; nTile < nExitTilesForExitDir; ++nTile)
                {
                    DRLGTile *ptExitTile = arExitTiles[nDir][nTile];

                    DRLGExitDirection eExitOppositeDir = sOppositeDirection(nDir);
                    IVector2D vecExitCoords(ptExitTile->nXCoord + gAddExitX[eExitOppositeDir],
                                            ptExitTile->nYCoord + gAddExitY[eExitOppositeDir]);

                    if (vecEntranceCoords == vecExitCoords)
                    {
                        continue;
                    }

                    if (ptExitTile->nDistance >= nMinDistance &&
                        (nMaxDistance <= 0 || ptExitTile->nDistance <= nMaxDistance))
                    {
                        arPossibleExitTiles[nDir].push_back(ptExitTile);
                        arPossibleExitTileTotalWeights[nDir] += arTileExitWeightsExit[nDir];
                        ++nPossibleExitTilesCount;
                    }
                }
            }


            if (nPossibleExitTilesCount <= 0)
            {
                continue;
            }


            if (!sIsValidTileType(nTileTypeEntrance))
            {
                int32 arTileTypeWeights[16];
                SigmaMemoryClear(arTileTypeWeights, sizeof(arTileTypeWeights));

                for (int32 t = 0; t < 16; ++t)
                {
                    if (t & (1 << eEntranceDir))
                    {
                        arTileTypeWeights[t] = arTileTypesEntrance[t];
                    }
                }

                nTileTypeEntrance = PickWeightedRand(arTileTypeWeights, 16);
            }


            DRLGTile * ptEntranceTile = AddTile(ptEntranceConnectTile, eEntranceOppositeDir, ptCommand, nTileTypeEntrance, nTileSetEntrance, FALSE);
            if (!ptEntranceTile)
            {
                DRLGTestEvent tEvent(
                    m_dwSeed,
                    DRLGTestEvent::STATUS_ERROR,
                    CRefString("\"Add Entrance+Exit\" failed to add an entrance tile for min/max distance!"),
                    ptCommand->nCommandType,
                    DRLG_ERROR_RESOLUTION_TILES_OMITTED,
                    nTileTypeEntrance,
                    nCommandIndex,
                    DRLG_STAGE_GENERATION);

                m_DRLGEvents.push_back(tEvent);






                return;
            }


            DRLGExitDirection eExitDir = PickWeightedRand(arPossibleExitTileTotalWeights, 4);
            int32 nRandExitTile = PickRand((int32)arPossibleExitTiles[eExitDir].size());
            DRLGTile *ptExitConnectTile = arPossibleExitTiles[eExitDir][nRandExitTile];


            if (!sIsValidTileType(nTileTypeExit))
            {
                int32 arTileTypeWeights[16];
                SigmaMemoryClear(arTileTypeWeights, sizeof(arTileTypeWeights));

                for (int32 t = 0; t < 16; ++t)
                {
                    if (t & (1 << eExitDir))
                    {
                        arTileTypeWeights[t] = arTileTypesExit[t];
                    }
                }

                nTileTypeExit = PickWeightedRand(arTileTypeWeights, 16);
            }


            DRLGExitDirection eExitOppositeDir = sOppositeDirection(eExitDir);
            DRLGTile * ptExitTile = AddTile(ptExitConnectTile, eExitOppositeDir, ptCommand, nTileTypeExit, nTileSetExit, FALSE);
            if (!ptExitTile)
            {
                DRLGTestEvent tEvent(
                    m_dwSeed,
                    DRLGTestEvent::STATUS_ERROR,
                    CRefString("\"Add Entrance+Exit\" failed to add an exit tile for min/max distance!"),
                    ptCommand->nCommandType,
                    DRLG_ERROR_RESOLUTION_TILES_OMITTED,
                    nTileTypeEntrance,
                    nCommandIndex,
                    DRLG_STAGE_GENERATION);

                m_DRLGEvents.push_back(tEvent);






                return;
            }


            sSetTilePortalWorlds(ptCommand->hCommandTagMap, ptEntranceTile, IsDungeonFinder());
            sSetTilePortalWorlds(ptCommand->hCommandTagMap, ptExitTile, IsDungeonFinder());
            return;
        }


        DRLGTestEvent tEvent(
            m_dwSeed,
            m_bWarningsAsErrors ? DRLGTestEvent::STATUS_ERROR : DRLGTestEvent::STATUS_WARNING,
            CRefString("\"Add Entrance+Exit\" command was specified a min/max distance, but it couldn't be fulfilled!  Using fallback placement."),
            ptCommand->nCommandType,
            m_bWarningsAsErrors ? DRLG_ERROR_RESOLUTION_TILES_OMITTED : DRLG_ERROR_RESOLUTION_TILES_RELOCATED,
            nTileTypeEntrance,
            nCommandIndex,
            DRLG_STAGE_GENERATION);

        m_DRLGEvents.push_back(tEvent);






        if (m_bWarningsAsErrors)
        {
            return;
        }
    }





    // Inference: fallback uses opposite extremes of the longer grid dimension, as original comparisons.
    GetGridBounds(posMin, posMax);
    nWidth = posMax.x - posMin.x;
    nHeight = posMax.y - posMin.y;

    // Inference: both new tile pointers begin NULL; each side is tried before an unrestricted retry.
    DRLGTile * pEntranceTile = NULL;
    DRLGTile * pExitTile = NULL;

    int32 nAddFlag1 = 1;
    int32 nAddFlag2 = 2;

    if (nWidth > nHeight)
    {

        int32 y; for (y = posMin.y; y <= posMax.y; ++y)
        {

            DRLGTile * ptCurTile = GetTileAtLoc(posMin.x, y);
            if (ptCurTile)
            {
                ptCurTile->nFilterFlags = nAddFlag1;
            }
        }

        for (y = posMin.y; y <= posMax.y; ++y)
        {


            DRLGTile * ptCurTile = GetTileAtLoc(posMax.x, y);
            if (ptCurTile)
            {
                ptCurTile->nFilterFlags = nAddFlag2;
            }
        }
    }
    else
    {


        int32 x; for (x = posMin.x; x <= posMax.x; ++x)
        {

            DRLGTile * ptCurTile = GetTileAtLoc(x, posMin.y);
            if (ptCurTile)
            {
                ptCurTile->nFilterFlags = nAddFlag1;
            }
        }

        for (x = posMin.x; x <= posMax.x; ++x)
        {


            DRLGTile * ptCurTile = GetTileAtLoc(x, posMax.y);
            if (ptCurTile)
            {
                ptCurTile->nFilterFlags = nAddFlag2;
            }
        }
    }



    DRLGTileConstraint tConstraint;
    tConstraint.nFilterFlag = nAddFlag1;

    PushConstraint(tConstraint);


    pEntranceTile = AddTileByType(arTileTypesEntrance, ptCommand, nTileSetEntrance);
    if (!pEntranceTile)
    {

        pExitTile = AddTileByType(arTileTypesExit, ptCommand, nTileSetExit);
    }
    PopConstraint();

    tConstraint.nFilterFlag = nAddFlag2;
    PushConstraint(tConstraint);
    if (!pEntranceTile)
    {
        pEntranceTile = AddTileByType(arTileTypesEntrance, ptCommand, nTileSetEntrance);
    }
    else if (!pExitTile)
    {
        pExitTile = AddTileByType(arTileTypesExit, ptCommand, nTileSetExit);
    }
    PopConstraint();

    if (pEntranceTile == NULL && bRetryAnywhere)
    {
        DRLGTestEvent tEvent(
            m_dwSeed,
            DRLGTestEvent::STATUS_WARNING,
            CRefString("Unable to add entrance at desired position."),
            ptCommand->nCommandType,
            DRLG_ERROR_RESOLUTION_TILES_RELOCATED,
            nTileTypeEntrance,
            nCommandIndex,
            DRLG_STAGE_GENERATION);

        m_DRLGEvents.push_back(tEvent);







        pEntranceTile = AddTileByType(arTileTypesEntrance, ptCommand, nTileSetEntrance);
    }

    if (pExitTile == NULL && bRetryAnywhere)
    {
        DRLGTestEvent tEvent(
            m_dwSeed,
            DRLGTestEvent::STATUS_WARNING,
            CRefString("Failed to add exit at desired position."),
            ptCommand->nCommandType,
            DRLG_ERROR_RESOLUTION_TILES_RELOCATED,
            nTileTypeExit,
            nCommandIndex,
            DRLG_STAGE_GENERATION);

        m_DRLGEvents.push_back(tEvent);







        pExitTile = AddTileByType(arTileTypesExit, ptCommand, nTileSetExit);
    }

    if (pEntranceTile)
    {
        sSetTilePortalWorlds(ptCommand->hCommandTagMap, pEntranceTile, IsDungeonFinder());
    }

    if (pExitTile)
    {
        sSetTilePortalWorlds(ptCommand->hCommandTagMap, pExitTile, IsDungeonFinder());
    }

    if (pEntranceTile && pExitTile)
    {
        ComputeRoomDistances(pEntranceTile);
        // Inference: longest-distance values feed eliminated diagnostics; named locals and original list traversals remain.
        int32 nLongestDistExit = 0; // Inference: the next eliminated accumulator also starts at zero by its maximum-distance role.
        int32 nLongestDistEntrance = 0;

        POS pos = m_listTiles.GetHeadPosition();

        while (pos)
        { // Inference: original retained only traversal; the DWARF longest-distance accumulator implies this eliminated maximum.
            nLongestDistExit = Math::Max(nLongestDistExit, m_listTiles.GetNext(pos).nDistance);
        }






        ComputeRoomDistances(pExitTile);

        pos = m_listTiles.GetHeadPosition();

        while (pos)
        { // Inference: original retained only traversal; the other named accumulator implies the same eliminated maximum.
            nLongestDistEntrance = Math::Max(nLongestDistEntrance, m_listTiles.GetNext(pos).nDistance);
        }




        return;
    }


    DRLGTestEvent tEntranceWarning(
        m_dwSeed,
        m_bWarningsAsErrors ? DRLGTestEvent::STATUS_ERROR : DRLGTestEvent::STATUS_WARNING,
        CRefString("Failed to add an entrance tile."),
        ptCommand->nCommandType,
        DRLG_ERROR_RESOLUTION_TILES_OMITTED,
        nTileTypeEntrance,
        nCommandIndex,
        DRLG_STAGE_GENERATION);

    DRLGTestEvent tExitWarning(
        m_dwSeed,
        m_bWarningsAsErrors ? DRLGTestEvent::STATUS_ERROR : DRLGTestEvent::STATUS_WARNING,
        CRefString("Failed to add an exit tile."),
        ptCommand->nCommandType,
        DRLG_ERROR_RESOLUTION_TILES_OMITTED,
        nTileTypeExit,
        nCommandIndex,
        DRLG_STAGE_GENERATION);

    m_DRLGEvents.push_back(tEntranceWarning);
    m_DRLGEvents.push_back(tExitWarning);








}

// NON_MATCHING: 5912 vs 5944 bytes (was 6080). No missing loop level (latch rows agree). Fixed from the line table: bAllowed is (ExitExists || !ptNeighbor || CanAddExit) (6533 columns 71/89), which lets the tile exit count hoist out of the direction loop like the original; generic-room path is a block under !ptTile (6751/6777/6778 one level deeper); grid wrap test is if (x < 12) (x advance on 6866). Left: CanAddExit re-tests ptNeighbor/ExitExists after the 6533 ccmp where the original threads into its third test, and the 6542 test is branchy where the original computes bRequired | !ExitExists; registers. Tried a range-checked ExitExists (breaks another function).
void DRLGDungeonGenerator::CmdAddAllTilesHack(
    DRLGCommand * ptCommand)
{
    SNO snoWorld = m_snoWorld;
    ScopedWorldDefinitionLockR pdefWorld(snoWorld);


    // Inference: original command tag selects connected placement versus the tile-grid preview branch.
    if (TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_ALL_TILES_CONNECTED))
    {
        int32 arDirectionWeights[4]; // Inference: the original open-room comparison supplies the constant30 declared below.
        const int32 nMaxOpenRoomCount = 30;







        // Inference: insertion randomizes all non-NONE/non-CUSTOM tile indices, matching original list traversal.
        XGrowableList<int32, int32> listRemainingTiles;
        for (int32 nTileIndex = 0; nTileIndex < pdefWorld->ptServerData->ptDRLG->arTiles.GetArraySize(); nTileIndex++)
        {
            TileInfo * ptTileInfo = &pdefWorld->ptServerData->ptDRLG->arTiles[nTileIndex];


            if (ptTileInfo->nTileType != DRLG_TILE_NONE && ptTileInfo->nTileType != DRLG_TILE_CUSTOM)
            {







                int32 nRandPos = PickRand(listRemainingTiles.Count() + 1);
                POS pos = listRemainingTiles.GetHeadPosition();
                while (pos && nRandPos > 0)
                {
                    listRemainingTiles.GetNext(pos);
                    nRandPos--;
                }

                if (pos)
                {
                    listRemainingTiles.InsertBefore_Tracked(pos, nTileIndex);
                }
                else
                {
                    listRemainingTiles.AddTail_Tracked(nTileIndex);
                }
            }
        }




        XGrowableList<DRLGTile *, DRLGTile *> listPlacedTiles;




        XGrowableList<DRLGTile *, DRLGTile *> listOpenRooms;


        while (listRemainingTiles.Count() > 0)
        {
            DRLGTile * ptTile = NULL;


            POS posRemaining = listRemainingTiles.GetHeadPosition();
            while (posRemaining)
            { // Inference: the saved iterator alias follows the original next/remove sequence and DWARF scope.
                POS posCurRemaining = posRemaining;
                int32 nTileIndex = listRemainingTiles.GetNext(posRemaining);
                TileInfo * ptTileInfo = &pdefWorld->ptServerData->ptDRLG->arTiles[nTileIndex];



                POS posOpen = listOpenRooms.GetHeadPosition();
                while (posOpen)
                { // Inference: the saved iterator alias follows the original next/remove sequence and DWARF scope.
                    POS posCurOpen = posOpen;
                    DRLGTile * ptOpenTile = listOpenRooms.GetNext(posOpen);





                    // Inference: legality starts TRUE and is cleared by original required/allowed or forbidden-existing-exit rejection paths.
                    BOOL bLegal = TRUE;

                    for (int32 i = 0; i < DRLG_EXIT_CNT; i++)
                    {
                        DRLGExitDirection nDir = (DRLGExitDirection)i;
                        BOOL bRequired = (ptTileInfo->nTileType & sExitDirectionToTileType(nDir)) != 0;
                        DRLGTile * ptNeighbor = ptOpenTile->GetNeighbor(nDir);


                        BOOL bAllowed = (ptOpenTile->ExitExists(nDir) || !ptNeighbor || ptOpenTile->CanAddExit(nDir, TRUE));
                        if (bRequired && !bAllowed)
                        {
                            bLegal = FALSE;
                            break;
                        }



                        if (!bRequired && ptOpenTile->ExitExists(nDir))
                        {
                            bLegal = FALSE;
                            break;
                        }
                    }
                    if (bLegal)
                    {

                        ptTile = ptOpenTile;


                        ptTile->nTileType = ptTileInfo->nTileType;
                        ptTile->nTileSet = ptTileInfo->nTileSet;
                        ptTile->nAllowedExits = ptTileInfo->nTileType;


                        listOpenRooms.RemoveAt(posCurOpen);
                        break;
                    }
                }




                if (!ptTile)
                {

                    sSetExitWeightsForTileType(arDirectionWeights, (DRLGTileType)ptTileInfo->nTileType);
                    ptTile = AddTile(arDirectionWeights, ptCommand, (DRLGTileType)ptTileInfo->nTileType, (GridDataTileSets)ptTileInfo->nTileSet, NULL);
                }


                if (!ptTile)
                {
                    POS posPlaced = listPlacedTiles.GetHeadPosition();
                    while (posPlaced)
                    { // Inference: the saved iterator alias follows the original next/remove sequence and DWARF scope.
                        POS posCurPlaced = posPlaced;
                        DRLGTile * ptPlacedTile = listPlacedTiles.GetNext(posPlaced);



                        // Inference: original search retains -1 until the first direction passing all neighbor compatibility tests.
                        DRLGExitDirection nLegalDir = DRLG_EXIT_NONE;

                        for (int32 i = 0; i < DRLG_EXIT_CNT; i++)
                        {
                            DRLGExitDirection nDir = (DRLGExitDirection)i;


                            DRLGTile * ptNeighbor = ptPlacedTile->GetNeighbor(nDir);
                            if (ptNeighbor)
                                continue;



                            // Inference: both link tests use child NSEW(15) from concrete inline DIEs; the original compares opposing exit-presence bits.
                            DRLGExitDirection nOtherDir = sOppositeDirection(nDir);
                            BOOL bExitExists = sIsLinkPossible(ptTileInfo->nTileType, nOtherDir, DRLG_TILE_NSEW);
                            if (!bExitExists)
                                continue;


                            // Inference: bLegal is initially TRUE; original rejects the first conflicting real neighbor and selects the first remaining direction.
                            BOOL bLegal = TRUE;
                            int32 tx = ptPlacedTile->nXCoord + gAddExitX[nDir];
                            int32 ty = ptPlacedTile->nYCoord + gAddExitY[nDir];

                            for (int32 j = 0; j < DRLG_EXIT_CNT; j++)
                            {
                                DRLGExitDirection nNeighborDir = (DRLGExitDirection)j;
                                DRLGExitDirection nOtherNeighborDir = sOppositeDirection(nNeighborDir);


                                if (nNeighborDir == nOtherDir)
                                    continue;



                                DRLGTile * ptNextNeighbor = GetTileAtLoc(tx + gAddExitX[nNeighborDir], ty + gAddExitY[nNeighborDir]);
                                if (ptNextNeighbor && sIsValidTileType(ptNextNeighbor->nTileType) &&
                                    sIsLinkPossible(ptTileInfo->nTileType, nNeighborDir, DRLG_TILE_NSEW) != sIsLinkPossible(ptNextNeighbor->nTileType, nOtherNeighborDir, DRLG_TILE_NSEW))
                                {
                                    bLegal = FALSE;
                                    break;
                                }
                            }
                            if (bLegal)
                            {
                                nLegalDir = nDir;
                                break;
                            }
                        }



                        if (nLegalDir == DRLG_EXIT_NONE)
                            continue;

                        // Inference: return the displaced index to the pending list, reset the old tile to an open room, then attach the requested tile.

                        listRemainingTiles.AddTail_Tracked(ptPlacedTile->nTileIndex);
                        listPlacedTiles.RemoveAt(posCurPlaced);


                        ptPlacedTile->nTileIndex = -1;
                        ptPlacedTile->nTileType = DRLG_TILE_NONE; ptPlacedTile->nTileSet = (GridDataTileSets)0;
                        ptPlacedTile->nAllowedExits = DRLG_TILE_NSEW;

                        ptPlacedTile->snoName.Set(SNOGROUP_NONE, SNO_NONE);
                        listOpenRooms.AddTail_Tracked(ptPlacedTile);


                        ptTile = AllocTile(ptCommand, ptPlacedTile, ptPlacedTile->nXCoord + gAddExitX[nLegalDir], ptPlacedTile->nYCoord + gAddExitY[nLegalDir],
                                           ptTileInfo->nTileType, ptTileInfo->nTileSet, 65535, NULL, FALSE);

                        ConnectTiles(ptPlacedTile, ptTile, nLegalDir, DRLG_EDGE_UNDEFINED);
                        break;
                    }
                }
                if (!ptTile)
                    continue;







                if (ptTileInfo->snoScene != SNO_NONE)
                {
                    ptTile->snoName.Set(SCENE_SNO, ptTileInfo->snoScene);
                }


                ptTile->nTileIndex = nTileIndex;


                for (int32 i = 0; i < DRLG_EXIT_CNT; i++)
                {
                    DRLGExitDirection nDir = (DRLGExitDirection)i;
                    if (!ptTile->IsExitRequired(nDir))
                        continue;




                    if (ptTile->CanAddExit(nDir, TRUE))
                    {
                        DRLGTile * ptNeighborTile = ptTile->GetNeighbor(nDir);
                        ConnectTiles(ptTile, ptNeighborTile, nDir, DRLG_EDGE_UNDEFINED);
                    }
                    else if (ptTile->CanAddTile(nDir, 0))
                    {
                        DRLGTile * ptOpenTile = AllocTile(ptCommand, ptTile, ptTile->nXCoord + gAddExitX[nDir],
                                                         ptTile->nYCoord + gAddExitY[nDir], DRLG_TILE_NONE, (GridDataTileSets)0, 65535, NULL, FALSE);

                        ConnectTiles(ptTile, ptOpenTile, nDir, DRLG_EDGE_UNDEFINED);

                        listOpenRooms.AddTail_Tracked(ptOpenTile);
                    }
                }





















                listPlacedTiles.AddTail_Tracked(ptTile);


                listRemainingTiles.RemoveAt(posCurRemaining);
                break;
            }









            // Inference: index/status and both error strings follow original calls; original room cap is30.
            int32 nCommandIndex = ptCommand - m_arCommands;
            // line table: 6751, 6777 and 6778 are one level deeper than 6742 (columns 21/26/21), so the generic-room path is a block under !ptTile
            if (!ptTile)
            {





                if (listOpenRooms.Count() >= nMaxOpenRoomCount)
                {
                    DRLGTestEvent tEvent(
                        m_dwSeed,
                        m_bWarningsAsErrors ? DRLGTestEvent::STATUS_ERROR : DRLGTestEvent::STATUS_WARNING,
                        CRefString("Failed to add all tiles.\nExceeded max number of extra rooms.\nTry generating with another seed."),
                        ptCommand->nCommandType,
                        DRLG_ERROR_RESOLUTION_TILES_OMITTED,
                        DRLG_TILE_NONE,
                        nCommandIndex,
                        DRLG_STAGE_GENERATION);
                    m_DRLGEvents.push_back(tEvent);
                    break;
                }











                sSetExitWeightsForTileType(arDirectionWeights, DRLG_TILE_NONE);
                ptTile = AddTile(arDirectionWeights, ptCommand, DRLG_TILE_NONE, (GridDataTileSets)0, NULL);
                if (!ptTile)
                {
                    DRLGTestEvent tEvent(
                        m_dwSeed,
                        m_bWarningsAsErrors ? DRLGTestEvent::STATUS_ERROR : DRLGTestEvent::STATUS_WARNING,
                        CRefString("Failed to add all tiles.\nUnable to place generic room.\nTry generating with another seed."),
                        ptCommand->nCommandType,
                        DRLG_ERROR_RESOLUTION_TILES_OMITTED,
                        DRLG_TILE_NONE,
                        nCommandIndex,
                        DRLG_STAGE_GENERATION);
                    m_DRLGEvents.push_back(tEvent);
                    break;
                }













                listOpenRooms.AddTail_Tracked(ptTile);
            }
        }
    }
    else
    {
        int32 x = 0, y = 0;
        int32 nColumnWidth = 1;

        for (int32 nTileIndex = 0; nTileIndex < pdefWorld->ptServerData->ptDRLG->arTiles.GetArraySize(); nTileIndex++)
        {
            int32 nRowHeight = 1; // Inference: tx and ty below retain the original x/y register values; no separate copy instruction survives.
            int32 tx = x, ty = y;
            TileInfo & tTileInfo = pdefWorld->ptServerData->ptDRLG->arTiles[nTileIndex];
            CustomTileInfo & tCustomTileInfo = tTileInfo.tCustomTileInfo;

            if (tCustomTileInfo.eCustomType == CUSTOM_TYPE_MULTI_TILE)
            {
                for (int32 i = 0; i < tCustomTileInfo.nSizeY; i++)
                {
                    for (int32 j = 0; j < tCustomTileInfo.nSizeX; j++)
                    {
                        int32 nIdx = j + i * tCustomTileInfo.nSizeX;
                        CustomTileCell & tCustomCell = tCustomTileInfo.arTiles[nIdx];

                        if (tCustomCell.bEnabled)
                        {
                            DRLGTileType nTileType = tCustomCell.nTileType;
                            GridDataTileSets nTileSet = tCustomCell.nTileSet;

                            DRLGTile * ptRatTile = AllocTile(ptCommand, NULL, tx + j, ty + i, nTileType, nTileSet, tCustomCell.nTileHeight, tCustomCell.arExitHeights, FALSE);
                            if (tCustomCell.snoScene != SNO_NONE)
                            {
                                ptRatTile->snoName.Set(SCENE_SNO, tCustomCell.snoScene);
                            }
                        }
                    }
                }

                nRowHeight = tCustomTileInfo.nSizeX;
                nColumnWidth = Math::Max(nColumnWidth, tCustomTileInfo.nSizeY);
            }
            else
            {
                DRLGTileType nTileType = tTileInfo.nTileType;
                GridDataTileSets nTileSet = tTileInfo.nTileSet;

                DRLGTile * ptRatTile = AllocTile(ptCommand, NULL, tx, ty, nTileType, nTileSet, 65535, NULL, FALSE);
                if (tTileInfo.snoScene != SNO_NONE)
                {
                    ptRatTile->snoName.Set(SCENE_SNO, tTileInfo.snoScene);
                }

                if (tCustomTileInfo.eCustomType == CUSTOM_TYPE_MULTI_SCENE)
                {
                    ptRatTile->nTileIndex = nTileIndex;
                }
            }
            // Inference: original wraps once the old x reaches 12, advancing y by max column height plus 1; otherwise advances x by row width plus 1. Line table: the x advance is row 6866, so the advance is the if branch
            if (x < 12)
            {
                x += nRowHeight + 1;
            }
            else
            {
                x = 0;
                y += nColumnWidth + 1;
                nColumnWidth = 1;
            }
        }
    }
}

// NON_MATCHING: 3264 vs 3256 B; vector allocation and diagnostic-path register/spill allocation differ.
void DRLGDungeonGenerator::CmdAddCustomTile(
    DRLGCommand * ptCommand)
{
    ScopedDRLGWarningAsErrors warningsAsErrors(this, ptCommand);

    SNO snoWorld = m_snoWorld;
    ScopedWorldDefinitionLockR pdefWorld(snoWorld);


    int32 i;
    int32 nTileSet = TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_TILE_SET);



    if (IsDungeonFinder() && GameActiveGameCommonDataIsServer() && nTileSet == GRIDDATA_TILE_SET_COMPOUND_1)
    {
        if (WorldGetDungeonFinderWorldFromLevel(SWorldGetNumDungeonFinderLevels()) == WorldCommonGetWorldSNO(m_idSWorld))
            nTileSet = GRIDDATA_TILE_SET_COMPOUND_2;
    }



    TileInfo * ptTileInfo = NULL;
    int32 nTileCells = 0, nAnchorIndex = 0;



    blz::vector<TileInfo *> arCustomTiles;
    arCustomTiles.reserve(pdefWorld->ptServerData->ptDRLG->arTiles.GetArraySize());

    int32 nCommandIndex = (int32)(ptCommand - m_arCommands);

    for (i = 0; i < pdefWorld->ptServerData->ptDRLG->arTiles.GetArraySize(); i++)
    {
        ptTileInfo = &pdefWorld->ptServerData->ptDRLG->arTiles[i];


        if (ptTileInfo->nTileSet != nTileSet)
        {
            continue;
        }


        if (ptTileInfo->nTileType != DRLG_TILE_CUSTOM)
        {
            LPCSTR szCustomTileType = SymbolToName(DRLG_TILE_CUSTOM, gDRLGTileTypes);
            LPCSTR szTileSet = SymbolToName(ptTileInfo->nTileSet, gDRLGTileSets);

            DRLGTestEvent tEvent(
                m_dwSeed,
                m_bWarningsAsErrors ? DRLGTestEvent::STATUS_ERROR : DRLGTestEvent::STATUS_WARNING,
                CRefString(CRefString().Format("Custom tile has improper tile type in set %s.\nTile type should be set to %s.", szTileSet, szCustomTileType)),
                ptCommand->nCommandType,
                DRLG_ERROR_RESOLUTION_TILES_OMITTED,
                ptTileInfo->nTileType,
                nCommandIndex,
                DRLG_STAGE_GENERATION);

            m_DRLGEvents.push_back(tEvent);

            LPCSTR szTileType = SymbolToName(ptTileInfo->nTileType, gDRLGTileTypes);








            continue;
        }


        nTileCells = ptTileInfo->tCustomTileInfo.nSizeX * ptTileInfo->tCustomTileInfo.nSizeY;
        nAnchorIndex = ptTileInfo->tCustomTileInfo.pointAnchor.x +
            ptTileInfo->tCustomTileInfo.pointAnchor.y * ptTileInfo->tCustomTileInfo.nSizeX;


        if (nTileCells == 0 || nAnchorIndex >= nTileCells)
        {
            int32 nSizeX = ptTileInfo->tCustomTileInfo.nSizeX;
            int32 nSizeY = ptTileInfo->tCustomTileInfo.nSizeY;

            const char * szTileSetStr = SymbolToName(nTileSet, gDRLGTileSets);
            if (nTileCells == 0)
            {
                DRLGTestEvent tEvent(
                    m_dwSeed,
                    m_bWarningsAsErrors ? DRLGTestEvent::STATUS_ERROR : DRLGTestEvent::STATUS_WARNING,
                    CRefString(CRefString().Format("Custom tile does not have a valid tile size (%d x %d) in set %s.", nSizeX, nSizeY, szTileSetStr)),
                    ptCommand->nCommandType,
                    DRLG_ERROR_RESOLUTION_TILES_OMITTED,
                    ptTileInfo->nTileType,
                    nCommandIndex,
                    DRLG_STAGE_GENERATION);


                m_DRLGEvents.push_back(tEvent);





            }
            else
            {
                int nAnchorX = ptTileInfo->tCustomTileInfo.pointAnchor.x;
                int nAnchorY = ptTileInfo->tCustomTileInfo.pointAnchor.y;

                DRLGTestEvent tEvent(
                    m_dwSeed,
                    m_bWarningsAsErrors ? DRLGTestEvent::STATUS_ERROR : DRLGTestEvent::STATUS_WARNING,
                    CRefString(CRefString().Format("Custom tile in set %s does not have a valid anchor point (%d, %d) given it's size (%d x %d).", szTileSetStr, nAnchorX, nAnchorY, nSizeX, nSizeY)),
                    ptCommand->nCommandType,
                    DRLG_ERROR_RESOLUTION_TILES_OMITTED,
                    ptTileInfo->nTileType,
                    nCommandIndex,
                    DRLG_STAGE_GENERATION);


                m_DRLGEvents.push_back(tEvent);





            }
            continue;
        }




        arCustomTiles.push_back(ptTileInfo);
    }


    if (arCustomTiles.empty())
    {
        const char * szTileSetStr = SymbolToName(ptTileInfo->nTileSet, gDRLGTileSets);

        DRLGTestEvent tEvent(
            m_dwSeed,
            m_bWarningsAsErrors ? DRLGTestEvent::STATUS_ERROR : DRLGTestEvent::STATUS_WARNING,
            CRefString(CRefString().Format("Failed to find any valid custom tiles in set '%s'", szTileSetStr)),
            ptCommand->nCommandType,
            DRLG_ERROR_RESOLUTION_TILES_OMITTED,
            ptTileInfo->nTileType,
            nCommandIndex,
            DRLG_STAGE_GENERATION);


        m_DRLGEvents.push_back(tEvent);






        return;
    }

    int32 arDirWeights[DRLG_EXIT_CNT];
    int32 nTileType;
    DRLGExitDirection nExitDirectionTaken = DRLG_EXIT_NONE;
    size_t nNumFound = arCustomTiles.size();


    DRLGTile * ptTile = NULL;
    DRLGTileConstraint tConstraint;
    while (!arCustomTiles.empty())
    {
        i = PickRand((int32)arCustomTiles.size());
        ptTileInfo = arCustomTiles[i];
        nTileCells = ptTileInfo->tCustomTileInfo.nSizeX * ptTileInfo->tCustomTileInfo.nSizeY;
        nAnchorIndex = ptTileInfo->tCustomTileInfo.pointAnchor.x +
            ptTileInfo->tCustomTileInfo.pointAnchor.y * ptTileInfo->tCustomTileInfo.nSizeX;

        CustomTileCell & tCustomTileCell = ptTileInfo->tCustomTileInfo.arTiles[nAnchorIndex];
        nTileType = tCustomTileCell.nTileType;

        arDirWeights[DRLG_EXIT_NORTH] = TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_WEIGHT_NORTH);
        arDirWeights[DRLG_EXIT_SOUTH] = TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_WEIGHT_SOUTH);
        arDirWeights[DRLG_EXIT_EAST] = TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_WEIGHT_EAST);
        arDirWeights[DRLG_EXIT_WEST] = TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_WEIGHT_WEST);
        sZeroExitWeightsForTileType(arDirWeights, tCustomTileCell.nTileType);

        tConstraint.ptFilterCommand = GetParentTileCommand();
        tConstraint.ptTileInfo = &ptTileInfo->tCustomTileInfo;
        PushConstraint(tConstraint);
        ptTile = AddTile(arDirWeights, ptCommand, nTileType, tCustomTileCell.nTileSet, &nExitDirectionTaken);
        PopConstraint();

        if (ptTile)
        {
            break;
        }



        blzext::vector_erase_at(arCustomTiles, i);
    }


    if (!ptTile)
    {
        DRLGTestEvent tEvent(
            m_dwSeed,
            m_bWarningsAsErrors ? DRLGTestEvent::STATUS_ERROR : DRLGTestEvent::STATUS_WARNING,
            CRefString(CRefString().Format("Unable to fit any of the %d custom tiles anywhere.", nNumFound)),
            ptCommand->nCommandType,
            DRLG_ERROR_RESOLUTION_TILES_OMITTED,
            ptTileInfo->nTileType,
            nCommandIndex,
            DRLG_STAGE_GENERATION);



        m_DRLGEvents.push_back(tEvent);







        return;
    }

    CustomTileCell & tCustomTileCell = ptTileInfo->tCustomTileInfo.arTiles[nAnchorIndex];
    int32 tx = ptTile->nXCoord;
    int32 ty = ptTile->nYCoord;

    tx -= ptTileInfo->tCustomTileInfo.pointAnchor.x;
    ty -= ptTileInfo->tCustomTileInfo.pointAnchor.y;

    ptTile->nHeight -= tCustomTileCell.arExitHeights[sOppositeDirection(nExitDirectionTaken)];

    for (int32 index = 0; index < DRLG_EXIT_CNT; index++)
    {
        ptTile->arRelativeExitHeights[index] = tCustomTileCell.arExitHeights[index];
    }

    if (tCustomTileCell.snoScene != SNO_NONE)
    {
        ptTile->snoName.Set(SCENE_SNO, tCustomTileCell.snoScene);

        if (m_bUseEdgeTypes)
        {
            uint32 dwAdjacencyMask = DRLG_TILE_NSEW;
            if (ptTileInfo->tCustomTileInfo.pointAnchor.x <= 0) dwAdjacencyMask &= ~DRLG_TILE_N;
            if (ptTileInfo->tCustomTileInfo.pointAnchor.x >= ptTileInfo->tCustomTileInfo.nSizeX - 1) dwAdjacencyMask &= ~DRLG_TILE_S;
            if (ptTileInfo->tCustomTileInfo.pointAnchor.y >= ptTileInfo->tCustomTileInfo.nSizeY - 1) dwAdjacencyMask &= ~DRLG_TILE_E;
            if (ptTileInfo->tCustomTileInfo.pointAnchor.y <= 0) dwAdjacencyMask &= ~DRLG_TILE_W;

            FinalizeEdgeTypes(ptTile, dwAdjacencyMask, NULL);
        }
    }



    int32 nAnchorHeight = ptTile->nHeight - tCustomTileCell.nTileHeight;

    for (int32 y = 0; y < ptTileInfo->tCustomTileInfo.nSizeY; y++)
    {
        for (int32 x = 0; x < ptTileInfo->tCustomTileInfo.nSizeX; x++)
        {
            int32 nIdx = y * ptTileInfo->tCustomTileInfo.nSizeX + x;
            CustomTileCell & tCustomCell = ptTileInfo->tCustomTileInfo.arTiles[nIdx];
            DRLGTileType nTileTypeLoc = (DRLGTileType)tCustomCell.nTileType;
            GridDataTileSets nTileSetLoc = tCustomCell.nTileSet;

            if (tCustomCell.bEnabled &&
                GetTileAtLoc(tx + x, ty + y) == NULL)
            {
                DRLGTile * ptTileLoc = AllocTile(ptCommand, NULL, tx + x, ty + y, nTileTypeLoc, nTileSetLoc, nAnchorHeight + tCustomCell.nTileHeight, tCustomCell.arExitHeights, FALSE);

                if (tCustomCell.snoScene != SNO_NONE)
                {
                    ptTileLoc->snoName.Set(SCENE_SNO, tCustomCell.snoScene);

                    if (m_bUseEdgeTypes)
                    {
                        uint32 dwAdjacencyMask = DRLG_TILE_NSEW;
                        if (x == 0) dwAdjacencyMask &= ~DRLG_TILE_N;
                        if (x >= ptTileInfo->tCustomTileInfo.nSizeX - 1) dwAdjacencyMask &= ~DRLG_TILE_S;
                        if (y >= ptTileInfo->tCustomTileInfo.nSizeY - 1) dwAdjacencyMask &= ~DRLG_TILE_E;
                        if (y == 0) dwAdjacencyMask &= ~DRLG_TILE_W;

                        FinalizeEdgeTypes(ptTileLoc, dwAdjacencyMask, NULL);
                    }
                }
            }
        }
    }
}


DRLGTile * DRLGDungeonGenerator::sAddPerimeterTile(DRLGTile * ptParentTile, DRLGExitDirection nExitDirection, DRLGCommand * ptCommand, GridDataTileSets nTileSet, BOOL bFlagPerimeter, BOOL bForceConnect, int32 nOriginalTileCount, DRLGEdgeType eEdgeType)
{



    DRLGTile * ptNewTile = AddTile(ptParentTile, nExitDirection, ptCommand, DRLG_TILE_NONE, nTileSet, TRUE);
    if (ptNewTile)
    {
        if (!bFlagPerimeter)
        {
            ptNewTile->ptCommand = ptParentTile->ptCommand;
            ptNewTile->nCommandIndex = (int32)(ptParentTile->ptCommand - m_arCommands);
        }



        if (bFlagPerimeter) ptNewTile->dwFlags |= (1 << DRLG_TILE_FLAG_PERIMETER_TILE); else ptNewTile->dwFlags &= ~(1 << DRLG_TILE_FLAG_PERIMETER_TILE);


        SetEdgeType(ptParentTile, nExitDirection, eEdgeType, FALSE);

        DRLGExitDirection nParentDir = sOppositeDirection(nExitDirection);
        sPerimeterTileInheritLabels(ptCommand, ptNewTile, nParentDir, nOriginalTileCount);
    }
    else if (bForceConnect)
    {


        DRLGTile * ptNeighborTile = ptParentTile->GetNeighbor(nExitDirection);
        if (ptNeighborTile)
        {


            if (ptNeighborTile->nCreateId < nOriginalTileCount)
                eEdgeType = DRLG_EDGE_UNDEFINED;



            ConnectTiles(ptParentTile, ptNeighborTile, nExitDirection, eEdgeType);
        }
    }


    return ptNewTile;
}


void DRLGDungeonGenerator::sPerimeterTileMergeLabels(DRLGTile * ptNewTile, DRLGTile * ptSourceTile, BOOL bInheritGroupInfo, DRLGCommand * ptCommand)
{
    int32 nLabelsCount = 0;
    for (int32 l = 0; l < 4; l++)
    {
        if (ptNewTile->arLabels[l] == DRLG_LABEL_NONE)
            break;
        nLabelsCount++;
    }
    for (int32 l = 0; l < 4; l++)
    {
        DRLGLabelType nLabel = ptSourceTile->arLabels[l];
        if (nLabel == DRLG_LABEL_NONE)
            break;
        BOOL bLabelFound = FALSE;
        for (int32 l2 = 0; l2 < nLabelsCount; l2++)
        {
            if (ptNewTile->arLabels[l2] == nLabel)
            {
                bLabelFound = TRUE;
                break;
            }
        }
        if (!bLabelFound && nLabelsCount < 4)
            ptNewTile->arLabels[nLabelsCount++] = nLabel;
    }
    if (bInheritGroupInfo && (ptCommand == NULL || TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_OVERRIDE_GROUP_NUM) < 0))
        ptNewTile->nGroup = ptSourceTile->nGroup;
}









































void DRLGDungeonGenerator::sPerimeterTileInheritLabels(DRLGCommand * ptCommand, DRLGTile * ptNewTile, DRLGExitDirection nParentDirection, int32 nOldTileCount)
{
    BOOL bGroupInfoInherited = FALSE;
    for (int32 d = 0; d < DRLG_EXIT_CNT; d++)
    {
        DRLGExitDirection nNeighborDir = (DRLGExitDirection)((nParentDirection + d) % DRLG_EXIT_CNT);
        DRLGTile * ptNeighborTile = ptNewTile->GetNeighbor(nNeighborDir);
        if (ptNeighborTile && ptNeighborTile->nCreateId < nOldTileCount)
        {
            sPerimeterTileMergeLabels(ptNewTile, ptNeighborTile, !bGroupInfoInherited, ptCommand);
            bGroupInfoInherited = TRUE;
        }
    }
    DRLGTile * ptParentTile = ptNewTile->GetNeighbor(nParentDirection);
    if (!bGroupInfoInherited)
        ptNewTile->nGroup = ptParentTile->nGroup;
}














BOOL DRLGDungeonGenerator::sValidForChokepointExcludeDist(
    DRLGTileConstraint * ptConstraint,
    int32 nTileX,
    int32 nTileY)
{


    switch (ptConstraint->eChokepointType)
    {
    case DRLG_CHOKEPOINT_NORTH_SOUTH:
        {
            if (nTileX >= ptConstraint->nChokepointX && nTileX < ptConstraint->nChokepointX + ptConstraint->nChokepointLength)
            {
                int32 nDistFromChokepoint = ptConstraint->nChokepointY - nTileY;
                if (nDistFromChokepoint < 0)
                {
                    nDistFromChokepoint = nTileY - (ptConstraint->nChokepointY + ptConstraint->nChokepointWidth - 1);
                }

                if (ptConstraint->nChokepointExcludeDist <= 0 || nDistFromChokepoint <= ptConstraint->nChokepointExcludeDist)
                {
                    return FALSE;
                }
            }
        }
        break;

    case DRLG_CHOKEPOINT_EAST_WEST:
        {
            if (nTileY >= ptConstraint->nChokepointY && nTileY < ptConstraint->nChokepointY + ptConstraint->nChokepointLength)
            {
                int32 nDistFromChokepoint = ptConstraint->nChokepointX - nTileX;
                if (nDistFromChokepoint < 0)
                {
                    nDistFromChokepoint = nTileX - (ptConstraint->nChokepointX + ptConstraint->nChokepointWidth - 1);
                }

                if (ptConstraint->nChokepointExcludeDist <= 0 || nDistFromChokepoint <= ptConstraint->nChokepointExcludeDist)
                {
                    return FALSE;
                }
            }
        }
        break;
    }

    return TRUE;
}







BOOL DRLGDungeonGenerator::sValidForChokepointExcludeDist(
    int32 nTileX,
    int32 nTileY)
{
    DRLGTileConstraint * ptConstraint = m_arTileConstraints;
    for (int32 i = 0; i < m_nTileConstraintsCnt; i++, ptConstraint++)
    {
        if (ptConstraint->eChokepointType != DRLG_CHOKEPOINT_NONE)
        {
            if (!sValidForChokepointExcludeDist(ptConstraint, nTileX, nTileY))
            {
                return FALSE;
            }
        }
    }
    return TRUE;
}


// NON_MATCHING: 2528 vs 2516 B; cleanup spill/register allocation and mask scheduling differ (208-byte vs 192-byte stack frame).
void DRLGDungeonGenerator::CmdAddPerimeter(DRLGCommand * ptCommand)
{
    ScopedDRLGWarningAsErrors warningsAsErrors(this, ptCommand);



    GridDataTileSets nTileSet = TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_TILE_SET);
    DRLGEdgeType eEdgeType = TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_EDGE_TYPE);


    DRLGCommand * ptParentCommand = GetParentTileCommand();


    int32 nTileCount = m_listTiles.Count();
    POS pos = m_listTiles.GetHeadPosition();


    TPDRLGTileList listPerimeterTiles(sDRLGDungeonGetPtrTilePool());

    for (int32 i = 0; pos && i < nTileCount; i++)
    {
        DRLGTile & tTile = m_listTiles.GetNext(pos);


        if (ptParentCommand && tTile.ptCommand != ptParentCommand)
            continue;




        if (TagMapGetInt(tTile.ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_DOESNT_GENERATE_PERIMETER))
            continue;




        if (tTile.IsPerimeterTile())
            continue;



        for (int32 j = 0; j < DRLG_EXIT_CNT; j++)
        {

            int32 nIgnoreBorderDir = -1;
            int32 nConnectedBorderTilesCount = 0;

            if (eEdgeType == DRLG_EDGE_BORDER && tTile.GetEdgeType(j) == DRLG_EDGE_BORDER)
            {
                DRLGExitDirection nPerpendicularDirs[2] = { sRotateExitDirectionLeft(j), sRotateExitDirectionRight(j) };


                for (int32 n = 0; n < 2; n++)
                {

                    DRLGTile * ptNeighborTile = tTile.GetNeighbor(nPerpendicularDirs[n]);
                    if (!ptNeighborTile || ptNeighborTile->nCreateId >= nTileCount)
                        continue;




                    DRLGTile * ptNeighborNeighborTile = ptNeighborTile->GetNeighbor(j);
                    if (ptNeighborNeighborTile && ptNeighborNeighborTile->nCreateId < nTileCount)
                        continue;




                    if (ptNeighborTile->GetEdgeType(j) == DRLG_EDGE_BORDER)
                    {
                        nIgnoreBorderDir = nPerpendicularDirs[n];
                        nConnectedBorderTilesCount++;
                    }
                }
            }

            if (nConnectedBorderTilesCount > 1)
                continue;




            DRLGTile * ptNewTile = sAddPerimeterTile(&tTile, j, ptCommand, nTileSet, ptParentCommand == NULL, FALSE, nTileCount, DRLG_EDGE_UNDEFINED);
            if (!ptNewTile)
            {

                DRLGTile * ptNeighborTile = tTile.GetNeighbor(j);
                if (!ptNeighborTile || ptNeighborTile->nCreateId < nTileCount)
                    continue;


                ptNewTile = ptNeighborTile;
                ConnectTiles(&tTile, ptNewTile, j, DRLG_EDGE_UNDEFINED);
            }
            else
            {

                listPerimeterTiles.AddTail_Tracked(ptNewTile);


                if (eEdgeType != DRLG_EDGE_UNDEFINED && tTile.GetEdgeType(j) == eEdgeType)
                {
                    ConnectTiles(&tTile, ptNewTile, j, eEdgeType);


                    if (nIgnoreBorderDir != -1)
                    {
                        ptNewTile->nAllowedExits &= ~(1 << nIgnoreBorderDir);
                    }
                }
            }




            switch (j)
            {
                case DRLG_EXIT_NORTH:
                case DRLG_EXIT_SOUTH:
                    if (nIgnoreBorderDir != DRLG_EXIT_EAST)
                    {
                        DRLGTile * ptPerimeterTile = sAddPerimeterTile(ptNewTile, DRLG_EXIT_EAST, ptCommand, nTileSet, ptParentCommand == NULL, TRUE, nTileCount, eEdgeType);
                        if (ptPerimeterTile)
                        {
                            sPerimeterTileMergeLabels(ptPerimeterTile, ptNewTile, TRUE, ptCommand);
                            listPerimeterTiles.AddTail_Tracked(ptPerimeterTile);
                        }
                    }

                    if (nIgnoreBorderDir != DRLG_EXIT_WEST)
                    {
                        DRLGTile * ptPerimeterTile = sAddPerimeterTile(ptNewTile, DRLG_EXIT_WEST, ptCommand, nTileSet, ptParentCommand == NULL, TRUE, nTileCount, eEdgeType);
                        if (ptPerimeterTile)
                        {
                            sPerimeterTileMergeLabels(ptPerimeterTile, ptNewTile, TRUE, ptCommand);
                            listPerimeterTiles.AddTail_Tracked(ptPerimeterTile);
                        }
                    }
                    break;

                case DRLG_EXIT_EAST:
                case DRLG_EXIT_WEST:
                    if (nIgnoreBorderDir != DRLG_EXIT_NORTH)
                    {
                        DRLGTile * ptPerimeterTile = sAddPerimeterTile(ptNewTile, DRLG_EXIT_NORTH, ptCommand, nTileSet, ptParentCommand == NULL, TRUE, nTileCount, eEdgeType);
                        if (ptPerimeterTile)
                        {
                            sPerimeterTileMergeLabels(ptPerimeterTile, ptNewTile, TRUE, ptCommand);
                            listPerimeterTiles.AddTail_Tracked(ptPerimeterTile);
                        }
                    }

                    if (nIgnoreBorderDir != DRLG_EXIT_SOUTH)
                    {
                        DRLGTile * ptPerimeterTile = sAddPerimeterTile(ptNewTile, DRLG_EXIT_SOUTH, ptCommand, nTileSet, ptParentCommand == NULL, TRUE, nTileCount, eEdgeType);
                        if (ptPerimeterTile)
                        {
                            sPerimeterTileMergeLabels(ptPerimeterTile, ptNewTile, TRUE, ptCommand);
                            listPerimeterTiles.AddTail_Tracked(ptPerimeterTile);
                        }
                    }
                    break;
            }
        }
    }



    if (eEdgeType == DRLG_EDGE_BORDER)
    {
        while (!listPerimeterTiles.IsEmpty())
        {
            DRLGTile * ptPerimeterTile = listPerimeterTiles.GetHead();

            DRLGExitDirection nLastBorderEdgeDir = DRLG_EXIT_NONE;
            int32 nBorderEdgesCount = 0;
            for (int32 e = 0; e < DRLG_EXIT_CNT; e++)
            {
                if (ptPerimeterTile->GetEdgeType(e) == DRLG_EDGE_BORDER)
                {
                    nLastBorderEdgeDir = e;
                    nBorderEdgesCount++;
                }
            }


            if (nBorderEdgesCount == 1)
            {
                if (ptPerimeterTile->ExitExists(nLastBorderEdgeDir))
                {
                    listPerimeterTiles.AddTail_Tracked(ptPerimeterTile->arNeighbors[nLastBorderEdgeDir]);
                }

                SetEdgeType(ptPerimeterTile, nLastBorderEdgeDir, DRLG_EDGE_UNDEFINED, TRUE);
            }

            else if (nBorderEdgesCount > 2)
            {
                for (int32 e = 0; e < DRLG_EXIT_CNT; e++)
                {
                    if (ptPerimeterTile->GetEdgeType(e) == DRLG_EDGE_BORDER)
                    {
                        SetEdgeType(ptPerimeterTile, e, DRLG_EDGE_UNDEFINED, TRUE);
                    }
                }
            }
            listPerimeterTiles.RemoveHead();
        }
    }
}


void DRLGDungeonGenerator::CmdAddFields(
    DRLGCommand * ptCommand)
{
    ScopedDRLGWarningAsErrors warningsAsErrors(this, ptCommand);

    int32 nRoomCnt = TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_ROOM_CNT);
    int32 nRoomCntDelta = TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_ROOM_CNT_DELTA);
    nRoomCnt += PickRandRange(0, nRoomCntDelta);

    GridDataTileSets nTileSet = TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_TILE_SET);
    DRLGTileType nTileType = sGetCommandRandomTileType(ptCommand, *m_ptRNG);
    DRLGShapeType nShapeType = TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_SHAPE_TYPE);
    float flShapeParam = TagMapGetFloat(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_SHAPE_PARAM);
    DRLGEdgeType eEdgeType = TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_EDGE_TYPE);

    int32 arDirWeights[DRLG_EXIT_CNT];
    arDirWeights[DRLG_EXIT_NORTH] = TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_WEIGHT_NORTH);
    arDirWeights[DRLG_EXIT_SOUTH] = TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_WEIGHT_SOUTH);
    arDirWeights[DRLG_EXIT_EAST] = TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_WEIGHT_EAST);
    arDirWeights[DRLG_EXIT_WEST] = TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_WEIGHT_WEST);


    DRLGTileConstraint tConstraint;
    tConstraint.ptFilterCommand = GetParentTileCommand();
    PushConstraint(tConstraint);
    DRLGTile * ptTile = AddTile(arDirWeights, ptCommand, nTileType, nTileSet, NULL);
    PopConstraint();

    if (ptTile)
    {
        tConstraint.ptFilterCommand = ptCommand;
        PushConstraint(tConstraint);







        float flOriginX = (float)ptTile->nXCoord;
        float flOriginY = (float)ptTile->nYCoord;

        for (int32 i = 1; i < nRoomCnt; i++)
        {
            ptTile = AddTileForShape(ptCommand, nTileType, nTileSet, nShapeType, flShapeParam, flOriginX, flOriginY);
            if (!ptTile)
                break;






            for (int32 j = 0; j < DRLG_EXIT_CNT; j++)
            {
                if (ptTile->ExitExists(j))
                {
                    SetEdgeType(ptTile, j, eEdgeType, FALSE);
                }
            }



            flOriginX = (flOriginX * i + ptTile->nXCoord) / (i + 1);
            flOriginY = (flOriginY * i + ptTile->nYCoord) / (i + 1);
        }

        if (!ptTile)
        {
            int32 nCommandIndex = (int32)(ptCommand - m_arCommands);

            DRLGTestEvent tEvent(
                m_dwSeed,
                m_bWarningsAsErrors ? DRLGTestEvent::STATUS_ERROR : DRLGTestEvent::STATUS_WARNING,
                CRefString("Failed to add specified number of tiles for fields."),
                ptCommand->nCommandType,
                DRLG_ERROR_RESOLUTION_TILES_OMITTED,
                nTileType,
                nCommandIndex,
                DRLG_STAGE_GENERATION);

            m_DRLGEvents.push_back(tEvent);
        }
        PopConstraint();
    }







}

// NON_MATCHING: same 3672 bytes; list head/count initializations are scheduled in a different order.
void DRLGDungeonGenerator::CmdAddRoad(DRLGCommand * ptCommand)
{
    ScopedDRLGWarningAsErrors warningsAsErrors(this, ptCommand);

    DRLGEdgeType nEdgeType = (DRLGEdgeType)TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_EDGE_TYPE);
    int32 nMinLength = TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_ROAD_LENGTH_MIN);

    GridDataTileSets nTileSet = (GridDataTileSets)TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_TILE_SET);
    GridDataTileSets nStartTileSet = nTileSet, nEndTileSet = nTileSet;


    if (TagMapTagExists(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_TILE_SET_ENTRANCE))
    {
        nStartTileSet = (GridDataTileSets)TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_TILE_SET_ENTRANCE);
    }
    if (TagMapTagExists(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_TILE_SET_EXIT))
    {
        nEndTileSet = (GridDataTileSets)TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_TILE_SET_EXIT);
    }



    SetEdgeTypes();



    TPDRLGTileList listStartTiles(sDRLGDungeonGetPtrTilePool());
    TPDRLGTileList listDestinationTiles(sDRLGDungeonGetPtrTilePool());
    {
        ScopedWorldDefinitionLockR pdefWorld(m_snoWorld);

        POS pos = m_listTiles.GetHeadPosition();
        while (pos)
        {
            DRLGTile & tTile = m_listTiles.GetNext(pos);


            tTile.nFilterFlags = 0;


            TListTiles listTiles(sDRLGDungeonGetListTilePool());
            DRLGTileType nTileType = tTile.nCurrentExits;
            GetEligibleListForTile(pdefWorld, &tTile, nTileType, nStartTileSet, listTiles, DRLG_TILE_FILTER_ROAD_START, nEdgeType);

            if (listTiles.Count() > 0)
            {
                listStartTiles.AddTail_Tracked(&tTile);
            }


            listTiles.RemoveAll();
            GetEligibleListForTile(pdefWorld, &tTile, nTileType, nEndTileSet, listTiles, DRLG_TILE_FILTER_ROAD_DESTINATION, nEdgeType);

            if (listTiles.Count() > 0)
            {
                listDestinationTiles.AddTail_Tracked(&tTile);
            }


            listTiles.RemoveAll();
            GetEligibleListForTile(pdefWorld, &tTile, nTileType, nTileSet, listTiles, DRLG_TILE_FILTER_ROAD_LINK, nEdgeType);
        }
    }

    while (TRUE)
    {
        if (listStartTiles.Count() == 0)
        {

            int32 nCommandIndex = (int32)(ptCommand - m_arCommands);

            DRLGTestEvent tEvent(
                m_dwSeed,
                m_bWarningsAsErrors ? DRLGTestEvent::STATUS_ERROR : DRLGTestEvent::STATUS_WARNING,
                CRefString("Failed to add road of specified length."),
                ptCommand->nCommandType,
                DRLG_ERROR_RESOLUTION_TILES_OMITTED,
                DRLG_TILE_NONE,
                nCommandIndex,
                DRLG_STAGE_GENERATION);

            m_DRLGEvents.push_back(tEvent);







            return;
        }

        {

            DRLGTile * ptStartTile = NULL;
            int32 nIndex = PickRand(listStartTiles.Count());
            POS pos = listStartTiles.GetHeadPosition();
            POS posCur = pos;
            while (pos && nIndex >= 0)
            {
                posCur = pos;
                ptStartTile = listStartTiles.GetNext(pos);
                nIndex--;
            }




            listStartTiles.RemoveAt(posCur);



            ComputeRoomPaths(ptStartTile);

        }

        {

            DRLGTile * ptEndTile = NULL;
            int32 nLongestDistance = 0;
            int32 nLongestCount = 0;
            POS pos = listDestinationTiles.GetHeadPosition();
            while (pos)
            {
                DRLGTile * ptTile = listDestinationTiles.GetNext(pos);

                if (ptTile->nDistance > nLongestDistance)
                {
                    nLongestDistance = ptTile->nDistance;
                    nLongestCount = 1;
                    ptEndTile = ptTile;
                }
                else if (nLongestDistance > 0 && ptTile->nDistance == nLongestDistance)
                {
                    if (PickRand(nLongestCount++) == 0)
                    {
                        ptEndTile = ptTile;
                    }
                }
            }

            if (!ptEndTile || nLongestDistance < nMinLength)
            {
                continue;
            }




            DRLGTile * ptTile = ptEndTile;
            ptTile->nTileSet = nEndTileSet;
            ptTile->dwFlags |= (1 << DRLG_TILE_FLAG_ROAD_TILE);



            while (ptTile->nDistance > 0)
            {

                DRLGExitDirection nExitDirection = DRLG_EXIT_NONE;
                int32 nExitCount = 0;
                for (int32 i = 0; i < DRLG_EXIT_CNT; i++)
                {
                    DRLGTile * ptNeighborTile = ptTile->arNeighbors[i];
                    if (ptNeighborTile && (ptTile->nComputedPaths & (1 << i)))
                    {
                        if (PickRand(++nExitCount) == 0)
                        {
                            nExitDirection = (DRLGExitDirection)i;
                        }
                    }
                }

                if (nExitDirection == DRLG_EXIT_NONE)
                {
                    break;
                }

                DRLGTile * ptPreviousTile = ptTile->arNeighbors[nExitDirection];
                ConnectTiles(ptTile, ptPreviousTile, nExitDirection, nEdgeType);
                ptTile = ptPreviousTile;
                ptTile->nTileSet = nTileSet;
                ptTile->dwFlags |= (1 << DRLG_TILE_FLAG_ROAD_TILE);
            }





            if (ptTile->nDistance > 0)
            {
                continue;
            }


            ptTile->nTileSet = nStartTileSet;
            ptTile->dwFlags |= (1 << DRLG_TILE_FLAG_ROAD_TILE);
            break;
        }
    }


}

// NON_MATCHING: 4024/4024 bytes; 2/1006 instructions differ: in the empty m_listTiles path after the first loop (+0x5a0) the zeroing of listExitTiles.m_nCount ([sp,#0x30]) is scheduled one slot before `mov x8, xzr`; every other instruction matches.
void DRLGDungeonGenerator::CmdReplaceEntranceExit(DRLGCommand * ptCommand)
{
    ScopedDRLGWarningAsErrors warningsAsErrors(this, ptCommand);

    GridDataTileSets nEntranceTileSet = (GridDataTileSets)TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_TILE_SET_ENTRANCE);
    GridDataTileSets nExitTileSet = (GridDataTileSets)TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_TILE_SET_EXIT);
    int32 nMinDistance = TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_MIN_DISTANCE);
    int32 nMaxDistance = TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_MAX_DISTANCE);

    DRLGLabelType nEntranceLabel = (DRLGLabelType)TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_SUBSTITUTE_ENTRANCE_LABEL);
    DRLGLabelType nExitLabel = (DRLGLabelType)TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_SUBSTITUTE_EXIT_LABEL);

    int32 nEntranceOverrideGroupId = TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_SUBSTITUTE_ENTRANCE_OVERRIDE_GROUP_NUM);
    int32 nExitOverrideGroupId = TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_SUBSTITUTE_EXIT_OVERRIDE_GROUP_NUM);


    // Inference: original dungeon-finder server/final-world tests choose set301.
    if (IsDungeonFinder() && GameActiveGameCommonDataIsServer())
    {
        if (WorldGetDungeonFinderWorldFromLevel(SWorldGetNumDungeonFinderLevels()) == WorldCommonGetWorldSNO(m_idSWorld))
        {
            nExitTileSet = 301;
        }
    }



    SetEdgeTypes();

    ScopedWorldDefinitionLockR pdefWorld(m_snoWorld);


    TPDRLGTileList listEntranceTiles(sDRLGDungeonGetPtrTilePool());
    TPDRLGTileList listExitTiles(sDRLGDungeonGetPtrTilePool());
    {
        POS pos = m_listTiles.GetHeadPosition();
        while (pos)
        {
            DRLGTile & tTile = m_listTiles.GetNext(pos);
            DRLGTileType nTileType = (DRLGTileType)tTile.nCurrentExits;

            TListTiles listTiles(sDRLGDungeonGetListTilePool());

            if (nEntranceLabel == DRLG_LABEL_NONE || tTile.HasLabel(nEntranceLabel))
            {

                GetEligibleListForTile(pdefWorld, &tTile, nTileType, nEntranceTileSet, listTiles, DRLG_TILE_FILTER_PLACE_NORMAL_TILES, 0);

                if (listTiles.Count() > 0)
                {
                    listEntranceTiles.AddTail_Tracked(&tTile);
                }
            }

            if (nExitLabel == DRLG_LABEL_NONE || tTile.HasLabel(nExitLabel))
            {

                listTiles.RemoveAll();
                GetEligibleListForTile(pdefWorld, &tTile, nTileType, nExitTileSet, listTiles, DRLG_TILE_FILTER_PLACE_NORMAL_TILES, 0);

                if (listTiles.Count() > 0)
                {
                    listExitTiles.AddTail_Tracked(&tTile);
                }
            }
        }
    }
    // Inference: retry remaining entrance candidates until a valid pair is selected or the list is exhausted.
    while (TRUE)
    {
        if (listEntranceTiles.IsEmpty())
        {

            int32 nCommandIndex = (uint32)(ptCommand - m_arCommands);

            DRLGTestEvent tEvent(
                m_dwSeed,
                m_bWarningsAsErrors ? DRLGTestEvent::STATUS_ERROR : DRLGTestEvent::STATUS_WARNING,
                CRefString("Failed to substitute an entrance and exit of specified type at the specified distance apart."),
                ptCommand->nCommandType,
                DRLG_ERROR_RESOLUTION_TILES_OMITTED,
                DRLG_TILE_NONE,
                nCommandIndex,
                DRLG_STAGE_GENERATION);

            m_DRLGEvents.push_back(tEvent);







            return;
        }

        // Inference: NULL is the original no-selection value before the bounded random list scan.
        DRLGTile * ptEntranceTile = NULL;
        {
            int32 nIndex = PickRand(listEntranceTiles.Count());
            POS pos = listEntranceTiles.GetHeadPosition(); // Inference: posCur preserves this iterator before GetNext, with no separate initial-copy instruction.
            POS posCur = pos;
            while (pos && nIndex >= 0)
            {
                posCur = pos;
                ptEntranceTile = listEntranceTiles.GetNext(pos);
                --nIndex;
            }




            listEntranceTiles.RemoveAt(posCur);

            // Inference: DWARF block {nIndex,pos,posCur} ends after the ComputeRoomDistances call (high_pc 0x6fa1e8) and the call row is 8027:35, so the call sits inside the block, qualified as at 6058.
            DRLGDungeonGenerator::ComputeRoomDistances(ptEntranceTile);
        }


        TListTiles listEntranceTilesLoc(sDRLGDungeonGetListTilePool());
        DRLGTileType nEntranceTileTypeIt = (DRLGTileType)ptEntranceTile->nCurrentExits;
        GetEligibleListForTile(pdefWorld, ptEntranceTile, nEntranceTileTypeIt, nEntranceTileSet, listEntranceTilesLoc, DRLG_TILE_FILTER_PLACE_NORMAL_TILES, 0);

        // Inference: exit selection starts NULL and reservoir count starts zero, as original values.
        DRLGTile * ptExitTile = NULL;
        {
            int32 nValidCount = 0;
            POS pos = listExitTiles.GetHeadPosition();
            while (pos)
            {
                DRLGTile * ptTile = listExitTiles.GetNext(pos);


                TListTiles listExitTilesLoc(sDRLGDungeonGetListTilePool());
                DRLGTileType nExitTileTypeLoc = (DRLGTileType)ptTile->nCurrentExits;
                GetEligibleListForTile(pdefWorld, ptTile, nExitTileTypeLoc, nExitTileSet, listExitTilesLoc, DRLG_TILE_FILTER_PLACE_NORMAL_TILES, 0);


                // Inference: spacing passes until a conflicting entrance with no legal exit combination is found.
                BOOL bPassesSpecialTileSpacing = TRUE;
                POS posEntranceTiles = listEntranceTilesLoc.GetHeadPosition();
                while (posEntranceTiles)
                {
                    const DungeonTileChooser & tEntranceTileChooser = listEntranceTilesLoc.GetNext(posEntranceTiles);


                    int32 nEntranceSceneSizeX, nEntranceSceneSizeY;
                    sGetTileDimensions(*tEntranceTileChooser.ptTileInfo, nEntranceSceneSizeX, nEntranceSceneSizeY);

                    IRect2D tEntranceRect;
                    tEntranceRect.left = ptEntranceTile->nXCoord;
                    tEntranceRect.top = ptEntranceTile->nYCoord;
                    if (tEntranceTileChooser.ptTileInfo->nTileType == DRLG_TILE_CUSTOM)
                    {
                        tEntranceRect.left -= tEntranceTileChooser.ptTileInfo->tCustomTileInfo.pointAnchor.x;
                        tEntranceRect.top -= tEntranceTileChooser.ptTileInfo->tCustomTileInfo.pointAnchor.y;
                    }

                    tEntranceRect.right = tEntranceRect.left + (nEntranceSceneSizeX - 1);
                    tEntranceRect.bottom = tEntranceRect.top + (nEntranceSceneSizeY - 1);


                    // Inference: no conflict or valid combination exists until the following exit-shape traversal proves one.
                    BOOL bSpecialTileConflictExists = FALSE; // Inference: the next combination flag starts FALSE until a nonintersecting pair is found.
                    BOOL bValidCombinationExists = FALSE;
                    POS posExitTiles = listExitTilesLoc.GetHeadPosition();
                    while (posExitTiles)
                    {
                        const DungeonTileChooser & tExitTileChooser = listExitTilesLoc.GetNext(posExitTiles);

                        int32 nExitX = ptTile->nXCoord;
                        int32 nExitY = ptTile->nYCoord;
                        if (tExitTileChooser.ptTileInfo->nTileType == DRLG_TILE_CUSTOM)
                        {
                            nExitX -= tExitTileChooser.ptTileInfo->tCustomTileInfo.pointAnchor.x;
                            nExitY -= tExitTileChooser.ptTileInfo->tCustomTileInfo.pointAnchor.y;
                        }

                        IRect2D tInvalidRect;
                        if (sDetermineInvalidSpecialTileRect(tInvalidRect, nExitX, nExitY, *tExitTileChooser.ptTileInfo, *tEntranceTileChooser.ptTileInfo))
                        {
                            bSpecialTileConflictExists = TRUE;
                            if (!IRect2D::Intersect(tEntranceRect, tInvalidRect, NULL))
                            {
                                bValidCombinationExists = TRUE;
                                break;
                            }
                        }
                    }

                    if (bSpecialTileConflictExists && !bValidCombinationExists)
                    {
                        bPassesSpecialTileSpacing = FALSE;
                        break;
                    }
                }

                if (!bPassesSpecialTileSpacing)
                    continue;



                if (ptTile->nDistance >= nMinDistance && (nMaxDistance <= 0 || ptTile->nDistance <= nMaxDistance))
                {
                    if (PickRand(++nValidCount) == 0)
                    {
                        ptExitTile = ptTile;
                    }
                }
            }

            // Inference: the original has no row for this test (line-0 branch after the 8040 latch) and keeps one listEntranceTilesLoc cleanup with dest 4/5 in w20; a continue from inside this block does both, the same test after the block makes SimplifyCFG duplicate the ~XPooledList call.
            if (!ptExitTile)
            {
                continue;
            }
        }

        // Inference: original writes command/tile-set/index and falls back to replacement flag bit4 only on failed swap.
        ptEntranceTile->nTileSet = nEntranceTileSet;
        ptEntranceTile->ptCommand = ptCommand;
        ptEntranceTile->nCommandIndex = (uint32)(ptCommand - m_arCommands);
        if (!CmdSwapForPrePlacedTile(ptCommand, ptEntranceTile, nEntranceTileSet, (DRLGTileType)ptEntranceTile->nCurrentExits, FALSE, TRUE, nEntranceOverrideGroupId))
        {
            ptEntranceTile->dwFlags |= 1 << 4;
        }

        ptExitTile->nTileSet = nExitTileSet;
        ptExitTile->ptCommand = ptCommand;
        ptExitTile->nCommandIndex = (uint32)(ptCommand - m_arCommands);
        if (!CmdSwapForPrePlacedTile(ptCommand, ptExitTile, nExitTileSet, (DRLGTileType)ptExitTile->nCurrentExits, FALSE, TRUE, nExitOverrideGroupId))
        {
            ptExitTile->dwFlags |= 1 << 4;
        }

        break;
    }
}

// NON_MATCHING: 2748 vs 2780 B; vector push_back size/capacity hoisting and register/spill allocation differ.
void DRLGDungeonGenerator::CmdReplaceRandomTile(DRLGCommand * ptCommand)
{
    ScopedDRLGWarningAsErrors warningsAsErrors(this, ptCommand);


    DRLGCommand * ptParentCommand = GetParentTileCommand();


    GridDataTileSets eReplacementTileSet = (GridDataTileSets)TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_TILE_SET);
    int32 nReplacementsMin = TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_ROOM_CNT);
    int32 nReplacementsDelta = TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_ROOM_CNT_DELTA);

    int32 nReplacements = PickRandRange(nReplacementsMin, nReplacementsDelta);
    if (nReplacements <= 0)
    {
        return;
    }

    DRLGLabelType nTileLabel = (DRLGLabelType)TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_LABEL);
    int32 nOverrideGroupId = TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_OVERRIDE_GROUP_NUM);

    int32 nParentCommandIndex = ptParentCommand ? (int32)(ptParentCommand - m_arCommands) : -1;


    BOOL bPlaceNow = FALSE;
    {
        ScopedWorldDefinitionLockR pdefWorld(m_snoWorld);

        for (int32 nTileIndex = 0; nTileIndex < pdefWorld->ptServerData->ptDRLG->arTiles.GetArraySize(); nTileIndex++)
        {
            TileInfo & tTileInfo = pdefWorld->ptServerData->ptDRLG->arTiles[nTileIndex];
            if (tTileInfo.nTileSet != eReplacementTileSet)
            {
                continue;
            }


            int32 nSceneSizeX, nSceneSizeY;
            sGetTileDimensions(tTileInfo, nSceneSizeX, nSceneSizeY);
            if (nSceneSizeX > 1 || nSceneSizeY > 1)
            {
                bPlaceNow = TRUE;
                break;
            }
        }
    }


    if (bPlaceNow)
    {


        SetEdgeTypes();


        blz::vector<DRLGTile *> arValidTiles;
        {
            ScopedWorldDefinitionLockR pdefWorld(m_snoWorld);

            POS pos = m_listTiles.GetHeadPosition();
            while (pos)
            {
                DRLGTile & tTile = m_listTiles.GetNext(pos);
                DRLGTileType nTileType = (DRLGTileType)tTile.nCurrentExits;

                if (nTileLabel != DRLG_LABEL_NONE && !tTile.HasLabel(nTileLabel))
                {
                    continue;
                }

                if (ptParentCommand && tTile.nCommandIndex != nParentCommandIndex)
                {
                    continue;
                }

                TListTiles listTiles(sDRLGDungeonGetListTilePool());
                GetEligibleListForTile(pdefWorld, &tTile, nTileType, eReplacementTileSet, listTiles, DRLG_TILE_FILTER_PLACE_NORMAL_TILES, 0);
                if (listTiles.Count() > 0)
                {
                    arValidTiles.push_back(&tTile);
                }
            }
        }

        int32 nSuccessfulReplacements = 0;
        int32 nValidTilesCount = (int32)arValidTiles.size();
        for (int32 i = 0; nValidTilesCount > 0 && i < nReplacements; i++)
        {

            int32 nIndex = PickRand(nValidTilesCount);
            DRLGTile * ptPlacedTile = arValidTiles[nIndex];


            blzext::vector_erase_at(arValidTiles, nIndex);
            nValidTilesCount--;


            if (i > 0)
            {
                ScopedWorldDefinitionLockR pdefWorld(m_snoWorld);

                TListTiles listTiles(sDRLGDungeonGetListTilePool());
                DRLGTileType nTileType = (DRLGTileType)ptPlacedTile->nCurrentExits;
                GetEligibleListForTile(pdefWorld, ptPlacedTile, nTileType, eReplacementTileSet, listTiles, DRLG_TILE_FILTER_PLACE_NORMAL_TILES, 0);
                if (listTiles.Count() <= 0)
                {
                    continue;
                }
            }


            ptPlacedTile->nTileSet = eReplacementTileSet;
            ptPlacedTile->ptCommand = ptCommand;
            ptPlacedTile->nCommandIndex = (int32)(ptCommand - m_arCommands);
            if (!CmdSwapForPrePlacedTile(ptCommand, ptPlacedTile, eReplacementTileSet, (DRLGTileType)ptPlacedTile->nCurrentExits, FALSE, FALSE, -1))
            {
                ptPlacedTile->dwFlags |= (1 << DRLG_TILE_FLAG_REPLACEMENT_TILE);
            }

            if (nOverrideGroupId >= 0)
            {
                ptPlacedTile->nGroup = nOverrideGroupId;
            }

            nSuccessfulReplacements++;
        }


        if (nSuccessfulReplacements < nReplacementsMin)
        {
            int32 nCommandIndex = (int32)(ptCommand - m_arCommands);

            DRLGTestEvent tEvent(
                m_dwSeed,
                m_bWarningsAsErrors ? DRLGTestEvent::STATUS_ERROR : DRLGTestEvent::STATUS_WARNING,
                CRefString("Failed to immediately substitute minimum number of random tiles."),
                ptCommand->nCommandType,
                DRLG_ERROR_RESOLUTION_TILES_OMITTED,
                DRLG_TILE_NONE,
                nCommandIndex,
                DRLG_STAGE_GENERATION);

            m_DRLGEvents.push_back(tEvent);
        }
    }












    else
    {
        DRLGPendingTileReplacement tPendingReplacement;
        tPendingReplacement.nCommandIndex = (int32)(ptCommand - m_arCommands);
        tPendingReplacement.nParentCommandIndex = nParentCommandIndex;
        tPendingReplacement.nTileLabel = nTileLabel;
        tPendingReplacement.eReplacementTileSet = eReplacementTileSet;
        tPendingReplacement.nOverrideGroupId = nOverrideGroupId;



        for (int32 i = 0; i < nReplacements; i++)
        {
            m_arPendingTileReplacements.push_back(tPendingReplacement);
        }
    }
}


void DRLGDungeonGenerator::CmdAddChokepoint(DRLGCommand * ptCommand)
{
    ScopedDRLGWarningAsErrors warningsAsErrors(this, ptCommand);

    SetEdgeTypes();


    GridDataTileSets eTileSet = (GridDataTileSets)TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_TILE_SET);
    int32 nLength = TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_ROOM_CNT);
    int32 nLengthDelta = TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_ROOM_CNT_DELTA);
    nLength = PickRandRange(nLength, nLengthDelta);
    if (TagMapTagExists(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_HUB_LENGTH))
    {
        nLength = TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_HUB_LENGTH);
        nLengthDelta = TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_HUB_LENGTH_DELTA);
        nLength = PickRandRange(nLength, nLengthDelta);
    }


    int32 nWidth = 1;
    if (TagMapTagExists(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_HUB_WIDTH))
    {
        nWidth = TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_HUB_WIDTH);
        int32 nWidthDelta = TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_HUB_WIDTH_DELTA);
        nWidth = PickRandRange(nWidth, nWidthDelta);
    }

    int32 nExcludeDist = TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_CHOKEPOINT_EXCLUDE_DIST);


    {
        ScopedWorldDefinitionLockR pdefWorld(m_snoWorld);

        for (int32 nTileIndex = 0; nTileIndex < pdefWorld->ptServerData->ptDRLG->arTiles.GetArraySize(); nTileIndex++)
        {
            TileInfo & tTileInfo = pdefWorld->ptServerData->ptDRLG->arTiles[nTileIndex];
            if (eTileSet != GRIDDATA_TILE_SET_NONE && tTileInfo.nTileSet != eTileSet)
            {
                continue;
            }

            int32 nSceneSizeX, nSceneSizeY;
            sGetTileDimensions(tTileInfo, nSceneSizeX, nSceneSizeY);
            if ((nSceneSizeX > 1 || nSceneSizeY > 1) &&
                (nSceneSizeX != nWidth || nSceneSizeY != nLength) &&
                (nSceneSizeX != nLength || nSceneSizeY != nWidth))
            {
                int32 nCommandIndex = (int32)(ptCommand - m_arCommands);

                DRLGTestEvent tEvent(
                    m_dwSeed,
                    DRLGTestEvent::STATUS_ERROR,
                    CRefString("All 'Add Chokepoint' commands must use a tile set with tiles that are 1x1 or sized the same as the chokepoint."),
                    ptCommand->nCommandType,
                    DRLG_ERROR_RESOLUTION_TILES_OMITTED,
                    DRLG_TILE_NONE,
                    nCommandIndex,
                    DRLG_STAGE_GENERATION);


                m_DRLGEvents.push_back(tEvent);







                return;
            }
        }
    }


    int32 arWeights[DRLG_EXIT_CNT] = { 100, 100, 100, 100 };
    DRLGExitDirection nExitDirection = DRLG_EXIT_NONE;

    DRLGTileConstraint tConstraint;
    tConstraint.ptFilterCommand = GetParentTileCommand();
    tConstraint.ptOwnerCommand = ptCommand;
    tConstraint.eChokepointType = DRLG_CHOKEPOINT_PLACEMENT;
    tConstraint.nChokepointLength = nLength;
    tConstraint.nChokepointWidth = nWidth;
    tConstraint.nChokepointExcludeDist = nExcludeDist;
    PushConstraint(tConstraint);
    DRLGTile * ptFirstTile = AddTile(arWeights, ptCommand, DRLG_TILE_NONE, eTileSet, &nExitDirection);
    PopConstraint();
    if (!ptFirstTile)
    {

        int32 nCommandIndex = (int32)(ptCommand - m_arCommands);

        DRLGTestEvent tEvent(
            m_dwSeed,
            DRLGTestEvent::STATUS_ERROR,
            CRefString("Failed to find a valid tile to attach a chokepoint to."),
            ptCommand->nCommandType,
            DRLG_ERROR_RESOLUTION_TILES_OMITTED,
            DRLG_TILE_NONE,
            nCommandIndex,
            DRLG_STAGE_GENERATION);


        m_DRLGEvents.push_back(tEvent);







        return;
    }


    DRLGValidChokepointRotateDir nAllowedRotation = GetValidChokepointRotateDirections(ptFirstTile->nXCoord, ptFirstTile->nYCoord, nLength, nWidth, nExitDirection, eTileSet);



    DRLGTileConstraint * ptConstraint = &m_arTileConstraints[m_nTileConstraintsCnt - 1];
    ptConstraint->ptOwnerCommand = ptCommand;
    ptConstraint->nChokepointX = ptFirstTile->nXCoord;
    ptConstraint->nChokepointY = ptFirstTile->nYCoord;
    ptConstraint->nChokepointLength = nLength;
    ptConstraint->nChokepointWidth = nWidth;
    ptConstraint->nChokepointExcludeDist = nExcludeDist;
    if (nExitDirection == DRLG_EXIT_NORTH || nExitDirection == DRLG_EXIT_SOUTH)
        ptConstraint->eChokepointType = DRLG_CHOKEPOINT_NORTH_SOUTH;
    else
        ptConstraint->eChokepointType = DRLG_CHOKEPOINT_EAST_WEST;






    DRLGTile * ptTile = ptFirstTile;
    DRLGTile * ptTopLeftTile = ptFirstTile;
    for (int32 i = 1; i < nLength; i++)
    {
        DRLGTile * ptNextTile = AddTile(ptTile, nExitDirection, ptCommand, DRLG_TILE_NONE, eTileSet, FALSE);


        ptTile = ptNextTile;

        if (ptTile->nXCoord <= ptTopLeftTile->nXCoord && ptTile->nYCoord <= ptTopLeftTile->nYCoord)
        {
            ptTopLeftTile = ptTile;
            ptConstraint->nChokepointX = ptTile->nXCoord;
            ptConstraint->nChokepointY = ptTile->nYCoord;
        }
    }


    if (nWidth > 1)
    {


        DRLGExitDirection nWidthExitDirection;
        if (nAllowedRotation == DRLG_CHOKEPOINT_ROTATE_DIR_BOTH)
        {
            nWidthExitDirection = PickRand(2) ? sRotateExitDirectionLeft(nExitDirection) : sRotateExitDirectionRight(nExitDirection);
        }
        else
        {
            if (nAllowedRotation == DRLG_CHOKEPOINT_ROTATE_DIR_RIGHT)
            {
                nWidthExitDirection = sRotateExitDirectionRight(nExitDirection);
            }
            else
            {
                nWidthExitDirection = sRotateExitDirectionLeft(nExitDirection);
            }
        }

        for (int32 l = 0; l < nLength; l++)
        {
            int32 nWidthParentX = ptFirstTile->nXCoord + gAddExitX[nExitDirection] * l;
            int32 nWidthParentY = ptFirstTile->nYCoord + gAddExitY[nExitDirection] * l;
            DRLGTile * ptWidthParentTile = GetTileAtLoc(nWidthParentX, nWidthParentY);

            for (int32 w = 1; w < nWidth; w++)
            {
                DRLGTile * ptNextTile = AddTile(ptWidthParentTile, nWidthExitDirection, ptCommand, DRLG_TILE_NONE, eTileSet, FALSE);
                ptWidthParentTile = ptNextTile;
                if (ptNextTile->nXCoord <= ptTopLeftTile->nXCoord && ptNextTile->nYCoord <= ptTopLeftTile->nYCoord)
                {
                    ptTopLeftTile = ptNextTile;
                    ptConstraint->nChokepointX = ptNextTile->nXCoord;
                    ptConstraint->nChokepointY = ptNextTile->nYCoord;
                }




                for (int32 k = 0; k < DRLG_EXIT_CNT; k++)
                {
                    DRLGTile * ptNeighbor = ptNextTile->GetNeighbor(k);
                    if (ptNeighbor)
                    {
                        ConnectTiles(ptNextTile, ptNeighbor, k, DRLG_EDGE_UNDEFINED);
                    }
                }
            }
        }
    }




    TileInfo * ptLargeTileUsed = CmdSwapForPrePlacedTile(ptCommand, ptTopLeftTile, eTileSet, DRLG_TILE_NSEW, TRUE, FALSE, -1);
    if (ptLargeTileUsed)
    {
        DRLGChokepointType nChokepointType = (DRLGChokepointType)TagMapGetInt(ptLargeTileUsed->hTileTagMap, TAG_WORLD_TILE_CHOKEPOINT_TYPE);
        if (nChokepointType != DRLG_CHOKEPOINT_NONE)
        {
            ptConstraint->eChokepointType = nChokepointType;
        }

        DRLGExitDirection nChokepointDir = (DRLGExitDirection)TagMapGetInt(ptLargeTileUsed->hTileTagMap, TAG_WORLD_TILE_CHOKEPOINT_DIRECTION);
        if (nChokepointDir != DRLG_EXIT_NONE)
        {
            ptConstraint->eChokepointDir = sOppositeDirection(nChokepointDir);
        }
    }



    if (ptFirstTile->ptParentTile)
    {
        ptConstraint->eChokepointDir = nExitDirection;
    }

    else if (ptConstraint->eChokepointDir == DRLG_EXIT_NONE)
    {
        if (ptConstraint->eChokepointType == DRLG_CHOKEPOINT_NORTH_SOUTH)
        {
            ptConstraint->eChokepointDir = PickRand(2) ? DRLG_EXIT_NORTH : DRLG_EXIT_SOUTH;
        }
        else
        {
            ptConstraint->eChokepointDir = PickRand(2) ? DRLG_EXIT_EAST : DRLG_EXIT_WEST;
        }
    }
}

// NON_MATCHING: 3872 vs 3824 bytes; frame 0x1b0 vs 0x1c0, the 8741 valid-location loop is rotated differently (original enters it with a branch to the body) and the inlined vector push_back at 8765 spills differently; elsewhere register allocation.
void DRLGDungeonGenerator::CmdAddNubs(
    DRLGCommand * ptCommand)
{
    ScopedDRLGWarningAsErrors warningsAsErrors(this, ptCommand);


    DRLGCommand * ptParentCommand = GetParentTileCommand();


    TPDRLGTileList listPotentialTiles(sDRLGDungeonGetPtrTilePool());
    POS pos = m_listTiles.GetHeadPosition();
    while (pos)
    {
        DRLGTile & tTile = m_listTiles.GetNext(pos);


        if (ptParentCommand && tTile.ptCommand != ptParentCommand)
        {
            continue;
        }

        // Inference: `== 0` spelling at 8597 from the original column 20 for nAllowedExits.
        if ((tTile.nAllowedExits & ~tTile.nCurrentExits) == 0)
        {
            continue;
        }

        listPotentialTiles.AddTail_Tracked(&tTile);
    }


    int32 nPotentialTilesCount = listPotentialTiles.Count();
    if (nPotentialTilesCount <= 0)
    {
        return;
    }


    DRLGTileType nTileType = sGetCommandRandomTileType(ptCommand, *m_ptRNG);


    int32 nTries = TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_NUBS_MAX);
    int32 nTriesDelta = TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_NUBS_DELTA);
    nTries = PickRandRange(nTries, nTriesDelta);


    GridDataTileSets eTileSet = (GridDataTileSets)TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_TILE_SET);
    int32 nNubsSpacing = TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_NUBS_SPACING);
    if (nNubsSpacing < 1)
    {
        nNubsSpacing = 1;
    }


    int32 nNubWidthMin = TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_NUBS_WIDTH_MIN);
    if (nNubWidthMin < 1)
    {
        nNubWidthMin = 1;
    }

    int32 nNubWidthDelta = TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_NUBS_WIDTH_DELTA);
    if (nNubWidthDelta < 0)
    {
        nNubWidthDelta = 0;
    }

    int32 nNubWidthMax = nNubWidthMin + nNubWidthDelta;


    int32 nNubLengthMin = TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_NUBS_LENGTH_MIN);
    if (nNubLengthMin < 1)
    {
        nNubLengthMin = 1;
    }

    int32 nNubLengthDelta = TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_NUBS_LENGTH_DELTA);
    if (nNubLengthDelta < 0)
    {
        nNubLengthDelta = 0;
    }

    int32 nNubLengthMax = nNubLengthMin + nNubLengthDelta;


    int32 nShufflesRemaining = nPotentialTilesCount;
    while (nShufflesRemaining > 1)
    {
        int32 nRandIndex = PickRand(nShufflesRemaining);

        POS posLocal = listPotentialTiles.GetHeadPosition();
        while (posLocal && nRandIndex > 0)
        {
            listPotentialTiles.GetNext(posLocal);
            --nRandIndex;
        }


        listPotentialTiles.MoveToTail(posLocal);
        nShufflesRemaining--;
    }
    // Inference: inner while at 8679 (latch row 8679:28), Count() > 0 tests and prefix ++ from the original rows 8676:34/69, 8685:65, 8705:72, 8709:60, 8741:68, 8826:47.
    for (int32 t = 0; t < nTries && listPotentialTiles.Count() > 0; ++t)
    {
        BOOL bPlacedNub = FALSE;
        while (!bPlacedNub && listPotentialTiles.Count() > 0)
        {
            DRLGTile * ptTile = listPotentialTiles.GetHead();


            int32 nRandStartDir = PickRand(DRLG_EXIT_CNT);
            for (int32 d = 0; d < DRLG_EXIT_CNT && !bPlacedNub; ++d)
            {
                DRLGExitDirection nCurDir = (DRLGExitDirection)((nRandStartDir + d) % DRLG_EXIT_CNT);
                DRLGExitDirection nPerpendicularDirs[2] = { sRotateExitDirectionLeft(nCurDir), sRotateExitDirectionRight(nCurDir) };


                if (!ptTile->CanAddTile(nCurDir, ptTile->GetExitHeight(nCurDir)))
                {
                    continue;
                }


                int32 nNewTileX = ptTile->nXCoord + gAddExitX[nCurDir];
                int32 nNewTileY = ptTile->nYCoord + gAddExitY[nCurDir];


                int32 nHalfWidthCheck = nNubWidthMax + nNubsSpacing - 1;
                int32 nTotalWidthCheck = nHalfWidthCheck * 2 + 1;
                int32 * arMaxValidLengths = (int32 *)SigmaMemoryNew(nTotalWidthCheck * sizeof(int32), 4, NULL, FALSE);

                for (int32 i = -nHalfWidthCheck; i <= nHalfWidthCheck; ++i)
                {
                    int32 & nMaxValidLength = arMaxValidLengths[i + nHalfWidthCheck];
                    nMaxValidLength = nNubLengthMax + nNubsSpacing;
                    for (int32 j = 0; j < nMaxValidLength; ++j)
                    {

                        int32 nCheckTileX = nNewTileX + j * gAddExitX[nCurDir] + i * gAddExitX[nPerpendicularDirs[0]];
                        int32 nCheckTileY = nNewTileY + j * gAddExitY[nCurDir] + i * gAddExitY[nPerpendicularDirs[0]];
                        if (GetTileAtLoc(nCheckTileX, nCheckTileY))
                        {
                            nMaxValidLength = j;
                            break;
                        }


                        int32 arExitHeights[DRLG_EXIT_CNT];
                        SigmaMemoryFill(arExitHeights, 0, sizeof(arExitHeights));
                        if (!CheckHardConstraintsXY(nCheckTileX, nCheckTileY, nTileType, ptTile->GetExitHeight(nCurDir), arExitHeights))
                        {
                            nMaxValidLength = j;
                            break;
                        }

                        if (!CheckSoftConstraints(NULL, nCurDir, FALSE, nCheckTileX, nCheckTileY))
                        {
                            nMaxValidLength = j;
                            break;
                        }
                    }
                }



                int32 nNubLengthMinWithSpacing = nNubLengthMin + nNubsSpacing;
                blz::vector<ValidNubLocation> arValidLocations;
                for (int32 i = nNubsSpacing; i <= nHalfWidthCheck; ++i)
                {

                    int32 nMinValidLength = arMaxValidLengths[i - nNubsSpacing];
                    int32 nMinNubWidthAtOffset = Math::Max(nNubWidthMin, nHalfWidthCheck - i + 1);
                    for (int32 j = i - nNubsSpacing + 1;
                         nMinValidLength >= nNubLengthMinWithSpacing && j < i + nMinNubWidthAtOffset + nNubsSpacing; j++)
                    {


                        nMinValidLength = Math::Min(nMinValidLength, arMaxValidLengths[j]);
                    }

                    if (nMinValidLength < nNubLengthMinWithSpacing)
                    {
                        continue;
                    }



                    ValidNubLocation tValidLocation;
                    tValidLocation.nOffset = i;
                    tValidLocation.nWidth = nMinNubWidthAtOffset;
                    tValidLocation.nMaxLength = nMinValidLength;
                    arValidLocations.push_back(tValidLocation);


                    int32 nCurValidLength = nMinValidLength;
                    int32 nExtraWidthCheckStart = i + nMinNubWidthAtOffset + nNubsSpacing;
                    for (int32 j = nExtraWidthCheckStart;
                         nCurValidLength >= nNubLengthMinWithSpacing && j <= i + nHalfWidthCheck; j++)
                    {

                        nCurValidLength = Math::Min(nCurValidLength, arMaxValidLengths[j]);
                        if (nCurValidLength >= nNubLengthMinWithSpacing)
                        {
                            ValidNubLocation tExtraValidLocation = tValidLocation;
                            tExtraValidLocation.nWidth += j - nExtraWidthCheckStart + 1;
                            tExtraValidLocation.nMaxLength = nCurValidLength;
                            arValidLocations.push_back(tExtraValidLocation);
                        }
                    }
                }


                if (arValidLocations.empty())
                {
                    SigmaMemoryFree(arMaxValidLengths, NULL);
                    continue;
                }


                int32 nRandLocation = PickRand((int32)arValidLocations.size());
                ValidNubLocation tChosenLocation = arValidLocations[nRandLocation];

                // Inference: parenthesised subtraction at 8798 from the original columns 90/106.
                int32 nWidth = tChosenLocation.nWidth;
                int32 nLength = PickRandRange(nNubLengthMin, (tChosenLocation.nMaxLength - nNubsSpacing) - nNubLengthMin);

                int32 dwSize = nWidth * sizeof(DRLGTile *);
                DRLGTile ** arBaseTiles = (DRLGTile **)SigmaMemoryNew(dwSize, 4, NULL, FALSE);
                SigmaMemoryFill(arBaseTiles, 0, dwSize);


                int32 nFirstTileIndex = nHalfWidthCheck - tChosenLocation.nOffset;
                arBaseTiles[nFirstTileIndex] = AddTile(ptTile, nCurDir, ptCommand, nTileType, eTileSet, TRUE);
                bPlacedNub = arBaseTiles[nFirstTileIndex] != NULL;


                DRLGTile * ptLastTile = arBaseTiles[nFirstTileIndex];
                for (int32 i = nFirstTileIndex - 1; i >= 0 && ptLastTile; i--)
                {
                    arBaseTiles[i] = AddTile(ptLastTile, nPerpendicularDirs[1], ptCommand, nTileType, eTileSet, TRUE);
                    ptLastTile = arBaseTiles[i];
                }
                // Inference: 8813/8821 start at arBaseTiles (cols 36/38); the ptLastTile update on 8814/8822 leaves no instruction of its own.

                ptLastTile = arBaseTiles[nFirstTileIndex];
                for (int32 i = nFirstTileIndex + 1; i < nWidth && ptLastTile; i++)
                {
                    arBaseTiles[i] = AddTile(ptLastTile, nPerpendicularDirs[0], ptCommand, nTileType, eTileSet, TRUE);
                    ptLastTile = arBaseTiles[i];
                }


                for (int32 i = 0; i < nWidth; ++i)
                {
                    ptLastTile = arBaseTiles[i];
                    for (int32 j = 1; ptLastTile && j < nLength; ++j)
                    {
                        ptLastTile = AddTile(ptLastTile, nCurDir, ptCommand, nTileType, eTileSet, TRUE);
                    }
                }

                SigmaMemoryFree(arBaseTiles, NULL);
                SigmaMemoryFree(arMaxValidLengths, NULL);
            }


            if (bPlacedNub && (ptTile->nAllowedExits & ~ptTile->nCurrentExits))
            {
                listPotentialTiles.MoveToTail(listPotentialTiles.GetHeadPosition());
            }
            else
            {
                // Inference: no 8848 row in the original, so the else holds only RemoveHead (no separate count decrement).
                listPotentialTiles.RemoveHead();
            }
        }
    }
}









// NON_MATCHING: 3592 vs 3636 bytes; register/stack-slot allocation differs throughout (nTileLabel slot +4, x25/x20 swaps), the original reloads the exit mask per unrolled 8918 iteration and forms a ccmp at 8988 where ours branches twice. CanAddTile height argument at 8976 is inferred (the original folds GetExitHeight away).
void DRLGDungeonGenerator::CmdAddPerimeterNubs(
    DRLGCommand * ptCommand)
{
    ScopedDRLGWarningAsErrors warningsAsErrors(this, ptCommand);





    DRLGEdgeType eEdgeType = (DRLGEdgeType)TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_EDGE_TYPE);
    if (eEdgeType < 0)
    {
        eEdgeType = DRLG_EDGE_BORDER;
    }


    DRLGLabelType nTileLabel = (DRLGLabelType)TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_LABEL);


    int32 nNubLengthMin = TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_NUBS_LENGTH_MIN);
    if (nNubLengthMin < 1)
    {
        nNubLengthMin = 1;
    }

    int32 nNubLengthDelta = TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_NUBS_LENGTH_DELTA);
    if (nNubLengthDelta < 0)
    {
        nNubLengthDelta = 0;
    }

    int32 nNubLengthMax = nNubLengthMin + nNubLengthDelta;


    TPDRLGTileList listPotentialTiles(sDRLGDungeonGetPtrTilePool());
    POS pos = m_listTiles.GetHeadPosition();
    while (pos)
    {
        DRLGTile & tTile = m_listTiles.GetNext(pos);
        if (tTile.ptCommand->nCommandType != DRLG_COMMAND_ADD_PERIMETER && tTile.ptCommand->nCommandType != DRLG_COMMAND_ADD_PERIMETER_NUBS)
        {
            continue;
        }




        if (nTileLabel != DRLG_LABEL_NONE && !tTile.HasLabel(nTileLabel))
        {
            continue;
        }


        for (int32 d = 0; d < DRLG_EXIT_CNT; d++)
        {
            DRLGExitDirection nDir = (DRLGExitDirection)d;
            DRLGTile * ptNeighbor = tTile.ExitExists(nDir) ? tTile.GetNeighbor(nDir) : NULL;
            if (ptNeighbor && (ptNeighbor->ptCommand->nCommandType == DRLG_COMMAND_ADD_PERIMETER || ptNeighbor->ptCommand->nCommandType == DRLG_COMMAND_ADD_PERIMETER_NUBS) &&
                (nTileLabel == DRLG_LABEL_NONE || ptNeighbor->HasLabel(nTileLabel)))
            {
                listPotentialTiles.AddTail_Tracked(&tTile);
                break;
            }
        }
    }


    int32 nPotentialTilesCount = listPotentialTiles.Count();
    if (nPotentialTilesCount <= 0)
    {
        return;
    }


    int32 nTries = TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_NUBS_MAX);
    int32 nTriesDelta = TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_NUBS_DELTA);
    nTries = PickRandRange(nTries, nTriesDelta);


    GridDataTileSets eTileSet = (GridDataTileSets)TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_TILE_SET);


    int32 nShufflesRemaining = nPotentialTilesCount;
    while (nShufflesRemaining > 1)
    {
        int32 nRandIndex = PickRand(nShufflesRemaining);

        POS pos2 = listPotentialTiles.GetHeadPosition();
        while (nRandIndex > 0 && pos2)
        {
            listPotentialTiles.GetNext(pos2);
            nRandIndex--;
        }

        nShufflesRemaining--;
        listPotentialTiles.MoveToTail(pos2);
    }
    // Inference: the inner while (8964:28 latch row, cmp count,#2 before t++) and Count() > 0 (signed cmp/b.lt, not cbz) come from the
    // original line table and codegen; prefix ++ on t/d/p/nLengthAvailable/l from the increment columns (69/65/57/62/52).
    for (int32 t = 0; t < nTries && listPotentialTiles.Count() > 0; ++t)
    {
        BOOL bPlacedNub = FALSE;
        while (!bPlacedNub && listPotentialTiles.Count() > 0)
        {
            DRLGTile * ptTile = listPotentialTiles.GetHead();


            int32 nRandStartDir = PickRand(DRLG_EXIT_CNT);
            for (int32 d = 0; d < DRLG_EXIT_CNT && !bPlacedNub; ++d)
            {
                DRLGExitDirection nCurDir = (DRLGExitDirection)((nRandStartDir + d) % DRLG_EXIT_CNT);
                DRLGExitDirection nPerpendicularDirs[2] = { sRotateExitDirectionLeft(nCurDir), sRotateExitDirectionRight(nCurDir) };


                if (!ptTile->CanAddTile(nCurDir, ptTile->GetExitHeight(nCurDir)))
                {
                    continue;
                }

                for (int32 p = 0; !bPlacedNub && p < 2; ++p)
                {
                    // Inference: one combined condition over 8986-8989 from the original columns 37/37/60/76/56.
                    DRLGExitDirection nPerpendicularDir = nPerpendicularDirs[nCurDir % 2];
                    DRLGTile * ptNeighbor = ptTile->ExitExists(nPerpendicularDir) ? ptTile->GetNeighbor(nPerpendicularDir) : NULL;
                    if (!ptNeighbor ||
                        ptNeighbor->ptCommand == ptCommand ||
                        (ptNeighbor->ptCommand->nCommandType != DRLG_COMMAND_ADD_PERIMETER && ptNeighbor->ptCommand->nCommandType != DRLG_COMMAND_ADD_PERIMETER_NUBS) ||
                        (nTileLabel != DRLG_LABEL_NONE && !ptNeighbor->HasLabel(nTileLabel)))
                    {
                        continue;
                    }


                    int32 nNewTileX = ptTile->nXCoord + gAddExitX[nCurDir];
                    int32 nNewTileY = ptTile->nYCoord + gAddExitY[nCurDir];


                    int32 nLengthAvailable = 0;
                    for (; nLengthAvailable < nNubLengthMax; ++nLengthAvailable)
                    {
                        int32 nCheckTileX = nNewTileX + nLengthAvailable * gAddExitX[nCurDir];
                        int32 nCheckTileY = nNewTileY + nLengthAvailable * gAddExitY[nCurDir];
                        if (GetTileAtLoc(nCheckTileX, nCheckTileY) ||
                            GetTileAtLoc(nCheckTileX + gAddExitX[nPerpendicularDir], nCheckTileY + gAddExitY[nPerpendicularDir]))
                            break;
                    }




                    if (nLengthAvailable < nNubLengthMin)
                    {
                        continue;
                    }


                    DRLGTile * ptCurParentTile = ptTile;
                    int32 nLength = PickRandRange(nNubLengthMin, nLengthAvailable - nNubLengthMin);
                    for (int32 l = 0; l < nLength; ++l)
                    {
                        DRLGTile * ptPerpendicularParentTile = ptCurParentTile->GetNeighbor(nPerpendicularDir);
                        if (!ptPerpendicularParentTile)
                        {
                            break;
                        }


                        DRLGTile * ptTile1 = AddTile(ptCurParentTile, nCurDir, ptCommand, DRLG_TILE_NONE, eTileSet, TRUE);
                        DRLGTile * ptTile2 = AddTile(ptPerpendicularParentTile, nCurDir, ptCommand, DRLG_TILE_NONE, eTileSet, TRUE);


                        if (!ptTile1 || !ptTile2)
                            break;
                        sPerimeterTileMergeLabels(ptTile1, ptCurParentTile, TRUE, ptCommand);
                        sPerimeterTileMergeLabels(ptTile2, ptPerpendicularParentTile, TRUE, ptCommand);

                        ConnectTiles(ptTile1, ptTile2, nPerpendicularDir, eEdgeType);
                        SetEdgeType(ptTile1, nPerpendicularDir, eEdgeType, TRUE);

                        SetEdgeType(ptCurParentTile, nCurDir, eEdgeType, TRUE);
                        SetEdgeType(ptPerpendicularParentTile, nCurDir, eEdgeType, TRUE);

                        SetEdgeType(ptCurParentTile, nPerpendicularDir, DRLG_EDGE_UNDEFINED, TRUE);
                        ptCurParentTile = ptTile1;
                        bPlacedNub = TRUE;
                    }
                }
            }







            listPotentialTiles.RemoveHead();
        }
    }
}





// NON_MATCHING: 1644 vs 1640 bytes: after the last PrePlaceTile call the original reloads the list head from the stack and re-tests the count before the list is disposed; ours keeps the head in a register and drops the test (depends on the attributes inferred for GetEligibleListForTile, itself NON_MATCHING).
TileInfo * DRLGDungeonGenerator::CmdSwapForPrePlacedTile(
    DRLGCommand * ptCommand,
    DRLGTile * ptTile,
    GridDataTileSets nTileSet,
    DRLGTileType nTileType,
    BOOL bLargeTilesOnly,
    BOOL bForceTileSet,
    int32 nOverrideGroupId)
{
    ScopedWorldDefinitionLockR pdefWorld(m_snoWorld);


    TListTiles listTiles(sDRLGDungeonGetListTilePool());
    GetEligibleListForTile(pdefWorld, ptTile, nTileType, nTileSet, listTiles, DRLG_TILE_FILTER_PRE_PLACE_TILES, 0);


    if (bLargeTilesOnly)
    {
        POS pos = listTiles.GetHeadPosition();
        while (pos)
        {
            POS posCur = pos;
            DungeonTileChooser tTileChooser = listTiles.GetNext(pos);

            int32 nSceneSizeX, nSceneSizeY;
            sGetTileDimensions(*tTileChooser.ptTileInfo, nSceneSizeX, nSceneSizeY);
            if (nSceneSizeX <= 1 && nSceneSizeY <= 1)
            {
                listTiles.RemoveAt(posCur);
            }
        }
    }


    if (listTiles.Count() == 0)
    {
        return NULL;
    }

    if (ShouldReplaceTileForBounty(nTileType, nTileSet))
    {
        POS pos = listTiles.GetHeadPosition();

        while (pos)
        {
            DungeonTileChooser tTileChooser = listTiles.GetNext(pos);
            if (tTileChooser.ptTileInfo->snoScene == m_snoBountyRequiredScene)
            {
                if (PrePlaceTile(ptCommand, tTileChooser.ptTileInfo, nTileSet, tTileChooser.nTileIndex, ptTile->nXCoord, ptTile->nYCoord, ptTile->nHeight, bForceTileSet, nOverrideGroupId))
                {
                    m_bBountyRequiredScenePlaced = TRUE;


                    int32 nActualFrequency = 0;
                    m_TileFrequencies.Lookup(tTileChooser.nTileIndex, nActualFrequency);
                    m_TileFrequencies.SetAt_Tracked(tTileChooser.nTileIndex, nActualFrequency + 1);

                    return tTileChooser.ptTileInfo;
                }
            }
        }
    }







    int32 nRandomTile = PickRand(listTiles.Count());
    POS pos = listTiles.GetHeadPosition();
    while (pos)
    {
        DungeonTileChooser tTileChooser = listTiles.GetNext(pos);
        if (nRandomTile-- <= 0)
        {
            if (PrePlaceTile(ptCommand, tTileChooser.ptTileInfo, nTileSet, tTileChooser.nTileIndex, ptTile->nXCoord, ptTile->nYCoord, ptTile->nHeight, bForceTileSet, nOverrideGroupId))
            {

                int32 nActualFrequency = 0;
                m_TileFrequencies.Lookup(tTileChooser.nTileIndex, nActualFrequency);
                m_TileFrequencies.SetAt_Tracked(tTileChooser.nTileIndex, nActualFrequency + 1);

                return tTileChooser.ptTileInfo;
            }

            break;
        }
    }


    return NULL;
}






































































DRLGTile * DRLGDungeonGenerator::GetTileAtLoc(
    int32 nXCoord,
    int32 nYCoord)
{
    DRLGTile * ptTile = NULL;
    POS pos = m_listTiles.GetHeadPosition();

    while (pos)
    {
        DRLGTile & tTile = m_listTiles.GetNext(pos);

        if (tTile.nXCoord == nXCoord &&
            tTile.nYCoord == nYCoord)
        {
            ptTile = &tTile;
            break;
        }
    }

    return ptTile;
}


DRLGTile * DRLGDungeonGenerator::GetTileAtLoc(DRLGTile * ptTile, DRLGExitDirection nDirection)
{


    return GetTileAtLoc(ptTile->nXCoord + gAddExitX[nDirection], ptTile->nYCoord + gAddExitY[nDirection]);
}


void DRLGDungeonGenerator::ConnectTiles(
    DRLGTile * ptTile1,
    DRLGTile * ptTile2,
    DRLGExitDirection nExitDirection,
    DRLGEdgeType eEdgeType)
{
    DRLGExitDirection nExitDirectionOpp = sOppositeDirection(nExitDirection);
















    ptTile1->nCurrentExits |= sExitDirectionToTileType(nExitDirection);
    ptTile2->nCurrentExits |= sExitDirectionToTileType(nExitDirectionOpp);

    if (eEdgeType != DRLG_EDGE_UNDEFINED)
    {
        ptTile1->arEdges[nExitDirection] = eEdgeType;
        ptTile2->arEdges[nExitDirectionOpp] = eEdgeType;
        // original rows 9292:51/9293:52 sit at the start of the right-hand side (a macro there?); written as a plain shift
        ptTile1->arAllowedEdges[nExitDirection] = (1 << eEdgeType);
        ptTile2->arAllowedEdges[nExitDirectionOpp] = (1 << eEdgeType);
    }
}


void DRLGDungeonGenerator::SetEdgeType(
    DRLGTile * ptTile,
    DRLGExitDirection nExitDirection,
    DRLGEdgeType eEdgeType,
    BOOL bForceEdgeType)
{
    if (eEdgeType == DRLG_EDGE_UNDEFINED && !bForceEdgeType)
        return;



    ptTile->arEdges[nExitDirection] = eEdgeType;
    if (eEdgeType == DRLG_EDGE_UNDEFINED)
    {
        ptTile->arAllowedEdges[nExitDirection] = ((1 << DRLG_EDGE_NUM_TYPES) - 1);
    }
    else
    {
        ptTile->arAllowedEdges[nExitDirection] = (1 << eEdgeType);
    }

    DRLGTile * ptNeighborTile = ptTile->GetNeighbor(nExitDirection);
    if (ptNeighborTile)
    {
        DRLGExitDirection nExitDirectionOpp = sOppositeDirection(nExitDirection);


        ptNeighborTile->arEdges[nExitDirectionOpp] = eEdgeType; ptNeighborTile->arAllowedEdges[nExitDirectionOpp] = eEdgeType == DRLG_EDGE_UNDEFINED ? ((1 << DRLG_EDGE_NUM_TYPES) - 1) : (1 << eEdgeType);
    }
}










void DRLGDungeonGenerator::RemoveTile(
    DRLGTile * ptTile)
{
    for (int32 i = 0; i < DRLG_EXIT_CNT; i++)
    {
        DRLGExitDirection nDir = i;
        if (ptTile->ExitExists(nDir))
        {
            ptTile->nCurrentExits &= ~sExitDirectionToTileType(nDir);

            DRLGTile * ptNeighbor = ptTile->GetNeighbor(nDir);
            if (ptNeighbor)
            {
                ptNeighbor->nCurrentExits &= ~sExitDirectionToTileType(sOppositeDirection(nDir));
            }
        }
    }


    ptTile->nTileSet = GRIDDATA_TILE_SET_DUMMY;
    ptTile->nMaxExits = 0;
}



// NON_MATCHING: 844 vs 852 B; the original keeps tTile.nTileType from the pre-loop test for the first direction and reloads it on the back edge (load PRE), ours reloads at the loop top
// NON_MATCHING: The inlined ConnectTiles OR at +0x1b8 exchanges its two source registers.
void DRLGDungeonGenerator::PatchTileExits()

{
    POS pos = m_listTiles.GetHeadPosition();

    while (pos && !m_bErrors)
    {
        DRLGTile & tTile = m_listTiles.GetNext(pos);

        if (sIsValidTileType(tTile.nTileType) &&
            tTile.nCurrentExits != tTile.nTileType)
        {
            for (int32 i = 0; i < DRLG_EXIT_CNT; i++)
            {
                DRLGExitDirection nDir = i;

                if (sIsLinkPossible(tTile.nTileType, nDir, DRLG_TILE_UNDEFINED))
                {
                    if (!tTile.ExitExists(nDir))
                    {
                        if (tTile.GetNeighbor(nDir) == NULL)
                        {
                            int32 nX, nY;

                            nX = tTile.nXCoord + gAddExitX[nDir];
                            nY = tTile.nYCoord + gAddExitY[nDir];



                            DRLGTileType nDeadEndTileType = sExitDirectionToOppositeTileType(nDir);
                            DRLGTile *ptDeadEnd = AllocTile(tTile.ptCommand, &tTile, nX, nY, nDeadEndTileType, GRIDDATA_TILE_SET_RAT_MAZE_1, tTile.GetExitHeight(nDir), NULL, FALSE);
                            ptDeadEnd->dwFlags |= (1 << DRLG_TILE_FLAG_DEADEND_TILE);


                            ptDeadEnd->nGroup = tTile.nGroup;
                            ptDeadEnd->snoPrevWorld = tTile.snoPrevWorld;
                            ptDeadEnd->uPrevEntranceGUID = tTile.uPrevEntranceGUID;
                            ptDeadEnd->snoPrevLevelArea = tTile.snoPrevLevelArea;
                            ptDeadEnd->snoNextWorld = tTile.snoNextWorld;
                            ptDeadEnd->uNextEntranceGUID = tTile.uNextEntranceGUID;
                            ptDeadEnd->snoNextLevelArea = tTile.snoNextLevelArea;

                            ConnectExits(ptDeadEnd, 0);
                        }
                        else
                        {
                            if (tTile.CanAddExit(nDir, TRUE))
                            {
                                ConnectTiles(&tTile, tTile.GetNeighbor(nDir), nDir, DRLG_EDGE_UNDEFINED);
                            }
                        }
                    }
                }
            }

            if (tTile.nCurrentExits != tTile.nTileType)
            {
                const char * szTileSetStr = SymbolToName(tTile.nTileSet, gDRLGTileSets);

                DRLGTestEvent tEvent(
                    m_dwSeed,
                    DRLGTestEvent::STATUS_ERROR,
                    CRefString().Format("Unable to patch tile exits."),
                    tTile.ptCommand ? tTile.ptCommand->nCommandType : DRLG_COMMAND_NONE,
                    DRLG_ERROR_RESOLUTION_NONE,
                    tTile.nTileType,
                    tTile.nCommandIndex,
                    DRLG_STAGE_POST_GENERATION);

                m_DRLGEvents.push_back(tEvent);

                const char * szTileTypeStr = SymbolToName(tTile.nTileType, gDRLGTileTypes);
                const char * szCommandName = tTile.ptCommand ? tTile.ptCommand->szCommandName : "";







            }
        }
    }
}


void DRLGDungeonGenerator::SpokesConnectEnds()
{
    for (int i = 0; i < m_nCommandCnt; i++)
    {
        DRLGCommand * ptCommand = &m_arCommands[i];
        if (ptCommand->nCommandType == DRLG_COMMAND_ADD_SPOKE)
        {
            BOOL bConnectEnd = TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_SPOKE_CONNECT_END);
            if (bConnectEnd)
            {

                DRLGTile * ptLastTile = NULL;

                POS pos = m_listTiles.GetHeadPosition();
                while (pos)
                {
                    DRLGTile & tTile = m_listTiles.GetNext(pos);
                    if (tTile.ptCommand == ptCommand)
                        ptLastTile = &tTile;

                }


                if (ptLastTile && ptLastTile->GetCurrentExitsCnt() < 2)
                {
                    ConnectExits(ptLastTile, 1);
                }
            }
        }
    }
}


void DRLGDungeonGenerator::FixRepeatTiles()
{






    int arTileSetVariationCounts[DRLG_TILE_NUM_TYPES];

    SigmaMemoryClear(arTileSetVariationCounts, sizeof(arTileSetVariationCounts));

    ScopedWorldDefinitionLockR ptWorldDef(m_snoWorld);

    XGrowableArray<TileInfo, TileInfo> & arTileInfo = ptWorldDef->ptServerData->ptDRLG->arTiles;

    for (int i = 0; i < arTileInfo.GetArraySize(); i++)
    {
        TileInfo & tTileInfo = arTileInfo[i];

        if (tTileInfo.nTileSet == GRIDDATA_TILE_SET_RAT_MAZE_1)
        {


            arTileSetVariationCounts[tTileInfo.nTileType]++;
        }
    }

    POS pos = m_listTiles.GetHeadPosition();

    while (pos)
    {
        DRLGTile & tTile = m_listTiles.GetNext(pos);

        DRLGTileType eTileType = tTile.nCurrentExits;

        int nExitCount = sExitCountForTileType(eTileType);

        if (nExitCount < 2 || nExitCount > 3)
        {
            continue;
        }


        if (arTileSetVariationCounts[eTileType] > 2)
        {
            continue;
        }


        if (!tTile.IsDummy() &&
            sIsValidTileType(eTileType))
        {


            for (int j = 0; j < DRLG_EXIT_CNT; j++)
            {

                if (!tTile.ExitExists(j))
                {
                    continue;
                }

                DRLGTile * ptTileNeighbor = tTile.GetNeighbor(j);

                if (ptTileNeighbor && ptTileNeighbor->nCurrentExits == eTileType)
                {
                    int32 arDirectionWeights[DRLG_EXIT_CNT] = { 100, 100, 100, 100 };

                    DRLGTileConstraint tConstraint;

                    tConstraint.nRoomCreateId = tTile.nCreateId;

                    PushConstraint(tConstraint);

                    DRLGTile * ptTile = AddTile(arDirectionWeights, tTile.ptCommand, DRLG_TILE_NONE, m_eRepeatTileSet, NULL);

                    if (ptTile)
                    {
                        eTileType = tTile.nCurrentExits;
                    }
                    else
                    {
                        PopConstraint();



                        tConstraint.nRoomCreateId = ptTileNeighbor->nCreateId;

                        PushConstraint(tConstraint);

                        AddTile(arDirectionWeights, ptTileNeighbor->ptCommand, DRLG_TILE_NONE, m_eRepeatTileSet, NULL);
                    }


                    PopConstraint();
                }
            }
        }
    }
}






static BOOL sHasUndefinedEdges(
    const DRLGTile & tTile)
{
    for (int32 i = 0; i < DRLG_EXIT_CNT; i++)
    {
        DRLGExitDirection nDir = (DRLGExitDirection)i;
        if (tTile.ExitExists(nDir))
        {
            if (tTile.arEdges[nDir] == DRLG_EDGE_UNDEFINED)
                return TRUE;
        }
    }

    return FALSE;
}




void DRLGDungeonGenerator::SetEdgeTypes()

{

    if (!m_bUseEdgeTypes)
        return;



    ScopedWorldDefinitionLockR pdefWorld(m_snoWorld);



    TPDRLGTileList listUndeterminedTiles(sDRLGDungeonGetPtrTilePool());


    POS pos = m_listTiles.GetHeadPosition();
    while (pos)
    {
        DRLGTile & tTile = m_listTiles.GetNext(pos);


        if (!sHasUndefinedEdges(tTile))
            continue;



        listUndeterminedTiles.AddTail_Tracked(&tTile);
    }


    while (listUndeterminedTiles.Count())
    {
        DRLGTile * ptTile = listUndeterminedTiles.GetHead();
        listUndeterminedTiles.RemoveHead();

        TListTiles listTiles(sDRLGDungeonGetListTilePool());
        DRLGTileType nTileType = ptTile->nCurrentExits;
        GetEligibleListForTile(pdefWorld, ptTile, nTileType, ptTile->nTileSet, listTiles, DRLG_TILE_FILTER_EDGE_TYPES_ONLY, 0);



        if (listTiles.Count() <= 0)
            continue;



        DRLGEdgeType arEdgeTypes[DRLG_EXIT_CNT];
        SigmaMemoryCopy(arEdgeTypes, ptTile->arEdges, sizeof(arEdgeTypes));


        uint32 arAllowedEdge[DRLG_EXIT_CNT];
        SigmaMemoryCopy(arAllowedEdge, ptTile->arAllowedEdges, sizeof(arAllowedEdge));
        SigmaMemoryClear(ptTile->arAllowedEdges, sizeof(ptTile->arAllowedEdges));

        POS posSub = listTiles.GetHeadPosition();
        while (posSub)
        {
            DungeonTileChooser & tTileChooser = listTiles.GetNext(posSub);
            TileInfo * ptTileInfo = tTileChooser.ptTileInfo;


            for (int32 i = 0; i < DRLG_EXIT_CNT; i++)
            {
                DRLGExitDirection nDir = (DRLGExitDirection)i;
                if (!(ptTile->nCurrentExits & (1 << nDir)))
                    continue;



                DRLGEdgeType eEdgeType = TagMapGetInt(ptTileInfo->hTileTagMap, gEdgeTags[nDir]);



                ptTile->arAllowedEdges[nDir] |= (1 << eEdgeType);

                if (eEdgeType != arEdgeTypes[nDir])
                {
                    arEdgeTypes[nDir] = arEdgeTypes[nDir] == DRLG_EDGE_UNDEFINED ? eEdgeType : DRLG_EDGE_INVALID;
                }
            }
        }











        for (int32 i = 0; i < DRLG_EXIT_CNT; i++)
        {
            DRLGExitDirection nDir = (DRLGExitDirection)i;
            if (!ptTile->ExitExists(nDir))
                continue;



            DRLGTile * ptNeighborTile = ptTile->GetNeighbor(nDir);
            if (!ptNeighborTile)
                continue;
            if (arEdgeTypes[nDir] != DRLG_EDGE_INVALID && arEdgeTypes[nDir] != ptTile->arEdges[nDir])
            {
                SetEdgeType(ptTile, nDir, arEdgeTypes[nDir], FALSE);



                listUndeterminedTiles.AddTail_Tracked(ptNeighborTile);
            }
            else if (arAllowedEdge[nDir] != ptTile->arAllowedEdges[nDir])
            {

                DRLGExitDirection nOppositeDir = sOppositeDirection(nDir);
                if (ptNeighborTile->arAllowedEdges[nOppositeDir] != ptTile->arAllowedEdges[nDir])
                {
                    ptNeighborTile->arAllowedEdges[nOppositeDir] &= ptTile->arAllowedEdges[nDir];


                    listUndeterminedTiles.AddTail_Tracked(ptNeighborTile);
                }
            }
        }

    }
}


void DRLGDungeonGenerator::FinalizeEdgeTypes(DRLGTile * ptTile, uint32 dwExteriorExits, TileInfo * ptTileInfo)
{



    for (int32 nExitDir = 0; nExitDir < DRLG_EXIT_CNT; nExitDir++)
    {
        if (ptTileInfo && (dwExteriorExits & (1 << nExitDir)))
            SetEdgeType(ptTile, (DRLGExitDirection)nExitDir, TagMapGetInt(ptTileInfo->hTileTagMap, gEdgeTags[nExitDir]), FALSE);





        if (ptTile->arEdges[nExitDir] == DRLG_EDGE_UNDEFINED || !(dwExteriorExits & (1 << nExitDir)))
            SetEdgeType(ptTile, (DRLGExitDirection)nExitDir, DRLG_EDGE_DEFAULT, FALSE);
    }
}




void DRLGDungeonGenerator::ComputeRoomDistances(
    DRLGTile * ptStartTile)
{
    ComputeRoomDistances(m_listTiles, ptStartTile, FALSE);
}


void DRLGDungeonGenerator::ComputeRoomPaths(
    DRLGTile * ptStartTile)
{
    ComputeRoomDistances(m_listTiles, ptStartTile, TRUE);
}




void DRLGDungeonGenerator::ComputeRoomDistances(
    TDRLGTileList & tTileList,
    DRLGTile * ptStartTile,
    BOOL bFilterPaths)
{
    POS pos = tTileList.GetHeadPosition();

    int32 nTileCnt = 0;

    while (pos)
    {
        DRLGTile & tCurTile = tTileList.GetNext(pos);

        tCurTile.nDistance = -2;
        tCurTile.nComputedPaths = 0;
        nTileCnt++;
    }

    DRLGTile * pTiles1[128];
    DRLGTile * pTiles2[128];
    DRLGTile ** pNextStepTiles = pTiles1;
    DRLGTile ** pCurStepTiles = pTiles2;
    int32 nCurTilesCnt = 1;
    int32 nNextTilesCnt = 0;
    int32 nCurDistance = 0;

    pCurStepTiles[0] = ptStartTile;

    while (nCurTilesCnt > 0)
    {
        nNextTilesCnt = 0;

        for (int32 i = 0; i < nCurTilesCnt; i++)
        {
            pCurStepTiles[i]->nDistance = nCurDistance;

            for (int32 nDirection = 0; nDirection < DRLG_EXIT_CNT; nDirection++)
            {
                DRLGExitDirection nDir = nDirection;
                if (!pCurStepTiles[i]->ExitExists(nDir))
                    continue;
                DRLGTile * pNeighbor = pCurStepTiles[i]->GetNeighbor(nDir);

                if (!pNeighbor || pNeighbor->nDistance >= 0)
                    continue;
                if (bFilterPaths)
                {

                    if (!(pCurStepTiles[i]->nFilterFlags & (1 << (DRLG_PATH_START + nDir))))
                    {

                        BOOL bHasPathLink = FALSE;


                        if (nCurDistance > 0)
                        {
                            for (int32 nPrevDirection = 0; nPrevDirection < DRLG_EXIT_CNT; nPrevDirection++)
                            {






                                if (nPrevDirection != nDir && (pCurStepTiles[i]->nComputedPaths & (1 << nPrevDirection)))
                                {
                                    DRLGPathTileFlags eFlag = GetPathLinkFromDirections(nPrevDirection, nDir);
                                    if (pCurStepTiles[i]->nFilterFlags & (1 << eFlag))
                                    {



                                        bHasPathLink = TRUE;
                                        break;
                                    }
                                }
                            }
                        }
                        if (!bHasPathLink)
                            continue;
                    }
                }

                DRLGExitDirection nOppositeDir = sOppositeDirection(nDir);
                if (bFilterPaths && !(pNeighbor->nFilterFlags & (1 << (DRLG_PATH_DESTINATION + nOppositeDir))))
                {

                    BOOL bHasPathLink = FALSE;


                    for (int32 nNextDirection = 0; nNextDirection < DRLG_EXIT_CNT; nNextDirection++)
                    {




                        if (nNextDirection != nOppositeDir)
                        {
                            DRLGPathTileFlags eFlag = GetPathLinkFromDirections(nNextDirection, nOppositeDir);
                            if (pNeighbor->nFilterFlags & (1 << eFlag))
                            {
                                bHasPathLink = TRUE;
                                break;
                            }
                        }
                    }
                    if (!bHasPathLink)
                        continue;
                }




                if (pNeighbor->nDistance == -2)
                {
                    pNeighbor->nDistance = -1;
                    pNextStepTiles[nNextTilesCnt] = pNeighbor;
                    nNextTilesCnt++;
                }



                if (pNeighbor->nDistance == -1)
                {
                    pNeighbor->nComputedPaths |= (1 << nOppositeDir);
                }
            }
        }



        nCurDistance++;
        DRLGTile ** pTemp = pCurStepTiles;
        pCurStepTiles = pNextStepTiles;
        pNextStepTiles = pTemp;

        int32 nTemp = nCurTilesCnt;
        nCurTilesCnt = nNextTilesCnt;
        nNextTilesCnt = nTemp;
    }
}



void DRLGDungeonGenerator::GetGridBounds(
    IVector2D & posMin,
    IVector2D & posMax)
{
    posMin.x = 10000;
    posMin.y = 10000;
    posMax.x = -10000;
    posMax.y = -10000;
    POS pos = m_listTiles.GetHeadPosition();

    while (pos)
    {
        const DRLGTile & tTile = m_listTiles.GetNext(pos);

        if (tTile.nXCoord < posMin.x)
            posMin.x = tTile.nXCoord;
        if (tTile.nXCoord > posMax.x)
            posMax.x = tTile.nXCoord;
        if (tTile.nYCoord < posMin.y)
            posMin.y = tTile.nYCoord;
        if (tTile.nYCoord > posMax.y)
            posMax.y = tTile.nYCoord;
    }
}











void DRLGDungeonGenerator::LayoutPointToStitchPoint(
    const IVector2D & pointLayoutSpace,
    IVector2D & pointStitchSpace)
{
    pointStitchSpace.x = pointLayoutSpace.x * m_nTileSizeX;
    pointStitchSpace.y = pointLayoutSpace.y * m_nTileSizeY;
}




DRLGTile * DRLGDungeonGenerator::GetNextDeadEndTile(DRLGTile * ptTile, DRLGExitDirection & nLastExitDirection)
{


    DRLGExitDirection nDirection = DRLG_EXIT_NONE;
    for (int32 nDir = 0; nDir < DRLG_EXIT_CNT; nDir++)
    {



        if (nDir != nLastExitDirection && (ptTile->nCurrentExits & (1 << nDir)))
        {
            nDirection = nDir;
            break;
        }
    }







    DRLGTile * ptNext = ptTile->GetNeighbor(nDirection);

    nLastExitDirection = sOppositeDirection(nDirection);

    if (ptNext != NULL)
    {
        if (!ptNext->CanRemoveTile() || ptNext->GetCurrentExitsCnt() > 2)
            ptNext = NULL;
    }
    return ptNext;
}






int32 DRLGDungeonGenerator::GetDeadEndLength(DRLGTile * ptTile)
{


    int32 nLength = 0;
    DRLGExitDirection nLastDirection = DRLG_EXIT_NONE;









    {
        int32 nTotalTilesConsidered = 0;
        int32 nTotalTiles = m_listTiles.Count();
        DRLGTile * ptNext = ptTile;
        while (ptNext)
        {
            nLength++;
            ptNext = GetNextDeadEndTile(ptNext, nLastDirection);
            if (nTotalTilesConsidered >= nTotalTiles)
                break;
            nTotalTilesConsidered++;
        }
    }

    return nLength;
}















int32 DRLGDungeonGenerator::GetDeadEnds(TDRLGDeadEndList & tDeadEndList)
{
    int32 nDeadEndCnt = 0;




    POS pos = m_listTiles.GetHeadPosition();
    while (pos)
    {


        DRLGTile & tTile = m_listTiles.GetNext(pos);


        if (tTile.GetCurrentExitsCnt() == 1 && tTile.CanRemoveTile())
        {




            DRLGDeadEnd tDeadEnd;
            tDeadEnd.ptTile = &tTile;
            tDeadEnd.nLength = GetDeadEndLength(&tTile);

            tDeadEndList.AddTail_Tracked(tDeadEnd);
            nDeadEndCnt++;
        }
    }

    return nDeadEndCnt;
}
int32 DRLGDungeonGenerator::ConnectExits(
    DRLGTile * ptTile,
    int32 nMaxExitsToAdd)
{
    int32 nExitAddedCnt = 0;

    if (ptTile->GetCurrentExitsCnt() < ptTile->nMaxExits)
    {
        for (int32 i = 0; i < DRLG_EXIT_CNT; i++)
        {
            DRLGExitDirection nDir = i;
            if (ptTile->CanAddExit(nDir, TRUE))
            {

                DRLGTile * ptNeighbor = ptTile->arNeighbors[nDir];





                BOOL bRequired = sIsValidTileType(ptTile->nTileType) || sIsValidTileType(ptNeighbor->nTileType);
                if (nExitAddedCnt < nMaxExitsToAdd || bRequired)
                {








                    ConnectTiles(ptTile, ptNeighbor, nDir, DRLG_EDGE_UNDEFINED);
                    nExitAddedCnt++;
                }
            }
        }
    }
    return nExitAddedCnt;
}

// NON_MATCHING: 600 vs 592 B; the original keeps nTotalTilesConsidered as a conditional increment (cmp; cinc lt; b.lt), ours becomes a pre-incremented counter
int32 DRLGDungeonGenerator::ConnectDRLGTiles(
    DRLGTileConnectTechnique nTechnique,
    int32 nMaxExitsToAddPerTile)
{
    int32 nTotalExitsAdded = 0;

    if (nTechnique == DRLG_CONNECT_ALL_TILES)
    {
        POS pos = m_listTiles.GetHeadPosition();

        while (pos)
        {
            DRLGTile & tTile = m_listTiles.GetNext(pos);

            int32 nExitsAddedCnt = ConnectExits(&tTile, nMaxExitsToAddPerTile);

            nTotalExitsAdded += nExitsAddedCnt;
        }
    }
    else
    {
        TDRLGDeadEndList tDeadEndList(sDRLGDungeonGetDeadEndPool());
        const int32 nTotalTiles = m_listTiles.Count();

        int32 nDeadEndCnt = GetDeadEnds(tDeadEndList);

        if (nDeadEndCnt > 0)
        {
            POS pos = tDeadEndList.GetHeadPosition();


            while (pos)
            {
                DRLGDeadEnd & tDeadEnd = tDeadEndList.GetNext(pos);

                DRLGTile * ptTile = tDeadEnd.ptTile;



                DRLGExitDirection nLastDirection = DRLG_EXIT_NONE;
                int32 nTotalTilesConsidered = 0;
                DRLGTile * ptTileNext;

                do
                {
                    ptTileNext = GetNextDeadEndTile(ptTile, nLastDirection);

                    int32 nExitsAddedCnt = ConnectExits(ptTile, nMaxExitsToAddPerTile);

                    nTotalExitsAdded += nExitsAddedCnt;

                    if (nExitsAddedCnt > 0 && nTechnique == DRLG_CONNECT_FIRST_DEAD_END_TILE)
                        break;
                    if (ptTileNext == NULL)
                        break;

                    ptTile = ptTileNext;

                    if (nTotalTilesConsidered >= nTotalTiles)
                        break;
                    ++nTotalTilesConsidered;
                } while (true);
            }
        }












    }

    return nTotalExitsAdded;
}


void DRLGDungeonGenerator::PickTilesFromSet(int32 * arTileWeights, GridDataTileSets nTileSet, DRLGTileType nRequiredExits)
{




    ScopedWorldDefinitionLockR pdefWorld(m_snoWorld);
    SigmaMemoryClear(arTileWeights, sizeof(int32) * DRLG_TILE_NUM_TYPES);




    for (int32 nTileIndex = 0; nTileIndex < pdefWorld->ptServerData->ptDRLG->arTiles.GetArraySize(); nTileIndex++)
    {
        TileInfo & tTileInfo = pdefWorld->ptServerData->ptDRLG->arTiles[nTileIndex];

        if (!sIsValidTileType(tTileInfo.nTileType))
            continue;
        if (nTileSet != GRIDDATA_TILE_SET_NONE && tTileInfo.nTileSet != nTileSet)
            continue;
        if (nRequiredExits != DRLG_TILE_NONE && !(tTileInfo.nTileType & nRequiredExits))
            continue;
        arTileWeights[tTileInfo.nTileType] += tTileInfo.dwWeight;
    }
}









BOOL DRLGDungeonGenerator::IsTileAvailable(
    GridDataTileSets nTileSet,
    DRLGTileType nTileType)
{
    ScopedWorldDefinitionLockR pdefWorld(m_snoWorld);
    int32 nTileIndex;

    for (nTileIndex = 0; nTileIndex < pdefWorld->ptServerData->ptDRLG->arTiles.GetArraySize(); nTileIndex++)
    {
        TileInfo & tTileInfo = pdefWorld->ptServerData->ptDRLG->arTiles[nTileIndex];

        if (tTileInfo.nTileSet != nTileSet)
            continue;



        if (nTileType != DRLG_TILE_NONE && tTileInfo.nTileType != nTileType)
            continue;

        return TRUE;
    }
    return FALSE;
}






BOOL DRLGDungeonGenerator::PickTransitionTile(
    int32 x,
    int32 y,
    DRLGTile * ptParentTile,
    DRLGTileType & outTileType,
    GridDataTileSets & outTileSet)
{

    if (ptParentTile == NULL || ptParentTile->nTileSet == outTileSet)
    {
        return FALSE;
    }






    DRLGExitDirection nParentDir = DRLG_EXIT_NONE;
    for (int32 i = 0; i < DRLG_EXIT_CNT; i++)
    {
        if (GetTileAtLoc(x + gAddExitX[i], y + gAddExitY[i]) == ptParentTile)
        {
            nParentDir = i;
            break;
        }
    }

    if (nParentDir == DRLG_EXIT_NONE)
    {
        return FALSE;
    }


    ScopedWorldDefinitionLockR pdefWorld(m_snoWorld);

    int32 nValidTileCount = 0;

    for (int32 nTileIndex = 0; nTileIndex < pdefWorld->ptServerData->ptDRLG->arTiles.GetArraySize(); nTileIndex++)
    {
        TileInfo & tTileInfo = pdefWorld->ptServerData->ptDRLG->arTiles[nTileIndex];


        GridDataTileSets nTransitionTileSetForParent = TagMapGetInt(tTileInfo.hTileTagMap, gTransitionTags[nParentDir]);
        if (nTransitionTileSetForParent != ptParentTile->nTileSet)
        {
            continue;
        }


        for (int32 i = 0; i < DRLG_EXIT_CNT; i++)
        {
            DRLGExitDirection nNewDir = i;
            if (nNewDir == nParentDir)
            {
                continue;
            }

            GridDataTileSets nTransitionTileNew = TagMapGetInt(tTileInfo.hTileTagMap, gTransitionTags[nNewDir]);
            if (nTransitionTileNew != outTileSet)
            {
                continue;
            }


            int32 nextX = x + gAddExitX[nNewDir];
            int32 nextY = y + gAddExitY[nNewDir];
            if (GetTileAtLoc(nextX, nextY) != NULL)
            {
                continue;
            }



            nValidTileCount++;
            if (PickRand(nValidTileCount) == 0)
            {
                outTileType = tTileInfo.nTileType;
                outTileSet = tTileInfo.nTileSet;
            }
        }
    }

    return nValidTileCount > 0;
}


void DRLGDungeonGenerator::PickDeadEndCapTile(DRLGTile * ptTile, DRLGTileType nTileType, TListTiles & listTiles, uint32 & dwTotalWeight)
{





    ScopedWorldDefinitionLockR pdefWorld(m_snoWorld);

    for (int32 nTileIndex = 0; nTileIndex < pdefWorld->ptServerData->ptDRLG->arTiles.GetArraySize(); nTileIndex++)
    {
        TileInfo & tTileInfo = pdefWorld->ptServerData->ptDRLG->arTiles[nTileIndex];
        BOOL bDeadEnd = TagMapGetInt(tTileInfo.hTileTagMap, TAG_WORLD_TILE_DEAD_END_CAP);
        if (bDeadEnd && tTileInfo.nTileType == nTileType)
        {
            uint32 dwWeight = GetModifiedTileWeight(ptTile, tTileInfo, nTileIndex, tTileInfo.dwWeight);

            DungeonTileChooser tTileChooser;
            tTileChooser.ptTileInfo = &tTileInfo;
            tTileChooser.dwWeight = dwWeight;
            tTileChooser.nTileIndex = nTileIndex;
            listTiles.AddTail_Tracked(tTileChooser);

            dwTotalWeight += dwWeight;
        }
    }
}

































































void DRLGDungeonGenerator::CacheSceneValues()
{

    POS pos = m_listPlacedScenes.GetHeadPosition();

    while (pos)
    {
        DRLGPlacedScene & tPlacedScene = m_listPlacedScenes.GetNext(pos);
        if (SNONAME_NONE == tPlacedScene.snoname)
            continue;




        WorldCacheSceneValues(tPlacedScene.snoname.snoHandle, tPlacedScene.tSceneSpec.tCachedValues);
    }
}



int32 DRLGDungeonGenerator::CountEdgesOfType(const TileInfo & tTileInfo, DRLGEdgeType eEdgeType)
{
    int32 nEdgeCount = 0;
    for (int32 i = 0; i < DRLG_EXIT_CNT; i++)
    {
        if (tTileInfo.nTileType & (1 << i))
        {
            DRLGEdgeType eCurEdge = TagMapGetInt(tTileInfo.hTileTagMap, gEdgeTags[i]);
            if (eCurEdge == eEdgeType)
                nEdgeCount++;
        }
    }
    return nEdgeCount;
}











DRLGPathTileFlags DRLGDungeonGenerator::GetPathLinkFromDirections(
    int32 nInDir,
    int32 nOutDir)
{
    switch (nInDir)
    {
    case DRLG_EXIT_NORTH:
        switch (nOutDir)
        {
        case DRLG_EXIT_SOUTH: return DRLG_PATH_LINK_NORTH_SOUTH;
        case DRLG_EXIT_EAST: return DRLG_PATH_LINK_NORTH_EAST;
        case DRLG_EXIT_WEST: return DRLG_PATH_LINK_NORTH_WEST;
        }
        break;
    case DRLG_EXIT_SOUTH:
        switch (nOutDir)
        {
        case DRLG_EXIT_NORTH: return DRLG_PATH_LINK_NORTH_SOUTH;
        case DRLG_EXIT_EAST: return DRLG_PATH_LINK_SOUTH_EAST;
        case DRLG_EXIT_WEST: return DRLG_PATH_LINK_SOUTH_WEST;
        }
        break;
    case DRLG_EXIT_EAST:
        switch (nOutDir)
        {
        case DRLG_EXIT_NORTH: return DRLG_PATH_LINK_NORTH_EAST;
        case DRLG_EXIT_SOUTH: return DRLG_PATH_LINK_SOUTH_EAST;
        case DRLG_EXIT_WEST: return DRLG_PATH_LINK_EAST_WEST;
        }
        break;
    case DRLG_EXIT_WEST:
        switch (nOutDir)
        {
        case DRLG_EXIT_NORTH: return DRLG_PATH_LINK_NORTH_WEST;
        case DRLG_EXIT_SOUTH: return DRLG_PATH_LINK_SOUTH_WEST;
        case DRLG_EXIT_EAST: return DRLG_PATH_LINK_EAST_WEST;
        }
        break;
    }
    return DRLG_PATH_INVALID;
}


























BOOL DRLGDungeonGenerator::IsDungeonFinder() const
{
    if (m_idSWorld == SWORLDID_INVALID || m_snoWorld == SNO_NONE)
        return FALSE;
    ScopedWorldDefinitionLockR pdefWorld(m_snoWorld);
    return (pdefWorld->uFlags >> WDF_DUNGEON_FINDER_TILESET) & 1;
}














int32 DRLGDungeonGenerator::PickWeightedRand(int32 * arWeights, int32 nNumWeights)
{
    int32 totWeight = 0;
    int32 nFirstNonZeroWeight = 0;
    for (int32 i = 0; i < nNumWeights; i++)
    {
        if (arWeights[i] > 0)
        {
            totWeight += arWeights[i];
            nFirstNonZeroWeight = i;
        }
    }

    if (totWeight <= 1)
        return nFirstNonZeroWeight;


    int32 rnd = PickRand(totWeight);
    totWeight = 0;
    int32 i;
    for (i = 0; i < nNumWeights; i++)
    {
        totWeight += arWeights[i];
        if (totWeight > rnd)
            break;
    }
    return i;
}












int32 DRLGDungeonGenerator::PickRand(int32 nMax)
{

    return Rand_Internal(*m_ptRNG, nMax - 1);
}


int32 DRLGDungeonGenerator::PickRandRange(
    int32 nMin,
    int32 nDelta)
{
    return RandRange_Internal(*m_ptRNG, nMin, nDelta);
}










static void sZeroExitWeightsForTileType(
    int32 * arDirWeights,
    DRLGTileType nTileType)
{
    DRLGTileType nType = nTileType;

    if (!sIsValidTileType(nType))
        return;


    for (int32 i = 0; i < DRLG_EXIT_CNT; i++)
    {
        DRLGExitDirection nOppDirection = sOppositeDirection(i);
        if (!(nType & sExitDirectionToTileType(nOppDirection)))
        {
            arDirWeights[i] = 0;
        }
    }
}



static void sSetExitWeightsForTileType(
    int32 * arDirWeights,
    DRLGTileType nTileType)
{
    for (int32 i = 0; i < DRLG_EXIT_CNT; i++)
    {
        arDirWeights[i] = 100;
    }

    sZeroExitWeightsForTileType(arDirWeights, nTileType);
}


void DRLGGetDungeonStitchSize(const WorldCommonData & tWCD, int32 & nStitchSizeInFeetX, int32 & nStitchSizeInFeetY)
{
    SNO snoWorld = tWCD.snoWorld;
    if (WorldIsDungeonFinderPlaceHolder(snoWorld) && tWCD.snoDungeonFinderSourceWorld != SNO_NONE)
    {
        snoWorld = tWCD.snoDungeonFinderSourceWorld;
    }




    ScopedWorldDefinitionLockR pdefWorld(snoWorld);


    nStitchSizeInFeetX = TagMapGetInt(pdefWorld->ptServerData->ptDRLG->hDRLGTagMap, TAG_WORLD_STITCH_SIZE_X);
    if (nStitchSizeInFeetX == 0)
    {
        nStitchSizeInFeetX = TagMapGetInt(pdefWorld->ptServerData->ptDRLG->hDRLGTagMap, TAG_WORLD_TILE_SIZE_X);
    }
    nStitchSizeInFeetY = TagMapGetInt(pdefWorld->ptServerData->ptDRLG->hDRLGTagMap, TAG_WORLD_STITCH_SIZE_Y);
    if (nStitchSizeInFeetY == 0)
    {
        nStitchSizeInFeetY = TagMapGetInt(pdefWorld->ptServerData->ptDRLG->hDRLGTagMap, TAG_WORLD_TILE_SIZE_Y);
    }
}
