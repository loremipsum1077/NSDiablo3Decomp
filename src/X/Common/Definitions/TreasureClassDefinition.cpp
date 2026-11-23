// Original file: X/Common/Definitions/TreasureClassDefinition.cpp

#include "TreasureClassDefinition.h"

// Contrib/Contrib/blz/include/blz/tuple.h, which the original reaches through an include chain
// not adopted here. Its header-level static blz::ignore sits in front of this file's own statics.
#include "../../../Contrib/Contrib/blz/include/blz/tuple.h"

// For SNONAME_NONE, cgplaceNull and the RGBAColor normals, whose initializers the original runs in
// this file's static initializer.
#include "../SNO/SNOName.h"
#include "../WorldPlace.h"
#include "../../Client/Gfx/GfxVB.h"
#include "../D3TypeDescriptor/D3TypeDescriptor.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptorField.h"

// Defined in X/Sigma/Files/XFilesCommon.cpp:20; no adopted header declares it yet.
extern const XTypeDescriptor * DT_SERIALIZEDATA;

// Defined in X/Common/D3TypeDescriptor/D3DescribedType.cpp; no adopted header declares these yet.
extern const XTypeDescriptor * DT_SNO;
extern const XTypeDescriptor * DT_GBID;

// GameBalanceDefinition.cpp:2466 (out of line at 0x710061cbc0; not declared by an adopted header).
BOOL GBIDExists(GBID gbid, GameBalanceType eGBType);

enum {
    TREASURECLASS_SNO_COMMON_VERSION = 57,
    TREASURECLASS_SNO_PC_VERSION = 57,
    TREASURECLASS_SNO_CONSOLE_VERSION = 58,
    TREASURECLASS_SNO_AXE_VERSION = 58,
};

enum {
    GB_AFFIXES = 8,
};

static const float scflDefaultDropChance = 1.0f;  // line 26

