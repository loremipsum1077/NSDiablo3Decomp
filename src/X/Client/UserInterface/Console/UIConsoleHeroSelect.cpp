#include "../../../../BlizzardCore/Include/Blizzard/Lock.h"
#include "../../../../BlizzardCore/Include/Blizzard/Time.h"
#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../../Contrib/Contrib/blz/include/blz/atomic.h"
#include "../../../../Contrib/Contrib/blz/include/blz/list.h"
#include "../../../../Contrib/Contrib/blz/include/blz/string.h"
#include "../../Battlenet/Subsystems/ModalNotificationManager.h"
#include "../../CGame.h"
#include "../../Gfx/GfxVB.h"
#include "../../Console/GamerProfile.h"
#include "../../Console/Online.h"
#include "../Controls/UIConsoleListboxControl.h"
#include "../../../Common/OnlineService/OnlineService.h"
#include "../../../Common/Game.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/Definitions/HeroDefinition.h"
#include "../../../Sigma/RefString.h"
#include "../../../Sigma/UnicodeText.h"
#include "../../../Common/Shell/CmdLine.h"
#include "../UI.h"
#include "../UIID.h"
#include "../Controls/UITextControl.h"
#include "../Controls/UIWindowControl.h"

#include "../../../Common/VisualInventory.h"
#include "../../Sound/Sound.h"
#include "../../../../Contrib/Contrib/DemonWare/bdCore/bdSocket/bdCommonAddr.h"
#include "../UIInit.h"
#include "ConsoleScreenManager.h"
#include "ConsoleModalNotificationManager.h"
#include "../Controls/UIRActorControl.h"
#include "../Controls/UIButtonControl.h"
#include "../UIUtils.h"
#include "../../Battlenet/UI/Controls/ControlVisibilityManager.h"
#include "../../../Common/Protobuf/Console.pb.h"
#include "../../Battlenet/Subsystems/FrontendBackgroundHeroManager.h"
#include "../../../Sigma/LanguageRules.h"
#include "../../../Common/Console/ConsoleLicenses.h"
#include "../../Battlenet/Subsystems/FrontendBackgroundCameraManager.h"
#include "../../../Common/PlayerAccount.h"
#include "ControllerActions.h"
// X/Client/Console/GamerProfile.h; declared here because adding it there would pull VisualInventory.h into every includer.
namespace Console { namespace GamerProfile {
struct HeroInfo { HeroInfo(); void Export(D3::Console::HeroInfo & tHeroInfo) const; OnlineService::HeroId uHeroId; Blizzard::Time::UnixTime uCreationTime; char szFilename[32]; char szName[49]; PlayerClass eClass; uint32 uLevel; uint32 uAltLevel; uint32 uPlayerFlags; uint32 unk_70; Act eLastAct; Blizzard::Time::UnixTime uDeathTime; SNO snoKiller; MonsterRarity eKillerRarity; GBID arKillerRareNameGBIDs[2]; CRefString usKillerName; uint64 uMonstersKilled; uint64 uElitesKilled; uint64 uGoldCollected; uint32 uTimePlayed; uint32 uSeasonCreated; VisualEquipment tEquipment; };
static_assert(sizeof(HeroInfo) == 0x158, "HeroInfo");
static_assert(__builtin_offsetof(HeroInfo, unk_70) == 0x70, "HeroInfo new word");
static_assert(__builtin_offsetof(HeroInfo, eLastAct) == 0x74, "HeroInfo act");
static_assert(__builtin_offsetof(HeroInfo, usKillerName) == 0x90, "HeroInfo killer name");
static_assert(__builtin_offsetof(HeroInfo, tEquipment) == 0xc8, "HeroInfo equipment");
 } }
namespace Console { namespace UIOnlineActions { bool IsActiveSeason(uint32 uSeason, const D3::Account::Digest & tAccountDigest); void ClearLocalJoinFlag(); } }  // OnlineActions.h
namespace Console { namespace GamerProfile { void SignOut(int32 nUserIdx); struct HeroInfo; typedef OnlineService::Callback<void (unsigned int)> FindHeroesCallback; void FindHeroes(int32 nUserIdx, HeroInfo * ptHeroInfoList, uint32 uMaxHeroes, FindHeroesCallback pfnCallback); enum LoadHeroResult { LOAD_HERO_FAILED = 0, LOAD_HERO_FAILED_HERO_IN_USE = 1, LOAD_HERO_FAILED_NO_HEROES = 2, LOAD_HERO_FAILED_CORRUPT = 3, LOAD_HERO_FAILED_CONTENT_MISSING = 4, LOAD_HERO_SUCCESS = 5 }; } }  // GamerProfile.h
namespace Console { namespace GamerProfile { typedef OnlineService::Callback<void (LoadHeroResult)> LoadHeroCallback; enum DeleteHeroResult { DELETE_HERO_FAILED = 0, DELETE_HERO_FAILED_NOT_FOUND = 1, DELETE_HERO_FAILED_CONTENT_MISSING = 2, DELETE_HERO_FAILED_NOT_OWNER = 3, DELETE_HERO_FAILED_IN_USE = 4, DELETE_HERO_SUCCESS = 5 }; typedef OnlineService::Callback<void (DeleteHeroResult)> DeleteHeroCallback; void LoadHero(int32 nUserIdx, OnlineService::HeroId idHero, LoadHeroCallback pfnCallback); void DeleteHero(int32 nUserIdx, OnlineService::HeroId idHero, DeleteHeroCallback pfnCallback); D3::Console::Profile & GetProfile(int32 nUserIdx); uint32 GetNumAvailableHeroes(int32 nUserIdx); } }
namespace Console { namespace UIOnlineActions { void CreatePartyIfNeeded(int32 nControllerIndex); void FinalizeLocalPlayerJoin(int32 nControllerIndex, OnlineService::Callback<void (int)> pfnCallback); } }  // OnlineActions.h
namespace Console { namespace Online { void PartyStartGame(); BOOL IsSeasonsInitialized(); void IsSeasonInProgress(int32 nUserIndex, IsSeasonInProgressCallback pfnCallback); } namespace DLCManager { uint32 GetLicenses(); } }  // Online.h, DLCManager.h
namespace Console { namespace SaveMigration { void RebirthHero(int32 nControllerIndex, uint32 uSeason, BOOL bUnk); } } namespace UIGameSettings { namespace Console { void ClearGameSettingsViewed(); } }  // SaveMigration.h, UIConsoleGameSettings.h
namespace Console { namespace GamerProfile {  OnlinePlayPrivilege GetOnlinePlayPrivilege(int32 nUserIdx); } }  // GamerProfile.h
extern UIID UITEXT_CONSOLE_PLAYER_NUMBER, UITEXT_CONSOLE_HEROSELECT_REBIRTH_WARNING_TITLE, UITEXT_CONSOLE_HEROSELECT_REBIRTH_WARNING_DESCRIPTION, UITEXT_CONSOLE_HERO_NAME, UITEXT_CONSOLE_HERO_QUEST, UIWND_CONSOLE_HERO_SELECT_HARDCORE, UIWND_CONSOLE_HERO_SELECT_SEASONAL, UIWND_CONSOLE_HERO_SELECT_SEASONAL_HARDCORE;  // UIIDs.h
namespace UIUtils { namespace Console { int32 GetParagonLevel(const D3::Account::SavedDefinition & tAccount, BOOL bHardcore, BOOL bSeasonal, BOOL bUnk); CRefString PlayerGetDecoratedPlayerNum(int32 nControllerIndex); } }  // UIUtils.h
int32 GetConsoleMaxHeroesAllowed(int32 nControllerIndex); BOOL PlayersAreAllLocal(); SNO UI_GetDefaultHeroLight(); LightID LightAdd(uint32 dwFlags, SNO snoLight, const PRTransform & tTransform, BOOL bUnused, RenderContext eContext);  // Game.h, CPlayer.h, UI.h, Light.h
CRefString CPlayerGetLevelString(const int32 nLevel, const int32 nAltLevel); CRefString DisplayActName(int32 eAct); PlayerClass PlayerClassFromGBID(const GBID gbid); CRefString PlayerGetClassUIName(const PlayerClass ePlayerClass, const BOOL fIsMale);  // X/Client/CPlayer.h, X/Common/Act.h, X/Common/Player.h
SNO GetActBackgroundSNO(Act eAct); void UIStartup_SetBackgroundWorld(SNO snoWorld); SNO GetFrontendCharacterSNO(PlayerClass eClass, BOOL bIsFemale);  // UIStartup.h, UIConsoleCharacterSelect.h
RActorID UI_CreateHeroUIRActor(SNO snoActor, SNO snoIrradianceTex, const VisualEquipment & tEquipment, VisualInventoryType eVIT, PlayerClass ePlayerClass, BOOL fFemale, const PRTransform & tTransform, RenderContext eContext, uint32 dwAnimTag, float flScale, BOOL bUnknown);  // UI.h
namespace UIHallOfFallenHeroes { namespace Console { void OpenForController(int32 nControllerIndex); void OpenForController(int32 nControllerIndex, OnlineService::HeroId idHeroToArchive, OnlineService::Callback<void ()> pfnOnComplete); } }  // UIConsoleHallOfFallenHeroes.h  // GamerProfile.h
UIRActor & UI_GetRActorFromID(const UIID & id); void InputClearRemappings(); void LightDelete(LightID idLight);  // UI.h, X/Client/Input.h, X/Client/Light.h
namespace Console { namespace Online { void PartyRemovePlayer(int32 nUserIdx); void ClearOutstandingInvite(); } }  // Online.h
namespace Console { namespace UIOnlineActions { void SetGameParamsForHero(const D3::Account::SavedDefinition & tAccount, const D3::Hero::SavedDefinition & tHero, Online::GameParams & tParams); } }  // OnlineActions.h
namespace Console { namespace Screens { enum { MAIN_MENU_SCREEN_SET = 20 }; BOOL OpenToScreen(const ScreenSets eScreenSet, const int32 idScreen, int32 nControllerOpening, BOOL fSkipTransition); } }  // ConsoleScreenManager.h
namespace UIHallOfFallenHeroes { namespace Console { BOOL IsVisible(); } } namespace UIMainMenu { namespace Console { void OpenStoreToNecromancer(int32 nControllerIndex); } } namespace Console { namespace GamerProfile { void SetBlizzconHero(int32 nUserIdx, OnlineService::HeroId uHeroId); } }  // UIConsoleHallOfFallenHeroes.h, UIConsoleMainMenu.h, GamerProfile.h
CRefString LocalizedSprintf(LPCSTR szFormat, ...);  // X/Common/LocalizedString.h
CRefString StringListFetch(SNO snoStringList, LPCSTR szLabel); CRefString StringListFetch(LPCSTR szLabel);  // StringList.h
extern UIID UITEXT_CONSOLE_HERO_SELECT_STATUS, UIWND_CONSOLE_HERO_SELECT, UITEXT_CONSOLE_HERO_SLOTS_AVAILABLE, UILIST_CONSOLE_HERO_LIST, UIWND_CONSOLE_HEROSELECT_REBIRTH_WARNING, UIRACTOR_CONSOLE_HERO_SELECT_HERO, UIRACTOR_CONSOLE_HERO_SELECT_FILAGREE;  // UIIDs.h


