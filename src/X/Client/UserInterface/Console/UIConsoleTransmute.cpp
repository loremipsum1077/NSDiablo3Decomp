#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../../Contrib/Contrib/blz/include/blz/vector.h"
#include "../../CGame.h"
#include "../../Console/ControllerUtils.h"
#include "ConsoleScreenManager.h"
#include "ControllerActions.h"
#include "../Controls/UIConsoleListboxControl.h"
#include "../Controls/UIControlControl.h"
#include "../Controls/UIWindowControl.h"
#include "../Controls/UIRActorControl.h"
#include "../UI.h"
#include "../UIID.h"
#include "../../../Common/CommonTypes.h"
#include "../../../Common/GameBalance.h"
#include "../../../Common/Definitions/GameBalanceDefinition.h"
#include "../../../Common/Crafting/ItemCrafting.h"
#include "../../../Common/ActorCommonData.h"
#include "../../../Common/Player.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/Networking/GameMessage.h"
#include "../../../Common/Definitions/StringListDefinition.h"
#include "../../../Sigma/RefString.h"
#include "../../Sound/Sound.h"
#include "../Controls/UITextControl.h"
#include "UIConsoleUtils.h"
#include "../../../Common/SNO/SymbolicValue.h"
#include "../../../Sigma/TypeDescriptor/SymbolicValue.h"
#include "../../../../BlizzardCore/Include/Blizzard/Time.h"
#include "../Interface/UIFunctions.h"
#include "../Interface/UITransmuteCommon.h"
// Declared here because their headers are not adopted yet.
// X/Client/UserInterface/UIIDs.h
extern UIID UIWND_KANAIS_CUBE_FX; extern UIID UIWND_KANAIS_CUBE_FX_UBER; extern UIID UIDLG_KANAIS_CUBE; extern UIID UILIST_KANAIS_CUBE_RECIPES; extern UIID UILIST_KANAIS_CUBE_INGREDIENTS; extern UIID UILIST_KANAIS_CUBE_INGREDIENTS_COMPARE;
extern UIID UIWND_KANAIS_CUBE_INGREDIENTS_COMPARE_CONTAINER; extern UIID UIWND_KANAIS_CUBE_RECIPE_DETAILS_CONTAINER; extern UIID UIWND_KANAIS_CUBE_INGREDIENTS_DETAILS_CONTAINER; extern UIID UIWND_KANAIS_CUBE_DETAILS_CONTAINER; extern UIID UITXT_KANAIS_CUBE_DIRECTIONS;
extern UIID UITXT_KANAIS_CUBE_RECIPE_DETAILS_FLAVOR_TITLE; extern UIID UITXT_KANAIS_CUBE_RECIPE_DETAILS_RECIPE_TITLE; extern UIID UITXT_KANAIS_CUBE_RECIPE_DETAILS_FLAVOR_TEXT; extern UIID UISTACKPANEL_KANAIS_CUBE_RECIPE_DETAILS_INGREDIENTS;
namespace UIInventory { namespace Console { CRefString GetItemDescription(const ACDID idACDPlayer, const ACDID idACDItem, const BOOL bShowStatRanges); void UpdateViewItemUI(const UIID & idItemUI, const ActorCommonData & tACDItem, const ACDID idACDPlayer, const ACDID idACDOwner, const int32 eFilterType, const BOOL bIsSelected, const BOOL bIsDetailedView, const BOOL bIsNew, const BOOL bIdentifyAllowed, const Blizzard::Time::Timestamp nExpiration);
void UpdateViewItemSelectionUI(const UIID & idItemUI, const BOOL bIsSelected); } }  // UIConsoleInventoryViewUtilities.cpp
namespace UITransmuteRecipes { void DisplayIngredients(const TransmuteRecipe * pdefRecipe, const UIID & idFlavorTitle, const UIID & idRecipeTitle, const UIID & idFlavorText, const UIID & idIngredients, const LPCSTR sTemplate, UIFunctionPtr pfnOnTooltip, ActorCommonData * ptACDPlayer); }  // UITransmuteRecipesCommon.cpp
extern UIID UISCROLL_KANAIS_CUBE_RECIPE_DETAILS; extern UIID UISCROLL_KANAIS_CUBE_RESULTS_ITEM; extern UIID UISCROLL_KANAIS_CUBE_INGREDIENTS_COMPARE;
UIRActor & UI_GetRActorFromID(const UIID & id); const UIID & UI_GetControllerFocus();  // X/Client/UserInterface/UI.cpp
namespace UIUtils { namespace Console { void ScrollContentInScrollBox(const UIID & idScrollBox, const float flDelta); } }  // UIConsoleUtils.cpp
struct TransmuteResultsMessage;
namespace UITransmute { bool IsUberRecipe(TransmuteRecipeAction eAction); void PlatformInitializeForGame(); void PlatformDisposeForGame(); void OpenForController(int32 nControllerIndex); void Close(); void OnUpdate(const UIID & id);
void OnButton1(const UIID & id, const int32 nControllerIndex); void OnButton2(const UIID & id, const int32 nControllerIndex); void OnButton3(const UIID & id, const int32 nControllerIndex); void OnButton4(const UIID & id, const int32 nControllerIndex);
void OnL2(const UIID &, const int32 nControllerIndex); void OnR2(const UIID &, const int32 nControllerIndex); void DrawGrid(const uint8, float & flZSlice); void OnTransmuteResults(const TransmuteResultsMessage & tMessage); void OnTransmuteFailed(const IntDataMessage & tMessage); }  // X/Client/UserInterface/Interface/UITransmuteCommon.cpp
namespace UITransmute { void RestoreItems(); void Unlock(); void Reset(ActorCommonData * ptACDPlayer); }  // UITransmuteCommon.cpp
namespace UIFastEquip { namespace Console { void CloseForPlayer(const int32 nPlayerIndex); } } /* UIConsoleFastEquip.cpp */ class UITutorial;
#include "../Interface/UITutorials.h"
extern UITutorialManager gtTutorialMgr; /* UITutorialManager.h */ // X/Common/Player.h:318
namespace UIGame { void DisplayError(const GameErrorData & tData, BOOL fDisplayBnetError, const Player * ptPlayer); } 
BOOL ItemCrafting_TransmuteIsValidIngredientInput(const TransmuteRecipeIngredient & tIngredient, const TransmuteInput & tInput);  // X/Common/Crafting/ItemCrafting.cpp:782
BOOL ItemPlayerHasExtractedLegendaryPower(const Player & tPlayer, GBID gbidItem);  // X/Common/Items.cpp:10795
int32 ItemGetNumberOfItemsInSameSet(const GBID gbid);  // X/Common/Items.cpp:9404
uint32 ItemGetRequiredAugmentJewelRankByGBID(GBID gbidAncientItem);  // X/Common/Items.cpp:11015
extern const SymbolicEntry gTransmuteRecipeIngredientTypes[17];  // X/Common/SNO/SymbolicValue.cpp:2907
#include "ConsoleModalNotificationManager.h"
// X/Client/UserInterface/Interface/UITransmuteRecipesCommon.h:28-52 (header not in src yet); the constructor body is inferred: its inlined instance only zeroes the vector (header line 52)
namespace UITransmuteRecipes { struct IngredientCount { GBID gbidItem; int32 nCount; IngredientCount(); }; struct CanCompleteCallbackData { ActorCommonData * ptACDPlayer; const TransmuteRecipeIngredient * ptIngredient; blz::vector<IngredientCount, blz::allocator<IngredientCount> > arIngredientCounts; int32 nSpecificGBIDCount; BOOL bCanComplete; CanCompleteCallbackData() {} };
void CanCompleteCallback(ACDID idACD, InventorySlotType eSlot, void * pData); }  // UITransmuteRecipesCommon.cpp:542
static void sBuildRecipesList(const Player * ptPlayer, blz::vector<int, blz::allocator<int> > & arRecipes);  // defined at line 1213
static BOOL sCanCompleteRecipe(GBID gbidRecipe);  // defined at line 1920
static void sAutoFillIngredients();  // defined at line 1811
enum ButtonActions {
    ACTION_SELECT = 1,
    ACTION_SELECT_RECIPE = 2,
    ACTION_SELECT_INGREDIENT = 3,
    ACTION_TRANSMUTE = 4,
    ACTION_CLEAR = 5,
    ACTION_CLOSE = 6,
    ACTION_BACK = 7,
    ACTION_ACCEPT_RESULTS = 8,
    ACTION_DETAILS = 9,
    ACTION_LIST = 10,
    ACTION_SCROLL_INGREDIENTS = 11,
    ACTION_SCROLL_DETAILS = 12,
    ACTION_SCROLL_NEW_ITEM_DETAILS = 13,
};

enum class TransmuteState : int {
    SELECTING_RECIPE = 0,
    SELECTING_INGREDIENTS = 1,
    READY_TO_TRANSMUTE = 2,
    TRANSMUTING = 3,
    PLAYING_CUBE_FX = 4,
    WAITING_FOR_RESULTS = 5,
    RESULTS_NEW_ITEM = 6,
    RESULTS_NEW_POWER = 7,
};

static void sSetupRecipeSelect(BOOL bOnOpen); static void sEnableTabNavigation(const BOOL bEnable); static void sBrowseIngredientsList(); static void sGoBack(); static void sClearCube(); static void sBrowseIngredientsDetails(); static void sSetupIngredientsSelect(); static void sOnRecipeSelectionChanged(const UIID & id, const int32 nOldVal, const int32 nNewVal);
static void sBuildIngredientsList(const Player * ptPlayer, blz::vector<ACDID, blz::allocator<ACDID> > & arIngredients);  // defined at line 1373
CurrencyType CurrencyFromGBID(GBID gbid);  // X/Common/Items.cpp:10444
static void sSelectRecipe(); static void sSelectIngredient(); static void sTransmute();  // defined at lines 1989, 2025, 2235
extern UIID UIWND_KANAIS_CUBE_RESULTS_ITEM; extern UIID UIWND_KANAIS_CUBE_RESULTS_ITEM_CONTAINER; extern UIID UIWND_KANAIS_CUBE_RESULTS_POWER_CONTAINER;  // X/Client/UserInterface/UIGlobals.cpp:1178-1181
extern UIID UIWND_KANAIS_CUBE_RESULTS_POWER_VISUAL; extern UIID UIWND_KANAIS_CUBE_RESULTS_POWER_VISUAL_SMALL; extern UIID UITXT_KANAIS_CUBE_RESULTS_POWER_NAME; extern UIID UITXT_KANAIS_CUBE_RESULTS_POWER_DETAILS;  // UIGlobals.cpp:1182-1185
SNO ItemGetLegendaryPower(GBID gbidItem); float ItemGetLegendaryPowerValue(GBID gbidItem); BOOL ItemIsDrawnAsLargeSlotItem(const GBID gbidItem);  // X/Common/Items.cpp:10556, 10606, 1858
void ACDInventoryGetItemDrawInfoWithGBID(const GBID gbidItem, ImageTextureFrame & tImageTexFrame, const ItemDrawType eDrawType);  // X/Client/CActorInventory.cpp:153
namespace UIUtils { namespace Console { BOOL CanScrollContentInScrollBox(const UIID & idScrollBox); } }  // X/Client/UserInterface/Console/UIConsoleUtils.cpp:950
static void sSetupResultsNewItem(ActorCommonData * ptACDItem); static void sSetupResultsNewPower(GBID gbidPower); static void sUpdateControllerActions();  // defined at lines 1464, 1509, 996
#include "../../../Common/UserInterface/UIToolTips.h"
#include "../../../Common/UserInterface/UITextControl.h"
#include "../Interface/UIEquipLegendaryPowersCommon.h"
#include "../../ImageCollection.h"
#include "../UIUtils.h"
#include "../../Sprite.h"
#include "../../Font/Font.h"
#include "../../../Sigma/MemoryPools.h"
ItemQualityLevel ItemGetDisplayedQualityLevel(const ActorCommonData & tACD);  // X/Common/Items.cpp:2208
namespace UIInventory { BOOL GetItemBkgrndImage(const ItemQualityLevel eIQL, ImageTextureFrame & tImageTexFrame, const BOOL fEquipped, const BOOL fLargeItem, const int32 nAncientRank, const BOOL fSeasonOnly, const BOOL fSanctified, const BOOL fPrimalized); }
extern "C" BOOL sub_710050e880(const ActorCommonData * ptACD);
extern "C" BOOL sub_710050ea10(const ActorCommonData * ptACD);
extern "C" BOOL sub_710050ecb0(const ActorCommonData * ptACD);  // X/Client/UserInterface/Interface/UIInventory.cpp:3043
void ACDInventoryGetItemDrawInfo(const ActorCommonData & tACDItem, ImageTextureFrame & tImageTexFrame, const ItemDrawType eDrawType);  // X/Client/CActorInventory.cpp:58
namespace UIInventory { void DrawItemSockets(ActorCommonData & tACD, XPooledDataIDList & listSocketedACDs, const PlayerClass eClassToDisplay, const BOOL fIsMale, const Rect2D & rectItemImage, const BOOL fEquipped, const float flAlpha, float & flZ); }  // X/Client/UserInterface/Interface/UIInventory.cpp:2417
extern UIID UIWND_KANAIS_CUBE_GRID;  // X/Client/UserInterface/UIGlobals.cpp:1186











































































