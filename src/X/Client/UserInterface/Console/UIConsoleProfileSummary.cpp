// Original file: X/Client/UserInterface/Console/UIConsoleProfileSummary.cpp
// The file-static helpers (DisplayInfo, GetNextParagonLevelXPNeeded, PopulateClassProgressBar, PopulateInactiveClassNames)
// are only inlined into OpenForController; their bodies are written from that copy.
#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Common/SNO/SNOName.h"
#include "../../../Common/WorldPlace.h"
#include "../../Gfx/GfxVB.h"
#include "../../../../NintendoSDK/Include/nn/ldn/ldn_Ipv4Address.h"
#include "../../../../NintendoSDK/Include/nn/ldn/ldn_MacAddress.h"
#include "../../../Common/GameCommonData.h"
#include "../../../Common/GlobalSNO.h"
#include "../../Console/GamerProfile.h"
#include "../../Sound/Sound.h"
#include "ConsoleScreenManager.h"
#include "../Controls/UIWindowControl.h"
#include "../UIID.h"
#include "../Controls/UIProgressBarControl.h"
#include "../Controls/UITextControl.h"
#include "../UI.h"
#include "ControllerActions.h"
#include "../../../Common/Player.h"
#include "../../../Common/Definitions/GameBalanceDefinition.h"
#include "../../../Common/Definitions/StringListDefinition.h"
#include "../../../Common/Protobuf/Account.pb.h"
#include "../../../Common/Protobuf/Console.pb.h"
#include "../../../Common/Protobuf/Hero.pb.h"
#include "../../../Common/Protobuf/Profile.pb.h"
#include "../../../Sigma/Math/MathGeneral.h"
#include "../../../Sigma/Memory/SigmaMemory.h"
#include "../../../Sigma/RefString.h"
#include "../../../Sigma/UnicodeText.h"
// Declared here because their headers are not adopted yet; each comment names the defining file.
extern UIID UWND_CONSOLE_PROFILE_SUMMARY, UWND_CONSOLE_PROFILE_SUMMARY_PORTRAIT, UTEXT_CONSOLE_PROFILE_SUMMARY_NAME, UTEXT_CONSOLE_PROFILE_SUMMARY_PARAGON_INSTRUCTIONS, UWND_CONSOLE_PROFILE_SUMMARY_PARAGON, UTEXT_CONSOLE_PROFILE_SUMMARY_PARAGON_PERCENT, UTEXT_CONSOLE_PROFILE_SUMMARY_PARAGON_LEVEL, UPROGRESS_CONSOLE_PROFILE_SUMMARY_PARAGON;  // X/Client/UserInterface/UIGlobals.cpp
extern UIID UTEXT_CONSOLE_PROFILE_SUMMARY_GOLD, UTEXT_CONSOLE_PROFILE_SUMMARY_ELITES, UTEXT_CONSOLE_PROFILE_SUMMARY_KILLS, UTEXT_CONSOLE_PROFILE_SUMMARY_HIGHEST_HARDCORE, UTEXT_CONSOLE_PROFILE_SUMMARY_HIGHEST_DIFFICULTY;  // X/Client/UserInterface/UIGlobals.cpp
extern UIID UWND_CONSOLE_PROFILE_SUMMARY_BARBARIAN, UWND_CONSOLE_PROFILE_SUMMARY_CRUSADER, UWND_CONSOLE_PROFILE_SUMMARY_DEMONHUNTER, UWND_CONSOLE_PROFILE_SUMMARY_MONK, UWND_CONSOLE_PROFILE_SUMMARY_NECROMANCER, UWND_CONSOLE_PROFILE_SUMMARY_WITCHDOCTOR, UWND_CONSOLE_PROFILE_SUMMARY_WIZARD;  // X/Client/UserInterface/UIGlobals.cpp
extern UIID UTEXT_CONSOLE_PROFILE_SUMMARY_BARBARIAN_INACTIVE, UTEXT_CONSOLE_PROFILE_SUMMARY_CRUSADER_INACTIVE, UTEXT_CONSOLE_PROFILE_SUMMARY_DEMONHUNTER_INACTIVE, UTEXT_CONSOLE_PROFILE_SUMMARY_MONK_INACTIVE, UTEXT_CONSOLE_PROFILE_SUMMARY_NECROMANCER_INACTIVE, UTEXT_CONSOLE_PROFILE_SUMMARY_WITCHDOCTOR_INACTIVE, UTEXT_CONSOLE_PROFILE_SUMMARY_WIZARD_INACTIVE;  // X/Client/UserInterface/UIGlobals.cpp
void UI_SetPrimaryUIController(int32 nPrimaryController);  // X/Client/UserInterface/UI.cpp
namespace Console { namespace Screens { void ShowModalDialog(int32 nControllerIndex, const UIID & idDialog, BOOL bUnused, const UIID & idLightboxOverride); } }  // ConsoleScreenManager.cpp
namespace Console { namespace UIOnlineActions { bool IsActiveSeason(uint32 uSeason, const D3::Account::Digest & tAccountDigest); } }  // OnlineActions.cpp
namespace UIUtils { namespace Console {  // UIConsoleUtils.cpp:1000, 1021, 1043
int32 GetParagonLevel(const D3::Account::SavedDefinition & tAccount, BOOL bIsHardcore, BOOL bIsSeasonal);
uint64 GetParagonExperience(const D3::Account::SavedDefinition & tAccount, BOOL bIsHardcore, BOOL bIsSeasonal);
void DisplayPortrait(PlayerClass eClass, BOOL bFemale, int32 nAltLevel, const UIID & idPortrait, GBID gbidPortrait);
} }

