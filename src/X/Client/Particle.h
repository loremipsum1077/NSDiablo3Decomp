// Subset of X/Client/Particle.h. The full reference header pulls in most of the client; only the
// types and members ParticleCommon.cpp touches are declared, at their documented DWARF offsets.
// Fields that are not adopted keep their space as _unadoptedN with the range they cover.
#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "AttachmentParent.h"
#include "InterpolationTrack.h"
#include "Gfx/GfxCommon.h"
#include "../Common/InactiveMarker.h"
#include "../Common/SNO/SNOFiles.h"
#include "../Sigma/Math/MathGeneral.h"
#include "../Sigma/Math/Transform.h"
#include "../Sigma/Math/Vector.h"
#include "../Sigma/Math/Quaternion.h"
#include "../Sigma/RGBAColor.h"
#include "../Sigma/Math/Sphere.h"
#include "../Sigma/DataStructures/XPooledList.h"
#include "../../Contrib/Contrib/blz/include/blz/vector.h"
#include "Physics/Physics.h"


typedef int32 ParticleSystemType;  // X/Client/Particle.h:23


class Frustum;
struct Camera;
class DisplayList;
struct ExplosionImpulse;
struct KeyedMessage;
struct Particle;
struct RainParticle;
struct ParticleCounts;
struct ParticleTextures;
struct ParticleDefinition;
struct HardpointLink;
struct ParticleQuad;  // Definitions/ParticleDefinition.h
















enum {
    PARTICLE_SYSTEM_USES_CYLINDER_MOTION = 0,
    PARTICLE_SYSTEM_USES_SPHERE_MOTION = 1,
    PARTICLE_SYSTEM_USES_CARTESIAN_MOTION = 2,
    PARTICLE_SYSTEM_USES_RELATIVE_CARTESIAN_MOTION = 3,
    PARTICLE_SYSTEM_USES_ORIENTATION_MOTION = 4,
    PARTICLE_SYSTEM_USES_JITTER = 5,
    PARTICLE_SYSTEM_HAS_ORIENTATION_AXIS = 6,
    PARTICLE_SYSTEM_USES_ATTRACTOR_MOTION = 7,
    PARTICLE_SYSTEM_USES_SPIN = 8,
    PARTICLE_SYSTEM_VISIBLE_BIT = 9,
    PARTICLE_SYSTEM_PARTIALLY_VISIBLE_BIT = 10,
    PARTICLE_SYSTEM_DONT_GET_ALPHA_FROM_PARENT = 11,
    PARTICLE_SYSTEM_UPDATE_WAS_SKIPPED = 12,
    PARTICLE_SYSTEM_HAS_ACTORS = 13,
    PARTICLE_SYSTEM_CAUSES_DISTORTION = 14,
    PARTICLE_SYSTEM_FLAGGED_FOR_RESET = 15,
    PARTICLE_SYSTEM_ALL_TEXTURE_PREFETCHED = 16,
    PARTICLE_SYSTEM_UPDATED_ON_SPU = 17,
    PARTICLE_SYSTEM_HAS_PARTICLE_TO_DELETE = 18,
    PARTICLE_SYSTEM_USES_LOCAL_PLAYER_SCALAR = 19,
    PARTICLE_SYSTEM_FIXED_UPDATE = 20,
    PARTICLE_SYSTEM_RENDERED_ONCE = 21,
};







enum BillboardTexState {
    BTS_DIFFUSE = 0,
    BTS_ALPHA = 1,
    BTS_ALPHA_LAYER_1 = 2,
    BTS_ALPHA_LAYER_2 = 3,
    NUM_BILLBOARD_TEX_STATES = 4,
};


















// DWARF puts the PARTICLESYSTEMID_INVALID enum and the ParticleSystemID typedef both on
// line 113, so they share one physical line here.

enum { PARTICLESYSTEMID_INVALID = -1, }; typedef int32 ParticleSystemID;





typedef blz::vector<Particle *, blz::allocator<Particle *> > ParticleList;


struct InactiveMarkerReference {  // size 0x38

    InactiveMarkerOwner tMarkerOwner;  // 0x00
    Quaternion qOrientation;  // 0x0c
    Vector3D wp;  // 0x1c
    Sphere wsBounds;  // 0x28
};
static_assert(sizeof(InactiveMarkerReference) == 0x38, "InactiveMarkerReference");
typedef XPooledList<InactiveMarkerReference, InactiveMarkerReference> InactiveMarkerReferenceList;
typedef XListMemoryPool<InactiveMarkerReference> InactiveMarkerReferenceListPool;

