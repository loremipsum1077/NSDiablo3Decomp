// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/Definitions/AppearanceDefinition.h
// Subset: the members X/Client/StructureOctree.cpp reads. Members whose types are not adopted yet
// are padding (`_unadoptedN`, invented names); the offsets and sizes are the DWARF's.

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../BlizzardCore/Include/Blizzard/Util/Crypto.h"
#include "../../../Contrib/Contrib/blz/include/blz/vector.h"
#include "../StructureOctree.h"
#include "../Gfx/GfxIB.h"
#include "../Gfx/GfxMaterial.h"
#include "../../Common/Definitions/AttachmentDefinition.h"
#include "../../Common/Definitions/Definitions.h"
#include "../../Common/SNO/SNOFiles.h"
#include "../../Sigma/DataStructures/Octree.h"
#include "../../Sigma/DataStructures/XPtr64.h"
#include "../../Sigma/Files/XSerializeData.h"
#include "../../Sigma/Math/Rect.h"
#include "../../Sigma/Math/Vector.h"
#include "../../Sigma/SigmaTypes.h"
// Inference: the original static initializer at 0x1a82e0 stores cgplaceNull before the GfxVB.h
// cgc*Normal statics, so WorldPlace.h reaches includers first; the original include chain that
// brings it in is not known.
#include "../../Common/WorldPlace.h"
#include "../Gfx/GfxVB.h"
#include <stddef.h>
#include <stdint.h>

class CTagMap;
struct AppearanceMaterial;

enum {
    SUBOBJECTAPPEARANCE_RENDER_BIT = 0,
    SUBOBJECTAPPEARANCE_INTERIOR_BIT_DEPRECATED = 1,
    SUBOBJECTAPPEARANCE_MESH_LIT_BIT = 2,
    SUBOBJECTAPPEARANCE_KEEP_VERTEX_COLOR_BIT_DEPRECATED = 3,
    SUBOBJECTAPPEARANCE_USE_SHADER_FACTOR_BIT_DEPRECATED = 4,
    SUBOBJECTAPPEARANCE_IMPORT_OVERWRITES_TEXTURES_DEPRECATED = 5,
    SUBOBJECTAPPEARANCE_LIGHTMAPPED_BIT_DEPRECATED = 6,
    SUBOBJECTAPPEARANCE_NO_PROJECTED_SHADOWS_BIT_DEPRECATED = 7,
    SUBOBJECTAPPEARANCE_NO_HIGHLIGHT_BIT = 8,
    SUBOBJECTAPPEARANCE_NOT_FOGGED_BIT_DEPRECATED = 9,
    SUBOBJECTAPPEARANCE_NO_CLOUD_SHADOWS_BIT_DEPRECATED = 10,
    SUBOBJECTAPPEARANCE_LOCKED_FOR_LIGHTING_BIT_DEPRECATED = 11,
    SUBOBJECTAPPEARANCE_PMA_SHADOW_BIT = 12,
};

enum {
    SUBOBJECT_UNSTRIPIFIED_BIT = 0,
    SUBOBJECT_CASTS_AO = 1,
    SUBOBJECT_RECEIVES_AO = 2,
    SUBOBJECT_NOT_WALKABLE_BIT = 3,
    SUBOBJECT_NOT_COLLIDABLE_BIT = 4,
    SUBOBJECT_CASTS_SHADOW = 5,
    SUBOBJECT_RECEIVES_SHADOW = 6,
    SUBOBJECT_FADE_CHILD_BIT = 7,
    SUBOBJECT_CLOTH_BIT = 8,
    SUBOBJECT_PROJECTILE_ONLY_BIT = 9,
    SUBOBJECT_RECEIVES_FX_BIT = 10,
    SUBOBJECT_INTERIOR_SCENE_BIT = 11,
    SUBOBJECT_IN_OCTREE_BIT = 12,
    SUBOBJECT_EDGE_GEOMETRY_BIT = 13,
    SUBOBJECT_EDGE_SKIN_BIT = 14,
    SUBOBJECT_PACK_GEOMETRY_BIT = 15,
    SUBOBJECT_PACK_SKIN_BIT = 16,
    SUBOBJECT_COARSE_GEO_FADE_TEST_BIT = 17,
};

struct Face {  // size 0x6
    uint16 wIndex0;  // 0x00
    uint16 wIndex1;  // 0x02
    uint16 wIndex2;  // 0x04
};
static_assert(sizeof(Face) == 0x6, "Face");

struct VertexOwnerData;  // X/Client/SubObject.h

struct SubObjectPostData {  // size 0x70
    AppearanceMaterial * ptMaterial;  // 0x00
    int32 nFaceCount;  // 0x08
    Face * ptFaceList;  // 0x10
    VertexOwnerData * ptVertexOwnerList;  // 0x18
    float flAlpha;  // 0x20
    int32 nTotalTriArea;  // 0x24
    blz::vector<int, blz::allocator<int> > tTriAreas;  // 0x28
    PackedIBEntry tIBEntry;  // 0x40
    PackedVBEntry tVBEntry;  // 0x50
    PackedVBEntry tPackVBEntry;  // 0x60
};
static_assert(sizeof(SubObjectPostData) == 0x70, "SubObjectPostData");

struct FatVertex;
struct VertInfluences;

struct CompInfluence {  // size 0x2
    uint8 dwBoneIndex;  // 0x00
    uint8 dwWeight;  // 0x01
};
static_assert(sizeof(CompInfluence) == 0x2, "CompInfluence");

struct CompVertInfluences {  // size 0x6
    CompInfluence tBoneInfluence[3];  // 0x00
};
static_assert(sizeof(CompVertInfluences) == 0x6, "CompVertInfluences");

struct SkinningInfo {  // size 0x68
    int32 nNumBones;  // 0x00
    SerializeData serBoneIDs;  // 0x04
    XPtr64<int> pBoneIDs;  // 0x10
    int32 nPackVertCount;  // 0x18
    XPtr64<unsigned char> pPackVertList;  // 0x20
    SerializeData serPackVertList;  // 0x28
    int32 nPackIndexCount;  // 0x30
    XPtr64<unsigned char> pPackIndexList;  // 0x38
    SerializeData serPackIndexList;  // 0x40
    PackedIBEntry64 tPackIBEntry;  // 0x48
    PackedVBEntry64 tPackVBEntry;  // 0x58
};
static_assert(sizeof(SkinningInfo) == 0x68, "SkinningInfo");
struct ClothVertexSerializable;
struct ClothFaceSerializable;
struct ClothConstraintSerializable;
struct ClothStaple;

