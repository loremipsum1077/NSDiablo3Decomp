#include "UITransmuteRecipesCommon.h"
#include "../UI.h"
#include "../UIInit.h"
#include "../Controls/UITextControl.h"
#include "../Controls/UIStackPanelControl.h"
#include "../../ImageCollection.h"
#include "../../../Common/ActorCommonData.h"
#include "../../../Common/ActorInventory.h"
#include "../../../Common/Crafting/ItemCrafting.h"
#include "../../../Common/Definitions/GameBalanceDefinition.h"
#include "../../../Common/Definitions/StringListDefinition.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/Game.h"
#include "../../../Common/Items.h"
#include "../../../Common/ItemsSwig.h"
#include "../../../Sigma/UnicodeText.h"
#include "../../../Sigma/TypeDescriptor/SymbolicValue.h"

UIStackPanel& UI_GetStackPanelFromID(const UIID&);
CurrencyType CurrencyFromGBID(GBID);

// Original APIs from their DWARF owners; declarations not yet adopted in their headers.
BOOL ItemCrafting_TransmuteIsValidIngredientInput(const TransmuteRecipeIngredient&, const TransmuteInput&);
void ACDInventoryGetItemDrawInfoWithGBID(const GBID, ImageTextureFrame&, const ItemDrawType);
uint32 ItemGetRequiredAugmentJewelRankBySlot(LegendaryPowerSlotType);
extern SymbolicEntry gTransmuteRecipeActionTypes[];
extern SymbolicEntry gTransmuteRecipeIngredientTypes[];

inline UITransmuteRecipes::IngredientCount::IngredientCount() : gbidItem(GBID_NONE), nCount(0) {  // inline: DWARF abstract DIE only, no concrete DIE (r06 test)
}
inline UITransmuteRecipes::CanCompleteCallbackData::CanCompleteCallbackData()  // inline: DWARF abstract DIE only, no concrete DIE (r06 test)
    : ptACDPlayer(NULL), ptIngredient(NULL), nSpecificGBIDCount(0), bCanComplete(FALSE) {
}

#line 73
static void sSpecialGemRecipe(
    const UIID& idIngredients,
    const LPCSTR sTemplate,
    UIFunctionPtr pfnOnTooltip)
{

    UIStackPanel& tIngredientsPanel = UI_GetStackPanelFromID(idIngredients);

    const UIControlSourceKey INGREDIENT_LIST_SOURCE_KEY = UIControlSourceKey::Parse(sTemplate, SNO_NONE, NULL, NULL);

    for (int32 eGem = 1; eGem <= 5; ++eGem)
    {
        UIWindow* pControl = static_cast<UIWindow*>(

        UI_CreateControlFromTemplate(INGREDIENT_LIST_SOURCE_KEY, DynamicControlCreationParams(CRefString().Format("Ingredient_GemConvert%d", eGem), idIngredients), NULL));


        pControl->SetActive(TRUE);

        UIID idControl = pControl->GetID();

        tIngredientsPanel.AddChild(idControl);

        UIText& tIngredientText = UI_GetTextFromID(UIIDCreateFromParent(idControl, ".recipe_ingredients"));
        UIWindow& tItemBullet = UI_GetWindowFromID(UIIDCreateFromParent(idControl, ".ItemWrapper.ItemBullet"));
        UIWindow& tItemIcon = UI_GetWindowFromID(UIIDCreateFromParent(idControl, ".ItemWrapper.ItemIcon"));

        CRefString sIngredientDescription;

        // if/else-if chain below: rows 104:13 (condition) and 110:30 (second assignment), no switch row at col 9.
        GBID gbidGemReagent = GlobalGBIDGet(GLOBALGBID_ITEM_GEM_CONVERT_AMETHYST);
        if (eGem == 1)
        {
            gbidGemReagent = GlobalGBIDGet(GLOBALGBID_ITEM_GEM_CONVERT_AMETHYST);
        }
        else if (eGem == 2)
        {
            gbidGemReagent = GlobalGBIDGet(GLOBALGBID_ITEM_GEM_CONVERT_EMERALD);
        }
        else if (eGem == 3)
        {
            gbidGemReagent = GlobalGBIDGet(GLOBALGBID_ITEM_GEM_CONVERT_RUBY);
        }
        else if (eGem == 4)
        {
            gbidGemReagent = GlobalGBIDGet(GLOBALGBID_ITEM_GEM_CONVERT_TOPAZ);
        }
        else if (eGem == 5)
        {
            gbidGemReagent = GlobalGBIDGet(GLOBALGBID_ITEM_GEM_CONVERT_DIAMOND);
        }

        sIngredientDescription = ItemGetDisplayName(gbidGemReagent);

        tItemBullet.SetActive(FALSE);
        tItemIcon.SetActive(TRUE);


        ImageTextureFrame tImageTexFrame;
        ACDInventoryGetItemDrawInfoWithGBID(gbidGemReagent, tImageTexFrame, 0);
        tItemIcon.SetTextureFrame(tImageTexFrame);








        tIngredientText.SetText(sIngredientDescription, FALSE);
    }
}

