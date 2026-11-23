// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/Camera/CameraManager.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Common/SNO/SNOFiles.h"
#include "../../Sigma/SigmaTypes.h"

enum {
    NUM_REFLECTION_PLANES = 2,
};

enum {
    CAMERA_CURRENT = -1,
    CAMERA_DEFAULT_VIEW = 0,
    CAMERA_SHADOW = 1,
    CAMERA_REFLECTION_0 = 2,
    CAMERA_REFLECTION_1 = 3,
    CAMERA_RACTOR_SPRITE = 4,
    CAMERA_ADDITIVE = 5,
    CAMERA_RIPPLEMAP = 6,
    CAMERA_HOLODECK = 7,
    CAMERA_HOLODECK_ADDITIVE = 8,
    NUM_CAMERAS = 9,
};

enum {
    INVALID_VIEW_CONTROLLER = -1,
    ACTOR_VIEW_CONTROLLER = 0,
    AV_VIEW_CONTROLLER = 1,
    CONSOLE_VIEW_CONTROLLER = 2,
    FIXED_VIEW_CONTROLLER = 3,
    HARDPOINT_VIEW_CONTROLLER = 4,
    NUM_VIEW_CONTROLLERS = 5,
};

typedef int32 ViewControllerType;


struct Camera;
struct CameraState;
struct IVector2D;
struct LineSeg3D;
struct PRTransform;
struct Vector2D;
struct Vector3D;

// Functions defined in X/Client/Camera/CameraManager.cpp.
void CameraManagerInitializeForGame();
void CameraManagerDisposeForGame();
void CameraManagerResetCameras();
void CameraManagerResetCamera(int nCameraIndex);
void CameraManagerInitializeForWorld();
void CameraManagerDisposeForWorld();
void CameraManagerPrepareRender();
void CameraManagerRestoreCurrentCameraToMainCamera();
void CameraManagerSetCurrentCamera(const int32 nCameraIndex);
const Camera & CameraManagerGetCamera(const int32 nCameraIndex);
void CameraManagerSetMicrophoneTransformFromTarget(int32 nCameraIndex, const PRTransform & transform, const Vector3D & wpLookAt);
void CameraManagerSetMicrophoneTransform(int32 nCameraIndex, const PRTransform & volumeTransform, const PRTransform & panningTransform);
void CameraManagerSetAspectRatio(const float flAspectRatio);
void CameraManagerSetCameraState(int32 nCameraIndex, const CameraState & cmsNew);
void CameraManagerSetCameraTransform(const int32 nCameraIndex, const PRTransform & transform);
const CameraState & CameraManagerGetCameraState(const int32 nCameraIndex);
void CameraManagerBuildShadowCamera(const int32 nCameraIndex, const Vector3D & targetPos);
void CameraManagerBuildAdditiveCamera(const int32 nCameraIndex, const Vector3D & wpInitialTarget, const float aRotation);
void CameraManagerBuildRippleMapCamera(const int32 nCameraIndex, const Vector3D & wpTarget);
void ScreenPointToWorldLineSeg(const IVector2D & p, LineSeg3D & wline, int32 nCameraToUse);
BOOL WorldPointToScreenPoint(const Vector3D & wp, Vector2D & wpScreen, const int32 nCameraIndex);
BOOL WorldPointToScreenPointUIC(const Vector3D & wp, Vector2D & wpUIC, const int32 nCameraIndex);
BOOL WorldPointToScreenPoint3DUIC(const Vector3D & wp, Vector3D & wpUIC, const int32 nCameraIndex);
BOOL WorldPointToScreenPointUICUnclipped(const Vector3D & wp, Vector2D & wpUIC, const int32 nCameraIndex);
BOOL CameraManagerShouldNearPlaneFade(const Vector3D & wp);
SNO CameraManagerGetDefaultObserver(BOOL bPlayingGameModeOnly);
// Defined in CameraManager.cpp:46; FixedViewController::Update reads it through the GOT, so it is declared extern somewhere.
extern const float CAMERA_UNIT_ZOOM_EPSILON;
void CameraManagerSetHighResScreenshotMode(const uint32 dwTile, const uint32 dwTilesPerRow);
void CameraManagerUpdateChildCameras(const int32 nCameraIndex);
BOOL CameraManagerIsInitializedForWorld();
