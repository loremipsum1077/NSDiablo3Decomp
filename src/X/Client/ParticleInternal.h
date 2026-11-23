// Subset of X/Client/ParticleInternal.h. The full reference header pulls in Particle.h and the
// texture/animation tree; only the members ParticleCommon.cpp touches are declared, at their
// documented DWARF offsets. Fields that are not adopted keep their space as _unadoptedN.
#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "Particle.h"
#include "TexAnimation.h"
#include "Physics/Physics.h"
#include "../Sigma/Math/MathGeneral.h"
#include "../Sigma/Math/Vector.h"

typedef int32 ParticleSystemType;

// X/Client/ParticleInternal.h (DWARF enum ParticleInterp).
enum ParticleInterp {
    PARTICLE_INTERP_INVALID = 0,
    PARTICLE_INTERP_SCALE = 1,
    PARTICLE_INTERP_STRETCH = 2,
    PARTICLE_INTERP_COLOR = 3,
    PARTICLE_INTERP_EMISSIVE = 4,
    PARTICLE_INTERP_ALPHA = 5,
    PARTICLE_INTERP_ALPHAAUX = 6,
    PARTICLE_INTERP_CYLINDERRADIALPOS = 7,
    PARTICLE_INTERP_CYLINDERRADIALVEL = 8,
    PARTICLE_INTERP_CYLINDERANGLEVEL = 9,
    PARTICLE_INTERP_CYLINDERAXIS = 10,
    PARTICLE_INTERP_SPHERERADIALPOS = 11,
    PARTICLE_INTERP_SPHERERADIALVEL = 12,
    PARTICLE_INTERP_ATTRACTORVEL = 13,
    PARTICLE_INTERP_ATTRACTORPOS = 14,
    PARTICLE_INTERP_ORIENTATIONVEL = 15,
    PARTICLE_INTERP_ORIENTATIONPOS = 16,
    PARTICLE_INTERP_CARTESIANPOS = 17,
    PARTICLE_INTERP_CARTESIANVEL = 18,
    PARTICLE_INTERP_CARTESIANACC = 19,
    PARTICLE_INTERP_RELATIVECARTESIANPOS = 20,
    PARTICLE_INTERP_RELATIVECARTESIANVEL = 21,
    PARTICLE_INTERP_RELATIVECARTESIANACC = 22,
    PARTICLE_INTERP_ORIENTATIONAXIS = 23,
    PARTICLE_INTERP_ANG_POSITION = 24,
    PARTICLE_INTERP_ANG_VELOCITY = 25,
    PARTICLE_INTERP_JITTERAMPLITUDE = 26,
    PARTICLE_INTERP_JITTERRATE = 27,
    PARTICLE_INTERP_INITIAL_SCALE = 28,
    PARTICLE_INTERP_PARTICLE_DURATION = 29,
    PARTICLE_INTERP_SPEED_DURATION_FACTOR = 30,
    PARTICLE_INTERP_SYSTEM_COUNT = 31,
    PARTICLE_INTERP_SYSTEM_SPAWN_RATE = 32,
    PARTICLE_INTERP_SYSTEM_SPEED_SPAWN_RATE = 33,
    PARTICLE_INTERP_SYSTEM_SCALE = 34,
    PARTICLE_INTERP_SYSTEM_ALPHA = 35,
    PARTICLE_INTERP_SYSTEM_ATTRACTOR_VEL = 36,
    PARTICLE_INTERP_SYSTEM_ATTRACTOR_POS = 37,
    PARTICLE_INTERP_INITIAL_SOURCE_RELATIVE_VEL = 38,
    PARTICLE_INTERP_INITIAL_SOURCE_RELATIVE_SPREAD = 39,
    PARTICLE_INTERP_INITIAL_WORLD_RELATIVE_VEL = 40,
    PARTICLE_INTERP_INITIAL_ORIENTATION_VEL = 41,
};

class XSphereNode;

