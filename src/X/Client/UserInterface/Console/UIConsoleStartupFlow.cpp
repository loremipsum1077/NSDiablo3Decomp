// Original file: X/Client/UserInterface/Console/UIConsoleStartupFlow.cpp
#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Common/SNO/SNOName.h"
#include "../../../Common/WorldPlace.h"
#include "../../../Sigma/Math/Vector.h"
#include "../../Gfx/GfxVB.h"
#include "../../../../NintendoSDK/Include/nn/ldn/ldn_Types.h"
#include "UIConsoleStartupFlow.h"
#include "ConsoleScreenManager.h"
#include "ControllerActions.h"
#include "../Controls/UITextControl.h"
#include "../Controls/UIWindowControl.h"
#include "../UI.h"
#include "../UIID.h"
#include "../UIIDs.h"
#include "../../Console/GamerProfile.h"
#include "../../CGame.h"
#include "../../Prefetch.h"
#include "../../SNOPreload.h"
#include "../../Sound/Sound.h"
#include "../../../Common/Definitions/GlobalDefinition.h"
#include "../../../Common/Definitions/HeroDefinition.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/SNO/SNOFiles.h"
#include "../../../Common/Protobuf/Hero.pb.h"
#include "../../../Sigma/RefString.h"
// Declared here because their headers are not adopted yet (signatures from the DWARF).
BOOL SNOPackChunkLoad(SNOPackChunk eSNOPackChunk);  // SNOFiles.cpp:11332
CRefString StringListFetch(LPCSTR szLabel);  // StringListDefinition.cpp
namespace Console { namespace UISaveIcon { const Vector2D & GetSaveIconDim(); void ForceDraw(const Vector2D * ptVecCenter); void StopForceDraw(); } }  // UISaveIcon.cpp:235, 244, 268
namespace Console { namespace Screens { enum { MAIN_MENU_SCREEN_SET = 20 }; BOOL OpenToScreen(const ScreenSets eScreenSet, const ScreenID idScreen, int32 nControllerOpening, BOOL fSkipTransition); } }  // ConsoleScreenManager.cpp

namespace UIStartupFlow {
namespace Console {
enum {
    SPLASHSCREEN_DELAY = 600,
    LEGALSCREEN_NOSKIP_DELAY = 300,
    LEGALSCREEN_DELAY = 600,
    AUTOSAVEWARNINGSCREEN_DELAY = 600,
    AUTOSAVEWARNINGSCREEN_SKIP_DELAY = 300,
    RATINGS_SCREEN_DELAY = 480,
    JAPANESE_INFO_SCREENS_DELAY = 180,
};

struct UIStartupFlowData {
    BOOL bStartupFlowComplete;
    BOOL bStartupFlowActive;
    StartupFlowScreens eStartupFlowScreen;
    uint32 dwStartupScreenStartTicks;









