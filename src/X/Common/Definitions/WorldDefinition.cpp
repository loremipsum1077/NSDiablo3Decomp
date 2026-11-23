// Original file: X/Common/Definitions/WorldDefinition.cpp

#include "WorldDefinition.h"

// For cgplaceNull, whose initializer the original runs in this file's static initializer.
#include "../WorldPlace.h"
#include "../../Client/Gfx/GfxVB.h"
#include "../GlobalSNO.h"
#include "../D3TypeDescriptor/D3TypeDescriptor.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptorField.h"
#include "../TagMaps/TagNumbers.h"

// Defined in X/Sigma/Files/XFilesCommon.cpp:20; no adopted header declares it yet.
extern const XTypeDescriptor * DT_SERIALIZEDATA;

// Defined in X/Common/D3TypeDescriptor/D3DescribedType.cpp:321 and :1295; no adopted header
// declares them yet.
extern const XTypeDescriptor * DT_SNO;
extern const XTypeDescriptor * DT_GBID;

// Defined in X/Common/Labels.cpp:860; no adopted header declares it yet.
extern const XTypeDescriptor * DT_LABEL_RULE_SET;

// Defined in X/Common/Clusters.cpp:192; no adopted header declares it yet.
extern const XTypeDescriptor * DT_SCENE_CLUSTER_SET;

// Defined in X/Common/Networking/GameMessage.cpp:65; no adopted header declares it yet.
extern const XTypeDescriptor * DT_REQUIRED_MESSAGE_HEADER;

enum {
    WORLD_XML_VERSION = 19,
    WORLD_COMMON_SNO_VERSION = 246,
    WORLD_PC_SNO_VERSION = 250,
    WORLD_CONSOLE_SNO_VERSION = 252,
    WORLD_AXE_SNO_VERSION = 256,
};

enum {
    ENVIRONMENT_XML_VERSION = 2,
    SCENE_CACHED_VALUES_XML_VERSION = 1,
};

extern const float cgflGravity = -43.2f;  // line 53

