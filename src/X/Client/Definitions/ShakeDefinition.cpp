// Original file: X/Client/Definitions/ShakeDefinition.cpp

#include "ShakeDefinition.h"

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
    SHAKE_SNO_VERSION = 20,
    SHAKE_XML_VERSION = 2,
};

static const TypeDescriptorField ShakeDefinition_Fields[14] = {  // line 32
    TypeDescriptorField("", DT_INT, offsetof(ShakeDefinition, dwShakeFlags), NULL, 0x80001),
    TypeDescriptorField("", DT_TIME, offsetof(ShakeDefinition, tSystemDuration), NULL, 1),
    TypeDescriptorField("", DT_TIME, offsetof(ShakeDefinition, tOutroDuration), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(ShakeDefinition, wdMinRange), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(ShakeDefinition, wdMaxRange), NULL, 1),
    TypeDescriptorField("", DT_VECTOR_PATH, offsetof(ShakeDefinition, pathCameraPositionAmplitude), &cgpathNullVectorPath, 1),
    TypeDescriptorField("", DT_VECTOR_PATH, offsetof(ShakeDefinition, pathCameraPositionFrequency), &cgpathNullVectorPath, 1),
    TypeDescriptorField("", DT_ANGLE_PATH, offsetof(ShakeDefinition, pathCameraRollAmplitude), &cgpathNullAnglePath, 1),
    TypeDescriptorField("", DT_FLOAT_PATH, offsetof(ShakeDefinition, pathCameraRollFrequency), &cgpathNullFloatPath, 1),
    TypeDescriptorField("", DT_ANGLE_PATH, offsetof(ShakeDefinition, pathCameraPitchAmplitude), &cgpathNullAnglePath, 1),
    TypeDescriptorField("", DT_FLOAT_PATH, offsetof(ShakeDefinition, pathCameraPitchFrequency), &cgpathNullFloatPath, 1),
    TypeDescriptorField("", DT_ANGLE_PATH, offsetof(ShakeDefinition, pathCameraYawAmplitude), &cgpathNullAnglePath, 1),
    TypeDescriptorField("", DT_FLOAT_PATH, offsetof(ShakeDefinition, pathCameraYawFrequency), &cgpathNullFloatPath, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(ShakeDefinition), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_ShakeDefinition("Shakes", SHAKE_XML_VERSION, ShakeDefinition_Fields, 14, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 46
const XTypeDescriptor * DT_SHAKE = &TypeDescriptor_ShakeDefinition;

ShakeGroupDef * gptGroupDefShake;  // line 52

// line 58
ShakeGroupDef::ShakeGroupDef()
    : GroupDefinition("Shakes", SHAKE_SNO, SNOVersions(SHAKE_SNO_VERSION), SNODATATYPE_TITLE, sizeof(ShakeDefinition),
                      DT_SHAKE, 0x2241, 0x500a) {
}

// line 83
uint32 ShakeGroupDef::GetMaxCount() {
    return 32;
}

// line 95
void ShakeGroupDef::GetDefaultData(void * ptDefaultDef) const {
    SigmaMemoryClear(ptDefaultDef, sizeof(ShakeDefinition));

    ShakeDefinition * pdef = (ShakeDefinition *)ptDefaultDef;
    pdef->dwShakeFlags = 0;
    pdef->tSystemDuration = 60;
    pdef->tOutroDuration = 30;
    pdef->wdMinRange = 20.0f;
    pdef->wdMaxRange = 40.0f;
    pdef->pathCameraPositionAmplitude = cgpathNullVectorPath;
    pdef->pathCameraPositionFrequency = cgpathNullVectorPath;
    pdef->pathCameraRollAmplitude = cgpathNullAnglePath;
    pdef->pathCameraRollFrequency = cgpathNullFloatPath;
    pdef->pathCameraPitchAmplitude = cgpathNullAnglePath;
    pdef->pathCameraPitchFrequency = cgpathNullFloatPath;
    pdef->pathCameraYawAmplitude = cgpathNullAnglePath;
    pdef->pathCameraYawFrequency = cgpathNullFloatPath;
}
