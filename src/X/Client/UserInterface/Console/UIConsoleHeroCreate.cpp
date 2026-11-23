#include "../../../../BlizzardCore/Include/Blizzard/Time.h"
#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Battlenet/Subsystems/FrontendBackgroundHeroManager.h"
#include "../../CGame.h"
#include "ConsoleInputUtilities.h"
#include "../Controls/UIConsoleListboxControl.h"
#include "../../../Sigma/Math/Vector.h"
#include "../../Gfx/GfxVB.h"
#include "../../Console/GamerProfile.h"
#include "../../Console/Online.h"
#include "../../../Common/OnlineService/OnlineService.h"
#include "../../../Common/Game.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Sigma/RefString.h"
#include "../../../Sigma/UnicodeText.h"
#include "../../../Common/Shell/CmdLine.h"
#include "../UI.h"
#include "../UIID.h"
#include "../Controls/UITextControl.h"
#include "../Controls/UIWindowControl.h"
#include "../Controls/UICutsceneControl.h"
#include "../Controls/UIRadioButtonGroupControl.h"
#include "../Controls/UIRadioButtonControl.h"
#include "../../Sound/Sound.h"
#include "../../../../Contrib/Contrib/DemonWare/bdCore/bdSocket/bdCommonAddr.h"
#include "ConsoleScreenManager.h"
#include "../Controls/UIRActorControl.h"
#include "../Controls/UIButtonControl.h"
#include "ControllerActions.h"
#include "../../Console/ControllerUtils.h"
#include "../../../Sigma/LanguageRules.h"
#include "../../Battlenet/Subsystems/ModalNotificationManager.h"
#include "ConsoleModalNotificationManager.h"
#include "../../../Common/Console/ConsoleLicenses.h"
#include "../../Battlenet/Subsystems/FrontendBackgroundCameraManager.h"
#include "../../Definitions/ObserverDefinition.h"
#include "../../../Common/Definitions/StringListDefinition.h"
#include "../../../Common/Definitions/GameBalanceDefinition.h"
#include "../../../Common/Definitions/HeroDefinition.h"
#include "../../../Sigma/TypeDescriptor/SymbolicValue.h"
#include "../UIUtils.h"
#include "../../Battlenet/UI/Controls/ControlVisibilityManager.h"

namespace Console { namespace UIOnlineActions { void ClearLocalJoinFlag(); void FinalizeLocalPlayerJoin(int32 nControllerIndex, OnlineService::Callback<void (int)> pfnCallback); void SetGameParamsForHero(const D3::Account::SavedDefinition & tAccount, const D3::Hero::SavedDefinition & tHero, Online::GameParams & tParams); } }  // OnlineActions.h
namespace Console { namespace GamerProfile { void SignOut(int32 nUserIdx); } namespace Online { void PartyRemovePlayer(int32 nUserIdx); } }  // GamerProfile.h, Online.h
namespace Console { namespace Screens { enum { MAIN_MENU_SCREEN_SET = 20 }; BOOL OpenToScreen(const ScreenSets eScreenSet, const int32 idScreen, int32 nControllerOpening, BOOL fSkipTransition); } }  // ConsoleScreenManager.h
namespace UIGameSettings { namespace Console { void ClearGameSettingsViewed(); } }  // UIConsoleGameSettings.h
UICutscene & UI_GetCutsceneFromID(const UIID & id);  // UI.h
CRefString StringListFetch(LPCSTR szLabel);  // StringList.h
namespace Console { namespace Online { BOOL HasOutstandingInvite(int32 & nUserIdx); void ClearOutstandingInvite(); } }  // Online.h
namespace SoundManager { void SetCustomListenerMode(BOOL bEnabled); }  // Sound.h
UIRActor & UI_GetRActorFromID(const UIID & id); void LightDelete(LightID idLight);  // UI.h, Light.h
CRefString PlayerGetClassUIName(const PlayerClass ePlayerClass, const BOOL fIsMale);  // X/Common/Player.h
namespace Console { namespace Online { BOOL IsSeasonsInitialized(); void IsSeasonInProgress(int32 nUserIndex, IsSeasonInProgressCallback pfnCallback); } namespace DLCManager { uint32 GetLicenses(); } }  // Online.h, DLCManager.h
namespace SoundManager { void ListenerGetAxes(Vector3D & wvView, Vector3D & wvUp, Vector3D & wvRight); } void ListenerSetMicrophoneLocation(const PRTransform & tVolume, const PRTransform & tPanning, float flRolloff, float flDoppler);  // Sound.h
BOOL PlayersAreAllLocal(); SNO UI_GetDefaultHeroLight(); LightID LightAdd(uint32 dwFlags, SNO snoLight, const PRTransform & tTransform, BOOL bUnused, RenderContext eContext); SNO GetActBackgroundSNO(Act eAct); void UIStartup_SetBackgroundWorld(SNO snoWorld);  /* below: Battlenet/UI/Util/UIUtil.cpp, HeroDefinition.cpp, GamerProfile.cpp, Player.cpp, PreferencesDefinition.cpp */ BOOL HasOnlySpaces(const CRefString & usString); void HeroDefinition_Init(D3::Hero::SavedDefinition & tHeroDefinition); void HeroDefinitionSavedAttributeOverwriteInt(D3::AttributeSerializer::SavedAttributes & tAttributes, const FastAttribKey & tKey, int32 nValue); namespace Console { namespace GamerProfile { OnlineService::HeroId GenerateHeroID(int32 nUserIdx, LPCSTR szName); void SetHero(int32 nUserIdx, const D3::Hero::SavedDefinition & tHero); void SavePrefs(); } } GBID PlayerClassToGBID(const PlayerClass ePlayerClass); namespace Prefs { BOOL & GetFirstTimeFlowCompleted(); }  // Game.h, UI.h, Light.h, UIStartup.h
namespace UIUtils { namespace Console { CRefString PlayerGetDecoratedPlayerNum(int32 nControllerIndex); } }  // UIUtils.h
namespace UIWhatsNew { namespace Console { BOOL IsVisible(); } } namespace UIWhatsNewSeason { namespace Console { BOOL IsVisible(); } } namespace UIWhatsNewNX64 { namespace Console { BOOL IsVisible(); } }  // UIConsoleWhatsNew*.h
namespace Console { namespace Utilities { typedef OnlineService::Callback<void (int, CRefString)> StringInputCallback; void ShowKeyboardForUser(int32 nUserIndex, LPCSTR szDefaultText, LPCSTR szDialogTitle, LPCSTR szDialogMessage, uint32 dwMaxTextLength, StringInputCallback cb); } }  // Utilities.h
namespace Console { namespace GamerProfile {  OnlinePlayPrivilege GetOnlinePlayPrivilege(int32 nUserIdx); } }  // GamerProfile.h
extern SymbolicEntry gPlayerClasses[], gActorGenderNames[];  // X/Common/Player.h, X/Common/Actor.h
namespace UIMainMenu { namespace Console { void OpenStoreToNecromancer(int32 nControllerIndex); } } void RActorSetIrradianceMap(RActor * ptRActor, SNO snoIrradianceTex); SNO GetHeroCreateCharacterSNO(int32 ePlayerClass, int32 bIsFemale);  // UIConsoleMainMenu.h
extern UIID UIWND_CONSOLE_HEROCREATE_L1, UIWND_CONSOLE_HEROCREATE_R1, UITXT_CONSOLE_HEROCREATE_HARDCORE_WARNING_TITLE, UITXT_CONSOLE_HEROCREATE_HARDCORE_WARNING_DESC;  // UIIDs.h
extern UIID UITXT_CONSOLE_HEROCREATE_MALE, UITXT_CONSOLE_HEROCREATE_FEMALE, UIRADIOBTN_CONSOLE_HEROCREATE_MALE, UIRADIOBTN_CONSOLE_HEROCREATE_FEMALE, UITXT_CONSOLE_HEROCREATE_TITLE, UIWND_CONSOLE_HEROCREATE_HARDCORE, UIWND_CONSOLE_HEROCREATE_SEASONAL, UIWND_CONSOLE_HEROCREATE_SEASONAL_HARDCORE, UITXT_CONSOLE_HEROCREATE_PLAYER_NUMBER, UITXT_CONSOLE_HEROCREATE_GENDER_BUTTON;  // UIIDs.h
extern UIID UIRADIO_CONSOLE_HEROCREATE_GENDER_MALE, UIRADIO_CONSOLE_HEROCREATE_GENDER_FEMALE, UIRADIO_CONSOLE_HEROCREATE_GENDER, UIRACTOR_CONSOLE_HEROCREATE_HERO, UIRACTOR_CONSOLE_HEROCREATE_FILAGREE;  // UIIDs.h
extern UIID UIVID_CONSOLE_HEROCREATE_SKILL_VIDEO, UIWND_CONSOLE_HEROCREATE, UIRADIO_CONSOLE_HEROCREATE_HERO_TYPE, UILIST_CONSOLE_HEROCREATE_CLASSES, UILIST_CONSOLE_MENU_GROUP, UITXT_CONSOLE_HEROCREATE_STATUS, UIWND_CONSOLE_HEROCREATE_HARDCORE_WARNING, UITXT_CONSOLE_HEROCREATE_TYPE_TITLE, UITXT_CONSOLE_HEROCREATE_TYPE_DESC;  // UIIDs.h
extern UIID UITXT_CONSOLE_HEROCREATE_CLASS_SUMMARY, UITXT_CONSOLE_HEROCREATE_CLASS_DESC, UITXT_CONSOLE_HEROCREATE_CLASS_ATTRIBUTE;  // UIIDs.h (defined X/Client/UserInterface/UIGlobals.cpp:257-259)
enum { GB_MONSTER_NAMES = 19 };  // X/Common/GameBalance.h GameBalanceType (header enum in src is partial; RandomMonster.cpp keeps it local too)



































