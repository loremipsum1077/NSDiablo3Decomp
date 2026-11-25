// Original file: X/Client/UserInterface/Console/ConsoleScreenManager.cpp
// The console screen manager: screen sets, transitions, modal dialogs and the shared managers.
// Generated from build/try/r5-screenmgr/frag.cpp by gen.py, which pads each function onto
// its original line.

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "ConsoleScreenManager.h"
#include "ConsoleModalNotificationManager.h"
#include "ConsoleToastManager.h"
#include "ControllerActions.h"
#include "../../Battlenet/Hub.h"
#include "../../Battlenet/Subsystems/FanfareManager.h"
#include "../../Battlenet/Subsystems/FrontendBackgroundCameraManager.h"
#include "../../Battlenet/Subsystems/FrontendBackgroundHeroManager.h"
#include "../../Battlenet/Subsystems/FrontendScreenTransitionManager.h"
#include "../../Battlenet/Subsystems/LightBoxManager.h"
#include "../../Battlenet/Subsystems/ModalNotificationManager.h"
#include "../Controls/UITextControl.h"
#include "../UI.h"
#include "../UIID.h"
#include "../UIIDs.h"
#include "../../CGame.h"
#include "../../Sound/Sound.h"
#include "../../../Common/ActorCommonData.h"
#include "../../../Common/Game.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/Player.h"
#include "../../../Common/Shell/CmdLine.h"
#include "../../Gfx/GfxVB.h"
#include "../../../Sigma/LanguageRules.h"
#include "../../../Sigma/DataStructures/XGrowableList.h"
#include "UIConsoleNewItemIndicators.h"
#include "../../../Common/Crafting/ItemCraftingSwig.h"
#include "../../../Common/Networking/GameMessage.h"
#include "../../Console/GamerProfile.h"
#include "../../Console/Online.h"
#include "UIConsoleGameSettings.h"
#include "../Interface/UILoreCommon.h"
#include "ConsoleUIRActorManager.h"
#include "../../Scene.h"
#include "../../Attachments.h"

// Declared here because their headers are not adopted yet. Each comment names the file that
// defines the function.
// X/Client/Battlenet/Hub.cpp
Hub & HubGet();
// X/Client/Battlenet/Subsystems/ScreenManager.cpp
#include "../../Battlenet/Subsystems/ScreenManager.h"




// X/Client/CGame.cpp
uint32 CGameGetPredictedServerTime();
// X/Client/Network/CGameMessage.cpp
void MessageSendToServer(GameMessageType eType, void * pMessage, uint32 dwMessageSize, const Player * ptPlayer);
// X/Common/ActorCommonData.cpp
BOOL ACDIsInTown(const ActorCommonData & tACD);
// X/Client/UserInterface/Console/UIConsoleStash.cpp
namespace UIStash { namespace Console { void OpenForController(int32 nControllerIndex); void Close(); } }
// X/Client/UserInterface/Console/UIConsoleEquipmentManagerDetails.cpp
namespace UIEquipmentManagerDetails { void OpenForController(int32 nControllerIndex); void Close(); }
// X/Client/UserInterface/Console/UIConsoleMail.cpp
namespace UIMail { namespace Console { void OpenForController(int32 nControllerIndex); void Close(); } }
// X/Client/UserInterface/Console/UIConsoleRiftEntry.cpp
namespace UIConsoleRiftEntry { void OpenForController(int32 nControllerIndex); void Close(); }
// X/Client/UserInterface/Console/UIConsoleJoinRift.cpp
namespace UIJoinRift { void Close(); }
// X/Client/UserInterface/Console/UIConsoleRiftComplete.cpp
namespace UIRiftComplete { void Close(); }
// X/Client/UserInterface/Console/UIConsoleJewelUpgrade.cpp
namespace UIJewelUpgrade { void Close(); }
// X/Client/UserInterface/Console/UIConsoleShop.cpp
namespace UIShop { namespace Console { void OpenForController(int32 nControllerIndex); void Close(); } }
// X/Client/UserInterface/Console/UIConsoleBloodShardVendor.cpp
namespace UIBloodShardVendor { namespace Console { void OpenForController(int32 nControllerIndex); void Close(); } }
// X/Client/UserInterface/Console/UIConsoleJeweler.cpp
namespace UIJeweler { namespace Console { void OpenForController(int32 nControllerIndex); void Close(); } }
// X/Client/UserInterface/Console/UIConsoleBlacksmith.cpp
namespace UIBlacksmith { namespace Console { void OpenForController(int32 nControllerIndex); void Close(); } }
// X/Client/UserInterface/Console/UIConsoleMystic.cpp
namespace UIMystic { namespace Console { void OpenForController(int32 nControllerIndex); void Close(); } }
// X/Client/UserInterface/Console/UIConsoleHallOfFallenHeroes.cpp
namespace UIHallOfFallenHeroes { namespace Console { void InitializeForWorld(); void DisposeForWorld(); } }
// X/Client/UserInterface/Console/UIConsoleRiftEndScreen.cpp
namespace UIConsoleRiftEndScreen { void InitializeForWorld(); }

namespace Console { namespace Screens { enum { MAIN_MENU_SCREEN_SET = 20 }; } }
namespace Console { namespace Screens {
BOOL OpenToScreen(const ScreenSets eScreenSet, const ScreenID idScreen, int32 nControllerOpening, BOOL fSkipTransition);
BOOL ScreenHasPriority(const ScreenSets eScreenSet, const ScreenID idScreen);
} }
// X/Common/Networking/GameMessage.h
enum {
    CMSG_CLOSE_INTERACTION = 22,


};
// X/Client/Battlenet/Subsystems/ToastManager.cpp
extern const ToastId INVALID_TOAST_ID;
namespace Console { namespace Notifications {
void EnableHostMigrationLoadscreenTip(BOOL bFlag);
void ShowLeaveGameDialog(int32 nControllerIndex, BOOL fShow, uint32 uMinTickDuration, uint32 uMaxTickDuration, uint32 uKickTickDuration);
BOOL HostMigrationLoadscreenTipEnabled();
void ShowAmiiboTutorial(int32 nControllerIndex);
void OnAmiiboTutorialContinueButton(const UIID & id, const int32 nControllerIndex);
} }




namespace {

struct ScreenHistoryNode {  // size 0x10
    Console::ScreenSet * ptScreenSet;  // 0x00
    int32 nScreenIndex;  // 0x08
};
BOOL s_bShowMigrationTip;



uint32 s_dwOnlineStateChangingStart;
ToastId s_idStateChangingToast = INVALID_TOAST_ID;




struct LogoffCountDownData {  // size 0x10
    uint32 m_uCountDownStartTime;  // 0x00
    uint32 m_uMinCountDownDuration;  // 0x04
    uint32 m_uMaxCountDownDuration;  // 0x08
    uint32 m_uKickCountDownDuration;  // 0x0c
    LogoffCountDownData()
    : m_uCountDownStartTime( 0 )
    , m_uMinCountDownDuration( 0 )
    , m_uMaxCountDownDuration( 0 )
    , m_uKickCountDownDuration( 0 )
    {
    }
};

}  // namespace


































static FrontendScreenTransitionManager::ScreenKey sMakeScreenKey( Console::Screens::ScreenSets eScreenSet, Console::Screens::ScreenID eScreen )
{
    FrontendScreenTransitionManager::ScreenKey eKey = ( eScreenSet << 16 ) | eScreen;
    return eKey;
}








static Console::Screens::ScreenSets sGetScreenSet( FrontendScreenTransitionManager::ScreenKey eKey )
{
    int32 nVal = ( eKey >> 16 ) & 0xFFFF;
    return nVal;
}






static Console::Screens::ScreenID sGetScreen( FrontendScreenTransitionManager::ScreenKey eKey )
{
    int32 nVal = eKey & 0xFFFF;
    return nVal;
}





struct ModalDialogInfo {  // size 0x20
    UIID m_idControllerFocusBeforeModal;  // 0x00
    UIID m_idModalDialog;  // 0x08
    UIID m_idModalLightBox;  // 0x10
    BOOL m_bBlockInput;  // 0x18
};




#include "../../../../Contrib/Contrib/blz/include/blz/algo_seq_const.h"
struct ConsoleScreenManagerData {  // size 0x1020

    ConsoleScreenManagerData()
    : m_ptCurrentScreenSet( NULL )
    , m_idLeaveGameDialog( INVALID_MODAL_NOTIFICATION_ID )
    {
        SigmaMemoryClear( m_parScreenSets, sizeof(m_parScreenSets) );
    }
    Console::ScreenSet * m_parScreenSets[Console::Screens::NUM_SCREEN_SETS];  // 0x0
    Console::ScreenSet * m_ptCurrentScreenSet;  // 0x108
    Console::ControllerActionManager m_tActionManager;  // 0x110
    FrontendBackgroundCameraManager m_tFrontendBackgroundCameraManager;  // 0x6d8
    FrontendBackgroundHeroManager m_tFrontendBackgroundHeroManager;  // 0x6e8
    FrontendScreenTransitionManager m_tScreenTransitionManager;  // 0x9b8
    LightBoxManager m_tLightboxManager;  // 0xa60
    ConsoleModalNotificationManager m_tModalManager;  // 0xad0
    ConsoleToastManager m_tToastManager;  // 0xb40
    FanfareManager m_tFanfareManager;  // 0xd18
    XGrowableList<ModalDialogInfo, const ModalDialogInfo &> m_listModalDialogInfoStack;  // 0xf10
    XGrowableList<ScreenHistoryNode, const ScreenHistoryNode &> m_listScreenStack;  // 0xf68
    LogoffCountDownData m_atLogoffCountDownData[4];  // 0xfc0
    ModalNotificationId m_idLeaveGameDialog;  // 0x1000
    blz::vector<int32, blz::allocator<int32> > unk_1008;  // 0x1008 (2.7.7: used by sub_710016a990..sub_710016ab00)
};
static_assert(sizeof(ConsoleScreenManagerData) == 0x1020, "ConsoleScreenManagerData");

ConsoleScreenManagerData * gptScreenManagerData;

