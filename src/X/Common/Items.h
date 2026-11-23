// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Items.h
// Subset: ItemType and the item-type queries UIInventory.cpp needs.

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Contrib/Contrib/blz/include/blz/vector.h"
#include "CommonTypes.h"
#include "Definitions/GlobalDefinition.h"
#include "GameBalance.h"
#include "GameSwig.h"
#include "ItemsSwig.h"
#include "../Sigma/SigmaTypes.h"
#include "Attributes/FastAttribute.h"
#include "SNO/SNOFiles.h"
#include "UserInterface/FontDefinition.h"
typedef int32 ItemQualityLevel;  // Items.h:30 in the DWARF.
class CRefString;
typedef int32 GameError;
enum ItemUseType {
    ITEM_USE_NONE = 0,
    ITEM_USE_HEALTHPOTION = 1,
    ITEM_USE_TOWNPORTALSTONE = 3,
    ITEM_USE_CALLDOWN_GRENADE = 6,
    ITEM_USE_PORTAL_SCROLL = 19,
    ITEM_USE_POWER_SCROLL = 20,
    ITEM_USE_PAGE_OF_FATE = 21,
    ITEM_USE_HEARTHPORTALSCROLL = 24,
    ITEM_USE_HEALTHGLYPH = 25,
    ITEM_USE_SCROLL = 28,
    ITEM_USE_MANAGLYPH = 31,
    ITEM_USE_DYE = 32,
    ITEM_USE_LEARN_ENCHANT = 38,
    ITEM_USE_LEARN_RECIPE = 39,
    ITEM_USE_SUMMON_MONSTER = 46,
    ITEM_USE_SUMMON_COMPANION = 48,
    ITEM_USE_ARCANEGLYPH = 50,
    ITEM_USE_PAGE_OF_TRAINING = 53,
    ITEM_USE_TOWNPORTAL_UNLOCK = 54,
    ITEM_USE_POWERUPGLYPH = 55,
    ITEM_USE_COLLECTORS_EDITION_BUFF = 56,
    ITEM_USE_DYE_REUSABLE = 57,
    ITEM_USE_ENCHANT_SCROLL = 58,
    ITEM_USE_SHRINEGLYPH = 59,
    ITEM_USE_UNLOCK_TRANSMOGS = 60,
    ITEM_USE_TREASURE_BAG = 61,
    ITEM_USE_PLAYER_GIFT = 62,
    ITEM_USE_ADD_SOCKETS = 63,
    ITEM_USE_LOOT_RUN_PROGRESS_GLYPH = 64,
    ITEM_USE_LEARN_WINGS = 65,
    ITEM_USE_LEARN_PET = 67,
    ITEM_USE_LEARN_PORTRAIT_FRAME = 68,
    ITEM_USE_LEARN_PENNANT = 69,
    ITEM_USE_HORADRIC_REAGENT = 70,
    NUM_ITEMUSE_TYPES = 71,
};

typedef int32 ToolTipValueColoring;  // line 38

struct Item;
struct ItemType;

typedef blz::vector<ItemType *, blz::allocator<ItemType *> > ItemTypePointerArray;

typedef blz::vector<Item *, blz::allocator<Item *> > ItemPointerArray;

struct ItemType {  // size 0x58
    GBID gbid;  // 0x00
    BOOL fAlwaysMagic;  // 0x04
    int32 nLootLevelRange;  // 0x08
    int32 nReqCrafterLevelForEnchant;  // 0x0c
    uint32 dwFlags;  // 0x10
    uint32 * pdwLabelMask;  // 0x18
    ItemType * ptParentType;  // 0x20
    ItemPointerArray arItems;  // 0x28
    ItemTypePointerArray arChildItemTypes;  // 0x40
};
static_assert(sizeof(ItemType) == 0x58, "ItemType");
enum { ITEMOVERLAY_NONE = 0, ITEMOVERLAY_LOCKED = 1, ITEMOVERLAY_UNUSABLE = 2, ITEMOVERLAY_UNIDENTIFIED = 3, ITEMOVERLAY_RANDOM_ITEM_PLACEHOLDER = 4 };  // line 74 (anonymous in the DWARF; one line here so no later line shifts)
typedef int32 UIItemOverlay;  // line 64

class ActorCommonData;

