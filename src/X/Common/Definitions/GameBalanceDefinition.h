// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Definitions/GameBalanceDefinition.h
// Subset: ItemDefinition's leading members and the ScopedGBLockR lock guard, which
// UIConsoleDyeItemView.cpp needs, and the monster-level and rare-monster-name records Monsters.cpp
// needs. Members not adopted yet are invented placeholder arrays that keep each struct its
// original size.

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../GameBalance.h"
#include "../ImageHandle.h"
#include "../Monsters.h"
#include "Definitions.h"
#include "../SNO/SNOFiles.h"
#include "../Attributes/FastAttribConstantsSwig.h"
#include "../../Sigma/DataStructures/XPtr64.h"
#include "../../Sigma/Files/XSerializeData.h"
#include "../../Sigma/SigmaTypes.h"

struct GameBalanceDefinition;

typedef int32 Act;

typedef int32 SavedDyeType;

typedef int32 MonsterRarity;

enum {
    GBA_INVALID = -1,
    GBA_HITPOINTS = 4,
    GBA_HITPOINTS_FACTOR_LEVEL = 5,
    GBA_HPRANGE = 7,
    GBA_HPREGEN = 8,
    GBA_PRIMARY_RESOURCE_BASE = 9,
    GBA_PRIMARY_RESOURCE_FACTOR_LEVEL = 10,
    GBA_PRIMARY_RESOURCE_REGEN = 11,
    GBA_SECONDARY_RESOURCE_BASE = 13,
    GBA_SECONDARY_RESOURCE_FACTOR_LEVEL = 14,
    GBA_SECONDARY_RESOURCE_REGEN = 15,
    GBA_DAMAGE = 22,
    GBA_DAMAGERANGE = 23,
    GBA_DAMAGE_FIRE = 24,
    GBA_DAMAGERANGE_FIRE = 25,
    GBA_DAMAGE_LIGHTNING = 26,
    GBA_DAMAGERANGE_LIGHTNING = 27,
    GBA_DAMAGE_COLD = 28,
    GBA_DAMAGERANGE_COLD = 29,
    GBA_DAMAGE_POISON = 30,
    GBA_DAMAGERANGE_POISON = 31,
    GBA_DAMAGE_ARCANE = 32,
    GBA_DAMAGERANGE_ARCANE = 33,
    GBA_DAMAGE_SIEGE = 34,
    GBA_DAMAGERANGE_SIEGE = 35,
    GBA_GETHIT_DAMAGE = 36,
    GBA_GETHIT_DAMAGE_SCALAR = 37,
    GBA_GETHIT_MAX = 38,
    GBA_GETHIT_RECOVERY = 39,
    GBA_CRIT_DAMAGE_CAP = 40,
    GBA_CRIT_PERCENT_BASE = 41,
    GBA_CRIT_PERCENT_CAP = 42,
    GBA_SPRINTSPEED = 43,
    GBA_ATTACKSPEED = 44,
    GBA_ARMOR = 45,
    GBA_ARMOR_DELTA = 46,
    GBA_ARMOR_DEPRECATED = 47,
    GBA_ARMOR_FIRE_DEPRECATED = 48,
    GBA_ARMOR_LIGHTNING_DEPRECATED = 49,
    GBA_ARMOR_COLD_DEPRECATED = 50,
    GBA_ARMOR_POISON_DEPRECATED = 51,
    GBA_ARMOR_ARCANE_DEPRECATED = 52,
    GBA_ARMOR_SIEGE_DEPRECATED = 53,
    GBA_WALKSPEED = 54,
    GBA_RUNSPEED = 55,
    GBA_AUTOWALKDISTANCE = 56,
    GBA_PROJSPEED = 57,
    GBA_EXPERIENCE = 58,
    GBA_ATTACK_FACTOR_DEX_DEPRECATED = 59,
    GBA_ATTACK_FACTOR_STR_DEPRECATED = 60,
    GBA_DAMAGE_FACTOR_DEX_DEPRECATED = 61,
    GBA_DEFENSE_FACTOR_DEX_DEPRECATED = 63,
    GBA_MANA_COST_REDUCTION_FACTOR_STR_DEPRECATED = 64,
    GBA_RESIST_PHYSICAL = 66,
    GBA_RESIST_FIRE = 67,
    GBA_RESIST_LIGHTNING = 68,
    GBA_RESIST_COLD = 69,
    GBA_RESIST_POISON = 70,
    GBA_RESIST_ARCANE = 71,
    GBA_RESIST_SIEGE = 72,
    GBA_RESIST_CHILL = 73,
    GBA_RESIST_STUN = 74,
    GBA_IMMUNITY_PHYSICAL = 75,
    GBA_IMMUNITY_FIRE = 76,
    GBA_IMMUNITY_LIGHTNING = 77,
    GBA_IMMUNITY_COLD = 78,
    GBA_IMMUNITY_POISON = 79,
    GBA_IMMUNITY_ARCANE = 80,
    GBA_IMMUNITY_SIEGE = 81,
    GBA_IMMUNITY_KNOCKBACK = 82,
    GBA_INITIAL_COOLDOWN_MIN = 83,
    GBA_INITIAL_COOLDOWN_DELTA = 84,
    GBA_ATTACK_COOLDOWN_MIN = 85,
    GBA_ATTACK_COOLDOWN_DELTA = 86,
    GBA_CLOSING_COOLDOWN_MIN = 87,
    GBA_CLOSING_COOLDOWN_DELTA = 88,
    GBA_ATTACKS_PER_SECOND = 89,
    GBA_KNOCKBACK_WEIGHT = 90,
    GBA_BREAKABLE_SHIELD_HP = 91,
    GBA_POWER_RATING = 92,
    GBA_OOC_HEALTH_REGEN_PERCENT = 93,
    GBA_OOC_MANA_REGEN_PERCENT = 94,
    GBA_POTION_DILUTION_DURATION = 95,
    GBA_POTION_DILUTION_SCALAR = 96,
    GBA_IMMUNITY_FEAR = 97,
    GBA_IMMUNITY_SLOW = 99,
    GBA_DUAL_WIELD_BOTH_ATTACK_CHANCE = 100,
    GBA_FREEZE_CAPACITY = 101,
    GBA_THAW_RATE = 102,
    GBA_IMMUNITY_CHARM = 103,
    GBA_IMMUNITY_BLIND = 104,
    GBA_IMMUNITY_FREEZE = 106,
    GBA_IMMUNITY_STUN = 107,
    GBA_DAMAGE_HOLY = 109,
    GBA_DAMAGERANGE_HOLY = 110,
    GBA_DAMAGE_VARIANCE_MIN = 111,
    GBA_DAMAGE_VARIANCE_DELTA = 112,
    GBA_DODGE_RATING_BASE = 113,
    GBA_HIRELING_HITPOINTS = 114,
    GBA_HIRELING_HPRANGE = 115,
    GBA_HIRELING_HPREGEN = 116,
    GBA_HIRELING_DAMAGE = 117,
    GBA_HIRELING_DAMAGE_RANGE = 118,
    GBA_STRENGTH = 120,
    GBA_DEXTERITY = 121,
    GBA_VITALITY = 122,
    GBA_INTELLIGENCE = 123,
    GBA_HIRELING_RETRAIN_COST = 125,
    GBA_HIRELING_LIGHT_LOOK_LEVEL = 126,
    GBA_HIRELING_MEDIUM_LOOK_LEVEL = 127,
    GBA_HIRELING_HEAVY_LOOK_LEVEL = 128,
    GBA_STRAFESPEED = 129,
    GBA_IMMUNITY_ROOT = 130,
    GBA_GETHIT_MAX_BASE = 134,
    GBA_GETHIT_MAX_PER_LEVEL = 135,
    GBA_GETHIT_RECOVERY_BASE = 136,
    GBA_GETHIT_RECOVERY_PER_LEVEL = 137,
    GBA_CLASS_DAMAGE_REDUCTION_PERCENT = 138,
    GBA_CLASS_DAMAGE_REDUCTION_PERCENT_PVP = 139,
    GBA_CONSOLE_HEALTH_SCALAR = 140,
    GBA_CONSOLE_DAMAGE_SCALAR = 141,
    GBA_MONSTER_1_AFFIX_WEIGHT = 142,
    GBA_MONSTER_2_AFFIX_WEIGHT = 143,
    GBA_MONSTER_3_AFFIX_WEIGHT = 144,
    GBA_MONSTER_4_AFFIX_WEIGHT = 145,
    NUM_GAMEBALANCE_ATTRIBUTES = 146,
};

typedef int32 GameBalanceAttribute;

typedef int32 AffixType;
enum AffixFlags { AFFIX_VENDOR_AVAILABLE_BIT = 0, AFFIX_CRAFTER_AVAILABLE_BIT = 1, AFFIX_CANNOT_BE_ENCHANTED_BIT = 2 };  // GameBalanceDefinition.h:1072
enum ItemFlags {
    ITEM_LEGENDARY_BIT = 0,
    ITEM_SPECIAL_BIT = 1,
    ITEM_UNIQUE_BIT = 2,
    ITEM_NEVER_MAGIC_BIT = 3,
    ITEM_NO_RANDOM_DROP_BIT = 4,
    ITEM_DYEABLE_BIT = 5,
    ITEM_BIND_ON_PICKUP = 6,
    ITEM_CANNOT_BE_SOLD_BIT = 7,
    ITEM_TEST_ONLY_BIT = 8,
    ITEM_DEPRECATED_BIT = 9,
    ITEM_IRREPLACEABLE_BIT = 10,
    ITEM_SELLS_NORMALLY_WHEN_BOUND_BIT = 11,
    ITEM_CANNOT_BE_TRANSMOGGED = 12,
    ITEM_CANNOT_UNLOCK_TRANSMOG = 13,
    ITEM_NEVER_BOE = 14,
    ITEM_UNIVERSAL_DEPRECATED = 15,
    ITEM_MADE_TO_ORDER_AVAILABLE = 16,
    ITEM_CAN_SALVAGE = 17,
    ITEM_HARDCORE_ONLY_BIT = 18,
    ITEM_X1_ONLY_BIT = 19,
    ITEM_TORMENT_ONLY_BIT = 20,
    ITEM_FORCE_TO_LARGE_ITEM_BIT = 21,
    ITEM_TIERED_LOOT_RUN_ONLY_BIT = 22,
    ITEM_ACCOUNT_WIDE_TRANSMOG = 23,
    ITEM_CANNOT_EXTRACT_POWER = 24,
    ITEM_UNK_25 = 25,
    ITEM_UNK_26 = 26,                   // 2.7.7: drops only while GameIsSeasonal() (sub_7100897970)
    ITEM_IS_CURRENCY_BIT = 27,
    ITEM_FORCE_EXTRACT_POWER_MINIMUM = 28,
    ITEM_IS_NOT_TRASH_BIT = 29,
    ITEM_UNK_30 = 30,                   // 2.7.7: red name; SItemUpgradeJewel replaces the item afterwards
    ITEM_ETHEREAL_BIT = 31,             // 2.7.7: drop gated on ATTRIBUTE_ETHEREAL_ITEMS_UNLOCKED (sub_7100897970)
};

