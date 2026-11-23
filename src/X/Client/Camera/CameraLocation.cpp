#include "CameraLocation.h"

#include "../../Common/Definitions/ObserverDefinition.h"

// Functions sit on their original lines (CameraLocation.cpp:18, 65, 103, 141, 163).











//-------------------------------------------------------------------------------------------------
void CameraStateSetPerspective(
    CameraState & cmState,
    const PRTransform & transform,
    const Angle aFOV,
    const float flAspectRatio,
    const float flNearPlane,
    const float flFarPlane,
    const Vector3D & wvCameraOffsetVector,
    const Vector3D & wvLookAtOffsetVector,
    const Vector3D & wvLookFrom,
    const Vector3D & wvLookAt,
    const Rect2D & rectViewport,
    const Vector2D & vecScale,
    const Vector2D & vecProjectionTranslate )
{






    cmState.transform = transform;

    cmState.bPerspective = TRUE;
    cmState.aFOV = aFOV;
    cmState.flAspectRatio = flAspectRatio;
    cmState.flNearPlane = flNearPlane;
    cmState.flFarPlane = flFarPlane;
    cmState.wvCameraOffsetVector = wvCameraOffsetVector;
    cmState.wvLookAtOffsetVector = wvLookAtOffsetVector;
    cmState.rectViewport = rectViewport;
    cmState.vecScale = vecScale;
    cmState.vecProjectionTranslate = vecProjectionTranslate;
    cmState.wvLastPOI = cgwpNull;
    cmState.flHeight = 0.0f;


    cmState.wvLookFrom = wvLookFrom;
    cmState.wvLookAt = wvLookAt;
    cmState.matIrrad = cgm4x4Identity;
    cmState.flZoomFactor = 0.0f;

}



//-------------------------------------------------------------------------------------------------
void CameraStateSetOrthographic(
    CameraState & cmState,
    const PRTransform & transform,
    const float flHeight,
    const float flAspectRatio,
    const float flNearPlane,
    const float flFarPlane,
    const Rect2D & rectViewport,
    const Vector2D & vecScale,
    const Vector2D & vecProjectionTranslate )
{



    cmState.transform = transform;

    cmState.bPerspective = FALSE;
    cmState.flHeight = flHeight;
    cmState.flAspectRatio = flAspectRatio;
    cmState.flNearPlane = flNearPlane;
    cmState.flFarPlane = flFarPlane;
    cmState.wvCameraOffsetVector = cgwvNull;
    cmState.wvLookAtOffsetVector = cgwvNull;
    cmState.rectViewport = rectViewport;
    cmState.vecScale = vecScale;
    cmState.vecProjectionTranslate = vecProjectionTranslate;
    cmState.wvLastPOI = cgwpNull;
    cmState.aFOV = 0.0f;


    cmState.matIrrad = cgm4x4Identity;
    cmState.flZoomFactor = 0.0f;

}



//-------------------------------------------------------------------------------------------------
BOOL CameraState::operator==(const CameraState & cms)
{
    return
        transform == cms.transform &&
        aFOV == cms.aFOV &&
        flNearPlane == cms.flNearPlane &&
        flFarPlane == cms.flFarPlane &&
        Math::FloatEqual(flAspectRatio, cms.flAspectRatio) &&
        bPerspective == cms.bPerspective &&
        flHeight == cms.flHeight &&
        rectViewport == cms.rectViewport &&
        vecScale == cms.vecScale;
}
























//-------------------------------------------------------------------------------------------------
void CameraStateInterpolate(
    CameraState & cmStateNew,
    const CameraState & cmState0,
    const CameraState & cmState1,
    const float flT )
{
    cmStateNew = cmState0;

    PRTransform::Lerp(cmState0.transform, cmState1.transform, flT, cmStateNew.transform);

    Math::Lerp(cmState0.aFOV, cmState1.aFOV, flT, cmStateNew.aFOV);
    Math::Lerp(cmState0.flNearPlane, cmState1.flNearPlane, flT, cmStateNew.flNearPlane);
    Math::Lerp(cmState0.flFarPlane, cmState1.flFarPlane, flT, cmStateNew.flFarPlane);
    Math::Lerp(cmState0.wvCameraOffsetVector, cmState1.wvCameraOffsetVector, flT, cmStateNew.wvCameraOffsetVector);
    Math::Lerp(cmState0.wvLookAtOffsetVector, cmState1.wvLookAtOffsetVector, flT, cmStateNew.wvLookAtOffsetVector);


}



//-------------------------------------------------------------------------------------------------
void CameraStateInterpolate(
    CameraState & cmStateNew,
    const CameraState & cmState,
    const SNO snoObserver,
    const float flT )
{
    CameraState cmStateTarget;  // only the observer's lens and offsets are filled in
    {
        ScopedObserverDefinitionLockR pdefObserver(snoObserver);
        cmStateTarget.aFOV = pdefObserver->aFOV;
        cmStateTarget.flNearPlane = pdefObserver->flZNear;
        cmStateTarget.flFarPlane = pdefObserver->flZFar;
        cmStateTarget.wvCameraOffsetVector = pdefObserver->wvCameraOffsetVector;
        cmStateTarget.wvLookAtOffsetVector = pdefObserver->wvLookAtOffsetVector;
    }

    CameraStateInterpolate(cmStateNew, cmState, cmStateTarget, flT);
}