typedef BOOL (*ItemDescriptionColorOverride)(CRefString &, const ActorCommonData &, const FastAttribKey &, void *);  // line 46

// X/Common/Items.cpp
extern LPCSTR guszItemLabelEmptySocket;  // Items.cpp:133
extern LPCSTR guszItemLabelDurability;  // Items.cpp:135
const ItemType * ItemsGetItemType(GBID gbidItemType);  // Items.cpp:1735
BOOL ItemHasLabel(const GBID gbidItem, const ItemLabel eItemLabel);  // Items.cpp:2977
BOOL ItemIsOfType(GBHandle hgbItem, GBID gbidItemType);  // Items.cpp:3303
BOOL ItemHasEnchantedAffix(const ActorCommonData & tACDItem);  // Items.cpp:5280
GBID ItemGetEnchantedAffixNew(const ActorCommonData & tACDItem);  // Items.cpp:5301
int32 ItemAffixGetNumSockets(GBID gbidItem, GBID gbidAffix);  // Items.cpp:5694
CRefString ItemGetDisplayName(const ActorCommonData & tACD, const BOOL fColorText, const BOOL fForceIdentified, const BOOL fSuppressStackAmount, const BOOL fShortenMagicNames);  // Items.cpp:1302
BOOL ItemIsUnidentified(const ActorCommonData & tACDItem);  // Items.cpp:3679
BOOL ItemIsGem(const ActorCommonData & tACDItem);  // Items.cpp:9584
BOOL ItemIsJewel(const ActorCommonData & tACDItem);  // Items.cpp:9603
BOOL ItemIsUpgradeableJewel(const ActorCommonData & tACDItem);  // Items.cpp:9622
CRefString ItemGetAttributeDescriptions(const ActorCommonData & tACD, const PlayerClass ePlayerClass, BOOL bDisplayItemRanges, BOOL bWantPrimarySecondaryTags, ItemDescriptionColorOverride pfnColorOverride, void * ptColorOverrideData, BOOL bWantPrimarySecondaryTagsTitles, ACDID idACDPlayer);  // Items.cpp:12457
CRefString ItemGetPowerBonusAttributeDescriptions(const CRefString & sLinePrefix, const ActorCommonData * ptACDItem, const AttribGroupID idFastAttribMin, const AttribGroupID idFastAttribMax, const PlayerClass ePlayerClass, ToolTipValueColoring eColoringType, ItemDescriptionColorOverride pfnColorOverride, void * ptColorOverrideData);  // Items.cpp:12350
CRefString ItemGetAttributeTip(LPCTSTR sLinePrefix, AttribGroupID idFastAttribMin, AttribGroupID idFastAttribMax, ToolTipValueColoring eColoringType);  // Items.cpp:13470
CRefString ItemGetDisplayName(const GBID gbidItem);  // Items.cpp:13651
BOOL ItemResolveValueTagForItemPassivePowerDescription(const ActorCommonData *, AttribGroupID idFastAttribMin, AttribGroupID idFastAttribMax, LPCSTR szTag, float & flResultMin, float * pflResultMax, CRefString &, SNO snoPower, int32, void * pData, void *, FontSpecifier &);  // Items.cpp:13618

BOOL ItemCanBeRepaired(const ActorCommonData & tACDItem);
BOOL ItemHasSockets(const ActorCommonData & tACDItem);
int32 ItemGetNumFilledSockets(const ActorCommonData & tACDItem);
BOOL ItemIsDyeable(const ActorCommonData & tACDItem);
GameError IsSocketAddable(const ActorCommonData & tACDItemTarget, const Attrib eAttribAddSocketsType);
GameError IsSocketAddable(const ActorCommonData & tACDItemTarget, const ActorCommonData & tACDItemToUse);
BOOL ItemIsShard(const ActorCommonData & tACDItem);
BOOL ItemIsShard(GBID gbidItem);
BOOL ItemIsJunk(const ActorCommonData & tACDItem);
BOOL ItemIsCurrency(const ActorCommonData & tACDItem);
BOOL ItemIsCurrency(GBID gbidItem);
BOOL ItemIsCraftingReagent(const ActorCommonData & tACDItem);
BOOL ItemIsCraftingReagent(GBID gbidItem);
BOOL ItemIsGold(const ActorCommonData & tACDItem);
BOOL ItemIsGift(GBID gbidItem);
BOOL ItemIsEquippable(const ActorCommonData & tACDItem);
BOOL ItemIsEquippable(GBID gbidItem);
ItemUseType ItemGetUseType(GBID gbidItem);
ItemUseType ItemGetUseType(const ActorCommonData & tACDItem);
BOOL ItemOwnerMeetsRequirements(const ActorCommonData & tACDItem, const ActorCommonData & tACDOwner, GameError * pOutError);
ItemQualityLevel ItemGetQualityLevel(const ActorCommonData & tACD);
BOOL ItemIsCosmeticItem(const GBID gbidItem);

