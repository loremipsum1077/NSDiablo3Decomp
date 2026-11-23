// Original file: X/Client/UserInterface/Interface/UIVictory.cpp
#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Common/SNO/SNOName.h"
#include "../../../Common/WorldPlace.h"
#include "../../Gfx/GfxVB.h"
#include "../../Gfx/GfxCommon.h"
#include "../../CGame.h"
#include "../../Console/GamerProfile.h"
#include "../Controls/UITextControl.h"
#include "../Controls/UIWindowControl.h"
#include "../UI.h"
#include "../UIID.h"
#include "../UIStoryboard.h"
#include "UIConfirmation.h"
#include "../../../Common/Definitions/StringListDefinition.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/Shell/CmdLine.h"
#include "../../../Sigma/DataStructures/XBitArray.h"
#include "../../../Sigma/LanguageRules.h"
#include "../../../Sigma/RefString.h"
// Declared here because their headers are not adopted yet (UI.cpp, UIGlobals.cpp, Sound.cpp, CWorld.cpp, CBlizzconTimer.cpp, UICredits.cpp, UIVictory.h).
UIStoryboard & UI_GetStoryboardFromID(const UIID & id); extern UIID UIWND_VICTORYSCREEN; extern UIID UIWND_VICTORYSCREEN_X1;
namespace SoundManager { void SuppressSounds(const uint32 dwSuppressedTypes); void DeleteAll(const uint32 dwSoundTypesToDelete, const RenderContext eContext); void PlayMusic(const SNO snoMusic, const int32 nFirstIndexToPlay, const BOOL fDisableShuffle); }
void CWorldDisableAreaMusic(const BOOL fDisabled); void CBlizzconReturnToStartScreen();
namespace UICredits { void VanillaCredits(); void ExpansionCredits(); void CreditsEternalCollection(); namespace Console { void Show(int32 nControllerIndex); } }
namespace UIVictory { enum Mode { MODE_VANILLA = 0, MODE_XP1 = 1, MODE_ETERNAL = 2, MODE_BLIZZCON = 3, MODE_BETA = 4, }; }

namespace UIVictory {
namespace {
static uint32 dwTickOpened = (uint32)-1;
static uint32 bShowRewards;
static uint32 bSkipped;
static uint32 bHidden;
static const uint32 DISPLAY_TIME = 600;
static const uint32 CONSOLE_DEMO_DISPLAY_TIME = 5400;

static UIID idCurVictoryWindow = GetUIIDNone();
static UIVictory::Mode sgeMode;
}






















void sShow()
{

    if (UI_TestID(idCurVictoryWindow))
    {



        if (UIConfirmation::IsVisible())
        {
            UIConfirmation::Close();
        }









        bHidden = FALSE;
        dwTickOpened = CGameGetClientTicks();

        const uint32 dwSuppressFlag = ~(uint32)2;
        SoundManager::SuppressSounds(dwSuppressFlag);
        SoundManager::DeleteAll((uint32)-1, RENDERCONTEXT_INVALID);
        SoundManager::PlayMusic(GlobalSNOGet(GLOBALSNO_MUSIC_CREDITS), 0, FALSE);
        CWorldDisableAreaMusic(TRUE);

        UI_GetWindowFromID(idCurVictoryWindow).SetActive(TRUE);

        const UIID idFadeIn = UIIDCreateFromParent(idCurVictoryWindow, ".OnLoad");
        UI_GetStoryboardFromID(idFadeIn).Play(FALSE, 0.0f);

        const UIID idNextDifficulty = UIIDCreateFromParent(idCurVictoryWindow, ".NextDifficulty");
        UIText & tNextDifficulty = UI_GetTextFromID(idNextDifficulty);
        tNextDifficulty.SetActive(FALSE);
        CRefString sRewardsText;

        const UIID idUnlockedRewards = UIIDCreateFromParent(idCurVictoryWindow, ".UnlockedRewards");
        UIText & tRewardsBlock = UI_GetTextFromID(idUnlockedRewards);

        if (sgeMode == MODE_BLIZZCON || sgeMode == MODE_BETA)
        {

            tNextDifficulty.SetActive(TRUE);
            const UIID idCongrats = UIIDCreateFromParent(idCurVictoryWindow, ".congrats");
            const UIID idDefeated = UIIDCreateFromParent(idCurVictoryWindow, ".defeated_d3");
            UIText & tCongrats = UI_GetTextFromID(idCongrats);
            UIText & tDefeatedD3 = UI_GetTextFromID(idDefeated);

            tDefeatedD3.SetActive(FALSE);

            tCongrats.SetText(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_DEMO_VICTORY), "VictoryThanks"), FALSE);
            if (sgeMode == MODE_BETA)
            {
                tNextDifficulty.SetText(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_DEMO_VICTORY), "VictoryDemoCompleteBeta"), FALSE);
            }
            else
            {
                tNextDifficulty.SetText(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_DEMO_VICTORY), "VictoryDemoComplete"), FALSE);
            }

            tRewardsBlock.SetActive(FALSE);
        }
        else
        {
            if (bShowRewards == TRUE)
            {
                sRewardsText = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_H2O_LAYOUT), "PostMalthaelKillRewards");
                CRefString sRewardsAfterLanguageRules(LanguageRules::ApplyLanguageRules(sRewardsText.str()));
                tRewardsBlock.SetText(sRewardsAfterLanguageRules, FALSE);
            }
            tRewardsBlock.SetActive(bShowRewards);
        }

        bShowRewards = FALSE; }
}
// NON_MATCHING: 132 vs 136 bytes. The original still tests eMode for the rewards flag on the out-of-range path (switch default edge not threaded); ours threads it. Re-laid on DWARF lines (vanilla assignment inlined at 150, X1 at 156, eMode on 144); tried case order ETERNAL-before-XP1 and an explicit default: break, no change.