namespace UIHeroCreate {
namespace Console {


PlayerClass arClasses[] =
{
    PLAYER_CLASS_BARBARIAN,
    PLAYER_CLASS_CRUSADER,
    PLAYER_CLASS_DEMONHUNTER,
    PLAYER_CLASS_MONK,
    PLAYER_CLASS_NECROMANCER,
    PLAYER_CLASS_WITCHDOCTOR,
    PLAYER_CLASS_WIZARD,
};


PlayerClass arDemoClasses[] =
{
    PLAYER_CLASS_NECROMANCER,
};

// arStarterEditionClasses (line 125) is not in the binary; the linker dropped it.

enum { HEROCREATE_FLAG_HARDCORE = 0, HEROCREATE_FLAG_FEMALE = 1, HEROCREATE_FLAG_SEASONAL = 2 };
enum { HARDCORE_COOLDOWN_MILLISECS = 3000 };
enum ScreenActions { ACTION_SELECT = 0, ACTION_CONFIRM = 1, ACTION_BACK = 2, ACTION_TOGGLE_GENDER = 3, ACTION_CHANGE_HEROTYPE = 4, ACTION_TOGGLE_GAME_MODE = 5, ACTION_TOGGLE_BOOST_LEVEL = 6 };
enum UIHeroType { UIHEROTYPE_NORMAL = 0, UIHEROTYPE_HARDCORE = 1, UIHEROTYPE_SEASONAL = 2, UIHEROTYPE_SEASONAL_HARDCORE = 3, UIHEROTYPE_COUNT = 4 };
enum UIGender { UIGENDER_MALE = 0, UIGENDER_FEMALE = 1 };


















static int32 s_nDelayTicks = 0;
static int32 nLockedHeroType = -1;
static BOOL bRequestSeasonConfirmation = FALSE;
static BOOL bSeasonConfirmedAvailable = FALSE;
static uint32 uCachedSeasonAtConfirmation = 0;
static BOOL bPlayerIsGuest = FALSE;
static BOOL bHaveNecroLicense = FALSE;
static BOOL bIsAddingLocalPlayer = FALSE;








inline BOOL IsClassAvailable(PlayerClass eClass)
{
    if (eClass == PLAYER_CLASS_NECROMANCER)
    {
        return bHaveNecroLicense;
    }

    return TRUE;
}














inline PlayerClass * GetAvailablePlayerClasses()
{

    if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
    {
        return arDemoClasses;
    }

    return arClasses;
}

void DisplayClassSkillVideo(int32 eClass, BOOL bForce); void UpdateClassDescription(int32 eClass);
void SetHeroType(UIHeroType eType); CRefString GenerateRandomName(PlayerClass eClass, BOOL bIsFemale); void OnHeroNameEntered(int32 nControllerIndex, PlayerClass eClass, uint32 uFlags, BOOL bSuccess, CRefString sString); BOOL CreateHero(int32 nControllerIndex, PlayerClass eClass, uint32 uFlags, CRefString sString);
void OnAccept(const UIID &, const int32 nControllerIndex);




class HeroCreateDataSource : public ConsoleListBoxDataSource {
public:
    virtual ~HeroCreateDataSource() {}


    HeroCreateDataSource()
    {
    }


    virtual int32 GetNumItems() const
    {

        if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
        {
            return (int32)(sizeof(arDemoClasses) / sizeof(arDemoClasses[0]));
        }

        return (int32)(sizeof(arClasses) / sizeof(arClasses[0]));
    }






    virtual UIControlSourceKey GetSourceKeyForItem(const int32) const
    {
        return UIControlSourceKey::Parse("ConsoleHeroCreate:ConsoleHeroCreate_main.hero_item_template", SNO_NONE, NULL, NULL);
    }




    virtual void InitializeItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex)
    {
    }









    virtual void UpdateItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex)
    {

        // the class icons
        UIID idWizard = UIIDCreateFromParent(idItemUI, ".WizardIcon");
        UIID idWitchDoctor = UIIDCreateFromParent(idItemUI, ".WitchdoctorIcon");
        UIID idMonk = UIIDCreateFromParent(idItemUI, ".MonkIcon");
        UIID idDemonHunter = UIIDCreateFromParent(idItemUI, ".DemonhunterIcon");
        UIID idBarb = UIIDCreateFromParent(idItemUI, ".BarbarianIcon");
        UIID idCrusader = UIIDCreateFromParent(idItemUI, ".CrusaderIcon");
        UIID idNecromancer = UIIDCreateFromParent(idItemUI, ".NecromancerIcon");

        ControlVisibilityManager tIcons;
        tIcons.AddControl(idBarb);
        tIcons.AddControl(idDemonHunter);
        tIcons.AddControl(idMonk);
        tIcons.AddControl(idWitchDoctor);
        tIcons.AddControl(idWizard);
        tIcons.AddControl(idCrusader);
        tIcons.AddControl(idNecromancer);
        tIcons.SetActive(nIndex, false);

        PlayerClass * parClasses = GetAvailablePlayerClasses();

        PlayerClass eClass = parClasses[nIndex];
        BOOL bIsSelected = tListBox.GetSelectedIndex() == nIndex;

        UIID idSelected;
        if (tIcons.GetSelectedControl(idSelected))
        {
            UIRadioButtonGroup & tGenderSelection = UI_GetRadioButtonGroupFromID(UIRADIO_CONSOLE_HEROCREATE_GENDER);
            UIButton & tButton = UI_GetButtonFromID(idSelected);
            tButton.SetText(PlayerGetClassUIName(eClass, tGenderSelection.GetSelectedValue() == UIGENDER_MALE), FALSE);
            tButton.SetButtonState(bIsSelected ? BS_PUSHED : BS_NORMAL, FALSE, FALSE);
            tButton.SetEnabled(IsClassAvailable(eClass));
        }

        if (bIsSelected)
        {
            // move the highlight behind the selected class
            UIRActor & tFilagree = UI_GetRActorFromID(UIRACTOR_CONSOLE_HEROCREATE_FILAGREE);
            Vector2D vecFinalTL;
            Rect2D rectFilagree = tFilagree.GetWindowRect();
            UIWindow & tItem = UI_GetWindowFromID(idItemUI);
            Rect2D rectItem = tItem.GetWindowRect();
            AlignRect(Vector2D(rectItem.left, rectItem.top),
                      Vector2D(rectItem.Width(), rectItem.Height()),
                      ALIGN_CENTER_VERTICAL,
                      rectFilagree.left,
                      Vector2D(rectFilagree.Width(), rectFilagree.Height()),
                      vecFinalTL);

            tFilagree.MoveTo(vecFinalTL);
        }
    }
};















struct ListenerSettings {
    Vector3D wpPanningPos;
    Vector3D wpVolumePos;
    Vector3D wvView;
    Vector3D wvUp;
    Vector3D wvRight;
    BOOL fOverridden;
};


struct HeroCreateData {
    HeroCreateDataSource tHeroCreateDataSource;
    BOOL fCreateHardcore;
    BOOL fCreateSeasonal;
    int32 nPlayingClassVideo;
    LightID idInGameHeroLight;
    FrontendDemoHeroAnimationManager tAnimationManager;
    BOOL bInputEnabled;
    ListenerSettings tSavedListener;
    ::Console::AnalogStickInputFilter m_tStickInputFilter;
    BOOL fAdventureMode;
    BOOL fUsePAXBoost35;
    Blizzard::Time::Millisecond nModalOpenTime;
    BOOL fSeasonAvailable;
    int32 nControllerIndex;









    HeroCreateData()
        : fCreateHardcore(FALSE)
        , fCreateSeasonal(FALSE)
        , nPlayingClassVideo(-1)
        , idInGameHeroLight(-1)
        , bInputEnabled(TRUE)
        , fAdventureMode(FALSE)
        , fUsePAXBoost35(FALSE)
        , nModalOpenTime(0)
        , fSeasonAvailable(FALSE)
    {


        m_tStickInputFilter = ::Console::GetDefaultStickInputFilter(::Console::FILTER_HORIZONTAL);
    }
};


static HeroCreateData s_tHeroCreateData;


inline void EnableInput(BOOL bEnabled)
{
    s_tHeroCreateData.bInputEnabled = bEnabled;

    if (bEnabled)
    {
        if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT)) UI_SetControllerFocus(UILIST_CONSOLE_MENU_GROUP);
        else UI_SetControllerFocus(UILIST_CONSOLE_HEROCREATE_CLASSES);
    }
    else
    {
        UI_SetControllerFocus(GetUIIDNone());
    }
}




inline void UpdateTitle()
{
    BOOL bIsHardcore = s_tHeroCreateData.fCreateHardcore;
    BOOL bIsSeasonal = s_tHeroCreateData.fCreateSeasonal;

    UI_GetWindowFromID(UIWND_CONSOLE_HEROCREATE_HARDCORE).SetActive(bIsHardcore && !bIsSeasonal);
    UI_GetWindowFromID(UIWND_CONSOLE_HEROCREATE_SEASONAL).SetActive(bIsSeasonal && !bIsHardcore);
    UI_GetWindowFromID(UIWND_CONSOLE_HEROCREATE_SEASONAL_HARDCORE).SetActive(bIsHardcore && bIsSeasonal);
}