// X/Common/Definitions/StringListDefinition.cpp
CRefString StringListFetch(LPCSTR szLabel);
CRefString StringListFetch(const SNO snoStringList, LPCSTR szLabel);
// X/Client/UserInterface/Console/UIConsoleHeroCreate.cpp, UIConsoleHeroSelect.cpp
namespace UIHeroCreate { namespace Console { void OpenForController(int32 nControllerIndex); void Close(); void CloseForHostMigration(); } }
namespace UIHeroSelect { namespace Console { void OpenForController(int32 nControllerIndex); void Close(); void CloseForHostMigration(); } }
// X/Client/UserInterface/Console/UIConsoleStartScreen.cpp, UIConsoleMainMenu.cpp, UIConsolePause.cpp, UIConsoleOptions.cpp
namespace UIStartScreen { namespace Console { void OpenForController(int32); void Close(); } }
namespace UIMainMenu { namespace Console { void OpenForController(int32 nControllerIndex); void Close(); } }
namespace UIPause { namespace Console { void OpenForController(int32 nControllerIndex); void Close(); } }
namespace UIOptions { namespace Console { void OpenForController(int32 nControllerIndex); void Close(); } }
// X/Client/UserInterface/Console/OnlineActions.cpp
namespace Console { namespace UIOnlineActions { void ShowPartySwitchOnlineResultMessage(::Console::Online::PartySwitchOnlineResult eResult, int32 nControllerIndex, const char * szMessage); } }
// X/Client/UserInterface/Console/UIConsoleNPC.cpp
namespace UINPC { namespace Console { BOOL InteractMenuIsActive(); void InteractMenuDisable(int32 nControllerIndex); } }
// X/Client/UserInterface/UI.cpp
void UI_SetPrimaryUIController(int32 nControllerIndex);
void sShowGameUI(const BOOL bShow);
const UIID & UI_GetControllerFocus();
// X/Client/Attachments.cpp
Attachment * AttachmentFind(XPooledDataIDList & tAttachmentList, const uint32 dwID);
void UI_DeleteControl(const UIID & idControl, const BOOL fAssertIfDeletingRoot);
// X/Client/CPlayer.cpp
BOOL CPlayerIsValid(const Player * ptPlayer);
// X/Client/UserInterface/Interface/UIConversation.cpp, UIRewards.cpp
namespace UIConversation { BOOL IsUIBlockingConversationPlaying(); }
namespace UIRewards { BOOL IsAnyVisible(); }
// ConsoleScreenManager.h:328-345
namespace Console { namespace Screens { enum { PAUSE_SCREEN = 0, OPTIONS_SCREEN = 1 }; enum { RIFT_JOIN_SCREEN = 0 }; } }
namespace Console { namespace Screens { enum { START_SCREEN = 0, MAIN_MENU_SCREEN = 1 }; enum { HERO_CREATE_SCREEN = 0 }; enum { HERO_SELECT_SCREEN = 0 }; } }
extern UIID UIWND_CONSOLE_HEROCREATE;
extern UIID UIWND_CONSOLE_HERO_SELECT;
extern UIID UIDLG_CONSOLE_START_BUTTON;
extern UIID UIWND_PAUSE_CONSOLE;
extern UIID UIWND_CONSOLE_OPTIONS;
inline void sCleanUpScreenSet(Console::Screens::ScreenSets nScreenSet);
void sOnPreviousScreenSetTransitionOut(int32 nControllerOpening, BOOL bTransitioningIn, FrontendScreenTransitionManager::ScreenKey eNextScreenKey);
// UIIDs.h (generated in the original)
extern UIID UIWND_CONSOLE_HUD;  // 0x71017d5850
extern UIID UIDLG_MINIMAP_ROOT;  // 0x71017d55c8
extern UIID UISTACK_OBJHUD_MAINQUEST_REGION;  // 0x71017d91e0
extern UIID UISTACK_OBJHUD_SIDEQUEST_REGION;  // 0x71017d91e8
extern UIID UIHOTKEY_CONSOLE_PREV_SCREEN;  // 0x71017d58a0
extern UIID UIHOTKEY_CONSOLE_NEXT_SCREEN;  // 0x71017d58a8
extern UIID UIWND_CONSOLE_AMIIBOTUTORIAL;
namespace { void sUpdateLeaveGameDialog(); void sOnPartyJoinedByInvite( Console::Online::JoinResult eResult ); }
// X/Client/Console/Online.cpp, SaveMigration.cpp, GamerProfileNX64.cpp, OnlineActions.cpp
namespace Console { namespace Online { BOOL HasOutstandingInvite(int32 & nUserIdx); BOOL IsInviteToCurrentParty(); void ClearOutstandingInvite(); BOOL PartyIsOnlineStateChanging(PartyOnlineState & ePendingState); } }
namespace Console { namespace SaveMigration { BOOL IsInProgress(); } }
namespace Console { namespace GamerProfile { namespace Internal { void HandleUserSignedOut(int32 nUserIdx); } } }
namespace Console { namespace UIOnlineActions { BOOL CanLocalPlayerJoin(); void LocalPlayerJoin(int32 nUserIdx, BOOL bFromFrontEnd); void JoinPartyByInvite(Online::PartyJoinCallback fnCallback); } }
// X/Client/UserInterface/Console/UIConsolePartyDisplay.cpp, UIConsoleMainMenu.cpp
namespace UIPartyDisplay { namespace Console { void RefreshPartyDisplay(); } }
namespace UIMainMenu { namespace Console { void Refresh3DBackgroundHeroes(); } }
// X/Client/Shell/Shell.cpp
BOOL ShellIsWindowMinimized();
// X/Common/Networking/GameMessage.h (also declared locally by X/Client/CPlayer.cpp)




// Screen open/close functions of the console UI files (UIConsole*.cpp); their headers are not adopted yet.
namespace UIInventory { namespace Console { void OpenForController(int32 nControllerIndex); void Close(); void OpenTradeSelectForController(int32 nControllerIndex); } }
namespace UISkills { namespace Console { void OpenForController(int32 nControllerIndex); void Close(); } }
namespace UIParagonBonuses { namespace Console { void OpenForController(int32 nControllerIndex); void Close(); } }
namespace UIFollower { namespace Console { void OpenForController(int32 nControllerIndex); void Close(); } }
namespace UIParty { namespace Console { void OpenForController(int32 nControllerIndex); void Close(); } }
namespace UILore { namespace Console { void OpenForController(int32 nControllerIndex); void Close(); } }
namespace UIEquipmentManager { namespace Console { void OpenForController(int32 nControllerIndex); void Close(); } }
namespace UIEquipmentManagerDetails { void OpenInventory(int32 nControllerIndex); void OpenSkills(int32 nControllerIndex); }
namespace UISeasonStash { namespace Console { void OpenForController(int32 nControllerIndex); void Close(); } }
namespace UIRiftComplete { void OpenForController(int32 nControllerIndex); }
namespace UIJewelUpgrade { void OpenForController(int32 nControllerIndex); }
namespace UIShop { namespace Console { void OpenBuyScreen(int32 nControllerIndex); void OpenSellScreen(int32 nControllerIndex); void OpenRepairScreen(int32 nControllerIndex); void OpenBuybackScreen(int32 nControllerIndex); } }
namespace UITransmute { void OpenForController(int32 nControllerIndex); void Close(); }
namespace UIEquipLegendaryPowers { void OpenForController(int32 nControllerIndex); void Close(); }
namespace UIJeweler { namespace Console { void OpenCombineScreen(int32 nControllerIndex); void OpenSocketScreen(int32 nControllerIndex); void CloseSocketScreen(); void OpenRemoveScreen(int32 nControllerIndex); void CloseRemoveScreen(); void OpenTrainScreen(int32 nControllerIndex); } }
namespace UIBlacksmith { namespace Console { void OpenWeaponsScreen(int32 nControllerIndex); void OpenArmorScreen(int32 nControllerIndex); void OpenSalvageScreen(int32 nControllerIndex); void OpenRepairScreen(int32 nControllerIndex); void OpenTrainScreen(int32 nControllerIndex); } }
namespace UIMystic { namespace Console { void OpenEnchantScreen(int32 nControllerIndex); void OpenTransmogrifyScreen(int32 nControllerIndex); void OpenDyeItemScreen(int32 nControllerIndex); void OpenTrainScreen(int32 nControllerIndex); } }
namespace UIWaypointMap { void OpenWaypointMap(int32 nControllerIndex); void CloseWaypointMap(int32 nControllerIndex); void OpenActMap(int32 nControllerIndex); void CloseActMap(); void OpenWorldMap(int32 nControllerIndex); void CloseWorldMap(); }
namespace UIMap { void OpenForController(int32 nControllerIndex); void Close(); }
namespace UISeasonJourney { namespace Console { void OpenForController(int32 nControllerIndex); void Close(); } }
namespace UIBannerCustomization { namespace Console { void OpenForController(int32 nControllerIndex); void Close(); } }
namespace UIAchievements { namespace Console { void OpenForController(int32 nControllerIndex); void Close(); } }
namespace UICharacterDetails { namespace Console { void OpenForController(int32 nControllerIndex); void Close(); } }
namespace UICinematics { namespace Console { void OpenForController(int32 nControllerIndex); void Close(); } }
namespace UIConsoleDesignerMenu { void Open(int32 nControllerIndex); void Close(); }
namespace UITrade { namespace Console { void OpenForController(int32 nControllerIndex); void Close(); } }
namespace UILeaderboards { namespace Console { void OpenForController(int32 nControllerIndex); void Close(); } }
namespace UIMainMenu { namespace Console { void ResetMainMenuFlag(); } }
namespace Console { namespace Notifications { void CreateAmiiboTutorial(); } }
inline void sInitGameplayScreenSet(); inline void sInitStashScreenSet(); inline void sInitEquipmentManagerScreenSet(); inline void sInitMailScreenSets();
inline void sInitRiftEntryScreenSet(); inline void sInitRiftJoinScreenSet(); inline void sInitRiftCompleteScreenSet(); inline void sInitJewelUpgradeScreenSet();
inline void sInitShopScreenSet(); inline void sInitBloodShardVendorScreenSet(); inline void sInitKanaisCubeScreenSet(); inline void sInitJewelerScreenSet();
inline void sInitBlacksmithScreenSet(); inline void sInitMysticScreenSet(); inline void sInitMapScreenSet(); inline void sInitSeasonJourneyScreenSet();
void sInitMainMenuScreenSet(); void sInitHeroCreateScreenSet(); void sInitHeroSelectScreenSet(); void sInitOptionsScreenSet();
inline void sInitBannerScreenSet(); inline void sInitAchievementsScreenSet(); inline void sInitCharacterDetailsScreenSet(); inline void sInitCinematicsScreenSet();
// 2.7.7 screen sets 31/32 (unnamed open/close callbacks).
extern "C" void sub_7100178a10(int32 nControllerIndex);
extern "C" void sub_71001794e0(int32 nControllerIndex);
extern "C" void sub_7100370440(int32 nControllerIndex);
static void sub_710016cae0(int32 nControllerIndex) { UIInventory::Console::Close(); }
inline void sInitDesignerMenuScreenSet(); inline void sInitTradeScreenSet(); inline void sInitTradeItemSelectScreenSet(); inline void sInitLeaderboardScreenSet();

Console::Screens::ScreenSets sGetScreenSet( const Console::ScreenSet * ptScreenSet )
{
    for( int32 i = 0; i < Console::Screens::NUM_SCREEN_SETS; ++i )
    {
        if( gptScreenManagerData->m_parScreenSets[i] == ptScreenSet )
        {
            return i;
        }
    }
    return Console::Screens::INVALID_SCREEN_SET;
}









inline Console::ScreenSet::ScreenSet()
: m_arScreens()
, m_nCurrentScreen( -1 )
, m_nControllerIndex( -1 )
, m_snoCloseSound( -1 )
, m_fnOpen( NULL )
, m_fnClose( NULL )
{
}




inline Console::ScreenSet::ScreenSet( OpenFunc fnOpen, CloseFunc fnClose )
: m_arScreens()
, m_nCurrentScreen( -1 )
, m_nControllerIndex( -1 )
, m_snoCloseSound( -1 )
, m_fnOpen( fnOpen )
, m_fnClose( fnClose )
{
}





































void Console::ScreenSet::AddScreen( Screen::OpenFunc fnOpen, Screen::CloseFunc fnClose )
{
    Screen tScreen( fnOpen, fnClose );
    m_arScreens.push_back( tScreen );
}
















inline void Console::ScreenSet::EnableScreen( const int32 nScreenIndex, const BOOL bEnable )
{
    m_arScreens[nScreenIndex].bEnabled = bEnable;
}

















inline void Console::ScreenSet::OpenToScreen( const int32 nScreenIndex, int32 nControllerOpening )
{
    if( m_nCurrentScreen != nScreenIndex )
    {
        if( m_nCurrentScreen == -1 )
        {
            UI_SetPrimaryUIController( nControllerOpening );
            if( m_fnOpen )
            {
                m_fnOpen( nControllerOpening );
            }
        }
        else
        {
            if( m_arScreens[m_nCurrentScreen].fnClose )
            {
                m_arScreens[m_nCurrentScreen].fnClose();
            }
        }

        m_nCurrentScreen = nScreenIndex;
        m_nControllerIndex = nControllerOpening;

        OpenCurrentScreen( nControllerOpening );
    }
}








inline void Console::ScreenSet::OpenNextScreen( int32 nControllerOpening )
{
    int32 nNumScreens = GetNumScreens();

    if( nNumScreens > 1 )
    {
        int32 nNextScreen = m_nCurrentScreen;
        do
        {
            nNextScreen = Math::PinWrap( nNextScreen + 1, 0, nNumScreens );
        } while( nNextScreen != m_nCurrentScreen && !m_arScreens[nNextScreen].bEnabled );

        if( nNextScreen != m_nCurrentScreen )
        {
            if( m_nCurrentScreen != -1 && m_arScreens[m_nCurrentScreen].fnClose )
            {
                m_arScreens[m_nCurrentScreen].fnClose();
            }

            m_nCurrentScreen = nNextScreen;
            OpenCurrentScreen( nControllerOpening );
        }
    }
}









inline void Console::ScreenSet::OpenPreviousScreen( int32 nControllerOpening )
{
    int32 nNumScreens = GetNumScreens();

    if( nNumScreens > 1 )
    {
        int32 nPrevScreen = m_nCurrentScreen;
        do
        {
            nPrevScreen = Math::PinWrap( nPrevScreen - 1, 0, nNumScreens );
        } while( nPrevScreen != m_nCurrentScreen && !m_arScreens[nPrevScreen].bEnabled );

        if( nPrevScreen != m_nCurrentScreen )
        {
            if( m_nCurrentScreen != -1 && m_arScreens[m_nCurrentScreen].fnClose )
            {
                m_arScreens[m_nCurrentScreen].fnClose();
            }

            m_nCurrentScreen = nPrevScreen;
            OpenCurrentScreen( nControllerOpening );
        }
    }
}









inline void Console::ScreenSet::Close()
{
    if( m_nCurrentScreen != -1 )
    {
        if( m_arScreens[m_nCurrentScreen].fnClose )
        {
            m_arScreens[m_nCurrentScreen].fnClose();
        }

        int32 nContollerIndex = m_nControllerIndex;
        m_nCurrentScreen = -1;
        m_nControllerIndex = -1;

        if( m_fnClose )
        {
            m_fnClose( nContollerIndex );
        }
        UI_SetPrimaryUIController( -1 );
    }
}





