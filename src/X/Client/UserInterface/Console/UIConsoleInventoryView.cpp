#include "UIConsoleInventoryView.h"
#include "UIConsoleInventoryScreen.h"
#include "../Controls/UITextControl.h"
#include "../UI.h"
#include "../../Sound/Sound.h"
#include "../../CGame.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/Definitions/GameBalanceDefinition.h"
#include "../../../Common/Definitions/RecipeDefinition.h"
#include "../../../Common/ActorInventory.h"
#include "../../../Common/ItemsSwig.h"
#include "../../../Common/Items.h"
#include "../../../Common/Attributes/FastAttribConstantsSwig.h"
#include "../../../Sigma/UnicodeText.h"
#include "../../../Common/Definitions/StringListDefinition.h"
#include "../../../Common/SNO/SymbolicValue.h"
#include "../../../Sigma/BlzExt/BlzVectorHelper.h"
#include "../Interface/UICraftingCommon.h"
#include "../../../Common/UserInterface/UIInventory.h"

CRefString LocalizedSprintf(LPCSTR szFormat, ...);  // X/Sigma/Unicode.cpp
GBIDArray PlayerGetUnlockedPortraitsForParagonLevel(int32 nLevel);  // X/Common/Player.cpp:5490

void ACDDelete(ACDID idACD, BOOL fKeepInventoryItems);  // X/Common/ActorCommonData.cpp
GBID CurrencyGetGBID(const CurrencyType eCurrencyType);  // X/Common/Items.cpp:10429
// X/Common/Items.h
BOOL ItemIsSetItem(GBID gbid);
ItemQualityLevel ItemDetermineQualityLevel(LPCSTR szDebugStr, GBHandle hGBItem, const GBID * ptGBIDAffixes, const int32 nAffixCount, const int32 nBonusRandomAffixes);
RGBAColor ItemGetDisplayNameColor(const ItemQualityLevel eIQL);
extern const InventorySlotType cgtCraftingPlayerInventorySlots[3];  // X/Common/Items.cpp:225; the next line X/Common/Items.h
BOOL ItemMeetsOwnerClassRestrictions(const GBID gbidItem, const PlayerClass ePlayerClass, BOOL bCheckLegendaryClassDropWeight);
void Set_Text_Color(CRefString & usString, const RGBAColor & rgbaColor);  // X/Common/UserInterface/UITextControl.cpp
// Defined below at the original lines 1841-1956.
namespace {
CRefString sGetRecipeName(const UIInventory::Console::UIInventoryViewItemsProvider & tItemsProvider, const GBID gbidRecipe, const int32 nIndex);
ACDID sGenerateCraftingMaterialFromCurrency(ACDNetworkName ann, CurrencyType eCurrency, ActorCommonData * ptACDPlayer);
}

