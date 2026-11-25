// Original file: X/Client/UserInterface/Console/UIConsoleBannerCustomization.cpp
// Console banner customization screen. Generated from build/try/p2-uib/bc.cpp by gen.py (pads functions onto their
// original lines). Local declarations stand in for headers not adopted yet; each names its defining file.

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Common/SNO/SNOName.h"
#include "../../../Common/WorldPlace.h"
#include "../../../Sigma/Math/Vector.h"
#include "../../../Sigma/RGBAColor.h"
#include "../../Gfx/GfxVB.h"
#include "../../../../NintendoSDK/Include/nn/ldn/ldn_Ipv4Address.h"
#include "../../../../NintendoSDK/Include/nn/ldn/ldn_MacAddress.h"
#include "../../../../Contrib/Contrib/blz/include/blz/vector.h"
#include "../../CGame.h"
#include "../../Console/TouchGestureTracker.h"
#include "../../Console/GamerProfile.h"
#include "../../Console/ControllerUtils.h"
#include "ConsoleInputUtilities.h"
#include "ConsoleScreenManager.h"
#include "ControllerActions.h"
#include "Wrappers/RingWrapper.h"
#include "../Controls/UIConsoleListboxControl.h"
#include "../Controls/UIConsoleScrollBoxControl.h"
#include "../Controls/UIControlControl.h"
#include "../Controls/UIButtonControl.h"
#include "../Controls/UITextControl.h"
#include "../Controls/UIWindowControl.h"
#include "../UI.h"
#include "../UIID.h"
#include "../../../Common/OnlineService/OnlineService.h"
#include "../../../Common/OnlineService/Achievements.h"
#include "../../../Common/Achievements/Console/ConsoleAchievements.h"
#include "../../Battlenet/UI/Util/UIAchievementsUtil.h"
#include "../../../Common/PlayerAccount.h"
#include "../../../Common/Game.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/Definitions/GlobalDefinition.h"
#include "../../../Common/Definitions/StringListDefinition.h"
#include "../../../Common/UserInterface/UITextControl.h"
#include "../../../Common/Protobuf/Account.pb.h"
#include "../../ImageCollection.h"
#include "../../Sound/Sound.h"
#include "../../../Sigma/RefString.h"
#include "../Controls/UIRActorControl.h"
#include "../../RActors/RActors.h"
#include "../../../Common/Player.h"
#include "../../../Sigma/Math/Transform.h"
#include "../../Console/Online.h"
#include "../../Lights.h"
#include "../Controls/UIConsoleScrollBoxControl.h"
#include "../../../Sigma/UnicodeText.h"

// X/Client/UserInterface/UIIDs.h
extern UIID UIWND_CONSOLE_BANNER_CUSTOMIZATION;
extern UIID UIWND_CONSOLE_BANNER_CUSTOMIZATION_RING;
extern UIID UIWND_CONSOLE_BANNER_CUSTOMIZATION_CURSOR;
extern UIID UIWND_CONSOLE_BANNER_CUSTOMIZATION_DIMMER;
extern UIID UIWND_CONSOLE_BANNER_CUSTOMIZATION_RING_GLOW;
extern UIID UIWND_CONSOLE_BANNER_CUSTOMIZATION_PANEL_FOCUS_HIGHLIGHT;
extern UIID UILIST_CONSOLE_BANNER_CUSTOMIZATION_CHOICES;
extern UIID UITXT_CONSOLE_BANNER_CUSTOMIZATION_HEADER;
extern UIID UISCROLL_CONSOLE_BANNER_CUSTOMIZATION_DETAILS_ACHV_INFO;
extern UIID UIRACTOR_CONSOLE_BANNER_CUSTOMIZATION_BANNER;
extern UIID UIWND_CONSOLE_BANNER_CUSTOMIZATION_DETAILS_ICON, UITXT_CONSOLE_BANNER_CUSTOMIZATION_DETAILS_NAME, UIWND_CONSOLE_BANNER_CUSTOMIZATION_DETAILS_ACHV_PORTRAIT, UITXT_CONSOLE_BANNER_CUSTOMIZATION_DETAILS_INFO, UIWND_CONSOLE_BANNER_CUSTOMIZATION_DETAILS_ACHV_EARNED, UIWND_CONSOLE_BANNER_CUSTOMIZATION_DETAILS_ACHV_UNEARNED, UIWND_CONSOLE_BANNER_CUSTOMIZATION_DETAILS_FOS, UITXT_CONSOLE_BANNER_CUSTOMIZATION_DETAILS_STATUS;
// X/Client/UserInterface/UI.cpp
UIRActor & UI_GetRActorFromID(const UIID & id);
// X/Client/CPlayer.cpp
void CPlayerUpdateBanner(const PlayerAccountBanner & tBanner, BannerRenderData & tBannerRenderData, int32 nFlags);
void CPlayerUpdateBannerAttachments(RActor & tRActor, const BannerRenderData & tBannerRenderData);
const UIID & UI_GetControllerFocus();
UIConsoleScrollBox & UI_GetConsoleScrollBoxFromID(const UIID & id);
// X/Client/Console/GamerProfile.cpp
void LightDelete(LightID idLight); SNO UI_GetDefaultHeroLight(); LightID LightAdd(uint32 dwFlags, SNO snoLight, const PRTransform & tTransform, BOOL bUnused, RenderContext eContext);
void MessageSendToServer(GameMessageType eType, const google::protobuf::MessageLite & tMessage, const Player * ptPlayer); enum { CMSG_BANNER_CONFIGURATION = 448 }; namespace UIMainMenu { namespace Console { void Refresh3DBanner(); } }
namespace Console { namespace GamerProfile { BOOL IsRewardNew(int32 nUserIdx, OnlineService::RewardId idReward); void OnRewardViewed(int32 nUserIdx, OnlineService::RewardId idReward); } }

