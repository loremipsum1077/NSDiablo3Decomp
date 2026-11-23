#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "CameraManager.h"
#include "Camera.h"
#include "CameraLocation.h"
#include "../CGame.h"
#include "../CWorld.h"
#include "../PostFX.h"
#include "../Lights.h"
#include "../RWindow.h"
#include "../SubObjectGfx.h"
#include "../Weather.h"
#include "../Gfx/Gfx.h"
#include "../Gfx/GfxCommon.h"
#include "../Definitions/TextureDefinition.h"
#include "../UserInterface/UIUtils.h"
#include "../../Common/Definitions/ObserverDefinition.h"
#include "../../Common/Game.h"
#include "../../Common/GameCommonData.h"
#include "../../Common/GlobalSNO.h"
#include "../../Common/ThreadGlobals.h"
#include "../../Sigma/Math/MathGeneral.h"
#include "../../Sigma/Math/Matrix.h"
#include "../../Sigma/Math/Plane.h"
#include "../../Sigma/Math/Quaternion.h"
#include "../../Sigma/Math/Ray.h"
#include "../../Sigma/Math/Rect.h"
#include "../../Sigma/Math/Transform.h"
#include "../../Sigma/Math/Vector.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/SigmaTypes.h"

// X/Client/Camera/Listener.cpp has no header in the DWARF (only Listener.cpp), so these are
// declared here until an owning header exists.
void ListenerSetLocationFromTarget(PRTransform & transformMic, const Vector3D & wpLookAt, const WorldDistance wdUpComponent, const WorldDistance wdViewComponent, const PRTransform & transform);
void ListenerSetMicrophoneLocation(const PRTransform & volumeTransform, const PRTransform & panningTransform, const float flRolloffFactor, const float flDopplerFactor);

extern const float CAMERA_UNIT_ZOOM_EPSILON = 1.6666667e-06f;  // line 46; bits 0x35dfb23b from the image, the source expression is not known

struct CameraManager {  // size 0x2f50
    Camera arRenderCameras[1][NUM_CAMERAS];  // 0x00
    int32 nCurrentRenderCamera[1];  // 0x17a0
    Camera arCameras[NUM_CAMERAS];  // 0x17a4
    int32 nCurrentCamera;  // 0x2f44
    float flShadowZNear;  // 0x2f48
    BOOL bIsInitializedForWorld;  // 0x2f4c
};
static_assert(sizeof(CameraManager) == 0x2f50, "CameraManager");

static SNO sCameraManagerGetSNO(int32 nCameraIndex);
static void sUpdateChildCameras(const CameraState & cmsNew);
static Camera & sGetCamera(const int32 nCameraIndex);
void sQuantizeCameraTargetLocation(const int32 nCameraIndex, Vector3D & wpTarget, SNO snoTarget);

void CameraManagerInitializeForGame() {
    CGame & tCGame = CGameGet();

    CameraManager *& ptCameraManager = tCGame.ptCameraManager;
    ptCameraManager = (CameraManager *)SigmaMemoryNew(sizeof(CameraManager), 4, NULL, TRUE);

    for (int32 i = 0; i < NUM_CAMERAS; i++) {
        Camera & tCamera = ptCameraManager->arCameras[i];
        tCamera.snoObserver = SNO_NONE;
        tCamera.nCameraIndex = i;
    }
}

void CameraManagerDisposeForGame() {
    CGame & tCGame = CGameGet();
    CameraManager *& ptCameraManager = tCGame.ptCameraManager;

    SigmaMemoryFree(ptCameraManager, 0);
    ptCameraManager = NULL;
}

