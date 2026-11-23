// Subset of X/Client/Definitions/ParticleDefinition.h. The full reference header pulls in
// GfxMaterial.h and the definitions tree; only the enums and typedefs needed by adopted particle
// sources are included here.
// Original file: X/Client/Definitions/ParticleDefinition.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Common/Definitions/Definitions.h"
#include "../Gfx/GfxMaterial.h"
#include "../InterpolationScalar.h"
#include "../InterpolationTrack.h"
#include "../../Sigma/DataStructures/XPtr64.h"
#include "../../Sigma/Files/XSerializeData.h"

typedef int32 EmitterType;

enum {
    PSTYPE_BILLBOARD = 0,
    PSTYPE_ACTOR = 1,
    PSTYPE_PHYS_BILLBOARD = 2,
    PSTYPE_PHYS_ACTOR = 3,
    PSTYPE_FOLIAGE_ACTOR = 4,
    PSTYPE_FOLIAGE_BILLBOARD = 5,
    PSTYPE_FOLIAGE_DYNAMIC = 6,
    PSTYPE_CLUTTER_BILLBOARD = 7,
    PSTYPE_CLUTTER_DYNAMIC = 8,
    PSTYPE_RAIN = 9,
    PSTYPE_RAIN_RIPPLE = 10,
    PSTYPE_SPECIAL_TO_DELETE = 11,
};

typedef int32 ParticleSystemType;

enum {
    PARTICLE_DEF_INFINITE_DURATION_BIT = 0,
    PARTICLE_DEF_COMPUTE_NORMALS_BIT = 1,
    PARTICLE_DEF_GROUND_HUG_ON_SPAWN = 2,
    PARTICLE_DEF_CREATE_SYSTEM_WHEN_OFFSCREEN = 3,
    PARTICLE_DEF_CREATE_PARTICLES_WHEN_OFFSCREEN = 4,
    PARTICLE_DEF_DELETE_PARTICLES_WHEN_OFFSCREEN = 5,
    PARTICLE_DEF_SPU_UPDATE_FRIENDLY_BIT = 6,
    PARTICLE_DEF_BOTTOM_ALIGNED_BIT = 7,
    PARTICLE_DEF_STAYS_RELATIVE_TO_SYSTEM_BIT = 8,
    PARTICLE_DEF_DELETE_PARTICLE_AT_ANIMATION_END_BIT = 9,
    PARTICLE_DEF_DELETE_PARTICLE_AT_DURATION_END_BIT = 10,
    PARTICLE_DEF_PREPUMP_BY_SERVER_CREATION_TIME = 11,
    PARTICLE_DEF_RANDOM_LAUNCH_SPIN_FACING = 12,
    PARTICLE_DEF_GROUND_HUGGING = 13,
    PARTICLE_DEF_SPU_RENDER_UPDATE_FRIENDLY_BIT = 14,
    PARTICLE_DEF_CRITICAL = 15,
    PARTICLE_DEF_MESH_LIT_ON_CREATE = 16,
    PARTICLE_DEF_ATTRACTED_TO_CAMERA = 17,
    PARTICLE_DEF_ATTACHED_TO_CAMERA = 18,
    PARTICLE_DEF_DROP_ON_CONSOLE = 19,
    PARTICLE_DEF_DROP_ON_HIGH_LOAD = 20,
    PARTICLE_DEF_FADE_WHEN_SYSTEM_OCCLUDED_DEPRECATED = 21,
    PARTICLE_DEF_FADE_WHEN_PARTICLE_OCCLUDED_DEPRECATED = 22,
    PARTICLE_DEF_REFLECT_ON_COLLISION_DEPRECATED = 23,
    PARTICLE_DEF_ORIENTATION_SPIN_AROUND_OBJECT_CENTER = 24,
    PARTICLE_DEF_DYNAMICALLY_LIT = 25,
    PARTICLE_DEF_DELETE_WHEN_OUTSIDE_BOUNDING_RADIUS = 26,
    PARTICLE_DEF_USE_FIXED_CLOCK_BIT = 27,
    PARTICLE_DEF_SPEED_RELATIVE_SPAWN_DISABLED_AT_MAX_SPEED = 28,
    PARTICLE_DEF_IGNORES_SYSTEM_ORIENTATION_BIT = 29,
    PARTICLE_DEF_ATTRACTOR_VEL_WORLD_SPACE_BIT = 30,
    PARTICLE_DEF_HEAD_INSERT = 31,
};

