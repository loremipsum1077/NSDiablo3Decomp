// Original file: X/Common/Definitions/AnimTreeDefinition.cpp

#include "AnimTreeDefinition.h"

// Contrib/Contrib/blz/include/blz/tuple.h, which the original reaches through an include chain
// not adopted here. Its header-level static blz::ignore sits in front of this file's own statics.
#include "../../../Contrib/Contrib/blz/include/blz/tuple.h"

// For SNONAME_NONE, cgplaceNull and the RGBAColor normals, whose initializers the original runs in
// this file's static initializer.
#include "../SNO/SNOName.h"
#include "../WorldPlace.h"
#include "../../Client/Gfx/GfxVB.h"
#include "../D3TypeDescriptor/D3TypeDescriptor.h"
#include "../../Sigma/ErrorManager/ErrorManager.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptorField.h"

// Defined in X/Sigma/Files/XFilesCommon.cpp:20; no adopted header declares it yet.
extern const XTypeDescriptor * DT_SERIALIZEDATA;

// Defined in X/Common/D3TypeDescriptor/D3DescribedType.cpp; no adopted header declares it yet.
extern const XTypeDescriptor * DT_SNO;

// Defined in X/Common/SNO/SymbolicValue.cpp:4278, :4293, :4317 and :4326; no adopted header
// declares them yet.
extern const SymbolicEntry gAnimTreeNodeTypes[9];
extern const SymbolicEntry gAnimTreeControlParam[18];
extern const SymbolicEntry gAnimTreeUpdateModes[3];
extern const SymbolicEntry gAnimBoneWeightSets[4];

enum {
    ANIMTREE_SNO_VERSION = 30,
    ANIMTREE_XML_VERSION = 1,
};

