// Original file: X/Client/UserInterface/Console/UIConsoleWhatsNew.cpp
#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Common/SNO/SNOName.h"
#include "../../../Common/WorldPlace.h"
#include "../../../Sigma/Math/Vector.h"
#include "../../../Sigma/RGBAColor.h"
#include "../../Gfx/GfxVB.h"
#include "../../../../NintendoSDK/Include/nn/ldn/ldn_Ipv4Address.h"
#include "../../../../NintendoSDK/Include/nn/ldn/ldn_MacAddress.h"
#include "../../../../Contrib/Contrib/blz/include/blz/vector.h"
#include "../../../../BlizzardCore/Include/Blizzard/Time.h"
#include "ConsoleScreenManager.h"
#include "ControllerActions.h"
#include "../Controls/UIButtonControl.h"
#include "../Controls/UIControlControl.h"
#include "../Controls/UICutsceneControl.h"
#include "../Controls/UIWindowControl.h"
#include "../UI.h"
#include "../UIID.h"
#include "../../Definitions/PreferencesDefinition.h"
#include "../../Sound/Sound.h"
#include "../../../Common/Console/ConsoleLicenses.h"
#include "../../../Common/Definitions/StringListDefinition.h"
#include "../../../Common/Game.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/OnlineService/OnlineService.h"
#include "../../../Common/Shell/CmdLine.h"
#include "../../../Sigma/DataStructures/XBitArray.h"
#include "../../../Sigma/Math/MathGeneral.h"
#include "../../../Sigma/RefString.h"
// Declared here because their headers are not adopted yet (UI.cpp, UIGlobals.cpp, GamerProfile.cpp, Online.cpp, DLCManager.cpp, UIConsoleHeroCreate.cpp, UIConsoleStartScreen.cpp, UIConsoleMainMenu.cpp, UIConsoleWhatsNewSeason.cpp).
UICutscene & UI_GetCutsceneFromID(const UIID & id); BOOL UI_TestID(const UIID & id);
extern UIID UIWND_CONSOLE_WHATS_NEW; extern UIID UIWND_CONSOLE_WHATS_NEW_FEATURES; extern UIID UIWND_CONSOLE_WHATS_NEW_ANNIVERSARY; extern UIID UIWND_CONSOLE_WHATS_NEW_NECROMANCER; extern UIID UITEXT_CONSOLE_WHATS_NEW_SEASON_NAVIGATION; extern UIID UIWND_CONSOLE_HEROCREATE;
namespace Console { namespace GamerProfile { void SavePrefs(); } namespace Online { BOOL IsSeasonsInitialized(); } namespace DLCManager { uint32 GetLicenses(); } }
namespace UIHeroCreate { namespace Console { void StopVideo(); void RestartVideo(); } } namespace UIStartScreen { namespace Console { void OnWhatsNewComplete(int32 nControllerIndex); } }
namespace UIMainMenu { namespace Console { void OpenStoreToNecromancer(int32 nControllerIndex); } } namespace UIWhatsNewSeason { namespace Console { void OpenForController(int32 nControllerIndex); } }
namespace UIWhatsNew {
namespace Console {

static const float LATEST_WHATS_NEW_VERSION = 2.6f;

UIID idPreviousWindow = GetUIIDNone();
int32 nActivePage = -1;
int32 nActiveWhatsNew = -1;
int32 nNecromancerScreen = -1;
BOOL bForceAnniversary;
BOOL bForceNecromancer;
BOOL bSwitchingToSeason;

blz::vector<UIID> vecHistory;

void Close();





























inline int32 sGetAnniversaryYear()
{


    const Blizzard::Time::Timestamp tCurTime = Blizzard::Time::GetTimestamp();
    Blizzard::Time::TimeRec tTimeRecord; Blizzard::Time::BreakTime(tCurTime, tTimeRecord);

    return tTimeRecord.year;
}

void OpenForController(int32 nControllerIndex, BOOL bAnniversary, BOOL bNecromancer)
{


    int32 nAnniversaryScreen = -1;
    bForceAnniversary = bAnniversary;
    bForceNecromancer = bNecromancer;

    ::Console::Screens::ShowModalDialog(nControllerIndex, UIWND_CONSOLE_WHATS_NEW, TRUE, GetUIIDNone());

    UIWindow & tWnd = UI_GetWindowFromID(UIWND_CONSOLE_WHATS_NEW_FEATURES);
    const UIControlItemArray & arChildren = tWnd.GetChildList();




    for (uint32 i = 0; i < arChildren.size(); ++i)
    {
        if (arChildren[i].id == UIWND_CONSOLE_WHATS_NEW_ANNIVERSARY && GameGetAnniversaryEventStatus() != ANNIVERSARY_EVENT_START)
        {
            continue;
        }

        if (arChildren[i].id == UIWND_CONSOLE_WHATS_NEW_NECROMANCER && (::Console::DLCManager::GetLicenses() & (1 << LICENSE_NECROMANCER)))
        {
            continue;
        }

        vecHistory.push_back(arChildren[i].id);

        if (arChildren[i].ptControl->IsActive())
        {
            nActivePage = vecHistory.size() - 1;
            nActiveWhatsNew = vecHistory.size() - 1;
        }

        if (arChildren[i].id == UIWND_CONSOLE_WHATS_NEW_ANNIVERSARY)
        {
            nAnniversaryScreen = vecHistory.size() - 1;
        }

        if (arChildren[i].id == UIWND_CONSOLE_WHATS_NEW_NECROMANCER)
        {
            nNecromancerScreen = vecHistory.size() - 1;
        }
    }

    if (nAnniversaryScreen != -1 && bForceAnniversary)
    {
        UI_GetWindowFromID(vecHistory[nActivePage]).SetActive(FALSE);
        UI_GetWindowFromID(vecHistory[nAnniversaryScreen]).SetActive(TRUE);
        nActivePage = nAnniversaryScreen;
    }
    else if (bForceNecromancer && nNecromancerScreen != -1)
    {
        UI_GetWindowFromID(vecHistory[nActivePage]).SetActive(FALSE);
        UI_GetWindowFromID(vecHistory[nNecromancerScreen]).SetActive(TRUE);
        nActivePage = nNecromancerScreen;

        UIHeroCreate::Console::StopVideo();

        UIID idVideo = UIIDCreateFromParent(vecHistory[nNecromancerScreen], ".ClassSkillVideo");

        if (UI_TestID(idVideo))
        {

            UICutscene & tSkillVideo = UI_GetCutsceneFromID(idVideo);
            tSkillVideo.SetLooping(TRUE);
            tSkillVideo.Play("Hero_Create_Necromancer.ogv", FALSE);
        }
    }

    ::Console::Screens::GetActionManager().ClearAllActions();
    ::Console::Screens::GetActionManager().SetAction(7, 0, StringListFetch("Console:Close").str());

    if (vecHistory.size() > 1)
    {
        ::Console::Screens::GetActionManager().SetAction(16, 1, StringListFetch("Console:Page").str());
    }

    ::Console::Screens::UpdateButtonLegendText();

    UIButton & tTab = UI_GetButtonFromID(UITEXT_CONSOLE_WHATS_NEW_SEASON_NAVIGATION);

    if (::Console::Online::IsSeasonsInitialized() && OnlineService::GetSeasonState() == OnlineService::SEASON_STATE_ACTIVE)
    {
        tTab.SetEnabled(TRUE);
    }
    else
    {
        tTab.SetEnabled(FALSE);
    }
}





BOOL IsVisible()
{

    return UI_GetWindowFromID(UIWND_CONSOLE_WHATS_NEW).IsActive();
}

void Close()
{

    if (nActiveWhatsNew != -1)
    {
        UI_GetWindowFromID(vecHistory[nActiveWhatsNew]).SetActive(TRUE); }

    UI_GetWindowFromID(UIWND_CONSOLE_WHATS_NEW_ANNIVERSARY).SetActive(FALSE);
    UI_GetWindowFromID(UIWND_CONSOLE_WHATS_NEW_NECROMANCER).SetActive(FALSE);

    if (nNecromancerScreen != -1)
    {
        UIID idVideo = UIIDCreateFromParent(vecHistory[nNecromancerScreen], ".ClassSkillVideo");
        UICutscene & tSkillVideo = UI_GetCutsceneFromID(idVideo);
        tSkillVideo.Stop();
    }

    UIHeroCreate::Console::RestartVideo();

    vecHistory.clear();

    if (IsVisible())
    {



        ::Console::Screens::CloseModalDialog(UIWND_CONSOLE_WHATS_NEW);

        BOOL bSave = FALSE;



        if (!bForceAnniversary && !bForceNecromancer)
        {

            if (Prefs::GetWhatsNewSeen() != LATEST_WHATS_NEW_VERSION)
            {
                Prefs::GetWhatsNewSeen() = LATEST_WHATS_NEW_VERSION;
                bSave = TRUE;
            }
        }
        else
        {





            if (Prefs::GetAnniversarySeen() != sGetAnniversaryYear())
            {
                Prefs::GetAnniversarySeen() = sGetAnniversaryYear();
                bSave = TRUE; } }
        if (bSave) { ::Console::GamerProfile::SavePrefs(); }


        UIStartScreen::Console::OnWhatsNewComplete(UI_GetPrimaryUIController());
    }
}
void OnButton2(const UIID &, const int32 nControllerIndex)
{


    if (UI_GetPrimaryUIController() == nControllerIndex)
    {



        if (!::Console::Screens::GetActionManager().IsActionDelayed(7))
        {



            SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_PANEL_GENERIC_CLOSE), cgtPSODefaults);
            Close(); } }
}