BOOL ItemIsEquipped(const ActorCommonData & tACDItem); // Items.cpp

// Items.cpp: the app subsystem entry points (X/Common/Game.cpp sgtAppSubsystems).
void ItemsInitializeForApp();
void ItemsDisposeForApp();

BOOL ItemIsSocketable(const ActorCommonData & tACDItem);  // Items.cpp:3513
int32 ItemGetLevelRequirement(const GBID gbidItem, const ActorCommonData * ptACDItem, BOOL bIgnoreLevelReductionAffixes);  // Items.cpp:3774
ACDID ItemGetBoundToACD(const ActorCommonData & tACD, BOOL bCheckSockets);  // Items.cpp:5661
BOOL ItemAddSocketedItemAttributes(ActorCommonData & tACDItemToSocket, const ActorCommonData & tACDItemSocketedInto);  // Items.cpp:4752

namespace D3 { namespace Items { class Generator; } }
struct RareItemName;
CRefString ItemGetRareName(const RareItemName & tRareItemName, SNO snoRareItemNameStringList);  // Items.cpp:13888
enum ItemGeneratorFlags {  // X/Common/Items.h:509
    IGF_IDENTIFIED_BIT = 0,
    IGF_BOUND_BIT = 1,
    IGF_CRAFTED_BIT = 2,
    IGF_RESTORED_BIT = 3,
    IGF_CHEATED = 4,
    IGF_MARK_DIRTY_ON_CREATE = 5,
    IGF_ITEM_MARKED_JUNK = 6,
    IGF_VENDOR_BOUGHT_BIT = 7,
    IGF_LOOT_2_0_BIT = 8,
    IGF_ADD_SOCKETS_BIT = 9,
    IGF_ANCIENT_BIT = 10,
    IGF_POST_2_1_2_BIT = 11,
    IGF_REMOVE_LEVEL_REQ = 12,
    IGF_POST_2_3_0_BIT = 13,
    IGF_PRIMAL_ANCIENT_BIT = 14,
    IGF_POST_2_5_0_BIT = 15,
    IGF_POST_2_6_0A_LEGENDARY_GEM_CAP_BIT = 16,
    IGF_TOUCHED_BY_ARMORY_BIT = 17,
    IGF_TOUCHED_BY_ARMORY_LAST_GAME_BIT = 18,
    IGF_COUNT = 19,
};
namespace D3 { namespace OnlineService { class ItemId; } }
void ItemComputeConsoleItemId(const ActorCommonData & tItemACD, D3::OnlineService::ItemId & tId);  // Items.cpp:10332
void ItemGeneratorToDefaults(D3::Items::Generator & tGenerator);  // Items.cpp:5853
template <typename Key, typename ARG_KEY, typename VALUE, typename ARG_VALUE> class XGrowableMap;
typedef XGrowableMap<int, int, int, int> ItemAffixFastAttribMap;
ACDID CItemGenerate(ACDNetworkName ann, const D3::Items::Generator & tGenerator, ItemAffixFastAttribMap * arAffixSpecificAttributes);  // Items.cpp:7494

