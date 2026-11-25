// Original file: X/Client/Physics/DominoClient.cpp
// Partly written: functions sit on their original lines (see the reference .cpp).

#include "../../../BlizzardCore/Include/Blizzard/Lock.h"
#include "../../../BlizzardCore/Include/Blizzard/Thread.h"
#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Contrib/Contrib/Domino/Include/dmApi.h"
#include "../../../Contrib/Contrib/Domino/Include/dmCallbacks.h"
#include "../../../Contrib/Contrib/Domino/Include/dmJobDispatcher.h"
#include "../../../Contrib/Contrib/Domino/Include/dmMath.h"
#include "../../../Contrib/Contrib/Domino/Include/dmStructures.h"
#include "../../Common/ThreadGlobals.h"
#include "../../Sigma/DataStructures/XCommandBuffer.h"
#include "../../Sigma/Math/Quaternion.h"
#include "../../Sigma/Math/Transform.h"
#include "../../Sigma/Math/Vector.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../CGame.h"
#include "../../Common/GameCommonData.h"
#include "DominoBody.h"
#include "DominoClient.h"

// X/Client/CWorld.cpp and X/Common/Shell/MainNX64.cpp.
BOOL CWorldPhysicsAreRetro();
void ConfigureDominoAsyncPhysicsThread(Blizzard::Thread::Thread thread);
#include "../../../Contrib/Contrib/Domino/Include/dmBuffer.h"
#include "../../../Contrib/Contrib/blz/include/blz/algo_sort.h"
#include "../../Common/Marker/Marker.h"
#include "../../Common/Definitions/PhysMeshDefinition.h"
bool operator!=(dmBodyId id1, dmBodyId id2);  // Contrib/Domino/Dynamics/dmBody.cpp:31
void dmWorld_DestroyBody(dmBodyId id);  // Contrib/Domino/Dynamics/dmWorld.cpp:2962


#include "../RActors/RActors.h"
#include "../../../Contrib/Contrib/Domino/Include/dmPolytopeData.h"
#include "../Definitions/AppearanceDefinition.h"
// X/Client/Physics/DominoPolytope.h:9 and X/Client/Definitions/AppearanceDefinition.h:108
// (the src AppearanceDefinition.h only forward-declares CollisionShape; same
// definition as in AppearanceDefinition.cpp).
struct PolytopeData : public dmPolytopeData {  // size 0x60
    SerializeData serVertices;  // 0x40
    SerializeData serPlanes;  // 0x48
    SerializeData serSubEdges;  // 0x50
    SerializeData serFaceSubEdges;  // 0x58
};
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












DominoWorld * gptPhysicsAPIImpl;



static inline Vector3D sConvertVec3(dm_vec4Arg v)
{
    return Vector3D(dmGetX(v), dmGetY(v), dmGetZ(v));
}




static dm_vec4 sConvert(const Vector3D & v)
{

    return dmVecSet(v.x, v.y, v.z);
}











static inline dm_vec4 sConvert(const Quaternion & q)
{
    return dmVecSet(q.wvImag.x, q.wvImag.y, q.wvImag.z, q.wdReal);
}

class DominoDispatcher : public dmJobDispatcher {  // size 0x10
public:
    DominoWorld::Async * m_pAsync;  // 0x08
    DominoDispatcher();
    // implicit virtual destructor (DWARF: ~DominoDispatcher is artificial)
    virtual void Submit(dmJobFcn * pFcn, void * pParam);  // vtable slot 2
    virtual void FinishJobs();  // vtable slot 3
};

class DominoListener : public dmContactListener {  // size 0x6010
public:
    DominoListener();
    // implicit virtual destructor (DWARF: ~DominoListener is artificial)
    void Reset();
    virtual void Report(const dmListenerManifold * pManifold);  // vtable slot 2
    void ProcessPoints();
    enum {
        MAX_POINT_COUNT = 256,
    };
private:
    ContactPoint m_points[256];  // 0x08
    int32 m_nPointCount;  // 0x6008
};

struct DeferDestroyConstraintData {  // size 0x10
    DominoWorld * pWorld;  // 0x00
    Constraint * pConstraint;  // 0x08
};

struct DeferSplashData {  // size 0x8
    DominoWorld * pWorld;  // 0x00
};

#include "../Attachments.h"
#include "../RActors/RActorAttachments.h"
#include "../../Common/Game.h"
void RActorApplyCollision(RActor* ptRActor, const ContactPoint* ptPoint);

#include "../StructureOctree.h"
#include "../Particle.h"
#include "../Definitions/SurfaceDefinition.h"
#include "../../Sigma/Math/Intersect.h"
#include "../../Common/SceneCommon.h"
#include "../../Common/TagMaps/Tags.h"
dmBodyId dmWorld_GetFirstBody(dmWorldId id);
dmBodyId dmWorld_GetNextBody(dmBodyId id);
BOOL CScenesIntersectLineSeg(StructureIntersectData&, CSceneDataArrayIterateMode);
SNO CSceneGetSurfaceFromIntersection(const StructureIntersectData&);
void BarycentricCoordsToTextureUV(SNO, int32, int32, int32, int32, int32, float, float, float&, float&);
SNO DominoWorld::s_currentActor = SNO_NONE;
SNO DominoWorld::s_currentScene = SNO_NONE;
// DWARF: X/Sigma/Math/Constraint.h
enum {
    CONSTRAINT_COLLIDE_CONNECTED = 1,
    CONSTRAINT_ANGLE_LIMIT = 2,
    CONSTRAINT_TWIST_LIMIT = 4,
    CONSTRAINT_CONE_LIMIT = 8,
    CONSTRAINT_KINEMATIC = 16,
};

enum ConstraintType {
    CONSTRAINT_TYPE_REVOLUTE = 0,
    CONSTRAINT_TYPE_RAGDOLL = 1,
    CONSTRAINT_TYPE_SPHERICAL = 2,
    CONSTRAINT_TYPE_WELD = 3,
    CONSTRAINT_TYPE_BREAKABLE = 4,
};

enum {
    CONSTRAINT_NAME_LENGTH = 64,
    CONSTRAINT_PARTICLE_SYSTEM_NAME_LENGTH = 64,
};

struct ConstraintParameters {  // size 0xec
    uint32 dwHash;  // 0x00
    uint32 dwHashPadding;  // 0x04
    uint32 dwFlags;  // 0x08
    ConstraintType eType;  // 0x0c
    int32 nBone1;  // 0x10
    int32 nBone2;  // 0x14
    PRTransform frame;  // 0x18
    Vector3D twistAxis;  // 0x34
    PRTransform localFrame1;  // 0x40
    PRTransform localFrame2;  // 0x5c
    float flRevoluteAngleLow;  // 0x78
    float flRevoluteAngleHigh;  // 0x7c
    float flRagdollConeAngle;  // 0x80
    float flRagdollTwistLow;  // 0x84
    float flRagdollTwistHigh;  // 0x88
    float flFriction;  // 0x8c
    float flCompliance;  // 0x90
    float flAngularFrequencyHz;  // 0x94
    float flAngularDampingRatio;  // 0x98
    float flLinearFrequencyHz;  // 0x9c
    float flLinearDampingRatio;  // 0xa0
    float flMotorStrength;  // 0xa4
    float flStrength;  // 0xa8
    char szParticleSystem[64];  // 0xac
};
static_assert(sizeof(ConstraintParameters) == 0xec, "ConstraintParameters");
































































































































































// DominoClient.cpp:374 is the original class line; its destructor is implicit
// (artificial in DWARF), D0 emitted with the vtable, D2 folds to dmJobDispatcher's.



inline DominoDispatcher::DominoDispatcher() : m_pAsync(NULL) {}  // inline: DWARF abstract DIE only, no concrete DIE (r06 test)


void DominoDispatcher::Submit(dmJobFcn * pFcn, void * pParam)
{

    m_pAsync->JobsSubmit(pFcn, pParam);
}




void DominoDispatcher::FinishJobs()
{
    m_pAsync->JobsFlush();
}








// Implicit destructor (artificial in DWARF); D2 folds to dmContactListener's.


inline DominoListener::DominoListener()  // inline: DWARF abstract DIE only, no concrete DIE (r06 test)
{
    Reset();
}

inline void DominoListener::Reset()  // inline: DWARF abstract DIE only, no concrete DIE (r06 test)
{
    m_nPointCount = 0;
}


void DominoListener::Report(const dmListenerManifold * pManifold)
{
    dmFixtureId fixtureIdA = pManifold->fixtureIdA;
    dmFixtureId fixtureIdB = pManifold->fixtureIdB;
    dmBodyId bodyIdA = dmFixture_GetOwnerBody(fixtureIdA);
    dmBodyId bodyIdB = dmFixture_GetOwnerBody(fixtureIdB);
    RigidBody * pBodyA = (RigidBody *)dmBody_GetUserData(bodyIdA);
    RigidBody * pBodyB = (RigidBody *)dmBody_GetUserData(bodyIdB);
    int32 count;
    ShapeUserData shapeDataA = *(ShapeUserData *)dmFixture_GetUserData(fixtureIdA);
    ShapeUserData shapeDataB = *(ShapeUserData *)dmFixture_GetUserData(fixtureIdB);

    count = pManifold->pointCount;
    for (int32 i = 0; i < count; ++i) {
        if (m_nPointCount >= MAX_POINT_COUNT) {
            return;
        }

        ContactPoint & point = m_points[m_nPointCount++];
        point.idRActor1 = pBodyA->m_idRActor;
        point.idRActor2 = pBodyB->m_idRActor;
        point.tShapeData1 = shapeDataA;
        point.tShapeData2 = shapeDataB;
        point.position = sConvertVec3(pManifold->points[i].position);
        point.normal = sConvertVec3(pManifold->normal);
        point.normalVelocity = pManifold->points[i].normalVelocity;
        point.impulse = pManifold->points[i].normalImpulse;
        point.isNew = pManifold->points[i].isNew;
    }
}















