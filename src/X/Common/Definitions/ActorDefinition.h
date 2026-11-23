// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Definitions/ActorDefinition.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../ActorCommonData.h"
#include "AttachmentDefinition.h"
#include "Definitions.h"
#include "../ImageHandle.h"
#include "../SNO/SNOFiles.h"
#include "../../Sigma/DataStructures/XPtr64.h"
#include "../../Sigma/Files/XSerializeData.h"
#include "../../Sigma/Math/Cylinder.h"
#include "../../Sigma/Math/Rect.h"
#include "../../Sigma/Math/Shape.h"
#include "../../Sigma/Math/Sphere.h"
#include "../../Sigma/Math/Vector.h"
#include "../../Sigma/SigmaTypes.h"
#include <stddef.h>
#include <stdint.h>

struct FileReference;
struct ImageTextureFrame;
struct RNG;
class CTagMap;  // X/Common/TagMaps/Tags.h; only held through XPtr64 here
class XMemoryAllocatorInterface;

enum {
    ACTORDEF_SCALE_COMES_FROM_ACTOR_BIT = 0,
    ACTORDEF_HAS_RANDOM_FACING_BIT = 1,
    ACTORDEF_FADE_WHEN_NEAR_BIT = 2,
    ACTORDEF_NEVER_TURNS_BIT = 3,
    ACTORDEF_SENDS_CREATION_TIME_BIT = 4,
    ACTORDEF_HIGH_DYNAMIC_SHADOWS_DEPRECADED_BIT = 5,
    ACTORDEF_LOAD_ALL_LOOK_TEXTURES_BIT = 6,
};

enum {
    MAX_ACTOR_LOOKS_CNT = 8,
};

enum {
    ACTORTYPE_INVALID = 0,
    ACTORTYPE_MONSTER = 1,
    ACTORTYPE_GIZMO = 2,
    ACTORTYPE_CLIENT_EFFECT = 3,
    ACTORTYPE_SERVER_PROP = 4,
    ACTORTYPE_ENVIRONMENT = 5,
    ACTORTYPE_CRITTER = 6,
    ACTORTYPE_PLAYER = 7,
    ACTORTYPE_ITEM = 8,
    ACTORTYPE_AXE_SYMBOL = 9,
    ACTORTYPE_PROJECTILE = 10,
    ACTORTYPE_CUSTOM_BRAIN = 11,
    ACTORTYPE_COUNT = 12,
};

typedef int32 ActorType;

ActorType ActorGetType(const SNOName & snoName);  // ActorCommonData.cpp

enum {
    COLLISION_SHAPE_NONE = 0,
    COLLISION_SHAPE_CYLINDER = 1,
    COLLISION_SHAPE_BOX = 2,
};

typedef int32 CollisionShapeType;

enum {
    ACTORTOPOLOGY_SMALL = 0,
    ACTORTOPOLOGY_MEDIUM = 1,
    ACTORTOPOLOGY_LARGE = 2,
    ACTORTOPOLOGY_FAT = 3,
    ACTORTOPOLOGY_TALL = 4,
    ACTORTOPOLOGY_SKELETON = 5,
    ACTORTOPOLOGY_CRITTER = 6,
    ACTORTOPOLOGY_SPIDER = 7,
    ACTORTOPOLOGY_BOSS = 8,
    ACTORTOPOLOGY_GHOST = 9,
    NUM_ACTORTOPOLOGIES = 10,
};

typedef int32 ActorTopology;

enum {
    COLL_DYN_ACTOR = 0,
    COLL_DYN_PET = 1,
    COLL_DYN_PROJECTILE = 2,
    COLL_DYN_PLAYER = 3,
    COLL_DYN_GIZMO = 4,
    COLL_DYN_LOS_BLOCKER = 5,
    COLL_DYN_GATE = 6,
    COLL_DYN_FLIPPY = 7,
    COLL_DYN_OWNED = 8,
    COLL_DYN_WRECKABLE = 9,
    COLL_DYN_PROJECTILE_BLOCKER = 10,
    COLL_DYN_TELEPORT_BLOCKER = 11,
    COLL_DYN_MALTHAEL_GHOST = 12,
    COLL_DYN_FLAG_COUNT = 13,
};

struct ActorCollisionFlags {  // size 0x10
    uint32 uStaticOneRequiredFlags;  // 0x00
    uint32 uStaticPreventFlags;  // 0x04
    uint32 uDynamicFlagsNoOverlap;  // 0x08
    uint32 uDynamicMyFlags;  // 0x0c
};
static_assert(sizeof(ActorCollisionFlags) == 0x10, "ActorCollisionFlags");

typedef int32 ActorTag;

struct WeightedLook {  // size 0xc
    LookLink tLookLink;  // 0x00
    int32 nWeight;  // 0x08
};
static_assert(sizeof(WeightedLook) == 0xc, "WeightedLook");

struct ActorCollisionData {  // size 0x4c
    ActorCollisionFlags tCollFlags;  // 0x00
    CollisionShapeType eCollisionShape;  // 0x10
    AxialCylinder wcylCollision;  // 0x14
    AABB aabbCollision;  // 0x28
    float flMovingRadiusScalar;  // 0x40
    float flCylinderRadiusConsole;  // 0x44
    float flPadding;  // 0x48
};
static_assert(sizeof(ActorCollisionData) == 0x4c, "ActorCollisionData");

struct InventoryImages {  // size 0x8
    ImageHandle hDefaultImage;  // 0x00
    ImageHandle hFemaleImage;  // 0x04
};
static_assert(sizeof(InventoryImages) == 0x8, "InventoryImages");