namespace UIHeroSelect {
namespace Console {


BOOL bSeasonConfirmedAvailable;
BOOL bRequestedSeasonAvailability;
uint32 uCachedSeasonAtConfirmation;
BOOL bHaveNecroLicense;
BOOL bIsAddingLocalPlayer;
#line 81
struct Timer {
    Timer() : start(0), duration(0) {}
#line 86
    static Blizzard::Time::Millisecond Now() {
        return Blizzard::Time::Milliseconds();
    }
#line 91
    void Start(uint32 uDurationMilliseconds) {
        start = Now();
        duration = uDurationMilliseconds;
    }
#line 97
    Blizzard::Time::Millisecond Read() {
        return Now() - start;
    }
#line 102
    bool Started() {
        return start != 0;
    }
#line 107
    bool Finished();


#line 113
    void Clear() {
        start = 0;
        duration = 0;
    }
#line 83
    Blizzard::Time::Millisecond start;
    Blizzard::Time::Millisecond duration;
};
#line 113








struct BlizzconGame {
    Timer lastJoinCheckTimer;
    Timer hostLaunchTimer;
    Timer playGameRequestTimer;
    blz::list<::Console::Online::PartyInfo> tFindResult;
    Blizzard::Lock::RMutex tFindMutex;
    blz::atomic_bool bFindResultDirty{false};
    BOOL bReadyToPlay = FALSE;
    uint uAvailableParties = 0;
    char szCurrentMessage[512] = {};
    char szDesiredGameName[512] = {};
    ::Console::Online::SwitchUniqueID tDesiredGameID = {};
    int32 unk_46c = -1;  // 0x46c; static init stores -1 after the memset





    void Clear()
    {
        szDesiredGameName[0] = '\0';
        tDesiredGameID = {};
        lastJoinCheckTimer.Clear();
        hostLaunchTimer.Clear();
        szCurrentMessage[0] = '\0';
        bReadyToPlay = FALSE;
        uAvailableParties = 0;
        playGameRequestTimer.Clear();

        tFindResult.clear();
        bFindResultDirty = false;
    }
};

BlizzconGame tBlizzconGame;

struct BlizzconModal {
    blz::string m_sBaseMessage;
    ModalNotificationId m_tWaitForPartyModalID = INVALID_MODAL_NOTIFICATION_ID;
    Timer m_tDotTimer;
    int m_nDotCount = 0;
    int m_nMaxDotCount = 3;

    void UpdateMessage();
    void SetMessage(const char * message);
    void CloseMessage();
    bool IsShowing() { return m_tWaitForPartyModalID != INVALID_MODAL_NOTIFICATION_ID; }
};
















































BlizzconModal tBlizzconModal;
enum { REBIRTH_COOLDOWN_MILLISECS = 3000 };








inline BOOL IsHardcoreMismatchInGameOnly(const ::Console::GamerProfile::HeroInfo & tHeroInfo)
{
    if (GameIsStartUp() || GameIsWeeklyChallenge())
        return FALSE;
    return (tHeroInfo.uPlayerFlags ^ (sgtAppGlobals.dwGameCreateParamFlags >> GAMECREATEPARAM_HARDCORE)) & (1 << PLAYERFLAG_HARDCORE);
}

inline BOOL IsSeasonalMismatchInGameOnly(const ::Console::GamerProfile::HeroInfo & tHeroInfo, const D3::Account::SavedDefinition & tAccountDefinition)
{
    if (GameIsStartUp() || GameIsWeeklyChallenge())
        return FALSE;
    return ::Console::UIOnlineActions::IsActiveSeason(tHeroInfo.uSeasonCreated, tAccountDefinition.digest()) != ((sgtAppGlobals.dwGameCreateParamFlags & (1 << GAMECREATEPARAM_SEASON)) != 0);
}


extern "C" BOOL sub_710072e0b0(uint32 uSeasonCreated, uint32 uFlags);  // OnlineService.cpp region (after GetSeasonState)
uint32 PlayerGetLocalPlayerCount();  // CPlayer.h
BOOL IsHeroSelectable(const ::Console::GamerProfile::HeroInfo & tHeroInfo, const D3::Account::SavedDefinition & tAccountDefinition)
{
    // a necromancer hero needs the necromancer pack
    if (tHeroInfo.eClass == PLAYER_CLASS_NECROMANCER && !bHaveNecroLicense)
        return FALSE;




    BOOL bIsDeceased = tHeroInfo.uPlayerFlags & (1 << PLAYERFLAG_DECEASED);
    BOOL bHardcoreMismatch = IsHardcoreMismatchInGameOnly(tHeroInfo);
    BOOL bSeasonalMismatch = IsSeasonalMismatchInGameOnly(tHeroInfo, tAccountDefinition);
    // 2.7.7: game-create param bit 20 (no enumerator in Game.h) against the hero's flag from sub_710072e0b0
    // NON_MATCHING: 2.7.7 solo (bit 20) mismatch check reproduced; 376 vs 352 bytes, target folds the
    // check into a single cset/cmp chain ending in csel (block layout/register allocation differ)
    BOOL bUnkMismatch = FALSE;
    if (!GameIsStartUp() && !GameIsWeeklyChallenge())
    {
        uint32 dwFlags = sgtAppGlobals.dwGameCreateParamFlags;
        BOOL bHeroFlag = sub_710072e0b0(tHeroInfo.uSeasonCreated, tHeroInfo.unk_70);
        if (((dwFlags & (1 << 20)) != 0) != (bHeroFlag != FALSE))
            bUnkMismatch = TRUE;
        else if ((dwFlags & (1 << 20)) && PlayerGetLocalPlayerCount() != 0)
            bUnkMismatch = TRUE;
    }

    return !bIsDeceased && !bHardcoreMismatch && !bSeasonalMismatch && !bUnkMismatch;
}

class HeroListDataSource : public ConsoleListBoxDataSource {
public:
    virtual ~HeroListDataSource() {}

    HeroListDataSource() {
        nHeroCount = 0;
    }

    virtual int32 GetNumItems() const {
        return nHeroCount;
    }

    virtual UIControlSourceKey GetSourceKeyForItem(const int32) const {
        return UIControlSourceKey::Parse("ConsoleHeroSelect:ConsoleCharacterSelect_main.hero_item_template", SNO_NONE, NULL, NULL);
    }

    virtual void InitializeItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex) {
    }




















    virtual void UpdateItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex)
    {

        // the class icons
        UIID idWizard = UIIDCreateFromParent(idItemUI, ".WizardIcon");
        UIID idWitchDoctor = UIIDCreateFromParent(idItemUI, ".WitchdoctorIcon");
        UIID idMonk = UIIDCreateFromParent(idItemUI, ".MonkIcon");
        UIID idDemonHunter = UIIDCreateFromParent(idItemUI, ".DemonhunterIcon");
        UIID idBarb = UIIDCreateFromParent(idItemUI, ".BarbarianIcon");

        const D3::Account::SavedDefinition & tAccountDefinition = ::Console::GamerProfile::GetAccount(nControllerIndex);
        const ::Console::GamerProfile::HeroInfo & tHeroInfo = tHeroInfoList[nIndex];
        ControlVisibilityManager tIcons;
        tIcons.AddControl(idDemonHunter);
        tIcons.AddControl(idBarb);
        tIcons.AddControl(idWizard);
        tIcons.AddControl(idWitchDoctor);
        tIcons.AddControl(idMonk);
        tIcons.SetActive(tHeroInfo.eClass, false);

        BOOL bIsHardcore = tHeroInfo.uPlayerFlags & (1 << PLAYERFLAG_HARDCORE);
        BOOL bIsSeasonal = ::Console::UIOnlineActions::IsActiveSeason(tHeroInfo.uSeasonCreated, tAccountDefinition.digest());

        UIID idHardcoreIcon = UIIDCreateFromParent(idItemUI, ".HeroTypeIcon.HeroTypeHardcore");
        UIID idSeasonalIcon = UIIDCreateFromParent(idItemUI, ".HeroTypeIcon.HeroTypeSeasonal");
        UIID idSeasonalHardcoreIcon = UIIDCreateFromParent(idItemUI, ".HeroTypeIcon.HeroTypeHardcoreSeasonal");

        UI_GetWindowFromID(idHardcoreIcon).SetActive(bIsHardcore && !bIsSeasonal);
        UI_GetWindowFromID(idSeasonalIcon).SetActive(bIsSeasonal && !bIsHardcore);
        UI_GetWindowFromID(idSeasonalHardcoreIcon).SetActive(bIsHardcore && bIsSeasonal);

        BOOL bIsSelected = tListBox.GetSelectedIndex() == nIndex;

        UIID idSelected;
        if (tIcons.GetSelectedControl(idSelected))
        {
            UIButton & tButton = UI_GetButtonFromID(idSelected);
            tButton.SetText(tHeroInfo.szName, FALSE);
            tButton.SetButtonState(bIsSelected ? BS_PUSHED : BS_NORMAL, FALSE, FALSE);
            tButton.SetEnabled(bIsSelected || IsHeroSelectable(tHeroInfo, tAccountDefinition));
        }

        if (bIsSelected)
        {
            // move the highlight behind the selected hero
            UIRActor & tFilagree = UI_GetRActorFromID(UIRACTOR_CONSOLE_HERO_SELECT_FILAGREE);
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

    ::Console::GamerProfile::HeroInfo tHeroInfoList[17];
    int32 nHeroCount;
    int32 nControllerIndex;
};


struct HeroSelectionGlobals {
    HeroListDataSource tHeroListDataSource;
    OnlineService::HeroId idHero;
    LightID idInGameHeroLight;
    int32 nMaxHeroes;
    BOOL bFetchingHeroes;
    BOOL bEnableInput;
    Blizzard::Time::Millisecond nModalOpenTime;
    uint32 uSeasonNum;
    OnlineService::SeasonState eSeasonState;
    BOOL unk_170c;  // 0x170c; OnRebirth stores its third argument here (TRUE from UIHeroSelect_RebirthSolo)





    HeroSelectionGlobals() {
        idInGameHeroLight = -1;
        nMaxHeroes = 17;
        bFetchingHeroes = FALSE;
        bEnableInput = TRUE;
        nModalOpenTime = 0;
        uSeasonNum = 0;
        eSeasonState = OnlineService::SEASON_STATE_PRE;
        unk_170c = FALSE;
    }
};




HeroSelectionGlobals sgtHeroSelectionGlobals;

void PopulateHeroList(int32 nControllerIndex, BOOL bScrollToTop);
void UpdateCharacterModel();
void RefreshHighlightRActor();
void RefreshResetWarning(const UIID & id);
void OnConfirmRebirthHero(const UIID & id, const int32 nControllerIndex);
void OnPartyHeroUpdated(int32 nControllerIndex, BOOL bSuccess);
void OnBack(const UIID &, const int32 nControllerIndex);
void OnConfirmDeleteHero(const int32 nControllerIndex, CRefString sStatusStringKey, const UIID & id);
















void PopulateHeroList(int32 nControllerIndex, BOOL bScrollToTop)
{
    UIConsoleListBox & tHeroList = UI_GetConsoleListBoxFromID(UILIST_CONSOLE_HERO_LIST);

    // default to the hero that is loaded now
    OnlineService::HeroId idCurrentHero = OnlineService::INVALID_HERO_ID;
    if (::Console::GamerProfile::IsHeroLoaded(nControllerIndex))
        idCurrentHero = ::Console::GamerProfile::GetHero(nControllerIndex).digest().hero_id().id_low();

    if (!bHaveNecroLicense && PlayerClassFromGBID(::Console::GamerProfile::GetHero(nControllerIndex).digest().gbid_class()) == PLAYER_CLASS_NECROMANCER)
    {
        idCurrentHero = OnlineService::INVALID_HERO_ID;
    }

    int32 nDefaultIndex = 0;
    for (int32 nHero = 0; nHero < sgtHeroSelectionGlobals.tHeroListDataSource.nHeroCount; ++nHero)
    {
        const ::Console::GamerProfile::HeroInfo & tHeroInfo = sgtHeroSelectionGlobals.tHeroListDataSource.tHeroInfoList[nHero];
        if (tHeroInfo.uHeroId == idCurrentHero)
        {
            nDefaultIndex = nHero;
            sgtHeroSelectionGlobals.idHero = idCurrentHero;
        }
    }

    tHeroList.SetDataSource(sgtHeroSelectionGlobals.tHeroListDataSource);
    tHeroList.ResetAllItems();

    if (sgtHeroSelectionGlobals.tHeroListDataSource.nHeroCount != 0)
    {
        tHeroList.SetSelectedIndex(nDefaultIndex);
        if (bScrollToTop)
        {
            tHeroList.ScrollItemIntoView(nDefaultIndex);
        }
    }
}





void RefreshHeroSlotsDisplay()
{
    int32 nAvailableHeroSlots = Math::Max(sgtHeroSelectionGlobals.nMaxHeroes != 2 ? sgtHeroSelectionGlobals.nMaxHeroes - sgtHeroSelectionGlobals.tHeroListDataSource.nHeroCount : 0, 0);
    CRefString sSlotsText = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_HEROSELECT), "SlotsAvailable"), UnicodeText::FormatNumber(nAvailableHeroSlots, TRUE, FNS_NORMAL, XLOCALE_INVALID).str(), NULL);
    UI_GetTextFromID(UITEXT_CONSOLE_HERO_SLOTS_AVAILABLE).SetText(sSlotsText, FALSE);
}









