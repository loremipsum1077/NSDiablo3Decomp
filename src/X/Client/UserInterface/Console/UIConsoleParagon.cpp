// Original file: X/Client/UserInterface/Console/UIConsoleParagon.cpp
//
// The console paragon screen: the category ring, the attribute list and point spending.

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../../Contrib/Contrib/blz/include/blz/vector.h"
#include "ConsoleInputUtilities.h"
#include "Wrappers/RingWrapper.h"
#include "../Controls/UIConsoleListboxControl.h"
#include "../../../Common/GameBalance.h"
#include "../../Console/ControllerUtils.h"
#include "ConsoleScreenManager.h"
#include "ControllerActions.h"
#include "ConsoleUIRActorManager.h"
#include "../UI.h"
#include "../UIID.h"
#include "../Controls/UIButtonControl.h"
#include "../Controls/UIRActorControl.h"
#include "../Controls/UITextControl.h"
#include "../Controls/UIProgressBarControl.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/Game.h"
#include "../../../Common/ActorCommonData.h"
#include "../../../Common/Player.h"
#include "../../../Common/Attributes/FastAttribConstants.h"
#include "../../../Common/Attributes/FastAttribute.h"
#include "../../../Common/VisualInventory.h"
#include "../../../Common/Definitions/GameBalanceDefinition.h"
#include "../../../Common/Definitions/GlobalDefinition.h"
#include "../../../Common/Networking/GameMessage.h"
#include "../../../Common/Definitions/HeroDefinition.h"
#include "../../CPlayer.h"
#include "../../CGame.h"
#include "../../Sound/Sound.h"
#include "../../../Sigma/RefString.h"
#include "../../../Sigma/Memory/SigmaMemory.h"
#include "../../../Sigma/Math/Transform.h"

#include <float.h>

// Declared here because their headers are not adopted yet; each comment names the defining file.
#include "../Interface/UIParagonCommon.h"
#include "../../../Common/UserInterface/UIToolTips.h"
#include "../../ImageCollection.h"
#include "../../../Sigma/UnicodeText.h"
#include "../../../Common/Definitions/StringListDefinition.h"
CRefString StringListFetch(LPCSTR szLabel);  // X/Common/Definitions/StringListDefinition.cpp
CRefString StringListFetch(SNO snoStringList, LPCSTR szLabel);  // X/Common/Definitions/StringListDefinition.cpp
CRefString LocalizedSprintf(LPCSTR szFormat, ...);  // X/Common/Definitions/StringListDefinition.cpp
void Set_Text_Color(CRefString & sText, int32 eColor);  // X/Client/UserInterface/Controls/UITextControl.cpp
CRefString UIText_ValueToGameCurrency(uint64 nValue, BOOL bShort);  // X/Client/UserInterface/Controls/UITextControl.cpp
void MessageSendToServer(GameMessageType eType, void * pMessage, uint32 dwMessageSize, const Player * ptPlayer);  // X/Client/Network/CGameMessage.cpp
void UI_SetControllerFocus(const UIID & id);  // UI.cpp
UIRActor & UI_GetRActorFromID(const UIID & id);  // UI.cpp
UIProgressBar & UI_GetProgressBarFromID(const UIID & id);  // UI.cpp
UIButton & UI_GetButtonFromID(const UIID & id);  // UI.cpp
UIControl & UI_GetControlFromID(const UIID & id);  // UI.cpp
LightID LightAdd(uint32 dwFlags, SNO snoLight, const PRTransform & tTransform, BOOL bUnused, RenderContext eContext);  // X/Client/Light.cpp
SNO UI_GetDefaultHeroLight();  // X/Client/UserInterface/UI.cpp
PlayerClass PlayerGetPlayerClass(const Player * ptPlayer);  // X/Common/Player.cpp
namespace UIUtils { namespace Console { BOOL ShowTieredRiftMessage(const Player * ptPlayer); SNO GetObserverForPlayerClass(PlayerClass ePlayerClass); } }  // UIConsoleUtils.cpp
namespace UIInventory { namespace Console { void UpdatePlayerActorUI(const UIID & idRActor, VisualInventory & tVisualInventory, const Player & tPlayer, uint32 dwFlags); void UpdateNavBarTabState(const Player * ptPlayer, const UIID & idParent, LPCSTR szNavBar, int32 nTab); } }  // UIInventoryConsole.cpp
namespace UIConfirmation { void Show(LPCSTR szText, UIFunctionPtr pfnOk, UIFunctionPtr pfnCancel, BOOL bUnused, BOOL bModal, int32 nControllerIndex, int32 nTimeout); }  // UIConfirmation.cpp
namespace UIGame { void DisplayError(const GameErrorData & tError, BOOL bUnused, const Player * ptPlayer); }  // UIGame.cpp
extern UIID UIWND_PARAGON_CONSOLE;  // X/Client/UserInterface/UIIDs.cpp
extern UIID UIWND_PARAGON_CONSOLE_RING;  // X/Client/UserInterface/UIIDs.cpp
extern UIID UIWND_PARAGON_CONSOLE_RING_CURSOR;  // X/Client/UserInterface/UIIDs.cpp
extern UIID UIWND_PARAGON_CONSOLE_RING_GLOW;  // X/Client/UserInterface/UIIDs.cpp
extern UIID UIWND_PARAGON_CONSOLE_MENU_GLOW;  // X/Client/UserInterface/UIIDs.cpp
extern UIID UILIST_PARAGON_CONSOLE_ATTRIBUTE_LIST;  // X/Client/UserInterface/UIIDs.cpp
extern UIID UITXT_PARAGON_CONSOLE_INFO_TITLE;  // X/Client/UserInterface/UIIDs.cpp
extern UIID UPROGRESS_PARAGON_CONSOLE_PARAGON_EXP;  // X/Client/UserInterface/UIIDs.cpp
extern UIID UIRACTOR_CONSOLE_RING;  // X/Client/UserInterface/UIIDs.cpp
extern UIID UITXT_CONSOLE_PLAYER_LABEL_RING;  // X/Client/UserInterface/UIIDs.cpp
extern UIID UITXT_PARAGON_CONSOLE_PARAGON_LEVEL; // UIIDs.cpp
extern UIID UIWND_PARAGON_CONSOLE_ROOT; // UIIDs.cpp
extern UIID UITXT_PARAGON_CONSOLE_UNSPENT_CURRENT_CATEGORY; // UIIDs.cpp























