struct ActorDefinition {  // size 0x1c0
    RequiredDefinitionHeader tHeader;  // 0x00
    uint32 dwFlags;  // 0x0c
    ActorType eType;  // 0x10
    SNO snoAppearance;  // 0x14
    SNO snoPhysMesh;  // 0x18
    AxialCylinder wcylBounds;  // 0x1c
    Sphere wsBounds;  // 0x30
    AABB aabbBounds;  // 0x40
    SerializeData serTagMap;  // 0x58
    XPtr64<CTagMap> hTagMap;  // 0x60
    SNO snoAnimSet;  // 0x68
    SNO snoAnimTree;  // 0x6c
    uint32 dwPad;  // 0x70
    SNO snoMonster;  // 0x74
    SerializeData serMsgTriggeredEvents;  // 0x78
    int32 nMsgTriggeredEventCount;  // 0x80
    XPtr64<MsgTriggeredEvent> ptMsgTriggeredEvents;  // 0x88
    Vector3D wpLocationPowerSrc;  // 0x90
    WeightedLook arActorLooks[8];  // 0x9c
    SNO snoPhysics;  // 0xfc
    uint32 dwPhysicsFlags;  // 0x100
    Shape::Material eMaterial;  // 0x104
    float flExplosionFactor;  // 0x108
    float flWindFactor;  // 0x10c
    float flPartialRagdollResponsiveness;  // 0x110
    ActorCollisionData tCollData;  // 0x114
    InventoryImages tInvImages[7];  // 0x160
    ImageHandle hSocketedImage;  // 0x198
    XPtr64<char> szVOCastingNotes;  // 0x1a0
    SerializeData serVOCastingNotes;  // 0x1a8
    XPtr64<char> szVORole;  // 0x1b0
    SerializeData serVORole;  // 0x1b8
};
static_assert(sizeof(ActorDefinition) == 0x1c0, "ActorDefinition");

class ActorGroupDef : public GroupDefinition {  // size 0xa0, line 219
public:
    ActorGroupDef();
    GROUPDEFINITION_MEMORY_FUNCTIONS()  // line 224; the macro name is invented, see Definitions.h
    virtual uint32 GetMaxCount();  // vtable slot 11
    virtual BOOL CanReferenceOtherSNOs() const { return TRUE; }  // vtable slot 13, line 227
    virtual SNOPackType GetSNOPackDestination(const void *) const {  // vtable slot 7, line 229
        return SNOPACKTYPE_COREDATA;
    }
    virtual void GetDefaultData(void * ptDefaultDef) const;  // vtable slot 21
    virtual BOOL Write(void * lpDefinition, FileReference & tRef) const;  // vtable slot 19
    virtual void Postprocess(void * lpDefinition);  // vtable slot 17
};
static_assert(sizeof(ActorGroupDef) == 0xa0, "ActorGroupDef");

extern ActorGroupDef * gptGroupDefActor;  // ActorDefinition.cpp:151

extern const float BOUNDING_SPHERE_FUDGE_FACTOR;  // ActorDefinition.cpp:59

typedef ScopedDefinitionLockR<ActorDefinition> ScopedActorDefinitionLockR;

template <> inline const ActorDefinition * DefinitionLockR<ActorDefinition>(SNO sno) {  // line 281
    return (const ActorDefinition *)gptGroupDefActor->LockR(sno, FALSE);
}
template <> inline void DefinitionUnlockR<ActorDefinition>(const ActorDefinition * pdef) {  // line 282
    void * lpDef = (void *)pdef;
    gptGroupDefActor->UnlockR(lpDef);
}

float ActorGetCollisionCylinderRadius(const ActorDefinition * pdefActor);
BOOL ActorDefinitionTagMapValueExists(const ActorTag eTag, const SNO snoActor);
int32 ActorDefinitionGetTagMapValueInt(const ActorTag eTag, const SNO snoActor);
uint32 ActorDefinitionGetTagMapValueDWORD(const ActorTag eTag, const SNO snoActor);
SNO ActorDefinitionGetTagMapValueSNO(const ActorTag eTag, const SNO snoActor);
float ActorDefinitionGetTagMapValueFloat(const ActorTag eTag, const SNO snoActor);
SNO ActorGetAnim(const SNO snoActor, const WeaponClass eWeaponClass, const uint32 dwAnimTagId);
void ActorGetSpecificAnimSet(const SNO snoActor, const WeaponClass eWeaponClass, const uint32 dwTag, SNO * psnoAnimSet, uint32 * pdwAnimSetTag);
SNO ActorGetAnimSetSNO(const SNO snoActor);
uint8 ActorChooseLookLink(const ActorDefinition * pdefActor, RNG & rng);
SNO ActorGetMonsterSNO(const SNO snoActor);
void ActorGetPortraitTexFrame(ImageTextureFrame & tTexFrame, const SNO snoActor, const BOOL bUseAlt);
BOOL ActorSNOIsUntargetable(const SNO & snoActor);


// Functions defined in this file (bodies are not in the DWARF):
//   line   219  ActorGroupDef::~ActorGroupDef()  [0x710060a3a0]
//   line   224  void * ActorGroupDef::MemAlloc(size_t nSize, uint32 dwAlign) const  [0x710060a3d0 (+1 more)]
//   line   224  void ActorGroupDef::MemFree(void *& ptr) const  [0x710060a420 (+1 more)]
//   line   227  BOOL ActorGroupDef::CanReferenceOtherSNOs() const  [0x710060a4b0]
//   line   229  SNOPackType ActorGroupDef::GetSNOPackDestination(const void *) const  [0x710060a4a0]
//   line   281  const ActorDefinition * DefinitionLockR<ActorDefinition>(SNO sno)  [inlined into 301 functions]
//   line   282  void DefinitionUnlockR<ActorDefinition>(const ActorDefinition * pdef)  [inlined into 301 functions]
