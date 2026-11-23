// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/Camera/CameraLocation.h

#pragma once

#include "../../Sigma/DataStructures/XGrowableList.h"
#include "../../Sigma/Math/MathGeneral.h"
#include "../../Sigma/Math/Matrix.h"
#include "../../Sigma/Math/Quaternion.h"
#include "../../Sigma/Math/Rect.h"
#include "../../Sigma/Math/Transform.h"
#include "../../Sigma/Math/Vector.h"
#include "../../Sigma/SigmaTypes.h"

struct SavedCameraState {  // size 0x40
    PRTransform transform;  // 0x00
    Vector3D wvCameraOffsetVector;  // 0x1c
    Vector3D wvLookAtOffsetVector;  // 0x28
    Vector3D wvLastPOI;  // 0x34
};
static_assert(sizeof(SavedCameraState) == 0x40, "SavedCameraState");

struct CameraState {  // size 0xe4
    PRTransform transform;  // 0x00
    Rect2D rectViewport;  // 0x1c
    Vector2D vecScale;  // 0x2c
    Vector2D vecProjectionTranslate;  // 0x34
    BOOL bPerspective;  // 0x3c
    float flAspectRatio;  // 0x40
    Angle aFOV;  // 0x44
    float flHeight;  // 0x48
    float flNearPlane;  // 0x4c
    float flFarPlane;  // 0x50
    Vector3D wvCameraOffsetVector;  // 0x54
    Vector3D wvLookAtOffsetVector;  // 0x60
    Vector3D wvLookAt;  // 0x6c
    Vector3D wvLookFrom;  // 0x78
    Quaternion qPostTransformRotation;  // 0x84
    Matrix4x4 matIrrad;  // 0x94
    float flZoomFactor;  // 0xd4
    Vector3D wvLastPOI;  // 0xd8
    BOOL operator==(const CameraState & cms);
};
static_assert(sizeof(CameraState) == 0xe4, "CameraState");

typedef XGrowableList<CameraState, const CameraState &> CameraStateList;

typedef int32 SNO;  // as in SNO/SNOFiles.h

// Defined in CameraLocation.cpp.
void CameraStateSetPerspective(CameraState & cmState, const PRTransform & transform, const Angle aFOV, const float flAspectRatio, const float flNearPlane, const float flFarPlane, const Vector3D & wvCameraOffsetVector, const Vector3D & wvLookAtOffsetVector, const Vector3D & wvLookFrom, const Vector3D & wvLookAt, const Rect2D & rectViewport, const Vector2D & vecScale, const Vector2D & vecProjectionTranslate);
void CameraStateSetOrthographic(CameraState & cmState, const PRTransform & transform, const float flHeight, const float flAspectRatio, const float flNearPlane, const float flFarPlane, const Rect2D & rectViewport, const Vector2D & vecScale, const Vector2D & vecProjectionTranslate);
void CameraStateInterpolate(CameraState & cmStateNew, const CameraState & cmState0, const CameraState & cmState1, const float flT);
void CameraStateInterpolate(CameraState & cmStateNew, const CameraState & cmState, const SNO snoObserver, const float flT);


// Functions defined in this file (bodies are not in the DWARF):
//   line    27  CameraState::CameraState(const CameraState &)  [inlined into 6 functions]
//   line    27  CameraState & CameraState::operator=(const CameraState &)  [inlined into 5 functions]
