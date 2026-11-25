// X/Client/Particle.cpp
// Functions sit on their original line numbers (2.6.2 DWARF). Generated from
// build/try/r5-particle/frag.cpp by build/try/r5-particle/gen.py.
#include "RWindow.h"
#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include <algorithm>
#include "Explosions.h"
#include "Gfx/Gfx.h"
#include "Gfx/GfxIB.h"
#include "Gfx/GfxVB.h"
#include "Particle.h"
#include "ParticleUpdate.h"
#include "ParticleInternal.h"
#include "ParticleRenderUpdate.h"
#include "TexAnimation.h"
#include "Attachments.h"
#include "CGame.h"
#include "Camera/Camera.h"
#include "Camera/CameraManager.h"
#include "Prefetch.h"
#include "EffectGroup.h"
#include "Definitions/CAttachmentDefinition.h"
#include "RActors/RActors.h"
#include "RActors/RActorAttachments.h"
#include "Definitions/PhysicsDefinition.h"
#include "Definitions/TextureDefinition.h"
#include "Definitions/ParticleDefinition.h"
#include "../Common/JobManager/JobManager.h"
#include "../Common/GameCommonData.h"
#include "../Common/InactiveMarker.h"
#include "../Common/SceneCommon.h"
#include "../Sigma/DataStructures/ChainedDataArrayTemplated.h"
#include "../Sigma/DataStructures/DataArraysTemplated.h"
#include "../Sigma/DataStructures/XArray.h"
#include "../Sigma/DataStructures/XChainedFixedBlockAllocator.h"
#include "../Sigma/DataStructures/XCommandBuffer.h"
#include "../Sigma/DataStructures/XGrowableMap.h"
#include "../Sigma/DataStructures/XPooledList.h"
#include "../Sigma/DataStructures/XSphereArray.h"
#include "../Sigma/Math/Frustum.h"
#include "../Sigma/Math/Intersect.h"
#include "../Sigma/Math/Vector.h"
#include "../Sigma/MemoryPools.h"
#include "../Sigma/Memory/SigmaMemory.h"
#include "../Common/Game.h"
#include "../Sigma/BlzExt/BlzVectorHelper.h"
#include "CPlayer.h"
#include "Emitter.h"
#include "Scene.h"
#include "../Common/Definitions/SceneDefinition.h"
#include "Wind.h"
#include "../Common/GameConstants.h"
struct TextureDefinition;
struct VertexBillboardParticle;
class XCommandBuffer;
class XSphereArray;

void ParticleSystemDelete(const ParticleSystemID idParticleSystem);
static void sPostThreadSafeUpdateAParticleSystem(ParticleSystem * ptParticleSystem, float flDeltaTime);
static void sDeleteParticle(ParticleSystem * ptParticleSystem, Particle * ptParticle, BOOL fFlagOnly);
static void sDeleteRainParticle(ParticleSystem * ptParticleSystem, RainParticle * ptParticle);
static void sThreadSafeUpdateAParticleSystem(ParticleSystem * ptParticleSystem, DeletePartCallback pDeletePartFn, BOOL fFlagOnly, float flDeltaTime);
float ParticleGetClutterDistance();  // ParticleRenderUpdate.cpp:33
void UpdateDynamicFoliage(ParticleSystem * ptParticleSystem, const Vector3D & wvWind3D, float flOscAmplitude, float flOscAngle, float flDeltaTime);  // ParticleUpdate.cpp:711
int32 UpdateParticlesForSystem(ParticleSystem * ptParticleSystem, DeletePartCallback pDeletePartFn, BOOL fFlagOnly, float & wdSystemRadius, BOOL bUpdateVisibleParams, float flDeltaTime);  // ParticleUpdate.cpp:808
static void sUpdateRainParticlesForSystem(ParticleSystem * ptParticleSystem, float flDeltaTime);
uint32 TexAnimUpdate(const CHAR * pTexAnimCompParams, uint32 dwTexAnimCompParamsSize, const ObjectTexAnimParam * ptObjTexAnimParam, TexAnimState & tTexAnimState, float flDeltaTime, float flClientTime);  // TexAnimation.cpp:316
Intersect::DetailedFrustumResult CameraComputeDetailedSphereVisibility(const Camera & cam, const Sphere & ws);  // Camera/Camera.cpp:145
void RActorGetHardpointTransformByIndex(const RActor & tRActor, int32 nIndex, PRTransform & tTransform);  // RActors/RActorHardpoints
void RActorGetCenterPoint(const RActor * ptRActor, Vector3D & wpCenter);  // RActors/RActors.cpp
static void sParticleSystemMove(ParticleSystem * ptParticleSystem, const PRTransform & transform);  // :1780
static void sParticleSystemRenderUpdate(ParticleSystem * ptParticleSystem, const Camera & cam, DisplayList & dispList, DisplayList * pdispListReflection, DisplayList * pdispListProjected, const Vector3D & wpPlayerPos, BOOL fAnyACDPlayer, const float flDeltaTime);  // :5492
AttachmentID AttachmentSpawn(const RMessage & msg, const ParentData & tParentData, const PRTransform & transform, const float flScale, const BOOL fOverrideScale);  // Attachments.cpp
void RActorChangeLook(const RActorID idRActor, int32 nHashKey);  // RActors.cpp:3191
void RActorCancelLook(const RActorID idRActor, int32 nHashKey);  // RActors.cpp:3236
static BOOL sPreThreadSafeUpdateAParticleSystem(ParticleSystem * ptParticleSystem, BOOL bMustUpdate, float flDeltaTime);  // :2998
const MaterialTextureEntry * MaterialLookup(const UberMaterial & tUberMaterial, ShaderTex eShaderTex);  // Gfx/GfxMaterial.cpp:354
void ParticleSystemUpdateBounds(float & wdSystemRadius, const Vector3D & wpSystemPos, const float flScale, const float flStretch, const Vector3D & wpPos);  // ParticleUpdate.cpp:1175
int32 AppearanceSubObjectFromHash(const SNO snoAppearance, uint32 dwSubObjectHash);  // Definitions/AppearanceDefinition.cpp:4673
static void sParticleParamsHandleDeletion(ParticleParams & tParams, ParticleSystem * ptParticleSystem);
void PointMassDelete(PhysicsSystem & tPhysics, PointMass * ptPointMass);  // Physics/Physics.cpp:113
void ParentObjectRegister(int nDepth, ParentType eType, DataID idObject, void * pObject, ACDID idACD);  // ParentObjects.cpp:723
static void sParticleDestructor(ParticleSystem * ptParticleSystem, Particle * ptParticle);
static void sRainParticleDestructor(ParticleSystem * ptParticleSystem, RainParticle * ptParticle);
BOOL PostFXEnabled();  // PostFX.cpp:2157
void PhysicsSystemUpdate(PhysicsSystem & tPhysics, float flDeltaTime);  // Physics/Physics.cpp:233
float SceneSubObjectGetFade(RActor * ptRActor, SSceneID idSScene, int32 nSubObjectIndex);  // Scene.cpp:3880
static void sCreateParticle(ParticleSystem * ptParticleSystem, Emitter & tEmitter, const ParticleCreateParams * ptCreateParams, float flDeltaTime);
#include "Definitions/ShaderDefinition.h"
#include "Definitions/ShaderMapDefinition.h"
#include "Definitions/AppearanceDefinition.h"
#include "../Common/TagMaps/ShaderMapTags.h"
#include "../Common/TagMaps/Tags.h"
#include "../Sigma/Math/Rect.h"
BOOL gEnableConstPathOptimization;
#include "SubObjectGfx.h"
#include "PostFX.h"
#include "../Common/TagMaps/TagNumbers.h"
#include "../Sigma/Math/Plane.h"
#include "Gfx/GfxPrimitives.h"
enum {
    MAX_PARTICLES = 4096,
    MAX_NORMAL_PARTICLE_SYSTEMS = 256,
    MAX_PARTICLE_SYSTEMS = 544,
    COSMETIC_PARTICLE_SYSTEM_DROP_LIMIT = 128,
    NUM_PARTICLE_BILLBOARD_BUFFERS = 4,
    MAX_PARTICLE_BILLBOARD_VERTICES = 49152,
};
















static const float PARTICLE_MAX_PREPUMP_TIME = 1000.0f;

static const float PARTICLE_SYSTEM_UPDATE_RADIUS = 50.0f;
static const float PARTICLE_MARKER_MAX_DISTANCE_TO_MERGE = 30.0f;
typedef XChainedFixedBlockAllocator<ParticleParams> TParticleParams;
static const ParticleCreateParams scgtDefaultCreateParams = { RGBAColor(0xff, 0xff, 0xff, 0xff), RGBAColor(0xff, 0xff, 0xff, 0xff), 1.0f, FALSE };

struct ParticleTextures {  // size 0x28
    int32 nRefCount;  // 0x00
    BOOL fLocked;  // 0x04
    const TextureDefinition * ptTextures[4];  // 0x08
};

struct ParticleSystemRenderUpdateRecord {  // size 0x1a0
    ParticleSystem * ptParticleSystem;  // 0x00
    VertexBillboardParticle * ptVertices;  // 0x08
    ParticleSystemRenderUpdateParams tParams;  // 0x10
    Vector3D wpPlayer;  // 0xc0
    Frustum tFrustum;  // 0xcc
    Vector3D wvCameraForward;  // 0x12c
    TextureFramesInfo atTexFramesInfo[4];  // 0x138
    float flDeltaTime;  // 0x198
};
static_assert(sizeof(ParticleSystemRenderUpdateRecord) == 0x1a0, "ParticleSystemRenderUpdateRecord");

typedef XArray<ParticleSystemRenderUpdateRecord, const ParticleSystemRenderUpdateRecord &, 256> ParticleSystemRenderUpdateList;

struct ParticleGlobals {  // size 0x1a418
    ParticleCounts tParticleCounts;  // 0x00
    TChainedDataArray<Particle, int> * ptParticles;  // 0x20
    TParticleParams tParticleParams;  // 0x28
    InactiveMarkerReferenceListPool tInactiveMarkerRederenceListPool;  // 0x58
    XGrowableMap<int, int, ParticleTextures, const ParticleTextures &> * ptMapParticleTextures;  // 0x88
    BOOL fInMainParticleRenderUpdate;  // 0x90
    TChainedDataArray<RainParticle, int> * ptRainParticles;  // 0x98
    int32 nRainNumInstances;  // 0xa0
    Vector3D wvRainInstanceOffsets[64];  // 0xa4
    VBID vbid[4];  // 0x3a4
    IBID ibid;  // 0x3b4
    uint32 dwRenderUpdateVertIndex;  // 0x3b8
    uint32 dwRenderUpdateMaxVertices;  // 0x3bc
    VertexBillboardParticle * ptRenderUpdateVBuffer;  // 0x3c0
    uint32 dwRenderUpdateIndex;  // 0x3c8
    BOOL bRenderUpdateVBLocked;  // 0x3cc
    BOOL fKeepSystemAlwaysLocked;  // 0x3d0
    XSphereArray * ptFoliageSphereTree;  // 0x3d8
    XGrowableMap<int, int, unsigned int, unsigned int> * ptMapSimultaneousSystems;  // 0x3e0
    int32 nRenderUpdateJobCount;  // 0x3e8
    ParticleSystemRenderUpdateList sParticleSystemRenderUpdateList;  // 0x3f0
    BOOL bUseClearedSoftParticleDepth[1];  // 0x1a400
    XCommandBuffer * pDisturbFoliageTaskBuffer;  // 0x1a408
    SphereNodeMemoryPool * pDisturbFoliageSphereNodeMemoryPool;  // 0x1a410
};
static_assert(sizeof(ParticleGlobals) == 0x1a418, "ParticleGlobals");

struct ParticleSortNode {  // size 0x10
    Particle * ptParticle;  // 0x00
    float flProjection;  // 0x08
};

struct PosVelFoliageTask {  // size 0x18
    Vector3D wp;  // 0x00
    Vector3D wvVelocity;  // 0x0c
    static void Func(const void * pDataIn);
};

struct ExplosionDisturbFoliageTask {  // size 0x30
    ExplosionImpulse tExplosion;  // 0x00
    static void Func(const void * pDataIn);
};

void ParticlesGfxInit();
void ParticlesGfxDispose();

#include "Definitions/PreferencesDefinition.h"
namespace Prefs {
const VideoPrefs & GetVideoPrefs();  // Preferences.cpp
}  // namespace Prefs
BOOL CameraComputeSphereVisibility(const Camera & cam, const Sphere & ws);  // Camera/Camera.cpp:135
void TextureGetFrameRectUVs(Rect2D & rectTextureUVs, const TextureDefinition * pdefTexture, const uint32 dwFrameIndex);  // Definitions/TextureDefinition.cpp:2752
void PhysicsSystemAwaken(PhysicsSystem & tPhysics, const int32 nInitialRelaxationSteps);  // Physics/Physics.cpp:209
BOOL CSceneGetMeshLighting(const Vector3D & wp, RGBAColor & rgbaColor);  // Scene.cpp:4738
static uint32 sInitializeConstantFlags(const ParticleDefinition * ptParticleDef);  // :6884
static void sParticleParamsConstInitialize(ParticleSystem * ptParticleSystem, ParticleConstParams & tConstParams);  // :4068
static BOOL sParticleSystemIsOnScreen(const ParticleSystem * ptParticleSystem);  // :2345
static void sHandleMsg(const DataID id, RMessage & msg);  // :6227
static RActorID sParticleSystemGetLocalRActor(ParticleSystem * ptParticleSystem);  // :1983
#include "../Common/TagMaps/ActorTags.h"
void TexAnimInitialize(const SNO snoTexture, const CHAR * pTexAnimCompParams, uint32 dwTexAnimCompParamsSize, TexAnimState & tTexAnimState);  // TexAnimation.cpp:254
void EmitterGetRandomPoint(RNG & rng, const Vector3D & wpPointOffset, Emitter & tEmitter, Vector3D & wp, Vector3D & wvNormal, int32 & nBoneIndex);  // Emitter.cpp:100
PointMass * PointMassCreate(PhysicsSystem & tPhysics);  // Physics/Physics.cpp:95
void UpdateGroundNormal(const ParticleDefinition * pdefParticle, const Vector3D & wpParticle, ParticleParams & tParams);  // ParticleRenderUpdate.cpp:68
void ParticlesUpdatePosterOrientation(const BOOL fHasActors, const Vector3D & wvLastMotionNormalized, const Vector3D & wvMotion, Quaternion & qOrientation);  // ParticleRenderUpdate.cpp:418
void UpdateParticleOrientation(ParticleSystem * ptParticleSystem, const ParticleDefinition * pdefParticle, Quaternion & qOrientation, const Vector3D & wvMotion, const Vector3D & wvLastMotionNormalized, const Quaternion & qRelativeOrientation, const Vector3D & wvGroundNormal, const Vector3D & wpParticle, const Vector3D & wvCameraForward);  // ParticleRenderUpdate.cpp:638
static void sParticleCalcInitialVelocity(ParticleSystem * ptParticleSystem, const ParticleDefinition * pdefParticle, ParticleParams & tParams, Vector3D & wvVelocity);  // :3443
static BOOL sParticleParamsInitialize(ParticleSystem * ptParticleSystem, ParticleParams & tParams, Emitter & tEmitter, float flDeltaTime);  // :3495
static BOOL sCreateActorParticle(ParticleSystem * ptParticleSystem, Emitter & tEmitter, const ParticleCreateParams * ptCreateParams, float flDeltaTime);  // :3676
static BOOL sCreateBillboardParticle(ParticleSystem * ptParticleSystem, Emitter & tEmitter, const ParticleCreateParams * ptCreateParams, float flDeltaTime);  // :3910
static BOOL sCreateRainParticle(ParticleSystem * ptParticleSystem, Emitter & tEmitter, float flDeltaTime);  // :4407
void RActorGfxRenderRActorInDisplayList(const Camera &, DisplayList &, DisplayList *, DisplayList *, RActorID, const BOOL, const float, const BOOL); // RActors/RActorGfx.cpp:2777
void JitterRenderedPosition(const ParticleSystem *, const ParticleDefinition *, ParticleParams &, Vector3D &, float); // ParticleRenderUpdate.cpp:114
uint8 UpdateParticleAlpha(ParticleSystem *, const float, const Vector3D &, const Vector3D &); // ParticleRenderUpdate.cpp:202
void RActorSetScale(RActor *, const float, BOOL, const ActorDefinition *); // RActors/RActors.cpp:6366
void RActorSetOrientation(RActor *, const Quaternion &, const ActorDefinition *, const ActorCommonData *); // RActors/RActors.cpp:3616
void ParticleBillboardVerticesSubmit(ParticleSystemRenderUpdateParams &, const Vector3D &, const Quaternion &, VertexBillboardParticle *, const TextureFramesInfo *, const blz::vector<ParticleQuad, blz::allocator<ParticleQuad> > &, const blz::vector<ParticleQuad, blz::allocator<ParticleQuad> > &); // ParticleRenderUpdate.cpp:979
int32 LightsGetDynamic(const RenderContext, const Sphere &, const uint32, const int32, const LightState **, const AABB *, SNO *); // Lights.cpp:629

BOOL CameraComputeAABBVisibility(const Camera & cam, const AABB & aabb); // Camera/Camera.cpp:115

// IVector3D: declared in Sigma/Math/Vector.h (DWARF Vector.h:159).


void GfxAABB3DDrawDeferred(const AABB &, const RGBAColor &, GlobalSNO, GfxLineStyle, const RenderContext); // GfxPrimitives.cpp:457
#include "../Common/ActorCommonData.h"
void DebugTextSubmit(const char *, const Vector3D &, const RWindow &); // DebugTextDraw.cpp:96
#include "Gfx/GfxShader.h"
#include "Gfx/GfxAPI.h"
GfxAPI GfxGetSystemID(); // Gfx.cpp:994
void GfxWorldMatrixSet(const Matrix4x4 &); // Gfx.cpp:2053
void GfxShaderSetPointLinearLightCount(const int32, const ShaderDefinition *, const int32, const LightState *const *); // GfxShader.cpp:1593
void GfxShaderInit(const ShaderDefinition *, const int32); // GfxShader.cpp:409
void GfxShaderSetIB(const IBID, const uint32); // GfxShader.cpp:1553
void GfxShaderSetStream(const StreamIndex, const uint32, const VBID); // GfxShader.cpp:1491
TextureBinding * GfxGetTextureBinding(const ShaderTex); // GfxShader.cpp:868
void GfxShaderSetTexture(const TextureBinding *, const SNO); // GfxShader.cpp:917
void GfxShaderSetLights(const Material &, const ShaderDefinition *, const int32, const LightState *const *); // GfxShader.cpp:1622
void GfxShaderDraw(const DrawMode, const uint32, const uint32, const uint32, const uint32, const uint32, const PerfBin); // GfxShader.cpp:800

// Original indexed API signatures from Cinematography.cpp, CWorld.cpp, Scene.cpp, RandomPoint.cpp.
void CinematographyUpdate(float, BOOL);
WorldPlace ClientPointToPlace(const Vector3D &);
BOOL CScenesGetFloorHeight(SWorldID, const Vector3D &, float &);
void GetRandomVectorPointOnSphere(RNG &, float, Vector3D &);













































void ParticlesInitializeForGame()
{

    CGame & tCGame = CGameGet();

    tCGame.ptParticleGlobals = (ParticleGlobals *)SigmaMemoryNew(sizeof(ParticleGlobals), 4, NULL, TRUE);

    tCGame.ptParticleGlobals->tParticleParams.Initialize();
    tCGame.ptParticleGlobals->tParticleParams.SetBlockCount_Tracked(1000);

    tCGame.ptParticleGlobals->tInactiveMarkerRederenceListPool.Initialize();
    tCGame.ptParticleGlobals->tInactiveMarkerRederenceListPool.SetBlockCount_Tracked(MAX_PARTICLE_SYSTEMS);

    tCGame.ptParticleGlobals->ptParticles = TChainedDataArrayCreateTracked<Particle, ParticleID>("Particles", AppGetCurrentMemoryInterface(), MAX_PARTICLES);

    tCGame.ptParticleGlobals->ptRainParticles = TChainedDataArrayCreateTracked<RainParticle, RainParticleID>("RainParticles", AppGetCurrentMemoryInterface(), MAX_PARTICLES);

    tCGame.ptParticleSystems = TDataArrayCreateTracked<ParticleSystem, ParticleSystemID>("Particle Systems", AppGetCurrentMemoryInterface(), MAX_PARTICLE_SYSTEMS);

    tCGame.ptParticleGlobals->ptFoliageSphereTree = new XSphereArray(NULL);
    tCGame.ptParticleGlobals->ptFoliageSphereTree->Init("FoliageCollisionTree", 512, 0.5f);

    tCGame.ptParticleGlobals->ptMapSimultaneousSystems = new XGrowableMap<int, int, unsigned int, unsigned int>(16);
    tCGame.ptParticleGlobals->ptMapParticleTextures = new XGrowableMap<int, int, ParticleTextures, const ParticleTextures &>(256);
    tCGame.ptParticleGlobals->fInMainParticleRenderUpdate = FALSE;

    tCGame.ptParticleGlobals->sParticleSystemRenderUpdateList.Initialize();
    tCGame.ptParticleGlobals->fKeepSystemAlwaysLocked = TRUE;


    tCGame.ptParticleGlobals->pDisturbFoliageTaskBuffer = new XCommandBuffer("DisturbFoliageTasks", 0x8000);
    tCGame.ptParticleGlobals->pDisturbFoliageSphereNodeMemoryPool = new (SigmaMemoryNew(sizeof(SphereNodeMemoryPool), 4, NULL, FALSE)) SphereNodeMemoryPool;



}



void ParticlesDisposeForGame()
{

    CGame & tCGame = CGameGet();


    delete tCGame.ptParticleGlobals->pDisturbFoliageTaskBuffer;
    tCGame.ptParticleGlobals->pDisturbFoliageSphereNodeMemoryPool->~SphereNodeMemoryPool();
    SigmaMemoryFree(tCGame.ptParticleGlobals->pDisturbFoliageSphereNodeMemoryPool, NULL);
    tCGame.ptParticleGlobals->pDisturbFoliageSphereNodeMemoryPool = NULL;

    delete tCGame.ptParticleGlobals->ptMapParticleTextures;
    delete tCGame.ptParticleGlobals->ptMapSimultaneousSystems;

    tCGame.ptParticleGlobals->ptFoliageSphereTree->Uninit();
    delete tCGame.ptParticleGlobals->ptFoliageSphereTree;

    TDataArrayDispose(tCGame.ptParticleSystems);

    tCGame.ptParticleGlobals->tParticleParams.Dispose();

    tCGame.ptParticleGlobals->tInactiveMarkerRederenceListPool.Dispose();

    TChainedDataArrayDispose(tCGame.ptParticleGlobals->ptParticles);
    TChainedDataArrayDispose(tCGame.ptParticleGlobals->ptRainParticles);

    SigmaMemoryFree(tCGame.ptParticleGlobals, NULL);
    tCGame.ptParticleGlobals = NULL;
}





void sParticlesGfxLockRenderVB()
{
    ParticleGlobals * ptParticleGlobals = CGameGet().ptParticleGlobals;

    VBID vbid = ptParticleGlobals->vbid[ptParticleGlobals->dwRenderUpdateIndex];
    if (!GfxVBIsValid(vbid))
    {
        ParticlesGfxDispose();
        ParticlesGfxInit();
    }
    vbid = ptParticleGlobals->vbid[ptParticleGlobals->dwRenderUpdateIndex];

    ptParticleGlobals->ptRenderUpdateVBuffer = (VertexBillboardParticle *)GfxVBLockAll(vbid);
    ptParticleGlobals->dwRenderUpdateVertIndex = 0;
    ptParticleGlobals->bRenderUpdateVBLocked = TRUE;
}






static void sParticlesGfxUnlockRenderVB()
{
    ParticleGlobals * ptParticleGlobals = CGameGet().ptParticleGlobals;

    if (ptParticleGlobals->bRenderUpdateVBLocked)
    {
        VBID vbid = ptParticleGlobals->vbid[ptParticleGlobals->dwRenderUpdateIndex];
        GfxVBUnlock(vbid);
        ptParticleGlobals->ptRenderUpdateVBuffer = NULL;
        ptParticleGlobals->bRenderUpdateVBLocked = FALSE;
    }
}


























// LINKER-REMOVED: DWARF size 0x5c, ours 0x64 (was 0x58): rows put dwStartIndex = idx at 445:22 before the buffer test, 454 with no reload (+ dwStartIndex), 461:52 reloading; ours differs only by two `mov x8, xzr; mov x0, x8` NULL-return copies (register choice). Nesting inferred from columns.
VertexBillboardParticle * sParticlesGfxVBBeginAlloc(
    uint32 dwVertexCount,
    uint32 & dwStartIndex)
{
    ParticleGlobals * ptParticleGlobals = CGameGet().ptParticleGlobals;

    if (ptParticleGlobals->dwRenderUpdateVertIndex + dwVertexCount < ptParticleGlobals->dwRenderUpdateMaxVertices)
    {
        dwStartIndex = ptParticleGlobals->dwRenderUpdateVertIndex;


        if (ptParticleGlobals->ptRenderUpdateVBuffer == NULL)
            return NULL;



        {
            VertexBillboardParticle * ptVertices = ptParticleGlobals->ptRenderUpdateVBuffer + dwStartIndex;






        ptParticleGlobals->dwRenderUpdateVertIndex += dwVertexCount;
            return ptVertices;
        }
    }




    return NULL;
}

















void ParticlesGfxInit()
{
    ParticleGlobals * ptParticleGlobals = CGameGet().ptParticleGlobals;

    for (int i = 0; i < NUM_PARTICLE_BILLBOARD_BUFFERS; i++)
    {
        VBID vbid;
        BOOL fSuccess = GfxVBAllocTracked(RESOURCEUSAGE_STATIC, VBFORMAT_BILLBOARD_PARTICLE, MAX_PARTICLE_BILLBOARD_VERTICES, TRUE, NULL, vbid, "", 0, "");
        (void)fSuccess;
        ptParticleGlobals->vbid[i] = vbid;
    }

    ptParticleGlobals->dwRenderUpdateIndex = 0;
    ptParticleGlobals->bRenderUpdateVBLocked = FALSE;
    ptParticleGlobals->dwRenderUpdateVertIndex = 0;
    ptParticleGlobals->dwRenderUpdateMaxVertices = MAX_PARTICLE_BILLBOARD_VERTICES;

    sParticlesGfxLockRenderVB();

    int32 nIndexCount;
    GfxQuadlistIBAlloc(RESOURCEUSAGE_IMMUTABLE, MAX_PARTICLES, FALSE, nIndexCount, ptParticleGlobals->ibid);

    for (int32 worker = 0; worker < 1; worker++)
    {
        ptParticleGlobals->bUseClearedSoftParticleDepth[worker] = FALSE;
    }
}



















void ParticlesGfxDispose()
{
    ParticleGlobals * ptParticleGlobals = CGameGet().ptParticleGlobals;

    sParticlesGfxUnlockRenderVB();

    for (int i = 0; i < NUM_PARTICLE_BILLBOARD_BUFFERS; i++)
    {
        GfxVBRelease(ptParticleGlobals->vbid[i]);
        ptParticleGlobals->vbid[i] = VBID_INVALID;
    }
    GfxIBRelease(ptParticleGlobals->ibid);
}










void ParticleGfxAsyncSwapRender(AsyncRenderDeviceStatus eStatus)
{
    ParticleGlobals * ptParticleGlobals = CGameGet().ptParticleGlobals;
    BOOL bDeviceAvailable = (eStatus == ASYNC_RENDER_DEVICE_AVAILABLE);

    sParticlesGfxUnlockRenderVB();

    ptParticleGlobals->dwRenderUpdateIndex = (ptParticleGlobals->dwRenderUpdateIndex + 1) % NUM_PARTICLE_BILLBOARD_BUFFERS;

    if (bDeviceAvailable)
    {
        sParticlesGfxLockRenderVB();
    }
}










void ParticlesInitializeForWorld()
{
}






void ParticlesDeleteAll()
{


    CGame & tCGame = CGameGet();
    if (tCGame.ptParticleSystems)
    {



        TDataArrayIterator<ParticleSystem> tIt;
        while (tCGame.ptParticleSystems->Iterate(tIt))
        {
            ParticleSystem * ptParticleSystem = tIt.ptElement;
            ParticleSystemDelete(ptParticleSystem->id);
        }

        tCGame.ptParticleSystems->DeleteAll();
    }
}
























void ParticlesDisposeForWorld()
{

    ParticlesDeleteAll();
}




static Particle * sParticleGet(const ParticleID id)
{

    return CGameGet().ptParticleGlobals->ptParticles->ElementGet(id);
}




static Particle * sParticleTryToGet(const ParticleID id)
{
    return CGameGet().ptParticleGlobals->ptParticles->ElementTryToGet(id);
}





Particle * ParticleGet(ParticleID id)
{
    return sParticleGet(id);
}





Particle * ParticleTryToGet(ParticleID id)
{
    return sParticleTryToGet(id);
}





static void sParticleSystemLockTextures(
    ParticleSystem * ptParticleSystem,
    const ParticleDefinition * pdefParticle,
    ParticleSystemRenderUpdateParams & tParams)
{
    ParticleTextures * ptTextures = ptParticleSystem->ptParticleTextures;
    BOOL fLockTextures = !ptTextures->fLocked;

    for (int32 i = 0; i < NUM_BILLBOARD_TEX_STATES; ++i)
    {
        ShaderTex eShaderTex = (i == BTS_DIFFUSE) ? SHADERTEX_DIFFUSE : (i == BTS_ALPHA) ? SHADERTEX_ALPHA : (i == BTS_ALPHA_LAYER_1) ? SHADERTEX_ALPHA_LAYER_1 : SHADERTEX_ALPHA_LAYER_2;

        const MaterialTextureEntry * ptMatTex = MaterialLookup(pdefParticle->tUberMaterial, eShaderTex);
        if (!ptMatTex || ptMatTex->tMatTex.snoTex == SNO_NONE)
            continue;











        tParams.pTexAnimCompParams[i] = ptMatTex->tMatTex.pTexAnimCompParams;
        tParams.dwTexAnimCompParamsSize[i] = ptMatTex->tMatTex.serTexAnimCompParams.dwSize;

        if (fLockTextures)
        {

            // Inference: typed result uses the existing untyped GroupDefinition::LockR API.
            tParams.ptTextureDef[i] = ptTextures->ptTextures[i] = (const TextureDefinition *)gptGroupDefTexture->LockR(ptMatTex->tMatTex.snoTex, FALSE);
        }
        else
        {
            tParams.ptTextureDef[i] = ptTextures->ptTextures[i];
        }
        tParams.nTexStageCount = i + 1;
    }


    if (fLockTextures)
    {
        ptTextures->fLocked = TRUE;
    }
}




void sParticleSystemUnlockTextures(
    ParticleSystem * ptParticleSystem)
{
    CGame & tCGame = CGameGet();
    ParticleTextures * ptTextures = ptParticleSystem->ptParticleTextures;


    if (!tCGame.ptParticleGlobals->fInMainParticleRenderUpdate)
    {
        for (int32 i = 0; i < NUM_BILLBOARD_TEX_STATES; ++i)
        {
            if (ptTextures->ptTextures[i])
                // Inference: the existing GroupDefinition API takes the pointer storage as void*&.
                gptGroupDefTexture->UnlockR((void *&)ptTextures->ptTextures[i]);
        }

        ptTextures->fLocked = FALSE;
    }
}