namespace UIBannerCustomization {
namespace Console {

enum {
    BANNERDATA_NONE = -1,
    BANNERDATA_SHAPE = 0,
    BANNERDATA_PATTERN = 1,
    BANNERDATA_SIGIL_MAIN = 2,
    BANNERDATA_SIGIL_ACCENT = 3,
    BANNERDATA_BACKGROUND_COLOR = 4,
    BANNERDATA_PATTERN_COLOR = 5,
    BANNERDATA_SIGIL_COLOR = 6,
    BANNERDATA_SIGIL_PLACEMENT = 7,
    NUM_BASIC_BANNERDATA_TYPES = 8,
};
typedef int32 BannerDataType;
BOOL IsVisible();
void OnAccept(const UIID & id, int32 nControllerIndex);

}  // namespace Console

namespace {
void RevertToSavedBanner();
void RefreshRingDisplay();
void IconSelectionChanged(const ::Console::RingWrapper &, const UIID & idIcon, const BOOL bSelected);
void FocusCategoryRing(BOOL bFocus);
void UpdateBannerHeader();
void UpdateRenderedBanner();
int32 GetNumChoicesForBannerData(Console::BannerDataType eBannerDataType);
int32 GetSavedBannerDataSelection(Console::BannerDataType eBannerDataType);
BOOL GetSavedSigilUsesVariant();
BOOL GetRenderSigilUsesVariant();
ImageTextureFrame GetIconTexture(Console::BannerDataType eBannerDataType, int32 nDataIndex);
OnlineService::RewardId GetRewardIdForBannerData(Console::BannerDataType eBannerDataType, int32 nDataIndex);
BOOL IsBannerPieceLocked(Console::BannerDataType eBannerDataType, int32 nDataIndex);
BOOL IsBannerPieceNew(Console::BannerDataType eBannerDataType, int32 nDataIndex);
CRefString GetBannerDataLabel(Console::BannerDataType eBannerDataType, int32 nIndex);
UIID GetElementUIIDForData(Console::BannerDataType eType);
UIID GetPortraitUIIDForData(Console::BannerDataType eType);
UIID GetBlinkerUIIDForData(Console::BannerDataType eType);
int32 GetRenderBannerDataSelection(Console::BannerDataType eBannerDataType);
BOOL BannerDataHasNewPieces(Console::BannerDataType eBannerDataType);
Console::BannerDataType GetSelectedDataType();
void UpdateButton1Legend(); void UpdateDetailsDisplay(Console::BannerDataType eType, int32 nDataIndex); void SetRenderBannerDataSelection(Console::BannerDataType eBannerDataType, int32 nDataIndex);
BOOL RingIncludeControlPredicate(const UIID & idControl); void SaveBannerSelectionsToAccount(); void UpdateBannerListDisplay();
}  // namespace

namespace Console {









































LPCSTR szcarTypeNames[NUM_BASIC_BANNERDATA_TYPES] = {
    "BannerShape",
    "BannerPattern",
    "BannerSigil",
    "BannerAccent",
    "ShapeDye",
    "PatternDye",
    "SigilDye",
    "BannerSigilPosition",
};


LPCSTR szcarTypeLabelStringKeys[NUM_BASIC_BANNERDATA_TYPES] = {
    "Bnet_BannerCustomization:BannerButton",
    "Bnet_BannerCustomization:PatternButton",
    "Bnet_BannerCustomization:SigilButton",
    "Bnet_BannerCustomization:SigilAccentButton",
    "Bnet_BannerCustomization:BackgroundDyeButton",
    "Bnet_BannerCustomization:PatternDyeButton",
    "Bnet_BannerCustomization:SigilDyeButton",
    "Bnet_BannerCustomization:SigilPositionButton",
};
class BannerListDataSource : public ConsoleListBoxDataSource {
public:
    virtual ~BannerListDataSource() {}


    BannerListDataSource() : m_eCurrentDataType(BANNERDATA_NONE) {}


    virtual int32 GetNumItems() const
    {
        return GetNumChoicesForBannerData(GetSelectedDataType()) - m_arHiddenIndices.size();
    }




    virtual UIControlSourceKey GetSourceKeyForItem(const int32 nIndex) const
    {
        return UIControlSourceKey::Parse("ConsoleBannerCustomization:ConsoleBannerCustomization_main.CustomizationListItemTemplate", SNO_NONE, NULL, NULL);
    }




    virtual void InitializeItemUI(const UIConsoleListBox &, const UIID &, const int32)
    {
    }







    virtual void UpdateItemUI(
        const UIConsoleListBox & tListBox,
        const UIID & idItemUI,
        const int32 nIndex)
    {
        int32 nDataIndex = GetDataIndex(nIndex);
        BannerDataType eType = GetSelectedDataType();

        UIID idLock = UIIDCreateFromParent(idItemUI, ".Lock");
        UIID idStar = UIIDCreateFromParent(idItemUI, ".Star");
        UIID idCheckmark = UIIDCreateFromParent(idItemUI, ".Checkmark");
        UIID idSelected = UIIDCreateFromParent(idItemUI, ".Selected");
        UIID idIcon = UIIDCreateFromParent(idItemUI, ".SigilPortrait");
        UIID idLabel = UIIDCreateFromParent(idItemUI, ".SigilName");
        UIID idArrow = UIIDCreateFromParent(idItemUI, ".Arrow");

        BOOL bIsLocked = IsBannerPieceLocked(eType, nDataIndex);
        BOOL bIsEquipped = nDataIndex == GetSavedBannerDataSelection(eType) &&
            (eType != BANNERDATA_SIGIL_MAIN || GetRenderSigilUsesVariant() == GetSavedSigilUsesVariant());
        BOOL bIsSelected = tListBox.GetSelectedIndex() == nIndex;
        BOOL bIsNew = IsBannerPieceNew(eType, nDataIndex);
        if (bIsSelected && bIsNew && UI_GetPrimaryUIController() != -1)
        {
            ::Console::GamerProfile::OnRewardViewed(UI_GetPrimaryUIController(), GetRewardIdForBannerData(eType, nDataIndex));
            RefreshRingDisplay();
        }

        UI_GetWindowFromID(idLock).SetActive(bIsLocked);
        UI_GetWindowFromID(idStar).SetActive(bIsNew);
        UI_GetWindowFromID(idCheckmark).SetActive(bIsEquipped);
        UI_GetWindowFromID(idSelected).SetActive(bIsSelected && UI_GetControllerFocus() == UILIST_CONSOLE_BANNER_CUSTOMIZATION_CHOICES);
        UI_GetWindowFromID(idArrow).SetActive(bIsSelected);

        UIWindow & tIcon = UI_GetWindowFromID(idIcon);
        if (eType >= BANNERDATA_BACKGROUND_COLOR && eType <= BANNERDATA_SIGIL_COLOR)
        {


            ScopedGlobalDefinitionLockR pGlobals;
            RGBAColor color = pGlobals->tBannerParams.arColorSets[nDataIndex].rgbaTint[0];
            tIcon.SetTextureFrame(ImageTextureFrame("UI:BattleNetBannerCustomization_DyeColor"));
            tIcon.SetWindowColor(color);
        }
        else
        {
            tIcon.SetWindowColor(crgbaWhite);
            tIcon.SetTextureFrame(GetIconTexture(eType, nDataIndex));
        }


















        CRefString sLabel = GetBannerDataLabel(eType, nDataIndex);
        UI_GetTextFromID(idLabel).SetText(sLabel, FALSE);

    }




    void Dispose()
    {
        m_arHiddenIndices.clear();
        m_eCurrentDataType = BANNERDATA_NONE;
    }


    BannerDataType GetCurrentDataType()
    {
        return m_eCurrentDataType;
    }


    int32 GetDataIndex(int32 nListIndex)
    {
        int32 nDataIndex = nListIndex;

        for (uint32 i = 0; i < m_arHiddenIndices.size() && m_arHiddenIndices[i] <= nDataIndex; i++)
        {
            nDataIndex++;
        }
        return nDataIndex;
    }

    int32 GetListIndex(int32 nDataIndex)
    {
        int32 nListIndex = nDataIndex;
        for (uint32 i = 0; i < m_arHiddenIndices.size() && nDataIndex >= m_arHiddenIndices[i]; i++)
        {
            nListIndex--;
        }
        return nListIndex;
    }

    void InitializeForDataType(BannerDataType eType);

private:
    blz::vector<int> m_arHiddenIndices;
    BannerDataType m_eCurrentDataType;
};













struct BannerCustomizationGlobals {
    BannerListDataSource tBannerListSource;
    ::Console::RingWrapper tRingUI;
    ::Console::AnalogStickInputFilter tRingStickInputFilter;
    ::Console::TouchGestureTracker tTouchInputHandler;
    OnlineService::GameAccountHandle idPlayer;
    PlayerAccountBanner * ptBanner;
    LightID idLight;
    BOOL bSaveSelectionForRender;
    BOOL bInitialized;
    BOOL bDirty;








    BannerCustomizationGlobals()
        : ptBanner(NULL)
        , idLight(-1)
        , bSaveSelectionForRender(TRUE)
        , bInitialized(FALSE)
        , bDirty(FALSE)
    {
    }