inline void EnableInput(BOOL bEnabled)
{

    sgtHeroSelectionGlobals.bEnableInput = bEnabled;
    if (bEnabled && (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT) ||
        !UIHallOfFallenHeroes::Console::IsVisible()))
    {
        UI_SetControllerFocus(UILIST_CONSOLE_HERO_LIST);
    }
    else
    {
        UI_SetControllerFocus(GetUIIDNone());
    }
}


inline void UpdateStatsPane(const ::Console::GamerProfile::HeroInfo & tHeroInfo)
{

    BOOL bIsHardcore = tHeroInfo.uPlayerFlags & (1 << PLAYERFLAG_HARDCORE);
    D3::Account::SavedDefinition & tAccountDefinition = ::Console::GamerProfile::GetAccount(sgtHeroSelectionGlobals.tHeroListDataSource.nControllerIndex);
    BOOL bIsSeasonal = ::Console::UIOnlineActions::IsActiveSeason(tHeroInfo.uSeasonCreated, tAccountDefinition.digest());

    UI_GetWindowFromID(UIWND_CONSOLE_HERO_SELECT_HARDCORE).SetActive(bIsHardcore && !bIsSeasonal);
    UI_GetWindowFromID(UIWND_CONSOLE_HERO_SELECT_SEASONAL).SetActive(bIsSeasonal && !bIsHardcore);
    UI_GetWindowFromID(UIWND_CONSOLE_HERO_SELECT_SEASONAL_HARDCORE).SetActive(bIsHardcore && bIsSeasonal);
}


void UpdateName(const ::Console::GamerProfile::HeroInfo & tHeroInfo)
{
    int32 nLevel = tHeroInfo.uLevel;
    int32 nAltLevel = 0;
    if (UI_GetPrimaryUIController() != -1)
    {
        nAltLevel = UIUtils::Console::GetParagonLevel(::Console::GamerProfile::GetAccount(UI_GetPrimaryUIController()),
                                                      tHeroInfo.uPlayerFlags & (1 << PLAYERFLAG_HARDCORE),
                                                      ::Console::UIOnlineActions::IsActiveSeason(tHeroInfo.uSeasonCreated, ::Console::GamerProfile::GetAccount(sgtHeroSelectionGlobals.tHeroListDataSource.nControllerIndex).digest()),
                                                      sub_710072e0b0(tHeroInfo.uSeasonCreated, tHeroInfo.unk_70));
    }

    PlayerClass eClass = tHeroInfo.eClass;
    BOOL bIsFemale = tHeroInfo.uPlayerFlags & (1 << PLAYERFLAG_FEMALE);
    BOOL bDeceased = tHeroInfo.uPlayerFlags & (1 << PLAYERFLAG_DECEASED);
    Act eAct = tHeroInfo.eLastAct;

    CRefString sName = LocalizedSprintf(StringListFetch("ConsoleHeroSelect:HeroLevelInfo").str(), CPlayerGetLevelString(nLevel, nAltLevel).str(), PlayerGetClassUIName(eClass, !bIsFemale).str(), NULL);
    if (bDeceased)
    {
        LanguageRules::SetClientGender(!bIsFemale);
        sName = LocalizedSprintf(StringListFetch("ConsoleHeroSelect:DeadHeroInfo").str(), sName.str());
    }

    UI_GetTextFromID(UITEXT_CONSOLE_HERO_NAME).SetText(sName, FALSE);
    UI_GetTextFromID(UITEXT_CONSOLE_HERO_QUEST).SetText(DisplayActName(eAct).str(), FALSE);
}







void UpdateCharacterModel()
{
    UIConsoleListBox & tHeroList = UI_GetConsoleListBoxFromID(UILIST_CONSOLE_HERO_LIST);
    if (tHeroList.GetNumItems() <= 0)
    {
        if (GameIsStartUp())
        {
            ::Console::Screens::GetFrontendBackgroundHeroManager().ClearAllBackgroundHeros();
        }
        return;
    }

    int32 nIndex = tHeroList.GetSelectedIndex();
    if (nIndex == -1)
    {
        return;
    }

    const ::Console::GamerProfile::HeroInfo & tHeroInfo = sgtHeroSelectionGlobals.tHeroListDataSource.tHeroInfoList[nIndex];
    BOOL bIsFemale = (tHeroInfo.uPlayerFlags & (1 << PLAYERFLAG_FEMALE)) != 0;
    BOOL bIsHardcore = (tHeroInfo.uPlayerFlags & (1 << PLAYERFLAG_HARDCORE)) != 0;
    BOOL bIsDeceased = (tHeroInfo.uPlayerFlags & (1 << PLAYERFLAG_DECEASED)) != 0;

    if (GameIsStartUp())
    {
        SNO snoWorld = GetActBackgroundSNO(tHeroInfo.eLastAct);
        UIStartup_SetBackgroundWorld(snoWorld);

        BackgroundHeroDescription tHero;
        tHero.SetPlayerHero(tHeroInfo.eClass, bIsFemale, bIsHardcore, bIsDeceased, tHeroInfo.tEquipment);

        FrontendBackgroundHeroManager & tHeroManager = ::Console::Screens::GetFrontendBackgroundHeroManager();
        tHeroManager.ChangeBackgroundHero(HERO_SLOT_1, tHero);
        tHeroManager.ClearBackgroundHero(HERO_SLOT_2);
        tHeroManager.ClearBackgroundHero(HERO_SLOT_3);
        tHeroManager.ClearBackgroundHero(HERO_SLOT_4);
    }
    else
    {
        UIRActor & tHeroRActor = UI_GetRActorFromID(UIRACTOR_CONSOLE_HERO_SELECT_HERO);
        tHeroRActor.DeleteRActor();

        PRTransform transform(cgqIdentity, cgwpNull);
        SNO snoActor = GetFrontendCharacterSNO(tHeroInfo.eClass, bIsFemale);
        RActorID idRActorNew = UI_CreateHeroUIRActor(snoActor, GlobalSNOGet(GLOBALSNO_RACTOR_NEUTRAL_IRRADIANCE_TEXTURE), tHeroInfo.tEquipment, 0, tHeroInfo.eClass, bIsFemale, transform, tHeroRActor.tRenderData[CGameAsyncGetRenderDataIndex(0)].m_tDisplayList.GetRenderContext(), 0x68700, 1.0f, 0);
        tHeroRActor.SetRActor(idRActorNew);
    }








    sgtHeroSelectionGlobals.idHero = tHeroInfo.uHeroId;

    RefreshHighlightRActor();
    UpdateStatsPane(tHeroInfo);
    UpdateName(tHeroInfo);
}


void sOnHeroesListed(int32 nControllerIndex, BOOL bScrollToTop, int32 nHeroCount)
{
    EnableInput(TRUE);
    sgtHeroSelectionGlobals.bFetchingHeroes = FALSE;
    sgtHeroSelectionGlobals.tHeroListDataSource.nControllerIndex = nControllerIndex;
    sgtHeroSelectionGlobals.tHeroListDataSource.nHeroCount = nHeroCount;
    RefreshHeroSlotsDisplay();
    PopulateHeroList(nControllerIndex, bScrollToTop);
    UpdateCharacterModel();
}





void RefreshHeroListAndData(int32 nControllerIndex, BOOL bScrollToTop)
{
    EnableInput(FALSE);

    UI_GetTextFromID(UITEXT_CONSOLE_HERO_SLOTS_AVAILABLE).SetText("", FALSE);

    sgtHeroSelectionGlobals.bFetchingHeroes = TRUE;
    ::Console::GamerProfile::FindHeroes(nControllerIndex, sgtHeroSelectionGlobals.tHeroListDataSource.tHeroInfoList, 17,
        OnlineService::internal::function_binder_3<void, int32, BOOL, int32>::bind<&sOnHeroesListed>(nControllerIndex, bScrollToTop));
}







void RefreshHighlightRActor()
{
    UIRActor & tFilagree = UI_GetRActorFromID(UIRACTOR_CONSOLE_HERO_SELECT_FILAGREE);
    PRTransform transform(cgqIdentity, cgwpNull);
    if (tFilagree.GetRActor() == RACTORID_INVALID)
    {
        RActorID idRActor = RActorCreate(GlobalSNOGet(GLOBALSNO_ACTOR_CONSOLE_MENU_FILAGREE), 1.0f, -1.0f, transform, 0, tFilagree.tRenderData[CGameAsyncGetRenderDataIndex(0)].m_tDisplayList.GetRenderContext(), -1);
        tFilagree.SetRActor(idRActor);
    }
}


















BOOL CanCloseSafely()
{
    return !UI_GetWindowFromID(UIWND_CONSOLE_HERO_SELECT).IsActive();
}



inline void sSeasonConfirmationCallback(BOOL bInProgress)
{
    bSeasonConfirmedAvailable = bInProgress;
    bRequestedSeasonAvailability = FALSE;
    uCachedSeasonAtConfirmation = OnlineService::GetSeasonNum();
}



