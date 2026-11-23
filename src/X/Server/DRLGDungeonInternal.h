// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Server/DRLGDungeonInternal.h

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Contrib/Contrib/blz/include/blz/util_pair.h"
#include "../../Contrib/Contrib/blz/include/blz/vector.h"
#include "../Common/DRLG.h"
#include "../Common/DRLGTypes.h"
#include "../Common/SNO/SNOTOC.h"
#include "../Common/TagMaps/Tags.h"
#include "DRLGDungeon.h"
#include "DRLGResults.h"
#include "SWorld.h"
#include "../Sigma/DataStructures/XGrowableMap.h"
#include "../Sigma/DataStructures/XMap.h"
#include "../Sigma/DataStructures/XPooledAllocator.h"
#include "../Sigma/DataStructures/XPooledList.h"
#include "../Sigma/RefString.h"

struct CustomTileInfo;
struct DRLGCommand;
struct IVector2D;
struct SNOName;
struct SceneSpecification;
struct TileInfo;
struct WorldDefinition;

struct DungeonTileChooser {  // size 0x10
    TileInfo * ptTileInfo;  // 0x00
    uint32 dwWeight;  // 0x08
    int32 nTileIndex;  // 0x0c
};
static_assert(sizeof(DungeonTileChooser) == 0x10, "DungeonTileChooser");

typedef XListMemoryPool<DungeonTileChooser> TListTilesPool;

typedef XPooledList<DungeonTileChooser, const DungeonTileChooser &> TListTiles;

struct DRLGTileConstraint {  // size 0x68
    DRLGTileConstraint();
    int32 nMaxExitCnt;  // 0x00
    int32 nRoomCreateNum;  // 0x04
    int32 nRoomCreateId;  // 0x08
    int32 nGroupNum;  // 0x0c
    int32 nHubLength;  // 0x10
    int32 nHubWidth;  // 0x14
    int32 nNumSpokeRooms;  // 0x18
    DRLGTileType nAllowedSpokeDir;  // 0x1c
    uint32 uSpokeTileTypeFlags;  // 0x20
    DRLGCommand * ptFilterCommand;  // 0x28
    DRLGCommand * ptOwnerCommand;  // 0x30
    DRLGCommand * ptFillHubCommand;  // 0x38
    DRLGChokepointType eChokepointType;  // 0x40
    DRLGExitDirection eChokepointDir;  // 0x44
    int32 nChokepointX;  // 0x48
    int32 nChokepointY;  // 0x4c
    int32 nChokepointLength;  // 0x50
    int32 nChokepointWidth;  // 0x54
    int32 nChokepointExcludeDist;  // 0x58
    int32 nFilterFlag;  // 0x5c
    CustomTileInfo * ptTileInfo;  // 0x60
};
static_assert(sizeof(DRLGTileConstraint) == 0x68, "DRLGTileConstraint");

enum {
    DRLG_TILE_FLAG_BORDER_TILE = 0,
    DRLG_TILE_FLAG_DEADEND_TILE = 1,
    DRLG_TILE_FLAG_ROAD_TILE = 2,
    DRLG_TILE_FLAG_PERIMETER_TILE = 3,
    DRLG_TILE_FLAG_REPLACEMENT_TILE = 4,
};

enum DRLGTileFilterType {
    DRLG_TILE_FILTER_EDGE_TYPES_ONLY = 0,
    DRLG_TILE_FILTER_PRE_PLACE_TILES = 1,
    DRLG_TILE_FILTER_PLACE_NORMAL_TILES = 2,
    DRLG_TILE_FILTER_ROAD_START = 3,
    DRLG_TILE_FILTER_ROAD_DESTINATION = 4,
    DRLG_TILE_FILTER_ROAD_LINK = 5,
};

