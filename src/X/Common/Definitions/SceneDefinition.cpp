// Original file: X/Common/Definitions/SceneDefinition.cpp

#include "SceneDefinition.h"

// For cgplaceNull, whose initializer the original runs in this file's static initializer.
#include "../WorldPlace.h"
#include "../../Client/Gfx/GfxVB.h"
#include "../D3TypeDescriptor/D3TypeDescriptor.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/RefString.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptorField.h"

// Defined in X/Sigma/Files/XFilesCommon.cpp:20; no adopted header declares it yet.
extern const XTypeDescriptor * DT_SERIALIZEDATA;

// Defined in X/Common/D3TypeDescriptor/D3DescribedType.cpp; no adopted header declares it yet.
extern const XTypeDescriptor * DT_SNO;

// Defined in X/Common/Nav/NavigationMesh.cpp:55 and :60; no adopted header declares them yet.
extern const XTypeDescriptor * DT_NAVMESH_SQUARE;
extern const XTypeDescriptor * DT_COMP_NAVMESH_SQUARE;

// Defined in X/Common/SNO/SNOFiles.cpp:7271; no adopted header declares it yet.
CRefString SNOToString(const SNOGroup snoGroup, const SNO snoHandle, const BOOL fUsedForXMLFields);

enum {
    SCENE_XML_VERSION = 11,
    SCENE_COMMON_SNO_VERSION = 170,
    SCENE_PC_SNO_VERSION = 171,
    SCENE_CONSOLE_SNO_VERSION = 178,
    SCENE_PS4_SNO_VERSION = 179,
    SCENE_AXE_SNO_VERSION = 179,
    DEFAULT_WIND_DURATION = 18000,
    DEFAULT_WEATHER_DURATION = 18000,
};

LPCSTR gszMinimapTexturePrefix = "mmap_";  // line 42