void Console::ScreenSet::OpenCurrentScreen( int32 nControllerOpening )
{
    if( m_nCurrentScreen != -1 )
    {
        UI_SetPrimaryUIController( nControllerOpening );
        m_nControllerIndex = nControllerOpening;

        if( m_arScreens[m_nCurrentScreen].fnOpen )
        {
            gptScreenManagerData->m_tActionManager.ClearAllActions();
            gptScreenManagerData->m_tActionManager.SetActionsDirtyFlag( TRUE );

            m_arScreens[m_nCurrentScreen].fnOpen( nControllerOpening );

            Console::Screens::UpdateButtonLegendText();
        }
    }
}






void sOnHostMigration( Console::Online::MigrationEvent eEvent )
{
    switch( eEvent )
    {
    case Console::Online::MIGRATION_STARTED:
        if( !GameIsStartUp() )
        {
            Console::Notifications::EnableHostMigrationLoadscreenTip( TRUE );

            Console::Screens::GetModalNotificationManager().RemoveAllNotifications();

            if( ( Console::Screens::IsScreenSetOpen( Console::Screens::HERO_CREATE_SCREEN_SET ) ||
                  Console::Screens::IsScreenSetOpen( Console::Screens::HERO_SELECT_SCREEN_SET ) ) &&
                !GameIsStartUp() )
            {
                UIHeroCreate::Console::CloseForHostMigration();

                UIHeroSelect::Console::CloseForHostMigration();
            }

            Console::Screens::CloseScreenSet( TRUE, TRUE );
        }
        break;

    case Console::Online::MIGRATION_NOT_POSSIBLE:
        if( gptScreenManagerData &&
            Console::GamerProfile::GetPrimaryProfileUserIndex() != -1 )
        {
            if( Console::Online::PartyGetOnlineState() == Console::Online::PARTY_ADHOC )
            {
                Console::Screens::GetToastManager().SendToastConsole( StringListFetch( GlobalSNOGet( GLOBALSNO_STRINGLIST_CONSOLE_ERRORS ), "GameMigrationErrorFromAdHoc" ), 0 );
            }
            else
            {
                Console::Screens::GetToastManager().SendToastConsole( StringListFetch( GlobalSNOGet( GLOBALSNO_STRINGLIST_CONSOLE_ERRORS ), "GameMigrationError" ), 0 );
            }
        }
        break;
    }
}












void sOnPSNSignInChanged( int32 nUserIndex, Console::Online::PSNSignInStatus eStatus )
{
    if( Console::GamerProfile::IsSignedIn( nUserIndex ) && eStatus == Console::Online::PSN_NOT_SIGNED_IN )
    {
        if( gptScreenManagerData )
        {
            CRefString sMessage = StringListFetch( "Console:PS3OfflineMessage" );
            Console::Screens::GetToastManager().SendToastConsole( sMessage, 0 );
        }
    }
}








void sOnLobbyConnectionEvent( int32 nUserIndex, Console::Online::LobbyConnectionEventType eStatus )
{
    if( Console::GamerProfile::IsSignedIn( nUserIndex ) && eStatus == Console::Online::LOBBY_EVENT_DISCONNECTED )
    {
        if( gptScreenManagerData )
        {
            CRefString sMessage = StringListFetch( "ConsoleErrors:LeaderboardDisconnectMessage" );
            Console::Screens::GetToastManager().SendToastConsole( sMessage, 0 );
        }
    }
}








void sOnEventPartyOnlineStateChanged( Console::Online::PartySwitchOnlineResult eResult )
{
    if( !Console::Online::PartyIsOnline() )
    {
        UIGameSettings::Console::Close();
    }

    if( gptScreenManagerData )
    {
        Console::UIOnlineActions::ShowPartySwitchOnlineResultMessage( eResult, -1, NULL );
    }
}






void sOnEventPhysicalNetworkStatusChanged( Console::Online::PhysicalNetworkStatus eStatus )
{
    if( eStatus == Console::Online::PHYSICAL_NETWORK_STATUS_NOT_CONNECTED && gptScreenManagerData )
    {
        CRefString sMessage = StringListFetch( "ConsoleErrors:EthernetLoss" );
        Console::Screens::GetToastManager().SendToastConsole( sMessage, 0 );
    }
}




BOOL Console::Screens::IsScreenManagerValid()
{
    return gptScreenManagerData != NULL;
}




void Console::Screens::InitializeForApp()
{
    Online::GetPartyMigratingEvent().Subscribe( OnlineService::internal::function_bind_0_1<void, Online::MigrationEvent, &sOnHostMigration>() );
    Online::GetPartyOnlineStateChangedEvent().Subscribe( OnlineService::internal::function_bind_0_1<void, Online::PartySwitchOnlineResult, &sOnEventPartyOnlineStateChanged>() );
    Online::GetPSNSignInChangedEvent().Subscribe( OnlineService::internal::function_bind_0_2<void, int32, Online::PSNSignInStatus, &sOnPSNSignInChanged>() );
    Online::GetPhysicalNetworkStatusChangedEvent().Subscribe( OnlineService::internal::function_bind_0_1<void, Online::PhysicalNetworkStatus, &sOnEventPhysicalNetworkStatusChanged>() );
    Online::GetLobbyConnectionEvent().Subscribe( OnlineService::internal::function_bind_0_2<void, int32, Online::LobbyConnectionEventType, &sOnLobbyConnectionEvent>() );

    gptScreenManagerData = new ConsoleScreenManagerData;
}




void Console::Screens::DisposeForApp()
{
    Online::GetPartyMigratingEvent().Unsubscribe( OnlineService::internal::function_bind_0_1<void, Online::MigrationEvent, &sOnHostMigration>() );
    Online::GetPartyOnlineStateChangedEvent().Unsubscribe( OnlineService::internal::function_bind_0_1<void, Online::PartySwitchOnlineResult, &sOnEventPartyOnlineStateChanged>() );
    Online::GetPSNSignInChangedEvent().Unsubscribe( OnlineService::internal::function_bind_0_2<void, int32, Online::PSNSignInStatus, &sOnPSNSignInChanged>() );
    Online::GetPhysicalNetworkStatusChangedEvent().Unsubscribe( OnlineService::internal::function_bind_0_1<void, Online::PhysicalNetworkStatus, &sOnEventPhysicalNetworkStatusChanged>() );
    Online::GetLobbyConnectionEvent().Unsubscribe( OnlineService::internal::function_bind_0_2<void, int32, Online::LobbyConnectionEventType, &sOnLobbyConnectionEvent>() );

    delete gptScreenManagerData;
    gptScreenManagerData = NULL;
}


void OnPartyMemberAddedEvent( int32 nPartyIndex )
{
    Console::Online::PartyMember atMembers[4];
    uint32 uNumPartyMembers = Console::Online::PartyGetMembers( atMembers, 0, FALSE );
    int32 nPackedPartyIndex = -1;
    for( uint32 i = 0; i < uNumPartyMembers; ++i )
    {
        if( atMembers[i].idPlayer == (uint8)nPartyIndex )
        {
            nPackedPartyIndex = i;
            break;
        }
    }

    if( nPackedPartyIndex != -1 && !atMembers[nPackedPartyIndex].bIsLocal )
    {
        CRefString sMessage = StringListFetch( "ConsoleErrors:PartyMemberJoined" );
        Console::Screens::GetToastManager().SendToastConsole( sMessage, 0 );
    }
}




void Console::Screens::InitializeForGame()
{

    sInitGameplayScreenSet();
    sInitStashScreenSet();
    sInitEquipmentManagerScreenSet();
    sInitMailScreenSets();
    sInitRiftEntryScreenSet();
    sInitRiftJoinScreenSet();
    sInitRiftCompleteScreenSet();
    sInitJewelUpgradeScreenSet();
    sInitShopScreenSet();
    sInitBloodShardVendorScreenSet();
    sInitKanaisCubeScreenSet();
    sInitJewelerScreenSet();
    sInitBlacksmithScreenSet();
    sInitMysticScreenSet();
    sInitMapScreenSet();
    sInitSeasonJourneyScreenSet();
    sInitMainMenuScreenSet();
    sInitHeroCreateScreenSet();
    sInitHeroSelectScreenSet();
    sInitBannerScreenSet();
    sInitAchievementsScreenSet();
    sInitOptionsScreenSet();
    sInitCharacterDetailsScreenSet();
    sInitCinematicsScreenSet();
    sInitDesignerMenuScreenSet();
    sInitTradeScreenSet();
    sInitTradeItemSelectScreenSet();

    sInitLeaderboardScreenSet();

    {
        Console::ScreenSet * ptScreenSet = new Console::ScreenSet( sub_7100178a10, sub_71001794e0 );
        gptScreenManagerData->m_parScreenSets[Console::Screens::UNK_31_SCREEN_SET] = ptScreenSet;
        ptScreenSet->AddScreen();
    }
    {
        Console::ScreenSet * ptScreenSet = new Console::ScreenSet( sub_7100370440, sub_710016cae0 );
        ptScreenSet->SetCloseSound( GlobalSNOGet( GLOBALSNO_SOUND_PANEL_GENERIC_CLOSE ) );
        gptScreenManagerData->m_parScreenSets[Console::Screens::UNK_32_SCREEN_SET] = ptScreenSet;
        ptScreenSet->AddScreen();
    }

    Console::Notifications::CreateAmiiboTutorial();


    gptScreenManagerData->m_tModalManager.InitializeForGame();
    UIMainMenu::Console::ResetMainMenuFlag();

    if( !GameIsStartUp() )
    {
        Online::GetPartyMemberAddedEvent().Subscribe( OnlineService::internal::function_bind_0_1<void, int32, &OnPartyMemberAddedEvent>() );
    }
}






void Console::Screens::DisposeForGame()
{
    gptScreenManagerData->m_tModalManager.DisposeForGame();

    gptScreenManagerData->m_tLightboxManager.ClearAllLightBoxes();
    gptScreenManagerData->m_tActionManager.ClearAllActions();
    gptScreenManagerData->m_listScreenStack.RemoveAll();
    gptScreenManagerData->m_listModalDialogInfoStack.RemoveAll();
    gptScreenManagerData->m_idLeaveGameDialog = INVALID_MODAL_NOTIFICATION_ID;
    gptScreenManagerData->m_ptCurrentScreenSet = NULL;

    sCleanUpScreenSet( Console::Screens::GAMEPLAY_SCREEN_SET );
    sCleanUpScreenSet( Console::Screens::STASH_SCREEN_SET );
    sCleanUpScreenSet( Console::Screens::MAIL_SCREEN_SET );
    sCleanUpScreenSet( Console::Screens::MAIL_ITEM_SELECT_SCREEN_SET );
    sCleanUpScreenSet( Console::Screens::SHOP_SCREEN_SET );
    sCleanUpScreenSet( Console::Screens::MAP_SCREEN_SET );
    sCleanUpScreenSet( Console::Screens::SEASON_JOURNEY_SCREEN_SET );
    sCleanUpScreenSet( Console::Screens::MAIN_MENU_SCREEN_SET );
    sCleanUpScreenSet( Console::Screens::HERO_CREATE_SCREEN_SET );
    sCleanUpScreenSet( Console::Screens::HERO_SELECT_SCREEN_SET );

    sCleanUpScreenSet( Console::Screens::DESIGNER_MENU_SCREEN_SET );
    sCleanUpScreenSet( Console::Screens::OPTIONS_SCREEN_SET );
    sCleanUpScreenSet( Console::Screens::CHARACTER_DETAILS_SCREEN_SET );
    sCleanUpScreenSet( Console::Screens::TRADE_SCREEN_SET );
    sCleanUpScreenSet( Console::Screens::TRADE_ITEM_SELECT_SCREEN_SET );
    sCleanUpScreenSet( Console::Screens::CINEMATICS_SCREEN_SET );
    sCleanUpScreenSet( Console::Screens::JEWELER_SCREEN_SET );
    sCleanUpScreenSet( Console::Screens::BLACKSMITH_SCREEN_SET );
    sCleanUpScreenSet( Console::Screens::MYSTIC_SCREEN_SET );
    sCleanUpScreenSet( Console::Screens::BANNER_SCREEN_SET );
    sCleanUpScreenSet( Console::Screens::ACHIEVEMENTS_SCREEN_SET );
    sCleanUpScreenSet( Console::Screens::LEADERBOARD_SCREEN_SET );
    sCleanUpScreenSet( Console::Screens::UNK_31_SCREEN_SET );
    sCleanUpScreenSet( Console::Screens::UNK_32_SCREEN_SET );

    Online::GetPartyMemberAddedEvent().Unsubscribe( OnlineService::internal::function_bind_0_1<void, int32, &OnPartyMemberAddedEvent>() );
}