static void sBuildPrimaryCamera(const int32 nCameraIndex) {
    SNO snoObserver = sCameraManagerGetSNO(nCameraIndex);

    ScopedObserverDefinitionLockR pdefObserver(snoObserver);

    Vector3D wpLookFrom;
    Vector3D wpLookTo;

    wpLookFrom = cgplaceNull.wp + pdefObserver->wvCameraOffsetVector;
    wpLookTo = cgplaceNull.wp + pdefObserver->wvLookAtOffsetVector;

    Rect2D rectScreenNDC = cgrectViewportDefault;
    float flAspectRatioToUse = CGameGetLetterboxedViewportNDC(rectScreenNDC);

    PRTransform transform;
    transform.Set(wpLookFrom, wpLookTo, cgwvZAxis);

    Angle aFOV = pdefObserver->aFOV;
    float flZNear = pdefObserver->flZNear;
    float flZFar = pdefObserver->flZFar;

    const float FOV_ANGLE_MIN = PI / 64.0f;
    if (aFOV < FOV_ANGLE_MIN) {
        aFOV = FOV_ANGLE_MIN;
    }

    const float MIN_FAR_DISTANCE_FROM_NEAR = 1.0f;
    if (flZFar - flZNear <= MIN_FAR_DISTANCE_FROM_NEAR) {
        flZFar = flZNear + MIN_FAR_DISTANCE_FROM_NEAR;
    }

    CameraState cms;
    CameraStateSetPerspective(cms, transform, aFOV, flAspectRatioToUse, flZNear, flZFar,
                              pdefObserver->wvCameraOffsetVector, pdefObserver->wvLookAtOffsetVector,
                              wpLookFrom, wpLookTo, rectScreenNDC, Vector2D(1.0f, 1.0f), Vector2D(0.0f, 0.0f));

    CameraManagerSetCameraState(nCameraIndex, cms);
}

void CameraManagerResetCameras() {
    for (int32 i = 0; i < NUM_CAMERAS; i++) {
        CameraManagerResetCamera(i);
    }
}

void CameraManagerResetCamera(int nCameraIndex) {
    CameraManager * ptCameraManager = CGameGet().ptCameraManager;
    if (!ptCameraManager) {
        return;
    }

    SNO snoObserver = CameraManagerGetDefaultObserver(FALSE);
    Camera & tCamera = ptCameraManager->arCameras[nCameraIndex];
    tCamera.snoObserver = snoObserver;

    switch (nCameraIndex) {
        case CAMERA_DEFAULT_VIEW:
        case CAMERA_RACTOR_SPRITE:
        case CAMERA_HOLODECK:
            sBuildPrimaryCamera(nCameraIndex);
            break;
    }
}

void CameraManagerInitializeForWorld() {
    CameraManager * ptCameraManager = CGameGet().ptCameraManager;

    ptCameraManager->nCurrentCamera = CAMERA_DEFAULT_VIEW;
    for (int32 worker = 0; worker < 1; worker++) {
        ptCameraManager->nCurrentRenderCamera[worker] = CAMERA_DEFAULT_VIEW;
    }

    CameraManagerResetCameras();

    ptCameraManager->bIsInitializedForWorld = TRUE;
}

void CameraManagerDisposeForWorld() {
}

void CameraManagerPrepareRender() {
    CameraManager * ptCameraManager = CGameGet().ptCameraManager;

    for (int32 worker = 0; worker < 1; worker++) {
        for (uint32 i = 0; i < NUM_CAMERAS; i++) {
            ptCameraManager->arRenderCameras[worker][i] = ptCameraManager->arCameras[i];
        }
    }
}

void CameraManagerRestoreCurrentCameraToMainCamera() {
    CameraManager * ptCameraManager = CGameGet().ptCameraManager;

    if (ptCameraManager) {
        CameraManagerSetCurrentCamera(GetRWindowMgr()->GetCurrentRWindow().GetCameraIndex());
    }
}

void CameraManagerSetCurrentCamera(const int32 nCameraIndex) {
    CameraManager * ptCameraManager = CGameGet().ptCameraManager;

    BOOL fRenderThread = ThreadGlobals::Get().m_nThreadIndex == THREADINDEX_ASYNCRENDER;
    if (fRenderThread) {
        ptCameraManager->nCurrentRenderCamera[GfxGetWorkerIndex()] = nCameraIndex;

        const Camera & cam = sGetCamera(nCameraIndex);
        GfxViewMatrixSet(cam.mView);
        GfxProjectionMatrixSet(cam.mProjection);
        GfxIrradMatrixSet(cam.cmState.matIrrad);
        GfxCameraBillboardSet(cam.qBillboard);
    } else {
        ptCameraManager->nCurrentCamera = nCameraIndex;
    }
}

const Camera & CameraManagerGetCamera(const int32 nCameraIndex) {
    return sGetCamera(nCameraIndex);
}