static void sParticleSystemSetFlags(ParticleSystem * ptParticleSystem, const ParticleDefinition * pdefParticle)
{


    Angle aMinCylinderVel = 0.0f;
    Angle aMaxCylinderVel = 0.0f;
    if (pdefParticle->pathMotionCylinderAngleVel)
    {
        pdefParticle->pathMotionCylinderAngleVel->GetMinMaxValues(aMinCylinderVel, aMaxCylinderVel);
        aMinCylinderVel = aMinCylinderVel * TICKS_PER_SECOND;
        aMaxCylinderVel = aMaxCylinderVel * TICKS_PER_SECOND;
    }

    float fMinCylinderPos = 0.0f;
    float fMaxCylinderPos = 0.0f;
    if (pdefParticle->ptExtendedPath_Console && pdefParticle->ptExtendedPath_Console->pathMotionCylinderRadialPos)
    {
        pdefParticle->ptExtendedPath_Console->pathMotionCylinderRadialPos->GetMinMaxValues(fMinCylinderPos, fMaxCylinderPos);
    }

    float fMinCylinderVel = 0.0f;
    float fMaxCylinderVel = 0.0f;
    if (pdefParticle->pathMotionCylinderRadialVel)
    {
        pdefParticle->pathMotionCylinderRadialVel->GetMinMaxValues(fMinCylinderVel, fMaxCylinderVel);
        fMinCylinderVel = fMinCylinderVel * TICKS_PER_SECOND;
        fMaxCylinderVel = fMaxCylinderVel * TICKS_PER_SECOND;
    }

    if (aMinCylinderVel != 0.0f || aMaxCylinderVel != 0.0f ||
        fMinCylinderPos != 0.0f || fMaxCylinderPos != 0.0f ||
        fMinCylinderVel != 0.0f || fMaxCylinderVel != 0.0f)
    {
        ptParticleSystem->dwFlags |= (1 << PARTICLE_SYSTEM_USES_CYLINDER_MOTION);
    }

    float fMinSpherePos = 0.0f;
    float fMaxSpherePos = 0.0f;
    if (pdefParticle->ptExtendedPath_Console && pdefParticle->ptExtendedPath_Console->pathMotionSphereRadialPos)
    {
        pdefParticle->ptExtendedPath_Console->pathMotionSphereRadialPos->GetMinMaxValues(fMinSpherePos, fMaxSpherePos);
    }

    float fMinSphereVel = 0.0f;
    float fMaxSphereVel = 0.0f;
    if (pdefParticle->ptExtendedPath_Console && pdefParticle->ptExtendedPath_Console->pathMotionSphereRadialVel)
    {
        pdefParticle->ptExtendedPath_Console->pathMotionSphereRadialVel->GetMinMaxValues(fMinSphereVel, fMaxSphereVel);
        fMinSphereVel = fMinSphereVel * TICKS_PER_SECOND;
        fMaxSphereVel = fMaxSphereVel * TICKS_PER_SECOND;
    }

    if (fMinSpherePos != 0.0f || fMaxSpherePos != 0.0f ||
        fMinSphereVel != 0.0f || fMaxSphereVel != 0.0f)
    {
        ptParticleSystem->dwFlags |= (1 << PARTICLE_SYSTEM_USES_SPHERE_MOTION);
    }

    Vector3D wvMinCartesianPos(cgwvNull);
    Vector3D wvMaxCartesianPos(cgwvNull);
    if (pdefParticle->pathMotionCartesianPos)
    {
        pdefParticle->pathMotionCartesianPos->GetMinMaxValues(wvMinCartesianPos, wvMaxCartesianPos);
    }

    Vector3D wvMinCartesianVel(cgwvNull);
    Vector3D wvMaxCartesianVel(cgwvNull);
    if (pdefParticle->pathMotionCartesianVel)
    {
        pdefParticle->pathMotionCartesianVel->GetMinMaxValues(wvMinCartesianVel, wvMaxCartesianVel);
        wvMinCartesianVel = wvMinCartesianVel * TICKS_PER_SECOND;
        wvMaxCartesianVel = wvMaxCartesianVel * TICKS_PER_SECOND;
    }

    Vector3D wvMinCartesianAcc(cgwvNull);
    Vector3D wvMaxCartesianAcc(cgwvNull);
    if (pdefParticle->pathMotionCartesianAcc)
    {
        pdefParticle->pathMotionCartesianAcc->GetMinMaxValues(wvMinCartesianAcc, wvMaxCartesianAcc);
        wvMinCartesianAcc = wvMinCartesianAcc * TICKS_PER_SECOND * TICKS_PER_SECOND;
        wvMaxCartesianAcc = wvMaxCartesianAcc * TICKS_PER_SECOND * TICKS_PER_SECOND;
    }

    if (!(wvMinCartesianPos == cgwvNull) ||
        !(wvMaxCartesianPos == cgwvNull) ||
        !(wvMinCartesianVel == cgwvNull) ||
        !(wvMaxCartesianVel == cgwvNull) ||
        !(wvMinCartesianAcc == cgwvNull) ||
        !(wvMaxCartesianAcc == cgwvNull))
    {
        ptParticleSystem->dwFlags |= (1 << PARTICLE_SYSTEM_USES_CARTESIAN_MOTION);
    }

    Vector3D wvMinRelativeCartesianPos(cgwvNull);
    Vector3D wvMaxRelativeCartesianPos(cgwvNull);
    if (pdefParticle->pathMotionRelativeCartesianPos)
    {
        pdefParticle->pathMotionRelativeCartesianPos->GetMinMaxValues(wvMinRelativeCartesianPos, wvMaxRelativeCartesianPos);
    }

    Vector3D wvMinRelativeCartesianVel(cgwvNull);
    Vector3D wvMaxRelativeCartesianVel(cgwvNull);
    if (pdefParticle->pathMotionRelativeCartesianVel)
    {
        pdefParticle->pathMotionRelativeCartesianVel->GetMinMaxValues(wvMinRelativeCartesianVel, wvMaxRelativeCartesianVel);
        wvMinRelativeCartesianVel = wvMinRelativeCartesianVel * TICKS_PER_SECOND;
        wvMaxRelativeCartesianVel = wvMaxRelativeCartesianVel * TICKS_PER_SECOND;
    }

    Vector3D wvMinRelativeCartesianAcc(cgwvNull);
    Vector3D wvMaxRelativeCartesianAcc(cgwvNull);
    if (pdefParticle->ptExtendedPath_Console && pdefParticle->ptExtendedPath_Console->pathMotionRelativeCartesianAcc)
    {
        pdefParticle->ptExtendedPath_Console->pathMotionRelativeCartesianAcc->GetMinMaxValues(wvMinRelativeCartesianAcc, wvMaxRelativeCartesianAcc);
        wvMinRelativeCartesianAcc = wvMinRelativeCartesianAcc * TICKS_PER_SECOND * TICKS_PER_SECOND;
        wvMaxRelativeCartesianAcc = wvMaxRelativeCartesianAcc * TICKS_PER_SECOND * TICKS_PER_SECOND;
    }

    if (!(wvMinRelativeCartesianPos == cgwvNull) ||
        !(wvMaxRelativeCartesianPos == cgwvNull) ||
        !(wvMinRelativeCartesianVel == cgwvNull) ||
        !(wvMaxRelativeCartesianVel == cgwvNull) ||
        !(wvMinRelativeCartesianAcc == cgwvNull) ||
        !(wvMaxRelativeCartesianAcc == cgwvNull))
    {
        ptParticleSystem->dwFlags |= (1 << PARTICLE_SYSTEM_USES_RELATIVE_CARTESIAN_MOTION);
    }

    float fMinAttractorPos = 0.0f;
    float fMaxAttractorPos = 0.0f;
    if (pdefParticle->ptExtendedPath_Console && pdefParticle->ptExtendedPath_Console->pathMotionAttractorPos)
    {
        pdefParticle->ptExtendedPath_Console->pathMotionAttractorPos->GetMinMaxValues(fMinAttractorPos, fMaxAttractorPos);
    }

    float fMinAttractorVel = 0.0f;
    float fMaxAttractorVel = 0.0f;
    if (pdefParticle->ptExtendedPath_Console && pdefParticle->ptExtendedPath_Console->pathMotionAttractorVel)
    {
        pdefParticle->ptExtendedPath_Console->pathMotionAttractorVel->GetMinMaxValues(fMinAttractorVel, fMaxAttractorVel);
        fMinAttractorVel = fMinAttractorVel * TICKS_PER_SECOND;
        fMaxAttractorVel = fMaxAttractorVel * TICKS_PER_SECOND;
    }

    if (!(fMinAttractorPos == 0.0f &&
          fMaxAttractorPos == 0.0f &&
          fMinAttractorVel == 0.0f &&
          fMaxAttractorVel == 0.0f))
    {
        ptParticleSystem->dwFlags |= (1 << PARTICLE_SYSTEM_USES_ATTRACTOR_MOTION);
    }

    Angle aMinOrientationVel = 0.0f;
    Angle aMaxOrientationVel = 0.0f;
    if (pdefParticle->ptExtendedPath_Console && pdefParticle->ptExtendedPath_Console->pathOrientationVelocity)
    {
        pdefParticle->ptExtendedPath_Console->pathOrientationVelocity->GetMinMaxValues(aMinOrientationVel, aMaxOrientationVel);
        aMinOrientationVel = aMinOrientationVel * TICKS_PER_SECOND;
        aMaxOrientationVel = aMaxOrientationVel * TICKS_PER_SECOND;
    }

    Angle aMinOrientationPos = 0.0f;
    Angle aMaxOrientationPos = 0.0f;
    if (pdefParticle->ptExtendedPath_Console && pdefParticle->ptExtendedPath_Console->pathOrientationPosition)
    {
        pdefParticle->ptExtendedPath_Console->pathOrientationPosition->GetMinMaxValues(aMinOrientationPos, aMaxOrientationPos);
    }

    if (aMinOrientationVel > EPSILON || aMaxOrientationVel > EPSILON ||
        aMinOrientationPos > EPSILON || aMaxOrientationPos > EPSILON)
    {
        ptParticleSystem->dwFlags |= (1 << PARTICLE_SYSTEM_USES_ORIENTATION_MOTION);
    }

    Vector3D wvMinOrientationAxis(cgwvNull);
    Vector3D wvMaxOrientationAxis(cgwvNull);
    if (pdefParticle->pathOrientationAxis)
    {
        pdefParticle->pathOrientationAxis->GetMinMaxValues(wvMinOrientationAxis, wvMaxOrientationAxis);
    }

    if (!(wvMinOrientationAxis == cgwvNull) ||
        !(wvMaxOrientationAxis == cgwvNull))
    {
        ptParticleSystem->dwFlags |= (1 << PARTICLE_SYSTEM_HAS_ORIENTATION_AXIS);
    }

    Angle aMinAngularPos = 0.0f;
    Angle aMaxAngularPos = 0.0f;
    if (pdefParticle->pathAngularPosition)
    {
        pdefParticle->pathAngularPosition->GetMinMaxValues(aMinAngularPos, aMaxAngularPos);
    }


    if (pdefParticle->eType != PSTYPE_FOLIAGE_DYNAMIC && pdefParticle->eType != PSTYPE_CLUTTER_DYNAMIC)
    {

        Angle aMinAngularVel = 0.0f;
        Angle aMaxAngularVel = 0.0f;
        if (pdefParticle->pathAngularVelocity)
        {
            pdefParticle->pathAngularVelocity->GetMinMaxValues(aMinAngularVel, aMaxAngularVel);
            aMinAngularVel = aMinAngularVel * TICKS_PER_SECOND;
            aMaxAngularVel = aMaxAngularVel * TICKS_PER_SECOND;
        }

        if (Math::Abs(aMinAngularPos) > EPSILON || Math::Abs(aMaxAngularPos) > EPSILON ||
            Math::Abs(aMinAngularVel) > EPSILON || Math::Abs(aMaxAngularVel) > EPSILON)
        {
            ptParticleSystem->dwFlags |= (1 << PARTICLE_SYSTEM_USES_SPIN);
        }
    }






























    if (pdefParticle->tUberMaterial.snoShaderMap != SNO_NONE)
    {
        ScopedShaderMapDefinitionLockR pdefShaderMap(pdefParticle->tUberMaterial.snoShaderMap);
        uint32 dwShaderTag = TAG_SHADERMAP_MED_DEFAULT;
        if (TagMapTagExists(pdefShaderMap->hTagMap, dwShaderTag))
        {
            SNO snoShader = TagMapGetSNO(pdefShaderMap->hTagMap, dwShaderTag);
            if (snoShader != SNO_NONE)
            {
                ScopedShaderDefinitionLockR pdefShader(snoShader);
                if (pdefShader->dwRenderLayers & (1 << RENDERLAYER_POSTFX_DISTORTION)) ptParticleSystem->dwFlags |= (1 << PARTICLE_SYSTEM_CAUSES_DISTORTION); else ptParticleSystem->dwFlags &= ~(1 << PARTICLE_SYSTEM_CAUSES_DISTORTION);
            }
        }
    }
}




































































inline bool operator<(const ParticleSortNode & a, const ParticleSortNode & b)
{
    return a.flProjection > b.flProjection;
}





void ParticleSystem::SortParticles()
{
    SigmaMemoryFree(arCameraSortedParticles, NULL);
    arCameraSortedParticles = NULL;

    const int32 nCount = (int32)tParticleList.size();
    if (nCount == 0)
        return;

    int32 nIndex;

    ParticleSortNode * ptSortNodes = (ParticleSortNode *)alloca(nCount * sizeof(ParticleSortNode));



    const Camera & camera = CameraManagerGetCamera(-1);
    Vector3D wvAxis = camera.wvCameraForward;

    nIndex = 0;

    for (int32 pos = 0; pos < nCount; pos++)
    {
        Particle * ptParticle = tParticleList[pos];
        ptSortNodes[pos].ptParticle = ptParticle;
        ptSortNodes[pos].flProjection = Vector3D::DotProduct(wvAxis, ptParticle->wp);
    }




    std::sort(ptSortNodes, ptSortNodes + nCount);


    arCameraSortedParticles = (Particle **)SigmaMemoryNew(nCount * sizeof(Particle *), 4, NULL, FALSE);
    for (int32 i = 0; i < nCount; i++)
    {

        arCameraSortedParticles[i] = ptSortNodes[i].ptParticle;
    }
}



void ParticleSystem::LockParticleDef()
{




}












void ParticleSystem::UnlockParticleDef()
{




}














void ParticleSystem::InitializeParticleDef(const ParticleDefinition * pdefParticle)
{







    if (pdefParticle)
    {
        ptParticleDef = (const ParticleDefinition *)gptGroupDefParticle->LockR((void *)pdefParticle);
    }
    else
    {
        ptParticleDef = (const ParticleDefinition *)gptGroupDefParticle->LockR(snoParticle, FALSE);
    }
}




void ParticleSystem::DisposeParticleDef()
{



    gptGroupDefParticle->UnlockR((void *&)ptParticleDef);
}

















static float sOneNegativeOneToZeroOne(float value)
{


    return 1.0f - (value + 1.0f) * 0.5f;
}
static float sNegativeOneOneToZeroOne(float value)
{



    return (value + 1.0f) * 0.5f;
}



uint32 s_nTotalParticleSystems;

ParticleSystemID ParticleSystemAdd(const ParticleSystemAddParams & tParams)

{
    if (tParams.snoParticle == SNO_NONE)
    {
        return PARTICLESYSTEMID_INVALID;
    }




    CGame & tCGame = CGameGet();
    const ParticleDefinition * pdefParticle = (const ParticleDefinition *)gptGroupDefParticle->LockR(tParams.snoParticle, FALSE);


    if (Prefs::GetVideoPrefs().GetUseLowFXMode() && (pdefParticle->dwFlags & (1 << PARTICLE_DEF_DROP_ON_CONSOLE)))
    {
        gptGroupDefParticle->UnlockR((void *&)pdefParticle);
        return PARTICLESYSTEMID_INVALID;
    }


    if ((pdefParticle->dwFlags & (1 << PARTICLE_DEF_DROP_ON_HIGH_LOAD)) && tCGame.ptParticleSystems->GetTotalCount() >= COSMETIC_PARTICLE_SYSTEM_DROP_LIMIT)
    {
        gptGroupDefParticle->UnlockR((void *&)pdefParticle);
        return PARTICLESYSTEMID_INVALID;
    }



    if (tCGame.ptParticleSystems->GetTotalCount() >= MAX_NORMAL_PARTICLE_SYSTEMS && !(pdefParticle->dwFlags & (1 << PARTICLE_DEF_CRITICAL)))
    {
        gptGroupDefParticle->UnlockR((void *&)pdefParticle);
        return PARTICLESYSTEMID_INVALID;
    }




    if (pdefParticle->dwMaxSystems > 0)
    {
        uint32 dwNumSystems = 0;
        tCGame.ptParticleGlobals->ptMapSimultaneousSystems->Lookup(tParams.snoParticle, dwNumSystems);
        if (dwNumSystems >= pdefParticle->dwMaxSystems && tParams.eContext != RENDERCONTEXT_HOLODECK)
        {
            gptGroupDefParticle->UnlockR((void *&)pdefParticle);
            return PARTICLESYSTEMID_INVALID;
        }
    }

    BOOL fCreateWhenOffScreen = pdefParticle->dwFlags & (1 << PARTICLE_DEF_CREATE_SYSTEM_WHEN_OFFSCREEN);
    switch (pdefParticle->eType)
    {
    case PSTYPE_CLUTTER_BILLBOARD:
    case PSTYPE_CLUTTER_DYNAMIC:
        fCreateWhenOffScreen = TRUE;
        break;
    }







    if (!fCreateWhenOffScreen)
    {
        const Camera & cam = CameraManagerGetCamera(-1);
        Sphere ws;
        ws.Set(tParams.transform.wp, PARTICLE_SYSTEM_UPDATE_RADIUS);
        if (!CameraComputeSphereVisibility(cam, ws))
        {
            gptGroupDefParticle->UnlockR((void *&)pdefParticle);
            return PARTICLESYSTEMID_INVALID;
        }
    }


    ParticleSystemID idParticleSystem = tCGame.ptParticleSystems->ElementNew();

    s_nTotalParticleSystems = tCGame.ptParticleSystems->GetTotalCount();
    if (idParticleSystem == PARTICLESYSTEMID_INVALID)
    {
        gptGroupDefParticle->UnlockR((void *&)pdefParticle);
        return PARTICLESYSTEMID_INVALID;
    }


    ParticleSystem * ptParticleSystem = ParticleSystemGet(idParticleSystem);


    if (pdefParticle->dwMaxSystems > 0)
    {
        uint32 dwNumSystems = 0;
        tCGame.ptParticleGlobals->ptMapSimultaneousSystems->Lookup(tParams.snoParticle, dwNumSystems);
        tCGame.ptParticleGlobals->ptMapSimultaneousSystems->SetAt_Tracked(tParams.snoParticle, dwNumSystems + 1);
    }

    ParticleTextures * ptTextures = tCGame.ptParticleGlobals->ptMapParticleTextures->LookupPtr(tParams.snoParticle);
    if (ptTextures)
    {
        ptTextures->nRefCount++;
    }
    else
    {
        ParticleTextures tTextures;
        tTextures.nRefCount = 1;
        tTextures.fLocked = FALSE;
        for (int32 i = 0; i < NUM_BILLBOARD_TEX_STATES; i++)
        {
            tTextures.ptTextures[i] = NULL;
        }
        tCGame.ptParticleGlobals->ptMapParticleTextures->SetAt_Tracked(tParams.snoParticle, tTextures);
        ptTextures = tCGame.ptParticleGlobals->ptMapParticleTextures->LookupPtr(tParams.snoParticle);
    }
    ptParticleSystem->ptParticleTextures = ptTextures;

    ptParticleSystem->snoParticle = tParams.snoParticle;

    ptParticleSystem->InitializeParticleDef(pdefParticle);


    SNO snoShader = ShaderMapGetDefaultShader(pdefParticle->tUberMaterial.snoShaderMap, TRUE, NULL);
    ptParticleSystem->snoShader = snoShader;



    uint32 nFrameCount = pdefParticle->serParticleQuadStructure.dwSize / sizeof(ParticleQuad);
    if (nFrameCount > 0)
    {

        ptParticleSystem->tParticleQuads.resize(nFrameCount);

        for (uint32 idx = 0; idx < nFrameCount; idx++)
        {
            ptParticleSystem->tParticleQuads[idx] = pdefParticle->ptParticleQuadStructure[idx];
        }


        ptParticleSystem->tParticleQuadUVs.resize(nFrameCount);

        const ShaderTex eShaderTex = SHADERTEX_DIFFUSE;
        const MaterialTextureEntry * ptMatTex = MaterialLookup(pdefParticle->tUberMaterial, eShaderTex);

        SNO textureSno = ptMatTex->tMatTex.snoTex;


        ScopedTextureDefinitionLockR ptTexture(textureSno);

        Rect2D uvs;
        for (uint32 idx = 0; idx < nFrameCount; idx++)
        {
            const ParticleQuad & posQuad = ptParticleSystem->tParticleQuads[idx];
            ParticleQuad & uvQuad = ptParticleSystem->tParticleQuadUVs[idx];
            TextureGetFrameRectUVs(uvs, ptTexture, idx);

            uvQuad.flShrinkMinX = Math::Lerp(uvs.left, uvs.right, sOneNegativeOneToZeroOne(posQuad.flShrinkMinX));
            uvQuad.flShrinkMaxX = Math::Lerp(uvs.left, uvs.right, sNegativeOneOneToZeroOne(posQuad.flShrinkMaxX));

            uvQuad.flShrinkMinY = Math::Lerp(uvs.top, uvs.bottom, 1.0f - sOneNegativeOneToZeroOne(posQuad.flShrinkMinY));
            uvQuad.flShrinkMaxY = Math::Lerp(uvs.top, uvs.bottom, 1.0f - sNegativeOneOneToZeroOne(posQuad.flShrinkMaxY));
        }
    }





    if (snoShader != SNO_NONE)
    {
        ScopedShaderDefinitionLockR pdefShader(snoShader);
        if (pdefShader->nPassCount != 1)
        {




            ptParticleSystem->snoShader = SNO_NONE;
        }
        else
        {
            ptParticleSystem->eRenderLayer = pdefShader->ptPasses[0].eRenderLayer;
        }
    }


    ptParticleSystem->dwDefFlags = pdefParticle->dwFlags;

    ptParticleSystem->flMsgScalar = tParams.flParticleScalar;

    ScopedParticleSystemLock tLock(ptParticleSystem);

    ptParticleSystem->tInactiveMarkerReferenceList.SetPool(&tCGame.ptParticleGlobals->tInactiveMarkerRederenceListPool);

    ptParticleSystem->tParticleRActorList.SetPool(MemoryPoolsGetDATAIDListPool());
    ptParticleSystem->tRMsgList.Initialize(ptParticleSystem->id, sHandleMsg);
    ptParticleSystem->tAttachmentList.SetPool(MemoryPoolsGetDATAIDListPool());

    ptParticleSystem->eType = pdefParticle->eType;
    ptParticleSystem->flLastParentAlpha = 1.0f;

    ptParticleSystem->eContext = tParams.eContext;

    ptParticleSystem->flEmitterRadiusMax = 0.0f;
    ptParticleSystem->flEmitterHeightMax = 0.0f;

    ptParticleSystem->dwConstFlags = sInitializeConstantFlags(ptParticleSystem->GetParticleDef());

    if (ptParticleSystem->eType == PSTYPE_ACTOR ||
        ptParticleSystem->eType == PSTYPE_PHYS_ACTOR ||
        ptParticleSystem->eType == PSTYPE_FOLIAGE_ACTOR)
    {
        ptParticleSystem->dwFlags |= (1 << PARTICLE_SYSTEM_HAS_ACTORS);
    }

    RNG & rng = CGameGlobalsGet().rngLocal;

    ptParticleSystem->dwInterpSeed = Rand_Internal(rng, 0xfffffffd);




















    ptParticleSystem->flSeconds = 0.0f;
    ptParticleSystem->flDuration = (float)pdefParticle->tSystemDuration * OO_TICKS_PER_SECOND;

    {

        InterpTime tInterpTime;
        tInterpTime.Init();
        tInterpTime.tCachedScalarParams.flMsgScalar = ptParticleSystem->flMsgScalar;

        RActorID idRActor = sParticleSystemGetLocalRActor(ptParticleSystem);
        RActor * ptRActor = RActorTryToGet(idRActor);
        if (ptRActor)
        {
            tInterpTime.tCachedScalarParams.InitParamsFromRActor(ptRActor);
        }

        float flFinalScalar;
        if (pdefParticle->tSystemDurationScalar.GetScalar(tInterpTime.tCachedScalarParams, flFinalScalar))
        {
            ptParticleSystem->flDuration = ptParticleSystem->flDuration * flFinalScalar;
        }
    }

    ptParticleSystem->bOutro = FALSE;
    ptParticleSystem->flOutroSeconds = 0.0f;

    ptParticleSystem->qOrientation = tParams.transform.q;

    ptParticleSystem->snoActor = SNO_NONE;
    if (ptParticleSystem->eType == PSTYPE_ACTOR ||
        ptParticleSystem->eType == PSTYPE_PHYS_ACTOR ||
        ptParticleSystem->eType == PSTYPE_FOLIAGE_ACTOR)
    {
        ptParticleSystem->snoActor = (tParams.snoActorOverride != SNO_NONE) ? tParams.snoActorOverride : pdefParticle->snoActor;
    }

    ptParticleSystem->fIsFlippy = tParams.fIsFlippy;
    ptParticleSystem->idCapturedRActor = RACTORID_INVALID;

    ptParticleSystem->dwParticleLastSeen = GameGetServerTime();
    ptParticleSystem->idSScene = SSCENEID_INVALID;
    ptParticleSystem->tMarkerOwner.Initialize();
    ptParticleSystem->nFadeGroupSubObjectIndex = -1;

    sParticleSystemSetFlags(ptParticleSystem, pdefParticle);

    ptParticleSystem->fSpawnRateAccumulator = 0.0f;

    ptParticleSystem->SetParent(tParams.tParentData, (uint32)-1, tParams.fIgnoreOwnerAlpha);

    ptParticleSystem->wp = tParams.transform.wp;

    ptParticleSystem->wsBounds.Set(ptParticleSystem->wp, pdefParticle->wdBoundingRadius);

    ptParticleSystem->bRemote = tParams.bRemote;

    if (pdefParticle->tEmitterParams.eType == EMIT_SCENE_SUBOBJECT_SHELL &&
        ptParticleSystem->tParentData.m_eParentType == PARENT_SCENE)
    {



        const CScene * ptCScene = CSceneGet((CSceneID)ptParticleSystem->tParentData.m_idParent);

        ScopedSceneDefinitionLockR pdefScene(ptCScene->tSCD.snoScene);
        ScopedAppearanceDefinitionLockR pdefAppearance(pdefScene->GetAppearance());
        const GeoSet & tGeoSet = pdefAppearance->tStructure.tGeoSets[0];
        SubObject & tSubObject = tGeoSet.ptSubObjects[ptParticleSystem->dwSubObjectEmitterIndex];

        Sphere ws;
        tSubObject.aabbBounds.GetMinimalBoundingSphere(ws);
        ptParticleSystem->wsBounds.Set(ptCScene->tSCD.transformWorld * ws.wpCenter, ws.wdRadius);
    }

    if (tParams.fHasInitialAttractor)
    {
        ptParticleSystem->wpAttractor = tParams.wpAttractorDest;
    }
    else
    {
        ptParticleSystem->wpAttractor = ptParticleSystem->wp;
    }
    ptParticleSystem->idRActorAttractedTo = RACTORID_INVALID;
    ptParticleSystem->nAttractorHardpointIndex = -1;

    ptParticleSystem->wpStart = ptParticleSystem->wp;
    ptParticleSystem->wpLast = ptParticleSystem->wp;

    ptParticleSystem->flFoliageUpdateStep = 0.0f;
    ptParticleSystem->nDrawCount = 1;

    if (ptParticleSystem->eType == PSTYPE_PHYS_BILLBOARD ||
        ptParticleSystem->eType == PSTYPE_PHYS_ACTOR ||
        ptParticleSystem->eType == PSTYPE_RAIN)
    {
        PhysicsSystemInitialize(ptParticleSystem->tPhysicsSystem, pdefParticle->snoPhysics, RACTORID_INVALID);

        PhysicsSystemAwaken(ptParticleSystem->tPhysicsSystem, FALSE);
    }
    else
    {
        PhysicsSystemInitialize(ptParticleSystem->tPhysicsSystem, SNO_NONE, RACTORID_INVALID);
    }

    if (ptParticleSystem->eType == PSTYPE_FOLIAGE_ACTOR ||
        ptParticleSystem->eType == PSTYPE_FOLIAGE_BILLBOARD ||
        ptParticleSystem->eType == PSTYPE_FOLIAGE_DYNAMIC ||
        (ptParticleSystem->dwDefFlags & (1 << PARTICLE_DEF_MESH_LIT_ON_CREATE)))
    {
        ptParticleSystem->rgbaColorMesh = crgbaBlack;








        CSceneGetMeshLighting(ptParticleSystem->wp, ptParticleSystem->rgbaColorMesh);
    }


    ptParticleSystem->flSeconds = 0.0f;

    BOOL fSystemWasDeleted = FALSE;



    float flPrepumpDuration = (float)pdefParticle->tPrepumpDuration * OO_TICKS_PER_SECOND;
    if (ptParticleSystem->dwDefFlags & (1 << PARTICLE_DEF_PREPUMP_BY_SERVER_CREATION_TIME))
    {
        RActorID idRActor = sParticleSystemGetLocalRActor(ptParticleSystem);
        RActor * ptRActor = RActorTryToGet(idRActor);
        if (ptRActor && ptRActor->dwSActorCreationTick != 0)
        {
            uint32 dwPredictedServerTime = CGameGetPredictedServerTime();
            if (dwPredictedServerTime >= ptRActor->dwSActorCreationTick)
            {
                flPrepumpDuration = (float)(dwPredictedServerTime - ptRActor->dwSActorCreationTick) * OO_TICKS_PER_SECOND;
            }
        }
    }

    sParticleParamsConstInitialize(ptParticleSystem, ptParticleSystem->tConstParams);


    float flPrepumpTime = Math::Min(ptParticleSystem->flDuration - OO_TICKS_PER_SECOND, flPrepumpDuration);
    float flPinPrepumpTime = Math::Pin(flPrepumpTime, OO_TICKS_PER_SECOND, PARTICLE_MAX_PREPUMP_TIME);

    BOOL bStatic = (ptParticleSystem->eType >= PSTYPE_FOLIAGE_ACTOR &&
                    ptParticleSystem->eType <= PSTYPE_CLUTTER_DYNAMIC);








    BOOL bStopWhenOffScreen = (ptParticleSystem->dwDefFlags & (1 << PARTICLE_DEF_DELETE_PARTICLES_WHEN_OFFSCREEN)) && !bStatic;

    while (flPinPrepumpTime > 0.0f)
    {
        fSystemWasDeleted = sPreThreadSafeUpdateAParticleSystem(ptParticleSystem, TRUE, OO_TICKS_PER_SECOND);
        if (fSystemWasDeleted)
        {
            gptGroupDefParticle->UnlockR((void *&)pdefParticle);
            return PARTICLESYSTEMID_INVALID;
        }
        sThreadSafeUpdateAParticleSystem(ptParticleSystem, sDeleteParticle, FALSE, OO_TICKS_PER_SECOND);
        sPostThreadSafeUpdateAParticleSystem(ptParticleSystem, OO_TICKS_PER_SECOND);



        if (bStopWhenOffScreen && !sParticleSystemIsOnScreen(ptParticleSystem))
        {
            break;
        }


        flPinPrepumpTime -= OO_TICKS_PER_SECOND;
    }


    if (ptParticleSystem->eType == PSTYPE_FOLIAGE_DYNAMIC)
    {
        ptParticleSystem->SortParticles();
    }

    ParticleAttachmentsOnMsg(*ptParticleSystem, idParticleSystem, KeyedMessage(PARTICLE_MSGKEY_SYSTEM_CREATE, 0.0f, (Shape::Material)0));

    gptGroupDefParticle->UnlockR((void *&)pdefParticle);


    return idParticleSystem;
}



