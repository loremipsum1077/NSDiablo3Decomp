// Original file: X/Client/UserInterface/Console/UIConsoleOptionsDataSource.cpp
//
// The console options screen's list and selector data sources.

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Common/SNO/SNOName.h"
#include "../../../Common/WorldPlace.h" // Includes Vector.h; original initializes cgplaceNull before packed normals.
#include "../../../Sigma/RGBAColor.h"
#include "../../Gfx/GfxVB.h"
#include "../../../../NintendoSDK/Include/nn/ldn/ldn_Ipv4Address.h"
#include "../../../../NintendoSDK/Include/nn/ldn/ldn_MacAddress.h"
#include "../../../../Contrib/Contrib/blz/include/blz/array.h"
#include "UIConsoleOptionsDataSource.h"
#include "ConsoleScreenManager.h"
#include "ControllerActions.h"
#include "../UI.h"
#include "../UIID.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/Player.h"
#include "../../CPlayer.h"
#include "../../CGame.h"
#include "../../Sound/Sound.h"
#include "../../../Sigma/UnicodeText.h"
#include "../Controls/UITextControl.h"
#include "../../Console/GamerProfile.h"
#include "../../Console/Matchmaking.h"
#include "../../Definitions/PreferencesDefinition.h"
#include "../../../Common/Game.h"
#include "../../../Common/Definitions/StringListDefinition.h"

// Declared here because their headers are not adopted yet; each comment names the defining file.
extern UIID UILIST_CONSOLE_OPTIONS_LIST;  // X/Client/UserInterface/UIIDs.cpp
extern UIID UITXT_CONSOLE_OPTIONS_TOOLTIP;  // X/Client/UserInterface/UIIDs.cpp
extern blz::array<int, 10> g_tNX64TextLocales;  // X/Common/SNO/SNOFiles.cpp
extern blz::array<int, 11> g_tNX64AudioLocales;  // X/Common/SNO/SNOFiles.cpp
CRefString StringListFetch(LPCSTR szLabel);  // X/Common/Definitions/StringListDefinition.cpp
void CPlayerUpdateServerSidePrefs(Player * ptPlayer);  // X/Client/CPlayer.cpp
void CPlayerSendPrefsToServer(Player * ptPlayer);  // X/Client/CPlayer.cpp
namespace Console { namespace Utilities { XLocale GetSystemLanguage(); } }  // X/Client/Console/ConsoleUtilities.cpp
namespace Console { namespace GamerProfile { void SavePrefs(); } }  // X/Client/Console/GamerProfile.cpp
void UI_SetPrimaryUIController(int32 nPrimaryController);  // X/Client/UserInterface/UI.cpp
namespace UIControllerLayout { namespace Console { void SetOnCloseFunction(UIFunctionPtr pfnOnClose); void OpenForController(int32 nControllerIndex); } }  // UIConsoleControllerLayout.cpp
namespace UIBattlenetLink { void SetOnCloseFunction(UIFunctionPtr pfnOnClose); void OpenForController(int32 nControllerIndex, BOOL bInFirstTimeFlow); }  // UIConsoleBattlenetLink.cpp
namespace Console { namespace WebBrowser { BOOL NavigateTo(const CRefString & sUri, int32 nControllerIndex); } }  // X/Client/Console/WebBrowserNX64.cpp
namespace UIOptions { namespace Console { void OnBack(const UIID & id, int32 nControllerIndex); } }  // UIConsoleOptions.cpp
namespace UISafeZoneSettings { void SetOnCloseFunction(UIFunctionPtr pfnOnClose); void Open(const int32 nControllerIndex); }  // UIConsoleSafeZoneSettings.cpp
namespace UIGammaAdjust { namespace Console { void SetOnCloseFunction(const UIFunctionPtr pfnOnClose); void OpenForController(int32 nControllerIndex, BOOL bInFirstTimeFlow); } }  // UIConsoleGammaAdjust.cpp

namespace SoundManager {
void ApplyPrefSettings();  // X/Client/Sound/Sound.cpp
}
UIConsoleSelector & UI_GetConsoleSelectorFromID(const UIID & id);  // X/Client/UserInterface/UI.cpp

static int32 sFindLocaleOverrideIndex(int32 eLocale)
{
    for (int32 i = 0; i < (int32)g_tNX64TextLocales.size(); i++) { if (g_tNX64TextLocales[i] == eLocale) return i; }
    return 0;
}

static int32 sFindAudioLocaleOverrideIndex(int32 eLocale)
{
    for (int32 i = 0; i < (int32)g_tNX64AudioLocales.size(); i++) { if (g_tNX64AudioLocales[i] == eLocale) return i; }
    return 0;
}

#include "../Controls/UICheckBoxControl.h"
#include "../Controls/UIRActorControl.h"
#include "../UIUtils.h"
#include "../../Definitions/FontDefinition.h"
#include "../../Definitions/TextureDefinition.h"
#include "../../Prefetch.h"
UICheckBox & UI_GetCheckBoxFromID(const UIID &); // UI.cpp
UIRActor & UI_GetRActorFromID(const UIID &); // UI.cpp
void ShellShutdownUI(); // ShellCommon.cpp:8
void ShellRestoreUI(); // ShellCommon.cpp:29
namespace SNOManager {
void ReloadRequired(SNOGroup); // SNOManager.cpp
}








namespace UIOptions {
namespace Console {

struct ConsoleOptionsDataSourceGlobals {
    ConsoleOptionsListDataSource listData;