enum {
    IQL_INVALID = -1,
    IQL_INFERIOR = 0,
    IQL_NORMAL = 1,
    IQL_SUPERIOR = 2,
    IQL_AFFIX_COUNT_START = 3,
    IQL_MAGIC_AFFIX_START = 3,
    IQL_MAGIC_1_AFFIX = 3,
    IQL_MAGIC_2_AFFIX = 4,
    IQL_MAGIC_3_AFFIX = 5,
    IQL_MAGIC_AFFIX_END = 5,
    IQL_RARE_AFFIX_START = 6,
    IQL_RARE_4_AFFIX = 6,
    IQL_RARE_5_AFFIX = 7,
    IQL_RARE_6_AFFIX = 8,
    IQL_RARE_AFFIX_END = 8,
    IQL_AFFIX_COUNT_END = 9,
    IQL_SPECIAL = 10,
    IQL_NUM_ITEM_QUALITY_LEVELS = 11,
    IQL_DISPLAY_AS_SET_ITEM = 11,
};

typedef int32 ItemQualityLevel;  // X/Common/Items.h

typedef int32 PlayerClass;  // X/Common/GameSwig.h

// line 49 (adopted subset)
enum {
    GB_ITEMS = 2,
    GB_HEROS = 7,
    GB_SCENERY = 17,
    GB_SOCKETED_EFFECTS = 21,
    GB_RECIPES = 32,
    GB_SET_ITEM_BONUSES = 33,
    GB_ELITE_MODIFIERS = 34,
};
enum {
    GB_PARAGON_BONUSES = 40,
    GB_TRANSMUTE_RECIPES = 50,
    GB_CURRENCY_CONVERSIONS = 51,
};

// X/Common/GameBalance.cpp
const void * GBLockR(const GBHandle & hGB, const GameBalanceDefinition *& pdefGB, const BOOL & bUnused);
void GBUnlockR(const GameBalanceDefinition *& pdefGB);
void GBEnumerate(GameBalanceType eType, GBHandleList & listResults);  // GameBalanceDefinition.cpp:3396
GBHandlePool * GBGetHandlePool();  // GameBalanceDefinition.cpp:3510
GBID GBIDExists(LPCSTR szGBID, GameBalanceType eGBType);  // GameBalanceDefinition.cpp:2444
BOOL GBIDExists(GBID gbid, GameBalanceType eGBType);  // GameBalanceDefinition.cpp:2466
BOOL GBIDExists(const GBHandle & hGB);  // GameBalanceDefinition.cpp:2460
SNO GameBalanceGetActorSNO(const GBHandle & hGB);  // GameBalanceDefinition.cpp:3273

class ActorCommonData;
struct RNG;

float AffixGetFormulaValue(uint8 * pbAttributeFormula, int32 nAttributeFormulaSize, const ActorCommonData * ptACDOwner, GBID gbidItem, RNG * ptRNGOverride, uint32 uFlags, int32 nLegendaryItemLevelOverride);  // GameBalanceDefinition.cpp:3631

class CRefString;
struct AttributeSpecifier;

CRefString GBIDToString(GBID gbid, GameBalanceType eGBType, const BOOL fUsedForXMLFields);  /* GameBalanceDefinition.cpp:3458 */  CRefString GBIDToString(const GBHandle & hGB, const BOOL fUsedForXMLFields);  // GameBalanceDefinition.cpp:3467
float AffixGetFormulaMinValue(const AttributeSpecifier & tAttribute, const ActorCommonData * ptACDOwner, GBID gbidItem, RNG * ptRNGOverride, const ActorCommonData * ptACDItem);  // GameBalanceDefinition.cpp:3668
float AffixGetFormulaMaxValue(const AttributeSpecifier & tAttribute, const ActorCommonData * ptACDOwner, GBID gbidItem, RNG * ptRNGOverride, const ActorCommonData * ptACDItem);  // GameBalanceDefinition.cpp:3719

struct GBHeader {  // size 0x18
    SerializeData serName_Console;  // 0x00
    XPtr64<char> szName;  // 0x08
    GBID gbid;  // 0x10
    uint32 dwPad;  // 0x14
};
static_assert(sizeof(GBHeader) == 0x18, "GBHeader");

struct AttributeSpecifier {  // size 0x18
    Attrib eAttribute;  // 0x00
    int32 nParam;  // 0x04
    XPtr64<unsigned char> pbAttributeFormula;  // 0x08
    SerializeData serAttributeFormula;  // 0x10
    BOOL operator==(const AttributeSpecifier & tSpecifier) const;
    BOOL operator!=(const AttributeSpecifier & tSpecifier) const;
};
static_assert(sizeof(AttributeSpecifier) == 0x18, "AttributeSpecifier");

// Original line 382.
inline BOOL AttributeSpecifier::operator==(const AttributeSpecifier & tSpecifier) const {
    return eAttribute == tSpecifier.eAttribute && (eAttribute == -1 || nParam == tSpecifier.nParam);
}

// Original line 389 (DWARF: inlined into UIEnchant::EnumPossibleReplacementAffixes).
inline BOOL AttributeSpecifier::operator!=(const AttributeSpecifier & tSpecifier) const {
    if (eAttribute != tSpecifier.eAttribute)
        return TRUE;
    if (eAttribute != -1 && nParam != tSpecifier.nParam)
        return TRUE;
    return FALSE;
}

// Original line 351 (defined before ItemDefinition, which embeds it).
struct RecipeIngredient {  // size 0x8
    GBID gbidItem;  // 0x00
    uint32 nQuantity;  // 0x04
};
static_assert(sizeof(RecipeIngredient) == 0x8, "RecipeIngredient");

// 2.7.7 layout, from the 2.7.7 ItemDefinition_Fields descriptor construction in
// _GLOBAL__sub_I_GameBalanceDefinition.cpp (0x71006cd580) and SItemUpgradeJewel (0x71007df9b0).
struct ItemDefinition {  // size 0x320
    GBHeader tHeader;  // 0x00
    SNO snoActor;  // 0x18
    GBID gbidItemType;  // 0x1c
    uint32 dwFlags;  // 0x20
    uint32 unk_24;  // 0x24 (flag word: descriptors for bits 0 and 1)
    SavedDyeType eSavedDyeType;  // 0x28
    int32 nItemLevel;  // 0x2c
    Act eMinAct;  // 0x30
    int32 nAffixLevelBonus;  // 0x34
    int32 nBonusRandomAffixes;  // 0x38
    int32 nBonusMajorAffixes;  // 0x3c
    int32 nBonusMinorAffixes;  // 0x40
    int32 nMaxSockets;  // 0x44
    int32 nMaxStackSize;  // 0x48
    int32 nBaseCostGold;  // 0x4c
    int32 nBaseCostSpecialCurrency;  // 0x50
    int32 nIdentifyCost;  // 0x54
    int32 nSellOverrideCost;  // 0x58
    int32 nRemoveGemCost;  // 0x5c
    int32 nNormalRequiredLevel;  // 0x60
    int32 nCrafterRequiredLevel;  // 0x64
    int32 nBaseDurability;  // 0x68
    int32 nDurabilityVariance;  // 0x6c
    int32 nEnchantAffixCost;  // 0x70
    int32 nEnchantAffixCostX1;  // 0x74
    int32 nTransmogUnlockCrafterLevel;  // 0x78
    int32 nTransmogCost;  // 0x7c
    GBID gbidBaseItem;  // 0x80
    GBID gbidSet;  // 0x84
    SNO snoRandomItemTreasureClass;  // 0x88
    SNO snoSalvageTreasureClassNormal;  // 0x8c
    SNO snoSalvageTreasureClassMagic;  // 0x90
    SNO snoSalvageTreasureClassRare;  // 0x94
    SNO snoRareNamePrefixStringList;  // 0x98
    SNO snoRareNameSuffixStringList;  // 0x9c
    SNO snoStartEffect;  // 0xa0
    SNO snoEndEffect;  // 0xa4
    ImageHandle hPortraitBkgrnd;  // 0xa8
    ImageHandle hPortraitHPBar;  // 0xac
    ImageHandle hPortraitBanner;  // 0xb0
    ImageHandle hPortraitFrame;  // 0xb4
    uint32 dwItemLabelMask[5];  // 0xb8
    float flAttributesDamage;  // 0xcc
    float flAttributesDamageRange;  // 0xd0
    float flAttributesDamageVarianceMin;  // 0xd4
    float flAttributesDamageVarianceDelta;  // 0xd8
    float flAttributesAttacksPerSecond;  // 0xdc
    float flAttributesArmor;  // 0xe0
    float flAttributesArmorDelta;  // 0xe4
    SkillDeclaration tSkills[4];  // 0xe8
    SerializeData serAttributes_Console;  // 0x108
    XPtr64<AttributeSpecifier> ptAttributes;  // 0x110
    ItemQualityLevel eDisplayedQualityLevel;  // 0x118
    GBID gbidCraftingRecipesToGrant[10];  // 0x11c
    GBID gbidTransmogsToGrant[8];  // 0x144
    int32 nLegendaryDropWeight;  // 0x164
    int32 nClassLegendaryDropWeight[7];  // 0x168
    int32 nLegendaryRating;  // 0x184
    GBID gbidLegendaryAffixFamily[6];  // 0x188
    int32 nLegendaryAffixFamilyLevel[6];  // 0x1a0
    BOOL bUseLegendaryAllowedTypes[6];  // 0x1b8
    int32 unk_1d0[6];  // 0x1d0
    GBID gbidLegendaryFamily;  // 0x1e8
    GBID unk_1ec;  // 0x1ec
    int32 unk_1f0;  // 0x1f0
    int32 eGemType;  // 0x1f4 (GemType)
    int32 nCraftingTier;  // 0x1f8
    int32 eCraftingQualityLevel;  // 0x1fc (CraftingQualityLevel)
    SNO snoActorPageOfFatePortal;  // 0x200
    SNO snoWorldPageOfFate[2];  // 0x204
    SNO snoLevelAreaPageOfFatePortal;  // 0x20c
    int32 nEnchantAffixIngredientsCount;  // 0x210
    RecipeIngredient arEnchantAffixIngredients[6];  // 0x214
    int32 nEnchantAffixIngredientsCountX1;  // 0x244
    RecipeIngredient arEnchantAffixIngredientsX1[6];  // 0x248
    GBID gbidLegendaryPowerItemReplacement;  // 0x278
    int32 nSeasonRequiredToDrop;  // 0x27c
    AttributeSpecifier tJewelUpgradeAttribute;  // 0x280
    AttributeSpecifier tJewelSecondaryEffectAttribute;  // 0x298
    int32 nJewelSecondaryEffectUnlockRank;  // 0x2b0
    AttributeSpecifier unk_2b8;  // 0x2b8
    int32 unk_2d0;  // 0x2d0 (unlock rank of unk_2b8)
    AttributeSpecifier unk_2d8;  // 0x2d8
    int32 unk_2f0;  // 0x2f0 (unlock rank of unk_2d8)
    int32 nJewelMaxRank;  // 0x2f4
    int32 unk_2f8;  // 0x2f8
    SNO snoMainEffect;  // 0x2fc
    int32 nBaseCostSpecialCurrency2;  // 0x300
    uint32 nDateReleased;  // 0x304
    int32 eVacuumPickup;  // 0x308 (ItemVacuumPickupType)
    int32 nDynamicCraftCostMagic;  // 0x30c
    int32 nDynamicCraftCostRare;  // 0x310
    int32 nDynamicCraftAffixCount;  // 0x314
    SNO snoSeasonCache;  // 0x318
};
static_assert(sizeof(ItemDefinition) == 0x320, "ItemDefinition");
static_assert(__builtin_offsetof(ItemDefinition, tSkills) == 0xe8, "ItemDefinition::tSkills");
static_assert(offsetof(ItemDefinition, nTransmogCost) == 0x7c, "ItemDefinition");
static_assert(offsetof(ItemDefinition, gbidLegendaryFamily) == 0x1e8, "ItemDefinition");
static_assert(offsetof(ItemDefinition, unk_2b8) == 0x2b8, "ItemDefinition");
static_assert(offsetof(ItemDefinition, snoMainEffect) == 0x2fc, "ItemDefinition");
static_assert(offsetof(ItemDefinition, snoSeasonCache) == 0x318, "ItemDefinition");