static void sParticleSystemMoveParticle(
    ParticleSystem * ptParticleSystem,
    Particle * ptParticle,
    const Vector3D & wpSystemLast,
    const Vector3D & wpSystemNew)
{
    ParticleParams * ptParams = &ptParticle->tParams;




    Vector3D wvOldOffset = ptParticle->wp - wpSystemLast;
    Vector3D wpNew = wvOldOffset + wpSystemNew;

    ptParticle->wp = wpNew;
    ptParams->qRelativeOrientation = ptParticleSystem->qOrientation;
}




static void sParticleSystemMoveParticle(
    ParticleSystem * ptParticleSystem,
    Particle * ptParticle,
    const Vector3D & wpSystemLast,
    const Vector3D & wpSystemNew,
    const Quaternion & qChange)
{
    ParticleParams * ptParams = &ptParticle->tParams;




    Vector3D wvOldOffset = ptParticle->wp - wpSystemLast;
    Vector3D wpNew = qChange.Rotate(wvOldOffset) + wpSystemNew;

    ptParticle->wp = wpNew;

    ptParams->qRelativeOrientation = ptParticleSystem->qOrientation;

    Vector3D wvNewMotion = qChange.Rotate(ptParams->wvLastMotionNormalized);
    ptParams->wvLastMotionNormalized = wvNewMotion;
}




static void sParticleSystemMoveRActor(
    ParticleSystem * ptParticleSystem,
    RActorID idRActor,
    const Vector3D & wpSystemLast,
    const Vector3D & wpSystemNew)
{
    ParticleParams * ptParams;
    RActor * ptRActor = RActorTryToGet(idRActor);
    if (!ptRActor)
        return;





    ptParams = ptRActor->ptParticleParams;

    Vector3D wvOldOffset = ptRActor->tLocation.tWorldLocation.transform.wp - wpSystemLast;
    Vector3D wpNew = wvOldOffset + wpSystemNew;

    RActorSnapToPosition(ptRActor, wpNew, SSCENEID_INVALID, NULL, NULL);

    ptParams->qRelativeOrientation = ptParticleSystem->qOrientation;
}




static void sParticleSystemMoveRActor(
    ParticleSystem * ptParticleSystem,
    RActorID idRActor,
    const Vector3D & wpSystemLast,
    const Vector3D & wpSystemNew,
    const Quaternion & qChange)
{
    ParticleParams * ptParams;
    RActor * ptRActor = RActorTryToGet(idRActor);
    if (!ptRActor)
        return;





    ptParams = ptRActor->ptParticleParams;

    Vector3D wvOldOffset = ptRActor->tLocation.tWorldLocation.transform.wp - wpSystemLast;
    Vector3D wpNew = qChange.Rotate(wvOldOffset) + wpSystemNew;

    RActorSnapToPosition(ptRActor, wpNew, SSCENEID_INVALID, NULL, NULL);

    ptParams->qRelativeOrientation = ptParticleSystem->qOrientation;

    Vector3D wvNewMotion = qChange.Rotate(ptParams->wvLastMotionNormalized);
    ptParams->wvLastMotionNormalized = wvNewMotion;
}




static void sParticleSystemMove(
    ParticleSystem * ptParticleSystem,
    const PRTransform & transform)
{
    if (ptParticleSystem->eType == PSTYPE_RAIN)
        return;



    Vector3D wpSystemLast = ptParticleSystem->wp;
    Quaternion qSystemLast = ptParticleSystem->qOrientation;

    ptParticleSystem->wp = transform.wp;
    ptParticleSystem->wsBounds.wpCenter = ptParticleSystem->wp;

    ptParticleSystem->qOrientation = transform.q;

    if (ptParticleSystem->flSeconds == 0.0f)
    {



        ptParticleSystem->wpLast = transform.wp;
    }

    if (ptParticleSystem->eType != PSTYPE_PHYS_BILLBOARD && ptParticleSystem->eType != PSTYPE_PHYS_ACTOR)
    {

        if (ptParticleSystem->dwDefFlags & (1 << PARTICLE_DEF_STAYS_RELATIVE_TO_SYSTEM_BIT))
        {
            BOOL fPosOnly = FALSE;
            if (ptParticleSystem->dwDefFlags & (1 << PARTICLE_DEF_IGNORES_SYSTEM_ORIENTATION_BIT))
            {
                fPosOnly = TRUE;
            }
            else
            {
                fPosOnly = transform.q == qSystemLast;
            }

            if (fPosOnly)
            {
                if ((wpSystemLast - ptParticleSystem->wp).MagnitudeSquared() != 0.0f)
                {
                    if (ptParticleSystem->eType == PSTYPE_ACTOR)
                    {
                        POS pos = ptParticleSystem->tParticleRActorList.GetHeadPosition();
                        while (pos)
                        {
                            RActorID idRActor = (RActorID)ptParticleSystem->tParticleRActorList.GetNext(pos);
                            sParticleSystemMoveRActor(ptParticleSystem, idRActor, wpSystemLast, ptParticleSystem->wp);
                        }
                    }
                    else
                    {
                        int32 nCount = ptParticleSystem->tParticleList.size();
                        for (int32 pos = 0; pos < nCount; pos++)
                        {
                            Particle * ptParticle = ptParticleSystem->tParticleList[pos];
                            sParticleSystemMoveParticle(ptParticleSystem, ptParticle, wpSystemLast, ptParticleSystem->wp);
                        }
                    }
                }
            }
            else
            {
                Quaternion qChange = transform.q * qSystemLast.Conjugate();
                if (ptParticleSystem->eType == PSTYPE_ACTOR)
                {
                    POS pos = ptParticleSystem->tParticleRActorList.GetHeadPosition();
                    while (pos)
                    {
                        RActorID idRActor = (RActorID)ptParticleSystem->tParticleRActorList.GetNext(pos);
                        sParticleSystemMoveRActor(ptParticleSystem, idRActor, wpSystemLast, ptParticleSystem->wp, qChange);
                    }
                }
                else
                {
                    int32 nCount = ptParticleSystem->tParticleList.size();
                    for (int32 pos = 0; pos < nCount; pos++)
                    {
                        Particle * ptParticle = ptParticleSystem->tParticleList[pos];
                        sParticleSystemMoveParticle(ptParticleSystem, ptParticle, wpSystemLast, ptParticleSystem->wp, qChange);
                    }
                }
            }
        }
    }
}




static void sParticleSystemUpdateAttractorPosition(ParticleSystem * ptParticleSystem)
{

    if (ptParticleSystem->idRActorAttractedTo != RACTORID_INVALID)
    {
        const RActor * ptRActor = RActorTryToGet(ptParticleSystem->idRActorAttractedTo);


        if (ptRActor)
        {
            if (ptParticleSystem->nAttractorHardpointIndex != -1)
            {
                PRTransform tTransform;
                RActorGetHardpointTransformByIndex(*ptRActor, ptParticleSystem->nAttractorHardpointIndex, tTransform);
                ptParticleSystem->wpAttractor = tTransform.wp;
            }
            else
            {
                RActorGetCenterPoint(ptRActor, ptParticleSystem->wpAttractor);
            }
        }
    }
}


void ParticleSystem::SetAttractor(const Vector3D & _wpAttractor, const RActorID _idRActorAttractedTo, const HardpointLink * ptHardpointLink)
{
    ScopedParticleSystemLock tLock(this);


    if (eType == PSTYPE_RAIN)
        return;

    wpAttractor = _wpAttractor;
    idRActorAttractedTo = _idRActorAttractedTo;
    nAttractorHardpointIndex = -1;

    if (ptHardpointLink && idRActorAttractedTo != RACTORID_INVALID)
    {




        RActor * ptRActor = RActorTryToGet(idRActorAttractedTo);
        if (ptRActor)
        {
            nAttractorHardpointIndex = HardpointIndexFromName(ptRActor->snoAppearance, ptHardpointLink->tHash);
        }
    }

    sParticleSystemUpdateAttractorPosition(this);


    sParticleSystemMove(this, PRTransform(qOrientation, wp));
}




void ParticleSystem::PlayOutro()
{
    ScopedParticleSystemLock tLock(this);


    ParticleAttachmentsOnMsg(*this, id, KeyedMessage(PARTICLE_MSGKEY_SYSTEM_OUTRO, 0.0f, Shape::DEFAULT_MATERIAL));



    POS pos = tParticleRActorList.GetHeadPosition();
    while (pos)
    {
        RActorID idRActor = (RActorID)tParticleRActorList.GetNext(pos);
        RActor * ptRActor = RActorTryToGet(idRActor);
        if (ptRActor)
        {

            RActorSendMsgKey(idRActor, KeyedMessage(MSGKEY_RESERVED_OUTRO, 0.0f, Shape::DEFAULT_MATERIAL), MSGPASS_TO_ACTORCHAIN);
        }
    }



    if (eType >= PSTYPE_FOLIAGE_ACTOR && eType <= PSTYPE_CLUTTER_DYNAMIC)
    {






        ParticleSystemDelete(id);
    }
    else
    {

        bOutro = TRUE;
    }
}

#line 1970
void ParticleSystem::Detach()
{
    ScopedParticleSystemLock tLock(this);

    ParticleAttachmentsOnMsg(*this, id, KeyedMessage(PARTICLE_MSGKEY_SYSTEM_DETACH, 0.0f, Shape::DEFAULT_MATERIAL));

    tParentData.Init();
}

#line 1983
static RActorID sParticleSystemGetLocalRActor(ParticleSystem * ptParticleSystem)
{
    RActorID idLocalRActor = RACTORID_INVALID;



    if (ParentIsRActor(ptParticleSystem->tParentData.m_eParentType))
    {
        idLocalRActor = RActorGetTopmostOwner((RActorID)ptParticleSystem->tParentData.m_idParent, TRUE, FALSE);
    }

    return idLocalRActor;
}



static BOOL sIsTopmostParentInUI(ParticleSystem * ptParticleSystem)
{


    if (ParentIsRActor(ptParticleSystem->tParentData.m_eParentType))
    {

        RActorID idParent = RActorGetTopmostOwner((RActorID)ptParticleSystem->tParentData.m_idParent, FALSE, TRUE);
        RActor * ptRActor = RActorTryToGet(idParent);
        if (ptRActor)
        {

            return ptRActor->dwFlagsEx & (1 << RACTOREX_FIXED_UPDATE_BIT);
        }
    }

    return FALSE;
}

static BOOL sIsTopmostParentInUI(RActor * ptRActor)
{

    RActorID idParent = RActorGetTopmostOwner(ptRActor->id, FALSE, TRUE);
    RActor * ptRActorParent = RActorTryToGet(idParent);





    return (ptRActorParent ? ptRActorParent : ptRActor)->dwFlagsEx & (1 << RACTOREX_FIXED_UPDATE_BIT);
}




void ParticleSystem::SetParent(const ParentData & _tParentData, const uint32 dwSubobjectEmitterIndex, BOOL fIgnoreOwnerAlpha)
{


    ScopedParticleSystemLock tLock(this);


    if (fIgnoreOwnerAlpha) dwFlags |= (1 << PARTICLE_SYSTEM_DONT_GET_ALPHA_FROM_PARENT); else dwFlags &= ~(1 << PARTICLE_SYSTEM_DONT_GET_ALPHA_FROM_PARENT);

    tParentData = _tParentData;

    if (sIsTopmostParentInUI(this)) dwFlags |= (1 << PARTICLE_SYSTEM_FIXED_UPDATE); else dwFlags &= ~(1 << PARTICLE_SYSTEM_FIXED_UPDATE);

    dwSubObjectEmitterIndex = dwSubobjectEmitterIndex;
    if (dwSubObjectEmitterIndex == (uint32)-1)
    {
        const ParticleDefinition * pdefParticle = GetParticleDef();
        if ((pdefParticle->tEmitterParams.eType == EMIT_ACTOR_SUBOBJECT_SHELL_SEQUENTIAL || pdefParticle->tEmitterParams.eType == EMIT_ACTOR_SUBOBJECT_SHELL) &&

            ParentIsRActor(tParentData.m_eParentType))
        {
            const RActor * ptRActor = RActorTryToGet((RActorID)tParentData.m_idParent);
            if (ptRActor)
            {

                dwSubObjectEmitterIndex = AppearanceSubObjectFromHash(ptRActor->snoAppearance, pdefParticle->tEmitterParams.dwSubObjectHash);
            }
        }



        else if (pdefParticle->tEmitterParams.eType == EMIT_SCENE_SUBOBJECT_SHELL &&
                 tParentData.m_eParentType == PARENT_SCENE)
        {
            const CScene * ptCScene = CSceneTryToGet((CSceneID)tParentData.m_idParent);
            if (ptCScene)
            {
                ScopedSceneDefinitionLockR pdefScene(ptCScene->tSCD.snoScene);

                dwSubObjectEmitterIndex = AppearanceSubObjectFromHash(pdefScene->GetAppearance(), pdefParticle->tEmitterParams.dwSubObjectHash);



            }
        }
    }
    dwEmitterTriIndex = 0;

    if (eType == PSTYPE_BILLBOARD || eType == PSTYPE_FOLIAGE_BILLBOARD || eType == PSTYPE_FOLIAGE_DYNAMIC ||
        eType == PSTYPE_CLUTTER_BILLBOARD || eType == PSTYPE_CLUTTER_DYNAMIC)
    {




        tPhysicsSystem.idLocalRActor = sParticleSystemGetLocalRActor(this);
    }



}




void ParticleSystemLostRActor(RActor * ptRActor)
{

    ParticleSystem * ptParticleSystem = ParticleSystemTryToGet(ptRActor->ptParticleParams->idParticleSystem);
    ParticleGlobals * ptGlobals = CGameGet().ptParticleGlobals;

    if (ptParticleSystem)
    {

        POS posDelete = ptParticleSystem->tParticleRActorList.Find(ptRActor->id);
        ptParticleSystem->tParticleRActorList.RemoveAt(posDelete);
        ptParticleSystem->idCapturedRActor = RACTORID_INVALID;
        if (ptParticleSystem->eType >= PSTYPE_FOLIAGE_ACTOR && ptParticleSystem->eType <= PSTYPE_CLUTTER_DYNAMIC)
        {





            ptGlobals->tParticleCounts.nStaticCount--;
        }


        sParticleParamsHandleDeletion(*ptRActor->ptParticleParams, ptParticleSystem);
    }


    ptGlobals->tParticleParams.Free(ptRActor->ptParticleParams);
    ptRActor->ptParticleParams = NULL;
}

static void sDeleteParticlesFromSystem(ParticleSystem * ptParticleSystem)

{
    if (ptParticleSystem->idCapturedRActor != RACTORID_INVALID &&
        ptParticleSystem->GetParticleCount() > 0)
    {
        return;
    }



    switch (ptParticleSystem->eType)
    {
    case PSTYPE_ACTOR:
    case PSTYPE_PHYS_ACTOR:
    case PSTYPE_FOLIAGE_ACTOR:
    {
        POS pos = ptParticleSystem->tParticleRActorList.GetHeadPosition();
        while (pos)
        {
            RActorID idRActor = (RActorID)ptParticleSystem->tParticleRActorList.GetNext(pos);
            const RActor * ptRActor = RActorTryToGet(idRActor);
            if (ptRActor)
            {



                RActorDelete(idRActor);
            }
        }
        break;
    }
    case PSTYPE_RAIN:
    {

        uint32 nCount = ptParticleSystem->tParticleList.size(); for (uint32 pos = 0; pos < nCount; pos++)
        {
            RainParticle * ptParticle = (RainParticle *)ptParticleSystem->tParticleList[pos];
            sRainParticleDestructor(ptParticleSystem, ptParticle);
        }
        break;
    }
    default:
    {

        uint32 nCount = ptParticleSystem->tParticleList.size(); for (uint32 pos = 0; pos < nCount; pos++)
        {
            Particle * ptParticle = ptParticleSystem->tParticleList[pos];
            sParticleDestructor(ptParticleSystem, ptParticle);
        }
        break;
    }
    }

    ptParticleSystem->tParticleRActorList.RemoveAll();
    ptParticleSystem->tParticleList.clear();
}




void ParticleSystemDelete(const ParticleSystemID idParticleSystem)

{
    CGame & tCGame = CGameGet();
    ParticleSystem * ptParticleSystem = ParticleSystemTryToGet(idParticleSystem);
    if (!ptParticleSystem)
    {
        return;
    }


    {
        ScopedParticleSystemLock tLock(ptParticleSystem);
        ParticleAttachmentsOnMsg(*ptParticleSystem, idParticleSystem, KeyedMessage(PARTICLE_MSGKEY_SYSTEM_DELETE, 0.0f, Shape::DEFAULT_MATERIAL));

        AttachmentListHandleParentDeletion(ptParticleSystem->tAttachmentList);



        sDeleteParticlesFromSystem(ptParticleSystem);

        PhysicsSystemDispose(ptParticleSystem->tPhysicsSystem);

        ptParticleSystem->tRMsgList.Dispose();

        SigmaMemoryFree(ptParticleSystem->arCameraSortedParticles, NULL); ptParticleSystem->arCameraSortedParticles = NULL;

        {
            uint32 dwNumSystems;
            if (tCGame.ptParticleGlobals->ptMapSimultaneousSystems->Lookup(ptParticleSystem->snoParticle, dwNumSystems) && dwNumSystems)
            {


                tCGame.ptParticleGlobals->ptMapSimultaneousSystems->SetAt_Tracked(ptParticleSystem->snoParticle, dwNumSystems - 1);
            }
        }


        ptParticleSystem->ptParticleTextures->nRefCount--;
        if (ptParticleSystem->ptParticleTextures->nRefCount == 0)
        {
            tCGame.ptParticleGlobals->ptMapParticleTextures->RemoveAt(ptParticleSystem->snoParticle);
        }

        ptParticleSystem->DisposeParticleDef();

        ptParticleSystem->tInactiveMarkerReferenceList.RemoveAll();
    }

    tCGame.ptParticleSystems->ElementDeleteByID(idParticleSystem);
    s_nTotalParticleSystems = tCGame.ptParticleSystems->GetTotalCount();
}




static BOOL sDynamicFoliageInView(ParticleSystem * ptParticleSystem)
{





    Vector3D wpPlayer;
    ACDID idACDPlayer = LocalPlayerACDGet();
    ActorCommonData * ptACDPlayer = ACDTryToGet(idACDPlayer);
    if (ptACDPlayer)
    {
        float flClutterDistanceMax = ParticleGetClutterDistance();

        wpPlayer = ptACDPlayer->GetWorldLocation().transform.wp;
        float flDistance = Vector3D::Distance(wpPlayer, ptParticleSystem->wp);
        flDistance -= ptParticleSystem->wsBounds.wdRadius;

        if (flDistance > flClutterDistanceMax)
        {

            return FALSE;
        }
    }

    return TRUE;
}

static int32 sRegisterParentObject(ParticleSystem * ptParticleSystem)
{
    BOOL fParticleFixedUpdate = ((ptParticleSystem->dwDefFlags & (1 << PARTICLE_DEF_USE_FIXED_CLOCK_BIT)) || (ptParticleSystem->dwFlags & (1 << PARTICLE_SYSTEM_FIXED_UPDATE))) && (GameIsPaused() || GameIsSoftPaused());
    float flCurrentTime = fParticleFixedUpdate ? CGameGetRealTime() + CGameGetRealDeltaTime() : CGameGetClientTime() + CGameGetClientDeltaTime();
    if (ptParticleSystem->flLastRegisterTime == flCurrentTime)

        return ptParticleSystem->nSimTreeDepth;

    ptParticleSystem->flLastRegisterTime = flCurrentTime;

    int16 nParentDepth = -1;
    if (ptParticleSystem->tParentData.m_eParentType != PARENT_NONE)

        nParentDepth = ptParticleSystem->tParentData.RegisterParentObject();
    nParentDepth++;

    ptParticleSystem->nSimTreeDepth = nParentDepth;
    ParentObjectRegister(ptParticleSystem->nSimTreeDepth, PARENT_PARTICLE_SYSTEM, ptParticleSystem->id, ptParticleSystem, ACDID_INVALID);


    return ptParticleSystem->nSimTreeDepth;
}




int32 ParticleSystem::RegisterParentObject()
{

    return sRegisterParentObject(this);
}




void ParticlesRegisterParentObjects(BOOL fFixedUpdateOnly)
{

    CGame & tCGame = CGameGet();
    TDataArrayIterator<ParticleSystem> tIt;

    while (tCGame.ptParticleSystems->Iterate(tIt))
    {
        ParticleSystem * ptParticleSystem = tIt.ptElement;
        BOOL fParticleFixedUpdate = (ptParticleSystem->dwDefFlags & (1 << PARTICLE_DEF_USE_FIXED_CLOCK_BIT)) || (ptParticleSystem->dwFlags & (1 << PARTICLE_SYSTEM_FIXED_UPDATE));
        if (!fFixedUpdateOnly || fParticleFixedUpdate)
        {
            sRegisterParentObject(ptParticleSystem);
        }
    }
}




void ParticleGlobalsUpdate()
{
    ParticleGlobals * ptGlobals = CGameGet().ptParticleGlobals;
    ptGlobals->tParticleCounts.nParticleUpdatedCount = 0;
    ptGlobals->tParticleCounts.nSystemUpdatedCount = 0;

    ptGlobals->ptFoliageSphereTree->Update();
}







static BOOL sParticleSystemIsOnScreen(const ParticleSystem * ptParticleSystem)
{

    if (ptParticleSystem->eContext != RENDERCONTEXT_DEFAULT)
    {
        return TRUE;
    }

    if (ptParticleSystem->dwDefFlags & (1 << PARTICLE_DEF_ATTACHED_TO_CAMERA))
    {
        return TRUE;
    }

    const Camera & cam = CameraManagerGetCamera(-1);

    return CameraComputeSphereVisibility(cam, ptParticleSystem->wsBounds);
}





static Intersect::DetailedFrustumResult sParticleSystemDetailedVisibilityCheck(const ParticleSystem * ptParticleSystem)
{

    if (ptParticleSystem->eContext != RENDERCONTEXT_DEFAULT || (ptParticleSystem->dwDefFlags & (1 << PARTICLE_DEF_ATTACHED_TO_CAMERA)))
    {
        return Intersect::INTERSECT_INSIDE;
    }






    const Camera & cam = CameraManagerGetCamera(-1);
    return CameraComputeDetailedSphereVisibility(cam, ptParticleSystem->wsBounds);
}






static void sParticleGetBoxExtents(const ParticleDefinition * pdefParticle, const ParticleSystem * ptParticleSystem, Vector3D & wvMinBoxExtent, Vector3D & wvMaxBoxExtent)
{




    InterpTime tInterpTime;
    tInterpTime.Init();

    tInterpTime.flTime = ptParticleSystem->flSeconds;
    tInterpTime.flScale = ptParticleSystem->flDuration;
    tInterpTime.tCachedScalarParams.flMsgScalar = ptParticleSystem->flMsgScalar;



    tInterpTime.nTimeType = TICK_TIME_LOOPING;

    const EmitterParams * ptEmitParams = &pdefParticle->tEmitterParams;
    Vector3D wvOne(1.0f, 1.0f, 1.0f);

    if (ptEmitParams->pathBoxExtents)
    {
        ptEmitParams->pathBoxExtents->GetInterpolatedValueByTime(cgwvNull, tInterpTime, wvMinBoxExtent);
        (*ptEmitParams->pathBoxExtents).GetInterpolatedValueByTime(wvOne, tInterpTime, wvMaxBoxExtent);
    }
    else
    {
        wvMinBoxExtent.Set(0.0f, 0.0f, 0.0f);
        wvMaxBoxExtent.Set(0.0f, 0.0f, 0.0f);
    }
}




static BOOL sGetSystemInterpTime(ParticleSystem * ptParticleSystem, float flDeltaTime)

{

    InterpTime & tInterpTime = ptParticleSystem->tInterpTime;

    tInterpTime.Init();

    RActorID idRActor = sParticleSystemGetLocalRActor(ptParticleSystem);
    RActor * ptRActor = RActorTryToGet(idRActor);
    if (ptRActor)
    {
        tInterpTime.tCachedScalarParams.InitParamsFromRActor(ptRActor);
    }

    const ParticleDefinition * pdefParticle = ptParticleSystem->GetParticleDef();

    tInterpTime.flTime = ptParticleSystem->flSeconds;
    tInterpTime.flScale = ptParticleSystem->flDuration;
    tInterpTime.tCachedScalarParams.flMsgScalar = ptParticleSystem->flMsgScalar;

    if (ptParticleSystem->bOutro)
    {
        ptParticleSystem->flOutroSeconds += flDeltaTime;

        ptParticleSystem->flOutroCrossFade = ptParticleSystem->flOutroSeconds / ((float)pdefParticle->tOutroDuration * OO_TICKS_PER_SECOND);
        ptParticleSystem->flOutroCrossFade = Math::Pin(ptParticleSystem->flOutroCrossFade, 0.0f, 1.0f);
    }
    else
    {
        ptParticleSystem->flOutroCrossFade = 0.0f;
    }

    if (ptParticleSystem->dwDefFlags & (1 << PARTICLE_DEF_INFINITE_DURATION_BIT))
    {
        if (ptParticleSystem->bOutro)
        {
            if (ptParticleSystem->flOutroSeconds >= (float)pdefParticle->tOutroDuration * OO_TICKS_PER_SECOND)
            {
                return TRUE;
            }
            tInterpTime.flRatio = ptParticleSystem->flOutroCrossFade;
            tInterpTime.flOutro = ptParticleSystem->flOutroCrossFade;
            tInterpTime.nTimeType = TICK_TIME_OUTRO_CROSSFADE;
        }
        else
        {
            tInterpTime.nTimeType = TICK_TIME_LOOPING;
        }
    }
    else
    {

        if (tInterpTime.flTime >= tInterpTime.flScale)
        {
            return TRUE;
        }

        tInterpTime.nTimeType = TICK_TIME_NON_LOOPING;
    }

    return FALSE;
}





static void sGenerateParticles(ParticleSystem * ptParticleSystem, float flDeltaTime)

