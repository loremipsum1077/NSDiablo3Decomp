// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/Physics/DominoClient.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Lock.h"
#include "../../../BlizzardCore/Include/Blizzard/Thread.h"
#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Contrib/Contrib/Domino/Include/dmId.h"
#include "../../Common/CommonTypes.h"
#include "../../Common/SNO/SNOFiles.h"
#include "../../Sigma/DataStructures/XChainedFixedBlockAllocator.h"
#include "../../Sigma/Math/Shape.h"
#include "../../Sigma/Math/Vector.h"
#include "../../Sigma/SigmaTypes.h"
#include "../../Sigma/Unicode.h"
#include <stddef.h>
#include <stdint.h>

struct AABB;
struct CollisionMesh;
struct ConstraintParameters;
class DominoDispatcher;
class DominoListener;
struct PRTransform;
class RWindow;
class RigidBody;
class RigidBodyParams;
class XCommandBuffer;
struct dmCounters;
struct dmProfile;

enum {
    MAX_RIGID_BODIES = 1024,
    MAX_SHAPES = 2048,
    MAX_CONSTRAINTS = 512,
};

struct ShapeUserData {  // size 0x18
    enum Type {
        AUTO_AABB_SHAPE = 0,
        SUBOBJECT_SHAPE = 1,
        BONE_SHAPE = 2,
        TRIANGLE_SHAPE = 3,
        MESH_SHAPE = 4,
    };
    float flExplosionFactor;  // 0x00
    ShapeUserData::Type eType;  // 0x04
    Shape::Material eMaterial;  // 0x08
    uint16 wBoneIndex;  // 0x0c
    void * pDupedMesh;  // 0x10
};
static_assert(sizeof(ShapeUserData) == 0x18, "ShapeUserData");

struct ContactPoint {  // size 0x60
    RActorID idRActor1;  // 0x00
    RActorID idRActor2;  // 0x04
    ShapeUserData tShapeData1;  // 0x08
    ShapeUserData tShapeData2;  // 0x20
    Vector3D position;  // 0x38
    Vector3D normal;  // 0x44
    float normalVelocity;  // 0x50
    float impulse;  // 0x54
    BOOL isNew;  // 0x58
};
static_assert(sizeof(ContactPoint) == 0x60, "ContactPoint");

struct RayCastInput {  // size 0x1c
    Vector3D wpFrom;  // 0x00
    Vector3D wpTo;  // 0x0c
    uint16 uIncludeMask;  // 0x18
    uint16 uExcludeMask;  // 0x1a
};
static_assert(sizeof(RayCastInput) == 0x1c, "RayCastInput");

struct RayCastOutput {  // size 0x20
    RigidBody * pRigidBody;  // 0x00
    Vector3D wpPoint;  // 0x08
    Vector3D wvNormal;  // 0x14
};
static_assert(sizeof(RayCastOutput) == 0x20, "RayCastOutput");

class Constraint {  // size 0x14
public:
    void UpdateMotor(const PRTransform & transformRelative);
    void ActivateMotor(BOOL flag);
    float GetSoundData();
    uint32 m_dwHash;  // 0x00
    dmJointId m_jointId;  // 0x04
    float m_frictionTorque;  // 0x0c
    float m_motorTorque;  // 0x10
};
static_assert(sizeof(Constraint) == 0x14, "Constraint");

