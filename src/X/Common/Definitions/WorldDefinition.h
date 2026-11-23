// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Definitions/WorldDefinition.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../Clusters.h"
#include "../CommonTypes.h"
#include "Definitions.h"
#include "../GameBalance.h"
#include "../Labels.h"
#include "../SNO/SNOFiles.h"
#include "../SNO/SNOName.h"
#include "../../Sigma/DataStructures/XGrowableArray.h"
#include "../../Sigma/DataStructures/XPooledList.h"
#include "../../Sigma/DataStructures/XPtr64.h"
#include "../../Sigma/Files/XSerializeData.h"
#include "../../Sigma/Math/Rect.h"
#include "../../Sigma/Math/Transform.h"
#include "../../Sigma/Math/Vector.h"
#include "../../Sigma/RGBAColor.h"
#include "../../Sigma/SigmaTypes.h"
#include "../../Sigma/Unicode.h"
#include <stddef.h>
#include <stdint.h>

class CTagMap;  // X/Common/TagMaps/Tags.h; only held through XPtr64 here

class XMemoryAllocatorInterface;

typedef int32 DRLGCommandType;

enum WorldTag {
    TAG_WORLD_TILE_SIZE_X = 851970,
    TAG_WORLD_TILE_SIZE_Y = 851971,
    TAG_WORLD_GRID_TILES_X = 851972,
    TAG_WORLD_GRID_TILES_Y = 851973,
    TAG_WORLD_LEVEL_AREA = 851983,
    TAG_WORLD_NEXT_WORLD_SNO = 851984,
    TAG_WORLD_MUSIC = 851986,
    TAG_WORLD_AMBIENT = 851987,
    TAG_WORLD_WEATHER = 851993,
    TAG_WORLD_PREV_WORLD_SNO = 852000,
    TAG_WORLD_HEIGHT_CHANGE_TO_NORTH = 852006,
    TAG_WORLD_HEIGHT_CHANGE_TO_WEST = 852007,
    TAG_WORLD_STITCH_SIZE_X = 852009,
    TAG_WORLD_STITCH_SIZE_Y = 852016,
    TAG_WORLD_PATCH_EXITS = 852034,
    TAG_WORLD_FIX_REPEATS = 852048,
    TAG_WORLD_REPEATS_TILE_SET = 852049,
    TAG_WORLD_PREV_ENTRANCE = 852050,
    TAG_WORLD_NEXT_ENTRANCE = 852051,
    TAG_WORLD_NEXT_LEVEL_AREA = 854612,
    TAG_WORLD_PREV_LEVEL_AREA = 854613,
    TAG_WORLD_CHECK_FOR_SWASTIKAS = 852054,
    TAG_WORLD_USE_EDGE_TYPES = 852055,
    TAG_WORLD_LEVEL_AREA_ALT1 = 852064,
    TAG_WORLD_LEVEL_AREA_ALT2 = 852065,
    TAG_WORLD_LEVEL_AREA_ALT3 = 852066,
    TAG_WORLD_MAX_CUR_GEN_VARIATION_PER_TILE_TYPE = 852067,
};

enum WorldTileTag {
    TAG_WORLD_TILE_BORDER_N = 983041,
    TAG_WORLD_TILE_BORDER_NE = 983042,
    TAG_WORLD_TILE_BORDER_E = 983043,
    TAG_WORLD_TILE_BORDER_SE = 983044,
    TAG_WORLD_TILE_BORDER_S = 983045,
    TAG_WORLD_TILE_BORDER_SW = 983046,
    TAG_WORLD_TILE_BORDER_W = 983047,
    TAG_WORLD_TILE_BORDER_NW = 983048,
    TAG_WORLD_TILE_TRANSITION_N = 983049,
    TAG_WORLD_TILE_TRANSITION_E = 983050,
    TAG_WORLD_TILE_TRANSITION_S = 983051,
    TAG_WORLD_TILE_TRANSITION_W = 983052,
    TAG_WORLD_TILE_DEAD_END_CAP = 983053,
    TAG_WORLD_TILE_EDGE_TYPE_N = 983054,
    TAG_WORLD_TILE_EDGE_TYPE_S = 983055,
    TAG_WORLD_TILE_EDGE_TYPE_E = 983056,
    TAG_WORLD_TILE_EDGE_TYPE_W = 983057,
    TAG_WORLD_TILE_MIN_FREQUENCY = 983296,
    TAG_WORLD_TILE_MAX_FREQUENCY = 983297,
    TAG_WORLD_TILE_SCENE_SIZE_X = 983298,
    TAG_WORLD_TILE_SCENE_SIZE_Y = 983299,
    TAG_WORLD_TILE_SPECIAL_TILE_TYPE = 983552,
    TAG_WORLD_TILE_SPECIAL_TILE_MIN_SPACING_0 = 983553,
    TAG_WORLD_TILE_SPECIAL_TILE_MIN_SPACING_1 = 983554,
    TAG_WORLD_TILE_CHOKEPOINT_TYPE = 983808,
    TAG_WORLD_TILE_CHOKEPOINT_DIRECTION = 983809,
    TAG_WORLD_TILE_IGNORE_NAVMESH = 984064,
};

