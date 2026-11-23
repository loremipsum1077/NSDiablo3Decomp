// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/Definitions/AmbientSoundDefinition.h
// Subset: the definition structs, the group definition class and its global.

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../X/Common/Definitions/Definitions.h"
#include "../../../X/Common/SNO/SNOFiles.h"
#include "../../../X/Sigma/SigmaTypes.h"

class XMemoryAllocatorInterface;

enum {
    AMBIENTSOUND_CONTROLLED_BY_JOINT_VELOCITY_BIT = 0,
};

struct RandomAmbientSoundParams {  // size 0x14
    SNO snoSound;  // 0x00
    float flRadiusMin;  // 0x04
    float flRadiusDelta;  // 0x08
    uint32 dwPeriodMin;  // 0x0c
    uint32 dwPeriodDelta;  // 0x10
};
static_assert(sizeof(RandomAmbientSoundParams) == 0x14, "RandomAmbientSoundParams");

struct AmbientSoundDefinition {  // size 0x60
    RequiredDefinitionHeader tHeader;  // 0x00
    uint32 dwFlags;  // 0x0c
    SNO snoBackground;  // 0x10
    RandomAmbientSoundParams tRandomSounds[2];  // 0x14
    SNO snoFixedIntervalSound;  // 0x3c
    uint32 dwIntervalOffset;  // 0x40
    uint32 dwIntervalLength;  // 0x44
    uint32 dwConstraintHash;  // 0x48
    float flVolumeJointVelMin;  // 0x4c
    float flVolumeJointVelDelta;  // 0x50
    float flPitchJointVelMin;  // 0x54
    float flPitchJointVelDelta;  // 0x58
    float flPitchSemitones;  // 0x5c
};
static_assert(sizeof(AmbientSoundDefinition) == 0x60, "AmbientSoundDefinition");

class AmbientSoundGroupDef : public GroupDefinition {  // size 0xa0
public:
    AmbientSoundGroupDef();
    GROUPDEFINITION_MEMORY_FUNCTIONS()  // vtable slots 2-5; the macro name is invented, see Definitions.h
    virtual uint32 GetMaxCount();  // vtable slot 11
    virtual BOOL CanReferenceOtherSNOs() const { return TRUE; }  // vtable slot 13
    virtual SNOPackType GetSNOPackDestination(const void *) const { return SNOPACKTYPE_CLIENTDATA; }  // vtable slot 7
    virtual BOOL Write(void * lpDefinition, FileReference & tRef) const;  // vtable slot 19
    virtual void GetDefaultData(void * ptDefaultDef) const;  // vtable slot 21
};
static_assert(sizeof(AmbientSoundGroupDef) == 0xa0, "AmbientSoundGroupDef");

extern AmbientSoundGroupDef * gptGroupDefAmbientSound;  // AmbientSoundDefinition.cpp

typedef ScopedDefinitionLockR<AmbientSoundDefinition> ScopedAmbientSoundDefinitionLockR;

// Line 103. LockRFast, as the copy inlined into AmbientSoundMarker::Draw shows; the unlock is empty.
template <> inline const AmbientSoundDefinition * DefinitionLockR<AmbientSoundDefinition>(SNO sno) {
    return (const AmbientSoundDefinition *)gptGroupDefAmbientSound->LockRFast(sno);
}
template <> inline void DefinitionUnlockR<AmbientSoundDefinition>(const AmbientSoundDefinition *) {}