void OnDPadLeft(const UIID &, const int32 nControllerIndex)
{


    const int32 nNewIndex = Math::PinWrap(nActivePage - 1, 0, (int32)vecHistory.size());

    UI_GetWindowFromID(vecHistory[nActivePage]).SetActive(FALSE);
    nActivePage = nNewIndex;
    UI_GetWindowFromID(vecHistory[nActivePage]).SetActive(TRUE);


    if (nActivePage == nNecromancerScreen)
    {
        UIID idVideo = UIIDCreateFromParent(vecHistory[nActivePage], ".ClassSkillVideo");

        if (UI_TestID(idVideo))
        {

            if (UI_GetControlFromID(UIWND_CONSOLE_HEROCREATE).IsActive())
            {
                UIHeroCreate::Console::StopVideo();
            }

            UICutscene & tSkillVideo = UI_GetCutsceneFromID(idVideo);
            tSkillVideo.SetLooping(TRUE);
            tSkillVideo.Play("Hero_Create_Necromancer.ogv", FALSE);
        }
    }

    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_GENERIC_MENU_CLICK), cgtPSODefaults);
}

void OnDPadRight(const UIID &, const int32 nControllerIndex)
{


    const int32 nNewIndex = Math::PinWrap(nActivePage + 1, 0, (int32)vecHistory.size());

    UI_GetWindowFromID(vecHistory[nActivePage]).SetActive(FALSE);
    nActivePage = nNewIndex;
    UI_GetWindowFromID(vecHistory[nActivePage]).SetActive(TRUE);

    if (nActivePage == nNecromancerScreen)
    {
        UIID idVideo = UIIDCreateFromParent(vecHistory[nActivePage], ".ClassSkillVideo");

        if (UI_TestID(idVideo))
        {
            if (UI_GetControlFromID(UIWND_CONSOLE_HEROCREATE).IsActive())
            {
                UIHeroCreate::Console::StopVideo();
            }

            UICutscene & tSkillVideo = UI_GetCutsceneFromID(idVideo);
            tSkillVideo.SetLooping(TRUE);
            tSkillVideo.Play("Hero_Create_Necromancer.ogv", FALSE);
        }
    }

    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_GENERIC_MENU_CLICK), cgtPSODefaults);
}