// The field names are "" in the binary.
static const TypeDescriptorField ItemSpecifierData_Fields[10] = {  // line 37
    FilterTypeDescriptorField("", DT_GBID, offsetof(ItemSpecifierData, gbidItem), "", 2, 0, -1),
    TypeDescriptorField("", DT_INT, offsetof(ItemSpecifierData, nNumAffixes), NULL, 1),
    FilteredFixedArrayTypeDescriptorField("", DT_GBID, offsetof(ItemSpecifierData, gbidAffixes), "", 0, MAX_SPECIFIED_ITEM_AFFIXES, 8, 0),
    TypeDescriptorField("", DT_INT, offsetof(ItemSpecifierData, nAdditionalRandomAffixes), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ItemSpecifierData, nAdditionalRandomAffixesDelta), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ItemSpecifierData, nMinLegendaryAffixes), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ItemSpecifierData, bAccountBound), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ItemSpecifierData, nAdditionalMajorAffixes), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ItemSpecifierData, nAdditionalMinorAffixes), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(ItemSpecifierData), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_ItemSpecifierData("ItemSpecifierData", 0, ItemSpecifierData_Fields, 10, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 47
const XTypeDescriptor * DT_ITEMSPECIFIERDATA = &TypeDescriptor_ItemSpecifierData;

static const TypeDescriptorField LootDropModifier_Fields[17] = {  // line 49
    TypeDescriptorField("", DT_INT, offsetof(LootDropModifier, eDropType), NULL, 1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(LootDropModifier, snoSubTreasureClass), "", TREASURECLASS_SNO, 0, -1),
    TypeDescriptorField("", DT_ITEMSPECIFIERDATA, offsetof(LootDropModifier, tSpecificItem), NULL, 1),
    FilterTypeDescriptorField("", DT_GBID, offsetof(LootDropModifier, gbidQualityClass), "", 27, 0, -1),
    TypeDescriptorField("", DT_HIGHPRECISIONPERCENT, offsetof(LootDropModifier, flChance), (const void *)0x3f800000, 1),
    TypeDescriptorField("", DT_INT, offsetof(LootDropModifier, nWeight), NULL, 1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(LootDropModifier, snoCondition), "", CONDITION_SNO, 0, -1),
    TypeDescriptorField("", DT_INT, offsetof(LootDropModifier, nItemLabel), (const void *)-1, 1),
    TypeDescriptorField("", DT_INT, offsetof(LootDropModifier, nItemLabel2), (const void *)-1, 1),
    FixedArrayTypeDescriptorField("", DT_INT, offsetof(LootDropModifier, dwItemLabelMask), NULL, 0, 5, 0),
    TypeDescriptorField("", DT_INT, offsetof(LootDropModifier, nQuantity), (const void *)1, 1),
    TypeDescriptorField("", DT_INT, offsetof(LootDropModifier, nQuantityDelta), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(LootDropModifier, bTryStack), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(LootDropModifier, bBossSpecificDrop), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(LootDropModifier, flGoldMultiplier), (const void *)0x3f800000, 1),
    TypeDescriptorField("", DT_INT, offsetof(LootDropModifier, bIgnoreGoldRanges), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(LootDropModifier), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_LootDropModifier("LootDropModifier", 2, LootDropModifier_Fields, 17, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 67
const XTypeDescriptor * DT_LOOTDROPMODIFIER = &TypeDescriptor_LootDropModifier;

static const TypeDescriptorField TreasureClassDefinition_Fields[9] = {  // line 69
    TypeDescriptorField("", DT_HIGHPRECISIONPERCENT, offsetof(TreasureClassDefinition, flNoDrop), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(TreasureClassDefinition, eChooseType), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(TreasureClassDefinition, nForcedDropLevel), NULL, 1),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(TreasureClassDefinition, serModifierList), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_LOOTDROPMODIFIER, offsetof(TreasureClassDefinition, ptModifierList), 0x21, 8),
    VariableArrayCountTypeDescriptorField("", offsetof(TreasureClassDefinition, nModifierCount), 0x10000, DT_LOOTDROPMODIFIER, 12),
    TypeDescriptorField("", DT_INT, offsetof(TreasureClassDefinition, nAncientChanceHandicapLevelOverride), (const void *)-1, 1),
    TypeDescriptorField("", DT_INT, offsetof(TreasureClassDefinition, dwPad), NULL, 0),
    TypeDescriptorField(NULL, DT_NULL, sizeof(TreasureClassDefinition), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_TreasureClassDefinition("TreasureClass", 0, TreasureClassDefinition_Fields, 9, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 78
const XTypeDescriptor * DT_TREASURECLASS = &TypeDescriptor_TreasureClassDefinition;

TreasureClassGroupDef * gptGroupDefTreasureClass;  // line 88

// line 94
TreasureClassGroupDef::TreasureClassGroupDef()
    : GroupDefinition("TreasureClass", TREASURECLASS_SNO,
                      SNOVersions(TREASURECLASS_SNO_COMMON_VERSION, 60, TREASURECLASS_SNO_CONSOLE_VERSION, TREASURECLASS_SNO_AXE_VERSION, 59),
                      SNODATATYPE_TITLE, sizeof(TreasureClassDefinition), DT_TREASURECLASS, 0x9241, 0x5006) {
}

// line 120
uint32 TreasureClassGroupDef::GetMaxCount() {
    return 128;
}

// line 132
void TreasureClassGroupDef::GetDefaultData(void * ptDefaultDef) const {
    TreasureClassDefinition * pdefTreasureClass = (TreasureClassDefinition *)ptDefaultDef;
    SigmaMemoryClear(pdefTreasureClass, sizeof(TreasureClassDefinition));

    pdefTreasureClass->eChooseType = TCC_ALL_OF;
}

// line 144
static void sConvertLabelToMask(const int32 nItemLabel, uint32 * pdwItemLabelMask) {
    if (nItemLabel != -1) {
        int32 nDwordIndex = nItemLabel / 32;
        int32 nBit = nItemLabel - nDwordIndex * 32;
        pdwItemLabelMask[nDwordIndex] |= 1 << nBit;
    }
}

// line 163
BOOL TreasureClassGroupDef::Write(void * lpDefinition, FileReference & tRef) const {
    TreasureClassDefinition * pdefTreasureClass = (TreasureClassDefinition *)lpDefinition;

    for (int32 i = 0; i < pdefTreasureClass->nModifierCount; i++) {
        LootDropModifier & tModifier = pdefTreasureClass->ptModifierList[i];

        if (tModifier.eDropType == LDT_ITEMLABEL || tModifier.eDropType == LDT_LEGENDARY ||
            tModifier.eDropType == LDT_CLASS_TARGETED_LEGENDARY) {
            sConvertLabelToMask(tModifier.nItemLabel, tModifier.dwItemLabelMask);
            sConvertLabelToMask(tModifier.nItemLabel2, tModifier.dwItemLabelMask);
        } else {
            SigmaMemoryClear(tModifier.dwItemLabelMask, sizeof(tModifier.dwItemLabelMask));
        }
    }

    return GroupDefinition::Write(lpDefinition, tRef);
}

// line 270
BOOL ItemSpecifierData::IsValid() const {
    for (int32 i = 0; i < nNumAffixes; i++) {
        if (!GBIDExists(gbidAffixes[i], GB_AFFIXES))
            return FALSE;
    }

    return TRUE;
}
