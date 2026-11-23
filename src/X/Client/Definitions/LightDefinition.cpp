// Original file: X/Client/Definitions/LightDefinition.cpp
// Blank lines keep the original line numbers.

#include "LightDefinition.h"

// Contrib/Contrib/blz/include/blz/tuple.h, which the original reaches through an include chain
// not adopted here. Its header-level static blz::ignore sits in front of this file's own statics.
#include "../../../Contrib/Contrib/blz/include/blz/tuple.h"

// For SNONAME_NONE, cgplaceNull and the RGBAColor normals, whose initializers the original runs in
// this file's static initializer.
#include "../../Common/SNO/SNOName.h"
#include "../../Common/WorldPlace.h"
#include "../Gfx/GfxVB.h"
#include "../../Common/D3TypeDescriptor/D3TypeDescriptor.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptorField.h"

// Defined in X/Client/Gfx/GfxMaterial.cpp:33 and :56; no adopted header declares them yet.
extern const MaterialTexture cgtMatTexDefault;
extern const XTypeDescriptor * DT_MATERIALTEXTURE;

enum {
    LIGHT_XML_VERSION = 5,
    LIGHT_COMMON_SNO_VERSION = 58,
    LIGHT_PC_SNO_VERSION = 58,
    LIGHT_CONSOLE_SNO_VERSION = 60,
    LIGHT_AXE_SNO_VERSION = 60,
    LIGHT_XBOXONE_SNO_VERSION = 60,
    LIGHT_PS4_SNO_VERSION = 60,
    LIGHT_NX64_SNO_VERSION = 60,
};

static const TickTime cnDefaultDuration = 600;  // line 33

// The field names are "" in the binary.






static const TypeDescriptorField LightStateParams_Fields[12] = { TypeDescriptorField("", DT_FLOAT_PATH, offsetof(LightStateParams, pathInnerRadius), &cgpathNullFloatPath, 1),
    TypeDescriptorField("", DT_FLOAT_PATH, offsetof(LightStateParams, pathRadius), &cgpathNullFloatPath, 1),
    FixedArrayTypeDescriptorField("", DT_FLOAT_PATH, offsetof(LightStateParams, pathAttenuation), NULL, 1, 3, 0),
    TypeDescriptorField("", DT_ANGLE_PATH, offsetof(LightStateParams, pathHotspot), &cgpathNullAnglePath, 1),
    TypeDescriptorField("", DT_ANGLE_PATH, offsetof(LightStateParams, pathFalloff), &cgpathNullAnglePath, 1),
    TypeDescriptorField("", DT_COLOR_PATH, offsetof(LightStateParams, pathAmbientColor), &cgpathNullColorPath, 1),
    TypeDescriptorField("", DT_FLOAT_PATH, offsetof(LightStateParams, pathAmbientIntensity), &cgpathNullFloatPath, 1),
    TypeDescriptorField("", DT_COLOR_PATH, offsetof(LightStateParams, pathColor), &cgpathNullColorPath, 1),
    TypeDescriptorField("", DT_FLOAT_PATH, offsetof(LightStateParams, pathIntensity), &cgpathNullFloatPath, 1),
    TypeDescriptorField("", DT_COLOR_PATH, offsetof(LightStateParams, pathSpecularColor), &cgpathNullColorPath, 1),
    TypeDescriptorField("", DT_FLOAT_PATH, offsetof(LightStateParams, pathSpecularIntensity), &cgpathNullFloatPath, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(LightStateParams), NULL, 0),};

ComplexTypeDescriptor TypeDescriptor_LightStateParams("LightStateParams", 0, LightStateParams_Fields, 12, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_LIGHTSTATEPARAMS = &TypeDescriptor_LightStateParams;

static const TypeDescriptorField LightDefinition_Fields[10] = { TypeDescriptorField("", DT_INT, offsetof(LightDefinition, dwFlags), NULL, 0x80001),
    TypeDescriptorField("", DT_INT, offsetof(LightDefinition, dwIgnoreActorTypeFlags), NULL, 0x80001),
    TypeDescriptorField("", DT_INT, offsetof(LightDefinition, eType), NULL, 1),
    TypeDescriptorField("", DT_MATERIALTEXTURE, offsetof(LightDefinition, tMatTexProjected), &cgtMatTexDefault, 1),
    TypeDescriptorField("", DT_TIME, offsetof(LightDefinition, nSystemDuration), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(LightDefinition, nPriority), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(LightDefinition, flMaxPlayerDist), NULL, 1),
    TypeDescriptorField("", DT_LIGHTSTATEPARAMS, offsetof(LightDefinition, tStateA), NULL, 1),
    TypeDescriptorField("", DT_LIGHTSTATEPARAMS, offsetof(LightDefinition, tStateB), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(LightDefinition), NULL, 0),};




ComplexTypeDescriptor TypeDescriptor_LightDefinition("Light", LIGHT_XML_VERSION, LightDefinition_Fields, 10, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_LIGHT = &TypeDescriptor_LightDefinition;





LightGroupDef * gptGroupDefLight;  // line 79





LightGroupDef::LightGroupDef()
    : GroupDefinition("Light", LIGHT_SNO, SNOVersions(LIGHT_PC_SNO_VERSION, 62, LIGHT_CONSOLE_SNO_VERSION, LIGHT_CONSOLE_SNO_VERSION, 61),
                      SNODATATYPE_TITLE, sizeof(LightDefinition), DT_LIGHT, 0x2241, 0x500e) {
}






















uint32 LightGroupDef::GetMaxCount() {
    return 256;
}









void LightStateParams::GetDefaultData() {
    pathInnerRadius = cgpathNullFloatPath;
    pathRadius = cgpathNullFloatPath;
    for (int32 i = 0; i < 3; i++) {
        pathAttenuation[i] = cgpathNullFloatPath;
    }
    pathAttenuation[1].SetConstantValue(1.0f);
    pathHotspot = cgpathNullAnglePath;
    pathFalloff = cgpathNullAnglePath;
    pathColor = cgpathNullColorPath;
    pathIntensity = cgpathNullFloatPath;
    pathSpecularColor = cgpathNullColorPath;
    pathSpecularIntensity = cgpathNullFloatPath;
    pathAmbientColor = cgpathNullColorPath;
    pathAmbientIntensity = cgpathNullFloatPath;
}












void LightGroupDef::GetDefaultData(void * ptDefaultDef) const {
    SigmaMemoryClear(ptDefaultDef, sizeof(LightDefinition));

    LightDefinition * pdef = (LightDefinition *)ptDefaultDef;
    pdef->dwFlags = 1;
    pdef->eType = LIGHTTYPE_POINT;
    pdef->nSystemDuration = cnDefaultDuration;
    pdef->tMatTexProjected = cgtMatTexDefault;
    pdef->tStateA.GetDefaultData();
    pdef->tStateB.GetDefaultData();
}








BOOL LightGroupDef::Write(void * lpDefinition, FileReference & tRef) const {
    return GroupDefinition::Write(lpDefinition, tRef);
}
