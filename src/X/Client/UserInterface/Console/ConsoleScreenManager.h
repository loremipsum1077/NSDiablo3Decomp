// X/Client/UserInterface/Console/ConsoleScreenManager.h -- provisional owning header.
//
// The 2.6.2 DWARF attributes Console::Screens::IsScreenSetOpen to
// X/Client/UserInterface/Console/ConsoleScreenManager.cpp:935. Only the entry point this
// file needs is declared here; the Screen/ScreenSet layouts are not adopted yet.

#pragma once

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Sigma/SigmaTypes.h"
#include "../../../../Contrib/Contrib/blz/include/blz/vector.h"

struct UIID;

class ConsoleModalNotificationManager;
namespace Console {
class ControllerActionManager;
namespace Screens {

BOOL IsScreenSetOpen();
// ConsoleScreenManager.h:64; ConsoleScreenManager.cpp:944.
typedef int32 ScreenSets;
enum { STASH_SCREEN_SET = 1 };
BOOL IsScreenSetOpen(const ScreenSets eScreenSet);
void CloseModalDialog(const UIID & idDialog);  // ConsoleScreenManager.cpp:1364
void ShowModalDialog(int32 nControllerIndex, const UIID & idDialog, BOOL bBlockInput, const UIID & idLightboxOverride);  // line 1334
void UpdateButtonLegendText();  // line 1768
ControllerActionManager & GetActionManager();  // line 1798
enum { LEADERBOARD_SCREEN_SET = 28 };
void CloseScreenSet(BOOL fClearScreenSetHistory, BOOL fSkipTransition);  // ConsoleScreenManager.cpp:999
BOOL IsScreenTransitioning();  // ConsoleScreenManager.cpp:1438
ConsoleModalNotificationManager & GetModalNotificationManager();  // line 1808

}  // namespace Screens
}  // namespace Console

// Added for ConsoleScreenManager.cpp: the screen and screen-set classes (ConsoleScreenManager.h:13-140).
typedef int32 SNO;  // X/Common/SNO/SNOFiles.h
namespace Console {
struct Screen {  // size 0x18
    typedef void (*OpenFunc)(int32);
    typedef void (*CloseFunc)();
    Console::Screen::OpenFunc fnOpen;  // 0x00
    Console::Screen::CloseFunc fnClose;  // 0x08
    BOOL bEnabled;  // 0x10
    Screen();
    Screen(Console::Screen::OpenFunc fnOpen, Console::Screen::CloseFunc fnClose) : fnOpen(fnOpen), fnClose(fnClose), bEnabled(TRUE) {}  // line 43
};
static_assert(sizeof(Screen) == 0x18, "Screen");

class ScreenSet {  // size 0x38
    typedef void (*OpenFunc)(int32);
    typedef void (*CloseFunc)(int32);
public:
    ScreenSet();
    ScreenSet(Console::ScreenSet::OpenFunc fnOpen, Console::ScreenSet::CloseFunc fnClose);
    void AddScreen(Console::Screen::OpenFunc fnOpen, Console::Screen::CloseFunc fnClose);
    void AddScreen() { AddScreen(NULL, NULL); }  // line 82
    void ClearScreens();
    void EnableScreen(const int32 nScreenIndex, const BOOL bEnable);
    BOOL IsScreenEnabled(const int32) const;
    void OpenToScreen(const int32 nScreenIndex, int32 nControllerOpening);
    void OpenNextScreen(int32 nControllerOpening);
    void OpenPreviousScreen(int32 nControllerOpening);
    int32 GetControllerIndex() const { return m_nControllerIndex; }  // line 108
    void Close();
    int32 GetCurrentScreen() const { return m_nCurrentScreen; }  // line 117
    int32 GetNumScreens() const { return (int32)m_arScreens.size(); }  // line 123
    SNO GetCloseSound() const { return m_snoCloseSound; }  // line 129
    void SetCloseSound(const SNO snoSound) { m_snoCloseSound = snoSound; }  // line 135
private:
    blz::vector<Console::Screen, blz::allocator<Console::Screen> > m_arScreens;  // 0x00
    int32 m_nCurrentScreen;  // 0x18
    int32 m_nControllerIndex;  // 0x1c
    SNO m_snoCloseSound;  // 0x20
    Console::ScreenSet::OpenFunc m_fnOpen;  // 0x28
    Console::ScreenSet::CloseFunc m_fnClose;  // 0x30
    void OpenCurrentScreen(int32 nControllerOpening);
};
static_assert(sizeof(ScreenSet) == 0x38, "ScreenSet");

}  // namespace Console

