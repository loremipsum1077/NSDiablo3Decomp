#include "ActorViewController.h"

#include "CameraLocation.h"
#include "../RWindow.h"
#include "../../Sigma/ErrorManager/ErrorManager.h"
#include "../../Sigma/Math/Transform.h"

// sGetRWindow never returns in this build, so everything after the first
// sGetCameraIndex() call is unreachable and was dropped by the compiler. The
// bodies below stop there; the rest of the camera logic is not recoverable.

ActorViewController::ActorViewController() {
    m_idFollow = -1;
    m_nLastMouseWheelPos = 0;
    m_snogroupTarget = -1;
    m_wvPositionOffset = cgwvNull;
    m_aPitch = 0.0f;
    m_aYaw = 0.0f;
    m_flRadiusOffset = 0.0f;
    m_ptLast.Set(0, 0);
    m_bHoldPosition = FALSE;
    m_bHoldOrientation = FALSE;
    m_wpLastPos = cgwvNull;
    m_flT = 0.0f;
    m_flTargetT = 0.0f;
}

void ActorViewController::SetZoom(const float flZoom) {
    m_flTargetT = flZoom;
    m_flT = flZoom;
    sReset(m_wvPositionOffset);
}

RWindow * ActorViewController::sGetRWindow() const {
    DisplayInternalError(NULL, "", 89, ERROR_INTERNAL_FATAL);
    ExitProgram(ERROR_INTERNAL_FATAL);
}

inline int32 ActorViewController::sGetCameraIndex() const {
    return sGetRWindow()->GetCameraIndex();
}

inline void ActorViewController::sGetZoomedCameraState(CameraState * pcmState) {
    int32 nCameraIndex = sGetCameraIndex();
    (void)nCameraIndex;
    (void)pcmState;
}

inline void ActorViewController::sReset(const Vector3D & vecOffset) {
    CameraState cmState;
    sGetZoomedCameraState(&cmState);
    m_wvPositionOffset = vecOffset;
}

void ActorViewController::Update() {
    PRTransform transform;
    CameraState cmState;
    sGetZoomedCameraState(&cmState);
    (void)transform;
}