// NON_MATCHING: 868 vs 872 bytes. The original tests the sample flag as a bare i1 (tbz #0 at 514) and folds the index sign-extension into ldrsw at 516; ours tests a widened flag (cbz) and sign-extends after the compare. The DWARF has no local for the flag at all, so the original likely had no fCollectSample variable; tried BOOL instead of bool (worse, 864 B) and postfix ++ at 519 (column 33, no change).
void DominoListener::ProcessPoints()
{
    CollisionDebugData& tCollisionData = sgtAppGlobals.tCollisionDebugData;
    BOOL bTrack = tCollisionData.snoActorToTrack != -1 || tCollisionData.eMaterialToTrack != Shape::INVALID_MATERIAL;
    const float cflSecondsPerTick = 1.0f / 60.0f;

    for (int32 i = 0; i < m_nPointCount; ++i)
    {
        ContactPoint* ptPoint = &m_points[i];

        if (ptPoint->normalVelocity >= -0.16404199600219727f)
        {
            continue;
        }

        if (!ptPoint->isNew)
        {
            continue;
        }

        if (ptPoint->impulse <= EPSILON)
        {
            continue;
        }

        RActorID id1 = ptPoint->idRActor1;
        RActorID id2 = ptPoint->idRActor2;
        RActor* ptRActor1 = RActorTryToGet(id1);
        RActor* ptRActor2 = RActorTryToGet(id2);

        if (bTrack)
        {
            bool fCollectSample = true;

            if (tCollisionData.snoActorToTrack != -1)
            {
                if (!((ptRActor1 && ptRActor1->snoActor == tCollisionData.snoActorToTrack) ||
                      (ptRActor2 && ptRActor2->snoActor == tCollisionData.snoActorToTrack)))
                {
                    fCollectSample = false;
                }
            }
            if (tCollisionData.eMaterialToTrack != Shape::INVALID_MATERIAL)
            {
                if (ptPoint->tShapeData1.eMaterial != tCollisionData.eMaterialToTrack &&
                    ptPoint->tShapeData2.eMaterial != tCollisionData.eMaterialToTrack)
                {
                    fCollectSample = false;
                }
            }

            if (fCollectSample)
            {
                if (tCollisionData.nNumSamples < 1024)
                {
                    tCollisionData.arSamples[tCollisionData.nNumSamples] = ptPoint->impulse;
                    ++tCollisionData.nNumSamples;
                }
            }
        }










        if (ptRActor1)
        {
            if (!(ptRActor1->dwFlags & (1u << RACTOR_DISABLE_COLLISION_MESSAGES_BIT)))
            {

                Shape::Material eMaterial;
                if (ptPoint->tShapeData2.eMaterial != Shape::WATER_MATERIAL)
                {
                    eMaterial = ptPoint->tShapeData1.eMaterial;
                }
                else
                {
                    eMaterial = Shape::WATER_MATERIAL;
                }


                KeyedMessage tMsg(MSGKEY_RESERVED_COLLISION, ptPoint->impulse * cflSecondsPerTick, eMaterial);
                RActorSendMsgKey(id1, tMsg, MSGPASS_TO_ACTORCHAIN);

                RActorApplyCollision(ptRActor1, ptPoint);


                ptRActor2 = RActorTryToGet(id2);
            }
        }

        if (ptRActor2)
        {
            if (!(ptRActor2->dwFlags & (1u << RACTOR_DISABLE_COLLISION_MESSAGES_BIT)))
            {

                Shape::Material eMaterial;
                if (ptPoint->tShapeData1.eMaterial != Shape::WATER_MATERIAL)
                {
                    eMaterial = ptPoint->tShapeData2.eMaterial;
                }
                else
                {
                    eMaterial = Shape::WATER_MATERIAL;
                }


                KeyedMessage tMsg(MSGKEY_RESERVED_COLLISION, ptPoint->impulse * cflSecondsPerTick, eMaterial);
                RActorSendMsgKey(id2, tMsg, MSGPASS_TO_ACTORCHAIN);

                RActorApplyCollision(ptRActor2, ptPoint);
            }
        }
    }
}









































void Constraint::UpdateMotor(const PRTransform & transformRelative)
{
    if (m_jointId == dm_nullJointId) {
        return;
    }

    dmJointType type = dmJoint_GetType(m_jointId);
    if (type == dm_revoluteJoint) {
        dm_vec4 q = sConvert(transformRelative.q);
        dmRevoluteJoint_SetDesiredMotorAngle(m_jointId, q, dm_wakeIfChanged);
    } else if (type == dm_shoulderJoint) {
        dm_vec4 q = sConvert(transformRelative.q);
        dmShoulderJoint_SetDesiredMotorOrientation(m_jointId, q, dm_wakeIfChanged);
    }
}

















void Constraint::ActivateMotor(BOOL flag)
{
    if (m_jointId == dm_nullJointId) {
        return;
    }

    dmJointType type = dmJoint_GetType(m_jointId);
    dmMotorMode mode = flag ? dm_motorPositionMode : dm_motorVelocityMode;
    float maxTorque = flag ? m_motorTorque : m_frictionTorque;
    if (type == dm_revoluteJoint) {
        dmJoint_SetMotorMode(m_jointId, mode, dm_leaveAsleep);
        dmJoint_SetMaxMotorTorque(m_jointId, maxTorque, dm_leaveAsleep);
    } else if (type == dm_shoulderJoint) {
        dmJoint_SetMotorMode(m_jointId, mode, dm_leaveAsleep);
        dmJoint_SetMaxMotorTorque(m_jointId, maxTorque, dm_leaveAsleep);
    }
}














float Constraint::GetSoundData()
{
    dmBodyId bodyIdA = dmJoint_GetBodyA(m_jointId);
    dmBodyId bodyIdB = dmJoint_GetBodyB(m_jointId);
    dm_vec4 omegaA = dmBody_GetAngularVelocity(bodyIdA);
    dm_vec4 omegaB = dmBody_GetAngularVelocity(bodyIdB);



    dm_vec4 delta = dmSub(omegaB, omegaA);
    return dmLengthSqr3_f32(delta);
}



void DominoWorld::InitializeForApp()
{
    gptPhysicsAPIImpl = new DominoWorld;

}




void DominoWorld::DisposeForApp()
{

    delete gptPhysicsAPIImpl;
    gptPhysicsAPIImpl = NULL;
}



void DominoWorld::InitializeForGame()
{

    gptPhysicsAPIImpl->InitializeAPIForGame();
}




void DominoWorld::DisposeForGame()
{

    gptPhysicsAPIImpl->DisposeAPIForGame();
}


DominoWorld::DominoWorld()
{
    m_worldId = dm_nullWorldId;
    m_groundBodyId = dm_nullBodyId;
    m_mouseJointId = dm_nullJointId;



    m_ptFirstRigidBody = NULL;
    m_bodyPool.SetBlockCount_Tracked(MAX_RIGID_BODIES);
    m_constraintPool.SetBlockCount_Tracked(MAX_CONSTRAINTS);
    m_shapeUserDataPool.SetBlockCount_Tracked(MAX_SHAPES);

    m_pDispatcher = new DominoDispatcher;
    m_pListener = new DominoListener;

    m_nGroupIndex = 0;
    m_deltaTimeAccumulator = 0.0f;


    m_pAsync = new DominoWorld::Async(this, 2);
    m_pDispatcher->m_pAsync = m_pAsync;

    m_bActive = FALSE;
}




DominoWorld::~DominoWorld()
{
    delete m_pAsync;
    ::delete m_pDispatcher;
    ::delete m_pListener;
}









void DominoWorld::InitializeAPIForGame()
{
    GameMode eMode = CGameGet().tGameStartParams.eMode;
    if (eMode == GAMEMODE_PLAYING || eMode == GAMEMODE_HOLODECK || eMode == GAMEMODE_MENU) {
        dmWorldDef worldDef;
        worldDef.m_gravity = dmVecSet(0.0f, 0.0f, -32.2f, 0.0f);
        worldDef.m_bodyCapacity = MAX_RIGID_BODIES;
        worldDef.m_phantomCapacity = 0;
        worldDef.m_fixtureCapacity = 1536;
        worldDef.m_convexContactCapacity = 256;
        worldDef.m_meshContactCapacity = 256;
        worldDef.m_distanceJointCapacity = 0;
        worldDef.m_mouseJointCapacity = 1;
        worldDef.m_prismaticJointCapacity = 8;
        worldDef.m_revoluteJointCapacity = 192;
        worldDef.m_shoulderJointCapacity = 256;
        worldDef.m_sphericalJointCapacity = 128;
        worldDef.m_weldJointCapacity = 192;
        worldDef.m_stackCapacity = 2 * 1024 * 1024;
        m_worldId = dmWorld_Create(&worldDef);
        m_nGroupIndex = 0;
        dmWorld_SetJobDispatcher(m_worldId, m_pDispatcher);
        dmWorld_SetListener(m_worldId, m_pListener);
        m_bActive = TRUE;
    }
}










void DominoWorld::DisposeAPIForGame()
{
    m_bActive = FALSE;
    m_pAsync->Flush();
    m_mouseJointId = dm_nullJointId;
    m_groundBodyId = dm_nullBodyId;
    if (m_worldId != dm_nullWorldId) {
        dmWorld_Destroy(m_worldId);
    }
    m_worldId = dm_nullWorldId;
}










static const float RETRO_MODE_UPDATE_TIME = 0.05f;




void DominoWorld::UpdateAsyncWait()
{
    m_pAsync->Flush();
}