enum DRLGPathTileFlags {
    DRLG_PATH_INVALID = -1,
    DRLG_PATH_START = 0,
    DRLG_PATH_START_NORTH = 0,
    DRLG_PATH_START_SOUTH = 1,
    DRLG_PATH_START_EAST = 2,
    DRLG_PATH_START_WEST = 3,
    DRLG_PATH_DESTINATION = 4,
    DRLG_PATH_DESTINATION_NORTH = 4,
    DRLG_PATH_DESTINATION_SOUTH = 5,
    DRLG_PATH_DESTINATION_EAST = 6,
    DRLG_PATH_DESTINATION_WEST = 7,
    DRLG_PATH_LINK_NORTH_SOUTH = 8,
    DRLG_PATH_LINK_NORTH_EAST = 9,
    DRLG_PATH_LINK_NORTH_WEST = 10,
    DRLG_PATH_LINK_SOUTH_EAST = 11,
    DRLG_PATH_LINK_SOUTH_WEST = 12,
    DRLG_PATH_LINK_EAST_WEST = 13,
};

enum DRLGValidChokepointRotateDir {
    DRLG_CHOKEPOINT_ROTATE_DIR_NONE = 0,
    DRLG_CHOKEPOINT_ROTATE_DIR_LEFT = 1,
    DRLG_CHOKEPOINT_ROTATE_DIR_RIGHT = 2,
    DRLG_CHOKEPOINT_ROTATE_DIR_BOTH = 3,
};

typedef XListMemoryPool<DRLGTile> TDRLGTilePool;

typedef XListMemoryPool<DRLGTile *> TPDRLGTilePool;

typedef XPooledList<DRLGTile *, DRLGTile *> TPDRLGTileList;

typedef blz::pair<int, int> DRLGCoord;

typedef blz::vector<blz::pair<int, int>, blz::allocator<blz::pair<int, int> > > DRLGCoordArray;

struct DRLGDeadEnd {  // size 0x10
    DRLGTile * ptTile;  // 0x00
    int32 nLength;  // 0x08
};
static_assert(sizeof(DRLGDeadEnd) == 0x10, "DRLGDeadEnd");

struct DRLGPendingTileReplacement {  // size 0x14
    int32 nCommandIndex;  // 0x00
    int32 nParentCommandIndex;  // 0x04
    DRLGLabelType nTileLabel;  // 0x08
    GridDataTileSets eReplacementTileSet;  // 0x0c
    int32 nOverrideGroupId;  // 0x10
};
static_assert(sizeof(DRLGPendingTileReplacement) == 0x14, "DRLGPendingTileReplacement");

enum DRLGTileConnectTechnique {
    DRLG_CONNECT_ALL_TILES = 0,
    DRLG_CONNECT_ALL_DEAD_END_TILES = 1,
    DRLG_CONNECT_FIRST_DEAD_END_TILE = 2,
};

enum {
    MAX_NUM_DRLG_CONSTRAINTS = 16,
    MAX_NUM_NESTED_DRLG_COMMANDS = 16,
    MAX_NUM_NESTED_SCENE_SPECS = 16,
    DRLG_INVALID_EXIT_HEIGHT = 65535,
};

typedef XListMemoryPool<DRLGDeadEnd> TDRLGDeadEndPool;

typedef XPooledList<DRLGDeadEnd, const DRLGDeadEnd &> TDRLGDeadEndList;

typedef XMap<int, int, SceneSpecification, const SceneSpecification &, 16> SceneSpecificationGroupMap;