void CameraManagerSetMicrophoneTransformFromTarget(int32 nCameraIndex, const PRTransform & transform, const Vector3D & wpLookAt) {
    PRTransform transformMicVolume;
    PRTransform transformMicPanning;
    ScopedObserverDefinitionLockR pdefObserver(sCameraManagerGetSNO(nCameraIndex));

    ListenerSetLocationFromTarget(transformMicVolume, wpLookAt, pdefObserver->wdMicrophoneHeight, pdefObserver->wdMicrophoneXYOffset, transform);
    ListenerSetLocationFromTarget(transformMicPanning, wpLookAt, pdefObserver->wdMicrophoneHeight, pdefObserver->wdMicrophoneXYOffset - pdefObserver->wdMicrophonePanningDistance, transform);

    CameraManagerSetMicrophoneTransform(nCameraIndex, transformMicVolume, transformMicPanning);
}

void CameraManagerSetMicrophoneTransform(int32 nCameraIndex, const PRTransform & volumeTransform, const PRTransform & panningTransform) {
    if (nCameraIndex == CAMERA_CURRENT) {
        CameraManager * ptCameraManager = CGameGet().ptCameraManager;
        nCameraIndex = ptCameraManager->nCurrentCamera;
    }

    RWindow * ptRWindow = GetRWindowMgr()->GetActiveRWindowForAudio();
    if (!ptRWindow || nCameraIndex != ptRWindow->GetCameraIndex()) {
        return;
    }

    ScopedObserverDefinitionLockR pdefObserver(sCameraManagerGetSNO(nCameraIndex));
    float flRolloffFactor = pdefObserver->flRolloffFactor;
    float flDopplerFactor = 1.0f;

    ListenerSetMicrophoneLocation(volumeTransform, panningTransform, flRolloffFactor, flDopplerFactor);
}

static void sRebuildCamera(const int32 nCameraIndex, Camera & cam);

static void sSetCameraAspectRatio(const int32 nCameraIndex, const float flAspectRatio) {
    Camera & cam = sGetCamera(nCameraIndex);
    float flAspectRatioToUse = flAspectRatio;

    if (nCameraIndex == CAMERA_DEFAULT_VIEW) {
        Rect2D rectScreenNDC;
        flAspectRatioToUse = CGameGetLetterboxedViewportNDC(flAspectRatio, rectScreenNDC);
        cam.cmState.rectViewport = rectScreenNDC;
    }

    cam.cmState.flAspectRatio = flAspectRatioToUse;
    sRebuildCamera(nCameraIndex, cam);
}

void CameraManagerSetAspectRatio(const float flAspectRatio) {
    sSetCameraAspectRatio(CAMERA_DEFAULT_VIEW, flAspectRatio);
    sSetCameraAspectRatio(CAMERA_RACTOR_SPRITE, flAspectRatio);
}

static void sShearProjectionMatrix(Camera & cam, Plane & wplane) {
    Matrix4x4 mWVP = cam.mView * cam.mProjection;
    mWVP = mWVP.Inverse().Transpose();

    Vector4D wvClipPlane(wplane.wvNormal.x, wplane.wvNormal.y, wplane.wvNormal.z, wplane.wdD);
    Vector4D wvProjClipPlane = wvClipPlane * mWVP;

    if (wvProjClipPlane.w == 0.0f) {
        return;
    }

    if (wvProjClipPlane.w > 0.0f) {
        wvProjClipPlane = -wvProjClipPlane;
    }

    Matrix4x4 mClipProj = cgm4x4Identity;
    mClipProj._13 = wvProjClipPlane.x;
    mClipProj._23 = wvProjClipPlane.y;
    mClipProj._33 = wvProjClipPlane.z;
    mClipProj._43 = wvProjClipPlane.w;

    cam.mProjection = cam.mProjection * mClipProj;
}

static void sCameraSet(Camera & cam, const CameraState & cms) {
    cam.cmState = cms;
    CameraRebuild(cam);
}