typedef XPooledDataIDList RActorIDListPooled;

struct ParticleSystemAddParams {  // size 0x74

    ParticleSystemAddParams(
        const SNO snoParticle,
        const PRTransform & transform)
    {
        this->snoParticle = snoParticle;
        this->transform = transform;
        this->fIsFlippy = FALSE;
        this->snoActorOverride = SNO_NONE;
        this->tParentData.Init();
        this->fIgnoreOwnerAlpha = FALSE;
        this->eContext = RENDERCONTEXT_DEFAULT;  // RENDERCONTEXT_DEFAULT
        this->flParticleScalar = 1.0f;
        this->bRemote = FALSE;
        this->fHasInitialAttractor = FALSE;
        this->wpAttractorDest = cgwvNull;
    }
    // Emission order of the inlined copy in ParticleSystemMarker::CreateForClientGame: tParentData's ctor,
    // snoParticle, transform, fIsFlippy/snoActorOverride, Init(), fIgnoreOwnerAlpha/eContext, the rest.
    SNO snoParticle;  // 0x00
    PRTransform transform;  // 0x04
    BOOL fIsFlippy;  // 0x20
    SNO snoActorOverride;  // 0x24
    ParentData tParentData;  // 0x28
    BOOL fIgnoreOwnerAlpha;  // 0x54
    RenderContext eContext;  // 0x58, RenderContext; RENDERCONTEXT_DEFAULT == 0
    float flParticleScalar;  // 0x5c
    BOOL bRemote;  // 0x60

    BOOL fHasInitialAttractor;  // 0x64
    Vector3D wpAttractorDest;  // 0x68
};
static_assert(sizeof(ParticleSystemAddParams) == 0x74, "ParticleSystemAddParams");
struct ParticleCreateParams {  // size 0x10

    RGBAColor rgbaColor;  // 0x00
    RGBAColor rgbaAlphaAux;  // 0x04
    float flDensity;  // 0x08
    BOOL fIgnoreGroundHug;  // 0x0c
};
static_assert(sizeof(ParticleCreateParams) == 0x10, "ParticleCreateParams");
struct ParticleConstParams {  // size 0xa0


    RGBAColor rgbaConstColor;  // 0x00

    float flConstAlpha;  // 0x04
    float flConstAlphaAux;  // 0x08
    float flConstScale;  // 0x0c
    float flConstStretch;  // 0x10

    Angle aConstAngVel;  // 0x14
    Angle aConstAngPos;  // 0x18

    Angle aConstOrientationVel;  // 0x1c
    Angle aConstOrientationPos;  // 0x20
    Vector3D vConstOrientationAxis;  // 0x24

    float flConstCylinderRadialVel;  // 0x30
    float flConstCylinderRadialPos;  // 0x34
    Angle aConstCylinderAngleVel;  // 0x38
    Vector3D wvConstCylinderAxis;  // 0x3c

    float flConstSphereRadialVel;  // 0x48
    float flConstSphereRadialPos;  // 0x4c

    Vector3D vConstCartesianPos;  // 0x50
    Vector3D vConstCartesianVel;  // 0x5c
    Vector3D vConstCartesianAcc;  // 0x68

    Vector3D vConstRelativeCartesianPos;  // 0x74
    Vector3D vConstRelativeCartesianVel;  // 0x80
    Vector3D vConstRelativeCartesianAcc;  // 0x8c

