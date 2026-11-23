// Original file: X/Common/Definitions/TutorialDefinition.cpp

#include "TutorialDefinition.h"

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

enum {
    TUTORIAL_XML_VERSION = 2,
    TUTORIAL_COMMON_SNO_VERSION = 20,
    TUTORIAL_PC_SNO_VERSION = 20,
    TUTORIAL_CONSOLE_SNO_VERSION = 23,
    TUTORIAL_AXE_SNO_VERSION = 24,
};

// The field names are "" in the binary.
static const TypeDescriptorField TutorialDefinition_Fields[9] = {  // line 38
    TypeDescriptorField("", DT_INT, offsetof(TutorialDefinition, dwFlags), NULL, 0x80001),
    TypeDescriptorField("", DT_INT, offsetof(TutorialDefinition, hLabel), (const void *)-1, 1),
    TypeDescriptorField("", DT_TIME, offsetof(TutorialDefinition, dwTimeBeforeFading), NULL, 1),
    TypeDescriptorField("", DT_TIME, offsetof(TutorialDefinition, dwRecurTime), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(TutorialDefinition, nOccurrencesUntilLearned), (const void *)1, 1),
    TypeDescriptorField("", DT_INT, offsetof(TutorialDefinition, eArrowPosition), NULL, 1),
    TypeDescriptorField("", DT_IVECTOR2D, offsetof(TutorialDefinition, vecOffset), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(TutorialDefinition, dwPad), NULL, 0),
    TypeDescriptorField(NULL, DT_NULL, sizeof(TutorialDefinition), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_TutorialDefinition("Tutorial", TUTORIAL_XML_VERSION, TutorialDefinition_Fields, 9, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 48
const XTypeDescriptor * DT_TUTORIALDEFINITION = &TypeDescriptor_TutorialDefinition;

TutorialGroupDef * gptGroupDefTutorial;  // line 54

// line 60
TutorialGroupDef::TutorialGroupDef()
    : GroupDefinition("Tutorial", TUTORIAL_SNO,
                      SNOVersions(TUTORIAL_COMMON_SNO_VERSION, 27, TUTORIAL_CONSOLE_SNO_VERSION, TUTORIAL_CONSOLE_SNO_VERSION, TUTORIAL_AXE_SNO_VERSION),
                      SNODATATYPE_TITLE, sizeof(TutorialDefinition), DT_TUTORIALDEFINITION, 0x4005, 14) {
}

// line 82
uint32 TutorialGroupDef::GetMaxCount() {
    return 128;
}

// line 94
void TutorialGroupDef::GetDefaultData(void * ptDefaultDef) const {
    TutorialDefinition * pdef = (TutorialDefinition *)ptDefaultDef;
    SigmaMemoryClear(pdef, sizeof(TutorialDefinition));

    pdef->hLabel = (StringLabelHandle)-1;
}
