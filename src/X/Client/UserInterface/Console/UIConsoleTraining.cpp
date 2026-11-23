// Original file: X/Client/UserInterface/Console/UIConsoleTraining.cpp; functions sit on their original line numbers (2.6.2 DWARF).
#include "UIConsoleTraining.h"
#include "UIConsoleInventoryScreen.h"
#include "UIConsoleInventoryViewUtilities.h"
#include "../UI.h"
#include "../UIID.h"
#include "../Controls/UITextControl.h"
#include "../Controls/UIWindowControl.h"
#include "../../../Common/ActorCommonData.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/Items.h"
#include "../../../Common/Protobuf/Items.pb.h"
#include "../../../Sigma/DataStructures/XGrowableMap.h"
#include "../../../Common/Player.h"
#include "../../../Common/Crafting/ItemCrafting.h"
#include "../../../Common/Definitions/GameBalanceDefinition.h"
#include "../../../Common/Definitions/StringListDefinition.h"
#include "../../../Common/SNO/SNOFiles.h"
#include "../../../Common/SNO/SymbolicValue.h"
#include "../Interface/UICraftingCommon.h"
#include "../../../Sigma/BlzExt/BlzVectorHelper.h"
#include "../../../Sigma/LanguageRules.h"
#include "../../../Sigma/RefString.h"
#include "../../../Sigma/UnicodeText.h"
// Declared here because UIConsoleInventoryViewUtilities.h does not declare it yet (as in UIConsoleSalvage.cpp).
namespace UIInventory { namespace Console { void ShowDetailsMode(const UIID & idParent, const DetailsViewMode eViewMode); } }
// X/Client/UserInterface/Console/UIConsoleInventoryView.cpp:1745 to :1798; no header in src declares them.
namespace UIInventory { namespace Console { int BlacksmithRecipeSortPredicate(const void * gbidArg1, const void * gbidArg2); int JewelerRecipeSortPredicate(const void * gbidArg1, const void * gbidArg2); int MysticRecipeSortPredicate(const void * gbidArg1, const void * gbidArg2); } }
namespace UIInventory { namespace Console {

TrainingItems::~TrainingItems()

{
    CleanupACDs();
}




const ACDIDArray & TrainingItems::GetItemsWithFilter(const UIFilterType) const


{
    return m_arrItems;
}




CRefString TrainingItems::GetItemName(const int32 nIndex) const

{
    GBID gbidRecipe = GetRecipes()[nIndex];
    ScopedRecipeEntryLockR pdefRecipeEntry(gbidRecipe);

    CRefString sLabel = SNOToString(RECIPE_SNO, pdefRecipeEntry->snoRecipe, FALSE);
    return StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_RECIPES), sLabel);
}




int32 TrainingItems::GetIndexOfItem(const ACDID idItem) const
{

    for (int32 i = 0; i < (int32)m_arrItems.size(); ++i)
    {
        if (m_arrItems[i] == idItem)
        {
            return i;
        }
    }

    return -1;
}




void TrainingItems::Refresh()

{
    CleanupACDs();

    Player * ptPlayer = GetPlayer();
    if (ptPlayer)
    {
        m_arrItems.clear();
        m_arrRecipes.clear();

        const CrafterData & tData = ptPlayer->tCrafterSavedData.tCrafterData[ptPlayer->eActiveCrafterType];

        if (ptPlayer->eActiveCrafterType != CRAFTER_TYPE_MYSTIC)
        {
            RefreshTrainingRewards(*ptPlayer, ptPlayer->eActiveCrafterType, tData, m_arrItems, m_arrRecipes);
        }
        else
        {
            RefreshMysticTrainingRewards(tData, m_arrItems);
        }
    }
}




void TrainingItems::RefreshTrainingRewards(
    const Player & tPlayer,
    CrafterType eCrafterType,
    const CrafterData & tCrafterData,
    ACDIDArray & arrItems,
    GBIDArray & arrRecipes)
{
    GBHandleList listGBHandles(GBGetHandlePool());
    GBEnumerate(GB_RECIPES, listGBHandles);

    ACDNetworkName ann = (ACDNetworkName)9076;  // 0x2374; no enumerator in src has this value

    POS pos = listGBHandles.GetHeadPosition();
    while (pos)
    {
        GBHandle hGB = listGBHandles.GetNext(pos);
        ScopedRecipeEntryLockR pdefRecipeEntry(hGB.gbid);
        if (pdefRecipeEntry &&
            pdefRecipeEntry->eCrafterType == eCrafterType &&
            pdefRecipeEntry->nLevel == tCrafterData.nLevel + 1 &&
            !(pdefRecipeEntry->dwFlags & (1 << RECIPE_DROPPED_BIT)))
        {
            arrRecipes.push_back(hGB.gbid);
        }
    }

    PFN_COMPARE fnCompare = GetSortFuncForCrafter(eCrafterType);
    if (fnCompare)
    {

        blzext::vector_sort(arrRecipes, fnCompare);
    }


    for (uint32 i = 0; i < arrRecipes.size(); ++i)
    {
        GBID gbidRecipe = arrRecipes[i];
        ACDID idACDItem = UICrafting::GenerateItemFromRecipe((ACDNetworkName)(ann + i), gbidRecipe);

        arrItems.push_back(idACDItem);
    }
}