    ConsoleOptionsDataSourceGlobals()
    {

        listData.m_sListSourceKey.Format("ConsoleOptions:ConsoleOptions_main.LayoutRoot.List");
        listData.m_idList = UILIST_CONSOLE_OPTIONS_LIST;
        listData.m_idTooltip = UITXT_CONSOLE_OPTIONS_TOOLTIP;
        bChanged = FALSE;
    }
    ~ConsoleOptionsDataSourceGlobals()
    {

    }
    BOOL bChanged;
};
ConsoleOptionsDataSourceGlobals * gptConsoleOptionsDataSourceGlobals;

void OnReturnToVideoOptions(const UIID &);
void ConfigureLegendForOptions();
void OnAcceptSubMenuSelector(const Selection & tSelection);
void OnAcceptGameplay(const Selection & tSelection);
inline void OnAcceptVideo(const Selection & tSelection);
void SaveCheckboxDataFromList(SelectionArray & tList);
inline void SaveCheckboxData();
ConsoleOptionsListDataSource & GetListDataSource();
void OnReturnToOptions(const UIID &);
void SaveData();
void ApplyTextLocale();
inline void ApplyAudioLocale(int32 nUserIndex);





void OnCheckboxToggle(const UIID &);
void OnSelectorChanged(const UIID &);





inline Selection::Selection()
{

    m_eType = TEXT;
    m_bSelected = FALSE;
    m_eOptionID = NONE;
    m_bChecked = FALSE;
    m_bWrap = FALSE;
}


inline void Selection::Initialize(SelectionType type, CRefString sLabel, CRefString sToolTipLabel, OptionID eOptionID)
{




    m_sDisplayText = StringListFetch(sLabel);
    m_sToolTip = sToolTipLabel;
    m_eType = type;
    m_bSelected = FALSE;
    m_eOptionID = eOptionID;
    m_bChecked = FALSE;
    m_bWrap = FALSE;
}



inline ConsoleOptionsListDataSource::ConsoleOptionsListDataSource()
{

    RefreshSelections();
}

int32 ConsoleOptionsListDataSource::GetNumItems() const
{


    return m_ptActiveList->size();
}
UIControlSourceKey ConsoleOptionsListDataSource::GetSourceKeyForItem(const int32 nIndex) const
{

    CRefString sSourceKey;

    SelectionType eType = CHECKBOX;
    if (nIndex >= 0)
    {

        eType = (*m_ptActiveList)[nIndex].m_eType;
    }
        switch (eType)
        {
        case TEXT:
            sSourceKey.Format("%s.TextItem", m_sListSourceKey.str());
            break;
        case SELECTOR:
            sSourceKey.Format("%s.SelectorItem", m_sListSourceKey.str());
            break;
        case CHECKBOX:
            sSourceKey.Format("%s.CheckboxItem", m_sListSourceKey.str());
            break;
        }



    LPCSTR szSourceKey = sSourceKey.str();
    return UIControlSourceKey::Parse(szSourceKey, -1, NULL, NULL);
}



void ConsoleOptionsListDataSource::InitializeItemUI(
    const UIConsoleListBox & tListBox,
    const UIID & idItemUI,
    const int32 nIndex)
{
    if (nIndex >= 0) {
        UIID idText; // line 213
        UIID idCheckbox; // line 214
        UIID idSelector; // line 215
        switch ((*m_ptActiveList)[nIndex].m_eType) {
        case TEXT:
            idText.SetFromParent(idItemUI, "Text");
            break;
        case SELECTOR: {
            idText.SetFromParent(idItemUI, "SelectorText");
            idSelector.SetFromParent(idItemUI, "Selector");
            m_selectorData.m_eCurrentOptionID = (*m_ptActiveList)[nIndex].m_eOptionID;
            m_selectorData.m_nCurrentListItemIndex = nIndex;
            UIConsoleSelector & tSelector = UI_GetConsoleSelectorFromID(idSelector); // line 230
            tSelector.SetDataSource(m_selectorData);
            tSelector.SetOnSelChangeFunction(OnSelectorChanged);
            break;
        }
        case CHECKBOX: {
            idCheckbox.SetFromParent(idItemUI, "Checkbox");
            UICheckBox & tCheckbox = UI_GetCheckBoxFromID(idCheckbox); // line 239
            tCheckbox.SetText("");
            tCheckbox.SetOnClickFunction(OnCheckboxToggle);
            idText.SetFromParent(idItemUI, "CheckboxText");
            break;
        }
        }
        UI_GetTextFromID(idText).SetText((*m_ptActiveList)[nIndex].m_sDisplayText, FALSE);
    }
}
















void ConsoleOptionsListDataSource::UpdateItemUI(
    const UIConsoleListBox & tListBox,
    const UIID & idItemUI,
    const int32 nIndex)
{
    UIID idText;
    UIID idCheckbox;
    UIID idSelector;
    BOOL bSelected = tListBox.GetSelectedIndex() == nIndex; // line 264
    int32 nOldIndex = m_selectorData.m_nCurrentListItemIndex;
    OptionID eOldID = m_selectorData.m_eCurrentOptionID;
    m_selectorData.m_nCurrentListItemIndex = nIndex;
    Selection & tSelection = (*m_ptActiveList)[nIndex]; // line 269
    switch (tSelection.m_eType) {
    case TEXT:
        idText.SetFromParent(idItemUI, "Text");
        break;
    case CHECKBOX: {
        idText.SetFromParent(idItemUI, "CheckboxText");
        idCheckbox.SetFromParent(idItemUI, "Checkbox");
        UICheckBox & tCheckbox = UI_GetCheckBoxFromID(idCheckbox); // line 281
        tCheckbox.SetCheckBoxState(tSelection.m_bChecked, FALSE);
        tCheckbox.SetCheckBoxHoverState(bSelected);
        break;
    }
    case SELECTOR: {
        idText.SetFromParent(idItemUI, "SelectorText");
        idSelector.SetFromParent(idItemUI, "Selector");
        m_selectorData.m_eCurrentOptionID = tSelection.m_eOptionID;
        UIConsoleSelector & tSelector = UI_GetConsoleSelectorFromID(idSelector); // line 293
        tSelector.SetWrapping(tSelection.m_bWrap);
        m_selectorData.SetItemIndex(tSelector);
        break;
    }
    }
    UIText & tText = UI_GetTextFromID(idText); // line 300
    tText.SetText(tSelection.m_sDisplayText, FALSE);
    if (bSelected)
        tText.SetTextColor(crgbaWhite);
    else
        tText.SetTextColor(crgbaGold);
    if (bSelected) {
        UIRActor & tFilagree = UI_GetRActorFromID(m_idRActor); // line 313
        Vector2D vecFinalTL; // line 316
        Rect2D rectFilagree = tFilagree.GetWindowRect(); // line 317
        UIWindow & tItem = UI_GetWindowFromID(idItemUI); // line 318
        Rect2D rectItem = tItem.GetWindowRect(); // line 319
        AlignRect(Vector2D(rectItem.left, rectItem.top), Vector2D(rectItem.Width(), rectItem.Height()),
                  ALIGN_CENTER_VERTICAL, rectFilagree.left, Vector2D(rectFilagree.Width(), rectFilagree.Height()), vecFinalTL);
        tFilagree.MoveTo(vecFinalTL);
    }
    m_selectorData.m_eCurrentOptionID = eOldID;
    m_selectorData.m_nCurrentListItemIndex = nOldIndex;
}

























void ConsoleOptionsListDataSource::SetUIControls(CRefString sSourceKey, const UIID & idList, const UIID & idTooltip, const UIID & idRActor)
{

    m_sListSourceKey = sSourceKey;
    m_idList = idList;
    m_idTooltip = idTooltip;
    m_idRActor = idRActor;
}



XLocale m_eTextLocaleOverride;
XLocale m_eAudioLocaleOverride;
void ConsoleOptionsListDataSource::RefreshSelections()
{
    m_tSubMenuSelector.clear();
    m_tGameplay.clear();
    m_tVideo.clear();
    m_tAudio.clear();
    m_tMultiplayer.clear();



    Selection tGameplay, tVideo, tAudio, tMultiplayer, tController, tBattlenet;

    tGameplay.Initialize(TEXT, "GameOptions:GameplayTab", "", GAMEPLAY_TAB);
    tVideo.Initialize(TEXT, "GameOptions:VideoTab", "", VIDEO_TAB);
    tAudio.Initialize(TEXT, "GameOptions:SoundTab", "", AUDIO_TAB);
    tMultiplayer.Initialize(TEXT, "ConsoleOptions:MultiplayerTab", "", MULTIPLAYER_TAB);
    tController.Initialize(TEXT, "ConsoleOptions:ControllerTab", "", CONTROLS);
    tBattlenet.Initialize(TEXT, "ConsoleOptions:BattlenetTab", "", BATTLENET_TAB);

    m_tSubMenuSelector.push_back(tGameplay);
    m_tSubMenuSelector.push_back(tVideo);
    m_tSubMenuSelector.push_back(tAudio);
    m_tSubMenuSelector.push_back(tMultiplayer);
    m_tSubMenuSelector.push_back(tController);








    Selection tDisplayPlayerHealthBars, tDisplayMonsterHealthBars, tDisplayPlayerNames;
    Selection tDisplayHealingNumbers, tDisplayDamageNumbers, tDisplayCriticalDamageNumbers, tDisplayShortFloatingNumbers;
    Selection tDisplayDefensiveMessages, tAlwaysShowItemLabelsOnDrop, tShowTutorials;
    Selection tAutoEquipItems, tShowAdvancedTooltips, tElectiveMode, tSmartMinimap, tAlwaysShowCampaignObjectives, tAlwaysShowAdventureObjectives;
    Selection tItemRarityIcons;


    Selection tTextLocaleOverride;
    Selection tAudioLocaleOverride;
    if (GameIsStartUp())
    {
        tTextLocaleOverride.Initialize(SELECTOR, "GameOptions:TextLocaleOverride", "GameOptions:TextLocaleOverrideTooltip", TEXT_LANGUAGE_OVERRIDE);
        tAudioLocaleOverride.Initialize(SELECTOR, "GameOptions:AudioLocaleOverride", "GameOptions:AudioLocaleOverrideTooltip", AUDIO_LANGUAGE_OVERRIDE);
    }


    tElectiveMode.Initialize(CHECKBOX, "GameOptions:ElectiveModeToggle", "GameOptions:ElectiveModeTooltip", ELECTIVE_MODE);
    tDisplayPlayerHealthBars.Initialize(CHECKBOX, "GameOptions:PlayerHealthToggle", "GameOptions:PlayerHealthTooltip", DISPLAY_PLAYER_HEALTH_BARS);
    tDisplayMonsterHealthBars.Initialize(CHECKBOX, "GameOptions:MonsterHealthToggle", "GameOptions:MonsterHealthTooltip", DISPLAY_MONSTER_HEALTH_BARS);
    tDisplayPlayerNames.Initialize(CHECKBOX, "GameOptions:PlayerNamesToggle", "ConsoleOptions:PlayerHealthTooltip", DISPLAY_PLAYER_NAMES);
    tDisplayHealingNumbers.Initialize(CHECKBOX, "GameOptions:HealingNumbersToggle", "GameOptions:HealingNumbersTooltip", DISPLAY_HEALING_NUMBERS);
    tDisplayDamageNumbers.Initialize(CHECKBOX, "GameOptions:DamageNumbersToggle", "GameOptions:DamageNumbersTooltip", DISPLAY_DAMAGE_NUMBERS);
    tDisplayCriticalDamageNumbers.Initialize(CHECKBOX, "GameOptions:CriticalDamageNumbersToggle", "GameOptions:CriticalDamageNumbersTooltip", DISPLAY_CRITICAL_DAMAGE_NUMBERS);
    tDisplayShortFloatingNumbers.Initialize(CHECKBOX, "GameOptions:ShortFloatingNumbers", "GameOptions:ShortFloatingNumbersTooltip", DISPLAY_SHORT_FLOATING_NUMBERS);
    tDisplayDefensiveMessages.Initialize(CHECKBOX, "GameOptions:DefensiveMessagesToggle", "GameOptions:DefensiveMessagesTooltip", DISPLAY_DEFENSIVE_MESSAGES);
    tAlwaysShowItemLabelsOnDrop.Initialize(CHECKBOX, "GameOptions:ItemLabelsToggle", "GameOptions:ItemLabelsTooltip", ALWAYS_SHOW_ITEM_LABELS_ON_DROP);
    tShowTutorials.Initialize(CHECKBOX, "GameOptions:TutorialsToggle", "GameOptions:TutorialsTooltip", SHOW_TUTORIALS);
    tAutoEquipItems.Initialize(CHECKBOX, "GameOptions:AutoEquipToggle", "GameOptions:AutoEquipTooltip", AUTO_EQUIP_ITEMS);
    tShowAdvancedTooltips.Initialize(CHECKBOX, "GameOptions:AdvancedTooltipsToggle", "GameOptions:AdvancedTooltipsTooltip", SHOW_ADVANCED_TOOLTIPS);
    tSmartMinimap.Initialize(CHECKBOX, "GameOptions:SmartCoopMinimapTitle", "GameOptions:SmartCoopMinimapTooltip", SMART_MINIMAP);
    tAlwaysShowCampaignObjectives.Initialize(CHECKBOX, "GameOptions:ShowStoryObjectives_con", "GameOptions:ShowStoryObjectives_Tooltip_con", ALWAYS_SHOW_CAMPAIGN_OBJECTIVES);
    tAlwaysShowAdventureObjectives.Initialize(CHECKBOX, "GameOptions:ShowBountyObjectives_con", "GameOptions:ShowBountyObjectives_Tooltip_con", ALWAYS_SHOW_ADVENTURE_OBJECTIVES);
    tItemRarityIcons.Initialize(CHECKBOX, "GameOptions:ItemRarityIcons", "GameOptions:ItemRarityIconsTooltip", ITEM_RARITY_ICONS);


    const GameplayPrefs & tGameplayPrefs = Prefs::GetGameplayPrefs();

    tElectiveMode.m_bChecked = tGameplayPrefs.GetSkillElectiveMode();
    tDisplayPlayerHealthBars.m_bChecked = tGameplayPrefs.GetShowPlayerHPBars();
    tDisplayMonsterHealthBars.m_bChecked = tGameplayPrefs.GetShowMonsterHPBars();
    tDisplayPlayerNames.m_bChecked = tGameplayPrefs.GetShowPlayerNames();
    tDisplayHealingNumbers.m_bChecked = tGameplayPrefs.GetShowHealNumbers();
    tDisplayDamageNumbers.m_bChecked = tGameplayPrefs.GetShowDamageNumbers();
    tDisplayCriticalDamageNumbers.m_bChecked = tGameplayPrefs.GetShowCriticals();
    tDisplayShortFloatingNumbers.m_bChecked = tGameplayPrefs.GetDisableShortFloatingNumbers();
    tDisplayDefensiveMessages.m_bChecked = tGameplayPrefs.GetShowDefensiveMessages();
    tAlwaysShowItemLabelsOnDrop.m_bChecked = tGameplayPrefs.GetShowItemDroppedTooltips();
    tShowTutorials.m_bChecked = tGameplayPrefs.GetShowTutorials();
    tAutoEquipItems.m_bChecked = tGameplayPrefs.GetAutoEquipItems();
    tShowAdvancedTooltips.m_bChecked = tGameplayPrefs.bShowAdvancedTooltips;
    tSmartMinimap.m_bChecked = tGameplayPrefs.GetEnableSmartCoopMinimap();
    tAlwaysShowCampaignObjectives.m_bChecked = tGameplayPrefs.GetAlwaysShowCampaignObjectives();
    tAlwaysShowAdventureObjectives.m_bChecked = tGameplayPrefs.GetAlwaysShowAdventureObjectives();
    tItemRarityIcons.m_bChecked = tGameplayPrefs.GetItemRarityIcons();


    if (GameIsStartUp())
    {
        m_eTextLocaleOverride = tGameplayPrefs.GetSelectedTextLocale();
        m_tGameplay.push_back(tTextLocaleOverride);

        m_eAudioLocaleOverride = tGameplayPrefs.GetSelectedAudioLocale();
        m_tGameplay.push_back(tAudioLocaleOverride);
    }


    m_tGameplay.push_back(tElectiveMode);
    m_tGameplay.push_back(tDisplayPlayerHealthBars);
    m_tGameplay.push_back(tDisplayMonsterHealthBars);
    m_tGameplay.push_back(tDisplayPlayerNames);
    m_tGameplay.push_back(tDisplayHealingNumbers);
    m_tGameplay.push_back(tDisplayDamageNumbers);
    m_tGameplay.push_back(tDisplayCriticalDamageNumbers);
    m_tGameplay.push_back(tDisplayShortFloatingNumbers);
    m_tGameplay.push_back(tDisplayDefensiveMessages);
    m_tGameplay.push_back(tAlwaysShowItemLabelsOnDrop);
    m_tGameplay.push_back(tShowTutorials);
    m_tGameplay.push_back(tAutoEquipItems);
    m_tGameplay.push_back(tShowAdvancedTooltips);
    m_tGameplay.push_back(tSmartMinimap);
    m_tGameplay.push_back(tAlwaysShowCampaignObjectives);
    m_tGameplay.push_back(tAlwaysShowAdventureObjectives);
    m_tGameplay.push_back(tItemRarityIcons);



    Selection tGammaSetting, tSafeZone, tSubtitlesGame, tSubtitlesCinematics;

    tGammaSetting.Initialize(TEXT, "GameOptions:GammaTitle", "GameOptions:GammaSummary", GAMMA_SETTING);
    tSafeZone.Initialize(TEXT, "ConsoleOptions:SafeZone", "ConsoleOptions:SafeZoneTooltip", SAFE_ZONE);
    tSubtitlesGame.Initialize(CHECKBOX, "GameOptions:QuestDialogSubtitlesToggle", "GameOptions:QuestDialogSubtitlesTooltip", QUEST_DIALOG_SUBTITLES);
    tSubtitlesCinematics.Initialize(CHECKBOX, "GameOptions:CinematicSubtitlesToggle", "GameOptions:CinematicSubtitlesTooltip", CINEMATIC_SUBTITLES);

    const SoundPrefs & tSoundPrefs = Prefs::GetSoundPrefs();
    tSubtitlesGame.m_bChecked = tSoundPrefs.bQuestSubtitlesEnabled;
    tSubtitlesCinematics.m_bChecked = tSoundPrefs.bCinematicsSubtitlesEnabled;

    m_tVideo.push_back(tGammaSetting);
    m_tVideo.push_back(tSafeZone);
    m_tVideo.push_back(tSubtitlesGame);
    m_tVideo.push_back(tSubtitlesCinematics);



    Selection tMusicVolume, tEffectsVolume, tDialogueVolume, tAmbientVolume, tMasterVolume;

    tMasterVolume.Initialize(SELECTOR, "GameOptions:MasterVolumeTitle", "GameOptions:MasterVolumeTooltip", MASTER_VOLUME);
    tMusicVolume.Initialize(SELECTOR, "GameOptions:MusicVolumeTitle", "GameOptions:MusicVolumeTooltip", MUSIC_VOLUME);
    tEffectsVolume.Initialize(SELECTOR, "GameOptions:EffectsVolumeTitle", "GameOptions:EffectsVolumeTooltip", EFFECTS_VOLUME);
    tDialogueVolume.Initialize(SELECTOR, "GameOptions:VoiceVolumeTitle", "GameOptions:VoiceVolumeTooltip", DIALOGUE_VOLUME);
    tAmbientVolume.Initialize(SELECTOR, "GameOptions:AmbientVolumeTitle", "GameOptions:AmbientVolumeTooltip", AMBIENT_VOLUME);

    m_tAudio.push_back(tMasterVolume);
    m_tAudio.push_back(tMusicVolume);
    m_tAudio.push_back(tEffectsVolume);
    m_tAudio.push_back(tDialogueVolume);
    m_tAudio.push_back(tAmbientVolume);



    Selection tRegionOverride, tRegionPreference, tLanguagePreference;

    tRegionOverride.Initialize(SELECTOR, "ConsoleOptions:RegionOverride", "ConsoleOptions:RegionOverrideTooltip", REGION_OVERRIDE);
    tRegionPreference.Initialize(CHECKBOX, "ConsoleOptions:RegionPreference", "ConsoleOptions:RegionPreferenceTooltip", REGION_PREFERENCE);
    tLanguagePreference.Initialize(CHECKBOX, "ConsoleOptions:LanguagePreference", "ConsoleOptions:LanguagePreferenceTooltip", LANGUAGE_PREFERENCE);


    ConsoleMultiplayerPrefs & tMultiplayerPrefs = Prefs::GetConsoleMultiplayerPrefs();

    tRegionOverride.m_bWrap = TRUE;
    tRegionPreference.m_bChecked = tMultiplayerPrefs.GetMatchmakeRegionStrict();
    tLanguagePreference.m_bChecked = tMultiplayerPrefs.GetMatchmakeLanguageStrict();

    m_tMultiplayer.push_back(tRegionOverride);
    m_tMultiplayer.push_back(tRegionPreference);
    m_tMultiplayer.push_back(tLanguagePreference);

    m_ptActiveList = &m_tSubMenuSelector;
    m_eCurrentMenu = SUB_MENU_SELECTOR;
}



inline ConsoleOptionsSelectorDataSource::ConsoleOptionsSelectorDataSource()
{


    m_eCurrentOptionID = NONE;
    m_nCurrentListItemIndex = 0;
}
int32 ConsoleOptionsSelectorDataSource::GetNumItems() const
{

    switch (m_eCurrentOptionID)
    {
    case MUSIC_VOLUME: case MASTER_VOLUME: case EFFECTS_VOLUME: case DIALOGUE_VOLUME: case AMBIENT_VOLUME:
    case AUDIO_LANGUAGE_OVERRIDE:
        return 11;

    case TEXT_LANGUAGE_OVERRIDE:
        return 10;

    case REGION_OVERRIDE:
        return 8;

    default:
        break;
    }

    return 0;



}

UIControlSourceKey ConsoleOptionsSelectorDataSource::GetSourceKeyForItem(const int32 nIndex) const
{

    CRefString sSourceKey;
    sSourceKey.Format("%s.SelectorItem.SelectorTemplate", GetListDataSource().m_sListSourceKey.str());
    LPCSTR szSourceKey = sSourceKey.str();
    return UIControlSourceKey::Parse(szSourceKey, -1, NULL, NULL);
}




void ConsoleOptionsSelectorDataSource::InitializeItemUI(const UIConsoleSelector & tSelector, const UIID & idItemUI, const int32 nIndex)
{
}

void SetTextForRegion(CRefString & sText, int32 nRegion);
void SetTextForLocale(CRefString & sText, int32 eLocale);




void ConsoleOptionsSelectorDataSource::UpdateItemUI(const UIConsoleSelector & tSelector, const UIID & idItemUI, const int32 nIndex)



{



    UIID idText;
    idText.SetFromParent(idItemUI, "SelectorTemplateText");

    const SoundPrefs & tSoundPrefs = Prefs::GetSoundPrefs();


    CRefString sDisplayText;
    switch (m_eCurrentOptionID)
    {
    case MASTER_VOLUME:
        sDisplayText.Format("%i%%", (int32)(tSoundPrefs.flMasterVolume * 100.0f));
        break;
    case MUSIC_VOLUME:
        sDisplayText.Format("%i%%", (int32)(tSoundPrefs.flTypeVolume[SOUNDTYPE_MUSIC] * 100.0f));
        break;
    case EFFECTS_VOLUME:
        sDisplayText.Format("%i%%", (int32)(tSoundPrefs.flTypeVolume[SOUNDTYPE_EFFECT] * 100.0f));
        break;
    case DIALOGUE_VOLUME:
        sDisplayText.Format("%i%%", (int32)(tSoundPrefs.flTypeVolume[SOUNDTYPE_NARRATION] * 100.0f));
        break;
    case AMBIENT_VOLUME:
        sDisplayText.Format("%i%%", (int32)(tSoundPrefs.flTypeVolume[SOUNDTYPE_AMBIENT] * 100.0f));
        break;
    case REGION_OVERRIDE:
        SetTextForRegion(sDisplayText, nIndex);
        break;

    case TEXT_LANGUAGE_OVERRIDE:
        SetTextForLocale(sDisplayText, g_tNX64TextLocales[nIndex]);
        break;
    case AUDIO_LANGUAGE_OVERRIDE:
        SetTextForLocale(sDisplayText, g_tNX64AudioLocales[nIndex]);
        break;
    default:
        break;
    }



    UIText & tText = UI_GetTextFromID(idText);
    tText.SetText(sDisplayText, FALSE);


    if (m_nCurrentListItemIndex ==
        UI_GetConsoleListBoxFromID(GetListDataSource().m_idList).GetSelectedIndex())
    {
        tText.SetTextColor(crgbaWhite);
    }
    else
    {
        tText.SetTextColor(crgbaGold);
    }
}

void ConsoleOptionsSelectorDataSource::SetItemIndex(UIConsoleSelector & tSelector)

{

    const SoundPrefs & tSoundPrefs = Prefs::GetSoundPrefs();
    int32 nNewIndex = 0;

    switch (m_eCurrentOptionID)
    {
    case MASTER_VOLUME:
        nNewIndex = (int32)(tSoundPrefs.flMasterVolume * 10.0f);
        break;
    case MUSIC_VOLUME:
        nNewIndex = (int32)(tSoundPrefs.flTypeVolume[SOUNDTYPE_MUSIC] * 10.0f);
        break;
    case EFFECTS_VOLUME:
        nNewIndex = (int32)(tSoundPrefs.flTypeVolume[SOUNDTYPE_EFFECT] * 10.0f);
        break;
    case DIALOGUE_VOLUME:
        nNewIndex = (int32)(tSoundPrefs.flTypeVolume[SOUNDTYPE_NARRATION] * 10.0f);
        break;
    case AMBIENT_VOLUME:
        nNewIndex = (int32)(tSoundPrefs.flTypeVolume[SOUNDTYPE_AMBIENT] * 10.0f);
        break;
    case REGION_OVERRIDE:
        nNewIndex = Prefs::GetConsoleMultiplayerPrefs().GetMatchmakeRegion();
        break;

    case TEXT_LANGUAGE_OVERRIDE:
        nNewIndex = sFindLocaleOverrideIndex(UnicodeText::GetStringDisplayLocale());
        break;
    case AUDIO_LANGUAGE_OVERRIDE:
        nNewIndex = sFindAudioLocaleOverrideIndex(Prefs::GetGameplayPrefs().GetSelectedAudioLocale());
        break;
    default:
        break;
    }


    tSelector.SetSelectedIndex(nNewIndex, FALSE);
}





void OnSelectorChanged(const UIID & id)
{   gptConsoleOptionsDataSourceGlobals->bChanged = TRUE;
    Selection & tSelection = (*gptConsoleOptionsDataSourceGlobals->listData.m_ptActiveList)[gptConsoleOptionsDataSourceGlobals->listData.m_selectorData.m_nCurrentListItemIndex];
    int32 nIndex = UI_GetConsoleSelectorFromID(id).GetSelectedIndex();
    SoundPrefs & tSoundPrefs = Prefs::GetMutableSoundPrefs();



    switch (tSelection.m_eOptionID)
    {
    case MASTER_VOLUME:
        tSoundPrefs.flMasterVolume = nIndex * 0.1f;
        SoundManager::ApplyPrefSettings();
        break;
    case MUSIC_VOLUME:
        tSoundPrefs.flTypeVolume[SOUNDTYPE_MUSIC] = nIndex * 0.1f;
        SoundManager::ApplyPrefSettings();
        break;
    case EFFECTS_VOLUME:
        tSoundPrefs.flTypeVolume[SOUNDTYPE_EFFECT] = nIndex * 0.1f;
        SoundManager::ApplyPrefSettings();
        break;
    case DIALOGUE_VOLUME:
        tSoundPrefs.flTypeVolume[SOUNDTYPE_NARRATION] = nIndex * 0.1f;
        SoundManager::ApplyPrefSettings();
        break;
    case AMBIENT_VOLUME:
        tSoundPrefs.flTypeVolume[SOUNDTYPE_AMBIENT] = nIndex * 0.1f;
        SoundManager::ApplyPrefSettings();
        break;
    case REGION_OVERRIDE:
        Prefs::GetConsoleMultiplayerPrefs().SetMatchmakeRegion(nIndex);
        break;

    case TEXT_LANGUAGE_OVERRIDE:
        m_eTextLocaleOverride = (XLocale)g_tNX64TextLocales[nIndex];
        break;
    case AUDIO_LANGUAGE_OVERRIDE:
        m_eAudioLocaleOverride = (XLocale)g_tNX64AudioLocales[nIndex];
        break;
    default:
        break;
    }

}

void OnListSelectionChanged(const UIID & id, const int32, const int32 nNewVal)



{
    if (id == GetUIIDNone() || nNewVal == -1)
        return;








    ConsoleOptionsListDataSource & tDataSource = GetListDataSource();

    Selection & tCurrentSelection = (*tDataSource.m_ptActiveList)[nNewVal];
    tDataSource.m_selectorData.m_nCurrentListItemIndex = nNewVal;
    tDataSource.m_selectorData.m_eCurrentOptionID = tCurrentSelection.m_eOptionID;


    CRefString sTooltip = StringListFetch(tCurrentSelection.m_sToolTip);
    if (sTooltip.Length())
    {
        CRefString sDescription = StringListFetch("ConsoleOptions:DescriptionFormat");
        sDescription = StringListEntryReplace(sDescription, "{s1}", tCurrentSelection.m_sDisplayText, TRUE);
        sDescription = StringListEntryReplace(sDescription, "{s2}", sTooltip, TRUE);
        UI_GetTextFromID(tDataSource.m_idTooltip).SetText(sDescription, FALSE);
    }
    else
    {
        UI_GetTextFromID(tDataSource.m_idTooltip).SetText("", FALSE);
    }

    ::Console::ControllerActionManager & tActionManager = ::Console::Screens::GetActionManager();
    switch (tCurrentSelection.m_eType)
    {
    case SELECTOR:
        tActionManager.ClearAction(::Console::CONTROLLER_INPUT_BUTTON_1);
        break;
    case CHECKBOX:
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_ACCEPT, StringListFetch("Console:Toggle"));
        break;
    default:
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_ACCEPT, StringListFetch("Console:Accept"));
        break;
    }

