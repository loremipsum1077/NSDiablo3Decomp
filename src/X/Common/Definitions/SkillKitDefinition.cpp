// Original file: X/Common/Definitions/SkillKitDefinition.cpp

#include "SkillKitDefinition.h"

// For cgplaceNull, whose initializer the original runs in this file's static initializer.
#include "../WorldPlace.h"
#include "../../Client/Gfx/GfxVB.h"
#include "../D3TypeDescriptor/D3TypeDescriptor.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptorField.h"

// Defined in X/Sigma/Files/XFilesCommon.cpp:20; no adopted header declares it yet.
extern const XTypeDescriptor * DT_SERIALIZEDATA;

// Defined in X/Common/D3TypeDescriptor/D3DescribedType.cpp; no adopted header declares it yet.
extern const XTypeDescriptor * DT_SNO;

enum {
    SKILLKIT_SNO_VERSION = 26,
    SKILLKIT_XML_VERSION = 4,
};

// _GLOBAL__sub_I_SkillKitDefinition.cpp: the translation unit also initializes the four RGBAColor constants
// of X/Client/Gfx/GfxVB.h:351-354 (included above). The field names are "" in the binary.
static const TypeDescriptorField TraitEntry_Fields[5] = {  // line 30
    FilterTypeDescriptorField("", DT_SNO, offsetof(TraitEntry, snoPower), "", POWER_SNO, 0x100, -1),
    TypeDescriptorField("", DT_INT, offsetof(TraitEntry, fHide), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(TraitEntry, nMasteryReq), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(TraitEntry, bNew), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(TraitEntry), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_TraitEntry("TraitEntry", 0, TraitEntry_Fields, 5, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 35
const XTypeDescriptor * DT_TRAIT_ENTRY = &TypeDescriptor_TraitEntry;

static const TypeDescriptorField ActiveSkillEntry_Fields[13] = {  // line 37
    FilterTypeDescriptorField("", DT_SNO, offsetof(ActiveSkillEntry, snoPower), "", POWER_SNO, 0x100, -1),
    TypeDescriptorField("", DT_INT, offsetof(ActiveSkillEntry, eCategory), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ActiveSkillEntry, eCategoryNew), (const void *)SKILLCAT_INVALID, 1),
    TypeDescriptorField("", DT_INT, offsetof(ActiveSkillEntry, nMasteryReq), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ActiveSkillEntry, fInitiallyGranted), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ActiveSkillEntry, nRuneAReq), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ActiveSkillEntry, nRuneBReq), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ActiveSkillEntry, nRuneCReq), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ActiveSkillEntry, nRuneDReq), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ActiveSkillEntry, nRuneEReq), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ActiveSkillEntry, bNew), NULL, 1),
    FixedArrayTypeDescriptorField("", DT_INT, offsetof(ActiveSkillEntry, bRuneNew), NULL, 1, 5, 0),
    TypeDescriptorField(NULL, DT_NULL, sizeof(ActiveSkillEntry), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_ActiveSkillEntry("ActiveSkillEntry", 0, ActiveSkillEntry_Fields, 13, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 50
const XTypeDescriptor * DT_ACTIVE_SKILL_ENTRY = &TypeDescriptor_ActiveSkillEntry;

static const TypeDescriptorField SkillKitDefinition_Fields[7] = {  // line 52
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(SkillKitDefinition, serTraitEntries), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_TRAIT_ENTRY, offsetof(SkillKitDefinition, arTraitEntries), 0x21,
                                     (int32)offsetof(SkillKitDefinition, serTraitEntries) - (int32)offsetof(SkillKitDefinition, arTraitEntries)),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(SkillKitDefinition, serActiveSkillEntries), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_ACTIVE_SKILL_ENTRY, offsetof(SkillKitDefinition, arActiveSkillEntries), 0x21,
                                     (int32)offsetof(SkillKitDefinition, serActiveSkillEntries) - (int32)offsetof(SkillKitDefinition, arActiveSkillEntries)),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(SkillKitDefinition, serPowerFallbacks), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_INT, offsetof(SkillKitDefinition, arPowerFallbacks), 0x21,
                                     (int32)offsetof(SkillKitDefinition, serPowerFallbacks) - (int32)offsetof(SkillKitDefinition, arPowerFallbacks)),
    TypeDescriptorField(NULL, DT_NULL, sizeof(SkillKitDefinition), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_SkillKitDefinition("SkillKit", SKILLKIT_XML_VERSION, SkillKitDefinition_Fields, 7, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 56
const XTypeDescriptor * DT_SKILLKIT = &TypeDescriptor_SkillKitDefinition;

SkillKitGroupDef * gptGroupDefSkillKit;  // line 62

// line 68
SkillKitGroupDef::SkillKitGroupDef()
    : GroupDefinition("SkillKit", SKILLKIT_SNO,
                      SNOVersions(SKILLKIT_SNO_VERSION, 28, SKILLKIT_SNO_VERSION, SKILLKIT_SNO_VERSION, 27),
                      SNODATATYPE_TITLE, sizeof(SkillKitDefinition), DT_SKILLKIT, 0x4005, 14) {
}

// line 89
uint32 SkillKitGroupDef::GetMaxCount() {
    return 16;
}

// line 193
const ActiveSkillEntry * SkillKitGetMatchingActiveSkillEntry(const SNO snoSkillKit, const SNO snoPower) {
    int32 nCount;

    ScopedSkillKitDefinitionLockR pdefSkillKit(snoSkillKit);
    nCount = pdefSkillKit->serActiveSkillEntries.dwSize / sizeof(ActiveSkillEntry);

    for (int32 i = 0; i < nCount; i++) {
        const ActiveSkillEntry & tEntry = pdefSkillKit->arActiveSkillEntries[i];
        if (tEntry.snoPower == snoPower)
            return &tEntry;
    }

    return NULL;
}

// line 214
const TraitEntry * SkillKitGetMatchingTraitEntry(const SNO snoSkillKit, const SNO snoPower) {
    int32 nCount;

    ScopedSkillKitDefinitionLockR pdefSkillKit(snoSkillKit);
    nCount = pdefSkillKit->serTraitEntries.dwSize / sizeof(TraitEntry);

    for (int32 i = 0; i < nCount; i++) {
        const TraitEntry & tEntry = pdefSkillKit->arTraitEntries[i];
        if (tEntry.snoPower == snoPower)
            return &tEntry;
    }

    return NULL;
}

// line 235
int32 ActiveSkillEntry::GetRuneLevelRequirement(const RuneType eRuneType) const {
    switch (eRuneType) {
    case 0:
        return nRuneAReq;
    case 1:
        return nRuneBReq;
    case 2:
        return nRuneCReq;
    case 3:
        return nRuneDReq;
    case 4:
        return nRuneEReq;
    default:
        return 0;
    }
}
