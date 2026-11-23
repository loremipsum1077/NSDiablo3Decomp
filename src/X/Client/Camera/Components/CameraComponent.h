// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/Camera/Components/CameraComponent.h
// Members sit on their DWARF decl_line. The in-class bodies are the binary's out-of-line copies
// at 0x71003e6140..0x71003e61a0 (CameraComponent.cpp's CU) and the copies inlined into
// SpringFollowComponent::OnFirstTick and ConsoleViewController::ForceClearOrbitComponent.

#pragma once

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Common/SNO/SNOFiles.h"
#include "../../../Common/WorldPlace.h"
#include "../../../Sigma/SigmaTypes.h"

struct CameraState;


typedef int32 CameraComponentType; enum {
    INVALID_CAMERA_COMPONENT = -1,
    BASE_CAMERA_COMPONENT = 0,
    CAMERA_SETTINGS_COMPONENT = 1,
    FIXED_VIEW_COMPONENT = 2,
    LEFT_TRIGGER_ZOOM_COMPONENT = 3,
    ORBIT_FOLLOW_COMPONENT = 4,
    SHAKE_COMPONENT = 5,
    SPRING_FOLLOW_COMPONENT = 6,
    NUM_CAMERA_COMPONENTS = 7,
};



class CameraComponent {  // size 0x38
    // vtable pointer
public:
    CameraComponent();
    virtual ~CameraComponent() {}  // vtable slot 0

    virtual void Update(const CameraState &, CameraState &, float);  // vtable slot 2, CameraComponent.cpp
    virtual void SetObserver(SNO snoObs) { snoObserver = snoObs; }  // vtable slot 3
    virtual CameraComponentType GetCameraComponentType() { return BASE_CAMERA_COMPONENT; }  // vtable slot 4
    void SetDisabled(BOOL bValue) { bDisabled = bValue; }

    virtual void OnAddedToStack() {}  // vtable slot 5
    virtual void OnFirstTick() { bRanFirstTick = TRUE; }  // vtable slot 6
    virtual void OnWarped() {}  // vtable slot 7

protected:






    void ProcessCentroidLocation();
    WorldPlace GetCentroidLocation() const { return m_wpCentroidLocation; }

    SNO snoObserver;  // 0x08
    BOOL bDisabled;  // 0x0c
    BOOL bForceAllPlayersForCentroid;  // 0x10

    uint32 m_nActivePlayers;  // 0x14
    uint32 m_nMovingPlayers;  // 0x18
private:

    BOOL bRanFirstTick;  // 0x1c
    BOOL m_bUseInactiveCentroid;  // 0x20
    WorldPlace m_wpCentroidLocation;  // 0x24
};
static_assert(sizeof(CameraComponent) == 0x38, "CameraComponent");
