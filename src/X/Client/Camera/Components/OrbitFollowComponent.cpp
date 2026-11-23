#include "X/Client/Camera/Components/OrbitFollowComponent.h"
#include "X/Common/WorldPlace.h"


void OrbitFollowComponent::SetOrbitPlace(const WorldPlace & wp)
{
    m_wpToFollow = wp;
    m_flTransitionTimeTotal = 0.0f;
    m_flTransitionTimeLeft = 0.0f;
    m_flCurBlendWeight = 1.0f;
}

#include "X/Client/Camera/CameraLocation.h"
#include "X/Common/Definitions/ObserverDefinition.h"
#include <math.h>

#line 16
OrbitFollowComponent::OrbitFollowComponent()
    : CameraComponent(),
      m_flOrbitSLerpScalar(1.0f),
      m_flTransitionTimeTotal(0.0f),
      m_flTransitionTimeLeft(0.0f),
      m_flCurBlendWeight(0.0f),
      m_flCosLimitingAngle(1.0f),
      m_snoTransitionObserver(-1)
{
    m_wpToFollow = cgplaceNull;
    bForceAllPlayersForCentroid = TRUE;
}
OrbitFollowComponent::~OrbitFollowComponent()
{
}
CameraComponentType OrbitFollowComponent::GetCameraComponentType()
{
    return ORBIT_FOLLOW_COMPONENT;
}
void OrbitFollowComponent::EnableExitTransition()
{
    m_flCurBlendWeight = 1.0f;
    m_snoTransitionObserver = snoObserver;
    ScopedObserverDefinitionLockR pdefObserver(snoObserver);
    m_flTransitionTimeTotal = pdefObserver->flConsoleOrbitTransitionDuration;
    m_flTransitionTimeLeft = pdefObserver->flConsoleOrbitTransitionDuration;
}
void OrbitFollowComponent::SetObserver(SNO snoObs)
{
    CameraComponent::SetObserver(snoObs);
    ScopedObserverDefinitionLockR pdefObserver(snoObs);
    m_flCosLimitingAngle = cosf(Math::DegToAngle(pdefObserver->flConsoleOrbitMaxAngleFromOffsetCenter));
}
#line 142
static const float ORBIT_CLOSEST_DISTANCE_PIN = 32.0f;

