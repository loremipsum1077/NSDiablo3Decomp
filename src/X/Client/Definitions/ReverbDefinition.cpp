// Original file: X/Client/Definitions/ReverbDefinition.cpp

#include "ReverbDefinition.h"

// For cgplaceNull, whose initializer the original runs in this file's static initializer.
#include "../../Common/WorldPlace.h"
// For the RGBAColor normals, which the original also initializes here; it reaches GfxVB.h
// through an include chain the DWARF does not record.
#include "../Gfx/GfxVB.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptorField.h"
#include <stddef.h>

// Contrib/Contrib/blz/include/blz/tuple.h, which the original reaches through an include chain
// not adopted here. Its header-level static blz::ignore sits in front of this file's own statics.
#include "../../../Contrib/Contrib/blz/include/blz/tuple.h"

enum {
    REVERB_COMMON_SNO_VERSION = 15,
    REVERB_PC_SNO_VERSION = 15,
    REVERB_CONSOLE_SNO_VERSION = 16,
    REVERB_AXE_SNO_VERSION = 16,
    REVERB_XBOXONE_SNO_VERSION = 16,
    REVERB_PS4_SNO_VERSION = 16,
    REVERB_NX64_SNO_VERSION = 16,
    REVERB_XML_VERSION = 0,
    REVERB_PARAMS_XML_VERSION = 2,
};

static const TypeDescriptorField ReverbParams_Fields[23] = {  // line 50
    TypeDescriptorField("", DT_INT, offsetof(ReverbParams, dwEnvironment), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(ReverbParams, flEnvironmentSize), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(ReverbParams, flEnvironmentDiffusion), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ReverbParams, nRoom), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ReverbParams, nRoomHF), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ReverbParams, nRoomLF), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(ReverbParams, flDecayTime), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(ReverbParams, flDecayHFRatio), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(ReverbParams, flDecayLFRatio), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ReverbParams, nReflections), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(ReverbParams, flReflectionsDelay), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ReverbParams, nReverb), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(ReverbParams, flReverbDelay), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(ReverbParams, flEchoTime), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(ReverbParams, flEchoDepth), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(ReverbParams, flModulationTime), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(ReverbParams, flModulationDepth), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(ReverbParams, flAirAbsorptionHF), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(ReverbParams, flHFReference), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(ReverbParams, flLFReference), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(ReverbParams, flRoomRolloffFactor), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ReverbParams, dwFlags), (const void *)0x3f, 0x80001),
    TypeDescriptorField(NULL, DT_NULL, sizeof(ReverbParams), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_ReverbParams("ReverbParams", REVERB_PARAMS_XML_VERSION, ReverbParams_Fields, 23, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 73
const XTypeDescriptor * DT_REVERBPARAMS = &TypeDescriptor_ReverbParams;

static const TypeDescriptorField ReverbDefinition_Fields[3] = {  // line 75
    TypeDescriptorField("", DT_REVERBPARAMS, offsetof(ReverbDefinition, tParams), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ReverbDefinition, dwPad), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(ReverbDefinition), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_ReverbDefinition("Reverb", REVERB_XML_VERSION, ReverbDefinition_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 79
const XTypeDescriptor * DT_REVERBDEFINITION = &TypeDescriptor_ReverbDefinition;

ReverbGroupDef * gptGroupDefReverb;  // line 85

// line 91
ReverbGroupDef::ReverbGroupDef()
    : GroupDefinition("Reverb", REVERB_SNO,
                      SNOVersions(REVERB_PC_SNO_VERSION,
                                  REVERB_AXE_SNO_VERSION + AXE_BASE_SNO_VERSION,
                                  REVERB_XBOXONE_SNO_VERSION,
                                  REVERB_PS4_SNO_VERSION,
                                  REVERB_NX64_SNO_VERSION + NX64_BASE_SNO_VERSION),
                      SNODATATYPE_TITLE, sizeof(ReverbDefinition), DT_REVERBDEFINITION, 0x2005, 8) {
}

// line 108
uint32 ReverbGroupDef::GetMaxCount() {
    return 32;
}

// line 120
void ReverbGroupDef::GetDefaultData(void * ptDefaultDef) const {
    SigmaMemoryClear(ptDefaultDef, sizeof(ReverbDefinition));

    ReverbDefinition * pdef = (ReverbDefinition *)ptDefaultDef;
    ReverbParams & tReverb = pdef->tParams;

    tReverb.dwEnvironment = 17;
    tReverb.flEnvironmentSize = 17.7324f;
    tReverb.flEnvironmentDiffusion = 1.0f;
    tReverb.nRoom = -641;
    tReverb.nRoomHF = -100;
    tReverb.nRoomLF = 0;
    tReverb.flDecayTime = 1.49f;
    tReverb.flDecayHFRatio = 0.83f;
    tReverb.flDecayLFRatio = 1.0f;
    tReverb.nReflections = -2602;
    tReverb.flReflectionsDelay = 0.011f;
    tReverb.nReverb = -2648;
    tReverb.flReverbDelay = 0.011f;
    tReverb.flEchoTime = 0.25f;
    tReverb.flEchoDepth = 0.0f;
    tReverb.flModulationTime = 0.25f;
    tReverb.flModulationDepth = 0.0f;
    tReverb.flAirAbsorptionHF = -23.2394f;
    tReverb.flHFReference = 5000.0f;
    tReverb.flLFReference = 250.0f;
    tReverb.flRoomRolloffFactor = 3.4507f;
}
