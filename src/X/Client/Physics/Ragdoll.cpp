#include "Ragdoll.h"
#include "DominoClient.h"
#include "../RActors/RActors.h"
#include "../RActors/RActorAnimation.h"
#include "../Definitions/AppearanceDefinition.h"
#include "../../Common/Definitions/ActorDefinition.h"
#include "DominoBody.h"
// Constructor and destruction recovered to provide the natural C1/C2 and D1/D2
// aliases needed by derived ragdolls. Other Ragdoll.cpp functions remain unadopted.
Ragdoll::Ragdoll(RActorID id)
{
    m_nType = INVALID_RAGDOLL_TYPE;
    m_bNeedsInitialVelocity = FALSE;
    m_idRActor = id;
    m_nBoneCount = 0;
    m_nLoopCount = 0;
    m_flMass = 0.0f;
    m_flLifeTime = 0.0f;
    m_bMotorsActive = TRUE;
    RActor * ptRActor = RActorGet(m_idRActor);
    DominoWorld::s_currentActor = ptRActor->snoActor;
    m_oldActorTransform = ptRActor->transformClient;
    ScopedAppearanceDefinitionLockR pdefAppearance(ptRActor->snoAppearance);
    const Structure * ptStructure = &pdefAppearance->tStructure;
    ScopedActorDefinitionLockR pdefActor(ptRActor->snoActor);
    m_flTracking = Math::Pin(1.0f - pdefActor->flPartialRagdollResponsiveness, 0.0f, 1.0f);
    m_bAnimationOverride = FALSE;
    m_bSnapToBindPose = FALSE;
    for (int32 i = 0; i < MAX_RAGDOLL_BONES; ++i)
    {
        m_ptBones[i].partialTransform.q = cgqIdentity;
        m_ptBones[i].partialTransform.wp = cgwpNull;
        m_ptBones[i].deltaTransform.q = cgqIdentity;
        m_ptBones[i].deltaTransform.wp = cgwpNull;
        m_ptBones[i].bHaveWater = m_ptBones[i].bTeleport = FALSE;
        m_ptBones[i].ptBody = NULL;
        m_ptBones[i].ptConstraint = NULL;
    }
    for (int32 i = 0; i < ptStructure->nBoneCount; ++i)
        m_anBoneMap[i] = LEAF_BONE;
    for (int32 i = 0; i < MAX_RAGDOLL_LOOPS; ++i)
        m_ptLoops[i] = NULL;
    m_nGroupIndex = gptPhysicsAPIImpl->GetNextGroupIndex();
}

Ragdoll::~Ragdoll()
{
    for (int32 i = 0; i < m_nBoneCount; ++i)
    {
        RagdollBone * ptBone = &m_ptBones[i];
        if (ptBone->ptConstraint)
        {
            gptPhysicsAPIImpl->DestroyConstraint(ptBone->ptConstraint);
            ptBone->ptConstraint = NULL;
        }
    }
    for (int32 i = 0; i < m_nLoopCount; ++i)
    {
        gptPhysicsAPIImpl->DestroyConstraint(m_ptLoops[i]);
        m_ptLoops[i] = NULL;
    }
    for (int32 i = 0; i < m_nBoneCount; ++i)
    {
        RagdollBone * ptBone = &m_ptBones[i];
        gptPhysicsAPIImpl->DestroyBody(ptBone->ptBody);
        ptBone->ptBody = NULL;
    }
    RActor * ptRActor = RActorGet(m_idRActor);
    ActorAnim * ptAnim = ptRActor->ptActorAnim;
    ScopedAppearanceDefinitionLockR pdefAppearance(ptRActor->snoAppearance);
    const Structure * ptStructure = &pdefAppearance->tStructure;
    for (int32 i = 0; i < ptStructure->nBoneCount; ++i)
    {
        if (ptAnim->ptBoneAnimControllerList[i] == this)
            ptAnim->ptBoneAnimControllerList[i] = NULL;
    }
}


























































void Ragdoll::InstallBoneControllers()