enum ParticleFacing {
    PARTICLE_FACING_BILLBOARD = 0,
    PARTICLE_FACING_UP = 1,
    PARTICLE_FACING_SIDE_OF_ARROW = 2,
    PARTICLE_FACING_POSTER_ON_ARROW = 3,
    PARTICLE_FACING_CYLINDER = 4,
    PARTICLE_FACING_SPHERE = 5,
    PARTICLE_FACING_CYLINDER_BILLBOARD = 6,
    PARTICLE_FACING_STATIC = 7,
    PARTICLE_FACING_FULL_SCREEN = 8,
    PARTICLE_FACING_GROUND = 9,
    PARTICLE_FACING_GROUND_PROJECTION = 10,
    PARTICLE_FACING_SPIKY_BALL = 11,
    PARTICLE_FACING_FRISBEE = 12,
    PARTICLE_FACING_CAMERA_LOCKED_TO_XY = 13,
};

enum {
    PARTICLE_MSGKEY_SYSTEM_CREATE = 3000,
    PARTICLE_MSGKEY_SYSTEM_DELETE = 3001,
    PARTICLE_MSGKEY_SYSTEM_OUTRO = 3002,
    PARTICLE_MSGKEY_SYSTEM_DETACH = 3003,
    PARTICLE_MSGKEY_PARTICLE_CREATE = 3500,
    PARTICLE_MSGKEY_PARTICLE_DELETE = 3501,
    PARTICLE_MSGKEY_PARTICLE_COLLISION_DEPRECATED = 3502,
    PARTICLE_MSGKEY_PARTICLE_OUTRO = 3503,
    PARTICLE_MSGKEY_PARTICLE_COLLISION_FLOOR_DEPRECATED = 3504,
    PARTICLE_MSGKEY_PARTICLE_COLLISION_WALL_DEPRECATED = 3505,
};

class XMemoryAllocatorInterface;
struct FileReference;

struct MsgTriggeredEvent;

struct EmitterParams {  // size 0x40
    XPtr64<InterpolationPath<float> > pathRadius;  // 0x00
    XPtr64<InterpolationPath<float> > pathHeight;  // 0x08
    XPtr64<VectorPath> pathBoxExtents;  // 0x10
    SerializeData serPathRadius_Console;  // 0x18
    SerializeData serPathHeight_Console;  // 0x20
    SerializeData serPathBoxExtents_Console;  // 0x28
    EmitterType eType;  // 0x30
    uint32 dwSubObjectHash;  // 0x34
    uint32 dwPad;  // 0x38
};
static_assert(sizeof(EmitterParams) == 0x40, "EmitterParams");

struct ExtendedPath_Console {  // size 0xc0
    XPtr64<VectorPath> pathInitialSourceRelativeVelocity;  // 0x00
    SerializeData serPathInitialSourceRelativeVelocity_Console;  // 0x08
    XPtr64<VectorPath> pathInitialWorldRelativeVelocity;  // 0x10
    SerializeData serPathInitialWorldRelativeVelocity_Console;  // 0x18
    XPtr64<InterpolationPath<float> > pathSystemSpeedSpawnRate;  // 0x20
    SerializeData serPathSystemSpeedSpawnRate_Console;  // 0x28
    XPtr64<InterpolationPath<float> > pathParticleAlphaAux;  // 0x30
    SerializeData serPathParticleAlphaAux_Console;  // 0x38
    XPtr64<InterpolationPath<float> > pathOrientationVelocity;  // 0x40
    SerializeData serPathOrientationVelocity_Console;  // 0x48
    XPtr64<InterpolationPath<float> > pathOrientationPosition;  // 0x50
    SerializeData serPathOrientationPosition_Console;  // 0x58
    XPtr64<InterpolationPath<float> > pathMotionCylinderRadialPos;  // 0x60
    SerializeData serPathMotionCylinderRadialPos_Console;  // 0x68
    XPtr64<InterpolationPath<float> > pathMotionSphereRadialPos;  // 0x70
    SerializeData serPathMotionSphereRadialPos_Console;  // 0x78
    XPtr64<InterpolationPath<float> > pathMotionSphereRadialVel;  // 0x80
    SerializeData serPathMotionSphereRadialVel_Console;  // 0x88
    XPtr64<VectorPath> pathMotionRelativeCartesianAcc;  // 0x90
    SerializeData serPathMotionRelativeCartesianAcc_Console;  // 0x98
    XPtr64<InterpolationPath<float> > pathMotionAttractorVel;  // 0xa0
    SerializeData serPathMotionAttractorVel_Console;  // 0xa8
    XPtr64<InterpolationPath<float> > pathMotionAttractorPos;  // 0xb0
    SerializeData serPathMotionAttractorPos_Console;  // 0xb8
};
static_assert(sizeof(ExtendedPath_Console) == 0xc0, "ExtendedPath_Console");