void DominoWorld::UpdateAsyncKick(float fTimeStep)
{
    m_pAsync->Kick(fTimeStep);
}





void DominoWorld::Update(float fTimeStep)
{
}



















































dmFixtureId DominoWorld::CreatePhysMeshFixture(
    dmBodyId bodyId,
    float flScale,
    const CollisionMesh * pMesh)
{

    m_pAsync->Sync(TRUE);








    ShapeUserData * pUserData = m_shapeUserDataPool.Alloc_Tracked();
    if (pUserData == NULL)
    {
        return dm_nullFixtureId;
    }



    dmFilter filter;
    if (pMesh->uFlags & 2)
    {
        // 5 compared with uMaterial (0x3178a0); read as Shape::WATER_MATERIAL (inference).
        if (pMesh->uMaterial == Shape::WATER_MATERIAL) filter.m_categoryBits = 0x40;
        else filter.m_categoryBits = 0x200;
    }
    else
    {








        filter.m_categoryBits = (pMesh->uFlags & 4) ? 0x81 : 1;
        if (pMesh->uFlags & 8)
            filter.m_categoryBits |= 0x100;
    }
    // Mask 0xFFDE and group 0 are folded into the filter word stored at 0x317908
    // with no line of their own; their position here is an inference.
    filter.m_includeMask = 0xFFDE;
    filter.m_groupIndex = 0;






    dmTreeMeshDef meshDef;




















    CollisionMesh * pDupedMesh = NULL;
    meshDef.m_pData = pMesh;


    pUserData->eType = ShapeUserData::MESH_SHAPE;
    pUserData->eMaterial = Shape::DEFAULT_MATERIAL;
    pUserData->wBoneIndex = 0;
    pUserData->flExplosionFactor = 0.0f;
    pUserData->pDupedMesh = pDupedMesh;

    meshDef.m_pUserData = pUserData;
    meshDef.m_density = 0.0f;
    meshDef.m_scale = dmVecSet(flScale);
    meshDef.m_friction = 0.9f;
    meshDef.m_restitution = 0.0f;
    meshDef.m_filter = filter;

    dmFixtureId fixtureId = dmBody_CreateFixture(bodyId, &meshDef);
    if (fixtureId == dm_nullFixtureId)
    {

        m_shapeUserDataPool.Free(pUserData);
    }


    return fixtureId;
}


RigidBody * DominoWorld::NewRigidBody()
{
    RigidBody * pRigidBody = m_bodyPool.Alloc_Tracked();
    if (pRigidBody == NULL)
    {
        return NULL;
    }

    pRigidBody->m_ptPrev = NULL;
    pRigidBody->m_ptNext = m_ptFirstRigidBody;
    if (m_ptFirstRigidBody)
    {
        m_ptFirstRigidBody->m_ptPrev = pRigidBody;
    }
    m_ptFirstRigidBody = pRigidBody;

    return pRigidBody;
}



void DominoWorld::DeleteRigidBody(RigidBody * ptBody)
{
    if (m_ptFirstRigidBody == ptBody) {
        m_ptFirstRigidBody = ptBody->m_ptNext;
    } else {
        ptBody->m_ptPrev->m_ptNext = ptBody->m_ptNext;
    }
    if (ptBody->m_ptNext) {
        ptBody->m_ptNext->m_ptPrev = ptBody->m_ptPrev;
    }
    m_bodyPool.Free(ptBody);
}












struct DeferCreateBodyFromPhysMeshData {  // size 0x138

    static const size_t MAX_NAME = 256;
    DominoWorld * pWorld;  // 0x00
    RigidBody * pRigidBody;  // 0x08
    SNO snoPhysMesh;  // 0x10
    PRTransform tTransform;  // 0x14
    float flScale;  // 0x30
    char szName[MAX_NAME];  // 0x34
};


void DominoWorld::DeferCreateBodyFromPhysMeshFunc(const void * pDataIn)
{
    const DeferCreateBodyFromPhysMeshData * pData = (const DeferCreateBodyFromPhysMeshData *)pDataIn;

    const PhysMeshDefinition * pdefPhysMesh = (const PhysMeshDefinition *)gptGroupDefPhysMesh->LockR(pData->snoPhysMesh, FALSE);


    dmBodyDef bodyDef;
    bodyDef.m_bodyType = dm_staticBody;
    bodyDef.m_position = sConvert(pData->tTransform.wp);
    bodyDef.m_quaternion = sConvert(pData->tTransform.q);
    bodyDef.m_pUserData = pData->pRigidBody;
    bodyDef.m_pName = pData->szName;

    dmBodyId bodyId = dmWorld_CreateBody(pData->pWorld->m_worldId, &bodyDef);
    if (bodyId == dm_nullBodyId)
    {
        gptGroupDefPhysMesh->UnlockR((void *&)pdefPhysMesh);
        return;
    }


    bool bFound = false;
    for (int32 i = 0; i < pdefPhysMesh->nCollisionMeshCount; ++i)
    {
        const CollisionMesh * pMesh = &((CollisionMesh *)pdefPhysMesh->ptCollisionMeshes)[i];
        if (pMesh->m_triangleCount != 0)
        {



            dmFixtureId fixtureId = pData->pWorld->CreatePhysMeshFixture(bodyId, pData->flScale, pMesh);
            if (fixtureId == dm_nullFixtureId) { continue; }

            bFound = true;
        }
    }



    if (!bFound)
    {
        dmWorld_DestroyBody(bodyId);
        gptGroupDefPhysMesh->UnlockR((void *&)pdefPhysMesh);
        return;
    }

    pData->pRigidBody->m_bodyId = bodyId;






    pData->pRigidBody->m_pLockedPhysMesh = pdefPhysMesh;


    pData->pRigidBody->m_area = 0.0f;
    pData->pRigidBody->m_volume = 0.0f;
}



RigidBody * DominoWorld::CreateBodyFromPhysMesh(
    SNO snoPhysMesh,
    const PRTransform & tTransform,
    float flScale,
    LPCSTR pszName)
{

    m_pAsync->Sync(TRUE);

    if (m_worldId == dm_nullWorldId)
    {
        return NULL;
    }

    const PhysMeshDefinition * pdefPhysMesh = (const PhysMeshDefinition *)gptGroupDefPhysMesh->LockR(snoPhysMesh, FALSE);
    if (pdefPhysMesh->nCollisionMeshCount == 0)
    {
        gptGroupDefPhysMesh->UnlockR((void *&)pdefPhysMesh);
        return NULL;
    }


    RigidBody * pRigidBody = NewRigidBody();
    if (pRigidBody == NULL)
    {
        gptGroupDefPhysMesh->UnlockR((void *&)pdefPhysMesh);
        return NULL;
    }

    pRigidBody->m_tName = MarkerStringHash(pszName);
    pRigidBody->m_wpLastRenderedCenterPosition = tTransform.wp;
    pRigidBody->m_idCScene = -1;
    pRigidBody->m_idRActor = RACTORID_INVALID;
    pRigidBody->m_slowExplosionFactor = 1.0f;
    pRigidBody->m_dwFlags = 0;


    if (m_pAsync->IsKicked())
    {
        gptGroupDefPhysMesh->UnlockR((void *&)pdefPhysMesh);

        DeferCreateBodyFromPhysMeshData * pData = (DeferCreateBodyFromPhysMeshData *)m_pAsync->Defer(DeferCreateBodyFromPhysMeshFunc, sizeof(DeferCreateBodyFromPhysMeshData));
        pData->pWorld = this;
        pData->pRigidBody = pRigidBody;
        pData->snoPhysMesh = snoPhysMesh;
        pData->tTransform = tTransform;
        pData->flScale = flScale;
        strncpy(pData->szName, pszName, DeferCreateBodyFromPhysMeshData::MAX_NAME - 1);
        pData->szName[DeferCreateBodyFromPhysMeshData::MAX_NAME - 1] = 0;
        return pRigidBody;
    }




    dmBodyDef bodyDef;
    bodyDef.m_bodyType = dm_staticBody;
    bodyDef.m_position = sConvert(tTransform.wp);
    bodyDef.m_quaternion = sConvert(tTransform.q);
    bodyDef.m_pName = pszName;
    bodyDef.m_pUserData = pRigidBody;

    dmBodyId bodyId = dmWorld_CreateBody(m_worldId, &bodyDef);
    if (bodyId == dm_nullBodyId)
    {
        DeleteRigidBody(pRigidBody);
        gptGroupDefPhysMesh->UnlockR((void *&)pdefPhysMesh);
        return NULL;
    }


    bool bFound = false;
    for (int32 i = 0; i < pdefPhysMesh->nCollisionMeshCount; ++i)
    {
        const CollisionMesh * pMesh = &((CollisionMesh *)pdefPhysMesh->ptCollisionMeshes)[i];
        if (pMesh->m_triangleCount != 0)
        {



            dmFixtureId fixtureId = CreatePhysMeshFixture(bodyId, flScale, pMesh);
            if (fixtureId == dm_nullFixtureId) { continue; }

            bFound = true;
        }
    }



    if (!bFound)
    {
        dmWorld_DestroyBody(bodyId);
        DeleteRigidBody(pRigidBody);
        gptGroupDefPhysMesh->UnlockR((void *&)pdefPhysMesh);
        return NULL;
    }

    pRigidBody->m_bodyId = bodyId;






    pRigidBody->m_pLockedPhysMesh = pdefPhysMesh;


    pRigidBody->m_area = 0.0f;
    pRigidBody->m_volume = 0.0f;

    return pRigidBody;
}