static void sUpdateChildCameras(const CameraState & cmsNew) {
    CameraManager * ptCameraManager = CGameGet().ptCameraManager;

    BOOL fRenderThread = ThreadGlobals::Get().m_nThreadIndex == THREADINDEX_ASYNCRENDER;
    Camera * pCameras = fRenderThread ? ptCameraManager->arRenderCameras[GfxGetWorkerIndex()] : ptCameraManager->arCameras;

    for (int nPlaneIndex = 0; nPlaneIndex < PostFXGetNumReflectionPlanes(); nPlaneIndex++) {
        CameraState cmsReflection(cmsNew);
        PRTransform & transform = cmsReflection.transform;
        Plane & wplaneReflect = PostFXGetReflectionPlane(nPlaneIndex);

        Vector3D wvLook = transform.q.Rotate(cgwvInitCameraForward);
        wvLook = -Vector3D::Reflect(wvLook, wplaneReflect.wvNormal);
        transform.q.FromViewVector(wvLook, cgwvZAxis, cgwvInitCameraForward, cgwvInitCameraRight, cgwvInitCameraUp);

        transform.wp = wplaneReflect.Reflect(transform.wp);

        sCameraSet(pCameras[CAMERA_REFLECTION_0 + nPlaneIndex], cmsReflection);

        sShearProjectionMatrix(pCameras[CAMERA_REFLECTION_0 + nPlaneIndex], wplaneReflect);
    }
}

static void sRebuildCamera(const int32 nCameraIndex, Camera & cam) {
    CameraRebuild(cam);

    int32 nMainCamera = GetRWindowMgr()->GetCurrentRWindow().GetCameraIndex();
    if (nCameraIndex == nMainCamera) {
        sUpdateChildCameras(cam.cmState);
    }
}

void CameraManagerSetCameraState(int32 nCameraIndex, const CameraState & cmsNew) {
    Camera & cam = sGetCamera(nCameraIndex);

    if (cam.cmState == cmsNew) {
        return;
    }

    cam.cmState = cmsNew;
    sRebuildCamera(nCameraIndex, cam);
}

void CameraManagerSetCameraTransform(const int32 nCameraIndex, const PRTransform & transform) {
    Camera & cam = sGetCamera(nCameraIndex);

    cam.cmState.transform = transform;
    sRebuildCamera(nCameraIndex, cam);
}

static Camera & sGetCamera(const int32 nCameraIndex) {
    CameraManager * ptCameraManager = CGameGet().ptCameraManager;

    BOOL fRenderThread = ThreadGlobals::Get().m_nThreadIndex == THREADINDEX_ASYNCRENDER;
    Camera * pCameras = fRenderThread ? ptCameraManager->arRenderCameras[GfxGetWorkerIndex()] : ptCameraManager->arCameras;
    const int32 & nCurrentCamera = fRenderThread ? ptCameraManager->nCurrentRenderCamera[GfxGetWorkerIndex()] : ptCameraManager->nCurrentCamera;

    int32 nCameraIndexToUse = (nCameraIndex == CAMERA_CURRENT) ? nCurrentCamera : nCameraIndex;
    return pCameras[nCameraIndexToUse];
}

const CameraState & CameraManagerGetCameraState(const int32 nCameraIndex) {
    return sGetCamera(nCameraIndex).cmState;
}

static SNO sCameraManagerGetSNO(int32 nCameraIndex) {
    CGame & tCGame = CGameGet();
    CameraManager * ptCameraManager = tCGame.ptCameraManager;

    BOOL fRenderThread = ThreadGlobals::Get().m_nThreadIndex == THREADINDEX_ASYNCRENDER;
    Camera * pCameras = fRenderThread ? ptCameraManager->arRenderCameras[GfxGetWorkerIndex()] : ptCameraManager->arCameras;
    const int32 & nCurrentCamera = fRenderThread ? ptCameraManager->nCurrentRenderCamera[GfxGetWorkerIndex()] : ptCameraManager->nCurrentCamera;

    if (nCameraIndex == CAMERA_CURRENT) {
        nCameraIndex = nCurrentCamera;
    }

    return pCameras[nCameraIndex].snoObserver;
}

