// Original file: X/Common/Definitions/TimedEventDefinition.cpp

#include "TimedEventDefinition.h"

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

// Defined in X/Common/D3TypeDescriptor/D3DescribedType.cpp; no adopted header declares it yet.
extern const XTypeDescriptor * DT_TIME;

enum {
    TIMEDEVENT_SNO_VERSION = 18,
    TIMEDEVENT_DEFINITION_VERSION = 1,
};

// The field names are "" in the binary.
static const TypeDescriptorField TimedEventDefinition_Fields[2] = {  // line 22
    TypeDescriptorField("", DT_TIME, offsetof(TimedEventDefinition, dwDuration), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(TimedEventDefinition), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_TimedEventDefinition("TimedEvent", TIMEDEVENT_DEFINITION_VERSION, TimedEventDefinition_Fields, 2, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 25
const XTypeDescriptor * DT_TIMEDEVENT_DEFINITION = &TypeDescriptor_TimedEventDefinition;

TimedEventGroupDef * gptGroupDefTimedEvent;  // line 30

TimedEventGroupDef::TimedEventGroupDef()
    : GroupDefinition("TimedEvent", TIMEDEVENT_SNO, SNOVersions(TIMEDEVENT_SNO_VERSION), SNODATATYPE_TITLE, sizeof(TimedEventDefinition),
                      DT_TIMEDEVENT_DEFINITION, 0x9005, 4) {
}

uint32 TimedEventGroupDef::GetMaxCount() {
    return 16;
}

void TimedEventGroupDef::GetDefaultData(void * ptDefaultDef) const {
    SigmaMemoryClear(ptDefaultDef, sizeof(TimedEventDefinition));
}