#line 148
static void sSpecialAugmentRecipeLegendaryGem(
    const UIID& idIngredients,
    const LPCSTR sTemplate,
    UIFunctionPtr pfnOnTooltip)
{

    UIStackPanel& tIngredientsPanel = UI_GetStackPanelFromID(idIngredients);

    const UIControlSourceKey INGREDIENT_LIST_SOURCE_KEY = UIControlSourceKey::Parse(sTemplate, SNO_NONE, NULL, NULL);


    LegendaryPowerSlotType arSlots[3] = {LPST_WEAPON, LPST_JEWELRY, LPST_ARMOR};
    for (int eSlot: arSlots)
    {
        UIWindow* pControl = static_cast<UIWindow*>(

        UI_CreateControlFromTemplate(INGREDIENT_LIST_SOURCE_KEY, DynamicControlCreationParams(CRefString().Format("AugmentLegGem%d", eSlot), idIngredients), NULL));


        pControl->SetActive(TRUE);

        UIID idControl = pControl->GetID();

        tIngredientsPanel.AddChild(idControl);

        UIText& tIngredientText = UI_GetTextFromID(UIIDCreateFromParent(idControl, ".recipe_ingredients"));
        UIWindow& tItemBullet = UI_GetWindowFromID(UIIDCreateFromParent(idControl, ".ItemWrapper.ItemBullet"));
        UIWindow& tItemIcon = UI_GetWindowFromID(UIIDCreateFromParent(idControl, ".ItemWrapper.ItemIcon"));

        CRefString sIngredientDescription;

        int32 nGemRank = ItemGetRequiredAugmentJewelRankBySlot(eSlot);
        ImageTextureFrame tImageTexFrame;
        // if/else-if chain: rows 183:13 (condition), 185/189/193:13 (calls), no switch row at col 9.

        if (eSlot == 0)
        {
            ImageCollectionGetImageInfo("UI:KanaisRecipes_bullet_Weapon_P2", tImageTexFrame);
        }
        else if (eSlot == 1)
        {
            ImageCollectionGetImageInfo("UI:KanaisRecipes_bullet_Armor_P2", tImageTexFrame);
        }
        else if (eSlot == 2)
        {
            ImageCollectionGetImageInfo("UI:KanaisRecipes_bullet_Jewelry_P2", tImageTexFrame);
        }

        sIngredientDescription = LocalizedSprintf(StringListFetch("Kanais_Cube:Recipe_RequiredGemRank"), UnicodeText::FormatNumber(nGemRank, TRUE, FNS_NORMAL, 0).str(), NULL);

        tItemBullet.SetActive(FALSE);
        tItemIcon.SetActive(TRUE);

        tItemIcon.SetTextureFrame(tImageTexFrame);









        tIngredientText.SetText(sIngredientDescription, FALSE);
    }
}