    void Initialize()
    {
        if (!bInitialized)
        {
            tRingStickInputFilter.AddRule(::Console::AnalogStickInputFilter::Rule::CatchAllRule);
            bInitialized = TRUE;
        }
    }
};



BannerCustomizationGlobals sgtBannerCustomizationGlobals;



void BannerListDataSource::InitializeForDataType(BannerDataType eType)
{
    m_eCurrentDataType = eType;
    m_arHiddenIndices.clear();

    int32 nNumChoices = GetNumChoicesForBannerData(eType);
    ::Console::Achievements & tAchievements = *sgtAppGlobals.ptAchievements;
    for (int32 nDataIndex = 0; nDataIndex < nNumChoices; nDataIndex++)
    {
        OnlineService::RewardDefinition tReward;
        BOOL bLocked = IsBannerPieceLocked(eType, nDataIndex);
        if (tAchievements.GetRewardDefinition(GetRewardIdForBannerData(eType, nDataIndex), tReward))
        {
            OnlineService::AchievementIdList arParents;
            tAchievements.GetRewardParentAchievements(tReward.m_id, arParents);
            OnlineService::SortRewardParentAchievements(arParents, &sgtBannerCustomizationGlobals.idPlayer);
            for (uint32 i = 0; i < arParents.size(); i++)
            {
                const OnlineService::AchievementDefinition * ptAchievement = tAchievements.GetAchievementDefinition(arParents[i]);
                if (ptAchievement)
                {
                    if (bLocked && ptAchievement->m_pointsValue == 0)
                    {
                        m_arHiddenIndices.push_back(nDataIndex);
                    }
                    break;
                }
            }
        }
    }
}























void OpenForController(int32 nControllerIndex)
{
    ::Console::Screens::ShowModalDialog(nControllerIndex, UIWND_CONSOLE_BANNER_CUSTOMIZATION, TRUE, GetUIIDNone());
    delete sgtBannerCustomizationGlobals.ptBanner;
    sgtBannerCustomizationGlobals.ptBanner = NULL;
    sgtBannerCustomizationGlobals.ptBanner = new PlayerAccountBanner;
    const D3::Account::SavedDefinition & tAccount = ::Console::GamerProfile::GetAccount(nControllerIndex);
    sgtBannerCustomizationGlobals.ptBanner->Import(tAccount.digest().banner_configuration());
    sgtBannerCustomizationGlobals.Initialize();
    sgtBannerCustomizationGlobals.idPlayer = ::Console::ClientAchievementsInterface::UserIndexToGameAccount(nControllerIndex);
    UI_SetControllerFocus(UILIST_CONSOLE_BANNER_CUSTOMIZATION_CHOICES);
    sgtBannerCustomizationGlobals.bDirty = FALSE;
    ::Console::ControllerActionManager & tActionManager = ::Console::Screens::GetActionManager();
    tActionManager.ClearAllActions();
    tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, 1, StringListFetch("ConsoleUI:Close"));
    ::Console::Screens::UpdateButtonLegendText();





    UIRActor & tBanner = UI_GetRActorFromID(UIRACTOR_CONSOLE_BANNER_CUSTOMIZATION_BANNER);
    sgtBannerCustomizationGlobals.idLight = LightAdd(0, UI_GetDefaultHeroLight(),
        PRTransform(cgqIdentity, cgwpNull), FALSE,
        tBanner.tRenderData[CGameAsyncGetRenderDataIndex(0)].m_tDisplayList.GetRenderContext());
    UpdateRenderedBanner();
    RefreshRingDisplay();
    sgtBannerCustomizationGlobals.tRingUI.WrapUsingContainerControl(UIWND_CONSOLE_BANNER_CUSTOMIZATION_RING, UIWND_CONSOLE_BANNER_CUSTOMIZATION_CURSOR, RingIncludeControlPredicate, FALSE);









    sgtBannerCustomizationGlobals.tRingUI.SelectControl(GetElementUIIDForData(BANNERDATA_SHAPE), IconSelectionChanged);
    FocusCategoryRing(TRUE);
}








void Close()
{
    if (IsVisible())
    {
        if (sgtBannerCustomizationGlobals.bDirty && UI_GetPrimaryUIController() != -1)
        {





            ::Console::GamerProfile::Save(UI_GetPrimaryUIController(), ::Console::GamerProfile::SaveCallback());
            if (GameIsStartUp())
            {
                ::Console::Online::PartyUpdatePlayerHero(UI_GetPrimaryUIController());
            }
        }
        UI_GetConsoleListBoxFromID(UILIST_CONSOLE_BANNER_CUSTOMIZATION_CHOICES).SetSelectedIndex(-1);
        UI_SetControllerFocus(GetUIIDNone());
        ::Console::Screens::CloseModalDialog(UIWND_CONSOLE_BANNER_CUSTOMIZATION);



        sgtBannerCustomizationGlobals.tBannerListSource.Dispose();
        delete sgtBannerCustomizationGlobals.ptBanner;
        sgtBannerCustomizationGlobals.ptBanner = NULL;
        UI_GetRActorFromID(UIRACTOR_CONSOLE_BANNER_CUSTOMIZATION_BANNER).Cleanup();
        if (sgtBannerCustomizationGlobals.idLight != -1)
        {
            LightDelete(sgtBannerCustomizationGlobals.idLight);
            sgtBannerCustomizationGlobals.idLight = -1;
        }
        if (GameIsStartUp())
        {
            UIMainMenu::Console::Refresh3DBanner();
        }
    }
}





BOOL IsVisible()
{
    return UI_GetWindowFromID(UIWND_CONSOLE_BANNER_CUSTOMIZATION).IsActive();
}




void OnUpdate(const UIID &)
{
}




void OnAccept(const UIID & id, int32 nControllerIndex)
{
    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SELECT_GENERIC), cgtPSODefaults);
    UIConsoleListBox & tList = UI_GetConsoleListBoxFromID(UILIST_CONSOLE_BANNER_CUSTOMIZATION_CHOICES);



    if (UI_GetControllerFocus() == UILIST_CONSOLE_BANNER_CUSTOMIZATION_CHOICES)
    {
        int32 nSelectedIndex = tList.GetSelectedIndex();
        if (nSelectedIndex == -1)
        {
            return;
        }
        int32 nDataIndex = sgtBannerCustomizationGlobals.tBannerListSource.GetDataIndex(nSelectedIndex);
        BannerDataType eSelectedData = GetSelectedDataType();
        if (eSelectedData != BANNERDATA_NONE && !IsBannerPieceLocked(eSelectedData, nDataIndex))
        {
            SaveBannerSelectionsToAccount();
            tList.RefreshVisibleItems();
            RefreshRingDisplay();
            UpdateRenderedBanner();
            FocusCategoryRing(TRUE);
        }
    }
    else
    {
        FocusCategoryRing(FALSE);
        tList.RefreshVisibleItems();
    }
}




void OnCancel(const UIID & id, int32 nControllerIndex)
{
    if (UI_GetControllerFocus() == UILIST_CONSOLE_BANNER_CUSTOMIZATION_CHOICES)
    {
        RevertToSavedBanner();
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SELECT_GENERIC), cgtPSODefaults);
        FocusCategoryRing(TRUE);
    }
    else if (!GameIsStartUp() && !::Console::Screens::IsScreenOpen(::Console::Screens::OPTIONS_SCREEN_SET, 0))
    {
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_PANEL_GENERIC_CLOSE), cgtPSODefaults);
        ::Console::Screens::GoToPreviousScreenSet(nControllerIndex, FALSE);
    }
    else
    {
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_PANEL_GENERIC_CLOSE), cgtPSODefaults);
        Close();
    }
}







