// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/Physics/DominoBody.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Contrib/Contrib/Domino/Include/dmId.h"
#include "../../Common/CommonTypes.h"
#include "../../Common/WorldCommonCollision.h"
#include "../../Sigma/Math/Shape.h"
#include "../../Sigma/Math/Transform.h"
#include "../../Sigma/Math/Vector.h"
#include "../../Sigma/SigmaTypes.h"

struct ExplosionImpulse;
struct PhysMeshDefinition;
struct Plane;

typedef int32 CSceneID;

class RigidBodyParams {  // size 0x170
public:
    // Original inline constructor at line 34; defaults recovered from its callers.
    RigidBodyParams() : m_transform(cgtPRTransformIdentity), m_linearVel(cgwvNull), m_angularVel(cgwvNull) {
        m_scale = 1.0f;
        m_materialOverride = Shape::DEFAULT_MATERIAL;
        m_friction = 0.3f;
        m_rollingResistance = 0.1f;
        m_restitution = 0.0f;
        m_linearDamping = 0.0f;
        m_angularDamping = 0.01f;
        m_collisionCategory = 1 << COLLISION_CATEGORY_RIGID_BODY;
        m_collisionMask = COLLISION_CATEGORY_ALL;
        m_groupIndex = 0;
        m_nBoneCount = m_nDegrade = m_bStatic = m_bCollideWithSameActor = m_bDisableSleep = m_bAffectedByDisturbance = 0;
    }
    PRTransform m_transform;  // 0x00
    Vector3D m_linearVel;  // 0x1c
    Vector3D m_angularVel;  // 0x28
    float m_scale;  // 0x34
    Shape::Material m_materialOverride;  // 0x38
    float m_friction;  // 0x3c
    float m_rollingResistance;  // 0x40
    float m_restitution;  // 0x44
    float m_linearDamping;  // 0x48
    float m_angularDamping;  // 0x4c
    uint16 m_collisionCategory;  // 0x50
    uint16 m_collisionMask;  // 0x52
    int32 m_groupIndex;  // 0x54
    int32 m_nBones[64];  // 0x58
    int32 m_nBoneCount;  // 0x158
    int32 m_nDegrade;  // 0x15c
    BOOL m_bStatic;  // 0x160
    BOOL m_bCollideWithSameActor;  // 0x164
    BOOL m_bDisableSleep;  // 0x168
    BOOL m_bAffectedByDisturbance;  // 0x16c
};
static_assert(sizeof(RigidBodyParams) == 0x170, "RigidBodyParams");

class RigidBody {  // size 0x50
public:
    // The DWARF dump prints the type enum before the first access specifier, but
    // the 1.0 mirror of this header has it public and free functions (e.g.
    // ApplyBuoyancy) compare against DYNAMIC.
    enum Type {
        NONE = -1,
        DYNAMIC = 0,
        KINEMATIC = 1,
        STATIC = 2,
    };
    // DominoBody.h: the m_dwFlags bit numbers, listed next to Type in the DWARF.
    enum {
        COLLIDE_WITH_SAME_ACTOR_BIT = 0,
        AFFECTED_BY_DISTURBANCE_BIT = 1,
    };
    RigidBody();
    ~RigidBody();
    void SetTransform(const PRTransform & transform);
    void SetTransformNoSync(const PRTransform & transform);
    void GetTransform(PRTransform & transform) const;
    void SetPosition(const Vector3D & position);
    Vector3D GetPosition() const;
    Vector3D GetCenterOfMassPosition() const;
    float GetMass() const;
    Vector3D WorldToLocalPoint(const Vector3D &) const;
    Vector3D LocalToWorldPoint(const Vector3D &) const;
    Vector3D GetCenterOfMassLocal() const;
    BOOL IsSleeping() const;
    void SetSleeping(BOOL bFlag);
    void SetCenterOfMassVelocity(const Vector3D & wvVelocity);
    void SetAngularVelocity(const Vector3D & wvAngularVelocity);
    Vector3D GetCenterOfMassVelocity() const;
    Vector3D GetAngularVelocity() const;
    Vector3D GetPointVelocity(const Vector3D & wp) const;
    void ApplyForce(const Vector3D & wvForce);
    void ApplyForce(const Vector3D & wvForce, const Vector3D & wp);
    void ApplyTorque(const Vector3D & wvTorque);
    void ApplyImpulse(const Vector3D & wvImpulse);
    void ApplyImpulse(const Vector3D &, const Vector3D &);
    void ApplyImpulseToClosestPoint(const Vector3D & wvImpulse, const Vector3D & wpTarget);
    void ApplyWind(const Vector3D & wvWind);
    void StabilizeInertias(float flInertiaFactor, float flMassRatio);
    void ApplyExplosion(const ExplosionImpulse & tExplosion, float flFactor, float flMaxDeltaVelocity);
    RigidBody::Type GetType() const;
    void SetType(RigidBody::Type eType);
    float ComputeSubmergedVolume(Vector3D * pCentroid, const Plane & plane) const;
    void ComputeBoundingRadius();
    dmBodyId m_bodyId;  // 0x00
    uint32 m_dwFlags;  // 0x08
    uint32 m_tName;  // 0x0c
    CSceneID m_idCScene;  // 0x10
    RActorID m_idRActor;  // 0x14
    Vector3D m_wpLastRenderedCenterPosition;  // 0x18
    float m_slowExplosionFactor;  // 0x24
    float m_boundingRadius;  // 0x28
    float m_area;  // 0x2c
    float m_volume;  // 0x30
    RigidBody * m_ptPrev;  // 0x38
    RigidBody * m_ptNext;  // 0x40
    const PhysMeshDefinition * m_pLockedPhysMesh;  // 0x48
};
static_assert(sizeof(RigidBody) == 0x50, "RigidBody");


// Functions defined in this file (bodies are not in the DWARF):
//   line    34  RigidBodyParams::RigidBodyParams()  [inlined into 7 functions]
//   line    99  RigidBody::RigidBody()  [inlined into 5 functions]

// No inlined or out-of-line copy anywhere in the binary: DominoWorld frees its
// bodies through XChainedFixedBlockAllocator::Free, which runs this destructor,
// with no code between the unlink and the block release, so it is empty.
inline RigidBody::~RigidBody() {}

// DominoBody.h:99, written from its five inlined copies in DominoClient.cpp
// (NewRigidBody inside CreateBodyFromPhysMesh 0x317ddc, CreateBodyFromAABB
// 0x3181f0, ...). Surviving stores and their lines: m_bodyId with the zeroed
// m_dwFlags/m_tName pair as one stp (102), m_ptNext = NULL (104), m_idCScene and
// m_idRActor as one 8-byte -1 (108), m_wpLastRenderedCenterPosition = cgwvNull
// (110, inlined Vector3D::operator=), the four floats as two 8-byte zero stores
// (113), m_pLockedPhysMesh = NULL (118). m_ptPrev = NULL is inferred: NewRigidBody
// overwrites it at once (DominoClient.cpp:1023), and statement order between the
// listed lines is an inference.
inline RigidBody::RigidBody()
{
    m_bodyId = dm_nullBodyId;
    m_dwFlags = 0;
    m_tName = 0;
    m_ptPrev = NULL;
    m_ptNext = NULL;
    m_idCScene = -1;
    m_idRActor = RACTORID_INVALID;
    m_wpLastRenderedCenterPosition = cgwvNull;
    m_slowExplosionFactor = 0.0f;
    m_boundingRadius = 0.0f;
    m_area = 0.0f;
    m_volume = 0.0f;
    m_pLockedPhysMesh = NULL;
}
