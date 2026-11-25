// Original file: X/Common/Definitions/GameBalanceDefinition.cpp
//

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Contrib/Contrib/blz/include/blz/float.h"
#include "../../../Contrib/Contrib/blz/include/blz/tuple.h"
#include "GameBalanceDefinition.h"
#include "HeroDefinition.h"
#include "../GameBalance.h"
#include "../Game.h"
#include "../GameCommonData.h"
#include "../ActorCommonData.h"
#include "../GlobalSNO.h"
#include "../BBE.h"
#include "../BBEUtil.h"
#include "../../Server/SGame.h"
#include "../../Sigma/Random.h"
#include "../../Sigma/Math/MathGeneral.h"
#include "../../Sigma/RefString.h"
#include "../SNO/SymbolicValue.h"
#include "../../Sigma/DataStructures/XGrowableMap.h"
#include "../../Sigma/DataStructures/XChainedFixedBlockAllocator.h"
#include "../SNO/SNOName.h"
#include "../WorldPlace.h"
#include "../../Client/Gfx/GfxVB.h"
#include "../D3TypeDescriptor/D3TypeDescriptor.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptorField.h"
#include "../Attributes/FastAttribute.h"

enum {
    GAME_BALANCE_XML_VERSION = 3,
    GAMEBALANCE_COMMON_SNO_VERSION = 2444,
    GAMEBALANCE_PC_SNO_VERSION = 2469,
    GAMEBALANCE_CONSOLE_SNO_VERSION = 2466,
    GAMEBALANCE_AXE_SNO_VERSION = 2454,
};

// GameBalanceDefinition.h:49, the values GameBalanceDefinition.h does not declare yet.
enum {
    GB_INVALID = -1,
    GB_ITEMTYPES = 1,
    GB_EXPERIENCETABLE = 3,
    GB_HIRELINGS = 4,
    GB_MONSTER_LEVELS = 5,
    GB_AFFIXES = 8,
    GB_MOVEMENT_STYLES = 10,
    GB_LABELS = 11,
    GB_LOOTDISTRIBUTION = 12,
    GB_RARE_ITEM_NAMES = 16,

    GB_MONSTER_AFFIXES = 18,
    GB_MONSTER_NAMES = 19,
    GB_HELPCODES = 24,
    GB_ITEM_DROP_TABLE = 25,
    GB_ITEM_LEVEL_MODIFIERS = 26,
    GB_ITEM_QUALITY_CLASSES = 27,
    GB_HANDICAPS = 28,
    GB_ITEM_SALVAGE_LEVELS = 29,

    GB_ITEM_TIERS = 35,
    GB_POWER_FORMULA_TABLES = 36,
    GB_SCRIPTED_ACHIEVEMENT_EVENTS = 37,
    GB_LOOTRUN_QUEST_TIERS = 39,
    GB_LEGACY_ITEM_CONVERSIONS = 45,
    GB_ENCHANT_ITEM_AFFIX_USE_COUNT_COST_SCALARS = 46,
    GB_AFFIXFAMILY = 47,
    GB_EXPERIENCETABLEALT = 48,
    GB_TIERED_LOOT_RUN_LEVELS = 49,

    NUM_GBTYPES = 52,
};

struct GBEntry {  // size 0x8
    SNO snoGB;  // 0x00
    int32 nIndex;  // 0x04
};
static_assert(sizeof(GBEntry) == 0x8, "GBEntry");

typedef XGrowableMap<int, int, GBEntry, const GBEntry &> GBIDMap;

typedef XGrowableMap<int, int, int, int> GBActorSNOToIDMap;

struct GameBalanceAppGlobals {  // size 0xb8
    GBIDMap * pmapGameBalanceDefs;  // 0x00
    GBActorSNOToIDMap mapActorSNOToItemGBID;  // 0x08
    GBHandlePool poolGBHandles;  // 0x88
};
static_assert(sizeof(GameBalanceAppGlobals) == 0xb8, "GameBalanceAppGlobals");

struct GameBalanceGlobals {  // size 0x30
    GBHandlePool poolGBHandles;  // 0x00
};
static_assert(sizeof(GameBalanceGlobals) == 0x30, "GameBalanceGlobals");

size_t sGetGBTableOffset(GameBalanceType eType);
int32 sGetGBSize(GameBalanceType eType);
int32 sGetGBCount(const GameBalanceDefinition * pdefGB);
void * sGetGBTablePointer(const GameBalanceDefinition * pdefGB);
static BOOL sGBTypeUsesGBID(GameBalanceType eType);
CRefString GBIDToString(const GBHandle & hGB, const BOOL fUsedForXMLFields);




struct LegacyItemConversion {  // size 0x28; GameBalanceDefinition.h (local copies in Items.cpp, Loot.cpp, SPlayer.cpp conflict)
    GBHeader tHeader; GBID gbidOldItem; GBID gbidNewItem; BOOL bConsoleIgnore; uint32 dwPad;
};
extern const CHAR cgszBadDataName[];  // Definitions.cpp:116
extern const XTypeDescriptor * DT_ATTRIBUTEPARAM; extern const XTypeDescriptor * DT_RECIPE_INGREDIENT;
void GlobalGBIDListRefresh();  // GlobalSNO.cpp:3210
BOOL ItemIsLegendary(GBID gbidItem); BOOL ItemIsCrafted(const ActorCommonData & tACDItem); BOOL ItemIsAncientRank(const ActorCommonData & tACDItem); int32 ItemGetAncientRank(const ActorCommonData & tACDItem); BOOL ItemAttributePrefersMinimum(GBID gbidItem, Attrib eAttribute);




static const TypeDescriptorField AttributeSpecifier_Fields[5] = {
TranslatableTypeDescriptorField("", offsetof(AttributeSpecifier, eAttribute), (const void *)-1, XMLStringToAttrib, AttribToXMLString),
TypeDescriptorField("", DT_ATTRIBUTEPARAM, offsetof(AttributeSpecifier, nParam), (const void *)-1, 1),
TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(AttributeSpecifier, serAttributeFormula), NULL, 0), FormulaTypeDescriptorField("", offsetof(AttributeSpecifier, pbAttributeFormula), offsetof(AttributeSpecifier, serAttributeFormula) - offsetof(AttributeSpecifier, pbAttributeFormula)),
TypeDescriptorField(NULL, DT_NULL, sizeof(AttributeSpecifier), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_AttributeSpecifier("AttributeSpecifier", 0, AttributeSpecifier_Fields, 5, NULL, 0x0, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_ATTRIBUTE_SPECIFIER = &TypeDescriptor_AttributeSpecifier;

static const TypeDescriptorField GBHandle_Fields[3] = {
BoundedTypeDescriptorField("", DT_INT, offsetof(GBHandle, eType), (const void *)-1, (const void *)-2, (const void *)51),
TypeDescriptorField("", DT_INT, offsetof(GBHandle, gbid), (const void *)-1, 1),
TypeDescriptorField(NULL, DT_NULL, sizeof(GBHandle), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_GBHandle("GBHandle", 0, GBHandle_Fields, 3, NULL, 0x0, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_GBHANDLE = &TypeDescriptor_GBHandle;














static const TypeDescriptorField ItemDefinition_Fields[172] = {
TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(ItemDefinition, tHeader), NULL, 7340032), VariableArrayTypeDescriptorField("", DT_BYTE, offsetof(ItemDefinition, tHeader.szName), 0x700020, offsetof(ItemDefinition, tHeader) - offsetof(ItemDefinition, tHeader.szName)), TypeDescriptorField("", DT_INT, offsetof(ItemDefinition, tHeader.gbid), NULL, 0), TypeDescriptorField("", DT_INT, offsetof(ItemDefinition, tHeader.dwPad), NULL, 0),
FilterTypeDescriptorField("", DT_SNO, offsetof(ItemDefinition, snoActor), "", 1, 0, -1),
FilterTypeDescriptorField("", DT_GBID, offsetof(ItemDefinition, gbidItemType), "", 1, 0, -1),

TypeDescriptorField("", DT_INT, offsetof(ItemDefinition, nItemLevel), (const void *)1, 1),
EnumTypeDescriptorField("", offsetof(ItemDefinition, eMinAct), NULL, gActs),
TypeDescriptorField("", DT_INT, offsetof(ItemDefinition, nAffixLevelBonus), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDefinition, nBonusRandomAffixes), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDefinition, nBonusMajorAffixes), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDefinition, nBonusMinorAffixes), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDefinition, nMaxSockets), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDefinition, nMaxStackSize), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDefinition, nBaseCostGold), (const void *)10, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDefinition, nBaseCostSpecialCurrency), (const void *)5, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDefinition, nBaseDurability), (const void *)250, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDefinition, nDurabilityVariance), (const void *)200, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDefinition, nEnchantAffixCost), (const void *)500, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDefinition, nEnchantAffixCostX1), (const void *)500, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDefinition, nIdentifyCost), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDefinition, nSellOverrideCost), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDefinition, nTransmogUnlockCrafterLevel), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDefinition, nTransmogCost), (const void *)100, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDefinition, nRemoveGemCost), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDefinition, nNormalRequiredLevel), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDefinition, nCrafterRequiredLevel), NULL, 1),

FilterTypeDescriptorField("", DT_GBID, offsetof(ItemDefinition, gbidBaseItem), "", 2, 0, -1),
FilterTypeDescriptorField("", DT_GBID, offsetof(ItemDefinition, gbidSet), "", 33, 0, -1),

FilterTypeDescriptorField("", DT_SNO, offsetof(ItemDefinition, snoRandomItemTreasureClass), "", 52, 0, -1),
FilterTypeDescriptorField("", DT_SNO, offsetof(ItemDefinition, snoSalvageTreasureClassNormal), "", 52, 0, -1),
FilterTypeDescriptorField("", DT_SNO, offsetof(ItemDefinition, snoSalvageTreasureClassMagic), "", 52, 0, -1),
FilterTypeDescriptorField("", DT_SNO, offsetof(ItemDefinition, snoSalvageTreasureClassRare), "", 52, 0, -1),
FilterTypeDescriptorField("", DT_SNO, offsetof(ItemDefinition, snoRareNamePrefixStringList), "", 42, 0, -1),
FilterTypeDescriptorField("", DT_SNO, offsetof(ItemDefinition, snoRareNameSuffixStringList), "", 42, 0, -1),

TypeDescriptorField("", DT_INT, offsetof(ItemDefinition, nLegendaryDropWeight), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDefinition, nClassLegendaryDropWeight), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDefinition, nClassLegendaryDropWeight[1]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDefinition, nClassLegendaryDropWeight[2]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDefinition, nClassLegendaryDropWeight[3]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDefinition, nClassLegendaryDropWeight[4]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDefinition, nClassLegendaryDropWeight[5]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDefinition, nClassLegendaryDropWeight[6]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDefinition, nLegendaryRating), NULL, 1),

FlagTypeDescriptorField("", offsetof(ItemDefinition, dwFlags), NULL, 0),
FlagTypeDescriptorField("", offsetof(ItemDefinition, dwFlags), NULL, 1),
FlagTypeDescriptorField("", offsetof(ItemDefinition, dwFlags), NULL, 2),
FlagTypeDescriptorField("", offsetof(ItemDefinition, dwFlags), NULL, 3),
FlagTypeDescriptorField("", offsetof(ItemDefinition, dwFlags), NULL, 4),
FlagTypeDescriptorField("", offsetof(ItemDefinition, dwFlags), (const void *)1, 5),
FlagTypeDescriptorField("", offsetof(ItemDefinition, dwFlags), NULL, 7),
FlagTypeDescriptorField("", offsetof(ItemDefinition, dwFlags), NULL, 6),
FlagTypeDescriptorField("", offsetof(ItemDefinition, dwFlags), NULL, 8),
FlagTypeDescriptorField("", offsetof(ItemDefinition, dwFlags), NULL, 9),
FlagTypeDescriptorField("", offsetof(ItemDefinition, dwFlags), NULL, 10),
FlagTypeDescriptorField("", offsetof(ItemDefinition, dwFlags), NULL, 11),
FlagTypeDescriptorField("", offsetof(ItemDefinition, dwFlags), NULL, 12),
FlagTypeDescriptorField("", offsetof(ItemDefinition, dwFlags), NULL, 13),
FlagTypeDescriptorField("", offsetof(ItemDefinition, dwFlags), NULL, 24),
FlagTypeDescriptorField("", offsetof(ItemDefinition, dwFlags), NULL, 25),
FlagTypeDescriptorField("", offsetof(ItemDefinition, dwFlags), NULL, 26),
FlagTypeDescriptorField("", offsetof(ItemDefinition, dwFlags), NULL, 28),
FlagTypeDescriptorField("", offsetof(ItemDefinition, dwFlags), NULL, 23),
FlagTypeDescriptorField("", offsetof(ItemDefinition, dwFlags), NULL, 14),
FlagTypeDescriptorField("", offsetof(ItemDefinition, dwFlags), NULL, 17),
FlagTypeDescriptorField("", offsetof(ItemDefinition, dwFlags), NULL, 18),
FlagTypeDescriptorField("", offsetof(ItemDefinition, dwFlags), NULL, 19),
FlagTypeDescriptorField("", offsetof(ItemDefinition, dwFlags), NULL, 20),
FlagTypeDescriptorField("", offsetof(ItemDefinition, dwFlags), NULL, 21),
FlagTypeDescriptorField("", offsetof(ItemDefinition, dwFlags), NULL, 22),
FlagTypeDescriptorField("", offsetof(ItemDefinition, dwFlags), NULL, 30),
FlagTypeDescriptorField("", offsetof(ItemDefinition, dwFlags), NULL, 31),
FlagTypeDescriptorField("", offsetof(ItemDefinition, unk_24), NULL, 0),
FlagTypeDescriptorField("", offsetof(ItemDefinition, unk_24), NULL, 1),

TypeDescriptorField("", DT_INT, offsetof(ItemDefinition, eSavedDyeType), NULL, 1),

TypeDescriptorField("", DT_FLOAT, offsetof(ItemDefinition, flAttributesDamage), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(ItemDefinition, flAttributesDamageRange), NULL, 1),

TypeDescriptorField("", DT_FLOAT, offsetof(ItemDefinition, flAttributesDamageVarianceMin), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(ItemDefinition, flAttributesDamageVarianceDelta), NULL, 1),

TypeDescriptorField("", DT_FLOAT, offsetof(ItemDefinition, flAttributesAttacksPerSecond), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(ItemDefinition, flAttributesArmor), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(ItemDefinition, flAttributesArmorDelta), NULL, 1),
FilterTypeDescriptorField("", DT_SNO, offsetof(ItemDefinition, tSkills), "", 29, 0, -1),
TypeDescriptorField("", DT_INT, offsetof(ItemDefinition, tSkills[0].nLevelMod), NULL, 1),
FilterTypeDescriptorField("", DT_SNO, offsetof(ItemDefinition, tSkills[1]), "", 29, 0, -1),
TypeDescriptorField("", DT_INT, offsetof(ItemDefinition, tSkills[1].nLevelMod), NULL, 1),
FilterTypeDescriptorField("", DT_SNO, offsetof(ItemDefinition, tSkills[2]), "", 29, 0, -1),
TypeDescriptorField("", DT_INT, offsetof(ItemDefinition, tSkills[2].nLevelMod), NULL, 1),
FilterTypeDescriptorField("", DT_SNO, offsetof(ItemDefinition, tSkills[3]), "", 29, 0, -1),
TypeDescriptorField("", DT_INT, offsetof(ItemDefinition, tSkills[3].nLevelMod), NULL, 1),

FixedArrayTypeDescriptorField("", DT_INT, offsetof(ItemDefinition, dwItemLabelMask), NULL, 1, 5, 0),




















































TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(ItemDefinition, serAttributes_Console), NULL, 7340032), VariableArrayTypeDescriptorField("", DT_ATTRIBUTE_SPECIFIER, offsetof(ItemDefinition, ptAttributes), 0x700020, offsetof(ItemDefinition, serAttributes_Console) - offsetof(ItemDefinition, ptAttributes)),

EnumTypeDescriptorField("", offsetof(ItemDefinition, eDisplayedQualityLevel), (const void *)-1, gItemQualityLevelNames),
FilteredFixedArrayTypeDescriptorField("", DT_GBID, offsetof(ItemDefinition, gbidCraftingRecipesToGrant), "", 0, 10, 32, 0),
FilteredFixedArrayTypeDescriptorField("", DT_GBID, offsetof(ItemDefinition, gbidTransmogsToGrant), "", 0, 8, 2, 0),

FilterTypeDescriptorField("", DT_GBID, offsetof(ItemDefinition, gbidLegendaryAffixFamily), "", 47, 0, -1),
FilterTypeDescriptorField("", DT_GBID, offsetof(ItemDefinition, gbidLegendaryAffixFamily[1]), "", 47, 0, -1),
FilterTypeDescriptorField("", DT_GBID, offsetof(ItemDefinition, gbidLegendaryAffixFamily[2]), "", 47, 0, -1),
FilterTypeDescriptorField("", DT_GBID, offsetof(ItemDefinition, gbidLegendaryAffixFamily[3]), "", 47, 0, -1),
FilterTypeDescriptorField("", DT_GBID, offsetof(ItemDefinition, gbidLegendaryAffixFamily[4]), "", 47, 0, -1),
FilterTypeDescriptorField("", DT_GBID, offsetof(ItemDefinition, gbidLegendaryAffixFamily[5]), "", 47, 0, -1),

TypeDescriptorField("", DT_INT, offsetof(ItemDefinition, nLegendaryAffixFamilyLevel), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDefinition, nLegendaryAffixFamilyLevel[1]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDefinition, nLegendaryAffixFamilyLevel[2]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDefinition, nLegendaryAffixFamilyLevel[3]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDefinition, nLegendaryAffixFamilyLevel[4]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDefinition, nLegendaryAffixFamilyLevel[5]), NULL, 1),

TypeDescriptorField("", DT_INT, offsetof(ItemDefinition, bUseLegendaryAllowedTypes), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDefinition, bUseLegendaryAllowedTypes[1]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDefinition, bUseLegendaryAllowedTypes[2]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDefinition, bUseLegendaryAllowedTypes[3]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDefinition, bUseLegendaryAllowedTypes[4]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDefinition, bUseLegendaryAllowedTypes[5]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDefinition, unk_1d0), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDefinition, unk_1d0[1]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDefinition, unk_1d0[2]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDefinition, unk_1d0[3]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDefinition, unk_1d0[4]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDefinition, unk_1d0[5]), NULL, 1),

FilterTypeDescriptorField("", DT_GBID, offsetof(ItemDefinition, gbidLegendaryFamily), "", 4294967295, 0, -1),
FilterTypeDescriptorField("", DT_GBID, offsetof(ItemDefinition, unk_1ec), "", 47, 0, -1),
TypeDescriptorField("", DT_INT, offsetof(ItemDefinition, unk_1f0), NULL, 1),

EnumTypeDescriptorField("", offsetof(ItemDefinition, eGemType), NULL, gGemTypeNames),
TypeDescriptorField("", DT_INT, offsetof(ItemDefinition, nCraftingTier), NULL, 1),
EnumTypeDescriptorField("", offsetof(ItemDefinition, eCraftingQualityLevel), NULL, gCraftingQualityLevelNames),
FilterTypeDescriptorField("", DT_SNO, offsetof(ItemDefinition, snoActorPageOfFatePortal), "", 1, 0, -1),
FilterTypeDescriptorField("", DT_SNO, offsetof(ItemDefinition, snoWorldPageOfFate), "", 48, 0, -1),
FilterTypeDescriptorField("", DT_SNO, offsetof(ItemDefinition, snoWorldPageOfFate[1]), "", 48, 0, -1),
FilterTypeDescriptorField("", DT_SNO, offsetof(ItemDefinition, snoLevelAreaPageOfFatePortal), "", 22, 0, -1),

TypeDescriptorField("", DT_INT, offsetof(ItemDefinition, nEnchantAffixIngredientsCount), NULL, 1),
FixedArrayTypeDescriptorField("", DT_RECIPE_INGREDIENT, offsetof(ItemDefinition, arEnchantAffixIngredients), NULL, 1, 6, 0),

TypeDescriptorField("", DT_INT, offsetof(ItemDefinition, nEnchantAffixIngredientsCountX1), NULL, 1),
FixedArrayTypeDescriptorField("", DT_RECIPE_INGREDIENT, offsetof(ItemDefinition, arEnchantAffixIngredientsX1), NULL, 1, 6, 0),

TypeDescriptorField("", DT_INT, offsetof(ItemDefinition, gbidLegendaryPowerItemReplacement), (const void *)-1, 1),

TypeDescriptorField("", DT_INT, offsetof(ItemDefinition, nSeasonRequiredToDrop), (const void *)-1, 1),

TranslatableTypeDescriptorField("", offsetof(ItemDefinition, tJewelUpgradeAttribute), (const void *)-1, XMLStringToAttrib, AttribToXMLString),
TypeDescriptorField("", DT_ATTRIBUTEPARAM, offsetof(ItemDefinition, tJewelUpgradeAttribute.nParam), (const void *)-1, 1),
TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(ItemDefinition, tJewelUpgradeAttribute.serAttributeFormula), NULL, 0), FormulaTypeDescriptorField("", offsetof(ItemDefinition, tJewelUpgradeAttribute.pbAttributeFormula), offsetof(ItemDefinition, tJewelUpgradeAttribute.serAttributeFormula) - offsetof(ItemDefinition, tJewelUpgradeAttribute.pbAttributeFormula)),

TranslatableTypeDescriptorField("", offsetof(ItemDefinition, tJewelSecondaryEffectAttribute), (const void *)-1, XMLStringToAttrib, AttribToXMLString),
TypeDescriptorField("", DT_ATTRIBUTEPARAM, offsetof(ItemDefinition, tJewelSecondaryEffectAttribute.nParam), (const void *)-1, 1),
TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(ItemDefinition, tJewelSecondaryEffectAttribute.serAttributeFormula), NULL, 0), FormulaTypeDescriptorField("", offsetof(ItemDefinition, tJewelSecondaryEffectAttribute.pbAttributeFormula), offsetof(ItemDefinition, tJewelSecondaryEffectAttribute.serAttributeFormula) - offsetof(ItemDefinition, tJewelSecondaryEffectAttribute.pbAttributeFormula)),
TypeDescriptorField("", DT_INT, offsetof(ItemDefinition, nJewelSecondaryEffectUnlockRank), (const void *)-1, 1),
TranslatableTypeDescriptorField("", offsetof(ItemDefinition, unk_2b8), (const void *)-1, XMLStringToAttrib, AttribToXMLString),
TypeDescriptorField("", DT_ATTRIBUTEPARAM, offsetof(ItemDefinition, unk_2b8.nParam), (const void *)-1, 1),
TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(ItemDefinition, unk_2b8.serAttributeFormula), NULL, 0), FormulaTypeDescriptorField("", offsetof(ItemDefinition, unk_2b8.pbAttributeFormula), offsetof(ItemDefinition, unk_2b8.serAttributeFormula) - offsetof(ItemDefinition, unk_2b8.pbAttributeFormula)),
TypeDescriptorField("", DT_INT, offsetof(ItemDefinition, unk_2d0), (const void *)-1, 1),
TranslatableTypeDescriptorField("", offsetof(ItemDefinition, unk_2d8), (const void *)-1, XMLStringToAttrib, AttribToXMLString),
TypeDescriptorField("", DT_ATTRIBUTEPARAM, offsetof(ItemDefinition, unk_2d8.nParam), (const void *)-1, 1),
TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(ItemDefinition, unk_2d8.serAttributeFormula), NULL, 0), FormulaTypeDescriptorField("", offsetof(ItemDefinition, unk_2d8.pbAttributeFormula), offsetof(ItemDefinition, unk_2d8.serAttributeFormula) - offsetof(ItemDefinition, unk_2d8.pbAttributeFormula)),
TypeDescriptorField("", DT_INT, offsetof(ItemDefinition, unk_2f0), (const void *)-1, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDefinition, unk_2f8), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDefinition, nJewelMaxRank), (const void *)-1, 1),

FilterTypeDescriptorField("", DT_SNO, offsetof(ItemDefinition, snoStartEffect), "", 14, 0, -1),
FilterTypeDescriptorField("", DT_SNO, offsetof(ItemDefinition, snoEndEffect), "", 14, 0, -1),
FilterTypeDescriptorField("", DT_SNO, offsetof(ItemDefinition, snoMainEffect), "", 14, 0, -1),

TypeDescriptorField("", DT_INT, offsetof(ItemDefinition, hPortraitBkgrnd), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDefinition, hPortraitHPBar), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDefinition, hPortraitBanner), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDefinition, hPortraitFrame), NULL, 1),

TypeDescriptorField("", DT_INT, offsetof(ItemDefinition, nDateReleased), NULL, 1),

TypeDescriptorField("", DT_INT, offsetof(ItemDefinition, eVacuumPickup), NULL, 1),

TypeDescriptorField("", DT_INT, offsetof(ItemDefinition, nBaseCostSpecialCurrency2), (const void *)5, 1),

TypeDescriptorField("", DT_INT, offsetof(ItemDefinition, nDynamicCraftCostMagic), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDefinition, nDynamicCraftCostRare), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDefinition, nDynamicCraftAffixCount), NULL, 1),