namespace UIInventory { namespace Console {
// UIConsoleInventoryViewUtilities.cpp:2567.
CRefString GetItemDescription(const ACDID idACDOwner, const ACDID idACDItem, const BOOL bShowStatRanges);
// Defined below at the original lines 1705, 1745, 1773 and 1798.
int JewelerRecipeSortPredicate(const void * gbidArg1, const void * gbidArg2);
int BlacksmithRecipeSortPredicate(const void * gbidArg1, const void * gbidArg2);
int CosmeticItemSortPredicate(const void * gbidArg1, const void * gbidArg2);
int MysticRecipeSortPredicate(const void * gbidArg1, const void * gbidArg2);
// UIConsoleInventoryViewUtilities.cpp:1613.
ACDID GenerateLeaderboardEquippedItem(const D3::Leaderboard::HeroSnapshot & tHeroSnapshot, const UIFilterType eFilterType, blz::vector<ACDID, blz::allocator<ACDID> > & vecSocketACDs);



CRefString UIInventoryViewItemsProvider::GetItemDescription(
    const ACDID idACDItem,
    const BOOL bShowStatRanges) const
{
    return UIInventory::Console::GetItemDescription(m_idACDOwner, idACDItem, bShowStatRanges);
}




void InventoryItems::GetBagSpace(
    int32 & nUsed,
    int32 & nTotal)
{
    nUsed = 0;
    nTotal = 0;

    Player * ptPlayer = GetPlayer();
    if (!ptPlayer) return;

    ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
    if (!ptACDPlayer) return;
    if (m_eProviderMode == PROVIDERMODE_BACKPACK || m_eProviderMode == PROVIDERMODE_BACKPACK_AND_STASH)
    {
        nUsed += ACDInventoryGetNumFilledSlots(*ptACDPlayer, SLOT_PLAYER_BACKPACK);


        IVector2D tSlotSize;
        ACDInventorySlotGetSize(SLOT_PLAYER_BACKPACK, tSlotSize);
        nTotal += tSlotSize.x * tSlotSize.y;
    }
    if (m_eProviderMode == PROVIDERMODE_STASH || m_eProviderMode == PROVIDERMODE_BACKPACK_AND_STASH)
    {
        nUsed += ACDInventoryGetNumFilledSlots(*ptACDPlayer, SLOT_PLAYER_SHARED_STASH);
        nTotal += ptACDPlayer->AttributesGetInt(FastAttribKey(ATTRIBUTE_SHARED_STASH_SLOTS));
    }


}




void InventoryItems::Refresh()

{
    if (!GetPlayer()) return;




    ACDID idACDPlayer = GetPlayer()->idPlayerACD;



    m_tView.ClearItems();

    switch (m_eProviderMode)
    {
    case PROVIDERMODE_BACKPACK:
    case PROVIDERMODE_STASH:
    case PROVIDERMODE_BACKPACK_AND_STASH:
    {
        uint32 dwFlags = 0;
        if (m_dwItemOptions & (1 << SORT_EQUIPPED_ITEMS_LAST)) dwFlags |= 1 << FilteredInventoryView::FIV_SHOW_EQUIPPED_LAST_BIT;
        if (m_dwItemOptions & (1 << INCLUDE_EQUIPPED_ITEMS)) dwFlags |= 1 << FilteredInventoryView::FIV_INCLUDE_EQUIPPED_BIT;
        if (!(m_dwItemOptions & (1 << EXCLUDE_UNIDENTIFIED_ITEMS))) dwFlags |= 1 << FilteredInventoryView::FIV_INCLUDE_UNIDENTIFIED_BIT;
        if (!(m_dwItemOptions & (1 << EXCLUDE_JUNK_ITEMS))) dwFlags |= 1 << FilteredInventoryView::FIV_INCLUDE_JUNK_BIT;
        if (!(m_dwItemOptions & (1 << EXCLUDE_PLAYER_GIFTS))) dwFlags |= 1 << FilteredInventoryView::FIV_INCLUDE_PLAYER_GIFTS_BIT;
        if (m_eProviderMode == PROVIDERMODE_BACKPACK || m_eProviderMode == PROVIDERMODE_BACKPACK_AND_STASH)
            m_tView.AddItems(idACDPlayer, idACDPlayer, SLOT_PLAYER_BACKPACK, dwFlags, m_eMinimumItemQuality);
        if (m_eProviderMode == PROVIDERMODE_STASH || m_eProviderMode == PROVIDERMODE_BACKPACK_AND_STASH)
            m_tView.AddItems(idACDPlayer, idACDPlayer, SLOT_PLAYER_SHARED_STASH, dwFlags & ~(1 << FilteredInventoryView::FIV_INCLUDE_EQUIPPED_BIT), m_eMinimumItemQuality);
        break;
    }
    case PROVIDERMODE_FOLLOWER:
    {
        uint32 dwFlags = 1 << FilteredInventoryView::FIV_INCLUDE_UNIDENTIFIED_BIT;
        if (m_dwItemOptions & (1 << INCLUDE_EQUIPPED_ITEMS)) dwFlags |= 1 << FilteredInventoryView::FIV_INCLUDE_EQUIPPED_FOLLOWER_BIT;
        m_tView.AddItems(idACDPlayer, idACDPlayer, SLOT_PLAYER_BACKPACK, dwFlags, m_eMinimumItemQuality);
        break;
    }
    case PROVIDERMODE_REPAIR:
        m_tView.AddItems(idACDPlayer, idACDPlayer, SLOT_PLAYER_BACKPACK, (1 << FilteredInventoryView::FIV_INCLUDE_EQUIPPED_BIT) | (1 << FilteredInventoryView::FIV_ONLY_REPAIRABLE_BIT), m_eMinimumItemQuality); break;
    case PROVIDERMODE_TRADE_SELECT:
        m_tView.AddItems(idACDPlayer, idACDPlayer, SLOT_PLAYER_BACKPACK, (1 << FilteredInventoryView::FIV_EXCLUDE_OFFERED_ITEMS_BIT) | (1 << FilteredInventoryView::FIV_EXCLUDE_ACCOUNT_BOUND_BIT), m_eMinimumItemQuality); break;
    case PROVIDERMODE_MAIL_SELECT:
        m_tView.AddItems(idACDPlayer, idACDPlayer, SLOT_PLAYER_BACKPACK, 0, m_eMinimumItemQuality); break;
    case PROVIDERMODE_MYSTIC_ENCHANT:
        m_tView.AddItems(idACDPlayer, idACDPlayer, SLOT_PLAYER_BACKPACK, (1 << FilteredInventoryView::FIV_INCLUDE_EQUIPPED_BIT) | (1 << FilteredInventoryView::FIV_EXCLUDE_NON_ENCHANTABLE_BIT), m_eMinimumItemQuality); break;
    }





































}

// The DWARF gives this definition no line; its DIE follows InventoryItems::Refresh.
const uint32 UIInventoryView::IDENTIFYING_DURATION = 150;

CRefString RecipeItemsProvider::GetItemName(const int32 nIndex) const

{
    GBID gbidRecipe = GetRecipes()[nIndex];
    return sGetRecipeName(*this, gbidRecipe, nIndex);
}
} }
CRefString ItemGetItemTypeDescription(const ActorCommonData & tACDItem, const PlayerClass ePlayerClass); CRefString ItemGetArmorTotalDescription(const ActorCommonData & tACD, const PlayerClass ePlayerClass, ItemDescriptionColorOverride pfnColorOverride, void * ptColorOverrideData); CRefString ItemGetDPSTotalDescription(const ActorCommonData & tACD, const PlayerClass ePlayerClass, ItemDescriptionColorOverride pfnColorOverride, void * ptColorOverrideData); CRefString ItemGetAttacksPerSecond(const ActorCommonData & tACD, const PlayerClass ePlayerClass, ItemDescriptionColorOverride pfnColorOverride, void * ptColorOverrideData); CRefString ItemGetBlockStats(const ActorCommonData & tACD, const PlayerClass ePlayerClass, ItemDescriptionColorOverride pfnColorOverride, void * ptColorOverrideData); CRefString ItemGetPrimaryDescription(const ActorCommonData & tACD);  // X/Client/CItems.cpp:896; X/Common/Items.cpp:11362, 11826, 11851, 11721, 11319
extern LPCSTR guszItemLabelRandomProps; extern LPCSTR guszItemLabelRandomPropsRange; void Set_Text_Color(CRefString & usString, FontSpecifier efcDisplayColor); namespace LanguageRules { BOOL PreferShorterMagicNames(); } namespace UIUtils { namespace Console { CRefString GetStatsFromAffix(GBID gbidAffix, GBID gbidItem, const PlayerClass ePlayerClass); } } namespace { CRefString sGetRecipeItemAttributeDescription(const ItemSpecifierData & tSpecificItem, const PlayerClass ePlayerClass); }  // X/Common/Items.cpp:334-335; X/Common/UserInterface/UITextControl.h:14; X/Sigma/LanguageRules.cpp; UIConsoleUtils.cpp:1147; defined below at 1893
namespace UIInventory { CRefString GetRequirementsTooltip(const ActorCommonData * ptACDPlayer, const ActorCommonData & tACDItem, GBID gbidRecipe); namespace Console {  // X/Client/UserInterface/Interface/UIInventory.cpp:4434
CRefString RecipeItemsProvider::GetItemDescription(
    const ACDID idACDItem,
    const BOOL bShowStatRanges) const
{


    int32 nIndex = GetIndexOfItem(idACDItem);
    if (nIndex == -1)
    {
        return "";
    }

    ActorCommonData * ptACDItem = ACDTryToGet(idACDItem);
    if (ptACDItem == NULL)
    {
        return "";
    }

    if (ItemIsGem(*ptACDItem))
    {

        return UIInventoryViewItemsProvider::GetItemDescription(idACDItem, FALSE);
    }

    const int32 MAX_SECTIONS = 8;
    CRefString sTextSections[MAX_SECTIONS];
    int32 nCurrentSection = 0;

    Player * ptPlayer = GetPlayer();
    PlayerClass ePlayerClass = PlayerGetPlayerClass(ptPlayer);
    ActorCommonData & tPlayerACD = ACDGet(ptPlayer->idPlayerACD);


    if (LanguageRules::PreferShorterMagicNames())
    {
        ItemQualityLevel eIQL = ItemGetQualityLevel(*ptACDItem);
        if (eIQL <= IQL_MAGIC_AFFIX_END)
        {
            sTextSections[nCurrentSection] += ItemGetDisplayName(*ptACDItem, TRUE, FALSE, FALSE, FALSE);
            sTextSections[nCurrentSection] += "\n{icon:Console_PartyDivider}\n";
        }
    }


    sTextSections[nCurrentSection] += ItemGetItemTypeDescription(*ptACDItem, PlayerGetPlayerClass(ptPlayer));
    sTextSections[nCurrentSection] += "\n";

    // inferred: the section index advances between groups (the stores below go to sTextSections[1..6])
    nCurrentSection++;


    if (ItemHasLabel(ptACDItem->hGB.gbid, ITL_ARMOR) || ItemHasLabel(ptACDItem->hGB.gbid, ITL_SHIELD))
    {
        CRefString sAttribute = LocalizedSprintf(StringListFetch("ConsoleInventory:ArmorFormatted").str(),
            ItemGetArmorTotalDescription(*ptACDItem, ePlayerClass, NULL, NULL).str(), NULL);

        sTextSections[nCurrentSection] += sAttribute;
        sTextSections[nCurrentSection] += "\n";
    }
    else if (ItemHasLabel(ptACDItem->hGB.gbid, ITL_WEAPON))
    {
        CRefString sAttribute = LocalizedSprintf(StringListFetch("ConsoleInventory:DPSFormatted").str(),
            ItemGetDPSTotalDescription(*ptACDItem, ePlayerClass, NULL, NULL).str(), NULL);

        sTextSections[nCurrentSection] += sAttribute;
        sTextSections[nCurrentSection] += "\n";
    }


    if (ItemHasLabel(ptACDItem->hGB.gbid, ITL_WEAPON))
    {
        sTextSections[nCurrentSection] += ItemGetAttacksPerSecond(*ptACDItem, ePlayerClass, NULL, NULL);
        sTextSections[nCurrentSection] += "\n";
    }
    else if (ItemHasLabel(ptACDItem->hGB.gbid, ITL_ARMOR) ||
        ItemHasLabel(ptACDItem->hGB.gbid, ITL_SHIELD))
    {
        CRefString sBlockStats = ItemGetBlockStats(*ptACDItem, ePlayerClass, NULL, NULL);
        if (sBlockStats.Length() > 0 && sBlockStats != "\n")
        {
            sTextSections[nCurrentSection] += sBlockStats;
            sTextSections[nCurrentSection] += "\n";
        }
    }

    nCurrentSection++;


    CRefString sPrimaryDescription = ItemGetPrimaryDescription(*ptACDItem);
    if (sPrimaryDescription.Length())
    {
        sTextSections[nCurrentSection] += sPrimaryDescription;
        sTextSections[nCurrentSection] += "\n";
    }

    nCurrentSection++;




    if (!ItemHasLabel(ptACDItem->hGB.gbid, ITL_WEAPON) &&
        !ItemHasLabel(ptACDItem->hGB.gbid, ITL_ARMOR) &&
        !ItemHasLabel(ptACDItem->hGB.gbid, ITL_SHIELD))
    {
        CRefString usItemDescription = ItemGetAttributeDescriptions(*ptACDItem, ePlayerClass, FALSE, TRUE, NULL, NULL, TRUE, tPlayerACD.id);
        if (usItemDescription.Length())
        {
            sTextSections[nCurrentSection] += usItemDescription;
        }
    }


    GBID gbidRecipe = GetRecipes()[nIndex];
    ScopedRecipeEntryLockR pdefRecipeEntry(gbidRecipe);
    if (pdefRecipeEntry)
    {
        ScopedRecipeDefinitionLockR pdefRecipe(pdefRecipeEntry->snoRecipe);

        CRefString sAffixes = sGetRecipeItemAttributeDescription(pdefRecipe->tSpecificItem, ePlayerClass);
        sTextSections[nCurrentSection] += sAffixes;
        sTextSections[nCurrentSection] += "\n";
    }

    nCurrentSection++;


    if (ItemIsSetItem(ptACDItem->hGB.gbid))
    {
        Player * ptPlayerScoped = GetPlayer();
        ACDID idACDOwner = ptPlayerScoped ? ptPlayerScoped->idPlayerACD : ACDID_INVALID;
        if (idACDOwner != ACDID_INVALID)
        {
            ActorCommonData & tACDOwner = ACDGet(idACDOwner);
            sTextSections[nCurrentSection] += GetSetItemTooltip(&tACDOwner, ePlayerClass, *ptACDItem, NULL);
        }
    }

    nCurrentSection++;



    {
        gbidRecipe = GetRecipes()[nIndex];
        ScopedRecipeEntryLockR pdefRecipeEntryScoped(gbidRecipe);
        ScopedRecipeDefinitionLockR pdefRecipe(pdefRecipeEntryScoped->snoRecipe);

        CRefString sIngredientDescription;
        for (int32 i = 0; i < pdefRecipeEntryScoped->nNumIngredients; ++i)
        {
            CRefString sGBIDName = GBIDToString(pdefRecipeEntryScoped->tIngredient[i].gbidItem);
            if (StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CONSOLE_INGREDIENT_TIPS), sGBIDName.str(), sIngredientDescription))
            {
                sTextSections[nCurrentSection] += sIngredientDescription;
                sTextSections[nCurrentSection] += "\n";
            }
        }
    }