class RecipeDataSource : public ConsoleListBoxDataSource {
public:
    virtual int32 GetNumItems() const;
    virtual UIControlSourceKey GetSourceKeyForItem(const int32 nIndex) const;
    virtual void InitializeItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex);
    virtual void UpdateItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex);
    GBID GetItem(const int32 nIndex) const;
    void Refresh();
    blz::vector<int, blz::allocator<int> > m_arRecipes;
};






















class IngredientsDataSource : public ConsoleListBoxDataSource {
public:
    virtual int32 GetNumItems() const;
    virtual UIControlSourceKey GetSourceKeyForItem(const int32 nIndex) const;
    virtual void InitializeItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex);
    virtual void UpdateItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex);
    ACDID GetItem(const int32 nIndex) const;
    int32 RemoveItem(ACDID idACD);
    void AddItem(ACDID idACD);
    void Refresh();
    blz::vector<ACDID, blz::allocator<ACDID> > m_arIngredients;
};

struct RecipeIngredientInfo {  // size 0x10
    TransmuteRecipeIngredient tIngredient;  // 0x00
    int32 nHave;  // 0x0c
    RecipeIngredientInfo() {}  // DWARF line 264; no out-of-line copy and the inlined instance leaves no code: empty body inferred
};

struct TransmuteGlobalsConsole {  // size 0x88
    TransmuteGlobalsConsole();
    ~TransmuteGlobalsConsole();
    int32 nControllerIndex;  // 0x00
    RecipeDataSource * ptRecipesDataSource;  // 0x08
    IngredientsDataSource * ptIngredientsDataSource;  // 0x10
    blz::vector<RecipeIngredientInfo, blz::allocator<RecipeIngredientInfo> > arRecipeIngredientInfo;  // 0x18
    int32 nCurrentIngredient;  // 0x30
    TransmuteState eState;  // 0x34
    uint32 nFXStartTime;  // 0x38
    uint32 nFXCoverageEndTime;  // 0x3c
    uint32 nFXEndTime;  // 0x40
    BOOL bMissingAutoFillIngredients;  // 0x44
    BOOL unk_48;  // 0x48
    BOOL bSpecialItemsInCube;  // 0x4c
    BOOL bBrowsingDetails;  // 0x50
    ActorCommonData * ptACDInsertedSpecialIngredient;  // 0x58
    TransmuteResultsMessage tResultsMsg;  // 0x60
};
static_assert(sizeof(TransmuteGlobalsConsole) == 0x88, "TransmuteGlobalsConsole");















































TransmuteGlobalsConsole * sgptTransmuteGlobalsConsole = NULL;





static const uint32 FX_DURATION = 120;
static const uint32 FX_UBER_DURATION = 240;
static const uint32 FX_COVERAGE_TIME = 60;
static const uint32 FX_UBER_COVERAGE_TIME = 180;




inline TransmuteGlobalsConsole::TransmuteGlobalsConsole()
    : nControllerIndex(-1), nCurrentIngredient(-1), eState(TransmuteState::SELECTING_RECIPE), nFXStartTime(0), nFXCoverageEndTime(0), nFXEndTime(0)
    , bMissingAutoFillIngredients(FALSE), unk_48(FALSE), bSpecialItemsInCube(FALSE), bBrowsingDetails(FALSE), ptACDInsertedSpecialIngredient(NULL)
{





    ptRecipesDataSource = new RecipeDataSource;
    ptIngredientsDataSource = new IngredientsDataSource;
    tResultsMsg.annItem = ANN_INVALID; tResultsMsg.gbidPower = GBID_NONE; tResultsMsg.eType = -1;
}







inline TransmuteGlobalsConsole::~TransmuteGlobalsConsole()
{
    ::delete ptRecipesDataSource; ptRecipesDataSource = NULL;
    ::delete ptIngredientsDataSource; ptIngredientsDataSource = NULL;
}





int32 RecipeDataSource::GetNumItems() const
{
    return m_arRecipes.size();
}





UIControlSourceKey RecipeDataSource::GetSourceKeyForItem(const int32 nIndex) const
{
    const LPCSTR ITEM_TEMPLATE = "ConsoleInventoryListRecipeTemplate:ConsoleInventoryListRecipeTemplate_main";
    const UIControlSourceKey ITEM_SOURCE_KEY = UIControlSourceKey::Parse(ITEM_TEMPLATE, SNO_NONE, NULL, NULL);
    return ITEM_SOURCE_KEY;
}








inline GBID RecipeDataSource::GetItem(const int32 nIndex) const
{
    return m_arRecipes[nIndex];
}





void RecipeDataSource::Refresh()

{
    Player *ptPlayer = PlayerGetByControllerIndex(sgptTransmuteGlobalsConsole->nControllerIndex);
    if (ptPlayer == NULL)
    {
        return;
    }



    sBuildRecipesList(ptPlayer, m_arRecipes);
}




void RecipeDataSource::InitializeItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex)
{
}










void RecipeDataSource::UpdateItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex)
{
    GBID gbidRecipe = m_arRecipes[nIndex];



    ScopedTransmuteRecipeEntryLockR pdefTransmuteRecipe(gbidRecipe);
    if (pdefTransmuteRecipe == NULL)
    {
        return;
    }
    Player * ptPlayer = PlayerGetByControllerIndex(sgptTransmuteGlobalsConsole->nControllerIndex);
    if (ptPlayer == NULL)
    {
        return;
    }
    UIID idRoot;
    idRoot.SetFromParent(idItemUI, "LayoutRoot");
    UIID idRecipeName;
    idRecipeName.SetFromParent(idRoot, "RecipeName");
    UIText & tRecipeName = UI_GetTextFromID(idRecipeName);
    CRefString sRecipeName;




    if (!pdefTransmuteRecipe->bHidden)
    {
        CRefString sTitleLabel = SymbolToName(pdefTransmuteRecipe->eAction, gTransmuteRecipeActionTypes);
        sRecipeName = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_TRANSMUTE_RECIPES), sTitleLabel);
    }
    else
    {
        sRecipeName = StringListFetch("Console_Kanais_Cube:HiddenRecipeTitle");
    }
    tRecipeName.SetText(sRecipeName, FALSE);
    UIID idSelected;
    idSelected.SetFromParent(idRoot, "Selected");




    BOOL bShowSelection = tListBox.GetSelectedIndex() == nIndex && UI_GetControllerFocus() == tListBox.GetID();
    UIInventory::Console::UpdateViewItemSelectionUI(idItemUI, bShowSelection);
}





int32 IngredientsDataSource::GetNumItems() const
{
    return m_arIngredients.size();
}





UIControlSourceKey IngredientsDataSource::GetSourceKeyForItem(const int32 nIndex) const
{
    const LPCSTR ITEM_TEMPLATE = "ConsoleInventoryListEquipmentTemplate:ConsoleInventoryListEquipmentTemplate_main";
    const UIControlSourceKey ITEM_SOURCE_KEY = UIControlSourceKey::Parse(ITEM_TEMPLATE, SNO_NONE, NULL, NULL);
    return ITEM_SOURCE_KEY;
}









inline ACDID IngredientsDataSource::GetItem(const int32 nIndex) const
{

    return m_arIngredients[nIndex];
}




inline int32 IngredientsDataSource::RemoveItem(
    ACDID idACD)
{
    int32 nIndex = blzext::vector_find(m_arIngredients, idACD);
    if (nIndex >= 0)
    {
        blzext::vector_erase_at(m_arIngredients, nIndex);
    }

    return nIndex;
}




inline void IngredientsDataSource::AddItem(
    ACDID idACD)
{
    m_arIngredients.push_back(idACD);
}




void IngredientsDataSource::Refresh()

{
    Player *ptPlayer = PlayerGetByControllerIndex(sgptTransmuteGlobalsConsole->nControllerIndex);
    if (ptPlayer == NULL)
    {
        return;
    }

    m_arIngredients.clear();

    sBuildIngredientsList(ptPlayer, m_arIngredients);
}




void IngredientsDataSource::InitializeItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex)
{
}










void IngredientsDataSource::UpdateItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex)
{
    ACDID idACD = m_arIngredients[nIndex];



    ActorCommonData * ptACDItem = ACDTryToGet(idACD);
    if (ptACDItem == NULL)
    {
        return;
    }
    Player * ptPlayer = PlayerGetByControllerIndex(sgptTransmuteGlobalsConsole->nControllerIndex);
    if (ptPlayer == NULL)
    {
        return;
    }


    BOOL bShowSelection = tListBox.GetSelectedIndex() == nIndex && UI_GetControllerFocus() == tListBox.GetID();
    UIInventory::Console::UpdateViewItemUI(idItemUI, *ptACDItem, ptPlayer->idPlayerACD, ptPlayer->idPlayerACD, -1, bShowSelection, FALSE, FALSE, TRUE, 0);
    UIInventory::Console::UpdateViewItemSelectionUI(idItemUI, bShowSelection);
}




static BOOL sIsActive()
{
    return UI_GetWindowFromID(UIDLG_KANAIS_CUBE).IsActive();
}




void UITransmute::PlatformInitializeForGame()
{


    sgptTransmuteGlobalsConsole = new TransmuteGlobalsConsole;
}



void UITransmute::PlatformDisposeForGame()
{
    if (sgptTransmuteGlobalsConsole)
    {
        delete sgptTransmuteGlobalsConsole;
    }
    sgptTransmuteGlobalsConsole = NULL;
}













void UITransmute::OpenForController(int32 nControllerIndex)
{

    if (!sIsActive())
    {
        CancelAllChannelingAbilities();
        sgptTransmuteGlobalsConsole->nControllerIndex = nControllerIndex;
        Player * ptPlayer = PlayerGetByControllerIndex(nControllerIndex);
        if (ptPlayer)
        {
            UIFastEquip::Console::CloseForPlayer(ptPlayer->nPlayerIndex);
        }
        sSetupRecipeSelect(TRUE);
        ::Console::Screens::ShowModalDialog(nControllerIndex, UIDLG_KANAIS_CUBE, FALSE, GetUIIDNone());
        UI_SetControllerFocus(UILIST_KANAIS_CUBE_RECIPES);
        gtTutorialMgr.ShowTutorial(UILIST_KANAIS_CUBE_RECIPES, GlobalSNOGet(GLOBALSNO_TUTORIAL_CONSOLE_CUBE_RECIPE_PROMPT), NULL, TRUE);










        sEnableTabNavigation(TRUE);
    }
}








static void sResetUI()
{
    UI_SetControllerFocus(GetUIIDNone());
}







void UITransmute::Close()
{
    Player * ptPlayer = PlayerGetByControllerIndex(sgptTransmuteGlobalsConsole->nControllerIndex);
    ptPlayer->idACDActiveVendor = ACDID_INVALID;
    sgptTransmuteGlobalsConsole->nControllerIndex = -1;

    sResetUI();

    UI_GetConsoleListBoxFromID(UILIST_KANAIS_CUBE_RECIPES).SetOnSelChangeFunction(NULL);
    UI_GetConsoleListBoxFromID(UILIST_KANAIS_CUBE_INGREDIENTS).SetOnSelChangeFunction(NULL);
    UI_GetConsoleListBoxFromID(UILIST_KANAIS_CUBE_INGREDIENTS_COMPARE).SetOnSelChangeFunction(NULL);

    ActorCommonData * ptACDPlayer = ptPlayer ? ACDTryToGet(ptPlayer->idPlayerACD) : NULL;
    UITransmute::Reset(ptACDPlayer);
    UI_GetRActorFromID(UIWND_KANAIS_CUBE_FX).SetActive(FALSE);
    UI_GetRActorFromID(UIWND_KANAIS_CUBE_FX_UBER).SetActive(FALSE);
    ::Console::Screens::CloseModalDialog(UIDLG_KANAIS_CUBE);
}











