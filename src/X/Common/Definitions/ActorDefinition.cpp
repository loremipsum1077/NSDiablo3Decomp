// Original file: X/Common/Definitions/ActorDefinition.cpp

#include "ActorDefinition.h"

// For cgplaceNull, whose initializer the original runs in this file's static initializer.
#include "../WorldPlace.h"
#include "../../Client/Gfx/GfxVB.h"
#include "../D3TypeDescriptor/D3TypeDescriptor.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/Random.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptorField.h"

// Defined in X/Sigma/Files/XFilesCommon.cpp:20; no adopted header declares it yet.
extern const XTypeDescriptor * DT_SERIALIZEDATA;

// Defined in X/Common/D3TypeDescriptor/D3DescribedType.cpp; no adopted header declares it yet.
extern const XTypeDescriptor * DT_SNO;

// Defined in X/Common/SNO/SymbolicValue.cpp:5221; no adopted header declares it yet.
extern const SymbolicEntry gActorTypes[13];

// Defined in X/Common/TagMaps/Tags.cpp:607, :1040, :1084, :1107 and :1252; no adopted header
// declares them yet.
BOOL TagMapTagExists(const CTagMap * pTagMap, uint32 dwTag);
int32 TagMapGetInt(const CTagMap * pTagMap, uint32 dwTag);
uint32 TagMapGetDWORD(const CTagMap * pTagMap, uint32 dwTag);
float TagMapGetFloat(const CTagMap * pTagMap, uint32 dwTag);
SNO TagMapGetSNO(const CTagMap * pTagMap, uint32 dwTag);

// Defined in X/Common/Definitions/AnimSetDefinition.cpp:106 and :182; no adopted header declares
// them yet.
SNO AnimSetGetAnim(SNO snoAnimSet, const WeaponClass eWeaponClass, const uint32 dwAnimTagId);
void AnimSetGetSpecificAnimSet(const SNO snoAnimSet, const WeaponClass eWeaponClass, const uint32 dwTag, SNO * psnoAnimSet, uint32 * pdwAnimSetTag);

// Defined in X/Client/ImageCollection.cpp:903; no adopted header declares it yet.
enum {
    IMAGE_COLLECTION_UI = 2,
};
typedef int32 ImageCollectionType;
BOOL ImageCollectionGetImageInfo(const ImageCollectionType eCollection, const ImageHandle hImage, ImageTextureFrame & tImageTexFrame);

enum {
    ACTOR_XML_VERSION = 41,
    ACTOR_COMMON_SNO_VERSION = 281,
    ACTOR_PC_SNO_VERSION = 282,
    ACTOR_CONSOLE_SNO_VERSION = 287,
    ACTOR_AXE_SNO_VERSION = 290,
    ACTOR_XBOXONE_SNO_VERSION = 288,
    ACTOR_PS4_SNO_VERSION = 288,
    ACTOR_NX64_SNO_VERSION = 287,
    ACTOR_COLLISION_FLAGS_VERSION = 1,
};

const float BOUNDING_SPHERE_FUDGE_FACTOR = 2.0f;  // line 59

// _GLOBAL__sub_I_ActorDefinition.cpp: the translation unit also initializes the four RGBAColor constants
// of X/Client/Gfx/GfxVB.h:351-354 (included above). The field names are "" in the binary.
static const Vector3D scgwpDefaultLocationSpawn(0.0f, 0.0f, 0.0f);  // line 61
static const Vector3D scgwpDefaultLocationPowerSrc(0.0f, 0.0f, 4.0f);  // line 62

static const float cflDefaultExplosionFactor = 1.0f;  // line 64
static const float cflDefaultWindFactor = 1.0f;  // line 65
static const float cflDefaultPartialRagdollResponsiveness = 0.5f;  // line 66