    nCurrentSection++;


    CRefString sRequirements = GetRequirementsTooltip(&tPlayerACD, *ptACDItem, GBID_NONE);
    if (sRequirements.Length())
    {
        sTextSections[nCurrentSection] += sRequirements;
    }

    nCurrentSection++;



    CRefString sItemDescription;
    for (int32 i = 0; i < MAX_SECTIONS; ++i)
    {
        if (sTextSections[i].Length())
        {
            if (sItemDescription.Length())
            {

                sItemDescription += "\n";
            }
            sItemDescription += sTextSections[i];
        }
    }

    return sItemDescription;
}



int32 RecipeItemsProvider::GetIndexOfItem(
    const ACDID idItem) const
{
    return -1;
}





UIInventoryViewItemsProvider::ProviderType RecipeItemsProvider::GetProviderType() const
{
    return ITEM_PROVIDER_BLACKSMITH_RECIPES;
}






BOOL RecipeItemsProvider::CanProvideUnidentifiedItems() const

{
    return FALSE;
}




void RecipeItemsProvider::OnPlayerChanged()

{
    Refresh();
}




void CrafterRecipes::CleanupACDs()
{

    for (int32 i = 0; i < FILTER_RECIPES_COUNT; ++i)
    {
        for (int32 j = 0; j < m_arrItems[i].size(); ++j)
        {
            ACDDelete(m_arrItems[i][j], FALSE);
        }
        m_arrItems[i].clear();
    }
}





UIInventoryViewItemsProvider::ProviderType CrafterRecipes::GetProviderType() const

{
    if (GetPlayer() && GetPlayer()->eActiveCrafterType == 1) return ITEM_PROVIDER_JEWELER_RECIPES;





    return ITEM_PROVIDER_BLACKSMITH_RECIPES;
}



BOOL CrafterRecipes::CanProvideUnidentifiedItems() const

{
    return FALSE;
}




void CrafterRecipes::Refresh()

{
    CleanupACDs();

    Player * ptPlayer = GetPlayer();
    if (ptPlayer)
    {
        for (int32 i = 0; i < FILTER_RECIPES_COUNT; ++i)
        {
            m_arrRecipes[i].clear();
            m_arrItems[i].clear();
        }

        int32 nCrafterLevel = GetCraftLevel(ptPlayer->eActiveCrafterType, *ptPlayer);

        GBIDArray arrRecipes;
        UICrafting::BuildRecipeListForCrafter(arrRecipes, ptPlayer, ptPlayer->eActiveCrafterType, NULL);
        // The original starts the generated items' network names at 0x2f74; no named constant found.
        ACDNetworkName ann = (ACDNetworkName)12148;


        for (uint32 i = 0; i < arrRecipes.size(); ++i)
        {
            GBID gbidRecipe = arrRecipes[i];

            ScopedRecipeEntryLockR pdefRecipeEntry(gbidRecipe);
            if (!pdefRecipeEntry || !SNOFileExists(RECIPE_SNO, pdefRecipeEntry->snoRecipe))
            {
                continue;
            }


            ScopedRecipeDefinitionLockR pdefRecipe(pdefRecipeEntry->snoRecipe);
            if (!GBIDExists(pdefRecipe->tSpecificItem.gbidItem, GB_ITEMS))
            {
                continue;
            }

            ScopedItemDefinitionLockR pdefItem(pdefRecipe->tSpecificItem.gbidItem);
            if (!pdefItem)
            {
                continue;
            }


            GBID gbidItem = pdefRecipe->tSpecificItem.gbidItem;
            for (int32 j = 0; j < FILTER_RECIPES_COUNT; ++j)
            {
                UIFilterType eFilter = j + FILTER_RECIPES_START;
                if (FilterIncludesItem(gbidItem, eFilter))
                {
                    if (OptionsApplyToItem(*ptPlayer, gbidRecipe, pdefRecipe->tSpecificItem.gbidItem, m_dwRecipeOptions))
                    {
                        int32 nIndex = eFilter - FILTER_RECIPES_START;


                        if (pdefRecipeEntry->dwFlags & (1 << RECIPE_DYNAMIC_RLEVEL_BIT))
                        {

                            if (pdefRecipeEntry->nLevel > nCrafterLevel)
                            {
                                continue;
                            }
                        }

                        m_arrRecipes[nIndex].push_back(gbidRecipe);
                    }
                }
            }
        }

        for (int32 i = 0; i < FILTER_RECIPES_COUNT; ++i)
        {
            PFN_COMPARE fnCompare = GetSortFuncForFilter(FILTER_RECIPES_START + i);
            // Inferred: the original skips the sort on the switch's default path (no row of its own).
            if (fnCompare != NULL)
            {
                blzext::vector_sort(m_arrRecipes[i], fnCompare);
            }


            for (int32 j = 0; j < m_arrRecipes[i].size(); ++j)
            {
                GBID gbidRecipe = m_arrRecipes[i][j];
                ann = (ACDNetworkName)(ann + 1);
                ACDID idACDItem = UICrafting::GenerateItemFromRecipe(ann, gbidRecipe);
                m_arrItems[i].push_back(idACDItem);
            }
        }
    }
}