void OpenForController(int32 nControllerIndex)
{

    if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
    {
        tBlizzconGame.Clear();
    }

    if (::Console::GamerProfile::GetPrimaryProfileUserIndex() != nControllerIndex)
    {
        bIsAddingLocalPlayer = TRUE;
    }

    // the necromancer pack decides which heroes can be played
    bHaveNecroLicense = (::Console::DLCManager::GetLicenses() & (1 << LICENSE_NECROMANCER)) != 0;

    UIWindow & tHeroSelect = UI_GetWindowFromID(UIWND_CONSOLE_HERO_SELECT);
    if (tHeroSelect.IsActive())
    {
        return;
    }

    UI_SetControllerFocus(UILIST_CONSOLE_HERO_LIST);
    if (::Console::GamerProfile::IsSignedIn(nControllerIndex))
    {
        sgtHeroSelectionGlobals.nMaxHeroes = GetConsoleMaxHeroesAllowed(nControllerIndex);
    }
    else
    {
        sgtHeroSelectionGlobals.nMaxHeroes = 2;
    }

    sgtHeroSelectionGlobals.uSeasonNum = OnlineService::GetSeasonNum();
    sgtHeroSelectionGlobals.eSeasonState = OnlineService::GetSeasonState();

    if (GameIsStartUp())
    {
        const D3::Account::SavedDefinition & tAccount = ::Console::GamerProfile::GetAccount(nControllerIndex);
        PlayerAccountBanner tBanner;
        tBanner.Import(tAccount.digest().banner_configuration());
        ::Console::Screens::GetFrontendBackgroundHeroManager().ChangeBackgroundBanner(tBanner);
    }
    else if (PlayersAreAllLocal())
    {
        GamePause(TRUE);
    }

    RefreshHighlightRActor();

    UI_GetTextFromID(UITEXT_CONSOLE_HERO_NAME).SetText("", FALSE);
    UI_GetTextFromID(UITEXT_CONSOLE_HERO_QUEST).SetText("", FALSE);
    RefreshHeroListAndData(nControllerIndex, TRUE);

    ::Console::Screens::GetActionManager().ClearAllActions();
    ::Console::Screens::UpdateButtonLegendText();

    if (GameIsStartUp())
    {
        tHeroSelect.SetActive(TRUE);
    }
    else
    {
        ::Console::Screens::ShowModalDialog(nControllerIndex, UIWND_CONSOLE_HERO_SELECT, TRUE, GetUIIDNone());
        UI_SetControllerFocus(UILIST_CONSOLE_HERO_LIST);
    }

    if (GameIsStartUp())
    {
        FrontendBackgroundCameraManager & tCameraManager = ::Console::Screens::GetFrontendBackgroundCameraManager();
        if (tCameraManager.GetBackgroundCameraPosition() != CAM_POS_CLOSE ||
            (tCameraManager.GetTargetBackgroundCameraPosition() != CAM_POS_NONE && tCameraManager.GetTargetBackgroundCameraPosition() != CAM_POS_CLOSE))
        {
            tCameraManager.TransitionBackgroundCamera(CAM_POS_CLOSE, 42.0f);
            SNO sound = GlobalSNOGet(GLOBALSNO_SOUND_CAMERA_ZOOMIN);
            SoundManager::PlaySound(sound, cgtPSODefaults);
        }
    }
    else
    {
        sgtHeroSelectionGlobals.idInGameHeroLight = LightAdd(0, UI_GetDefaultHeroLight(), PRTransform(cgqIdentity, cgwpNull), FALSE, RENDERCONTEXT_HUD_0);
    }

    uint32 nNumMembers = ::Console::Online::PartyGetMemberCount();
    UIText & tPlayerNumber = UI_GetTextFromID(UITEXT_CONSOLE_PLAYER_NUMBER);
    if (nNumMembers > 1 && ::Console::Online::PartyIsInParty(nControllerIndex))
    {
        tPlayerNumber.SetText(UIUtils::Console::PlayerGetDecoratedPlayerNum(nControllerIndex), FALSE);
        tPlayerNumber.SetActive(TRUE);
    }
    else
    {
        tPlayerNumber.SetActive(FALSE);
    }
}

























void Close()
{
    InputClearRemappings();
    bIsAddingLocalPlayer = FALSE;

    if (GameIsPaused())
    {
        GamePause(FALSE);
    }

    if (sgtHeroSelectionGlobals.idInGameHeroLight != -1)
    {
        LightDelete(sgtHeroSelectionGlobals.idInGameHeroLight);
        sgtHeroSelectionGlobals.idInGameHeroLight = -1;
    }

    UIRActor & tHeroRActor = UI_GetRActorFromID(UIRACTOR_CONSOLE_HERO_SELECT_HERO);
    tHeroRActor.DeleteRActor();
    UIRActor & tFilagree = UI_GetRActorFromID(UIRACTOR_CONSOLE_HERO_SELECT_FILAGREE);
    tFilagree.DeleteRActor();

    UI_SetControllerFocus(GetUIIDNone());
    if (GameIsStartUp())
    {
        UI_GetWindowFromID(UIWND_CONSOLE_HERO_SELECT).SetActive(FALSE);
    }
    else
    {
        ::Console::Screens::CloseModalDialog(UIWND_CONSOLE_HERO_SELECT);
    }
}










































