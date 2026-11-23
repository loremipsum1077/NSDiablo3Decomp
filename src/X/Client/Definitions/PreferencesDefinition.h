// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/Definitions/PreferencesDefinition.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "SoundBankDefinition.h"
#include "SoundDefinition.h"
#include "../Gfx/GfxCommon.h"
#include "../Input/Input.h"
#include "../../Common/Game.h"
#include "../../Common/OnlineService/Callback/Callback_Impl.h"
#include "../../Sigma/DataStructures/XGrowableMap.h"
#include "../../Sigma/SigmaTypes.h"

enum {
    ACTION_INVALID = -1,
    ACTION_FIRST_GAME_ACTION = 0,
    ACTION_UI_INVENTORY = 0,
    ACTION_UI_HIRELING = 1,
    ACTION_UI_SKILLS = 2,
    ACTION_UI_QUESTS = 3,
    ACTION_UI_LORE = 4,
    ACTION_UI_LEADERBOARDS = 5,
    ACTION_UI_SOCIAL = 6,
    ACTION_UI_GROUPS = 7,
    ACTION_UI_CLAN = 8,
    ACTION_UI_STORE = 9,
    ACTION_UI_WORLDMAP = 10,
    ACTION_UI_HELP = 11,
    ACTION_UI_RECENTPLAYERS = 12,
    ACTION_UI_CHATLOBBY = 13,
    ACTION_UI_ACHIEVEMENTS = 14,
    ACTION_UI_PROFILE = 15,
    ACTION_UI_AUCTIONHOUSE = 16,
    ACTION_UI_BANNERSCREEN = 17,
    ACTION_UI_CLEAR = 18,
    ACTION_UI_CONSOLE = 19,
    ACTION_UI_MAP = 20,
    ACTION_UI_SKILL_CYCLE = 21,
    ACTION_UI_TOGGLE_MONSTER_HP_BARS = 22,
    ACTION_UI_TOGGLE_PLAYER_HP_BARS = 23,
    ACTION_UI_TOGGLE_DAMAGE_NUMBERS = 24,
    ACTION_UI_TOGGLE_ITEMS_ON_GROUND = 25,
    ACTION_UI_REWHISPER = 26,
    ACTION_UI_WHISPER_REPLY = 27,
    ACTION_UI_VOICE_PTT = 28,
    ACTION_UI_CHAT_PAGE_UP = 29,
    ACTION_UI_CHAT_PAGE_DOWN = 30,
    ACTION_UI_CHAT_NEXT_VIEW = 31,
    ACTION_UI_CHAT_PREV_VIEW = 32,
    ACTION_UI_CHAT_RESET_FADE = 33,
    ACTION_UI_SKILL_1 = 34,
    ACTION_UI_SKILL_2 = 35,
    ACTION_UI_SKILL_3 = 36,
    ACTION_UI_SKILL_4 = 37,
    ACTION_UI_POTION = 38,
    ACTION_UI_BANNER_DROP = 39,
    ACTION_UI_ZOOM = 40,
    ACTION_UI_EMOTE_FOLLOW = 41,
    ACTION_UI_EMOTE_GIVE = 42,
    ACTION_UI_EMOTE_THANKS = 43,
    ACTION_UI_EMOTE_SORRY = 44,
    ACTION_UI_EMOTE_BYE = 45,
    ACTION_UI_EMOTE_DIE = 46,
    ACTION_UI_EMOTE_RUN = 47,
    ACTION_UI_EMOTE_WAIT = 48,
    ACTION_UI_EMOTE_GO = 49,
    ACTION_UI_EMOTE_HELP = 50,
    ACTION_UI_EMOTE_YES = 51,
    ACTION_UI_EMOTE_NO = 52,
    ACTION_UI_EMOTE_STAY = 53,
    ACTION_UI_EMOTE_ATTACK = 54,
    ACTION_UI_EMOTE_RETREAT = 55,
    ACTION_UI_EMOTE_HOLD = 56,
    ACTION_UI_EMOTE_TAKEOBJECTIVE = 57,
    ACTION_UI_EMOTE_LAUGH = 58,
    ACTION_UI_HOLD_POSITION = 59,
    ACTION_SYSTEM_MASTER_VOLUME_UP = 60,
    ACTION_SYSTEM_MASTER_VOLUME_DOWN = 61,
    ACTION_SYSTEM_TOGGLE_SOUND = 62,
    ACTION_SYSTEM_TOGGLE_MUSIC = 63,
    ACTION_SYSTEM_TOGGLE_FPS = 64,
    ACTION_SYSTEM_SCREENSHOT = 65,
    ACTION_UI_STONE_OF_RECALL = 66,
    ACTION_UI_WALK = 67,
    ACTION_UI_PARAGON = 68,
    ACTION_UI_SEASONJOURNEY = 69,
    ACTION_UI_SHOW_HIDDEN_BUFFS = 70,
    ACTION_HIDE_UI = 71,
    NUM_BINDABLE_ACTIONS = 72,
    ACTION_UI_DELETE = 72,
    ACTION_UI_ARROW_UP = 73,
    ACTION_UI_ARROW_DOWN = 74,
    ACTION_UI_SIMPLECHAT = 75,
    ACTION_UI_CYCLECHAT = 76,
    ACTION_UI_PASTE_ITEM = 77,
    ACTION_UI_SECONDARY_TIPS = 78,
    ACTION_UI_QUICK_INVENTORY_KEY = 79,
    ACTION_UI_OK = 80,
    ACTION_UI_ESCAPE = 81,
    ACTION_UI_OPTIONS = 82,
    ACTION_UI_LEFT_MOUSE_BUTTON = 83,
    ACTION_UI_RIGHT_MOUSE_BUTTON = 84,
    ACTION_UI_SCREEN_LEFT = 85,
    ACTION_UI_SCREEN_RIGHT = 86,
    ACTION_UI_DESIGNERDIALOG = 87,
    ACTION_CAMERA_ANGLE_UP = 88,
    ACTION_CAMERA_ANGLE_DOWN = 89,
    ACTION_CAMERA_RADIUS_IN = 90,
    ACTION_CAMERA_RADIUS_OUT = 91,
    ACTION_CAMERA_FOCAL_POINT_UP = 92,
    ACTION_CAMERA_FOCAL_POINT_DOWN = 93,
    ACTION_CAMERA_ANGLE_LEFT = 94,
    ACTION_CAMERA_ANGLE_RIGHT = 95,
    ACTION_CONSOLE_EVASION = 96,
    ACTION_SKIP_CUTSCENE = 97,
    ACTION_UI_LAST = 97,
    NUM_ACTIONS = 98,
};