BOOL CrafterRecipes::OptionsApplyToItem(
    const Player & tPlayer,
    const GBID gbidRecipe,
    const GBID gbidItem,
    const RecipeOptions dwRecipeOptions)
{
    const ActorCommonData & tACDPlayer = ACDGet(tPlayer.idPlayerACD);

    if (dwRecipeOptions & (1 << EXCLUDE_RECIPES_BY_CLASS))
    {
        ScopedItemDefinitionLockR pdefItemDef(gbidItem);
        if (!ItemIsEquippable(gbidItem) ||
            !ItemMeetsOwnerClassRestrictions(gbidItem, PlayerGetPlayerClass(&tPlayer), FALSE))
        {
            return FALSE;
        }
    }

    if (dwRecipeOptions & (1 << EXCLUDE_RECIPES_BY_MATERIALS))
    {
        ScopedRecipeEntryLockR pdefRecipeEntry(gbidRecipe);

        for (int32 i = 0; i < pdefRecipeEntry->nNumIngredients; ++i)
        {
            const RecipeIngredient & tIngredient = pdefRecipeEntry->tIngredient[i];
            int32 nRequired = tIngredient.nQuantity;
            GBHandle hgbItem(GB_ITEMS, tIngredient.gbidItem);
            int32 nOnHand = ACDInventoryGetItemCount(tACDPlayer, hgbItem, cgtCraftingPlayerInventorySlots, TRUE, FALSE);
            if (nOnHand < nRequired)
            {
                return FALSE;
            }
        }

    }

    return TRUE;
}




BOOL CrafterRecipes::FilterIncludesItem(
    const GBID gbidItem,
    const UIFilterType eFilter)
{
    if (ItemHasLabel(gbidItem, ITL_GEM))
    {
        ScopedItemDefinitionLockR pdefGem(gbidItem);

        // The original turns this switch into a five-entry GemType table at 0x7100e3e5d0 (3,4,2,1,5).
        switch (eFilter)
        {
        case FILTER_JEWELER_RECIPES_RUBY:
            return pdefGem->eGemType == GEM_RUBY;
        case FILTER_JEWELER_RECIPES_TOPAZ:
            return pdefGem->eGemType == GEM_TOPAZ;
        case FILTER_JEWELER_RECIPES_EMERALD:
            return pdefGem->eGemType == GEM_EMERALD;
        case FILTER_JEWELER_RECIPES_AMETHYST:
            return pdefGem->eGemType == GEM_AMETHYST;
        case FILTER_JEWELER_RECIPES_DIAMOND:
            return pdefGem->eGemType == GEM_DIAMOND;
        default:
            return FALSE;
        }



    }

    if (ItemHasLabel(gbidItem, ITL_SWORD))
    {
        return eFilter == FILTER_BLACKSMITH_RECIPES_SWORD;
    }

    if ( ItemHasLabel(gbidItem, ITL_CROSSBOW) || ItemHasLabel(gbidItem, ITL_HAND_CROSSBOW) )
    {
        return eFilter == FILTER_BLACKSMITH_RECIPES_CROSSBOW;
    }

    if ( ItemHasLabel(gbidItem, ITL_DAGGER) || ItemHasLabel(gbidItem, ITL_CEREMONIAL_DAGGER) )
    {
        return eFilter == FILTER_BLACKSMITH_RECIPES_DAGGER;
    }

    if (ItemHasLabel(gbidItem, ITL_WAND))
    {
        return eFilter == FILTER_BLACKSMITH_RECIPES_WAND;
    }

    if (ItemHasLabel(gbidItem, ITL_MACE))
    {
        return eFilter == FILTER_BLACKSMITH_RECIPES_MACE;
    }

    if (ItemHasLabel(gbidItem, ITL_OFFHAND))
    {
        return eFilter == FILTER_BLACKSMITH_RECIPES_OFF_HAND;
    }

    if (ItemHasLabel(gbidItem, ITL_FLAIL))
    {
        return eFilter == FILTER_BLACKSMITH_RECIPES_FLAIL;
    }

    if (ItemHasLabel(gbidItem, ITL_MIGHTY_WEAPON))
    {
        return eFilter == FILTER_BLACKSMITH_RECIPES_MIGHTY;
    }

    if(ItemHasLabel(gbidItem, ITL_SCYTHE))
    {
        return eFilter == FILTER_BLACKSMITH_RECIPES_SCYTHE;
    }

    if ( ItemHasLabel(gbidItem, ITL_STAFF) || ItemHasLabel(gbidItem, ITL_COMBAT_STAFF) || ItemHasLabel(gbidItem, ITL_POLEARM) || ItemHasLabel(gbidItem, ITL_SPEAR) )
    {
        return eFilter == FILTER_BLACKSMITH_RECIPES_STAFF;
    }

    if (ItemHasLabel(gbidItem, ITL_AXE))
    {
        return eFilter == FILTER_BLACKSMITH_RECIPES_AXE;
    }

    if (ItemHasLabel(gbidItem, ITL_FIST_WEAPON))
    {
        return eFilter == FILTER_BLACKSMITH_RECIPES_FIST;
    }

    if (ItemHasLabel(gbidItem, ITL_BOW))
    {
        return eFilter == FILTER_BLACKSMITH_RECIPES_BOW;
    }

    if (ItemHasLabel(gbidItem, ITL_RING))
    {
        return eFilter == FILTER_BLACKSMITH_RECIPES_RING_RIGHT || eFilter == FILTER_BLACKSMITH_RECIPES_RING_LEFT;
    }

    if (ItemHasLabel(gbidItem, ITL_AMULET))
    {
        return eFilter == FILTER_BLACKSMITH_RECIPES_NECK;
    }

    if (ItemHasLabel(gbidItem, ITL_GLOVES))
    {
        return eFilter == FILTER_BLACKSMITH_RECIPES_HANDS;
    }

    if (ItemHasLabel(gbidItem, ITL_SHOULDERS))
    {
        return eFilter == FILTER_BLACKSMITH_RECIPES_SHOULDERS;
    }

    if (ItemHasLabel(gbidItem, ITL_CHESTARMOR))
    {
        return eFilter == FILTER_BLACKSMITH_RECIPES_CHEST;
    }

    if (ItemHasLabel(gbidItem, ITL_HELM))
    {
        return eFilter == FILTER_BLACKSMITH_RECIPES_HEAD;
    }

    if (ItemHasLabel(gbidItem, ITL_BELT))
    {
        return eFilter == FILTER_BLACKSMITH_RECIPES_BELT;
    }

    if (ItemHasLabel(gbidItem, ITL_LEGS))
    {
        return eFilter == FILTER_BLACKSMITH_RECIPES_LEGS;
    }

    if (ItemHasLabel(gbidItem, ITL_BOOTS))
    {
        return eFilter == FILTER_BLACKSMITH_RECIPES_FEET;
    }

    if (ItemHasLabel(gbidItem, ITL_BRACERS))
    {
        return eFilter == FILTER_BLACKSMITH_RECIPES_WRIST;
    }

    return eFilter == FILTER_BLACKSMITH_RECIPES_OTHER;
}