inline void sSwitchToSeason(const UIID & id, const int32 nControllerIndex)
{


    if (::Console::Online::IsSeasonsInitialized() && OnlineService::GetSeasonState() == OnlineService::SEASON_STATE_ACTIVE)
    {
        bSwitchingToSeason = TRUE;
        Close();
        UIWhatsNewSeason::Console::OpenForController(nControllerIndex);
        bSwitchingToSeason = FALSE;
    }
}

void OnL1(const UIID & id, const int32 nControllerIndex)
{


    sSwitchToSeason(id, nControllerIndex);
}

void OnR1(const UIID & id, const int32 nControllerIndex)
{


    sSwitchToSeason(id, nControllerIndex);
}

void OnBuyNecromancer(const UIID & id, const int32 nControllerIndex)
{



    if (vecHistory[nActivePage] == UIWND_CONSOLE_WHATS_NEW_NECROMANCER)
    {
        UIMainMenu::Console::OpenStoreToNecromancer(nControllerIndex);
    }
}

BOOL ShouldShow()
{

    if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
    {
        return FALSE;
    }

    if (bSwitchingToSeason)
    {
        return FALSE;
    }

    return Prefs::GetWhatsNewSeen() < LATEST_WHATS_NEW_VERSION;
}

BOOL ShouldShowAnniversary()
{

    if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
    {
        return FALSE;
    }

    if (GameGetAnniversaryEventStatus() != ANNIVERSARY_EVENT_START)
    {
        return FALSE;
    }

    return Prefs::GetAnniversarySeen() < sGetAnniversaryYear();
}

BOOL ShouldShowNecromancer()
{

    if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
    {
        return FALSE;
    }

    if (!(::Console::DLCManager::GetLicenses() & (1 << LICENSE_NECROMANCER)))
    {
        return ShouldShow();
    }
    return FALSE;
}

}  // namespace Console
}  // namespace UIWhatsNew