class DRLGDungeonGenerator : public DRLGGenerator {  // size 0x1998
public:
    TDRLGTileList m_listTiles;  // 0x150
    int32 m_nCreateId;  // 0x178
    int32 m_arGroupIdStack[16];  // 0x17c
    int32 m_nGroupIdStackCnt;  // 0x1bc
    DRLGTileConstraint m_arTileConstraints[16];  // 0x1c0
    int32 m_nTileConstraintsCnt;  // 0x840
    DRLGLabelType m_arLabelStack[4];  // 0x844
    int32 m_nLabelStackCnt;  // 0x854
    DRLGCommand * m_arCommandStack[16];  // 0x858
    int32 m_nCommandStackCnt;  // 0x8d8
    DRLGCommand * m_arCommands;  // 0x8e0
    int32 m_nCommandCnt;  // 0x8e8
    int32 * m_arCommandParentIndices;  // 0x8f0
    SceneSpecificationGroupMap m_arSceneSpecMap;  // 0x8f8
    int32 m_nDRLGIndex;  // 0x1850
    int32 m_nFeetPerTileX;  // 0x1854
    int32 m_nFeetPerTileY;  // 0x1858
    int32 m_nFeetPerTileZ;  // 0x185c
    int32 m_nTileSizeX;  // 0x1860
    int32 m_nTileSizeY;  // 0x1864
    int32 m_nTileOffsetX;  // 0x1868
    int32 m_nTileOffsetY;  // 0x186c
    int32 m_nHeightChangeToNorth;  // 0x1870
    int32 m_nHeightChangeToWest;  // 0x1874
    blz::vector<DRLGTestEvent, blz::allocator<DRLGTestEvent> > m_DRLGEvents;  // 0x1878
    XGrowableMap<int, int, int, int> m_TileFrequencies;  // 0x1890
    blz::vector<DRLGPendingTileReplacement, blz::allocator<DRLGPendingTileReplacement> > m_arPendingTileReplacements;  // 0x1910
    blz::vector<const DRLGTile *, blz::allocator<const DRLGTile *> > m_arSpecialTiles;  // 0x1928
    CRefString m_sCreationLog;  // 0x1940
    BOOL m_bErrors;  // 0x1958
    BOOL m_bWarnings;  // 0x195c
    BOOL m_bWarningsAsErrors;  // 0x1960
    BOOL m_bPatchTileExits;  // 0x1964
    BOOL m_bTryFixRepeatTiles;  // 0x1968
    BOOL m_bCheckForSwastika;  // 0x196c
    BOOL m_bUseEdgeTypes;  // 0x1970
    int32 m_nMaxCurGenVariationPerTileType;  // 0x1974
    GridDataTileSets m_eRepeatTileSet;  // 0x1978
    int32 m_nCommandTilesCount;  // 0x197c
    SNO m_snoDungeonFinderWeather;  // 0x1980
    SNO m_snoBountyRequiredScene;  // 0x1984
    DRLGTileType m_eBountyRequiredTileType;  // 0x1988
    GridDataTileSets m_eBountyRequiredTileSet;  // 0x198c
    BOOL m_bBountyRequiredScenePlaced;  // 0x1990
    DRLGDungeonGenerator();
    BOOL GenerateDungeon();
    void PlaceTiles();
    void PlaceTile(DRLGTile * ptTile);
    void PlaceBorderTiles(DRLGTile * ptTile);
    int32 GetEligibleListForTile(const WorldDefinition * pdefWorld, DRLGTile * ptTile, DRLGTileType nTileType, GridDataTileSets nTileSet, TListTiles & listTiles, DRLGTileFilterType eFilterType, int32 nFilterParam);
    BOOL DoesTilePassChecks(const TileInfo & tTileInfo, DRLGTile * ptTile, DRLGTileType nTileType, int32 nTileIndex, DRLGTileFilterType eFilterType, int32 nFilterParam, uint32 dwAdjacencyMask);
    BOOL DoesTilePassSpecialTileSpacing(const WorldDefinition * pdefWorld, int32 nXCoord, int32 nYCoord, TileInfo * ptTileInfo);
    BOOL DoesTilePassValidCustomTileChecks(const TileInfo & tTileInfo, DRLGTile * ptTile, DRLGTileFilterType eFilterType, int32 nBaseTileHeight, int32 nTilePosX, int32 nTilePosY, int32 nCustomTileX, int32 nCustomTileY, DRLGTileType & nOutTileType);
    BOOL ShouldReplaceTileForBounty(DRLGTileType eTileType, GridDataTileSets eTileSet);
    SNOName PickTile(DRLGTile * ptTile, DRLGTileType nTileType, GridDataTileSets nTileSet, SNONameArray & listSNONames);
    SNOName PickReplacementTile(DRLGTile * ptTile, DRLGTileType nTileType, GridDataTileSets nTileSet, SNONameArray & listSNONames);
    uint32 GetModifiedTileWeight(DRLGTile * ptTile, const TileInfo & tTileInfo, uint32 nTileIndex, uint32 dwOriginalWeight);
    DRLGValidChokepointRotateDir GetValidChokepointRotateDirections(int32 nXCoord, int32 nYCoord, int32 nChokepointLength, int32 nChokepointWidth, DRLGExitDirection nChokepointDir, GridDataTileSets eTileSet);
    BOOL PushCommand(DRLGCommand * ptCommand, BOOL bPlacingTiles);
    void PopCommand();
    void PushCommandTree(int32 nCommandIndex);
    void PopCommandTree(int32 nCommandIndex);
    DRLGCommand * GetCommand();
    DRLGCommand * GetParentTileCommand();
    void PushConstraint(const DRLGTileConstraint & tConstraint);
    void PopConstraint();
    void PopAllConstraints();
    void PushLabel(DRLGLabelType nLabel);
    void PopLabel();
    void PopAllLabels();
    void AddSceneSpec(SceneSpecification & tSceneSpec);
    BOOL GetSceneSpec(SceneSpecification & tSceneSpec, int32 nGroupId);
    BOOL GetDefaultSceneSpec(SceneSpecification & tSceneSpec);
    void PushGroupId(int32 nGroupId);
    void PopGroupId();
    int32 GetGroupId();
    void PushGroupCommand(DRLGCommand * ptCommand, BOOL bPlacingTiles);
    void PopGroupCommand();
    int32 GetCreateId() { return m_nCreateId; }
    DRLGTile * AllocTile(DRLGCommand * ptCommand, DRLGTile * ptParentTile, int32 nX, int32 nY, DRLGTileType nTileType, GridDataTileSets nTileSet, int32 nHeight, int32 * arExitHeights, BOOL bBorder);
    BOOL CheckHardConstraintsXY(int32 nTileX, int32 nTileY, DRLGTileType nTileType, int32 nHeight, int32 * arExitHeights);
    BOOL CheckHardConstraints(const DRLGTile * ptTile, DRLGExitDirection nDirection, DRLGTileType nTileType);
    BOOL CheckSoftConstraints(const DRLGTile * ptTile, DRLGExitDirection nDirection, BOOL bFilterForError, int32 nTileX, int32 nTileY);
    BOOL FilterTile(const DRLGTile * ptTile, DRLGExitDirection nExitDirection, DRLGTileType nTileType);
    void FilterTiles(TPDRLGTileList & tTileList, DRLGExitDirection nExitDirection, DRLGTileType nTileType);
    void GenerateFailedRecordList(int32, const DRLGTileConstraint &);
    DRLGTile * AddTile(DRLGTile * ptParentTile, DRLGExitDirection nExitDirection, DRLGCommand * ptCommand, DRLGTileType nTileType, GridDataTileSets nTileSet, BOOL bFilter);
    DRLGTile * AddTile(DRLGExitDirection nExitDirection, DRLGCommand * ptCommand, DRLGTileType nTileType, GridDataTileSets nTileSet);
    DRLGTile * AddTile(int32 * arDirectionExitWeights, DRLGCommand * ptCommand, DRLGTileType nTileType, GridDataTileSets nTileSet, DRLGExitDirection * pExitDirection);
    DRLGTile * AddTileByType(int32 * arTileWeightsIn, DRLGCommand * ptCommand, GridDataTileSets nTileSet);
    DRLGTile * AddTileForShape(DRLGCommand * ptCommand, DRLGTileType nTileType, GridDataTileSets nTileSet, DRLGShapeType nShapeType, float flShapeParam, float flOriginX, float flOriginY);
    BOOL PrePlaceTile(DRLGCommand * ptCommand, TileInfo * ptTileInfo, GridDataTileSets nPlacedSceneTileSet, int32 nTileIndex, int32 nBaseTileX, int32 nBaseTileY, int32 nBaseTileHeight, BOOL bForcePlacedSceneTileSet, int32 nOverrideGroupId);
    void ExecuteCommand(int32 nCommandIndex);
    void CmdAddTiles(DRLGCommand * ptCommand);
    void CmdAddEntrance(DRLGCommand * ptCommand);
    void CmdAddExit(DRLGCommand * ptCommand);
    void CmdAddHub(DRLGCommand * ptCommand);
    void CmdAddSpoke(DRLGCommand * ptCommand);
    void CmdRemoveDeadEnds(DRLGCommand * ptCommand);
    void CmdAddEntranceExit(DRLGCommand * ptCommand);
    void CmdAddAllTilesHack(DRLGCommand * ptCommand);
    void CmdAddCustomTile(DRLGCommand * ptCommand);
    void CmdAddPerimeter(DRLGCommand * ptCommand);
    void CmdAddFields(DRLGCommand * ptCommand);
    void CmdAddRoad(DRLGCommand * ptCommand);
    void CmdReplaceEntranceExit(DRLGCommand * ptCommand);
    void CmdReplaceRandomTile(DRLGCommand * ptCommand);
    void CmdAddChokepoint(DRLGCommand * ptCommand);
    void CmdAddNubs(DRLGCommand * ptCommand);
    void CmdAddPerimeterNubs(DRLGCommand * ptCommand);
    TileInfo * CmdSwapForPrePlacedTile(DRLGCommand * ptCommand, DRLGTile * ptTile, GridDataTileSets nTileSet, DRLGTileType nTileType, BOOL bLargeTilesOnly, BOOL bForceTileSet, int32 nOverrideGroupId);
    class ScopedDRLGWarningAsErrors {  // size 0x8
    public:
        ScopedDRLGWarningAsErrors(DRLGDungeonGenerator * ptDRLG, DRLGCommand * ptCommand) : m_ptDRLG(ptDRLG) { if (TagMapGetInt(ptCommand->hCommandTagMap, TAG_WORLD_DRLG_COMMAND_REROLL_ON_FAIL)) { m_ptDRLG->m_bWarningsAsErrors = TRUE; } }
        ~ScopedDRLGWarningAsErrors() { m_ptDRLG->m_bWarningsAsErrors = FALSE; }
    private:
        DRLGDungeonGenerator * m_ptDRLG;  // 0x00
    };
    DRLGTile * GetTileAtLoc(int32 nXCoord, int32 nYCoord);
    DRLGTile * GetTileAtLoc(DRLGTile * ptTile, DRLGExitDirection nDirection);
    void ConnectTiles(DRLGTile * ptTile1, DRLGTile * ptTile2, DRLGExitDirection nExitDirection, DRLGEdgeType eEdgeType);
    void SetEdgeType(DRLGTile * ptTile, DRLGExitDirection nExitDirection, DRLGEdgeType eEdgeType, BOOL bForceEdgeType);
    void RemoveTile(DRLGTile * ptTile);
    void PatchTileExits();
    void SpokesConnectEnds();
    void FixRepeatTiles();
    void SetEdgeTypes();
    void FinalizeEdgeTypes(DRLGTile * ptTile, uint32 dwExteriorExits, TileInfo * ptTileInfo);
    void ComputeRoomDistances(DRLGTile * ptStartTile);
    void ComputeRoomPaths(DRLGTile * ptStartTile);
    static void ComputeRoomDistances(TDRLGTileList & tTileList, DRLGTile * ptStartTile, BOOL bFilterPaths);
    void GetGridBounds(IVector2D & posMin, IVector2D & posMax);
    void LayoutPointToStitchPoint(const IVector2D & pointLayoutSpace, IVector2D & pointStitchSpace);
    DRLGTile * GetNextDeadEndTile(DRLGTile * ptTile, DRLGExitDirection & nLastExitDirection);
    int32 GetDeadEndLength(DRLGTile * ptTile);
    int32 GetDeadEnds(TDRLGDeadEndList & tDeadEndList);
    int32 ConnectExits(DRLGTile * ptTile, int32 nMaxExitsToAdd);
    int32 ConnectDRLGTiles(DRLGTileConnectTechnique nTechnique, int32 nMaxExitsToAddPerTile);
    void PickTilesFromSet(int32 * arTileWeights, GridDataTileSets nTileSet, DRLGTileType nRequiredExits);
    BOOL IsTileAvailable(GridDataTileSets nTileSet, DRLGTileType nTileType);
    BOOL PickTransitionTile(int32 x, int32 y, DRLGTile * ptParentTile, DRLGTileType & outTileType, GridDataTileSets & outTileSet);
    void PickDeadEndCapTile(DRLGTile * ptTile, DRLGTileType nTileType, TListTiles & listTiles, uint32 & dwTotalWeight);
    void ZeroExitWeightsForTileSet(int32 *, GridDataTileSets);
    void SetExitWeightsForTileSet(int32 *, GridDataTileSets);
    void CacheSceneValues();
    int32 CountEdgesOfType(const TileInfo & tTileInfo, DRLGEdgeType eEdgeType);
    static DRLGPathTileFlags GetPathLinkFromDirections(int32, int32);
    BOOL IsDungeonFinder() const;
    int32 PickWeightedRand(int32 * arWeights, int32 nNumWeights);
    int32 PickRand(int32 nMax);
    int32 PickRandRange(int32 nMin, int32 nDelta);
protected:
    virtual void InitSpecific();  // vtable slot 0
private:
    void sConnectToHub(DRLGTile *& ptTileParent, DRLGCommand * ptCommand, const int32 x, const int32 y, const DRLGExitDirection eDir, GridDataTileSets nTileSet, DRLGTileType nTileType);
    int32 sDetermineOffsetForHub(const int32 x, const int32 y, const int32 nHubLength, const int32 nHubWidth, const DRLGExitDirection eDir, DRLGCommand * ptCommand);
    DRLGTile * sAddPerimeterTile(DRLGTile * ptParentTile, DRLGExitDirection nExitDirection, DRLGCommand * ptCommand, GridDataTileSets nTileSet, BOOL bFlagPerimeter, BOOL bForceConnect, int32 nOriginalTileCount, DRLGEdgeType eEdgeType);
    void sPerimeterTileMergeLabels(DRLGTile * ptNewTile, DRLGTile * ptSourceTile, BOOL bInheritGroupInfo, DRLGCommand * ptCommand);
    void sPerimeterTileInheritLabels(DRLGCommand * ptCommand, DRLGTile * ptNewTile, DRLGExitDirection nParentDirection, int32 nOldTileCount);
    BOOL sValidForChokepointExcludeDist(int32 nTileX, int32 nTileY);
    BOOL sValidForChokepointExcludeDist(DRLGTileConstraint * ptConstraint, int32 nTileX, int32 nTileY);
};
static_assert(sizeof(DRLGDungeonGenerator) == 0x1998, "DRLGDungeonGenerator");

