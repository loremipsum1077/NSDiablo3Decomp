#include "BreakableRagdoll.h"
#include "DominoClient.h"
#include "../RActors/RActors.h"
#include "../Definitions/AppearanceDefinition.h"
#include "../../Sigma/Math/Constraint.h"
#include "DominoBody.h"
#include "../Explosions.h"
#include "../RActors/RActorAnimation.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Common/Definitions/ActorDefinition.h"
#include "../Definitions/PhysicsDefinition.h"
// CollisionShape declaration: DWARF X/Client/Definitions/AppearanceDefinition.h:148.
struct PolytopeData;
struct CollisionShape {
    enum Type { NONE = -1, SPHERE = 0, CAPSULE = 1, POLYTOPE = 2 };
    uint32 dwFlags; Type eType; Shape::Material eMaterial; int32 nDegrade;
    float flDensityFactor; float flExplosionFactor; float flVelocityFactor;
    SerializeData serPolytope; XPtr64<PolytopeData> pPolytopeData;
    Vector3D wp1; Vector3D wp2; float flRadius;
    BOOL Create(const Shape *); void Destroy(); void Rotate(const Quaternion &);
};
static_assert(sizeof(CollisionShape) == 0x50, "CollisionShape");
#include "../CGame.h"
#include "../Particle.h"