RigidBody * DominoWorld::CreateBodyFromAABB(
    const RigidBodyParams & tParams,
    const AABB & aabbBounds,
    RActorID idRActor)
{

    m_pAsync->Sync(TRUE);



    GameMode eGameMode = CGameGetGameCommonData().GameModeGet();
    if (eGameMode != GAMEMODE_PLAYING && eGameMode != GAMEMODE_HOLODECK && eGameMode != GAMEMODE_MENU)
    {
        return NULL;
    }





    RActor * pActor = RActorGet(idRActor);

    dm_vec4 center = sConvert(tParams.m_scale * aabbBounds.wp);
    dm_vec4 extents = sConvert(tParams.m_scale * aabbBounds.wvExt);
    float flDensity = Shape::GetDensity(tParams.m_materialOverride);


    RigidBody * pRigidBody = NewRigidBody();
    if (pRigidBody == NULL)
    {
        return NULL;
    }

    pRigidBody->m_tName = pActor->tName;
    pRigidBody->m_idRActor = idRActor;
    pRigidBody->m_idCScene = -1;
    pRigidBody->m_slowExplosionFactor = 1.0f;
    pRigidBody->m_wpLastRenderedCenterPosition = tParams.m_transform.wp;
    pRigidBody->m_pLockedPhysMesh = NULL;
    // 1291-1292: each bit is set or cleared (inferred: the original selects between flags and flags|2 at 0x3182c4).
    pRigidBody->m_dwFlags = 0;
    if (tParams.m_bCollideWithSameActor) pRigidBody->m_dwFlags |= (1 << RigidBody::COLLIDE_WITH_SAME_ACTOR_BIT); else pRigidBody->m_dwFlags &= ~(1 << RigidBody::COLLIDE_WITH_SAME_ACTOR_BIT);
    if (tParams.m_bAffectedByDisturbance) pRigidBody->m_dwFlags |= (1 << RigidBody::AFFECTED_BY_DISTURBANCE_BIT); else pRigidBody->m_dwFlags &= ~(1 << RigidBody::AFFECTED_BY_DISTURBANCE_BIT);

    dmBodyDef bodyDef;
    bodyDef.m_bodyType = dm_dynamicBody;
    bodyDef.m_position = sConvert(tParams.m_transform.wp);
    bodyDef.m_quaternion = sConvert(tParams.m_transform.q);
    bodyDef.m_pUserData = pRigidBody;
    bodyDef.m_linearDamping = tParams.m_linearDamping;
    bodyDef.m_angularDamping = tParams.m_angularDamping;

    pRigidBody->m_bodyId = dmWorld_CreateBody(m_worldId, &bodyDef);
    if (pRigidBody->m_bodyId == dm_nullBodyId)
    {
        DeleteRigidBody(pRigidBody);
        return NULL;
    }




    dmTransform boxTransform;
    boxTransform.q = dm_quat_identity;
    boxTransform.p = center;
    dmPolytopeData box;
    box.SetAsBox(boxTransform, extents);


    ShapeUserData * pUserData = m_shapeUserDataPool.Alloc_Tracked();
    if (pUserData == NULL)
    {
        dmWorld_DestroyBody(pRigidBody->m_bodyId);
        DeleteRigidBody(pRigidBody);
        return NULL;
    }

    pUserData->eType = ShapeUserData::AUTO_AABB_SHAPE;
    pUserData->eMaterial = tParams.m_materialOverride;
    pUserData->wBoneIndex = 0;
    pUserData->flExplosionFactor = 1.0f;
    pUserData->pDupedMesh = NULL;


    dmPolytopeDef polytopeDef;
    polytopeDef.m_density = flDensity;
    polytopeDef.m_pPolytope = &box;
    polytopeDef.m_pUserData = pUserData;
    polytopeDef.m_filter.m_categoryBits = tParams.m_collisionCategory;
    polytopeDef.m_filter.m_includeMask = tParams.m_collisionMask;
    polytopeDef.m_filter.m_groupIndex = 0;
    polytopeDef.m_friction = tParams.m_friction;
    polytopeDef.m_restitution = tParams.m_restitution;


    dmFixtureId fixtureId = dmBody_CreateFixture(pRigidBody->m_bodyId, &polytopeDef);
    if (fixtureId == dm_nullFixtureId)
    {
        dmWorld_DestroyBody(pRigidBody->m_bodyId);
        m_shapeUserDataPool.Free(pUserData);
        DeleteRigidBody(pRigidBody);
        return NULL;
    }


    pRigidBody->m_area = dmGetX(dmBody_ComputeSurfaceArea(pRigidBody->m_bodyId));
    pRigidBody->m_volume = dmGetX(dmBody_ComputeVolume(pRigidBody->m_bodyId));

    return pRigidBody;
}







static dmFixtureId sCreateShape(RigidBody * pRigidBody, ShapeUserData * pUserData, const CollisionShape * pShape, const RigidBodyParams * pParams, const dmTransform & transform)


{
    Shape::Material material = pShape->eMaterial;
    if (material == Shape::DEFAULT_MATERIAL)
    {
        material = pParams->m_materialOverride;
    }
    float flDensity = Shape::GetDensity(material);
    flDensity *= pShape->flDensityFactor;

    dmFixtureId fixtureId = dm_nullFixtureId;



    switch (pShape->eType)
    {
    case CollisionShape::SPHERE:
        {
            dmSphereDef shapeDef;
            shapeDef.m_density = flDensity;
            shapeDef.m_friction = pParams->m_friction;
            shapeDef.m_rollingResistance = pParams->m_rollingResistance;
            shapeDef.m_restitution = pParams->m_restitution;
            shapeDef.m_filter.m_categoryBits = pParams->m_collisionCategory;
            shapeDef.m_filter.m_includeMask = pParams->m_collisionMask;
            shapeDef.m_filter.m_groupIndex = pParams->m_groupIndex;
            shapeDef.m_pUserData = pUserData;

            shapeDef.m_localPosition = dmMul(transform, sConvert(pParams->m_scale * pShape->wp1));
            shapeDef.m_radius = dmMax(pParams->m_scale * pShape->flRadius, 0.0833f);

            fixtureId = dmBody_CreateFixture(pRigidBody->m_bodyId, &shapeDef);
        }
        break;

    case CollisionShape::CAPSULE:
        {
            dmCapsuleDef shapeDef;
            shapeDef.m_density = flDensity;
            shapeDef.m_friction = pParams->m_friction;
            shapeDef.m_rollingResistance = pParams->m_rollingResistance;
            shapeDef.m_restitution = pParams->m_restitution;
            shapeDef.m_filter.m_categoryBits = pParams->m_collisionCategory;
            shapeDef.m_filter.m_includeMask = pParams->m_collisionMask;
            shapeDef.m_filter.m_groupIndex = pParams->m_groupIndex;
            shapeDef.m_pUserData = pUserData;

            shapeDef.m_p1 = dmMul(transform, sConvert(pParams->m_scale * pShape->wp1));
            shapeDef.m_p2 = dmMul(transform, sConvert(pParams->m_scale * pShape->wp2));
            shapeDef.m_radius = dmMax(pParams->m_scale * pShape->flRadius, 0.0833f);

            fixtureId = dmBody_CreateFixture(pRigidBody->m_bodyId, &shapeDef);
        }
        break;

    case CollisionShape::POLYTOPE:
        {

            const dmPolytopeData * pPolytopeData = (PolytopeData *)pShape->pPolytopeData;
            float tol = 4.4143326e-06f;
            if (!dmIsFinite(pPolytopeData->m_volume) || pPolytopeData->m_volume < tol)
            {
                break;
            }

            dmPolytopeDef shapeDef;
            shapeDef.m_density = flDensity;
            shapeDef.m_friction = pParams->m_friction;

            shapeDef.m_rollingResistance = 0.0f;
            shapeDef.m_restitution = pParams->m_restitution;
            shapeDef.m_filter.m_categoryBits = pParams->m_collisionCategory;
            shapeDef.m_filter.m_includeMask = pParams->m_collisionMask;
            shapeDef.m_filter.m_groupIndex = pParams->m_groupIndex;
            shapeDef.m_pUserData = pUserData;

            shapeDef.m_pPolytope = pPolytopeData;
            shapeDef.m_transform = transform;
            shapeDef.m_scale = pParams->m_scale;

            fixtureId = dmBody_CreateFixture(pRigidBody->m_bodyId, &shapeDef);
        }
        break;

    default:
        break;
    }


    return fixtureId;
}