enum {
    SHADOWPREF_NONE = 0,
    SHADOWPREF_LOW = 1,
    SHADOWPREF_MID = 2,
    SHADOWPREF_HIGH = 3,
    SHADOWPREF_LOW_SOFT = 4,
    SHADOWPREF_MID_SOFT = 5,
    SHADOWPREF_HIGH_SOFT = 6,
    SHADOWPREF_LAST = 7,
};

enum {
    SHADOWDETAILPERF_LOW = 0,
    SHADOWDETAILPERF_HIGH = 1,
    SHADOWDETAILPERF_LAST = 2,
};

enum {
    CLUTTER_NONE = 0,
    CLUTTER_LOW = 1,
    CLUTTER_MID = 2,
    CLUTTER_HIGH = 3,
    CLUTTER_LAST = 4,
};

enum {
    INPUT_MODE_NORMAL = 0,
    INPUT_MODE_ACTOR_TARGET_LOCK = 1,
    INPUT_MODE_MOVE_LOCK = 2,
    INPUT_MODE_CANCELED = 3,
};

enum {
    ITEMS_ON_GROUND_SHOW_WHILE_HELD = 0,
    ITEMS_ON_GROUND_TOGGLE = 2,
    ITEMS_ON_GROUND_SHOW_FOR_DURATION = 3,
    ITEMS_ON_GROUND_NUM_SETTINGS = 4,
};

typedef XGrowableMap<CRefString, const CRefString &, CRefString, const CRefString &> KeyValueMap;

typedef int32 Key;