void OnToggleVariant(const UIID & id, int32 nControllerIndex)
{
    if (sgtBannerCustomizationGlobals.tBannerListSource.GetCurrentDataType() == BANNERDATA_SIGIL_MAIN && UI_GetControllerFocus() == UILIST_CONSOLE_BANNER_CUSTOMIZATION_CHOICES)
    {
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SELECT_GENERIC), cgtPSODefaults);
        sgtBannerCustomizationGlobals.ptBanner->bUseSigilVariant = !sgtBannerCustomizationGlobals.ptBanner->bUseSigilVariant;
        UI_GetConsoleListBoxFromID(UILIST_CONSOLE_BANNER_CUSTOMIZATION_CHOICES).RefreshVisibleItems();
        UI_GetConsoleListBoxFromID(UILIST_CONSOLE_BANNER_CUSTOMIZATION_CHOICES).ForceCallToSelectionChange();
    }
}







void OnLeftStick(const UIID & id, int32 nControllerIndex)
{
    if (sgtBannerCustomizationGlobals.tRingStickInputFilter.HandleInput(::Console::ControllerUtils::GetRawLStickPosition(nControllerIndex, FALSE), FALSE) == 0)
    {
        sgtBannerCustomizationGlobals.tRingUI.UpdateInput(nControllerIndex, IconSelectionChanged, NULL);
    }
}






void OnLeftTrigger(const UIID & id, int32 nControllerIndex)
{
    UI_GetConsoleScrollBoxFromID(UISCROLL_CONSOLE_BANNER_CUSTOMIZATION_DETAILS_ACHV_INFO).Scroll(-4.0f);
}






void OnRightTrigger(const UIID & id, int32 nControllerIndex)
{
    UI_GetConsoleScrollBoxFromID(UISCROLL_CONSOLE_BANNER_CUSTOMIZATION_DETAILS_ACHV_INFO).Scroll(4.0f);
}






void OnBannerListSelectionChanged(const UIID & id)
{
    if (id == GetUIIDNone())
    {
        return;
    }


    UIConsoleListBox & tList = UI_GetConsoleListBoxFromID(id);
    int32 nSelectedIndex = tList.GetSelectedIndex();
    if (nSelectedIndex == -1)
    {
        return;
    }
    BannerDataType eType;
    int32 nDataIndex = sgtBannerCustomizationGlobals.tBannerListSource.GetDataIndex(nSelectedIndex);
    eType = GetSelectedDataType();
    SetRenderBannerDataSelection(eType, nDataIndex);
    UpdateDetailsDisplay(eType, nDataIndex);
    UpdateRenderedBanner();
    UpdateButton1Legend();
}

}  // namespace Console

namespace {
void EnableRing(BOOL bEnable)
{
    Console::sgtBannerCustomizationGlobals.tRingUI.SetEnabled(bEnable);
}
























void RevertToSavedBanner()

{

    int32 nPrimaryController = UI_GetPrimaryUIController();
    if (nPrimaryController != -1)
    {
        const D3::Account::Digest & tDigest = ::Console::GamerProfile::GetAccount(nPrimaryController).digest();

        Console::sgtBannerCustomizationGlobals.ptBanner->Import(tDigest.banner_configuration());
    }
    UpdateBannerListDisplay();
    UpdateRenderedBanner();
}

// Inference: loop test `!=` (no row of its own). With `<` our InstCombine folds the line 745 `<= SIGIL_COLOR` into `!= 7` from the dominating loop test; the original keeps the 4..6 range test.
void RefreshRingDisplay()

{
    for (int32 i = 0; i != Console::NUM_BASIC_BANNERDATA_TYPES; i++)
    {
        Console::BannerDataType eType = i;
        int32 nDataIndex = GetSavedBannerDataSelection(eType);
        UIID idIcon = GetPortraitUIIDForData(eType);
        UIWindow & tIcon = UI_GetWindowFromID(idIcon);
        if (eType >= Console::BANNERDATA_BACKGROUND_COLOR && eType <= Console::BANNERDATA_SIGIL_COLOR)
        {


            ScopedGlobalDefinitionLockR pGlobals;
            RGBAColor color = pGlobals->tBannerParams.arColorSets[nDataIndex].rgbaTint[0];
            tIcon.SetTextureFrame(ImageTextureFrame("UI:BattleNetBannerCustomization_DyeColor"));
            tIcon.SetWindowColor(color);
        }
        else
        {
            tIcon.SetWindowColor(crgbaWhite);
            tIcon.SetTextureFrame(GetIconTexture(eType, nDataIndex));
        }

        UIID idBlinker = GetBlinkerUIIDForData(eType);
        UI_GetWindowFromID(idBlinker).SetActive(BannerDataHasNewPieces(eType));
    }
}


BOOL RingIncludeControlPredicate(const UIID & idControl)
{
    return UI_GetControlFromID(idControl).IsType(UIT_BUTTON);
}




void IconSelectionChanged(const ::Console::RingWrapper &, const UIID & idIcon, const BOOL bSelected)
{
    if (idIcon != GetUIIDNone())
    {
        UI_GetButtonFromID(idIcon).SetButtonState(bSelected ? BS_MOUSEOVER : BS_NORMAL, FALSE, FALSE);
        if (GetSelectedDataType() != Console::sgtBannerCustomizationGlobals.tBannerListSource.GetCurrentDataType())
        {
            RevertToSavedBanner();
            UpdateButton1Legend();
        }
    }
}








void FocusCategoryRing(BOOL bFocus)
{
    EnableRing(bFocus);
    ::Console::ControllerActionManager & tActions = ::Console::Screens::GetActionManager();
    if (bFocus)
    {
        UI_SetControllerFocus(GetUIIDNone());
        tActions.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, 1, StringListFetch("ConsoleUI:Close"));
        tActions.ClearAction(::Console::CONTROLLER_INPUT_BUTTON_3);
    }
    else
    {
        UI_SetControllerFocus(UILIST_CONSOLE_BANNER_CUSTOMIZATION_CHOICES);
        tActions.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, 1, StringListFetch("ConsoleBannerCustomization:BannerCustomizationButtonB"));
        if (GetSelectedDataType() == Console::BANNERDATA_SIGIL_MAIN)
        {
            tActions.SetAction(::Console::CONTROLLER_INPUT_BUTTON_3, 2, StringListFetch("ConsoleBannerCustomization:BannerCustomizationButtonX"));
        }
    }
    ::Console::Screens::UpdateButtonLegendText();

    UI_GetWindowFromID(UIWND_CONSOLE_BANNER_CUSTOMIZATION_DIMMER).SetActive(bFocus);
    UI_GetWindowFromID(UIWND_CONSOLE_BANNER_CUSTOMIZATION_RING_GLOW).SetActive(bFocus);
    UI_GetWindowFromID(UIWND_CONSOLE_BANNER_CUSTOMIZATION_PANEL_FOCUS_HIGHLIGHT).SetActive(!bFocus);
    UpdateBannerHeader();
}






void UpdateRenderedBanner()
{
    UIRActor & tUIRActor = UI_GetRActorFromID(UIRACTOR_CONSOLE_BANNER_CUSTOMIZATION_BANNER);
    RActorID idRActor = tUIRActor.GetRActor();
    if (idRActor == RACTORID_INVALID)
    {
        idRActor = RActorCreate(GlobalSNOGet(GLOBALSNO_ACTOR_EMOTE_BANNER_LIT), 1.0f, -1.0f, PRTransform(cgqIdentity, cgwpNull), 0, tUIRActor.tRenderData[CGameAsyncGetRenderDataIndex(0)].m_tDisplayList.GetRenderContext(), -1);
        tUIRActor.SetRActor(idRActor);
        idRActor = tUIRActor.GetRActor();
    }

    RActor & tRActor = *RActorGet(idRActor);
    tRActor.snoIrradianceTex = GlobalSNOGet(GLOBALSNO_RACTOR_NEUTRAL_IRRADIANCE_TEXTURE);
    if (!tRActor.ptBannerRenderData)
    {
        tRActor.ptBannerRenderData = new BannerRenderData;
    }
    CPlayerUpdateBanner(*Console::sgtBannerCustomizationGlobals.ptBanner, *tRActor.ptBannerRenderData, FALSE);
    CPlayerUpdateBannerAttachments(tRActor, *tRActor.ptBannerRenderData);
}