{
    RActor * ptRActor = RActorGet(m_idRActor);
    ActorAnim * ptAnim = ptRActor->ptActorAnim;
    ScopedAppearanceDefinitionLockR pdefAppearance(ptRActor->snoAppearance);
    const Structure * ptStructure = &pdefAppearance->tStructure;


    for (int32 i = 0; i < ptStructure->nBoneCount; ++i)
    {
        if (m_anBoneMap[i] > LEAF_BONE)
        {
            ptAnim->ptBoneAnimControllerList[i] = this;
        }
    }
}









void Ragdoll::InitializeVelocities(
    const PRSTransform * ptRelativeTransforms1,
    const PRSTransform * ptRelativeTransforms2,
    float flInvDeltaT,
    float flMomentumFactor)
{



    RActor * ptRActor = RActorGet(m_idRActor);

    const PRSTransform & actorTransform = ptRActor->transformClient;

    float flInvDt = flInvDeltaT * 60.0f;

    PRSTransform ptModelTransforms1[255];
    PRSTransform ptModelTransforms2[255];
    ScopedAppearanceDefinitionLockR pdefAppearance(ptRActor->snoAppearance);
    const Structure * ptStructure = &pdefAppearance->tStructure;

    Vector3D awvLinearVelocities[MAX_RAGDOLL_BONES];
    Vector3D awvAngularVelocities[MAX_RAGDOLL_BONES];

    Vector3D wvTotalMomentum = cgwvNull;
    float flTotalMass = 0.0f;


    for (int32 i = 0; i < ptStructure->nBoneCount; ++i)
    {
        if (m_anBoneMap[i] == LEAF_BONE)
            continue;



        int32 nParentIndex = ptStructure->ptBoneStructure[i].nParentIndex;

        if (nParentIndex == INTERMEDIATE_BONE)
        {

            ptModelTransforms1[i] = ptRelativeTransforms1[i];
            ptModelTransforms2[i] = ptRelativeTransforms2[i];
        }
        else
        {



            ptModelTransforms1[i] = ptModelTransforms1[nParentIndex] * ptRelativeTransforms1[i];
            ptModelTransforms2[i] = ptModelTransforms2[nParentIndex] * ptRelativeTransforms2[i];
        }

        int32 nRagdollBoneIndex = m_anBoneMap[i];
        if (nRagdollBoneIndex < 0)
            continue;





        RagdollBone * ptBone = &m_ptBones[nRagdollBoneIndex];
        RigidBody * ptBody = ptBone->ptBody;
        if (ptBody == NULL)
            continue;




        PRSTransform worldTransform1 = actorTransform * ptModelTransforms1[i];
        PRSTransform worldTransform2 = actorTransform * ptModelTransforms2[i];

        Vector3D com = ptBody->GetCenterOfMassLocal();
        Quaternion q1 = worldTransform1.q;
        Vector3D p1 = worldTransform1.wp + q1.Rotate(com);

        Quaternion q2 = worldTransform2.q;
        Vector3D p2 = worldTransform2.wp + q2.Rotate(com);

        Vector3D v = (flInvDt * ptBone->flInitialVelocityFactor) * (p2 - p1);

        awvLinearVelocities[nRagdollBoneIndex] = v;


        if (Quaternion::DotProduct(q1, q2) < 0.0f)
        {
            q2.wdReal = -q2.wdReal;
            q2.wvImag = -q2.wvImag;
        }
        Quaternion dqdt = flInvDt * (q2 - q1);
        Quaternion qw = (2.0f * dqdt) * Quaternion(Vector3D(-q1.wvImag.x, -q1.wvImag.y, -q1.wvImag.z), q1.wdReal);
        Vector3D omega = ptBone->flInitialVelocityFactor * qw.wvImag;

        awvAngularVelocities[nRagdollBoneIndex] = omega;

        ptBone->wpInitialCOMPosition = p1;

        float flMass = ptBody->GetMass();
        flTotalMass += flMass;
        wvTotalMomentum += flMass * v;
    }


    Vector3D wvDeltaV = cgwvNull;
    if (flMomentumFactor != 1.0f && flTotalMass > 0.0f)
    {
        Vector3D wvTotalImpulse = (flMomentumFactor - 1.0f) * wvTotalMomentum;
        wvDeltaV = (1.0f / flTotalMass) * wvTotalImpulse;
    }


    for (int32 i = 0; i < ptStructure->nBoneCount; ++i)
    {
        int32 nRagdollBoneIndex = m_anBoneMap[i];
        if (nRagdollBoneIndex < 0)
            continue;






        RagdollBone * ptBone = &m_ptBones[nRagdollBoneIndex];
        RigidBody * ptBody = ptBone->ptBody;
        if (ptBody == NULL)
            continue;

        Vector3D v = ptBody->GetCenterOfMassVelocity();
        v += wvDeltaV + awvLinearVelocities[nRagdollBoneIndex];
        ptBody->SetCenterOfMassVelocity(v);

        Vector3D omega = ptBody->GetAngularVelocity();
        omega += awvAngularVelocities[nRagdollBoneIndex];
        ptBody->SetAngularVelocity(omega);

        ptBone->wvInitialLinearVelocity = v;
        ptBone->wvInitialAngularVelocity = omega;
    }


    m_bNeedsInitialVelocity = FALSE;
}



