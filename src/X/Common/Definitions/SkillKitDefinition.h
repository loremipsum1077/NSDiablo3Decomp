// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Definitions/SkillKitDefinition.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "AttachmentDefinition.h"
#include "Definitions.h"
#include "../SNO/SNOFiles.h"
#include "../../Sigma/DataStructures/XPtr64.h"
#include "../../Sigma/Files/XSerializeData.h"
#include "../../Sigma/SigmaTypes.h"
#include <stddef.h>
#include <stdint.h>

class XMemoryAllocatorInterface;

enum {
    SKILLCAT_INVALID = -1,
    SKILLCAT_BARB_1 = 0,
    SKILLCAT_BARB_2 = 1,
    SKILLCAT_BARB_3 = 2,
    SKILLCAT_WIZ_1 = 3,
    SKILLCAT_WIZ_2 = 4,
    SKILLCAT_WIZ_3 = 5,
    SKILLCAT_WITCHDOC_1 = 6,
    SKILLCAT_WITCHDOC_2 = 7,
    SKILLCAT_WITCHDOC_3 = 8,
    SKILLCAT_DHUNTER_1 = 9,
    SKILLCAT_DHUNTER_2 = 10,
    SKILLCAT_DHUNTER_3 = 11,
    SKILLCAT_MONK_1 = 12,
    SKILLCAT_MONK_2 = 13,
    SKILLCAT_MONK_3 = 14,
    SKILLCAT_BARB_4 = 15,
    SKILLCAT_BARB_5 = 16,
    SKILLCAT_BARB_6 = 17,
    SKILLCAT_WIZ_4 = 18,
    SKILLCAT_WIZ_5 = 19,
    SKILLCAT_WIZ_6 = 20,
    SKILLCAT_WITCHDOC_4 = 21,
    SKILLCAT_WITCHDOC_5 = 22,
    SKILLCAT_WITCHDOC_6 = 23,
    SKILLCAT_DHUNTER_4 = 24,
    SKILLCAT_DHUNTER_5 = 25,
    SKILLCAT_DHUNTER_6 = 26,
    SKILLCAT_MONK_4 = 27,
    SKILLCAT_MONK_5 = 28,
    SKILLCAT_MONK_6 = 29,
    SKILLCAT_CRUSADER_1 = 30,
    SKILLCAT_CRUSADER_2 = 31,
    SKILLCAT_CRUSADER_3 = 32,
    SKILLCAT_CRUSADER_4 = 33,
    SKILLCAT_CRUSADER_5 = 34,
    SKILLCAT_CRUSADER_6 = 35,
    SKILLCAT_NECROMANCER_1 = 36,
    SKILLCAT_NECROMANCER_2 = 37,
    SKILLCAT_NECROMANCER_3 = 38,
    SKILLCAT_NECROMANCER_4 = 39,
    SKILLCAT_NECROMANCER_5 = 40,
    SKILLCAT_NECROMANCER_6 = 41,
    NUM_SKILL_CATEGORIES = 42,
};

typedef int32 SkillCategory;

struct TraitEntry {  // size 0x10
    SNO snoPower;  // 0x00
    BOOL fHide;  // 0x04
    int32 nMasteryReq;  // 0x08
    BOOL bNew;  // 0x0c
};
static_assert(sizeof(TraitEntry) == 0x10, "TraitEntry");

struct ActiveSkillEntry {  // size 0x40
    SNO snoPower;  // 0x00
    SkillCategory eCategory;  // 0x04
    SkillCategory eCategoryNew;  // 0x08
    int32 nMasteryReq;  // 0x0c
    BOOL fInitiallyGranted;  // 0x10
    int32 nRuneAReq;  // 0x14
    int32 nRuneBReq;  // 0x18
    int32 nRuneCReq;  // 0x1c
    int32 nRuneDReq;  // 0x20
    int32 nRuneEReq;  // 0x24
    BOOL bNew;  // 0x28
    BOOL bRuneNew[5];  // 0x2c
    int32 GetRuneLevelRequirement(const RuneType eRuneType) const;
};
static_assert(sizeof(ActiveSkillEntry) == 0x40, "ActiveSkillEntry");

struct SkillKitDefinition {  // size 0x40
    RequiredDefinitionHeader tHeader;  // 0x00
    XPtr64<TraitEntry> arTraitEntries;  // 0x10
    SerializeData serTraitEntries;  // 0x18
    XPtr64<ActiveSkillEntry> arActiveSkillEntries;  // 0x20
    SerializeData serActiveSkillEntries;  // 0x28
    XPtr64<int> arPowerFallbacks;  // 0x30
    SerializeData serPowerFallbacks;  // 0x38
};
static_assert(sizeof(SkillKitDefinition) == 0x40, "SkillKitDefinition");

class SkillKitGroupDef : public GroupDefinition {  // size 0xa0, line 119
public:
    SkillKitGroupDef();
    GROUPDEFINITION_MEMORY_FUNCTIONS()  // line 124; the macro name is invented, see Definitions.h
    virtual uint32 GetMaxCount();  // vtable slot 11
    virtual BOOL CanReferenceOtherSNOs() const { return TRUE; }  // vtable slot 13, line 127
    virtual SNOPackType GetSNOPackDestination(const void *) const {  // vtable slot 7, line 129
        return SNOPACKTYPE_COREDATA;
    }
};
static_assert(sizeof(SkillKitGroupDef) == 0xa0, "SkillKitGroupDef");

extern SkillKitGroupDef * gptGroupDefSkillKit;  // SkillKitDefinition.cpp:62

typedef ScopedDefinitionLockR<SkillKitDefinition> ScopedSkillKitDefinitionLockR;

// Line 154. Its callers call LockRFast and have nothing from an unlock, so
// DefinitionUnlockR<SkillKitDefinition> is taken to be empty, as for FlagSetDefinition.
template <> inline const SkillKitDefinition * DefinitionLockR<SkillKitDefinition>(SNO sno) {
    return (const SkillKitDefinition *)gptGroupDefSkillKit->LockRFast(sno);
}
template <> inline void DefinitionUnlockR<SkillKitDefinition>(const SkillKitDefinition *) {}

const ActiveSkillEntry * SkillKitGetMatchingActiveSkillEntry(const SNO snoSkillKit, const SNO snoPower);
const TraitEntry * SkillKitGetMatchingTraitEntry(const SNO snoSkillKit, const SNO snoPower);


// Functions defined in this file (bodies are not in the DWARF):
//   line   119  SkillKitGroupDef::~SkillKitGroupDef()  [0x71006421d0]
//   line   124  void * SkillKitGroupDef::MemAlloc(size_t nSize, uint32 dwAlign) const  [0x7100642200 (+1 more)]
//   line   124  void SkillKitGroupDef::MemFree(void *& ptr) const  [0x7100642250 (+1 more)]
//   line   127  BOOL SkillKitGroupDef::CanReferenceOtherSNOs() const  [0x71006422e0]
//   line   129  SNOPackType SkillKitGroupDef::GetSNOPackDestination(const void *) const  [0x71006422d0]
//   line   154  const SkillKitDefinition * DefinitionLockR<SkillKitDefinition>(SNO sno)  [inlined into 34 functions]