PFN_COMPARE CrafterRecipes::GetSortFuncForFilter(const UIFilterType eFilter)

{
    switch (eFilter)
    {
    case FILTER_JEWELER_RECIPES_RUBY:
    case FILTER_JEWELER_RECIPES_TOPAZ:
    case FILTER_JEWELER_RECIPES_EMERALD:
    case FILTER_JEWELER_RECIPES_AMETHYST:
    case FILTER_JEWELER_RECIPES_DIAMOND:
        return JewelerRecipeSortPredicate;
    case FILTER_BLACKSMITH_RECIPES_SWORD:
    case FILTER_BLACKSMITH_RECIPES_CROSSBOW:
    case FILTER_BLACKSMITH_RECIPES_DAGGER:
    case FILTER_BLACKSMITH_RECIPES_WAND:
    case FILTER_BLACKSMITH_RECIPES_MACE:
    case FILTER_BLACKSMITH_RECIPES_OFF_HAND:
    case FILTER_BLACKSMITH_RECIPES_MIGHTY:
    case FILTER_BLACKSMITH_RECIPES_STAFF:
    case FILTER_BLACKSMITH_RECIPES_AXE:
    case FILTER_BLACKSMITH_RECIPES_FIST:
    case FILTER_BLACKSMITH_RECIPES_BOW:
    case FILTER_BLACKSMITH_RECIPES_FLAIL:
    case FILTER_BLACKSMITH_RECIPES_SCYTHE:
    case FILTER_BLACKSMITH_RECIPES_RING_LEFT:
    case FILTER_BLACKSMITH_RECIPES_RING_RIGHT:
    case FILTER_BLACKSMITH_RECIPES_NECK:
    case FILTER_BLACKSMITH_RECIPES_HANDS:
    case FILTER_BLACKSMITH_RECIPES_SHOULDERS:
    case FILTER_BLACKSMITH_RECIPES_CHEST:
    case FILTER_BLACKSMITH_RECIPES_HEAD:
    case FILTER_BLACKSMITH_RECIPES_BELT:
    case FILTER_BLACKSMITH_RECIPES_LEGS:
    case FILTER_BLACKSMITH_RECIPES_FEET:
    case FILTER_BLACKSMITH_RECIPES_WRIST:
    case FILTER_BLACKSMITH_RECIPES_OTHER:
        return BlacksmithRecipeSortPredicate;
    default:
        return NULL;
    }
}






CraftingItem::CraftingItem()
    : m_idRecipeItem(ACDID_INVALID), m_gbidRecipe(-1), m_idCraftedItem(ACDID_INVALID), m_eCraftingStage(BEFORE_CRAFTING)
{
    m_aridItem.push_back(ACDID_INVALID);
}




void CraftingItem::OnCraftingStarted(
    const GBID gbidRecipe,
    const ACDID idRecipeItem)
{
    m_eCraftingStage = STARTED_CRAFTING;

    m_idRecipeItem = idRecipeItem;
    m_gbidRecipe = gbidRecipe;
}



void CraftingItem::OnCraftingFinished(const ACDID idCraftedItem)

{
    m_idCraftedItem = idCraftedItem;
    m_eCraftingStage = FINISHED_CRAFTING;
}




const ACDIDArray & CraftingItem::GetItemsWithFilter(const UIFilterType eFilterType) const

{
    m_aridItem[0] = m_eCraftingStage == STARTED_CRAFTING ? m_idRecipeItem : m_idCraftedItem;
    return m_aridItem;
}












UIInventoryViewItemsProvider::ProviderType CraftingItem::GetProviderType() const

{
    return m_eCraftingStage == FINISHED_CRAFTING ? ITEM_PROVIDER_REAL_ITEMS : ITEM_PROVIDER_BLACKSMITH_RECIPES;
}











CRefString CraftingItem::GetItemName(const int32 nIndex) const

{
    if (GetProviderType() == ITEM_PROVIDER_BLACKSMITH_RECIPES)
        return sGetRecipeName(*this, m_gbidRecipe, nIndex);
    return CRefString("");
}







void TransmogableItems::Refresh()

{
    if (GetPlayer())
    {



        uint32 dwFlags = 1 << FilteredInventoryView::FIV_INCLUDE_EQUIPPED_BIT;


        ACDID idACDPlayer = GetPlayer()->idPlayerACD;
        m_tView.ClearItems();
        m_tView.AddItems(idACDPlayer, idACDPlayer, SLOT_PLAYER_BACKPACK, dwFlags, 0);
    }
}



void DyeableItems::Refresh()

{
    if (GetPlayer())
    {



        uint32 dwFlags = 1 << FilteredInventoryView::FIV_INCLUDE_EQUIPPED_BIT;


        ACDID idACDPlayer = GetPlayer()->idPlayerACD;
        m_tView.ClearItems();
        m_tView.AddItems(idACDPlayer, idACDPlayer, SLOT_PLAYER_BACKPACK, dwFlags, 0);
    }
}



void CosmeticItemsProvider::CleanupACDs()

{
    for (int32 i = 0; i < m_arrVirtualItems.size(); ++i)
    {
        ACDDelete(m_arrVirtualItems[i], FALSE);
    }
    m_arrVirtualItems.clear();


    for (int32 i = 0; i < 4; ++i)
    {
        m_arrFilteredItems[i].clear();
    }
}




UIInventoryViewItemsProvider::ProviderType CosmeticItemsProvider::GetProviderType() const

{
    return ITEM_PROVIDER_COSMETIC_ITEMS;
}




BOOL CosmeticItemsProvider::CanProvideUnidentifiedItems() const

{
    return FALSE;
}






static BOOL sCosmeticItemIteratorCallback(
    const InventoryLocation & tInvLoc,
    void * pCallBackData)
{
    ACDIDArray * parrCosmeticItems = (ACDIDArray *)pCallBackData;
    ActorCommonData & tACDContainer = ACDGet(tInvLoc.idACDContainer);

    ACDID idACDItem = ACDInventoryGetSquareItem(tACDContainer, tInvLoc.eSlot, tInvLoc.pt);
    ActorCommonData * ptACDItem = ACDTryToGet(idACDItem);


    if (ptACDItem && ItemIsCosmeticItem(ptACDItem->hGB.gbid))
    {
        parrCosmeticItems->push_back(idACDItem);
    }

    return TRUE;
}






void CosmeticItemsProvider::Refresh()

