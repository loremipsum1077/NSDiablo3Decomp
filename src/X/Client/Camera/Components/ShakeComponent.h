// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/Camera/Components/ShakeComponent.h
// ShakeComponent adds no members to CameraComponent, so it stays at 0x38. The destructor and
// GetCameraComponentType bodies are inline (DWARF records them at lines 20 and 24 here); Update
// is out of line in ShakeComponent.cpp (0x71003e7550).

#pragma once

#include "CameraComponent.h"

struct CameraState;





class ShakeComponent : public CameraComponent {  // size 0x38
public:
    ShakeComponent() {}  // only ever inlined (ConsoleViewController::ConsoleViewController)
    virtual ~ShakeComponent() {}  // vtable slot 0

    virtual void Update(const CameraState &, CameraState & csThisFrame, float);  // vtable slot 2

    virtual CameraComponentType GetCameraComponentType() { return SHAKE_COMPONENT; }  // vtable slot 4
};
static_assert(sizeof(ShakeComponent) == 0x38, "ShakeComponent");
