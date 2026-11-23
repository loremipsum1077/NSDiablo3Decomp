#include "UICraftingCommon.h"

#include "../../../../Contrib/Contrib/blz/include/blz/algo_seq_const.h"
#include "../../../Common/Definitions/GameBalanceDefinition.h"
#include "../../../Common/Definitions/RecipeDefinition.h"
#include "../../../Common/Hyperlink.h"
#include "../../../Common/Items.h"
#include "../../../Common/Player.h"
#include "../../../Common/Protobuf/Items.pb.h"
#include "../../../Common/SNO/SNOName.h"
#include "../../../Sigma/RefString.h"
// Declared here because their headers are not adopted yet; each comment names the defining file.
BOOL ItemIsLegendary(GBID gbidItem);  // X/Common/Items.cpp
ItemQualityLevel ItemDetermineQualityLevel(LPCSTR szDebugStr, GBHandle hGBItem, const GBID * ptGBIDAffixes, const int32 nAffixCount, const int32 nBonusRandomAffixes);  // X/Common/Items.cpp

namespace UICrafting {


//----------------------------------------------------------------------------
// Collects the recipes the crafter can make.
void BuildRecipeListForCrafter(
    GBIDArray & arrRecipes,
    const Player * ptPlayer,
    CrafterType eCrafterType,
    RecipeDefPredicate fnPredicate)
{
    POS pos;






    const CrafterData & tData = ptPlayer->tCrafterSavedData.tCrafterData[eCrafterType];

    arrRecipes.reserve(tData.nNumRecipes);

    GBHandleList listGBHandles(GBGetHandlePool());
    GBEnumerate(GB_RECIPES, listGBHandles);

    pos = listGBHandles.GetHeadPosition();
    while (pos)
    {
        GBHandle hGB = listGBHandles.GetNext(pos);
        ScopedRecipeEntryLockR pdefRecipeEntry(hGB.gbid);
        BOOL bRecipeIsValid = fnPredicate ? fnPredicate(pdefRecipeEntry) : TRUE;
        if (pdefRecipeEntry &&
            pdefRecipeEntry->eCrafterType == eCrafterType &&
            pdefRecipeEntry->snoRecipe != SNO_NONE &&
            !(pdefRecipeEntry->dwFlags & (1 << RECIPE_DROPPED_BIT)) && bRecipeIsValid)
        {

            if (blz::find(arrRecipes.begin(), arrRecipes.end(), hGB.gbid) == arrRecipes.end())
            {
                arrRecipes.push_back(hGB.gbid);
            }
        }
    }

    for (int32 i = 0; i < tData.nNumRecipes; i++)
    {
        if (!GBIDExists(tData.gbidRecipes[i], GB_RECIPES))
        {
            continue;
        }

        ScopedRecipeEntryLockR pdefRecipeEntry(tData.gbidRecipes[i]);
        BOOL bRecipeIsValid = fnPredicate ? fnPredicate(pdefRecipeEntry) : TRUE;
        if (pdefRecipeEntry && bRecipeIsValid)
        {
            if (blz::find(arrRecipes.begin(), arrRecipes.end(), tData.gbidRecipes[i]) == arrRecipes.end())
            {
                arrRecipes.push_back(tData.gbidRecipes[i]);
            }
        }
    }
}

ACDID GenerateItemFromRecipe(
    ACDNetworkName ann,
    GBID gbidRecipe)
{
    ScopedRecipeEntryLockR pdefRecipeEntry(gbidRecipe);
    ScopedRecipeDefinitionLockR pdefRecipe(pdefRecipeEntry->snoRecipe);
    ItemSpecifierData tSpecificItem = pdefRecipe->tSpecificItem;




    GBHandle hgbItem(GB_ITEMS, tSpecificItem.gbidItem);

    D3::Items::Generator tItem;
    ItemGeneratorToDefaults(tItem);
    hgbItem.Export(*tItem.mutable_gb_handle());

    ItemQualityLevel eIQL = ItemDetermineQualityLevel(
        HyperlinkCreateFromSNO(SNOName(RECIPE_SNO, pdefRecipeEntry->snoRecipe), NULL).str(),
        hgbItem,
        tSpecificItem.gbidAffixes,
        tSpecificItem.nNumAffixes,
        tSpecificItem.nAdditionalRandomAffixes);

    const D3::Items::Generator & tDefault = D3::Items::Generator::default_instance();
    if (eIQL != tDefault.item_quality_level())
    {
        tItem.set_item_quality_level(eIQL);
    }

    tItem.set_seed(0);
    uint32 dwFlags = tItem.flags() | (1 << IGF_IDENTIFIED_BIT) | (1 << IGF_CRAFTED_BIT);
    tItem.set_flags(dwFlags);

    if (pdefRecipeEntry->dwFlags & (1 << RECIPE_DYNAMIC_RLEVEL_BIT))
    {
        ScopedItemDefinitionLockR pdefItem(tSpecificItem.gbidItem);
        if (pdefItem)
        {
            int32 nRequiredLevel = pdefItem->nCrafterRequiredLevel;
            if (ItemIsLegendary(tSpecificItem.gbidItem))
            {
                tItem.set_legendary_item_level(nRequiredLevel);
            }
        }
    }

    return CItemGenerate(ann, tItem, NULL);
}

}  // namespace UICrafting