void Ragdoll::ApplyExplosion(
    const ExplosionImpulse & tExplosion,
    float flExplosionFactor)
{



    const float cflMaxPartialRagdollVelocity = 20.0f;
    const float cflMaxRagdollVelocity = 50.0f;

    float flMaxDeltaVelocity = (m_nType == PARTIAL_RAGDOLL_TYPE) ? cflMaxPartialRagdollVelocity : cflMaxRagdollVelocity;

    for (int32 i = 0; i < m_nBoneCount; ++i)
    {
        RagdollBone * ptBone = &m_ptBones[i];
        if (ptBone->ptBody != NULL)
        {
            ptBone->ptBody->ApplyExplosion(tExplosion, flExplosionFactor, flMaxDeltaVelocity);
        }
    }
}



// Inferred: the NULL test as its own if + continue (cleanup-destination value 4 in the original loop; columns 336:13/21 and 341:35 put the two tests on separate lines at the same depth).
void Ragdoll::DisableConstraint(
    uint32 dwHash)
{
    for (int32 i = 0; i < m_nBoneCount; ++i)
    {
        RagdollBone * ptBone = &m_ptBones[i];
        if (ptBone->ptConstraint == NULL)
        {
            continue;
        }

        if (ptBone->ptConstraint->m_dwHash == dwHash)
        {
            gptPhysicsAPIImpl->DestroyConstraint(ptBone->ptConstraint);
            ptBone->ptConstraint = NULL;
            return;
        }
    }

    for (int32 i = 0; i < m_nLoopCount; ++i)
    {
        if (m_ptLoops[i] != NULL && m_ptLoops[i]->m_dwHash == dwHash)
        {
            gptPhysicsAPIImpl->DestroyConstraint(m_ptLoops[i]);
            m_ptLoops[i] = NULL;
            return;
        }
    }
}


// Inferred: the NULL test as its own if + continue (cleanup-destination value 4 in the original loop; columns 368:13/21 and 373:35/44 put the two tests on separate lines at the same depth).
Constraint * Ragdoll::GetConstraint(
    uint32 dwHash)
{
    for (int32 i = 0; i < m_nBoneCount; ++i)
    {
        RagdollBone * ptBone = &m_ptBones[i];
        if (ptBone->ptConstraint == NULL)
        {
            continue;
        }

        if (ptBone->ptConstraint->m_dwHash == dwHash)
        {
            return ptBone->ptConstraint;
        }
    }

    for (int32 i = 0; i < m_nLoopCount; ++i)
    {
        if (m_ptLoops[i] != NULL && m_ptLoops[i]->m_dwHash == dwHash)
            return m_ptLoops[i];
    }
    return NULL;
}




#include "Buoyancy.h"
#include "../CGame.h"
#include "../../Sigma/Random.h"
#include "../../Sigma/Math/MathGeneral.h"
BOOL GetWaterPlane(WaterPlane * ptWaterPlane, const Vector3D & wp);  // X/Client/Physics/Buoyancy.cpp:112
void ApplyBuoyancy(RigidBody * pRigidBody, const WaterPlane & tWaterPlane);  // X/Client/Physics/Buoyancy.cpp:139
#include "../Wind.h"
#include "../Definitions/PhysicsDefinition.h"



























