RigidBody * DominoWorld::CreateBodyFromSubObjects(const RigidBodyParams & tParams, RActorID idRActor)
{



    m_pAsync->Sync(TRUE);


    GameMode eGameMode = CGameGetGameCommonData().GameModeGet();
    if (eGameMode != GAMEMODE_PLAYING && eGameMode != GAMEMODE_HOLODECK && eGameMode != GAMEMODE_MENU)
    {
        return NULL;
    }


    RActor * pActor = RActorGet(idRActor);
    ScopedAppearanceDefinitionLockR pAppearance(pActor->snoAppearance);
    const Structure * pStructure = &pAppearance->tStructure;

    const GeoSetType eGeoSetType = GEOSET_HIGHPOLY;
    const GeoSet * pGeoSet = &pStructure->tGeoSets[eGeoSetType];
    RActorGeoSetData & tGeoSetData = pActor->tGeoSetData[eGeoSetType];


    RigidBody * pRigidBody = NewRigidBody();
    if (pRigidBody == NULL)
    {
        return NULL;
    }
    // Line 1493: every instruction carries column 5, so the original statement is probably a macro; written out.
    pRigidBody->m_tName = pActor->tName;
    pRigidBody->m_wpLastRenderedCenterPosition = tParams.m_transform.wp;
    pRigidBody->m_idCScene = -1;
    pRigidBody->m_idRActor = idRActor;
    pRigidBody->m_slowExplosionFactor = 1.0f;
    pRigidBody->m_pLockedPhysMesh = NULL;
    // 1501-1502: each bit is set or cleared (inferred, as in CreateBodyFromAABB: the original selects between flags and flags|2).
    pRigidBody->m_dwFlags = 0;
    if (tParams.m_bCollideWithSameActor) pRigidBody->m_dwFlags |= (1 << RigidBody::COLLIDE_WITH_SAME_ACTOR_BIT); else pRigidBody->m_dwFlags &= ~(1 << RigidBody::COLLIDE_WITH_SAME_ACTOR_BIT);
    if (tParams.m_bAffectedByDisturbance) pRigidBody->m_dwFlags |= (1 << RigidBody::AFFECTED_BY_DISTURBANCE_BIT); else pRigidBody->m_dwFlags &= ~(1 << RigidBody::AFFECTED_BY_DISTURBANCE_BIT);


    dmBodyDef bodyDef;
    bodyDef.m_bodyType = dm_staticBody;
    bodyDef.m_position = sConvert(tParams.m_transform.wp);
    bodyDef.m_quaternion = sConvert(tParams.m_transform.q);
    bodyDef.m_pUserData = pRigidBody;
    bodyDef.m_pName = MarkerNameGetString(pActor->tName);
    bodyDef.m_linearDamping = tParams.m_linearDamping;
    bodyDef.m_angularDamping = tParams.m_angularDamping;

    pRigidBody->m_bodyId = dmWorld_CreateBody(m_worldId, &bodyDef);
    if (pRigidBody->m_bodyId == dm_nullBodyId)
    {
        DeleteRigidBody(pRigidBody);
        return NULL;
    }

    dm_int32 nShapeCount = 0;
    for (int32 i = 0; i < pGeoSet->nSubObjectCount; i++)
    {
        SubObjectState * pSubObjectState = &tGeoSetData.ptSubObjectState[i];


        if (!(pSubObjectState->dwFlags & (1 << SUBOBJECTSTATE_RENDER_BIT)))
        {
            continue;
        }

        SubObject * pSubObject = &pGeoSet->ptSubObjects[i];

        for (int32 j = 0; j < pSubObject->nShapeCount; j++)
        {
            CollisionShape * pShape = pSubObject->ptShapes + j;

            ShapeUserData * pUserData = m_shapeUserDataPool.Alloc_Tracked();
            if (pUserData == NULL)
            {
                continue;
            }

            pUserData->flExplosionFactor = pShape->flExplosionFactor;
            pUserData->eType = ShapeUserData::SUBOBJECT_SHAPE;
            pUserData->eMaterial = pShape->eMaterial;
            pUserData->wBoneIndex = 0;
            pUserData->pDupedMesh = NULL;

            dmFixtureId fixtureId = sCreateShape(pRigidBody, pUserData, pShape, &tParams, dmTransformIdentity());
            if (fixtureId == dm_nullFixtureId)
            {
                m_shapeUserDataPool.Free(pUserData);
                continue;
            }

            nShapeCount++;
        }
    }

    if (nShapeCount == 0)
    {
        dmWorld_DestroyBody(pRigidBody->m_bodyId);
        DeleteRigidBody(pRigidBody);
        return NULL;
    }


    pRigidBody->m_area = dmGetX(dmBody_ComputeSurfaceArea(pRigidBody->m_bodyId));
    pRigidBody->m_volume = dmGetX(dmBody_ComputeVolume(pRigidBody->m_bodyId));

    return pRigidBody;
}



RigidBody * DominoWorld::CreateBodyFromBone(const RigidBodyParams & tParams, RActorID idRActor, int32 nBoneIndex)
{




    m_pAsync->Sync(TRUE);

    RActor * pActor = RActorGet(idRActor);
    ScopedAppearanceDefinitionLockR pAppearance(pActor->snoAppearance);
    const Structure * pStructure = &pAppearance->tStructure;



    const BoneStructure * pBone = &pStructure->ptBoneStructure[nBoneIndex];





    RigidBody * pRigidBody = NewRigidBody();
    if (pRigidBody == NULL)
    {
        return NULL;
    }

    // Line 1604: every instruction carries column 5, so the original statement is probably a macro; written out.
    pRigidBody->m_tName = MarkerStringHash(CRefString().Format("%s:0x%.8x", MarkerNameGetString(pActor->tName), pBone->dwHash));



    pRigidBody->m_wpLastRenderedCenterPosition = tParams.m_transform.wp;
    pRigidBody->m_idCScene = -1;
    pRigidBody->m_idRActor = idRActor;
    pRigidBody->m_slowExplosionFactor = 1.0f;
    pRigidBody->m_boundingRadius = 0.0f;
    pRigidBody->m_pLockedPhysMesh = NULL;
    // 1616-1617: each bit is set or cleared (inferred, as in CreateBodyFromAABB: the original selects between flags and flags|2 at 0x3190d4).
    pRigidBody->m_dwFlags = 0;
    if (tParams.m_bCollideWithSameActor) pRigidBody->m_dwFlags |= (1 << RigidBody::COLLIDE_WITH_SAME_ACTOR_BIT); else pRigidBody->m_dwFlags &= ~(1 << RigidBody::COLLIDE_WITH_SAME_ACTOR_BIT);
    if (tParams.m_bAffectedByDisturbance) pRigidBody->m_dwFlags |= (1 << RigidBody::AFFECTED_BY_DISTURBANCE_BIT); else pRigidBody->m_dwFlags &= ~(1 << RigidBody::AFFECTED_BY_DISTURBANCE_BIT);


    dmBodyDef bodyDef;
    bodyDef.m_bodyType = tParams.m_bStatic ? dm_staticBody : dm_dynamicBody;
    bodyDef.m_position = sConvert(tParams.m_transform.wp);
    bodyDef.m_quaternion = sConvert(tParams.m_transform.q);
    bodyDef.m_pUserData = pRigidBody;
    bodyDef.m_pName = MarkerNameGetString(pRigidBody->m_tName);
    bodyDef.m_linearDamping = tParams.m_linearDamping;
    bodyDef.m_angularDamping = tParams.m_angularDamping;
    bodyDef.m_enableSleeping = tParams.m_bDisableSleep == FALSE;

    pRigidBody->m_bodyId = dmWorld_CreateBody(m_worldId, &bodyDef);
    if (pRigidBody->m_bodyId == dm_nullBodyId)
    {
        DeleteRigidBody(pRigidBody);
        return NULL;
    }

    int32 nShapeCount = 0;
    for (int32 i = 0; i < pBone->nShapeCount; i++)
    {
        CollisionShape * pShape = pBone->ptShapes + i;

        if (pShape->nDegrade != tParams.m_nDegrade)
        {
            continue;
        }

        ShapeUserData * pUserData = m_shapeUserDataPool.Alloc_Tracked();
        if (pUserData == NULL)
        {
            continue;
        }

        pUserData->flExplosionFactor = pShape->flExplosionFactor;
        pUserData->eType = ShapeUserData::BONE_SHAPE;
        pUserData->eMaterial = pShape->eMaterial;
        pUserData->wBoneIndex = nBoneIndex;
        pUserData->pDupedMesh = NULL;
        // dmTransformIdentity (dmMath.inl:809, inlined elsewhere): its inlined body here left no instruction of its own (the quat load is shared with line 208), so no DIE; column 81 is this argument.
        dmFixtureId fixtureId = sCreateShape(pRigidBody, pUserData, pShape, &tParams, dmTransformIdentity());
        if (fixtureId == dm_nullFixtureId)
        {
            m_shapeUserDataPool.Free(pUserData);
            continue;
        }

        nShapeCount++;
    }

    if (nShapeCount == 0)
    {
        dmWorld_DestroyBody(pRigidBody->m_bodyId);
        DeleteRigidBody(pRigidBody);
        return NULL;
    }


    pRigidBody->ComputeBoundingRadius();


    pRigidBody->m_area = dmGetX(dmBody_ComputeSurfaceArea(pRigidBody->m_bodyId));
    pRigidBody->m_volume = dmGetX(dmBody_ComputeVolume(pRigidBody->m_bodyId));

    return pRigidBody;
}