void CloseForHostMigration()
{
    if (bIsAddingLocalPlayer && !GameIsStartUp())
    {
        int32 nControllerIndex = sgtHeroSelectionGlobals.tHeroListDataSource.nControllerIndex;
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



































































































































































































































































































































































































































void OnHeroSelectionChanged(const UIID &, const int32, const int32 nNewVal)
{
    if (nNewVal != -1)
    {
        UpdateCharacterModel();
    }
}







void OnPartyHeroUpdated(int32 nControllerIndex, BOOL bSuccess)
{
    // the hero list can come back with input still disabled

    if (!sgtHeroSelectionGlobals.bEnableInput)
    {
        EnableInput(TRUE);
        UI_GetTextFromID(UITEXT_CONSOLE_HERO_SELECT_STATUS).SetText("", FALSE);
    }

    // tell the party about the new hero
    if (bSuccess && ::Console::Online::PartyIsInParty(nControllerIndex))
    {

        ::Console::Online::PartyUpdatePlayerHero(nControllerIndex);
        if (::Console::Online::PartyIsHost() &&
            ::Console::GamerProfile::GetPrimaryProfileUserIndex() == nControllerIndex &&
            GameIsStartUp())
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
    }
}

inline void OnLocalPartyMemberAdded(int32 nControllerIndex, BOOL bSuccess)
{
    // the local player is in the party now


    bIsAddingLocalPlayer = FALSE;

    if (GameIsStartUp())
    {
        UIGameSettings::Console::ClearGameSettingsViewed();
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


void OnLoadHeroFailed(int32 nControllerIndex, ::Console::GamerProfile::LoadHeroResult eResult)
{
    const char * szMessage;

    switch (eResult)
    {
    case ::Console::GamerProfile::LOAD_HERO_FAILED_HERO_IN_USE:
        szMessage = "ConsoleErrors:LoadHeroInUseError";
        break;
    case ::Console::GamerProfile::LOAD_HERO_FAILED_CORRUPT:
        szMessage = "ConsoleErrors:LoadHeroCorruptError";
        break;
    case ::Console::GamerProfile::LOAD_HERO_FAILED_CONTENT_MISSING:
        szMessage = "ConsoleErrors:LoadHeroContentMissingError";
        break;
    default:
        szMessage = "ConsoleErrors:LoadHeroFailedError";
        break;
    }

    NotificationCreationParams tParams;
    tParams.nControllerIndex = nControllerIndex;
    tParams.uszMessage = StringListFetch(szMessage);
    ::Console::Screens::GetModalNotificationManager().SetOkNotification(tParams);
}








void OnChosenHeroLoaded(int32 nControllerIndex, BOOL bLoadForRebirth, ::Console::GamerProfile::LoadHeroResult eResult)
{
    // the hero is loaded: input comes back either way


    EnableInput(TRUE);
    UI_GetTextFromID(UITEXT_CONSOLE_HERO_SELECT_STATUS).SetText("", FALSE);
    ::Console::UIOnlineActions::CreatePartyIfNeeded(nControllerIndex);

    if (!CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
    {
        if (eResult == ::Console::GamerProfile::LOAD_HERO_SUCCESS)
        {
            if (bLoadForRebirth == TRUE)
            {
                if (!GameIsStartUp())
                {
                    // the rebirthed hero must still fit the game in progress

                    BOOL bIsHardcore = ::Console::GamerProfile::GetHero(nControllerIndex).digest().player_flags() & (1 << PLAYERFLAG_HARDCORE);
                    ::Console::Online::GameParams tGameParams;
                    ::Console::Online::PartyGetGameParams(tGameParams);
                    if (bIsHardcore != (tGameParams.m_bIsHardcore != FALSE))
                    {

                        OnLoadHeroFailed(nControllerIndex, ::Console::GamerProfile::LOAD_HERO_FAILED);
                        return;
                    }


                }

                ::Console::SaveMigration::RebirthHero(nControllerIndex, uCachedSeasonAtConfirmation, sgtHeroSelectionGlobals.unk_170c);
            }


            if (!::Console::Online::PartyIsInParty(nControllerIndex))
            {
                ::Console::UIOnlineActions::FinalizeLocalPlayerJoin(nControllerIndex,
                    OnlineService::internal::function_binder_2<void, int32, BOOL>::bind<&OnLocalPartyMemberAdded>(nControllerIndex));
            }
            else
            {

                OnPartyHeroUpdated(nControllerIndex, TRUE);
                OnLocalPartyMemberAdded(nControllerIndex, TRUE);
            }
        }
        else
        {
            OnLoadHeroFailed(nControllerIndex, eResult);
        }
    }
    else
    {
        ::Console::Online::PartyUpdatePlayerHero(nControllerIndex);
        ::Console::Online::PartyStartGame();
    }
}


inline void OnArchiveReplaceComplete(int32 nControllerIndex)
{
    UIHallOfFallenHeroes::Console::OpenForController(nControllerIndex);
    OnConfirmDeleteHero(nControllerIndex, "Console:HardcoreArchiving", GetUIIDNone());
}



void OnArchiveWarningAccepted(int32 nControllerIndex, int32 nHeroIndex, const UIID & id)
{
    const ::Console::GamerProfile::HeroInfo & tHeroInfo = sgtHeroSelectionGlobals.tHeroListDataSource.tHeroInfoList[nHeroIndex];
    D3::Console::Profile & tProfile = ::Console::GamerProfile::GetProfile(nControllerIndex);



    if (tProfile.fallen_heroes_size() < 5)
    {
        // there is room in the hall: archive the hero, then delete it
        D3::Console::HeroInfo * ptNewHero = tProfile.add_fallen_heroes();
        tHeroInfo.Export(*ptNewHero);


        ::Console::GamerProfile::Save(nControllerIndex, OnlineService::Callback<void ()>());
        OnArchiveReplaceComplete(nControllerIndex);
    }
    else
    {
        // the hall is full: let the player pick a hero to replace
        UIHallOfFallenHeroes::Console::OpenForController(nControllerIndex, tHeroInfo.uHeroId, OnlineService::internal::function_binder_1<void, int32>::bind<&OnArchiveReplaceComplete>(nControllerIndex));
    }
}


void ArchiveHero(int32 nControllerIndex, int32 nHeroIndex)
{
    const ::Console::GamerProfile::HeroInfo & tHeroInfo = sgtHeroSelectionGlobals.tHeroListDataSource.tHeroInfoList[nHeroIndex];

    if (tHeroInfo.uLevel < 10)
    {
        return;
    }

    NotificationCreationParams tParams;
    tParams.nControllerIndex = nControllerIndex;
    tParams.uszMessage = LocalizedSprintf(StringListFetch("Console:HardcoreArchive").str(), tHeroInfo.szName);
    tParams.tCallbacks.pfnOnClickOkButton = OnlineService::internal::function_binder_3<void, int32, int32, const UIID &>::bind<&OnArchiveWarningAccepted>(nControllerIndex, nHeroIndex);
    ::Console::Screens::GetModalNotificationManager().SetOkCancelNotification(tParams);
}





// DWARF declares tHeroInfo (1563) before tAccountDefinition (1564); the body is not yet on its DWARF lines.
#line 1518
void OnPlayHero(const UIID &, const int32 nControllerIndex)
{

    if (sgtHeroSelectionGlobals.tHeroListDataSource.nHeroCount == 0 ||
        ::Console::Screens::IsScreenTransitioning() ||
        !sgtHeroSelectionGlobals.bEnableInput)
    {
        return;
    }

    UIConsoleListBox & tHeroList = UI_GetConsoleListBoxFromID(UILIST_CONSOLE_HERO_LIST);
    if (tHeroList.GetNumItems() <= 0)
    {
        return;
    }

    int32 nIndex = tHeroList.GetSelectedIndex();
    if (nIndex == -1)
    {
        return;
    }

    if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT) && !tBlizzconGame.bReadyToPlay)
    {
        if (tBlizzconModal.IsShowing())
        {
            return;
        }

        bool bInParty = ::Console::Online::PartyGetMemberCount() > 1;
        if (bInParty)
        {
            tBlizzconGame.playGameRequestTimer.Start(2000);
        }
        else
        {
            tBlizzconGame.playGameRequestTimer.Start(8000);
        }
        tBlizzconModal.SetMessage("Waiting for party");
        return;
    }

    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SELECT_GENERIC), cgtPSODefaults);

    const ::Console::GamerProfile::HeroInfo & tHeroInfo = sgtHeroSelectionGlobals.tHeroListDataSource.tHeroInfoList[nIndex];
    D3::Account::SavedDefinition & tAccountDefinition = ::Console::GamerProfile::GetAccount(nControllerIndex);

    BOOL bIsDeceased = tHeroInfo.uPlayerFlags & (1 << PLAYERFLAG_DECEASED);
    if (bIsDeceased)
    {
        if (GameIsStartUp())
        {
            ArchiveHero(nControllerIndex, nIndex);
        }
        return;
    }

    if (tHeroInfo.eClass == PLAYER_CLASS_NECROMANCER && !bHaveNecroLicense)
    {
        UIMainMenu::Console::OpenStoreToNecromancer(nControllerIndex);
        return;
    }

    if (IsHardcoreMismatchInGameOnly(tHeroInfo))
    {
        CRefString sMsg;
        if (sgtAppGlobals.dwGameCreateParamFlags & (1 << GAMECREATEPARAM_HARDCORE))
        {
            sMsg = "ConsoleErrors:HeroValidationMustBeHardcoreDetail";
        }
        else
        {
            sMsg = "ConsoleErrors:HeroValidationMustNotBeHardcoreDetail";
        }

        NotificationCreationParams tParams;
        tParams.nControllerIndex = nControllerIndex;
        tParams.uszMessage = StringListFetch(sMsg.str());
        ::Console::Screens::GetModalNotificationManager().SetOkNotification(tParams);
        return;
    }

    if (IsSeasonalMismatchInGameOnly(tHeroInfo, tAccountDefinition))
    {
        CRefString sMsg;
        if (sgtAppGlobals.dwGameCreateParamFlags & (1 << GAMECREATEPARAM_SEASON))
        {
            sMsg = "ConsoleErrors:HeroValidationMustBeSeasonalDetail";
        }
        else
        {
            sMsg = "ConsoleErrors:HeroValidationMustNotBeSeasonalDetail";
        }

        NotificationCreationParams tParams;
        tParams.nControllerIndex = nControllerIndex;
        tParams.uszMessage = StringListFetch(sMsg.str());
        ::Console::Screens::GetModalNotificationManager().SetOkNotification(tParams);
        return;
    }

    if (!::Console::Online::IsSeasonsInitialized() && ::Console::UIOnlineActions::IsActiveSeason(tHeroInfo.uSeasonCreated, tAccountDefinition.digest()))
    {
        CRefString sMsg;
        sMsg = "ConsoleErrors:HeroValidationSeasonsNotReady";

        NotificationCreationParams tParams;
        tParams.nControllerIndex = nControllerIndex;
        tParams.uszMessage = StringListFetch(sMsg.str());
        ::Console::Screens::GetModalNotificationManager().SetOkNotification(tParams);
        return;
    }

    if (!IsHeroSelectable(tHeroInfo, tAccountDefinition))
    {
        return;
    }

    tAccountDefinition.mutable_digest()->mutable_last_played_hero_id()->set_id_high(0);
    tAccountDefinition.mutable_digest()->mutable_last_played_hero_id()->set_id_low(sgtHeroSelectionGlobals.idHero);

    EnableInput(FALSE);
    CRefString sMessage = StringListFetch("ConsoleHeroSelect:LoadingHeroMessage");
    UI_GetTextFromID(UITEXT_CONSOLE_HERO_SELECT_STATUS).SetText(sMessage, FALSE);

    if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
    {
        ::Console::GamerProfile::SetBlizzconHero(nControllerIndex, sgtHeroSelectionGlobals.idHero);
    }

    LanguageRules::SetClientGender(!(tHeroInfo.uPlayerFlags & (1 << PLAYERFLAG_FEMALE)));

    ::Console::GamerProfile::LoadHero(nControllerIndex, sgtHeroSelectionGlobals.idHero,
                                      OnlineService::internal::function_binder_3<void, int32, BOOL, ::Console::GamerProfile::LoadHeroResult>::bind<&OnChosenHeroLoaded>(nControllerIndex, FALSE));
}
















inline void OnNewHeroWarningAccepted(int32 nControllerIndex, const UIID & id)
{
    ::Console::Screens::OpenToScreen((::Console::Screens::ScreenSets)::Console::Screens::HERO_CREATE_SCREEN_SET, 0, nControllerIndex, FALSE);
}







void OnNewHero(const UIID &, const int32 nControllerIndex)
{

    if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT) || !sgtHeroSelectionGlobals.bEnableInput)
    {
        return;
    }

    uint32 uNumHeroes = ::Console::GamerProfile::GetNumAvailableHeroes(nControllerIndex);
    if ((int32)uNumHeroes < sgtHeroSelectionGlobals.nMaxHeroes && !::Console::Screens::IsScreenTransitioning() && !sgtHeroSelectionGlobals.bFetchingHeroes)
    {
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SELECT_GENERIC), cgtPSODefaults);

        if (!::Console::GamerProfile::IsSignedIn(nControllerIndex) || ::Console::GamerProfile::IsGuest(nControllerIndex))
        {
            NotificationCreationParams tParams;
            tParams.nControllerIndex = nControllerIndex;
            tParams.uszMessage = StringListFetch("ConsoleErrors:CreateHeroNoProfileErrorNX64");
            tParams.tCallbacks.pfnOnClickOkButton = OnlineService::internal::function_binder_2<void, int32, const UIID &>::bind<&OnNewHeroWarningAccepted>(nControllerIndex);
            ::Console::Screens::GetModalNotificationManager().SetOkCancelNotification(tParams);
        }
        else
        {
            OnNewHeroWarningAccepted(nControllerIndex, GetUIIDNone());
        }
    }
}




















void OnLoadedNewHeroAfterDeletion(int32 nControllerIndex, ::Console::GamerProfile::LoadHeroResult eResult)
{
    // the screen may have been closed while the hero was loading
    if (!::Console::Screens::IsScreenOpen(::Console::Screens::HERO_SELECT_SCREEN_SET, 0))
    {
        return;
    }

    OnPartyHeroUpdated(nControllerIndex, eResult == ::Console::GamerProfile::LOAD_HERO_SUCCESS);
    UI_GetTextFromID(UITEXT_CONSOLE_HERO_SELECT_STATUS).SetText("", FALSE);
    EnableInput(TRUE);
    PopulateHeroList(nControllerIndex, FALSE);
    UpdateCharacterModel();

    if (eResult != ::Console::GamerProfile::LOAD_HERO_SUCCESS)
    {
        OnLoadHeroFailed(nControllerIndex, eResult);
    }
}






void OnDeleteHeroComplete(int32 nControllerIndex, int32 nIndex, OnlineService::HeroId idDeletedHero, OnlineService::HeroId idCurrentHero, ::Console::GamerProfile::DeleteHeroResult eResult)
{
    if (eResult != ::Console::GamerProfile::DELETE_HERO_SUCCESS)
    {
        const char * szMessage;
        switch (eResult)
        {
        case ::Console::GamerProfile::DELETE_HERO_FAILED_CONTENT_MISSING:
            szMessage = "ConsoleErrors:DeleteHeroFailedContentMissingError";
            break;
        default:
            szMessage = "ConsoleErrors:DeleteHeroFailedError";
            break;
        }

        {
            NotificationCreationParams tParams;
            tParams.nControllerIndex = nControllerIndex;
            tParams.uszMessage = StringListFetch(szMessage);
            ::Console::Screens::GetModalNotificationManager().SetOkNotification(tParams);
        }

        UI_GetTextFromID(UITEXT_CONSOLE_HERO_SELECT_STATUS).SetText("", FALSE);
        EnableInput(TRUE);
        return;
    }











    if (nIndex >= 0 && nIndex <= sgtHeroSelectionGlobals.tHeroListDataSource.nHeroCount)
    {
        sgtHeroSelectionGlobals.tHeroListDataSource.nHeroCount--;
        for (int32 i = nIndex; i < sgtHeroSelectionGlobals.tHeroListDataSource.nHeroCount; ++i)
        {
            sgtHeroSelectionGlobals.tHeroListDataSource.tHeroInfoList[i] = sgtHeroSelectionGlobals.tHeroListDataSource.tHeroInfoList[i + 1];
        }
    }





    RefreshHeroSlotsDisplay();

    OnlineService::HeroId idNew = OnlineService::INVALID_HERO_ID;

    if (idCurrentHero != OnlineService::INVALID_HERO_ID && idCurrentHero == idDeletedHero && sgtHeroSelectionGlobals.tHeroListDataSource.nHeroCount > 0)
    {
        // select the hero that took the deleted one's place
        int32 nHeroIndex = nIndex;
        if (nHeroIndex >= sgtHeroSelectionGlobals.tHeroListDataSource.nHeroCount)
        {
            nHeroIndex--;
        }








        idNew = sgtHeroSelectionGlobals.tHeroListDataSource.tHeroInfoList[nHeroIndex].uHeroId;
    }




    if (sgtHeroSelectionGlobals.tHeroListDataSource.nHeroCount == 0)
    {
        EnableInput(TRUE);
        UI_GetTextFromID(UITEXT_CONSOLE_HERO_SELECT_STATUS).SetText("", FALSE);

        if (GameIsStartUp() || GameGetDifficultyLevel() <= 0)
        {
            ::Console::Screens::OpenToScreen((::Console::Screens::ScreenSets)::Console::Screens::HERO_CREATE_SCREEN_SET, 0, nControllerIndex, FALSE);
        }
        else
        {
            OnBack(GetUIIDNone(), nControllerIndex);
        }
        return;
    }





    if (idNew != OnlineService::INVALID_HERO_ID)
    {
        D3::Account::SavedDefinition & tAccountDefinition = ::Console::GamerProfile::GetAccount(nControllerIndex);

        tAccountDefinition.mutable_digest()->mutable_last_played_hero_id()->set_id_high(0);
        tAccountDefinition.mutable_digest()->mutable_last_played_hero_id()->set_id_low(idNew);


        ::Console::GamerProfile::LoadHero(nControllerIndex, idNew, OnlineService::internal::function_binder_2<void, int32, ::Console::GamerProfile::LoadHeroResult>::bind<&OnLoadedNewHeroAfterDeletion>(nControllerIndex));
    }
    else
    {
        UI_GetTextFromID(UITEXT_CONSOLE_HERO_SELECT_STATUS).SetText("", FALSE);
        EnableInput(TRUE);

        PopulateHeroList(nControllerIndex, FALSE);
        UpdateCharacterModel();
    }
}


