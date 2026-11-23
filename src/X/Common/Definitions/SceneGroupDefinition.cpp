// Original file: X/Common/Definitions/SceneGroupDefinition.cpp

#include "SceneGroupDefinition.h"

// Contrib/Contrib/blz/include/blz/tuple.h, which the original reaches through an include chain
// not adopted here. Its header-level static blz::ignore sits in front of this file's own statics.
#include "../../../Contrib/Contrib/blz/include/blz/tuple.h"

// For SNONAME_NONE, cgplaceNull and the RGBAColor normals, whose initializers the original runs in
// this file's static initializer.
#include "../SNO/SNOName.h"
#include "../WorldPlace.h"
#include "../../Client/Gfx/GfxVB.h"
#include "../D3TypeDescriptor/D3TypeDescriptor.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptorField.h"

// Defined in X/Sigma/Files/XFilesCommon.cpp:20; no adopted header declares it yet.
extern const XTypeDescriptor * DT_SERIALIZEDATA;

// Defined in X/Common/D3TypeDescriptor/D3DescribedType.cpp; no adopted header declares them yet.
extern const XTypeDescriptor * DT_SNO;
extern const XTypeDescriptor * DT_GBID;

enum {
    SCENEGROUP_SNO_VERSION = 18,
    SCENE_GROUP_XML_VERSION = 1,
};

// The field names are "" in the binary.
static const TypeDescriptorField SceneGroupItem_Fields[4] = {  // line 34
    FilterTypeDescriptorField("", DT_SNO, offsetof(SceneGroupItem, snoScene), "", 0x21, 0x100, -1),
    TypeDescriptorField("", DT_INT, offsetof(SceneGroupItem, nWeight), NULL, 1),
    FilterTypeDescriptorField("", DT_GBID, offsetof(SceneGroupItem, gbidLabel), "", 11, 0, -1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(SceneGroupItem), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_SceneGroupItem("SceneGroupItem", 0, SceneGroupItem_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 38
const XTypeDescriptor * DT_SCENEGROUPITEM = &TypeDescriptor_SceneGroupItem;

static const TypeDescriptorField SceneGroupDefinition_Fields[5] = {  // line 40
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(SceneGroupDefinition, serScenes), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_SCENEGROUPITEM, offsetof(SceneGroupDefinition, arScenes), 0x21,
                                     (int32)offsetof(SceneGroupDefinition, serScenes) - (int32)offsetof(SceneGroupDefinition, arScenes)),
    VariableArrayCountTypeDescriptorField("", offsetof(SceneGroupDefinition, nSceneCnt), 0x10000, DT_SCENEGROUPITEM,
                                          (int32)offsetof(SceneGroupDefinition, serScenes) - (int32)offsetof(SceneGroupDefinition, nSceneCnt)),
    TypeDescriptorField("", DT_INT, offsetof(SceneGroupDefinition, dwFlags), NULL, 0x80001),
    TypeDescriptorField(NULL, DT_NULL, sizeof(SceneGroupDefinition), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_SceneGroupDefinition("SceneGroup", SCENE_GROUP_XML_VERSION, SceneGroupDefinition_Fields, 5, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 44
const XTypeDescriptor * DT_SCENEGROUP = &TypeDescriptor_SceneGroupDefinition;

SceneGroupGroupDef * gptGroupDefSceneGroup;  // line 50

// line 56
SceneGroupGroupDef::SceneGroupGroupDef()
    : GroupDefinition("SceneGroup", SCENEGROUP_SNO, SNOVersions(SCENEGROUP_SNO_VERSION), SNODATATYPE_TITLE,
                      sizeof(SceneGroupDefinition), DT_SCENEGROUP, 0x4005, 14) {
}

// line 77
uint32 SceneGroupGroupDef::GetMaxCount() {
    return 4;
}

// line 89
void SceneGroupGroupDef::GetDefaultData(void * ptDefaultDef) const {
    SigmaMemoryClear(ptDefaultDef, sizeof(SceneGroupDefinition));

    SceneGroupDefinition * pdef = (SceneGroupDefinition *)ptDefaultDef;
    pdef->nSceneCnt = 0;
    pdef->serScenes.dwOffset = 0;
    pdef->serScenes.dwSize = 0;
    pdef->arScenes = NULL;
}