FilterTypeDescriptorField("", DT_SNO, offsetof(ItemDefinition, snoSeasonCache), "", 52, 0, -1),
TypeDescriptorField(NULL, DT_NULL, sizeof(ItemDefinition), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_ItemDefinition("Item", 0, ItemDefinition_Fields, 172, NULL, 0x0, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_ITEM = &TypeDescriptor_ItemDefinition;

static const TypeDescriptorField ItemTable_Fields[3] = {
TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(ItemTable, serItems), NULL, 0), VariableArrayTypeDescriptorField("", DT_ITEM, offsetof(ItemTable, ptItems), 0x21, offsetof(ItemTable, serItems) - offsetof(ItemTable, ptItems)),
TypeDescriptorField(NULL, DT_NULL, sizeof(ItemTable), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_ItemTable("Items", 0, ItemTable_Fields, 3, NULL, 0x0, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_ITEMTABLE = &TypeDescriptor_ItemTable;

static const TypeDescriptorField ItemTypeDefinition_Fields[34] = {
TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(ItemTypeDefinition, tHeader), NULL, 7340032), VariableArrayTypeDescriptorField("", DT_BYTE, offsetof(ItemTypeDefinition, tHeader.szName), 0x700020, offsetof(ItemTypeDefinition, tHeader) - offsetof(ItemTypeDefinition, tHeader.szName)), TypeDescriptorField("", DT_INT, offsetof(ItemTypeDefinition, tHeader.gbid), NULL, 0), TypeDescriptorField("", DT_INT, offsetof(ItemTypeDefinition, tHeader.dwPad), NULL, 0),
FilterTypeDescriptorField("", DT_GBID, offsetof(ItemTypeDefinition, gbidParent), "", 1, 0, -1),
TypeDescriptorField("", DT_INT, offsetof(ItemTypeDefinition, nLootLevelRange), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemTypeDefinition, nReqCrafterLevelForEnchant), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemTypeDefinition, nMaxSockets), NULL, 1),

EnumTypeDescriptorField("", offsetof(ItemTypeDefinition, eBodySlot1), (const void *)-1, gBodySlots),
EnumTypeDescriptorField("", offsetof(ItemTypeDefinition, eBodySlot2), (const void *)-1, gBodySlots),
EnumTypeDescriptorField("", offsetof(ItemTypeDefinition, eBodySlot3), (const void *)-1, gBodySlots),
EnumTypeDescriptorField("", offsetof(ItemTypeDefinition, eBodySlot4), (const void *)-1, gBodySlots),
FlagTypeDescriptorField("", offsetof(ItemTypeDefinition, dwFlags), NULL, 0),
FlagTypeDescriptorField("", offsetof(ItemTypeDefinition, dwFlags), NULL, 12),
FlagTypeDescriptorField("", offsetof(ItemTypeDefinition, dwFlags), NULL, 1),
FlagTypeDescriptorField("", offsetof(ItemTypeDefinition, dwFlags), NULL, 3),
FlagTypeDescriptorField("", offsetof(ItemTypeDefinition, dwFlags), NULL, 6),
FlagTypeDescriptorField("", offsetof(ItemTypeDefinition, dwFlags), NULL, 7),

FlagTypeDescriptorField("", offsetof(ItemTypeDefinition, dwFlags), NULL, 8),
FlagTypeDescriptorField("", offsetof(ItemTypeDefinition, dwFlags), NULL, 9),
FlagTypeDescriptorField("", offsetof(ItemTypeDefinition, dwFlags), NULL, 10),
FlagTypeDescriptorField("", offsetof(ItemTypeDefinition, dwFlags), NULL, 13),
FlagTypeDescriptorField("", offsetof(ItemTypeDefinition, dwFlags), NULL, 11),
FlagTypeDescriptorField("", offsetof(ItemTypeDefinition, dwFlags), NULL, 17),
FlagTypeDescriptorField("", offsetof(ItemTypeDefinition, dwFlags), NULL, 20),

FlagTypeDescriptorField("", offsetof(ItemTypeDefinition, dwFlags), NULL, 14),
FlagTypeDescriptorField("", offsetof(ItemTypeDefinition, dwFlags), NULL, 15),
FlagTypeDescriptorField("", offsetof(ItemTypeDefinition, dwFlags), NULL, 16),

FilterTypeDescriptorField("", DT_GBID, offsetof(ItemTypeDefinition, gbidInherentAffix), "", 8, 0, -1),
FilterTypeDescriptorField("", DT_GBID, offsetof(ItemTypeDefinition, gbidInherentAffix[1]), "", 8, 0, -1),
FilterTypeDescriptorField("", DT_GBID, offsetof(ItemTypeDefinition, gbidInherentAffix[2]), "", 8, 0, -1),
FilterTypeDescriptorField("", DT_GBID, offsetof(ItemTypeDefinition, gbidInherentAffixFamily), "", 47, 0, -1),

FixedArrayTypeDescriptorField("", DT_INT, offsetof(ItemTypeDefinition, dwItemLabelMask), NULL, 1, 5, 0),

TypeDescriptorField(NULL, DT_NULL, sizeof(ItemTypeDefinition), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_ItemTypeDefinition("ItemType", 0, ItemTypeDefinition_Fields, 34, NULL, 0x0, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_ITEMTYPE = &TypeDescriptor_ItemTypeDefinition;

static const TypeDescriptorField ItemTypeTable_Fields[3] = {
TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(ItemTypeTable, serItemTypes), NULL, 0), VariableArrayTypeDescriptorField("", DT_ITEMTYPE, offsetof(ItemTypeTable, ptItemTypes), 0x21, offsetof(ItemTypeTable, serItemTypes) - offsetof(ItemTypeTable, ptItemTypes)),
TypeDescriptorField(NULL, DT_NULL, sizeof(ItemTypeTable), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_ItemTypeTable("Items", 0, ItemTypeTable_Fields, 3, NULL, 0x0, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_ITEMTYPETABLE = &TypeDescriptor_ItemTypeTable;

static const TypeDescriptorField RecipeIngredient_Fields[3] = {
FilterTypeDescriptorField("", DT_GBID, offsetof(RecipeIngredient, gbidItem), "", 2, 0, -1),
TypeDescriptorField("", DT_INT, offsetof(RecipeIngredient, nQuantity), NULL, 1),
TypeDescriptorField(NULL, DT_NULL, sizeof(RecipeIngredient), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_RecipeIngredient("RecipeIngredient", 0, RecipeIngredient_Fields, 3, NULL, 0x0, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_RECIPE_INGREDIENT = &TypeDescriptor_RecipeIngredient;

static const TypeDescriptorField ExperienceLevel_Fields[117] = {
TypeDescriptorField("", DT_INT64, offsetof(ExperienceLevel, nDeltaXP), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, nDeltaXPClassic), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, nPlayerXPValue), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(ExperienceLevel, flDurabilityLossPct), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(ExperienceLevel, flLifePerVitality), NULL, 1),

TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, nQuestTierXP), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, nQuestTierXP[1]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, nQuestTierXP[2]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, nQuestTierXP[3]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, nQuestTierXP[4]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, nQuestTierXP[5]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, nQuestTierXP[6]), NULL, 1),

TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, nQuestTierGold), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, nQuestTierGold[1]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, nQuestTierGold[2]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, nQuestTierGold[3]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, nQuestTierGold[4]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, nQuestTierGold[5]), NULL, 1),

TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, nLoreTierXP), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, nLoreTierXP[1]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, nLoreTierXP[2]), NULL, 1),

TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, nChallengeBonusXP), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, nChallengeBonusXP[1]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, nChallengeBonusXP[2]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, nChallengeBonusXP[3]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, nChallengeBonusXP[4]), NULL, 1),

TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, nChallengeBonusGold), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, nChallengeBonusGold[1]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, nChallengeBonusGold[2]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, nChallengeBonusGold[3]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, nChallengeBonusGold[4]), NULL, 1),

TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, nBountyXP), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, nBountyGold), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, nBountyXP[1]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, nBountyGold[1]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, nBountyXP[2]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, nBountyGold[2]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, nBountyXP[3]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, nBountyGold[3]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, nBountyXP[4]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, nBountyGold[4]), NULL, 1),

TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, nKillCounterXP), NULL, 1),

TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, nSkillPointAward), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(ExperienceLevel, flCritMultiplier), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(ExperienceLevel, flDodgeMultiplier), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(ExperienceLevel, flLifeStealMultiplier), (const void *)0x3f800000 /* 1f */, 1),

TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, tAttributeAward), (const void *)1, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, tAttributeAward[0].nDexterity), (const void *)1, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, tAttributeAward[0].nIntelligence), (const void *)1, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, tAttributeAward[0].nVitality), (const void *)1, 1),

TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, tAttributeAward[1]), (const void *)1, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, tAttributeAward[1].nDexterity), (const void *)1, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, tAttributeAward[1].nIntelligence), (const void *)1, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, tAttributeAward[1].nVitality), (const void *)1, 1),

TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, tAttributeAward[2]), (const void *)1, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, tAttributeAward[2].nDexterity), (const void *)1, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, tAttributeAward[2].nIntelligence), (const void *)1, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, tAttributeAward[2].nVitality), (const void *)1, 1),

TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, tAttributeAward[3]), (const void *)1, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, tAttributeAward[3].nDexterity), (const void *)1, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, tAttributeAward[3].nIntelligence), (const void *)1, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, tAttributeAward[3].nVitality), (const void *)1, 1),

TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, tAttributeAward[4]), (const void *)1, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, tAttributeAward[4].nDexterity), (const void *)1, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, tAttributeAward[4].nIntelligence), (const void *)1, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, tAttributeAward[4].nVitality), (const void *)1, 1),

TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, tAttributeAward[5]), (const void *)1, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, tAttributeAward[5].nDexterity), (const void *)1, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, tAttributeAward[5].nIntelligence), (const void *)1, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, tAttributeAward[5].nVitality), (const void *)1, 1),

TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, tAttributeAward[6]), (const void *)1, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, tAttributeAward[6].nDexterity), (const void *)1, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, tAttributeAward[6].nIntelligence), (const void *)1, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, tAttributeAward[6].nVitality), (const void *)1, 1),

TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, tHirelingAttributeAward[1]), (const void *)1, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, tHirelingAttributeAward[1].nDexterity), (const void *)1, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, tHirelingAttributeAward[1].nIntelligence), (const void *)1, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, tHirelingAttributeAward[1].nVitality), (const void *)1, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(ExperienceLevel, arHirelingDamageAbsorbPercent[1]), NULL, 1),

TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, tHirelingAttributeAward[2]), (const void *)1, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, tHirelingAttributeAward[2].nDexterity), (const void *)1, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, tHirelingAttributeAward[2].nIntelligence), (const void *)1, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, tHirelingAttributeAward[2].nVitality), (const void *)1, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(ExperienceLevel, arHirelingDamageAbsorbPercent[2]), NULL, 1),

TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, tHirelingAttributeAward[3]), (const void *)1, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, tHirelingAttributeAward[3].nDexterity), (const void *)1, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, tHirelingAttributeAward[3].nIntelligence), (const void *)1, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, tHirelingAttributeAward[3].nVitality), (const void *)1, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(ExperienceLevel, arHirelingDamageAbsorbPercent[3]), NULL, 1),

TypeDescriptorField("", DT_INT64, offsetof(ExperienceLevel, nPVPXPToLevel), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, tPVPAward), (const void *)1, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, tPVPAward[0].nGoldByRank), (const void *)1, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, tPVPAward[0].nPVPXP), (const void *)1, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, tPVPAward[0].nXP), (const void *)1, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, tPVPAward[0].nTokens), (const void *)1, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, tPVPAward[0].nAltXP), (const void *)1, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, tPVPAward[1]), (const void *)1, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, tPVPAward[1].nGoldByRank), (const void *)1, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, tPVPAward[1].nPVPXP), (const void *)1, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, tPVPAward[1].nXP), (const void *)1, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, tPVPAward[1].nTokens), (const void *)1, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, tPVPAward[1].nAltXP), (const void *)1, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, tPVPAward[2]), (const void *)1, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, tPVPAward[2].nGoldByRank), (const void *)1, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, tPVPAward[2].nPVPXP), (const void *)1, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, tPVPAward[2].nXP), (const void *)1, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, tPVPAward[2].nTokens), (const void *)1, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, tPVPAward[2].nAltXP), (const void *)1, 1),

TypeDescriptorField("", DT_FLOAT, offsetof(ExperienceLevel, flGoldCostLevelScalar), (const void *)0x3f800000 /* 1f */, 1),

TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, nSidekickPrimaryStatIdeal), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, nSidekickVitalityIdeal), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, nSidekickTotalArmorIdeal), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, nSidekickTotalResistIdeal), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, nSidekickLifeOnHitIdeal), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevel, nSidekickTargetDPSIdeal), NULL, 1),

TypeDescriptorField("", DT_FLOAT, offsetof(ExperienceLevel, flGearXPScalar), (const void *)0x3f800000 /* 1f */, 1),

TypeDescriptorField(NULL, DT_NULL, sizeof(ExperienceLevel), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_ExperienceLevel("ExperienceLevel", 0, ExperienceLevel_Fields, 117, NULL, 0x0, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_EXPERIENCELEVEL = &TypeDescriptor_ExperienceLevel;

static const TypeDescriptorField ExperienceTable_Fields[3] = {
TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(ExperienceTable, serExperienceLevels), NULL, 0), VariableArrayTypeDescriptorField("", DT_EXPERIENCELEVEL, offsetof(ExperienceTable, ptExperienceLevels), 0x21, offsetof(ExperienceTable, serExperienceLevels) - offsetof(ExperienceTable, ptExperienceLevels)),
TypeDescriptorField(NULL, DT_NULL, sizeof(ExperienceTable), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_ExperienceTable("ExperienceTable", 0, ExperienceTable_Fields, 3, NULL, 0x0, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_EXPERIENCETABLE = &TypeDescriptor_ExperienceTable;

static const TypeDescriptorField ExperienceLevelAlt_Fields[32] = {
TypeDescriptorField("", DT_INT64, offsetof(ExperienceLevelAlt, nDeltaXP), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevelAlt, nDeltaXPClassic), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevelAlt, dwPad), NULL, 0),

TypeDescriptorField("", DT_INT, offsetof(ExperienceLevelAlt, tAttributeAward), (const void *)1, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevelAlt, tAttributeAward[0].nDexterity), (const void *)1, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevelAlt, tAttributeAward[0].nIntelligence), (const void *)1, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevelAlt, tAttributeAward[0].nVitality), (const void *)1, 1),

TypeDescriptorField("", DT_INT, offsetof(ExperienceLevelAlt, tAttributeAward[1]), (const void *)1, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevelAlt, tAttributeAward[1].nDexterity), (const void *)1, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevelAlt, tAttributeAward[1].nIntelligence), (const void *)1, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevelAlt, tAttributeAward[1].nVitality), (const void *)1, 1),

TypeDescriptorField("", DT_INT, offsetof(ExperienceLevelAlt, tAttributeAward[2]), (const void *)1, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevelAlt, tAttributeAward[2].nDexterity), (const void *)1, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevelAlt, tAttributeAward[2].nIntelligence), (const void *)1, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevelAlt, tAttributeAward[2].nVitality), (const void *)1, 1),

TypeDescriptorField("", DT_INT, offsetof(ExperienceLevelAlt, tAttributeAward[3]), (const void *)1, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevelAlt, tAttributeAward[3].nDexterity), (const void *)1, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevelAlt, tAttributeAward[3].nIntelligence), (const void *)1, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevelAlt, tAttributeAward[3].nVitality), (const void *)1, 1),

TypeDescriptorField("", DT_INT, offsetof(ExperienceLevelAlt, tAttributeAward[4]), (const void *)1, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevelAlt, tAttributeAward[4].nDexterity), (const void *)1, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevelAlt, tAttributeAward[4].nIntelligence), (const void *)1, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevelAlt, tAttributeAward[4].nVitality), (const void *)1, 1),

TypeDescriptorField("", DT_INT, offsetof(ExperienceLevelAlt, tAttributeAward[5]), (const void *)1, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevelAlt, tAttributeAward[5].nDexterity), (const void *)1, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevelAlt, tAttributeAward[5].nIntelligence), (const void *)1, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevelAlt, tAttributeAward[5].nVitality), (const void *)1, 1),

