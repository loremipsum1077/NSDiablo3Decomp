// Minimal layout adopted from X/Client/BonePulse.h DWARF.
#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "Animation.h"
#include "../Common/CommonTypes.h"

struct BonePulseData;
struct PRSTransform;
struct RActor;

class BonePulse : public AnimationController {  // size 0x20
public:
    BonePulse(RActorID, const BonePulseData *);
    virtual ~BonePulse();  // vtable slot 0
    virtual void ComputeBone(RActor * ptRActor, PRSTransform & transform, const PRSTransform & parentTransform, const PRSTransform & relativeTransform, const PRSTransform & defaultRelativeTransform, int32 nBoneIndex, float flDeltaTime);  // vtable slot 2
private:
    RActorID m_idRActor;  // 0x08
    int32 m_nBoneIndex;  // 0x0c
    float m_flAmplitude;  // 0x10
    float m_flOmega;  // 0x14
    float m_flPhase;  // 0x18
    float m_flTime;  // 0x1c
};
static_assert(sizeof(BonePulse) == 0x20, "BonePulse");
