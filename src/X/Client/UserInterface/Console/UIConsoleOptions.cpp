// Original file: X/Client/UserInterface/Console/UIConsoleOptions.cpp
#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Common/SNO/SNOName.h"
#include "../../../Common/WorldPlace.h"
#include "../../../Sigma/Math/Vector.h"
#include "../../Gfx/GfxVB.h"
#include "../../../../NintendoSDK/Include/nn/ldn/ldn_Types.h"
#include "../../Battlenet/Subsystems/FrontendBackgroundHeroManager.h"
#include "../../Console/Online.h"
#include "../../Console/GamerProfile.h"
#include "ConsoleInputUtilities.h"
#include "ConsoleScreenManager.h"
#include "ConsoleUIRActorManager.h"
#include "UIConsoleOptionsDataSource.h"
#include "../Controls/UIConsoleListboxControl.h"
#include "../Controls/UIRActorControl.h"
#include "../Controls/UITextControl.h"
#include "../UI.h"
#include "../UIID.h"
#include "../UIIDs.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Sigma/RefString.h"
#include "../../../Common/Game.h"
#include "../../../Common/Player.h"
#include "../../CPlayer.h"
#include "../../CGame.h"
#include "../../RActors/RActors.h"
#include "../../../Sigma/Math/Transform.h"
#include "../../Sound/Sound.h"
#include "../../../Common/VisualInventory.h"
#include "../../../Common/Definitions/AttachmentDefinition.h"
#include "../../../Common/Protobuf/Account.pb.h"
#include "../../../Common/Protobuf/Hero.pb.h"
// Declared here because their headers are not adopted yet (signatures from the DWARF).
void UI_SetControllerFocus(const UIID & idFocus);  // UI.cpp:599
UIRActor & UI_GetRActorFromID(const UIID & id);  // UI.cpp:5757
extern UIID UIWND_CONSOLE_UIRACTOR_ROOT; extern UIID UIRACTOR_CONSOLE_CENTERED; extern UIID UITXT_CONSOLE_PLAYER_LABEL_CENTERED;  // UIIDs.cpp
LightID LightAdd(const uint32 dwFlags, const SNO sno, const PRTransform & transform, const int32 nPriorityBoost, const RenderContext eContext);  // Lights.cpp:1104
SNO UI_GetDefaultHeroLight();  // UI.cpp:6354
SNO GetFrontendCharacterSNO(PlayerClass eClass, BOOL fIsFemale);  // UIUtil.cpp:1725
RActorID UI_CreateHeroUIRActor(SNO snoActor, SNO snoIrradianceTex, const VisualEquipment & tVisualEquipment, int32 eVIT, int32 ePlayerClass, BOOL fFemale, const PRTransform & transform, RenderContext eContext, uint32 dwAnimTag, float flScale, BOOL bDisableBlending);  // UIFunctions.cpp:3324
void RActorAttachExistingRActor(RActorID idRActorMain, RActorID idRActorAttachment, HardpointHash szHardpointName);  // RActorAttachments.cpp:2348
namespace Console { namespace UIOnlineActions { bool IsActiveSeason(uint32 uSeason, const D3::Account::Digest & tAccountDigest); } }  // OnlineActions.cpp
namespace UIUtils { namespace Console { CRefString PlayerGetDecoratedPlayerNum(int32 nControllerIndex); } }  // UIConsoleUtils.cpp
namespace SoundManager { void ApplyPrefSettings(); }  // Sound.cpp
namespace UIGammaAdjust { namespace Console { void Close(); } }  // UIConsoleGammaAdjust.cpp
namespace UISafeZoneSettings { void Close(); }  // UIConsoleSafeZoneSettings.cpp
namespace UIControllerLayout { namespace Console { void Close(); void OnUpdate(); } }  // UIConsoleControllerLayout.cpp
namespace UIBattlenetLink { void Close(); }  // UIConsoleBattlenetLink.cpp
namespace UIOptions {
namespace Console {
// UIConsoleOptionsDataSource.cpp
void ConfigureLegendForOptions(); ConsoleOptionsListDataSource & GetListDataSource(); void SetToSubmenuSelector(); void SaveData();
void OnListSelectionChanged(const UIID & id, const int32 nOldVal, const int32 nNewVal); void OnAccept(Selection & tSelection);
void RefreshHighlightRActor(); void LoadHeroModel(UIID & idRActorWindow);






void UpdateCharacterModel(int32 nControllerIndex)
{

    FrontendBackgroundHeroManager & tHeroManager = ::Console::Screens::GetFrontendBackgroundHeroManager();
    BackgroundHeroDescription tHero;
    D3::Hero::SavedDefinition & tHeroDefinition = ::Console::GamerProfile::GetHero(nControllerIndex);

    PlayerClass eClass = PlayerClassFromGBID(tHeroDefinition.digest().gbid_class());
    BOOL bIsFemale = (tHeroDefinition.digest().player_flags() & (1 << PLAYERFLAG_FEMALE)) != 0;
    BOOL bIsHardcore = (tHeroDefinition.digest().player_flags() & (1 << PLAYERFLAG_HARDCORE)) != 0;
    VisualEquipment tEquipment;
    tEquipment.Init();
    if (tHeroDefinition.digest().has_visual_equipment())
    {
        tEquipment.Import(tHeroDefinition.digest().visual_equipment());
    }
    tHero.SetPlayerHero(eClass, bIsFemale, bIsHardcore, FALSE, tEquipment);
    tHeroManager.ChangeBackgroundHero(HERO_SLOT_1, tHero);


    tHeroManager.ClearBackgroundHero(HERO_SLOT_2);
    tHeroManager.ClearBackgroundHero(HERO_SLOT_3);
    tHeroManager.ClearBackgroundHero(HERO_SLOT_4);
}



void RefreshHighlightRActor()
{

    UIRActor & tFilagree = UI_GetRActorFromID(UIRACTOR_CONSOLE_OPTIONS_HIGHLIGHT); PRTransform transform(cgqIdentity, cgwpNull);

    if (tFilagree.GetRActor() == RACTORID_INVALID)
    {
        RActorID idRActor = RActorCreate(
            GlobalSNOGet(GLOBALSNO_ACTOR_CONSOLE_MENU_FILAGREE),
            1.0f,
            -1.0f,
            transform,
            0,
            tFilagree.tRenderData[CGameAsyncGetRenderDataIndex(0)].m_tDisplayList.GetRenderContext(),
            -1);
        tFilagree.SetRActor(idRActor);
    }
}



void LoadHeroModel(UIID & idRActorWindow)
{

    UIRActor * tHeroRActor = &UI_GetRActorFromID(idRActorWindow);
    Player * ptPlayer = PlayerGetByControllerIndex(UI_GetPrimaryUIController());
    VisualInventory * ptVisualInventory = VisualInventoryGet(ptPlayer->idPlayerACD);
    PlayerClass ePlayerClass = PlayerGetPlayerClass(ptPlayer->idPlayerACD);
    BOOL bIsFemale = !PlayerIsMale(*ptPlayer);

    tHeroRActor->DeleteRActor();

    PRTransform transform(cgqIdentity, cgwpNull);

    SNO snoActor = GetFrontendCharacterSNO(ePlayerClass, bIsFemale);
    RActorID idRActorNew = UI_CreateHeroUIRActor(
        snoActor,
        GlobalSNOGet(GLOBALSNO_RACTOR_NEUTRAL_IRRADIANCE_TEXTURE),
        ptVisualInventory->tEquipmentCurrent,
        0,
        ePlayerClass,
        bIsFemale,
        transform,
        tHeroRActor->tRenderData[CGameAsyncGetRenderDataIndex(0)].m_tDisplayList.GetRenderContext(),
        0x68700, 1.0f, 0);
    tHeroRActor->SetRActor(idRActorNew);


    LightID idLight = LightAdd(
        0,
        UI_GetDefaultHeroLight(),
        PRTransform(cgqIdentity, cgwpNull),
        FALSE,
        tHeroRActor->tRenderData[CGameAsyncGetRenderDataIndex(0)].m_tDisplayList.GetRenderContext());
    tHeroRActor->SetLight(idLight);


    RActor * ptRActor = RActorGet(tHeroRActor->GetRActor());




    D3::Account::SavedDefinition & tAccountDefinition = ::Console::GamerProfile::GetAccount(ptPlayer->nControllerIndex);
    D3::Hero::SavedDefinition & tHeroDefinition = ::Console::GamerProfile::GetHero(ptPlayer->nControllerIndex);
    const D3::Hero::Digest & tHeroDigest = tHeroDefinition.digest();

    BOOL bSeasonal = ::Console::UIOnlineActions::IsActiveSeason(tHeroDigest.season_created(), tAccountDefinition.digest());
    BOOL bHardcore = ptPlayer->tHeroStateData.uPlayerFlags & 1;
    GlobalSNO gsnoWaypointActor = GLOBALSNO_ACTOR_CONSOLE_BNET_HERO_WAYPOINT;
    if (bHardcore && !bSeasonal)
    {
        gsnoWaypointActor = GLOBALSNO_ACTOR_CONSOLE_BNET_HERO_WAYPOINT_HC;
    }
    else if (!bHardcore && bSeasonal)
    {
        gsnoWaypointActor = GLOBALSNO_ACTOR_CONSOLE_BNET_HERO_WAYPOINT_SEASONAL;
    }
    else if (bSeasonal && bHardcore)
    {
        gsnoWaypointActor = GLOBALSNO_ACTOR_CONSOLE_BNET_HERO_WAYPOINT_SEASONAL_HC;
    }

    RActorID idWaypoint = RActorCreate(
        GlobalSNOGet(gsnoWaypointActor),
        1.0f,
        -1.0f,
        PRTransform(cgqIdentity, cgwpNull),
        0,
        ptRActor->eContext, -1);
    RActorAttachExistingRActor(
        ptRActor->id,
        idWaypoint,
        HardpointHash("Default"));
}

















void OpenForController(int32 nControllerIndex)
{

    UIWindow & tWnd = UI_GetWindowFromID(UIWND_CONSOLE_OPTIONS);
    if (tWnd.IsActive())
    {
        return;
    }

    BOOL bCreateRActor = ::Console::UIRActorManager::ShouldCreateRActor(::Console::UIRActorManager::RACTOR_CONTROL_CENTERED, ::Console::UIRActorManager::RACTOR_CHARACTER_PLAYER, FALSE, TRUE);
    BOOL bStartUp = GameIsStartUp();
    if (bStartUp)
    {
        tWnd.SetActive(TRUE);

        UpdateCharacterModel(nControllerIndex);
    }
    else
    {
        ::Console::Screens::ShowModalDialog(nControllerIndex, UIWND_CONSOLE_OPTIONS, TRUE, UIWND_CONSOLE_UIRACTOR_ROOT);
        if (bCreateRActor)
        {
            LoadHeroModel(UIRACTOR_CONSOLE_CENTERED);
        }
        if (PlayersAreAllLocal())
        {
            GamePause(TRUE);
        }
    }

    ConfigureLegendForOptions();

    RefreshHighlightRActor();

    UIConsoleListBox & tList = UI_GetConsoleListBoxFromID(UILIST_CONSOLE_OPTIONS_LIST);
    tList.SetAnalogStickInputFilter(::Console::GetDefaultStickInputFilter(::Console::FILTER_STRICT_VERTICAL));
    ConsoleOptionsListDataSource & tListData = GetListDataSource();
    tListData.SetUIControls("ConsoleOptions:ConsoleOptions_main.LayoutRoot", UILIST_CONSOLE_OPTIONS_LIST, UITXT_CONSOLE_OPTIONS_TOOLTIP, UIRACTOR_CONSOLE_OPTIONS_HIGHLIGHT);



    SetToSubmenuSelector();
    tList.SetOnSelChangeFunction(OnListSelectionChanged);
    tList.SetDataSource(tListData);
    tList.ResetAllItems();
    if (tList.GetNumItems() > 0)
    {
        tList.SetSelectedIndex(0);
    }

    UI_SetControllerFocus(GetUIIDNone());


    uint32 nMemberCount = ::Console::Online::PartyGetMemberCount();
    UIText & tPlayerLabel = UI_GetTextFromID(UITXT_CONSOLE_PLAYER_LABEL_CENTERED);
    if (!bStartUp && nMemberCount > 1)
    {
        tPlayerLabel.SetText(UIUtils::Console::PlayerGetDecoratedPlayerNum(nControllerIndex), FALSE);
        tPlayerLabel.SetActive(TRUE);
    }
    else
    {
        tPlayerLabel.SetActive(FALSE);
    }

    D3::Account::SavedDefinition & tAccountDefinition = ::Console::GamerProfile::GetAccount(nControllerIndex);
    D3::Hero::SavedDefinition & tHeroDefinition = ::Console::GamerProfile::GetHero(nControllerIndex);
    const D3::Hero::Digest & tHeroDigest = tHeroDefinition.digest();

    BOOL bHardcore = tHeroDigest.player_flags() & 1;
    bool bSeasonal = ::Console::UIOnlineActions::IsActiveSeason(tHeroDigest.season_created(), tAccountDefinition.digest());

    UI_GetWindowFromID(UIWND_CONSOLE_OPTIONS_TITLE_HARDCORE).SetActive(bHardcore && !bSeasonal);
    UI_GetWindowFromID(UIWND_CONSOLE_OPTIONS_TITLE_SEASONAL).SetActive(bSeasonal && !bHardcore);
    UI_GetWindowFromID(UIWND_CONSOLE_OPTIONS_TITLE_SEASONAL_HARDCORE).SetActive(bSeasonal && bHardcore);




}


void Close()
{





    UIGammaAdjust::Console::Close();

    UISafeZoneSettings::Close();

    UIControllerLayout::Console::Close();
    UIBattlenetLink::Close();
    if (GameIsStartUp())
    {
        UI_GetControlFromID(UIWND_CONSOLE_OPTIONS).SetActive(FALSE);
    }
    else
    {
        if (PlayersAreAllLocal())
        {
            GamePause(FALSE);
        }
        UIText & tPlayerLabel = UI_GetTextFromID(UITXT_CONSOLE_PLAYER_LABEL_CENTERED);
        tPlayerLabel.SetActive(FALSE);
        ::Console::Screens::CloseModalDialog(UIWND_CONSOLE_OPTIONS);
    }

    UIRActor & tFilagree = UI_GetRActorFromID(UIRACTOR_CONSOLE_OPTIONS_HIGHLIGHT);
    tFilagree.Cleanup();
}


void OnAccept(const UIID & id, const int32 nControllerIndex)
{

    if (UI_GetPrimaryUIController() != nControllerIndex || ::Console::Screens::IsScreenTransitioning())
    {
        return;
    }

    UIConsoleListBox & tList = UI_GetConsoleListBoxFromID(UILIST_CONSOLE_OPTIONS_LIST);
    if (tList.GetNumItems() <= 0)
    {
        return;
    }

    Selection & tSelection = (*GetListDataSource().m_ptActiveList)[tList.GetSelectedIndex()];
    OnAccept(tSelection);
}



void OnBack(const UIID &, const int32 nControllerIndex)
{

    if (UI_GetPrimaryUIController() != nControllerIndex || ::Console::Screens::IsScreenTransitioning())
    {
        return;
    }

    if (GetListDataSource().m_eCurrentMenu == SUB_MENU_SELECTOR)
    {
        if (!::Console::Screens::IsScreenTransitioning())
        {




            SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_PANEL_GENERIC_CLOSE), cgtPSODefaults);
            SaveData();
            if (GameIsStartUp())
            {
                ::Console::Screens::GoToPreviousScreenSet(nControllerIndex, FALSE);
            }
            else
            {
                ::Console::Screens::OpenPreviousScreen(nControllerIndex, TRUE);
            }
        }
    }
    else {
        SetToSubmenuSelector();
    }
}