struct MonsterLevelDefinition {  // size 0xf0
    int32 nLevel;  // 0x00
    float flAttributesStrength;  // 0x04
    float flAttributesDexterity;  // 0x08
    float flAttributesIntelligence;  // 0x0c
    float flAttributesVitality;  // 0x10
    float flAttributesHitPoints;  // 0x14
    float flAttributesHPRange;  // 0x18
    float flAttributesHPRegen;  // 0x1c
    float flAttributesPrimaryResourceBase;  // 0x20
    float flAttributesPrimaryResourceRegen;  // 0x24
    float flAttributesArmor;  // 0x28
    float flAttributesDamage;  // 0x2c
    float flAttributesDamageRange;  // 0x30
    float flAttributesDamageFire;  // 0x34
    float flAttributesDamageRangeFire;  // 0x38
    float flAttributesDamageLightning;  // 0x3c
    float flAttributesDamageRangeLightning;  // 0x40
    float flAttributesDamageCold;  // 0x44
    float flAttributesDamageRangeCold;  // 0x48
    float flAttributesDamagePoison;  // 0x4c
    float flAttributesDamageRangePoison;  // 0x50
    float flAttributesDamageArcane;  // 0x54
    float flAttributesDamageRangeArcane;  // 0x58
    float flAttributesDamageHoly;  // 0x5c
    float flAttributesDamageRangeHoly;  // 0x60
    float flAttributesDamageSiege;  // 0x64
    float flAttributesDamageRangeSiege;  // 0x68
    float flAttributesHirelingHitPoints;  // 0x6c
    float flAttributesHirelingHPRange;  // 0x70
    float flAttributesHirelingHPRegen;  // 0x74
    float flAttributesHirelingDamage;  // 0x78
    float flAttributesHirelingDamageRange;  // 0x7c
    float flAttributesHirelingRetrainCost;  // 0x80
    float flAttributesGetHitDamage;  // 0x84
    float flAttributesGetHitDamageScalar;  // 0x88
    float flAttributesGetHitMax;  // 0x8c
    float flAttributesGetHitRecovery;  // 0x90
    float flAttributesWalkSpeed;  // 0x94
    float flAttributesRunSpeed;  // 0x98
    float flAttributesSprintSpeed;  // 0x9c
    float flAttributesStrafeSpeed;  // 0xa0
    float flAttributesAttackSpeed;  // 0xa4
    float flAttributesProjSpeed;  // 0xa8
    float flAttributesExperience;  // 0xac
    float flAttributesResistPhysical;  // 0xb0
    float flAttributesResistFire;  // 0xb4
    float flAttributesResistLightning;  // 0xb8
    float flAttributesResistCold;  // 0xbc
    float flAttributesResistPoison;  // 0xc0
    float flAttributesResistArcane;  // 0xc4
    float flAttributesResistSiege;  // 0xc8
    float flAttributesResistChill;  // 0xcc
    float flAttributesResistStun;  // 0xd0
    float flAttributesConsoleHealthScalar;  // 0xd4
    float flAttributesConsoleDamageScalar;  // 0xd8
    float flAttributesMonster1AffixWeight;  // 0xdc
    float flAttributesMonster2AffixWeight;  // 0xe0
    float flAttributesMonster3AffixWeight;  // 0xe4
    float flAttributesMonster4AffixWeight;  // 0xe8
    uint32 dwPad;  // 0xec
};
static_assert(sizeof(MonsterLevelDefinition) == 0xf0, "MonsterLevelDefinition");

struct MonsterLevelTable {  // size 0x10
    SerializeData serMonsterLevels;  // 0x00
    XPtr64<MonsterLevelDefinition> ptMonsterLevels;  // 0x08
};
static_assert(sizeof(MonsterLevelTable) == 0x10, "MonsterLevelTable");

struct RareMonsterNamesEntry {  // size 0xa0
    GBHeader tHeader;  // 0x00
    AffixType eAffixType;  // 0x18
    CHAR szAffixEffect[128];  // 0x1c
    uint32 dwPad;  // 0x9c
};
static_assert(sizeof(RareMonsterNamesEntry) == 0xa0, "RareMonsterNamesEntry");

struct ParagonBonus {  // size 0x1f0
    GBHeader tHeader;  // 0x00
    int32 nPointsMax;  // 0x18
    int32 unk_1c;  // 0x1c
    AttributeSpecifier ptAttributes[4];  // 0x20
    AttributeSpecifier unk_80[4];  // 0x80
    int32 nCategory;  // 0xe0
    int32 nIndex;  // 0xe4
    PlayerClass ePlayerClass;  // 0xe8
    CHAR szIcon[256];  // 0xec
};
static_assert(sizeof(ParagonBonus) == 0x1f0, "ParagonBonus");

struct ParagonBonusesTable {  // size 0x10
    SerializeData serParagonBonuses;  // 0x00
    XPtr64<ParagonBonus> ptParagonBonuses;  // 0x08
};
static_assert(sizeof(ParagonBonusesTable) == 0x10, "ParagonBonusesTable");

const int32 MAX_SET_ITEM_BONUS_ATTRIBUTES = 8;  // line 1380

struct SetItemBonusTableEntry {  // size 0xe0
    GBHeader tHeader;  // 0x00
    GBID gbidSet;  // 0x18
    int32 nRequired;  // 0x1c
    AttributeSpecifier ptSetItemBonusAttributes[MAX_SET_ITEM_BONUS_ATTRIBUTES];  // 0x20
};
static_assert(sizeof(SetItemBonusTableEntry) == 0xe0, "SetItemBonusTableEntry");

struct SocketedEffectsTableEntry {  // size 0x498
    GBHeader tHeader;  // 0x00
    GBID gbidItemBeingSocketed;  // 0x18
    GBID gbidItemTypeSocketedInto;  // 0x1c
    AttributeSpecifier ptAttributes[3];  // 0x20
    AttributeSpecifier ptRequirements[2];  // 0x68
    CHAR szStringlistLabel[1024];  // 0x98
};
static_assert(sizeof(SocketedEffectsTableEntry) == 0x498, "SocketedEffectsTableEntry");

typedef int32 PrimaryDamageAttribute;

typedef int32 ResourceType;

