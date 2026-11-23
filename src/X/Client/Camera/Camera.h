// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/Camera/Camera.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "CameraLocation.h"
#include "../../Common/SNO/SNOFiles.h"
#include "../../Sigma/Math/Frustum.h"
#include "../../Sigma/Math/Matrix.h"
#include "../../Sigma/Math/Quaternion.h"
#include "../../Sigma/Math/Vector.h"

struct Camera {  // size 0x2a0
    SNO snoObserver;  // 0x00
    int32 nCameraIndex;  // 0x04
    CameraState cmState;  // 0x08
    float flFarHeight;  // 0xec
    float flFarWidth;  // 0xf0
    float flNearWidth;  // 0xf4
    float flNearHeight;  // 0xf8
    Vector3D wp[8];  // 0xfc
    Frustum tFrustum;  // 0x15c
    Matrix4x4 mView;  // 0x1bc
    Matrix4x4 mProjection;  // 0x1fc
    Matrix4x4 mProjectionUnadjusted;  // 0x23c
    Quaternion qBillboard;  // 0x27c
    Vector3D wvCameraForward;  // 0x28c
    uint32 dwHighResScreenshotTile;  // 0x298
    uint32 dwHighResScreenshotTilesPerRow;  // 0x29c
};
static_assert(sizeof(Camera) == 0x2a0, "Camera");


// Defined in X/Client/Camera/Camera.cpp.
void CameraRebuild(Camera & cam);
void CameraScreenToWorldPoint(const Camera & cam, const IVector2D & p, Vector3D & wp);
BOOL CameraWorldToScreenPoint(const Camera & cam, const Vector3D & wp, Vector3D & p);

// Functions defined in this file (bodies are not in the DWARF):
//   line    19  Camera::Camera(const Camera &)  [inlined into 1 functions]
//   line    19  Camera & Camera::operator=(const Camera &)  [0x710004a090]

// Globals defined in the corresponding original .cpp.
extern const Vector3D cgwvInitCameraForward;
extern const Vector3D cgwvInitCameraRight;
extern const Vector3D cgwvInitCameraUp;