namespace UIParagonBonuses {
namespace Console {
void UpdateItemAttributeFixedDetails(const UIID & idItemUI, const int32 nAttribute); void UpdateItemAttributeDynamicDetails(const UIID & idItemUI, const int32 nAttribute, const BOOL bSelected);

enum ButtonActions {
    ACTION_CHOOSE_CATEGORY = 1,
    ACTION_OPEN_CATEGORY = 2,
    ACTION_CLOSE_CATEGORY = 3,
    ACTION_RESET_CATEGORY_POINTS = 4,
    ACTION_CHOOSE_ATTRIBUTE = 5,
    ACTION_ADD_OR_REMOVE_POINTS = 6,
    ACTION_CONFIRM_ATTRIBUTE_POINTS = 7,
    ACTION_CLOSE = 8,
    ACTION_SELECT_ATTRIBUTE = 9,
};


enum SelectionMode {
    CATEGORY_SELECTION_MODE = 0,
    ATTRIBUTE_SELECTION_MODE = 1,
};

static const ::Console::AnalogStickInputFilter::Rule::ID RULE_STICK_STRICT_LEFT = 10;
static const ::Console::AnalogStickInputFilter::Rule::ID RULE_STICK_STRICT_RIGHT = 11;
static const float STICK_SENSITIVITY = 0.3f;

static const int32 ATTRIBUTE_TICKS = 50;




class AttributesDataSource : public ConsoleListBoxDataSource {
public:
    AttributesDataSource() {}
    virtual int32 GetNumItems() const; virtual UIControlSourceKey GetSourceKeyForItem(const int32 nIndex) const;
    virtual void InitializeItemUI(const UIConsoleListBox &, const UIID & idItemUI, const int32); virtual void UpdateItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex);
};



int32 AttributesDataSource::GetNumItems() const
{
    return 4;
}




UIControlSourceKey AttributesDataSource::GetSourceKeyForItem(const int32 nIndex) const
{
    return UIControlSourceKey::Parse("ConsoleParagon:ConsoleParagon_main.LayoutRoot.AttributeList.AttributeItem", -1, NULL, NULL);
}




void AttributesDataSource::InitializeItemUI(const UIConsoleListBox &, const UIID & idItemUI, const int32)
{
}






void AttributesDataSource::UpdateItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex)
{
    BOOL bSelected = tListBox.GetSelectedIndex() == nIndex;
    UpdateItemAttributeFixedDetails(idItemUI, nIndex);
    UpdateItemAttributeDynamicDetails(idItemUI, nIndex, bSelected);
}





class ParagonGlobals {
public:
    AttributesDataSource m_tDataSource;
    ::Console::RingWrapper m_tRingUI;
    ::Console::AnalogStickInputFilter m_tRingStickInputFilter;
    ::Console::AnalogStickInputFilter m_tStickInputFilter;
    int32 m_nTimeSinceLastPointSpend;
    int32 m_nLastSpendDirection;
    BOOL m_bFast;
    BOOL m_bClear;
    const Player * m_ptPlayer;
    SelectionMode m_eSelectionMode;
    int32 m_nCategory;
    int32 m_nAttribute;
    int32 m_nAttributePointsSpending;
    int32 m_nAttributePointsSpent[4][8];
    int32 m_nCategoryPointsAvailable[4];
    GBID m_gbidAttributes[4][8];
    BOOL m_bAttributeDataDirty[4];












    ParagonGlobals()
        : m_nTimeSinceLastPointSpend(0)
        , m_nLastSpendDirection(0)
        , m_bFast(FALSE)
        , m_bClear(TRUE)
        , m_ptPlayer(NULL)
        , m_eSelectionMode(CATEGORY_SELECTION_MODE)
        , m_nCategory(-1)
        , m_nAttribute(-1)
        , m_nAttributePointsSpending(0)
    {



        InitAnalogStickRules();
        SigmaMemoryClear(m_nCategoryPointsAvailable, sizeof(m_nCategoryPointsAvailable));
        SigmaMemoryClear(m_nAttributePointsSpent, sizeof(m_nAttributePointsSpent));
        SigmaMemoryClear(m_gbidAttributes, sizeof(m_gbidAttributes));
        SigmaMemoryClear(m_bAttributeDataDirty, sizeof(m_bAttributeDataDirty));
    }
    BOOL IsActive() const;
    void InitRingUI();
    void InitParagonInformation(const Player & tPlayer);
    BOOL AttemptToAllocatePoints(int32 nPoints);
    void SpendPoints();
private:
    void InitAnalogStickRules();
    void EnableRingUI(const BOOL);
};

void IconSelectionChanged(const ::Console::RingWrapper & tRingWrapper, const UIID & idIcon, const BOOL bSelected);
void AttributeSelectionChanged(const UIID & id, const int32, const int32 nNewVal);
BOOL sShouldIncludeInRing(const UIID & idControl);
void FocusCategoryRing(BOOL bFocus);
void UpdateCharacterInfo(const Player & tPlayer);
void UpdateActionsAndButtonLegend();
void UpdateParagonPointInformation();
void UpdateXPBar();
BOOL CanAffordReset();
void ResetCategoryPoints();
CRefString GetCurrentCategoryName();
inline BOOL IsActive();
inline UIID GetElementUIIDForCategory(int32 nIndex);
inline int32 GetCategoryIndexForElementUIID(const UIID & id);
inline void SetUp3DCharacter(const Player & tPlayer);
inline void OpenCategory();
inline void ConfirmAttributePoints();
inline void AllocatePoints(int32 nPoints);
inline void UpdateAttributeUI(int32 nAttribute);
inline BOOL IsResetCostActive();
inline BOOL HasPointsToReset();

CRefString GetUnspentPointsSourceKey(int32 nCategory);
UIID GetBlinkerUIIDForCategory(int32 nIndex);





ParagonGlobals * sgptParagonGlobals;

























void OpenForController(int32 nControllerOpening)
{

    if (!IsActive() && nControllerOpening != -1)
    {


        Player * ptPlayerOpening = PlayerGetByControllerIndex(nControllerOpening);
        if (ptPlayerOpening)
        {



            int32 nLocalPlayerIndex = PlayerIndexToLocalPlayerIndex(ptPlayerOpening->nPlayerIndex);
            if (nLocalPlayerIndex != -1)
            {



                sgptParagonGlobals->m_ptPlayer = ptPlayerOpening;
                sgptParagonGlobals->InitParagonInformation(*ptPlayerOpening);
                sgptParagonGlobals->m_bAttributeDataDirty[nControllerOpening] = FALSE;


                ::Console::Screens::ShowModalDialog(nControllerOpening, UIWND_PARAGON_CONSOLE, FALSE, GetUIIDNone());


                UI_SetControllerFocus(UIWND_PARAGON_CONSOLE_RING);
                sgptParagonGlobals->InitRingUI();
                FocusCategoryRing(TRUE);


                ::Console::UIRActorManager::SetActive(TRUE);
                UpdateCharacterInfo(*ptPlayerOpening);


                UIInventory::Console::UpdateNavBarTabState(ptPlayerOpening, UIWND_PARAGON_CONSOLE, "LayoutRoot.Navigation", 1);


                ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);


                sgptParagonGlobals->m_eSelectionMode = CATEGORY_SELECTION_MODE;

                UIConsoleListBox & tList = UI_GetConsoleListBoxFromID(UILIST_PARAGON_CONSOLE_ATTRIBUTE_LIST);
                tList.SetDataSource(sgptParagonGlobals->m_tDataSource);
                tList.SetOnSelChangeFunction(AttributeSelectionChanged);
                tList.SetSelectedIndex(-1);
            }
        }
    }
}






