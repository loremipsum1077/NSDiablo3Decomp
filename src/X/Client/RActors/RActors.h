// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/RActors/RActors.h
// Types and members sit on their DWARF decl_line (the reference lists each type twice; the DWARF
// has one copy). The inline helpers at the end are written from their copies in the binary:
// RActorTryToGet from the matched stand-ins in Gizmo.cpp / CGame.cpp, RActorGet from
// Ragdoll::PostCompute, the accessors from RActorPhysicsUpdate, sRenderUpdateActorSubmit and
// Ragdoll::PostCompute (llvm-objdump -l). RActorGetInvClientTransform (line 652,
// DW_INL_inlined only, no concrete DIE) is inline, written from RActorDoesRayIntersectExact.
// The two static const floats at lines 139 and 143 have no value in the DWARF and are left out.
#pragma once
#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../AttachmentParent.h"
#include "../CGame.h"
#include "../Gfx/GfxCommon.h"
#include "../RMessage.h"
#include "../TexAnimation.h"
#include "../../Common/ActorCommon.h"
#include "../../Common/ActorCommonData.h"
#include "../../Common/Attributes/FastAttributeStructs.h"
#include "../../Common/CommonTypes.h"
#include "../../Common/InactiveMarker.h"
#include "../../Common/SNO/SNOFiles.h"
#include "../../Common/Target.h"
#include "../../Sigma/DataStructures/ChainedDataArrayTemplated.h"
#include "../../Sigma/DataStructures/DataArraysTemplated.h"
#include "../../Sigma/DataStructures/XGrowableMap.h"
#include "../../Sigma/DataStructures/XPooledList.h"
#include "../../Sigma/DataStructures/XTemplate.h"
#include "../../Sigma/Math/Cylinder.h"
#include "../../Sigma/Math/MathGeneral.h"
#include "../../Sigma/Math/Matrix.h"
#include "../../Sigma/Math/Transform.h"
#include "../../Sigma/Math/Vector.h"
#include "../../Sigma/RGBAColor.h"
#include "../../Sigma/SigmaTypes.h"
struct ActorAnim;
struct BannerRenderData;
struct Behavior;
class CActorMovement;
class OcclusionHighlighting;
struct ParticleParams;
struct RActorSubObjectData;
struct RigidAnim;
class RigidBody;
struct SubObjectState;
class XSphereNode;
typedef int32 OverlayType;
typedef int32 DyeType;
typedef int32 HairStyle;
typedef int32 ActorTag;
typedef int32 CosmeticItemType;
enum RActorFlags {
    RACTOR_SHADOWS_ENABLED_BIT = 0,
    RACTOR_HIGHLIGHT_BIT = 1,
    RACTOR_HIDE_BIT = 2,
    RACTOR_NEVER_DRAW_BIT = 3,
    RACTOR_HIDE_CLIENT_SIDE = 4,
    RACTOR_SELECTABLE_BIT = 5,
    RACTOR_DONT_PASS_ALPHA_TO_CHILDREN_BIT = 6,
    RACTOR_DONT_GET_ALPHA_FROM_PARENT_BIT = 7,
    RACTOR_IGNORE_SCALE_FROM_PARENT_BIT = 8,
    RACTOR_OFFSET_CENTER_BIT = 9,
    RACTOR_DELETE_ON_DEACTIVATE_BIT = 10,
    RACTOR_MESH_LIT_BIT = 11,
    RACTOR_NEED_EFFECT_UPDATE_BIT = 12,
    RACTOR_GOT_MELEE_EXPLOSION_BIT = 13,
    RACTOR_IS_MIRROR_IMAGE_BIT = 14,
    RACTOR_DISABLE_COLLISION_MESSAGES_BIT = 15,
    RACTOR_WEAPONS_SHEATHED_BIT = 16,
    RACTOR_USE_SHEATH_HP_BIT = 17,
    RACTOR_STEALTHED_BIT = 18,
    RACTOR_COMPLETELY_STEALTHED_BIT = 19,
    RACTOR_DELETE_THIS_TICK_BIT = 20,
    RACTOR_FADE_AFFECTS_SHADOWS_BIT = 21,
    RACTOR_HIDE_FIST_WEAPONS_BIT = 22,
    RACTOR_WEAPONS_HIDDEN_BIT = 23,
    RACTOR_DISABLE_ATTACHMENT_ERROR_BIT = 24,
    RACTOR_HIDE_ALPHA_ATTACHMENTS_BIT = 25,
    RACTOR_ALL_TEXTURE_PREFETCHED_BIT = 26,
    RACTOR_ALL_TEXTURE_LOADED_BIT = 27,
    RACTOR_BANNER_RING_ACTIVE = 28,
    RACTOR_BANNER_PLAYER_IS_PARTY_GUIDE = 29,
    RACTOR_SUBOBJECT_LOOK_UPDATE_LOCKED = 30,
    RACTOR_SUBOBJECT_TEXTURE_LOAD_ALL = 31,
};
enum RActorFlagsEx {
    RACTOREX_FIXED_UPDATE_BIT = 0,
    RACTOREX_SLOW_TIME_RADIUS_BIT = 1,
    RACTOREX_MAIN_HAND_WEAPON_HIDDEN_BIT = 2,
    RACTOREX_OFF_HAND_WEAPON_HIDDEN_BIT = 3,
    RACTOREX_FADE_WHEN_NEAR_BIT = 4,
    RACTOREX_FORCE_IDLE_ANIMATION_REFRESH_BIT = 5,
    RACTOREX_DISABLE_ANIMSET_PRELOAD_BIT = 6,
    RACTOREX_SUMMON_PRELOADED_BIT = 7,
    RACTOREX_SOUNDS_MUTED = 8,
    RACTOREX_FORCE_REMOTE_FX = 9,
    RACTOREX_USE_REMOTE_PET_ALPHA = 10,
};