class ConsoleToastManager;
class FanfareManager;
class FrontendBackgroundCameraManager;
class FrontendBackgroundHeroManager;
class LightBoxManager;
struct CScene;
namespace Console {
namespace Screens {
typedef int32 ScreenID;
// STASH_SCREEN_SET and LEADERBOARD_SCREEN_SET are declared above; MAIN_MENU_SCREEN_SET is
// declared locally by UI.cpp (and ConsoleScreenManager.cpp), so it is left out here.
enum {
    INVALID_SCREEN_SET = -1,
    GAMEPLAY_SCREEN_SET = 0,
    SHOP_SCREEN_SET = 2,
    BLOODSHARD_VENDOR_SCREEN_SET = 3,
    KANAIS_CUBE_SCREEN_SET = 4,
    BLACKSMITH_SCREEN_SET = 5,
    JEWELER_SCREEN_SET = 6,
    MYSTIC_SCREEN_SET = 7,
    MAP_SCREEN_SET = 8,
    BANNER_SCREEN_SET = 9,
    ACHIEVEMENTS_SCREEN_SET = 10,
    MAIL_SCREEN_SET = 11,
    MAIL_ITEM_SELECT_SCREEN_SET = 12,
    RIFT_ENTRY_SCREEN_SET = 13,
    RIFT_JOIN_SCREEN_SET = 14,
    RIFT_COMPLETE_SCREEN_SET = 15,
    JEWEL_UPGRADE_SCREEN_SET = 16,
    SEASON_JOURNEY_SCREEN_SET = 17,
    EQUIPMENT_MANAGER_SCREEN_SET = 18,
    EQUIPMENT_MANAGER_DETAILS_SCREEN_SET = 19,
    OPTIONS_SCREEN_SET = 21,
    HERO_CREATE_SCREEN_SET = 22,
    HERO_SELECT_SCREEN_SET = 23,
    CHARACTER_DETAILS_SCREEN_SET = 24,
    CINEMATICS_SCREEN_SET = 25,
    TRADE_SCREEN_SET = 26,
    TRADE_ITEM_SELECT_SCREEN_SET = 27,
    TRIAL_END_SCREEN_SET = 29,
    DESIGNER_MENU_SCREEN_SET = 30,
    NUM_SCREEN_SETS = 31,
};

BOOL IsScreenManagerValid();
void InitializeForApp();
void DisposeForApp();
void InitializeForGame();
void DisposeForGame();
void InitializeForWorld();
void DisposeForWorld();
BOOL IsScreenOpen(const ScreenSets eScreenSet, const ScreenID idScreen);
void CloseAllScreenSets(BOOL bSkipTransitions);
void TryOpenToScreen(const ScreenSets eScreenSet, const ScreenID idScreen, int32 nControllerOpening, BOOL fSkipTransition);
void OpenNextScreen(int32 nControllerOpening, BOOL fSkipTransition);
void OpenPreviousScreen(int32 nControllerOpening, BOOL fSkipTransition);
int32 GetCurrentControllerIndex();
void EnableScreen(const ScreenSets eScreenSet, const ScreenID idScreen, const BOOL bEnable);
void CloseAllModalDialogs();
BOOL IsModalDialogVisible();
FrontendBackgroundCameraManager & GetFrontendBackgroundCameraManager();
FrontendBackgroundHeroManager & GetFrontendBackgroundHeroManager();
LightBoxManager & GetLightBoxManager();
void Update();
void UpdateBackgroundLightBeamActor(CScene * ptScene);
void GoToPreviousScreenSet(int32 nControllerOpening, BOOL fSkipTransition);
void ClearScreenSetHistory();
void ShowButtonLegend(const BOOL bShow);
ConsoleToastManager & GetToastManager();
FanfareManager & GetFanfareManager();

}  // namespace Screens
}  // namespace Console