    switch (tCurrentSelection.m_eOptionID)
    {
    case TEXT_LANGUAGE_OVERRIDE: case AUDIO_LANGUAGE_OVERRIDE:
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_ACCEPT, StringListFetch("Console:Accept"));
        break;
    default: break;
    }
    ::Console::Screens::UpdateButtonLegendText();
}



void SetToSubmenuSelector()
{

    ConsoleOptionsListDataSource & tListData = GetListDataSource();
    if (tListData.m_eCurrentMenu != SUB_MENU_SELECTOR)
    {

        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_PANEL_GENERIC_CLOSE), cgtPSODefaults);
        int32 nPrevMenu = tListData.m_eCurrentMenu;
        tListData.m_ptActiveList = &tListData.m_tSubMenuSelector;
        tListData.m_eCurrentMenu = SUB_MENU_SELECTOR;

        UIConsoleListBox & tList = UI_GetConsoleListBoxFromID(tListData.m_idList);
        tList.ResetAllItems();
        if (nPrevMenu >= 1 && nPrevMenu < tList.GetNumItems())
        {
            tList.SetSelectedIndex(nPrevMenu);
        }
    }
}


void OnAccept(Selection & tSelection)
{

    switch (GetListDataSource().m_eCurrentMenu)
    {
    case SUB_MENU_SELECTOR:

        OnAcceptSubMenuSelector(tSelection);
        break;
    case GAMEPLAY:
        OnAcceptGameplay(tSelection);
        break;
    case VIDEO:
        OnAcceptVideo(tSelection);
        break;
    }
}









