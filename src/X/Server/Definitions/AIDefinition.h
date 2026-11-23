// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Server/Definitions/AIDefinition.h
// Subset: the group definition class and its global, which X/Common/Game.cpp registers
// (GroupDefinition::Create<AIStateGroupDef, AIBehaviorGroupDef>). The definition struct itself is not adopted yet.

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../X/Common/Definitions/Definitions.h"
#include "../../../X/Common/SNO/SNOFiles.h"
#include "../../../X/Sigma/SigmaTypes.h"

#include "../../../X/Common/AI/ActorAIStatesSwig.h"
#include "../../../X/Sigma/DataStructures/XPtr64.h"
#include "../../../X/Sigma/Files/XSerializeData.h"

typedef int32 GBID;  // X/Common/GameBalance.h

class XMemoryAllocatorInterface;

static const int32 AISTATE_MAX_CONDITION_PARAMETERS = 4;  // line 23
static const int32 AISTATE_MAX_CONDITIONS_PER_ENTRY = 5;  // line 24

struct AICondition {  // size 0x1c
    AIStateConditionType eCondition;  // 0x00
    AIStateTargetType eConditionTargetType;  // 0x04
    float flConditionParm[AISTATE_MAX_CONDITION_PARAMETERS];  // 0x08
    SNO snoPower;  // 0x18
};
static_assert(sizeof(AICondition) == 0x1c, "AICondition");

struct AIEntry {  // size 0xb8
    uint32 dwPriority;  // 0x00
    uint32 dwCoolUpMin;  // 0x04
    uint32 dwCoolUpDelta;  // 0x08
    uint32 dwWeight;  // 0x0c
    AICondition arCondition[AISTATE_MAX_CONDITIONS_PER_ENTRY];  // 0x10
    SNO snoPower;  // 0x9c
    GBID gbidMovementStyle;  // 0xa0
    AIStateTargetType ePowerTargetType;  // 0xa4
    uint32 dwCoolDownMin;  // 0xa8
    uint32 dwCoolDownDelta;  // 0xac
    AICoolDownType eCoolDown;  // 0xb0
    AIStateType eStateSwitch;  // 0xb4
};
static_assert(sizeof(AIEntry) == 0xb8, "AIEntry");

struct AIDecision {  // size 0x18
    int32 nEntryCount;  // 0x00
    XPtr64<AIEntry> arEntries;  // 0x08
    SerializeData serEntries;  // 0x10
};
static_assert(sizeof(AIDecision) == 0x18, "AIDecision");

struct AIStateDefinition {  // size 0x40
    RequiredDefinitionHeader tHeader;  // 0x00
    AIDecision arDecision[2];  // 0x10
};
static_assert(sizeof(AIStateDefinition) == 0x40, "AIStateDefinition");

struct AIBehaviorDefinition {  // size 0x30
    RequiredDefinitionHeader tHeader;  // 0x00
    SNO arAIState[8];  // 0x0c
    uint32 dwPad;  // 0x2c
};
static_assert(sizeof(AIBehaviorDefinition) == 0x30, "AIBehaviorDefinition");

class AIStateGroupDef : public GroupDefinition {  // size 0xa0
public:
    AIStateGroupDef();
    GROUPDEFINITION_MEMORY_FUNCTIONS()  // vtable slots 2-5; the macro name is invented, see Definitions.h
    virtual uint32 GetMaxCount();  // vtable slot 11
    virtual BOOL CanReferenceOtherSNOs() const { return TRUE; }  // vtable slot 13, line 82
    virtual SNOPackType GetSNOPackDestination(const void *) const { return SNOPACKTYPE_SERVERDATA; }  // vtable slot 7, line 84
};
static_assert(sizeof(AIStateGroupDef) == 0xa0, "AIStateGroupDef");

extern AIStateGroupDef * gptGroupDefAIState;  // AIDefinition.cpp

class AIBehaviorGroupDef : public GroupDefinition {  // size 0xa0
public:
    AIBehaviorGroupDef();
    GROUPDEFINITION_MEMORY_FUNCTIONS()  // vtable slots 2-5; the macro name is invented, see Definitions.h
    virtual uint32 GetMaxCount();  // vtable slot 11
    virtual BOOL CanReferenceOtherSNOs() const { return TRUE; }  // vtable slot 13, line 112
    virtual SNOPackType GetSNOPackDestination(const void *) const { return SNOPACKTYPE_SERVERDATA; }  // vtable slot 7, line 114
};
static_assert(sizeof(AIBehaviorGroupDef) == 0xa0, "AIBehaviorGroupDef");

extern AIBehaviorGroupDef * gptGroupDefAIBehavior;  // AIDefinition.cpp

typedef ScopedDefinitionLockR<AIStateDefinition> ScopedAIStateDefinitionLockR;

template <> inline const AIStateDefinition * DefinitionLockR<AIStateDefinition>(SNO sno) {  // line 131
    return (const AIStateDefinition *)gptGroupDefAIState->LockR(sno, FALSE);
}
template <> inline void DefinitionUnlockR<AIStateDefinition>(const AIStateDefinition * pdef) {  // line 132
    void * lpDef = (void *)pdef;
    gptGroupDefAIState->UnlockR(lpDef);
}