RigidBody * DominoWorld::CreateBodyFromBones(const RigidBodyParams & tParams, RActorID idRActor)
{



    m_pAsync->Sync(TRUE);

    if (tParams.m_nBoneCount == 0)
    {
        return NULL;
    }

    RActor * pActor = RActorGet(idRActor);
    ScopedAppearanceDefinitionLockR pAppearance(pActor->snoAppearance);
    const Structure * pStructure = &pAppearance->tStructure;
    const BoneStructure * pRootBone = &pStructure->ptBoneStructure[tParams.m_nBones[0]];


    RigidBody * pRigidBody = NewRigidBody();
    if (pRigidBody == NULL)
    {
        return NULL;
    }

    // Line 1714: every instruction carries column 5, so the original statement is probably a macro; written out.
    pRigidBody->m_tName = MarkerStringHash(CRefString().Format("%s:0x%.8x", MarkerNameGetString(pActor->tName), pRootBone->dwHash));



    pRigidBody->m_wpLastRenderedCenterPosition = tParams.m_transform.wp;
    pRigidBody->m_idCScene = -1;
    pRigidBody->m_idRActor = idRActor;
    pRigidBody->m_slowExplosionFactor = 1.0f;
    pRigidBody->m_boundingRadius = 0.0f;
    pRigidBody->m_pLockedPhysMesh = NULL;
    // 1726-1727: each bit is set or cleared (inferred, as in CreateBodyFromAABB: the original selects between flags and flags|2).
    pRigidBody->m_dwFlags = 0;
    if (tParams.m_bCollideWithSameActor) pRigidBody->m_dwFlags |= (1 << RigidBody::COLLIDE_WITH_SAME_ACTOR_BIT); else pRigidBody->m_dwFlags &= ~(1 << RigidBody::COLLIDE_WITH_SAME_ACTOR_BIT);
    if (tParams.m_bAffectedByDisturbance) pRigidBody->m_dwFlags |= (1 << RigidBody::AFFECTED_BY_DISTURBANCE_BIT); else pRigidBody->m_dwFlags &= ~(1 << RigidBody::AFFECTED_BY_DISTURBANCE_BIT);


    dmBodyDef bodyDef;
    bodyDef.m_bodyType = tParams.m_bStatic ? dm_staticBody : dm_dynamicBody;
    bodyDef.m_position = sConvert(tParams.m_transform.wp);
    bodyDef.m_quaternion = sConvert(tParams.m_transform.q);
    bodyDef.m_pUserData = pRigidBody;
    bodyDef.m_pName = MarkerNameGetString(pActor->tName);
    bodyDef.m_linearDamping = tParams.m_linearDamping;
    bodyDef.m_angularDamping = tParams.m_angularDamping;

    pRigidBody->m_bodyId = dmWorld_CreateBody(m_worldId, &bodyDef);
    if (pRigidBody->m_bodyId == dm_nullBodyId)
    {
        DeleteRigidBody(pRigidBody);
        return NULL;
    }


    PRSTransform invRootTransform = pStructure->ptBoneStructure[tParams.m_nBones[0]].transformInv;

    int32 nShapeCount = 0;

    for (int32 i = 0; i < tParams.m_nBoneCount; i++)
    {
        int32 nBoneIndex = tParams.m_nBones[i];


        const BoneStructure * pBone = &pStructure->ptBoneStructure[nBoneIndex];
        PRSTransform transform = invRootTransform * pBone->transform;
        transform.wp *= tParams.m_scale;


        dmTransform xf;
        xf.q = sConvert(transform.q);
        xf.p = sConvert(transform.wp);



        for (int32 j = 0; j < pBone->nShapeCount; j++)
        {
            CollisionShape * pShape = pBone->ptShapes + j;

            ShapeUserData * pUserData = m_shapeUserDataPool.Alloc_Tracked();
            if (pUserData == NULL)
            {
                continue;
            }

            pUserData->eType = ShapeUserData::BONE_SHAPE;
            pUserData->eMaterial = pShape->eMaterial;
            pUserData->wBoneIndex = nBoneIndex;
            pUserData->flExplosionFactor = pShape->flExplosionFactor;
            pUserData->pDupedMesh = NULL;

            dmFixtureId fixtureId = sCreateShape(pRigidBody, pUserData, pShape, &tParams, xf);
            if (fixtureId == dm_nullFixtureId)
            {
                m_shapeUserDataPool.Free(pUserData);
                continue;
            }

            nShapeCount++;
        }
    }

    if (nShapeCount == 0)
    {
        dmWorld_DestroyBody(pRigidBody->m_bodyId);
        DeleteRigidBody(pRigidBody);
        return NULL;
    }


    pRigidBody->ComputeBoundingRadius();


    pRigidBody->m_area = dmGetX(dmBody_ComputeSurfaceArea(pRigidBody->m_bodyId));
    pRigidBody->m_volume = dmGetX(dmBody_ComputeVolume(pRigidBody->m_bodyId));

    return pRigidBody;
}





struct DeferDestroyBodyData {  // size 0x10
    DominoWorld * pWorld;  // 0x00
    RigidBody * pRigidBody;  // 0x08
};


void DominoWorld::DeferDestroyBodyFunc(const void * pDataIn)
{
    const DeferDestroyBodyData * pData = (const DeferDestroyBodyData *)pDataIn;

    if (pData->pRigidBody->m_bodyId != dm_nullBodyId)
    {
        dmFixtureId fixtureId = dmBody_GetFirstFixture(pData->pRigidBody->m_bodyId);
        while (fixtureId != dm_nullFixtureId)
        {
            ShapeUserData * pUserData = (ShapeUserData *)dmFixture_GetUserData(fixtureId);
            if (pUserData)
            {


                CollisionMesh * pMesh = (CollisionMesh *)pUserData->pDupedMesh;
                if (pMesh)
                {

                    SigmaMemoryFree(pMesh->m_pRoot, NULL); pMesh->m_pRoot = NULL;
                    SigmaMemoryFree(pMesh->m_aTriangles, NULL); pMesh->m_aTriangles = NULL;
                    SigmaMemoryFree(pMesh->m_aVertices, NULL); pMesh->m_aVertices = NULL;
                    SigmaMemoryFree(pMesh, NULL);
                    pUserData->pDupedMesh = NULL;
                }

                pData->pWorld->m_shapeUserDataPool.Free(pUserData);
            }

            fixtureId = dmBody_GetNextFixture(fixtureId);
        }

        dmWorld_DestroyBody(pData->pRigidBody->m_bodyId);
        pData->pRigidBody->m_bodyId = dm_nullBodyId;
    }



    if (pData->pRigidBody->m_pLockedPhysMesh)
    {
        gptGroupDefPhysMesh->UnlockR((void *&)pData->pRigidBody->m_pLockedPhysMesh);
        pData->pRigidBody->m_pLockedPhysMesh = NULL;
    }

    pData->pWorld->DeleteRigidBody(pData->pRigidBody);
}


void DominoWorld::DestroyBody(RigidBody * pRigidBody)
{
    if (pRigidBody == NULL)
    {
        return;
    }


    if (m_pAsync->IsKicked())
    {
        DeferDestroyBodyData * pCommand = (DeferDestroyBodyData *)m_pAsync->Defer(DeferDestroyBodyFunc, sizeof(DeferDestroyBodyData));
        pCommand->pWorld = this;
        pCommand->pRigidBody = pRigidBody;
        return;
    }


    if (pRigidBody->m_bodyId != dm_nullBodyId)
    {
        dmFixtureId fixtureId = dmBody_GetFirstFixture(pRigidBody->m_bodyId);
        while (fixtureId != dm_nullFixtureId)
        {
            ShapeUserData * pUserData = (ShapeUserData *)dmFixture_GetUserData(fixtureId);
            if (pUserData)
            {


                CollisionMesh * pMesh = (CollisionMesh *)pUserData->pDupedMesh;
                if (pMesh)
                {

                    SigmaMemoryFree(pMesh->m_pRoot, NULL); pMesh->m_pRoot = NULL;
                    SigmaMemoryFree(pMesh->m_aTriangles, NULL); pMesh->m_aTriangles = NULL;
                    SigmaMemoryFree(pMesh->m_aVertices, NULL); pMesh->m_aVertices = NULL;
                    SigmaMemoryFree(pMesh, NULL);
                    pUserData->pDupedMesh = NULL;
                }

                m_shapeUserDataPool.Free(pUserData);
            }

            fixtureId = dmBody_GetNextFixture(fixtureId);
        }

        dmWorld_DestroyBody(pRigidBody->m_bodyId);
        pRigidBody->m_bodyId = dm_nullBodyId;
    }



    if (pRigidBody->m_pLockedPhysMesh)
    {
        gptGroupDefPhysMesh->UnlockR((void *&)pRigidBody->m_pLockedPhysMesh);
        pRigidBody->m_pLockedPhysMesh = NULL;
    }

    DeleteRigidBody(pRigidBody);
}