namespace UIProfileSummary {
namespace Console {

void Close();
BOOL IsVisible();









struct UIProfileSummaryData {  // size 0x38
    struct ClassInfo {  // size 0x8
        uint32 nHighestLevel;  // 0x00
        DifficultyLevel eHighestDifficulty;  // 0x04
    };
    UIProfileSummaryData::ClassInfo arClassInfo[7];  // 0x00
};
static_assert(sizeof(UIProfileSummaryData) == 0x38, "UIProfileSummaryData");




UIProfileSummaryData sgtProfileSummaryData;



namespace {
void DisplayInfo(int32 nControllerIndex);
uint64 GetNextParagonLevelXPNeeded(int32 nParagonLevel);
void PopulateClassProgressBar(int32 nClass, uint32 nLevel);
void PopulateInactiveClassNames();
}




void OpenForController(int32 nControllerIndex)

{
    ::Console::Screens::ShowModalDialog(nControllerIndex, UWND_CONSOLE_PROFILE_SUMMARY, TRUE, GetUIIDNone());
    SigmaMemoryClear(&sgtProfileSummaryData, sizeof(sgtProfileSummaryData));
    UI_SetPrimaryUIController(nControllerIndex);

    ::Console::ControllerActionManager & tActionManager = ::Console::Screens::GetActionManager();
    tActionManager.ClearAllActions();
    tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, 1, StringListFetch("ConsoleUI:Close"));
    ::Console::Screens::UpdateButtonLegendText();

    PopulateInactiveClassNames();
    DisplayInfo(nControllerIndex);




}



void Close()
{
    if (!IsVisible())
    {
        return;
    }


    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_PANEL_GENERIC_CLOSE), cgtPSODefaults);

    UI_SetPrimaryUIController(::Console::GamerProfile::GetPrimaryProfileUserIndex());
    ::Console::Screens::CloseModalDialog(UWND_CONSOLE_PROFILE_SUMMARY);

}




void OnCancel(const UIID &, int32)


{
    Close();
}



BOOL IsVisible()

{
    return UI_GetWindowFromID(UWND_CONSOLE_PROFILE_SUMMARY).IsActive();
}



void OnLeftBumper(const UIID &, int32)









{
}


void OnRightBumper(const UIID &, int32)









{
}