enum WorldDRLGCommandTag {
    TAG_WORLD_DRLG_COMMAND_ROOM_CNT = 1015808,
    TAG_WORLD_DRLG_COMMAND_WEIGHT_NORTH = 1015809,
    TAG_WORLD_DRLG_COMMAND_WEIGHT_SOUTH = 1015810,
    TAG_WORLD_DRLG_COMMAND_WEIGHT_EAST = 1015811,
    TAG_WORLD_DRLG_COMMAND_WEIGHT_WEST = 1015812,
    TAG_WORLD_DRLG_COMMAND_TILE_SET = 1015813,
    TAG_WORLD_DRLG_COMMAND_TILE_TYPE = 1015814,
    TAG_WORLD_DRLG_COMMAND_HUB_SIZE = 1015815,
    TAG_WORLD_DRLG_COMMAND_HUB_SIZE_DELTA = 1015816,
    TAG_WORLD_DRLG_COMMAND_DEAD_END_MAX_NUM = 1015817,
    TAG_WORLD_DRLG_COMMAND_DEAD_END_MAX_LEN = 1015818,
    TAG_WORLD_DRLG_COMMAND_MAX_EXITS = 1015819,
    TAG_WORLD_DRLG_COMMAND_ROOM_CREATE_NUM = 1015820,
    TAG_WORLD_DRLG_COMMAND_START_GROUP_NUM = 1015821,
    TAG_WORLD_DRLG_COMMAND_GROUP_NUM = 1015822,
    TAG_WORLD_DRLG_COMMAND_ROOM_CNT_DELTA = 1015823,
    TAG_WORLD_DRLG_COMMAND_SELECT_NUM = 1015824,
    TAG_WORLD_DRLG_COMMAND_SELECT_DELTA = 1015825,
    TAG_WORLD_DRLG_COMMAND_TILE_SET_ENTRANCE = 1015826,
    TAG_WORLD_DRLG_COMMAND_TILE_TYPE_ENTRANCE = 1015827,
    TAG_WORLD_DRLG_COMMAND_TILE_SET_EXIT = 1015828,
    TAG_WORLD_DRLG_COMMAND_TILE_TYPE_EXIT = 1015829,
    TAG_WORLD_DRLG_COMMAND_MIN_DISTANCE = 1015830,
    TAG_WORLD_DRLG_COMMAND_MAX_DISTANCE = 1015831,
    TAG_WORLD_DRLG_COMMAND_MAX_EXIT_CNT = 1015832,
    TAG_WORLD_DRLG_COMMAND_MAX_EXIT_CNT_DELTA = 1015833,
    TAG_WORLD_DRLG_COMMAND_ALLOWABLE_EXITS = 1015834,
    TAG_WORLD_DRLG_COMMAND_DEAD_END_CONNECT_TILES = 1015839,
    TAG_WORLD_DRLG_COMMAND_DEAD_END_CONNECT_FIRST_ONLY = 1015840,
    TAG_WORLD_DRLG_COMMAND_ENABLED = 1015841,
    TAG_WORLD_DRLG_COMMAND_HUB_LENGTH = 1015842,
    TAG_WORLD_DRLG_COMMAND_HUB_LENGTH_DELTA = 1015843,
    TAG_WORLD_DRLG_COMMAND_HUB_WIDTH = 1015844,
    TAG_WORLD_DRLG_COMMAND_HUB_WIDTH_DELTA = 1015845,
    TAG_WORLD_DRLG_COMMAND_ALLOW_ROTATION = 1015846,
    TAG_WORLD_DRLG_COMMAND_HUB_FLEXIBLE_DIMENSIONS = 1015847,
    TAG_WORLD_DRLG_COMMAND_ADD_SPECIAL_TILE_1 = 1019904,
    TAG_WORLD_DRLG_COMMAND_ADD_SPECIAL_TILE_2 = 1019905,
    TAG_WORLD_DRLG_COMMAND_ADD_SPECIAL_TILE_3 = 1019906,
    TAG_WORLD_DRLG_COMMAND_REROLL_ON_FAIL = 1020032,
    TAG_WORLD_DRLG_COMMAND_SPOKE_CONNECT_END = 1020160,
    TAG_WORLD_DRLG_COMMAND_ALL_TILES_CONNECTED = 1020176,
    TAG_WORLD_DRLG_COMMAND_CONNECT_NEIGHBORS = 1020192,
    TAG_WORLD_DRLG_COMMAND_EDGE_TYPE = 1020208,
    TAG_WORLD_DRLG_COMMAND_CHANCE = 1020224,
    TAG_WORLD_DRLG_COMMAND_SELECT_WEIGHT = 1020240,
    TAG_WORLD_DRLG_COMMAND_PARENT_EDGE = 1020256,
    TAG_WORLD_DRLG_COMMAND_RETRY_ANYWHERE = 1020416,
    TAG_WORLD_DRLG_COMMAND_SHAPE_TYPE = 1020672,
    TAG_WORLD_DRLG_COMMAND_SHAPE_PARAM = 1020688,
    TAG_WORLD_DRLG_COMMAND_ROAD_LENGTH_MIN = 1020928,
    TAG_WORLD_DRLG_COMMAND_LABEL = 1021184,
    TAG_WORLD_DRLG_COMMAND_SUBSTITUTE_ENTRANCE_LABEL = 1021440,
    TAG_WORLD_DRLG_COMMAND_SUBSTITUTE_EXIT_LABEL = 1021441,
    TAG_WORLD_DRLG_COMMAND_SUBSTITUTE_ENTRANCE_OVERRIDE_GROUP_NUM = 1021442,
    TAG_WORLD_DRLG_COMMAND_SUBSTITUTE_EXIT_OVERRIDE_GROUP_NUM = 1021443,
    TAG_WORLD_DRLG_COMMAND_NUBS_MAX = 1021696,
    TAG_WORLD_DRLG_COMMAND_NUBS_DELTA = 1021697,
    TAG_WORLD_DRLG_COMMAND_NUBS_SPACING = 1021698,
    TAG_WORLD_DRLG_COMMAND_NUBS_WIDTH_MIN = 1021699,
    TAG_WORLD_DRLG_COMMAND_NUBS_WIDTH_DELTA = 1021700,
    TAG_WORLD_DRLG_COMMAND_NUBS_LENGTH_MIN = 1021701,
    TAG_WORLD_DRLG_COMMAND_NUBS_LENGTH_DELTA = 1021702,
    TAG_WORLD_DRLG_COMMAND_CHOKEPOINT_EXCLUDE_DIST = 1021952,
    TAG_WORLD_DRLG_COMMAND_SPOKE_NO_REPEAT_TYPES = 1022208,
    TAG_WORLD_DRLG_COMMAND_OVERRIDE_GROUP_NUM = 1024000,
    TAG_WORLD_DRLG_COMMAND_TILE_TYPE_ALT_1 = 1028096,
    TAG_WORLD_DRLG_COMMAND_TILE_TYPE_ALT_2 = 1028097,
    TAG_WORLD_DRLG_COMMAND_TILE_TYPE_ALT_3 = 1028098,
    TAG_WORLD_DRLG_COMMAND_DOESNT_GENERATE_PERIMETER = 1028099,
};