void Console::Screens::InitializeForWorld()
{
    if( !CmdLineGetParams()->tBitArray.Get( CMDLINE_BLIZZCON_BIT ) )
    {
        if( GameIsStartUp() )
        {
            UIHallOfFallenHeroes::Console::InitializeForWorld();
        }
        else
        {
            UIConsoleRiftEndScreen::InitializeForWorld();
        }
    }
}






void Console::Screens::DisposeForWorld()
{
    gptScreenManagerData->m_tFrontendBackgroundHeroManager.ClearAllBackgroundHeros();
    gptScreenManagerData->m_tFrontendBackgroundHeroManager.ClearBackgroundBanner();

    if( GameIsStartUp() && !CmdLineGetParams()->tBitArray.Get( CMDLINE_BLIZZCON_BIT ) )
    {
        UIHallOfFallenHeroes::Console::DisposeForWorld();
    }
}







BOOL Console::Screens::IsScreenSetOpen()
{
    return gptScreenManagerData->m_ptCurrentScreenSet != NULL;
}




// Line table: { at 946, || at 947:28, return at 952 (== at col 55); the early FALSE return has no row of its own.
BOOL Console::Screens::IsScreenSetOpen(
    const ScreenSets eScreenSet )
{
    if( !IsScreenSetOpen() || eScreenSet == INVALID_SCREEN_SET )
    {
        return FALSE;  // inferred: no row of its own; the branch goes straight to the shared epilogue
    }

    return gptScreenManagerData->m_ptCurrentScreenSet == gptScreenManagerData->m_parScreenSets[eScreenSet];
}


// Line table: return at 961:5, && at 961:40, == at 962:72.
BOOL Console::Screens::IsScreenOpen( const ScreenSets eScreenSet,
                                     const ScreenID idScreen )

{
    return IsScreenSetOpen(eScreenSet) &&
        gptScreenManagerData->m_ptCurrentScreenSet->GetCurrentScreen() == idScreen;
}



void sOnCloseTransitionOut(
    BOOL fClearScreenSetHistory,
    BOOL bTransitioningIn,
    FrontendScreenTransitionManager::ScreenKey eNextScreen )
{
    SNO snoCloseSound = -1;


    if( gptScreenManagerData->m_ptCurrentScreenSet )
    {
        snoCloseSound = gptScreenManagerData->m_ptCurrentScreenSet->GetCloseSound();
        gptScreenManagerData->m_ptCurrentScreenSet->Close();
        gptScreenManagerData->m_ptCurrentScreenSet = NULL;
    }
    if( fClearScreenSetHistory )
    {
        Console::Screens::ClearScreenSetHistory();
    }

    sShowGameUI( TRUE );
    Console::Screens::ShowButtonLegend( FALSE );

    if( eNextScreen == -1 && snoCloseSound != -1 )
    {

        SoundManager::PlaySound( snoCloseSound, cgtPSODefaults );
    }
}




void Console::Screens::CloseScreenSet(
    BOOL fClearScreenSetHistory,
    BOOL fSkipTransition )
{

    if( gptScreenManagerData->m_ptCurrentScreenSet && !fSkipTransition )
    {
        ScreenSet * ptScreenSet = gptScreenManagerData->m_ptCurrentScreenSet;
        ScreenID idScreen = ptScreenSet->GetCurrentScreen();
        ScreenSets eScreenSet = sGetScreenSet( ptScreenSet );
        FrontendScreenTransitionManager::ScreenKey eCurrent = sMakeScreenKey( eScreenSet, idScreen );
        gptScreenManagerData->m_tScreenTransitionManager.DoScreenTransition( eCurrent, -1, OnlineService::internal::function_bind_1_2<void, BOOL, BOOL, FrontendScreenTransitionManager::ScreenKey, &sOnCloseTransitionOut>( fClearScreenSetHistory ) );
    }
    else
    {
        sOnCloseTransitionOut( fClearScreenSetHistory, fSkipTransition, -1 );
    }

    Console::UIRActorManager::CloseRActor();
}




void Console::Screens::CloseAllScreenSets( BOOL bSkipTransitions )
{
    while( gptScreenManagerData->m_listScreenStack.Count() > 0 )
    {
        sOnPreviousScreenSetTransitionOut( 0, FALSE, -1 );
    }

    if( IsScreenSetOpen() )
    {
        CloseScreenSet( TRUE, bSkipTransitions );
    }
}


















void sOnTransitionOutDone( int32 nControllerOpening, BOOL bTransitioningIn, FrontendScreenTransitionManager::ScreenKey eNextScreenKey )
{
    Console::Screens::ScreenID idScreen = sGetScreen( eNextScreenKey );
    Console::Screens::ScreenSets eScreenSet = sGetScreenSet( eNextScreenKey );

    if( gptScreenManagerData->m_ptCurrentScreenSet && gptScreenManagerData->m_ptCurrentScreenSet != gptScreenManagerData->m_parScreenSets[eScreenSet] )
    {
        ScreenHistoryNode tHistory;
        tHistory.ptScreenSet = gptScreenManagerData->m_ptCurrentScreenSet;
        tHistory.nScreenIndex = gptScreenManagerData->m_ptCurrentScreenSet->GetCurrentScreen();

        Console::Screens::CloseScreenSet( FALSE, TRUE );
        gptScreenManagerData->m_listScreenStack.AddTail_Tracked( tHistory );
    }

    sShowGameUI( FALSE );
    Console::Screens::ShowButtonLegend( TRUE );

    gptScreenManagerData->m_ptCurrentScreenSet = gptScreenManagerData->m_parScreenSets[eScreenSet];
    gptScreenManagerData->m_ptCurrentScreenSet->OpenToScreen( idScreen, nControllerOpening );
}










inline BOOL Console::Screens::ScreenHasPriority( const ScreenSets eScreenSet, const ScreenID idScreen )
{
    if( eScreenSet == OPTIONS_SCREEN_SET && idScreen == PAUSE_SCREEN )
    {
        return TRUE;
    }
    if( eScreenSet == RIFT_JOIN_SCREEN_SET && idScreen == RIFT_JOIN_SCREEN )
    {
        return TRUE;
    }
    return FALSE;
}








BOOL Console::Screens::OpenToScreen( const ScreenSets eScreenSet, const ScreenID idScreen, int32 nControllerOpening, BOOL fSkipTransition )
{
    if( !GameIsStartUp() && !ScreenHasPriority( eScreenSet, idScreen ) )
    {
        Player * ptPlayer = PlayerGetByControllerIndex( nControllerOpening );
        if( ptPlayer )
        {
            ActorCommonData * ptACDPlayer = ACDTryToGet( ptPlayer->idPlayerACD );
            if( !ptACDPlayer->IsAlive() )
            {
                return FALSE;
            }
        }

        if( UIConversation::IsUIBlockingConversationPlaying() )
        {
            return FALSE;
        }

        if( UIRewards::IsAnyVisible() )
        {
            return FALSE;
        }
    }

    if( !GameIsStartUp() && eScreenSet != HERO_CREATE_SCREEN_SET && eScreenSet != HERO_SELECT_SCREEN_SET )
    {
        Player * ptPlayer = PlayerGetByControllerIndex( nControllerOpening );
        if( !CPlayerIsValid( ptPlayer ) )
        {
            return FALSE;
        }
    }

    FrontendScreenTransitionManager::ScreenKey eCurrent = -1;
    if( gptScreenManagerData->m_ptCurrentScreenSet )
    {
        ScreenID idCurrentScreen = gptScreenManagerData->m_ptCurrentScreenSet->GetCurrentScreen();
        eCurrent = sMakeScreenKey( sGetScreenSet( gptScreenManagerData->m_ptCurrentScreenSet ), idCurrentScreen );
    }

    gptScreenManagerData->m_tScreenTransitionManager.Reset();
    if( !fSkipTransition )
    {
        gptScreenManagerData->m_tScreenTransitionManager.DoScreenTransition( eCurrent, sMakeScreenKey( eScreenSet, idScreen ), OnlineService::internal::function_bind_1_2<void, int32, BOOL, FrontendScreenTransitionManager::ScreenKey, &sOnTransitionOutDone>( nControllerOpening ) );
    }
    else
    {
        sOnTransitionOutDone( nControllerOpening, FALSE, sMakeScreenKey( eScreenSet, idScreen ) );
    }

    return TRUE;
}















void Console::Screens::TryOpenToScreen( const ScreenSets eScreenSet, const ScreenID idScreen, int32 nControllerOpening, BOOL fSkipTransition )
{
    if( !IsScreenSetOpen() || UI_GetPrimaryUIController() == nControllerOpening )
    {
        OpenToScreen( eScreenSet, idScreen, nControllerOpening, fSkipTransition );
    }
}









inline void sOnNextTransitionOutDone( int32 nControllerOpening, Console::ScreenSet * ptScreenSet, BOOL, FrontendScreenTransitionManager::ScreenKey )
{
    ptScreenSet->OpenNextScreen( nControllerOpening );
}








void Console::Screens::OpenNextScreen( int32 nControllerOpening, BOOL fSkipTransition )
{
    if( IsScreenSetOpen() )
    {
        if( !GameIsStartUp() )
        {
            Player * ptPlayer = PlayerGetByControllerIndex( nControllerOpening );
            if( !CPlayerIsValid( ptPlayer ) )
            {
                return;
            }
        }

        ScreenID idScreen = gptScreenManagerData->m_ptCurrentScreenSet->GetCurrentScreen();
        ScreenSets eScreenSet = sGetScreenSet( gptScreenManagerData->m_ptCurrentScreenSet );
        int32 nNewScreen = Math::PinWrap( idScreen + 1, 0, gptScreenManagerData->m_ptCurrentScreenSet->GetNumScreens() );

        if( !fSkipTransition )
        {
            FrontendScreenTransitionManager::ScreenKey keyNext = sMakeScreenKey( eScreenSet, nNewScreen );
            FrontendScreenTransitionManager::ScreenKey keyCurrent = sMakeScreenKey( eScreenSet, idScreen );
            gptScreenManagerData->m_tScreenTransitionManager.DoScreenTransition( keyCurrent, keyNext,
                OnlineService::internal::function_binder_4<void, int32, ScreenSet *, BOOL, FrontendScreenTransitionManager::ScreenKey>::bind<&sOnNextTransitionOutDone>( nControllerOpening, gptScreenManagerData->m_ptCurrentScreenSet ) );
        }
        else
        {
            sOnNextTransitionOutDone( nControllerOpening, gptScreenManagerData->m_ptCurrentScreenSet, FALSE, -1 );
        }
    }
}

















inline void sOnPrevTransitionOutDone( int32 nControllerOpening, Console::ScreenSet * ptScreenSet, BOOL, FrontendScreenTransitionManager::ScreenKey )
{
    ptScreenSet->OpenPreviousScreen( nControllerOpening );
}








void Console::Screens::OpenPreviousScreen( int32 nControllerOpening, BOOL fSkipTransition )
{
    if( IsScreenSetOpen() )
    {
        if( !GameIsStartUp() )
        {
            Player * ptPlayer = PlayerGetByControllerIndex( nControllerOpening );
            if( !CPlayerIsValid( ptPlayer ) )
            {
                return;
            }
        }

        ScreenID idScreen = gptScreenManagerData->m_ptCurrentScreenSet->GetCurrentScreen();
        ScreenSets eScreenSet = sGetScreenSet( gptScreenManagerData->m_ptCurrentScreenSet );
        int32 nNewScreen = Math::PinWrap( idScreen - 1, 0, gptScreenManagerData->m_ptCurrentScreenSet->GetNumScreens() );

        if( !fSkipTransition )
        {
            FrontendScreenTransitionManager::ScreenKey keyPrev = sMakeScreenKey( eScreenSet, nNewScreen );
            FrontendScreenTransitionManager::ScreenKey keyCurrent = sMakeScreenKey( eScreenSet, idScreen );
            gptScreenManagerData->m_tScreenTransitionManager.DoScreenTransition( keyCurrent, keyPrev,
                OnlineService::internal::function_binder_4<void, int32, ScreenSet *, BOOL, FrontendScreenTransitionManager::ScreenKey>::bind<&sOnPrevTransitionOutDone>( nControllerOpening, gptScreenManagerData->m_ptCurrentScreenSet ) );
        }
        else
        {
            sOnPrevTransitionOutDone( nControllerOpening, gptScreenManagerData->m_ptCurrentScreenSet, FALSE, -1 );
        }
    }
}
















int32 Console::Screens::GetCurrentControllerIndex()
{
    if( IsScreenSetOpen() )
    {
        return gptScreenManagerData->m_ptCurrentScreenSet->GetControllerIndex();
    }
    return -1;
}

// 2.7.7: accessors of ConsoleScreenManagerData::unk_1008 (names not known).
extern "C" void sub_710016a990(int32 n)
{
    gptScreenManagerData->unk_1008.push_back(n);
}

