// X/Common/Crafting/ItemCrafting.cpp
// Functions sit on their original line numbers.

#include "ItemCrafting.h"
#include "ItemCraftingSwig.h"
#include "../ActorCommonData.h"
#include "../ActorInventory.h"
#include "../Definitions/GameBalanceDefinition.h"
#include "../Game.h"
#include "../GameBalance.h"
#include "../Items.h"
#include "../ItemsSwig.h"
#include "../Player.h"
#include "../SNO/SNOFiles.h"
#include "../TagMaps/AnimTagsSwig.h"
#include "../../Server/SPlayer.h"
#include "../Definitions/ActorDefinition.h"
#include "../Definitions/GlobalDefinition.h"
#include "../Definitions/Definitions.h"
#include "../TagMaps/ActorTags.h"
#include "../TagMaps/Tags.h"
#include "../../Sigma/Math/MathGeneral.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../Protobuf/ItemCrafting.pb.h"
#include "../../Sigma/RefString.h"
#include "../Networking/GameMessage.h"
#include "../../Sigma/ErrorManager/ErrorManager.h"
// DWARF: X/Common/Items.cpp:3558.
ItemEnchantAffixError ItemCanEnchantAffix(const ActorCommonData & tACDItem);
// DWARF: X/Common/Player.cpp:3323.
void PlayerCrafterInitialize(CrafterData & tCrafterData);
// DWARF declarations: Items.cpp:9448,9460,10556,9709,9736.
BOOL ItemIsEquippableByPlayerOrHireling(const ActorCommonData & tACDItem);
BOOL ItemIsEquippableByPlayerOrHireling(GBID gbidItem);
SNO ItemGetLegendaryPower(GBID gbidItem);
BOOL ItemIsActBountyReagent(GBID gbidItem);
BOOL ItemIsGemConversionReagent(GBID gbidItem);
#include "../../Sigma/TypeDescriptor/TypeDescriptor.h"
// DWARF: X/Common/Items.cpp:115.
extern const XTypeDescriptor * DT_ENCHANT_AFFIX_CHOICE_DATA;
// Inferred GfxVB.h inclusion from its four normal-color objects in the original initializer (DWARF351-354).
#include "../../Client/Gfx/GfxVB.h"
#include "../../Sigma/DataStructures/XGrowableMap.h"
#include "../../Sigma/DataStructures/XChainedFixedBlockAllocator.h"
#include "../../../Contrib/Contrib/blz/include/blz/unordered_map.h"
// Local copies of X/Common/Items.h types (Items.h:223-326 in the DWARF; Items.cpp keeps the same cpp-local definitions until Items.h adopts them).
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
struct CraftedLegendaryPowerInfo {  // size 0x8
    SNO snoPower;  // 0x00
    float flUsePowerValue;  // 0x04
};
struct CurrencyInfo {  // size 0x8
    GBID gbidItem;  // 0x00
    BOOL bAutoPickup;  // 0x04
};
typedef int32 AffixGroup;  // X/Common/Definitions/GameBalanceDefinition.h
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
static_assert(offsetof(ItemAppGlobals, nAffixes) == 0x78, "nAffixes");
// X/Common/Definitions/GameBalanceDefinition.h:49 (not in the adopted GB_ subset there).
enum {
    GB_AFFIXES = 8,
};
// DWARF: X/Common/Items.cpp:6089.
BOOL ItemCanAddAffix(const ItemAffixSpecifier & tSpecifier, const AffixTableEntry * ptAffixTableEntry, AffixTableEntry ** ptSpecifierAffixTableEntries);
#line 47
const int32 cgnCrafterLevelsPerRank[CRAFTER_TYPE_COUNT] =
{
    1,  // CRAFTER_TYPE_BLACKSMITH
    1,  // CRAFTER_TYPE_JEWEL
    1,  // CRAFTER_TYPE_MYSTIC
    0,  // CRAFTER_TYPE_HORADRIM
    0,  // CRAFTER_TYPE_JEWEL_UPGRADE
};







static const float scflJewelUpgradeChanceLowerRank[11] =
{
    0.6f, 0.6f, 0.6f, 0.6f, 0.6f, 0.6f, 0.6f, 0.7f, 0.8f, 0.9f, 1.0f,
};












static const float scflJewelUpgradeChanceHigherRank[17] =
{
    0.6f, 0.3f, 0.15f, 0.08f, 0.04f, 0.02f, 0.01f, 0.01f, 0.01f, 0.01f, 0.01f, 0.01f, 0.01f, 0.01f, 0.01f, 0.01f, 0.01f,
};

// DWARF: X/Common/Crafting/ItemCrafting.h:67.
struct CraftItemsMessage { RequiredMessageHeader tHeader; GBID gbidRecipe; int32 nCount; }; static_assert(sizeof(CraftItemsMessage) == 0x10, "CraftItemsMessage");
// DWARF: X/Common/Crafting/ItemCrafting.h:76.
struct CraftingResultsMessage { RequiredMessageHeader tHeader; ACDNetworkName annItem; GBID gbidItem; ItemQualityLevel eIQL; }; static_assert(sizeof(CraftingResultsMessage) == 0x14, "CraftingResultsMessage");
// DWARF: X/Common/Crafting/ItemCrafting.h:86.
struct CrafterLevelUpMessage { RequiredMessageHeader tHeader; CrafterType eType; uint32 dwAnimTag; uint32 dwNewIdle; int32 nLevel; }; static_assert(sizeof(CrafterLevelUpMessage) == 0x18, "CrafterLevelUpMessage");
// DWARF: X/Common/Crafting/ItemCrafting.h:97.
struct TransmogItemMessage { RequiredMessageHeader tHeader; ACDNetworkName annItem; GBID gbidTransmog; }; static_assert(sizeof(TransmogItemMessage) == 0x10, "TransmogItemMessage");
// DWARF: X/Common/Crafting/ItemCrafting.h:106.
struct DyeItemMessage { RequiredMessageHeader tHeader; ACDNetworkName annItem; DyeType eDyeType; }; static_assert(sizeof(DyeItemMessage) == 0x10, "DyeItemMessage");
// DWARF: X/Common/Crafting/ItemCrafting.h:121.
struct EnchantAffixMessage { RequiredMessageHeader tHeader; ACDNetworkName annItem; GBID gbidAffixToReroll; }; static_assert(sizeof(EnchantAffixMessage) == 0x10, "EnchantAffixMessage");
// DWARF: X/Common/Crafting/ItemCrafting.h:130.
struct EnchantAffixChooseMessage { RequiredMessageHeader tHeader; ACDNetworkName annItem; EnchantAffixChoice tEnchantAffixChoices[6]; int32 nEnchantAffixChoicesCount; }; static_assert(sizeof(EnchantAffixChooseMessage) == 0x40, "EnchantAffixChooseMessage");