// _GLOBAL__sub_I_SceneDefinition.cpp: the translation unit also initializes the four RGBAColor constants
// of X/Client/Gfx/GfxVB.h:351-354 (included above). The field names are "" in the binary.
static const TypeDescriptorField NavMeshDef_Fields[17] = {  // line 69
    TypeDescriptorField("", DT_INT, offsetof(NavMesh, nSizeX), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(NavMesh, nSizeY), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(NavMesh, eNavMeshType), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(NavMesh, nNumSquares), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(NavMeshDef, nEdgeTopSize), NULL, 0x700000),
    TypeDescriptorField("", DT_INT, offsetof(NavMeshDef, nEdgeBottomSize), NULL, 0x700000),
    TypeDescriptorField("", DT_INT, offsetof(NavMeshDef, nEdgeLeftSize), NULL, 0x700000),
    TypeDescriptorField("", DT_INT, offsetof(NavMeshDef, nEdgeRightSize), NULL, 0x700000),
    TypeDescriptorField("", DT_FLOAT, offsetof(NavMeshDef, flMinZ), NULL, 0x700000),
    TypeDescriptorField("", DT_FLOAT, offsetof(NavMeshDef, flMaxZ), NULL, 0x700000),
    TypeDescriptorField("", DT_NAVMESH_SQUARE, offsetof(NavMeshDef, tEdge), NULL, 0x700000),
    FixedArrayTypeDescriptorField("", DT_INT, offsetof(NavMeshDef, arNumNodesForDistByLevelArea), NULL, 0x700000, 4, 0),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(NavMeshDef, serCompNavMeshArraySquares), NULL, 0x700000),
    VariableArrayTypeDescriptorField("", DT_COMP_NAVMESH_SQUARE, offsetof(NavMeshDef, ptCompNavMeshArraySquares), 0x700020,
                                     (int32)offsetof(NavMeshDef, serCompNavMeshArraySquares) - (int32)offsetof(NavMeshDef, ptCompNavMeshArraySquares)),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(NavMeshDef, serCompNavMeshArraySquaresSpecialFlags), NULL, 0x700000),
    VariableArrayTypeDescriptorField("", DT_BYTE, offsetof(NavMeshDef, ptCompNavMeshArraySquaresSpecialFlags), 0x700020,
                                     (int32)offsetof(NavMeshDef, serCompNavMeshArraySquaresSpecialFlags) - (int32)offsetof(NavMeshDef, ptCompNavMeshArraySquaresSpecialFlags)),
    TypeDescriptorField(NULL, DT_NULL, sizeof(NavMeshDef), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_NavMeshDef("NavMeshDef", 0, NavMeshDef_Fields, 17, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 88
const XTypeDescriptor * DT_NAVMESH = &TypeDescriptor_NavMeshDef;

static const TypeDescriptorField NavCell_Fields[6] = {  // line 91
    TypeDescriptorField("", DT_VECTOR3D, offsetof(NavCell, tMin), NULL, 1),
    TypeDescriptorField("", DT_VECTOR3D, offsetof(NavCell, tMax), NULL, 1),
    TypeDescriptorField("", DT_WORD, offsetof(NavCell, uFlags), NULL, 1),
    TypeDescriptorField("", DT_WORD, offsetof(NavCell, uNeighborCount), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(NavCell, uNeighborOffset), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(NavCell), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_NavCell("NavCell", 0, NavCell_Fields, 6, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 97
const XTypeDescriptor * DT_NAV_CELL = &TypeDescriptor_NavCell;

static const TypeDescriptorField NavGridSquare_Fields[4] = {  // line 99
    TypeDescriptorField("", DT_WORD, offsetof(NavGridSquare, uCellFlags), NULL, 1),
    TypeDescriptorField("", DT_WORD, offsetof(NavGridSquare, uCellCount), NULL, 1),
    TypeDescriptorField("", DT_WORD, offsetof(NavGridSquare, uLookupOffset), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(NavGridSquare), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_NavGridSquare("NavGridSquare", 0, NavGridSquare_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 103
const XTypeDescriptor * DT_NAV_GRID_SQUARE = &TypeDescriptor_NavGridSquare;

static const TypeDescriptorField NavCellLookup_Fields[3] = {  // line 105
    TypeDescriptorField("", DT_WORD, offsetof(NavCellLookup, uCellFlags), NULL, 1),
    TypeDescriptorField("", DT_WORD, offsetof(NavCellLookup, uCellIndex), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(NavCellLookup), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_NavCellLookup("NavCellLookup", 0, NavCellLookup_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 108
const XTypeDescriptor * DT_NAV_CELL_LOOKUP = &TypeDescriptor_NavCellLookup;

static const TypeDescriptorField NavCellBorderData_Fields[3] = {  // line 110
    TypeDescriptorField("", DT_WORD, offsetof(NavCellBorderData, uCellIndex), NULL, 1),
    TypeDescriptorField("", DT_WORD, offsetof(NavCellBorderData, uBorderFlags), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(NavCellBorderData), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_NavCellBorderData("NavCellBorderData", 0, NavCellBorderData_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 113
const XTypeDescriptor * DT_NAV_CELL_BORDER_DATA = &TypeDescriptor_NavCellBorderData;

static const TypeDescriptorField NavZoneDefinition_Fields[19] = {  // line 115
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(NavZoneDefinition, serNavCells), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_NAV_CELL, offsetof(NavZoneDefinition, arNavCells), 0x21,
                                     (int32)offsetof(NavZoneDefinition, serNavCells) - (int32)offsetof(NavZoneDefinition, arNavCells)),
    VariableArrayCountTypeDescriptorField("", offsetof(NavZoneDefinition, uNavCellCount), 0x10000, DT_NAV_CELL,
                                          (int32)offsetof(NavZoneDefinition, serNavCells) - (int32)offsetof(NavZoneDefinition, uNavCellCount)),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(NavZoneDefinition, serNavCellNeighbors), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_NAV_CELL_LOOKUP, offsetof(NavZoneDefinition, arNavCellNeighbors), 0x21,
                                     (int32)offsetof(NavZoneDefinition, serNavCellNeighbors) - (int32)offsetof(NavZoneDefinition, arNavCellNeighbors)),
    VariableArrayCountTypeDescriptorField("", offsetof(NavZoneDefinition, uNavCellNeighborCount), 0x10000, DT_NAV_CELL_LOOKUP,
                                          (int32)offsetof(NavZoneDefinition, serNavCellNeighbors) - (int32)offsetof(NavZoneDefinition, uNavCellNeighborCount)),
    TypeDescriptorField("", DT_FLOAT, offsetof(NavZoneDefinition, flGridSizeFeet), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(NavZoneDefinition, flOOGridSizeFeet), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(NavZoneDefinition, nGridSize), NULL, 1),
    TypeDescriptorField("", DT_IVECTOR2D, offsetof(NavZoneDefinition, tGridSize), NULL, 1),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(NavZoneDefinition, serGridSquares), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_NAV_GRID_SQUARE, offsetof(NavZoneDefinition, arGridSquares), 0x21,
                                     (int32)offsetof(NavZoneDefinition, serGridSquares) - (int32)offsetof(NavZoneDefinition, arGridSquares)),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(NavZoneDefinition, serCellLookups), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_NAV_CELL_LOOKUP, offsetof(NavZoneDefinition, arCellLookups), 0x21,
                                     (int32)offsetof(NavZoneDefinition, serCellLookups) - (int32)offsetof(NavZoneDefinition, arCellLookups)),
    VariableArrayCountTypeDescriptorField("", offsetof(NavZoneDefinition, uCellLookupCount), 0x10000, DT_NAV_CELL_LOOKUP,
                                          (int32)offsetof(NavZoneDefinition, serCellLookups) - (int32)offsetof(NavZoneDefinition, uCellLookupCount)),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(NavZoneDefinition, serBorderData), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_NAV_CELL_BORDER_DATA, offsetof(NavZoneDefinition, arBorderData), 0x21,
                                     (int32)offsetof(NavZoneDefinition, serBorderData) - (int32)offsetof(NavZoneDefinition, arBorderData)),
    VariableArrayCountTypeDescriptorField("", offsetof(NavZoneDefinition, uBorderDataCount), 0x10000, DT_NAV_CELL_BORDER_DATA,
                                          (int32)offsetof(NavZoneDefinition, serBorderData) - (int32)offsetof(NavZoneDefinition, uBorderDataCount)),
    TypeDescriptorField(NULL, DT_NULL, sizeof(NavZoneDefinition), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_NavZoneDefinition("NavZoneDefinition", 0, NavZoneDefinition_Fields, 19, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 129
const XTypeDescriptor * DT_NAV_ZONE_DEFINITION = &TypeDescriptor_NavZoneDefinition;

static const TypeDescriptorField SceneDefinition_Fields[19] = {  // line 134
    TypeDescriptorField("", DT_INT, offsetof(SceneDefinition, dwFlags), NULL, 0x80001),
    FilterTypeDescriptorField("", DT_SNO, offsetof(SceneDefinition, snoAppearance), "", 9, 0x100, -1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(SceneDefinition, snoPhysMesh), "", 61, 0, -1),
    TypeDescriptorField("", DT_AABB, offsetof(SceneDefinition, aabbBounds), NULL, 0),
    TypeDescriptorField("", DT_AABB, offsetof(SceneDefinition, aabbMarkerSetBounds), NULL, 0),
    TypeDescriptorField("", DT_NAVMESH, offsetof(SceneDefinition, tNavMeshData), NULL, 1),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(SceneDefinition, serExclusions), NULL, 0),
    FilteredVariableArrayTypeDescriptorField("", DT_SNO, offsetof(SceneDefinition, arExclusions), 0x123,
                                             (int32)offsetof(SceneDefinition, serExclusions) - (int32)offsetof(SceneDefinition, arExclusions), SCENE_SNO),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(SceneDefinition, serInclusions), NULL, 0),
    FilteredVariableArrayTypeDescriptorField("", DT_SNO, offsetof(SceneDefinition, arInclusions), 0x123,
                                             (int32)offsetof(SceneDefinition, serInclusions) - (int32)offsetof(SceneDefinition, arInclusions), SCENE_SNO),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(SceneDefinition, serMarkerSets), NULL, 0),
    FilteredVariableArrayTypeDescriptorField("", DT_SNO, offsetof(SceneDefinition, arMarkerSets), 0x123,
                                             (int32)offsetof(SceneDefinition, serMarkerSets) - (int32)offsetof(SceneDefinition, arMarkerSets), 24),
    TypeDescriptorField("", DT_LOOKLINK, offsetof(SceneDefinition, tLookLink), NULL, 1),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(SceneDefinition, serMsgTriggeredEvents), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_MSGTRIGGEREDEVENT, offsetof(SceneDefinition, ptMsgTriggeredEvents), 0x21,
                                     (int32)offsetof(SceneDefinition, serMsgTriggeredEvents) - (int32)offsetof(SceneDefinition, ptMsgTriggeredEvents)),
    VariableArrayCountTypeDescriptorField("", offsetof(SceneDefinition, nMsgTriggeredEventCount), 0x10000, DT_MSGTRIGGEREDEVENT,
                                          (int32)offsetof(SceneDefinition, serMsgTriggeredEvents) - (int32)offsetof(SceneDefinition, nMsgTriggeredEventCount)),
    TypeDescriptorField("", DT_NAV_ZONE_DEFINITION, offsetof(SceneDefinition, tNavZoneDef), NULL, 0),
    TypeDescriptorField("", DT_FLOAT, offsetof(SceneDefinition, flXPDensityMultiplier), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(SceneDefinition), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_SceneDefinition("Scene", SCENE_XML_VERSION, SceneDefinition_Fields, 19, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 159
const XTypeDescriptor * DT_SCENE = &TypeDescriptor_SceneDefinition;

SceneGroupDef * gptGroupDefScene;  // line 165

// line 171
SceneGroupDef::SceneGroupDef()
    : GroupDefinition("Scene", SCENE_SNO,
                      SNOVersions(SCENE_PC_SNO_VERSION, 181, SCENE_CONSOLE_SNO_VERSION, SCENE_PS4_SNO_VERSION, SCENE_AXE_SNO_VERSION),
                      SNODATATYPE_TITLE, sizeof(SceneDefinition), DT_SCENE, 1, 0x840e) {
}

// line 192
SceneGroupDef::~SceneGroupDef() {
}

// line 198
uint32 SceneGroupDef::GetMaxCount() {
    return 1024;
}

// line 218
void SceneGroupDef::GetDefaultData(void * ptDefaultDef) const {
    SigmaMemoryClear(ptDefaultDef, sizeof(SceneDefinition));

    SceneDefinition & defDefault = *(SceneDefinition *)ptDefaultDef;
    defDefault.snoPhysMesh = SNO_NONE;
    defDefault.snoAppearance = SNO_NONE;
    defDefault.dwFlags = 0;
    defDefault.flXPDensityMultiplier = 1.0f;

    defDefault.arExclusions.Initialize();

    defDefault.arInclusions.Initialize();

    defDefault.arMarkerSets.Initialize();
}

// line 241
void SceneGroupDef::Postprocess(void * lpDefinition) {
    GroupDefinition::Postprocess(lpDefinition);
}

// line 250
void SceneGroupDef::Delete(void * lpDefinition) const {
    GroupDefinition::Delete(lpDefinition);
}

// line 667
BOOL SceneGroupDef::Write(void * lpDefinition, FileReference & tRef) const {
    SceneDefinition * pdef = (SceneDefinition *)lpDefinition;
    return GroupDefinition::Write(pdef, tRef);
}

// line 937
CRefString SceneDefinitionGetMinimapName(const SNO snoScene) {
    return CRefString(gszMinimapTexturePrefix) + SNOToString(SCENE_SNO, snoScene, FALSE);
}

// line 946
SNO SceneDefinition::GetAppearance() const {
    return snoAppearance;
}