void OrbitFollowComponent::Update(
    const CameraState &csLastFrame,
    CameraState &csThisFrame,
    float flDeltaSec)
{




    CameraComponent::Update(csLastFrame, csThisFrame, flDeltaSec);

    if (m_wpToFollow.idSWorld == SWORLDID_INVALID ||
        bDisabled)
    {
        return;
    }

    SNO snoObserverToUse;
    if (m_snoTransitionObserver == -1)
    {
        snoObserverToUse = snoObserver;
    }
    else
    {
        snoObserverToUse = m_snoTransitionObserver;
    }
    ScopedObserverDefinitionLockR pdefObserver(snoObserverToUse);

    if (m_flTransitionTimeTotal > 0.000001f)
    {
        m_flTransitionTimeLeft -= flDeltaSec;
        float flUnitTimeLeft = m_flTransitionTimeLeft / m_flTransitionTimeTotal;








        m_flCurBlendWeight = Math::Pow(2.7182817f, pdefObserver->flConsoleOrbitTransitionExponentialScalar * (1.0f - flUnitTimeLeft));

        if (m_flTransitionTimeLeft < 0.000001f)
        {
            m_flCurBlendWeight = 0.0f;
            m_flTransitionTimeTotal = 0.0f;
            m_flTransitionTimeLeft = 0.0f;
            m_wpToFollow = cgplaceNull;
            m_snoTransitionObserver = -1;
            return;
        }
    }

    m_flOrbitSLerpScalar = pdefObserver->flConsoleOrbitSLerpScalar;

    PRTransform tStartTransform = csThisFrame.transform;
    Vector3D wpStartLookAt = csThisFrame.wvLookAt;
    Vector3D wpStartLookFrom = csThisFrame.wvLookFrom;

    Vector3D wvTargetCameraOffsetVector = pdefObserver->wvCameraOffsetVector;



    float flCameraFixedZ = wvTargetCameraOffsetVector.z;
    wvTargetCameraOffsetVector.z = 0.0f;

    Vector3D vDefaultCamCoordinateSpace(1.0f, 1.0f, 0.0f);
    vDefaultCamCoordinateSpace.Normalize();

    Vector3D vCurCamPos;




    WorldPlace wpCentroidCenter = GetCentroidLocation();


    float flMaxAngleFromOffsetCenter = pdefObserver->flConsoleOrbitMaxAngleFromOffsetCenter;
    Vector3D wvOffsetCenterForAngleLimiting = pdefObserver->wvConsoleOrbitOffsetCenterForAngleLimiting;
    Vector3D wpCentreLocation = wvOffsetCenterForAngleLimiting + m_wpToFollow.wp;

    Vector3D vFollowToCentre = wpCentreLocation - m_wpToFollow.wp;
    Vector3D vFollowToCentroid = wpCentroidCenter.wp - m_wpToFollow.wp;

    float flDistToCentre = Vector3D::Distance(wpCentroidCenter.wp, m_wpToFollow.wp);
    if (flDistToCentre <= ORBIT_CLOSEST_DISTANCE_PIN)
    {
        Vector3D vFollowToCentroidNormal = vFollowToCentroid;
        vFollowToCentroidNormal.Normalize();
        vFollowToCentroidNormal *= ORBIT_CLOSEST_DISTANCE_PIN;
        wpCentroidCenter.wp = vFollowToCentroidNormal + m_wpToFollow.wp;
        vFollowToCentroid = vFollowToCentroidNormal;
    }
    if (vFollowToCentre.MagnitudeSquared() > 0.000001f)
    {
        vFollowToCentre.Normalize();
    }

    float flDistToCentroid = 0.0f;
    if (vFollowToCentroid.MagnitudeSquared() > 0.000001f)
    {
        flDistToCentroid = vFollowToCentroid.Normalize();
    }

    float flBetweenCentroidCenter = Vector3D::DotProduct(vFollowToCentre, vFollowToCentroid);
    if (flBetweenCentroidCenter < m_flCosLimitingAngle && flMaxAngleFromOffsetCenter > 0.000001f)
    {

        Vector3D vCrossCentroidCenter = Vector3D::CrossProduct(vFollowToCentre, vFollowToCentroid);
        if (vCrossCentroidCenter.z > 0.0f)
        {
            Vector3D vRightBounds = vFollowToCentre;
            Vector3D::Rotate(cgwvZAxis, Math::DegToAngle(flMaxAngleFromOffsetCenter), vRightBounds);

            vRightBounds *= flDistToCentroid;
            wpCentroidCenter.wp = vRightBounds + m_wpToFollow.wp;
        }
        else
        {
            Vector3D vLeftBounds = vFollowToCentre;
            Vector3D::Rotate(cgwvZAxis, Math::DegToAngle(-flMaxAngleFromOffsetCenter), vLeftBounds);

            vLeftBounds *= flDistToCentroid;
            wpCentroidCenter.wp = vLeftBounds + m_wpToFollow.wp;
        }
    }

    Vector3D vFromBossToCentroid = wpCentroidCenter.wp - m_wpToFollow.wp;
    vFromBossToCentroid.z = 0.0f;
    vFromBossToCentroid.Normalize();



    Quaternion quatRot;
    quatRot.FromChangeOfBasis(vDefaultCamCoordinateSpace, vFromBossToCentroid);
    vCurCamPos = quatRot.Rotate(wvTargetCameraOffsetVector);
    vCurCamPos.z = flCameraFixedZ;



    csThisFrame.transform.Set(vCurCamPos + wpCentroidCenter.wp, pdefObserver->wvCameraOffsetVector + wpCentroidCenter.wp, cgwvZAxis);


    PRTransform tLerpTransform;
    PRTransform::SLerp(tStartTransform, csThisFrame.transform, m_flOrbitSLerpScalar * flDeltaSec, tLerpTransform);



    Vector3D wpLookAtLerp;
    Vector3D wpLookAtCur = m_wpToFollow.wp + pdefObserver->wvLookAtOffsetVector;
    Math::Lerp(csThisFrame.wvLookAt, wpLookAtCur, m_flOrbitSLerpScalar * flDeltaSec, wpLookAtLerp);


    if (m_flCurBlendWeight < 1.0f)
    {

        Vector3D wpWeightedLookFrom = csThisFrame.transform.wp * m_flCurBlendWeight + wpStartLookFrom * (1.0f - m_flCurBlendWeight);
        Vector3D wpWeightedLookAt = wpLookAtCur * m_flCurBlendWeight + wpStartLookAt * (1.0f - m_flCurBlendWeight);

        csThisFrame.wvLookFrom = wpWeightedLookFrom;
        csThisFrame.wvLookAt = wpWeightedLookAt;
    }
    else
    {
        csThisFrame.wvLookFrom = tLerpTransform.wp;
        csThisFrame.wvLookAt = wpLookAtLerp;
    }
}
