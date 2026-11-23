// Generated from the DWARF by tools/gen_headers.py -- reference only.
// Original file: X/Client/Camera/Components/LeftTriggerZoomComponent.h

#pragma once

#include "CameraComponent.h"

struct CameraState;

class LeftTriggerZoomComponent : public CameraComponent {  // size 0x40
public:
    LeftTriggerZoomComponent() : m_flLeftTriggerZoom(0.0f), m_flZoomSpeed(1.0f) {}  // line 19; body from the copy inlined into ConsoleViewController::ConsoleViewController
    virtual ~LeftTriggerZoomComponent();  // vtable slot 0
    virtual void Update(const CameraState & csLastFrame, CameraState & csThisFrame, float deltaSec);  // vtable slot 2
    virtual void OnWarped();  // vtable slot 7
    virtual CameraComponentType GetCameraComponentType();  // vtable slot 4
private:
    void UpdateLeftTriggerZoom(float deltaSec);
    float CalculateLocalPlayerDistanceZoom() const;
    float FindIfAnyLocalPlayerIsInAZoomedOutScene();
    float m_flLeftTriggerZoom;  // 0x34
    float m_flZoomSpeed;  // 0x38
};
static_assert(sizeof(LeftTriggerZoomComponent) == 0x40, "LeftTriggerZoomComponent");


// Functions defined in this file (bodies are not in the DWARF):
//   line    19  LeftTriggerZoomComponent::LeftTriggerZoomComponent()  [inlined into 1 functions]
//   line    20  LeftTriggerZoomComponent::~LeftTriggerZoomComponent()  [0x71003e6a10]
//   line    26  CameraComponentType LeftTriggerZoomComponent::GetCameraComponentType()  [0x71003e6a20]