struct HeroData {  // size 0x108
    GBHeader tHeader;  // 0x00
    SNO snoActorMale;  // 0x18
    SNO snoActorFemale;  // 0x1c
    SNO snoInventory;  // 0x20
    int32 nMaxTrainableSkills;  // 0x24
    SNO snoStartingLMBSkill;  // 0x28
    SNO snoStartingRMBSkill;  // 0x2c
    SNO arSkillKit[4];  // 0x30
    ResourceType ePrimaryResourceType;  // 0x40
    ResourceType eSecondaryResourceType;  // 0x44
    PrimaryDamageAttribute ePrimaryDamageAttribute;  // 0x48
    float flPlayerAwarenessRadius;  // 0x4c
    BOOL bIsRanged;  // 0x50
    float flAttributesStrength;  // 0x54
    float flAttributesDexterity;  // 0x58
    float flAttributesIntelligence;  // 0x5c
    float flAttributesVitality;  // 0x60
    float flAttributesHitPoints;  // 0x64
    float flAttributesHitPointsFactorLevel;  // 0x68
    float flAttributesHPRegen;  // 0x6c
    float flAttributesClassDamageReductionPercent;  // 0x70
    float flAttributesClassDamageReductionPercentPVP;  // 0x74
    float flAttributesPrimaryResourceBase;  // 0x78
    float flAttributesPrimaryResourceFactorLevel;  // 0x7c
    float flAttributesPrimaryResourceRegen;  // 0x80
    float flAttributesSecondaryResourceBase;  // 0x84
    float flAttributesSecondaryResourceFactorLevel;  // 0x88
    float flAttributesSecondaryResourceRegen;  // 0x8c
    float flAttributesArmor;  // 0x90
    float flAttributesDamage;  // 0x94
    float flAttributesWalkSpeed;  // 0x98
    float flAttributesRunSpeed;  // 0x9c
    float flAttributesSprintSpeed;  // 0xa0
    float flAttributesProjSpeed;  // 0xa4
    float flAttributesCritDamageCap;  // 0xa8
    float flAttributesCritPercentBase;  // 0xac
    float flAttributesCritPercentCap;  // 0xb0
    float flAttributesDodgeRatingBase;  // 0xb4
    float flAttributesGetHitMaxBase;  // 0xb8
    float flAttributesGetHitMaxPerLevel;  // 0xbc
    float flAttributesGetHitRecoveryBase;  // 0xc0
    float flAttributesGetHitRecoveryPerLevel;  // 0xc4
    float flAttributesResistPhysical;  // 0xc8
    float flAttributesResistFire;  // 0xcc
    float flAttributesResistLightning;  // 0xd0
    float flAttributesResistCold;  // 0xd4
    float flAttributesResistPoison;  // 0xd8
    float flAttributesResistArcane;  // 0xdc
    float flAttributesResistChill;  // 0xe0
    float flAttributesResistStun;  // 0xe4
    float flAttributesKnockbackWeight;  // 0xe8
    float flAttributesOOCHealthRegenPercent;  // 0xec
    float flAttributesOOCManaRegenPercent;  // 0xf0
    float flAttributesPotionDilutionDuration;  // 0xf4
    float flAttributesPotionDilutionScalar;  // 0xf8
    float flAttributesDualWieldBothAttackChance;  // 0xfc
    float flAttributesFreezeCapacity;  // 0x100
    float flAttributesThawRate;  // 0x104
};
static_assert(sizeof(HeroData) == 0x108, "HeroData");
static_assert(__builtin_offsetof(HeroData, flPlayerAwarenessRadius) == 0x4c, "HeroData::flPlayerAwarenessRadius");
static_assert(__builtin_offsetof(HeroData, bIsRanged) == 0x50, "HeroData::bIsRanged");
static_assert(__builtin_offsetof(HeroData, flAttributesThawRate) == 0x104, "HeroData::flAttributesThawRate");

struct HandicapValues {  // size 0x1c, line 688
    float flHealthScalar;  // 0x00
    float flDamageScalar;  // 0x04
    float flMagicFind;  // 0x08
    float flGoldFind;  // 0x0c
    float flXPBonus;  // 0x10
    float flCrushingBlowScalar;  // 0x14
    uint32 nSignificantKillPlatinum;  // 0x18
};
static_assert(sizeof(HandicapValues) == 0x1c, "HandicapValues");

struct HandicapLevel {  // size 0x20
    HandicapValues tHandicapValues;  // 0x00
    uint32 dwPad;  // 0x1c
};
static_assert(sizeof(HandicapLevel) == 0x20, "HandicapLevel");

struct HandicapLevelTable {  // size 0x10
    SerializeData serHandicapLevels;  // 0x00
    XPtr64<HandicapLevel> ptHandicapLevels;  // 0x08
};
static_assert(sizeof(HandicapLevelTable) == 0x10, "HandicapLevelTable");

struct ItemSalvageLevel {  // size 0x10
    SNO snoSalvageTreasureClassNormal;  // 0x00
    SNO snoSalvageTreasureClassMagic;  // 0x04
    SNO snoSalvageTreasureClassRare;  // 0x08
    SNO snoSalvageTreasureClassLegendary;  // 0x0c
};
static_assert(sizeof(ItemSalvageLevel) == 0x10, "ItemSalvageLevel");

struct ItemSalvageLevelTable {  // size 0x10
    SerializeData serItemSalvageLevels;  // 0x00
    XPtr64<ItemSalvageLevel> ptItemSalvageLevels;  // 0x08
};
static_assert(sizeof(ItemSalvageLevelTable) == 0x10, "ItemSalvageLevelTable");

// Original ExperienceCommon.cpp CU: GameBalanceDefinition.h lines 709-800.
struct AttributeAwardDefinition {  // size 0x10
    int32 nStrength;  // 0x00
    int32 nDexterity;  // 0x04
    int32 nIntelligence;  // 0x08
    int32 nVitality;  // 0x0c
};
static_assert(sizeof(AttributeAwardDefinition) == 0x10, "AttributeAwardDefinition");

struct PVPAwardDefinition {  // size 0x18
    int32 nGold;  // 0x00
    int32 nGoldByRank;  // 0x04
    int32 nXP;  // 0x08
    int32 nPVPXP;  // 0x0c
    int32 nTokens;  // 0x10
    int32 nAltXP;  // 0x14
};
static_assert(sizeof(PVPAwardDefinition) == 0x18, "PVPAwardDefinition");

struct ExperienceLevel {  // size 0x1f0
    uint64 nDeltaXP;  // 0x00
    int32 nDeltaXPClassic;  // 0x08
    int32 nPlayerXPValue;  // 0x0c
    float flDurabilityLossPct;  // 0x10
    float flLifePerVitality;  // 0x14
    int32 nQuestTierXP[7];  // 0x18
    int32 nQuestTierGold[6];  // 0x34
    int32 nLoreTierXP[3];  // 0x4c
    int32 nChallengeBonusXP[5];  // 0x58
    int32 nChallengeBonusGold[5];  // 0x6c
    int32 nBountyXP[5];  // 0x80
    int32 nBountyGold[5];  // 0x94
    int32 nKillCounterXP;  // 0xa8
    int32 nSkillPointAward;  // 0xac
    float flCritMultiplier;  // 0xb0
    float flDodgeMultiplier;  // 0xb4
    float flLifeStealMultiplier;  // 0xb8
    AttributeAwardDefinition tAttributeAward[7];  // 0xbc
    AttributeAwardDefinition tHirelingAttributeAward[4];  // 0x12c
    float arHirelingDamageAbsorbPercent[4];  // 0x16c
    uint32 padding4Bytes;  // 0x17c
    uint64 nPVPXPToLevel;  // 0x180
    PVPAwardDefinition tPVPAward[3];  // 0x188
    float flGoldCostLevelScalar;  // 0x1d0
    int32 nSidekickPrimaryStatIdeal;  // 0x1d4
    int32 nSidekickVitalityIdeal;  // 0x1d8
    int32 nSidekickTotalArmorIdeal;  // 0x1dc
    int32 nSidekickTotalResistIdeal;  // 0x1e0
    int32 nSidekickLifeOnHitIdeal;  // 0x1e4
    int32 nSidekickTargetDPSIdeal;  // 0x1e8
    float flGearXPScalar;  // 0x1ec
};
static_assert(sizeof(ExperienceLevel) == 0x1f0, "ExperienceLevel");
static_assert(__builtin_offsetof(ExperienceLevel, nDeltaXP) == 0x00, "ExperienceLevel::nDeltaXP");
static_assert(__builtin_offsetof(ExperienceLevel, tAttributeAward) == 0xbc, "ExperienceLevel::tAttributeAward");
static_assert(__builtin_offsetof(ExperienceLevel, tPVPAward) == 0x188, "ExperienceLevel::tPVPAward");
static_assert(__builtin_offsetof(ExperienceLevel, flGearXPScalar) == 0x1ec, "ExperienceLevel::flGearXPScalar");

struct ExperienceTable {  // size 0x10
    SerializeData serExperienceLevels;  // 0x00
    XPtr64<ExperienceLevel> ptExperienceLevels;  // 0x08
};
static_assert(sizeof(ExperienceTable) == 0x10, "ExperienceTable");
static_assert(__builtin_offsetof(ExperienceTable, ptExperienceLevels) == 0x08, "ExperienceTable::ptExperienceLevels");

struct ExperienceLevelAlt {  // size 0x80
    uint64 nDeltaXP;  // 0x00
    int32 nDeltaXPClassic;  // 0x08
    uint32 dwPad;  // 0x0c
    AttributeAwardDefinition tAttributeAward[7];  // 0x10
};
static_assert(sizeof(ExperienceLevelAlt) == 0x80, "ExperienceLevelAlt");
static_assert(__builtin_offsetof(ExperienceLevelAlt, tAttributeAward) == 0x10, "ExperienceLevelAlt::tAttributeAward");

struct ExperienceTableAlt {  // size 0x10
    SerializeData serExperienceLevelsAlt;  // 0x00
    XPtr64<ExperienceLevelAlt> ptExperienceLevelsAlt;  // 0x08
};
static_assert(sizeof(ExperienceTableAlt) == 0x10, "ExperienceTableAlt");
static_assert(__builtin_offsetof(ExperienceTableAlt, ptExperienceLevelsAlt) == 0x08, "ExperienceTableAlt::ptExperienceLevelsAlt");

struct TieredLootRunLevelValues {  // size 0x38
    float flHealthScalar;  // 0x00
    float flDamageScalar;  // 0x04
    float flMagicFind;  // 0x08
    float flGoldFind;  // 0x0c
    float flXPBonus;  // 0x10
    float flLegendaryDropBonus;  // 0x14
    float flBloodShardBonus;  // 0x18
    float flAncientLegendaryChance;  // 0x1c
    int32 nJewelUpgradeCharges;  // 0x20
    uint32 nSignificantKillPlatinum;  // 0x24
    uint64 nEmpowerCost;  // 0x28
    float flMonsterHPPerAdditionalPlayer;  // 0x30
    float flXPReductionScalar;  // 0x34
};
static_assert(sizeof(TieredLootRunLevelValues) == 0x38, "TieredLootRunLevelValues");

struct TieredLootRunLevel {  // size 0x38
    TieredLootRunLevelValues tLootRunValues;  // 0x00
};
static_assert(sizeof(TieredLootRunLevel) == 0x38, "TieredLootRunLevel");

struct TieredLootRunLevelTable {  // size 0x10
    SerializeData serTieredLootRunLevels;  // 0x00
    XPtr64<TieredLootRunLevel> ptTieredLootRunLevels;  // 0x08
};
static_assert(sizeof(TieredLootRunLevelTable) == 0x10, "TieredLootRunLevelTable");