struct ActionBinding {  // size 0x10
    Key eKey1;  // 0x00
    uint32 dwKeyModifierFlags1;  // 0x04
    Key eKey2;  // 0x08
    uint32 dwKeyModifierFlags2;  // 0x0c
};
static_assert(sizeof(ActionBinding) == 0x10, "ActionBinding");

struct VideoPrefsToConfirm {  // size 0x48
    BOOL bVsync;  // 0x00
    int32 nMipOffset;  // 0x04
    int32 nShadowQuality;  // 0x08
    DisplayMode tDisplayMode;  // 0x0c
    BOOL bLargeCursor;  // 0x44
};
static_assert(sizeof(VideoPrefsToConfirm) == 0x48, "VideoPrefsToConfirm");

struct VideoPrefs {  // size 0xb0
    void ResetToDefaults();
    HardwareClass GetHWClass() const;
    float GetGamma() const;
    void SetGamma(float flGammaValue);
    float GetGammaMin() const;
    float GetGammaMax() const;
    float GetGammaDefault() const;
    void GetSafeZoneDefualtValues(float & _flSafeZoneLeft, float & _flSafeZoneTop, float & _flSafeZoneRight, float & _flSafeZoneBottom) const;
    BOOL GetUseLetterBox() const;
    BOOL GetLimitForegroundFPS() const;
    int32 GetForegroundFPSLimit() const;
    BOOL GetUseColorCorrection() const;
    BOOL GetLimitBackgroundFPS() const;
    int32 GetBackgroundFPSLimit() const;
    BOOL GetDisableTrilinearFiltering() const;
    BOOL GetUseLowFXMode() const;
    void SetUseLowFXMode(BOOL bUseLowFX);
    BOOL GetUseSSAO() const;
    void SetUseSSAO(BOOL);
    BOOL GetLockCursorInFullscreenWindowed() const;
    BOOL GetLargeCursor() const;
    void SetLargeCursor(BOOL);
    BOOL GetUseAntialiasing() const;
    void SetUseAntialiasing(BOOL bUse);
    BOOL GetDisableScreenShake() const;
    void SetDisableScreenShake(BOOL);
    BOOL GetDisableChromaEffects() const;
    void SetDisableChromaEffects(BOOL);
    int32 GetClutterQuality() const;
    void SetClutterQuality(int32);
    uint32 GetWindowMode() const;
    void SetWindowMode(uint32);
    int32 GetShadowQuality() const;
    void SetShadowQuality(int32 nQuality);
    int32 GetShadowMapSize() const;
    void SetShadowDetail(BOOL);
    int32 GetShadowDetail() const;
    void SetTextureMipOffset(int32);
    int32 GetTextureMipOffset() const;
    void SetTextureMipBias(float);
    float GetTextureMipBias() const;
    int32 GetReflectionQuality() const;
    void SetReflectionQuality(int32 nQuality);
    int32 GetPhysicsQuality() const;
    void SetPhysicsQuality(int32);
    VideoPrefsToConfirm tVideoPrefsToConfirm;  // 0x00
    float flGamma;  // 0x48
    int32 nClutterQuality;  // 0x4c
    int32 nPhysicsQuality;  // 0x50
    int32 nShadowDetail;  // 0x54
    BOOL bLetterbox;  // 0x58
    BOOL bAntialiasing;  // 0x5c
    BOOL bLowFX;  // 0x60
    BOOL bSSAO;  // 0x64
    BOOL bLockCursorInFullscreenWindowed;  // 0x68
    BOOL bDisableScreenShake;  // 0x6c
    BOOL bDisableChromaEffects;  // 0x70
    BOOL bLimitForegroundFPS;  // 0x74
    int32 nMaxForegroundFPS;  // 0x78
    BOOL bLimitBackgroundFPS;  // 0x7c
    int32 nMaxBackgroundFPS;  // 0x80
    int32 nReflectionQuality;  // 0x84
    BOOL bDisableTrilinearFiltering;  // 0x88
    BOOL bColorCorrection;  // 0x8c
    float flMipBias;  // 0x90
    HardwareClass eHWClass;  // 0x94
    uint32 dwPCIVendor;  // 0x98
    uint32 dwPCIDevice;  // 0x9c
    float flSafeZoneLeft;  // 0xa0
    float flSafeZoneTop;  // 0xa4
    float flSafeZoneRight;  // 0xa8
    float flSafeZoneBottom;  // 0xac
};
static_assert(sizeof(VideoPrefs) == 0xb0, "VideoPrefs");

