// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/Definitions/ShakeDefinition.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../InterpolationTrack.h"
#include "../../Common/Definitions/Definitions.h"
#include "../../Common/SNO/SNOFiles.h"
#include "../../Sigma/Math/MathGeneral.h"
#include "../../Sigma/SigmaTypes.h"
#include <stddef.h>
#include <stdint.h>

class XMemoryAllocatorInterface;
class XTypeDescriptor;

enum ShakeFlags {
    SHAKE_DEF_CONTINUOUS = 0,
    SHAKE_DEF_RESPECT_ORIENTATION = 1,
};

struct ShakeDefinition {  // size 0x1a0
    RequiredDefinitionHeader tHeader;  // 0x00
    uint32 dwShakeFlags;  // 0x0c
    TickTime tSystemDuration;  // 0x10
    TickTime tOutroDuration;  // 0x14
    WorldDistance wdMinRange;  // 0x18
    WorldDistance wdMaxRange;  // 0x1c
    VectorPath pathCameraPositionAmplitude;  // 0x20
    VectorPath pathCameraPositionFrequency;  // 0x50
    AnglePath pathCameraRollAmplitude;  // 0x80
    FloatPath pathCameraRollFrequency;  // 0xb0
    AnglePath pathCameraPitchAmplitude;  // 0xe0
    FloatPath pathCameraPitchFrequency;  // 0x110
    AnglePath pathCameraYawAmplitude;  // 0x140
    FloatPath pathCameraYawFrequency;  // 0x170
};
static_assert(sizeof(ShakeDefinition) == 0x1a0, "ShakeDefinition");

class ShakeGroupDef : public GroupDefinition {  // size 0xa0, line 44
public:
    ShakeGroupDef();
    GROUPDEFINITION_MEMORY_FUNCTIONS()  // line 49; the macro name is invented, see Definitions.h
    virtual uint32 GetMaxCount();  // vtable slot 11
    virtual BOOL CanReferenceOtherSNOs() const { return FALSE; }  // vtable slot 13, line 52
    virtual SNOPackType GetSNOPackDestination(const void *) const {  // vtable slot 7, line 54
        return SNOPACKTYPE_CLIENTDATA;
    }
    virtual void GetDefaultData(void * ptDefaultDef) const;  // vtable slot 21
};
static_assert(sizeof(ShakeGroupDef) == 0xa0, "ShakeGroupDef");

extern ShakeGroupDef * gptGroupDefShake;  // ShakeDefinition.cpp:52

extern const XTypeDescriptor * DT_SHAKE;  // ShakeDefinition.cpp:46

typedef ScopedDefinitionLockR<ShakeDefinition> ScopedShakeDefinitionLockR;

template <> inline const ShakeDefinition * DefinitionLockR<ShakeDefinition>(SNO sno) {  // line 75
    return (const ShakeDefinition *)gptGroupDefShake->LockR(sno, FALSE);
}
template <> inline void DefinitionUnlockR<ShakeDefinition>(const ShakeDefinition * pdef) {  // line 76
    void * lpDef = (void *)pdef;
    gptGroupDefShake->UnlockR(lpDef);
}