BreakableRagdoll::BreakableRagdoll(
    RActorID id)
    : Ragdoll(id)
{
    m_nType = BREAKABLE_RAGDOLL_TYPE;
    m_nBoneCount = m_nLoopCount = 0;
    m_bNeedsInitialVelocity = FALSE;
    m_bIsBroken = FALSE;

    m_bHasDynamicBodies = FALSE;

    RActor * ptRActor = RActorGet(m_idRActor);
    ScopedAppearanceDefinitionLockR pdefAppearance(ptRActor->snoAppearance);
    const Structure * ptStructure = &pdefAppearance->tStructure;


    RNG & rng = CGameGlobalsGet().rngLocal;

    SigmaMemoryClear(m_ptBones, sizeof(m_ptBones));
    for (int32 i = 0; i < ptStructure->nBoneCount && m_nBoneCount < MAX_RAGDOLL_BONES; i++)
    {
        const BoneStructure * ptBoneStructure = &ptStructure->ptBoneStructure[i];


        if (!ptBoneStructure->nShapeCount)
            continue;



        RagdollBone * ptRagdollBone = &m_ptBones[m_nBoneCount];
        ptRagdollBone->ptConstraint = NULL;
        ptRagdollBone->flInitialVelocityFactor = 0.0f;
        for (int32 j = 0; j < ptBoneStructure->nShapeCount; j++)
        {
            ptRagdollBone->flInitialVelocityFactor += ptBoneStructure->ptShapes[j].flVelocityFactor;
        }
        ptRagdollBone->flInitialVelocityFactor /= ptBoneStructure->nShapeCount;

        ptRagdollBone->flWindPhase = RandFloat_Internal(rng) * FULL_CIRCLE;


        ptRagdollBone->nAnimIndex = i;
        m_anBoneMap[i] = m_nBoneCount;


        ptRagdollBone->nParentIndex = -1;
        ptRagdollBone->nSiblingIndex = ptRagdollBone->nChildIndex = -1;

        ptRagdollBone->nRootIndex = m_nBoneCount;
        int32 nAnimParent = ptBoneStructure->nParentIndex;
        while (nAnimParent != -1)
        {
            if (m_anBoneMap[nAnimParent] == LEAF_BONE)
            {

                m_anBoneMap[nAnimParent] = INTERMEDIATE_BONE;
            }
            else if (m_anBoneMap[nAnimParent] >= 0)
            {
                ptRagdollBone->nParentIndex = m_anBoneMap[nAnimParent];
                RagdollBone * ptParent = &m_ptBones[ptRagdollBone->nParentIndex];
                ptRagdollBone->nSiblingIndex = ptParent->nChildIndex;
                ptParent->nChildIndex = m_nBoneCount;
                if (!ptBoneStructure->ptConstraint || ptBoneStructure->ptConstraint->eType == CONSTRAINT_TYPE_BREAKABLE)
                {
                    ptRagdollBone->nRootIndex = ptParent->nRootIndex;
                }
                break;
            }

            nAnimParent = ptStructure->ptBoneStructure[nAnimParent].nParentIndex;
        }

        m_nBoneCount++;
    }

    m_bHasLeaves = FALSE;
    for (int32 i = 0; i < ptStructure->nBoneCount; i++)
    {
        if (m_anBoneMap[i] == LEAF_BONE)
        {
            m_bHasLeaves = TRUE;
            break;
        }
    }

    const PRSTransform & actorTransform = ptRActor->transformClient;

    ScopedActorDefinitionLockR pdefActor(ptRActor->snoActor);

    ScopedPhysicsDefinitionLockR pdefPhysics(ptRActor->snoPhysics);


    RigidBodyParams tBodyParams;
    tBodyParams.m_materialOverride = pdefActor->eMaterial;
    tBodyParams.m_angularDamping = pdefPhysics->flAngularDamping;
    tBodyParams.m_linearDamping = pdefPhysics->flLinearDamping;
    tBodyParams.m_scale = actorTransform.flScale;
    tBodyParams.m_friction = pdefPhysics->flFriction;
    tBodyParams.m_rollingResistance = pdefPhysics->flRollingResistance;
    tBodyParams.m_restitution = pdefPhysics->flRestitution;
    tBodyParams.m_collisionCategory = 1 << COLLISION_CATEGORY_BREAKABLE;
    tBodyParams.m_collisionMask = (pdefPhysics->wCollisionMask & ~((1 << COLLISION_CATEGORY_WATER) | (1 << COLLISION_CATEGORY_MOUSE) | (1 << COLLISION_CATEGORY_SMOKE))) | (1 << COLLISION_CATEGORY_MOUSE);
    tBodyParams.m_bCollideWithSameActor = (pdefPhysics->dwFlags >> PHYSICSDEF_COLLIDE_WITH_SAME_ACTOR_BIT) & 1;
    tBodyParams.m_groupIndex = 0;
    tBodyParams.m_nBoneCount = m_nBoneCount;
    tBodyParams.m_bDisableSleep = (pdefPhysics->dwFlags >> PHYSICSDEF_DO_NOT_AUTOSLEEP_BIT) & 1;
    tBodyParams.m_bAffectedByDisturbance = (pdefPhysics->dwFlags >> PHYSICSDEF_AFFECTED_BY_DISTURBANCE_BIT) & 1;

    if (tBodyParams.m_bCollideWithSameActor)
    {
        tBodyParams.m_groupIndex = m_nGroupIndex;
    }


    for (int32 i = 0; i < m_nBoneCount; i++)
    {
        RagdollBone * ptBone = &m_ptBones[i];


        if (ptBone->nRootIndex != i)
            continue;




        tBodyParams.m_nBoneCount = 0;

        int32 anStack[MAX_RAGDOLL_BONES];
        int32 nStack = 0;
        anStack[nStack++] = i;

        while (nStack > 0)
        {
            int32 nNodeIndex = anStack[--nStack];

            RagdollBone * ptNode = &m_ptBones[nNodeIndex];

            tBodyParams.m_nBones[tBodyParams.m_nBoneCount++] = ptNode->nAnimIndex;


            int32 nChildIndex = ptNode->nChildIndex;
            while (nChildIndex != -1)
            {
                RagdollBone * ptChild = &m_ptBones[nChildIndex];

                if (ptChild->nRootIndex == i)
                {
                    anStack[nStack++] = nChildIndex;
                }

                nChildIndex = ptChild->nSiblingIndex;
            }
        }


        if (i == 0)
        {
            tBodyParams.m_bStatic = TRUE;
        }
        else
        {
            tBodyParams.m_bStatic = FALSE;
            m_bHasDynamicBodies = TRUE;
        }


        const BoneStructure * ptBoneStructure = &ptStructure->ptBoneStructure[ptBone->nAnimIndex];
        tBodyParams.m_transform = actorTransform * ptBoneStructure->transform;

        ptBone->ptBody = gptPhysicsAPIImpl->CreateBodyFromBones(tBodyParams, m_idRActor);
        if (!ptBone->ptBody)
            continue;




        CreateConstraint(ptStructure, i);
    }

    InstallBoneControllers();

    m_bAnimationOverride = FALSE;

    DominoWorld::s_currentActor = SNO_NONE;
}