#line 217
static void sSpecialAugmentRecipeNormalGem(
    const UIID& idIngredients,
    const LPCSTR sTemplate,
    uint32 nQuantity,
    UIFunctionPtr pfnOnTooltip)
{

    UIStackPanel& tIngredientsPanel = UI_GetStackPanelFromID(idIngredients);

    const UIControlSourceKey INGREDIENT_LIST_SOURCE_KEY = UIControlSourceKey::Parse(sTemplate, SNO_NONE, NULL, NULL);



    for (int32 eGem = GLOBALGBID_ITEM_AMETHYST_MAX_RANK; eGem <= GLOBALGBID_ITEM_TOPAZ_MAX_RANK; ++eGem)
    {
        UIWindow* pControl = static_cast<UIWindow*>(

        UI_CreateControlFromTemplate(INGREDIENT_LIST_SOURCE_KEY, DynamicControlCreationParams(CRefString().Format("Augment%d", eGem - GLOBALGBID_ITEM_AMETHYST_MAX_RANK + 1), idIngredients), NULL));


        pControl->SetActive(TRUE);

        UIID idControl = pControl->GetID();

        tIngredientsPanel.AddChild(idControl);

        UIText& tIngredientText = UI_GetTextFromID(UIIDCreateFromParent(idControl, ".recipe_ingredients"));
        UIWindow& tItemBullet = UI_GetWindowFromID(UIIDCreateFromParent(idControl, ".ItemWrapper.ItemBullet"));
        UIWindow& tItemIcon = UI_GetWindowFromID(UIIDCreateFromParent(idControl, ".ItemWrapper.ItemIcon"));

        CRefString sIngredientDescription;

        // if/else-if chain below: rows 251:13 (condition) and 257:23 (second assignment), no switch row at col 9.
        GBID gbidGem = GlobalGBIDGet(GLOBALGBID_ITEM_AMETHYST_MAX_RANK);
        if (eGem == GLOBALGBID_ITEM_AMETHYST_MAX_RANK)
        {
            gbidGem = GlobalGBIDGet(GLOBALGBID_ITEM_AMETHYST_MAX_RANK);
        }
        else if (eGem == GLOBALGBID_ITEM_EMERALD_MAX_RANK)
        {
            gbidGem = GlobalGBIDGet(GLOBALGBID_ITEM_EMERALD_MAX_RANK);
        }
        else if (eGem == GLOBALGBID_ITEM_RUBY_MAX_RANK)
        {
            gbidGem = GlobalGBIDGet(GLOBALGBID_ITEM_RUBY_MAX_RANK);
        }
        else if (eGem == GLOBALGBID_ITEM_TOPAZ_MAX_RANK)
        {
            gbidGem = GlobalGBIDGet(GLOBALGBID_ITEM_TOPAZ_MAX_RANK);
        }

        sIngredientDescription = ItemGetDisplayName(gbidGem);
        sIngredientDescription.Format("%s x%d", sIngredientDescription.str(), nQuantity);

        tItemBullet.SetActive(FALSE);
        tItemIcon.SetActive(TRUE);


        ImageTextureFrame tImageTexFrame;
        ACDInventoryGetItemDrawInfoWithGBID(gbidGem, tImageTexFrame, 0);
        tItemIcon.SetTextureFrame(tImageTexFrame);








        tIngredientText.SetText(sIngredientDescription, FALSE);
    }
}

#line 292
static void sSpecialCraftingMatRecipe(
    const UIID& idIngredients,
    const LPCSTR sTemplate,
    const TransmuteRecipeAction eAction,
    UIFunctionPtr pfnOnTooltip)
{



    UIStackPanel& tIngredientsPanel = UI_GetStackPanelFromID(idIngredients);

    const UIControlSourceKey INGREDIENT_LIST_SOURCE_KEY = UIControlSourceKey::Parse(sTemplate, SNO_NONE, NULL, NULL);

    for (int32 nRecipeNum = TRA_CONVERT_CRAFTING_MATS_FROM_NORMAL; nRecipeNum <= TRA_CONVERT_CRAFTING_MATS_FROM_RARE; nRecipeNum++)
    {
        TransmuteRecipeAction eRecipe = (TransmuteRecipeAction)nRecipeNum;

        if (eRecipe == eAction)
        {
            continue;
        }

        UIWindow* pControl = static_cast<UIWindow*>(

        UI_CreateControlFromTemplate(INGREDIENT_LIST_SOURCE_KEY, DynamicControlCreationParams(CRefString().Format("Ingredient_MatConvert%d", nRecipeNum), idIngredients), NULL));


        pControl->SetActive(TRUE);

        UIID idControl = pControl->GetID();

        tIngredientsPanel.AddChild(idControl);

        UIText& tIngredientText = UI_GetTextFromID(UIIDCreateFromParent(idControl, ".recipe_ingredients"));
        UIWindow& tItemBullet = UI_GetWindowFromID(UIIDCreateFromParent(idControl, ".ItemWrapper.ItemBullet"));
        UIWindow& tItemIcon = UI_GetWindowFromID(UIIDCreateFromParent(idControl, ".ItemWrapper.ItemIcon"));

        CRefString sIngredientDescription;
        ImageTextureFrame tImageTexFrame;

        switch (eRecipe)
        {
        case TRA_CONVERT_CRAFTING_MATS_FROM_NORMAL:
            sIngredientDescription = StringListFetch("KanaisIngredients:EquippableItem_Common");
            ImageCollectionGetImageInfo("UI:KanaisRecipes_bulletWhite_P2", tImageTexFrame);
            break;
        case TRA_CONVERT_CRAFTING_MATS_FROM_MAGIC:
            sIngredientDescription = StringListFetch("KanaisIngredients:EquippableItem_Magic");
            ImageCollectionGetImageInfo("UI:KanaisRecipes_bulletBlue_P2", tImageTexFrame);
            break;
        case TRA_CONVERT_CRAFTING_MATS_FROM_RARE:
            sIngredientDescription = StringListFetch("KanaisIngredients:EquippableItem_Rare");
            ImageCollectionGetImageInfo("UI:KanaisRecipes_bulletYellow_P2", tImageTexFrame);
            break;
        }
        tIngredientText.SetText(sIngredientDescription, FALSE);

        tItemIcon.SetTextureFrame(tImageTexFrame);
        tItemIcon.SetActive(TRUE);
        tItemBullet.SetActive(FALSE);
    }
}