extern "C" BOOL sub_710016aa70()
{
    return !gptScreenManagerData->unk_1008.empty();
}

extern "C" BOOL sub_710016aa90(int32 n)
{
    return blz::find(gptScreenManagerData->unk_1008.begin(), gptScreenManagerData->unk_1008.end(), n) != gptScreenManagerData->unk_1008.end();
}

extern "C" size_t sub_710016aae0()
{
    return gptScreenManagerData->unk_1008.size();
}

extern "C" void sub_710016ab00()
{
    gptScreenManagerData->unk_1008.clear();
}











void Console::Screens::EnableScreen( const ScreenSets eScreenSet, const ScreenID idScreen, const BOOL bEnable )
{
    gptScreenManagerData->m_parScreenSets[eScreenSet]->EnableScreen( idScreen, bEnable );
}






void Console::Screens::ShowModalDialog( int32 nControllerIndex, const UIID & idDialog, BOOL bBlockInput, const UIID & idLightboxOverride )
{
    ModalDialogInfo tInfo;

    tInfo.m_idControllerFocusBeforeModal = UI_GetControllerFocus();
    tInfo.m_idModalDialog = idDialog;
    if( idLightboxOverride != GetUIIDNone() )
    {
        tInfo.m_idModalDialog = idLightboxOverride;
    }
    tInfo.m_idModalLightBox = gptScreenManagerData->m_tLightboxManager.CreateLightBoxForWindow( tInfo.m_idModalDialog, bBlockInput );
    tInfo.m_bBlockInput = bBlockInput;
    gptScreenManagerData->m_listModalDialogInfoStack.AddTail_Tracked( tInfo );
    if( tInfo.m_idModalLightBox != GetUIIDNone() )
    {
        gptScreenManagerData->m_tLightboxManager.PushLightBox( tInfo.m_idModalLightBox );
    }

    UI_SetControllerFocus( GetUIIDNone() );
    UI_SetPrimaryUIController( nControllerIndex );
    UI_GetWindowFromID( idDialog ).SetActive( TRUE );
}








void Console::Screens::CloseModalDialog( const UIID & idDialog )
{
    if( gptScreenManagerData->m_listModalDialogInfoStack.Count() )
    {
        const ModalDialogInfo & tInfo = gptScreenManagerData->m_listModalDialogInfoStack.GetTail();
        gptScreenManagerData->m_tLightboxManager.PopLightBox();
        UI_DeleteControl( tInfo.m_idModalLightBox, TRUE );

        if( tInfo.m_idControllerFocusBeforeModal != GetUIIDNone() && UI_GetControlFromID( tInfo.m_idControllerFocusBeforeModal ).IsActive() )
        {
            UI_SetControllerFocus( tInfo.m_idControllerFocusBeforeModal );
        }

        UI_SetPrimaryUIController( GetCurrentControllerIndex() );

        gptScreenManagerData->m_listModalDialogInfoStack.RemoveTail();

        if( gptScreenManagerData->m_listModalDialogInfoStack.Count() )
        {
            ModalDialogInfo & tInfoScoped = gptScreenManagerData->m_listModalDialogInfoStack.GetTail();
            tInfoScoped.m_idModalLightBox = gptScreenManagerData->m_tLightboxManager.CreateLightBoxForWindow( tInfoScoped.m_idModalDialog, tInfoScoped.m_bBlockInput );

            if( UI_TestID( tInfoScoped.m_idModalLightBox ) )
            {
                UI_GetWindowFromID( tInfoScoped.m_idModalLightBox ).SetActive( TRUE );
            }
        }

        UI_GetWindowFromID( idDialog ).SetActive( FALSE );
    }
}








void Console::Screens::CloseAllModalDialogs()
{
    while( gptScreenManagerData->m_listModalDialogInfoStack.Count() )
    {
        const ModalDialogInfo & tInfo = gptScreenManagerData->m_listModalDialogInfoStack.GetTail();
        UI_GetWindowFromID( tInfo.m_idModalDialog ).SetActive( FALSE );
        gptScreenManagerData->m_tLightboxManager.PopLightBox();
        UI_DeleteControl( tInfo.m_idModalLightBox, TRUE );

        if( tInfo.m_idControllerFocusBeforeModal != GetUIIDNone() && UI_GetControlFromID( tInfo.m_idControllerFocusBeforeModal ).IsActive() )
        {
            UI_SetControllerFocus( tInfo.m_idControllerFocusBeforeModal );
        }

        UI_SetPrimaryUIController( GetCurrentControllerIndex() );

        gptScreenManagerData->m_listModalDialogInfoStack.RemoveTail();
    }
}







BOOL Console::Screens::IsModalDialogVisible()
{
    return gptScreenManagerData->m_tLightboxManager.IsLightBoxVisible();
}





BOOL Console::Screens::IsScreenTransitioning()
{
    return gptScreenManagerData->m_tScreenTransitionManager.IsTransitioning();
}





FrontendBackgroundCameraManager & Console::Screens::GetFrontendBackgroundCameraManager()
{
    return gptScreenManagerData->m_tFrontendBackgroundCameraManager;
}





FrontendBackgroundHeroManager & Console::Screens::GetFrontendBackgroundHeroManager()
{
    return gptScreenManagerData->m_tFrontendBackgroundHeroManager;
}





LightBoxManager & Console::Screens::GetLightBoxManager()
{
    return gptScreenManagerData->m_tLightboxManager;
}

#include "../../Battlenet/UI/Util/UIUtil.h"












CRefString DisplayElapsedTime(uint32 dwTicks, uint32 dwTotalTicks, const ElapsedTimeDisplayOptions & tOptions);





























namespace {
void CheckForOnlineStateTransitionToast()
{
    Console::Online::PartyOnlineState ePendingState;
    if( Console::Online::PartyIsOnlineStateChanging( ePendingState ) )
    {
        if( ePendingState > Console::Online::PARTY_LAN && s_dwOnlineStateChangingStart == 0 )
        {
            s_dwOnlineStateChangingStart = AppGetTicks();
        }
    }
    else
    {
        if( s_idStateChangingToast != INVALID_TOAST_ID )
        {
            Console::Screens::GetToastManager().OutroToast( s_idStateChangingToast );
            s_idStateChangingToast = INVALID_TOAST_ID;
        }
        s_dwOnlineStateChangingStart = 0;
    }

    if( s_dwOnlineStateChangingStart != 0 && s_idStateChangingToast == INVALID_TOAST_ID &&
        AppGetTicks() - s_dwOnlineStateChangingStart > 30 )
    {
        s_idStateChangingToast = Console::Screens::GetToastManager().SendIndefiniteToastConsole(
            StringListFetch( "Console:PS3OnlineInitMessage" ) );
    }
}
}








void Console::Screens::Update()
{
    GameCommonData & tGCD = CGameGetGameCommonData();
    if( tGCD.GameModeGet() == GAMEMODE_MENU && GameIsStartUp() )
    {
        gptScreenManagerData->m_tFrontendBackgroundCameraManager.UpdateBackgroundCamera();
        gptScreenManagerData->m_tFrontendBackgroundHeroManager.Update();
    }

    gptScreenManagerData->m_tScreenTransitionManager.Update();
    if( !ShellIsWindowMinimized() )
    {
        gptScreenManagerData->m_tToastManager.ProcessToasts();
    }
    gptScreenManagerData->m_tFanfareManager.Update();

    if( gptScreenManagerData && gptScreenManagerData->m_idLeaveGameDialog != INVALID_MODAL_NOTIFICATION_ID )
    {
        sUpdateLeaveGameDialog();
    }
    gptScreenManagerData->m_tActionManager.OnUpdate();

    int32 nUserIdx = -1;
    if( Online::PartyIsInParty() && !SaveMigration::IsInProgress() && Online::HasOutstandingInvite( nUserIdx ) )
    {
        BOOL bInviteToCurrentParty = Online::IsInviteToCurrentParty();
        BOOL bInParty = Online::PartyIsInParty( nUserIdx );
        if( bInviteToCurrentParty )
        {
            if( !bInParty )
            {
                if( UIOnlineActions::CanLocalPlayerJoin() )
                {
                    UIOnlineActions::LocalPlayerJoin( nUserIdx, !GameIsStartUp() );
                    Online::ClearOutstandingInvite();
                }
            }
            else
            {
                Online::ClearOutstandingInvite();
            }
        }
        else if( !bInParty )
        {
            if( GamerProfile::GetPrimaryProfileUserIndex() != -1 )
            {
                GamerProfile::Internal::HandleUserSignedOut( GamerProfile::GetPrimaryProfileUserIndex() );
            }
        }
        else if( GamerProfile::IsHeroLoaded( nUserIdx ) )
        {
            if( !( GamerProfile::GetHero( nUserIdx ).digest().player_flags() & 8 ) )
            {
                UIOnlineActions::JoinPartyByInvite( &sOnPartyJoinedByInvite );
            }
        }
    }

    CheckForOnlineStateTransitionToast();
}


























static void sFadeInTick( RActor * ptRActor )
{
    if( ptRActor->flAlpha != 1.0f )
    {
        float flFadeInLength = 42.0f;
        float flFadeInInc = 1.0f / flFadeInLength;
        ptRActor->flAlpha += flFadeInInc;
        ptRActor->flAlpha = 1.0f < ptRActor->flAlpha ? 1.0f : ptRActor->flAlpha;
    }
}







static void sFadeOutTick( RActor * ptRActor )
{
    if( ptRActor->flAlpha != 0.0f )
    {
        float flFadeOutLength = 30.0f;
        float flFadeOutInc = 1.0f / flFadeOutLength;
        ptRActor->flAlpha -= flFadeOutInc;
        ptRActor->flAlpha = 0.0f > ptRActor->flAlpha ? 0.0f : ptRActor->flAlpha;
    }
}







void Console::Screens::UpdateBackgroundLightBeamActor( CScene * ptScene )
{
    Attachment * ptLightBeamsAttachment = AttachmentFind( ptScene->tAttachmentList, 1 );
    if( ptLightBeamsAttachment )
    {
        RActor * ptLightBeamsActor = RActorTryToGet( (RActorID)ptLightBeamsAttachment->idAttachedObject );
        if( ptLightBeamsActor )
        {
            BackgroundCameraPosition eTargetPos = gptScreenManagerData->m_tFrontendBackgroundCameraManager.GetTargetBackgroundCameraPosition();
            BackgroundCameraPosition ePos = gptScreenManagerData->m_tFrontendBackgroundCameraManager.GetBackgroundCameraPosition();
            if( eTargetPos == CAM_POS_CLOSE || ( eTargetPos == CAM_POS_NONE && ePos == CAM_POS_CLOSE ) )
            {
                sFadeInTick( ptLightBeamsActor );
            }
            else
            {
                sFadeOutTick( ptLightBeamsActor );
            }
        }
    }
}










void sOnPreviousScreenSetTransitionOut( int32 nControllerOpening, BOOL bTransitioningIn, FrontendScreenTransitionManager::ScreenKey eNextScreenKey )
{
    ScreenHistoryNode tHistoryNode = gptScreenManagerData->m_listScreenStack.GetTail();
    gptScreenManagerData->m_listScreenStack.RemoveTail();

    if( gptScreenManagerData->m_ptCurrentScreenSet )
    {
        gptScreenManagerData->m_ptCurrentScreenSet->Close();
    }

    gptScreenManagerData->m_ptCurrentScreenSet = tHistoryNode.ptScreenSet;
    tHistoryNode.ptScreenSet->OpenToScreen( tHistoryNode.nScreenIndex, nControllerOpening );
}






void Console::Screens::GoToPreviousScreenSet( int32 nControllerOpening, BOOL fSkipTransition )
{
    if( gptScreenManagerData->m_listScreenStack.Count() > 0 )
    {
        const ScreenHistoryNode & tHistoryNode = gptScreenManagerData->m_listScreenStack.GetTail();
        ScreenID idPrevScreen = tHistoryNode.nScreenIndex;
        ScreenSets ePrevScreenSet = sGetScreenSet( tHistoryNode.ptScreenSet );

        FrontendScreenTransitionManager::ScreenKey keyCurrent = -1;
        if( gptScreenManagerData->m_ptCurrentScreenSet )
        {
            ScreenID idScreen = gptScreenManagerData->m_ptCurrentScreenSet->GetCurrentScreen();
            ScreenSets eScreenSet = sGetScreenSet( gptScreenManagerData->m_ptCurrentScreenSet );
            keyCurrent = sMakeScreenKey( eScreenSet, idScreen );
        }

        if( !fSkipTransition )
        {
            FrontendScreenTransitionManager::ScreenKey keyPrev = sMakeScreenKey( ePrevScreenSet, idPrevScreen );
            gptScreenManagerData->m_tScreenTransitionManager.DoScreenTransition( keyCurrent, keyPrev, OnlineService::internal::function_bind_1_2<void, int32, BOOL, FrontendScreenTransitionManager::ScreenKey, &sOnPreviousScreenSetTransitionOut>( nControllerOpening ) );
        }
        else
        {
            sOnPreviousScreenSetTransitionOut( nControllerOpening, FALSE, -1 );
        }
    }
    else
    {
        CloseScreenSet( TRUE, FALSE );
    }
}