static void sProcessResults()
{

    if (sgptTransmuteGlobalsConsole->tResultsMsg.annItem != ANN_INVALID)
    {


        ACDID idACD = ACDFromANN(sgptTransmuteGlobalsConsole->tResultsMsg.annItem);
        ActorCommonData *ptACD = ACDTryToGet(idACD);
        if (ptACD)
        {
            sSetupResultsNewItem(ptACD);
        }
    }
    else if (sgptTransmuteGlobalsConsole->tResultsMsg.gbidPower != GBID_NONE)
    {
        UIID idText = UIIDCreateFromParent(UIDLG_KANAIS_CUBE, ".LayoutRoot.Navigation");
        gtTutorialMgr.ShowTutorial(idText, GlobalSNOGet(GLOBALSNO_TUTORIAL_CONSOLE_CUBE_POWER_EXTRACTED), NULL, TRUE);

        sSetupResultsNewPower(sgptTransmuteGlobalsConsole->tResultsMsg.gbidPower);
    }
    else if (sgptTransmuteGlobalsConsole->tResultsMsg.eType == TRA_OPEN_PORTAL_COW || sgptTransmuteGlobalsConsole->tResultsMsg.eType == TRA_OPEN_PORTAL_GREED ||
             sgptTransmuteGlobalsConsole->tResultsMsg.eType == TRA_OPEN_PORTAL_WHIMSEYSHIRE ||
             sgptTransmuteGlobalsConsole->tResultsMsg.eType == 16 || sgptTransmuteGlobalsConsole->tResultsMsg.eType == 17)
    {


        Console::Screens::CloseScreenSet(TRUE, TRUE);
    }
}




void UITransmute::OnUpdate(
    const UIID & id)
{
    if (!sIsActive())
    {
        return;
    }

    if (sgptTransmuteGlobalsConsole->eState == TransmuteState::PLAYING_CUBE_FX)
    {
        uint32 nNow = CGameGetRealTicks();
        if (nNow - sgptTransmuteGlobalsConsole->nFXStartTime > sgptTransmuteGlobalsConsole->nFXCoverageEndTime)
        {
            sgptTransmuteGlobalsConsole->eState = TransmuteState::WAITING_FOR_RESULTS;
            UITransmute::ClearItems();
        }
    }

    if (sgptTransmuteGlobalsConsole->eState == TransmuteState::WAITING_FOR_RESULTS)
    {
        uint32 nNow = CGameGetRealTicks();
        if (nNow - sgptTransmuteGlobalsConsole->nFXStartTime > sgptTransmuteGlobalsConsole->nFXEndTime)
        {
            sgptTransmuteGlobalsConsole->nFXStartTime = 0;

            sProcessResults();

            UI_GetRActorFromID(UIWND_KANAIS_CUBE_FX).SetActive(FALSE);
            UI_GetRActorFromID(UIWND_KANAIS_CUBE_FX_UBER).SetActive(FALSE);

            sEnableTabNavigation(TRUE);
            Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
        }
    }


    if (Console::Screens::GetActionManager().AreActionsDirty())
    {
        sUpdateControllerActions();
        Console::Screens::UpdateButtonLegendText();
        Console::Screens::GetActionManager().SetActionsDirtyFlag(FALSE);
    }
}




void UITransmute::OnButton1(
    const UIID & id,
    const int32 nControllerIndex)
{
    if (nControllerIndex != sgptTransmuteGlobalsConsole->nControllerIndex)
    {
        return;
    }

    Console::ControllerActionId idAction = Console::Screens::GetActionManager().GetActionForInput(Console::CONTROLLER_INPUT_BUTTON_1);
    switch (idAction) {
    case ACTION_SELECT_RECIPE:
        sSelectRecipe();
        break;

    case ACTION_SELECT_INGREDIENT:
        sSelectIngredient();
        break;

    case ACTION_TRANSMUTE:
        sTransmute();
        break;

    case ACTION_ACCEPT_RESULTS:
        sSetupIngredientsSelect();
    }  // the switch closes on 794 (the function's brace is DWARF 795): no break after the last case, inferred
}




void UITransmute::OnButton2(const UIID & id, const int32 nControllerIndex)
{
    if (sgptTransmuteGlobalsConsole->nControllerIndex == nControllerIndex)
    {
        ::Console::ControllerActionId idAction = ::Console::Screens::GetActionManager().GetActionForInput(::Console::CONTROLLER_INPUT_BUTTON_2);
        switch (idAction)
        {
        case ACTION_CLOSE:
            ::Console::Screens::CloseScreenSet(TRUE, TRUE);
            break;
        case ACTION_BACK:





            sGoBack();
            break;
        case ACTION_CLEAR:

            sClearCube();
            break;
        }
    }
}


void UITransmute::OnButton3(const UIID & id, const int32 nControllerIndex)
{
    if (sgptTransmuteGlobalsConsole->nControllerIndex != nControllerIndex)
    {
        return;
    }
    ::Console::ControllerActionId idAction = ::Console::Screens::GetActionManager().GetActionForInput(::Console::CONTROLLER_INPUT_BUTTON_3);
    if (idAction == ACTION_LIST)
    {




        sBrowseIngredientsList();
    }
    else if (idAction == ACTION_DETAILS)
    {
        sBrowseIngredientsDetails();
    }
}




void UITransmute::OnButton4(const UIID & id, const int32 nControllerIndex)
{
}










void UITransmute::OnL2(const UIID &, const int32 nControllerIndex)
{
    if (!::Console::ControllerUtils::IsButtonDown(nControllerIndex, 7))
    {
        ::Console::ControllerActionId idAction = ::Console::Screens::GetActionManager().GetActionForInput(::Console::CONTROLLER_INPUT_LEFT_TRIGGER);
        switch (idAction)
        {
        case ACTION_SCROLL_DETAILS:
            UIUtils::Console::ScrollContentInScrollBox(UISCROLL_KANAIS_CUBE_INGREDIENTS_COMPARE, -4.0f);
            break;
        case ACTION_SCROLL_INGREDIENTS:
            UIUtils::Console::ScrollContentInScrollBox(UISCROLL_KANAIS_CUBE_RECIPE_DETAILS, -4.0f);
            break;
        case ACTION_SCROLL_NEW_ITEM_DETAILS:
            UIUtils::Console::ScrollContentInScrollBox(UISCROLL_KANAIS_CUBE_RESULTS_ITEM, -4.0f);
            break;
        }
    }
}








void UITransmute::OnR2(const UIID &, const int32 nControllerIndex)
{
    if (!::Console::ControllerUtils::IsButtonDown(nControllerIndex, 6))
    {
        ::Console::ControllerActionId idAction = ::Console::Screens::GetActionManager().GetActionForInput(::Console::CONTROLLER_INPUT_RIGHT_TRIGGER);
        switch (idAction)
        {
        case ACTION_SCROLL_DETAILS:
            UIUtils::Console::ScrollContentInScrollBox(UISCROLL_KANAIS_CUBE_INGREDIENTS_COMPARE, 4.0f);
            break;
        case ACTION_SCROLL_INGREDIENTS:
            UIUtils::Console::ScrollContentInScrollBox(UISCROLL_KANAIS_CUBE_RECIPE_DETAILS, 4.0f);
            break;
        case ACTION_SCROLL_NEW_ITEM_DETAILS:
            UIUtils::Console::ScrollContentInScrollBox(UISCROLL_KANAIS_CUBE_RESULTS_ITEM, 4.0f);
            break;
        }
    }
}








static void sResetListBox(UIID & idListBox)
{
    UIConsoleListBox & tListBox = UI_GetConsoleListBoxFromID(idListBox);
    tListBox.ResetAllItems();
}





static void sRebuildRecipesLists()
{
    sgptTransmuteGlobalsConsole->ptRecipesDataSource->Refresh();


    sResetListBox(UILIST_KANAIS_CUBE_RECIPES);
}




static void sRebuildIngredientsLists()
{

    sgptTransmuteGlobalsConsole->ptIngredientsDataSource->Refresh();

    sResetListBox(UILIST_KANAIS_CUBE_INGREDIENTS);
    sResetListBox(UILIST_KANAIS_CUBE_INGREDIENTS_COMPARE);
}





static GBID sGetSelectedRecipe()
{
    UIConsoleListBox & tListBox = UI_GetConsoleListBoxFromID(UILIST_KANAIS_CUBE_RECIPES);
    int32 nSelectedIndex = tListBox.GetSelectedIndex();
    if (nSelectedIndex == -1)
    {
        return GBID_NONE;
    }


    return sgptTransmuteGlobalsConsole->ptRecipesDataSource->GetItem(nSelectedIndex);
}





static ACDID sGetSelectedIngredient()
{
    UIConsoleListBox * ptListBox;
    if (!sgptTransmuteGlobalsConsole->bBrowsingDetails)
    {
        ptListBox = &UI_GetConsoleListBoxFromID(UILIST_KANAIS_CUBE_INGREDIENTS);
    }
    else
    {
        ptListBox = &UI_GetConsoleListBoxFromID(UILIST_KANAIS_CUBE_INGREDIENTS_COMPARE);
    }

    int32 nSelectedIndex = ptListBox->GetSelectedIndex();
    if (nSelectedIndex == -1)
    {
        return ACDID_INVALID;
    }




    return sgptTransmuteGlobalsConsole->ptIngredientsDataSource->GetItem(nSelectedIndex);
}




static void sUpdateControllerActions()
{

    Console::ControllerActionManager &tActionManager = Console::Screens::GetActionManager();
    tActionManager.ClearAllActions();

    switch (sgptTransmuteGlobalsConsole->eState) {
    case TransmuteState::SELECTING_RECIPE: {
        tActionManager.SetAction(Console::CONTROLLER_INPUT_BUTTON_1, ACTION_SELECT_RECIPE, StringListFetch("Console_Kanais_Cube:Button_SelectRecipe"));
        tActionManager.SetAction(Console::CONTROLLER_INPUT_BUTTON_2, ACTION_CLOSE, StringListFetch("ConsoleUI:Close"));

        BOOL bCanScroll = UIUtils::Console::CanScrollContentInScrollBox(UISCROLL_KANAIS_CUBE_RECIPE_DETAILS);
        if (bCanScroll)
        {
            CRefString sScrollPrompt = StringListFetch("Console_Kanais_Cube:Button_ScrollRecipeDescription");
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_LEFT_TRIGGER, ACTION_SCROLL_INGREDIENTS, sScrollPrompt);
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_RIGHT_TRIGGER, ACTION_SCROLL_INGREDIENTS, sScrollPrompt);
        }
        break;
    }
    case TransmuteState::SELECTING_INGREDIENTS:
        if (sgptTransmuteGlobalsConsole->ptIngredientsDataSource->GetNumItems() > 0)
        {
            tActionManager.SetAction(Console::CONTROLLER_INPUT_BUTTON_1, ACTION_SELECT_INGREDIENT, StringListFetch("Console_Kanais_Cube:Button_SelectSpecialIngredient"));

            if (!sgptTransmuteGlobalsConsole->bBrowsingDetails)
            {
                tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_3, ACTION_DETAILS, StringListFetch("ConsoleUI:Details"));
            }
            else
            {
                tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_3, ACTION_LIST, StringListFetch("ConsoleUI:List"));

                BOOL bCanScroll = UIUtils::Console::CanScrollContentInScrollBox(UISCROLL_KANAIS_CUBE_INGREDIENTS_COMPARE);
                if (bCanScroll)
                {
                    CRefString sScrollPrompt = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CONSOLE_CHARACTER_DETAILS), "ScrollDetails");
                    tActionManager.SetAction(::Console::CONTROLLER_INPUT_LEFT_TRIGGER, ACTION_SCROLL_DETAILS, sScrollPrompt);
                    tActionManager.SetAction(::Console::CONTROLLER_INPUT_RIGHT_TRIGGER, ACTION_SCROLL_DETAILS, sScrollPrompt);
                }
            }
        }

        if (sgptTransmuteGlobalsConsole->bSpecialItemsInCube)
        {
            tActionManager.SetAction(Console::CONTROLLER_INPUT_BUTTON_2, ACTION_CLEAR, StringListFetch("Console_Kanais_Cube:Button_Clear"));
        }
        else
        {
            tActionManager.SetAction(Console::CONTROLLER_INPUT_BUTTON_2, ACTION_BACK, StringListFetch("Console_Kanais_Cube:Button_BackToRecipes"));
        }
        break;

    case TransmuteState::READY_TO_TRANSMUTE:
        tActionManager.SetAction(Console::CONTROLLER_INPUT_BUTTON_1, ACTION_TRANSMUTE, StringListFetch("Kanais_Cube:Transmute"));

        if (sgptTransmuteGlobalsConsole->bSpecialItemsInCube)
        {
            tActionManager.SetAction(Console::CONTROLLER_INPUT_BUTTON_2, ACTION_CLEAR, StringListFetch("Console_Kanais_Cube:Button_Clear"));
        }
        else
        {
            tActionManager.SetAction(Console::CONTROLLER_INPUT_BUTTON_2, ACTION_BACK, StringListFetch("Console_Kanais_Cube:Button_BackToRecipes"));
        }
        break;

    case TransmuteState::WAITING_FOR_RESULTS:
        tActionManager.SetAction(Console::CONTROLLER_INPUT_BUTTON_2, ACTION_BACK, StringListFetch("Console_Kanais_Cube:Button_BackToRecipes"));
        break;

    case TransmuteState::RESULTS_NEW_ITEM: {
        tActionManager.SetAction(Console::CONTROLLER_INPUT_BUTTON_1, ACTION_ACCEPT_RESULTS, StringListFetch("Console:Accept"));
        tActionManager.SetAction(Console::CONTROLLER_INPUT_BUTTON_2, ACTION_BACK, StringListFetch("Console_Kanais_Cube:Button_BackToRecipes"));

        BOOL bCanScroll = UIUtils::Console::CanScrollContentInScrollBox(UISCROLL_KANAIS_CUBE_RESULTS_ITEM);
        if (bCanScroll)
        {
            CRefString sScrollPrompt = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CONSOLE_CHARACTER_DETAILS), "ScrollDetails");
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_LEFT_TRIGGER, ACTION_SCROLL_NEW_ITEM_DETAILS, sScrollPrompt);
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_RIGHT_TRIGGER, ACTION_SCROLL_NEW_ITEM_DETAILS, sScrollPrompt);
        }
        break;
    }
    case TransmuteState::RESULTS_NEW_POWER:
        tActionManager.SetAction(Console::CONTROLLER_INPUT_BUTTON_1, ACTION_ACCEPT_RESULTS, StringListFetch("Console:Accept"));
        tActionManager.SetAction(Console::CONTROLLER_INPUT_BUTTON_2, ACTION_BACK, StringListFetch("Console_Kanais_Cube:Button_BackToRecipes"));
        break;
    default:
        break;
    }
}