typedef int32 GridDataTileSets;

typedef int32 DRLGTileType;

typedef int32 DRLGTileFamily;

enum {
    SCENE_NAVMESH_SIZE_X_VALID_BIT = 0,
    SCENE_NAVMESH_SIZE_Y_VALID_BIT = 1,
    SCENE_AABB_BOUNDS_VALID_BIT = 2,
    SCENE_AABB_MARKERSET_BOUNDS_VALID_BIT = 3,
    SCENE_NUM_NODES_VALID_BIT = 4,
    SCENE_FLAGS_VALID_BIT = 5,
    SCENE_NAVMESH_SIZE_VALID = 3,
    SCENE_ALL_VALUES_VALID = 63,
};

enum {
    WDF_PVP_DOTA = 0,
    WDF_PVP_ARENA = 1,
    WDF_ALTERNATE_PMA = 4,
    WDF_TREAT_AS_ONE_SCENE = 5,
    WDF_SCENE_VALUES_CACHED = 6,
    WDF_DUELING_ARENA = 7,
    WDF_DUNGEON_FINDER_TILESET = 8,
    WDF_OPEN_WORLD_HUB = 9,
    WDF_DUNGEON_FINDER_BOSSWORLD = 10,
    WDF_AXE_MINIMAP_GENERATION_ALLOWED = 11,
    WDF_WORLD_USES_NON_UNIFORM_SCENE_SIZES = 12,
    WDF_COUNT = 13,
};

struct SceneCachedValues {  // size 0x50
    uint32 dwCachedValuesValid;  // 0x00
    int32 nNavmeshSizeX;  // 0x04
    int32 nNavmeshSizeY;  // 0x08
    AABB aabbBounds;  // 0x0c
    AABB aabbMarkerSetBounds;  // 0x24
    int32 arNumNodesForDistByLevelArea[4];  // 0x3c
    uint32 dwFlags;  // 0x4c
};
static_assert(sizeof(SceneCachedValues) == 0x50, "SceneCachedValues");

