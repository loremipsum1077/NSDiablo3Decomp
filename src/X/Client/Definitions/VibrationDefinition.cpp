// Original file: X/Client/Definitions/VibrationDefinition.cpp

#include "VibrationDefinition.h"

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
    VIBRATION_XML_VERSION = 2,
    VIBRATION_SNO_VERSION = 18,
};

static const TypeDescriptorField VibrationDefinition_Fields[9] = {  // line 33
    TypeDescriptorField("", DT_INT, offsetof(VibrationDefinition, dwFlags), NULL, 0x80001),
    TypeDescriptorField("", DT_TIME, offsetof(VibrationDefinition, tSystemDuration), NULL, 1),
    TypeDescriptorField("", DT_TIME, offsetof(VibrationDefinition, tOutroDuration), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(VibrationDefinition, wdMinRange), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(VibrationDefinition, wdMaxRange), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(VibrationDefinition, flPriority), NULL, 1),
    TypeDescriptorField("", DT_FLOAT_PATH, offsetof(VibrationDefinition, primaryVibrationIntensityCurve), &cgpathNullFloatPath, 1),
    TypeDescriptorField("", DT_FLOAT_PATH, offsetof(VibrationDefinition, outroVibrationIntensityCurve), &cgpathNullFloatPath, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(VibrationDefinition), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_VibrationDefinition("Vibrations", VIBRATION_XML_VERSION, VibrationDefinition_Fields, 9, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 42
const XTypeDescriptor * DT_VIBRATION = &TypeDescriptor_VibrationDefinition;

VibrationGroupDef * gptGroupDefVibration;  // line 46

// line 52
VibrationGroupDef::VibrationGroupDef()
    : GroupDefinition("Vibrations", VIBRATION_SNO, SNOVersions(VIBRATION_SNO_VERSION, 20, VIBRATION_SNO_VERSION, VIBRATION_SNO_VERSION, 19),
                      SNODATATYPE_TITLE, sizeof(VibrationDefinition), DT_VIBRATION, 0x2005, 8) {
}

// line 67
uint32 VibrationGroupDef::GetMaxCount() {
    return 16;
}

// line 77
void VibrationGroupDef::GetDefaultData(void * ptDefaultDef) const {
    SigmaMemoryClear(ptDefaultDef, sizeof(VibrationDefinition));

    VibrationDefinition * pdef = (VibrationDefinition *)ptDefaultDef;
    pdef->dwFlags = 0;
    pdef->tSystemDuration = 60;
    pdef->tOutroDuration = 30;
    pdef->wdMinRange = 20.0f;
    pdef->wdMaxRange = 40.0f;
    pdef->flPriority = 0.0f;
    pdef->outroVibrationIntensityCurve = cgpathNullFloatPath;
    pdef->primaryVibrationIntensityCurve = cgpathNullFloatPath;
}