static void sOnRecipeSelectionChanged(const UIID & id, const int32 nOldVal, const int32 nNewVal)
{
    if (sgptTransmuteGlobalsConsole->eState != TransmuteState::SELECTING_RECIPE)
    {
        return;
    }





    GBID gbidRecipe = sGetSelectedRecipe();
    if (gbidRecipe == GBID_NONE)
    {
        return;
    }

    ScopedTransmuteRecipeEntryLockR pdefTransmuteRecipe(gbidRecipe);
    if (pdefTransmuteRecipe == NULL)
    {
        return;
    }
    Player * ptPlayer = PlayerGetByControllerIndex(sgptTransmuteGlobalsConsole->nControllerIndex);






    ActorCommonData * ptACDPlayer = ptPlayer ? ACDTryToGet(ptPlayer->idPlayerACD) : NULL;
    if (ptACDPlayer == NULL)
    {
        return;
    }
    UIUtils::Console::ResetScrollBox(UISCROLL_KANAIS_CUBE_RECIPE_DETAILS);




    UITransmuteRecipes::DisplayIngredients(pdefTransmuteRecipe, UITXT_KANAIS_CUBE_RECIPE_DETAILS_FLAVOR_TITLE, UITXT_KANAIS_CUBE_RECIPE_DETAILS_RECIPE_TITLE, UITXT_KANAIS_CUBE_RECIPE_DETAILS_FLAVOR_TEXT, UISTACKPANEL_KANAIS_CUBE_RECIPE_DETAILS_INGREDIENTS, "ConsoleKanais_Cube:ConsoleKanais_Cube_main.LayoutRoot.TemplateContainer", NULL, ptACDPlayer);
    BOOL bActive = UI_GetWindowFromID(UIWND_KANAIS_CUBE_RECIPE_DETAILS_CONTAINER).IsActive();
    bActive = UI_GetWindowFromID(UITXT_KANAIS_CUBE_RECIPE_DETAILS_FLAVOR_TITLE).IsActive();
    UIID idBox;
    idBox.SetFromParent(UIWND_KANAIS_CUBE_RECIPE_DETAILS_CONTAINER, "ScrollBox");
    bActive = UI_GetWindowFromID(idBox).IsActive();
    UIID idPanel;
    idPanel.SetFromParent(UIWND_KANAIS_CUBE_RECIPE_DETAILS_CONTAINER, "ScrollBox.Panel");
    bActive = UI_GetWindowFromID(idPanel).IsActive();
    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
}

















static void sOnIngredientDetailsSelectionChanged(const UIID & id, const int32 nOldVal, const int32 nNewVal)
{


    if (nNewVal == -1 || sgptTransmuteGlobalsConsole->eState != TransmuteState::SELECTING_INGREDIENTS)
    {
        return;
    }







    Player * ptPlayer = PlayerGetByControllerIndex(sgptTransmuteGlobalsConsole->nControllerIndex);
    if (ptPlayer == NULL)
    {
        return;
    }

    UIUtils::Console::ResetScrollBox(UISCROLL_KANAIS_CUBE_INGREDIENTS_COMPARE);


    UIID idStats = GetUIIDNone();
    idStats.SetFromParent(UISCROLL_KANAIS_CUBE_INGREDIENTS_COMPARE, "CompareAffixes");
    CRefString sItemDescription = UIInventory::Console::GetItemDescription(ptPlayer->idPlayerACD, sGetSelectedIngredient(), FALSE);
    UI_GetTextFromID(idStats).SetText(sItemDescription, FALSE);

    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
}





static int32 sRecipeSort(
    const void * gbidArg1,
    const void * gbidArg2)
{
    GBID gbidRecipe1 = *(const GBID *)gbidArg1;
    GBID gbidRecipe2 = *(const GBID *)gbidArg2;

    ScopedTransmuteRecipeEntryLockR pdefRecipeEntry1(gbidRecipe1);
    ScopedTransmuteRecipeEntryLockR pdefRecipeEntry2(gbidRecipe2);


    if (pdefRecipeEntry1->nPage != pdefRecipeEntry2->nPage)
    {
        return pdefRecipeEntry1->nPage - pdefRecipeEntry2->nPage;
    }

    return 0;
}




static void sBuildRecipesList(
    const Player * ptPlayer,
    blz::vector<int, blz::allocator<int> > & arRecipes)
{
    arRecipes.clear();



    GBHandleList listGBHandles(GBGetHandlePool());
    GBEnumerate(GB_TRANSMUTE_RECIPES, listGBHandles);

    POS pos = listGBHandles.GetHeadPosition();

    while (pos)
    {
        GBHandle hGB = listGBHandles.GetNext(pos);
        ScopedTransmuteRecipeEntryLockR pdefTransmuteRecipe(hGB.gbid);
        if (pdefTransmuteRecipe == NULL)
        {
            continue;
        }


        if (pdefTransmuteRecipe->bHidden && !sCanCompleteRecipe(hGB.gbid) && !AppDrawFlagIsSet(APP_DRAW_KANAIS_HIDDEN_RECIPES_DEBUG_BIT))
        {
            continue;
        }

        arRecipes.push_back(hGB.gbid);
    }


    blzext::vector_sort(arRecipes, sRecipeSort);
}

struct SpecialIngredientCallbackData
{
    ActorCommonData * ptACDPlayer;
    blz::vector<ACDID, blz::allocator<ACDID> > * ptarIngredients;
    int32 nIngredientIndex;

    SpecialIngredientCallbackData()  // DWARF line 1254; no inlined instance survives in sBuildIngredientsList: initialisers inferred (dead stores)
        : ptACDPlayer(NULL)
        , ptarIngredients(NULL)
        , nIngredientIndex(-1)
    {
    }
};




extern "C" BOOL sub_71004f88d0(const ActorCommonData * ptACDItem);
extern "C" int32 sub_71006a5a50(int32 nJewelRank);

static void sSpecialIngredientsCallback(
    ACDID idACDSquare,
    InventorySlotType,
    void * pData)
{
    SpecialIngredientCallbackData * ptCallbackData = (SpecialIngredientCallbackData *)pData;

    RecipeIngredientInfo &tInfo = sgptTransmuteGlobalsConsole->arRecipeIngredientInfo[ptCallbackData->nIngredientIndex];


    ActorCommonData * ptACDItem = ACDTryToGet(idACDSquare);
    if (ptACDItem == NULL)
    {
        return;
    }

    if (ItemIsUnidentified(*ptACDItem))
    {
        return;
    }

    TransmuteInput tInput;
    tInput.ptACDItem = ptACDItem;
    tInput.bUsedInRecipe = FALSE;
    tInput.nStackCount = Math::Max<int32>(ptACDItem->GetStackCount(), 1);
    tInput.eIngredientType = TRIT_INVALID;

    if (ItemCrafting_TransmuteIsValidIngredientInput(tInfo.tIngredient, tInput))
    {
        GBID gbidRecipe = sGetSelectedRecipe();
        if (gbidRecipe == GBID_NONE)
        {
            return;
        }

        ScopedTransmuteRecipeEntryLockR pdefTransmuteRecipe(gbidRecipe);
        if (pdefTransmuteRecipe == NULL)
        {
            return;
        }



        if (ptACDItem->AttributesGetInt(ATTRIBUTE_CONSOLE_PROMO_ITEM))
        {
            return;
        }



        switch (pdefTransmuteRecipe->eAction) {
        case TRA_EXTRACT_LEGENDARY_POWER: {
            Player *ptPlayer = PlayerGetByControllerIndex(sgptTransmuteGlobalsConsole->nControllerIndex);
            if (!ptPlayer ||
                ItemPlayerHasExtractedLegendaryPower(*ptPlayer, ptACDItem->hGB.gbid) ||
                ItemHasLabel(ptACDItem->hGB.gbid, ITL_FOLLOWER_SPECIAL) ||
                ItemHasLabel(ptACDItem->hGB.gbid, ITL_HEALTHPOTION))
            {
                return;
            }
            break;
        }
        case TRA_REMOVE_LEVEL_REQ:
            if (ptACDItem->AttributesGetInt(ATTRIBUTE_REMOVE_LEVEL_REQUIREMENT))
            {
                return;
            }
            break;

        case TRA_CONVERT_SET_ITEM:
            if (ItemGetNumberOfItemsInSameSet(ptACDItem->hGB.gbid) < 3)
            {
                return;
            }
            break;

        case TRA_AUGMENT_ARMOR:
            if (tInfo.tIngredient.eType == TRIT_UPGRADED_LEGENDARY_GEM)
            {
                int32 nJewelRank = ptACDItem->AttributesGetInt(ATTRIBUTE_JEWEL_RANK);
                if (sub_71004f88d0(ptACDItem))
                    nJewelRank = sub_71006a5a50(nJewelRank);



                blz::vector<ACDID, blz::allocator<ACDID> > arItemsInCube;
                UITransmute::GetItemsInTheCube(arItemsInCube);

                for (uint32 nItem = 0; nItem < arItemsInCube.size(); nItem++)
                {
                    ACDID idAncientItem = arItemsInCube[nItem];
                    ActorCommonData * ptACDAncient = ACDTryToGet(idAncientItem);
                    if (ptACDAncient && ItemIsAncientRank(*ptACDAncient))
                    {
                        uint32 nRequiredRank = ItemGetRequiredAugmentJewelRankByGBID(ptACDAncient->hGB.gbid);
                        if (nJewelRank < (int32)nRequiredRank)  // the compare is signed (b.lt at +0x2f0, line 0): cast inferred
                        {
                            return;
                        }
                    }
                }
            }
            break;
        }
        ptCallbackData->ptarIngredients->push_back(ptACDItem->id);
    }
}



