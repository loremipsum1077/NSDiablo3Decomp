// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/Definitions/VibrationDefinition.h
// X/Common/Game.cpp registers the group definition (GroupDefinition::Create<VibrationGroupDef>).

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../X/Common/Definitions/Definitions.h"
#include "../../../X/Common/SNO/SNOFiles.h"
#include "../../../X/Sigma/SigmaTypes.h"
#include "../InterpolationTrack.h"
#include "../../Sigma/Math/MathGeneral.h"
#include <stddef.h>
#include <stdint.h>

class XMemoryAllocatorInterface;
class XTypeDescriptor;

enum {
    VIBRATION_DEF_CONTINUOUS = 0,
    VIBRATION_DEF_ALL_PLAYERS = 1,
};

struct VibrationDefinition {  // size 0x88
    RequiredDefinitionHeader tHeader;  // 0x00
    uint32 dwFlags;  // 0x0c
    TickTime tSystemDuration;  // 0x10
    TickTime tOutroDuration;  // 0x14
    WorldDistance wdMinRange;  // 0x18
    WorldDistance wdMaxRange;  // 0x1c
    float flPriority;  // 0x20
    FloatPath primaryVibrationIntensityCurve;  // 0x28
    FloatPath outroVibrationIntensityCurve;  // 0x58
};
static_assert(sizeof(VibrationDefinition) == 0x88, "VibrationDefinition");

class VibrationGroupDef : public GroupDefinition {  // size 0xa0, line 44
public:
    VibrationGroupDef();
    GROUPDEFINITION_MEMORY_FUNCTIONS()  // line 49; the macro name is invented, see Definitions.h
    virtual uint32 GetMaxCount();  // vtable slot 11
    virtual BOOL CanReferenceOtherSNOs() const { return FALSE; }  // vtable slot 13, line 52
    virtual SNOPackType GetSNOPackDestination(const void *) const {  // vtable slot 7, line 54
        return SNOPACKTYPE_CLIENTDATA;
    }
    virtual void GetDefaultData(void * ptDefaultDef) const;  // vtable slot 21
};
static_assert(sizeof(VibrationGroupDef) == 0xa0, "VibrationGroupDef");

extern VibrationGroupDef * gptGroupDefVibration;  // VibrationDefinition.cpp:46

extern const XTypeDescriptor * DT_VIBRATION;  // VibrationDefinition.cpp:42

typedef ScopedDefinitionLockR<VibrationDefinition> ScopedVibrationDefinitionLockR;

// Line 81. LockRFast, as the copies inlined into X/Client/Vibration.cpp show; the unlock is empty.
template <> inline const VibrationDefinition * DefinitionLockR<VibrationDefinition>(SNO sno) {
    return (const VibrationDefinition *)gptGroupDefVibration->LockRFast(sno);
}
template <> inline void DefinitionUnlockR<VibrationDefinition>(const VibrationDefinition *) {}
