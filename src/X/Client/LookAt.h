// Minimal layout adopted from X/Client/LookAt.h DWARF.
#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "Animation.h"
#include "../Common/CommonTypes.h"

struct PRSTransform;
struct RActor;

class LookAt : public AnimationController {  // size 0x38
public:
    LookAt(RActorID);
    virtual ~LookAt();  // vtable slot 0
    void SetTarget(RActorID idRActor);
    RActorID GetTarget();
    virtual void ComputeBone(RActor * ptRActor, PRSTransform & transform, const PRSTransform & parentTransform, const PRSTransform & relativeTransform, const PRSTransform & defaultRelativeTransform, int32 nBoneIndex, float flDeltaTime);  // vtable slot 2
private:
    float SeekToTarget(float flCurrent, float flTarget, float flSpeed);
    void ComputeRelativeTransform(PRSTransform & relativeTransform);
    RActorID m_idRActor;  // 0x08
    RActorID m_idTargetRActor;  // 0x0c
    int32 m_nBoneIndex;  // 0x10
    float m_flRange;  // 0x14
    float m_flYaw;  // 0x18
    float m_flYawMax;  // 0x1c
    float m_flPitch;  // 0x20
    float m_flPitchMin;  // 0x24
    float m_flPitchMax;  // 0x28
    float m_flPitchOffset;  // 0x2c
    float m_flSpeed;  // 0x30
    float m_flUnitBlend;  // 0x34
};
static_assert(sizeof(LookAt) == 0x38, "LookAt");

inline RActorID LookAt::GetTarget() {  // line 38
    return m_idTargetRActor;
}

inline void LookAt::SetTarget(RActorID idRActor) {  // line 73
    m_idTargetRActor = idRActor;
}