Constraint * DominoWorld::CreateConstraint(
    RigidBody * pBodyA,
    RigidBody * pBodyB,
    const ConstraintParameters * pParams,
    RActorID idRActor)
{

    m_pAsync->Sync(TRUE);

    if (!pBodyA || !pBodyB)
    {
        return NULL;
    }

    Constraint* pConstraint = m_constraintPool.Alloc_Tracked();
    pConstraint->m_jointId = dm_nullJointId;

    bool bCollideConnected = (pParams->dwFlags & CONSTRAINT_COLLIDE_CONNECTED) != 0;

    const RActor* pActor = RActorGet(idRActor);
    PRSTransform actorTransform = pActor->transformClient;

    float flLengthScale = actorTransform.flScale;
    float flMassScale = flLengthScale * flLengthScale * flLengthScale;
    float flForceScale = flMassScale;
    float flTorqueScale = flLengthScale * flForceScale;

    dmTransform frameA, frameB;
    frameA.q = sConvert(pParams->localFrame1.q);
    frameA.p = sConvert(pParams->localFrame1.wp * flLengthScale);
    frameB.q = sConvert(pParams->localFrame2.q);
    frameB.p = sConvert(pParams->localFrame2.wp * flLengthScale);

    pConstraint->m_motorTorque = flTorqueScale * pParams->flMotorStrength;
    pConstraint->m_frictionTorque = flTorqueScale * pParams->flFriction;

    switch (pParams->eType)
    {
    case CONSTRAINT_TYPE_SPHERICAL:
    {
        dmSphericalJointDef def;
        def.m_bodyIdA = pBodyA->m_bodyId;
        def.m_bodyIdB = pBodyB->m_bodyId;
        def.m_collideConnected = bCollideConnected;
        def.m_localAnchorA = frameA.p;
        def.m_localAnchorB = frameB.p;
        def.m_pUserData = NULL;
        def.m_frictionTorque = pConstraint->m_frictionTorque;

        pConstraint->m_jointId = dmWorld_CreateJoint(m_worldId, &def);
        break;
    }

    case CONSTRAINT_TYPE_REVOLUTE:
    {


        dm_vec4 qYZX = {0.5f, 0.5f, 0.5f, 0.5f};
        dmRevoluteJointDef def;
        def.m_bodyIdA = pBodyA->m_bodyId;
        def.m_bodyIdB = pBodyB->m_bodyId;
        def.m_localFrameA.q = dmMulQQ(frameA.q, qYZX);
        def.m_localFrameA.p = frameA.p;
        def.m_localFrameB.q = dmMulQQ(frameB.q, qYZX);
        def.m_localFrameB.p = frameB.p;
        def.m_collideConnected = bCollideConnected;
        def.m_pUserData = NULL;


        def.m_maxMotorTorque = pConstraint->m_motorTorque;
        def.m_motorMode = dm_motorPositionMode;

        if (pParams->dwFlags & CONSTRAINT_ANGLE_LIMIT)
        {
            def.m_enableLimit = true;
            def.m_lowerAngle = pParams->flRevoluteAngleLow;
            def.m_upperAngle = pParams->flRevoluteAngleHigh;
        }

        pConstraint->m_jointId = dmWorld_CreateJoint(m_worldId, &def);
        break;
    }

    case CONSTRAINT_TYPE_RAGDOLL:
    {


        dm_vec4 qZXY = {-0.5f, -0.5f, -0.5f, 0.5f};
        dmShoulderJointDef def;
        def.m_bodyIdA = pBodyA->m_bodyId;
        def.m_bodyIdB = pBodyB->m_bodyId;
        def.m_localFrameA.q = dmMulQQ(frameA.q, qZXY);
        def.m_localFrameA.p = frameA.p;
        def.m_localFrameB.q = dmMulQQ(frameB.q, qZXY);
        def.m_localFrameB.p = frameB.p;
        def.m_lowerTwistAngle = pParams->flRagdollTwistLow;
        def.m_upperTwistAngle = pParams->flRagdollTwistHigh;
        def.m_coneAngle = pParams->flRagdollConeAngle;
        def.m_maxMotorTorque = pConstraint->m_motorTorque;
        def.m_motorMode = dm_motorPositionMode;
        def.m_enableTwistLimit = true;
        def.m_collideConnected = bCollideConnected;
        pConstraint->m_jointId = dmWorld_CreateJoint(m_worldId, &def);
        break;
    }


    case CONSTRAINT_TYPE_WELD:
    {
        dmWeldJointDef def;
        def.m_bodyIdA = pBodyA->m_bodyId;
        def.m_bodyIdB = pBodyB->m_bodyId;
        def.m_collideConnected = bCollideConnected;
        def.m_localFrameA = frameA;
        def.m_localFrameB = frameB;
        def.m_pUserData = NULL;

        if (pParams->flAngularFrequencyHz > 0.0f)
        {
            def.m_angularFrequencyHz = pParams->flAngularFrequencyHz;
            def.m_angularDampingRatio = pParams->flAngularDampingRatio;
        }
        else if (pParams->flCompliance > 0.0f)
        {
            float alpha = dmMax(0.0f, 1.0f - pParams->flCompliance);  // operand order from the original fmaxnm (0x31a3cc); dmMax(x, 0.0f) gives fmax
            float hz = alpha * 15.0f + 1.0f;
            def.m_angularFrequencyHz = hz;
            def.m_angularDampingRatio = 0.9f;
        }

        if (pParams->flLinearFrequencyHz > 0.0f)
        {
            def.m_linearFrequencyHz = pParams->flLinearFrequencyHz;
            def.m_linearDampingRatio = pParams->flLinearDampingRatio;
        }

        pConstraint->m_jointId = dmWorld_CreateJoint(m_worldId, &def);
        break;
    }

    default:
        break;
    }

    if (pConstraint->m_jointId == dm_nullJointId)
    {
        m_constraintPool.Free(pConstraint);
        return NULL;
    }

    return pConstraint;
}













void DominoWorld::DeferDestroyConstraintFunc(const void * pDataIn)
{
    const DeferDestroyConstraintData * pData = (const DeferDestroyConstraintData *)pDataIn;
    if (!dmJoint_IsValid(pData->pConstraint->m_jointId)) {
        pData->pConstraint->m_jointId = dm_nullJointId;
    }
    if (pData->pConstraint->m_jointId != dm_nullJointId) {
        dmWorld_DestroyJoint(pData->pConstraint->m_jointId);
        pData->pConstraint->m_jointId = dm_nullJointId;
    }
    pData->pWorld->m_constraintPool.Free(pData->pConstraint);
}






void DominoWorld::DestroyConstraint(Constraint * pConstraint)
{
    if (pConstraint == NULL) {
        return;
    }




    if (m_pAsync->IsKicked()) {

        DeferDestroyConstraintData * pCommand = (DeferDestroyConstraintData *)m_pAsync->Defer(DeferDestroyConstraintFunc, sizeof(DeferDestroyConstraintData));
        pCommand->pWorld = this;
        pCommand->pConstraint = pConstraint;
        return;
    }

    if (!dmJoint_IsValid(pConstraint->m_jointId)) {
        pConstraint->m_jointId = dm_nullJointId;
    }
    if (pConstraint->m_jointId != dm_nullJointId) {
        dmWorld_DestroyJoint(pConstraint->m_jointId);
        pConstraint->m_jointId = dm_nullJointId;
    }
    m_constraintPool.Free(pConstraint);
}




















































void DominoWorld::DeferSplashFunc(const void * pDataIn)
{
    const DeferSplashData * pData = (const DeferSplashData *)pDataIn;
    if (pData->pWorld->m_bActive) {
        pData->pWorld->SplashInternal();
    }
}





void DominoWorld::SplashInternal()
{
    if (m_worldId == dm_nullWorldId)
        return;
    float fVelocityTolerance = -20.0f;
    for (dmBodyId id = dmWorld_GetFirstBody(m_worldId); id != dm_nullBodyId; id = dmWorld_GetNextBody(id))
    {
        if (dmBody_IsSleeping(id) || dmBody_GetType(id) == dm_staticBody)
            continue;
        Vector3D wp = sConvertVec3(dmBody_GetCenter(id));
        RigidBody* pRigidBody = static_cast<RigidBody*>(dmBody_GetUserData(id));
        Vector3D wpOld = pRigidBody->m_wpLastRenderedCenterPosition;
        float fVelocity = (wp.z - wpOld.z) * 60.0f;
        if (fVelocity < fVelocityTolerance)
        {
            StructureIntersectData tStructIntersect(CGameGetCurrentSWorldID());
            tStructIntersect.wline.Set(wpOld, wp);
            tStructIntersect.dwIntersectFlags = 0x10;
            BOOL fIntersect = CScenesIntersectLineSeg(tStructIntersect, SkipHolodeckScenes);
            if (fIntersect)
            {
                float flUTriangle, flVTriangle;
                BOOL bIntersect = Intersect::Ray3DAndTriangle3D(tStructIntersect.wline.wr, tStructIntersect.wtriIntersect, flUTriangle, flVTriangle);
                Vector3D wpIntersect;
                if (bIntersect)
                {
                    float flU, flV;
                    BarycentricCoordsToTextureUV(tStructIntersect.snoAppearance, 0, tStructIntersect.nSubobject, tStructIntersect.nSegment, tStructIntersect.nIndexListIndex, 0, flUTriangle, flVTriangle, flU, flV);
                    PRTransform transform(cgqIdentity, tStructIntersect.wpIntersect);
                    const SurfaceDefinition* pdefSurface = static_cast<const SurfaceDefinition *>(gptGroupDefSurface->LockRFast(CSceneGetSurfaceFromIntersection(tStructIntersect)));
                    SNO snoRippleParticleSystem = TagMapGetSNO(pdefSurface->hTagMap, 0x50001);
                    if (snoRippleParticleSystem != -1)
                    {
                        ParticleSystemAddParams tParams(snoRippleParticleSystem, transform);
                        ParticleSystemAdd(tParams);
                    }
                    SNO snoSplashParticleSystem = TagMapGetSNO(pdefSurface->hTagMap, 0x50000);
                    if (snoSplashParticleSystem != -1)
                    {
                        ParticleSystemAddParams tParams(snoSplashParticleSystem, transform);
                        ParticleSystemAdd(tParams);
                    }
                }
            }
        }
        pRigidBody->m_wpLastRenderedCenterPosition = wp;
    }
}

























void DominoWorld::Splash()
{
    DeferSplashData * pData = (DeferSplashData *)m_pAsync->Defer(DeferSplashFunc, sizeof(DeferSplashData));
    pData->pWorld = this;
}







void DominoWorld::GetProfile(dmProfile * pProfile) const
{
    SigmaMemoryFill(pProfile, 0, sizeof(dmProfile));
}


















































BOOL DominoWorld::RayCast(
    RayCastOutput * ptOutput,
    const RayCastInput * ptInput)
{

    m_pAsync->Sync(TRUE);


    if (m_worldId == dm_nullWorldId) return FALSE;




    dmWorldRayCastInput input;
    input.p1 = sConvert(ptInput->wpFrom);
    input.p2 = sConvert(ptInput->wpTo);
    input.filter.m_includeMask = ptInput->uIncludeMask;
    input.filter.m_excludeMask = ptInput->uExcludeMask;
    input.pUserData = NULL;

    dmWorldRayCastOutput output;

    bool hit = dmWorld_RayCastClosest(m_worldId, &output, &input);

    if (hit)
    {
        dmBodyId bodyId = dmFixture_GetOwnerBody(output.fixtureId);


        ptOutput->pRigidBody = (RigidBody *)dmBody_GetUserData(bodyId);
        ptOutput->wpPoint = sConvertVec3(output.p);
        ptOutput->wvNormal = sConvertVec3(output.normal);
        return TRUE;
    }
    else
    {


        ptOutput->pRigidBody = NULL;
        ptOutput->wpPoint = cgwpNull;
        ptOutput->wvNormal = cgwvNull;
        return FALSE;
    }
}