{
    CleanupACDs();

    Player * ptPlayer = GetPlayer();
    if (ptPlayer)
    {

        m_arrRecipes.clear();
        m_arrItems.clear();
        m_arrVirtualItems.clear();

        for (int32 n = 0; n < 4; ++n)
        {
            m_arrFilteredItems[n].clear();
        }

        ACDNetworkName ann = (ACDNetworkName)13684;  // inferred: 0x3574 + 1 is the first name used


        for (uint32 i = 0; i < 512; ++i)
        {
            GBID gbidCosmetic = ptPlayer->tCosmeticItemSavedData.tCosmeticItemData.gbidCosmeticItems[i];

            if (!GBIDExists(gbidCosmetic, GB_ITEMS))
            {
                continue;
            }

            ScopedItemDefinitionLockR pdefItem(gbidCosmetic);

            if (!pdefItem)
            {
                continue;
            }
            if (FilterIncludesItem(gbidCosmetic, FILTER_COSMETICS))
            {
                if (blz::find(m_arrRecipes.begin(), m_arrRecipes.end(), gbidCosmetic) == m_arrRecipes.end())
                {
                    m_arrRecipes.push_back(gbidCosmetic);
                }
            }
        }

        PFN_COMPARE fnCompare = CosmeticItemSortPredicate;
        // inferred (lines 1024-1028 hold only the sort): the null check of CrafterRecipes::Refresh line 539
        if (fnCompare != NULL)
        {
            blzext::vector_sort(m_arrRecipes, fnCompare);
        }


        for (int32 j = 0; j < m_arrRecipes.size(); ++j)
        {
            GBID gbidCosmetic = m_arrRecipes[j];
            ann = (ACDNetworkName)(ann + 1);
            ACDID idACDItem = CosmeticItems_GenerateItemFromGBID(ann, gbidCosmetic);
            m_arrVirtualItems.push_back(idACDItem);
            m_arrItems.push_back(idACDItem);
        }

        GBIDArray arrPortraitFrames = PlayerGetUnlockedPortraitsForParagonLevel(ptPlayer->nCurrentAltLevel);

        for (int32 j = 0; j < arrPortraitFrames.size(); ++j)
        {
            GBID gbidCosmetic = arrPortraitFrames[j];
            ann = (ACDNetworkName)(ann + 1);
            ACDID idACDItem = CosmeticItems_GenerateItemFromGBID(ann, gbidCosmetic);
            m_arrVirtualItems.push_back(idACDItem);
            m_arrItems.push_back(idACDItem);
        }

        ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
        if (ptACDPlayer)
        {
            ACDInventoryIterateSlot(*ptACDPlayer, SLOT_PLAYER_BACKPACK, NULL, sCosmeticItemIteratorCallback, &m_arrItems, FALSE, FALSE);
        }

        for (int32 n = 0; n < m_arrItems.size(); ++n)
        {
            ActorCommonData * ptACDItem = ACDTryToGet(m_arrItems[n]);

            if (ptACDItem)
            {
                CosmeticItemType eType = CosmeticItems_GetCosmeticItemTypeFromGBID(ptACDItem->hGB.gbid);

                switch (eType)
                {
                case COSMETIC_ITEM_WINGS:
                    m_arrFilteredItems[FILTER_WINGS - FILTER_WINGS].push_back(m_arrItems[n]);
                    break;
                case COSMETIC_ITEM_PET:
                    m_arrFilteredItems[FILTER_PETS - FILTER_WINGS].push_back(m_arrItems[n]);
                    break;
                case COSMETIC_ITEM_PENNANT:
                    m_arrFilteredItems[FILTER_PENNANTS - FILTER_WINGS].push_back(m_arrItems[n]);
                    break;
                case COSMETIC_ITEM_FRAME:
                    m_arrFilteredItems[FILTER_FRAMES - FILTER_WINGS].push_back(m_arrItems[n]);
                    break;
                }
            }
        }
    }
}




BOOL CosmeticItemsProvider::FilterIncludesItem(
    const GBID gbidItem,
    const UIFilterType eFilter)
{
    // Only inlined (into Refresh with FILTER_COSMETICS); the eFilter tests are inferred from the
    // filter names in the header's GetItemsWithFilter, the label order from lines 1094-1109.
    if (ItemHasLabel(gbidItem, ITL_COSMETIC_WINGS))
    {
        return eFilter == FILTER_COSMETICS || eFilter == FILTER_WINGS;
    }

    if (ItemHasLabel(gbidItem, ITL_COSMETIC_PENNANT))
    {
        return eFilter == FILTER_COSMETICS || eFilter == FILTER_PENNANTS;
    }

    if (ItemHasLabel(gbidItem, ITL_COSMETIC_PET))
    {
        return eFilter == FILTER_COSMETICS || eFilter == FILTER_PETS;
    }

    if (ItemHasLabel(gbidItem, ITL_COSMETIC_PORTRAIT_FRAME))
    {
        return eFilter == FILTER_COSMETICS || eFilter == FILTER_FRAMES;
    }

    return FALSE;
}



// The original's copy was removed by the linker (DWARF low_pc 0, 12 bytes); body inferred.
PFN_COMPARE CosmeticItemsProvider::GetSortFuncForFilter(
    const UIFilterType eFilter)
{
    return CosmeticItemSortPredicate;
}







CRefString sGetCosmeticName(
    const UIInventoryViewItemsProvider & tItemsProvider,
    const GBID gbidCosmetic,
    const int32 nIndex)
{
    ScopedItemDefinitionLockR pdefItemEntry(gbidCosmetic);

    CRefString sLabel = SNOToString(ACTOR_SNO, pdefItemEntry->snoActor, FALSE);
    CRefString sItemName = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_ITEMS), sLabel);

    return sItemName;
}




CRefString CosmeticItemsProvider::GetItemName(const int32 nIndex) const

{
    GBID gbidRecipe = GetRecipes()[nIndex];
    return sGetCosmeticName(*this, gbidRecipe, nIndex);
}

void LeaderboardEquippedItem::CleanupACDs()

{
    for (int32 eFilter = FILTER_PLAYER_START; eFilter < FILTER_PLAYER_END; ++eFilter)
    {
        if (m_arrItems[eFilter].size() > 0)
        {
            ACDDelete(m_arrItems[eFilter][0], FALSE);
            m_arrItems[eFilter].clear();
        }
    }

    for (ACDID * iter = m_vecSocketACDs.begin(); iter != m_vecSocketACDs.end(); ++iter)
    {
        ACDDelete(*iter, FALSE);
    }
    m_vecSocketACDs.clear();
}


void LeaderboardEquippedItem::SetData(
    const D3::Leaderboard::HeroSnapshot & tHeroSnapshot,
    PlayerClass eClass)
{
    CleanupACDs();

    for (int32 eFilter = FILTER_PLAYER_START; eFilter < FILTER_PLAYER_END; ++eFilter)
    {
        ACDID idItem = GenerateLeaderboardEquippedItem(tHeroSnapshot, eFilter, m_vecSocketACDs);
        if (idItem != ACDID_INVALID)
        {
            m_arrItems[eFilter].push_back(idItem);
        }
    }

    m_ePlayerClass = eClass;
}





void CraftingMatItemsProvider::CleanupACDs()

{
    for (int32 i = 0; i < m_arrVirtualItems.size(); ++i)
    {
        ACDDelete(m_arrVirtualItems[i], FALSE);
    }
    m_arrVirtualItems.clear();

}




UIInventoryViewItemsProvider::ProviderType CraftingMatItemsProvider::GetProviderType() const

{
    return ITEM_PROVIDER_CRAFTING_MATERIALS;
}




BOOL CraftingMatItemsProvider::CanProvideUnidentifiedItems() const

{
    return FALSE;
}




























void CraftingMatItemsProvider::Refresh()