void Close()
{

    if (IsActive())
    {
        ::Console::Screens::CloseModalDialog(UIWND_PARAGON_CONSOLE);
        ::Console::UIRActorManager::SetActive(FALSE);
    }
}








void OnPlayerLeveled(const Player * ptPlayer)
{
    sgptParagonGlobals->m_bAttributeDataDirty[ptPlayer->nControllerIndex] = TRUE;
}





void OnUpdate(const UIID & id)
{

    int32 nControllerIndex = sgptParagonGlobals->m_ptPlayer->nControllerIndex;
    if (sgptParagonGlobals->m_bAttributeDataDirty[nControllerIndex])
    {

        sgptParagonGlobals->m_bAttributeDataDirty[nControllerIndex] = FALSE;
        sgptParagonGlobals->InitParagonInformation(*sgptParagonGlobals->m_ptPlayer);
    }

    UpdateActionsAndButtonLegend();
    UpdateParagonPointInformation();
    UpdateXPBar();
    if (sgptParagonGlobals->m_eSelectionMode == ATTRIBUTE_SELECTION_MODE)
    {

        for (int32 nAttribute = 0; nAttribute < 4; nAttribute++)
        {
            UpdateAttributeUI(nAttribute);
        }
    }
}

void OnButton1(const UIID & id, const int32 nControllerIndex)
{


    ::Console::ControllerActionId idAction = ::Console::Screens::GetActionManager().GetActionForInput(::Console::CONTROLLER_INPUT_BUTTON_1);
    if (idAction != -1)
    {
        if (UIUtils::Console::ShowTieredRiftMessage(sgptParagonGlobals->m_ptPlayer))
            return;




        switch (idAction)
        {
        case ACTION_OPEN_CATEGORY:
            OpenCategory();
            break;

        case ACTION_CONFIRM_ATTRIBUTE_POINTS:
            ConfirmAttributePoints();
            break;
        } }
    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
}

void OnButton2(const UIID & id, const int32 nControllerIndex)
{


    ::Console::ControllerActionId idAction = ::Console::Screens::GetActionManager().GetActionForInput(::Console::CONTROLLER_INPUT_BUTTON_2);
    switch (idAction)
    {
    case ACTION_CLOSE:
        ::Console::Screens::CloseScreenSet(TRUE, FALSE);
        break;

    case ACTION_CLOSE_CATEGORY:
        FocusCategoryRing(TRUE);
        sgptParagonGlobals->m_eSelectionMode = CATEGORY_SELECTION_MODE;
        UI_GetConsoleListBoxFromID(UILIST_PARAGON_CONSOLE_ATTRIBUTE_LIST).SetSelectedIndex(-1);
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_FOCUS_BACK), cgtPSODefaults);
        break;
    }
    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
}

static void sDoPurchaseReset(const UIID & id)
{
    ResetCategoryPoints();
}


void OnButton3(const UIID & id, const int32 nControllerIndex)
{


    ::Console::ControllerActionId idAction = ::Console::Screens::GetActionManager().GetActionForInput(::Console::CONTROLLER_INPUT_BUTTON_3);
    if (idAction == ACTION_RESET_CATEGORY_POINTS)
    {

        if (!UIUtils::Console::ShowTieredRiftMessage(sgptParagonGlobals->m_ptPlayer))
        {



            if (!GameIsWeeklyChallenge())
            {



                if (HasPointsToReset() && (!IsResetCostActive() || CanAffordReset()))
                {



                    if (IsResetCostActive())
                    {

                        ScopedGlobalDefinitionLockR pdefGlobal;
                        CRefString sCost = UIText_ValueToGameCurrency(pdefGlobal->nParagonCategoryResetCost, FALSE);
                        Set_Text_Color(sCost, 0);
                        CRefString sMsg = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_STASH), "Warning"), sCost.str(), NULL);
                        UIConfirmation::Show(sMsg.str(), sDoPurchaseReset, NULL, FALSE, TRUE, sgptParagonGlobals->m_ptPlayer->nControllerIndex, -1);
                    }
                    else
                    {
                        ResetCategoryPoints();
                    }
                } } }
    }
    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
}

void OnDPadLeft(const UIID & id, const int32 nControllerIndex)
{
    if (sgptParagonGlobals->m_eSelectionMode == ATTRIBUTE_SELECTION_MODE)
    {
        AllocatePoints(-1);
    }
}



void OnDPadRight(const UIID & id, const int32 nControllerIndex)
{
    if (sgptParagonGlobals->m_eSelectionMode == ATTRIBUTE_SELECTION_MODE)
    {
        AllocatePoints(1);
    }
}



void OnLeftStick(const UIID & id, const int32 nControllerIndex)
{
    const int32 TICKS_BEFORE_FAST_SPEND_ALLOWED = 30;
    const int32 TICKS_BEFORE_RESET = 6;
    switch (sgptParagonGlobals->m_eSelectionMode) {
    case CATEGORY_SELECTION_MODE:
        if (!sgptParagonGlobals->m_tRingStickInputFilter.HandleInput(::Console::ControllerUtils::GetRawLStickPosition(nControllerIndex, 0), 0))
        {

            sgptParagonGlobals->m_tRingUI.UpdateInput(nControllerIndex, IconSelectionChanged);
        }
        break;
    case ATTRIBUTE_SELECTION_MODE:
        {
            ::Console::AnalogStickInputFilter::Rule::ID idRule = sgptParagonGlobals->m_tStickInputFilter.HandleInput(::Console::ControllerUtils::GetRawLStickPosition(nControllerIndex, 0), 0);
            int32 nDirectionToSpend = 0;

            if (idRule == RULE_STICK_STRICT_LEFT)
                nDirectionToSpend = -1;
            else if (idRule == RULE_STICK_STRICT_RIGHT)
                nDirectionToSpend = 1;
            else
            {




                sgptParagonGlobals->m_bFast = FALSE;
                sgptParagonGlobals->m_bClear = TRUE;
                break;
            }

            int32 nCurrentTime = CGameGetRealTicks();




            if (!sgptParagonGlobals->m_bFast && !sgptParagonGlobals->m_bClear)
            {
                if (nCurrentTime - sgptParagonGlobals->m_nTimeSinceLastPointSpend < TICKS_BEFORE_FAST_SPEND_ALLOWED)
                    break;




                sgptParagonGlobals->m_bFast = TRUE;
            }
            else
            {

                if (nCurrentTime - sgptParagonGlobals->m_nTimeSinceLastPointSpend >= TICKS_BEFORE_RESET)
                {
                    sgptParagonGlobals->m_bFast = FALSE;
                    sgptParagonGlobals->m_bClear = TRUE;
                }
            }

            if (nDirectionToSpend != sgptParagonGlobals->m_nLastSpendDirection)
            {
                sgptParagonGlobals->m_bFast = FALSE;
                sgptParagonGlobals->m_bClear = TRUE;
            }

            if (sgptParagonGlobals->m_bClear || sgptParagonGlobals->m_bFast)
            {
                AllocatePoints(nDirectionToSpend);
                sgptParagonGlobals->m_nTimeSinceLastPointSpend = nCurrentTime;
                sgptParagonGlobals->m_nLastSpendDirection = nDirectionToSpend;

                sgptParagonGlobals->m_bClear = FALSE;
            }
        }
        break;
    }
}