struct ClothStructure {  // size 0x90
    int32 nVertexCount;  // 0x00
    SerializeData serVertices;  // 0x04
    XPtr64<ClothVertexSerializable> ptVertices;  // 0x10
    int32 nFaceCount;  // 0x18
    SerializeData serFaces;  // 0x1c
    XPtr64<ClothFaceSerializable> ptFaces;  // 0x28
    int32 nStapleCount;  // 0x30
    SerializeData serStaples;  // 0x34
    XPtr64<ClothStaple> ptStaples;  // 0x40
    int32 nDistanceConstraintCount;  // 0x48
    SerializeData serDistanceConstraints;  // 0x4c
    XPtr64<ClothConstraintSerializable> ptDistanceConstraints;  // 0x58
    int32 nBendingConstraintCount;  // 0x60
    SerializeData serBendingConstraints;  // 0x64
    XPtr64<ClothConstraintSerializable> ptBendingConstraints;  // 0x70
    int32 nIslandCount;  // 0x78
    float flArea;  // 0x7c
    SerializeData serClothVertMapping;  // 0x80
    XPtr64<unsigned short> ptClothVertexMapping;  // 0x88
};
static_assert(sizeof(ClothStructure) == 0x90, "ClothStructure");

struct CollisionShape;

struct SubObjectNameInfoDetail {  // size 0x4
    uint32 fIsHair : 1;  // 0x00
    uint32 fIsSlot : 1;  // 0x00
    uint32 fIsComponent : 1;  // 0x00
    uint32 fHasMissedHydraCombine : 1;  // 0x00
    uint32 dwUnused : 4;  // 0x00
    uint32 dwHairStyle : 8;  // 0x00
    uint32 dwSlot : 8;  // 0x00
    uint32 dwComponent : 8;  // 0x00
};
static_assert(sizeof(SubObjectNameInfoDetail) == 0x4, "SubObjectNameInfoDetail");

struct SubObjectNameInfo {  // size 0x4
    union {  // size 0x4
        uint32 dwNameInfo;  // 0x00
        SubObjectNameInfoDetail tDetails;  // 0x00
    };  // 0x00
};
static_assert(sizeof(SubObjectNameInfo) == 0x4, "SubObjectNameInfo");

struct SubObject {  // size 0xf0
    uint32 dwFlags;  // 0x00
    int32 nVertCount;  // 0x04
    SerializeData serVertList;  // 0x08
    XPtr64<FatVertex> ptVertList;  // 0x10
    SerializeData serInfluenceList;  // 0x18
    XPtr64<VertInfluences> ptInfluenceList;  // 0x20
    int32 nIndexCount;  // 0x28
    SerializeData serIndexList;  // 0x2c
    XPtr64<unsigned short> pwIndexList;  // 0x38
    SerializeData serClothStructure;  // 0x40
    XPtr64<ClothStructure> ptClothStructure;  // 0x48
    SNO snoSurface;  // 0x50
    int32 nFadeGroup;  // 0x54
    float flFadeAlpha;  // 0x58
    SubObjectNameInfo tNameInfo;  // 0x5c
    uint32 dwPad3;  // 0x60
    uint32 dwMaterialHash;  // 0x64
    uint32 dwSubObjectHash;  // 0x68
    AABB aabbBounds;  // 0x6c
    int32 nShapeCount;  // 0x84
    SerializeData serShapes;  // 0x88
    XPtr64<CollisionShape> ptShapes;  // 0x90
    XPtr64<SubObjectPostData> ptPostprocessed;  // 0x98
    int32 nPackVertCount;  // 0xa0
    uint32 dwPad;  // 0xa4
    XPtr64<unsigned char> pPackVertList;  // 0xa8
    SerializeData serPackVertList;  // 0xb0
    SerializeData serSkinningInfo;  // 0xb8
    XPtr64<SkinningInfo> ptSkinningInfo;  // 0xc0
    SerializeData serCompInfluenceList;  // 0xc8
    XPtr64<CompVertInfluences> ptCompInfluenceList;  // 0xd0
    Vector3D wpFixedPointPosOffset;  // 0xd8
    Vector3D wpFixedPointPosScale;  // 0xe4
};
static_assert(sizeof(SubObject) == 0xf0, "SubObject");

enum {
    GEOSET_HIGHPOLY = 0,
    GEOSET_LOWPOLY = 1,
    NUM_GEOSETS = 2,
};

typedef int32 GeoSetType;

struct GeoSet {  // size 0x18
    int32 nSubObjectCount;  // 0x00
    SerializeData serSubObjects;  // 0x04
    XPtr64<SubObject> ptSubObjects;  // 0x10
};
static_assert(sizeof(GeoSet) == 0x18, "GeoSet");

struct CollisionShape;
struct ConstraintParameters;

struct BoneStructure {  // size 0x100
    uint32 dwHash;  // 0x00
    uint32 dwPad;  // 0x04
    int32 nParentIndex;  // 0x08
    AABB aabb;  // 0x0c
    Sphere wsBounds;  // 0x24
    PRSTransform transform;  // 0x34
    PRSTransform transformInv;  // 0x54
    PRSTransform transformParentRel;  // 0x74
    PRSTransform transformBindPose;  // 0x94
    PRSTransform transformBindPoseInv;  // 0xb4
    int32 nShapeCount;  // 0xd4
    SerializeData serShapes;  // 0xd8
    XPtr64<CollisionShape> ptShapes;  // 0xe0
    SerializeData serConstraint;  // 0xe8
    XPtr64<ConstraintParameters> ptConstraint;  // 0xf0
    SNO snoParticleSystem;  // 0xf8
};
static_assert(sizeof(BoneStructure) == 0x100, "BoneStructure");