void Ragdoll::Update(
    BOOL bIsAnimating,
    float flDeltaTime,
    const ActorDefinition * pdefActor)
{
    if (!ShouldAnimate())
        return;





    if (!bIsAnimating && m_bMotorsActive == TRUE)
    {
        for (int32 i = 0; i < m_nBoneCount; ++i)
        {
            RagdollBone * ptRagdollBone = &m_ptBones[i];
            if (ptRagdollBone->ptConstraint != NULL && ptRagdollBone->ptBody != NULL)
            {
                ptRagdollBone->ptBody->SetSleeping(FALSE);
                ptRagdollBone->ptConstraint->ActivateMotor(FALSE);
            }
        }

        m_bMotorsActive = FALSE;
    }

    RActor * ptRActor = RActorGet(m_idRActor);
    BOOL fLocalLock = pdefActor == NULL;
    if (fLocalLock)
    {
        pdefActor = (const ActorDefinition *)gptGroupDefActor->LockR(ptRActor->snoActor, FALSE);
    }






    ScopedPhysicsDefinitionLockR pdefPhysics(ptRActor->snoPhysics);

    const CGame & tClientData = CGameGet();

    if (tClientData.bUpdateRigidBodyPhysics && ((pdefPhysics->dwFlags >> PHYSICSDEF_AFFECTED_BY_WIND_BIT) & 1))
    {
        WindState & tWindStateGlobal = Wind::GetGlobalWindState();
        for (int32 i = 0; i < m_nBoneCount; ++i)
        {
            RagdollBone * ptRagdollBone = &m_ptBones[i];
            if (ptRagdollBone->ptBody != NULL)
            {



                Vector3D wvWind = tWindStateGlobal.GetPrevailing(ptRagdollBone->flWindPhase);

                Vector3D wvWindDir(wvWind);
                float flWindSpeed = wvWindDir.Normalize();

                const float cflFudgeFactor = 1.0f;
                const float cflAirDensity = 0.001215f;
                const float cflPressureFactor = cflFudgeFactor * cflAirDensity;

                Vector3D wvWindImpulse = (pdefActor->flWindFactor * cflPressureFactor * (flWindSpeed * flWindSpeed)) * wvWindDir;
                ptRagdollBone->ptBody->ApplyWind(wvWindImpulse);
            }
        }
    }
    if (tClientData.bUpdateRigidBodyPhysics && ((pdefPhysics->dwFlags >> PHYSICSDEF_AFFECTED_BY_BUOYANCY_BIT) & 1))
    {
        UpdateBuoyancy();
    }

    if (tClientData.bUpdateRigidBodyPhysics)
    {
        float flAcceleration = pdefPhysics->flAirBuoyancy * 60.0f * 60.0f;
        if (flAcceleration != 0.0f)
            for (int32 i = 0; i < m_nBoneCount; ++i)
            {
                RagdollBone * ptRagdollBone = &m_ptBones[i];
                if (ptRagdollBone->ptBody != NULL)
                {



                    float flMass = ptRagdollBone->ptBody->GetMass();
                    Vector3D wvF(0.0f, 0.0f, flAcceleration * flMass);
                    ptRagdollBone->ptBody->ApplyForce(wvF);
                }
            }
    }


    m_flLifeTime += flDeltaTime;
    if (m_nType == FULL_RAGDOLL_TYPE && m_flLifeTime == 0.25f)
    {
        ptRActor->dwFlags |= (1 << RACTOR_GOT_MELEE_EXPLOSION_BIT);
    }

    if (fLocalLock)
    {
        gptGroupDefActor->UnlockR((void *&)pdefActor);
    }
}




