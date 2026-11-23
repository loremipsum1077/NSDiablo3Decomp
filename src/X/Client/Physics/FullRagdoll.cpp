// Original file: X/Client/Physics/FullRagdoll.cpp
#include "Ragdoll.h"
#include "DominoBody.h"
#include "DominoClient.h"
#include "../RActors/RActors.h"
#include "../RActors/RActorAnimation.h"
#include "../Definitions/AppearanceDefinition.h"
#include "../Definitions/PhysicsDefinition.h"
#include "../../Common/Definitions/ActorDefinition.h"
#include "../../Sigma/Math/Constraint.h"
#include "../../Sigma/Random.h"
#include "../CGame.h"

// DWARF FullRagdoll.h:26; no adopted header exists yet.
class FullRagdoll : public Ragdoll {
public:
    FullRagdoll(RActorID id, int32 nDegradeRequest);
    virtual ~FullRagdoll();
private:
    FullRagdoll(const FullRagdoll &);
    void operator=(const FullRagdoll &);
};
static_assert(sizeof(FullRagdoll) == 0x3098, "FullRagdoll");
// DWARF AppearanceDefinition.h:148; shared header forward declares this type.
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

#line 26
FullRagdoll::FullRagdoll(
    RActorID id,
    int32 nDegradeRequest)
    : Ragdoll(id)
{
    m_nType = FULL_RAGDOLL_TYPE;
    m_nBoneCount = m_nLoopCount = 0;
    m_bNeedsInitialVelocity = TRUE;
#line 37
    RActor * ptRActor = RActorGet(m_idRActor);

    ScopedAppearanceDefinitionLockR pdefAppearance(ptRActor->snoAppearance);

    const Structure * ptStructure = &pdefAppearance->tStructure;
#line 44
    int32 nDegrade = nDegradeRequest;
    while (nDegrade > 0 && !(ptStructure->uRagdollDegrade & (1 << nDegrade)))
    {
        --nDegrade;
    }

    const PRSTransform & actorTransform = ptRActor->transformClient;
    // DWARF flTorqueScale (line 51) has no surviving use or initializer evidence; omitted.
#line 54
    ScopedActorDefinitionLockR pdefActor(ptRActor->snoActor);

    ScopedPhysicsDefinitionLockR pdefPhysics(ptRActor->snoPhysics);
#line 59
    RigidBodyParams params;

    params.m_materialOverride = pdefActor->eMaterial;
    params.m_angularDamping = pdefPhysics->flAngularDamping;
    params.m_linearDamping = pdefPhysics->flLinearDamping;
    params.m_scale = actorTransform.flScale;
    params.m_friction = pdefPhysics->flFriction;
    params.m_rollingResistance = pdefPhysics->flRollingResistance;
    params.m_restitution = pdefPhysics->flRestitution;
    params.m_collisionCategory = 1 << COLLISION_CATEGORY_FULL_RAGDOLL;
    params.m_collisionMask = (pdefPhysics->wCollisionMask & ~((1 << COLLISION_CATEGORY_WATER) | (1 << COLLISION_CATEGORY_SMOKE))) | (1 << COLLISION_CATEGORY_MOUSE);
    params.m_groupIndex = 0;
    params.m_bCollideWithSameActor = (pdefPhysics->dwFlags >> PHYSICSDEF_COLLIDE_WITH_SAME_ACTOR_BIT) & 1;
    params.m_bDisableSleep = (pdefPhysics->dwFlags >> PHYSICSDEF_DO_NOT_AUTOSLEEP_BIT) & 1;
    params.m_bAffectedByDisturbance = (pdefPhysics->dwFlags >> PHYSICSDEF_AFFECTED_BY_DISTURBANCE_BIT) & 1;
    params.m_nDegrade = nDegrade;

    if (params.m_bCollideWithSameActor)
    {
        params.m_groupIndex = m_nGroupIndex;
    }
#line 84
    RNG & rng = CGameGlobalsGet().rngLocal;

    const ActorAnim * ptActorAnim = ptRActor->ptActorAnim;
    for (int32 i = 0; i < ptStructure->nBoneCount && m_nBoneCount < MAX_RAGDOLL_BONES; ++i)
    {
        const BoneStructure * ptBoneStructure = &ptStructure->ptBoneStructure[i];
#line 92
        if (!ptBoneStructure->nShapeCount)
            continue;
#line 97
        ProcessedBone * ptAnimBone = &ptActorAnim->ptProcessedBoneList[i];
        PRSTransform boneTransform = actorTransform * ptAnimBone->modelTransform;
        params.m_transform = boneTransform;
        params.m_scale = boneTransform.flScale;
        params.m_bStatic = TRUE;
#line 105
        int32 nShapeCount = 0;
        float flInitialVelocityFactor = 0.0f;
        for (int32 j = 0; j < ptBoneStructure->nShapeCount; ++j)
        {
            if (ptBoneStructure->ptShapes[j].nDegrade == nDegrade)
            {
                if (ptBoneStructure->ptShapes[j].flDensityFactor > 0.0f)
                {
                    params.m_bStatic = FALSE;
                }
#line 119
                flInitialVelocityFactor += ptBoneStructure->ptShapes[j].flVelocityFactor;
                ++nShapeCount;
            }
        }
#line 125
        RagdollBone * ptRagdollBone = &m_ptBones[m_nBoneCount];
        ptRagdollBone->ptBody = gptPhysicsAPIImpl->CreateBodyFromBone(params, id, i);
        if (!ptRagdollBone->ptBody)
            continue;
#line 133
        ptRagdollBone->flInitialVelocityFactor = flInitialVelocityFactor / nShapeCount;
        ptRagdollBone->flWindPhase = RandFloat_Internal(rng) * FULL_CIRCLE;
        ptRagdollBone->nAnimIndex = i;
        m_anBoneMap[i] = m_nBoneCount;
#line 141
        ptRagdollBone->nParentIndex = -1;
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
                break;
            }
#line 155
            nAnimParent = ptStructure->ptBoneStructure[nAnimParent].nParentIndex;
        }