struct Influence {  // size 0x8
    int32 nBoneIndex;  // 0x00
    float flWeight;  // 0x04
};
static_assert(sizeof(Influence) == 0x8, "Influence");

struct VertInfluences {  // size 0x18
    Influence tBoneInfluence[3];  // 0x00
};
static_assert(sizeof(VertInfluences) == 0x18, "VertInfluences");

enum {
    STRUCTURE_CANNOT_ANIMATE_BIT = 0,
    STRUCTURE_CAN_RAGDOLL_BIT = 1,
    STRUCTURE_HAS_PARTIAL_RAGDOLL_BIT = 2,
    STRUCTURE_SEPARATE_COLLISION_MESH_BIT = 3,
    STRUCTURE_HAS_BREAKABLE_RAGDOLL_BIT = 4,
    STRUCTURE_HAS_BUOYANT_WATER_BIT = 5,
};

struct LookAtData {  // size 0x24
    BOOL bEnabled;  // 0x00
    uint32 dwBoneHash;  // 0x04
    uint32 dwBoneHashPadding;  // 0x08
    Angle aAngleLimitYaw;  // 0x0c
    Angle aPitchMin;  // 0x10
    Angle aPitchMax;  // 0x14
    Angle aPitchOffset;  // 0x18
    Angle aSpeed;  // 0x1c
    float flRange;  // 0x20
};
static_assert(sizeof(LookAtData) == 0x24, "LookAtData");

struct BonePulseData {  // size 0x14
    uint32 dwBoneHash;  // 0x00
    uint32 dwBoneHashPadding;  // 0x04
    float flAmplitude;  // 0x08
    float flHertz;  // 0x0c
    Angle aPhase;  // 0x10
};
static_assert(sizeof(BonePulseData) == 0x14, "BonePulseData");

struct ClothStaple {  // size 0x1c
    int32 nVertex;  // 0x00
    int32 anBoneIndices[3];  // 0x04
    float aflBoneWeights[3];  // 0x10
};
static_assert(sizeof(ClothStaple) == 0x1c, "ClothStaple");

struct CollisionCapsule {  // size 0x30
    float flRadius;  // 0x00
    float flLength;  // 0x04
    Hardpoint tHardpoint;  // 0x08
};
static_assert(sizeof(CollisionCapsule) == 0x30, "CollisionCapsule");

struct StructureGeoSetPostData {  // size 0x28
    PackedVBList * ptPVBList;  // 0x00
    PackedIBList * ptPIBList;  // 0x08
    PackedVBList * ptPackPVBList;  // 0x10
    PackedVBList * ptSkinPVBList;  // 0x18
    PackedIBList * ptSkinPIBList;  // 0x20
};
static_assert(sizeof(StructureGeoSetPostData) == 0x28, "StructureGeoSetPostData");

struct StructurePostData {  // size 0x60
    StructureOctree tStructureOctree;  // 0x00
    StructureGeoSetPostData tGeoSetPost[2];  // 0x10
};
static_assert(sizeof(StructurePostData) == 0x60, "StructurePostData");

struct Structure {  // size 0x158
    uint32 dwFlags;  // 0x00
    int32 nBoneCount; // 0x04
    SerializeData serBoneStructure; // 0x08
    XPtr64<BoneStructure> ptBoneStructure; // 0x10
    LookAtData tLookAtData;  // 0x18
    int32 nBonePulseCount;  // 0x3c
    XPtr64<BonePulseData> ptBonePulses;  // 0x40
    SerializeData serBonePulses;  // 0x48
    GeoSet tGeoSets[2];  // 0x50
    Sphere wsBounds;  // 0x80
    int32 nCollisionCapsuleCount;  // 0x90
    SerializeData serCollisionCapsules;  // 0x94
    XPtr64<CollisionCapsule> ptCollisionCapsules;  // 0xa0
    int32 nHardpointCount;  // 0xa8
    SerializeData serHardpoints;  // 0xac
    XPtr64<Hardpoint> ptHardpoints;  // 0xb8
    Vector3D wvRigidAnimAxis;  // 0xc0
    Octree tOctreeVisualMesh;  // 0xd0
    AABB aabbBounds;  // 0x118
    int32 nLoopConstraintCount; // 0x130
    SerializeData serLoopConstraints; // 0x134
    XPtr64<ConstraintParameters> ptLoopConstraints; // 0x140
    uint32 uRagdollDegrade;  // 0x148
    XPtr64<StructurePostData> ptPostprocessed;  // 0x150
};
static_assert(sizeof(Structure) == 0x158, "Structure");

struct SubObjectAppearance {  // size 0xf8
    uint32 dwFlags;  // 0x00
    SNO snoCloth;  // 0x04
    SerializeData serTagMap;  // 0x08
    XPtr64<CTagMap> hTagMap;  // 0x10
    UberMaterial tBaseUberMaterial;  // 0x18
    SNO snoMasterMaterial;  // 0x80
    UberMaterial tCompositeUberMaterial;  // 0x88
    XPtr64<CTagMap> hTagMapMasterMaterial;  // 0xf0
};
static_assert(sizeof(SubObjectAppearance) == 0xf8, "SubObjectAppearance");

struct AppearanceMaterial {  // size 0x18
    uint32 dwMaterialHash;  // 0x00
    uint32 dwPad;  // 0x04
    SerializeData serSOAs;  // 0x08
    XPtr64<SubObjectAppearance> ptSOAs;  // 0x10
};
static_assert(sizeof(AppearanceMaterial) == 0x18, "AppearanceMaterial");

struct AppearanceLook {  // size 0x4
    uint32 dwLookHash;  // 0x00
};
static_assert(sizeof(AppearanceLook) == 0x4, "AppearanceLook");

enum {
    APPEARANCE_SAMPLE_COLOR_IS_VALID_BIT = 0,
    APPEARANCE_SAMPLE_SURFACE_IS_VALID_BIT = 1,
    APPEARANCE_SAMPLE_CLUTTER_IS_VALID_BIT = 2,
};

struct WalkableZFractionAndSurface {  // size 0x4
    uint32 dwZFraction : 8;  // 0x00
    SNO snoSurface : 24;  // 0x00
};
static_assert(sizeof(WalkableZFractionAndSurface) == 0x4, "WalkableZFractionAndSurface");