    float flConstAttractorVel;  // 0x98
    float flConstAttractorPos;  // 0x9c
};
static_assert(sizeof(ParticleConstParams) == 0xa0, "ParticleConstParams");
enum {
    PARTICLE_PATH_CONST_COLOR = 0,
    PARTICLE_PATH_CONST_ALPHA = 1,
    PARTICLE_PATH_CONST_ALPHAAUX = 2,
    PARTICLE_PATH_CONST_SCALE = 3,
    PARTICLE_PATH_CONST_STRETCH = 4,
    PARTICLE_PATH_CONST_ANGULAR_POSITION = 5,
    PARTICLE_PATH_CONST_ANGULAR_VELOCITY = 6,
    PARTICLE_PATH_CONST_ORIENTATION_VELOCITY = 7,
    PARTICLE_PATH_CONST_ORIENTATION_POSITION = 8,
    PARTICLE_PATH_CONST_ORIENTATION_AXIS = 9,
    PARTICLE_PATH_CONST_MOTION_CYLINDER_RADIAL_POS = 10,
    PARTICLE_PATH_CONST_MOTION_CYLINDER_RADIAL_VEL = 11,
    PARTICLE_PATH_CONST_MOTION_CYLINDER_ANGLE_VEL = 12,
    PARTICLE_PATH_CONST_MOTION_CYLINDER_AXIS = 13,
    PARTICLE_PATH_CONST_SPHERE_RADIAL_POS = 14,
    PARTICLE_PATH_CONST_SPHERE_RADIAL_VEL = 15,
    PARTICLE_PATH_CONST_MOTION_CARTESIAN_POS = 16,
    PARTICLE_PATH_CONST_MOTION_CARTESIAN_VEL = 17,
    PARTICLE_PATH_CONST_MOTION_CARTESIAN_ACC = 18,
    PARTICLE_PATH_CONST_MOTION_RELATIVE_CARTESIAN_POS = 19,
    PARTICLE_PATH_CONST_MOTION_RELATIVE_CARTESIAN_VEL = 20,
    PARTICLE_PATH_CONST_MOTION_RELATIVE_CARTESIAN_ACC = 21,
    PARTICLE_PATH_CONST_MOTION_ATTRACTOR_VEL = 22,
    PARTICLE_PATH_CONST_MOTION_ATTRACTOR_POS = 23,
};


struct ParticleSystem {  // size 0x360

    ParticleSystemID id;  // 0x00
    ParticleSystemType eType;  // 0x04
    uint32 dwFlags;  // 0x08
    uint32 dwDefFlags;  // 0x0c
    SNO snoParticle;  // 0x10
    SNO snoShader;  // 0x14
    int32 eRenderLayer;  // 0x18 RenderLayer
    uint32 dwInterpSeed;  // 0x1c
    int32 eContext;  // 0x20 RenderContext
    ParticleTextures * ptParticleTextures;  // 0x28
    Sphere wsBounds;  // 0x30

    Vector3D wp;  // 0x40
    Vector3D wpStart;  // 0x4c
    Vector3D wpLast;  // 0x58
    Quaternion qOrientation;  // 0x64

    PhysicsSystem tPhysicsSystem;  // 0x78


    ParentData tParentData;  // 0xb8
    float flSeconds;  // 0xe4
    float flDuration;  // 0xe8
    BOOL bOutro;  // 0xec
    float flOutroSeconds;  // 0xf0
    float flOutroCrossFade;  // 0xf4
    InterpTime tInterpTime;  // 0xf8
    float flMsgScalar;  // 0x124

    float flLastParentAlpha;  // 0x128
    float flAlpha;  // 0x12c

    float fSpawnRateAccumulator;  // 0x130

    float flScaleFrac;  // 0x134

    float flAlphaRadialMin;  // 0x138
    float flAlphaRadialMax;  // 0x13c
    float flAlphaHeightMin;  // 0x140
    float flAlphaHeightMax;  // 0x144

    uint32 dwSubObjectEmitterIndex;  // 0x148
    uint32 dwEmitterTriIndex;  // 0x14c
    float flEmitterRadiusMax;  // 0x150
    float flEmitterHeightMax;  // 0x154
    RActorID idRActorAttractedTo;  // 0x158
    int32 nAttractorHardpointIndex;  // 0x15c
    Vector3D wpAttractor;  // 0x160

    ParticleList tParticleList;  // 0x170
    RActorIDListPooled tParticleRActorList;  // 0x188
    Particle ** arCameraSortedParticles;  // 0x1b0

    SNO snoActor;  // 0x1b8
    BOOL fIsFlippy;  // 0x1bc
    RActorID idCapturedRActor;  // 0x1c0

    uint32 dwParticleLastSeen;  // 0x1c4
    SSceneID idSScene;  // 0x1c8
    InactiveMarkerOwner tMarkerOwner;  // 0x1cc
    InactiveMarkerReferenceList tInactiveMarkerReferenceList;  // 0x1d8
    int32 nFadeGroupSubObjectIndex;  // 0x200

    RMessageList tRMsgList;  // 0x208
    XPooledDataIDList tAttachmentList;  // 0x240

    float flLastRegisterTime;  // 0x268
    int16 nSimTreeDepth;  // 0x26c

    int8 nDrawCount;  // 0x26e
    float flFoliageUpdateStep;  // 0x270

    RGBAColor rgbaColorMesh;  // 0x274

    BOOL bRemote;  // 0x278


    uint32 dwConstFlags;  // 0x27c

    ParticleConstParams tConstParams;  // 0x280