// Currency types from Items.h:117 and the queries used by inventory displays.
enum {
    CURRENCY_INVALID = -1,
    CURRENCY_GOLD = 0,
    CURRENCY_BLOODSHARDS = 1,
    CURRENCY_PLATINUM = 2,
    CURRENCY_CRAFTING_MAT_START = 3,
    CURRENCY_REUSABLE_PARTS = 3,
    CURRENCY_ARCANE_DUST = 4,
    CURRENCY_VEILED_CRYSTAL = 5,
    CURRENCY_DEATHS_BREATH = 6,
    CURRENCY_FORGOTTEN_SOUL = 7,
    CURRENCY_KHANDURAN_RUNE = 8,
    CURRENCY_CALDEUM_NIGHTSHADE = 9,
    CURRENCY_ARREAT_WAR_TAPESTRY = 10,
    CURRENCY_CORRUPTED_ANGEL_FLESH = 11,
    CURRENCY_WESTMARCH_HOLY_WATER = 12,
    CURRENCY_DEMON_ORGAN_DIABLO = 13,
    CURRENCY_DEMON_ORGAN_GHOM = 14,
    CURRENCY_DEMON_ORGAN_SIEGE_BREAKER = 15,
    CURRENCY_DEMON_ORGAN_SKELETON_KING = 16,
    CURRENCY_DEMON_ORGAN_EYE = 17,
    CURRENCY_DEMON_ORGAN_SPINE_CORD = 18,
    CURRENCY_DEMON_ORGAN_TOOTH = 19,
    NUM_CURRENCY_TYPES = 20,
};

typedef int32 CurrencyType;
uint64 ItemGetMaxStackSize(const GBHandle hgbItem, const ActorCommonData * ptACDOwner, const ActorCommonData * ptACDItem);

// X/Common/Items.cpp:9204, 9253, 4889.
struct AttributeSpecifier;
struct RNG;
BOOL ItemIsDye(GBID gbidItem);
GBID ItemGetGBIDForDyeType(DyeType eDyeType);
float ItemGetAttributeSpecifierValue(const GBID gbidItem, const AttributeSpecifier & tAttribute,
    const ActorCommonData * ptACDOwner, RNG * ptRNGOverride, int32 nLegendaryItemLevelOverride, BOOL bPrimalAncient);

// X/Common/Items.h (reference lines 41, 49) and further Items.cpp entry points.
typedef int32 AncientRank;
enum ItemBindingLevel {
    IBL_NONE = 0,
    IBL_ACCOUNT_EQUIP = 1,
    IBL_ACCOUNT_PICKUP = 2,
    IBL_BOUND_TO_GAME = 3,
    IBL_COUNT = 4,
};
BOOL ItemIsGem(GBID gbidItem);  // Items.cpp:9596
BOOL ItemIsJewel(GBID gbidItem);  // Items.cpp:9615
BOOL ItemIsUpgradeableJewel(GBID gbidItem);  // Items.cpp:9634
BOOL ItemIsUniqueEquipped(GBID gbidItem);  // Items.cpp:5232

// X/Common/Items.h:109-209.
enum {
    CT_NONE = -1,
    CT_SELL = 0,
    CT_BUY = 1,
    CT_BUYBACK = 2,
    CT_GEM_REMOVAL = 3,
};
enum SocketReason {
    SOCKET_REASON_INVALID = -1,
    SOCKET_REASON_ITEM_NOT_SOCKETABLE = 0,
    SOCKET_REASON_NO_FREE_SOCKETS = 1,
    SOCKET_REASON_LEVEL_RESTRICTION_NOT_MET = 2,
    SOCKET_REASON_TYPE_MISMATCH = 3,
    SOCKET_REASON_UNIDENTIFIED = 4,
    SOCKET_REASON_SUCCESS = 5,
    SOCKET_REASON_UNIQUE_EQUIPPED = 6,
    SOCKET_REASON_COUNT = 7,
};
SocketReason ItemCanSocketInto(const ActorCommonData & tACDItemToSocket, ActorCommonData & tACDItemSocketedInto);  // Items.cpp:4416
enum ItemSellError {
    ISE_NONE = 0,
    ISE_UNIDENTIFIED = 1,
    ISE_UNKNOWN = 2,
    ITEM_SELL_ERROR_COUNT = 3,
};
enum ItemReforgeError {
    IRE_NONE = 0,
    IRE_UNIDENTIFIED = 1,
    IRE_NOT_LEGENDARY = 2,
    IRE_LEGACY_ITEM = 3,
    IRE_CRAFTED = 4,
    IRE_CANNOT_EQUIP = 5,
    IRE_HAS_GEMS = 6,
    ITEM_REFORGE_ERROR_COUNT = 7,
};
enum ItemEnchantAffixError {
    IEAE_NONE = 0,
    IEAE_UNIDENTIFIED = 1,
    IEAE_SOCKETABLE = 2,
    IEAE_INVALID_QUALITY = 3,
    IEAE_FILLED_SOCKETS = 4,
    IEAE_CRAFTED = 5,
    IEAE_NO_RANDOM_AFFIXES = 6,
    IEAE_LEGACY_ITEM = 7,
    IEAE_ITEM_TYPE_ABOVE_CRAFTER_LEVEL = 8,
    ITEM_ENCHANT_AFFIX_ERROR_COUNT = 9,
};
enum {
    LPST_NONE = -1,
    LPST_WEAPON = 0,
    LPST_ARMOR = 1,
    LPST_JEWELRY = 2,
    LPST_NUM_TYPES = 3,
};
typedef int32 LegendaryPowerSlotType;

