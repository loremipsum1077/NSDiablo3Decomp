#include "AttachedRagdoll.h"
#include "DominoBody.h"
#include "DominoClient.h"
#include "../RActors/RActors.h"
#include "../RActors/RActorAnimation.h"
#include "../Definitions/AppearanceDefinition.h"
#include "../Definitions/PhysicsDefinition.h"
#include "../../Common/Definitions/ActorDefinition.h"
#include "../../Sigma/Math/Constraint.h"

AttachedRagdoll::AttachedRagdoll(RActorID id) : Ragdoll(id)
{
    m_nType = ATTACHED_RAGDOLL_TYPE;
    m_nBoneCount = 0;
    m_nLoopCount = 0;
    m_bNeedsInitialVelocity = FALSE;
    RActor * ptRActor = RActorGet(m_idRActor);
    ScopedAppearanceDefinitionLockR pdefAppearance(ptRActor->snoAppearance);
    const Structure * ptStructure = &pdefAppearance->tStructure;
    PRSTransform actorTransform = ptRActor->transformClient;
    ScopedActorDefinitionLockR pdefActor(ptRActor->snoActor);
    ScopedPhysicsDefinitionLockR pdefPhysics(ptRActor->snoPhysics);
    RigidBodyParams params;
    params.m_materialOverride = pdefActor->eMaterial;
    params.m_angularDamping = pdefPhysics->flAngularDamping;
    params.m_linearDamping = pdefPhysics->flLinearDamping;
    params.m_scale = actorTransform.flScale;
    params.m_friction = pdefPhysics->flFriction;
    params.m_rollingResistance = pdefPhysics->flRollingResistance;
    params.m_restitution = pdefPhysics->flRestitution;
    params.m_collisionCategory = 1 << COLLISION_CATEGORY_FULL_RAGDOLL;
    params.m_collisionMask = (pdefPhysics->wCollisionMask & ~(1 << COLLISION_CATEGORY_WATER)) | (1 << COLLISION_CATEGORY_MOUSE);
    params.m_bCollideWithSameActor = (pdefPhysics->dwFlags >> PHYSICSDEF_COLLIDE_WITH_SAME_ACTOR_BIT) & 1;
    params.m_bDisableSleep = (pdefPhysics->dwFlags >> PHYSICSDEF_DO_NOT_AUTOSLEEP_BIT) & 1;
    params.m_groupIndex = 0;
    if (params.m_bCollideWithSameActor)
        params.m_groupIndex = m_nGroupIndex;
    const ActorAnim * ptActorAnim = ptRActor->ptActorAnim;
    for (int32 i = 0; i < ptStructure->nBoneCount && m_nBoneCount < MAX_RAGDOLL_BONES; ++i)
    {
        const BoneStructure * ptBoneStructure = &ptStructure->ptBoneStructure[i];
        if (!ptBoneStructure->nShapeCount)
            continue;
        ProcessedBone * ptAnimBone = &ptActorAnim->ptProcessedBoneList[i];
        params.m_transform = actorTransform * ptAnimBone->modelTransform;
        RagdollBone * ptRagdollBone = &m_ptBones[m_nBoneCount];
        ptRagdollBone->ptBody = gptPhysicsAPIImpl->CreateBodyFromBone(params, id, i);
        if (!ptRagdollBone->ptBody)
            continue;
        ptRagdollBone->flInitialVelocityFactor = 0.0f;
        ptRagdollBone->flWindPhase = 0.0f;
        if (m_nBoneCount == 0)
            ptRagdollBone->ptBody->SetType(RigidBody::KINEMATIC);
        ptRagdollBone->nAnimIndex = i;
        m_anBoneMap[i] = m_nBoneCount;
        ptRagdollBone->nParentIndex = -1;
        int32 nAnimParent = ptBoneStructure->nParentIndex;
        while (nAnimParent != -1)
        {
            if (m_anBoneMap[nAnimParent] == LEAF_BONE)
                m_anBoneMap[nAnimParent] = INTERMEDIATE_BONE;
            else if (m_anBoneMap[nAnimParent] >= 0)
            {
                ptRagdollBone->nParentIndex = m_anBoneMap[nAnimParent];
                break;
            }
            nAnimParent = ptStructure->ptBoneStructure[nAnimParent].nParentIndex;
        }
        ptRagdollBone->ptConstraint = NULL;
        const ConstraintParameters * ptParams = ptBoneStructure->ptConstraint;
        if (ptParams && ptRagdollBone->nParentIndex >= 0)
        {
            RagdollBone * ptParent = &m_ptBones[ptRagdollBone->nParentIndex];
            if (ptParent->ptBody)
            {
                ptRagdollBone->ptConstraint = gptPhysicsAPIImpl->CreateConstraint(ptParent->ptBody, ptRagdollBone->ptBody, ptParams, m_idRActor);
                if (ptRagdollBone->ptConstraint)
                {
                    ptRagdollBone->ptConstraint->ActivateMotor(FALSE);
                    ptRagdollBone->ptConstraint->m_dwHash = ptParams->dwHash;
                }
            }
        }
        ++m_nBoneCount;
    }
    if (m_nBoneCount > 0 && m_ptBones[0].ptBody)
    {
        const float cflInertiaFactor = 1.5f;
        const float cflMassRatio = 0.75f;
        m_ptBones[0].ptBody->StabilizeInertias(cflInertiaFactor, cflMassRatio);
    }
    m_nLoopCount = Math::Min(ptStructure->nLoopConstraintCount, (int32)MAX_RAGDOLL_LOOPS);
    for (int32 i = 0; i < m_nLoopCount; ++i)
    {
        const ConstraintParameters * ptLoopParams = &ptStructure->ptLoopConstraints[i];
        int32 nBone1 = m_anBoneMap[ptLoopParams->nBone1];
        int32 nBone2 = m_anBoneMap[ptLoopParams->nBone2];
        RagdollBone * ptBone1 = &m_ptBones[nBone1];
        RagdollBone * ptBone2 = &m_ptBones[nBone2];
        if (ptBone1->ptBody && ptBone2->ptBody)
        {
            m_ptLoops[i] = gptPhysicsAPIImpl->CreateConstraint(ptBone1->ptBody, ptBone2->ptBody, ptLoopParams, m_idRActor);
            if (m_ptLoops[i])
                m_ptLoops[i]->m_dwHash = ptLoopParams->dwHash;
        }
    }
    InstallBoneControllers();
    DominoWorld::s_currentActor = -1;
}

AttachedRagdoll::~AttachedRagdoll() {}

void AttachedRagdoll::Detach()
{
    if (m_nBoneCount > 0 && m_ptBones[0].ptBody)
        m_ptBones[0].ptBody->SetType(RigidBody::DYNAMIC);
}