struct SoundPrefs {  // size 0x4c
    void ResetToDefaults();
    BOOL QuestSubtitlesEnabled() const;
    BOOL CinematicsSubtitlesEnabled() const;
    BOOL PlayInBackground() const;
    BOOL GetMuteSound() const;
    void SetMuteSound(BOOL);
    BOOL GetMuteSoundType(SoundType) const;
    void SetMuteSoundType(SoundType, BOOL);
    float GetMasterVolume() const;
    void SetMasterVolume(float);
    int32 GetChannelsToUse() const;
    uint32 GetDriverHash() const;
    SpeakerModes GetSpeakerMode() const;
    float flMasterVolume;  // 0x00
    float flTypeVolume[5];  // 0x04
    int32 nChannelsToUse;  // 0x18
    uint32 dwDriverHash;  // 0x1c
    SpeakerModes eSpeakerMode;  // 0x20
    BOOL bReverseSpeakers;  // 0x24
    BOOL bQuestSubtitlesEnabled;  // 0x28
    BOOL bCinematicsSubtitlesEnabled;  // 0x2c
    BOOL bMuteSound;  // 0x30
    BOOL bMuteSoundType[5];  // 0x34
    BOOL bPlayInBackground;  // 0x48
};
static_assert(sizeof(SoundPrefs) == 0x4c, "SoundPrefs");