BreakableRagdoll::~BreakableRagdoll()
{
}





void BreakableRagdoll::Break()

{
    RActor * ptRActor = RActorGet(m_idRActor);


    if (m_bIsBroken == TRUE)
        return;




    const PRSTransform & actorTransform = RActorGetClientTransform(ptRActor);
    ScopedAppearanceDefinitionLockR pdefAppearance(ptRActor->snoAppearance);
    const Structure * ptStructure = &pdefAppearance->tStructure;
    ActorAnim * ptAnim = ptRActor->ptActorAnim;


    for (int32 i = 0; i < m_nBoneCount; i++)
    {
        RagdollBone * ptBone = &m_ptBones[i];
        RagdollBone * ptRoot = &m_ptBones[ptBone->nRootIndex];

        if (ptRoot->ptBody && ptRoot->ptBody->GetType() == RigidBody::DYNAMIC)
        {
            Vector3D wvOmega = ptRoot->ptBody->GetAngularVelocity();
            Vector3D wvCenterVelocity = ptRoot->ptBody->GetCenterOfMassVelocity();

            ptBone->wvInitialAngularVelocity = wvOmega;
            Vector3D wpCenter = ptRoot->ptBody->GetCenterOfMassPosition();
            Vector3D wpBone = actorTransform * ptAnim->ptProcessedBoneList[ptBone->nAnimIndex].modelTransform.wp;
            ptBone->wvInitialLinearVelocity = wvCenterVelocity + Vector3D::CrossProduct(wvOmega, wpBone - wpCenter);
        }
        else
        {
            ptBone->wvInitialAngularVelocity = cgwvNull;
            ptBone->wvInitialLinearVelocity = cgwvNull;
        }
    }


    for (int32 i = 0; i < m_nBoneCount; i++)
    {
        if (m_ptBones[i].ptConstraint)
        {
            gptPhysicsAPIImpl->DestroyConstraint(m_ptBones[i].ptConstraint);
            m_ptBones[i].ptConstraint = NULL;
        }
    }


    for (int32 i = 0; i < m_nBoneCount; i++)
    {
        if (m_ptBones[i].ptBody)
        {
            gptPhysicsAPIImpl->DestroyBody(m_ptBones[i].ptBody);
            m_ptBones[i].ptBody = NULL;
        }
    }


    for (int32 i = 0; i < m_nBoneCount; i++)
    {
        RagdollBone * ptBone = &m_ptBones[i];
        const BoneStructure * ptBoneStructure = &ptStructure->ptBoneStructure[ptBone->nAnimIndex];

        if (ptBone->nParentIndex == -1 || !ptBoneStructure->ptConstraint ||
            ptBoneStructure->ptConstraint->eType != CONSTRAINT_TYPE_BREAKABLE)
            ptBone->nRootIndex = i;
        else
        {



            ptBone->nRootIndex = m_ptBones[ptBone->nParentIndex].nRootIndex;
        }
    }

    ScopedActorDefinitionLockR pdefActor(ptRActor->snoActor);

    ScopedPhysicsDefinitionLockR pdefPhysics(ptRActor->snoPhysics);


    RigidBodyParams tBodyParams;
    tBodyParams.m_materialOverride = pdefActor->eMaterial;
    tBodyParams.m_angularDamping = pdefPhysics->flAngularDamping;
    tBodyParams.m_linearDamping = pdefPhysics->flLinearDamping;
    tBodyParams.m_scale = actorTransform.flScale;
    tBodyParams.m_friction = pdefPhysics->flFriction;
    tBodyParams.m_rollingResistance = pdefPhysics->flRollingResistance;
    tBodyParams.m_restitution = pdefPhysics->flRestitution;
    tBodyParams.m_collisionCategory = 1 << COLLISION_CATEGORY_BREAKABLE;
    tBodyParams.m_collisionMask = (pdefPhysics->wCollisionMask & ~(1 << COLLISION_CATEGORY_WATER)) | (1 << COLLISION_CATEGORY_MOUSE);
    tBodyParams.m_groupIndex = 0;
    tBodyParams.m_bCollideWithSameActor = (pdefPhysics->dwFlags >> PHYSICSDEF_COLLIDE_WITH_SAME_ACTOR_BIT) & 1;
    tBodyParams.m_bDisableSleep = (pdefPhysics->dwFlags >> PHYSICSDEF_DO_NOT_AUTOSLEEP_BIT) & 1;
    tBodyParams.m_bAffectedByDisturbance = (pdefPhysics->dwFlags >> PHYSICSDEF_AFFECTED_BY_DISTURBANCE_BIT) & 1;

    if (tBodyParams.m_bCollideWithSameActor)
    {
        tBodyParams.m_groupIndex = m_nGroupIndex;
    }


    for (int32 i = 0; i < m_nBoneCount; i++)
    {
        RagdollBone * ptBone = &m_ptBones[i];


        if (ptBone->nRootIndex != i)
            continue;




        tBodyParams.m_nBoneCount = 0;

        int32 anStack[MAX_RAGDOLL_BONES];
        int32 nStack = 0;
        anStack[nStack++] = i;

        while (nStack > 0)
        {
            int32 nNodeIndex = anStack[--nStack];
            RagdollBone * ptNode = &m_ptBones[nNodeIndex];

            if (ptNode->nRootIndex == i)
            {
                tBodyParams.m_nBones[tBodyParams.m_nBoneCount++] = ptNode->nAnimIndex;


                int32 nChildIndex = ptNode->nChildIndex;
                while (nChildIndex != -1)
                {
                    anStack[nStack++] = nChildIndex;
                    RagdollBone * ptChild = &m_ptBones[nChildIndex];
                    nChildIndex = ptChild->nSiblingIndex;
                }
            }
        }


        tBodyParams.m_transform = actorTransform * ptAnim->ptProcessedBoneList[ptBone->nAnimIndex].modelTransform;


        tBodyParams.m_bStatic = TRUE;
        const BoneStructure * ptBoneStructure = &ptStructure->ptBoneStructure[ptBone->nAnimIndex];
        for (int32 j = 0; j < ptBoneStructure->nShapeCount; j++)
        {
            if (ptBoneStructure->ptShapes[j].flDensityFactor > 0.0f)
            {
                tBodyParams.m_bStatic = FALSE;
                break;
            }
        }


        Vector3D wvOmega = ptBone->wvInitialAngularVelocity;
        tBodyParams.m_angularVel = wvOmega;

        ptBone->ptBody = gptPhysicsAPIImpl->CreateBodyFromBones(tBodyParams, m_idRActor);
        if (!ptBone->ptBody)
            continue;



        CreateConstraint(ptStructure, i);

        Vector3D wpOrigin = tBodyParams.m_transform.wp;
        Vector3D wpCenter = ptBone->ptBody->GetCenterOfMassPosition();
        Vector3D wvCenterVelocity = ptBone->wvInitialLinearVelocity + Vector3D::CrossProduct(wvOmega, wpCenter - wpOrigin);
        ptBone->ptBody->SetCenterOfMassVelocity(wvCenterVelocity);
    }

    m_bIsBroken = TRUE;


    m_bNeedsInitialVelocity = TRUE;


    m_bAnimationOverride = FALSE;
}