void Show(
    const UIVictory::Mode eMode)
{
    sgeMode = eMode;
    switch (eMode)
    {
    case MODE_VANILLA:
        idCurVictoryWindow = UIWND_VICTORYSCREEN;
        break;
    case MODE_XP1:
    case MODE_ETERNAL:
    case MODE_BLIZZCON:
    case MODE_BETA:
        idCurVictoryWindow = UIWND_VICTORYSCREEN_X1;
        break;
    }


    if (eMode == MODE_XP1 || eMode == MODE_ETERNAL)
    {
        bShowRewards = TRUE;
    }

    sShow();
}


void OnHideFinished(const UIID &)
{

    if (!UI_TestID(idCurVictoryWindow))
    {
        return;
    }





    CWorldDisableAreaMusic(FALSE);

    dwTickOpened = (uint32)-1;

    if (sgeMode == MODE_BLIZZCON || sgeMode == MODE_BETA)
    {

        CBlizzconReturnToStartScreen();
        return;
    }




    UI_GetWindowFromID(idCurVictoryWindow).SetActive(FALSE);

    switch (sgeMode) { case MODE_XP1:

        UICredits::ExpansionCredits();
        break;
    case MODE_ETERNAL:

        UICredits::CreditsEternalCollection();
        break;
    default:

        UICredits::VanillaCredits();
        break;
    }

    UICredits::Console::Show(::Console::GamerProfile::GetPrimaryProfileUserIndex());
}





BOOL IsActive()
{

    if (UI_TestID(idCurVictoryWindow) &&

        UI_GetWindowFromID(idCurVictoryWindow).IsActive())
    {
        return TRUE;
    }
    return FALSE;
}


void Hide()
{

    if (IsActive())
    {

        bHidden = TRUE;



        const UIID idFadeOut = UIIDCreateFromParent(idCurVictoryWindow, ".OnUnload");
        UI_GetStoryboardFromID(idFadeOut).Play(FALSE, 0.0f);
    }
}

void OnUpdate(const UIID &)
{

    const uint32 dwNow = CGameGetClientTicks();




    const uint32 nDisplayTime = CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT) ? CONSOLE_DEMO_DISPLAY_TIME : DISPLAY_TIME;





    if (!bHidden &&
        dwNow - dwTickOpened > nDisplayTime)
    {
        Hide();
    }
}

namespace Console {








void Hide(const UIID &, const int32)
{


    if (!CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
    {
        UIVictory::Hide();
    }
}
}  // namespace Console

}  // namespace UIVictory
