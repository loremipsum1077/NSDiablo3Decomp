// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/Camera/Components/CameraSettingsComponent.h
//
// CameraSettingsComponent derives from CameraComponent (size 0x38) and adds no
// members of its own. Update is the class key function (defined out-of-line at
// CameraSettingsComponent.cpp:19), so this translation unit emits the vtable
// together with the inline constructor, destructor and GetCameraComponentType
// below. Each of their bodies sits on the decl_line the DWARF records.

#pragma once

#include "CameraComponent.h"

struct CameraState;

// The base CameraComponent already supplies snoObserver and the rest.
class CameraSettingsComponent : public CameraComponent {  // size 0x38
public:
    // Trivial body; only ever inlined, so no out-of-line constructor is emitted.
    CameraSettingsComponent() {}
    virtual ~CameraSettingsComponent() {}  // vtable slot 0; D0 is `b _ZdlPv`

    virtual void Update(const CameraState & csLastFrame, CameraState & csThisFrame, float deltaSec);  // vtable slot 2, CameraSettingsComponent.cpp

    virtual CameraComponentType GetCameraComponentType() { return CAMERA_SETTINGS_COMPONENT; }  // vtable slot 4
};
static_assert(sizeof(CameraSettingsComponent) == 0x38, "CameraSettingsComponent");
