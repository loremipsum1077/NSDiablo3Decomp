// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Definitions/AccoladeDefinition.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "Definitions.h"
#include "../ImageHandle.h"
#include "../SNO/SNOFiles.h"
#include "../../Sigma/SigmaTypes.h"
#include <stddef.h>
#include <stdint.h>

class XMemoryAllocatorInterface;

struct AccoladeDefinition {  // size 0x10
    RequiredDefinitionHeader tHeader;  // 0x00
    ImageHandle hIcon;  // 0x0c
};
static_assert(sizeof(AccoladeDefinition) == 0x10, "AccoladeDefinition");

class AccoladeGroupDef : public GroupDefinition {  // size 0xa0, line 25
public:
    AccoladeGroupDef();
    GROUPDEFINITION_MEMORY_FUNCTIONS()  // line 30; the macro name is invented, see Definitions.h
    virtual uint32 GetMaxCount();  // vtable slot 11
    virtual BOOL CanReferenceOtherSNOs() const { return FALSE; }  // vtable slot 13, line 34
    virtual SNOPackType GetSNOPackDestination(const void *) const {  // vtable slot 7, line 36
        return SNOPACKTYPE_COREDATA;
    }
    virtual void GetDefaultData(void * ptDefaultDef) const;  // vtable slot 21
};
static_assert(sizeof(AccoladeGroupDef) == 0xa0, "AccoladeGroupDef");

extern const XTypeDescriptor * DT_ACCOLADEDEFINITION;  // AccoladeDefinition.cpp:36
extern AccoladeGroupDef * gptGroupDefAccolade;  // AccoladeDefinition.cpp:42
