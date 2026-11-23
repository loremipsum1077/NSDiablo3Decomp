// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/Camera/ActorViewController.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Common/SNO/SNOFiles.h"
#include "../../Common/ThreadGlobals.h"
#include "../../Sigma/Math/MathGeneral.h"
#include "../../Sigma/Math/Vector.h"
#include "../../Sigma/SigmaTypes.h"

struct CameraState;
struct PRTransform;
class RWindow;

class ActorViewController {  // size 0x48
public:
    ActorViewController();
    ~ActorViewController() {}  // empty and inline: ~Cinematography calls no destructor for it
    float GetZoom() const;
    void SetZoom(const float flZoom);
    void SetFollow(const DataID, const SNOGroup, const Vector3D &, const BOOL);
    void Update();
    void HoldOrientation(BOOL);
    void HoldPosition(BOOL);
    DataID m_idFollow;  // 0x00
    int32 m_nLastMouseWheelPos;  // 0x04
private:
    void sReset(const Vector3D & vecOffset);
    BOOL sUpdateEverything(const CameraState &);
    RWindow * sGetRWindow() const;
    int32 sGetCameraIndex() const;
    void sGetZoomedCameraState(CameraState * pcmState);
    void sUpdateTargetPosition();
    void sRecomputePosition(PRTransform &, const CameraState &);
    SNOGroup m_snogroupTarget;  // 0x08
    Vector3D m_wvPositionOffset;  // 0x0c
    Angle m_aPitch;  // 0x18
    Angle m_aYaw;  // 0x1c
    float m_flRadiusOffset;  // 0x20
    IVector2D m_ptLast;  // 0x24
    BOOL m_bHoldPosition;  // 0x2c
    BOOL m_bHoldOrientation;  // 0x30
    Vector3D m_wpLastPos;  // 0x34
    float m_flT;  // 0x40
    float m_flTargetT;  // 0x44
};
static_assert(sizeof(ActorViewController) == 0x48, "ActorViewController");


// Functions defined in this file (bodies are not in the DWARF):
//   line    17  ActorViewController & ActorViewController::operator=(const ActorViewController &)  [inline]
