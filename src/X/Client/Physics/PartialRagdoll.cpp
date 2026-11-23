// Original file: X/Client/Physics/PartialRagdoll.cpp
#include "PartialRagdoll.h"
#include "DominoBody.h"
#include "DominoClient.h"
#include "../RActors/RActors.h"
#include "../RActors/RActorAnimation.h"
#include "../Definitions/AppearanceDefinition.h"
#include "../Definitions/PhysicsDefinition.h"
#include "../../Common/Definitions/ActorDefinition.h"
#include "../../Sigma/Math/Constraint.h"

// DWARF AppearanceDefinition.h:148; shared header currently only forward declares this type.
struct PolytopeData;
struct CollisionShape {
    enum Type { NONE = -1, SPHERE = 0, CAPSULE = 1, POLYTOPE = 2 };
    uint32 dwFlags;
    Type eType;
    Shape::Material eMaterial;
    int32 nDegrade;
    float flDensityFactor;
    float flExplosionFactor;
    float flVelocityFactor;
    SerializeData serPolytope;
    XPtr64<PolytopeData> pPolytopeData;
    Vector3D wp1;
    Vector3D wp2;
    float flRadius;
    BOOL Create(const Shape *);
    void Destroy();
    void Rotate(const Quaternion &);
};
static_assert(sizeof(CollisionShape) == 0x50, "CollisionShape");