enum RActorVisibleFlags {
    RACTOR_VISIBLE_BIT = 0,
    RACTOR_NOT_IN_LOS_BIT = 1,
    RACTOR_CASTS_SHADOW_BIT = 2,
};


enum {
    MAX_RACTORS = 1024,
};

enum RActorAddFlags {
    RACTOR_ADD_NO_WALK = 0,
    RACTOR_ADD_NO_MESH_LIGHT = 1,
    RACTOR_ADD_OVERRIDE_SCALE = 2,
    RACTOR_ADD_IS_FLIPPY = 3,
    RACTOR_ADD_FORCE_REMOTE_FX = 4,
};


typedef int32 TiltType; enum {
    TILT_TYPE_NORMAL = 0,
    TILT_TYPE_QUADRUPED = 1,
    TILT_TYPE_SPIDER = 2,
};


enum PickStyle {
    PICKSTYLE_CYLINDER = 0,
    PICKSTYLE_AABB = 1,
    PICKSTYLE_EXACT = 2,
    PICKSTYLE_LOWPOLY = 3,
};



// static const float cgflMinRActorElevation;  // line 139: value not in the DWARF

// static const float cgflStealthedFadeAmount;  // line 143: value not in the DWARF











typedef int32 InventorySlotType;
typedef int32 ItemEffectType;

typedef int32 EffectType;
typedef int32 DamageType;
typedef int32 ParentType;

typedef int32 MarkerID;
typedef int32 SoundID;

















struct CollisionCapsuleInstance {  // size 0x30
    Capsule tCapsule;  // 0x00
    XSphereNode * ptSphereNode;  // 0x20
    RActorID idRActor;  // 0x28
    int32 nCollisionCapsuleIndex;  // 0x2c
};
static_assert(sizeof(CollisionCapsuleInstance) == 0x30, "CollisionCapsuleInstance");

typedef XListMemoryPool<CollisionCapsuleInstance> CollisionCapsulePool;
typedef XPooledList<CollisionCapsuleInstance, CollisionCapsuleInstance &> CollisionCapsuleList;
typedef XGrowableMap<FastAttribKey, FastAttribKey, int, int> AppearanceAttributeAttachmentIDMap;
typedef XGrowableMap<unsigned int, unsigned int, int, int> LookOverrideMap;


class RActorTilt {  // size 0x24
public:

    void Clear();

    void Set(const TiltType nTiltType, const BOOL bSnap, const Vector3D & wvNormal, const Angle aYaw);




    BOOL Update();

    void Apply(PRSTransform & transform) const;

private:

    BOOL m_bUsingTilt;  // 0x00
    BOOL m_bTiltChanging;  // 0x04
    Vector3D m_wvTargetTilt;  // 0x08
    Vector3D m_wvCurrentTilt;  // 0x14
    Angle m_aCurrentSpeed;  // 0x20
};
static_assert(sizeof(RActorTilt) == 0x24, "RActorTilt");
class HitJitter {  // size 0x4
public:

    void Clear();

    void Set(const float flMag);

    BOOL Update();

    void Apply(PRSTransform & transform) const;

private:

    float m_flMag;  // 0x00
};
static_assert(sizeof(HitJitter) == 0x4, "HitJitter");
class RActorSink {  // size 0x8
public:

    void Clear();

    void Start(int32 nTicksToSink);

