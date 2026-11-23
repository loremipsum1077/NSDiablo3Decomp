// Generated from the DWARF by tools/gen_headers.py -- reference only.
// Original file: X/Client/Camera/Components/SpringFollowComponent.h

#pragma once

#include "CameraComponent.h"
#include "../../../Common/WorldPlace.h"
#include "../../../Sigma/SigmaTypes.h"

struct CameraState;

class SpringFollowComponent : public CameraComponent {  // size 0x50
public:
    SpringFollowComponent();
    virtual ~SpringFollowComponent();  // vtable slot 0
    virtual void Update(const CameraState & csLastFrame, CameraState & csThisFrame, float flDeltaSec);  // vtable slot 2
    virtual void OnFirstTick();  // vtable slot 6
    virtual void OnWarped();  // vtable slot 7
    virtual CameraComponentType GetCameraComponentType();  // vtable slot 4
private:
    WorldPlace m_tLastLocation;  // 0x34
    BOOL m_bWarp;  // 0x44
    float m_flCurrentSpeed;  // 0x48
};
static_assert(sizeof(SpringFollowComponent) == 0x50, "SpringFollowComponent");


// Functions defined in this file (bodies are not in the DWARF):
//   line    20  SpringFollowComponent::~SpringFollowComponent()  [0x71003e7c40]
//   line    26  CameraComponentType SpringFollowComponent::GetCameraComponentType()  [0x71003e7c50]
