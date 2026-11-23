// Generated from the DWARF by tools/gen_headers.py -- reference only.
// Original file: X/Client/Camera/FixedViewController.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "CameraManager.h"
#include "../../Common/CommonTypes.h"
#include "../../Common/SNO/SNOFiles.h"
#include "../../Sigma/Math/MathGeneral.h"
#include "../../Sigma/Math/PointSpherical.h"
#include "../../Sigma/Math/Vector.h"
#include "../../Sigma/SigmaTypes.h"

class RWindow;

class FixedViewController {  // size 0xb0
    // vtable pointer
public:
    FixedViewController();
    virtual ~FixedViewController() {}  // vtable slot 0; line 26
    float GetZoom() const;
    void SetDefaultZoom(const float flZoom);
    void SetZoom(const float flZoom);
    void SetYaw(Angle aAngle, BOOL bSnap, float flDuration);
    void SetFollow(const RActorID idRActorToFollow, const float flRadiusOffset);
    RActorID GetFocus();
    void SetFocus(const RActorID idRActorToFocus, BOOL bSnapFocus, float flDuration);
    void ClearFocus(BOOL bSnap, float flDuration);
    void SetScriptedZoom(const float flZoom, const BOOL bSnap, const float flZoomDuration);
    void OffsetClear();
    void SetWheelPosition(const int32 nWheelPosition);
    virtual void Update(float flDeltaTime, const BOOL fForceUpdateOfAllParams);  // vtable slot 2
    void SetDefaultZoomSpeed(const float flZoomSpeed);
    void SetZoomSpeed(const float flZoomSpeed);
    virtual void OnWarped() {}  // vtable slot 3; line 80
    virtual void SetLookAt(const Vector3D &, const SWorldID) {}  // vtable slot 4; line 82
    virtual ViewControllerType GetViewControllerType() { return FIXED_VIEW_CONTROLLER; }  // vtable slot 5; line 85
    SNO GetObserver();
    void SetObserver(const SNO snoObserver) { m_snoObserver = snoObserver; }  // line 88
protected:
    float m_flCurrentZoom;  // 0x08
    float m_flDefaultT;  // 0x0c
    float m_flZoomSpeedDefault;  // 0x10
    float m_flZoomSpeedOverride;  // 0x14
    float m_flOverrideT;  // 0x18
    float m_flT;  // 0x1c
private:
    RWindow * sGetRWindow() const;
    BOOL sKeyDown();
    RActorID m_idFollow;  // 0x20
    RActorID m_idFocus;  // 0x24
    RActorID m_idFocusPrevious;  // 0x28
    float m_flFocusT;  // 0x2c
    float m_flTargetFocusT;  // 0x30
    float m_flFocusReleaseT;  // 0x34
    float m_flFocusDuration;  // 0x38
    float m_flFocusReleaseDuration;  // 0x3c
    float m_flFocusLastUpdate;  // 0x40
    uint32 m_bSnapFocus;  // 0x44
    Angle m_aDesiredYaw;  // 0x48
    Angle m_aCurrentYaw;  // 0x4c
    Angle m_aPrevYaw;  // 0x50
    float m_flYawT;  // 0x54
    float m_flYawDuration;  // 0x58
    float m_flYawLastUpdate;  // 0x5c
    float m_flDesiredZoom;  // 0x60
    float m_flPreviousZoom;  // 0x64
    float m_flZoomT;  // 0x68
    float m_flTargetZoomT;  // 0x6c
    float m_flZoomReleaseT;  // 0x70
    float m_flZoomDuration;  // 0x74
    float m_flZoomReleaseDuration;  // 0x78
    float m_flZoomLastUpdate;  // 0x7c
    float m_flRadiusOffsetDesired;  // 0x80
    float m_flRadiusOffsetCurrent;  // 0x84
    float m_flRadiusOffsetPrev;  // 0x88
    Vector3D m_wpLastActorLocation;  // 0x8c
    int32 m_nLastMouseWheelPos;  // 0x98
    PointSpherical m_psCameraOffset;  // 0x9c
    float m_flTargetOffsetZ;  // 0xa8
    SNO m_snoObserver;  // 0xac
};
static_assert(sizeof(FixedViewController) == 0xb0, "FixedViewController");


// Functions defined in this file (bodies are not in the DWARF):
//   line    26  FixedViewController::~FixedViewController()  [0x71001a3e50 (+1 more)]
//   line    80  void FixedViewController::OnWarped()  [0x71001a3e70]
//   line    82  void FixedViewController::SetLookAt(const Vector3D &, const SWorldID)  [0x71001a3e80]
//   line    85  ViewControllerType FixedViewController::GetViewControllerType()  [0x71001a3e90]
//   line    88  void FixedViewController::SetObserver(const SNO snoObserver)  [inlined into 1 functions]