    BOOL Update();

    void Apply(RActor & tRActor, PRSTransform & transform) const;

private:


    float m_flUnitSink;  // 0x00
    float m_flUnitSinkSpeed;  // 0x04
};
static_assert(sizeof(RActorSink) == 0x8, "RActorSink");
struct RActorGeoSetData {  // size 0x10

    RActorSubObjectData * pSubobjectData;  // 0x00
    SubObjectState * ptSubObjectState;  // 0x08
};
static_assert(sizeof(RActorGeoSetData) == 0x10, "RActorGeoSetData");



struct RActor {  // size 0x410

    RActorID id;  // 0x00












    SSceneID idSScene;  // 0x04
    ACDID idACD;  // 0x08
    SNO snoActor;  // 0x0c
    SNO snoMonster;  // 0x10



    ActorLocation tLocation;  // 0x18


    PRSTransform transformClient;  // 0x88
    Matrix4x4 mWorld;  // 0xa8





    SNO snoAppearance;  // 0xe8

    int32 nLookNative;  // 0xec
    int32 nLookActive;  // 0xf0
    SNO snoPhysMesh;  // 0xf4

    ParentData tParentData;  // 0xf8
    ParticleParams * ptParticleParams;  // 0x128

    XSphereNode * ptEnvAppSphereNode;  // 0x130


    Vector3D wpWalkingPos;  // 0x138
    ActorAnim * ptActorAnim;  // 0x148






    CActorMovement * ptCActorMovement;  // 0x150

    Angle aYaw;  // 0x158


    float flLife;  // 0x15c
    float flTriggerFadePerTick;  // 0x160
    int32 nFadeGroupSubObjectIndex;  // 0x164
    float flAlpha;  // 0x168
    float flFadeAlpha;  // 0x16c
    float flTriggerFadeAlpha;  // 0x170


    OverlayType eOverlayType;  // 0x174
    uint32 dwOverlayFadeTimeCurrent;  // 0x178
    uint32 dwOverlayFadeTimeOriginal;  // 0x17c


    uint32 tName;  // 0x180



    uint32 dwLastFrameSubmitted;  // 0x184


    uint32 dwCreationTick;  // 0x188
    float flDeathTime;  // 0x18c
    uint32 dwSActorCreationTick;  // 0x190
    float flTimeLastHit;  // 0x194
    float flTimeLastFlinch;  // 0x198

    float flHitFlashAddTimeRemaining;  // 0x19c
    float flHitFlashAddDuration;  // 0x1a0
    RGBAColorValue rgbavalHitFlashAdd;  // 0x1a4
    float flHitFlashMulTimeRemaining;  // 0x1b4
    float flHitFlashMulDuration;  // 0x1b8
    RGBAColorValue rgbavalHitFlashMul;  // 0x1bc

    uint32 dwFlags;  // 0x1cc
    uint32 dwFlagsEx;  // 0x1d0
    uint32 dwLightFilterFlags;  // 0x1d4
    uint32 dwVisibilityFlags;  // 0x1d8

    float flLastLOSCheckTime;  // 0x1dc
    BOOL fLastLOSCheckResult;  // 0x1e0

    SNO snoPhysics;  // 0x1e4
    RigidBody * ptRigidBody;  // 0x1e8
    float flForceMultiplier;  // 0x1f0


    RMessageList tRMsgList;  // 0x1f8
    XPooledDataIDList tAttachmentList;  // 0x230
    SoundID idTalkSound;  // 0x258
    int32 nTrackedEffectGroupID;  // 0x25c


    RenderContext eContext;  // 0x260
    WorldDistance wdSimpleShadowRadius;  // 0x264
    SNO snoIrradianceTex;  // 0x268
    SNO snoIrradianceTexHighlight;  // 0x26c
    SNO snoAmbientOcclusionOverrideTex;  // 0x270
    float flRand;  // 0x274

    RActorGeoSetData tGeoSetData[2];  // 0x278

    RigidAnim * ptRigidAnim;  // 0x298

    RActorTilt tTilt;  // 0x2a0
    HitJitter tHitJitter;  // 0x2c4
    RActorSink tSink;  // 0x2c8


    CollisionCapsuleList tCollisionCapsuleList;  // 0x2d0

    BOOL bBoundingBoxCreated;  // 0x2f8
    Vector3D wvMinBounds, wvMaxBounds;  // 0x2fc, 0x308






    RGBAColor rgbaColor;  // 0x314
    ItemEffectType eItemEffectType;  // 0x318
    int32 nItemEffectLevel;  // 0x31c
    PickStyle ePickStyle;  // 0x320
    HairStyle eHairStyle;  // 0x324