inline void OpenCategory()
{

    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SELECT_GENERIC), cgtPSODefaults);
    sgptParagonGlobals->m_eSelectionMode = ATTRIBUTE_SELECTION_MODE;
    FocusCategoryRing(FALSE);
    UI_GetConsoleListBoxFromID(UILIST_PARAGON_CONSOLE_ATTRIBUTE_LIST).SetSelectedIndex(0);
}

inline void ConfirmAttributePoints()
{

    if (sgptParagonGlobals->m_nAttributePointsSpending > 0)
    {



        sgptParagonGlobals->SpendPoints();
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_CONSOLE_PARAGON_ACCEPT_POINTS), cgtPSODefaults);
    }
}
void ResetCategoryPoints()
{

    if (CanAffordReset() && HasPointsToReset())
    {



        IntDataMessage tMessage;
        tMessage.nData = sgptParagonGlobals->m_nCategory;
        MessageSendToServer(CMSG_PARAGON_PURCHASE_RESET_CATEGORY, &tMessage, sizeof(tMessage), sgptParagonGlobals->m_ptPlayer);


        sgptParagonGlobals->m_nAttributePointsSpending = 0;

        for (int32 nAttribute = 0; nAttribute < 4; nAttribute++)
        {
            int32 nCurrentAttributePoints = sgptParagonGlobals->m_nAttributePointsSpent[sgptParagonGlobals->m_nCategory][nAttribute];
            sgptParagonGlobals->m_nCategoryPointsAvailable[sgptParagonGlobals->m_nCategory] += nCurrentAttributePoints;
            sgptParagonGlobals->m_nAttributePointsSpent[sgptParagonGlobals->m_nCategory][nAttribute] = 0;
            UpdateAttributeUI(nAttribute);
        }

        ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_CONSOLE_PARAGON_RESET_POINTS), cgtPSODefaults);
    }
}
void UpdateParagonPointInformation()
{


    CRefString sParagonLevel = StringListFetch("X1_ConsoleParagon:ParagonLevelFormat");
    sParagonLevel = StringListEntryReplace(sParagonLevel, "{s1}", StringListFetch("X1_ConsoleParagon:ParagonLevel").str(), 999);
    sParagonLevel = StringListEntryReplace(sParagonLevel, "{s2}", UnicodeText::FormatNumber(sgptParagonGlobals->m_ptPlayer->nCurrentAltLevel, TRUE, FNS_NORMAL, XLOCALE_INVALID).str(), 999);
    UI_GetTextFromID(UITXT_PARAGON_CONSOLE_PARAGON_LEVEL).SetText(sParagonLevel, FALSE);


    UIID idUnspentPoints;
    CRefString sUnspentText;
    for (int32 nCategory = 0; nCategory < 4; ++nCategory)
    {

        int32 nAvailablePoints = sgptParagonGlobals->m_nCategoryPointsAvailable[nCategory];
        if (sgptParagonGlobals->m_nCategory == nCategory)
        {
            nAvailablePoints -= sgptParagonGlobals->m_nAttributePointsSpending;
        }
        idUnspentPoints.SetFromParent(UIWND_PARAGON_CONSOLE_ROOT, GetUnspentPointsSourceKey(nCategory).str());
        UIText & tText = UI_GetTextFromID(idUnspentPoints);
        tText.SetText(UnicodeText::FormatNumber(nAvailablePoints, TRUE, FNS_NORMAL, XLOCALE_INVALID).str(), FALSE);






        tText.SetTextColor(nAvailablePoints != 0 || sgptParagonGlobals->m_nCategory == nCategory ? crgbaWhite : crgbaGray);
    }



    CRefString sUnspentPointsAndResetCost;
    int32 nAvailablePoints = sgptParagonGlobals->m_nCategoryPointsAvailable[sgptParagonGlobals->m_nCategory] - sgptParagonGlobals->m_nAttributePointsSpending;

    CRefString sUnspentPointsInCategoryFormat = StringListFetch("X1_ConsoleParagon:UnspentCategoryPoints");
    sUnspentPointsAndResetCost = LocalizedSprintf(sUnspentPointsInCategoryFormat.str(), UnicodeText::FormatNumber(nAvailablePoints, TRUE, FNS_NORMAL, XLOCALE_INVALID).str(), GetCurrentCategoryName().str(), NULL);






    if (IsResetCostActive())
    {
        CRefString sResetCost = StringListFetch("X1_ConsoleParagon:ResetCost");
        ScopedGlobalDefinitionLockR pdefGlobal;
        int32 nResetCost = pdefGlobal->nParagonCategoryResetCost;
        sResetCost = StringListEntryReplace(sResetCost, "{s1}", UIText_ValueToGameCurrency(nResetCost, FALSE), 1);
        sResetCost = StringListEntryReplace(sResetCost, "{s2}", GetCurrentCategoryName().str(), 1);
        sUnspentPointsAndResetCost.Format("%s\n%s", sUnspentPointsAndResetCost.str(), sResetCost.str());
    }
    UI_GetTextFromID(UITXT_PARAGON_CONSOLE_UNSPENT_CURRENT_CATEGORY).SetText(sUnspentPointsAndResetCost, FALSE);
}

CRefString GetCurrentCategoryName()
{
    CRefString sCategoryName;
    switch (sgptParagonGlobals->m_nCategory)
    {
    case 0:
        sCategoryName.Format(StringListFetch("SkillsUI:ParagonCategory_Primary_Attributes").str());
        break;
    case 1:
        sCategoryName.Format(StringListFetch("SkillsUI:ParagonCategory_Attack").str());
        break;
    case 2:
        sCategoryName.Format(StringListFetch("SkillsUI:ParagonCategory_Defense").str());
        break;
    case 3:
        sCategoryName.Format(StringListFetch("SkillsUI:ParagonCategory_Utility").str());
        break;
    }
    return sCategoryName;
}