struct ClutterZFractionAndParticle {  // size 0x4
    uint32 dwZFraction : 8;  // 0x00
    SNO snoParticle : 24;  // 0x00
};
static_assert(sizeof(ClutterZFractionAndParticle) == 0x4, "ClutterZFractionAndParticle");

struct ClutterDensityMaskAndColor {  // size 0x4
    uint32 dwClutterDensity : 4;  // 0x00
    uint32 dwMask : 4;  // 0x00
    uint32 dwColor : 24;  // 0x00
};
static_assert(sizeof(ClutterDensityMaskAndColor) == 0x4, "ClutterDensityMaskAndColor");

struct AppearanceSample {  // size 0xc
    union {  // size 0x4
        uint32 dwWalkableZFractionAndSurface;  // 0x00
        WalkableZFractionAndSurface tWalkableZFractionAndSurface;  // 0x00
    };  // 0x00
    union {  // size 0x4
        uint32 dwClutterZFractionAndParticle;  // 0x00
        ClutterZFractionAndParticle tClutterZFractionAndParticle;  // 0x00
    };  // 0x04
    union {  // size 0x4
        uint32 dwClutterDensityMaskAndColor;  // 0x00
        ClutterDensityMaskAndColor tClutterDensityMaskAndColor;  // 0x00
    };  // 0x08
};
static_assert(sizeof(AppearanceSample) == 0xc, "AppearanceSample");

struct AppearanceSamplingSubgrid {  // size 0x38
    int32 nSamplesX;  // 0x00
    int32 nSamplesY;  // 0x04
    Vector2D wpBase;  // 0x08
    SerializeData serSamples;  // 0x10
    XPtr64<AppearanceSample> ptSamples;  // 0x18
    AABB aabb;  // 0x20
};
static_assert(sizeof(AppearanceSamplingSubgrid) == 0x38, "AppearanceSamplingSubgrid");

struct StaticLight;

struct AppearanceDefinition {  // size 0x228
    RequiredDefinitionHeader tHeader;  // 0x00
    Structure tStructure;  // 0x10
    int32 nNumLooks;  // 0x168
    int32 nNumMaterials;  // 0x16c
    SerializeData serAppearanceMaterials;  // 0x170
    XPtr64<AppearanceMaterial> ptAppearanceMaterials;  // 0x178
    SerializeData serAppearanceLooks;  // 0x180
    XPtr64<AppearanceLook> ptAppearanceLooks;  // 0x188
    int32 nStaticLightCount;  // 0x190
    SerializeData serStaticLights;  // 0x194
    XPtr64<StaticLight> ptStaticLights;  // 0x1a0
    OverlayProjectionAxis eOverlayProjectionAxis;  // 0x1a8
    uint32 dwFadeOutTimeCriticalHitBlood;  // 0x1ac
    uint32 dwFadeOutTimeStoned;  // 0x1b0
    uint32 dwFadeOutTimeChilled;  // 0x1b4
    uint32 dwFadeOutTimeFrozen;  // 0x1b8
    uint32 dwFadeOutTimePoisoned;  // 0x1bc
    uint32 dwMachineTime;  // 0x1c0
    float flUVGenQuality;  // 0x1c4
    uint32 dwLightMapWidth;  // 0x1c8
    uint32 dwLightMapHeight;  // 0x1cc
    int64 tUnwrapTimeStamp;  // 0x1d0
    float flMinZ;  // 0x1d8
    float flMaxZ;  // 0x1dc
    int32 nSubgridsX;  // 0x1e0
    int32 nSubgridsY;  // 0x1e4
    AABB tSubgridsAABB;  // 0x1e8
    SerializeData serSubgrids;  // 0x200
    XPtr64<AppearanceSamplingSubgrid> ptSubgrids;  // 0x208
    SerializeData serAppearanceSubObjectsIndices;  // 0x210
    SerializeData serAppearanceSubObjectsVertices;  // 0x218
    uint32 dwFlags;  // 0x220
};
static_assert(sizeof(AppearanceDefinition) == 0x228, "AppearanceDefinition");

class AppearanceGroupDef : public GroupDefinition {  // size 0xb0
public:
    AppearanceGroupDef();
    GROUPDEFINITION_MEMORY_FUNCTIONS()  // vtable slots 2-5; the macro name is invented, see Definitions.h
    virtual uint32 GetMaxCount();  // vtable slot 11
    virtual BOOL CanReferenceOtherSNOs() const { return TRUE; }  // vtable slot 13, line 836
    virtual SNOPackType GetSNOPackDestination(const void *) const { return SNOPACKTYPE_CLIENTDATA; }  // vtable slot 7, line 838
    virtual uint32 GetEstCacheSizeLimit();  // vtable slot 12
    virtual uint32 GetContiguousReadSize(SNOElement * ptSNOElement);  // vtable slot 27
    virtual void Read(const RequiredDefinitionHeader & tHeader, void *& lpDefinition, SNOElement * ptSNOElement, BOOL fContiguousBuffer, uint32 dwContiguousSize, XMemoryAllocatorInterface * ptMemoryInterface) const;  // vtable slot 14
    virtual void Postprocess(void * lpDefinition);  // vtable slot 17
    virtual void Delete(void * lpDefinition) const;  // vtable slot 18
    virtual BOOL Write(void * lpDefinition, FileReference & tRef) const;  // vtable slot 19
    virtual void GetDefaultData(void * ptDefaultDef) const;  // vtable slot 21
    virtual BOOL CacheManagerCanPurgeElement(const DefDataElement & tDefElement, BOOL fIgnoreDoNotPurgeBit) const;  // vtable slot 29
private:
    void sComputeMD5(const AppearanceDefinition *, Blizzard::Util::Digest<16> &) const;  // DWARF type: Blizzard::Util::MD5::Digest
    Blizzard::Util::Digest<16> tMD5Digest;  // 0x9c (in GroupDefinition's tail padding)
};
static_assert(sizeof(AppearanceGroupDef) == 0xb0, "AppearanceGroupDef");

