// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Definitions/EncounterDefinition.h
// Subset: the group definition class and its global, which X/Common/Game.cpp registers
// (GroupDefinition::Create<EncounterGroupDef>). The definition struct itself is not adopted yet.

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../X/Common/Definitions/Definitions.h"
#include "../../../X/Common/SNO/SNOFiles.h"
#include "../../../X/Sigma/SigmaTypes.h"

class XMemoryAllocatorInterface;

class EncounterGroupDef : public GroupDefinition {  // size 0xa0
public:
    EncounterGroupDef();
    GROUPDEFINITION_MEMORY_FUNCTIONS()  // vtable slots 2-5; the macro name is invented, see Definitions.h
    virtual uint32 GetMaxCount();  // vtable slot 11
    virtual BOOL CanReferenceOtherSNOs() const { return TRUE; }  // vtable slot 13, line 77
    virtual SNOPackType GetSNOPackDestination(const void *) const { return SNOPACKTYPE_COREDATA; }  // vtable slot 7, line 79
    virtual void GetDefaultData(void * ptDefaultDef) const;  // vtable slot 21
};
static_assert(sizeof(EncounterGroupDef) == 0xa0, "EncounterGroupDef");

extern EncounterGroupDef * gptGroupDefEncounter;  // EncounterDefinition.cpp

#include "../../Sigma/DataStructures/XPtr64.h"
#include "../../Sigma/Files/XSerializeData.h"

typedef int32 Team;

struct EncounterSpawnOption {  // size 0x10
    SNO snoSpawn;  // 0x00
    int32 nWeight;  // 0x04
    Team eTeam;  // 0x08
    SNO snoCondition;  // 0x0c
};
static_assert(sizeof(EncounterSpawnOption) == 0x10, "EncounterSpawnOption");

struct EncounterDefinition {  // size 0x20
    RequiredDefinitionHeader tHeader;  // 0x00
    SNO snoSymbol;  // 0x0c
    XPtr64<EncounterSpawnOption> arSpawnOptions;  // 0x10
    SerializeData serSpawnOptions;  // 0x18
};
static_assert(sizeof(EncounterDefinition) == 0x20, "EncounterDefinition");

typedef ScopedDefinitionLockR<EncounterDefinition> ScopedEncounterDefinitionLockR;

// Line 119; the fast immutable definition lock inlined into ACDIsLootRunBoss.
template <> inline const EncounterDefinition * DefinitionLockR<EncounterDefinition>(SNO sno) {
    return (const EncounterDefinition *)gptGroupDefEncounter->LockRFast(sno);
}
template <> inline void DefinitionUnlockR<EncounterDefinition>(const EncounterDefinition *) {}
