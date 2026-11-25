// Original file: X/Common/Items.cpp
//
// Functions sit on their original line numbers (asserts embed __LINE__); gaps are blank lines.

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "Definitions/GameBalanceDefinition.h"
#include "Items.h"
#include "ActorInventory.h"
#include "ItemsSwig.h"
#include "ActorCommonData.h"
#include "Game.h"
#include "GameBalance.h"
#include "GlobalSNO.h"
#include "Attributes/FastAttribConstantsSwig.h"
#include "Attributes/FastAttribute.h"
#include "Definitions/GameBalanceDefinition.h"
#include "Definitions/GlobalDefinition.h"
#include "Crafting/ItemCrafting.h"
#include "GameTemplate.h"
#include "HirelingSwig.h"
#include "CosmeticItems.h"
#include "GameCommonData.h"
#include "../Client/CGame.h"
#include "Definitions/ActorDefinition.h"
#include "TagMaps/ActorTags.h"
#include "TagMaps/Tags.h"
#include "../Sigma/DataStructures/XGrowableMap.h"
#include "../Sigma/DataStructures/XChainedFixedBlockAllocator.h"
#include "../../Contrib/Contrib/blz/include/blz/unordered_map.h"
#include "../Sigma/DataStructures/DataStructuresExternal.h"
#include "Powers/PowersCommon.h"
#include "TagMaps/PowerTags.h"
#include "../Server/SGame.h"
#include "Protobuf/Items.pb.h"
#include "Player.h"
#include "../Server/SGameStats.h"
#include "Protobuf/OnlineService.pb.h"
#include "../../BlizzardCore/Include/Blizzard/Util/Hash.h"
#include "Definitions/StringListDefinition.h"
#include "../Sigma/LanguageRules.h"
#include "../Sigma/UnicodeText.h"
#include "../Sigma/RGBAColor.h"
#include "NephalemRiftCommon.h"
#include "UserInterface/UITextControl.h"
#include "../Sigma/Random.h"
#include "Crafting/ItemCraftingSwig.h"
#include "World.h"
RGBAColor FontGetColorForFontSpecifier(FontSpecifier eFS, BOOL fIgnorePlayerOverrides);  // X/Common/UserInterface/Font.cpp
BOOL ActorInventoryHasUniqueEquippedSocketablesEquipped(const ActorCommonData & tACDOwner, const GBID * gbidUniqueEquippedSocketables, int32 nUniqueEquippedSocketables, ACDID idACDSkipped, InventorySlotType eSkippedSlot);  /* X/Common/ActorInventory.cpp:13205 */  int32 ItemGetSocketedUniqueEquipped(const ActorCommonData & tACDItem, GBID * gbidUniqueEquippedSocketables, int32 nMaxUniqueEquippedSocketables);  // Items.cpp:4865
// X/Common/Definitions/GameBalanceDefinition.h:49 (TreasureClassDefinition.cpp declares its own).
enum {
    GB_INVALID = -1,
    GB_ITEMTYPES = 1,
    GB_AFFIXES = 8, GB_ITEM_DROP_TABLE = 25,
};
void FastAttribSetValueToDefault(AttribGroupID idAttribGroup, FastAttribKey tKey);  // X/Common/Attributes/FastAttribute.cpp:4401
// X/Common/Definitions/GameBalanceDefinition.h:234 (AffixType values).
enum {
    AT_INVALID = -1,
    AT_PREFIX = 0,
    AT_SUFFIX = 1,
    AT_INHERENT = 2,
    AT_CURSE_DEPRECATED = 3,
    AT_BLESSING_DEPRECATED = 4,
    AT_TITLE = 5,
    AT_QUALITY = 6,
    AT_IMMUNITY = 7,
    AT_RANDOM = 9,
    AT_ENCHANTMENT = 10,
    AT_ADD_SOCKET = 11,
    AT_NUM_TYPES = 12,
};
#include "Shell/CmdLine.h"
// X/Common/Items.h:326. Defined here: UIEnchantCommon.cpp still declares its own copy.
struct Affix;
struct ItemAffixSpecifier {  // size 0x50
    ItemAffixSpecifier();
    GBID gbidItem;  // 0x00
    const Affix * ptAffix;  // 0x08
    int32 nAffixLevel;  // 0x10
    ItemQualityLevel eIQL;  // 0x14
    AffixType eLastAffixType;  // 0x18
    uint32 dwFlags;  // 0x1c
    PlayerClass eTargetedPlayerClass;  // 0x20
    HirelingClass eTargetedHirelingClass;  // 0x24
    GBID arCurrentAffixes[6];  // 0x28
    int32 nCurrentAffixesCount;  // 0x40
    GBID gbidAllowedLegendaryAffixFamily;  // 0x44
    AncientRank eAncientRank;  // 0x48
};
static_assert(sizeof(ItemAffixSpecifier) == 0x50, "ItemAffixSpecifier");
BOOL PlayerAffectedByDurability(const ActorCommonData & tACDPlayer);  /* X/Common/Player.cpp:5096 */  void ActorInventoryMergeItemAttributes(ActorCommonData & tACDOwner, ActorCommonData & tACDItem);  /* X/Common/ActorInventory.cpp:12177 */  void VisualInventoryUpdateItem(ActorCommonData & tACDItem);  /* X/Common/VisualInventory.cpp:1236 */  void ACDInventoryEquipmentOrSkillChanged(ActorCommonData & tACD);  /* X/Common/ActorInventory.cpp:9033 */  DataID SACDInventoryAddToDirtyList(const ActorCommonData & tACDItem, ActorInventoryDatabaseAction eAction, InventorySlotType eCameFrom, InventorySlotType eDestination);  /* X/Common/ActorInventory.cpp */  void ItemDeleteAltAttribGroup(const ActorCommonData & tACDItem);  /* Items.cpp:7854 */  BOOL ItemIsSpecial(GBID gbidItem);  /* Items.cpp:5117 */
// X/Common/Items.h:223-319. Only Items.cpp reaches into these yet.
struct Item {  // size 0x38
    GBID gbid;  // 0x00
    uint32 dwFlags;  // 0x04
    int32 nItemLevel;  // 0x08
    Act eMinAct;  // 0x0c
    ItemType * ptItemType;  // 0x10
    int32 * pnDropWeights;  // 0x18
    int32 * pnVendorDropWeights;  // 0x20
    uint32 * pdwLabelMask;  // 0x28
    GBID gbidBaseItem;  // 0x30
    int32 nSeasonRequiredToDrop;  // 0x34
};
static_assert(sizeof(Item) == 0x38, "Item");
#include "Definitions/RecipeDefinition.h"
enum { AFFIXGROUP_INVALID = -1, AFFIXGROUP_MAJOR = 0, AFFIXGROUP_MINOR = 1, NUM_AFFIXGROUPS = 2 }; typedef int32 AffixGroup;  // X/Common/Definitions/GameBalanceDefinition.h
struct Affix {  // size 0x144
    GBID gbid;  // 0x00
    uint32 dwFlags;  // 0x04
    int32 nFrequencyWeight;  // 0x08
    int32 nClassFrequencyWeight[7];  // 0x0c
    int32 nHirelingFrequencyWeight[4];  // 0x28
    int32 nAffixLevelMin;  // 0x38
    int32 nAffixLevelMax;  // 0x3c
    GBID gbidAffixFamily0;  // 0x40
    GBID gbidAffixFamily1;  // 0x44
    PlayerClass ePlayerClass;  // 0x48
    GBID arAllowedTypes[24];  // 0x4c
    GBID arLegendaryAllowedTypes[24];  // 0xac
    GBID gbidExclusionCategory;  // 0x10c
    GBID arExcludedCategories[6];  // 0x110
    uint32 dwAllowedQualityLevels;  // 0x128
    AffixType eAffixType;  // 0x12c
    GBID gbidAssociatedAffix;  // 0x130
    ItemEffectType eItemEffectType;  // 0x134
    int32 nItemEffectLevel;  // 0x138
    GBID gbidConvertsTo;  // 0x13c
    AffixGroup eAffixGroup;  // 0x140
};
static_assert(sizeof(Affix) == 0x144, "Affix");
#include "UserInterface/UIToolTips.h"
struct CraftedLegendaryPowerInfo {  // size 0x8
    SNO snoPower;  // 0x00
    float flUsePowerValue;  // 0x04
};
#include "../Server/Actors/Actors.h"
struct CurrencyInfo {  // size 0x8
    GBID gbidItem;  // 0x00
    BOOL bAutoPickup;  // 0x04
};
#include "../Server/SItems.h"
// Declaration from X/Client/CPlayer.cpp:1686 (DWARF); shared CPlayer.h declaration.
ACDID LocalPlayerACDGet();




































struct ItemAppGlobals {  // size 0x1d8
    ItemType tRootItemType;  // 0x00
    int32 nItemTypes;  // 0x58
    ItemType * ptItemTypes;  // 0x60
    int32 nItems;  // 0x68
    Item * ptItems;  // 0x70
    int32 nAffixes;  // 0x78
    Affix * ptAffixes;  // 0x80
    AffixTableEntry ** ptCachedAffixTableEntyPtrs;  // 0x88
    BOOL bCachedAffixTableEntriesValid;  // 0x90
    XGrowableMap<int, int, Item *, Item *> mapItems;  // 0x98
    ChainedFixedBlockAllocator tLabelMaskAllocator;  // 0x118
    ChainedFixedBlockAllocator tDropWeightsAllocator;  // 0x148
    blz::unordered_map<int, int, blz::hash<int>, blz::equal_to<int>, blz::allocator<blz::pair<const int, int> > > tGBIDToCurrencyMap;  // 0x178
    blz::unordered_map<int, CurrencyInfo, blz::hash<int>, blz::equal_to<int>, blz::allocator<blz::pair<const int, CurrencyInfo> > > tCurrencyToCurrencyInfoMap;  // 0x198
    blz::unordered_map<int, CraftedLegendaryPowerInfo, blz::hash<int>, blz::equal_to<int>, blz::allocator<blz::pair<const int, CraftedLegendaryPowerInfo> > > tCraftedItemGBIDToPowerInfo;  // 0x1b8
};
static_assert(sizeof(ItemAppGlobals) == 0x1d8, "ItemAppGlobals");
static_assert(offsetof(ItemAppGlobals, mapItems) == 0x98, "mapItems");
static_assert(offsetof(ItemAppGlobals, tGBIDToCurrencyMap) == 0x178, "tGBIDToCurrencyMap");

// X/Common/Items.h:217, 321 (kept here: Items.h only forward-declares XGrowableMap).
typedef XGrowableMap<int, int, int, int> AlternateAttributeGroupMap;
struct ItemGlobals {  // size 0x80
    AlternateAttributeGroupMap mapAlternateAttributeGroups;  // 0x00
};
static_assert(sizeof(ItemGlobals) == 0x80, "ItemGlobals");
static const BasicDisplayItem ctBasicDisplayItems[] = { { ATTRIBUTE_DAMAGE_WEAPON_MIN_TOTAL, ATTRIBUTE_DAMAGE_WEAPON_MIN }, { ATTRIBUTE_DAMAGE_WEAPON_MAX_TOTAL, ATTRIBUTE_DAMAGE_WEAPON_MAX }, { ATTRIBUTE_ARMOR_ITEM_TOTAL, ATTRIBUTE_ARMOR_ITEM }, { ATTRIBUTE_ATTACKS_PER_SECOND_ITEM_TOTAL, ATTRIBUTE_ATTACKS_PER_SECOND_ITEM }, { ATTRIBUTE_BLOCK_CHANCE_ITEM_TOTAL, ATTRIBUTE_BLOCK_CHANCE_ITEM } };  /* Items.cpp:171 */ static const int32 scgNumBasicDisplayItems = sizeof(ctBasicDisplayItems) / sizeof(ctBasicDisplayItems[0]);  /* Items.cpp:180 */
// X/Common/Player.h (GameError values used here; the Player.h subset does not list them yet).
CRefString GetAttributeDescription(const AttribTooltipInfo & tInfo, const ActorCommonData & tACDItem, const AttribGroupID idFastAttrib, const PlayerClass ePlayerClass, ToolTipValueColoring eColoringType, ItemDescriptionColorOverride pfnColorOverride, void * ptColorOverrideData);  /* Items.cpp:11606 */ RGBAColor ColorFloatByZeroRelativeAttribute(const ActorCommonData & tACDItem, const AttribTooltipInfo & tInfo, AttribGroupID idFastAttrib);  /* Items.cpp:11791 */ CRefString ItemGetAttacksPerSecond(const ActorCommonData & tACD, const PlayerClass ePlayerClass, ItemDescriptionColorOverride pfnColorOverride, void * ptColorOverrideData);  /* Items.cpp:11851 */ namespace UISkills { BOOL ResolvePowerTag(const ActorCommonData * ptACD, AttribGroupID idFastAttrib, AttribGroupID, LPCSTR szTag, float & flResult, float *, CRefString &, SNO snoPower, int32 nLevel, void *, void *, FontSpecifier &); }  /* X/Common/UserInterface/UISkillsCommon.cpp:32 */ CRefString GetMinMaxAttributeDescription(const AttribTooltipInfo & tInfo, const AttribGroupID idFastAttribMin, const AttribGroupID idFastAttribMax, const PlayerClass ePlayerClass, LPCSTR pszAttributeFormatString, BOOL bAppendResourceRestriction, ToolTipValueColoring eColoringType);  /* Items.cpp:12856 */

#include "ActorCommonDataSwig.h"
#include "SNO/SymbolicValue.h"
#include "../../Contrib/Contrib/blz/include/blz/algo_sort.h"
#include "../../Contrib/Contrib/blz/include/blz/algo_search.h"
#include <float.h>
#include "Marker/Marker.h"
#include "../Sigma/MemoryPools.h"
#include "../Client/Battlenet/UI/Util/UIUtil.h"



// Items.cpp:152.
extern const InventorySlotType cgtCraftingPlayerInventorySlots[3] = { SLOT_PLAYER_BACKPACK, SLOT_PLAYER_SHARED_STASH, SLOT_NONE };

// X/Common/Definitions/GameBalanceDefinition.h:1333, 1475.
struct LegacyItemConversion {  // size 0x28
    GBHeader tHeader;  // 0x00
    GBID gbidOldItem;  // 0x18
    GBID gbidNewItem;  // 0x1c
    BOOL bConsoleIgnore;  // 0x20
    uint32 dwPad;  // 0x24
};
static_assert(sizeof(LegacyItemConversion) == 0x28, "LegacyItemConversion");
enum { GB_LEGACY_ITEM_CONVERSIONS = 45 };
typedef ScopedGBLockR<LegacyItemConversion, 45> ScopedLegacyItemConversionLockR;

struct ItemGetSocketedUniqueEquippedData {  // size 0x10
    GBID * gbidUniqueEquippedSocketables;  // 0x00
    int32 nMaxUniqueEquippedSocketables;  // 0x08
    int32 nUniqueEquippedSocketables;  // 0x0c
};

// Defined later in this file.
SNO ItemGetLegendaryPower(const ItemDefinition * pdefItem);  // Items.cpp:10518
BOOL ItemIsLegendary(GBID gbidItem);  // Items.cpp:5080
int32 ItemGetItemLevel(const GBID gbidItem);  // Items.cpp:3895
BOOL ItemTypeCanBeMagical(GBID gbidItemType);  // Items.cpp:5448
int32 ItemGetCurrentDurability(const ActorCommonData & tACDItem);  // Items.cpp:9013
int32 ItemGetMaxDurability(const ActorCommonData & tACDItem);  // Items.cpp:9022
BOOL ItemIsEquippableByPlayerOrHireling(GBID gbidItem);  // Items.cpp:9460
BOOL ItemIsEquippableByPlayerOrHireling(const ActorCommonData & tACDItem);  // Items.cpp:9448
float ItemGetDurabilityPercent(const ActorCommonData & tACDItem);  // Items.cpp:8921
ItemSellError ItemBaseCanBeSold(const GBHandle & hGB);  // Items.cpp:3939
WeaponClass GetWeaponClass(const GBID gbidItemLeft, const GBID gbidItemRight);  // ActorInventory.cpp:11233
static inline BOOL sItemTestItemTypeFlag(const GBID gbidItem, const int32 nBit);  // Items.cpp:3384
BOOL ItemIsSetItem(const GBID gbid);  // Items.cpp:9373
GBHandle ItemsGetItemTypeGB(const GBHandle hGBItem);  // Items.cpp:5409
BOOL ItemAttributePrefersMinimum(GBID gbidItem, Attrib eAttribute);  // Items.cpp:10575
BOOL ItemUsableByHireling(const GBID gbidItem, const HirelingClass eHirelingClass);  // Items.cpp:2074
ItemSellError ItemCanBeSold(const ActorCommonData & tACDItem, const ActorCommonData * ptACDVendor);  // Items.cpp:3963
uint32 ItemGetLockedFromTradeSecondsRemaining(const ActorCommonData & tACD);  // Items.cpp:5596
BOOL sCheckSocketBoundToACD(const InventoryLocation & tInvLoc, void * pCallBackData);  // Items.cpp:5642
BOOL ItemIsUniqueEquipped(const ActorCommonData & tACDItem);  // Items.cpp:5219
SNO ItemGetLegendaryPower(GBID gbidItem);  // Items.cpp:10556
void ItemModifyDurability(const ActorCommonData & tACDPlayer, ActorCommonData & tACDItem, int32 nDurabilityChange);  // Items.cpp:8729
void LootAddUnluckyAndAvengerTicks(ActorCommonData & tACD, uint32 uTicksToAdd);  // X/Server/Loot/Loot.cpp:2532
int32 GetCraftLevel(const CrafterType eType, const Player & tPlayer);  // X/Common/Crafting/Crafting.h
ImageHandle ACDInventoryGetImageHandleFromGBID(const GBID gbidItem, const int32 eDrawType, const PlayerClass eClass, const BOOL fIsMale);  // X/Client/CActorInventory.cpp:79 (eDrawType is an ItemDrawType)
GBID CurrencyGetGBID(CurrencyType eCurrencyType);  // Items.cpp:10429
void ServerQueueClientUpdate();  // X/Server/Network/Server.cpp:438
BOOL ItemCanExtractLegendaryPower(GBID gbidItem);  // Items.cpp:10656
BOOL ItemIsHoradricReagent(const ActorCommonData & tACDItem);  // Items.cpp:9678
BOOL ItemIsHoradricReagent(GBID gbidItem);  // Items.cpp:9690
BOOL ItemIsGlyph(const ActorCommonData & tACDItem);  // Items.cpp:4252
BOOL ItemIsHealthPotion(const ActorCommonData & tACDItem);  // Items.cpp:4290
BOOL ACDInventorySlotIsEquippable(InventorySlotType eSlot);  // ActorInventory.cpp:10928
BOOL ItemIsEquippedButDisabled(const ActorCommonData & tACDItem);  // Items.cpp:8690
GameError ItemCanDrop(const ActorCommonData & tACDItem);  // Items.cpp:9360
ACDID ACDCreate();  // ActorCommonData.cpp:518
static inline int32 sGetAffixLevelRequirement(const ActorCommonData & tACDItem, GBID gbidAffix);  // Items.cpp:6522
uint32 ItemSocketCountFromGenerator(const D3::Items::Generator & tGenerator);  // Items.cpp:7820
BOOL ItemGenerateCommon(ActorCommonData & tACD, const D3::Items::Generator & tGenerator, ItemAffixFastAttribMap * arAffixSpecificAttributes);  // Items.cpp:6942
BOOL ItemIsEquippedButDisabledDuplicateLegendary(const ActorCommonData & tACDItem);  // Items.cpp:8705
CurrencyType CurrencyFromGBID(GBID gbid);  // Items.cpp:10444
PlayerClass PlayerGetPlayerClass(const Player * ptPlayer);  // X/Common/Player.cpp:2318
namespace Tutorial { BOOL SendTutorialToClient(SNO snoTutorial, Player * ptPlayer, const char * szUIAnchorName, const ACDID idACDTarget); }  // X/Server/Tutorial
// X/Common/Player.h:531 (not in the Player.h subset yet).
enum DataPersistFlags {
    DPF_DIRTY_PERSIST = 0,
    DPF_DIRTY_CLIENTSYNC = 1,
    DPF_PERSIST_IN_PROGRESS = 2,
    DPF_PERSISTED = 3,
};
BOOL ItemPlayerHasRequiredCrafterLevelForItemType(const ActorCommonData & tACDPlayer, const ActorCommonData & tACDItem);  // Items.cpp:8201
BOOL ItemGetLegendaryPowerInfoFromCraftedItemGBID(GBID gbid, CraftedLegendaryPowerInfo & tInfo);  // Items.cpp:10720
CRefString ItemGetFlavorText(const GBID gbidItem);  // Items.cpp:9920
static void sAddAttributesForAffix(ActorCommonData & tACDItem, GBID gbidAffix, BOOL bSimulateOnly, ItemAffixFastAttribMap * arAffixSpecificAttributes);  /* Items.cpp:6547 */ const Item * ItemsGetItem(GBID gbidItem); GBID ItemGetEnchantedAffixOld(const ActorCommonData & tACDItem); uint32 ItemGetEnchantedAffixSeed(const ActorCommonData & tACDItem); BOOL ItemTypeAllowedForAffix(const ItemType * pItemType, const GBID * arAllowedTypes, const GBID * arLegendaryAllowedTypes, BOOL bAllowLegendaryTypes); static void sFastAttribAggregrateAttributesFromSpecifiers(const ActorCommonData & tACD, AttribGroupID idFastAttrib, const AttributeSpecifier * ptAttributes, int32 nNumAttributes, const GBID gbidItem, const ActorCommonData * ptACDOwner, RNG * ptRNG, BOOL bSimulateOnly, BOOL bUprankLegendaryItemLevel);  /* Items.cpp:4942 */ DyeType DyeTypeTranslateFromSavedDyeType(const SavedDyeType eSavedDye); BOOL ItemIsAncientRank(const ActorCommonData & tACDItem); AncientRank ItemGetAncientRank(const ActorCommonData & tACDItem); uint32 ItemGetTradeTimeLeft(const ActorCommonData & tACDItem);  /* Items.cpp:10344 */ void ItemComputeConsoleItemId(const ActorCommonData & tItemACD, D3::OnlineService::ItemId & tId);  /* Items.cpp:10332 */ void ItemGeneratorPopulateFromACDID(const ActorCommonData & tACDItem, D3::Items::Generator & tGenerator, BOOL bSetRestoredBit); SavedDyeType DyeTypeTranslateToSavedDyeType(const DyeType eDye);  /* Definitions/GlobalDefinition.cpp */ static BOOL sResolveSocketTag(const ActorCommonData * ptACDItem, AttribGroupID, AttribGroupID, LPCSTR szTag, float & flResult, float *, CRefString &, SNO, int32, void * pData, void *, FontSpecifier &); BOOL ItemResolveValueTagForItemPassivePowerDescription(const ActorCommonData *, AttribGroupID idFastAttribMin, AttribGroupID idFastAttribMax, LPCSTR szTag, float & flResultMin, float * pflResultMax, CRefString &, SNO snoPower, int32, void * pData, void *, FontSpecifier &);  static int32 saEmptyItemDropWeights[110];  /* Items.cpp:98 */ struct ItemDropTableEntry; static void sBuildCurrencyMaps(ItemAppGlobals & tItemAppGlobals); static void sItemsBuildTypeTree(ItemAppGlobals & tItemAppGlobals); static void sBuildAffixList(ItemAppGlobals & tItemAppGlobals); static void sBuildCraftedItemGBIDToPowerSNOMap(ItemAppGlobals & tItemAppGlobals); static void sCopyItemTypes(ItemAppGlobals & tItemAppGlobals, ItemType & tRootItemType, ItemType * ptItemTypes, GBHandleList & lstItemTypeDefs); static void sFindMemberItems(ItemType * ptItemType, Item * ptItems, GBHandleList & lstItemDefs, GBHandleList & lstItemTypeDefs);  
// ---- Items.cpp:98-669: the file's globals. They sit here, not on their original lines, because
// ---- the local copies of header declarations above take lines 1-298. Rows generated by
// ---- build/try/agentC/items_gen.py from the original static initializer.
#include "../Client/Gfx/GfxVB.h"
#include "D3TypeDescriptor/D3TypeDescriptor.h"
#include "../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../Sigma/TypeDescriptor/TypeDescriptorField.h"
#include "../../Contrib/Contrib/blz/include/blz/map.h"
typedef blz::map<int, int, blz::less<int>, blz::allocator<blz::pair<const int, int> > > DyeTypeToGBIDMap;  // Items.cpp:28

// RareItemName is declared in Items.h.
// 2.7.7: UnlockableRandomAffixProperties (built right after RareItemName, 0x7100510c70) is declared in Networking/GameMessage.h.
#include "Networking/GameMessage.h"




static_assert(sizeof(RareItemName) == 0x10, "RareItemName");

// line 105
static const TypeDescriptorField RareItemName_Fields[] = {
    BoundedTypeDescriptorField("", DT_INT, offsetof(RareItemName, fItemNameIsPrefix), NULL, (const void *)0, (const void *)1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(RareItemName, snoAffixStringList), "", STRINGLIST_SNO, 0, -1),
    TypeDescriptorField("", DT_INT, offsetof(RareItemName, nAffixStringListIndex), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(RareItemName, nItemStringListIndex), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(RareItemName), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_RareItemName("RareItemName", 0, RareItemName_Fields, 5, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_RARE_ITEM_NAME = &TypeDescriptor_RareItemName;
static const TypeDescriptorField UnlockableRandomAffixProperties_Fields[] = { TypeDescriptorField("", DT_INT, offsetof(UnlockableRandomAffixProperties, unk_00), NULL, 1), BoundedTypeDescriptorField("", DT_INT, offsetof(UnlockableRandomAffixProperties, unk_04), NULL, NULL, (const void *)1), TypeDescriptorField(NULL, DT_NULL, sizeof(UnlockableRandomAffixProperties), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_UnlockableRandomAffixProperties("UnlockableRandomAffixProperties", 0, UnlockableRandomAffixProperties_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_UNLOCKABLE_RANDOM_AFFIX_PROPERTIES = &TypeDescriptor_UnlockableRandomAffixProperties;
// line 112
static const TypeDescriptorField EnchantAffixChoice_Fields[] = {
    TypeDescriptorField("", DT_GBID, offsetof(EnchantAffixChoice, gbidAffix), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(EnchantAffixChoice, dwSeed), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(EnchantAffixChoice), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_EnchantAffixChoice("EnchantAffixChoice", 0, EnchantAffixChoice_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_ENCHANT_AFFIX_CHOICE_DATA = &TypeDescriptor_EnchantAffixChoice;

// lines 131-137
LPCSTR guszItemLabelRandomProps = "Random_Props";
LPCSTR guszItemLabelRandomPropsRange = "Random_Props_Range";
LPCSTR guszItemLabelEmptySocket = "EmptySocket";
LPCSTR guszItemLabelTransmogrification = "Transmogrification";
LPCSTR guszItemLabelDurability = "Durability";
LPCSTR guszItemLabelEnchantedAffix = "EnchantedAffix";
LPCSTR guszItemLabelTieredLootRunKey = "TieredLootRunKey";

// line 139
LPCSTR garItemLabelsUsedByCode[] =
{
    guszItemLabelRandomProps,
    guszItemLabelRandomPropsRange,
    guszItemLabelEmptySocket,
    guszItemLabelTransmogrification,
    guszItemLabelDurability,
    guszItemLabelEnchantedAffix,
    guszItemLabelTieredLootRunKey,
    NULL,
};

// line 192
RGBAColor srgbaConsoleTooltip(0xd0, 0x9b, 0x46);

// line 194
LPCSTR sgtRangeStrings[RT_COUNT] =
{
    "{VALUE}",
    "[{VALUE}*100]%",
    "{VALUE}-[{VALUE}+{VALUE2}]",
    "{VALUE}-{VALUE2}",
};

// line 202
LPCSTR sgtRangeStringsSmall[RT_COUNT] =
{
    "[{VALUE}|1|]",
    "[{VALUE}*100|1|]%",
    "[{VALUE}|1|]-[{VALUE}+{VALUE2}|1|]",
    "[{VALUE}|1|]-[{VALUE2}|1|]",
};

static BOOL sDontDisplayNegatives(const AttribTooltipInfo & tInfo, AttribGroupID idFastAttrib);  // Items.cpp:12780
static CRefString sDisplayItemBOEAttribute(const ActorCommonData & tACDItem, const AttribTooltipInfo & tInfo, CRefString & usAttributeFormatSpecifier);  // Items.cpp:12815
BOOL CheckFirstAttrib(const AttribTooltipInfo & tInfo, AttribGroupID idFastAttrib);  // Items.cpp:12836

// line 216 (rows on lines 219-372)
extern const AttribTooltipInfo ctAttribTooltipInfoPrimary[] =
{
    AttribTooltipInfo(StringListGetLabelHandle("Damage_Weapon_Bonus_Min#Physical"), RT_VALUE, TAT_BASIC, MakeAttribKey(ATTRIBUTE_DAMAGE_WEAPON_BONUS_MIN, 0), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Damage_Weapon_Bonus_Delta#Physical"), RT_VALUE, TAT_BASIC, MakeAttribKey(ATTRIBUTE_DAMAGE_WEAPON_BONUS_DELTA, 0), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Damage_Weapon_Bonus_Flat#Physical"), RT_VALUE, TAT_BASIC, MakeAttribKey(ATTRIBUTE_DAMAGE_WEAPON_BONUS_FLAT, 0), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Damage_Weapon_Bonus#Physical (pair)"), RT_MINDELTA, TAT_BASIC, MakeAttribKey(ATTRIBUTE_DAMAGE_WEAPON_BONUS_MIN_X1, 0), MakeAttribKey(ATTRIBUTE_DAMAGE_WEAPON_BONUS_DELTA_X1, 0), ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Damage#Physical (pair)"), RT_MINDELTA, TAT_BASIC, MakeAttribKey(ATTRIBUTE_DAMAGE_MIN, 0), MakeAttribKey(ATTRIBUTE_DAMAGE_DELTA, 0), ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Damage_Min#Physical"), RT_VALUE, TAT_BASIC, MakeAttribKey(ATTRIBUTE_DAMAGE_MIN, 0), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, MakeAttribKey(ATTRIBUTE_DAMAGE_DELTA, 0), FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Damage_Bonus_Min#Physical"), RT_VALUE, TAT_BASIC, MakeAttribKey(ATTRIBUTE_DAMAGE_BONUS_MIN, 0), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Damage_Delta#Physical"), RT_VALUE, TAT_BASIC, MakeAttribKey(ATTRIBUTE_DAMAGE_DELTA, 0), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, MakeAttribKey(ATTRIBUTE_DAMAGE_MIN, 0), FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("WeaponDamage#Cold (pair)"), RT_MINMAX, TAT_BASIC, MakeAttribKey(ATTRIBUTE_DAMAGE_WEAPON_MIN, 3), MakeAttribKey(ATTRIBUTE_DAMAGE_WEAPON_MAX, 3), ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("WeaponDamage#Fire (pair)"), RT_MINMAX, TAT_BASIC, MakeAttribKey(ATTRIBUTE_DAMAGE_WEAPON_MIN, 1), MakeAttribKey(ATTRIBUTE_DAMAGE_WEAPON_MAX, 1), ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("WeaponDamage#Lightning (pair)"), RT_MINMAX, TAT_BASIC, MakeAttribKey(ATTRIBUTE_DAMAGE_WEAPON_MIN, 2), MakeAttribKey(ATTRIBUTE_DAMAGE_WEAPON_MAX, 2), ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("WeaponDamage#Poison (pair)"), RT_MINMAX, TAT_BASIC, MakeAttribKey(ATTRIBUTE_DAMAGE_WEAPON_MIN, 4), MakeAttribKey(ATTRIBUTE_DAMAGE_WEAPON_MAX, 4), ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("WeaponDamage#Arcane (pair)"), RT_MINMAX, TAT_BASIC, MakeAttribKey(ATTRIBUTE_DAMAGE_WEAPON_MIN, 5), MakeAttribKey(ATTRIBUTE_DAMAGE_WEAPON_MAX, 5), ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("WeaponDamage#Holy (pair)"), RT_MINMAX, TAT_BASIC, MakeAttribKey(ATTRIBUTE_DAMAGE_WEAPON_MIN, 6), MakeAttribKey(ATTRIBUTE_DAMAGE_WEAPON_MAX, 6), ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Damage#Cold (pair)"), RT_MINDELTA, TAT_BASIC, MakeAttribKey(ATTRIBUTE_DAMAGE_MIN, 3), MakeAttribKey(ATTRIBUTE_DAMAGE_DELTA, 3), ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Damage#Fire (pair)"), RT_MINDELTA, TAT_BASIC, MakeAttribKey(ATTRIBUTE_DAMAGE_MIN, 1), MakeAttribKey(ATTRIBUTE_DAMAGE_DELTA, 1), ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Damage#Lightning (pair)"), RT_MINDELTA, TAT_BASIC, MakeAttribKey(ATTRIBUTE_DAMAGE_MIN, 2), MakeAttribKey(ATTRIBUTE_DAMAGE_DELTA, 2), ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Damage#Poison (pair)"), RT_MINDELTA, TAT_BASIC, MakeAttribKey(ATTRIBUTE_DAMAGE_MIN, 4), MakeAttribKey(ATTRIBUTE_DAMAGE_DELTA, 4), ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Damage#Arcane (pair)"), RT_MINDELTA, TAT_BASIC, MakeAttribKey(ATTRIBUTE_DAMAGE_MIN, 5), MakeAttribKey(ATTRIBUTE_DAMAGE_DELTA, 5), ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Damage#Holy (pair)"), RT_MINDELTA, TAT_BASIC, MakeAttribKey(ATTRIBUTE_DAMAGE_MIN, 6), MakeAttribKey(ATTRIBUTE_DAMAGE_DELTA, 6), ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Damage#Cold"), RT_VALUE, TAT_BASIC, MakeAttribKey(ATTRIBUTE_DAMAGE_MIN, 3), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, MakeAttribKey(ATTRIBUTE_DAMAGE_DELTA, 3), FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Damage#Fire"), RT_VALUE, TAT_BASIC, MakeAttribKey(ATTRIBUTE_DAMAGE_MIN, 1), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, MakeAttribKey(ATTRIBUTE_DAMAGE_DELTA, 1), FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Damage#Lightning"), RT_VALUE, TAT_BASIC, MakeAttribKey(ATTRIBUTE_DAMAGE_MIN, 2), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, MakeAttribKey(ATTRIBUTE_DAMAGE_DELTA, 2), FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Damage#Poison"), RT_VALUE, TAT_BASIC, MakeAttribKey(ATTRIBUTE_DAMAGE_MIN, 4), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, MakeAttribKey(ATTRIBUTE_DAMAGE_DELTA, 4), FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Damage#Arcane"), RT_VALUE, TAT_BASIC, MakeAttribKey(ATTRIBUTE_DAMAGE_MIN, 5), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, MakeAttribKey(ATTRIBUTE_DAMAGE_DELTA, 5), FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Damage#Holy"), RT_VALUE, TAT_BASIC, MakeAttribKey(ATTRIBUTE_DAMAGE_MIN, 6), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, MakeAttribKey(ATTRIBUTE_DAMAGE_DELTA, 6), FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Damage_Type_Percent_Bonus#Physical"), RT_PERCENT, TAT_BASIC, MakeAttribKey(ATTRIBUTE_DAMAGE_TYPE_PERCENT_BONUS, 0), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Damage_Type_Percent_Bonus#Cold"), RT_PERCENT, TAT_BASIC, MakeAttribKey(ATTRIBUTE_DAMAGE_TYPE_PERCENT_BONUS, 3), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Damage_Type_Percent_Bonus#Fire"), RT_PERCENT, TAT_BASIC, MakeAttribKey(ATTRIBUTE_DAMAGE_TYPE_PERCENT_BONUS, 1), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Damage_Type_Percent_Bonus#Lightning"), RT_PERCENT, TAT_BASIC, MakeAttribKey(ATTRIBUTE_DAMAGE_TYPE_PERCENT_BONUS, 2), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Damage_Type_Percent_Bonus#Poison"), RT_PERCENT, TAT_BASIC, MakeAttribKey(ATTRIBUTE_DAMAGE_TYPE_PERCENT_BONUS, 4), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Damage_Type_Percent_Bonus#Arcane"), RT_PERCENT, TAT_BASIC, MakeAttribKey(ATTRIBUTE_DAMAGE_TYPE_PERCENT_BONUS, 5), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Damage_Type_Percent_Bonus#Holy"), RT_PERCENT, TAT_BASIC, MakeAttribKey(ATTRIBUTE_DAMAGE_TYPE_PERCENT_BONUS, 6), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Damage_Dealt_Percent_Bonus#Physical"), RT_PERCENT, TAT_UTILITY, MakeAttribKey(ATTRIBUTE_DAMAGE_DEALT_PERCENT_BONUS, 0), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Damage_Dealt_Percent_Bonus#Cold"), RT_PERCENT, TAT_UTILITY, MakeAttribKey(ATTRIBUTE_DAMAGE_DEALT_PERCENT_BONUS, 3), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Damage_Dealt_Percent_Bonus#Fire"), RT_PERCENT, TAT_UTILITY, MakeAttribKey(ATTRIBUTE_DAMAGE_DEALT_PERCENT_BONUS, 1), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Damage_Dealt_Percent_Bonus#Lightning"), RT_PERCENT, TAT_UTILITY, MakeAttribKey(ATTRIBUTE_DAMAGE_DEALT_PERCENT_BONUS, 2), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Damage_Dealt_Percent_Bonus#Poison"), RT_PERCENT, TAT_UTILITY, MakeAttribKey(ATTRIBUTE_DAMAGE_DEALT_PERCENT_BONUS, 4), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Damage_Dealt_Percent_Bonus#Arcane"), RT_PERCENT, TAT_UTILITY, MakeAttribKey(ATTRIBUTE_DAMAGE_DEALT_PERCENT_BONUS, 5), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Damage_Dealt_Percent_Bonus#Holy"), RT_PERCENT, TAT_UTILITY, MakeAttribKey(ATTRIBUTE_DAMAGE_DEALT_PERCENT_BONUS, 6), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Damage_Weapon_Percent_Bonus"), RT_PERCENT, TAT_BASIC, MakeAttribKey(ATTRIBUTE_DAMAGE_WEAPON_PERCENT_BONUS, 0), ATTRIBUTE_NONE, ATTRIBUTE_NONE, sDontDisplayNegatives, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Damage_Weapon_Percent_All"), RT_PERCENT, TAT_BASIC, ATTRIBUTE_DAMAGE_WEAPON_PERCENT_ALL, ATTRIBUTE_NONE, ATTRIBUTE_NONE, sDontDisplayNegatives, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Strength"), RT_VALUE, TAT_BASIC, ATTRIBUTE_STRENGTH, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Dexterity"), RT_VALUE, TAT_BASIC, ATTRIBUTE_DEXTERITY, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Intelligence"), RT_VALUE, TAT_BASIC, ATTRIBUTE_INTELLIGENCE, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Vitality"), RT_VALUE, TAT_BASIC, ATTRIBUTE_VITALITY, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Strength_Bonus"), RT_VALUE, TAT_BASIC, ATTRIBUTE_STRENGTH_BONUS, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Dexterity_Bonus"), RT_VALUE, TAT_BASIC, ATTRIBUTE_DEXTERITY_BONUS, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Intelligence_Bonus"), RT_VALUE, TAT_BASIC, ATTRIBUTE_INTELLIGENCE_BONUS, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Vitality_Bonus"), RT_VALUE, TAT_BASIC, ATTRIBUTE_VITALITY_BONUS, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Strength_Item"), RT_VALUE, TAT_BASIC, ATTRIBUTE_STRENGTH_ITEM, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Dexterity_Item"), RT_VALUE, TAT_BASIC, ATTRIBUTE_DEXTERITY_ITEM, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Intelligence_Item"), RT_VALUE, TAT_BASIC, ATTRIBUTE_INTELLIGENCE_ITEM, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Vitality_Item"), RT_VALUE, TAT_BASIC, ATTRIBUTE_VITALITY_ITEM, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Resistance_All"), RT_VALUE, TAT_BASIC, ATTRIBUTE_RESIST_ALL, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Attacks_Per_Second_Item_Bonus"), RT_VALUE, TAT_BASIC, ATTRIBUTE_ATTACKS_PER_SECOND_ITEM_BONUS, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Attacks_Per_Second_Item_Percent"), RT_PERCENT, TAT_BASIC, ATTRIBUTE_ATTACKS_PER_SECOND_ITEM_PERCENT, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Attacks_Per_Second_Percent"), RT_PERCENT, TAT_BASIC, ATTRIBUTE_ATTACKS_PER_SECOND_PERCENT, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Hitpoints_Percent"), RT_PERCENT, TAT_BASIC, ATTRIBUTE_HITPOINTS_PERCENT, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Hitpoints_Max_Percent_Bonus_Item"), RT_PERCENT, TAT_BASIC, ATTRIBUTE_HITPOINTS_MAX_PERCENT_BONUS_ITEM, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Armor_Bonus_Item"), RT_VALUE, TAT_BASIC, ATTRIBUTE_ARMOR_BONUS_ITEM, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Armor_Item_Percent"), RT_PERCENT, TAT_BASIC, ATTRIBUTE_ARMOR_ITEM_PERCENT, ATTRIBUTE_NONE, ATTRIBUTE_NONE, sDontDisplayNegatives, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Casting_Speed_Bonus"), RT_VALUE, TAT_BASIC, ATTRIBUTE_CASTING_SPEED_BONUS, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Casting_Speed_Percent"), RT_PERCENT, TAT_BASIC, ATTRIBUTE_CASTING_SPEED_PERCENT, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Crit_Damage_Percent"), RT_PERCENT, TAT_BASIC, ATTRIBUTE_CRIT_DAMAGE_PERCENT, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Hitpoints_Max_Bonus"), RT_VALUE, TAT_BASIC, ATTRIBUTE_HITPOINTS_MAX_BONUS, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Hitpoints_Regen_Per_Second"), RT_VALUE, TAT_BASIC, ATTRIBUTE_HITPOINTS_REGEN_PER_SECOND, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Critical_Chance"), RT_PERCENT, TAT_BASIC, ATTRIBUTE_CRIT_PERCENT_BONUS_CAPPED, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Weapon_Crit_Chance"), RT_PERCENT, TAT_BASIC, ATTRIBUTE_WEAPON_CRIT_CHANCE, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Dodge_Chance_Bonus"), RT_PERCENT, TAT_BASIC, ATTRIBUTE_DODGE_CHANCE_BONUS, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Block_Chance_Bonus_Item"), RT_PERCENT, TAT_UTILITY, ATTRIBUTE_BLOCK_CHANCE_BONUS_ITEM, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Crushing_Blow_Proc_Chance"), RT_PERCENT, TAT_BASIC, ATTRIBUTE_CRUSHING_BLOW_PROC_CHANCE, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Damage_Percent_Reduction_Turns_Into_Heal"), RT_PERCENT, TAT_BASIC, ATTRIBUTE_DAMAGE_PERCENT_REDUCTION_TURNS_INTO_HEAL, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Damage_Percent_Reduction_From_Elites"), RT_PERCENT, TAT_BASIC, ATTRIBUTE_DAMAGE_PERCENT_REDUCTION_FROM_ELITES, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Damage_Percent_Reduction_From_Type#Physical"), RT_PERCENT, TAT_BASIC, MakeAttribKey(ATTRIBUTE_DAMAGE_PERCENT_REDUCTION_FROM_TYPE, 0), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Damage_Percent_Reduction_From_Type#Cold"), RT_PERCENT, TAT_BASIC, MakeAttribKey(ATTRIBUTE_DAMAGE_PERCENT_REDUCTION_FROM_TYPE, 3), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Damage_Percent_Reduction_From_Type#Fire"), RT_PERCENT, TAT_BASIC, MakeAttribKey(ATTRIBUTE_DAMAGE_PERCENT_REDUCTION_FROM_TYPE, 1), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Damage_Percent_Reduction_From_Type#Lightning"), RT_PERCENT, TAT_BASIC, MakeAttribKey(ATTRIBUTE_DAMAGE_PERCENT_REDUCTION_FROM_TYPE, 2), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Damage_Percent_Reduction_From_Type#Arcane"), RT_PERCENT, TAT_BASIC, MakeAttribKey(ATTRIBUTE_DAMAGE_PERCENT_REDUCTION_FROM_TYPE, 5), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Damage_Percent_Reduction_From_Type#Holy"), RT_PERCENT, TAT_BASIC, MakeAttribKey(ATTRIBUTE_DAMAGE_PERCENT_REDUCTION_FROM_TYPE, 6), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Damage_Percent_Reduction_From_Type#Poison"), RT_PERCENT, TAT_BASIC, MakeAttribKey(ATTRIBUTE_DAMAGE_PERCENT_REDUCTION_FROM_TYPE, 4), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Amplify_Damage_Type_Percent#Physical"), RT_PERCENT, TAT_BASIC, MakeAttribKey(ATTRIBUTE_AMPLIFYDAMAGE_TYPE_PERCENT, 0), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Amplify_Damage_Type_Percent#Cold"), RT_PERCENT, TAT_BASIC, MakeAttribKey(ATTRIBUTE_AMPLIFYDAMAGE_TYPE_PERCENT, 3), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Amplify_Damage_Type_Percent#Fire"), RT_PERCENT, TAT_BASIC, MakeAttribKey(ATTRIBUTE_AMPLIFYDAMAGE_TYPE_PERCENT, 1), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Amplify_Damage_Type_Percent#Lightning"), RT_PERCENT, TAT_BASIC, MakeAttribKey(ATTRIBUTE_AMPLIFYDAMAGE_TYPE_PERCENT, 2), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Amplify_Damage_Type_Percent#Arcane"), RT_PERCENT, TAT_BASIC, MakeAttribKey(ATTRIBUTE_AMPLIFYDAMAGE_TYPE_PERCENT, 5), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Amplify_Damage_Type_Percent#Holy"), RT_PERCENT, TAT_BASIC, MakeAttribKey(ATTRIBUTE_AMPLIFYDAMAGE_TYPE_PERCENT, 6), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Amplify_Damage_Type_Percent#Poison"), RT_PERCENT, TAT_BASIC, MakeAttribKey(ATTRIBUTE_AMPLIFYDAMAGE_TYPE_PERCENT, 4), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Amplify_Damage_Percent"), RT_PERCENT, TAT_BASIC, ATTRIBUTE_AMPLIFYDAMAGE_PERCENT, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Hitpoints_On_Hit"), RT_VALUE, TAT_BASIC, ATTRIBUTE_HITPOINTS_ON_HIT, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Damage_Percent_Bonus_Witchdoctor"), RT_PERCENT, TAT_BASIC, ATTRIBUTE_DAMAGE_PERCENT_BONUS_WITCHDOCTOR, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Damage_Percent_Bonus_Wizard"), RT_PERCENT, TAT_BASIC, ATTRIBUTE_DAMAGE_PERCENT_BONUS_WIZARD, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Steal_Health_Percent"), RT_PERCENT, TAT_BASIC, ATTRIBUTE_STEAL_HEALTH_PERCENT, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Spending_Resource_Heals_Percent#Mana"), RT_VALUE, TAT_BASIC, MakeAttribKey(ATTRIBUTE_SPENDING_RESOURCE_HEALS_PERCENT, 0), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Spending_Resource_Heals_Percent#Arcanum"), RT_VALUE, TAT_BASIC, MakeAttribKey(ATTRIBUTE_SPENDING_RESOURCE_HEALS_PERCENT, 1), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Spending_Resource_Heals_Percent#Spirit"), RT_VALUE, TAT_BASIC, MakeAttribKey(ATTRIBUTE_SPENDING_RESOURCE_HEALS_PERCENT, 3), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Spending_Resource_Heals_Percent#Fury"), RT_VALUE, TAT_BASIC, MakeAttribKey(ATTRIBUTE_SPENDING_RESOURCE_HEALS_PERCENT, 2), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Spending_Resource_Heals_Percent#Hatred"), RT_VALUE, TAT_BASIC, MakeAttribKey(ATTRIBUTE_SPENDING_RESOURCE_HEALS_PERCENT, 5), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Spending_Resource_Heals_Percent#Discipline"), RT_VALUE, TAT_BASIC, MakeAttribKey(ATTRIBUTE_SPENDING_RESOURCE_HEALS_PERCENT, 6), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Spending_Resource_Heals_Percent#Faith"), RT_VALUE, TAT_BASIC, MakeAttribKey(ATTRIBUTE_SPENDING_RESOURCE_HEALS_PERCENT, 7), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Spending_Resource_Heals_Percent#Essence"), RT_VALUE, TAT_BASIC, MakeAttribKey(ATTRIBUTE_SPENDING_RESOURCE_HEALS_PERCENT, 8), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Resource_Regen_Per_Second#Fury"), RT_VALUE, TAT_UTILITY, MakeAttribKey(ATTRIBUTE_RESOURCE_REGEN_PER_SECOND, 2), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Resource_Regen_Per_Second#Faith"), RT_VALUE, TAT_UTILITY, MakeAttribKey(ATTRIBUTE_RESOURCE_REGEN_PER_SECOND, 7), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Resource_Regen_Per_Second#Mana"), RT_VALUE, TAT_UTILITY, MakeAttribKey(ATTRIBUTE_RESOURCE_REGEN_PER_SECOND, 0), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Resource_Regen_Per_Second#Arcanum"), RT_VALUE, TAT_UTILITY, MakeAttribKey(ATTRIBUTE_RESOURCE_REGEN_PER_SECOND, 1), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Resource_Regen_Per_Second#Hatred"), RT_VALUE, TAT_UTILITY, MakeAttribKey(ATTRIBUTE_RESOURCE_REGEN_PER_SECOND, 5), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Resource_Regen_Per_Second#Discipline"), RT_VALUE, TAT_UTILITY, MakeAttribKey(ATTRIBUTE_RESOURCE_REGEN_PER_SECOND, 6), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Resource_Regen_Per_Second#Spirit"), RT_VALUE, TAT_UTILITY, MakeAttribKey(ATTRIBUTE_RESOURCE_REGEN_PER_SECOND, 3), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Resource_Regen_Per_Second#Essence"), RT_VALUE, TAT_UTILITY, MakeAttribKey(ATTRIBUTE_RESOURCE_REGEN_PER_SECOND, 8), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Resource_On_Crit#Mana"), RT_VALUE, TAT_UTILITY, MakeAttribKey(ATTRIBUTE_RESOURCE_ON_CRIT, 0), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Resource_On_Crit#Arcanum"), RT_VALUE, TAT_UTILITY, MakeAttribKey(ATTRIBUTE_RESOURCE_ON_CRIT, 1), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Resource_On_Crit#Spirit"), RT_VALUE, TAT_UTILITY, MakeAttribKey(ATTRIBUTE_RESOURCE_ON_CRIT, 3), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Resource_On_Crit#Fury"), RT_VALUE, TAT_UTILITY, MakeAttribKey(ATTRIBUTE_RESOURCE_ON_CRIT, 2), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Resource_On_Crit#Hatred"), RT_VALUE, TAT_UTILITY, MakeAttribKey(ATTRIBUTE_RESOURCE_ON_CRIT, 5), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Resource_On_Crit#Discipline"), RT_VALUE, TAT_UTILITY, MakeAttribKey(ATTRIBUTE_RESOURCE_ON_CRIT, 6), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Resource_On_Crit#Faith"), RT_VALUE, TAT_UTILITY, MakeAttribKey(ATTRIBUTE_RESOURCE_ON_CRIT, 7), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Resource_On_Crit#Essence"), RT_VALUE, TAT_UTILITY, MakeAttribKey(ATTRIBUTE_RESOURCE_ON_CRIT, 8), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
};
extern const int32 cgNumTooltipAttribsPrimary = sizeof(ctAttribTooltipInfoPrimary) / sizeof(ctAttribTooltipInfoPrimary[0]);  // line 390
// line 392 (rows on lines 394-582)
extern const AttribTooltipInfo ctAttribTooltipInfoSecondary[] =
{
    AttribTooltipInfo(StringListGetLabelHandle("Resistance#Physical"), RT_VALUE, TAT_BASIC, MakeAttribKey(ATTRIBUTE_RESISTANCE, 0), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Resistance#Cold"), RT_VALUE, TAT_BASIC, MakeAttribKey(ATTRIBUTE_RESISTANCE, 3), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Resistance#Fire"), RT_VALUE, TAT_BASIC, MakeAttribKey(ATTRIBUTE_RESISTANCE, 1), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Resistance#Lightning"), RT_VALUE, TAT_BASIC, MakeAttribKey(ATTRIBUTE_RESISTANCE, 2), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Resistance#Arcane"), RT_VALUE, TAT_BASIC, MakeAttribKey(ATTRIBUTE_RESISTANCE, 5), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Resistance#Holy"), RT_VALUE, TAT_BASIC, MakeAttribKey(ATTRIBUTE_RESISTANCE, 6), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Resistance#Poison"), RT_VALUE, TAT_BASIC, MakeAttribKey(ATTRIBUTE_RESISTANCE, 4), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Damage_Percent_Reduction_From_Ranged"), RT_PERCENT, TAT_BASIC, ATTRIBUTE_DAMAGE_PERCENT_REDUCTION_FROM_RANGED, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Damage_Percent_Reduction_From_Melee"), RT_PERCENT, TAT_BASIC, ATTRIBUTE_DAMAGE_PERCENT_REDUCTION_FROM_MELEE, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Damage_Percent_Bonus_Vs_Monster_Type#Undead"), RT_PERCENT, TAT_UTILITY, MakeAttribKey(ATTRIBUTE_DAMAGE_PERCENT_BONUS_VS_MONSTER_TYPE, 0), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Damage_Percent_Bonus_Vs_Monster_Type#Demon"), RT_PERCENT, TAT_UTILITY, MakeAttribKey(ATTRIBUTE_DAMAGE_PERCENT_BONUS_VS_MONSTER_TYPE, 1), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Damage_Percent_Bonus_Vs_Monster_Type#Beast"), RT_PERCENT, TAT_UTILITY, MakeAttribKey(ATTRIBUTE_DAMAGE_PERCENT_BONUS_VS_MONSTER_TYPE, 2), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Damage_Percent_Bonus_Vs_Monster_Type#Human"), RT_PERCENT, TAT_UTILITY, MakeAttribKey(ATTRIBUTE_DAMAGE_PERCENT_BONUS_VS_MONSTER_TYPE, 3), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Damage_Percent_Bonus_Vs_Monster_Type#Breakable"), RT_PERCENT, TAT_UTILITY, MakeAttribKey(ATTRIBUTE_DAMAGE_PERCENT_BONUS_VS_MONSTER_TYPE, 4), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Damage_Percent_Bonus_Vs_Monster_Type#Scenery"), RT_PERCENT, TAT_UTILITY, MakeAttribKey(ATTRIBUTE_DAMAGE_PERCENT_BONUS_VS_MONSTER_TYPE, 5), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Damage_Percent_Bonus_Vs_Monster_Type#Ally"), RT_PERCENT, TAT_UTILITY, MakeAttribKey(ATTRIBUTE_DAMAGE_PERCENT_BONUS_VS_MONSTER_TYPE, 6), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Damage_Percent_Bonus_Vs_Monster_Type#Helper"), RT_PERCENT, TAT_UTILITY, MakeAttribKey(ATTRIBUTE_DAMAGE_PERCENT_BONUS_VS_MONSTER_TYPE, 8), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Hitpoints_On_Kill"), RT_VALUE, TAT_BASIC, ATTRIBUTE_HITPOINTS_ON_KILL, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Item_Is_BOE"), RT_INVALID, TAT_UTILITY, ATTRIBUTE_ITEM_IS_BOE, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, sDisplayItemBOEAttribute, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Stun_Chance"), RT_PERCENT, TAT_UTILITY, ATTRIBUTE_STUN_CHANCE, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Damage_To_Mana"), RT_VALUE, TAT_UTILITY, ATTRIBUTE_DAMAGE_TO_MANA, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Poison_Length_Reduction"), RT_VALUE, TAT_UTILITY, ATTRIBUTE_POISON_LENGTH_REDUCTION, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Freeze_Length_Reduction"), RT_VALUE, TAT_UTILITY, ATTRIBUTE_FREEZE_LENGTH_REDUCTION, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Stun_Recovery"), RT_VALUE, TAT_UTILITY, ATTRIBUTE_STUN_RECOVERY, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Requirements_Ease_Percent"), RT_PERCENT, TAT_UTILITY, ATTRIBUTE_REQUIREMENTS_EASE_PERCENT, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Always_Hits"), RT_INVALID, TAT_UTILITY, ATTRIBUTE_ALWAYS_HITS, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Gold_Find"), RT_PERCENT, TAT_UTILITY, ATTRIBUTE_GOLD_FIND, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Immune#Physical"), RT_INVALID, TAT_UTILITY, MakeAttribKey(ATTRIBUTE_IMMUNITY, 0), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Immune#Cold"), RT_INVALID, TAT_UTILITY, MakeAttribKey(ATTRIBUTE_IMMUNITY, 3), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Immune#Fire"), RT_INVALID, TAT_UTILITY, MakeAttribKey(ATTRIBUTE_IMMUNITY, 1), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Immune#Lightning"), RT_INVALID, TAT_UTILITY, MakeAttribKey(ATTRIBUTE_IMMUNITY, 2), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Immune#Arcane"), RT_INVALID, TAT_UTILITY, MakeAttribKey(ATTRIBUTE_IMMUNITY, 5), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Immune#Holy"), RT_INVALID, TAT_UTILITY, MakeAttribKey(ATTRIBUTE_IMMUNITY, 6), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Immune#Poison"), RT_INVALID, TAT_UTILITY, MakeAttribKey(ATTRIBUTE_IMMUNITY, 4), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Light_Radius_Percent_Bonus"), RT_PERCENT, TAT_UTILITY, ATTRIBUTE_LIGHT_RADIUS_PERCENT_BONUS, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Magic_Find"), RT_PERCENT, TAT_UTILITY, ATTRIBUTE_MAGIC_FIND, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Gem_Attributes_Multiplier"), RT_PERCENT, TAT_LEGENDARY, ATTRIBUTE_GEM_ATTRIBUTES_MULTIPLIER, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, TRUE),
    AttribTooltipInfo(StringListGetLabelHandle("Stats_All_Bonus"), RT_VALUE, TAT_UTILITY, ATTRIBUTE_STATS_ALL_BONUS, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Steal_Mana_Percent"), RT_PERCENT, TAT_UTILITY, ATTRIBUTE_STEAL_MANA_PERCENT, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Thorns_Percent#Physical"), RT_PERCENT, TAT_UTILITY, MakeAttribKey(ATTRIBUTE_THORNS_PERCENT, 0), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Thorns_Percent#Cold"), RT_PERCENT, TAT_UTILITY, MakeAttribKey(ATTRIBUTE_THORNS_PERCENT, 3), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Thorns_Percent#Fire"), RT_PERCENT, TAT_UTILITY, MakeAttribKey(ATTRIBUTE_THORNS_PERCENT, 1), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Thorns_Percent#Lightning"), RT_PERCENT, TAT_UTILITY, MakeAttribKey(ATTRIBUTE_THORNS_PERCENT, 2), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Thorns_Percent#Arcane"), RT_PERCENT, TAT_UTILITY, MakeAttribKey(ATTRIBUTE_THORNS_PERCENT, 5), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Thorns_Percent#Holy"), RT_PERCENT, TAT_UTILITY, MakeAttribKey(ATTRIBUTE_THORNS_PERCENT, 6), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Thorns_Percent#Poison"), RT_PERCENT, TAT_UTILITY, MakeAttribKey(ATTRIBUTE_THORNS_PERCENT, 4), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Thorns_Fixed#Physical"), RT_VALUE, TAT_UTILITY, MakeAttribKey(ATTRIBUTE_THORNS_FIXED, 0), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Thorns_Fixed#Cold"), RT_VALUE, TAT_UTILITY, MakeAttribKey(ATTRIBUTE_THORNS_FIXED, 3), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Thorns_Fixed#Fire"), RT_VALUE, TAT_UTILITY, MakeAttribKey(ATTRIBUTE_THORNS_FIXED, 1), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Thorns_Fixed#Lightning"), RT_VALUE, TAT_UTILITY, MakeAttribKey(ATTRIBUTE_THORNS_FIXED, 2), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Thorns_Fixed#Arcane"), RT_VALUE, TAT_UTILITY, MakeAttribKey(ATTRIBUTE_THORNS_FIXED, 5), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Thorns_Fixed#Holy"), RT_VALUE, TAT_UTILITY, MakeAttribKey(ATTRIBUTE_THORNS_FIXED, 6), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Thorns_Fixed#Poison"), RT_VALUE, TAT_UTILITY, MakeAttribKey(ATTRIBUTE_THORNS_FIXED, 4), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Thorns_Has_Damage_Type#Physical"), RT_VALUE, TAT_UTILITY, MakeAttribKey(ATTRIBUTE_THORNS_HAS_DAMAGE_TYPE, 0), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Thorns_Has_Damage_Type#Cold"), RT_VALUE, TAT_UTILITY, MakeAttribKey(ATTRIBUTE_THORNS_HAS_DAMAGE_TYPE, 3), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Thorns_Has_Damage_Type#Fire"), RT_VALUE, TAT_UTILITY, MakeAttribKey(ATTRIBUTE_THORNS_HAS_DAMAGE_TYPE, 1), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Thorns_Has_Damage_Type#Lightning"), RT_VALUE, TAT_UTILITY, MakeAttribKey(ATTRIBUTE_THORNS_HAS_DAMAGE_TYPE, 2), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Thorns_Has_Damage_Type#Arcane"), RT_VALUE, TAT_UTILITY, MakeAttribKey(ATTRIBUTE_THORNS_HAS_DAMAGE_TYPE, 5), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Thorns_Has_Damage_Type#Holy"), RT_VALUE, TAT_UTILITY, MakeAttribKey(ATTRIBUTE_THORNS_HAS_DAMAGE_TYPE, 6), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Thorns_Has_Damage_Type#Poison"), RT_VALUE, TAT_UTILITY, MakeAttribKey(ATTRIBUTE_THORNS_HAS_DAMAGE_TYPE, 4), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Pierce_Chance"), RT_PERCENT, TAT_UTILITY, ATTRIBUTE_PIERCE_CHANCE, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Run_Speed_Duration"), RT_VALUE, TAT_UTILITY, ATTRIBUTE_RUN_SPEED_DURATION, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Item_Cost_Percent_Bonus"), RT_PERCENT, TAT_UTILITY, ATTRIBUTE_ITEM_COST_PERCENT_BONUS, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Faster_Healing_Percent"), RT_PERCENT, TAT_UTILITY, ATTRIBUTE_FASTER_HEALING_PERCENT, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("HighlySalvageable"), RT_INVALID, TAT_UTILITY, ATTRIBUTE_HIGHLY_SALVAGEABLE, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Resource_On_Hit#Mana"), RT_VALUE, TAT_UTILITY, MakeAttribKey(ATTRIBUTE_RESOURCE_ON_HIT, 0), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Resource_On_Hit#Arcanum"), RT_VALUE, TAT_UTILITY, MakeAttribKey(ATTRIBUTE_RESOURCE_ON_HIT, 1), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Resource_On_Hit#Spirit"), RT_VALUE, TAT_UTILITY, MakeAttribKey(ATTRIBUTE_RESOURCE_ON_HIT, 3), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Resource_On_Hit#Fury"), RT_VALUE, TAT_UTILITY, MakeAttribKey(ATTRIBUTE_RESOURCE_ON_HIT, 2), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Resource_On_Hit#Hatred"), RT_VALUE, TAT_UTILITY, MakeAttribKey(ATTRIBUTE_RESOURCE_ON_HIT, 5), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Resource_On_Hit#Discipline"), RT_VALUE, TAT_UTILITY, MakeAttribKey(ATTRIBUTE_RESOURCE_ON_HIT, 6), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Resource_On_Hit#Faith"), RT_VALUE, TAT_UTILITY, MakeAttribKey(ATTRIBUTE_RESOURCE_ON_HIT, 7), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Resource_On_Hit#Essence"), RT_VALUE, TAT_UTILITY, MakeAttribKey(ATTRIBUTE_RESOURCE_ON_HIT, 8), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Resource_On_Kill#Mana"), RT_VALUE, TAT_UTILITY, MakeAttribKey(ATTRIBUTE_RESOURCE_ON_KILL, 0), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Resource_On_Kill#Arcanum"), RT_VALUE, TAT_UTILITY, MakeAttribKey(ATTRIBUTE_RESOURCE_ON_KILL, 1), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Resource_On_Kill#Spirit"), RT_VALUE, TAT_UTILITY, MakeAttribKey(ATTRIBUTE_RESOURCE_ON_KILL, 3), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Resource_On_Kill#Fury"), RT_VALUE, TAT_UTILITY, MakeAttribKey(ATTRIBUTE_RESOURCE_ON_KILL, 2), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Resource_On_Kill#Hatred"), RT_VALUE, TAT_UTILITY, MakeAttribKey(ATTRIBUTE_RESOURCE_ON_KILL, 5), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Resource_On_Kill#Discipline"), RT_VALUE, TAT_UTILITY, MakeAttribKey(ATTRIBUTE_RESOURCE_ON_KILL, 6), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Resource_On_Kill#Faith"), RT_VALUE, TAT_UTILITY, MakeAttribKey(ATTRIBUTE_RESOURCE_ON_KILL, 7), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Resource_On_Kill#Essence"), RT_VALUE, TAT_UTILITY, MakeAttribKey(ATTRIBUTE_RESOURCE_ON_KILL, 8), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Resistance_Stun"), RT_VALUE, TAT_UTILITY, ATTRIBUTE_RESISTANCE_STUN, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Resistance_Root"), RT_VALUE, TAT_UTILITY, ATTRIBUTE_RESISTANCE_ROOT, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Resistance_Freeze"), RT_VALUE, TAT_UTILITY, ATTRIBUTE_RESISTANCE_FREEZE, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Resistance_StunRootFreeze"), RT_VALUE, TAT_UTILITY, ATTRIBUTE_RESISTANCE_STUNROOTFREEZE, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Experience_Bonus"), RT_VALUE, TAT_UTILITY, ATTRIBUTE_EXPERIENCE_BONUS, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Experience_Bonus_Percent"), RT_PERCENT, TAT_UTILITY, ATTRIBUTE_EXPERIENCE_BONUS_PCT, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Health_Globe_Bonus_Chance"), RT_PERCENT, TAT_UTILITY, ATTRIBUTE_HEALTH_GLOBE_BONUS_CHANCE, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Health_Globe_Bonus_Chance_Mult_Percent"), RT_PERCENT, TAT_UTILITY, ATTRIBUTE_HEALTH_GLOBE_BONUS_CHANCE_MULT_PERCENT, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Powerup_Globe_Bonus_Chance"), RT_PERCENT, TAT_UTILITY, ATTRIBUTE_POWERUP_GLOBE_BONUS_CHANCE, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Powerup_Globe_Bonus_Chance_Mult_Percent"), RT_PERCENT, TAT_UTILITY, ATTRIBUTE_POWERUP_GLOBE_BONUS_CHANCE_MULT_PERCENT, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Gold_PickUp_Radius"), RT_VALUE, TAT_UTILITY, ATTRIBUTE_GOLD_PICKUP_RADIUS, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("CrowdControl_Reduction"), RT_PERCENT, TAT_UTILITY, ATTRIBUTE_CROWD_CONTROL_REDUCTION, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Chill_Duration_Bonus_Percent"), RT_PERCENT, TAT_UTILITY, ATTRIBUTE_CHILL_DURATION_BONUS_PERCENT, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Freeze_Duration_Bonus_Percent"), RT_PERCENT, TAT_UTILITY, ATTRIBUTE_FREEZE_DURATION_BONUS_PERCENT, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Stun_Duration_Bonus_Percent"), RT_PERCENT, TAT_UTILITY, ATTRIBUTE_STUN_DURATION_BONUS_PERCENT, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Immobilize_Duration_Bonus_Percent"), RT_PERCENT, TAT_UTILITY, ATTRIBUTE_IMMOBILIZE_DURATION_BONUS_PERCENT, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Fear_Duration_Bonus_Percent"), RT_PERCENT, TAT_UTILITY, ATTRIBUTE_FEAR_DURATION_BONUS_PERCENT, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Blind_Duration_Bonus_Percent"), RT_PERCENT, TAT_UTILITY, ATTRIBUTE_BLIND_DURATION_BONUS_PERCENT, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Charm_Duration_Bonus_Percent"), RT_PERCENT, TAT_UTILITY, ATTRIBUTE_CHARM_DURATION_BONUS_PERCENT, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Slow_Duration_Bonus_Percent"), RT_PERCENT, TAT_UTILITY, ATTRIBUTE_SLOW_DURATION_BONUS_PERCENT, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("On_Hit_Fear_Proc_Chance"), RT_PERCENT, TAT_UTILITY, ATTRIBUTE_ON_HIT_FEAR_PROC_CHANCE, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("On_Hit_Stun_Proc_Chance"), RT_PERCENT, TAT_UTILITY, ATTRIBUTE_ON_HIT_STUN_PROC_CHANCE, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("On_Hit_Blind_Proc_Chance"), RT_PERCENT, TAT_UTILITY, ATTRIBUTE_ON_HIT_BLIND_PROC_CHANCE, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("On_Hit_Freeze_Proc_Chance"), RT_PERCENT, TAT_UTILITY, ATTRIBUTE_ON_HIT_FREEZE_PROC_CHANCE, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("On_Hit_Chill_Proc_Chance"), RT_PERCENT, TAT_UTILITY, ATTRIBUTE_ON_HIT_CHILL_PROC_CHANCE, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("On_Hit_Slow_Proc_Chance"), RT_PERCENT, TAT_UTILITY, ATTRIBUTE_ON_HIT_SLOW_PROC_CHANCE, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("On_Hit_Immobilize_Proc_Chance"), RT_PERCENT, TAT_UTILITY, ATTRIBUTE_ON_HIT_IMMOBILIZE_PROC_CHANCE, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("On_Hit_Knockback_Proc_Chance"), RT_PERCENT, TAT_UTILITY, ATTRIBUTE_ON_HIT_KNOCKBACK_PROC_CHANCE, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Weapon_On_Hit_Fear_Proc_Chance"), RT_PERCENT, TAT_UTILITY, ATTRIBUTE_WEAPON_ON_HIT_FEAR_PROC_CHANCE, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Weapon_On_Hit_Stun_Proc_Chance"), RT_PERCENT, TAT_UTILITY, ATTRIBUTE_WEAPON_ON_HIT_STUN_PROC_CHANCE, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Weapon_On_Hit_Blind_Proc_Chance"), RT_PERCENT, TAT_UTILITY, ATTRIBUTE_WEAPON_ON_HIT_BLIND_PROC_CHANCE, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Weapon_On_Hit_Freeze_Proc_Chance"), RT_PERCENT, TAT_UTILITY, ATTRIBUTE_WEAPON_ON_HIT_FREEZE_PROC_CHANCE, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Weapon_On_Hit_Chill_Proc_Chance"), RT_PERCENT, TAT_UTILITY, ATTRIBUTE_WEAPON_ON_HIT_CHILL_PROC_CHANCE, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Weapon_On_Hit_Slow_Proc_Chance"), RT_PERCENT, TAT_UTILITY, ATTRIBUTE_WEAPON_ON_HIT_SLOW_PROC_CHANCE, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Weapon_On_Hit_Immobilize_Proc_Chance"), RT_PERCENT, TAT_UTILITY, ATTRIBUTE_WEAPON_ON_HIT_IMMOBILIZE_PROC_CHANCE, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Weapon_On_Hit_Knockback_Proc_Chance"), RT_PERCENT, TAT_UTILITY, ATTRIBUTE_WEAPON_ON_HIT_KNOCKBACK_PROC_CHANCE, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Item_Durability_Percent_Bonus"), RT_PERCENT, TAT_UTILITY, ATTRIBUTE_ITEM_DURABILITY_PERCENT_BONUS, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Item_Indestructible"), RT_INVALID, TAT_UTILITY, ATTRIBUTE_ITEM_INDESTRUCTIBLE, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Health_Globe_Bonus_Health"), RT_VALUE, TAT_BASIC, ATTRIBUTE_HEALTH_GLOBE_BONUS_HEALTH, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Item_Level_Requirement_Reduction"), RT_VALUE, TAT_UTILITY, ATTRIBUTE_ITEM_LEVEL_REQUIREMENT_REDUCTION, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Item_Level_Requirement_Override"), RT_VALUE, TAT_LEGENDARY, ATTRIBUTE_ITEM_LEVEL_REQUIREMENT_OVERRIDE, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
};
extern const int32 cgNumTooltipAttribsSecondary = sizeof(ctAttribTooltipInfoSecondary) / sizeof(ctAttribTooltipInfoSecondary[0]);  // line 594
// line 597 (rows on lines 599-606)
extern const AttribTooltipInfo ctAttribTooltipInfoLegendary[] =
{
    AttribTooltipInfo(StringListGetLabelHandle("Potion_Bonus_Armor_Percent"), RT_PERCENT, TAT_LEGENDARY, ATTRIBUTE_POTION_BONUS_ARMOR_PERCENT, ATTRIBUTE_POTION_BONUS_BUFF_DURATION, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Potion_Bonus_Resist_All"), RT_VALUE, TAT_LEGENDARY, ATTRIBUTE_POTION_BONUS_RESIST_ALL, ATTRIBUTE_POTION_BONUS_BUFF_DURATION, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Potion_Bonus_Hitpoints_Granted"), RT_VALUE, TAT_LEGENDARY, ATTRIBUTE_POTION_BONUS_HITPOINTS_GRANTED, ATTRIBUTE_POTION_BONUS_BUFF_DURATION, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Potion_Bonus_Life_On_Hit"), RT_VALUE, TAT_LEGENDARY, ATTRIBUTE_POTION_BONUS_LIFE_ON_HIT, ATTRIBUTE_POTION_BONUS_BUFF_DURATION, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Potion_Bonus_Life_On_Kill"), RT_VALUE, TAT_LEGENDARY, ATTRIBUTE_POTION_BONUS_LIFE_ON_KILL, ATTRIBUTE_POTION_BONUS_BUFF_DURATION, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Set_Item_Discount"), RT_VALUE, TAT_LEGENDARY, ATTRIBUTE_SET_ITEM_DISCOUNT, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, TRUE),
    AttribTooltipInfo(StringListGetLabelHandle("Health_Potion_Affects_Allies_Percent"), RT_PERCENT, TAT_LEGENDARY, ATTRIBUTE_HEALTH_POTION_AFFECTS_ALLIES_PERCENT, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
};
extern const int32 cgNumTooltipAttribsLegendary = sizeof(ctAttribTooltipInfoLegendary) / sizeof(ctAttribTooltipInfoLegendary[0]);  // line 608
// line 611 (rows on lines 613-667)
extern const AttribTooltipInfo ctAttribTooltipInfoTrailing[] =
{
    AttribTooltipInfo(StringListGetLabelHandle("Power_Cooldown_Reduction_Percent_All"), RT_PERCENT, TAT_CONSOLE, ATTRIBUTE_POWER_COOLDOWN_REDUCTION_PERCENT_ALL, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Resource_Max_Percent_Bonus#Fury"), RT_PERCENT, TAT_CONSOLE, MakeAttribKey(ATTRIBUTE_RESOURCE_MAX_PERCENT_BONUS, 2), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Resource_Granted#Fury"), RT_VALUE, TAT_CONSOLE, MakeAttribKey(ATTRIBUTE_RESOURCE_GRANTED, 2), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Resource_Max_Bonus#Fury"), RT_VALUE, TAT_CONSOLE, MakeAttribKey(ATTRIBUTE_RESOURCE_MAX_BONUS, 2), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Resource_Cost_Reduction_Amount#Fury"), RT_VALUE, TAT_CONSOLE, MakeAttribKey(ATTRIBUTE_RESOURCE_COST_REDUCTION_AMOUNT, 2), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Resource_Max_Percent_Bonus#Faith"), RT_PERCENT, TAT_CONSOLE, MakeAttribKey(ATTRIBUTE_RESOURCE_MAX_PERCENT_BONUS, 7), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Resource_Granted#Faith"), RT_VALUE, TAT_CONSOLE, MakeAttribKey(ATTRIBUTE_RESOURCE_GRANTED, 7), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Resource_Max_Bonus#Faith"), RT_VALUE, TAT_CONSOLE, MakeAttribKey(ATTRIBUTE_RESOURCE_MAX_BONUS, 7), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Resource_Cost_Reduction_Amount#Faith"), RT_VALUE, TAT_CONSOLE, MakeAttribKey(ATTRIBUTE_RESOURCE_COST_REDUCTION_AMOUNT, 7), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Resource_Max_Percent_Bonus#Mana"), RT_PERCENT, TAT_CONSOLE, MakeAttribKey(ATTRIBUTE_RESOURCE_MAX_PERCENT_BONUS, 0), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Resource_Granted#Mana"), RT_VALUE, TAT_CONSOLE, MakeAttribKey(ATTRIBUTE_RESOURCE_GRANTED, 0), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Resource_Max_Bonus#Mana"), RT_VALUE, TAT_CONSOLE, MakeAttribKey(ATTRIBUTE_RESOURCE_MAX_BONUS, 0), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Resource_Cost_Reduction_Amount#Mana"), RT_VALUE, TAT_CONSOLE, MakeAttribKey(ATTRIBUTE_RESOURCE_COST_REDUCTION_AMOUNT, 0), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Resource_Max_Percent_Bonus#Arcanum"), RT_PERCENT, TAT_CONSOLE, MakeAttribKey(ATTRIBUTE_RESOURCE_MAX_PERCENT_BONUS, 1), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Resource_Granted#Arcanum"), RT_VALUE, TAT_CONSOLE, MakeAttribKey(ATTRIBUTE_RESOURCE_GRANTED, 1), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Resource_Max_Bonus#Arcanum"), RT_VALUE, TAT_CONSOLE, MakeAttribKey(ATTRIBUTE_RESOURCE_MAX_BONUS, 1), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Resource_Cost_Reduction_Amount#Arcanum"), RT_VALUE, TAT_CONSOLE, MakeAttribKey(ATTRIBUTE_RESOURCE_COST_REDUCTION_AMOUNT, 1), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Resource_Max_Percent_Bonus#Hatred"), RT_PERCENT, TAT_CONSOLE, MakeAttribKey(ATTRIBUTE_RESOURCE_MAX_PERCENT_BONUS, 5), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Resource_Granted#Hatred"), RT_VALUE, TAT_CONSOLE, MakeAttribKey(ATTRIBUTE_RESOURCE_GRANTED, 5), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Resource_Max_Bonus#Hatred"), RT_VALUE, TAT_CONSOLE, MakeAttribKey(ATTRIBUTE_RESOURCE_MAX_BONUS, 5), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Resource_Cost_Reduction_Amount#Hatred"), RT_VALUE, TAT_CONSOLE, MakeAttribKey(ATTRIBUTE_RESOURCE_COST_REDUCTION_AMOUNT, 5), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Resource_Max_Percent_Bonus#Discipline"), RT_PERCENT, TAT_CONSOLE, MakeAttribKey(ATTRIBUTE_RESOURCE_MAX_PERCENT_BONUS, 6), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Resource_Granted#Discipline"), RT_VALUE, TAT_CONSOLE, MakeAttribKey(ATTRIBUTE_RESOURCE_GRANTED, 6), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Resource_Max_Bonus#Discipline"), RT_VALUE, TAT_CONSOLE, MakeAttribKey(ATTRIBUTE_RESOURCE_MAX_BONUS, 6), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Resource_Cost_Reduction_Amount#Discipline"), RT_VALUE, TAT_CONSOLE, MakeAttribKey(ATTRIBUTE_RESOURCE_COST_REDUCTION_AMOUNT, 6), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Resource_Max_Percent_Bonus#Spirit"), RT_PERCENT, TAT_CONSOLE, MakeAttribKey(ATTRIBUTE_RESOURCE_MAX_PERCENT_BONUS, 3), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Resource_Granted#Spirit"), RT_VALUE, TAT_CONSOLE, MakeAttribKey(ATTRIBUTE_RESOURCE_GRANTED, 3), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Resource_Max_Bonus#Spirit"), RT_VALUE, TAT_CONSOLE, MakeAttribKey(ATTRIBUTE_RESOURCE_MAX_BONUS, 3), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Resource_Cost_Reduction_Amount#Spirit"), RT_VALUE, TAT_CONSOLE, MakeAttribKey(ATTRIBUTE_RESOURCE_COST_REDUCTION_AMOUNT, 3), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Resource_Max_Percent_Bonus#Essence"), RT_PERCENT, TAT_CONSOLE, MakeAttribKey(ATTRIBUTE_RESOURCE_MAX_PERCENT_BONUS, 8), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Resource_Granted#Essence"), RT_VALUE, TAT_CONSOLE, MakeAttribKey(ATTRIBUTE_RESOURCE_GRANTED, 8), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Resource_Max_Bonus#Essence"), RT_VALUE, TAT_CONSOLE, MakeAttribKey(ATTRIBUTE_RESOURCE_MAX_BONUS, 8), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Resource_Cost_Reduction_Amount#Essence"), RT_VALUE, TAT_CONSOLE, MakeAttribKey(ATTRIBUTE_RESOURCE_COST_REDUCTION_AMOUNT, 8), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Resource_Set_Point_Bonus#Fury"), RT_VALUE, TAT_CONSOLE, MakeAttribKey(ATTRIBUTE_RESOURCE_SET_POINT_BONUS, 2), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Resource_Set_Point_Bonus#Faith"), RT_VALUE, TAT_CONSOLE, MakeAttribKey(ATTRIBUTE_RESOURCE_SET_POINT_BONUS, 7), ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Sockets"), RT_VALUE, TAT_CONSOLE, ATTRIBUTE_SOCKETS, ATTRIBUTE_SOCKETS_FILLED, ATTRIBUTE_NONE, CheckFirstAttrib, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Resource_Cost_Reduction_Percent_All"), RT_PERCENT, TAT_CONSOLE, ATTRIBUTE_RESOURCE_COST_REDUCTION_PERCENT_ALL, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Splash_Damage_Effect_Percent"), RT_PERCENT, TAT_CONSOLE, ATTRIBUTE_SPLASH_DAMAGE_EFFECT_PERCENT, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("On_Hit_Bleed_Proc_Chance"), RT_PERCENT, TAT_CONSOLE, ATTRIBUTE_ON_HIT_BLEED_PROC_CHANCE, ATTRIBUTE_ON_HIT_BLEED_PROC_DAMAGE_BASE, ATTRIBUTE_ON_HIT_BLEED_PROC_DAMAGE_DELTA, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Weapon_On_Hit_Bleed_Proc_Chance"), RT_PERCENT, TAT_CONSOLE, ATTRIBUTE_WEAPON_ON_HIT_BLEED_PROC_CHANCE, ATTRIBUTE_WEAPON_ON_HIT_BLEED_PROC_DAMAGE_BASE, ATTRIBUTE_WEAPON_ON_HIT_BLEED_PROC_DAMAGE_DELTA, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Weapon_On_Hit_Percent_Bleed_Proc_Chance"), RT_PERCENT, TAT_CONSOLE, ATTRIBUTE_WEAPON_ON_HIT_PERCENT_BLEED_PROC_CHANCE, ATTRIBUTE_WEAPON_ON_HIT_PERCENT_BLEED_PROC_DAMAGE, ATTRIBUTE_WEAPON_ON_HIT_PERCENT_BLEED_PROC_DURATION, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Damage_Percent_Bonus_Vs_Elites"), RT_PERCENT, TAT_CONSOLE, ATTRIBUTE_DAMAGE_PERCENT_BONUS_VS_ELITES, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
    AttribTooltipInfo(StringListGetLabelHandle("Movement_Speed"), RT_PERCENT, TAT_CONSOLE, ATTRIBUTE_MOVEMENT_SCALAR, ATTRIBUTE_NONE, ATTRIBUTE_NONE, NULL, NULL, ATTRIBUTE_NONE, FALSE),
};
extern const int32 cgNumTooltipAttribsTrailing = sizeof(ctAttribTooltipInfoTrailing) / sizeof(ctAttribTooltipInfoTrailing[0]);  // line 669




























































void ItemsInitializeForApp()

{
    AppGlobals & tAppGlobals = sgtAppGlobals;
    ItemAppGlobals *& ptItemAppGlobals = tAppGlobals.ptItemAppGlobals;



    ptItemAppGlobals = (ItemAppGlobals *)SigmaMemoryNew(sizeof(ItemAppGlobals), 4, NULL, TRUE);
    ptItemAppGlobals->tRootItemType.gbid = GBID_NONE;
    new (&ptItemAppGlobals->tRootItemType.arChildItemTypes) ItemTypePointerArray();
    ptItemAppGlobals->mapItems.Initialize(16, FALSE, NULL);
    ptItemAppGlobals->mapItems.SetHashTableSize_Tracked(256);
    ptItemAppGlobals->bCachedAffixTableEntriesValid = FALSE;


    SigmaMemoryClear(saEmptyItemDropWeights, sizeof(saEmptyItemDropWeights));




    new (&ptItemAppGlobals->tGBIDToCurrencyMap) blz::unordered_map<int, int>();
    new (&ptItemAppGlobals->tCurrencyToCurrencyInfoMap) blz::unordered_map<int, CurrencyInfo>();
    new (&ptItemAppGlobals->tCraftedItemGBIDToPowerInfo) blz::unordered_map<int, CraftedLegendaryPowerInfo>();

    sBuildCurrencyMaps(*ptItemAppGlobals);
    sItemsBuildTypeTree(*ptItemAppGlobals);
    sBuildAffixList(*ptItemAppGlobals);
    sBuildCraftedItemGBIDToPowerSNOMap(*ptItemAppGlobals);
}




void ItemsDisposeForApp()
{
    AppGlobals & tAppGlobals = sgtAppGlobals;
    ItemAppGlobals *& ptItemAppGlobals = tAppGlobals.ptItemAppGlobals;

    ptItemAppGlobals->tRootItemType.arChildItemTypes.~ItemTypePointerArray();
    SigmaMemoryFree(ptItemAppGlobals->ptItems, NULL);
    ptItemAppGlobals->ptItems = NULL;

    for (int32 i = 0; i < ptItemAppGlobals->nItemTypes; i++)
    {
        ptItemAppGlobals->ptItemTypes[i].arChildItemTypes.~ItemTypePointerArray();
        ptItemAppGlobals->ptItemTypes[i].arItems.~ItemPointerArray();
    }
    SigmaMemoryFree(ptItemAppGlobals->ptItemTypes, NULL);
    ptItemAppGlobals->ptItemTypes = NULL;

    ptItemAppGlobals->mapItems.Dispose();

    SigmaMemoryFree(ptItemAppGlobals->ptAffixes, NULL);
    ptItemAppGlobals->ptAffixes = NULL;
    SigmaMemoryFree(ptItemAppGlobals->ptCachedAffixTableEntyPtrs, NULL);
    ptItemAppGlobals->ptCachedAffixTableEntyPtrs = NULL;

    ptItemAppGlobals->tGBIDToCurrencyMap.~unordered_map();
    ptItemAppGlobals->tCurrencyToCurrencyInfoMap.~unordered_map();
    ptItemAppGlobals->tCraftedItemGBIDToPowerInfo.~unordered_map();

    SigmaMemoryFree(ptItemAppGlobals, NULL);
    ptItemAppGlobals = NULL;
}





void ItemsReinitialize()
{
    ItemsDisposeForApp();
    ItemsInitializeForApp();
}





void ItemsInitializeForGame()
{
    GameCommonData & tGCD = GameGetActiveGameCommonData();
    XMemoryAllocatorInterface * ptMemoryInterface = AppGetCurrentMemoryInterface();
    tGCD.ptItemGlobals = (ItemGlobals *)SigmaMemoryNew(sizeof(ItemGlobals), 4, ptMemoryInterface, TRUE);
    tGCD.ptItemGlobals->mapAlternateAttributeGroups.Initialize(16, FALSE, NULL);
}











void ItemsDisposeForGame()
{
    GameCommonData & tGCD = GameGetActiveGameCommonData();
    XMemoryAllocatorInterface * ptMemoryInterface = AppGetCurrentMemoryInterface();
    tGCD.ptItemGlobals->mapAlternateAttributeGroups.Dispose();
    SigmaMemoryFree(tGCD.ptItemGlobals, ptMemoryInterface);
    tGCD.ptItemGlobals = NULL;
}











ItemAffixSpecifier::ItemAffixSpecifier()
{
    gbidItem = GBID_NONE;
    ptAffix = NULL;
    nAffixLevel = 1;
    eIQL = IQL_INVALID;
    eLastAffixType = -1;
    dwFlags = 0;
    eTargetedPlayerClass = -1;
    eTargetedHirelingClass = 0;
    eAncientRank = 0;
    nCurrentAffixesCount = 0;
    gbidAllowedLegendaryAffixFamily = GBID_NONE;
    for (int32 i = 0; i < 6; i++)
        arCurrentAffixes[i] = GBID_NONE;
}






void ExtractedLegendariesData::SetToDefaults()
{
    nExtractedLegendariesCount = 0;
    for (int32 i = 0; i < 512; i++)
        gbidExtractedLegendaries[i] = GBID_NONE;
}






void ExtractedLegendariesData::Export(D3::GameBalance::BitPackedGbidArray & tPartitionData) const
{
    PackAssetsIntoBitPackedGbidArray(tPartitionData, ASSET_LIST_EXTRACTABLE_LEGENDARY_GBIDS, (int32 *)gbidExtractedLegendaries, nExtractedLegendariesCount, 512);
}








void ExtractedLegendariesData::Import(const D3::GameBalance::BitPackedGbidArray & tPartitionData)
{
    GBID gbidExtractedLegendariesLocal[512];
    int32 nDataCount = UnpackAssetsFromBitPackedGbidArray(tPartitionData, ASSET_LIST_EXTRACTABLE_LEGENDARY_GBIDS, (int32 *)gbidExtractedLegendariesLocal, 512);
    const int32 MAX_NEW_LEGENDARY_CONVERSIONS = 256;
    GBID gbidNewExtractedLegendaryConversions[MAX_NEW_LEGENDARY_CONVERSIONS];
    memset(gbidNewExtractedLegendaryConversions, 0xff, sizeof(gbidNewExtractedLegendaryConversions));
    int32 nExtractedLegendaryConversions = 0;
    for (int32 i = 0; i < nDataCount; i++)
    {
        GBID gbidExtractedLegendary = gbidExtractedLegendariesLocal[i];
        ScopedItemDefinitionLockR ptItemDef(gbidExtractedLegendary);
        if (ptItemDef != NULL && ptItemDef->gbidLegendaryPowerItemReplacement != GBID_NONE)
            gbidNewExtractedLegendaryConversions[nExtractedLegendaryConversions++] = ptItemDef->gbidLegendaryPowerItemReplacement;
    }
    for (int32 i = 0; i < MAX_NEW_LEGENDARY_CONVERSIONS; i++)
    {
        GBID gbidNewExtractedLegendary = gbidNewExtractedLegendaryConversions[i];
        if (gbidNewExtractedLegendary == GBID_NONE)
            break;
        BOOL bAlreadyExists = FALSE;
        for (int32 i = 0; i < nDataCount; i++)
        {
            if (gbidExtractedLegendariesLocal[i] == gbidNewExtractedLegendary)
            {
                bAlreadyExists = TRUE;
                break;
            }
        }
        if (!bAlreadyExists && nDataCount < 512)
            gbidExtractedLegendariesLocal[nDataCount++] = gbidNewExtractedLegendary;
    }
    for (int32 i = 0; i < nDataCount; i++)
    {
        GBID gbidItem = gbidExtractedLegendariesLocal[i];
        BOOL bCanBeExtracted = ItemCanExtractLegendaryPower(gbidItem);
        BOOL bContains = Contains(gbidItem);
        if (bCanBeExtracted && bContains)
        {
            ScopedItemDefinitionLockR ptItemDef(gbidItem);
            ItemGetLegendaryPower(ptItemDef);  // only feeds a stripped log line
        }
        if (bCanBeExtracted && !bContains)
        {
            gbidExtractedLegendaries[nExtractedLegendariesCount] = gbidItem;
            nExtractedLegendariesCount++;
        }
    }
}























































BOOL ExtractedLegendariesData::Contains(GBID gbidItem) const
{
    SNO snoPower = ItemGetLegendaryPower(gbidItem);
    for (int32 i = 0; i < nExtractedLegendariesCount; i++)
    {
        if (ItemGetLegendaryPower(gbidExtractedLegendaries[i]) == snoPower)
            return TRUE;
    }
    return FALSE;
}







































static void sSetItemTypeLabels(Item & tItem, const ItemType * ptItemType)
{
    const ItemType * ptItemTypeToCheck = ptItemType;


    if (ptItemTypeToCheck == NULL)
    {
        ptItemTypeToCheck = tItem.ptItemType;
        if (ptItemTypeToCheck == NULL)
        {
            return;
        }
    }






    for (int32 i = 0; i < 5; i++)
    {
        tItem.pdwLabelMask[i] |= ptItemTypeToCheck->pdwLabelMask[i];
    }

    if (ptItemTypeToCheck->ptParentType != NULL)
    {
        sSetItemTypeLabels(tItem, ptItemTypeToCheck->ptParentType);
    }
}





static void sItemsBuildTypeTree(ItemAppGlobals & tItemAppGlobals)
{

    GBHandleList lstItemDefs(GBGetHandlePool());
    GBHandleList lstItemTypeDefs(GBGetHandlePool());
    GBEnumerate(GB_ITEMS, lstItemDefs);
    GBEnumerate(GB_ITEMTYPES, lstItemTypeDefs);


    int32 nItemTypeDefCount = lstItemTypeDefs.Count();


    tItemAppGlobals.nItems = lstItemDefs.Count();
    tItemAppGlobals.nItemTypes = nItemTypeDefCount;


    tItemAppGlobals.ptItems = (Item *)SigmaMemoryNew(sizeof(Item) * tItemAppGlobals.nItems, 4, NULL, TRUE);
    tItemAppGlobals.ptItemTypes = (ItemType *)SigmaMemoryNew(sizeof(ItemType) * tItemAppGlobals.nItemTypes, 4, NULL, TRUE);

    sCopyItemTypes(tItemAppGlobals, tItemAppGlobals.tRootItemType, tItemAppGlobals.ptItemTypes, lstItemTypeDefs);


    int32 nItemIndex = 0;


    POS pos = lstItemDefs.GetHeadPosition();

    while (pos)
    {
        const GBHandle & hGB = lstItemDefs.GetNext(pos);
        ScopedItemDefinitionLockR ptItemDef(hGB.gbid);
        if (ptItemDef != NULL)
        {



            Item & tItem = tItemAppGlobals.ptItems[nItemIndex];
            tItem.gbid = ptItemDef->tHeader.gbid;
            tItem.nItemLevel = ptItemDef->nItemLevel;
            tItem.eMinAct = ptItemDef->eMinAct;
            tItem.dwFlags = ptItemDef->dwFlags;
            tItem.gbidBaseItem = ptItemDef->gbidBaseItem;
            tItem.ptItemType = NULL;
            tItem.nSeasonRequiredToDrop = ptItemDef->nSeasonRequiredToDrop;


            tItem.pdwLabelMask = (uint32 *)ptItemDef->dwItemLabelMask;







            const GBHandle hGBDropWeights(GB_ITEM_DROP_TABLE, hGB.gbid);
            if (GBIDExists(hGBDropWeights))
            {
                const GameBalanceDefinition * pdefGBDropWeights;
                ItemDropTableEntry * ptItemDropWeights = (ItemDropTableEntry *)GBLockR(hGBDropWeights, pdefGBDropWeights, TRUE);

                tItem.pnDropWeights = ptItemDropWeights->pnDropWeights;
                tItem.pnVendorDropWeights = ptItemDropWeights->pnVendorDropWeights;
                GBUnlockR(pdefGBDropWeights);
            }
            else
            {
                tItem.pnDropWeights = saEmptyItemDropWeights;
                tItem.pnVendorDropWeights = saEmptyItemDropWeights;
            }





















            tItemAppGlobals.mapItems.SetAt_Tracked(ptItemDef->tHeader.gbid, &tItemAppGlobals.ptItems[nItemIndex]);
            nItemIndex++;



            if (CurrencyFromGBID(tItem.gbid) != CURRENCY_INVALID)
                tItem.dwFlags |= (1 << ITEM_IS_CURRENCY_BIT); else tItem.dwFlags &= ~(1 << ITEM_IS_CURRENCY_BIT);
        }
    }
    sFindMemberItems(&tItemAppGlobals.tRootItemType,
        tItemAppGlobals.ptItems, lstItemDefs, lstItemTypeDefs);




    for (int32 i = 0; i < nItemIndex; i++)
    {
        sSetItemTypeLabels(tItemAppGlobals.ptItems[i], NULL);
    }



}




static void sBuildAffixList(ItemAppGlobals & tItemAppGlobals)
{


    GBHandleList lstAffixes(GBGetHandlePool());
    GBEnumerate(GB_AFFIXES, lstAffixes);
    tItemAppGlobals.nAffixes = lstAffixes.Count();



    tItemAppGlobals.ptAffixes = (Affix *)SigmaMemoryNew(sizeof(Affix) * tItemAppGlobals.nAffixes, 4, NULL, TRUE);
    tItemAppGlobals.ptCachedAffixTableEntyPtrs = (AffixTableEntry **)SigmaMemoryNew(sizeof(AffixTableEntry *) * tItemAppGlobals.nAffixes, 4, NULL, TRUE);
    int32 nAffixIndex = 0;
    POS pos = lstAffixes.GetHeadPosition();

    while (pos)
    {
        const GBHandle & hGB = lstAffixes.GetNext(pos);
        ScopedItemAffixLockR ptAffix(hGB.gbid);
        Affix & tAffix = tItemAppGlobals.ptAffixes[nAffixIndex];

        tAffix.gbid = ptAffix->tHeader.gbid;
        tAffix.dwFlags = ptAffix->dwFlags;
        tAffix.nFrequencyWeight = ptAffix->nFrequencyWeight;

        for (int32 i = 0; i < 7; i++)
            tAffix.nClassFrequencyWeight[i] = ptAffix->nClassFrequencyWeight[i];

        for (int32 i = 0; i < 4; i++)
        {
            tAffix.nHirelingFrequencyWeight[i] = ptAffix->nHirelingFrequencyWeight[i];
        }
        tAffix.nAffixLevelMin = ptAffix->nAffixLevelMin;
        tAffix.nAffixLevelMax = ptAffix->nAffixLevelMax;
        tAffix.gbidAffixFamily0 = ptAffix->gbidAffixFamily0;
        tAffix.gbidAffixFamily1 = ptAffix->gbidAffixFamily1;
        tAffix.ePlayerClass = ptAffix->ePlayerClass;
        tAffix.gbidExclusionCategory = ptAffix->gbidExclusionCategory;
        tAffix.dwAllowedQualityLevels = ptAffix->dwAllowedQualityLevels;
        tAffix.eAffixType = ptAffix->eAffixType;
        tAffix.gbidAssociatedAffix = ptAffix->gbidAssociatedAffix;
        tAffix.eItemEffectType = ptAffix->eItemEffectType;
        tAffix.nItemEffectLevel = ptAffix->nItemEffectLevel;
        tAffix.eAffixGroup = ptAffix->eAffixGroup;

        for (int32 i = 0; i < 24; i++)
        {
            tAffix.arAllowedTypes[i] = ptAffix->arAllowedTypes[i];
            tAffix.arLegendaryAllowedTypes[i] = ptAffix->arLegendaryAllowedTypes[i];
        }

        for (int32 i = 0; i < 6; i++)
        {
            tAffix.arExcludedCategories[i] = ptAffix->arExcludedCategories[i];
        }

        nAffixIndex++;
    }

}




CRefString sGetRareItemName(const ActorCommonData & tACD)
{

    CRefString sItemName;
    SNO snoRareItemNameStringList = SNO_NONE;



    {   ScopedItemDefinitionLockR pItemDef(tACD.hGB.gbid);
        if (pItemDef)
        {
            snoRareItemNameStringList = tACD.tRareItemName.fItemNameIsPrefix ?
                pItemDef->snoRareNamePrefixStringList : pItemDef->snoRareNameSuffixStringList;
        }
    }

    return ItemGetRareName(tACD.tRareItemName, snoRareItemNameStringList);
}




CRefString ItemGetDisplayName(
    const ActorCommonData & tACD,
    const BOOL fColorText,
    const BOOL fForceIdentified,
    const BOOL fSuppressStackAmount,
    const BOOL fShortenMagicNames)
{
    CRefString usReturn("ERROR GETTING NAME");

    if (tACD.hGB.eType != GB_ITEMS)
    {
        return usReturn;
    }

    BOOL fIdentified = !ItemIsUnidentified(tACD) ||
        fForceIdentified;




    ScopedItemDefinitionLockR pdefItem(tACD.hGB.gbid);
    if (!pdefItem)
    {
        return "BAD GBID";
    }


    if (tACD.hGB.gbid == GlobalGBIDGet(GLOBALGBID_CONSOLE_FRIEND_GIFT) || tACD.hGB.gbid == GlobalGBIDGet(GLOBALGBID_CONSOLE_AVENGER_GIFT))
    {
        if (tACD.ptFriendGiftData != NULL)
        {
            CRefString friendName(tACD.ptFriendGiftData->sNameRecipient);

            usReturn = LocalizedSprintf(StringListFetch("X1_ConsoleMail:FriendGiftName"),
                friendName.str(), NULL);
        }
        else
        {
            usReturn = CRefString("");
        }
        return usReturn;
    }


    CRefString sItemName1, sItemName2;
    uint32 dwGrammaticalForm;
    int32 nItemLevel;
    ItemGetBaseDisplayName(pdefItem, fIdentified, sItemName1, sItemName2, dwGrammaticalForm, nItemLevel);

    ItemQualityLevel eIQL = ItemGetQualityLevel(tACD);
    if (ItemIsGold(tACD))
    {
        int32 nGoldAmount = tACD.AttributesGetInt(ATTRIBUTE_GOLD);
        CRefString usGoldAmount = UnicodeText::FormatNumber(nGoldAmount, TRUE, FNS_NORMAL, 0);
        usReturn = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GENERAL), "GoldFormat"),
            usGoldAmount.str(), NULL);
    }
    else if (ItemIsShard(tACD))
    {

        usReturn = sItemName1;


    }
    else if (eIQL >= IQL_RARE_AFFIX_START && eIQL <= IQL_RARE_AFFIX_END)
    {

        if (!fIdentified)
        {
            if (LanguageRules::ItemNamesShouldBeCapitalized())
            {
                sItemName1 = UnicodeText::Capitalize(sItemName1);
            }
            usReturn = sItemName1;
        }
        else
        {
            usReturn = sGetRareItemName(tACD);
        }
    }
    else if (eIQL < IQL_RARE_AFFIX_START)
    {


        const GBIDPooledList * plistAffixGBIDs = fForceIdentified ? &tACD.listUnidentifiedAffixGBIDs : &tACD.listIdentifiedAffixGBIDs;

        uint32 dwExtractionFlags[12];
        CRefString sAffixes[12];

        SigmaMemoryClear(dwExtractionFlags, sizeof(dwExtractionFlags));

        BOOL fShouldShorten = fShortenMagicNames && LanguageRules::PreferShorterMagicNames();
        if (!fShouldShorten)
        {
            POS pos = plistAffixGBIDs->GetHeadPosition();
            while (pos)
            {
                const GBID & gbid = plistAffixGBIDs->GetNext(pos);
                ItemGetAffixNameFromGBID(gbid, dwGrammaticalForm, MarkerNameGetString(tACD.tName), sAffixes, dwExtractionFlags);
            }
        }

        usReturn = ItemGetMagicItemDisplayName(sItemName1, sItemName2, sAffixes, dwExtractionFlags, dwGrammaticalForm,


            tACD.snoActor == SNOFileLookupGUID(ACTOR_SNO, "Amiibo_Portal_Device"));


    }
    else
    {

        usReturn = sItemName1;
    }


    if (!fSuppressStackAmount)
    {
        uint64 uStackCount = tACD.GetStackCount();
        if (uStackCount > 1 && uStackCount != 0x8000000000000000ULL)
        {

            usReturn += CRefString().Format(" (%llu)", uStackCount);
        }
    }
























    if (fColorText)
    {
        RGBAColor rgbaColor = ItemGetDisplayNameColor(tACD);
        Set_Text_Color(usReturn, rgbaColor);
    }

    return usReturn;
}




static void sBuildCurrencyMaps(ItemAppGlobals & tItemAppGlobals)
{



    GBHandleList listGBHandles(GBGetHandlePool());
    GBEnumerate(GB_CURRENCY_CONVERSIONS, listGBHandles);

    POS pos = listGBHandles.GetHeadPosition();
    while (pos)
    {
        GBHandle hGB = listGBHandles.GetNext(pos);
        ScopedCurrencyConversionEntryLockR pdefCurrencyConversion(hGB.gbid);

        if (pdefCurrencyConversion != NULL)
        {
            CurrencyType eCurrency = pdefCurrencyConversion->eCurrency;
            tItemAppGlobals.tCurrencyToCurrencyInfoMap[eCurrency].bAutoPickup = pdefCurrencyConversion->bAutoPickup;

            for (int32 i = 0; i < 5; i++)
            {
                GBID gbidItem = pdefCurrencyConversion->arItemGBIDs[i];
                if (gbidItem != GBID_NONE)
                {
                    tItemAppGlobals.tGBIDToCurrencyMap[gbidItem] = eCurrency;


                    if (i == 0)
                    {
                        tItemAppGlobals.tCurrencyToCurrencyInfoMap[eCurrency].gbidItem = gbidItem;
                    }
                }
            }
        }
    }
}



static void sBuildCraftedItemGBIDToPowerSNOMap(ItemAppGlobals & tItemAppGlobals)
{




    GBHandleList listGBHandles(GBGetHandlePool());
    GBEnumerate(GB_RECIPES, listGBHandles);


























    POS pos = listGBHandles.GetHeadPosition();
    while (pos)
    {
        GBHandle hGB = listGBHandles.GetNext(pos);

        ScopedRecipeEntryLockR pdefRecipeEntry(hGB.gbid);
        if (pdefRecipeEntry == NULL || pdefRecipeEntry->snoRecipe == SNO_NONE)
        {
            continue;
        }

        ScopedRecipeDefinitionLockR pdefRecipe(pdefRecipeEntry->snoRecipe);
        if (pdefRecipe == NULL)
        {
            continue;
        }

        if (!ItemIsLegendary(pdefRecipe->tSpecificItem.gbidItem))
        {
            continue;
        }

        for (int32 i = 0; i < 6; i++)
        {
            GBID gbidAffix = pdefRecipe->tSpecificItem.gbidAffixes[i];
            if (gbidAffix == GBID_NONE)
            {
                continue;
            }
            if (!GBIDExists(gbidAffix, GB_AFFIXES))
            {
                continue;
            }

            ScopedItemAffixLockR ptAffix(gbidAffix);
            if (ptAffix == NULL)
            {
                continue;
            }
            for (int32 j = 0; j < 4; j++)
            {
                const AttributeSpecifier & tAttribute = ptAffix->ptAffixAttributes[j]; if (tAttribute.eAttribute == ATTRIBUTE_ITEM_POWER_PASSIVE)
                {
                    SNO snoPower = tAttribute.nParam;
                    if (SNOFileExists(POWER_SNO, snoPower))
                    {
                        GBID gbidItem = pdefRecipe->tSpecificItem.gbidItem;
                        tItemAppGlobals.tCraftedItemGBIDToPowerInfo[gbidItem].snoPower = snoPower;

                        RNG tRNG;
                        RandSetSeed_Internal(tRNG, 0);

                        ScopedItemDefinitionLockR pdefItem(gbidItem);
                        BOOL bNeedMinimum = pdefItem != NULL && ((pdefItem->dwFlags >> ITEM_FORCE_EXTRACT_POWER_MINIMUM) & 1);
                        tItemAppGlobals.tCraftedItemGBIDToPowerInfo[gbidItem].flUsePowerValue = bNeedMinimum ? AffixGetFormulaMinValue(tAttribute, NULL, gbidItem, &tRNG, NULL) : AffixGetFormulaMaxValue(tAttribute, NULL, gbidItem, &tRNG, NULL);
                        break;


                    }

                }
            }
        }
    }
}



static void sCopyItemTypes(ItemAppGlobals & tItemAppGlobals, ItemType & tRootItemType, ItemType * ptItemTypes, GBHandleList & lstItemTypeDefs)
{





    POS pos = lstItemTypeDefs.GetHeadPosition();
    int32 nITIndex = 0;
    const GameBalanceDefinition * pdefGB;

    while (pos)
    {
        const GBHandle & hGB = lstItemTypeDefs.GetNext(pos);
        ItemTypeDefinition * ptItemTypeDef = (ItemTypeDefinition *)GBLockR(hGB, pdefGB, TRUE);


        new (&ptItemTypes[nITIndex].arItems) ItemPointerArray();
        new (&ptItemTypes[nITIndex].arChildItemTypes) ItemTypePointerArray();
        ptItemTypes[nITIndex].dwFlags = ptItemTypeDef->dwFlags;
        ptItemTypes[nITIndex].fAlwaysMagic = (ptItemTypeDef->dwFlags >> IT_ALWAYSMAGIC_BIT) & 1;
        ptItemTypes[nITIndex].gbid = ptItemTypeDef->tHeader.gbid;
        ptItemTypes[nITIndex].nLootLevelRange = ptItemTypeDef->nLootLevelRange;
        ptItemTypes[nITIndex].nReqCrafterLevelForEnchant = ptItemTypeDef->nReqCrafterLevelForEnchant;


        ptItemTypes[nITIndex].pdwLabelMask = ptItemTypeDef->dwItemLabelMask;
        nITIndex++;




        GBUnlockR(pdefGB);
    }



    int32 nItemTypeDefCount = lstItemTypeDefs.Count();
    for (int32 nParentIndex = 0; nParentIndex < nItemTypeDefCount; nParentIndex++)
    {
        int32 nChildIndex = 0; pos = lstItemTypeDefs.GetHeadPosition();

        while (pos)
        {
            const GBHandle & hGB = lstItemTypeDefs.GetNext(pos);
            ItemTypeDefinition * ptItemTypeDef = (ItemTypeDefinition *)GBLockR(hGB, pdefGB, TRUE);

            if (ptItemTypes[nParentIndex].gbid == ptItemTypeDef->gbidParent)
            {
                ItemTypePointerArray & arChildItemTypes = ptItemTypes[nParentIndex].arChildItemTypes;

                arChildItemTypes.push_back(&ptItemTypes[nChildIndex]);
                ptItemTypes[nChildIndex].ptParentType = &ptItemTypes[nParentIndex];
            }
            GBUnlockR(pdefGB);
            nChildIndex++;
        }
    }


    for (nITIndex = 0; nITIndex < nItemTypeDefCount; nITIndex++)
    {
        if (ptItemTypes[nITIndex].ptParentType == NULL)
        {

            tRootItemType.arChildItemTypes.push_back(&ptItemTypes[nITIndex]);
        }
    }
}



static void sFindMemberItems(ItemType * ptItemType, Item * ptItems, GBHandleList & lstItemDefs, GBHandleList & lstItemTypeDefs)




{
    if (ptItemType == NULL)
        return;

    if (ptItemType->gbid != GBID_NONE)
    {
        POS pos = lstItemDefs.GetHeadPosition();
        int32 nItemIndex = 0;
        while (pos)
        {
            const GBHandle & hGB = lstItemTypeDefs.GetNext(pos);  // sic: the DWARF puts GetNext's `this` in the register holding lstItemTypeDefs; pos comes from lstItemDefs
            ScopedItemDefinitionLockR ptItemDef(hGB.gbid);

            if (ptItemDef->gbidItemType == ptItemType->gbid)
            {
                Item & tItem = ptItems[nItemIndex];

                tItem.ptItemType = ptItemType;
                tItem.ptItemType->arItems.push_back(&tItem);
            }
            nItemIndex++;
        }
    }


    for (uint32 i = 0; i < ptItemType->arChildItemTypes.size(); i++)
    {
        sFindMemberItems(ptItemType->arChildItemTypes[i], ptItems, lstItemDefs, lstItemTypeDefs);
    }



}



GBID ItemGetItemTypeGBID(GBID gbidItem)
{
    GBID gbidItemType = GBID_NONE;
    if (gbidItem != GBID_NONE)
    {
        ScopedItemDefinitionLockR ptItem(gbidItem);
        if (ptItem != NULL)
            gbidItemType = ptItem->gbidItemType;
    }
    return gbidItemType;
}


























const ItemType * ItemsGetItemType(GBID gbidItemType)
{
    ItemAppGlobals & tItemAppGlobals = *sgtAppGlobals.ptItemAppGlobals;














    XASSERT(tItemAppGlobals.ptItemTypes);
    for (int32 i = 0; i < tItemAppGlobals.nItemTypes; i++)
    {
        if (tItemAppGlobals.ptItemTypes[i].gbid == gbidItemType)
            return &tItemAppGlobals.ptItemTypes[i];
    }
    return NULL;
}
const Item * ItemsGetItem(GBID gbidItem)
{
    ItemAppGlobals * ptItemAppGlobals = sgtAppGlobals.ptItemAppGlobals;
    Item * ptItemRetval = NULL;
    BOOL bRetval = ptItemAppGlobals->mapItems.Lookup(gbidItem, ptItemRetval);
    if (bRetval)
        return ptItemRetval;
    return NULL;
}










































































BOOL ItemIsLargeSlotItem(GBID gbidItem)
{
    return FALSE;
}











BOOL ItemIsDrawnAsLargeSlotItem(const GBID gbidItem)
{
    if ((ItemHasLabel(gbidItem, ITL_ARMOR) && !ItemHasLabel(gbidItem, ITL_BELT)) || ItemHasLabel(gbidItem, ITL_WEAPON) || ItemHasLabel(gbidItem, ITL_OFFHAND))
        return TRUE;
    if (gbidItem != GBID_NONE)
    {
        ScopedItemDefinitionLockR pdefItem(gbidItem);
        if (pdefItem != NULL && (pdefItem->dwFlags & (1 << ITEM_FORCE_TO_LARGE_ITEM_BIT)))
            return TRUE;
    }
    return FALSE;
}













BOOL ItemMeetsOwnerClassRestrictions(const GBID gbidItem, const PlayerClass ePlayerClass, BOOL bCheckLegendaryClassDropWeight)
{
    int32 nBit;
    switch (ePlayerClass)
    {
    case PLAYER_CLASS_DEMONHUNTER:
        nBit = IT_USABLE_DEMONHUNTER_BIT;
        break;
    case PLAYER_CLASS_BARBARIAN:
        nBit = IT_USABLE_BARBARIAN_BIT;
        break;
    case PLAYER_CLASS_WIZARD:
        nBit = IT_USABLE_WIZARD_BIT;
        break;
    case PLAYER_CLASS_WITCHDOCTOR:
        nBit = IT_USABLE_WITCHDOCTOR_BIT;
        break;
    case PLAYER_CLASS_MONK:
        nBit = IT_USABLE_MONK_BIT;
        break;
    case PLAYER_CLASS_CRUSADER:
        nBit = IT_USABLE_CRUSADER_BIT;
        break;
    case PLAYER_CLASS_NECROMANCER:
        nBit = IT_USABLE_NECROMANCER_BIT;
        break;
    default:
        return TRUE;
    }
    if (!sItemTestItemTypeFlag(gbidItem, nBit))
        return FALSE;
    if (bCheckLegendaryClassDropWeight && ItemIsLegendary(gbidItem))
    {
        ScopedItemDefinitionLockR pdefItem(gbidItem);
        if (pdefItem != NULL && pdefItem->nClassLegendaryDropWeight[ePlayerClass] <= 0)
            return FALSE;
    }
    return TRUE;
}






















BOOL ItemTypeMeetsOwnerClassRestrictions(const GBID gbidItemType, const PlayerClass ePlayerClass)
{
    ScopedItemTypeDefinitonLockR pdefItemType(gbidItemType);
    if (pdefItemType == NULL)
        return TRUE;
    int32 nBit;
    switch (ePlayerClass)
    {
    case PLAYER_CLASS_DEMONHUNTER:
        nBit = IT_USABLE_DEMONHUNTER_BIT;
        break;
    case PLAYER_CLASS_BARBARIAN:
        nBit = IT_USABLE_BARBARIAN_BIT;
        break;
    case PLAYER_CLASS_WIZARD:
        nBit = IT_USABLE_WIZARD_BIT;
        break;
    case PLAYER_CLASS_WITCHDOCTOR:
        nBit = IT_USABLE_WITCHDOCTOR_BIT;
        break;
    case PLAYER_CLASS_MONK:
        nBit = IT_USABLE_MONK_BIT;
        break;
    case PLAYER_CLASS_CRUSADER:
        nBit = IT_USABLE_CRUSADER_BIT;
        break;
    case PLAYER_CLASS_NECROMANCER:
        nBit = IT_USABLE_NECROMANCER_BIT;
        break;
    default:
        return TRUE;
    }
    return (pdefItemType->dwFlags >> nBit) & 1;
}

















PlayerClass ItemGetRandomEligibleClass(GBID gbidItem, RNG & tRNG, BOOL bCheckLegendaryClassDropWeight)
{
    PlayerClass eEligibleClasses[NUM_PLAYER_CLASSES];
    int32 nEligibleClasses = 0;
    for (int32 i = 0; i < NUM_PLAYER_CLASSES; i++)
    {
        if (ItemMeetsOwnerClassRestrictions(gbidItem, (PlayerClass)i, bCheckLegendaryClassDropWeight))
            eEligibleClasses[nEligibleClasses++] = (PlayerClass)i;
    }
    if (nEligibleClasses <= 0)
        return PLAYER_CLASS_NONE;
    int32 nRandClass = Rand_Internal(tRNG, nEligibleClasses - 1);
    return eEligibleClasses[nRandClass];
}















void ItemGetValidTargetedClass(PlayerClass & eTargetedPlayerClass, HirelingClass & eTargetedHirelingClass, GBID gbidItem, PlayerClass eDesiredPlayerClass, RNG * ptRNGRandomClass, BOOL bCheckLegendaryClassDropWeight)
{
    eTargetedPlayerClass = PLAYER_CLASS_NONE;
    eTargetedHirelingClass = HC_NONE;
    if (ItemHasLabel(gbidItem, ITL_FOLLOWER_SPECIAL))
    {
        for (int32 i = HC_TEMPLAR; i < NUM_HIRELING_CLASSES; i++)
        {
            if (ItemUsableByHireling(gbidItem, (HirelingClass)i))
            {
                eTargetedHirelingClass = (HirelingClass)i;
                break;
            }
        }
    }
    else if (eDesiredPlayerClass != PLAYER_CLASS_NONE && ItemMeetsOwnerClassRestrictions(gbidItem, eDesiredPlayerClass, bCheckLegendaryClassDropWeight))
    {
        eTargetedPlayerClass = eDesiredPlayerClass;
    }
    else
    {
        if (GameActiveGameCommonDataIsClient())
            eTargetedPlayerClass = ItemGetRandomEligibleClass(gbidItem, ptRNGRandomClass ? *ptRNGRandomClass : CGameGlobalsGet().rngLocal, bCheckLegendaryClassDropWeight);
        if (GameActiveGameCommonDataIsServer())
            eTargetedPlayerClass = ItemGetRandomEligibleClass(gbidItem, ptRNGRandomClass ? *ptRNGRandomClass : SGameGlobalsGet().rng, bCheckLegendaryClassDropWeight);
    }
}























BOOL ItemUsableByHireling(const GBID gbidItem, const HirelingClass eHirelingClass)
{
    int32 nBit;
    switch (eHirelingClass)
    {
    case HC_TEMPLAR:
        nBit = IT_USABLE_TEMPLAR_BIT;
        break;
    case HC_SCOUNDREL:
        nBit = IT_USABLE_SCOUNDREL_BIT;
        break;
    case HC_ENCHANTRESS:
        nBit = IT_USABLE_ENCHANTRESS_BIT;
        break;
    default:
        return TRUE;
    }
    if (sItemTestItemTypeFlag(gbidItem, nBit))
        return TRUE;
    return FALSE;
}













extern "C" int32 sub_71004b8900(const ActorCommonData * ptACD, int32 nLevel);  // 2.7.7 only, unnamed: returns 1 (no level requirement) when a buff/attribute flag is set on ptACD, else nLevel
BOOL ItemOwnerMeetsRequirements(const ActorCommonData & tACDItem, const ActorCommonData & tACDOwner, GameError * pOutError)
{
    if (pOutError)
        *pOutError = GAMEERROR_IARESULT_PLAYER_DOESNT_MEET_ITEM_REQUIREMENTS;

    BOOL fOwnerIsHireling = ACDIsHireling(tACDOwner);
    if (ItemIsEquippedButDisabled(tACDItem))
    {
        if (pOutError)
        {
            if (ItemIsEquippedButDisabledDuplicateLegendary(tACDItem))
                *pOutError = GAMEERROR_ITEM_UNIQUE_EQUIPPED;
            else
                *pOutError = GAMEERROR_IARESULT_ITEM_REQUIRES_SKILL;
        }
        return FALSE;
    }

    PlayerClass ePlayerClass = PlayerGetPlayerClass(tACDOwner.id);
    if (!ItemMeetsOwnerClassRestrictions(tACDItem.hGB.gbid, ePlayerClass, FALSE))
    {
        if (pOutError)
            *pOutError = GAMEERROR_IARESULT_PLAYER_IS_WRONG_CLASS_FOR_ITEM;
        return FALSE;
    }

    BOOL fEquipped = FALSE;
    InventorySlotType eSlot = SLOT_NONE;
    if (tACDItem.GetLocationType() == AL_INVENTORY)
    {
        const InventoryLocation & tInvLocation = tACDItem.GetInventoryLocation();
        if (tInvLocation.idACDContainer == tACDOwner.id)
        {
            eSlot = tInvLocation.eSlot;
            fEquipped = ACDInventorySlotIsEquippable(eSlot);
        }
    }

    if (fOwnerIsHireling || ePlayerClass != PLAYER_CLASS_NONE)
    {
        int32 nOwnerLevel = tACDOwner.AttributesGetInt(ATTRIBUTE_LEVEL);
        int32 nRequiredLevel = ItemGetLevelRequirement(tACDItem.hGB.gbid, &tACDItem, FALSE); if (!fOwnerIsHireling) nRequiredLevel = sub_71004b8900(&tACDOwner, nRequiredLevel);
        if (nOwnerLevel < nRequiredLevel)
        {
            if (pOutError)
                { if (fOwnerIsHireling) *pOutError = GAMEERROR_IARESULT_HIRELING_DOESNT_MEET_ITEM_LEVEL; else *pOutError = GAMEERROR_IARESULT_PLAYER_DOESNT_MEET_ITEM_LEVEL; }
            return FALSE;
        }
    }

    if ((tACDItem.AttributesAttributeExists(MakeAttribKey(ATTRIBUTE_REQUIREMENT_WHEN_EQUIPPED, ATTRIBUTE_IS_BOW_ANY)) && fEquipped) || tACDItem.AttributesAttributeExists(MakeAttribKey(ATTRIBUTE_REQUIREMENT, ATTRIBUTE_IS_BOW_ANY)))
    {
        float flAttributeValue = tACDOwner.AttributesGetFloat(ATTRIBUTE_IS_BOW_ANY);
        if (flAttributeValue <= 0.000001f)
            return FALSE;
    }

    if (pOutError)
        *pOutError = GAMEERROR_NONE;
    return TRUE;
}





























ItemQualityLevel ItemGetQualityLevel(const ActorCommonData & tACD)
{
    return tACD.AttributesGetInt(ATTRIBUTE_ITEM_QUALITY_LEVEL);
}





ItemQualityLevel ItemGetDisplayedQualityLevel(const ActorCommonData & tACD)
{
    ScopedItemDefinitionLockR pdefItem(tACD.hGB.gbid);
    if (pdefItem != NULL && pdefItem->eDisplayedQualityLevel != IQL_INVALID)
        return pdefItem->eDisplayedQualityLevel;
    if (ItemIsSetItem(tACD.hGB.gbid))
        return IQL_DISPLAY_AS_SET_ITEM;
    return ItemGetQualityLevel(tACD);
}












ItemQualityLevel ItemGetDisplayedQualityLevel(const D3::Items::Generator & tItem)
{
    ScopedItemDefinitionLockR pdefItem(tItem.gb_handle().gbid());
    if (pdefItem != NULL && pdefItem->eDisplayedQualityLevel != IQL_INVALID)
        return pdefItem->eDisplayedQualityLevel;
    if (ItemIsSetItem(tItem.gb_handle().gbid()))
        return IQL_DISPLAY_AS_SET_ITEM;
    return tItem.item_quality_level();
}












int32 ItemGetStatsDataKey(const ActorCommonData & tACDItem)
{
    int32 nItemLevel = ItemGetItemLevel(tACDItem.hGB.gbid);
    int32 nLegendaryItemLevel = tACDItem.AttributesGetInt(ATTRIBUTE_ITEM_LEGENDARY_ITEM_LEVEL_OVERRIDE);
    if (nLegendaryItemLevel != 0)
        nItemLevel = nLegendaryItemLevel;
    int32 nQualityLevel = ItemGetQualityLevel(tACDItem);
    int32 nDataKey = nItemLevel + nQualityLevel * 1000;
    return nDataKey;
}

































BOOL ItemIsJunk(const ActorCommonData & tACDItem)
{
    return tACDItem.AttributesGetInt(ATTRIBUTE_ITEM_FLAGGED_AS_JUNK) == 1;
}





BOOL ItemCanBeMarkedAsJunk(const ActorCommonData & tACDItem)
{
    if (ItemCanBeSold(tACDItem, NULL) != ISE_NONE)
        return FALSE;
    if (ItemGetBoundToACD(tACDItem, TRUE) != ACDID_INVALID)
        return FALSE;
    if (ItemGetUseType(tACDItem) == ITEM_USE_TREASURE_BAG)
        return FALSE;
    return TRUE;
}













BOOL ItemIsAncientRank(const ActorCommonData & tACDItem)
{
    return tACDItem.AttributesGetInt(ATTRIBUTE_ANCIENT_RANK) > 0;
}




AncientRank ItemGetAncientRank(const ActorCommonData & tACDItem)
{
    return tACDItem.AttributesGetInt(ATTRIBUTE_ANCIENT_RANK);
}





BOOL ItemIsRandomItemPlaceholder(const GBID gbidItem)
{
    ScopedItemDefinitionLockR pdefItem(gbidItem);
    return pdefItem->snoRandomItemTreasureClass != SNO_NONE;
}





BOOL ItemCanBeAssignedToUsableItemButton(const ActorCommonData & tACDItem)
{
    if (ItemHasLabel(tACDItem.hGB.gbid, ITL_POTION) && (ItemIsLegendary(tACDItem.hGB.gbid) || tACDItem.hGB.gbid == GlobalGBIDGet(GLOBALGBID_ITEM_HEALTHPOTION_BOTTOMLESS)))
        return TRUE;
    return FALSE;
}









BOOL ItemIsConsumablePotion(const GBID gbidItem)
{
    if (ItemHasLabel(gbidItem, ITL_HEALTHPOTION) && !ItemIsLegendary(gbidItem) && GlobalGBIDGet(GLOBALGBID_ITEM_HEALTHPOTION_BOTTOMLESS) != gbidItem)
        return TRUE;
    return FALSE;
}










SNO ItemGetActorForGBID(const GBID gbidItem)
{
    ScopedItemDefinitionLockR ptItemDef(gbidItem);
    if (ptItemDef == NULL)
        return SNO_NONE;
    return ptItemDef->snoActor;
}








static inline ItemUseType sItemActorGetUseType(SNO snoActor)
{
    ScopedActorDefinitionLockR pdefActor(snoActor);
    ItemUseType eType = (ItemUseType)TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_USEITEM_TYPE);
    return eType;
}



ItemUseType ItemGetUseType(GBID gbidItem)
{
    ScopedItemDefinitionLockR pdefItem(gbidItem);
    if (pdefItem == NULL)
        return ITEM_USE_NONE;
    return sItemActorGetUseType(pdefItem->snoActor);
}







ItemUseType ItemGetUseType(const ActorCommonData & tACDItem)
{
    if (!tACDItem.IsItem())
        return ITEM_USE_NONE;
    return sItemActorGetUseType(tACDItem.snoActor);
}







SNO ItemGetUsePower(const GBID gbidItem)
{
    SNO snoActor = ItemGetActorForGBID(gbidItem);
    GlobalSNO eGlobalSNO;
    switch (sItemActorGetUseType(snoActor))
    {
    case ITEM_USE_HEALTHPOTION:
        eGlobalSNO = GLOBALSNO_POWERS_DRINK_HEALTH_POTION;
        break;
    case ITEM_USE_TOWNPORTALSTONE:
        eGlobalSNO = GLOBALSNO_POWERS_USE_STONE_OF_RECALL;
        break;
    case ITEM_USE_CALLDOWN_GRENADE:
        eGlobalSNO = GLOBALSNO_POWERS_CALLDOWN_GRENADE;
        break;
    case ITEM_USE_HEARTHPORTALSCROLL:
        eGlobalSNO = GLOBALSNO_POWERS_USE_HEARTH_SCROLL;
        break;
    case ITEM_USE_HEALTHGLYPH:
        eGlobalSNO = GLOBALSNO_POWERS_USE_HEALTH_GLYPH;
        break;
    case ITEM_USE_MANAGLYPH:
        eGlobalSNO = GLOBALSNO_POWERS_USE_MANA_GLYPH;
        break;
    case ITEM_USE_ARCANEGLYPH:
        eGlobalSNO = GLOBALSNO_POWERS_USE_ARCANE_GLYPH;
        break;
    case ITEM_USE_LOOT_RUN_PROGRESS_GLYPH:
        eGlobalSNO = GLOBALSNO_POWERS_USE_LOOT_RUN_PROGRESS_GLYPH;
        break;
    default:
        eGlobalSNO = GLOBALSNO_POWERS_USE_ITEM;
        break;
    }
    return GlobalSNOGet(eGlobalSNO);
}


uint32 ItemGetRemainingCooldownTime(const ActorCommonData & tACDPlayer, const ActorCommonData & tACDItem)
{
    SNO snoPower = ItemGetUsePower(tACDItem.hGB.gbid);
    if (snoPower == GlobalSNOGet(GLOBALSNO_POWERS_USE_ITEM))
        return (uint32)-1;
    return tACDPlayer.AttributesGetInt(MakeAttribKey(ATTRIBUTE_POWER_COOLDOWN, snoPower));
}









uint32 ItemGetCooldownTimeToApply(const ActorCommonData & tACDItem)
{
    SNO snoPower = ItemGetUsePower(tACDItem.hGB.gbid);
    if (snoPower == GlobalSNOGet(GLOBALSNO_POWERS_USE_ITEM))
        return (uint32)-1;
    float flCooldownTime = PowerGetFormulaValue(&tACDItem, snoPower, TAG_POWER_COOLDOWN, -1, -1, 0);
    uint32 dwCooldownTicks = Math::FloatToInt(flCooldownTime * 60.0f);
    return dwCooldownTicks;
}







BOOL ItemUseIsTargettedInInventory(ItemUseType eItemUseType)
{
    switch (eItemUseType)
    {
    case ITEM_USE_DYE:
    case ITEM_USE_DYE_REUSABLE:
    case ITEM_USE_ENCHANT_SCROLL:
    case ITEM_USE_ADD_SOCKETS:
    case 71: case 72:  // 2.7.7 ItemUseType values past ITEM_USE_HORADRIC_REAGENT (no names recovered)
        return TRUE;
    default:
        return FALSE;
    }
}








BOOL ItemUseIsTargettedInWorld(ItemUseType eItemUseType)
{
    return eItemUseType == ITEM_USE_CALLDOWN_GRENADE;
}













SNO ItemUseGetScrollPower(
    const ActorCommonData & tACDItem)
{
    if (!tACDItem.IsItem())
        return SNO_NONE;


    // switch inferred: the DWARF has no use-type local and lines 2552-2560 carry no rows
    switch (ItemGetUseType(tACDItem))
    {
    case ITEM_USE_POWER_SCROLL:
    case ITEM_USE_SCROLL:
        break;
    default:
        return SNO_NONE;
    }


    ScopedItemDefinitionLockR pdefItem(tACDItem.hGB.gbid);
    if (pdefItem == NULL)
        return SNO_NONE;



    SNO snoPower = SNO_NONE;
    ScopedActorDefinitionLockR pdefActor(pdefItem->snoActor);
    if (pdefActor != NULL)
    {
        snoPower = TagMapGetSNO(pdefActor->hTagMap, TAG_ACTOR_POWERUP_POWER);
    }

    if (snoPower != SNO_NONE)
        return snoPower;

    for (int32 i = 0; i < (int32)(pdefItem->serAttributes_Console.dwSize / sizeof(AttributeSpecifier)); i++)
    {
        AttributeSpecifier tSpecifier = pdefItem->ptAttributes[i];
        if (tSpecifier.eAttribute == ATTRIBUTE_ITEM_POWER_PASSIVE)
            return tSpecifier.nParam;
    }

    return SNO_NONE;




}




GameError ItemCanBeUsed(const ACDID idACDPlayer, const ActorCommonData & tACDItemToUse, const Target * ptTarget)
{



    int32 nDifficultyRestriction = ActorDefinitionGetTagMapValueInt(TAG_ACTOR_ITEM_USE_DIFFICULTY_REQUIREMENT, tACDItemToUse.snoActor);
    if (nDifficultyRestriction > 0)
    {
        int32 nDifficulty = GameGetHandicapLevel();
        if (nDifficulty < nDifficultyRestriction)
        {
            return GAMEERROR_DIFFICULTY_TOO_LOW;
        }
    }

    SNO snoLevelAreaRestriction = ActorDefinitionGetTagMapValueSNO(TAG_ACTOR_ITEM_USE_LEVELAREA_REQUIREMENT, tACDItemToUse.snoActor);
    if (snoLevelAreaRestriction != SNO_NONE)
    {
        ActorCommonData * ptACDPlayer = ACDTryToGet(idACDPlayer);
        if (ptACDPlayer == NULL || ACDGetLevelArea(*ptACDPlayer) != snoLevelAreaRestriction)
        {
            return GAMEERROR_POWER_UNUSABLE_IN_AREA;
        }
    }

    ItemUseType eType = ItemGetUseType(tACDItemToUse);
    switch (eType)
    {
    case ITEM_USE_HEALTHPOTION:
    {
        ActorCommonData * ptACDPlayer = ACDTryToGet(idACDPlayer);
        if (ptACDPlayer == NULL)
        {
            return GAMEERROR_ITEMUSE_UNKNOWN;
        }

        float flHPCurrent = ptACDPlayer->GetHitPointsCurrent();
        float flHPMax = ptACDPlayer->GetHitPointsMaxTotal();
        if (flHPMax - flHPCurrent < 0.000001f)
        {
            return GAMEERROR_ITEMUSE_ALREADY_AT_MAX_HEALTH;
        }


        if (ptACDPlayer->AttributesGetInt(ATTRIBUTE_CANT_START_DISPLAYED_POWERS))
        {
            return GAMEERROR_CANNOT_DO_THAT_RIGHT_NOW;
        }

        return GAMEERROR_NONE;
    }

    case ITEM_USE_DYE:
    case ITEM_USE_DYE_REUSABLE:
    {


        ActorCommonData & tACDTarget = ACDGet(ptTarget->idACDTarget);
        return ItemIsDyeable(tACDTarget) ? GAMEERROR_NONE : GAMEERROR_ITEM_CANNOT_BE_DYED;
    }

    case ITEM_USE_ADD_SOCKETS:
    {

        ActorCommonData & tACDTarget = ACDGet(ptTarget->idACDTarget);
        return IsSocketAddable(tACDTarget, tACDItemToUse);
    }

    case ITEM_USE_LEARN_ENCHANT:
    case ITEM_USE_ENCHANT_SCROLL:
        return GAMEERROR_ITEMUSE_UNKNOWN;



    case ITEM_USE_SUMMON_MONSTER:
    {

        ActorCommonData * ptACDPlayer = ACDTryToGet(idACDPlayer);
        if (ptACDPlayer == NULL)
        {
            return GAMEERROR_ITEMUSE_UNKNOWN;
        }

        if (ACDIsInTown(*ptACDPlayer))
        {
            return GAMEERROR_ITEMUSE_UNKNOWN;
        }

        return GAMEERROR_NONE;
    }



    case ITEM_USE_PAGE_OF_TRAINING:
    {

        ActorCommonData * ptACDPlayer = ACDTryToGet(idACDPlayer);
        if (ptACDPlayer == NULL)
        {
            return GAMEERROR_ITEMUSE_UNKNOWN;
        }


        uint32 nCount = ActorInventoryCountItemsOfTypeOnPlayer(idACDPlayer, tACDItemToUse.hGB.gbid, NULL);
        if (nCount < 5)
        {
            return GAMEERROR_ITEMUSE_NOT_ENOUGH_PAGES_TO_CONVERT;
        }
        break;
    }

    case ITEM_USE_LEARN_RECIPE:
    {
        const Player * ptPlayer = PlayerGetByACD(idACDPlayer);
        if (ptPlayer == NULL)
        {
            return GAMEERROR_ITEMUSE_UNKNOWN;
        }

        ScopedItemDefinitionLockR pdefItem(tACDItemToUse.hGB.gbid);
        for (int32 i = 0; i < 10; i++)
        {
            if (pdefItem->gbidCraftingRecipesToGrant[i] != GBID_NONE)
            {
                ScopedRecipeEntryLockR pdefRecipeEntry(pdefItem->gbidCraftingRecipesToGrant[i]);
                if (pdefRecipeEntry == NULL)
                {
                    return GAMEERROR_ITEMUSE_UNKNOWN;
                }



















                GameError eError = ItemCrafting_CanLearnRecipe(pdefRecipeEntry, ptPlayer);
                if (eError != GAMEERROR_NONE)
                {
                    return eError;
                }
            }
        }
    }
    break;

    case ITEM_USE_SUMMON_COMPANION:
    case ITEM_USE_TREASURE_BAG:
        return GAMEERROR_NONE;



    case ITEM_USE_UNLOCK_TRANSMOGS:
    {

        const Player * ptPlayer = PlayerGetByACD(idACDPlayer);
        if (ptPlayer == NULL)
        {
            return GAMEERROR_ITEMUSE_UNKNOWN;
        }










        BOOL bCanUnlock = FALSE;
        ScopedItemDefinitionLockR pdefItem(tACDItemToUse.hGB.gbid);
        for (int32 i = 0; i < 8; i++)
        {
            if (pdefItem->gbidTransmogsToGrant[i] != GBID_NONE &&
                ItemCrafting_CanUnlockTransmog(pdefItem->gbidTransmogsToGrant[i], NULL, TRUE) &&
                !ItemCrafting_IsTransmogUnlocked(pdefItem->gbidTransmogsToGrant[i], ptPlayer, NULL))
            {
                bCanUnlock = TRUE;
                break;
            }
        }
        if (!bCanUnlock)
        {
            return GAMEERROR_ITEMUSE_RECIPE_ALREADY_KNOWN;
        }

    }
    break;
    }











    BOOL bUsingAmiiboPortalDevice = tACDItemToUse.snoActor == SNOFileLookupGUID(ACTOR_SNO, "Amiibo_Portal_Device");
    if (idACDPlayer != ACDID_INVALID && bUsingAmiiboPortalDevice)
    {
        ActorCommonData * ptACDPlayer = ACDTryToGet(idACDPlayer);
        if (ptACDPlayer != NULL)
        {
            if (!ptACDPlayer->GetHasNFPDevice())
            {
                return GAMEERROR_AMIIBO_NO_DEVICE;
            }


            SNO snoLevelArea = ACDGetLevelArea(*ptACDPlayer);
            if (snoLevelArea != SNO_NONE)
            {
                BOOL bInRift = FALSE;
                BOOL bInBossEncounter = ptACDPlayer->AttributesGetInt(ATTRIBUTE_IN_BOSS_ENCOUNTER);

                SWorldID idSWorld = ptACDPlayer->GetWorldLocation().idSWorld;
                if (WorldTryGetCommonData(idSWorld) != NULL)
                {
                    bInRift = WorldIsDungeonFinderPlaceHolder(WorldCommonGetWorldSNO(idSWorld));
                }

                BOOL bInSetDungeon = ACDIsParticipatingInSetDungeon(*ptACDPlayer);

                if (bInRift || bInBossEncounter || bInSetDungeon ||


                    snoLevelArea == SNOFileLookupGUID(LEVELAREA_SNO, "p1_TreasureRealm") ||
                    snoLevelArea == SNOFileLookupGUID(LEVELAREA_SNO, "p1_TreasureRealm_Greed"))
                {
                    if (bInRift && ACDIsInTieredLootRun(*ptACDPlayer))
                    {
                        return GAMEERROR_LOOTRUN_TIERED_CANNOT_DO_THAT;
                    }
                    return GAMEERROR_POWER_UNUSABLE_IN_AREA;
                }
            }
        }
    }














    return GAMEERROR_NONE;
}




BOOL ItemHasLabelMask(const Item & tItem, const uint32 * pdwItemLabelMask)
{
    for (int32 i = 0; i < ITEM_LABEL_MASK_DWORDS; i++)
    {
        if ((tItem.pdwLabelMask[i] & pdwItemLabelMask[i]) != pdwItemLabelMask[i])
            return FALSE;
    }
    return TRUE;
}









BOOL ItemHasLabelMask(const GBID gbidItem, const uint32 * pdwItemLabelMask)
{
    if (gbidItem == GBID_NONE)
        return FALSE;
    const Item * ptItem = ItemsGetItem(gbidItem);
    return ItemHasLabelMask(*ptItem, pdwItemLabelMask);
}





































































BOOL ItemHasLabel(
    const Item & tItem,
    const ItemLabel eItemLabel)
{
    uint32 dwItemLabelMask[ITEM_LABEL_MASK_DWORDS] = { 0 };



    dwItemLabelMask[eItemLabel >> 5] |= 1 << (eItemLabel & 31);
    return ItemHasLabelMask(tItem, dwItemLabelMask);
}



// Matches once the original linkage is used: ItemHasLabel(const Item &) etc. are external, not static inline (DWARF).
BOOL ItemHasLabel(
    const GBID gbidItem,
    const ItemLabel eItemLabel)
{
    if (gbidItem == GBID_NONE || eItemLabel == ITL_INVALID)
        return FALSE;








    const Item * ptItem = ItemsGetItem(gbidItem);
    if (ptItem == NULL)
        return FALSE;




    return ItemHasLabel(*ptItem, eItemLabel);
}



void ItemGetItemTypes(
    const ItemDefinition * ptItem,
    XGrowableList<int, int> & listItemTypes)
{
    const ItemType * ptItemType = ItemsGetItemType(ptItem->gbidItemType);
    // if-block inferred: gbidItem's lexical block (DWARF 0x043502be) encloses the whole loop
    if (ptItemType != NULL)
    {
        GBID gbidItem = ptItemType->gbid;
        while (ptItemType != NULL)
        {
            listItemTypes.AddHead_Tracked(ptItemType->gbid);
            // original locks the handle of gbidItem (constant w21) every iteration, not of ptItemType->gbid
            GBHandle hGB(GB_ITEMTYPES, gbidItem);

            const GameBalanceDefinition * pdefGB;
            const ItemTypeDefinition * ptItemTypeDef = (const ItemTypeDefinition *)GBLockR(hGB, pdefGB, TRUE);

            if (ptItemTypeDef == NULL)
                break;



            ptItemType = ptItemType->ptParentType;
            GBUnlockR(pdefGB);
        }
    }
}



BOOL ItemGetInherentAffixCount(const GBHandle hGBItem, blz::vector<int, blz::allocator<int> > * arInherentAffixGBIDs, blz::vector<int, blz::allocator<int> > * arInherentAffixFamilyGBIDs)
{



    int32 nInherentAffixCount = 0;

    if (!ItemIsLegendary(hGBItem.gbid) &&
        !ItemIsSpecial(hGBItem.gbid))
    {
        GBHandle hGBItemType = ItemsGetItemTypeGB(hGBItem);
        ScopedItemTypeDefinitonLockR pdefItemType(hGBItemType.gbid);
        if (pdefItemType == NULL)
        {
            return 0;
        }

        for (int32 i = 0; i < 3; i++)
        {
            if (pdefItemType->gbidInherentAffix[i] != GBID_NONE)
            {
                nInherentAffixCount++;
                if (arInherentAffixGBIDs != NULL)
                {
                    arInherentAffixGBIDs->push_back(pdefItemType->gbidInherentAffix[i]);
                }
            }
        }

        for (int32 i = 0; i < 1; i++)
        {
            if (pdefItemType->gbidInherentAffixFamily[i] != GBID_NONE)
            {
                nInherentAffixCount++;
                if (arInherentAffixFamilyGBIDs != NULL)
                {
                    arInherentAffixFamilyGBIDs->push_back(pdefItemType->gbidInherentAffixFamily[i]);
                }
            }
        }
    }
    else
    {
        ScopedItemDefinitionLockR pdefItem(hGBItem.gbid);
        if (pdefItem == NULL)
        {
            return 0;
        }

        for (int32 i = 0; i < 6; i++)
        {
            if (pdefItem->gbidLegendaryAffixFamily[i] != GBID_NONE)
            {
                nInherentAffixCount++;
                if (arInherentAffixFamilyGBIDs != NULL)
                {
                    arInherentAffixFamilyGBIDs->push_back(pdefItem->gbidLegendaryAffixFamily[i]);
                }
            }
        }
    }


    return nInherentAffixCount;
}


BOOL ItemIsAffixInherent(const ActorCommonData & tACDItem, const GBID gbidAffix, BOOL bIgnoreLegendaryAffixFamilies)
{




    if (ItemIsUnidentified(tACDItem))
    {
        return TRUE;
    }


    ScopedItemAffixLockR pdefAffix(gbidAffix);
    if (pdefAffix == NULL)
    {
        return TRUE;
    }

    for (int32 i = 0; i < 4; i++)
    {
        if (pdefAffix->ptAffixAttributes[i].eAttribute == ATTRIBUTE_ITEM_IS_BOE)
        {
            return TRUE;
        }
    }

    if (pdefAffix->dwFlags & (1 << AFFIX_CANNOT_BE_ENCHANTED_BIT))
    {
        return TRUE;
    }


    int32 nAffixIndex = 0;
    POS pos = tACDItem.listIdentifiedAffixGBIDs.GetHeadPosition();
    while (pos)
    {
        GBID gbid = tACDItem.listIdentifiedAffixGBIDs.GetNext(pos);
        if (gbid == gbidAffix)
        {
            break;
        }
        nAffixIndex++;
    }


    if (nAffixIndex >= tACDItem.listIdentifiedAffixGBIDs.Count())
    {
        return TRUE;
    }


    blz::vector<int, blz::allocator<int> > arInherentAffixGBIDs;
    blz::vector<int, blz::allocator<int> > arInherentAffixFamilyGBIDs;
    int32 nInherentAffixCount = ItemGetInherentAffixCount(tACDItem.hGB, &arInherentAffixGBIDs, &arInherentAffixFamilyGBIDs);
    if (nInherentAffixCount <= 0)
    {
        return FALSE;
    }


    size_t nInherentAffixGBIDs = arInherentAffixGBIDs.size();
    for (uint32 i = 0; i < nInherentAffixGBIDs; i++)
    {
        if (arInherentAffixGBIDs[i] == gbidAffix)
        {
            return TRUE;
        }
    }


    if (!bIgnoreLegendaryAffixFamilies ||
        (!ItemIsLegendary(tACDItem.hGB.gbid) && !ItemIsSpecial(tACDItem.hGB.gbid)))
    {
        size_t nInherentAffixFamilyGBIDs = arInherentAffixFamilyGBIDs.size();
        for (uint32 i = 0; i < nInherentAffixFamilyGBIDs; i++)
        {
            if (pdefAffix->gbidAffixFamily0 == arInherentAffixFamilyGBIDs[i] ||
                pdefAffix->gbidAffixFamily1 == arInherentAffixFamilyGBIDs[i])
            {
                return TRUE;
            }
        }
    }

    return FALSE;
}




void ItemGetLabels(XGrowableList<int, int> & listLabels, const GBID gbidItem)
{
    if (gbidItem == GBID_NONE)
        return;

    const Item * ptItem = ItemsGetItem(gbidItem);
    if (ptItem == NULL)
        return;

    for (int32 i = 0; i < ITEM_LABEL_MASK_DWORDS; i++)
    {
        uint32 nValue = ptItem->pdwLabelMask[i];
        for (int32 nBit = 0; nValue != 0; nBit++)
        {
            if (nValue & 1)
            {
                ItemLabel itemLabel = (ItemLabel)(i * 32 + nBit);
                listLabels.AddTail_Tracked(itemLabel);
            }
            nValue >>= 1;
        }
    }
}



















BOOL ItemIsHalfOfSwordFistComboForMonk(GBID gbidItem)
{
    return ItemHasLabel(gbidItem, ITL_SWORD) || ItemHasLabel(gbidItem, ITL_AXE) || ItemHasLabel(gbidItem, ITL_MACE) || ItemHasLabel(gbidItem, ITL_DAGGER) || ItemHasLabel(gbidItem, ITL_SPEAR);
}






























BOOL ItemTypeIsOfType(GBID gbidItemType, GBID gbidItemTypeToCheckAgainst)
{
    if (gbidItemType == gbidItemTypeToCheckAgainst)
        return TRUE;
    if (gbidItemType == GBID_NONE || gbidItemTypeToCheckAgainst == GBID_NONE)
        return FALSE;
    const GameBalanceDefinition * pdefGB;
    GBHandle hGBItemType(GB_ITEMTYPES, gbidItemType);
    ItemTypeDefinition * ptItemTypeDef = (ItemTypeDefinition *)GBLockR(hGBItemType, pdefGB, TRUE);
    if (ptItemTypeDef == NULL)
        return FALSE;
    GBID gbidParentType = ptItemTypeDef->gbidParent;
    GBUnlockR(pdefGB);
    if (gbidParentType == GBID_NONE)
        return FALSE;
    return ItemTypeIsOfType(gbidParentType, gbidItemTypeToCheckAgainst);
}



















BOOL ItemIsOfType(GBHandle hgbItem, GBID gbidItemType)
{
    if (gbidItemType == GBID_NONE || hgbItem.gbid == GBID_NONE)
        return FALSE;
    ScopedItemDefinitionLockR ptItemDef(hgbItem.gbid);
    if (ptItemDef == NULL)
        return FALSE;
    GBID gbidThisItemsType = ptItemDef->gbidItemType;
    return ItemTypeIsOfType(gbidThisItemsType, gbidItemType);
}









































BOOL ActorCommonData::IsItem() const
{
    return hGB.eType == GB_ITEMS;
}









ItemTypeDefinition * ItemsItemTypeDefLockR(const GBID gbidItem, const GameBalanceDefinition *& pdefGBItemTypes)
{
    ScopedItemDefinitionLockR pItemDef(gbidItem);
    if (pItemDef == NULL)
        return NULL;
    GBHandle hGBItemType(GB_ITEMTYPES, pItemDef->gbidItemType);
    ItemTypeDefinition * pItemTypeDef = (ItemTypeDefinition *)GBLockR(hGBItemType, pdefGBItemTypes, TRUE);
    return pItemTypeDef;
}








static inline BOOL sItemTestItemTypeFlag(const GBID gbidItem, const int32 nBit)
{
    const GameBalanceDefinition * pdefGBItemTypes;
    ItemTypeDefinition * pItemTypeDef = ItemsItemTypeDefLockR(gbidItem, pdefGBItemTypes);
    BOOL fRetVal = FALSE;
    if (pItemTypeDef != NULL)
    {
        fRetVal = (pItemTypeDef->dwFlags >> nBit) & 1;
        GBUnlockR(pdefGBItemTypes);
    }
    return fRetVal;
}







BOOL ItemIsTwoHanded(const ActorCommonData & tACDItem)
{
    if (!tACDItem.IsItem())
        return FALSE;
    return ItemHasLabel(tACDItem.hGB.gbid, ITL_2_HAND) != FALSE;
}













BOOL ItemIsOneHandedWeapon(const ActorCommonData & tACDItem)
{
    if (!tACDItem.IsItem())
        return FALSE;
    GBID gbidItem = tACDItem.hGB.gbid;
    WeaponClass eClass = GetWeaponClass(gbidItem, GBID_NONE);
    switch (eClass)
    {
    case WEAPONCLASS_1HSWING:
    case WEAPONCLASS_1HTHRUST:
    case WEAPONCLASS_1HFIST:
    case WEAPONCLASS_HANDXBOW:
        return TRUE;
    default:
        return FALSE;
    }
}




























BOOL ItemUsesQuiver(const ActorCommonData & tACDItem)
{
    if (!tACDItem.IsItem())
        return FALSE;
    GBID gbidItem = tACDItem.hGB.gbid;
    if (ItemHasLabel(gbidItem, ITL_BOW) || ItemHasLabel(gbidItem, ITL_CROSSBOW) || ItemHasLabel(gbidItem, ITL_HAND_CROSSBOW))
        return TRUE;
    WeaponClass eClass = GetWeaponClass(gbidItem, GBID_NONE);
    switch (eClass)
    {
    case WEAPONCLASS_1HSWING:
    case WEAPONCLASS_1HTHRUST:
    case WEAPONCLASS_HANDXBOW:
        return TRUE;
    default:
        return FALSE;
    }
}














BOOL ItemIsUsable(const ActorCommonData & tACDItem)
{
    if (sItemTestItemTypeFlag(tACDItem.hGB.gbid, IT_USEABLE_BIT))
        return TRUE;
    return FALSE;
}








BOOL ItemIsSocketable(const ActorCommonData & tACDItem)
{
    return sItemTestItemTypeFlag(tACDItem.hGB.gbid, IT_SOCKETABLE_BIT);
}





BOOL ItemIsSocketed(const ActorCommonData & tACDItem)
{
    const ActorLocation & tLocation = tACDItem.GetLocation();
    if (tLocation.eActorLocation != AL_INVENTORY)
        return FALSE;
    return ACDInventoryIsSlotSocket(tLocation.tInvLocation.eSlot);
}







BOOL ItemHasSockets(const ActorCommonData & tACDItem)
{
    if (!tACDItem.IsItem())
        return FALSE;
    return tACDItem.AttributesGetInt(ATTRIBUTE_SOCKETS) > 0;
}







SNO ItemGetLoreSNO(const ActorCommonData & tACDItem)
{
    return ActorDefinitionGetTagMapValueSNO(TAG_ACTOR_LORE, tACDItem.snoActor);
}





ItemEnchantAffixError ItemCanEnchantAffix(const ActorCommonData & tACDItem)
{
    if (ItemIsUnidentified(tACDItem))
        return IEAE_UNIDENTIFIED;
    if (ItemIsSocketable(tACDItem))
        return IEAE_SOCKETABLE;
    if (ItemGetQualityLevel(tACDItem) < IQL_RARE_AFFIX_START)
        return IEAE_INVALID_QUALITY;
    if (!tACDItem.AttributesGetInt(ATTRIBUTE_LOOT_2_0_DROP))
        return IEAE_LEGACY_ITEM;
    if (tACDItem.listIdentifiedAffixGBIDs.Count() == 0)
        return IEAE_NO_RANDOM_AFFIXES;
    if (GameActiveGameCommonDataIsClient())
    {
        Player * ptPlayer = PlayerGetByACD(tACDItem.GetInventoryContainerOwner());
        if (ptPlayer != NULL)
        {
            ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
            if (ptACDPlayer != NULL && !ItemPlayerHasRequiredCrafterLevelForItemType(*ptACDPlayer, tACDItem))
                return IEAE_ITEM_TYPE_ABOVE_CRAFTER_LEVEL;
        }
    }
    return IEAE_NONE;
}











































void ItemSetIdentifyCost(ActorCommonData & tACDItem, const int32 nRating)
{



    FastAttribSetValueInt(tACDItem.idPassiveAttribGroup, ATTRIBUTE_IDENTIFY_COST, nRating);
}




int32 SItemComputeIdentifyCost(const ActorCommonData & tACDItem)
{
    int32 nRating = 0;
    if (ItemIsUnidentified(tACDItem))
    {
        {
            ScopedItemDefinitionLockR ptItemDef(tACDItem.hGB.gbid);
            if (ptItemDef != NULL)
                nRating = ptItemDef->nIdentifyCost;
        }
        POS pos = tACDItem.listUnidentifiedAffixGBIDs.GetTailPosition();
        while (pos)
        {
            GBID gbidAffix = tACDItem.listUnidentifiedAffixGBIDs.GetPrev(pos);
            ScopedItemAffixLockR ptAffix(gbidAffix);
            if (ptAffix != NULL)
                nRating += ptAffix->nIdentifyCost;
        }
    }
    return nRating;
}






















BOOL ItemIsUnidentified(const ActorCommonData & tACDItem)
{
    if (!tACDItem.IsItem())
        return FALSE;
    return tACDItem.AttributesGetInt(ATTRIBUTE_UNIDENTIFIED) != 0;
}

















































BOOL ItemDeletedAfterUse(const ActorCommonData &)
{
    return TRUE;
}






BOOL ItemIsEquipped(const ActorCommonData & tACDItem)
{
    return tACDItem.AttributesGetInt(ATTRIBUTE_ITEM_EQUIPPED) != 0;
}




int32 ItemGetLevelRequirement(const ItemDefinition * ptItemDef, BOOL bCrafted)
{
    int32 nRequiredLevel = bCrafted ? ptItemDef->nCrafterRequiredLevel : ptItemDef->nNormalRequiredLevel;
    return nRequiredLevel;
}
















// NON_MATCHING: 628 bytes both, 2 instructions differ: the original selects a folded constant 61 (mov #0x3d) at 3839 where ours computes override - 3 (sub); writing the constant 61 instead (ternary or if) lets clang if-convert the > 0 test (5 differ).
int32 ItemGetLevelRequirement(
    const GBID gbidItem,
    const ActorCommonData * ptACDItem,
    BOOL bIgnoreLevelReductionAffixes)
{


    if (ItemHasLabel(gbidItem, ITL_HEALTHPOTION))
        return 0;




    int32 nRequiredLevel = 0;
    const GameBalanceDefinition * pdefGBItems;
    GBHandle hGBItem(GB_ITEMS, gbidItem);
    const ItemDefinition * ptItemDef = (const ItemDefinition *)GBLockR(hGBItem, pdefGBItems, TRUE);
    if (ptItemDef != NULL)
    {
        BOOL bCrafted = FALSE;
        if (ptACDItem)  // if-form inferred: the null test sits on 3794 and the attribute call on 3796
        {
            bCrafted = ptACDItem->AttributesGetInt(ATTRIBUTE_IS_CRAFTED);
        }

        nRequiredLevel = ItemGetLevelRequirement(ptItemDef, bCrafted);

        if (!ItemIsLegendary(gbidItem))
        {
            if (ptACDItem && ptACDItem->listIdentifiedAffixGBIDs.Count() > 0)
            {
                POS pos = ptACDItem->listIdentifiedAffixGBIDs.GetHeadPosition();
                while (pos)
                {
                    GBID gbidAffix = ptACDItem->listIdentifiedAffixGBIDs.GetNext(pos);
                    int32 nAffixLevel = sGetAffixLevelRequirement(*ptACDItem, gbidAffix);

                    nRequiredLevel = Math::Max(nRequiredLevel, nAffixLevel);
                }
            }
        }
        else
        {
            if (ptACDItem)
            {
                int32 nLegendaryItemLevelOverride = ptACDItem->AttributesGetInt(ATTRIBUTE_ITEM_LEGENDARY_ITEM_LEVEL_OVERRIDE);
                if (nLegendaryItemLevelOverride > 0)
                {
                    const int32 LEGENDARY_LEVEL_REQUIREMENT_REDUCTION = 3;

                    const int32 LEGENDARY_LEVEL_MAX = 64;













                    // ternary inferred: compare, select and assignment all sit on 3839
                    nRequiredLevel = (nLegendaryItemLevelOverride == LEGENDARY_LEVEL_MAX) ? (nLegendaryItemLevelOverride - LEGENDARY_LEVEL_REQUIREMENT_REDUCTION) : nLegendaryItemLevelOverride;
                }
            }
        }










        GBUnlockR(pdefGBItems);

        if (ptACDItem && !bIgnoreLevelReductionAffixes)
        {
            int32 nRequiredLevelReduction = ptACDItem->AttributesGetInt(ATTRIBUTE_ITEM_LEVEL_REQUIREMENT_REDUCTION);
            if (nRequiredLevelReduction > 0)
            {
                nRequiredLevel = Math::Max(nRequiredLevel - ptACDItem->AttributesGetInt(ATTRIBUTE_ITEM_LEVEL_REQUIREMENT_REDUCTION), 1);
            }


            int32 nRequiredLevelOverride = ptACDItem->AttributesGetInt(ATTRIBUTE_ITEM_LEVEL_REQUIREMENT_OVERRIDE);
            if (nRequiredLevelOverride > 0)
                nRequiredLevel = nRequiredLevelOverride;



            BOOL bRemoveLevelRequirement = ptACDItem->AttributesGetInt(ATTRIBUTE_REMOVE_LEVEL_REQUIREMENT);
            if (bRemoveLevelRequirement)
                nRequiredLevel = 1;
        }
    }




    if (ptACDItem)
    {

        int32 nConsoleMaxLevel = ptACDItem->AttributesGetInt(ATTRIBUTE_ITEM_CONSOLE_MAX_LEVEL);
        if (nConsoleMaxLevel > 0)
        {
            nRequiredLevel = Math::Min(nRequiredLevel, nConsoleMaxLevel);
        }
    }

    return nRequiredLevel;

}



int32 ItemGetItemLevel(const GBID gbidItem)
{
    int32 nItemLevel = 0;
    const GameBalanceDefinition * pdefGBItems;
    const ItemDefinition * ptItemDef = (const ItemDefinition *)GBLockR(GBHandle(GB_ITEMS, gbidItem), pdefGBItems, TRUE);
    if (ptItemDef != NULL)
    {
        nItemLevel = ptItemDef->nItemLevel;
        GBUnlockR(pdefGBItems);
    }
    return nItemLevel;
}


























































































































int32 ItemGetBaseCost(const GBHandle & hGB, const CurrencyType eCurrencyType)
{
    ScopedItemDefinitionLockR pItemDef(hGB.gbid);
    if (pItemDef == NULL)
        return 0;
    if (eCurrencyType == CURRENCY_BLOODSHARDS)
        return pItemDef->nBaseCostSpecialCurrency;
    if (eCurrencyType == CURRENCY_GOLD)
        return pItemDef->nBaseCostGold;










    XASSERT(eCurrencyType == CURRENCY_PLATINUM);
    return pItemDef->nBaseCostSpecialCurrency2;
}




ItemSellError ItemBaseCanBeSold(const GBHandle & hGB)
{
    if (ItemHasLabel(hGB.gbid, ITL_QUEST))
        return ISE_UNKNOWN;
    ScopedItemDefinitionLockR ptItem(hGB.gbid);
    if (ptItem != NULL && (ptItem->dwFlags & (1 << ITEM_CANNOT_BE_SOLD_BIT)))
        return ISE_UNKNOWN;
    return ISE_NONE;
}















ItemSellError ItemCanBeSold(const ActorCommonData & tACDItem, const ActorCommonData * ptACDVendor)
{
    if (ItemIsUnidentified(tACDItem))
        return ISE_UNIDENTIFIED;
    if (ptACDVendor != NULL && (ptACDVendor->GetCurrencyType() == CURRENCY_BLOODSHARDS || ptACDVendor->GetCurrencyType() == CURRENCY_PLATINUM))
        return ISE_UNKNOWN;
    return ItemBaseCanBeSold(tACDItem.hGB);
}













uint64 ItemGetCost(
    const ActorCommonData & tACDItem,
    const CostType eCostType,
    const CurrencyType eCurrencyType,
    const ActorCommonData * ptACDVendor,
    const BOOL bConsiderStackSize,
    BOOL bIgnoreSockets)
{
    if (!tACDItem.IsItem())
    {
        return 0;
    }


    if (eCostType == CT_SELL &&
        ItemCanBeSold(tACDItem, ptACDVendor) != ISE_NONE)
    {
        return 0;
    }

    if (eCurrencyType == CURRENCY_BLOODSHARDS || eCurrencyType == CURRENCY_PLATINUM)
    {

        return ItemGetBaseCost(tACDItem.hGB, eCurrencyType);
    }

    if (eCostType == CT_GEM_REMOVAL)
    {
        uint64 uCost = 0;
        ScopedItemDefinitionLockR ptItem(tACDItem.hGB.gbid);
        if (ptItem != NULL) uCost = ptItem->nRemoveGemCost > 0 ? ptItem->nRemoveGemCost : 0;
        return uCost;
    }





    uint64 uStackSize = tACDItem.GetStackCount();


    if (eCostType == CT_SELL || eCostType == CT_BUYBACK)
    {





        if (ItemGetBoundToACD(tACDItem, TRUE) != ACDID_INVALID)
        {
            ScopedItemDefinitionLockR ptItem(tACDItem.hGB.gbid);
            if (ptItem != NULL)
            {
                if ((ptItem->dwFlags & (1 << ITEM_BIND_ON_PICKUP)) && !(ptItem->dwFlags & (1 << ITEM_SELLS_NORMALLY_WHEN_BOUND_BIT)))
                {
                    uint64 uBoundCost = 1;

                    if (bConsiderStackSize && uStackSize > 0)
                    {
                        uBoundCost = uStackSize;
                    }
                    return uBoundCost;
                }
            }
        }
    }
    int32 nFilledSockets = 0;
    int32 nBaseCost = ItemGetBaseCost(tACDItem.hGB, eCurrencyType);
    int32 nCost = nBaseCost;

    ActorLocation tLocation = tACDItem.GetLocation();


    int32 nNumSockets = tACDItem.AttributesGetInt(ATTRIBUTE_SOCKETS);

    if (!bIgnoreSockets && nNumSockets > 0)
    {



        nCost = Math::FloatRoundToInt(nCost * (nNumSockets * 0.05f + 1.1f));


        for (int32 nSocket = 0; nNumSockets <= 3 && nSocket != nNumSockets; nSocket++)
        {


            IVector2D tPoint;
            tPoint.Set(0, nSocket);
            ACDID idACDSocketedItem = ACDInventoryGetSquareItem(tACDItem, SLOT_ITEM_SOCKETS, tPoint);
            ActorCommonData * ptACDSocketedItem = ACDTryToGet(idACDSocketedItem);
            if (ptACDSocketedItem != NULL)
            {


                int32 nSocketedCost = ItemGetCost(*ptACDSocketedItem, eCostType, eCurrencyType, ptACDVendor, FALSE, FALSE);
                nCost += nSocketedCost;
            }
        }
    }




    POS pos = tACDItem.listIdentifiedAffixGBIDs.GetHeadPosition();
    while (pos)
    {
        GBID gbidAffix = tACDItem.listIdentifiedAffixGBIDs.GetNext(pos);
        ScopedItemAffixLockR ptAffix(gbidAffix);


        nCost += ptAffix->nCost;
    }


    float flCostBonus = tACDItem.AttributesGetFloat(ATTRIBUTE_ITEM_COST_PERCENT_BONUS);
    if (flCostBonus > 0.000001f)
    {
        nCost += Math::FloatRoundToInt(nCost * flCostBonus);
    }


    if (eCostType == CT_SELL || eCostType == CT_BUYBACK)
    {

        ScopedGlobalDefinitionLockR pdefGlobal;
        nCost = Math::FloatRoundToInt(nCost * pdefGlobal->flSellPricePercent);

        ScopedItemDefinitionLockR ptItem(tACDItem.hGB.gbid);
        if (ptItem != NULL && ptItem->nSellOverrideCost > 0)
        {
            nCost = ptItem->nSellOverrideCost;
        }
    }
    else if (eCostType == CT_BUY)
    {
        float flVendorMarkupPct = 1.0f;
        if (ptACDVendor != NULL)
        {
            flVendorMarkupPct = ActorDefinitionGetTagMapValueFloat(TAG_ACTOR_VENDOR_PRICE_MARKUP, ptACDVendor->snoActor);
        }
        nCost = Math::FloatRoundToInt(nCost * flVendorMarkupPct);
    }


    ItemQualityLevel eIQL = ItemGetQualityLevel(tACDItem);

    float flFinalMult = (eIQL == IQL_INFERIOR) ? 0.75f : (eIQL == IQL_SUPERIOR) ? 1.25f : 1.0f;













    nCost = Math::FloatRoundToInt(nCost * flFinalMult);


    nCost = Math::Max(nCost, 1);





    uint64 uFinalCost = nCost;

    if (bConsiderStackSize && uStackSize > 1)
    {
        uFinalCost *= uStackSize;
    }


    return uFinalCost;
}



BOOL ItemCanBeSelectedOnGround(const ActorCommonData & tACDItem)
{
    if (tACDItem.IsItem() && ItemHasLabel(tACDItem.hGB.gbid, ITL_GLYPH))
        return FALSE;
    return TRUE;
}










BOOL ItemIsAutoPickup(const ActorCommonData & tACDItem)
{
    BOOL bAutoPickup = FALSE;
    if (ItemIsGlyph(tACDItem) || ItemIsHoradricReagent(tACDItem) || (ItemIsHealthPotion(tACDItem) && !ItemIsLegendary(tACDItem.hGB.gbid)))
    {
        bAutoPickup = TRUE;
    }
    else if (ItemIsCurrency(tACDItem))
    {
        ItemAppGlobals & tItemAppGlobals = *sgtAppGlobals.ptItemAppGlobals;
        CurrencyType eCurrencyType = CurrencyFromGBID(tACDItem.hGB.gbid);
        bAutoPickup = tItemAppGlobals.tCurrencyToCurrencyInfoMap[eCurrencyType].bAutoPickup;
    }
    if (bAutoPickup && !tACDItem.AttributesGetInt(ATTRIBUTE_NO_AUTO_PICKUP))
        return TRUE;
    return FALSE;
}

































BOOL ItemIsGold(const ActorCommonData & tACDItem)
{
    if (!tACDItem.IsItem())
        return FALSE;
    return ItemHasLabel(tACDItem.hGB.gbid, ITL_GOLD) != FALSE;
}













BOOL ItemIsGlyph(const ActorCommonData & tACDItem)
{
    if (!tACDItem.IsItem())
        return FALSE;
    return ItemHasLabel(tACDItem.hGB.gbid, ITL_GLYPH) != FALSE;
}













BOOL ItemIsQuest(const ActorCommonData & tACDItem)
{
    if (!tACDItem.IsItem())
        return FALSE;
    return ItemHasLabel(tACDItem.hGB.gbid, ITL_QUEST) != FALSE;
}













BOOL ItemIsHealthPotion(const ActorCommonData & tACDItem)
{
    if (!tACDItem.IsItem())
        return FALSE;
    return ItemHasLabel(tACDItem.hGB.gbid, ITL_HEALTHPOTION);
}

















BOOL ItemIsCurrency(const ActorCommonData & tACDItem)
{
    return ItemIsCurrency(tACDItem.hGB.gbid);
}





BOOL ItemIsCurrency(GBID gbidItem)
{
    const Item * ptItem = ItemsGetItem(gbidItem);
    if (ptItem == NULL)
        return FALSE;
    return (ptItem->dwFlags >> ITEM_IS_CURRENCY_BIT) & 1;
}









BOOL ItemUnlocksTransmogOnPickup(const ActorCommonData & tACDItem)
{
    if (ItemIsUnidentified(tACDItem))
        return FALSE;
    return TRUE;
}








































int32 ItemGetMaxNumberOfSocketsFromGBID(const GBID & gbidItem)
{
    ScopedItemDefinitionLockR ptItemDef(gbidItem);
    if (ptItemDef != NULL)
    {
        ScopedItemTypeDefinitonLockR ptItemTypeDef(ptItemDef->gbidItemType);
        if (ptItemTypeDef != NULL)
            return ptItemTypeDef->nMaxSockets;
    }
    return 0;
}







int32 ItemGetMaxNumberOfSockets(const ActorCommonData & tACDItem)
{

    return tACDItem.IsItem() ? ItemGetMaxNumberOfSocketsFromGBID(tACDItem.hGB.gbid) : 0;
}









SocketReason ItemCanSocketInto(
    const ActorCommonData & tACDItemToSocket,
    ActorCommonData & tACDItemSocketedInto)
{
    if (!ItemIsSocketable(tACDItemToSocket) ||
        !ItemHasSockets(tACDItemSocketedInto))
    {
        return SOCKET_REASON_ITEM_NOT_SOCKETABLE;
    }



    ACDID idACDOwner = tACDItemSocketedInto.GetInventoryContainerOwner();
    ActorCommonData * ptACDOwner = ACDTryToGet(idACDOwner);
    if (ptACDOwner == NULL)
    {
        return SOCKET_REASON_ITEM_NOT_SOCKETABLE;
    }

    if (ItemIsUnidentified(tACDItemToSocket) ||
        ItemIsUnidentified(tACDItemSocketedInto))
    {
        return SOCKET_REASON_UNIDENTIFIED;
    }


    int32 nSocketsFilled = ItemGetNumFilledSockets(tACDItemSocketedInto);
    int32 nSocketsTotal = tACDItemSocketedInto.AttributesGetInt(ATTRIBUTE_SOCKETS);
    if (nSocketsFilled >= nSocketsTotal)
    {
        return SOCKET_REASON_NO_FREE_SOCKETS;
    }

    if (ItemIsEquipped(tACDItemSocketedInto))
    {

        int32 nRequiredLevel = ItemGetLevelRequirement(tACDItemToSocket.hGB.gbid, &tACDItemToSocket, 0);
        int32 nOwnerLevel = ptACDOwner->AttributesGetInt(ATTRIBUTE_LEVEL);
        if (nOwnerLevel < nRequiredLevel)
        {
            return SOCKET_REASON_LEVEL_RESTRICTION_NOT_MET;
        }
    }

    GBID gbidItemToSocket = tACDItemToSocket.hGB.gbid;
    GBHandle hgbItemSocketedInto = tACDItemSocketedInto.hGB;
    GBHandleList lstEffects(GBGetHandlePool());
    GBEnumerate(GB_SOCKETED_EFFECTS, lstEffects);

    const GameBalanceDefinition * pdefGB;
    POS pos = lstEffects.GetHeadPosition();

    BOOL fFound = FALSE;
    BOOL fHasSocketedEffects = FALSE;
    while (pos && !fFound)
    {
        const GBHandle & hGB = lstEffects.GetNext(pos);
        SocketedEffectsTableEntry * ptEffectDef = (SocketedEffectsTableEntry *)GBLockR(hGB, pdefGB, TRUE);


        if (ptEffectDef->gbidItemBeingSocketed == gbidItemToSocket)
        {
            fHasSocketedEffects = TRUE;

            if (ptEffectDef->gbidItemTypeSocketedInto == StringToGBID("All") ||
                ItemIsOfType(hgbItemSocketedInto, ptEffectDef->gbidItemTypeSocketedInto))
            {
                fFound = TRUE;
            }

        }
        GBUnlockR(pdefGB);
    }


    if (ItemIsJewel(tACDItemToSocket))
    {
        if (fHasSocketedEffects && !fFound)
        {
            return SOCKET_REASON_TYPE_MISMATCH;
        }
    }
    else
    if (!fFound)
    {
        return SOCKET_REASON_TYPE_MISMATCH;
    }


    if (ItemIsUniqueEquipped(tACDItemToSocket))
    {
        GBID gbidUniqueEquippedSocketables[3];

        int32 nUniqueEquippedSocketables = ItemGetSocketedUniqueEquipped(tACDItemSocketedInto, gbidUniqueEquippedSocketables, 3);


        for (int32 s = 0; s < nUniqueEquippedSocketables; s++)
        {
            if (tACDItemToSocket.hGB.gbid == gbidUniqueEquippedSocketables[s])
            {
                return SOCKET_REASON_UNIQUE_EQUIPPED;
            }
        }

        BOOL bIsInBodySlot = tACDItemSocketedInto.tLocation.eActorLocation == AL_INVENTORY &&
            tACDItemSocketedInto.tLocation.tInvLocation.eSlot >= SLOT_PLAYER_BODYSLOT_START &&
            tACDItemSocketedInto.tLocation.tInvLocation.eSlot < SLOT_PLAYER_SHARED_STASH;


        if ((ItemIsEquipped(tACDItemSocketedInto) || bIsInBodySlot) &&
            ActorInventoryHasUniqueEquippedSocketablesEquipped(*ptACDOwner, &tACDItemToSocket.hGB.gbid, 1, tACDItemSocketedInto.id, SLOT_NONE))
        {
            return SOCKET_REASON_UNIQUE_EQUIPPED;
        }
    }


    return SOCKET_REASON_SUCCESS;
}


static int32 sItemTypeGetNumParents(GBID gbidItemType)
{
    int32 nNumParents = -1;

    const ItemType * ptItemType = ItemsGetItemType(gbidItemType);

    while (ptItemType != NULL)
    {
        nNumParents++;
        ptItemType = ptItemType->ptParentType;
    }
    return nNumParents;
}



void ItemModifyAttributesFromSpecifiers(ActorCommonData * ptACDItem, const AttributeSpecifier * ptAttributes, const int32 nNumAttributes, const float flAttributeMultiplier)
{
    ActorCommonData * ptACDOwner = ACDTryToGet(ptACDItem->GetInventoryContainerOwner());
    for (int32 i = 0; i < nNumAttributes; i++)
    {
        Attrib eAttribute = ptAttributes[i].eAttribute;
        if (eAttribute == ATTRIBUTE_NONE)
            continue;

        int32 nAttributeParam = ptAttributes[i].nParam;
        FastAttribKey tKey = MakeAttribKey(eAttribute, nAttributeParam);
        AttributeDataType eAttribType = KeyGetDataType(tKey);
        float flAttributeValue = ItemGetAttributeSpecifierValue(ptACDItem->hGB.gbid, ptAttributes[i], ptACDOwner, ptACDItem->ptRNG, 0, FALSE) * flAttributeMultiplier;
        FastAttribValue tRunningTotal = FastAttribGetValue(ptACDItem->idFastAttrib, tKey);
        FastAttribValue tValue(flAttributeValue);
        if (eAttribType != ATTRIBUTEDT_FLOAT)
            tValue.nValue = (int32)flAttributeValue;
        AttributeOperator eOperator = KeyGetAggregationOperator(tKey);
        FastAttribAggregate(tRunningTotal, tValue, eAttribType, eOperator, tKey);
        FastAttribSetValue(ptACDItem->idFastAttrib, tKey, tRunningTotal);
    }

    if (GameActiveGameCommonDataIsServer())
        ptACDItem->ModifiedMark(0);
}












void ItemRemoveAttributesFromSpecifiers(ActorCommonData * ptACDItem, const AttributeSpecifier * ptAttributes, const int32 nNumAttributes)
{


    for (int32 i = 0; i < nNumAttributes; i++)
    {

        Attrib eAttribute = ptAttributes[i].eAttribute;
        if (eAttribute == ATTRIBUTE_NONE)
        {
            continue;
        }

        int32 nAttributeParam = ptAttributes[i].nParam;
        FastAttribKey tKey = MakeAttribKey(eAttribute, nAttributeParam);

        FastAttribSetValueToDefault(ptACDItem->idFastAttrib, tKey);
    }

    if (GameActiveGameCommonDataIsServer())
    {
        ptACDItem->ModifiedMark(0);
    }
}





















































































BOOL ItemRemoveSocketedItemAttributes(ActorCommonData & tACDItemToSocket, const ActorCommonData & tACDItemSocketedInto)
{


    GBID gbidItemToSocket = tACDItemToSocket.hGB.gbid;
    GBHandle hgbItemSocketedInto = tACDItemSocketedInto.hGB;

    if (ItemIsJewel(gbidItemToSocket))
    {
        return TRUE;
    }

    if (ItemIsGem(gbidItemToSocket))
    {
        GBHandleList lstEffects(GBGetHandlePool());
        GBEnumerate(GB_SOCKETED_EFFECTS, lstEffects);


        POS pos = lstEffects.GetHeadPosition();
        int32 nMostParents = -2;
        const GameBalanceDefinition * pdefGB;
        GBHandle hGBHigestTypeEffect = cghgbNone;
        while (pos)
        {
            const GBHandle & hGB = lstEffects.GetNext(pos);
            SocketedEffectsTableEntry * ptEffectDef = (SocketedEffectsTableEntry *)GBLockR(hGB, pdefGB, TRUE);


            if (ptEffectDef->gbidItemBeingSocketed == gbidItemToSocket &&
                (ptEffectDef->gbidItemTypeSocketedInto == StringToGBID("All") ||
                 ItemIsOfType(hgbItemSocketedInto, ptEffectDef->gbidItemTypeSocketedInto)))
            {

                int32 nNumParentsOfType = -1;
                if (ptEffectDef->gbidItemTypeSocketedInto != StringToGBID("All"))
                {
                    nNumParentsOfType = sItemTypeGetNumParents(ptEffectDef->gbidItemTypeSocketedInto);
                }

                if (nNumParentsOfType > nMostParents)
                {
                    hGBHigestTypeEffect = hGB;
                    nMostParents = nNumParentsOfType;
                }
            }


            GBUnlockR(pdefGB);
        }


        if (hGBHigestTypeEffect.gbid != GBID_NONE)
        {
            SocketedEffectsTableEntry * ptEffectDef = (SocketedEffectsTableEntry *)GBLockR(hGBHigestTypeEffect, pdefGB, TRUE);
            ItemRemoveAttributesFromSpecifiers(&tACDItemToSocket, ptEffectDef->ptAttributes, 3);
            GBUnlockR(pdefGB);
        }
        else
        {

            XASSERT(FALSE);
        }
        return TRUE;
    }

    return FALSE;
}




BOOL ItemAddSocketedItemAttributes(ActorCommonData & tACDItemToSocket, const ActorCommonData & tACDItemSocketedInto)
{


    GBID gbidItemToSocket = tACDItemToSocket.hGB.gbid;
    GBHandle hgbItemSocketedInto = tACDItemSocketedInto.hGB;

    if (ItemIsJewel(gbidItemToSocket))
    {
        return TRUE;
    }

    if (ItemIsGem(gbidItemToSocket))
    {
        GBHandleList lstEffects(GBGetHandlePool());
        GBEnumerate(GB_SOCKETED_EFFECTS, lstEffects);


        POS pos = lstEffects.GetHeadPosition();
        int32 nMostParents = -2;
        const GameBalanceDefinition * pdefGB;
        GBHandle hGBHigestTypeEffect = cghgbNone;
        while (pos)
        {
            const GBHandle & hGB = lstEffects.GetNext(pos);
            SocketedEffectsTableEntry * ptEffectDef = (SocketedEffectsTableEntry *)GBLockR(hGB, pdefGB, TRUE);


            if (ptEffectDef->gbidItemBeingSocketed == gbidItemToSocket &&
                (ptEffectDef->gbidItemTypeSocketedInto == StringToGBID("All") ||
                 ItemIsOfType(hgbItemSocketedInto, ptEffectDef->gbidItemTypeSocketedInto)))
            {

                int32 nNumParentsOfType = -1;
                if (ptEffectDef->gbidItemTypeSocketedInto != StringToGBID("All"))
                {
                    nNumParentsOfType = sItemTypeGetNumParents(ptEffectDef->gbidItemTypeSocketedInto);
                }

                if (nNumParentsOfType > nMostParents)
                {
                    hGBHigestTypeEffect = hGB;
                    nMostParents = nNumParentsOfType;
                }
            }


            GBUnlockR(pdefGB);
        }


        if (hGBHigestTypeEffect.gbid != GBID_NONE)
        {
            SocketedEffectsTableEntry * ptEffectDef = (SocketedEffectsTableEntry *)GBLockR(hGBHigestTypeEffect, pdefGB, TRUE);
            float flAttributeMultiplier = tACDItemSocketedInto.AttributesGetFloat(ATTRIBUTE_GEM_ATTRIBUTES_MULTIPLIER) + 1.0f;
            flAttributeMultiplier = Math::Max(flAttributeMultiplier, 1.0f);
            ItemModifyAttributesFromSpecifiers(&tACDItemToSocket, ptEffectDef->ptAttributes, 3, flAttributeMultiplier);
            GBUnlockR(pdefGB);
        }
        else
        {

            XASSERT(FALSE);
        }
        return TRUE;
    }




    return FALSE;
}


int32 ItemGetNumFilledSockets(const ActorCommonData & tACDItem)
{
    return tACDItem.AttributesGetInt(ATTRIBUTE_SOCKETS_FILLED);
}














BOOL sItemGetSocketedUniqueEquippedCallback(const InventoryLocation & tInvLoc, void * pData)
{
    ItemGetSocketedUniqueEquippedData * tCallbackData = (ItemGetSocketedUniqueEquippedData *)pData;
    ActorCommonData & tACDHostItem = ACDGet(tInvLoc.idACDContainer);
    ACDID idACDSocketable = ACDInventoryGetSquareItem(tACDHostItem, tInvLoc.eSlot, tInvLoc.pt);
    if (idACDSocketable == ACDID_INVALID)
        return TRUE;
    ActorCommonData * ptACDSocketable = ACDTryToGet(idACDSocketable);
    if (ptACDSocketable != NULL && ItemIsSocketable(*ptACDSocketable) && ItemIsUniqueEquipped(*ptACDSocketable))
    {
        tCallbackData->gbidUniqueEquippedSocketables[tCallbackData->nUniqueEquippedSocketables] = ptACDSocketable->hGB.gbid;
        tCallbackData->nUniqueEquippedSocketables++;
        return tCallbackData->nUniqueEquippedSocketables < tCallbackData->nMaxUniqueEquippedSocketables;
    }
    return TRUE;
}





int32 ItemGetSocketedUniqueEquipped(const ActorCommonData & tACDItem, GBID * gbidUniqueEquippedSocketables, int32 nMaxUniqueEquippedSocketables)
{
    if (!tACDItem.IsItem())
        return 0;
    if (!ItemHasSockets(tACDItem) || gbidUniqueEquippedSocketables == NULL || nMaxUniqueEquippedSocketables <= 0)
        return 0;
    ItemGetSocketedUniqueEquippedData tCallbackData;
    tCallbackData.gbidUniqueEquippedSocketables = gbidUniqueEquippedSocketables;
    tCallbackData.nMaxUniqueEquippedSocketables = nMaxUniqueEquippedSocketables;
    tCallbackData.nUniqueEquippedSocketables = 0;
    ACDInventoryIterateSlot(tACDItem, SLOT_SOCKETS_START, NULL, sItemGetSocketedUniqueEquippedCallback, &tCallbackData, FALSE, FALSE);
    return tCallbackData.nUniqueEquippedSocketables;
}











float ItemGetAttributeSpecifierValue(const GBID gbidItem, const AttributeSpecifier & tAttribute, const ActorCommonData * ptACDOwner, RNG * ptRNGOverride, int32 nLegendaryItemLevelOverride, BOOL bPrimalAncient)
{
    int32 nAttributeParam = tAttribute.nParam;
    FastAttribKey tKey = MakeAttribKey(tAttribute.eAttribute, nAttributeParam);
    AttributeDataType eAttribType = KeyGetDataType(tKey);
    uint32 uFlags = 0;
    if (bPrimalAncient)
    {
        if (ItemAttributePrefersMinimum(gbidItem, tAttribute.eAttribute))
            uFlags = 8;
        else
            uFlags = 16;
    }
    float flAttributeValue = AffixGetFormulaValue(tAttribute.pbAttributeFormula, tAttribute.serAttributeFormula.dwSize, ptACDOwner, gbidItem, ptRNGOverride, uFlags, nLegendaryItemLevelOverride);
    return flAttributeValue;
}


















static void sAddAttributesFromSpecifiers(ActorCommonData & tACDItem, const AttributeSpecifier * ptAttributes, int32 nNumAttributes, BOOL bSimulateOnly, BOOL bUprankLegendaryItemLevel)
{





    ActorCommonData * ptACDOwner = ACDTryToGet(tACDItem.tLocation.tInvLocation.idACDContainer);
    sFastAttribAggregrateAttributesFromSpecifiers(tACDItem, tACDItem.idFastAttrib, ptAttributes, nNumAttributes, tACDItem.hGB.gbid, ptACDOwner, tACDItem.ptRNG, bSimulateOnly, bUprankLegendaryItemLevel);

    if (GameActiveGameCommonDataIsServer())
    {
        tACDItem.ModifiedMark(0);
    }
}




static void sFastAttribAggregrateAttributesFromSpecifiers(const ActorCommonData & tACD,
                                                          AttribGroupID idFastAttrib,
                                                          const AttributeSpecifier * ptAttributes,
                                                          int32 nNumAttributes,
                                                          const GBID gbidItem,
                                                          const ActorCommonData * ptACDOwner,
                                                          RNG * ptRNG,
                                                          BOOL bSimulateOnly,
                                                          BOOL bUprankLegendaryItemLevel)

{
    for (int32 i = 0; i < nNumAttributes; i++)
    {
        Attrib eAttribute = ptAttributes[i].eAttribute;
        if (eAttribute == ATTRIBUTE_NONE)
        {
            continue;
        }

        int32 nAttributeParam = ptAttributes[i].nParam;
        FastAttribKey tKey = MakeAttribKey(eAttribute, nAttributeParam);
        AttributeDataType eAttribType = KeyGetDataType(tKey);


        int32 nLegendaryItemLevelOverride = tACD.AttributesGetInt(ATTRIBUTE_ITEM_LEGENDARY_ITEM_LEVEL_OVERRIDE);



        if (nLegendaryItemLevelOverride == 0 && ItemIsLegendary(gbidItem) && ItemIsCrafted(tACD))
        {
            ScopedItemDefinitionLockR ptItem(gbidItem);
            nLegendaryItemLevelOverride = ptItem->nItemLevel;
        }

        if (bUprankLegendaryItemLevel && nLegendaryItemLevelOverride > 0)
        {


            nLegendaryItemLevelOverride += ItemIsAncientRank(tACD) ? 2 : 1;
        }






        BOOL bPrimalAncient = ItemGetAncientRank(tACD) == 2;
        float flAttributeValue = ItemGetAttributeSpecifierValue(gbidItem, ptAttributes[i], ptACDOwner, ptRNG, nLegendaryItemLevelOverride, bPrimalAncient);
        if (!bSimulateOnly)
        {
            FastAttribValue tValue(flAttributeValue); if (eAttribType != ATTRIBUTEDT_FLOAT) tValue.nValue = (int32)flAttributeValue;
            AttributeOperator eOperator = KeyGetAggregationOperator(tKey);
            if (eOperator == ATTRIBUTEOP_NOAGG)
            {
                FastAttribSetValue(idFastAttrib, tKey, tValue);
            }
            else
            {
                FastAttribValue tRunningTotal = FastAttribGetValue(idFastAttrib, tKey);
                FastAttribAggregate(tRunningTotal, tValue, eAttribType, eOperator, tKey);
                FastAttribSetValue(idFastAttrib, tKey, tRunningTotal);
            }
        }
    }
}




BOOL ItemsPreEquip(ActorCommonData & tACDItem, ActorCommonData & tACDContainer, const InventorySlotType)
{



    if (!tACDItem.IsItem())
    {
        return FALSE;
    }

    ItemDeleteAltAttribGroup(tACDItem);

    AttribGroupID idAltAttribGroup = FastAttribGroupCreate(FAT_ITEM, -1);
    if (idAltAttribGroup == -1)
    {
        return FALSE;
    }

    BOOL bUseAlt = FALSE;
    ResourceType eOwnerPrimaryResourceType = tACDContainer.GetPrimaryResourceType();
    ResourceType eOwnerSecondaryResourceType = tACDContainer.GetSecondaryResourceType();




    FastAttribIterator tIt(tACDItem.idFastAttrib, TRUE);
    while (FastAttribIterate(tIt))
    {
        FastAttribValue tValue;
        FastAttribKey tKey = tIt.tEntryBuffer.tKey;
        AttributeParameterType eParamType = KeyGetParamType(tKey);
        switch (eParamType)
        {
        case APT_RESOURCE:
        {
            ResourceType eCurrResourceType = KeyGetParam(tKey);
            if (eCurrResourceType != eOwnerSecondaryResourceType && eCurrResourceType != eOwnerPrimaryResourceType && !tACDContainer.IsItem())
            {
                bUseAlt = TRUE;
                continue;
            }
            break;
        }
        case APT_SKILL:
            ACDIsPlayer(tACDContainer.id);


            break;
        }
        tValue = FastAttribGetValue(tACDItem.idFastAttrib, tKey);
        FastAttribSetValue(idAltAttribGroup, tKey, tValue);
    }

    if (!bUseAlt)
    {
        FastAttribGroupDelete(idAltAttribGroup);
    }
    else
    {
        ItemGlobals * ptItemGlobals = GameGetActiveGameCommonData().ptItemGlobals;

        ptItemGlobals->mapAlternateAttributeGroups.SetAt_Tracked(tACDItem.id, idAltAttribGroup);
    }


    return TRUE;
}


BOOL ItemIsLegendary(GBID gbidItem)
{
    if (gbidItem == GBID_NONE)
        return FALSE;
    ScopedItemDefinitionLockR ptItem(gbidItem);
    if (ptItem == NULL)
        return FALSE;
    return (ptItem->dwFlags >> ITEM_LEGENDARY_BIT) & 1;
}










BOOL ItemIsGift(GBID gbidItem)
{
    if (gbidItem == GBID_NONE)
        return FALSE;
    return GlobalGBIDGet(GLOBALGBID_CONSOLE_FRIEND_GIFT) == gbidItem || GlobalGBIDGet(GLOBALGBID_CONSOLE_AVENGER_GIFT) == gbidItem;
}












BOOL ItemIsSpecial(GBID gbidItem)
{
    ScopedItemDefinitionLockR ptItem(gbidItem);
    if (ptItem == NULL)
        return FALSE;
    return (ptItem->dwFlags >> ITEM_SPECIAL_BIT) & 1;
}





















BOOL ItemIsIrreplaceable(GBID gbidItem)
{
    ScopedItemDefinitionLockR ptItem(gbidItem);
    if (ptItem == NULL)
        return FALSE;
    return (ptItem->dwFlags >> ITEM_IRREPLACEABLE_BIT) & 1;
}







void ItemGetBonusRandomAffixes(GBHandle hgbItem, int32 & nMajor, int32 & nMinor, int32 & nRandom)
{
    ScopedItemDefinitionLockR ptItem(hgbItem.gbid);
    if (ptItem != NULL)
    {
        nMajor += ptItem->nBonusMajorAffixes;
        nMinor += ptItem->nBonusMinorAffixes;
        nRandom += ptItem->nBonusRandomAffixes;
    }
}









BOOL ItemIsCrafted(const ActorCommonData & tACDItem)
{
    if (!tACDItem.IsItem())
        return FALSE;
    return tACDItem.AttributesGetInt(ATTRIBUTE_IS_CRAFTED) > 0;
}






















BOOL ItemIsUnique(const ActorCommonData & tACDItem)
{
    if (!tACDItem.IsItem())
        return FALSE;
    ScopedItemDefinitionLockR ptItem(tACDItem.hGB.gbid);
    if (ptItem == NULL)
        return FALSE;
    return (ptItem->dwFlags >> ITEM_UNIQUE_BIT) & 1;
}




BOOL ItemIsUniqueEquipped(const ActorCommonData & tACDItem)
{
    if (!tACDItem.IsItem())
        return FALSE;
    return ItemIsUniqueEquipped(tACDItem.hGB.gbid);
}







BOOL ItemIsUniqueEquipped(GBID gbidItem)
{
    if (!ItemIsLegendary(gbidItem))
        return FALSE;
    if (ItemHasLabel(gbidItem, ITL_2_HAND))
        return FALSE;

    ScopedItemDefinitionLockR pdefItem(gbidItem);
    if (pdefItem == NULL)
        return FALSE;
    if (pdefItem->gbidLegendaryFamily != GBID_NONE)
        return TRUE;

    ScopedItemTypeDefinitonLockR pItemTypeDef(pdefItem->gbidItemType);
    if (pItemTypeDef == NULL)
        return FALSE;

    if (ItemIsAllowedInSlot(SLOT_PLAYER_RIGHT_FINGER, pItemTypeDef) && ItemIsAllowedInSlot(SLOT_PLAYER_LEFT_FINGER, pItemTypeDef))
        return TRUE;
    if (ItemIsAllowedInSlot(SLOT_PLAYER_RIGHT_HAND, pItemTypeDef) && ItemIsAllowedInSlot(SLOT_PLAYER_LEFT_HAND, pItemTypeDef))
        return TRUE;

    if (ItemIsUpgradeableJewel(gbidItem))
        return TRUE;
    return FALSE;
}






















BOOL ItemHasEnchantedAffix(const ActorCommonData & tACDItem)
{
    return tACDItem.AttributesGetInt(ATTRIBUTE_ENCHANTED_AFFIX_OLD) != GBID_NONE && tACDItem.AttributesGetInt(ATTRIBUTE_ENCHANTED_AFFIX_NEW) != GBID_NONE;
}





GBID ItemGetEnchantedAffixOld(const ActorCommonData & tACDItem)
{
    if (!ItemHasEnchantedAffix(tACDItem))
        return GBID_NONE;
    return tACDItem.AttributesGetInt(ATTRIBUTE_ENCHANTED_AFFIX_OLD);
}






GBID ItemGetEnchantedAffixNew(const ActorCommonData & tACDItem)
{
    if (!ItemHasEnchantedAffix(tACDItem))
        return GBID_NONE;
    return tACDItem.AttributesGetInt(ATTRIBUTE_ENCHANTED_AFFIX_NEW);
}






uint32 ItemGetEnchantedAffixSeed(const ActorCommonData & tACDItem)
{
    if (!ItemHasEnchantedAffix(tACDItem))
        return 0;
    return tACDItem.AttributesGetInt(ATTRIBUTE_ENCHANTED_AFFIX_SEED);
}






BOOL ItemHasCubeEnchant(const ActorCommonData & tACDItem)
{
    return tACDItem.AttributesGetInt(ATTRIBUTE_CUBE_ENCHANTED_GEM_RANK) >= 0 && tACDItem.AttributesGetInt(ATTRIBUTE_CUBE_ENCHANTED_GEM_TYPE) != 0;
}





Attrib ItemGetAttributeForCubeEnchant(const ActorCommonData & tACDItem)
{
    if (!ItemHasCubeEnchant(tACDItem))
        return -1;
    GemType eGemType = tACDItem.AttributesGetInt(ATTRIBUTE_CUBE_ENCHANTED_GEM_TYPE);
    switch (eGemType)
    {
    case GEM_AMETHYST:
        return ATTRIBUTE_CUBE_ENCHANTED_VITALITY_ITEM;
    case GEM_EMERALD:
        return ATTRIBUTE_CUBE_ENCHANTED_DEXTERITY_ITEM;
    case GEM_RUBY:
        return ATTRIBUTE_CUBE_ENCHANTED_STRENGTH_ITEM;
    case GEM_TOPAZ:
        return ATTRIBUTE_CUBE_ENCHANTED_INTELLIGENCE_ITEM;
    default:
        return -1;
    }
}








float ItemGetValueForCubeEnchant(
    const ActorCommonData & tACDItem)
{
    if (!ItemHasCubeEnchant(tACDItem))
    {
        return 0.0f;
    }

    int32 nJewelRank = tACDItem.AttributesGetInt(ATTRIBUTE_CUBE_ENCHANTED_GEM_RANK);

    return nJewelRank * 5.0f;
}



int32 ItemGetRequiredCrafterLevelForEnchant(const ActorCommonData & tACDItem)
{
    if (tACDItem.IsItem())
    {
        const ItemType * ptItemType = ItemsGetItemType(ItemsGetItemTypeGB(tACDItem.hGB).gbid);
        if (ptItemType != NULL)
        {
            int32 nRootRequiredCrafterLevel = 0;
            for (; ptItemType != NULL; ptItemType = ptItemType->ptParentType)
            {
                int32 nLeafRequiredCrafterLevel = ptItemType->nReqCrafterLevelForEnchant;
                if (nLeafRequiredCrafterLevel > 0)
                    nRootRequiredCrafterLevel = nLeafRequiredCrafterLevel;
            }
            return nRootRequiredCrafterLevel;
        }
    }
    return 0x7fffffff;
}














GBHandle ItemsGetItemTypeGB(const GBHandle hGBItem)
{
    ScopedItemDefinitionLockR ptItem(hGBItem.gbid);
    GBHandle hGBItemType;
    if (ptItem == NULL)
        hGBItemType.Set(GB_ITEMTYPES, GBID_NONE);
    else
        hGBItemType.Set(GB_ITEMTYPES, ptItem->gbidItemType);
    return hGBItemType;
}









BOOL ItemCanBeMagical(GBID gbidItem)
{
    ScopedItemDefinitionLockR ptItem(gbidItem);
    BOOL fCanBeMagical = FALSE;
    if (ptItem != NULL && !(ptItem->dwFlags & (1 << ITEM_NEVER_MAGIC_BIT)))
        fCanBeMagical = ItemTypeCanBeMagical(ptItem->gbidItemType);
    return fCanBeMagical;
}












BOOL ItemTypeCanBeMagical(GBID gbidItemType)
{
    const GameBalanceDefinition * pdefGB;
    GBHandle hgbItemType(GB_ITEMTYPES, gbidItemType);
    ItemTypeDefinition * ptItemType = (ItemTypeDefinition *)GBLockR(hgbItemType, pdefGB, TRUE);
    BOOL fCanBeMagical = FALSE;
    if (ptItemType != NULL)
    {
        fCanBeMagical = !(ptItemType->dwFlags & (1 << IT_NEVER_MAGIC_BIT));
        GBUnlockR(pdefGB);
    }
    return fCanBeMagical;
}









AttribGroupID SItemGetAttributeGroup(const ActorCommonData & tACDItem)
{
    AttribGroupID idFastAttribGroup;
    ItemGlobals * ptItemGlobals = GameGetActiveGameCommonData().ptItemGlobals;
    if (ptItemGlobals->mapAlternateAttributeGroups.Lookup(tACDItem.id, idFastAttribGroup))
        return idFastAttribGroup;
    return tACDItem.idFastAttrib;
}















































void ItemSetBindingLevel(ActorCommonData & tACD, ItemBindingLevel eBindingType)
{
    if (GameActiveGameCommonDataIsServer())
        tACD.AttributesSetInt(ATTRIBUTE_ITEM_BINDING_LEVEL_OVERRIDE, eBindingType);
    else
        FastAttribSetValueInt(tACD.idFastAttrib, ATTRIBUTE_ITEM_BINDING_LEVEL_OVERRIDE, eBindingType);
}


ItemBindingLevel ItemGetBindingLevel(const ActorCommonData & tACD)
{
    ItemBindingLevel eOverride = (ItemBindingLevel)tACD.AttributesGetInt(ATTRIBUTE_ITEM_BINDING_LEVEL_OVERRIDE);
    if (eOverride != IBL_NONE)
        return eOverride;
    ScopedItemDefinitionLockR ptItem(tACD.hGB.gbid);
    if (ptItem == NULL)
        return IBL_NONE;
    if (ptItem->dwFlags & (1 << ITEM_BIND_ON_PICKUP))
        return IBL_ACCOUNT_PICKUP;
    return IBL_NONE;
}



















void ItemSetBoundToACD(ActorCommonData & tACD, ACDID idACDOwner)
{
}











void ItemSetLockedToACD(ActorCommonData & tACD, ACDID idACDOwner)
{
    if (GameActiveGameCommonDataIsServer())
        tACD.AttributesSetInt(ATTRIBUTE_ITEM_LOCKED_TO_ACD, idACDOwner);
    else
        FastAttribSetValueInt(tACD.idFastAttrib, ATTRIBUTE_ITEM_LOCKED_TO_ACD, idACDOwner);
}


BOOL ItemIsLockedFromTrade(const ActorCommonData & tACD)
{
    return ItemGetLockedFromTradeSecondsRemaining(tACD) != 0;
}




uint32 ItemGetLockedFromTradeSecondsRemaining(const ActorCommonData & tACD)
{
    uint64 uUnlockTime = tACD.GetUnlockTime();
    if (uUnlockTime != 0)
    {
        Blizzard::Time::UnixTime64 unixTime = Blizzard::Time::ToUnixTime64(Blizzard::Time::GetTimestamp());
        if (uUnlockTime >= unixTime * 1000000)
            return (uint32)(uUnlockTime / 1000000 - unixTime);
    }
    return 0;
}














void ItemSetPlayerRecipient(ActorCommonData & tACDItem, int32 nPlayerIndex)
{
    if (GameActiveGameCommonDataIsServer())
        tACDItem.AttributesSetInt(ATTRIBUTE_ITEM_PLAYER_RECIPIENT, nPlayerIndex);
    else
        FastAttribSetValueInt(tACDItem.idFastAttrib, ATTRIBUTE_ITEM_PLAYER_RECIPIENT, nPlayerIndex);
}



int32 ItemGetPlayerRecipient(const ActorCommonData & tACD)
{
    return tACD.AttributesGetInt(ATTRIBUTE_ITEM_PLAYER_RECIPIENT);
}







BOOL sCheckSocketBoundToACD(const InventoryLocation & tInvLoc, void * pCallBackData)
{
    ActorCommonData & tACD = ACDGet(tInvLoc.idACDContainer);
    ACDID idACDItem = ACDInventoryGetSquareItem(tACD, tInvLoc.eSlot, tInvLoc.pt);
    if (idACDItem == ACDID_INVALID)
        return TRUE;
    ActorCommonData * ptACDItem = ACDTryToGet(idACDItem);
    if (ptACDItem != NULL && ItemIsSocketable(*ptACDItem))
    {
        ACDID * pidACDOwnerAttribute = (ACDID *)pCallBackData;
        *pidACDOwnerAttribute = ItemGetBoundToACD(*ptACDItem, TRUE);
        return *pidACDOwnerAttribute == ACDID_INVALID;
    }
    return TRUE;
}




ACDID ItemGetBoundToACD(const ActorCommonData & tACD, BOOL bCheckSockets)
{
    ACDID idACDOwnerAttribute = (ACDID)tACD.AttributesGetInt(ATTRIBUTE_ITEM_BOUND_TO_ACD);
    if (idACDOwnerAttribute != ACDID_INVALID)
    {
        if (!GameActiveGameCommonDataIsClient())
            return idACDOwnerAttribute;
        idACDOwnerAttribute = ACDFromANN((ACDNetworkName)idACDOwnerAttribute);
    }
    if (!bCheckSockets || idACDOwnerAttribute != ACDID_INVALID)
        return idACDOwnerAttribute;
    if (!ItemHasSockets(tACD))
        return ACDID_INVALID;
    ACDInventoryIterateSlot(tACD, SLOT_SOCKETS_START, NULL, sCheckSocketBoundToACD, &idACDOwnerAttribute, FALSE, FALSE);
    return idACDOwnerAttribute;
}




ACDID ItemGetLockedToACD(const ActorCommonData & tACD)
{
    ACDID idACDOwnerAttribute = (ACDID)tACD.AttributesGetInt(ATTRIBUTE_ITEM_LOCKED_TO_ACD);
    if (idACDOwnerAttribute != ACDID_INVALID && GameActiveGameCommonDataIsClient())
        return ACDFromANN((ACDNetworkName)idACDOwnerAttribute);
    return idACDOwnerAttribute;
}






int32 ItemAffixGetNumSockets(GBID gbidItem, GBID gbidAffix)
{
    ScopedItemAffixLockR ptAffix(gbidAffix);
    int32 nSocketCount = 0;
    if (ptAffix != NULL)
    {
        for (int32 i = 0; i < 4; i++)
        {
            Attrib eAttribute = ptAffix->ptAffixAttributes[i].eAttribute;
            if (eAttribute == ATTRIBUTE_SOCKETS)
            {
                nSocketCount = (int32)ItemGetAttributeSpecifierValue(gbidItem, ptAffix->ptAffixAttributes[i], NULL, NULL, 0, FALSE);
                break;
            }
        }
    }
    return nSocketCount;
}









int32 ItemAffixGetFrequencyWeight(const AffixTableEntry * ptAffix, PlayerClass eTargetedPlayerClass, HirelingClass eTargetedHirelingClass)
{
    int32 nWeight = ptAffix->nFrequencyWeight;
    if (eTargetedPlayerClass != PLAYER_CLASS_NONE)
        nWeight *= ptAffix->nClassFrequencyWeight[eTargetedPlayerClass];
    else if (eTargetedHirelingClass != 0)
        nWeight *= ptAffix->nHirelingFrequencyWeight[eTargetedHirelingClass];
    return nWeight;
}















int32 ItemGetNumPredeterminedSockets(GBID gbidItem)
{
    ScopedItemDefinitionLockR ptItemDef(gbidItem);
    int32 nSockets = 0;
    if (ptItemDef != NULL)
    {
        for (int32 i = 0; i < (int32)(ptItemDef->serAttributes_Console.dwSize / sizeof(AttributeSpecifier)); i++)
        {
            if (ptItemDef->ptAttributes[i].eAttribute == ATTRIBUTE_SOCKETS)
                nSockets += (int32)ItemGetAttributeSpecifierValue(gbidItem, ptItemDef->ptAttributes[i], NULL, NULL, 0, FALSE);
        }
    }
    return nSockets;
}









int32 ItemGetNumAffixedSockets(const ActorCommonData & tACDItem)
{
    GBID gbidEnchantedAffixOld;
    int32 uSocketCount;
    const GBIDPooledList & listAffixGBIDs = ItemIsUnidentified(tACDItem) ? tACDItem.listUnidentifiedAffixGBIDs : tACDItem.listIdentifiedAffixGBIDs;
    POS pos = listAffixGBIDs.GetHeadPosition();
    if (ItemIsUnidentified(tACDItem) || !ItemHasEnchantedAffix(tACDItem))
    {
        gbidEnchantedAffixOld = GBID_NONE;
        uSocketCount = 0;
    }
    else
    {
        gbidEnchantedAffixOld = ItemGetEnchantedAffixOld(tACDItem);
        GBID gbidEnchantedAffixNew = ItemGetEnchantedAffixNew(tACDItem);
        uSocketCount = ItemAffixGetNumSockets(tACDItem.hGB.gbid, gbidEnchantedAffixNew);
    }
    while (pos)
    {
        GBID gbidAffix = listAffixGBIDs.GetNext(pos);
        if (gbidAffix == gbidEnchantedAffixOld)
            continue;
        uSocketCount += ItemAffixGetNumSockets(tACDItem.hGB.gbid, gbidAffix);
    }
    return uSocketCount;
}








uint64 ItemGetMaxStackSize(const GBHandle hgbItem, const ActorCommonData * ptACDOwner, const ActorCommonData * ptACDItem)
{
    ScopedItemDefinitionLockR ptItemDef(hgbItem.gbid);
    if (ptItemDef == NULL)
        return 0;
    int32 nStackSize = ptItemDef->nMaxStackSize;
    if (ItemHasLabel(hgbItem.gbid, ITL_BLOODSHARD))
    {
        int32 nHighestSoloRiftLevel = 0;
        if (ptACDOwner != NULL)
        {
            nHighestSoloRiftLevel = ptACDOwner->AttributesGetInt(ATTRIBUTE_HIGHEST_SOLO_RIFT_LEVEL);
        }
        else if (ptACDItem != NULL && ptACDItem->IsItem() && !ptACDItem->IsInWorld())
        {
            InventoryLocation tLocation = ptACDItem->GetInventoryLocation();
            ActorCommonData * ptACDOwnerLoc = ACDTryToGet(tLocation.idACDContainer);
            if (ptACDOwnerLoc != NULL)
                nHighestSoloRiftLevel = ptACDOwnerLoc->AttributesGetInt(ATTRIBUTE_HIGHEST_SOLO_RIFT_LEVEL);
        }
        nStackSize += nHighestSoloRiftLevel * 10;
    }
    uint64 uFinalStackSize = Math::Max(nStackSize, 0);
    return uFinalStackSize;
}


























void ItemGeneratorToDefaults(D3::Items::Generator & tGenerator)
{
    tGenerator.Clear();
    tGenerator.set_seed(0);
    tGenerator.mutable_gb_handle()->set_game_balance_type(GB_ITEMS);
    tGenerator.mutable_gb_handle()->set_gbid(GBID_NONE);
    tGenerator.set_flags(0x1a900);
    tGenerator.set_durability(0);
    tGenerator.set_stack_size(1);
}









static void sSetItemAttribute(ActorCommonData & tACD, FastAttribKey tKey, int nValue)
{



    if (GameActiveGameCommonDataIsServer())
    {
        tACD.AttributesSetInt(tKey, nValue);
    }
    else
    {
        FastAttribSetValueInt(tACD.idFastAttrib, tKey, nValue);
    }
}


static void sSetItemAttribute(ActorCommonData & tACD, FastAttribKey tKey, float fValue)
{



    if (GameActiveGameCommonDataIsServer())
    {
        tACD.AttributesSetFloat(tKey, fValue);
    }
    else
    {
        FastAttribSetValueFloat(tACD.idFastAttrib, tKey, fValue);
    }
}


static void sSetBaseAttributeIfDifferent(ActorCommonData & tACD, FastAttribKey tKey, float flValue)
{



    float flOldValue = tACD.AttributesGetFloat(tKey);
    if (flOldValue != flValue)
    {
        sSetItemAttribute(tACD, tKey, flValue);
    }
}

// RareItemName::Import is declared in Items.h:377 and only ever inlined (abstract DIE 0x0435e751, no
// concrete DIE). Body from the inlined copy in ItemGenerateCommon (Items.cpp:7442; Items.h:380-383 rows);
// defined here instead of in the header to avoid a 439-includer header edit.
inline void RareItemName::Import(const D3::Items::RareItemName & tRareItemName)
{
    fItemNameIsPrefix = tRareItemName.item_name_is_prefix();
    snoAffixStringList = tRareItemName.sno_affix_string_list();
    nAffixStringListIndex = tRareItemName.affix_string_list_index();
    nItemStringListIndex = tRareItemName.item_string_list_index();
}
BOOL ItemIsDyeableGenerationCheck(const ActorCommonData & tACDItem);  // Items.cpp:9031
float GameBalanceGetAttribute(const GBHandle & hGB, const GameBalanceAttribute eGBAttribute);  // GameBalanceDefinition.cpp:4171
// (int32) casts in ItemGenerateCommon pick the int sSetItemAttribute overload the DWARF inlines (Items.cpp:5872) for uint32 getters

BOOL ItemTypeAllowedForAffix(const ItemType * pItemType, const GBID * arAllowedTypes, const GBID * arLegendaryAllowedTypes, BOOL bAllowLegendaryTypes)
{
    if (pItemType == NULL)
        return FALSE;
    while (pItemType != NULL)
    {
        if (arAllowedTypes != NULL)
        {
            for (int32 i = 0; i < 24; i++)
            {
                if (pItemType->gbid == arAllowedTypes[i])
                    return TRUE;
            }
        }
        if (arLegendaryAllowedTypes != NULL && bAllowLegendaryTypes)
        {
            for (int32 i = 0; i < 24; i++)
            {
                if (pItemType->gbid == arLegendaryAllowedTypes[i])
                    return TRUE;
            }
        }
        pItemType = pItemType->ptParentType;
    }
    return FALSE;
}















BOOL ItemTypeAllowedForAffix(const ItemType * pItemType, const Affix & tAffix, BOOL bAllowLegendaryTypes)
{
    return ItemTypeAllowedForAffix(pItemType, tAffix.arAllowedTypes, tAffix.arLegendaryAllowedTypes, bAllowLegendaryTypes);
}






BOOL ItemAllowedForAffix(GBID gbidItem, const Affix & tAffix, BOOL bAllowLegendaryTypes)
{
    if (gbidItem == GBID_NONE)
        return TRUE;
    const Item * pItem = ItemsGetItem(gbidItem);
    if (pItem == NULL)
        return FALSE;
    if (tAffix.ePlayerClass != PLAYER_CLASS_NONE && !ItemMeetsOwnerClassRestrictions(gbidItem, tAffix.ePlayerClass, FALSE))
        return FALSE;
    if (pItem->ptItemType != NULL && ItemTypeAllowedForAffix(pItem->ptItemType, tAffix, bAllowLegendaryTypes))
        return TRUE;
    return FALSE;
}























static BOOL sAffixesAreFromSameFamilyOrDifferentClasses(const AffixTableEntry * ptAffix1, const AffixTableEntry * ptAffix2)
{






    BOOL fResult = FALSE;
    if (ptAffix1->gbidAffixFamily0 != GBID_NONE)
    {   if (ptAffix1->gbidAffixFamily0 == ptAffix2->gbidAffixFamily0




            |
            ptAffix1->gbidAffixFamily0 == ptAffix2->gbidAffixFamily1)
            fResult = TRUE;
    }


    if (ptAffix1->gbidAffixFamily1 != GBID_NONE)
    {   if (ptAffix1->gbidAffixFamily1 == ptAffix2->gbidAffixFamily0




            |
            ptAffix1->gbidAffixFamily1 == ptAffix2->gbidAffixFamily1)
            fResult = TRUE;
    }


    if (ptAffix1->ePlayerClass != PLAYER_CLASS_NONE)
    {   if (!(ptAffix2->ePlayerClass == PLAYER_CLASS_NONE |
              ptAffix1->ePlayerClass == ptAffix2->ePlayerClass))
            fResult = TRUE;
    }


    return fResult;
}












BOOL ItemAffixAddsSockets(GBID gbidAffix)
{
    ScopedItemAffixLockR pdefAffix(gbidAffix);
    if (pdefAffix == NULL)
        return FALSE;
    for (int32 i = 0; i < 4; i++)
    {
        if (pdefAffix->ptAffixAttributes[i].eAttribute == ATTRIBUTE_SOCKETS)
            return TRUE;
    }
    return FALSE;
}







// NON_MATCHING: 2.7.7 tests the eIQL bit with a 32-bit lsl (1u << eIQL here; 1ULL gave lsl x); 4 instructions remain, a register swap of the ptAffix/eIQL loads at line 6351. Operand-swapped and >> forms tried.
BOOL ItemCanAddAffix(
    const ItemAffixSpecifier & tSpecifier,
    const AffixTableEntry * ptAffixTableEntry,
    AffixTableEntry ** ptSpecifierAffixTableEntries)
{
    if (!(tSpecifier.dwFlags & (1 << 1)) && tSpecifier.nAffixLevel != -1)
    {
        if (tSpecifier.nAffixLevel < tSpecifier.ptAffix->nAffixLevelMin ||
            tSpecifier.nAffixLevel > tSpecifier.ptAffix->nAffixLevelMax)
        {
            return FALSE;
        }
    }

    int32 nFrequencyWeight = tSpecifier.ptAffix->nFrequencyWeight;













    if (tSpecifier.eTargetedPlayerClass != PLAYER_CLASS_NONE)
    {


        nFrequencyWeight *= tSpecifier.ptAffix->nClassFrequencyWeight[tSpecifier.eTargetedPlayerClass];
    }
    else if (tSpecifier.eTargetedHirelingClass != 0)
    {


        nFrequencyWeight *= tSpecifier.ptAffix->nHirelingFrequencyWeight[tSpecifier.eTargetedHirelingClass];
    }

    if (!(tSpecifier.dwFlags & (1 << 4)) && nFrequencyWeight <= 0)
    {
        return FALSE;
    }


    if (tSpecifier.nCurrentAffixesCount == 1 &&
        (tSpecifier.eIQL == IQL_MAGIC_2_AFFIX || tSpecifier.eIQL == IQL_MAGIC_3_AFFIX) &&
        tSpecifier.ptAffix->eAffixType == tSpecifier.eLastAffixType)
    {
        return FALSE;
    }

    if (tSpecifier.nCurrentAffixesCount >= 6)
    {
        return FALSE;
    }

    if (!(tSpecifier.ptAffix->dwFlags & (1 << AFFIX_CRAFTER_AVAILABLE_BIT)) && (tSpecifier.dwFlags & (1 << 5)))
    {
        return FALSE;
    }

    if (!(tSpecifier.ptAffix->dwFlags & (1 << AFFIX_VENDOR_AVAILABLE_BIT)) && (tSpecifier.dwFlags & (1 << 6)))
    {
        return FALSE;
    }


    if ((tSpecifier.dwFlags & (1 << 7)) &&
        ptAffixTableEntry->eAffixGroup != AFFIXGROUP_MAJOR)
    {
        return FALSE;
    }
    if ((tSpecifier.dwFlags & (1 << 8)) &&
        ptAffixTableEntry->eAffixGroup != AFFIXGROUP_MINOR)
    {
        return FALSE;
    }


    for (int32 i = 0; i < tSpecifier.nCurrentAffixesCount; i++)
    {
        if (tSpecifier.arCurrentAffixes[i] != GBID_NONE &&
            sAffixesAreFromSameFamilyOrDifferentClasses(ptAffixTableEntry, ptSpecifierAffixTableEntries[i]))
        {
            return FALSE;
        }
    }

    if (tSpecifier.ptAffix->eAffixType == AT_ENCHANTMENT || tSpecifier.ptAffix->eAffixType == AT_ADD_SOCKET)
    {
        return FALSE;
    }


    BOOL bAllowLegendaryTypes = (tSpecifier.dwFlags & (1 << 3)) ||
                                (tSpecifier.gbidAllowedLegendaryAffixFamily != GBID_NONE &&
                                 (tSpecifier.ptAffix->gbidAffixFamily0 == tSpecifier.gbidAllowedLegendaryAffixFamily ||
                                  tSpecifier.ptAffix->gbidAffixFamily1 == tSpecifier.gbidAllowedLegendaryAffixFamily));
    if (!ItemAllowedForAffix(tSpecifier.gbidItem, *tSpecifier.ptAffix, bAllowLegendaryTypes))
    {
        return FALSE;
    }

    if (!(tSpecifier.dwFlags & (1 << 0)) &&
        !(tSpecifier.ptAffix->dwAllowedQualityLevels & (1u << tSpecifier.eIQL)))
    {
        return FALSE;
    }

    if ((tSpecifier.dwFlags & (1 << 2)) &&
        !(tSpecifier.ptAffix->dwAllowedQualityLevels & (1 << 9)))
    {
        return FALSE;
    }

    if ((tSpecifier.dwFlags & (1 << 9)) &&
        ItemAffixAddsSockets(tSpecifier.ptAffix->gbid))
    {
        return FALSE;
    }

    return TRUE;
}


























































void ItemsAddItemAffixGBID(ActorCommonData & tACD,
                           GBID gbidAffix,
                           uint32 dwFlags,
                           ItemQualityLevel eIQL,
                           BOOL bIsReplacementAffix,
                           ItemAffixFastAttribMap * arAffixSpecificAttributes)

{





    if (GameActiveGameCommonDataIsServer() && eIQL == IQL_LEGENDARY)
    {


        ScopedItemAffixLockR ptAffix(gbidAffix);
        if (ptAffix != NULL && ptAffix->gbidConvertsTo != GBID_NONE)
        {
            const Item * ptItem = ItemsGetItem(tACD.hGB.gbid);
            if (ptItem != NULL)
            {

                BOOL bAllowed = ptItem->ptItemType != NULL &&

                                ItemTypeAllowedForAffix(ptItem->ptItemType, ptAffix->arAllowedTypes, ptAffix->arLegendaryAllowedTypes, TRUE);





                if (!bAllowed)
                {
                    gbidAffix = ptAffix->gbidConvertsTo;
                }
            }
        }
    }



    if (dwFlags & (1 << IGF_IDENTIFIED_BIT))
    {
        GBID gbidRerolledAffix = ItemGetEnchantedAffixOld(tACD);
        uint32 dwEnchantedAffixSeed = ItemGetEnchantedAffixSeed(tACD);



        BOOL bSimulateOnly = !bIsReplacementAffix && gbidAffix == gbidRerolledAffix && dwEnchantedAffixSeed != 0;
        sAddAttributesForAffix(tACD, gbidAffix, bSimulateOnly, arAffixSpecificAttributes);

        if (!bIsReplacementAffix)
        {
            tACD.listIdentifiedAffixGBIDs.AddTail_Tracked(gbidAffix);
        }
    }
    else
    {

        tACD.listUnidentifiedAffixGBIDs.AddTail_Tracked(gbidAffix);
    }



    if (GameActiveGameCommonDataIsServer())
    {
        tACD.ModifiedMark(3);
    }
}




BOOL ItemsValidateGeneratorParams(const D3::Items::Generator & tGenerator)
{
    if (tGenerator.gb_handle().game_balance_type() == GB_INVALID)
        return FALSE;
    GBHandle hGBItem;
    hGBItem.Import(tGenerator.gb_handle());
    if (!GBIDExists(hGBItem.gbid, hGBItem.eType))
        return FALSE;
    for (int32 i = 0; i < tGenerator.base_affixes_size(); i++)
    {
        if (tGenerator.base_affixes(i) != GBID_NONE && !GBIDExists(tGenerator.base_affixes(i), GB_AFFIXES))
            return FALSE;
    }
    return TRUE;
}




















static void sAggregateFloatAttribute(ActorCommonData & tACDItem, FastAttribKey tKey, float flValue)
{



    FastAttribValue tRunningTotal = FastAttribGetValue(tACDItem.idFastAttrib, tKey);

    AttributeOperator eOperator = KeyGetAggregationOperator(tKey);
    FastAttribAggregate(tRunningTotal, flValue, ATTRIBUTEDT_FLOAT, eOperator, tKey);
    FastAttribSetValue(tACDItem.idFastAttrib, tKey, tRunningTotal);
}



void ItemAddBaseAttributesForItem(ActorCommonData & tACDItem,
                                  GBHandle hGBItem,
                                  BOOL bIsCrafted,
                                  BOOL bSimulateOnly,
                                  BOOL bAddJewelUpgrades)

{


    const GameBalanceDefinition * pdefGB;
    ItemDefinition * ptItemDef = (ItemDefinition *)GBLockR(hGBItem, pdefGB, TRUE);
    if (ptItemDef != NULL)
    {
        DyeType eOldDyeType = tACDItem.AttributesGetInt(ATTRIBUTE_DYE_TYPE);



        tACDItem.AttributesSetInt(ATTRIBUTE_DYE_TYPE, 0);
        sAddAttributesFromSpecifiers(tACDItem, ptItemDef->ptAttributes, ptItemDef->serAttributes_Console.dwSize / sizeof(AttributeSpecifier), bSimulateOnly, TRUE);
        SavedDyeType eNewSavedDyeType = tACDItem.AttributesGetInt(ATTRIBUTE_DYE_TYPE);
        if (eNewSavedDyeType != 0)
        {





            SavedDyeType eSavedDyeType = tACDItem.AttributesGetInt(ATTRIBUTE_DYE_TYPE);
            if (eSavedDyeType != 0)
            {
                tACDItem.AttributesSetInt(ATTRIBUTE_DYE_TYPE, DyeTypeTranslateFromSavedDyeType(eSavedDyeType));
            }
        }
        else


        tACDItem.AttributesSetInt(ATTRIBUTE_DYE_TYPE, eOldDyeType);


        if (!bIsCrafted)
        {

            float flMin = ptItemDef->flAttributesDamageVarianceMin; float flDelta = ptItemDef->flAttributesDamageVarianceDelta;

            if (Math::Abs(flMin) > 0.000001f ||
                Math::Abs(flDelta) > 0.000001f)
            {
                flMin = (float)Math::FloatRoundToInt(RandRangeFloat_Internal(*tACDItem.ptRNG, 0.0f, flMin));
                flDelta = (float)Math::FloatRoundToInt(RandRangeFloat_Internal(*tACDItem.ptRNG, 0.0f, flDelta));
                if (!bSimulateOnly)
                {
                    sAggregateFloatAttribute(tACDItem, MakeAttribKey(ATTRIBUTE_DAMAGE_WEAPON_MIN, 0), flMin);
                    sAggregateFloatAttribute(tACDItem, MakeAttribKey(ATTRIBUTE_DAMAGE_WEAPON_DELTA, 0), flDelta);
                }
            }
        }


        if (bAddJewelUpgrades &&
            ptItemDef->tJewelUpgradeAttribute.eAttribute != ATTRIBUTE_NONE &&
            ItemIsUpgradeableJewel(tACDItem))
        {
            ActorCommonData * ptACDOwner = ACDTryToGet(tACDItem.tLocation.tInvLocation.idACDContainer);
            int32 nJewelRank = tACDItem.AttributesGetInt(ATTRIBUTE_JEWEL_RANK);



            float flAttributeValuePerRank = ItemGetAttributeSpecifierValue(hGBItem.gbid, ptItemDef->tJewelUpgradeAttribute, ptACDOwner, tACDItem.ptRNG, tACDItem.AttributesGetInt(ATTRIBUTE_ITEM_LEGENDARY_ITEM_LEVEL_OVERRIDE), FALSE);
            if (!bSimulateOnly && nJewelRank > 0)
            {
                FastAttribKey tKey = MakeAttribKey(ptItemDef->tJewelUpgradeAttribute.eAttribute, ptItemDef->tJewelUpgradeAttribute.nParam);
                AttributeDataType eAttribType = KeyGetDataType(tKey);


                float flAttributeValue = flAttributeValuePerRank * nJewelRank;
                FastAttribValue tValue(flAttributeValue); if (eAttribType != ATTRIBUTEDT_FLOAT) tValue.nValue = (int32)flAttributeValue;
                FastAttribValue tCurrentValue = FastAttribGetValue(tACDItem.idFastAttrib, tKey);

                if (eAttribType == ATTRIBUTEDT_FLOAT)
                {
                    tValue.flValue += tCurrentValue.flValue;
                }
                else
                {
                    tValue.nValue += tCurrentValue.nValue;
                }

                FastAttribSetValue(tACDItem.idFastAttrib, tKey, tValue);
            }



            if (ptItemDef->nJewelSecondaryEffectUnlockRank > 0)
            {
                float flSecondaryEffectAttributeValue = ItemGetAttributeSpecifierValue(hGBItem.gbid, ptItemDef->tJewelSecondaryEffectAttribute, ptACDOwner, tACDItem.ptRNG, tACDItem.AttributesGetInt(ATTRIBUTE_ITEM_LEGENDARY_ITEM_LEVEL_OVERRIDE), FALSE);
                if (!bSimulateOnly && nJewelRank >= ptItemDef->nJewelSecondaryEffectUnlockRank)
                {
                    FastAttribKey tKey = MakeAttribKey(ptItemDef->tJewelSecondaryEffectAttribute.eAttribute, ptItemDef->tJewelSecondaryEffectAttribute.nParam);
                    AttributeDataType eAttribType = KeyGetDataType(tKey);


                    FastAttribValue tValue(flSecondaryEffectAttributeValue); if (eAttribType != ATTRIBUTEDT_FLOAT) tValue.nValue = (int32)flSecondaryEffectAttributeValue;
                    FastAttribValue tCurrentValue = FastAttribGetValue(tACDItem.idFastAttrib, tKey);

                    if (eAttribType == ATTRIBUTEDT_FLOAT)
                    {
                        tValue.flValue += tCurrentValue.flValue;
                    }
                    else
                    {
                        tValue.nValue += tCurrentValue.nValue;
                    }

                    FastAttribSetValue(tACDItem.idFastAttrib, tKey, tValue);
                }
            }
        }

        GBUnlockR(pdefGB);
    }
}



static inline int32 sGetAffixLevelRequirement(const ActorCommonData & tACDItem, GBID gbidAffix)
{
    ScopedItemAffixLockR ptAffix(gbidAffix);
    int32 nRequiredLevel = 0;
    if (ptAffix != NULL)
    {
        if (tACDItem.AttributesGetInt(ATTRIBUTE_IS_CRAFTED))
            nRequiredLevel = ptAffix->nCrafterRequiredLevel;
        else
            nRequiredLevel = ptAffix->nNormalRequiredLevel;
    }
    return nRequiredLevel;
}












static void sAddAttributesForAffix(ActorCommonData & tACDItem,
                                   GBID gbidAffix,
                                   BOOL bSimulateOnly,
                                   ItemAffixFastAttribMap * arAffixSpecificAttributes)
{

    ScopedItemAffixLockR ptAffix(gbidAffix);
    if (ptAffix == NULL)
    {
        return;
    }



    AttribGroupID idAffixAttribGroup = -1;
    if (arAffixSpecificAttributes != NULL)
    {

        if (arAffixSpecificAttributes->Lookup(gbidAffix, idAffixAttribGroup))
        {
            FastAttribGroupDelete(idAffixAttribGroup);
            arAffixSpecificAttributes->RemoveAt(gbidAffix);
        }

        idAffixAttribGroup = FastAttribGroupCreate(2, -1);
        if (idAffixAttribGroup != -1)
        {
            arAffixSpecificAttributes->SetAt_Tracked(gbidAffix, idAffixAttribGroup);
        }
    }

    RNG tCopyRNG = *tACDItem.ptRNG;
    sAddAttributesFromSpecifiers(tACDItem, ptAffix->ptAffixAttributes, 4, bSimulateOnly, TRUE);


    if (idAffixAttribGroup != -1)
    {
        ActorCommonData * ptACDOwner = ACDTryToGet(tACDItem.tLocation.tInvLocation.idACDContainer);
        sFastAttribAggregrateAttributesFromSpecifiers(tACDItem, idAffixAttribGroup, ptAffix->ptAffixAttributes, 4, tACDItem.hGB.gbid, ptACDOwner, &tCopyRNG, FALSE, FALSE);
    }

    if (!ItemIsLegendary(tACDItem.hGB.gbid) && !tACDItem.AttributesGetInt(ATTRIBUTE_IS_CRAFTED))
    {
        int32 nRequiredLevel = sGetAffixLevelRequirement(tACDItem, gbidAffix);
        if (nRequiredLevel > 0 &&

            (float)nRequiredLevel > FastAttribGetValueFloat(tACDItem.idFastAttrib, MakeAttribKey(ATTRIBUTE_REQUIREMENT, ATTRIBUTE_LEVEL)))
        {
            FastAttribSetValueFloat(tACDItem.idFastAttrib, MakeAttribKey(ATTRIBUTE_REQUIREMENT, ATTRIBUTE_LEVEL), (float)nRequiredLevel);

            if (GameActiveGameCommonDataIsServer())
            {
                tACDItem.ModifiedMark(0);
            }
        }
    }

}


static const DyeType sarrRandomAllowedDyes[] = { DYE_TYPE_01, DYE_TYPE_02, DYE_TYPE_03, DYE_TYPE_10, DYE_TYPE_12, DYE_TYPE_15, DYE_TYPE_17, DYE_TYPE_18, DYE_TYPE_19 };

























GBID ItemsChooseLevelAppropriateItemFromBaseItem(GBID gbidBaseItem, int32 nLevel)
{
    GBID gbidItemType = ItemGetItemTypeGBID(gbidBaseItem);
    if (ItemIsGem(gbidBaseItem))
        return gbidBaseItem;

    const ItemAppGlobals * ptItemAppGlobals = sgtAppGlobals.ptItemAppGlobals;
    int32 nMemberItems = ptItemAppGlobals->nItems;
    uint32 dwDisallowedItemFlags = (1 << ITEM_LEGENDARY_BIT) | (1 << ITEM_SPECIAL_BIT) | (1 << ITEM_TEST_ONLY_BIT);
    int32 nHighestFoundLevel = -1;
    GBID gbidHighestFound = GBID_NONE;
    for (int32 nItemIdx = 0; nItemIdx < nMemberItems; nItemIdx++)
    {
        const Item & tItem = ptItemAppGlobals->ptItems[nItemIdx];
        ScopedItemDefinitionLockR pdefItem(tItem.gbid);
        if ((tItem.dwFlags & dwDisallowedItemFlags) == 0 && pdefItem != NULL && pdefItem->snoRandomItemTreasureClass == SNO_NONE && ItemGetItemTypeGBID(tItem.gbid) == gbidItemType)
        {
            int32 nRequiredLevel = pdefItem->nNormalRequiredLevel;
            if (nRequiredLevel > 0 && nRequiredLevel <= nLevel && tItem.nItemLevel > nHighestFoundLevel)
            {
                gbidHighestFound = tItem.gbid;
                nHighestFoundLevel = tItem.nItemLevel;
            }
        }
    }
    return gbidHighestFound;
}























GBID sGetUprankedAffix(GBID gbidCurr, int32 nUpscaledLevel)
{
    ScopedItemAffixLockR ptAffix(gbidCurr);
    int32 nAffixLevelMax = ptAffix->nAffixLevelMax;
    GBID gbidUpranked = ptAffix->gbidLegendaryUprankAffix;
    while (nAffixLevelMax < nUpscaledLevel && gbidUpranked != GBID_NONE)
    {
        gbidCurr = gbidUpranked;
        ScopedItemAffixLockR ptAffix2(gbidCurr);
        nAffixLevelMax = ptAffix2->nAffixLevelMax;
        gbidUpranked = ptAffix2->gbidLegendaryUprankAffix;
    }
    return gbidCurr;
}









AttribGroupID ItemGenerateUpscaledAttribForPlayer(
    ActorCommonData & tACDOwner,
    ActorCommonData & tACDItem)
{


    AppGlobals & tAppGlobals = sgtAppGlobals;

    tACDItem.idUpscaledAttribGroup = FastAttribGroupCreate(FAT_SIMPLE, -1);

    int32 nUpscaledLevel = tACDOwner.AttributesGetInt(ATTRIBUTE_UPSCALED_LEVEL);







    D3::Items::Generator tGenerator;
    ItemGeneratorPopulateFromACDID(tACDItem, tGenerator, TRUE);


    GBID gbidBase = tACDItem.GetGBHandle().gbid;

    GBID gbidNew = GBID_NONE;

    if (tGenerator.has_legendary_base_item_gbid())
    {
        gbidNew = tGenerator.legendary_base_item_gbid();
    }

    else
    {
        gbidNew = ItemsChooseLevelAppropriateItemFromBaseItem(gbidBase, nUpscaledLevel);
    }

    GBHandle hGBNew(GB_ITEMS, gbidNew);
    hGBNew.Export(*tGenerator.mutable_gb_handle());






    if (ItemIsLegendary(gbidBase))
    {
        tGenerator.set_legendary_item_level(nUpscaledLevel);
    }



    int32 nAffixes = tGenerator.base_affixes_size();

    if (nAffixes > 0)
    {
        XPooledDataIDList listGBIDs(MemoryPoolsGetDATAIDListPool());

        for (int32 n = 0; n < nAffixes; ++n)
        {
            listGBIDs.AddTail_Tracked(tGenerator.base_affixes(n));
        }

        tGenerator.clear_base_affixes();

        int32 nAffix = 1;









        POS pos = listGBIDs.GetHeadPosition();
        while (pos)
        {
            GBID gbidCurr = listGBIDs.GetNext(pos);

            GBID gbidUpranked = sGetUprankedAffix(gbidCurr, nUpscaledLevel);

            tGenerator.add_base_affixes(gbidUpranked);
        }

    }






    ACDID idACDNew = ACDID_INVALID;
    if (GameActiveGameCommonDataIsClient())
    {
        idACDNew = CItemGenerate((ACDNetworkName)12070, tGenerator, NULL);
    }
    else
    {
        idACDNew = SItemGenerate(OnlineService::ItemId::INVALID, tGenerator, cgplaceNull, 0.0f, 0);
    }

    if (idACDNew == ACDID_INVALID)
    {





        FastAttribIterator tIt(tACDItem.idFastAttrib, TRUE);
        while (FastAttribIterate(tIt))
        {
            FastAttribKey tKey = tIt.tEntryBuffer.tKey;

            FastAttribValue tValue = FastAttribGetValue(tACDItem.idFastAttrib, tKey);
            FastAttribSetValue(tACDItem.idUpscaledAttribGroup, tKey, tValue);
        }

        return tACDItem.idUpscaledAttribGroup;
    }

    ActorCommonData & newItemACD = ACDGet(idACDNew);

    newItemACD.AttributesSetInt(ATTRIBUTE_HELD_IN_OFFHAND, tACDItem.AttributesGetInt(ATTRIBUTE_HELD_IN_OFFHAND));

    {
        ScopedItemDefinitionLockR ptItemDef(tACDItem.hGB.gbid);

        FastAttribKey tSetKey = MakeAttribKey(ATTRIBUTE_SET_ITEM_COUNT, ptItemDef->gbidSet);

        if (tACDItem.AttributesGetInt(tSetKey) > 0)
        {
            newItemACD.AttributesSetInt(tSetKey, tACDItem.AttributesGetInt(tSetKey));
            FastAttribKey tSetDiscountKey = MakeAttribKey(ATTRIBUTE_SET_ITEM_DISCOUNT, ptItemDef->gbidSet);
            newItemACD.AttributesSetInt(tSetDiscountKey, tACDItem.AttributesGetInt(tSetDiscountKey));
        }

        for (int32 i = 0; i < 4; ++i)
        {
            const SkillDeclaration & tSkill = ptItemDef->tSkills[i];
            if (tSkill.snoPower == SNO_NONE || tSkill.snoPower == 0)
            {
                break;
            }


            sSetItemAttribute(newItemACD, MakeAttribKey(ATTRIBUTE_SKILL, tSkill.snoPower), 1);
        }

        for (int32 i = 0; i < (int32)(ptItemDef->serAttributes_Console.dwSize / sizeof(AttributeSpecifier)); ++i)
        {
            AttributeSpecifier tSpecifier = ptItemDef->ptAttributes[i];
            if (tSpecifier.eAttribute == ATTRIBUTE_ITEM_POWER_PASSIVE)
            {
                SNO snoPower = tSpecifier.nParam;

                FastAttribKey tPassiveKey = MakeAttribKey(ATTRIBUTE_ITEM_POWER_PASSIVE, snoPower);

                FastAttribValue tValue = FastAttribGetValue(tACDItem.idFastAttrib, tPassiveKey);

                FastAttribSetValue(newItemACD.idFastAttrib, tPassiveKey, tValue);
            }
        }
    }

    FastAttribIterator tOldIt(tACDItem.idFastAttrib, TRUE);
    while (FastAttribIterate(tOldIt))
    {
        if (KeyGetAttrib(tOldIt.tEntryBuffer.tKey) == ATTRIBUTE_ITEM_POWER_PASSIVE)
        {
            FastAttribValue tValue = tOldIt.tEntryBuffer.tValue;
            FastAttribSetValue(newItemACD.idFastAttrib, tOldIt.tEntryBuffer.tKey, tValue);
        }
    }


    FastAttribIterator tIt(newItemACD.idFastAttrib, TRUE);
    while (FastAttribIterate(tIt))
    {
        FastAttribKey tKey = tIt.tEntryBuffer.tKey;

        FastAttribValue tValue = FastAttribGetValue(newItemACD.idFastAttrib, tKey);
        FastAttribSetValue(tACDItem.idUpscaledAttribGroup, tKey, tValue);

        if (tAppGlobals.bShowSidekickDebug)
        {
            FastAttribValue tOldValue = FastAttribGetValue(tACDItem.idFastAttrib, tKey);
            // The type-query call survives; erased diagnostic text/control flow is unknown.
            KeyGetDataType(tKey);















        }
    }



    if (GameActiveGameCommonDataIsClient())
    {
        ACDDelete(idACDNew, FALSE);
    }
    else
    {
        ActorDelete((ActorID)ACDGetOwner(idACDNew));
    }

    return tACDItem.idUpscaledAttribGroup;
}




void ClearUpscaledAttributes(ActorCommonData & tACDItem)
{
    if (tACDItem.idUpscaledAttribGroup != -1)
    {
        FastAttribGroupDelete(tACDItem.idUpscaledAttribGroup);
        tACDItem.idUpscaledAttribGroup = -1;
    }
}





BOOL ItemGenerateCommon(
    ActorCommonData & tACD,
    const D3::Items::Generator & tGenerator,
    ItemAffixFastAttribMap * arAffixSpecificAttributes)
{

    if (!GameActiveGameCommonDataIsClient())
    {


        Actor * ptActor = ActorGet((ActorID)tACD.idOwner);
        ptActor->dwItemInitSeed = tGenerator.seed();
    }



    RNG tItemRNG;
    RandInit(tItemRNG);
    RandSetSeed_Internal(tItemRNG, tGenerator.seed());

    GBHandle hGBItem;
    hGBItem.Import(tGenerator.gb_handle());
    const GameBalanceDefinition * pdefGB;
    const ItemDefinition * ptItem = (const ItemDefinition *)GBLockR(hGBItem, pdefGB, TRUE);
    if (ptItem == NULL)
    {
        return FALSE;
    }


    GBHandle hGBItemBase;
    if (ptItem->gbidBaseItem != GBID_NONE)
    {










        GBID gbidBaseItem = tGenerator.has_legendary_item_level() ? (tGenerator.has_legendary_base_item_gbid() ? tGenerator.legendary_base_item_gbid() : ItemsChooseLevelAppropriateItemFromBaseItem(ptItem->gbidBaseItem, tGenerator.legendary_item_level())) : ptItem->gbidBaseItem;
        hGBItemBase.Set(GB_ITEMS, gbidBaseItem);
        // the GBIDExists call has rows inside the inlined Set at line 6986 (machine code +0xfc)
        if (!GBIDExists(hGBItemBase))
        {
            GBUnlockR(pdefGB);
            return FALSE;
        }
    }


















    else
    {
        hGBItemBase = hGBItem;
    }

    for (int32 i = 0; i < 4; ++i)
    {
        const SkillDeclaration & tSkill = ptItem->tSkills[i];

        if (tSkill.snoPower == SNO_NONE || tSkill.snoPower == 0)
        {
            break;
        }
        sSetItemAttribute(tACD, MakeAttribKey(ATTRIBUTE_SKILL, tSkill.snoPower), 1);
    }


    if (tGenerator.flags() & (1 << IGF_RESTORED_BIT))
    {
        sSetItemAttribute(tACD, ATTRIBUTE_DURABILITY_MAX, (int32)tGenerator.max_durability());
        sSetItemAttribute(tACD, ATTRIBUTE_DURABILITY_CUR, (int32)tGenerator.durability());

        if (ptItem->nBaseDurability != 0 && ptItem->nDurabilityVariance > 0)
        {
            Rand_Internal(tItemRNG, ptItem->nDurabilityVariance - 1);
        }

        DyeType eDyeType = DyeTypeTranslateFromSavedDyeType(tGenerator.dye_type());
        sSetItemAttribute(tACD, ATTRIBUTE_DYE_TYPE, eDyeType);

        if (ptItem->eSavedDyeType == SAVED_DYE_TYPE_NONE &&
            tGenerator.item_quality_level() >= IQL_RARE_AFFIX_START &&
            ItemIsDyeableGenerationCheck(tACD))
        {
            RandRange_Internal(tItemRNG, 0, sizeof(sarrRandomAllowedDyes) / sizeof(sarrRandomAllowedDyes[0]) - 1);
        }
    }
    else
    {
        int32 nDurability = ptItem->nBaseDurability;
        if (nDurability != 0 && ptItem->nDurabilityVariance > 0)
        {
            nDurability += Rand_Internal(tItemRNG, ptItem->nDurabilityVariance - 1);
        }

        sSetItemAttribute(tACD, ATTRIBUTE_DURABILITY_MAX, nDurability);
        sSetItemAttribute(tACD, ATTRIBUTE_DURABILITY_CUR, nDurability);

        DyeType eDyeType = DyeTypeTranslateFromSavedDyeType(ptItem->eSavedDyeType);
        if (eDyeType == DYE_TYPE_NONE &&
            tGenerator.item_quality_level() >= IQL_RARE_AFFIX_START &&
            ItemIsDyeableGenerationCheck(tACD))
        {

            eDyeType = sarrRandomAllowedDyes[RandRange_Internal(tItemRNG, 0, sizeof(sarrRandomAllowedDyes) / sizeof(sarrRandomAllowedDyes[0]) - 1)];
        }

        sSetItemAttribute(tACD, ATTRIBUTE_DYE_TYPE, eDyeType);
    }

    if (ptItem->gbidSet != GBID_NONE)
    {
        sSetItemAttribute(tACD, MakeAttribKey(ATTRIBUTE_SET_ITEM_COUNT, ptItem->gbidSet), 1);
    }

    if (tGenerator.has_legendary_item_level())
    {
        tACD.AttributesSetInt(ATTRIBUTE_ITEM_LEGENDARY_ITEM_LEVEL_OVERRIDE, tGenerator.legendary_item_level());
    }

    if (tGenerator.has_legendary_base_item_gbid())
    {
        tACD.AttributesSetInt(ATTRIBUTE_ITEM_LEGENDARY_ITEM_BASE_ITEM, tGenerator.legendary_base_item_gbid());
    }


    if (tGenerator.has_console_max_level())
    {
        tACD.AttributesSetInt(ATTRIBUTE_ITEM_CONSOLE_MAX_LEVEL, tGenerator.console_max_level());
    }

    if (tGenerator.has_console_promo_item())
    {
        tACD.AttributesSetInt(ATTRIBUTE_CONSOLE_PROMO_ITEM, tGenerator.console_promo_item());
    }


    BOOL bIsCrafted = (tGenerator.flags() >> IGF_CRAFTED_BIT) & 1;
    if (bIsCrafted)
    {
        sSetItemAttribute(tACD, ATTRIBUTE_IS_CRAFTED, 1);
    }






    BOOL bVendorBought = (tGenerator.flags() >> IGF_VENDOR_BOUGHT_BIT) & 1;
    if (bVendorBought)
    {
        sSetItemAttribute(tACD, ATTRIBUTE_IS_VENDOR_BOUGHT, 1);
    }

    BOOL bLoot2_0 = (tGenerator.flags() >> IGF_LOOT_2_0_BIT) & 1;
    if (bLoot2_0)
    {
        sSetItemAttribute(tACD, ATTRIBUTE_LOOT_2_0_DROP, 1);
    }

    BOOL bPost2_1_2 = (tGenerator.flags() >> IGF_POST_2_1_2_BIT) & 1;
    if (bPost2_1_2)
    {
        sSetItemAttribute(tACD, ATTRIBUTE_POST_2_1_2_DROP, 1);
    }

    BOOL bPost2_3_0 = (tGenerator.flags() >> IGF_POST_2_3_0_BIT) & 1;
    if (bPost2_3_0)
    {
        sSetItemAttribute(tACD, ATTRIBUTE_POST_2_3_0_DROP, 1);
    }

    BOOL bPost2_5_0 = (tGenerator.flags() >> IGF_POST_2_5_0_BIT) & 1;
    if (bPost2_5_0)
    {
        sSetItemAttribute(tACD, ATTRIBUTE_POST_2_5_0_DROP, 1);
    }

    BOOL bAncient = (tGenerator.flags() >> IGF_ANCIENT_BIT) & 1;
    BOOL bPrimalAncient = (tGenerator.flags() >> IGF_PRIMAL_ANCIENT_BIT) & 1;
    if (bAncient)
    {
        sSetItemAttribute(tACD, ATTRIBUTE_ANCIENT_RANK, 1);
    }
    else if (bPrimalAncient)
    {
        sSetItemAttribute(tACD, ATTRIBUTE_ANCIENT_RANK, 2);
    }

    BOOL bUnidentified = !(tGenerator.flags() & (1 << IGF_IDENTIFIED_BIT));
    if (bUnidentified)
    {
        sSetItemAttribute(tACD, ATTRIBUTE_UNIDENTIFIED, 1);
    }

    BOOL bConsumableAddSockets = (tGenerator.flags() >> IGF_ADD_SOCKETS_BIT) & 1;
    if (bConsumableAddSockets)
    {
        sSetItemAttribute(tACD, ATTRIBUTE_CONSUMABLE_ADD_SOCKETS, 1);
        sSetItemAttribute(tACD, ATTRIBUTE_SOCKETS, ItemGetMaxNumberOfSockets(tACD));
    }

    BOOL bRemoveLevelReq = (tGenerator.flags() >> IGF_REMOVE_LEVEL_REQ) & 1;
    if (bRemoveLevelReq)
    {
        sSetItemAttribute(tACD, ATTRIBUTE_REMOVE_LEVEL_REQUIREMENT, 1);
    }

    int32 nRequiredLevel = ItemGetLevelRequirement(hGBItem.gbid, &tACD, FALSE);
    if (nRequiredLevel > 0)
    {
        sSetItemAttribute(tACD, MakeAttribKey(ATTRIBUTE_REQUIREMENT, ATTRIBUTE_LEVEL), (float)nRequiredLevel);
    }

    BOOL bTouchedByArmory = (tGenerator.flags() >> IGF_TOUCHED_BY_ARMORY_BIT) & 1;
    if (bTouchedByArmory)
    {
        sSetItemAttribute(tACD, ATTRIBUTE_ITEM_WAS_TOUCHED_BY_ARMORY, 1);
    }

    GBUnlockR(pdefGB);

    BOOL bIsGold = ItemHasLabel(hGBItem.gbid, ITL_GOLD);
    BOOL bIsPlatinum = ItemHasLabel(hGBItem.gbid, ITL_PLATINUM);

    if (ItemGetMaxStackSize(hGBItem, NULL, NULL) > 0 || bIsGold > 0 || bIsPlatinum)
    {
        tACD.SetStackCount(tGenerator.stack_size());
    }

    if (tGenerator.has_item_unlock_timestamp())
    {
        tACD.SetUnlockTime(tGenerator.item_unlock_timestamp());
    }


    if (!bIsGold)
    {
        float flDamage = GameBalanceGetAttribute(hGBItemBase, GBA_DAMAGE);
        float flDamageRange = GameBalanceGetAttribute(hGBItemBase, GBA_DAMAGERANGE);
        float flArmor = GameBalanceGetAttribute(hGBItemBase, GBA_ARMOR);
        if (!bIsCrafted || bLoot2_0)
        {
            float flArmorDelta = GameBalanceGetAttribute(hGBItemBase, GBA_ARMOR_DELTA);
            if (!bPrimalAncient)
            {




                flArmorDelta = RandRangeFloat_Internal(tItemRNG, 0.0f, flArmorDelta);
            }
            flArmor += floor(flArmorDelta + 0.5f);
        }

        sSetBaseAttributeIfDifferent(tACD, MakeAttribKey(ATTRIBUTE_DAMAGE_WEAPON_MIN, 0), flDamage);
        sSetBaseAttributeIfDifferent(tACD, MakeAttribKey(ATTRIBUTE_DAMAGE_WEAPON_DELTA, 0), flDamageRange);
        sSetBaseAttributeIfDifferent(tACD, ATTRIBUTE_ARMOR_ITEM, flArmor);

        if (ItemHasLabel(hGBItem.gbid, ITL_WEAPON))
        {
            float flAttacksPerSecond = GameBalanceGetAttribute(hGBItemBase, GBA_ATTACKS_PER_SECOND);
            sSetItemAttribute(tACD, ATTRIBUTE_ATTACKS_PER_SECOND_ITEM, flAttacksPerSecond);
        }
    }

    if (ItemHasLabel(hGBItem.gbid, ITL_BOW))
    {
        sSetItemAttribute(tACD, ATTRIBUTE_IS_BOW_NORMAL, 1.0f);
    }
    if (ItemHasLabel(hGBItem.gbid, ITL_CROSSBOW) ||
        ItemHasLabel(hGBItem.gbid, ITL_HAND_CROSSBOW))
    {
        sSetItemAttribute(tACD, ATTRIBUTE_IS_BOW_CROSSBOW, 1.0f);
    }


    uint32 uCurrentSeed = RandGetSeed(tItemRNG);

    int32 nSeasonCreated = tGenerator.season_created();
    if (nSeasonCreated == 0 && GameIsSeasonal())
    {
        nSeasonCreated = GameGetCurrentSeason();
    }

    if (GameActiveGameCommonDataIsServer())
    {


        Actor * ptActor = ActorGet((ActorID)tACD.idOwner);
        ptActor->dwItemIdentifySeed = uCurrentSeed;
        ptActor->dwSeasonCreated = nSeasonCreated;
    }


    tACD.AttributesSetInt(ATTRIBUTE_SEASON, nSeasonCreated);

    if (bIsGold)
    {

        sSetItemAttribute(tACD, ATTRIBUTE_ITEM_QUALITY_LEVEL, IQL_NORMAL);

        return TRUE;
    }



    RandInit(tItemRNG);
    RandSetSeed_Internal(tItemRNG, uCurrentSeed);
    tACD.ptRNG = &tItemRNG;


    sSetItemAttribute(tACD, ATTRIBUTE_SEED, (int32)tGenerator.seed());


    if (tGenerator.enchanted_affix_old() != GBID_NONE &&
        tGenerator.enchanted_affix_new() != GBID_NONE)
    {
        sSetItemAttribute(tACD, ATTRIBUTE_ENCHANTED_AFFIX_OLD, tGenerator.enchanted_affix_old());
        sSetItemAttribute(tACD, ATTRIBUTE_ENCHANTED_AFFIX_NEW, tGenerator.enchanted_affix_new());
        sSetItemAttribute(tACD, ATTRIBUTE_ENCHANTED_AFFIX_SEED, (int32)tGenerator.enchanted_affix_seed());
        sSetItemAttribute(tACD, ATTRIBUTE_ENCHANTED_AFFIX_COUNT, (int32)tGenerator.enchanted_affix_count());
    }


    if (ItemIsUpgradeableJewel(tACD))
    {

        int32 nJewelRank = tGenerator.jewel_rank();
        int32 nJewelMaxRank = ptItem->nJewelMaxRank;

        const int32 MAX_JEWEL_RANK = 150;

        if (nJewelMaxRank == -1)
        {
            nJewelMaxRank = MAX_JEWEL_RANK;
        }

        sSetItemAttribute(tACD, ATTRIBUTE_POST_260A_LEGENDARY_GEM_CAP, 1);

        if (nJewelMaxRank >= 0)
        {





            nJewelRank = Math::Min(nJewelRank, nJewelMaxRank);
        }

        sSetItemAttribute(tACD, ATTRIBUTE_JEWEL_RANK, nJewelRank);
    }


    ItemAddBaseAttributesForItem(tACD, hGBItem, bIsCrafted, FALSE, TRUE);


    ItemQualityLevel eIQL = tGenerator.item_quality_level();
    uint32 dwAddAffixFlag = tGenerator.flags() & (1 << IGF_IDENTIFIED_BIT);


    int32 nAffixesCount = Math::Min(tGenerator.base_affixes_size(), 6);










    if (ItemIsGem(hGBItem.gbid))
    {
        nAffixesCount = 0;
    }










    for (int32 i = 0; i < nAffixesCount; i++)
    {
        if (tGenerator.base_affixes(i) != GBID_NONE)
        {
            ItemsAddItemAffixGBID(tACD, tGenerator.base_affixes(i), dwAddAffixFlag, eIQL, FALSE, arAffixSpecificAttributes);
        }
    }

    if (tGenerator.flags() & (1 << IGF_RESTORED_BIT))
    {


        sSetItemAttribute(tACD, ATTRIBUTE_DURABILITY_MAX, (int32)tGenerator.max_durability());
    }
    else
    {

        float flPercentDurabilityBonus = tACD.AttributesGetFloat(ATTRIBUTE_ITEM_DURABILITY_PERCENT_BONUS);
        if (flPercentDurabilityBonus > 0.000001f)
        {
            int32 nMaxDurability = Math::FloatToInt(flPercentDurabilityBonus * tACD.AttributesGetInt(ATTRIBUTE_DURABILITY_MAX));

            sSetItemAttribute(tACD, ATTRIBUTE_DURABILITY_CUR, nMaxDurability);
            sSetItemAttribute(tACD, ATTRIBUTE_DURABILITY_MAX, nMaxDurability);
        }
    }


    GBID gbidEnchantReplacementAffix = ItemGetEnchantedAffixNew(tACD);
    uint32 dwEnchantedAffixSeed = ItemGetEnchantedAffixSeed(tACD);
    if (gbidEnchantReplacementAffix != GBID_NONE && dwEnchantedAffixSeed != 0 && (tGenerator.flags() & (1 << IGF_IDENTIFIED_BIT)))
    {



        RNG tStoredRNG = tItemRNG;
        RandSetSeed_Internal(tItemRNG, dwEnchantedAffixSeed);

        ItemsAddItemAffixGBID(tACD, gbidEnchantReplacementAffix, dwAddAffixFlag, eIQL, TRUE, arAffixSpecificAttributes);


        tItemRNG = tStoredRNG;
    }

    if (tGenerator.cube_enchanted_gem_rank() >= 0 &&
        tGenerator.cube_enchanted_gem_type() != 0)
    {
        const int32 MAX_CUBE_ENCHANT_RANK = 150;
        int32 nCubeEnchantLevel =
            Math::Pin(tGenerator.cube_enchanted_gem_rank(), 0, MAX_CUBE_ENCHANT_RANK);

        sSetItemAttribute(tACD, ATTRIBUTE_CUBE_ENCHANTED_GEM_TYPE, (int32)tGenerator.cube_enchanted_gem_type());
        sSetItemAttribute(tACD, ATTRIBUTE_CUBE_ENCHANTED_GEM_RANK, nCubeEnchantLevel);


        Attrib eAttrib = ItemGetAttributeForCubeEnchant(tACD);
        float flCurrentValue = tACD.AttributesGetFloat(eAttrib);
        float flNewValue = flCurrentValue + ItemGetValueForCubeEnchant(tACD);

        sSetItemAttribute(tACD, ItemGetAttributeForCubeEnchant(tACD), flNewValue);
    }

    if (tGenerator.flags() & (1 << IGF_ITEM_MARKED_JUNK))
    {
        sSetItemAttribute(tACD, ATTRIBUTE_ITEM_FLAGGED_AS_JUNK, 1);
    }


    int32 nDurabilityMax = tACD.AttributesGetInt(ATTRIBUTE_DURABILITY_MAX);
    int32 nDurabilityCurr = tACD.AttributesGetInt(ATTRIBUTE_DURABILITY_CUR);
    nDurabilityCurr = Math::Min(nDurabilityCurr, nDurabilityMax);
    sSetItemAttribute(tACD, ATTRIBUTE_DURABILITY_CUR, nDurabilityCurr);

    // bare scope: the DWARF lexical block for gbidTransmog spans lines 7423-7426 only
    {
        GBID gbidTransmog = tGenerator.transmog_gbid();
        if (gbidTransmog != GBID_NONE && ItemCrafting_CanUnlockTransmog(gbidTransmog, NULL, FALSE))
        {
            sSetItemAttribute(tACD, ATTRIBUTE_TRANSMOG_GBID, gbidTransmog);
        }
    }


    tACD.ptRNG = NULL;




    if (GameActiveGameCommonDataIsServer())
    {
        ItemSetIdentifyCost(tACD, SItemComputeIdentifyCost(tACD));
    }


    tACD.tRareItemName.Import(tGenerator.rare_item_name());




    if (tGenerator.item_binding_level() != IBL_NONE)
    {
        ItemSetBindingLevel(tACD, (ItemBindingLevel)tGenerator.item_binding_level());

        int32 nTradePlayerIdx = 0;
        for (int32 i = 0; i < tGenerator.trade_account_id_size(); i++)
        {
            uint64 uTradeAccountId = tGenerator.trade_account_id(i);
            if (uTradeAccountId != 0)
            {
                tACD.SetItemTradePlayerID(uTradeAccountId, nTradePlayerIdx);
                nTradePlayerIdx++;
            }
        }

        if (nTradePlayerIdx > 0)
        {
            uint32 uTradeTicksLeft = tGenerator.trade_ticks_left();
            uint32 uNow = GameGetServerTime();
            uint32 uTradeEndTime = uNow + uTradeTicksLeft;
            tACD.AttributesSetInt(ATTRIBUTE_ITEM_TRADE_END_TIME, uTradeEndTime);
        }
    }

    if (GameActiveGameCommonDataIsServer())
    {
        // condition order and layout inferred from columns: flags() inlined at 7475, the range test at 7477:46

        if (!(tGenerator.flags() & (1 << IGF_IDENTIFIED_BIT)) &&

            (eIQL >= IQL_RARE_AFFIX_START && eIQL <= IQL_RARE_AFFIX_END))
        {
            sSetItemAttribute(tACD, ATTRIBUTE_ITEM_QUALITY_LEVEL_IDENTIFIED, eIQL);

            eIQL = IQL_RARE_AFFIX_START;
        }
    }

    sSetItemAttribute(tACD, ATTRIBUTE_ITEM_QUALITY_LEVEL, eIQL);

    return TRUE;
}





ACDID CItemGenerate(
    ACDNetworkName ann,
    const D3::Items::Generator & tGenerator,
    ItemAffixFastAttribMap * arAffixSpecificAttributes)
{
    if (!ItemsValidateGeneratorParams(tGenerator))
        return ACDID_INVALID;




    uint32 uSocketCount = ItemSocketCountFromGenerator(tGenerator);
    ActorLocation tLocation;
    tLocation.eActorLocation = AL_NONE;
    GBHandle hGB(tGenerator.gb_handle());
    SNO snoActor = GameBalanceGetActorSNO(hGB);
    if (snoActor == SNO_NONE)
        return ACDID_INVALID;





    AttribGroupID idFastAttributeGroup = FastAttribGroupCreate(FAT_ITEM, -1);

    ACDID idACD = ACDCreate();
    ActorCommonData & tACD = ACDGet(idACD);
    tACD.Initialize(
        (ACDNetworkName)ann,  // inferred: the original stores a temporary copy of ann at 7522:3 just before the call (prvalue argument)
        DATAID_INVALID,
        snoActor,
        GBHandle(tGenerator.gb_handle()),
        SNONAME_NONE,
        0,
        tLocation,
        idFastAttributeGroup,
        (uSocketCount != 0),  // parentheses: the compare sits at column 17
        TRUE);

    if (!ItemGenerateCommon(tACD, tGenerator, arAffixSpecificAttributes))

        ACDDelete(tACD.id, FALSE);


    return tACD.id;
}





struct ItemGeneratorIterData { const ActorCommonData * ptACDItem; D3::Items::Generator * ptGenerator; };  // size 0x10





void sItemGeneratorSocket(ACDID idACDSquare, InventorySlotType eSlot, void * pUserData)
{
    ItemGeneratorIterData * ptParentData = (ItemGeneratorIterData *)pUserData;

    ActorCommonData * ptACDGem = ACDTryToGet(idACDSquare);
    if (ptACDGem == NULL)
    {
        return;
    }
    Actor * ptActor;
    D3::Items::EmbeddedGenerator * ptGem;
    if (GameActiveGameCommonDataIsClient())
    {

        D3::Items::EmbeddedGenerator * ptGem = ptParentData->ptGenerator->add_contents();
        ItemComputeConsoleItemId(*ptACDGem, *ptGem->mutable_id());
        ItemGeneratorPopulateFromACDID(*ptACDGem, *ptGem->mutable_generator(), TRUE);
        return;
    }


    if (ptACDGem->idOwner == ACTORID_INVALID)
    {
        return;
    }
    ptActor = ActorGet((ActorID)ptACDGem->idOwner);
    if (ptActor == NULL)
    {
        return;
    }
    if (ptActor->tItemId == OnlineService::ItemId::INVALID)
    {
        return;
    }







    ptGem = ptParentData->ptGenerator->add_contents();
    ptActor->tItemId.Export(*ptGem->mutable_id());
    ItemGeneratorPopulateFromACDID(*ptACDGem, *ptGem->mutable_generator(), TRUE);
}



void ItemGeneratorPopulateFromACDID(const ActorCommonData & tACDItem,
                                    D3::Items::Generator & tGenerator,
                                    BOOL bSetRestoredBit)

{


    ItemGeneratorToDefaults(tGenerator);

    if (GameActiveGameCommonDataIsClient())
    {

        tGenerator.set_seed(tACDItem.AttributesGetInt(ATTRIBUTE_SEED));

        tGenerator.set_season_created(tACDItem.AttributesGetInt(ATTRIBUTE_SEASON));
    }
    else
    {



        const Actor * ptActor = ActorGet((ActorID)tACDItem.idOwner);

        tGenerator.set_seed(ptActor->dwItemInitSeed);
        tGenerator.set_season_created(ptActor->dwSeasonCreated);
    }










    tACDItem.hGB.Export(*tGenerator.mutable_gb_handle());
    uint32 uFlags = tGenerator.flags();
    uFlags = ItemIsUnidentified(tACDItem) ? (uFlags & ~(1 << IGF_IDENTIFIED_BIT)) : (uFlags | (1 << IGF_IDENTIFIED_BIT));
    uFlags = (ItemGetBoundToACD(tACDItem, FALSE) != ACDID_INVALID) ? (uFlags | (1 << IGF_BOUND_BIT)) : (uFlags & ~(1 << IGF_BOUND_BIT));
    uFlags = ItemIsCrafted(tACDItem) ? (uFlags | (1 << IGF_CRAFTED_BIT)) : (uFlags & ~(1 << IGF_CRAFTED_BIT));
    uFlags = bSetRestoredBit ? (uFlags | (1 << IGF_RESTORED_BIT)) : (uFlags & ~(1 << IGF_RESTORED_BIT));
    uFlags = ItemIsJunk(tACDItem) ? (uFlags | (1 << IGF_ITEM_MARKED_JUNK)) : (uFlags & ~(1 << IGF_ITEM_MARKED_JUNK));
    uFlags = tACDItem.AttributesGetInt(ATTRIBUTE_IS_VENDOR_BOUGHT) ? (uFlags | (1 << IGF_VENDOR_BOUGHT_BIT)) : (uFlags & ~(1 << IGF_VENDOR_BOUGHT_BIT));
    uFlags = tACDItem.AttributesGetInt(ATTRIBUTE_LOOT_2_0_DROP) ? (uFlags | (1 << IGF_LOOT_2_0_BIT)) : (uFlags & ~(1 << IGF_LOOT_2_0_BIT));
    uFlags = tACDItem.AttributesGetInt(ATTRIBUTE_POST_2_1_2_DROP) ? (uFlags | (1 << IGF_POST_2_1_2_BIT)) : (uFlags & ~(1 << IGF_POST_2_1_2_BIT));
    uFlags = tACDItem.AttributesGetInt(ATTRIBUTE_POST_2_3_0_DROP) ? (uFlags | (1 << IGF_POST_2_3_0_BIT)) : (uFlags & ~(1 << IGF_POST_2_3_0_BIT));
    uFlags = tACDItem.AttributesGetInt(ATTRIBUTE_POST_2_5_0_DROP) ? (uFlags | (1 << IGF_POST_2_5_0_BIT)) : (uFlags & ~(1 << IGF_POST_2_5_0_BIT));
    uFlags = tACDItem.AttributesGetInt(ATTRIBUTE_CONSUMABLE_ADD_SOCKETS) ? (uFlags | (1 << IGF_ADD_SOCKETS_BIT)) : (uFlags & ~(1 << IGF_ADD_SOCKETS_BIT));
    uFlags = tACDItem.AttributesGetInt(ATTRIBUTE_REMOVE_LEVEL_REQUIREMENT) ? (uFlags | (1 << IGF_REMOVE_LEVEL_REQ)) : (uFlags & ~(1 << IGF_REMOVE_LEVEL_REQ));
    uFlags = tACDItem.AttributesGetInt(ATTRIBUTE_POST_260A_LEGENDARY_GEM_CAP) ? (uFlags | (1 << IGF_POST_2_6_0A_LEGENDARY_GEM_CAP_BIT)) : (uFlags & ~(1 << IGF_POST_2_6_0A_LEGENDARY_GEM_CAP_BIT));
    uFlags = tACDItem.AttributesGetInt(ATTRIBUTE_ITEM_WAS_TOUCHED_BY_ARMORY) ? (uFlags | (1 << IGF_TOUCHED_BY_ARMORY_BIT)) : (uFlags & ~(1 << IGF_TOUCHED_BY_ARMORY_BIT));
    uFlags = tACDItem.AttributesGetInt(ATTRIBUTE_ITEM_WAS_TOUCHED_BY_ARMORY_LAST_GAME) ? (uFlags | (1 << IGF_TOUCHED_BY_ARMORY_LAST_GAME_BIT)) : (uFlags & ~(1 << IGF_TOUCHED_BY_ARMORY_LAST_GAME_BIT));

    AncientRank eAncientRank = ItemGetAncientRank(tACDItem);
    uFlags = (eAncientRank == 1) ? (uFlags | (1 << IGF_ANCIENT_BIT)) : (uFlags & ~(1 << IGF_ANCIENT_BIT));
    uFlags = (eAncientRank == 2) ? (uFlags | (1 << IGF_PRIMAL_ANCIENT_BIT)) : (uFlags & ~(1 << IGF_PRIMAL_ANCIENT_BIT));

    tGenerator.set_flags(uFlags);


    const GBIDPooledList & listAffixGBIDs = (uFlags & (1 << IGF_IDENTIFIED_BIT)) ? tACDItem.listIdentifiedAffixGBIDs : tACDItem.listUnidentifiedAffixGBIDs;
    POS pos = listAffixGBIDs.GetHeadPosition();
    while (pos)
    {



        GBID gbid = listAffixGBIDs.GetNext(pos);
        if (gbid != GBID_NONE)
        {
            tGenerator.add_base_affixes(gbid);
        }
    }

    const D3::Items::Generator & tDefault = D3::Items::Generator::default_instance();

    GBID gbidTransmog = tACDItem.AttributesGetInt(ATTRIBUTE_TRANSMOG_GBID);
    if (gbidTransmog != GBID_NONE)
    {
        tGenerator.set_transmog_gbid(gbidTransmog);
    }

    GBID gbidEnchantedAffixOld = ItemGetEnchantedAffixOld(tACDItem);
    GBID gbidEnchantedAffixNew = ItemGetEnchantedAffixNew(tACDItem);
    if (gbidEnchantedAffixOld != tDefault.enchanted_affix_old() &&
        gbidEnchantedAffixNew != tDefault.enchanted_affix_new())
    {
        uint32 dwEnchantedAffixSeed = ItemGetEnchantedAffixSeed(tACDItem);
        tGenerator.set_enchanted_affix_old(gbidEnchantedAffixOld);
        tGenerator.set_enchanted_affix_new(gbidEnchantedAffixNew);
        tGenerator.set_enchanted_affix_seed(dwEnchantedAffixSeed);
        tGenerator.set_enchanted_affix_count(tACDItem.AttributesGetInt(ATTRIBUTE_ENCHANTED_AFFIX_COUNT));
    }

    if (ItemHasCubeEnchant(tACDItem))
    {
        tGenerator.set_cube_enchanted_gem_rank(tACDItem.AttributesGetInt(ATTRIBUTE_CUBE_ENCHANTED_GEM_RANK));
        tGenerator.set_cube_enchanted_gem_type(tACDItem.AttributesGetInt(ATTRIBUTE_CUBE_ENCHANTED_GEM_TYPE));
    }

    int32 nJewelRank = tACDItem.AttributesGetInt(ATTRIBUTE_JEWEL_RANK);
    if (ItemIsUpgradeableJewel(tACDItem) && nJewelRank > 0)
    {
        tGenerator.set_jewel_rank(nJewelRank);
    }


    tACDItem.tRareItemName.Export(tGenerator);

    tGenerator.set_durability(tACDItem.AttributesGetInt(ATTRIBUTE_DURABILITY_CUR));

    uint32 dwMaxDurability = tACDItem.AttributesGetInt(ATTRIBUTE_DURABILITY_MAX);
    if (dwMaxDurability != tDefault.max_durability())
    {
        tGenerator.set_max_durability(dwMaxDurability);
    }

    BOOL bIsGold = ItemHasLabel(tACDItem.hGB.gbid, ITL_GOLD);
    uint64 uStackSize = 0;

    if (ItemGetMaxStackSize(tACDItem.hGB, NULL, NULL) > 0 || bIsGold > 0)
    {
        uStackSize = tACDItem.GetStackCount();
    }
    tGenerator.set_stack_size(uStackSize);

    ItemQualityLevel eIQL = tACDItem.AttributesGetInt(ATTRIBUTE_ITEM_QUALITY_LEVEL);

    if (eIQL != tDefault.item_quality_level())
    {
        tGenerator.set_item_quality_level(eIQL);
    }

    ItemBindingLevel eIBL = (ItemBindingLevel)tACDItem.AttributesGetInt(ATTRIBUTE_ITEM_BINDING_LEVEL_OVERRIDE);
    if (eIBL != tDefault.item_binding_level())
    {
        tGenerator.set_item_binding_level(eIBL);
    }

    if (eIBL == IBL_BOUND_TO_GAME)
    {
        for (int32 i = 0; i < 4; i++)
        {
            uint64 uPlayerAccountID = tACDItem.GetItemTradePlayerID(i);
            if (uPlayerAccountID != 0)
            {
                tGenerator.add_trade_account_id(uPlayerAccountID);
            }
        }

        uint32 uTimeLeft = ItemGetTradeTimeLeft(tACDItem);
        tGenerator.set_trade_ticks_left(uTimeLeft);
    }

    DyeType eDyeType = tACDItem.AttributesGetInt(ATTRIBUTE_DYE_TYPE);
    SavedDyeType eSavedDyeType = DyeTypeTranslateToSavedDyeType(eDyeType);
    if (eSavedDyeType != tDefault.dye_type())
    {
        tGenerator.set_dye_type(eSavedDyeType);
    }

    int32 nLegendaryItemLevel = tACDItem.AttributesGetInt(ATTRIBUTE_ITEM_LEGENDARY_ITEM_LEVEL_OVERRIDE);
    if (nLegendaryItemLevel != 0 && !ItemIsCrafted(tACDItem))
    {
        tGenerator.set_legendary_item_level(nLegendaryItemLevel);
    }

    GBID gbidLegendaryBaseItem = tACDItem.AttributesGetInt(ATTRIBUTE_ITEM_LEGENDARY_ITEM_BASE_ITEM);
    if (gbidLegendaryBaseItem != GBID_NONE)
    {
        tGenerator.set_legendary_base_item_gbid(gbidLegendaryBaseItem);
    }


    int32 nConsoleMaxLevel = tACDItem.AttributesGetInt(ATTRIBUTE_ITEM_CONSOLE_MAX_LEVEL);
    if (nConsoleMaxLevel != 0)
    {
        tGenerator.set_console_max_level(nConsoleMaxLevel);
    }

    int32 nItemIsConsolePromo = tACDItem.AttributesGetInt(ATTRIBUTE_CONSOLE_PROMO_ITEM);
    if (nItemIsConsolePromo != 0)
    {
        tGenerator.set_console_promo_item(TRUE);
    }


    if (ItemIsLockedFromTrade(tACDItem))
    {
        uint64 uUnlockTime = tACDItem.GetUnlockTime();
        if (uUnlockTime != tDefault.item_unlock_timestamp())
        {
            tGenerator.set_item_unlock_timestamp(uUnlockTime);
        }
    }



    if (tACDItem.ptInventory != NULL && ItemHasSockets(tACDItem))
    {

        ItemGeneratorIterData data;
        data.ptACDItem = &tACDItem; data.ptGenerator = &tGenerator;


        ACDInventoryIterateItems(tACDItem, SLOT_ITEM_SOCKETS, 0, sItemGeneratorSocket, &data);
    }









}



uint32 ItemSocketCountFromGenerator(const D3::Items::Generator & tGenerator)
{
    if (tGenerator.flags() & (1 << IGF_ADD_SOCKETS_BIT))
    {
        return ItemGetMaxNumberOfSocketsFromGBID(tGenerator.gb_handle().gbid());
    }
    BOOL bHasEnchantedAffix = tGenerator.enchanted_affix_old() != GBID_NONE && tGenerator.enchanted_affix_new() != GBID_NONE;
    uint32 uSocketCount = ItemGetNumPredeterminedSockets(tGenerator.gb_handle().gbid());
    int32 nAffixesCount = Math::Min(tGenerator.base_affixes_size(), 6);
    for (int32 i = 0; i < nAffixesCount; i++)
    {
        if (tGenerator.base_affixes(i) == GBID_NONE)
            continue;
        if (bHasEnchantedAffix && tGenerator.base_affixes(i) == tGenerator.enchanted_affix_old())
            continue;
        uSocketCount += ItemAffixGetNumSockets(tGenerator.gb_handle().gbid(), tGenerator.base_affixes(i));
    }
    if (bHasEnchantedAffix)
        uSocketCount += ItemAffixGetNumSockets(tGenerator.gb_handle().gbid(), tGenerator.enchanted_affix_new());
    return uSocketCount;
}













void ItemDeleteAltAttribGroup(const ActorCommonData & tACDItem)
{
    if (GameActiveGameCommonDataIsServer())
    {
        AttribGroupID idAltAttribGroup;
        ItemGlobals * ptItemGlobals = GameGetActiveGameCommonData().ptItemGlobals;
        if (ptItemGlobals->mapAlternateAttributeGroups.Lookup(tACDItem.id, idAltAttribGroup))
        {
            FastAttribGroupDelete(idAltAttribGroup);
            ptItemGlobals->mapAlternateAttributeGroups.RemoveAt(tACDItem.id);
        }
    }
}




















































































BOOL ItemPlayerModifyCurrency(ActorCommonData & tACD, CurrencyType eCurrency, int64 nCount)
{
    Player * ptPlayer = PlayerGetByACD(tACD.id);
    if (ptPlayer == NULL)
        return FALSE;
    CurrencySavedData & tCurrencySavedData = ptPlayer->tCurrencySavedData;
    int64 & nCurrencyCount = tCurrencySavedData.arCurrency[eCurrency].nCount;
    int64 nOldCurrencyCount = nCurrencyCount;
    nCurrencyCount += nCount;
    GBHandle hGB(GB_ITEMS, CurrencyGetGBID(eCurrency));
    int64 nMaxStackSize = ItemGetMaxStackSize(hGB, &tACD, NULL);
    if (nCount > 0 && nMaxStackSize != 0)
        nCurrencyCount = Math::Min(nCurrencyCount, nMaxStackSize);
    if (nOldCurrencyCount != nCurrencyCount)
    {
        tCurrencySavedData.arCurrency[eCurrency].uFlags |= (1 << DPF_DIRTY_PERSIST) | (1 << DPF_DIRTY_CLIENTSYNC);
        tACD.MarkHeroStateCurrencyChanged();
        ServerQueueClientUpdate();
        if (eCurrency == CURRENCY_PLATINUM && nOldCurrencyCount == 0)
            Tutorial::SendTutorialToClient(GlobalSNOGet(GLOBALSNO_TUTORIAL_EARNED_PLATINUM), ptPlayer, "Root.NormalLayer.BattleNetFooter_main.LayoutRoot.ButtonContainer.StoreFrontButton", ACDID_INVALID);
    }
    return TRUE;
}






























BOOL ItemPlayerSetCurrency(ActorCommonData & tACD, CurrencyType eCurrency, int64 nAmount)
{
    Player * ptPlayer = PlayerGetByACD(tACD.id);
    if (ptPlayer == NULL)
        return FALSE;
    CurrencySavedData & tCurrencySavedData = ptPlayer->tCurrencySavedData;
    int64 & nCurrencyCount = tCurrencySavedData.arCurrency[eCurrency].nCount;
    int64 nOldCurrencyCount = nCurrencyCount;
    nCurrencyCount = nAmount;
    GBHandle hGB(GB_ITEMS, CurrencyGetGBID(eCurrency));
    int64 nMaxStackSize = ItemGetMaxStackSize(hGB, &tACD, NULL);
    if (nMaxStackSize != 0)
        nCurrencyCount = Math::Min(nCurrencyCount, nMaxStackSize);
    if (nOldCurrencyCount != nCurrencyCount)
    {
        tCurrencySavedData.arCurrency[eCurrency].uFlags |= (1 << DPF_DIRTY_PERSIST) | (1 << DPF_DIRTY_CLIENTSYNC);
        tACD.MarkHeroStateCurrencyChanged();
        ServerQueueClientUpdate();
    }
    return TRUE;
}




























int64 ItemPlayerGetCurrencyCount(const ActorCommonData & tACD, CurrencyType eCurrency)
{
    Player * ptPlayer = PlayerGetByACD(tACD.id);
    if (ptPlayer == NULL)
        return 0;
    CurrencySavedData & tCurrencySavedData = ptPlayer->tCurrencySavedData;
    return tCurrencySavedData.arCurrency[eCurrency].nCount;
}










BOOL ItemPlayerHasRecipeReagents(const Player & tPlayer, GBID gbidRecipe)
{
    ActorCommonData * ptACD = ACDTryToGet(tPlayer.idPlayerACD);
    if (ptACD == NULL)
        return FALSE;

    ScopedRecipeEntryLockR pdefRecipeEntry(gbidRecipe);
    int32 nCost = ItemCrafting_GetRecipeCost(gbidRecipe);
    if (!ptACD->CanAffordCurrencyCost(nCost, CURRENCY_GOLD))
        return FALSE;

    for (int32 i = 0; i < pdefRecipeEntry->nNumIngredients; i++)
    {
        const RecipeIngredient & tIngredient = pdefRecipeEntry->tIngredient[i];
        if (ItemIsCurrency(tIngredient.gbidItem))
        {
            CurrencyType eCurrency = CurrencyFromGBID(tIngredient.gbidItem);
            if (!ptACD->CanAffordCurrencyCost(tIngredient.nQuantity, eCurrency))
                return FALSE;
        }
        else if (ACDInventoryGetItemCount(*ptACD, GBHandle(GB_ITEMS, tIngredient.gbidItem), cgtCraftingPlayerInventorySlots, TRUE, FALSE) < (int32)tIngredient.nQuantity)
        {
            return FALSE;
        }
    }
    return TRUE;
}



























BOOL ItemPlayerConsumeRecipeReagents(const Player & tPlayer, GBID gbidRecipe)
{
    ActorCommonData * ptACD = ACDTryToGet(tPlayer.idPlayerACD);
    if (ptACD == NULL)
        return FALSE;

    if (!ItemPlayerHasRecipeReagents(tPlayer, gbidRecipe))
        return FALSE;

    ScopedRecipeEntryLockR pdefRecipeEntry(gbidRecipe);
    int32 nRecipeGold = ItemCrafting_GetRecipeCost(gbidRecipe);
    if (!ptACD->ModifyCurrencyAmount(-nRecipeGold, CURRENCY_GOLD, GMR_CRAFTING, FALSE))
        return FALSE;

    for (int32 i = 0; i < pdefRecipeEntry->nNumIngredients; i++)
    {
        const RecipeIngredient & tIngredient = pdefRecipeEntry->tIngredient[i];
        if (ItemIsCurrency(tIngredient.gbidItem))
        {
            CurrencyType eCurrency = CurrencyFromGBID(tIngredient.gbidItem);
            if (!ptACD->ModifyCurrencyAmount(-(int64)tIngredient.nQuantity, eCurrency, GMR_CRAFTING, FALSE))
                return FALSE;
        }
        else
        {
            int32 nRequiredCount = tIngredient.nQuantity;
            while (nRequiredCount > 0)
            {
                int32 nItemCount;
                ACDID idACDItem;
                GBHandle hGBItem(GB_ITEMS, tIngredient.gbidItem);
                idACDItem = ACDID_INVALID;
                ACDInventoryGetItem(*ptACD, hGBItem, cgtCraftingPlayerInventorySlots, idACDItem, nItemCount, TRUE);
                if (idACDItem == ACDID_INVALID)
                    return FALSE;
                ActorCommonData * ptACDItem = ACDTryToGet(idACDItem);
                if (ptACDItem == NULL)
                    return FALSE;
                SACDInventoryDeleteItem(*ptACDItem, nRequiredCount, TRUE, FALSE);
                nRequiredCount -= nItemCount;
            }
        }
    }
    return TRUE;
}































BOOL ItemPlayerHasRequiredCrafterLevelForItemType(const ActorCommonData & tACDPlayer, const ActorCommonData & tACDItem)
{
    if (!ACDIsPlayer(tACDPlayer.id))
        return FALSE;
    if (!tACDItem.IsItem())
        return FALSE;
    Player * ptPlayer = PlayerGetByACD(tACDPlayer.id);
    if (ptPlayer == NULL)
        return FALSE;
    int32 nItemRequiredCrafterLevelForEnchant = ItemGetRequiredCrafterLevelForEnchant(tACDItem);
    int32 nCrafterLevel = GetCraftLevel(CRAFTER_TYPE_MYSTIC, *ptPlayer);
    return nCrafterLevel >= nItemRequiredCrafterLevelForEnchant;
}




















BOOL ItemPlayerHasEnchantAffixReagents(ActorCommonData & tACDPlayer, ActorCommonData & tACDItem)
{
    RecipeIngredient arIngredients[6];
    Player * ptPlayer = PlayerGetByACD(tACDPlayer.id);
    if (ptPlayer && (ptPlayer->tHeroStateData.uPlayerFlags & (1 << PLAYERFLAG_FREE_ENCHANT)))
        return TRUE;

    int32 nEnchantCost = ItemCrafting_GetEnchantAffixCost(tACDItem);
    if (!tACDPlayer.CanAffordCurrencyCost(nEnchantCost, CURRENCY_GOLD))
        return FALSE;

    int32 nIngredientsCount = ItemCrafting_GetEnchantAffixReagents(tACDItem, arIngredients, 6);
    for (int32 i = 0; i < nIngredientsCount; i++)
    {
        const RecipeIngredient & tIngredient = arIngredients[i];
        if (ItemIsCurrency(tIngredient.gbidItem))
        {
            CurrencyType eCurrency = CurrencyFromGBID(tIngredient.gbidItem);
            if (!tACDPlayer.CanAffordCurrencyCost(tIngredient.nQuantity, eCurrency))
                return FALSE;
        }
        else
        {
            GBHandle hGBIngredient;
            hGBIngredient.Set(GB_ITEMS, tIngredient.gbidItem);
            int32 nItemCount = ACDInventoryGetItemCount(tACDPlayer, hGBIngredient, cgtCraftingPlayerInventorySlots, TRUE, FALSE);
            if (nItemCount < (int32)tIngredient.nQuantity)
                return FALSE;
        }
    }
    return TRUE;
}























BOOL ItemPlayerConsumeEnchantAffixReagents(
    ActorCommonData & tACDPlayer,
    ActorCommonData & tACDItem)
{
    Player * ptPlayer = PlayerGetByACD(tACDPlayer.id);
    if (ptPlayer && (ptPlayer->tHeroStateData.uPlayerFlags & (1 << PLAYERFLAG_FREE_ENCHANT)))
        return TRUE;



    if (!ItemPlayerHasEnchantAffixReagents(tACDPlayer, tACDItem))
        return FALSE;

    {



        int32 nEnchantCost = ItemCrafting_GetEnchantAffixCost(tACDItem);
        if (!tACDPlayer.ModifyCurrencyAmount(-nEnchantCost, CURRENCY_GOLD, GMR_CRAFTING, FALSE))
            return FALSE;



        SGameStatsPlayerCounterIncrement(ptPlayer, STAT_COUNTER_PLAYER_CURRENCY_SPENT_ON_ENCHANTING, 0, 0, (float)nEnchantCost);


        RecipeIngredient arIngredients[6];
        int32 nIngredientsCount = ItemCrafting_GetEnchantAffixReagents(tACDItem, arIngredients, 6);

        for (int32 i = 0; i < nIngredientsCount; i++)
        {
            const RecipeIngredient & tIngredient = arIngredients[i];

            if (ItemIsCurrency(tIngredient.gbidItem))
            {
                CurrencyType eCurrency = CurrencyFromGBID(tIngredient.gbidItem);

                if (!tACDPlayer.ModifyCurrencyAmount(-(int64)tIngredient.nQuantity, eCurrency, GMR_CRAFTING, FALSE))
                    return FALSE;
            }
            else
            {


                GBHandle hGBIngredient;
                hGBIngredient.Set(GB_ITEMS, tIngredient.gbidItem);

                int32 nRequiredCount = tIngredient.nQuantity;
                while (nRequiredCount > 0)
                {
                    int32 nItemCount = 0;
                    ACDID idACDIngredient = ACDID_INVALID;
                    ACDInventoryGetItem(tACDPlayer, hGBIngredient, cgtCraftingPlayerInventorySlots, idACDIngredient, nItemCount, TRUE);

                    ActorCommonData * ptACDIngredient = ACDTryToGet(idACDIngredient);
                    if (ptACDIngredient == NULL)
                        return FALSE;










                    SGameStatsPlayerCounterIncrement(ptPlayer, STAT_COUNTER_PLAYER_ITEM_SPENT_ON_ENCHANTING, tIngredient.gbidItem, 0, (float)nRequiredCount);


                    SACDInventoryDeleteItem(*ptACDIngredient, nRequiredCount, TRUE, FALSE);
                    nRequiredCount -= nItemCount;
                }
            }
        }
    }

    return TRUE;
}



BOOL ItemPlayerHasReforgeReagents(ActorCommonData & tACDPlayer, ActorCommonData & tACDItem)
{
    RecipeIngredient arIngredients[6];
    int32 nReforgeCost = ItemCrafting_GetReforgeCost(tACDItem.id);
    if (!tACDPlayer.CanAffordCurrencyCost(nReforgeCost, CURRENCY_GOLD))
        return FALSE;
    int32 nIngredientsCount = ItemCrafting_GetReforgeReagents(tACDItem, arIngredients);
    if (nReforgeCost <= 0 && nIngredientsCount <= 0)
        return FALSE;
    for (int32 i = 0; i < nIngredientsCount; i++)
    {
        GBHandle hGBIngredient(GB_ITEMS, arIngredients[i].gbidItem);
        int32 nItemCount = ACDInventoryGetItemCount(tACDPlayer, hGBIngredient, cgtCraftingPlayerInventorySlots, TRUE, FALSE);
        if (nItemCount < (int32)arIngredients[i].nQuantity)
            return FALSE;
    }
    return TRUE;
}






















BOOL ItemPlayerConsumeReforgeReagents(
    ActorCommonData & tACDPlayer,
    ActorCommonData & tACDItem)
{
    if (!ItemPlayerHasReforgeReagents(tACDPlayer, tACDItem))
        return FALSE;








    int32 nReforgeCost = ItemCrafting_GetReforgeCost(tACDItem.id);
    if (!tACDPlayer.ModifyCurrencyAmount(-nReforgeCost, CURRENCY_GOLD, GMR_CRAFTING, FALSE))
        return FALSE;



    RecipeIngredient arIngredients[6];
    int32 nIngredientsCount = ItemCrafting_GetReforgeReagents(tACDItem, arIngredients);

    for (int32 i = 0; i < nIngredientsCount; i++)
    {
        GBHandle hGBIngredient;
        hGBIngredient.Set(GB_ITEMS, arIngredients[i].gbidItem);

        int32 nRequiredCount = arIngredients[i].nQuantity;
        while (nRequiredCount > 0)
        {
            int32 nItemCount = 0;
            ACDID idACDIngredient = ACDID_INVALID;
            ACDInventoryGetItem(tACDPlayer, hGBIngredient, cgtCraftingPlayerInventorySlots, idACDIngredient, nItemCount, TRUE);

            ActorCommonData * ptACDIngredient = ACDTryToGet(idACDIngredient);
            if (ptACDIngredient == NULL)
                return FALSE;









            SACDInventoryDeleteItem(*ptACDIngredient, nRequiredCount, TRUE, FALSE);
            nRequiredCount -= nItemCount;
        }
    }

    return TRUE;
}



















BOOL ItemPlayerChargeTransmogCosts(ActorCommonData & tACDPlayer, const ActorCommonData & tACDItem, GBID gbidTransmog)
{
    int32 nCost = ItemCrafting_GetTransmogCost(tACDItem, gbidTransmog);
    if (nCost > 0 && !tACDPlayer.CanAffordCurrencyCost(nCost, CURRENCY_GOLD))
        return FALSE;
    SGameStatsPlayerCounterIncrement(PlayerGetByACD(tACDPlayer.id), STAT_COUNTER_PLAYER_CURRENCY_SPENT_ON_TRANSMOG, 0, 0, (float)nCost);
    if (nCost <= 0)
        return TRUE;
    return tACDPlayer.ModifyCurrencyAmount(-nCost, CURRENCY_GOLD, GMR_CRAFTING, FALSE) != FALSE;
}


















BOOL ItemPlayerChargeDyeItemCosts(ActorCommonData & tACDPlayer, DyeType eDyeType)
{
    GBID gbidDye = ItemGetGBIDForDyeType(eDyeType);
    if (gbidDye == GBID_NONE)
        return FALSE;
    BOOL bResult = TRUE;
    ScopedItemDefinitionLockR pdefDye(gbidDye);
    int32 nCost = pdefDye->nBaseCostGold;
    if (nCost > 0)
    {
        if (tACDPlayer.CanAffordCurrencyCost(nCost, CURRENCY_GOLD))
            bResult = tACDPlayer.ModifyCurrencyAmount(-nCost, CURRENCY_GOLD, GMR_CRAFTING, FALSE) != FALSE;
        else
            bResult = FALSE;
    }
    return bResult;
}



















ItemQualityLevel ItemDetermineUnvalidatedQualityLevel(int32 nMagicAffixCount)
{
    if (nMagicAffixCount <= 0)
        return IQL_NORMAL;
    return (ItemQualityLevel)Math::Pin(nMagicAffixCount + IQL_AFFIX_COUNT_START - 1, (int)IQL_AFFIX_COUNT_START, (int)IQL_RARE_AFFIX_END);
}



ItemQualityLevel ItemDetermineQualityLevel(LPCSTR szDebugStr, GBHandle hGBItem, const GBID * ptGBIDAffixes, const int32 nAffixCount, const int32 nBonusRandomAffixes)
{
    if (ItemIsLegendary(hGBItem.gbid))
        return (ItemQualityLevel)IQL_LEGENDARY;

    if (ItemIsSpecial(hGBItem.gbid))
        return IQL_SPECIAL;

    for (int32 i = 0; i < nAffixCount; i++)
    {
        GBID gbidAffix = ptGBIDAffixes[i];
        ScopedItemAffixLockR ptAffix(gbidAffix);
        if (ptAffix == NULL)
            continue;
        if (ptAffix->eAffixType == AT_QUALITY)
        {
            if (ptAffix->dwAllowedQualityLevels & (1 << IQL_INFERIOR))
                return IQL_INFERIOR;
            if (ptAffix->dwAllowedQualityLevels & (1 << IQL_SUPERIOR))
                return IQL_SUPERIOR;
        }
    }

    return ItemDetermineUnvalidatedQualityLevel(nAffixCount + nBonusRandomAffixes);
}






































CRefString ItemGetDurability(const ActorCommonData & tACDItem, const BOOL bNumericOnly, const BOOL bShowAsRepaired)
{
    CRefString usDurability;
    if (ItemIsHealthPotion(tACDItem))
        return usDurability;

    int32 nMaxDurability = ItemGetMaxDurability(tACDItem);
    if (nMaxDurability == 0)
        return usDurability;

    usDurability = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_ITEMS), guszItemLabelDurability);
    int32 nDurability = nMaxDurability;
    if (!bShowAsRepaired)
        nDurability = ItemGetCurrentDurability(tACDItem);

    CRefString sValue = CRefString().Format("%d/%d", nDurability, nMaxDurability);
    if (nDurability == 0)
    {
        Set_Text_Color(sValue, RGBAColor(255, 0, 0, 255));
    }
    else if ((float)nDurability / (float)nMaxDurability <= 0.2f)
    {
        Set_Text_Color(sValue, RGBAColor(240, 210, 5, 255));
    }

    if (bNumericOnly)
        return sValue;

    usDurability += sValue;
    return usDurability;
}





















BOOL ItemIsBroken(const ActorCommonData & tACDItem)
{
    if (tACDItem.AttributesGetInt(ATTRIBUTE_DURABILITY_MAX) == 0 || GameTypeIsPvP())
        return FALSE;
    return ItemGetCurrentDurability(tACDItem) == 0;
}












BOOL ItemIsEquippedButDisabled(const ActorCommonData & tACDItem)
{
    if (tACDItem.AttributesGetInt(ATTRIBUTE_ITEM_EQUIPPED) && tACDItem.AttributesGetInt(ATTRIBUTE_ITEM_EQUIPPED_BUT_DISABLED))
        return TRUE;
    return FALSE;
}









BOOL ItemIsEquippedButDisabledDuplicateLegendary(const ActorCommonData & tACDItem)
{
    if (tACDItem.AttributesGetInt(ATTRIBUTE_ITEM_EQUIPPED) && tACDItem.AttributesGetInt(ATTRIBUTE_ITEM_EQUIPPED_BUT_DISABLED) && tACDItem.AttributesGetInt(ATTRIBUTE_ITEM_EQUIPPED_BUT_DISABLED_DUPLICATE_LEGENDARY))
        return TRUE;
    return FALSE;
}










BOOL ItemIsDamaged(const ActorCommonData & tACDItem)
{
    ScopedGlobalDefinitionLockR pdefGlobals;
    return ItemGetDurabilityPercent(tACDItem) <= pdefGlobals->flItemsDamagedThresh;
}



void ItemModifyDurability(const ActorCommonData & tACDPlayer, ActorCommonData & tACDItem, int32 nDurabilityChange)
{



    if (nDurabilityChange < 0 &&
        !PlayerAffectedByDurability(tACDPlayer))
    {
        return;
    }



    if (CmdLineGetParams()->eTestType != TESTTYPE_NONE)
    {
        return;
    }


    if (nDurabilityChange < 0 && tACDPlayer.IsDueling())
    {
        return;
    }

    if (tACDItem.AttributesGetInt(ATTRIBUTE_ITEM_INDESTRUCTIBLE))
    {
        return;
    }

    BOOL bItemWasBrokenOrDisabled = ItemIsBroken(tACDItem) || ItemIsEquippedButDisabled(tACDItem);
    int32 nMaxDurability = tACDItem.AttributesGetInt(ATTRIBUTE_DURABILITY_MAX);
    if (nMaxDurability == 0)
    {
        return;
    }

    int32 nCurrentDurability = tACDItem.AttributesGetInt(ATTRIBUTE_DURABILITY_CUR);
    int32 nNewDurability = nCurrentDurability + nDurabilityChange;
    nNewDurability = Math::Pin(nNewDurability, 0, nMaxDurability);



    sSetItemAttribute(tACDItem, ATTRIBUTE_DURABILITY_CUR, nNewDurability);
    BOOL bItemIsBrokenOrDisabled = ItemIsBroken(tACDItem) || ItemIsEquippedButDisabled(tACDItem);



    SACDInventoryAddToDirtyList(tACDItem, AIDA_UPDATE_DURABILITY, tACDItem.GetLocationSlot(), tACDItem.GetLocationSlot());


    if (bItemWasBrokenOrDisabled && bItemIsBrokenOrDisabled)
    {
        return;
    }



    if (!tACDItem.AttributesGetInt(ATTRIBUTE_ITEM_EQUIPPED) || !tACDItem.IsInInventory())
    {
        return;
    }

    InventoryLocation & tLocation = tACDItem.GetInventoryLocation();
    ActorCommonData * ptACDOwner = ACDTryToGet(tLocation.idACDContainer);







    if (ptACDOwner == NULL)
    {
        return;
    }

    if (bItemIsBrokenOrDisabled)
    {
        if (tACDItem.idUpscaledAttribGroup != -1)
        {
            ptACDOwner->AttributesUnmergeAttributeGroup(tACDItem.idUpscaledAttribGroup);
            ClearUpscaledAttributes(tACDItem);
        }
        else
        {

            AttribGroupID idFastAttribItem = SItemGetAttributeGroup(tACDItem);
            ptACDOwner->AttributesUnmergeAttributeGroup(idFastAttribItem);
            ACDInventoryEquipmentOrSkillChanged(*ptACDOwner);
            VisualInventoryUpdateItem(tACDItem);
        }
    }
    else if (bItemWasBrokenOrDisabled)
    {
        ActorInventoryMergeItemAttributes(*ptACDOwner, tACDItem);
        ACDInventoryEquipmentOrSkillChanged(*ptACDOwner);
        VisualInventoryUpdateItem(tACDItem);
    }


}


BOOL ItemIsAllowedInSlot(InventorySlotType eSlot, const ItemTypeDefinition * pItemTypeDef)
{
    if (pItemTypeDef->eBodySlot1 == eSlot || pItemTypeDef->eBodySlot2 == eSlot || pItemTypeDef->eBodySlot3 == eSlot || pItemTypeDef->eBodySlot4 == eSlot)
    {
        return TRUE;
    }
    return FALSE;
}










static inline int32 sGetDurabilityScaleFactor()
{
    ScopedGlobalDefinitionLockR pdefGlobal;
    int32 factor = pdefGlobal->nDurabilityScaleFactor;
    if (factor == 0)
        factor = 1;
    return factor;
}


BOOL ItemCanBeRepaired(const ActorCommonData & tACDItem)
{
    int32 nMaxDurabilityRaw = tACDItem.AttributesGetInt(ATTRIBUTE_DURABILITY_MAX);
    int32 nCurrentDurabilityRaw = tACDItem.AttributesGetInt(ATTRIBUTE_DURABILITY_CUR);
    int32 nFactor = sGetDurabilityScaleFactor();
    return nCurrentDurabilityRaw / nFactor != nMaxDurabilityRaw / nFactor;
}











ItemReforgeError ItemCanBeReforged(const ActorCommonData & tACDItem)
{
    if (ItemIsUnidentified(tACDItem))
        return IRE_UNIDENTIFIED;
    if (!ItemIsLegendary(tACDItem.hGB.gbid))
        return IRE_NOT_LEGENDARY;
    if (!tACDItem.AttributesGetInt(ATTRIBUTE_LOOT_2_0_DROP))
        return IRE_LEGACY_ITEM;
    if (ItemIsCrafted(tACDItem))
        return IRE_CRAFTED;
    if (!ItemIsEquippableByPlayerOrHireling(tACDItem))
        return IRE_CANNOT_EQUIP;
    if (ItemGetNumFilledSockets(tACDItem) > 0)
        return IRE_HAS_GEMS;
    return IRE_NONE;
}



























float ItemGetDurabilityPercent(const ActorCommonData & tACDItem)
{
    int32 nFactor = sGetDurabilityScaleFactor();
    int32 nMaxDurability = tACDItem.AttributesGetInt(ATTRIBUTE_DURABILITY_MAX) / nFactor;
    if (nMaxDurability == 0)
        return 1.0f;
    int32 nCurrentDurability = tACDItem.AttributesGetInt(ATTRIBUTE_DURABILITY_CUR) / nFactor;
    return (float)nCurrentDurability / (float)nMaxDurability;
}









static void sDecayItem(ActorCommonData & tACDPlayer, const InventorySlotType eSlot)
{
    ACDID idACDItem = ACDInventoryGetSquareItem(tACDPlayer, eSlot, IVector2D(0, 0));
    ActorCommonData * ptACDItem = ACDTryToGet(idACDItem);
    if (ptACDItem != NULL && ptACDItem->IsItem())
        ItemModifyDurability(tACDPlayer, *ptACDItem, -1);
}






extern const InventorySlotType cgtInventorySlotsThatTakeDamage[14] = {
    SLOT_PLAYER_HEAD, SLOT_PLAYER_TORSO, SLOT_PLAYER_RIGHT_HAND, SLOT_PLAYER_LEFT_HAND, SLOT_PLAYER_HANDS,
    SLOT_PLAYER_WAIST, SLOT_PLAYER_FEET, SLOT_PLAYER_SHOULDERS, SLOT_PLAYER_LEGS, SLOT_PLAYER_BRACERS,
    SLOT_PLAYER_LEFT_FINGER, SLOT_PLAYER_RIGHT_FINGER, SLOT_PLAYER_NECK, SLOT_NONE
};
















void ItemsTakeDamage(ActorCommonData * ptACDOwner)
{
    if (ptACDOwner == NULL || !ACDIsPlayer(ptACDOwner->id))
        return;
    ScopedGlobalDefinitionLockR pdefGlobal;
    uint32 nDelay = (uint32)(pdefGlobal->ptServerData->flDurLossFrequency * 60.0f);
    uint32 nLastDamagedTimeStamp = ptACDOwner->AttributesGetInt(ATTRIBUTE_DURABILITY_LAST_DAMAGE);
    uint32 dwNow = GameGetServerTime();
    uint32 nNextDamageTime = nLastDamagedTimeStamp + nDelay;
    if (nNextDamageTime > dwNow)
        return;
    LootAddUnluckyAndAvengerTicks(*ptACDOwner, nDelay);
    ptACDOwner->AttributesSetInt(ATTRIBUTE_DURABILITY_LAST_DAMAGE, dwNow);
    for (int32 nIndex = 0; cgtInventorySlotsThatTakeDamage[nIndex] != SLOT_NONE; nIndex++)
        sDecayItem(*ptACDOwner, cgtInventorySlotsThatTakeDamage[nIndex]);
}
























int32 ItemGetCurrentDurability(const ActorCommonData & tACDItem)
{
    int32 nCurrentDurabilityRaw = tACDItem.AttributesGetInt(ATTRIBUTE_DURABILITY_CUR);
    return nCurrentDurabilityRaw / sGetDurabilityScaleFactor();
}




int32 ItemGetMaxDurability(const ActorCommonData & tACDItem)
{
    int32 nCurrentDurabilityRaw = tACDItem.AttributesGetInt(ATTRIBUTE_DURABILITY_MAX);
    return nCurrentDurabilityRaw / sGetDurabilityScaleFactor();
}




BOOL ItemIsDyeableGenerationCheck(const ActorCommonData & tACDItem)
{
    if (!tACDItem.IsItem())
        return FALSE;
    GBID gbidItem = tACDItem.hGB.gbid;
    if (ItemHasLabel(gbidItem, ITL_HELM) || ItemHasLabel(gbidItem, ITL_SHOULDERS) || ItemHasLabel(gbidItem, ITL_GLOVES) || ItemHasLabel(gbidItem, ITL_BOOTS) || ItemHasLabel(gbidItem, ITL_LEGS) || ItemHasLabel(gbidItem, ITL_CHESTARMOR))
    {
        ScopedItemDefinitionLockR ptItem(gbidItem);
        BOOL fDyeable = FALSE;
        if (ptItem != NULL)
            fDyeable = (ptItem->dwFlags >> ITEM_DYEABLE_BIT) & 1;
        return fDyeable;
    }
    return FALSE;
}

















BOOL ItemIsDyeable(const ActorCommonData & tACDItem)
{
    if (!tACDItem.IsItem())
        return FALSE;
    GBID gbidItem = tACDItem.hGB.gbid;
    if (ItemHasLabel(gbidItem, ITL_HELM) || ItemHasLabel(gbidItem, ITL_SHOULDERS) || ItemHasLabel(gbidItem, ITL_GLOVES) || ItemHasLabel(gbidItem, ITL_BOOTS) || ItemHasLabel(gbidItem, ITL_LEGS) || ItemHasLabel(gbidItem, ITL_CHESTARMOR))
    {
        if (ItemIsUnidentified(tACDItem))
            return FALSE;
        return TRUE;
    }
    return FALSE;
}
















GameError IsSocketAddable(const ActorCommonData & tACDItemTarget, const Attrib eAttribAddSocketsType)
{
    if (ItemIsUnidentified(tACDItemTarget))
        return GAMEERROR_ITEMUSE_NOT_ON_UNIDENTIFIED_ITEMS;
    if (!tACDItemTarget.IsItem() || ItemGetMaxNumberOfSockets(tACDItemTarget) == 0)
        return GAMEERROR_IARESULT_CANNOT_SOCKET;
    BOOL bCorrectItemType = FALSE;
    switch (eAttribAddSocketsType)
    {
    case ATTRIBUTE_ADD_SOCKETS_TYPE_WEAPON:
        bCorrectItemType = ACDInventoryItemAllowedInThisSlotType(tACDItemTarget.hGB.gbid, SLOT_PLAYER_LEFT_HAND);
        break;
    case ATTRIBUTE_ADD_SOCKETS_TYPE_OFFHAND:
        if (ACDInventoryItemAllowedInThisSlotType(tACDItemTarget.hGB.gbid, SLOT_PLAYER_RIGHT_HAND))
            bCorrectItemType = ItemHasLabel(tACDItemTarget.hGB.gbid, ITL_OFFHAND);
        break;
    case ATTRIBUTE_ADD_SOCKETS_TYPE_LEGS:
        bCorrectItemType = ACDInventoryItemAllowedInThisSlotType(tACDItemTarget.hGB.gbid, SLOT_PLAYER_LEGS);
        break;
    case ATTRIBUTE_ADD_SOCKETS_TYPE_TORSO:
        bCorrectItemType = ACDInventoryItemAllowedInThisSlotType(tACDItemTarget.hGB.gbid, SLOT_PLAYER_TORSO);
        break;
    case ATTRIBUTE_ADD_SOCKETS_TYPE_HEAD:
        bCorrectItemType = ACDInventoryItemAllowedInThisSlotType(tACDItemTarget.hGB.gbid, SLOT_PLAYER_HEAD);
        break;
    case ATTRIBUTE_ADD_SOCKETS_TYPE_JEWELRY:
        if (ACDInventoryItemAllowedInThisSlotType(tACDItemTarget.hGB.gbid, SLOT_PLAYER_RIGHT_FINGER) ||
            ACDInventoryItemAllowedInThisSlotType(tACDItemTarget.hGB.gbid, SLOT_PLAYER_LEFT_FINGER) ||
            ACDInventoryItemAllowedInThisSlotType(tACDItemTarget.hGB.gbid, SLOT_PLAYER_NECK))
            bCorrectItemType = TRUE;
        break;
    }
    if (!bCorrectItemType)
        return GAMEERROR_IARESULT_CANNOT_ADD_SOCKETS_WITH_THIS_ITEM;
    if (ItemHasSockets(tACDItemTarget))
        return GAMEERROR_IARESULT_ALREADY_SOCKETED;
    return GAMEERROR_NONE;
}




























GameError IsSocketAddable(const ActorCommonData & tACDItemTarget, const ActorCommonData & tACDItemToUse)
{
    Attrib eAttribAddSocketsType;
    if (tACDItemToUse.AttributesGetInt(ATTRIBUTE_ADD_SOCKETS_TYPE_WEAPON))
        eAttribAddSocketsType = ATTRIBUTE_ADD_SOCKETS_TYPE_WEAPON;
    else if (tACDItemToUse.AttributesGetInt(ATTRIBUTE_ADD_SOCKETS_TYPE_OFFHAND))
        eAttribAddSocketsType = ATTRIBUTE_ADD_SOCKETS_TYPE_OFFHAND;
    else if (tACDItemToUse.AttributesGetInt(ATTRIBUTE_ADD_SOCKETS_TYPE_TORSO))
        eAttribAddSocketsType = ATTRIBUTE_ADD_SOCKETS_TYPE_TORSO;
    else if (tACDItemToUse.AttributesGetInt(ATTRIBUTE_ADD_SOCKETS_TYPE_HEAD))
        eAttribAddSocketsType = ATTRIBUTE_ADD_SOCKETS_TYPE_HEAD;
    else if (tACDItemToUse.AttributesGetInt(ATTRIBUTE_ADD_SOCKETS_TYPE_LEGS))
        eAttribAddSocketsType = ATTRIBUTE_ADD_SOCKETS_TYPE_LEGS;
    else if (tACDItemToUse.AttributesGetInt(ATTRIBUTE_ADD_SOCKETS_TYPE_JEWELRY))
        eAttribAddSocketsType = ATTRIBUTE_ADD_SOCKETS_TYPE_JEWELRY;
    else
        return GAMEERROR_IARESULT_CANNOT_ADD_SOCKETS_WITH_THIS_ITEM;
    return IsSocketAddable(tACDItemTarget, eAttribAddSocketsType);
}














BOOL ItemIsDye(const ActorCommonData & tACDItem)
{
    if (!tACDItem.IsItem())
        return FALSE;
    ItemUseType eUseType = ItemGetUseType(tACDItem);
    return eUseType == ITEM_USE_DYE || eUseType == ITEM_USE_DYE_REUSABLE;
}






BOOL ItemIsDye(GBID gbidItem)
{
    ScopedItemDefinitionLockR ptItem(gbidItem);
    BOOL bIsDye = FALSE;
    if (ptItem != NULL)
        bIsDye = ItemHasLabel(gbidItem, ITL_DYE);
    return bIsDye;
}








static DyeTypeToGBIDMap sgtDyeMap;

static void sLazyInitDyeMap()
{

    if (sgtDyeMap.empty())
    {
        GBHandleList listItems(GBGetHandlePool());
        GBEnumerate(GB_ITEMS, listItems);
        POS pos = listItems.GetHeadPosition();
        while (pos)
        {
            GBID gbidItem = listItems.GetNext(pos).gbid;
            if (ItemIsDye(gbidItem))
            {
                ScopedItemDefinitionLockR ptDyeDef(gbidItem);
                for (int32 i = 0; i < (int32)(ptDyeDef->serAttributes_Console.dwSize / sizeof(AttributeSpecifier)); i++)
                {
                    if (ptDyeDef->ptAttributes[i].eAttribute == ATTRIBUTE_DYE_TYPE)
                    {
                        SavedDyeType eSavedDyeType = (SavedDyeType)ItemGetAttributeSpecifierValue(gbidItem, ptDyeDef->ptAttributes[i], NULL, NULL, 0, FALSE);
                        DyeType eItemDyeType = DyeTypeTranslateFromSavedDyeType(eSavedDyeType);


                        sgtDyeMap[eItemDyeType] = gbidItem;
                    }
                }
            }
        }
    }
}


GBID ItemGetGBIDForDyeType(DyeType eDyeType)
{
    GBID gbidDye = GBID_NONE;

    sLazyInitDyeMap();
    blz::rb_tree_iterator<blz::pair<const int, int> > iter = sgtDyeMap.find(eDyeType);
    if (iter != sgtDyeMap.end())
    {
        gbidDye = iter->second;
    }
    return gbidDye;
}


BOOL ItemIsAddSocketsItem(const ActorCommonData & tACDItem)
{
    if (!tACDItem.IsItem())
        return FALSE;
    return ItemGetUseType(tACDItem) == ITEM_USE_ADD_SOCKETS;
}







BOOL ItemIsTradeable(const Player & tOwner, const Player * ptPlayerDest, const ActorCommonData & tACDItem, const BOOL bCheckSockets)
{
    if (!tACDItem.IsItem())
        return FALSE;

    if (ItemGetBindingLevel(tACDItem) == IBL_BOUND_TO_GAME)
    {
        if (ptPlayerDest == NULL)
            return FALSE;

        BOOL bFound = FALSE;
        for (int32 i = 0; i < 4; i++)
        {
            uint64 uTradePlayerID = tACDItem.GetItemTradePlayerID(i);
            if (uTradePlayerID != 0 && uTradePlayerID == ptPlayerDest->tAccount.tHandle.m_id)
            {
                bFound = TRUE;
                break;
            }
        }
        if (!bFound)
            return FALSE;
    }

    if (ItemGetBoundToACD(tACDItem, bCheckSockets) != ACDID_INVALID)
        return FALSE;
    if (ItemIsLockedFromTrade(tACDItem))
        return FALSE;
    if (ItemIsQuest(tACDItem))
        return FALSE;
    if (!tACDItem.IsInInventory())
        return FALSE;
    if (tOwner.idPlayerACD != tACDItem.GetInventoryContainerOwner())
        return FALSE;

    const ActorLocation & tLocation = tACDItem.GetLocation();
    if (tLocation.tInvLocation.idACDContainer != tOwner.idPlayerACD)
        return FALSE;

    if (ItemIsCosmeticItem(tACDItem.hGB.gbid))
        return TRUE;

    return ItemCanDrop(tACDItem) == GAMEERROR_NONE;
}




































GameError ItemCanDrop(const ActorCommonData & tACDItem)
{
    ScopedActorDefinitionLockR pdefActor(tACDItem.snoActor);
    if (TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_INVENTORY_MUST_KEEP))
        return GAMEERROR_IARESULT_UNKNOWN;
    return GAMEERROR_NONE;
}






BOOL ItemIsSetItem(const GBID gbid)
{
    ScopedItemDefinitionLockR ptItem(gbid);
    if (ptItem == NULL)
        return FALSE;
    return ptItem->gbidSet != GBID_NONE;
}











GBID ItemGetSet(const GBID gbid)
{
    ScopedItemDefinitionLockR ptItem(gbid);
    if (ptItem == NULL)
        return GBID_NONE;
    return ptItem->gbidSet;
}






int32 ItemGetNumberOfItemsInSameSet(const GBID gbid)
{
    ScopedItemDefinitionLockR ptItem(gbid);
    if (ptItem == NULL)
        return 0;
    GBID gbidSet = ptItem->gbidSet;
    if (gbidSet == GBID_NONE)
        return 0;
    const ItemAppGlobals * ptItemAppGlobals = sgtAppGlobals.ptItemAppGlobals;
    int32 nMemberItems = 0;
    uint32 dwDisallowedItemFlags = (1 << ITEM_TEST_ONLY_BIT);
    int32 nNumberOfItems = ptItemAppGlobals->nItems;
    for (int32 nItemIdx = 0; nItemIdx < nNumberOfItems; nItemIdx++)
    {
        const Item & tItem = ptItemAppGlobals->ptItems[nItemIdx];
        ScopedItemDefinitionLockR pdefItem(tItem.gbid);
        if ((tItem.dwFlags & dwDisallowedItemFlags) == 0 && pdefItem != NULL && pdefItem->snoRandomItemTreasureClass == SNO_NONE)
        {
            if (ItemGetSet(tItem.gbid) == gbidSet)
                nMemberItems++;
        }
    }
    return nMemberItems;
}




















BOOL ItemIsEquippableByPlayerOrHireling(const ActorCommonData & tACDItem)
{
    if (!tACDItem.IsItem())
        return FALSE;
    return ItemIsEquippableByPlayerOrHireling(tACDItem.hGB.gbid);
}






BOOL ItemIsEquippableByPlayerOrHireling(GBID gbidItem)
{
    return ItemHasLabel(gbidItem, ITL_FOLLOWER_SPECIAL) || ItemIsEquippable(gbidItem);
}



BOOL ItemIsEquippable(const ActorCommonData & tACDItem)
{
    if (!tACDItem.IsItem())
        return FALSE;
    return ItemIsEquippable(tACDItem.hGB.gbid);
}






BOOL ItemIsEquippable(GBID gbidItem)
{
    BOOL bEquippable = FALSE;
    ScopedItemDefinitionLockR pdefItem(gbidItem);
    if (pdefItem != NULL)
    {



        ScopedItemTypeDefinitonLockR pItemTypeDef(pdefItem->gbidItemType);
        if (pItemTypeDef != NULL)
        {






            for (int32 nIndex = 0; cgtInventorySlotsThatTakeDamage[nIndex] != SLOT_NONE; nIndex++)
            {
                if (ItemIsAllowedInSlot(cgtInventorySlotsThatTakeDamage[nIndex], pItemTypeDef))
                {
                    bEquippable = TRUE;
                    break;
                }
            }
        }
    }

    return bEquippable;
}


GBID ItemGetHighestAffixWithEffectType(const ActorCommonData & tACD, const BOOL bForTooltip)
{


    int32 nHighestEffectLevel = -1;
    GBID gbidHighestAffix = GBID_NONE;

    GBID gbidEnchantedAffixOld = ItemGetEnchantedAffixOld(tACD);

    POS pos = tACD.listIdentifiedAffixGBIDs.GetHeadPosition();
    while (pos)
    {
        const GameBalanceDefinition * pdefGBAffixes = NULL;
        GBHandle hGB(GB_AFFIXES, tACD.listIdentifiedAffixGBIDs.GetNext(pos));


        if (hGB.gbid == gbidEnchantedAffixOld)
        {
            GBID gbidEnchantedAffixNew = ItemGetEnchantedAffixNew(tACD);
            if (gbidEnchantedAffixNew == GBID_NONE)
            {
                continue;
            }
            hGB.gbid = gbidEnchantedAffixNew;
        }

        const AffixTableEntry * ptAffix = (const AffixTableEntry *)GBLockR(hGB, pdefGBAffixes, TRUE);
        if (ptAffix != NULL)
        {
            int32 nCurEffectLevel = ptAffix->nItemEffectLevel;
            ItemEffectType eCurEffectType = ptAffix->eItemEffectType;

            if (!bForTooltip)
            {
                if (nCurEffectLevel > nHighestEffectLevel &&
                    eCurEffectType != ITEM_EFFECT_TYPE_NONE)
                {

                    nHighestEffectLevel = nCurEffectLevel;
                    gbidHighestAffix = ptAffix->tHeader.gbid;
                }
            }
            else
            {


                switch (eCurEffectType)
                {
                case ITEM_EFFECT_TYPE_LIGHTNING: case ITEM_EFFECT_TYPE_COLD:
                case ITEM_EFFECT_TYPE_FIRE: case ITEM_EFFECT_TYPE_POISON:
                case ITEM_EFFECT_TYPE_ARCANE: case ITEM_EFFECT_TYPE_HOLY:


                    if (nCurEffectLevel > nHighestEffectLevel)
                    {
                        nHighestEffectLevel = nCurEffectLevel;
                        gbidHighestAffix = ptAffix->tHeader.gbid;
                    }
                    break;
                }
            }
        }
        if (pdefGBAffixes != NULL)
        {
            GBUnlockR(pdefGBAffixes);
        }
    }

    return gbidHighestAffix;
}


BOOL ItemIsGem(const ActorCommonData & tACDItem)
{
    if (!tACDItem.IsItem())
        return FALSE;
    return ItemIsGem(tACDItem.hGB.gbid);
}






BOOL ItemIsGem(GBID gbidItem)
{
    return ItemHasLabel(gbidItem, ITL_GEM);
}



BOOL ItemIsJewel(const ActorCommonData & tACDItem)
{
    if (!tACDItem.IsItem())
        return FALSE;
    return ItemIsJewel(tACDItem.hGB.gbid);
}






BOOL ItemIsJewel(GBID gbidItem)
{
    return ItemHasLabel(gbidItem, ITL_JEWEL);
}



BOOL ItemIsUpgradeableJewel(
    const ActorCommonData & tACDItem)
{
    if (!tACDItem.IsItem())
        return FALSE;



    return ItemIsUpgradeableJewel(tACDItem.hGB.gbid);
}


BOOL ItemIsUpgradeableJewel(
    GBID gbidItem)
{
    return ItemHasLabel(gbidItem, ITL_UPGRADEABLE_JEWEL);
}


BOOL ItemIsShard(const ActorCommonData & tACDItem)
{
    if (!tACDItem.IsItem())
        return FALSE;
    return ItemIsShard(tACDItem.hGB.gbid);
}





BOOL ItemIsShard(GBID gbidItem)
{
    return ItemHasLabel(gbidItem, ITL_SHARD) || ItemHasLabel(gbidItem, ITL_GREATERSHARD);
}



BOOL ItemIsCraftingReagent(const ActorCommonData & tACDItem)
{
    if (!tACDItem.IsItem())
        return FALSE;
    return ItemIsCraftingReagent(tACDItem.hGB.gbid);
}






BOOL ItemIsCraftingReagent(GBID gbidItem)
{
    return ItemHasLabel(gbidItem, ITL_CRAFTING);
}



BOOL ItemIsHoradricReagent(const ActorCommonData & tACDItem)
{
    if (!tACDItem.IsItem())
        return FALSE;
    return ItemIsHoradricReagent(tACDItem.hGB.gbid);
}






BOOL ItemIsHoradricReagent(GBID gbidItem)
{
    return ItemHasLabel(gbidItem, ITL_HORADRIC_REAGENT);
}















BOOL ItemIsActBountyReagent(GBID gbidItem)
{
    if (gbidItem == GlobalGBIDGet(GLOBALGBID_ITEM_ACT_BOUNTY_REAGENT_1) ||
        gbidItem == GlobalGBIDGet(GLOBALGBID_ITEM_ACT_BOUNTY_REAGENT_2) ||
        gbidItem == GlobalGBIDGet(GLOBALGBID_ITEM_ACT_BOUNTY_REAGENT_3) ||
        gbidItem == GlobalGBIDGet(GLOBALGBID_ITEM_ACT_BOUNTY_REAGENT_4) ||
        gbidItem == GlobalGBIDGet(GLOBALGBID_ITEM_ACT_BOUNTY_REAGENT_5))
    {
        return TRUE;
    }
    return FALSE;
}















BOOL ItemIsGemConversionReagent(GBID gbidItem)
{
    if (gbidItem == GlobalGBIDGet(GLOBALGBID_ITEM_GEM_CONVERT_AMETHYST) ||
        gbidItem == GlobalGBIDGet(GLOBALGBID_ITEM_GEM_CONVERT_DIAMOND) ||
        gbidItem == GlobalGBIDGet(GLOBALGBID_ITEM_GEM_CONVERT_EMERALD) ||
        gbidItem == GlobalGBIDGet(GLOBALGBID_ITEM_GEM_CONVERT_RUBY) ||
        gbidItem == GlobalGBIDGet(GLOBALGBID_ITEM_GEM_CONVERT_TOPAZ))
    {
        return TRUE;
    }
    return FALSE;
}






















CRefString ItemGetItemQualityDescription(const ItemQualityLevel eIQL, const AncientRank eAncientRank, const BOOL bSeasonOnly, const BOOL bSanctified)
{
    SNO snoStringListItemQuality = GlobalSNOGet(GLOBALSNO_STRINGLIST_ITEM_QUALITY);
    if (bSanctified)
        return StringListFetch(snoStringListItemQuality, "Sanctified");
    if (bSeasonOnly)
    {
        if (eAncientRank == ANCIENT_RANK_ANCIENT)
            return StringListFetch(snoStringListItemQuality, "AncientSeasonOnly");
        return StringListFetch(snoStringListItemQuality, "SeasonOnly");
    }
    switch (eIQL)
    {
    case IQL_INVALID:
        if (eAncientRank == ANCIENT_RANK_ANCIENT)
            return StringListFetch(snoStringListItemQuality, "Ancient");
        else if (eAncientRank == ANCIENT_RANK_PRIMAL_ANCIENT)
            return StringListFetch(snoStringListItemQuality, "PrimalAncient");
        break;
    case IQL_INFERIOR:
        return StringListFetch(snoStringListItemQuality, "Inferior");
    case IQL_SUPERIOR:
        return StringListFetch(snoStringListItemQuality, "Superior");
    case IQL_MAGIC_1_AFFIX:
    case IQL_MAGIC_2_AFFIX:
    case IQL_MAGIC_3_AFFIX:
    case IQL_SPECIAL:
        return StringListFetch(snoStringListItemQuality, "Magic");
    case IQL_LEGENDARY:
        if (eAncientRank == ANCIENT_RANK_ANCIENT)
            return StringListFetch(snoStringListItemQuality, "AncientLegendary");
        else if (eAncientRank == ANCIENT_RANK_PRIMAL_ANCIENT)
            return StringListFetch(snoStringListItemQuality, "PrimalAncientLegendary");
        else
            return StringListFetch(snoStringListItemQuality, "Legendary");
    case IQL_DISPLAY_AS_SET_ITEM:
        if (eAncientRank == ANCIENT_RANK_ANCIENT)
            return StringListFetch(snoStringListItemQuality, "AncientSet");
        else if (eAncientRank == ANCIENT_RANK_PRIMAL_ANCIENT)
            return StringListFetch(snoStringListItemQuality, "PrimalAncientSet");
        else
            return StringListFetch(snoStringListItemQuality, "Set");
    default:
        if (eIQL >= IQL_RARE_AFFIX_START && eIQL <= IQL_RARE_AFFIX_END)
            return StringListFetch(snoStringListItemQuality, "Rare");
        break;
    }
    return "";
}





CRefString sGetItemTypeString(GBID gbidItemType)
{
    CRefString sItemType;
    if (StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_ITEM_TYPES), GBIDToString(gbidItemType), sItemType))
        return sItemType;
    return "";
}

static CRefString sBuildItemTypeDescription(CRefString & sItemQuality, CRefString & sItemType)
{
    CRefString sItemTypeName, sName2Ignored;
    uint32 dwGrammaticalForm;
    LanguageRules::ExtractGrammaticalForm(sItemType, sItemTypeName, sName2Ignored, dwGrammaticalForm);
    if (sItemQuality.Length())
    {
        uint32 dwExtractionFlags;
        LanguageRules::ExtractMatchingGrammaticalForm(sItemQuality, dwGrammaticalForm, dwExtractionFlags);
        if ((dwGrammaticalForm & (1 << GF_CONCATENATE_BIT)) && (dwExtractionFlags & (1 << GFE_CAN_CONCATENATE_BIT)))
            LanguageRules::HandleConcatenation(sItemQuality, sItemType);
    }
    return LanguageRules::BuildItemName(sItemQuality, sItemTypeName, "", INC_ITEM_TYPE, FALSE);
}

// 2.7.7 helpers (unnamed): the item definition's ITEM_ETHEREAL_BIT ("SeasonOnly" quality text) and the
// ITEM_WAS_SANCTIFIED attribute ("Sanctified" quality text); both are inlined into ItemTypeGetDescription.
extern "C" BOOL sub_7100505c00(const ActorCommonData & tACDItem);
extern "C" BOOL sub_7100505c90(const ActorCommonData & tACDItem);

CRefString ItemTypeGetDescription(const ActorCommonData & tACDItem)
{
    GBHandle hGBItem = tACDItem.hGB;
    GBHandle hGBItemType = ItemsGetItemTypeGB(hGBItem);
    if (hGBItem.gbid == GlobalGBIDGet(GLOBALGBID_ITEM_DEATHS_BREATH))
    {
        CRefString sItemType = sGetItemTypeString(hGBItemType.gbid);
        CRefString sItemQuality = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_UI_TOOLTIPS), "Unique");
        return sBuildItemTypeDescription(sItemQuality, sItemType);
    }
    else
    {
        ItemQualityLevel eIQL = ItemGetDisplayedQualityLevel(tACDItem);
        AncientRank eAncientRank = ItemIsUnidentified(tACDItem) ? ANCIENT_RANK_NONE : ItemGetAncientRank(tACDItem);
        return ItemTypeGetDescription(hGBItemType.gbid, eIQL, eAncientRank, sub_7100505c00(tACDItem), sub_7100505c90(tACDItem));
    }
}

extern "C" BOOL sub_7100505c00(const ActorCommonData & tACDItem)
{
    if (tACDItem.hGB.eType != GB_ITEMS)
        return FALSE;
    ScopedItemDefinitionLockR ptItem(tACDItem.hGB.gbid);
    if (ptItem == NULL)
        return FALSE;
    return (ptItem->dwFlags >> ITEM_ETHEREAL_BIT) & 1;
}

extern "C" BOOL sub_7100505c90(const ActorCommonData & tACDItem)
{
    return tACDItem.AttributesGetInt(ATTRIBUTE_ITEM_WAS_SANCTIFIED) != 0;
}

CRefString ItemTypeGetDescription(const GBID gbidItemType, const ItemQualityLevel eIQL, const AncientRank eAncientRank, const BOOL bSeasonOnly, const BOOL bSanctified)
{
    CRefString sItemType = sGetItemTypeString(gbidItemType);
    if (!sItemType.Length())
        return "";
    CRefString sItemQuality = ItemGetItemQualityDescription(eIQL, eAncientRank, bSeasonOnly, bSanctified);
    return sBuildItemTypeDescription(sItemQuality, sItemType);
}



























CRefString ItemGetFlavorText(const GBID gbidItem)
{
    CRefString usRetval;
    ScopedItemDefinitionLockR ptItem(gbidItem);
    if (ptItem != NULL)
    {
        BOOL fSuccess = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_ITEM_FLAVOR), ptItem->tHeader.szName, usRetval);
        if (!fSuccess)
            fSuccess = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_BNETSTORE_ITEMDESCRIPTIONS), ptItem->tHeader.szName, usRetval);
        if (fSuccess)
            Set_Text_Color(usRetval, FONTCOLOR_GOLD);
        else
            usRetval = "";
        if (NephalemRift::IsRiftKey(gbidItem))
        {
            usRetval = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_ITEM_FLAVOR), "TieredLootRunKey"), NULL);
            Set_Text_Color(usRetval, FONTCOLOR_GOLD);
        }
    }
    return usRetval;
}





















CRefString ItemGetFlavorText(const ActorCommonData & tACDItem)
{
    CRefString usRetval;
    if (ItemIsUnidentified(tACDItem))
        return usRetval;
    return ItemGetFlavorText(tACDItem.hGB.gbid);
}









CRefString ItemGetClassRestrictionDescription(const ActorCommonData & tACDItem,
                                              const PlayerClass ePlayerClass)

{
    const GameBalanceDefinition * pdefGBItemTypes;

    ItemTypeDefinition * pItemTypeDef = ItemsItemTypeDefLockR(tACDItem.hGB.gbid, pdefGBItemTypes);

    uint32 dwFlags = pItemTypeDef->dwFlags;
    GBUnlockR(pdefGBItemTypes);

    struct BitClassMapping
    {
        int32 nBit;
        PlayerClass eClass;
    };

    BitClassMapping tBitsToTest[] =
    {
        { IT_USABLE_BARBARIAN_BIT, PLAYER_CLASS_BARBARIAN },
        { IT_USABLE_WIZARD_BIT, PLAYER_CLASS_WIZARD },
        { IT_USABLE_WITCHDOCTOR_BIT, PLAYER_CLASS_WITCHDOCTOR },
        { IT_USABLE_DEMONHUNTER_BIT, PLAYER_CLASS_DEMONHUNTER },
        { IT_USABLE_MONK_BIT, PLAYER_CLASS_MONK },
        { IT_USABLE_CRUSADER_BIT, PLAYER_CLASS_CRUSADER },
        { IT_USABLE_NECROMANCER_BIT, PLAYER_CLASS_NECROMANCER },
    };

    PlayerClass eClass = PLAYER_CLASS_NONE;
    for (int32 i = 0; i < 7; i++)
    {
        if (dwFlags & (1 << tBitsToTest[i].nBit))
        {
            if (eClass != PLAYER_CLASS_NONE)
            {
                return CRefString("");
            }
            eClass = tBitsToTest[i].eClass;
        }
    }

    if (eClass != PLAYER_CLASS_NONE)
    {
        CRefString sClassName = PlayerGetClassUIName(eClass, TRUE);
        CRefString sRestriction = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_UI_TOOLTIPS), "ClassRestriction").str(),
                                                   sClassName.str(), NULL);
        if (!ItemMeetsOwnerClassRestrictions(tACDItem.hGB.gbid, ePlayerClass, FALSE))
        {

            Set_Text_Color(sRestriction, FONTCOLOR_RED);
        }

        return sRestriction;
    }

    return CRefString("");
}


































































































































































































void ItemsValidateCachedAffixTableEntries()
{
    ItemAppGlobals * ptItemAppGlobals = sgtAppGlobals.ptItemAppGlobals;
    if (ptItemAppGlobals->bCachedAffixTableEntriesValid)
        return;
    for (int32 i = 0; i < ptItemAppGlobals->nAffixes; i++)
    {
        GBHandle hGB(GB_AFFIXES, ptItemAppGlobals->ptAffixes[i].gbid);
        const GameBalanceDefinition * ptDef;
        ptItemAppGlobals->ptCachedAffixTableEntyPtrs[i] = (AffixTableEntry *)GBLockR(hGB, ptDef, TRUE);
        GBUnlockR(ptDef);
    }
    ptItemAppGlobals->bCachedAffixTableEntriesValid = TRUE;
}






































ACDID ItemGetAssignedACDID(const ActorCommonData & tACDItem, BOOL bUnk)
{
    const InventoryLocation * ptInventoryLoc = tACDItem.TryToGetInventoryLocation();
    if (ptInventoryLoc != NULL)
    {
        const InventoryLocation * ptUltimateInventoryLoc;
        do
        {
            ptUltimateInventoryLoc = ptInventoryLoc;
            if (ptUltimateInventoryLoc->idACDContainer == ACDID_INVALID)
                break;
            ActorCommonData * ptACDContainer = ACDTryToGet(ptUltimateInventoryLoc->idACDContainer);
            if (ptACDContainer == NULL)
                break;
            ptInventoryLoc = ptACDContainer->TryToGetInventoryLocation();
        } while (ptInventoryLoc != NULL);
        return ptUltimateInventoryLoc->idACDContainer;
    }
    ACDID idACDBoundTo = ItemGetBoundToACD(tACDItem, FALSE);
    if (idACDBoundTo != ACDID_INVALID)
        return idACDBoundTo;
    if (tACDItem.HasAssignedHeroID())
    {
        for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer != NULL; ptPlayer = PlayerGetNextInGame(ptPlayer))
        {
            if (tACDItem.DoesPlayerMatchAssignedHeroID(ptPlayer, bUnk))
                return ptPlayer->idPlayerACD;
        }
    }
    return ACDID_INVALID;
}




















void ItemComputeConsoleItemId(const ActorCommonData & tItemACD, D3::OnlineService::ItemId & tId)
{
    tId.set_id_high(1);
    tId.set_id_low(tItemACD.ann);
}







uint32 ItemGetTradeTimeLeft(const ActorCommonData & tACDItem)
{
    uint32 uTradeEndTime = tACDItem.AttributesGetInt(ATTRIBUTE_ITEM_TRADE_END_TIME);
    uint32 uNow = GameActiveGameCommonDataIsServer() ? GameGetServerTime() : CGameGetPredictedServerTime();
    uint32 uTimeLeft = 0;
    if (uTradeEndTime > uNow)
        uTimeLeft = uTradeEndTime - uNow;
    return uTimeLeft;
}



















CRefString UIText_IconFromCurrency(CurrencyType eCurrencyType)
{
    switch (eCurrencyType)
    {
    case CURRENCY_GOLD:
        return "{icon:gold}";
    case CURRENCY_BLOODSHARDS:
        return "{icon:x1_shard}";
    case CURRENCY_PLATINUM:
        return "{icon:BattleNetStore_CurrencyIcon}";
    default:
        {
            GBID gbidItem = CurrencyGetGBID(eCurrencyType);
            if (gbidItem != GBID_NONE)
            {
                ImageHandle hImage = ACDInventoryGetImageHandleFromGBID(gbidItem, 0, 1, TRUE);
                if (hImage != 0)
                {
                    CRefString sIcon;
                    return sIcon.Format("{item:%08x,1.5}", hImage);
                }
            }
        }
        return "";
    }
}










CRefString UIText_ValueToGameCurrency(const uint64 uAmount, CurrencyType eCurrencyType)
{
    CRefString sValue = UnicodeText::FormatNumber((int64)uAmount, TRUE, FNS_NORMAL, XLOCALE_INVALID);
    return sValue + " " + UIText_IconFromCurrency(eCurrencyType);
}
















GBID CurrencyGetGBID(CurrencyType eCurrencyType)
{
    ItemAppGlobals & tItemAppGlobals = *sgtAppGlobals.ptItemAppGlobals;
    blz::unordered_map<int, CurrencyInfo>::const_iterator it = tItemAppGlobals.tCurrencyToCurrencyInfoMap.find(eCurrencyType);
    if (it == tItemAppGlobals.tCurrencyToCurrencyInfoMap.end())
        return GBID_NONE;
    return it->second.gbidItem;
}







CurrencyType CurrencyFromGBID(GBID gbid)
{
    ItemAppGlobals & tItemAppGlobals = *sgtAppGlobals.ptItemAppGlobals;
    blz::unordered_map<int, int>::const_iterator it = tItemAppGlobals.tGBIDToCurrencyMap.find(gbid);
    if (it == tItemAppGlobals.tGBIDToCurrencyMap.end())
        return CURRENCY_INVALID;
    return it->second;
}








int64 CurrencyDataGetCount(const CurrencyType eCurrencyType, const D3::Items::CurrencySavedData & data)
{
    for (int i = 0; i < data.currency_size(); i++)
    {
        if (data.currency(i).id() == eCurrencyType)
            return data.currency(i).count();
    }
    return 0;
}








void CurrencyDataSetCount(const CurrencyType eCurrencyType, D3::Items::CurrencySavedData & data, int64 count)
{
    for (int i = 0; i < data.currency_size(); i++)
    {
        if (data.currency(i).id() == eCurrencyType)
        {
            data.mutable_currency(i)->set_count(count);
            return;
        }
    }
    D3::Items::CurrencyData * added = data.add_currency();
    added->set_id(eCurrencyType);
    added->set_count(count);
}







int32 ItemCompareMatchingLegendaryPowers(GBID gbidLegendary1, GBID gbidLegendary2)
{
    ScopedItemDefinitionLockR pdefLegendary1(gbidLegendary1);
    ScopedItemDefinitionLockR pdefLegendary2(gbidLegendary2);
    if (pdefLegendary1->nItemLevel > pdefLegendary2->nItemLevel)
        return -1;
    if (pdefLegendary1->nItemLevel < pdefLegendary2->nItemLevel)
        return 1;
    if (pdefLegendary1->tHeader.gbid > pdefLegendary2->tHeader.gbid)
        return -1;
    if (pdefLegendary1->tHeader.gbid < pdefLegendary2->tHeader.gbid)
        return 1;
    return 0;
}






SNO ItemGetLegendaryPower(const ItemDefinition * ptItemDef)
{
    if (ptItemDef->gbidLegendaryPowerItemReplacement != GBID_NONE)
    {
        ScopedItemDefinitionLockR pReplacementItemDef(ptItemDef->gbidLegendaryPowerItemReplacement);
        if (pReplacementItemDef != NULL)
            return ItemGetLegendaryPower(pReplacementItemDef);
    }
    for (int32 i = 0; i < (int32)(ptItemDef->serAttributes_Console.dwSize / sizeof(AttributeSpecifier)); i++)
    {
        if (ptItemDef->ptAttributes[i].eAttribute == ATTRIBUTE_ITEM_POWER_PASSIVE)
        {
            SNO snoPower = ptItemDef->ptAttributes[i].nParam;
            if (!SNOFileExists(POWER_SNO, snoPower))
                return SNO_NONE;
            return snoPower;
        }
    }
    CraftedLegendaryPowerInfo tInfo;
    if (ItemGetLegendaryPowerInfoFromCraftedItemGBID(ptItemDef->tHeader.gbid, tInfo))
        return tInfo.snoPower;
    return SNO_NONE;
}















SNO ItemGetLegendaryPower(
    GBID gbidItem)
{
    if (gbidItem == GBID_NONE)
        return SNO_NONE;



    ScopedItemDefinitionLockR pdefItem(gbidItem);
    if (pdefItem == NULL)
        return SNO_NONE;



    return ItemGetLegendaryPower(pdefItem);
}



BOOL ItemAttributePrefersMinimum(GBID gbidItem, Attrib eAttribute)
{
    if (eAttribute == ATTRIBUTE_AMPLIFYDAMAGE_TYPE_PERCENT)
        return TRUE;
    if (eAttribute == ATTRIBUTE_ITEM_POWER_PASSIVE)
    {
        ScopedItemDefinitionLockR pdefItem(gbidItem);
        if (pdefItem == NULL)
            return FALSE;
        return (pdefItem->dwFlags >> ITEM_FORCE_EXTRACT_POWER_MINIMUM) & 1;
    }
    return FALSE;
}


















float ItemGetLegendaryPowerValue(GBID gbidItem)
{
    if (gbidItem == GBID_NONE)
        return 0.0f;
    ScopedItemDefinitionLockR pdefItem(gbidItem);
    if (pdefItem == NULL)
        return 0.0f;
    BOOL bNeedMinimum = (pdefItem->dwFlags >> ITEM_FORCE_EXTRACT_POWER_MINIMUM) & 1;
    for (int32 i = 0; i < (int32)(pdefItem->serAttributes_Console.dwSize / sizeof(AttributeSpecifier)); i++)
    {
        const AttributeSpecifier & tAttribute = pdefItem->ptAttributes[i];
        if (tAttribute.eAttribute == ATTRIBUTE_ITEM_POWER_PASSIVE)
        {
            SNO snoPower = tAttribute.nParam;
            if (!SNOFileExists(POWER_SNO, snoPower))
                return 0.0f;
            RNG tRNG;
            RandSetSeed_Internal(tRNG, 0);
            float flValue;
            if (bNeedMinimum)
                flValue = AffixGetFormulaMinValue(tAttribute, NULL, gbidItem, &tRNG, NULL);
            else
                flValue = AffixGetFormulaMaxValue(tAttribute, NULL, gbidItem, &tRNG, NULL);
            return flValue;
        }
    }
    CraftedLegendaryPowerInfo tInfo;
    if (ItemGetLegendaryPowerInfoFromCraftedItemGBID(gbidItem, tInfo))
        return tInfo.flUsePowerValue;
    return 0.0f;
}



















BOOL ItemCanExtractLegendaryPower(GBID gbidItem)
{
    ScopedItemDefinitionLockR pdefItem(gbidItem);
    if (pdefItem == NULL)
        return FALSE;
    if ((pdefItem->dwFlags >> ITEM_CANNOT_EXTRACT_POWER) & 1)
        return FALSE;
    if (!SNOFileExists(ACTOR_SNO, pdefItem->snoActor))
        return FALSE;
    if (!ItemIsLegendary(gbidItem))
        return FALSE;
    if (!ItemIsEquippable(gbidItem))
        return FALSE;
    if (pdefItem->gbidLegendaryPowerItemReplacement != GBID_NONE)
        return FALSE;
    SNO snoPower = ItemGetLegendaryPower(pdefItem);
    return snoPower != SNO_NONE;
}
























BOOL ItemCanExtractLegendaryPowerOrLinkedPower(
    GBID gbidItem)
{
    if (ItemCanExtractLegendaryPower(gbidItem))
    {
        return TRUE;
    }

    ScopedItemDefinitionLockR pdefItem(gbidItem);
    if (pdefItem && pdefItem->gbidLegendaryPowerItemReplacement != GBID_NONE)
    {
        if (ItemCanExtractLegendaryPower(pdefItem->gbidLegendaryPowerItemReplacement))
        {
            return TRUE;
        }
    }

    return FALSE;
}



BOOL ItemGetLegendaryPowerInfoFromCraftedItemGBID(GBID gbid, CraftedLegendaryPowerInfo & tInfo)
{
    ItemAppGlobals & tItemAppGlobals = *sgtAppGlobals.ptItemAppGlobals;
    blz::unordered_map<int, CraftedLegendaryPowerInfo>::iterator it = tItemAppGlobals.tCraftedItemGBIDToPowerInfo.find(gbid);
    if (it == tItemAppGlobals.tCraftedItemGBIDToPowerInfo.end())
        return FALSE;
    tInfo = it->second;
    return TRUE;
}










void sAddItemToLegendaryPowersList(GBID gbidItem, SNO snoPower, blz::vector<int, blz::allocator<int> > & arExtractableLegendaryPowers)
{
    BOOL bExists = FALSE;
    size_t nExtractableLegendaryPowersCount = arExtractableLegendaryPowers.size();
    for (size_t i = 0; i < nExtractableLegendaryPowersCount; i++)
    {
        GBID gbidExistingItem = arExtractableLegendaryPowers[i];
        SNO snoExistingPower = ItemGetLegendaryPower(gbidExistingItem);
        if (snoExistingPower == snoPower)
        {
            bExists = TRUE;
            if (ItemCompareMatchingLegendaryPowers(gbidExistingItem, gbidItem) > 0)
                arExtractableLegendaryPowers[i] = gbidItem;
            break;
        }
    }
    if (!bExists)
        arExtractableLegendaryPowers.push_back(gbidItem);
}












void ItemsGetExtractableLegendaryPowers(blz::vector<int, blz::allocator<int> > & arExtractableLegendaryPowers)
{
    arExtractableLegendaryPowers.clear();
    GBHandleList listItems(GBGetHandlePool());
    GBEnumerate(GB_ITEMS, listItems);
    for (POS pos = listItems.GetHeadPosition(); pos != NULL; )
    {
        GBID gbidItem = listItems.GetNext(pos).gbid;
        if (ItemCanExtractLegendaryPower(gbidItem))
        {
            SNO snoPower = ItemGetLegendaryPower(gbidItem);
            sAddItemToLegendaryPowersList(gbidItem, snoPower, arExtractableLegendaryPowers);
        }
    }
}










BOOL ItemPlayerHasExtractedLegendaryPower(const Player & tPlayer, GBID gbidItem)
{
    if (!ItemCanExtractLegendaryPower(gbidItem))
        return FALSE;
    return tPlayer.tCrafterSavedData.tExtractedLegendariesData.Contains(gbidItem);
}








BOOL ItemPlayerHasExtractedLegendaryPowerOrLinkedPower(const Player & tPlayer, GBID gbidItem)
{
    bool bCanExtract = ItemCanExtractLegendaryPower(gbidItem) != FALSE;
    if (!bCanExtract)
    {
        ScopedItemDefinitionLockR pdefItem(gbidItem);
        if (pdefItem != NULL && pdefItem->gbidLegendaryPowerItemReplacement != GBID_NONE && ItemCanExtractLegendaryPower(pdefItem->gbidLegendaryPowerItemReplacement))
            gbidItem = pdefItem->gbidLegendaryPowerItemReplacement;
    }
    return tPlayer.tCrafterSavedData.tExtractedLegendariesData.Contains(gbidItem) != FALSE;
}

















// 2.7.7 only, unnamed: the slot test split out of ItemPlayerCanAssignLegendaryPower (PlayerCanChangeLoadoutEquipment and PlayerSendHeroStateLoadoutToClient call it for slots 0-3; slot 3 is the fourth Kanai's Cube slot).
// NON_MATCHING: 2.7.7 logic; the switch compares slot 3, 2, 1 where the original compares 1, 2, 3, so the case-3 block is placed differently (126 vs 128 insns).
extern "C" BOOL sub_7100506d10(const Player & tPlayer, GBID gbidItem, LegendaryPowerSlotType eSlotType)
{
    if (gbidItem == GBID_NONE)
        return TRUE;
    ActorCommonData * ptACDPlayer = ACDTryToGet(tPlayer.idPlayerACD);
    if (ptACDPlayer && ptACDPlayer->AttributesGetInt(ATTRIBUTE_KANAI_POWERS))
        return TRUE;
    switch (eSlotType)
    {
    case LPST_ARMOR:
        if (ItemHasLabel(gbidItem, ITL_WEAPON) || ItemHasLabel(gbidItem, ITL_OFFHAND))
            return FALSE;
        return ItemHasLabel(gbidItem, ITL_ARMOR) != 0;
    case LPST_JEWELRY:
        if (ItemHasLabel(gbidItem, ITL_WEAPON) || ItemHasLabel(gbidItem, ITL_OFFHAND) || ItemHasLabel(gbidItem, ITL_ARMOR))
            return FALSE;
        return ItemHasLabel(gbidItem, ITL_JEWELRY) != 0;
    case 3:  // 2.7.7 slot past LPST_JEWELRY (no name recovered)
        return ptACDPlayer && ptACDPlayer->AttributesGetInt(ATTRIBUTE_FOURTH_KANAIS_CUBE_SLOT) != 0;
    default:
        return ItemHasLabel(gbidItem, ITL_WEAPON) || ItemHasLabel(gbidItem, ITL_OFFHAND);
    }
}

BOOL ItemPlayerCanAssignLegendaryPower(const Player & tPlayer, GBID gbidItem, LegendaryPowerSlotType eSlotType)
{
    if (!ItemCanExtractLegendaryPower(gbidItem))
        return FALSE;
    PlayerClass ePlayerClass = PlayerGetPlayerClass(&tPlayer);
    if (!ItemMeetsOwnerClassRestrictions(gbidItem, ePlayerClass, FALSE))
        return FALSE;
    SNO snoPower = ItemGetLegendaryPower(gbidItem);
    PlayerClass eClassRestriction = PowerGetClassRestriction(AutoPowerDefinitionR(snoPower));
    if (eClassRestriction != PLAYER_CLASS_NONE && ePlayerClass != eClassRestriction)
        return FALSE;
    return sub_7100506d10(tPlayer, gbidItem, eSlotType) != 0;
}





















LegendaryPowerSlotType ItemGetLegendaryPowerSlot(GBID gbidItem)
{
    if (ItemHasLabel(gbidItem, ITL_WEAPON) || ItemHasLabel(gbidItem, ITL_OFFHAND))
        return LPST_WEAPON;
    if (ItemHasLabel(gbidItem, ITL_ARMOR))
        return LPST_ARMOR;
    if (ItemHasLabel(gbidItem, ITL_JEWELRY))
        return LPST_JEWELRY;
    return LPST_NONE;
}














bool ItemCompareExtractableLegendaryPowers(GBID gbidPower1, GBID gbidPower2)
{
    CRefString sGBIDItem1 = GBIDToString(gbidPower1, GB_ITEMS, FALSE);
    CRefString sLocalizedLegendaryName1 = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_ITEMS), sGBIDItem1.str());
    CRefString sGBIDItem2 = GBIDToString(gbidPower2, GB_ITEMS, FALSE);
    CRefString sLocalizedLegendaryName2 = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_ITEMS), sGBIDItem2.str());
    return strcmp(sLocalizedLegendaryName1.str(), sLocalizedLegendaryName2.str()) < 0;
}







struct FindUpgradeableJewelsCallbackData
{
    BOOL bExcludeUnidentified;
    ACDIDArray & aridItems;
    FindUpgradeableJewelsCallbackData(BOOL bExcludeUnidentified, ACDIDArray & aridItems)
        : bExcludeUnidentified(bExcludeUnidentified), aridItems(aridItems)
    {
    }
private:
    FindUpgradeableJewelsCallbackData(FindUpgradeableJewelsCallbackData &);
    FindUpgradeableJewelsCallbackData & operator=(FindUpgradeableJewelsCallbackData &);
};

static void sFindUpgradeableJewelsCallback(ACDID idACDSquare, InventorySlotType eSlot, void * pUserData)
{
    FindUpgradeableJewelsCallbackData * ptData = (FindUpgradeableJewelsCallbackData *)pUserData;
    ActorCommonData * ptACDItem = ACDTryToGet(idACDSquare);
    if (ptACDItem == NULL)
        return;

    if (!ItemIsUpgradeableJewel(*ptACDItem))
        return;
    if (ptData->bExcludeUnidentified && ItemIsUnidentified(*ptACDItem))
        return;

    ptData->aridItems.push_back(idACDSquare);
}










void ItemsFindUpgradeableJewels(const Player * ptPlayer, BOOL bExcludeUnidentified, ACDIDArray & aridItems)
{
    if (ptPlayer == NULL)
        return;

    ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
    if (ptACDPlayer == NULL)
        return;

    FindUpgradeableJewelsCallbackData tData(bExcludeUnidentified, aridItems);
    for (int32 i = SLOT_PLAYER_START; i < SLOT_PLAYER_BODYSLOT_END; i++)
        ACDInventoryIterateItems(*ptACDPlayer, i, 1 << ACDII_CONTAINED_ITEMS, sFindUpgradeableJewelsCallback, &tData);

    tData.bExcludeUnidentified = TRUE;
    ACDInventoryIterateItems(*ptACDPlayer, SLOT_PLAYER_SHARED_STASH, 1 << ACDII_CONTAINED_ITEMS, sFindUpgradeableJewelsCallback, &tData);

    ACDID idACDHireling = PlayerGetHireling(ptPlayer->idPlayerACD);
    ActorCommonData * ptACDHireling = ACDTryToGet(idACDHireling);
    if (ptACDHireling == NULL)
        return;

    for (int32 i = SLOT_HIRELING_START; i < SLOT_HIRELING_END; i++)
        ACDInventoryIterateItems(*ptACDHireling, i, 1 << ACDII_CONTAINED_ITEMS, sFindUpgradeableJewelsCallback, &tData);
}


















uint32 ItemGetRequiredAugmentJewelRankByGBID(GBID gbidAncientItem)
{
    if (ItemHasLabel(gbidAncientItem, ITL_WEAPON) || ItemHasLabel(gbidAncientItem, ITL_OFFHAND))
        return 30;
    if (ItemHasLabel(gbidAncientItem, ITL_ARMOR))
        return 50;
    if (ItemHasLabel(gbidAncientItem, ITL_JEWELRY))
        return 40;
    return 0;
}














uint32 ItemGetRequiredAugmentJewelRankBySlot(LegendaryPowerSlotType eSlot)
{
    switch (eSlot)
    {
    case LPST_WEAPON:
        return 30;
    case LPST_ARMOR:
        return 50;
    case LPST_JEWELRY:
        return 40;
    default:
        return 0;
    }
}




void ItemUpdateLegacyItem(D3::Items::Generator & tGenerator)
{
    const D3::GameBalance::Handle & tGBHandle = tGenerator.gb_handle(); if (tGBHandle.game_balance_type() == GB_ITEMS)
    {
        GBID gbidItem = tGBHandle.gbid();
        if (GBIDExists(gbidItem, GB_LEGACY_ITEM_CONVERSIONS))
        {
            ScopedLegacyItemConversionLockR pdefLegacyItemConversion(gbidItem);
            if (pdefLegacyItemConversion != NULL && pdefLegacyItemConversion->gbidOldItem == gbidItem && !pdefLegacyItemConversion->bConsoleIgnore)
            {
                tGenerator.mutable_gb_handle()->set_gbid(pdefLegacyItemConversion->gbidNewItem);
                uint32 uFlags = tGenerator.flags();
                uFlags |= (1 << IGF_MARK_DIRTY_ON_CREATE);
                tGenerator.set_flags(uFlags);
            }
        }
    }
}
























void ItemCapJewelRank(D3::Items::Generator & tGenerator)
{
}

























GBID ItemGetCosmeticFromItem(const ActorCommonData & tACDItem)
{
    GBID gbidItem = tACDItem.hGB.gbid;
    if (GBIDExists(gbidItem, GB_LEGACY_ITEM_CONVERSIONS))
    {
        ScopedLegacyItemConversionLockR pdefLegacyItemConversion(gbidItem);
        if (pdefLegacyItemConversion != NULL && pdefLegacyItemConversion->gbidOldItem == gbidItem)
            return pdefLegacyItemConversion->gbidNewItem;
    }
    return GBID_NONE;
}









BOOL ItemIsCosmeticItem(const GBID gbidItem)
{
    ItemUseType eUseType = ItemGetUseType(gbidItem);
    switch (eUseType)
    {
    case ITEM_USE_COLLECTORS_EDITION_BUFF:
    case ITEM_USE_LEARN_WINGS:
    case ITEM_USE_LEARN_PET:
    case ITEM_USE_LEARN_PORTRAIT_FRAME:
    case ITEM_USE_LEARN_PENNANT:
        return TRUE;
    default:
        break;
    }
    return CosmeticItems_GetCosmeticItemTypeFromGBID(gbidItem) != 0;
}








BOOL ItemIsNeverTrash(GBID gbidItem)
{
    ScopedItemDefinitionLockR ptItem(gbidItem);
    if (ptItem == NULL)
        return FALSE;
    return (ptItem->dwFlags >> ITEM_IS_NOT_TRASH_BIT) & 1;
}



static void sHashSockets32(ACDID idACDSquare, InventorySlotType, void * pUserData)
{
    if (idACDSquare == ACDID_INVALID)
        return;
    ActorCommonData * ptACDSocket = ACDTryToGet(idACDSquare);
    if (ptACDSocket != NULL)
    {
        uint32 * dwHash = (uint32 *)pUserData;
        *dwHash = Blizzard::Util::Hash32More(&ptACDSocket->hGB.gbid, sizeof(GBID), *dwHash);
    }
}



uint32 ItemGenerateHash32(const ActorCommonData & tACDItem, BOOL bIncludeSockets)
{
    uint32 dwHash = Blizzard::Util::Hash32More(&tACDItem.hGB.gbid, sizeof(GBID), 0);
    uint32 dwItemSeed = tACDItem.AttributesGetInt(ATTRIBUTE_SEED);
    dwHash = Blizzard::Util::Hash32More(&dwItemSeed, sizeof(dwItemSeed), dwHash);
    const GBIDPooledList & listAffixGBIDs = ItemIsUnidentified(tACDItem) ? tACDItem.listUnidentifiedAffixGBIDs : tACDItem.listIdentifiedAffixGBIDs;
    POS pos = listAffixGBIDs.GetHeadPosition();
    while (pos)
    {
        GBID gbidAffix = listAffixGBIDs.GetNext(pos);
        dwHash = Blizzard::Util::Hash32More(&gbidAffix, sizeof(gbidAffix), dwHash);
    }
    if (bIncludeSockets && ItemHasSockets(tACDItem))
        ACDInventoryIterateItems(tACDItem, SLOT_SOCKETS_START, 0, sHashSockets32, &dwHash);
    return dwHash;
}














static void sHashSockets64(ACDID idACDSquare, InventorySlotType, void * pUserData)
{
    if (idACDSquare == ACDID_INVALID)
        return;
    ActorCommonData * ptACDSocket = ACDTryToGet(idACDSquare);
    if (ptACDSocket != NULL)
    {
        uint64 * dwHash = (uint64 *)pUserData;
        *dwHash = Blizzard::Util::Hash64More(&ptACDSocket->hGB.gbid, sizeof(GBID), *dwHash);
    }
}



uint64 ItemGenerateHash64(const ActorCommonData & tACDItem, BOOL bIncludeSockets)
{
    uint64 dwHash = Blizzard::Util::Hash64More(&tACDItem.hGB.gbid, sizeof(GBID), 0);
    uint32 dwItemSeed = tACDItem.AttributesGetInt(ATTRIBUTE_SEED);
    dwHash = Blizzard::Util::Hash64More(&dwItemSeed, sizeof(dwItemSeed), dwHash);
    const GBIDPooledList & listAffixGBIDs = ItemIsUnidentified(tACDItem) ? tACDItem.listUnidentifiedAffixGBIDs : tACDItem.listIdentifiedAffixGBIDs;
    POS pos = listAffixGBIDs.GetHeadPosition();
    while (pos)
    {
        GBID gbidAffix = listAffixGBIDs.GetNext(pos);
        dwHash = Blizzard::Util::Hash64More(&gbidAffix, sizeof(gbidAffix), dwHash);
    }
    if (bIncludeSockets && ItemHasSockets(tACDItem))
        ACDInventoryIterateItems(tACDItem, SLOT_SOCKETS_START, 0, sHashSockets64, &dwHash);
    return dwHash;
}














uint64 ItemGenerateConsoleEquipmentManagerID(const ActorCommonData & tACDItem)
{
    uint32 dwItemHash = ItemGenerateHash32(tACDItem, FALSE);
    uint64 uFakeItemId = ((uint64)tACDItem.hGB.gbid << 32) | dwItemHash;
    return uFakeItemId;
}






BasicAttribTooltipInfo ctArmorAttribTooltipInfo(StringListGetLabelHandle("Armor_Item_Total"), ATTRIBUTE_ARMOR_ITEM_TOTAL, ATTRIBUTE_NONE, ATTRIBUTE_NONE, ITL_INVALID);
BasicAttribTooltipInfo ctDPSAttribTooltipInfo(StringListGetLabelHandle("Damage_Per_Second"), ATTRIBUTE_ATTACKS_PER_SECOND_ITEM_TOTAL, ATTRIBUTE_DAMAGE_WEAPON_MIN_TOTAL_ALL, ATTRIBUTE_DAMAGE_WEAPON_DELTA_TOTAL_ALL, ITL_WEAPON);
BasicAttribTooltipInfo ctBlockChanceAttribTooltipInfo(StringListGetLabelHandle("Block_Chance_Item_Total"), ATTRIBUTE_BLOCK_CHANCE_ITEM_TOTAL, ATTRIBUTE_NONE, ATTRIBUTE_NONE, ITL_SHIELD);
BasicAttribTooltipInfo ctBlockAmountAttribTooltipInfo(StringListGetLabelHandle("Block_Amount_Item (pair)"), ATTRIBUTE_BLOCK_AMOUNT_ITEM_MIN, ATTRIBUTE_BLOCK_AMOUNT_ITEM_DELTA, ATTRIBUTE_NONE, ITL_SHIELD);
BasicAttribTooltipInfo ctDamageAttribTooltipInfo(StringListGetLabelHandle("WeaponDamage#Physical (pair)"), ATTRIBUTE_DAMAGE_WEAPON_MIN_TOTAL_ALL, ATTRIBUTE_DAMAGE_WEAPON_MAX_TOTAL_ALL, ATTRIBUTE_NONE, ITL_WEAPON);
BasicAttribTooltipInfo ctAPSAttribTooltipInfo(StringListGetLabelHandle("Attacks_Per_Second_Item_Total"), ATTRIBUTE_ATTACKS_PER_SECOND_ITEM_TOTAL, ATTRIBUTE_NONE, ATTRIBUTE_NONE, ITL_WEAPON);




CRefString sItemGetDescription(const GBID gbidItem)
{
    CRefString usRetval;
    ScopedItemDefinitionLockR ptItem(gbidItem);
    if (ptItem != NULL)
    {
        if (!StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_ITEM_DESCRIPTIONS), ptItem->tHeader.szName, usRetval))
            usRetval = "";
        if (NephalemRift::IsRiftKey(gbidItem))
            usRetval = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_ITEM_DESCRIPTIONS), "GreaterLootRunKey");
    }
    return usRetval;
}















CRefString ItemGetPrimaryDescription(const ActorCommonData & tACD)
{
    CRefString usFullDesc("");
    if (ItemGetUseType(tACD) == ITEM_USE_HEALTHPOTION)
    {
        ScopedGlobalDefinitionLockR pdefGlobal;
        float flHPGranted = pdefGlobal->flHealthPotionHealPercent;
        CRefString sVal = UnicodeText::FormatNumber(flHPGranted, TRUE, FNS_PERCENT, 0, 0, XLOCALE_INVALID);
        CRefString usHealthPotionString = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_UI_TOOLTIPS), "HealthPotion").str(), sVal.str(), NULL);
        usFullDesc += usHealthPotionString;
    }
    else
    {
        usFullDesc += sItemGetDescription(tACD.hGB.gbid);
    }
    return usFullDesc;
}













CRefString sGetSingleAttributeDescription(const ActorCommonData & tACD,
                                          const PlayerClass ePlayerClass,
                                          const AttribTooltipInfo & tAttribTooltipInfo,
                                          ItemDescriptionColorOverride pfnColorOverride,
                                          void * ptColorOverrideData)

{
    return GetAttributeDescription(tAttribTooltipInfo, tACD, tACD.idFastAttrib, ePlayerClass, pfnColorOverride ? TTVC_NORMAL_COLORING : TTVC_NO_COLORING, pfnColorOverride, ptColorOverrideData);
}




CRefString ItemGetArmorTotalDescription(const ActorCommonData & tACD,
                                        const PlayerClass ePlayerClass,
                                        ItemDescriptionColorOverride pfnColorOverride,
                                        void * ptColorOverrideData)

{
    return sGetSingleAttributeDescription(tACD, ePlayerClass, ctArmorAttribTooltipInfo, pfnColorOverride, ptColorOverrideData);
}



static BOOL sCheckAllAttribs(const AttribTooltipInfo & tInfo,
                             AttribGroupID idFastAttrib)

{
    if (idFastAttrib == ATTRIBGROUPID_INVALID)
        return FALSE;

    for (int32 i = 0; i < 3; i++)
    {
        const FastAttribKey & tKey = tInfo.arKey[i];

        if (KeyGetAttrib(tKey) != KeyGetAttrib(FastAttribKey(ATTRIBUTE_NONE)) && !FastAttribExists(idFastAttrib, tKey))
            return FALSE;
    }

    return TRUE;
}






static int32 sGetKeyIdxFromStringTag(LPCSTR szTag)
{
    int32 nKeyIdx = -1;

    if (strcasecmp(szTag, "VALUE") == 0 || strcasecmp(szTag, "VALUE1") == 0)
    {
        nKeyIdx = 0;
    }
    else if (strcasecmp(szTag, "VALUE2") == 0)
    {
        nKeyIdx = 1;
    }
    else if (strcasecmp(szTag, "VALUE3") == 0)
    {
        nKeyIdx = 2;
    }
    else if (strcasecmp(szTag, "VALUE4") == 0)
    {
        nKeyIdx = 3;
    }

    return nKeyIdx;
}




static void sAttribValueForValueTag(AttribGroupID idFastAttrib,
                                    const FastAttribKey & tKey,
                                    float & flResult,
                                    FontSpecifier & eFontColor)

{
    if (KeyGetDataType(tKey) == ATTRIBUTEDT_INT)
    {
        flResult = (float)FastAttribGetValueInt(idFastAttrib, tKey);
    }
    else
    {
        flResult = FastAttribGetValueFloat(idFastAttrib, tKey);
    }

    if (!FastAttribGetValueInt(idFastAttrib, ATTRIBUTE_HIDE_AFFIXES))
    {

        int32 nParam = KeyGetParam(tKey);
        Attrib eAttrib = KeyGetAttrib(tKey);
        for (int32 i = 0; i < scgNumBasicDisplayItems; i++)
        {
            if (ctBasicDisplayItems[i].eAttrib == eAttrib)
            {
                float flBaseValue;
                FastAttribKey tBaseKey = MakeAttribKey(ctBasicDisplayItems[i].eBaseAttrib, nParam);

                if (KeyGetDataType(tKey) == ATTRIBUTEDT_INT)
                {
                    flBaseValue = (float)FastAttribGetValueInt(idFastAttrib, tBaseKey);
                }
                else
                {
                    flBaseValue = FastAttribGetValueFloat(idFastAttrib, tBaseKey);
                }

                if (flBaseValue > flResult + 0.000001f)
                {
                    eFontColor = FONTCOLOR_CURSE;
                }
                else if (flBaseValue < flResult - 0.000001f)
                {
                    eFontColor = FONTCOLOR_BONUS;
                }

                break;
            }
        }
    }
}

static BOOL sResolveValueTag(const ActorCommonData * ptACD,
                             AttribGroupID idFastAttrib,
                             AttribGroupID idFastAttribMax,
                             LPCSTR szTag,
                             float & flResult,
                             float * pflResultMax,
                             CRefString &,
                             SNO snoPower,
                             int32 nLevel,
                             void * pData,
                             void *,
                             FontSpecifier & eFontColor)

{
    BOOL bMatch = FALSE;
    const AttribTooltipInfo * ptInfo = (const AttribTooltipInfo *)pData;
    if (ptInfo == NULL)
        return FALSE;



    int32 nKeyIdx = sGetKeyIdxFromStringTag(szTag);

    if (nKeyIdx >= 0)
    {
        const FastAttribKey & tKey = ptInfo->arKey[nKeyIdx];
        Attrib eAttrib = KeyGetAttrib(tKey);
        if (eAttrib == KeyGetAttrib(FastAttribKey(ATTRIBUTE_NONE)))
        {
            bMatch = FALSE;
        }
        else
        {






            sAttribValueForValueTag(idFastAttrib, tKey, flResult, eFontColor);
            if (idFastAttribMax != ATTRIBGROUPID_INVALID)
            {

                sAttribValueForValueTag(idFastAttribMax, tKey, *pflResultMax, eFontColor); bMatch = TRUE;
            }
            else bMatch = TRUE;
        }
    }

    else if (snoPower != SNO_NONE)
    {


        float flResultAfter = 0.0f;
        float flResultBefore = 0.0f;
        CRefString strResult;
        bMatch = UISkills::ResolvePowerTag(ptACD, idFastAttrib, ATTRIBGROUPID_INVALID, szTag, flResultAfter, NULL, strResult, snoPower, nLevel, pData, NULL, eFontColor);
        if (bMatch)
        {

            float flValueBefore[3] = { 0.0f, 0.0f, 0.0f };


            for (int32 i = 0; i < 3; i++)
            {
                const FastAttribKey & tKey = ptInfo->arKey[i]; if (KeyGetAttrib(tKey) != KeyGetAttrib(FastAttribKey(ATTRIBUTE_NONE)))
                {
                    flValueBefore[i] = FastAttribGetValueFloat(idFastAttrib, tKey);
                    FastAttribSetValueFloat(idFastAttrib, tKey, 0.0f);
                }
            }

            UISkills::ResolvePowerTag(ptACD, idFastAttrib, ATTRIBGROUPID_INVALID, szTag, flResultBefore, NULL, strResult, snoPower, nLevel, pData, NULL, eFontColor);


            for (int32 i = 0; i < 3; i++)
            {
                const FastAttribKey & tKey = ptInfo->arKey[i]; if (KeyGetAttrib(tKey) != KeyGetAttrib(FastAttribKey(ATTRIBUTE_NONE)))
                {
                    FastAttribSetValueFloat(idFastAttrib, tKey, flValueBefore[i]);
                }
            }

            flResult = flResultAfter - flResultBefore;
        }
    }


    return bMatch;
}



static CRefString sAppendResourceRestriction(const PlayerClass ePlayerClass, const AttribTooltipInfo & tInfo)


{
    if (ePlayerClass != PLAYER_CLASS_NONE &&
        KeyGetParamType(tInfo.arKey[0]) == APT_RESOURCE)
    {
        ResourceType eAttribResourceType = KeyGetParam(tInfo.arKey[0]);
        if (eAttribResourceType != RESOURCE_TYPE_INVALID && eAttribResourceType < RESOURCE_TYPE_COUNT)
        {

            CRefString usRestrictionString(SymbolToName(eAttribResourceType, gResourceTypes));
            usRestrictionString += "_Restriction";
            CRefString usRestriction = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_ATTRIBUTE_DESCRIPTIONS), usRestrictionString.str());

            GBID gbidHero = PlayerClassToGBID(ePlayerClass);
            const GameBalanceDefinition * pdefHeros;
            const HeroData * pHero = (const HeroData *)GBLockR(GBHandle(GB_HEROS, gbidHero), pdefHeros, TRUE);
            if (pHero)
            {
                ResourceType eOwnerResourceType = pHero->ePrimaryResourceType;
                ResourceType eOwnerSecondaryResourceType = pHero->eSecondaryResourceType;
                if (eOwnerResourceType != eAttribResourceType && eOwnerSecondaryResourceType != eAttribResourceType)
                {

                    Set_Text_Color(usRestriction, FONTCOLOR_RED);
                }
                GBUnlockR(pdefHeros);
            }

            return CRefString(" ") + usRestriction;
        }
    }

    return CRefString("");
}



CRefString GetAttributeDescription(const AttribTooltipInfo & tInfo,
                                   const ActorCommonData & tACDItem,
                                   const AttribGroupID idFastAttrib,
                                   const PlayerClass ePlayerClass,
                                   ToolTipValueColoring eColoringType,
                                   ItemDescriptionColorOverride pfnColorOverride,
                                   void * ptColorOverrideData)

{
    CRefString usTip;

    BOOL bRequired = FALSE;
    BOOL bHideAffixes = FastAttribGetValueInt(idFastAttrib, ATTRIBUTE_HIDE_AFFIXES);


    if (tInfo.eRequiredItemLabel != ITL_INVALID)
    {


        if (!ItemHasLabel(tACDItem.hGB.gbid, tInfo.eRequiredItemLabel))
        {
            return usTip;
        }
        bRequired = TRUE;
    }









    if (!bRequired)
    {
        if (bHideAffixes)
            return usTip;

        AttribDisplayFunc pDisplayFunc = tInfo.pDisplayFunc;
        if (pDisplayFunc == NULL)
        {
            pDisplayFunc = sCheckAllAttribs;
        }

        if (!pDisplayFunc(tInfo, idFastAttrib))
        {
            return usTip;
        }
    }

    if (KeyGetAttrib(tInfo.eExclusionKey) != 0xFFFF)
    {
        if (FastAttribExists(idFastAttrib, tInfo.eExclusionKey))
        {
            return usTip;
        }
    }

    SNO snoAttributeStringList = GlobalSNOGet(GLOBALSNO_STRINGLIST_ATTRIBUTE_DESCRIPTIONS);
    CRefString usAttributeFormatSpecifier;
    if (!StringListFetchByLabel(snoAttributeStringList, tInfo.hLabel, usAttributeFormatSpecifier))
    {
        usTip.Format("Missing item attribute label (%s):(%d)\n", SNOToString(STRINGLIST_SNO, snoAttributeStringList, FALSE).str(), tInfo.hLabel);
        return usTip;
    }

    if (tInfo.pDescriptionFunc)
    {
        usTip = tInfo.pDescriptionFunc(tACDItem, tInfo, usAttributeFormatSpecifier);
    }
    else
    {
        ToolTipValueColoring eActualColoringType = pfnColorOverride ? TTVC_NO_COLORING : eColoringType;
        usTip = UIToolTips_ProcessTip(&tACDItem, idFastAttrib, ATTRIBGROUPID_INVALID, SNO_NONE, 1, usAttributeFormatSpecifier, eActualColoringType, sResolveValueTag, (void *)&tInfo, NULL);
        if (ePlayerClass != PLAYER_CLASS_NONE)
        {
            usTip += sAppendResourceRestriction(ePlayerClass, tInfo);
        }
    }

    if (!bHideAffixes)
    {
        if (!pfnColorOverride || !pfnColorOverride(usTip, tACDItem, tInfo.arKey[0], ptColorOverrideData))
        {



            if (eColoringType == TTVC_NORMAL_COLORING)
            {

                ItemColorFunc pColorFunc = tInfo.pColorFunc;
                if (pColorFunc == NULL) pColorFunc = ColorFloatByZeroRelativeAttribute;



                if (ItemIsUpgradeableJewel(tACDItem))
                {
                    RGBAColor rgbaLegendaryColor = FontGetColorForFontSpecifier(FONTCOLOR_LEGENDARY, FALSE);
                    Set_Text_Color(usTip, rgbaLegendaryColor);
                }
                else
                {
                    Set_Text_Color(usTip, pColorFunc(tACDItem, tInfo, idFastAttrib));
                }
            }
        }
    }

    return usTip;
}




CRefString ItemGetBlockStats(const ActorCommonData & tACD,
                             const PlayerClass ePlayerClass,
                             ItemDescriptionColorOverride pfnColorOverride,
                             void * ptColorOverrideData)
{
    return sGetSingleAttributeDescription(tACD, ePlayerClass, ctBlockChanceAttribTooltipInfo, pfnColorOverride, ptColorOverrideData) + CRefString("\n") +
           sGetSingleAttributeDescription(tACD, ePlayerClass, ctBlockAmountAttribTooltipInfo, pfnColorOverride, ptColorOverrideData);
}




RGBAColor ColorBasicAttribute(const ActorCommonData &, const AttribTooltipInfo &, AttribGroupID)
{
    return crgbaWhite;
}






RGBAColor ColorFloatByZeroRelativeAttribute(const ActorCommonData & tACDItem,
                                            const FastAttribKey & tKey,
                                            AttribGroupID idFastAttrib,
                                            BOOL & fBonus)

{
    RGBAColor rgbaColor(crgbaWhite);
    ItemQualityLevel eIQL = ItemGetQualityLevel(tACDItem);

    if (KeyGetAttrib(tKey) != KeyGetAttrib(FastAttribKey(ATTRIBUTE_NONE)))
    {
        float flVal;
        if (KeyGetDataType(tKey) == ATTRIBUTEDT_INT)
        {
            flVal = (float)FastAttribGetValueInt(idFastAttrib, tKey);
        }
        else
        {
            flVal = FastAttribGetValueFloat(idFastAttrib, tKey);
        }
        if (flVal > 0.000001f)
        {
            if (eIQL == IQL_SUPERIOR)
            {
                rgbaColor = crgbaWhite;
            }
            else
            {
                rgbaColor = FontGetColorForFontSpecifier(FONTCOLOR_BONUS, FALSE);
            }
            fBonus = TRUE;
        }
        else if (flVal < -0.000001f && !fBonus)
        {
            if (eIQL == IQL_INFERIOR)
            {
                rgbaColor = crgbaGray;
            }
            else
            {
                rgbaColor = FontGetColorForFontSpecifier(FONTCOLOR_CURSE, FALSE);
            }
        }
    }

    return rgbaColor;
}

RGBAColor ColorFloatByZeroRelativeAttribute(const ActorCommonData & tACDItem,
                                            const AttribTooltipInfo & tInfo,
                                            AttribGroupID idFastAttrib)

{
    RGBAColor rgbaColor(crgbaWhite);
    BOOL fBonus = FALSE;


    for (int32 i = 0; i < 3; i++)
    {
        const FastAttribKey & tKey = tInfo.arKey[i]; if (KeyGetAttrib(tKey) == KeyGetAttrib(FastAttribKey(ATTRIBUTE_NONE)))
            break;



        switch (tInfo.eAttribType)
        {
        case TAT_LEGENDARY:
            rgbaColor = FontGetColorForFontSpecifier(FONTCOLOR_LEGENDARY, FALSE);
            break;

        case TAT_CONSOLE:
            rgbaColor = srgbaConsoleTooltip;
            break;

        default:
            rgbaColor = ColorFloatByZeroRelativeAttribute(tACDItem, tKey, idFastAttrib, fBonus);
            break;
        }
    }

    return rgbaColor;
}

CRefString ItemGetDPSTotalDescription(const ActorCommonData & tACD,
                                      const PlayerClass ePlayerClass,
                                      ItemDescriptionColorOverride pfnColorOverride,
                                      void * ptColorOverrideData)

{
    return sGetSingleAttributeDescription(tACD, ePlayerClass, ctDPSAttribTooltipInfo, pfnColorOverride, ptColorOverrideData);
}




CRefString ItemGetDamageStats(const ActorCommonData & tACD,
                              const PlayerClass ePlayerClass,
                              ItemDescriptionColorOverride pfnColorOverride,
                              void * ptColorOverrideData)

{
    return sGetSingleAttributeDescription(tACD, ePlayerClass, ctDamageAttribTooltipInfo, pfnColorOverride, ptColorOverrideData) + CRefString("\n") +
           ItemGetAttacksPerSecond(tACD, ePlayerClass, NULL, NULL);
}




CRefString ItemGetAttacksPerSecond(const ActorCommonData & tACD,
                                   const PlayerClass ePlayerClass,
                                   ItemDescriptionColorOverride pfnColorOverride,
                                   void * ptColorOverrideData)

{
    return sGetSingleAttributeDescription(tACD, ePlayerClass, ctAPSAttribTooltipInfo, pfnColorOverride, ptColorOverrideData);
}



static LPCTSTR sAttribTooltipDelimiter(
    const AttribTooltipInfo & tInfo)
{
    switch (tInfo.eAttribType)
    {
    case TAT_UTILITY:
        return "{icon:bullet} ";
    case TAT_LEGENDARY:
        return "{icon:bullet2} ";
    case TAT_CONSOLE:
        return "{icon:bullet3} ";
    default:
        return "{icon:bullet} ";
    }
}













static BOOL sDisplayFractionalValues(const AttribTooltipInfo & tInfo, AttribGroupID idAttribGroupMin)
{


    if (KeyGetDataType(tInfo.arKey[0]) == ATTRIBUTEDT_FLOAT)
    {
        if (tInfo.eRangeType != RT_PERCENT)
        {
            if (FastAttribGetValueFloat(idAttribGroupMin, tInfo.arKey[0]) <= 5.0f)
            {

                Attrib eAttrib = KeyGetAttrib(tInfo.arKey[0]);
                if (eAttrib != ATTRIBUTE_GOLD_PICKUP_RADIUS)
                {
                    return TRUE;
                }
            }
        }
        else
        {
            if (FastAttribGetValueFloat(idAttribGroupMin, tInfo.arKey[0]) <= 0.1f)
            {
                return TRUE;
            }
        }
    }
    return FALSE;
}




// NON_MATCHING: 2120 vs 2112 bytes; the inlined sAttribTooltipDelimiter switch lowers to compare/select where the original builds a 3-entry lookup table (TAT_UTILITY case kept although it equals the default).
static int32 sGetAttributeDescriptions(
    const ActorCommonData & tACD,
    CRefString & usFullAttribString,
    AttribGroupID idAttribGroupMin,
    AttribGroupID idAttribGroupMax,
    int32 nNumTooltipAttribs,
    const AttribTooltipInfo * arAttribTooltipInfo,
    BOOL bDisplayItemRanges,
    const PlayerClass ePlayerClass,
    ItemDescriptionColorOverride pfnColorOverride,
    void * ptColorOverrideData,
    ACDID idACDPlayer)
{
    CRefString sEnchantedDelimiter("{icon:enchanted} ");

    int32 nCount = 0;

    GBID gbidEnchantedAffix = ItemGetEnchantedAffixNew(tACD);
    FastAttribKey arEnchantedAffixKeys[4];
    if (gbidEnchantedAffix != GBID_NONE)
    {
        ScopedItemAffixLockR pdefAffix(gbidEnchantedAffix);
        for (int32 i = 0; i < 4; i++)
        {
            arEnchantedAffixKeys[i] = MakeAttribKey(pdefAffix->ptAffixAttributes[i].eAttribute, pdefAffix->ptAffixAttributes[i].nParam);
        }
    }


    FastAttribKey tAttribKeyToSkip = ATTRIBUTE_NONE;
    ScopedItemDefinitionLockR pdefItem(tACD.hGB.gbid);
    if (pdefItem &&
        ItemIsUpgradeableJewel(tACD) &&
        pdefItem->nJewelSecondaryEffectUnlockRank > 0)
    {
        tAttribKeyToSkip = MakeAttribKey(pdefItem->tJewelSecondaryEffectAttribute.eAttribute, pdefItem->tJewelSecondaryEffectAttribute.nParam);
    }

    for (int32 i = 0; i < nNumTooltipAttribs; i++)
    {
        CRefString usAttribString("");
        const AttribTooltipInfo & tInfo = arAttribTooltipInfo[i];




        if (tAttribKeyToSkip.nValue != -1)
        {
            BOOL bSkip = FALSE;
            for (int32 k = 0; k < 3; k++)
            {
                if (tInfo.arKey[k] != FastAttribKey(ATTRIBUTE_NONE) && tAttribKeyToSkip == tInfo.arKey[k])
                {
                    bSkip = TRUE;
                    break;
                }
            }

            if (bSkip)
            {
                continue;
            }
        }

        usAttribString = GetAttributeDescription(tInfo, tACD, tACD.idFastAttrib, ePlayerClass, TTVC_NORMAL_COLORING, pfnColorOverride, ptColorOverrideData);
        if (usAttribString.Length() != 0)
        {
            nCount++;

            BOOL bIsEnchanted = FALSE;
            if (gbidEnchantedAffix != GBID_NONE)
            {
                for (int32 j = 0; j < 4 && !bIsEnchanted; j++)
                {
                    if (arEnchantedAffixKeys[j].nValue != -1) for (int32 k = 0; k < 3 && !bIsEnchanted; k++)
                    {
                        bIsEnchanted = tInfo.arKey[k] != FastAttribKey(ATTRIBUTE_NONE) && arEnchantedAffixKeys[j] == tInfo.arKey[k];
                    }
                }
            }





            if (tInfo.bLinkedToCubePower)
            {
                Player * ptPlayer = PlayerGetByACD(idACDPlayer);
                if (ptPlayer && ItemPlayerHasExtractedLegendaryPowerOrLinkedPower(*ptPlayer, tACD.hGB.gbid))
                {
                    usFullAttribString += "{icon:bullet_powerextracted} ";
                }
                else
                {
                    usFullAttribString += "{icon:bullet_poweravailableforextract} ";
                }
            }
            else if (bIsEnchanted)
            {
                usFullAttribString += sEnchantedDelimiter;
            }
            else
            {

                usFullAttribString += sAttribTooltipDelimiter(tInfo);
            }
            if (tInfo.eAttribType == TAT_LEGENDARY)
            {
                RGBAColor rgbaLegendaryColor = FontGetColorForFontSpecifier(FONTCOLOR_LEGENDARY, FALSE);
                Set_Text_Color(usAttribString, rgbaLegendaryColor);
            }
            else if (tInfo.eAttribType == TAT_CONSOLE)
            {
                Set_Text_Color(usAttribString, srgbaConsoleTooltip);
            }

            usFullAttribString += usAttribString;

            if (bDisplayItemRanges && tInfo.eRangeType != RT_INVALID)
            {



                CRefString usMinMaxString;
                if (tInfo.eRangeType == RT_MINDELTA || tInfo.eRangeType == RT_MINMAX)
                {

                    CRefString usTip("{VALUE}");
                    CRefString usMinString = UIToolTips_ProcessTip(NULL, idAttribGroupMin, ATTRIBGROUPID_INVALID, SNO_NONE, 1,




                        usTip, TTVC_NORMAL_COLORING, sResolveValueTag, (void *)&tInfo, NULL);

                    if (tInfo.eRangeType == RT_MINDELTA)
                    {
                        usTip = "[{VALUE}+{VALUE2}]";
                    }
                    else
                    {
                        usTip = "{VALUE2}";
                    }
                    CRefString usMaxString = UIToolTips_ProcessTip(NULL, idAttribGroupMax, ATTRIBGROUPID_INVALID, SNO_NONE, 1,




                        usTip, TTVC_NORMAL_COLORING, sResolveValueTag, (void *)&tInfo, NULL);

                    usMinMaxString = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GENERAL), "AffixRangeFormat"),
                                    usMinString.str(), usMaxString.str(), NULL);
                }
                else
                {
                    LPCSTR * arStrings = sDisplayFractionalValues(tInfo, idAttribGroupMin) ? sgtRangeStringsSmall : sgtRangeStrings;

                    usMinMaxString = GetMinMaxAttributeDescription(tInfo, idAttribGroupMin, idAttribGroupMax, ePlayerClass, arStrings[tInfo.eRangeType], FALSE, TTVC_NORMAL_COLORING);
                }
                Set_Text_Color(usMinMaxString, FONTCOLOR_LIGHTGREY);
                usFullAttribString += " ";
                usFullAttribString += usMinMaxString;
            }

            usFullAttribString += "\n";
        }
    }

    return nCount;
}




static BOOL sResolveValueTagForPowerBonuses(
    const ActorCommonData *,
    AttribGroupID idFastAttribMin,
    AttribGroupID idFastAttribMax,
    LPCSTR szTag,
    float & flResult,
    float * pflResultMax,
    CRefString & sResult,
    SNO snoPower,
    int32,
    void * pData,
    void *,
    FontSpecifier &)
{



    int32 nKeyIdx = sGetKeyIdxFromStringTag(szTag);
    if (nKeyIdx == 0)
    {
        CRefString sPowerNameLabel = PowerGetNameLabel(snoPower);
        sResult = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_POWERS), sPowerNameLabel);

    }
    else if (nKeyIdx == 1)
    {
        Attrib eAttribPowerBonus = (Attrib)(intptr_t)pData;
        flResult = FastAttribGetValueFloat(idFastAttribMin, MakeAttribKey(eAttribPowerBonus, snoPower));
        if (idFastAttribMax != ATTRIBGROUPID_INVALID)
        {
            *pflResultMax = FastAttribGetValueFloat(idFastAttribMax, MakeAttribKey(eAttribPowerBonus, snoPower));
        }
    }
    else if (nKeyIdx == 2)
    {

        ScopedPowerDefinitionLockR ptPowerDef(snoPower);
        GBID gbidClass = TagMapGetInt(ptPowerDef->tPower.hGeneralTagMap, TAG_POWER_CLASS_RESTRICTION);

        const GameBalanceDefinition * pdefGB;
        const HeroData * pHeroData = (const HeroData *)GBLockR(GBHandle(GB_HEROS, gbidClass), pdefGB, TRUE);
        if (pHeroData)
        {
            const ResourceType ePrimaryResource = pHeroData->ePrimaryResourceType;
            const ResourceType eSecondaryResource = pHeroData->eSecondaryResourceType;
            GBUnlockR(pdefGB);

            ResourceType eResourceType = ePrimaryResource;
            if (ptPowerDef->bUseSecondaryResource)
            {

                eResourceType = eSecondaryResource;
            }




            sResult = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_H2O_LAYOUT), SymbolToName(eResourceType, gResourceTypes));
        }

    }
    else return FALSE;
    return TRUE;
}



static void sItemGetPowerBonusAttributeDescriptions(
    CRefString & usOutput,
    CRefString & usClassOutput,
    const CRefString & sLinePrefix,
    const ActorCommonData * ptACDItem,
    const AttribGroupID idFastAttrib,
    const AttribGroupID idFastAttribMin,
    const AttribGroupID idFastAttribMax,
    const PlayerClass ePlayerClass,
    ToolTipValueColoring eColoringType,
    ItemDescriptionColorOverride pfnColorOverride,
    void * ptColorOverrideData,
    BOOL bDisplayItemRanges)
{
    if (idFastAttrib == ATTRIBGROUPID_INVALID)
        return;



    BOOL bHideAffixes = FastAttribGetValueInt(idFastAttrib, ATTRIBUTE_HIDE_AFFIXES);
    if (bHideAffixes)
        return;



    GBID gbidEnchantedAffix = ptACDItem ? ItemGetEnchantedAffixNew(*ptACDItem) : GBID_NONE;
    FastAttribKey arEnchantedAffixKeys[4];
    if (gbidEnchantedAffix != GBID_NONE)
    {
        ScopedItemAffixLockR pdefAffix(gbidEnchantedAffix);
        for (int32 i = 0; i < 4; ++i)
        {
            arEnchantedAffixKeys[i] = MakeAttribKey(pdefAffix->ptAffixAttributes[i].eAttribute, pdefAffix->ptAffixAttributes[i].nParam);
        }
    }


    FastAttribKey tAttribKeyToSkip = ATTRIBUTE_NONE;
    if (ptACDItem && ItemIsUpgradeableJewel(*ptACDItem))
    {
        ScopedItemDefinitionLockR pdefItem(ptACDItem->hGB.gbid);
        if (pdefItem && pdefItem->nJewelSecondaryEffectUnlockRank > 0)
        {
            tAttribKeyToSkip = MakeAttribKey(pdefItem->tJewelSecondaryEffectAttribute.eAttribute, pdefItem->tJewelSecondaryEffectAttribute.nParam);
        }
    }

    SNO snoAttributeStringList = GlobalSNOGet(GLOBALSNO_STRINGLIST_ATTRIBUTE_DESCRIPTIONS);
    ToolTipValueColoring eActualColoringType = pfnColorOverride ? TTVC_NO_COLORING : eColoringType;



    FastAttribIterator tIt(idFastAttribMax == ATTRIBGROUPID_INVALID ? idFastAttrib : idFastAttribMax, TRUE);
    while (FastAttribIterate(tIt))
    {

        FastAttribKey & tKey = tIt.tEntryBuffer.tKey;
        if (!FastAttribExists(tIt.ptGroup, tKey))
        {
            continue;
        }

        Attrib eAttribute = KeyGetAttrib(tKey);

        CRefString sStringListLabel;
        if (eAttribute == ATTRIBUTE_POWER_DAMAGE_PERCENT_BONUS || eAttribute == ATTRIBUTE_POWER_RESOURCE_REDUCTION || eAttribute == ATTRIBUTE_POWER_COOLDOWN_REDUCTION || eAttribute == ATTRIBUTE_POWER_DURATION_INCREASE || eAttribute == ATTRIBUTE_POWER_CRIT_PERCENT_BONUS)
        {




            if (tAttribKeyToSkip.nValue != ATTRIBUTE_NONE)
            {
                if (tAttribKeyToSkip == tKey)
                {
                    continue;
                }
            }

            CRefString usPrefix = sLinePrefix;

            if (gbidEnchantedAffix != GBID_NONE)
            {
                for (int32 j = 0; j < 4; ++j)
                {
                    if (arEnchantedAffixKeys[j].nValue != ATTRIBUTE_NONE && arEnchantedAffixKeys[j] == tKey)
                    {

                        usPrefix = "{icon:enchanted} ";
                        break;
                    }
                }
            }

            sStringListLabel = AttribToString(eAttribute);
            SNO snoParam = KeyGetParam(tKey);

            CRefString usTip = usPrefix;

            StringLabelHandle hLabel = StringListGetLabelHandle(sStringListLabel);

            CRefString usAttributeFormatSpecifier;
            if (!StringListFetchByLabel(snoAttributeStringList, hLabel, usAttributeFormatSpecifier))
            {
                CRefString usErrorTip;
                usErrorTip.Format("Missing item power bonus attribute label (%s):(%d),(%s)\n", SNOToString(STRINGLIST_SNO, snoAttributeStringList, FALSE).str(), hLabel, sStringListLabel.str());
                usTip += usErrorTip;
                continue;
            }

            if (bDisplayItemRanges)
            {
                usTip += UIToolTips_ProcessTip(ptACDItem, idFastAttrib, ATTRIBGROUPID_INVALID, snoParam, TRUE, usAttributeFormatSpecifier, eActualColoringType, sResolveValueTagForPowerBonuses, (void *)(intptr_t)eAttribute, NULL);
            }
            else
            {
                usTip += UIToolTips_ProcessTip(ptACDItem, idFastAttrib, idFastAttribMax, snoParam, TRUE, usAttributeFormatSpecifier, eActualColoringType, sResolveValueTagForPowerBonuses, (void *)(intptr_t)eAttribute, NULL);
            }

            PlayerClass ePowerPlayerClass = PowerGetClassRestriction(AutoPowerDefinitionR(snoParam));
            if (ePlayerClass != PLAYER_CLASS_NONE)
            {

                CRefString sClassName = PlayerGetClassUIName(ePowerPlayerClass, TRUE);
                CRefString sRestriction = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_ATTRIBUTE_DESCRIPTIONS), "ClassRestriction").str(), sClassName.str(), NULL);


                if (ePowerPlayerClass != ePlayerClass)
                {
                    Set_Text_Color(sRestriction, FONTCOLOR_RED);
                }

                usTip += CRefString(" ") + sRestriction;
            }

            if (ptACDItem)
            {
                if (pfnColorOverride && pfnColorOverride(usTip, *ptACDItem, tKey, ptColorOverrideData))
                    eColoringType = TTVC_NO_COLORING;
                else
                {

                    if (eColoringType == TTVC_NORMAL_COLORING)
                    {
                        BOOL fBonus = FALSE;
                        Set_Text_Color(usTip, ColorFloatByZeroRelativeAttribute(*ptACDItem, MakeAttribKey(eAttribute, snoParam), idFastAttrib, fBonus));
                    }
                }
            }
            if (bDisplayItemRanges)
            {
                CRefString sStringListLabelRange = sStringListLabel + CRefString("Range");
                StringLabelHandle hLabelRange = StringListGetLabelHandle(sStringListLabelRange);
                CRefString usRangeFormatSpecifier;
                if (!StringListFetchByLabel(snoAttributeStringList, hLabelRange, usRangeFormatSpecifier))
                {
                    CRefString usErrorTip;
                    usErrorTip.Format("Missing item power bonus attribute label (%s):(%d),(%s)\n", SNOToString(STRINGLIST_SNO, snoAttributeStringList, FALSE).str(), hLabelRange, sStringListLabelRange.str());
                    usTip += usErrorTip;
                    continue;
                }
                CRefString strRange = UIToolTips_ProcessTip(ptACDItem, idFastAttribMin, idFastAttribMax, snoParam, TRUE, usRangeFormatSpecifier, eActualColoringType, sResolveValueTagForPowerBonuses, (void *)(intptr_t)eAttribute, NULL);
                if (strRange.Length())
                {
                    Set_Text_Color(strRange, FONTCOLOR_LIGHTGREY);
                    usTip += " ";
                    usTip += strRange;
                }
            }

            usTip += "\n";

            if (ePowerPlayerClass == PLAYER_CLASS_NONE)
            {
                usOutput += usTip;
            }
            else
            {
                usClassOutput += usTip;
            }
        }
    }
}



CRefString ItemGetPowerBonusAttributeDescriptions(
    const CRefString & sLinePrefix,
    const ActorCommonData * ptACDItem,
    const AttribGroupID idFastAttribMin,
    const AttribGroupID idFastAttribMax,
    const PlayerClass ePlayerClass,
    ToolTipValueColoring eColoringType,
    ItemDescriptionColorOverride pfnColorOverride,
    void * ptColorOverrideData)
{
    return ItemGetPowerBonusAttributeDescriptions(sLinePrefix, ptACDItem, idFastAttribMin, idFastAttribMin, idFastAttribMax, ePlayerClass, eColoringType, pfnColorOverride, ptColorOverrideData, FALSE);
}













CRefString ItemGetPowerBonusAttributeDescriptions(
    const CRefString & sLinePrefix,
    const ActorCommonData * ptACDItem,
    const AttribGroupID idFastAttrib,
    const AttribGroupID idFastAttribMin,
    const AttribGroupID idFastAttribMax,
    const PlayerClass ePlayerClass,
    ToolTipValueColoring eColoringType,
    ItemDescriptionColorOverride pfnColorOverride,
    void * ptColorOverrideData,
    BOOL bDisplayItemRanges)
{
    CRefString usBonusDesc;
    CRefString usClassBonusDesc;
    sItemGetPowerBonusAttributeDescriptions(usBonusDesc, usClassBonusDesc, sLinePrefix, ptACDItem, idFastAttrib, idFastAttribMin, idFastAttribMax, ePlayerClass, eColoringType, pfnColorOverride, ptColorOverrideData, bDisplayItemRanges);
    return usBonusDesc + usClassBonusDesc;
}
















struct LegendaryGemNameCallbackData
{
    BOOL bFoundLegendaryGem;
    CRefString sLegendaryGemName;

    LegendaryGemNameCallbackData()
        :
        bFoundLegendaryGem(FALSE)  // mem-initializer inferred: the store precedes the CRefString constructor and sits at 12415:9
    {
    }
};
static_assert(sizeof(LegendaryGemNameCallbackData) == 0x20, "LegendaryGemNameCallbackData size");
static_assert(offsetof(LegendaryGemNameCallbackData, bFoundLegendaryGem) == 0, "bFoundLegendaryGem offset");
static_assert(offsetof(LegendaryGemNameCallbackData, sLegendaryGemName) == 8, "sLegendaryGemName offset");


static void sPopulateLegendaryGemNameData(
    ACDID idACDSquare,
    InventorySlotType,
    void * pData)
{
    LegendaryGemNameCallbackData * ptCallbackData = (LegendaryGemNameCallbackData *)pData;


    ActorCommonData * ptACDItem = ACDTryToGet(idACDSquare);
    if (!ptACDItem)
        return;



    if (ItemIsLegendary(ptACDItem->hGB.gbid) && ItemIsUpgradeableJewel(ptACDItem->hGB.gbid))
    {

        ptCallbackData->bFoundLegendaryGem = TRUE;

        CRefString sJewelName = ItemGetDisplayName(*ptACDItem, TRUE, FALSE, FALSE, TRUE);
        int32 nRank = ptACDItem->AttributesGetInt(ATTRIBUTE_JEWEL_RANK);
        ptCallbackData->sLegendaryGemName = LocalizedSprintf(StringListFetch("ConsoleInventory:UpgradeableGemNameFormat").str(),
            sJewelName.str(),
            UnicodeText::FormatNumber(nRank, TRUE, FNS_NORMAL, FALSE).str(), NULL);

        ptCallbackData->sLegendaryGemName += "\n";
    }
}





CRefString ItemGetAttributeDescriptions(
    const ActorCommonData & tACD,
    const PlayerClass ePlayerClass,
    BOOL bDisplayItemRanges,
    BOOL bWantPrimarySecondaryTags,
    ItemDescriptionColorOverride pfnColorOverride,
    void * ptColorOverrideData,
    BOOL bWantPrimarySecondaryTagsTitles,
    ACDID idACDPlayer)
{
    CRefString sDelimiter("{icon:bullet} ");



    CRefString sUtilityDelimiter("{icon:bullet} ");


    if (ItemIsGem(tACD))
    {
        return ItemGetGemDescriptions(tACD, sDelimiter, ePlayerClass, TRUE);
    }

    GBID gbidItem = tACD.hGB.gbid;
    CRefString usFullAttribString("");
    SNO snoUIToolTips = GlobalSNOGet(GLOBALSNO_STRINGLIST_UI_TOOLTIPS);
    if (ItemIsUpgradeableJewel(tACD))
    {
        bDisplayItemRanges = FALSE;

        if (!ItemIsSocketed(tACD))
        {

            GBHandleList lstEffects(GBGetHandlePool());
            GBEnumerate(GB_SOCKETED_EFFECTS, lstEffects);

            const GameBalanceDefinition * pdefGB;
            POS pos = lstEffects.GetHeadPosition();

            BOOL fFound = FALSE;
            while (pos && !fFound)
            {
                const GBHandle & hGB = lstEffects.GetNext(pos);
                SocketedEffectsTableEntry * ptEffectDef = (SocketedEffectsTableEntry *)GBLockR(hGB, pdefGB, TRUE);


                if (ptEffectDef->gbidItemBeingSocketed == gbidItem)
                {
                    fFound = TRUE;
                    usFullAttribString += sDelimiter;
                    usFullAttribString += CRefString().Format("%s\n\n", StringListFetch(snoUIToolTips, ptEffectDef->szStringlistLabel).str());
                }

                GBUnlockR(pdefGB);
            }
        }
    }

    AttribGroupID idAttribGroupMin = ATTRIBGROUPID_INVALID;
    AttribGroupID idAttribGroupMax = ATTRIBGROUPID_INVALID;

    if (bDisplayItemRanges)
    {
        idAttribGroupMin = FastAttribGroupCreate(FAT_ITEM, -1);
        idAttribGroupMax = FastAttribGroupCreate(FAT_ITEM, -1);

        ScopedItemDefinitionLockR pdefItem(gbidItem);
        if (pdefItem)
        {
            ItemUpdateMinMaxAttributeStats(idAttribGroupMin, idAttribGroupMax, gbidItem, pdefItem->ptAttributes, pdefItem->serAttributes_Console.dwSize / sizeof(AttributeSpecifier), TRUE, NULL, NULL, &tACD);
        }

        GBID gbidEnchantedAffixOld = ItemGetEnchantedAffixOld(tACD);
        POS pos = tACD.listIdentifiedAffixGBIDs.GetHeadPosition();
        while (pos)
        {
            GBID gbidAffix = tACD.listIdentifiedAffixGBIDs.GetNext(pos);
            if (gbidAffix == gbidEnchantedAffixOld)
            {
                continue;
            }

            ScopedItemAffixLockR pdefAffix(gbidAffix);
            if (pdefAffix)
            {
                ItemUpdateMinMaxAttributeStats(idAttribGroupMin, idAttribGroupMax, gbidItem, pdefAffix->ptAffixAttributes, 4, TRUE, NULL, NULL, &tACD);
            }
        }

        if (ItemHasEnchantedAffix(tACD))
        {
            GBID gbidAffix = ItemGetEnchantedAffixNew(tACD);
            ScopedItemAffixLockR pdefAffix(gbidAffix);
            if (pdefAffix)
            {
                ItemUpdateMinMaxAttributeStats(idAttribGroupMin, idAttribGroupMax, gbidItem, pdefAffix->ptAffixAttributes, 4, TRUE, NULL, NULL, &tACD);
            }
        }
    }

    if (ItemGetUseType(tACD) == ITEM_USE_HEALTHPOTION ||
        ItemIsShard(tACD) ||
        ItemIsUpgradeableJewel(tACD))
    {
        bWantPrimarySecondaryTags = FALSE;
    }


    CRefString strPrimary;
    int32 nNumPrimary = sGetAttributeDescriptions(tACD, strPrimary, idAttribGroupMin, idAttribGroupMax, cgNumTooltipAttribsPrimary, ctAttribTooltipInfoPrimary, bDisplayItemRanges, ePlayerClass, pfnColorOverride, ptColorOverrideData, idACDPlayer);


    CRefString usPowerBonusString;
    CRefString usPowerBonusClassString;
    sItemGetPowerBonusAttributeDescriptions(usPowerBonusString, usPowerBonusClassString, sUtilityDelimiter, &tACD, tACD.idFastAttrib, idAttribGroupMin, idAttribGroupMax, ePlayerClass, TTVC_NORMAL_COLORING, pfnColorOverride, ptColorOverrideData, bDisplayItemRanges);


    CRefString usCubeEnchant;
    if (ItemHasCubeEnchant(tACD))
    {
        int32 nValue = (int32)ItemGetValueForCubeEnchant(tACD);
        int32 nRank = tACD.AttributesGetInt(ATTRIBUTE_CUBE_ENCHANTED_GEM_RANK);
        Attrib eAttrib = ItemGetAttributeForCubeEnchant(tACD);

        usCubeEnchant = LocalizedSprintf(
            StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_ATTRIBUTE_DESCRIPTIONS), FastAttribToString(eAttrib).str()),
            UnicodeText::FormatNumber(nValue, TRUE, FNS_NORMAL, FALSE).str(),
            UnicodeText::FormatNumber(nRank, TRUE, FNS_NORMAL, FALSE).str(), NULL);

        usCubeEnchant.Format("%s%s\n", sDelimiter.str(), usCubeEnchant.str());
        Set_Text_Color(usCubeEnchant, FONTCOLOR_BLUE);
    }

    BOOL bHasPrimaryTag = FALSE;
    if (nNumPrimary > 0 || usPowerBonusString.Length() || usPowerBonusClassString.Length())
    {
        if (bWantPrimarySecondaryTags)
            bHasPrimaryTag = TRUE;










        usFullAttribString += strPrimary;
        usFullAttribString += usPowerBonusString;





        usFullAttribString += usCubeEnchant;
    }

    CRefString strSecondary;
    int32 nNumSecondary = sGetAttributeDescriptions(tACD, strSecondary, idAttribGroupMin, idAttribGroupMax, cgNumTooltipAttribsSecondary, ctAttribTooltipInfoSecondary, bDisplayItemRanges, ePlayerClass, pfnColorOverride, ptColorOverrideData, idACDPlayer);


    CRefString usPassivePowerString;
    CRefString usPassivePowerClassString;
    ItemGetPassivePowerAttributeDescriptions(usPassivePowerString, usPassivePowerClassString, &tACD, TTVC_NORMAL_COLORING, ePlayerClass, bDisplayItemRanges, idAttribGroupMin, idAttribGroupMax, NULL, idACDPlayer);

    if (nNumSecondary > 0 || usPassivePowerString.Length() || usPassivePowerClassString.Length())
    {
        if (nNumSecondary > 0 && bHasPrimaryTag && bWantPrimarySecondaryTags)  // operand order inferred from the original test order (machine code)
        {







            usFullAttribString += "{icon:Console_InventoryBreak2}\n";










        }

        usFullAttribString += strSecondary;
    }









    if (ItemIsUpgradeableJewel(tACD))
    {
        ScopedItemDefinitionLockR pdefItem(gbidItem);
        if (pdefItem && pdefItem->nJewelSecondaryEffectUnlockRank > 0)
        {

            BOOL bUnlocked = tACD.AttributesGetInt(ATTRIBUTE_JEWEL_RANK) >= pdefItem->nJewelSecondaryEffectUnlockRank;
            ToolTipValueColoring eColoring = bUnlocked ? TTVC_NORMAL_COLORING : TTVC_NO_COLORING;

            AttribGroupID idSecondaryAttribGroupMin = FastAttribGroupCreate(FAT_ITEM, -1);
            AttribGroupID idSecondaryAttribGroupMax = ATTRIBGROUPID_INVALID;

            if (bUnlocked)
            {

                FastAttribKey tKey = MakeAttribKey(pdefItem->tJewelSecondaryEffectAttribute.eAttribute, pdefItem->tJewelSecondaryEffectAttribute.nParam);
                FastAttribSetValue(idSecondaryAttribGroupMin, tKey, FastAttribGetValue(tACD.idFastAttrib, tKey));
            }
            else
            {
                idSecondaryAttribGroupMax = FastAttribGroupCreate(FAT_ITEM, -1);
                ItemUpdateMinMaxAttributeStats(idSecondaryAttribGroupMin, idSecondaryAttribGroupMax, gbidItem, &pdefItem->tJewelSecondaryEffectAttribute, 1, TRUE, NULL, NULL, &tACD);
            }

            CRefString usSecondaryEffect = ItemGetAttributeTip(NULL, idSecondaryAttribGroupMin, idSecondaryAttribGroupMax, eColoring);

            usSecondaryEffect += ItemGetPowerBonusAttributeDescriptions(sUtilityDelimiter, NULL, idSecondaryAttribGroupMin, idSecondaryAttribGroupMax, ePlayerClass, eColoring, NULL, NULL);

            CRefString usSecondaryPassivePowerString;
            CRefString usSecondaryPassivePowerClassString;
            ItemGetPassivePowerAttributeDescriptions(usSecondaryPassivePowerString, usSecondaryPassivePowerClassString, NULL, eColoring, ePlayerClass, FALSE, idSecondaryAttribGroupMin, idSecondaryAttribGroupMax, NULL, idACDPlayer);
            usSecondaryEffect += usSecondaryPassivePowerString;
            usSecondaryEffect += usSecondaryPassivePowerClassString;

            FastAttribGroupDelete(idSecondaryAttribGroupMin);
            if (idSecondaryAttribGroupMax != ATTRIBGROUPID_INVALID)
                FastAttribGroupDelete(idSecondaryAttribGroupMax);








            if (usSecondaryEffect.Length())
            {
                CRefString * ptLegendaryGemSecondaryRankString = &usPassivePowerString;

                if (bUnlocked)
                {
                    RGBAColor rgbaLegendaryColor = FontGetColorForFontSpecifier(FONTCOLOR_LEGENDARY, FALSE);
                    Set_Text_Color(usSecondaryEffect, rgbaLegendaryColor);
                    *ptLegendaryGemSecondaryRankString += usSecondaryEffect;
                }
                else
                {
                    Set_Text_Color(usSecondaryEffect, FONTCOLOR_LIGHTGREY);

                    CHAR szSecondaryEffect[1024];
                    Blizzard::String::Copy(szSecondaryEffect, usSecondaryEffect.str(), sizeof(szSecondaryEffect));

                    *ptLegendaryGemSecondaryRankString += StripWhitespace(CRefString(szSecondaryEffect));

                    CRefString usRankRequirement = UnicodeText::FormatNumber(pdefItem->nJewelSecondaryEffectUnlockRank, TRUE, FNS_NORMAL, FALSE);
                    *ptLegendaryGemSecondaryRankString += LocalizedSprintf(StringListFetch(snoUIToolTips, "UpgradeableJewelSecondaryEffectRank"), usRankRequirement.str(), NULL);
                    *ptLegendaryGemSecondaryRankString += "\n";
                }
            }
        }
    }



    LegendaryGemNameCallbackData tLegendaryGemNameData;


    uint32 dwSocketsFilled = tACD.AttributesGetInt(ATTRIBUTE_SOCKETS_FILLED);
    if (dwSocketsFilled)
    {
        ACDInventoryIterateItems(tACD, SLOT_SOCKETS_START, 0, sPopulateLegendaryGemNameData, &tLegendaryGemNameData);
    }


    CRefString strLegendary;
    int32 nNumLegendary = sGetAttributeDescriptions(tACD, strLegendary, idAttribGroupMin, idAttribGroupMax, cgNumTooltipAttribsLegendary, ctAttribTooltipInfoLegendary, bDisplayItemRanges, ePlayerClass, pfnColorOverride, ptColorOverrideData, idACDPlayer);

    if (nNumLegendary > 0 || usPassivePowerString.Length() || usPassivePowerClassString.Length() || tLegendaryGemNameData.bFoundLegendaryGem)
    {
        usFullAttribString += "{icon:Console_InventoryBreak2}\n";
        usFullAttribString += tLegendaryGemNameData.sLegendaryGemName;
        usFullAttribString += strLegendary;
        usFullAttribString += usPassivePowerString;
        usFullAttribString += usPassivePowerClassString;
    }

    CRefString strTrailing;
    int32 nNumTrailing = sGetAttributeDescriptions(tACD, strTrailing, idAttribGroupMin, idAttribGroupMax, cgNumTooltipAttribsTrailing, ctAttribTooltipInfoTrailing, bDisplayItemRanges, ePlayerClass, pfnColorOverride, ptColorOverrideData, idACDPlayer);

    if (nNumTrailing > 0 || strTrailing.Length() || usPowerBonusClassString.Length())
    {
        usFullAttribString += "{icon:Console_InventoryBreak2}\n";

        usFullAttribString += strTrailing;
        usFullAttribString += usPowerBonusClassString;
    }






    if (bDisplayItemRanges)
    {
        FastAttribGroupDelete(idAttribGroupMax);
        FastAttribGroupDelete(idAttribGroupMin);
    }

    return usFullAttribString;
}



static BOOL sDontDisplayNegatives(const AttribTooltipInfo & tInfo, AttribGroupID idFastAttrib)
{
    if (idFastAttrib == ATTRIBGROUPID_INVALID)
        return FALSE;

    const FastAttribKey & tKey = tInfo.arKey[0];
    if (KeyGetAttrib(tKey) != KeyGetAttrib(FastAttribKey(ATTRIBUTE_NONE)) && !FastAttribExists(idFastAttrib, tKey))
        return FALSE;

    float flResult;
    if (KeyGetDataType(tKey) == ATTRIBUTEDT_INT)
        flResult = (float)FastAttribGetValueInt(idFastAttrib, tKey);
    else
        flResult = FastAttribGetValueFloat(idFastAttrib, tKey);
    return !(flResult < 0.0f);
}



















static CRefString sDisplayItemBOEAttribute(const ActorCommonData & tACDItem, const AttribTooltipInfo & tInfo, CRefString & usAttributeFormatSpecifier)
{
    SNO snoAttributeStringList = GlobalSNOGet(GLOBALSNO_STRINGLIST_ATTRIBUTE_DESCRIPTIONS);
    if (ItemGetBoundToACD(tACDItem, TRUE) != ACDID_INVALID)
        return StringListFetch(snoAttributeStringList, "Item_Is_BOE_Bound");
    return StringListFetch(snoAttributeStringList, "Item_Is_BOE");
}














BOOL CheckFirstAttrib(const AttribTooltipInfo & tInfo, AttribGroupID idFastAttrib)
{
    if (idFastAttrib == ATTRIBGROUPID_INVALID)
        return FALSE;

    const FastAttribKey & tKey = tInfo.arKey[0];
    if (KeyGetAttrib(tKey) != KeyGetAttrib(FastAttribKey(ATTRIBUTE_NONE)) && !FastAttribExists(idFastAttrib, tKey))
        return FALSE;
    return TRUE;
}










CRefString GetMinMaxAttributeDescription(const AttribTooltipInfo & tInfo,
                                         const AttribGroupID idFastAttribMin,
                                         const AttribGroupID idFastAttribMax,
                                         const PlayerClass ePlayerClass,
                                         LPCSTR pszAttributeFormatString,
                                         BOOL bAppendResourceRestriction,
                                         ToolTipValueColoring eColoringType)

{
    CRefString usTip;


    AttribDisplayFunc pDisplayFunc = tInfo.pDisplayFunc;
    if (pDisplayFunc == NULL)
    {
        pDisplayFunc = sCheckAllAttribs;
    }

    if (!pDisplayFunc(tInfo, idFastAttribMin))
    {
        return usTip;
    }

    if (KeyGetAttrib(tInfo.eExclusionKey) != 0xFFFF)
    {
        if (FastAttribExists(idFastAttribMin, tInfo.eExclusionKey))
        {
            return usTip;
        }
    }

    SNO snoAttributeStringList = GlobalSNOGet(GLOBALSNO_STRINGLIST_ATTRIBUTE_DESCRIPTIONS);
    CRefString usAttributeFormatSpecifier;
    if (pszAttributeFormatString)
    {
        usAttributeFormatSpecifier = pszAttributeFormatString;
    }
    else if (!StringListFetchByLabel(snoAttributeStringList, tInfo.hLabel, usAttributeFormatSpecifier))
    {
        usTip.Format("Missing item attribute label (%s):(%d)\n", SNOToString(STRINGLIST_SNO, snoAttributeStringList, FALSE).str(), tInfo.hLabel);
        return usTip;
    }

    usTip = UIToolTips_ProcessTip(NULL, idFastAttribMin, idFastAttribMax, SNO_NONE, 1, usAttributeFormatSpecifier, eColoringType, sResolveValueTag, (void *)&tInfo, NULL);
    if (ePlayerClass != PLAYER_CLASS_NONE && bAppendResourceRestriction)
    {

        usTip += sAppendResourceRestriction(ePlayerClass, tInfo);
    }

    return usTip;
}




static BOOL sResolveSocketTag(const ActorCommonData * ptACDItem,
                              AttribGroupID,
                              AttribGroupID,
                              LPCSTR szTag,
                              float & flResult,
                              float *,
                              CRefString &,
                              SNO,
                              int32,
                              void * pData,
                              void *,
                              FontSpecifier &)

{
    int32 nKeyIdx = sGetKeyIdxFromStringTag(szTag);
    if (pData != NULL && nKeyIdx >= 0)
    {
        SocketedEffectsTableEntry * ptEffectDef = (SocketedEffectsTableEntry *)pData;


        {
            Attrib eAttrib = ptEffectDef->ptAttributes[nKeyIdx].eAttribute;
            FastAttribKey keyAttribute = MakeAttribKey(eAttrib, ptEffectDef->ptAttributes[nKeyIdx].nParam);
            flResult = AffixGetFormulaValue(
                ptEffectDef->ptAttributes[nKeyIdx].pbAttributeFormula,
                ptEffectDef->ptAttributes[nKeyIdx].serAttributeFormula.dwSize,
                ptACDItem,
                ptEffectDef->gbidItemBeingSocketed,
                NULL, 0, 0);
            return TRUE;
        }
    }

    return FALSE;
}







CRefString ItemGetGemDescriptions(const ActorCommonData & tACD,
                                  LPCSTR uszDelimiter,
                                  const PlayerClass ePlayerClass,
                                  BOOL bShowSocketableText)

{
    SNO snoItemTypeNames = GlobalSNOGet(GLOBALSNO_STRINGLIST_ITEM_TYPES);
    SNO snoAttributeStringList = GlobalSNOGet(GLOBALSNO_STRINGLIST_ATTRIBUTE_DESCRIPTIONS);
    SNO snoUIToolTips = GlobalSNOGet(GLOBALSNO_STRINGLIST_UI_TOOLTIPS);
    GBID gbidItemToSocket = tACD.hGB.gbid;

    POS pos;
    CRefString aEffectStrings[8];
    GBHandleList lstEffects(GBGetHandlePool());
    GBEnumerate(GB_SOCKETED_EFFECTS, lstEffects);

    const GameBalanceDefinition * pdefGB;
    pos = lstEffects.GetHeadPosition();

    while (pos)
    {
        const GBHandle & hGB = lstEffects.GetNext(pos);
        SocketedEffectsTableEntry * ptEffectDef = (SocketedEffectsTableEntry *)GBLockR(hGB, pdefGB, TRUE);


        if (ptEffectDef->gbidItemBeingSocketed == gbidItemToSocket)
        {
            GBHandle hgbItemType(GB_ITEMTYPES, GBID_NONE);
            hgbItemType.gbid = ptEffectDef->gbidItemTypeSocketedInto;
            CRefString usInitialItemTypeString;
            CRefString usAttributeFormatSpecifier;
            CRefString sItemType("");

            if (StringToGBID("All") == hgbItemType.gbid)
            {
                sItemType = "All";
            }
            else
            {
                sItemType = GBIDToString(hgbItemType, FALSE);
            }

            BOOL bIsPowerPassive = StringToGBID("Item_Power_Passive") == StringToGBID(ptEffectDef->szStringlistLabel);

            if (StringListFetch(snoItemTypeNames, sItemType, usInitialItemTypeString) &&
                (StringListFetch(snoAttributeStringList, ptEffectDef->szStringlistLabel, usAttributeFormatSpecifier) || bIsPowerPassive))
            {

                CRefString usEffect;

                SNO snoItemPassivePowerStringList = GlobalSNOGet(GLOBALSNO_STRINGLIST_ITEM_PASSIVE_POWER_DESCRIPTIONS);
                for (int x = 0; x < 3; x++)
                {
                    AttributeSpecifier & tAttributeSpec = ptEffectDef->ptAttributes[x];
                    if (tAttributeSpec.eAttribute == ATTRIBUTE_ITEM_POWER_PASSIVE)
                    {
                        CRefString sStringListLabel = AttribToString(tAttributeSpec.eAttribute);

                        StringLabelHandle hLabel = StringListGetLabelHandle(SNOToString(SNOName(POWER_SNO, tAttributeSpec.nParam), FALSE).str());

                        CRefString usAttributeFormatSpecifier2;
                        if (!StringListFetchByLabel(snoItemPassivePowerStringList, hLabel, usAttributeFormatSpecifier2))
                        {
                            CRefString usErrorTip;
                            usErrorTip.Format("Missing item passive power label (%s):(%d),(%s)\n", SNOToString(STRINGLIST_SNO, snoItemPassivePowerStringList, FALSE).str(), hLabel, sStringListLabel.str());
                            aEffectStrings[1] = usErrorTip;
                            continue;
                        }

                        AttribGroupID idAttribGem = FastAttribLightCreate(0);
                        FastAttribKey tKey = MakeAttribKey(tAttributeSpec.eAttribute, tAttributeSpec.nParam);
                        float flFormulaValue = AffixGetFormulaValue(tAttributeSpec.pbAttributeFormula,
                                                                    tAttributeSpec.serAttributeFormula.dwSize,
                                                                    NULL,
                                                                    ptEffectDef->gbidItemBeingSocketed,
                                                                    NULL, 0, 0);
                        FastAttribSetValueFloat(idAttribGem, tKey, flFormulaValue);

                        usEffect = UIToolTips_ProcessTip(&tACD, idAttribGem, -1, tAttributeSpec.nParam, 1, usAttributeFormatSpecifier2, TTVC_NORMAL_COLORING, ItemResolveValueTagForItemPassivePowerDescription, (void *)(intptr_t)tAttributeSpec.eAttribute, NULL);

                        FastAttribGroupDelete(idAttribGem);

                        PlayerClass eClassRestriction = PowerGetClassRestriction(AutoPowerDefinitionR(tAttributeSpec.nParam));
                        if (eClassRestriction != PLAYER_CLASS_NONE && eClassRestriction != ePlayerClass)
                        {
                            CRefString sClassName = PlayerGetClassUIName(eClassRestriction, TRUE);
                            CRefString sRestriction = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_ATTRIBUTE_DESCRIPTIONS), "ClassRestriction").str(),
                                                                       sClassName.str(), NULL);
                            Set_Text_Color(sRestriction, FONTCOLOR_RED);
                            usEffect += " ";
                            usEffect += sRestriction;
                        }
                    }
                }

                if (usEffect.Length() == 0)
                {
                    usEffect = UIToolTips_ProcessTip(&tACD,

                                                     tACD.AttributesGetAttributeGroup(),
                                                     -1,
                                                     SNO_NONE,
                                                     1,
                                                     usAttributeFormatSpecifier,
                                                     TTVC_NORMAL_COLORING,
                                                     sResolveSocketTag,
                                                     ptEffectDef,
                                                     NULL);
                }

                if (usEffect.Length() > 0)
                {
                    int32 nStringIdx;

                    if (StringToGBID("Weapon") == hgbItemType.gbid)
                    {
                        nStringIdx = 0;
                    }
                    else if (StringToGBID("Helm") == hgbItemType.gbid)
                    {
                        nStringIdx = 1;
                    }
                    else if (StringToGBID("All") == hgbItemType.gbid)
                    {
                        nStringIdx = 2;
                    }
                    else
                    {
                        nStringIdx = 3;
                        while (nStringIdx < 8 && aEffectStrings[nStringIdx].Length() > 0)
                        {
                            nStringIdx++;
                        }

                        if (nStringIdx >= 8)
                        {

                            aEffectStrings[8 - 1].Format("More than max number of supported slots restrictions for gems! - %d \n", 8);
                            Set_Text_Color(aEffectStrings[8 - 1], FONTCOLOR_RED);
                            break;
                        }
                    }




                    uint32 dwGrammaticalForm;
                    CRefString sItemTypeLoc, sName2Ignored;
                    LanguageRules::ExtractGrammaticalForm(usInitialItemTypeString, sItemTypeLoc, sName2Ignored, dwGrammaticalForm);
                    if (LanguageRules::ItemNamesShouldBeCapitalized())
                    {
                        sItemTypeLoc = UnicodeText::Capitalize(sItemTypeLoc);
                    }

                    Set_Text_Color(sItemTypeLoc, FONTCOLOR_GREY);
                    CRefString sEffect = LocalizedSprintf(StringListFetch(snoUIToolTips, "SocketableEffectFormat").str(), sItemTypeLoc.str(), usEffect.str(), NULL);
                    aEffectStrings[nStringIdx] = CRefString().Format("%s%s\n", uszDelimiter, sEffect.str());
                }
            }
        }

        GBUnlockR(pdefGB);
    }


    CRefString usRetval;
    if (bShowSocketableText)
    {
        usRetval = CRefString().Format("%s%s\n", uszDelimiter, StringListFetch(snoUIToolTips, "Socketable").str());
    }

    for (int32 i = 0; i < 8; i++)
    {
        usRetval += aEffectStrings[i];
    }



    return usRetval;
}



// NON_MATCHING: 1900 bytes both, 400/475 differ (branch threading at 13159-13170 and register allocation); fixing the line layout of 13249/13260 and of the inlined ItemGetLegendaryPower/ItemIsUpgradeableJewel changed nothing.
void ItemGetPassivePowerAttributeDescriptions(
    CRefString & usOutput,
    CRefString & usClassOutput,
    const ActorCommonData * ptACDItem,
    const ToolTipValueColoring eColoringType,
    const PlayerClass ePlayerClass,
    BOOL bDisplayItemRanges,
    AttribGroupID idFastAttribMin,
    AttribGroupID idFastAttribMax,
    LPCSTR uszDelimiter,
    ACDID idACDPlayer)
{
    AttribGroupID idFastAttribCurrent = idFastAttribMin;
    if (ptACDItem)
    {
        BOOL bHideAffixes = FastAttribGetValueInt(ptACDItem->idFastAttrib, ATTRIBUTE_HIDE_AFFIXES);
        if (bHideAffixes)
        {
            return;
        }

        if (bDisplayItemRanges || idFastAttribMin == ATTRIBGROUPID_INVALID)
        {
            idFastAttribCurrent = ptACDItem->idFastAttrib;
        }
    }






    if (idFastAttribCurrent == ATTRIBGROUPID_INVALID)
    {
        return;
    }


    FastAttribKey tAttribKeyToSkip = ATTRIBUTE_NONE;
    if (ptACDItem && ItemIsUpgradeableJewel(*ptACDItem))
    {
        ScopedItemDefinitionLockR pdefItem(ptACDItem->hGB.gbid);
        if (pdefItem && pdefItem->nJewelSecondaryEffectUnlockRank > 0)
        {
            tAttribKeyToSkip = MakeAttribKey(pdefItem->tJewelSecondaryEffectAttribute.eAttribute, pdefItem->tJewelSecondaryEffectAttribute.nParam);
        }
    }

    SNO snoExtractablePower = SNO_NONE;
    if (ptACDItem && ItemCanExtractLegendaryPowerOrLinkedPower(ptACDItem->hGB.gbid))
    {
        snoExtractablePower = ItemGetLegendaryPower(ptACDItem->hGB.gbid);
    }
    BOOL bItemHasExtractableLegendaryPower = ptACDItem && snoExtractablePower != SNO_NONE;

    SNO snoItemPassivePowerStringList = GlobalSNOGet(GLOBALSNO_STRINGLIST_ITEM_PASSIVE_POWER_DESCRIPTIONS);

    FastAttribIterator tIt(idFastAttribMax == ATTRIBGROUPID_INVALID ? idFastAttribCurrent : idFastAttribMax, TRUE);
    while (FastAttribIterate(tIt))
    {

        FastAttribKey & tKey = tIt.tEntryBuffer.tKey;
        if (!FastAttribExists(tIt.ptGroup, tKey))
        {
            continue;
        }

        Attrib eAttribute = KeyGetAttrib(tKey);

        CRefString sStringListLabel;
        if (eAttribute == ATTRIBUTE_ITEM_POWER_PASSIVE)
        {
            if (tAttribKeyToSkip.nValue != ATTRIBUTE_NONE)
            {
                if (tAttribKeyToSkip == tKey)
                {
                    continue;
                }
            }

            sStringListLabel = AttribToString(eAttribute);
            SNO snoParam = KeyGetParam(tKey);


            CRefString usTip(uszDelimiter ? uszDelimiter : "{icon:bullet2} ");
            if (bItemHasExtractableLegendaryPower)
            {
                usTip = "{icon:bullet_poweravailableforextract} ";


                Player * ptPlayer = PlayerGetByACD(idACDPlayer);
                if (ptPlayer && ItemPlayerHasExtractedLegendaryPowerOrLinkedPower(*ptPlayer, ptACDItem->hGB.gbid))
                {
                    usTip = "{icon:bullet_powerextracted} ";
                }
            }

            StringLabelHandle hLabel = StringListGetLabelHandle(SNOToString(SNOName(POWER_SNO, snoParam), FALSE).str());

            CRefString usAttributeFormatSpecifier;
            if (!StringListFetchByLabel(snoItemPassivePowerStringList, hLabel, usAttributeFormatSpecifier))
            {
                CRefString usErrorTip;
                usErrorTip.Format("Missing item passive power label (%s):(%d),(%s)\n", SNOToString(STRINGLIST_SNO, snoItemPassivePowerStringList, FALSE).str(), hLabel, sStringListLabel.str());
                usTip += usErrorTip;
                continue;
            }

            usTip += UIToolTips_ProcessTip(ptACDItem, idFastAttribCurrent, bDisplayItemRanges ? ATTRIBGROUPID_INVALID : idFastAttribMax, snoParam, TRUE, usAttributeFormatSpecifier, eColoringType, ItemResolveValueTagForItemPassivePowerDescription, (void *)(intptr_t)eAttribute, NULL);


            if (eColoringType == TTVC_NORMAL_COLORING)
            {
                RGBAColor rgbaColor = FontGetColorForFontSpecifier(FONTCOLOR_LEGENDARY, FALSE);
                Set_Text_Color(usTip, rgbaColor);
            }

            PlayerClass eClassRestriction = PowerGetClassRestriction(AutoPowerDefinitionR(snoParam));
            if (eClassRestriction != PLAYER_CLASS_NONE && eClassRestriction != ePlayerClass)
            {
                CRefString sClassName = PlayerGetClassUIName(eClassRestriction, TRUE);
                CRefString sRestriction = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_ATTRIBUTE_DESCRIPTIONS), "ClassRestriction").str(),
                    sClassName.str(), NULL);
                Set_Text_Color(sRestriction, FONTCOLOR_RED);
                usTip += " ";
                usTip += sRestriction;
            }

            if (bDisplayItemRanges)
            {

                LPCSTR szValueStart = Blizzard::String::Find(usAttributeFormatSpecifier.str(), "{VALUE1}");
                if (!szValueStart)
                {
                    szValueStart = Blizzard::String::Find(usAttributeFormatSpecifier.str(), "{VALUE}");
                }

                if (szValueStart)
                {

                    CHAR nEndChar = '}';
                    if (szValueStart > usAttributeFormatSpecifier.str() && szValueStart[-1] == '[')
                    {
                        --szValueStart;
                        nEndChar = ']';
                    }


                    int32 nCopyPos = 0;
                    CHAR szValueFormatSpecifier[64];
                    for (; nCopyPos == 0 || (szValueStart[nCopyPos - 1] != nEndChar && szValueStart[nCopyPos]); ++nCopyPos)
                    {
                        szValueFormatSpecifier[nCopyPos] = szValueStart[nCopyPos];
                    }


                    if (szValueStart[nCopyPos] == '%')
                    {
                        szValueFormatSpecifier[nCopyPos++] = '%';
                    }

                    szValueFormatSpecifier[nCopyPos] = 0;


                    CRefString strRange = UIToolTips_ProcessTip(ptACDItem, idFastAttribMin, idFastAttribMax, snoParam, TRUE, szValueFormatSpecifier, eColoringType, ItemResolveValueTagForItemPassivePowerDescription, (void *)(intptr_t)eAttribute, NULL);
                    if (strRange.Length())
                    {
                        Set_Text_Color(strRange, FONTCOLOR_LIGHTGREY);
                        usTip += " ";
                        usTip += strRange;
                    }
                }
            }

            usTip += "\n";

            if (eClassRestriction == PLAYER_CLASS_NONE)
            {
                usOutput += usTip;
            }
            else
            {
                usClassOutput += usTip;
            }
        }
    }
}




static void sAggregateMinMaxValue(
    AttribGroupID idAttribGroup,
    const FastAttribKey & tKey,
    float flNewValue)
{
    FastAttribValue tValueNew;
    if (KeyGetDataType(tKey) == ATTRIBUTEDT_INT)
    {
        tValueNew.nValue = (int32)flNewValue;
    }
    else
    {
        tValueNew.flValue = flNewValue;
    }
    FastAttribValue tValueRunning = FastAttribGetValue(idAttribGroup, tKey);
    FastAttribAggregate(tValueRunning, tValueNew, KeyGetDataType(tKey), KeyGetAggregationOperator(tKey), tKey);
    FastAttribSetValue(idAttribGroup, tKey, tValueRunning);
}




void ItemUpdateMinMaxAttributeStats(
    AttribGroupID idAttribGroupMin,
    AttribGroupID idAttribGroupMax,
    GBID gbidItem,
    const AttributeSpecifier * const arAttributeSpecifiers,
    int32 nAttributeSpecifiersCount,
    BOOL bAggregateAttributes,
    blz::vector<int> * arAllowedFastAttribKeys,
    XGrowableMap<int, int, int, int> * arFastAttribKeysInUse,
    const ActorCommonData * ptACDItem)
{



    RNG tRNG;
    RandSetSeed_Internal(tRNG, 0);


    if (arAllowedFastAttribKeys)
    {
        blz::sort(arAllowedFastAttribKeys->begin(), arAllowedFastAttribKeys->end());
    }

    for (int32 i = 0; i < nAttributeSpecifiersCount; i++)
    {

        const AttributeSpecifier & tAttribute = arAttributeSpecifiers[i];
        if (tAttribute.eAttribute == ATTRIBUTE_NONE ||
            (arAllowedFastAttribKeys &&
             !blz::binary_search(arAllowedFastAttribKeys->begin(), arAllowedFastAttribKeys->end(), tAttribute.eAttribute)))
        {
            continue;
        }


        int32 nAttributeParam = tAttribute.nParam;
        FastAttribKey tKey = MakeAttribKey(tAttribute.eAttribute, nAttributeParam);
        AttributeDataType eAttribType = KeyGetDataType(tKey);
        if (eAttribType == ATTRIBUTEDT_UNKNOWN)
        {
            continue;
        }


        float flMinAffixValue = AffixGetFormulaMinValue(tAttribute, NULL, gbidItem, &tRNG, ptACDItem);







        float flMaxAffixValue = AffixGetFormulaMaxValue(tAttribute, NULL, gbidItem, &tRNG, ptACDItem);







        if (flMinAffixValue < FLT_EPSILON && flMaxAffixValue < FLT_EPSILON)
        {
            continue;
        }



        int32 nUses = 0;
        if (arFastAttribKeysInUse)
        {
            arFastAttribKeysInUse->Lookup(tKey.nValue, nUses);
        }


        if (bAggregateAttributes)
        {

            sAggregateMinMaxValue(idAttribGroupMin, tKey, flMinAffixValue);
            sAggregateMinMaxValue(idAttribGroupMax, tKey, flMaxAffixValue);
        }
        else
        {
            if (KeyGetDataType(tKey) == ATTRIBUTEDT_INT)
            {
                if (!FastAttribExists(idAttribGroupMin, tKey) || (float)FastAttribGetValueInt(idAttribGroupMin, tKey) > flMinAffixValue)
                {
                    FastAttribSetValueInt(idAttribGroupMin, tKey, (int32)flMinAffixValue);
                }

                if (!FastAttribExists(idAttribGroupMax, tKey) || (float)FastAttribGetValueInt(idAttribGroupMax, tKey) < flMaxAffixValue)
                {
                    FastAttribSetValueInt(idAttribGroupMax, tKey, (int32)flMaxAffixValue);
                }
            }
            else
            {
                if (!FastAttribExists(idAttribGroupMin, tKey) || flMinAffixValue < FastAttribGetValueFloat(idAttribGroupMin, tKey))
                {
                    FastAttribSetValueFloat(idAttribGroupMin, tKey, flMinAffixValue);
                }

                if (!FastAttribExists(idAttribGroupMax, tKey) || flMaxAffixValue > FastAttribGetValueFloat(idAttribGroupMax, tKey))
                {
                    FastAttribSetValueFloat(idAttribGroupMax, tKey, flMaxAffixValue);
                }
            }
        }


        if (arFastAttribKeysInUse)
        {
            arFastAttribKeysInUse->SetAt_Tracked(tKey.nValue, nUses + 1);
        }
    }
}



// NON_MATCHING: 2904/2864 bytes; delimiter lowers to comparisons instead of original string table; explicit default and trailing fallback tried.
CRefString ItemGetAttributeTip(
    LPCTSTR sLinePrefix,
    AttribGroupID idFastAttribMin,
    AttribGroupID idFastAttribMax,
    ToolTipValueColoring eColoringType)
{
    CRefString usFullAttribString = "";
    PlayerClass ePlayerClass = PLAYER_CLASS_NONE;
    // DWARF ePlayerClass location starts at constant -1; absent client/player retains that value.
    if (GameActiveGameCommonDataIsClient())
    {
        ActorCommonData * ptACDPlayer = ACDTryToGet(LocalPlayerACDGet());
        if (ptACDPlayer)
        {
            ePlayerClass = PlayerGetPlayerClass(ptACDPlayer);
        }
    }


    // Inference: enumerate the original table; retain nonempty descriptions with the selected prefix.
    for (int32 i = 0; i < cgNumTooltipAttribsPrimary; i++)
    {
        const AttribTooltipInfo & tInfo = ctAttribTooltipInfoPrimary[i];

        CRefString usAttribString = GetMinMaxAttributeDescription(tInfo, idFastAttribMin, idFastAttribMax, ePlayerClass, NULL, TRUE, eColoringType);

        if (usAttribString.Length())
        {
            usFullAttribString += sLinePrefix ? sLinePrefix : sAttribTooltipDelimiter(tInfo);
            usFullAttribString += usAttribString;
            usFullAttribString += "\n";
        }
    }
    // Inference: enumerate the original table; retain nonempty descriptions with the selected prefix.
    for (int32 i = 0; i < cgNumTooltipAttribsSecondary; i++)
    {
        const AttribTooltipInfo & tInfo = ctAttribTooltipInfoSecondary[i];

        CRefString usAttribString = GetMinMaxAttributeDescription(tInfo, idFastAttribMin, idFastAttribMax, ePlayerClass, NULL, TRUE, eColoringType);

        if (usAttribString.Length())
        {
            usFullAttribString += sLinePrefix ? sLinePrefix : sAttribTooltipDelimiter(tInfo);
            usFullAttribString += usAttribString;
            usFullAttribString += "\n";
        }
    }

    // Inference: enumerate the original table; retain nonempty descriptions with the selected prefix.
    for (int32 i = 0; i < cgNumTooltipAttribsLegendary; i++)
    {
        const AttribTooltipInfo & tInfo = ctAttribTooltipInfoLegendary[i];

        CRefString usAttribString = GetMinMaxAttributeDescription(tInfo, idFastAttribMin, idFastAttribMax, ePlayerClass, NULL, TRUE, eColoringType);

        if (usAttribString.Length())
        {
            usFullAttribString += sLinePrefix ? sLinePrefix : sAttribTooltipDelimiter(tInfo);
            usFullAttribString += usAttribString;
            usFullAttribString += "\n";
        }
    }
    // Inference: enumerate the original table; retain nonempty descriptions with the selected prefix.
    for (int32 i = 0; i < cgNumTooltipAttribsTrailing; i++)
    {
        const AttribTooltipInfo & tInfo = ctAttribTooltipInfoTrailing[i];

        CRefString usAttribString = GetMinMaxAttributeDescription(tInfo, idFastAttribMin, idFastAttribMax, ePlayerClass, NULL, TRUE, eColoringType);

        if (usAttribString.Length())
        {
            usFullAttribString += sLinePrefix ? sLinePrefix : sAttribTooltipDelimiter(tInfo);
            usFullAttribString += usAttribString;
            usFullAttribString += "\n";
        }
    }

    return usFullAttribString;

}




































































BOOL ItemResolveValueTagForItemPassivePowerDescription(const ActorCommonData *,
                                                       AttribGroupID idFastAttribMin,
                                                       AttribGroupID idFastAttribMax,
                                                       LPCSTR szTag,
                                                       float & flResultMin,
                                                       float * pflResultMax,
                                                       CRefString &,
                                                       SNO snoPower,
                                                       int32,
                                                       void * pData,
                                                       void *,
                                                       FontSpecifier &)

{

    int32 nKeyIdx = sGetKeyIdxFromStringTag(szTag);
    if (nKeyIdx == 0)
    {
        Attrib eAttribPassivePower = (Attrib)(intptr_t)pData;
        flResultMin = FastAttribGetValueFloat(idFastAttribMin, MakeAttribKey(eAttribPassivePower, snoPower));
        if (idFastAttribMax != -1 && pflResultMax != NULL)
        {
            *pflResultMax = FastAttribGetValueFloat(idFastAttribMax, MakeAttribKey(eAttribPassivePower, snoPower));
        }
        return TRUE;
    }
    return FALSE;
}





CRefString ItemGetDisplayName(const GBID gbidItem)
{
    ScopedItemDefinitionLockR pdefItem(gbidItem);
    if (pdefItem == NULL)
        return "BAD GBID";
    CRefString sBaseItemName;
    CRefString sItemName2Ignored;
    uint32 dwGrammaticalForm;
    int32 nItemLevel;
    ItemGetBaseDisplayName(pdefItem, TRUE, sBaseItemName, sItemName2Ignored, dwGrammaticalForm, nItemLevel);
    if (LanguageRules::ItemNamesShouldBeCapitalized())
        sBaseItemName = UnicodeText::Capitalize(sBaseItemName);
    return sBaseItemName;
}











void ItemGetBaseDisplayName(const ItemDefinition * pdefItem, const BOOL fIdentified, CRefString & sItemName1, CRefString & sItemName2, uint32 & dwGrammaticalForm, int32 & nItemLevel)
{
    sItemName1 = "";
    sItemName2 = "";
    dwGrammaticalForm = 0;
    nItemLevel = pdefItem->nItemLevel;
    if (!fIdentified && (pdefItem->dwFlags & ((1 << ITEM_LEGENDARY_BIT) | (1 << ITEM_SPECIAL_BIT))))
    {
        CRefString sInitialItemType;
        if (StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_ITEM_TYPES), GBIDToString(pdefItem->gbidItemType, GB_ITEMTYPES, FALSE), sInitialItemType))
        {
            LanguageRules::ExtractGrammaticalForm(sInitialItemType, sItemName1, sItemName2, dwGrammaticalForm);
            if (LanguageRules::ItemNamesShouldBeCapitalized())
                sItemName1 = UnicodeText::Capitalize(sItemName1);
        }
    }
    else
    {
        LPCSTR szItemLabel = pdefItem->tHeader.szName;
        CRefString sInitialItemName;
        if (StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_ITEMS), szItemLabel, sInitialItemName))
            LanguageRules::ExtractGrammaticalForm(sInitialItemName, sItemName1, sItemName2, dwGrammaticalForm);
    }
}


















CRefString ItemGetMagicItemDisplayName(const CRefString & sItemName1, const CRefString & sItemName2, CRefString * psAffixes, const uint32 * pdwExtractionFlags, const uint32 dwGrammaticalForm, const BOOL bPreventCapitalization)
{
    AffixType ePrefixToUse = AT_PREFIX;
    if (psAffixes[AT_QUALITY].Length())
        ePrefixToUse = AT_QUALITY;
    CRefString sItemName = sItemName1;
    if (UnicodeText::GetStringDisplayLocale() == XLOCALE_TURKISH && psAffixes[AT_SUFFIX].Length() && sItemName2.Length())
        sItemName = sItemName2;
    if ((dwGrammaticalForm & (1 << GF_CONCATENATE_BIT)) && (pdwExtractionFlags[ePrefixToUse] & (1 << GFE_CAN_CONCATENATE_BIT)))
        LanguageRules::HandleConcatenation(psAffixes[ePrefixToUse], sItemName);
    return LanguageRules::BuildItemName(psAffixes[ePrefixToUse], sItemName, psAffixes[AT_SUFFIX], INC_MAGIC_ITEM, bPreventCapitalization);
}
































RGBAColor ItemGetDisplayNameColor(const ItemQualityLevel eIQL)
{
    switch (eIQL)
    {
    case IQL_INFERIOR:
        return crgbaGray;
    case IQL_MAGIC_1_AFFIX:
    case IQL_MAGIC_2_AFFIX:
    case IQL_MAGIC_3_AFFIX:
    case IQL_SPECIAL:
        return FontGetColorForFontSpecifier(FONTCOLOR_BONUS, FALSE);
    default:
        if (eIQL >= IQL_RARE_AFFIX_START && eIQL <= IQL_RARE_AFFIX_END)
            return FontGetColorForFontSpecifier(FONTCOLOR_RARE, FALSE);
        switch (eIQL)
        {
        case IQL_LEGENDARY:
            return FontGetColorForFontSpecifier(FONTCOLOR_LEGENDARY, FALSE);
        case IQL_DISPLAY_AS_SET_ITEM:
            return crgbaGreen;
        }
    }
    return crgbaWhite;
}






// 2.7.7 helpers (unnamed in functions.csv). Bit 31 of ItemDefinition::dwFlags forces an ancient roll
// (LootRollForAncientLegendary) and a cyan name; attribute ITEM_WAS_SANCTIFIED marks sanctified items.
extern "C" BOOL sub_710050e910(GBID gbidItem)
{
    ScopedItemDefinitionLockR ptItem(gbidItem);
    if (ptItem == NULL)
        return FALSE;
    return (ptItem->dwFlags >> ITEM_ETHEREAL_BIT) & 1;
}

extern "C" BOOL sub_710050e800(GBID gbidItem)
{
    ScopedItemDefinitionLockR ptItem(gbidItem);
    if (ptItem == NULL)
        return FALSE;
    return (ptItem->dwFlags >> ITEM_UNK_26) & 1;
}

extern "C" BOOL sub_71004f88d0(const ActorCommonData * ptACDItem)
{
    if (!ptACDItem || ptACDItem->hGB.eType != GB_ITEMS)
        return FALSE;
    ScopedItemDefinitionLockR pdefItem(ptACDItem->hGB.gbid);
    return (pdefItem->dwFlags >> ITEM_UNK_30) & 1;
}

extern "C" BOOL sub_71004f8a30(GBID gbidItem)
{
    ScopedItemDefinitionLockR ptItem(gbidItem);
    return ptItem->unk_24 & 1;
}

extern "C" BOOL sub_710050eb20(GBID gbidItem)
{
    ScopedItemDefinitionLockR ptItem(gbidItem);
    return (ptItem->unk_24 >> 1) & 1;
}

extern "C" RGBAColor sub_710050e7e0()
{
    return unk_7100ed3648;
}

extern "C" BOOL sub_710050e880(const ActorCommonData * ptACD)
{
    if (ptACD == NULL || ptACD->hGB.eType != GB_ITEMS)
        return FALSE;
    ScopedItemDefinitionLockR ptItem(ptACD->hGB.gbid);
    if (ptItem == NULL)
        return FALSE;
    return (ptItem->dwFlags >> ITEM_ETHEREAL_BIT) & 1;
}

extern "C" BOOL sub_71004f71e0(const ActorCommonData & tACDItem)
{
    if (tACDItem.hGB.eType != GB_ITEMS)
        return FALSE;
    ScopedItemDefinitionLockR ptItem(tACDItem.hGB.gbid);
    return (ptItem->dwFlags >> ITEM_UNK_30) & 1;
}

extern "C" int32 sub_710050e990(GBID gbidItem)
{
    if (gbidItem == GBID_NONE)
        return 0;
    ScopedItemDefinitionLockR ptItem(gbidItem);
    if (ptItem == NULL)
        return 0;
    return ptItem->unk_2f8;
}

extern "C" BOOL sub_710050ea10(const ActorCommonData * ptACD)
{
    if (ptACD == NULL)
        return FALSE;
    return ptACD->AttributesGetInt(ATTRIBUTE_ITEM_WAS_SANCTIFIED) != 0;
}

// 2.7.7: the cyan bit-31 (ethereal) color is the RGBAColor global at 0x7100ed3648.
RGBAColor ItemGetDisplayNameColor(const ActorCommonData & tACD)
{
    if (ItemIsGold(tACD))
        return crgbaWhite;
    RGBAColor rgbaColor;
    if (ItemHasForcedDisplayNameColor(tACD.hGB.gbid, rgbaColor))
        return rgbaColor;
    if (ItemIsQuest(tACD))
        return FontGetColorForFontSpecifier(FONTCOLOR_QUEST, FALSE);
    if (sub_710050e880(&tACD))
        return unk_7100ed3648;
    if (sub_71004f71e0(tACD))
        return RGBAColor(254, 24, 0, 255);
    if (sub_710050ea10(&tACD))
        return RGBAColor(172, 191, 205, 255);
    ItemQualityLevel eIQL = ItemGetDisplayedQualityLevel(tACD);
    return ItemGetDisplayNameColor(eIQL);
}













BOOL ItemHasForcedDisplayNameColor(GBID gbid, RGBAColor & rgbaColor)
{
    if (ItemHasLabel(gbid, ITL_GEM) || (ItemHasLabel(gbid, ITL_POTION) && !ItemIsLegendary(gbid)) ||
        ItemHasLabel(gbid, ITL_KNOWLEDGE_UTILITY) || ItemHasLabel(gbid, ITL_BOOK))
    {
        rgbaColor = FontGetColorForFontSpecifier(FONTCOLOR_SPECIAL_ITEM, FALSE);
        return TRUE;
    }
    if (gbid == GlobalGBIDGet(GLOBALGBID_ITEM_DEATHS_BREATH))
    {
        rgbaColor = FontGetColorForFontSpecifier(FONTCOLOR_SPECIAL_REAGENT, FALSE);
        return TRUE;
    }
    if (ItemHasLabel(gbid, ITL_SCROLL))
    {
        rgbaColor = crgbaWhite;
        return TRUE;
    }
    if (ItemHasLabel(gbid, ITL_PLATINUM))
    {
        rgbaColor = FontGetColorForFontSpecifier(FONTCOLOR_SPECIAL_ITEM, FALSE);
        return TRUE;
    }
    if (ItemHasLabel(gbid, ITL_COLLECTIBLE))
    {
        rgbaColor = FontGetColorForFontSpecifier(FONTCOLOR_COLLECTIBLE, FALSE);
        return TRUE;
    }
    return FALSE;
}








void ItemGetAffixNameFromGBID(const GBID gbidAffix, const uint32 dwGrammaticalForm, LPCSTR szDebugItemName, CRefString * psAffixes, uint32 * pdwExtractionFlags)
{
    ScopedItemAffixLockR ptAffix(gbidAffix);
    if (ptAffix != NULL)
    {
        CRefString sAffixName(ptAffix->tHeader.szName);
        if (ptAffix->gbidAssociatedAffix != GBID_NONE)
            sAffixName = GBIDToString(ptAffix->gbidAssociatedAffix, GB_AFFIXES, FALSE);
        if (StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_AFFIXES), sAffixName, psAffixes[ptAffix->eAffixType]))
            LanguageRules::ExtractMatchingGrammaticalForm(psAffixes[ptAffix->eAffixType], dwGrammaticalForm, pdwExtractionFlags[ptAffix->eAffixType]);
    }
}





















CRefString ItemGetRareName(
    const RareItemName & tRareItemName,
    SNO snoRareItemNameStringList)
{
    CRefString sInitialItemName;

    if (snoRareItemNameStringList != SNO_NONE)
    {




















        ScopedStringListDefinitionLockR pdefStringList(snoRareItemNameStringList);
        if (pdefStringList->dwStringCount != 0)
        {
            int32 nIndex = Math::Pin(tRareItemName.nItemStringListIndex, 0, (int32)pdefStringList->dwStringCount - 1);

            sInitialItemName = pdefStringList->arStrings[nIndex].szText;
        }
        else
        {

            return "";
        }



    }


    CRefString sAffixName;
    if (tRareItemName.snoAffixStringList != SNO_NONE)
    {




















        ScopedStringListDefinitionLockR pdefStringList(tRareItemName.snoAffixStringList);
        if (pdefStringList->dwStringCount != 0)
        {
            int32 nIndex = Math::Pin(tRareItemName.nAffixStringListIndex, 0, (int32)pdefStringList->dwStringCount - 1);

            sAffixName = pdefStringList->arStrings[nIndex].szText;
        }
        else
        {



            return "";
        }



    }

    CRefString sName1, sName2;
    uint32 dwGrammaticalForm;
    uint32 dwExtractionFlags;
    LanguageRules::ExtractGrammaticalForm(sInitialItemName, sName1, sName2, dwGrammaticalForm);
    LanguageRules::ExtractMatchingGrammaticalForm(sAffixName, dwGrammaticalForm, dwExtractionFlags);

    if (tRareItemName.fItemNameIsPrefix)
    {

        CRefString sName = (dwExtractionFlags & (1 << GFE_USE_NOUN_2_BIT)) ? sName2 : sName1;
        if ((dwGrammaticalForm & (1 << GF_CONCATENATE_BIT)) && (dwExtractionFlags & (1 << GFE_CAN_CONCATENATE_BIT)))
        {

            LanguageRules::HandleConcatenation(sName, sAffixName);

            return LanguageRules::BuildItemName("", sAffixName, "", INC_RARE_ITEM, FALSE);
        }


        return LanguageRules::BuildItemName("", sName, sAffixName, INC_RARE_ITEM, FALSE);
    }




    if ((dwGrammaticalForm & (1 << GF_CONCATENATE_BIT)) && (dwExtractionFlags & (1 << GFE_CAN_CONCATENATE_BIT)))
    {

        LanguageRules::HandleConcatenation(sAffixName, sName1);
    }
    return LanguageRules::BuildItemName(sAffixName, sName1, "", INC_RARE_ITEM, FALSE);

}