{
    CleanupACDs();

    Player * ptPlayer = GetPlayer();
    if (ptPlayer)
    {
        ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
        if (ptACDPlayer)
        {
            m_arrVirtualItems.clear();

            ACDNetworkName ann = (ACDNetworkName)14196;  // inferred: 14197 (0x3775) is the first name used


            GBHandleList listGBHandles(GBGetHandlePool());
            GBEnumerate(GB_CURRENCY_CONVERSIONS, listGBHandles);


            int32 nCurrenciesDiscovered = ptACDPlayer->AttributesGetInt(FastAttribKey(ATTRIBUTE_CURRENCIES_DISCOVERED));


            struct CurrencyItemInfo
            {
                CurrencyType eCurrency;
                int32 nSortOrder;
            };

            blz::vector<CurrencyItemInfo> arCurrencyInfos;

            POS pos = listGBHandles.GetHeadPosition();
            while (pos)
            {
                GBHandle hGB = listGBHandles.GetNext(pos);
                ScopedCurrencyConversionEntryLockR pdefCurrencyConversion(hGB.gbid);

                if (pdefCurrencyConversion && !pdefCurrencyConversion->bHidden)
                {
                    if (ptACDPlayer->GetCurrencyAmount(pdefCurrencyConversion->eCurrency) > 0)
                    {
                        CurrencyItemInfo tCurrencyItemInfo;
                        tCurrencyItemInfo.eCurrency = pdefCurrencyConversion->eCurrency;
                        tCurrencyItemInfo.nSortOrder = pdefCurrencyConversion->nSortOrder;
                        arCurrencyInfos.push_back(tCurrencyItemInfo);
                    }
                }
            }

            blz::function<bool(const CurrencyItemInfo&, const CurrencyItemInfo&)> funcSortEntries = [](const CurrencyItemInfo & tCurrencyA, const CurrencyItemInfo & tCurrencyB)
            {
                return tCurrencyA.nSortOrder < tCurrencyB.nSortOrder;
            };
            blz::sort(arCurrencyInfos.begin(), arCurrencyInfos.end(), funcSortEntries);

            for (int32 i = 0; i < arCurrencyInfos.size(); ++i)
            {
                ann = (ACDNetworkName)(ann + 1);
                ACDID idACDItem = sGenerateCraftingMaterialFromCurrency(ann, arCurrencyInfos[i].eCurrency, ptACDPlayer);
                m_arrVirtualItems.push_back(idACDItem);
            }
        }
    }
}




















CRefString CraftingMatItemsProvider::GetItemName(const int32 nIndex) const

{
    if (nIndex >= 0 && nIndex < m_arrVirtualItems.size()) return ItemGetDisplayName(m_arrVirtualItems[nIndex]);
    return CRefString("");
}








































































































































































































UIInventoryView::UIInventoryView(const UIID & idRoot)
    : m_ptParentScreen(NULL), m_idRoot(idRoot), m_bIsActive(FALSE), m_bIsEnabled(TRUE),
      m_idStatusLine(GetUIIDNone()), m_fUpdateRequestedFlag(FALSE), m_eMaintainSelectionOptions(RESET_SELECTION),
      m_idSelectedItem(ACDID_INVALID), m_idIdentifyingItemACD(ACDID_INVALID), m_bIsIdentifying(FALSE),
      m_bIdentifyingRequestSent(FALSE), m_dwIdentifyingEndTime(0), m_idSoundLoop(SOUNDID_INVALID)










{
}




void UIInventoryView::Show(const BOOL bShow)

{
    if (m_bIsActive != bShow)
    {
        m_bIsActive = bShow;
        OnShow(bShow);
        if (bShow) ForceUpdate(RESET_SELECTION);
        else DoneIdentifying(FALSE);
    }
}












void UIInventoryView::Update()

{
    if (m_fUpdateRequestedFlag)
    {
        OnRequestedUpdate(m_eMaintainSelectionOptions);
        m_fUpdateRequestedFlag = FALSE;
    }
    OnUpdate();



    UIID idHeader = GetHeaderUIID();
    UIFilterType eFilterType = m_ptParentScreen ? m_ptParentScreen->GetFilter() : FILTER_ALL;
    CRefString sHeaderText = StringListEntryReplace(m_sHeaderText.str(), "{Filter}",
        StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CONSOLE_INVENTORY), GetIconNameFromFilterType(eFilterType)).str(), 999);
    UI_GetTextFromID(idHeader).SetText(sHeaderText.str(), FALSE);
}







UIID UIInventoryView::GetHeaderUIID() const

{
    return UIIDCreateFromParent(GetRootUIID(), ".LayoutRoot.WindowTitle");
}





void UIInventoryView::ShowBagSpace(
    InventoryItems & tItemsProvider,
    CRefString sBagLabel)
{
    const UIID & idStatusLine = GetStatusLineUIID();
    if (idStatusLine == GetUIIDNone()) return;




    int32 nUsedBagSpace = 0;
    int32 nTotalBagSpace = 0;
    tItemsProvider.GetBagSpace(nUsedBagSpace, nTotalBagSpace);

    CRefString sBagSpace = LocalizedSprintf(StringListFetch("ConsoleInventory:BagSpace").str(), sBagLabel.str(),
        UnicodeText::FormatNumber(nUsedBagSpace, TRUE, FNS_NORMAL, XLOCALE_INVALID).str(),
        UnicodeText::FormatNumber(nTotalBagSpace, TRUE, FNS_NORMAL, XLOCALE_INVALID).str(), NULL);
    UI_GetTextFromID(idStatusLine).SetText(sBagSpace, FALSE);
}








BOOL UIInventoryView::CanScrollWithTriggers() const

{
    return FALSE;
}




UIID UIInventoryView::GetScrollBoxUIID() const

{
    return GetUIIDNone();
}




void UIInventoryView::StartIdentifying()

{
    SetIsIdentifying(TRUE);
    m_dwIdentifyingEndTime = sgtAppGlobals.dwFrameCounter + IDENTIFYING_DURATION;



    PlaySoundOptions tOptions;
    tOptions.fForce2D = TRUE;

    m_idSoundLoop = SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_IDENTIFY_LOOP), tOptions);
}



void UIInventoryView::DoneIdentifying(const BOOL fSuccess)

{
    SoundDelete(m_idSoundLoop);
    m_idSoundLoop = SOUNDID_INVALID;
    SetIdentifyingItemACD(ACDID_INVALID);
    SetIsIdentifying(FALSE);
    if (fSuccess)
    {

        PlaySoundOptions tOptions;
        tOptions.fForce2D = TRUE;
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_IDENTIFY_FINISH1), tOptions);
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_IDENTIFY_FINISH2), tOptions);
        GetParentScreen()->RequestUpdate(KEEP_ITEM_SELECTED);
    }
}





int JewelerRecipeSortPredicate(
    const void * gbidArg1,
    const void * gbidArg2)
{
    ScopedRecipeEntryLockR pdefRecipeEntry1(*(const GBID *)gbidArg1);
    ScopedRecipeEntryLockR pdefRecipeEntry2(*(const GBID *)gbidArg2);
    ScopedRecipeDefinitionLockR pdefRecipe1(pdefRecipeEntry1->snoRecipe);
    ScopedRecipeDefinitionLockR pdefRecipe2(pdefRecipeEntry2->snoRecipe);

    ScopedItemDefinitionLockR pdefItem1(pdefRecipe1->tSpecificItem.gbidItem);
    ScopedItemDefinitionLockR pdefItem2(pdefRecipe2->tSpecificItem.gbidItem);
    if (pdefItem1 && pdefItem2)
    {




        if (pdefItem1->eGemType != pdefItem2->eGemType)
        {
            CRefString sGem1 = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_ITEM_TYPES), SymbolToName(pdefItem1->eGemType, gGemTypeNames));
            CRefString sGem2 = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_ITEM_TYPES), SymbolToName(pdefItem2->eGemType, gGemTypeNames));
            int32 nRetVal = strcmp(sGem1.str(), sGem2.str());




            if (nRetVal != 0) return nRetVal;
        }

        if (pdefItem1->nItemLevel != pdefItem2->nItemLevel)
        {
            return pdefItem2->nItemLevel - pdefItem1->nItemLevel;
        }
    }
    return 0;
}