struct ParticleQuad {  // size 0x10
    float flShrinkMinX;  // 0x00
    float flShrinkMaxX;  // 0x04
    float flShrinkMinY;  // 0x08
    float flShrinkMaxY;  // 0x0c
};
static_assert(sizeof(ParticleQuad) == 0x10, "ParticleQuad");

struct ParticleDefinition {  // size 0x2c0
    RequiredDefinitionHeader tHeader;  // 0x00
    ParticleSystemType eType;  // 0x0c
    uint32 dwFlags;  // 0x10
    TickTime tSystemDuration;  // 0x14
    TickTime tOutroDuration;  // 0x18
    TickTime tPrepumpDuration;  // 0x1c
    InterpolationScalar tSystemDurationScalar;  // 0x20
    uint32 dwPad;  // 0x2c
    float flJitterApproachSpeed;  // 0x30
    UberMaterial tUberMaterial;  // 0x38
    SNO snoPhysics;  // 0xa0
    float flMass;  // 0xa4
    uint32 dwMaxSystems;  // 0xa8
    float flClutterZOffset;  // 0xac
    float flFoliageFrequency;  // 0xb0
    float flFoliageDampingRatio;  // 0xb4
    float flFoliageMaxOffsetFactor;  // 0xb8
    float flFoliageOscWindFactor;  // 0xbc
    float flFoliagePrevailingWindFactor;  // 0xc0
    float flFoliageExplosionFactor;  // 0xc4
    SNO snoActor;  // 0xc8
    EmitterParams tEmitterParams;  // 0xd0
    XPtr64<InterpolationPath<float> > pathSystemScale;  // 0x110
    SerializeData serPathSystemScale_Console;  // 0x118
    XPtr64<InterpolationPath<int> > pathSystemCount;  // 0x120
    SerializeData serPathSystemCount_Console;  // 0x128
    XPtr64<InterpolationPath<float> > pathSystemAlpha;  // 0x130
    SerializeData serPathSystemAlpha_Console;  // 0x138
    XPtr64<InterpolationPath<int> > pathParticleDuration;  // 0x140
    SerializeData serPathParticleDuration_Console;  // 0x148
    XPtr64<InterpolationPath<float> > pathInitialScale;  // 0x150
    SerializeData serPathInitialScale_Console;  // 0x158
    XPtr64<InterpolationPath<float> > pathInitialSourceRelativeSpread;  // 0x160
    SerializeData serPathInitialSourceRelativeSpread_Console;  // 0x168
    XPtr64<InterpolationPath<float> > pathSystemSpawnRate;  // 0x170
    SerializeData serPathSystemSpawnRate_Console;  // 0x178
    XPtr64<InterpolationPath<float> > pathSystemSpeedDurationFactor;  // 0x180
    SerializeData serPathSystemSpeedDurationFactor_Console;  // 0x188
    XPtr64<InterpolationPath<RGBAColor> > pathParticleColor;  // 0x190
    SerializeData serPathParticleColor_Console;  // 0x198
    XPtr64<InterpolationPath<float> > pathParticleAlpha;  // 0x1a0
    SerializeData serPathParticleAlpha_Console;  // 0x1a8
    XPtr64<InterpolationPath<float> > pathParticleScale;  // 0x1b0
    SerializeData serPathParticleScale_Console;  // 0x1b8
    XPtr64<InterpolationPath<float> > pathParticleStretch;  // 0x1c0
    SerializeData serPathParticleStretch_Console;  // 0x1c8
    XPtr64<InterpolationPath<float> > pathAngularPosition;  // 0x1d0
    SerializeData serPathAngularPosition_Console;  // 0x1d8
    XPtr64<InterpolationPath<float> > pathAngularVelocity;  // 0x1e0
    SerializeData serPathAngularVelocity_Console;  // 0x1e8
    XPtr64<VectorPath> pathOrientationAxis;  // 0x1f0
    SerializeData serPathOrientationAxis_Console;  // 0x1f8
    XPtr64<InterpolationPath<float> > pathMotionCylinderRadialVel;  // 0x200
    SerializeData serPathMotionCylinderRadialVel_Console;  // 0x208
    XPtr64<InterpolationPath<float> > pathMotionCylinderAngleVel;  // 0x210
    SerializeData serPathMotionCylinderAngleVel_Console;  // 0x218
    XPtr64<VectorPath> pathMotionCylinderAxis;  // 0x220
    SerializeData serPathMotionCylinderAxis_Console;  // 0x228
    XPtr64<VectorPath> pathMotionCartesianPos;  // 0x230
    SerializeData serPathMotionCartesianPos_Console;  // 0x238
    XPtr64<VectorPath> pathMotionCartesianVel;  // 0x240
    SerializeData serPathMotionCartesianVel_Console;  // 0x248
    XPtr64<VectorPath> pathMotionCartesianAcc;  // 0x250
    SerializeData serPathMotionCartesianAcc_Console;  // 0x258
    XPtr64<VectorPath> pathMotionRelativeCartesianPos;  // 0x260
    SerializeData serPathMotionRelativeCartesianPos_Console;  // 0x268
    XPtr64<VectorPath> pathMotionRelativeCartesianVel;  // 0x270
    SerializeData serPathMotionRelativeCartesianVel_Console;  // 0x278
    XPtr64<ExtendedPath_Console> ptExtendedPath_Console;  // 0x280
    SerializeData serExtendedPath_Console;  // 0x288
    XPtr64<ParticleQuad> ptParticleQuadStructure;  // 0x290
    SerializeData serParticleQuadStructure;  // 0x298
    ParticleFacing nFacing;  // 0x2a0
    WorldDistance wdBoundingRadius;  // 0x2a4
    float fAttachedToCameraDistanceFactor;  // 0x2a8
    SerializeData serMsgTriggeredEvents;  // 0x2ac
    int32 nMsgTriggeredEventCount;  // 0x2b4
    XPtr64<MsgTriggeredEvent> ptMsgTriggeredEvents;  // 0x2b8
};
static_assert(sizeof(ParticleDefinition) == 0x2c0, "ParticleDefinition");

