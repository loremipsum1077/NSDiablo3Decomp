// Generated from the DWARF by tools/gen_headers.py -- reference only.
// Original file: X/Client/Battlenet/Subsystems/FrontendBackgroundCameraManager.h

#pragma once

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Sigma/Unicode.h"

enum {
    CAM_POS_NONE = -1,
    CAM_POS_CLOSE = 0,
    CAM_POS_WIDE = 1,
};

typedef int32 BackgroundCameraPosition;

class FrontendBackgroundCameraManager {  // size 0x10
public:
    FrontendBackgroundCameraManager();
    BackgroundCameraPosition GetBackgroundCameraPosition() const  // line 29
    {
        return m_eCameraPosition;
    }
    BackgroundCameraPosition GetTargetBackgroundCameraPosition() const  // line 34
    {
        return m_eCameraPositionTarget;
    }
    void TransitionBackgroundCamera(BackgroundCameraPosition eTargetCameraPosition, const float flTransitionDurationInSeconds);
    void UpdateBackgroundCamera();
    static LPCSTR GetCameraPositionHardpoint(BackgroundCameraPosition eCameraPosition, const float flAspectRatio);
private:
    BackgroundCameraPosition m_eCameraPosition;  // 0x00
    BackgroundCameraPosition m_eCameraPositionTarget;  // 0x04
    float m_flCameraTransitionElapsed;  // 0x08
    float m_flCameraTransitionTotal;  // 0x0c
};
static_assert(sizeof(FrontendBackgroundCameraManager) == 0x10, "FrontendBackgroundCameraManager");


// Functions defined in this file (bodies are not in the DWARF):
//   line    29  BackgroundCameraPosition FrontendBackgroundCameraManager::GetBackgroundCameraPosition() const  [inlined into 4 functions]
//   line    34  BackgroundCameraPosition FrontendBackgroundCameraManager::GetTargetBackgroundCameraPosition() const  [inlined into 4 functions]