CRefString GetUnspentPointsSourceKey(int32 nCategory)
{

    CRefString sKey;
    switch (nCategory)
    {
    case 0: sKey.Format("UnspentCore"); break;
    case 1: sKey.Format("UnspentOffense"); break;
    case 2: sKey.Format("UnspentDefense"); break;
    case 3: sKey.Format("UnspentUtility"); break;
    }
    return sKey;
}









void UpdateItemAttributeFixedDetails(const UIID & idItemUI, const int32 nAttribute)
{
    if (nAttribute == -1 || sgptParagonGlobals->m_nCategory == -1) return;







    UIID idAttributeImage = UIIDCreateFromParent(idItemUI, ".AttributeImage");
    UIID idAttributeName = UIIDCreateFromParent(idItemUI, ".AttributeName");
    UIID idBonusPerPoint = UIIDCreateFromParent(idItemUI, ".BonusPerPoint");

    GBID gbidParagonBonus = sgptParagonGlobals->m_gbidAttributes[sgptParagonGlobals->m_nCategory][nAttribute];
    ScopedParagonBonusLockR pdefParagonBonus(gbidParagonBonus);


    UIWindow & tAttributeImage = UI_GetWindowFromID(idAttributeImage);
    ImageTextureFrame tFrame;
    ImageCollectionGetImageInfo(IMAGE_COLLECTION_UI, pdefParagonBonus->szIcon, tFrame);
    tAttributeImage.SetTextureFrame(tFrame);


    CRefString sAttribute = CRefString().Format("ParagonBonusLabel_%s", (char *)pdefParagonBonus->tHeader.szName);
    CRefString sAttributeName = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_SKILLS_UI), sAttribute.str());
    UI_GetTextFromID(idAttributeName).SetText(sAttributeName, FALSE);
    UI_GetTextFromID(idBonusPerPoint).SetText(GetStatContribution(gbidParagonBonus), FALSE);




    UIID idBGCore = UIIDCreateFromParent(idItemUI, ".BaseStatBGCore");
    UIID idBGOffense = UIIDCreateFromParent(idItemUI, ".BaseStatBGOffense");
    UIID idBGDefense = UIIDCreateFromParent(idItemUI, ".BaseStatBGDefense");
    UIID idBGUtility = UIIDCreateFromParent(idItemUI, ".BaseStatBGUtility");
    UI_GetWindowFromID(idBGCore).SetActive(sgptParagonGlobals->m_nCategory == 0);
    UI_GetWindowFromID(idBGOffense).SetActive(sgptParagonGlobals->m_nCategory == 1);
    UI_GetWindowFromID(idBGDefense).SetActive(sgptParagonGlobals->m_nCategory == 2);
    UI_GetWindowFromID(idBGUtility).SetActive(sgptParagonGlobals->m_nCategory == 3);

}

void UpdateItemAttributeDynamicDetails(const UIID & idItemUI, const int32 nAttribute, const BOOL bSelected)
{
    if (nAttribute == -1 || sgptParagonGlobals->m_nCategory == -1) return;




    if (idItemUI == GetUIIDNone()) return;




    UIID idPoints = UIIDCreateFromParent(idItemUI, ".Points");
    UIID idTotalBonus = UIIDCreateFromParent(idItemUI, ".TotalBonus");
    UIID idHighlight = UIIDCreateFromParent(idItemUI, ".Highlight");
    UIID idPlus = UIIDCreateFromParent(idItemUI, ".PointsPlus");
    UIID idProgressSpent = UIIDCreateFromParent(idItemUI, ".ProgressPointsSpent");
    UIID idProgressSpending = UIIDCreateFromParent(idItemUI, ".ProgressPointsSpending");

    GBID gbidParagonBonus = sgptParagonGlobals->m_gbidAttributes[sgptParagonGlobals->m_nCategory][nAttribute];
    ScopedParagonBonusLockR pdefParagonBonus(gbidParagonBonus);
    const int32 UNCAPPED_POINTS_MAX = 100000;
    int32 nPointsMax = pdefParagonBonus->nPointsMax;
    BOOL bUncapped = nPointsMax >= UNCAPPED_POINTS_MAX;

    UI_GetWindowFromID(idHighlight).SetActive(bSelected);


    int32 nPointsSpent = sgptParagonGlobals->m_nAttributePointsSpent[sgptParagonGlobals->m_nCategory][nAttribute];
    int32 nPointsTotal = nPointsSpent;
    if (bSelected)
    {
        nPointsTotal += sgptParagonGlobals->m_nAttributePointsSpending;
    }


    BOOL bShowPlus = bUncapped && nPointsTotal >= ATTRIBUTE_TICKS && bSelected && sgptParagonGlobals->m_nAttributePointsSpending != 0;
    UI_GetWindowFromID(idPlus).SetActive(bShowPlus);


    CRefString sPoints;
    if (bSelected)
    {
        sPoints = StringListFetch("X1_ConsoleParagon:PointsSelectedFormat");
    }
    else
    {
        sPoints = StringListFetch("X1_ConsoleParagon:PointsUnselectedFormat");
    }
    sPoints = StringListEntryReplace(sPoints, "{s1}", UnicodeText::FormatNumber(nPointsTotal, TRUE, FNS_NORMAL, XLOCALE_INVALID).str(), 1);
    if (bUncapped)
    {
        sPoints = StringListEntryReplace(sPoints, "{s2}", "", 1);
    }
    else
    {
        sPoints = StringListEntryReplace(sPoints, "{s2}", CRefString().Format("/%d", nPointsMax), 1);
    }
    UI_GetTextFromID(idPoints).SetText(sPoints, FALSE);


    // The original sign-extends both integer callback payloads to pointer width before UIToolTips_ProcessTip.
    CRefString sTip = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_SKILLS_UI), CRefString().Format("ParagonBonusValue_%s", (char *)pdefParagonBonus->tHeader.szName));
    CRefString sResult = UIToolTips_ProcessTip(NULL, -1, -1, SNO_NONE, 0, sTip, TTVC_NO_COLORING, ResolveValueTagForParagonBonusTooltip, (void *)(intptr)gbidParagonBonus, (void *)(intptr)nPointsTotal);
    CRefString sTotalBonus;
    if (bSelected)
    {
        sTotalBonus = StringListFetch("X1_ConsoleParagon:TotalBonusSelectedFormat");
    }
    else
    {
        sTotalBonus = StringListFetch("X1_ConsoleParagon:TotalBonusUnselectedFormat");
    }
    sTotalBonus = StringListEntryReplace(sTotalBonus, "{s1}", sResult.str(), 1);
    sTotalBonus = StringListEntryReplace(sTotalBonus, "{s2}", StringListFetch("X1_ConsoleParagon:Total").str(), 1);
    UI_GetTextFromID(idTotalBonus).SetText(sTotalBonus, FALSE);



    float flProgressSpending = Math::Min(nPointsTotal, ATTRIBUTE_TICKS);
    float flProgressSpent = Math::Min(nPointsSpent, ATTRIBUTE_TICKS);

    UIProgressBar & tProgressSpent = UI_GetProgressBarFromID(idProgressSpent);
    tProgressSpent.SetExtentsAndPosition(0.0f, float(ATTRIBUTE_TICKS), flProgressSpent, -1.0f);
    UIProgressBar & tProgressSpending = UI_GetProgressBarFromID(idProgressSpending);
    tProgressSpending.SetExtentsAndPosition(0.0f, float(ATTRIBUTE_TICKS), flProgressSpending, -1.0f);


    UIID idCoreNew = GetBlinkerUIIDForCategory(0);
    UIID idOffenseNew = GetBlinkerUIIDForCategory(1);
    UIID idDefenseNew = GetBlinkerUIIDForCategory(2);
    UIID idUtilityNew = GetBlinkerUIIDForCategory(3);
    UI_GetWindowFromID(idCoreNew).SetActive(sgptParagonGlobals->m_nCategoryPointsAvailable[0] != 0);
    UI_GetWindowFromID(idOffenseNew).SetActive(sgptParagonGlobals->m_nCategoryPointsAvailable[1] != 0);
    UI_GetWindowFromID(idDefenseNew).SetActive(sgptParagonGlobals->m_nCategoryPointsAvailable[2] != 0);
    UI_GetWindowFromID(idUtilityNew).SetActive(sgptParagonGlobals->m_nCategoryPointsAvailable[3] != 0);
}


