// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/Definitions/ReverbDefinition.h
// X/Common/Game.cpp registers the group definition (GroupDefinition::Create<ReverbGroupDef>).

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../X/Common/Definitions/Definitions.h"
#include "../../../X/Common/SNO/SNOFiles.h"
#include "../../../X/Sigma/SigmaTypes.h"
#include <stddef.h>
#include <stdint.h>

class XMemoryAllocatorInterface;
class XTypeDescriptor;

struct ReverbParams {  // size 0x58
    uint32 dwEnvironment;  // 0x00
    float flEnvironmentSize;  // 0x04
    float flEnvironmentDiffusion;  // 0x08
    int32 nRoom;  // 0x0c
    int32 nRoomHF;  // 0x10
    int32 nRoomLF;  // 0x14
    float flDecayTime;  // 0x18
    float flDecayHFRatio;  // 0x1c
    float flDecayLFRatio;  // 0x20
    int32 nReflections;  // 0x24
    float flReflectionsDelay;  // 0x28
    int32 nReverb;  // 0x2c
    float flReverbDelay;  // 0x30
    float flEchoTime;  // 0x34
    float flEchoDepth;  // 0x38
    float flModulationTime;  // 0x3c
    float flModulationDepth;  // 0x40
    float flAirAbsorptionHF;  // 0x44
    float flHFReference;  // 0x48
    float flLFReference;  // 0x4c
    float flRoomRolloffFactor;  // 0x50
    uint32 dwFlags;  // 0x54
};
static_assert(sizeof(ReverbParams) == 0x58, "ReverbParams");

struct ReverbParamsExtended {  // size 0x14
    float flConvertedHighCut;  // 0x00
    float flConvertedEarlyLateMix;  // 0x04
    float flConvertedWetLevel;  // 0x08
    float flDiffusion;  // 0x0c
    float flDensity;  // 0x10
};
static_assert(sizeof(ReverbParamsExtended) == 0x14, "ReverbParamsExtended");

struct ReverbDefinition {  // size 0x68
    RequiredDefinitionHeader tHeader;  // 0x00
    ReverbParams tParams;  // 0x0c
    uint32 dwPad;  // 0x64
};
static_assert(sizeof(ReverbDefinition) == 0x68, "ReverbDefinition");

class ReverbGroupDef : public GroupDefinition {  // size 0xa0
public:
    ReverbGroupDef();
    GROUPDEFINITION_MEMORY_FUNCTIONS()  // vtable slots 2-5; the macro name is invented, see Definitions.h
    virtual uint32 GetMaxCount();  // vtable slot 11
    virtual BOOL CanReferenceOtherSNOs() const { return FALSE; }  // vtable slot 13
    virtual SNOPackType GetSNOPackDestination(const void *) const {  // vtable slot 7
        return SNOPACKTYPE_CLIENTDATA;
    }
    virtual void GetDefaultData(void * ptDefaultDef) const;  // vtable slot 21
};
static_assert(sizeof(ReverbGroupDef) == 0xa0, "ReverbGroupDef");

extern ReverbGroupDef * gptGroupDefReverb;  // ReverbDefinition.cpp:85

extern const XTypeDescriptor * DT_REVERBPARAMS;  // ReverbDefinition.cpp:73
extern const XTypeDescriptor * DT_REVERBDEFINITION;  // ReverbDefinition.cpp:79

typedef ScopedDefinitionLockR<ReverbDefinition> ScopedReverbDefinitionLockR;

// ReverbDefinition.h:117 per DWARF; the original inlines LockRFast with no matching unlock call.
template <> inline const ReverbDefinition * DefinitionLockR<ReverbDefinition>(SNO sno) {
    return (const ReverbDefinition *)gptGroupDefReverb->LockRFast(sno);
}
template <> inline void DefinitionUnlockR<ReverbDefinition>(const ReverbDefinition *) {}