void OnAcceptSubMenuSelector(const Selection & tSelection)
{
    ConsoleOptionsListDataSource & tListData = GetListDataSource();

    switch (tSelection.m_eOptionID)
    {
    case OPTIONID_UNK_5: {
        CRefString sUri = StringListFetch("ConsoleUI:DoNotSellInformationLink");
        ::Console::WebBrowser::NavigateTo(sUri, UI_GetPrimaryUIController());
        tListData.m_eCurrentMenu = SUB_MENU_SELECTOR;
        break;
    }
    case GAMEPLAY_TAB:
        tListData.m_ptActiveList = &tListData.m_tGameplay;
        tListData.m_eCurrentMenu = GAMEPLAY;
        break;
    case VIDEO_TAB:
        tListData.m_ptActiveList = &tListData.m_tVideo;
        tListData.m_eCurrentMenu = VIDEO;
        break;
    case AUDIO_TAB:
        tListData.m_ptActiveList = &tListData.m_tAudio;
        tListData.m_eCurrentMenu = AUDIO;
        break;
    case MULTIPLAYER_TAB:
        tListData.m_ptActiveList = &tListData.m_tMultiplayer;
        tListData.m_eCurrentMenu = MULTIPLAYER;
        break;
    case CONTROLS:

        UIControllerLayout::Console::SetOnCloseFunction(OnReturnToOptions);
        UIControllerLayout::Console::OpenForController(UI_GetPrimaryUIController());
        tListData.m_eCurrentMenu = CONTROLLER;
        break;
    case BATTLENET_TAB:
        UIBattlenetLink::SetOnCloseFunction(OnReturnToOptions);
        UIBattlenetLink::OpenForController(UI_GetPrimaryUIController(), FALSE);
        tListData.m_eCurrentMenu = BATTLENET;
        break;
    }



    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_PANEL_GENERIC_OPEN), cgtPSODefaults);
    if (tSelection.m_eOptionID != CONTROLS && tSelection.m_eOptionID != BATTLENET_TAB && tSelection.m_eOptionID != OPTIONID_UNK_5)
    {
        UIConsoleListBox & tList = UI_GetConsoleListBoxFromID(tListData.m_idList);
        tListData.m_selectorData.m_nCurrentListItemIndex = 0;
        tList.ResetAllItems();
    }
}
void OnAcceptGameplay(const Selection & tSelection)
{

    switch (tSelection.m_eOptionID) {
    case TEXT_LANGUAGE_OVERRIDE: {
            GameplayPrefs & tPrefs = Prefs::GetMutableGameplayPrefs();
            tPrefs.SetSelectedTextLocale(m_eTextLocaleOverride);

            if (GameIsStartUp())
            {


                BOOL bLocaleChanged = m_eTextLocaleOverride != UnicodeText::GetCurrentLocale();
                SelectionArray & tVideo = GetListDataSource().m_tVideo;
                for (Selection & tSelection : tVideo)
                {
                    if (tSelection.m_eOptionID == QUEST_DIALOG_SUBTITLES || tSelection.m_eOptionID == CINEMATIC_SUBTITLES)
                    {
                        tSelection.m_bChecked = bLocaleChanged;
                    }
                }



                SaveData();
                int32 nController = UI_GetPrimaryUIController();

                ApplyTextLocale();

                UI_SetPrimaryUIController(nController);
                OnBack(GetUIIDNone(), nController);
            }
        } break;
    case AUDIO_LANGUAGE_OVERRIDE: {

            int32 nController = UI_GetPrimaryUIController();
            ApplyAudioLocale(nController);
        } break;
    }
}



