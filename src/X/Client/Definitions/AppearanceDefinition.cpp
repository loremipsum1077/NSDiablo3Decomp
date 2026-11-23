#include "X/Client/Definitions/AppearanceDefinition.h"
#include "../../../Contrib/Contrib/blz/include/blz/tuple.h"
#include "../../Common/SNO/SNOName.h"
#include "../../Common/WorldPlace.h"
#include "../Gfx/GfxVB.h"
#include "../ClothUpdate.h"
#include "../../Common/D3TypeDescriptor/D3TypeDescriptor.h"
#include "../../Sigma/Math/Constraint.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptorField.h"
#include "../../Common/Game.h"
#include "../../Sigma/ErrorManager/ErrorManager.h"
#include "../Physics/DominoBody.h"
#include "../../../Contrib/Contrib/Domino/Include/dmApi.h"
#include "../../../Contrib/Contrib/Domino/Include/dmSettings.h"
#include "../SubObject.h"
struct PolytopeData; typedef int32 AttenType;  // PolytopeData: X/Client/Physics/DominoPolytope.h
struct CollisionShape {  // size 0x50
    enum Type {
        NONE = -1,
        SPHERE = 0,
        CAPSULE = 1,
        POLYTOPE = 2,
    };
    uint32 dwFlags;  // 0x00
    CollisionShape::Type eType;  // 0x04
    Shape::Material eMaterial;  // 0x08
    int32 nDegrade;  // 0x0c
    float flDensityFactor;  // 0x10
    float flExplosionFactor;  // 0x14
    float flVelocityFactor;  // 0x18
    SerializeData serPolytope;  // 0x1c
    XPtr64<PolytopeData> pPolytopeData;  // 0x28
    Vector3D wp1;  // 0x30
    Vector3D wp2;  // 0x3c
    float flRadius;  // 0x48
    BOOL Create(const Shape *);
    void Destroy();
    void Rotate(const Quaternion &);
};
static_assert(sizeof(CollisionShape) == 0x50, "CollisionShape");
struct StaticLight {  // size 0x5c
    BOOL fCastsShadows;  // 0x00
    LightType eType;  // 0x04
    Vector3D wp;  // 0x08
    Vector3D wv;  // 0x14
    WorldDistance wdRadius;  // 0x20
    AttenType eAttenType;  // 0x24
    float flAtten[3];  // 0x28
    float flAttenNearStart;  // 0x34
    float flAttenNearEnd;  // 0x38
    float flAttenFarStart;  // 0x3c
    float flAttenFarEnd;  // 0x40
    Angle aHotspot;  // 0x44
    Angle aFalloff;  // 0x48
    RGBAColorValue rgbavalDiffuse;  // 0x4c
};
static_assert(sizeof(StaticLight) == 0x5c, "StaticLight");
enum {
    APPEARANCE_COMMON_SNO_VERSION = 259,
    APPEARANCE_XML_VERSION = 7,
    APPEARANCE_PC_SNO_VERSION = 260,
    APPEARANCE_CONSOLE_SNO_VERSION = 301,
    APPEARANCE_AXE_SNO_VERSION = 282,
    APPEARANCE_XBOXONE_SNO_VERSION = 305,
    APPEARANCE_PS4_SNO_VERSION = 305,
    APPEARANCE_NX64_SNO_VERSION = 305,
    COLLISIONSHAPE_XML_VERSION = 1,
    SUBOBJECT_XML_VERSION = 4,
    SOA_XML_VERSION = 9,
};




enum {
    MAX_APPEARANCE_DEFS_IN_MEMORY = 256,
    MAX_APPEARANCE_CACHE_SIZE = 314572800,
};











enum {
    DEFAULT_FADE_OUT_TIME_CRITICALHIT_BLOOD = 480,
    DEFAULT_FADE_OUT_TIME_STONED = 30,
    DEFAULT_FADE_OUT_TIME_CHILLED = 30,
    DEFAULT_FADE_OUT_TIME_FROZEN = 30,
    DEFAULT_FADE_OUT_TIME_POISONED = 30,
};

struct AppearanceGlobals {  // size 0xe0

    XChainedFixedBlockAllocator<PackedVBList> allocPackedVbList;  // 0x00
    XChainedFixedBlockAllocator<PackedIBList> allocPackedIbList;  // 0x30
    ChainedFixedBlockAllocator allocStructurePostData;  // 0x60
    ChainedFixedBlockAllocator allocSubObjectPostData;  // 0x90

    Blizzard::Lock::Mutex tAppearanceMutex;  // 0xc0
};

static_assert(sizeof(AppearanceGlobals) == 0xe0, "AppearanceGlobals");
#include "../../Common/SNO/SymbolicValue.h"
#include "../../Sigma/TypeDescriptor/SymbolicValue.h"
// CollisionShape and StaticLight (lines 16-58) are X/Client/Definitions/AppearanceDefinition.h types;
// the src header only forward-declares them.

// Declared here because their headers do not carry them yet.
extern const XTypeDescriptor * DT_SERIALIZEDATA;  // X/Sigma/Files/XFilesCommon.cpp:20
extern const XTypeDescriptor * DT_OCTREE;  // X/Sigma/DataStructures/Octree.cpp:47
extern const XTypeDescriptor * DT_POLYTOPE_DATA;  // X/Client/Physics/DominoPolytope.cpp:42
extern const XTypeDescriptor * DT_SNO;  // X/Common/D3TypeDescriptor/D3DescribedType.cpp

// The type descriptor tables were generated with build/try/solo/desc.py from the original
// static initializer's constructor calls and the DWARF struct layouts.
// The field names are "" in the binary.

// Headers for AppearanceGroupDef::Postprocess (CGameGlobals, Prefetch::AddToQueue,
// gptGroupDefMaterial / ScopedMaterialDefinitionLockR, TagMapRef).
#include "../CGame.h"
#include "../Prefetch.h"
#include "MaterialDefinition.h"
#include "../../Common/TagMaps/Tags.h"
uint32 StringHashI(LPCSTR pszName);  // X/Sigma/TypeDescriptor/SymbolicValue.cpp:117
int32 MaterialGetTextureCount(const UberMaterial & tUberMaterial);  // X/Client/Gfx/GfxMaterial.cpp:412
void MaterialListSort(UberMaterial & tUberMaterial);  // X/Client/Gfx/GfxMaterial.cpp:281
uint32 MaterialTextureBuildHash(const UberMaterial & tUberMaterial);  // X/Client/Gfx/GfxMaterial.cpp:436