void OnClose(const UIID &, const int32 nControllerIndex)
{

    if (UI_GetPrimaryUIController() == nControllerIndex &&
        !GameIsStartUp() && !::Console::Screens::IsScreenTransitioning())
    {



        SaveData();
        ::Console::Screens::CloseScreenSet(TRUE, TRUE);
    }
}


void OnNetworkOptions(const UIID &, const int32)
{
}



















void OnUpdate(const UIID &)
{

    UIConsoleListBox & tList = UI_GetConsoleListBoxFromID(UILIST_CONSOLE_OPTIONS_LIST);
    UI_GetWindowFromID(UIWND_CONSOLE_OPTIONS_ARROW_UP).SetActive(tList.GetFirstVisibleItemIndex() > 0);
    UI_GetWindowFromID(UIWND_CONSOLE_OPTIONS_ARROW_DOWN).SetActive(tList.GetFirstVisibleItemIndex() + tList.GetNumVisibleItems() < tList.GetNumItems());

    UIControllerLayout::Console::OnUpdate();
}



void OnPreferencesLoaded()
{

    ConsoleOptionsListDataSource & tListData = GetListDataSource();
    tListData.RefreshSelections();
    SoundManager::ApplyPrefSettings();
}

}  // namespace Console
}  // namespace UIOptions