// Recovered descriptor expansion: original ctor arguments, field order and DWARF array/global names.
static const TypeDescriptorField CraftItemsMessage_Fields[] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(CraftItemsMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_GBID, offsetof(CraftItemsMessage, gbidRecipe), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(CraftItemsMessage, nCount), NULL, 1),
TypeDescriptorField(NULL, DT_NULL, sizeof(CraftItemsMessage), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_CraftItemsMessage("CraftItemsMessage", 0, CraftItemsMessage_Fields, 4, NULL, (1 << TD_IS_COMPLEX_TYPE), NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_CRAFT_ITEMS_MESSAGE = &TypeDescriptor_CraftItemsMessage;

static const TypeDescriptorField CraftingResultsMessage_Fields[] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(CraftingResultsMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(CraftingResultsMessage, annItem), NULL, 1),
    TypeDescriptorField("", DT_GBID, offsetof(CraftingResultsMessage, gbidItem), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(CraftingResultsMessage, eIQL), NULL, 1),
TypeDescriptorField(NULL, DT_NULL, sizeof(CraftingResultsMessage), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_CraftingResultsMessage("CraftingResultsMessage", 0, CraftingResultsMessage_Fields, 5, NULL, (1 << TD_IS_COMPLEX_TYPE), NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_CRAFTING_RESULT_MESSAGE = &TypeDescriptor_CraftingResultsMessage;

static const TypeDescriptorField CrafterLevelUpMessage_Fields[] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(CrafterLevelUpMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(CrafterLevelUpMessage, eType), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(CrafterLevelUpMessage, dwAnimTag), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(CrafterLevelUpMessage, dwNewIdle), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(CrafterLevelUpMessage, nLevel), NULL, 1),
TypeDescriptorField(NULL, DT_NULL, sizeof(CrafterLevelUpMessage), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_CrafterLevelUpMessage("CrafterLevelUpMessage", 0, CrafterLevelUpMessage_Fields, 6, NULL, (1 << TD_IS_COMPLEX_TYPE), NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_CRAFTER_LEVEL_UP_MESSAGE = &TypeDescriptor_CrafterLevelUpMessage;

static const TypeDescriptorField TransmogItemMessage_Fields[] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(TransmogItemMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(TransmogItemMessage, annItem), NULL, 1),
    TypeDescriptorField("", DT_GBID, offsetof(TransmogItemMessage, gbidTransmog), NULL, 1),
TypeDescriptorField(NULL, DT_NULL, sizeof(TransmogItemMessage), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_TransmogItemMessage("TransmogItemMessage", 0, TransmogItemMessage_Fields, 4, NULL, (1 << TD_IS_COMPLEX_TYPE), NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_TRANSMOG_ITEM_MESSAGE = &TypeDescriptor_TransmogItemMessage;

static const TypeDescriptorField DyeItemMessage_Fields[] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(DyeItemMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(DyeItemMessage, annItem), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(DyeItemMessage, eDyeType), NULL, 1),
TypeDescriptorField(NULL, DT_NULL, sizeof(DyeItemMessage), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_DyeItemMessage("DyeItemMessage", 0, DyeItemMessage_Fields, 4, NULL, (1 << TD_IS_COMPLEX_TYPE), NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_DYE_ITEM_MESSAGE = &TypeDescriptor_DyeItemMessage;

static const TypeDescriptorField EnchantAffixMessage_Fields[] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(EnchantAffixMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(EnchantAffixMessage, annItem), NULL, 1),
    TypeDescriptorField("", DT_GBID, offsetof(EnchantAffixMessage, gbidAffixToReroll), NULL, 1),
TypeDescriptorField(NULL, DT_NULL, sizeof(EnchantAffixMessage), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_EnchantAffixMessage("EnchantAffixMessage", 0, EnchantAffixMessage_Fields, 4, NULL, (1 << TD_IS_COMPLEX_TYPE), NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_ENCHANT_AFFIX_MESSAGE = &TypeDescriptor_EnchantAffixMessage;

static const TypeDescriptorField EnchantAffixChooseMessage_Fields[] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(EnchantAffixChooseMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(EnchantAffixChooseMessage, annItem), NULL, 1),
    FixedArrayTypeDescriptorField("", DT_ENCHANT_AFFIX_CHOICE_DATA, offsetof(EnchantAffixChooseMessage, tEnchantAffixChoices), NULL, 1, 6, 0),
    TypeDescriptorField("", DT_INT, offsetof(EnchantAffixChooseMessage, nEnchantAffixChoicesCount), NULL, 1),
TypeDescriptorField(NULL, DT_NULL, sizeof(EnchantAffixChooseMessage), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_EnchantAffixChooseMessage("EnchantAffixChooseMessage", 0, EnchantAffixChooseMessage_Fields, 5, NULL, (1 << TD_IS_COMPLEX_TYPE), NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_ENCHANT_AFFIX_CHOOSE_MESSAGE = &TypeDescriptor_EnchantAffixChooseMessage;

static const TypeDescriptorField TransmuteItemsMessage_Fields[] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(TransmuteItemsMessage, tHeader), NULL, 1),
    FixedArrayTypeDescriptorField("", DT_INT, offsetof(TransmuteItemsMessage, annItems), NULL, 1, 9, 0),
    TypeDescriptorField("", DT_INT, offsetof(TransmuteItemsMessage, nItemsCount), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(TransmuteItemsMessage, nStackedItemsBitField), NULL, 1),
    FixedArrayTypeDescriptorField("", DT_GBID, offsetof(TransmuteItemsMessage, gbidCurrencies), NULL, 1, 9, 0),
    TypeDescriptorField("", DT_INT, offsetof(TransmuteItemsMessage, nCurrenciesCount), NULL, 1),
TypeDescriptorField(NULL, DT_NULL, sizeof(TransmuteItemsMessage), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_TransmuteItemsMessage("TransmuteItemsMessage", 0, TransmuteItemsMessage_Fields, 7, NULL, (1 << TD_IS_COMPLEX_TYPE), NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_TRANSMUTE_ITEMS_MESSAGE = &TypeDescriptor_TransmuteItemsMessage;

static const TypeDescriptorField TransmuteResultsMessage_Fields[] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(TransmuteResultsMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(TransmuteResultsMessage, annItem), NULL, 1),
    TypeDescriptorField("", DT_GBID, offsetof(TransmuteResultsMessage, gbidPower), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(TransmuteResultsMessage, eType), NULL, 1),
    TypeDescriptorField("", DT_GBID, offsetof(TransmuteResultsMessage, gbidFakeItem), NULL, 1),
    TypeDescriptorField("", DT_INT64, offsetof(TransmuteResultsMessage, uFakeItemStackCount), NULL, 1),
TypeDescriptorField(NULL, DT_NULL, sizeof(TransmuteResultsMessage), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_TransmuteResultsMessage("TransmuteResultsMessage", 0, TransmuteResultsMessage_Fields, 7, NULL, (1 << TD_IS_COMPLEX_TYPE), NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_TRANSMUTE_RESULTS_MESSAGE = &TypeDescriptor_TransmuteResultsMessage;










namespace ItemCrafting_BackEnd
{




struct RepairCostVisitor
{
    RepairCostVisitor(const Player * cptPlayer) : ptPlayer(cptPlayer), nAccumulatedCost(0) {}
    const Player * ptPlayer;
    int32 nAccumulatedCost;
};




uint32 sArrLevelUpAnimTags[CRAFTER_RANKS] =
{
    TAG_ACTOR_ANIMATION_NEUTRAL,
    TAG_ACTOR_ANIMATION_SCRIPTED_EVENT_1,
    TAG_ACTOR_ANIMATION_SCRIPTED_EVENT_2,
    TAG_ACTOR_ANIMATION_SCRIPTED_EVENT_3,
    TAG_ACTOR_ANIMATION_SCRIPTED_EVENT_4,
    TAG_ACTOR_ANIMATION_SCRIPTED_EVENT_5,
    TAG_ACTOR_ANIMATION_SCRIPTED_EVENT_6,
    TAG_ACTOR_ANIMATION_SCRIPTED_EVENT_7,
    TAG_ACTOR_ANIMATION_SCRIPTED_EVENT_8,
    TAG_ACTOR_ANIMATION_SCRIPTED_EVENT_9,
    TAG_ACTOR_ANIMATION_SCRIPTED_EVENT_10,
    TAG_ACTOR_ANIMATION_SCRIPTED_EVENT_11,
    TAG_ACTOR_ANIMATION_SCRIPTED_EVENT_12,
};

uint32 sArrIdleAnimTags[CRAFTER_RANKS] =
{
    TAG_ACTOR_ANIMATION_NEUTRAL,
    TAG_ACTOR_ANIMATION_GIZMO_STATE_1,
    TAG_ACTOR_ANIMATION_GIZMO_STATE_2,
    TAG_ACTOR_ANIMATION_GIZMO_STATE_3,
    TAG_ACTOR_ANIMATION_GIZMO_STATE_4,
    TAG_ACTOR_ANIMATION_GIZMO_STATE_5,
    TAG_ACTOR_ANIMATION_GIZMO_STATE_6,
    TAG_ACTOR_ANIMATION_GIZMO_STATE_7,
    TAG_ACTOR_ANIMATION_GIZMO_STATE_8,
    TAG_ACTOR_ANIMATION_GIZMO_STATE_9,
    TAG_ACTOR_ANIMATION_GIZMO_STATE_10,
    TAG_ACTOR_ANIMATION_GIZMO_STATE_11,
    TAG_ACTOR_ANIMATION_GIZMO_STATE_12,
};
}  // namespace ItemCrafting_BackEnd















































void CrafterSavedData::Export(
    D3::ItemCrafting::CrafterSavedData & tData,
    D3::Account::SavedDefinition * ptAccountData) const
{
    for (int32 i = 0; i <= CRAFTER_TYPE_LAST_IMPROVEABLE; i++)
    {
        tCrafterData[i].Export(*tData.add_crafter_data());
    }

    tTransmogData.Export(*tData.mutable_transmog_data(), ptAccountData ? ptAccountData->mutable_account_wide_transmog_data() : NULL);
    tExtractedLegendariesData.Export(*tData.mutable_extracted_legendaries_data());
}


void CrafterSavedData::Import(const D3::ItemCrafting::CrafterSavedData & tData, const D3::Account::SavedDefinition * ptAccountData)
{
    ImportInternal(tData, ptAccountData, false);
}




void CrafterSavedData::ImportNoLogs(const D3::ItemCrafting::CrafterSavedData & tData, const D3::Account::SavedDefinition * ptAccountData)
{
    ImportInternal(tData, ptAccountData, true);
}



// NON_MATCHING: 848 vs 944 bytes; silent/non-silent branches collapse because existing CrafterTransmogData::ImportInternal ignores bSilent; retained original wrapper calls without artificial flag use.
void CrafterSavedData::ImportInternal(
    const D3::ItemCrafting::CrafterSavedData & tData,
    const D3::Account::SavedDefinition * ptAccountData,
    bool bSilent)
{
    GBHandleList listGBHandles(GBGetHandlePool());
    GBEnumerate(GB_RECIPES, listGBHandles);

    for (int32 i = 0; i < CRAFTER_TYPE_COUNT; i++)
    {
        PlayerCrafterInitialize(tCrafterData[i]);
        if (i < tData.crafter_data_size())
        {
            tCrafterData[i].Import(tData.crafter_data(i));
        }
    }




    // Inferred initial upgrade-crafter level 1 from the original store at this row.
    tCrafterData[CRAFTER_TYPE_JEWEL_UPGRADE].nLevel = 1;

    BOOL bHasPartitionTransmogs = tData.has_transmog_data();
    BOOL bHasAccountWideTransmogs = ptAccountData && ptAccountData->has_account_wide_transmog_data();
    if (bHasPartitionTransmogs || bHasAccountWideTransmogs)
    {
        D3::GameBalance::BitPackedGbidArray tEmptyTransmogData;
        if (bSilent)
        {
            tTransmogData.ImportNoLogs(bHasPartitionTransmogs ? tData.transmog_data() : tEmptyTransmogData,
                bHasAccountWideTransmogs ? &ptAccountData->account_wide_transmog_data() : NULL);
        }
        else
        {
            tTransmogData.Import(bHasPartitionTransmogs ? tData.transmog_data() : tEmptyTransmogData,
                bHasAccountWideTransmogs ? &ptAccountData->account_wide_transmog_data() : NULL);
        }
    }
    else
    {
        tTransmogData.SetToDefaults();
    }

    tExtractedLegendariesData.SetToDefaults();
    if (tData.has_extracted_legendaries_data())
    {
        tExtractedLegendariesData.Import(tData.extracted_legendaries_data());
    }
}


void CrafterData::Export(
    D3::ItemCrafting::CrafterData & tData) const
{
    tData.Clear();


    int32 nLeadingNullBytes = 0;
    uint8 arLearnedRecipes[MAX_RECIPES_PER_CRAFTER_BYTE_ARRAY_SIZE];
    int32 nNumBytes = AssetListArrayToBitfield(ASSET_LIST_RECIPE_GBIDS,
                                                gbidRecipes,
                                                nNumRecipes,
                                                arLearnedRecipes,
                                                MAX_RECIPES_PER_CRAFTER_BYTE_ARRAY_SIZE,
                                                &nLeadingNullBytes);


    // Inferred false until the compact bitfield is accepted, from both original rejected-size paths.
    BOOL bUseBitfield = FALSE;
    if (nNumBytes > 0 && nNumBytes < nNumRecipes * sizeof(GBID))
    {
        tData.set_recipes_bitfield(arLearnedRecipes, nNumBytes);
        if (nLeadingNullBytes > 0)
        {
            tData.set_bitfield_leading_null_bytes(nLeadingNullBytes);
        }
        // Inferred true on the accepted-bitfield path from the original later recipe-loop split.
        bUseBitfield = TRUE;
    }

    for (int32 i = 0; i < nNumRecipes; i++)
    {
        int32 nListIndex = AssetListIndexGet(ASSET_LIST_RECIPE_GBIDS, gbidRecipes[i]);
        if (!bUseBitfield || nListIndex == -1)
        {

            ScopedRecipeEntryLockR pdefRecipeEntry(gbidRecipes[i]);
            if (pdefRecipeEntry != NULL && (pdefRecipeEntry->dwFlags & (1 << RECIPE_DROPPED_BIT)))
            {
                tData.add_recipes(gbidRecipes[i]);
            }
        }
    }

    tData.set_level(nLevel);


    // Inferred zero cooldown: original stores zero and sets the protobuf presence bit.
    tData.set_cooldown_end(0);

}


void CrafterData::Import(const D3::ItemCrafting::CrafterData & tData)
{
    nNumRecipes = 0;


    const blz::string & strData = tData.recipes_bitfield();


    if (strData.length() > 0)
    {
        uint8 arLearnedRecipes[MAX_RECIPES_PER_CRAFTER_BYTE_ARRAY_SIZE];
        int32 nNumBytes = Math::Min((int32)strData.length(), (int32)MAX_RECIPES_PER_CRAFTER_BYTE_ARRAY_SIZE);
        SigmaMemoryCopy(arLearnedRecipes, strData.data(), nNumBytes);

        if (nNumBytes > 0)
        {
            int32 nLeadingNullBytes = tData.has_bitfield_leading_null_bytes() ? tData.bitfield_leading_null_bytes() : 0;
            nNumRecipes = AssetListBitfieldToArray(ASSET_LIST_RECIPE_GBIDS, arLearnedRecipes, nNumBytes, gbidRecipes, MAX_RECIPES_PER_CRAFTER, nLeadingNullBytes);
        }
    }

    // also import any recipes saved by gbid



    int32 nNumSavedRecipes = Math::Min(tData.recipes_size(), (int)MAX_RECIPES_PER_CRAFTER);
    for (int32 i = 0; i < nNumSavedRecipes; i++)
    {
        GBID gbidRecipe = tData.recipes(i);
        if (GBIDExists(gbidRecipe, GB_RECIPES))
        {
            ScopedRecipeEntryLockR pdefRecipeEntry(gbidRecipe);
            if (pdefRecipeEntry != NULL && (pdefRecipeEntry->dwFlags & (1 << RECIPE_DROPPED_BIT)))
            {
                gbidRecipes[nNumRecipes] = gbidRecipe;
                nNumRecipes++;
            }
        }
    }

    nLevel = tData.level();
}







void CrafterTransmogData::SetToDefaults()
{
    nUnlockedTransmogs = 0;
    for (int32 i = 0; i < MAX_UNLOCKED_TRANSMOGS; i++)
    {
        gbidUnlockedTransmogs[i] = GBID_NONE;
    }
    nUnlockedAccountTransmogs = 0;
    for (int32 i = 0; i < MAX_UNLOCKED_ACCOUNT_TRANSMOGS_PER_ACCOUNT; i++)
    {
        gbidUnlockedAccountTransmogs[i] = GBID_NONE;
    }
}





void sCrafterTransmogDataExport(D3::GameBalance::BitPackedGbidArray & tData, const GBID * const gbidUnlockedTransmogs, int32 nUnlockedTransmogs)
{
    PackAssetsIntoBitPackedGbidArray(tData, ASSET_LIST_TRANSMOG_GBIDS, (int32 *)gbidUnlockedTransmogs, nUnlockedTransmogs, MAX_UNLOCKED_TRANSMOGS);
}









void CrafterTransmogData::Export(D3::GameBalance::BitPackedGbidArray & tPartitionData, D3::GameBalance::BitPackedGbidArray * ptAccountData) const
{
    if (ptAccountData != NULL)
    {
        // export partition and account-wide transmogs separately


        sCrafterTransmogDataExport(tPartitionData, gbidUnlockedTransmogs, nUnlockedTransmogs);
        sCrafterTransmogDataExport(*ptAccountData, gbidUnlockedAccountTransmogs, nUnlockedAccountTransmogs);
    }
    else
    {
        // no account data, so export everything to the partition
        GBID gbidAllUnlockedTransmogs[MAX_UNLOCKED_TRANSMOGS];
        int32 nAllUnlockedTransmogs = 0;
        if (nUnlockedTransmogs > 0)
        {
            SigmaMemoryCopy(gbidAllUnlockedTransmogs, gbidUnlockedTransmogs, nUnlockedTransmogs * sizeof(GBID));
            nAllUnlockedTransmogs = nUnlockedTransmogs;
        }

        for (int32 i = 0; i < nUnlockedAccountTransmogs; i++)
        {
            gbidAllUnlockedTransmogs[nAllUnlockedTransmogs++] = gbidUnlockedAccountTransmogs[i];
        }

        sCrafterTransmogDataExport(tPartitionData, gbidAllUnlockedTransmogs, nAllUnlockedTransmogs);
    }
}







int32 sCrafterTransmogDataImport(const D3::GameBalance::BitPackedGbidArray & tData, GBID * gbidUnlockedTransmogs, const int32 nMaxUnlockedTransmogs)
{
    GBID gbidUnlockedTransmogsLocal[MAX_UNLOCKED_TRANSMOGS];
    int32 nUnlockedTransmogs = 0;
    int32 nUnlockedTransmogsLocal = UnpackAssetsFromBitPackedGbidArray(tData, ASSET_LIST_TRANSMOG_GBIDS, gbidUnlockedTransmogsLocal, MAX_UNLOCKED_TRANSMOGS);
    for (int32 i = 0; i < nUnlockedTransmogsLocal; i++)
    {
        GBID gbidTransmog = gbidUnlockedTransmogsLocal[i];











        if (ItemCrafting_CanUnlockTransmog(gbidTransmog, NULL, TRUE))
        {
            gbidUnlockedTransmogs[nUnlockedTransmogs++] = gbidTransmog;
        }
    }

    return nUnlockedTransmogs;
}




void CrafterTransmogData::Import(const D3::GameBalance::BitPackedGbidArray & tPartitionData, const D3::GameBalance::BitPackedGbidArray * ptAccountData)
{
    ImportInternal(tPartitionData, ptAccountData, false);
}


// NON_MATCHING: original passes bSilent=1 (w3) to ImportInternal; ours passes undef (DeadArgElim, bSilent unused)
void CrafterTransmogData::ImportNoLogs(
    const D3::GameBalance::BitPackedGbidArray & tPartitionData,
    const D3::GameBalance::BitPackedGbidArray * ptAccountData)
{
    ImportInternal(tPartitionData, ptAccountData, true);
}

void CrafterTransmogData::ImportInternal(const D3::GameBalance::BitPackedGbidArray & tPartitionData, const D3::GameBalance::BitPackedGbidArray * ptAccountData, bool bSilent)
{
    nUnlockedTransmogs = sCrafterTransmogDataImport(tPartitionData, gbidUnlockedTransmogs, MAX_UNLOCKED_PARTITION_TRANSMOGS_PER_ACCOUNT);
    if (ptAccountData != NULL)
    {
        nUnlockedAccountTransmogs = sCrafterTransmogDataImport(*ptAccountData, gbidUnlockedAccountTransmogs, MAX_UNLOCKED_ACCOUNT_TRANSMOGS_PER_ACCOUNT);
    }
    else
    {
        nUnlockedAccountTransmogs = 0;
    }

    // move account-wide transmogs out of the partition list
    for (int32 n = 0; n < nUnlockedTransmogs; )
    {
        GBID gbidPartitionTransmog = gbidUnlockedTransmogs[n];
        ScopedItemDefinitionLockR pdefTransmog(gbidPartitionTransmog);
        if (pdefTransmog != NULL && (pdefTransmog->dwFlags & (1 << ITEM_ACCOUNT_WIDE_TRANSMOG)))
        {
            BOOL bAlreadyKnown = FALSE;
            for (int32 m = 0; m < nUnlockedAccountTransmogs; m++)
            {
                if (gbidPartitionTransmog == gbidUnlockedAccountTransmogs[m])
                {
                    bAlreadyKnown = TRUE;
                    break;
                }
            }

            if (!bAlreadyKnown)
            {
                if (nUnlockedAccountTransmogs == MAX_UNLOCKED_ACCOUNT_TRANSMOGS_PER_ACCOUNT)
                {
                    n++;
                    continue;
                }
                gbidUnlockedAccountTransmogs[nUnlockedAccountTransmogs++] = gbidPartitionTransmog;
            }

            if (n != nUnlockedTransmogs - 1)
            {
                gbidUnlockedTransmogs[n] = gbidUnlockedTransmogs[nUnlockedTransmogs - 1];
            }
            gbidUnlockedTransmogs[nUnlockedTransmogs - 1] = GBID_NONE;
            nUnlockedTransmogs--;
        }
        else
        {
            n++;
        }
    }

    for (int32 n = 0; n < nUnlockedAccountTransmogs; n++)
    {
        GBID gbidAccountTransmog = gbidUnlockedAccountTransmogs[n];
        ScopedItemDefinitionLockR pdefTransmog(gbidAccountTransmog);
    }
}



















BOOL sCrafterIsLockedForPlayerContentLevel(const CrafterType eType, const Player & tPlayer)
{
    uint32 uContentFlags = 0xffffffff;
    if (GameActiveGameCommonDataIsServer())
    {
        uContentFlags = SPlayerGetContentFlags(tPlayer);
    }



















    switch (eType)
    {
    case CRAFTER_TYPE_JEWEL:
        {
            return !(uContentFlags & (1 << OnlineService::CONTENT_LICENSE_LEVEL_60));
        }
    case CRAFTER_TYPE_HORADRIM:
    case CRAFTER_TYPE_JEWEL_UPGRADE:
        {
            return !(uContentFlags & (1 << OnlineService::CONTENT_LICENSE_ADVMODE_ACTV));
        }
    default:
        return FALSE;
    }
}






























int32 ItemCrafting_GetUnidentifiedItemCount(const ActorCommonData & tACDContainer)
{
    int32 nAccumulatedCount = 0;
    for (int32 nIndex = 0; cgtDefaultPlayerInventorySlots[nIndex] != SLOT_NONE; nIndex++)
    {
        ACDInventoryIterateItems(tACDContainer, cgtDefaultPlayerInventorySlots[nIndex], 0, ItemCrafting_BackEnd::GetIdentifyCount, &nAccumulatedCount);
    }

    return nAccumulatedCount;
}




float ItemCrafting_GetJewelUpgradeChance(
    int32 nJewelRank,
    int32 nRiftLevel)
{
    nRiftLevel = nRiftLevel + 1;

    if (nRiftLevel < 0 || nJewelRank >= 150)
    {
        return 0.0f;
    }

    // The chance falls off with the difference between the jewel rank and
    // the rift level, using a separate table when the jewel is below the
    // rift level.
    // Inferred (no rows on 746/747): defaults are the higher-rank table; the negation at 754 has no row
    // of its own (folded into the cneg at 749 col 36); else-return at 763 col 54 implies 8-space indent.
    const float * pflJewelUpgradeChances = scflJewelUpgradeChanceHigherRank;
    int32 nMaxDifference = (int32)(sizeof(scflJewelUpgradeChanceHigherRank) / sizeof(float));

    int32 nDifference = nJewelRank - nRiftLevel;
    if (nDifference < 0)
    {
        pflJewelUpgradeChances = scflJewelUpgradeChanceLowerRank;
        nMaxDifference = (int32)(sizeof(scflJewelUpgradeChanceLowerRank) / sizeof(float));
        nDifference = -nDifference;
    }

    if (nDifference < nMaxDifference)
    {
        return pflJewelUpgradeChances[nDifference];
    }
    else
    {
        return pflJewelUpgradeChances[nMaxDifference - 1];
    }
}


float ItemCrafting_GetJewelUpgradeChance(const ActorCommonData & tACDJewel, int32 nRiftLevel)
{
    if (!ItemIsUpgradeableJewel(tACDJewel))
    {
        return 0.0f;
    }

    int32 nJewelRank = tACDJewel.AttributesGetInt(FastAttribKey(ATTRIBUTE_JEWEL_RANK));
    return ItemCrafting_GetJewelUpgradeChance(nJewelRank, nRiftLevel);
}




BOOL ItemCrafting_TransmuteIsValidIngredientInput(
    const TransmuteRecipeIngredient & tIngredient,
    const TransmuteInput & tInput)
{

    if (!tInput.ptACDItem && tInput.gbidCurrency != GBID_NONE)
    {
        if (tIngredient.eType == TRIT_SPECIFIC_GBID && tInput.gbidCurrency == tIngredient.nTypeValue)
            return TRUE;
        return FALSE;
    }



    GBID gbidItem = tInput.ptACDItem->hGB.gbid;

    switch (tIngredient.eType)
    {
    case TRIT_ANY:
        return TRUE;

    case TRIT_SPECIFIC_GBID:
        if (gbidItem == tIngredient.nTypeValue)
            return TRUE;
        break;



    case TRIT_LEGENDARY:
        if (ItemIsLegendary(gbidItem) && ItemIsEquippableByPlayerOrHireling(*tInput.ptACDItem))
            return TRUE;
        break;



    case TRIT_LEGENDARY_WITH_POWER:
        if (ItemGetLegendaryPower(gbidItem) != SNO_NONE)
        {
            ScopedItemDefinitionLockR pdefItem(gbidItem);
            if (pdefItem && !(pdefItem->dwFlags & (1 << ITEM_CANNOT_EXTRACT_POWER)))
                return TRUE;


        }
        break;

    case TRIT_RARE_EQUIPPABLE_ITEM:
        if (ItemIsEquippableByPlayerOrHireling(*tInput.ptACDItem))
        {
            ItemQualityLevel eIQL = ItemGetQualityLevel(*tInput.ptACDItem);
            if (eIQL >= IQL_RARE_AFFIX_START && eIQL <= IQL_RARE_AFFIX_END)
            {
                int32 nLevelReq = ItemGetLevelRequirement(tInput.ptACDItem->hGB.gbid, tInput.ptACDItem, TRUE);
                // Inferred level-70 threshold from the original compare against70 after the level query.
                if (nLevelReq >= 70)
                    return TRUE;
            }
        }
        break;


    case TRIT_SET_ITEM:
        if (ItemIsSetItem(gbidItem))
            return TRUE;
        break;



    case TRIT_UPGRADED_GEM_OF_EASE:
        if (gbidItem == GlobalGBIDGet(GLOBALGBID_ITEM_GEM_OF_EASE))
        {
            const GameBalanceDefinition * pdefGB;
            ItemDefinition * ptItemDef = (ItemDefinition *)GBLockR(tInput.ptACDItem->hGB, pdefGB, TRUE);
            if (ptItemDef && ptItemDef->nJewelSecondaryEffectUnlockRank > 0)
            {
                int32 nJewelRank = tInput.ptACDItem->AttributesGetInt(FastAttribKey(ATTRIBUTE_JEWEL_RANK));
                if (nJewelRank >= ptItemDef->nJewelSecondaryEffectUnlockRank)
                {
                    GBUnlockR(pdefGB);
                    return TRUE;
                }
            }
            GBUnlockR(pdefGB);
        }
        break;

    case TRIT_EQUIPPABLE:
        if (ItemIsEquippableByPlayerOrHireling(gbidItem))
            return TRUE;
        break;



    case TRIT_NORMAL_MAGIC_RARE_ITEM:
        if (ItemIsEquippableByPlayerOrHireling(gbidItem) && !ItemIsLegendary(gbidItem))
            return TRUE;
        break;



    case TRIT_GEM:
        if (ItemIsGem(gbidItem))
            return TRUE;
        break;



    case TRIT_BASIC_CRAFTING_MAT:
        if (gbidItem == GlobalGBIDGet(GLOBALGBID_ITEM_CRAFTING_NORMAL) ||
            gbidItem == GlobalGBIDGet(GLOBALGBID_ITEM_CRAFTING_MAGIC) ||
            gbidItem == GlobalGBIDGet(GLOBALGBID_ITEM_CRAFTING_RARE))
            return TRUE;
        break;



    case TRIT_ACT_BOUNTY_REAGENT:
        if (ItemIsActBountyReagent(gbidItem))
            return TRUE;
        break;



    case TRIT_GEM_CONVERSION_REAGENT:
        if (ItemIsGemConversionReagent(gbidItem))
            return TRUE;
        break;



    case TRIT_UPGRADED_LEGENDARY_GEM:
        if (ItemIsUpgradeableJewel(gbidItem))
            return TRUE;
        break;




    case TRIT_ANCIENT:
        if (ItemIsAncientRank(*tInput.ptACDItem))
            return TRUE;
        break;



    case TRIT_MAX_RANK_STANDARD_GEM:
        if (gbidItem == GlobalGBIDGet(GLOBALGBID_ITEM_AMETHYST_MAX_RANK) ||
            gbidItem == GlobalGBIDGet(GLOBALGBID_ITEM_EMERALD_MAX_RANK) ||
            gbidItem == GlobalGBIDGet(GLOBALGBID_ITEM_RUBY_MAX_RANK) ||
            gbidItem == GlobalGBIDGet(GLOBALGBID_ITEM_TOPAZ_MAX_RANK))
            return TRUE;
        break;



    case TRIT_BLACKROCK_PAGE:
        if (gbidItem == GlobalGBIDGet(GLOBALGBID_ITEM_BLACKROCK_LEDGER_1) ||
            gbidItem == GlobalGBIDGet(GLOBALGBID_ITEM_BLACKROCK_LEDGER_2) ||
            gbidItem == GlobalGBIDGet(GLOBALGBID_ITEM_BLACKROCK_LEDGER_3) ||
            gbidItem == GlobalGBIDGet(GLOBALGBID_ITEM_BLACKROCK_LEDGER_4) ||
            gbidItem == GlobalGBIDGet(GLOBALGBID_ITEM_BLACKROCK_LEDGER_5) ||
            gbidItem == GlobalGBIDGet(GLOBALGBID_ITEM_BLACKROCK_LEDGER_6))
            return TRUE;
        break;
    }


    // Inferred shared rejection return from original out-of-range and failed ingredient branches.
    return FALSE;


}



BOOL ItemCrafting_TransmuteEvaluate(
    const TransmuteInventory & tInventory,
    const TransmuteRecipeIngredient *const arIngredients,
    const int32 nIngredientsCount,
    TransmuteInventory & tTransmuteInventoryFinal,
    const int32 nIngredientToEvaluate)
{
    int32 nInputsCount = tInventory.arInputs.size();

    if (nIngredientToEvaluate >= nIngredientsCount)
    {

        for (int32 i = 0; i < nInputsCount; i++)
        {
            const TransmuteInput * ptInput = &tInventory.arInputs[i];



            if (ptInput->eIngredientType != TRIT_SPECIFIC_GBID)
            {
                for (int32 t = 0; t < nInputsCount; t++)
                {
                    if (i == t)
                    {
                        continue;
                    }

                    const TransmuteInput * ptOtherInput = &tInventory.arInputs[t];
                    if (ptInput->eIngredientType == ptOtherInput->eIngredientType)
                    {

                        if (!ptInput->ptACDItem || !ptOtherInput->ptACDItem)
                        {
                            return FALSE;
                        }

                        if (ptInput->ptACDItem->hGB.gbid != ptOtherInput->ptACDItem->hGB.gbid)
                        {
                            return FALSE;
                        }
                    }
                }
            }


            if (ptInput->nStackCount > 0 && !ptInput->bUsedInRecipe)
            {



                // Inferred false until a specific-GBID ingredient accepts this unused input.
                BOOL bGBIDMatches = FALSE;
                for (int32 j = 0; j < nIngredientsCount; j++)
                {
                    if (arIngredients[j].eType == TRIT_SPECIFIC_GBID)
                    {
                        if (ptInput->gbidCurrency != GBID_NONE && ptInput->gbidCurrency == arIngredients[j].nTypeValue)
                        {
                            // Inferred match flag and exit from the original shared success branch.
                            bGBIDMatches = TRUE; break;
                        }
                        else if (ptInput->ptACDItem && ptInput->ptACDItem->hGB.gbid == arIngredients[j].nTypeValue)
                        {
                            // Inferred match flag and exit from the original shared success branch.
                            bGBIDMatches = TRUE; break;
                        }
                    }
                }
                // Inferred failed-search rejection from original loop exhaustion.
                if (!bGBIDMatches)
                {
                    return FALSE;
                }


                if (!ptInput->ptACDItem)
                {
                    return FALSE;
                }






                else if (ItemGetMaxStackSize(ptInput->ptACDItem->hGB, NULL, NULL) <= 1)
                {
                    return FALSE;
                }
            }
        }

        tTransmuteInventoryFinal = tInventory;
        return TRUE;
    }


    const TransmuteRecipeIngredient & tIngredientToEvaluate = arIngredients[nIngredientToEvaluate];
    for (int32 nCurStartInput = 0; nCurStartInput < nInputsCount; nCurStartInput++)
    {

        if (tInventory.arInputs[nCurStartInput].nStackCount <= 0)
        {
            continue;
        }


        if (!ItemCrafting_TransmuteIsValidIngredientInput(tIngredientToEvaluate, tInventory.arInputs[nCurStartInput]))
        {
            continue;
        }

        // Inferred zero consumed quantity before considering this candidate start input.
        int64 nQuantityConsumed = 0;
        TransmuteInventory tInventoryCopy = tInventory;
        for (int32 i = nCurStartInput; i < nInputsCount && nQuantityConsumed < tIngredientToEvaluate.nQuantity; i++)
        {
            if (i > nCurStartInput)
            {

                if (tInventory.arInputs[i].nStackCount <= 0)
                {
                    continue;
                }


                if (!ItemCrafting_TransmuteIsValidIngredientInput(tIngredientToEvaluate, tInventory.arInputs[i]))
                {
                    continue;
                }
            }


            int64 nQuantityNeeded = tIngredientToEvaluate.nQuantity - nQuantityConsumed;
            int64 nStackSizeConsumed = Math::Min(nQuantityNeeded, tInventory.arInputs[i].nStackCount);
            tInventoryCopy.arInputs[i].nStackCount -= nStackSizeConsumed;
            if (nStackSizeConsumed > 0)
            {
                tInventoryCopy.arInputs[i].bUsedInRecipe = TRUE;
                tInventoryCopy.arInputs[i].eIngredientType = tIngredientToEvaluate.eType;
            }
            nQuantityConsumed += nStackSizeConsumed;
        }


        if (nQuantityConsumed < tIngredientToEvaluate.nQuantity)
        {
            return FALSE;
        }


        if (ItemCrafting_TransmuteEvaluate(tInventoryCopy, arIngredients, nIngredientsCount, tTransmuteInventoryFinal, nIngredientToEvaluate + 1))
        {
            return TRUE;
        }
    }
    // Inferred rejection after exhausting every candidate starting input.
    return FALSE;
}
















































int32 ItemCrafting_GetDynamicRecipeMaxTargetLevel(int32 nCrafterLevel)
{
    int32 nRequiredLevel = nCrafterLevel * 5 + 10;
    return nRequiredLevel;
}



void ItemCrafting_InitializeForApp()
{
}



void ItemCrafting_DisposeForApp()
{
}



BOOL ItemCrafting_ArtisanCanCraftItem(GBID gbidRecipe, CrafterType eType, const Player * ptPlayer)
{
    int32 nCrafterLevel = GetCraftLevel(eType, *ptPlayer);
    ScopedRecipeEntryLockR pdefRecipeEntry(gbidRecipe);
    return pdefRecipeEntry->nLevel <= nCrafterLevel;
}






BOOL ItemCrafting_CanCraftItem(GBID gbidRecipe, CrafterType eType, const Player * ptPlayer)
{
    if (ptPlayer == NULL || !GBIDExists(gbidRecipe, GB_RECIPES))
    {
        return FALSE;
    }

    if (!ItemCrafting_ArtisanCanCraftItem(gbidRecipe, eType, ptPlayer))
    {
        return FALSE;
    }

    return ItemPlayerHasRecipeReagents(*ptPlayer, gbidRecipe) != FALSE;
}











void ItemCrafting_GetValidAffixes(
    const ItemAffixSpecifier & tSpecifier,
    GBIDArray & arValidAffixes,
    GBID gbidAffixFamily)
{
    ItemAffixSpecifier tAffixSpecifier = tSpecifier;
    arValidAffixes.clear();

    AffixTableEntry * ptSpecifierAffixTableEntries[6];
    const GameBalanceDefinition * ptSpecifierAffixGameBalances[6];

    for (int32 i = 0; i < tSpecifier.nCurrentAffixesCount; ++i)
    {
        if (tSpecifier.arCurrentAffixes[i] == GBID_NONE)
        {
            ptSpecifierAffixTableEntries[i] = NULL;
            ptSpecifierAffixGameBalances[i] = NULL;
        }
        else
        {
            GBHandle hGB(GB_AFFIXES, tSpecifier.arCurrentAffixes[i]);
            ptSpecifierAffixTableEntries[i] = (AffixTableEntry *)GBLockR(hGB, ptSpecifierAffixGameBalances[i], TRUE);
        }
    }

    const ItemAppGlobals * ptItemAppGlobals = sgtAppGlobals.ptItemAppGlobals;
    for (int32 i = 0; i < ptItemAppGlobals->nAffixes; ++i)
    {
        const Affix * ptAffix = &ptItemAppGlobals->ptAffixes[i];
        ScopedItemAffixLockR pdefAffix(ptAffix->gbid);
        // Inferred: the null check has no line of its own (cbz attributed to the inlined lock, 2203).
        if (pdefAffix == NULL)
        {
            continue;
        }
        if (gbidAffixFamily != GBID_NONE &&
            pdefAffix->gbidAffixFamily0 != gbidAffixFamily &&
            pdefAffix->gbidAffixFamily1 != gbidAffixFamily)
        {
            continue;
        }

        tAffixSpecifier.ptAffix = ptAffix;
        if (ItemCanAddAffix(tAffixSpecifier, pdefAffix, ptSpecifierAffixTableEntries))
        {

            GBID gbidAffix = ptAffix->gbid;
            if (tSpecifier.eIQL == IQL_LEGENDARY)
            {
                if (pdefAffix->gbidLegendaryUprankAffix != GBID_NONE &&
                    GBIDExists(pdefAffix->gbidLegendaryUprankAffix, GB_AFFIXES))
                {
                    gbidAffix = pdefAffix->gbidLegendaryUprankAffix;
                }

                AncientRank eAncientRank = tAffixSpecifier.eAncientRank;
                while (eAncientRank > ANCIENT_RANK_NONE)
                {
                    ScopedItemAffixLockR pdefCurAffix(gbidAffix);
                    if (pdefCurAffix == NULL ||
                        pdefCurAffix->gbidLegendaryUprankAffix == GBID_NONE ||
                        !GBIDExists(pdefCurAffix->gbidLegendaryUprankAffix, GB_AFFIXES))
                    {
                        break;
                    }


                    if (eAncientRank != ANCIENT_RANK_PRIMAL_ANCIENT)
                    {
                        gbidAffix = pdefCurAffix->gbidLegendaryUprankAffix;
                    }

                    eAncientRank--;
                }
            }

            arValidAffixes.push_back(gbidAffix);
        }
    }

    for (int32 i = 0; i < tSpecifier.nCurrentAffixesCount; ++i)
    {
        if (ptSpecifierAffixGameBalances[i] != NULL)
        {
            GBUnlockR(ptSpecifierAffixGameBalances[i]);
        }
    }
}



GameError ItemCrafting_CanLearnRecipe(const RecipeEntry * pdefRecipeEntry, const Player * ptPlayer)
{
    if (pdefRecipeEntry->eCrafterType < CRAFTER_TYPE_BLACKSMITH || pdefRecipeEntry->eCrafterType > CRAFTER_TYPE_LAST_IMPROVEABLE)
    {
        return GAMEERROR_ITEMUSE_UNKNOWN;
    }

    const CrafterData & tCrafterSavedData = ptPlayer->tCrafterSavedData.tCrafterData[pdefRecipeEntry->eCrafterType];
    for (int32 i = 0; i < tCrafterSavedData.nNumRecipes; i++)
    {
        if (tCrafterSavedData.gbidRecipes[i] == pdefRecipeEntry->tHeader.gbid)
        {
            return GAMEERROR_ITEMUSE_RECIPE_ALREADY_KNOWN;
        }
    }

    return GAMEERROR_NONE;
}









void ItemCrafting_GetLearnableRecipes(blz::vector<int> & arLearnableRecipes, CrafterType eCrafterType)
{
    POS pos;
    GBHandleList listGBHandles(GBGetHandlePool());
    GBEnumerate(GB_RECIPES, listGBHandles);

    pos = listGBHandles.GetHeadPosition();
    while (pos)
    {
        GBHandle hGB = listGBHandles.GetNext(pos);
        ScopedRecipeEntryLockR pdefRecipeEntry(hGB.gbid);
        if (pdefRecipeEntry &&
            pdefRecipeEntry->eCrafterType >= CRAFTER_TYPE_BLACKSMITH && pdefRecipeEntry->eCrafterType <= CRAFTER_TYPE_LAST_IMPROVEABLE &&
            (eCrafterType == CRAFTER_TYPE_INVALID || pdefRecipeEntry->eCrafterType == eCrafterType) &&
            (pdefRecipeEntry->dwFlags & (1 << RECIPE_DROPPED_BIT)))
        {
            arLearnableRecipes.push_back(hGB.gbid);
        }
    }
}




BOOL ItemCrafting_PlayerKnowsRecipe(const Player & tPlayer, GBID gbidRecipe)
{
    ScopedRecipeEntryLockR pdefRecipeEntry(gbidRecipe);
    if (pdefRecipeEntry == NULL || pdefRecipeEntry->eCrafterType < CRAFTER_TYPE_BLACKSMITH || pdefRecipeEntry->eCrafterType > CRAFTER_TYPE_LAST_IMPROVEABLE)
    {
        return FALSE;
    }

    int32 nCrafterLevel = GetCraftLevel(pdefRecipeEntry->eCrafterType, tPlayer);
    if (pdefRecipeEntry->nLevel <= nCrafterLevel && !(pdefRecipeEntry->dwFlags & (1 << RECIPE_DROPPED_BIT)))
    {
        return TRUE;
    }

    const CrafterData & tCrafterSavedData = tPlayer.tCrafterSavedData.tCrafterData[pdefRecipeEntry->eCrafterType];
    for (int32 i = 0; i < tCrafterSavedData.nNumRecipes; i++)
    {
        if (tCrafterSavedData.gbidRecipes[i] == gbidRecipe)
        {
            return TRUE;
        }
    }

    return FALSE;
}







struct PossessRecipeSearch
{
    GBID gbidRecipeToFind;
    BOOL bFound;
};



static BOOL sPlayerHasRecipe(const InventoryLocation & tInvLoc, void * pCallBackData)
{
    PossessRecipeSearch * ptRecipeSearch = (PossessRecipeSearch *)pCallBackData;
    ActorCommonData & tACDContainer = ACDGet(tInvLoc.idACDContainer);
    ACDID idACDItem = ACDInventoryGetSquareItem(tACDContainer, tInvLoc.eSlot, tInvLoc.pt);
    if (idACDItem != ACDID_INVALID)
    {
        ActorCommonData * ptACDItem = ACDTryToGet(idACDItem);
        if (ptACDItem != NULL && ptACDItem->hGB.gbid == ptRecipeSearch->gbidRecipeToFind)
        {
            ptRecipeSearch->bFound = TRUE;
            return FALSE;
        }
    }

    return TRUE;
}

BOOL ItemCrafting_PlayerPossessesRecipe(const Player & tPlayer, GBID gbidRecipe)
{
    PossessRecipeSearch tRecipeSearch;
    tRecipeSearch.gbidRecipeToFind = gbidRecipe;
    tRecipeSearch.bFound = FALSE;
    ActorCommonData & tACDPlayer = ACDGet(tPlayer.idPlayerACD);

    ACDInventoryIterateSlot(tACDPlayer, SLOT_PLAYER_BACKPACK, NULL, sPlayerHasRecipe, &tRecipeSearch, FALSE, FALSE);
    if (!tRecipeSearch.bFound)
    {
        ACDInventoryIterateSlot(tACDPlayer, SLOT_PLAYER_SHARED_STASH, NULL, sPlayerHasRecipe, &tRecipeSearch, FALSE, FALSE);
    }

    if (PlayerHasOnGroundCraftingPlan(tPlayer, gbidRecipe))
    {
        tRecipeSearch.bFound = TRUE;
    }

    return tRecipeSearch.bFound;
}









BOOL ItemCrafting_PlayerKnowsAllRecipes(const Player & tPlayer, CrafterType eCrafterType)
{
    switch (eCrafterType)
    {
    case CRAFTER_TYPE_BLACKSMITH:
    case CRAFTER_TYPE_JEWEL:
        break;

    default:
        return FALSE;
    }




    blz::vector<int> arLearnableRecipes;
    ItemCrafting_GetLearnableRecipes(arLearnableRecipes, eCrafterType);


    size_t nLearnableRecipesCount = arLearnableRecipes.size();
    for (uint32 i = 0; i < nLearnableRecipesCount; i++)
    {
        if (!ItemCrafting_PlayerKnowsRecipe(tPlayer, arLearnableRecipes[i]))
        {
            return FALSE;
        }
    }

    return TRUE;
}




GBID ItemCrafting_GetTrainingRecipe(CrafterType eType, const Player * ptPlayer)
{
    int32 nLevel = GetCraftLevel(eType, *ptPlayer);
    if (nLevel > 0)
    {
        switch (eType)
        {
        case CRAFTER_TYPE_JEWEL:
            return ItemCrafting_BackEnd::GetJewelerTrainingRecipe(nLevel);
        case CRAFTER_TYPE_BLACKSMITH:
            return ItemCrafting_BackEnd::GetBlackSmithTrainingRecipe(nLevel);
        case CRAFTER_TYPE_MYSTIC:
            return ItemCrafting_BackEnd::GetMysticTrainingRecipe(nLevel);
        }
    }

    return GBID_NONE;
}





BOOL ItemCrafting_CanTrain(Player & tPlayer)
{
    GBID gbidRecipe = ItemCrafting_GetTrainingRecipe(tPlayer.eActiveCrafterType, &tPlayer);
    if (gbidRecipe == GBID_NONE)
    {
        return FALSE;
    }

    if (!ItemPlayerHasRecipeReagents(tPlayer, gbidRecipe))
    {
        return FALSE;
    }

    int32 nCrafterLevel = GetCraftLevel(tPlayer.eActiveCrafterType, tPlayer);
    int32 nMaxLevel = CrafterGetMaxLevel(tPlayer.eActiveCrafterType);
    return nCrafterLevel < nMaxLevel;
}









int32 GetCraftLevel(const CrafterType eType, const Player & tPlayer)
{
    if (!sCrafterIsLockedForPlayerContentLevel(eType, tPlayer) && eType >= 0 && eType < CRAFTER_TYPE_COUNT)
    {
        return tPlayer.tCrafterSavedData.tCrafterData[eType].nLevel;
    }
    return 0;
}










void SetCraftLevel(const CrafterType eType, Player & tPlayer, const int32 nLevel)
{
    if (!sCrafterIsLockedForPlayerContentLevel(eType, tPlayer) && eType >= 0 && eType < CRAFTER_TYPE_COUNT)
    {
        tPlayer.tCrafterSavedData.tCrafterData[eType].nLevel = nLevel;
    }
}












int32 ItemCrafting_GetRecipeCost(GBID gbidRecipe)
{
    ScopedRecipeEntryLockR pdefRecipeEntry(gbidRecipe);
    if (pdefRecipeEntry == NULL)
    {
        return 0;
    }

    return pdefRecipeEntry->nCost;
}


int32 ItemCrafting_GetRepairAllCost(const Player * ptPlayer, const BOOL fEquippedOnly)
{
    ItemCrafting_BackEnd::RepairCostVisitor tVisitor(ptPlayer);
    ActorCommonData & tACDPlayer = ACDGet(ptPlayer->idPlayerACD);
    if (fEquippedOnly)
    {
        for (int32 i = SLOT_PLAYER_BODYSLOT_START; i < SLOT_PLAYER_BODYSLOT_END; i++)
        {
            ACDInventoryIterateItems(tACDPlayer, i, 0, ItemCrafting_BackEnd::RepairCostVisitItem, &tVisitor);
        }
    }
    else
    {
        for (int32 nIndex = 0; cgtDefaultPlayerInventorySlots[nIndex] != SLOT_NONE; nIndex++)
        {
            ACDInventoryIterateItems(tACDPlayer, cgtDefaultPlayerInventorySlots[nIndex], 0, ItemCrafting_BackEnd::RepairCostVisitItem, &tVisitor);
        }
    }

    return tVisitor.nAccumulatedCost;
}






const int32 gsnItemHighLevelThresholdMin = 50;

const int32 gsnItemHighLevelThresholdDelta = 8;

int32 ItemCrafting_GetRepairCost(
    ACDID idACDItem,
    ACDID idACDPlayer)
{
    ActorCommonData * ptACDItem = ACDTryToGet(idACDItem);
    if (ptACDItem == NULL)
    {
        return 0;
    }

    if (!ItemCanBeRepaired(*ptACDItem))
    {
        return 0;
    }

    ActorCommonData * ptACDPlayer = ACDTryToGet(idACDPlayer);
    float flItemDurPerctile = ItemGetDurabilityPercent(*ptACDItem);
    float itemCost = (float)ItemGetCost(*ptACDItem, 1, 0, NULL, TRUE, TRUE);

    ScopedGlobalDefinitionLockR pdefGlobals;
    float flRepairCostScalar = pdefGlobals->flRepairCostScalar;

    ScopedItemDefinitionLockR pdefItem(ptACDItem->hGB.gbid);
    if (pdefItem &&
        pdefItem->nItemLevel >= gsnItemHighLevelThresholdMin)
    {
        float flUnitScalar = (float)(pdefItem->nItemLevel - gsnItemHighLevelThresholdMin);
        flUnitScalar = flUnitScalar / gsnItemHighLevelThresholdDelta;
        flUnitScalar = Math::Pin(flUnitScalar, 0.0f, 1.0f);
        float flRepairCostScalarDelta = pdefGlobals->flRepairCostScalarHighLevel - pdefGlobals->flRepairCostScalar;
        flRepairCostScalarDelta = Math::Max(0.0f, flRepairCostScalarDelta);
        flRepairCostScalar = flRepairCostScalarDelta * flUnitScalar;
        flRepairCostScalar += pdefGlobals->flRepairCostScalar;
    }

    float flRepairCost = 0.5f + (itemCost * (1.f - flItemDurPerctile) * flRepairCostScalar);
    int32 nCost = static_cast<int32>(flRepairCost);


    float flMarkupPct = 1.0f;
    if (ptACDPlayer != NULL)
    {
        flMarkupPct = Math::Max(0.0f, flMarkupPct - ptACDPlayer->AttributesGetFloat(FastAttribKey(ATTRIBUTE_REPAIR_DISCOUNT_PERCENT)));
    }

    nCost = Math::FloatRoundToInt(nCost * flMarkupPct);


    return Math::Max(nCost, 1);


}


float ItemCrafting_GetEnchantAffixLevelCostScalar(
    const ActorCommonData & tACDItem)
{

    int32 nLegendaryItemLevel = tACDItem.AttributesGetInt(FastAttribKey(ATTRIBUTE_ITEM_LEGENDARY_ITEM_LEVEL_OVERRIDE));
    int32 nItemLevel = nLegendaryItemLevel > 0 ? nLegendaryItemLevel : ItemGetItemLevel(tACDItem.hGB.gbid);

    ScopedGameBalanceDefinitionLockR ptExpTableDef(GlobalSNOGet(GLOBALSNO_GB_EXPERIENCE));
    const ExperienceTable & tExpTable = ptExpTableDef->tExperienceTable;

    return tExpTable.ptExperienceLevels[nItemLevel - 1].flGoldCostLevelScalar;

}

float ItemCrafting_GetEnchantAffixUseCountCostScalar(
    const ActorCommonData & tACDItem)
{

    int32 nEnchantUseCount = tACDItem.AttributesGetInt(FastAttribKey(ATTRIBUTE_ENCHANTED_AFFIX_COUNT)) + 1;


    ScopedGameBalanceDefinitionLockR pdefGB(GlobalSNOGet(GLOBALSNO_GB_ENCHANT_ITEM_USE_COUNT_COST_SCALARS));
    const EnchantItemAffixUseCountCostScalarsTable & tCostMultTable = pdefGB->tEnchantItemAffixUseCountCostScalarsTable;
    // Inferred initial key 0 and multiplier 1.0f from the original empty-table and first-entry paths.
    int32 nLastKey = 0;
    float flCostMultiplier = 1.0f;
    int32 nCostMultTableEntryCount = tCostMultTable.serEnchantItemAffixUseCountCostScalars.ElementCount(sizeof(EnchantItemAffixUseCountCostScalar));
    for (int32 i = 0; i < nCostMultTableEntryCount; i++)
    {
        int32 nCurKey = tCostMultTable.ptEnchantItemAffixUseCountCostScalars[i].nUseCount;
        float flCurEntryCostMultiplier = tCostMultTable.ptEnchantItemAffixUseCountCostScalars[i].flCostMultiplier;

        if (nEnchantUseCount < nCurKey)
        {
            if (nLastKey <= 0)
            {
                return flCurEntryCostMultiplier;
            }


            float s = (float)(nEnchantUseCount - nLastKey) / (float)(nCurKey - nLastKey);
            return flCostMultiplier + (flCurEntryCostMultiplier - flCostMultiplier) * s;
        }
        // Inferred shared loop-tail updates from the original previous-key and multiplier register carries.
        nLastKey = nCurKey;
        flCostMultiplier = flCurEntryCostMultiplier;
    }





    return flCostMultiplier;
}

int32 ItemCrafting_GetEnchantAffixCost(
    ActorCommonData & tACDItem)
{
    if (ItemCanEnchantAffix(tACDItem) != IEAE_NONE)
    {
        return 0;
    }

    ScopedItemDefinitionLockR pdefItem(tACDItem.hGB.gbid);
    if (pdefItem == NULL)
    {
        return 0;
    }

    int32 nCost = pdefItem->nEnchantAffixCost;
    int32 nLevelReq = ItemGetLevelRequirement(tACDItem.hGB.gbid, &tACDItem, TRUE);
    if (nLevelReq > 60)
    {
        nCost = pdefItem->nEnchantAffixCostX1;
    }

    float flItemLevelCostMultiplier = ItemCrafting_GetEnchantAffixLevelCostScalar(tACDItem);
    float flUseCountCostMultiplier = ItemCrafting_GetEnchantAffixUseCountCostScalar(tACDItem);
    // Inferred default 1.0f from the original non-crafted branch retaining its initial scalar.
    float flCraftedCostMultiplier = 1.0f;
    if (ItemIsCrafted(tACDItem))
    {
        ScopedGlobalDefinitionLockR pdefGlobals;
        flCraftedCostMultiplier = pdefGlobals->flEnchantCraftedItemCostScalar;
    }

    nCost = (int32)((float)nCost * flItemLevelCostMultiplier * flUseCountCostMultiplier * flCraftedCostMultiplier);

    return nCost;
}


int32 ItemCrafting_GetEnchantAffixReagents(
    ActorCommonData & tACDItem,
    RecipeIngredient * arOutIngredients,
    int32 nOutIngredientsMax)
{
    ScopedItemDefinitionLockR pdefItem(tACDItem.hGB.gbid);
    if (pdefItem == NULL)
    {
        return 0;
    }

    int32 nMaxIngredients = Math::Min(pdefItem->nEnchantAffixIngredientsCount, nOutIngredientsMax);
    int32 nLevelReq = ItemGetLevelRequirement(tACDItem.hGB.gbid, &tACDItem, TRUE);
    if (nLevelReq > 60)
    {
        nMaxIngredients = Math::Min(pdefItem->nEnchantAffixIngredientsCountX1, nOutIngredientsMax);
        for (int32 i = 0; i < nMaxIngredients; ++i)
        {
            arOutIngredients[i] = pdefItem->arEnchantAffixIngredientsX1[i];
        }
    }
    else
    {
        for (int32 i = 0; i < nMaxIngredients; ++i)
        {
            arOutIngredients[i] = pdefItem->arEnchantAffixIngredients[i];
        }
    }
    // Inferred clamp: the original returns max(nMaxIngredients, 0) (bic w19, w22, w22, asr #31 on line 1788).
    return nMaxIngredients > 0 ? nMaxIngredients : 0;
}


int32 ItemCrafting_GetReforgeCost(ACDID idACDItem)
{
    ActorCommonData * ptACDItem = ACDTryToGet(idACDItem);
    if (ptACDItem != NULL && ItemCanBeReforged(*ptACDItem) == IRE_NONE)
    {
        ScopedItemDefinitionLockR pdefItem(ptACDItem->hGB.gbid);
    }

    return 0;
}














int32 ItemCrafting_GetReforgeReagents(ActorCommonData & tACDItem, RecipeIngredient * arOutIngredients)
{
    return 0;
}












GameMessageType ItemCrafting_BackEnd::MessageTypeFromCrafterType(CrafterType eType, BOOL fInitialInfo)
{
    GameMessageType eMessageType = 0;
    switch (eType)
    {
    case CRAFTER_TYPE_BLACKSMITH:   eMessageType = fInitialInfo ? SMSG_CRAFTING_DATA_BLACKSMITH_INITIAL : SMSG_CRAFTING_DATA_BLACKSMITH; break;
    case CRAFTER_TYPE_JEWEL:        eMessageType = fInitialInfo ? SMSG_CRAFTING_DATA_JEWELER_INITIAL : SMSG_CRAFTING_DATA_JEWELER; break;
    case CRAFTER_TYPE_MYSTIC:       eMessageType = fInitialInfo ? SMSG_CRAFTING_DATA_MYSTIC_INITIAL : SMSG_CRAFTING_DATA_MYSTIC; break;
    case CRAFTER_TYPE_JEWEL_UPGRADE: break;
    case CRAFTER_TYPE_HORADRIM:     eMessageType = fInitialInfo ? SMSG_CRAFTING_DATA_HORADRIM_INITIAL : SMSG_CRAFTING_DATA_HORADRIM; break;
    default:

            // invalid crafter type



            DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }
    return eMessageType;
}
CrafterType ItemCrafting_BackEnd::CrafterTypeFromMessageType(GameMessageType eType, BOOL & fInitialInfo)
{
    CrafterType eCrafterType = CRAFTER_TYPE_INVALID;
    switch (eType)
    {

    case SMSG_CRAFTING_DATA_BLACKSMITH:         eCrafterType = CRAFTER_TYPE_BLACKSMITH; fInitialInfo = FALSE; break;
    case SMSG_CRAFTING_DATA_BLACKSMITH_INITIAL: eCrafterType = CRAFTER_TYPE_BLACKSMITH; fInitialInfo = TRUE; break;
    case SMSG_CRAFTING_DATA_JEWELER:            eCrafterType = CRAFTER_TYPE_JEWEL; fInitialInfo = FALSE; break;
    case SMSG_CRAFTING_DATA_JEWELER_INITIAL:    eCrafterType = CRAFTER_TYPE_JEWEL; fInitialInfo = TRUE; break;
    case SMSG_CRAFTING_DATA_MYSTIC:             eCrafterType = CRAFTER_TYPE_MYSTIC; fInitialInfo = FALSE; break;
    case SMSG_CRAFTING_DATA_MYSTIC_INITIAL:     eCrafterType = CRAFTER_TYPE_MYSTIC; fInitialInfo = TRUE; break;
    case SMSG_CRAFTING_DATA_HORADRIM:           eCrafterType = CRAFTER_TYPE_HORADRIM; fInitialInfo = FALSE; break;
    case SMSG_CRAFTING_DATA_HORADRIM_INITIAL:   eCrafterType = CRAFTER_TYPE_HORADRIM; fInitialInfo = TRUE; break;
    case SMSG_CRAFTING_DATA_TEST:
    case SMSG_CRAFTING_DATA_TEST_INITIAL:
        break;
    default:

            // invalid message type








            DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }
    return eCrafterType;
}
void ItemCrafting_BackEnd::RepairCostVisitItem(ACDID idACDSquare, InventorySlotType, void * pUserData)
{
    RepairCostVisitor * ptVisitor = (RepairCostVisitor *)pUserData;
    if (ptVisitor != NULL)
    {
        ptVisitor->nAccumulatedCost += ItemCrafting_GetRepairCost(idACDSquare, ptVisitor->ptPlayer->idPlayerACD);
    }
}





GBID ItemCrafting_BackEnd::GetJewelerTrainingRecipe(int32 nLevel)
{
    CRefString strGBID;
    return GBIDExists(strGBID.Format("Jeweler_Train_Level%d", nLevel), GB_RECIPES);
}

GBID ItemCrafting_BackEnd::GetBlackSmithTrainingRecipe(int32 nLevel)
{
    CRefString strGBID;
    return GBIDExists(strGBID.Format("BlackSmith_Train_Level%d", nLevel), GB_RECIPES);
}

GBID ItemCrafting_BackEnd::GetMysticTrainingRecipe(int32 nLevel)
{
    CRefString strGBID;
    GBID gbid = GBIDExists(strGBID.Format("Mystic_Train_Level%d", nLevel), GB_RECIPES);
    return gbid;
}




















void ItemCrafting_BackEnd::GetIdentifyCount(ACDID idACDSquare, InventorySlotType, void * pUserData)
{
    ActorCommonData & tACDItem = ACDGet(idACDSquare);
    if (ItemIsUnidentified(tACDItem))
    {
        int32 * pResults = (int32 *)pUserData;
        (*pResults)++;
    }
}


































const InventorySlotType sgarTransmogSlots[] =
{
    SLOT_PLAYER_HEAD,
    SLOT_PLAYER_SHOULDERS,
    SLOT_PLAYER_TORSO,
    SLOT_PLAYER_LEFT_HAND,
    SLOT_PLAYER_RIGHT_HAND,
    SLOT_PLAYER_HANDS,
    SLOT_PLAYER_LEGS,
    SLOT_PLAYER_FEET,
};
extern const int32 sgnTransmogSlotsCount = sizeof(sgarTransmogSlots) / sizeof(sgarTransmogSlots[0]);

InventorySlotType ItemCrafting_GetTransmogSlot(GBID gbidItem)
{
    InventorySlotType eTransmogSlot = SLOT_NONE;
    if (ItemHasLabel(gbidItem, ITL_QUIVER))
    {
        return eTransmogSlot;
    }





    for (int32 i = 0; i < sgnTransmogSlotsCount; i++)
    {
        if (ACDInventoryItemAllowedInThisSlotType(gbidItem, sgarTransmogSlots[i]))
        {
            eTransmogSlot = sgarTransmogSlots[i];
            break;
        }
    }

    if (eTransmogSlot == SLOT_PLAYER_LEFT_HAND && ItemHasLabel(gbidItem, ITL_2_HAND))
    {
        eTransmogSlot = SLOT_PLAYER_END;
    }

    return eTransmogSlot;
}
BOOL ItemCrafting_IsValidTransmogSlot(InventorySlotType eSlot)
{
    for (int32 i = 0; i < sgnTransmogSlotsCount; i++)
    {
        if (sgarTransmogSlots[i] == eSlot)
        {
            return TRUE;
        }
    }

    return FALSE;
}



int32 ItemCrafting_CompareMatchingTransmogs(GBID gbidTransmog1, GBID gbidTransmog2)
{
    ScopedItemDefinitionLockR pdefTransmog1(gbidTransmog1);
    ScopedItemDefinitionLockR pdefTransmog2(gbidTransmog2);

    // transmogs that can't be unlocked sort last
    if (pdefTransmog2->dwFlags & (1 << ITEM_CANNOT_UNLOCK_TRANSMOG)) return -1;
    if (pdefTransmog1->dwFlags & (1 << ITEM_CANNOT_UNLOCK_TRANSMOG)) return 1;

    if (pdefTransmog1->nTransmogUnlockCrafterLevel > pdefTransmog2->nTransmogUnlockCrafterLevel) return -1;
    if (pdefTransmog1->nTransmogUnlockCrafterLevel < pdefTransmog2->nTransmogUnlockCrafterLevel) return 1;

    if (pdefTransmog1->nItemLevel > pdefTransmog2->nItemLevel) return -1;
    if (pdefTransmog1->nItemLevel < pdefTransmog2->nItemLevel) return 1;

    if (pdefTransmog1->nTransmogCost > pdefTransmog2->nTransmogCost) return -1;
    if (pdefTransmog1->nTransmogCost < pdefTransmog2->nTransmogCost) return 1;

    if (pdefTransmog1->tHeader.gbid > pdefTransmog2->tHeader.gbid) return -1;
    if (pdefTransmog1->tHeader.gbid < pdefTransmog2->tHeader.gbid) return 1;

    return 0;
}





int32 ItemCrafting_GetTransmogCost(const ActorCommonData & tACDItem, GBID gbidTransmog)
{
    {
        ScopedItemDefinitionLockR pdefItem(tACDItem.hGB.gbid);
        ScopedItemDefinitionLockR pdefTransmog(gbidTransmog);
        if (pdefItem == NULL || (pdefItem->dwFlags & (1 << ITEM_CANNOT_BE_TRANSMOGGED)) ||
            pdefTransmog == NULL || (pdefTransmog->dwFlags & (1 << ITEM_CANNOT_UNLOCK_TRANSMOG)))
        {
            return 0;
        }
    }

    InventorySlotType eTransmogSlot = ItemCrafting_GetTransmogSlot(gbidTransmog);
    if (eTransmogSlot == SLOT_NONE || ItemCrafting_ItemTransmogsMatch(tACDItem.hGB.gbid, gbidTransmog))
    {
        return 0;
    }

    // find the cheapest matching transmog
    GBID gbidTransmogCost = gbidTransmog;
    POS pos;
    GBHandleList listItems(GBGetHandlePool());
    GBEnumerate(GB_ITEMS, listItems);
    pos = listItems.GetHeadPosition();
    while (pos)
    {
        GBID gbidItem = listItems.GetNext(pos).gbid;
        if (gbidItem == gbidTransmog)
        {
            continue;
        }

        ScopedItemDefinitionLockR pdefItem(gbidItem);
        if (!ItemCrafting_CanUnlockTransmog(gbidItem, NULL, FALSE))
        {
            continue;
        }

        InventorySlotType eItemSlot = ItemCrafting_GetTransmogSlot(gbidItem);
        if (eItemSlot == SLOT_NONE || eTransmogSlot != eItemSlot)
        {
            continue;
        }

        if (ItemCrafting_ItemTransmogsMatch(gbidTransmogCost, gbidItem) && ItemCrafting_CompareMatchingTransmogs(gbidTransmogCost, gbidItem) > 0)
        {
            gbidTransmogCost = gbidItem;
        }
    }

    ScopedItemDefinitionLockR pdefTransmogCost(gbidTransmogCost);
    return pdefTransmogCost->nTransmogCost;
}



















BOOL ItemCrafting_IsTransmogUnlocked(GBID gbidTransmog, const Player * ptPlayer, GBID * gbidOutUnlockedTransmog)
{
    ScopedItemDefinitionLockR pdefTransmog(gbidTransmog);
    if (pdefTransmog == NULL)
    {
        return FALSE;
    }

    if (pdefTransmog->nTransmogUnlockCrafterLevel > 0)
    {
        return GetCraftLevel(CRAFTER_TYPE_MYSTIC, *ptPlayer) >= pdefTransmog->nTransmogUnlockCrafterLevel;
    }

    for (int32 i = 0; i < ptPlayer->tCrafterSavedData.tTransmogData.nUnlockedAccountTransmogs; i++)
    {
        ScopedItemDefinitionLockR pdefUnlockedAccountTransmog(ptPlayer->tCrafterSavedData.tTransmogData.gbidUnlockedAccountTransmogs[i]);
        if (pdefUnlockedAccountTransmog != NULL && pdefUnlockedAccountTransmog->snoActor == pdefTransmog->snoActor)
        {
            if (gbidOutUnlockedTransmog != NULL)
            {
                *gbidOutUnlockedTransmog = ptPlayer->tCrafterSavedData.tTransmogData.gbidUnlockedAccountTransmogs[i];
            }
            return TRUE;
        }
    }

    for (int32 i = 0; i < ptPlayer->tCrafterSavedData.tTransmogData.nUnlockedTransmogs; i++)
    {
        ScopedItemDefinitionLockR pdefUnlockedTransmog(ptPlayer->tCrafterSavedData.tTransmogData.gbidUnlockedTransmogs[i]);
        if (pdefUnlockedTransmog != NULL && pdefUnlockedTransmog->snoActor == pdefTransmog->snoActor)
        {
            if (gbidOutUnlockedTransmog != NULL)
            {
                *gbidOutUnlockedTransmog = ptPlayer->tCrafterSavedData.tTransmogData.gbidUnlockedTransmogs[i];
            }
            return TRUE;
        }
    }

    return FALSE;
}





struct TransmogSearchData
{
    SNO snoItemActor;
    BOOL bFound;
};




static void sFindTransmogGBIDInInventory(ACDID idACDSquare, InventorySlotType, void * pUserData)
{
    TransmogSearchData * ptSearchData = (TransmogSearchData *)pUserData;
    ActorCommonData & tACDItem = ACDGet(idACDSquare);
    ptSearchData->bFound |= (ptSearchData->snoItemActor == tACDItem.snoActor);
}




BOOL ItemCrafting_CanUnlockTransmog(GBID gbidTransmog, const Player * ptPlayerRequiredOwner, BOOL bManuallyLearnedOnly)
{
    ScopedItemDefinitionLockR pdefTransmog(gbidTransmog);
    if (pdefTransmog == NULL || (pdefTransmog->dwFlags & (1 << ITEM_CANNOT_UNLOCK_TRANSMOG)) || !SNOFileExists(ACTOR_SNO, pdefTransmog->snoActor) || ItemCrafting_GetTransmogSlot(gbidTransmog) == SLOT_NONE)
    {
        return FALSE;
    }

    if (bManuallyLearnedOnly && pdefTransmog->nTransmogUnlockCrafterLevel > 0)
    {
        return FALSE;
    }

    if (ptPlayerRequiredOwner != NULL)
    {
        TransmogSearchData tSearchData;
        tSearchData.snoItemActor = pdefTransmog->snoActor;
        tSearchData.bFound = FALSE;
        ActorCommonData & tACDPlayer = ACDGet(ptPlayerRequiredOwner->idPlayerACD);
        for (int32 i = SLOT_PLAYER_BACKPACK; !tSearchData.bFound && i < SLOT_PLAYER_BODYSLOT_END; i++)
        {
            ACDInventoryIterateItems(tACDPlayer, i, 0, sFindTransmogGBIDInInventory, &tSearchData);
        }

        if (!tSearchData.bFound)
        {
            ACDInventoryIterateItems(tACDPlayer, SLOT_PLAYER_SHARED_STASH, 0, sFindTransmogGBIDInInventory, &tSearchData);
        }

        if (!tSearchData.bFound)
        {
            return FALSE;
        }
    }

    return TRUE;
}

















BOOL ItemCrafting_ItemTransmogsMatch(GBID gbidItem1, GBID gbidItem2)
{
    if (gbidItem1 == gbidItem2)
    {
        return TRUE;
    }

    if (gbidItem1 == GBID_NONE || gbidItem2 == GBID_NONE)
    {
        return FALSE;
    }

    ScopedItemDefinitionLockR pdefItem1(gbidItem1);
    ScopedItemDefinitionLockR pdefItem2(gbidItem2);
    return pdefItem1 != NULL && pdefItem2 != NULL && pdefItem1->snoActor == pdefItem2->snoActor;
}




BOOL ItemCrafting_ItemTransmogsMatch(const ActorCommonData & tACDItem, GBID gbidItem2)
{
    return ItemCrafting_ItemTransmogsMatch(tACDItem.GetVisualGBID(), gbidItem2);
}




BOOL ItemCrafting_CanTransmogItem(
    const ActorCommonData & tACDItem,
    GBID gbidTransmog,
    const Player * ptPlayer,
    BOOL bCheckUnlocked,
    BOOL bCheckCannotTransmog)
{

    InventorySlotType eTransmogSlot = ItemCrafting_GetTransmogSlot(tACDItem.hGB.gbid);
    if (eTransmogSlot == SLOT_NONE)
    {
        return FALSE;
    }


    if (ItemIsUnidentified(tACDItem))
    {
        return FALSE;
    }


    ScopedItemDefinitionLockR pdefItem(tACDItem.hGB.gbid);
    if (bCheckCannotTransmog && (!pdefItem || (pdefItem->dwFlags & (1 << ITEM_CANNOT_BE_TRANSMOGGED))))
    {
        return FALSE;
    }


    if (gbidTransmog == GBID_NONE)
    {
        return TRUE;
    }


    ScopedItemDefinitionLockR pdefTransmog(gbidTransmog);
    if (pdefTransmog == NULL)
    {
        return FALSE;
    }


    if (ItemCrafting_ItemTransmogsMatch(tACDItem.hGB.gbid, gbidTransmog))
    {
        return TRUE;
    }


    if (bCheckUnlocked && !ItemCrafting_IsTransmogUnlocked(gbidTransmog, ptPlayer, NULL))
    {
        return FALSE;
    }


    if (eTransmogSlot != ItemCrafting_GetTransmogSlot(gbidTransmog))
    {
        return FALSE;
    }
    // Inferred: offhand block first (DWARF lexical-block order, lines 2342-2348); all slot compares sit on 2340, so the
    // else-if at 2353 has no row of its own. 2303 col 44 (|| after "(!pdefItem") implies the ! spelling.
    if (eTransmogSlot == SLOT_PLAYER_RIGHT_HAND)
    {
        BOOL bIsSpecialOffhand = ItemHasLabel(tACDItem.hGB.gbid, ITL_ORB) || ItemHasLabel(tACDItem.hGB.gbid, ITL_MOJO) || ItemHasLabel(tACDItem.hGB.gbid, ITL_OFFHAND_NECROMANCER);
        BOOL bIsShield = ItemHasLabel(tACDItem.hGB.gbid, ITL_SHIELD) || ItemHasLabel(tACDItem.hGB.gbid, ITL_SHIELD_CRUSADER);

        BOOL bTransmogIsSpecialOffhand = ItemHasLabel(gbidTransmog, ITL_ORB) || ItemHasLabel(gbidTransmog, ITL_MOJO) || ItemHasLabel(gbidTransmog, ITL_OFFHAND_NECROMANCER);
        BOOL bTransmogIsShield = ItemHasLabel(gbidTransmog, ITL_SHIELD) || ItemHasLabel(gbidTransmog, ITL_SHIELD_CRUSADER);

        if (bIsSpecialOffhand != bTransmogIsSpecialOffhand || bIsShield != bTransmogIsShield)
        {
            return FALSE;
        }
    }
    else if (eTransmogSlot == SLOT_PLAYER_END || eTransmogSlot == SLOT_PLAYER_LEFT_HAND)
    {

        ScopedActorDefinitionLockR pdefItemActor(pdefItem->snoActor);
        WeaponClass eWeaponClass = (WeaponClass)TagMapGetInt(pdefItemActor->hTagMap, TAG_ACTOR_WEAPON_CLASS);

        ScopedActorDefinitionLockR pdefTransmogActor(pdefTransmog->snoActor);
        WeaponClass eTransmogWeaponClass = (WeaponClass)TagMapGetInt(pdefTransmogActor->hTagMap, TAG_ACTOR_WEAPON_CLASS);

        if (eWeaponClass != eTransmogWeaponClass)
        {
            return FALSE;
        }
    }

    return TRUE;
}














































int32 ItemCrafting_GetGemRemovalCost(const ActorCommonData & tACDItem)
{
    int32 nSocketCost = 0;
    return nSocketCost;
}






int32 ItemCrafting_GetGemRemovalCostForNewSocketCount(const ActorCommonData & tACDItem, int32 nNewSocketCount)
{
    int32 nUnsocketCost = 0;
    return nUnsocketCost;
}






















int32 ItemCrafting_GetGemQuality(const GBID gbidItem)
{
    ScopedItemDefinitionLockR ptDefItem(gbidItem);
    for (int32 nAttribIndex = 0; nAttribIndex < (int32)(ptDefItem->serAttributes_Console.dwSize / sizeof(AttributeSpecifier)); nAttribIndex++)
    {
        const AttributeSpecifier & tAttribute = ptDefItem->ptAttributes[nAttribIndex];
        if (tAttribute.eAttribute == ATTRIBUTE_GEM_QUALITY)
        {
            return (int32)ItemGetAttributeSpecifierValue(gbidItem, tAttribute, NULL, NULL, FALSE, FALSE);
        }
    }

    return -1;
}







int32 ItemCrafting_GetGemTier(const ActorCommonData & tACDGem)
{
    const int32 GEM_QUALITIES[10] = { 3, 4, 7, 8, 11, 15, 16, 17, 18, 19 };

    int32 nTier = 0;
    int32 nGemQuality = tACDGem.AttributesGetInt(FastAttribKey(ATTRIBUTE_GEM_QUALITY));
    for (int32 i = 0; i < 10; i++)
    {
        if (GEM_QUALITIES[i] == nGemQuality)
        {
            nTier = i + 1;
            break;
        }
    }

    return nTier;
}






int32 CrafterGetMaxLevel(const CrafterType eType)
{
    return cgnCrafterLevelsPerRank[eType] * (CRAFTER_RANKS - 2) + 1;
}




uint32 CrafterGetRank(const CrafterType eType, const uint32 nLevel)
{
    if (nLevel == 0)
    {
        return 0;
    }

    if (cgnCrafterLevelsPerRank[eType] == 0)
    {
        return 1;
    }

    return (nLevel - 1) / cgnCrafterLevelsPerRank[eType] + 1;
}




uint32 CrafterGetLevelUpAnim(uint32 nRank)
{
    return ItemCrafting_BackEnd::sArrLevelUpAnimTags[nRank];
}




uint32 CrafterGetIdleAnim(uint32 nRank)
{
    return ItemCrafting_BackEnd::sArrIdleAnimTags[nRank];
}