void UpdateBannerHeader()
{
    Console::BannerDataType eType = GetSelectedDataType();
    CRefString sHeader = eType < 0 ? CRefString("") : StringListFetch(Console::szcarTypeLabelStringKeys[eType]);
    if (UI_GetControllerFocus() == UILIST_CONSOLE_BANNER_CUSTOMIZATION_CHOICES)
    {
        Set_Text_Color(sHeader, FONTCOLOR_WHITE);
    }
    UI_GetTextFromID(UITXT_CONSOLE_BANNER_CUSTOMIZATION_HEADER).SetText(sHeader, FALSE);
}





void UpdateBannerListDisplay()
{



    UpdateBannerHeader();

    UIConsoleListBox & tList = UI_GetConsoleListBoxFromID(UILIST_CONSOLE_BANNER_CUSTOMIZATION_CHOICES);
    Console::BannerDataType eType = GetSelectedDataType();
    Console::sgtBannerCustomizationGlobals.tBannerListSource.InitializeForDataType(eType);
    Console::sgtBannerCustomizationGlobals.bSaveSelectionForRender = FALSE;
    tList.SetDataSource(Console::sgtBannerCustomizationGlobals.tBannerListSource);
    tList.ResetAllItems();
    Console::sgtBannerCustomizationGlobals.bSaveSelectionForRender = TRUE;
    int32 nSelectedData = GetRenderBannerDataSelection(eType);
    int32 nSelectedIndex = Console::sgtBannerCustomizationGlobals.tBannerListSource.GetListIndex(nSelectedData);
    tList.SetSelectedIndex(nSelectedIndex);
}


void UpdateDetailsDisplay(Console::BannerDataType eType, int32 nDataIndex)
{



    UIWindow & tIcon = UI_GetWindowFromID(UIWND_CONSOLE_BANNER_CUSTOMIZATION_DETAILS_ICON);
    if (eType >= Console::BANNERDATA_BACKGROUND_COLOR && eType <= Console::BANNERDATA_SIGIL_COLOR)
    {


        ScopedGlobalDefinitionLockR pGlobals;
        RGBAColor color = pGlobals->tBannerParams.arColorSets[nDataIndex].rgbaTint[0];
        tIcon.SetTextureFrame(ImageTextureFrame("UI:BattleNetBannerCustomization_DyeColor"));
        tIcon.SetWindowColor(color);
    }
    else
    {
        tIcon.SetWindowColor(crgbaWhite);
        tIcon.SetTextureFrame(GetIconTexture(eType, nDataIndex));
    }

    CRefString sLabel = GetBannerDataLabel(eType, nDataIndex);
    UI_GetTextFromID(UITXT_CONSOLE_BANNER_CUSTOMIZATION_DETAILS_NAME).SetText(sLabel, FALSE);


    ::Console::Achievements & tAchievements = *sgtAppGlobals.ptAchievements;
    OnlineService::RewardId idReward = GetRewardIdForBannerData(eType, nDataIndex);
    OnlineService::RewardDefinition tReward;

    OnlineService::AchievementIdList listParentAchievements;
    tAchievements.GetRewardParentAchievements(idReward, listParentAchievements);
    OnlineService::SortRewardParentAchievements(listParentAchievements, &Console::sgtBannerCustomizationGlobals.idPlayer);

    const OnlineService::AchievementDefinition * ptAchievement;
    if (tAchievements.GetRewardDefinition(idReward, tReward)
        && listParentAchievements.size() > 0
        && (ptAchievement = tAchievements.GetAchievementDefinition(listParentAchievements[0])) != NULL)
    {
        OnlineService::AchievementRecord tAchRecord;

        BOOL bEarned = sgtAppGlobals.ptAchievementsInterface->FindAwardedAchievement(Console::sgtBannerCustomizationGlobals.idPlayer, sub_71000b4be0(), ptAchievement->m_id, tAchRecord);
        BOOL bIsFOS = ptAchievement->m_pointsValue == 0;

        CRefString sAchievementTitle = StringListFetch(ptAchievement->m_pszNameKey);
        CRefString sAchievementDesc = StringListFetch(ptAchievement->m_pszDescriptionKey);
        CRefString sAchievementIcon;
        sAchievementIcon.Format("UI:%s", ptAchievement->m_pszIconTexture);
        ImageTextureFrame tAchievementIcon(sAchievementIcon);

        UI_GetWindowFromID(UIWND_CONSOLE_BANNER_CUSTOMIZATION_DETAILS_ACHV_PORTRAIT).SetTextureFrame(tAchievementIcon);
        CRefString sDescription = LocalizedSprintf(
            StringListFetch("ConsoleBannerCustomization:BannerDescription").str(),
            sAchievementTitle.str(),
            sAchievementDesc.str(), NULL);
        UI_GetTextFromID(UITXT_CONSOLE_BANNER_CUSTOMIZATION_DETAILS_INFO).SetText(sDescription, FALSE);

        UI_GetWindowFromID(UIWND_CONSOLE_BANNER_CUSTOMIZATION_DETAILS_ACHV_EARNED).SetActive(bEarned && !bIsFOS);
        UI_GetWindowFromID(UIWND_CONSOLE_BANNER_CUSTOMIZATION_DETAILS_ACHV_UNEARNED).SetActive(!(bEarned || bIsFOS));
        UI_GetWindowFromID(UIWND_CONSOLE_BANNER_CUSTOMIZATION_DETAILS_FOS).SetActive(bIsFOS);

        if (bEarned)
        {
            CRefString sEarnedMessage = LocalizedSprintf(
                StringListFetch("ConsoleBannerCustomization:Earned").str(),
                UnicodeText::FormatDate(tAchRecord.m_timestampEarned, (FormatDateStyle)0, 0).str(), NULL);

            UI_GetTextFromID(UITXT_CONSOLE_BANNER_CUSTOMIZATION_DETAILS_STATUS).SetText(sEarnedMessage, FALSE);
        }
        else
        {

            UI_GetTextFromID(UITXT_CONSOLE_BANNER_CUSTOMIZATION_DETAILS_STATUS).SetText(StringListFetch("ConsoleBannerCustomization:Unearned"), FALSE);
        }


        UIConsoleScrollBox & tScrollBox = UI_GetConsoleScrollBoxFromID(UISCROLL_CONSOLE_BANNER_CUSTOMIZATION_DETAILS_ACHV_INFO);
        tScrollBox.SetActive(TRUE);
        tScrollBox.SetScrollPos(0.0f);
        ::Console::ControllerActionManager & tManager = ::Console::Screens::GetActionManager();
        if (tScrollBox.GetScrollLimit() > 0.000001f)
        {
            tManager.SetAction(::Console::CONTROLLER_INPUT_LEFT_TRIGGER, 2, StringListFetch("ConsoleAchievements:Scroll"));
            tManager.SetAction(::Console::CONTROLLER_INPUT_RIGHT_TRIGGER, 2, StringListFetch("ConsoleAchievements:Scroll"));
        }
        else
        {
            tManager.ClearAction(::Console::CONTROLLER_INPUT_LEFT_TRIGGER);
            tManager.ClearAction(::Console::CONTROLLER_INPUT_RIGHT_TRIGGER);
        }
    }
    else
    {
        UI_GetWindowFromID(UISCROLL_CONSOLE_BANNER_CUSTOMIZATION_DETAILS_ACHV_INFO).SetActive(FALSE);
        ::Console::ControllerActionManager & tManager = ::Console::Screens::GetActionManager();
        tManager.ClearAction(::Console::CONTROLLER_INPUT_LEFT_TRIGGER);
        tManager.ClearAction(::Console::CONTROLLER_INPUT_RIGHT_TRIGGER);
    }
    ::Console::Screens::UpdateButtonLegendText();
}