class DominoWorld {  // size 0xd0
public:
    static void InitializeForApp();
    static void DisposeForApp();
    static void InitializeForGame();
    static void DisposeForGame();
    DominoWorld();
    ~DominoWorld();
    void InitializeAPIForGame();
    void DisposeAPIForGame();
    void UpdateAsyncWait();
    void UpdateAsyncKick(float fTimeStep);
    void Update(float fTimeStep);
    void DrawBounds(RWindow &, BOOL);
    void Splash();
    RigidBody * CreateBodyFromPhysMesh(SNO snoPhysMesh, const PRTransform & tTransform, float flScale, LPCSTR pszName);
    RigidBody * CreateBodyFromAABB(const RigidBodyParams & tParams, const AABB & aabbBounds, RActorID idRActor);
    RigidBody * CreateBodyFromSubObjects(const RigidBodyParams & tParams, RActorID idRActor);
    RigidBody * CreateBodyFromBone(const RigidBodyParams & tParams, RActorID idRActor, int32 nBoneIndex);
    RigidBody * CreateBodyFromBones(const RigidBodyParams & tParams, RActorID idRActor);
    Constraint * CreateConstraint(RigidBody * pBodyA, RigidBody * pBodyB, const ConstraintParameters * pParams, RActorID idRActor);
    void DestroyBody(RigidBody * pRigidBody);
    void DestroyConstraint(Constraint * pConstraint);
    BOOL RayCast(RayCastOutput * ptOutput, const RayCastInput * ptInput);
    int32 QueryAABB(RigidBody ** pptBodies, int32 nMaxBodies, const AABB & ab);
    void EnableMousePhysics(const Vector3D &, const Vector3D &, RigidBody *);
    void UpdateMousePhysics(const Vector3D &);
    void DisableMousePhysics();
    RigidBody * GetMousePhysicsBody() const;
    void GetProfile(dmProfile * pProfile) const;
    void ResetProfile();
    void GetCounters(dmCounters *) const;
    int32 GetNextGroupIndex();
    void Lock();
    void Unlock();
    static SNO s_currentActor;
    static SNO s_currentScene;
    class Async {  // size 0xa8
    public:
        typedef void (*DeferFunc)(const void *);
        typedef void (*JobFunc)(void *);
    public:
        Async(DominoWorld *, const int32);
        ~Async();
        void Kick(float fTimeStep);
        void Sync(const BOOL bWarn);
        void Flush();
        void * Defer(const DominoWorld::Async::DeferFunc pFunc, const size_t uSize);
        const BOOL IsKicked();
        void JobsSubmit(DominoWorld::Async::JobFunc pFcn, void * pParam);
        void JobsFlush();
    private:
        const DominoWorld::Async & operator=(const DominoWorld::Async &);
        Async(const DominoWorld::Async &);
        struct Job {  // size 0x10
            DominoWorld::Async::JobFunc pFunc;  // 0x00
            void * pParam;  // 0x08
        };
        static uint32 ThreadProc(void * pData);
        static uint32 WorkerProc(void * pData);
        DominoWorld * m_pWorld;  // 0x00
        BOOL m_bActive;  // 0x08
        BOOL m_bKicked;  // 0x0c
        float m_fTimeStep;  // 0x10
        Blizzard::Thread::Thread * m_atThreads;  // 0x18
        int32 m_nThreadsCount;  // 0x20
        Blizzard::Lock::Event m_tPendingEvent;  // 0x24
        Blizzard::Lock::Event m_tCompleteEvent;  // 0x30
        XCommandBuffer * m_ptDeferredCommands;  // 0x40
        DominoWorld::Async::Job * m_atJobs;  // 0x48
        volatile int32 m_nJobsIndex;  // 0x50
        int32 m_nJobsCount;  // 0x54
        Blizzard::Lock::Semaphore m_tJobsPending;  // 0x58
        Blizzard::Lock::Semaphore m_tJobsComplete;  // 0x80
    };
    void * AsyncDefer(DominoWorld::Async::DeferFunc pFunc, size_t uSize);
private:
    dmFixtureId CreatePhysMeshFixture(dmBodyId bodyId, float flScale, const CollisionMesh * pMesh);
    dmWorldId m_worldId;  // 0x00
    BOOL m_bActive;  // 0x04
    DominoDispatcher * m_pDispatcher;  // 0x08
    DominoListener * m_pListener;  // 0x10
    dmBodyId m_groundBodyId;  // 0x18
    dmJointId m_mouseJointId;  // 0x20
    int32 m_nGroupIndex;  // 0x28
    float m_deltaTimeAccumulator;  // 0x2c
    RigidBody * NewRigidBody();
    void DeleteRigidBody(RigidBody * ptBody);
    RigidBody * m_ptFirstRigidBody;  // 0x30
    XChainedFixedBlockAllocator<RigidBody> m_bodyPool;  // 0x38
    XChainedFixedBlockAllocator<Constraint> m_constraintPool;  // 0x68
    XChainedFixedBlockAllocator<ShapeUserData> m_shapeUserDataPool;  // 0x98
    void UpdateInternal(float fTimeStep);
    void SplashInternal();
    static void DeferDestroyBodyFunc(const void * pDataIn);
    static void DeferDestroyConstraintFunc(const void * pDataIn);
    static void DeferCreateBodyFromPhysMeshFunc(const void * pDataIn);
    static void DeferSplashFunc(const void * pDataIn);
    DominoWorld::Async * m_pAsync;  // 0xc8
};
static_assert(sizeof(DominoWorld) == 0xd0, "DominoWorld");


// Functions defined in this file (bodies are not in the DWARF):
//   line   270  const BOOL DominoWorld::Async::IsKicked()  [inlined into 3 functions]

// line 270. Body written from the copies inlined into DominoClient.cpp.
inline const BOOL DominoWorld::Async::IsKicked() {
    return m_bKicked;
}

// Defined in DominoClient.cpp:81.
extern DominoWorld * gptPhysicsAPIImpl;
