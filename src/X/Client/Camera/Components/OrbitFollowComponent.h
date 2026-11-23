// Generated from the DWARF by tools/gen_headers.py -- reference only.
// Original file: X/Client/Camera/Components/OrbitFollowComponent.h

#pragma once

#include "CameraComponent.h"
#include "../../../Common/SNO/SNOFiles.h"
#include "../../../Common/WorldPlace.h"

struct CameraState;

class OrbitFollowComponent : public CameraComponent {  // size 0x60
public:
    OrbitFollowComponent();
    virtual ~OrbitFollowComponent();  // vtable slot 0
    virtual void Update(const CameraState & csLastFrame, CameraState & csThisFrame, float flDeltaSec);  // vtable slot 2
    void SetOrbitPlace(const WorldPlace & wp);
    const WorldPlace GetOrbitPlace() const;
    void EnableExitTransition();
    virtual void SetObserver(SNO snoObs);  // vtable slot 3
    virtual CameraComponentType GetCameraComponentType();  // vtable slot 4
private:
    WorldPlace m_wpToFollow;  // 0x34
    float m_flOrbitSLerpScalar;  // 0x44
    float m_flTransitionTimeTotal;  // 0x48
    float m_flTransitionTimeLeft;  // 0x4c
    float m_flCurBlendWeight;  // 0x50
    float m_flCosLimitingAngle;  // 0x54
    SNO m_snoTransitionObserver;  // 0x58
};
static_assert(sizeof(OrbitFollowComponent) == 0x60, "OrbitFollowComponent");


// Functions defined in this file (bodies are not in the DWARF):
//   line    20  OrbitFollowComponent::~OrbitFollowComponent()  [0x71003e7530]
//   line    30  CameraComponentType OrbitFollowComponent::GetCameraComponentType()  [0x71003e7540]