void Ragdoll::ComputeBone(
    RActor * ptRActor,
    PRSTransform & boneTransform,
    const PRSTransform & parentTransform,
    const PRSTransform & relativeTransform,
    const PRSTransform & defaultRelativeTransform,
    int32 nBoneIndex,
    float flDeltaTime)
{
    int32 nRagdollBoneIndex = m_anBoneMap[nBoneIndex];


    if (m_bAnimationOverride == TRUE)
    {
        boneTransform = parentTransform * relativeTransform;
        return;
    }

    if (nRagdollBoneIndex == INTERMEDIATE_BONE)
    {

        boneTransform = parentTransform * defaultRelativeTransform;
        return;
    }


    RagdollBone * ptRagdollBone = &m_ptBones[nRagdollBoneIndex];


    if (ptRagdollBone->ptBody == NULL)
    {




        if (ptRagdollBone->nParentIndex == -1)
        {
            // no row of its own: the -1 is the loaded nParentIndex (w8) reused by the shared str at +0xda4 (line 0)
            ptRagdollBone->nKinematicRoot = -1;
        }
        else
            ptRagdollBone->nKinematicRoot = m_ptBones[ptRagdollBone->nParentIndex].nKinematicRoot;

        // no row of its own: tail-merged into line 586's copy (+0xda4 falls through to +0xda8, loads from x5 = defaultRelativeTransform)
        boneTransform = parentTransform * defaultRelativeTransform;
        return;
    }


    const PRSTransform & actorTransform = ptRActor->transformClient;
    PRSTransform invActorTransform = actorTransform.Inverse();

    if (ptRagdollBone->ptBody->GetType() == RigidBody::KINEMATIC)
    {
        RigidBody * ptBody = ptRagdollBone->ptBody;


        ptRagdollBone->nKinematicRoot = nRagdollBoneIndex;


        boneTransform = parentTransform * relativeTransform;


        PRTransform transform1;
        ptBody->GetTransform(transform1);

        PRTransform transform2;
        transform2.q = actorTransform.q * boneTransform.q;
        transform2.wp = actorTransform.wp + actorTransform.flScale * actorTransform.q.Rotate(boneTransform.wp);

        ptRagdollBone->deltaTransform = transform2 * transform1.Inverse();

        Quaternion q1 = transform1.q;
        Quaternion q2 = transform2.q;
        if (Quaternion::DotProduct(q1, q2) < 0.0f)
        {
            q2.wvImag = -q2.wvImag;
            q2.wdReal = -q2.wdReal;
        }

        Vector3D wvLocalCenter = ptBody->GetCenterOfMassLocal();
        Vector3D wpCenter1 = transform1.wp + q1.Rotate(wvLocalCenter);
        Vector3D wpCenter2 = transform2.wp + q2.Rotate(wvLocalCenter);

        Vector3D wvTranslation = wpCenter2 - wpCenter1;
        Vector3D wvRotation = Quaternion::AngularVelocity(q1, q2, 1.0f);


        float flAlphaLinear = 0.0f, flAlphaAngular = 0.0f;
        if (wvRotation.MagnitudeSquared() > QUARTER_CIRCLE * QUARTER_CIRCLE)
        {
            float flLength = wvRotation.Magnitude();
            flAlphaAngular = (flLength - QUARTER_CIRCLE) / flLength;
        }

        if (wvTranslation.MagnitudeSquared() > 4.0f)
        {
            float flLength = wvTranslation.Magnitude();
            flAlphaLinear = (flLength - 2.0f) / flLength;
        }

        float flAlpha = Math::Max(flAlphaAngular, flAlphaLinear);


        flAlpha = Math::Max(flAlpha, m_flTracking);

        if (m_flLifeTime <= 4.0f || m_bSnapToBindPose)
        {
            // no row of its own: the fmov #1.0 at +0x934 (line 0) is the then-branch of 671
            flAlpha = 1.0f;
        }


        Vector3D wpCenter1a = wpCenter1 + flAlpha * wvTranslation;
        Quaternion q1a = flAlpha * (q2 - q1) + q1;
        if (Quaternion::DotProduct(q1a, q1a) > 0.0f)
        {
            q1a.Normalize();
        }
        else
        {
            // no row of its own: on the b.le path (+0x9b8 -> +0xa14) lines 689/690 read q1 from w20-w24 (loaded at 637), not q1a
            q1a = q1;
        }
        Quaternion qDelta = q1a * q1.Conjugate();
        Vector3D wp1a = wpCenter1a - q1a.Rotate(wvLocalCenter);

        PRTransform transform1a;
        transform1a.q = q1a;
        transform1a.wp = wp1a;
        ptBody->SetTransformNoSync(transform1a);


        ptRagdollBone->partialTransform.q = qDelta;
        ptRagdollBone->partialTransform.wp = wp1a - qDelta.Rotate(transform1.wp);
        ptRagdollBone->bTeleport = TRUE;


        Vector3D wvAngularVelocity = Quaternion::AngularVelocity(q1a, q2, 1.0f / flDeltaTime);
        Vector3D wvLinearVelocity = (1.0f / flDeltaTime) * (wpCenter2 - wpCenter1a);

        ptBody->SetAngularVelocity(wvAngularVelocity);
        ptBody->SetCenterOfMassVelocity(wvLinearVelocity);
        // no row of its own: the b to the epilogue at +0xd78 carries line 707 (as B01's returns at 580/587 have no row)
        return;
    }



    if (ptRagdollBone->nParentIndex != -1 && ptRagdollBone->ptConstraint != NULL)
    {







        ptRagdollBone->nKinematicRoot = m_ptBones[ptRagdollBone->nParentIndex].nKinematicRoot;
    }
    else
        // no row of its own: the -1 store at +0x1344/+0x1348 is line 0 (merged with 723's store, then jump-threaded into the 769 path)
        ptRagdollBone->nKinematicRoot = -1;

    int32 nRoot = ptRagdollBone->nKinematicRoot;


    if (nRoot >= 0 && nRoot < m_nBoneCount)
    {

        if (m_bSnapToBindPose)
        {
            boneTransform = parentTransform * relativeTransform;

            PRTransform transform;
            transform.q = actorTransform.q * boneTransform.q;
            transform.wp = actorTransform.wp + actorTransform.flScale * actorTransform.q.Rotate(boneTransform.wp);
            ptRagdollBone->ptBody->SetTransformNoSync(transform);
            ptRagdollBone->ptBody->SetAngularVelocity(cgwvNull);
            ptRagdollBone->ptBody->SetCenterOfMassVelocity(cgwvNull);
        }
        else
        {
            PRTransform bodyTransform;
            ptRagdollBone->ptBody->GetTransform(bodyTransform);


            PRTransform renderTransform = m_ptBones[nRoot].deltaTransform * bodyTransform;

            float flScale = boneTransform.flScale;
            boneTransform = invActorTransform * renderTransform;
            boneTransform.q.Normalize();
            boneTransform.flScale = flScale;

            if (m_ptBones[nRoot].bTeleport)
            {
                bodyTransform = m_ptBones[nRoot].partialTransform * bodyTransform;
                bodyTransform.q.Normalize();
                ptRagdollBone->ptBody->SetTransformNoSync(bodyTransform);
            }
        }
    }
    else
    {
        PRTransform bodyTransform;
        ptRagdollBone->ptBody->GetTransform(bodyTransform);


        if (bodyTransform.wp.z < -1000.0f)
        {
            bodyTransform.wp.z = -1000.0f;
            ptRagdollBone->ptBody->SetTransformNoSync(bodyTransform);
            ptRagdollBone->ptBody->SetSleeping(TRUE);
        }


        float flScale = boneTransform.flScale;
        boneTransform = invActorTransform * bodyTransform;
        boneTransform.q.Normalize();
        boneTransform.flScale = flScale;
    }


    if (ptRagdollBone->ptConstraint && m_bMotorsActive)
    {
        PRTransform motorTransform(relativeTransform.q, relativeTransform.wp);
        motorTransform.wp *= actorTransform.flScale;
        ptRagdollBone->ptConstraint->UpdateMotor(motorTransform);
    }
}