void FocusCategoryRing(BOOL bFocus)
{


    sgptParagonGlobals->m_tRingUI.SetEnabled(bFocus);


    const UIControlItemArray & arChildren = UI_GetWindowFromID(UIWND_PARAGON_CONSOLE_RING).GetChildList();
    for (uint32 i = 0; i < arChildren.size(); i++)
    {
        if (arChildren[i].ptControl->IsType(UIT_BUTTON) &&
            sgptParagonGlobals->m_tRingUI.GetSelectedControlID() != arChildren[i].id)
        {
            UIButton * ptButton = (UIButton *)arChildren[i].ptControl;
            ptButton->SetButtonState(BS_NORMAL, FALSE, FALSE);
            if (bFocus)
            {
                ptButton->SetEnabled(TRUE);
                ptButton->SetShader(-1);
            }
            else
            {
                ptButton->SetEnabled(FALSE);
                ptButton->SetShader(GlobalSNOGet(GLOBALSNO_SHADER_SPRITE_TRANSPARENT_INACTIVE));
            }
        }
    }

    if (bFocus)
    {
        UI_SetControllerFocus(UIWND_PARAGON_CONSOLE_RING);
    }
    else
    {
        UI_SetControllerFocus(UILIST_PARAGON_CONSOLE_ATTRIBUTE_LIST);
    }

    UI_GetWindowFromID(UIWND_PARAGON_CONSOLE_MENU_GLOW).SetActive(!bFocus);
    UI_GetWindowFromID(UIWND_PARAGON_CONSOLE_RING_CURSOR).SetActive(bFocus);
    UI_GetWindowFromID(UIWND_PARAGON_CONSOLE_RING_GLOW).SetActive(bFocus);
}


inline void AllocatePoints(int32 nPoints)
{


    if (sgptParagonGlobals->AttemptToAllocatePoints(nPoints))
    {

        UpdateAttributeUI(sgptParagonGlobals->m_nAttribute);
    }
}



inline void UpdateAttributeUI(int32 nAttribute)
{


    UIConsoleListBox & tList = UI_GetConsoleListBoxFromID(UILIST_PARAGON_CONSOLE_ATTRIBUTE_LIST);
    const UIID & idCurrentItem = tList.GetItemUIID(nAttribute);
    UpdateItemAttributeDynamicDetails(idCurrentItem, nAttribute, tList.GetSelectedIndex() == nAttribute);
}



BOOL HasLeveledSinceLastVisit(const Player * ptPlayer)
{
    return sgptParagonGlobals->m_bAttributeDataDirty[ptPlayer->nControllerIndex];
}





inline BOOL IsActive()
{


    return UI_GetWindowFromID(UIWND_PARAGON_CONSOLE).IsActive();
}



inline UIID GetElementUIIDForCategory(int32 nIndex)
{

    switch (nIndex)
    {
    case 0:
        return UIIDCreateFromParent(UIWND_PARAGON_CONSOLE_RING, ".Core");
    case 1:
        return UIIDCreateFromParent(UIWND_PARAGON_CONSOLE_RING, ".Offense");
    case 2:
        return UIIDCreateFromParent(UIWND_PARAGON_CONSOLE_RING, ".Defense");
    case 3:
        return UIIDCreateFromParent(UIWND_PARAGON_CONSOLE_RING, ".Utility");
    }
    return GetUIIDNone();
}




inline int32 GetCategoryIndexForElementUIID(const UIID & id)
{

    for (int32 i = 0; i < 4; i++)
    {
        if (id == GetElementUIIDForCategory(i))
            return i;
    }
    return -1;
}







UIID GetBlinkerUIIDForCategory(int32 nIndex)
{
    switch (nIndex)
    {
    case 0: return UIIDCreateFromParent(UIWND_PARAGON_CONSOLE_RING, ".CoreNew");
    case 1: return UIIDCreateFromParent(UIWND_PARAGON_CONSOLE_RING, ".OffenseNew");
    case 2: return UIIDCreateFromParent(UIWND_PARAGON_CONSOLE_RING, ".DefenseNew");
    case 3: return UIIDCreateFromParent(UIWND_PARAGON_CONSOLE_RING, ".UtilityNew");
    }
    return GetUIIDNone();



    // Inference: default none follows the original inlined UIID copy at1052 and the sibling GetElementUIIDForCategory.
    return GetUIIDNone();
}




void IconSelectionChanged(const ::Console::RingWrapper & tRingWrapper, const UIID & idIcon, const BOOL bSelected)
{



    if (idIcon == GetUIIDNone())
        return;



    UI_GetButtonFromID(idIcon).SetButtonState(bSelected ? BS_PUSHED : BS_NORMAL, FALSE, FALSE);

    int32 nSelectedCategory = GetCategoryIndexForElementUIID(idIcon);
    if (nSelectedCategory != -1)
    {
        sgptParagonGlobals->m_nCategory = nSelectedCategory;
        UI_GetTextFromID(UITXT_PARAGON_CONSOLE_INFO_TITLE).SetText(GetCurrentCategoryName(), FALSE);
        UI_GetButtonFromID(idIcon).SetButtonState(bSelected ? BS_MOUSEOVER : BS_NORMAL, FALSE, FALSE);

        UI_GetConsoleListBoxFromID(UILIST_PARAGON_CONSOLE_ATTRIBUTE_LIST).ResetAllItems();
        UI_GetConsoleListBoxFromID(UILIST_PARAGON_CONSOLE_ATTRIBUTE_LIST).SetSelectedIndex(-1);
    }
}