void UpdateClassDescription(
    int32 eClass)
{
    CRefString         sDesc1, sDesc2, sSummary, sAttribute;
    switch (eClass)
    {
        case PLAYER_CLASS_DEMONHUNTER:
                {
                    sSummary = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_CLASSES), "DemonHunterSummary").str(), NULL);
                    sDesc1 = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_CLASSES), "DemonHunterDescription1").str(), NULL);
                    sDesc2 = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_CLASSES), "DemonHunterDescription3").str(), NULL);
                    sAttribute = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_CLASSES), "DemonHunterPrimaryAttribute").str(), NULL);
                }
                break;
        case PLAYER_CLASS_BARBARIAN:
                {
                    sSummary = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_CLASSES), "BarbarianSummary").str(), NULL);
                    sDesc1 = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_CLASSES), "BarbarianDescription1").str(), NULL);
                    sDesc2 = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_CLASSES), "BarbarianDescription3").str(), NULL);
                    sAttribute = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_CLASSES), "BarbarianPrimaryAttribute").str(), NULL);
                }
                break;
        case PLAYER_CLASS_WIZARD:
                {
                    sSummary = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_CLASSES), "WizardSummary").str(), NULL);
                    sDesc1 = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_CLASSES), "WizardDescription1").str(), NULL);
                    sDesc2 = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_CLASSES), "WizardDescription3").str(), NULL);
                    sAttribute = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_CLASSES), "WizardPrimaryAttribute").str(), NULL);
                }
                break;
        case PLAYER_CLASS_WITCHDOCTOR:
                {
                    sSummary = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_CLASSES), "WitchDoctorSummary").str(), NULL);
                    sDesc1 = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_CLASSES), "WitchDoctorDescription1").str(), NULL);
                    sDesc2 = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_CLASSES), "WitchDoctorDescription3").str(), NULL);
                    sAttribute = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_CLASSES), "WitchDoctorPrimaryAttribute").str(), NULL);
                }
                break;

        case PLAYER_CLASS_MONK:
                {
                    sSummary = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_CLASSES), "MonkSummary").str(), NULL);
                    sDesc1 = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_CLASSES), "MonkDescription1").str(), NULL);
                    sDesc2 = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_CLASSES), "MonkDescription3").str(), NULL);
                    sAttribute = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_CLASSES), "MonkPrimaryAttribute").str(), NULL);
                }
                break;
        case PLAYER_CLASS_CRUSADER:
                {
                    sSummary = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_CLASSES), "CrusaderSummary").str(), NULL);
                    sDesc1 = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_CLASSES), "CrusaderDescription1").str(), NULL);
                    sDesc2 = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_CLASSES), "CrusaderDescription3").str(), NULL);
                    sAttribute = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_CLASSES), "CrusaderPrimaryAttribute").str(), NULL);
                }
                break;
        case PLAYER_CLASS_NECROMANCER:
                {
                    sSummary = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_CLASSES), "NecromancerSummary").str(), NULL);
                    sDesc1 = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_CLASSES), "NecromancerDescription1").str(), NULL);
                    sDesc2 = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_CLASSES), "NecromancerDescription3").str(), NULL);
                    sAttribute = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_CLASSES), "NecromancerPrimaryAttribute").str(), NULL);
                }
                break;
    }



    UI_GetTextFromID(UITXT_CONSOLE_HEROCREATE_CLASS_SUMMARY).SetText(sSummary, FALSE);
    UI_GetTextFromID(UITXT_CONSOLE_HEROCREATE_CLASS_DESC).SetText(sDesc1 + "\n\n" + sDesc2, FALSE);
    UI_GetTextFromID(UITXT_CONSOLE_HEROCREATE_CLASS_ATTRIBUTE).SetText(sAttribute, FALSE);
}

void DisplayClassSkillVideo(int32 eClass, BOOL bForce)
{
    if (!bForce && s_tHeroCreateData.nPlayingClassVideo == eClass)
    {
        return;
    }

    CRefString sCutscenePath;

    switch (eClass)
    {
    case PLAYER_CLASS_DEMONHUNTER:
        sCutscenePath = "Hero_Create_DHunter.ogv";
        break;
    case PLAYER_CLASS_BARBARIAN:
        sCutscenePath = "Hero_Create_Barb.ogv";
        break;
    case PLAYER_CLASS_WIZARD:
        sCutscenePath = "Hero_Create_Wiz.ogv";
        break;
    case PLAYER_CLASS_WITCHDOCTOR:
        sCutscenePath = "Hero_Create_WitchDoc.ogv";
        break;
    case PLAYER_CLASS_MONK:
        sCutscenePath = "Hero_Create_Monk.ogv";
        break;
    case PLAYER_CLASS_CRUSADER:
        sCutscenePath = "Hero_Create_Crusader.ogv";
        break;
    case PLAYER_CLASS_NECROMANCER:
        sCutscenePath = "Hero_Create_Necromancer.ogv";
        break;
    }

    if (sCutscenePath.Length())
    {
        UICutscene & ctrlSkillVideo = UI_GetCutsceneFromID(UIVID_CONSOLE_HEROCREATE_SKILL_VIDEO);
        ctrlSkillVideo.SetLooping(TRUE);
        ctrlSkillVideo.Play(sCutscenePath, FALSE);
        s_tHeroCreateData.nPlayingClassVideo = eClass;
    }
}






void StopVideo()
{
    UICutscene & ctrlSkillVideo = UI_GetCutsceneFromID(UIVID_CONSOLE_HEROCREATE_SKILL_VIDEO);
    ctrlSkillVideo.Stop();
}



void RestartVideo()
{
    if (UI_GetControlFromID(UIWND_CONSOLE_HEROCREATE).IsActive())
    {
        DisplayClassSkillVideo(s_tHeroCreateData.nPlayingClassVideo, TRUE);
    }
}



inline BOOL HasModalCooldownPassed()
{
    int32 nMillisecondsPassed = Blizzard::Time::Milliseconds() - s_tHeroCreateData.nModalOpenTime;
    return nMillisecondsPassed > HARDCORE_COOLDOWN_MILLISECS;
}



void RefreshResetWarning(const UIID & id)
{
    UIID idAcceptText, idAcceptButton;

    idAcceptText = UIIDCreateFromParent(id, ".AcceptText");
    idAcceptButton = UIIDCreateFromParent(id, ".Button3");

    UIWindow & tAcceptButton = UI_GetWindowFromID(idAcceptButton);
    UIText & tAcceptText = UI_GetTextFromID(idAcceptText);

    int32 nMillisecondsPassed = Blizzard::Time::Milliseconds() - s_tHeroCreateData.nModalOpenTime;

    if (HasModalCooldownPassed())
    {
        tAcceptButton.SetTransparency(0xff);
        tAcceptText.SetTransparency(0xff);

        tAcceptText.SetText(StringListFetch("Console:HardcoreAccept"), FALSE);
    }
    else
    {
        tAcceptButton.SetTransparency(0x40);
        tAcceptText.SetTransparency(0x40);

        CRefString sAcceptText;
        sAcceptText.Format("%s (%s)",
                           StringListFetch("Console:HardcoreAccept").str(),
                           UnicodeText::FormatNumber((HARDCORE_COOLDOWN_MILLISECS - nMillisecondsPassed) / 1000 + 1, TRUE, FNS_NORMAL, XLOCALE_INVALID).str());
        tAcceptText.SetText(sAcceptText, FALSE);
    }
}



void sSetHeroCreateActions()
{
    ::Console::ControllerActionManager & tActionManager = ::Console::Screens::GetActionManager();
    tActionManager.ClearAllActions();

    PlayerClass * parClasses = GetAvailablePlayerClasses();
    UIConsoleListBox & tClassSelection = UI_GetConsoleListBoxFromID(UILIST_CONSOLE_HEROCREATE_CLASSES);

    if (tClassSelection.GetSelectedIndex() != -1 &&
        parClasses[tClassSelection.GetSelectedIndex()] == PLAYER_CLASS_NECROMANCER && !bHaveNecroLicense)
    {
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_CONFIRM, StringListFetch("ConsoleHeroCreate:NecroUpsell"));
    }
    else
    {
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_CONFIRM, StringListFetch("Console:Accept"));
    }

    tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_BACK, StringListFetch("Console:Back"));

    if (!CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
    {
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_LEFT_STICK_VERTICAL, ACTION_CHANGE_HEROTYPE, StringListFetch("ConsoleHeroCreate:ChangeHeroType"));
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_LEFT_STICK_HORIZONTAL, ACTION_TOGGLE_GENDER, StringListFetch("ConsoleHeroCreate:ToggleGender"));
    }

    ::Console::Screens::UpdateButtonLegendText();
}









void OnUpdate(const UIID & id)
{
    s_tHeroCreateData.tAnimationManager.Update();

    // the necromancer may have been bought while the screen is open
    if (bHaveNecroLicense != ((::Console::DLCManager::GetLicenses() & (1 << LICENSE_NECROMANCER)) != 0))
    {
        bHaveNecroLicense = !bHaveNecroLicense;
        UIConsoleListBox & tListbox = UI_GetConsoleListBoxFromID(UILIST_CONSOLE_HEROCREATE_CLASSES);
        tListbox.ResetAllItems();
    }

    if (::Console::Online::IsSeasonsInitialized())
    {
        s_tHeroCreateData.fSeasonAvailable = OnlineService::GetSeasonState() == OnlineService::SEASON_STATE_ACTIVE;
    }
    else
    {
        s_tHeroCreateData.fSeasonAvailable = FALSE;
    }

    UIRadioButtonGroup & tHeroTypeSelection = UI_GetRadioButtonGroupFromID(UIRADIO_CONSOLE_HEROCREATE_HERO_TYPE);
    if (nLockedHeroType == -1)
    {
        tHeroTypeSelection.GetButtonByValue(UIHEROTYPE_SEASONAL)->SetActive(s_tHeroCreateData.fSeasonAvailable && !bPlayerIsGuest);
        tHeroTypeSelection.GetButtonByValue(UIHEROTYPE_SEASONAL_HARDCORE)->SetActive(s_tHeroCreateData.fSeasonAvailable && !bPlayerIsGuest);
    }
    else
    {
        for (int32 n = 0; n < UIHEROTYPE_COUNT; n++)
        {
            tHeroTypeSelection.GetButtonByValue(n)->SetActive(n == nLockedHeroType);
        }
        SetHeroType((UIHeroType)nLockedHeroType);
    }

    UI_GetWindowFromID(UIWND_CONSOLE_HEROCREATE_L1).SetActive(GameIsStartUp());
    UI_GetWindowFromID(UIWND_CONSOLE_HEROCREATE_R1).SetActive(GameIsStartUp());

    UIHeroType eHeroType = (UIHeroType)tHeroTypeSelection.GetSelectedValue();
    if ((int32)eHeroType >= UIHEROTYPE_SEASONAL && !s_tHeroCreateData.fSeasonAvailable)
    {
        tHeroTypeSelection.SetSelection(UIHEROTYPE_NORMAL, FALSE);
        UpdateTitle();
    }

    if (!UIWhatsNew::Console::IsVisible() && !UIWhatsNewSeason::Console::IsVisible() && !UIWhatsNewNX64::Console::IsVisible())
    {
        sSetHeroCreateActions();
    }
}
