    blz::vector<ParticleQuad, blz::allocator<ParticleQuad> > tParticleQuads;  // 0x320
    blz::vector<ParticleQuad, blz::allocator<ParticleQuad> > tParticleQuadUVs;  // 0x338


    int32 RegisterParentObject();


    BOOL PreThreadSafeUpdate(float flDeltaTime);


    void ThreadSafeUpdate(float flDeltaTime);


    void PostThreadSafeUpdate(float flDeltaTime);


    void Update(float flDeltaTime);


    void CreateParticle(const Vector3D & _wp, const ParticleCreateParams * ptCreateParams);



    void RenderInDisplayList(const Camera & cam, DisplayList & tDisplayList, const float flDeltaTime);




    void SetAttractor(const Vector3D & _wpAttractor, const RActorID _idRActorAttractedTo, const HardpointLink * ptHardpointLink);




    void PlayOutro();


    void Detach();


    void SetTransform(const PRTransform & transform);


    BOOL UpdateRainParams(const BOOL, RainParticle & tParticle, Vector3D & wpParticle, ParticleCounts & tParticleCounts, float flDeltaTime);






    void SetParent(const ParentData & _tParentData, const uint32 dwSubobjectEmitterIndex, BOOL fIgnoreOwnerAlpha);




    void UpdatePropertiesFromParent();





    void SendMsgKey(const KeyedMessage & tMsg, const MsgPassMethod eMsgPassMethod);



    void ApplyExplosion(const ExplosionImpulse & tExplosion);


    void SortParticles();


    int32 GetParticleCount() { return (int32)tParticleList.size() + tParticleRActorList.Count(); }



    // Inference: body is not in the DWARF. CPlayer.cpp's inlined copy (call line 3684) has
    // line rows Particle.h:405 col 13 and :407 col 20 around blz::vector size()/operator[].
    Particle * TryToGetFirstParticle()
    {

        if (tParticleList.size() > 0)
        {
            return tParticleList[0];
        }
        return NULL;
    }

    void LockParticleDef();


    void UnlockParticleDef();


    void InitializeParticleDef(const ParticleDefinition * pdefParticle);

    void DisposeParticleDef();


    const ParticleDefinition * GetParticleDef() { return ptParticleDef; }





protected:
    int nLockCount;  // 0x350
    const ParticleDefinition * ptParticleDef;  // 0x358
};
static_assert(sizeof(ParticleSystem) == 0x360, "ParticleSystem");
class ScopedParticleSystemLock {  // size 0x8
public:

    ScopedParticleSystemLock(ParticleSystem * ptParticleSystem) : m_ptParticleSystem(ptParticleSystem) {
        m_ptParticleSystem->LockParticleDef();
    }



    ~ScopedParticleSystemLock() {
        m_ptParticleSystem->UnlockParticleDef();
    }


protected:
    ParticleSystem * m_ptParticleSystem;  // 0x00
};
static_assert(sizeof(ScopedParticleSystemLock) == 0x8, "ScopedParticleSystemLock");
struct ParticleCounts {  // size 0x20

    int32 nSystemCurrentTotal;  // 0x00
    int32 nParticlesCurrentTotal;  // 0x04
    int32 nStaticCount;  // 0x08
    int32 nPeakCount;  // 0x0c
    int32 nParticleVisibleCount;  // 0x10
    int32 nSystemVisibleCount;  // 0x14
    int32 nSystemUpdatedCount;  // 0x18
    int32 nParticleUpdatedCount;  // 0x1c
};
static_assert(sizeof(ParticleCounts) == 0x20, "ParticleCounts");

void ParticleSetLastSeenTime(Frustum & tFrustum);  // Particle.cpp:6306
void ParticleCheckSeenRecently();  // Particle.cpp:6399
ParticleSystem * ParticleSystemGet(int32 idParticleSystem);  // Particle.cpp
ParticleSystem * ParticleSystemTryToGet(int32 idParticleSystem);  // Particle.cpp

// Particle.cpp:1236, 2680, 2692, 2708.
ParticleSystemID ParticleSystemAdd(const ParticleSystemAddParams & tParams);
BOOL ParticleSystemIsMergeable(ParticleSystem * ptParticleSystem);
BOOL ParticleSystemCanMergeMarker(ParticleSystem * ptParticleSystem, ParticleSystem * ptMarkerParticleSystem);
void ParticleSystemMergeMarker(ParticleSystem * ptParticleSystem, ParticleSystem * ptMarkerParticleSystem);