{

    const ParticleDefinition * pdefParticle = ptParticleSystem->GetParticleDef();
    InterpTime tInterpTimeNoCrossFade;
    tInterpTimeNoCrossFade = ptParticleSystem->tInterpTime;
    tInterpTimeNoCrossFade.flRatio = 1.0f;


    int32 nSpawnCount = 0;
    Emitter tEmitter;
    const EmitterParams * ptEmitParams = &pdefParticle->tEmitterParams;

    tEmitter.eType = ptEmitParams->eType;
    tEmitter.ptPoint.Set(0.0f, 0.0f, 0.0f);
    switch (tEmitter.eType)
    {
    case EMIT_POINT:
        break;

    case EMIT_SPHERE_SHELL:
    case EMIT_DOME_SHELL:

        if (ptEmitParams->pathRadius)
        {
            ptEmitParams->pathRadius->GetMinMaxValuesByTime(tInterpTimeNoCrossFade, tEmitter.wdRadiusMin, ptParticleSystem->flEmitterRadiusMax);
        }
        else
        {
            ptParticleSystem->flEmitterRadiusMax = 0.0f;
            tEmitter.wdRadiusMin = 0.0f;
        }
        tEmitter.wdRadiusDelta = ptParticleSystem->flEmitterRadiusMax - tEmitter.wdRadiusMin;
        break;

    case EMIT_CYLINDER_SHELL:
    case EMIT_EVEN_CYLINDER_SHELL:

        if (ptEmitParams->pathRadius)
        {
            ptEmitParams->pathRadius->GetMinMaxValuesByTime(tInterpTimeNoCrossFade, tEmitter.wdRadiusMin, ptParticleSystem->flEmitterRadiusMax);
        }
        else
        {
            ptParticleSystem->flEmitterRadiusMax = 0.0f;
            tEmitter.wdRadiusMin = 0.0f;
        }
        tEmitter.wdRadiusDelta = ptParticleSystem->flEmitterRadiusMax - tEmitter.wdRadiusMin;

        if (ptEmitParams->pathHeight)
        {
            ptEmitParams->pathHeight->GetMinMaxValuesByTime(tInterpTimeNoCrossFade, tEmitter.wdHeightMin, ptParticleSystem->flEmitterHeightMax);
        }
        else
        {
            ptParticleSystem->flEmitterHeightMax = 0.0f;
            tEmitter.wdHeightMin = 0.0f;
        }
        tEmitter.wdHeightDelta = ptParticleSystem->flEmitterHeightMax - tEmitter.wdHeightMin;
        break;

    case EMIT_ACTOR_SUBOBJECT_SHELL:
    case EMIT_ACTOR_SUBOBJECT_SHELL_SEQUENTIAL:

        if (ParentIsRActor(ptParticleSystem->tParentData.m_eParentType)) { tEmitter.id = ptParticleSystem->tParentData.m_idParent; tEmitter.dwSubObjectIndex = ptParticleSystem->dwSubObjectEmitterIndex; break; }
        tEmitter.id = -1;
        tEmitter.dwSubObjectIndex = (uint32)-1;
        break;




    case EMIT_SCENE_SUBOBJECT_SHELL:




        if (ptParticleSystem->tParentData.m_eParentType == PARENT_SCENE) { tEmitter.id = ptParticleSystem->tParentData.m_idParent; tEmitter.dwSubObjectIndex = ptParticleSystem->dwSubObjectEmitterIndex; break; }
        tEmitter.id = -1;
        tEmitter.dwSubObjectIndex = (uint32)-1;
        break;




    case EMIT_BOX_SHELL:




        sParticleGetBoxExtents(
            pdefParticle,
            ptParticleSystem,
            tEmitter.wvMinBoxExtent,
            tEmitter.wvMaxBoxExtent);
        break;

    default:

        DisplayInternalError(NULL, "", 2592, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }

    float fSpawnRate = 0.0f;
    if (pdefParticle->pathSystemSpawnRate)
    {
        pdefParticle->pathSystemSpawnRate->GetInterpolatedValueByTime(ptParticleSystem->dwInterpSeed, PARTICLE_INTERP_SYSTEM_SPAWN_RATE, tInterpTimeNoCrossFade, fSpawnRate);
        fSpawnRate *= 60.0f;
    }

    ptParticleSystem->fSpawnRateAccumulator += fSpawnRate * flDeltaTime;

    float fSpeedSpawnRate = 0.0f;
    if (pdefParticle->ptExtendedPath_Console && pdefParticle->ptExtendedPath_Console->pathSystemSpeedSpawnRate)
    {
        pdefParticle->ptExtendedPath_Console->pathSystemSpeedSpawnRate->GetInterpolatedValueByTime(ptParticleSystem->dwInterpSeed, PARTICLE_INTERP_SYSTEM_SPEED_SPAWN_RATE, tInterpTimeNoCrossFade, fSpeedSpawnRate);
        fSpeedSpawnRate *= 60.0f;


        if (fSpeedSpawnRate > EPSILON)
        {

            Vector3D wvVelocity = (ptParticleSystem->wp - ptParticleSystem->wpLast) * (1.0f / flDeltaTime);

            float wdSpeed = wvVelocity.Magnitude();


            const float wdMax = 300.0f;
            if (wdSpeed > EPSILON &&
                (!(pdefParticle->dwFlags & (1 << PARTICLE_DEF_SPEED_RELATIVE_SPAWN_DISABLED_AT_MAX_SPEED)) || wdSpeed < wdMax))
            {
                ptParticleSystem->fSpawnRateAccumulator += wdSpeed * (fSpeedSpawnRate * flDeltaTime) * flDeltaTime;
            }
        }
    }
    while (ptParticleSystem->fSpawnRateAccumulator >= 1.0f)
    {
        nSpawnCount++;
        ptParticleSystem->fSpawnRateAccumulator -= 1.0f;
    }


    int32 nTargetCount = 1;
    if (pdefParticle->pathSystemCount)
    {
        pdefParticle->pathSystemCount->GetInterpolatedValueByTime(ptParticleSystem->dwInterpSeed, PARTICLE_INTERP_SYSTEM_COUNT, ptParticleSystem->tInterpTime, nTargetCount);
    }

    int32 nParticleCount = ptParticleSystem->GetParticleCount();
    int32 nDeltaCount = nTargetCount - nParticleCount;

    nSpawnCount = Math::Max(nSpawnCount, nDeltaCount);
    nSpawnCount = Math::Min(nSpawnCount, MAX_PARTICLES - nParticleCount);

    tEmitter.nSpawnCount = nSpawnCount;

    for (int32 i = 0; i < nSpawnCount; i++)
    {
        tEmitter.nCurCount = i;
        if (tEmitter.eType == EMIT_ACTOR_SUBOBJECT_SHELL_SEQUENTIAL)
        {
            tEmitter.dwTriIndex = ptParticleSystem->dwEmitterTriIndex;
            ptParticleSystem->dwEmitterTriIndex++;
        }
        sCreateParticle(ptParticleSystem, tEmitter, NULL, flDeltaTime);
    }
}



// Inline-only in the original (inlined into sPreThreadSafeUpdateAParticleSystem at line 3005); so is SetTransform (line 5973).
void ParticleSystem::UpdatePropertiesFromParent()
{

    if (tParentData.ParentIsValid())
    {




        PRTransform transform; tParentData.GetTransform(transform);

        SetTransform(transform);
    }
}


static void sCreateParticle(ParticleSystem * ptParticleSystem, Emitter & tEmitter, const ParticleCreateParams * ptCreateParams, float flDeltaTime);
BOOL ParticleSystemIsMergeable(ParticleSystem * ptParticleSystem)
{
    if (ptParticleSystem->eType >= PSTYPE_FOLIAGE_BILLBOARD && ptParticleSystem->eType <= PSTYPE_CLUTTER_DYNAMIC)
    {

        return !(ptParticleSystem->dwDefFlags & (1 << PARTICLE_DEF_MESH_LIT_ON_CREATE));
    }
    return FALSE;
}



BOOL ParticleSystemCanMergeMarker(ParticleSystem * ptBaseParticleSystem, ParticleSystem * ptParticleSystemToMerge)
{


    return (ptBaseParticleSystem != ptParticleSystemToMerge) &&
           (ptBaseParticleSystem->eType == ptParticleSystemToMerge->eType) &&
           (ptBaseParticleSystem->idSScene == ptParticleSystemToMerge->idSScene) &&
           ParticleSystemIsMergeable(ptBaseParticleSystem) &&
           !(ptParticleSystemToMerge->dwDefFlags & (1 << PARTICLE_DEF_MESH_LIT_ON_CREATE)) &&
           ((ptBaseParticleSystem->wp - ptParticleSystemToMerge->wp).MagnitudeSquared() < (30.0f * 30.0f)) &&
           (ptBaseParticleSystem->nFadeGroupSubObjectIndex == ptParticleSystemToMerge->nFadeGroupSubObjectIndex);
}




void ParticleSystemMergeMarker(ParticleSystem * ptBaseParticleSystem, ParticleSystem * ptParticleSystemToMerge)


{

    if (ptBaseParticleSystem->tInactiveMarkerReferenceList.Count() == 0)
    {
        InactiveMarkerReference markerSelf;
        markerSelf.tMarkerOwner = ptBaseParticleSystem->tMarkerOwner;
        markerSelf.qOrientation = ptBaseParticleSystem->qOrientation;
        markerSelf.wp = ptBaseParticleSystem->wp;
        markerSelf.wsBounds = ptBaseParticleSystem->wsBounds;
        ptBaseParticleSystem->tInactiveMarkerReferenceList.AddTail_Tracked(markerSelf);
    }

    InactiveMarkerReference markerRef;
    markerRef.tMarkerOwner = ptParticleSystemToMerge->tMarkerOwner;
    markerRef.qOrientation = ptParticleSystemToMerge->qOrientation;
    markerRef.wp = ptParticleSystemToMerge->wp;
    markerRef.wsBounds = ptParticleSystemToMerge->wsBounds;
    ptBaseParticleSystem->tInactiveMarkerReferenceList.AddTail_Tracked(markerRef);


    uint32 nCount = ptParticleSystemToMerge->tParticleList.size(); for (uint32 pos = 0; pos < nCount; pos++)
    {
        Particle * ptParticle = ptParticleSystemToMerge->tParticleList[pos];
        ptParticle->tParams.idParticleSystem = ptBaseParticleSystem->id;

        if (!(ptBaseParticleSystem->dwDefFlags & (1 << PARTICLE_DEF_HEAD_INSERT)))
        {
            ptParticle->nIndex = ptBaseParticleSystem->tParticleList.size();
            ptBaseParticleSystem->tParticleList.push_back(ptParticle);
        }
        else
        {
            ptBaseParticleSystem->tParticleList.insert(
                ptBaseParticleSystem->tParticleList.begin(), ptParticle);
        }
    }
    ptParticleSystemToMerge->tParticleList.clear();

    if (ptBaseParticleSystem->dwDefFlags & (1 << PARTICLE_DEF_HEAD_INSERT))
    {

        int32 nCountLoc = ptBaseParticleSystem->tParticleList.size();
        for (int32 i = 0; i < nCountLoc; i++)
        {
            ptBaseParticleSystem->tParticleList[i]->nIndex = i;
        }
    }


    Vector3D wpMin(0.0f, 0.0f, 0.0f);
    Vector3D wpMax(0.0f, 0.0f, 0.0f);
    BOOL fFirst = TRUE;
    POS pos = ptBaseParticleSystem->tInactiveMarkerReferenceList.GetHeadPosition();
    while (pos)
    {
        InactiveMarkerReference & markerRefLoc = ptBaseParticleSystem->tInactiveMarkerReferenceList.GetNext(pos);
        if (fFirst)
        {
            wpMin = markerRefLoc.wsBounds.wpCenter - Vector3D(markerRefLoc.wsBounds.wdRadius, markerRefLoc.wsBounds.wdRadius, markerRefLoc.wsBounds.wdRadius);
            wpMax = markerRefLoc.wsBounds.wpCenter + Vector3D(markerRefLoc.wsBounds.wdRadius, markerRefLoc.wsBounds.wdRadius, markerRefLoc.wsBounds.wdRadius);
            fFirst = FALSE;
        }
        else
        {
            wpMin.x = Math::Min(wpMin.x, markerRefLoc.wsBounds.wpCenter.x - markerRefLoc.wsBounds.wdRadius);
            wpMin.y = Math::Min(wpMin.y, markerRefLoc.wsBounds.wpCenter.y - markerRefLoc.wsBounds.wdRadius);
            wpMin.z = Math::Min(wpMin.z, markerRefLoc.wsBounds.wpCenter.z - markerRefLoc.wsBounds.wdRadius);

            wpMax.x = Math::Max(wpMax.x, markerRefLoc.wsBounds.wpCenter.x + markerRefLoc.wsBounds.wdRadius);
            wpMax.y = Math::Max(wpMax.y, markerRefLoc.wsBounds.wpCenter.y + markerRefLoc.wsBounds.wdRadius);
            wpMax.z = Math::Max(wpMax.z, markerRefLoc.wsBounds.wpCenter.z + markerRefLoc.wsBounds.wdRadius);
        }
    }

    ptBaseParticleSystem->wsBounds.wdRadius = (wpMax - wpMin).Magnitude();
    ptBaseParticleSystem->wsBounds.wpCenter.x = (wpMax.x + wpMin.x) * 0.5f;
    ptBaseParticleSystem->wsBounds.wpCenter.y = (wpMax.y + wpMin.y) * 0.5f;
    ptBaseParticleSystem->wsBounds.wpCenter.z = (wpMax.z + wpMin.z) * 0.5f;


    if (ptBaseParticleSystem->eType == PSTYPE_FOLIAGE_DYNAMIC)
    {
        ptBaseParticleSystem->SortParticles();
    }


    ParticleSystemDelete(ptParticleSystemToMerge->id);
}





BOOL ParticleSystem::PreThreadSafeUpdate(float flDeltaTime)
{



    if (eType >= PSTYPE_FOLIAGE_ACTOR && eType <= PSTYPE_CLUTTER_DYNAMIC)
    {








        Intersect::DetailedFrustumResult tResult = sParticleSystemDetailedVisibilityCheck(this);
        BOOL bOnScreen = tResult != Intersect::INTERSECT_OUTSIDE;
        if (tResult == Intersect::INTERSECT_PARTIALLY) dwFlags |= (1 << PARTICLE_SYSTEM_PARTIALLY_VISIBLE_BIT); else dwFlags &= ~(1 << PARTICLE_SYSTEM_PARTIALLY_VISIBLE_BIT);


        if (bOnScreen) dwFlags |= (1 << PARTICLE_SYSTEM_VISIBLE_BIT); else dwFlags &= ~(1 << PARTICLE_SYSTEM_VISIBLE_BIT);



        const ParticleDefinition * pdefParticle = GetParticleDef();

        if (!(dwFlags & (1 << PARTICLE_SYSTEM_ALL_TEXTURE_PREFETCHED)))
        {
            BOOL fAllPrefetched = TRUE;
            for (int32 i = 0; i < NUM_BILLBOARD_TEX_STATES; i++)
            {
                ShaderTex eShaderTex = (i == BTS_DIFFUSE) ? SHADERTEX_DIFFUSE : (i == BTS_ALPHA) ? SHADERTEX_ALPHA : (i == BTS_ALPHA_LAYER_1) ? SHADERTEX_ALPHA_LAYER_1 : SHADERTEX_ALPHA_LAYER_2;
                const MaterialTextureEntry * ptMatTex = MaterialLookup(pdefParticle->tUberMaterial, eShaderTex);
                if (ptMatTex)
                {
                    if (!Prefetch::AddToQueue(SNOName(TEXTURE_SNO, ptMatTex->tMatTex.snoTex), 2))
                        fAllPrefetched = FALSE;
                }
            }



            if (fAllPrefetched)
                dwFlags |= (1 << PARTICLE_SYSTEM_ALL_TEXTURE_PREFETCHED);
        }
    }
    else
    {




        BOOL fSystemWasDeleted = sPreThreadSafeUpdateAParticleSystem(this, FALSE, flDeltaTime);
        if (fSystemWasDeleted)
            return FALSE;
    }






    return TRUE;
}














void ParticleSystem::ThreadSafeUpdate(float flDeltaTime)
{

    switch (eType)
    {
    case PSTYPE_FOLIAGE_ACTOR:
    case PSTYPE_FOLIAGE_BILLBOARD:
    case PSTYPE_CLUTTER_BILLBOARD:
        break;

    case PSTYPE_FOLIAGE_DYNAMIC:
    case PSTYPE_CLUTTER_DYNAMIC:

        if (sDynamicFoliageInView(this))
        {
            float flOscAmplitude;
            float flOscAngle;
            Vector3D wvWind3D;
            Wind::GetGlobalWindState().GetOscillatoryParams(wvWind3D, flOscAmplitude, flOscAngle);

            UpdateDynamicFoliage(this, wvWind3D, flOscAmplitude, flOscAngle, flDeltaTime);
        }
        break;

    default:


        BOOL fFlagOnly = TRUE;

        sThreadSafeUpdateAParticleSystem(this, sDeleteParticle, fFlagOnly, flDeltaTime);
        break;
    }
}




void ParticleSystem::PostThreadSafeUpdate(float flDeltaTime)
{


    if (dwFlags & (1 << PARTICLE_SYSTEM_HAS_PARTICLE_TO_DELETE))
    {

        int32 nCount = tParticleList.size();

        for (int32 pos = 0; pos < nCount; pos++)
        {
            Particle * ptParticle = tParticleList[pos];
            if (ptParticle->tParams.eType == PSTYPE_SPECIAL_TO_DELETE)
            {
                ParticleAttachmentsOnMsg(*this, ptParticle->id, KeyedMessage(PARTICLE_MSGKEY_PARTICLE_DELETE, 0.0f, Shape::DEFAULT_MATERIAL));
                sDeleteParticle(this, ptParticle, FALSE);

                pos--;
                nCount--;
            }
        }

        dwFlags &= ~(1 << PARTICLE_SYSTEM_HAS_PARTICLE_TO_DELETE);
    }


    switch (eType)
    {
    case PSTYPE_FOLIAGE_ACTOR:
    case PSTYPE_FOLIAGE_BILLBOARD:
    case PSTYPE_CLUTTER_BILLBOARD:
        break;

    case PSTYPE_FOLIAGE_DYNAMIC: case PSTYPE_CLUTTER_DYNAMIC:
        if (flFoliageUpdateStep == 0.0f)
        {
            flFoliageUpdateStep = flDeltaTime;
        }
        else
        {
            flFoliageUpdateStep += flDeltaTime;
            const float cflUpdateStepsTotal = 0.25f;

            flFoliageUpdateStep = (flFoliageUpdateStep >= cflUpdateStepsTotal) ? 0.0f : flFoliageUpdateStep;
        }
        break;




    default:
        sPostThreadSafeUpdateAParticleSystem(this, flDeltaTime);
        break;
    }
}




void ParticleSystem::Update(float flDeltaTime)
{
    ScopedParticleSystemLock tLock(this);





    if (PreThreadSafeUpdate(flDeltaTime))
    {



        ThreadSafeUpdate(flDeltaTime);
        PostThreadSafeUpdate(flDeltaTime);
    }
}



static BOOL sPreThreadSafeUpdateAParticleSystem(ParticleSystem * ptParticleSystem, BOOL bMustUpdate, float flDeltaTime)



{
    if (ptParticleSystem->tParentData.m_eParentType != PARENT_NONE)
    {
        ptParticleSystem->UpdatePropertiesFromParent();
    }

    BOOL bStatic = FALSE;
    BOOL bOnScreen = TRUE;
    BOOL fSystemWasDeleted = FALSE;


    if ((ptParticleSystem->dwFlags & (1 << PARTICLE_SYSTEM_CAUSES_DISTORTION)) &&
        !PostFXEnabled())
    {
        return FALSE;
    }

    const ParticleDefinition * pdefParticle = ptParticleSystem->GetParticleDef();



























    ptParticleSystem->flSeconds += flDeltaTime;

    fSystemWasDeleted = sGetSystemInterpTime(ptParticleSystem, flDeltaTime); if (fSystemWasDeleted)
    {
        ParticleSystemDelete(ptParticleSystem->id); return TRUE;
    }




    Intersect::DetailedFrustumResult tResult = sParticleSystemDetailedVisibilityCheck(ptParticleSystem);
    bOnScreen = tResult != Intersect::INTERSECT_OUTSIDE;
    /*VIS*/
    if (tResult == Intersect::INTERSECT_PARTIALLY) ptParticleSystem->dwFlags |= (1 << PARTICLE_SYSTEM_PARTIALLY_VISIBLE_BIT); else ptParticleSystem->dwFlags &= ~(1 << PARTICLE_SYSTEM_PARTIALLY_VISIBLE_BIT);
    if (bOnScreen) { ptParticleSystem->dwFlags |= (1 << PARTICLE_SYSTEM_VISIBLE_BIT); bMustUpdate = TRUE; } else ptParticleSystem->dwFlags &= ~(1 << PARTICLE_SYSTEM_VISIBLE_BIT);









    if (ptParticleSystem->dwDefFlags & (1 << PARTICLE_DEF_CREATE_PARTICLES_WHEN_OFFSCREEN))
    {
        bMustUpdate = TRUE;
    }



    bStatic = ptParticleSystem->eType >= PSTYPE_FOLIAGE_ACTOR && ptParticleSystem->eType <= PSTYPE_CLUTTER_DYNAMIC;








    if ((ptParticleSystem->dwDefFlags & (1 << PARTICLE_DEF_DELETE_PARTICLES_WHEN_OFFSCREEN)) && !bOnScreen && !bStatic)
    {


        sDeleteParticlesFromSystem(ptParticleSystem);
    }

    if (bMustUpdate && !(ptParticleSystem->dwFlags & (1 << PARTICLE_SYSTEM_ALL_TEXTURE_PREFETCHED)))
    {
        for (int32 i = 0; i < NUM_BILLBOARD_TEX_STATES; i++)
        {
            ShaderTex eShaderTex = (i == BTS_DIFFUSE) ? SHADERTEX_DIFFUSE : (i == BTS_ALPHA) ? SHADERTEX_ALPHA : (i == BTS_ALPHA_LAYER_1) ? SHADERTEX_ALPHA_LAYER_1 : SHADERTEX_ALPHA_LAYER_2;
            const MaterialTextureEntry * ptMatTex = MaterialLookup(pdefParticle->tUberMaterial, eShaderTex);
            if (ptMatTex)
            {
                if (!Prefetch::AddToQueue(SNOName(TEXTURE_SNO, ptMatTex->tMatTex.snoTex), 2)) bMustUpdate = FALSE;
            }
        }





        if (bMustUpdate)
        {
            ptParticleSystem->dwFlags |= (1 << PARTICLE_SYSTEM_ALL_TEXTURE_PREFETCHED);
        }
    }

    if (!bMustUpdate)
    {
        ptParticleSystem->dwFlags |= (1 << PARTICLE_SYSTEM_UPDATE_WAS_SKIPPED);
        return FALSE;
    }


    if (ptParticleSystem->dwFlags & (1 << PARTICLE_SYSTEM_UPDATE_WAS_SKIPPED))
    {
        ptParticleSystem->wpLast = ptParticleSystem->wp;
        ptParticleSystem->dwFlags &= ~(1 << PARTICLE_SYSTEM_UPDATE_WAS_SKIPPED);
    }

    ParticleGlobals * ptGlobals = CGameGet().ptParticleGlobals;
    ptGlobals->tParticleCounts.nSystemUpdatedCount++;


    if (ptParticleSystem->eType == PSTYPE_PHYS_BILLBOARD || ptParticleSystem->eType == PSTYPE_PHYS_ACTOR || ptParticleSystem->eType == PSTYPE_RAIN)
    {


        PhysicsSystemUpdate(ptParticleSystem->tPhysicsSystem, flDeltaTime);
    }
    else
    {

        sParticleSystemUpdateAttractorPosition(ptParticleSystem);
    }


    if (ptParticleSystem->dwDefFlags & (1 << PARTICLE_DEF_ATTACHED_TO_CAMERA))
    {
        const CameraState & cmState = CameraManagerGetCameraState(-1);
        Vector3D wvForward = cmState.transform.q.Rotate(cgwvZAxis);

        float wdToMainGuy = cmState.wvCameraOffsetVector.Magnitude();

        ptParticleSystem->wp = cmState.transform.wp + wvForward * wdToMainGuy * pdefParticle->fAttachedToCameraDistanceFactor;
        ptParticleSystem->wsBounds.wpCenter = ptParticleSystem->wp;
    }

    if (pdefParticle->pathSystemScale)
    {
        pdefParticle->pathSystemScale->GetInterpolatedValueByTime(ptParticleSystem->dwInterpSeed, PARTICLE_INTERP_SYSTEM_SCALE, ptParticleSystem->tInterpTime, ptParticleSystem->flScaleFrac);
    }
    else
    {
        ptParticleSystem->flScaleFrac = 1.0f;
    }
    if (pdefParticle->pathSystemAlpha)
    {
        pdefParticle->pathSystemAlpha->GetInterpolatedValueByTime(ptParticleSystem->dwInterpSeed, PARTICLE_INTERP_SYSTEM_ALPHA, ptParticleSystem->tInterpTime, ptParticleSystem->flAlpha);
    }
    else
    {
        ptParticleSystem->flAlpha = 1.0f;
    }

    if (!(ptParticleSystem->dwFlags & (1 << PARTICLE_SYSTEM_DONT_GET_ALPHA_FROM_PARENT)))
    {



        if (ptParticleSystem->tParentData.m_eParentType != PARENT_NONE)
        {
            ptParticleSystem->flLastParentAlpha = ptParticleSystem->tParentData.GetAlpha();
        }
        ptParticleSystem->flAlpha *= ptParticleSystem->flLastParentAlpha;
    }

    if (ptParticleSystem->nFadeGroupSubObjectIndex != -1)
    {
        float flFadeGroupAlpha = SceneSubObjectGetFade(NULL, ptParticleSystem->idSScene, ptParticleSystem->nFadeGroupSubObjectIndex);
        ptParticleSystem->flAlpha *= flFadeGroupAlpha;
    }


    if (ptParticleSystem->eType != PSTYPE_CLUTTER_BILLBOARD && ptParticleSystem->eType != PSTYPE_CLUTTER_DYNAMIC)
    {

        sGenerateParticles(ptParticleSystem, flDeltaTime);
    }



    return FALSE;
}




static void sThreadSafeUpdateAParticleSystem(ParticleSystem * ptParticleSystem, DeletePartCallback pDeletePartFn, BOOL fFlagOnly, float flDeltaTime)
{





    if (ptParticleSystem->dwFlags & (1 << PARTICLE_SYSTEM_UPDATE_WAS_SKIPPED))
    {

        return;
    }

    switch (ptParticleSystem->eType)
    {

    case PSTYPE_RAIN:
        sUpdateRainParticlesForSystem(ptParticleSystem, flDeltaTime);
        break;

    case PSTYPE_BILLBOARD:
    case PSTYPE_PHYS_BILLBOARD:
    case PSTYPE_RAIN_RIPPLE:
    {
        ParticleCounts & tParticleCounts = CGameGet().ptParticleGlobals->tParticleCounts;
        tParticleCounts.nParticleUpdatedCount += UpdateParticlesForSystem(ptParticleSystem, pDeletePartFn, fFlagOnly, ptParticleSystem->wsBounds.wdRadius, TRUE, flDeltaTime);
    }
        break;
    }
}

static void sPostThreadSafeUpdateAParticleSystem(ParticleSystem * ptParticleSystem, float flDeltaTime)
{



    if (!(ptParticleSystem->dwFlags & (1 << PARTICLE_SYSTEM_UPDATE_WAS_SKIPPED)))
    {



        ptParticleSystem->wpLast = ptParticleSystem->wp;

        ptParticleSystem->tRMsgList.Update(flDeltaTime);
    }
}


void ParticleSystem::CreateParticle(const Vector3D & _wp, const ParticleCreateParams * ptCreateParams)
{


    const ParticleDefinition * pdefParticle = GetParticleDef();


    Emitter tEmitter;
    tEmitter.eType = EMIT_POINT;
    tEmitter.ptPoint = _wp;
    tEmitter.ptPoint.z += pdefParticle->flClutterZOffset;
    sCreateParticle(this, tEmitter, ptCreateParams, 1.0f / 60.0f);
}




static void sCreateParticle(
    ParticleSystem * ptParticleSystem,
    Emitter & tEmitter,
    const ParticleCreateParams * ptCreateParams,
    float flDeltaTime)
{
    BOOL bResult;

    switch (ptParticleSystem->eType)
    {
    case PSTYPE_RAIN:
        bResult = sCreateRainParticle(ptParticleSystem, tEmitter, flDeltaTime);
        break;

    case PSTYPE_ACTOR:
    case PSTYPE_PHYS_ACTOR:
    case PSTYPE_FOLIAGE_ACTOR:
        bResult = sCreateActorParticle(ptParticleSystem, tEmitter, ptCreateParams, flDeltaTime);
        break;

    default:
        bResult = sCreateBillboardParticle(ptParticleSystem, tEmitter, ptCreateParams, flDeltaTime);
        break;
    }

    if (bResult)
    {
        ParticleGlobals * ptGlobals = CGameGet().ptParticleGlobals;
        ptGlobals->tParticleCounts.nPeakCount = Math::Max(ptGlobals->tParticleCounts.nPeakCount, (int32)ptGlobals->ptParticles->GetTotalCount());

        switch (ptParticleSystem->eType)
        {
        case PSTYPE_FOLIAGE_ACTOR:
        case PSTYPE_FOLIAGE_BILLBOARD:
        case PSTYPE_FOLIAGE_DYNAMIC:
        case PSTYPE_CLUTTER_BILLBOARD: case PSTYPE_CLUTTER_DYNAMIC:
            ptGlobals->tParticleCounts.nStaticCount++;
            break;
        }
    }



}




static void sParticleParamsHandleDeletion(ParticleParams & tParams, ParticleSystem * ptParticleSystem)
{

    ParticleGlobals * ptParticleGlobals = CGameGet().ptParticleGlobals;

    if (tParams.ptPointMass)
    {

        PointMassDelete(ptParticleSystem->tPhysicsSystem, tParams.ptPointMass);
    }

    if (tParams.tFoliageState.ptSphereNode)
    {

        ptParticleGlobals->ptFoliageSphereTree->RemoveSphereNode(tParams.tFoliageState.ptSphereNode);
        tParams.tFoliageState.ptSphereNode = NULL;
    }
}



static void sParticleDestructor(ParticleSystem * ptParticleSystem, Particle * ptParticle)
{





    sParticleParamsHandleDeletion(ptParticle->tParams, ptParticleSystem);

    ParticleGlobals * ptParticleGlobals = CGameGet().ptParticleGlobals;
    ptParticleGlobals->ptParticles->ElementDelete(ptParticle);
    CGame & tCGame = CGameGet();
    s_nTotalParticleSystems = tCGame.ptParticleSystems->GetTotalCount();


    switch (ptParticleSystem->eType)
    {
    case PSTYPE_FOLIAGE_ACTOR:
    case PSTYPE_FOLIAGE_BILLBOARD:
    case PSTYPE_FOLIAGE_DYNAMIC:
    case PSTYPE_CLUTTER_BILLBOARD: case PSTYPE_CLUTTER_DYNAMIC:

        ptParticleGlobals->tParticleCounts.nStaticCount--;
    }
}


static void sRainParticleDestructor(ParticleSystem * ptParticleSystem, RainParticle * ptParticle)
{



    Blizzard::Lock::ScopedMutex mutex(CGameGet().tObjectUpdateGlobalLock);

    if (ptParticle->ptPointMass)
    {

        PointMassDelete(ptParticleSystem->tPhysicsSystem, ptParticle->ptPointMass);
    }


    ParticleGlobals * ptParticleGlobals = CGameGet().ptParticleGlobals;
    ptParticleGlobals->ptRainParticles->ElementDelete(ptParticle);
    CGame & tCGame = CGameGet();
    s_nTotalParticleSystems = tCGame.ptParticleSystems->GetTotalCount();
}

static void sDeleteParticle(ParticleSystem * ptParticleSystem, Particle * ptParticle, BOOL fFlagOnly)
{



    if (fFlagOnly)
    {

        ptParticleSystem->dwFlags |= (1 << PARTICLE_SYSTEM_HAS_PARTICLE_TO_DELETE);
        ptParticle->tParams.eType = PSTYPE_SPECIAL_TO_DELETE;
        return;
    }



    int32 nIndex = ptParticle->nIndex;
    blzext::vector_erase_at(ptParticleSystem->tParticleList, nIndex);
    ptParticle->nIndex = -1;


    int32 nCount = ptParticleSystem->tParticleList.size();
    for (int32 i = nIndex; i < nCount; i++)
    {
        ptParticleSystem->tParticleList[i]->nIndex = i;
    }


    sParticleDestructor(ptParticleSystem, ptParticle);
}


static void sDeleteRainParticle(ParticleSystem * ptParticleSystem, RainParticle * ptParticle)
{



    Blizzard::Lock::ScopedMutex mutex(CGameGet().tObjectUpdateGlobalLock);


    int32 nIndex = ptParticle->nIndex;
    blzext::vector_erase_at(ptParticleSystem->tParticleList, nIndex);
    ptParticle->nIndex = -1;


    int32 nCount = ptParticleSystem->tParticleList.size();
    for (int32 i = nIndex; i < nCount; i++)
    {

        ((RainParticle *)ptParticleSystem->tParticleList[i])->nIndex = i;
    }


    sRainParticleDestructor(ptParticleSystem, ptParticle);
}

static void sParticleCalcInitialVelocity(
    ParticleSystem * ptParticleSystem,
    const ParticleDefinition * pdefParticle,
    ParticleParams & tParams,
    Vector3D & wvVelocity)
{
    RNG & rng = CGameGlobalsGet().rngLocal;


    if (pdefParticle->ptExtendedPath_Console && pdefParticle->ptExtendedPath_Console->pathInitialSourceRelativeVelocity)
    {
        pdefParticle->ptExtendedPath_Console->pathInitialSourceRelativeVelocity->GetInterpolatedValueByTime(tParams.dwInterpSeed, PARTICLE_INTERP_INITIAL_SOURCE_RELATIVE_VEL, ptParticleSystem->tInterpTime, wvVelocity);
        wvVelocity = wvVelocity * 60.0f;
    }
    else
    {
        wvVelocity = cgwvNull;
    }


    Angle aAngSpread = 0.0f;
    if (pdefParticle->pathInitialSourceRelativeSpread)
    {
        pdefParticle->pathInitialSourceRelativeSpread->GetInterpolatedValueByTime(ptParticleSystem->dwInterpSeed, PARTICLE_INTERP_INITIAL_SOURCE_RELATIVE_SPREAD, ptParticleSystem->tInterpTime, aAngSpread);
    }


    if (aAngSpread != 0.0f)
    {
        if (wvVelocity.Magnitude() > EPSILON)
        {
            Angle aAzimuth = RandAngle(rng);
            wvVelocity.Bias(aAngSpread, aAzimuth);
        }
    }


    wvVelocity = ptParticleSystem->qOrientation.Rotate(wvVelocity);


    Vector3D wvWorldVel(cgwvNull);
    if (pdefParticle->ptExtendedPath_Console && pdefParticle->ptExtendedPath_Console->pathInitialWorldRelativeVelocity)
    {
        pdefParticle->ptExtendedPath_Console->pathInitialWorldRelativeVelocity->GetInterpolatedValueByTime(tParams.dwInterpSeed, PARTICLE_INTERP_INITIAL_WORLD_RELATIVE_VEL, ptParticleSystem->tInterpTime, wvWorldVel);
        wvWorldVel = wvWorldVel * 60.0f;
    }

    wvVelocity += wvWorldVel;
}



static BOOL sParticleParamsInitialize(
    ParticleSystem * ptParticleSystem,
    ParticleParams & tParams,
    Emitter & tEmitter,
    float flDeltaTime)
{
    tParams.idParticleSystem = ptParticleSystem->id;




    const ParticleDefinition * pdefParticle = ptParticleSystem->GetParticleDef();
    if (ptParticleSystem->dwDefFlags & (1 << PARTICLE_DEF_ATTRACTED_TO_CAMERA))
    {
        CinematographyUpdate(1.0f / 60.0f, FALSE);

        const Camera & cam = CameraManagerGetCamera(CAMERA_CURRENT);
        ptParticleSystem->wpAttractor = cam.cmState.transform.wp;
    }

    RNG & rng = CGameGlobalsGet().rngLocal;
    tParams.dwInterpSeed = Rand_Internal(rng, 0xfffffffd);
    Vector3D wpAlongSystemPath;
    if (!(ptParticleSystem->dwDefFlags & (1 << PARTICLE_DEF_STAYS_RELATIVE_TO_SYSTEM_BIT)))
    {


        float flRandAlongSystemPath = RandFloat_Internal(rng);
        Math::Lerp(ptParticleSystem->wpLast, ptParticleSystem->wp, flRandAlongSystemPath, wpAlongSystemPath);
    }
    else
    {
        wpAlongSystemPath = ptParticleSystem->wp;
    }

    if (ptParticleSystem->eType == PSTYPE_RAIN_RIPPLE)
    {
        wpAlongSystemPath = cgwpNull;
    }

    Vector3D wvNormal;
    int32 nBoneIndex;
    EmitterGetRandomPoint(rng, wpAlongSystemPath, tEmitter, tParams.wpLast, wvNormal, nBoneIndex);
    if ((ptParticleSystem->dwDefFlags & (1 << PARTICLE_DEF_GROUND_HUG_ON_SPAWN)) && !tParams.tCreateParams.fIgnoreGroundHug)
    {







        WorldPlace tPlaceSnapped = ClientPointToPlace(tParams.wpLast);












        BOOL bSnapped = CScenesGetFloorHeight(tPlaceSnapped.idSWorld, tPlaceSnapped.wp, tPlaceSnapped.wp.z);
        if (!bSnapped)
        {
            return FALSE;
        }




        tParams.wpLast = tPlaceSnapped.wp;
    }

    if (pdefParticle->pathInitialScale)
    {
        pdefParticle->pathInitialScale->GetInterpolatedValueByTime(tParams.dwInterpSeed, PARTICLE_INTERP_INITIAL_SCALE, ptParticleSystem->tInterpTime, tParams.flBaseScale);
    }
    else
    {
        tParams.flBaseScale = 1.0f;
    }

    int32 nDuration;
    if (pdefParticle->pathParticleDuration)
    {
        pdefParticle->pathParticleDuration->GetInterpolatedValueByTime(tParams.dwInterpSeed, PARTICLE_INTERP_PARTICLE_DURATION, ptParticleSystem->tInterpTime, nDuration);
    }
    else
    {
        nDuration = 1;
    }

    tParams.flDuration = nDuration * (1.0f / 60.0f);

    float flSystemSpeedDurationFactor = 0.0f;
    if (pdefParticle->pathSystemSpeedDurationFactor)
    {
        pdefParticle->pathSystemSpeedDurationFactor->GetInterpolatedValueByTime(tParams.dwInterpSeed, PARTICLE_INTERP_SPEED_DURATION_FACTOR, ptParticleSystem->tInterpTime, flSystemSpeedDurationFactor);
    }
    if (flSystemSpeedDurationFactor > EPSILON)
    {

        Vector3D wvVelocity = (ptParticleSystem->wp - ptParticleSystem->wpLast) * (1.0f / flDeltaTime);
        float wdSpeed = wvVelocity.Magnitude();

        float flDurationFactor = 1.0f - wdSpeed * flSystemSpeedDurationFactor * flDeltaTime;
        float flNewDuration = tParams.flDuration * flDurationFactor;
        tParams.flDuration = Math::Pin(flNewDuration, 1.0f / 60.0f, tParams.flDuration);
    }
    if (ptParticleSystem->dwFlags & (1 << PARTICLE_SYSTEM_USES_ORIENTATION_MOTION))
    {
        GetRandomVectorPointOnSphere(rng, 1.0f, tParams.wvOrientationAxis);
    }


    tParams.qRelativeOrientation = ptParticleSystem->qOrientation;
    if (ptParticleSystem->dwFlags & (1 << PARTICLE_SYSTEM_USES_CYLINDER_MOTION))
    {


        Angle aAngle = RandAngle(rng);
        tParams.wvLastCylinderRadial.Set(Math::Cos(aAngle), Math::Sin(aAngle));

    }
    tParams.wvLastMotionNormalized = ptParticleSystem->qOrientation.Rotate(cgwvXAxis);
    if (ptParticleSystem->dwDefFlags & (1 << PARTICLE_DEF_RANDOM_LAUNCH_SPIN_FACING))
    {
        tParams.aAng = RandAngle(rng);
    }





    SNO snoShader = ptParticleSystem->snoShader;
    if (snoShader != SNO_NONE && ptParticleSystem->eRenderLayer == RENDERLAYER_PROJECTED_ADDITIVE)
    {


        Player * ptLocalPlayer = LocalPlayerGet();
        if (GameIsParamFlagSet(GPF_REVERSE_CAMERA_FOR_TEAM_1) && ptLocalPlayer && ptLocalPlayer->eTeam == 3)
        {
            tParams.aAng += PI;
        }
    }
    tParams.ptPointMass = NULL;
    if (ptParticleSystem->eType == PSTYPE_PHYS_BILLBOARD || ptParticleSystem->eType == PSTYPE_PHYS_ACTOR || ptParticleSystem->eType == PSTYPE_RAIN)
    {










        PointMass & pm = *PointMassCreate(ptParticleSystem->tPhysicsSystem);
        tParams.ptPointMass = &pm;
        pm.wp = tParams.wpLast;
        float flMass = Math::Max(pdefParticle->flMass, EPSILON);
        pm.flInvMass = 1.0f / flMass;


        Vector3D wvVelocity;
        sParticleCalcInitialVelocity(ptParticleSystem, pdefParticle, tParams, wvVelocity);
        pm.wv = wvVelocity;
    }
    return TRUE;
}






static BOOL sCreateActorParticle(
    ParticleSystem * ptParticleSystem,
    Emitter & tEmitter,
    const ParticleCreateParams * ptCreateParams,
    float flDeltaTime)
{
    RNG & rng = CGameGlobalsGet().rngLocal;

    if (ptParticleSystem->snoActor == SNO_NONE)
    {
        return FALSE;
    }

    ParticleParams tCreationParams;
    SigmaMemoryClear(&tCreationParams, sizeof(tCreationParams));

    if (ptCreateParams)
    {
        tCreationParams.tCreateParams = *ptCreateParams;
    }
    else
    {
        tCreationParams.tCreateParams = scgtDefaultCreateParams;
    }
    if (!sParticleParamsInitialize(ptParticleSystem, tCreationParams, tEmitter, flDeltaTime))
    {
        return FALSE;
    }
    if ((ptParticleSystem->dwFlags & (1 << PARTICLE_SYSTEM_HAS_ORIENTATION_AXIS)) && (ptParticleSystem->dwConstFlags & (1 << PARTICLE_PATH_CONST_ORIENTATION_AXIS)))
    {

        Vector3D wvRotationAxis(ptParticleSystem->tConstParams.vConstOrientationAxis);
        float wdMagnitude = wvRotationAxis.Normalize();
        if (wdMagnitude > EPSILON)
        {
            tCreationParams.wvOrientationAxis = wvRotationAxis;
        }
    }


    const ParticleDefinition * pdefParticle = ptParticleSystem->GetParticleDef();
    ScopedActorDefinitionLockR pdefActor(ptParticleSystem->snoActor);
    float flMinScale = TagMapGetFloat(pdefActor->hTagMap, TAG_ACTOR_SCALE_MIN);
    float flDeltaScale = TagMapGetFloat(pdefActor->hTagMap, TAG_ACTOR_SCALE_DELTA);
    if (pdefParticle->pathInitialScale)
    {
        pdefParticle->pathInitialScale->GetInterpolatedValueByTime(tCreationParams.dwInterpSeed, PARTICLE_INTERP_INITIAL_SCALE, ptParticleSystem->tInterpTime, tCreationParams.flBaseScale);
    }
    else
    {
        tCreationParams.flBaseScale = 1.0f;
    }
    tCreationParams.flBaseScale *= RandRangeFloat_Internal(CGameGlobalsGet().rngLocal, flMinScale, flDeltaScale);

    tCreationParams.qOrientation = ptParticleSystem->qOrientation;
    if (ptParticleSystem->dwDefFlags & (1 << PARTICLE_DEF_RANDOM_LAUNCH_SPIN_FACING))
    {
        Angle aYaw = RandAngle(rng);

        Quaternion qRotation;
        qRotation.SetRotation(cgwvXAxis, aYaw);

        tCreationParams.qOrientation *= qRotation;
    }

    Vector3D wpSpawn(tCreationParams.wpLast);

    ParticleGlobals * ptGlobals = CGameGet().ptParticleGlobals;
    ParticleCounts & tParticleCounts = ptGlobals->tParticleCounts;
    BOOL fDelete = ParticleSystemUpdateParams(ptParticleSystem, TRUE, PARTICLEID_INVALID, tCreationParams, wpSpawn, FALSE, TRUE, flDeltaTime);
    tParticleCounts.nParticleUpdatedCount++;
    if (fDelete)
    {
        return FALSE;
    }

    const Camera & cam = CameraManagerGetCamera(CAMERA_CURRENT);





    UpdateGroundNormal(pdefParticle, wpSpawn, tCreationParams);


    UpdateParticleOrientation(ptParticleSystem,
        pdefParticle,
        tCreationParams.qOrientation,

        tCreationParams.wvMotion,
        tCreationParams.wvLastMotionNormalized,
        tCreationParams.qRelativeOrientation,



        tCreationParams.wvGroundNormal,
        wpSpawn,

        cam.wvCameraForward);




















    RActorID idRActor = RACTORID_INVALID;

    if (ptParticleSystem->idCapturedRActor == RACTORID_INVALID)
    {









        Vector3D wvVelocity;
        sParticleCalcInitialVelocity(ptParticleSystem, pdefParticle, tCreationParams, wvVelocity);

        Vector3D wvLinearVelocity = wvVelocity * TICKS_PER_SECOND;

        uint32 dwRActorFlags = (1 << RACTOR_ADD_NO_WALK) | (1 << RACTOR_ADD_OVERRIDE_SCALE);
        if (ptParticleSystem->fIsFlippy)
        {
            dwRActorFlags |= (1 << RACTOR_ADD_IS_FLIPPY);
        }







        idRActor = RActorCreate(
            ptParticleSystem->snoActor,
            tCreationParams.flScale,
            -1.0f,
            PRTransform(tCreationParams.qOrientation, wpSpawn),
            dwRActorFlags,
            (RenderContext)ptParticleSystem->eContext,
            SSCENEID_INVALID);
        if (idRActor == RACTORID_INVALID)
        {
            return FALSE;
        }

        RActor * ptRActor = RActorGet(idRActor);
        if (ptRActor->ptRigidBody)
        {
            ptRActor->ptRigidBody->SetCenterOfMassVelocity(wvLinearVelocity);
            ptRActor->ptRigidBody->SetAngularVelocity(cgwvNull);
        }
    }
    else
    {
        if (ptParticleSystem->GetParticleCount() > 0)
        {
            return FALSE;
        }

        idRActor = ptParticleSystem->idCapturedRActor;

        if (!RActorTryToGet(idRActor))
        {
            return FALSE;
        }
    }


    RActor * ptRActor = RActorGet(idRActor);

    ptRActor->ptParticleParams = ptGlobals->tParticleParams.Alloc_Tracked();
    SigmaMemoryCopy(ptRActor->ptParticleParams, &tCreationParams, sizeof(ParticleParams));

    if (sIsTopmostParentInUI(ptRActor)) ptRActor->dwFlagsEx |= (1 << RACTOREX_FIXED_UPDATE_BIT); else ptRActor->dwFlagsEx &= ~(1 << RACTOREX_FIXED_UPDATE_BIT);

    ptParticleSystem->tParticleRActorList.AddTail_Tracked(idRActor);

    ptRActor->rgbaColor = tCreationParams.rgbaColor;

    ptRActor->flAlpha = tCreationParams.flAlpha;

    if (ptParticleSystem->dwDefFlags & (1 << PARTICLE_DEF_ORIENTATION_SPIN_AROUND_OBJECT_CENTER))
    {
        ptRActor->dwFlags |= (1 << RACTOR_OFFSET_CENTER_BIT);
    }

    ptRActor->ptParticleParams->eType = ptParticleSystem->eType;

    ParticleAttachmentsOnMsg(*ptParticleSystem, idRActor, KeyedMessage(PARTICLE_MSGKEY_PARTICLE_CREATE, 0.0f, Shape::DEFAULT_MATERIAL));

    return TRUE;
}

























static BOOL sCreateBillboardParticle(
    ParticleSystem * ptParticleSystem,
    Emitter & tEmitter,
    const ParticleCreateParams * ptCreateParams,
    float flDeltaTime)
{
    CGame & tCGame = CGameGet();

    ParticleID idParticle = tCGame.ptParticleGlobals->ptParticles->ElementNew();
    if (idParticle == PARTICLEID_INVALID)
    {
        return FALSE;
    }









    Particle * ptParticle = sParticleGet(idParticle);

    if (!(ptParticleSystem->dwDefFlags & (1 << PARTICLE_DEF_HEAD_INSERT)))
    {
        ptParticle->nIndex = ptParticleSystem->tParticleList.size();
        ptParticleSystem->tParticleList.push_back(ptParticle);
    }
    else
    {
        ptParticle->nIndex = 0;
        ptParticleSystem->tParticleList.insert(ptParticleSystem->tParticleList.begin(), ptParticle);


        int32 nCount = ptParticleSystem->tParticleList.size();
        for (int32 i = 1; i < nCount; i++)
        {
            ptParticleSystem->tParticleList[i]->nIndex = i;
        }
    }

    ptParticle->idParticleSystem = ptParticleSystem->id;

    ParticleParams & tParams = ptParticle->tParams;
    if (ptCreateParams)
    {
        tParams.tCreateParams = *ptCreateParams;
    }
    else
    {
        tParams.tCreateParams = scgtDefaultCreateParams;
    }
    if (!sParticleParamsInitialize(ptParticleSystem, tParams, tEmitter, flDeltaTime))
    {
        sDeleteParticle(ptParticleSystem, ptParticle, FALSE);
        return FALSE;
    }
    if ((ptParticleSystem->dwFlags & (1 << PARTICLE_SYSTEM_HAS_ORIENTATION_AXIS)) && (ptParticleSystem->dwConstFlags & (1 << PARTICLE_PATH_CONST_ORIENTATION_AXIS)))
    {

        Vector3D wvRotationAxis(ptParticleSystem->tConstParams.vConstOrientationAxis);
        float wdMagnitude = wvRotationAxis.Normalize();
        if (wdMagnitude > EPSILON)
        {
            tParams.wvOrientationAxis = wvRotationAxis;
        }
    }

    ptParticle->wp = tParams.wpLast;
    tParams.qOrientation = ptParticleSystem->qOrientation;

    const ParticleDefinition * pdefParticle = ptParticleSystem->GetParticleDef();

    if (pdefParticle->nFacing == PARTICLE_FACING_UP)
    {

        tParams.qOrientation = cgqIdentity;
    }

    if (ptParticleSystem->dwFlags & (1 << PARTICLE_SYSTEM_USES_ORIENTATION_MOTION))
    {


        const Camera & tCamera = CameraManagerGetCamera(CAMERA_CURRENT);

        BOOL fHasActors = (ptParticleSystem->dwFlags >> PARTICLE_SYSTEM_HAS_ACTORS) & 1;
        ParticlesUpdatePosterOrientation(fHasActors, tParams.wvLastMotionNormalized, -tCamera.wvCameraForward, tParams.qOrientation);
    }

    for (int32 i = 0; i < NUM_BILLBOARD_TEX_STATES; i++)
    {
        ShaderTex eShaderTex = (i == BTS_DIFFUSE) ? SHADERTEX_DIFFUSE : (i == BTS_ALPHA) ? SHADERTEX_ALPHA : (i == BTS_ALPHA_LAYER_1) ? SHADERTEX_ALPHA_LAYER_1 : SHADERTEX_ALPHA_LAYER_2;

        const MaterialTextureEntry * ptMatTex = MaterialLookup(pdefParticle->tUberMaterial, eShaderTex);
        if (ptMatTex)
        {

            TexAnimInitialize(ptMatTex->tMatTex.snoTex, ptMatTex->tMatTex.pTexAnimCompParams, ptMatTex->tMatTex.serTexAnimCompParams.dwSize, ptParticle->tTexAnimState[i]);
        }
    }




    ParticleCounts & tParticleCounts = tCGame.ptParticleGlobals->tParticleCounts;
    BOOL fDelete = ParticleSystemUpdateParams(ptParticleSystem, TRUE, idParticle, tParams, ptParticle->wp, FALSE, TRUE, flDeltaTime);
    tParticleCounts.nParticleUpdatedCount++;
    if (fDelete)
    {
        sDeleteParticle(ptParticleSystem, ptParticle, FALSE);
        return FALSE;
    }

    ParticleSystemUpdateBounds(ptParticleSystem->wsBounds.wdRadius, ptParticleSystem->wp, ptParticle->tParams.flScale, ptParticle->tParams.fStretch, ptParticle->wp);

    ParticleAttachmentsOnMsg(*ptParticleSystem, idParticle, KeyedMessage(PARTICLE_MSGKEY_PARTICLE_CREATE, 0.0f, Shape::DEFAULT_MATERIAL));

    if (ptParticleSystem->eType == PSTYPE_CLUTTER_DYNAMIC || ptParticleSystem->eType == PSTYPE_FOLIAGE_DYNAMIC)
    {

        Angle angle = RandAngle(CGameGlobalsGet().rngLocal);
        tParams.qRelativeOrientation.SetRotation(cgwvZAxis, angle);

        Sphere ws;
        ws.Set(ptParticle->wp, tParams.flScale);

        if (tCGame.ptParticleGlobals->ptFoliageSphereTree->IsSphereValid(ws))
        {





            XSphereNode * ptSphereNode = tCGame.ptParticleGlobals->ptFoliageSphereTree->AddSphereNode(ws, (void *)(intptr_t)idParticle);
            tParams.tFoliageState.ptSphereNode = ptSphereNode;
            tParams.tFoliageState.flExplosionFactor = pdefParticle->flFoliageExplosionFactor;
        }
    }

    switch (ptParticleSystem->eType)
    {
    case PSTYPE_FOLIAGE_ACTOR: case PSTYPE_FOLIAGE_BILLBOARD:
    case PSTYPE_FOLIAGE_DYNAMIC:
        tParams.rgbaColor.Set(ptParticleSystem->rgbaColorMesh.r, ptParticleSystem->rgbaColorMesh.a, 0xff, 0xff);
        break;

    default:
        tParams.rgbaColor = tParams.tCreateParams.rgbaColor;
    }
    tParams.rgbaAlphaAux = tParams.tCreateParams.rgbaAlphaAux;

    return TRUE;
}




static void sParticleParamsConstInitialize(ParticleSystem * ptParticleSystem, ParticleConstParams & tConstParams)
{


    InterpTime tInterpTime;

    tInterpTime.Init();

    const ParticleDefinition * pdefParticle = ptParticleSystem->GetParticleDef();

    if (ptParticleSystem->dwConstFlags & (1 << PARTICLE_PATH_CONST_COLOR))
    {
        if (pdefParticle->pathParticleColor)
        {
            pdefParticle->pathParticleColor->GetInterpolatedValueByTime(0, PARTICLE_INTERP_COLOR, tInterpTime, tConstParams.rgbaConstColor);
        }
        else
        {
            tConstParams.rgbaConstColor = RGBAColor(255, 255, 255, 255);
        }
    }

    if (ptParticleSystem->dwConstFlags & (1 << PARTICLE_PATH_CONST_ALPHA))
    {
        if (pdefParticle->pathParticleAlpha)
        {
            pdefParticle->pathParticleAlpha->GetInterpolatedValueByTime(0, PARTICLE_INTERP_ALPHA, tInterpTime, tConstParams.flConstAlpha);
        }
        else
        {
            tConstParams.flConstAlpha = 1.0f;
        }
    }

    if (ptParticleSystem->dwConstFlags & (1 << PARTICLE_PATH_CONST_ALPHAAUX))
    {
        if (pdefParticle->ptExtendedPath_Console && pdefParticle->ptExtendedPath_Console->pathParticleAlphaAux)
        {
            pdefParticle->ptExtendedPath_Console->pathParticleAlphaAux->GetInterpolatedValueByTime(0, PARTICLE_INTERP_ALPHAAUX, tInterpTime, tConstParams.flConstAlphaAux);
        }
        else
        {
            tConstParams.flConstAlphaAux = 1.0f;
        }
    }

    if (ptParticleSystem->dwConstFlags & (1 << PARTICLE_PATH_CONST_SCALE))
    {
        if (pdefParticle->pathParticleScale)
        {
            pdefParticle->pathParticleScale->GetInterpolatedValueByTime(0, PARTICLE_INTERP_SCALE, tInterpTime, tConstParams.flConstScale);
        }
        else
        {
            tConstParams.flConstScale = 1.0f;
        }
    }

    if (ptParticleSystem->dwConstFlags & (1 << PARTICLE_PATH_CONST_STRETCH))
    {
        if (pdefParticle->pathParticleStretch)
        {
            pdefParticle->pathParticleStretch->GetInterpolatedValueByTime(0, PARTICLE_INTERP_STRETCH, tInterpTime, tConstParams.flConstStretch);
        }
        else
        {
            tConstParams.flConstStretch = 1.0f;
        }
    }

    if (ptParticleSystem->dwFlags & (1 << PARTICLE_SYSTEM_USES_SPIN))
    {
        if (ptParticleSystem->dwConstFlags & (1 << PARTICLE_PATH_CONST_ANGULAR_VELOCITY))
        {
            if (pdefParticle->pathAngularVelocity)
            {
                pdefParticle->pathAngularVelocity->GetInterpolatedValueByTime(0, PARTICLE_INTERP_ANG_VELOCITY, tInterpTime, tConstParams.aConstAngVel);
                tConstParams.aConstAngVel = tConstParams.aConstAngVel * TICKS_PER_SECOND;
            }
            else
            {
                tConstParams.aConstAngVel = 0.0f;
            }
        }

        if (ptParticleSystem->dwConstFlags & (1 << PARTICLE_PATH_CONST_ANGULAR_POSITION))
        {
            if (pdefParticle->pathAngularPosition)
            {
                pdefParticle->pathAngularPosition->GetInterpolatedValueByTime(0, PARTICLE_INTERP_ANG_POSITION, tInterpTime, tConstParams.aConstAngPos);
            }
            else
            {
                tConstParams.aConstAngPos = 0.0f;
            }
        }
    }

    if (ptParticleSystem->dwFlags & (1 << PARTICLE_SYSTEM_USES_ORIENTATION_MOTION))
    {
        if (ptParticleSystem->dwConstFlags & (1 << PARTICLE_PATH_CONST_ORIENTATION_VELOCITY))
        {
            if (pdefParticle->ptExtendedPath_Console && pdefParticle->ptExtendedPath_Console->pathOrientationVelocity)
            {
                pdefParticle->ptExtendedPath_Console->pathOrientationVelocity->GetInterpolatedValueByTime(0, PARTICLE_INTERP_ORIENTATIONVEL, tInterpTime, tConstParams.aConstOrientationVel);
                tConstParams.aConstOrientationVel = tConstParams.aConstOrientationVel * TICKS_PER_SECOND;
            }
            else
            {
                tConstParams.aConstOrientationVel = 0.0f;
            }
        }

        if (ptParticleSystem->dwConstFlags & (1 << PARTICLE_PATH_CONST_ORIENTATION_POSITION))
        {
            if (pdefParticle->ptExtendedPath_Console && pdefParticle->ptExtendedPath_Console->pathOrientationPosition)
            {
                pdefParticle->ptExtendedPath_Console->pathOrientationPosition->GetInterpolatedValueByTime(0, PARTICLE_INTERP_ORIENTATIONPOS, tInterpTime, tConstParams.aConstOrientationPos);
            }
            else
            {
                tConstParams.aConstOrientationPos = 0.0f;
            }
        }
    }

    if (ptParticleSystem->dwFlags & (1 << PARTICLE_SYSTEM_HAS_ORIENTATION_AXIS))
    {
        if (ptParticleSystem->dwConstFlags & (1 << PARTICLE_PATH_CONST_ORIENTATION_AXIS))
        {
            if (pdefParticle->pathOrientationAxis)
            {
                pdefParticle->pathOrientationAxis->GetInterpolatedValueByTime(0, PARTICLE_INTERP_ORIENTATIONAXIS, tInterpTime, tConstParams.vConstOrientationAxis);
            }
            else
            {
                tConstParams.vConstOrientationAxis = cgwvNull;
            }
        }
    }

    if (ptParticleSystem->dwFlags & (1 << PARTICLE_SYSTEM_USES_CYLINDER_MOTION))
    {
        if (ptParticleSystem->dwConstFlags & (1 << PARTICLE_PATH_CONST_MOTION_CYLINDER_RADIAL_VEL))
        {
            if (pdefParticle->pathMotionCylinderRadialVel)
            {
                pdefParticle->pathMotionCylinderRadialVel->GetInterpolatedValueByTime(0, PARTICLE_INTERP_CYLINDERRADIALVEL, tInterpTime, tConstParams.flConstCylinderRadialVel);
                tConstParams.flConstCylinderRadialVel = tConstParams.flConstCylinderRadialVel * TICKS_PER_SECOND;
            }
            else
            {
                tConstParams.flConstCylinderRadialVel = 0.0f;
            }
        }

        if (ptParticleSystem->dwConstFlags & (1 << PARTICLE_PATH_CONST_MOTION_CYLINDER_RADIAL_POS))
        {
            if (pdefParticle->ptExtendedPath_Console && pdefParticle->ptExtendedPath_Console->pathMotionCylinderRadialPos)
            {
                pdefParticle->ptExtendedPath_Console->pathMotionCylinderRadialPos->GetInterpolatedValueByTime(0, PARTICLE_INTERP_CYLINDERRADIALPOS, tInterpTime, tConstParams.flConstCylinderRadialPos);
            }
            else
            {
                tConstParams.flConstCylinderRadialPos = 0.0f;
            }
        }

        if (ptParticleSystem->dwConstFlags & (1 << PARTICLE_PATH_CONST_MOTION_CYLINDER_ANGLE_VEL))
        {
            if (pdefParticle->pathMotionCylinderAngleVel)
            {
                pdefParticle->pathMotionCylinderAngleVel->GetInterpolatedValueByTime(0, PARTICLE_INTERP_CYLINDERANGLEVEL, tInterpTime, tConstParams.aConstCylinderAngleVel);
                tConstParams.aConstCylinderAngleVel = tConstParams.aConstCylinderAngleVel * TICKS_PER_SECOND;
            }
            else
            {
                tConstParams.aConstCylinderAngleVel = 0.0f;
            }
        }

        if (ptParticleSystem->dwConstFlags & (1 << PARTICLE_PATH_CONST_MOTION_CYLINDER_AXIS))
        {
            Vector3D wvRotationAxis(cgwvZAxis);
            if (pdefParticle->pathMotionCylinderAxis)
            {
                pdefParticle->pathMotionCylinderAxis->GetInterpolatedValueByTime(0, PARTICLE_INTERP_CYLINDERAXIS, tInterpTime, wvRotationAxis);
            }

            float wdMagnitude = wvRotationAxis.Normalize();
            if (wdMagnitude > EPSILON)
            {
                tConstParams.wvConstCylinderAxis = wvRotationAxis;
            }
        }
    }

    if (ptParticleSystem->dwFlags & (1 << PARTICLE_SYSTEM_USES_SPHERE_MOTION))
    {
        if (ptParticleSystem->dwConstFlags & (1 << PARTICLE_PATH_CONST_SPHERE_RADIAL_VEL))
        {
            if (pdefParticle->ptExtendedPath_Console && pdefParticle->ptExtendedPath_Console->pathMotionSphereRadialVel)
            {
                pdefParticle->ptExtendedPath_Console->pathMotionSphereRadialVel->GetInterpolatedValueByTime(0, PARTICLE_INTERP_SPHERERADIALVEL, tInterpTime, tConstParams.flConstSphereRadialVel);
                tConstParams.flConstSphereRadialVel = tConstParams.flConstSphereRadialVel * TICKS_PER_SECOND;
            }
            else
            {
                tConstParams.flConstSphereRadialVel = 0.0f;
            }
        }

        if (ptParticleSystem->dwConstFlags & (1 << PARTICLE_PATH_CONST_SPHERE_RADIAL_POS))
        {
            if (pdefParticle->ptExtendedPath_Console && pdefParticle->ptExtendedPath_Console->pathMotionSphereRadialPos)
            {
                pdefParticle->ptExtendedPath_Console->pathMotionSphereRadialPos->GetInterpolatedValueByTime(0, PARTICLE_INTERP_SPHERERADIALPOS, tInterpTime, tConstParams.flConstSphereRadialPos);
            }
            else
            {
                tConstParams.flConstSphereRadialPos = 0.0f;
            }
        }
    }

    if (ptParticleSystem->dwFlags & (1 << PARTICLE_SYSTEM_USES_CARTESIAN_MOTION))
    {
        if (ptParticleSystem->dwConstFlags & (1 << PARTICLE_PATH_CONST_MOTION_CARTESIAN_VEL))
        {
            if (pdefParticle->pathMotionCartesianVel)
            {
                pdefParticle->pathMotionCartesianVel->GetInterpolatedValueByTime(0, PARTICLE_INTERP_CARTESIANVEL, tInterpTime, tConstParams.vConstCartesianVel);
                tConstParams.vConstCartesianVel = tConstParams.vConstCartesianVel * TICKS_PER_SECOND;
            }
            else
            {
                tConstParams.vConstCartesianVel = cgwvNull;
            }
        }

        if (ptParticleSystem->dwConstFlags & (1 << PARTICLE_PATH_CONST_MOTION_CARTESIAN_POS))
        {
            if (pdefParticle->pathMotionCartesianPos)
            {
                pdefParticle->pathMotionCartesianPos->GetInterpolatedValueByTime(0, PARTICLE_INTERP_CARTESIANPOS, tInterpTime, tConstParams.vConstCartesianPos);
            }
            else
            {
                tConstParams.vConstCartesianPos = cgwvNull;
            }
        }

        if (ptParticleSystem->dwConstFlags & (1 << PARTICLE_PATH_CONST_MOTION_CARTESIAN_ACC))
        {
            if (pdefParticle->pathMotionCartesianAcc)
            {
                pdefParticle->pathMotionCartesianAcc->GetInterpolatedValueByTime(0, PARTICLE_INTERP_CARTESIANACC, tInterpTime, tConstParams.vConstCartesianAcc);
                tConstParams.vConstCartesianAcc = tConstParams.vConstCartesianAcc * TICKS_PER_SECOND * TICKS_PER_SECOND;
            }
            else
            {
                tConstParams.vConstCartesianAcc = cgwvNull;
            }
        }
    }

    if (ptParticleSystem->dwFlags & (1 << PARTICLE_SYSTEM_USES_RELATIVE_CARTESIAN_MOTION))
    {
        if (ptParticleSystem->dwConstFlags & (1 << PARTICLE_PATH_CONST_MOTION_RELATIVE_CARTESIAN_VEL))
        {
            if (pdefParticle->pathMotionRelativeCartesianVel)
            {
                pdefParticle->pathMotionRelativeCartesianVel->GetInterpolatedValueByTime(0, PARTICLE_INTERP_RELATIVECARTESIANVEL, tInterpTime, tConstParams.vConstRelativeCartesianVel);
                tConstParams.vConstRelativeCartesianVel = tConstParams.vConstRelativeCartesianVel * TICKS_PER_SECOND;
            }
            else
            {
                tConstParams.vConstRelativeCartesianVel = cgwvNull;
            }
        }

        if (ptParticleSystem->dwConstFlags & (1 << PARTICLE_PATH_CONST_MOTION_RELATIVE_CARTESIAN_POS))
        {
            if (pdefParticle->pathMotionRelativeCartesianPos)
            {
                pdefParticle->pathMotionRelativeCartesianPos->GetInterpolatedValueByTime(0, PARTICLE_INTERP_RELATIVECARTESIANPOS, tInterpTime, tConstParams.vConstRelativeCartesianPos);
            }
            else
            {
                tConstParams.vConstRelativeCartesianPos = cgwvNull;
            }
        }

        if (ptParticleSystem->dwConstFlags & (1 << PARTICLE_PATH_CONST_MOTION_RELATIVE_CARTESIAN_ACC))
        {
            if (pdefParticle->ptExtendedPath_Console && pdefParticle->ptExtendedPath_Console->pathMotionRelativeCartesianAcc)
            {
                pdefParticle->ptExtendedPath_Console->pathMotionRelativeCartesianAcc->GetInterpolatedValueByTime(0, PARTICLE_INTERP_RELATIVECARTESIANACC, tInterpTime, tConstParams.vConstRelativeCartesianAcc);
                tConstParams.vConstRelativeCartesianAcc = tConstParams.vConstRelativeCartesianAcc * TICKS_PER_SECOND * TICKS_PER_SECOND;
            }
            else
            {
                tConstParams.vConstRelativeCartesianAcc = cgwvNull;
            }
        }
    }

    if (ptParticleSystem->dwFlags & (1 << PARTICLE_SYSTEM_USES_ATTRACTOR_MOTION))
    {
        if (ptParticleSystem->dwConstFlags & (1 << PARTICLE_PATH_CONST_MOTION_ATTRACTOR_VEL))
        {
            if (pdefParticle->ptExtendedPath_Console && pdefParticle->ptExtendedPath_Console->pathMotionAttractorVel)
            {
                pdefParticle->ptExtendedPath_Console->pathMotionAttractorVel->GetInterpolatedValueByTime(0, PARTICLE_INTERP_ATTRACTORVEL, tInterpTime, tConstParams.flConstAttractorVel);
                tConstParams.flConstAttractorVel = tConstParams.flConstAttractorVel * TICKS_PER_SECOND;
            }
            else
            {
                tConstParams.flConstAttractorVel = 0.0f;
            }
        }

        if (ptParticleSystem->dwConstFlags & (1 << PARTICLE_PATH_CONST_MOTION_ATTRACTOR_POS))
        {
            if (pdefParticle->ptExtendedPath_Console && pdefParticle->ptExtendedPath_Console->pathMotionAttractorPos)
            {
                pdefParticle->ptExtendedPath_Console->pathMotionAttractorPos->GetInterpolatedValueByTime(0, PARTICLE_INTERP_ATTRACTORPOS, tInterpTime, tConstParams.flConstAttractorPos);
            }
            else
            {
                tConstParams.flConstAttractorPos = 0.0f;
            }
        }
    }
}




static BOOL sCreateRainParticle(
    ParticleSystem * ptParticleSystem,
    Emitter & tEmitter,
    float flDeltaTime)
{
    ParticleGlobals * ptParticleGlobals = CGameGet().ptParticleGlobals;

    RainParticleID idParticle = ptParticleGlobals->ptRainParticles->ElementNew();
    if (idParticle == PARTICLEID_INVALID)
    {
        return FALSE;
    }

    RainParticle * ptParticle = ptParticleGlobals->ptRainParticles->ElementGet(idParticle);

    ptParticle->nIndex = ptParticleSystem->tParticleList.size();
    ptParticleSystem->tParticleList.push_back((Particle *)ptParticle);

    ptParticle->idParticleSystem = ptParticleSystem->id;

    const ParticleDefinition * pdefParticle = ptParticleSystem->GetParticleDef();

    RNG & rng = CGameGlobalsGet().rngLocal;
    uint32 dwInterpSeed = Rand_Internal(rng, 0xfffffffd);

    Vector3D wvNormal;
    int32 nBoneIndex;
    EmitterGetRandomPoint(rng,
        cgwpNull,
        tEmitter,

        ptParticle->wp,
        wvNormal,
        nBoneIndex);

    const MaterialTextureEntry * ptMatTexDiffuse = MaterialLookup(pdefParticle->tUberMaterial, SHADERTEX_DIFFUSE);
    if (ptMatTexDiffuse)
    {

        TexAnimInitialize(ptMatTexDiffuse->tMatTex.snoTex, ptMatTexDiffuse->tMatTex.pTexAnimCompParams, ptMatTexDiffuse->tMatTex.serTexAnimCompParams.dwSize, ptParticle->tTexAnimState);
    }




    if (pdefParticle->pathInitialScale)
    {
        pdefParticle->pathInitialScale->GetInterpolatedValueByTime(dwInterpSeed, PARTICLE_INTERP_INITIAL_SCALE, ptParticleSystem->tInterpTime, ptParticle->flScale);
    }
    else
    {
        ptParticle->flScale = 1.0f;
    }

    int32 nDuration;
    if (pdefParticle->pathParticleDuration)
    {
        pdefParticle->pathParticleDuration->GetInterpolatedValueByTime(dwInterpSeed, PARTICLE_INTERP_PARTICLE_DURATION, ptParticleSystem->tInterpTime, nDuration);
    }
    else
    {
        nDuration = 1;
    }

    ptParticle->flDuration = nDuration * (1.0f / TICKS_PER_SECOND);

    {
        InterpTime tInterpTimeParticle;
        tInterpTimeParticle.Init();
        if (pdefParticle->pathParticleColor)
        {
            pdefParticle->pathParticleColor->GetInterpolatedValueByTime(dwInterpSeed, PARTICLE_INTERP_COLOR, tInterpTimeParticle, ptParticle->rgbaColor);
        }
        else
        {
            ptParticle->rgbaColor = scgtDefaultCreateParams.rgbaColor;
        }
        if (pdefParticle->pathParticleStretch)
        {
            pdefParticle->pathParticleStretch->GetInterpolatedValueByTime(dwInterpSeed, PARTICLE_INTERP_STRETCH, tInterpTimeParticle, ptParticle->fStretch);
        }
        else
        {
            ptParticle->fStretch = 1.0f;
        }
    }


    ptParticle->wvLastMotionNormalized = ptParticleSystem->qOrientation.Rotate(cgwvXAxis);

    ptParticle->ptPointMass = PointMassCreate(ptParticleSystem->tPhysicsSystem);

    {
        PointMass & pm = *ptParticle->ptPointMass;

        pm.wp = ptParticle->wp;
        float flMass = Math::Max(pdefParticle->flMass, EPSILON);
        pm.flInvMass = 1.0f / flMass;


        pm.wv = cgwvNull;
    }

    ptParticle->qOrientation = ptParticleSystem->qOrientation;

    if (pdefParticle->nFacing == PARTICLE_FACING_UP)
    {

        ptParticle->qOrientation = cgqIdentity;
    }

    ParticleCounts & tParticleCounts = ptParticleGlobals->tParticleCounts;
    BOOL fDelete = ptParticleSystem->UpdateRainParams(TRUE, *ptParticle, ptParticle->wp, tParticleCounts, flDeltaTime);
    if (fDelete)
    {
        sDeleteRainParticle(ptParticleSystem, ptParticle);
        return FALSE;
    }

    ParticleSystemUpdateBounds(ptParticleSystem->wsBounds.wdRadius, ptParticleSystem->wp, ptParticle->flScale, ptParticle->fStretch, ptParticle->wp);

    return TRUE;
}







































































static void sDoBoxWrapping(const ParticleSystem * ptParticleSystem, const ParticleDefinition * pdefParticle, Vector3D & wpParticle)
{



    if (pdefParticle->tEmitterParams.eType == EMIT_BOX_SHELL)
    {



        Vector3D wvMinBoxExtent;
        Vector3D wvMaxBoxExtent;
        sParticleGetBoxExtents(pdefParticle, ptParticleSystem, wvMinBoxExtent, wvMaxBoxExtent);

        Vector3D wpBoxMin(wvMinBoxExtent);
        Vector3D wpBoxMax(wvMaxBoxExtent);

        if (wpBoxMax.x - wpBoxMin.x > EPSILON &&
            wpBoxMax.y - wpBoxMin.y > EPSILON &&
            wpBoxMax.z - wpBoxMin.z > EPSILON)
        {
            wpParticle.x = Math::PinWrap(wpParticle.x, wpBoxMin.x, wpBoxMax.x);
            wpParticle.y = Math::PinWrap(wpParticle.y, wpBoxMin.y, wpBoxMax.y);
            wpParticle.z = Math::PinWrap(wpParticle.z, wpBoxMin.z, wpBoxMax.z);
        }
    }
}



BOOL ParticleSystem::UpdateRainParams(const BOOL, RainParticle & tParticle, Vector3D & wpParticle, ParticleCounts & tParticleCounts, float flDeltaTime)
{





    const ParticleDefinition * pdefParticle = GetParticleDef();
    tParticleCounts.nParticleUpdatedCount++;

    Vector3D wpParticlePrev(wpParticle);





    tParticle.flSeconds += flDeltaTime;

    if (tParticle.flSeconds >= tParticle.flDuration)
    {

        tParticle.qOrientation.FromArc(cgwvXAxis, tParticle.wvLastMotionNormalized);

        return TRUE;
    }


    if (tParticle.flDuration <= 0.0f)
    {
        return FALSE;
    }


    PointMass & pm = *tParticle.ptPointMass;
    Vector3D wvMotion(pm.wv);


    sDoBoxWrapping(this, pdefParticle, wpParticle);


    tParticle.wvMotion = wvMotion;

    Vector3D wvMotionNormalized(wvMotion);
    float flParticleSpeed = wvMotionNormalized.Normalize();
    if (flParticleSpeed > EPSILON)
    {
        tParticle.wvLastMotionNormalized = wvMotionNormalized;
    }

    InterpTime tInterpTimeLoc;
    tInterpTimeLoc.Init();
    tInterpTimeLoc.flTime = tParticle.flSeconds;
    tInterpTimeLoc.flScale = tParticle.flDuration;
    tInterpTimeLoc.nTimeType = TICK_TIME_NON_LOOPING;

    uint32 uInterpSeed = 0;
    if (pdefParticle->pathParticleAlpha)
    {
        pdefParticle->pathParticleAlpha->GetInterpolatedValueByTime(uInterpSeed, PARTICLE_INTERP_ALPHA, tInterpTimeLoc, tParticle.flAlpha);
    }
    else
    {
        tParticle.flAlpha = 1.0f;
    }

    tParticle.flAlpha *= flAlpha;

    {

        Vector3D wvPositionChange = wpParticle - wpParticlePrev;




        pm.wp += wvPositionChange;

        wpParticle = pm.wp;
    }

    return FALSE;
}










































static void sUpdateRainParticlesForSystem(
    ParticleSystem * ptParticleSystem,
    float flDeltaTime)
{
    const ParticleDefinition * pdefParticle = ptParticleSystem->GetParticleDef();
    const MaterialTextureEntry * ptMatTexDiffuse = MaterialLookup(pdefParticle->tUberMaterial, SHADERTEX_DIFFUSE);

    float wdSystemRadius = ptParticleSystem->wsBounds.wdRadius;

    ParticleCounts & tParticleCounts = CGameGet().ptParticleGlobals->tParticleCounts;
    uint32 nCount = ptParticleSystem->tParticleList.size(); for (uint32 pos = 0; pos < nCount; pos++)
    {
        RainParticle * ptParticle = (RainParticle *)ptParticleSystem->tParticleList[pos];

        Vector3D wpParticle = ptParticle->wp;
        BOOL fDelete;




        if (ptMatTexDiffuse)
        {
            TexAnimUpdate(ptMatTexDiffuse->tMatTex.pTexAnimCompParams, ptMatTexDiffuse->tMatTex.serTexAnimCompParams.dwSize, NULL, ptParticle->tTexAnimState, flDeltaTime, 0.0f);
        }




        fDelete = ptParticleSystem->UpdateRainParams(TRUE, *ptParticle, wpParticle, tParticleCounts, flDeltaTime);
        if (!fDelete)
        {




            ptParticle->wp = wpParticle;

            ParticleSystemUpdateBounds(wdSystemRadius, ptParticleSystem->wp, ptParticle->flScale, ptParticle->fStretch, ptParticle->wp);
        }
        else
        {

            sDeleteRainParticle(ptParticleSystem, ptParticle);

            pos--;
            nCount--;
        }
    }

    ptParticleSystem->wsBounds.wdRadius = wdSystemRadius;
}




static void sSubmitRActorParticles(
    ParticleSystem * ptParticleSystem,
    const Camera & cam,
    DisplayList & dispList,
    DisplayList * ptdispListReflection,
    DisplayList * ptDisplayListProjected,
    const float flDeltaTime)
{
    POS pos = ptParticleSystem->tParticleRActorList.GetHeadPosition();
    while (pos)
    {
        RActorID idRActor = (RActorID)ptParticleSystem->tParticleRActorList.GetNext(pos);

        RActor * ptRActor = RActorTryToGet(idRActor);
        // Inference: absent RActor follows the merged next-node branch.
        if (ptRActor)
        {



            RActorGfxRenderRActorInDisplayList(cam, dispList, ptdispListReflection, ptDisplayListProjected, idRActor, dispList.GetRenderContext() == RENDERCONTEXT_DEFAULT, flDeltaTime, FALSE);
        }
    }
}



static void sUpdateRActorParticleRenderTimeParams(
    ParticleSystem * ptParticleSystem,
    const Camera & cam,
    const Vector3D & wpPlayerPos,
    float flDeltaTime)
{
    const ParticleDefinition * pdefParticle = ptParticleSystem->GetParticleDef();

    POS pos = ptParticleSystem->tParticleRActorList.GetHeadPosition();
    while (pos)
    {
        RActorID idRActor = (RActorID)ptParticleSystem->tParticleRActorList.GetNext(pos);

        RActor * ptRActor = RActorTryToGet(idRActor);
        // Inference: invalid or missing actor merges into the next-node path.
        if (!ptRActor)
            continue;


        Vector3D wpParticle = ptRActor->tLocation.tWorldLocation.transform.wp;
        ParticleParams & tParams = *ptRActor->ptParticleParams;

        BOOL fUsesPhysics;
        fUsesPhysics = ptParticleSystem->eType == PSTYPE_PHYS_ACTOR;




        if (!fUsesPhysics)
        {
            JitterRenderedPosition(ptParticleSystem, pdefParticle, tParams, wpParticle, flDeltaTime);
        }


        tParams.rgbaColor.a = UpdateParticleAlpha(ptParticleSystem, tParams.flAlpha, wpPlayerPos, wpParticle);

        if (!fUsesPhysics)
        {

            UpdateGroundNormal(pdefParticle, wpParticle, tParams);


            UpdateParticleOrientation(
                ptParticleSystem,
                pdefParticle,
                tParams.qOrientation,
                tParams.wvMotion,
                tParams.wvLastMotionNormalized,
                tParams.qRelativeOrientation,



                tParams.wvGroundNormal, wpParticle, cam.wvCameraForward);




            ScopedActorDefinitionLockR pdefActor(ptRActor->snoActor);
            RActorSetScale(ptRActor, tParams.flScale, TRUE, pdefActor);
            ptRActor->flStretch = tParams.fStretch;
            RActorSetOrientation(ptRActor, tParams.qOrientation, pdefActor, NULL);
        }

        ptRActor->rgbaColor = tParams.rgbaColor;
        ptRActor->flAlpha = tParams.flAlpha;
    }
}




static void sParticleSystemGenerateRainVertices(
    ParticleSystem * ptParticleSystem,
    VertexBillboardParticle * ptVertices,
    ParticleSystemRenderUpdateParams & tParams)
{
    TextureFramesInfo atTexFramesInfo[NUM_BILLBOARD_TEX_STATES];
    for (int32 i = 0; i < tParams.nTexStageCount; ++i)
    {
        if (tParams.ptTextureDef[i])
        {
            atTexFramesInfo[i].dwFrameCount = tParams.ptTextureDef[i]->dwFrameCount;
            atTexFramesInfo[i].dwHeight = tParams.ptTextureDef[i]->dwHeight;
            atTexFramesInfo[i].dwWidth = tParams.ptTextureDef[i]->dwWidth;
            atTexFramesInfo[i].ptTexFrames = tParams.ptTextureDef[i]->ptFrame;
        }
        else
        {
            atTexFramesInfo[i].dwFrameCount = 0;
        }
    }

    for (uint32 nCount = ptParticleSystem->tParticleList.size(), pos = 0; pos < nCount; ++pos)
    {
        RainParticle * ptParticle = (RainParticle *)ptParticleSystem->tParticleList[pos];

        ptParticle->rgbaColor.a = Math::FloatRoundToInt(ptParticle->flAlpha * 255.0f);






        tParams.tTexAnimState[BTS_DIFFUSE] = &ptParticle->tTexAnimState;
        tParams.snoTexAnimStateTex[BTS_DIFFUSE] = ptParticle->tTexAnimState.snoTex;
        tParams.flScale = ptParticle->flScale;
        tParams.flStretch = ptParticle->fStretch;
        tParams.rgbaColor = ptParticle->rgbaColor;
        tParams.rgbaAlphaAux = crgbaWhite;
        ParticleBillboardVerticesSubmit(tParams,
                ptParticle->wp,
                ptParticle->qOrientation,
                ptVertices, atTexFramesInfo, ptParticleSystem->tParticleQuads, ptParticleSystem->tParticleQuadUVs);








        ptVertices += 4;
    }
}




static void sParticleSystemRenderUpdateParamsInitialize(
    ParticleSystemRenderUpdateParams & tParams)
{
    SigmaMemoryClear(&tParams, sizeof(tParams));

    tParams.flScale = tParams.flStretch = 1.0f;


    for (int32 i = 0; i < NUM_BILLBOARD_TEX_STATES; ++i)
    {

        tParams.pTexAnimCompParams[i] = NULL;



        tParams.tTexAnimState[i] = NULL;
        tParams.snoTexAnimStateTex[i] = SNO_NONE;
    }

    tParams.rgbaColor = crgbaWhite;
    tParams.rgbaAlphaAux = crgbaWhite;
}




static void sParticleSystemRainRender(
    ParticleSystem * ptParticleSystem,
    const int32 nNumInstances,
    const Vector3D * pwvInstanceOffsets,
    DisplayList & dispList)
{
    const ParticleDefinition * pdefParticle = ptParticleSystem->GetParticleDef();

    ParticleSystemRenderUpdateParams tParams;
    sParticleSystemRenderUpdateParamsInitialize(tParams);

    tParams.dwFlags = 0;

    if (pdefParticle->nFacing != PARTICLE_FACING_BILLBOARD && pdefParticle->nFacing != PARTICLE_FACING_FULL_SCREEN && pdefParticle->nFacing != PARTICLE_FACING_GROUND_PROJECTION)
    {


        tParams.dwFlags |= 2;
    }

    const MaterialTextureEntry * ptMatTexDiffuse = MaterialLookup(pdefParticle->tUberMaterial, SHADERTEX_DIFFUSE);


    tParams.pTexAnimCompParams[BTS_DIFFUSE] = ptMatTexDiffuse->tMatTex.pTexAnimCompParams;
    tParams.dwTexAnimCompParamsSize[BTS_DIFFUSE] = ptMatTexDiffuse->tMatTex.serTexAnimCompParams.dwSize;



    tParams.nTexStageCount = 1;

    ParticleGlobals * ptGlobals = CGameGet().ptParticleGlobals;

    int32 nCount = ptParticleSystem->tParticleList.size();
    uint32 dwVertexStartIndex = ptGlobals->dwRenderUpdateVertIndex;
    uint32 dwVertexCount = nCount * 4;

    VertexBillboardParticle * ptVertices;
    if (nCount > 0)
    {
    ptVertices = sParticlesGfxVBBeginAlloc(dwVertexCount, dwVertexStartIndex);

    if (ptVertices)
    {

        sParticleSystemLockTextures(ptParticleSystem, pdefParticle, tParams);

        sParticleSystemGenerateRainVertices(ptParticleSystem, ptVertices, tParams);

        ScopedShaderDefinitionLockR pdefShader(ptParticleSystem->snoShader);

        Renderable tRenderable(RENDERABLETYPE_BILLBOARDPARTICLE,

                              ptParticleSystem->id,
                              PERFBIN_PARTICLES,
                              pdefShader,
                              pdefParticle->tUberMaterial.mat);
        RenderableHeader & tHeader = tRenderable.header;
        RenderableDetails & tDetails = tRenderable.details;

        tDetails.tParticle.dwBillboardFlags = tParams.dwFlags;
        tDetails.tParticle.dwVertexStartIndex = dwVertexStartIndex;
        tDetails.tParticle.dwVertexCount = dwVertexCount;

        for (int32 i = 0; i < NUM_BILLBOARD_TEX_STATES; ++i)
        {
            if (ptParticleSystem->ptParticleTextures->ptTextures[i])
            {
                tDetails.tParticle.snoTex[i] = ptParticleSystem->ptParticleTextures->ptTextures[i]->tHeader.snoHandle;
            }
            else
            {
                tDetails.tParticle.snoTex[i] = SNO_NONE;
            }
        }

        tDetails.tParticle.nNumInstances = nNumInstances;

        if (nNumInstances > 0)
        {
            Vector3D * pwvInstancesOffsetsCopy = (Vector3D *)CGameAsyncRenderGetMemory(nNumInstances * sizeof(Vector3D));
            SigmaMemoryCopy(pwvInstancesOffsetsCopy, pwvInstanceOffsets, nNumInstances * sizeof(Vector3D));
            tDetails.tParticle.pwvInstanceOffsets = pwvInstancesOffsetsCopy;
        }
        else
        {
            tDetails.tParticle.pwvInstanceOffsets = NULL;
        }




        tHeader.SetDist(0.0f);
        tHeader.SetRectFromPoint(CameraManagerGetCamera(CAMERA_CURRENT).cmState.transform.wp);
        dispList.SubmitRenderable(tRenderable);

        sParticleSystemUnlockTextures(ptParticleSystem);
    }

    }
}




static void sParticleSystemGenerateRainRippleVertices(
    ParticleSystem * ptParticleSystem,
    VertexBillboardParticle * ptVertices,
    ParticleSystemRenderUpdateParams & tParams)
{
    TextureFramesInfo atTexFramesInfo[NUM_BILLBOARD_TEX_STATES];
    for (int32 i = 0; i < tParams.nTexStageCount; ++i)
    {
        if (tParams.ptTextureDef[i])
        {
            atTexFramesInfo[i].dwFrameCount = tParams.ptTextureDef[i]->dwFrameCount;
            atTexFramesInfo[i].dwHeight = tParams.ptTextureDef[i]->dwHeight;
            atTexFramesInfo[i].dwWidth = tParams.ptTextureDef[i]->dwWidth;
            atTexFramesInfo[i].ptTexFrames = tParams.ptTextureDef[i]->ptFrame;
        }
        else
        {
            atTexFramesInfo[i].dwFrameCount = 0;
        }
    }

    for (uint32 nCount = ptParticleSystem->tParticleList.size(), pos = 0; pos < nCount; ++pos)
    {
        Particle * ptParticle = ptParticleSystem->tParticleList[pos];

        ptParticle->tParams.rgbaColor.a = Math::FloatRoundToInt(ptParticle->tParams.flAlpha * 255.0f);






        tParams.flScale = ptParticle->tParams.flScale;
        tParams.flStretch = ptParticle->tParams.fStretch;
        for (int32 i = 0; i < tParams.nTexStageCount; ++i)
        {
            tParams.tTexAnimState[i] = &ptParticle->tTexAnimState[i];
            tParams.snoTexAnimStateTex[i] = ptParticle->tTexAnimState[i].snoTex;
        }
        tParams.rgbaColor = ptParticle->tParams.rgbaColor;
        tParams.rgbaAlphaAux = ptParticle->tParams.rgbaAlphaAux;
        tParams.aAng = ptParticle->tParams.aAng;

        ParticleBillboardVerticesSubmit(tParams,
                ptParticle->wp,
                ptParticle->tParams.qOrientation,
                ptVertices, atTexFramesInfo, ptParticleSystem->tParticleQuads, ptParticleSystem->tParticleQuadUVs);








        ptVertices += 4;
    }
}




static void sParticleSystemRainRippleRender(
    ParticleSystem * ptParticleSystem,
    DisplayList & dispList)
{
    const ParticleDefinition * pdefParticle = ptParticleSystem->GetParticleDef();

    ParticleSystemRenderUpdateParams tParams;
    sParticleSystemRenderUpdateParamsInitialize(tParams);

    for (int32 i = 0; i < NUM_BILLBOARD_TEX_STATES; ++i)
    {
        ShaderTex eShaderTex = (i == BTS_DIFFUSE) ? SHADERTEX_DIFFUSE : (i == BTS_ALPHA) ? SHADERTEX_ALPHA : (i == BTS_ALPHA_LAYER_1) ? SHADERTEX_ALPHA_LAYER_1 : SHADERTEX_ALPHA_LAYER_2;
        const MaterialTextureEntry * ptMatTex = MaterialLookup(pdefParticle->tUberMaterial, eShaderTex);

        if (ptMatTex)
        {

            tParams.pTexAnimCompParams[i] = ptMatTex->tMatTex.pTexAnimCompParams;
            tParams.dwTexAnimCompParamsSize[i] = ptMatTex->tMatTex.serTexAnimCompParams.dwSize;



            tParams.nTexStageCount = i + 1;
        }
    }

    tParams.dwFlags = 0;

    if (pdefParticle->nFacing != PARTICLE_FACING_BILLBOARD && pdefParticle->nFacing != PARTICLE_FACING_FULL_SCREEN && pdefParticle->nFacing != PARTICLE_FACING_GROUND_PROJECTION)
    {


        tParams.dwFlags |= 2;
    }

    ParticleGlobals * ptGlobals = CGameGet().ptParticleGlobals;

    int32 nCount = ptParticleSystem->tParticleList.size();
    uint32 dwVertexStartIndex = ptGlobals->dwRenderUpdateVertIndex;
    uint32 dwVertexCount = nCount * 4;

    VertexBillboardParticle * ptVertices;
    if (nCount > 0)
    {
    ptVertices = sParticlesGfxVBBeginAlloc(dwVertexCount, dwVertexStartIndex);

    if (ptVertices)
    {

        sParticleSystemLockTextures(ptParticleSystem, pdefParticle, tParams);

        sParticleSystemGenerateRainRippleVertices(ptParticleSystem, ptVertices, tParams);

        ScopedShaderDefinitionLockR pdefShader(ptParticleSystem->snoShader);

        Renderable tRenderable(RENDERABLETYPE_BILLBOARDPARTICLE,

                              ptParticleSystem->id,
                              PERFBIN_PARTICLES,
                              pdefShader,
                              pdefParticle->tUberMaterial.mat);
        RenderableHeader & tHeader = tRenderable.header;
        RenderableDetails & tDetails = tRenderable.details;

        tDetails.tParticle.dwBillboardFlags = tParams.dwFlags;
        tDetails.tParticle.dwVertexStartIndex = dwVertexStartIndex;
        tDetails.tParticle.dwVertexCount = dwVertexCount;

        for (int32 i = 0; i < NUM_BILLBOARD_TEX_STATES; ++i)
        {
            if (ptParticleSystem->ptParticleTextures->ptTextures[i])
            {
                tDetails.tParticle.snoTex[i] = ptParticleSystem->ptParticleTextures->ptTextures[i]->tHeader.snoHandle;
            }
            else
            {
                tDetails.tParticle.snoTex[i] = SNO_NONE;
            }
        }

        tDetails.tParticle.nNumInstances = 0;
        tDetails.tParticle.pwvInstanceOffsets = NULL;

        tHeader.SetDist(0.0f);
        tHeader.SetRectFromPoint(CameraManagerGetCamera(CAMERA_CURRENT).cmState.transform.wp);
        dispList.SubmitRenderable(tRenderable);

        // Inference: the ripple submit shares the rain unlock tail; place it within the successful allocation scope.
        sParticleSystemUnlockTextures(ptParticleSystem);
    }
    }
}




static void sParticleSystemRenderUpdateJobExecute(void * pParam)
{
    ParticleSystemRenderUpdateRecord * ptRenderUpdate = (ParticleSystemRenderUpdateRecord *)pParam;
    ParticleSystem * ptParticleSystem = ptRenderUpdate->ptParticleSystem;
    ScopedParticleSystemLock tLock(ptParticleSystem);

    ParticleSystemGenerateVertices(
        ptParticleSystem,
        ptRenderUpdate->ptVertices,
        ptRenderUpdate->tParams,
        ptRenderUpdate->wpPlayer,
        ptRenderUpdate->tFrustum,
        ptRenderUpdate->wvCameraForward,
        ptRenderUpdate->atTexFramesInfo,
        ptRenderUpdate->flDeltaTime);
}







// NON_MATCHING: 864 vs 864 bytes, 47/216 insns differ: scheduling of the PositiveFloatMod/fdiv chain at 5292-5298 and w26/w28 swapped for vecStart.x/z. Constructing vecCount through the IVector3D ctor removed the vecCount.x spill (was 868).
static void sGetTiledInstances(
    const ParticleDefinition * pdefParticle,
    const Vector3D & wpBoundsMin,
    const Vector3D & wpBoundsMax,
    const Vector3D & wvBoxSize,
    const Camera & cam,
    int32 & nNumInstances,
    Vector3D * pwvInstanceOffsets)
{
    Vector3D vecStart = wpBoundsMin;
    IVector3D vecCount(1, 1, 1);

    if (wvBoxSize.x > EPSILON && wvBoxSize.y > EPSILON &&

        wvBoxSize.z > EPSILON)
    {
        vecStart.x -= Math::PositiveFloatMod(vecStart.x, wvBoxSize.x);
        vecStart.y -= Math::PositiveFloatMod(vecStart.y, wvBoxSize.y);
        vecStart.z -= Math::PositiveFloatMod(vecStart.z, wvBoxSize.z);

        vecCount.x = (int32)((wpBoundsMax.x - vecStart.x) / wvBoxSize.x) + 1;
        vecCount.y = (int32)((wpBoundsMax.y - vecStart.y) / wvBoxSize.y) + 1;
        vecCount.z = (int32)((wpBoundsMax.z - vecStart.z) / wvBoxSize.z) + 1;


        vecCount.x = Math::Min(vecCount.x, 20);
        vecCount.y = Math::Min(vecCount.y, 20);
        vecCount.z = Math::Min(vecCount.z, 20);
    }

    AABB aabbBounds;
    aabbBounds.wvExt.Set(wvBoxSize.x * 0.5f, wvBoxSize.y * 0.5f, wvBoxSize.z * 0.5f);

    Vector3D wpCurrent;

    wpCurrent.x = vecStart.x;

    BOOL bDebugDraw = AppDrawFlagIsSet(APP_DRAW_TILE_PARTICLE_GRIDS_BIT);

    for (int32 x = 0; x < vecCount.x; ++x)
    {
        wpCurrent.y = vecStart.y;

        for (int32 y = 0; y < vecCount.y; ++y)
        {
            wpCurrent.z = vecStart.z;

            for (int32 z = 0; z < vecCount.z; ++z)
            {
                aabbBounds.wp = wpCurrent + wvBoxSize * 0.5f;

                if (CameraComputeAABBVisibility(cam, aabbBounds))
                {
                    if (bDebugDraw)
                    {
                        GfxAABB3DDrawDeferred(aabbBounds, crgbaGreen, GLOBALSNO_SHADER_3D_PRIMS, GFX_LINE_STYLE_SOLID, RENDERCONTEXT_DEFAULT);
                    }

                    if (nNumInstances >= 64)
                        return;




                    pwvInstanceOffsets[nNumInstances++] = wpCurrent;
                }
                else
                {
                    if (bDebugDraw)
                    {
                        GfxAABB3DDrawDeferred(aabbBounds, crgbaYellow, GLOBALSNO_SHADER_3D_PRIMS, GFX_LINE_STYLE_SOLID, RENDERCONTEXT_DEFAULT);
                    }
                }

                wpCurrent.z += wvBoxSize.z;
            }

            wpCurrent.y += wvBoxSize.y;
        }

        wpCurrent.x += wvBoxSize.x;
    }
}




static void sParticleSystemRenderUpdateRain(
    ParticleSystem * ptParticleSystem,
    DisplayList & dispList)
{
    ParticleGlobals * ptGlobals = CGameGet().ptParticleGlobals;

    if (ptParticleSystem->flAlpha < 0.000001f)
        return;



    const ParticleDefinition * pdefParticle = ptParticleSystem->GetParticleDef();
    if (pdefParticle->tEmitterParams.eType != 8)
        return;



    const Camera & cam = CameraManagerGetCamera(CAMERA_CURRENT);

    for (uint32 nCount = ptParticleSystem->tParticleList.size(), pos = 0; pos < nCount; ++pos)
    {
        RainParticle * ptParticle = (RainParticle *)ptParticleSystem->tParticleList[pos];

        UpdateParticleOrientation(
            ptParticleSystem,
            pdefParticle,
            ptParticle->qOrientation,
            ptParticle->wvMotion,
            ptParticle->wvLastMotionNormalized,
            cgqIdentity, cgwvZAxis, cgwpNull, cam.wvCameraForward);
    }




    Vector3D wpBoundsMin(-40.0f, -40.0f, 0.0f);
    Vector3D wpBoundsMax(40.0f, 40.0f, 30.0f);
    wpBoundsMin += ptParticleSystem->wp;
    wpBoundsMax += ptParticleSystem->wp;

    Vector3D wvMinBoxExtent;
    Vector3D wvMaxBoxExtent;
    sParticleGetBoxExtents(pdefParticle, ptParticleSystem, wvMinBoxExtent, wvMaxBoxExtent);
    Vector3D wvBoxSize = wvMaxBoxExtent - wvMinBoxExtent;

    ptGlobals->nRainNumInstances = 0;
    sGetTiledInstances(pdefParticle, wpBoundsMin, wpBoundsMax, wvBoxSize, cam, ptGlobals->nRainNumInstances, ptGlobals->wvRainInstanceOffsets);

    if (ptGlobals->nRainNumInstances)
    {
        ptGlobals->tParticleCounts.nSystemVisibleCount++;
    }
    ptParticleSystem->nDrawCount = ptGlobals->nRainNumInstances;
    ptGlobals->tParticleCounts.nParticleVisibleCount += ptParticleSystem->GetParticleCount() * ptGlobals->nRainNumInstances;

    sParticleSystemRainRender(ptParticleSystem, ptGlobals->nRainNumInstances, ptGlobals->wvRainInstanceOffsets, dispList);
}




static void sParticleSystemRenderUpdateRainRipple(
    ParticleSystem * ptParticleSystem,
    const Vector3D & wpPlayerPos,
    BOOL fAnyACDPlayer,
    DisplayList & dispList)
{
    ParticleGlobals * ptGlobals = CGameGet().ptParticleGlobals;

    if (ptParticleSystem->flAlpha < 0.000001f)
        return;



    const ParticleDefinition * pdefParticle = ptParticleSystem->GetParticleDef();
    if (pdefParticle->tEmitterParams.eType != 8 || !fAnyACDPlayer)
        return;








    const Camera & cam = CameraManagerGetCamera(CAMERA_CURRENT);

    for (uint32 nCount = ptParticleSystem->tParticleList.size(), pos = 0; pos < nCount; ++pos)
    {
        Particle * ptParticle = ptParticleSystem->tParticleList[pos];

        UpdateParticleOrientation(
            ptParticleSystem,
            pdefParticle,
            ptParticle->tParams.qOrientation,
            ptParticle->tParams.wvMotion,
            ptParticle->tParams.wvLastMotionNormalized,
            ptParticle->tParams.qRelativeOrientation,
            cgwvZAxis, cgwpNull, cam.wvCameraForward);
    }



    Vector3D wvMinBoxExtent;
    Vector3D wvMaxBoxExtent;
    sParticleGetBoxExtents(pdefParticle, ptParticleSystem, wvMinBoxExtent, wvMaxBoxExtent);
    Vector3D wvBoxSize = wvMaxBoxExtent - wvMinBoxExtent;

    float flHalfWidth = cwdRippleMapWidth * 0.5f;
    Vector3D wpBoundsMin(wpPlayerPos.x - flHalfWidth, wpPlayerPos.y - flHalfWidth, 0.0f);
    Vector3D wpBoundsMax(wpPlayerPos.x + flHalfWidth, wpPlayerPos.y + flHalfWidth, 0.0f);

    int32 nNumInstances = 0;
    Vector3D wvInstanceOffsets[64];
    sGetTiledInstances(pdefParticle, wpBoundsMin, wpBoundsMax, wvBoxSize, cam, nNumInstances, wvInstanceOffsets);

    if (nNumInstances)
    {
        ptGlobals->tParticleCounts.nSystemVisibleCount++;
    }
    ptParticleSystem->nDrawCount = nNumInstances;
    ptGlobals->tParticleCounts.nParticleVisibleCount += ptParticleSystem->GetParticleCount() * nNumInstances;

    sParticleSystemRainRippleRender(ptParticleSystem, dispList);
}



// NON_MATCHING: 5624 vs 5604 bytes; same ordered calls, five extra instructions/spill differences around rain tiling; tried faithful unlock scope and nested allocation guards.
static void sParticleSystemRenderUpdate(
    ParticleSystem * ptParticleSystem,
    const Camera & cam,
    DisplayList & dispList,
    DisplayList * pdispListReflection,
    DisplayList * pdispListProjected,
    const Vector3D & wpPlayerPos,
    BOOL fAnyACDPlayer,
    const float flDeltaTime)
{
    ParticleGlobals * ptGlobals = CGameGet().ptParticleGlobals;
    const ParticleDefinition * pdefParticle = ptParticleSystem->GetParticleDef();

    switch (ptParticleSystem->eType)
    {
    case PSTYPE_RAIN:
        sParticleSystemRenderUpdateRain(ptParticleSystem, dispList);
        return;

    case PSTYPE_RAIN_RIPPLE:
        sParticleSystemRenderUpdateRainRipple(ptParticleSystem, wpPlayerPos, fAnyACDPlayer, dispList);
        return;
    }

    if (!(ptParticleSystem->dwFlags & (1 << PARTICLE_SYSTEM_VISIBLE_BIT)))
        return;



    if (AppDrawFlagIsSet(APP_DRAW_NO_PLAYER_ACTOR_BIT))
    {
        if (ptParticleSystem->tParentData.HasLocalPlayerAsParent())
            return;
    }



    if (ptParticleSystem->dwFlags & (1 << PARTICLE_SYSTEM_CAUSES_DISTORTION))
    {


        if (!AppDrawFlagIsSet(APP_DRAW_HWCLASS_DISTORTIONS_BIT) || ptParticleSystem->snoShader == GlobalSNOGet(GLOBALSNO_SHADER_FULLSCREEN_DISTORTION))
            return;





        PostFXSetFrameFlag(POSTFX_FRAME_HAS_SOMETHING_DISTORTING, TRUE);
    }

    ptGlobals->tParticleCounts.nSystemVisibleCount++;

    if (ptParticleSystem->dwFlags & (1 << PARTICLE_SYSTEM_HAS_ACTORS))
    {
        sUpdateRActorParticleRenderTimeParams(ptParticleSystem, cam, wpPlayerPos, flDeltaTime);
        sSubmitRActorParticles(ptParticleSystem, cam, dispList, pdispListReflection, pdispListProjected, flDeltaTime);
    }
    else
    {
        ptGlobals->tParticleCounts.nParticleVisibleCount += ptParticleSystem->GetParticleCount();

        SNO snoShader = ptParticleSystem->snoShader;
        if (snoShader != SNO_NONE)
        {
            if (!(ptParticleSystem->dwFlags & (1 << PARTICLE_SYSTEM_HAS_ACTORS)))
            {
                int32 nCount = ptParticleSystem->tParticleList.size();
                uint32 dwVertexStartIndex;
                uint32 dwVertexCount = nCount * 4;

                VertexBillboardParticle * ptVertices;
                if (nCount > 0)
                {
                    ptVertices = sParticlesGfxVBBeginAlloc(dwVertexCount, dwVertexStartIndex);

                    if (ptVertices)
                    {

                        ParticleSystemRenderUpdateParams tParams;
                        sParticleSystemRenderUpdateParamsInitialize(tParams);

                        sParticleSystemLockTextures(ptParticleSystem, pdefParticle, tParams);


                        tParams.dwFlags = 0;
                        if (ptParticleSystem->dwDefFlags & (1 << PARTICLE_DEF_BOTTOM_ALIGNED_BIT))
                            tParams.dwFlags |= 1;



                        if (ptParticleSystem->dwFlags & (1 << PARTICLE_SYSTEM_USES_ORIENTATION_MOTION))
                        {
                            tParams.dwFlags |= 2;
                        }

                        if (ptParticleSystem->eType == PSTYPE_CLUTTER_DYNAMIC || ptParticleSystem->eType == PSTYPE_FOLIAGE_DYNAMIC)
                        {

                            tParams.dwFlags |= 4;
                        }

                        if (pdefParticle->nFacing != PARTICLE_FACING_BILLBOARD && pdefParticle->nFacing != PARTICLE_FACING_FULL_SCREEN && pdefParticle->nFacing != PARTICLE_FACING_GROUND_PROJECTION)
                        {


                            tParams.dwFlags |= 2;
                        }

                        if (ptParticleSystem->dwDefFlags & (1 << PARTICLE_DEF_COMPUTE_NORMALS_BIT))
                        {
                            tParams.dwFlags |= 8;
                        }

                        TextureFramesInfo atTexFramesInfo[NUM_BILLBOARD_TEX_STATES];
                        for (int32 i = 0; i < tParams.nTexStageCount; ++i)
                        {
                            if (tParams.ptTextureDef[i])
                            {
                                atTexFramesInfo[i].dwFrameCount = tParams.ptTextureDef[i]->dwFrameCount;
                                atTexFramesInfo[i].dwHeight = tParams.ptTextureDef[i]->dwHeight;
                                atTexFramesInfo[i].dwWidth = tParams.ptTextureDef[i]->dwWidth;
                                atTexFramesInfo[i].ptTexFrames = tParams.ptTextureDef[i]->ptFrame;
                            }
                            else
                            {
                                atTexFramesInfo[i].dwFrameCount = 0;
                            }
                        }




                        if (ptGlobals->fInMainParticleRenderUpdate && ptGlobals->nRenderUpdateJobCount < MAX_NORMAL_PARTICLE_SYSTEMS)
                        {

                            ParticleSystemRenderUpdateRecord * ptRenderUpdate = &ptGlobals->sParticleSystemRenderUpdateList.AppendElement_Tracked();
                            ptRenderUpdate->ptParticleSystem = ptParticleSystem;
                            ptRenderUpdate->ptVertices = ptVertices;
                            ptRenderUpdate->tParams = tParams;
                            ptRenderUpdate->wpPlayer = wpPlayerPos;
                            ptRenderUpdate->tFrustum = cam.tFrustum;
                            ptRenderUpdate->wvCameraForward = cam.wvCameraForward;
                            for (int32 i = 0; i < tParams.nTexStageCount; ++i)
                            {
                                ptRenderUpdate->atTexFramesInfo[i] = atTexFramesInfo[i];
                            }
                            ptGlobals->nRenderUpdateJobCount++;
                        }
                        else
                        {

                            ParticleSystemGenerateVertices(ptParticleSystem, ptVertices, tParams, wpPlayerPos, cam.tFrustum, cam.wvCameraForward, atTexFramesInfo, flDeltaTime);
                        }


                        ScopedShaderDefinitionLockR pdefShader(ptParticleSystem->snoShader);

                        Renderable tRenderable(RENDERABLETYPE_BILLBOARDPARTICLE,

                                              ptParticleSystem->id,
                                              PERFBIN_PARTICLES,
                                              pdefShader,
                                              pdefParticle->tUberMaterial.mat);
                        RenderableHeader & tHeader = tRenderable.header;
                        RenderableDetails & tDetails = tRenderable.details;

                        if ((ptParticleSystem->dwDefFlags & (1 << PARTICLE_DEF_DYNAMICALLY_LIT)) && (ptParticleSystem->dwFlags & (1 << PARTICLE_SYSTEM_VISIBLE_BIT)))
                        {
                            // Inference: eContext is RenderContext in DWARF; current header stores it as int32.
                            tDetails.tCommon.nLightCount = LightsGetDynamic(
                                (RenderContext)ptParticleSystem->eContext,
                                ptParticleSystem->wsBounds,
                                16,
                                8,
                                tDetails.tCommon.pArrayLights, NULL, NULL);
                        }
                        else
                        {
                            tDetails.tCommon.nLightCount = 0;
                        }

                        tDetails.tParticle.dwBillboardFlags = tParams.dwFlags;
                        tDetails.tParticle.dwVertexStartIndex = dwVertexStartIndex;
                        tDetails.tParticle.dwVertexCount = dwVertexCount;

                        for (int32 i = 0; i < NUM_BILLBOARD_TEX_STATES; ++i)
                        {
                            if (tParams.ptTextureDef[i])
                            {
                                tDetails.tParticle.snoTex[i] = tParams.ptTextureDef[i]->tHeader.snoHandle;
                            }
                            else
                            {
                                tDetails.tParticle.snoTex[i] = SNO_NONE;
                            }
                        }

                        tDetails.tParticle.nNumInstances = 0;
                        tDetails.tParticle.pwvInstanceOffsets = NULL;

                        tHeader.SetDist(Vector3D::DistanceSquared(ptParticleSystem->wp, cam.cmState.transform.wp));
                        tHeader.SetRectFromPoint(ptParticleSystem->wp);
                        dispList.SubmitRenderable(tRenderable);
                        if (pdispListReflection)
                        {
                            for (int32 i = 0; i < PostFXGetNumReflectionPlanes(); ++i)
                            {
                                const Camera & tCamera = CameraManagerGetCamera(CAMERA_REFLECTION_0 + i);
                                const Plane & tPlane = PostFXGetReflectionPlane(i);

                                if (CameraComputeSphereVisibility(tCamera, ptParticleSystem->wsBounds))
                                {
                                    float flDist = tPlane.ContainsPoint(ptParticleSystem->wsBounds.wpCenter);
                                    if (flDist <= ptParticleSystem->wsBounds.wdRadius + 6.0 &&
                                        flDist >= -ptParticleSystem->wsBounds.wdRadius)
                                    {
                                        pdispListReflection[i].SubmitRenderable(tRenderable);
                                    }
                                }
                            }
                        }

                        sParticleSystemUnlockTextures(ptParticleSystem);
                    }
                }
            }
        }
    }

}



void sUnlockParticleTextures()
{
    XGrowableMap<int, int, ParticleTextures, const ParticleTextures &> * ptMapParticleTextures = CGameGet().ptParticleGlobals->ptMapParticleTextures;

    POS pos = ptMapParticleTextures->GetStartAssoc();
    while (pos)
    {
        SNO snoParticle;

        ParticleTextures * ptTextures = ptMapParticleTextures->GetNextAssocPtr(pos, snoParticle);
        if (ptTextures->fLocked)
        {
            for (int32 i = 0; i < NUM_BILLBOARD_TEX_STATES; i++)
            {
                if (ptTextures->ptTextures[i])
                {
                    gptGroupDefTexture->UnlockR((void *&)ptTextures->ptTextures[i]);
                }
            }

            ptTextures->fLocked = FALSE;
        }
    }
}


void ParticlesRenderUpdate(
    RWindow & tRWindow,
    const float flDeltaTime)
{


    CGame & tCGame = CGameGet();

    ParticleGlobals * ptGlobals = tCGame.ptParticleGlobals;

    ptGlobals->tParticleCounts.nParticleVisibleCount = ptGlobals->tParticleCounts.nSystemVisibleCount = 0;


    if (!ptGlobals->ptRenderUpdateVBuffer)
        return;




    DebugRenderMode eDRM = sgtAppGlobals.eDebugRenderMode;
    if (AppDrawFlagIsSet(APP_DRAW_NO_PARTICLES_BIT) || eDRM == DRM_AO || eDRM == DRM_AO_SM || eDRM == DRM_ENVMAP || eDRM == DRM_GLOSS)
        return;







    const Camera & cam = CameraManagerGetCamera(tRWindow.GetCameraIndex());
    AsyncRWindowData & tAsyncRVData = tRWindow.tAsyncData[CGameAsyncGetRenderDataIndex(FALSE)];

    BOOL fDistortionEnabled = PostFXEnabled();
    ActorCommonData * ptACDPlayer = ACDTryToGet(LocalPlayerACDGet());
    Vector3D wpPlayerPos = ptACDPlayer ? ptACDPlayer->GetWorldLocation().transform.wp : cam.cmState.transform.wp;


    ptGlobals->nRenderUpdateJobCount = 0;
    ptGlobals->sParticleSystemRenderUpdateList.EmptyArray();



    tCGame.ptParticleGlobals->fInMainParticleRenderUpdate = TRUE;
    TDataArrayIterator<ParticleSystem> tIt;
    while (tCGame.ptParticleSystems->Iterate(tIt))
    {
        ParticleSystem * ptParticleSystem = tIt.ptElement;

        if (!fDistortionEnabled && (ptParticleSystem->dwFlags & (1 << PARTICLE_SYSTEM_CAUSES_DISTORTION)))
            continue;




        if (ptParticleSystem->eContext != tAsyncRVData.dispList.GetRenderContext())
            continue;







        ScopedParticleSystemLock tLock(ptParticleSystem);

        sParticleSystemRenderUpdate(ptParticleSystem, cam, tAsyncRVData.dispList, tAsyncRVData.dispListReflections, &tAsyncRVData.dispListProjected, wpPlayerPos, ptACDPlayer != NULL, flDeltaTime);

        if (AppDrawFlagIsSet(APP_DRAW_DISPLAY_PARTICLE_NAMES_BIT))
        {
            if (!(ptParticleSystem->dwDefFlags & (1 << PARTICLE_DEF_ATTACHED_TO_CAMERA)))
            {
                Vector3D wp;


                wp = ptParticleSystem->wp;







                DebugTextSubmit(SNOToString(PARTICLE_SNO, ptParticleSystem->snoParticle).str(), wp, tRWindow);
            }
        }
    }








    tCGame.ptParticleGlobals->fInMainParticleRenderUpdate = FALSE;






}




void ParticlesMultiRenderUpdateKick()
{
    CGame & tCGame = CGameGet();
    ParticleGlobals * ptGlobals = tCGame.ptParticleGlobals;
    int32 nRenderUpdateJobCount = ptGlobals->nRenderUpdateJobCount;
    for (int32 i = 0; i < nRenderUpdateJobCount; i++)
    {
        ParticleSystemRenderUpdateRecord * ptRenderUpdate = &ptGlobals->sParticleSystemRenderUpdateList[i];
        JobManager::Get()->PushJob(sParticleSystemRenderUpdateJobExecute, ptRenderUpdate);
    }

    JobManager::Get()->PushingCompleted();
}







void ParticlesMultiRenderUpdateFinish()
{

    sUnlockParticleTextures();





    JobManager::Get()->WaitForIdle();
}






void ParticlesUpdateFullscreenParticles()
{





    CGame & tCGame = CGameGet();
    const Camera & tCamera = CameraManagerGetCamera(-1);

    TDataArrayIterator<ParticleSystem> tIt;
    while (tCGame.ptParticleSystems->Iterate(tIt))
    {
        ParticleSystem * ptParticleSystem = tIt.ptElement;
        ScopedParticleSystemLock tLock(ptParticleSystem);


        const ParticleDefinition * pdefParticle = ptParticleSystem->GetParticleDef();
        if (pdefParticle->nFacing == PARTICLE_FACING_FULL_SCREEN)
        {
            uint32 nCount = ptParticleSystem->tParticleList.size();
            for (uint32 pos = 0; pos < nCount; pos++)
            {
                Particle * ptParticle = ptParticleSystem->tParticleList[pos];





                ptParticle->wp = tCamera.cmState.transform.wp + tCamera.wvCameraForward * (tCamera.cmState.flFarPlane * 31.0f / 32.0f);

                ptParticle->tParams.flScale = ptParticle->tParams.flBaseScale * ptParticleSystem->flScaleFrac;
                ptParticle->tParams.fStretch = 1.0f;

                ptParticle->tParams.flScale *= 2.0f * tCamera.flFarWidth;
                ptParticle->tParams.fStretch = tCamera.flFarHeight / tCamera.flFarWidth;
            }
        }
    }
}


void ParticleSystem::RenderInDisplayList(const Camera & cam, DisplayList & tDisplayList, const float flDeltaTime)
{


    if (AppDrawFlagIsSet(21))
        return;


    if (eContext != tDisplayList.GetRenderContext())
        return;


    CGame & tCGame = CGameGet();
    ParticleGlobals * ptGlobals = tCGame.ptParticleGlobals;


    if (!ptGlobals->ptRenderUpdateVBuffer)
        return;

    ActorCommonData * ptACDPlayer;




    ptACDPlayer = ACDTryToGet(LocalPlayerACDGet());
    Vector3D wpPlayerPos = ptACDPlayer ? ptACDPlayer->GetWorldLocation().transform.wp : cam.cmState.transform.wp;

    ScopedParticleSystemLock tLock(this);
    sParticleSystemRenderUpdate(this, cam, tDisplayList, NULL, NULL, wpPlayerPos, ptACDPlayer != NULL, flDeltaTime);
}






void ParticleSystem::SetTransform(const PRTransform & transform)
{

    if (tParentData.m_eParentType == PARENT_NONE)
    {
        return;
    }


    if (dwDefFlags & (1 << PARTICLE_DEF_ATTACHED_TO_CAMERA))
    {
        return;
    }


    sParticleSystemMove(this, transform);
}




void ParticlesGetCount(ParticleCounts & tParticleCounts)
{
    CGame & tCGame = CGameGet();
    ParticleGlobals * ptGlobals = tCGame.ptParticleGlobals;


    tParticleCounts = ptGlobals->tParticleCounts;


    tParticleCounts.nSystemCurrentTotal = tCGame.ptParticleSystems->GetTotalCount();
    tParticleCounts.nParticlesCurrentTotal = ptGlobals->ptParticles->GetTotalCount();


}









































static void sParticleAttachmentGetAttachmentScale(const int32 nParentType, const DataID idParent, float & flScale)
{



    switch (nParentType)
    {
        case PARENT_PARTICLE:
        {
            Particle * ptParticle = sParticleGet(idParent);
            flScale = ptParticle->tParams.flScale;
            break;
        }

        case PARENT_RACTOR:
        case PARENT_RACTOR_TOP:
        {
            RActor * ptRActor = RActorGet((RActorID)idParent);
            flScale = ptRActor->ptParticleParams->flScale;
            break;
        }

        case PARENT_PARTICLE_SYSTEM:
        {
            ParticleSystem * ptParticleSystem = ParticleSystemGet(idParent);
            flScale = ptParticleSystem->flScaleFrac;
            break;
        }

        default:
            DisplayInternalError(NULL, "", 6079, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }
}




static void sParticleAttachmentGetAttachmentSpeed(const int32 nParentType, const DataID idParent, float & flSpeed)
{
    switch (nParentType)
    {
        case PARENT_RACTOR: case PARENT_RACTOR_TOP: case PARENT_PARTICLE_SYSTEM:
            flSpeed = 0.0f;
            break;
        case PARENT_PARTICLE:
        {
            Particle * ptParticle = sParticleGet(idParent);
            flSpeed = ptParticle->tParams.wdParticleSpeed;
            break;
        }














        default:
            DisplayInternalError(NULL, "", 6114, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }
}




static void sParticleAttachmentCreate(RMessage & msg, const BOOL fSpawn)
{

    PRTransform transform;
    ParentData tParentData;
    tParentData.SetHardpointIndex(msg.nParentType, msg.idParent, -1);

    if (!tParentData.ParentIsValid())
        return;

    float flSpeed;
    BOOL fOverrideScale;
    float flScale;
    tParentData.GetTransform(transform);

    sParticleAttachmentGetAttachmentScale(msg.nParentType, msg.idParent, flScale);
    sParticleAttachmentGetAttachmentSpeed(msg.nParentType, msg.idParent, flSpeed);

    if (msg.tTriggerEvent.fUseMinimumSpeed && flSpeed < msg.tTriggerEvent.flMinimumSpeed)
        return;






    fOverrideScale = !msg.tTriggerEvent.fIgnoreOwnerScale;





    if (!msg.tTriggerEvent.fUseParticleScalar)
    {


        msg.tTriggerEvent.fUseParticleScalar = TRUE;
        msg.tTriggerEvent.flParticleScalar = flSpeed;
    }

    if (!fOverrideScale)
    {
        flScale = 1.0f;
    }



    if (fSpawn)
        AttachmentSpawn(msg, tParentData, transform, flScale, fOverrideScale);
    else
        AttachmentCreate(msg, tParentData, transform, flScale, fOverrideScale);
}





static void sSendMsgToParticles(ParticleSystem * ptParticleSystem, const KeyedMessage & tMsg)
{


    if (ptParticleSystem->dwFlags & (1 << PARTICLE_SYSTEM_HAS_ACTORS))
    {

        POS pos = ptParticleSystem->tParticleRActorList.GetHeadPosition();
        while (pos)
        {
            RActorID idRActor = (RActorID)ptParticleSystem->tParticleRActorList.GetNext(pos);
            RActorSendMsgKey(idRActor, tMsg, MSGPASS_TO_SELF_AND_CHILDREN);
        }
    }
}




void ParticleSystem::SendMsgKey(const KeyedMessage & tMsg, const MsgPassMethod eMsgPassMethod)
{
    ScopedParticleSystemLock tLock(this);


    switch (eMsgPassMethod)
    {
    case MSGPASS_TO_THIS_ACTOR:


        ParticleAttachmentsOnMsg(*this, id, tMsg);
        break;
    case MSGPASS_TO_ACTORCHAIN:
    case MSGPASS_TO_SELF_AND_CHILDREN:
        ParticleAttachmentsOnMsg(*this, id, tMsg);

        AttachmentListSendMsgToChildren(tAttachmentList, tMsg);
        sSendMsgToParticles(this, tMsg);
        break;
    case MSGPASS_TO_CHILDREN_ONLY:


        AttachmentListSendMsgToChildren(tAttachmentList, tMsg);
        sSendMsgToParticles(this, tMsg);
        break;
    }
}



static void sHandleMsg(const DataID id, RMessage & msg)
{

    const ParticleSystemID idParticleSystem = (ParticleSystemID)id;

    ParticleSystem * ptParticleSystem = ParticleSystemGet(idParticleSystem);

    const TriggerConditions & tConditions = msg.tTriggerEvent.tConditions;

    if (tConditions.dwRepeatTimeMin || tConditions.dwRepeatTimeDelta)
    {

        BOOL bRemote = (msg.tTriggerEvent.dwTriggerFlags & (1 << TRIGGERFLAG_DISABLED_IF_REMOTE_PLAYER)) ? TRUE : FALSE;
        ptParticleSystem->tRMsgList.SendMessage(msg.nParentType, msg.idParent, TRUE, msg.tTriggerEvent, msg.flImpulse, bRemote);
    }

    switch (msg.tTriggerEvent.eType)
    {
    case TRIGGEREDEVENTTYPE_ADD_OBJECT:

        sParticleAttachmentCreate(msg, FALSE);
        break;

    case TRIGGEREDEVENTTYPE_DETACH_OBJECT:

        AttachmentFindAndDetach(ptParticleSystem->tAttachmentList, msg.tTriggerEvent.dwID);
        break;

    case TRIGGEREDEVENTTYPE_DELETE_OBJECT:
        AttachmentFindAndDelete(ptParticleSystem->tAttachmentList, msg.tTriggerEvent.dwID, FALSE);
        break;
    case TRIGGEREDEVENTTYPE_SPAWN_OBJECT:
        sParticleAttachmentCreate(msg, TRUE);
        break;
    case TRIGGEREDEVENTTYPE_OUTRO_OBJECT:
        AttachmentFindAndDelete(ptParticleSystem->tAttachmentList, msg.tTriggerEvent.dwID, TRUE);
        break;




    case TRIGGEREDEVENTTYPE_CHANGE_LOOK:
        RActorChangeLook((RActorID)ptParticleSystem->tParentData.m_idParent, msg.tTriggerEvent.tLookLink.dwLookHash);
        break;

    case TRIGGEREDEVENTTYPE_ADD_EFFECT_GROUP:
    {
        DataID idParents[3] = { msg.idParent, (DataID)-1, (DataID)-1 };


        EffectGroupAttachmentAdd(id, idParents, msg.nParentType, msg.tTriggerEvent);
        break;
    }
    case TRIGGEREDEVENTTYPE_MSG:
        ptParticleSystem->SendMsgKey(KeyedMessage(msg.tTriggerEvent.dwMsgKey, 0.0f, Shape::DEFAULT_MATERIAL), msg.tTriggerEvent.eMsgPassMethod);
        break;







    case TRIGGEREDEVENTTYPE_CANCEL_LOOK:
        RActorCancelLook((RActorID)ptParticleSystem->tParentData.m_idParent, msg.tTriggerEvent.tLookLink.dwLookHash);
        break;
    }
}











void ParticleSetLastSeenTime(Frustum & tFrustum)
{

    uint32 dwNow = GameGetServerTime();
    TDataArrayIterator<ParticleSystem> tIt;

    CGame & tCGame = CGameGet();

    while (tCGame.ptParticleSystems->Iterate(tIt))
    {
        ParticleSystem * ptParticleSystem = tIt.ptElement;

        BOOL bInside = Intersect::FrustumAndSphere(tFrustum, ptParticleSystem->wsBounds);

        if (bInside)
        {
            ptParticleSystem->dwParticleLastSeen = dwNow;
        }
    }
}



static void sParticleDeactivate(ParticleSystem * ptParticleSystem)
{
    SceneCommonData * ptSCD = SceneCommonDataTryToGet(ptParticleSystem->idSScene);
    if (ptSCD)
    {









































        if (ptParticleSystem->tInactiveMarkerReferenceList.Count() == 0)
        {




            InactiveMarkerCreateFromMarkerSet(*ptSCD, ptParticleSystem->tMarkerOwner, PRTransform(ptParticleSystem->qOrientation, ptParticleSystem->wp), ptParticleSystem->wsBounds);
        }
        else
        {
            POS pos = ptParticleSystem->tInactiveMarkerReferenceList.GetHeadPosition();
            while (pos)
            {
                InactiveMarkerReference & markerRef = ptParticleSystem->tInactiveMarkerReferenceList.GetNext(pos);




                InactiveMarkerCreateFromMarkerSet(*ptSCD, markerRef.tMarkerOwner, PRTransform(markerRef.qOrientation, markerRef.wp), markerRef.wsBounds);
            }
        }
    }

    ParticleSystemDelete(ptParticleSystem->id);
}

#line 6399
void ParticleCheckSeenRecently()
{

    uint32 dwNow = GameGetServerTime();
    TDataArrayIterator<ParticleSystem> tIt;

    CGame & tCGame = CGameGet();

    while (tCGame.ptParticleSystems->Iterate(tIt))
    {
        ParticleSystem * ptParticleSystem = tIt.ptElement;

        if (ptParticleSystem->tMarkerOwner.snoMarkerSet != SNO_NONE && ptParticleSystem->idSScene != SSCENEID_INVALID && ptParticleSystem->dwParticleLastSeen < dwNow)
        {
            sParticleDeactivate(ptParticleSystem);
        }
    }
}
#line 6421













void ParticleDeactivateInScene(SSceneID idSScene)
{

    CGame & tCGame = CGameGet();

    if (!tCGame.ptParticleSystems)
    {
        return;
    }

    TDataArrayIterator<ParticleSystem> tIt;
    while (tCGame.ptParticleSystems->Iterate(tIt))
    {
        ParticleSystem * ptParticleSystem = tIt.ptElement;

        if (ptParticleSystem->idSScene != idSScene)
        {
            continue;
        }

        if (ptParticleSystem->tMarkerOwner.snoMarkerSet == SNO_NONE)
        {
            continue;
        }





        sParticleDeactivate(ptParticleSystem);
    }
}





















































void ParticleSystemsIntersectSphere(const Sphere & tSphere, XPooledDataIDList & listParticleSystems)
{

    CGame & tCGame = CGameGet();
    TDataArrayIterator<ParticleSystem> tIt;
    while (tCGame.ptParticleSystems->Iterate(tIt))
    {
        ParticleSystem * ptParticleSystem = tIt.ptElement;

        if (Intersect::SphereAndSphere(tSphere, ptParticleSystem->wsBounds))
        {
            listParticleSystems.AddTail_Tracked(ptParticleSystem->id);
        }
    }
}







void ParticleSystem::ApplyExplosion(const ExplosionImpulse & tExplosion)
{
    if (eType != PSTYPE_PHYS_BILLBOARD)
    {
        return;
    }




    const PhysicsDefinition * pdefPhysics = (const PhysicsDefinition *)gptGroupDefPhysics->LockRFast(tPhysicsSystem.snoPhysics);

    if (pdefPhysics->dwFlags & (1 << PHYSICSDEF_AFFECTED_BY_EXPLOSIONS_BIT))
    {
        uint32 nCount = tParticleList.size();
        for (uint32 pos = 0; pos < nCount; pos++)
        {
            Particle * ptParticle = tParticleList[pos];

            PointMass & pm = *ptParticle->tParams.ptPointMass;
            pm.wv += pm.flInvMass * tExplosion.ComputeImpulse(pm.wp);
        }
    }


}



static const float cflDisturbanceFudge = 0.05f;
static float cflExplosionFudge = 100.0f;







// NON_MATCHING: ours folds -wvParallel.x into the dot product (fsub, one fmul); the original keeps a separate fnmul and fadd
void PosVelFoliageTask::Func(const void * pDataIn)
{
    const PosVelFoliageTask * pData = (const PosVelFoliageTask *)pDataIn;

    Vector2D wvParallel = { pData->wvVelocity.x * cflDisturbanceFudge, pData->wvVelocity.y * cflDisturbanceFudge };
    Vector2D wvPerpendicular = { wvParallel.y, -wvParallel.x };

    CGame & tCGame = CGameGet();
    ParticleGlobals * ptParticleGlobals = tCGame.ptParticleGlobals;
    XSphereArray * ptSphereTree = ptParticleGlobals->ptFoliageSphereTree;

    XSphereNodeList listSpheres(ptParticleGlobals->pDisturbFoliageSphereNodeMemoryPool);
    ptSphereTree->PointCull(pData->wp, listSpheres);

    POS pos = listSpheres.GetHeadPosition();
    while (pos)
    {
        const XSphereNode * ptSphereNode = listSpheres.GetNext(pos);

        uintptr dwId = (uintptr)ptSphereNode->GetUserData();
        Particle * ptParticle = ptParticleGlobals->ptParticles->ElementGet((ParticleID)dwId);

        // DWARF has no wvChange local: the two velocity updates are separate branches (6604 test, 6605-6611).
        Vector2D wvHorizontalDelta = { ptParticle->wp.x - pData->wp.x, ptParticle->wp.y - pData->wp.y };
        if (Vector2D::DotProduct(wvPerpendicular, wvHorizontalDelta) > 0.0f)
        {
            ptParticle->tParams.tFoliageState.wvVelocity += Vector2D(wvParallel.x + wvPerpendicular.x, wvParallel.y + wvPerpendicular.y);
        }
        else
        {
            ptParticle->tParams.tFoliageState.wvVelocity += Vector2D(wvParallel.x - wvPerpendicular.x, wvParallel.y - wvPerpendicular.y);
        }
    }
}






void ExplosionDisturbFoliageTask::Func(const void * pDataIn)
{
    const ExplosionDisturbFoliageTask * pData = (const ExplosionDisturbFoliageTask *)pDataIn;

    CGame & tCGame = CGameGet();
    ParticleGlobals * ptParticleGlobals = tCGame.ptParticleGlobals;
    XSphereArray * ptSphereTree = ptParticleGlobals->ptFoliageSphereTree;

    XSphereNodeList listSpheres(ptParticleGlobals->pDisturbFoliageSphereNodeMemoryPool);
    Sphere wsExplosion;
    wsExplosion.Set(pData->tExplosion.m_wpCenter, pData->tExplosion.m_flMinRadius + pData->tExplosion.m_flDeltaRadius);
    ptSphereTree->SphereCull(wsExplosion, listSpheres);

    POS pos = listSpheres.GetHeadPosition();
    while (pos)
    {
        const XSphereNode * ptSphereNode = listSpheres.GetNext(pos);

        uintptr dwId = (uintptr)ptSphereNode->GetUserData();
        Particle * ptParticle = ptParticleGlobals->ptParticles->ElementGet((ParticleID)dwId);

        Vector3D wvImpulse = pData->tExplosion.ComputeImpulse(ptParticle->wp);

        float flFactor = ptParticle->tParams.tFoliageState.flExplosionFactor * cflExplosionFudge;

        ptParticle->tParams.tFoliageState.wvVelocity.x += flFactor * wvImpulse.x;
        ptParticle->tParams.tFoliageState.wvVelocity.y += flFactor * wvImpulse.y;
    }
}


static void sParticleSystemDisturbFoliageJob(void * pParam)
{
    XCommandBuffer * pBuffer = (XCommandBuffer *)pParam;
    pBuffer->Flush();
}



void ParticleSystemDisturbFoliageKick()
{

    CGame & tCGame = CGameGet();
    JobManager::Get()->PushJob(sParticleSystemDisturbFoliageJob, tCGame.ptParticleGlobals->pDisturbFoliageTaskBuffer);
    JobManager::Get()->PushingCompleted();
}








void ParticleSystemDisturbFoliageWait()
{




    JobManager::Get()->WaitForIdle();
}





void ParticleSystemDisturbFoliage(const Vector3D & wp, const Vector3D & wvVelocity)
{

    const float cflMinVelocitySquared = 10.0f;


    if (wvVelocity.MagnitudeSquared() < cflMinVelocitySquared)
    {

        return;
    }


    ACDID idACDPlayer = LocalPlayerACDGet();
    ActorCommonData * ptACDPlayer = ACDTryToGet(idACDPlayer);
    if (ptACDPlayer)
    {
        float flClutterDistanceMax = ParticleGetClutterDistance();
        Vector3D wpPlayer;
        wpPlayer = ptACDPlayer->GetWorldLocation().transform.wp;
        float flDistanceSquared = Vector3D::DistanceSquared(wpPlayer, wp);
        if (flDistanceSquared > flClutterDistanceMax * flClutterDistanceMax)
        {

            return;
        }
    }
    CGame & tCGame = CGameGet();
    ParticleGlobals * ptParticleGlobals = tCGame.ptParticleGlobals;


    PosVelFoliageTask * pTask = (PosVelFoliageTask *)ptParticleGlobals->pDisturbFoliageTaskBuffer->Reserve(PosVelFoliageTask::Func, sizeof(PosVelFoliageTask));
    pTask->wp = wp;
    pTask->wvVelocity = wvVelocity;





























}




void ParticleSystemExplodeFoliage(const ExplosionImpulse & tExplosion)
{



    ACDID idACDPlayer = LocalPlayerACDGet();
    ActorCommonData * ptACDPlayer = ACDTryToGet(idACDPlayer);
    if (ptACDPlayer)
    {
        float flClutterDistanceMax = ParticleGetClutterDistance();
        Vector3D wpPlayer;
        wpPlayer = ptACDPlayer->GetWorldLocation().transform.wp;
        float flDistanceSquared = Vector3D::DistanceSquared(wpPlayer, tExplosion.m_wpCenter);
        float flRadius = tExplosion.m_flMinRadius + tExplosion.m_flDeltaRadius;
        float flRadiusSquared = flRadius * flRadius;
        if (flDistanceSquared - flRadiusSquared > flClutterDistanceMax * flClutterDistanceMax)
        {

            return;
        }
    }
    CGame & tCGame = CGameGet();
    ParticleGlobals * ptParticleGlobals = tCGame.ptParticleGlobals;

    ExplosionDisturbFoliageTask * pTask = (ExplosionDisturbFoliageTask *)ptParticleGlobals->pDisturbFoliageTaskBuffer->Reserve(ExplosionDisturbFoliageTask::Func, sizeof(ExplosionDisturbFoliageTask));
    pTask->tExplosion = tExplosion;
























}




BOOL ParticleSystemUpdateParamsForRActor(RActor * ptRActor, Vector3D & wpActorResult, float flDeltaTime)
{





    WorldLocation & loc = ptRActor->tLocation.tWorldLocation;
    ParticleParams & tParams = *ptRActor->ptParticleParams;


    ParticleSystem * ptParticleSystem = ParticleSystemGet(tParams.idParticleSystem);

    ScopedParticleSystemLock tLock(ptParticleSystem);


    Vector3D wpActor = loc.transform.wp;

    ParticleCounts & tParticleCounts = CGameGet().ptParticleGlobals->tParticleCounts;
    BOOL bDelete = ParticleSystemUpdateParams(ptParticleSystem, FALSE, ptRActor->id, tParams, wpActor, TRUE, TRUE, flDeltaTime);
    tParticleCounts.nParticleUpdatedCount++;


    wpActorResult = wpActor;


    return bDelete;
}




ParticleSystem * ParticleSystemGet(ParticleSystemID idParticleSystem)
{

    return CGameGet().ptParticleSystems->ElementGet(idParticleSystem);
}




ParticleSystem * ParticleSystemTryToGet(ParticleSystemID idParticleSystem)
{
    ParticleSystem * ptParticleSystem = NULL;
    CGame & tCGame = CGameGet();
    if (tCGame.ptParticleSystems && idParticleSystem != PARTICLESYSTEMID_INVALID)
    {




        ptParticleSystem = tCGame.ptParticleSystems->ElementTryToGet(idParticleSystem);
    }
    return ptParticleSystem;
}



















static uint32 sInitializeConstantFlags(const ParticleDefinition * ptParticleDef)
{

    uint32 uConstFlags = 0;

    if (gEnableConstPathOptimization)
    {
        if (!ptParticleDef->pathParticleColor || (ptParticleDef->pathParticleColor->tHeader.nNodeCount == 1 && ptParticleDef->pathParticleColor->tHeader.tInterpScalar.nScalarFunction == SCALAR_FUNCTION_NONE && ptParticleDef->pathParticleColor->ptArrayNodes[0].tValueMax == ptParticleDef->pathParticleColor->ptArrayNodes[0].tValueMin)) uConstFlags |= (1 << PARTICLE_PATH_CONST_COLOR);
        if (!ptParticleDef->pathParticleAlpha || (ptParticleDef->pathParticleAlpha->tHeader.nNodeCount == 1 && ptParticleDef->pathParticleAlpha->tHeader.tInterpScalar.nScalarFunction == SCALAR_FUNCTION_NONE && ptParticleDef->pathParticleAlpha->ptArrayNodes[0].tValueMin == ptParticleDef->pathParticleAlpha->ptArrayNodes[0].tValueMax)) uConstFlags |= (1 << PARTICLE_PATH_CONST_ALPHA);
        if (!ptParticleDef->pathParticleScale || (ptParticleDef->pathParticleScale->tHeader.nNodeCount == 1 && ptParticleDef->pathParticleScale->tHeader.tInterpScalar.nScalarFunction == SCALAR_FUNCTION_NONE && ptParticleDef->pathParticleScale->ptArrayNodes[0].tValueMin == ptParticleDef->pathParticleScale->ptArrayNodes[0].tValueMax)) uConstFlags |= (1 << PARTICLE_PATH_CONST_SCALE);
        if (!ptParticleDef->pathParticleStretch || (ptParticleDef->pathParticleStretch->tHeader.nNodeCount == 1 && ptParticleDef->pathParticleStretch->tHeader.tInterpScalar.nScalarFunction == SCALAR_FUNCTION_NONE && ptParticleDef->pathParticleStretch->ptArrayNodes[0].tValueMin == ptParticleDef->pathParticleStretch->ptArrayNodes[0].tValueMax)) uConstFlags |= (1 << PARTICLE_PATH_CONST_STRETCH);
        if (!ptParticleDef->pathAngularPosition || (ptParticleDef->pathAngularPosition->tHeader.nNodeCount == 1 && ptParticleDef->pathAngularPosition->tHeader.tInterpScalar.nScalarFunction == SCALAR_FUNCTION_NONE && ptParticleDef->pathAngularPosition->ptArrayNodes[0].tValueMin == ptParticleDef->pathAngularPosition->ptArrayNodes[0].tValueMax)) uConstFlags |= (1 << PARTICLE_PATH_CONST_ANGULAR_POSITION);
        if (!ptParticleDef->pathAngularVelocity || (ptParticleDef->pathAngularVelocity->tHeader.nNodeCount == 1 && ptParticleDef->pathAngularVelocity->tHeader.tInterpScalar.nScalarFunction == SCALAR_FUNCTION_NONE && ptParticleDef->pathAngularVelocity->ptArrayNodes[0].tValueMin == ptParticleDef->pathAngularVelocity->ptArrayNodes[0].tValueMax)) uConstFlags |= (1 << PARTICLE_PATH_CONST_ANGULAR_VELOCITY);
        if (!ptParticleDef->pathOrientationAxis || (ptParticleDef->pathOrientationAxis->tHeader.nNodeCount == 1 && ptParticleDef->pathOrientationAxis->tHeader.tInterpScalar.nScalarFunction == SCALAR_FUNCTION_NONE && ptParticleDef->pathOrientationAxis->ptArrayNodes[0].tValueMin == ptParticleDef->pathOrientationAxis->ptArrayNodes[0].tValueMax)) uConstFlags |= (1 << PARTICLE_PATH_CONST_ORIENTATION_AXIS);
        if (!ptParticleDef->pathMotionCylinderRadialVel || (ptParticleDef->pathMotionCylinderRadialVel->tHeader.nNodeCount == 1 && ptParticleDef->pathMotionCylinderRadialVel->tHeader.tInterpScalar.nScalarFunction == SCALAR_FUNCTION_NONE && ptParticleDef->pathMotionCylinderRadialVel->ptArrayNodes[0].tValueMin == ptParticleDef->pathMotionCylinderRadialVel->ptArrayNodes[0].tValueMax)) uConstFlags |= (1 << PARTICLE_PATH_CONST_MOTION_CYLINDER_RADIAL_VEL);
        if (!ptParticleDef->pathMotionCylinderAngleVel || (ptParticleDef->pathMotionCylinderAngleVel->tHeader.nNodeCount == 1 && ptParticleDef->pathMotionCylinderAngleVel->tHeader.tInterpScalar.nScalarFunction == SCALAR_FUNCTION_NONE && ptParticleDef->pathMotionCylinderAngleVel->ptArrayNodes[0].tValueMin == ptParticleDef->pathMotionCylinderAngleVel->ptArrayNodes[0].tValueMax)) uConstFlags |= (1 << PARTICLE_PATH_CONST_MOTION_CYLINDER_ANGLE_VEL);
        if (!ptParticleDef->pathMotionCylinderAxis || (ptParticleDef->pathMotionCylinderAxis->tHeader.nNodeCount == 1 && ptParticleDef->pathMotionCylinderAxis->tHeader.tInterpScalar.nScalarFunction == SCALAR_FUNCTION_NONE && ptParticleDef->pathMotionCylinderAxis->ptArrayNodes[0].tValueMin == ptParticleDef->pathMotionCylinderAxis->ptArrayNodes[0].tValueMax)) uConstFlags |= (1 << PARTICLE_PATH_CONST_MOTION_CYLINDER_AXIS);
        if (!ptParticleDef->pathMotionCartesianPos || (ptParticleDef->pathMotionCartesianPos->tHeader.nNodeCount == 1 && ptParticleDef->pathMotionCartesianPos->tHeader.tInterpScalar.nScalarFunction == SCALAR_FUNCTION_NONE && ptParticleDef->pathMotionCartesianPos->ptArrayNodes[0].tValueMin == ptParticleDef->pathMotionCartesianPos->ptArrayNodes[0].tValueMax)) uConstFlags |= (1 << PARTICLE_PATH_CONST_MOTION_CARTESIAN_POS);
        if (!ptParticleDef->pathMotionCartesianVel || (ptParticleDef->pathMotionCartesianVel->tHeader.nNodeCount == 1 && ptParticleDef->pathMotionCartesianVel->tHeader.tInterpScalar.nScalarFunction == SCALAR_FUNCTION_NONE && ptParticleDef->pathMotionCartesianVel->ptArrayNodes[0].tValueMin == ptParticleDef->pathMotionCartesianVel->ptArrayNodes[0].tValueMax)) uConstFlags |= (1 << PARTICLE_PATH_CONST_MOTION_CARTESIAN_VEL);
        if (!ptParticleDef->pathMotionCartesianAcc || (ptParticleDef->pathMotionCartesianAcc->tHeader.nNodeCount == 1 && ptParticleDef->pathMotionCartesianAcc->tHeader.tInterpScalar.nScalarFunction == SCALAR_FUNCTION_NONE && ptParticleDef->pathMotionCartesianAcc->ptArrayNodes[0].tValueMin == ptParticleDef->pathMotionCartesianAcc->ptArrayNodes[0].tValueMax)) uConstFlags |= (1 << PARTICLE_PATH_CONST_MOTION_CARTESIAN_ACC);
        if (!ptParticleDef->pathMotionRelativeCartesianPos || (ptParticleDef->pathMotionRelativeCartesianPos->tHeader.nNodeCount == 1 && ptParticleDef->pathMotionRelativeCartesianPos->tHeader.tInterpScalar.nScalarFunction == SCALAR_FUNCTION_NONE && ptParticleDef->pathMotionRelativeCartesianPos->ptArrayNodes[0].tValueMin == ptParticleDef->pathMotionRelativeCartesianPos->ptArrayNodes[0].tValueMax)) uConstFlags |= (1 << PARTICLE_PATH_CONST_MOTION_RELATIVE_CARTESIAN_POS);
        if (!ptParticleDef->pathMotionRelativeCartesianVel || (ptParticleDef->pathMotionRelativeCartesianVel->tHeader.nNodeCount == 1 && ptParticleDef->pathMotionRelativeCartesianVel->tHeader.tInterpScalar.nScalarFunction == SCALAR_FUNCTION_NONE && ptParticleDef->pathMotionRelativeCartesianVel->ptArrayNodes[0].tValueMin == ptParticleDef->pathMotionRelativeCartesianVel->ptArrayNodes[0].tValueMax)) uConstFlags |= (1 << PARTICLE_PATH_CONST_MOTION_RELATIVE_CARTESIAN_VEL);

        if (!ptParticleDef->ptExtendedPath_Console || !ptParticleDef->ptExtendedPath_Console->pathParticleAlphaAux || (ptParticleDef->ptExtendedPath_Console->pathParticleAlphaAux->tHeader.nNodeCount == 1 && ptParticleDef->ptExtendedPath_Console->pathParticleAlphaAux->tHeader.tInterpScalar.nScalarFunction == SCALAR_FUNCTION_NONE && ptParticleDef->ptExtendedPath_Console->pathParticleAlphaAux->ptArrayNodes[0].tValueMin == ptParticleDef->ptExtendedPath_Console->pathParticleAlphaAux->ptArrayNodes[0].tValueMax)) uConstFlags |= (1 << PARTICLE_PATH_CONST_ALPHAAUX);
        if (!ptParticleDef->ptExtendedPath_Console || !ptParticleDef->ptExtendedPath_Console->pathOrientationVelocity || (ptParticleDef->ptExtendedPath_Console->pathOrientationVelocity->tHeader.nNodeCount == 1 && ptParticleDef->ptExtendedPath_Console->pathOrientationVelocity->tHeader.tInterpScalar.nScalarFunction == SCALAR_FUNCTION_NONE && ptParticleDef->ptExtendedPath_Console->pathOrientationVelocity->ptArrayNodes[0].tValueMin == ptParticleDef->ptExtendedPath_Console->pathOrientationVelocity->ptArrayNodes[0].tValueMax)) uConstFlags |= (1 << PARTICLE_PATH_CONST_ORIENTATION_VELOCITY);
        if (!ptParticleDef->ptExtendedPath_Console || !ptParticleDef->ptExtendedPath_Console->pathOrientationPosition || (ptParticleDef->ptExtendedPath_Console->pathOrientationPosition->tHeader.nNodeCount == 1 && ptParticleDef->ptExtendedPath_Console->pathOrientationPosition->tHeader.tInterpScalar.nScalarFunction == SCALAR_FUNCTION_NONE && ptParticleDef->ptExtendedPath_Console->pathOrientationPosition->ptArrayNodes[0].tValueMin == ptParticleDef->ptExtendedPath_Console->pathOrientationPosition->ptArrayNodes[0].tValueMax)) uConstFlags |= (1 << PARTICLE_PATH_CONST_ORIENTATION_POSITION);
        if (!ptParticleDef->ptExtendedPath_Console || !ptParticleDef->ptExtendedPath_Console->pathMotionCylinderRadialPos || (ptParticleDef->ptExtendedPath_Console->pathMotionCylinderRadialPos->tHeader.nNodeCount == 1 && ptParticleDef->ptExtendedPath_Console->pathMotionCylinderRadialPos->tHeader.tInterpScalar.nScalarFunction == SCALAR_FUNCTION_NONE && ptParticleDef->ptExtendedPath_Console->pathMotionCylinderRadialPos->ptArrayNodes[0].tValueMin == ptParticleDef->ptExtendedPath_Console->pathMotionCylinderRadialPos->ptArrayNodes[0].tValueMax)) uConstFlags |= (1 << PARTICLE_PATH_CONST_MOTION_CYLINDER_RADIAL_POS);
        if (!ptParticleDef->ptExtendedPath_Console || !ptParticleDef->ptExtendedPath_Console->pathMotionSphereRadialPos || (ptParticleDef->ptExtendedPath_Console->pathMotionSphereRadialPos->tHeader.nNodeCount == 1 && ptParticleDef->ptExtendedPath_Console->pathMotionSphereRadialPos->tHeader.tInterpScalar.nScalarFunction == SCALAR_FUNCTION_NONE && ptParticleDef->ptExtendedPath_Console->pathMotionSphereRadialPos->ptArrayNodes[0].tValueMin == ptParticleDef->ptExtendedPath_Console->pathMotionSphereRadialPos->ptArrayNodes[0].tValueMax)) uConstFlags |= (1 << PARTICLE_PATH_CONST_SPHERE_RADIAL_POS);
        if (!ptParticleDef->ptExtendedPath_Console || !ptParticleDef->ptExtendedPath_Console->pathMotionSphereRadialVel || (ptParticleDef->ptExtendedPath_Console->pathMotionSphereRadialVel->tHeader.nNodeCount == 1 && ptParticleDef->ptExtendedPath_Console->pathMotionSphereRadialVel->tHeader.tInterpScalar.nScalarFunction == SCALAR_FUNCTION_NONE && ptParticleDef->ptExtendedPath_Console->pathMotionSphereRadialVel->ptArrayNodes[0].tValueMin == ptParticleDef->ptExtendedPath_Console->pathMotionSphereRadialVel->ptArrayNodes[0].tValueMax)) uConstFlags |= (1 << PARTICLE_PATH_CONST_SPHERE_RADIAL_VEL);
        if (!ptParticleDef->ptExtendedPath_Console || !ptParticleDef->ptExtendedPath_Console->pathMotionRelativeCartesianAcc || (ptParticleDef->ptExtendedPath_Console->pathMotionRelativeCartesianAcc->tHeader.nNodeCount == 1 && ptParticleDef->ptExtendedPath_Console->pathMotionRelativeCartesianAcc->tHeader.tInterpScalar.nScalarFunction == SCALAR_FUNCTION_NONE && ptParticleDef->ptExtendedPath_Console->pathMotionRelativeCartesianAcc->ptArrayNodes[0].tValueMin == ptParticleDef->ptExtendedPath_Console->pathMotionRelativeCartesianAcc->ptArrayNodes[0].tValueMax)) uConstFlags |= (1 << PARTICLE_PATH_CONST_MOTION_RELATIVE_CARTESIAN_ACC);
        if (!ptParticleDef->ptExtendedPath_Console || !ptParticleDef->ptExtendedPath_Console->pathMotionAttractorVel || (ptParticleDef->ptExtendedPath_Console->pathMotionAttractorVel->tHeader.nNodeCount == 1 && ptParticleDef->ptExtendedPath_Console->pathMotionAttractorVel->tHeader.tInterpScalar.nScalarFunction == SCALAR_FUNCTION_NONE && ptParticleDef->ptExtendedPath_Console->pathMotionAttractorVel->ptArrayNodes[0].tValueMin == ptParticleDef->ptExtendedPath_Console->pathMotionAttractorVel->ptArrayNodes[0].tValueMax)) uConstFlags |= (1 << PARTICLE_PATH_CONST_MOTION_ATTRACTOR_VEL);
        if (!ptParticleDef->ptExtendedPath_Console || !ptParticleDef->ptExtendedPath_Console->pathMotionAttractorPos || (ptParticleDef->ptExtendedPath_Console->pathMotionAttractorPos->tHeader.nNodeCount == 1 && ptParticleDef->ptExtendedPath_Console->pathMotionAttractorPos->tHeader.tInterpScalar.nScalarFunction == SCALAR_FUNCTION_NONE && ptParticleDef->ptExtendedPath_Console->pathMotionAttractorPos->ptArrayNodes[0].tValueMin == ptParticleDef->ptExtendedPath_Console->pathMotionAttractorPos->ptArrayNodes[0].tValueMax)) uConstFlags |= (1 << PARTICLE_PATH_CONST_MOTION_ATTRACTOR_POS);
    }

    return uConstFlags;
}






















































static const Material gmatParticle = {
    {1.0f, 1.0f, 1.0f, 1.0f},
    {1.0f, 1.0f, 1.0f, 1.0f},
    {0.0f, 0.0f, 0.0f, 1.0f},
    {0.0f, 0.0f, 0.0f, 1.0f},
    0.0f,
    0
};





static void sParticleDrawBillboards(
    const RenderableHeader & tHeader,
    const RenderableDetails & tDetails,
    const VBID & vbid,
    const IBID & ibid,
    BOOL fSupportsLights)
{
    ParticleGlobals * ptParticleGlobals = CGameGet().ptParticleGlobals;

    uint32 dwTriCount = tDetails.tParticle.dwVertexCount / 4 * 2;

    int32 nPassIndex = 0;
    const ShaderDefinition * pdefShader = tHeader.pdefShader;

    GfxShaderSetPointLinearLightCount(nPassIndex, pdefShader, tDetails.tCommon.nLightCount, tDetails.tCommon.pArrayLights);

    GfxShaderInit(pdefShader, nPassIndex);

    GfxShaderSetIB(ibid, 0);
    GfxShaderSetStream(STREAMINDEX_DEFAULT, 0, vbid);

    GfxShaderSetTexture(GfxGetTextureBinding(SHADERTEX_DIFFUSE), tDetails.tParticle.snoTex[BTS_DIFFUSE]);
    GfxShaderSetTexture(GfxGetTextureBinding(SHADERTEX_ALPHA), tDetails.tParticle.snoTex[BTS_ALPHA]);
    GfxShaderSetTexture(GfxGetTextureBinding(SHADERTEX_ALPHA_LAYER_1), tDetails.tParticle.snoTex[BTS_ALPHA_LAYER_1]);
    GfxShaderSetTexture(GfxGetTextureBinding(SHADERTEX_ALPHA_LAYER_2), tDetails.tParticle.snoTex[BTS_ALPHA_LAYER_2]);
    if (!ptParticleGlobals->bUseClearedSoftParticleDepth[GfxGetWorkerIndex()])
    {
        if (GfxGetSystemID() == GFX_D3D11 || GfxGetSystemID() == GFX_PS4GNM || GfxGetSystemID() == GFX_XBOXONED3D)
        {
            GfxShaderSetTexture(GfxGetTextureBinding(SHADERTEX_DEPTH), PostFXGetBuffer(EFFECTBUFFER_DEPTH_PREV));
        }
        else
        {
            GfxShaderSetTexture(GfxGetTextureBinding(SHADERTEX_DEPTH), PostFXGetBuffer(EFFECTBUFFER_DEPTH));
        }
    }
    else
    {
        GfxShaderSetTexture(GfxGetTextureBinding(SHADERTEX_DEPTH), PostFXGetDepthBuffer(EFFECTBUFFER_DEFAULT_SOFTPARTICLE_DEPTH));
    }

    if (fSupportsLights)
    {
        GfxShaderSetLights(gmatParticle, pdefShader, tDetails.tCommon.nLightCount, tDetails.tCommon.pArrayLights);
    }

    BOOL bComputeNomals = (tDetails.tParticle.dwBillboardFlags & 8) != 0;
    BOOL bCustomOrientation = (tDetails.tParticle.dwBillboardFlags & 2) != 0;
    BOOL bBottomAligned = (tDetails.tParticle.dwBillboardFlags & 1) != 0;

    const ShaderConstBinding * ptBinding = GfxShaderGetConstantBinding(VSC_BILLBOARD_PARAMS);
    if (ptBinding)
    {
        ShaderFloat tParams = {bComputeNomals ? 1.0f : 0.0f, bCustomOrientation ? 1.0f : 0.0f, bBottomAligned ? 1.0f : 0.0f, 0.0f};
        GfxShaderSetConstant(ptBinding, tParams, 1);
    }

    ptBinding = GfxShaderGetConstantBinding(VSC_PLAYERPOS);
    if (ptBinding)
    {
        ShaderFloat tPos = {0.0f, 1.0f, 0.0f, 1.0f};
        GfxShaderSetConstant(ptBinding, tPos, 1);
    }

    GfxShaderDraw(DRAWMODE_TRIANGLELIST, tDetails.tParticle.dwVertexStartIndex, 0, tDetails.tParticle.dwVertexCount, 0, dwTriCount, tDetails.tCommon.ePerfBin);
}




void ParticleDrawSetUseClearedSoftParticleDepth(BOOL bUseClearedSoftParticleDepth)
{
    ParticleGlobals * ptParticleGlobals = CGameGet().ptParticleGlobals;

    ptParticleGlobals->bUseClearedSoftParticleDepth[GfxGetWorkerIndex()] = bUseClearedSoftParticleDepth;
}





void ParticleDrawBillboards(
    const RenderableHeader & tHeader,
    const RenderableDetails & tDetails,
    const RenderLayer eRenderLayer,
    BOOL fSupportsLights)
{




    ParticleGlobals * ptParticleGlobals = CGameGet().ptParticleGlobals;





    VBID vbid = ptParticleGlobals->vbid[(ptParticleGlobals->dwRenderUpdateIndex + NUM_PARTICLE_BILLBOARD_BUFFERS - 1) % NUM_PARTICLE_BILLBOARD_BUFFERS];
    IBID ibid = ptParticleGlobals->ibid;


    if (!GfxVBIsValid(vbid))
    {
        ParticlesGfxDispose();
        ParticlesGfxInit();
    }

    const Camera & cam = CameraManagerGetCamera(CAMERA_CURRENT);
    if (eRenderLayer != RENDERLAYER_SKY)
    {
        GfxWorldMatrixSet(cgm4x4Identity);
    }
    else
    {
        Matrix4x4 mWorld;
        mWorld.Set(cgm3x3Identity, cam.cmState.transform.wp);
        GfxWorldMatrixSet(mWorld);
    }

    if (tDetails.tParticle.nNumInstances == 0)
    {
        sParticleDrawBillboards(tHeader, tDetails, vbid, ibid, fSupportsLights);
    }
    else
    {
        for (int32 nInstance = 0; nInstance < tDetails.tParticle.nNumInstances; ++nInstance)
        {
            Vector3D wpPos;
            if (eRenderLayer != RENDERLAYER_SKY)
            {
                wpPos = tDetails.tParticle.pwvInstanceOffsets[nInstance];
            }
            else
            {
                wpPos = cam.cmState.transform.wp + tDetails.tParticle.pwvInstanceOffsets[nInstance];
            }

            Matrix4x4 mWorld;
            mWorld.Set(cgm3x3Identity, wpPos);
            GfxWorldMatrixSet(mWorld);

            sParticleDrawBillboards(tHeader, tDetails, vbid, ibid, fSupportsLights);
        }
    }
}




void ParticlesUpdateFixedUpdateFlag()
{

    CGame & tCGame = CGameGet();

    TDataArrayIterator<ParticleSystem> tIt;
    while (tCGame.ptParticleSystems->Iterate(tIt))
    {
        ParticleSystem * ptParticleSystem = tIt.ptElement;


        if (sIsTopmostParentInUI(ptParticleSystem))
            ptParticleSystem->dwFlags |= (1 << PARTICLE_SYSTEM_FIXED_UPDATE);
        else
            ptParticleSystem->dwFlags &= ~(1 << PARTICLE_SYSTEM_FIXED_UPDATE);
    }
}