static void sBuildIngredientsList(
    const Player * ptPlayer,
    blz::vector<ACDID, blz::allocator<ACDID> > & arIngredients)
{





    int32 nCurrentIngredient = sgptTransmuteGlobalsConsole->nCurrentIngredient;


    RecipeIngredientInfo &tInfo = sgptTransmuteGlobalsConsole->arRecipeIngredientInfo[nCurrentIngredient];

    if (tInfo.tIngredient.eType == TRIT_SPECIFIC_GBID)
    {
        return;
    }

    ActorCommonData *ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
    if (ptACDPlayer == NULL)
    {
        return;
    }


    SpecialIngredientCallbackData tCallbackData;
    tCallbackData.ptACDPlayer = ptACDPlayer;
    tCallbackData.ptarIngredients = &arIngredients;
    tCallbackData.nIngredientIndex = nCurrentIngredient;

    ACDInventoryIterateItems(*ptACDPlayer, SLOT_PLAYER_BACKPACK, 4, sSpecialIngredientsCallback, &tCallbackData);
    ACDInventoryIterateItems(*ptACDPlayer, SLOT_PLAYER_SHARED_STASH, 4, sSpecialIngredientsCallback, &tCallbackData);


    CRefString sIngredientLabel = SymbolToName(tInfo.tIngredient.eType, gTransmuteRecipeIngredientTypes);
    CRefString sIngredient = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_TRANSMUTE_INGREDIENTS), sIngredientLabel);
    CRefString sInstructionText = LocalizedSprintf(StringListFetch("Console_Kanais_Cube:Directions_SelectAnIngredient"), sIngredient.str(), NULL);

    if (tInfo.tIngredient.nQuantity > 1)
    {
        sInstructionText.Format("%s x%d", sInstructionText.str(), tInfo.tIngredient.nQuantity);
    }

    UIText &tInstructionText = UI_GetTextFromID(UITXT_KANAIS_CUBE_DIRECTIONS);
    tInstructionText.SetText(sInstructionText, FALSE);
}




static BOOL sFindNextSpecialIngredient()
{

    int32 nCurrentIngredient = sgptTransmuteGlobalsConsole->nCurrentIngredient;
    int32 nTotalIngredients = sgptTransmuteGlobalsConsole->arRecipeIngredientInfo.size();
    for (int32 nIngredient = nCurrentIngredient + 1; nIngredient < nTotalIngredients; nIngredient++)
    {
        RecipeIngredientInfo &tIngredientInfo = sgptTransmuteGlobalsConsole->arRecipeIngredientInfo[nIngredient];
        if (tIngredientInfo.tIngredient.eType != TRIT_SPECIFIC_GBID)
        {
            sgptTransmuteGlobalsConsole->nCurrentIngredient = nIngredient;
            return TRUE;
        }
    }

    return FALSE;
}




static void sSetupReadyToTransmute()
{


    sgptTransmuteGlobalsConsole->eState = TransmuteState::READY_TO_TRANSMUTE;

    sgptTransmuteGlobalsConsole->ptIngredientsDataSource->m_arIngredients.clear();
    sResetListBox(UILIST_KANAIS_CUBE_INGREDIENTS);
    sResetListBox(UILIST_KANAIS_CUBE_INGREDIENTS_COMPARE);

    UIText &tInstructionText = UI_GetTextFromID(UITXT_KANAIS_CUBE_DIRECTIONS);
    tInstructionText.SetText(StringListFetch("Console_Kanais_Cube:Directions_ReadyToTransmute"), FALSE);

    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
}




static void sSetupResultsNewItem(
    ActorCommonData * ptACDItem)
{

    sgptTransmuteGlobalsConsole->eState = TransmuteState::RESULTS_NEW_ITEM;


    UI_GetWindowFromID(UIWND_KANAIS_CUBE_INGREDIENTS_DETAILS_CONTAINER).SetActive(FALSE);


    UI_GetWindowFromID(UIWND_KANAIS_CUBE_DETAILS_CONTAINER).SetActive(TRUE);


    UI_GetWindowFromID(UIWND_KANAIS_CUBE_RESULTS_ITEM_CONTAINER).SetActive(TRUE);


    UI_GetWindowFromID(UIWND_KANAIS_CUBE_RESULTS_POWER_CONTAINER).SetActive(FALSE);

    Player *ptPlayer = PlayerGetByControllerIndex(sgptTransmuteGlobalsConsole->nControllerIndex);
    if (ptPlayer == NULL)
    {
        return;
    }

    UIUtils::Console::ResetScrollBox(UISCROLL_KANAIS_CUBE_RESULTS_ITEM);




    UIID idStats = GetUIIDNone();
    idStats.SetFromParent(UISCROLL_KANAIS_CUBE_RESULTS_ITEM, "Affixes");
    CRefString sItemDescription = UIInventory::Console::GetItemDescription(ptPlayer->idPlayerACD, ptACDItem->id, FALSE);
    UI_GetTextFromID(idStats).SetText(sItemDescription, FALSE);


    UIInventory::Console::UpdateViewItemUI(UIWND_KANAIS_CUBE_RESULTS_ITEM, *ptACDItem, ptPlayer->idPlayerACD, ptPlayer->idPlayerACD, -1, FALSE, FALSE, FALSE, TRUE, 0);
    UIInventory::Console::UpdateViewItemSelectionUI(UIWND_KANAIS_CUBE_RESULTS_ITEM, FALSE);


    Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
}




static void sSetupResultsNewPower(
    GBID gbidPower)
{

    sgptTransmuteGlobalsConsole->eState = TransmuteState::RESULTS_NEW_POWER;


    UI_GetWindowFromID(UIWND_KANAIS_CUBE_INGREDIENTS_DETAILS_CONTAINER).SetActive(FALSE);


    UI_GetWindowFromID(UIWND_KANAIS_CUBE_DETAILS_CONTAINER).SetActive(TRUE);


    UI_GetWindowFromID(UIWND_KANAIS_CUBE_RESULTS_ITEM_CONTAINER).SetActive(FALSE);


    UI_GetWindowFromID(UIWND_KANAIS_CUBE_RESULTS_POWER_CONTAINER).SetActive(TRUE);


    CRefString sPowerName = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_ITEMS), GBIDToString(gbidPower).str());
    UI_GetTextFromID(UITXT_KANAIS_CUBE_RESULTS_POWER_NAME).SetText(sPowerName, FALSE);


    CRefString sPowerDesc;
    SNO snoPower = ItemGetLegendaryPower(gbidPower);
    if (snoPower != SNO_NONE)
    {
        CRefString sRawDesc = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_ITEM_PASSIVE_POWER_DESCRIPTIONS), SNOToString(POWER_SNO, snoPower).str());
        Set_Text_Color(sRawDesc, FONTCOLOR_LEGENDARY);


        float flPowerValue = ItemGetLegendaryPowerValue(gbidPower);

        AttribGroupID idAttribGroup = FastAttribGroupCreate(FAT_ITEM, -1);
        FastAttribSetValueFloat(idAttribGroup, MakeAttribKey(ATTRIBUTE_ITEM_POWER_PASSIVE, snoPower), flPowerValue);

        sPowerDesc = UIToolTips_ProcessTip(NULL, idAttribGroup, ATTRIBGROUPID_INVALID, snoPower, 1, sRawDesc, TTVC_NORMAL_COLORING, ItemResolveValueTagForItemPassivePowerDescription, (void *)(intptr_t)ATTRIBUTE_ITEM_POWER_PASSIVE, NULL);

        FastAttribGroupDelete(idAttribGroup);
    }
    UI_GetTextFromID(UITXT_KANAIS_CUBE_RESULTS_POWER_DETAILS).SetText(sPowerDesc, FALSE);


    ImageTextureFrame tImageTexFrame;
    ACDInventoryGetItemDrawInfoWithGBID(gbidPower, tImageTexFrame, ITEMDRAW_NORMAL);

    UIWindow &tIconWnd = UI_GetWindowFromID(UIWND_KANAIS_CUBE_RESULTS_POWER_VISUAL);
    UIWindow &tIconWndSmall = UI_GetWindowFromID(UIWND_KANAIS_CUBE_RESULTS_POWER_VISUAL_SMALL);

    if (ItemIsDrawnAsLargeSlotItem(gbidPower))
    {
        tIconWnd.SetActive(TRUE);
        tIconWnd.SetTextureFrame(tImageTexFrame);
        tIconWndSmall.SetActive(FALSE);
    }
    else
    {
        tIconWnd.SetActive(FALSE);
        tIconWndSmall.SetActive(TRUE);
        tIconWndSmall.SetTextureFrame(tImageTexFrame);
    }


    UIEquipLegendaryPowers::MarkPowerAsNew(gbidPower);

    Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
}




static void sSetupRecipeSelect(BOOL bOnOpen)
{
    sgptTransmuteGlobalsConsole->eState = TransmuteState::SELECTING_RECIPE;
    UI_SetControllerFocus(UILIST_KANAIS_CUBE_RECIPES);
    UIConsoleListBox & tRecipeListBox = UI_GetConsoleListBoxFromID(UILIST_KANAIS_CUBE_RECIPES);




    int32 nSelectedIndex = tRecipeListBox.GetSelectedIndex();
    tRecipeListBox.SetDataSource(*sgptTransmuteGlobalsConsole->ptRecipesDataSource);
    tRecipeListBox.SetOnSelChangeFunction(sOnRecipeSelectionChanged);
    UIWindow & tRecipeListContainer = UI_GetWindowFromID(tRecipeListBox.GetParent());
    tRecipeListContainer.SetActive(TRUE);
    UIConsoleListBox & tIngredientsListBox = UI_GetConsoleListBoxFromID(UILIST_KANAIS_CUBE_INGREDIENTS);
    UIWindow & tIngredientsListContainer = UI_GetWindowFromID(tIngredientsListBox.GetParent());
    tIngredientsListContainer.SetActive(FALSE);
    UI_GetWindowFromID(UIWND_KANAIS_CUBE_INGREDIENTS_COMPARE_CONTAINER).SetActive(FALSE);
    UIWindow & tRecipeDetails = UI_GetWindowFromID(UIWND_KANAIS_CUBE_RECIPE_DETAILS_CONTAINER);
    tRecipeDetails.SetActive(TRUE);
    UI_GetWindowFromID(UIWND_KANAIS_CUBE_INGREDIENTS_DETAILS_CONTAINER).SetActive(FALSE);
    UI_GetWindowFromID(UIWND_KANAIS_CUBE_DETAILS_CONTAINER).SetActive(FALSE);
    UIText & tInstructionText = UI_GetTextFromID(UITXT_KANAIS_CUBE_DIRECTIONS);
    tInstructionText.SetText(StringListFetch("Console_Kanais_Cube:Directions_SelectARecipe"), FALSE);














    sRebuildRecipesLists();
    if (!bOnOpen)
    {


        tRecipeListBox.SetSelectedIndex(Math::Min(nSelectedIndex, tRecipeListBox.GetNumItems() - 1));
    }
    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
}






static void sSetupIngredientsSelect()