void Ragdoll::PostCompute()
{

    RActor * ptRActor = RActorGet(m_idRActor);
    m_oldActorTransform = RActorGetClientTransform(ptRActor);
    m_bSnapToBindPose = FALSE;
}






































// Inference (RemovePenetrations): input.uIncludeMask = 1 and uExcludeMask = 0 are written at 892-893 because the original stores 1 as one word at input+0x18 and nothing else sets those fields; no other source for the values was found.
void Ragdoll::RemovePenetrations()
{

    const int32 cnMaxIterations = 10;

    if (m_nBoneCount == 0)
        return;



    if (m_ptBones[0].ptBody == NULL)
        return;




    RigidBody::Type eType = m_ptBones[0].ptBody->GetType();
    if (eType == RigidBody::KINEMATIC || eType == RigidBody::STATIC)
        return;


// Inference: the early returns at 849, 854 and 861 are written from the branch structure of the instructions at 3e42fc..3e4318; they have no other evidence.


    for (int32 nIter = 0; nIter < cnMaxIterations; ++nIter)
    {
        BOOL bHadShift = FALSE;


        for (int32 i = 1; i < m_nBoneCount; ++i)
        {
            const RagdollBone * ptBone = &m_ptBones[i];
            if (ptBone->ptBody == NULL || ptBone->nParentIndex == -1)
                continue;



            const RagdollBone * ptParentBone = &m_ptBones[ptBone->nParentIndex];
            if (ptParentBone->ptBody == NULL)
                continue;




            RayCastInput input;
            RayCastOutput output;
            input.wpFrom = ptParentBone->ptBody->GetCenterOfMassPosition();
            input.wpTo = ptBone->ptBody->GetCenterOfMassPosition();
            input.uIncludeMask = 1;
            input.uExcludeMask = 0;
            BOOL bHit = gptPhysicsAPIImpl->RayCast(&output, &input);


            if (bHit)
            {
                Vector3D wvShift = output.wpPoint + ptBone->ptBody->m_boundingRadius * output.wvNormal - input.wpTo;


                for (int32 j = 0; j < m_nBoneCount; ++j)
                {
                    const RagdollBone * ptBonej = &m_ptBones[j];
                    if (ptBonej->ptBody != NULL)
                    {



                        Vector3D wp = ptBonej->ptBody->GetPosition();
                        wp += wvShift;
                        ptBonej->ptBody->SetPosition(wp);
                    }
                }
                bHadShift = TRUE;
            }
        }

        if (!bHadShift)
            break;
    }

}