void OnConfirmDeleteHero(const int32 nControllerIndex, CRefString sStatusStringKey, const UIID & id)
{
    // the screen may have been closed while the warning was up

    if (::Console::Screens::IsScreenOpen(::Console::Screens::HERO_SELECT_SCREEN_SET, 0))
    {
        // delete the selected hero






        UIConsoleListBox & tHeroList = UI_GetConsoleListBoxFromID(UILIST_CONSOLE_HERO_LIST);

        int32 nIndex = tHeroList.GetSelectedIndex();

        OnlineService::HeroId idDeletedHero = sgtHeroSelectionGlobals.tHeroListDataSource.tHeroInfoList[nIndex].uHeroId;
        OnlineService::HeroId idCurrentHero = OnlineService::INVALID_HERO_ID;
        if (::Console::GamerProfile::IsHeroLoaded(nControllerIndex))
        {
            idCurrentHero = ::Console::GamerProfile::GetHero(nControllerIndex).digest().hero_id().id_low();
        }

        EnableInput(FALSE);
        CRefString sMessage = StringListFetch(sStatusStringKey);
        UI_GetTextFromID(UITEXT_CONSOLE_HERO_SELECT_STATUS).SetText(sMessage, FALSE);

        ::Console::GamerProfile::DeleteHero(nControllerIndex, idDeletedHero, OnlineService::internal::function_binder_5<void, int32, int32, OnlineService::HeroId, OnlineService::HeroId, ::Console::GamerProfile::DeleteHeroResult>::bind<&OnDeleteHeroComplete>(nControllerIndex, nIndex, idDeletedHero, idCurrentHero));

        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_HERO_DELETE), cgtPSODefaults);
    }
}

void OnDeleteHero(const UIID &, const int32 nControllerIndex)
{

    if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT) || !sgtHeroSelectionGlobals.bEnableInput)
    {
        return;
    }

    UIConsoleListBox & tHeroList = UI_GetConsoleListBoxFromID(UILIST_CONSOLE_HERO_LIST);
    if (tHeroList.GetNumItems() != 0 && !::Console::Screens::IsScreenTransitioning())
    {
        NotificationCreationParams tParams;
        tParams.nControllerIndex = nControllerIndex;
        tParams.tAcceptButtonDelay = 3000;
        tParams.uszMessage = StringListFetch("ConsoleHeroSelect:HeroDeleteWarning");
        tParams.tCallbacks.pfnOnClickOkButton = OnlineService::internal::function_binder_3<void, int32, CRefString, const UIID &>::bind<&OnConfirmDeleteHero>(nControllerIndex, "ConsoleHeroSelect:DeletingHeroMessage");
        ::Console::Screens::GetModalNotificationManager().SetOkCancelNotification(tParams);
    }
}






void OnBack(const UIID &, const int32 nControllerIndex)
{

    if (::Console::Screens::IsScreenTransitioning() || !sgtHeroSelectionGlobals.bEnableInput)
    {
        return;
    }

    int32 nProfileUserIdx = ::Console::GamerProfile::GetPrimaryProfileUserIndex();
    if (GameIsStartUp() && nProfileUserIdx == nControllerIndex)
    {
        if (!::Console::GamerProfile::IsHeroLoaded(nControllerIndex) ||
            (::Console::GamerProfile::GetHero(nControllerIndex).digest().player_flags() & (1 << PLAYERFLAG_DECEASED)))
        {
            ::Console::Online::ClearOutstandingInvite();
            ::Console::Screens::OpenToScreen((::Console::Screens::ScreenSets)::Console::Screens::MAIN_MENU_SCREEN_SET, 0, -1, FALSE);
        }
        else if (!CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
        {
            ::Console::Screens::OpenToScreen((::Console::Screens::ScreenSets)::Console::Screens::MAIN_MENU_SCREEN_SET, 1, nControllerIndex, FALSE);
        }
        else
        {
            ::Console::Screens::OpenToScreen((::Console::Screens::ScreenSets)::Console::Screens::MAIN_MENU_SCREEN_SET, 0, nControllerIndex, FALSE);
        }
    }
    else
    {
        ::Console::Screens::GoToPreviousScreenSet(nProfileUserIdx, FALSE);

        // a local player who backs out without a usable hero leaves the party
        if (!::Console::GamerProfile::IsHeroLoaded(nControllerIndex) ||
            (::Console::GamerProfile::GetHero(nControllerIndex).digest().player_flags() & (1 << PLAYERFLAG_DECEASED)) ||
            !GameIsStartUp())
        {
            if (::Console::Online::PartyIsInParty(nControllerIndex))
            {
                ::Console::Online::PartyRemovePlayer(nControllerIndex);
            }
            ::Console::GamerProfile::SignOut(nControllerIndex);
        }
    }
}





















void OnConfirmRebirthHero(const UIID & id, const int32 nControllerIndex)
{

    if (!::Console::Screens::IsScreenOpen(::Console::Screens::HERO_SELECT_SCREEN_SET, 0))
    {
        return;
    }

    // rebirth the selected hero into the current season


    UIConsoleListBox & tHeroList = UI_GetConsoleListBoxFromID(UILIST_CONSOLE_HERO_LIST);

    // the list can be out of date: check the hero again

    int32 nIndex = tHeroList.GetSelectedIndex();
    const ::Console::GamerProfile::HeroInfo & tHeroInfo = sgtHeroSelectionGlobals.tHeroListDataSource.tHeroInfoList[nIndex];

    BOOL bDeceased = tHeroInfo.uPlayerFlags & (1 << PLAYERFLAG_DECEASED);

    if (!bDeceased &&
        ::Console::Online::IsSeasonsInitialized() &&
        OnlineService::GetSeasonState() == OnlineService::SEASON_STATE_ACTIVE &&
        OnlineService::GetSeasonNum() != tHeroInfo.uSeasonCreated)
    {
        if (!bSeasonConfirmedAvailable)
        {
            NotificationCreationParams tParams;
            tParams.uszMessage = StringListFetch("ConsoleHeroCreate:SeasonNotAvailable");
            if (!::Console::GamerProfile::IsSignedInOnline(nControllerIndex))
            {
                ::Console::GamerProfile::OnlinePlayPrivilege ePrivilege = ::Console::GamerProfile::GetOnlinePlayPrivilege(nControllerIndex);
                if (ePrivilege == ::Console::GamerProfile::ONLINE_PLAY_BLOCKED_PARENTAL_CONTROLS)
                {
                    // parental controls block online play




                    tParams.uszMessage = StringListFetch("ConsoleErrors:OnlineStateErrorParentalBlockSwitch");
                }
            }






            tParams.nControllerIndex = nControllerIndex;
            ::Console::Screens::GetModalNotificationManager().SetOkNotification(tParams);
        }
        else
        {
            ::Console::GamerProfile::LoadHero(nControllerIndex,

                                              sgtHeroSelectionGlobals.idHero,
                                              OnlineService::internal::function_binder_3<void, int32, BOOL, ::Console::GamerProfile::LoadHeroResult>::bind<&OnChosenHeroLoaded>(nControllerIndex, TRUE));
        }
    }
}


inline BOOL HasModalCooldownPassed()
{
    int32 nMillisecondsPassed = Blizzard::Time::Milliseconds() - sgtHeroSelectionGlobals.nModalOpenTime;
    return nMillisecondsPassed > REBIRTH_COOLDOWN_MILLISECS;
}



void RefreshResetWarning(const UIID & id)
{
    UIID idAcceptText, idAcceptButton, idCancelText, idCancelButton;

    idAcceptText = UIIDCreateFromParent(id, ".AcceptText");
    idAcceptButton = UIIDCreateFromParent(id, ".Button3");

    UIWindow & tAcceptButton = UI_GetWindowFromID(idAcceptButton);
    UIText & tAcceptText = UI_GetTextFromID(idAcceptText);

    int32 nMillisecondsPassed = Blizzard::Time::Milliseconds() - sgtHeroSelectionGlobals.nModalOpenTime;

    if (HasModalCooldownPassed())
    {
        tAcceptButton.SetTransparency(0xff);
        tAcceptText.SetTransparency(0xff);

        tAcceptText.SetText(StringListFetch("Console:HardcoreAccept"), FALSE);
    }
    else
    {
        // count down the seconds until the accept button works
        tAcceptButton.SetTransparency(0x40);
        tAcceptText.SetTransparency(0x40);

        CRefString sAcceptText;
        sAcceptText.Format("%s (%s)",
                           StringListFetch("Console:HardcoreAccept").str(),
                           UnicodeText::FormatNumber((REBIRTH_COOLDOWN_MILLISECS - nMillisecondsPassed) / 1000 + 1, TRUE, FNS_NORMAL, XLOCALE_INVALID).str());
        tAcceptText.SetText(sAcceptText, FALSE);
    }
}



void OnOpenModalWarning(const UIID & id, const int32)
{
    sgtHeroSelectionGlobals.nModalOpenTime = Blizzard::Time::Milliseconds();
    RefreshResetWarning(id);
}





void OnUpdateModalWarning(const UIID & id, const int32)
{
    RefreshResetWarning(id);
}




void OnAcceptModalWarning(const UIID & id, const int32 nControllerIndex)
{
    if (!HasModalCooldownPassed())
        return;

    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SELECT_GENERIC), cgtPSODefaults);




    ::Console::Screens::CloseModalDialog(UIWND_CONSOLE_HEROSELECT_REBIRTH_WARNING);
    OnConfirmRebirthHero(GetUIIDNone(), nControllerIndex);
}


void OnCancelModalWarning(const UIID &, const int32)
{
    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_PANEL_GENERIC_CLOSE), cgtPSODefaults);

    ::Console::Screens::CloseModalDialog(UIWND_CONSOLE_HEROSELECT_REBIRTH_WARNING);
}




