// Original file: X/Client/UserInterface/Console/UIConsoleGamePrivacyOptions.cpp
#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Battlenet/Subsystems/ModalNotificationManager.h"
#include "../Controls/UIConsoleListboxControl.h"
#include "../Controls/UITextControl.h"
// SNOName.h, WorldPlace.h and GfxVB.h only for their statics: the original's static initialiser
// runs them ahead of Online.h's nn::ldn constants.
#include "../../../Common/SNO/SNOName.h"
#include "../../../Common/WorldPlace.h"
#include "../../Gfx/GfxVB.h"
#include "../../Console/Online.h"
#include "../../Console/GamerProfile.h"
#include "../../Console/Utilities.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/OnlineService/Callback/Callback_Impl.h"
#include "../../../Sigma/RGBAColor.h"
#include "../../../Sigma/RefString.h"
#include "../../Sound/Sound.h"
#include "../UI.h"
#include "../UIID.h"
#include "../UIIDs.h"
#include "ConsoleModalNotificationManager.h"
#include "ConsoleScreenManager.h"
#include "UIConsoleGameSettings.h"
#include "UIConsoleUtils.h"
// OnlineActions.h's declaration; that subset does not include Online.h yet.
namespace Console { namespace UIOnlineActions { void ShowPartySwitchOnlineResultMessage(::Console::Online::PartySwitchOnlineResult eResult, int32 nControllerIndex, const char * szMessage); } }
namespace UIGamePrivacyOptions { namespace Console {
class GamePrivacyOptionsDataSource : public ConsoleListBoxDataSource {
public:
    GamePrivacyOptionsDataSource();
    virtual ~GamePrivacyOptionsDataSource();
    virtual int32 GetNumItems() const;
    virtual UIControlSourceKey GetSourceKeyForItem(const int32 nIndex) const;
    virtual void InitializeItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex);
    virtual void UpdateItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex);
};
::Console::Online::PartyOnlineState arPossiblePrivacyOptions[] = {
    ::Console::Online::PARTY_ONLINE_INVITEONLY,
    ::Console::Online::PARTY_ONLINE_FRIENDSONLY,
    ::Console::Online::PARTY_ONLINE_PUBLIC,
};

GamePrivacyOptionsDataSource::~GamePrivacyOptionsDataSource()
{
}
inline GamePrivacyOptionsDataSource::GamePrivacyOptionsDataSource()
{
}




int32 GamePrivacyOptionsDataSource::GetNumItems() const
{
    return 3;
}



UIControlSourceKey GamePrivacyOptionsDataSource::GetSourceKeyForItem(const int32 nIndex) const
{
    return UIControlSourceKey::Parse("ConsoleGamePrivacyOptions:ConsoleGamePrivacyOptions_main.menuItem", -1, NULL, NULL);
}




void GamePrivacyOptionsDataSource::InitializeItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex)
{
}








void GamePrivacyOptionsDataSource::UpdateItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex)
{
    UIID idText = UIIDCreateFromParent(idItemUI, ".text");
    CRefString sLabel = UIUtils::Console::DisplayPartyOnlineState(arPossiblePrivacyOptions[nIndex], TRUE, 2.0f);
    UIText & tText = UI_GetTextFromID(idText);
    tText.SetText(sLabel, FALSE);

    RGBAColor rgbaTextColor;
    if (tListBox.GetSelectedIndex() == nIndex)
    {
        rgbaTextColor = crgbaWhite;
    }
    else
    {
        rgbaTextColor = crgbaGold;
    }

    tText.SetTextColor(rgbaTextColor);
}





struct UIGamePrivacyOptionsData
{
    GamePrivacyOptionsDataSource tGamePrivacyListSource;
    int32 nOpeningControllerIndex;
    ModalNotificationId idBlockingDialog;

    UIGamePrivacyOptionsData()
        : nOpeningControllerIndex(-1), idBlockingDialog(INVALID_MODAL_NOTIFICATION_ID)
    {
    }
};

UIGamePrivacyOptionsData sgtGamePrivacyOptionsData;















