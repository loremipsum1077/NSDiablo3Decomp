#include "Contrib/Contrib/blz/include/blz/float.h"
#include "X/Client/Camera/FixedViewController.h"
#include "X/Client/CGame.h"

#include "X/Client/Camera/CameraLocation.h"
#include "X/Client/Movement/CActorMovement.h"
#include "X/Client/RActors/RActors.h"
#include "X/Client/RWindow.h"
#include "X/Client/Shake.h"
#include "X/Common/Game.h"
#include "X/Common/GlobalSNO.h"
#include "X/Common/Definitions/ObserverDefinition.h"
#include "X/Sigma/Math/Matrix.h"
#include "X/Sigma/Math/Quaternion.h"
#include "X/Sigma/Math/Transform.h"

// X/Client/Camera/Movement.cpp:83; no header in src declares it yet.
void SmoothMovementFloat(const float fCurrentSpeed, const float fCurrentPosition, const float fMaxSpeed, const float fMaxAcceleration, const float fMaxDeceleration, const float fFinalPosition, float & fNewPosition);

































// line 42
FixedViewController::FixedViewController()
{
    m_idFollow = RACTORID_INVALID;
    m_idFocus = RACTORID_INVALID;
    m_idFocusPrevious = RACTORID_INVALID;

    m_flFocusT = 0.0f;
    m_flTargetFocusT = 0.0f;
    m_flFocusReleaseT = 0.0f;
    m_flFocusDuration = 0.0f;
    m_flFocusReleaseDuration = 0.0f;
    m_flFocusLastUpdate = 0.0f;
    m_bSnapFocus = FALSE;

    m_aDesiredYaw = 0.0f;
    m_aCurrentYaw = 0.0f;
    m_aPrevYaw = 0.0f;
    m_flYawT = 0.0f;
    m_flYawDuration = 0.0f;
    m_flYawLastUpdate = 0.0f;

    m_flCurrentZoom = 0.0f;
    m_flDesiredZoom = 0.0f;
    m_flPreviousZoom = 0.0f;
    m_flZoomT = 0.0f;
    m_flTargetZoomT = 0.0f;
    m_flZoomReleaseT = 0.0f;
    m_flZoomDuration = 0.0f;
    m_flZoomReleaseDuration = 0.0f;
    m_flZoomLastUpdate = 0.0f;

    m_flRadiusOffsetDesired = 0.0f;
    m_flRadiusOffsetCurrent = 0.0f;
    m_flRadiusOffsetPrev = 0.0f;
    m_wpLastActorLocation = cgwpNull;
    m_nLastMouseWheelPos = 0;
    m_flT = 0.0f;
    m_flDefaultT = 0.0f;
    m_flZoomSpeedDefault = 1.0f;

    m_flZoomSpeedOverride = 1.0f;
    m_flOverrideT = 0.0f;
    m_snoObserver = CameraManagerGetDefaultObserver(FALSE);
    OffsetClear();
}







// line 94
void FixedViewController::SetFollow(const RActorID idRActorToFollow, const float flRadiusOffset)
{
    m_idFollow = idRActorToFollow;
    m_flRadiusOffsetDesired = flRadiusOffset;
    OffsetClear();
}

// line 106
void FixedViewController::SetYaw(Angle aAngle, BOOL bSnap, float flDuration)
{
    if (bSnap)
    {
        m_aDesiredYaw = aAngle;
        m_aCurrentYaw = aAngle;
    }
    else
    {
        m_aPrevYaw = m_aCurrentYaw;
        m_flYawT = 0.0f;
        m_aDesiredYaw = aAngle;
        m_flYawDuration = flDuration;
        m_flYawLastUpdate = CGameGetClientTime();

        if (m_flYawDuration < EPSILON)
        {
            m_flYawDuration = 1.0f;
        }
    }
}

RActorID FixedViewController::GetFocus()
{
    return m_idFocus;
}

// line 144
void FixedViewController::SetFocus(const RActorID idRActorToFocus, BOOL bSnapFocus, float flDuration)
{
    m_idFocusPrevious = m_idFocus;
    m_idFocus = idRActorToFocus;
    m_flFocusDuration = flDuration;

    if (bSnapFocus)
    {
        m_flFocusT = 1.0f;
        m_flTargetFocusT = 1.0f;
    }
    else
    {
        m_flFocusT = 0.0f;
        m_flTargetFocusT = flDuration;
        if (m_flFocusDuration < EPSILON)
            m_flFocusDuration = 1.0f;
    }

    m_flFocusReleaseT = 0.0f;
    m_flFocusLastUpdate = CGameGetClientTime();
    m_bSnapFocus = bSnapFocus;
    OffsetClear();
}