void TrainingItems::RefreshMysticTrainingRewards(
    const CrafterData & tCrafterData,
    ACDIDArray & arrItems)
{
    XGrowableMap<SNO, SNO, GBID, GBID> tUnlockableTransmogsMap(16, NULL);

    GBHandleList listItems(GBGetHandlePool());
    GBEnumerate(GB_ITEMS, listItems);


    POS pos = listItems.GetHeadPosition();
    while (pos)
    {
        GBID gbidItem = listItems.GetNext(pos).gbid;

        ScopedItemDefinitionLockR pdefItem(gbidItem);
        if (ItemCrafting_CanUnlockTransmog(gbidItem, NULL, FALSE) &&
            pdefItem->nTransmogUnlockCrafterLevel == tCrafterData.nLevel + 1)
        {



            GBID gbidExistingTransmog;
            if (tUnlockableTransmogsMap.Lookup(pdefItem->snoActor, gbidExistingTransmog))
            {
                if (ItemCrafting_CompareMatchingTransmogs(gbidItem, gbidExistingTransmog) > 0)
                {
                    continue;
                }
            }

            tUnlockableTransmogsMap.SetAt_Tracked(pdefItem->snoActor, gbidItem);
        }
    }
    ACDNetworkName ann = (ACDNetworkName)9076;  // 0x2374; no enumerator in src has this value


    pos = tUnlockableTransmogsMap.GetStartAssoc();
    while (pos)
    {
        SNO sno;
        GBID gbidItem;

        tUnlockableTransmogsMap.GetNextAssoc(pos, sno, gbidItem);

        D3::Items::Generator tGenerator;
        ItemGeneratorToDefaults(tGenerator);
        tGenerator.mutable_gb_handle()->set_gbid(gbidItem);
        tGenerator.set_durability(0);

        ACDID idACDItem = CItemGenerate(ann, tGenerator, NULL);
        ann = (ACDNetworkName)(ann + 1);

        arrItems.push_back(idACDItem);
    }
}



void TrainingItems::CleanupACDs()
{

    for (uint32 i = 0; i < m_arrItems.size(); ++i)
    {
        ACDDelete(m_arrItems[i], FALSE);
    }

    m_arrItems.clear();
}




PFN_COMPARE TrainingItems::GetSortFuncForCrafter(CrafterType eCrafterType)
{

    switch (eCrafterType)
    {
    case CRAFTER_TYPE_BLACKSMITH:
        return BlacksmithRecipeSortPredicate;
    case CRAFTER_TYPE_JEWEL:
        return JewelerRecipeSortPredicate;
    case CRAFTER_TYPE_MYSTIC:
        return MysticRecipeSortPredicate;
    default:
        return NULL;
    }
}









UIInventoryTrainingView::UIInventoryTrainingView(const UIID & idRoot)
    : UIInventoryView(idRoot)
{
}


void UIInventoryTrainingView::OnShow(const BOOL bShow)

{
    UI_GetWindowFromID(GetRootUIID()).SetActive(bShow);

    UIID idTraining;
    idTraining.SetFromParent(GetRootUIID(), "LayoutRoot.Train");
    UI_GetWindowFromID(idTraining).SetActive(bShow);
}




void UIInventoryTrainingView::OnUpdate()

{
    UpdateUI();
}




void UIInventoryTrainingView::OnSelectedItemChanged()

{
    UpdateUI();
}




void UIInventoryTrainingView::UpdateUI()

{

    const UIID & idRoot = GetRootUIID();
    UIInventoryScreen * ptParentScreen = GetParentScreen();
    Player * ptPlayer = ptParentScreen ? ptParentScreen->GetPlayer() : NULL;
    CrafterType eType = ptPlayer ? ptPlayer->eActiveCrafterType : CRAFTER_TYPE_INVALID;


    LanguageRules::SetClientGender(eType != CRAFTER_TYPE_MYSTIC);
    CRefString sCrafter = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_VENDOR), SymbolToName(eType, gCrafterTypes));
    SetHeaderText(sCrafter);

    ShowDetailsMode(idRoot, TRAINING_VIEW);


    UIID idTraining;
    idTraining.SetFromParent(idRoot, "LayoutRoot.Train");

    int32 nRank = 0;
    int32 nLevel = 0;
    int32 nMaxLevel = 0;
    if (ptPlayer)
    {
        nLevel = GetCraftLevel(eType, *ptPlayer);
        nRank = CrafterGetRank(eType, nLevel);
        nMaxLevel = CrafterGetMaxLevel(eType);
    }

    UIID idRankTitle;
    idRankTitle.SetFromParent(idTraining, "RankTitle");
    CRefString sRankName = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_VENDOR), CRefString().Format("Crafter_rank_%d", nRank));
    UI_GetTextFromID(idRankTitle).SetText(sRankName.str(), FALSE);

    UIID idRankLevel;
    idRankLevel.SetFromParent(idTraining, "RankLevel");
    UI_GetTextFromID(idRankLevel).SetText(UnicodeText::FormatNumber(nRank, TRUE, FNS_NORMAL, 0).str(), FALSE);

    UIID idRankInstructions;
    idRankInstructions.SetFromParent(idTraining, "RankInstructions");

    CRefString sText;
    if (nLevel >= nMaxLevel)
    {
        sText = StringListFetch("ConsoleArtisan:MaxRank");
    }
    else if (eType == CRAFTER_TYPE_BLACKSMITH)
    {
        int32 nItemLevel = ItemCrafting_GetDynamicRecipeMaxTargetLevel(nLevel + 1);
        sText = LocalizedSprintf(StringListFetch("ConsoleArtisan:TrainBlacksmithInstructions").str(),
            UnicodeText::FormatNumber(nItemLevel, TRUE, FNS_NORMAL, 0).str(), NULL);
    }
    else
    {


        sText = LocalizedSprintf(StringListFetch("ConsoleArtisan:TrainInstructions").str(),
            sCrafter.str(), NULL);
    }

    UI_GetTextFromID(idRankInstructions).SetText(sText, FALSE);
}
}  // namespace Console
}  // namespace UIInventory