void AttributeSelectionChanged(const UIID & id, const int32, const int32 nNewVal)
{
    sgptParagonGlobals->m_nAttribute = nNewVal;
    sgptParagonGlobals->m_nAttributePointsSpending = 0;
}







void UpdateActionsAndButtonLegend()
{

    ::Console::ControllerActionManager & tActionManager = ::Console::Screens::GetActionManager();
    if (!tActionManager.AreActionsDirty()) return;




    tActionManager.SetActionsDirtyFlag(FALSE);
    tActionManager.ClearAllActions();

    SelectionMode eSelectionMode = sgptParagonGlobals->m_eSelectionMode;
    switch (eSelectionMode)
    {
    case CATEGORY_SELECTION_MODE:
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_OPEN_CATEGORY, StringListFetch("X1_ConsoleParagon:OpenCategory").str());
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_CLOSE, StringListFetch("ConsoleUI:Close").str());
        break;
    case ATTRIBUTE_SELECTION_MODE:
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_CONFIRM_ATTRIBUTE_POINTS, StringListFetch("H2OLayout:AssignButton").str());
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_CLOSE_CATEGORY, StringListFetch("ConsoleUI:Back").str());
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_LEFT_STICK, ACTION_SELECT_ATTRIBUTE, StringListFetch("H2OLayout:Select").str());
        if (!GameIsWeeklyChallenge())
        {
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_DPAD_HORIZONTAL, ACTION_ADD_OR_REMOVE_POINTS, StringListFetch("X1_ConsoleParagon:AddOrRemovePoints").str());
        }
        if (!GameIsWeeklyChallenge() && HasPointsToReset() && (!IsResetCostActive() || CanAffordReset()))
        {
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_3, ACTION_RESET_CATEGORY_POINTS, StringListFetch("H2OLayout:Reset").str());
        }
        break;
    }
    ::Console::Screens::UpdateButtonLegendText();
}






void UpdateCharacterInfo(const Player & tPlayer)
{

    const BOOL bShouldCreateRActor = ::Console::UIRActorManager::ShouldCreateRActor(::Console::UIRActorManager::RACTOR_CONTROL_RING, ::Console::UIRActorManager::RACTOR_CHARACTER_PLAYER, FALSE, TRUE);

    if (bShouldCreateRActor)
    {

        VisualInventory * ptVisualInventory = VisualInventoryGet(tPlayer.idPlayerACD);
        UIInventory::Console::UpdatePlayerActorUI(UIRACTOR_CONSOLE_RING, *ptVisualInventory, tPlayer, 0x11000);
        SetUp3DCharacter(tPlayer);
    }


    CRefString sNumber;
    if (PlayerGetLocalPlayerCount() > 1 && PlayerIsLocal(&tPlayer))
    {
        sNumber = CPlayerGetIndicatorString(tPlayer);
    }

    UI_GetTextFromID(UITXT_CONSOLE_PLAYER_LABEL_RING).SetText(sNumber.str(), FALSE);
}



inline void SetUp3DCharacter(const Player & tPlayer)
{


    UIRActor & tHero = UI_GetRActorFromID(UIRACTOR_CONSOLE_RING);
    LightID idLight = LightAdd(0, UI_GetDefaultHeroLight(), PRTransform(cgqIdentity, cgwpNull), FALSE,
                               tHero.tRenderData[CGameAsyncGetRenderDataIndex(0)].m_tDisplayList.GetRenderContext());


    tHero.SetLight(idLight);
    tHero.SetObserver(UIUtils::Console::GetObserverForPlayerClass(PlayerGetPlayerClass(&tPlayer)));
}






void UpdateXPBar()
{

    ActorCommonData * ptACDPlayer = ACDTryToGet(sgptParagonGlobals->m_ptPlayer->idPlayerACD);
    if (ptACDPlayer)
    {
        uint64 nExpDiff = 0;
        uint64 nExp = 0;


        int32 nLevel = ptACDPlayer->AttributesGetInt(ATTRIBUTE_ALT_LEVEL);
        if (nLevel >= 0 && nLevel <= HERO_ALT_LEVEL_MAX)
        {
            ScopedGameBalanceDefinitionLockR ptGameBalDef(GlobalSNOGet(GLOBALSNO_GB_EXPERIENCE_ALT));
            const ExperienceTableAlt & tExpTableAlt = ptGameBalDef->tExperienceTableAlt;

            nExp = tExpTableAlt.ptExperienceLevelsAlt[Math::Min(nLevel, (int32)HERO_ALT_LEVEL_REPEAT_INDEX)].nDeltaXP;

            nExpDiff = Math::Min(ptACDPlayer->GetAltExperienceNext(), nExp);
        }


        UI_GetProgressBarFromID(UPROGRESS_PARAGON_CONSOLE_PARAGON_EXP).SetExtentsAndPosition(0.0f, (float)nExp, (float)(nExp - nExpDiff), -1.0f);
    }
}



inline BOOL IsResetCostActive()
{


    ScopedGlobalDefinitionLockR pdefGlobal;
    int32 nUpgradeCost = pdefGlobal->nParagonCategoryResetCost;
    return nUpgradeCost != 0;
}



BOOL CanAffordReset()
{

    ActorCommonData * ptACDPlayer = ACDTryToGet(sgptParagonGlobals->m_ptPlayer->idPlayerACD);
    if (ptACDPlayer)
    {





        ScopedGlobalDefinitionLockR pdefGlobals;
        int32 nUpgradeCost = pdefGlobals->nParagonCategoryResetCost;
        return ptACDPlayer->CanAffordCurrencyCost(nUpgradeCost, 0);
    }
    return FALSE;
}
inline BOOL HasPointsToReset()
{

    for (int32 nAttribute = 0; nAttribute < 4; nAttribute++)
    {
        if (sgptParagonGlobals->m_nAttributePointsSpent[sgptParagonGlobals->m_nCategory][nAttribute] > 0)
            return TRUE;
    }
    return FALSE;
}






void InitializeForGame()
{

    sgptParagonGlobals = new ParagonGlobals;
}




void DisposeForGame()
{


    delete sgptParagonGlobals;
    sgptParagonGlobals = NULL;
}

void InitializeForPlayer(const int32 nPlayerIndex)
{
    int32 nIndex = PlayerIndexToLocalPlayerIndex(nPlayerIndex);
    if (nIndex != -1)
    {
        sgptParagonGlobals->m_bAttributeDataDirty[nIndex] = FALSE;
    }
}










