// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Definitions/SceneDefinition.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../CommonTypes.h"
#include "AttachmentDefinition.h"
#include "Definitions.h"
#include "../Nav/NavMeshInternal.h"
#include "../SNO/SNOFiles.h"
#include "../../Sigma/DataStructures/XPtr64.h"
#include "../../Sigma/Files/XSerializeData.h"
#include "../../Sigma/Math/Rect.h"
#include "../../Sigma/Math/Vector.h"
#include "../../Sigma/SigmaTypes.h"
#include <stddef.h>
#include <stdint.h>

struct FileReference;
class XMemoryAllocatorInterface;

typedef int32 CrafterType;

enum {
    SCENE_DISABLE_LOS_BIT = 0,
    SCENE_DISABLE_DISTRIBUTION_SPAWNING_BIT = 1,
    SCENE_IS_DRLG_BORDER_TILE = 2,
    SCENE_IS_REVEALED_ON_MINIMAP = 3,
    SCENE_DISABLE_MINIMAP_GENERATION_BIT = 4,
    SCENE_IGNORE_NAVMESH = 5,
    SCENE_CONSOLE_ZOOMED_OUT_CAMERA_BIT = 6,
};

enum {
    CSCENE_MSGKEY_CREATE = 9000,
    CSCENE_MSGKEY_DELETE = 9001,
};

enum NavMeshType {
    NAVMESH_TYPE_UNDEFINED = 0,
    NAVMESH_TYPE_ARRAY = 1,
    NAVMESH_TYPE_CONNECTED = 2,
};

class NavMesh {  // size 0x10
public:
    int32 nSizeX;  // 0x00
    int32 nSizeY;  // 0x04
    NavMeshType eNavMeshType;  // 0x08
    int32 nNumSquares;  // 0x0c
};
static_assert(sizeof(NavMesh) == 0x10, "NavMesh");

class NavMeshDef : public NavMesh {  // size 0x60
public:
    int32 nEdgeTopSize;  // 0x10
    int32 nEdgeBottomSize;  // 0x14
    int32 nEdgeLeftSize;  // 0x18
    int32 nEdgeRightSize;  // 0x1c
    float flMinZ;  // 0x20
    float flMaxZ;  // 0x24
    NavMeshSquare tEdge;  // 0x28
    int32 arNumNodesForDistByLevelArea[4];  // 0x30
    SerializeData serCompNavMeshArraySquares;  // 0x40
    XPtr64<CompNavMeshSquare> ptCompNavMeshArraySquares;  // 0x48
    SerializeData serCompNavMeshArraySquaresSpecialFlags;  // 0x50
    XPtr64<unsigned char> ptCompNavMeshArraySquaresSpecialFlags;  // 0x58
};
static_assert(sizeof(NavMeshDef) == 0x60, "NavMeshDef");

class NavMeshInst : public NavMesh {  // size 0x14
public:
    SSceneID idSScene;  // 0x10
};
static_assert(sizeof(NavMeshInst) == 0x14, "NavMeshInst");

struct NavCell {  // size 0x20
    Vector3D tMin;  // 0x00
    Vector3D tMax;  // 0x0c
    uint16 uFlags;  // 0x18
    uint16 uNeighborCount;  // 0x1a
    uint32 uNeighborOffset;  // 0x1c
};
static_assert(sizeof(NavCell) == 0x20, "NavCell");

struct NavGridSquare {  // size 0x6
    uint16 uCellFlags;  // 0x00
    uint16 uCellCount;  // 0x02
    uint16 uLookupOffset;  // 0x04
};
static_assert(sizeof(NavGridSquare) == 0x6, "NavGridSquare");

struct NavCellLookup {  // size 0x4
    uint16 uCellFlags;  // 0x00
    uint16 uCellIndex;  // 0x02
};
static_assert(sizeof(NavCellLookup) == 0x4, "NavCellLookup");

enum {
    NCBD_MIN_X = 0,
    NCBD_MAX_X = 1,
    NCBD_MIN_Y = 2,
    NCBD_MAX_Y = 3,
};

struct NavCellBorderData {  // size 0x4
    uint16 uCellIndex;  // 0x00
    uint16 uBorderFlags;  // 0x02
};
static_assert(sizeof(NavCellBorderData) == 0x4, "NavCellBorderData");