    UIStartupFlowData()
        : bStartupFlowComplete(FALSE)
        , bStartupFlowActive(FALSE)
        , eStartupFlowScreen(NO_STARTUP_SCREEN)
        , dwStartupScreenStartTicks(0)
    {
    }
};

UIStartupFlowData sgtStartupFlowData;
void OnStartupScreenOpened(StartupFlowScreens eScreen); void OnStartupScreenClosed(StartupFlowScreens eScreen);


BOOL CanSkipAutosaveScreen()
{

    return CGameGetRealTicks() - sgtStartupFlowData.dwStartupScreenStartTicks > AUTOSAVEWARNINGSCREEN_SKIP_DELAY &&
           !Prefetch::HasPriorityPending(TRUE);
}

void OnStartupFlowComplete()
{

    Prefetch::EnableHighPriority(TRUE);
    Prefetch::EndActiveWait();

    sgtStartupFlowData.bStartupFlowComplete = TRUE;
    sgtStartupFlowData.bStartupFlowActive = FALSE;

    int32 nUserIndex = ::Console::GamerProfile::GetPrimaryProfileUserIndex();
    if (nUserIndex != -1)
    {
        if (::Console::GamerProfile::IsHeroLoaded(nUserIndex) &&
            !(::Console::GamerProfile::GetHero(nUserIndex).digest().player_flags() & (1 << PLAYERFLAG_DECEASED)))
        {
            ::Console::Screens::OpenToScreen(::Console::Screens::MAIN_MENU_SCREEN_SET, 1, nUserIndex, FALSE);
        }
        else
        {



            ::Console::Screens::OpenToScreen(::Console::Screens::HERO_SELECT_SCREEN_SET, 0, nUserIndex, FALSE);
        }
    }
    else
    {



        ::Console::Screens::OpenToScreen(::Console::Screens::MAIN_MENU_SCREEN_SET, 0, -1, FALSE);
    }




    CGameGlobalsGet().dwFlags &= ~(1 << CGAMEGLOBALS_GAME_PAUSED_BIT);
}


















void PerformStartupFlow()
{
    CGameGlobalsGet().dwFlags |= (1 << CGAMEGLOBALS_GAME_PAUSED_BIT);


    BOOL bAct1 = SNOPackChunkLoad(SNOPACKCHUNK_ACT1);
    BOOL bAct2 = SNOPackChunkLoad(SNOPACKCHUNK_ACT2);
    BOOL bAct3 = SNOPackChunkLoad(SNOPACKCHUNK_ACT3);
    BOOL bAct4 = SNOPackChunkLoad(SNOPACKCHUNK_ACT4);
    BOOL bAct5 = SNOPackChunkLoad(SNOPACKCHUNK_ACT5);









    Prefetch::BeginActiveWait();
    Prefetch::EnableHighPriority(FALSE);
    Prefetch::BeginAdd();

    SNOPreload::AddFullClientWorld(GlobalSNOGet(GLOBALSNO_WORLD_BATTLENET_MAINSCREEN), FALSE);
    if (bAct1)
    {
        SNOPreload::AddFullClientWorld(GlobalSNOGet(GLOBALSNO_WORLD_BATTLENET_ACT1SCREEN), FALSE);
    }
    if (bAct2)
    {
        SNOPreload::AddFullClientWorld(GlobalSNOGet(GLOBALSNO_WORLD_BATTLENET_ACT2SCREEN), FALSE);
    }
    if (bAct3)
    {
        SNOPreload::AddFullClientWorld(GlobalSNOGet(GLOBALSNO_WORLD_BATTLENET_ACT3SCREEN), FALSE);
    }
    if (bAct4)
    {
        SNOPreload::AddFullClientWorld(GlobalSNOGet(GLOBALSNO_WORLD_BATTLENET_ACT4SCREEN), FALSE);
    }
    if (bAct5)
    {
        SNOPreload::AddFullClientWorld(GlobalSNOGet(GLOBALSNO_WORLD_BATTLENET_ACT5SCREEN), FALSE);
    }

    SNOPreload::AddNeutralClientActor(GlobalSNOGet(GLOBALSNO_ACTOR_CONSOLE_MENU_FILAGREE), FALSE);
    SNOPreload::AddNeutralClientActor(GlobalSNOGet(GLOBALSNO_ACTOR_FRONTEND_BARBARIAN_MALE), FALSE);
    SNOPreload::AddNeutralClientActor(GlobalSNOGet(GLOBALSNO_ACTOR_FRONTEND_BARBARIAN_FEMALE), FALSE);
    SNOPreload::AddNeutralClientActor(GlobalSNOGet(GLOBALSNO_ACTOR_FRONTEND_WITCHDOCTOR_MALE), FALSE);
    SNOPreload::AddNeutralClientActor(GlobalSNOGet(GLOBALSNO_ACTOR_FRONTEND_WITCHDOCTOR_FEMALE), FALSE);
    SNOPreload::AddNeutralClientActor(GlobalSNOGet(GLOBALSNO_ACTOR_FRONTEND_MONK_MALE), FALSE);
    SNOPreload::AddNeutralClientActor(GlobalSNOGet(GLOBALSNO_ACTOR_FRONTEND_MONK_FEMALE), FALSE);
    SNOPreload::AddNeutralClientActor(GlobalSNOGet(GLOBALSNO_ACTOR_FRONTEND_DHUNTER_MALE), FALSE);
    SNOPreload::AddNeutralClientActor(GlobalSNOGet(GLOBALSNO_ACTOR_FRONTEND_DHUNTER_FEMALE), FALSE);
    SNOPreload::AddNeutralClientActor(GlobalSNOGet(GLOBALSNO_ACTOR_FRONTEND_WIZARD_MALE), FALSE);
    SNOPreload::AddNeutralClientActor(GlobalSNOGet(GLOBALSNO_ACTOR_FRONTEND_WIZARD_FEMALE), FALSE);
    SNOPreload::AddNeutralClientActor(GlobalSNOGet(GLOBALSNO_ACTOR_FRONTEND_CRUSADER_MALE), FALSE);
    SNOPreload::AddNeutralClientActor(GlobalSNOGet(GLOBALSNO_ACTOR_FRONTEND_CRUSADER_FEMALE), FALSE);
    SNOPreload::AddNeutralClientActor(GlobalSNOGet(GLOBALSNO_ACTOR_FRONTEND_NECROMANCER_MALE), FALSE);
    SNOPreload::AddNeutralClientActor(GlobalSNOGet(GLOBALSNO_ACTOR_FRONTEND_NECROMANCER_FEMALE), FALSE);

    SNOPreload::AddNeutralClientActor(GlobalSNOGet(GLOBALSNO_ACTOR_STARTUP_BARBARIAN_MALE), FALSE);
    SNOPreload::AddNeutralClientActor(GlobalSNOGet(GLOBALSNO_ACTOR_STARTUP_BARBARIAN_FEMALE), FALSE);
    SNOPreload::AddNeutralClientActor(GlobalSNOGet(GLOBALSNO_ACTOR_STARTUP_WITCHDOCTOR_MALE), FALSE);
    SNOPreload::AddNeutralClientActor(GlobalSNOGet(GLOBALSNO_ACTOR_STARTUP_WITCHDOCTOR_FEMALE), FALSE);
    SNOPreload::AddNeutralClientActor(GlobalSNOGet(GLOBALSNO_ACTOR_STARTUP_MONK_MALE), FALSE);
    SNOPreload::AddNeutralClientActor(GlobalSNOGet(GLOBALSNO_ACTOR_STARTUP_MONK_FEMALE), FALSE);
    SNOPreload::AddNeutralClientActor(GlobalSNOGet(GLOBALSNO_ACTOR_STARTUP_DHUNTER_MALE), FALSE);
    SNOPreload::AddNeutralClientActor(GlobalSNOGet(GLOBALSNO_ACTOR_STARTUP_DHUNTER_FEMALE), FALSE);
    SNOPreload::AddNeutralClientActor(GlobalSNOGet(GLOBALSNO_ACTOR_STARTUP_WIZARD_MALE), FALSE);
    SNOPreload::AddNeutralClientActor(GlobalSNOGet(GLOBALSNO_ACTOR_STARTUP_WIZARD_FEMALE), FALSE);
    SNOPreload::AddNeutralClientActor(GlobalSNOGet(GLOBALSNO_ACTOR_STARTUP_CRUSADER_MALE), FALSE);
    SNOPreload::AddNeutralClientActor(GlobalSNOGet(GLOBALSNO_ACTOR_STARTUP_CRUSADER_FEMALE), FALSE);
    SNOPreload::AddNeutralClientActor(GlobalSNOGet(GLOBALSNO_ACTOR_STARTUP_NECROMANCER_MALE), FALSE);
    SNOPreload::AddNeutralClientActor(GlobalSNOGet(GLOBALSNO_ACTOR_STARTUP_NECROMANCER_FEMALE), FALSE);

    SNOPreload::AddNeutralClientActor(GlobalSNOGet(GLOBALSNO_ACTOR_EMOTE_BANNER_LIT), FALSE);

    const GlobalDefinition * ptGlobals = DefinitionLockR<GlobalDefinition>();
    const BannerParams & tBannerParams = ptGlobals->tBannerParams;

    for (uint32 i = 0; i < tBannerParams.serActorRibbons.dwSize / sizeof(SNO); i++)
    {
        SNOPreload::AddNeutralClientActor(tBannerParams.arActorRibbons[i], FALSE);
    }

    for (uint32 i = 0; i < tBannerParams.serActorCaps.dwSize / sizeof(SNO); i++)
    {
        SNOPreload::AddNeutralClientActor(tBannerParams.arActorCaps[i], FALSE);
    }

    for (uint32 i = 0; i < tBannerParams.serActorBases.dwSize / sizeof(SNO); i++)
    {
        SNOPreload::AddNeutralClientActor(tBannerParams.arActorBases[i], FALSE);
    }

    for (uint32 i = 0; i < tBannerParams.serActorPoles.dwSize / sizeof(SNO); i++)
    {
        SNOPreload::AddNeutralClientActor(tBannerParams.arActorPoles[i], FALSE);
    }

    Prefetch::EndAdd();






    if (sgtStartupFlowData.bStartupFlowComplete)
    {
        OnStartupFlowComplete();
        return;
    }

    sgtStartupFlowData.bStartupFlowActive = TRUE;
    OnStartupScreenOpened(RATINGS_SCREEN);


    ::Console::Screens::ShowButtonLegend(TRUE);
}












void UpdateStartupFlow()
{
    uint32 dwDelay = 0;

    switch (sgtStartupFlowData.eStartupFlowScreen)
    {
    case RATINGS_SCREEN:
        dwDelay = RATINGS_SCREEN_DELAY;
        break;
    case JAPANESE_KIDOJI_SCREEN:
    case JAPANESE_SHIYOJO_SCREEN:
    case JAPANESE_KENKO_SCREEN:
    case JAPANESE_SQEX_SCREEN:
        dwDelay = JAPANESE_INFO_SCREENS_DELAY;
        break;
    case AUTOSAVEWARNING_SCREEN:



















        if (CanSkipAutosaveScreen())
        {
            dwDelay = AUTOSAVEWARNINGSCREEN_DELAY;
        }
        else
        {
            dwDelay = AUTOSAVEWARNINGSCREEN_DELAY;
        }
        break;
    }



    BOOL bPrefetchPending = Prefetch::HasPriorityPending(TRUE);









    if (CGameGetRealTicks() - sgtStartupFlowData.dwStartupScreenStartTicks > dwDelay &&
        (!bPrefetchPending || sgtStartupFlowData.eStartupFlowScreen != AUTOSAVEWARNING_SCREEN))
    {
        OnStartupScreenClosed(sgtStartupFlowData.eStartupFlowScreen);
    }
}



void OnStartupScreenOpened(StartupFlowScreens eScreen)
{

    sgtStartupFlowData.dwStartupScreenStartTicks = CGameGetRealTicks();
    sgtStartupFlowData.eStartupFlowScreen = eScreen;

    switch (eScreen)
    {
    case RATINGS_SCREEN:
    {










        OnStartupScreenClosed(eScreen);
        break;
    }
    case JAPANESE_KIDOJI_SCREEN:
    case JAPANESE_SHIYOJO_SCREEN:
    case JAPANESE_KENKO_SCREEN:
    case JAPANESE_SQEX_SCREEN:
    {


        OnStartupScreenClosed(eScreen);
        break;
    }
    case AUTOSAVEWARNING_SCREEN:
    {


        UIWindow & tWindow = UI_GetWindowFromID(UIWND_CONSOLE_AUTOSAVEWARNINGSCREEN);
        tWindow.SetActive(TRUE);
        break;
    }
    }
}




void OnStartupScreenClosed(StartupFlowScreens eScreen)
{
    switch (eScreen)
    {
    case RATINGS_SCREEN:
    {
        UIWindow & tWindow = UI_GetWindowFromID(UIWND_CONSOLE_RATINGSSCENE);
        tWindow.SetActive(FALSE);
        OnStartupScreenOpened(JAPANESE_KIDOJI_SCREEN);
        break;
    }
    case JAPANESE_KIDOJI_SCREEN:
    {
        OnStartupScreenOpened(JAPANESE_SHIYOJO_SCREEN);
        break;
    }
    case JAPANESE_SHIYOJO_SCREEN:
    {
        OnStartupScreenOpened(JAPANESE_KENKO_SCREEN);
        break;
    }
    case JAPANESE_KENKO_SCREEN:
    {
        OnStartupScreenOpened(JAPANESE_SQEX_SCREEN);
        break;
    }
    case JAPANESE_SQEX_SCREEN:
    {
        UIWindow & tWindow = UI_GetWindowFromID(UIWND_CONSOLE_RATINGSSCENE);
        tWindow.SetActive(FALSE);
        OnStartupScreenOpened(AUTOSAVEWARNING_SCREEN);
        break;
    }
    case AUTOSAVEWARNING_SCREEN:
    {


























        UIWindow & tWindow = UI_GetWindowFromID(UIWND_CONSOLE_AUTOSAVEWARNINGSCREEN);
        tWindow.SetActive(FALSE);

        ::Console::UISaveIcon::StopForceDraw();

        sgtStartupFlowData.eStartupFlowScreen = NO_STARTUP_SCREEN;

        ::Console::Screens::GetActionManager().ClearAllActions();
        ::Console::Screens::UpdateButtonLegendText();

        if (Prefetch::HasPriorityPending(TRUE))
        {
            CGameLoadingPartRequired(LSC_SNO_MANAGER);
        }

        OnStartupFlowComplete();
        break;
    }
    }
}



void OnStartupFlowUpdate(const UIID &)
{

    UpdateStartupFlow();
}
















































void OnAutosaveScreenOpen(const UIID &)
{

    UIText & tText = UI_GetTextFromID(UITEXT_CONSOLE_AUTOSAVEWARNINGSCREEN);





    tText.SetText(StringListFetch("ConsoleUI:AutosaveWarningScreenText_Switch"), FALSE);

    Vector2D vecIconCenter, vecTextTL, vecTextDim, vecIconDim;
    tText.GetRelativePosition(vecTextTL, vecTextDim);
    vecIconDim = ::Console::UISaveIcon::GetSaveIconDim();
    vecIconCenter.x = vecTextTL.x + vecTextDim.x * 0.5f;
    vecIconCenter.y = vecTextTL.y + vecTextDim.y + vecIconDim.y * 0.5f;
    ::Console::UISaveIcon::ForceDraw(&vecIconCenter);
    ::Console::ControllerActionManager & tActionManager = ::Console::Screens::GetActionManager();
    tActionManager.ClearAllActions();

    ::Console::Screens::UpdateButtonLegendText();

}



void OnAutosaveScreenCloseButton(const UIID &, int32 nControllerIndex)
{

    if (CanSkipAutosaveScreen())
    {
        OnStartupScreenClosed(AUTOSAVEWARNING_SCREEN);
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SELECT_GENERIC), cgtPSODefaults);
    }
}

}  // namespace Console
}  // namespace UIStartupFlow