class GameplayPrefs {  // size 0x70
public:
    void ResetToDefaults();
    BOOL GetShowBreadCrumb() const { return bShowBreadCrumb; }  // line 506
    void SetShowBreadCrumbs(BOOL);
    BOOL GetShowClock() const;
    void SetShowClock(BOOL);
    BOOL GetShowTutorials() const { return bShowTutorials; }  // line 512
    void SetShowTutorials(BOOL bShow);
    BOOL GetShowHPBarNumbers() const { return bShowHPBarNumbers; }  // line 515
    void SetShowHPBarNumbers(BOOL);
    BOOL GetShowMonsterHPBars() const { return bShowMonsterHPBars; }  // line 518
    void SetShowMonsterHPBars(BOOL bShow);
    BOOL GetShowPlayerHPBars() const { return bShowPlayerHPBars; }  // line 521
    void SetShowPlayerHPBars(BOOL bShow);
    BOOL GetShowDefensiveMessages() const { return bShowDefensiveMessages; }  // line 524
    void SetShowDefensiveMessages(BOOL bShow);
    BOOL GetShowCriticals() const { return bShowCriticals; }  // line 527
    void SetShowCriticals(BOOL bShow);
    BOOL GetShowDamageNumbers() const { return bShowDamageNumbers; }  // line 530
    void SetShowDamageNumbers(BOOL bShow);
    BOOL GetShowHealNumbers() const { return bShowHealNumbers; }  // line 533
    void SetShowHealNumbers(BOOL bShow);
    BOOL GetShowPlayerNames() const { return bShowPlayerNames; }  // line 536
    void SetShowPlayerNames(BOOL bShow);
    BOOL GetShowItemsOnGround() const { return bShowItemsOnGround; }  // line 539
    void SetShowItemsOnGround(BOOL bShow);
    BOOL GetSkillElectiveMode() const { return bElectiveMode; }  // line 542
    void SetUseElectiveMode(BOOL bShow);
    BOOL GetInventoryZoom() const;
    void SetUseInventoryZoom(BOOL);
    BOOL GetAutoEquipItems() const { return bAutoEquipItems; }  // line 548
    void SetAutoEquipItems(BOOL bShow);
    BOOL GetItemTagsAsIcons() const { return bItemTagsAsIcons; }  // line 551
    void SetItemTagsAsIcons(BOOL);
    BOOL GetShowItemDroppedTooltips() const { return bShowItemTooltipOnDrop; }  // line 554
    void SetShowItemToolTipOnDrop(BOOL bShow);
    BOOL ShowAdvancedTooltips() const;
    void SetShowAdvancedTooltips(BOOL bShow);
    BOOL GetDisableCutscenes() const;
    // line 562; inline only in the original. The body is the copy inlined into
    // ClientMessageReceiveGameMessage (SMSG_PLAY_ANNIVERSARY_CUTSCENE).
    BOOL GetDisableCutscenesRaw() const { return bDisableCutscenes; }
    void SetDisableCutscenes(BOOL);
    int32 GetItemsOnGroundSetting() const { return nItemsOnGroundSetting; }  // line 566
    void SetItemsOnGroundSetting(int32);
    BOOL GetItemRarityIcons() const;
    void SetItemRarityIcons(BOOL bVal);
    BOOL GetDisableShortFloatingNumbers() const;
    void SetDisableShortFloatingNumbers(BOOL bVal);
    BOOL GetEquipmentManagerDropToStash() const;
    void SetEquipmentManagerDropToStash(BOOL);
    BOOL GetEnableSmartCoopMinimap() const;
    void SetEnableSmartCoopMinimap(BOOL bEnable);
    BOOL GetAlwaysShowCampaignObjectives() const;
    void SetAlwaysShowCampaignObjectives(BOOL bEnable);
    BOOL GetAlwaysShowAdventureObjectives() const;
    void SetAlwaysShowAdventureObjectives(BOOL bEnable);
    XLocale GetSelectedTextLocale() const;
    void SetSelectedTextLocale(XLocale nLocale);
    XLocale GetSelectedAudioLocale() const;
    void SetSelectedAudioLocale(XLocale nLocale);
    int32 nItemsOnGroundSetting;  // 0x00
    BOOL bItemTagsAsIcons;  // 0x04
    BOOL bShowItemTooltipOnDrop;  // 0x08
    BOOL bShowMonsterHPBars;  // 0x0c
    BOOL bShowPlayerHPBars;  // 0x10
    BOOL bShowItemsOnGround;  // 0x14
    BOOL bShowPlayerNames;  // 0x18
    BOOL bShowDamageNumbers;  // 0x1c
    BOOL bShowHealNumbers;  // 0x20
    BOOL bShowHPBarNumbers;  // 0x24
    BOOL bShowCriticals;  // 0x28
    BOOL bShowDefensiveMessages;  // 0x2c
    BOOL bInventoryZoom;  // 0x30
    BOOL bAutoEquipItems;  // 0x34
    BOOL bShowTutorials;  // 0x38
    BOOL bShowBreadCrumb;  // 0x3c
    BOOL bShowClock;  // 0x40
    BOOL bShowAdvancedTooltips;  // 0x44
    BOOL bElectiveMode;  // 0x48
    BOOL bDisableCutscenes;  // 0x4c
    BOOL bItemRarityIcons;  // 0x50
    BOOL bDisableShortFloatingNumbers;  // 0x54
    BOOL bEquipmentManagerDropToStash;  // 0x58
    BOOL bEnableSmartCoopMinimap;  // 0x5c
    BOOL bAlwaysShowCampaignObjectives;  // 0x60
    BOOL bAlwaysShowAdventureObjectives;  // 0x64
    int32 nSelectedTextLocale;  // 0x68
    int32 nSelectedAudioLocale;  // 0x6c
};
static_assert(sizeof(GameplayPrefs) == 0x70, "GameplayPrefs");

// PreferencesDefinition.h:545. CPlayerUpdateServerSidePrefs (X/Client/CPlayer.cpp:5121) calls
// GetGameplayPrefs() and then clears flag 0x10 with nothing loaded, and the DWARF records no
// inlined copy, so this getter is a constant in the console build. Which getter it is is a
// guess: GetInventoryZoom and GetShowClock are the two that are inline and never inlined.
inline BOOL GameplayPrefs::GetInventoryZoom() const { return FALSE; }

