// X/Client/Camera/Components/ShakeComponent.cpp
//
// Decompiled from the Diablo III 2.6.2 (Switch) debug build.
// Evidence: build/ghidra/decomp/X/Client/Camera/Components/ShakeComponent.cpp and the
// DWARF/line-attributed disassembly for _ZN14ShakeComponent6UpdateERK11CameraStateRS0_f
// (0x71003e7550). The destructor and GetCameraComponentType are inline in ShakeComponent.h.

#include "ShakeComponent.h"

#include "../CameraLocation.h"
#include "../../RWindow.h"
#include "../../Shake.h"
#include "../../../Common/Definitions/ObserverDefinition.h"

// ShakeGetCameraOffset (X/Client/Shake.cpp, relocation 0x71000d9330) is declared by its owning
// header X/Client/Shake.h, included above.

// 0x71003e7550  size 260  _ZN14ShakeComponent6UpdateERK11CameraStateRS0_f
// DWARF original prototype:
//   void Update(ShakeComponent *this, const CameraState &, CameraState & csThisFrame, float)
//
// Source lines (from the instruction/line attribution):
//   19 prologue, 20 bDisabled test, 27 ShakeGetCameraOffset, 29 lock,
//   31/32/33 scale, 35 MagnitudeSquared, 36 threshold, 38/39 +=, 41 quaternion assign, 43 epilogue.
void ShakeComponent::Update(const CameraState &, CameraState & csThisFrame, float)
{
    if (bDisabled)
        return;

    // Locals qShakeCameraRotation and wvShakeCameraOffset; the layout the original uses puts the
    // Quaternion at sp+0 and the Vector3D at sp+0x10 (the ShakeGetCameraOffset argument order).
    Vector3D wvShakeCameraOffset;
    Quaternion qShakeCameraRotation;

    // line 27: inlines GetRWindowMgr()->GetCurrentRWindow().GetShakeRWindowData()
    ShakeGetCameraOffset(GetRWindowMgr()->GetCurrentRWindow().GetShakeRWindowData(),
                         wvShakeCameraOffset, qShakeCameraRotation);

    // line 29: ScopedDefinitionLockR<ObserverDefinition>, whose ctor (Definitions.h:990) inlines
    // DefinitionLockR<ObserverDefinition> (ObserverDefinition.h:97) -> GroupDefinition::LockRFast.
    ScopedObserverDefinitionLockR pdefObserver(snoObserver);

    // lines 31/32/33: scale each component by the locked definition's camera-shake scalar
    // (X/Common/Definitions/ObserverDefinition.h:46, Vector3D vConsoleCameraShakeScalar at 0x8c).
    wvShakeCameraOffset.x *= pdefObserver->vConsoleCameraShakeScalar.x;
    wvShakeCameraOffset.y *= pdefObserver->vConsoleCameraShakeScalar.y;
    wvShakeCameraOffset.z *= pdefObserver->vConsoleCameraShakeScalar.z;

    // line 35: Vector3D::MagnitudeSquared()
    float flShakeMagSquared = wvShakeCameraOffset.MagnitudeSquared();

    // line 36: literal 1e-06 (original bytes bd378635 at 0x7100e3bea0, _ZL15s_intTypeLookup+0x60)
    if (flShakeMagSquared > 1e-06f)
    {
        // lines 38/39: Vector3D::operator+= (wvLookFrom first, then wvLookAt, as the original
        // emits them)
        csThisFrame.wvLookFrom += wvShakeCameraOffset;
        csThisFrame.wvLookAt += wvShakeCameraOffset;

        // line 41: Quaternion::operator= (inlines Vector3D::operator= for wvImag)
        csThisFrame.qPostTransformRotation = qShakeCameraRotation;
    }
}