void OnClassChanged(
    const UIID &,
    const int32,
    const int32 nNewVal)
{
    if (nNewVal == -1)
    {
        return;
    }
    PlayerClass * parClasses = GetAvailablePlayerClasses();
    PlayerClass eClass = parClasses[nNewVal];
    if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
    {

        UIRadioButtonGroup & tGenderSelection = UI_GetRadioButtonGroupFromID(UIRADIO_CONSOLE_HEROCREATE_GENDER);
        BOOL bFemale = eClass == PLAYER_CLASS_DEMONHUNTER || eClass == PLAYER_CLASS_WIZARD || eClass == PLAYER_CLASS_MONK;
        tGenderSelection.SetSelection(bFemale ? UIGENDER_FEMALE : UIGENDER_MALE, FALSE);
        LanguageRules::SetClientGender(!bFemale);
    }
    BOOL bIsFemale = UI_GetRadioButtonGroupFromID(UIRADIO_CONSOLE_HEROCREATE_GENDER).GetSelectedValue();
    BOOL bIsHardcore = FALSE;
    if (GameIsStartUp())
    {
        BackgroundHeroDescription tHero;
        tHero.SetDemoHero(eClass, bIsFemale, bIsHardcore);
        ::Console::Screens::GetFrontendBackgroundHeroManager().ChangeBackgroundHero(0, tHero);
    }
    else
    {
        SNO snoActor = GetHeroCreateCharacterSNO(eClass, bIsFemale);
        UIRActor & tHeroRActor = UI_GetRActorFromID(UIRACTOR_CONSOLE_HEROCREATE_HERO);
        tHeroRActor.DeleteRActor();
        PRTransform transform(cgqIdentity, cgwpNull);
        RActorID idRActorNew = RActorCreate(snoActor, 1.0f, -1.0f, transform, 0, tHeroRActor.tRenderData[CGameAsyncGetRenderDataIndex(0)].m_tDisplayList.GetRenderContext(), -1);
        tHeroRActor.SetRActor(idRActorNew);
        RActor * ptRActor = RActorTryToGet(idRActorNew);
        if (ptRActor)
        {
            RActorSetIrradianceMap(ptRActor, GlobalSNOGet(GLOBALSNO_RACTOR_NEUTRAL_IRRADIANCE_TEXTURE));
        }
        s_tHeroCreateData.tAnimationManager.StartAnimation(idRActorNew);
    }

    UpdateClassDescription(eClass);
    DisplayClassSkillVideo(eClass, FALSE);
}


static void sSetupSoundForScreen()
{
    GamePause(TRUE);

    SoundManager::ListenerGetVolumePosition(s_tHeroCreateData.tSavedListener.wpVolumePos);
    SoundManager::ListenerGetPanningPosition(s_tHeroCreateData.tSavedListener.wpPanningPos);
    SoundManager::ListenerGetAxes(s_tHeroCreateData.tSavedListener.wvView, s_tHeroCreateData.tSavedListener.wvUp, s_tHeroCreateData.tSavedListener.wvRight);

    {
        UIRActor & tHeroRActor = UI_GetRActorFromID(UIRACTOR_CONSOLE_HEROCREATE_HERO);
        ScopedObserverDefinitionLockR pdefObserver(tHeroRActor.GetObserver());
        Vector3D wpLookTo(cgwpNull);

        Vector3D wpLookFrom = wpLookTo + pdefObserver->wvCameraOffsetVector;


        PRTransform transform;
        transform.Set(wpLookFrom, wpLookTo + pdefObserver->wvLookAtOffsetVector, cgwvZAxis);
        ListenerSetMicrophoneLocation(transform, transform, pdefObserver->flRolloffFactor, 1.0f);
    }
    SoundManager::SetCustomListenerMode(TRUE);
    s_tHeroCreateData.tSavedListener.fOverridden = TRUE;
}

static void sRestoreSoundForScreen()
{
    if (s_tHeroCreateData.tSavedListener.fOverridden)
    {
        SoundManager::SetCustomListenerMode(FALSE);
        SoundManager::ListenerSet(s_tHeroCreateData.tSavedListener.wpVolumePos,
                                  s_tHeroCreateData.tSavedListener.wpPanningPos,
                                  s_tHeroCreateData.tSavedListener.wvView,
                                  s_tHeroCreateData.tSavedListener.wvUp);
        s_tHeroCreateData.tSavedListener.fOverridden = FALSE;
    }
}






BOOL CanCloseSafely()
{

    return !UI_GetControlFromID(UIWND_CONSOLE_HEROCREATE).IsActive();
}


void SetHeroType(UIHeroType eType)
{
    UIRadioButtonGroup & tHeroTypeSelection = UI_GetRadioButtonGroupFromID(UIRADIO_CONSOLE_HEROCREATE_HERO_TYPE);
    int32 nSelectedValue = tHeroTypeSelection.GetSelectedValue();
    tHeroTypeSelection.SetSelection(eType, FALSE);

    UIText & tHeroTypeTitle = UI_GetTextFromID(UITXT_CONSOLE_HEROCREATE_TYPE_TITLE);
    UIText & tHeroTypeDesc = UI_GetTextFromID(UITXT_CONSOLE_HEROCREATE_TYPE_DESC);
    switch (eType)
    {
    case UIHEROTYPE_NORMAL:
        tHeroTypeTitle.SetText(StringListFetch("ConsoleHeroCreate:StandardHeroTitle"), FALSE);
        tHeroTypeDesc.SetText(StringListFetch("ConsoleHeroCreate:StandardHeroDescription"), FALSE);
        s_tHeroCreateData.fCreateHardcore = FALSE;
        s_tHeroCreateData.fCreateSeasonal = FALSE;
        break;
    case UIHEROTYPE_HARDCORE:
        tHeroTypeTitle.SetText(StringListFetch("ConsoleHeroCreate:HardcoreHeroTitle"), FALSE);
        tHeroTypeDesc.SetText(StringListFetch("ConsoleHeroCreate:HardcoreHeroDescription"), FALSE);
        s_tHeroCreateData.fCreateHardcore = TRUE;
        s_tHeroCreateData.fCreateSeasonal = FALSE;
        break;
    case UIHEROTYPE_SEASONAL:
        tHeroTypeTitle.SetText(StringListFetch("ConsoleHeroCreate:SeasonalHeroTitle"), FALSE);
        tHeroTypeDesc.SetText(StringListFetch("ConsoleHeroCreate:SeasonalHeroDescription"), FALSE);
        s_tHeroCreateData.fCreateHardcore = FALSE;
        s_tHeroCreateData.fCreateSeasonal = TRUE;
        break;
    case UIHEROTYPE_SEASONAL_HARDCORE:
        tHeroTypeTitle.SetText(StringListFetch("ConsoleHeroCreate:HardcoreSeasonalHeroTitle"), FALSE);
        tHeroTypeDesc.SetText(StringListFetch("ConsoleHeroCreate:HardcoreSeasonalHeroDescription"), FALSE);
        s_tHeroCreateData.fCreateHardcore = TRUE;
        s_tHeroCreateData.fCreateSeasonal = TRUE;
        break;
    }

    UpdateTitle();
}






