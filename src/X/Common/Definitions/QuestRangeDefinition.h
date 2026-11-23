// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Definitions/QuestRangeDefinition.h
// Subset: the group definition class and its global, which X/Common/Game.cpp registers
// (GroupDefinition::Create<QuestRangeGroupDef>). The definition struct itself is not adopted yet.

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../X/Common/Definitions/Definitions.h"
#include "../../../X/Common/SNO/SNOFiles.h"
#include "../../../X/Sigma/SigmaTypes.h"

class XMemoryAllocatorInterface;

class QuestRangeGroupDef : public GroupDefinition {  // size 0xa0
public:
    QuestRangeGroupDef();
    GROUPDEFINITION_MEMORY_FUNCTIONS()  // vtable slots 2-5; the macro name is invented, see Definitions.h
    virtual uint32 GetMaxCount();  // vtable slot 11
    virtual BOOL CanReferenceOtherSNOs() const { return TRUE; }  // vtable slot 13, line 60
    virtual SNOPackType GetSNOPackDestination(const void *) const { return SNOPACKTYPE_COREDATA; }  // vtable slot 7, line 62
    virtual void GetDefaultData(void * ptDefaultDef) const;  // vtable slot 21
};
static_assert(sizeof(QuestRangeGroupDef) == 0xa0, "QuestRangeGroupDef");

extern QuestRangeGroupDef * gptGroupDefQuestRange;  // QuestRangeDefinition.cpp

// --- the definition struct, adopted for X/Server/LuaScript/ScriptEventParams.cpp ---

#include "../../Sigma/DataStructures/XPtr64.h"
#include "../../Sigma/Files/XSerializeData.h"

struct QuestTime {  // size 0x8
    SNO snoQuest;  // 0x00
    int32 nStepUID;  // 0x04
    void Initialize();  // QuestRangeDefinition.cpp:202
};
static_assert(sizeof(QuestTime) == 0x8, "QuestTime");

struct QuestRange {  // size 0x10
    QuestTime tStart;  // 0x00
    QuestTime tEnd;  // 0x08
};
static_assert(sizeof(QuestRange) == 0x10, "QuestRange");

struct QuestRangeDefinition {  // size 0x20
    RequiredDefinitionHeader tHeader;  // 0x00
    int32 nQuestRangeCount;  // 0x0c
    XPtr64<QuestRange> arQuestRanges;  // 0x10
    SerializeData serQuestRanges;  // 0x18
};
static_assert(sizeof(QuestRangeDefinition) == 0x20, "QuestRangeDefinition");

typedef ScopedDefinitionLockR<QuestRangeDefinition> ScopedQuestRangeDefinitionLockR;

// Line 89. Its callers call LockRFast and have nothing from an unlock (ScriptEventParams.cpp,
// BossEncounter.cpp, QuestRangeDefinition.cpp), so the unlock is taken to be empty.
template <> inline const QuestRangeDefinition * DefinitionLockR<QuestRangeDefinition>(SNO sno) {  // line 89
    return (const QuestRangeDefinition *)gptGroupDefQuestRange->LockRFast(sno);
}
template <> inline void DefinitionUnlockR<QuestRangeDefinition>(const QuestRangeDefinition *) {}  // line 90

enum QuestRangeResult {
    QRR_INVALID = 0,
    QRR_NO_QUEST_RANGE = 1,
    QRR_NO_ENDPOINTS = 2,
    QRR_NOT_REACHED_YET = 3,
    QRR_ACTIVE = 4,
    QRR_EXPIRED = 5,
};

BOOL QuestRangeEvaluate(const SNO snoQuestRange, QuestRangeResult * pResult);  // QuestRangeDefinition.cpp:280
BOOL QuestRangeEvaluate(const SNO snoQuestRange, const SNO snoCurrentQuest, const int32 nCurrentStepUID, QuestRangeResult * pResult);  // QuestRangeDefinition.cpp:497
BOOL QuestRangeStillPending(const SNO snoQuestRange);  // QuestRangeDefinition.cpp:578