{

    sgptTransmuteGlobalsConsole->eState = TransmuteState::SELECTING_INGREDIENTS;
    UI_SetControllerFocus(UILIST_KANAIS_CUBE_INGREDIENTS);


    UIConsoleListBox &tIngredientsListBox = UI_GetConsoleListBoxFromID(UILIST_KANAIS_CUBE_INGREDIENTS);
    tIngredientsListBox.SetDataSource(*sgptTransmuteGlobalsConsole->ptIngredientsDataSource);
    tIngredientsListBox.SetActive(TRUE);

    UIConsoleListBox &tIngredientsDetailsListBox = UI_GetConsoleListBoxFromID(UILIST_KANAIS_CUBE_INGREDIENTS_COMPARE);
    tIngredientsDetailsListBox.SetDataSource(*sgptTransmuteGlobalsConsole->ptIngredientsDataSource);
    tIngredientsDetailsListBox.SetOnSelChangeFunction(sOnIngredientDetailsSelectionChanged);

    UIWindow &tDetailsContainer = UI_GetWindowFromID(UIWND_KANAIS_CUBE_INGREDIENTS_DETAILS_CONTAINER);
    tDetailsContainer.SetActive(FALSE);


    UIWindow &tIngredientsListContainer = UI_GetWindowFromID(tIngredientsListBox.GetParent());
    tIngredientsListContainer.SetActive(TRUE);


    UIConsoleListBox &tRecipeListBox = UI_GetConsoleListBoxFromID(UILIST_KANAIS_CUBE_RECIPES);
    UIWindow &tRecipeListContainer = UI_GetWindowFromID(tRecipeListBox.GetParent());
    tRecipeListContainer.SetActive(FALSE);


    UI_GetWindowFromID(UIWND_KANAIS_CUBE_INGREDIENTS_DETAILS_CONTAINER).SetActive(TRUE);


    UI_GetWindowFromID(UIWND_KANAIS_CUBE_RECIPE_DETAILS_CONTAINER).SetActive(FALSE);


    UI_GetWindowFromID(UIWND_KANAIS_CUBE_DETAILS_CONTAINER).SetActive(FALSE);


    UIID idRecipeTitle;
    idRecipeTitle.SetFromParent(UIWND_KANAIS_CUBE_INGREDIENTS_DETAILS_CONTAINER, "ItemTitle");
    UIText &tRecipeTitle = UI_GetTextFromID(idRecipeTitle);

    GBID gbidRecipe = sGetSelectedRecipe();
    if (gbidRecipe == GBID_NONE)
    {
        return;
    }

    ScopedTransmuteRecipeEntryLockR pdefTransmuteRecipe(gbidRecipe);
    if (pdefTransmuteRecipe == NULL)
    {
        return;
    }

    if (pdefTransmuteRecipe->eAction == TRA_CONVERT_GEMS)
    {
        gtTutorialMgr.ShowTutorial(UIWND_KANAIS_CUBE_INGREDIENTS_DETAILS_CONTAINER, GlobalSNOGet(GLOBALSNO_TUTORIAL_CUBE_ESSENCE_OF_GEM), NULL, TRUE);
    }

    CRefString sRecipeName;
    if (!pdefTransmuteRecipe->bHidden)
    {
        CRefString sTitleLabel = SymbolToName(pdefTransmuteRecipe->eAction, gTransmuteRecipeActionTypes);
        sRecipeName = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_TRANSMUTE_RECIPES), sTitleLabel);
    }
    else
    {
        sRecipeName = StringListFetch("Console_Kanais_Cube:HiddenRecipeTitle");
    }
    tRecipeTitle.SetText(sRecipeName, FALSE);



    UIText &tInstructionText = UI_GetTextFromID(UITXT_KANAIS_CUBE_DIRECTIONS);
    tInstructionText.SetText("", FALSE);


    sgptTransmuteGlobalsConsole->arRecipeIngredientInfo.clear();
    sgptTransmuteGlobalsConsole->nCurrentIngredient = -1;
    sgptTransmuteGlobalsConsole->bSpecialItemsInCube = FALSE;  // lines 1712-1714 share one stp (line 1713); order inferred from the field order
    sgptTransmuteGlobalsConsole->bBrowsingDetails = FALSE;
    sgptTransmuteGlobalsConsole->ptACDInsertedSpecialIngredient = NULL;

    for (int32 nIngredient = 0; nIngredient < pdefTransmuteRecipe->nIngredientsCount; nIngredient++)
    {
        const TransmuteRecipeIngredient &tIngredient = pdefTransmuteRecipe->tIngredients[nIngredient];
        RecipeIngredientInfo tIngredientInfo;
        tIngredientInfo.nHave = 0;
        tIngredientInfo.tIngredient = tIngredient;
        sgptTransmuteGlobalsConsole->arRecipeIngredientInfo.push_back(tIngredientInfo);

        if (tIngredient.eType != TRIT_SPECIFIC_GBID && sgptTransmuteGlobalsConsole->nCurrentIngredient == -1)
        {
            sgptTransmuteGlobalsConsole->nCurrentIngredient = nIngredient;
        }
    }


    Player *ptPlayer = PlayerGetByControllerIndex(sgptTransmuteGlobalsConsole->nControllerIndex);
    ActorCommonData *ptACDPlayer = ptPlayer ? ACDTryToGet(ptPlayer->idPlayerACD) : NULL;
    UITransmute::Reset(ptACDPlayer);


    sAutoFillIngredients();


    if (sgptTransmuteGlobalsConsole->nCurrentIngredient != -1)
    {

        sRebuildIngredientsLists();
    }
    else
    {
        sSetupReadyToTransmute();
    }

    sBrowseIngredientsList();

    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
}

struct AutoFillCallbackData
{
    ActorCommonData * ptACDPlayer;
    blz::vector<ACDID, blz::allocator<ACDID> > arIngredients;
    GBID gbidItem;
    int32 nIngredientIndex;
    BOOL unk_28;

    AutoFillCallbackData()  // DWARF line 1761; its inlined instance only zeroes the vector (line 1764): scalar initialisers inferred (dead stores)
        : ptACDPlayer(NULL)
        , gbidItem(GBID_NONE)
        , nIngredientIndex(-1)
        , unk_28(FALSE)
    {
    }
};




static void sAutoFillCallback(
    ACDID idACDSquare,
    InventorySlotType,
    void * pData)
{
    AutoFillCallbackData * ptCallbackData = (AutoFillCallbackData *)pData;

    RecipeIngredientInfo &tInfo = sgptTransmuteGlobalsConsole->arRecipeIngredientInfo[ptCallbackData->nIngredientIndex];


    if (tInfo.nHave >= tInfo.tIngredient.nQuantity)  // the idACDSquare == ACDID_INVALID test of ACDTryToGet (line 1788) is hoisted onto this line
    {
        return;
    }


    ActorCommonData * ptACDItem = ACDTryToGet(idACDSquare);
    if (ptACDItem == NULL)
    {
        return;
    }


    const BOOL bUnidentified = ItemIsUnidentified(*ptACDItem);
    if (tInfo.tIngredient.unk_0c != -1 && tInfo.tIngredient.unk_0c > ItemGetAncientRank(*ptACDItem))
        return;
    if (tInfo.tIngredient.unk_10 != -1 && tInfo.tIngredient.unk_10 != ItemGetAncientRank(*ptACDItem))
        return;

    if (ptACDItem->hGB.gbid == ptCallbackData->gbidItem)
    {
        if (bUnidentified)
        {
            ptCallbackData->unk_28 = TRUE;
            return;
        }
        ptCallbackData->arIngredients.push_back(idACDSquare);
        tInfo.nHave += Math::Max<int32>(ptACDItem->GetStackCount(), 1);
    }
}





static void sAutoFillIngredients()
{

    Player *ptPlayer = PlayerGetByControllerIndex(sgptTransmuteGlobalsConsole->nControllerIndex);
    if (ptPlayer == NULL)
    {
        return;
    }

    ActorCommonData *ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
    if (ptACDPlayer == NULL)
    {
        return;
    }

    sgptTransmuteGlobalsConsole->bMissingAutoFillIngredients = FALSE;
    int32 nIngredientsCount = (int32)sgptTransmuteGlobalsConsole->arRecipeIngredientInfo.size();




    BOOL bFoundAll = TRUE;
    for (int32 nIngredient = 0; nIngredient < nIngredientsCount; nIngredient++)
    {
        const TransmuteRecipeIngredient &tIngredient = sgptTransmuteGlobalsConsole->arRecipeIngredientInfo[nIngredient].tIngredient;
        if (tIngredient.eType == TRIT_SPECIFIC_GBID)
        {
            GBID gbidItem = (GBID)tIngredient.nTypeValue;


            BOOL bCurrency = ItemIsCurrency(gbidItem);
            if (bCurrency)
            {
                CurrencyType eCurrency = CurrencyFromGBID(gbidItem);
                int64 nAmount = ptACDPlayer->GetCurrencyAmount(eCurrency);
                BOOL bFoundItem = nAmount > 0;



                UITransmute::AddItem(NULL, NULL, gbidItem, nAmount, TRIT_INVALID, FALSE, tIngredient.nQuantity);
                if (!bFoundItem)
                {
                    UIID idGrid = UIIDCreateFromParent(UIWND_KANAIS_CUBE_INGREDIENTS_DETAILS_CONTAINER, ".grid");
                    gtTutorialMgr.ShowTutorial(idGrid, GlobalSNOGet(GLOBALSNO_TUTORIAL_CONSOLE_CUBE_MISSING_INGREDIENT), NULL, TRUE);
                    bFoundAll = FALSE;
                }



                RecipeIngredientInfo &tInfo = sgptTransmuteGlobalsConsole->arRecipeIngredientInfo[nIngredient];
                tInfo.nHave = nAmount;
                if (tInfo.nHave < tInfo.tIngredient.nQuantity)
                {
                    sgptTransmuteGlobalsConsole->bMissingAutoFillIngredients = TRUE;
                }
            }
            else
            {

                AutoFillCallbackData tCallbackData;
                tCallbackData.ptACDPlayer = ptACDPlayer;
                tCallbackData.gbidItem = gbidItem;
                tCallbackData.nIngredientIndex = nIngredient;
                ACDInventoryIterateItems(*ptACDPlayer, SLOT_PLAYER_BACKPACK, 4, sAutoFillCallback, &tCallbackData);
                ACDInventoryIterateItems(*ptACDPlayer, SLOT_PLAYER_SHARED_STASH, 4, sAutoFillCallback, &tCallbackData);

                BOOL bFoundItem = FALSE;
                for (int32 nFoundItem = 0; nFoundItem < tCallbackData.arIngredients.size(); nFoundItem++)
                {
                    ActorCommonData *ptACD = ACDTryToGet(tCallbackData.arIngredients[nFoundItem]);
                    if (ptACD)
                    {
                        UITransmute::AddItem(ptACD, NULL, GBID_NONE, 0, TRIT_SPECIFIC_GBID, FALSE, 0);
                        bFoundItem = TRUE;
                    }
                }



                if (!bFoundItem)
                {
                    sgptTransmuteGlobalsConsole->unk_48 |= tCallbackData.unk_28;
                    UITransmute::AddItem(NULL, NULL, gbidItem, tIngredient.nQuantity, TRIT_INVALID, FALSE, 0);

                    UIID idGrid = UIIDCreateFromParent(UIWND_KANAIS_CUBE_INGREDIENTS_DETAILS_CONTAINER, ".grid");
                    gtTutorialMgr.ShowTutorial(idGrid, GlobalSNOGet(GLOBALSNO_TUTORIAL_CONSOLE_CUBE_MISSING_INGREDIENT), NULL, TRUE);
                    bFoundAll = FALSE;
                }



                RecipeIngredientInfo &tInfo = sgptTransmuteGlobalsConsole->arRecipeIngredientInfo[nIngredient];
                if (tInfo.nHave < tInfo.tIngredient.nQuantity)
                {
                    sgptTransmuteGlobalsConsole->bMissingAutoFillIngredients = TRUE;
                }
            }
        }
    }

   if (bFoundAll)  // the branch is at column 8: 3-space indent inferred
    {
        UIID idGrid = UIIDCreateFromParent(UIWND_KANAIS_CUBE_INGREDIENTS_DETAILS_CONTAINER, ".grid");
        gtTutorialMgr.ShowTutorial(idGrid, GlobalSNOGet(GLOBALSNO_TUTORIAL_CONSOLE_CUBE_ADD_ITEMS), NULL, TRUE);
    }
}




static BOOL sCanCompleteRecipe(
    GBID gbidRecipe)
{
    if (gbidRecipe == GBID_NONE)
    {
        return FALSE;
    }

    Player *ptPlayer = PlayerGetByControllerIndex(sgptTransmuteGlobalsConsole->nControllerIndex);
    if (ptPlayer == NULL)
    {
        return FALSE;
    }

    ActorCommonData *ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
    if (ptACDPlayer == NULL)
    {
        return FALSE;
    }

    ScopedTransmuteRecipeEntryLockR pdefTransmuteRecipe(gbidRecipe);
    if (pdefTransmuteRecipe == NULL)
    {
        return FALSE;
    }


    int32 nIngredientsCount = pdefTransmuteRecipe->nIngredientsCount;


    for (int32 nIngredient = 0; nIngredient < nIngredientsCount; nIngredient++)
    {
        const TransmuteRecipeIngredient & tIngredient = pdefTransmuteRecipe->tIngredients[nIngredient];

        UITransmuteRecipes::CanCompleteCallbackData tCallbackData;
        tCallbackData.ptACDPlayer = ptACDPlayer;
        tCallbackData.ptIngredient = &tIngredient;
        tCallbackData.bCanComplete = FALSE;
        tCallbackData.nSpecificGBIDCount = 0;

        ACDInventoryIterateItems(*ptACDPlayer, SLOT_PLAYER_BACKPACK, 4, UITransmuteRecipes::CanCompleteCallback, &tCallbackData);
        ACDInventoryIterateItems(*ptACDPlayer, SLOT_PLAYER_SHARED_STASH, 4, UITransmuteRecipes::CanCompleteCallback, &tCallbackData);

        if (!tCallbackData.bCanComplete)
        {
            return FALSE;
        }
    }

    return TRUE;
}