BOOL ParagonGlobals::AttemptToAllocatePoints(int32 nPoints)
{

    if (GameIsWeeklyChallenge())
        return FALSE;



    m_nAttributePointsSpending += nPoints;


    GBID gbidParagonBonus = m_gbidAttributes[m_nCategory][m_nAttribute];
    ScopedParagonBonusLockR pdefParagonBonus(gbidParagonBonus);
    int32 nPointsMax = pdefParagonBonus->nPointsMax;
    BOOL bOverCap = nPointsMax < m_nAttributePointsSpending + m_nAttributePointsSpent[m_nCategory][m_nAttribute];




    if (m_nAttributePointsSpending < 0)
    {

        m_nAttributePointsSpending -= nPoints;
        return FALSE;
    }
    else if (m_nAttributePointsSpending > m_nCategoryPointsAvailable[m_nCategory])
    {

        m_nAttributePointsSpending -= nPoints;
        UIGame::DisplayError(GameErrorData(GAMEERROR_INSUFFICIENT_PARAGON_POINTS), FALSE, sgptParagonGlobals->m_ptPlayer);
        return FALSE;
    }
    else if (bOverCap)
    {

        m_nAttributePointsSpending -= nPoints;
        UIGame::DisplayError(GameErrorData(GAMERROR_PARAGON_CATEGORY_AT_CAP), FALSE, sgptParagonGlobals->m_ptPlayer);
        return FALSE;
    }

    if (nPoints > 0)
    {
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_CONSOLE_PARAGON_ASSIGN_POINT), cgtPSODefaults);
    }
    else
    {

        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_CONSOLE_PARAGON_REMOVE_POINT), cgtPSODefaults);
    }

    return TRUE;
}



inline void ParagonGlobals::SpendPoints()
{


    SpendParagonPointsMessage tMessage;
    tMessage.gbidParagonBonus = m_gbidAttributes[m_nCategory][m_nAttribute];
    tMessage.nPoints = m_nAttributePointsSpending;
    MessageSendToServer(CMSG_PARAGON_POINT_SPEND, &tMessage, sizeof(tMessage), m_ptPlayer);


    m_nAttributePointsSpent[m_nCategory][m_nAttribute] += m_nAttributePointsSpending;
    m_nCategoryPointsAvailable[m_nCategory] -= m_nAttributePointsSpending;
    m_nAttributePointsSpending = 0;
}



BOOL sShouldIncludeInRing(const UIID & idControl)
{
    return UI_GetControlFromID(idControl).IsType(UIT_BUTTON);
}



inline void ParagonGlobals::InitRingUI()
{


    m_tRingUI.WrapUsingContainerControl(UIWND_PARAGON_CONSOLE_RING, UIWND_PARAGON_CONSOLE_RING_CURSOR, sShouldIncludeInRing, FALSE);




    m_tRingUI.SelectControl(GetElementUIIDForCategory(0), IconSelectionChanged);



    UI_GetWindowFromID(UIWND_PARAGON_CONSOLE_RING_GLOW).SetActive(TRUE);
    UI_GetWindowFromID(UIWND_PARAGON_CONSOLE_RING_CURSOR).SetActive(TRUE);
}



void ParagonGlobals::InitAnalogStickRules()
{
    const Blizzard::Time::Microsecond DEFAULT_STICK_DELAY = 0;

    m_tRingStickInputFilter.AddRule(::Console::AnalogStickInputFilter::Rule::CatchAllRule);

    m_tStickInputFilter.SetMinimumInputDelay(DEFAULT_STICK_DELAY);

    ::Console::AnalogStickInputFilter::Rule tStickStrictLeftRule;
    tStickStrictLeftRule.id = RULE_STICK_STRICT_LEFT;
    tStickStrictLeftRule.vecMinStickPos.Set(-FLT_MAX, -STICK_SENSITIVITY);
    tStickStrictLeftRule.vecMaxStickPos.Set(-STICK_SENSITIVITY, STICK_SENSITIVITY);
    m_tStickInputFilter.AddRule(tStickStrictLeftRule);

    ::Console::AnalogStickInputFilter::Rule tStickStrictRightRule;
    tStickStrictRightRule.id = RULE_STICK_STRICT_RIGHT;
    tStickStrictRightRule.vecMinStickPos.Set(STICK_SENSITIVITY, -STICK_SENSITIVITY);
    tStickStrictRightRule.vecMaxStickPos.Set(FLT_MAX, STICK_SENSITIVITY);
    m_tStickInputFilter.AddRule(tStickStrictRightRule);

    UIConsoleListBox & tList = UI_GetConsoleListBoxFromID(UILIST_PARAGON_CONSOLE_ATTRIBUTE_LIST);
    tList.SetAnalogStickInputFilter(::Console::GetDefaultStickInputFilter(::Console::FILTER_STRICT_VERTICAL));
}




void ParagonGlobals::InitParagonInformation(const Player & tPlayer)
{

    ActorCommonData * ptACDPlayer = ACDTryToGet(tPlayer.idPlayerACD);
    if (ptACDPlayer)
    {


        PlayerClass ePlayerClass = PlayerGetPlayerClass(ptACDPlayer->id);

        GBHandleList listBonuses(GBGetHandlePool());
        GBEnumerate(GB_PARAGON_BONUSES, listBonuses);

        POS pos = listBonuses.GetHeadPosition();
        while (pos)
        {
            const GBHandle & hGBBonus = listBonuses.GetNext(pos);
            ScopedParagonBonusLockR pdefParagonBonus(hGBBonus.gbid);

            if (pdefParagonBonus->ePlayerClass == -1 || pdefParagonBonus->ePlayerClass == ePlayerClass)
            {

                int32 nCategory = pdefParagonBonus->nCategory;
                int32 nAttribute = pdefParagonBonus->nIndex - 1;


                sgptParagonGlobals->m_gbidAttributes[nCategory][nAttribute] = hGBBonus.gbid;


                int32 nPointsSpent = ptACDPlayer->AttributesGetInt(MakeAttribKey(ATTRIBUTE_PARAGON_BONUS, hGBBonus.gbid));
                sgptParagonGlobals->m_nAttributePointsSpent[nCategory][nAttribute] = nPointsSpent;
            }
        }


        for (int32 nCategory = 0; nCategory < 4; nCategory++)
        {

            int32 nPointsAvailable = ptACDPlayer->AttributesGetInt(MakeAttribKey(ATTRIBUTE_PARAGON_BONUS_POINTS_AVAILABLE, nCategory));
            sgptParagonGlobals->m_nCategoryPointsAvailable[nCategory] = nPointsAvailable;
        }
    }
}

}  // namespace Console
}  // namespace UIParagonBonuses