inline void OnAcceptVideo(const Selection & tSelection)
{

    switch (tSelection.m_eOptionID)
    {
    case SAFE_ZONE:
        if (CGameGet().ptUIGlobals)
        {
            gptConsoleOptionsDataSourceGlobals->bChanged = TRUE;




            UISafeZoneSettings::SetOnCloseFunction(OnReturnToVideoOptions);
            UISafeZoneSettings::Open(UI_GetPrimaryUIController());
        }
        break;
    case GAMMA_SETTING:


        gptConsoleOptionsDataSourceGlobals->bChanged = TRUE;
        UIGammaAdjust::Console::SetOnCloseFunction(OnReturnToVideoOptions);
        UIGammaAdjust::Console::OpenForController(UI_GetPrimaryUIController(), FALSE);
        break;
    }
}
void OnReturnToVideoOptions(const UIID &)
{

    ConsoleOptionsListDataSource & tListData = GetListDataSource(); int32 nPrevMenu = tListData.m_eCurrentMenu;
    if (nPrevMenu != VIDEO)
    {


        tListData.m_ptActiveList = &tListData.m_tVideo;
        tListData.m_eCurrentMenu = VIDEO;

        UIConsoleListBox & tList = UI_GetConsoleListBoxFromID(tListData.m_idList);
        tList.ResetAllItems();
        if (nPrevMenu >= 1 && nPrevMenu < tList.GetNumItems())
        {
            tList.SetSelectedIndex(nPrevMenu);
        }
    }
    ConfigureLegendForOptions();
}