void SaveBannerSelectionsToAccount()
{
    int32 nControllerIndex = UI_GetPrimaryUIController();
    if (nControllerIndex == -1)
    {
        return;
    }
    const D3::Account::SavedDefinition & tAccount = ::Console::GamerProfile::GetAccount(nControllerIndex);

    PlayerAccountBanner tSavedAccountBanner(tAccount.digest().banner_configuration());
    D3::Account::BannerConfiguration * ptConfig = ::Console::GamerProfile::GetAccount(nControllerIndex).mutable_digest()->mutable_banner_configuration();
    Console::sgtBannerCustomizationGlobals.ptBanner->Export(*ptConfig);
    if (tSavedAccountBanner != *Console::sgtBannerCustomizationGlobals.ptBanner)
    {
        Console::sgtBannerCustomizationGlobals.bDirty = TRUE;
    }
    if (!GameIsStartUp())
    {
        MessageSendToServer(CMSG_BANNER_CONFIGURATION, *ptConfig, PlayerGetByControllerIndex(nControllerIndex));
    }
}





int32 GetNumChoicesForBannerData(Console::BannerDataType eBannerDataType)
{

    ScopedGlobalDefinitionLockR pdefGlobals;
    const BannerParams & tParams = pdefGlobals->tBannerParams;

    switch (eBannerDataType)
    {
    case Console::BANNERDATA_SHAPE:
        return tParams.serTexBackgrounds.ElementCount(sizeof(BannerTexturePair));
    case Console::BANNERDATA_PATTERN:
        return tParams.serTexPatterns.ElementCount(sizeof(BannerTexturePair));
    case Console::BANNERDATA_SIGIL_ACCENT:
        return tParams.serTexSigilAccents.ElementCount(sizeof(BannerTexturePair));
    case Console::BANNERDATA_BACKGROUND_COLOR:
        return tParams.serColorSets.ElementCount(sizeof(BannerColorSet));
    case Console::BANNERDATA_PATTERN_COLOR:
        return tParams.serColorSets.ElementCount(sizeof(BannerColorSet));
    case Console::BANNERDATA_SIGIL_COLOR:
        return tParams.serColorSets.ElementCount(sizeof(BannerColorSet));
    case Console::BANNERDATA_SIGIL_MAIN:
        if (Console::sgtBannerCustomizationGlobals.ptBanner->bUseSigilVariant)
        {
            return tParams.serTexVariantSigils.ElementCount(sizeof(BannerTexturePair));
        }
        else
        {
            return tParams.serTexMainSigils.ElementCount(sizeof(BannerTexturePair));
        }
    case Console::BANNERDATA_SIGIL_PLACEMENT:
        if (Console::sgtBannerCustomizationGlobals.ptBanner->bUseSigilVariant)
        {
            return tParams.serSigilPlacements.ElementCount(sizeof(BannerSigilPlacement));
        }
        else
        {
            return tParams.serSigilPlacements.ElementCount(sizeof(BannerSigilPlacement));
        }
    }

    return 0;
}



void SetRenderBannerDataSelection(Console::BannerDataType eBannerDataType, int32 nDataIndex)
{




    if (!Console::sgtBannerCustomizationGlobals.bSaveSelectionForRender)
    {
        return;
    }

    switch (eBannerDataType)
    {
    case Console::BANNERDATA_SHAPE:
        {
            Console::sgtBannerCustomizationGlobals.ptBanner->uBannerIndex = nDataIndex;
        }
        break;
    case Console::BANNERDATA_PATTERN:
        {
            Console::sgtBannerCustomizationGlobals.ptBanner->nPattern = nDataIndex;
        }
        break;
    case Console::BANNERDATA_SIGIL_MAIN:
        {
            Console::sgtBannerCustomizationGlobals.ptBanner->nSigilMain = nDataIndex;
        }
        break;
    case Console::BANNERDATA_SIGIL_ACCENT:
        {
            Console::sgtBannerCustomizationGlobals.ptBanner->nSigilAccent = nDataIndex;
        }
        break;
    case Console::BANNERDATA_BACKGROUND_COLOR:
        {
            Console::sgtBannerCustomizationGlobals.ptBanner->nBackgroundColorIndex = nDataIndex;
        }
        break;
    case Console::BANNERDATA_PATTERN_COLOR:
        {
            Console::sgtBannerCustomizationGlobals.ptBanner->nPatternColorIndex = nDataIndex;
        }
        break;
    case Console::BANNERDATA_SIGIL_COLOR:
        {
            Console::sgtBannerCustomizationGlobals.ptBanner->nSigilColorIndex = nDataIndex;
        }
        break;
    case Console::BANNERDATA_SIGIL_PLACEMENT:
        {
            Console::sgtBannerCustomizationGlobals.ptBanner->nPlacementIndex = nDataIndex;
        }
        break;
    }
}




int32 GetRenderBannerDataSelection(
    Console::BannerDataType eBannerDataType)
{
    int32 nNumChoices = GetNumChoicesForBannerData(eBannerDataType);
    if (nNumChoices > 0)
    {





        switch (eBannerDataType)
        {
        case Console::BANNERDATA_SHAPE:
            return Console::sgtBannerCustomizationGlobals.ptBanner->uBannerIndex % nNumChoices;
        case Console::BANNERDATA_PATTERN:
            return Console::sgtBannerCustomizationGlobals.ptBanner->nPattern % nNumChoices;
        case Console::BANNERDATA_PATTERN_COLOR:
            return Console::sgtBannerCustomizationGlobals.ptBanner->nPatternColorIndex % nNumChoices;
        case Console::BANNERDATA_BACKGROUND_COLOR:
            return Console::sgtBannerCustomizationGlobals.ptBanner->nBackgroundColorIndex % nNumChoices;
        case Console::BANNERDATA_SIGIL_MAIN:
            return Console::sgtBannerCustomizationGlobals.ptBanner->nSigilMain % nNumChoices;
        case Console::BANNERDATA_SIGIL_COLOR:
            return Console::sgtBannerCustomizationGlobals.ptBanner->nSigilColorIndex % nNumChoices;
        case Console::BANNERDATA_SIGIL_PLACEMENT:
            return Console::sgtBannerCustomizationGlobals.ptBanner->nPlacementIndex % nNumChoices;
        case Console::BANNERDATA_SIGIL_ACCENT:
            return Console::sgtBannerCustomizationGlobals.ptBanner->nSigilAccent % nNumChoices;
        }
    }
    return -1;
}