class MacPrefs {  // size 0xc
public:
    void ResetToDefaults();
    BOOL GetDisableOSShortcuts() const;
    void SetDisableOSShortcuts(BOOL);
    BOOL GetUseCommandAsControl() const;
    void SetUseCommandAsControl(BOOL);
    BOOL GetUseCommandClickAsRightClick() const;
    void SetUseCommandClickAsRightClick(BOOL);
    BOOL bDisableOSShortcuts;  // 0x00
    BOOL bUseCommandAsControl;  // 0x04
    BOOL bUseCommandClickAsRightClick;  // 0x08
};
static_assert(sizeof(MacPrefs) == 0xc, "MacPrefs");

class ConsoleMultiplayerPrefs {  // size 0xc
public:
    void ResetToDefaults();
    int32 GetMatchmakeRegion();
    void SetMatchmakeRegion(const int32 nSetting);
    BOOL GetMatchmakeRegionStrict();
    void SetMatchmakeRegionStrict(BOOL bSetting);
    BOOL GetMatchmakeLanguageStrict();
    void SetMatchmakeLanguageStrict(BOOL bSetting);
    int32 nMatchmakeRegion;  // 0x00
    BOOL bMatchmakeRegionStrict;  // 0x04
    BOOL bMatchmakeLanguageStrict;  // 0x08
};
static_assert(sizeof(ConsoleMultiplayerPrefs) == 0xc, "ConsoleMultiplayerPrefs");

struct KeybindingPrefs {  // size 0x484
    void ResetToDefaults();
    int32 nKeybindingVersion;  // 0x00
    ActionBinding tBindings[72];  // 0x04
};
static_assert(sizeof(KeybindingPrefs) == 0x484, "KeybindingPrefs");

struct SocialPrefs {  // size 0x30
    void ResetToDefaults();
    BOOL GetNotifyFriendOnline() const;
    void SetNotifyFriendOnline(BOOL);
    BOOL GetNotifyFriendOffline() const;
    void SetNotifyFriendOffline(BOOL);
    BOOL GetNotifyFriendRequest() const;
    void SetNotifyFriendRequest(BOOL);
    BOOL GetNotifyFriendAchievement() const;
    void SetNotifyFriendAchievement(BOOL);
    BOOL GetNotifyDisplayWindow() const;
    void SetNotifyDisplayWindow(BOOL);
    int32 GetNotifyDuration() const;
    void SetNotifyDuration(int32);
    BOOL GetAllowQuickJoin() const;
    void SetAllowQuickJoin(BOOL);
    BOOL GetNotifyAuctionHouseEvents() const;
    void SetNotifyAuctionHouseEvents(BOOL);
    BOOL GetNotifyFriendBroadcastDisabled() const;
    void SetNotifyFriendBroadcastDisabled(BOOL);
    BOOL GetShowLocalPlayersDisabled() const;
    void SetShowLocalPlayersDisabled(BOOL);
    BOOL GetNotifyGuildMemberOnlineDisabled() const;
    void SetNotifyGuildMemberOnlineDisabled(BOOL);
    BOOL GetNotifyGuildAchievementDisabled() const;
    void SetNotifyGuildAchievementDisabled(BOOL);
    BOOL bNotifyFriendOnline;  // 0x00
    BOOL bNotifyFriendOffline;  // 0x04
    BOOL bNotifyFriendRequest;  // 0x08
    BOOL bNotifyFriendAchievement;  // 0x0c
    BOOL bNotifyDisplayWindow;  // 0x10
    int32 nNotifyDuration;  // 0x14
    BOOL bAllowQuickJoin;  // 0x18
    BOOL bNotifyAuctionHouseEvents;  // 0x1c
    BOOL bNotifyFriendBroadcastDisabled;  // 0x20
    BOOL bShowLocalPlayersListDisabled;  // 0x24
    BOOL bNotifyGuildMemberOnlineDisabled;  // 0x28
    BOOL bNotifyGuildAchievementDisabled;  // 0x2c
};
static_assert(sizeof(SocialPrefs) == 0x30, "SocialPrefs");