void Console::Screens::ClearScreenSetHistory()
{
    gptScreenManagerData->m_listScreenStack.RemoveAll();
}






void Console::Screens::UpdateButtonLegendText()
{
    CRefString sLegend = gptScreenManagerData->m_tActionManager.ToString( ControllerActionManager::DEFAULT_SPACE_BETWEEN_LABELS );

    BOOL nFontBoost = 0;
    if( gptScreenManagerData->m_tActionManager.GetActiveActionCount() < 6 )
    {
        nFontBoost = 2;
    }
    UI_GetTextFromID( UITXT_CONSOLE_BUTTON_LEGEND_TEXT ).SetFontBoostSize( nFontBoost );

    UI_GetTextFromID( UITXT_CONSOLE_BUTTON_LEGEND_TEXT ).SetText( sLegend.str(), FALSE );
}








void Console::Screens::ShowButtonLegend( const BOOL bShow )
{
    UI_GetWindowFromID( UIWND_CONSOLE_BUTTON_LEGEND ).SetActive( bShow );
}





Console::ControllerActionManager & Console::Screens::GetActionManager()
{
    return gptScreenManagerData->m_tActionManager;
}






ConsoleModalNotificationManager & Console::Screens::GetModalNotificationManager()
{
    return gptScreenManagerData->m_tModalManager;
}





ConsoleToastManager & Console::Screens::GetToastManager()
{
    return gptScreenManagerData->m_tToastManager;
}





FanfareManager & Console::Screens::GetFanfareManager()
{
    return gptScreenManagerData->m_tFanfareManager;
}






void sSoftPauseOutsideTown( const int32 nControllerIndex, const BOOL bPause )
{
    BOOL bShouldPause = bPause;

    if( bPause )
    {
        Player * ptPlayer = PlayerGetByControllerIndex( nControllerIndex );
        if( ptPlayer )
        {
            if( const ActorCommonData * ptACD = ACDTryToGet( ptPlayer->idPlayerACD ) )
            {
                bShouldPause &= !ACDIsInTown( *ptACD );
            }
        }
    }

    GameSoftPause( bShouldPause );
}







void sShowGameUI(
    const BOOL bShow )
{
    if( GameIsStartUp() )
    {
        return;  // inferred: no row of its own; the branch goes straight to the shared epilogue
    }


    UI_GetWindowFromID( UIWND_CONSOLE_HUD ).SetActive( bShow );
    UI_GetWindowFromID( UIDLG_MINIMAP_ROOT ).SetActive( bShow );
    UI_GetWindowFromID( UISTACK_OBJHUD_MAINQUEST_REGION ).SetActive( bShow );
    UI_GetWindowFromID( UISTACK_OBJHUD_SIDEQUEST_REGION ).SetActive( bShow );


    UI_GetControlFromID( UIHOTKEY_CONSOLE_PREV_SCREEN ).SetActive( !bShow );
    UI_GetControlFromID( UIHOTKEY_CONSOLE_NEXT_SCREEN ).SetActive( !bShow );

    if( !bShow )
    {

        if( UINPC::Console::InteractMenuIsActive() )
        {
            UINPC::Console::InteractMenuDisable( UI_GetPrimaryUIController() );
        }


        UILoreNotify::Hide();
        UILorePlayer::Close( GetUIIDNone(), FALSE );
    }
}




static void sSetPlayerGender( int32 nControllerOpening )
{
    Player * ptPlayer = PlayerGetByControllerIndex( nControllerOpening );
    if( ptPlayer )
    {
        LanguageRules::SetClientGender( PlayerIsMale( *ptPlayer ) );
    }
}






void sOpenGameplayScreenSet( int32 nControllerOpening )
{
    sSetPlayerGender( nControllerOpening );

    GameSoftPause( TRUE );
}




void sCloseGameplayScreenSet( int32 nControllerIndex )
{
    Player * ptPlayer = PlayerGetByControllerIndex( nControllerIndex );
    if( ptPlayer )
    {
        ptPlayer->ptNewItemManager->Reset();
    }

    GameSoftPause( FALSE );
}






inline void sInitGameplayScreenSet()
{
    Console::ScreenSet * ptScreenSet = new Console::ScreenSet( sOpenGameplayScreenSet, sCloseGameplayScreenSet );
    ptScreenSet->SetCloseSound( GlobalSNOGet( GLOBALSNO_SOUND_PANEL_GENERIC_CLOSE ) );
    gptScreenManagerData->m_parScreenSets[Console::Screens::GAMEPLAY_SCREEN_SET] = ptScreenSet;

    ptScreenSet->AddScreen( UIInventory::Console::OpenForController, UIInventory::Console::Close );
    ptScreenSet->AddScreen( UISkills::Console::OpenForController, UISkills::Console::Close );
    ptScreenSet->AddScreen( UIParagonBonuses::Console::OpenForController, UIParagonBonuses::Console::Close );
    ptScreenSet->AddScreen( UIFollower::Console::OpenForController, UIFollower::Console::Close );
    ptScreenSet->AddScreen( UIParty::Console::OpenForController, UIParty::Console::Close );
    ptScreenSet->AddScreen( UILore::Console::OpenForController, UILore::Console::Close );
}





inline void sCleanUpScreenSet( Console::Screens::ScreenSets nScreenSet )
{
    delete gptScreenManagerData->m_parScreenSets[nScreenSet];
    gptScreenManagerData->m_parScreenSets[nScreenSet] = NULL;
}





void sOpenStashScreenSet( int32 nControllerOpening )
{
    sSetPlayerGender( nControllerOpening );

    GameSoftPause( TRUE );
    UIStash::Console::OpenForController( nControllerOpening );
}




void sCloseStashScreenSet( int32 nControllerOpening )
{
    GameSoftPause( FALSE );
    UIStash::Console::Close();
}






void sOpenEquipmentManagerScreenSet( int32 nControllerOpening )
{
    sSetPlayerGender( nControllerOpening );

    GameSoftPause( TRUE );
}




void sCloseEquipmentManagerScreenSet( int32 nControllerOpening )
{
    GameSoftPause( FALSE );
}






void sOpenEquipManDetailsScreenSet( int32 nControllerOpening )
{
    UIEquipmentManagerDetails::OpenForController( nControllerOpening );
}





void sCloseEquipManDetailsScreenSet( int32 nControllerOpening )
{
    UIEquipmentManagerDetails::Close();
}






inline void sInitStashScreenSet()
{
    Console::ScreenSet * ptScreenSet = new Console::ScreenSet( sOpenStashScreenSet, sCloseStashScreenSet );
    gptScreenManagerData->m_parScreenSets[Console::Screens::STASH_SCREEN_SET] = ptScreenSet;

    ptScreenSet->AddScreen( UIStash::Console::OpenForController, UIStash::Console::Close );
}





inline void sInitEquipmentManagerScreenSet()
{
    Console::ScreenSet * ptScreenSet = new Console::ScreenSet( sOpenEquipmentManagerScreenSet, sCloseEquipmentManagerScreenSet );
    gptScreenManagerData->m_parScreenSets[Console::Screens::EQUIPMENT_MANAGER_SCREEN_SET] = ptScreenSet;

    ptScreenSet->AddScreen( UIEquipmentManager::Console::OpenForController, UIEquipmentManager::Console::Close );

    Console::ScreenSet * ptEquipManSet = new Console::ScreenSet( sOpenEquipManDetailsScreenSet, sCloseEquipManDetailsScreenSet );
    ptEquipManSet->SetCloseSound( GlobalSNOGet( GLOBALSNO_SOUND_PANEL_GENERIC_CLOSE ) );
    gptScreenManagerData->m_parScreenSets[Console::Screens::EQUIPMENT_MANAGER_DETAILS_SCREEN_SET] = ptEquipManSet;

    ptEquipManSet->AddScreen( UIEquipmentManagerDetails::OpenInventory, NULL );
    ptEquipManSet->AddScreen( UIEquipmentManagerDetails::OpenSkills, NULL );
}





void sOpenMailScreenSet( int32 nControllerOpening )
{
    UIMail::Console::OpenForController( nControllerOpening );
}





void sCloseMailScreenSet( int32 nControllerIndex )
{
    UIMail::Console::Close();
}






inline void sInitMailScreenSets()
{
    {
        Console::ScreenSet * ptScreenSet = new Console::ScreenSet( sOpenMailScreenSet, sCloseMailScreenSet );
        ptScreenSet->SetCloseSound( GlobalSNOGet( GLOBALSNO_SOUND_MAIL_CLOSE ) );
        gptScreenManagerData->m_parScreenSets[Console::Screens::MAIL_SCREEN_SET] = ptScreenSet;

        ptScreenSet->AddScreen( UIMail::Console::OpenForController, UIMail::Console::Close );
        ptScreenSet->AddScreen( UISeasonStash::Console::OpenForController, UISeasonStash::Console::Close );
    }
}






void sOpenRiftEntryScreenSet( int32 nControllerOpening )
{
    GameSoftPause( TRUE );
    UIConsoleRiftEntry::OpenForController( nControllerOpening );
}





void sCloseRiftEntryScreenSet( int32 nControllerIndex )
{
    UIConsoleRiftEntry::Close();

    GameSoftPause( FALSE );
}





inline void sInitRiftEntryScreenSet()
{
    Console::ScreenSet * ptScreenSet = new Console::ScreenSet( sOpenRiftEntryScreenSet, sCloseRiftEntryScreenSet );
    ptScreenSet->SetCloseSound( GlobalSNOGet( GLOBALSNO_SOUND_PANEL_GENERIC_CLOSE ) );
    gptScreenManagerData->m_parScreenSets[Console::Screens::RIFT_ENTRY_SCREEN_SET] = ptScreenSet;

    ptScreenSet->AddScreen();
}





void sCloseRiftJoinScreenSet( int32 nControllerIndex )
{
    UIJoinRift::Close();
}






inline void sInitRiftJoinScreenSet()
{
    Console::ScreenSet * ptScreenSet = new Console::ScreenSet( NULL, sCloseRiftJoinScreenSet );
    ptScreenSet->SetCloseSound( GlobalSNOGet( GLOBALSNO_SOUND_PANEL_GENERIC_CLOSE ) );
    gptScreenManagerData->m_parScreenSets[Console::Screens::RIFT_JOIN_SCREEN_SET] = ptScreenSet;

    ptScreenSet->AddScreen();
}





void sCloseRiftCompleteScreenSet( int32 nControllerIndex )
{
    UIRiftComplete::Close();
}






inline void sInitRiftCompleteScreenSet()
{
    Console::ScreenSet * ptScreenSet = new Console::ScreenSet( UIRiftComplete::OpenForController, sCloseRiftCompleteScreenSet );
    ptScreenSet->SetCloseSound( GlobalSNOGet( GLOBALSNO_SOUND_PANEL_GENERIC_CLOSE ) );
    gptScreenManagerData->m_parScreenSets[Console::Screens::RIFT_COMPLETE_SCREEN_SET] = ptScreenSet;

    ptScreenSet->AddScreen();
}





void sCloseJewelUpgradeScreenSet( int32 nControllerIndex )
{
    UIJewelUpgrade::Close();
}






inline void sInitJewelUpgradeScreenSet()
{
    Console::ScreenSet * ptScreenSet = new Console::ScreenSet( UIJewelUpgrade::OpenForController, sCloseJewelUpgradeScreenSet );
    ptScreenSet->SetCloseSound( GlobalSNOGet( GLOBALSNO_SOUND_PANEL_GENERIC_CLOSE ) );
    gptScreenManagerData->m_parScreenSets[Console::Screens::JEWEL_UPGRADE_SCREEN_SET] = ptScreenSet;

    ptScreenSet->AddScreen();
}





void sOpenShopScreenSet( int32 nControllerOpening )
{
    sSetPlayerGender( nControllerOpening );
    UIShop::Console::OpenForController( nControllerOpening );
}





void sCloseShopScreenSet( int32 nControllerIndex )
{
    UIShop::Console::Close();
}






inline void sInitShopScreenSet()
{
    Console::ScreenSet * ptScreenSet = new Console::ScreenSet( sOpenShopScreenSet, sCloseShopScreenSet );
    ptScreenSet->SetCloseSound( GlobalSNOGet( GLOBALSNO_SOUND_PANEL_GENERIC_CLOSE ) );
    gptScreenManagerData->m_parScreenSets[Console::Screens::SHOP_SCREEN_SET] = ptScreenSet;

    ptScreenSet->AddScreen( UIShop::Console::OpenBuyScreen, NULL );
    ptScreenSet->AddScreen( UIShop::Console::OpenSellScreen, NULL );
    ptScreenSet->AddScreen( UIShop::Console::OpenRepairScreen, NULL );
    ptScreenSet->AddScreen( UIShop::Console::OpenBuybackScreen, NULL );
}