int32 GetSavedBannerDataSelection(Console::BannerDataType eBannerDataType)
{
    int32 nNumChoices = GetNumChoicesForBannerData(eBannerDataType);
    if (nNumChoices <= 0)
    {
        return -1;
    }

    int32 nPrimaryController = UI_GetPrimaryUIController();
    if (nPrimaryController == -1)
    {
        return -1;
    }

    const D3::Account::Digest & tDigest = ::Console::GamerProfile::GetAccount(nPrimaryController).digest();
    PlayerAccountBanner tSavedBanner;
    tSavedBanner.Import(tDigest.banner_configuration());

    switch (eBannerDataType)
    {
    case Console::BANNERDATA_SHAPE:
        return tSavedBanner.uBannerIndex % nNumChoices;
    case Console::BANNERDATA_PATTERN:
        return tSavedBanner.nPattern % nNumChoices;
    case Console::BANNERDATA_SIGIL_MAIN:
        return tSavedBanner.nSigilMain % nNumChoices;
    case Console::BANNERDATA_SIGIL_ACCENT:
        return tSavedBanner.nSigilAccent % nNumChoices;
    case Console::BANNERDATA_BACKGROUND_COLOR:
        return tSavedBanner.nBackgroundColorIndex % nNumChoices;
    case Console::BANNERDATA_PATTERN_COLOR:
        return tSavedBanner.nPatternColorIndex % nNumChoices;
    case Console::BANNERDATA_SIGIL_COLOR:
        return tSavedBanner.nSigilColorIndex % nNumChoices;
    case Console::BANNERDATA_SIGIL_PLACEMENT:
        return tSavedBanner.nPlacementIndex % nNumChoices;
    }
    return -1;
}





BOOL GetSavedSigilUsesVariant()
{



    int32 nPrimaryController = UI_GetPrimaryUIController();
    if (nPrimaryController != -1)
    {


        const D3::Account::Digest & tDigest = ::Console::GamerProfile::GetAccount(nPrimaryController).digest();
        PlayerAccountBanner tSavedBanner;
        tSavedBanner.Import(tDigest.banner_configuration());

        return tSavedBanner.bUseSigilVariant;
    }
    return FALSE;
}
BOOL GetRenderSigilUsesVariant()
{

    return Console::sgtBannerCustomizationGlobals.ptBanner->bUseSigilVariant;
}


ImageTextureFrame GetIconTexture(
    Console::BannerDataType eBannerDataType,
    int32 nDataIndex)
{
    using namespace Console;

    ScopedGlobalDefinitionLockR pdefGlobals;
    const BannerParams & tParams = pdefGlobals->tBannerParams;
    ImageTextureFrame tTexture(SNO_NONE, 0);
    switch (eBannerDataType)
    {
        case BANNERDATA_SHAPE:
            {
                int32 nNumIcons = tParams.serTexBackgrounds.ElementCount(sizeof(BannerTexturePair));
                if (nNumIcons > 0)
                {
                    ImageCollectionGetImageInfo(
                        IMAGE_COLLECTION_UI,
                        tParams.arTexBackgrounds[nDataIndex % nNumIcons].hIcon,
                        tTexture);
                }
            }
            break;
        case BANNERDATA_PATTERN:
            {
                int32 nNumIcons = tParams.serTexPatterns.ElementCount(sizeof(BannerTexturePair));
                if (nNumIcons > 0)
                {
                    ImageCollectionGetImageInfo(
                        IMAGE_COLLECTION_UI,
                        tParams.arTexPatterns[nDataIndex % nNumIcons].hIcon,
                        tTexture);
                }
            }
            break;
        case BANNERDATA_BACKGROUND_COLOR:
        case BANNERDATA_PATTERN_COLOR:
        case BANNERDATA_SIGIL_COLOR:
            {
                int32 nNumColors = tParams.serColorSets.ElementCount(sizeof(BannerColorSet));
                if (nNumColors > 0)
                {
                    ImageCollectionGetImageInfo(
                        IMAGE_COLLECTION_UI,
                        tParams.arColorSets[nDataIndex % nNumColors].hIcon,
                        tTexture);
                }
            }
            break;
        case BANNERDATA_SIGIL_MAIN:
            {
                if (sgtBannerCustomizationGlobals.ptBanner->bUseSigilVariant)
                {
                    int32 nNumIcons = tParams.serTexVariantSigils.ElementCount(sizeof(BannerTexturePair));
                    if (nNumIcons > 0)
                    {
                        ImageCollectionGetImageInfo(
                            IMAGE_COLLECTION_UI,
                            tParams.arTexVariantSigils[nDataIndex % nNumIcons].hIcon,
                            tTexture);
                    }
                }
                else
                {
                    int32 nNumIcons = tParams.serTexMainSigils.ElementCount(sizeof(BannerTexturePair));
                    if (nNumIcons > 0)
                    {
                        ImageCollectionGetImageInfo(
                            IMAGE_COLLECTION_UI,
                            tParams.arTexMainSigils[nDataIndex % nNumIcons].hIcon,
                            tTexture);
                    }
                }
            }
            break;
        case BANNERDATA_SIGIL_PLACEMENT:
            {
                int32 nNumIcons = tParams.serSigilPlacements.ElementCount(sizeof(BannerSigilPlacement));
                if (nNumIcons > 0)
                {
                    ImageCollectionGetImageInfo(
                        IMAGE_COLLECTION_UI,
                        tParams.arSigilPlacements[nDataIndex % nNumIcons].hIcon,
                        tTexture);
                }
            }
            break;
        case BANNERDATA_SIGIL_ACCENT:
            {
                int32 nNumIcons = tParams.serTexSigilAccents.ElementCount(sizeof(BannerTexturePair));
                if (nNumIcons > 0)
                {
                    ImageCollectionGetImageInfo(
                        IMAGE_COLLECTION_UI,
                        tParams.arTexSigilAccents[nDataIndex % nNumIcons].hIcon,
                        tTexture);
                }
            }
            break;
    }




    return tTexture;
}

OnlineService::RewardId GetRewardIdForBannerData(Console::BannerDataType eBannerDataType, int32 nDataIndex)
{
    ScopedGlobalDefinitionLockR pdefGlobals;
    const BannerParams & tParams = pdefGlobals->tBannerParams;

    switch (eBannerDataType)
    {
    case Console::BANNERDATA_SHAPE:
    {




        int32 nNumIcons = tParams.serTexBackgrounds.ElementCount(sizeof(BannerTexturePair));
        if (nNumIcons > 0)
        {
            return tParams.arTexBackgrounds[nDataIndex % nNumIcons].idReward;
        }
        break;
    }
    case Console::BANNERDATA_PATTERN:
    {
        int32 nNumIcons = tParams.serTexPatterns.ElementCount(sizeof(BannerTexturePair));
        if (nNumIcons > 0)
        {
            return tParams.arTexPatterns[nDataIndex % nNumIcons].idReward;
        }
        break;
    }
    case Console::BANNERDATA_BACKGROUND_COLOR:
    case Console::BANNERDATA_PATTERN_COLOR:
    case Console::BANNERDATA_SIGIL_COLOR:
    {
        int32 nNumColors = tParams.serColorSets.ElementCount(sizeof(BannerColorSet));
        if (nNumColors > 0)
        {
            return tParams.arColorSets[nDataIndex % nNumColors].idReward;
        }
        break;
    }
    case Console::BANNERDATA_SIGIL_MAIN:
        if (GetRenderSigilUsesVariant())
        {

            int32 nNumIcons = tParams.serTexVariantSigils.ElementCount(sizeof(BannerTexturePair));
            if (nNumIcons > 0)
            {
                return tParams.arTexVariantSigils[nDataIndex % nNumIcons].idReward;
            }
        }
        else
        {
            int32 nNumIcons = tParams.serTexMainSigils.ElementCount(sizeof(BannerTexturePair));
            if (nNumIcons > 0)
            {
                return tParams.arTexMainSigils[nDataIndex % nNumIcons].idReward;
            }
        }
        break;
    case Console::BANNERDATA_SIGIL_ACCENT:
    {

        int32 nNumIcons = tParams.serTexSigilAccents.ElementCount(sizeof(BannerTexturePair));
        if (nNumIcons > 0)
        {
            return tParams.arTexSigilAccents[nDataIndex % nNumIcons].idReward;
        }
        break;
    }
    }
    return 0;
}