struct SceneDRLGCreationInfo {  // size 0x2c
    BOOL bCreatedFromDRLG;  // 0x00
    int32 nDRLGCommandIndex;  // 0x04
    int32 nAddOrder;  // 0x08
    DRLGCommandType nCommandType;  // 0x0c
    BOOL bIsBorderTile;  // 0x10
    BOOL bIsDeadEndTile;  // 0x14
    BOOL bIsRoadTile;  // 0x18
    BOOL bIsReplacementTile;  // 0x1c
    BOOL bIsPerimeterTile;  // 0x20
    BOOL bIgnoreNavmesh;  // 0x24
    GridDataTileSets eTileSet;  // 0x28
};
static_assert(sizeof(SceneDRLGCreationInfo) == 0x2c, "SceneDRLGCreationInfo");

struct SceneSpecification {  // size 0xdc
    int32 dHeight;  // 0x00
    IVector2D pointGridLocation;  // 0x04
    SNO arSnoLevelAreas[4];  // 0x0c
    SNO snoPrevWorld;  // 0x1c
    uint32 uPrevEntranceGUID;  // 0x20
    SNO snoPrevLevelArea;  // 0x24
    SNO snoNextWorld;  // 0x28
    uint32 uNextEntranceGUID;  // 0x2c
    SNO snoNextLevelArea;  // 0x30
    SNO snoMusic;  // 0x34
    SNO snoAmbient;  // 0x38
    SNO snoReverb;  // 0x3c
    SNO snoWeather;  // 0x40
    SNO snoPresetWorld;  // 0x44
    int32 nDRLGIndex;  // 0x48
    int32 nSceneChunk;  // 0x4c
    uint32 dwOnPathBits;  // 0x50
    DRLGTileType nTileType;  // 0x54
    DRLGTileFamily nTileFamily;  // 0x58
    SceneDRLGCreationInfo tDRLGCreationInfo;  // 0x5c
    uint32 uClusterID;  // 0x88
    SceneCachedValues tCachedValues;  // 0x8c
};
static_assert(sizeof(SceneSpecification) == 0xdc, "SceneSpecification");

struct SceneChunk {  // size 0x100
    SNOName snoname;  // 0x00
    PRTransform transform;  // 0x08
    SceneSpecification tSceneSpec;  // 0x24
};
static_assert(sizeof(SceneChunk) == 0x100, "SceneChunk");

struct SceneParams {  // size 0x18
    SerializeData serSceneChunks;  // 0x00
    int32 nSceneChunkCount;  // 0x08
    XPtr64<SceneChunk> ptSceneChunks;  // 0x10
};
static_assert(sizeof(SceneParams) == 0x18, "SceneParams");