typedef XMemoryPool<DRLGDungeonGenerator> TDRLGDungeonGeneratorPool;

struct DRLGDungeonGlobals {  // size 0x150
    DRLGDungeonGlobals();
    ~DRLGDungeonGlobals();
    TDRLGTilePool tTilePool;  // 0x00
    TPDRLGTilePool tPTilePool;  // 0x30
    TListTilesPool tListTilesPool;  // 0x60
    TSpawnGroupExactInstancePool tSpawnGroupExactPool;  // 0x90
    TDRLGDungeonGeneratorPool tDRLGDungeonGeneratorPool;  // 0xc0
    TPlacedScenePool tPlacedScenePool;  // 0xf0
    TDRLGDeadEndPool tDeadEndPool;  // 0x120
};
static_assert(sizeof(DRLGDungeonGlobals) == 0x150, "DRLGDungeonGlobals");


// Functions defined in this file (bodies are not in the DWARF):
//   line   279  DRLGDungeonGenerator::~DRLGDungeonGenerator()  [0x71006ff680]
//   line   533  int32 DRLGDungeonGenerator::GetCreateId()  [inlined into 1 functions]
//   line   711  DRLGDungeonGenerator::ScopedDRLGWarningAsErrors::ScopedDRLGWarningAsErrors(DRLGDungeonGenerator * ptDRLG, DRLGCommand * ptCommand)  [inlined into 16 functions]
//   line   718  DRLGDungeonGenerator::ScopedDRLGWarningAsErrors::~ScopedDRLGWarningAsErrors()  [inlined into 16 functions]