// The GameBalanceDefinition tables (GameBalanceDefinition.h, from the DWARF). The element types
// that are not defined in this header are only forward-declared here.
struct ItemTypeDefinition;
struct ItemDefinition;
struct HelpCode;
struct AffixTableEntry;
struct HeroData;
struct MovementStyleDefinition;
struct LabelGBID;
struct LootDistEntry;
struct RareItemNamesEntry;
struct MonsterAffixesEntry;
struct RareMonsterNamesEntry;
struct SocketedEffectsTableEntry;
struct ItemDropTableEntry;
struct ItemLevelModifier;
struct QualityClass;
struct HirelingEntry;
struct SetItemBonusTableEntry;
struct EliteModifierData;
struct ItemTierData;
struct PowerFormulaTableEntry;
struct RecipeEntry;
struct ScriptedAchievementEvent;
struct LootRunQuestTierEntry;
struct LegacyItemConversion;
struct EnchantItemAffixUseCountCostScalar;
struct TransmuteRecipe;
struct CurrencyConversionTableEntry;
struct ItemTypeTable {  // size 0x10
    SerializeData serItemTypes;  // 0x00
    XPtr64<ItemTypeDefinition> ptItemTypes;  // 0x08
};
static_assert(sizeof(ItemTypeTable) == 0x10, "ItemTypeTable");
struct ItemTable {  // size 0x10
    SerializeData serItems;  // 0x00
    XPtr64<ItemDefinition> ptItems;  // 0x08
};
static_assert(sizeof(ItemTable) == 0x10, "ItemTable");
struct HelpCodeTable {  // size 0x10
    SerializeData serHelpCodes;  // 0x00
    XPtr64<HelpCode> ptHelpCodes;  // 0x08
};
static_assert(sizeof(HelpCodeTable) == 0x10, "HelpCodeTable");
struct AffixTable {  // size 0x10
    SerializeData serAffixes;  // 0x00
    XPtr64<AffixTableEntry> ptAffixes;  // 0x08
};
static_assert(sizeof(AffixTable) == 0x10, "AffixTable");
struct HeroTable {  // size 0x10
    SerializeData serHeros;  // 0x00
    XPtr64<HeroData> ptHeros;  // 0x08
};
static_assert(sizeof(HeroTable) == 0x10, "HeroTable");
struct MovementStyleTable {  // size 0x10
    SerializeData serMovementStyles;  // 0x00
    XPtr64<MovementStyleDefinition> ptMovementStyles;  // 0x08
};
static_assert(sizeof(MovementStyleTable) == 0x10, "MovementStyleTable");
struct LabelGBIDTable {  // size 0x10
    SerializeData serLabelGBIDs;  // 0x00
    XPtr64<LabelGBID> ptLabelGBIDs;  // 0x08
};
static_assert(sizeof(LabelGBIDTable) == 0x10, "LabelGBIDTable");
struct LootDistributionTable {  // size 0x10
    SerializeData serLootDistributions;  // 0x00
    XPtr64<LootDistEntry> ptLootDistributions;  // 0x08
};
static_assert(sizeof(LootDistributionTable) == 0x10, "LootDistributionTable");
struct RareItemNamesTable {  // size 0x10
    SerializeData serRareItemNames;  // 0x00
    XPtr64<RareItemNamesEntry> ptRareItemNames;  // 0x08
};
static_assert(sizeof(RareItemNamesTable) == 0x10, "RareItemNamesTable");
struct MonsterAffixesTable {  // size 0x10
    SerializeData serMonsterAffixes;  // 0x00
    XPtr64<MonsterAffixesEntry> ptMonsterAffixes;  // 0x08
};
static_assert(sizeof(MonsterAffixesTable) == 0x10, "MonsterAffixesTable");
struct RareMonsterNamesTable {  // size 0x10
    SerializeData serRareMonsterNames;  // 0x00
    XPtr64<RareMonsterNamesEntry> ptRareMonsterNames;  // 0x08
};
static_assert(sizeof(RareMonsterNamesTable) == 0x10, "RareMonsterNamesTable");
struct SocketedEffectsTable {  // size 0x10
    SerializeData serSocketedEffects;  // 0x00
    XPtr64<SocketedEffectsTableEntry> ptSocketedEffects;  // 0x08
};
static_assert(sizeof(SocketedEffectsTable) == 0x10, "SocketedEffectsTable");
struct ItemDropTable {  // size 0x10
    SerializeData serItemDrops;  // 0x00
    XPtr64<ItemDropTableEntry> ptItemDrops;  // 0x08
};
static_assert(sizeof(ItemDropTable) == 0x10, "ItemDropTable");
struct ItemLevelModifierTable {  // size 0x10
    SerializeData serItemLevelModifiers;  // 0x00
    XPtr64<ItemLevelModifier> ptItemLevelModifiers;  // 0x08
};
static_assert(sizeof(ItemLevelModifierTable) == 0x10, "ItemLevelModifierTable");
struct QualityClassTable {  // size 0x10
    SerializeData serQualityClasses;  // 0x00
    XPtr64<QualityClass> ptQualityClasses;  // 0x08
};
static_assert(sizeof(QualityClassTable) == 0x10, "QualityClassTable");
struct HirelingTable {  // size 0x10
    SerializeData serHirelings;  // 0x00
    XPtr64<HirelingEntry> ptHirelings;  // 0x08
};
static_assert(sizeof(HirelingTable) == 0x10, "HirelingTable");
struct SetItemBonusTable {  // size 0x10
    SerializeData serSetItemBonuses;  // 0x00
    XPtr64<SetItemBonusTableEntry> ptSetItemBonuses;  // 0x08
};
static_assert(sizeof(SetItemBonusTable) == 0x10, "SetItemBonusTable");
struct EliteModifierTable {  // size 0x10
    SerializeData serMods;  // 0x00
    XPtr64<EliteModifierData> ptMods;  // 0x08
};
static_assert(sizeof(EliteModifierTable) == 0x10, "EliteModifierTable");
struct ItemTierTable {  // size 0x10
    SerializeData serTiers;  // 0x00
    XPtr64<ItemTierData> ptTiers;  // 0x08
};
static_assert(sizeof(ItemTierTable) == 0x10, "ItemTierTable");
struct PowerFormulaTable {  // size 0x10
    SerializeData serPowerFormulaTableEntries;  // 0x00
    XPtr64<PowerFormulaTableEntry> ptPowerFormulaTableEntries;  // 0x08
};
static_assert(sizeof(PowerFormulaTable) == 0x10, "PowerFormulaTable");
struct RecipesTable {  // size 0x10
    SerializeData serRecipes;  // 0x00
    XPtr64<RecipeEntry> ptRecipes;  // 0x08
};
static_assert(sizeof(RecipesTable) == 0x10, "RecipesTable");
struct ScriptedAchievementEventsTable {  // size 0x10
    SerializeData serScriptedAchievementEvents;  // 0x00
    XPtr64<ScriptedAchievementEvent> ptScriptedAchievementEvents;  // 0x08
};
static_assert(sizeof(ScriptedAchievementEventsTable) == 0x10, "ScriptedAchievementEventsTable");
struct LootRunQuestTierTable {  // size 0x10
    SerializeData serLootRunQuestTiers;  // 0x00
    XPtr64<LootRunQuestTierEntry> ptLootRunQuestTiers;  // 0x08
};
static_assert(sizeof(LootRunQuestTierTable) == 0x10, "LootRunQuestTierTable");
struct LegacyItemConversionTable {  // size 0x10
    SerializeData serLegacyItemConversions;  // 0x00
    XPtr64<LegacyItemConversion> ptLegacyItemConversions;  // 0x08
};
static_assert(sizeof(LegacyItemConversionTable) == 0x10, "LegacyItemConversionTable");
struct EnchantItemAffixUseCountCostScalarsTable {  // size 0x10
    SerializeData serEnchantItemAffixUseCountCostScalars;  // 0x00
    XPtr64<EnchantItemAffixUseCountCostScalar> ptEnchantItemAffixUseCountCostScalars;  // 0x08
};
static_assert(sizeof(EnchantItemAffixUseCountCostScalarsTable) == 0x10, "EnchantItemAffixUseCountCostScalarsTable");
struct TransmuteRecipesTable {  // size 0x10
    SerializeData serRecipes;  // 0x00
    XPtr64<TransmuteRecipe> ptRecipes;  // 0x08
};
static_assert(sizeof(TransmuteRecipesTable) == 0x10, "TransmuteRecipesTable");
struct CurrencyConversionTable {  // size 0x10
    SerializeData serCurrencyConversionEntries;  // 0x00
    XPtr64<CurrencyConversionTableEntry> ptCurrencyConversionEntries;  // 0x08
};
static_assert(sizeof(CurrencyConversionTable) == 0x10, "CurrencyConversionTable");

struct GameBalanceDefinition {  // size 0x238
    RequiredDefinitionHeader tHeader;  // 0x00
    GameBalanceType eGameBalanceType;  // 0x0c
    uint32 dwMachineTime;  // 0x10
    BOOL bIgnoreOnLoad;  // 0x14
    ItemTypeTable tItemTypeTable;  // 0x18
    ItemTable tItemTable;  // 0x28
    ExperienceTable tExperienceTable;  // 0x38
    ExperienceTableAlt tExperienceTableAlt;  // 0x48
    HelpCodeTable tHelpCodesTable;  // 0x58
    MonsterLevelTable tMonsterLevelTable;  // 0x68
    AffixTable tAffixTable;  // 0x78
    HeroTable tHeroTable;  // 0x88
    MovementStyleTable tMovementStyleTable;  // 0x98
    LabelGBIDTable tLabelGBIDTable;  // 0xa8
    LootDistributionTable tLootDistTable;  // 0xb8
    RareItemNamesTable tRareItemNamesTable;  // 0xc8
    MonsterAffixesTable tMonsterAffixesTable;  // 0xd8
    RareMonsterNamesTable tMonsterNamesTable;  // 0xe8
    SocketedEffectsTable tSocketedEffectTable;  // 0xf8
    ItemDropTable tItemDropTable;  // 0x108
    ItemLevelModifierTable tItemLevelModTable;  // 0x118
    QualityClassTable tQualityClassTable;  // 0x128
    HandicapLevelTable tHandicapLevelTable;  // 0x138
    ItemSalvageLevelTable tItemSalvageLevelTable;  // 0x148
    HirelingTable tHirelingTable;  // 0x158
    SetItemBonusTable tSetItemBonusTable;  // 0x168
    EliteModifierTable tEliteModTable;  // 0x178
    ItemTierTable tItemTierTable;  // 0x188
    PowerFormulaTable tPowerFormulaTable;  // 0x198
    RecipesTable tRecipeTable;  // 0x1a8
    ScriptedAchievementEventsTable tScriptedAchievementEventsTable;  // 0x1b8
    LootRunQuestTierTable tLootRunQuestTierTable;  // 0x1c8
    ParagonBonusesTable tParagonBonusesTable;  // 0x1d8
    LegacyItemConversionTable tLegacyItemConversionTable;  // 0x1e8
    EnchantItemAffixUseCountCostScalarsTable tEnchantItemAffixUseCountCostScalarsTable;  // 0x1f8
    TieredLootRunLevelTable tTieredLootRunLevelTable;  // 0x208
    TransmuteRecipesTable tTransmuteRecipeTable;  // 0x218
    CurrencyConversionTable tCurrencyConversionTable;  // 0x228
};
static_assert(sizeof(GameBalanceDefinition) == 0x238, "GameBalanceDefinition");
static_assert(__builtin_offsetof(GameBalanceDefinition, tExperienceTable) == 0x38, "tExperienceTable");
static_assert(__builtin_offsetof(GameBalanceDefinition, tExperienceTableAlt) == 0x48, "tExperienceTableAlt");
static_assert(__builtin_offsetof(GameBalanceDefinition, tHandicapLevelTable) == 0x138, "tHandicapLevelTable");
static_assert(__builtin_offsetof(GameBalanceDefinition, tItemSalvageLevelTable) == 0x148, "tItemSalvageLevelTable");
static_assert(__builtin_offsetof(GameBalanceDefinition, tTieredLootRunLevelTable) == 0x208, "tTieredLootRunLevelTable");

