// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Definitions/AccountDefinition.h
// Subset: the group definition class and its global, which X/Common/Game.cpp registers
// (GroupDefinition::Create<AccountGroupDef>). The definition struct itself is not adopted yet.

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../X/Common/Definitions/Definitions.h"
#include "../../../X/Common/SNO/SNOFiles.h"
#include "../../../X/Sigma/SigmaTypes.h"

class XMemoryAllocatorInterface;

enum AccountSerializeMode {
    ASM_ALL = 0,
    ASM_ACCOUNT_ONLY = 1,
    ASM_PARITION_ONLY = 2,
};

class AccountGroupDef : public GroupDefinition {  // size 0xa0
public:
    AccountGroupDef();
    GROUPDEFINITION_MEMORY_FUNCTIONS()  // vtable slots 2-5; the macro name is invented, see Definitions.h
    virtual uint32 GetMaxCount();  // vtable slot 11
    virtual BOOL CanReferenceOtherSNOs() const { return FALSE; }  // vtable slot 13, line 56
    virtual SNOPackType GetSNOPackDestination(const void *) const { return SNOPACKTYPE_INVALID; }  // vtable slot 7, line 61
    virtual void GetDefaultData(void * ptDefaultDef) const;  // vtable slot 21
};
static_assert(sizeof(AccountGroupDef) == 0xa0, "AccountGroupDef");

extern AccountGroupDef * gptGroupDefAccount;  // AccountDefinition.cpp