enum {
    GRIDDATA_TILE_SET_NONE = 0,
    GRIDDATA_TILE_SET_RAT_MAZE_1 = 100,
    GRIDDATA_TILE_SET_RAT_MAZE_2 = 101,
    GRIDDATA_TILE_SET_RAT_MAZE_3 = 102,
    GRIDDATA_TILE_SET_RAT_MAZE_4 = 103,
    GRIDDATA_TILE_SET_RAT_MAZE_5 = 104,
    GRIDDATA_TILE_SET_RAT_MAZE_6 = 105,
    GRIDDATA_TILE_SET_RAT_MAZE_7 = 106,
    GRIDDATA_TILE_SET_RAT_MAZE_8 = 107,
    GRIDDATA_TILE_SET_RAT_MAZE_9 = 108,
    GRIDDATA_TILE_SET_RAT_MAZE_10 = 109,
    GRIDDATA_TILE_SET_RAT_MAZE_11 = 110,
    GRIDDATA_TILE_SET_RAT_MAZE_12 = 111,
    GRIDDATA_TILE_SET_RAT_MAZE_13 = 112,
    GRIDDATA_TILE_SET_RAT_MAZE_14 = 113,
    GRIDDATA_TILE_SET_RAT_MAZE_15 = 114,
    GRIDDATA_TILE_SET_RAT_MAZE_16 = 115,
    GRIDDATA_TILE_SET_RAT_MAZE_17 = 116,
    GRIDDATA_TILE_SET_RAT_MAZE_18 = 117,
    GRIDDATA_TILE_SET_RAT_MAZE_19 = 118,
    GRIDDATA_TILE_SET_RAT_MAZE_20 = 119,
    GRIDDATA_TILE_SET_ENTRANCE_1 = 200,
    GRIDDATA_TILE_SET_ENTRANCE_2 = 201,
    GRIDDATA_TILE_SET_ENTRANCE_3 = 202,
    GRIDDATA_TILE_SET_ENTRANCE_4 = 203,
    GRIDDATA_TILE_SET_ENTRANCE_5 = 204,
    GRIDDATA_TILE_SET_ENTRANCE_6 = 205,
    GRIDDATA_TILE_SET_ENTRANCE_7 = 206,
    GRIDDATA_TILE_SET_ENTRANCE_8 = 207,
    GRIDDATA_TILE_SET_ENTRANCE_9 = 208,
    GRIDDATA_TILE_SET_ENTRANCE_10 = 209,
    GRIDDATA_TILE_SET_ENTRANCE_11 = 210,
    GRIDDATA_TILE_SET_ENTRANCE_12 = 211,
    GRIDDATA_TILE_SET_ENTRANCE_13 = 212,
    GRIDDATA_TILE_SET_ENTRANCE_14 = 213,
    GRIDDATA_TILE_SET_ENTRANCE_15 = 214,
    GRIDDATA_TILE_SET_ENTRANCE_16 = 215,
    GRIDDATA_TILE_SET_ENTRANCE_17 = 216,
    GRIDDATA_TILE_SET_ENTRANCE_18 = 217,
    GRIDDATA_TILE_SET_ENTRANCE_19 = 218,
    GRIDDATA_TILE_SET_ENTRANCE_20 = 219,
    GRIDDATA_TILE_SET_EXIT_1 = 300,
    GRIDDATA_TILE_SET_EXIT_2 = 301,
    GRIDDATA_TILE_SET_EXIT_3 = 302,
    GRIDDATA_TILE_SET_EXIT_4 = 303,
    GRIDDATA_TILE_SET_EXIT_5 = 304,
    GRIDDATA_TILE_SET_EXIT_6 = 305,
    GRIDDATA_TILE_SET_EXIT_7 = 306,
    GRIDDATA_TILE_SET_EXIT_8 = 307,
    GRIDDATA_TILE_SET_EXIT_9 = 308,
    GRIDDATA_TILE_SET_EXIT_10 = 309,
    GRIDDATA_TILE_SET_EXIT_11 = 310,
    GRIDDATA_TILE_SET_EXIT_12 = 311,
    GRIDDATA_TILE_SET_EXIT_13 = 312,
    GRIDDATA_TILE_SET_EXIT_14 = 313,
    GRIDDATA_TILE_SET_EXIT_15 = 314,
    GRIDDATA_TILE_SET_EXIT_16 = 315,
    GRIDDATA_TILE_SET_EXIT_17 = 316,
    GRIDDATA_TILE_SET_EXIT_18 = 317,
    GRIDDATA_TILE_SET_EXIT_19 = 318,
    GRIDDATA_TILE_SET_EXIT_20 = 319,
    GRIDDATA_TILE_SET_FEATURE_1 = 401,
    GRIDDATA_TILE_SET_FEATURE_2 = 402,
    GRIDDATA_TILE_SET_FEATURE_3 = 403,
    GRIDDATA_TILE_SET_FEATURE_4 = 404,
    GRIDDATA_TILE_SET_FEATURE_5 = 405,
    GRIDDATA_TILE_SET_FEATURE_6 = 406,
    GRIDDATA_TILE_SET_FEATURE_7 = 407,
    GRIDDATA_TILE_SET_FEATURE_8 = 408,
    GRIDDATA_TILE_SET_FEATURE_9 = 409,
    GRIDDATA_TILE_SET_FEATURE_10 = 410,
    GRIDDATA_TILE_SET_FEATURE_11 = 411,
    GRIDDATA_TILE_SET_FEATURE_12 = 412,
    GRIDDATA_TILE_SET_FEATURE_13 = 413,
    GRIDDATA_TILE_SET_FEATURE_14 = 414,
    GRIDDATA_TILE_SET_FEATURE_15 = 415,
    GRIDDATA_TILE_SET_FEATURE_16 = 416,
    GRIDDATA_TILE_SET_FEATURE_17 = 417,
    GRIDDATA_TILE_SET_FEATURE_18 = 418,
    GRIDDATA_TILE_SET_FEATURE_19 = 419,
    GRIDDATA_TILE_SET_FEATURE_20 = 420,
    GRIDDATA_TILE_SET_TRANSITION_1 = 500,
    GRIDDATA_TILE_SET_TRANSITION_2 = 501,
    GRIDDATA_TILE_SET_TRANSITION_3 = 502,
    GRIDDATA_TILE_SET_TRANSITION_4 = 503,
    GRIDDATA_TILE_SET_TRANSITION_5 = 504,
    GRIDDATA_TILE_SET_TRANSITION_6 = 505,
    GRIDDATA_TILE_SET_TRANSITION_7 = 506,
    GRIDDATA_TILE_SET_TRANSITION_8 = 507,
    GRIDDATA_TILE_SET_TRANSITION_9 = 508,
    GRIDDATA_TILE_SET_TRANSITION_10 = 509,
    GRIDDATA_TILE_SET_TRANSITION_11 = 510,
    GRIDDATA_TILE_SET_TRANSITION_12 = 511,
    GRIDDATA_TILE_SET_TRANSITION_13 = 512,
    GRIDDATA_TILE_SET_TRANSITION_14 = 513,
    GRIDDATA_TILE_SET_TRANSITION_15 = 514,
    GRIDDATA_TILE_SET_TRANSITION_16 = 515,
    GRIDDATA_TILE_SET_TRANSITION_17 = 516,
    GRIDDATA_TILE_SET_TRANSITION_18 = 517,
    GRIDDATA_TILE_SET_TRANSITION_19 = 518,
    GRIDDATA_TILE_SET_TRANSITION_20 = 519,
    GRIDDATA_TILE_SET_DUMMY = 600,
    GRIDDATA_TILE_SET_COMPOUND_1 = 700,
    GRIDDATA_TILE_SET_COMPOUND_2 = 701,
    GRIDDATA_TILE_SET_COMPOUND_3 = 702,
    GRIDDATA_TILE_SET_COMPOUND_4 = 703,
    GRIDDATA_TILE_SET_COMPOUND_5 = 704,
    GRIDDATA_TILE_SET_COMPOUND_6 = 705,
    GRIDDATA_TILE_SET_COMPOUND_7 = 706,
    GRIDDATA_TILE_SET_COMPOUND_8 = 707,
    GRIDDATA_TILE_SET_COMPOUND_9 = 708,
    GRIDDATA_TILE_SET_COMPOUND_10 = 709,
    GRIDDATA_TILE_SET_COMPOUND_11 = 710,
    GRIDDATA_TILE_SET_COMPOUND_12 = 711,
    GRIDDATA_TILE_SET_COMPOUND_13 = 712,
    GRIDDATA_TILE_SET_COMPOUND_14 = 713,
    GRIDDATA_TILE_SET_COMPOUND_15 = 714,
    GRIDDATA_TILE_SET_COMPOUND_16 = 715,
    GRIDDATA_TILE_SET_COMPOUND_17 = 716,
    GRIDDATA_TILE_SET_COMPOUND_18 = 717,
    GRIDDATA_TILE_SET_COMPOUND_19 = 718,
    GRIDDATA_TILE_SET_COMPOUND_20 = 719,
    GRIDDATA_TILE_SET_PRESENT_SCENE = 800,
};

