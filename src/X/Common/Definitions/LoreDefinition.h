// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Definitions/LoreDefinition.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "Definitions.h"
#include "../SNO/SNOFiles.h"
#include "../../Sigma/SigmaTypes.h"

// X/Common/Labels.h (not adopted yet)
typedef uint32 StringLabelHandle;

enum {
    LOREDEF_SUPPRESS_PLAYER_BIT = 0,
};

enum LoreCategory {
    LORECAT_LATEST = -1,
    LORECAT_QUEST = 0,
    LORECAT_WORLD = 1,
    LORECAT_PEOPLE = 2,
    LORECAT_BESTIARY = 3,
    NUM_LORE_CATEGORIES = 4,
};

struct LoreDefinition {  // size 0x24
    RequiredDefinitionHeader tHeader;  // 0x00
    uint32 dwFlags;  // 0x0c
    LoreCategory eCategory;  // 0x10
    int32 nXPLevel;  // 0x14
    int32 nXPTier;  // 0x18
    SNO snoConversation;  // 0x1c
    StringLabelHandle hSignatureLabel;  // 0x20
};
static_assert(sizeof(LoreDefinition) == 0x24, "LoreDefinition");

class LoreGroupDef : public GroupDefinition {  // size 0xa0, line 53
public:
    LoreGroupDef();
    GROUPDEFINITION_MEMORY_FUNCTIONS()  // line 58; the macro name is invented, see Definitions.h
    virtual uint32 GetMaxCount();  // vtable slot 11
    virtual BOOL CanReferenceOtherSNOs() const { return TRUE; }  // vtable slot 13, line 61
    virtual SNOPackType GetSNOPackDestination(const void *) const {  // vtable slot 7, line 63
        return SNOPACKTYPE_COREDATA;
    }
    virtual void GetDefaultData(void * ptDefaultDef) const;  // vtable slot 21
};
static_assert(sizeof(LoreGroupDef) == 0xa0, "LoreGroupDef");

extern const XTypeDescriptor * DT_LORE_DEFINITION;  // LoreDefinition.cpp:41
extern LoreGroupDef * gptGroupDefLore;  // LoreDefinition.cpp:46

typedef ScopedDefinitionLockR<LoreDefinition> ScopedLoreDefinitionLockR;

// Line 103. Its callers call LockRFast and have nothing from an unlock.
template <> inline const LoreDefinition * DefinitionLockR<LoreDefinition>(SNO sno) {  // line 103
    return (const LoreDefinition *)gptGroupDefLore->LockRFast(sno);
}
template <> inline void DefinitionUnlockR<LoreDefinition>(const LoreDefinition *) {}
