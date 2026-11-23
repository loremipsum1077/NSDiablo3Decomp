#include "LookAt.h"
#include "RActors/RActors.h"
#include "RActors/RActorAnimation.h"
#include "Definitions/AppearanceDefinition.h"
#include "../Common/ActorCommonData.h"
#include <math.h>

static const float scgflBlendSeconds = 0.2f;
#line 26 "X/Client/LookAt.cpp"
LookAt::LookAt(RActorID idRActor)
    : m_idRActor(idRActor), m_idTargetRActor(RACTORID_INVALID), m_flYaw(0.0f), m_flPitch(0.0f)
{
    RActor* ptRActor = RActorGet(idRActor);
    ScopedAppearanceDefinitionLockR pdefAppearance(ptRActor->snoAppearance);
    const LookAtData* ptData = &pdefAppearance->tStructure.tLookAtData;
    const BoneStructure* ptBoneStuctures = pdefAppearance->tStructure.ptBoneStructure;
    m_nBoneIndex = -1;
    for (int32 i = 0; i < pdefAppearance->tStructure.nBoneCount; ++i) {
        const BoneStructure* ptBoneStructure = &ptBoneStuctures[i];
        if (ptData->dwBoneHash == ptBoneStructure->dwHash) {
            m_nBoneIndex = i;
            break;
        }
    }
    m_flRange = ptData->flRange;
    m_flYawMax = ptData->aAngleLimitYaw;
    m_flSpeed = ptData->aSpeed * 60.0f;
    m_flPitchMin = ptData->aPitchMin;
    m_flPitchMax = ptData->aPitchMax;
    m_flPitchOffset = ptData->aPitchOffset;
    m_flUnitBlend = 0.0f;
    if (m_nBoneIndex != -1)
        ptRActor->ptActorAnim->ptBoneAnimControllerList[m_nBoneIndex] = this;
}
#line 81 "X/Client/LookAt.cpp"
LookAt::~LookAt()
{
    if (m_nBoneIndex != -1) {
        RActor* ptRActor = RActorGet(m_idRActor);
        ActorAnim* ptAnim = ptRActor->ptActorAnim;
        ptAnim->ptBoneAnimControllerList[m_nBoneIndex] = 0;
    }
}
#line 95 "X/Client/LookAt.cpp"
float LookAt::SeekToTarget(
    float flCurrent,
    float flTarget,
    float flSpeed)
{
    float flResult;
    if (flTarget < flCurrent)
    {
        flResult = Math::Max(flCurrent - flSpeed, flTarget);
    }
    else
    {
        flResult = Math::Min(flCurrent + flSpeed, flTarget);
    }
    return flResult;
}








void LookAt::ComputeBone(
    RActor* ptRActor,
    PRSTransform& transform,
    const PRSTransform& parentTransform,
    const PRSTransform& relativeTransform,
    const PRSTransform& defaultRelativeTransform,
    int32 nBoneIndex,
    float flDeltaTime)
{

    Vector3D vTarget;
    BOOL bUseTarget = FALSE;

































    RActor* ptTarget = RActorTryToGet(m_idTargetRActor);
    if (ptTarget)
    {


        const ActorLocation* ptALoc = 0;
        if (ptTarget->idACD != ACDID_INVALID)
        {
            ActorCommonData* ptACDTarget = ACDTryToGet(ptTarget->idACD);
            if (ptACDTarget)
            {
                ptALoc = &ptACDTarget->tLocation;
            }
        }

        if (!ptALoc)
        {
            ptALoc = &ptTarget->tLocation;
        }

        if (ptALoc->eActorLocation == 0)
        {
            vTarget = ptALoc->tWorldLocation.transform.wp;

            float flHeightOffset = ptALoc->tWorldLocation.wcylBounds.wdHeight;
            flHeightOffset *= 0.8f;

            vTarget.z += flHeightOffset;
            bUseTarget = TRUE;
        }
    }


    float flTargetYaw = 0.0f;
    float flTargetPitch = 0.0f;
    float flUnitBlendTarget = 0.0f;
    if (bUseTarget)
    {
        const PRSTransform& actorTransform = ptRActor->transformClient;
        PRSTransform transformBone = parentTransform * defaultRelativeTransform;
        transformBone = actorTransform * transformBone;











        Vector3D wvForward = actorTransform.Rotate(cgwvXAxis);
        Vector3D vToTarget = vTarget - transformBone.wp;
        float flToTargetZ = vToTarget.z;
        vToTarget.z = 0.0f;
        if (vToTarget.MagnitudeSquared() >= 1e-12f)
        {
            float flLength = vToTarget.Normalize();
            if (flLength < m_flRange)
            {
                float flCosine = Vector3D::DotProduct(wvForward, vToTarget);
                float flSine = Vector3D::CrossProduct(wvForward, vToTarget).z;


                flTargetYaw = atan2f(flSine, flCosine);
                flTargetYaw = Math::Pin(flTargetYaw, -m_flYawMax, m_flYawMax);

                flTargetPitch = -atan2f(flToTargetZ, flLength);
                flTargetPitch += m_flPitchOffset;
                flTargetPitch = Math::Pin(flTargetPitch, m_flPitchMin, m_flPitchMax);
                flUnitBlendTarget = 1.0f;
            }
        }
    }

    m_flYaw = SeekToTarget(m_flYaw, flTargetYaw, m_flSpeed * flDeltaTime);
    m_flPitch = SeekToTarget(m_flPitch, flTargetPitch, m_flSpeed * flDeltaTime);
    m_flUnitBlend = SeekToTarget(m_flUnitBlend, flUnitBlendTarget, flDeltaTime * scgflBlendSeconds);

    if (m_flUnitBlend > 1e-6f)
    {
        PRSTransform lookTransform;
        ComputeRelativeTransform(lookTransform);

        PRSTransform blendedTransform;
        PRSTransform::Lerp(relativeTransform, lookTransform, m_flUnitBlend, blendedTransform);

        transform = parentTransform * blendedTransform;
    }
    else
    {
        transform = parentTransform * relativeTransform;
    }
}

void LookAt::ComputeRelativeTransform(
    PRSTransform& relativeTransform)
{
    RActor* ptRActor = RActorGet(m_idRActor);
    float flHalfAngle = m_flYaw * 0.5f;
    float flHalfPitch = m_flPitch * 0.5f;
    Quaternion qNeckYaw(sinf(flHalfAngle) * cgwvZAxis, cosf(flHalfAngle));
    Quaternion qNeckPitch(sinf(flHalfPitch) * cgwvYAxis, cosf(flHalfPitch));
    Quaternion qNeck = qNeckPitch * qNeckYaw;

    ScopedAppearanceDefinitionLockR pdefAppearance(ptRActor->snoAppearance);

    const BoneStructure* ptBoneStucture = &pdefAppearance->tStructure.ptBoneStructure[m_nBoneIndex];
    relativeTransform.Set(qNeck, ptBoneStucture->transformParentRel.wp, ptBoneStucture->transformParentRel.flScale);
}
