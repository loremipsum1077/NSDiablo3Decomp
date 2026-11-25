#include "UIConsoleInventoryCategorySummaryView.h"
#include "UIConsoleInventoryScreen.h"
#include "../UI.h"
#include "../Controls/UITextControl.h"
#include "../../../Common/ActorCommonData.h"
#include "../../../Common/Player.h"
#include "../../../Common/Items.h"
#include "../../../Common/ItemsSwig.h"
#include "../../../Common/GameSwig.h"
#include "../../../Common/CosmeticItems.h"
#include "../../../Common/Definitions/GameBalanceDefinition.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/Definitions/StringListDefinition.h"
#include "../../../Sigma/TypeDescriptor/SymbolicValue.h"
// Unadopted declarations from Common/SNO/SymbolicValue.cpp, Items.h,
// GlobalSNO.h and UIConsoleUtils.h.
GBID GlobalGBIDGet(const GlobalGBID eGlobalGBID);
extern const SymbolicEntry gGemTypeNames[], gCosmeticTypeNames[], gItemLabels[];
CRefString ItemTypeGetDescription(GBID gbidItemType, ItemQualityLevel eIQL, int32 eAncientRank, BOOL bSeasonOnly, BOOL bSanctified);  // 2.7.7: 5 args (Items.cpp)
namespace UIUtils { namespace Console {
void DisplayPortrait(PlayerClass ePlayerClass, BOOL bIsFemale, int32 nAltLevel, const UIID & idPortrait, GBID gbidPortraitFrame);
} }
namespace UIInventory { namespace Console {
// UIConsoleInventoryViewUtilities.h
 enum DetailsViewMode { ITEM_CATEGORY_VIEW = 4 };
void ShowDetailsMode(const UIID & idParent, DetailsViewMode eViewMode);
// UIConsoleItemCategories.h
BOOL IsGemSubCategory(ItemCategory eCategory);
BOOL IsCosmeticSubCategory(ItemCategory eCategory);
} }
using namespace UIInventory::Console;
namespace {
void sShowTheme(const UIID &, ItemCategory);
void sUpdateCategoryName(const UIID &, const CRefString &);
void sUpdateCategoryDescription(const UIID &, const CRefString &);
void sUpdateGemCategoryUI(const UIID &, ItemCategory);
void sUpdateShardCategoryUI(const UIID &);
void sUpdateItemCategoryUI(const UIID &, ItemCategory);
void sUpdateCosmeticCategoryUI(const UIID &, ItemCategory);
}
namespace UIInventory { namespace Console {
UIInventoryCategorySummaryView::UIInventoryCategorySummaryView(const UIID & idRoot)
    : UIInventoryView(idRoot), m_eCategory(CATEGORY_NONE) {}
void UIInventoryCategorySummaryView::OnShow(const BOOL bShow)
{
    UI_GetWindowFromID(GetRootUIID()).SetActive(bShow);
    UIID idSummary;
    idSummary.SetFromParent(GetRootUIID(), "LayoutRoot.ItemCategory");
    UI_GetWindowFromID(idSummary).SetActive(bShow);
}
void UIInventoryCategorySummaryView::OnRequestedUpdate(const MaintainSelectionOptions) { UpdateUI(); }
void UIInventoryCategorySummaryView::UpdateUI()
{
    if (!GetParentScreen() || m_eCategory == CATEGORY_NONE) return;
    if (IsGemSubCategory(m_eCategory)) sUpdateGemCategoryUI(GetRootUIID(), m_eCategory);
    else if (IsCosmeticSubCategory(m_eCategory)) sUpdateCosmeticCategoryUI(GetRootUIID(), m_eCategory);
    else if (m_eCategory == CATEGORY_SHARD) sUpdateShardCategoryUI(GetRootUIID());
    else sUpdateItemCategoryUI(GetRootUIID(), m_eCategory);
}
UIPortraitFrameSummaryView::UIPortraitFrameSummaryView(const UIID & idRoot)
    : UIInventoryView(idRoot), m_idACD(ACDID_INVALID) {}
void UIPortraitFrameSummaryView::OnShow(const BOOL bShow)
{
    UI_GetWindowFromID(GetRootUIID()).SetActive(bShow);
    UIID idSummary;
    idSummary.SetFromParent(GetRootUIID(), "LayoutRoot.ItemCategory.CosmeticPortraitDetails");
    UI_GetWindowFromID(idSummary).SetActive(bShow);
}
void UIPortraitFrameSummaryView::OnRequestedUpdate(const MaintainSelectionOptions) { UpdateUI(); }
void UIPortraitFrameSummaryView::UpdateUI()
{
    if (!GetParentScreen()) return;
    const UIID & idRoot = GetRootUIID();
    ShowDetailsMode(idRoot, ITEM_CATEGORY_VIEW);
    UIID idItemBackground = UIIDCreateFromParent(idRoot, ".LayoutRoot.ItemCategory.ItemCategoryBackground");
    UI_GetWindowFromID(idItemBackground).SetActive(FALSE);
    UIID idGemBackground = UIIDCreateFromParent(idRoot, ".LayoutRoot.ItemCategory.GemCategoryBackground");
    UI_GetWindowFromID(idGemBackground).SetActive(FALSE);
    UIID idPortraitBackground = UIIDCreateFromParent(idRoot, ".LayoutRoot.ItemCategory.CosmeticPortraitDetails");
    UI_GetWindowFromID(idPortraitBackground).SetActive(TRUE);
    sUpdateCategoryName(idRoot, "");
    sUpdateCategoryDescription(idRoot, "");
    if (m_idACD != ACDID_INVALID) {
        Player * ptPlayer = GetParentScreen()->GetPlayer();
        UIID idTitle = UIIDCreateFromParent(idPortraitBackground, ".Title");
        UIID idPortrait = UIIDCreateFromParent(idPortraitBackground, ".Portrait");
        ActorCommonData * ptACDPortrait = ACDTryToGet(m_idACD);
        if (ptACDPortrait) {
            GBID gbidPortraitFrame = ptACDPortrait->hGB.gbid;
            UI_GetTextFromID(idTitle).SetText(ItemGetDisplayName(gbidPortraitFrame), FALSE);
            UIUtils::Console::DisplayPortrait(ptPlayer->ePlayerClassPortrait, (ptPlayer->tHeroStateData.uPlayerFlags >> 1) & 1, ptPlayer->nCurrentAltLevel, idPortrait, gbidPortraitFrame);
        }
    }
}
} }
namespace {
const LPCSTR CATEGORY_THEME_LOOKUP[16] = {"Treasure", "Cosmetics", "Misc", "Gems", "Crafting", "Ruby", "Amethyst", "Emerald", "Topaz", "Diamond", "Legendary", "Shard", "Wings", "Pets", "Pennants", "PortraitFrames"};
void sShowTheme(const UIID & idParent, const ItemCategory eCategory)
{
    UIID idTheme;
    idTheme.SetFromParent(idParent, CATEGORY_THEME_LOOKUP[eCategory]);
    const UIControlItemArray & aridChildren = UI_GetWindowFromID(idParent).GetChildList();
    for (uint32 i = 0; i < aridChildren.size(); ++i)
        UI_GetWindowFromID(aridChildren[i].id).SetActive(aridChildren[i] == idTheme);
}
void sUpdateCategoryName(const UIID & idParent, const CRefString & sText)
{
    UIID idCategoryName;
    idCategoryName.SetFromParent(idParent, "LayoutRoot.ItemCategory.CategoryName");
    UI_GetTextFromID(idCategoryName).SetText(sText, FALSE);
}
void sUpdateCategoryDescription(const UIID & idParent, const CRefString & sText)
{
    UIID idEffects;
    idEffects.SetFromParent(idParent, "LayoutRoot.ItemCategory.ScrollBox.CategoryAttributes");
    UI_GetTextFromID(idEffects).SetText(sText.str(), FALSE);
}
void sUpdateGemCategoryUI(const UIID & idRoot, const ItemCategory eCategory)
{
    static const int32 GEM_TYPE_LOOKUP[] = {GEM_RUBY, GEM_AMETHYST, GEM_EMERALD, GEM_TOPAZ, GEM_DIAMOND};
    ShowDetailsMode(idRoot, ITEM_CATEGORY_VIEW);
    UIID idGemBackground = UIIDCreateFromParent(idRoot, ".LayoutRoot.ItemCategory.GemCategoryBackground");
    UI_GetWindowFromID(idGemBackground).SetActive(TRUE);
    UIID idCosmeticBackground = UIIDCreateFromParent(idRoot, ".LayoutRoot.ItemCategory.GemCategoryBackground.CosmeticsDetailsBackground");
    UI_GetWindowFromID(idCosmeticBackground).SetActive(FALSE);
    UIID idItemBackground = UIIDCreateFromParent(idRoot, ".LayoutRoot.ItemCategory.ItemCategoryBackground");
    UI_GetWindowFromID(idItemBackground).SetActive(FALSE);
    sShowTheme(idGemBackground, eCategory);
    CRefString sGemTypeName;
    CRefString sEffects;
    SNO snoStringlistConsoleInventory = GlobalSNOGet(GLOBALSNO_STRINGLIST_CONSOLE_INVENTORY);
    if (eCategory == CATEGORY_UPGRADEABLE_JEWEL) {
        sGemTypeName = ItemTypeGetDescription(GlobalGBIDGet(GLOBALGBID_ITEMTYPE_UPGRADEABLE_JEWEL), IQL_LEGENDARY, ANCIENT_RANK_NONE, FALSE, FALSE);
        sEffects = StringListFetch(snoStringlistConsoleInventory, CRefString().Format("%sEffectsGeneral", SymbolToName(ITL_UPGRADEABLE_JEWEL, gItemLabels)));
    } else {
        sGemTypeName = SymbolToName(GEM_TYPE_LOOKUP[eCategory - CATEGORY_RUBY], gGemTypeNames);
        sEffects = StringListFetch(snoStringlistConsoleInventory, CRefString().Format("%sEffectsGeneral", sGemTypeName.str()));
        sGemTypeName = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_ITEM_TYPES), sGemTypeName.str());
    }
    sUpdateCategoryName(idRoot, sGemTypeName.str());
    sUpdateCategoryDescription(idRoot, sEffects);
}
void sUpdateShardCategoryUI(const UIID & idRoot)
{
    ShowDetailsMode(idRoot, ITEM_CATEGORY_VIEW);
    UIID idGemBackground = UIIDCreateFromParent(idRoot, ".LayoutRoot.ItemCategory.GemCategoryBackground");
    UI_GetWindowFromID(idGemBackground).SetActive(TRUE);
    UIID idCosmeticBackground = UIIDCreateFromParent(idRoot, ".LayoutRoot.ItemCategory.GemCategoryBackground.CosmeticsDetailsBackground");
    UI_GetWindowFromID(idCosmeticBackground).SetActive(FALSE);
    UIID idItemBackground = UIIDCreateFromParent(idRoot, ".LayoutRoot.ItemCategory.ItemCategoryBackground");
    UI_GetWindowFromID(idItemBackground).SetActive(FALSE);
    sShowTheme(idGemBackground, CATEGORY_SHARD);
    sUpdateCategoryName(idRoot, StringListFetch("ConsoleInventory:CategoryShardsTitle"));
    sUpdateCategoryDescription(idRoot, StringListFetch("ConsoleInventory:ShardEffectsGeneral"));
}
void sUpdateItemCategoryUI(const UIID & idRoot, const ItemCategory eCategory)
{
    if (eCategory == CATEGORY_NONE) return;
    static const LPCSTR arszTitleLabels[] = {"CategoryTreasureTitle", "CategoryCosmeticsTitle", "CategoryItems2Title", "CategoryGemTitle", "CategoryCraftingTitle"};
    static const LPCSTR arszDescriptionLabels[] = {"CategoryTreasureDescription", "CategoryCosmeticsDescription", "CategoryItems2Description", "CategoryGemDescription", "CategoryCraftingDescription"};
    ShowDetailsMode(idRoot, ITEM_CATEGORY_VIEW);
    UIID idGemBackground = UIIDCreateFromParent(idRoot, ".LayoutRoot.ItemCategory.GemCategoryBackground");
    UI_GetWindowFromID(idGemBackground).SetActive(FALSE);
    UIID idItemBackground = UIIDCreateFromParent(idRoot, ".LayoutRoot.ItemCategory.ItemCategoryBackground");
    UI_GetWindowFromID(idItemBackground).SetActive(TRUE);
    UIID idIcons = UIIDCreateFromParent(idItemBackground, ".ItemIcons");
    sShowTheme(idIcons, eCategory);
    if (eCategory >= CATEGORY_TREASURE && eCategory <= CATEGORY_CRAFTING) {
        CRefString sTitle = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CONSOLE_INVENTORY), arszTitleLabels[eCategory]);
        sUpdateCategoryName(idRoot, sTitle);
        CRefString sDescription = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CONSOLE_INVENTORY), arszDescriptionLabels[eCategory]);
        sUpdateCategoryDescription(idRoot, sDescription);
    }
}
void sUpdateCosmeticCategoryUI(const UIID & idRoot, const ItemCategory eCategory)
{
    static const int32 COSMETIC_TYPE_LOOKUP[] = {COSMETIC_ITEM_WINGS, COSMETIC_ITEM_PET, COSMETIC_ITEM_PENNANT, COSMETIC_ITEM_FRAME};
    static const LPCSTR arszDescriptionLabels[] = {"CategoryCosmeticWings", "CategoryCosmeticPets", "CategoryCosmeticPennants", "CategoryCosmeticFrames"};
    ShowDetailsMode(idRoot, ITEM_CATEGORY_VIEW);
    UIID idItemBackground = UIIDCreateFromParent(idRoot, ".LayoutRoot.ItemCategory.ItemCategoryBackground");
    UI_GetWindowFromID(idItemBackground).SetActive(FALSE);
    UIID idGemBackground = UIIDCreateFromParent(idRoot, ".LayoutRoot.ItemCategory.GemCategoryBackground");
    UI_GetWindowFromID(idGemBackground).SetActive(TRUE);
    sShowTheme(idGemBackground, eCategory);
    UIID idCosmeticBackground = UIIDCreateFromParent(idRoot, ".LayoutRoot.ItemCategory.GemCategoryBackground.CosmeticsDetailsBackground");
    UI_GetWindowFromID(idCosmeticBackground).SetActive(TRUE);
    CRefString sCosmeticTypeName;
    int32 nCategoryIndex = eCategory - CATEGORY_WINGS;
    sCosmeticTypeName = SymbolToName(COSMETIC_TYPE_LOOKUP[nCategoryIndex], gCosmeticTypeNames);
    sCosmeticTypeName = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CONSOLE_INVENTORY), sCosmeticTypeName.str());
    sUpdateCategoryName(idRoot, sCosmeticTypeName.str());
    sUpdateCategoryDescription(idRoot, StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CONSOLE_INVENTORY), arszDescriptionLabels[nCategoryIndex]));
}
}