namespace {


void DisplayInfo(int32 nControllerIndex)
{

    const D3::Account::SavedDefinition & tAccount =
        ::Console::GamerProfile::GetAccount(nControllerIndex);

    const D3::Hero::SavedDefinition & tHero =
        ::Console::GamerProfile::GetHero(nControllerIndex);
    const D3::Console::Profile & tProfile =
        ::Console::GamerProfile::GetProfile(nControllerIndex);
    const D3::Profile::AccountProfile & tAccountProfile = tProfile.account_profile();

    UI_GetTextFromID(UTEXT_CONSOLE_PROFILE_SUMMARY_NAME).SetText(
        ::Console::GamerProfile::GetGamerTag(nControllerIndex), FALSE);

    BOOL bHardcore = tHero.digest().player_flags() & 1;
    BOOL bSeasonal = ::Console::UIOnlineActions::IsActiveSeason(tHero.digest().season_created(), tAccount.digest());
    int32 nParagonLevel = UIUtils::Console::GetParagonLevel(tAccount, bHardcore, bSeasonal);
    uint64 nParagonXP = UIUtils::Console::GetParagonExperience(tAccount, bHardcore, bSeasonal);
    uint64 nParagonNeededXP = GetNextParagonLevelXPNeeded(nParagonLevel);
    nParagonXP = Math::Min(nParagonXP, nParagonNeededXP);
    float flParagonPercentage = (float)(nParagonNeededXP - nParagonXP) / (float)nParagonNeededXP * 100.0f;

    GBID gbidPortraitFrame = -1;

    if (tHero.digest().has_visual_equipment() && tHero.digest().visual_equipment().cosmetic_item_size() >= 3)
    {
        gbidPortraitFrame = tHero.digest().visual_equipment().cosmetic_item(3).gbid();
    }

    UIUtils::Console::DisplayPortrait(
        PlayerClassFromGBID(tHero.digest().gbid_class()),
        (tHero.digest().player_flags() >> 1) & 1,
        nParagonLevel,
        UWND_CONSOLE_PROFILE_SUMMARY_PORTRAIT,
        gbidPortraitFrame);

    if (nParagonLevel == 0)
    {
        UI_GetTextFromID(UTEXT_CONSOLE_PROFILE_SUMMARY_PARAGON_INSTRUCTIONS).SetActive(TRUE);
        UI_GetWindowFromID(UWND_CONSOLE_PROFILE_SUMMARY_PARAGON).SetActive(FALSE);
    }
    else
    {
        UI_GetTextFromID(UTEXT_CONSOLE_PROFILE_SUMMARY_PARAGON_INSTRUCTIONS).SetActive(FALSE);
        UI_GetWindowFromID(UWND_CONSOLE_PROFILE_SUMMARY_PARAGON).SetActive(TRUE);

        UI_GetTextFromID(UTEXT_CONSOLE_PROFILE_SUMMARY_PARAGON_LEVEL).SetText(
            LocalizedSprintf(
                StringListFetch("Console:AltLevel").str(),
                UnicodeText::FormatNumber(nParagonLevel, TRUE, FNS_NORMAL, 0).str(), NULL), FALSE);

        UI_GetTextFromID(UTEXT_CONSOLE_PROFILE_SUMMARY_PARAGON_PERCENT).SetText(
            LocalizedSprintf(
                StringListFetch("ConsoleUI:Percentage").str(),
                UnicodeText::FormatNumber((int32)flParagonPercentage, TRUE, FNS_NORMAL, 0).str(), NULL), FALSE);
    }


    UI_GetProgressBarFromID(UPROGRESS_CONSOLE_PROFILE_SUMMARY_PARAGON).SetExtentsAndPosition(0.0f, (float)nParagonNeededXP, (float)nParagonXP, -1.0f);





    uint64 dwGoldCollected = tAccountProfile.gold_collected();
    uint64 dwMonstersKilled = tAccountProfile.monsters_killed();
    uint64 dwElitesKilled = tAccountProfile.elites_killed();
    uint64 dwHighestHardcore = tAccountProfile.highest_hardcore_level();

    DifficultyLevel eHighestDifficulty = 0;
    uint32 nHighestLevel = 0;

    const D3::Profile::ClassInfo * artClassInfo[] =
    {
        &tAccountProfile.class_demonhunter(),
        &tAccountProfile.class_barbarian(),
        &tAccountProfile.class_wizard(),
        &tAccountProfile.class_witchdoctor(),
        &tAccountProfile.class_monk(),
        &tAccountProfile.class_crusader(),
        &tAccountProfile.class_necromancer(),
    };

    for (int32 i = 0; i < 7; i++)
    {
        const D3::Profile::ClassInfo * tClassInfo = artClassInfo[i];

        sgtProfileSummaryData.arClassInfo[i].eHighestDifficulty =
            tClassInfo->has_deprecated_highest_difficulty() ? tClassInfo->deprecated_highest_difficulty() : 0;
        sgtProfileSummaryData.arClassInfo[i].nHighestLevel =
            tClassInfo->has_highest_level() ? tClassInfo->highest_level() : 0;
    }



    for (int32 i = 0; i < 7; i++)
    {
        PopulateClassProgressBar(i, sgtProfileSummaryData.arClassInfo[i].nHighestLevel);
    }

    UI_GetTextFromID(UTEXT_CONSOLE_PROFILE_SUMMARY_HIGHEST_DIFFICULTY).SetText("", FALSE);

    UI_GetTextFromID(UTEXT_CONSOLE_PROFILE_SUMMARY_GOLD).SetText(
        UnicodeText::FormatNumber((int64)dwGoldCollected, TRUE, FNS_NORMAL, 0), FALSE);

    UI_GetTextFromID(UTEXT_CONSOLE_PROFILE_SUMMARY_ELITES).SetText(
        UnicodeText::FormatNumber((int32)dwElitesKilled, TRUE, FNS_NORMAL, 0), FALSE);

    UI_GetTextFromID(UTEXT_CONSOLE_PROFILE_SUMMARY_KILLS).SetText(
        UnicodeText::FormatNumber((int64)dwMonstersKilled, TRUE, FNS_NORMAL, 0), FALSE);

    UI_GetTextFromID(UTEXT_CONSOLE_PROFILE_SUMMARY_HIGHEST_HARDCORE).SetText(
        UnicodeText::FormatNumber((int32)dwHighestHardcore, TRUE, FNS_NORMAL, 0), FALSE);
}



uint64 GetNextParagonLevelXPNeeded(int32 nParagonLevel)
{

    SNO snoXPTable = GlobalSNOGet(GLOBALSNO_GB_EXPERIENCE_ALT);
    ScopedGameBalanceDefinitionLockR ptExpTableDef(snoXPTable);
    const ExperienceTableAlt & tExpTableAlt = ptExpTableDef->tExperienceTableAlt;


    uint64 nExpDiff = tExpTableAlt.ptExperienceLevelsAlt[Math::Min(nParagonLevel, 19999)].nDeltaXP;

    return nExpDiff;
}



void PopulateClassProgressBar(int32 nClass, uint32 nLevel)
{

    UIID arClassWindows[7] =
    {
        UWND_CONSOLE_PROFILE_SUMMARY_DEMONHUNTER,
        UWND_CONSOLE_PROFILE_SUMMARY_BARBARIAN,
        UWND_CONSOLE_PROFILE_SUMMARY_WIZARD,
        UWND_CONSOLE_PROFILE_SUMMARY_WITCHDOCTOR,
        UWND_CONSOLE_PROFILE_SUMMARY_MONK,
        UWND_CONSOLE_PROFILE_SUMMARY_CRUSADER,
        UWND_CONSOLE_PROFILE_SUMMARY_NECROMANCER,
    };

    const float MAX_LEVEL = 60.0f;

    const UIID & idWindow = arClassWindows[nClass];
    UIID idLevelText = UIIDCreateFromParent(idWindow, ".Level");
    UIID idLevelProgress = UIIDCreateFromParent(idWindow, ".Progress");
    UIID idClass = UIIDCreateFromParent(idWindow, ".Class");

    UI_GetTextFromID(idLevelText).SetText(
        UnicodeText::FormatNumber((int32)nLevel, TRUE, FNS_NORMAL, 0), FALSE);

    UI_GetProgressBarFromID(idLevelProgress).SetExtentsAndPosition(0.0f, MAX_LEVEL,
        (float)nLevel, -1.0f);

    UI_GetTextFromID(idClass).SetText(
        PlayerGetClassUIName((PlayerClass)nClass, TRUE), FALSE);

    UI_GetWindowFromID(idWindow).SetActive(nLevel > 0);
}




void PopulateInactiveClassNames()
{

    UIID arClassNames[7] =
    {
        UTEXT_CONSOLE_PROFILE_SUMMARY_DEMONHUNTER_INACTIVE,
        UTEXT_CONSOLE_PROFILE_SUMMARY_BARBARIAN_INACTIVE,
        UTEXT_CONSOLE_PROFILE_SUMMARY_WIZARD_INACTIVE,
        UTEXT_CONSOLE_PROFILE_SUMMARY_WITCHDOCTOR_INACTIVE,
        UTEXT_CONSOLE_PROFILE_SUMMARY_MONK_INACTIVE,
        UTEXT_CONSOLE_PROFILE_SUMMARY_CRUSADER_INACTIVE,
        UTEXT_CONSOLE_PROFILE_SUMMARY_NECROMANCER_INACTIVE,
    };

    for (int32 i = 0; i < 7; i++)
    {
        UI_GetTextFromID(arClassNames[i]).SetText(
            PlayerGetClassUIName((PlayerClass)i, TRUE), FALSE);
    }
}

}  // namespace

}  // namespace Console
}  // namespace UIProfileSummary