class ParticleGroupDef : public GroupDefinition {  // size 0xa0
public:
    ParticleGroupDef();
    GROUPDEFINITION_MEMORY_FUNCTIONS()  // vtable slots 2-5; the macro name is invented, see Definitions.h
    virtual uint32 GetMaxCount();  // vtable slot 11
    virtual BOOL CanReferenceOtherSNOs() const { return TRUE; }  // vtable slot 13
    virtual SNOPackType GetSNOPackDestination(const void *) const { return SNOPACKTYPE_CLIENTDATA; }  // vtable slot 7
    virtual BOOL Write(void * lpDefinition, FileReference & tRef) const;  // vtable slot 19
    virtual void Postprocess(void * lpDefinition);  // vtable slot 17
    virtual void GetDefaultData(void * ptDefaultDef) const;  // vtable slot 21
};
static_assert(sizeof(ParticleGroupDef) == 0xa0, "ParticleGroupDef");

extern ParticleGroupDef * gptGroupDefParticle;  // ParticleDefinition.cpp

typedef ScopedDefinitionLockR<ParticleDefinition> ScopedParticleDefinitionLockR;

#line 420
template <> inline const ParticleDefinition * DefinitionLockR<ParticleDefinition>(SNO sno) { return (const ParticleDefinition *)gptGroupDefParticle->LockR(sno, FALSE); }
template <> inline void DefinitionUnlockR<ParticleDefinition>(const ParticleDefinition * pdef) { gptGroupDefParticle->UnlockR((void *&)pdef); }