void BreakableRagdoll::Split(
    int32 nSplitIndex)
{
    RagdollBone * ptSplit = &m_ptBones[nSplitIndex];

    if (ptSplit->nParentIndex == -1)
        return;



    RActor * ptRActor = RActorGet(m_idRActor);
    ScopedAppearanceDefinitionLockR pdefAppearance(ptRActor->snoAppearance);
    const Structure * ptStructure = &pdefAppearance->tStructure;


    int32 nRootIndex = ptSplit->nRootIndex;
    RagdollBone * ptRoot = &m_ptBones[nRootIndex];
    if (!ptRoot->ptBody)
        return;




    PRTransform rootTransform;
    ptRoot->ptBody->GetTransform(rootTransform);
    Vector3D wpCenterOldRoot = ptRoot->ptBody->GetCenterOfMassPosition();
    Vector3D wvLinearVelocity = ptRoot->ptBody->GetCenterOfMassVelocity();
    Vector3D wvAngularVelocity = ptRoot->ptBody->GetAngularVelocity();


    int32 anConstraintRecords[MAX_RAGDOLL_BONES];
    int32 nConstraintRecordCount = 0;


    if (ptRoot->ptConstraint)
    {
        gptPhysicsAPIImpl->DestroyConstraint(ptRoot->ptConstraint);
        ptRoot->ptConstraint = NULL;
        anConstraintRecords[nConstraintRecordCount++] = nRootIndex;
    }


    int32 anStack[MAX_RAGDOLL_BONES];
    int32 nStack = 0;
    anStack[nStack++] = nRootIndex;

    while (nStack > 0)
    {
        int32 nNodeIndex = anStack[--nStack];
        RagdollBone * ptNode = &m_ptBones[nNodeIndex];


        if (ptNode->nRootIndex != nRootIndex)
        {

            if (ptNode->ptConstraint)
            {
                gptPhysicsAPIImpl->DestroyConstraint(ptNode->ptConstraint);
                ptNode->ptConstraint = NULL;

                anConstraintRecords[nConstraintRecordCount++] = nNodeIndex;
            }

        }

        else
        {

            int32 nChildIndex = ptNode->nChildIndex;
            while (nChildIndex != -1)
            {
                anStack[nStack++] = nChildIndex;
                RagdollBone * ptChild = &m_ptBones[nChildIndex];
                nChildIndex = ptChild->nSiblingIndex;
            }
        }
    }


    gptPhysicsAPIImpl->DestroyBody(ptRoot->ptBody);
    ptRoot->ptBody = NULL;

    // Inferred stack reset between traversals; the preceding loop exhausts it, so no separate instruction remains.
    nStack = 0;
    anStack[nStack++] = nSplitIndex;

    while (nStack > 0)
    {
        int32 nNodeIndex = anStack[--nStack];
        RagdollBone * ptNode = &m_ptBones[nNodeIndex];


        if (ptNode->nRootIndex == nRootIndex)
        {
            ptNode->nRootIndex = nSplitIndex;


            int32 nChildIndex = ptNode->nChildIndex;
            while (nChildIndex != -1)
            {
                anStack[nStack++] = nChildIndex;
                RagdollBone * ptChild = &m_ptBones[nChildIndex];
                nChildIndex = ptChild->nSiblingIndex;
            }
        }
    }


    CreateSubtreeBody(nRootIndex);
    CreateSubtreeBody(nSplitIndex);


    for (int32 i = 0; i < nConstraintRecordCount; i++)
    {
        CreateConstraint(ptStructure, anConstraintRecords[i]);
    }

    if (!ptRoot->ptBody || !ptSplit->ptBody)
        return;




    Vector3D wpCenterRoot = ptRoot->ptBody->GetCenterOfMassPosition();
    Vector3D wpCenterSplit = ptSplit->ptBody->GetCenterOfMassPosition();

    Vector3D wvRootVelocity = wvLinearVelocity + Vector3D::CrossProduct(wvAngularVelocity, wpCenterRoot - wpCenterOldRoot);
    Vector3D wvSplitVelocity = wvLinearVelocity + Vector3D::CrossProduct(wvAngularVelocity, wpCenterSplit - wpCenterOldRoot);

    ptRoot->ptBody->SetCenterOfMassVelocity(wvRootVelocity);
    ptRoot->ptBody->SetAngularVelocity(wvAngularVelocity);

    ptSplit->ptBody->SetCenterOfMassVelocity(wvSplitVelocity);
    ptSplit->ptBody->SetAngularVelocity(wvAngularVelocity);



    const BoneStructure * ptBoneStructure = &ptStructure->ptBoneStructure[ptSplit->nAnimIndex];
    if (ptBoneStructure->snoParticleSystem != SNO_NONE)
    {
        PRSTransform localFrame = ptBoneStructure->transformInv * ptBoneStructure->ptConstraint->frame;
        PRSTransform frame = ptRActor->transformClient * ptRActor->ptActorAnim->ptProcessedBoneList[ptSplit->nAnimIndex].modelTransform * localFrame;
        PRTransform frameNoScale; frameNoScale = frame;
        ParticleSystemAddParams tParams(ptBoneStructure->snoParticleSystem, frameNoScale);
        ParticleSystemAdd(tParams);
    }
}