// line 179
void FixedViewController::ClearFocus(BOOL bSnap, float flDuration)
{
    m_flFocusReleaseDuration = flDuration;
    m_flFocusReleaseT = m_flFocusT + flDuration;
    m_bSnapFocus = bSnap;

    if (!bSnap && flDuration < EPSILON)
    {
        m_flFocusReleaseDuration = 1.0f;
    }
}

// line 200
inline void FixedViewController::OffsetClear()
{
    m_psCameraOffset.wdRadius = m_psCameraOffset.aPitch = m_psCameraOffset.aYaw = m_flTargetOffsetZ = 0.0f;
}

void FixedViewController::SetDefaultZoom(const float flZoom)
{
    m_flDefaultT = flZoom;
}

void FixedViewController::SetZoom(const float flZoom)
{
    m_flOverrideT = flZoom;
}

// line 323
void FixedViewController::SetScriptedZoom(const float flZoom, const BOOL bSnap, const float flZoomDuration)
{
    m_flPreviousZoom = m_flCurrentZoom;
    m_flDesiredZoom = flZoom;

    if (bSnap)
    {
        m_flCurrentZoom = flZoom;
        m_flZoomT = 1.0f;
        m_flTargetZoomT = 1.0f;
    }
    else
    {
        m_flDesiredZoom = flZoom;
        m_flZoomT = 0.0f;
        m_flZoomDuration = flZoomDuration < EPSILON ? 1.0f : flZoomDuration;
        m_flTargetZoomT = flZoomDuration;
    }

    m_flZoomReleaseT = 0.0f;
    m_flZoomLastUpdate = CGameGetClientTime();
}

// line 361
void FixedViewController::SetDefaultZoomSpeed(const float flZoomSpeed)
{
    m_flZoomSpeedDefault = Math::Pin(flZoomSpeed, 0.1f, 1.0f);
}

// line 370
void FixedViewController::SetZoomSpeed(const float flZoomSpeed)
{
    m_flZoomSpeedOverride = Math::Pin(flZoomSpeed, 0.1f, 1.0f);
}

void FixedViewController::SetWheelPosition(const int32 nWheelPosition)
{
    m_nLastMouseWheelPos = nWheelPosition;
}

// line 389
RWindow * FixedViewController::sGetRWindow() const
{

    return &GetRWindowMgr()->GetCurrentRWindow();
}