#line 159
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
#line 182
    const float cflInertiaFactor = 1.5f;
    const float cflMassRatio = 0.75f;
    if (m_nBoneCount > 0 && m_ptBones[0].ptBody)
    {
        m_ptBones[0].ptBody->StabilizeInertias(cflInertiaFactor, cflMassRatio);
    }
#line 190
    m_nLoopCount = Math::Min(ptStructure->nLoopConstraintCount, (int32)MAX_RAGDOLL_LOOPS);
    for (int32 i = 0; i < m_nLoopCount; ++i)
    {
        const ConstraintParameters * ptLoopParams = &ptStructure->ptLoopConstraints[i];
#line 198
        int32 nBone1 = m_anBoneMap[ptLoopParams->nBone1];
        int32 nBone2 = m_anBoneMap[ptLoopParams->nBone2];
        if (nBone1 < 0 || nBone2 < 0)
            continue;
#line 210
        RagdollBone * ptBone1 = &m_ptBones[nBone1];
        RagdollBone * ptBone2 = &m_ptBones[nBone2];
        if (ptBone1->ptBody && ptBone2->ptBody)
        {
            m_ptLoops[i] = gptPhysicsAPIImpl->CreateConstraint(ptBone1->ptBody, ptBone2->ptBody, ptLoopParams, m_idRActor);
            if (m_ptLoops[i])
            {
                m_ptLoops[i]->m_dwHash = ptLoopParams->dwHash;
            }
        }
    }
#line 231
    InstallBoneControllers();
    if (pdefPhysics->wCollisionMask & (1 << COLLISION_CATEGORY_SCENE))
    {
        RemovePenetrations();
    }
    DominoWorld::s_currentActor = SNO_NONE;
}
#line 244
FullRagdoll::~FullRagdoll()
{
}