void DisplayGamePrivacyOptions(int32 nControllerIndex)
{
    sgtGamePrivacyOptionsData.nOpeningControllerIndex = nControllerIndex;

    UIConsoleListBox & tListbox = UI_GetConsoleListBoxFromID(UILIST_CONSOLE_GAME_PRIVACY_OPTIONS);
    tListbox.SetDataSource(sgtGamePrivacyOptionsData.tGamePrivacyListSource);
    tListbox.RefreshVisibleItems();

    int32 nSelectedIndex = 0;
    for (int32 i = 0; i < 3; ++i)
    {
        if (arPossiblePrivacyOptions[i] == ::Console::Online::PartyGetOnlineState())
        {
            nSelectedIndex = i;
            break;
        }
    }
    tListbox.SetSelectedIndex(nSelectedIndex);

    UI_GetTextFromID(UIIDCreateFromParent(UIWND_CONSOLE_GAME_PRIVACY_OPTIONS, ".Buttons.ButtonList.OkButton")).ResizeToFitText();
    UI_GetTextFromID(UIIDCreateFromParent(UIWND_CONSOLE_GAME_PRIVACY_OPTIONS, ".Buttons.ButtonList.Cancel")).ResizeToFitText();

    ::Console::Screens::ShowModalDialog(::Console::GamerProfile::GetPrimaryProfileUserIndex(), UIWND_CONSOLE_GAME_PRIVACY_OPTIONS, TRUE, GetUIIDNone());
    UI_SetControllerFocus(UILIST_CONSOLE_GAME_PRIVACY_OPTIONS);
}









void ShowGamePrivacyOptions(int32 nControllerIndex)
{
    ::Console::Online::PartySwitchOnlineResult eResult = ::Console::Online::PartyCanSwitchOnlineState(::Console::Online::PARTY_ONLINE);
    if (eResult == ::Console::Online::PARTY_SWITCH_ONLINE_OK || eResult == ::Console::Online::PARTY_SWITCH_ONLINE_WILL_DISCONNECT)
    {
        DisplayGamePrivacyOptions(nControllerIndex);
    }
    else if (eResult != ::Console::Online::PARTY_SWITCH_ONLINE_PREMIUM_SERVICE_REQUIRED)
    {
        ::Console::UIOnlineActions::ShowPartySwitchOnlineResultMessage(eResult, nControllerIndex, NULL);
    }
    else
    {
        // Retry once the upsell succeeds.
        ::Console::Utilities::ShowNX64Upsell(nControllerIndex,
            OnlineService::internal::function_binder_1<void, int32>::bind<&ShowGamePrivacyOptions>(nControllerIndex),
            [](){});
    }
}










static void sCloseDialog()
{
    if (UI_GetWindowFromID(UIWND_CONSOLE_GAME_PRIVACY_OPTIONS).IsActive())
        ::Console::Screens::CloseModalDialog(UIWND_CONSOLE_GAME_PRIVACY_OPTIONS);
}


void CloseGamePrivacyOptions()
{
    sgtGamePrivacyOptionsData.nOpeningControllerIndex = -1;
    if (sgtGamePrivacyOptionsData.idBlockingDialog != INVALID_MODAL_NOTIFICATION_ID)
    {
        ::Console::Screens::GetModalNotificationManager().RemoveNotification(sgtGamePrivacyOptionsData.idBlockingDialog);
        sgtGamePrivacyOptionsData.idBlockingDialog = INVALID_MODAL_NOTIFICATION_ID;
    }

    sCloseDialog();
}





void OnGamePrivacyOptionsCancel(const UIID & id, const int32 nControllerIndex)
{
    if (sgtGamePrivacyOptionsData.nOpeningControllerIndex != nControllerIndex)
    {
        return;
    }

    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_PANEL_GENERIC_CLOSE), cgtPSODefaults);

    // Closes the dialog.
    CloseGamePrivacyOptions();
}


void OnGamePrivacyOptionsAccept(const UIID & id, const int32 nControllerIndex)
{
    if (sgtGamePrivacyOptionsData.nOpeningControllerIndex != nControllerIndex)
    {
        return;
    }

    UIConsoleListBox & tListbox = UI_GetConsoleListBoxFromID(UILIST_CONSOLE_GAME_PRIVACY_OPTIONS);

    if (tListbox.GetSelectedIndex() != -1 && ::Console::Online::PartyIsInParty())
    {
        ::Console::Online::PartyOnlineState eNewState = arPossiblePrivacyOptions[tListbox.GetSelectedIndex()];

        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SELECT_GENERIC), cgtPSODefaults);

        // Closes the dialog.
        sCloseDialog();

        if (eNewState == ::Console::Online::PARTY_ONLINE_PUBLIC)
        {
            // Pick a tag.
            UIGameSettings::Console::OpenForTags(nControllerIndex, UIGameSettings::Console::ScreenClosedCallback());
        }

        ::Console::Online::PartySwitchPreferredOnlineState(eNewState);
    }
}

}  // namespace Console
}  // namespace UIGamePrivacyOptions