void OnRebirth(const UIID &, const int32 nControllerIndex, const BOOL bSolo)
{

    if (sgtHeroSelectionGlobals.tHeroListDataSource.nHeroCount == 0 ||
        ::Console::Screens::IsScreenTransitioning() ||
        !sgtHeroSelectionGlobals.bEnableInput)
    {
        return;
    }

    UIConsoleListBox & tHeroList = UI_GetConsoleListBoxFromID(UILIST_CONSOLE_HERO_LIST);
    if (tHeroList.GetNumItems() <= 0)
    {
        return;
    }

    int32 nSelectedIndex = tHeroList.GetSelectedIndex();
    if (nSelectedIndex == -1)
    {
        return;
    }

    if (!GameIsStartUp() && !GameIsSeasonal())
    {
        return;
    }

    int32 nIndex = UI_GetConsoleListBoxFromID(UILIST_CONSOLE_HERO_LIST).GetSelectedIndex();

    const ::Console::GamerProfile::HeroInfo & tHeroInfo = sgtHeroSelectionGlobals.tHeroListDataSource.tHeroInfoList[nIndex];
    BOOL bDeceased = tHeroInfo.uPlayerFlags & (1 << PLAYERFLAG_DECEASED);
    if (bDeceased)
    {
        return;
    }

    if (!bRequestedSeasonAvailability)
    {
        bSeasonConfirmedAvailable = FALSE;
        bRequestedSeasonAvailability = TRUE;
        uCachedSeasonAtConfirmation = 0;
        ::Console::Online::IsSeasonInProgress(nControllerIndex, OnlineService::internal::function_bind_0_1<void, BOOL, &sSeasonConfirmationCallback>());
    }

    if (tHeroInfo.eClass == PLAYER_CLASS_NECROMANCER && !(::Console::DLCManager::GetLicenses() & (1 << LICENSE_NECROMANCER)))
    {
        NotificationCreationParams tParams;
        tParams.nControllerIndex = nControllerIndex;
        tParams.uszMessage = StringListFetch("ConsoleHeroSelect:NecromancerLicenseRequiredForRebirth");
        ::Console::Screens::GetModalNotificationManager().SetOkNotification(tParams);
    }
    else if (::Console::Online::IsSeasonsInitialized() &&
             OnlineService::GetSeasonState() == OnlineService::SEASON_STATE_ACTIVE &&
             OnlineService::GetSeasonNum() != tHeroInfo.uSeasonCreated)
    {
        BOOL bHardcore = tHeroInfo.uPlayerFlags & (1 << PLAYERFLAG_HARDCORE);
        UIText & tModalTitle = UI_GetTextFromID(UITEXT_CONSOLE_HEROSELECT_REBIRTH_WARNING_TITLE);
        UIText & tModalDesc = UI_GetTextFromID(UITEXT_CONSOLE_HEROSELECT_REBIRTH_WARNING_DESCRIPTION);

        if (!GameIsStartUp())
        {
            // a hardcore hero cannot be rebirthed into a softcore game, and the reverse
            ::Console::Online::GameParams tGameParams;
            ::Console::Online::PartyGetGameParams(tGameParams);
            if (bHardcore != (tGameParams.m_bIsHardcore != FALSE))
            {
                return;
            }
        }

        sgtHeroSelectionGlobals.unk_170c = bSolo;
        if (!bHardcore)
        {
            if (bSolo)
            {
                tModalTitle.SetText(StringListFetch("ConsoleHeroSelect:RebirthTitleSolo"), FALSE);
                tModalTitle.SetTextColor(RGBAColor(0x0a, 0x47, 0x00));
                tModalDesc.SetText(StringListFetch("ConsoleHeroSelect:HeroRebirthWarningSolo"), FALSE);
            }
            else
            {
                tModalTitle.SetText(StringListFetch("ConsoleHeroSelect:RebirthTitle"), FALSE);
                tModalTitle.SetTextColor(RGBAColor(0x0a, 0x47, 0x00));
                tModalDesc.SetText(StringListFetch("ConsoleHeroSelect:HeroRebirthWarning"), FALSE);
            }
        }
        else
        {
            if (bSolo)
            {
                tModalTitle.SetText(StringListFetch("ConsoleHeroSelect:RebirthTitleSolo"), FALSE);
                tModalTitle.SetTextColor(RGBAColor(0x3f, 0x00, 0x00));
                tModalDesc.SetText(StringListFetch("ConsoleHeroSelect:HardcoreRebirthDescriptionSolo"), FALSE);
            }
            else
            {
                tModalTitle.SetText(StringListFetch("ConsoleHeroSelect:RebirthTitle"), FALSE);
                tModalTitle.SetTextColor(RGBAColor(0x3f, 0x00, 0x00));
                tModalDesc.SetText(StringListFetch("ConsoleHeroSelect:HardcoreRebirthDescription"), FALSE);
            }
        }

        ::Console::Screens::ShowModalDialog(nControllerIndex, UIWND_CONSOLE_HEROSELECT_REBIRTH_WARNING, TRUE, GetUIIDNone());
    }
}

} }  // namespace UIHeroSelect::Console

// The 2.7.7 UI function table entries "UIHeroSelect_Rebirth" / "UIHeroSelect_RebirthSolo".
extern "C" void sub_710035ae80(const UIID & id, const int32 nControllerIndex)
{
    UIHeroSelect::Console::OnRebirth(id, nControllerIndex, FALSE);
}

extern "C" void sub_710035ae90(const UIID & id, const int32 nControllerIndex)
{
    UIHeroSelect::Console::OnRebirth(id, nControllerIndex, TRUE);
}

namespace UIHeroSelect { namespace Console {

}  // namespace Console
}  // namespace UIHeroSelect

namespace UIHeroSelect {
namespace Console {
#line 164 "X/Client/UserInterface/Console/UIConsoleHeroSelect.cpp"
void BlizzconModal::UpdateMessage()
{
    // Inference: the branch to the common epilogue is expressed as an early return.
#line 166
    if (m_sBaseMessage.empty())
        return;

    if (!m_tDotTimer.Started() || m_tDotTimer.Finished())
    {
        m_tDotTimer.Start(750);

        blz::string message = m_sBaseMessage;
        ++m_nDotCount;
        m_nDotCount %= m_nMaxDotCount + 1;
        // Inference: the inclusive loop bound reproduces the initial negative test and trailing compare.
#line 176
        for (int i = 0; i <= m_nMaxDotCount; ++i)
            message.append(i < m_nDotCount ? "." : " ");

        if (m_tWaitForPartyModalID == INVALID_MODAL_NOTIFICATION_ID)
        {
            NotificationCreationParams params(message.c_str(), NOTIFICATIONLEVEL_NORMAL);
            m_tWaitForPartyModalID = ::Console::Screens::GetModalNotificationManager().SetMessageOnlyNotification(params);
        }
        else
        {
            ::Console::Screens::GetModalNotificationManager().ChangeMessage(m_tWaitForPartyModalID, message.c_str());
        }

    }
}

void BlizzconModal::SetMessage(const char * message)
{
    // Inference: the equality branch to the epilogue is expressed as an early return.
#line 194
    if (m_sBaseMessage == message)
        return;

    m_sBaseMessage = message;
    m_tDotTimer.Clear();
    UpdateMessage();
}

} // namespace Console
} // namespace UIHeroSelect

namespace UIHeroSelect {
namespace Console {
// Inference: original modal timeout branch at 0x272124 uses unsigned strict greater-than.
// DWARF-only helper body is not independently checkable; UpdateMessage requires it.
// Inference: out-of-class spelling preserves existing physical lines while restoring result at DWARF line 109.
#line 107 "X/Client/UserInterface/Console/UIConsoleHeroSelect.cpp"
inline bool Timer::Finished()  // inline: DWARF abstract DIE only, no concrete DIE (r06 test)
{
    bool result = Started() && Read() > duration;
    return result;
}

} // namespace Console
} // namespace UIHeroSelect