// Items.cpp:8071, 8878 (called from X/Common/Crafting/ItemCrafting.cpp).
struct Player;
BOOL ItemPlayerHasRecipeReagents(const Player & tPlayer, GBID gbidRecipe);  // Items.cpp:8071
ItemReforgeError ItemCanBeReforged(const ActorCommonData & tACDItem);  // Items.cpp:8878
// Items.cpp:3984, 8921 (called from X/Common/Crafting/ItemCrafting.cpp).
typedef int32 CostType;  // X/Common/ActorInventory.h
uint64 ItemGetCost(const ActorCommonData & tACDItem, const CostType eCostType, const int32 eCurrencyType, const ActorCommonData * ptACDVendor, const BOOL bConsiderStackSize, BOOL bIgnoreSockets);  // Items.cpp:3984 (eCurrencyType is a CurrencyType)
float ItemGetDurabilityPercent(const ActorCommonData & tACDItem);  // Items.cpp:8921
// Item names, colours and descriptions (Items.cpp).
struct RGBAColor;
CRefString ItemGetItemQualityDescription(const ItemQualityLevel eIQL, const AncientRank eAncientRank);  // Items.cpp:9770
CRefString ItemTypeGetDescription(const ActorCommonData & tACDItem);  // Items.cpp:9872
CRefString ItemTypeGetDescription(const GBID gbidItemType, const ItemQualityLevel eIQL, const AncientRank eAncientRank);  // Items.cpp:9902
CRefString UIText_IconFromCurrency(CurrencyType eCurrencyType);  // Items.cpp:10372
CRefString UIText_ValueToGameCurrency(const uint64 uAmount, CurrencyType eCurrencyType);  // Items.cpp:10408
RGBAColor ItemGetDisplayNameColor(const ItemQualityLevel eIQL);  // Items.cpp:13762
RGBAColor ItemGetDisplayNameColor(const ActorCommonData & tACD);  // Items.cpp:13792
BOOL ItemHasForcedDisplayNameColor(GBID gbid, RGBAColor & rgbaColor);  // Items.cpp:13817
struct ItemDefinition;
void ItemGetBaseDisplayName(const ItemDefinition * pdefItem, const BOOL fIdentified, CRefString & sItemName1, CRefString & sItemName2, uint32 & dwGrammaticalForm, int32 & nItemLevel);  // Items.cpp:13676
CRefString ItemGetMagicItemDisplayName(const CRefString & sItemName1, const CRefString & sItemName2, CRefString * psAffixes, const uint32 * pdwExtractionFlags, const uint32 dwGrammaticalForm, const BOOL bPreventCapitalization);  // Items.cpp:13718
void ItemGetAffixNameFromGBID(const GBID gbidAffix, const uint32 dwGrammaticalForm, LPCSTR szDebugItemName, CRefString * psAffixes, uint32 * pdwExtractionFlags);  // Items.cpp:13855
// Item queries that other files used to declare locally (Items.cpp).
typedef int32 InventorySlotType;
struct ItemTypeDefinition;
GBID ItemGetItemTypeGBID(GBID gbidItem);  // Items.cpp:1717
BOOL ItemIsLargeSlotItem(GBID gbidItem);  // Items.cpp:1843
BOOL ItemCanBeMarkedAsJunk(const ActorCommonData & tACDItem);  // Items.cpp:2302
BOOL ItemIsAncientRank(const ActorCommonData & tACDItem);  // Items.cpp:2325
AncientRank ItemGetAncientRank(const ActorCommonData & tACDItem);  // Items.cpp:2333
BOOL ItemIsLegendary(GBID gbidItem);  // Items.cpp:5080
BOOL ItemIsCrafted(const ActorCommonData & tACDItem);  // Items.cpp:5178
ItemBindingLevel ItemGetBindingLevel(const ActorCommonData & tACD);  // Items.cpp:5534
BOOL ItemIsAllowedInSlot(InventorySlotType eSlot, const ItemTypeDefinition * pItemTypeDef);  // Items.cpp:8832
BOOL ItemIsSetItem(const GBID gbid);  // Items.cpp:9373
// Item tooltip attribute table entries (X/Common/Items.h).
typedef uint32 StringLabelHandle;  // X/Common/Labels.h
enum RangeType {
    RT_INVALID = -1,
    RT_VALUE = 0,
    RT_PERCENT = 1,
    RT_MINDELTA = 2,
    RT_MINMAX = 3,
    RT_COUNT = 4,
};