void sQuantizeCameraTargetLocation(const int32 nCameraIndex, Vector3D & wpTarget, SNO snoTarget) {
    Camera cam = CameraManagerGetCamera(nCameraIndex);

    Vector3D wvCamViewX(cam.mView._11, cam.mView._21, cam.mView._31);
    Vector3D wvCamViewY(cam.mView._12, cam.mView._22, cam.mView._32);
    Vector3D wvCamViewZ(cam.mView._13, cam.mView._23, cam.mView._33);

    if (wvCamViewX.MagnitudeSquared() > 0.0f) {
        ScopedTextureDefinitionLockR pdefTexture(snoTarget);

        float worldToTexelRatio = cam.cmState.flHeight / pdefTexture->dwHeight;

        Vector3D wvViewUnits(cam.cmState.flAspectRatio * worldToTexelRatio, worldToTexelRatio, worldToTexelRatio);
        Vector3D unitsOffset(Vector3D::DotProduct(wvCamViewX, wpTarget) / wvViewUnits.x,
                             Vector3D::DotProduct(wvCamViewY, wpTarget) / wvViewUnits.y,
                             Vector3D::DotProduct(wvCamViewZ, wpTarget) / wvViewUnits.z);

        unitsOffset.x = (float)Math::FloatRoundToInt(unitsOffset.x); unitsOffset.y = (float)Math::FloatRoundToInt(unitsOffset.y); unitsOffset.z = (float)Math::FloatRoundToInt(unitsOffset.z);

        wpTarget = wvViewUnits.x * (wvCamViewX * unitsOffset.x) + wvViewUnits.y * (wvCamViewY * unitsOffset.y) + wvViewUnits.z * (wvCamViewZ * unitsOffset.z);
    }
}

static void sBuildDungeonShadowCam(CameraState & cmsDungeon, const Vector3D & targetPos) {
    static Vector3D lightPosOffset(0.0f, 0.0f, 50.0f);

    const Angle MaxFOV = PI * (5.0f / 6.0f);  // DWARF :799 value 0x40278d36 (PI * 5.0f / 6.0f rounds to ...37); spelling inferred
    const float FOVBias = 0.9f;

    Vector3D lightPos = targetPos + lightPosOffset;
    PRTransform transform;
    transform.Set(lightPos, targetPos, cgwvZAxis);

    const Camera & cam = CameraManagerGetCamera(GetRWindowMgr()->GetCurrentRWindow().GetCameraIndex());
    Vector3D cameraPos(cam.cmState.transform.wp);

    Vector3D pointOnFarPlane = (cam.wp[4] + cam.wp[6]) * 0.5f;
    float flDenom = pointOnFarPlane.z - cameraPos.z;
    float T = 0.0f;
    if (Math::Abs(flDenom) > EPSILON) {
        T = (targetPos.z - cameraPos.z) / flDenom;
    }

    Vector3D pointOfInterest = cameraPos + (pointOnFarPlane - cameraPos) * T;

    Vector3D lightDirection = targetPos - lightPos;
    Vector3D directionOfInterestInLightSpace = pointOfInterest - lightPos;
    lightDirection.Normalize();
    directionOfInterestInLightSpace.Normalize();
    float cosTheta = Vector3D::DotProduct(lightDirection, directionOfInterestInLightSpace);

    Angle theta = Math::ACos(cosTheta * FOVBias);

    Angle fov = Math::Min(theta * 2.0f, MaxFOV);

    CameraStateSetPerspective(cmsDungeon, transform, fov, 1.0f, 3.0f, 100.0f, lightPos, targetPos, lightPos, targetPos,
                              cgrectViewportDefault, cgvecViewportScaleDefault, Vector2D(0.0f, 0.0f));

    CameraManager * ptCameraManager = CGameGet().ptCameraManager;
    ptCameraManager->flShadowZNear = 3.0f;
}

static void sBuildOutdoorShadowCam(CameraState & cmsOutdoor, const Vector3D & targetPos, Light * ptLight) {
    const float fZMin = 850.0f;
    const float fZMax = 1150.0f;

    Vector3D lightPos = targetPos + -ptLight->tState.wv * 1000.0f;
    PRTransform transform;
    transform.Set(lightPos, targetPos, cgwvZAxis);
    float flLargestAllowedShadowDist = 128.0f;  // DWARF :894; both locals are 128.0f (0x43000000) at 0x1a0c50, and 128.0f is the width passed at :922
    float flMaxDist = flLargestAllowedShadowDist;  // DWARF :898; the copy is inferred (no rows on :899-921)
    CameraStateSetOrthographic(cmsOutdoor, transform, flMaxDist, 1.0f, fZMin, fZMax,
                               cgrectViewportDefault, cgvecViewportScaleDefault, Vector2D(0.0f, 0.0f));
    CameraManager * ptCameraManager = CGameGet().ptCameraManager;
    ptCameraManager->flShadowZNear = 900.0f;
}