void BreakableRagdoll::CreateSubtreeBody(
    int32 nBoneIndex)
{
    RActor * ptRActor = RActorGet(m_idRActor);
    const PRSTransform & actorTransform = ptRActor->transformClient;

    ScopedAppearanceDefinitionLockR pdefAppearance(ptRActor->snoAppearance);
    const Structure * ptStructure = &pdefAppearance->tStructure;

    ScopedActorDefinitionLockR pdefActor(ptRActor->snoActor);

    ScopedPhysicsDefinitionLockR pdefPhysics(ptRActor->snoPhysics);


    RigidBodyParams tBodyParams;
    tBodyParams.m_materialOverride = pdefActor->eMaterial;
    tBodyParams.m_angularDamping = pdefPhysics->flAngularDamping;
    tBodyParams.m_linearDamping = pdefPhysics->flLinearDamping;
    tBodyParams.m_scale = actorTransform.flScale;
    tBodyParams.m_friction = pdefPhysics->flFriction;
    tBodyParams.m_rollingResistance = pdefPhysics->flRollingResistance;
    tBodyParams.m_restitution = pdefPhysics->flRestitution;
    tBodyParams.m_collisionCategory = 1 << COLLISION_CATEGORY_BREAKABLE;
    tBodyParams.m_collisionMask = (pdefPhysics->wCollisionMask & ~(1 << COLLISION_CATEGORY_WATER)) | (1 << COLLISION_CATEGORY_MOUSE);
    tBodyParams.m_groupIndex = 0;
    tBodyParams.m_bCollideWithSameActor = (pdefPhysics->dwFlags >> PHYSICSDEF_COLLIDE_WITH_SAME_ACTOR_BIT) & 1;
    tBodyParams.m_bDisableSleep = (pdefPhysics->dwFlags >> PHYSICSDEF_DO_NOT_AUTOSLEEP_BIT) & 1;
    tBodyParams.m_bAffectedByDisturbance = (pdefPhysics->dwFlags >> PHYSICSDEF_AFFECTED_BY_DISTURBANCE_BIT) & 1;

    if (tBodyParams.m_bCollideWithSameActor)
    {
        tBodyParams.m_groupIndex = m_nGroupIndex;
    }


    RagdollBone * ptBone = &m_ptBones[nBoneIndex];




    tBodyParams.m_nBoneCount = 0;

    int32 anStack[MAX_RAGDOLL_BONES];
    int32 nStack = 0;
    anStack[nStack++] = nBoneIndex;

    while (nStack > 0)
    {
        int32 nNodeIndex = anStack[--nStack];

        RagdollBone * ptNode = &m_ptBones[nNodeIndex];

        if (ptNode->nRootIndex == nBoneIndex)
        {
            tBodyParams.m_nBones[tBodyParams.m_nBoneCount++] = ptNode->nAnimIndex;


            int32 nChildIndex = ptNode->nChildIndex;
            while (nChildIndex != -1)
            {
                anStack[nStack++] = nChildIndex;
                RagdollBone * ptChild = &m_ptBones[nChildIndex];
                nChildIndex = ptChild->nSiblingIndex;
            }
        }
    }


    tBodyParams.m_transform = actorTransform * ptRActor->ptActorAnim->ptProcessedBoneList[ptBone->nAnimIndex].modelTransform;


    const BoneStructure * ptRootBoneStructure = &ptStructure->ptBoneStructure[ptBone->nAnimIndex];
    tBodyParams.m_bStatic = TRUE;

    for (int32 i = 0; i < ptRootBoneStructure->nShapeCount; i++)
    {
        if (ptRootBoneStructure->ptShapes[i].flDensityFactor > 0.0f)
        {
            tBodyParams.m_bStatic = FALSE;
            break;
        }
    }

    ptBone->ptBody = gptPhysicsAPIImpl->CreateBodyFromBones(tBodyParams, m_idRActor);
}