static void sGoBack()
{
    sSetupRecipeSelect(FALSE);
    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_FOCUS_BACK), cgtPSODefaults);
    Player * ptPlayer = PlayerGetByControllerIndex(sgptTransmuteGlobalsConsole->nControllerIndex);


    ActorCommonData * ptACDPlayer = ptPlayer ? ACDTryToGet(ptPlayer->idPlayerACD) : NULL;
    UITransmute::Reset(ptACDPlayer);
}




static void sSelectRecipe()
{

    if (sgptTransmuteGlobalsConsole->eState != TransmuteState::SELECTING_RECIPE)
    {
        return;
    }


    Player *ptPlayer = PlayerGetByControllerIndex(sgptTransmuteGlobalsConsole->nControllerIndex);
    if (ptPlayer)
    {
        ActorCommonData *ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
        if (ptACDPlayer && ACDIsParticipatingInTieredLootRun(*ptACDPlayer))
        {
            UIGame::DisplayError(GameErrorData(GAMEERROR_LOOTRUN_TIERED_CANNOT_DO_THAT), FALSE, ptPlayer);
            return;
        }
    }

    GBID gbidRecipe = sGetSelectedRecipe();
    if (gbidRecipe == GBID_NONE)
    {
        return;
    }

    UI_SetControllerFocus(GetUIIDNone());

    sSetupIngredientsSelect();

    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SELECT_GENERIC), cgtPSODefaults);
}




static void sSelectIngredient()
{

    if (sgptTransmuteGlobalsConsole->eState != TransmuteState::SELECTING_INGREDIENTS)
    {
        return;
    }




    if (sgptTransmuteGlobalsConsole->bMissingAutoFillIngredients)
    {
        UIGame::DisplayError(GameErrorData(GAMEERROR_TRANSMUTE_MISSING_CORE_INGREDIENTS), FALSE, NULL);
        return;
    }

    ActorCommonData * ptACD = ACDTryToGet(sGetSelectedIngredient());
    if (ptACD == NULL)
    {
        return;
    }


    int32 nCurrentIngredient = sgptTransmuteGlobalsConsole->nCurrentIngredient;
    RecipeIngredientInfo &tInfo = sgptTransmuteGlobalsConsole->arRecipeIngredientInfo[nCurrentIngredient];



    BOOL bItemSwapped = FALSE;
    if (sgptTransmuteGlobalsConsole->ptACDInsertedSpecialIngredient)
    {
        GBID gbidInsertedSpecialIngredient = sgptTransmuteGlobalsConsole->ptACDInsertedSpecialIngredient->hGB.gbid;

        if (gbidInsertedSpecialIngredient != ptACD->hGB.gbid ||
            // DWARF lines 2060-2065 carry no code; the condition resumes on 2066





            (ItemIsGem(*sgptTransmuteGlobalsConsole->ptACDInsertedSpecialIngredient) && ItemIsGem(*ptACD) &&
             ItemCrafting_GetGemQuality(gbidInsertedSpecialIngredient) != ItemCrafting_GetGemQuality(ptACD->hGB.gbid)))
        {






            bItemSwapped = TRUE;  // placement inferred: it lives in a register, set at the merge after the loop

            blz::vector<ACDID, blz::allocator<ACDID> > arItemsInCube;
            UITransmute::GetItemsInTheCube(arItemsInCube);
            for (int32 nIndex = 0; nIndex < arItemsInCube.size(); nIndex++)
            {
                ActorCommonData *ptACDItem = ACDTryToGet(arItemsInCube[nIndex]);
                if (ptACDItem->hGB.gbid == gbidInsertedSpecialIngredient)
                {
                    UITransmute::RemoveItem(ptACDItem->id);
                    sgptTransmuteGlobalsConsole->ptIngredientsDataSource->AddItem(ptACDItem->id);
                }
            }
        }
    }


    BOOL bSuccess = UITransmute::AddItem(ptACD, NULL, GBID_NONE, 0, tInfo.tIngredient.eType, TRUE, 0);
    if (!bSuccess)
    {
        return;
    }

    sgptTransmuteGlobalsConsole->bSpecialItemsInCube = TRUE;
    sgptTransmuteGlobalsConsole->ptACDInsertedSpecialIngredient = ptACD;


    UIConsoleListBox &tListBox = UI_GetConsoleListBoxFromID(UILIST_KANAIS_CUBE_INGREDIENTS);
    UIConsoleListBox &tListBoxDetails = UI_GetConsoleListBoxFromID(UILIST_KANAIS_CUBE_INGREDIENTS_COMPARE);
    int32 nOldFirstVisibleIndex = tListBox.GetFirstVisibleItemIndex();
    int32 nOldSelectedIndex = sgptTransmuteGlobalsConsole->ptIngredientsDataSource->RemoveItem(ptACD->id);
    sResetListBox(UILIST_KANAIS_CUBE_INGREDIENTS);
    sResetListBox(UILIST_KANAIS_CUBE_INGREDIENTS_COMPARE);


    int32 nNumItems = sgptTransmuteGlobalsConsole->ptIngredientsDataSource->GetNumItems();
    int32 nNewSelectedIndex = Math::Pin(nOldSelectedIndex, 0, nNumItems - 1);
    if (nOldFirstVisibleIndex >= 0 && nOldFirstVisibleIndex < nNumItems)
    {
        tListBox.SetFirstVisibleItemIndex(nOldFirstVisibleIndex);
        tListBoxDetails.SetFirstVisibleItemIndex(nOldFirstVisibleIndex);
    }

    tListBox.SetSelectedIndex(nNewSelectedIndex);
    tListBoxDetails.SetSelectedIndex(nNewSelectedIndex);


    if (bItemSwapped)
    {

        tInfo.nHave = 0;
    }
    tInfo.nHave += Math::Max<int32>(ptACD->GetStackCount(), 1);


    tListBox.SetActive(TRUE);
    UI_GetWindowFromID(UIWND_KANAIS_CUBE_INGREDIENTS_COMPARE_CONTAINER).SetActive(FALSE);
    sgptTransmuteGlobalsConsole->bBrowsingDetails = FALSE;


    if (tInfo.nHave >= tInfo.tIngredient.nQuantity)
    {
        sgptTransmuteGlobalsConsole->ptACDInsertedSpecialIngredient = NULL;
        BOOL bFoundAnotherSpecialIngredient = sFindNextSpecialIngredient();

        if (bFoundAnotherSpecialIngredient)
        {

            sRebuildIngredientsLists();
        }
        else
        {
            sSetupReadyToTransmute();
        }
    }

    Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
}




static void sClearCube()
{
    switch (sgptTransmuteGlobalsConsole->eState)
    {
    case TransmuteState::SELECTING_INGREDIENTS:
    case TransmuteState::READY_TO_TRANSMUTE:
    case TransmuteState::TRANSMUTING:
    case TransmuteState::WAITING_FOR_RESULTS:
        sSetupIngredientsSelect();
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_CONSOLE_CANCEL_RIFT_ENTRY), cgtPSODefaults);
        break;
    default:
        break;
    }
}





static void sBeginCubeFX(bool bIsUber)
{
    UI_GetRActorFromID(bIsUber ? UIWND_KANAIS_CUBE_FX_UBER : UIWND_KANAIS_CUBE_FX).SetActive(TRUE);



    sgptTransmuteGlobalsConsole->nFXStartTime = CGameGetRealTicks();
    sgptTransmuteGlobalsConsole->nFXCoverageEndTime = bIsUber ? FX_UBER_COVERAGE_TIME : FX_COVERAGE_TIME;
    sgptTransmuteGlobalsConsole->nFXEndTime = bIsUber ? FX_UBER_DURATION : FX_DURATION;
    sgptTransmuteGlobalsConsole->eState = TransmuteState::PLAYING_CUBE_FX;
}




static void sStartTransmute(
    const UIID &)
{
    if (sgptTransmuteGlobalsConsole->eState != TransmuteState::READY_TO_TRANSMUTE)
    {
        return;
    }


    Player *ptPlayer = PlayerGetByControllerIndex(sgptTransmuteGlobalsConsole->nControllerIndex);
    if (ptPlayer == NULL)
    {
        return;
    }

    ActorCommonData *ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
    if (ptACDPlayer == NULL)
    {
        return;
    }


    if (UITransmute::DoTransmute(ptPlayer))
    {

        sgptTransmuteGlobalsConsole->eState = TransmuteState::TRANSMUTING;
        Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);





        UITransmute::ReplaceItemsWithCopies();

        UITransmute::Lock(FX_DURATION);

        sEnableTabNavigation(FALSE);
    }
    else if (sgptTransmuteGlobalsConsole->unk_48)
    {
        UIGame::DisplayError(GameErrorData(GAMEERROR_TRANSMUTE_CANNOT_USE_UNIDENTIFIED_ITEM), FALSE, NULL);
    }
}




static void sTransmute()
{

    if (sgptTransmuteGlobalsConsole->eState != TransmuteState::READY_TO_TRANSMUTE)
    {
        return;
    }


    Player *ptPlayer = PlayerGetByControllerIndex(sgptTransmuteGlobalsConsole->nControllerIndex);
    if (ptPlayer == NULL)
    {
        return;
    }


    GBID gbidSelectedRecipe = sGetSelectedRecipe();
    if (gbidSelectedRecipe != GBID_NONE)
    {
        ScopedTransmuteRecipeEntryLockR pdefRecipe(gbidSelectedRecipe);
        if (pdefRecipe)  // no braces: the lock's scope closes on 2282, right after the switch (inferred)

        switch (pdefRecipe->eAction)
        {
            case TRA_EXTRACT_LEGENDARY_POWER: {
                NotificationCreationParams tParams;
                tParams.uszTitle = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GENERAL), "WarningTitle");
                tParams.uszMessage = StringListFetch("Kanais_Cube:ExtractLegendaryWarning");
                tParams.tCallbacks.pfnOnClickOkButton = sStartTransmute;
                tParams.nControllerIndex = ptPlayer->nControllerIndex;
                Console::Screens::GetModalNotificationManager().SetOkCancelNotification(tParams);
                return;
            }

            case TRA_OPEN_PORTAL_GREED:
            case TRA_OPEN_PORTAL_COW:
            case TRA_OPEN_PORTAL_WHIMSEYSHIRE:
            case 16:
            case 17: {

                NotificationCreationParams tParams;
                tParams.uszTitle = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GENERAL), "WarningTitle");
                tParams.uszMessage = StringListFetch("Kanais_Cube:ConsumeWarning");
                tParams.tCallbacks.pfnOnClickOkButton = sStartTransmute;
                tParams.nControllerIndex = ptPlayer->nControllerIndex;
                Console::Screens::GetModalNotificationManager().SetOkCancelNotification(tParams);
                return;
            }
        }
    }


    sStartTransmute(GetUIIDNone());
}


static void sBrowseIngredientsList()
{
    sgptTransmuteGlobalsConsole->bBrowsingDetails = FALSE;
    UIConsoleListBox & tListIngredients = UI_GetConsoleListBoxFromID(UILIST_KANAIS_CUBE_INGREDIENTS);
    UIConsoleListBox & tListDetails = UI_GetConsoleListBoxFromID(UILIST_KANAIS_CUBE_INGREDIENTS_COMPARE);
    UIWindow & tDetailsContainer = UI_GetWindowFromID(UIWND_KANAIS_CUBE_INGREDIENTS_COMPARE_CONTAINER);
    tListIngredients.SetActive(TRUE);
    tDetailsContainer.SetActive(FALSE);




    tListIngredients.SetSelectedIndex(tListDetails.GetSelectedIndex());
    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
}



static void sBrowseIngredientsDetails()
{
    sgptTransmuteGlobalsConsole->bBrowsingDetails = TRUE;
    UIConsoleListBox & tListIngredients = UI_GetConsoleListBoxFromID(UILIST_KANAIS_CUBE_INGREDIENTS);
    UIConsoleListBox & tListDetails = UI_GetConsoleListBoxFromID(UILIST_KANAIS_CUBE_INGREDIENTS_COMPARE);
    UIWindow & tDetailsContainer = UI_GetWindowFromID(UIWND_KANAIS_CUBE_INGREDIENTS_COMPARE_CONTAINER);
    tListIngredients.SetActive(FALSE);
    tDetailsContainer.SetActive(TRUE);




    tListDetails.SetSelectedIndex(tListIngredients.GetSelectedIndex());
    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
}



