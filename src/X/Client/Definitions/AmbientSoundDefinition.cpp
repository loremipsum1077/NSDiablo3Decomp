// Original file: X/Client/Definitions/AmbientSoundDefinition.cpp

#include "AmbientSoundDefinition.h"

// Contrib/Contrib/blz/include/blz/tuple.h, which the original reaches through an include chain
// not adopted here.
#include "../../../Contrib/Contrib/blz/include/blz/tuple.h"
// For SNONAME_NONE, cgplaceNull and the RGBAColor normals, whose initializers the original runs
// in this file's static initializer; it reaches them through include chains the DWARF does not record.
#include "../../Common/SNO/SNOName.h"
#include "../../Common/WorldPlace.h"
#include "../Gfx/GfxVB.h"
#include "../../Common/D3TypeDescriptor/D3TypeDescriptor.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptorField.h"
#include <stddef.h>

enum {
    AMBIENTSOUND_SNO_COMMON_VERSION = 20,
    AMBIENTSOUND_XML_VERSION = 2,
    AMBIENTSOUND_SNO_PC_VERSION = 20,
    AMBIENTSOUND_SNO_CONSOLE_VERSION = 21,
    AMBIENTSOUND_SNO_AXE_VERSION = 21,
    FIXED_INTERVAL_DEFAULT = 300,
};

static const TypeDescriptorField RandomAmbientSoundParams_Fields[6] = {  // line 43
    FilterTypeDescriptorField("", DT_SNO, offsetof(RandomAmbientSoundParams, snoSound), "", SOUND_SNO, 0, -1),
    TypeDescriptorField("", DT_FLOAT, offsetof(RandomAmbientSoundParams, flRadiusMin), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(RandomAmbientSoundParams, flRadiusDelta), NULL, 1),
    TypeDescriptorField("", DT_TIME, offsetof(RandomAmbientSoundParams, dwPeriodMin), NULL, 1),
    TypeDescriptorField("", DT_TIME, offsetof(RandomAmbientSoundParams, dwPeriodDelta), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(RandomAmbientSoundParams), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_RandomAmbientSoundParams("RandomAmbientSoundParams", 0, RandomAmbientSoundParams_Fields, 6, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 49
const XTypeDescriptor * DT_RANDOM_AMBIENT_SOUND_PARAMS = &TypeDescriptor_RandomAmbientSoundParams;

static const TypeDescriptorField AmbientSoundDefinition_Fields[13] = {  // line 51
    TypeDescriptorField("", DT_INT, offsetof(AmbientSoundDefinition, dwFlags), NULL, 0x80001),
    FilterTypeDescriptorField("", DT_SNO, offsetof(AmbientSoundDefinition, snoBackground), "", SOUND_SNO, 0, -1),
    FixedArrayTypeDescriptorField("", DT_RANDOM_AMBIENT_SOUND_PARAMS, offsetof(AmbientSoundDefinition, tRandomSounds), NULL, 1, 2, 0),
    FilterTypeDescriptorField("", DT_SNO, offsetof(AmbientSoundDefinition, snoFixedIntervalSound), "", SOUND_SNO, 0, -1),
    TypeDescriptorField("", DT_TIME, offsetof(AmbientSoundDefinition, dwIntervalOffset), NULL, 1),
    TypeDescriptorField("", DT_TIME, offsetof(AmbientSoundDefinition, dwIntervalLength), (const void *)FIXED_INTERVAL_DEFAULT, 1),
    TypeDescriptorField("", DT_INT, offsetof(AmbientSoundDefinition, dwConstraintHash), NULL, 0x700000),
    TypeDescriptorField("", DT_FLOAT, offsetof(AmbientSoundDefinition, flVolumeJointVelMin), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(AmbientSoundDefinition, flVolumeJointVelDelta), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(AmbientSoundDefinition, flPitchJointVelMin), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(AmbientSoundDefinition, flPitchJointVelDelta), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(AmbientSoundDefinition, flPitchSemitones), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(AmbientSoundDefinition), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_AmbientSoundDefinition("AmbientSound", AMBIENTSOUND_XML_VERSION, AmbientSoundDefinition_Fields, 13, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 67
const XTypeDescriptor * DT_AMBIENTSOUND = &TypeDescriptor_AmbientSoundDefinition;

AmbientSoundGroupDef * gptGroupDefAmbientSound;  // line 73

// line 79
AmbientSoundGroupDef::AmbientSoundGroupDef()
    : GroupDefinition("AmbientSound", AMBIENTSOUND_SNO,
                      SNOVersions(AMBIENTSOUND_SNO_PC_VERSION,
                                  AMBIENTSOUND_SNO_AXE_VERSION + AXE_BASE_SNO_VERSION,
                                  AMBIENTSOUND_SNO_CONSOLE_VERSION,
                                  AMBIENTSOUND_SNO_CONSOLE_VERSION,
                                  AMBIENTSOUND_SNO_CONSOLE_VERSION + NX64_BASE_SNO_VERSION),
                      SNODATATYPE_TITLE, sizeof(AmbientSoundDefinition), DT_AMBIENTSOUND, 0x2005, 8) {
}

// line 96
uint32 AmbientSoundGroupDef::GetMaxCount() {
    return 580;
}

// line 108
void AmbientSoundGroupDef::GetDefaultData(void * ptDefaultDef) const {
    SigmaMemoryClear(ptDefaultDef, sizeof(AmbientSoundDefinition));

    AmbientSoundDefinition * pdef = (AmbientSoundDefinition *)ptDefaultDef;
    pdef->snoBackground = SNO_NONE;

    for (int32 i = 0; i < 2; i++) {
        RandomAmbientSoundParams & tParams = pdef->tRandomSounds[i];
        tParams.snoSound = SNO_NONE;
        tParams.flRadiusMin = 0.0f;
        tParams.flRadiusDelta = 0.0f;
        tParams.dwPeriodMin = 180;
        tParams.dwPeriodDelta = 120;
    }

    pdef->snoFixedIntervalSound = SNO_NONE;
    pdef->dwIntervalOffset = 0;
    pdef->dwIntervalLength = FIXED_INTERVAL_DEFAULT;
}

// line 134
BOOL AmbientSoundGroupDef::Write(void * lpDefinition, FileReference & tRef) const {
    return GroupDefinition::Write(lpDefinition, tRef);
}