void BreakableRagdoll::CreateConstraint(
    const Structure * ptStructure,
    int32 nBoneIndex)
{


    RagdollBone * ptBone = &m_ptBones[nBoneIndex];
    if (!ptBone->ptBody)
        return;






    const BoneStructure * ptBoneStructure = &ptStructure->ptBoneStructure[ptBone->nAnimIndex];
    const ConstraintParameters * ptConstraintParameters = ptBoneStructure->ptConstraint;

    if (ptConstraintParameters &&
        ptConstraintParameters->eType != CONSTRAINT_TYPE_BREAKABLE &&
        ptBone->nParentIndex != -1)
    {
        RagdollBone * ptParent = &m_ptBones[ptBone->nParentIndex];
        RagdollBone * ptParentRoot = &m_ptBones[ptParent->nRootIndex];
        if (ptParentRoot->ptBody)
        {
            ConstraintParameters tConstraintParameters;
            SigmaMemoryCopy(&tConstraintParameters, ptConstraintParameters, sizeof(ConstraintParameters));

            const BoneStructure * ptParentBoneStructure = &ptStructure->ptBoneStructure[ptParent->nAnimIndex];
            const BoneStructure * ptParentRootBoneStructure = &ptStructure->ptBoneStructure[ptParentRoot->nAnimIndex];


            tConstraintParameters.localFrame1 = ptParentRootBoneStructure->transformInv * ptParentBoneStructure->transform * ptConstraintParameters->localFrame1;

            ptBone->ptConstraint = gptPhysicsAPIImpl->CreateConstraint(ptParentRoot->ptBody, ptBone->ptBody, &tConstraintParameters, m_idRActor);
            if (ptBone->ptConstraint)
            {

                ptBone->ptConstraint->m_dwHash = ptConstraintParameters->dwHash;
            }
        }
    }



}