int32 DominoWorld::QueryAABB(
    RigidBody ** pptBodies,
    int32 nMaxBodies,
    const AABB & ab)
{

    m_pAsync->Sync(TRUE);


    if (m_worldId == dm_nullWorldId) return 0;




    dmAABB aabb;
    aabb.m_lowerBound = sConvert(ab.wp - ab.wvExt);
    aabb.m_upperBound = sConvert(ab.wp + ab.wvExt);

    dmQueryFilter filter;
    dmBuffer<dmFixtureId, 32> buffer;
    dmWorld_QueryAABB(m_worldId, buffer, aabb.m_lowerBound, aabb.m_upperBound, filter);

    int32 count = buffer.GetCount();
    count = dmMin(nMaxBodies, count);
    for (int32 i = 0; i < count; ++i)
    {
        dmFixtureId fixtureId = buffer[i];
        dmBodyId bodyId = dmFixture_GetOwnerBody(fixtureId);

        pptBodies[i] = (RigidBody *)dmBody_GetUserData(bodyId);
    }

    if (count < 2)
    {
        return count;
    }


    blz::sort(pptBodies, pptBodies + count);
    int32 i1 = 0;
    for (int32 i2 = 1; i2 < count; ++i2)
    {
        if (pptBodies[i2] != pptBodies[i1])
        {
            ++i1;
            pptBodies[i1] = pptBodies[i2];
        }
    }

    return i1 + 1;
}






























































































int32 DominoWorld::GetNextGroupIndex()
{
    if (m_nGroupIndex == 0x7fffffff) {
        m_nGroupIndex = 0;
    }
    return ++m_nGroupIndex;
}






void DominoWorld::Lock()
{
    dmWorld_Lock(m_worldId);
}



void DominoWorld::Unlock()
{
    dmWorld_Unlock(m_worldId);
}




static const int32 MAX_ASYNC_JOBS = 512;

void * DominoWorld::AsyncDefer(DominoWorld::Async::DeferFunc pFunc, size_t uSize)
{
    return m_pAsync->Defer(pFunc, uSize);
}

DominoWorld::Async::Async(DominoWorld * pWorld, const int32 nWorkersCount)
    : m_pWorld(pWorld),
      m_bActive(TRUE),
      m_bKicked(FALSE),
      m_fTimeStep(0.0f),
      m_atThreads(NULL),
      m_nThreadsCount(nWorkersCount + 1),
      m_tPendingEvent(),
      m_tCompleteEvent(),
      m_ptDeferredCommands(NULL),
      m_atJobs(NULL),
      m_nJobsIndex(0),
      m_nJobsCount(0),
      m_tJobsPending(),
      m_tJobsComplete()
{
    m_atThreads = (Blizzard::Thread::Thread *)SigmaMemoryNew(sizeof(Blizzard::Thread::Thread) * m_nThreadsCount, 4, NULL, FALSE);
    m_atJobs = (Job *)SigmaMemoryNew(sizeof(Job) * MAX_ASYNC_JOBS, 4, NULL, FALSE);
    m_ptDeferredCommands = new XCommandBuffer("DominoDeferredCommands", 128 * 1024);
    m_tPendingEvent.Create();
    m_tCompleteEvent.Create();
    m_tJobsPending.Create();
    m_tJobsComplete.Create();
    {
        const bool bResult = Blizzard::Thread::Create(ThreadProc, this, &m_atThreads[0], "DominoAsyncPhysics", 0, 256 * 1024);
        Blizzard::Thread::SetPriority(m_atThreads[0], Blizzard::Thread::PRIORITY_BELOW_NORMAL);
        ConfigureDominoAsyncPhysicsThread(m_atThreads[0]);
        Blizzard::Thread::Resume(m_atThreads[0]);
    }
    for (int32 i = 1; i < m_nThreadsCount; ++i) {
        Blizzard::Thread::Create(WorkerProc, this, &m_atThreads[i], "DominoAsyncPhysicsWorker", 0, 256 * 1024);
        Blizzard::Thread::SetPriority(m_atThreads[i], Blizzard::Thread::PRIORITY_BELOW_NORMAL);
        ConfigureDominoAsyncPhysicsThread(m_atThreads[i]);
        Blizzard::Thread::Resume(m_atThreads[i]);
    }
}







DominoWorld::Async::~Async()
{
    Flush();
    m_bActive = FALSE;
    for (int32 i = 0; i < m_nThreadsCount; ++i) {
        m_tPendingEvent.Set();
        m_tJobsPending.Set();
        Blizzard::Thread::WaitForever(m_atThreads[i]);
        Blizzard::Thread::Close(m_atThreads[i]);
    }
    m_tJobsPending.Destroy();
    m_tJobsComplete.Destroy();
    m_tPendingEvent.Destroy();
    m_tCompleteEvent.Destroy();
    delete m_ptDeferredCommands;
}








void DominoWorld::Async::Kick(float fTimeStep)
{
    GameMode eGameMode = CGameGetGameCommonData().GameModeGet();
    if (eGameMode != GAMEMODE_PLAYING && eGameMode != GAMEMODE_HOLODECK && eGameMode != GAMEMODE_MENU) {
        return;
    }
    if (m_pWorld->m_pListener == NULL) {
        return;
    }
    if (m_pWorld->m_worldId == dm_nullWorldId) {
        return;
    }

    dmWorld_EnableContinuous(m_pWorld->m_worldId, false);
    if (CWorldPhysicsAreRetro()) {
        m_pWorld->m_deltaTimeAccumulator += fTimeStep;
        if (m_pWorld->m_deltaTimeAccumulator < RETRO_MODE_UPDATE_TIME) {
            return;
        }
        fTimeStep = Math::Max(fTimeStep, RETRO_MODE_UPDATE_TIME);
        m_pWorld->m_deltaTimeAccumulator -= fTimeStep;
    } else if (m_pWorld->m_deltaTimeAccumulator > 0.0f) {
        fTimeStep += m_pWorld->m_deltaTimeAccumulator;
        m_pWorld->m_deltaTimeAccumulator = 0.0f;
    }

    m_fTimeStep = fTimeStep;
    m_bKicked = TRUE;
    m_tPendingEvent.Set();
}





















void DominoWorld::Async::Sync(const BOOL bWarn)
{
    if (m_bKicked) {
        if (!m_tCompleteEvent.Wait(0)) {
            for (int32 i = 0; i < m_nThreadsCount; ++i) {
                Blizzard::Thread::SetPriority(m_atThreads[i], Blizzard::Thread::PRIORITY_ABOVE_NORMAL);
            }
            m_tCompleteEvent.WaitForever();
            for (int32 i = 0; i < m_nThreadsCount; ++i) {
                Blizzard::Thread::SetPriority(m_atThreads[i], Blizzard::Thread::PRIORITY_BELOW_NORMAL);
            }
        }
        m_bKicked = FALSE;
    }
}


















void DominoWorld::Async::Flush()
{

    Sync(FALSE);
    m_pWorld->m_pListener->ProcessPoints();
    m_ptDeferredCommands->Flush();
}










void * DominoWorld::Async::Defer(const DominoWorld::Async::DeferFunc pFunc, const size_t uSize)
{
    return m_ptDeferredCommands->Reserve(pFunc, uSize);
}


void DominoWorld::Async::JobsSubmit(
    DominoWorld::Async::JobFunc pFcn,
    void * pParam)
{

    const bool bHasWorkers = m_nThreadsCount > 1;
    const bool bHasSpace = m_nJobsCount < MAX_ASYNC_JOBS;
    if (!bHasWorkers || !bHasSpace)
    {




        pFcn(pParam);
        return;
    }

    Job & job = m_atJobs[m_nJobsCount++];
    job.pFunc = pFcn;
    job.pParam = pParam;

    m_tJobsPending.Set();
}


void DominoWorld::Async::JobsFlush()
{
    if (m_nThreadsCount == 1) {
        return;
    }
    while (m_tJobsPending.Wait(0)) {
        int32 nJobsIndex = Blizzard::Lock::Atomic::Increment(&m_nJobsIndex) - 1;
        const Job & job = m_atJobs[nJobsIndex];
        job.pFunc(job.pParam);
        m_tJobsComplete.Set();
    }
    for (int32 i = 0; i < m_nJobsCount; ++i) {
        m_tJobsComplete.WaitForever();
    }
    m_nJobsIndex = 0;
    m_nJobsCount = 0;
}













uint32 DominoWorld::Async::ThreadProc(void * pData)
{
    DominoWorld::Async * pAsync = (DominoWorld::Async *)pData;
    ThreadGlobals::InitializeForThread();
    pAsync->m_tPendingEvent.WaitForever();
    while (pAsync->m_bActive) {
        pAsync->m_pWorld->UpdateInternal(pAsync->m_fTimeStep);
        pAsync->m_tCompleteEvent.Set();
        pAsync->m_tPendingEvent.WaitForever();
    }
    ThreadGlobals::DisposeForThread();
    return 0;
}











uint32 DominoWorld::Async::WorkerProc(void * pData)
{
    DominoWorld::Async * pAsync = (DominoWorld::Async *)pData;
    ThreadGlobals::InitializeForThread();
    pAsync->m_tJobsPending.WaitForever();
    while (pAsync->m_bActive) {
        int32 nJobsIndex = Blizzard::Lock::Atomic::Increment(&pAsync->m_nJobsIndex) - 1;
        const DominoWorld::Async::Job & job = pAsync->m_atJobs[nJobsIndex];
        job.pFunc(job.pParam);
        pAsync->m_tJobsComplete.Set();
        pAsync->m_tJobsPending.WaitForever();
    }
    ThreadGlobals::DisposeForThread();
    return 0;
}
















void DominoWorld::UpdateInternal(float fTimeStep)
{



    m_pListener->Reset();
    dmWorld_Step(m_worldId, fTimeStep, 4, 1, true);
    for (RigidBody * ptBody = m_ptFirstRigidBody; ptBody; ptBody = ptBody->m_ptNext) {
        ptBody->m_slowExplosionFactor = 1.0f;
    }
}