void OpenForController(int32 nControllerIndex)
{
    s_tHeroCreateData.bInputEnabled = TRUE;
    s_tHeroCreateData.tSavedListener.fOverridden = FALSE;
    s_tHeroCreateData.nControllerIndex = nControllerIndex;

    if (::Console::GamerProfile::GetPrimaryProfileUserIndex() != nControllerIndex)
    {
        bIsAddingLocalPlayer = TRUE;
    }

    bHaveNecroLicense = (::Console::DLCManager::GetLicenses() & (1 << LICENSE_NECROMANCER)) != 0;

    if (!GameIsStartUp())
    {
        ::Console::Screens::ShowModalDialog(nControllerIndex, UIWND_CONSOLE_HEROCREATE, TRUE, GetUIIDNone());
        if (PlayersAreAllLocal())
        {
            sSetupSoundForScreen();
        }
    }
    else
    {
        UI_GetControlFromID(UIWND_CONSOLE_HEROCREATE).SetActive(TRUE);
    }

    UI_SetControllerFocus(UILIST_CONSOLE_HEROCREATE_CLASSES);

    RNG & rng = CGameGlobalsGet().rngLocal;

    if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
    {
        UI_GetControlFromID(UITXT_CONSOLE_HEROCREATE_MALE).SetActive(FALSE);
        UI_GetControlFromID(UITXT_CONSOLE_HEROCREATE_FEMALE).SetActive(FALSE);
        UI_GetControlFromID(UIRADIOBTN_CONSOLE_HEROCREATE_MALE).SetActive(FALSE);
        UI_GetControlFromID(UIRADIOBTN_CONSOLE_HEROCREATE_FEMALE).SetActive(FALSE);
        UI_GetTextFromID(UITXT_CONSOLE_HEROCREATE_TITLE).SetText(StringListFetch("ConsoleDemo:Demo").str(), FALSE);
        s_tHeroCreateData.fUsePAXBoost35 = FALSE;
    }

    if (GameIsStartUp())
    {
        SNO snoWorld = GetActBackgroundSNO(ACT_1);
        if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
        {
            snoWorld = GetActBackgroundSNO(ACT_5);
        }
        UIStartup_SetBackgroundWorld(snoWorld);

        FrontendBackgroundCameraManager & tCameraManager = ::Console::Screens::GetFrontendBackgroundCameraManager();
        if (tCameraManager.GetBackgroundCameraPosition() != CAM_POS_CLOSE ||
            (tCameraManager.GetTargetBackgroundCameraPosition() != CAM_POS_NONE && tCameraManager.GetTargetBackgroundCameraPosition() != CAM_POS_CLOSE))
        {
            tCameraManager.TransitionBackgroundCamera(CAM_POS_CLOSE, 42.0f);
            SNO sound = GlobalSNOGet(GLOBALSNO_SOUND_CAMERA_ZOOMIN);
            SoundManager::PlaySound(sound, cgtPSODefaults);
        }

        FrontendBackgroundHeroManager & tHeroManager = ::Console::Screens::GetFrontendBackgroundHeroManager();
        tHeroManager.ClearBackgroundHero(HERO_SLOT_2);
        tHeroManager.ClearBackgroundHero(HERO_SLOT_3);
        tHeroManager.ClearBackgroundHero(HERO_SLOT_4);
        tHeroManager.ClearBackgroundBanner();
    }

    UIRActor & tFilagree = UI_GetRActorFromID(UIRACTOR_CONSOLE_HEROCREATE_FILAGREE);
    PRTransform transform(cgqIdentity, cgwpNull);
    RActorID idRActorNew = RActorCreate(GlobalSNOGet(GLOBALSNO_ACTOR_CONSOLE_MENU_FILAGREE), 1.0f, -1.0f, transform, 0, tFilagree.tRenderData[CGameAsyncGetRenderDataIndex(0)].m_tDisplayList.GetRenderContext(), -1);
    tFilagree.SetRActor(idRActorNew);

    s_tHeroCreateData.nPlayingClassVideo = -1;

    if (GameIsStartUp())
    {
        s_tHeroCreateData.fCreateHardcore = FALSE;
        nLockedHeroType = -1;

        bPlayerIsGuest = ::Console::GamerProfile::IsGuest(nControllerIndex);
        if (bPlayerIsGuest)
        {
            NotificationCreationParams tParams;
            tParams.nControllerIndex = nControllerIndex;
            tParams.uszMessage = StringListFetch("ConsoleHeroCreate:SeasonalGuestWarning");
            ::Console::Screens::GetModalNotificationManager().SetOkNotification(tParams);
        }
    }
    else
    {
        AppGlobals & tAppGlobals = sgtAppGlobals;
        s_tHeroCreateData.fCreateHardcore = (tAppGlobals.dwGameCreateParamFlags & (1 << GAMECREATEPARAM_HARDCORE)) != 0;
        s_tHeroCreateData.fCreateSeasonal = (tAppGlobals.dwGameCreateParamFlags & (1 << GAMECREATEPARAM_SEASON)) != 0;
        nLockedHeroType = s_tHeroCreateData.fCreateHardcore | (s_tHeroCreateData.fCreateSeasonal << 1);
        s_tHeroCreateData.idInGameHeroLight = LightAdd(0, UI_GetDefaultHeroLight(), PRTransform(cgqIdentity, cgwpNull), FALSE, RENDERCONTEXT_HUD_0);
    }

    UpdateTitle();

    UIRadioButtonGroup & tGenderSelection = UI_GetRadioButtonGroupFromID(UIRADIO_CONSOLE_HEROCREATE_GENDER);
    if (!CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
    {
        UIGender eGender = (UIGender)RandRange_Internal(rng, UIGENDER_MALE, UIGENDER_FEMALE - UIGENDER_MALE);
        if (eGender == UIGENDER_MALE)
        {
            UI_GetTextFromID(UIRADIO_CONSOLE_HEROCREATE_GENDER_MALE).SetTextColor((FontSpecifier)0);
            UI_GetTextFromID(UIRADIO_CONSOLE_HEROCREATE_GENDER_FEMALE).SetTextColor((FontSpecifier)7);
        }
        else
        {
            UI_GetTextFromID(UIRADIO_CONSOLE_HEROCREATE_GENDER_MALE).SetTextColor((FontSpecifier)7);
            UI_GetTextFromID(UIRADIO_CONSOLE_HEROCREATE_GENDER_FEMALE).SetTextColor((FontSpecifier)0);
        }
        tGenderSelection.SetSelection(eGender, FALSE);
    }
    else
    {
        tGenderSelection.SetSelection(UIGENDER_MALE, FALSE);
    }
    LanguageRules::SetClientGender(tGenderSelection.GetSelectedValue() == UIGENDER_MALE);

    if (::Console::Online::IsSeasonsInitialized() && OnlineService::GetSeasonState() == OnlineService::SEASON_STATE_ACTIVE && !bPlayerIsGuest)
    {
        SetHeroType(UIHEROTYPE_SEASONAL);
    }
    else
    {
        SetHeroType(UIHEROTYPE_NORMAL);
    }

    UIConsoleListBox & tListbox = UI_GetConsoleListBoxFromID(UILIST_CONSOLE_HEROCREATE_CLASSES);
    tListbox.SetDataSource(s_tHeroCreateData.tHeroCreateDataSource);
    tListbox.ResetAllItems();
    if (!CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
    {
        tListbox.SetSelectedIndex(RandRange_Internal(rng, 0, (sizeof(arClasses) / sizeof(arClasses[0])) - 1));
    }

    sSetHeroCreateActions();

    uint32 nNumMembers = ::Console::Online::PartyGetMemberCount();
    UIText & tPlayerNumber = UI_GetTextFromID(UITXT_CONSOLE_HEROCREATE_PLAYER_NUMBER);
    if (nNumMembers > 1 && ::Console::Online::PartyIsInParty(nControllerIndex) && !CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
    {
        tPlayerNumber.SetText(UIUtils::Console::PlayerGetDecoratedPlayerNum(nControllerIndex), FALSE);
        tPlayerNumber.SetActive(TRUE);
    }
    else
    {
        tPlayerNumber.SetActive(FALSE);
    }

    UI_GetTextFromID(UITXT_CONSOLE_HEROCREATE_GENDER_BUTTON).SetText("{icon:l_stick_H_Button,2.0}", FALSE);
}



































void Close()
{
    bIsAddingLocalPlayer = FALSE;

    if (GameIsPaused())
    {
        GamePause(FALSE);
    }

    // put the sound listener back where the game had it
    sRestoreSoundForScreen();

    s_tHeroCreateData.bInputEnabled = TRUE;

    if (GameIsStartUp() && UI_GetWindowFromID(UIWND_CONSOLE_HEROCREATE_HARDCORE_WARNING).IsActive())
    {
        ::Console::Screens::CloseModalDialog(UIWND_CONSOLE_HEROCREATE_HARDCORE_WARNING);
    }

    UI_SetControllerFocus(GetUIIDNone());
    UI_GetCutsceneFromID(UIVID_CONSOLE_HEROCREATE_SKILL_VIDEO).Stop();

    if (GameIsStartUp())
    {
        UI_GetControlFromID(UIWND_CONSOLE_HEROCREATE).SetActive(FALSE);
    }
    else
    {
        ::Console::Screens::CloseModalDialog(UIWND_CONSOLE_HEROCREATE);
    }

    if (s_tHeroCreateData.idInGameHeroLight != -1)
    {
        LightDelete(s_tHeroCreateData.idInGameHeroLight);
        s_tHeroCreateData.idInGameHeroLight = -1;
    }

    UI_GetRActorFromID(UIRACTOR_CONSOLE_HEROCREATE_HERO).DeleteRActor();
    UI_GetRActorFromID(UIRACTOR_CONSOLE_HEROCREATE_FILAGREE).DeleteRActor();
}










void OnBack(const UIID &, const int32 nControllerIndex)
{
    if (::Console::Screens::IsScreenTransitioning() || !s_tHeroCreateData.bInputEnabled)
    {
        return;
    }

    if (::Console::GamerProfile::IsHeroLoaded(nControllerIndex))
    {
        ::Console::Screens::GoToPreviousScreenSet(nControllerIndex, FALSE);
        return;
    }

    if (::Console::GamerProfile::GetPrimaryProfileUserIndex() == nControllerIndex)
    {
        ::Console::Screens::ClearScreenSetHistory();
        ::Console::Screens::OpenToScreen((::Console::Screens::ScreenSets)::Console::Screens::MAIN_MENU_SCREEN_SET, 0, nControllerIndex, FALSE);

        int32 nInviteUserIdx = -1;
        if (::Console::Online::HasOutstandingInvite(nInviteUserIdx) && nInviteUserIdx == nControllerIndex)
        {
            ::Console::Online::ClearOutstandingInvite();
        }
    }
    else
    {
        if (::Console::Online::PartyIsInParty(nControllerIndex))
        {
            ::Console::Online::PartyRemovePlayer(nControllerIndex);
        }

        ::Console::Screens::ClearScreenSetHistory();
        if (GameIsStartUp())
        {
            ::Console::Screens::OpenToScreen((::Console::Screens::ScreenSets)::Console::Screens::MAIN_MENU_SCREEN_SET,
                                             1,
                                             ::Console::GamerProfile::GetPrimaryProfileUserIndex(),
                                             FALSE);
        }
        else
        {
            ::Console::Screens::CloseScreenSet(TRUE, FALSE);
        }
    }

    ::Console::GamerProfile::SignOut(nControllerIndex);
}

CRefString GenerateTempName(AffixType eType);
















// RandRange_Internal min 1 / delta nNameCount: the +1 sits on Random.h:189 (inside RandRange_Internal) and nDelta is the count itself (add w22, w21, #1 = nMax)
CRefString GenerateTempName(
    AffixType eType)
{
    GBHandleList listNames(GBGetHandlePool());
    GBEnumerate(GB_MONSTER_NAMES, listNames);


    POS pos = listNames.GetHeadPosition();
    while (pos)
    {
        POS posPrev = pos;

        const GBHandle & hGBName = listNames.GetNext(pos);
        ScopedRareMonsterNameEntryLockR pName(hGBName.gbid);

        if (!pName || pName->eAffixType != eType)
        {
            listNames.RemoveAt(posPrev);
        }
    }


    int32 nNameCount = listNames.Count();
    if (nNameCount == 0)
    {
        return CRefString("nil");
    }

    int32 nCount = RandRange_Internal(CGameGlobalsGet().rngLocal, 1, nNameCount);

    GBID gbidName = GBID_NONE;
    pos = listNames.GetHeadPosition();
    for (int32 i = 0; i < nCount && pos; i++)
    {
        gbidName = listNames.GetNext(pos).gbid;
    }


    UIRadioButtonGroup & tGenderSelection = UI_GetRadioButtonGroupFromID(UIRADIO_CONSOLE_HEROCREATE_GENDER);
    BOOL fIsFemale = tGenderSelection.GetSelectedValue();

    ScopedRareMonsterNameEntryLockR pName(gbidName);

    if (pName == NULL)
    {
        return CRefString("!!NIL!!");
    }

    uint32 dwGrammaticalForm =
        1 << (fIsFemale ? GF_FEMININE_BIT : GF_MASCULINE_BIT);

    CRefString usRetval;
    uint32 dwExtractionFlagsIgnored;
    if (StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_MONSTER_NAMES), pName->tHeader.szName, usRetval))
    {
        LanguageRules::ExtractMatchingGrammaticalForm(usRetval, dwGrammaticalForm, dwExtractionFlagsIgnored);
    }

    return usRetval;

}


CRefString GenerateRandomName(PlayerClass eClass, BOOL bIsFemale)
{
    LPCSTR szClass = SymbolToName(eClass, gPlayerClasses);
    LPCSTR szGender = SymbolToName(bIsFemale != 0, gActorGenderNames);
    CRefString sStringListName = CRefString().Format("PlayerNames_%s_%s", szClass, szGender);

    SNO snoStringList = SNOFileLookupGUID(STRINGLIST_SNO, sStringListName);
    ScopedStringListDefinitionLockR pHeroNames(snoStringList);
    CRefString sRandomName = "Xenon";
    if (pHeroNames && pHeroNames->dwStringCount)
    {
        int32 nRand = RandRange_Internal(CGameGlobalsGet().rngLocal, 0, pHeroNames->dwStringCount - 1);
        sRandomName = (char *)((StringTableEntry *)pHeroNames->arStrings)[nRand].szText;
    }
    else
    {
        // no name list: make a rare-monster style name that is short and not taken
        do
        {
            do
            {
                CRefString sPrefix = GenerateTempName(0);
                sRandomName = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GENERAL), "MonsterRare");
                sRandomName = StringListEntryReplace(sRandomName, "{PREFIX}", sPrefix, 999);
                sRandomName = StringListEntryReplace(sRandomName, "{SUFFIX}", "", 999);
            } while (SNOFileLookupGUID(HERO_SNO, sRandomName.str()) != SNO_NONE);
        } while (sRandomName.Length() > 12);
    }

    return sRandomName;
}