// _GLOBAL__sub_I_WorldDefinition.cpp: the translation unit also initializes the four RGBAColor
// constants of X/Client/Gfx/GfxVB.h:351-354 (included above). The field names are "" in the
// binary except the two RevealSceneMessage arrays.
static const TypeDescriptorField PostFXParams_Fields[3] = {  // line 60
    FixedArrayTypeDescriptorField("", DT_FLOAT, offsetof(PostFXParams, flFilterOffset), NULL, 1, 4, 0),
    FixedArrayTypeDescriptorField("", DT_FLOAT, offsetof(PostFXParams, flFilterCoeff), NULL, 1, 4, 0),
    TypeDescriptorField(NULL, DT_NULL, sizeof(PostFXParams), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_PostFXParams("PostFXParams", 0, PostFXParams_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 63
const XTypeDescriptor * DT_POSTFXPARAMS = &TypeDescriptor_PostFXParams;

static const TypeDescriptorField Environment_Fields[15] = {  // line 71
    TypeDescriptorField("", DT_RGBACOLOR, offsetof(Environment, rgbaSkyColor), NULL, 1),
    TypeDescriptorField("", DT_POSTFXPARAMS, offsetof(Environment, tPostFXParams), NULL, 1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(Environment, snoSkyboxActor), "", 1, 0, -1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(Environment, snoMusic), "", 62, 0, -1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(Environment, snoIntroMusic), "", 62, 0, -1),
    TypeDescriptorField("", DT_FLOAT, offsetof(Environment, flIntroMusicCooldownSecsMin), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(Environment, flIntroMusicCooldownSecsMax), NULL, 1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(Environment, snoAmbient), "", 5, 0, -1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(Environment, snoReverb), "", 60, 0, -1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(Environment, snoWeather), "", 47, 0, -1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(Environment, snoIrradianceTex), "", 44, 0, -1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(Environment, snoIrradianceTexDead), "", 44, 0, -1),
    TypeDescriptorField("", DT_FLOAT, offsetof(Environment, flFarPlaneCap), NULL, 0x700001),
    TypeDescriptorField("", DT_INT, offsetof(Environment, dwPad), NULL, 0x700000),
    TypeDescriptorField(NULL, DT_NULL, sizeof(Environment), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_Environment("Environment", ENVIRONMENT_XML_VERSION, Environment_Fields, 15, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 86
const XTypeDescriptor * DT_ENVIRONMENT = &TypeDescriptor_Environment;

static const TypeDescriptorField SceneCachedValues_Fields[8] = {  // line 108
    TypeDescriptorField("", DT_INT, offsetof(SceneCachedValues, dwCachedValuesValid), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(SceneCachedValues, nNavmeshSizeX), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(SceneCachedValues, nNavmeshSizeY), NULL, 1),
    TypeDescriptorField("", DT_AABB, offsetof(SceneCachedValues, aabbBounds), NULL, 1),
    TypeDescriptorField("", DT_AABB, offsetof(SceneCachedValues, aabbMarkerSetBounds), NULL, 1),
    FixedArrayTypeDescriptorField("", DT_INT, offsetof(SceneCachedValues, arNumNodesForDistByLevelArea), NULL, 1, 4, 0),
    TypeDescriptorField("", DT_INT, offsetof(SceneCachedValues, dwFlags), NULL, 0x80001),
    TypeDescriptorField(NULL, DT_NULL, sizeof(SceneCachedValues), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_SceneCachedValues("SceneCachedValues", SCENE_CACHED_VALUES_XML_VERSION, SceneCachedValues_Fields, 8, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 116
const XTypeDescriptor * DT_SCENECACHEDVALUES = &TypeDescriptor_SceneCachedValues;

static const TypeDescriptorField SceneSpecification_Fields[20] = {  // line 118
    TypeDescriptorField("", DT_INT, offsetof(SceneSpecification, dHeight), NULL, 1),
    TypeDescriptorField("", DT_IVECTOR2D, offsetof(SceneSpecification, pointGridLocation), NULL, 1),
    FilteredFixedArrayTypeDescriptorField("", DT_SNO, offsetof(SceneSpecification, arSnoLevelAreas), NULL, 0, 4, 22, 0),
    FilterTypeDescriptorField("", DT_SNO, offsetof(SceneSpecification, snoPrevWorld), "", 48, 0, -1),
    TypeDescriptorField("", DT_INT, offsetof(SceneSpecification, uPrevEntranceGUID), NULL, 1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(SceneSpecification, snoPrevLevelArea), "", 22, 0, -1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(SceneSpecification, snoNextWorld), "", 48, 0, -1),
    TypeDescriptorField("", DT_INT, offsetof(SceneSpecification, uNextEntranceGUID), NULL, 1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(SceneSpecification, snoNextLevelArea), "", 22, 0x200, -1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(SceneSpecification, snoMusic), "", 62, 0, -1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(SceneSpecification, snoAmbient), "", 5, 0, -1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(SceneSpecification, snoReverb), "", 60, 0, -1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(SceneSpecification, snoWeather), "", 47, 0, -1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(SceneSpecification, snoPresetWorld), "", 48, 0, -1),
    TypeDescriptorField("", DT_INT, offsetof(SceneSpecification, nDRLGIndex), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(SceneSpecification, nSceneChunk), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(SceneSpecification, dwOnPathBits), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(SceneSpecification, uClusterID), NULL, 1),
    TypeDescriptorField("", DT_SCENECACHEDVALUES, offsetof(SceneSpecification, tCachedValues), NULL, 0),
    TypeDescriptorField(NULL, DT_NULL, sizeof(SceneSpecification), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_SceneSpecification("SceneSpecification", 4, SceneSpecification_Fields, 20, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 138
const XTypeDescriptor * DT_SCENESPEC = &TypeDescriptor_SceneSpecification;

namespace {

static const SceneDRLGCreationInfo cDefaultDRLGCreationInfo = {  // line 142
    FALSE,
    -1,
    -1,
    -1,
    FALSE,
    FALSE,
    FALSE,
    FALSE,
    FALSE,
    FALSE,
    GRIDDATA_TILE_SET_NONE,
};

static const SceneSpecification cDefaultSceneSpec = {  // line 157
    0,
    IVector2D(0, 0),
    { SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE },
    SNO_NONE,
    (uint32)-1,
    SNO_NONE,
    SNO_NONE,
    (uint32)-1,
    SNO_NONE,
    SNO_NONE,
    SNO_NONE,
    SNO_NONE,
    SNO_NONE,
    SNO_NONE,
    0,
    0,
    (uint32)-1,
    -1,
    -1,
    cDefaultDRLGCreationInfo,
    (uint32)-1,
};

}  // namespace

static const TypeDescriptorField SceneChunk_Fields[4] = {  // line 183
    TypeDescriptorField("", DT_SNO_NAME, offsetof(SceneChunk, snoname), NULL, 1),
    TypeDescriptorField("", DT_PRTRANSFORM, offsetof(SceneChunk, transform), NULL, 1),
    TypeDescriptorField("", DT_SCENESPEC, offsetof(SceneChunk, tSceneSpec), &cDefaultSceneSpec, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(SceneChunk), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_SceneChunk("SceneChunk", 2, SceneChunk_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 187
const XTypeDescriptor * DT_SCENECHUNK = &TypeDescriptor_SceneChunk;

static const TypeDescriptorField SceneParams_Fields[4] = {  // line 189
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(SceneParams, serSceneChunks), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_SCENECHUNK, offsetof(SceneParams, ptSceneChunks), 0x21,
                                     (int32)offsetof(SceneParams, serSceneChunks) - (int32)offsetof(SceneParams, ptSceneChunks)),
    VariableArrayCountTypeDescriptorField("", offsetof(SceneParams, nSceneChunkCount), 0x10000, DT_SCENECHUNK,
                                          (int32)offsetof(SceneParams, serSceneChunks) - (int32)offsetof(SceneParams, nSceneChunkCount)),
    TypeDescriptorField(NULL, DT_NULL, sizeof(SceneParams), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_SceneParams("SceneParams", 0, SceneParams_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 192
const XTypeDescriptor * DT_SCENEPARAMS = &TypeDescriptor_SceneParams;

static const TypeDescriptorField CustomTileCell_Fields[7] = {  // line 194
    BoundedTypeDescriptorField("", DT_INT, offsetof(CustomTileCell, bEnabled), NULL, (const void *)FALSE, (const void *)TRUE),
    TypeDescriptorField("", DT_INT, offsetof(CustomTileCell, nTileType), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(CustomTileCell, nTileSet), NULL, 1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(CustomTileCell, snoScene), "", 33, 0, -1),
    TypeDescriptorField("", DT_INT, offsetof(CustomTileCell, nTileHeight), NULL, 1),
    FixedArrayTypeDescriptorField("", DT_INT, offsetof(CustomTileCell, arExitHeights), NULL, 1, 4, 0),
    TypeDescriptorField(NULL, DT_NULL, sizeof(CustomTileCell), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_CustomTileCell("CustomTileCell", 0, CustomTileCell_Fields, 7, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 201
const XTypeDescriptor * DT_CUSTOM_TILE_CELL = &TypeDescriptor_CustomTileCell;

static const TypeDescriptorField CustomTileInfo_Fields[7] = {  // line 203
    BoundedTypeDescriptorField("", DT_INT, offsetof(CustomTileInfo, eCustomType), NULL, (const void *)CUSTOM_TYPE_NONE,
                               (const void *)(NUM_CUSTOM_TYPES - 1)),
    TypeDescriptorField("", DT_INT, offsetof(CustomTileInfo, nSizeX), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(CustomTileInfo, nSizeY), NULL, 1),
    TypeDescriptorField("", DT_IVECTOR2D, offsetof(CustomTileInfo, pointAnchor), NULL, 1),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(CustomTileInfo, serTiles), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_CUSTOM_TILE_CELL, offsetof(CustomTileInfo, arTiles), 0x21,
                                     (int32)offsetof(CustomTileInfo, serTiles) - (int32)offsetof(CustomTileInfo, arTiles)),
    TypeDescriptorField(NULL, DT_NULL, sizeof(CustomTileInfo), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_CustomTileInfo("CustomTileInfo", 0, CustomTileInfo_Fields, 7, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 209
const XTypeDescriptor * DT_CUSTOM_TILE_INFO = &TypeDescriptor_CustomTileInfo;

static const TypeDescriptorField TileInfo_Fields[9] = {  // line 211
    TypeDescriptorField("", DT_INT, offsetof(TileInfo, nTileType), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(TileInfo, dwWeight), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(TileInfo, nTileSet), NULL, 1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(TileInfo, snoScene), "", 33, 0, -1),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(TileInfo, serTileTagMap), NULL, 0),
    TagMapTypeDescriptorField("", offsetof(TileInfo, hTileTagMap), 0x21,
                              (int32)offsetof(TileInfo, serTileTagMap) - (int32)offsetof(TileInfo, hTileTagMap), 0xf0000),
    TypeDescriptorField("", DT_CUSTOM_TILE_INFO, offsetof(TileInfo, tCustomTileInfo), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(TileInfo, uClusterID), (const void *)-1, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(TileInfo), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_TileInfo("TileInfo", 0, TileInfo_Fields, 9, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 219
const XTypeDescriptor * DT_TILE_INFO = &TypeDescriptor_TileInfo;

static const TypeDescriptorField DRLGCommand_Fields[5] = {  // line 221
    CharArrayTypeDescriptorField("", offsetof(DRLGCommand, szCommandName), "", 1, 128, -1),
    TypeDescriptorField("", DT_INT, offsetof(DRLGCommand, nCommandType), NULL, 1),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(DRLGCommand, serCommandTagMap), NULL, 0),
    TagMapTypeDescriptorField("", offsetof(DRLGCommand, hCommandTagMap), 0x21,
                              (int32)offsetof(DRLGCommand, serCommandTagMap) - (int32)offsetof(DRLGCommand, hCommandTagMap), 0x110000),
    TypeDescriptorField(NULL, DT_NULL, sizeof(DRLGCommand), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_DRLGCommand("DRLGCommand", 0, DRLGCommand_Fields, 5, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 225
const XTypeDescriptor * DT_DRLG_COMMAND = &TypeDescriptor_DRLGCommand;

static const TypeDescriptorField DRLGParams_Fields[10] = {  // line 227
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(DRLGParams, serTiles), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_TILE_INFO, offsetof(DRLGParams, arTiles), 0x23,
                                     (int32)offsetof(DRLGParams, serTiles) - (int32)offsetof(DRLGParams, arTiles)),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(DRLGParams, serCommands), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_DRLG_COMMAND, offsetof(DRLGParams, ptCommands), 0x21,
                                     (int32)offsetof(DRLGParams, serCommands) - (int32)offsetof(DRLGParams, ptCommands)),
    VariableArrayCountTypeDescriptorField("", offsetof(DRLGParams, nNumCommands), 0x10000, DT_DRLG_COMMAND,
                                          (int32)offsetof(DRLGParams, serCommands) - (int32)offsetof(DRLGParams, nNumCommands)),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(DRLGParams, serParentIndices), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_INT, offsetof(DRLGParams, pParentIndices), 0x21,
                                     (int32)offsetof(DRLGParams, serParentIndices) - (int32)offsetof(DRLGParams, pParentIndices)),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(DRLGParams, serDRLGTagMap), NULL, 0),
    TagMapTypeDescriptorField("", offsetof(DRLGParams, hDRLGTagMap), 0x21,
                              (int32)offsetof(DRLGParams, serDRLGTagMap) - (int32)offsetof(DRLGParams, hDRLGTagMap), 0xd0000),
    TypeDescriptorField(NULL, DT_NULL, sizeof(DRLGParams), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_DRLGParams("DRLGParams", 0, DRLGParams_Fields, 10, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 233
const XTypeDescriptor * DT_DRLGPARAMS = &TypeDescriptor_DRLGParams;

static const TypeDescriptorField WorldServerData_Fields[14] = {  // line 235
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(WorldServerData, serDRLGParams), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_DRLGPARAMS, offsetof(WorldServerData, ptDRLG), 0x21,
                                     (int32)offsetof(WorldServerData, serDRLGParams) - (int32)offsetof(WorldServerData, ptDRLG)),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(WorldServerData, serSceneParams), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_SCENEPARAMS, offsetof(WorldServerData, ptScene), 0x21,
                                     (int32)offsetof(WorldServerData, serSceneParams) - (int32)offsetof(WorldServerData, ptScene)),
    TypeDescriptorField("", DT_LABEL_RULE_SET, offsetof(WorldServerData, tLabelRuleSet), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(WorldServerData, nSceneClusterIDNext), NULL, 1),
    TypeDescriptorField("", DT_SCENE_CLUSTER_SET, offsetof(WorldServerData, tSceneClusterSet), NULL, 1),
    FilteredFixedArrayTypeDescriptorField("", DT_SNO, offsetof(WorldServerData, arNavMeshFuncs), NULL, 0, 4, 29, 0),
    FixedArrayTypeDescriptorField("", DT_GBID, offsetof(WorldServerData, arLootrunTiers), NULL, 1, LOOTRUN_TIER_COUNT, 0),
    FilterTypeDescriptorField("", DT_SNO, offsetof(WorldServerData, snoScript), "", 35, 0, -1),
    TypeDescriptorField("", DT_INT, offsetof(WorldServerData, nSeed), NULL, 1),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(WorldServerData, serWeatherSNOs), NULL, 0),
    FilteredVariableArrayTypeDescriptorField("", DT_SNO, offsetof(WorldServerData, arWeatherSNOs), 0x21,
                                             (int32)offsetof(WorldServerData, serWeatherSNOs) - (int32)offsetof(WorldServerData, arWeatherSNOs), 47),
    TypeDescriptorField(NULL, DT_NULL, sizeof(WorldServerData), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_WorldServerData("WorldServerData", 0, WorldServerData_Fields, 14, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 246
const XTypeDescriptor * DT_WORLD_SERVER_DATA = &TypeDescriptor_WorldServerData;

static const TypeDescriptorField WorldDefinition_Fields[10] = {  // line 248
    BoundedTypeDescriptorField("", DT_INT, offsetof(WorldDefinition, bDRLG), NULL, (const void *)FALSE, (const void *)TRUE),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(WorldDefinition, serServerData), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_WORLD_SERVER_DATA, offsetof(WorldDefinition, ptServerData), 0x421,
                                     (int32)offsetof(WorldDefinition, serServerData) - (int32)offsetof(WorldDefinition, ptServerData)),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(WorldDefinition, serMarkerSets), NULL, 0),
    FilteredVariableArrayTypeDescriptorField("", DT_SNO, offsetof(WorldDefinition, arMarkerSets), 0x123,
                                             (int32)offsetof(WorldDefinition, serMarkerSets) - (int32)offsetof(WorldDefinition, arMarkerSets), 24),
    TypeDescriptorField("", DT_ENVIRONMENT, offsetof(WorldDefinition, tEnvironment), NULL, 1),
    // The default is the bit pattern of 1.0f stored in the pointer.
    TypeDescriptorField("", DT_FLOAT, offsetof(WorldDefinition, flDeformationScale), (const void *)0x3f800000, 1),
    TypeDescriptorField("", DT_INT, offsetof(WorldDefinition, uFlags), NULL, 0x80001),
    TypeDescriptorField("", DT_FLOAT, offsetof(WorldDefinition, flPVPMinimapFeetPerPixel), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(WorldDefinition), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_WorldDefinition("Worlds", WORLD_XML_VERSION, WorldDefinition_Fields, 10, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 259
const XTypeDescriptor * DT_WORLD = &TypeDescriptor_WorldDefinition;

static const TypeDescriptorField RevealSceneMessage_Fields[12] = {  // line 261
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(RevealSceneMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(RevealSceneMessage, idSWorld), NULL, 1),
    TypeDescriptorField("", DT_SCENESPEC, offsetof(RevealSceneMessage, tSceneSpec), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(RevealSceneMessage, idSScene), NULL, 1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(RevealSceneMessage, snoScene), "", 33, 0, -1),
    TypeDescriptorField("", DT_PRTRANSFORM, offsetof(RevealSceneMessage, transform), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(RevealSceneMessage, idSSceneParent), NULL, 1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(RevealSceneMessage, snoSceneGroup), "", 34, 0, -1),
    FixedArrayTypeDescriptorField("arAppliedLabels", DT_GBID, offsetof(RevealSceneMessage, arAppliedLabels), NULL, 8, 256,
                                  (int32)offsetof(RevealSceneMessage, uLabelCount) - (int32)offsetof(RevealSceneMessage, arAppliedLabels)),
    FixedArrayTypeDescriptorField("snoActors", DT_SNO, offsetof(RevealSceneMessage, snoActors), NULL, 8, 256,
                                  (int32)offsetof(RevealSceneMessage, uActorCount) - (int32)offsetof(RevealSceneMessage, snoActors)),
    TypeDescriptorField("", DT_INT, offsetof(RevealSceneMessage, bIsVista), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(RevealSceneMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_RevealSceneMessage("RevealSceneMessage", 0, RevealSceneMessage_Fields, 12, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 273
const XTypeDescriptor * DT_REVEAL_SCENE_MESSAGE = &TypeDescriptor_RevealSceneMessage;

static const TypeDescriptorField DestroySceneMessage_Fields[4] = {  // line 275
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(DestroySceneMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(DestroySceneMessage, idSWorld), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(DestroySceneMessage, idSScene), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(DestroySceneMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_DestroySceneMessage("DestroySceneMessage", 0, DestroySceneMessage_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 279
const XTypeDescriptor * DT_DESTROY_SCENE_MESSAGE = &TypeDescriptor_DestroySceneMessage;

static const TypeDescriptorField SwapSceneMessage_Fields[5] = {  // line 281
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(SwapSceneMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(SwapSceneMessage, idSWorld), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(SwapSceneMessage, idSSceneNew), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(SwapSceneMessage, idSSceneOld), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(SwapSceneMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_SwapSceneMessage("SwapSceneMessage", 0, SwapSceneMessage_Fields, 5, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 286
const XTypeDescriptor * DT_SWAP_SCENE_MESSAGE = &TypeDescriptor_SwapSceneMessage;

static const TypeDescriptorField PreloadSceneDataMessage_Fields[5] = {  // line 288
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(PreloadSceneDataMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(PreloadSceneDataMessage, idSScene), NULL, 1),
    TypeDescriptorField("", DT_SNO, offsetof(PreloadSceneDataMessage, snoScene), NULL, 1),
    FixedArrayTypeDescriptorField("", DT_SNO, offsetof(PreloadSceneDataMessage, snoLevelAreas), NULL, 1, 4, 0),
    TypeDescriptorField(NULL, DT_NULL, sizeof(PreloadSceneDataMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_PreloadSceneDataMessage("PreloadSceneDataMessage", 0, PreloadSceneDataMessage_Fields, 5, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 293
const XTypeDescriptor * DT_PRELOAD_SCENE_DATA_MESSAGE = &TypeDescriptor_PreloadSceneDataMessage;

static const TypeDescriptorField PrefetchDataMessage_Fields[3] = {  // line 295
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(PrefetchDataMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_SNO, offsetof(PrefetchDataMessage, sno), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(PrefetchDataMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_PrefetchDataMessage("PrefetchDataMessage", 0, PrefetchDataMessage_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 298
const XTypeDescriptor * DT_PREFETCH_DATA_MESSAGE = &TypeDescriptor_PrefetchDataMessage;

static const uint32 WORLD_DEFAULT_TILE_SIZE = 200;  // line 304
static const uint32 WORLD_DEFAULT_GRID_TILES = 4;  // line 305
static const uint32 WORLD_DEFAULT_TILE_SET = 100;  // line 306
static const int32 WORLD_DEFAULT_EXIT_DIR_TYPE = -1;  // line 307
static const int32 WORLD_DEFAULT_TILE_EDGE_TYPE = 0;  // line 308
static const int32 WORLD_DEFAULT_TILE_CHOKEPOINT_TYPE = -1;  // line 309
static const int32 WORLD_DEFAULT_COMMAND_EDGE_TYPE = -1;  // line 310
static const int32 WORLD_DEFAULT_COMMAND_SHAPE_TYPE = -1;  // line 311
static const int32 WORLD_DEFAULT_LABEL_TYPE = -1;  // line 312
static const int32 WORLD_DEFAULT_COMMAND_NUBS_SPACING = 1;  // line 315
static const int32 WORLD_DEFAULT_SPECIAL_TILE_TYPE = -1;  // line 316
static const int32 WORLD_DEFAULT_OVERRIDE_GROUP_NUM = -1;  // line 317

const AxeTagInfo tWorldLayoutGroupTagInfo[23] = {  // line 319
    { TAG_WORLD_TILE_SIZE_X, AXETAGTYPE_INT, (void *)&WORLD_DEFAULT_TILE_SIZE, 0, "", "", "", AXETAGGROUP_DRLG_GROUP_RULE, -1, -1, -1 },
    { TAG_WORLD_TILE_SIZE_Y, AXETAGTYPE_INT, (void *)&WORLD_DEFAULT_TILE_SIZE, 0, "", "", "", AXETAGGROUP_DRLG_GROUP_RULE, -1, -1, -1 },
    { TAG_WORLD_GRID_TILES_X, AXETAGTYPE_INT, (void *)&WORLD_DEFAULT_GRID_TILES, 0, "", "", "", AXETAGGROUP_DRLG_GROUP_RULE, -1, -1, -1 },
    { TAG_WORLD_GRID_TILES_Y, AXETAGTYPE_INT, (void *)&WORLD_DEFAULT_GRID_TILES, 0, "", "", "", AXETAGGROUP_DRLG_GROUP_RULE, -1, -1, -1 },
    { TAG_WORLD_STITCH_SIZE_X, AXETAGTYPE_INT, (void *)&TAG_VALUE_ZERO, 0, "", "", "", AXETAGGROUP_DRLG_GROUP_RULE, -1, -1, -1 },
    { TAG_WORLD_STITCH_SIZE_Y, AXETAGTYPE_INT, (void *)&TAG_VALUE_ZERO, 0, "", "", "", AXETAGGROUP_DRLG_GROUP_RULE, -1, -1, -1 },
    { TAG_WORLD_LEVEL_AREA, AXETAGTYPE_SNO_LEVEL_AREA, (void *)&TAG_VALUE_NULL_SNO, 0, "", "", "", AXETAGGROUP_DRLG_GROUP_RULE, -1, -1, -1 },
    { TAG_WORLD_LEVEL_AREA_ALT1, AXETAGTYPE_SNO_LEVEL_AREA, (void *)&TAG_VALUE_NULL_SNO, 0, "", "", "", AXETAGGROUP_DRLG_GROUP_RULE, -1, -1, -1 },
    { TAG_WORLD_LEVEL_AREA_ALT2, AXETAGTYPE_SNO_LEVEL_AREA, (void *)&TAG_VALUE_NULL_SNO, 0, "", "", "", AXETAGGROUP_DRLG_GROUP_RULE, -1, -1, -1 },
    { TAG_WORLD_LEVEL_AREA_ALT3, AXETAGTYPE_SNO_LEVEL_AREA, (void *)&TAG_VALUE_NULL_SNO, 0, "", "", "", AXETAGGROUP_DRLG_GROUP_RULE, -1, -1, -1 },
    { TAG_WORLD_NEXT_WORLD_SNO, AXETAGTYPE_WORLD, (void *)&TAG_VALUE_NULL_SNO, 0, "", "", "", AXETAGGROUP_DRLG_GROUP_RULE, -1, -1, -1 },
    { TAG_WORLD_PREV_WORLD_SNO, AXETAGTYPE_WORLD, (void *)&TAG_VALUE_NULL_SNO, 0, "", "", "", AXETAGGROUP_DRLG_GROUP_RULE, -1, -1, -1 },
    { TAG_WORLD_MUSIC, AXETAGTYPE_MUSIC, (void *)&TAG_VALUE_NULL_SNO, 0, "", "", "", AXETAGGROUP_DRLG_GROUP_RULE, -1, -1, -1 },
    { TAG_WORLD_AMBIENT, AXETAGTYPE_SOUND, (void *)&TAG_VALUE_NULL_SNO, 0, "", "", "", AXETAGGROUP_DRLG_GROUP_RULE, -1, -1, -1 },
    { TAG_WORLD_WEATHER, AXETAGTYPE_WEATHER, (void *)&TAG_VALUE_NULL_SNO, 0, "", "", "", AXETAGGROUP_DRLG_GROUP_RULE, -1, -1, -1 },
    { TAG_WORLD_HEIGHT_CHANGE_TO_NORTH, AXETAGTYPE_INT, (void *)&TAG_VALUE_ZERO, 0, "", "", "", AXETAGGROUP_DRLG_GROUP_RULE, -1, -1, -1 },
    { TAG_WORLD_HEIGHT_CHANGE_TO_WEST, AXETAGTYPE_INT, (void *)&TAG_VALUE_ZERO, 0, "", "", "", AXETAGGROUP_DRLG_GROUP_RULE, -1, -1, -1 },
    { TAG_WORLD_PATCH_EXITS, AXETAGTYPE_BOOL, (void *)&TAG_VALUE_INT_ONE, 0, "", "", "", AXETAGGROUP_DRLG_GROUP_RULE, -1, -1, -1 },
    { TAG_WORLD_FIX_REPEATS, AXETAGTYPE_BOOL, (void *)&TAG_VALUE_ZERO, 0, "", "", "", AXETAGGROUP_DRLG_GROUP_RULE, -1, -1, -1 },
    { TAG_WORLD_REPEATS_TILE_SET, AXETAGTYPE_WORLD_TILE_SET, (void *)&TAG_VALUE_ZERO, 0, "", "", "", AXETAGGROUP_DRLG_GROUP_RULE, -1, -1, -1 },
    { TAG_WORLD_CHECK_FOR_SWASTIKAS, AXETAGTYPE_BOOL, (void *)&TAG_VALUE_ZERO, 0, "", "", "", AXETAGGROUP_DRLG_GROUP_RULE, -1, -1, -1 },
    { TAG_WORLD_USE_EDGE_TYPES, AXETAGTYPE_BOOL, (void *)&TAG_VALUE_ZERO, 0, "", "", "", AXETAGGROUP_DRLG_GROUP_RULE, -1, -1, -1 },
    { TAG_WORLD_MAX_CUR_GEN_VARIATION_PER_TILE_TYPE, AXETAGTYPE_INT, (void *)&TAG_VALUE_ZERO, 0, "", "", "", AXETAGGROUP_DRLG_GROUP_RULE, -1, -1, -1 },
};

const int32 gnWorldLayoutGroupTagCount = sizeof(tWorldLayoutGroupTagInfo) / sizeof(tWorldLayoutGroupTagInfo[0]);  // line 347

const AxeTagInfo tWorldTileTagInfo[27] = {  // line 349
    { TAG_WORLD_TILE_BORDER_N, AXETAGTYPE_WORLD_TILE_SET, (void *)&TAG_VALUE_ZERO, 0, "", "", "", AXETAGGROUP_DRLG_RULE, -1, -1, -1 },
    { TAG_WORLD_TILE_BORDER_NE, AXETAGTYPE_WORLD_TILE_SET, (void *)&TAG_VALUE_ZERO, 0, "", "", "", AXETAGGROUP_DRLG_RULE, -1, -1, -1 },
    { TAG_WORLD_TILE_BORDER_E, AXETAGTYPE_WORLD_TILE_SET, (void *)&TAG_VALUE_ZERO, 0, "", "", "", AXETAGGROUP_DRLG_RULE, -1, -1, -1 },
    { TAG_WORLD_TILE_BORDER_SE, AXETAGTYPE_WORLD_TILE_SET, (void *)&TAG_VALUE_ZERO, 0, "", "", "", AXETAGGROUP_DRLG_RULE, -1, -1, -1 },
    { TAG_WORLD_TILE_BORDER_S, AXETAGTYPE_WORLD_TILE_SET, (void *)&TAG_VALUE_ZERO, 0, "", "", "", AXETAGGROUP_DRLG_RULE, -1, -1, -1 },
    { TAG_WORLD_TILE_BORDER_SW, AXETAGTYPE_WORLD_TILE_SET, (void *)&TAG_VALUE_ZERO, 0, "", "", "", AXETAGGROUP_DRLG_RULE, -1, -1, -1 },
    { TAG_WORLD_TILE_BORDER_W, AXETAGTYPE_WORLD_TILE_SET, (void *)&TAG_VALUE_ZERO, 0, "", "", "", AXETAGGROUP_DRLG_RULE, -1, -1, -1 },
    { TAG_WORLD_TILE_BORDER_NW, AXETAGTYPE_WORLD_TILE_SET, (void *)&TAG_VALUE_ZERO, 0, "", "", "", AXETAGGROUP_DRLG_RULE, -1, -1, -1 },
    { TAG_WORLD_TILE_TRANSITION_N, AXETAGTYPE_WORLD_TILE_SET, (void *)&TAG_VALUE_ZERO, 0, "", "", "", AXETAGGROUP_DRLG_RULE, -1, -1, -1 },
    { TAG_WORLD_TILE_TRANSITION_E, AXETAGTYPE_WORLD_TILE_SET, (void *)&TAG_VALUE_ZERO, 0, "", "", "", AXETAGGROUP_DRLG_RULE, -1, -1, -1 },
    { TAG_WORLD_TILE_TRANSITION_S, AXETAGTYPE_WORLD_TILE_SET, (void *)&TAG_VALUE_ZERO, 0, "", "", "", AXETAGGROUP_DRLG_RULE, -1, -1, -1 },
    { TAG_WORLD_TILE_TRANSITION_W, AXETAGTYPE_WORLD_TILE_SET, (void *)&TAG_VALUE_ZERO, 0, "", "", "", AXETAGGROUP_DRLG_RULE, -1, -1, -1 },
    { TAG_WORLD_TILE_DEAD_END_CAP, AXETAGTYPE_BOOL, (void *)&TAG_VALUE_ZERO, 0, "", "", "", AXETAGGROUP_DRLG_RULE, -1, -1, -1 },
    { TAG_WORLD_TILE_EDGE_TYPE_N, AXETAGTYPE_WORLD_EDGE_TYPE, (void *)&WORLD_DEFAULT_TILE_EDGE_TYPE, 0, "", "", "", AXETAGGROUP_DRLG_RULE, -1, -1, -1 },
    { TAG_WORLD_TILE_EDGE_TYPE_S, AXETAGTYPE_WORLD_EDGE_TYPE, (void *)&WORLD_DEFAULT_TILE_EDGE_TYPE, 0, "", "", "", AXETAGGROUP_DRLG_RULE, -1, -1, -1 },
    { TAG_WORLD_TILE_EDGE_TYPE_E, AXETAGTYPE_WORLD_EDGE_TYPE, (void *)&WORLD_DEFAULT_TILE_EDGE_TYPE, 0, "", "", "", AXETAGGROUP_DRLG_RULE, -1, -1, -1 },
    { TAG_WORLD_TILE_EDGE_TYPE_W, AXETAGTYPE_WORLD_EDGE_TYPE, (void *)&WORLD_DEFAULT_TILE_EDGE_TYPE, 0, "", "", "", AXETAGGROUP_DRLG_RULE, -1, -1, -1 },
    { TAG_WORLD_TILE_MIN_FREQUENCY, AXETAGTYPE_INT, (void *)&TAG_VALUE_NONE, 0, "", "", "", AXETAGGROUP_DRLG_RULE, -1, -1, -1 },
    { TAG_WORLD_TILE_MAX_FREQUENCY, AXETAGTYPE_INT, (void *)&TAG_VALUE_NONE, 0, "", "", "", AXETAGGROUP_DRLG_RULE, -1, -1, -1 },
    { TAG_WORLD_TILE_SCENE_SIZE_X, AXETAGTYPE_INT, (void *)&TAG_VALUE_INT_ONE, 0, "", "", "", AXETAGGROUP_DRLG_RULE, -1, -1, -1 },
    { TAG_WORLD_TILE_SCENE_SIZE_Y, AXETAGTYPE_INT, (void *)&TAG_VALUE_INT_ONE, 0, "", "", "", AXETAGGROUP_DRLG_RULE, -1, -1, -1 },
    { TAG_WORLD_TILE_SPECIAL_TILE_TYPE, AXETAGTYPE_WORLD_SPECIAL_TILE_TYPE, (void *)&WORLD_DEFAULT_SPECIAL_TILE_TYPE, 0, "", "", "", AXETAGGROUP_DRLG_RULE, -1, -1, -1 },
    { TAG_WORLD_TILE_SPECIAL_TILE_MIN_SPACING_0, AXETAGTYPE_INT, (void *)&TAG_VALUE_ZERO, 0, "", "", "", AXETAGGROUP_DRLG_RULE, -1, -1, -1 },
    { TAG_WORLD_TILE_SPECIAL_TILE_MIN_SPACING_1, AXETAGTYPE_INT, (void *)&TAG_VALUE_ZERO, 0, "", "", "", AXETAGGROUP_DRLG_RULE, -1, -1, -1 },
    { TAG_WORLD_TILE_CHOKEPOINT_TYPE, AXETAGTYPE_WORLD_CHOKEPOINT_TYPE, (void *)&WORLD_DEFAULT_TILE_CHOKEPOINT_TYPE, 0, "", "", "", AXETAGGROUP_DRLG_RULE, -1, -1, -1 },
    { TAG_WORLD_TILE_CHOKEPOINT_DIRECTION, AXETAGTYPE_WORLD_CHOKEPOINT_DIR_TYPE, (void *)&WORLD_DEFAULT_EXIT_DIR_TYPE, 0, "", "", "", AXETAGGROUP_DRLG_RULE, -1, -1, -1 },
    { TAG_WORLD_TILE_IGNORE_NAVMESH, AXETAGTYPE_BOOL, (void *)&TAG_VALUE_ZERO, 0, "", "", "", AXETAGGROUP_DRLG_RULE, -1, -1, -1 },
};

const int32 gnWorldTileTagCount = sizeof(tWorldTileTagInfo) / sizeof(tWorldTileTagInfo[0]);  // line 379

const AxeTagInfo tWorldLayoutCommandTagInfo[83] = {  // line 382
    { TAG_WORLD_DRLG_COMMAND_ROOM_CNT, AXETAGTYPE_INT, (void *)&TAG_VALUE_INT_ONE, 0, "", "", "", AXETAGGROUP_DRLG_RULE, -1, -1, -1 },
    { TAG_WORLD_DRLG_COMMAND_ROOM_CNT_DELTA, AXETAGTYPE_INT, (void *)&TAG_VALUE_ZERO, 0, "", "", "", AXETAGGROUP_DRLG_RULE, -1, -1, -1 },
    { TAG_WORLD_DRLG_COMMAND_WEIGHT_NORTH, AXETAGTYPE_INT, (void *)&TAG_VALUE_INT_ONE, 0, "", "", "", AXETAGGROUP_DRLG_RULE, -1, -1, -1 },
    { TAG_WORLD_DRLG_COMMAND_WEIGHT_SOUTH, AXETAGTYPE_INT, (void *)&TAG_VALUE_INT_ONE, 0, "", "", "", AXETAGGROUP_DRLG_RULE, -1, -1, -1 },
    { TAG_WORLD_DRLG_COMMAND_WEIGHT_EAST, AXETAGTYPE_INT, (void *)&TAG_VALUE_INT_ONE, 0, "", "", "", AXETAGGROUP_DRLG_RULE, -1, -1, -1 },
    { TAG_WORLD_DRLG_COMMAND_WEIGHT_WEST, AXETAGTYPE_INT, (void *)&TAG_VALUE_INT_ONE, 0, "", "", "", AXETAGGROUP_DRLG_RULE, -1, -1, -1 },
    { TAG_WORLD_DRLG_COMMAND_TILE_SET, AXETAGTYPE_WORLD_TILE_SET, (void *)&WORLD_DEFAULT_TILE_SET, 0, "", "", "", AXETAGGROUP_DRLG_RULE, -1, -1, -1 },
    { TAG_WORLD_DRLG_COMMAND_TILE_TYPE, AXETAGTYPE_WORLD_TILE_TYPE, (void *)&TAG_VALUE_ZERO, 0, "", "", "", AXETAGGROUP_DRLG_RULE, -1, -1, -1 },
    { TAG_WORLD_DRLG_COMMAND_CONNECT_NEIGHBORS, AXETAGTYPE_BOOL, (void *)&TAG_VALUE_ZERO, 0, "", "", "", AXETAGGROUP_DRLG_RULE, -1, -1, -1 },
    { TAG_WORLD_DRLG_COMMAND_EDGE_TYPE, AXETAGTYPE_WORLD_EDGE_TYPE, (void *)&WORLD_DEFAULT_COMMAND_EDGE_TYPE, 0, "", "", "", AXETAGGROUP_DRLG_RULE, -1, -1, -1 },
    { TAG_WORLD_DRLG_COMMAND_CHANCE, AXETAGTYPE_FLOAT, (void *)&TAG_VALUE_FLOAT_ONE, 0, "", "", "", AXETAGGROUP_DRLG_RULE, -1, -1, -1 },
    { TAG_WORLD_DRLG_COMMAND_SELECT_WEIGHT, AXETAGTYPE_INT, (void *)&TAG_VALUE_INT_ONE, 0, "", "", "", AXETAGGROUP_DRLG_RULE, -1, -1, -1 },
    { TAG_WORLD_DRLG_COMMAND_PARENT_EDGE, AXETAGTYPE_WORLD_EDGE_TYPE, (void *)&WORLD_DEFAULT_COMMAND_EDGE_TYPE, 0, "", "", "", AXETAGGROUP_DRLG_RULE, -1, -1, -1 },
    { TAG_WORLD_DRLG_COMMAND_HUB_SIZE, AXETAGTYPE_INT, (void *)&TAG_VALUE_ZERO, 0, "", "", "", AXETAGGROUP_DRLG_RULE, -1, -1, -1 },
    { TAG_WORLD_DRLG_COMMAND_HUB_SIZE_DELTA, AXETAGTYPE_INT, (void *)&TAG_VALUE_ZERO, 0, "", "", "", AXETAGGROUP_DRLG_RULE, -1, -1, -1 },
    { TAG_WORLD_DRLG_COMMAND_HUB_LENGTH, AXETAGTYPE_INT, (void *)&TAG_VALUE_ZERO, 0, "", "", "", AXETAGGROUP_DRLG_RULE, -1, -1, -1 },
    { TAG_WORLD_DRLG_COMMAND_HUB_LENGTH_DELTA, AXETAGTYPE_INT, (void *)&TAG_VALUE_ZERO, 0, "", "", "", AXETAGGROUP_DRLG_RULE, -1, -1, -1 },
    { TAG_WORLD_DRLG_COMMAND_HUB_WIDTH, AXETAGTYPE_INT, (void *)&TAG_VALUE_ZERO, 0, "", "", "", AXETAGGROUP_DRLG_RULE, -1, -1, -1 },
    { TAG_WORLD_DRLG_COMMAND_HUB_WIDTH_DELTA, AXETAGTYPE_INT, (void *)&TAG_VALUE_ZERO, 0, "", "", "", AXETAGGROUP_DRLG_RULE, -1, -1, -1 },
    { TAG_WORLD_DRLG_COMMAND_ADD_SPECIAL_TILE_1, AXETAGTYPE_WORLD_TILE_SET, (void *)&WORLD_DEFAULT_TILE_SET, 0, "", "", "", AXETAGGROUP_DRLG_RULE, -1, -1, -1 },
    { TAG_WORLD_DRLG_COMMAND_ADD_SPECIAL_TILE_2, AXETAGTYPE_WORLD_TILE_SET, (void *)&WORLD_DEFAULT_TILE_SET, 0, "", "", "", AXETAGGROUP_DRLG_RULE, -1, -1, -1 },
    { TAG_WORLD_DRLG_COMMAND_ADD_SPECIAL_TILE_3, AXETAGTYPE_WORLD_TILE_SET, (void *)&WORLD_DEFAULT_TILE_SET, 0, "", "", "", AXETAGGROUP_DRLG_RULE, -1, -1, -1 },
    { TAG_WORLD_DRLG_COMMAND_ALLOW_ROTATION, AXETAGTYPE_BOOL, (void *)&TAG_VALUE_INT_ONE, 0, "", "", "", AXETAGGROUP_DRLG_RULE, -1, -1, -1 },
    { TAG_WORLD_DRLG_COMMAND_HUB_FLEXIBLE_DIMENSIONS, AXETAGTYPE_BOOL, (void *)&TAG_VALUE_ZERO, 0, "", "", "", AXETAGGROUP_DRLG_RULE, -1, -1, -1 },
    { TAG_WORLD_DRLG_COMMAND_SPOKE_CONNECT_END, AXETAGTYPE_BOOL, (void *)&TAG_VALUE_ZERO, 0, "", "", "", AXETAGGROUP_DRLG_RULE, -1, -1, -1 },
    { TAG_WORLD_DRLG_COMMAND_DEAD_END_MAX_NUM, AXETAGTYPE_INT, (void *)&TAG_VALUE_ZERO, 0, "", "", "", AXETAGGROUP_DRLG_RULE, -1, -1, -1 },
    { TAG_WORLD_DRLG_COMMAND_DEAD_END_MAX_LEN, AXETAGTYPE_INT, (void *)&TAG_VALUE_ZERO, 0, "", "", "", AXETAGGROUP_DRLG_RULE, -1, -1, -1 },
    { TAG_WORLD_DRLG_COMMAND_MAX_EXITS, AXETAGTYPE_INT, (void *)&TAG_VALUE_ZERO, 0, "", "", "", AXETAGGROUP_DRLG_RULE, -1, -1, -1 },
    { TAG_WORLD_DRLG_COMMAND_ROOM_CREATE_NUM, AXETAGTYPE_INT, (void *)&TAG_VALUE_ZERO, 0, "", "", "", AXETAGGROUP_DRLG_RULE, -1, -1, -1 },
    { TAG_WORLD_DRLG_COMMAND_START_GROUP_NUM, AXETAGTYPE_INT, (void *)&TAG_VALUE_ZERO, 0, "", "", "", AXETAGGROUP_DRLG_GROUP_RULE, -1, -1, -1 },
    { TAG_WORLD_DRLG_COMMAND_GROUP_NUM, AXETAGTYPE_INT, (void *)&TAG_VALUE_ZERO, 0, "", "", "", AXETAGGROUP_DRLG_GROUP_RULE, -1, -1, -1 },
    { TAG_WORLD_LEVEL_AREA, AXETAGTYPE_SNO_LEVEL_AREA, (void *)&TAG_VALUE_NULL_SNO, 0, "", "", "", AXETAGGROUP_DRLG_GROUP_RULE, -1, -1, -1 },
    { TAG_WORLD_LEVEL_AREA_ALT1, AXETAGTYPE_SNO_LEVEL_AREA, (void *)&TAG_VALUE_NULL_SNO, 0, "", "", "", AXETAGGROUP_DRLG_GROUP_RULE, -1, -1, -1 },
    { TAG_WORLD_LEVEL_AREA_ALT2, AXETAGTYPE_SNO_LEVEL_AREA, (void *)&TAG_VALUE_NULL_SNO, 0, "", "", "", AXETAGGROUP_DRLG_GROUP_RULE, -1, -1, -1 },
    { TAG_WORLD_LEVEL_AREA_ALT3, AXETAGTYPE_SNO_LEVEL_AREA, (void *)&TAG_VALUE_NULL_SNO, 0, "", "", "", AXETAGGROUP_DRLG_GROUP_RULE, -1, -1, -1 },
    { TAG_WORLD_NEXT_WORLD_SNO, AXETAGTYPE_WORLD, (void *)&TAG_VALUE_NULL_SNO, 0, "", "", "", AXETAGGROUP_DRLG_GROUP_RULE, -1, -1, -1 },
    { TAG_WORLD_NEXT_ENTRANCE, AXETAGTYPE_STARTLOC_NAME, (void *)&TAG_VALUE_NONE, 0, "", "", "", AXETAGGROUP_DRLG_GROUP_RULE, -1, -1, -1 },
    { TAG_WORLD_PREV_WORLD_SNO, AXETAGTYPE_WORLD, (void *)&TAG_VALUE_NULL_SNO, 0, "", "", "", AXETAGGROUP_DRLG_GROUP_RULE, -1, -1, -1 },
    { TAG_WORLD_PREV_ENTRANCE, AXETAGTYPE_STARTLOC_NAME, (void *)&TAG_VALUE_NONE, 0, "", "", "", AXETAGGROUP_DRLG_GROUP_RULE, -1, -1, -1 },
    { TAG_WORLD_NEXT_LEVEL_AREA, AXETAGTYPE_SNO_LEVEL_AREA, (void *)&TAG_VALUE_NULL_SNO, 0, "", "", "", AXETAGGROUP_DRLG_GROUP_RULE, -1, -1, -1 },
    { TAG_WORLD_PREV_LEVEL_AREA, AXETAGTYPE_SNO_LEVEL_AREA, (void *)&TAG_VALUE_NULL_SNO, 0, "", "", "", AXETAGGROUP_DRLG_GROUP_RULE, -1, -1, -1 },
    { TAG_WORLD_MUSIC, AXETAGTYPE_MUSIC, (void *)&TAG_VALUE_NULL_SNO, 0, "", "", "", AXETAGGROUP_DRLG_GROUP_RULE, -1, -1, -1 },
    { TAG_WORLD_AMBIENT, AXETAGTYPE_SOUND, (void *)&TAG_VALUE_NULL_SNO, 0, "", "", "", AXETAGGROUP_DRLG_GROUP_RULE, -1, -1, -1 },
    { TAG_WORLD_WEATHER, AXETAGTYPE_WEATHER, (void *)&TAG_VALUE_NULL_SNO, 0, "", "", "", AXETAGGROUP_DRLG_GROUP_RULE, -1, -1, -1 },
    { TAG_WORLD_DRLG_COMMAND_SELECT_NUM, AXETAGTYPE_INT, (void *)&TAG_VALUE_INT_ONE, 0, "", "", "", AXETAGGROUP_DRLG_RULE, -1, -1, -1 },
    { TAG_WORLD_DRLG_COMMAND_SELECT_DELTA, AXETAGTYPE_INT, (void *)&TAG_VALUE_ZERO, 0, "", "", "", AXETAGGROUP_DRLG_RULE, -1, -1, -1 },
    { TAG_WORLD_DRLG_COMMAND_TILE_SET_ENTRANCE, AXETAGTYPE_WORLD_TILE_SET, (void *)&WORLD_DEFAULT_TILE_SET, 0, "", "", "", AXETAGGROUP_DRLG_RULE, -1, -1, -1 },
    { TAG_WORLD_DRLG_COMMAND_TILE_TYPE_ENTRANCE, AXETAGTYPE_WORLD_TILE_TYPE, (void *)&TAG_VALUE_ZERO, 0, "", "", "", AXETAGGROUP_DRLG_RULE, -1, -1, -1 },
    { TAG_WORLD_DRLG_COMMAND_TILE_SET_EXIT, AXETAGTYPE_WORLD_TILE_SET, (void *)&WORLD_DEFAULT_TILE_SET, 0, "", "", "", AXETAGGROUP_DRLG_RULE, -1, -1, -1 },
    { TAG_WORLD_DRLG_COMMAND_TILE_TYPE_EXIT, AXETAGTYPE_WORLD_TILE_TYPE, (void *)&TAG_VALUE_ZERO, 0, "", "", "", AXETAGGROUP_DRLG_RULE, -1, -1, -1 },
    { TAG_WORLD_DRLG_COMMAND_MIN_DISTANCE, AXETAGTYPE_INT, (void *)&TAG_VALUE_ZERO, 0, "", "", "", AXETAGGROUP_DRLG_RULE, -1, -1, -1 },
    { TAG_WORLD_DRLG_COMMAND_MAX_DISTANCE, AXETAGTYPE_INT, (void *)&TAG_VALUE_ZERO, 0, "", "", "", AXETAGGROUP_DRLG_RULE, -1, -1, -1 },
    { TAG_WORLD_DRLG_COMMAND_RETRY_ANYWHERE, AXETAGTYPE_BOOL, (void *)&TAG_VALUE_INT_ONE, 0, "", "", "", AXETAGGROUP_DRLG_RULE, -1, -1, -1 },
    { TAG_WORLD_DRLG_COMMAND_MAX_EXIT_CNT, AXETAGTYPE_INT, (void *)&TAG_VALUE_ZERO, 0, "", "", "", AXETAGGROUP_DRLG_RULE, -1, -1, -1 },
    { TAG_WORLD_DRLG_COMMAND_MAX_EXIT_CNT_DELTA, AXETAGTYPE_INT, (void *)&TAG_VALUE_ZERO, 0, "", "", "", AXETAGGROUP_DRLG_RULE, -1, -1, -1 },
    { TAG_WORLD_DRLG_COMMAND_ALLOWABLE_EXITS, AXETAGTYPE_WORLD_TILE_TYPE, (void *)&TAG_VALUE_ZERO, 0, "", "", "", AXETAGGROUP_DRLG_RULE, -1, -1, -1 },
    { TAG_WORLD_DRLG_COMMAND_DEAD_END_CONNECT_TILES, AXETAGTYPE_BOOL, (void *)&TAG_VALUE_ZERO, 0, "", "", "", AXETAGGROUP_DRLG_RULE, -1, -1, -1 },
    { TAG_WORLD_DRLG_COMMAND_DEAD_END_CONNECT_FIRST_ONLY, AXETAGTYPE_BOOL, (void *)&TAG_VALUE_ZERO, 0, "", "", "", AXETAGGROUP_DRLG_RULE, -1, -1, -1 },
    { TAG_WORLD_DRLG_COMMAND_ENABLED, AXETAGTYPE_BOOL, (void *)&TAG_VALUE_INT_ONE, 0, "", "", "", AXETAGGROUP_DRLG_GROUP_RULE, -1, -1, -1 },
    { TAG_WORLD_DRLG_COMMAND_REROLL_ON_FAIL, AXETAGTYPE_BOOL, (void *)&TAG_VALUE_ZERO, 0, "", "", "", AXETAGGROUP_DRLG_RULE, -1, -1, -1 },
    { TAG_WORLD_DRLG_COMMAND_ALL_TILES_CONNECTED, AXETAGTYPE_BOOL, (void *)&TAG_VALUE_ZERO, 0, "", "", "", AXETAGGROUP_DRLG_RULE, -1, -1, -1 },
    { TAG_WORLD_DRLG_COMMAND_SHAPE_TYPE, AXETAGTYPE_WORLD_SHAPE_TYPE, (void *)&WORLD_DEFAULT_COMMAND_SHAPE_TYPE, 0, "", "", "", AXETAGGROUP_DRLG_RULE, -1, -1, -1 },
    { TAG_WORLD_DRLG_COMMAND_SHAPE_PARAM, AXETAGTYPE_FLOAT, (void *)&TAG_VALUE_ZERO, 0, "", "", "", AXETAGGROUP_DRLG_RULE, -1, -1, -1 },
    { TAG_WORLD_DRLG_COMMAND_ROAD_LENGTH_MIN, AXETAGTYPE_INT, (void *)&TAG_VALUE_INT_ONE, 0, "", "", "", AXETAGGROUP_DRLG_RULE, -1, -1, -1 },
    { TAG_WORLD_DRLG_COMMAND_LABEL, AXETAGTYPE_WORLD_LABEL_TYPE, (void *)&WORLD_DEFAULT_LABEL_TYPE, 0, "", "", "", AXETAGGROUP_DRLG_RULE, -1, -1, -1 },
    { TAG_WORLD_DRLG_COMMAND_SUBSTITUTE_ENTRANCE_LABEL, AXETAGTYPE_WORLD_LABEL_TYPE, (void *)&WORLD_DEFAULT_LABEL_TYPE, 0, "", "", "", AXETAGGROUP_DRLG_RULE, -1, -1, -1 },
    { TAG_WORLD_DRLG_COMMAND_SUBSTITUTE_EXIT_LABEL, AXETAGTYPE_WORLD_LABEL_TYPE, (void *)&WORLD_DEFAULT_LABEL_TYPE, 0, "", "", "", AXETAGGROUP_DRLG_RULE, -1, -1, -1 },
    { TAG_WORLD_DRLG_COMMAND_SUBSTITUTE_ENTRANCE_OVERRIDE_GROUP_NUM, AXETAGTYPE_INT, (void *)&WORLD_DEFAULT_OVERRIDE_GROUP_NUM, 0, "", "", "", AXETAGGROUP_DRLG_RULE, -1, -1, -1 },
    { TAG_WORLD_DRLG_COMMAND_SUBSTITUTE_EXIT_OVERRIDE_GROUP_NUM, AXETAGTYPE_INT, (void *)&WORLD_DEFAULT_OVERRIDE_GROUP_NUM, 0, "", "", "", AXETAGGROUP_DRLG_RULE, -1, -1, -1 },
    { TAG_WORLD_DRLG_COMMAND_NUBS_MAX, AXETAGTYPE_INT, (void *)&TAG_VALUE_INT_ONE, 0, "", "", "", AXETAGGROUP_DRLG_RULE, -1, -1, -1 },
    { TAG_WORLD_DRLG_COMMAND_NUBS_DELTA, AXETAGTYPE_INT, (void *)&TAG_VALUE_ZERO, 0, "", "", "", AXETAGGROUP_DRLG_RULE, -1, -1, -1 },
    { TAG_WORLD_DRLG_COMMAND_NUBS_SPACING, AXETAGTYPE_INT, (void *)&WORLD_DEFAULT_COMMAND_NUBS_SPACING, 0, "", "", "", AXETAGGROUP_DRLG_RULE, -1, -1, -1 },
    { TAG_WORLD_DRLG_COMMAND_NUBS_WIDTH_MIN, AXETAGTYPE_INT, (void *)&TAG_VALUE_INT_ONE, 0, "", "", "", AXETAGGROUP_DRLG_RULE, -1, -1, -1 },
    { TAG_WORLD_DRLG_COMMAND_NUBS_WIDTH_DELTA, AXETAGTYPE_INT, (void *)&TAG_VALUE_ZERO, 0, "", "", "", AXETAGGROUP_DRLG_RULE, -1, -1, -1 },
    { TAG_WORLD_DRLG_COMMAND_NUBS_LENGTH_MIN, AXETAGTYPE_INT, (void *)&TAG_VALUE_INT_ONE, 0, "", "", "", AXETAGGROUP_DRLG_RULE, -1, -1, -1 },
    { TAG_WORLD_DRLG_COMMAND_NUBS_LENGTH_DELTA, AXETAGTYPE_INT, (void *)&TAG_VALUE_ZERO, 0, "", "", "", AXETAGGROUP_DRLG_RULE, -1, -1, -1 },
    { TAG_WORLD_DRLG_COMMAND_CHOKEPOINT_EXCLUDE_DIST, AXETAGTYPE_INT, (void *)&TAG_VALUE_ZERO, 0, "", "", "", AXETAGGROUP_DRLG_RULE, -1, -1, -1 },
    { TAG_WORLD_DRLG_COMMAND_SPOKE_NO_REPEAT_TYPES, AXETAGTYPE_BOOL, (void *)&TAG_VALUE_ZERO, 0, "", "", "", AXETAGGROUP_DRLG_RULE, -1, -1, -1 },
    { TAG_WORLD_DRLG_COMMAND_OVERRIDE_GROUP_NUM, AXETAGTYPE_INT, (void *)&WORLD_DEFAULT_OVERRIDE_GROUP_NUM, 0, "", "", "", AXETAGGROUP_DRLG_RULE, -1, -1, -1 },
    { TAG_WORLD_DRLG_COMMAND_TILE_TYPE_ALT_1, AXETAGTYPE_WORLD_TILE_TYPE, (void *)&TAG_VALUE_ZERO, 0, "", "", "", AXETAGGROUP_DRLG_RULE, -1, -1, -1 },
    { TAG_WORLD_DRLG_COMMAND_TILE_TYPE_ALT_2, AXETAGTYPE_WORLD_TILE_TYPE, (void *)&TAG_VALUE_ZERO, 0, "", "", "", AXETAGGROUP_DRLG_RULE, -1, -1, -1 },
    { TAG_WORLD_DRLG_COMMAND_TILE_TYPE_ALT_3, AXETAGTYPE_WORLD_TILE_TYPE, (void *)&TAG_VALUE_ZERO, 0, "", "", "", AXETAGGROUP_DRLG_RULE, -1, -1, -1 },
    { TAG_WORLD_DRLG_COMMAND_DOESNT_GENERATE_PERIMETER, AXETAGTYPE_BOOL, (void *)&TAG_VALUE_ZERO, 0, "", "", "", AXETAGGROUP_DRLG_RULE, -1, -1, -1 },
};

const int32 gnWorldLayoutCommandTagCount = sizeof(tWorldLayoutCommandTagInfo) / sizeof(tWorldLayoutCommandTagInfo[0]);  // line 512

WorldGroupDef * gptGroupDefWorld;  // line 518

// line 524
WorldGroupDef::WorldGroupDef()
    : GroupDefinition("Worlds", WORLD_SNO, SNOVersions(WORLD_PC_SNO_VERSION, 258, WORLD_CONSOLE_SNO_VERSION, WORLD_CONSOLE_SNO_VERSION, 253),
                      SNODATATYPE_TITLE, sizeof(WorldDefinition), DT_WORLD, 0x5041, 0x501e) {
}

// line 553
uint32 WorldGroupDef::GetMaxCount() {
    return 48;
}

// line 653. Not in the binary as a function (inlined into GetDefaultData only), so inline.
inline void EnvironmentGetDefaults(Environment & tEnvironment) {
    PostFXParams & tPostFXParams = tEnvironment.tPostFXParams;

    tEnvironment.rgbaSkyColor = crgbaBlack;
    SigmaMemoryFill(&tEnvironment, 0, sizeof(Environment));

    tPostFXParams.flFilterOffset[0] = 0.5f;
    tPostFXParams.flFilterOffset[1] = 3.5f;
    tPostFXParams.flFilterCoeff[0] = 0.27f;
    tPostFXParams.flFilterCoeff[1] = 0.12f;

    tEnvironment.snoSkyboxActor = SNO_NONE;
    tEnvironment.snoMusic = SNO_NONE;
    tEnvironment.snoIntroMusic = SNO_NONE;
    tEnvironment.flIntroMusicCooldownSecsMin = 0.0f;
    tEnvironment.flIntroMusicCooldownSecsMax = 0.0f;
    tEnvironment.snoAmbient = SNO_NONE;
    tEnvironment.snoReverb = SNO_NONE;
    tEnvironment.snoWeather = GlobalSNOGet(GLOBALSNO_WEATHER_NO_WEATHER);
    tEnvironment.snoIrradianceTex = GlobalSNOGet(GLOBALSNO_SUBOBJECT_IRRADIANCE_TEXTURE);
    tEnvironment.snoIrradianceTexDead = GlobalSNOGet(GLOBALSNO_SUBOBJECT_IRRADIANCE_TEXTURE);
}

// line 565
void WorldGroupDef::GetDefaultData(void * ptDefaultDef) const {
    SigmaMemoryClear(ptDefaultDef, sizeof(WorldDefinition));

    WorldDefinition * pdef = (WorldDefinition *)ptDefaultDef;
    pdef->ptServerData = (WorldServerData *)SigmaMemoryNew(sizeof(WorldServerData), 4, NULL, TRUE);
    pdef->serServerData.dwSize = sizeof(WorldServerData);

    EnvironmentGetDefaults(pdef->tEnvironment);

    pdef->flDeformationScale = 1.0f;

    for (int32 i = 0; i < LOOTRUN_TIER_COUNT; i++) {
        pdef->ptServerData->arLootrunTiers[i] = GBID_NONE;
    }
}

// line 622
void SceneSpecInitialize(SceneSpecification & tSceneSpec) {
    tSceneSpec.dHeight = 0;
    tSceneSpec.pointGridLocation = cgptNull;
    for (int32 i = 0; i < 4; i++)
        tSceneSpec.arSnoLevelAreas[i] = SNO_NONE;
    tSceneSpec.snoPrevWorld = SNO_NONE;
    tSceneSpec.uPrevEntranceGUID = (uint32)-1;
    tSceneSpec.snoNextWorld = SNO_NONE;
    tSceneSpec.uNextEntranceGUID = (uint32)-1;
    tSceneSpec.snoPresetWorld = SNO_NONE;
    tSceneSpec.nDRLGIndex = -1;
    tSceneSpec.snoMusic = SNO_NONE;
    tSceneSpec.snoAmbient = SNO_NONE;
    tSceneSpec.snoReverb = SNO_NONE;
    tSceneSpec.snoWeather = SNO_NONE;
    tSceneSpec.nSceneChunk = -1;
    tSceneSpec.dwOnPathBits = 0;
    tSceneSpec.nTileType = 0;
    tSceneSpec.nTileFamily = 0;
    tSceneSpec.uClusterID = (uint32)-1;
    tSceneSpec.tCachedValues.dwCachedValuesValid = 0;
}

// line 1044
void WorldGroupDef::Postprocess(void * lpDefinition) {
    GroupDefinition::Postprocess(lpDefinition);

    WorldDefinition * pdefWorld = (WorldDefinition *)lpDefinition;

    if (pdefWorld->ptServerData && pdefWorld->ptServerData->ptScene) {
        SceneParams & tScene = *pdefWorld->ptServerData->ptScene;
        for (int32 i = 0; i < tScene.nSceneChunkCount; i++) {
            SceneChunk & tSceneChunk = tScene.ptSceneChunks[i];
            tSceneChunk.tSceneSpec.tCachedValues.dwCachedValuesValid = 0;
        }
    }
}

// line 1071
SNOPackType WorldGroupDef::GetSNOPackDestination(const void * ptDefinition) const {
    const WorldDefinition * pdefWorld = (const WorldDefinition *)ptDefinition;
    SNO snoWorld = pdefWorld->tHeader.snoHandle;
    SNOPackType eSNOPackType;
    if (snoWorld == GlobalSNOGet(GLOBALSNO_WORLD_BATTLENET_MAINSCREEN) ||
        snoWorld == GlobalSNOGet(GLOBALSNO_WORLD_BATTLENET_ACT1SCREEN) ||
        snoWorld == GlobalSNOGet(GLOBALSNO_WORLD_BATTLENET_ACT2SCREEN) ||
        snoWorld == GlobalSNOGet(GLOBALSNO_WORLD_BATTLENET_ACT3SCREEN) ||
        snoWorld == GlobalSNOGet(GLOBALSNO_WORLD_BATTLENET_ACT4SCREEN) ||
        snoWorld == GlobalSNOGet(GLOBALSNO_WORLD_BATTLENET_ACT5SCREEN) ||
        snoWorld == GlobalSNOGet(GLOBALSNO_WORLD_START_MENU_WORLD) ||
        snoWorld == GlobalSNOGet(GLOBALSNO_WORLD_LOAD_PLAYER_WORLD)) {
        eSNOPackType = SNOPACKTYPE_COREDATA;
    } else {
        eSNOPackType = SNOPACKTYPE_SERVERDATA;
    }

    return eSNOPackType;
}

// line 1102
void WorldGroupDef::Delete(void * lpDefinition) const {
    GroupDefinition::Delete(lpDefinition);
}

// line 1224
void WorldGroupDef::Read(const RequiredDefinitionHeader & tHeader, void *& lpDefinition, SNOElement * ptSNOElement, BOOL fContiguousBuffer,
                         uint32 dwContiguousSize, XMemoryAllocatorInterface * ptMemoryInterface) const {
    GroupDefinition::Read(tHeader, lpDefinition, ptSNOElement, fContiguousBuffer, dwContiguousSize, ptMemoryInterface);
}