typedef int32 GridDataTileSets;

enum {
    CUSTOM_TYPE_NONE = 0,
    CUSTOM_TYPE_MULTI_SCENE = 1,
    CUSTOM_TYPE_MULTI_TILE = 2,
    NUM_CUSTOM_TYPES = 3,
};

typedef int32 CustomType;

struct CustomTileCell {  // size 0x24
    BOOL bEnabled;  // 0x00
    int32 nTileType;  // 0x04
    GridDataTileSets nTileSet;  // 0x08
    SNO snoScene;  // 0x0c
    int32 nTileHeight;  // 0x10
    int32 arExitHeights[4];  // 0x14
};
static_assert(sizeof(CustomTileCell) == 0x24, "CustomTileCell");

struct CustomTileInfo {  // size 0x28
    void Copy(const CustomTileInfo & tSourceTileInfo);
    CustomType eCustomType;  // 0x00
    int32 nSizeX;  // 0x04
    int32 nSizeY;  // 0x08
    IVector2D pointAnchor;  // 0x0c
    SerializeData serTiles;  // 0x14
    XPtr64<CustomTileCell> arTiles;  // 0x20
};
static_assert(sizeof(CustomTileInfo) == 0x28, "CustomTileInfo");

struct TileInfo {  // size 0x50
    void Copy(const TileInfo &);
    int32 nTileType;  // 0x00
    GridDataTileSets nTileSet;  // 0x04
    SNO snoScene;  // 0x08
    uint32 dwWeight;  // 0x0c
    SerializeData serTileTagMap;  // 0x10
    XPtr64<CTagMap> hTileTagMap;  // 0x18
    CustomTileInfo tCustomTileInfo;  // 0x20
    uint32 uClusterID;  // 0x48
};
static_assert(sizeof(TileInfo) == 0x50, "TileInfo");

struct DRLGCommand {  // size 0x98
    CHAR szCommandName[128];  // 0x00
    DRLGCommandType nCommandType;  // 0x80
    SerializeData serCommandTagMap;  // 0x84
    XPtr64<CTagMap> hCommandTagMap;  // 0x90
};
static_assert(sizeof(DRLGCommand) == 0x98, "DRLGCommand");

struct DRLGParams {  // size 0x78
    SerializeData serTiles;  // 0x00
    XGrowableArray<TileInfo, TileInfo> arTiles;  // 0x08
    int32 nNumCommands;  // 0x40
    SerializeData serCommands;  // 0x44
    XPtr64<DRLGCommand> ptCommands;  // 0x50
    SerializeData serParentIndices;  // 0x58
    XPtr64<int> pParentIndices;  // 0x60
    SerializeData serDRLGTagMap;  // 0x68
    XPtr64<CTagMap> hDRLGTagMap;  // 0x70
};
static_assert(sizeof(DRLGParams) == 0x78, "DRLGParams");