void OnLocalPartyMemberAdded(int32 nControllerIndex, BOOL bSuccess)
{
    bIsAddingLocalPlayer = FALSE;

    if (GameIsStartUp())
    {
        if (::Console::Online::PartyIsHost() &&
            ::Console::GamerProfile::GetPrimaryProfileUserIndex() == nControllerIndex &&
            !CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
        {
            ::Console::Online::GameParams tGameParams;
            if (::Console::Online::PartyGetGameParams(tGameParams))
            {
                const D3::Account::SavedDefinition & tAccountDefinition = ::Console::GamerProfile::GetAccount(nControllerIndex);
                const D3::Hero::SavedDefinition & tHeroDefinition = ::Console::GamerProfile::GetHero(nControllerIndex);
                ::Console::UIOnlineActions::SetGameParamsForHero(tAccountDefinition, tHeroDefinition, tGameParams);
                ::Console::Online::PartySetGameParams(tGameParams);
            }
        }

        if (!CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
        {
            ::Console::Screens::OpenToScreen((::Console::Screens::ScreenSets)::Console::Screens::MAIN_MENU_SCREEN_SET,
                                             1,
                                             ::Console::GamerProfile::GetPrimaryProfileUserIndex(),
                                             FALSE);
        }
    }
    else
    {
        ::Console::Screens::CloseScreenSet(TRUE, FALSE);
    }
}
















void OnInvalidHeroWarningClosed(int32 nControllerIndex, PlayerClass eClass, uint32 uFlags, CRefString sName, const UIID &)
{
    // an empty name gets a fresh random one
    if (sName.Length() == 0)
    {
        sName = GenerateRandomName(eClass, (uFlags & (1 << HEROCREATE_FLAG_FEMALE)) != 0);
    }

    // ask for the name again



    EnableInput(FALSE);






    ::Console::Utilities::ShowKeyboardForUser(nControllerIndex, sName, StringListFetch("ConsoleHeroCreate:HeroNameTitle"), StringListFetch("ConsoleHeroCreate:HeroNameDesc"), 12,
                                              OnlineService::internal::function_binder_5<void, int32, PlayerClass, uint32, BOOL, CRefString>::bind<&OnHeroNameEntered>(nControllerIndex, eClass, uFlags));
}


void OnSaveComplete(int32 nControllerIndex)
{
    EnableInput(TRUE);
    UI_GetTextFromID(UITXT_CONSOLE_HEROCREATE_STATUS).SetText("", FALSE);

    if (::Console::GamerProfile::IsActive(nControllerIndex))
    {
        if (!::Console::Online::PartyIsInParty())
        {
            UIGameSettings::Console::ClearGameSettingsViewed();
            ::Console::Screens::OpenToScreen((::Console::Screens::ScreenSets)::Console::Screens::MAIN_MENU_SCREEN_SET,
                                             1,
                                             ::Console::GamerProfile::GetPrimaryProfileUserIndex(),
                                             FALSE);
        }
        else if (::Console::Online::PartyIsInParty(nControllerIndex))
        {
            ::Console::Online::PartyUpdatePlayerHero(nControllerIndex);
            OnLocalPartyMemberAdded(nControllerIndex, TRUE);
        }
        else
        {
            ::Console::UIOnlineActions::FinalizeLocalPlayerJoin(nControllerIndex,
                OnlineService::internal::function_binder_2<void, int32, BOOL>::bind<&OnLocalPartyMemberAdded>(nControllerIndex));
        }
    }
}











void OnHeroNameEntered(int32 nControllerIndex, PlayerClass eClass, uint32 uFlags, BOOL bSuccess, CRefString sString)
{
    EnableInput(TRUE);

    if (bSuccess && ::Console::Screens::IsScreenOpen(::Console::Screens::HERO_CREATE_SCREEN_SET, 0))
    {
        if (CreateHero(nControllerIndex, eClass, uFlags, sString))
        {
            EnableInput(FALSE);
        }
    }
}














static void FillBlizzconVisualEquipment(D3::Hero::VisualEquipment * visualEquipment, PlayerClass eClass)
{
    VisualEquipment equipment;




    equipment.Import(*visualEquipment);
    auto SetItem = [&equipment](int32 nSlot, LPCSTR szItemGBID)
    {
        equipment.tVisualItem[nSlot].Set(StringToGBID(szItemGBID), 0, 0, 0);
    };



    switch (eClass)
    {
    case PLAYER_CLASS_DEMONHUNTER:

        SetItem(0, "Unique_Helm_Set_03_p2");
        SetItem(1, "Unique_Chest_Set_03_p2");
        SetItem(2, "Unique_Boots_Set_03_p2");
        SetItem(3, "Unique_Gloves_Set_03_p2");
        SetItem(4, "P61_Unique_Bow_104_x1");
        SetItem(5, "P61_Unique_Quiver_007");
        SetItem(6, "Unique_Shoulder_Set_03_p2");
        SetItem(7, "Unique_Pants_Set_03_p2");
        break;

    case PLAYER_CLASS_BARBARIAN:

        SetItem(0, "Unique_Helm_Set_01_p2");
        SetItem(1, "Unique_Chest_Set_01_p2");
        SetItem(2, "Unique_Boots_Set_01_p2");
        SetItem(3, "Unique_Gloves_Set_01_p2");
        SetItem(4, "Unique_Mighty_1H_010_x1");
        SetItem(5, "Unique_Mighty_1H_011_x1");
        SetItem(6, "Unique_Shoulder_Set_01_p2");
        SetItem(7, "Unique_Pants_Set_01_p2");
        break;

    case PLAYER_CLASS_WIZARD:

        SetItem(0, "Unique_Helm_010_x1");
        SetItem(1, "Unique_Chest_014_x1");
        SetItem(2, "P61_Unique_Boots_01");
        SetItem(3, "P2_Unique_Gloves_02");
        SetItem(4, "P61_Unique_Sword_1H_112_x1");
        SetItem(5, "P61_Unique_Orb_002");
        SetItem(6, "P4_Unique_Shoulder_103");
        SetItem(7, "P2_Unique_Pants_03");
        break;

    case PLAYER_CLASS_WITCHDOCTOR:

        SetItem(0, "Unique_Helm_Set_16_x1");
        SetItem(1, "Unique_Chest_Set_16_x1");
        SetItem(2, "Unique_Boots_Set_16_x1");
        SetItem(3, "Unique_Gloves_Set_16_x1");
        SetItem(4, "Unique_Staff_104_x1");
        // slot 5: the staff is two-handed
        SetItem(6, "Unique_Shoulder_Set_16_x1");
        SetItem(7, "Unique_Pants_Set_16_x1");
        break;

    case PLAYER_CLASS_MONK:

        SetItem(0, "Unique_SpiritStone_007_x1");
        SetItem(1, "Unique_Chest_Set_11_x1");
        SetItem(2, "Unique_Boots_010_x1");
        SetItem(3, "Unique_Gloves_Set_11_x1");
        SetItem(4, "P4_Unique_Fist_102");
        SetItem(5, "P4_fistWeapon_norm_unique_02");
        SetItem(6, "Unique_Shoulder_Set_11_x1");
        SetItem(7, "Unique_Pants_Set_11_x1");
        break;

    case PLAYER_CLASS_CRUSADER:

        SetItem(0, "Unique_Helm_Set_10_x1");
        SetItem(1, "Unique_Chest_Set_10_x1");
        SetItem(2, "Unique_Boots_Set_10_x1");
        SetItem(3, "Unique_Gloves_Set_10_x1");
        SetItem(4, "P61_Unique_Flail_1H_105_x1");
        SetItem(5, "P61_Unique_CruShield_102_x1");
        SetItem(6, "Unique_Shoulder_Set_10_x1");
        SetItem(7, "Unique_Pants_Set_10_x1");
        break;

    case PLAYER_CLASS_NECROMANCER:

        SetItem(0, "P6_Necro_Set_3_Helm");
        SetItem(1, "P6_Necro_Set_3_Chest");
        SetItem(2, "P6_Necro_Set_3_Boots");
        SetItem(3, "P6_Necro_Set_3_Gloves");
        SetItem(4, "P6_Unique_Scythe1H_02");
        SetItem(5, "P61_Unique_Phylactery_01");
        SetItem(6, "P6_Necro_Set_3_Shoulders");
        SetItem(7, "P6_Necro_Set_3_Pants");
        break;
    }


    equipment.Export(*visualEquipment);
}


BOOL CreateHero(int32 nControllerIndex, PlayerClass eClass, uint32 uFlags, CRefString sString)




{
    if (!::Console::GamerProfile::GetAccount(nControllerIndex).has_digest())
    {
        return FALSE;
    }


    if ((uFlags & (1 << HEROCREATE_FLAG_SEASONAL)) && (!::Console::Online::IsSeasonsInitialized() || OnlineService::GetSeasonState() != OnlineService::SEASON_STATE_ACTIVE))
    {
        return FALSE;
    }

    UIText_RemoveFormattingCharacters(sString, FALSE, TRUE);

    LPCSTR szStringStart = sString.str();
    while ((size_t)(szStringStart - sString.str()) < sString.Length() && *szStringStart == ' ')
    {
        szStringStart++;
    }
    if (szStringStart > sString.str())
    {
        sString = CRefString(szStringStart);
    }

    CRefString szErrorMessage;

    int32 nStrLen = sString.Length();
    if (nStrLen < 1 || HasOnlySpaces(sString))
    {
        NotificationCreationParams tParams;
        tParams.nControllerIndex = nControllerIndex;
        tParams.uszTitle = StringListFetch("Bnet_Notifications:ErrorTitle");
        tParams.uszMessage = StringListFetch("ConsoleErrors:HeroNameErrorZeroLength");
        tParams.tCallbacks.pfnOnClickOkButton = OnlineService::internal::function_binder_5<void, int32, PlayerClass, uint32, CRefString, const UIID &>::bind<&OnInvalidHeroWarningClosed>(nControllerIndex, eClass, uFlags, CRefString());
        ::Console::Screens::GetModalNotificationManager().SetOkNotification(tParams);
        return FALSE;
    }

    size_t nNumChars = 0;
    Blizzard::Unicode::CountUTF8Chars(sString.str(), nStrLen, nNumChars);


    if (nNumChars > MAX_HERO_NAME_LENGTH)
    {
        NotificationCreationParams tParams;
        tParams.nControllerIndex = nControllerIndex;
        tParams.uszTitle = StringListFetch("Bnet_Notifications:ErrorTitle");
        tParams.uszMessage = LocalizedSprintf(
            StringListFetch("ConsoleErrors:HeroNameErrorTooLong").str(),
            UnicodeText::FormatNumber(MAX_HERO_NAME_LENGTH, TRUE, FNS_NORMAL, XLOCALE_INVALID).str(), NULL);

        tParams.tCallbacks.pfnOnClickOkButton = OnlineService::internal::function_binder_5<void, int32, PlayerClass, uint32, CRefString, const UIID &>::bind<&OnInvalidHeroWarningClosed>(nControllerIndex, eClass, uFlags, sString);
        ::Console::Screens::GetModalNotificationManager().SetOkNotification(tParams);
        return FALSE;
    }

    D3::Hero::SavedDefinition tHeroDefinition;
    HeroDefinition_Init(tHeroDefinition);



    uint32 dwPlayerFlags = uFlags & ((1 << HEROCREATE_FLAG_HARDCORE) | (1 << HEROCREATE_FLAG_FEMALE));

    OnlineService::HeroId idHero = ::Console::GamerProfile::GenerateHeroID(nControllerIndex, sString.str());

    D3::Hero::Digest & tDigest = *tHeroDefinition.mutable_digest();
    tDigest.set_hero_name(sString.str(), nStrLen);
    tDigest.set_gbid_class(PlayerClassToGBID(eClass));
    tDigest.set_player_flags(dwPlayerFlags);
    tDigest.set_create_time(Blizzard::Time::ToUnixTime(Blizzard::Time::GetTimestamp()));

    if (uFlags & (1 << HEROCREATE_FLAG_SEASONAL))
    {
        tDigest.set_season_created(uCachedSeasonAtConfirmation);
    }













    if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
    {
        D3::AttributeSerializer::SavedAttribute * ptAttribute = tHeroDefinition.mutable_saved_attributes()->add_attributes();
        ptAttribute->set_key(-4016);
        ptAttribute->set_value(69);
        HeroDefinitionSavedAttributeOverwriteInt(*tHeroDefinition.mutable_saved_attributes(), FastAttribKey(ATTRIBUTE_LEVEL), 69);

        tHeroDefinition.mutable_digest()->set_level(70);
        D3::Hero::VisualEquipment * visual_equipment = tHeroDefinition.mutable_digest()->mutable_visual_equipment();
        FillBlizzconVisualEquipment(visual_equipment, eClass);
    }

    tDigest.mutable_hero_id()->set_id_high(0);
    tDigest.mutable_hero_id()->set_id_low(idHero);

    tHeroDefinition.set_version(gptGroupDefHero->GetCurrentVersion(geSigmaPlatform));

    GBHandle gbHandle(GB_HEROS, PlayerClassToGBID(eClass));
    gbHandle.Export(*tHeroDefinition.mutable_saved_attributes()->mutable_gb_handle());

    ::Console::GamerProfile::SetHero(nControllerIndex, tHeroDefinition);


    D3::Account::SavedDefinition & tAccountDefinition = ::Console::GamerProfile::GetAccount(nControllerIndex);

    *tAccountDefinition.mutable_digest()->mutable_last_played_hero_id() = tHeroDefinition.digest().hero_id();


    if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT) && !::Console::Online::PartyIsInParty(nControllerIndex))
    {
        ::Console::UIOnlineActions::FinalizeLocalPlayerJoin(nControllerIndex,
            OnlineService::internal::function_binder_2<void, int32, BOOL>::bind<&OnLocalPartyMemberAdded>(nControllerIndex));
    }
    else
    {
        UI_GetTextFromID(UITXT_CONSOLE_HEROCREATE_STATUS).SetText(StringListFetch("ConsoleHeroCreate:CreatingHero"), FALSE);

        ::Console::GamerProfile::Save(nControllerIndex, OnlineService::internal::function_binder_1<void, int32>::bind<&OnSaveComplete>(nControllerIndex));

        if (::Console::GamerProfile::GetPrimaryProfileUserIndex() == nControllerIndex && !Prefs::GetFirstTimeFlowCompleted())
        {
            Prefs::GetFirstTimeFlowCompleted() = TRUE;
            ::Console::GamerProfile::SavePrefs();
        }
    }

    return TRUE;
}


