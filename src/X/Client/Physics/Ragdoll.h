// Adopted from DWARF; hand-maintained.
// Original file: X/Client/Physics/Ragdoll.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../Animation.h"
#include "../../Common/CommonTypes.h"
#include "../../Sigma/Math/Plane.h"
#include "../../Sigma/Math/Transform.h"
#include "../../Sigma/Math/Vector.h"
#include "../../Sigma/SigmaTypes.h"

struct ActorDefinition;
class Constraint;
struct ContactPoint;
struct ExplosionImpulse;
struct RActor;
class RigidBody;

enum {
    MAX_RAGDOLL_BONES = 64,
    MAX_RAGDOLL_LOOPS = 8,
    LEAF_BONE = -2,
    INTERMEDIATE_BONE = -1,
};

enum {
    INVALID_RAGDOLL_TYPE = 0,
    FULL_RAGDOLL_TYPE = 1,
    ATTACHED_RAGDOLL_TYPE = 2,
    PARTIAL_RAGDOLL_TYPE = 3,
    BREAKABLE_RAGDOLL_TYPE = 4,
};

typedef int32 RagdollType;

struct RagdollBone {  // size 0xb0
    Vector3D wpInitialCOMPosition;  // 0x00
    Vector3D wvInitialLinearVelocity;  // 0x0c
    Vector3D wvInitialAngularVelocity;  // 0x18
    PRTransform partialTransform;  // 0x24
    PRTransform deltaTransform;  // 0x40
    Plane waterPlane;  // 0x5c
    Vector3D wvWaterVelocity;  // 0x6c
    RigidBody * ptBody;  // 0x78
    Constraint * ptConstraint;  // 0x80
    float flInitialVelocityFactor;  // 0x88
    float flWindPhase;  // 0x8c
    int32 nRootIndex;  // 0x90
    int32 nParentIndex;  // 0x94
    int32 nAnimIndex;  // 0x98
    int32 nSiblingIndex;  // 0x9c
    int32 nChildIndex;  // 0xa0
    int32 nKinematicRoot;  // 0xa4
    BOOL bHaveWater;  // 0xa8
    BOOL bTeleport;  // 0xac
};
static_assert(sizeof(RagdollBone) == 0xb0, "RagdollBone");

class Ragdoll : public AnimationController {  // size 0x3098
public:
    Ragdoll(RActorID);
    virtual ~Ragdoll();  // vtable slot 0
    RagdollType GetType() const;
    BOOL NeedsInitialVelocity() const;
    virtual BOOL ShouldAnimate() const { return TRUE; }  // vtable slot 3
    virtual BOOL ShouldUpdateBounds() const { return TRUE; }  // vtable slot 4
    void InitializeVelocities(const PRSTransform * ptRelativeTransforms1, const PRSTransform * ptRelativeTransforms2, float flInvDeltaT, float flMomentumFactor);
    virtual void ApplyExplosion(const ExplosionImpulse & tExplosion, float flExplosionFactor);  // vtable slot 5
    virtual void ApplyCollision(const ContactPoint *) {}  // vtable slot 6
    void DisableConstraint(uint32 dwHash);
    Constraint * GetConstraint(uint32 dwHash);
    void Update(BOOL bIsAnimating, float flDeltaTime, const ActorDefinition * pdefActor);
    virtual void ComputeBone(RActor * ptRActor, PRSTransform & boneTransform, const PRSTransform & parentTransform, const PRSTransform & relativeTransform, const PRSTransform & defaultRelativeTransform, int32 nBoneIndex, float flDeltaTime);  // vtable slot 2
    void PostCompute();
    void DrawInitialVelocity() const;
    void SetAnimationOverride(BOOL bEnable);
    void SnapToBindPose();
protected:
    Ragdoll(const Ragdoll &);
    void operator=(const Ragdoll &);
    void InstallBoneControllers();
    void RemovePenetrations();
    void UpdateBuoyancy();
    RagdollBone m_ptBones[64];  // 0x08
    Constraint * m_ptLoops[8];  // 0x2c08
    int32 m_anBoneMap[255];  // 0x2c48
    PRSTransform m_oldActorTransform;  // 0x3044
    float m_flMass;  // 0x3064
    float m_flTracking;  // 0x3068
    int32 m_nBoneCount;  // 0x306c
    int32 m_nLoopCount;  // 0x3070
    float m_flLifeTime;  // 0x3074
    int32 m_nGroupIndex;  // 0x3078
    RActorID m_idRActor;  // 0x307c
    RagdollType m_nType;  // 0x3080
    BOOL m_bNeedsInitialVelocity;  // 0x3084
    BOOL m_bMotorsActive;  // 0x3088
    BOOL m_bAnimationOverride;  // 0x308c
    BOOL m_bSnapToBindPose;  // 0x3090
};
static_assert(sizeof(Ragdoll) == 0x3098, "Ragdoll");

// line 190. Body written from RActorGetRagdollType (RActors.cpp:11362).
inline RagdollType Ragdoll::GetType() const { return m_nType; }

// line 151. Body from sHandleSnapWeaponAttachment (RActorAttachments.cpp:607).
inline void Ragdoll::SnapToBindPose() { m_bSnapToBindPose = TRUE; }
// line 195 (return at 198). Body from the m_bNeedsInitialVelocity (0x3084) loads in RActorUpdateBones (RActorAnimation.cpp:1091/1247).
inline BOOL Ragdoll::NeedsInitialVelocity() const { return m_bNeedsInitialVelocity; }
// Functions defined in this file (bodies are not in the DWARF):
//   line    84  BOOL Ragdoll::ShouldAnimate() const  [0x71003dd890]
//   line    87  BOOL Ragdoll::ShouldUpdateBounds() const  [0x71003dd8a0]
//   line   103  void Ragdoll::ApplyCollision(const ContactPoint *)  [0x71003dd8b0]
//   line   147  void Ragdoll::SetAnimationOverride(BOOL bEnable)  [inlined into 1 functions]
//   line   151  void Ragdoll::SnapToBindPose()  [inlined into 1 functions]
//   line   190  RagdollType Ragdoll::GetType() const  [inlined into 9 functions]
//   line   195  BOOL Ragdoll::NeedsInitialVelocity() const  [inlined into 1 functions]