void BreakableRagdoll::ApplyExplosion(
    const ExplosionImpulse & tExplosion,
    float flExplosionFactor)
{


    if (!m_bIsBroken)
        return;



    RActor * ptRActor = RActorGet(m_idRActor);
    ScopedAppearanceDefinitionLockR pdefAppearance(ptRActor->snoAppearance);
    const Structure * ptStructure = &pdefAppearance->tStructure;

    const PRSTransform & actorTransform = ptRActor->transformClient;

    for (int32 i = 0; i < m_nBoneCount; i++)
    {
        RagdollBone * ptBone = &m_ptBones[i];
        if (ptBone->ptBody)
        {
            const float cflMaxDeltaVelocity = 50.0f;
            ptBone->ptBody->ApplyExplosion(tExplosion, flExplosionFactor, cflMaxDeltaVelocity);
        }
        else
        {

            const BoneStructure * ptBoneStructure = &ptStructure->ptBoneStructure[ptBone->nAnimIndex];
            if (ptBoneStructure->ptConstraint)
            {
                Vector3D wp = actorTransform * ptRActor->ptActorAnim->ptProcessedBoneList[ptBone->nAnimIndex].modelTransform.wp;
                Vector3D wvImpulse = tExplosion.ComputeImpulse(wp) * 60.0f;


                const float cflFudgeFactor = 2.0f;
                const float cflStrength = cflFudgeFactor * ptBoneStructure->ptConstraint->flStrength;
                const float cflStrengthSqr = cflStrength * cflStrength;
                if (wvImpulse.MagnitudeSquared() > cflStrengthSqr)
                {
                    Split(i);
                }
            }
        }
    }
}