// The field names are "" in the binary.
static const TypeDescriptorField AnimTreeLayerName_Fields[2] = {  // line 40
    TypeDescriptorField("", DT_INT, offsetof(AnimTreeLayerName, dwHash), NULL, 0),
    TypeDescriptorField(NULL, DT_NULL, sizeof(AnimTreeLayerName), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_AnimTreeLayerName("AnimTreeLayerName", 0, AnimTreeLayerName_Fields, 2, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 43
const XTypeDescriptor * DT_ANIMTREELAYERNAME = &TypeDescriptor_AnimTreeLayerName;

static const TypeDescriptorField AnimTreeBlendRamp_Fields[7] = {  // line 45
    TypeDescriptorField("", DT_FLOAT, offsetof(AnimTreeBlendRamp, flRampInTimeSec), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(AnimTreeBlendRamp, flRampOutTimeSec), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(AnimTreeBlendRamp, flDurationSec), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(AnimTreeBlendRamp, flBlendStart), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(AnimTreeBlendRamp, flBlendMid), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(AnimTreeBlendRamp, flBlendEnd), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(AnimTreeBlendRamp), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_AnimTreeBlendRamp("AnimTreeBlendRamp", 0, AnimTreeBlendRamp_Fields, 7, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 52
const XTypeDescriptor * DT_ANIMTREEBLENDRAMP = &TypeDescriptor_AnimTreeBlendRamp;

static const TypeDescriptorField AnimTreeBlendCase_Fields[4] = {  // line 54
    EnumTypeDescriptorField("", offsetof(AnimTreeBlendCase, eParam), NULL, gAnimTreeControlParam),
    TypeDescriptorField("", DT_FLOAT, offsetof(AnimTreeBlendCase, flLiteral), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(AnimTreeBlendCase, nLiteral), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(AnimTreeBlendCase), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_AnimTreeBlendCase("AnimTreeBlendCase", 0, AnimTreeBlendCase_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 58
const XTypeDescriptor * DT_ANIMTREEBLENDCASE = &TypeDescriptor_AnimTreeBlendCase;

static const TypeDescriptorField AnimTreeLeaf_Fields[10] = {  // line 60
    EnumTypeDescriptorField("", offsetof(AnimTreeLeaf, eNodeType), NULL, gAnimTreeNodeTypes),
    FilterTypeDescriptorField("", DT_SNO, offsetof(AnimTreeLeaf, snoAnim), "", ANIMATION_SNO, 0, -1),
    TypeDescriptorField("", DT_INT, offsetof(AnimTreeLeaf, dwTagAnimation), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(AnimTreeLeaf, flPoseFrame), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(AnimTreeLeaf, flPlaySpeed), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(AnimTreeLeaf, nSyncGroupIndex), NULL, 1),
    EnumTypeDescriptorField("", offsetof(AnimTreeLeaf, eUpdateMode), NULL, gAnimTreeUpdateModes),
    EnumTypeDescriptorField("", offsetof(AnimTreeLeaf, eBoneWeightSet), NULL, gAnimBoneWeightSets),
    TypeDescriptorField("", DT_ANIMTREELAYERNAME, offsetof(AnimTreeLeaf, tLayerName), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(AnimTreeLeaf), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_AnimTreeLeaf("AnimTreeLeaf", 0, AnimTreeLeaf_Fields, 10, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 70
const XTypeDescriptor * DT_ANIMTREELEAF = &TypeDescriptor_AnimTreeLeaf;

static const TypeDescriptorField AnimTreeNode_Fields[9] = {  // line 72
    EnumTypeDescriptorField("", offsetof(AnimTreeNode, eNodeType), NULL, gAnimTreeNodeTypes),
    TypeDescriptorField("", DT_INT, offsetof(AnimTreeNode, dwNumChildren), NULL, 1),
    FixedArrayTypeDescriptorField("", DT_INT, offsetof(AnimTreeNode, pdwChildren), NULL, 1, ATN_MAX_CHILDREN, 0),
    EnumTypeDescriptorField("", offsetof(AnimTreeNode, eBlendParam), NULL, gAnimTreeControlParam),
    FixedArrayTypeDescriptorField("", DT_ANIMTREEBLENDCASE, offsetof(AnimTreeNode, tBlendCase), NULL, 1, ATN_MAX_BLEND_CASES, 0),
    TypeDescriptorField("", DT_ANIMTREEBLENDRAMP, offsetof(AnimTreeNode, tBlendRamp), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(AnimTreeNode, nSyncGroupIndex), NULL, 1),
    TypeDescriptorField("", DT_ANIMTREELAYERNAME, offsetof(AnimTreeNode, tLayerName), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(AnimTreeNode), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_AnimTreeNode("AnimTreeNode", 0, AnimTreeNode_Fields, 9, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 81
const XTypeDescriptor * DT_ANIMTREENODE = &TypeDescriptor_AnimTreeNode;

static const TypeDescriptorField AnimTreeDefinition_Fields[13] = {  // line 83
    TypeDescriptorField("", DT_INT, offsetof(AnimTreeDefinition, dwMachineTime), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(AnimTreeDefinition, dwFlags), NULL, 0x80001),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(AnimTreeDefinition, serLeafTable), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_ANIMTREELEAF, offsetof(AnimTreeDefinition, ptLeaf), 0x21,
                                     (int32)offsetof(AnimTreeDefinition, serLeafTable) - (int32)offsetof(AnimTreeDefinition, ptLeaf)),
    VariableArrayCountTypeDescriptorField("", offsetof(AnimTreeDefinition, dwNumLeafs), 0x10000, DT_ANIMTREELEAF,
                                          (int32)offsetof(AnimTreeDefinition, serLeafTable) - (int32)offsetof(AnimTreeDefinition, dwNumLeafs)),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(AnimTreeDefinition, serNodeTable), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_ANIMTREENODE, offsetof(AnimTreeDefinition, ptNode), 0x21,
                                     (int32)offsetof(AnimTreeDefinition, serNodeTable) - (int32)offsetof(AnimTreeDefinition, ptNode)),
    VariableArrayCountTypeDescriptorField("", offsetof(AnimTreeDefinition, dwNumNodes), 0x10000, DT_ANIMTREENODE,
                                          (int32)offsetof(AnimTreeDefinition, serNodeTable) - (int32)offsetof(AnimTreeDefinition, dwNumNodes)),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(AnimTreeDefinition, serSyncGroupTable), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_INT, offsetof(AnimTreeDefinition, ptSyncGroup), 0x21,
                                     (int32)offsetof(AnimTreeDefinition, serSyncGroupTable) - (int32)offsetof(AnimTreeDefinition, ptSyncGroup)),
    VariableArrayCountTypeDescriptorField("", offsetof(AnimTreeDefinition, dwNumSyncGroups), 0x10000, DT_INT,
                                          (int32)offsetof(AnimTreeDefinition, serSyncGroupTable) - (int32)offsetof(AnimTreeDefinition, dwNumSyncGroups)),
    TypeDescriptorField("", DT_INT, offsetof(AnimTreeDefinition, dwRootNode), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(AnimTreeDefinition), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_AnimTreeDefinition("AnimTree", ANIMTREE_XML_VERSION, AnimTreeDefinition_Fields, 13, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 95
const XTypeDescriptor * DT_ANIMTREE = &TypeDescriptor_AnimTreeDefinition;

AnimTreeGroupDef * gptGroupDefAnimTree;  // line 101

// line 107
AnimTreeGroupDef::AnimTreeGroupDef()
    : GroupDefinition("AnimTree", ANIMTREE_SNO, SNOVersions(ANIMTREE_SNO_VERSION, 32, ANIMTREE_SNO_VERSION, ANIMTREE_SNO_VERSION, 31),
                      SNODATATYPE_TITLE, sizeof(AnimTreeDefinition), DT_ANIMTREE, 0x4005, 0x4e) {
}

// line 129
uint32 AnimTreeGroupDef::GetMaxCount() {
    return 256;
}

// line 190
BOOL AnimTreeGroupDef::Write(void * lpDefinition, FileReference & tRef) const {
    DisplayInternalError(NULL, "", 196, ERROR_INTERNAL_FATAL);
    ExitProgram(ERROR_INTERNAL_FATAL);
}

// line 205
void AnimTreeGroupDef::Postprocess(void * lpDefinition) {
    GroupDefinition::Postprocess(lpDefinition);
}

// line 214
void AnimTreeGroupDef::Delete(void * lpDefinition) const {
    GroupDefinition::Delete(lpDefinition);
}

// line 223
void AnimTreeGroupDef::GetDefaultData(void * ptDefaultDef) const {
    SigmaMemoryClear(ptDefaultDef, sizeof(AnimTreeDefinition));
}