#include "../../../../Contrib/Contrib/blz/include/blz/algo_seq.h"
#include "../../../../NintendoSDK/Include/nn/account/account_Types.h"
// Lock.h: timed Enter's original inline body calls this overload (Lock.cpp:50).
namespace Blizzard {
namespace Lock {
Mutex * MutexEnter(Time::Millisecond waitMs, Mutex & mutex);
}
}
// Online.h and OnlineActions.h: original DWARF declarations, kept owner-local.
namespace Console {
namespace Online {
void RefreshAdHocList();
SwitchUniqueID GetSwitchUniqueID();
BOOL PartyIsGameStarted();
void PartySwitchOnlineState(PartyOnlineState eOnlineState);
void PartyFindAdHoc(blz::list<PartyInfo> & tFindResult, uint32 uMaxParties);  // Online.cpp:3199
}
namespace GamerProfile {
nn::account::Nickname GetUserNickName(int32 nUserIndex);
}
namespace UIOnlineActions {
void ClearJoinAdHocNetworkInProgress();
BOOL IsJoinPartyInProgress();
void JoinParty(Online::PartyConnectionInfo & tConnectionInfo, Online::PartyJoinCallback pfnCallback);
}
}
namespace UIHeroSelect {
namespace Console {
// Inference: the range-for at line 850 has no element DIE and only its traversal survives
// (0x270930-0x270950); its body compiled to nothing. Element name `p` is invented (column 29
// of the range expression fixes an 11-character declarator).
// Inference: limit 4 is the literal compared in the original (orr w1, wzr, #0x4); the
// predicate's == 4 is the literal at 0x270988. No named constant found.
// NON_MATCHING: 768 vs 768 bytes; 19/192 insns differ, register allocation only (end-of-list address held in x19/x23 vs x23/x25, one extra callee-saved x26).
#line 836 "X/Client/UserInterface/Console/UIConsoleHeroSelect.cpp"
void RefreshAdHocList()
{

    if (!Blizzard::Lock::MutexEnter(0, tBlizzconGame.tFindMutex))
        return;



    Blizzard::Lock::ScopedRMutex lock(tBlizzconGame.tFindMutex);
    tBlizzconGame.tFindMutex.Leave();

    tBlizzconGame.tFindResult.clear();
    ::Console::Online::PartyFindAdHoc(tBlizzconGame.tFindResult, 4);

    for (auto & p : tBlizzconGame.tFindResult)
    {
    }


    tBlizzconGame.tFindResult.erase(blz::remove_if(tBlizzconGame.tFindResult.begin(), tBlizzconGame.tFindResult.end(), [](const ::Console::Online::PartyInfo & info)
        {


            bool remove = info.m_UniqueID.Empty() || info.m_tGameInfo.m_uNumPlayers == 4;
            return remove;
        }), tBlizzconGame.tFindResult.end());

    tBlizzconGame.bFindResultDirty = true;
    tBlizzconGame.uAvailableParties = tBlizzconGame.tFindResult.size();
}
// Inference: empty aggregate assignment represents the original 16-byte ID clear.
#line 898
static void OnBlizzconPartyJoined(::Console::Online::JoinResult eJoinResult)
{
    ::Console::UIOnlineActions::ClearJoinAdHocNetworkInProgress();
    tBlizzconGame.szDesiredGameName[0] = '\0';
    tBlizzconGame.tDesiredGameID = {};
    tBlizzconGame.uAvailableParties = 0;
    if (tBlizzconGame.playGameRequestTimer.Started())
        tBlizzconGame.playGameRequestTimer.Start(1000);
}
#line 913
struct GameSort {
    bool in_game;
    int num_players;
    char name[64];
    ::Console::Online::SwitchUniqueID id;
};
#line 919
static const bool kSortByName = false;
#line 922
bool operator<(const GameSort & lhs, const GameSort & rhs)
{
    if (lhs.in_game)
        return true;
    if (rhs.in_game)
        return false;
    if (lhs.num_players != rhs.num_players)
        return lhs.num_players > rhs.num_players;
    return lhs.id < rhs.id;
}
// Inference: aggregate value initialization represents the original 88-byte zero fill.
#line 937
static GameSort GameSortFromPartyInfo(const ::Console::Online::PartyInfo & a)
{
    GameSort result = {};
    result.in_game = a.m_tGameInfo.m_bInGame;
    result.num_players = a.m_tGameInfo.m_uNumPlayers;
    Blizzard::String::Copy(result.name, a.m_szName, sizeof(result.name));
    result.id = a.m_UniqueID;
    return result;
}
// Inference: early returns express original branches to the common exit.
// Timed Mutex::Enter body is expanded because adopted Lock.h omits its overload.
#line 947
static void BlizzconJoinBestParty()
{
    Blizzard::Time::Millisecond now = Timer::Now();
#line 951
    bool bInGame = ::Console::Online::PartyGetMemberCount() > 1;
    if (!::Console::Online::PartyIsHost() || ::Console::UIOnlineActions::IsJoinPartyInProgress() || bInGame)
        return;
    if (::Console::Online::GetNetworkMode() != ::Console::Online::NET_ADHOC || ::Console::Online::PartyGetOnlineState() != ::Console::Online::PARTY_ADHOC)
    {
        ::Console::Online::PartySwitchOnlineState(::Console::Online::PARTY_ADHOC);
        return;
    }
    if (!tBlizzconGame.lastJoinCheckTimer.Started() || tBlizzconGame.lastJoinCheckTimer.Finished())
    {
        ::Console::Online::RefreshAdHocList();
        tBlizzconGame.lastJoinCheckTimer.Start(1500);
    }
    if (!tBlizzconGame.bFindResultDirty || !Blizzard::Lock::MutexEnter(0, tBlizzconGame.tFindMutex))
        return;
#line 979
    Blizzard::Lock::ScopedRMutex lock(tBlizzconGame.tFindMutex);
    tBlizzconGame.tFindMutex.Leave();
    tBlizzconGame.bFindResultDirty = false;
    if (::Console::Online::PartyGetMemberCount() < 2 && !tBlizzconGame.tFindResult.empty())
    {
#line 987
        tBlizzconGame.tFindResult.sort([](const ::Console::Online::PartyInfo & a, const ::Console::Online::PartyInfo & b) {
#line 989
            GameSort lhs = GameSortFromPartyInfo(a);
            GameSort rhs = GameSortFromPartyInfo(b);
            return lhs < rhs;
        });
#line 995
        nn::account::Nickname nickname = ::Console::GamerProfile::GetUserNickName(::Console::GamerProfile::GetPrimaryProfileUserIndex());
#line 997
        GameSort localGame = {};
        localGame.num_players = ::Console::Online::PartyGetMemberCount();
        localGame.id = ::Console::Online::GetSwitchUniqueID();
        Blizzard::String::Copy(localGame.name, nickname.name, sizeof(localGame.name));
#line 1004
        auto findResult = tBlizzconGame.tFindResult.begin();
        GameSort bestGame = GameSortFromPartyInfo(*findResult);
        if (bestGame < localGame)
        {
            if (tBlizzconGame.playGameRequestTimer.Started())
                tBlizzconGame.playGameRequestTimer.Start(5000);
#line 1018
            auto game = tBlizzconGame.tFindResult.begin();
            if (bestGame.id == tBlizzconGame.tDesiredGameID)
                ::Console::UIOnlineActions::JoinParty(*game, OnlineService::internal::function_bind_0_1<void, ::Console::Online::JoinResult, &OnBlizzconPartyJoined>());
            else
            {
                tBlizzconGame.tDesiredGameID = bestGame.id;
                Blizzard::String::Copy(tBlizzconGame.szDesiredGameName, game->m_szName, sizeof(tBlizzconGame.szDesiredGameName));
            }
        }
    }
}
#line 202
inline void BlizzconModal::CloseMessage()  // inline: DWARF abstract DIE only, no concrete DIE (r06 test)
{
    m_sBaseMessage.clear();
    ::Console::Screens::GetModalNotificationManager().RemoveNotification(m_tWaitForPartyModalID);
    m_tWaitForPartyModalID = INVALID_MODAL_NOTIFICATION_ID;
}
#line 1043
static void BlizzconLaunchGame(const UIID & id)
{
    tBlizzconModal.CloseMessage();
    tBlizzconGame.bReadyToPlay = true;
    OnPlayHero(id, sgtHeroSelectionGlobals.tHeroListDataSource.nControllerIndex);
}
}
}
// Input.h and UIIDs.h: owner-local DWARF declarations.
void InputRemapSingleJoyconButton(int32 eFrom, int32 eTo);
extern UIID UIWND_CONSOLE_HERO_LIST_UP_ARROW, UIWND_CONSOLE_HERO_LIST_DOWN_ARROW;
namespace UIHeroSelect {
namespace Console {
// UIConsoleHeroSelect.cpp: ScreenActions from DWARF.
enum ScreenActions {
    ACTION_SELECT = 0, ACTION_CONFIRM = 1, ACTION_ARCHIVE = 2,
    ACTION_BACK = 3, ACTION_NEW_HERO = 4, ACTION_DELETE_HERO = 5, ACTION_REBIRTH = 6
};
// NON_MATCHING: 4040 vs4056 bytes; adopted SwitchUniqueID inline comparisons and original lambda ordinal unresolved.
#line 1128 "X/Client/UserInterface/Console/UIConsoleHeroSelect.cpp"
void Update(const UIID & id)
{
    if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
    {
        BlizzconJoinBestParty();
        tBlizzconModal.UpdateMessage();
        if (tBlizzconModal.IsShowing() && tBlizzconGame.playGameRequestTimer.Finished())
        {
            if (::Console::Online::PartyIsHost())
            {
                BlizzconLaunchGame(id);
                return;
            }
            if (::Console::Online::PartyIsGameStarted())
            {
                tBlizzconModal.SetMessage("Host loading");
                if (!tBlizzconGame.hostLaunchTimer.Started())
                    tBlizzconGame.hostLaunchTimer.Start(1000);
                else if (tBlizzconGame.hostLaunchTimer.Finished())
                {
                    BlizzconLaunchGame(id);
                    return;
                }
            }
            else
                tBlizzconModal.SetMessage("Waiting for host to start the game");
        }
    }
    if (GameIsStartUp() && ::Console::Screens::IsModalDialogVisible())
        return;
#line 1181
    BOOL bForceRefresh = FALSE;
    if (sgtHeroSelectionGlobals.uSeasonNum != OnlineService::GetSeasonNum() || sgtHeroSelectionGlobals.eSeasonState != OnlineService::GetSeasonState())
    {
        sgtHeroSelectionGlobals.uSeasonNum = OnlineService::GetSeasonNum();
        sgtHeroSelectionGlobals.eSeasonState = OnlineService::GetSeasonState();
        bForceRefresh = TRUE;
    }
    if (bHaveNecroLicense != ((::Console::DLCManager::GetLicenses() >> 7) & 1))
    {
        bHaveNecroLicense = !bHaveNecroLicense;
        bForceRefresh = TRUE;
    }
    if (bForceRefresh)
    {
        RefreshHeroListAndData(sgtHeroSelectionGlobals.tHeroListDataSource.nControllerIndex, TRUE);
        PopulateHeroList(sgtHeroSelectionGlobals.tHeroListDataSource.nControllerIndex, TRUE);
        ::Console::Screens::GetActionManager().ClearAllActions();
        ::Console::Screens::UpdateButtonLegendText();
    }
#line 1206
    ::Console::ControllerActionManager & tActionManager = ::Console::Screens::GetActionManager();
    if (!sgtHeroSelectionGlobals.bFetchingHeroes && (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT) || !UIHallOfFallenHeroes::Console::IsVisible()))
    {
        InputClearRemappings();
#line 1213
        int32 nSelectedIndex = UI_GetConsoleListBoxFromID(UILIST_CONSOLE_HERO_LIST).GetSelectedIndex();
        if (nSelectedIndex != -1 && nSelectedIndex < sgtHeroSelectionGlobals.tHeroListDataSource.nHeroCount)
        {
#line 1217
            const ::Console::GamerProfile::HeroInfo & tHeroInfo = sgtHeroSelectionGlobals.tHeroListDataSource.tHeroInfoList[nSelectedIndex];
            BOOL bDeceased = (tHeroInfo.uPlayerFlags >> PLAYERFLAG_DECEASED) & 1;
            BOOL bValidForSeason = tHeroInfo.uSeasonCreated == 0 || (::Console::Online::IsSeasonsInitialized() && OnlineService::GetSeasonState() == OnlineService::SEASON_STATE_ACTIVE && OnlineService::GetSeasonNum() == tHeroInfo.uSeasonCreated) || OnlineService::GetSeasonState() != OnlineService::SEASON_STATE_ACTIVE;
            if (bDeceased)
            {
                if (tHeroInfo.uLevel >= 10 && GameIsStartUp())
                    tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_ARCHIVE, StringListFetch("Bnet_HeroSelect:ButtonArchiveHardcoreHero"));
                else
                    tActionManager.ClearAction(::Console::CONTROLLER_INPUT_BUTTON_1);
            }
            else if (tHeroInfo.eClass == PLAYER_CLASS_NECROMANCER && !bHaveNecroLicense)
                tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_CONFIRM, StringListFetch("ConsoleHeroSelect:NecroUpsell"));
            else if (bValidForSeason || GameIsStartUp() || GameIsWeeklyChallenge())
                tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_CONFIRM, StringListFetch("ConsoleHeroSelect:PlayHero"));
            else
                tActionManager.ClearAction(::Console::CONTROLLER_INPUT_BUTTON_1);
#line 1241
            bool bValidForRebirthOnDropin = false;
            if (!CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
            {
#line 1245
                BOOL bIsHardcore = tHeroInfo.uPlayerFlags & (1 << PLAYERFLAG_HARDCORE);
                ::Console::Online::GameParams tGameParams;
                ::Console::Online::PartyGetGameParams(tGameParams);
                bValidForRebirthOnDropin = bIsHardcore == (tGameParams.m_bIsHardcore != FALSE) && GameIsSeasonal();
            }
            if (!bDeceased && ::Console::Online::IsSeasonsInitialized() && OnlineService::GetSeasonState() == OnlineService::SEASON_STATE_ACTIVE && OnlineService::GetSeasonNum() != tHeroInfo.uSeasonCreated && (bValidForRebirthOnDropin || GameIsStartUp()))
            {
                // InputButton values from original call (GameSwig.h).
                InputRemapSingleJoyconButton(BUTTON_DPAD_RIGHT, BUTTON_RIGHT_STICK_BUTTON);
                tActionManager.SetAction(::Console::CONTROLLER_INPUT_RIGHT_STICK_BUTTON, ACTION_REBIRTH, StringListFetch("ConsoleHeroSelect:RebirthHero"));
            }
            else
                tActionManager.ClearAction(::Console::CONTROLLER_INPUT_RIGHT_STICK_BUTTON);
        }
        else
            tActionManager.ClearAction(::Console::CONTROLLER_INPUT_BUTTON_1);
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_BACK, StringListFetch("Console:Back"));
        if (!CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
            ::Console::Screens::GetActionManager().SetAction(::Console::CONTROLLER_INPUT_BUTTON_3, ACTION_DELETE_HERO, StringListFetch("ConsoleHeroSelect:DeleteHero"));
        if (!CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
        {
            if (sgtHeroSelectionGlobals.tHeroListDataSource.nHeroCount < sgtHeroSelectionGlobals.nMaxHeroes)
                tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_4, ACTION_NEW_HERO, StringListFetch("ConsoleHeroSelect:NewHero"));
            else
                tActionManager.ClearAction(::Console::CONTROLLER_INPUT_BUTTON_4);
        }
#line 1294
        UIConsoleListBox & tHeroList = UI_GetConsoleListBoxFromID(UILIST_CONSOLE_HERO_LIST);
        UI_GetWindowFromID(UIWND_CONSOLE_HERO_LIST_UP_ARROW).SetActive(tHeroList.GetFirstVisibleItemIndex() > 0);
        UI_GetWindowFromID(UIWND_CONSOLE_HERO_LIST_DOWN_ARROW).SetActive(tHeroList.GetLastVisibleItemIndex() + 1 != tHeroList.GetNumItems());
    }
    ::Console::Screens::UpdateButtonLegendText();
}
}
}