void sOpenBloodShardVendorScreenSet( int32 nControllerOpening )
{
    sSetPlayerGender( nControllerOpening );
    UIBloodShardVendor::Console::OpenForController( nControllerOpening );

    sSoftPauseOutsideTown( nControllerOpening, TRUE );
}






void sCloseBloodShardVendorScreenSet( int32 nControllerIndex )
{
    GameSoftPause( FALSE );
    UIBloodShardVendor::Console::Close();
}






inline void sInitBloodShardVendorScreenSet()
{
    Console::ScreenSet * ptScreenSet = new Console::ScreenSet( sOpenBloodShardVendorScreenSet, sCloseBloodShardVendorScreenSet );
    ptScreenSet->SetCloseSound( GlobalSNOGet( GLOBALSNO_SOUND_PANEL_GENERIC_CLOSE ) );
    gptScreenManagerData->m_parScreenSets[Console::Screens::BLOODSHARD_VENDOR_SCREEN_SET] = ptScreenSet;

    ptScreenSet->AddScreen();
}





void sOpenKanaisCubeScreenSet( int32 nControllerOpening )
{
    GameSoftPause( TRUE );

    Player * ptPlayer = PlayerGetByControllerIndex( nControllerOpening );
    if( ptPlayer )
    {
        ptPlayer->eActiveCrafterType = CRAFTER_TYPE_HORADRIM;
    }
}






void sCloseKanaisCubeScreenSet( int32 nControllerIndex )
{
    GameSoftPause( FALSE );

    Player * ptPlayer = PlayerGetByControllerIndex( nControllerIndex );
    if( ptPlayer )
    {
        SimpleMessage tMessage;
        ptPlayer->idACDActiveVendor = ACDID_INVALID;
        ptPlayer->eActiveCrafterType = CRAFTER_TYPE_INVALID;

        MessageSendToServer( CMSG_CLOSE_INTERACTION, &tMessage, sizeof(tMessage), ptPlayer );
    }
}





inline void sInitKanaisCubeScreenSet()
{
    Console::ScreenSet * ptScreenSet = new Console::ScreenSet( sOpenKanaisCubeScreenSet, sCloseKanaisCubeScreenSet );
    ptScreenSet->SetCloseSound( GlobalSNOGet( GLOBALSNO_SOUND_PANEL_GENERIC_CLOSE ) );
    gptScreenManagerData->m_parScreenSets[Console::Screens::KANAIS_CUBE_SCREEN_SET] = ptScreenSet;

    ptScreenSet->AddScreen( UITransmute::OpenForController, UITransmute::Close );
    ptScreenSet->AddScreen( UIEquipLegendaryPowers::OpenForController, UIEquipLegendaryPowers::Close );
}






void sOpenJewelerScreenSet( int32 nControllerOpening )
{
    sSetPlayerGender( nControllerOpening );
    UIJeweler::Console::OpenForController( nControllerOpening );

    sSoftPauseOutsideTown( nControllerOpening, TRUE );
}






void sCloseJewelerScreenSet( int32 nControllerIndex )
{
    GameSoftPause( FALSE );
    UIJeweler::Console::Close();
}







inline void sInitJewelerScreenSet()
{
    Console::ScreenSet * ptScreenSet = new Console::ScreenSet( sOpenJewelerScreenSet, sCloseJewelerScreenSet );
    ptScreenSet->SetCloseSound( GlobalSNOGet( GLOBALSNO_SOUND_PANEL_GENERIC_CLOSE ) );
    gptScreenManagerData->m_parScreenSets[Console::Screens::JEWELER_SCREEN_SET] = ptScreenSet;

    ptScreenSet->AddScreen( UIJeweler::Console::OpenCombineScreen, NULL );
    ptScreenSet->AddScreen( UIJeweler::Console::OpenSocketScreen, UIJeweler::Console::CloseSocketScreen );
    ptScreenSet->AddScreen( UIJeweler::Console::OpenRemoveScreen, UIJeweler::Console::CloseRemoveScreen );
    ptScreenSet->AddScreen( UIJeweler::Console::OpenTrainScreen, NULL );
}





void sOpenBlacksmithScreenSet( int32 nControllerOpening )
{
    sSetPlayerGender( nControllerOpening );
    UIBlacksmith::Console::OpenForController( nControllerOpening );

    sSoftPauseOutsideTown( nControllerOpening, TRUE );
}






void sCloseBlacksmithScreenSet( int32 nControllerIndex )
{
    GameSoftPause( FALSE );
    UIBlacksmith::Console::Close();
}







inline void sInitBlacksmithScreenSet()
{
    Console::ScreenSet * ptScreenSet = new Console::ScreenSet( sOpenBlacksmithScreenSet, sCloseBlacksmithScreenSet );
    ptScreenSet->SetCloseSound( GlobalSNOGet( GLOBALSNO_SOUND_PANEL_GENERIC_CLOSE ) );
    gptScreenManagerData->m_parScreenSets[Console::Screens::BLACKSMITH_SCREEN_SET] = ptScreenSet;

    ptScreenSet->AddScreen( UIBlacksmith::Console::OpenWeaponsScreen, NULL );
    ptScreenSet->AddScreen( UIBlacksmith::Console::OpenArmorScreen, NULL );
    ptScreenSet->AddScreen( UIBlacksmith::Console::OpenSalvageScreen, NULL );
    ptScreenSet->AddScreen( UIBlacksmith::Console::OpenRepairScreen, NULL );
    ptScreenSet->AddScreen( UIBlacksmith::Console::OpenTrainScreen, NULL );
}





void sOpenMysticScreenSet( int32 nControllerOpening )
{
    sSetPlayerGender( nControllerOpening );
    UIMystic::Console::OpenForController( nControllerOpening );

    sSoftPauseOutsideTown( nControllerOpening, TRUE );
}






void sCloseMysticScreenSet( int32 nControllerIndex )
{
    GameSoftPause( FALSE );
    UIMystic::Console::Close();
}







inline void sInitMysticScreenSet()
{
    Console::ScreenSet * ptScreenSet = new Console::ScreenSet( sOpenMysticScreenSet, sCloseMysticScreenSet );
    ptScreenSet->SetCloseSound( GlobalSNOGet( GLOBALSNO_SOUND_PANEL_GENERIC_CLOSE ) );
    gptScreenManagerData->m_parScreenSets[Console::Screens::MYSTIC_SCREEN_SET] = ptScreenSet;

    ptScreenSet->AddScreen( UIMystic::Console::OpenEnchantScreen, NULL );
    ptScreenSet->AddScreen( UIMystic::Console::OpenTransmogrifyScreen, NULL );
    ptScreenSet->AddScreen( UIMystic::Console::OpenDyeItemScreen, NULL );
    ptScreenSet->AddScreen( UIMystic::Console::OpenTrainScreen, NULL );
}





inline void sInitMapScreenSet()
{
    Console::ScreenSet * ptScreenSet = new Console::ScreenSet( UIWaypointMap::OpenWaypointMap, UIWaypointMap::CloseWaypointMap );
    gptScreenManagerData->m_parScreenSets[Console::Screens::MAP_SCREEN_SET] = ptScreenSet;

    ptScreenSet->AddScreen( UIMap::OpenForController, UIMap::Close );
    ptScreenSet->AddScreen( UIWaypointMap::OpenActMap, UIWaypointMap::CloseActMap );
    ptScreenSet->AddScreen( UIWaypointMap::OpenWorldMap, UIWaypointMap::CloseWorldMap );
}





inline void sInitSeasonJourneyScreenSet()
{
    Console::ScreenSet * ptScreenSet = new Console::ScreenSet;
    gptScreenManagerData->m_parScreenSets[Console::Screens::SEASON_JOURNEY_SCREEN_SET] = ptScreenSet;

    ptScreenSet->AddScreen( UISeasonJourney::Console::OpenForController, UISeasonJourney::Console::Close );
}






void sInitMainMenuScreenSet()
{
    Console::ScreenSet * ptScreenSet = new Console::ScreenSet;
    gptScreenManagerData->m_parScreenSets[Console::Screens::MAIN_MENU_SCREEN_SET] = ptScreenSet;

    ptScreenSet->AddScreen( UIStartScreen::Console::OpenForController, UIStartScreen::Console::Close );
    ptScreenSet->AddScreen( UIMainMenu::Console::OpenForController, UIMainMenu::Console::Close );

    UIIDArray arScreenUIIDs;
    arScreenUIIDs.push_back( UIDLG_CONSOLE_START_BUTTON );
    gptScreenManagerData->m_tScreenTransitionManager.RegisterScreenDialogs( sMakeScreenKey( Console::Screens::MAIN_MENU_SCREEN_SET, Console::Screens::START_SCREEN ),
                                                                            "StartScreen", arScreenUIIDs );

    arScreenUIIDs.clear();
    arScreenUIIDs.push_back( UIWND_CONSOLE_MAIN_MENU );
    gptScreenManagerData->m_tScreenTransitionManager.RegisterScreenDialogs( sMakeScreenKey( Console::Screens::MAIN_MENU_SCREEN_SET, Console::Screens::MAIN_MENU_SCREEN ),
                                                                            "MainMenu", arScreenUIIDs );
}









static void sCloseOptionsScreenSet( int32 nControllerIndex )
{
    SoundManager::PlaySound( GlobalSNOGet( GLOBALSNO_SOUND_INVENTORY_CLOSE ), cgtPSODefaults );
}







void sInitOptionsScreenSet()
{
    Console::ScreenSet * ptScreenSet = new Console::ScreenSet( NULL, sCloseOptionsScreenSet );
    gptScreenManagerData->m_parScreenSets[Console::Screens::OPTIONS_SCREEN_SET] = ptScreenSet;

    ptScreenSet->AddScreen( UIPause::Console::OpenForController, UIPause::Console::Close );
    ptScreenSet->AddScreen( UIOptions::Console::OpenForController, UIOptions::Console::Close );

    UIIDArray arScreenUIIDs;
    arScreenUIIDs.push_back( UIWND_PAUSE_CONSOLE );
    gptScreenManagerData->m_tScreenTransitionManager.RegisterScreenDialogs( sMakeScreenKey( Console::Screens::OPTIONS_SCREEN_SET, Console::Screens::PAUSE_SCREEN ),
                                                                            "Pause", arScreenUIIDs );

    arScreenUIIDs.clear();
    arScreenUIIDs.push_back( UIWND_CONSOLE_OPTIONS );
    gptScreenManagerData->m_tScreenTransitionManager.RegisterScreenDialogs( sMakeScreenKey( Console::Screens::OPTIONS_SCREEN_SET, Console::Screens::OPTIONS_SCREEN ),
                                                                            "Options", arScreenUIIDs );
}









inline void sInitLeaderboardScreenSet()
{
    Console::ScreenSet * ptScreenSet2 = new Console::ScreenSet;
    gptScreenManagerData->m_parScreenSets[Console::Screens::LEADERBOARD_SCREEN_SET] = ptScreenSet2;

    ptScreenSet2->AddScreen( UILeaderboards::Console::OpenForController, UILeaderboards::Console::Close );
}






static void sCloseHeroCreate( int32 nControllerIndex )
{
    UIHeroCreate::Console::Close();
}







void sInitHeroCreateScreenSet()
{
    Console::ScreenSet * ptScreenSet = new Console::ScreenSet( UIHeroCreate::Console::OpenForController, sCloseHeroCreate );
    ptScreenSet->SetCloseSound( GlobalSNOGet( GLOBALSNO_SOUND_PANEL_GENERIC_CLOSE ) );
    gptScreenManagerData->m_parScreenSets[Console::Screens::HERO_CREATE_SCREEN_SET] = ptScreenSet;

    ptScreenSet->AddScreen();

    UIIDArray arScreenUIIDs;
    arScreenUIIDs.push_back( UIWND_CONSOLE_HEROCREATE );
    gptScreenManagerData->m_tScreenTransitionManager.RegisterScreenDialogs( sMakeScreenKey( Console::Screens::HERO_CREATE_SCREEN_SET, Console::Screens::HERO_CREATE_SCREEN ),
                                                                            "HeroCreate", arScreenUIIDs );
}







static void sCloseHeroSelect( int32 nControllerIndex )
{
    UIHeroSelect::Console::Close();
}