class GameBalanceGroupDef : public GroupDefinition {  // size 0xa0, line 1440
public:
    GameBalanceGroupDef();
    GROUPDEFINITION_MEMORY_FUNCTIONS()  // vtable slots 2-5; the macro name is invented, see Definitions.h
    virtual uint32 GetMaxCount();  // vtable slot 11
    virtual BOOL CanReferenceOtherSNOs() const;  // vtable slot 13
    virtual BOOL SNOPackDestinationIsVariable() const;  // vtable slot 6
    virtual SNOPackType GetSNOPackDestination(const void * ptDefinition) const;  // vtable slot 7
    virtual BOOL Write(void * lpDefinition, FileReference & tRef) const;  // vtable slot 19
    virtual void Postprocess(void * lpDefinition);  // vtable slot 17
    virtual void Delete(void * lpDefinition) const;  // vtable slot 18
};
static_assert(sizeof(GameBalanceGroupDef) == 0xa0, "GameBalanceGroupDef");

extern GameBalanceGroupDef * gptGroupDefGameBalance;  // GameBalanceDefinition.cpp

typedef ScopedDefinitionLockR<GameBalanceDefinition> ScopedGameBalanceDefinitionLockR;  // line 1457

// line 2015. The lock is LockRFast and the unlock is empty: no UnlockR appears in any caller
// (X/Common/Game.cpp GameGetInfoForHandicapLevel, GameGetHandicapDamageScalar, ...).
template <> inline const GameBalanceDefinition * DefinitionLockR<GameBalanceDefinition>(SNO sno) {
    return (const GameBalanceDefinition *)gptGroupDefGameBalance->LockRFast(sno);
}
template <> inline void DefinitionUnlockR<GameBalanceDefinition>(const GameBalanceDefinition *) {}

// line 2195. A template, so the DWARF only shows instantiations; the bodies are written from the
// copies inlined into UIConsoleDyeItemView.cpp (UpdateUI).
template <typename _type, int gbType>
class ScopedGBLockR {  // size 0x10
public:
    ScopedGBLockR(GBID gbid)  // line 2199
    {
        GBHandle gbHandle(gbType, gbid);
        m_ptData = (const _type *)GBLockR(gbHandle, m_pdefGB, TRUE);
    }

    ~ScopedGBLockR()  // line 2206
    {
        if (m_pdefGB != NULL)
        {
            GBUnlockR(m_pdefGB);
        }
    }

    operator const _type *() { return m_ptData; }  // line 2214
    const _type * operator->() { return m_ptData; }  // line 2215

private:
    const GameBalanceDefinition * m_pdefGB;  // 0x00
    const _type * m_ptData;  // 0x08
};

typedef ScopedGBLockR<ItemDefinition, 2> ScopedItemDefinitionLockR;

typedef ScopedGBLockR<RareMonsterNamesEntry, 19> ScopedRareMonsterNameEntryLockR;

typedef ScopedGBLockR<ParagonBonus, 40> ScopedParagonBonusLockR;

typedef int32 CurrencyType;  // X/Common/Items.h
struct CurrencyConversionTableEntry {  // size 0x40
    GBHeader tHeader;  // 0x00
    CurrencyType eCurrency;  // 0x18
    GBID arItemGBIDs[5];  // 0x1c
    uint32 nSortOrder;  // 0x30
    BOOL bHidden;  // 0x34
    BOOL bAutoPickup;  // 0x38
    uint32 dwPad;  // 0x3c
};
static_assert(sizeof(CurrencyConversionTableEntry) == 0x40, "CurrencyConversionTableEntry");

typedef ScopedGBLockR<CurrencyConversionTableEntry, 51> ScopedCurrencyConversionEntryLockR;

typedef int32 CrafterType;
struct RecipeEntry {  // size 0x60
    GBHeader tHeader;  // 0x00
    SNO snoRecipe;  // 0x18
    CrafterType eCrafterType;  // 0x1c
    uint32 dwFlags;  // 0x20
    int32 nLevel;  // 0x24
    uint32 nCost;  // 0x28
    int32 nNumIngredients;  // 0x2c
    RecipeIngredient tIngredient[6];  // 0x30
};
static_assert(sizeof(RecipeEntry) == 0x60, "RecipeEntry");
typedef ScopedGBLockR<RecipeEntry, 32> ScopedRecipeEntryLockR;
enum RecipeFlags {  // line 1581
    RECIPE_DROPPED_BIT = 0,
    RECIPE_DYNAMIC_RLEVEL_BIT = 1,
};
enum { IQL_LEGENDARY = 9 }; // Original item quality enum.

// GameBalanceDefinition.cpp:2348.
CRefString GBIDToString(const GBID & gbid);

enum TreasureClassChooseType {
    TCC_NONE = -1,
    TCC_ONE_OF = 0,
    TCC_ALL_OF = 1,
    TCC_ALL_OF_SHARED = 2,
    TCC_ONE_OF_SHARED = 3,
    TCC_COUNT = 4,
};

// Item type records (reference lines 498-545, 1469); X/Common/Items.cpp needs them.
static const int32 ITEM_LABEL_MASK_DWORDS = 5;  // line 457
enum {
    IT_USEABLE_BIT = 0,
    IT_ALWAYSMAGIC_BIT = 1,
    IT_2HANDED_BIT_DEPRECATED = 2,
    IT_SOCKETABLE_BIT = 3,
    IT_FITS_IN_BELT_BIT_DEPRECATED = 4,
    IT_UNKNOWN_ON_CREATE_DEPRECATED = 5,
    IT_NEVER_MAGIC_BIT = 6,
    IT_ENCHANTMENT_INGREDIENT_BIT = 7,
    IT_USABLE_BARBARIAN_BIT = 8,
    IT_USABLE_WIZARD_BIT = 9,
    IT_USABLE_WITCHDOCTOR_BIT = 10,
    IT_USABLE_DEMONHUNTER_BIT = 11,
    IT_USABLE_HOTBAR_BIT = 12,
    IT_USABLE_MONK_BIT = 13,
    IT_USABLE_TEMPLAR_BIT = 14,
    IT_USABLE_SCOUNDREL_BIT = 15,
    IT_USABLE_ENCHANTRESS_BIT = 16,
    IT_USABLE_CRUSADER_BIT = 17,
    IT_CURRENCY_BIT_DEPRECATED = 18,
    IT_PICKUP_UNLOCKS_TRANSMOG_BIT = 19,
    IT_USABLE_NECROMANCER_BIT = 20,
};

struct ItemTypeDefinition {  // size 0x60
    GBHeader tHeader;  // 0x00
    GBID gbidParent;  // 0x18
    int32 nLootLevelRange;  // 0x1c
    int32 nReqCrafterLevelForEnchant;  // 0x20
    int32 nMaxSockets;  // 0x24
    uint32 dwFlags;  // 0x28
    int32 eBodySlot1;  // 0x2c (InventorySlotType)
    int32 eBodySlot2;  // 0x30
    int32 eBodySlot3;  // 0x34
    int32 eBodySlot4;  // 0x38
    GBID gbidInherentAffix[3];  // 0x3c
    GBID gbidInherentAffixFamily[1];  // 0x48
    uint32 dwItemLabelMask[5];  // 0x4c
};
static_assert(sizeof(ItemTypeDefinition) == 0x60, "ItemTypeDefinition");

typedef ScopedGBLockR<ItemTypeDefinition, 1> ScopedItemTypeDefinitonLockR;  // line 1469 (sic)

// The AI state machine's per-actor data (used by X/Server/AI/AIStates.cpp).
#include "../../Sigma/DataStructures/XPooledList.h"
#include "../../Sigma/DataStructures/XPooledMap.h"
#include "../AI/ActorAIStatesSwig.h"
#include "../CommonTypes.h"  // ACDID

typedef int32 BuffID;  // also X/Server/Powers/Powers.h

struct AIMergeData {  // size 0x1c
    SNOName snoName;  // 0x00
    uint32 uPowerPriority;  // 0x08
    uint32 uEndTime;  // 0x0c
    ACDID idACDSource;  // 0x10
    ACDID idACDCastLocation;  // 0x14
    BuffID idBuff;  // 0x18
};
static_assert(sizeof(AIMergeData) == 0x1c, "AIMergeData");

typedef XListMemoryPool<AIMergeData> TAIMergeDataPool;

typedef XPooledList<AIMergeData, const AIMergeData &> TAIMergeDataList;

typedef XPooledMap<unsigned int, unsigned int, unsigned int, unsigned int, 16> CoolDownMap;

typedef XMapMemoryPool<unsigned int, unsigned int> CoolDownMapPool;