inline void sSeasonConfirmationCallback(BOOL bInProgress)
{
    if (bRequestSeasonConfirmation)
    {
        bSeasonConfirmedAvailable = bInProgress;
        bRequestSeasonConfirmation = FALSE;
        uCachedSeasonAtConfirmation = OnlineService::GetSeasonNum();
    }
}






void OnButton1(const UIID &, const int32 nControllerIndex)
{

    UIRadioButtonGroup & tHeroTypeSelection = UI_GetRadioButtonGroupFromID(UIRADIO_CONSOLE_HEROCREATE_HERO_TYPE);
    UIHeroType eHeroType = (UIHeroType)tHeroTypeSelection.GetSelectedValue();

    UIText & tModalTitle = UI_GetTextFromID(UITXT_CONSOLE_HEROCREATE_HARDCORE_WARNING_TITLE);
    UIText & tModalDesc = UI_GetTextFromID(UITXT_CONSOLE_HEROCREATE_HARDCORE_WARNING_DESC);



    if (!bHaveNecroLicense)
    {
        PlayerClass * parClasses = GetAvailablePlayerClasses();
        UIConsoleListBox & tClassSelection = UI_GetConsoleListBoxFromID(UILIST_CONSOLE_HEROCREATE_CLASSES);
        if (tClassSelection.GetSelectedIndex() != -1 && parClasses[tClassSelection.GetSelectedIndex()] == PLAYER_CLASS_NECROMANCER)
        {
            UIMainMenu::Console::OpenStoreToNecromancer(nControllerIndex);
            return;
        }
    }

    switch (eHeroType)
    {
    case UIHEROTYPE_NORMAL:
        OnAccept(GetUIIDNone(), nControllerIndex);
        return;

    case UIHEROTYPE_HARDCORE:
        tModalTitle.SetText(StringListFetch("ConsoleHeroCreate:HardcoreModalWarningTitle"), FALSE);
        tModalTitle.SetTextColor(RGBAColor(0x3f, 0x00, 0x00, 0xff));
        tModalDesc.SetText(StringListFetch("ConsoleHeroCreate:HardcoreModalWarningDescription"), FALSE);
        ::Console::Screens::ShowModalDialog(nControllerIndex, UIWND_CONSOLE_HEROCREATE_HARDCORE_WARNING, TRUE, GetUIIDNone());
        return;

    case UIHEROTYPE_SEASONAL:
        tModalTitle.SetText(StringListFetch("ConsoleHeroCreate:SeasonalModalWarningTitle"), FALSE);
        tModalTitle.SetTextColor(RGBAColor(0x0a, 0x47, 0x00, 0xff));
        tModalDesc.SetText(StringListFetch("ConsoleHeroCreate:SeasonalModalWarningDescription"), FALSE);
        ::Console::Screens::ShowModalDialog(nControllerIndex, UIWND_CONSOLE_HEROCREATE_HARDCORE_WARNING, TRUE, GetUIIDNone());
        break;

    case UIHEROTYPE_SEASONAL_HARDCORE:
        tModalTitle.SetText(StringListFetch("ConsoleHeroCreate:SeasonalHardcoreModalWarningTitle"), FALSE);
        tModalTitle.SetTextColor(RGBAColor(0x3f, 0x00, 0x00, 0xff));
        tModalDesc.SetText(StringListFetch("ConsoleHeroCreate:SeasonalHardcoreModalWarningDescription"), FALSE);
        ::Console::Screens::ShowModalDialog(nControllerIndex, UIWND_CONSOLE_HEROCREATE_HARDCORE_WARNING, TRUE, GetUIIDNone());
        break;
    }

    if ((eHeroType == UIHEROTYPE_SEASONAL || eHeroType == UIHEROTYPE_SEASONAL_HARDCORE) && !bRequestSeasonConfirmation)
    {
        // ask the server whether the season is still running before the hero is made
        uCachedSeasonAtConfirmation = 0;
        bSeasonConfirmedAvailable = FALSE;
        bRequestSeasonConfirmation = TRUE;
        ::Console::Online::IsSeasonInProgress(nControllerIndex, OnlineService::internal::function_bind_0_1<void, BOOL, &sSeasonConfirmationCallback>());
    }
}