// Inference: flag test written first in the guard (the binary tests nDataIndex first); with nDataIndex first ours narrows the flag load to ldrb and keeps a separate FALSE block.
BOOL IsBannerPieceLocked(
    Console::BannerDataType eBannerDataType,
    int32 nDataIndex)
{
    AppGlobals & tAppGlobals = sgtAppGlobals;
    if ((tAppGlobals.dwDebugFlags & (1 << APP_GLOBAL_BATTLENET_VALIDATION_DISABLED_BIT)) || nDataIndex < 0)
    {
        return FALSE;
    }






        ::Console::Achievements & tAchievements = *tAppGlobals.ptAchievements;
        OnlineService::RewardId idReward = GetRewardIdForBannerData(eBannerDataType, nDataIndex);
        OnlineService::RewardDefinition tReward;
        if (tAchievements.GetRewardDefinition(idReward, tReward))
        {

            ::Console::ClientAchievementsInterface * ptAchievementsInterface = tAppGlobals.ptAchievementsInterface;

            OnlineService::AchievementRecord tAchRecord;
            OnlineService::AchievementIdList listParentAchievements;
            tAchievements.GetRewardParentAchievements(idReward, listParentAchievements);
            BOOL bEarned = FALSE;
            for (uint32 i = 0; i < listParentAchievements.size(); i++)
            {
                if (ptAchievementsInterface->FindAwardedAchievement(Console::sgtBannerCustomizationGlobals.idPlayer, sub_71000b4be0(),
                    listParentAchievements[i], tAchRecord))
                {
                    bEarned = TRUE;
                }
            }
            return !bEarned;
        }

    return FALSE;
}

BOOL IsBannerPieceNew(
    Console::BannerDataType eBannerDataType,
    int32 nDataIndex)
{
    return UI_GetPrimaryUIController() != -1 &&
        ::Console::GamerProfile::IsRewardNew(UI_GetPrimaryUIController(), GetRewardIdForBannerData(eBannerDataType, nDataIndex));
}


BOOL BannerDataHasNewPieces(
    Console::BannerDataType eBannerDataType)
{
    int32 nNumChoices = GetNumChoicesForBannerData(eBannerDataType);
    for (int32 i = 0; i < nNumChoices; i++)
    {
        if (IsBannerPieceNew(eBannerDataType, i))
        {
            return TRUE;
        }
    }
    return FALSE;
}
























CRefString GetBannerDataLabel(
    Console::BannerDataType eBannerDataType,
    int32 nIndex)
{
    CRefString sLabel = "";
    int32 nNumChoices = GetNumChoicesForBannerData(eBannerDataType);
    if (nNumChoices > 0)
    {



        using namespace Console;

        nIndex = Math::Pin(nIndex, 0, nNumChoices);
        ScopedGlobalDefinitionLockR ptGlobals;

        switch (eBannerDataType)
        {
            case BANNERDATA_SHAPE:
                {
                    const BannerTexturePair & tTexturePair = ptGlobals->tBannerParams.arTexBackgrounds[nIndex];
                    sLabel = StringListFetchByLabel(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_BANNER_SHAPES), tTexturePair.hName);
                }
                break;
            case BANNERDATA_PATTERN:
                {
                    const BannerTexturePair & tTexturePair = ptGlobals->tBannerParams.arTexPatterns[nIndex];
                    sLabel = StringListFetchByLabel(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_BANNER_PATTERNS), tTexturePair.hName);
                }
                break;
            case BANNERDATA_BACKGROUND_COLOR:
            case BANNERDATA_PATTERN_COLOR:
            case BANNERDATA_SIGIL_COLOR:
                {
                    const BannerColorSet & tColorSet = ptGlobals->tBannerParams.arColorSets[nIndex];
                    sLabel = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_BANNER_COLORS), tColorSet.szTagName);
                }
                break;
            case BANNERDATA_SIGIL_MAIN:
                {
                    const BannerTexturePair & tTexturePair = sgtBannerCustomizationGlobals.ptBanner->bUseSigilVariant ?
                        ptGlobals->tBannerParams.arTexVariantSigils[nIndex] :
                        ptGlobals->tBannerParams.arTexMainSigils[nIndex];
                    sLabel = StringListFetchByLabel(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_BANNER_SIGILS), tTexturePair.hName);
                }
                break;
            case BANNERDATA_SIGIL_PLACEMENT:
                {
                    const BannerSigilPlacement & tSigilPlacement = ptGlobals->tBannerParams.arSigilPlacements[nIndex];
                    sLabel = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_BANNER_SIGIL_PLACEMENTS), tSigilPlacement.szName);
                }
                break;
            case BANNERDATA_SIGIL_ACCENT:
                {
                    const BannerTexturePair & tTexturePair = ptGlobals->tBannerParams.arTexSigilAccents[nIndex];
                    sLabel = StringListFetchByLabel(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_BANNER_ACCENTS), tTexturePair.hName);
                }
                break;
        }
    }


    return sLabel;
}


UIID GetElementUIIDForData(Console::BannerDataType eType)
{
    CRefString sName = CRefString().Format(".%s_Select", Console::szcarTypeNames[eType]);
    return UIIDCreateFromParent(UIWND_CONSOLE_BANNER_CUSTOMIZATION_RING, sName);
}





UIID GetPortraitUIIDForData(
    Console::BannerDataType eType)
{

    CRefString sItemName = CRefString().Format(".%s_Portrait", Console::szcarTypeNames[eType]);
    UIID idItem = UIIDCreateFromParent(UIWND_CONSOLE_BANNER_CUSTOMIZATION_RING, sItemName);
    return idItem;
}


UIID GetBlinkerUIIDForData(
    Console::BannerDataType eType)
{

    CRefString sItemName = CRefString().Format(".%s_New", Console::szcarTypeNames[eType]);
    UIID idItem = UIIDCreateFromParent(UIWND_CONSOLE_BANNER_CUSTOMIZATION_RING, sItemName);
    return idItem;
}


Console::BannerDataType GetSelectedDataType()
{
    const UIID & idSelected = Console::sgtBannerCustomizationGlobals.tRingUI.GetSelectedControlID();
    for (Console::BannerDataType eType = 0; eType < Console::NUM_BASIC_BANNERDATA_TYPES; eType++)
    {
        if (idSelected == GetElementUIIDForData(eType))
        {
            return eType;
        }
    }
    return Console::BANNERDATA_NONE;
}



void UpdateButton1Legend()

{
    Console::BannerDataType eSelectedType = GetSelectedDataType();
    if (eSelectedType != Console::BANNERDATA_NONE)
    {
        int32 nSelectedIndex = UI_GetConsoleListBoxFromID(UILIST_CONSOLE_BANNER_CUSTOMIZATION_CHOICES).GetSelectedIndex();
        if (nSelectedIndex >= 0)
        {
            ::Console::ControllerActionManager & tActionManager = ::Console::Screens::GetActionManager();
            if (UI_GetControllerFocus() == UILIST_CONSOLE_BANNER_CUSTOMIZATION_CHOICES &&
                IsBannerPieceLocked(eSelectedType, nSelectedIndex))
            {
                tActionManager.ClearAction(::Console::CONTROLLER_INPUT_BUTTON_1);
            }
            else
            {
                CRefString sALegend = LocalizedSprintf(
                    StringListFetch("ConsoleBannerCustomization:SelectCategory").str(),
                    StringListFetch(Console::szcarTypeLabelStringKeys[eSelectedType]).str(), NULL);
                tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, 0, sALegend);
            }
            ::Console::Screens::UpdateButtonLegendText();
        }
    }
}

}  // namespace
}  // namespace UIBannerCustomization
