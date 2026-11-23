// Original file: X/Client/Definitions/EffectGroupDefinition.cpp

#include "EffectGroupDefinition.h"

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
    EFFECTGROUP_SNO_COMMON_VERSION = 47,
    EFFECTGROUP_XML_VERSION = 2,
    EFFECTGROUP_SNO_PC_VERSION = 47,
    EFFECTGROUP_SNO_CONSOLE_VERSION = 50,
    EFFECTGROUP_SNO_AXE_VERSION = 58,
};

static const TypeDescriptorField EffectItem_Fields[4] = {  // line 41
    TypeDescriptorField("", DT_INT, offsetof(EffectItem, dwWeight), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(EffectItem, dwHash), NULL, 0x700000),
    TypeDescriptorField("", DT_MSGTRIGGEREDEVENT, offsetof(EffectItem, tMsgTriggeredEvents), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(EffectItem), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_EffectItem("EffectItem", 0, EffectItem_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 46
const XTypeDescriptor * DT_EFFECT_ITEM = &TypeDescriptor_EffectItem;

static const TypeDescriptorField EffectGroupDefinition_Fields[10] = {  // line 48
    TypeDescriptorField("", DT_INT, offsetof(EffectGroupDefinition, dwEffectFlags), NULL, 0x80001),
    TypeDescriptorField("", DT_INT, offsetof(EffectGroupDefinition, nEffectWeightMethod), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(EffectGroupDefinition, nMinToPick), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(EffectGroupDefinition, nMaxToPick), NULL, 1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(EffectGroupDefinition, snoPower), "", POWER_SNO, 0, -1),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(EffectGroupDefinition, serEffects), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_EFFECT_ITEM, offsetof(EffectGroupDefinition, ptEffects), 0x21,
                                     (int32)offsetof(EffectGroupDefinition, serEffects) - (int32)offsetof(EffectGroupDefinition, ptEffects)),
    VariableArrayCountTypeDescriptorField("", offsetof(EffectGroupDefinition, nEffectCount), 0x10000, DT_EFFECT_ITEM,
                                          (int32)offsetof(EffectGroupDefinition, serEffects) - (int32)offsetof(EffectGroupDefinition, nEffectCount)),
    FixedArrayTypeDescriptorField("", DT_INT, offsetof(EffectGroupDefinition, fPlayedBit), NULL, 1, 16, 0),
    TypeDescriptorField(NULL, DT_NULL, sizeof(EffectGroupDefinition), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_EffectGroupDefinition("EffectGroup", EFFECTGROUP_XML_VERSION, EffectGroupDefinition_Fields, 10, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 58
const XTypeDescriptor * DT_EFFECTGROUP = &TypeDescriptor_EffectGroupDefinition;

EffectGroupGroupDef * gptGroupDefEffectGroup;  // line 64

// line 70
EffectGroupGroupDef::EffectGroupGroupDef()
    : GroupDefinition("EffectGroup", EFFECTGROUP_SNO,
                      SNOVersions(EFFECTGROUP_SNO_PC_VERSION,
                                  EFFECTGROUP_SNO_AXE_VERSION + AXE_BASE_SNO_VERSION,
                                  EFFECTGROUP_SNO_CONSOLE_VERSION,
                                  EFFECTGROUP_SNO_CONSOLE_VERSION,
                                  EFFECTGROUP_SNO_CONSOLE_VERSION + NX64_BASE_SNO_VERSION),
                      SNODATATYPE_TITLE, sizeof(EffectGroupDefinition), DT_EFFECTGROUP, 0x2041, 0x500a) {
}

// line 94
uint32 EffectGroupGroupDef::GetMaxCount() {
    return 128;
}

// line 106
void EffectGroupGroupDef::GetDefaultData(void * ptDefaultDef) const {
    SigmaMemoryClear(ptDefaultDef, sizeof(EffectGroupDefinition));

    EffectGroupDefinition * pdef = (EffectGroupDefinition *)ptDefaultDef;
    pdef->nEffectWeightMethod = EFFECT_WEIGHT_PICK_ALL;
}

// line 118
BOOL EffectGroupGroupDef::Write(void * lpDefinition, FileReference & tRef) const {
    return GroupDefinition::Write(lpDefinition, tRef);
}