enum TooltipAttribType {
    TAT_INVALID = -1,
    TAT_BASIC = 0,
    TAT_UTILITY = 1,
    TAT_LEGENDARY = 2,
    TAT_CONSOLE = 3,
    TAT_COUNT = 4,
};

struct AttribTooltipInfo;

typedef RGBAColor (*ItemColorFunc)(const ActorCommonData &, const AttribTooltipInfo &, AttribGroupID);

typedef BOOL (*AttribDisplayFunc)(const AttribTooltipInfo &, AttribGroupID);

typedef CRefString (*CustomDescriptionFunc)(const ActorCommonData &, const AttribTooltipInfo &, CRefString &);

struct AttribTooltipInfo {  // size 0x40
    AttribTooltipInfo(const StringLabelHandle, RangeType, TooltipAttribType, const FastAttribKey, const FastAttribKey, const FastAttribKey, const AttribDisplayFunc, const CustomDescriptionFunc, const FastAttribKey, const BOOL);
    StringLabelHandle hLabel;  // 0x00
    RangeType eRangeType;  // 0x04
    TooltipAttribType eAttribType;  // 0x08
    FastAttribKey arKey[3];  // 0x0c
    ItemLabel eRequiredItemLabel;  // 0x18
    ItemColorFunc pColorFunc;  // 0x20
    AttribDisplayFunc pDisplayFunc;  // 0x28
    CustomDescriptionFunc pDescriptionFunc;  // 0x30
    FastAttribKey eExclusionKey;  // 0x38
    BOOL bLinkedToCubePower;  // 0x3c
};
static_assert(sizeof(AttribTooltipInfo) == 0x40, "AttribTooltipInfo");

// line 565; the member assignments are on lines 577-588.
inline AttribTooltipInfo::AttribTooltipInfo(const StringLabelHandle hNewLabel, RangeType eNewRangeType, TooltipAttribType eNewAttribType,
                                            const FastAttribKey arNewKey0, const FastAttribKey arNewKey1, const FastAttribKey arNewKey2,
                                            const AttribDisplayFunc pNewDisplayFunc, const CustomDescriptionFunc pNewDescriptionFunc,
                                            const FastAttribKey eNewExclusionKey, const BOOL bNewLinkedToCubePower) {
    hLabel = hNewLabel;
    eRangeType = eNewRangeType;
    eAttribType = eNewAttribType;
    arKey[0] = arNewKey0;
    arKey[1] = arNewKey1;
    arKey[2] = arNewKey2;
    eRequiredItemLabel = ITL_INVALID;
    pColorFunc = NULL;
    pDisplayFunc = pNewDisplayFunc;
    pDescriptionFunc = pNewDescriptionFunc;
    eExclusionKey = eNewExclusionKey;
    bLinkedToCubePower = bNewLinkedToCubePower;
}

RGBAColor ColorBasicAttribute(const ActorCommonData &, const AttribTooltipInfo &, AttribGroupID);  // Items.cpp:11733

struct BasicAttribTooltipInfo : public AttribTooltipInfo {  // size 0x40
    BasicAttribTooltipInfo(const StringLabelHandle, const FastAttribKey, const FastAttribKey, const FastAttribKey, const ItemLabel);
};
static_assert(sizeof(BasicAttribTooltipInfo) == 0x40, "BasicAttribTooltipInfo");