void OnAccept(const UIID &, const int32 nControllerIndex)
{
    if (::Console::Screens::IsScreenTransitioning())
    {
        return;
    }

    if (!s_tHeroCreateData.bInputEnabled)
    {
        return;
    }

    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SELECT_GENERIC), cgtPSODefaults);

    PlayerClass * parClasses = GetAvailablePlayerClasses();
    UIConsoleListBox & tClassSelection = UI_GetConsoleListBoxFromID(UILIST_CONSOLE_HEROCREATE_CLASSES);
    PlayerClass eClass = PLAYER_CLASS_BARBARIAN;
    if (tClassSelection.GetSelectedIndex() != -1)
    {
        eClass = parClasses[tClassSelection.GetSelectedIndex()];
    }

    UIRadioButtonGroup & tGenderSelection = UI_GetRadioButtonGroupFromID(UIRADIO_CONSOLE_HEROCREATE_GENDER);
    int32 bFemale = tGenderSelection.GetSelectedValue();

    uint32 uHeroCreateFlags = ((bFemale != 0) << HEROCREATE_FLAG_FEMALE) | ((s_tHeroCreateData.fCreateHardcore != 0) << HEROCREATE_FLAG_HARDCORE);
    if (s_tHeroCreateData.fCreateSeasonal)
    {
        uHeroCreateFlags |= (1 << HEROCREATE_FLAG_SEASONAL);
    }

    if (s_tHeroCreateData.fCreateSeasonal && !bSeasonConfirmedAvailable)
    {
        {
            // the season ended, or the player cannot reach it
            NotificationCreationParams tParams;
            tParams.uszMessage = StringListFetch("ConsoleHeroCreate:SeasonNotAvailable");
            if (!::Console::GamerProfile::IsSignedInOnline(nControllerIndex))
            {
                switch (::Console::GamerProfile::GetOnlinePlayPrivilege(nControllerIndex))
                {
                case ::Console::GamerProfile::ONLINE_PLAY_BLOCKED_PARENTAL_CONTROLS:
                    tParams.uszMessage = StringListFetch("ConsoleErrors:OnlineStateErrorParentalBlockSwitch");
                    break;
                case ::Console::GamerProfile::ONLINE_PLAY_BLOCKED_NOT_SIGNED_IN_ONLINE:
                    tParams.uszMessage = StringListFetch("ConsoleHeroCreate:SeasonNotAvailableOnline");
                    break;
                }
            }
            tParams.nControllerIndex = nControllerIndex;
            ::Console::Screens::GetModalNotificationManager().SetOkNotification(tParams);
            return;
        }
    }


    if (!CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
    {
        EnableInput(FALSE);
        CRefString sRandomName = GenerateRandomName(eClass, bFemale);









        ::Console::Utilities::ShowKeyboardForUser(nControllerIndex,
                                                  sRandomName,
                                                  StringListFetch("ConsoleHeroCreate:HeroNameTitle"),
                                                  StringListFetch("ConsoleHeroCreate:HeroNameDesc"),
                                                  12,
                                                  OnlineService::internal::function_binder_5<void, int32, PlayerClass, uint32, BOOL, CRefString>::bind<&OnHeroNameEntered>(nControllerIndex, eClass, uHeroCreateFlags));
    }
    else
    {
        if (!CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
        {
            ::Console::Online::GameParams tParams;
            if (::Console::Online::PartyGetGameParams(tParams))
            {
                tParams.m_bResumeFromSave = FALSE;
                if (s_tHeroCreateData.fAdventureMode)
                {
                    tParams.m_eAct = ACT_OPEN_WORLD;
                    tParams.m_snoQuest = SNO_NONE;
                }
                else
                {
                    tParams.m_eAct = ACT_5;
                    tParams.m_snoQuest = 251355;
                }
                tParams.m_nQuestStep = -1;
                ::Console::Online::PartySetGameParams(tParams);
            }
        }

        OnHeroNameEntered(nControllerIndex, eClass, uHeroCreateFlags, TRUE, PlayerGetClassUIName(eClass, !bFemale));
    }
}


























void OnOpenModalWarning(const UIID & id, const int32)
{
    s_tHeroCreateData.nModalOpenTime = Blizzard::Time::Milliseconds();
    RefreshResetWarning(id);
}




void OnUpdateModalWarning(const UIID & id, const int32)
{
    RefreshResetWarning(id);
}




void OnAcceptModalWarning(const UIID & id, const int32 nControllerIndex)
{
    if (!HasModalCooldownPassed() || bRequestSeasonConfirmation)
        return;


    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SELECT_GENERIC), cgtPSODefaults);
    ::Console::Screens::CloseModalDialog(UIWND_CONSOLE_HEROCREATE_HARDCORE_WARNING);
    OnAccept(GetUIIDNone(), nControllerIndex);
}





void OnCancelModalWarning(const UIID &, const int32)
{
    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_PANEL_GENERIC_CLOSE), cgtPSODefaults);
    ::Console::Screens::CloseModalDialog(UIWND_CONSOLE_HEROCREATE_HARDCORE_WARNING);
}






inline void ChangeHeroType(int32 nDirection)
{
    UIRadioButtonGroup & tHeroTypeSelection = UI_GetRadioButtonGroupFromID(UIRADIO_CONSOLE_HEROCREATE_HERO_TYPE);
    int32 nSelectedValue = tHeroTypeSelection.GetSelectedValue();
    if (nDirection < 0 ? nSelectedValue != UIHEROTYPE_NORMAL : nSelectedValue != UIHEROTYPE_SEASONAL_HARDCORE)
    {
        if (nDirection < 0 || nSelectedValue != UIHEROTYPE_HARDCORE ||
            (s_tHeroCreateData.fSeasonAvailable && !bPlayerIsGuest))
        {
            UIHeroType eNewType = (UIHeroType)(nSelectedValue + nDirection);
            SetHeroType(eNewType);
            SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_GENERIC_FOCUS_SWITCH), cgtPSODefaults);
        }
    }
}









void ChangeGender(UIGender eGender)
{
    UIRadioButtonGroup & tGenderSelection = UI_GetRadioButtonGroupFromID(UIRADIO_CONSOLE_HEROCREATE_GENDER);
    UIGender eSelectedGender = (UIGender)tGenderSelection.GetSelectedValue();

    if (eSelectedGender == eGender)
    {
        return;
    }

    if (eGender == UIGENDER_MALE)
    {
        UI_GetTextFromID(UIRADIO_CONSOLE_HEROCREATE_GENDER_MALE).SetTextColor((FontSpecifier)0);
        UI_GetTextFromID(UIRADIO_CONSOLE_HEROCREATE_GENDER_FEMALE).SetTextColor((FontSpecifier)7);
    }
    else
    {
        UI_GetTextFromID(UIRADIO_CONSOLE_HEROCREATE_GENDER_MALE).SetTextColor((FontSpecifier)7);
        UI_GetTextFromID(UIRADIO_CONSOLE_HEROCREATE_GENDER_FEMALE).SetTextColor((FontSpecifier)0);
    }

    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SELECT_GENERIC), cgtPSODefaults);
    tGenderSelection.SetSelection(tGenderSelection.GetSelectedValue() ^ 1, FALSE);
    LanguageRules::SetClientGender(tGenderSelection.GetSelectedValue() == UIGENDER_MALE);

    UIConsoleListBox & tListbox = UI_GetConsoleListBoxFromID(UILIST_CONSOLE_HEROCREATE_CLASSES);
    tListbox.ForceCallToSelectionChange();
    tListbox.RefreshVisibleItems();
}


void OnDPadLeft(const UIID & id, const int32 nControllerIndex)
{
}














void OnDPadRight(const UIID & id, const int32 nControllerIndex)
{
}














void OnLStick(const UIID & id, const int32 nControllerIndex)
{
    if (::Console::Screens::IsScreenTransitioning())
        return;

    if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
        return;

    if (!s_tHeroCreateData.bInputEnabled)
        return;

    // the d-pad has its own handlers
    if (::Console::ControllerUtils::IsButtonDown(nControllerIndex, BUTTON_DPAD_LEFT) ||
        ::Console::ControllerUtils::IsButtonDown(nControllerIndex, BUTTON_DPAD_RIGHT) ||
        ::Console::ControllerUtils::IsButtonDown(nControllerIndex, BUTTON_DPAD_UP) ||
        ::Console::ControllerUtils::IsButtonDown(nControllerIndex, BUTTON_DPAD_DOWN))
        return;

    ::Console::AnalogStickInputFilter::Rule::ID idRule = s_tHeroCreateData.m_tStickInputFilter.HandleInput(::Console::ControllerUtils::GetLStickPosition(nControllerIndex, FALSE), FALSE);
    UIGender eGender;  // invented local; the DWARF records only idRule
    if (idRule == ::Console::RULE_STICK_LEFT)
    {
        eGender = UIGENDER_MALE;
    }
    else if (idRule == ::Console::RULE_STICK_RIGHT)
    {
        eGender = UIGENDER_FEMALE;
    }
    else
    {
        return;
    }

    ChangeGender(eGender);
}



void OnL1(const UIID & id, const int32 nControllerIndex)
{


    ChangeHeroType(-1);
}





































void OnButton3(const UIID &, const int32 nControllerIndex)
{
}
















void OnR1(const UIID & id, const int32 nControllerIndex)
{


    ChangeHeroType(1);
}












void CloseForHostMigration()
{
    if (bIsAddingLocalPlayer && !GameIsStartUp())
    {
        int32 nControllerIndex = s_tHeroCreateData.nControllerIndex;
        if (nControllerIndex != ::Console::GamerProfile::GetPrimaryProfileUserIndex() && ::Console::GamerProfile::IsActive(nControllerIndex))
        {
            if (::Console::Online::PartyIsInParty(nControllerIndex))
            {
                ::Console::Online::PartyRemovePlayer(nControllerIndex);
            }
            ::Console::UIOnlineActions::ClearLocalJoinFlag();
            ::Console::GamerProfile::SignOut(nControllerIndex);
        }
    }
}

}  // namespace Console
}  // namespace UIHeroCreate