void OnReturnToOptions(const UIID &)
{

    SetToSubmenuSelector();
    ConfigureLegendForOptions();
}

void ConfigureLegendForOptions()
{

    ::Console::ControllerActionManager & tActionManager = ::Console::Screens::GetActionManager();
    tActionManager.ClearAllActions();
    tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_ACCEPT, StringListFetch("Console:Accept"));
    tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_BACK, StringListFetch("Console:Back"));





    ::Console::Screens::UpdateButtonLegendText();
}



XLocale GetDefaultTextLocale()
{

    XLocale eLocale = ::Console::Utilities::GetSystemLanguage();



    BOOL bFound = FALSE;
    for (int32 i = 0; i < (int32)g_tNX64TextLocales.size(); i++)
        bFound |= (eLocale == g_tNX64TextLocales[i]);
    return bFound ? eLocale : XLOCALE_ENGLISH_US;
}





XLocale GetDefaultAudioLocale()
{

    XLocale eLocale = ::Console::Utilities::GetSystemLanguage();



    BOOL bFound = FALSE;
    for (int32 i = 0; i < (int32)g_tNX64AudioLocales.size(); i++)
        bFound |= (eLocale == g_tNX64AudioLocales[i]);
    return bFound ? eLocale : XLOCALE_ENGLISH_US;
}