static const TypeDescriptorField Influence_Fields[3] = {
    TypeDescriptorField("", DT_INT, offsetof(Influence, nBoneIndex), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(Influence, flWeight), NULL, 1),
TypeDescriptorField(NULL, DT_NULL, sizeof(Influence), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_Influence("Influence", 0, Influence_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_INFLUENCE = &TypeDescriptor_Influence;

static const TypeDescriptorField VertInfluences_Fields[2] = {
    FixedArrayTypeDescriptorField("", DT_INFLUENCE, offsetof(VertInfluences, tBoneInfluence), NULL, 1, 3, 0),
TypeDescriptorField(NULL, DT_NULL, sizeof(VertInfluences), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_VertInfluences("VertInfluences", 0, VertInfluences_Fields, 2, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_VERTINFLUENCES = &TypeDescriptor_VertInfluences;


static const TypeDescriptorField CompInfluence_Fields[3] = {
    TypeDescriptorField("", DT_BYTE, offsetof(CompInfluence, dwBoneIndex), NULL, 1),
    TypeDescriptorField("", DT_BYTE, offsetof(CompInfluence, dwWeight), NULL, 1),
TypeDescriptorField(NULL, DT_NULL, sizeof(CompInfluence), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_CompInfluence("CompInfluence", 0, CompInfluence_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_COMPINFLUENCE = &TypeDescriptor_CompInfluence;

static const TypeDescriptorField CompVertInfluences_Fields[2] = {
    FixedArrayTypeDescriptorField("", DT_COMPINFLUENCE, offsetof(CompVertInfluences, tBoneInfluence), NULL, 1, 3, 0),
TypeDescriptorField(NULL, DT_NULL, sizeof(CompVertInfluences), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_CompVertInfluences("CompVertInfluences", 0, CompVertInfluences_Fields, 2, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_COMPVERTINFLUENCES = &TypeDescriptor_CompVertInfluences;


static const TypeDescriptorField FatVertex_Fields[5] = {
    TypeDescriptorField("", DT_VECTOR3D, offsetof(FatVertex, wpPos), NULL, 1),
    TypeDescriptorField("", DT_RGBACOLOR, offsetof(FatVertex, wvpNormal), NULL, 1),
    FixedArrayTypeDescriptorField("", DT_RGBACOLOR, offsetof(FatVertex, rgbaColor), NULL, 1, 2, 0),
    FixedArrayTypeDescriptorField("", DT_RGBACOLOR, offsetof(FatVertex, vpUVSet), NULL, 1, 2, 0),




TypeDescriptorField(NULL, DT_NULL, sizeof(FatVertex), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_FatVertex("FatVertex", 0, FatVertex_Fields, 5, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_FATVERTEX = &TypeDescriptor_FatVertex;

static const TypeDescriptorField CollisionShape_Fields[13] = {
    TypeDescriptorField("", DT_INT, offsetof(CollisionShape, dwFlags), NULL, 0x80001),
    TypeDescriptorField("", DT_INT, offsetof(CollisionShape, eType), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(CollisionShape, eMaterial), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(CollisionShape, nDegrade), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(CollisionShape, flDensityFactor), (const void *)0x3f800000, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(CollisionShape, flExplosionFactor), (const void *)0x3f800000, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(CollisionShape, flVelocityFactor), (const void *)0x3f800000, 1),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(CollisionShape, serPolytope), NULL, 0), VariableArrayTypeDescriptorField("", DT_POLYTOPE_DATA, offsetof(CollisionShape, pPolytopeData), 0x21, -0xc),
    TypeDescriptorField("", DT_VECTOR3D, offsetof(CollisionShape, wp1), NULL, 1),
    TypeDescriptorField("", DT_VECTOR3D, offsetof(CollisionShape, wp2), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(CollisionShape, flRadius), NULL, 1),
TypeDescriptorField(NULL, DT_NULL, sizeof(CollisionShape), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_CollisionShape("CollisionShape", 1, CollisionShape_Fields, 13, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_COLLISION_SHAPE = &TypeDescriptor_CollisionShape;

static const TypeDescriptorField CollisionCapsule_Fields[4] = {
    TypeDescriptorField("", DT_FLOAT, offsetof(CollisionCapsule, flRadius), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(CollisionCapsule, flLength), NULL, 1),
    TypeDescriptorField("", DT_HARDPOINT, offsetof(CollisionCapsule, tHardpoint), NULL, 1),
TypeDescriptorField(NULL, DT_NULL, sizeof(CollisionCapsule), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_CollisionCapsule("CollisionCapsule", 0, CollisionCapsule_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_COLLISIONCAPSULE = &TypeDescriptor_CollisionCapsule;

static const TypeDescriptorField ClothVertexSerializable_Fields[8] = {
    TypeDescriptorField("", DT_VECTOR3D, offsetof(ClothVertexSerializable, wp), NULL, 1),

    TypeDescriptorField("", DT_VECTOR3D, offsetof(ClothVertexSerializable, wp0), NULL, 1),

    TypeDescriptorField("", DT_FLOAT, offsetof(ClothVertexSerializable, flInvMass), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ClothVertexSerializable, nParentIndex), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ClothVertexSerializable, nAttachmentDistance), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ClothVertexSerializable, nFatVertexIndex), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ClothVertexSerializable, nIslandIndex), NULL, 1),


TypeDescriptorField(NULL, DT_NULL, sizeof(ClothVertexSerializable), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_ClothVertexSerializable("ClothVertexSerializable", 0, ClothVertexSerializable_Fields, 8, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_CLOTH_VERTEX = &TypeDescriptor_ClothVertexSerializable;

static const TypeDescriptorField ClothFaceSerializable_Fields[5] = {

    TypeDescriptorField("", DT_FLOAT, offsetof(ClothFaceSerializable, flRestArea), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ClothFaceSerializable, nVertex1), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ClothFaceSerializable, nVertex2), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ClothFaceSerializable, nVertex3), NULL, 1),
TypeDescriptorField(NULL, DT_NULL, sizeof(ClothFaceSerializable), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_ClothFaceSerializable("ClothFaceSerializable", 0, ClothFaceSerializable_Fields, 5, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_CLOTH_FACE = &TypeDescriptor_ClothFaceSerializable;

static const TypeDescriptorField ClothConstraintSerializable_Fields[5] = {
    TypeDescriptorField("", DT_INT, offsetof(ClothConstraintSerializable, nVertex1), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ClothConstraintSerializable, nVertex2), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(ClothConstraintSerializable, flRestLengthSquared), NULL, 1),


    TypeDescriptorField("", DT_FLOAT, offsetof(ClothConstraintSerializable, flHorizontal), NULL, 1),
TypeDescriptorField(NULL, DT_NULL, sizeof(ClothConstraintSerializable), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_ClothConstraintSerializable("ClothConstraintSerializable", 0, ClothConstraintSerializable_Fields, 5, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_CLOTH_CONSTRAINT = &TypeDescriptor_ClothConstraintSerializable;

static const TypeDescriptorField ClothStaple_Fields[4] = {
    TypeDescriptorField("", DT_INT, offsetof(ClothStaple, nVertex), NULL, 1),
    FixedArrayTypeDescriptorField("", DT_INT, offsetof(ClothStaple, anBoneIndices), NULL, 1, 3, 0),
    FixedArrayTypeDescriptorField("", DT_FLOAT, offsetof(ClothStaple, aflBoneWeights), NULL, 1, 3, 0),
TypeDescriptorField(NULL, DT_NULL, sizeof(ClothStaple), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_ClothStaple("ClothStaple", 0, ClothStaple_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_CLOTH_STAPLE = &TypeDescriptor_ClothStaple;

static const TypeDescriptorField ClothStructure_Fields[20] = {
    TypeDescriptorField("", DT_INT, offsetof(ClothStructure, nVertexCount), NULL, 1),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(ClothStructure, serVertices), NULL, 0), VariableArrayTypeDescriptorField("", DT_CLOTH_VERTEX, offsetof(ClothStructure, ptVertices), 0x21, -0xc),
    TypeDescriptorField("", DT_INT, offsetof(ClothStructure, nFaceCount), NULL, 1),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(ClothStructure, serFaces), NULL, 0), VariableArrayTypeDescriptorField("", DT_CLOTH_FACE, offsetof(ClothStructure, ptFaces), 0x21, -0xc),
    TypeDescriptorField("", DT_INT, offsetof(ClothStructure, nStapleCount), NULL, 1),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(ClothStructure, serStaples), NULL, 0), VariableArrayTypeDescriptorField("", DT_CLOTH_STAPLE, offsetof(ClothStructure, ptStaples), 0x21, -0xc),
    TypeDescriptorField("", DT_INT, offsetof(ClothStructure, nDistanceConstraintCount), NULL, 1),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(ClothStructure, serDistanceConstraints), NULL, 0), VariableArrayTypeDescriptorField("", DT_CLOTH_CONSTRAINT, offsetof(ClothStructure, ptDistanceConstraints), 0x21, -0xc),
    TypeDescriptorField("", DT_INT, offsetof(ClothStructure, nBendingConstraintCount), NULL, 1),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(ClothStructure, serBendingConstraints), NULL, 0), VariableArrayTypeDescriptorField("", DT_CLOTH_CONSTRAINT, offsetof(ClothStructure, ptBendingConstraints), 0x21, -0xc),
    TypeDescriptorField("", DT_INT, offsetof(ClothStructure, nIslandCount), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(ClothStructure, flArea), NULL, 1),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(ClothStructure, serClothVertMapping), NULL, 0x700000), VariableArrayTypeDescriptorField("", DT_WORD, offsetof(ClothStructure, ptClothVertexMapping), 0x700020, -8),
TypeDescriptorField(NULL, DT_NULL, sizeof(ClothStructure), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_ClothStructure("ClothStructure", 0, ClothStructure_Fields, 20, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_CLOTH_STRUCTURE = &TypeDescriptor_ClothStructure;


static const TypeDescriptorField Vertex2TexLCPos_Fields[10] = {
    TypeDescriptorField("", DT_WORD, offsetof(Vertex2TexLCPos, nX), NULL, 1),
    TypeDescriptorField("", DT_WORD, offsetof(Vertex2TexLCPos, nY), NULL, 1),
    TypeDescriptorField("", DT_WORD, offsetof(Vertex2TexLCPos, nZ), NULL, 1),
    TypeDescriptorField("", DT_WORD, offsetof(Vertex2TexLCPos, nPad), NULL, 1),
    TypeDescriptorField("", DT_RGBACOLOR, offsetof(Vertex2TexLCPos, wvpNormal), NULL, 1),


    TypeDescriptorField("", DT_RGBACOLOR, offsetof(Vertex2TexLCPos, vcDiffuse), NULL, 1),
    TypeDescriptorField("", DT_RGBACOLOR, offsetof(Vertex2TexLCPos, vcAuxColor), NULL, 1),
    TypeDescriptorField("", DT_RGBACOLOR, offsetof(Vertex2TexLCPos, vpUV0), NULL, 1),
    TypeDescriptorField("", DT_RGBACOLOR, offsetof(Vertex2TexLCPos, vpUV1), NULL, 1),
TypeDescriptorField(NULL, DT_NULL, sizeof(Vertex2TexLCPos), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_Vertex2TexLCPos("Vertex2TexLCPos", 0, Vertex2TexLCPos_Fields, 10, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_VERTEX2TEXLCPOS = &TypeDescriptor_Vertex2TexLCPos;

static const TypeDescriptorField VertexSkinCPos_Fields[12] = {
    TypeDescriptorField("", DT_WORD, offsetof(VertexSkinCPos, nX), NULL, 1),
    TypeDescriptorField("", DT_WORD, offsetof(VertexSkinCPos, nY), NULL, 1),
    TypeDescriptorField("", DT_WORD, offsetof(VertexSkinCPos, nZ), NULL, 1),
    TypeDescriptorField("", DT_WORD, offsetof(VertexSkinCPos, nPad), NULL, 1),
    TypeDescriptorField("", DT_RGBACOLOR, offsetof(VertexSkinCPos, wvpNormal), NULL, 1),


    TypeDescriptorField("", DT_RGBACOLOR, offsetof(VertexSkinCPos, vcDiffuse), NULL, 1),
    TypeDescriptorField("", DT_RGBACOLOR, offsetof(VertexSkinCPos, vcAuxColor), NULL, 1),
    TypeDescriptorField("", DT_RGBACOLOR, offsetof(VertexSkinCPos, vpUV0), NULL, 1),
    TypeDescriptorField("", DT_RGBACOLOR, offsetof(VertexSkinCPos, vpUV1), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(VertexSkinCPos, dwMatrixIndices), NULL, 1),
    TypeDescriptorField("", DT_RGBACOLOR, offsetof(VertexSkinCPos, vMatrixWeights), NULL, 1),
TypeDescriptorField(NULL, DT_NULL, sizeof(VertexSkinCPos), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_VertexSkinCPos("VertexSkinCPos", 0, VertexSkinCPos_Fields, 12, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_VERTEXSKINCPOS = &TypeDescriptor_VertexSkinCPos;

static const TypeDescriptorField SkinningInfo_Fields[10] = {
    TypeDescriptorField("", DT_INT, offsetof(SkinningInfo, nNumBones), NULL, 0),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(SkinningInfo, serBoneIDs), NULL, 0), VariableArrayTypeDescriptorField("", DT_INT, offsetof(SkinningInfo, pBoneIDs), 0x20, -0xc),
    TypeDescriptorField("", DT_INT, offsetof(SkinningInfo, nPackVertCount), NULL, 0),
    TypeDescriptorField("", DT_INT64, offsetof(SkinningInfo, pPackVertList), NULL, 0),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(SkinningInfo, serPackVertList), NULL, 0),
    TypeDescriptorField("", DT_INT, offsetof(SkinningInfo, nPackIndexCount), NULL, 0),
    TypeDescriptorField("", DT_INT64, offsetof(SkinningInfo, pPackIndexList), NULL, 0),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(SkinningInfo, serPackIndexList), NULL, 0),
TypeDescriptorField(NULL, DT_NULL, sizeof(SkinningInfo), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_SkinningInfo("SkinningInfo", 0, SkinningInfo_Fields, 10, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_SKINNINGINFO = &TypeDescriptor_SkinningInfo;




























static const TypeDescriptorField SubObject_Fields[34] = {
    TypeDescriptorField("", DT_INT, offsetof(SubObject, dwFlags), NULL, 0x80001),
    TypeDescriptorField("", DT_INT, offsetof(SubObject, nVertCount), NULL, 0),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(SubObject, serVertList), NULL, 0), VariableArrayTypeDescriptorField("", DT_FATVERTEX, offsetof(SubObject, ptVertList), 0x20, -8),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(SubObject, serInfluenceList), NULL, 0), VariableArrayTypeDescriptorField("", DT_VERTINFLUENCES, offsetof(SubObject, ptInfluenceList), 0x20, -8),
    TypeDescriptorField("", DT_INT, offsetof(SubObject, nIndexCount), NULL, 0),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(SubObject, serIndexList), NULL, 0), VariableArrayTypeDescriptorField("", DT_WORD, offsetof(SubObject, pwIndexList), 0x20, -0xc),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(SubObject, serClothStructure), NULL, 0), VariableArrayTypeDescriptorField("", DT_CLOTH_STRUCTURE, offsetof(SubObject, ptClothStructure), 0x20, -8),

    FilterTypeDescriptorField("", DT_SNO, offsetof(SubObject, snoSurface), "", SURFACE_SNO, 0, -1),
    TypeDescriptorField("", DT_INT, offsetof(SubObject, nFadeGroup), (const void *)-1, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(SubObject, flFadeAlpha), NULL, 1),



    TypeDescriptorField("", DT_INT, offsetof(SubObject, tNameInfo), NULL, 0x700000),
    TypeDescriptorField("", DT_INT, offsetof(SubObject, dwPad3), NULL, 0x700000),
    TypeDescriptorField("", DT_INT, offsetof(SubObject, dwMaterialHash), NULL, 0x700000),
    TypeDescriptorField("", DT_INT, offsetof(SubObject, dwSubObjectHash), NULL, 0x700000),
    TypeDescriptorField("", DT_AABB, offsetof(SubObject, aabbBounds), NULL, 0),

    TypeDescriptorField("", DT_INT, offsetof(SubObject, nShapeCount), NULL, 0),

    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(SubObject, serShapes), NULL, 0), VariableArrayTypeDescriptorField("", DT_COLLISION_SHAPE, offsetof(SubObject, ptShapes), 0x20, -8),

    TypeDescriptorField("", DT_INT64, offsetof(SubObject, ptPostprocessed), NULL, 0),
    TypeDescriptorField("", DT_INT, offsetof(SubObject, nPackVertCount), NULL, 0x700000),
    TypeDescriptorField("", DT_INT, offsetof(SubObject, dwPad), NULL, 0x700000),
    TypeDescriptorField("", DT_INT64, offsetof(SubObject, pPackVertList), NULL, 0x700000),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(SubObject, serPackVertList), NULL, 0x700000),

    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(SubObject, serSkinningInfo), NULL, 0x700000), VariableArrayTypeDescriptorField("", DT_SKINNINGINFO, offsetof(SubObject, ptSkinningInfo), 0x740020, -8),

    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(SubObject, serCompInfluenceList), NULL, 0x700000), VariableArrayTypeDescriptorField("", DT_COMPVERTINFLUENCES, offsetof(SubObject, ptCompInfluenceList), 0x700020, -8),
    TypeDescriptorField("", DT_VECTOR3D, offsetof(SubObject, wpFixedPointPosOffset), NULL, 0x700000),
    TypeDescriptorField("", DT_VECTOR3D, offsetof(SubObject, wpFixedPointPosScale), NULL, 0x700000),

TypeDescriptorField(NULL, DT_NULL, sizeof(SubObject), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_SubObject("SubObject", 4, SubObject_Fields, 34, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_SUBOBJECT = &TypeDescriptor_SubObject;

static const TypeDescriptorField BoneStructure_Fields[17] = {

    TypeDescriptorField("", DT_INT, offsetof(BoneStructure, dwHash), NULL, 0x700000),
    TypeDescriptorField("", DT_INT, offsetof(BoneStructure, dwPad), NULL, 0x700000),
    TypeDescriptorField("", DT_INT, offsetof(BoneStructure, nParentIndex), NULL, 1),
    TypeDescriptorField("", DT_AABB, offsetof(BoneStructure, aabb), NULL, 1),
    TypeDescriptorField("", DT_SPHERE, offsetof(BoneStructure, wsBounds), NULL, 0),
    TypeDescriptorField("", DT_PRSTRANSFORM, offsetof(BoneStructure, transform), NULL, 1),
    TypeDescriptorField("", DT_PRSTRANSFORM, offsetof(BoneStructure, transformInv), NULL, 1),
    TypeDescriptorField("", DT_PRSTRANSFORM, offsetof(BoneStructure, transformParentRel), NULL, 1),
    TypeDescriptorField("", DT_PRSTRANSFORM, offsetof(BoneStructure, transformBindPose), NULL, 1),
    TypeDescriptorField("", DT_PRSTRANSFORM, offsetof(BoneStructure, transformBindPoseInv), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(BoneStructure, nShapeCount), NULL, 1),

    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(BoneStructure, serShapes), NULL, 0), VariableArrayTypeDescriptorField("", DT_COLLISION_SHAPE, offsetof(BoneStructure, ptShapes), 0x21, -8),

    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(BoneStructure, serConstraint), NULL, 0), VariableArrayTypeDescriptorField("", DT_CONSTRAINTPARAMETERS, offsetof(BoneStructure, ptConstraint), 0x21, -8),
    FilterTypeDescriptorField("", DT_SNO, offsetof(BoneStructure, snoParticleSystem), "", PARTICLE_SNO, 0, -1),
TypeDescriptorField(NULL, DT_NULL, sizeof(BoneStructure), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_BoneStructure("BoneStructure", 0, BoneStructure_Fields, 17, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_BONESTRUCTURE = &TypeDescriptor_BoneStructure;

static const TypeDescriptorField LookAtData_Fields[10] = {
    TypeDescriptorField("", DT_INT, offsetof(LookAtData, bEnabled), NULL, 1),

    TypeDescriptorField("", DT_INT, offsetof(LookAtData, dwBoneHash), NULL, 0x700000),
    TypeDescriptorField("", DT_INT, offsetof(LookAtData, dwBoneHashPadding), NULL, 0x700000),
    TypeDescriptorField("", DT_ANGLE, offsetof(LookAtData, aAngleLimitYaw), NULL, 1),
    TypeDescriptorField("", DT_ANGLE, offsetof(LookAtData, aPitchMin), NULL, 1),
    TypeDescriptorField("", DT_ANGLE, offsetof(LookAtData, aPitchMax), NULL, 1),
    TypeDescriptorField("", DT_ANGLE, offsetof(LookAtData, aPitchOffset), NULL, 1),
    TypeDescriptorField("", DT_ANGULARVELOCITY, offsetof(LookAtData, aSpeed), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(LookAtData, flRange), NULL, 1),
TypeDescriptorField(NULL, DT_NULL, sizeof(LookAtData), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_LookAtData("LookAtData", 0, LookAtData_Fields, 10, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_LOOKATDATA = &TypeDescriptor_LookAtData;

static const TypeDescriptorField BonePulseData_Fields[6] = {

    TypeDescriptorField("", DT_INT, offsetof(BonePulseData, dwBoneHash), NULL, 0x700000),
    TypeDescriptorField("", DT_INT, offsetof(BonePulseData, dwBoneHashPadding), NULL, 0x700000),
    TypeDescriptorField("", DT_FLOAT, offsetof(BonePulseData, flAmplitude), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(BonePulseData, flHertz), NULL, 1),
    TypeDescriptorField("", DT_ANGLE, offsetof(BonePulseData, aPhase), NULL, 1),
TypeDescriptorField(NULL, DT_NULL, sizeof(BonePulseData), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_BonePulseData("BonePulseData", 0, BonePulseData_Fields, 6, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_BONEPULSEATDATA = &TypeDescriptor_BonePulseData;

static const TypeDescriptorField GeoSet_Fields[4] = {
    TypeDescriptorField("", DT_INT, offsetof(GeoSet, nSubObjectCount), NULL, 1),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(GeoSet, serSubObjects), NULL, 0), VariableArrayTypeDescriptorField("", DT_SUBOBJECT, offsetof(GeoSet, ptSubObjects), 0x21, -0xc),
TypeDescriptorField(NULL, DT_NULL, sizeof(GeoSet), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_GeoSet("GeoSet", 0, GeoSet_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_GEOSET = &TypeDescriptor_GeoSet;

static const TypeDescriptorField Structure_Fields[25] = {
    TypeDescriptorField("", DT_INT, offsetof(Structure, dwFlags), NULL, 0),
    TypeDescriptorField("", DT_INT, offsetof(Structure, nBoneCount), NULL, 0),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(Structure, serBoneStructure), NULL, 0), VariableArrayTypeDescriptorField("", DT_BONESTRUCTURE, offsetof(Structure, ptBoneStructure), 0x20, -8),
    TypeDescriptorField("", DT_LOOKATDATA, offsetof(Structure, tLookAtData), NULL, 1),

    TypeDescriptorField("", DT_INT, offsetof(Structure, nBonePulseCount), NULL, 1),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(Structure, serBonePulses), NULL, 0), VariableArrayTypeDescriptorField("", DT_BONEPULSEATDATA, offsetof(Structure, ptBonePulses), 0x21, 8),

    FixedArrayTypeDescriptorField("", DT_GEOSET, offsetof(Structure, tGeoSets), NULL, 1, 2, 0),
    TypeDescriptorField("", DT_SPHERE, offsetof(Structure, wsBounds), NULL, 1),

    TypeDescriptorField("", DT_INT, offsetof(Structure, nCollisionCapsuleCount), NULL, 0),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(Structure, serCollisionCapsules), NULL, 0), VariableArrayTypeDescriptorField("", DT_COLLISIONCAPSULE, offsetof(Structure, ptCollisionCapsules), 0x20, -0xc),
    TypeDescriptorField("", DT_INT, offsetof(Structure, nHardpointCount), NULL, 0),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(Structure, serHardpoints), NULL, 0), VariableArrayTypeDescriptorField("", DT_HARDPOINT, offsetof(Structure, ptHardpoints), 0x20, -0xc),
    TypeDescriptorField("", DT_VECTOR3D, offsetof(Structure, wvRigidAnimAxis), &cgwvZAxis, 1),
    TypeDescriptorField("", DT_OCTREE, offsetof(Structure, tOctreeVisualMesh), NULL, 0),
    TypeDescriptorField("", DT_AABB, offsetof(Structure, aabbBounds), NULL, 0),

    TypeDescriptorField("", DT_INT64, offsetof(Structure, ptPostprocessed), NULL, 0),








    TypeDescriptorField("", DT_INT, offsetof(Structure, nLoopConstraintCount), NULL, 0),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(Structure, serLoopConstraints), NULL, 0), VariableArrayTypeDescriptorField("", DT_CONSTRAINTPARAMETERS, offsetof(Structure, ptLoopConstraints), 0x20, -0xc),

    TypeDescriptorField("", DT_INT, offsetof(Structure, uRagdollDegrade), NULL, 0),
TypeDescriptorField(NULL, DT_NULL, sizeof(Structure), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_Structure("Structure", 4, Structure_Fields, 25, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_STRUCTURE = &TypeDescriptor_Structure;

static const TypeDescriptorField SubObjectAppearance_Fields[8] = {
    TypeDescriptorField("", DT_INT, offsetof(SubObjectAppearance, dwFlags), NULL, 0x80001),
    FilterTypeDescriptorField("", DT_SNO, offsetof(SubObjectAppearance, snoCloth), "", CLOTH_SNO, 0, -1),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(SubObjectAppearance, serTagMap), NULL, 0), TagMapTypeDescriptorField("", 0x10, 0x21, -8, 0xa0000),

    TypeDescriptorField("", DT_UBERMATERIAL, offsetof(SubObjectAppearance, tBaseUberMaterial), NULL, 1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(SubObjectAppearance, snoMasterMaterial), "", MATERIAL_SNO, 0, -1),
    TypeDescriptorField("", DT_UBERMATERIAL, offsetof(SubObjectAppearance, tCompositeUberMaterial), NULL, 0),

TypeDescriptorField(NULL, DT_NULL, sizeof(SubObjectAppearance), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_SubObjectAppearance("SubObjectAppearance", 9, SubObjectAppearance_Fields, 8, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_SUBOBJECTAPPEARANCE = &TypeDescriptor_SubObjectAppearance;

static const TypeDescriptorField AppearanceMaterial_Fields[5] = {

    TypeDescriptorField("", DT_INT, offsetof(AppearanceMaterial, dwMaterialHash), NULL, 0x700000),
    TypeDescriptorField("", DT_INT, offsetof(AppearanceMaterial, dwPad), NULL, 0x700000),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(AppearanceMaterial, serSOAs), NULL, 0), VariableArrayTypeDescriptorField("", DT_SUBOBJECTAPPEARANCE, offsetof(AppearanceMaterial, ptSOAs), 0x21, -8),
TypeDescriptorField(NULL, DT_NULL, sizeof(AppearanceMaterial), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_AppearanceMaterial("AppearanceMaterial", 0, AppearanceMaterial_Fields, 5, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_APPEARANCEMATERIAL = &TypeDescriptor_AppearanceMaterial;

static const TypeDescriptorField AppearanceLook_Fields[2] = {

    TypeDescriptorField("", DT_INT, offsetof(AppearanceLook, dwLookHash), NULL, 0x700000),
TypeDescriptorField(NULL, DT_NULL, sizeof(AppearanceLook), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_AppearanceLook("AppearanceLook", 0, AppearanceLook_Fields, 2, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_APPEARANCE_LOOK = &TypeDescriptor_AppearanceLook;

static const TypeDescriptorField StaticLight_Fields[15] = {
    TypeDescriptorField("", DT_INT, offsetof(StaticLight, fCastsShadows), NULL, 0),
    TypeDescriptorField("", DT_INT, offsetof(StaticLight, eType), NULL, 0),

    TypeDescriptorField("", DT_VECTOR3D, offsetof(StaticLight, wp), NULL, 0),
    TypeDescriptorField("", DT_VECTOR3D, offsetof(StaticLight, wv), NULL, 0),

    TypeDescriptorField("", DT_FLOAT, offsetof(StaticLight, wdRadius), NULL, 0),

    TypeDescriptorField("", DT_INT, offsetof(StaticLight, eAttenType), NULL, 0),
    FixedArrayTypeDescriptorField("", DT_FLOAT, offsetof(StaticLight, flAtten), NULL, 0, 3, 0),

    TypeDescriptorField("", DT_FLOAT, offsetof(StaticLight, flAttenNearStart), NULL, 0),
    TypeDescriptorField("", DT_FLOAT, offsetof(StaticLight, flAttenNearEnd), NULL, 0),
    TypeDescriptorField("", DT_FLOAT, offsetof(StaticLight, flAttenFarStart), NULL, 0),
    TypeDescriptorField("", DT_FLOAT, offsetof(StaticLight, flAttenFarEnd), NULL, 0),

    TypeDescriptorField("", DT_INT, offsetof(StaticLight, aHotspot), NULL, 0),
    TypeDescriptorField("", DT_INT, offsetof(StaticLight, aFalloff), NULL, 0),

    TypeDescriptorField("", DT_RGBACOLORVALUE, offsetof(StaticLight, rgbavalDiffuse), NULL, 0),

TypeDescriptorField(NULL, DT_NULL, sizeof(StaticLight), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_StaticLight("StaticLight", 0, StaticLight_Fields, 15, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_STATICLIGHT = &TypeDescriptor_StaticLight;





static const TypeDescriptorField AppearanceSample_Fields[4] = {
    TypeDescriptorField("", DT_INT, offsetof(AppearanceSample, dwWalkableZFractionAndSurface), NULL, 1),
    TypeDescriptorField("", DT_INT, 4, NULL, 1),
    TypeDescriptorField("", DT_INT, 8, NULL, 1),
TypeDescriptorField(NULL, DT_NULL, sizeof(AppearanceSample), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_AppearanceSample("AppearanceSample", 0, AppearanceSample_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_APPEARANCE_SAMPLE = &TypeDescriptor_AppearanceSample;

static const TypeDescriptorField AppearanceSamplingSubgrid_Fields[7] = {
    TypeDescriptorField("", DT_INT, offsetof(AppearanceSamplingSubgrid, nSamplesX), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(AppearanceSamplingSubgrid, nSamplesY), NULL, 1),
    TypeDescriptorField("", DT_VECTOR2D, offsetof(AppearanceSamplingSubgrid, wpBase), NULL, 1),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(AppearanceSamplingSubgrid, serSamples), NULL, 0), VariableArrayTypeDescriptorField("", DT_APPEARANCE_SAMPLE, offsetof(AppearanceSamplingSubgrid, ptSamples), 0x21, -8),
    TypeDescriptorField("", DT_AABB, offsetof(AppearanceSamplingSubgrid, aabb), NULL, 1),
TypeDescriptorField(NULL, DT_NULL, sizeof(AppearanceSamplingSubgrid), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_AppearanceSamplingSubgrid("AppearanceSamplingSubgrid", 0, AppearanceSamplingSubgrid_Fields, 7, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_APPEARANCE_SAMPLING_SUBGRID = &TypeDescriptor_AppearanceSamplingSubgrid;



static const TypeDescriptorField AppearanceDefinition_Fields[32] = {




    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(AppearanceDefinition, serAppearanceMaterials), NULL, 0), VariableArrayTypeDescriptorField("", DT_APPEARANCEMATERIAL, offsetof(AppearanceDefinition, ptAppearanceMaterials), 0x21, -8),
    VariableArrayCountTypeDescriptorField("", offsetof(AppearanceDefinition, nNumMaterials), 0x10000, DT_APPEARANCEMATERIAL, 4),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(AppearanceDefinition, serAppearanceLooks), NULL, 0), VariableArrayTypeDescriptorField("", DT_APPEARANCE_LOOK, offsetof(AppearanceDefinition, ptAppearanceLooks), 0x21, -8),
    VariableArrayCountTypeDescriptorField("", offsetof(AppearanceDefinition, nNumLooks), 0x10000, DT_APPEARANCE_LOOK, 0x18),
    TypeDescriptorField("", DT_STRUCTURE, offsetof(AppearanceDefinition, tStructure), NULL, 1),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(AppearanceDefinition, serStaticLights), NULL, 0), VariableArrayTypeDescriptorField("", DT_STATICLIGHT, offsetof(AppearanceDefinition, ptStaticLights), 0x21, -0xc),
    VariableArrayCountTypeDescriptorField("", offsetof(AppearanceDefinition, nStaticLightCount), 0x10000, DT_STATICLIGHT, 4),

    TypeDescriptorField("", DT_INT, offsetof(AppearanceDefinition, eOverlayProjectionAxis), NULL, 1),
    TypeDescriptorField("", DT_TIME, offsetof(AppearanceDefinition, dwFadeOutTimeCriticalHitBlood), (const void *)0x1e0, 1),
    TypeDescriptorField("", DT_TIME, offsetof(AppearanceDefinition, dwFadeOutTimeStoned), (const void *)0x1e, 1),
    TypeDescriptorField("", DT_TIME, offsetof(AppearanceDefinition, dwFadeOutTimeChilled), (const void *)0x1e, 1),
    TypeDescriptorField("", DT_TIME, offsetof(AppearanceDefinition, dwFadeOutTimeFrozen), (const void *)0x1e, 1),
    TypeDescriptorField("", DT_TIME, offsetof(AppearanceDefinition, dwFadeOutTimePoisoned), (const void *)0x1e, 1),

    TypeDescriptorField("", DT_INT, offsetof(AppearanceDefinition, dwMachineTime), NULL, 1),

    TypeDescriptorField("", DT_FLOAT, offsetof(AppearanceDefinition, flUVGenQuality), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(AppearanceDefinition, dwLightMapWidth), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(AppearanceDefinition, dwLightMapHeight), NULL, 1),
    TypeDescriptorField("", DT_INT64, offsetof(AppearanceDefinition, tUnwrapTimeStamp), NULL, 1),

    TypeDescriptorField("", DT_INT, offsetof(AppearanceDefinition, dwFlags), NULL, 0x80001),

    TypeDescriptorField("", DT_FLOAT, offsetof(AppearanceDefinition, flMinZ), NULL, 0x700000),
    TypeDescriptorField("", DT_FLOAT, offsetof(AppearanceDefinition, flMaxZ), NULL, 0x700000),
    TypeDescriptorField("", DT_INT, offsetof(AppearanceDefinition, nSubgridsX), NULL, 0x700000),
    TypeDescriptorField("", DT_INT, offsetof(AppearanceDefinition, nSubgridsY), NULL, 0x700000),
    TypeDescriptorField("", DT_AABB, offsetof(AppearanceDefinition, tSubgridsAABB), NULL, 0x700000),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(AppearanceDefinition, serSubgrids), NULL, 0x700000), VariableArrayTypeDescriptorField("", DT_APPEARANCE_SAMPLING_SUBGRID, offsetof(AppearanceDefinition, ptSubgrids), 0x700020, -8),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(AppearanceDefinition, serAppearanceSubObjectsIndices), NULL, 0x700000),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(AppearanceDefinition, serAppearanceSubObjectsVertices), NULL, 0x700000),

TypeDescriptorField(NULL, DT_NULL, sizeof(AppearanceDefinition), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_AppearanceDefinition("Appearance", 7, AppearanceDefinition_Fields, 32, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_APPEARANCE = &TypeDescriptor_AppearanceDefinition;





AppearanceGroupDef * gptGroupDefAppearance;





AppearanceGroupDef::AppearanceGroupDef() : GroupDefinition(
	"Appearance",
	APPEARANCE_SNO,
	SNOVersions(APPEARANCE_PC_SNO_VERSION, APPEARANCE_AXE_SNO_VERSION + AXE_BASE_SNO_VERSION, APPEARANCE_XBOXONE_SNO_VERSION, APPEARANCE_PS4_SNO_VERSION, APPEARANCE_NX64_SNO_VERSION + NX64_BASE_SNO_VERSION),
	SNODATATYPE_TITLE,
	sizeof(AppearanceDefinition),
	DT_APPEARANCE,
	// the last two arguments are literals in the binary (1, 0x941a); their names are not in the DWARF
	1,
	0x941a),















    tMD5Digest(Blizzard::Util::Digest<16>::k_invalid)
{
}




uint32 AppearanceGroupDef::GetMaxCount()
// DWARF: '{' at 588 and the return at 597; the lines between are not in the binary
{








	return MAX_APPEARANCE_DEFS_IN_MEMORY;
}





uint32 AppearanceGroupDef::GetEstCacheSizeLimit()

{
    return MAX_APPEARANCE_CACHE_SIZE;
}




void AppearanceGroupDef::GetDefaultData(
	void * ptDefaultDef) const
{
	SigmaMemoryClear(ptDefaultDef, sizeof(AppearanceDefinition));
	AppearanceDefinition * pdef = (AppearanceDefinition *)ptDefaultDef;

	pdef->tStructure.wvRigidAnimAxis = cgwvZAxis;

	pdef->nNumLooks = 1;
	// OverlayProjectionAxis has no enumerators in the DWARF; 0 is the stored value (col 31 = this '=')
	pdef->eOverlayProjectionAxis = 0;
	// lines 624-627 are inferred; only 628 (col 28) has a row of its own
	pdef->dwFadeOutTimeCriticalHitBlood = DEFAULT_FADE_OUT_TIME_CRITICALHIT_BLOOD;
	pdef->dwFadeOutTimeStoned = DEFAULT_FADE_OUT_TIME_STONED;
	pdef->dwFadeOutTimeChilled = DEFAULT_FADE_OUT_TIME_CHILLED;
	pdef->dwFadeOutTimeFrozen = DEFAULT_FADE_OUT_TIME_FROZEN;
	pdef->dwFadeOutTimePoisoned = DEFAULT_FADE_OUT_TIME_POISONED;
}













































































































// Declarations for the Postprocess helpers below (GfxVB.cpp:1000/928, GfxIB.cpp:883/714, Gfx.cpp:81).
extern VBFormatInfo cgtVBFormatInfo[NUM_VBFORMATS];
void PackedVBListAllocProxy(PackedVBList * ptPVBList, const VBFormat eVBFormat, LPCSTR szDebugName);
void PackedIBListAllocProxy(PackedIBList * ptPIBList, LPCSTR szDebugName);
void GfxVBSetProxy(const VBID vbid, const void * ptSrc);
void GfxIBSetProxy(const IBID ibid, uint16 * pwSrc);
// Forward declarations; the definitions are at 3751, 3784, 3817 and 3847.
static StructurePostData * sAllocStructurePostData();
static SubObjectPostData * sAllocSubObjectPostData();
static PackedVBList * sAllocPackedVBList();
static PackedIBList * sAllocPackedIBList();








static void sPackVB(
	PackedVBList *& ptPVBList,
	const GeoSet & tGeoSet,
	VBFormat eVBFormat)
{
    uint32 dwVertSize = cgtVBFormatInfo[eVBFormat].dwSize;

    ptPVBList = sAllocPackedVBList();










    PackedVB * ptPVB = NULL;


    for (int32 nSubObject = 0; nSubObject < tGeoSet.nSubObjectCount; ++nSubObject)
    {
        SubObject & tSubObject = tGeoSet.ptSubObjects[nSubObject];
        SubObjectPostData & tSubObjectPost = *tSubObject.ptPostprocessed;

        if (tSubObject.nVertCount > 0)
        {
            PackedVBEntry & tEntry = tSubObjectPost.tVBEntry;
            int32 nCount = tSubObject.nVertCount;

            // Inference: tPVB (794) is in a nested block of its own and the AddTail call is one level deeper (col 17).


            {
                PackedVB tPVB;


                ptPVBList->AddTail_Tracked(tPVB);
            }

            ptPVB = &ptPVBList->GetTail();
            tEntry.dwVertexOffset = ptPVB->nVertCount;
            tEntry.dwStreamOffset = tEntry.dwVertexOffset * dwVertSize;
            tEntry.ptPVB = ptPVB;
            ptPVB->nVertCount += nCount;
        }
        else
        {

            PackedVBEntry &tEntry = tSubObjectPost.tVBEntry;
            SigmaMemoryClear(&tEntry, sizeof(tEntry));
        }
    }
}






































































static void sPackPackedVB(
	PackedVBList *& ptPVBList,
	const GeoSet & tGeoSet,
	VBFormat eVBFormat)
{
    uint32 dwVertSize = cgtVBFormatInfo[eVBFormat].dwSize;

    ptPVBList = sAllocPackedVBList();










    PackedVB * ptPVB = NULL;


    for (int32 nSubObject = 0; nSubObject < tGeoSet.nSubObjectCount; ++nSubObject)
    {
        SubObject & tSubObject = tGeoSet.ptSubObjects[nSubObject];
        SubObjectPostData & tSubObjectPost = *tSubObject.ptPostprocessed;

        if ((tSubObject.dwFlags & (1 << SUBOBJECT_PACK_GEOMETRY_BIT)) && !(tSubObject.dwFlags & (1 << SUBOBJECT_PACK_SKIN_BIT)))
        {

            PackedVBEntry & tEntry = tSubObjectPost.tPackVBEntry;
            int32 nCount = tSubObject.nPackVertCount;




            {
                PackedVB tPVB;


                ptPVBList->AddTail_Tracked(tPVB);
            }

            ptPVB = &ptPVBList->GetTail();
            tEntry.dwVertexOffset = ptPVB->nVertCount;
            tEntry.dwStreamOffset = tEntry.dwVertexOffset * dwVertSize;
            tEntry.ptPVB = ptPVB;
            ptPVB->nVertCount += nCount;
        }
        else
        {

            PackedVBEntry &tEntry = tSubObjectPost.tPackVBEntry;
            SigmaMemoryClear(&tEntry, sizeof(tEntry));
        }
    }
}







static void sPackSkinVB(
	PackedVBList *& ptPVBListSkin,



	PackedIBList *& ptPIBList,
	const GeoSet & tGeoSet)
{
	uint32 dwSkinSize = cgtVBFormatInfo[VBFORMAT_SKIN].dwSize;
	PackedVB tPVBSkin;
	PackedIB tPIB;
	// Inference: tPIB is {0, IBID_INVALID} when it is added (PackedIB has no constructor); where it is set is not known.
	tPIB.nIndexCount = 0;
	tPIB.ibid = IBID_INVALID;



	ptPVBListSkin = sAllocPackedVBList();
	ptPIBList = sAllocPackedIBList();





	PackedVB * ptPVBSkin = NULL;
















	PackedIB * ptPIB = NULL;


	for (int32 nSubObject = 0; nSubObject < tGeoSet.nSubObjectCount; ++nSubObject)
	{
		SubObject & tSubObject = tGeoSet.ptSubObjects[nSubObject];

        int32 nSkinningInfoCount = tSubObject.serSkinningInfo.dwSize / sizeof(SkinningInfo);

        for (int32 nInfo = 0; nInfo < nSkinningInfoCount; nInfo++)
		{
			SkinningInfo & tInfo = tSubObject.ptSkinningInfo[nInfo];
			PackedVBEntry64 & tEntrySkin = tInfo.tPackVBEntry;
			PackedIBEntry64 & tEntryIndex = tInfo.tPackIBEntry;

            int32 nVtxCount = tInfo.nPackVertCount;
            int32 nIndexCount = tInfo.nPackIndexCount;



















			// Inference: the original indents the AddTail/GetTail pairs one level deeper (cols 5/18 and 17/26).
				ptPVBListSkin->AddTail_Tracked(tPVBSkin);
				ptPVBSkin = &ptPVBListSkin->GetTail();











                ptPIBList->AddTail_Tracked(tPIB);
                ptPIB = &ptPIBList->GetTail();



			tEntrySkin.dwVertexOffset = ptPVBSkin->nVertCount;
			tEntrySkin.dwStreamOffset = tEntrySkin.dwVertexOffset * dwSkinSize;
			tEntrySkin.ptPVB = ptPVBSkin;
			ptPVBSkin->nVertCount += nVtxCount;








			tEntryIndex.dwIndexOffset = ptPIB->nIndexCount;
			tEntryIndex.ptPIB = ptPIB;
			ptPIB->nIndexCount += nIndexCount;
		}
	}
}







static void sPackVBs(
	AppearanceDefinition * pdef)
{
	Structure & tStructure = pdef->tStructure;

	for (int32 nGeoSet = 0; nGeoSet < NUM_GEOSETS; ++nGeoSet)
	{
		StructureGeoSetPostData & tGeoSetPost = tStructure.ptPostprocessed->tGeoSetPost[nGeoSet];
		const GeoSet & tGeoSet = tStructure.tGeoSets[nGeoSet];

		if (!tGeoSet.nSubObjectCount)
			continue;



        sPackVB(tGeoSetPost.ptPVBList, tGeoSet, VBFORMAT_2TEX_L);




        sPackPackedVB(tGeoSetPost.ptPackPVBList, tGeoSet, VBFORMAT_2TEX_L_CPOS);


		if (!(tStructure.dwFlags & (1 << STRUCTURE_CANNOT_ANIMATE_BIT)))
		{
			sPackSkinVB(
				tGeoSetPost.ptSkinPVBList,



				tGeoSetPost.ptSkinPIBList,
				tGeoSet);
		}
	}
}




static uint32 sPackIB(
	PackedIBList *& ptPIBList,
	GeoSet & tGeoSet)
{
    ptPIBList = sAllocPackedIBList();











    uint32 dwTotalIndices = 0;
    for (int32 nSubObject = 0; nSubObject < tGeoSet.nSubObjectCount; ++nSubObject)
    {
        SubObject & tSubObject = tGeoSet.ptSubObjects[nSubObject];
        SubObjectPostData & tSubObjectPost = *tSubObject.ptPostprocessed;

        if (tSubObject.nIndexCount > 0)
        {
            PackedIBEntry & tEntry = tSubObjectPost.tIBEntry;
            int32 nCount = tSubObject.nIndexCount;


            {
                PackedIB tPIB;
                tPIB.nIndexCount = 0;  // Inference: folded into the AddTail store {0, -1}
                tPIB.ibid = IBID_INVALID;
                ptPIBList->AddTail_Tracked(tPIB);
            }


            PackedIB *ptPIB = &ptPIBList->GetTail();
            tEntry.dwIndexOffset = ptPIB->nIndexCount;
            ptPIB->nIndexCount += nCount;
            tEntry.ptPIB = ptPIB;
            dwTotalIndices += nCount;  // Inference: dead (the result is unused), no row
        }
        else
        {

            PackedIBEntry &tEntry = tSubObjectPost.tIBEntry;
            SigmaMemoryClear(&tEntry, sizeof(tEntry));
        }
    }

    return dwTotalIndices;
}



static void sPackIBs(
	AppearanceDefinition * pdef)
{
    uint32 dwTotalIndices = 0;
    Structure & tStructure = pdef->tStructure;
    for (int32 nGeoSet = 0; nGeoSet < NUM_GEOSETS; ++nGeoSet)
    {
        StructureGeoSetPostData & tGeoSetPost = tStructure.ptPostprocessed->tGeoSetPost[nGeoSet];
        GeoSet & tGeoSet = tStructure.tGeoSets[nGeoSet];

        if (!tGeoSet.nSubObjectCount)
            continue;



        // Inference: the sum is dead and has no row.
        dwTotalIndices += sPackIB(tGeoSetPost.ptPIBList, tGeoSet);
    }
}
























































































































































































static void sAppearanceDefinitionForceLoad(
	AppearanceDefinition * pdef)
{











    Structure & tStructure = pdef->tStructure;
    BOOL fStructureCanAnimate = !(tStructure.dwFlags & (1 << STRUCTURE_CANNOT_ANIMATE_BIT));
    for (int32 nGeoSet = 0; nGeoSet < NUM_GEOSETS; ++nGeoSet)
    {
        StructureGeoSetPostData & tGeoSetPost = tStructure.ptPostprocessed->tGeoSetPost[nGeoSet];
        GeoSet & tGeoSet = tStructure.tGeoSets[nGeoSet];

        if (!tGeoSet.nSubObjectCount)
            continue;




        PackedVBListAllocProxy(tGeoSetPost.ptPVBList, VBFORMAT_2TEX_L, "");



        PackedIBListAllocProxy(tGeoSetPost.ptPIBList, "");

        PackedVBListAllocProxy(tGeoSetPost.ptPackPVBList, VBFORMAT_2TEX_L_CPOS, "");



































        if (fStructureCanAnimate)
        {




            PackedVBListAllocProxy(tGeoSetPost.ptSkinPVBList, VBFORMAT_SKIN_CPOS, "");




            PackedIBListAllocProxy(tGeoSetPost.ptSkinPIBList, "");
        }





















        for (int32 i = 0; i < tGeoSet.nSubObjectCount; ++i)
        {
            SubObject & tSubObject = tGeoSet.ptSubObjects[i];


            SubObjectPostData & tSubObjectPost = *tSubObject.ptPostprocessed;
            if (tSubObject.nIndexCount > 0)
            {
                if (tSubObjectPost.tIBEntry.ptPIB->ibid != IBID_INVALID)
                {
                    GfxIBSetProxy(tSubObjectPost.tIBEntry.ptPIB->ibid, tSubObject.pwIndexList);
                }
            }
            if (tSubObject.nVertCount > 0)
            {
                if (tSubObjectPost.tVBEntry.ptPVB->vbid != VBID_INVALID)
                {
                    GfxVBSetProxy(tSubObjectPost.tVBEntry.ptPVB->vbid, tSubObject.ptVertList);
                }
            }
            if ((tSubObject.dwFlags & (1 << SUBOBJECT_PACK_GEOMETRY_BIT)) && !(tSubObject.dwFlags & (1 << SUBOBJECT_PACK_SKIN_BIT)))
            {


                GfxVBSetProxy(tSubObjectPost.tPackVBEntry.ptPVB->vbid, tSubObject.pPackVertList);
            }




























            if (fStructureCanAnimate)
            {
                int32 nSkinningInfoCount = tSubObject.serSkinningInfo.dwSize / sizeof(SkinningInfo);
                for (int32 j = 0; j < nSkinningInfoCount; j++)
                {
                    const SkinningInfo & tSkinningInfo = tSubObject.ptSkinningInfo[j];

                    if (tSkinningInfo.tPackIBEntry.ptPIB->ibid != IBID_INVALID)
                    {
                        GfxIBSetProxy(tSkinningInfo.tPackIBEntry.ptPIB->ibid, (uint16 *)tSkinningInfo.pPackIndexList.Get());
                    }
                    if (tSkinningInfo.tPackVBEntry.ptPVB->vbid != VBID_INVALID)
                    {
                        GfxVBSetProxy(tSkinningInfo.tPackVBEntry.ptPVB->vbid, tSkinningInfo.pPackVertList);
                    }
                }
            }
        }
    }
}
































































































int32 AppearanceMaterialIndexFromHash(
	const AppearanceDefinition * pdef,
	uint32 dwMaterialHash)
{
    for (int32 i = 0; i < pdef->nNumMaterials; ++i)
    {
        if (pdef->ptAppearanceMaterials[i].dwMaterialHash == dwMaterialHash)
            return i;
    }

    // Inference: the not-found value is folded into the caller (2191 treats < 0 as not found).
    return -1;
}





BOOL SubObjectAppearanceDoesMasterMaterialOverrideShaderTex(
	ShaderTex eShaderTex)
{
    // Inference: one line (1661, every instruction at column 43); the five enumerators come from the
    // bit mask 0x04800000000c0004 (2, 18, 19, 55, 58); their order is not recoverable.
	return ( eShaderTex == SHADERTEX_ALPHA ) || ( eShaderTex == SHADERTEX_LIGHTMAP ) || ( eShaderTex == SHADERTEX_CLUTTER_MASK ) || ( eShaderTex == SHADERTEX_RIVER_MASK ) || ( eShaderTex == SHADERTEX_PMA_MASK );
}









































































































































#include "../../Common/TagMaps/Tags.h"
// Definitions.cpp:434 and 480. The original passes its build path and line; a macro around this call is likely but not in src.
void * DefinitionMemAllocTracked(GlobalCacheType eType, uint32 dwSize, uint32 dwAlign, BOOL fClear, const char * szFile, uint32 dwLine);
void DefinitionMemFree(GlobalCacheType eType, void * pMem);



static void sReadIndicesData(
	AppearanceDefinition * ptAppearanceDef,
	SNOElement * ptSNOElement)
{
    uint32 dwBaseOffset = ptAppearanceDef->serAppearanceSubObjectsIndices.dwOffset;

    Structure & tStructure = ptAppearanceDef->tStructure;
    for (int32 eGeoSetType = GEOSET_HIGHPOLY; eGeoSetType < NUM_GEOSETS; eGeoSetType++)
    {
        GeoSet & tGeoSet = tStructure.tGeoSets[eGeoSetType];
        for (int32 nSubObject = 0; nSubObject < tGeoSet.nSubObjectCount; nSubObject++)
        {
            SubObject & tSubObject = tGeoSet.ptSubObjects[nSubObject];
            uint32 dwIndexSize = tSubObject.nIndexCount * sizeof(uint16);
            if (!dwIndexSize)
                continue;



            uint16 * pwIndexList = NULL;

            {
                GlobalCachePurgeOverride purgeOverride(GLOBALCACHE_MAIN);
                pwIndexList = (uint16*)DefinitionMemAllocTracked(GLOBALCACHE_SECONDARY, dwIndexSize, 32, FALSE, "D:/BuildServer/2/work/code/branches/2_6_2/X/Client/Definitions/AppearanceDefinition.cpp", __LINE__);
            }





            SNOFileRead(ptSNOElement, dwBaseOffset, dwIndexSize, pwIndexList);
            dwBaseOffset += dwIndexSize;

            tSubObject.pwIndexList = pwIndexList;
        }
    }
}




static void sReadVerticesData(
	AppearanceDefinition * ptAppearanceDef,
	SNOElement * ptSNOElement)
{
    uint32 dwBaseOffset = ptAppearanceDef->serAppearanceSubObjectsVertices.dwOffset;

    Structure & tStructure = ptAppearanceDef->tStructure;
    for (int32 eGeoSetType = GEOSET_HIGHPOLY; eGeoSetType < NUM_GEOSETS; eGeoSetType++)
    {
        GeoSet & tGeoSet = tStructure.tGeoSets[eGeoSetType];
        for (int32 nSubObject = 0; nSubObject < tGeoSet.nSubObjectCount; nSubObject++)
        {
            SubObject & tSubObject = tGeoSet.ptSubObjects[nSubObject];
            uint32 dwVertSize = tSubObject.nVertCount * sizeof(FatVertex);
            if (!dwVertSize)
                continue;



            FatVertex * ptVertList = NULL;

            {
                GlobalCachePurgeOverride purgeOverride(GLOBALCACHE_MAIN);
                ptVertList = (FatVertex*)DefinitionMemAllocTracked(GLOBALCACHE_SECONDARY, dwVertSize, 32, FALSE, "D:/BuildServer/2/work/code/branches/2_6_2/X/Client/Definitions/AppearanceDefinition.cpp", __LINE__);
            }





            SNOFileRead(ptSNOElement, dwBaseOffset, dwVertSize, ptVertList);
            dwBaseOffset += dwVertSize;

            tSubObject.ptVertList = ptVertList;
        }
    }
}




static void sReadPackedGeomData(
	AppearanceDefinition * ptAppearanceDef,
	SNOElement * ptSNOElement)
{
    Structure & tStructure = ptAppearanceDef->tStructure;

    for (int32 eGeoSetType = GEOSET_HIGHPOLY; eGeoSetType < NUM_GEOSETS; eGeoSetType++)
    {
        GeoSet & tGeoSet = tStructure.tGeoSets[eGeoSetType];
        for (int32 nSubObject = 0; nSubObject < tGeoSet.nSubObjectCount; nSubObject++)
        {
            SubObject & tSubObject = tGeoSet.ptSubObjects[nSubObject];

            uint32 dwPackVertSize = tSubObject.serPackVertList.dwSize;
            uint8 * pPackVertList = NULL;
            if (dwPackVertSize)
            {

                {
                    GlobalCachePurgeOverride purgeOverride(GLOBALCACHE_MAIN);
                    pPackVertList = (uint8*)DefinitionMemAllocTracked(GLOBALCACHE_SECONDARY, dwPackVertSize, 32, FALSE, "D:/BuildServer/2/work/code/branches/2_6_2/X/Client/Definitions/AppearanceDefinition.cpp", __LINE__);
                }





                SNOFileRead(ptSNOElement, tSubObject.serPackVertList, pPackVertList);
            }

            tSubObject.pPackVertList = pPackVertList;

            int32 nSkinningInfoCount = tSubObject.serSkinningInfo.dwSize / sizeof(SkinningInfo);
            for (int32 i = 0; i < nSkinningInfoCount; i++)
            {
                SkinningInfo & tSkinningInfo = tSubObject.ptSkinningInfo[i];

                uint32 dwPackSkinVertSize = tSkinningInfo.serPackVertList.dwSize;
                uint8 * pPackSkinVertList = NULL;
                if (dwPackSkinVertSize)
                {

                    {
                        GlobalCachePurgeOverride purgeOverride(GLOBALCACHE_MAIN);
                        pPackSkinVertList = (uint8*)DefinitionMemAllocTracked(GLOBALCACHE_SECONDARY, dwPackSkinVertSize, 32, FALSE, "D:/BuildServer/2/work/code/branches/2_6_2/X/Client/Definitions/AppearanceDefinition.cpp", __LINE__);
                    }





                    SNOFileRead(ptSNOElement, tSkinningInfo.serPackVertList, pPackSkinVertList);
                }

                tSkinningInfo.pPackVertList = pPackSkinVertList;

                uint32 dwPackIndexSize = tSkinningInfo.serPackIndexList.dwSize;
                uint8 * pPackIndexList = NULL;
                if (dwPackIndexSize)
                {

                    {
                        GlobalCachePurgeOverride purgeOverride(GLOBALCACHE_MAIN);
                        pPackIndexList = (uint8 *)DefinitionMemAllocTracked(GLOBALCACHE_SECONDARY, dwPackIndexSize, 32, FALSE, "D:/BuildServer/2/work/code/branches/2_6_2/X/Client/Definitions/AppearanceDefinition.cpp", __LINE__);
                    }





                    SNOFileRead(ptSNOElement, tSkinningInfo.serPackIndexList, pPackIndexList);
                }

                tSkinningInfo.pPackIndexList = pPackIndexList;
            }
        }
    }
}




static void sDisposeVerticesData(
	AppearanceDefinition * ptAppearanceDef)
{
    // fCanAnimate: col 25 of the original row is not explained by this spelling
    Structure & tStructure = ptAppearanceDef->tStructure;
    BOOL fCanAnimate = !(tStructure.dwFlags & (1 << STRUCTURE_CANNOT_ANIMATE_BIT));

    for (int32 eGeoSetType = GEOSET_HIGHPOLY; eGeoSetType < NUM_GEOSETS; eGeoSetType++)
    {
        GeoSet & tGeoSet = tStructure.tGeoSets[eGeoSetType];
        for (int32 nSubObject = 0; nSubObject < tGeoSet.nSubObjectCount; nSubObject++)
        {
            SubObject & tSubObject = tGeoSet.ptSubObjects[nSubObject];

            if (tSubObject.pwIndexList)
            {
                // Inference: the original frees on line +4 two levels deeper and declares lpIndexList
                // on line +8 in a sibling block that left no code; what selects between them is unknown.
                    DefinitionMemFree(GLOBALCACHE_SECONDARY, tSubObject.pwIndexList);


                {
                    void * lpIndexList = tSubObject.pwIndexList;
                }


                tSubObject.pwIndexList = NULL;
            }

            if (tSubObject.ptVertList)
            {
                // Inference: the original frees on line +4 two levels deeper and declares lpVertList
                // on line +8 in a sibling block that left no code; what selects between them is unknown.
                    DefinitionMemFree(GLOBALCACHE_SECONDARY, tSubObject.ptVertList);


                {
                    void * lpVertList = tSubObject.ptVertList;
                }


                tSubObject.ptVertList = NULL;
            }

            if (tSubObject.pPackVertList)
            {
                // Inference: the original frees on line +4 two levels deeper and declares pPackVertList
                // on line +8 in a sibling block that left no code; what selects between them is unknown.
                    DefinitionMemFree(GLOBALCACHE_SECONDARY, tSubObject.pPackVertList);


                {
                    void * pPackVertList = tSubObject.pPackVertList;
                }


                tSubObject.pPackVertList = NULL;
            }

            if (fCanAnimate)
            {
                int32 nSkinningInfoCount = tSubObject.serSkinningInfo.dwSize / sizeof(SkinningInfo);
                for (int32 i = 0; i < nSkinningInfoCount; i++)
                {
                    SkinningInfo & tSkinningInfo = tSubObject.ptSkinningInfo[i];

                    if (tSkinningInfo.pPackVertList)
                    {
                        // Inference: the original frees on line +4 two levels deeper and declares pPackVertList
                        // on line +8 in a sibling block that left no code; what selects between them is unknown.
                            DefinitionMemFree(GLOBALCACHE_SECONDARY, tSkinningInfo.pPackVertList);


                        {
                            void * pPackVertList = tSkinningInfo.pPackVertList;
                        }


                        tSkinningInfo.pPackVertList = NULL;
                    }

                    if (tSkinningInfo.pPackIndexList)
                    {
                        // Inference: the original frees on line +4 two levels deeper and declares pPackIndexList
                        // on line +8 in a sibling block that left no code; what selects between them is unknown.
                            DefinitionMemFree(GLOBALCACHE_SECONDARY, tSkinningInfo.pPackIndexList);


                        {
                            void * pPackIndexList = tSkinningInfo.pPackIndexList;
                        }


                        tSkinningInfo.pPackIndexList = NULL;
                    }
                }
            }
        }
    }
}





void AppearanceGroupDef::Read(
	const RequiredDefinitionHeader & tHeader,
	void *& lpDefinition,
	SNOElement * ptSNOElement,
	BOOL fContiguousBuffer,
	uint32 dwContiguousSize,
	XMemoryAllocatorInterface * ptMemoryInterface) const
{
    GroupDefinition::Read(tHeader, lpDefinition, ptSNOElement, fContiguousBuffer, dwContiguousSize, ptMemoryInterface);

    AppearanceDefinition* ptDefAppearance = (AppearanceDefinition*)lpDefinition;







    if (ptDefAppearance->serAppearanceSubObjectsIndices.dwSize)
    {
        sReadIndicesData(ptDefAppearance, ptSNOElement);
    }
    if (ptDefAppearance->serAppearanceSubObjectsVertices.dwSize)
    {
        sReadVerticesData(ptDefAppearance, ptSNOElement);
    }


    sReadPackedGeomData(ptDefAppearance, ptSNOElement);

}
// GfxIB.cpp:929 and GfxVB.cpp:1047
void PackedIBListFree(PackedIBList * ptPIBList);
void PackedVBListFree(PackedVBList * ptPVBList);
// Forward declarations; the definitions are at 3919, 3861, 3831 and 3768.
void SubObjectDispose(SubObject & tSubObject, BOOL bFreePostprocessDataOnly);
static void sFreePackedIBList(PackedIBList * listPackedIb);



static void sPostProcessForRaycast(
	void * lpDefinition)
{
    AppearanceDefinition * pdef = (AppearanceDefinition *)lpDefinition;

    Structure & tStructure = pdef->tStructure;







    tStructure.ptPostprocessed = sAllocStructurePostData();
    {
        StructurePostData * ptStructurePost = tStructure.ptPostprocessed;
        // Inference: bCollides (2133) has no location and no code; its initialiser is not recoverable.
        BOOL bCollides = (tStructure.dwFlags != 0);



        // The stp at 2138:52 carries both stores; ptAppearance has no row of its own.
        ptStructurePost->tStructureOctree.ptOctree	= &tStructure.tOctreeVisualMesh;
        ptStructurePost->tStructureOctree.ptAppearance	= pdef;
    }


    BOOL fComputeBonemapping = TRUE;





    for (int32 nGeoSet = 0; nGeoSet < NUM_GEOSETS; ++nGeoSet)
    {
        GeoSet & tGeoSet = tStructure.tGeoSets[nGeoSet];



        for (int32 i = 0; i < tGeoSet.nSubObjectCount; ++i)
        {
            SubObject & tSubObject = tGeoSet.ptSubObjects[i];



            tSubObject.ptPostprocessed = sAllocSubObjectPostData();
            SubObjectPostData * ptPostprocessed = tSubObject.ptPostprocessed;












            ptPostprocessed->flAlpha = 1.0f;




            // Inference: the original zeroes 0x24-0x40 in one merged run (vector.h _init rows), so nTotalTriArea is set next to the vector.
            ptPostprocessed->nTotalTriArea = 0;
            new (&ptPostprocessed->tTriAreas) blz::vector<int>;


            int32 nMaterialIndex = AppearanceMaterialIndexFromHash(pdef, tSubObject.dwMaterialHash);





            if (nMaterialIndex < 0 || nMaterialIndex >= pdef->nNumMaterials)
            {
                nMaterialIndex = 0;
            }







            ptPostprocessed->ptMaterial = &pdef->ptAppearanceMaterials[nMaterialIndex];
        }
    }

    for (int32 nMaterialIndex = 0; nMaterialIndex < pdef->nNumMaterials; nMaterialIndex++)
    {
        AppearanceMaterial & tMaterial = pdef->ptAppearanceMaterials[nMaterialIndex];

        for (int32 nLookIndex = 0; nLookIndex < pdef->nNumLooks; nLookIndex++)
        {
            SubObjectAppearance & tAppearance = tMaterial.ptSOAs[nLookIndex];




            tAppearance.tCompositeUberMaterial.dwMatTexSortHash = MaterialTextureBuildHash(tAppearance.tCompositeUberMaterial);

            if (tAppearance.snoMasterMaterial != SNO_NONE)
            {
                ScopedMaterialDefinitionLockR pdefMasterMaterial(tAppearance.snoMasterMaterial);

                tAppearance.tCompositeUberMaterial.snoShaderMap		= pdefMasterMaterial->tUberMaterial.snoShaderMap;
                tAppearance.tCompositeUberMaterial.mat				= pdefMasterMaterial->tUberMaterial.mat;
                tAppearance.tCompositeUberMaterial.serMatTexList	= pdefMasterMaterial->tUberMaterial.serMatTexList;
                tAppearance.tCompositeUberMaterial.ptMatTexList = NULL;
                tAppearance.hTagMapMasterMaterial = NULL;

                int nCompositeMaterialSize = tAppearance.tCompositeUberMaterial.serMatTexList.dwSize;
                if (nCompositeMaterialSize > 0)
                {




                    int32 nTexAnimCompTotalParamSize = 0;

                    int32 nMatTexCount = pdefMasterMaterial->tUberMaterial.serMatTexList.dwSize / sizeof(MaterialTextureEntry);
                    BOOL afMaterialTexToSkip[256];

                    for (int32 i = 0; i != nMatTexCount; ++i)
                    {
                        nTexAnimCompTotalParamSize += pdefMasterMaterial->tUberMaterial.ptMatTexList[i].tMatTex.serTexAnimCompParams.dwSize;
                        afMaterialTexToSkip[i] = FALSE;
                    }

                    // The shift sits on 2248:39, the opening parenthesis of the initialiser.
                    int32 nTextures = (nCompositeMaterialSize / sizeof(MaterialTextureEntry));
                    int32 nOverrideTextures = MaterialGetTextureCount(tAppearance.tBaseUberMaterial);
                    int32 nAppendCount = 0;
                    for (int32 i = 0; i != nOverrideTextures; ++i)
                    {
                        ShaderTex eShaderTex = tAppearance.tBaseUberMaterial.ptMatTexList[i].eShaderTex;
                        if (SubObjectAppearanceDoesMasterMaterialOverrideShaderTex(eShaderTex))
                        {

                            BOOL bNeedAppend = TRUE;
                            for (int32 j = 0; j != nTextures; ++j)
                            {
                                if (pdefMasterMaterial->tUberMaterial.ptMatTexList[j].eShaderTex == eShaderTex)
                                {
                                    bNeedAppend = FALSE;

                                    nTexAnimCompTotalParamSize -= pdefMasterMaterial->tUberMaterial.ptMatTexList[j].tMatTex.serTexAnimCompParams.dwSize;
                                    afMaterialTexToSkip[j] = TRUE;
                                    break;
                                }
                            }



                            if (bNeedAppend)
                            {
                                nAppendCount++;
                            }
                        }
                    }

                    void *pCurrentBuf = SigmaMemoryNew(nCompositeMaterialSize + nAppendCount * sizeof(MaterialTextureEntry) + nTexAnimCompTotalParamSize, 4, NULL, FALSE);
                    tAppearance.tCompositeUberMaterial.ptMatTexList = (MaterialTextureEntry *)pCurrentBuf;
                    tAppearance.tCompositeUberMaterial.serMatTexList.dwSize = nCompositeMaterialSize + nAppendCount * sizeof(MaterialTextureEntry);
                    SigmaMemoryCopy(pCurrentBuf, pdefMasterMaterial->tUberMaterial.ptMatTexList, nCompositeMaterialSize);



                    char *ptCurrentBuf = (char*)pCurrentBuf + nCompositeMaterialSize + nAppendCount * sizeof(MaterialTextureEntry);
                    for (int32 i = 0; i != nMatTexCount; ++i)
                    {
                        if (!afMaterialTexToSkip[i])
                        {
                            MaterialTexture & tMatTex = tAppearance.tCompositeUberMaterial.ptMatTexList[i].tMatTex;
                            uint32 dwSize = tMatTex.serTexAnimCompParams.dwSize;
                            if (dwSize > 0)
                            {
                                SigmaMemoryCopy(ptCurrentBuf, tMatTex.pTexAnimCompParams, dwSize);
                                tMatTex.pTexAnimCompParams = ptCurrentBuf;
                                ptCurrentBuf += dwSize;
                            }
                        }
                    }




                    for (int32 i = 0; i != nOverrideTextures; ++i)
                    {
                        ShaderTex eShaderTex = tAppearance.tBaseUberMaterial.ptMatTexList[i].eShaderTex;
                        if (SubObjectAppearanceDoesMasterMaterialOverrideShaderTex(eShaderTex))
                        {

                            BOOL bNeedAppend = TRUE;
                            for (int32 j = 0; j != nTextures; ++j)
                            {
                                if (tAppearance.tCompositeUberMaterial.ptMatTexList[j].eShaderTex == eShaderTex)
                                {
                                    tAppearance.tCompositeUberMaterial.ptMatTexList[j].tMatTex =
                                        tAppearance.tBaseUberMaterial.ptMatTexList[i].tMatTex;
                                    bNeedAppend = FALSE;
                                    break;
                                }
                            }
                            if (bNeedAppend)
                            {

                                tAppearance.tCompositeUberMaterial.ptMatTexList[nTextures++] =
                                    tAppearance.tBaseUberMaterial.ptMatTexList[i];
                            }
                        }
                    }
                }
                MaterialListSort(tAppearance.tCompositeUberMaterial);
                tAppearance.tCompositeUberMaterial.dwMatTexSortHash = MaterialTextureBuildHash(tAppearance.tCompositeUberMaterial);

                if (pdefMasterMaterial->hTagMap)
                {

                    tAppearance.hTagMapMasterMaterial = TagMapRef(pdefMasterMaterial->hTagMap);
                }



            }
            else
            {

                tAppearance.tCompositeUberMaterial.snoShaderMap					= tAppearance.tBaseUberMaterial.snoShaderMap;
                tAppearance.tCompositeUberMaterial.mat							= tAppearance.tBaseUberMaterial.mat;
                tAppearance.tCompositeUberMaterial.serMatTexList				= tAppearance.tBaseUberMaterial.serMatTexList;
                tAppearance.tCompositeUberMaterial.dwMatTexSortHash = tAppearance.tBaseUberMaterial.dwMatTexSortHash;




















                tAppearance.tCompositeUberMaterial.ptMatTexList      = tAppearance.tBaseUberMaterial.ptMatTexList;
                tAppearance.hTagMapMasterMaterial                       = tAppearance.hTagMap;
            }
        }
    }
}




// NON_MATCHING: 5036 vs 5048 bytes; only the inlined sPostProcessForRaycast differs, in register allocation: the original keeps 0xf8 in x20 across the master-material LockR call and rematerialises the ShaderTex mask 0x04800000000c0004 in x16/x17 after each call, ours keeps the mask in x22 (3 instructions fewer). Tried: != loops with prefix ++ (fixed the loop guards), the compare order in SubObjectAppearanceDoesMasterMaterialOverrideShaderTex (columns only).
void AppearanceGroupDef::Postprocess(
	void * lpDefinition)
{

	GroupDefinition::Postprocess(lpDefinition);

	AppearanceDefinition * pdef = (AppearanceDefinition *)lpDefinition;











	// The original calls the base Postprocess a second time (2400:19).
	GroupDefinition::Postprocess(lpDefinition);


	pdef->tStructure.ptPostprocessed = NULL;
	sPostProcessForRaycast(lpDefinition);

	Structure & tStructure = pdef->tStructure;















































































	sPackVBs(pdef);
	sPackIBs(pdef);








	sAppearanceDefinitionForceLoad(pdef);













    CGameGlobals *ptCGameGlobals = CGameGlobalsTryToGet();
    if (ptCGameGlobals && (ptCGameGlobals->dwFlags & (1 << CGAMEGLOBALS_GAME_INITIALIZED_BIT)))
    {
        int32 nLookIndex = 0;
        int32 nNumLooks = pdef->nNumLooks;
        if (nNumLooks > 1)
        {
            nLookIndex = -1;
            uint32 dwLookKey = StringHashI("A");

            for (int32 nIndex = 0; nIndex < nNumLooks; nIndex++)
            {
                uint32 dwCurrentLookKey = pdef->ptAppearanceLooks[nIndex].dwLookHash;
                if (dwCurrentLookKey == dwLookKey)
                {
                    nLookIndex = nIndex;
                    break;
                }



            }
        }








        // Inference: the -1 test is skipped when nNumLooks <= 1, so nLookIndex starts at 0 and only the search sets -1.
        if (nLookIndex != -1)
        {
            const GeoSetType eGeoSetType = GEOSET_HIGHPOLY;
            const GeoSet & tGeoSet = pdef->tStructure.tGeoSets[eGeoSetType];

            for (int32 nSubObjectIndex = 0; nSubObjectIndex < tGeoSet.nSubObjectCount; ++nSubObjectIndex)
            {
                SubObject & tSubObject = tGeoSet.ptSubObjects[nSubObjectIndex];
                const SubObjectAppearance &tSOA = tSubObject.ptPostprocessed->ptMaterial->ptSOAs[0];
                int32 nCount = tSOA.tCompositeUberMaterial.serMatTexList.dwSize / sizeof(MaterialTextureEntry);
                MaterialTextureEntry * ptMatTexList = tSOA.tCompositeUberMaterial.ptMatTexList;
                for (int32 i = 0; i < nCount; ++i)
                {
                    SNO snoTex = ptMatTexList[i].tMatTex.snoTex;
                    if (snoTex != SNO_NONE)
                    {
                        Prefetch::AddToQueue(SNOName(TEXTURE_SNO, snoTex), 1);
                    }
                }
            }
        }
    }


}
static void sFreePackedVBList(PackedVBList * listPackedVb);
static void sFreeStructurePostData(StructurePostData * ptStructurePostData);

void sDeletePostProcessData(
	AppearanceDefinition * pdefAppearance)
{


    for (int32 eGeoSetType = GEOSET_HIGHPOLY; eGeoSetType < NUM_GEOSETS; eGeoSetType++)
    {
        GeoSet & tGeoSet = pdefAppearance->tStructure.tGeoSets[eGeoSetType];
        for (int32 i = 0; i < tGeoSet.nSubObjectCount; ++i)
        {
            SubObject & tSubObject = tGeoSet.ptSubObjects[i];
            SubObjectDispose(tSubObject, TRUE);
        }
    }

    if (pdefAppearance->tStructure.ptPostprocessed)
    {
        for (int32 nGeoSet = 0; nGeoSet < NUM_GEOSETS; ++nGeoSet)
        {
            StructureGeoSetPostData & tGeoSetPost = pdefAppearance->tStructure.ptPostprocessed->tGeoSetPost[nGeoSet];

            PackedIBListFree(tGeoSetPost.ptPIBList);
            PackedVBListFree(tGeoSetPost.ptPVBList);





            PackedVBListFree(tGeoSetPost.ptPackPVBList);


            PackedVBListFree(tGeoSetPost.ptSkinPVBList);



            PackedIBListFree(tGeoSetPost.ptSkinPIBList);

            sFreePackedIBList(tGeoSetPost.ptPIBList);

            sFreePackedVBList(tGeoSetPost.ptPVBList);






            sFreePackedVBList(tGeoSetPost.ptPackPVBList);


            sFreePackedVBList(tGeoSetPost.ptSkinPVBList);



            sFreePackedIBList(tGeoSetPost.ptSkinPIBList);
        }

        sFreeStructurePostData(pdefAppearance->tStructure.ptPostprocessed);
        pdefAppearance->tStructure.ptPostprocessed = NULL;
    }
}




void AppearanceGroupDef::Delete(
	void * lpDefinition) const
{
	AppearanceDefinition * ptDefinition = (AppearanceDefinition *)lpDefinition;






























    sDeletePostProcessData(ptDefinition);


	sDisposeVerticesData(ptDefinition);



	for (int32 nMaterialIndex = 0; nMaterialIndex < ptDefinition->nNumMaterials; nMaterialIndex++)
	{
		AppearanceMaterial & tMaterial = ptDefinition->ptAppearanceMaterials[nMaterialIndex];
		for (int32 nLookIndex = 0; nLookIndex < ptDefinition->nNumLooks; nLookIndex++)
		{
			SubObjectAppearance & tAppearance = tMaterial.ptSOAs[nLookIndex];
			if (tAppearance.tCompositeUberMaterial.ptMatTexList != tAppearance.tBaseUberMaterial.ptMatTexList)
			{
				SigmaMemoryFree(tAppearance.tCompositeUberMaterial.ptMatTexList, NULL);
			}


			tAppearance.tCompositeUberMaterial.ptMatTexList = NULL;

			if (tAppearance.hTagMapMasterMaterial != tAppearance.hTagMap)
			{
				TagMapDispose(tAppearance.hTagMapMasterMaterial);
			}
			else
			{
				tAppearance.hTagMapMasterMaterial = NULL;
			}
		}
	}









	GroupDefinition::Delete(lpDefinition);
}




BOOL AppearanceGroupDef::CacheManagerCanPurgeElement(
	const DefDataElement & tDefElement,
	BOOL fIgnoreDoNotPurgeBit) const
{
	if (!GroupDefinition::CacheManagerCanPurgeElement(tDefElement, fIgnoreDoNotPurgeBit))
	{
		return FALSE;
	}

	return TRUE;
}

























































































































































































































































































































































































































































































































































































































































































































































































































































































































BOOL AppearanceGroupDef::Write(
	void * lpDefinition,
	FileReference & tRef) const
{


	XASSERT(FALSE);
	return FALSE;
}






void * DominoAlloc(int32 size, int32 align, const char * pFileName, int32 lineNumber, const char * pFunctionName)
{
    return SigmaMemoryNew(size, align, NULL, FALSE);
}




void DominoFree(void * p)
{
    SigmaMemoryFree(p, NULL);
}




void DominoLog(const char * szText)
{
}





void DominoWarn(const char * szText)
{
}





void DominoAssert(const char * szText, const char * szFileName, int32 line, void * pUserA, void * pUserB)
{

	if (pUserA)
	{
		RigidBody * pRigidBody = (RigidBody *)pUserA;
		dmBodyId id = pRigidBody->m_bodyId;
		dm_vec4 p = dmBody_GetPosition(id);
		dm_vec4 q = dmBody_GetQuaternion(id);
		dm_vec4 v = dmBody_GetLinearVelocity(id);
		dm_vec4 w = dmBody_GetAngularVelocity(id);
	}





	if (pUserB)
	{
		RigidBody * pRigidBody = (RigidBody *)pUserB;
		dmBodyId id = pRigidBody->m_bodyId;
		dm_vec4 p = dmBody_GetPosition(id);
		dm_vec4 q = dmBody_GetQuaternion(id);
		dm_vec4 v = dmBody_GetLinearVelocity(id);
		dm_vec4 w = dmBody_GetAngularVelocity(id);
	}





	DisplayInternalError(szText, szFileName, line, ERROR_INTERNAL_FATAL);
}



void AppearanceInitializeForApp()

{
	AppearanceGlobals *& ptAppearanceGlobals = sgtAppGlobals.ptAppearanceGlobals;

	ptAppearanceGlobals = (AppearanceGlobals*)SigmaMemoryNew(sizeof(AppearanceGlobals), 4, NULL, TRUE);

	ptAppearanceGlobals->allocPackedVbList.Initialize(10, NULL);
	ptAppearanceGlobals->allocPackedVbList.SetBlockCount_Tracked(1024);
	ptAppearanceGlobals->allocPackedIbList.Initialize(10, NULL);
	ptAppearanceGlobals->allocPackedIbList.SetBlockCount_Tracked(1024);
	ptAppearanceGlobals->allocStructurePostData.Initialize(sizeof(StructurePostData), 1024, NULL);
	ptAppearanceGlobals->allocSubObjectPostData.Initialize(sizeof(SubObjectPostData), 4096, NULL);


    ptAppearanceGlobals->allocStructurePostData.SetGrow(XALLOC_GROW_LINEAR, 128);
    ptAppearanceGlobals->allocStructurePostData.SetShrinkable(TRUE);
    ptAppearanceGlobals->allocSubObjectPostData.SetGrow(XALLOC_GROW_LINEAR, 512);
    ptAppearanceGlobals->allocSubObjectPostData.SetShrinkable(TRUE);



    ptAppearanceGlobals->tAppearanceMutex.Create();


	dmSetAllocHook(DominoAlloc, DominoFree);
	dmSetAssertHook(DominoAssert);
	dmSetWarningHook(DominoWarn);
	dmSetLogHook(DominoLog);
}




void AppearanceDisposeForApp()

{
	AppearanceGlobals *& ptAppearanceGlobals = sgtAppGlobals.ptAppearanceGlobals;

	ptAppearanceGlobals->allocPackedVbList.Dispose();
	ptAppearanceGlobals->allocPackedIbList.Dispose();
	ptAppearanceGlobals->allocStructurePostData.Dispose();
	ptAppearanceGlobals->allocSubObjectPostData.Dispose();


    ptAppearanceGlobals->tAppearanceMutex.Destroy();


	XFREE(ptAppearanceGlobals, NULL);
}




static StructurePostData * sAllocStructurePostData()
{


	Blizzard::Lock::ScopedMutex scopedMutex(sgtAppGlobals.ptAppearanceGlobals->tAppearanceMutex);


	StructurePostData * ptPostData = (StructurePostData *)sgtAppGlobals.ptAppearanceGlobals->allocStructurePostData.Alloc_Tracked();

	SigmaMemoryClear(ptPostData, sizeof(StructurePostData));

	return ptPostData;
}




static void sFreeStructurePostData(
	StructurePostData * ptStructurePostData)
{
	if ( ptStructurePostData )
	{

		Blizzard::Lock::ScopedMutex scopedMutex(sgtAppGlobals.ptAppearanceGlobals->tAppearanceMutex);


    sgtAppGlobals.ptAppearanceGlobals->allocStructurePostData.Free(ptStructurePostData);
	}
}




static SubObjectPostData * sAllocSubObjectPostData()
{


	Blizzard::Lock::ScopedMutex scopedMutex(sgtAppGlobals.ptAppearanceGlobals->tAppearanceMutex);


	SubObjectPostData * ptPostData = (SubObjectPostData *)sgtAppGlobals.ptAppearanceGlobals->allocSubObjectPostData.Alloc_Tracked();

	SigmaMemoryClear(ptPostData, sizeof(SubObjectPostData));

	return ptPostData;
}




static void sFreeSubObjectPostData(
	SubObjectPostData * ptSubObjectPostData)
{
	if ( ptSubObjectPostData )
	{

		Blizzard::Lock::ScopedMutex scopedMutex(sgtAppGlobals.ptAppearanceGlobals->tAppearanceMutex);


    sgtAppGlobals.ptAppearanceGlobals->allocSubObjectPostData.Free(ptSubObjectPostData);
	}
}




static PackedVBList * sAllocPackedVBList()
{


	Blizzard::Lock::ScopedMutex scopedMutex(sgtAppGlobals.ptAppearanceGlobals->tAppearanceMutex);


	PackedVBList * listPackedVb = sgtAppGlobals.ptAppearanceGlobals->allocPackedVbList.Alloc_Tracked();
	return listPackedVb;
}




static void sFreePackedVBList(
	PackedVBList * listPackedVb)
{
	if ( listPackedVb )
	{

		Blizzard::Lock::ScopedMutex scopedMutex(sgtAppGlobals.ptAppearanceGlobals->tAppearanceMutex);


    sgtAppGlobals.ptAppearanceGlobals->allocPackedVbList.Free(listPackedVb);
	}
}




static PackedIBList * sAllocPackedIBList()
{


	Blizzard::Lock::ScopedMutex scopedMutex(sgtAppGlobals.ptAppearanceGlobals->tAppearanceMutex);


	PackedIBList * listPackedIb = sgtAppGlobals.ptAppearanceGlobals->allocPackedIbList.Alloc_Tracked();
	return listPackedIb;
}




static void sFreePackedIBList(
	PackedIBList * listPackedIb)
{
	if ( listPackedIb )
	{

		Blizzard::Lock::ScopedMutex scopedMutex(sgtAppGlobals.ptAppearanceGlobals->tAppearanceMutex);


    sgtAppGlobals.ptAppearanceGlobals->allocPackedIbList.Free(listPackedIb);
	}
}
























void SubObjectPostprocessDispose(
	SubObject & tSubObject)
{
	SubObjectPostprocessDisposeFaceAdjacency(tSubObject);

	if (tSubObject.ptPostprocessed)
	{
		SubObjectPostData * ptPostprocessed = tSubObject.ptPostprocessed;

		ptPostprocessed->tTriAreas.~vector();




		sFreeSubObjectPostData(tSubObject.ptPostprocessed);
		tSubObject.ptPostprocessed = NULL;
	}
}




void SubObjectDispose(
	SubObject & tSubObject,
	BOOL bFreePostprocessDataOnly)
{
	if ( !bFreePostprocessDataOnly )
	{
		// Lines 3925-3946 (a loop over i at 3932) free the subobject's own data. The only out-of-line
		// copy was removed by the linker and the inlined callers pass TRUE, so that code is not recovered.
	}






















	SubObjectPostprocessDispose(tSubObject);
}




































































































































































































































































































SubObjectAppearance & AppearanceGetSubObjectAppearance(const AppearanceDefinition * ptDefinition, int32 nSubObjectLook, const int32 nSubObject, const GeoSetType eGeoSetType) {
    const Structure & tStructure = ptDefinition->tStructure;
    const GeoSet & tGeoSet = tStructure.tGeoSets[eGeoSetType];
    SubObjectPostData * ptPostprocessed = tGeoSet.ptSubObjects[nSubObject].ptPostprocessed;
    AppearanceMaterial & tMaterial = *ptPostprocessed->ptMaterial;
    return tMaterial.ptSOAs[nSubObjectLook];
}












































































































































































































































































































































































































int32 AppearanceSubObjectFromLookKey(
	const SNO snoAppearance,
	const uint32 dwLookKey)
{
	int32 nSubObjectLook = 0;
	ScopedAppearanceDefinitionLockR pdefAppearance(snoAppearance);
	for (int32 nIndex = 0; nIndex < pdefAppearance->nNumLooks; nIndex++)
	{
		// 8 spaces: the load's column (77) is the member name
        uint32 dwCurrentLookKey = pdefAppearance->ptAppearanceLooks[nIndex].dwLookHash;
		if (dwCurrentLookKey == dwLookKey)
		{
			nSubObjectLook = nIndex;
			break;
		}
	}


	return nSubObjectLook;
}






int32 AppearanceSubObjectFromHash(
	const SNO snoAppearance,
	uint32 dwSubObjectHash)
{

    if (dwSubObjectHash == 0)
    {
        return -1;
    }

	ScopedAppearanceDefinitionLockR pdefAppearance(snoAppearance);
	const GeoSet & tGeoSet = pdefAppearance->tStructure.tGeoSets[GEOSET_HIGHPOLY];
    for (int32 i = 0; i < tGeoSet.nSubObjectCount; ++i)
    {
        SubObject & tSubObject = tGeoSet.ptSubObjects[i];

        if (tSubObject.dwSubObjectHash == dwSubObjectHash)
        {
            return i;
        }
    }

    return -1;
}


































void AppearanceGetNakedLook(
	const SNO snoAppearance,
	ComponentStyle & eCompStyle,
	int32 & nSubObjectLook)
{
	eCompStyle = COMPONENTSTYLE_NONE;
	nSubObjectLook = AppearanceSubObjectFromLookKey(snoAppearance, NameToSymbol("A", gComponentLooks));
}















void BarycentricCoordsToTextureUV(
	const SubObject & tSubObject,
	const int32 nSegmentIndex,
	const int32 nIndexListIndex,
	const int32 nUVSetIndex,
	const float flUTriangle,
	const float flVTriangle,
	float & flU,
	float & flV)
{
	float flWTriangle = 1.0f - flUTriangle - flVTriangle;

	uint16 wVertex0, wVertex1, wVertex2;

    SubObjectGetSegmentTriangle(tSubObject, nSegmentIndex, nIndexListIndex, wVertex0, wVertex1, wVertex2);

	Vector2D uvUV0, uvUV1, uvUV2;
    SubObjectGetSegmentVertexUV(tSubObject, nSegmentIndex, wVertex0, nUVSetIndex, uvUV0);
    SubObjectGetSegmentVertexUV(tSubObject, nSegmentIndex, wVertex1, nUVSetIndex, uvUV1);
    SubObjectGetSegmentVertexUV(tSubObject, nSegmentIndex, wVertex2, nUVSetIndex, uvUV2);

    flU = (uvUV1.x * flUTriangle) + (uvUV2.x * flVTriangle) + (uvUV0.x * flWTriangle);
    flV = (uvUV1.y * flUTriangle) + (uvUV2.y * flVTriangle) + (uvUV0.y * flWTriangle);
}




void BarycentricCoordsToTextureUV(
	const SNO snoAppearance,
	const GeoSetType eGeoSetType,
	const int32 nSubObjectIndex,
	const int32 nSegmentIndex,
	const int32 nIndexListIndex,
	const int32 nUVSetIndex,
	const float flUTriangle,
	const float flVTriangle,
	float & flU,
	float & flV)
{
	ScopedAppearanceDefinitionLockR pdefAppearance(snoAppearance);
	ScopedAppearanceBufferContentRetrieval tAppearanceBufferContentRetrieval(pdefAppearance);

	const GeoSet & tGeoSet = pdefAppearance->tStructure.tGeoSets[eGeoSetType];



	SubObject &tSubObject = tGeoSet.ptSubObjects[nSubObjectIndex];

	BarycentricCoordsToTextureUV(tSubObject, nSegmentIndex, nIndexListIndex, nUVSetIndex, flUTriangle, flVTriangle, flU, flV);
}























































void AppearanceEnableBufferContentRetrieval(const AppearanceDefinition * pdefAppearance)
{
}

















void AppearanceDisableBufferContentRetrieval(const AppearanceDefinition * pdefAppearance)
{
}
















































































































































































































































































































































































































































































































































































































































































uint32 AppearanceGroupDef::GetContiguousReadSize(SNOElement * ptSNOElement)

{
    AppearanceDefinition tAppearanceDef;
    ReadDefinitionHeader(ptSNOElement, &tAppearanceDef);





    uint32 dwContiguous = tAppearanceDef.serAppearanceSubObjectsIndices.dwOffset;

    return dwContiguous;
}