void Ragdoll::UpdateBuoyancy()
{

    CGameGlobals & tCGameGlobals = CGameGlobalsGet();

    float t = m_flLifeTime;
    float fHz = 0.25f;
    float fAmplitude = 0.1f;
    float fOmega = PI * 2.0f * fHz;

    for (int32 i = 0; i < m_nBoneCount; ++i)
    {
        RagdollBone * ptRagdollBone = &m_ptBones[i];

        if (i != 0 && ptRagdollBone->ptConstraint != NULL)
        {

            RagdollBone * ptParent = &m_ptBones[ptRagdollBone->nParentIndex];
            ptRagdollBone->waterPlane = ptParent->waterPlane;
            ptRagdollBone->wvWaterVelocity = ptParent->wvWaterVelocity;
            ptRagdollBone->bHaveWater = ptParent->bHaveWater;
        }
        else
        {



            const uint32 cdwChance = 15;

            uint32 dwRandom = RandGetDWord(tCGameGlobals.rngLocal);
            if (ptRagdollBone->ptBody != NULL && (dwRandom & cdwChance) == 0)
            {
                WaterPlane tWaterPlane;
                Vector3D wpCOM = ptRagdollBone->ptBody->GetCenterOfMassPosition();
                ptRagdollBone->bHaveWater = GetWaterPlane(&tWaterPlane, wpCOM);

                if (ptRagdollBone->bHaveWater)
                {
                    ptRagdollBone->waterPlane = tWaterPlane.tPlane;
                    ptRagdollBone->wvWaterVelocity = tWaterPlane.wvVelocity;
                }
            }
        }

        if (ptRagdollBone->ptBody != NULL && ptRagdollBone->bHaveWater)
        {
            Vector3D center = ptRagdollBone->ptBody->GetCenterOfMassPosition();

            float fWave = fAmplitude * Math::Sin(fOmega * (t + 0.1f * center.x + 0.1f * center.y));
            // Inferred from columns 974:29/48/53/60/71 (amplitude multiply on 974) and 978:27 (+=).
            WaterPlane tWaterPlane;
            tWaterPlane.tPlane = ptRagdollBone->waterPlane;
            tWaterPlane.tPlane.wdD += fWave;
            tWaterPlane.wvVelocity = ptRagdollBone->wvWaterVelocity;
            ApplyBuoyancy(ptRagdollBone->ptBody, tWaterPlane);
        }
    }
}