int BlacksmithRecipeSortPredicate(
    const void * gbidArg1,
    const void * gbidArg2)
{
    ScopedRecipeEntryLockR pdefRecipeEntry1(*(const GBID *)gbidArg1);
    ScopedRecipeEntryLockR pdefRecipeEntry2(*(const GBID *)gbidArg2);
    ScopedRecipeDefinitionLockR pdefRecipe1(pdefRecipeEntry1->snoRecipe);
    ScopedRecipeDefinitionLockR pdefRecipe2(pdefRecipeEntry2->snoRecipe);

    ScopedItemDefinitionLockR pdefItem1(pdefRecipe1->tSpecificItem.gbidItem);
    ScopedItemDefinitionLockR pdefItem2(pdefRecipe2->tSpecificItem.gbidItem);
    if (pdefItem1 && pdefItem2)
    {




        if (pdefItem1->nCrafterRequiredLevel != pdefItem2->nCrafterRequiredLevel)
        {
            return pdefItem2->nCrafterRequiredLevel - pdefItem1->nCrafterRequiredLevel;
        }
    }
    return 0;
}




int CosmeticItemSortPredicate(
    const void * gbidArg1,
    const void * gbidArg2)
{
    ScopedItemDefinitionLockR pdefEntry1(*(const GBID *)gbidArg1);
    ScopedItemDefinitionLockR pdefEntry2(*(const GBID *)gbidArg2);

    ScopedItemTypeDefinitonLockR pdefItemType1(pdefEntry1->gbidItemType);
    ScopedItemTypeDefinitonLockR pdefItemType2(pdefEntry2->gbidItemType);
    if (pdefItemType1 && pdefItemType2 && pdefItemType1 != pdefItemType2)
    {





        return pdefItemType1 > pdefItemType2 ? 1 : -1;
    }

    return 0;
}




int MysticRecipeSortPredicate(
    const void * gbidArg1,
    const void * gbidArg2)
{
    ScopedRecipeEntryLockR pdefRecipeEntry1(*(const GBID *)gbidArg1);
    ScopedRecipeEntryLockR pdefRecipeEntry2(*(const GBID *)gbidArg2);
    ScopedRecipeDefinitionLockR pdefRecipe1(pdefRecipeEntry1->snoRecipe);
    ScopedRecipeDefinitionLockR pdefRecipe2(pdefRecipeEntry2->snoRecipe);

    ScopedItemDefinitionLockR pdefItem1(pdefRecipe1->tSpecificItem.gbidItem);
    ScopedItemDefinitionLockR pdefItem2(pdefRecipe2->tSpecificItem.gbidItem);
    if (pdefItem1 && pdefItem2)
    {





        CRefString sRecipeName1 = SNOToString(RECIPE_SNO, pdefRecipeEntry1->snoRecipe, FALSE);
        CRefString sRecipeName2 = SNOToString(RECIPE_SNO, pdefRecipeEntry2->snoRecipe, FALSE);
        int32 nRetVal = strncmp(sRecipeName2.str(), sRecipeName1.str(), 3);
        if (nRetVal == 0)
        {




            if (pdefItem1->nCrafterRequiredLevel != pdefItem2->nCrafterRequiredLevel)
            {
                nRetVal = pdefItem2->nCrafterRequiredLevel - pdefItem1->nCrafterRequiredLevel;
            }
        }
        return nRetVal;
    }
    return 0;
}

} }

namespace {



CRefString sGetRecipeName(
    const UIInventory::Console::UIInventoryViewItemsProvider & tItemsProvider,
    const GBID gbidRecipe,
    const int32 nIndex)
{
    ScopedRecipeEntryLockR pdefRecipeEntry(gbidRecipe);

    CRefString sLabel = SNOToString(RECIPE_SNO, pdefRecipeEntry->snoRecipe, FALSE);
    CRefString sItemName = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_RECIPES), sLabel);

    if (sItemName.Length())
    {

        ScopedRecipeDefinitionLockR pdefRecipe(pdefRecipeEntry->snoRecipe);
        GBHandle hgbItem(2, pdefRecipe->tSpecificItem.gbidItem);
        ItemQualityLevel eIQL = ItemDetermineQualityLevel(sLabel.str(), hgbItem, pdefRecipe->tSpecificItem.gbidAffixes,
            pdefRecipe->tSpecificItem.nNumAffixes, pdefRecipe->tSpecificItem.nAdditionalRandomAffixes);

        if (ItemIsSetItem(hgbItem.gbid)) eIQL = 11;







        Set_Text_Color(sItemName, ItemGetDisplayNameColor(eIQL));
    }


    return sItemName;
}



CRefString sGetRecipeStatsFromAffixes(
    const ItemSpecifierData & tSpecificItem,
    const PlayerClass ePlayerClass)
{
    CRefString usAffixStats;

    for (int32 i = 0; i < tSpecificItem.nNumAffixes; ++i)
    {
        usAffixStats += UIUtils::Console::GetStatsFromAffix(tSpecificItem.gbidAffixes[i], tSpecificItem.gbidItem, ePlayerClass);
    }

    return usAffixStats;
}




CRefString sGetRecipeItemAttributeDescription(
    const ItemSpecifierData & tSpecificItem,
    const PlayerClass ePlayerClass)
{
    CRefString usAffixStats = sGetRecipeStatsFromAffixes(tSpecificItem, ePlayerClass);
    if (tSpecificItem.nAdditionalRandomAffixesDelta)
    {
        CRefString usRandomProperties = LocalizedSprintf(
            StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_ITEMS), guszItemLabelRandomPropsRange).str(),
            UnicodeText::FormatNumber(tSpecificItem.nAdditionalRandomAffixes, TRUE, FNS_NORMAL, 0).str(),
            UnicodeText::FormatNumber(tSpecificItem.nAdditionalRandomAffixes + tSpecificItem.nAdditionalRandomAffixesDelta, TRUE, FNS_NORMAL, 0).str(), NULL);

        usAffixStats += "{icon:bullet} ";
        usAffixStats += usRandomProperties;
    }
    else if (tSpecificItem.nAdditionalRandomAffixes)
    {
        CRefString usRandomProperties = LocalizedSprintf(
            StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_ITEMS), guszItemLabelRandomProps).str(),
            UnicodeText::FormatNumber(tSpecificItem.nAdditionalRandomAffixes, TRUE, FNS_NORMAL, 0).str(), NULL);

        usAffixStats += "{icon:bullet} ";
        usAffixStats += usRandomProperties;
    }
    Set_Text_Color(usAffixStats, FONTCOLOR_BLUE);

    return usAffixStats;
}




ACDID sGenerateCraftingMaterialFromCurrency(
    ACDNetworkName ann,
    CurrencyType eCurrency,
    ActorCommonData * ptACDPlayer)
{





    GBID gbidItem = CurrencyGetGBID(eCurrency);
    if (gbidItem == GBID_NONE)
    {
        return ACDID_INVALID;
    }

    GBHandle hgbItem(GB_ITEMS, gbidItem);


    D3::Items::Generator tItem;
    ItemGeneratorToDefaults(tItem);
    hgbItem.Export(*tItem.mutable_gb_handle());

    tItem.set_seed(0);
    uint32 dwFlags = tItem.flags() | (1 << IGF_IDENTIFIED_BIT);
    tItem.set_flags(dwFlags);

    int64 nCurrencyCount = ptACDPlayer->GetCurrencyAmount(eCurrency);
    tItem.set_stack_size(nCurrencyCount);

    return CItemGenerate(ann, tItem, NULL);
}

}