// line 608; the base constructor call is on line 614.
inline BasicAttribTooltipInfo::BasicAttribTooltipInfo(const StringLabelHandle hNewLabel, const FastAttribKey arNewKey0, const FastAttribKey arNewKey1,
                                                      const FastAttribKey arNewKey2, const ItemLabel eNewRequiredItemLabel)
    : AttribTooltipInfo(hNewLabel, RT_INVALID, TAT_BASIC, arNewKey0, arNewKey1, arNewKey2, NULL, NULL, FastAttribKey(-1), FALSE) {
    eRequiredItemLabel = eNewRequiredItemLabel;
    pColorFunc = ColorBasicAttribute;
}

// line 621
struct BasicDisplayItem {  // size 0x8
    Attrib eAttrib;  // 0x00
    Attrib eBaseAttrib;  // 0x04
};
static_assert(sizeof(BasicDisplayItem) == 0x8, "BasicDisplayItem");


void ItemGeneratorPopulateFromACDID(const ActorCommonData & tACDItem, D3::Items::Generator & tGenerator, BOOL bSetRestoredBit);  // Items.cpp:7599

int32 ItemGetItemLevel(const GBID gbidItem);  // Items.cpp:3895

#include "Protobuf/Items.pb.h"
#line 345
struct RareItemName {

    void Initialize()
    {

        fItemNameIsPrefix = FALSE;
        snoAffixStringList = SNO_NONE;
        nAffixStringListIndex = -1;
        nItemStringListIndex = -1;
    }

#line 357
    void Export(
        D3::Items::Generator & tGenerator) const
    {
        const D3::Items::RareItemName & tDefaultRareItemName = D3::Items::Generator::default_instance().rare_item_name();

        bool fPrefix = fItemNameIsPrefix != 0;
        if (fPrefix != tDefaultRareItemName.item_name_is_prefix() ||
            snoAffixStringList != tDefaultRareItemName.sno_affix_string_list() ||
            nAffixStringListIndex != tDefaultRareItemName.affix_string_list_index() ||
            nItemStringListIndex != tDefaultRareItemName.item_string_list_index())
        {
            D3::Items::RareItemName * ptRareItemName = tGenerator.mutable_rare_item_name();
            ptRareItemName->set_item_name_is_prefix(fPrefix);
            ptRareItemName->set_sno_affix_string_list(snoAffixStringList);
            ptRareItemName->set_affix_string_list_index(nAffixStringListIndex);
            ptRareItemName->set_item_string_list_index(nItemStringListIndex);
        }
    }
#line 377
    void Import(
        const D3::Items::RareItemName & tRareItemName);
#line 386
    BOOL fItemNameIsPrefix;
    SNO snoAffixStringList;
    int32 nAffixStringListIndex;
    int32 nItemStringListIndex;
};

