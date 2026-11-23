// DRLGTile layout and declarations adopted from original DWARF.
// Original file: X/Server/DRLGDungeon.h

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "../Common/DRLGTypes.h"
#include "../Common/Definitions/WorldDefinition.h"
#include "../Common/SNO/SNOFiles.h"
#include "../Common/SNO/SNOName.h"
#include "../Sigma/DataStructures/XPooledList.h"
#include "../Sigma/SigmaTypes.h"

enum {
    DRLG_ERROR_RESOLUTION_NONE = 0,
    DRLG_ERROR_RESOLUTION_TILES_OMITTED = 1,
    DRLG_ERROR_RESOLUTION_TILES_MALFORMED = 2,
    DRLG_ERROR_RESOLUTION_TILES_RELOCATED = 3,
};

typedef int32 DRLGErrorResolution;

enum {
    DRLG_STAGE_NONE = 0,
    DRLG_STAGE_PRE_GENERATION = 1,
    DRLG_STAGE_GENERATION = 2,
    DRLG_STAGE_POST_GENERATION = 3,
};

typedef int32 DRLGStage;

struct DRLGTile {  // size 0xd8
    uint32 dwFlags;  // 0x00
    DRLGTileType nTileType;  // 0x04
    GridDataTileSets nTileSet;  // 0x08
    SNOName snoName;  // 0x0c
    int32 nTileIndex;  // 0x14
    DRLGTile * arNeighbors[4];  // 0x18
    DRLGEdgeType arEdges[4];  // 0x38
    uint32 arAllowedEdges[4];  // 0x48
    DRLGTile * ptParentTile;  // 0x58
    DRLGCommand * ptCommand;  // 0x60
    int32 nCommandIndex;  // 0x68
    int32 nGroup;  // 0x6c
    int32 nCreateId;  // 0x70
    int32 nFilterFlags;  // 0x74
    int32 nDistance;  // 0x78
    int32 nComputedPaths;  // 0x7c
    int32 nMaxExits;  // 0x80
    int32 nAllowedExits;  // 0x84
    int32 nCurrentExits;  // 0x88
    int32 nXCoord;  // 0x8c
    int32 nYCoord;  // 0x90
    SNO snoPrevWorld;  // 0x94
    uint32 uPrevEntranceGUID;  // 0x98
    SNO snoPrevLevelArea;  // 0x9c
    SNO snoNextWorld;  // 0xa0
    uint32 uNextEntranceGUID;  // 0xa4
    SNO snoNextLevelArea;  // 0xa8
    int32 nHeight;  // 0xac
    int32 arRelativeExitHeights[4];  // 0xb0
    DRLGLabelType arLabels[4];  // 0xc0
    BOOL bIgnoreNavmesh;  // 0xd0
    BOOL IsDummy() const;
    BOOL IsBorderTile() const;
    BOOL UsesEntranceTileSet() const;
    BOOL UsesExitTileSet() const;
    BOOL IsDeadEndTile() const;
    BOOL IsRoadTile() const;
    BOOL IsPerimeterTile() const;
    BOOL IsReplacementTile() const;
    BOOL IsExitRequired(DRLGExitDirection nDirection) const;
    BOOL CanRemoveTile() const;
    BOOL CanAddTile(DRLGExitDirection nDirection, int32 _nHeight) const;
    BOOL CanAddExit(DRLGExitDirection nDirection, BOOL bCheckNeighbor) const;
    int32 GetCurrentExitsCnt() const;
    BOOL ExitExists(DRLGExitDirection nDirection) const;
    DRLGTile * GetNeighbor(DRLGExitDirection nDirection) const;
    int32 GetExitHeight(DRLGExitDirection nDirection) const;
    DRLGEdgeType GetEdgeType(DRLGExitDirection nDirection) const;
    BOOL HasLabel(DRLGLabelType nLabel) const;
    BOOL FormsSwastikaPivotPoint() const;
};
static_assert(sizeof(DRLGTile) == 0xd8, "DRLGTile");

typedef XPooledList<DRLGTile, const DRLGTile &> TDRLGTileList;