void CameraManagerBuildShadowCamera(const int32 nCameraIndex, const Vector3D & targetPos) {
    Light * ptLight = LightTryToGet(Weather::GetSunLightScene(GetRWindowMgr()->GetCurrentRWindow().GetWeatherRWindowData()));

    if (!ptLight) {
        CameraState cmsDungeon;
        sBuildDungeonShadowCam(cmsDungeon, targetPos);
        CameraManagerSetCameraState(nCameraIndex, cmsDungeon);
        return;
    }

    Vector3D finalTargetPos(targetPos);

    SNO snoShadowTex;
    Matrix4x4 shadowMat;
    SubObjectGfxGetShadowDepthData(snoShadowTex, shadowMat);
    sQuantizeCameraTargetLocation(nCameraIndex, finalTargetPos, snoShadowTex);

    CameraState cms;
    sBuildOutdoorShadowCam(cms, finalTargetPos, ptLight);
    CameraManagerSetCameraState(nCameraIndex, cms);
}

void CameraManagerBuildAdditiveCamera(const int32 nCameraIndex, const Vector3D & wpInitialTarget, const Angle aRotation) {
    Vector3D wpTarget(wpInitialTarget);

    sQuantizeCameraTargetLocation(nCameraIndex, wpTarget, GetRWindowMgr()->GetCurrentRWindow().GetPMATarget());

    Vector2D vecOffset = cgvecPMAOffset;
    if (aRotation != 0.0f) {
        vecOffset.Rotate(aRotation);
    }

    Vector3D eyePos(wpTarget.x, wpTarget.y, wpTarget.z + 500.0f);
    Vector3D wvUp(-cgflSqrt2Over2, -cgflSqrt2Over2, 0.0f);

    PRTransform transform;
    transform.Set(eyePos, wpTarget, wvUp);
    transform.wp.x += vecOffset.x;
    transform.wp.y += vecOffset.y;

    float flAspect = 1.25f;
    CameraState cms;
    CameraStateSetOrthographic(cms, transform, cwdPMAMapWidth, flAspect, -1000.0f, 1000.0f,
                               cgrectViewportDefault, cgvecViewportScaleDefault, Vector2D(0.0f, 0.0f));

    Camera cam = CameraManagerGetCamera(nCameraIndex);
    CameraManagerSetCameraState(nCameraIndex, cms);
}

void CameraManagerBuildRippleMapCamera(const int32 nCameraIndex, const Vector3D & wpTarget) {
    Vector3D eyePos(wpTarget.x, wpTarget.y, wpTarget.z + 500.0f);
    PRTransform transform;
    transform.Set(eyePos, wpTarget, cgwvYAxis);

    CameraState cms;
    CameraStateSetOrthographic(cms, transform, cwdRippleMapWidth, 1.0f, -1000.0f, 1000.0f,
                               cgrectViewportDefault, cgvecViewportScaleDefault, Vector2D(0.0f, 0.0f));

    Camera cam = CameraManagerGetCamera(nCameraIndex);
    CameraManagerSetCameraState(nCameraIndex, cms);
}

void ScreenPointToWorldLineSeg(const IVector2D & p, LineSeg3D & wline, int32 nCameraToUse) {
    const Camera & tCamera = CameraManagerGetCamera(nCameraToUse);
    Vector3D wp;

    CameraScreenToWorldPoint(tCamera, p, wp);

    if (tCamera.cmState.bPerspective) {
        wline.Set(tCamera.cmState.transform.wp, wp);
    } else {
        Vector3D wvView = tCamera.cmState.transform.q.Rotate(cgwvInitCameraForward);
        wline.Set(wp - wvView * tCamera.cmState.flFarPlane, wp);
    }
}

static BOOL WorldPointToScreenPoint3D(const Vector3D & wp, Vector3D & wpScreen, const int32 nCameraIndex) {
    const Camera & tCamera = CameraManagerGetCamera(nCameraIndex);

    BOOL bResult = CameraWorldToScreenPoint(tCamera, wp, wpScreen);
    return bResult;
}