extern AppearanceGroupDef * gptGroupDefAppearance;  // AppearanceDefinition.cpp:548

typedef ScopedDefinitionLockR<AppearanceDefinition> ScopedAppearanceDefinitionLockR;

template <> inline const AppearanceDefinition * DefinitionLockR<AppearanceDefinition>(SNO sno) {  // line 933
    return (const AppearanceDefinition *)gptGroupDefAppearance->LockR(sno, FALSE);
}
template <> inline void DefinitionUnlockR<AppearanceDefinition>(const AppearanceDefinition * pdef) {  // line 934
    void * lpDef = (void *)pdef;
    gptGroupDefAppearance->UnlockR(lpDef);
}

// Declared here from their definitions in AppearanceDefinition.cpp (:4860 and :4880).
void AppearanceEnableBufferContentRetrieval(const AppearanceDefinition * pdefAppearance);
void AppearanceDisableBufferContentRetrieval(const AppearanceDefinition * pdefAppearance);

class ScopedAppearanceBufferContentRetrieval {  // size 0x8
public:
    ScopedAppearanceBufferContentRetrieval(const AppearanceDefinition * pdefAppearance)  // line 1117
        : m_pdefAppearance(pdefAppearance) {
        AppearanceEnableBufferContentRetrieval(m_pdefAppearance);
    }
    ~ScopedAppearanceBufferContentRetrieval() {  // line 1124
        AppearanceDisableBufferContentRetrieval(m_pdefAppearance);
    }
private:
    const AppearanceDefinition * m_pdefAppearance;  // 0x00
};
static_assert(sizeof(ScopedAppearanceBufferContentRetrieval) == 0x8, "ScopedAppearanceBufferContentRetrieval");

#include "../../Common/ComponentStyle.h"
// AppearanceDefinition.cpp:4731.
void AppearanceGetNakedLook(const SNO snoAppearance, ComponentStyle & eCompStyle, int32 & nSubObjectLook);