// NON_MATCHING: 1692 both; 195/423 insns differ in block layout of the dynamic-subtree test (original shares one "dynamic" block and keeps the flag inverted). The ptConstraint = NULL at 192 is one statement after the !bKinematicBone block (both paths carry row 192 in the original).
#line 23
PartialRagdoll::PartialRagdoll(
    RActorID id)
    : Ragdoll(id)
{
    m_nType = PARTIAL_RAGDOLL_TYPE;
    m_nBoneCount = m_nLoopCount = 0;
    m_bNeedsInitialVelocity = FALSE;
#line 34
    RActor * ptRActor = RActorGet(m_idRActor);

    ScopedAppearanceDefinitionLockR pdefAppearance(ptRActor->snoAppearance);

    const Structure * ptStructure = &pdefAppearance->tStructure;

    PRSTransform actorTransform = ptRActor->transformClient;

    ScopedActorDefinitionLockR pdefActor(ptRActor->snoActor);

    ScopedPhysicsDefinitionLockR pdefPhysics(ptRActor->snoPhysics);
#line 47
    RigidBodyParams params;

    params.m_materialOverride = pdefActor->eMaterial;
    params.m_angularDamping = pdefPhysics->flAngularDamping;
    params.m_linearDamping = pdefPhysics->flLinearDamping;
    params.m_scale = actorTransform.flScale;
    params.m_friction = pdefPhysics->flFriction;
    params.m_rollingResistance = pdefPhysics->flRollingResistance;
    params.m_restitution = pdefPhysics->flRestitution;

    if (pdefPhysics->dwFlags & (1 << PHYSICSDEF_ANIMATED_PROP))
    {
        params.m_collisionCategory = 1 << COLLISION_CATEGORY_FULL_RAGDOLL;
        params.m_collisionMask = (pdefPhysics->wCollisionMask & ~((1 << COLLISION_CATEGORY_WATER) | (1 << COLLISION_CATEGORY_SMOKE))) | (1 << COLLISION_CATEGORY_MOUSE);
    }
    else
    {
        params.m_collisionCategory = 1 << COLLISION_CATEGORY_PARTIAL_RAGDOLL;
        params.m_collisionMask = 1 << COLLISION_CATEGORY_MOUSE;
    }
#line 69
    params.m_bCollideWithSameActor = (pdefPhysics->dwFlags >> PHYSICSDEF_COLLIDE_WITH_SAME_ACTOR_BIT) & 1;
#line 72
    params.m_bDisableSleep = TRUE;
    // Inference: original line 72 stores TRUE and adjacent disturbance FALSE together.
#line 73
    params.m_bAffectedByDisturbance = FALSE;
#line 75
    params.m_groupIndex = 0;

    if (params.m_bCollideWithSameActor)
    {
        params.m_groupIndex = m_nGroupIndex;
    }
#line 83
    const ActorAnim * ptActorAnim = ptRActor->ptActorAnim;
    for (int32 i = 0; i < ptStructure->nBoneCount && m_nBoneCount < MAX_RAGDOLL_BONES; ++i)
    {
        const BoneStructure * ptBoneStructure = &ptStructure->ptBoneStructure[i];
#line 89
        if (!ptBoneStructure->nShapeCount)
            continue;
#line 95
        BOOL bKinematicBone = FALSE;
        for (int32 j = 0; j < ptBoneStructure->nShapeCount; ++j)
        {
            if (ptBoneStructure->ptShapes[j].dwFlags & (1 << SHAPE_KINEMATIC_BIT))
            {
                bKinematicBone = TRUE;
                break;
            }
        }
#line 106
        if (!bKinematicBone)
        {
            BOOL bDynamicSubtree = FALSE;
            int32 nAnimParent = ptBoneStructure->nParentIndex;
            while (nAnimParent != -1)
            {
#line 113
                if (m_anBoneMap[nAnimParent] >= 0)
                {
                    RagdollBone * ptParentBone = &m_ptBones[m_anBoneMap[nAnimParent]];
#line 120
                    if (ptBoneStructure->ptConstraint)
                    {
#line 127
                        if ((ptBoneStructure->ptConstraint->dwFlags & CONSTRAINT_KINEMATIC) ||
                            (ptParentBone->ptBody && ptParentBone->ptBody->GetType() == RigidBody::DYNAMIC))
                        {
                            bDynamicSubtree = TRUE;
                        }
                    }
                    break;
                }
                nAnimParent = ptStructure->ptBoneStructure[nAnimParent].nParentIndex;
            }
            if (!bDynamicSubtree)
                continue;
        }
#line 145
        ProcessedBone * ptAnimBone = &ptActorAnim->ptProcessedBoneList[i];
        params.m_transform = actorTransform * ptAnimBone->modelTransform;

        RagdollBone * ptRagdollBone = &m_ptBones[m_nBoneCount];
        ptRagdollBone->ptBody = gptPhysicsAPIImpl->CreateBodyFromBone(params, id, i);
        if (!ptRagdollBone->ptBody)
            continue;
#line 157
        ptRagdollBone->flInitialVelocityFactor = 0.0f;
        ptRagdollBone->flWindPhase = 0.0f;

        if (bKinematicBone)
        {
            ptRagdollBone->ptBody->SetType(RigidBody::KINEMATIC);
        }
#line 166
        ptRagdollBone->nAnimIndex = i;
        m_anBoneMap[i] = m_nBoneCount;
#line 170
        ptRagdollBone->nParentIndex = -1;
        if (!bKinematicBone)
        {
            int32 nAnimParent = ptBoneStructure->nParentIndex;
            while (nAnimParent != -1)
            {
                if (m_anBoneMap[nAnimParent] == LEAF_BONE)
                {
#line 179
                    m_anBoneMap[nAnimParent] = INTERMEDIATE_BONE;
                }
                else if (m_anBoneMap[nAnimParent] >= 0)
                {
                    ptRagdollBone->nParentIndex = m_anBoneMap[nAnimParent];
                    break;
                }
#line 187
                nAnimParent = ptStructure->ptBoneStructure[nAnimParent].nParentIndex;
            }
        }
#line 192
        ptRagdollBone->ptConstraint = NULL;
        if (ptBoneStructure->ptConstraint && ptRagdollBone->nParentIndex >= 0)
        {
            ConstraintParameters * ptParams = ptBoneStructure->ptConstraint;
            RagdollBone * ptParent = &m_ptBones[ptRagdollBone->nParentIndex];
            if (ptParent->ptBody)
            {
                ptRagdollBone->ptConstraint = gptPhysicsAPIImpl->CreateConstraint(ptParent->ptBody, ptRagdollBone->ptBody, ptParams, m_idRActor);
                if (ptRagdollBone->ptConstraint)
                {
                    ptRagdollBone->ptConstraint->m_dwHash = ptParams->dwHash;
                }
            }
        }




        ++m_nBoneCount;
    }
#line 214
    if (m_nBoneCount > 0 && m_ptBones[0].ptBody)
    {
#line 217
        const float cflInertiaFactor = 1.5f;
        const float cflMassRatio = 0.75f;
        m_ptBones[0].ptBody->StabilizeInertias(cflInertiaFactor, cflMassRatio);
    }
#line 223
    m_nLoopCount = Math::Min(ptStructure->nLoopConstraintCount, (int32)MAX_RAGDOLL_LOOPS);
    for (int32 i = 0; i < m_nLoopCount; ++i)
    {
        const ConstraintParameters * ptLoopParams = &ptStructure->ptLoopConstraints[i];

        int32 nBone1 = m_anBoneMap[ptLoopParams->nBone1];
        int32 nBone2 = m_anBoneMap[ptLoopParams->nBone2];
#line 235
        RagdollBone * ptBone1 = &m_ptBones[nBone1];
        RagdollBone * ptBone2 = &m_ptBones[nBone2];

        if (ptBone1->ptBody && ptBone2->ptBody)
        {
#line 243
            m_ptLoops[i] = gptPhysicsAPIImpl->CreateConstraint(ptBone1->ptBody, ptBone2->ptBody, ptLoopParams, m_idRActor);
            if (m_ptLoops[i])
            {
#line 247
                m_ptLoops[i]->m_dwHash = ptLoopParams->dwHash;
            }
        }
    }
#line 254
    InstallBoneControllers();

    DominoWorld::s_currentActor = SNO_NONE;
}
#line 261
PartialRagdoll::~PartialRagdoll()
{
}
#line 30 "X/Client/Physics/PartialRagdoll.h"
BOOL PartialRagdoll::ShouldUpdateBounds() const
{
    return FALSE;
}