struct AIStateInstanceData {  // size 0x140
    uint32 dwStateEntryTime;  // 0x00
    AIBehaviorType eBehaviorType;  // 0x04
    AIStateType eCurrState;  // 0x08
    SNO arAIStateTable[8];  // 0x0c
    GBID arAIMovementStyles[8];  // 0x2c
    SNO snoLastPower;  // 0x4c
    ACDID idACDLastTarget;  // 0x50
    uint32 dwLastHitTime;  // 0x54
    uint32 dwFlags;  // 0x58
    SNO snoOverridePower;  // 0x5c
    ACDID idACDOverrideTarget;  // 0x60
    TAIMergeDataList listMergeData;  // 0x68
    CoolDownMap mapCoolDown;  // 0x90
    uint32 dwAggressiveStartTime;  // 0x138
};
static_assert(sizeof(AIStateInstanceData) == 0x140, "AIStateInstanceData");

// Element types of the GameBalanceDefinition tables.
typedef int32 ItemEffectType;
typedef int32 AffixGroup;
// line 325
enum {
    TRIT_INVALID = -1,
    TRIT_ANY = 0,
    TRIT_SPECIFIC_GBID = 1,
    TRIT_LEGENDARY = 2,
    TRIT_LEGENDARY_WITH_POWER = 3,
    TRIT_RARE_EQUIPPABLE_ITEM = 4,
    TRIT_SET_ITEM = 5,
    TRIT_UPGRADED_GEM_OF_EASE = 6,
    TRIT_EQUIPPABLE = 7,
    TRIT_NORMAL_MAGIC_RARE_ITEM = 8,
    TRIT_GEM = 9,
    TRIT_BASIC_CRAFTING_MAT = 10,
    TRIT_ACT_BOUNTY_REAGENT = 11,
    TRIT_GEM_CONVERSION_REAGENT = 12,
    TRIT_UPGRADED_LEGENDARY_GEM = 13,
    TRIT_ANCIENT = 14,
    TRIT_MAX_RANK_STANDARD_GEM = 15,
    TRIT_BLACKROCK_PAGE = 16,
    TRIT_NUM_TYPES = 17,
};
typedef int32 TransmuteRecipeIngredientType;
// line 349
enum {
    TRA_NONE = -1,
    TRA_EXTRACT_LEGENDARY_POWER = 0,
    TRA_REFORGE_LEGENDARY = 1,
    TRA_UPGRADE_RARE_ITEM = 2,
    TRA_CONVERT_SET_ITEM = 3,
    TRA_REMOVE_LEVEL_REQ = 4,
    TRA_CONVERT_CRAFTING_MATS_FROM_NORMAL = 5,
    TRA_CONVERT_CRAFTING_MATS_FROM_MAGIC = 6,
    TRA_CONVERT_CRAFTING_MATS_FROM_RARE = 7,
    TRA_CONVERT_GEMS = 8,
    TRA_UPGRADE_ITEM_TO_PLAYER_LEVEL = 9,
    TRA_OPEN_PORTAL_GREED = 10,
    TRA_OPEN_PORTAL_COW = 11,
    TRA_OPEN_PORTAL_WHIMSEYSHIRE = 12,
    TRA_AUGMENT_ARMOR = 13,
    TRA_CONVERT_PAGES = 14,
    TRA_INFUSE_LEGENDARY_POWER = 15,
    TRA_OPEN_PORTAL_ANCIENT_GREED = 16,  // 2.7.7; names from gTransmuteRecipeActionTypes
    TRA_OPEN_PORTAL_SWARM_RIFT = 17,  // 2.7.7
    TRA_PRIMALIZE_LEGENDARY = 18,  // 2.7.7
    TRA_NUM_TYPES = 19,
};
typedef int32 TransmuteRecipeAction;
enum MonsterAffixValidFor {
    VALID_FOR_ALL = 0,
    VALID_FOR_RARES = 1,
    VALID_FOR_SHOOTERS = 2,
    VALID_FOR_CHAMPION = 3,
};

struct AffixTableEntry {  // line 1078, size 0x220
    GBHeader tHeader;  // 0x00
    uint32 dwFlags;  // 0x18
    int32 nFrequencyWeight;  // 0x1c
    int32 nClassFrequencyWeight[7];  // 0x20
    int32 nHirelingFrequencyWeight[4];  // 0x3c
    int32 nAffixLevelMin;  // 0x4c
    int32 nAffixLevelMax;  // 0x50
    int32 nCost;  // 0x54
    int32 nIdentifyCost;  // 0x58
    int32 nNormalRequiredLevel;  // 0x5c
    int32 nCrafterRequiredLevel;  // 0x60
    ItemEffectType eItemEffectType;  // 0x64
    int nItemEffectLevel;  // 0x68
    GBID gbidConvertsTo;  // 0x6c
    GBID gbidLegendaryUprankAffix;  // 0x70
    SNO snoRareNamePrefixStringList;  // 0x74
    SNO snoRareNameSuffixStringList;  // 0x78
    GBID gbidAffixFamily0;  // 0x7c
    GBID gbidAffixFamily1;  // 0x80
    PlayerClass ePlayerClass;  // 0x84
    GBID gbidExclusionCategory;  // 0x88
    GBID arExcludedCategories[6];  // 0x8c
    GBID arAllowedTypes[24];  // 0xa4
    GBID arLegendaryAllowedTypes[24];  // 0x104
    uint32 dwAllowedQualityLevels;  // 0x164
    AffixType eAffixType;  // 0x168
    GBID gbidAssociatedAffix;  // 0x16c
    AttributeSpecifier ptAffixAttributes[4];  // 0x170
    AttributeSpecifier ptRequirements[3];  // 0x1d0
    AffixGroup eAffixGroup;  // 0x218
    uint32 dwExplicit64BitPadding;  // 0x21c
};
static_assert(sizeof(AffixTableEntry) == 0x220, "AffixTableEntry");
static_assert(offsetof(AffixTableEntry, tHeader) == 0x0, "AffixTableEntry::tHeader");
static_assert(offsetof(AffixTableEntry, dwFlags) == 0x18, "AffixTableEntry::dwFlags");
static_assert(offsetof(AffixTableEntry, nFrequencyWeight) == 0x1c, "AffixTableEntry::nFrequencyWeight");
static_assert(offsetof(AffixTableEntry, nClassFrequencyWeight) == 0x20, "AffixTableEntry::nClassFrequencyWeight");
static_assert(offsetof(AffixTableEntry, nHirelingFrequencyWeight) == 0x3c, "AffixTableEntry::nHirelingFrequencyWeight");
static_assert(offsetof(AffixTableEntry, nAffixLevelMin) == 0x4c, "AffixTableEntry::nAffixLevelMin");
static_assert(offsetof(AffixTableEntry, nAffixLevelMax) == 0x50, "AffixTableEntry::nAffixLevelMax");
static_assert(offsetof(AffixTableEntry, nCost) == 0x54, "AffixTableEntry::nCost");
static_assert(offsetof(AffixTableEntry, nIdentifyCost) == 0x58, "AffixTableEntry::nIdentifyCost");
static_assert(offsetof(AffixTableEntry, nNormalRequiredLevel) == 0x5c, "AffixTableEntry::nNormalRequiredLevel");
static_assert(offsetof(AffixTableEntry, nCrafterRequiredLevel) == 0x60, "AffixTableEntry::nCrafterRequiredLevel");
static_assert(offsetof(AffixTableEntry, eItemEffectType) == 0x64, "AffixTableEntry::eItemEffectType");
static_assert(offsetof(AffixTableEntry, nItemEffectLevel) == 0x68, "AffixTableEntry::nItemEffectLevel");
static_assert(offsetof(AffixTableEntry, gbidConvertsTo) == 0x6c, "AffixTableEntry::gbidConvertsTo");
static_assert(offsetof(AffixTableEntry, gbidLegendaryUprankAffix) == 0x70, "AffixTableEntry::gbidLegendaryUprankAffix");
static_assert(offsetof(AffixTableEntry, snoRareNamePrefixStringList) == 0x74, "AffixTableEntry::snoRareNamePrefixStringList");
static_assert(offsetof(AffixTableEntry, snoRareNameSuffixStringList) == 0x78, "AffixTableEntry::snoRareNameSuffixStringList");
static_assert(offsetof(AffixTableEntry, gbidAffixFamily0) == 0x7c, "AffixTableEntry::gbidAffixFamily0");
static_assert(offsetof(AffixTableEntry, gbidAffixFamily1) == 0x80, "AffixTableEntry::gbidAffixFamily1");
static_assert(offsetof(AffixTableEntry, ePlayerClass) == 0x84, "AffixTableEntry::ePlayerClass");
static_assert(offsetof(AffixTableEntry, gbidExclusionCategory) == 0x88, "AffixTableEntry::gbidExclusionCategory");
static_assert(offsetof(AffixTableEntry, arExcludedCategories) == 0x8c, "AffixTableEntry::arExcludedCategories");
static_assert(offsetof(AffixTableEntry, arAllowedTypes) == 0xa4, "AffixTableEntry::arAllowedTypes");
static_assert(offsetof(AffixTableEntry, arLegendaryAllowedTypes) == 0x104, "AffixTableEntry::arLegendaryAllowedTypes");
static_assert(offsetof(AffixTableEntry, dwAllowedQualityLevels) == 0x164, "AffixTableEntry::dwAllowedQualityLevels");
static_assert(offsetof(AffixTableEntry, eAffixType) == 0x168, "AffixTableEntry::eAffixType");
static_assert(offsetof(AffixTableEntry, gbidAssociatedAffix) == 0x16c, "AffixTableEntry::gbidAssociatedAffix");
static_assert(offsetof(AffixTableEntry, ptAffixAttributes) == 0x170, "AffixTableEntry::ptAffixAttributes");
static_assert(offsetof(AffixTableEntry, ptRequirements) == 0x1d0, "AffixTableEntry::ptRequirements");
static_assert(offsetof(AffixTableEntry, eAffixGroup) == 0x218, "AffixTableEntry::eAffixGroup");
static_assert(offsetof(AffixTableEntry, dwExplicit64BitPadding) == 0x21c, "AffixTableEntry::dwExplicit64BitPadding");

typedef ScopedGBLockR<AffixTableEntry, 8> ScopedItemAffixLockR;  // line 1463