// Layout checks generated from the DWARF by tools/layout_asserts.py.
#include <stddef.h>
static_assert(sizeof(RareItemName) == 0x10, "RareItemName");
static_assert(offsetof(RareItemName, fItemNameIsPrefix) == 0x0, "RareItemName");
static_assert(offsetof(RareItemName, snoAffixStringList) == 0x4, "RareItemName");
static_assert(offsetof(RareItemName, nAffixStringListIndex) == 0x8, "RareItemName");
static_assert(offsetof(RareItemName, nItemStringListIndex) == 0xc, "RareItemName");
static_assert(sizeof(ItemType) == 0x58, "ItemType");
static_assert(offsetof(ItemType, gbid) == 0x0, "ItemType");
static_assert(offsetof(ItemType, fAlwaysMagic) == 0x4, "ItemType");
static_assert(offsetof(ItemType, nLootLevelRange) == 0x8, "ItemType");
static_assert(offsetof(ItemType, nReqCrafterLevelForEnchant) == 0xc, "ItemType");
static_assert(offsetof(ItemType, dwFlags) == 0x10, "ItemType");
static_assert(offsetof(ItemType, pdwLabelMask) == 0x18, "ItemType");
static_assert(offsetof(ItemType, ptParentType) == 0x20, "ItemType");
static_assert(offsetof(ItemType, arItems) == 0x28, "ItemType");
static_assert(offsetof(ItemType, arChildItemTypes) == 0x40, "ItemType");
static_assert(sizeof(AttribTooltipInfo) == 0x40, "AttribTooltipInfo");
static_assert(offsetof(AttribTooltipInfo, hLabel) == 0x0, "AttribTooltipInfo");
static_assert(offsetof(AttribTooltipInfo, eRangeType) == 0x4, "AttribTooltipInfo");
static_assert(offsetof(AttribTooltipInfo, eAttribType) == 0x8, "AttribTooltipInfo");
static_assert(offsetof(AttribTooltipInfo, arKey) == 0xc, "AttribTooltipInfo");
static_assert(offsetof(AttribTooltipInfo, eRequiredItemLabel) == 0x18, "AttribTooltipInfo");
static_assert(offsetof(AttribTooltipInfo, pColorFunc) == 0x20, "AttribTooltipInfo");
static_assert(offsetof(AttribTooltipInfo, pDisplayFunc) == 0x28, "AttribTooltipInfo");
static_assert(offsetof(AttribTooltipInfo, pDescriptionFunc) == 0x30, "AttribTooltipInfo");
static_assert(offsetof(AttribTooltipInfo, eExclusionKey) == 0x38, "AttribTooltipInfo");
static_assert(offsetof(AttribTooltipInfo, bLinkedToCubePower) == 0x3c, "AttribTooltipInfo");
static_assert(sizeof(BasicAttribTooltipInfo) == 0x40, "BasicAttribTooltipInfo");
static_assert(sizeof(BasicDisplayItem) == 0x8, "BasicDisplayItem");
static_assert(offsetof(BasicDisplayItem, eAttrib) == 0x0, "BasicDisplayItem");
static_assert(offsetof(BasicDisplayItem, eBaseAttrib) == 0x4, "BasicDisplayItem");

// Placement in this owning API header is inferred; raw DWARF has only CPP definitions.
void ItemUpdateLegacyItem(D3::Items::Generator & tGenerator); // Items.cpp:11057
void ItemCapJewelRank(D3::Items::Generator & tGenerator); // Items.cpp:11099
void ItemSetLockedToACD(ActorCommonData & tACD, const ACDID idACDOwner); // Items.cpp:5579
BOOL ItemIsConsumablePotion(const GBID gbidItem); // Items.cpp:2367

// Owning-header placement inferred; raw DWARF has only Items.cpp:110, no header declaration.
class XTypeDescriptor;
extern const XTypeDescriptor * DT_RARE_ITEM_NAME;

// Owning-header placement inferred; raw DWARF records only Items.cpp:12375, no header declaration.
CRefString ItemGetPowerBonusAttributeDescriptions(const CRefString & sLinePrefix, const ActorCommonData * ptACDItem, const AttribGroupID idFastAttrib, const AttribGroupID idFastAttribMin, const AttribGroupID idFastAttribMax, const PlayerClass ePlayerClass, ToolTipValueColoring eColoringType, ItemDescriptionColorOverride pfnColorOverride, void * ptColorOverrideData, BOOL bDisplayItemRanges);

// Owning-header placement inferred; full raw DWARF search finds only Items.cpp definition records.
void ItemGetPassivePowerAttributeDescriptions(CRefString & usOutput, CRefString & usClassOutput, const ActorCommonData * ptACDItem, const ToolTipValueColoring eColoringType, const PlayerClass ePlayerClass, BOOL bDisplayItemRanges, AttribGroupID idFastAttribMin, AttribGroupID idFastAttribMax, LPCSTR uszDelimiter, ACDID idACDPlayer); // Items.cpp:13138
CRefString ItemGetGemDescriptions(const ActorCommonData & tACD, LPCSTR uszDelimiter, const PlayerClass ePlayerClass, BOOL bShowSocketableText); // Items.cpp:12954
void ItemUpdateMinMaxAttributeStats(AttribGroupID idAttribGroupMin, AttribGroupID idAttribGroupMax, GBID gbidItem, const AttributeSpecifier * const arAttributeSpecifiers, int32 nAttributeSpecifiersCount, BOOL bAggregateAttributes, blz::vector<int> * arAllowedFastAttribKeys, XGrowableMap<int, int, int, int> * arFastAttribKeysInUse, const ActorCommonData * ptACDItem); // Items.cpp:13351