void BreakableRagdoll::ApplyCollision(
    const ContactPoint * ptPoint)
{


    if (!m_bIsBroken)
        return;



    int32 nAnimBoneIndex;
    if (ptPoint->idRActor1 == m_idRActor)
    {

        nAnimBoneIndex = ptPoint->tShapeData1.wBoneIndex;
    }
    else if (ptPoint->idRActor2 == m_idRActor)
    {

        nAnimBoneIndex = ptPoint->tShapeData2.wBoneIndex;
    }
    else
    {
        return;
    }




    int32 nBoneIndex = m_anBoneMap[nAnimBoneIndex];


    RActor * ptRActor = RActorGet(m_idRActor);
    ScopedAppearanceDefinitionLockR pdefAppearance(ptRActor->snoAppearance);
    const Structure * ptStructure = &pdefAppearance->tStructure;

    RagdollBone * ptBone = &m_ptBones[nBoneIndex];
    if (!ptBone->ptBody)
    {



        const BoneStructure * ptBoneStructure = &ptStructure->ptBoneStructure[ptBone->nAnimIndex];
        const float cflStrength = ptBoneStructure->ptConstraint->flStrength;
        if (ptPoint->impulse > cflStrength)
        {

            Split(nBoneIndex);
        }
    }
    else
    {


        const int32 cnMaxSplitCount = 16;
        int32 anSplitChildren[cnMaxSplitCount];

        int32 nSplitCount = 0;
        int32 nChildIndex = ptBone->nChildIndex;
        while (nChildIndex != -1 && nSplitCount < cnMaxSplitCount)
        {
            RagdollBone * ptChildBone = &m_ptBones[nChildIndex];


            if (ptChildBone->nRootIndex == nBoneIndex)
            {



                const BoneStructure * ptBoneStructure = &ptStructure->ptBoneStructure[ptChildBone->nAnimIndex];
                const float cflStrength = ptBoneStructure->ptConstraint->flStrength;
                if (ptPoint->impulse > cflStrength)
                {

                    anSplitChildren[nSplitCount++] = nChildIndex;
                }
            }

            // Inferred loop continuation: line-0 branch follows the sibling load at 839.
            nChildIndex = ptChildBone->nSiblingIndex;
        }

        for (int32 i = 0; i < nSplitCount; i++)
        {
            Split(anSplitChildren[i]);
        }
    }
}

// Out-of-class definitions originally at BreakableRagdoll.h:80 and :92; no declared-inline DWARF attribute.
#line 80 "X/Client/Physics/BreakableRagdoll.h"
BOOL BreakableRagdoll::ShouldAnimate() const
{

    return m_bHasLeaves || m_bIsBroken || m_flLifeTime == 0.0f || m_bHasDynamicBodies || m_bAnimationOverride;
}
#line 92 "X/Client/Physics/BreakableRagdoll.h"
BOOL BreakableRagdoll::ShouldUpdateBounds() const
{
    return m_bIsBroken;
}