    RGBAColorValue rgbavalSceneCurrent, rgbavalSceneTarget;  // 0x328, 0x338







    AppearanceAttributeAttachmentIDMap * ptEffectAttachments;  // 0x348
    LookOverrideMap * ptLookOverrideMap;  // 0x350

    MarkerID idMarker;  // 0x358

    Behavior * ptBehavior;  // 0x360

    Target tAimTarget;  // 0x368

    Vector3D wvVelocity;  // 0x384
    Vector3D wpLastUpdate;  // 0x390

    uint32 dwRActorLastSeen;  // 0x39c
    InactiveMarkerOwner tMarkerOwner;  // 0x3a0
    MinimapMarkerSetInfo tMinimapInfo;  // 0x3ac

    AttachmentID idRopeAttachment;  // 0x3b4

    float flShear;  // 0x3b8
    float flStretch;  // 0x3bc
    float flBaseScale;  // 0x3c0

    float flTimeUntilFade;  // 0x3c4
    float flFadeTime;  // 0x3c8
    float flFloatingNumberHealAccumulator;  // 0x3cc
    uint32 uNextFloatingNumberHealTime;  // 0x3d0

    ObjectTexAnimParam tObjTexAnimParam;  // 0x3d4

    uint32 dwDebugFlags;  // 0x3d8

    float flLookAtTargetIntensity;  // 0x3dc
    uint32 uLookAtTargetExpireTime;  // 0x3e0

    float flLastRegisterTime;  // 0x3e4
    int32 nSimTreeDepth;  // 0x3e8

    POS posMonsterCorpseList;  // 0x3f0

    BannerRenderData * ptBannerRenderData;  // 0x3f8


    OcclusionHighlighting * ptOcclusionHighlighting;  // 0x400
    BOOL bShowLocatorBeam;  // 0x408
};
static_assert(sizeof(RActor) == 0x410, "RActor");




typedef XPooledDataIDList RActorIDListPooled;































































































































inline RActor * RActorGet(const RActorID idRActor)
{




    CGame & tCGame = CGameGet();
    return tCGame.ptRActors->ElementGet(idRActor);
}

inline RActor * RActorTryToGet(const RActorID idRActor)
{
    if (idRActor == RACTORID_INVALID)
        return NULL;
    CGame & tCGame = CGameGet();
    if (!tCGame.ptRActors)
        return NULL;
    return tCGame.ptRActors->ElementTryToGet(idRActor);
}















inline WorldLocation & RActorGetWorldLocation(RActor * ptRActor)
{


    return ptRActor->tLocation.tWorldLocation;
}










inline const Matrix4x4 & RActorGetWorldMatrix(const RActor * ptRActor)
{


    return ptRActor->mWorld;
}



inline const PRSTransform & RActorGetClientTransform(const RActor * ptRActor)
{


    return ptRActor->transformClient;
}

inline PRSTransform RActorGetInvClientTransform(const RActor * ptRActor) { return ptRActor->transformClient.Inverse(); }  // line 652; return on line 656

struct ActorDefinition;
class Frustum;

// Defined in X/Client/RActors/RActors.cpp. Declared here as callers need them.
RActorID RActorCreate(const SNO snoActor, const float flScale, const float flLife, const PRTransform & transform, const uint32 dwRActorAddFlags, const RenderContext eContext, SSceneID idSScene);  // RActors.cpp:2068
float RActorGetAlpha(const RActor * ptRActor);  // RActors.cpp
int32 RActorRegisterParentObject(RActor * ptRActor);  // RActors.cpp
void RActorGetTopPoint(const RActor * ptRActor, Vector3D & wp);  // RActors.cpp
void RActorFlagForDeletion(RActor * ptRActor);  // RActors.cpp:11473
void RActorSnapToPosition(RActor * ptRActor, const Vector3D & wpPos, SSceneID idSScene, const ActorDefinition * pdefActor, const ActorCommonData * ptACD);  // RActors.cpp:6326
void RActorSetLastSeenTime(Frustum & tFrustum);  // RActors.cpp:10680
void RActorCheckSeenRecently();  // RActors.cpp:10747
void RActorsACDAttributeChanged(ActorCommonData & tACD, FastAttribKey tKey, FastAttribValue tOldValue, FastAttribValue tValue);  // RActors.cpp:7894
void RActorDelete(RActorID idRActor);  // RActors.cpp:2120

// X/Client/RActors/RActors.cpp:5790
const RActorID RActorGetTopmostOwner(const RActorID idRActor, BOOL bForEffects, BOOL fForIsInUI);
