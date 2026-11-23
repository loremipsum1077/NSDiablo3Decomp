// X/Client/Camera/Components/CameraSettingsComponent.cpp
//
// Reconstructed from Diablo III 2.6.2 (Switch) debug evidence (clang 5.0.1).
// Symbols emitted by this translation unit:
//   _ZN23CameraSettingsComponent6UpdateERK11CameraStateRS0_f   0x71003e6230  168 bytes
//   _ZN23CameraSettingsComponentD0Ev                           0x71003e62e0    4 bytes
//   _ZN23CameraSettingsComponent22GetCameraComponentTypeEv     0x71003e62f0    8 bytes
//
// DWARF records only one out-of-line body in this .cpp (Update, line 19). The
// deleting destructor D0 and GetCameraComponentType carry decl_file/decl_line
// CameraSettingsComponent.h:21 and CameraSettingsComponent.h:25 (their emitted
// instructions are attributed to F0 = CameraSettingsComponent.h in the line
// listing), so they are inline definitions owned by that header. They are still
// emitted out-of-line here because Update is the class key function and anchors
// the vtable in this CU. They must therefore NOT be redefined in this file.
//
// Notes on the source below (verified against the original line listing):
//   - CameraSettingsComponent.h is the DWARF owner of the class and of the inline
//     destructor (line 21) and GetCameraComponentType (line 25); it was absent from
//     src/ and has been (re)created there.
//   - ObserverDefinition's real member names are used (aFOV at 0x14, flZNear at
//     0x1c, flZFar at 0x18, wvCameraOffsetVector at 0x30, wvLookAtOffsetVector at
//     0x3c). Ghidra's flNearPlane/flFarPlane labels are misleading: the two loads
//     land in csThisFrame.flNearPlane (0x4c) from flZNear and flFarPlane (0x50)
//     from flZFar, which is the order the instructions emit.
//   - CameraState is completed through ../CameraLocation.h (fields aFOV 0x44,
//     flNearPlane 0x4c, flFarPlane 0x50, wvCameraOffsetVector 0x54,
//     wvLookAtOffsetVector 0x60, qPostTransformRotation 0x84).

#include "CameraSettingsComponent.h"

#include "../CameraLocation.h"
#include "X/Common/Definitions/Definitions.h"
#include "X/Common/Definitions/ObserverDefinition.h"
#include "X/Sigma/Math/Quaternion.h"

// 0x71003e6230  size 168  quality U
// DWARF original prototype:
//   void Update(CameraSettingsComponent * this,
//               const CameraState & csLastFrame,
//               CameraState & csThisFrame,
//               float deltaSec)
//
// Inlined chains reproduced by the source below:
//   line 28  ScopedDefinitionLockR<ObserverDefinition>::ScopedDefinitionLockR(int)
//              (Definitions.h:990) -> DefinitionLockR<ObserverDefinition>(..., int)
//              (ObserverDefinition.h:97) -> GroupDefinition::LockRFast(int)
//   line 32  Vector3D::operator=(const Vector3D &)   (Vector.inl:502)
//   line 33  Vector3D::operator=(const Vector3D &)   (Vector.inl:502)
//   line 36  Quaternion::operator=(const Quaternion &) (Quaternion.h:19)
void CameraSettingsComponent::Update(const CameraState & csLastFrame, CameraState & csThisFrame, float deltaSec)
{
    CameraComponent::Update(csLastFrame, csThisFrame, deltaSec);

    if (snoObserver != -1)
    {
        ScopedObserverDefinitionLockR pdefObserver(snoObserver);

        csThisFrame.aFOV = pdefObserver->aFOV;
        csThisFrame.flNearPlane = pdefObserver->flZNear;
        csThisFrame.flFarPlane = pdefObserver->flZFar;
        csThisFrame.wvCameraOffsetVector = pdefObserver->wvCameraOffsetVector;
        csThisFrame.wvLookAtOffsetVector = pdefObserver->wvLookAtOffsetVector;
    }

    csThisFrame.qPostTransformRotation = cgqIdentity;
}

// _ZN23CameraSettingsComponentD0Ev 0x71003e62e0 4 bytes
// Emitted from the header-inline destructor at CameraSettingsComponent.h:21:
// clang generates the deleting destructor; ~CameraComponent is trivial/empty,
// so D0 is exactly the tail call `b _ZdlPv` (operator delete).
// DO NOT define ~CameraSettingsComponent here (it is header-inline).

// _ZN23CameraSettingsComponent22GetCameraComponentTypeEv 0x71003e62f0 8 bytes
// Emitted from the header-inline body at CameraSettingsComponent.h:25 returning
// the settings camera component type (value 1): `orr w0, wzr, #0x1 ; ret`.
// DO NOT define it here (it is header-inline).