struct NavZoneDefinition {  // size 0x88
    uint32 uNavCellCount;  // 0x00
    XPtr64<NavCell> arNavCells;  // 0x08
    SerializeData serNavCells;  // 0x10
    uint32 uNavCellNeighborCount;  // 0x18
    XPtr64<NavCellLookup> arNavCellNeighbors;  // 0x20
    SerializeData serNavCellNeighbors;  // 0x28
    float flGridSizeFeet;  // 0x30
    float flOOGridSizeFeet;  // 0x34
    int nGridSize;  // 0x38
    IVector2D tGridSize;  // 0x3c
    XPtr64<NavGridSquare> arGridSquares;  // 0x48
    SerializeData serGridSquares;  // 0x50
    uint32 uCellLookupCount;  // 0x58
    XPtr64<NavCellLookup> arCellLookups;  // 0x60
    SerializeData serCellLookups;  // 0x68
    uint32 uBorderDataCount;  // 0x70
    XPtr64<NavCellBorderData> arBorderData;  // 0x78
    SerializeData serBorderData;  // 0x80
};
static_assert(sizeof(NavZoneDefinition) == 0x88, "NavZoneDefinition");

class SceneDefinition {  // size 0x218
public:
    RequiredDefinitionHeader tHeader;  // 0x00
    uint32 dwFlags;  // 0x0c
    AABB aabbBounds;  // 0x10
    AABB aabbMarkerSetBounds;  // 0x28
    NavMeshDef tNavMeshData;  // 0x40
    SerializeData serExclusions;  // 0xa0
    SNOArray arExclusions;  // 0xa8
    SerializeData serInclusions;  // 0xe0
    SNOArray arInclusions;  // 0xe8
    SerializeData serMarkerSets;  // 0x120
    SNOArray arMarkerSets;  // 0x128
    LookLink tLookLink;  // 0x160
    SerializeData serMsgTriggeredEvents;  // 0x168
    int32 nMsgTriggeredEventCount;  // 0x170
    XPtr64<MsgTriggeredEvent> ptMsgTriggeredEvents;  // 0x178
    NavZoneDefinition tNavZoneDef;  // 0x180
    float flXPDensityMultiplier;  // 0x208
    SNO GetAppearance() const;
    SNO snoAppearance;  // 0x20c
    SNO snoPhysMesh;  // 0x210
};
static_assert(sizeof(SceneDefinition) == 0x218, "SceneDefinition");

class SceneGroupDef : public GroupDefinition {  // size 0xa0
public:
    SceneGroupDef();
    virtual ~SceneGroupDef();  // vtable slot 0
    GROUPDEFINITION_MEMORY_FUNCTIONS()  // line 266; the macro name is invented, see Definitions.h
    virtual uint32 GetMaxCount();  // vtable slot 11
    virtual BOOL CanReferenceOtherSNOs() const { return TRUE; }  // vtable slot 13, line 269
    virtual SNOPackType GetSNOPackDestination(const void *) const {  // vtable slot 7, line 271
        return SNOPACKTYPE_COREDATA;
    }
    virtual void Postprocess(void * lpDefinition);  // vtable slot 17
    virtual void Delete(void * lpDefinition) const;  // vtable slot 18
    virtual void GetDefaultData(void * ptDefaultDef) const;  // vtable slot 21
    virtual BOOL Write(void * lpDefinition, FileReference & tRef) const;  // vtable slot 19
};
static_assert(sizeof(SceneGroupDef) == 0xa0, "SceneGroupDef");

extern SceneGroupDef * gptGroupDefScene;  // SceneDefinition.cpp:165

typedef ScopedDefinitionLockR<SceneDefinition> ScopedSceneDefinitionLockR;

template <> inline const SceneDefinition * DefinitionLockR<SceneDefinition>(SNO sno) {  // line 338
    return (const SceneDefinition *)gptGroupDefScene->LockR(sno, FALSE);
}
template <> inline void DefinitionUnlockR<SceneDefinition>(const SceneDefinition * pdef) {  // line 339
    void * lpDef = (void *)pdef;
    gptGroupDefScene->UnlockR(lpDef);
}


// Functions defined in this file (bodies are not in the DWARF):
//   line   266  void * SceneGroupDef::MemAlloc(size_t nSize, uint32 dwAlign) const  [0x7100640d40 (+1 more)]
//   line   266  void SceneGroupDef::MemFree(void *& ptr) const  [0x7100640d90 (+1 more)]
//   line   269  BOOL SceneGroupDef::CanReferenceOtherSNOs() const  [0x7100640e20]
//   line   271  SNOPackType SceneGroupDef::GetSNOPackDestination(const void *) const  [0x7100640e10]
//   line   338  const SceneDefinition * DefinitionLockR<SceneDefinition>(SNO sno)  [inlined into 58 functions]
//   line   339  void DefinitionUnlockR<SceneDefinition>(const SceneDefinition * pdef)  [inlined into 58 functions]