void UITransmute::DrawGrid(
    const uint8 ,
    float & flZSlice)
{
    Player *ptPlayer = PlayerGetByControllerIndex(sgptTransmuteGlobalsConsole->nControllerIndex);
    if (!ptPlayer)
    {
        return;
    }

    ActorCommonData *ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
    if (!ptACDPlayer)
    {
        return;
    }

    ImageTextureFrame tImageTexFrame = cgtImageTextureFrameNull;
    if (!ImageCollectionGetImageInfo("UI:InventoryGrid_Slot", tImageTexFrame))
    {
        return;
    }

    PlayerClass ePlayerClass = PlayerGetPlayerClass(ptPlayer);
    BOOL bPlayerIsMale = PlayerIsMale(*ptPlayer);

    SNO snoShader = GlobalSNOGet(GLOBALSNO_SHADER_SPRITE_TRANSPARENT);
    float flZ = UIDC_DIALOG_Z_START - UIDC_DIALOG_Z_INCREMENT * flZSlice + (2.0f * UIDC_CONTROL_Z_INCREMENT);  // columns: '+' at 43, '*' at 51; the first operand's rows all sit at column 17 (a macro in the original?)


    Rect2D tGridRect;
    UI_GetWindowFromID(UIWND_KANAIS_CUBE_GRID).GetWindowRect(tGridRect);

    Vector2D vGridSlotSize(tGridRect.Width() / 3.0f,
        tGridRect.Height() / 3.0f);

    // loop bound: the columns leave 19-21 characters for it (a named constant in the original?); the value is 3
    for (int32 y = 0; y < 3; ++y)
    {
        for (int32 x = 0; x < 3; ++x)
        {
            Rect2D tRectGridSlot = tGridRect;
            tRectGridSlot.left += (float)x * vGridSlotSize.x;
            tRectGridSlot.top += (float)y * vGridSlotSize.y;
            tRectGridSlot.right = tRectGridSlot.left + vGridSlotSize.x;
            tRectGridSlot.bottom = tRectGridSlot.top + vGridSlotSize.y;

            Rect2D tRectGridSlotPixels;
            ConvertUICToPixels(tRectGridSlot, tRectGridSlotPixels);

            SpriteDrawDeferred(tImageTexFrame, snoShader, tRectGridSlotPixels, crgbaWhite, crgbaBlack, flZ, NULL, SNO_NONE, NULL, FALSE, RENDERLAYER_INVALID, FALSE);
        }
    }




    for (int32 x = 0; x < 3; ++x)
    {
        for (int32 y = 0; y < 3; ++y)
        {
            IVector2D vItemGridSlot(x, y);
            ACDID idACDItem = GetItemACDIDInGridSlot(vItemGridSlot);
            ActorCommonData * ptACDItem = ACDTryToGet(idACDItem);
            GBID gbidFakeItem = GetItemGBIDInGridSlot(vItemGridSlot);
            BOOL bFakeItem = gbidFakeItem != GBID_NONE;
            if (!ptACDItem && !bFakeItem)
            {
                continue;
            }

            uint64 uStackSize = bFakeItem ? GetItemGBIDStackCountInGridSlot(vItemGridSlot) : ptACDItem->GetStackCount();

            BOOL bCurrency = bFakeItem && ItemIsCurrency(gbidFakeItem);
            // line 2400 is an if: its last branch sits at the condition start (column 17), the || at 42, the call at 66
            BOOL bInsufficientIngredients = FALSE;
            if (bFakeItem && (!bCurrency || uStackSize < (uint64)GetCurrencyCountNeededForRecipeInGridSlot(vItemGridSlot)))
            {
                bInsufficientIngredients = TRUE;
            }
            else
            {

                TransmuteRecipeIngredientType eType = GetItemTypeInGridSlot(vItemGridSlot);
                int32 nNumIngredients = (int32)sgptTransmuteGlobalsConsole->arRecipeIngredientInfo.size();
                for (int32 nIngredient = 0; nIngredient < nNumIngredients; ++nIngredient)
                {
                    RecipeIngredientInfo & tInfo = sgptTransmuteGlobalsConsole->arRecipeIngredientInfo[nIngredient];

                    if ((eType == TRIT_SPECIFIC_GBID && tInfo.tIngredient.nTypeValue == ptACDItem->hGB.gbid) ||
                        (eType != TRIT_SPECIFIC_GBID && tInfo.tIngredient.eType == eType))
                    {

                        uStackSize = tInfo.tIngredient.nQuantity;
                        if (tInfo.nHave < tInfo.tIngredient.nQuantity)
                        {
                            bInsufficientIngredients = TRUE;
                        }
                        break;
                    }
                }
            }


            ItemQualityLevel eIQL = bFakeItem ? IQL_INVALID : ItemGetDisplayedQualityLevel(*ptACDItem);
            BOOL bLargeItem = bFakeItem ? ItemIsDrawnAsLargeSlotItem(gbidFakeItem) : ItemIsDrawnAsLargeSlotItem(ptACDItem->hGB.gbid);
            float flItemZ = flZ - UIDC_CONTROL_Z_INCREMENT;

            // a large item covers this slot and the one above it: draw it once
            if (y > 0 &&
                ((!bFakeItem && GetItemACDIDInGridSlot(vItemGridSlot - IVector2D(0, 1)) == idACDItem) ||
                 (bFakeItem && GetItemGBIDInGridSlot(vItemGridSlot - IVector2D(0, 1)) == gbidFakeItem)))
            {
                continue;
            }


            Rect2D tRectItemSlot;
            tRectItemSlot.left = tGridRect.left + vGridSlotSize.x * vItemGridSlot.x + 2.0f;
            tRectItemSlot.top = tGridRect.top + vGridSlotSize.y * vItemGridSlot.y + 2.0f;
            tRectItemSlot.bottom = tRectItemSlot.top + vGridSlotSize.y * (bLargeItem ? 2 : 1) - 4.0f;
            tRectItemSlot.right = tRectItemSlot.left + vGridSlotSize.x - 4.0f;

            Rect2D tRectItemSlotPixels;
            ConvertUICToPixels(tRectItemSlot, tRectItemSlotPixels);


            ImageTextureFrame tImageTexFrameBkgrnd;
            if (UIInventory::GetItemBkgrndImage(eIQL, tImageTexFrameBkgrnd, FALSE, bLargeItem, 0, sub_710050e880(ptACDItem), sub_710050ea10(ptACDItem), sub_710050ecb0(ptACDItem)))
            {
                SpriteDrawDeferred(tImageTexFrameBkgrnd, cgtImageTextureFrameNull, snoShader,
                    tRectItemSlotPixels, crgbaWhite, crgbaBlack, flItemZ, 1.0f, NULL, RENDERLAYER_INVALID, FALSE);
                flItemZ -= UIDC_CONTROL_Z_INCREMENT;
            }


            ImageTextureFrame tImageTexFrameItem = cgtImageTextureFrameNull;
            if (bFakeItem)
            {
                ACDInventoryGetItemDrawInfoWithGBID(gbidFakeItem, tImageTexFrameItem, ITEMDRAW_NORMAL);
            }
            else
            {
                ACDInventoryGetItemDrawInfo(*ptACDItem, tImageTexFrameItem, ITEMDRAW_NORMAL);
            }

            SNO snoItemShader = bInsufficientIngredients ? GlobalSNOGet(GLOBALSNO_SHADER_SPRITE_TRANSPARENT_INACTIVE) : GlobalSNOGet(GLOBALSNO_SHADER_SPRITE_TRANSPARENT);

            SpriteDrawDeferred(tImageTexFrameItem, cgtImageTextureFrameNull, snoItemShader,
                tRectItemSlotPixels, crgbaWhite, crgbaBlack, flItemZ, 1.0f, NULL, RENDERLAYER_INVALID, FALSE);
            flItemZ -= UIDC_CONTROL_Z_INCREMENT;


            if (!bFakeItem && ItemHasSockets(*ptACDItem))
            {

                DataIDMemoryPool * pMemoryPool;

                if (ThreadGlobals::Get().m_pDataIDMemoryPool)
                {
                    pMemoryPool = ThreadGlobals::Get().m_pDataIDMemoryPool;
                }

                else
                {
                    pMemoryPool = MemoryPoolsGetDATAIDListPool();
                }
                XPooledDataIDList listSocketedACDs(pMemoryPool);

                int32 nNumSockets = ptACDItem->AttributesGetInt(ATTRIBUTE_SOCKETS);
                for (int32 nSocket = 0; nSocket < nNumSockets; ++nSocket)
                {
                    ACDID idACDSocketedItem = ACDInventoryGetSquareItem(*ptACDItem, SLOT_ITEM_SOCKETS, IVector2D(0, nSocket));
                    listSocketedACDs.AddTail_Tracked(idACDSocketedItem);
                }

                UIInventory::DrawItemSockets(*ptACDItem, listSocketedACDs, ePlayerClass, bPlayerIsMale, tRectItemSlot, FALSE, 1.0f, flItemZ);
            }

            // inferred: the condition has no row of its own (cbnz/cbz on line 0 at +0xaf0)
            if (bInsufficientIngredients || uStackSize > 0)
            {
                RGBAColor rgbaColor = crgbaWhite;
                if (bInsufficientIngredients)
                {
                    rgbaColor = crgbaRed;
                }

                CHAR uzTemp[32];
                Blizzard::String::Format(uzTemp, sizeof(uzTemp), "%llu", uStackSize);

                Vector2D vTextSize;
                TextRenderProperties tProps(GlobalSNOGet(GLOBALSNO_FONT_SANS_SERIF), 16, 1.0f, 1.0f);
                FontStringGetRenderedSizeUIC(tProps, FormattedText(uzTemp, 5, FALSE), vTextSize);

                Vector2D vTextJustifyPos;
                vTextJustifyPos.Set(vTextSize.x + 8, vTextSize.y + 8);

                Vector2D vTextPos;
                vTextPos.Set(tRectItemSlot.right - vTextJustifyPos.x, tRectItemSlot.bottom - vTextJustifyPos.y);
                // the original passes uzTemp straight through (columns 54/43 are uzTemp itself): FormattedText(LPCSTR, 5, FALSE) via default arguments src lacks
                FontStringDrawUIC(tProps, FormattedText(uzTemp, 5, FALSE), vTextPos, rgbaColor, &crgbaBlack, flItemZ, TRUE, NULL, RENDERLAYER_INVALID, FALSE);
                flItemZ -= UIDC_CONTROL_Z_INCREMENT;
            }
        }
    }


}


void UITransmute::OnTransmuteResults(const TransmuteResultsMessage & tMessage)
{
    if (sgptTransmuteGlobalsConsole->eState == TransmuteState::TRANSMUTING)
    {
        sgptTransmuteGlobalsConsole->tResultsMsg = tMessage;







        sBeginCubeFX(UITransmute::IsUberRecipe(tMessage.eType));
    }
}


void UITransmute::OnTransmuteFailed(const IntDataMessage & tMessage)
{
    if (tMessage.nData >= 0 && tMessage.nData < GAMEERROR_COUNT)
    {

        UIGame::DisplayError(GameErrorData(tMessage.nData), FALSE, NULL);
    }
    UITransmute::RestoreItems();
    UITransmute::Unlock();


    sClearCube();

    sEnableTabNavigation(TRUE);
}


static void sEnableTabNavigation(
    const BOOL bEnable)
{
    ::Console::Screens::EnableScreen(::Console::Screens::KANAIS_CUBE_SCREEN_SET, 0, bEnable);
    ::Console::Screens::EnableScreen(::Console::Screens::KANAIS_CUBE_SCREEN_SET, 1, bEnable);

    UIID idNavBar = UIIDCreateFromParent(UIDLG_KANAIS_CUBE, ".LayoutRoot.Navigation");
    UI_GetWindowFromID(UIIDCreateFromParent(idNavBar, ".KeyBinding1")).SetActive(bEnable);
    UI_GetWindowFromID(UIIDCreateFromParent(idNavBar, ".KeyBinding2")).SetActive(bEnable);
}
