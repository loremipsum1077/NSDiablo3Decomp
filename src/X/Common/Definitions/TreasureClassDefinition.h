#pragma once
#include "Definitions.h"
#include "../GameBalance.h"
#include "GameBalanceDefinition.h"
#include "../../Sigma/DataStructures/XPtr64.h"
#include "../../Sigma/Files/XSerializeData.h"

enum {
    MAX_SPECIFIED_ITEM_AFFIXES = 6,
};

struct ItemSpecifierData {  // size 0x38
    ItemSpecifierData();
    void Init();
    BOOL IsValid() const;
    GBID gbidItem;  // 0x00
    int32 nNumAffixes;  // 0x04
    GBID gbidAffixes[6];  // 0x08
    int32 nAdditionalRandomAffixes;  // 0x20
    int32 nAdditionalRandomAffixesDelta;  // 0x24
    int32 nMinLegendaryAffixes;  // 0x28
    BOOL bAccountBound;  // 0x2c
    int32 nAdditionalMajorAffixes;  // 0x30
    int32 nAdditionalMinorAffixes;  // 0x34
};
static_assert(sizeof(ItemSpecifierData) == 0x38, "ItemSpecifierData");

class XMemoryAllocatorInterface;
struct FileReference;

typedef int32 ItemLabel;

enum LootDropType {
    LDT_NONE = -1,
    LDT_TREASURECLASS = 0,
    LDT_ITEMLABEL = 1,
    LDT_ITEM = 2,
    LDT_LEGENDARY = 3,
    LDT_CLASS_TARGETED_LEGENDARY = 4,
    LDT_COUNT = 5,
};

struct LootDropModifier {  // size 0x84
    LootDropType eDropType;  // 0x00
    SNO snoSubTreasureClass;  // 0x04
    float flChance;  // 0x08
    int32 nWeight;  // 0x0c
    GBID gbidQualityClass;  // 0x10
    ItemLabel nItemLabel;  // 0x14
    ItemLabel nItemLabel2;  // 0x18
    SNO snoCondition;  // 0x1c
    ItemSpecifierData tSpecificItem;  // 0x20
    int32 nQuantity;  // 0x58
    int32 nQuantityDelta;  // 0x5c
    uint32 dwItemLabelMask[5];  // 0x60
    BOOL bTryStack;  // 0x74
    BOOL bBossSpecificDrop;  // 0x78
    float flGoldMultiplier;  // 0x7c
    BOOL bIgnoreGoldRanges;  // 0x80
};
static_assert(sizeof(LootDropModifier) == 0x84, "LootDropModifier");

struct TreasureClassDefinition {  // size 0x38
    RequiredDefinitionHeader tHeader;  // 0x00
    float flNoDrop;  // 0x0c
    TreasureClassChooseType eChooseType;  // 0x10
    int32 nForcedDropLevel;  // 0x14
    int32 nAncientChanceHandicapLevelOverride;  // 0x18
    int32 nModifierCount;  // 0x1c
    XPtr64<LootDropModifier> ptModifierList;  // 0x20
    SerializeData serModifierList;  // 0x28
    uint32 dwPad;  // 0x30
};
static_assert(sizeof(TreasureClassDefinition) == 0x38, "TreasureClassDefinition");

class TreasureClassGroupDef : public GroupDefinition {  // size 0xa0
public:
    TreasureClassGroupDef();
    GROUPDEFINITION_MEMORY_FUNCTIONS()  // vtable slots 2-5; the macro name is invented, see Definitions.h
    virtual uint32 GetMaxCount();  // vtable slot 11
    virtual BOOL CanReferenceOtherSNOs() const { return TRUE; }  // vtable slot 13
    virtual SNOPackType GetSNOPackDestination(const void *) const {
        return SNOPACKTYPE_SERVERDATA;
    }  // vtable slot 7
    virtual void GetDefaultData(void * ptDefaultDef) const;  // vtable slot 21
    virtual BOOL Write(void * lpDefinition, FileReference & tRef) const;  // vtable slot 19
};
static_assert(sizeof(TreasureClassGroupDef) == 0xa0, "TreasureClassGroupDef");

extern TreasureClassGroupDef * gptGroupDefTreasureClass;  // TreasureClassDefinition.cpp
// Lines 161-162: original SChallengeRift.cpp GetChallengeReward calls LockR(sno, FALSE)
// and UnlockR(void *&), as does Power_ItemUse.cpp.
template <> inline const TreasureClassDefinition * DefinitionLockR<TreasureClassDefinition>(SNO sno) {
    return (const TreasureClassDefinition *)gptGroupDefTreasureClass->LockR(sno, FALSE);
}
template <> inline void DefinitionUnlockR<TreasureClassDefinition>(const TreasureClassDefinition * pdef) {
    gptGroupDefTreasureClass->UnlockR((void *&)pdef);
}
typedef ScopedDefinitionLockR<TreasureClassDefinition> ScopedTreasureClassDefinitionLockR;