typedef XPooledList<int, int> LabelListPooled;

enum {
    NAVMESH_FUNC_TYPE_IN = 0,
    NAVMESH_FUNC_TYPE_OUT = 1,
    NAVMESH_FUNC_TYPE_COUNT = 2,
};

typedef int32 NavMeshFuncType;

enum {
    NAVMESH_SPECIAL_0 = 0,
    NAVMESH_SPECIAL_1 = 1,
    NAVMESH_SPECIAL_COUNT = 2,
};

typedef int32 NavMeshSpecialType;

struct PostFXParams {  // size 0x20
    float flFilterOffset[4];  // 0x00
    float flFilterCoeff[4];  // 0x10
};
static_assert(sizeof(PostFXParams) == 0x20, "PostFXParams");

struct Environment {  // size 0x54
    RGBAColor rgbaSkyColor;  // 0x00
    PostFXParams tPostFXParams;  // 0x04
    SNO snoSkyboxActor;  // 0x24
    SNO snoMusic;  // 0x28
    SNO snoIntroMusic;  // 0x2c
    float flIntroMusicCooldownSecsMin;  // 0x30
    float flIntroMusicCooldownSecsMax;  // 0x34
    SNO snoAmbient;  // 0x38
    SNO snoReverb;  // 0x3c
    SNO snoWeather;  // 0x40
    SNO snoIrradianceTex;  // 0x44
    SNO snoIrradianceTexDead;  // 0x48
    float flFarPlaneCap;  // 0x4c
    uint32 dwPad;  // 0x50
};
static_assert(sizeof(Environment) == 0x54, "Environment");

enum {
    LOOTRUN_TIER_INVALID = -1,
    LOOTRUN_TIER_1 = 0,
    LOOTRUN_TIER_2 = 1,
    LOOTRUN_TIER_3 = 2,
    LOOTRUN_TIER_COUNT = 3,
};

struct WorldServerData {  // size 0x90
    SerializeData serDRLGParams;  // 0x00
    XPtr64<DRLGParams> ptDRLG;  // 0x08
    SerializeData serSceneParams;  // 0x10
    XPtr64<SceneParams> ptScene;  // 0x18
    LabelRuleSet tLabelRuleSet;  // 0x20
    int32 nSceneClusterIDNext;  // 0x38
    SceneClusterSet tSceneClusterSet;  // 0x40
    SNO arNavMeshFuncs[4];  // 0x58
    GBID arLootrunTiers[3];  // 0x68
    SNO snoScript;  // 0x74
    int32 nSeed;  // 0x78
    XPtr64<int> arWeatherSNOs;  // 0x80
    SerializeData serWeatherSNOs;  // 0x88
};
static_assert(sizeof(WorldServerData) == 0x90, "WorldServerData");

struct WorldDefinition {  // size 0xc0
    RequiredDefinitionHeader tHeader;  // 0x00
    BOOL bDRLG;  // 0x0c
    SerializeData serServerData;  // 0x10
    XPtr64<WorldServerData> ptServerData;  // 0x18
    SerializeData serMarkerSets;  // 0x20
    SNOArray arMarkerSets;  // 0x28
    Environment tEnvironment;  // 0x60
    float flDeformationScale;  // 0xb4
    uint32 uFlags;  // 0xb8
    float flPVPMinimapFeetPerPixel;  // 0xbc
};
static_assert(sizeof(WorldDefinition) == 0xc0, "WorldDefinition");

class WorldGroupDef : public GroupDefinition {  // size 0xa0
public:
    WorldGroupDef();
    virtual ~WorldGroupDef() {}  // vtable slot 0, line 713
    GROUPDEFINITION_MEMORY_FUNCTIONS()  // line 718; the macro name is invented, see Definitions.h
    virtual uint32 GetMaxCount();  // vtable slot 11
    virtual BOOL CanReferenceOtherSNOs() const { return TRUE; }  // vtable slot 13, line 721
    virtual BOOL SNOPackDestinationIsVariable() const { return TRUE; }  // vtable slot 6, line 723
    virtual SNOPackType GetSNOPackDestination(const void * ptDefinition) const;  // vtable slot 7
    virtual void GetDefaultData(void * ptDefaultDef) const;  // vtable slot 21
    virtual void Postprocess(void * lpDefinition);  // vtable slot 17
    virtual void Delete(void * lpDefinition) const;  // vtable slot 18
    virtual void Read(const RequiredDefinitionHeader & tHeader, void *& lpDefinition, SNOElement * ptSNOElement, BOOL fContiguousBuffer, uint32 dwContiguousSize, XMemoryAllocatorInterface * ptMemoryInterface) const;  // vtable slot 14
};
static_assert(sizeof(WorldGroupDef) == 0xa0, "WorldGroupDef");