// Layout checks generated from the DWARF by tools/layout_asserts.py.
static_assert(sizeof(Face) == 0x6, "Face");
static_assert(offsetof(Face, wIndex0) == 0x0, "Face");
static_assert(offsetof(Face, wIndex1) == 0x2, "Face");
static_assert(offsetof(Face, wIndex2) == 0x4, "Face");
static_assert(sizeof(CollisionCapsule) == 0x30, "CollisionCapsule");
static_assert(offsetof(CollisionCapsule, flRadius) == 0x0, "CollisionCapsule");
static_assert(offsetof(CollisionCapsule, flLength) == 0x4, "CollisionCapsule");
static_assert(offsetof(CollisionCapsule, tHardpoint) == 0x8, "CollisionCapsule");
static_assert(sizeof(CompInfluence) == 0x2, "CompInfluence");
static_assert(offsetof(CompInfluence, dwBoneIndex) == 0x0, "CompInfluence");
static_assert(offsetof(CompInfluence, dwWeight) == 0x1, "CompInfluence");
static_assert(sizeof(CompVertInfluences) == 0x6, "CompVertInfluences");
static_assert(offsetof(CompVertInfluences, tBoneInfluence) == 0x0, "CompVertInfluences");
static_assert(sizeof(Influence) == 0x8, "Influence");
static_assert(offsetof(Influence, nBoneIndex) == 0x0, "Influence");
static_assert(offsetof(Influence, flWeight) == 0x4, "Influence");
static_assert(sizeof(VertInfluences) == 0x18, "VertInfluences");
static_assert(offsetof(VertInfluences, tBoneInfluence) == 0x0, "VertInfluences");
static_assert(sizeof(SkinningInfo) == 0x68, "SkinningInfo");
static_assert(offsetof(SkinningInfo, nNumBones) == 0x0, "SkinningInfo");
static_assert(offsetof(SkinningInfo, serBoneIDs) == 0x4, "SkinningInfo");
static_assert(offsetof(SkinningInfo, pBoneIDs) == 0x10, "SkinningInfo");
static_assert(offsetof(SkinningInfo, nPackVertCount) == 0x18, "SkinningInfo");
static_assert(offsetof(SkinningInfo, pPackVertList) == 0x20, "SkinningInfo");
static_assert(offsetof(SkinningInfo, serPackVertList) == 0x28, "SkinningInfo");
static_assert(offsetof(SkinningInfo, nPackIndexCount) == 0x30, "SkinningInfo");
static_assert(offsetof(SkinningInfo, pPackIndexList) == 0x38, "SkinningInfo");
static_assert(offsetof(SkinningInfo, serPackIndexList) == 0x40, "SkinningInfo");
static_assert(offsetof(SkinningInfo, tPackIBEntry) == 0x48, "SkinningInfo");
static_assert(offsetof(SkinningInfo, tPackVBEntry) == 0x58, "SkinningInfo");
static_assert(sizeof(SubObjectPostData) == 0x70, "SubObjectPostData");
static_assert(offsetof(SubObjectPostData, ptMaterial) == 0x0, "SubObjectPostData");
static_assert(offsetof(SubObjectPostData, nFaceCount) == 0x8, "SubObjectPostData");
static_assert(offsetof(SubObjectPostData, ptFaceList) == 0x10, "SubObjectPostData");
static_assert(offsetof(SubObjectPostData, ptVertexOwnerList) == 0x18, "SubObjectPostData");
static_assert(offsetof(SubObjectPostData, flAlpha) == 0x20, "SubObjectPostData");
static_assert(offsetof(SubObjectPostData, nTotalTriArea) == 0x24, "SubObjectPostData");
static_assert(offsetof(SubObjectPostData, tTriAreas) == 0x28, "SubObjectPostData");
static_assert(offsetof(SubObjectPostData, tIBEntry) == 0x40, "SubObjectPostData");
static_assert(offsetof(SubObjectPostData, tVBEntry) == 0x50, "SubObjectPostData");
static_assert(offsetof(SubObjectPostData, tPackVBEntry) == 0x60, "SubObjectPostData");
static_assert(sizeof(ClothStaple) == 0x1c, "ClothStaple");
static_assert(offsetof(ClothStaple, nVertex) == 0x0, "ClothStaple");
static_assert(offsetof(ClothStaple, anBoneIndices) == 0x4, "ClothStaple");
static_assert(offsetof(ClothStaple, aflBoneWeights) == 0x10, "ClothStaple");
static_assert(sizeof(ClothStructure) == 0x90, "ClothStructure");
static_assert(offsetof(ClothStructure, nVertexCount) == 0x0, "ClothStructure");
static_assert(offsetof(ClothStructure, serVertices) == 0x4, "ClothStructure");
static_assert(offsetof(ClothStructure, ptVertices) == 0x10, "ClothStructure");
static_assert(offsetof(ClothStructure, nFaceCount) == 0x18, "ClothStructure");
static_assert(offsetof(ClothStructure, serFaces) == 0x1c, "ClothStructure");
static_assert(offsetof(ClothStructure, ptFaces) == 0x28, "ClothStructure");
static_assert(offsetof(ClothStructure, nStapleCount) == 0x30, "ClothStructure");
static_assert(offsetof(ClothStructure, serStaples) == 0x34, "ClothStructure");
static_assert(offsetof(ClothStructure, ptStaples) == 0x40, "ClothStructure");
static_assert(offsetof(ClothStructure, nDistanceConstraintCount) == 0x48, "ClothStructure");
static_assert(offsetof(ClothStructure, serDistanceConstraints) == 0x4c, "ClothStructure");
static_assert(offsetof(ClothStructure, ptDistanceConstraints) == 0x58, "ClothStructure");
static_assert(offsetof(ClothStructure, nBendingConstraintCount) == 0x60, "ClothStructure");
static_assert(offsetof(ClothStructure, serBendingConstraints) == 0x64, "ClothStructure");
static_assert(offsetof(ClothStructure, ptBendingConstraints) == 0x70, "ClothStructure");
static_assert(offsetof(ClothStructure, nIslandCount) == 0x78, "ClothStructure");
static_assert(offsetof(ClothStructure, flArea) == 0x7c, "ClothStructure");
static_assert(offsetof(ClothStructure, serClothVertMapping) == 0x80, "ClothStructure");
static_assert(offsetof(ClothStructure, ptClothVertexMapping) == 0x88, "ClothStructure");
static_assert(sizeof(SubObjectNameInfoDetail) == 0x4, "SubObjectNameInfoDetail");
static_assert(sizeof(SubObjectNameInfo) == 0x4, "SubObjectNameInfo");
static_assert(sizeof(SubObject) == 0xf0, "SubObject");
static_assert(offsetof(SubObject, dwFlags) == 0x0, "SubObject");
static_assert(offsetof(SubObject, nVertCount) == 0x4, "SubObject");
static_assert(offsetof(SubObject, serVertList) == 0x8, "SubObject");
static_assert(offsetof(SubObject, ptVertList) == 0x10, "SubObject");
static_assert(offsetof(SubObject, serInfluenceList) == 0x18, "SubObject");
static_assert(offsetof(SubObject, ptInfluenceList) == 0x20, "SubObject");
static_assert(offsetof(SubObject, nIndexCount) == 0x28, "SubObject");
static_assert(offsetof(SubObject, serIndexList) == 0x2c, "SubObject");
static_assert(offsetof(SubObject, pwIndexList) == 0x38, "SubObject");
static_assert(offsetof(SubObject, serClothStructure) == 0x40, "SubObject");
static_assert(offsetof(SubObject, ptClothStructure) == 0x48, "SubObject");
static_assert(offsetof(SubObject, snoSurface) == 0x50, "SubObject");
static_assert(offsetof(SubObject, nFadeGroup) == 0x54, "SubObject");
static_assert(offsetof(SubObject, flFadeAlpha) == 0x58, "SubObject");
static_assert(offsetof(SubObject, tNameInfo) == 0x5c, "SubObject");
static_assert(offsetof(SubObject, dwPad3) == 0x60, "SubObject");
static_assert(offsetof(SubObject, dwMaterialHash) == 0x64, "SubObject");
static_assert(offsetof(SubObject, dwSubObjectHash) == 0x68, "SubObject");
static_assert(offsetof(SubObject, aabbBounds) == 0x6c, "SubObject");
static_assert(offsetof(SubObject, nShapeCount) == 0x84, "SubObject");
static_assert(offsetof(SubObject, serShapes) == 0x88, "SubObject");
static_assert(offsetof(SubObject, ptShapes) == 0x90, "SubObject");
static_assert(offsetof(SubObject, ptPostprocessed) == 0x98, "SubObject");
static_assert(offsetof(SubObject, nPackVertCount) == 0xa0, "SubObject");
static_assert(offsetof(SubObject, dwPad) == 0xa4, "SubObject");
static_assert(offsetof(SubObject, pPackVertList) == 0xa8, "SubObject");
static_assert(offsetof(SubObject, serPackVertList) == 0xb0, "SubObject");
static_assert(offsetof(SubObject, serSkinningInfo) == 0xb8, "SubObject");
static_assert(offsetof(SubObject, ptSkinningInfo) == 0xc0, "SubObject");
static_assert(offsetof(SubObject, serCompInfluenceList) == 0xc8, "SubObject");
static_assert(offsetof(SubObject, ptCompInfluenceList) == 0xd0, "SubObject");
static_assert(offsetof(SubObject, wpFixedPointPosOffset) == 0xd8, "SubObject");
static_assert(offsetof(SubObject, wpFixedPointPosScale) == 0xe4, "SubObject");
static_assert(sizeof(BoneStructure) == 0x100, "BoneStructure");
static_assert(offsetof(BoneStructure, dwHash) == 0x0, "BoneStructure");
static_assert(offsetof(BoneStructure, dwPad) == 0x4, "BoneStructure");
static_assert(offsetof(BoneStructure, nParentIndex) == 0x8, "BoneStructure");
static_assert(offsetof(BoneStructure, aabb) == 0xc, "BoneStructure");
static_assert(offsetof(BoneStructure, wsBounds) == 0x24, "BoneStructure");
static_assert(offsetof(BoneStructure, transform) == 0x34, "BoneStructure");
static_assert(offsetof(BoneStructure, transformInv) == 0x54, "BoneStructure");
static_assert(offsetof(BoneStructure, transformParentRel) == 0x74, "BoneStructure");
static_assert(offsetof(BoneStructure, transformBindPose) == 0x94, "BoneStructure");
static_assert(offsetof(BoneStructure, transformBindPoseInv) == 0xb4, "BoneStructure");
static_assert(offsetof(BoneStructure, nShapeCount) == 0xd4, "BoneStructure");
static_assert(offsetof(BoneStructure, serShapes) == 0xd8, "BoneStructure");
static_assert(offsetof(BoneStructure, ptShapes) == 0xe0, "BoneStructure");
static_assert(offsetof(BoneStructure, serConstraint) == 0xe8, "BoneStructure");
static_assert(offsetof(BoneStructure, ptConstraint) == 0xf0, "BoneStructure");
static_assert(offsetof(BoneStructure, snoParticleSystem) == 0xf8, "BoneStructure");
static_assert(sizeof(LookAtData) == 0x24, "LookAtData");
static_assert(offsetof(LookAtData, bEnabled) == 0x0, "LookAtData");
static_assert(offsetof(LookAtData, dwBoneHash) == 0x4, "LookAtData");
static_assert(offsetof(LookAtData, dwBoneHashPadding) == 0x8, "LookAtData");
static_assert(offsetof(LookAtData, aAngleLimitYaw) == 0xc, "LookAtData");
static_assert(offsetof(LookAtData, aPitchMin) == 0x10, "LookAtData");
static_assert(offsetof(LookAtData, aPitchMax) == 0x14, "LookAtData");
static_assert(offsetof(LookAtData, aPitchOffset) == 0x18, "LookAtData");
static_assert(offsetof(LookAtData, aSpeed) == 0x1c, "LookAtData");
static_assert(offsetof(LookAtData, flRange) == 0x20, "LookAtData");
static_assert(sizeof(BonePulseData) == 0x14, "BonePulseData");
static_assert(offsetof(BonePulseData, dwBoneHash) == 0x0, "BonePulseData");
static_assert(offsetof(BonePulseData, dwBoneHashPadding) == 0x4, "BonePulseData");
static_assert(offsetof(BonePulseData, flAmplitude) == 0x8, "BonePulseData");
static_assert(offsetof(BonePulseData, flHertz) == 0xc, "BonePulseData");
static_assert(offsetof(BonePulseData, aPhase) == 0x10, "BonePulseData");
static_assert(sizeof(GeoSet) == 0x18, "GeoSet");
static_assert(offsetof(GeoSet, nSubObjectCount) == 0x0, "GeoSet");
static_assert(offsetof(GeoSet, serSubObjects) == 0x4, "GeoSet");
static_assert(offsetof(GeoSet, ptSubObjects) == 0x10, "GeoSet");
static_assert(sizeof(Structure) == 0x158, "Structure");
static_assert(offsetof(Structure, dwFlags) == 0x0, "Structure");
static_assert(offsetof(Structure, nBoneCount) == 0x4, "Structure");
static_assert(offsetof(Structure, serBoneStructure) == 0x8, "Structure");
static_assert(offsetof(Structure, ptBoneStructure) == 0x10, "Structure");
static_assert(offsetof(Structure, tLookAtData) == 0x18, "Structure");
static_assert(offsetof(Structure, nBonePulseCount) == 0x3c, "Structure");
static_assert(offsetof(Structure, ptBonePulses) == 0x40, "Structure");
static_assert(offsetof(Structure, serBonePulses) == 0x48, "Structure");
static_assert(offsetof(Structure, tGeoSets) == 0x50, "Structure");
static_assert(offsetof(Structure, wsBounds) == 0x80, "Structure");
static_assert(offsetof(Structure, nCollisionCapsuleCount) == 0x90, "Structure");
static_assert(offsetof(Structure, serCollisionCapsules) == 0x94, "Structure");
static_assert(offsetof(Structure, ptCollisionCapsules) == 0xa0, "Structure");
static_assert(offsetof(Structure, nHardpointCount) == 0xa8, "Structure");
static_assert(offsetof(Structure, serHardpoints) == 0xac, "Structure");
static_assert(offsetof(Structure, ptHardpoints) == 0xb8, "Structure");
static_assert(offsetof(Structure, wvRigidAnimAxis) == 0xc0, "Structure");
static_assert(offsetof(Structure, tOctreeVisualMesh) == 0xd0, "Structure");
static_assert(offsetof(Structure, aabbBounds) == 0x118, "Structure");
static_assert(offsetof(Structure, nLoopConstraintCount) == 0x130, "Structure");
static_assert(offsetof(Structure, serLoopConstraints) == 0x134, "Structure");
static_assert(offsetof(Structure, ptLoopConstraints) == 0x140, "Structure");
static_assert(offsetof(Structure, uRagdollDegrade) == 0x148, "Structure");
static_assert(offsetof(Structure, ptPostprocessed) == 0x150, "Structure");
static_assert(sizeof(SubObjectAppearance) == 0xf8, "SubObjectAppearance");
static_assert(offsetof(SubObjectAppearance, dwFlags) == 0x0, "SubObjectAppearance");
static_assert(offsetof(SubObjectAppearance, snoCloth) == 0x4, "SubObjectAppearance");
static_assert(offsetof(SubObjectAppearance, serTagMap) == 0x8, "SubObjectAppearance");
static_assert(offsetof(SubObjectAppearance, hTagMap) == 0x10, "SubObjectAppearance");
static_assert(offsetof(SubObjectAppearance, tBaseUberMaterial) == 0x18, "SubObjectAppearance");
static_assert(offsetof(SubObjectAppearance, snoMasterMaterial) == 0x80, "SubObjectAppearance");
static_assert(offsetof(SubObjectAppearance, tCompositeUberMaterial) == 0x88, "SubObjectAppearance");
static_assert(offsetof(SubObjectAppearance, hTagMapMasterMaterial) == 0xf0, "SubObjectAppearance");
static_assert(sizeof(AppearanceMaterial) == 0x18, "AppearanceMaterial");
static_assert(offsetof(AppearanceMaterial, dwMaterialHash) == 0x0, "AppearanceMaterial");
static_assert(offsetof(AppearanceMaterial, dwPad) == 0x4, "AppearanceMaterial");
static_assert(offsetof(AppearanceMaterial, serSOAs) == 0x8, "AppearanceMaterial");
static_assert(offsetof(AppearanceMaterial, ptSOAs) == 0x10, "AppearanceMaterial");
static_assert(sizeof(AppearanceLook) == 0x4, "AppearanceLook");
static_assert(offsetof(AppearanceLook, dwLookHash) == 0x0, "AppearanceLook");
static_assert(sizeof(WalkableZFractionAndSurface) == 0x4, "WalkableZFractionAndSurface");
static_assert(sizeof(ClutterZFractionAndParticle) == 0x4, "ClutterZFractionAndParticle");
static_assert(sizeof(ClutterDensityMaskAndColor) == 0x4, "ClutterDensityMaskAndColor");
static_assert(sizeof(AppearanceSample) == 0xc, "AppearanceSample");
static_assert(sizeof(AppearanceSamplingSubgrid) == 0x38, "AppearanceSamplingSubgrid");
static_assert(offsetof(AppearanceSamplingSubgrid, nSamplesX) == 0x0, "AppearanceSamplingSubgrid");
static_assert(offsetof(AppearanceSamplingSubgrid, nSamplesY) == 0x4, "AppearanceSamplingSubgrid");
static_assert(offsetof(AppearanceSamplingSubgrid, wpBase) == 0x8, "AppearanceSamplingSubgrid");
static_assert(offsetof(AppearanceSamplingSubgrid, serSamples) == 0x10, "AppearanceSamplingSubgrid");
static_assert(offsetof(AppearanceSamplingSubgrid, ptSamples) == 0x18, "AppearanceSamplingSubgrid");
static_assert(offsetof(AppearanceSamplingSubgrid, aabb) == 0x20, "AppearanceSamplingSubgrid");
static_assert(sizeof(AppearanceDefinition) == 0x228, "AppearanceDefinition");
static_assert(offsetof(AppearanceDefinition, tHeader) == 0x0, "AppearanceDefinition");
static_assert(offsetof(AppearanceDefinition, tStructure) == 0x10, "AppearanceDefinition");
static_assert(offsetof(AppearanceDefinition, nNumLooks) == 0x168, "AppearanceDefinition");
static_assert(offsetof(AppearanceDefinition, nNumMaterials) == 0x16c, "AppearanceDefinition");
static_assert(offsetof(AppearanceDefinition, serAppearanceMaterials) == 0x170, "AppearanceDefinition");
static_assert(offsetof(AppearanceDefinition, ptAppearanceMaterials) == 0x178, "AppearanceDefinition");
static_assert(offsetof(AppearanceDefinition, serAppearanceLooks) == 0x180, "AppearanceDefinition");
static_assert(offsetof(AppearanceDefinition, ptAppearanceLooks) == 0x188, "AppearanceDefinition");
static_assert(offsetof(AppearanceDefinition, nStaticLightCount) == 0x190, "AppearanceDefinition");
static_assert(offsetof(AppearanceDefinition, serStaticLights) == 0x194, "AppearanceDefinition");
static_assert(offsetof(AppearanceDefinition, ptStaticLights) == 0x1a0, "AppearanceDefinition");
static_assert(offsetof(AppearanceDefinition, eOverlayProjectionAxis) == 0x1a8, "AppearanceDefinition");
static_assert(offsetof(AppearanceDefinition, dwFadeOutTimeCriticalHitBlood) == 0x1ac, "AppearanceDefinition");
static_assert(offsetof(AppearanceDefinition, dwFadeOutTimeStoned) == 0x1b0, "AppearanceDefinition");
static_assert(offsetof(AppearanceDefinition, dwFadeOutTimeChilled) == 0x1b4, "AppearanceDefinition");
static_assert(offsetof(AppearanceDefinition, dwFadeOutTimeFrozen) == 0x1b8, "AppearanceDefinition");
static_assert(offsetof(AppearanceDefinition, dwFadeOutTimePoisoned) == 0x1bc, "AppearanceDefinition");
static_assert(offsetof(AppearanceDefinition, dwMachineTime) == 0x1c0, "AppearanceDefinition");
static_assert(offsetof(AppearanceDefinition, flUVGenQuality) == 0x1c4, "AppearanceDefinition");
static_assert(offsetof(AppearanceDefinition, dwLightMapWidth) == 0x1c8, "AppearanceDefinition");
static_assert(offsetof(AppearanceDefinition, dwLightMapHeight) == 0x1cc, "AppearanceDefinition");
static_assert(offsetof(AppearanceDefinition, tUnwrapTimeStamp) == 0x1d0, "AppearanceDefinition");
static_assert(offsetof(AppearanceDefinition, flMinZ) == 0x1d8, "AppearanceDefinition");
static_assert(offsetof(AppearanceDefinition, flMaxZ) == 0x1dc, "AppearanceDefinition");
static_assert(offsetof(AppearanceDefinition, nSubgridsX) == 0x1e0, "AppearanceDefinition");
static_assert(offsetof(AppearanceDefinition, nSubgridsY) == 0x1e4, "AppearanceDefinition");
static_assert(offsetof(AppearanceDefinition, tSubgridsAABB) == 0x1e8, "AppearanceDefinition");
static_assert(offsetof(AppearanceDefinition, serSubgrids) == 0x200, "AppearanceDefinition");
static_assert(offsetof(AppearanceDefinition, ptSubgrids) == 0x208, "AppearanceDefinition");
static_assert(offsetof(AppearanceDefinition, serAppearanceSubObjectsIndices) == 0x210, "AppearanceDefinition");
static_assert(offsetof(AppearanceDefinition, serAppearanceSubObjectsVertices) == 0x218, "AppearanceDefinition");
static_assert(offsetof(AppearanceDefinition, dwFlags) == 0x220, "AppearanceDefinition");
static_assert(sizeof(AppearanceGroupDef) == 0xb0, "AppearanceGroupDef");
static_assert(sizeof(ScopedAppearanceBufferContentRetrieval) == 0x8, "ScopedAppearanceBufferContentRetrieval");