#line 355
// NON_MATCHING: 3840 vs 3848 (frame 0x1a0 matches after the re-lay on DWARF lines 356-534). Ours hoists the 12-byte ingredient stride into a callee-saved register and copies pdefRecipe to x23 for the loop; the original rematerialises `orr w8,#0xc` at each use and keeps pdefRecipe in x26. NormalGem helper: original IV stays eGem (43..46), ours 0-based; a switch or one combined condition fold the range check and grow the frame to 0x1b0.
void UITransmuteRecipes::DisplayIngredients(
    const TransmuteRecipe* pdefRecipe,
    const UIID& idFlavorTitle,
    const UIID& idRecipeTitle,
    const UIID& idFlavorText,
    const UIID& idIngredients,
    const LPCSTR sTemplate,
    UIFunctionPtr pfnOnTooltip,
    ActorCommonData* ptACDPlayer)
{




    UIText& tTitle = UI_GetTextFromID(idRecipeTitle);
    UIText& tFlavorTitle = UI_GetTextFromID(idFlavorTitle);
    UIText& tFlavorText = UI_GetTextFromID(idFlavorText);


    CRefString sTitleLabel = SymbolToName(pdefRecipe->eAction, gTransmuteRecipeActionTypes);
    CRefString sTitle;
    if (!pdefRecipe->bHidden)
    {
        sTitle = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_TRANSMUTE_RECIPES), sTitleLabel);
    }
    tTitle.SetText(sTitle, FALSE);


    CRefString sFlavorTitle;
    if (!pdefRecipe->bHidden)
    {
        CRefString sFlavorTitleLabel = SymbolToName(pdefRecipe->eAction, gTransmuteRecipeActionTypes);
        sFlavorTitleLabel.Format("%s_Name", sFlavorTitleLabel.str());
        sFlavorTitle = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_TRANSMUTE_RECIPES), sFlavorTitleLabel);
    }
    else
    {
        sFlavorTitle = StringListFetch("Console_Kanais_Cube:HiddenRecipeTitle");
    }

    tFlavorTitle.SetText(sFlavorTitle, FALSE);


    CRefString sFlavorText = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_TRANSMUTE_FLAVOR_TEXT), sTitleLabel);
    tFlavorText.SetText(sFlavorText, FALSE);


    UIStackPanel& tIngredientsPanel = UI_GetStackPanelFromID(idIngredients);
    tIngredientsPanel.DeleteAllChildren();

    const UIControlSourceKey INGREDIENT_LIST_SOURCE_KEY = UIControlSourceKey::Parse(sTemplate, SNO_NONE, NULL, NULL);

    for (int32 nIngredient = 0; nIngredient < pdefRecipe->nIngredientsCount; nIngredient++)
    {
        const TransmuteRecipeIngredient& tIngredient = pdefRecipe->tIngredients[nIngredient];



        BOOL bCanCompleteIngredient = FALSE;

        if (ptACDPlayer)
        {
            CanCompleteCallbackData tCallbackData;
            tCallbackData.ptACDPlayer = ptACDPlayer;
            tCallbackData.ptIngredient = &tIngredient;
            tCallbackData.bCanComplete = FALSE;  // inferred: re-store after the pointers (one 8-byte zero store with 422 after the stp of the pointers)
            tCallbackData.nSpecificGBIDCount = 0;  // row 422:46 is this `=`

            ACDInventoryIterateItems(*ptACDPlayer, SLOT_PLAYER_BACKPACK, 4, CanCompleteCallback, &tCallbackData);
            ACDInventoryIterateItems(*ptACDPlayer, SLOT_PLAYER_SHARED_STASH, 4, CanCompleteCallback, &tCallbackData);
            bCanCompleteIngredient = tCallbackData.bCanComplete;


            if (tIngredient.eType == TRIT_SPECIFIC_GBID)
            {
                GBID gbidItem = (GBID)tIngredient.nTypeValue;
                CurrencyType eCurrency = CurrencyFromGBID(gbidItem);
                if (ItemIsCurrency(gbidItem) && ptACDPlayer->GetCurrencyAmount(eCurrency) >= tIngredient.nQuantity)
                {
                    bCanCompleteIngredient = TRUE;
                }
            }
        }


        UIWindow* pControl = static_cast<UIWindow*>(

        UI_CreateControlFromTemplate(INGREDIENT_LIST_SOURCE_KEY, DynamicControlCreationParams(CRefString().Format("Ingredient%d", nIngredient), idIngredients), NULL));


        pControl->SetActive(TRUE);

        UIID idControl = pControl->GetID();

        tIngredientsPanel.AddChild(idControl);

        UIText& tIngredientText = UI_GetTextFromID(UIIDCreateFromParent(idControl, ".recipe_ingredients"));
        UIWindow& tItemBullet = UI_GetWindowFromID(UIIDCreateFromParent(idControl, ".ItemWrapper.ItemBullet"));
        UIWindow& tItemIcon = UI_GetWindowFromID(UIIDCreateFromParent(idControl, ".ItemWrapper.ItemIcon"));

        CRefString sIngredientDescription;
        CRefString sIngredientLabel = SymbolToName(tIngredient.eType, gTransmuteRecipeIngredientTypes);
        if (tIngredient.eType == TRIT_SPECIFIC_GBID)
        {

            GBID gbidItem = (GBID)tIngredient.nTypeValue;
            sIngredientDescription = ItemGetDisplayName(gbidItem);

            if (AppDrawFlagIsSet(APP_DRAW_KANAIS_RECIPES_DEBUG_BIT))
            {

                sIngredientDescription.Format("%s (%s)", sIngredientDescription.str(), GBIDToString(gbidItem, GB_ITEMS, FALSE).str());
            }

            tItemBullet.SetActive(FALSE);
            tItemIcon.SetActive(TRUE);


            ImageTextureFrame tImageTexFrame;
            ACDInventoryGetItemDrawInfoWithGBID(tIngredient.nTypeValue, tImageTexFrame, 0);
            tItemIcon.SetTextureFrame(tImageTexFrame);







            SNO snoItemShader = bCanCompleteIngredient ? GlobalSNOGet(GLOBALSNO_SHADER_SPRITE_TRANSPARENT) : GlobalSNOGet(GLOBALSNO_SHADER_SPRITE_TRANSPARENT_INACTIVE);
            tItemIcon.SetShader(snoItemShader);
            UIWindow& tItemUnavailable = UI_GetWindowFromID(UIIDCreateFromParent(idControl, ".ItemWrapper.ItemUnavailable"));
            tItemUnavailable.SetActive(!bCanCompleteIngredient);
        }
        else
        {


            sIngredientDescription = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_TRANSMUTE_INGREDIENTS), sIngredientLabel);

            tItemBullet.SetActive(TRUE);
            tItemIcon.SetActive(FALSE);


            SNO snoItemShader = bCanCompleteIngredient ? GlobalSNOGet(GLOBALSNO_SHADER_SPRITE_TRANSPARENT) : GlobalSNOGet(GLOBALSNO_SHADER_SPRITE_TRANSPARENT_INACTIVE);
            tItemBullet.SetShader(snoItemShader);
            UIWindow& tItemUnavailable = UI_GetWindowFromID(UIIDCreateFromParent(idControl, ".ItemWrapper.ItemUnavailable"));
            tItemUnavailable.SetActive(!bCanCompleteIngredient);


            switch (tIngredient.eType)
            {
            case TRIT_GEM_CONVERSION_REAGENT:
                sSpecialGemRecipe(idIngredients, sTemplate, pfnOnTooltip);
                break;
            case TRIT_MAX_RANK_STANDARD_GEM:
                sSpecialAugmentRecipeNormalGem(idIngredients, sTemplate, tIngredient.nQuantity, pfnOnTooltip);
                break;
            case TRIT_UPGRADED_LEGENDARY_GEM:
                sSpecialAugmentRecipeLegendaryGem(idIngredients, sTemplate, pfnOnTooltip);
                break;
            case TRIT_NORMAL_MAGIC_RARE_ITEM:
                sSpecialCraftingMatRecipe(idIngredients, sTemplate, pdefRecipe->eAction, pfnOnTooltip);
                break;
            }
        }



        if (tIngredient.eType != TRIT_NORMAL_MAGIC_RARE_ITEM &&
            tIngredient.eType != TRIT_GEM_CONVERSION_REAGENT &&
            tIngredient.eType != TRIT_MAX_RANK_STANDARD_GEM)
        {

            sIngredientDescription.Format("%s x%d", sIngredientDescription.str(), tIngredient.nQuantity);
        }

        tIngredientText.SetText(sIngredientDescription, FALSE);
    }
}