struct FoliageState {  // size 0x28
    Vector2D wvOffset;  // 0x00
    Vector2D wvVelocity;  // 0x08
    Vector2D wvVelocityWind;  // 0x10
    XSphereNode * ptSphereNode;  // 0x18
    float flExplosionFactor;  // 0x20
};
static_assert(sizeof(FoliageState) == 0x28, "FoliageState");

struct ParticleParams {  // size 0x160
    ParticleSystemType eType;  // 0x00
    uint32 dwInterpSeed;  // 0x04
    int32 idParticleSystem;  // 0x08 ParticleSystemID
    PointMass * ptPointMass;  // 0x10
    float flBaseScale;  // 0x18
    Vector3D wpLast;  // 0x1c
    float flDuration;  // 0x28
    float flSeconds;  // 0x2c
    Vector3D wvLastSphereRadial;  // 0x30
    Vector2D wvLastCylinderRadial;  // 0x3c
    float fLastCylinderRadialPos;  // 0x44
    float fLastSphereRadialPos;  // 0x48
    Vector3D wvLastCartesianPos;  // 0x4c
    Vector3D wvTotalCartesianAcc;  // 0x58
    Vector3D wvLastRelativeCartesianPos;  // 0x64
    Vector3D wvTotalRelativeCartesianAcc;  // 0x70
    Vector3D wvTotalRelativeCartesianPos;  // 0x7c
    float fLastAttractorPos;  // 0x88
    Vector3D wvMotion;  // 0x8c
    Vector3D wvLastMotionNormalized;  // 0x98
    Angle aLastRotationPos;  // 0xa4
    WorldDistance wdParticleSpeed;  // 0xa8
    Vector3D wvOrientationAxis;  // 0xac
    Quaternion qRelativeOrientation;  // 0xb8
    Angle aLastAngPosition;  // 0xc8
    Angle aAng;  // 0xcc
    float flAlpha;  // 0xd0
    float flScale;  // 0xd4
    Quaternion qOrientation;  // 0xd8
    RGBAColor rgbaColor;  // 0xe8
    RGBAColor rgbaAlphaAux;  // 0xec
    float fStretch;  // 0xf0
    Vector3D wpLastValidNormal;  // 0xf4
    Vector3D wvGroundNormal;  // 0x100
    float flJitterRateAccumulator;  // 0x10c
    Vector3D wvJitterTarget;  // 0x110
    Vector3D wvJitter;  // 0x11c
    FoliageState tFoliageState;  // 0x128
    ParticleCreateParams tCreateParams;  // 0x150
};
static_assert(sizeof(ParticleParams) == 0x160, "ParticleParams");

enum {
    PARTICLEID_INVALID = -1,
};

typedef int32 ParticleID;

struct Particle {  // size 0x298
    ParticleID id;  // 0x00
    Vector3D wp;  // 0x04
    TexAnimState tTexAnimState[4];  // 0x10
    ParticleParams tParams;  // 0x130
    ParticleSystemID idParticleSystem;  // 0x290
    int32 nIndex;  // 0x294
};
static_assert(sizeof(Particle) == 0x298, "Particle");

// X/Client/Particle.cpp
Particle * ParticleGet(int32 idParticle);
Particle * ParticleTryToGet(int32 idParticle);

typedef int32 RainParticleID;

struct RainParticle {  // size 0xb0
    RainParticleID id;  // 0x00
    Vector3D wp;  // 0x04
    ParticleSystemID idParticleSystem;  // 0x10
    PointMass * ptPointMass;  // 0x18
    float flDuration;  // 0x20
    float flSeconds;  // 0x24
    Vector3D wvMotion;  // 0x28
    Vector3D wvLastMotionNormalized;  // 0x34
    TexAnimState tTexAnimState;  // 0x40
    float flAlpha;  // 0x88
    float flScale;  // 0x8c
    Quaternion qOrientation;  // 0x90
    RGBAColor rgbaColor;  // 0xa0
    float fStretch;  // 0xa4
    int32 nIndex;  // 0xa8
};
static_assert(sizeof(RainParticle) == 0xb0, "RainParticle");