struct EliteModifierData {  // size 0x70
    GBHeader tHeader;  // 0x00
    float flStunDR;  // 0x18
    int32 nStunDM;  // 0x1c
    float flCharmDR;  // 0x20
    int32 nCharmDM;  // 0x24
    float flConsumptionDR;  // 0x28
    int32 nConsumptionDM;  // 0x2c
    float flSessileDR;  // 0x30
    int32 nSessileDM;  // 0x34
    float flFearDR;  // 0x38
    int32 nFearDM;  // 0x3c
    float flRootDR;  // 0x40
    int32 nRootDM;  // 0x44
    float flMovementSlowDR;  // 0x48
    int32 nMovementSlowDM;  // 0x4c
    float flMovementSlowER;  // 0x50
    float flAttackSpeedDR;  // 0x54
    int32 nAttackSpeedDM;  // 0x58
    float flAttackSpeedER;  // 0x5c
    float flKnockbackResist;  // 0x60
    float flKnockbackER;  // 0x64
    float flKnockbackMin;  // 0x68
    uint32 dwPad;  // 0x6c
};
static_assert(sizeof(EliteModifierData) == 0x70, "EliteModifierData");

struct EnchantItemAffixUseCountCostScalar {  // size 0x8
    int32 nUseCount;  // 0x00
    float flCostMultiplier;  // 0x04
};
static_assert(sizeof(EnchantItemAffixUseCountCostScalar) == 0x8, "EnchantItemAffixUseCountCostScalar");

struct HelpCode {  // size 0x280
    CHAR szControl[256];  // 0x00
    CHAR szDisplay[256];  // 0x100
    CHAR szCommand[128];  // 0x200
};
static_assert(sizeof(HelpCode) == 0x280, "HelpCode");

struct HirelingEntry {  // size 0x58
    GBHeader tHeader;  // 0x00
    SNO snoActor;  // 0x18
    SNO snoProxyActor;  // 0x1c
    SNO snoSummonedActor;  // 0x20
    SNO snoInventory;  // 0x24
    PrimaryDamageAttribute ePrimaryDamageAttribute;  // 0x28
    float flAttributesStrength;  // 0x2c
    float flAttributesDexterity;  // 0x30
    float flAttributesIntelligence;  // 0x34
    float flAttributesVitality;  // 0x38
    float flAttributesCritDamageCap;  // 0x3c
    float flAttributesCritPercentBase;  // 0x40
    float flAttributesCritPercentCap;  // 0x44
    float flAttributesDodgeRatingBase;  // 0x48
    float flAttributesHirelingLightLookLevel;  // 0x4c
    float flAttributesHirelingMediumLookLevel;  // 0x50
    float flAttributesHirelingHeavyLookLevel;  // 0x54
};
static_assert(sizeof(HirelingEntry) == 0x58, "HirelingEntry");

struct ItemDropTableEntry {  // size 0x388
    GBHeader tHeader;  // 0x00
    int32 pnDropWeights[110];  // 0x18
    int32 pnVendorDropWeights[110];  // 0x1d0
};
static_assert(sizeof(ItemDropTableEntry) == 0x388, "ItemDropTableEntry");

struct ItemLevelModifier {  // size 0x5c
    int32 nLevelDiff;  // 0x00
    float flQualityLevelScalar[11];  // 0x04
    int32 nSocketWeight[11];  // 0x30
};
static_assert(sizeof(ItemLevelModifier) == 0x5c, "ItemLevelModifier");

struct ItemTierData {  // size 0x20
    GBID gbidHead;  // 0x00
    GBID gbidTorso;  // 0x04
    GBID gbidFeet;  // 0x08
    GBID gbidHands;  // 0x0c
    GBID gbidShoulders;  // 0x10
    GBID gbidLegs;  // 0x14
    GBID gbidBracers;  // 0x18
    GBID gbidBelt;  // 0x1c
};
static_assert(sizeof(ItemTierData) == 0x20, "ItemTierData");

struct LabelGBID {  // size 0x20
    GBHeader tHeader;  // 0x00
    int32 nIndex;  // 0x18
    uint32 dwPad;  // 0x1c
};
static_assert(sizeof(LabelGBID) == 0x20, "LabelGBID");

struct LootDistEntry {  // size 0x5c
    int32 nLevel;  // 0x00
    int32 nGoldWeight[3];  // 0x04
    int32 nGoldMin[3];  // 0x10
    int32 nGoldDelta[3];  // 0x1c
    float flQualityLevelChance[11];  // 0x28
    int32 nCurseChance;  // 0x54
    int32 nCurseBonus;  // 0x58
};
static_assert(sizeof(LootDistEntry) == 0x5c, "LootDistEntry");

struct LootRunQuest {  // size 0x10
    SNO snoQuest;  // 0x00
    float flWeight;  // 0x04
    GBID gbidType;  // 0x08
    GBID gbidLabel;  // 0x0c
};
static_assert(sizeof(LootRunQuest) == 0x10, "LootRunQuest");

struct LootRunQuestTierEntry {  // size 0x118
    GBHeader tHeader;  // 0x00
    LootRunQuest tLootRunQuests[16];  // 0x18
};
static_assert(sizeof(LootRunQuestTierEntry) == 0x118, "LootRunQuestTierEntry");

struct MonsterAffixesEntry {  // size 0x298
    GBHeader tHeader;  // 0x00
    int32 nFrequencyWeight;  // 0x18
    int32 nAffixLevelMin;  // 0x1c
    int32 nAffixLevelMax;  // 0x20
    MonsterAffixValidFor eValidFor;  // 0x24
    DamageType eBaseElement;  // 0x28
    AffixType eAffixType;  // 0x2c
    BOOL bAvailableForUniques;  // 0x30
    BOOL bShown;  // 0x34
    BOOL bDefensive;  // 0x38
    BOOL bCrowdControl;  // 0x3c
    AttributeSpecifier ptAttributes[10];  // 0x40
    AttributeSpecifier ptMinionAttributes[10];  // 0x130
    SNO snoOnDeathPower;  // 0x220
    SNO snoSpawnPower[3];  // 0x224
    char arAllowedByLevel[99];  // 0x230
};
static_assert(sizeof(MonsterAffixesEntry) == 0x298, "MonsterAffixesEntry");

struct MovementStyleDefinition {  // size 0x98
    GBHeader tHeader;  // 0x00
    uint32 fWalkSwitchStyle;  // 0x18
    uint32 fStopSwitchStyle;  // 0x1c
    uint32 fWalkBlockStyle;  // 0x20
    uint32 fStopBlockStyle;  // 0x24
    uint32 fFlankStyle;  // 0x28
    uint32 fZigZagStyle;  // 0x2c
    uint32 fSprintStyle;  // 0x30
    uint32 fSprintRunRandomStyle;  // 0x34
    float flRunTimer;  // 0x38
    float flRunDeltaTimer;  // 0x3c
    float flWalkTimer;  // 0x40
    float flWalkDeltaTimer;  // 0x44
    float flSwitchChance;  // 0x48
    float flBlockTimer;  // 0x4c
    float flBlockDeltaTimer;  // 0x50
    float flBlockCooldown;  // 0x54
    float flBlockCooldownDelta;  // 0x58
    float flBlockChance;  // 0x5c
    float flBlockDistance;  // 0x60
    float flFlankAngle;  // 0x64
    float flFlankDeltaAngle;  // 0x68
    float flFlankPeakDistance;  // 0x6c
    float flFlankMaxDistance;  // 0x70
    float flZigZagTimer;  // 0x74
    float flZigZagAngle;  // 0x78
    float flZigZagDeltaAngle;  // 0x7c
    float flZigZagPeakDistance;  // 0x80
    float flZigZagMaxDistance;  // 0x84
    float flSpeedScalar;  // 0x88
    float flAccel;  // 0x8c
    SNO snoPowerToBreakObjects;  // 0x90
    uint32 dwPad;  // 0x94
};
static_assert(sizeof(MovementStyleDefinition) == 0x98, "MovementStyleDefinition");

struct PowerFormulaTableEntry {  // size 0x530
    CHAR szName[1024];  // 0x00
    float flValue[76];  // 0x400
};
static_assert(sizeof(PowerFormulaTableEntry) == 0x530, "PowerFormulaTableEntry");

struct QualityClass {  // size 0x78
    GBHeader tHeader;  // 0x00
    int32 nLevelDiff;  // 0x18
    float flQualityLevelScalar[11];  // 0x1c
    float flQualityLevelAdd[11];  // 0x48
    uint32 dwPad;  // 0x74
};
static_assert(sizeof(QualityClass) == 0x78, "QualityClass");

struct RareItemNamesEntry {  // size 0x28
    GBHeader tHeader;  // 0x00
    GBHandle hgbRelatedAffixOrItemType;  // 0x18
    AffixType eAffixType;  // 0x20
    uint32 dwPad;  // 0x24
};
static_assert(sizeof(RareItemNamesEntry) == 0x28, "RareItemNamesEntry");

struct ScriptedAchievementEvent {  // size 0x18
    GBHeader tHeader;  // 0x00
};
static_assert(sizeof(ScriptedAchievementEvent) == 0x18, "ScriptedAchievementEvent");

struct TransmuteRecipeIngredient {  // size 0x14
    TransmuteRecipeIngredientType eType;  // 0x00
    int32 nTypeValue;  // 0x04
    int32 nQuantity;  // 0x08
    int32 unk_0c;  // 0x0c
    int32 unk_10;  // 0x10
};
static_assert(sizeof(TransmuteRecipeIngredient) == 0x14, "TransmuteRecipeIngredient");

struct TransmuteRecipe {  // size 0xc8
    GBHeader tHeader;  // 0x00
    TransmuteRecipeAction eAction;  // 0x18
    TransmuteRecipeIngredient tIngredients[8];  // 0x1c
    int32 nIngredientsCount;  // 0xbc
    int32 nPage;  // 0xc0
    BOOL bHidden;  // 0xc4
};
static_assert(sizeof(TransmuteRecipe) == 0xc8, "TransmuteRecipe");

const GameBalanceDefinition * ScriptedAchievementEventsTableLockR();
void ScriptedAchievementEventsTableUnlockR(const GameBalanceDefinition * lpDef);

const int32 MAX_QUEST_REWARD_XP_TIER = 7;  // GameBalanceDefinition.h:727
const int32 MAX_QUEST_REWARD_GOLD_TIER = 6;  // GameBalanceDefinition.h:728

static const uint32 POWER_FORMULA_TABLE_MAX_VALUES = 76;  // GameBalanceDefinition.h:801
const GameBalanceDefinition * PowerFormulaTableLockR();  // GameBalanceDefinition.cpp:2245
void PowerFormulaTableUnlockR(const GameBalanceDefinition * lpDef);  // GameBalanceDefinition.cpp:2254

// GameBalanceDefinition.h:2239
typedef ScopedGBLockR<TransmuteRecipe, 50> ScopedTransmuteRecipeEntryLockR;