struct ChatPrefs {  // size 0x8
    void ResetToDefaults();
    BOOL MatureLanguageFilter() const;
    void SetMatureLanguageFilter(BOOL);
    BOOL EchoQuestDialogToChat() const { return bEchoQuestDialogToChat; }  // line 783; inlined into UIConversation::PlayLine (load of +0x4)
    void SetEchoQuestDialogToChat(BOOL);
    BOOL bMatureLanguageFilter;  // 0x00
    BOOL bEchoQuestDialogToChat;  // 0x04
};
static_assert(sizeof(ChatPrefs) == 0x8, "ChatPrefs");

struct PreferencesDefinition {  // size 0x66c
    BOOL bSuccessfullyLoadedBNetAccountSettings;  // 0x00
    int32 nVersion;  // 0x04
    uint8 bPlayedCutscene[5];  // 0x08
    BOOL bFirstTimeFlowCompleted;  // 0x10
    float flWhatsNewSeen;  // 0x14
    int32 nAnniversarySeen;  // 0x18
    int32 nLeaderboardSeenSeason;  // 0x1c
    int32 nSeasonTutorialsSeen;  // 0x20
    int32 nEulaAccepted;  // 0x24
    int32 nWhatsNewSeenNX64;  // 0x28
    VideoPrefs tVideoPrefs;  // 0x2c
    SoundPrefs tSoundPrefs;  // 0xdc
    KeybindingPrefs tKeybindingPrefs;  // 0x128
    GameplayPrefs tGameplayPrefs;  // 0x5ac
    MacPrefs tMacPrefs;  // 0x61c
    SocialPrefs tSocialPrefs;  // 0x628
    ChatPrefs tChatPrefs;  // 0x658
    ConsoleMultiplayerPrefs tConsoleMultiplayerPrefs;  // 0x660
};
static_assert(sizeof(PreferencesDefinition) == 0x66c, "PreferencesDefinition");

typedef int32 UIAction;

struct BufferedInput;

void PreferencesGetDefaultData(PreferencesDefinition * pdef);

namespace Prefs {
typedef BOOL (*IteratorCallback)(LPCSTR, const XTypeDescriptor *, const void *, void *);

typedef OnlineService::Callback<void (int)> LoadFromAccountCallback;

void InitializeForApp();
void Load(const char * szPath);
void Parse(char * szFileData, uint32 uFileSize);
int32 Save(uint8 * ptBuffer, int32 nBufferSize);
void Backup();
void Restore();
void ResetToDefaults();
void ResetToBlizzConDefaults();
void ParseFileDataToKeyValues(CHAR * sFileData, uint32 uFileSize, KeyValueMap & mapKeyValues);
const VideoPrefs & GetVideoPrefs();
VideoPrefs & GetMutableVideoPrefs();
const ActionBinding & GetKeybinding(uint32 uBindingIndex);
const SoundPrefs & GetSoundPrefs();
SoundPrefs & GetMutableSoundPrefs();
const SocialPrefs & GetSocialPrefs();
SocialPrefs & GetMutableSocialPrefs();
const ChatPrefs & GetChatPrefs();
ChatPrefs & GetMutableChatPrefs();
const GameplayPrefs & GetGameplayPrefs();
GameplayPrefs & GetMutableGameplayPrefs();
const MacPrefs & GetMacPrefs();
MacPrefs & GetMutableMacPrefs();
ConsoleMultiplayerPrefs & GetConsoleMultiplayerPrefs();
BOOL & GetFirstTimeFlowCompleted();
float & GetWhatsNewSeen();
int32 GetEulaAccepted();
void SetEulaAccepted(int32 nAccepted);
int32 & GetWhatsNewSeenNX64();
int32 & GetLeaderboardSeenSeason();
int32 & GetSeasonTutorialsSeen();
int32 & GetAnniversarySeen();
uint8 * GetPlayedCutScenes(uint32 & nDataLen);
uint32 GetCurrentKeyModifierFlags();
BOOL CheckAction(const UIAction eAction, BOOL bValidateModifiers);
BOOL ActionKeyMatch(const int32 nActionIndex, const Key eKey, const BOOL bExactMatch);
CRefString GetActionKeyName(const int32 nActionIndex, const BOOL fShortForm, const uint32 nForcedAssignmentIndex);
void GetActionKey(const int32 nActionIndex, Key & eKey, uint32 & dwModifiers, uint32 nForcedAssignmentIndex);
BOOL ActionAllowsModifierAsAssignment(UIAction eAction);
BOOL KeyIsModifier(Key eKey);
UIAction GetUIActionFromInput(const BufferedInput & tInput);
}  // namespace Prefs