// line 397
void FixedViewController::Update(float flDeltaTime, const BOOL fForceUpdateOfAllParams)
{
    BOOL fSnapCamera = fForceUpdateOfAllParams;

    int32 nCameraIndex = -1;

    CameraState cmState = CameraManagerGetCameraState(nCameraIndex);
    float flLastZ = cmState.transform.wp.z;

    SNO snoObserver = m_snoObserver;
    if (snoObserver != SNO_NONE && RActorTryToGet(m_idFollow))
    {
        ScopedObserverDefinitionLockR pdefObserver(snoObserver);
        cmState.aFOV = pdefObserver->aFOV;
        cmState.flNearPlane = pdefObserver->flZNear;
        cmState.flFarPlane = pdefObserver->flZFar;
        cmState.wvCameraOffsetVector = pdefObserver->wvCameraOffsetVector;
        cmState.wvLookAtOffsetVector = pdefObserver->wvLookAtOffsetVector;
        cmState.bPerspective = !pdefObserver->fOrthographic;
        cmState.flHeight = pdefObserver->flOrthographicHeight;
    }

    if (CGameInScriptedSequence())
    {
        float flUpdateTime = CGameGetClientTime() - m_flZoomLastUpdate;
        m_flZoomLastUpdate = CGameGetClientTime();

        if (m_flZoomT < m_flTargetZoomT && m_flZoomDuration > EPSILON)
        {
            Math::Lerp(m_flPreviousZoom, m_flDesiredZoom, Math::SmootherStep(Math::Min(1.0f, m_flZoomT / m_flZoomDuration)), m_flCurrentZoom);

            m_flZoomT += flUpdateTime;
        }

        CameraState cmStateCurr = cmState;
        CameraStateInterpolate(cmState, cmStateCurr, GlobalSNOGet(GLOBALSNO_OBSERVER_ZOOMED), m_flCurrentZoom);
    }
    else
    {
        m_flCurrentZoom = 0.0f;

        float flZoom = m_flDefaultT;
        float flZoomSpeed = m_flZoomSpeedDefault;

        if (m_flOverrideT > CAMERA_UNIT_ZOOM_EPSILON || m_flOverrideT < -CAMERA_UNIT_ZOOM_EPSILON)
        {
            flZoom = m_flOverrideT;
            flZoomSpeed = m_flZoomSpeedOverride;
        }

        float flIncrement = Math::Min(flZoomSpeed * 0.07f * flDeltaTime * 60.0f, 1.0f);
        float flDelta = (flZoom - m_flT) * flIncrement;

        if (fForceUpdateOfAllParams || Math::Abs(flDelta) > CAMERA_UNIT_ZOOM_EPSILON)
        {
            m_flT += flDelta;
        }

        if (Math::Abs(flDelta) <= CAMERA_UNIT_ZOOM_EPSILON)
        {
            m_flT = flZoom;
        }

        if (m_flT > CAMERA_UNIT_ZOOM_EPSILON)
        {
            CameraState cmStateCurr = cmState;
            CameraStateInterpolate(cmState, cmStateCurr, GlobalSNOGet(GLOBALSNO_OBSERVER_ZOOMED), m_flT);
            fSnapCamera = TRUE;
        }
        else if (m_flT < -CAMERA_UNIT_ZOOM_EPSILON)
        {
            CameraState cmStateCurr = cmState;
            CameraStateInterpolate(cmState, cmStateCurr, GlobalSNOGet(GLOBALSNO_OBSERVER_WIDE), -m_flT);
            fSnapCamera = TRUE;
        }
    }

    const RActor * ptRActor = RActorTryToGet(m_idFollow);
    const RActor * ptRActorFocus = RActorTryToGet(m_idFocus);
    const RActor * ptRActorFocusPrevious = RActorTryToGet(m_idFocusPrevious);
    SWorldID idSWorld = SWORLDID_INVALID;

    if (ptRActor && ptRActorFocus && CGameInScriptedSequence())
    {
        float flUpdateTime = CGameGetClientTime() - m_flFocusLastUpdate;
        m_flFocusLastUpdate = CGameGetClientTime();

        if (!m_bSnapFocus)
        {
            idSWorld = ptRActorFocus->tLocation.tWorldLocation.idSWorld;
            if (m_flFocusT < m_flTargetFocusT)
            {

                float flLerpFrac = Math::SmootherStep(Math::Min(1.0f, m_flFocusT / m_flFocusDuration));
                if (ptRActorFocusPrevious)
                {
                    Math::Lerp(ptRActorFocusPrevious->wpWalkingPos, ptRActorFocus->wpWalkingPos, flLerpFrac, m_wpLastActorLocation);
                }
                else
                {
                    Math::Lerp(ptRActor->transformClient.wp, ptRActorFocus->wpWalkingPos, flLerpFrac, m_wpLastActorLocation);
                }
                m_flFocusT += flUpdateTime;
            }

            else if (m_flFocusT >= m_flTargetFocusT && m_flFocusReleaseT < EPSILON)
            {

                m_flFocusT += flUpdateTime;
                m_wpLastActorLocation = ptRActorFocus->wpWalkingPos;
            }
            else if (m_flFocusT < m_flFocusReleaseT)
            {

                float flLerpFrac = Math::SmootherStep(Math::Min(1.0f, (m_flFocusReleaseT - m_flFocusT) / m_flFocusReleaseDuration));
                Math::Lerp(ptRActor->transformClient.wp, ptRActorFocus->wpWalkingPos, flLerpFrac, m_wpLastActorLocation);
                m_flFocusT += flUpdateTime;
            }
            else
            {
                m_idFocus = RACTORID_INVALID;
            }
        }
        else
        {
            if (m_flFocusReleaseT < EPSILON)
            {
                m_wpLastActorLocation = ptRActorFocus->wpWalkingPos;
                idSWorld = ptRActorFocus->tLocation.tWorldLocation.idSWorld;
            }
            else
            {
                m_wpLastActorLocation = ptRActor->transformClient.wp;
                m_idFocus = RACTORID_INVALID;
                idSWorld = ptRActor->tLocation.tWorldLocation.idSWorld;
            }
        }
    }
    else if (ptRActor)
    {
        m_idFocus = RACTORID_INVALID;

        if (ptRActor->ptCActorMovement)
        {
            ptRActor->ptCActorMovement->GetCameraTargetPos(m_wpLastActorLocation);
        }
        else
        {
            m_wpLastActorLocation = ptRActor->transformClient.wp;
        }

        idSWorld = ptRActor->tLocation.tWorldLocation.idSWorld;
    }

    float flRadiusOffsetSpeed = m_flRadiusOffsetCurrent - m_flRadiusOffsetPrev;
    m_flRadiusOffsetPrev = m_flRadiusOffsetCurrent;

    SmoothMovementFloat(flRadiusOffsetSpeed,
                        m_flRadiusOffsetCurrent,
                        1.0f,
                        0.01f,
                        0.01f,
                        m_flRadiusOffsetDesired,
                        m_flRadiusOffsetCurrent);

    if (CGameInScriptedSequence())
    {
        if (m_flYawT < m_flYawDuration && m_flYawDuration > EPSILON)
        {

            float flUpdateTime = CGameGetClientTime() - m_flYawLastUpdate;
            m_flYawLastUpdate = CGameGetClientTime();
            Math::Lerp(m_aPrevYaw, m_aDesiredYaw, Math::Min(1.0f, m_flYawT / m_flYawDuration), m_aCurrentYaw);

            m_flYawT += flUpdateTime;
        }
    }
    else
    {

        if (!GameIsParamFlagSet(GPF_REVERSE_CAMERA_FOR_TEAM_1))
        {
            m_aCurrentYaw = 0.0f;
        }
    }

    Vector3D wvCameraOffsetVector = cmState.wvCameraOffsetVector;
    Vector3D wvLookAtOffsetVector = cmState.wvLookAtOffsetVector;
    Vector3D::Rotate(cgwvZAxis, m_aCurrentYaw, wvCameraOffsetVector);
    Vector3D::Rotate(cgwvZAxis, m_aCurrentYaw, wvLookAtOffsetVector);

    Vector3D wvDirectionVector = wvCameraOffsetVector;
    wvDirectionVector.Normalize();

    Vector3D wpLookFrom = m_wpLastActorLocation + wvCameraOffsetVector + wvDirectionVector * m_flRadiusOffsetCurrent;
    Vector3D wpLookAt = m_wpLastActorLocation + wvLookAtOffsetVector;

    Vector3D wpLookFromUnshaken = wpLookFrom;
    Vector3D wpLookAtUnshaken = wpLookAt;

    Vector3D wvShakeCameraOffset;
    Quaternion qShakeCameraRotation;
    ShakeGetCameraOffset(sGetRWindow()->GetShakeRWindowData(), wvShakeCameraOffset, qShakeCameraRotation);

    float flShakeMagSquared = wvShakeCameraOffset.MagnitudeSquared();
    if (flShakeMagSquared > EPSILON)
    {
        wpLookFrom += wvShakeCameraOffset;
        wpLookAt += wvShakeCameraOffset;
    }
    else
    {

        if (!fSnapCamera)
        {

            const WorldDistance cwdMaxDelta = 5.0f;
            const float cflFactor = 0.5f;


            float flDelta = (flLastZ - wpLookFromUnshaken.z) * cflFactor;
            if (Math::Abs(flDelta) < cwdMaxDelta)
            {
                flDelta = Math::Pin(flDelta, -1.0f, 1.0f);
                wpLookFrom.z = wpLookFromUnshaken.z + flDelta;
                wpLookAt.z += flDelta;
            }
        }
    }

    PRTransform transformUnshaken;
    transformUnshaken.Set(wpLookFromUnshaken, wpLookAtUnshaken, cgwvZAxis);

    PRTransform transformNew;
    transformNew.Set(wpLookFrom, wpLookAt, cgwvZAxis);
    transformNew.q = transformNew.q * qShakeCameraRotation;

    cmState.transform = transformNew;


    cmState.matIrrad = cgm4x4Identity;


    CameraManagerSetCameraState(nCameraIndex, cmState);

    CameraManagerSetMicrophoneTransformFromTarget(nCameraIndex, transformUnshaken, wpLookAtUnshaken);

    SetLookAt(wpLookAtUnshaken, idSWorld);
}