TypeDescriptorField("", DT_INT, offsetof(ExperienceLevelAlt, tAttributeAward[6]), (const void *)1, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevelAlt, tAttributeAward[6].nDexterity), (const void *)1, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevelAlt, tAttributeAward[6].nIntelligence), (const void *)1, 1),
TypeDescriptorField("", DT_INT, offsetof(ExperienceLevelAlt, tAttributeAward[6].nVitality), (const void *)1, 1),
TypeDescriptorField(NULL, DT_NULL, sizeof(ExperienceLevelAlt), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_ExperienceLevelAlt("ExperienceLevelAlt", 0, ExperienceLevelAlt_Fields, 32, NULL, 0x0, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_EXPERIENCELEVELALT = &TypeDescriptor_ExperienceLevelAlt;

static const TypeDescriptorField ExperienceTableAlt_Fields[3] = {
TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(ExperienceTableAlt, serExperienceLevelsAlt), NULL, 0), VariableArrayTypeDescriptorField("", DT_EXPERIENCELEVELALT, offsetof(ExperienceTableAlt, ptExperienceLevelsAlt), 0x21, offsetof(ExperienceTableAlt, serExperienceLevelsAlt) - offsetof(ExperienceTableAlt, ptExperienceLevelsAlt)),
TypeDescriptorField(NULL, DT_NULL, sizeof(ExperienceTableAlt), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_ExperienceTableAlt("ExperienceTableAlt", 0, ExperienceTableAlt_Fields, 3, NULL, 0x0, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_EXPERIENCETABLEALT = &TypeDescriptor_ExperienceTableAlt;

static const TypeDescriptorField PowerFormulaTableEntry_Fields[78] = {
CharArrayTypeDescriptorField("", offsetof(PowerFormulaTableEntry, szName), "NONE", 1, 1024, -1),
TypeDescriptorField("", DT_FLOAT, offsetof(PowerFormulaTableEntry, flValue), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(PowerFormulaTableEntry, flValue[1]), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(PowerFormulaTableEntry, flValue[2]), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(PowerFormulaTableEntry, flValue[3]), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(PowerFormulaTableEntry, flValue[4]), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(PowerFormulaTableEntry, flValue[5]), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(PowerFormulaTableEntry, flValue[6]), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(PowerFormulaTableEntry, flValue[7]), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(PowerFormulaTableEntry, flValue[8]), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(PowerFormulaTableEntry, flValue[9]), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(PowerFormulaTableEntry, flValue[10]), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(PowerFormulaTableEntry, flValue[11]), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(PowerFormulaTableEntry, flValue[12]), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(PowerFormulaTableEntry, flValue[13]), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(PowerFormulaTableEntry, flValue[14]), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(PowerFormulaTableEntry, flValue[15]), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(PowerFormulaTableEntry, flValue[16]), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(PowerFormulaTableEntry, flValue[17]), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(PowerFormulaTableEntry, flValue[18]), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(PowerFormulaTableEntry, flValue[19]), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(PowerFormulaTableEntry, flValue[20]), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(PowerFormulaTableEntry, flValue[21]), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(PowerFormulaTableEntry, flValue[22]), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(PowerFormulaTableEntry, flValue[23]), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(PowerFormulaTableEntry, flValue[24]), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(PowerFormulaTableEntry, flValue[25]), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(PowerFormulaTableEntry, flValue[26]), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(PowerFormulaTableEntry, flValue[27]), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(PowerFormulaTableEntry, flValue[28]), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(PowerFormulaTableEntry, flValue[29]), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(PowerFormulaTableEntry, flValue[30]), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(PowerFormulaTableEntry, flValue[31]), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(PowerFormulaTableEntry, flValue[32]), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(PowerFormulaTableEntry, flValue[33]), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(PowerFormulaTableEntry, flValue[34]), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(PowerFormulaTableEntry, flValue[35]), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(PowerFormulaTableEntry, flValue[36]), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(PowerFormulaTableEntry, flValue[37]), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(PowerFormulaTableEntry, flValue[38]), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(PowerFormulaTableEntry, flValue[39]), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(PowerFormulaTableEntry, flValue[40]), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(PowerFormulaTableEntry, flValue[41]), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(PowerFormulaTableEntry, flValue[42]), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(PowerFormulaTableEntry, flValue[43]), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(PowerFormulaTableEntry, flValue[44]), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(PowerFormulaTableEntry, flValue[45]), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(PowerFormulaTableEntry, flValue[46]), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(PowerFormulaTableEntry, flValue[47]), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(PowerFormulaTableEntry, flValue[48]), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(PowerFormulaTableEntry, flValue[49]), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(PowerFormulaTableEntry, flValue[50]), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(PowerFormulaTableEntry, flValue[51]), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(PowerFormulaTableEntry, flValue[52]), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(PowerFormulaTableEntry, flValue[53]), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(PowerFormulaTableEntry, flValue[54]), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(PowerFormulaTableEntry, flValue[55]), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(PowerFormulaTableEntry, flValue[56]), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(PowerFormulaTableEntry, flValue[57]), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(PowerFormulaTableEntry, flValue[58]), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(PowerFormulaTableEntry, flValue[59]), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(PowerFormulaTableEntry, flValue[60]), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(PowerFormulaTableEntry, flValue[61]), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(PowerFormulaTableEntry, flValue[62]), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(PowerFormulaTableEntry, flValue[63]), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(PowerFormulaTableEntry, flValue[64]), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(PowerFormulaTableEntry, flValue[65]), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(PowerFormulaTableEntry, flValue[66]), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(PowerFormulaTableEntry, flValue[67]), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(PowerFormulaTableEntry, flValue[68]), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(PowerFormulaTableEntry, flValue[69]), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(PowerFormulaTableEntry, flValue[70]), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(PowerFormulaTableEntry, flValue[71]), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(PowerFormulaTableEntry, flValue[72]), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(PowerFormulaTableEntry, flValue[73]), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(PowerFormulaTableEntry, flValue[74]), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(PowerFormulaTableEntry, flValue[75]), NULL, 1),

TypeDescriptorField(NULL, DT_NULL, sizeof(PowerFormulaTableEntry), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_PowerFormulaTableEntry("PowerFormulaTableEntry", 0, PowerFormulaTableEntry_Fields, 78, NULL, 0x0, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_POWERFORMULATABLEENTRY = &TypeDescriptor_PowerFormulaTableEntry;

static const TypeDescriptorField PowerFormulaTable_Fields[3] = {
TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(PowerFormulaTable, serPowerFormulaTableEntries), NULL, 0), VariableArrayTypeDescriptorField("", DT_POWERFORMULATABLEENTRY, offsetof(PowerFormulaTable, ptPowerFormulaTableEntries), 0x21, offsetof(PowerFormulaTable, serPowerFormulaTableEntries) - offsetof(PowerFormulaTable, ptPowerFormulaTableEntries)),
TypeDescriptorField(NULL, DT_NULL, sizeof(PowerFormulaTable), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_PowerFormulaTable("PowerFormulaTable", 0, PowerFormulaTable_Fields, 3, NULL, 0x0, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_POWERFORMULATABLE = &TypeDescriptor_PowerFormulaTable;

static const TypeDescriptorField HelpCode_Fields[4] = {
CharArrayTypeDescriptorField("", offsetof(HelpCode, szControl), "HOO-HA!", 1, 256, -1),
CharArrayTypeDescriptorField("", offsetof(HelpCode, szDisplay), "HOO-HA!", 1, 256, -1),
CharArrayTypeDescriptorField("", offsetof(HelpCode, szCommand), "", 1, 128, -1),
TypeDescriptorField(NULL, DT_NULL, sizeof(HelpCode), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_HelpCode("HelpCodes", 0, HelpCode_Fields, 4, NULL, 0x0, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_HELPCODES = &TypeDescriptor_HelpCode;

static const TypeDescriptorField HelpCodeTable_Fields[3] = {
TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(HelpCodeTable, serHelpCodes), NULL, 0), VariableArrayTypeDescriptorField("", DT_HELPCODES, offsetof(HelpCodeTable, ptHelpCodes), 0x21, offsetof(HelpCodeTable, serHelpCodes) - offsetof(HelpCodeTable, ptHelpCodes)),
TypeDescriptorField(NULL, DT_NULL, sizeof(HelpCodeTable), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_HelpCodeTable("HelpCodes", 0, HelpCodeTable_Fields, 3, NULL, 0x0, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_HELPCODESTABLE = &TypeDescriptor_HelpCodeTable;

static const TypeDescriptorField ItemTierData_Fields[9] = {
FilterTypeDescriptorField("", DT_GBID, offsetof(ItemTierData, gbidHead), "", 2, 0, -1),
FilterTypeDescriptorField("", DT_GBID, offsetof(ItemTierData, gbidTorso), "", 2, 0, -1),
FilterTypeDescriptorField("", DT_GBID, offsetof(ItemTierData, gbidFeet), "", 2, 0, -1),
FilterTypeDescriptorField("", DT_GBID, offsetof(ItemTierData, gbidHands), "", 2, 0, -1),
FilterTypeDescriptorField("", DT_GBID, offsetof(ItemTierData, gbidShoulders), "", 2, 0, -1),
FilterTypeDescriptorField("", DT_GBID, offsetof(ItemTierData, gbidLegs), "", 2, 0, -1),
FilterTypeDescriptorField("", DT_GBID, offsetof(ItemTierData, gbidBracers), "", 2, 0, -1),
FilterTypeDescriptorField("", DT_GBID, offsetof(ItemTierData, gbidBelt), "", 2, 0, -1),
TypeDescriptorField(NULL, DT_NULL, sizeof(ItemTierData), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_ItemTierData("ItemTierData", 0, ItemTierData_Fields, 9, NULL, 0x0, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_ITEM_TIER_DATA = &TypeDescriptor_ItemTierData;

static const TypeDescriptorField ItemTierTable_Fields[3] = {
TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(ItemTierTable, serTiers), NULL, 0), VariableArrayTypeDescriptorField("", DT_ITEM_TIER_DATA, offsetof(ItemTierTable, ptTiers), 0x21, offsetof(ItemTierTable, serTiers) - offsetof(ItemTierTable, ptTiers)),
TypeDescriptorField(NULL, DT_NULL, sizeof(ItemTierTable), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_ItemTierTable("ItemTiers", 0, ItemTierTable_Fields, 3, NULL, 0x0, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_ITEM_TIER_TABLE = &TypeDescriptor_ItemTierTable;


static const TypeDescriptorField LootDistEntry_Fields[24] = {
TypeDescriptorField("", DT_INT, offsetof(LootDistEntry, nLevel), (const void *)-99, 1),
TypeDescriptorField("", DT_INT, offsetof(LootDistEntry, nGoldWeight), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(LootDistEntry, nGoldWeight[1]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(LootDistEntry, nGoldWeight[2]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(LootDistEntry, nGoldMin), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(LootDistEntry, nGoldMin[1]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(LootDistEntry, nGoldMin[2]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(LootDistEntry, nGoldDelta), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(LootDistEntry, nGoldDelta[1]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(LootDistEntry, nGoldDelta[2]), NULL, 1),

TypeDescriptorField("", DT_FLOAT, offsetof(LootDistEntry, flQualityLevelChance), (const void *)0x42c80000 /* 100f */, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(LootDistEntry, flQualityLevelChance[1]), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(LootDistEntry, flQualityLevelChance[2]), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(LootDistEntry, flQualityLevelChance[3]), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(LootDistEntry, flQualityLevelChance[4]), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(LootDistEntry, flQualityLevelChance[5]), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(LootDistEntry, flQualityLevelChance[6]), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(LootDistEntry, flQualityLevelChance[7]), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(LootDistEntry, flQualityLevelChance[8]), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(LootDistEntry, flQualityLevelChance[9]), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(LootDistEntry, flQualityLevelChance[10]), NULL, 1),

TypeDescriptorField("", DT_INT, offsetof(LootDistEntry, nCurseChance), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(LootDistEntry, nCurseBonus), NULL, 1),
TypeDescriptorField(NULL, DT_NULL, sizeof(LootDistEntry), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_LootDistEntry("LootDistribution", 0, LootDistEntry_Fields, 24, NULL, 0x0, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_LOOTDISTENTRY = &TypeDescriptor_LootDistEntry;

static const TypeDescriptorField LootDistributionTable_Fields[3] = {
TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(LootDistributionTable, serLootDistributions), NULL, 0), VariableArrayTypeDescriptorField("", DT_LOOTDISTENTRY, offsetof(LootDistributionTable, ptLootDistributions), 0x21, offsetof(LootDistributionTable, serLootDistributions) - offsetof(LootDistributionTable, ptLootDistributions)),
TypeDescriptorField(NULL, DT_NULL, sizeof(LootDistributionTable), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_LootDistributionTable("LootDistributionTable", 0, LootDistributionTable_Fields, 3, NULL, 0x0, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_LOOTDISTRIBUTIONTABLE = &TypeDescriptor_LootDistributionTable;

static const TypeDescriptorField ItemLevelModifier_Fields[24] = {
TypeDescriptorField("", DT_INT, offsetof(ItemLevelModifier, nLevelDiff), (const void *)-99, 1),

TypeDescriptorField("", DT_FLOAT, offsetof(ItemLevelModifier, flQualityLevelScalar), (const void *)0x42c80000 /* 100f */, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(ItemLevelModifier, flQualityLevelScalar[1]), (const void *)0x42c80000 /* 100f */, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(ItemLevelModifier, flQualityLevelScalar[2]), (const void *)0x42c80000 /* 100f */, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(ItemLevelModifier, flQualityLevelScalar[3]), (const void *)0x42c80000 /* 100f */, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(ItemLevelModifier, flQualityLevelScalar[4]), (const void *)0x42c80000 /* 100f */, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(ItemLevelModifier, flQualityLevelScalar[5]), (const void *)0x42c80000 /* 100f */, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(ItemLevelModifier, flQualityLevelScalar[6]), (const void *)0x42c80000 /* 100f */, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(ItemLevelModifier, flQualityLevelScalar[7]), (const void *)0x42c80000 /* 100f */, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(ItemLevelModifier, flQualityLevelScalar[8]), (const void *)0x42c80000 /* 100f */, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(ItemLevelModifier, flQualityLevelScalar[9]), (const void *)0x42c80000 /* 100f */, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(ItemLevelModifier, flQualityLevelScalar[10]), (const void *)0x42c80000 /* 100f */, 1),

TypeDescriptorField("", DT_INT, offsetof(ItemLevelModifier, nSocketWeight), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemLevelModifier, nSocketWeight[1]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemLevelModifier, nSocketWeight[2]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemLevelModifier, nSocketWeight[3]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemLevelModifier, nSocketWeight[4]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemLevelModifier, nSocketWeight[5]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemLevelModifier, nSocketWeight[6]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemLevelModifier, nSocketWeight[7]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemLevelModifier, nSocketWeight[8]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemLevelModifier, nSocketWeight[9]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemLevelModifier, nSocketWeight[10]), NULL, 1),
TypeDescriptorField(NULL, DT_NULL, sizeof(ItemLevelModifier), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_ItemLevelModifier("LootDistribution", 0, ItemLevelModifier_Fields, 24, NULL, 0x0, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_ITEM_LEVEL_MODIFIER = &TypeDescriptor_ItemLevelModifier;

static const TypeDescriptorField ItemLevelModifierTable_Fields[3] = {
TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(ItemLevelModifierTable, serItemLevelModifiers), NULL, 0), VariableArrayTypeDescriptorField("", DT_ITEM_LEVEL_MODIFIER, offsetof(ItemLevelModifierTable, ptItemLevelModifiers), 0x21, offsetof(ItemLevelModifierTable, serItemLevelModifiers) - offsetof(ItemLevelModifierTable, ptItemLevelModifiers)),
TypeDescriptorField(NULL, DT_NULL, sizeof(ItemLevelModifierTable), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_ItemLevelModifierTable("ItemLevelModTable", 0, ItemLevelModifierTable_Fields, 3, NULL, 0x0, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_ITEM_LEVEL_MODIFIER_TABLE = &TypeDescriptor_ItemLevelModifierTable;

static const TypeDescriptorField QualityClass_Fields[28] = {
TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(QualityClass, tHeader), NULL, 7340032), VariableArrayTypeDescriptorField("", DT_BYTE, offsetof(QualityClass, tHeader.szName), 0x700020, offsetof(QualityClass, tHeader) - offsetof(QualityClass, tHeader.szName)), TypeDescriptorField("", DT_INT, offsetof(QualityClass, tHeader.gbid), NULL, 0), TypeDescriptorField("", DT_INT, offsetof(QualityClass, tHeader.dwPad), NULL, 0),
TypeDescriptorField("", DT_FLOAT, offsetof(QualityClass, flQualityLevelScalar), (const void *)0x42c80000 /* 100f */, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(QualityClass, flQualityLevelScalar[1]), (const void *)0x42c80000 /* 100f */, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(QualityClass, flQualityLevelScalar[2]), (const void *)0x42c80000 /* 100f */, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(QualityClass, flQualityLevelScalar[3]), (const void *)0x42c80000 /* 100f */, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(QualityClass, flQualityLevelScalar[4]), (const void *)0x42c80000 /* 100f */, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(QualityClass, flQualityLevelScalar[5]), (const void *)0x42c80000 /* 100f */, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(QualityClass, flQualityLevelScalar[6]), (const void *)0x42c80000 /* 100f */, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(QualityClass, flQualityLevelScalar[7]), (const void *)0x42c80000 /* 100f */, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(QualityClass, flQualityLevelScalar[8]), (const void *)0x42c80000 /* 100f */, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(QualityClass, flQualityLevelScalar[9]), (const void *)0x42c80000 /* 100f */, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(QualityClass, flQualityLevelScalar[10]), (const void *)0x42c80000 /* 100f */, 1),

TypeDescriptorField("", DT_FLOAT, offsetof(QualityClass, flQualityLevelAdd), (const void *)0x42c80000 /* 100f */, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(QualityClass, flQualityLevelAdd[1]), (const void *)0x42c80000 /* 100f */, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(QualityClass, flQualityLevelAdd[2]), (const void *)0x42c80000 /* 100f */, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(QualityClass, flQualityLevelAdd[3]), (const void *)0x42c80000 /* 100f */, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(QualityClass, flQualityLevelAdd[4]), (const void *)0x42c80000 /* 100f */, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(QualityClass, flQualityLevelAdd[5]), (const void *)0x42c80000 /* 100f */, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(QualityClass, flQualityLevelAdd[6]), (const void *)0x42c80000 /* 100f */, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(QualityClass, flQualityLevelAdd[7]), (const void *)0x42c80000 /* 100f */, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(QualityClass, flQualityLevelAdd[8]), (const void *)0x42c80000 /* 100f */, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(QualityClass, flQualityLevelAdd[9]), (const void *)0x42c80000 /* 100f */, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(QualityClass, flQualityLevelAdd[10]), (const void *)0x42c80000 /* 100f */, 1),
TypeDescriptorField("", DT_INT, offsetof(QualityClass, dwPad), NULL, 0),
TypeDescriptorField(NULL, DT_NULL, sizeof(QualityClass), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_QualityClass("QualityClass", 0, QualityClass_Fields, 28, NULL, 0x0, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_QUALITY_CLASS = &TypeDescriptor_QualityClass;

static const TypeDescriptorField QualityClassTable_Fields[3] = {
TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(QualityClassTable, serQualityClasses), NULL, 0), VariableArrayTypeDescriptorField("", DT_QUALITY_CLASS, offsetof(QualityClassTable, ptQualityClasses), 0x21, offsetof(QualityClassTable, serQualityClasses) - offsetof(QualityClassTable, ptQualityClasses)),
TypeDescriptorField(NULL, DT_NULL, sizeof(QualityClassTable), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_QualityClassTable("QualityClassTable", 0, QualityClassTable_Fields, 3, NULL, 0x0, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_QUALITY_CLASS_TABLE = &TypeDescriptor_QualityClassTable;

static const TypeDescriptorField HandicapLevel_Fields[9] = {
TypeDescriptorField("", DT_FLOAT, offsetof(HandicapLevel, tHandicapValues), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(HandicapLevel, tHandicapValues.flDamageScalar), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(HandicapLevel, tHandicapValues.flMagicFind), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(HandicapLevel, tHandicapValues.flGoldFind), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(HandicapLevel, tHandicapValues.flXPBonus), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(HandicapLevel, tHandicapValues.flCrushingBlowScalar), (const void *)0x3f800000 /* 1f */, 1),
TypeDescriptorField("", DT_INT, offsetof(HandicapLevel, tHandicapValues.nSignificantKillPlatinum), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(HandicapLevel, dwPad), NULL, 0),
TypeDescriptorField(NULL, DT_NULL, sizeof(HandicapLevel), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_HandicapLevel("HandicapLevel", 0, HandicapLevel_Fields, 9, NULL, 0x0, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_HANDICAP_LEVEL = &TypeDescriptor_HandicapLevel;

static const TypeDescriptorField HandicapLevelTable_Fields[3] = {
TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(HandicapLevelTable, serHandicapLevels), NULL, 0), VariableArrayTypeDescriptorField("", DT_HANDICAP_LEVEL, offsetof(HandicapLevelTable, ptHandicapLevels), 0x21, offsetof(HandicapLevelTable, serHandicapLevels) - offsetof(HandicapLevelTable, ptHandicapLevels)),
TypeDescriptorField(NULL, DT_NULL, sizeof(HandicapLevelTable), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_HandicapLevelTable("HandicapLevelTable", 0, HandicapLevelTable_Fields, 3, NULL, 0x0, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_HANDICAP_LEVEL_TABLE = &TypeDescriptor_HandicapLevelTable;

static const TypeDescriptorField TieredLootRunLevel_Fields[14] = {
TypeDescriptorField("", DT_FLOAT, offsetof(TieredLootRunLevel, tLootRunValues), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(TieredLootRunLevel, tLootRunValues.flDamageScalar), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(TieredLootRunLevel, tLootRunValues.flMagicFind), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(TieredLootRunLevel, tLootRunValues.flGoldFind), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(TieredLootRunLevel, tLootRunValues.flXPBonus), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(TieredLootRunLevel, tLootRunValues.flLegendaryDropBonus), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(TieredLootRunLevel, tLootRunValues.flBloodShardBonus), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(TieredLootRunLevel, tLootRunValues.flAncientLegendaryChance), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(TieredLootRunLevel, tLootRunValues.nJewelUpgradeCharges), (const void *)1, 1),
TypeDescriptorField("", DT_INT, offsetof(TieredLootRunLevel, tLootRunValues.nSignificantKillPlatinum), NULL, 1),
TypeDescriptorField("", DT_INT64, offsetof(TieredLootRunLevel, tLootRunValues.nEmpowerCost), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(TieredLootRunLevel, tLootRunValues.flMonsterHPPerAdditionalPlayer), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(TieredLootRunLevel, tLootRunValues.flXPReductionScalar), NULL, 1),

TypeDescriptorField(NULL, DT_NULL, sizeof(TieredLootRunLevel), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_TieredLootRunLevel("TieredLootRunLevel", 0, TieredLootRunLevel_Fields, 14, NULL, 0x0, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_TIERED_LOOT_RUN_LEVEL = &TypeDescriptor_TieredLootRunLevel;

static const TypeDescriptorField TieredLootRunLevelTable_Fields[3] = {
TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(TieredLootRunLevelTable, serTieredLootRunLevels), NULL, 0), VariableArrayTypeDescriptorField("", DT_TIERED_LOOT_RUN_LEVEL, offsetof(TieredLootRunLevelTable, ptTieredLootRunLevels), 0x21, offsetof(TieredLootRunLevelTable, serTieredLootRunLevels) - offsetof(TieredLootRunLevelTable, ptTieredLootRunLevels)),
TypeDescriptorField(NULL, DT_NULL, sizeof(TieredLootRunLevelTable), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_TieredLootRunLevelTable("TieredLootRunLevelTable", 0, TieredLootRunLevelTable_Fields, 3, NULL, 0x0, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_TIERED_LOOT_RUN_LEVEL_TABLE = &TypeDescriptor_TieredLootRunLevelTable;

static const TypeDescriptorField ItemSalvageLevel_Fields[5] = {
FilterTypeDescriptorField("", DT_SNO, offsetof(ItemSalvageLevel, snoSalvageTreasureClassNormal), "", 52, 0, -1),
FilterTypeDescriptorField("", DT_SNO, offsetof(ItemSalvageLevel, snoSalvageTreasureClassMagic), "", 52, 0, -1),
FilterTypeDescriptorField("", DT_SNO, offsetof(ItemSalvageLevel, snoSalvageTreasureClassRare), "", 52, 0, -1),
FilterTypeDescriptorField("", DT_SNO, offsetof(ItemSalvageLevel, snoSalvageTreasureClassLegendary), "", 52, 0, -1),
TypeDescriptorField(NULL, DT_NULL, sizeof(ItemSalvageLevel), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_ItemSalvageLevel("ItemSalvageLevel", 0, ItemSalvageLevel_Fields, 5, NULL, 0x0, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_ITEM_SALVAGE_LEVEL = &TypeDescriptor_ItemSalvageLevel;

static const TypeDescriptorField ItemSalvageLevelTable_Fields[3] = {
TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(ItemSalvageLevelTable, serItemSalvageLevels), NULL, 0), VariableArrayTypeDescriptorField("", DT_ITEM_SALVAGE_LEVEL, offsetof(ItemSalvageLevelTable, ptItemSalvageLevels), 0x21, offsetof(ItemSalvageLevelTable, serItemSalvageLevels) - offsetof(ItemSalvageLevelTable, ptItemSalvageLevels)),
TypeDescriptorField(NULL, DT_NULL, sizeof(ItemSalvageLevelTable), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_ItemSalvageLevelTable("ItemSalvageLevelTable", 0, ItemSalvageLevelTable_Fields, 3, NULL, 0x0, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_ITEM_SALVAGE_LEVEL_TABLE = &TypeDescriptor_ItemSalvageLevelTable;


static const TypeDescriptorField ParagonBonus_Fields[11] = {
TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(ParagonBonus, tHeader), NULL, 7340032), VariableArrayTypeDescriptorField("", DT_BYTE, offsetof(ParagonBonus, tHeader.szName), 0x700020, offsetof(ParagonBonus, tHeader) - offsetof(ParagonBonus, tHeader.szName)), TypeDescriptorField("", DT_INT, offsetof(ParagonBonus, tHeader.gbid), NULL, 0), TypeDescriptorField("", DT_INT, offsetof(ParagonBonus, tHeader.dwPad), NULL, 0),
TypeDescriptorField("", DT_INT, offsetof(ParagonBonus, nPointsMax), NULL, 1),
FixedArrayTypeDescriptorField("", DT_ATTRIBUTE_SPECIFIER, offsetof(ParagonBonus, ptAttributes), NULL, 1, 4, 0),
TypeDescriptorField("", DT_INT, offsetof(ParagonBonus, nCategory), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ParagonBonus, nIndex), NULL, 1),
EnumTypeDescriptorField("", offsetof(ParagonBonus, ePlayerClass), (const void *)-1, gPlayerClasses),
CharArrayTypeDescriptorField("", offsetof(ParagonBonus, szIcon), "", 1, 256, -1),
TypeDescriptorField(NULL, DT_NULL, sizeof(ParagonBonus), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_ParagonBonus("ParagonBonus", 0, ParagonBonus_Fields, 11, NULL, 0x0, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_PARAGON_BONUS = &TypeDescriptor_ParagonBonus;

static const TypeDescriptorField ParagonBonusesTable_Fields[3] = {
TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(ParagonBonusesTable, serParagonBonuses), NULL, 0), VariableArrayTypeDescriptorField("", DT_PARAGON_BONUS, offsetof(ParagonBonusesTable, ptParagonBonuses), 0x21, offsetof(ParagonBonusesTable, serParagonBonuses) - offsetof(ParagonBonusesTable, ptParagonBonuses)),
TypeDescriptorField(NULL, DT_NULL, sizeof(ParagonBonusesTable), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_ParagonBonusesTable("ParagonBonusesTable", 0, ParagonBonusesTable_Fields, 3, NULL, 0x0, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_PARAGON_BONUSES_TABLE = &TypeDescriptor_ParagonBonusesTable;

static const TypeDescriptorField HeroData_Fields[65] = {
TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(HeroData, tHeader), NULL, 7340032), VariableArrayTypeDescriptorField("", DT_BYTE, offsetof(HeroData, tHeader.szName), 0x700020, offsetof(HeroData, tHeader) - offsetof(HeroData, tHeader.szName)), TypeDescriptorField("", DT_INT, offsetof(HeroData, tHeader.gbid), NULL, 0), TypeDescriptorField("", DT_INT, offsetof(HeroData, tHeader.dwPad), NULL, 0),
FilterTypeDescriptorField("", DT_SNO, offsetof(HeroData, snoActorMale), "", 1, 0, -1),
FilterTypeDescriptorField("", DT_SNO, offsetof(HeroData, snoActorFemale), "", 1, 0, -1),
FilterTypeDescriptorField("", DT_SNO, offsetof(HeroData, snoInventory), "", 52, 0, -1),
TypeDescriptorField("", DT_INT, offsetof(HeroData, nMaxTrainableSkills), (const void *)99, 1),
FilterTypeDescriptorField("", DT_SNO, offsetof(HeroData, snoStartingLMBSkill), "", 29, 0, -1),
FilterTypeDescriptorField("", DT_SNO, offsetof(HeroData, snoStartingRMBSkill), "", 29, 0, -1),

FilterTypeDescriptorField("", DT_SNO, offsetof(HeroData, arSkillKit), "", 39, 0, -1),
FilterTypeDescriptorField("", DT_SNO, offsetof(HeroData, arSkillKit[1]), "", 39, 0, -1),
FilterTypeDescriptorField("", DT_SNO, offsetof(HeroData, arSkillKit[2]), "", 39, 0, -1),
FilterTypeDescriptorField("", DT_SNO, offsetof(HeroData, arSkillKit[3]), "", 39, 0, -1),

TypeDescriptorField("", DT_FLOAT, offsetof(HeroData, flPlayerAwarenessRadius), (const void *)0x42200000 /* 40f */, 1),
TypeDescriptorField("", DT_INT, offsetof(HeroData, bIsRanged), NULL, 1),

TypeDescriptorField("", DT_FLOAT, offsetof(HeroData, flAttributesStrength), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(HeroData, flAttributesDexterity), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(HeroData, flAttributesIntelligence), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(HeroData, flAttributesVitality), NULL, 1),

TypeDescriptorField("", DT_FLOAT, offsetof(HeroData, flAttributesHitPoints), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(HeroData, flAttributesHitPointsFactorLevel), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(HeroData, flAttributesHPRegen), NULL, 1),

TypeDescriptorField("", DT_FLOAT, offsetof(HeroData, flAttributesClassDamageReductionPercent), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(HeroData, flAttributesClassDamageReductionPercentPVP), NULL, 1),

EnumTypeDescriptorField("", offsetof(HeroData, ePrimaryResourceType), NULL, gResourceTypes),
EnumTypeDescriptorField("", offsetof(HeroData, eSecondaryResourceType), (const void *)-1, gResourceTypes),
EnumTypeDescriptorField("", offsetof(HeroData, ePrimaryDamageAttribute), (const void *)-1, gPrimaryDamageAttributes),

TypeDescriptorField("", DT_FLOAT, offsetof(HeroData, flAttributesPrimaryResourceBase), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(HeroData, flAttributesPrimaryResourceFactorLevel), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(HeroData, flAttributesPrimaryResourceRegen), NULL, 1),

TypeDescriptorField("", DT_FLOAT, offsetof(HeroData, flAttributesSecondaryResourceBase), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(HeroData, flAttributesSecondaryResourceFactorLevel), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(HeroData, flAttributesSecondaryResourceRegen), NULL, 1),

TypeDescriptorField("", DT_FLOAT, offsetof(HeroData, flAttributesArmor), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(HeroData, flAttributesDamage), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(HeroData, flAttributesWalkSpeed), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(HeroData, flAttributesRunSpeed), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(HeroData, flAttributesSprintSpeed), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(HeroData, flAttributesProjSpeed), NULL, 1),

TypeDescriptorField("", DT_FLOAT, offsetof(HeroData, flAttributesCritDamageCap), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(HeroData, flAttributesCritPercentBase), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(HeroData, flAttributesCritPercentCap), NULL, 1),

TypeDescriptorField("", DT_FLOAT, offsetof(HeroData, flAttributesDodgeRatingBase), NULL, 1),

TypeDescriptorField("", DT_FLOAT, offsetof(HeroData, flAttributesGetHitMaxBase), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(HeroData, flAttributesGetHitMaxPerLevel), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(HeroData, flAttributesGetHitRecoveryBase), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(HeroData, flAttributesGetHitRecoveryPerLevel), NULL, 1),

TypeDescriptorField("", DT_FLOAT, offsetof(HeroData, flAttributesResistPhysical), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(HeroData, flAttributesResistFire), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(HeroData, flAttributesResistLightning), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(HeroData, flAttributesResistCold), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(HeroData, flAttributesResistPoison), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(HeroData, flAttributesResistArcane), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(HeroData, flAttributesResistChill), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(HeroData, flAttributesResistStun), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(HeroData, flAttributesKnockbackWeight), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(HeroData, flAttributesOOCHealthRegenPercent), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(HeroData, flAttributesOOCManaRegenPercent), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(HeroData, flAttributesPotionDilutionDuration), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(HeroData, flAttributesPotionDilutionScalar), NULL, 1),

TypeDescriptorField("", DT_FLOAT, offsetof(HeroData, flAttributesDualWieldBothAttackChance), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(HeroData, flAttributesFreezeCapacity), (const void *)0x3f800000 /* 1f */, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(HeroData, flAttributesThawRate), (const void *)0x3dcccccd /* 0.1f */, 1),

TypeDescriptorField(NULL, DT_NULL, sizeof(HeroData), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_HeroData("HeroData", 0, HeroData_Fields, 65, NULL, 0x0, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_HERO_DATA = &TypeDescriptor_HeroData;

static const TypeDescriptorField HeroTable_Fields[3] = {
TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(HeroTable, serHeros), NULL, 0), VariableArrayTypeDescriptorField("", DT_HERO_DATA, offsetof(HeroTable, ptHeros), 0x21, offsetof(HeroTable, serHeros) - offsetof(HeroTable, ptHeros)),
TypeDescriptorField(NULL, DT_NULL, sizeof(HeroTable), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_HeroTable("Heros", 0, HeroTable_Fields, 3, NULL, 0x0, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_HEROTABLE = &TypeDescriptor_HeroTable;

static const TypeDescriptorField MovementStyleDefinition_Fields[37] = {
TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(MovementStyleDefinition, tHeader), NULL, 7340032), VariableArrayTypeDescriptorField("", DT_BYTE, offsetof(MovementStyleDefinition, tHeader.szName), 0x700020, offsetof(MovementStyleDefinition, tHeader) - offsetof(MovementStyleDefinition, tHeader.szName)), TypeDescriptorField("", DT_INT, offsetof(MovementStyleDefinition, tHeader.gbid), NULL, 0), TypeDescriptorField("", DT_INT, offsetof(MovementStyleDefinition, tHeader.dwPad), NULL, 0),
TypeDescriptorField("", DT_INT, offsetof(MovementStyleDefinition, fWalkSwitchStyle), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(MovementStyleDefinition, fStopSwitchStyle), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(MovementStyleDefinition, fWalkBlockStyle), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(MovementStyleDefinition, fStopBlockStyle), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(MovementStyleDefinition, fFlankStyle), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(MovementStyleDefinition, fZigZagStyle), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(MovementStyleDefinition, fSprintStyle), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(MovementStyleDefinition, fSprintRunRandomStyle), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(MovementStyleDefinition, flRunTimer), (const void *)0x3f000000 /* 0.5f */, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(MovementStyleDefinition, flRunDeltaTimer), (const void *)0x3f000000 /* 0.5f */, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(MovementStyleDefinition, flWalkTimer), (const void *)0x3f800000 /* 1f */, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(MovementStyleDefinition, flWalkDeltaTimer), (const void *)0x3f000000 /* 0.5f */, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(MovementStyleDefinition, flSwitchChance), (const void *)0x3f800000 /* 1f */, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(MovementStyleDefinition, flBlockTimer), (const void *)0x3f800000 /* 1f */, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(MovementStyleDefinition, flBlockDeltaTimer), (const void *)0x3f000000 /* 0.5f */, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(MovementStyleDefinition, flBlockCooldown), (const void *)0x3f800000 /* 1f */, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(MovementStyleDefinition, flBlockCooldownDelta), (const void *)0x3f000000 /* 0.5f */, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(MovementStyleDefinition, flBlockChance), (const void *)0x3f800000 /* 1f */, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(MovementStyleDefinition, flBlockDistance), (const void *)0x41000000 /* 8f */, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(MovementStyleDefinition, flFlankAngle), (const void *)0x41c80000 /* 25f */, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(MovementStyleDefinition, flFlankDeltaAngle), (const void *)0x41f00000 /* 30f */, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(MovementStyleDefinition, flFlankPeakDistance), (const void *)0x42200000 /* 40f */, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(MovementStyleDefinition, flFlankMaxDistance), (const void *)0x428c0000 /* 70f */, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(MovementStyleDefinition, flZigZagTimer), (const void *)0x3f000000 /* 0.5f */, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(MovementStyleDefinition, flZigZagAngle), (const void *)0x41c80000 /* 25f */, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(MovementStyleDefinition, flZigZagDeltaAngle), (const void *)0x41f00000 /* 30f */, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(MovementStyleDefinition, flZigZagPeakDistance), (const void *)0x42200000 /* 40f */, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(MovementStyleDefinition, flZigZagMaxDistance), (const void *)0x428c0000 /* 70f */, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(MovementStyleDefinition, flAccel), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(MovementStyleDefinition, flSpeedScalar), NULL, 1),
FilterTypeDescriptorField("", DT_SNO, offsetof(MovementStyleDefinition, snoPowerToBreakObjects), "", 29, 0, -1),
TypeDescriptorField("", DT_INT, offsetof(MovementStyleDefinition, dwPad), NULL, 0),
TypeDescriptorField(NULL, DT_NULL, sizeof(MovementStyleDefinition), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_MovementStyleDefinition("MovementStyleDefinition", 0, MovementStyleDefinition_Fields, 37, NULL, 0x0, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_MOVEMENT_STYLE_DEFINITION = &TypeDescriptor_MovementStyleDefinition;

static const TypeDescriptorField MovementStyleTable_Fields[3] = {
TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(MovementStyleTable, serMovementStyles), NULL, 0), VariableArrayTypeDescriptorField("", DT_MOVEMENT_STYLE_DEFINITION, offsetof(MovementStyleTable, ptMovementStyles), 0x21, offsetof(MovementStyleTable, serMovementStyles) - offsetof(MovementStyleTable, ptMovementStyles)),
TypeDescriptorField(NULL, DT_NULL, sizeof(MovementStyleTable), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_MovementStyleTable("MovementStyles", 0, MovementStyleTable_Fields, 3, NULL, 0x0, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_MOVEMENT_STYLE_TABLE = &TypeDescriptor_MovementStyleTable;



static const TypeDescriptorField LabelGBID_Fields[7] = {
TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(LabelGBID, tHeader), NULL, 7340032), VariableArrayTypeDescriptorField("", DT_BYTE, offsetof(LabelGBID, tHeader.szName), 0x700020, offsetof(LabelGBID, tHeader) - offsetof(LabelGBID, tHeader.szName)), TypeDescriptorField("", DT_INT, offsetof(LabelGBID, tHeader.gbid), NULL, 0), TypeDescriptorField("", DT_INT, offsetof(LabelGBID, tHeader.dwPad), NULL, 0),
TypeDescriptorField("", DT_INT, offsetof(LabelGBID, nIndex), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(LabelGBID, dwPad), NULL, 0),
TypeDescriptorField(NULL, DT_NULL, sizeof(LabelGBID), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_LabelGBID("LabelGBID", 0, LabelGBID_Fields, 7, NULL, 0x0, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_LABEL_GBID = &TypeDescriptor_LabelGBID;

static const TypeDescriptorField LabelGBIDTable_Fields[3] = {
TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(LabelGBIDTable, serLabelGBIDs), NULL, 0), VariableArrayTypeDescriptorField("", DT_LABEL_GBID, offsetof(LabelGBIDTable, ptLabelGBIDs), 0x21, offsetof(LabelGBIDTable, serLabelGBIDs) - offsetof(LabelGBIDTable, ptLabelGBIDs)),
TypeDescriptorField(NULL, DT_NULL, sizeof(LabelGBIDTable), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_LabelGBIDTable("Labels", 0, LabelGBIDTable_Fields, 3, NULL, 0x0, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_LABEL_GBID_TABLE = &TypeDescriptor_LabelGBIDTable;




static const TypeDescriptorField MonsterLevelDefinition_Fields[61] = {
TypeDescriptorField("", DT_INT, offsetof(MonsterLevelDefinition, nLevel), (const void *)-1, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(MonsterLevelDefinition, flAttributesStrength), (const void *)0x3f800000 /* 1f */, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(MonsterLevelDefinition, flAttributesDexterity), (const void *)0x3f800000 /* 1f */, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(MonsterLevelDefinition, flAttributesIntelligence), (const void *)0x3f800000 /* 1f */, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(MonsterLevelDefinition, flAttributesVitality), (const void *)0x3f800000 /* 1f */, 1),

TypeDescriptorField("", DT_FLOAT, offsetof(MonsterLevelDefinition, flAttributesHitPoints), (const void *)0x3f800000 /* 1f */, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(MonsterLevelDefinition, flAttributesHPRange), (const void *)0x3f800000 /* 1f */, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(MonsterLevelDefinition, flAttributesHPRegen), (const void *)0x3f800000 /* 1f */, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(MonsterLevelDefinition, flAttributesPrimaryResourceBase), (const void *)0x3f800000 /* 1f */, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(MonsterLevelDefinition, flAttributesPrimaryResourceRegen), (const void *)0x3f800000 /* 1f */, 1),

TypeDescriptorField("", DT_FLOAT, offsetof(MonsterLevelDefinition, flAttributesArmor), (const void *)0x3f800000 /* 1f */, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(MonsterLevelDefinition, flAttributesDamage), (const void *)0x3f800000 /* 1f */, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(MonsterLevelDefinition, flAttributesDamageRange), (const void *)0x3f800000 /* 1f */, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(MonsterLevelDefinition, flAttributesDamageFire), (const void *)0x3f800000 /* 1f */, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(MonsterLevelDefinition, flAttributesDamageRangeFire), (const void *)0x3f800000 /* 1f */, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(MonsterLevelDefinition, flAttributesDamageLightning), (const void *)0x3f800000 /* 1f */, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(MonsterLevelDefinition, flAttributesDamageRangeLightning), (const void *)0x3f800000 /* 1f */, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(MonsterLevelDefinition, flAttributesDamageCold), (const void *)0x3f800000 /* 1f */, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(MonsterLevelDefinition, flAttributesDamageRangeCold), (const void *)0x3f800000 /* 1f */, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(MonsterLevelDefinition, flAttributesDamagePoison), (const void *)0x3f800000 /* 1f */, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(MonsterLevelDefinition, flAttributesDamageRangePoison), (const void *)0x3f800000 /* 1f */, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(MonsterLevelDefinition, flAttributesDamageArcane), (const void *)0x3f800000 /* 1f */, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(MonsterLevelDefinition, flAttributesDamageRangeArcane), (const void *)0x3f800000 /* 1f */, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(MonsterLevelDefinition, flAttributesDamageHoly), (const void *)0x3f800000 /* 1f */, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(MonsterLevelDefinition, flAttributesDamageRangeHoly), (const void *)0x3f800000 /* 1f */, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(MonsterLevelDefinition, flAttributesDamageSiege), (const void *)0x3f800000 /* 1f */, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(MonsterLevelDefinition, flAttributesDamageRangeSiege), (const void *)0x3f800000 /* 1f */, 1),

TypeDescriptorField("", DT_FLOAT, offsetof(MonsterLevelDefinition, flAttributesHirelingHitPoints), (const void *)0x3f800000 /* 1f */, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(MonsterLevelDefinition, flAttributesHirelingHPRange), (const void *)0x3f800000 /* 1f */, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(MonsterLevelDefinition, flAttributesHirelingHPRegen), (const void *)0x3f800000 /* 1f */, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(MonsterLevelDefinition, flAttributesHirelingDamage), (const void *)0x3f800000 /* 1f */, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(MonsterLevelDefinition, flAttributesHirelingDamageRange), (const void *)0x3f800000 /* 1f */, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(MonsterLevelDefinition, flAttributesHirelingRetrainCost), (const void *)0x3f800000 /* 1f */, 1),

TypeDescriptorField("", DT_FLOAT, offsetof(MonsterLevelDefinition, flAttributesGetHitDamage), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(MonsterLevelDefinition, flAttributesGetHitDamageScalar), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(MonsterLevelDefinition, flAttributesGetHitMax), (const void *)0x3f800000 /* 1f */, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(MonsterLevelDefinition, flAttributesGetHitRecovery), NULL, 1),

TypeDescriptorField("", DT_FLOAT, offsetof(MonsterLevelDefinition, flAttributesWalkSpeed), (const void *)0x3f800000 /* 1f */, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(MonsterLevelDefinition, flAttributesRunSpeed), (const void *)0x3f800000 /* 1f */, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(MonsterLevelDefinition, flAttributesSprintSpeed), (const void *)0x3f800000 /* 1f */, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(MonsterLevelDefinition, flAttributesStrafeSpeed), (const void *)0x3f800000 /* 1f */, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(MonsterLevelDefinition, flAttributesAttackSpeed), (const void *)0x3f800000 /* 1f */, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(MonsterLevelDefinition, flAttributesProjSpeed), (const void *)0x3f800000 /* 1f */, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(MonsterLevelDefinition, flAttributesExperience), (const void *)0x3f800000 /* 1f */, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(MonsterLevelDefinition, flAttributesResistPhysical), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(MonsterLevelDefinition, flAttributesResistFire), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(MonsterLevelDefinition, flAttributesResistLightning), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(MonsterLevelDefinition, flAttributesResistCold), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(MonsterLevelDefinition, flAttributesResistPoison), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(MonsterLevelDefinition, flAttributesResistArcane), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(MonsterLevelDefinition, flAttributesResistSiege), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(MonsterLevelDefinition, flAttributesResistChill), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(MonsterLevelDefinition, flAttributesResistStun), NULL, 1),

TypeDescriptorField("", DT_FLOAT, offsetof(MonsterLevelDefinition, flAttributesConsoleHealthScalar), (const void *)0x3f800000 /* 1f */, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(MonsterLevelDefinition, flAttributesConsoleDamageScalar), (const void *)0x3f800000 /* 1f */, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(MonsterLevelDefinition, flAttributesMonster1AffixWeight), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(MonsterLevelDefinition, flAttributesMonster2AffixWeight), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(MonsterLevelDefinition, flAttributesMonster3AffixWeight), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(MonsterLevelDefinition, flAttributesMonster4AffixWeight), NULL, 1),

TypeDescriptorField("", DT_INT, offsetof(MonsterLevelDefinition, dwPad), NULL, 0),

TypeDescriptorField(NULL, DT_NULL, sizeof(MonsterLevelDefinition), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_MonsterLevelDefinition("MonsterLevelDefinition", 0, MonsterLevelDefinition_Fields, 61, NULL, 0x0, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_MONSTER_LEVEL_DEFINITION = &TypeDescriptor_MonsterLevelDefinition;

static const TypeDescriptorField MonsterLevelTable_Fields[3] = {
TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(MonsterLevelTable, serMonsterLevels), NULL, 0), VariableArrayTypeDescriptorField("", DT_MONSTER_LEVEL_DEFINITION, offsetof(MonsterLevelTable, ptMonsterLevels), 0x21, offsetof(MonsterLevelTable, serMonsterLevels) - offsetof(MonsterLevelTable, ptMonsterLevels)),
TypeDescriptorField(NULL, DT_NULL, sizeof(MonsterLevelTable), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_MonsterLevelTable("MonsterLevelTable", 0, MonsterLevelTable_Fields, 3, NULL, 0x0, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_MONSTER_LEVEL_TABLE = &TypeDescriptor_MonsterLevelTable;

static const TypeDescriptorField AffixTableEntry_Fields[42] = {
TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(AffixTableEntry, tHeader), NULL, 7340032), VariableArrayTypeDescriptorField("", DT_BYTE, offsetof(AffixTableEntry, tHeader.szName), 0x700020, offsetof(AffixTableEntry, tHeader) - offsetof(AffixTableEntry, tHeader.szName)), TypeDescriptorField("", DT_INT, offsetof(AffixTableEntry, tHeader.gbid), NULL, 0), TypeDescriptorField("", DT_INT, offsetof(AffixTableEntry, tHeader.dwPad), NULL, 0),
TypeDescriptorField("", DT_INT, offsetof(AffixTableEntry, dwFlags), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(AffixTableEntry, nFrequencyWeight), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(AffixTableEntry, nClassFrequencyWeight), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(AffixTableEntry, nClassFrequencyWeight[1]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(AffixTableEntry, nClassFrequencyWeight[2]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(AffixTableEntry, nClassFrequencyWeight[3]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(AffixTableEntry, nClassFrequencyWeight[4]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(AffixTableEntry, nClassFrequencyWeight[5]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(AffixTableEntry, nClassFrequencyWeight[6]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(AffixTableEntry, nHirelingFrequencyWeight), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(AffixTableEntry, nHirelingFrequencyWeight[1]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(AffixTableEntry, nHirelingFrequencyWeight[2]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(AffixTableEntry, nHirelingFrequencyWeight[3]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(AffixTableEntry, nAffixLevelMin), (const void *)1, 1),
TypeDescriptorField("", DT_INT, offsetof(AffixTableEntry, nAffixLevelMax), (const void *)100, 1),
TypeDescriptorField("", DT_INT, offsetof(AffixTableEntry, nCost), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(AffixTableEntry, nIdentifyCost), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(AffixTableEntry, nNormalRequiredLevel), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(AffixTableEntry, nCrafterRequiredLevel), NULL, 1),
EnumTypeDescriptorField("", offsetof(AffixTableEntry, eItemEffectType), (const void *)-1, gItemEffectTypes),
TypeDescriptorField("", DT_INT, offsetof(AffixTableEntry, nItemEffectLevel), NULL, 1),
FilterTypeDescriptorField("", DT_GBID, offsetof(AffixTableEntry, gbidConvertsTo), "", 8, 0, -1),
FilterTypeDescriptorField("", DT_GBID, offsetof(AffixTableEntry, gbidLegendaryUprankAffix), "", 8, 0, -1),

FilterTypeDescriptorField("", DT_SNO, offsetof(AffixTableEntry, snoRareNamePrefixStringList), "", 42, 0, -1),
FilterTypeDescriptorField("", DT_SNO, offsetof(AffixTableEntry, snoRareNameSuffixStringList), "", 42, 0, -1),
FilterTypeDescriptorField("", DT_GBID, offsetof(AffixTableEntry, gbidAffixFamily0), "", 47, 0, -1),
FilterTypeDescriptorField("", DT_GBID, offsetof(AffixTableEntry, gbidAffixFamily1), "", 47, 0, -1),
TypeDescriptorField("", DT_INT, offsetof(AffixTableEntry, dwAllowedQualityLevels), NULL, 1),
EnumTypeDescriptorField("", offsetof(AffixTableEntry, eAffixType), (const void *)-1, gAffixTypes),
FixedArrayTypeDescriptorField("", DT_INT, offsetof(AffixTableEntry, arAllowedTypes), NULL, 1, 24, 0),
FixedArrayTypeDescriptorField("", DT_INT, offsetof(AffixTableEntry, arLegendaryAllowedTypes), NULL, 1, 24, 0),

FilterTypeDescriptorField("", DT_GBID, offsetof(AffixTableEntry, gbidAssociatedAffix), "", 8, 0, -1),

EnumTypeDescriptorField("", offsetof(AffixTableEntry, ePlayerClass), (const void *)-1, gPlayerClasses),
FilterTypeDescriptorField("", DT_GBID, offsetof(AffixTableEntry, gbidExclusionCategory), "", 8, 0, -1),
FixedArrayTypeDescriptorField("", DT_INT, offsetof(AffixTableEntry, arExcludedCategories), NULL, 1, 6, 0),

FixedArrayTypeDescriptorField("", DT_ATTRIBUTE_SPECIFIER, offsetof(AffixTableEntry, ptAffixAttributes), NULL, 1, 4, 0),

EnumTypeDescriptorField("", offsetof(AffixTableEntry, eAffixGroup), (const void *)-1, gAffixGroups),

TypeDescriptorField(NULL, DT_NULL, sizeof(AffixTableEntry), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_AffixTableEntry("AffixTableEntry", 0, AffixTableEntry_Fields, 42, NULL, 0x0, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_AFFIX_TABLE_ENTRY = &TypeDescriptor_AffixTableEntry;

static const TypeDescriptorField AffixTable_Fields[3] = {
TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(AffixTable, serAffixes), NULL, 0), VariableArrayTypeDescriptorField("", DT_AFFIX_TABLE_ENTRY, offsetof(AffixTable, ptAffixes), 0x21, offsetof(AffixTable, serAffixes) - offsetof(AffixTable, ptAffixes)),
TypeDescriptorField(NULL, DT_NULL, sizeof(AffixTable), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_AffixTable("AffixTable", 0, AffixTable_Fields, 3, NULL, 0x0, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_AFFIX_TABLE = &TypeDescriptor_AffixTable;

static const TypeDescriptorField SetItemBonusTableEntry_Fields[8] = {
TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(SetItemBonusTableEntry, tHeader), NULL, 7340032), VariableArrayTypeDescriptorField("", DT_BYTE, offsetof(SetItemBonusTableEntry, tHeader.szName), 0x700020, offsetof(SetItemBonusTableEntry, tHeader) - offsetof(SetItemBonusTableEntry, tHeader.szName)), TypeDescriptorField("", DT_INT, offsetof(SetItemBonusTableEntry, tHeader.gbid), NULL, 0), TypeDescriptorField("", DT_INT, offsetof(SetItemBonusTableEntry, tHeader.dwPad), NULL, 0),
TypeDescriptorField("", DT_INT, offsetof(SetItemBonusTableEntry, nRequired), NULL, 1),
FilterTypeDescriptorField("", DT_GBID, offsetof(SetItemBonusTableEntry, gbidSet), "", 33, 0, -1),
FixedArrayTypeDescriptorField("", DT_ATTRIBUTE_SPECIFIER, offsetof(SetItemBonusTableEntry, ptSetItemBonusAttributes), NULL, 1, 8, 0),
TypeDescriptorField(NULL, DT_NULL, sizeof(SetItemBonusTableEntry), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_SetItemBonusTableEntry("SetItemBonusTableEntry", 0, SetItemBonusTableEntry_Fields, 8, NULL, 0x0, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_SET_ITEM_BONUS_TABLE_ENTRY = &TypeDescriptor_SetItemBonusTableEntry;

static const TypeDescriptorField SetItemBonusTable_Fields[3] = {
TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(SetItemBonusTable, serSetItemBonuses), NULL, 0), VariableArrayTypeDescriptorField("", DT_SET_ITEM_BONUS_TABLE_ENTRY, offsetof(SetItemBonusTable, ptSetItemBonuses), 0x21, offsetof(SetItemBonusTable, serSetItemBonuses) - offsetof(SetItemBonusTable, ptSetItemBonuses)),
TypeDescriptorField(NULL, DT_NULL, sizeof(SetItemBonusTable), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_SetItemBonusTable("SetItemBonusTable", 0, SetItemBonusTable_Fields, 3, NULL, 0x0, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_SET_ITEM_BONUS_TABLE = &TypeDescriptor_SetItemBonusTable;




static const TypeDescriptorField MonsterAffixesEntry_Fields[197] = {
TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(MonsterAffixesEntry, tHeader), NULL, 7340032), VariableArrayTypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, tHeader.szName), 0x700020, offsetof(MonsterAffixesEntry, tHeader) - offsetof(MonsterAffixesEntry, tHeader.szName)), TypeDescriptorField("", DT_INT, offsetof(MonsterAffixesEntry, tHeader.gbid), NULL, 0), TypeDescriptorField("", DT_INT, offsetof(MonsterAffixesEntry, tHeader.dwPad), NULL, 0),

TypeDescriptorField("", DT_INT, offsetof(MonsterAffixesEntry, nFrequencyWeight), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(MonsterAffixesEntry, nAffixLevelMin), (const void *)1, 1),
TypeDescriptorField("", DT_INT, offsetof(MonsterAffixesEntry, nAffixLevelMax), (const void *)100, 1),
EnumTypeDescriptorField("", offsetof(MonsterAffixesEntry, eAffixType), (const void *)-1, gAffixTypes),
EnumTypeDescriptorField("", offsetof(MonsterAffixesEntry, eValidFor), NULL, gMonsterAffixValidFor),
EnumTypeDescriptorField("", offsetof(MonsterAffixesEntry, eBaseElement), (const void *)-1, gDamageTypes),
TypeDescriptorField("", DT_INT, offsetof(MonsterAffixesEntry, bAvailableForUniques), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(MonsterAffixesEntry, bShown), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(MonsterAffixesEntry, bDefensive), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(MonsterAffixesEntry, bCrowdControl), NULL, 1),

FilterTypeDescriptorField("", DT_SNO, offsetof(MonsterAffixesEntry, snoSpawnPower), "", 29, 0, -1),
FilterTypeDescriptorField("", DT_SNO, offsetof(MonsterAffixesEntry, snoSpawnPower[2]), "", 29, 0, -1),
FilterTypeDescriptorField("", DT_SNO, offsetof(MonsterAffixesEntry, snoSpawnPower[1]), "", 29, 0, -1),

TranslatableTypeDescriptorField("", offsetof(MonsterAffixesEntry, ptAttributes), (const void *)-1, XMLStringToAttrib, AttribToXMLString),
TypeDescriptorField("", DT_ATTRIBUTEPARAM, offsetof(MonsterAffixesEntry, ptAttributes[0].nParam), (const void *)-1, 1),
TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(MonsterAffixesEntry, ptAttributes[0].serAttributeFormula), NULL, 0), FormulaTypeDescriptorField("", offsetof(MonsterAffixesEntry, ptAttributes[0].pbAttributeFormula), offsetof(MonsterAffixesEntry, ptAttributes[0].serAttributeFormula) - offsetof(MonsterAffixesEntry, ptAttributes[0].pbAttributeFormula)),
TranslatableTypeDescriptorField("", offsetof(MonsterAffixesEntry, ptAttributes[1]), (const void *)-1, XMLStringToAttrib, AttribToXMLString),
TypeDescriptorField("", DT_ATTRIBUTEPARAM, offsetof(MonsterAffixesEntry, ptAttributes[1].nParam), (const void *)-1, 1),
TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(MonsterAffixesEntry, ptAttributes[1].serAttributeFormula), NULL, 0), FormulaTypeDescriptorField("", offsetof(MonsterAffixesEntry, ptAttributes[1].pbAttributeFormula), offsetof(MonsterAffixesEntry, ptAttributes[1].serAttributeFormula) - offsetof(MonsterAffixesEntry, ptAttributes[1].pbAttributeFormula)),
TranslatableTypeDescriptorField("", offsetof(MonsterAffixesEntry, ptAttributes[2]), (const void *)-1, XMLStringToAttrib, AttribToXMLString),
TypeDescriptorField("", DT_ATTRIBUTEPARAM, offsetof(MonsterAffixesEntry, ptAttributes[2].nParam), (const void *)-1, 1),
TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(MonsterAffixesEntry, ptAttributes[2].serAttributeFormula), NULL, 0), FormulaTypeDescriptorField("", offsetof(MonsterAffixesEntry, ptAttributes[2].pbAttributeFormula), offsetof(MonsterAffixesEntry, ptAttributes[2].serAttributeFormula) - offsetof(MonsterAffixesEntry, ptAttributes[2].pbAttributeFormula)),
TranslatableTypeDescriptorField("", offsetof(MonsterAffixesEntry, ptAttributes[3]), (const void *)-1, XMLStringToAttrib, AttribToXMLString),
TypeDescriptorField("", DT_ATTRIBUTEPARAM, offsetof(MonsterAffixesEntry, ptAttributes[3].nParam), (const void *)-1, 1),
TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(MonsterAffixesEntry, ptAttributes[3].serAttributeFormula), NULL, 0), FormulaTypeDescriptorField("", offsetof(MonsterAffixesEntry, ptAttributes[3].pbAttributeFormula), offsetof(MonsterAffixesEntry, ptAttributes[3].serAttributeFormula) - offsetof(MonsterAffixesEntry, ptAttributes[3].pbAttributeFormula)),
TranslatableTypeDescriptorField("", offsetof(MonsterAffixesEntry, ptAttributes[4]), (const void *)-1, XMLStringToAttrib, AttribToXMLString),
TypeDescriptorField("", DT_ATTRIBUTEPARAM, offsetof(MonsterAffixesEntry, ptAttributes[4].nParam), (const void *)-1, 1),
TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(MonsterAffixesEntry, ptAttributes[4].serAttributeFormula), NULL, 0), FormulaTypeDescriptorField("", offsetof(MonsterAffixesEntry, ptAttributes[4].pbAttributeFormula), offsetof(MonsterAffixesEntry, ptAttributes[4].serAttributeFormula) - offsetof(MonsterAffixesEntry, ptAttributes[4].pbAttributeFormula)),
TranslatableTypeDescriptorField("", offsetof(MonsterAffixesEntry, ptAttributes[5]), (const void *)-1, XMLStringToAttrib, AttribToXMLString),
TypeDescriptorField("", DT_ATTRIBUTEPARAM, offsetof(MonsterAffixesEntry, ptAttributes[5].nParam), (const void *)-1, 1),
TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(MonsterAffixesEntry, ptAttributes[5].serAttributeFormula), NULL, 0), FormulaTypeDescriptorField("", offsetof(MonsterAffixesEntry, ptAttributes[5].pbAttributeFormula), offsetof(MonsterAffixesEntry, ptAttributes[5].serAttributeFormula) - offsetof(MonsterAffixesEntry, ptAttributes[5].pbAttributeFormula)),
TranslatableTypeDescriptorField("", offsetof(MonsterAffixesEntry, ptAttributes[6]), (const void *)-1, XMLStringToAttrib, AttribToXMLString),
TypeDescriptorField("", DT_ATTRIBUTEPARAM, offsetof(MonsterAffixesEntry, ptAttributes[6].nParam), (const void *)-1, 1),
TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(MonsterAffixesEntry, ptAttributes[6].serAttributeFormula), NULL, 0), FormulaTypeDescriptorField("", offsetof(MonsterAffixesEntry, ptAttributes[6].pbAttributeFormula), offsetof(MonsterAffixesEntry, ptAttributes[6].serAttributeFormula) - offsetof(MonsterAffixesEntry, ptAttributes[6].pbAttributeFormula)),
TranslatableTypeDescriptorField("", offsetof(MonsterAffixesEntry, ptAttributes[7]), (const void *)-1, XMLStringToAttrib, AttribToXMLString),
TypeDescriptorField("", DT_ATTRIBUTEPARAM, offsetof(MonsterAffixesEntry, ptAttributes[7].nParam), (const void *)-1, 1),
TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(MonsterAffixesEntry, ptAttributes[7].serAttributeFormula), NULL, 0), FormulaTypeDescriptorField("", offsetof(MonsterAffixesEntry, ptAttributes[7].pbAttributeFormula), offsetof(MonsterAffixesEntry, ptAttributes[7].serAttributeFormula) - offsetof(MonsterAffixesEntry, ptAttributes[7].pbAttributeFormula)),
TranslatableTypeDescriptorField("", offsetof(MonsterAffixesEntry, ptAttributes[8]), (const void *)-1, XMLStringToAttrib, AttribToXMLString),
TypeDescriptorField("", DT_ATTRIBUTEPARAM, offsetof(MonsterAffixesEntry, ptAttributes[8].nParam), (const void *)-1, 1),
TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(MonsterAffixesEntry, ptAttributes[8].serAttributeFormula), NULL, 0), FormulaTypeDescriptorField("", offsetof(MonsterAffixesEntry, ptAttributes[8].pbAttributeFormula), offsetof(MonsterAffixesEntry, ptAttributes[8].serAttributeFormula) - offsetof(MonsterAffixesEntry, ptAttributes[8].pbAttributeFormula)),
TranslatableTypeDescriptorField("", offsetof(MonsterAffixesEntry, ptAttributes[9]), (const void *)-1, XMLStringToAttrib, AttribToXMLString),
TypeDescriptorField("", DT_ATTRIBUTEPARAM, offsetof(MonsterAffixesEntry, ptAttributes[9].nParam), (const void *)-1, 1),
TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(MonsterAffixesEntry, ptAttributes[9].serAttributeFormula), NULL, 0), FormulaTypeDescriptorField("", offsetof(MonsterAffixesEntry, ptAttributes[9].pbAttributeFormula), offsetof(MonsterAffixesEntry, ptAttributes[9].serAttributeFormula) - offsetof(MonsterAffixesEntry, ptAttributes[9].pbAttributeFormula)),

TranslatableTypeDescriptorField("", offsetof(MonsterAffixesEntry, ptMinionAttributes), (const void *)-1, XMLStringToAttrib, AttribToXMLString),
TypeDescriptorField("", DT_ATTRIBUTEPARAM, offsetof(MonsterAffixesEntry, ptMinionAttributes[0].nParam), (const void *)-1, 1),
TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(MonsterAffixesEntry, ptMinionAttributes[0].serAttributeFormula), NULL, 0), FormulaTypeDescriptorField("", offsetof(MonsterAffixesEntry, ptMinionAttributes[0].pbAttributeFormula), offsetof(MonsterAffixesEntry, ptMinionAttributes[0].serAttributeFormula) - offsetof(MonsterAffixesEntry, ptMinionAttributes[0].pbAttributeFormula)),
TranslatableTypeDescriptorField("", offsetof(MonsterAffixesEntry, ptMinionAttributes[1]), (const void *)-1, XMLStringToAttrib, AttribToXMLString),
TypeDescriptorField("", DT_ATTRIBUTEPARAM, offsetof(MonsterAffixesEntry, ptMinionAttributes[1].nParam), (const void *)-1, 1),
TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(MonsterAffixesEntry, ptMinionAttributes[1].serAttributeFormula), NULL, 0), FormulaTypeDescriptorField("", offsetof(MonsterAffixesEntry, ptMinionAttributes[1].pbAttributeFormula), offsetof(MonsterAffixesEntry, ptMinionAttributes[1].serAttributeFormula) - offsetof(MonsterAffixesEntry, ptMinionAttributes[1].pbAttributeFormula)),
TranslatableTypeDescriptorField("", offsetof(MonsterAffixesEntry, ptMinionAttributes[2]), (const void *)-1, XMLStringToAttrib, AttribToXMLString),
TypeDescriptorField("", DT_ATTRIBUTEPARAM, offsetof(MonsterAffixesEntry, ptMinionAttributes[2].nParam), (const void *)-1, 1),
TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(MonsterAffixesEntry, ptMinionAttributes[2].serAttributeFormula), NULL, 0), FormulaTypeDescriptorField("", offsetof(MonsterAffixesEntry, ptMinionAttributes[2].pbAttributeFormula), offsetof(MonsterAffixesEntry, ptMinionAttributes[2].serAttributeFormula) - offsetof(MonsterAffixesEntry, ptMinionAttributes[2].pbAttributeFormula)),
TranslatableTypeDescriptorField("", offsetof(MonsterAffixesEntry, ptMinionAttributes[3]), (const void *)-1, XMLStringToAttrib, AttribToXMLString),
TypeDescriptorField("", DT_ATTRIBUTEPARAM, offsetof(MonsterAffixesEntry, ptMinionAttributes[3].nParam), (const void *)-1, 1),
TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(MonsterAffixesEntry, ptMinionAttributes[3].serAttributeFormula), NULL, 0), FormulaTypeDescriptorField("", offsetof(MonsterAffixesEntry, ptMinionAttributes[3].pbAttributeFormula), offsetof(MonsterAffixesEntry, ptMinionAttributes[3].serAttributeFormula) - offsetof(MonsterAffixesEntry, ptMinionAttributes[3].pbAttributeFormula)),
TranslatableTypeDescriptorField("", offsetof(MonsterAffixesEntry, ptMinionAttributes[4]), (const void *)-1, XMLStringToAttrib, AttribToXMLString),
TypeDescriptorField("", DT_ATTRIBUTEPARAM, offsetof(MonsterAffixesEntry, ptMinionAttributes[4].nParam), (const void *)-1, 1),
TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(MonsterAffixesEntry, ptMinionAttributes[4].serAttributeFormula), NULL, 0), FormulaTypeDescriptorField("", offsetof(MonsterAffixesEntry, ptMinionAttributes[4].pbAttributeFormula), offsetof(MonsterAffixesEntry, ptMinionAttributes[4].serAttributeFormula) - offsetof(MonsterAffixesEntry, ptMinionAttributes[4].pbAttributeFormula)),
TranslatableTypeDescriptorField("", offsetof(MonsterAffixesEntry, ptMinionAttributes[5]), (const void *)-1, XMLStringToAttrib, AttribToXMLString),
TypeDescriptorField("", DT_ATTRIBUTEPARAM, offsetof(MonsterAffixesEntry, ptMinionAttributes[5].nParam), (const void *)-1, 1),
TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(MonsterAffixesEntry, ptMinionAttributes[5].serAttributeFormula), NULL, 0), FormulaTypeDescriptorField("", offsetof(MonsterAffixesEntry, ptMinionAttributes[5].pbAttributeFormula), offsetof(MonsterAffixesEntry, ptMinionAttributes[5].serAttributeFormula) - offsetof(MonsterAffixesEntry, ptMinionAttributes[5].pbAttributeFormula)),
TranslatableTypeDescriptorField("", offsetof(MonsterAffixesEntry, ptMinionAttributes[6]), (const void *)-1, XMLStringToAttrib, AttribToXMLString),
TypeDescriptorField("", DT_ATTRIBUTEPARAM, offsetof(MonsterAffixesEntry, ptMinionAttributes[6].nParam), (const void *)-1, 1),
TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(MonsterAffixesEntry, ptMinionAttributes[6].serAttributeFormula), NULL, 0), FormulaTypeDescriptorField("", offsetof(MonsterAffixesEntry, ptMinionAttributes[6].pbAttributeFormula), offsetof(MonsterAffixesEntry, ptMinionAttributes[6].serAttributeFormula) - offsetof(MonsterAffixesEntry, ptMinionAttributes[6].pbAttributeFormula)),
TranslatableTypeDescriptorField("", offsetof(MonsterAffixesEntry, ptMinionAttributes[7]), (const void *)-1, XMLStringToAttrib, AttribToXMLString),
TypeDescriptorField("", DT_ATTRIBUTEPARAM, offsetof(MonsterAffixesEntry, ptMinionAttributes[7].nParam), (const void *)-1, 1),
TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(MonsterAffixesEntry, ptMinionAttributes[7].serAttributeFormula), NULL, 0), FormulaTypeDescriptorField("", offsetof(MonsterAffixesEntry, ptMinionAttributes[7].pbAttributeFormula), offsetof(MonsterAffixesEntry, ptMinionAttributes[7].serAttributeFormula) - offsetof(MonsterAffixesEntry, ptMinionAttributes[7].pbAttributeFormula)),
TranslatableTypeDescriptorField("", offsetof(MonsterAffixesEntry, ptMinionAttributes[8]), (const void *)-1, XMLStringToAttrib, AttribToXMLString),
TypeDescriptorField("", DT_ATTRIBUTEPARAM, offsetof(MonsterAffixesEntry, ptMinionAttributes[8].nParam), (const void *)-1, 1),
TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(MonsterAffixesEntry, ptMinionAttributes[8].serAttributeFormula), NULL, 0), FormulaTypeDescriptorField("", offsetof(MonsterAffixesEntry, ptMinionAttributes[8].pbAttributeFormula), offsetof(MonsterAffixesEntry, ptMinionAttributes[8].serAttributeFormula) - offsetof(MonsterAffixesEntry, ptMinionAttributes[8].pbAttributeFormula)),
TranslatableTypeDescriptorField("", offsetof(MonsterAffixesEntry, ptMinionAttributes[9]), (const void *)-1, XMLStringToAttrib, AttribToXMLString),
TypeDescriptorField("", DT_ATTRIBUTEPARAM, offsetof(MonsterAffixesEntry, ptMinionAttributes[9].nParam), (const void *)-1, 1),
TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(MonsterAffixesEntry, ptMinionAttributes[9].serAttributeFormula), NULL, 0), FormulaTypeDescriptorField("", offsetof(MonsterAffixesEntry, ptMinionAttributes[9].pbAttributeFormula), offsetof(MonsterAffixesEntry, ptMinionAttributes[9].serAttributeFormula) - offsetof(MonsterAffixesEntry, ptMinionAttributes[9].pbAttributeFormula)),

TypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, arAllowedByLevel), NULL, 1),
TypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, arAllowedByLevel[1]), NULL, 1),
TypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, arAllowedByLevel[2]), NULL, 1),
TypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, arAllowedByLevel[3]), NULL, 1),
TypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, arAllowedByLevel[4]), NULL, 1),
TypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, arAllowedByLevel[5]), NULL, 1),
TypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, arAllowedByLevel[6]), NULL, 1),
TypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, arAllowedByLevel[7]), NULL, 1),
TypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, arAllowedByLevel[8]), NULL, 1),
TypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, arAllowedByLevel[9]), NULL, 1),
TypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, arAllowedByLevel[10]), NULL, 1),
TypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, arAllowedByLevel[11]), NULL, 1),
TypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, arAllowedByLevel[12]), NULL, 1),
TypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, arAllowedByLevel[13]), NULL, 1),
TypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, arAllowedByLevel[14]), NULL, 1),
TypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, arAllowedByLevel[15]), NULL, 1),
TypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, arAllowedByLevel[16]), NULL, 1),
TypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, arAllowedByLevel[17]), NULL, 1),
TypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, arAllowedByLevel[18]), NULL, 1),
TypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, arAllowedByLevel[19]), NULL, 1),
TypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, arAllowedByLevel[20]), NULL, 1),
TypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, arAllowedByLevel[21]), NULL, 1),
TypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, arAllowedByLevel[22]), NULL, 1),
TypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, arAllowedByLevel[23]), NULL, 1),
TypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, arAllowedByLevel[24]), NULL, 1),
TypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, arAllowedByLevel[25]), NULL, 1),
TypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, arAllowedByLevel[26]), NULL, 1),
TypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, arAllowedByLevel[27]), NULL, 1),
TypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, arAllowedByLevel[28]), NULL, 1),
TypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, arAllowedByLevel[29]), NULL, 1),
TypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, arAllowedByLevel[30]), NULL, 1),
TypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, arAllowedByLevel[31]), NULL, 1),
TypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, arAllowedByLevel[32]), NULL, 1),
TypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, arAllowedByLevel[33]), NULL, 1),
TypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, arAllowedByLevel[34]), NULL, 1),
TypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, arAllowedByLevel[35]), NULL, 1),
TypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, arAllowedByLevel[36]), NULL, 1),
TypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, arAllowedByLevel[37]), NULL, 1),
TypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, arAllowedByLevel[38]), NULL, 1),
TypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, arAllowedByLevel[39]), NULL, 1),
TypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, arAllowedByLevel[40]), NULL, 1),
TypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, arAllowedByLevel[41]), NULL, 1),
TypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, arAllowedByLevel[42]), NULL, 1),
TypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, arAllowedByLevel[43]), NULL, 1),
TypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, arAllowedByLevel[44]), NULL, 1),
TypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, arAllowedByLevel[45]), NULL, 1),
TypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, arAllowedByLevel[46]), NULL, 1),
TypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, arAllowedByLevel[47]), NULL, 1),
TypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, arAllowedByLevel[48]), NULL, 1),
TypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, arAllowedByLevel[49]), NULL, 1),
TypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, arAllowedByLevel[50]), NULL, 1),
TypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, arAllowedByLevel[51]), NULL, 1),
TypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, arAllowedByLevel[52]), NULL, 1),
TypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, arAllowedByLevel[53]), NULL, 1),
TypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, arAllowedByLevel[54]), NULL, 1),
TypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, arAllowedByLevel[55]), NULL, 1),
TypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, arAllowedByLevel[56]), NULL, 1),
TypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, arAllowedByLevel[57]), NULL, 1),
TypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, arAllowedByLevel[58]), NULL, 1),
TypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, arAllowedByLevel[59]), NULL, 1),
TypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, arAllowedByLevel[60]), NULL, 1),
TypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, arAllowedByLevel[61]), NULL, 1),
TypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, arAllowedByLevel[62]), NULL, 1),
TypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, arAllowedByLevel[63]), NULL, 1),
TypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, arAllowedByLevel[64]), NULL, 1),
TypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, arAllowedByLevel[65]), NULL, 1),
TypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, arAllowedByLevel[66]), NULL, 1),
TypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, arAllowedByLevel[67]), NULL, 1),
TypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, arAllowedByLevel[68]), NULL, 1),
TypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, arAllowedByLevel[69]), NULL, 1),
TypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, arAllowedByLevel[70]), NULL, 1),
TypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, arAllowedByLevel[71]), NULL, 1),
TypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, arAllowedByLevel[72]), NULL, 1),
TypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, arAllowedByLevel[73]), NULL, 1),
TypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, arAllowedByLevel[74]), NULL, 1),
TypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, arAllowedByLevel[75]), NULL, 1),
TypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, arAllowedByLevel[76]), NULL, 1),
TypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, arAllowedByLevel[77]), NULL, 1),
TypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, arAllowedByLevel[78]), NULL, 1),
TypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, arAllowedByLevel[79]), NULL, 1),
TypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, arAllowedByLevel[80]), NULL, 1),
TypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, arAllowedByLevel[81]), NULL, 1),
TypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, arAllowedByLevel[82]), NULL, 1),
TypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, arAllowedByLevel[83]), NULL, 1),
TypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, arAllowedByLevel[84]), NULL, 1),
TypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, arAllowedByLevel[85]), NULL, 1),
TypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, arAllowedByLevel[86]), NULL, 1),
TypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, arAllowedByLevel[87]), NULL, 1),
TypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, arAllowedByLevel[88]), NULL, 1),
TypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, arAllowedByLevel[89]), NULL, 1),
TypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, arAllowedByLevel[90]), NULL, 1),
TypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, arAllowedByLevel[91]), NULL, 1),
TypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, arAllowedByLevel[92]), NULL, 1),
TypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, arAllowedByLevel[93]), NULL, 1),
TypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, arAllowedByLevel[94]), NULL, 1),
TypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, arAllowedByLevel[95]), NULL, 1),
TypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, arAllowedByLevel[96]), NULL, 1),
TypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, arAllowedByLevel[97]), NULL, 1),
TypeDescriptorField("", DT_BYTE, offsetof(MonsterAffixesEntry, arAllowedByLevel[98]), NULL, 1),
TypeDescriptorField(NULL, DT_NULL, sizeof(MonsterAffixesEntry), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_MonsterAffixesEntry("MonsterAffixesEntry", 0, MonsterAffixesEntry_Fields, 197, NULL, 0x0, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_MONSTER_AFFIXES_ENTRY = &TypeDescriptor_MonsterAffixesEntry;

static const TypeDescriptorField MonsterAffixesTable_Fields[3] = {
TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(MonsterAffixesTable, serMonsterAffixes), NULL, 0), VariableArrayTypeDescriptorField("", DT_MONSTER_AFFIXES_ENTRY, offsetof(MonsterAffixesTable, ptMonsterAffixes), 0x21, offsetof(MonsterAffixesTable, serMonsterAffixes) - offsetof(MonsterAffixesTable, ptMonsterAffixes)),
TypeDescriptorField(NULL, DT_NULL, sizeof(MonsterAffixesTable), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_MonsterAffixesTable("MonsterAffixesTable", 0, MonsterAffixesTable_Fields, 3, NULL, 0x0, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_MONSTER_AFFIXES_TABLE = &TypeDescriptor_MonsterAffixesTable;

static const TypeDescriptorField RareMonsterNamesEntry_Fields[8] = {
TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(RareMonsterNamesEntry, tHeader), NULL, 7340032), VariableArrayTypeDescriptorField("", DT_BYTE, offsetof(RareMonsterNamesEntry, tHeader.szName), 0x700020, offsetof(RareMonsterNamesEntry, tHeader) - offsetof(RareMonsterNamesEntry, tHeader.szName)), TypeDescriptorField("", DT_INT, offsetof(RareMonsterNamesEntry, tHeader.gbid), NULL, 0), TypeDescriptorField("", DT_INT, offsetof(RareMonsterNamesEntry, tHeader.dwPad), NULL, 0),
EnumTypeDescriptorField("", offsetof(RareMonsterNamesEntry, eAffixType), (const void *)-1, gAffixTypes),
CharArrayTypeDescriptorField("", offsetof(RareMonsterNamesEntry, szAffixEffect), "", 1, 128, -1),
TypeDescriptorField("", DT_INT, offsetof(RareMonsterNamesEntry, dwPad), NULL, 0),
TypeDescriptorField(NULL, DT_NULL, sizeof(RareMonsterNamesEntry), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_RareMonsterNamesEntry("RareMonsterNamesEntry", 0, RareMonsterNamesEntry_Fields, 8, NULL, 0x0, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_RARE_MONSTER_NAMES_ENTRY = &TypeDescriptor_RareMonsterNamesEntry;

static const TypeDescriptorField RareMonsterNamesTable_Fields[3] = {
TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(RareMonsterNamesTable, serRareMonsterNames), NULL, 0), VariableArrayTypeDescriptorField("", DT_RARE_MONSTER_NAMES_ENTRY, offsetof(RareMonsterNamesTable, ptRareMonsterNames), 0x21, offsetof(RareMonsterNamesTable, serRareMonsterNames) - offsetof(RareMonsterNamesTable, ptRareMonsterNames)),
TypeDescriptorField(NULL, DT_NULL, sizeof(RareMonsterNamesTable), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_RareMonsterNamesTable("RareMonsterNamesTable", 0, RareMonsterNamesTable_Fields, 3, NULL, 0x0, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_RARE_MONSTER_NAMES_TABLE = &TypeDescriptor_RareMonsterNamesTable;

static const TypeDescriptorField RareItemNamesEntry_Fields[9] = {
TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(RareItemNamesEntry, tHeader), NULL, 7340032), VariableArrayTypeDescriptorField("", DT_BYTE, offsetof(RareItemNamesEntry, tHeader.szName), 0x700020, offsetof(RareItemNamesEntry, tHeader) - offsetof(RareItemNamesEntry, tHeader.szName)), TypeDescriptorField("", DT_INT, offsetof(RareItemNamesEntry, tHeader.gbid), NULL, 0), TypeDescriptorField("", DT_INT, offsetof(RareItemNamesEntry, tHeader.dwPad), NULL, 0),
FilterTypeDescriptorField("", DT_GBID, offsetof(RareItemNamesEntry, hgbRelatedAffixOrItemType.gbid), "", 4294967295, 0, -1),
EnumTypeDescriptorField("", offsetof(RareItemNamesEntry, hgbRelatedAffixOrItemType), (const void *)-1, gGameBalanceTypes),
EnumTypeDescriptorField("", offsetof(RareItemNamesEntry, eAffixType), (const void *)-1, gAffixTypes),
TypeDescriptorField("", DT_INT, offsetof(RareItemNamesEntry, dwPad), NULL, 0),
TypeDescriptorField(NULL, DT_NULL, sizeof(RareItemNamesEntry), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_RareItemNamesEntry("RareItemNamesEntry", 0, RareItemNamesEntry_Fields, 9, NULL, 0x0, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_RARE_ITEM_NAMES_ENTRY = &TypeDescriptor_RareItemNamesEntry;

static const TypeDescriptorField RareItemNamesTable_Fields[3] = {
TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(RareItemNamesTable, serRareItemNames), NULL, 0), VariableArrayTypeDescriptorField("", DT_RARE_ITEM_NAMES_ENTRY, offsetof(RareItemNamesTable, ptRareItemNames), 0x21, offsetof(RareItemNamesTable, serRareItemNames) - offsetof(RareItemNamesTable, ptRareItemNames)),
TypeDescriptorField(NULL, DT_NULL, sizeof(RareItemNamesTable), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_RareItemNamesTable("RareItemNamesTable", 0, RareItemNamesTable_Fields, 3, NULL, 0x0, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_RARE_ITEM_NAMES_TABLE = &TypeDescriptor_RareItemNamesTable;




static const TypeDescriptorField SocketedEffectsTableEntry_Fields[28] = {
TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(SocketedEffectsTableEntry, tHeader), NULL, 7340032), VariableArrayTypeDescriptorField("", DT_BYTE, offsetof(SocketedEffectsTableEntry, tHeader.szName), 0x700020, offsetof(SocketedEffectsTableEntry, tHeader) - offsetof(SocketedEffectsTableEntry, tHeader.szName)), TypeDescriptorField("", DT_INT, offsetof(SocketedEffectsTableEntry, tHeader.gbid), NULL, 0), TypeDescriptorField("", DT_INT, offsetof(SocketedEffectsTableEntry, tHeader.dwPad), NULL, 0),

FilterTypeDescriptorField("", DT_GBID, offsetof(SocketedEffectsTableEntry, gbidItemBeingSocketed), "", 2, 0, -1),
FilterTypeDescriptorField("", DT_GBID, offsetof(SocketedEffectsTableEntry, gbidItemTypeSocketedInto), "", 1, 0, -1),

TranslatableTypeDescriptorField("", offsetof(SocketedEffectsTableEntry, ptAttributes), (const void *)-1, XMLStringToAttrib, AttribToXMLString),
TypeDescriptorField("", DT_ATTRIBUTEPARAM, offsetof(SocketedEffectsTableEntry, ptAttributes[0].nParam), (const void *)-1, 1),
TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(SocketedEffectsTableEntry, ptAttributes[0].serAttributeFormula), NULL, 0), FormulaTypeDescriptorField("", offsetof(SocketedEffectsTableEntry, ptAttributes[0].pbAttributeFormula), offsetof(SocketedEffectsTableEntry, ptAttributes[0].serAttributeFormula) - offsetof(SocketedEffectsTableEntry, ptAttributes[0].pbAttributeFormula)),
TranslatableTypeDescriptorField("", offsetof(SocketedEffectsTableEntry, ptAttributes[1]), (const void *)-1, XMLStringToAttrib, AttribToXMLString),
TypeDescriptorField("", DT_ATTRIBUTEPARAM, offsetof(SocketedEffectsTableEntry, ptAttributes[1].nParam), (const void *)-1, 1),
TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(SocketedEffectsTableEntry, ptAttributes[1].serAttributeFormula), NULL, 0), FormulaTypeDescriptorField("", offsetof(SocketedEffectsTableEntry, ptAttributes[1].pbAttributeFormula), offsetof(SocketedEffectsTableEntry, ptAttributes[1].serAttributeFormula) - offsetof(SocketedEffectsTableEntry, ptAttributes[1].pbAttributeFormula)),
TranslatableTypeDescriptorField("", offsetof(SocketedEffectsTableEntry, ptAttributes[2]), (const void *)-1, XMLStringToAttrib, AttribToXMLString),
TypeDescriptorField("", DT_ATTRIBUTEPARAM, offsetof(SocketedEffectsTableEntry, ptAttributes[2].nParam), (const void *)-1, 1),
TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(SocketedEffectsTableEntry, ptAttributes[2].serAttributeFormula), NULL, 0), FormulaTypeDescriptorField("", offsetof(SocketedEffectsTableEntry, ptAttributes[2].pbAttributeFormula), offsetof(SocketedEffectsTableEntry, ptAttributes[2].serAttributeFormula) - offsetof(SocketedEffectsTableEntry, ptAttributes[2].pbAttributeFormula)),

TranslatableTypeDescriptorField("", offsetof(SocketedEffectsTableEntry, ptRequirements), (const void *)-1, XMLStringToAttrib, AttribToXMLString),
TypeDescriptorField("", DT_ATTRIBUTEPARAM, offsetof(SocketedEffectsTableEntry, ptRequirements[0].nParam), (const void *)-1, 1),
TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(SocketedEffectsTableEntry, ptRequirements[0].serAttributeFormula), NULL, 0), FormulaTypeDescriptorField("", offsetof(SocketedEffectsTableEntry, ptRequirements[0].pbAttributeFormula), offsetof(SocketedEffectsTableEntry, ptRequirements[0].serAttributeFormula) - offsetof(SocketedEffectsTableEntry, ptRequirements[0].pbAttributeFormula)),
TranslatableTypeDescriptorField("", offsetof(SocketedEffectsTableEntry, ptRequirements[1]), (const void *)-1, XMLStringToAttrib, AttribToXMLString),
TypeDescriptorField("", DT_ATTRIBUTEPARAM, offsetof(SocketedEffectsTableEntry, ptRequirements[1].nParam), (const void *)-1, 1),
TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(SocketedEffectsTableEntry, ptRequirements[1].serAttributeFormula), NULL, 0), FormulaTypeDescriptorField("", offsetof(SocketedEffectsTableEntry, ptRequirements[1].pbAttributeFormula), offsetof(SocketedEffectsTableEntry, ptRequirements[1].serAttributeFormula) - offsetof(SocketedEffectsTableEntry, ptRequirements[1].pbAttributeFormula)),

CharArrayTypeDescriptorField("", offsetof(SocketedEffectsTableEntry, szStringlistLabel), "NONE", 1, 1024, -1),
TypeDescriptorField(NULL, DT_NULL, sizeof(SocketedEffectsTableEntry), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_SocketedEffectsTableEntry("SocketedEffectsTableEntry", 0, SocketedEffectsTableEntry_Fields, 28, NULL, 0x0, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_SOCKETED_EFFECTS_TABLE_ENTRY = &TypeDescriptor_SocketedEffectsTableEntry;

static const TypeDescriptorField SocketedEffectsTable_Fields[3] = {
TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(SocketedEffectsTable, serSocketedEffects), NULL, 0), VariableArrayTypeDescriptorField("", DT_SOCKETED_EFFECTS_TABLE_ENTRY, offsetof(SocketedEffectsTable, ptSocketedEffects), 0x21, offsetof(SocketedEffectsTable, serSocketedEffects) - offsetof(SocketedEffectsTable, ptSocketedEffects)),
TypeDescriptorField(NULL, DT_NULL, sizeof(SocketedEffectsTable), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_SocketedEffectsTable("SocketedEffectsTable", 0, SocketedEffectsTable_Fields, 3, NULL, 0x0, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_SOCKETED_EFFECTS_TABLE = &TypeDescriptor_SocketedEffectsTable;

static const TypeDescriptorField ItemDropTableEntry_Fields[224] = {
TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(ItemDropTableEntry, tHeader), NULL, 7340032), VariableArrayTypeDescriptorField("", DT_BYTE, offsetof(ItemDropTableEntry, tHeader.szName), 0x700020, offsetof(ItemDropTableEntry, tHeader) - offsetof(ItemDropTableEntry, tHeader.szName)), TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, tHeader.gbid), NULL, 0), TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, tHeader.dwPad), NULL, 0),

TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[1]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[2]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[3]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[4]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[5]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[6]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[7]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[8]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[9]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[10]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[11]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[12]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[13]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[14]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[15]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[16]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[17]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[18]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[19]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[20]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[21]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[22]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[23]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[24]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[25]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[26]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[27]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[28]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[29]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[30]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[31]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[32]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[33]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[34]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[35]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[36]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[37]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[38]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[39]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[40]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[41]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[42]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[43]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[44]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[45]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[46]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[47]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[48]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[49]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[50]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[51]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[52]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[53]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[54]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[55]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[56]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[57]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[58]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[59]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[60]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[61]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[62]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[63]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[64]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[65]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[66]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[67]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[68]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[69]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[70]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[71]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[72]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[73]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[74]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[75]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[76]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[77]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[78]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[79]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[80]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[81]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[82]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[83]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[84]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[85]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[86]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[87]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[88]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[89]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[90]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[91]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[92]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[93]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[94]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[95]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[96]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[97]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[98]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[99]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[100]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[101]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[102]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[103]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[104]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[105]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[106]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[107]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[108]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnDropWeights[109]), NULL, 1),


TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[1]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[2]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[3]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[4]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[5]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[6]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[7]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[8]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[9]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[10]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[11]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[12]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[13]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[14]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[15]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[16]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[17]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[18]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[19]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[20]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[21]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[22]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[23]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[24]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[25]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[26]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[27]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[28]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[29]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[30]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[31]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[32]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[33]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[34]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[35]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[36]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[37]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[38]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[39]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[40]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[41]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[42]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[43]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[44]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[45]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[46]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[47]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[48]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[49]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[50]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[51]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[52]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[53]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[54]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[55]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[56]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[57]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[58]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[59]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[60]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[61]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[62]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[63]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[64]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[65]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[66]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[67]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[68]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[69]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[70]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[71]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[72]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[73]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[74]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[75]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[76]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[77]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[78]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[79]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[81]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[82]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[83]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[84]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[85]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[86]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[87]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[88]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[89]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[90]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[91]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[92]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[93]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[94]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[95]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[96]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[97]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[98]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[99]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[100]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[101]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[102]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[103]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[104]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[105]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[106]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[107]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[108]), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(ItemDropTableEntry, pnVendorDropWeights[109]), NULL, 1),
TypeDescriptorField(NULL, DT_NULL, sizeof(ItemDropTableEntry), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_ItemDropTableEntry("ItemDropTableEntry", 0, ItemDropTableEntry_Fields, 224, NULL, 0x0, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_ITEM_DROP_TABLE_ENTRY = &TypeDescriptor_ItemDropTableEntry;

static const TypeDescriptorField ItemDropTable_Fields[3] = {
TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(ItemDropTable, serItemDrops), NULL, 0), VariableArrayTypeDescriptorField("", DT_ITEM_DROP_TABLE_ENTRY, offsetof(ItemDropTable, ptItemDrops), 0x21, offsetof(ItemDropTable, serItemDrops) - offsetof(ItemDropTable, ptItemDrops)),
TypeDescriptorField(NULL, DT_NULL, sizeof(ItemDropTable), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_ItemDropTable("ItemDropTable", 0, ItemDropTable_Fields, 3, NULL, 0x0, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_ITEM_DROP_TABLE = &TypeDescriptor_ItemDropTable;




static const TypeDescriptorField HirelingEntry_Fields[21] = {
TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(HirelingEntry, tHeader), NULL, 7340032), VariableArrayTypeDescriptorField("", DT_BYTE, offsetof(HirelingEntry, tHeader.szName), 0x700020, offsetof(HirelingEntry, tHeader) - offsetof(HirelingEntry, tHeader.szName)), TypeDescriptorField("", DT_INT, offsetof(HirelingEntry, tHeader.gbid), NULL, 0), TypeDescriptorField("", DT_INT, offsetof(HirelingEntry, tHeader.dwPad), NULL, 0),
FilterTypeDescriptorField("", DT_SNO, offsetof(HirelingEntry, snoActor), "", 1, 0, -1),
FilterTypeDescriptorField("", DT_SNO, offsetof(HirelingEntry, snoProxyActor), "", 1, 0, -1),
FilterTypeDescriptorField("", DT_SNO, offsetof(HirelingEntry, snoSummonedActor), "", 1, 0, -1),
FilterTypeDescriptorField("", DT_SNO, offsetof(HirelingEntry, snoInventory), "", 52, 0, -1),
EnumTypeDescriptorField("", offsetof(HirelingEntry, ePrimaryDamageAttribute), (const void *)-1, gPrimaryDamageAttributes),

TypeDescriptorField("", DT_FLOAT, offsetof(HirelingEntry, flAttributesStrength), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(HirelingEntry, flAttributesDexterity), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(HirelingEntry, flAttributesIntelligence), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(HirelingEntry, flAttributesVitality), NULL, 1),

TypeDescriptorField("", DT_FLOAT, offsetof(HirelingEntry, flAttributesCritDamageCap), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(HirelingEntry, flAttributesCritPercentBase), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(HirelingEntry, flAttributesCritPercentCap), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(HirelingEntry, flAttributesDodgeRatingBase), NULL, 1),

TypeDescriptorField("", DT_FLOAT, offsetof(HirelingEntry, flAttributesHirelingLightLookLevel), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(HirelingEntry, flAttributesHirelingMediumLookLevel), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(HirelingEntry, flAttributesHirelingHeavyLookLevel), NULL, 1),
TypeDescriptorField(NULL, DT_NULL, sizeof(HirelingEntry), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_HirelingEntry("HirelingEntry", 0, HirelingEntry_Fields, 21, NULL, 0x0, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_HIRELING_ENTRY = &TypeDescriptor_HirelingEntry;

static const TypeDescriptorField HirelingTable_Fields[3] = {
TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(HirelingTable, serHirelings), NULL, 0), VariableArrayTypeDescriptorField("", DT_HIRELING_ENTRY, offsetof(HirelingTable, ptHirelings), 0x21, offsetof(HirelingTable, serHirelings) - offsetof(HirelingTable, ptHirelings)),
TypeDescriptorField(NULL, DT_NULL, sizeof(HirelingTable), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_HirelingTable("Hirelings", 0, HirelingTable_Fields, 3, NULL, 0x0, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_HIRELINGTABLE = &TypeDescriptor_HirelingTable;




static const TypeDescriptorField EliteModifierData_Fields[27] = {
TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(EliteModifierData, tHeader), NULL, 7340032), VariableArrayTypeDescriptorField("", DT_BYTE, offsetof(EliteModifierData, tHeader.szName), 0x700020, offsetof(EliteModifierData, tHeader) - offsetof(EliteModifierData, tHeader.szName)), TypeDescriptorField("", DT_INT, offsetof(EliteModifierData, tHeader.gbid), NULL, 0), TypeDescriptorField("", DT_INT, offsetof(EliteModifierData, tHeader.dwPad), NULL, 0),
TypeDescriptorField("", DT_FLOAT, offsetof(EliteModifierData, flStunDR), NULL, 1),
TypeDescriptorField("", DT_TIME, offsetof(EliteModifierData, nStunDM), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(EliteModifierData, flCharmDR), NULL, 1),
TypeDescriptorField("", DT_TIME, offsetof(EliteModifierData, nCharmDM), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(EliteModifierData, flConsumptionDR), NULL, 1),
TypeDescriptorField("", DT_TIME, offsetof(EliteModifierData, nConsumptionDM), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(EliteModifierData, flSessileDR), NULL, 1),
TypeDescriptorField("", DT_TIME, offsetof(EliteModifierData, nSessileDM), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(EliteModifierData, flFearDR), NULL, 1),
TypeDescriptorField("", DT_TIME, offsetof(EliteModifierData, nFearDM), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(EliteModifierData, flRootDR), NULL, 1),
TypeDescriptorField("", DT_TIME, offsetof(EliteModifierData, nRootDM), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(EliteModifierData, flMovementSlowDR), NULL, 1),
TypeDescriptorField("", DT_TIME, offsetof(EliteModifierData, nMovementSlowDM), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(EliteModifierData, flMovementSlowER), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(EliteModifierData, flAttackSpeedDR), NULL, 1),
TypeDescriptorField("", DT_TIME, offsetof(EliteModifierData, nAttackSpeedDM), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(EliteModifierData, flAttackSpeedER), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(EliteModifierData, flKnockbackResist), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(EliteModifierData, flKnockbackER), NULL, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(EliteModifierData, flKnockbackMin), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(EliteModifierData, dwPad), NULL, 0),
TypeDescriptorField(NULL, DT_NULL, sizeof(EliteModifierData), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_EliteModifierData("EliteModifierData", 0, EliteModifierData_Fields, 27, NULL, 0x0, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_ELITEMODIFIER_DATA = &TypeDescriptor_EliteModifierData;

static const TypeDescriptorField EliteModifierTable_Fields[3] = {
TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(EliteModifierTable, serMods), NULL, 0), VariableArrayTypeDescriptorField("", DT_ELITEMODIFIER_DATA, offsetof(EliteModifierTable, ptMods), 0x21, offsetof(EliteModifierTable, serMods) - offsetof(EliteModifierTable, ptMods)),
TypeDescriptorField(NULL, DT_NULL, sizeof(EliteModifierTable), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_EliteModifierTable("EliteModifiers", 0, EliteModifierTable_Fields, 3, NULL, 0x0, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_ELITEMODIFIER_TABLE = &TypeDescriptor_EliteModifierTable;




static const TypeDescriptorField RecipeEntry_Fields[12] = {
TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(RecipeEntry, tHeader), NULL, 7340032), VariableArrayTypeDescriptorField("", DT_BYTE, offsetof(RecipeEntry, tHeader.szName), 0x700020, offsetof(RecipeEntry, tHeader) - offsetof(RecipeEntry, tHeader.szName)), TypeDescriptorField("", DT_INT, offsetof(RecipeEntry, tHeader.gbid), NULL, 0), TypeDescriptorField("", DT_INT, offsetof(RecipeEntry, tHeader.dwPad), NULL, 0),
FilterTypeDescriptorField("", DT_SNO, offsetof(RecipeEntry, snoRecipe), "", 49, 0, -1),
EnumTypeDescriptorField("", offsetof(RecipeEntry, eCrafterType), NULL, gCrafterTypes),
TypeDescriptorField("", DT_INT, offsetof(RecipeEntry, dwFlags), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(RecipeEntry, nLevel), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(RecipeEntry, nCost), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(RecipeEntry, nNumIngredients), NULL, 1),
FixedArrayTypeDescriptorField("", DT_RECIPE_INGREDIENT, offsetof(RecipeEntry, tIngredient), NULL, 1, 6, 0),
TypeDescriptorField(NULL, DT_NULL, sizeof(RecipeEntry), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_RecipeEntry("RecipeEntry", 0, RecipeEntry_Fields, 12, NULL, 0x0, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_RECIPE_ENTRY = &TypeDescriptor_RecipeEntry;

static const TypeDescriptorField RecipesTable_Fields[3] = {
TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(RecipesTable, serRecipes), NULL, 0), VariableArrayTypeDescriptorField("", DT_RECIPE_ENTRY, offsetof(RecipesTable, ptRecipes), 0x21, offsetof(RecipesTable, serRecipes) - offsetof(RecipesTable, ptRecipes)),
TypeDescriptorField(NULL, DT_NULL, sizeof(RecipesTable), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_RecipesTable("RecipesTable", 0, RecipesTable_Fields, 3, NULL, 0x0, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_RECIPE_TABLE = &TypeDescriptor_RecipesTable;




static const TypeDescriptorField ScriptedAchievementEvent_Fields[5] = {
TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(ScriptedAchievementEvent, tHeader), NULL, 7340032), VariableArrayTypeDescriptorField("", DT_BYTE, offsetof(ScriptedAchievementEvent, tHeader.szName), 0x700020, offsetof(ScriptedAchievementEvent, tHeader) - offsetof(ScriptedAchievementEvent, tHeader.szName)), TypeDescriptorField("", DT_INT, offsetof(ScriptedAchievementEvent, tHeader.gbid), NULL, 0), TypeDescriptorField("", DT_INT, offsetof(ScriptedAchievementEvent, tHeader.dwPad), NULL, 0),
TypeDescriptorField(NULL, DT_NULL, sizeof(ScriptedAchievementEvent), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_ScriptedAchievementEvent("ScriptedAchievementEvent", 0, ScriptedAchievementEvent_Fields, 5, NULL, 0x0, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_SCRIPTED_ACHIEVEMENT_EVENT = &TypeDescriptor_ScriptedAchievementEvent;

static const TypeDescriptorField ScriptedAchievementEventsTable_Fields[3] = {
TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(ScriptedAchievementEventsTable, serScriptedAchievementEvents), NULL, 0), VariableArrayTypeDescriptorField("", DT_SCRIPTED_ACHIEVEMENT_EVENT, offsetof(ScriptedAchievementEventsTable, ptScriptedAchievementEvents), 0x21, offsetof(ScriptedAchievementEventsTable, serScriptedAchievementEvents) - offsetof(ScriptedAchievementEventsTable, ptScriptedAchievementEvents)),

TypeDescriptorField(NULL, DT_NULL, sizeof(ScriptedAchievementEventsTable), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_ScriptedAchievementEventsTable("ScriptedAchievementEventsTable", 0, ScriptedAchievementEventsTable_Fields, 3, NULL, 0x0, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_SCRIPTED_ACHIEVEMENT_EVENTS_TABLE = &TypeDescriptor_ScriptedAchievementEventsTable;



static const TypeDescriptorField LootRunQuestTierEntry_Fields[69] = {
TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(LootRunQuestTierEntry, tHeader), NULL, 7340032), VariableArrayTypeDescriptorField("", DT_BYTE, offsetof(LootRunQuestTierEntry, tHeader.szName), 0x700020, offsetof(LootRunQuestTierEntry, tHeader) - offsetof(LootRunQuestTierEntry, tHeader.szName)), TypeDescriptorField("", DT_INT, offsetof(LootRunQuestTierEntry, tHeader.gbid), NULL, 0), TypeDescriptorField("", DT_INT, offsetof(LootRunQuestTierEntry, tHeader.dwPad), NULL, 0),
FilterTypeDescriptorField("", DT_SNO, offsetof(LootRunQuestTierEntry, tLootRunQuests), "", 31, 0, -1),
TypeDescriptorField("", DT_FLOAT, offsetof(LootRunQuestTierEntry, tLootRunQuests[0].flWeight), NULL, 1),
FilterTypeDescriptorField("", DT_GBID, offsetof(LootRunQuestTierEntry, tLootRunQuests[0].gbidType), "", 4294967295, 0, -1),
FilterTypeDescriptorField("", DT_GBID, offsetof(LootRunQuestTierEntry, tLootRunQuests[0].gbidLabel), "", 2, 0, -1),
FilterTypeDescriptorField("", DT_SNO, offsetof(LootRunQuestTierEntry, tLootRunQuests[1]), "", 31, 0, -1),
TypeDescriptorField("", DT_FLOAT, offsetof(LootRunQuestTierEntry, tLootRunQuests[1].flWeight), NULL, 1),
FilterTypeDescriptorField("", DT_GBID, offsetof(LootRunQuestTierEntry, tLootRunQuests[1].gbidType), "", 4294967295, 0, -1),
FilterTypeDescriptorField("", DT_GBID, offsetof(LootRunQuestTierEntry, tLootRunQuests[1].gbidLabel), "", 2, 0, -1),
FilterTypeDescriptorField("", DT_SNO, offsetof(LootRunQuestTierEntry, tLootRunQuests[2]), "", 31, 0, -1),
TypeDescriptorField("", DT_FLOAT, offsetof(LootRunQuestTierEntry, tLootRunQuests[2].flWeight), NULL, 1),
FilterTypeDescriptorField("", DT_GBID, offsetof(LootRunQuestTierEntry, tLootRunQuests[2].gbidType), "", 4294967295, 0, -1),
FilterTypeDescriptorField("", DT_GBID, offsetof(LootRunQuestTierEntry, tLootRunQuests[2].gbidLabel), "", 2, 0, -1),
FilterTypeDescriptorField("", DT_SNO, offsetof(LootRunQuestTierEntry, tLootRunQuests[3]), "", 31, 0, -1),
TypeDescriptorField("", DT_FLOAT, offsetof(LootRunQuestTierEntry, tLootRunQuests[3].flWeight), NULL, 1),
FilterTypeDescriptorField("", DT_GBID, offsetof(LootRunQuestTierEntry, tLootRunQuests[3].gbidType), "", 4294967295, 0, -1),
FilterTypeDescriptorField("", DT_GBID, offsetof(LootRunQuestTierEntry, tLootRunQuests[3].gbidLabel), "", 2, 0, -1),
FilterTypeDescriptorField("", DT_SNO, offsetof(LootRunQuestTierEntry, tLootRunQuests[4]), "", 31, 0, -1),
TypeDescriptorField("", DT_FLOAT, offsetof(LootRunQuestTierEntry, tLootRunQuests[4].flWeight), NULL, 1),
FilterTypeDescriptorField("", DT_GBID, offsetof(LootRunQuestTierEntry, tLootRunQuests[4].gbidType), "", 4294967295, 0, -1),
FilterTypeDescriptorField("", DT_GBID, offsetof(LootRunQuestTierEntry, tLootRunQuests[4].gbidLabel), "", 2, 0, -1),
FilterTypeDescriptorField("", DT_SNO, offsetof(LootRunQuestTierEntry, tLootRunQuests[5]), "", 31, 0, -1),
TypeDescriptorField("", DT_FLOAT, offsetof(LootRunQuestTierEntry, tLootRunQuests[5].flWeight), NULL, 1),
FilterTypeDescriptorField("", DT_GBID, offsetof(LootRunQuestTierEntry, tLootRunQuests[5].gbidType), "", 4294967295, 0, -1),
FilterTypeDescriptorField("", DT_GBID, offsetof(LootRunQuestTierEntry, tLootRunQuests[5].gbidLabel), "", 2, 0, -1),
FilterTypeDescriptorField("", DT_SNO, offsetof(LootRunQuestTierEntry, tLootRunQuests[6]), "", 31, 0, -1),
TypeDescriptorField("", DT_FLOAT, offsetof(LootRunQuestTierEntry, tLootRunQuests[6].flWeight), NULL, 1),
FilterTypeDescriptorField("", DT_GBID, offsetof(LootRunQuestTierEntry, tLootRunQuests[6].gbidType), "", 4294967295, 0, -1),
FilterTypeDescriptorField("", DT_GBID, offsetof(LootRunQuestTierEntry, tLootRunQuests[6].gbidLabel), "", 2, 0, -1),
FilterTypeDescriptorField("", DT_SNO, offsetof(LootRunQuestTierEntry, tLootRunQuests[7]), "", 31, 0, -1),
TypeDescriptorField("", DT_FLOAT, offsetof(LootRunQuestTierEntry, tLootRunQuests[7].flWeight), NULL, 1),
FilterTypeDescriptorField("", DT_GBID, offsetof(LootRunQuestTierEntry, tLootRunQuests[7].gbidType), "", 4294967295, 0, -1),
FilterTypeDescriptorField("", DT_GBID, offsetof(LootRunQuestTierEntry, tLootRunQuests[7].gbidLabel), "", 2, 0, -1),
FilterTypeDescriptorField("", DT_SNO, offsetof(LootRunQuestTierEntry, tLootRunQuests[8]), "", 31, 0, -1),
TypeDescriptorField("", DT_FLOAT, offsetof(LootRunQuestTierEntry, tLootRunQuests[8].flWeight), NULL, 1),
FilterTypeDescriptorField("", DT_GBID, offsetof(LootRunQuestTierEntry, tLootRunQuests[8].gbidType), "", 4294967295, 0, -1),
FilterTypeDescriptorField("", DT_GBID, offsetof(LootRunQuestTierEntry, tLootRunQuests[8].gbidLabel), "", 2, 0, -1),
FilterTypeDescriptorField("", DT_SNO, offsetof(LootRunQuestTierEntry, tLootRunQuests[9]), "", 31, 0, -1),
TypeDescriptorField("", DT_FLOAT, offsetof(LootRunQuestTierEntry, tLootRunQuests[9].flWeight), NULL, 1),
FilterTypeDescriptorField("", DT_GBID, offsetof(LootRunQuestTierEntry, tLootRunQuests[9].gbidType), "", 4294967295, 0, -1),
FilterTypeDescriptorField("", DT_GBID, offsetof(LootRunQuestTierEntry, tLootRunQuests[9].gbidLabel), "", 2, 0, -1),
FilterTypeDescriptorField("", DT_SNO, offsetof(LootRunQuestTierEntry, tLootRunQuests[10]), "", 31, 0, -1),
TypeDescriptorField("", DT_FLOAT, offsetof(LootRunQuestTierEntry, tLootRunQuests[10].flWeight), NULL, 1),
FilterTypeDescriptorField("", DT_GBID, offsetof(LootRunQuestTierEntry, tLootRunQuests[10].gbidType), "", 4294967295, 0, -1),
FilterTypeDescriptorField("", DT_GBID, offsetof(LootRunQuestTierEntry, tLootRunQuests[10].gbidLabel), "", 2, 0, -1),
FilterTypeDescriptorField("", DT_SNO, offsetof(LootRunQuestTierEntry, tLootRunQuests[11]), "", 31, 0, -1),
TypeDescriptorField("", DT_FLOAT, offsetof(LootRunQuestTierEntry, tLootRunQuests[11].flWeight), NULL, 1),
FilterTypeDescriptorField("", DT_GBID, offsetof(LootRunQuestTierEntry, tLootRunQuests[11].gbidType), "", 4294967295, 0, -1),
FilterTypeDescriptorField("", DT_GBID, offsetof(LootRunQuestTierEntry, tLootRunQuests[11].gbidLabel), "", 2, 0, -1),
FilterTypeDescriptorField("", DT_SNO, offsetof(LootRunQuestTierEntry, tLootRunQuests[12]), "", 31, 0, -1),
TypeDescriptorField("", DT_FLOAT, offsetof(LootRunQuestTierEntry, tLootRunQuests[12].flWeight), NULL, 1),
FilterTypeDescriptorField("", DT_GBID, offsetof(LootRunQuestTierEntry, tLootRunQuests[12].gbidType), "", 4294967295, 0, -1),
FilterTypeDescriptorField("", DT_GBID, offsetof(LootRunQuestTierEntry, tLootRunQuests[12].gbidLabel), "", 2, 0, -1),
FilterTypeDescriptorField("", DT_SNO, offsetof(LootRunQuestTierEntry, tLootRunQuests[13]), "", 31, 0, -1),
TypeDescriptorField("", DT_FLOAT, offsetof(LootRunQuestTierEntry, tLootRunQuests[13].flWeight), NULL, 1),
FilterTypeDescriptorField("", DT_GBID, offsetof(LootRunQuestTierEntry, tLootRunQuests[13].gbidType), "", 4294967295, 0, -1),
FilterTypeDescriptorField("", DT_GBID, offsetof(LootRunQuestTierEntry, tLootRunQuests[13].gbidLabel), "", 2, 0, -1),
FilterTypeDescriptorField("", DT_SNO, offsetof(LootRunQuestTierEntry, tLootRunQuests[14]), "", 31, 0, -1),
TypeDescriptorField("", DT_FLOAT, offsetof(LootRunQuestTierEntry, tLootRunQuests[14].flWeight), NULL, 1),
FilterTypeDescriptorField("", DT_GBID, offsetof(LootRunQuestTierEntry, tLootRunQuests[14].gbidType), "", 4294967295, 0, -1),
FilterTypeDescriptorField("", DT_GBID, offsetof(LootRunQuestTierEntry, tLootRunQuests[14].gbidLabel), "", 2, 0, -1),
FilterTypeDescriptorField("", DT_SNO, offsetof(LootRunQuestTierEntry, tLootRunQuests[15]), "", 31, 0, -1),
TypeDescriptorField("", DT_FLOAT, offsetof(LootRunQuestTierEntry, tLootRunQuests[15].flWeight), NULL, 1),
FilterTypeDescriptorField("", DT_GBID, offsetof(LootRunQuestTierEntry, tLootRunQuests[15].gbidType), "", 4294967295, 0, -1),
FilterTypeDescriptorField("", DT_GBID, offsetof(LootRunQuestTierEntry, tLootRunQuests[15].gbidLabel), "", 2, 0, -1),
TypeDescriptorField(NULL, DT_NULL, sizeof(LootRunQuestTierEntry), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_LootRunQuestTierEntry("LootRunQuestTierEntry", 0, LootRunQuestTierEntry_Fields, 69, NULL, 0x0, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_LOOTRUN_QUEST_TIER_ENTRY = &TypeDescriptor_LootRunQuestTierEntry;

static const TypeDescriptorField LootRunQuestTierTable_Fields[3] = {
TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(LootRunQuestTierTable, serLootRunQuestTiers), NULL, 0), VariableArrayTypeDescriptorField("", DT_LOOTRUN_QUEST_TIER_ENTRY, offsetof(LootRunQuestTierTable, ptLootRunQuestTiers), 0x21, offsetof(LootRunQuestTierTable, serLootRunQuestTiers) - offsetof(LootRunQuestTierTable, ptLootRunQuestTiers)),
TypeDescriptorField(NULL, DT_NULL, sizeof(LootRunQuestTierTable), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_LootRunQuestTierTable("LootRunQuestTierTable", 0, LootRunQuestTierTable_Fields, 3, NULL, 0x0, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_LOOTRUN_QUEST_TIER_TABLE = &TypeDescriptor_LootRunQuestTierTable;




static const TypeDescriptorField LegacyItemConversion_Fields[9] = {
TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(LegacyItemConversion, tHeader), NULL, 7340032), VariableArrayTypeDescriptorField("", DT_BYTE, offsetof(LegacyItemConversion, tHeader.szName), 0x700020, offsetof(LegacyItemConversion, tHeader) - offsetof(LegacyItemConversion, tHeader.szName)), TypeDescriptorField("", DT_INT, offsetof(LegacyItemConversion, tHeader.gbid), NULL, 0), TypeDescriptorField("", DT_INT, offsetof(LegacyItemConversion, tHeader.dwPad), NULL, 0),

FilterTypeDescriptorField("", DT_GBID, offsetof(LegacyItemConversion, gbidOldItem), "", 2, 0, -1),
FilterTypeDescriptorField("", DT_GBID, offsetof(LegacyItemConversion, gbidNewItem), "", 2, 0, -1),
TypeDescriptorField("", DT_INT, offsetof(LegacyItemConversion, bConsoleIgnore), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(LegacyItemConversion, dwPad), NULL, 1),
TypeDescriptorField(NULL, DT_NULL, sizeof(LegacyItemConversion), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_LegacyItemConversion("LegacyItemConversion", 0, LegacyItemConversion_Fields, 9, NULL, 0x0, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_LEGACY_ITEM_CONVERSION = &TypeDescriptor_LegacyItemConversion;

static const TypeDescriptorField LegacyItemConversionTable_Fields[3] = {
TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(LegacyItemConversionTable, serLegacyItemConversions), NULL, 0), VariableArrayTypeDescriptorField("", DT_LEGACY_ITEM_CONVERSION, offsetof(LegacyItemConversionTable, ptLegacyItemConversions), 0x21, offsetof(LegacyItemConversionTable, serLegacyItemConversions) - offsetof(LegacyItemConversionTable, ptLegacyItemConversions)),
TypeDescriptorField(NULL, DT_NULL, sizeof(LegacyItemConversionTable), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_LegacyItemConversionTable("LegacyItemConversionTable", 0, LegacyItemConversionTable_Fields, 3, NULL, 0x0, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_LEGACY_ITEM_CONVERSION_TABLE = &TypeDescriptor_LegacyItemConversionTable;




static const TypeDescriptorField EnchantItemAffixUseCountCostScalar_Fields[3] = {
TypeDescriptorField("", DT_INT, offsetof(EnchantItemAffixUseCountCostScalar, nUseCount), (const void *)1, 1),
TypeDescriptorField("", DT_FLOAT, offsetof(EnchantItemAffixUseCountCostScalar, flCostMultiplier), (const void *)0x3f800000 /* 1f */, 1),
TypeDescriptorField(NULL, DT_NULL, sizeof(EnchantItemAffixUseCountCostScalar), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_EnchantItemAffixUseCountCostScalar("EnchantItemAffixUseCountCostScalar", 0, EnchantItemAffixUseCountCostScalar_Fields, 3, NULL, 0x0, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_ENCHANT_ITEM_AFFIX_USE_COUNT_COST_SCALAR = &TypeDescriptor_EnchantItemAffixUseCountCostScalar;

static const TypeDescriptorField EnchantItemAffixUseCountCostScalarsTable_Fields[3] = {
TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(EnchantItemAffixUseCountCostScalarsTable, serEnchantItemAffixUseCountCostScalars), NULL, 0), VariableArrayTypeDescriptorField("", DT_ENCHANT_ITEM_AFFIX_USE_COUNT_COST_SCALAR, offsetof(EnchantItemAffixUseCountCostScalarsTable, ptEnchantItemAffixUseCountCostScalars), 0x21, offsetof(EnchantItemAffixUseCountCostScalarsTable, serEnchantItemAffixUseCountCostScalars) - offsetof(EnchantItemAffixUseCountCostScalarsTable, ptEnchantItemAffixUseCountCostScalars)),
TypeDescriptorField(NULL, DT_NULL, sizeof(EnchantItemAffixUseCountCostScalarsTable), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_EnchantItemAffixUseCountCostScalarsTable("EnchantItemAffixUseCountCostScalarsTable", 0, EnchantItemAffixUseCountCostScalarsTable_Fields, 3, NULL, 0x0, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_ENCHANT_ITEM_AFFIX_USE_COUNT_COST_SCALARS_TABLE = &TypeDescriptor_EnchantItemAffixUseCountCostScalarsTable;


static const TypeDescriptorField TransmuteRecipeIngredient_Fields[6] = {
TypeDescriptorField("", DT_INT, offsetof(TransmuteRecipeIngredient, eType), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(TransmuteRecipeIngredient, nTypeValue), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(TransmuteRecipeIngredient, nQuantity), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(TransmuteRecipeIngredient, unk_0c), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(TransmuteRecipeIngredient, unk_10), NULL, 1),
TypeDescriptorField(NULL, DT_NULL, sizeof(TransmuteRecipeIngredient), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_TransmuteRecipeIngredient("TransmuteRecipeIngredient", 0, TransmuteRecipeIngredient_Fields, 6, NULL, 0x0, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_TRANSMUTE_RECIPE_INGREDIENT = &TypeDescriptor_TransmuteRecipeIngredient;

static const TypeDescriptorField TransmuteRecipe_Fields[11] = {
TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(TransmuteRecipe, tHeader), NULL, 7340032), VariableArrayTypeDescriptorField("", DT_BYTE, offsetof(TransmuteRecipe, tHeader.szName), 0x700020, offsetof(TransmuteRecipe, tHeader) - offsetof(TransmuteRecipe, tHeader.szName)), TypeDescriptorField("", DT_INT, offsetof(TransmuteRecipe, tHeader.gbid), NULL, 0), TypeDescriptorField("", DT_INT, offsetof(TransmuteRecipe, tHeader.dwPad), NULL, 0),
TypeDescriptorField("", DT_INT, offsetof(TransmuteRecipe, eAction), NULL, 1),
EnumTypeDescriptorField("", offsetof(TransmuteRecipe, eAction), NULL, gTransmuteRecipeActionTypes),
FixedArrayTypeDescriptorField("", DT_TRANSMUTE_RECIPE_INGREDIENT, offsetof(TransmuteRecipe, tIngredients), NULL, 1, 8, 0),
TypeDescriptorField("", DT_INT, offsetof(TransmuteRecipe, nIngredientsCount), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(TransmuteRecipe, nPage), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(TransmuteRecipe, bHidden), NULL, 1),
TypeDescriptorField(NULL, DT_NULL, sizeof(TransmuteRecipe), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_TransmuteRecipe("TransmuteRecipe", 0, TransmuteRecipe_Fields, 11, NULL, 0x0, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_TRANSMUTE_RECIPE = &TypeDescriptor_TransmuteRecipe;

static const TypeDescriptorField TransmuteRecipesTable_Fields[3] = {
TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(TransmuteRecipesTable, serRecipes), NULL, 0), VariableArrayTypeDescriptorField("", DT_TRANSMUTE_RECIPE, offsetof(TransmuteRecipesTable, ptRecipes), 0x21, offsetof(TransmuteRecipesTable, serRecipes) - offsetof(TransmuteRecipesTable, ptRecipes)),
TypeDescriptorField(NULL, DT_NULL, sizeof(TransmuteRecipesTable), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_TransmuteRecipesTable("TransmuteRecipesTable", 0, TransmuteRecipesTable_Fields, 3, NULL, 0x0, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_TRANSMUTE_RECIPE_TABLE = &TypeDescriptor_TransmuteRecipesTable;




static const TypeDescriptorField CurrencyConversionTableEntry_Fields[11] = {
TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(CurrencyConversionTableEntry, tHeader), NULL, 7340032), VariableArrayTypeDescriptorField("", DT_BYTE, offsetof(CurrencyConversionTableEntry, tHeader.szName), 0x700020, offsetof(CurrencyConversionTableEntry, tHeader) - offsetof(CurrencyConversionTableEntry, tHeader.szName)), TypeDescriptorField("", DT_INT, offsetof(CurrencyConversionTableEntry, tHeader.gbid), NULL, 0), TypeDescriptorField("", DT_INT, offsetof(CurrencyConversionTableEntry, tHeader.dwPad), NULL, 0),
TypeDescriptorField("", DT_INT, offsetof(CurrencyConversionTableEntry, eCurrency), NULL, 1),
EnumTypeDescriptorField("", offsetof(CurrencyConversionTableEntry, eCurrency), NULL, gCurrencyTypes),
FilteredFixedArrayTypeDescriptorField("", DT_GBID, offsetof(CurrencyConversionTableEntry, arItemGBIDs), "", 0, 5, 2, 0),
TypeDescriptorField("", DT_INT, offsetof(CurrencyConversionTableEntry, nSortOrder), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(CurrencyConversionTableEntry, bHidden), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(CurrencyConversionTableEntry, bAutoPickup), NULL, 1),
TypeDescriptorField(NULL, DT_NULL, sizeof(CurrencyConversionTableEntry), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_CurrencyConversionTableEntry("CurrencyConversionTableEntry", 0, CurrencyConversionTableEntry_Fields, 11, NULL, 0x0, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_CURRENCY_CONVERSION_TABLE_ENTRY = &TypeDescriptor_CurrencyConversionTableEntry;

static const TypeDescriptorField CurrencyConversionTable_Fields[3] = {
TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(CurrencyConversionTable, serCurrencyConversionEntries), NULL, 0), VariableArrayTypeDescriptorField("", DT_CURRENCY_CONVERSION_TABLE_ENTRY, offsetof(CurrencyConversionTable, ptCurrencyConversionEntries), 0x21, offsetof(CurrencyConversionTable, serCurrencyConversionEntries) - offsetof(CurrencyConversionTable, ptCurrencyConversionEntries)),
TypeDescriptorField(NULL, DT_NULL, sizeof(CurrencyConversionTable), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_CurrencyConversionTable("CurrencyConversionTable", 0, CurrencyConversionTable_Fields, 3, NULL, 0x0, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_CURRENCY_CONVERSION_TABLE = &TypeDescriptor_CurrencyConversionTable;




static const TypeDescriptorField GameBalanceDefinition_Fields[38] = {
TypeDescriptorField("", DT_INT, offsetof(GameBalanceDefinition, eGameBalanceType), NULL, 1),


TypeDescriptorField("", DT_INT, offsetof(GameBalanceDefinition, dwMachineTime), NULL, 1),
TypeDescriptorField("", DT_INT, offsetof(GameBalanceDefinition, bIgnoreOnLoad), NULL, 1),
TypeDescriptorField("", DT_ITEMTYPETABLE, offsetof(GameBalanceDefinition, tItemTypeTable), NULL, 0),
TypeDescriptorField("", DT_ITEMTABLE, offsetof(GameBalanceDefinition, tItemTable), NULL, 0),
TypeDescriptorField("", DT_EXPERIENCETABLE, offsetof(GameBalanceDefinition, tExperienceTable), NULL, 0),
TypeDescriptorField("", DT_EXPERIENCETABLEALT, offsetof(GameBalanceDefinition, tExperienceTableAlt), NULL, 0),
TypeDescriptorField("", DT_HELPCODESTABLE, offsetof(GameBalanceDefinition, tHelpCodesTable), NULL, 0),
TypeDescriptorField("", DT_MONSTER_LEVEL_TABLE, offsetof(GameBalanceDefinition, tMonsterLevelTable), NULL, 0),
TypeDescriptorField("", DT_HEROTABLE, offsetof(GameBalanceDefinition, tHeroTable), NULL, 0),
TypeDescriptorField("", DT_MOVEMENT_STYLE_TABLE, offsetof(GameBalanceDefinition, tMovementStyleTable), NULL, 0),
TypeDescriptorField("", DT_LABEL_GBID_TABLE, offsetof(GameBalanceDefinition, tLabelGBIDTable), NULL, 0),
TypeDescriptorField("", DT_AFFIX_TABLE, offsetof(GameBalanceDefinition, tAffixTable), NULL, 0),
TypeDescriptorField("", DT_LOOTDISTRIBUTIONTABLE, offsetof(GameBalanceDefinition, tLootDistTable), NULL, 0),
TypeDescriptorField("", DT_RARE_ITEM_NAMES_TABLE, offsetof(GameBalanceDefinition, tRareItemNamesTable), NULL, 0),
TypeDescriptorField("", DT_MONSTER_AFFIXES_TABLE, offsetof(GameBalanceDefinition, tMonsterAffixesTable), NULL, 0),
TypeDescriptorField("", DT_RARE_MONSTER_NAMES_TABLE, offsetof(GameBalanceDefinition, tMonsterNamesTable), NULL, 0),
TypeDescriptorField("", DT_SOCKETED_EFFECTS_TABLE, offsetof(GameBalanceDefinition, tSocketedEffectTable), NULL, 0),
TypeDescriptorField("", DT_ITEM_DROP_TABLE, offsetof(GameBalanceDefinition, tItemDropTable), NULL, 0),
TypeDescriptorField("", DT_ITEM_LEVEL_MODIFIER_TABLE, offsetof(GameBalanceDefinition, tItemLevelModTable), NULL, 0),
TypeDescriptorField("", DT_QUALITY_CLASS_TABLE, offsetof(GameBalanceDefinition, tQualityClassTable), NULL, 0),
TypeDescriptorField("", DT_HANDICAP_LEVEL_TABLE, offsetof(GameBalanceDefinition, tHandicapLevelTable), NULL, 0),
TypeDescriptorField("", DT_ITEM_SALVAGE_LEVEL_TABLE, offsetof(GameBalanceDefinition, tItemSalvageLevelTable), NULL, 0),
TypeDescriptorField("", DT_HIRELINGTABLE, offsetof(GameBalanceDefinition, tHirelingTable), NULL, 0),
TypeDescriptorField("", DT_SET_ITEM_BONUS_TABLE, offsetof(GameBalanceDefinition, tSetItemBonusTable), NULL, 0),
TypeDescriptorField("", DT_ELITEMODIFIER_TABLE, offsetof(GameBalanceDefinition, tEliteModTable), NULL, 0),
TypeDescriptorField("", DT_ITEM_TIER_TABLE, offsetof(GameBalanceDefinition, tItemTierTable), NULL, 0),
TypeDescriptorField("", DT_POWERFORMULATABLE, offsetof(GameBalanceDefinition, tPowerFormulaTable), NULL, 0),
TypeDescriptorField("", DT_RECIPE_TABLE, offsetof(GameBalanceDefinition, tRecipeTable), NULL, 0),
TypeDescriptorField("", DT_SCRIPTED_ACHIEVEMENT_EVENTS_TABLE, offsetof(GameBalanceDefinition, tScriptedAchievementEventsTable), NULL, 0),
TypeDescriptorField("", DT_LOOTRUN_QUEST_TIER_TABLE, offsetof(GameBalanceDefinition, tLootRunQuestTierTable), NULL, 0),
TypeDescriptorField("", DT_PARAGON_BONUSES_TABLE, offsetof(GameBalanceDefinition, tParagonBonusesTable), NULL, 0),
TypeDescriptorField("", DT_LEGACY_ITEM_CONVERSION_TABLE, offsetof(GameBalanceDefinition, tLegacyItemConversionTable), NULL, 0),
TypeDescriptorField("", DT_ENCHANT_ITEM_AFFIX_USE_COUNT_COST_SCALARS_TABLE, offsetof(GameBalanceDefinition, tEnchantItemAffixUseCountCostScalarsTable), NULL, 0),
TypeDescriptorField("", DT_TIERED_LOOT_RUN_LEVEL_TABLE, offsetof(GameBalanceDefinition, tTieredLootRunLevelTable), NULL, 0),
TypeDescriptorField("", DT_TRANSMUTE_RECIPE_TABLE, offsetof(GameBalanceDefinition, tTransmuteRecipeTable), NULL, 0),
TypeDescriptorField("", DT_CURRENCY_CONVERSION_TABLE, offsetof(GameBalanceDefinition, tCurrencyConversionTable), NULL, 0),
TypeDescriptorField(NULL, DT_NULL, sizeof(GameBalanceDefinition), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_GameBalanceDefinition("GameBalance", 3, GameBalanceDefinition_Fields, 38, NULL, 0x0, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_GAMEBALANCE = &TypeDescriptor_GameBalanceDefinition;






GameBalanceGroupDef * gptGroupDefGameBalance;















GameBalanceGroupDef::GameBalanceGroupDef()
    : GroupDefinition("GameBalance", GAMEBALANCE_SNO,
                      SNOVersions(GAMEBALANCE_PC_SNO_VERSION, 2472, GAMEBALANCE_CONSOLE_SNO_VERSION,
                                  GAMEBALANCE_CONSOLE_SNO_VERSION, 2467),
                      SNODATATYPE_TITLE, sizeof(GameBalanceDefinition), DT_GAMEBALANCE, 0x9005, 15) {
}















































uint32 GameBalanceGroupDef::GetMaxCount() {
    return 60;
}









SNOPackType GameBalanceGroupDef::GetSNOPackDestination(const void * ptDefinition) const {
    const GameBalanceDefinition * pdefGB = (const GameBalanceDefinition *)ptDefinition;
    SNOPackType eSNOPackType;
    switch (pdefGB->eGameBalanceType) {
    case GB_MOVEMENT_STYLES:
    case GB_LOOTDISTRIBUTION:
    case GB_ITEM_DROP_TABLE:
    case GB_ITEM_LEVEL_MODIFIERS:
    case GB_ITEM_QUALITY_CLASSES:
    case GB_ELITE_MODIFIERS:
    case GB_ITEM_TIERS:
    case GB_SCRIPTED_ACHIEVEMENT_EVENTS:
    case GB_LOOTRUN_QUEST_TIERS:
        eSNOPackType = SNOPACKTYPE_SERVERDATA;
        break;
    default:
        eSNOPackType = SNOPACKTYPE_COREDATA;
        break;
    }
    return eSNOPackType;
}







































































BOOL GameBalanceGroupDef::Write(void * lpDefinition, FileReference & tRef) const {
    DisplayInternalError(NULL, "", 2123, ERROR_INTERNAL_FATAL);
    ExitProgram(ERROR_INTERNAL_FATAL);
}











void GameBalanceGroupDef::Postprocess(void * lpDefinition) {
    GroupDefinition::Postprocess(lpDefinition);

    GameBalanceDefinition * pdefGB = (GameBalanceDefinition *)lpDefinition;
    if (sGBTypeUsesGBID(pdefGB->eGameBalanceType) && pdefGB->bIgnoreOnLoad != TRUE) {
        int32 nSize = sGetGBSize(pdefGB->eGameBalanceType);
        int32 nCount = sGetGBCount(pdefGB);
        void * pTable = sGetGBTablePointer(pdefGB);
        for (int32 i = 0; i < nCount; i++) {
            GBHeader * ptHeader = (GBHeader *)((size_t)pTable + nSize * i);
            ptHeader->gbid = StringToGBID(ptHeader->szName);
        }
    }
}
























void GameBalanceGroupDef::Delete(void * lpDefinition) const {
    GroupDefinition::Delete(lpDefinition);
}









































































































const GameBalanceDefinition * PowerFormulaTableLockR() {
    return (const GameBalanceDefinition *)gptGroupDefGameBalance->LockRFast(
        GlobalSNOGet(GLOBALSNO_GB_POWER_FORMULA_TABLES));
}





void PowerFormulaTableUnlockR(const GameBalanceDefinition * lpDef) {
}







const GameBalanceDefinition * HelpCodesTableLockR() {
    return (const GameBalanceDefinition *)gptGroupDefGameBalance->LockRFast(GlobalSNOGet(GLOBALSNO_GB_HELPCODES));
}






void HelpCodesTableUnlockR(const GameBalanceDefinition * lpDef) {
}







const GameBalanceDefinition * MonsterLevelTableLockR() {
    return (const GameBalanceDefinition *)gptGroupDefGameBalance->LockRFast(GlobalSNOGet(GLOBALSNO_GB_MONSTER_LEVEL));
}






void MonsterLevelTableUnlockR(const GameBalanceDefinition * lpDef) {
}



























const GameBalanceDefinition * ItemTiersTableLockR() {
    return (const GameBalanceDefinition *)gptGroupDefGameBalance->LockRFast(GlobalSNOGet(GLOBALSNO_GB_ITEMTIERS));
}



void ItemTiersTableUnlockR(const GameBalanceDefinition * lpDef) {
}




const GameBalanceDefinition * LootDistributionTableLockR() {
    return (const GameBalanceDefinition *)gptGroupDefGameBalance->LockRFast(
        GlobalSNOGet(GLOBALSNO_GB_LOOT_DISTRIBUTION));
}













CRefString GBIDToString(const GBID & gbid) {
    const AppGlobals & tAppGlobals = sgtAppGlobals;
    const GameBalanceAppGlobals * ptGBAppGlobals = tAppGlobals.ptGBAppGlobals;
    for (int32 i = 0; i < NUM_GBTYPES; i++) {
        GBEntry tEntry;
        if (ptGBAppGlobals->pmapGameBalanceDefs[i].Lookup(gbid, tEntry)) {
            ScopedGameBalanceDefinitionLockR pdefGB(tEntry.snoGB);
            GBHeader * ptHeader = (GBHeader *)((uint8 *)sGetGBTablePointer(pdefGB) + sGetGBSize(i) * tEntry.nIndex);
            return CRefString(ptHeader->szName);
        }
    }
    return CRefString("[bad gbid]");
}



















































































GBID GBIDExists(LPCSTR szGBID, GameBalanceType eGBType) {
    GBID gbid = StringToGBID(szGBID);
    BOOL bRetval = GBIDExists(gbid, eGBType);
    return bRetval ? gbid : GBID_NONE;
}











BOOL GBIDExists(const GBHandle & hGB) {
    return GBIDExists(hGB.gbid, hGB.eType);
}



BOOL GBIDExists(GBID gbid, GameBalanceType eGBType) {
    const AppGlobals & tAppGlobals = sgtAppGlobals;
    const GameBalanceAppGlobals * ptGBAppGlobals = tAppGlobals.ptGBAppGlobals;
    GBEntry tEntry;
    if (eGBType < 0 || eGBType >= NUM_GBTYPES)
        return FALSE;
    return ptGBAppGlobals->pmapGameBalanceDefs[eGBType].Lookup(gbid, tEntry);
}













size_t sGetGBTableOffset(GameBalanceType eType) {
    switch (eType) {
    case GB_ITEMTYPES: return offsetof(GameBalanceDefinition, tItemTypeTable.ptItemTypes);
    case GB_ITEMS: return offsetof(GameBalanceDefinition, tItemTable.ptItems);
    case GB_EXPERIENCETABLE: return offsetof(GameBalanceDefinition, tExperienceTable.ptExperienceLevels);
    case GB_HIRELINGS: return offsetof(GameBalanceDefinition, tHirelingTable.ptHirelings);
    case GB_MONSTER_LEVELS: return offsetof(GameBalanceDefinition, tMonsterLevelTable.ptMonsterLevels);
    case GB_HEROS: return offsetof(GameBalanceDefinition, tHeroTable.ptHeros);
    case GB_AFFIXES: return offsetof(GameBalanceDefinition, tAffixTable.ptAffixes);
    case GB_MOVEMENT_STYLES: return offsetof(GameBalanceDefinition, tMovementStyleTable.ptMovementStyles);
    case GB_LABELS: return offsetof(GameBalanceDefinition, tLabelGBIDTable.ptLabelGBIDs);
    case GB_LOOTDISTRIBUTION: return offsetof(GameBalanceDefinition, tLootDistTable.ptLootDistributions);
    case GB_RARE_ITEM_NAMES: return offsetof(GameBalanceDefinition, tRareItemNamesTable.ptRareItemNames);
    case GB_MONSTER_AFFIXES: return offsetof(GameBalanceDefinition, tMonsterAffixesTable.ptMonsterAffixes);
    case GB_MONSTER_NAMES: return offsetof(GameBalanceDefinition, tMonsterNamesTable.ptRareMonsterNames);
    case GB_SOCKETED_EFFECTS: return offsetof(GameBalanceDefinition, tSocketedEffectTable.ptSocketedEffects);
    case GB_HELPCODES: return offsetof(GameBalanceDefinition, tHelpCodesTable.ptHelpCodes);
    case GB_ITEM_DROP_TABLE: return offsetof(GameBalanceDefinition, tItemDropTable.ptItemDrops);
    case GB_ITEM_LEVEL_MODIFIERS: return offsetof(GameBalanceDefinition, tItemLevelModTable.ptItemLevelModifiers);
    case GB_ITEM_QUALITY_CLASSES: return offsetof(GameBalanceDefinition, tQualityClassTable.ptQualityClasses);
    case GB_HANDICAPS: return offsetof(GameBalanceDefinition, tHandicapLevelTable.ptHandicapLevels);
    case GB_ITEM_SALVAGE_LEVELS: return offsetof(GameBalanceDefinition, tItemSalvageLevelTable.ptItemSalvageLevels);
    case GB_RECIPES: return offsetof(GameBalanceDefinition, tRecipeTable.ptRecipes);
    case GB_SET_ITEM_BONUSES: return offsetof(GameBalanceDefinition, tSetItemBonusTable.ptSetItemBonuses);
    case GB_ELITE_MODIFIERS: return offsetof(GameBalanceDefinition, tEliteModTable.ptMods);
    case GB_ITEM_TIERS: return offsetof(GameBalanceDefinition, tItemTierTable.ptTiers);
    case GB_POWER_FORMULA_TABLES: return offsetof(GameBalanceDefinition, tPowerFormulaTable.ptPowerFormulaTableEntries);
    case GB_SCRIPTED_ACHIEVEMENT_EVENTS: return offsetof(GameBalanceDefinition, tScriptedAchievementEventsTable.ptScriptedAchievementEvents);
    case GB_LOOTRUN_QUEST_TIERS: return offsetof(GameBalanceDefinition, tLootRunQuestTierTable.ptLootRunQuestTiers);
    case GB_PARAGON_BONUSES: return offsetof(GameBalanceDefinition, tParagonBonusesTable.ptParagonBonuses);
    case GB_LEGACY_ITEM_CONVERSIONS: return offsetof(GameBalanceDefinition, tLegacyItemConversionTable.ptLegacyItemConversions);
    case GB_ENCHANT_ITEM_AFFIX_USE_COUNT_COST_SCALARS: return offsetof(GameBalanceDefinition, tEnchantItemAffixUseCountCostScalarsTable.ptEnchantItemAffixUseCountCostScalars);
    case GB_EXPERIENCETABLEALT: return offsetof(GameBalanceDefinition, tExperienceTableAlt.ptExperienceLevelsAlt);
    case GB_TIERED_LOOT_RUN_LEVELS: return offsetof(GameBalanceDefinition, tTieredLootRunLevelTable.ptTieredLootRunLevels);
    case GB_TRANSMUTE_RECIPES: return offsetof(GameBalanceDefinition, tTransmuteRecipeTable.ptRecipes);
    case GB_CURRENCY_CONVERSIONS: return offsetof(GameBalanceDefinition, tCurrencyConversionTable.ptCurrencyConversionEntries);
    }
    DisplayInternalError(NULL, "", 2594, ERROR_INTERNAL_FATAL);
    ExitProgram(ERROR_INTERNAL_FATAL);
}









































int32 sGetGBSize(GameBalanceType eType) {
    switch (eType) {
    case GB_ITEMTYPES: return sizeof(ItemTypeDefinition);
    case GB_ITEMS: return sizeof(ItemDefinition);
    case GB_EXPERIENCETABLE: return sizeof(ExperienceLevel);
    case GB_HIRELINGS: return sizeof(HirelingEntry);
    case GB_MONSTER_LEVELS: return sizeof(MonsterLevelDefinition);
    case GB_HEROS: return sizeof(HeroData);
    case GB_AFFIXES: return sizeof(AffixTableEntry);
    case GB_MOVEMENT_STYLES: return sizeof(MovementStyleDefinition);
    case GB_LABELS: return sizeof(LabelGBID);
    case GB_LOOTDISTRIBUTION: return sizeof(LootDistEntry);
    case GB_RARE_ITEM_NAMES: return sizeof(RareItemNamesEntry);
    case GB_MONSTER_AFFIXES: return sizeof(MonsterAffixesEntry);
    case GB_MONSTER_NAMES: return sizeof(RareMonsterNamesEntry);
    case GB_SOCKETED_EFFECTS: return sizeof(SocketedEffectsTableEntry);
    case GB_HELPCODES: return sizeof(HelpCode);
    case GB_ITEM_DROP_TABLE: return sizeof(ItemDropTableEntry);
    case GB_ITEM_LEVEL_MODIFIERS: return sizeof(ItemLevelModifier);
    case GB_ITEM_QUALITY_CLASSES: return sizeof(QualityClass);
    case GB_HANDICAPS: return sizeof(HandicapLevel);
    case GB_ITEM_SALVAGE_LEVELS: return sizeof(ItemSalvageLevel);
    case GB_RECIPES: return sizeof(RecipeEntry);
    case GB_SET_ITEM_BONUSES: return sizeof(SetItemBonusTableEntry);
    case GB_ELITE_MODIFIERS: return sizeof(EliteModifierData);
    case GB_ITEM_TIERS: return sizeof(ItemTierData);
    case GB_POWER_FORMULA_TABLES: return sizeof(PowerFormulaTable);
    case GB_SCRIPTED_ACHIEVEMENT_EVENTS: return sizeof(ScriptedAchievementEvent);
    case GB_LOOTRUN_QUEST_TIERS: return sizeof(LootRunQuestTierEntry);
    case GB_PARAGON_BONUSES: return sizeof(ParagonBonus);
    case GB_LEGACY_ITEM_CONVERSIONS: return sizeof(LegacyItemConversion);
    case GB_ENCHANT_ITEM_AFFIX_USE_COUNT_COST_SCALARS: return sizeof(EnchantItemAffixUseCountCostScalar);
    case GB_TIERED_LOOT_RUN_LEVELS: return sizeof(TieredLootRunLevel);
    case GB_TRANSMUTE_RECIPES: return sizeof(TransmuteRecipe);
    case GB_CURRENCY_CONVERSIONS: return sizeof(CurrencyConversionTableEntry);
    }
    DisplayInternalError(NULL, "", 2673, ERROR_INTERNAL_FATAL);
    ExitProgram(ERROR_INTERNAL_FATAL);
}
























































































































inline void * sGetGBTablePointer(const GameBalanceDefinition * pdefGB) {
    size_t nTableOffset = sGetGBTableOffset(pdefGB->eGameBalanceType);
    return *(XPtr64<uint8> *)((uintptr)pdefGB + nTableOffset);
}







static BOOL sGBTypeUsesGBID(GameBalanceType eType) {
    switch (eType) {
    case GB_ITEMTYPES:
    case GB_ITEMS:
    case GB_HIRELINGS:
    case GB_HEROS:
    case GB_AFFIXES:
    case GB_MOVEMENT_STYLES:
    case GB_LABELS:
    case GB_RARE_ITEM_NAMES:
    case GB_MONSTER_AFFIXES:
    case GB_MONSTER_NAMES:
    case GB_SOCKETED_EFFECTS:
    case GB_ITEM_DROP_TABLE:
    case GB_ITEM_QUALITY_CLASSES:
    case GB_RECIPES:
    case GB_SET_ITEM_BONUSES:
    case GB_ELITE_MODIFIERS:
    case GB_SCRIPTED_ACHIEVEMENT_EVENTS:
    case GB_LOOTRUN_QUEST_TIERS:
    case GB_PARAGON_BONUSES:
    case GB_LEGACY_ITEM_CONVERSIONS:
    case GB_TRANSMUTE_RECIPES:
    case GB_CURRENCY_CONVERSIONS:
        return TRUE;
    }
    return FALSE;
}








const void * GBLockR(const GBHandle & hGB, const GameBalanceDefinition *& pDef, const BOOL & bDisplayMissing) {
    const AppGlobals & tAppGlobals = sgtAppGlobals;
    const GameBalanceAppGlobals * ptGBAppGlobals = tAppGlobals.ptGBAppGlobals;
    GBEntry tEntry;
    BOOL bRetval = ptGBAppGlobals->pmapGameBalanceDefs[hGB.eType].Lookup(hGB.gbid, tEntry);
    if (!bRetval) {
        GBID gbidBadData = StringToGBID(cgszBadDataName);
        BOOL bBadDataRetval = ptGBAppGlobals->pmapGameBalanceDefs[hGB.eType].Lookup(gbidBadData, tEntry);
        if (!bBadDataRetval) {
            if (bDisplayMissing) {
                // The missing-GBID report is compiled out of this build; the test is all that is left.
            }
            pDef = NULL;
            return NULL;
        }
    }
    pDef = DefinitionLockR<GameBalanceDefinition>(tEntry.snoGB);
    void * pRetval = (uint8 *)sGetGBTablePointer(pDef) + sGetGBSize(hGB.eType) * tEntry.nIndex;
    return pRetval;
}




























const void * GBTryLockR(const GBHandle & hGB, const GameBalanceDefinition *& pDef) {
    return GBLockR(hGB, pDef, FALSE);
}







void GBUnlockR(const GameBalanceDefinition *& pDef) {
}


















int32 sGetGBCount(const GameBalanceDefinition * pdefGB) {
    int32 nRetval;
    switch (pdefGB->eGameBalanceType) {
    case GB_ITEMTYPES: nRetval = pdefGB->tItemTypeTable.serItemTypes.dwSize; break;
    case GB_ITEMS: nRetval = pdefGB->tItemTable.serItems.dwSize; break;
    case GB_EXPERIENCETABLE: nRetval = pdefGB->tExperienceTable.serExperienceLevels.dwSize; break;
    case GB_HIRELINGS: nRetval = pdefGB->tHirelingTable.serHirelings.dwSize; break;
    case GB_MONSTER_LEVELS: nRetval = pdefGB->tMonsterLevelTable.serMonsterLevels.dwSize; break;
    case GB_HEROS: nRetval = pdefGB->tHeroTable.serHeros.dwSize; break;
    case GB_AFFIXES: nRetval = pdefGB->tAffixTable.serAffixes.dwSize; break;
    case GB_MOVEMENT_STYLES: nRetval = pdefGB->tMovementStyleTable.serMovementStyles.dwSize; break;
    case GB_LABELS: nRetval = pdefGB->tLabelGBIDTable.serLabelGBIDs.dwSize; break;
    case GB_LOOTDISTRIBUTION: nRetval = pdefGB->tLootDistTable.serLootDistributions.dwSize; break;
    case GB_RARE_ITEM_NAMES: nRetval = pdefGB->tRareItemNamesTable.serRareItemNames.dwSize; break;
    case GB_MONSTER_AFFIXES: nRetval = pdefGB->tMonsterAffixesTable.serMonsterAffixes.dwSize; break;
    case GB_MONSTER_NAMES: nRetval = pdefGB->tMonsterNamesTable.serRareMonsterNames.dwSize; break;
    case GB_SOCKETED_EFFECTS: nRetval = pdefGB->tSocketedEffectTable.serSocketedEffects.dwSize; break;
    case GB_HELPCODES: nRetval = pdefGB->tHelpCodesTable.serHelpCodes.dwSize; break;
    case GB_ITEM_DROP_TABLE: nRetval = pdefGB->tItemDropTable.serItemDrops.dwSize; break;
    case GB_ITEM_LEVEL_MODIFIERS: nRetval = pdefGB->tItemLevelModTable.serItemLevelModifiers.dwSize; break;
    case GB_ITEM_QUALITY_CLASSES: nRetval = pdefGB->tQualityClassTable.serQualityClasses.dwSize; break;
    case GB_HANDICAPS: nRetval = pdefGB->tHandicapLevelTable.serHandicapLevels.dwSize; break;
    case GB_ITEM_SALVAGE_LEVELS: nRetval = pdefGB->tItemSalvageLevelTable.serItemSalvageLevels.dwSize; break;
    case GB_RECIPES: nRetval = pdefGB->tRecipeTable.serRecipes.dwSize; break;
    case GB_SET_ITEM_BONUSES: nRetval = pdefGB->tSetItemBonusTable.serSetItemBonuses.dwSize; break;
    case GB_ELITE_MODIFIERS: nRetval = pdefGB->tEliteModTable.serMods.dwSize; break;
    case GB_ITEM_TIERS: nRetval = pdefGB->tItemTierTable.serTiers.dwSize; break;
    case GB_POWER_FORMULA_TABLES: nRetval = pdefGB->tPowerFormulaTable.serPowerFormulaTableEntries.dwSize; break;
    case GB_SCRIPTED_ACHIEVEMENT_EVENTS: nRetval = pdefGB->tScriptedAchievementEventsTable.serScriptedAchievementEvents.dwSize; break;
    case GB_LOOTRUN_QUEST_TIERS: nRetval = pdefGB->tLootRunQuestTierTable.serLootRunQuestTiers.dwSize; break;
    case GB_PARAGON_BONUSES: nRetval = pdefGB->tParagonBonusesTable.serParagonBonuses.dwSize; break;
    case GB_LEGACY_ITEM_CONVERSIONS: nRetval = pdefGB->tLegacyItemConversionTable.serLegacyItemConversions.dwSize; break;
    case GB_ENCHANT_ITEM_AFFIX_USE_COUNT_COST_SCALARS: nRetval = pdefGB->tEnchantItemAffixUseCountCostScalarsTable.serEnchantItemAffixUseCountCostScalars.dwSize; break;
    case GB_EXPERIENCETABLEALT: nRetval = pdefGB->tExperienceTableAlt.serExperienceLevelsAlt.dwSize; break;
    case GB_TIERED_LOOT_RUN_LEVELS: nRetval = pdefGB->tTieredLootRunLevelTable.serTieredLootRunLevels.dwSize; break;
    case GB_TRANSMUTE_RECIPES: nRetval = pdefGB->tTransmuteRecipeTable.serRecipes.dwSize; break;
    case GB_CURRENCY_CONVERSIONS: nRetval = pdefGB->tCurrencyConversionTable.serCurrencyConversionEntries.dwSize; break;
    default:
        DisplayInternalError(NULL, "", 2995, ERROR_INTERNAL_FATAL);
        ExitProgram(ERROR_INTERNAL_FATAL);
    }
    return nRetval / sGetGBSize(pdefGB->eGameBalanceType);
}














































































static void sLoadGBInfo(SNO snoGB, BOOL fAllowOverride) {
    GameBalanceDefinition * pdefGB = (GameBalanceDefinition *)gptGroupDefGameBalance->LockRFast(snoGB);
    if (!sGBTypeUsesGBID(pdefGB->eGameBalanceType) || pdefGB->bIgnoreOnLoad == TRUE)
        return;
    int32 nSize = sGetGBSize(pdefGB->eGameBalanceType);
    int32 nCount = sGetGBCount(pdefGB);
    void * pTable = sGetGBTablePointer(pdefGB);
    AppGlobals & tAppGlobals = sgtAppGlobals;
    GameBalanceAppGlobals * ptGBAppGlobals = tAppGlobals.ptGBAppGlobals;
    for (int32 i = 0; i < nCount; i++) {
        GBHeader * ptHeader = (GBHeader *)((uint8 *)pTable + i * nSize);
        if (ptHeader->gbid == GBID_NONE)
            continue;
        GBEntry tEntry;
        tEntry.snoGB = snoGB;
        tEntry.nIndex = i;
        ptGBAppGlobals->pmapGameBalanceDefs[pdefGB->eGameBalanceType].SetAt_Tracked(ptHeader->gbid, tEntry);
        if (pdefGB->eGameBalanceType == GB_ITEMS) {
            SNO snoActor = ((ItemDefinition *)ptHeader)->snoActor;
            GBID gbidItem;
            if (!ptGBAppGlobals->mapActorSNOToItemGBID.Lookup(snoActor, gbidItem))
                ptGBAppGlobals->mapActorSNOToItemGBID.SetAt_Tracked(snoActor, ptHeader->gbid);
        }
    }
}























































static void sLoadGBs() {
    GameBalanceAppGlobals * ptGBAppGlobals = sgtAppGlobals.ptGBAppGlobals;
    for (int32 i = 0; i < NUM_GBTYPES; i++) {
        ptGBAppGlobals->pmapGameBalanceDefs[i].RemoveAll();
    }
    ptGBAppGlobals->mapActorSNOToItemGBID.RemoveAll();
    PooledSNONameList listSNOFiles(GetSnoNameListPool());
    SNOFileEnumerateSNOsForGroup(GAMEBALANCE_SNO, &listSNOFiles);
    POS pos = listSNOFiles.GetHeadPosition();
    while (pos) {
        SNOName & snonameFile = listSNOFiles.GetNext(pos);
        sLoadGBInfo(snonameFile.snoHandle, FALSE);
    }
}





























void GameBalanceReloadSNOs() {
    AppGlobals & tAppGlobals = sgtAppGlobals;
    GameBalanceAppGlobals * ptGBAppGlobals = tAppGlobals.ptGBAppGlobals;
    for (int32 i = 0; i < NUM_GBTYPES; i++) {
        ptGBAppGlobals->pmapGameBalanceDefs[i].RemoveAll();
    }
    ptGBAppGlobals->mapActorSNOToItemGBID.RemoveAll();
    sLoadGBs();
}













static const uint32 sadwHashTableSizes[NUM_GBTYPES] = {
    16,
    128,
    2048,
    16,
    16,
    16,
    16,
    16,
    1024,
    16,
    16,
    512,
    16,
    16,
    16,
    16,
    512,
    16,
    32,
    256,
    16,
    128,
    16,
    256,
    16,
    512,
    16,
    16,
    16,
    16,
    16,
    16,
    512,
    256,
    16,
    16,
    16,
    32,
    16,
    16,
    16,
    16,
    128,
    16,
    16,
    16,
    16,
    16,
    16,
    16,
    16,
    16,
};







void GameBalanceInitializeForApp() {
    AppGlobals & tAppGlobals = sgtAppGlobals;
    GameBalanceAppGlobals *& ptGBAppGlobals = tAppGlobals.ptGBAppGlobals;
    ptGBAppGlobals = (GameBalanceAppGlobals *)SigmaMemoryNew(sizeof(GameBalanceAppGlobals), 4, NULL, TRUE);
    ptGBAppGlobals->pmapGameBalanceDefs = (GBIDMap *)SigmaMemoryNew(sizeof(GBIDMap) * NUM_GBTYPES, 4, NULL, TRUE);
    ptGBAppGlobals->poolGBHandles.Initialize();
    ptGBAppGlobals->poolGBHandles.SetBlockCount_Tracked(1024);
    ptGBAppGlobals->poolGBHandles.SetGrow(XALLOC_GROW_LINEAR, 1024);
    ptGBAppGlobals->poolGBHandles.SetShrinkable(TRUE);
    ptGBAppGlobals->mapActorSNOToItemGBID.Initialize(16, FALSE, NULL);
    ptGBAppGlobals->mapActorSNOToItemGBID.SetHashTableSize_Tracked(1024);
    for (int32 i = 0; i < NUM_GBTYPES; i++) {
        ptGBAppGlobals->pmapGameBalanceDefs[i].Initialize(16, FALSE, NULL);
        ptGBAppGlobals->pmapGameBalanceDefs[i].SetHashTableSize_Tracked(sadwHashTableSizes[i]);
    }
    GameBalanceReloadSNOs();
    GlobalGBIDListRefresh();
}

















void GameBalanceDisposeForApp() {
    AppGlobals & tAppGlobals = sgtAppGlobals;
    GameBalanceAppGlobals *& ptGBAppGlobals = tAppGlobals.ptGBAppGlobals;
    for (int32 i = 0; i < NUM_GBTYPES; i++) {
        ptGBAppGlobals->pmapGameBalanceDefs[i].Dispose();
    }
    ptGBAppGlobals->mapActorSNOToItemGBID.Dispose();
    ptGBAppGlobals->poolGBHandles.Dispose();
    SigmaMemoryFree(ptGBAppGlobals->pmapGameBalanceDefs, NULL);
    ptGBAppGlobals->pmapGameBalanceDefs = NULL;
    SigmaMemoryFree(ptGBAppGlobals, NULL);
    ptGBAppGlobals = NULL;
}











void GameBalanceInitializeForGame() {
    GameCommonData & tGCD = GameGetActiveGameCommonData();
    XMemoryAllocatorInterface * ptMemoryInterface = AppGetCurrentMemoryInterface();

    tGCD.ptGBGlobals = (GameBalanceGlobals *)SigmaMemoryNew(sizeof(GameBalanceGlobals), 4, ptMemoryInterface, TRUE);
    tGCD.ptGBGlobals->poolGBHandles.Initialize();
    tGCD.ptGBGlobals->poolGBHandles.SetMemoryInterface(ptMemoryInterface);
    tGCD.ptGBGlobals->poolGBHandles.SetBlockCount_Tracked(1024);

    tGCD.ptGBGlobals->poolGBHandles.SetGrow(XALLOC_GROW_LINEAR, 1024);
    tGCD.ptGBGlobals->poolGBHandles.SetShrinkable(TRUE);
}








void GameBalanceDisposeForGame() {
    GameCommonData & tGCD = GameGetActiveGameCommonData();
    XMemoryAllocatorInterface * ptMemoryInterface = AppGetCurrentMemoryInterface();
    tGCD.ptGBGlobals->poolGBHandles.Dispose();
    SigmaMemoryFree(tGCD.ptGBGlobals, ptMemoryInterface);
    tGCD.ptGBGlobals = NULL;
}








SNO GameBalanceGetActorSNO(const GBHandle & hGB) {
    SNO snoRetval = SNO_NONE;
    if (hGB.gbid == GBID_NONE)
        return SNO_NONE;
    switch (hGB.eType) {
    case GB_SCENERY:
        snoRetval = hGB.gbid;
        break;
    case GB_ITEMS: {
        ScopedItemDefinitionLockR ptItem(hGB.gbid);
        if (!ptItem)
            return SNO_NONE;
        snoRetval = ptItem->snoActor;
        break;
    }
    default:
        return SNO_NONE;
    }
    return snoRetval;
}




















SNO GameBalanceGetHeroSNO(const GBHandle & hGB, const uint32 dwPlayerFlags) {
    SNO snoRetval = SNO_NONE;
    if (hGB.gbid != GBID_NONE) {
        const GameBalanceDefinition * pdefGB;
        HeroData * ptChar = (HeroData *)GBLockR(hGB, pdefGB, TRUE);
        if (ptChar) {
            snoRetval = (dwPlayerFlags & (1 << PLAYERFLAG_FEMALE)) ? ptChar->snoActorFemale : ptChar->snoActorMale;
        }
        GBUnlockR(pdefGB);
    }
    return snoRetval;
}













































void sGBEnumerateType(GameBalanceType eType, GBHandleList & listResults) {
    AppGlobals & tAppGlobals = sgtAppGlobals;
    GameBalanceAppGlobals * ptGBAppGlobals = tAppGlobals.ptGBAppGlobals;
    GBIDMap & tMap = ptGBAppGlobals->pmapGameBalanceDefs[eType];
    POS pos = tMap.GetStartAssoc();
    while (pos) {
        GBID gbid;
        GBEntry gbEntry;
        tMap.GetNextAssoc(pos, gbid, gbEntry);
        GBHandle gbHandle(eType, gbid);
        listResults.AddTail_Tracked(gbHandle);
    }
}













void GBEnumerate(GameBalanceType eType, GBHandleList & listResults) {
    if (eType == GB_INVALID) {
        for (int32 i = 0; i < NUM_GBTYPES; i++) {
            sGBEnumerateType(i, listResults);
        }
    } else {
        sGBEnumerateType(eType, listResults);
    }
}










BOOL FindGBHandleByActor(SNO snoActor, GBHandle & gbHandle) {
    const AppGlobals & tAppGlobals = sgtAppGlobals;
    const GameBalanceAppGlobals * ptGBAppGlobals = tAppGlobals.ptGBAppGlobals;
    if (snoActor == SNO_NONE)
        return FALSE;
    GBID gbidItem;
    if (ptGBAppGlobals->mapActorSNOToItemGBID.Lookup(snoActor, gbidItem)) {
        gbHandle.Set(GB_ITEMS, gbidItem);
        return TRUE;
    }
    return FALSE;
}














CRefString GBHandleToString(const GBHandle & hGB) {
    LPCSTR szSymbol = SymbolToName(hGB.eType, gGameBalanceTypes);
    CRefString sGbid = GBIDToString(hGB, FALSE);
    CRefString sGBHandle;
    sGBHandle.Format("%s:%s", szSymbol, sGbid.str());
    return sGBHandle;
}










CRefString GBIDToString(GBID gbid, GameBalanceType eGBType, const BOOL fUsedForXMLFields) {
    GBHandle hGB(eGBType, gbid);
    return GBIDToString(hGB, fUsedForXMLFields);
}





CRefString GBIDToString(const GBHandle & hGB, const BOOL fUsedForXMLFields) {
    CRefString sReturn;
    if (hGB.eType == GB_INVALID) {
        sReturn = GBIDToString(hGB.gbid);
    } else if (sGBTypeUsesGBID(hGB.eType)) {
        if (hGB.gbid == GBID_NONE)
            return CRefString("!NONE!");
        const GameBalanceDefinition * ptDef;
        const GBHeader * ptHeader = (const GBHeader *)GBLockR(hGB, ptDef, FALSE);
        if (ptHeader) {
            sReturn = ptHeader->szName;
        } else if (!fUsedForXMLFields) {
            sReturn.Format("GBID String not found: GBID = %u, eType = %u", hGB.gbid, hGB.eType);
        }
    } else {
        sReturn = "GBType doesn't use GBID";
    }
    return sReturn;
}
























GBHandlePool * GBGetHandlePool() {
    GameCommonData & tGCD = GameGetActiveGameCommonData();
    if (tGCD.ptGBGlobals)
        return &tGCD.ptGBGlobals->poolGBHandles;
    {
        AppGlobals & tAppGlobals = sgtAppGlobals;
        GameBalanceAppGlobals *& ptGBAppGlobals = tAppGlobals.ptGBAppGlobals;
        return &ptGBAppGlobals->poolGBHandles;
    }
}

















static float sGameBalanceDefGetAttributes(const void * pGBData, GameBalanceType eType, const GameBalanceAttribute eGBAttribute) {
    if (eType == GB_ITEMS) {
        const ItemDefinition * pItem = (const ItemDefinition *)pGBData;
        switch (eGBAttribute) {
        case GBA_DAMAGE: return pItem->flAttributesDamage;
        case GBA_DAMAGERANGE: return pItem->flAttributesDamageRange;
        case GBA_ARMOR: return pItem->flAttributesArmor;
        case GBA_ARMOR_DELTA: return pItem->flAttributesArmorDelta;
        case GBA_ATTACKS_PER_SECOND: return pItem->flAttributesAttacksPerSecond;
        case GBA_DAMAGE_VARIANCE_MIN: return pItem->flAttributesDamageVarianceMin;
        case GBA_DAMAGE_VARIANCE_DELTA: return pItem->flAttributesDamageVarianceDelta;
        default: return 0.0f;
        }
    }
    if (eType == GB_HIRELINGS) {
        const HirelingEntry * pHireling = (const HirelingEntry *)pGBData;
        switch (eGBAttribute) {
        case GBA_CRIT_DAMAGE_CAP: return pHireling->flAttributesCritDamageCap;
        case GBA_CRIT_PERCENT_BASE: return pHireling->flAttributesCritPercentBase;
        case GBA_CRIT_PERCENT_CAP: return pHireling->flAttributesCritPercentCap;
        case GBA_DODGE_RATING_BASE: return pHireling->flAttributesDodgeRatingBase;
        case GBA_STRENGTH: return pHireling->flAttributesStrength;
        case GBA_DEXTERITY: return pHireling->flAttributesDexterity;
        case GBA_VITALITY: return pHireling->flAttributesVitality;
        case GBA_INTELLIGENCE: return pHireling->flAttributesIntelligence;
        case GBA_HIRELING_LIGHT_LOOK_LEVEL: return pHireling->flAttributesHirelingLightLookLevel;
        case GBA_HIRELING_MEDIUM_LOOK_LEVEL: return pHireling->flAttributesHirelingMediumLookLevel;
        case GBA_HIRELING_HEAVY_LOOK_LEVEL: return pHireling->flAttributesHirelingHeavyLookLevel;
        default: return 0.0f;
        }
    }
    if (eType == GB_HEROS) {
        const HeroData * pChar = (const HeroData *)pGBData;
        switch (eGBAttribute) {
        case GBA_HITPOINTS: return pChar->flAttributesHitPoints;
        case GBA_HITPOINTS_FACTOR_LEVEL: return pChar->flAttributesHitPointsFactorLevel;
        case GBA_HPREGEN: return pChar->flAttributesHPRegen;
        case GBA_PRIMARY_RESOURCE_BASE: return pChar->flAttributesPrimaryResourceBase;
        case GBA_PRIMARY_RESOURCE_FACTOR_LEVEL: return pChar->flAttributesPrimaryResourceFactorLevel;
        case GBA_PRIMARY_RESOURCE_REGEN: return pChar->flAttributesPrimaryResourceRegen;
        case GBA_SECONDARY_RESOURCE_BASE: return pChar->flAttributesSecondaryResourceBase;
        case GBA_SECONDARY_RESOURCE_FACTOR_LEVEL: return pChar->flAttributesSecondaryResourceFactorLevel;
        case GBA_SECONDARY_RESOURCE_REGEN: return pChar->flAttributesSecondaryResourceRegen;
        case GBA_DAMAGE: return pChar->flAttributesDamage;
        case GBA_CRIT_DAMAGE_CAP: return pChar->flAttributesCritDamageCap;
        case GBA_CRIT_PERCENT_BASE: return pChar->flAttributesCritPercentBase;
        case GBA_CRIT_PERCENT_CAP: return pChar->flAttributesCritPercentCap;
        case GBA_SPRINTSPEED: return pChar->flAttributesSprintSpeed;
        case GBA_ARMOR: return pChar->flAttributesArmor;
        case GBA_WALKSPEED: return pChar->flAttributesWalkSpeed;
        case GBA_RUNSPEED: return pChar->flAttributesRunSpeed;
        case GBA_PROJSPEED: return pChar->flAttributesProjSpeed;
        case GBA_RESIST_PHYSICAL: return pChar->flAttributesResistPhysical;
        case GBA_RESIST_FIRE: return pChar->flAttributesResistFire;
        case GBA_RESIST_LIGHTNING: return pChar->flAttributesResistLightning;
        case GBA_RESIST_COLD: return pChar->flAttributesResistCold;
        case GBA_RESIST_POISON: return pChar->flAttributesResistPoison;
        case GBA_RESIST_ARCANE: return pChar->flAttributesResistArcane;
        case GBA_RESIST_CHILL: return pChar->flAttributesResistChill;
        case GBA_RESIST_STUN: return pChar->flAttributesResistStun;
        case GBA_KNOCKBACK_WEIGHT: return pChar->flAttributesKnockbackWeight;
        case GBA_OOC_HEALTH_REGEN_PERCENT: return pChar->flAttributesOOCHealthRegenPercent;
        case GBA_OOC_MANA_REGEN_PERCENT: return pChar->flAttributesOOCManaRegenPercent;
        case GBA_POTION_DILUTION_DURATION: return pChar->flAttributesPotionDilutionDuration;
        case GBA_POTION_DILUTION_SCALAR: return pChar->flAttributesPotionDilutionScalar;
        case GBA_DUAL_WIELD_BOTH_ATTACK_CHANCE: return pChar->flAttributesDualWieldBothAttackChance;
        case GBA_FREEZE_CAPACITY: return pChar->flAttributesFreezeCapacity;
        case GBA_THAW_RATE: return pChar->flAttributesThawRate;
        case GBA_DODGE_RATING_BASE: return pChar->flAttributesDodgeRatingBase;
        case GBA_STRENGTH: return pChar->flAttributesStrength;
        case GBA_DEXTERITY: return pChar->flAttributesDexterity;
        case GBA_VITALITY: return pChar->flAttributesVitality;
        case GBA_INTELLIGENCE: return pChar->flAttributesIntelligence;
        case GBA_GETHIT_MAX_BASE: return pChar->flAttributesGetHitMaxBase;
        case GBA_GETHIT_MAX_PER_LEVEL: return pChar->flAttributesGetHitMaxPerLevel;
        case GBA_GETHIT_RECOVERY_BASE: return pChar->flAttributesGetHitRecoveryBase;
        case GBA_GETHIT_RECOVERY_PER_LEVEL: return pChar->flAttributesGetHitRecoveryPerLevel;
        case GBA_CLASS_DAMAGE_REDUCTION_PERCENT: return pChar->flAttributesClassDamageReductionPercent;
        case GBA_CLASS_DAMAGE_REDUCTION_PERCENT_PVP: return pChar->flAttributesClassDamageReductionPercentPVP;
        default: return 0.0f;
        }
    }
    DisplayInternalError(NULL, "", 3656, ERROR_INTERNAL_FATAL);
    ExitProgram(ERROR_INTERNAL_FATAL);
    return 0.0f;
}








float AffixGetFormulaValue(uint8 * pbAttributeFormula, int32 nAttributeFormulaSize, const ActorCommonData * ptACDOwner, GBID gbidItem, RNG * ptRNGOverride, uint32 uFlags, int32 nLegendaryItemLevelOverride) {
    BBECallbackData tCallback;
    InitBBECallbackData(&tCallback);
    tCallback.uFlags = uFlags;
    tCallback.nLegendaryItemLevelOverride = nLegendaryItemLevelOverride;
    tCallback.gbidItem = gbidItem;
    if (ptACDOwner)
        tCallback.idFastAttrib = ptACDOwner->idFastAttrib;
    float flRetVal;
    BOOL bSuccess = BBEEvaluate(pbAttributeFormula, nAttributeFormulaSize, BBEIdentifierEvaluate, BBEFunctions, NUM_BBEFUNCTIONS, ptRNGOverride, &tCallback, flRetVal);
    return flRetVal;
}

























float AffixGetFormulaMinValue(const AttributeSpecifier & tAttribute, const ActorCommonData * ptACDOwner, GBID gbidItem, RNG * ptRNGOverride, const ActorCommonData * ptACDItem) {
    uint32 uFlags = 4;
    int32 nLegendaryItemLvl = 0;
    if (ptACDItem) {
        nLegendaryItemLvl = ptACDItem->AttributesGetInt(ATTRIBUTE_ITEM_LEGENDARY_ITEM_LEVEL_OVERRIDE);
        if (nLegendaryItemLvl == 0 && ItemIsLegendary(gbidItem) && ItemIsCrafted(*ptACDItem)) {
            ScopedItemDefinitionLockR ptItem(gbidItem);
            nLegendaryItemLvl = ptItem->nItemLevel;
        }
        if (nLegendaryItemLvl > 0) {
            ScopedItemDefinitionLockR pdefItem(ptACDItem->hGB.gbid);
            if (pdefItem) {
                for (int32 i = 0; i < (int64)(pdefItem->serAttributes_Console.dwSize / sizeof(AttributeSpecifier)); i++) {
                    if (pdefItem->ptAttributes[i] == tAttribute) {
                        nLegendaryItemLvl += ItemIsAncientRank(*ptACDItem) ? 2 : 1;
                        break;
                    }
                }
            }
        }
        if (ItemGetAncientRank(*ptACDItem) == 2)
            uFlags = ItemAttributePrefersMinimum(gbidItem, tAttribute.eAttribute) ? 4 : 2;
    }
    return AffixGetFormulaValue(tAttribute.pbAttributeFormula, tAttribute.serAttributeFormula.dwSize, ptACDOwner, gbidItem, ptRNGOverride, uFlags, nLegendaryItemLvl);
}


























float AffixGetFormulaMaxValue(const AttributeSpecifier & tAttribute, const ActorCommonData * ptACDOwner, GBID gbidItem, RNG * ptRNGOverride, const ActorCommonData * ptACDItem) {
    uint32 uFlags = 2;
    int32 nLegendaryItemLvl = 0;
    if (ptACDItem) {
        nLegendaryItemLvl = ptACDItem->AttributesGetInt(ATTRIBUTE_ITEM_LEGENDARY_ITEM_LEVEL_OVERRIDE);
        if (nLegendaryItemLvl == 0 && ItemIsLegendary(gbidItem) && ItemIsCrafted(*ptACDItem)) {
            ScopedItemDefinitionLockR ptItem(gbidItem);
            nLegendaryItemLvl = ptItem->nItemLevel;
        }
        if (nLegendaryItemLvl > 0) {
            ScopedItemDefinitionLockR pdefItem(ptACDItem->hGB.gbid);
            if (pdefItem) {
                for (int32 i = 0; i < (int64)(pdefItem->serAttributes_Console.dwSize / sizeof(AttributeSpecifier)); i++) {
                    if (pdefItem->ptAttributes[i] == tAttribute) {
                        nLegendaryItemLvl += ItemIsAncientRank(*ptACDItem) ? 2 : 1;
                        break;
                    }
                }
            }
        }
        if (ItemGetAncientRank(*ptACDItem) == 2)
            uFlags = ItemAttributePrefersMinimum(gbidItem, tAttribute.eAttribute) ? 4 : 2;
    }
    return AffixGetFormulaValue(tAttribute.pbAttributeFormula, tAttribute.serAttributeFormula.dwSize, ptACDOwner, gbidItem, ptRNGOverride, uFlags, nLegendaryItemLvl);
}



























int32 BBEFunctionTranslate(const char * szIdentifier) {
    int32 nIndex = 0;
    while (BBEFunctions[nIndex].pfnEvalFunc != NULL) {
        const TFunctionEvaluateTable & tBBEFunction = BBEFunctions[nIndex];
        if (strcasecmp(tBBEFunction.szFunctionName, szIdentifier) == 0)
            return nIndex;
        nIndex++;
    }
    return -1;
}












int32 BBENumOperands(int32 nFunction) {
    return BBEFunctions[nFunction].nOperands;
}



float BBEFunctionMin(float a, float b, float c, RNG *, void * pData) {
    return Math::Min(a, b);
}





float BBEFunctionMax(float a, float b, float c, RNG *, void * pData) {
    return Math::Max(a, b);
}





float BBEFunctionPin(float a, float b, float c, RNG *, void * pData) {
    return Math::Pin(a, b, c);
}





float BBEFunctionRandRangeInt(float a, float b, float c, RNG * ptRNG, void * pData) {
    BBECallbackData * pCallbackData = (BBECallbackData *)pData;
    int32 min = Math::FloatToInt(a);
    int32 delta = Math::FloatToInt(b);
    if (pCallbackData) {
        if (pCallbackData->uFlags & (1 << BBE_FLAG_ALWAYS_MAX))
            return (float)(min + delta);
        if (pCallbackData->uFlags & (1 << BBE_FLAG_ALWAYS_MIN))
            return (float)min;
    }
    if (ptRNG == NULL)
        ptRNG = &SGameGlobalsGet().rng;
    float flReturnValue = (float)RandRange_Internal(*ptRNG, min, delta);
    if (pCallbackData) {
        if (pCallbackData->uFlags & (1 << BBE_FLAG_PRIMAL_MAX))
            return (float)(min + delta);
        if (pCallbackData->uFlags & (1 << BBE_FLAG_PRIMAL_MIN))
            return (float)min;
    }
    return flReturnValue;
}












































float BBEFunctionRandRangeIntMinMax(float a, float b, float c, RNG * ptRNG, void * pData) {
    if (b < a)
        return a;
    BBECallbackData * pCallbackData = (BBECallbackData *)pData;
    int32 nMin = Math::FloatToInt(a);
    int32 nMax = Math::FloatToInt(b);
    if (pCallbackData) {
        if (pCallbackData->uFlags & (1 << BBE_FLAG_ALWAYS_MAX))
            return (float)nMax;
        if (pCallbackData->uFlags & (1 << BBE_FLAG_ALWAYS_MIN))
            return (float)nMin;
    }
    int32 nDelta = nMax - nMin;
    if (ptRNG == NULL)
        ptRNG = &SGameGlobalsGet().rng;
    float flReturnValue = (float)RandRange_Internal(*ptRNG, nMin, nDelta);
    if (pCallbackData) {
        if (pCallbackData->uFlags & (1 << BBE_FLAG_PRIMAL_MAX))
            return (float)nMax;
        if (pCallbackData->uFlags & (1 << BBE_FLAG_PRIMAL_MIN))
            return (float)nMin;
    }
    return flReturnValue;
}































float BBEFunctionRandRangeFloat(float a, float b, float c, RNG * ptRNG, void * pData) {
    BBECallbackData * pCallbackData = (BBECallbackData *)pData;
    if (pCallbackData) {
        if (pCallbackData->uFlags & (1 << BBE_FLAG_ALWAYS_MAX))
            return a + b;
        if (pCallbackData->uFlags & (1 << BBE_FLAG_ALWAYS_MIN))
            return a;
    }
    if (ptRNG == NULL)
        ptRNG = &SGameGlobalsGet().rng;
    float flReturnValue = a + RandFloat_Internal(*ptRNG) * b;
    if (pCallbackData) {
        if (pCallbackData->uFlags & (1 << BBE_FLAG_PRIMAL_MAX))
            return a + b;
        if (pCallbackData->uFlags & (1 << BBE_FLAG_PRIMAL_MIN))
            return a;
    }
    return flReturnValue;
}
























float BBEFunctionRandRangeFloatMinMax(float a, float b, float c, RNG * ptRNG, void * pData) {
    if (b < a)
        return a;
    BBECallbackData * pCallbackData = (BBECallbackData *)pData;
    if (pCallbackData) {
        if (pCallbackData->uFlags & (1 << BBE_FLAG_ALWAYS_MAX))
            return b;
        if (pCallbackData->uFlags & (1 << BBE_FLAG_ALWAYS_MIN))
            return a;
    }
    float flDelta = b - a;
    if (ptRNG == NULL)
        ptRNG = &SGameGlobalsGet().rng;
    float flReturnValue = a + flDelta * RandFloat_Internal(*ptRNG);
    if (pCallbackData) {
        if (pCallbackData->uFlags & (1 << BBE_FLAG_PRIMAL_MAX))
            return b;
        if (pCallbackData->uFlags & (1 << BBE_FLAG_PRIMAL_MIN))
            return a;
    }
    return flReturnValue;
}




























float BBEFunctionFloor(float a, float b, float c, RNG *, void * pData) {
    return floorf(a);
}






static const float TUNING_CONSTANT_OFFSET = 6.0f;
static const float TUNING_CONSTANT_LOG_BASE = 4.0f;
static const float LOG_OF_TUNING_CONSTANT_BASE = logf(TUNING_CONSTANT_LOG_BASE);
static const float TUNING_CONSTANT_DELTA = logf(TUNING_CONSTANT_OFFSET + 1.0f) / LOG_OF_TUNING_CONSTANT_BASE;
float BBEFunctionDiminishing(float a, float b, float c, RNG *, void * pData) {
    float flScalar = a + TUNING_CONSTANT_OFFSET;
    if (flScalar < EPSILON)
        return 0.0f;
    flScalar = logf(flScalar) / LOG_OF_TUNING_CONSTANT_BASE - TUNING_CONSTANT_DELTA;
    float flResult = b + (c - b) * flScalar;
    return flResult;
}






float BBEFunctionPower(float a, float b, float c, RNG *, void * pData) {
    return powf(a, b);
}






float BBEFunctionTable(float a, float b, float c, RNG *, void * pData) {
    uint32 nEntryIndex = Math::FloatToInt(a);
    uint32 nValueIndex = Math::FloatToInt(b);
    float flValue = 0.0f;
    const GameBalanceDefinition * ptGameBalDef = PowerFormulaTableLockR();
    const PowerFormulaTable & tPowerFormulaTable = ptGameBalDef->tPowerFormulaTable;
    if (nEntryIndex < tPowerFormulaTable.serPowerFormulaTableEntries.dwSize / sizeof(PowerFormulaTableEntry) &&
        nValueIndex < 76)
        flValue = tPowerFormulaTable.ptPowerFormulaTableEntries[nEntryIndex].flValue[nValueIndex];
    PowerFormulaTableUnlockR(ptGameBalDef);
    return flValue;
}













float BBEFunctionLog(float a, float, float, RNG *, void *) {
    return logf(a);
}





float PowerGetFormulaValueAtLevel(AttribGroupID idFastAttrib, const ActorCommonData * ptACD, SNO snoPower, uint8 * pbFormula, int32 nFormulaSize, int32 nLevel) {
    BBECallbackData tCallback;
    InitBBECallbackData(&tCallback);
    tCallback.idFastAttrib = idFastAttrib;
    tCallback.nAttribParam = -1;
    tCallback.snoPower = snoPower;
    tCallback.nSkillLevel = nLevel;
    if (idFastAttrib != -1)
        tCallback.nLevel = Math::Max(1, FastAttribGetValueInt(idFastAttrib, ATTRIBUTE_LEVEL));
    else if (ptACD)
        tCallback.nLevel = Math::Max(1, ptACD->AttributesGetInt(ATTRIBUTE_LEVEL));
    else
        tCallback.nLevel = 1;
    float flRetVal;
    BOOL bSuccess = BBEEvaluate(pbFormula, nFormulaSize, BBEIdentifierEvaluate, BBEFunctions, NUM_BBEFUNCTIONS, NULL, &tCallback, flRetVal);
    return flRetVal;
}

















































float GameBalanceGetAttribute(const GBHandle & hGB, const GameBalanceAttribute eGBAttribute) {
    const GameBalanceDefinition * pdefGB;
    const void * pGBData = GBLockR(hGB, pdefGB, FALSE);
    float flRetval = sGameBalanceDefGetAttributes(pGBData, hGB.eType, eGBAttribute);
    return flRetval;
}

















































BOOL GBGetMovementStyleDef(const GBID gbidMovementStyle, MovementStyleDefinition & tMovementStyleDef) {
    if (gbidMovementStyle != GBID_NONE) {
        const GameBalanceDefinition * ptDefGameBalance;
        GBHandle hGB;
        hGB.Set(GB_MOVEMENT_STYLES, gbidMovementStyle);
        MovementStyleDefinition * ptDefStyle = (MovementStyleDefinition *)GBLockR(hGB, ptDefGameBalance, FALSE);
        if (ptDefStyle) {
            tMovementStyleDef = *ptDefStyle;
            return TRUE;
        }
    }
    SigmaMemoryClear(&tMovementStyleDef, sizeof(tMovementStyleDef));
    return FALSE;
}


































































































const GameBalanceDefinition * ScriptedAchievementEventsTableLockR() {
    return (const GameBalanceDefinition *)gptGroupDefGameBalance->LockRFast(
        GlobalSNOGet(GLOBALSNO_GB_SCRIPTED_ACHIEVEMENT_EVENTS));
}





void ScriptedAchievementEventsTableUnlockR(const GameBalanceDefinition * lpDef) {
}





void GameBalanceApplyHotfix(SNO snoGameBalance) {
    sLoadGBInfo(snoGameBalance, TRUE);
}











































BOOL GameBalanceGroupDef::CanReferenceOtherSNOs() const {
    return TRUE;
}

BOOL GameBalanceGroupDef::SNOPackDestinationIsVariable() const {
    return TRUE;
}