extern WorldGroupDef * gptGroupDefWorld;  // WorldDefinition.cpp:518

struct RevealSceneMessage {  // size 0x920
    RequiredMessageHeader tHeader;  // 0x00
    SWorldID idSWorld;  // 0x08
    SceneSpecification tSceneSpec;  // 0x0c
    SSceneID idSScene;  // 0xe8
    SNO snoScene;  // 0xec
    PRTransform transform;  // 0xf0
    SSceneID idSSceneParent;  // 0x10c
    SNO snoSceneGroup;  // 0x110
    uint32 uLabelCount;  // 0x114
    GBID arAppliedLabels[256];  // 0x118
    uint32 uActorCount;  // 0x518
    SNO snoActors[256];  // 0x51c
    BOOL bIsVista;  // 0x91c
};
static_assert(sizeof(RevealSceneMessage) == 0x920, "RevealSceneMessage");

struct DestroySceneMessage {  // size 0x10
    RequiredMessageHeader tHeader;  // 0x00
    SWorldID idSWorld;  // 0x08
    SSceneID idSScene;  // 0x0c
};
static_assert(sizeof(DestroySceneMessage) == 0x10, "DestroySceneMessage");

struct SwapSceneMessage {  // size 0x14
    RequiredMessageHeader tHeader;  // 0x00
    SWorldID idSWorld;  // 0x08
    SSceneID idSSceneOld;  // 0x0c
    SSceneID idSSceneNew;  // 0x10
};
static_assert(sizeof(SwapSceneMessage) == 0x14, "SwapSceneMessage");

struct PreloadSceneDataMessage {  // size 0x20
    RequiredMessageHeader tHeader;  // 0x00
    SSceneID idSScene;  // 0x08
    SNO snoScene;  // 0x0c
    SNO snoLevelAreas[4];  // 0x10
};
static_assert(sizeof(PreloadSceneDataMessage) == 0x20, "PreloadSceneDataMessage");

struct PrefetchDataMessage {  // size 0xc
    RequiredMessageHeader tHeader;  // 0x00
    SNO sno;  // 0x08
};
static_assert(sizeof(PrefetchDataMessage) == 0xc, "PrefetchDataMessage");

typedef ScopedDefinitionLockR<WorldDefinition> ScopedWorldDefinitionLockR;

template <> inline const WorldDefinition * DefinitionLockR<WorldDefinition>(SNO sno) {  // line 841
    return (const WorldDefinition *)gptGroupDefWorld->LockR(sno, FALSE);
}
template <> inline void DefinitionUnlockR<WorldDefinition>(const WorldDefinition * pdef) {  // line 842
    void * lpDef = (void *)pdef;
    gptGroupDefWorld->UnlockR(lpDef);
}


// Functions defined in this file (bodies are not in the DWARF):
//   line   285  SceneCachedValues::SceneCachedValues(const SceneCachedValues &)  [inlined into 1 functions]
//   line   285  SceneCachedValues & SceneCachedValues::operator=(const SceneCachedValues &)  [inlined into 9 functions]
//   line   314  SceneSpecification::SceneSpecification(const SceneSpecification &)  [inlined into 1 functions]
//   line   314  SceneSpecification & SceneSpecification::operator=(const SceneSpecification &)  [inlined into 9 functions]
//   line   350  SceneChunk::SceneChunk(const SceneChunk &)  [inlined into 1 functions]
//   line   713  WorldGroupDef::~WorldGroupDef()  [0x7100645d70]
//   line   718  void * WorldGroupDef::MemAlloc(size_t nSize, uint32 dwAlign) const  [0x7100645da0 (+1 more)]
//   line   718  void WorldGroupDef::MemFree(void *& ptr) const  [0x7100645df0 (+1 more)]
//   line   721  BOOL WorldGroupDef::CanReferenceOtherSNOs() const  [0x7100645e80]
//   line   723  BOOL WorldGroupDef::SNOPackDestinationIsVariable() const  [0x7100645e70]
//   line   841  const WorldDefinition * DefinitionLockR<WorldDefinition>(SNO sno)  [inlined into 63 functions]
//   line   842  void DefinitionUnlockR<WorldDefinition>(const WorldDefinition * pdef)  [inlined into 63 functions]

// External definitions in WorldDefinition.cpp; original DWARF variable inventory.
struct AxeTagInfo;
extern const AxeTagInfo tWorldLayoutGroupTagInfo[];
extern const int32 gnWorldLayoutGroupTagCount;

// External definitions in WorldDefinition.cpp; original DWARF variable inventory.
extern const AxeTagInfo tWorldTileTagInfo[];
extern const int32 gnWorldTileTagCount;

// External definitions in WorldDefinition.cpp; original DWARF variable inventory.
extern const AxeTagInfo tWorldLayoutCommandTagInfo[];
extern const int32 gnWorldLayoutCommandTagCount;
