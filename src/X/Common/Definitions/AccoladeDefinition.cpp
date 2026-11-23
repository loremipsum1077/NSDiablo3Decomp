// Original file: X/Common/Definitions/AccoladeDefinition.cpp

#include "AccoladeDefinition.h"

// Contrib/Contrib/blz/include/blz/tuple.h, which the original reaches through an include chain
// not adopted here. Its header-level static blz::ignore sits in front of this file's own statics.
#include "../../../Contrib/Contrib/blz/include/blz/tuple.h"

// For SNONAME_NONE, cgplaceNull and the RGBAColor normals, whose initializers the original runs in
// this file's static initializer.
#include "../SNO/SNOName.h"
#include "../WorldPlace.h"
#include "../../Client/Gfx/GfxVB.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptorField.h"

enum {
    ACCOLADE_XML_VERSION = 1,
    ACCOLADE_SNO_VERSION = 15,
};

// The field names are "" in the binary.
static const TypeDescriptorField AccoladeDefinition_Fields[2] = {  // line 34
    TypeDescriptorField("", DT_INT, offsetof(AccoladeDefinition, hIcon), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(AccoladeDefinition), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_AccoladeDefinition("Accolade", ACCOLADE_XML_VERSION, AccoladeDefinition_Fields, 2, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 36
const XTypeDescriptor * DT_ACCOLADEDEFINITION = &TypeDescriptor_AccoladeDefinition;

AccoladeGroupDef * gptGroupDefAccolade;  // line 42

// line 48
AccoladeGroupDef::AccoladeGroupDef()
    : GroupDefinition("Accolade", ACCOLADE_SNO, SNOVersions(ACCOLADE_SNO_VERSION), SNODATATYPE_TITLE, sizeof(AccoladeDefinition),
                      DT_ACCOLADEDEFINITION, 0x4005, 14) {
}

// line 70
uint32 AccoladeGroupDef::GetMaxCount() {
    return 180;
}

// line 82
void AccoladeGroupDef::GetDefaultData(void * ptDefaultDef) const {
    AccoladeDefinition * pdef = (AccoladeDefinition *)ptDefaultDef;
    SigmaMemoryClear(pdef, sizeof(AccoladeDefinition));

    pdef->hIcon = 0;
}
