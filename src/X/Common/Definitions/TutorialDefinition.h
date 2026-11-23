// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Definitions/TutorialDefinition.h
// Subset: the group definition class and its global, which X/Common/Game.cpp registers
// (GroupDefinition::Create<TutorialGroupDef>). The definition struct itself is not adopted yet.

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../X/Common/Definitions/Definitions.h"
#include "../../../X/Common/SNO/SNOFiles.h"
#include "../../../X/Sigma/SigmaTypes.h"
#include "../../../X/Sigma/Math/Vector.h"
#include "StringListDefinition.h"

class XMemoryAllocatorInterface;

struct TutorialDefinition {  // size 0x30
    RequiredDefinitionHeader tHeader;  // 0x00
    uint32 dwFlags;  // 0x0c
    StringLabelHandle hLabel;  // 0x10
    uint32 dwTimeBeforeFading;  // 0x14
    uint32 dwRecurTime;  // 0x18
    int32 nOccurrencesUntilLearned;  // 0x1c
    uint32 eArrowPosition;  // 0x20
    IVector2D vecOffset;  // 0x24
    uint32 dwPad;  // 0x2c
};
static_assert(sizeof(TutorialDefinition) == 0x30, "TutorialDefinition");

class TutorialGroupDef : public GroupDefinition {  // size 0xa0
public:
    TutorialGroupDef();
    GROUPDEFINITION_MEMORY_FUNCTIONS()  // vtable slots 2-5; the macro name is invented, see Definitions.h
    virtual uint32 GetMaxCount();  // vtable slot 11
    virtual BOOL CanReferenceOtherSNOs() const { return FALSE; }  // vtable slot 13
    virtual SNOPackType GetSNOPackDestination(const void *) const {
        return SNOPACKTYPE_COREDATA;
    }  // vtable slot 7
    virtual void GetDefaultData(void * ptDefaultDef) const;  // vtable slot 21
};
static_assert(sizeof(TutorialGroupDef) == 0xa0, "TutorialGroupDef");

extern TutorialGroupDef * gptGroupDefTutorial;  // TutorialDefinition.cpp

typedef ScopedDefinitionLockR<TutorialDefinition> ScopedTutorialDefinitionLockR;

template <> inline const TutorialDefinition * DefinitionLockR<TutorialDefinition>(SNO sno) {  // line 89
    return (const TutorialDefinition *)gptGroupDefTutorial->LockRFast(sno);
}
template <> inline void DefinitionUnlockR<TutorialDefinition>(const TutorialDefinition *) {}
