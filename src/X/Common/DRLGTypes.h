// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/DRLGTypes.h
// DRLG enums and DRLGPlacedScene on the DWARF layout; its destructor is the implicit one, the ctor and operator= are defined out of line.

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "CommonTypes.h"
#include "Definitions/WorldDefinition.h"
#include "SNO/SNOFiles.h"
#include "SNO/SNOName.h"
#include "../Sigma/DataStructures/XPooledList.h"
#include "../Sigma/DataStructures/XTemplate.h"
#include "../Sigma/Math/Vector.h"

enum {
    DRLG_COMMAND_NONE = -1,
    DRLG_COMMAND_ADD_ROOMS = 0,
    DRLG_COMMAND_ADD_ENTRANCE = 1,
    DRLG_COMMAND_ADD_EXIT = 2,
    DRLG_COMMAND_ADD_HUB = 3,
    DRLG_COMMAND_ADD_SPOKE = 4,
    DRLG_COMMAND_REMOVE_DEAD_ENDS = 5,
    DRLG_COMMAND_PUSH_CONSTRAINTS = 6,
    DRLG_COMMAND_POP_CONSTRAINTS = 7,
    DRLG_COMMAND_POP_ALL_CONSTRAINTS = 8,
    DRLG_COMMAND_SET_GROUP = 9,
    DRLG_COMMAND_SELECT = 10,
    DRLG_COMMAND_ADD_ENTRANCE_EXIT = 11,
    DRLG_COMMAND_ADD_ALL_TILES = 12,
    DRLG_COMMAND_ADD_CUSTOM_TILE = 13,
    DRLG_COMMAND_FILL_HUB = 14,
    DRLG_COMMAND_ADD_PERIMETER = 15,
    DRLG_COMMAND_ADD_FIELDS = 16,
    DRLG_COMMAND_ADD_ROAD = 17,
    DRLG_COMMAND_ADD_CHOKEPOINT = 18,
    DRLG_COMMAND_REPLACE_ENTRANCE_EXIT = 19,
    DRLG_COMMAND_REPLACE_RANDOM_TILE = 20,
    DRLG_COMMAND_ADD_LABEL = 21,
    DRLG_COMMAND_ADD_NUBS = 22,
    DRLG_COMMAND_ADD_PERIMETER_NUBS = 23,
    DRLG_COMMAND_CNT = 24,
};

enum {
    TILE_FAMILY_NONE = -1,
    TILE_FAMILY_FULL_NSEW = 0,
};

typedef int32 GridDataTileSets;

enum {
    DRLG_TILE_UNDEFINED = -1,
    DRLG_TILE_NONE = 0,
    DRLG_TILE_N = 1,
    DRLG_TILE_S = 2,
    DRLG_TILE_NS = 3,
    DRLG_TILE_E = 4,
    DRLG_TILE_NE = 5,
    DRLG_TILE_SE = 6,
    DRLG_TILE_NSE = 7,
    DRLG_TILE_W = 8,
    DRLG_TILE_NW = 9,
    DRLG_TILE_SW = 10,
    DRLG_TILE_NSW = 11,
    DRLG_TILE_EW = 12,
    DRLG_TILE_NEW = 13,
    DRLG_TILE_SEW = 14,
    DRLG_TILE_NSEW = 15,
    DRLG_TILE_NUM_TYPES = 16,
    DRLG_TILE_CUSTOM = 16777215,
};

typedef int32 DRLGTileType;

enum {
    DRLG_EXIT_NONE = -1,
    DRLG_EXIT_NORTH = 0,
    DRLG_EXIT_SOUTH = 1,
    DRLG_EXIT_EAST = 2,
    DRLG_EXIT_WEST = 3,
    DRLG_EXIT_CNT = 4,
};

typedef int32 DRLGExitDirection;

enum {
    DRLG_EDGE_INVALID = -2,
    DRLG_EDGE_UNDEFINED = -1,
    DRLG_EDGE_DEFAULT = 0,
    DRLG_EDGE_ROAD = 1,
    DRLG_EDGE_BORDER = 2,
    DRLG_EDGE_BOG = 3,
    DRLG_EDGE_SECRET = 4,
    DRLG_EDGE_NUM_TYPES = 5,
};

typedef int32 DRLGEdgeType;

enum {
    DRLG_SHAPE_UNDEFINED = -1,
    DRLG_SHAPE_CIRCLE = 0,
    DRLG_SHAPE_SQUARE = 1,
    DRLG_SHAPE_OVAL = 2,
    DRLG_SHAPE_RECTANGLE = 3,
    DRLG_SHAPE_ORGANIC = 4,
    DRLG_SHAPE_NUM_TYPES = 5,
};

typedef int32 DRLGShapeType;

enum {
    DRLG_CHOKEPOINT_NONE = -1,
    DRLG_CHOKEPOINT_NORTH_SOUTH = 0,
    DRLG_CHOKEPOINT_EAST_WEST = 1,
    DRLG_CHOKEPOINT_PLACEMENT = 2,
};

typedef int32 DRLGChokepointType;

enum {
    DRLG_LABEL_NONE = -1,
    DRLG_LABEL_ALPHA = 0,
    DRLG_LABEL_BRAVO = 1,
    DRLG_LABEL_CHARLIE = 2,
    DRLG_LABEL_DELTA = 3,
    DRLG_LABEL_ECHO = 4,
    DRLG_LABEL_FOXTROT = 5,
    DRLG_LABEL_NUM_TYPES = 6,
};

typedef int32 DRLGLabelType;

enum {
    DRLG_SPECIAL_TILE_NONE = -1,
    DRLG_SPECIAL_TILE_0 = 0,
    DRLG_SPECIAL_TILE_1 = 1,
    DRLG_SPECIAL_TILE_NUM_TYPES = 2,
};

typedef int32 DRLGSpecialTileType;

class DRLGPlacedScene;

typedef XListMemoryPool<DRLGPlacedScene> TPlacedScenePool;

typedef XPooledList<DRLGPlacedScene, const DRLGPlacedScene &> TPlacedSceneList;

typedef XListMemoryPool<DRLGPlacedScene *> TPlacedScenePtrPool;

typedef XPooledList<DRLGPlacedScene *, DRLGPlacedScene *> TPlacedScenePtrList;

class DRLGPlacedScene {  // size 0x168
public:
    SceneSpecification tSceneSpec;  // 0x00
    Vector3D wp;  // 0xdc
    SNOName snoname;  // 0xe8
    DRLGTileType nTileType;  // 0xf0
    GridDataTileSets nTileSet;  // 0xf4
    POS posParentScene;  // 0xf8
    SNO snoSceneGroup;  // 0x100
    SSceneID idSScene;  // 0x104
    SNO snoMarkerSetSrc;  // 0x108
    int32 nMarkerSetSrcIndex;  // 0x10c
    int32 nSpecialTypesPlaced;  // 0x110
    LabelListPooled listAppliedLabels;  // 0x118
    LabelListPooled listPotentialLabels;  // 0x140
    DRLGPlacedScene();
    const DRLGPlacedScene & operator=(const DRLGPlacedScene & tSrc);
};
static_assert(sizeof(DRLGPlacedScene) == 0x168, "DRLGPlacedScene");