// DWARF LockR returns const void*, Delete takes void*: original1047-1050 directly passes each result.

void ApplyTextLocale()
{
    if (GameIsStartUp()) {
        const GameplayPrefs & tGameplayPrefs = Prefs::GetGameplayPrefs(); // line 1038
        m_eTextLocaleOverride = tGameplayPrefs.GetSelectedTextLocale();
        if (m_eTextLocaleOverride != UnicodeText::GetStringDisplayLocale()) {
            ShellShutdownUI();
            gptGroupDefStringList->DeleteAll();
            gptGroupDefFont->DeleteAll();
            gptGroupDefTexture->Delete(const_cast<void *>(gptGroupDefTexture->LockR(GlobalSNOGet(GLOBALSNO_EFFECTGROUP_PLAYER_1_NUM_TEXT), FALSE)));
            gptGroupDefTexture->Delete(const_cast<void *>(gptGroupDefTexture->LockR(GlobalSNOGet(GLOBALSNO_EFFECTGROUP_PLAYER_2_NUM_TEXT), FALSE)));
            gptGroupDefTexture->Delete(const_cast<void *>(gptGroupDefTexture->LockR(GlobalSNOGet(GLOBALSNO_EFFECTGROUP_PLAYER_3_NUM_TEXT), FALSE)));
            gptGroupDefTexture->Delete(const_cast<void *>(gptGroupDefTexture->LockR(GlobalSNOGet(GLOBALSNO_EFFECTGROUP_PLAYER_4_NUM_TEXT), FALSE)));
            UnicodeText::SetStringDisplayLocale(m_eTextLocaleOverride);
            SNOManager::ReloadRequired(FONT_SNO);
            Prefetch::BlockUntilPriorityIdle(2);
            ShellRestoreUI();
        }
    }
}







inline void ApplyAudioLocale(int32 nUserIndex)
{

    GameplayPrefs & tPrefs = Prefs::GetMutableGameplayPrefs();
    tPrefs.SetSelectedAudioLocale(m_eAudioLocaleOverride);
    SaveData();
    ShowSystemLocaleAOCOffer(nUserIndex, LocaleOfferCallback());
}




void OnCheckboxToggle(const UIID & id)
{
    gptConsoleOptionsDataSourceGlobals->bChanged = TRUE;
    UIConsoleListBox & tListBox = UI_GetConsoleListBoxFromID(GetListDataSource().m_idList); // line 1080
    int32 nIndex = tListBox.GetSelectedIndex(); // line 1081
    UICheckBox & tCheckbox = UI_GetCheckBoxFromID(id); // line 1083
    (*GetListDataSource().m_ptActiveList)[nIndex].m_bChecked = tCheckbox.IsChecked();
}











