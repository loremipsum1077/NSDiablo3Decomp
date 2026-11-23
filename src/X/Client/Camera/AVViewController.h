// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/Camera/AVViewController.h

#pragma once

#include "../../Sigma/Math/MathGeneral.h"

class AVViewController {  // size 0x44
public:
    AVViewController();
    ~AVViewController() {}  // empty and inline: ~Cinematography calls no destructor for it
    void Reset();
    void ResetTurret();
    void ResetFOV();
    void ResetRoll();
    void Update();
private:
    float m_flTForward;  // 0x00
    float m_flTBackward;  // 0x04
    float m_flTStrafeLeft;  // 0x08
    float m_flTStrafeRight;  // 0x0c
    float m_flTUp;  // 0x10
    float m_flTDown;  // 0x14
    float m_flTPitchUp;  // 0x18
    float m_flTPitchDown;  // 0x1c
    float m_flTYawLeft;  // 0x20
    float m_flTYawRight;  // 0x24
    float m_flTRollLeft;  // 0x28
    float m_flTRollRight;  // 0x2c
    float m_flTTurretLeft;  // 0x30
    float m_flTTurretRight;  // 0x34
    float m_flTFOVUp;  // 0x38
    float m_flTFOVDown;  // 0x3c
    Angle m_aTurret;  // 0x40
};
static_assert(sizeof(AVViewController) == 0x44, "AVViewController");