void sInitHeroSelectScreenSet()
{
    Console::ScreenSet * ptScreenSet = new Console::ScreenSet( UIHeroSelect::Console::OpenForController, sCloseHeroSelect );
    ptScreenSet->SetCloseSound( GlobalSNOGet( GLOBALSNO_SOUND_PANEL_GENERIC_CLOSE ) );
    gptScreenManagerData->m_parScreenSets[Console::Screens::HERO_SELECT_SCREEN_SET] = ptScreenSet;

    ptScreenSet->AddScreen();

    UIIDArray arScreenUIIDs;
    arScreenUIIDs.push_back( UIWND_CONSOLE_HERO_SELECT );
    gptScreenManagerData->m_tScreenTransitionManager.RegisterScreenDialogs( sMakeScreenKey( Console::Screens::HERO_SELECT_SCREEN_SET, Console::Screens::HERO_SELECT_SCREEN ),
                                                                            "HeroSelect", arScreenUIIDs );
}






static void sCloseBannerCustomization( int32 nControllerIndex )
{
    UIBannerCustomization::Console::Close();
}





inline void sInitBannerScreenSet()
{
    Console::ScreenSet * ptScreenSet = new Console::ScreenSet( UIBannerCustomization::Console::OpenForController, sCloseBannerCustomization );
    ptScreenSet->SetCloseSound( GlobalSNOGet( GLOBALSNO_SOUND_PANEL_GENERIC_CLOSE ) );
    gptScreenManagerData->m_parScreenSets[Console::Screens::BANNER_SCREEN_SET] = ptScreenSet;

    ptScreenSet->AddScreen();
}




static void sCloseAchievements( int32 nControllerIndex )
{
    UIAchievements::Console::Close();
}





inline void sInitAchievementsScreenSet()
{
    Console::ScreenSet * ptScreenSet = new Console::ScreenSet( UIAchievements::Console::OpenForController, sCloseAchievements );
    ptScreenSet->SetCloseSound( GlobalSNOGet( GLOBALSNO_SOUND_PANEL_GENERIC_CLOSE ) );
    gptScreenManagerData->m_parScreenSets[Console::Screens::ACHIEVEMENTS_SCREEN_SET] = ptScreenSet;

    ptScreenSet->AddScreen();
}





static void sCloseCharacterDetails( int32 nControllerIndex )
{
    UICharacterDetails::Console::Close();
}





inline void sInitCharacterDetailsScreenSet()
{
    Console::ScreenSet * ptScreenSet = new Console::ScreenSet( UICharacterDetails::Console::OpenForController, sCloseCharacterDetails );
    ptScreenSet->SetCloseSound( GlobalSNOGet( GLOBALSNO_SOUND_PANEL_GENERIC_CLOSE ) );
    gptScreenManagerData->m_parScreenSets[Console::Screens::CHARACTER_DETAILS_SCREEN_SET] = ptScreenSet;

    ptScreenSet->AddScreen();
}




static void sCloseTrade( int32 nControllerIndex )
{
    UITrade::Console::Close();
}





inline void sInitTradeScreenSet()
{
    Console::ScreenSet * ptScreenSet = new Console::ScreenSet( UITrade::Console::OpenForController, sCloseTrade );
    ptScreenSet->SetCloseSound( GlobalSNOGet( GLOBALSNO_SOUND_PANEL_GENERIC_CLOSE ) );
    gptScreenManagerData->m_parScreenSets[Console::Screens::TRADE_SCREEN_SET] = ptScreenSet;

    ptScreenSet->AddScreen();
}




static void sCloseTradeSelect( int32 nControllerIndex )
{
    UIInventory::Console::Close();
}





inline void sInitTradeItemSelectScreenSet()
{
    Console::ScreenSet * ptScreenSet = new Console::ScreenSet( UIInventory::Console::OpenTradeSelectForController, sCloseTradeSelect );
    ptScreenSet->SetCloseSound( GlobalSNOGet( GLOBALSNO_SOUND_PANEL_GENERIC_CLOSE ) );
    gptScreenManagerData->m_parScreenSets[Console::Screens::TRADE_ITEM_SELECT_SCREEN_SET] = ptScreenSet;

    ptScreenSet->AddScreen();
}





static void sCloseCinematics( int32 nConrollerIndex )
{
    UICinematics::Console::Close();
}





inline void sInitCinematicsScreenSet()
{
    Console::ScreenSet * ptScreenSet = new Console::ScreenSet( UICinematics::Console::OpenForController, sCloseCinematics );
    gptScreenManagerData->m_parScreenSets[Console::Screens::CINEMATICS_SCREEN_SET] = ptScreenSet;

    ptScreenSet->AddScreen();
}





























static void sCloseDesignerMenu( int32 nControllerIndex )
{
    UIConsoleDesignerMenu::Close();
}






inline void sInitDesignerMenuScreenSet()
{
    Console::ScreenSet * ptScreenSet = new Console::ScreenSet( UIConsoleDesignerMenu::Open, sCloseDesignerMenu );
    gptScreenManagerData->m_parScreenSets[Console::Screens::DESIGNER_MENU_SCREEN_SET] = ptScreenSet;

    ptScreenSet->AddScreen();
}



namespace {
















void sUpdateLeaveGameDialog()
{
    for( int32 nControllerIndex = 0; nControllerIndex < 4; ++nControllerIndex )
    {
        LogoffCountDownData & tCountdownData = gptScreenManagerData->m_atLogoffCountDownData[nControllerIndex];
        if( Console::GamerProfile::GetPrimaryProfileUserIndex() == nControllerIndex &&
            gptScreenManagerData->m_idLeaveGameDialog != INVALID_MODAL_NOTIFICATION_ID )
        {
            uint32 uCurrentCountdownDuration = tCountdownData.m_uMinCountDownDuration;
            uint32 uTicksElapsed = CGameGetPredictedServerTime() - tCountdownData.m_uCountDownStartTime;
            uCurrentCountdownDuration -= tCountdownData.m_uKickCountDownDuration;

            ElapsedTimeDisplayOptions tElapsedTimeDisplayOptions;
            tElapsedTimeDisplayOptions.fOnlyDisplaySeconds = TRUE;
            CRefString szTimeRemaining = DisplayElapsedTime( uTicksElapsed, uCurrentCountdownDuration + 50, tElapsedTimeDisplayOptions );

            CRefString sMessage = CRefString().Format( "%s\n\n%s", StringListFetch( "Bnet_Notifications:LogoutText" ).str(), szTimeRemaining.str() );
            gptScreenManagerData->m_tModalManager.ChangeMessage( gptScreenManagerData->m_idLeaveGameDialog, sMessage );

            if( uTicksElapsed >= uCurrentCountdownDuration )
            {
                Player * ptPlayer = PlayerGetByControllerIndex( Console::GamerProfile::GetPrimaryProfileUserIndex() );
                SimpleMessage tMessage;
                MessageSendToServer( CMSG_LOGOUT_NOW, &tMessage, sizeof(tMessage), ptPlayer );
                tCountdownData = LogoffCountDownData();
            }
        }
    }
}




















void sOnPartyJoinedByInvite( Console::Online::JoinResult eResult )
{
    if( eResult == Console::Online::JOIN_SUCCESS && GameIsStartUp() )
    {
        UIPartyDisplay::Console::RefreshPartyDisplay();
        if( !Console::Screens::IsScreenSetOpen( Console::Screens::HERO_CREATE_SCREEN_SET ) &&
            !Console::Screens::IsScreenSetOpen( Console::Screens::HERO_SELECT_SCREEN_SET ) )
        {
            UIMainMenu::Console::Refresh3DBackgroundHeroes();
        }
    }
}








void sOnPlayerCanceledLogout( int32 nControllerIndex, const UIID & id )
{
    if( PlayerGetByControllerIndex( nControllerIndex ) )
    {
        BoolDataMessage tMessage;
        tMessage.bVal = FALSE;
        MessageSendToServer( CMSG_CANCEL_LOGOUT, &tMessage, sizeof(tMessage), PlayerGetByControllerIndex( nControllerIndex ) );
    }
}







void sOnLeaveGameDialogClosed( const UIID & id )
{
    gptScreenManagerData->m_idLeaveGameDialog = INVALID_MODAL_NOTIFICATION_ID;
}





void sShowLeaveGameDialog(
    int32 nControllerIndex,
    BOOL fShow )
{
    if(!gptScreenManagerData)
    {
        return;  // inferred: no row of its own; the branch goes straight to the shared epilogue
    }

    LogoffCountDownData &tLogoffData = gptScreenManagerData->m_atLogoffCountDownData[nControllerIndex];
    if( nControllerIndex == Console::GamerProfile::GetPrimaryProfileUserIndex() )
    {
        if( gptScreenManagerData->m_idLeaveGameDialog != INVALID_MODAL_NOTIFICATION_ID && !fShow )
        {
            gptScreenManagerData->m_tModalManager.RemoveNotification( gptScreenManagerData->m_idLeaveGameDialog );
            gptScreenManagerData->m_idLeaveGameDialog = INVALID_MODAL_NOTIFICATION_ID;
            tLogoffData = LogoffCountDownData();
        }

        if( gptScreenManagerData->m_idLeaveGameDialog == INVALID_MODAL_NOTIFICATION_ID && fShow )
        {
            NotificationCreationParams tParams;
            tParams.nControllerIndex = nControllerIndex;
            tParams.uszTitle = StringListFetch( "Bnet_Notifications:LogoutTitle" );
            tParams.uszMessage = StringListFetch( "Bnet_Notifications:LogoutText" );
            tParams.tCallbacks.pfnOnClickCancelButton = OnlineService::internal::function_bind_1_1<void, int32, const UIID &, &sOnPlayerCanceledLogout>( nControllerIndex );
            tParams.tCallbacks.pfnOnClose = OnlineService::internal::function_bind_0_1<void, const UIID &, &sOnLeaveGameDialogClosed>();


            if( tLogoffData.m_uKickCountDownDuration )
            {
                gptScreenManagerData->m_idLeaveGameDialog =
                    Console::Screens::GetModalNotificationManager().SetMessageOnlyNotification( tParams );
            }
            else
            {
                gptScreenManagerData->m_idLeaveGameDialog =
                    Console::Screens::GetModalNotificationManager().SetCancelNotification( tParams );
            }
        }
    }
}

}  // namespace






void Console::Notifications::ShowLeaveGameDialog(
    int32 nControllerIndex,
    BOOL fShow,
    uint32 uMinTickDuration,
    uint32 uMaxTickDuration,
    uint32 uKickTickDuration )
{
    if( !gptScreenManagerData )
    {
        return;  // inferred: no row of its own; the branch goes straight to the shared epilogue
    }

    if( fShow )
    {
        if( uMinTickDuration == 0 )
        {
            Player * ptPlayer = PlayerGetByControllerIndex( nControllerIndex );
            SimpleMessage tMessage;
            MessageSendToServer( CMSG_LOGOUT_NOW, &tMessage, sizeof(tMessage), ptPlayer );
            return;
        }
        else
        {
            LogoffCountDownData & tCountdownData = gptScreenManagerData->m_atLogoffCountDownData[nControllerIndex];
            tCountdownData.m_uCountDownStartTime = CGameGetPredictedServerTime();
            tCountdownData.m_uMinCountDownDuration = uMinTickDuration;
            tCountdownData.m_uMaxCountDownDuration = uMaxTickDuration;
            tCountdownData.m_uKickCountDownDuration = uKickTickDuration;
        }
    }

    sShowLeaveGameDialog( nControllerIndex, fShow );
}



void Console::Notifications::EnableHostMigrationLoadscreenTip( BOOL bFlag )
{
    s_bShowMigrationTip = bFlag;
}




BOOL Console::Notifications::HostMigrationLoadscreenTipEnabled()
{
    return s_bShowMigrationTip;
}



inline void Console::Notifications::CreateAmiiboTutorial()
{
    if( UI_TestID( UIWND_CONSOLE_AMIIBOTUTORIAL ) )
    {
        UI_GetWindowFromID( UIWND_CONSOLE_AMIIBOTUTORIAL ).SetActive( FALSE );
    }
}



void Console::Notifications::ShowAmiiboTutorial( int32 nControllerIndex )
{
    UIID idButton = UIIDCreateFromParent( UIWND_CONSOLE_AMIIBOTUTORIAL, ".LayoutRoot.AmiiboNotification.Buttons.ButtonList.ContinueButton" );
    UIText & tButton = UI_GetTextFromID( idButton );
    tButton.SetControllerIndexOverride( nControllerIndex );

    GameSoftPause( TRUE );
    HubGet().ScreenManagerGet().ShowModalDialog( UIWND_CONSOLE_AMIIBOTUTORIAL );
}



void Console::Notifications::OnAmiiboTutorialContinueButton( const UIID & id, const int32 nControllerIndex )
{
    GameSoftPause( FALSE );
    HubGet().ScreenManagerGet().CloseModalDialog( UIWND_CONSOLE_AMIIBOTUTORIAL );
}