// Functions defined in this file (bodies are not in the DWARF):
//   line   506  BOOL GameplayPrefs::GetShowBreadCrumb() const  [inlined into 2 functions]
//   line   509  BOOL GameplayPrefs::GetShowClock() const  [inline]
//   line   512  BOOL GameplayPrefs::GetShowTutorials() const  [inlined into 2 functions]
//   line   515  BOOL GameplayPrefs::GetShowHPBarNumbers() const  [inlined into 1 functions]
//   line   518  BOOL GameplayPrefs::GetShowMonsterHPBars() const  [inlined into 4 functions]
//   line   521  BOOL GameplayPrefs::GetShowPlayerHPBars() const  [inlined into 4 functions]
//   line   524  BOOL GameplayPrefs::GetShowDefensiveMessages() const  [inlined into 2 functions]
//   line   527  BOOL GameplayPrefs::GetShowCriticals() const  [inlined into 3 functions]
//   line   530  BOOL GameplayPrefs::GetShowDamageNumbers() const  [inlined into 3 functions]
//   line   533  BOOL GameplayPrefs::GetShowHealNumbers() const  [inlined into 3 functions]
//   line   536  BOOL GameplayPrefs::GetShowPlayerNames() const  [inlined into 2 functions]
//   line   539  BOOL GameplayPrefs::GetShowItemsOnGround() const  [inlined into 3 functions]
//   line   542  BOOL GameplayPrefs::GetSkillElectiveMode() const  [inlined into 8 functions]
//   line   545  BOOL GameplayPrefs::GetInventoryZoom() const  [inline]
//   line   548  BOOL GameplayPrefs::GetAutoEquipItems() const  [inlined into 2 functions]
//   line   551  BOOL GameplayPrefs::GetItemTagsAsIcons() const  [inlined into 2 functions]
//   line   554  BOOL GameplayPrefs::GetShowItemDroppedTooltips() const  [inlined into 2 functions]
//   line   562  BOOL GameplayPrefs::GetDisableCutscenesRaw() const  [inlined into 2 functions]
//   line   566  int32 GameplayPrefs::GetItemsOnGroundSetting() const  [inlined into 2 functions]
//   line   657  BOOL MacPrefs::GetDisableOSShortcuts() const  [inline]
//   line   660  BOOL MacPrefs::GetUseCommandAsControl() const  [inline]
//   line   663  BOOL MacPrefs::GetUseCommandClickAsRightClick() const  [inline]
//   line   723  BOOL SocialPrefs::GetNotifyFriendOnline() const  [inline]
//   line   726  BOOL SocialPrefs::GetNotifyFriendOffline() const  [inline]
//   line   729  BOOL SocialPrefs::GetNotifyFriendRequest() const  [inline]
//   line   732  BOOL SocialPrefs::GetNotifyFriendAchievement() const  [inline]
//   line   735  BOOL SocialPrefs::GetNotifyDisplayWindow() const  [inline]
//   line   738  int32 SocialPrefs::GetNotifyDuration() const  [inline]
//   line   741  BOOL SocialPrefs::GetAllowQuickJoin() const  [inline]
//   line   744  BOOL SocialPrefs::GetNotifyAuctionHouseEvents() const  [inline]
//   line   747  BOOL SocialPrefs::GetNotifyFriendBroadcastDisabled() const  [inline]
//   line   750  BOOL SocialPrefs::GetShowLocalPlayersDisabled() const  [inline]
//   line   753  BOOL SocialPrefs::GetNotifyGuildMemberOnlineDisabled() const  [inline]
//   line   756  BOOL SocialPrefs::GetNotifyGuildAchievementDisabled() const  [inline]
//   line   780  BOOL ChatPrefs::MatureLanguageFilter() const  [inline]
//   line   783  BOOL ChatPrefs::EchoQuestDialogToChat() const  [inlined into 1 functions]
