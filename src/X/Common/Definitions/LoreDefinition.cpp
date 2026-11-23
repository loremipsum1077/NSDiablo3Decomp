// Original file: X/Common/Definitions/LoreDefinition.cpp

#include "LoreDefinition.h"

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

// Defined in X/Common/D3TypeDescriptor/D3DescribedType.cpp; no adopted header declares it yet.
extern const XTypeDescriptor * DT_SNO;

enum {
    LORE_SNO_VERSION = 17,
    LORE_DEFINITION_VERSION = 1,
};

// The field names are "" in the binary.
static const TypeDescriptorField LoreDefinition_Fields[7] = {  // line 34
    TypeDescriptorField("", DT_INT, offsetof(LoreDefinition, dwFlags), NULL, 0x80001),
    TypeDescriptorField("", DT_INT, offsetof(LoreDefinition, eCategory), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(LoreDefinition, nXPLevel), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(LoreDefinition, nXPTier), NULL, 1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(LoreDefinition, snoConversation), "", CONVERSATION_SNO, 0x100, -1),
    TypeDescriptorField("", DT_INT, offsetof(LoreDefinition, hSignatureLabel), (const void *)-1, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(LoreDefinition), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_LoreDefinition("Lore", LORE_DEFINITION_VERSION, LoreDefinition_Fields, 7, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 41
const XTypeDescriptor * DT_LORE_DEFINITION = &TypeDescriptor_LoreDefinition;

LoreGroupDef * gptGroupDefLore;  // line 46

// line 51
LoreGroupDef::LoreGroupDef()
    : GroupDefinition("Lore", LORE_SNO, SNOVersions(LORE_SNO_VERSION), SNODATATYPE_TITLE, sizeof(LoreDefinition),
                      DT_LORE_DEFINITION, 0x4005, 14) {
}

// line 71
uint32 LoreGroupDef::GetMaxCount() {
    return 256;
}

// line 82
void LoreGroupDef::GetDefaultData(void * ptDefaultDef) const {
    LoreDefinition * pdef = (LoreDefinition *)ptDefaultDef;
    SigmaMemoryClear(pdef, sizeof(LoreDefinition));

    pdef->snoConversation = SNO_NONE;
    pdef->hSignatureLabel = (StringLabelHandle)-1;
}