BOOL WorldPointToScreenPoint(const Vector3D & wp, Vector2D & wpScreen, const int32 nCameraIndex) {
    const Camera & tCamera = CameraManagerGetCamera(nCameraIndex);

    Vector3D wpScreen3D;
    BOOL bResult = CameraWorldToScreenPoint(tCamera, wp, wpScreen3D);
    if (bResult) {
        wpScreen.Set(wpScreen3D.x, wpScreen3D.y);
    }

    return bResult;
}

BOOL WorldPointToScreenPointUIC(const Vector3D & wp, Vector2D & wpUIC, const int32 nCameraIndex) {
    Vector2D wpScreen;
    if (!WorldPointToScreenPoint(wp, wpScreen, nCameraIndex)) {
        return FALSE;
    }

    if (nCameraIndex != CAMERA_HOLODECK) {
        IRect2D rectViewportPixels;
        CGameGetLetterboxedViewportPixels(rectViewportPixels);
        if (wpScreen.x < rectViewportPixels.left || wpScreen.x >= rectViewportPixels.right ||
            wpScreen.y < rectViewportPixels.top || wpScreen.y >= rectViewportPixels.bottom) {
            return FALSE;
        }
    }

    ConvertPixelsToUIC(wpScreen, wpUIC);
    return TRUE;
}

BOOL WorldPointToScreenPoint3DUIC(const Vector3D & wp, Vector3D & wpUIC, const int32 nCameraIndex) {
    Vector3D wpScreen;
    if (!WorldPointToScreenPoint3D(wp, wpScreen, nCameraIndex)) {
        return FALSE;
    }

    Vector2D vecUIC;
    ConvertPixelsToUIC(Vector2D(wpScreen.x, wpScreen.y), vecUIC);
    wpUIC.x = vecUIC.x;
    wpUIC.y = vecUIC.y;
    wpUIC.z = wpScreen.z;
    return TRUE;
}

BOOL WorldPointToScreenPointUICUnclipped(const Vector3D & wp, Vector2D & wpUIC, const int32 nCameraIndex) {
    Vector2D wpScreen;
    if (!WorldPointToScreenPoint(wp, wpScreen, nCameraIndex)) {
        return FALSE;
    }

    ConvertPixelsToUIC(wpScreen, wpUIC);
    return TRUE;
}

BOOL CameraManagerShouldNearPlaneFade(const Vector3D & wp) {
    GameCommonData & tGCD = CGameGetGameCommonData();
    if (tGCD.GameModeGet() != GAMEMODE_PLAYING) {
        return FALSE;
    }

    const Camera & cam = CameraManagerGetCamera(CAMERA_CURRENT);
    const WorldDistance wdRadius = 15.0f;
    WorldDistance wdRadiusSqr = wdRadius * wdRadius;
    float flDist = Vector3D::Distance2DSquared(cam.cmState.transform.wp, wp);
    return flDist < wdRadiusSqr;
}

SNO CameraManagerGetDefaultObserver(BOOL bPlayingGameModeOnly) {
    if (!bPlayingGameModeOnly) {
        GameMode eMode = CGameGetGameCommonData().GameModeGet();
        if (GameModeIsAnEditMode(eMode)) {
            return GlobalSNOGet(GLOBALSNO_OBSERVER_EDITMODE_DEFAULT);
        }
    }

    return GlobalSNOGet(CWorldIsRetro() ? GLOBALSNO_OBSERVER_CONSOLE_RETRO : GLOBALSNO_OBSERVER_CONSOLE_DEFAULT);
}

void CameraManagerSetHighResScreenshotMode(const uint32 dwTile, const uint32 dwTilesPerRow) {
    int32 nCameraIndex = CAMERA_DEFAULT_VIEW;

    Camera & cam = sGetCamera(nCameraIndex);
    cam.dwHighResScreenshotTile = dwTile;
    cam.dwHighResScreenshotTilesPerRow = dwTilesPerRow;
    sRebuildCamera(nCameraIndex, cam);
}

void CameraManagerUpdateChildCameras(const int32 nCameraIndex) {
    Camera & cam = sGetCamera(nCameraIndex);
    sUpdateChildCameras(cam.cmState);
}

BOOL CameraManagerIsInitializedForWorld() {
    CGame & tCGame = CGameGet();
    CameraManager * ptCameraManager = tCGame.ptCameraManager;

    if (!ptCameraManager) {
        return FALSE;
    }

    return ptCameraManager->bIsInitializedForWorld;
}