static const TypeDescriptorField ActorCollisionFlags_Fields[5] = {  // line 82
    TypeDescriptorField("", DT_INT, offsetof(ActorCollisionFlags, uStaticOneRequiredFlags), NULL, 0x80001),
    TypeDescriptorField("", DT_INT, offsetof(ActorCollisionFlags, uStaticPreventFlags), NULL, 0x80001),
    TypeDescriptorField("", DT_INT, offsetof(ActorCollisionFlags, uDynamicFlagsNoOverlap), NULL, 0x80001),
    TypeDescriptorField("", DT_INT, offsetof(ActorCollisionFlags, uDynamicMyFlags), NULL, 0x80001),
    TypeDescriptorField(NULL, DT_NULL, sizeof(ActorCollisionFlags), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_ActorCollisionFlags("ActorCollisionFlags", ACTOR_COLLISION_FLAGS_VERSION, ActorCollisionFlags_Fields, 5, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 87
const XTypeDescriptor * DT_ACTOR_COLLISION_FLAGS = &TypeDescriptor_ActorCollisionFlags;

static const TypeDescriptorField ActorCollisionData_Fields[8] = {  // line 89
    TypeDescriptorField("", DT_ACTOR_COLLISION_FLAGS, offsetof(ActorCollisionData, tCollFlags), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ActorCollisionData, eCollisionShape), (const void *)COLLISION_SHAPE_CYLINDER, 1),
    TypeDescriptorField("", DT_AXIALCYLINDER, offsetof(ActorCollisionData, wcylCollision), NULL, 1),
    TypeDescriptorField("", DT_AABB, offsetof(ActorCollisionData, aabbCollision), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(ActorCollisionData, flMovingRadiusScalar), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(ActorCollisionData, flCylinderRadiusConsole), NULL, 0x700001),
    TypeDescriptorField("", DT_FLOAT, offsetof(ActorCollisionData, flPadding), NULL, 0x700000),
    TypeDescriptorField(NULL, DT_NULL, sizeof(ActorCollisionData), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_ActorCollisionData("ActorCollisionData", 0, ActorCollisionData_Fields, 8, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 97
const XTypeDescriptor * DT_ACTOR_COLLISION_DATA = &TypeDescriptor_ActorCollisionData;

static const TypeDescriptorField WeightedLook_Fields[3] = {  // line 99
    TypeDescriptorField("", DT_LOOKLINK, offsetof(WeightedLook, tLookLink), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(WeightedLook, nWeight), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(WeightedLook), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_WeightedLook("WeightedLook", 0, WeightedLook_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 102
const XTypeDescriptor * DT_WEIGHTEDLOOK = &TypeDescriptor_WeightedLook;

static const TypeDescriptorField InventoryImages_Fields[3] = {  // line 104
    TypeDescriptorField("", DT_INT, offsetof(InventoryImages, hDefaultImage), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(InventoryImages, hFemaleImage), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(InventoryImages), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_InventoryImages("InventoryImages", 0, InventoryImages_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 107
const XTypeDescriptor * DT_INVENTORYIMAGES = &TypeDescriptor_InventoryImages;

static const TypeDescriptorField ActorDefinition_Fields[32] = {  // line 109
    TypeDescriptorField("", DT_INT, offsetof(ActorDefinition, dwFlags), NULL, 0x80001),
    EnumTypeDescriptorField("", offsetof(ActorDefinition, eType), NULL, gActorTypes),
    FilterTypeDescriptorField("", DT_SNO, offsetof(ActorDefinition, snoAppearance), "", APPEARANCE_SNO, 0x100, -1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(ActorDefinition, snoPhysMesh), "", PHYSMESH_SNO, 0, -1),
    TypeDescriptorField("", DT_AXIALCYLINDER, offsetof(ActorDefinition, wcylBounds), NULL, 1),
    TypeDescriptorField("", DT_SPHERE, offsetof(ActorDefinition, wsBounds), NULL, 1),
    TypeDescriptorField("", DT_AABB, offsetof(ActorDefinition, aabbBounds), NULL, 0),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(ActorDefinition, serTagMap), NULL, 0),
    TagMapTypeDescriptorField("", offsetof(ActorDefinition, hTagMap), 0x21,
                              (int32)offsetof(ActorDefinition, serTagMap) - (int32)offsetof(ActorDefinition, hTagMap), 0x10000),
    FilterTypeDescriptorField("", DT_SNO, offsetof(ActorDefinition, snoAnimSet), "", ANIMSET_SNO, 0, -1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(ActorDefinition, snoAnimTree), "", ANIMTREE_SNO, 0x700000, -1),
    TypeDescriptorField("", DT_INT, offsetof(ActorDefinition, dwPad), NULL, 0x700000),
    FilterTypeDescriptorField("", DT_SNO, offsetof(ActorDefinition, snoMonster), "", MONSTER_SNO, 0, -1),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(ActorDefinition, serMsgTriggeredEvents), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_MSGTRIGGEREDEVENT, offsetof(ActorDefinition, ptMsgTriggeredEvents), 0x21,
                                     (int32)offsetof(ActorDefinition, serMsgTriggeredEvents) - (int32)offsetof(ActorDefinition, ptMsgTriggeredEvents)),
    VariableArrayCountTypeDescriptorField("", offsetof(ActorDefinition, nMsgTriggeredEventCount), 0x10000, DT_MSGTRIGGEREDEVENT,
                                          (int32)offsetof(ActorDefinition, serMsgTriggeredEvents) - (int32)offsetof(ActorDefinition, nMsgTriggeredEventCount)),
    TypeDescriptorField("", DT_VECTOR3D, offsetof(ActorDefinition, wpLocationPowerSrc), &scgwpDefaultLocationPowerSrc, 1),
    FixedArrayTypeDescriptorField("", DT_WEIGHTEDLOOK, offsetof(ActorDefinition, arActorLooks), NULL, 1, MAX_ACTOR_LOOKS_CNT, 0),
    FilterTypeDescriptorField("", DT_SNO, offsetof(ActorDefinition, snoPhysics), "", PHYSICS_SNO, 0, -1),
    TypeDescriptorField("", DT_INT, offsetof(ActorDefinition, dwPhysicsFlags), NULL, 0x80001),
    TypeDescriptorField("", DT_INT, offsetof(ActorDefinition, eMaterial), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(ActorDefinition, flExplosionFactor), (const void *)0x3f800000, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(ActorDefinition, flWindFactor), (const void *)0x3f800000, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(ActorDefinition, flPartialRagdollResponsiveness), (const void *)0x3f000000, 1),
    TypeDescriptorField("", DT_ACTOR_COLLISION_DATA, offsetof(ActorDefinition, tCollData), NULL, 1),
    FixedArrayTypeDescriptorField("", DT_INVENTORYIMAGES, offsetof(ActorDefinition, tInvImages), NULL, 1, 7, 0),
    TypeDescriptorField("", DT_INT, offsetof(ActorDefinition, hSocketedImage), NULL, 1),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(ActorDefinition, serVOCastingNotes), NULL, 0),
    CStringTypeDescriptorField("", offsetof(ActorDefinition, szVOCastingNotes), 0x21,
                               (int32)offsetof(ActorDefinition, serVOCastingNotes) - (int32)offsetof(ActorDefinition, szVOCastingNotes)),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(ActorDefinition, serVORole), NULL, 0),
    CStringTypeDescriptorField("", offsetof(ActorDefinition, szVORole), 0x21,
                               (int32)offsetof(ActorDefinition, serVORole) - (int32)offsetof(ActorDefinition, szVORole)),
    TypeDescriptorField(NULL, DT_NULL, sizeof(ActorDefinition), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_ActorDefinition("Actor", ACTOR_XML_VERSION, ActorDefinition_Fields, 32, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 144
const XTypeDescriptor * DT_ACTOR = &TypeDescriptor_ActorDefinition;

ActorGroupDef * gptGroupDefActor;  // line 151

// line 157
ActorGroupDef::ActorGroupDef()
    : GroupDefinition("Actor", ACTOR_SNO,
                      SNOVersions(ACTOR_PC_SNO_VERSION, 293, ACTOR_XBOXONE_SNO_VERSION, ACTOR_PS4_SNO_VERSION, 289),
                      SNODATATYPE_TITLE, sizeof(ActorDefinition), DT_ACTOR, 0x4641, 0x500e) {
}

// line 184
uint32 ActorGroupDef::GetMaxCount() {
    return 1024;
}

// line 200
void ActorGroupDef::GetDefaultData(void * ptDefaultDef) const {
    SigmaMemoryClear(ptDefaultDef, sizeof(ActorDefinition));

    ActorDefinition * pdef = (ActorDefinition *)ptDefaultDef;
    pdef->tHeader.snoHandle = SNO_NONE;
    pdef->snoAppearance = SNO_NONE;
    pdef->snoPhysMesh = SNO_NONE;
    pdef->snoPhysics = SNO_NONE;
    pdef->eMaterial = (Shape::Material)0;
    pdef->flExplosionFactor = cflDefaultExplosionFactor;
    pdef->flWindFactor = cflDefaultWindFactor;
    pdef->flPartialRagdollResponsiveness = cflDefaultPartialRagdollResponsiveness;
    pdef->snoAnimSet = SNO_NONE;
    pdef->snoAnimTree = SNO_NONE;
    pdef->tCollData.tCollFlags.uStaticPreventFlags = 0;
    pdef->snoMonster = SNO_NONE;
    pdef->tCollData.tCollFlags.uStaticOneRequiredFlags = 1;
    pdef->tCollData.tCollFlags.uDynamicFlagsNoOverlap = 0x63b;
    pdef->tCollData.tCollFlags.uDynamicMyFlags = 1;
    pdef->tCollData.eCollisionShape = COLLISION_SHAPE_CYLINDER;
    pdef->tCollData.wcylCollision.wdHeight = 4.0f;
    pdef->tCollData.wcylCollision.wdRadius = 1.25f;
    pdef->tCollData.flMovingRadiusScalar = 0.8f;
    pdef->tCollData.flCylinderRadiusConsole = -1.0f;
}

// line 809
void ActorGroupDef::Postprocess(void * lpDefinition) {
    GroupDefinition::Postprocess(lpDefinition);

    ActorDefinition * ptActorDef = (ActorDefinition *)lpDefinition;
    ActorCollisionData & tCollisionData = ptActorDef->tCollData;
    float flConsoleRadiusOverride = tCollisionData.flCylinderRadiusConsole;
    if (flConsoleRadiusOverride > 1e-06f)
        tCollisionData.wcylCollision.wdRadius = flConsoleRadiusOverride;
}

// line 825
float ActorGetCollisionCylinderRadius(const ActorDefinition * pdefActor) {
    const ActorCollisionData & tCollisionData = pdefActor->tCollData;
    return tCollisionData.wcylCollision.wdRadius;
}

// line 832
BOOL ActorGroupDef::Write(void * lpDefinition, FileReference & tRef) const {
    return GroupDefinition::Write(lpDefinition, tRef);
}

// line 973
BOOL ActorDefinitionTagMapValueExists(const ActorTag eTag, const SNO snoActor) {
    ScopedActorDefinitionLockR ptDefActor(snoActor);
    return TagMapTagExists(ptDefActor->hTagMap, eTag);
}

// line 983
int32 ActorDefinitionGetTagMapValueInt(const ActorTag eTag, const SNO snoActor) {
    ScopedActorDefinitionLockR ptDefActor(snoActor);
    return TagMapGetInt(ptDefActor->hTagMap, eTag);
}

// line 993
uint32 ActorDefinitionGetTagMapValueDWORD(const ActorTag eTag, const SNO snoActor) {
    ScopedActorDefinitionLockR ptDefActor(snoActor);
    return TagMapGetDWORD(ptDefActor->hTagMap, eTag);
}

// line 1003
SNO ActorDefinitionGetTagMapValueSNO(const ActorTag eTag, const SNO snoActor) {
    ScopedActorDefinitionLockR ptDefActor(snoActor);
    return TagMapGetSNO(ptDefActor->hTagMap, eTag);
}

// line 1022
float ActorDefinitionGetTagMapValueFloat(const ActorTag eTag, const SNO snoActor) {
    ScopedActorDefinitionLockR ptDefActor(snoActor);
    return TagMapGetFloat(ptDefActor->hTagMap, eTag);
}

// line 1032
SNO ActorGetAnim(const SNO snoActor, const WeaponClass eWeaponClass, const uint32 dwAnimTagId) {
    SNO snoAnim = SNO_NONE;

    if (snoActor != SNO_NONE) {
        ScopedActorDefinitionLockR ptActor(snoActor);
        if (ptActor->snoAnimSet != SNO_NONE)
            snoAnim = AnimSetGetAnim(ptActor->snoAnimSet, eWeaponClass, dwAnimTagId);
    }

    return snoAnim;
}

// line 1053
void ActorGetSpecificAnimSet(const SNO snoActor, const WeaponClass eWeaponClass, const uint32 dwTag, SNO * psnoAnimSet, uint32 * pdwAnimSetTag) {
    ScopedActorDefinitionLockR ptActor(snoActor);

    if (ptActor->snoAnimSet != SNO_NONE) {
        AnimSetGetSpecificAnimSet(ptActor->snoAnimSet, eWeaponClass, dwTag, psnoAnimSet, pdwAnimSetTag);
    } else {
        for (int32 i = 0; i < 3; i++) {
            psnoAnimSet[i] = SNO_NONE;
            pdwAnimSetTag[i] = (uint32)-1;
        }
    }
}

// line 1078
SNO ActorGetAnimSetSNO(const SNO snoActor) {
    ScopedActorDefinitionLockR ptActor(snoActor);
    return ptActor->snoAnimSet;
}

// line 1125
uint8 ActorChooseLookLink(const ActorDefinition * pdefActor, RNG & rng) {
    const WeightedLook * arLooks = pdefActor->arActorLooks;

    int32 nTotalWeight = 0;
    for (int32 i = 0; i < MAX_ACTOR_LOOKS_CNT; i++) {
        if (arLooks[i].nWeight > 0)
            nTotalWeight += arLooks[i].nWeight;
    }

    int32 nRand = RandRange_Internal(rng, 0, nTotalWeight);

    nTotalWeight = 0;
    for (int32 i = 0; i < MAX_ACTOR_LOOKS_CNT; i++) {
        if (arLooks[i].nWeight > 0) {
            nTotalWeight += arLooks[i].nWeight;
            if (nRand <= nTotalWeight)
                return (uint8)i;
        }
    }

    return 0;
}

// line 1177
SNO ActorGetMonsterSNO(const SNO snoActor) {
    ScopedActorDefinitionLockR ptActor(snoActor);
    return ptActor->snoMonster;
}

// line 1195
void ActorGetPortraitTexFrame(ImageTextureFrame & tTexFrame, const SNO snoActor, const BOOL bUseAlt) {
    ImageHandle hImage = 0;

    if (snoActor != SNO_NONE) {
        if (bUseAlt) {
            if (ActorDefinitionTagMapValueExists(0x10608, snoActor))
                hImage = ActorDefinitionGetTagMapValueInt(0x10608, snoActor);
        } else {
            if (ActorDefinitionTagMapValueExists(0x10606, snoActor))
                hImage = ActorDefinitionGetTagMapValueInt(0x10606, snoActor);
        }
    }

    ImageCollectionGetImageInfo(IMAGE_COLLECTION_UI, hImage, tTexFrame);
}

// line 1228
BOOL ActorSNOIsUntargetable(const SNO & snoActor) {
    BOOL bUntargetableByDefinition = FALSE;

    ScopedActorDefinitionLockR ptDefActor(snoActor);
    if (TagMapTagExists(ptDefActor->hTagMap, 0x100a0))
        bUntargetableByDefinition = TagMapGetInt(ptDefActor->hTagMap, 0x100a0) == 1;

    return bUntargetableByDefinition;
}
