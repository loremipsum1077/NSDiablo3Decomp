// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/Shake.h

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "AttachmentParent.h"
#include "Gfx/GfxCommon.h"
#include "../Common/SNO/SNOFiles.h"
#include "../Sigma/Math/Quaternion.h"
#include "../Sigma/Math/Transform.h"
#include "../Sigma/Math/Vector.h"
#include "../Sigma/SigmaTypes.h"

enum {
    SHAKEID_INVALID = -1,
};

typedef int32 ShakeID;

struct Shake {  // size 0x94
    ShakeID id;  // 0x00
    SNO snoShake;  // 0x04
    RenderContext eContext;  // 0x08
    float flTime;  // 0x0c
    BOOL bOutro;  // 0x10
    float flOutroTime;  // 0x14
    float flOutroCrossFade;  // 0x18
    PRTransform transform;  // 0x1c
    Vector3D wvCameraPositionAmplitudeInterp;  // 0x38
    Vector3D wvCameraPositionFrequencyInterp;  // 0x44
    float flCameraRollAmplitudeInterp;  // 0x50
    float flCameraRollFrequencyInterp;  // 0x54
    float flCameraPitchAmplitudeInterp;  // 0x58
    float flCameraPitchFrequencyInterp;  // 0x5c
    float flCameraYawAmplitudeInterp;  // 0x60
    float flCameraYawFrequencyInterp;  // 0x64
    ParentData tParentData;  // 0x68
};
static_assert(sizeof(Shake) == 0x94, "Shake");

struct ShakeRWindowData;

// Defined in X/Client/Shake.cpp; the DWARF gives the signatures but not which header declared them.
void ShakeDeleteAll(const RenderContext eContext);  // line 131
void ShakeDelete(ShakeID idShake);  // line 199

// Defined in X/Client/Shake.cpp (0x71000d9330); owned here so callers such as
// X/Client/Camera/Components/ShakeComponent.cpp get its declaration instead of a local prototype.
// DWARF: void ShakeGetCameraOffset(const ShakeRWindowData & tShakeRWindowData,
//                                  Vector3D & wvCameraOffset, Quaternion & qCameraRotation)  line 530.
void ShakeGetCameraOffset(const ShakeRWindowData & tShakeRWindowData, Vector3D & wvCameraOffset,
                          Quaternion & qCameraRotation);


// Functions defined in this file (bodies are not in the DWARF):
//   line    27  Shake::Shake()  [inlined into 1 functions]