#line 538
void UITransmuteRecipes::CanCompleteCallback(ACDID idACDSquare, InventorySlotType, void* pData) {
    CanCompleteCallbackData* ptCallbackData = static_cast<CanCompleteCallbackData*>(pData);
    if (ptCallbackData->bCanComplete) return;
    const TransmuteRecipeIngredient* ptIngredient = ptCallbackData->ptIngredient;
    ActorCommonData* ptACDItem = ACDTryToGet(idACDSquare);
    if (!ptACDItem) return;
    int32 nCount = ptACDItem->GetStackCount();
    nCount = nCount > 1 ? nCount : 1;
    if (ptIngredient->eType == TRIT_SPECIFIC_GBID) {
        if (ptACDItem->hGB.gbid == ptIngredient->nTypeValue &&
            (ptIngredient->unk_0c == -1 || ptIngredient->unk_0c <= ItemGetAncientRank(*ptACDItem)) &&
            (ptIngredient->unk_10 == -1 || ptIngredient->unk_10 == ItemGetAncientRank(*ptACDItem)) &&
            (ptCallbackData->nSpecificGBIDCount += nCount) >= ptIngredient->nQuantity)
            ptCallbackData->bCanComplete = TRUE;
    } else {
        if (ItemIsUnidentified(*ptACDItem)) return;
        TransmuteInput tInput;
        tInput.ptACDItem = ptACDItem;
        tInput.bUsedInRecipe = FALSE;
        tInput.eIngredientType = TRIT_INVALID;
        tInput.nStackCount = nCount;
        if (ItemCrafting_TransmuteIsValidIngredientInput(*ptIngredient, tInput)) {
            IngredientCount* ptIngredientCount = NULL;
            for (uint32 i = 0; i < ptCallbackData->arIngredientCounts.size(); ++i) {
                IngredientCount& tIngredientCount = ptCallbackData->arIngredientCounts[i];
                if (tIngredientCount.gbidItem == ptACDItem->hGB.gbid) {
                    ptIngredientCount = &tIngredientCount;
                    break;
                }
            }
            if (!ptIngredientCount) {
                IngredientCount tIngredientCount;
                tIngredientCount.gbidItem = ptACDItem->hGB.gbid;
                ptCallbackData->arIngredientCounts.push_back(tIngredientCount);
                // Original increments the local entry after copying it into the vector.
                ptIngredientCount = &tIngredientCount;
            }
            if ((ptIngredientCount->nCount += nCount) >= ptIngredient->nQuantity)
                ptCallbackData->bCanComplete = TRUE;
        }
    }
}