void SaveCheckboxDataFromList(SelectionArray & tList)
{

    for (uint32 i = 0; i < tList.size(); i++)
    {
        Selection & tSelection = tList[i];

        if (tSelection.m_eType == CHECKBOX)
        {

            switch (tSelection.m_eOptionID)
            {
            case ELECTIVE_MODE:
                Prefs::GetMutableGameplayPrefs().SetUseElectiveMode(tSelection.m_bChecked);
                break;
            case DISPLAY_PLAYER_HEALTH_BARS:
                Prefs::GetMutableGameplayPrefs().SetShowPlayerHPBars(tSelection.m_bChecked);
                break;
            case DISPLAY_MONSTER_HEALTH_BARS:
                Prefs::GetMutableGameplayPrefs().SetShowMonsterHPBars(tSelection.m_bChecked);
                break;
            case DISPLAY_PLAYER_NAMES:
                Prefs::GetMutableGameplayPrefs().SetShowPlayerNames(tSelection.m_bChecked);
                break;
            case DISPLAY_HEALING_NUMBERS:
                Prefs::GetMutableGameplayPrefs().SetShowHealNumbers(tSelection.m_bChecked);
                break;
            case DISPLAY_DAMAGE_NUMBERS:
                Prefs::GetMutableGameplayPrefs().SetShowDamageNumbers(tSelection.m_bChecked);
                break;
            case DISPLAY_CRITICAL_DAMAGE_NUMBERS:
                Prefs::GetMutableGameplayPrefs().SetShowCriticals(tSelection.m_bChecked);
                break;
            case DISPLAY_SHORT_FLOATING_NUMBERS:
                Prefs::GetMutableGameplayPrefs().SetDisableShortFloatingNumbers(tSelection.m_bChecked);
                break;
            case DISPLAY_DEFENSIVE_MESSAGES:
                Prefs::GetMutableGameplayPrefs().SetShowDefensiveMessages(tSelection.m_bChecked);
                break;
            case ALWAYS_SHOW_ITEM_LABELS_ON_DROP:
                Prefs::GetMutableGameplayPrefs().SetShowItemToolTipOnDrop(tSelection.m_bChecked);
                break;
            case SHOW_TUTORIALS:
                Prefs::GetMutableGameplayPrefs().SetShowTutorials(tSelection.m_bChecked);
                break;
            case AUTO_EQUIP_ITEMS:
                Prefs::GetMutableGameplayPrefs().SetAutoEquipItems(tSelection.m_bChecked);
                break;
            case SHOW_ADVANCED_TOOLTIPS:
                Prefs::GetMutableGameplayPrefs().SetShowAdvancedTooltips(tSelection.m_bChecked);
                break;
            case SMART_MINIMAP:
                Prefs::GetMutableGameplayPrefs().SetEnableSmartCoopMinimap(tSelection.m_bChecked);
                break;
            case ALWAYS_SHOW_CAMPAIGN_OBJECTIVES:
                Prefs::GetMutableGameplayPrefs().SetAlwaysShowCampaignObjectives(tSelection.m_bChecked);
                break;
            case ALWAYS_SHOW_ADVENTURE_OBJECTIVES:
                Prefs::GetMutableGameplayPrefs().SetAlwaysShowAdventureObjectives(tSelection.m_bChecked);
                break;
            case ITEM_RARITY_ICONS:
                Prefs::GetMutableGameplayPrefs().SetItemRarityIcons(tSelection.m_bChecked);
                break;
            case QUEST_DIALOG_SUBTITLES:
                Prefs::GetMutableSoundPrefs().bQuestSubtitlesEnabled = tSelection.m_bChecked;
                break;
            case CINEMATIC_SUBTITLES:
                Prefs::GetMutableSoundPrefs().bCinematicsSubtitlesEnabled = tSelection.m_bChecked;
                break;
            case REGION_PREFERENCE:
                Prefs::GetConsoleMultiplayerPrefs().SetMatchmakeRegionStrict(tSelection.m_bChecked);
                break;
            case LANGUAGE_PREFERENCE:
                Prefs::GetConsoleMultiplayerPrefs().SetMatchmakeLanguageStrict(tSelection.m_bChecked);
                break;
            }
        }
    }
}


inline void SaveCheckboxData()
{

    ConsoleOptionsListDataSource & tListData = GetListDataSource();
    SaveCheckboxDataFromList(tListData.m_tGameplay);
    SaveCheckboxDataFromList(tListData.m_tVideo);
    SaveCheckboxDataFromList(tListData.m_tAudio);
    SaveCheckboxDataFromList(tListData.m_tMultiplayer);
}


void SetTextForRegion(CRefString & sText, int32 nIndex)
{


    switch (nIndex)
    {
    case ::Console::Online::REGION_DEFAULT:
        sText = StringListFetch("ConsoleOptions:RegionNoOverride");
        break;
    case ::Console::Online::REGION_NORTH_AMERICA:
        sText = StringListFetch("ConsoleOptions:RegionNorthAmerica");
        break;
    case ::Console::Online::REGION_SOUTH_AMERICA:
        sText = StringListFetch("ConsoleOptions:RegionSouthAmerica");
        break;
    case ::Console::Online::REGION_EUROPE:
        sText = StringListFetch("ConsoleOptions:RegionEurope");
        break;
    case ::Console::Online::REGION_RUSSIA:
        sText = StringListFetch("ConsoleOptions:RegionRussia");
        break;
    case ::Console::Online::REGION_ASIA:
        sText = StringListFetch("ConsoleOptions:RegionAsia");
        break;
    case ::Console::Online::REGION_AUSTRALIA:
        sText = StringListFetch("ConsoleOptions:RegionAustralia");
        break;
    case ::Console::Online::REGION_AFRICA:
        sText = StringListFetch("ConsoleOptions:RegionAfrica");
        break;
    }
}




void SetTextForLocale(CRefString & sText, XLocale eLocale)
{
    switch (eLocale)
    {
    case XLOCALE_GERMAN:
        sText = StringListFetch("GameOptions:Lang_deDE");
        break;
    case XLOCALE_ENGLISH_US:
        sText = StringListFetch("GameOptions:Lang_enUS");
        break;
    case XLOCALE_ENGLISH_EU:
        sText = StringListFetch("GameOptions:Lang_enGB");
        break;
    case XLOCALE_SPANISH_EU:
        sText = StringListFetch("GameOptions:Lang_esES");
        break;
    case XLOCALE_SPANISH_LATAM:
        sText = StringListFetch("GameOptions:Lang_esMX");
        break;
    case XLOCALE_FRENCH:
        sText = StringListFetch("GameOptions:Lang_frFR");
        break;
    case XLOCALE_ITALIAN:
        sText = StringListFetch("GameOptions:Lang_itIT");
        break;
    case XLOCALE_POLISH:
        sText = StringListFetch("GameOptions:Lang_plPL");
        break;
    case XLOCALE_PORTUGUESE_BRAZIL:
        sText = StringListFetch("GameOptions:Lang_ptBR");
        break;
    case XLOCALE_RUSSIAN:
        sText = StringListFetch("GameOptions:Lang_ruRU");
        break;
    case XLOCALE_JAPANESE:
        sText = StringListFetch("GameOptions:Lang_jpJP");
        break;
    default:
        sText = XLocaleToString(eLocale);
        break;
    }
}



void InitializeForGame()
{

    gptConsoleOptionsDataSourceGlobals = new ConsoleOptionsDataSourceGlobals;
}

void DisposeForGame()
{

    delete gptConsoleOptionsDataSourceGlobals; gptConsoleOptionsDataSourceGlobals = NULL;
}

ConsoleOptionsListDataSource & GetListDataSource()
{


    return gptConsoleOptionsDataSourceGlobals->listData;
}
void SaveData()
{

    if (gptConsoleOptionsDataSourceGlobals->bChanged)
    {

        gptConsoleOptionsDataSourceGlobals->bChanged = FALSE;
        SaveCheckboxData();


        ::Console::GamerProfile::SavePrefs();
        for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
        {

            if (PlayerIsLocal(ptPlayer))
            {

                CPlayerUpdateServerSidePrefs(ptPlayer);
                CPlayerSendPrefsToServer(ptPlayer);
            }
        }
    }
}

}  // namespace Console
}  // namespace UIOptions
