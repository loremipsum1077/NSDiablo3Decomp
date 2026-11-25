// Original file: X/Client/Definitions/PreferencesDefinition.cpp

#include "PreferencesDefinition.h"

#include "../CGame.h"
// For the RGBAColor normals, which the original also initializes here; it reaches GfxVB.h
// through an include chain the DWARF does not record.
#include "../Gfx/GfxVB.h"
#include "../Console/Online.h"
#include "../UserInterface/UIID.h"
#include "../UserInterface/Controls/UIWindowControl.h"
#include "../../Common/GlobalSNO.h"
#include "../../Common/OnlineService/OnlineService.h"
#include "../../Sigma/DataStructures/XBitArray.h"
#include "../../Sigma/Files/XFiles.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/RefString.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../../Sigma/UnicodeText.h"
#include <stddef.h>
#include <stdio.h>

// Declared here because their headers are not adopted yet. Each comment names the file that
// defines the function or object.
#include "../../Common/Shell/CmdLine.h"






const CmdLineParams * CmdLineGetParams();
// X/Sigma/Files/XFilesCore.cpp
void FileReferenceInit(FileReference & tFileRef, LPCSTR szPath);
BOOL FileExists(const FileReference & tFileRef);
BOOL FileOpen(FileReference & tFileRef, uint32 dwFlags);
void FileClose(FileReference & tFileRef);
CHAR * FileReadIntoMemory(FileReference & tFileRef, uint32 & dwSize, ErrorCode eErrorCode);
// X/Common/SNO/SNOFiles.cpp
CRefString SNOFilesGetUserDataPath(LPCSTR szBranchName);
// X/Common/StringList.cpp
CRefString StringListFetch(SNO snoStringList, LPCSTR szLabel);
// X/Sigma/TypeDescriptor/SymbolicValue.cpp:63
LPCSTR SymbolToName(int32 nSymbol, const SymbolicEntry * pTable);
// X/Common/SNO/SymbolicValue.cpp:1332
extern const SymbolicEntry gKeysTex[154];
// X/Client/Input/Input.cpp
BOOL InputAltKeyIsDown(InputContext eContext);
BOOL InputCtrlKeyIsDown(InputContext eContext);
BOOL InputShiftKeyIsDown(InputContext eContext);
BOOL InputIsKeyboard(const BufferedInput & tInput);
BOOL InputIsController(const BufferedInput & tInput);
// X/Client/Definitions/ControllerSchemeDefinition.cpp:400
namespace Controller {
BOOL ActionButtonMatch(const int32 nActionIndex, const BufferedInput & tInput);
}  // namespace Controller
// X/Client/UserInterface/Console/UIConsoleOptionsDataSource.cpp:999
namespace UIOptions {
namespace Console {
XLocale GetDefaultTextLocale();
XLocale GetDefaultAudioLocale();
}  // namespace Console
}  // namespace UIOptions
// X/Common/Game.cpp
BOOL GameIsOpenWorld();
BOOL GameIsWeeklyChallenge();
// X/Client/Gfx/Gfx.cpp
void GfxSetPrefsChanged();
// X/Client/UserInterface/UI.cpp
BOOL UI_TestID(const UIID & id);
UIWindow & UI_GetWindowFromID(const UIID & id);

enum {
    PREFERENCES_VERSION = 46,
    KEYBINDING_VERSION = 4,
};

const char * VERSION_KEY = "PreferencesVersion";

static PreferencesDefinition s_tPrefsDef;
static BOOL s_bLoaded;
static BOOL s_bAccountPrefsDirty;

static PreferencesDefinition s_tPrefsBackup;
static BOOL s_bBackupValid;
static BOOL s_bBackupAccountPrefsDirty;

static const float GAMMA_DEFAULT = 1.0f;
static const float GAMMA_MIN = 0.5f;
static const float GAMMA_MAX = 1.5f;
static const float MIP_BIAS_MIN = -10.0f;
static const float MIP_BIAS_MAX = 10.0f;
static const float MIP_BIAS_DEFAULT = -0.0f;

static const int32 REFLECTION_QUALITY_DEFAULT = 1;
static const int32 REFLECTION_QUALITY_MIN = 0;
const int32 REFLECTION_QUALITY_MAX = 1;
static const int32 PHYSICS_QUALITY_MIN = 0;
static const int32 PHYSICS_QUALITY_MAX = 1;
static const float VOLUME_MIN = 0.0f;
static const float VOLUME_MAX = 1.0f;
static const int32 SOUND_CHANNEL_COUNT_MIN = 16;
static const int32 SOUND_CHANNEL_COUNT_MAX = 128;
static const float SAFEZONE_DEFAULT_LEFT = 0.03f;
static const float SAFEZONE_DEFAULT_TOP = 0.03f;
static const float SAFEZONE_DEFAULT_RIGHT = 0.97f;
static const float SAFEZONE_DEFAULT_BOTTOM = 0.97f;

// The key codes are #defines in the original; the DWARF has no names for them.
static const ActionBinding cgtDefaultBindings[NUM_ACTIONS] = {
    { 22, 0, 45, 0 },
    { 32, 0, -1, 0 },
    { 30, 0, -1, 0 },
    { 35, 0, -1, 0 },
    { 37, 0, -1, 0 },
    { 37, 4, -1, 0 },
    { 23, 0, -1, 0 },
    { 23, 4, -1, 0 },
    { 48, 0, -1, 0 },
    { 58, 0, -1, 0 },
    { 49, 0, -1, 0 },
    { 36, 0, -1, 0 },
    { 23, 2, -1, 0 },
    { 21, 0, -1, 0 },
    { 20, 0, 66, 0 },
    { 24, 4, 67, 0 },
    { 29, 0, 84, 0 },
    { 19, 0, -1, 0 },
    { 56, 0, -1, 0 },
    { 40, 0, -1, 0 },
    { 14, 0, -1, 0 },
    { -1, 0, -1, 0 },
    { 46, 0, -1, 0 },
    { 31, 0, -1, 0 },
    { -1, 0, -1, 0 },
    { 55, 0, -1, 0 },
    { 18, 4, -1, 0 },
    { 18, 0, -1, 0 },
    { 40, 0, -1, 0 },
    { 121, 0, -1, 0 },
    { 126, 0, -1, 0 },
    { 14, 2, -1, 0 },
    { 14, 6, -1, 0 },
    { 44, 0, -1, 0 },
    { 1, 0, -1, 0 },
    { 2, 0, -1, 0 },
    { 3, 0, -1, 0 },
    { 4, 0, -1, 0 },
    { 15, 0, -1, 0 },
    { 33, 0, -1, 0 },
    { 43, 0, -1, 0 },
    { 78, 0, -1, 0 },
    { 79, 0, -1, 0 },
    { 80, 0, -1, 0 },
    { 74, 0, -1, 0 },
    { 75, 0, -1, 0 },
    { 76, 0, -1, 0 },
    { 70, 0, -1, 0 },
    { 71, 0, -1, 0 },
    { 72, 0, -1, 0 },
    { 81, 0, -1, 0 },
    { -1, 0, -1, 0 },
    { -1, 0, -1, 0 },
    { -1, 0, -1, 0 },
    { -1, 0, -1, 0 },
    { -1, 0, -1, 0 },
    { -1, 0, -1, 0 },
    { -1, 0, -1, 0 },
    { -1, 0, -1, 0 },
    { 41, 0, -1, 0 },
    { 12, 2, -1, 0 },
    { 11, 2, -1, 0 },
    { 30, 2, -1, 0 },
    { 49, 2, -1, 0 },
    { 18, 2, -1, 0 },
    { 116, 0, -1, 0 },
    { 19, 0, -1, 0 },
    { -1, 0, -1, 0 },
    { 24, 0, -1, 0 },
    { 35, 4, -1, 0 },
    { -1, 0, -1, 0 },
    { 43, 1, -1, 0 },
    { 128, 0, -1, 0 },
    { 120, 0, -1, 0 },
    { 125, 0, -1, 0 },
    { 27, 0, 52, 0 },
    { 14, 0, -1, 0 },
    { 41, 0, -1, 0 },
    { 55, 0, -1, 0 },
    { 28, 0, -1, 0 },
    { 27, 0, -1, 0 },
    { 0, 0, -1, 0 },
    { 0, 0, -1, 0 },
    { 144, 0, -1, 0 },
    { 145, 0, -1, 0 },
    { -1, 0, -1, 0 },
    { -1, 0, -1, 0 },
    { -1, 0, -1, 0 },
    { 120, 0, -1, 0 },
    { 125, 0, -1, 0 },
    { 121, 0, -1, 0 },
    { 126, 0, -1, 0 },
    { 119, 0, -1, 0 },
    { 124, 0, -1, 0 },
    { 122, 0, -1, 0 },
    { 123, 0, -1, 0 },
    { -1, 0, -1, 0 },
    { 0, 0, -1, 0 },
};

// gActionNames[96] (line 223) is not in the binary: the linker dropped it, so its contents
// are unknown.

struct PrefValue {  // size 0x4
    PrefValue() {}
    PrefValue(int32 arg_nValue) : nValue(arg_nValue) {}
    PrefValue(uint32 arg_nValue) : nValue(arg_nValue) {}
    PrefValue(long int arg_nValue);
    PrefValue(float arg_flValue) : flValue(arg_flValue) {}
    union {
        float flValue;  // 0x00
        int32 nValue;  // 0x00
    };
};

struct PrefField {  // size 0x28
    LPCSTR szFieldName;  // 0x00
    const XTypeDescriptor * ptDataType;  // 0x08
    int32 nDataOffset;  // 0x10
    BOOL bBounded;  // 0x14
    BOOL bSerializeAxeOnly;  // 0x18
    PrefValue tDefaultValue;  // 0x1c
    PrefValue tMinValue;  // 0x20
    PrefValue tMaxValue;  // 0x24
};

// The field-table macros below have invented names; the original's are not in the DWARF.
#define VIDEO_OFFSET(m) offsetof(VideoPrefs, m)
#define DISPLAYMODE_OFFSET(m) offsetof(VideoPrefs, tVideoPrefsToConfirm.tDisplayMode.m)

static const PrefField sgtPrefFields[] = {
    { VERSION_KEY, DT_INT, offsetof(PreferencesDefinition, nVersion), FALSE, FALSE, PrefValue(PREFERENCES_VERSION) },
    { "PlayedCutscene0", DT_BYTE, offsetof(PreferencesDefinition, bPlayedCutscene[0]), FALSE, FALSE, PrefValue(1) },
    { "PlayedCutscene1", DT_BYTE, offsetof(PreferencesDefinition, bPlayedCutscene[1]), FALSE, FALSE, PrefValue(0) },
    { "PlayedCutscene2", DT_BYTE, offsetof(PreferencesDefinition, bPlayedCutscene[2]), FALSE, FALSE, PrefValue(0) },
    { "PlayedCutscene3", DT_BYTE, offsetof(PreferencesDefinition, bPlayedCutscene[3]), FALSE, FALSE, PrefValue(0) },
    { "PlayedCutscene4", DT_BYTE, offsetof(PreferencesDefinition, bPlayedCutscene[4]), FALSE, FALSE, PrefValue(0) },
    { "FirstTimeFlowCompleted", DT_INT, offsetof(PreferencesDefinition, bFirstTimeFlowCompleted), FALSE, FALSE, PrefValue(FALSE) },
    { "WhatsNewSeen", DT_FLOAT, offsetof(PreferencesDefinition, flWhatsNewSeen), FALSE, FALSE, PrefValue(0.0f) },
    { "AnniversarySeen", DT_INT, offsetof(PreferencesDefinition, nAnniversarySeen), FALSE, FALSE, PrefValue(0) },
    { "LeaderboardSeenSeason", DT_INT, offsetof(PreferencesDefinition, nLeaderboardSeenSeason), FALSE, FALSE, PrefValue(0) },
    { "SeasonTutorialsSeen", DT_INT, offsetof(PreferencesDefinition, nSeasonTutorialsSeen), FALSE, FALSE, PrefValue(0) },
    { "EulaAcceptedNX64", DT_INT, offsetof(PreferencesDefinition, nEulaAccepted), FALSE, FALSE, PrefValue(-1) },
    { "WhatsNewSeenNX64", DT_INT, offsetof(PreferencesDefinition, nWhatsNewSeenNX64), FALSE, FALSE, PrefValue(0) },
};

static const PrefField sgtVideoPrefFields[] = {
    { "DisplayModeFlags", DT_INT, DISPLAYMODE_OFFSET(dwFlags), FALSE, FALSE, PrefValue(cgtDisplayModeDefault.dwFlags) },
    { "DisplayModeWindowMode", DT_INT, DISPLAYMODE_OFFSET(dwWindowMode), TRUE, FALSE, PrefValue(cgtDisplayModeDefault.dwWindowMode), PrefValue(0), PrefValue(2) },
    { "DisplayModeWinLeft", DT_INT, DISPLAYMODE_OFFSET(nWinLeft), FALSE, FALSE, PrefValue(cgtDisplayModeDefault.nWinLeft) },
    { "DisplayModeWinTop", DT_INT, DISPLAYMODE_OFFSET(nWinTop), FALSE, FALSE, PrefValue(cgtDisplayModeDefault.nWinTop) },
    { "DisplayModeWinWidth", DT_INT, DISPLAYMODE_OFFSET(nWinWidth), FALSE, FALSE, PrefValue(cgtDisplayModeDefault.nWinWidth) },
    { "DisplayModeWinHeight", DT_INT, DISPLAYMODE_OFFSET(nWinHeight), FALSE, FALSE, PrefValue(cgtDisplayModeDefault.nWinHeight) },
    { "DisplayModeUIOptWidth", DT_INT, DISPLAYMODE_OFFSET(dwUIOptWidth), FALSE, FALSE, PrefValue(cgtDisplayModeDefault.dwUIOptWidth) },
    { "DisplayModeUIOptHeight", DT_INT, DISPLAYMODE_OFFSET(dwUIOptHeight), FALSE, FALSE, PrefValue(cgtDisplayModeDefault.dwUIOptHeight) },
    { "DisplayModeWidth", DT_INT, DISPLAYMODE_OFFSET(dwWidth), FALSE, FALSE, PrefValue(cgtDisplayModeDefault.dwWidth) },
    { "DisplayModeHeight", DT_INT, DISPLAYMODE_OFFSET(dwHeight), FALSE, FALSE, PrefValue(cgtDisplayModeDefault.dwHeight) },
    { "DisplayModeRefreshRate", DT_INT, DISPLAYMODE_OFFSET(nRefreshRate), FALSE, FALSE, PrefValue(cgtDisplayModeDefault.nRefreshRate) },
    { "DisplayModeBitDepth", DT_INT, DISPLAYMODE_OFFSET(dwBitDepth), FALSE, FALSE, PrefValue(cgtDisplayModeDefault.dwBitDepth) },
    { "DisplayModeMSAALevel", DT_INT, DISPLAYMODE_OFFSET(dwMSAALevel), FALSE, FALSE, PrefValue(cgtDisplayModeDefault.dwMSAALevel) },
    { "Gamma", DT_FLOAT, VIDEO_OFFSET(flGamma), TRUE, FALSE, PrefValue(GAMMA_DEFAULT), PrefValue(GAMMA_MIN), PrefValue(GAMMA_MAX) },
    { "MipOffset", DT_INT, VIDEO_OFFSET(tVideoPrefsToConfirm.nMipOffset), FALSE, FALSE, PrefValue(0) },
    { "ShadowQuality", DT_INT, VIDEO_OFFSET(tVideoPrefsToConfirm.nShadowQuality), TRUE, FALSE, PrefValue(SHADOWPREF_MID), PrefValue(SHADOWPREF_NONE), PrefValue(SHADOWPREF_HIGH_SOFT) },
    { "ShadowDetail", DT_INT, VIDEO_OFFSET(nShadowDetail), TRUE, FALSE, PrefValue(SHADOWDETAILPERF_LOW), PrefValue(SHADOWDETAILPERF_LOW), PrefValue(SHADOWDETAILPERF_HIGH) },
    { "PhysicsQuality", DT_INT, VIDEO_OFFSET(nPhysicsQuality), TRUE, FALSE, PrefValue(PHYSICS_QUALITY_MAX), PrefValue(PHYSICS_QUALITY_MIN), PrefValue(PHYSICS_QUALITY_MAX) },
    { "ClutterQuality", DT_INT, VIDEO_OFFSET(nClutterQuality), TRUE, FALSE, PrefValue(CLUTTER_MID), PrefValue(CLUTTER_NONE), PrefValue(CLUTTER_HIGH) },
    { "Vsync", DT_INT, VIDEO_OFFSET(tVideoPrefsToConfirm.bVsync), TRUE, FALSE, PrefValue(TRUE), PrefValue(FALSE), PrefValue(TRUE) },
    { "LargeCursor", DT_INT, VIDEO_OFFSET(tVideoPrefsToConfirm.bLargeCursor), TRUE, FALSE, PrefValue(FALSE), PrefValue(FALSE), PrefValue(TRUE) },
    { "Letterbox", DT_INT, VIDEO_OFFSET(bLetterbox), TRUE, FALSE, PrefValue(FALSE), PrefValue(FALSE), PrefValue(TRUE) },
    { "Antialiasing", DT_INT, VIDEO_OFFSET(bAntialiasing), TRUE, FALSE, PrefValue(TRUE), PrefValue(FALSE), PrefValue(TRUE) },
    { "DisableScreenShake", DT_INT, VIDEO_OFFSET(bDisableScreenShake), TRUE, FALSE, PrefValue(FALSE), PrefValue(FALSE), PrefValue(TRUE) },
    { "DisableChromaEffects", DT_INT, VIDEO_OFFSET(bDisableChromaEffects), TRUE, FALSE, PrefValue(FALSE), PrefValue(FALSE), PrefValue(TRUE) },
    { "SSAO", DT_INT, VIDEO_OFFSET(bSSAO), TRUE, FALSE, PrefValue(FALSE), PrefValue(FALSE), PrefValue(TRUE) },
    { "LockCursorInFullscreenWindowed", DT_INT, VIDEO_OFFSET(bLockCursorInFullscreenWindowed), TRUE, FALSE, PrefValue(FALSE), PrefValue(FALSE), PrefValue(TRUE) },
    { "LimitForegroundFPS", DT_INT, VIDEO_OFFSET(bLimitForegroundFPS), TRUE, FALSE, PrefValue(TRUE), PrefValue(FALSE), PrefValue(TRUE) },
    { "MaxForegroundFPS", DT_INT, VIDEO_OFFSET(nMaxForegroundFPS), TRUE, FALSE, PrefValue(150), PrefValue(1), PrefValue(200) },
    { "LimitBackgroundFPS", DT_INT, VIDEO_OFFSET(bLimitBackgroundFPS), TRUE, FALSE, PrefValue(TRUE), PrefValue(FALSE), PrefValue(TRUE) },
    { "MaxBackgroundFPS", DT_INT, VIDEO_OFFSET(nMaxBackgroundFPS), TRUE, FALSE, PrefValue(8), PrefValue(1), PrefValue(200) },
    { "DisableTrilinearFiltering", DT_INT, VIDEO_OFFSET(bDisableTrilinearFiltering), TRUE, FALSE, PrefValue(FALSE), PrefValue(FALSE), PrefValue(TRUE) },
    { "ColorCorrection", DT_INT, VIDEO_OFFSET(bColorCorrection), TRUE, FALSE, PrefValue(TRUE), PrefValue(FALSE), PrefValue(TRUE) },
    { "MipBias", DT_FLOAT, VIDEO_OFFSET(flMipBias), TRUE, FALSE, PrefValue(MIP_BIAS_DEFAULT), PrefValue(MIP_BIAS_MIN), PrefValue(MIP_BIAS_MAX) },
    { "ReflectionQuality", DT_INT, VIDEO_OFFSET(nReflectionQuality), TRUE, FALSE, PrefValue(REFLECTION_QUALITY_DEFAULT), PrefValue(REFLECTION_QUALITY_MIN), PrefValue(REFLECTION_QUALITY_MAX) },
    { "HardwareClass", DT_INT, VIDEO_OFFSET(eHWClass), TRUE, FALSE, PrefValue(4), PrefValue(0), PrefValue(6) },
    { "PCIVendor", DT_INT, VIDEO_OFFSET(dwPCIVendor), FALSE, FALSE, PrefValue(0) },
    { "PCIDevice", DT_INT, VIDEO_OFFSET(dwPCIDevice), FALSE, FALSE, PrefValue(0) },
    { "flSafeZoneLeft", DT_FLOAT, VIDEO_OFFSET(flSafeZoneLeft), FALSE, FALSE, PrefValue(SAFEZONE_DEFAULT_LEFT) },
    { "flSafeZoneTop", DT_FLOAT, VIDEO_OFFSET(flSafeZoneTop), FALSE, FALSE, PrefValue(SAFEZONE_DEFAULT_TOP) },
    { "flSafeZoneRight", DT_FLOAT, VIDEO_OFFSET(flSafeZoneRight), FALSE, FALSE, PrefValue(SAFEZONE_DEFAULT_RIGHT) },
    { "flSafeZoneBottom", DT_FLOAT, VIDEO_OFFSET(flSafeZoneBottom), FALSE, FALSE, PrefValue(SAFEZONE_DEFAULT_BOTTOM) },
};

static const PrefField sgtSoundPrefFields[] = {
    { "MasterVolume", DT_FLOAT, offsetof(SoundPrefs, flMasterVolume), TRUE, FALSE, PrefValue(1.0f), PrefValue(VOLUME_MIN), PrefValue(VOLUME_MAX) },
    { "EffectVolume", DT_FLOAT, offsetof(SoundPrefs, flTypeVolume[SOUNDTYPE_EFFECT]), TRUE, FALSE, PrefValue(0.8f), PrefValue(VOLUME_MIN), PrefValue(VOLUME_MAX) },
    { "MusicVolume", DT_FLOAT, offsetof(SoundPrefs, flTypeVolume[SOUNDTYPE_MUSIC]), TRUE, FALSE, PrefValue(0.8f), PrefValue(VOLUME_MIN), PrefValue(VOLUME_MAX) },
    { "NarrationVolume", DT_FLOAT, offsetof(SoundPrefs, flTypeVolume[SOUNDTYPE_NARRATION]), TRUE, FALSE, PrefValue(0.8f), PrefValue(VOLUME_MIN), PrefValue(VOLUME_MAX) },
    { "AmbientVolume", DT_FLOAT, offsetof(SoundPrefs, flTypeVolume[SOUNDTYPE_AMBIENT]), TRUE, FALSE, PrefValue(0.8f), PrefValue(VOLUME_MIN), PrefValue(VOLUME_MAX) },
    { "ChannelsToUse", DT_INT, offsetof(SoundPrefs, nChannelsToUse), TRUE, FALSE, PrefValue(32), PrefValue(SOUND_CHANNEL_COUNT_MIN), PrefValue(SOUND_CHANNEL_COUNT_MAX) },
    { "SoundDriver", DT_INT, offsetof(SoundPrefs, dwDriverHash), FALSE, FALSE, PrefValue(0) },
    { "SpeakerMode", DT_INT, offsetof(SoundPrefs, eSpeakerMode), TRUE, FALSE, PrefValue(0), PrefValue(0), PrefValue(7) },
    { "ReverseSpeakers", DT_INT, offsetof(SoundPrefs, bReverseSpeakers), TRUE, FALSE, PrefValue(FALSE), PrefValue(FALSE), PrefValue(TRUE) },
    { "QuestSubtitlesEnabled", DT_INT, offsetof(SoundPrefs, bQuestSubtitlesEnabled), TRUE, FALSE, PrefValue(FALSE), PrefValue(FALSE), PrefValue(TRUE) },
    { "CinematicsSubtitlesEnabled", DT_INT, offsetof(SoundPrefs, bCinematicsSubtitlesEnabled), TRUE, FALSE, PrefValue(FALSE), PrefValue(FALSE), PrefValue(TRUE) },
    { "PlayInBackground", DT_INT, offsetof(SoundPrefs, bPlayInBackground), TRUE, FALSE, PrefValue(FALSE), PrefValue(FALSE), PrefValue(TRUE) },
    { "MuteSound", DT_INT, offsetof(SoundPrefs, bMuteSound), TRUE, FALSE, PrefValue(FALSE), PrefValue(FALSE), PrefValue(TRUE) },
    { "MuteEffects", DT_INT, offsetof(SoundPrefs, bMuteSoundType[SOUNDTYPE_EFFECT]), TRUE, FALSE, PrefValue(FALSE), PrefValue(FALSE), PrefValue(TRUE) },
    { "MuteAmbient", DT_INT, offsetof(SoundPrefs, bMuteSoundType[SOUNDTYPE_AMBIENT]), TRUE, FALSE, PrefValue(FALSE), PrefValue(FALSE), PrefValue(TRUE) },
    { "MuteVoice", DT_INT, offsetof(SoundPrefs, bMuteSoundType[SOUNDTYPE_NARRATION]), TRUE, FALSE, PrefValue(FALSE), PrefValue(FALSE), PrefValue(TRUE) },
    { "MuteMusic", DT_INT, offsetof(SoundPrefs, bMuteSoundType[SOUNDTYPE_MUSIC]), TRUE, FALSE, PrefValue(FALSE), PrefValue(FALSE), PrefValue(TRUE) },
};

#define GAMEPLAY_BOOL(name, member, def) \
    { name, DT_INT, offsetof(GameplayPrefs, member), TRUE, TRUE, PrefValue(def), PrefValue(FALSE), PrefValue(TRUE) }

static const PrefField sgtGameplayPrefFields[] = {
    { "ItemsOnGroundSetting", DT_INT, offsetof(GameplayPrefs, nItemsOnGroundSetting), TRUE, TRUE, PrefValue(ITEMS_ON_GROUND_SHOW_FOR_DURATION), PrefValue(ITEMS_ON_GROUND_SHOW_WHILE_HELD), PrefValue(ITEMS_ON_GROUND_SHOW_FOR_DURATION) },
    GAMEPLAY_BOOL("ItemTagsAsIcons", bItemTagsAsIcons, FALSE),
    GAMEPLAY_BOOL("ShowItemTooltipOnDrop", bShowItemTooltipOnDrop, TRUE),
    GAMEPLAY_BOOL("ShowMonsterHPBars", bShowMonsterHPBars, FALSE),
    GAMEPLAY_BOOL("ShowItemsOnGround", bShowItemsOnGround, FALSE),
    GAMEPLAY_BOOL("ShowHPBarNumbers", bShowHPBarNumbers, FALSE),
    GAMEPLAY_BOOL("ShowHealNumbers", bShowHealNumbers, FALSE),
    GAMEPLAY_BOOL("ShowDamageNumbers", bShowDamageNumbers, FALSE),
    GAMEPLAY_BOOL("ShowCriticals", bShowCriticals, TRUE),
    GAMEPLAY_BOOL("ShowDefensiveMessages", bShowDefensiveMessages, TRUE),
    GAMEPLAY_BOOL("InventoryZoom", bInventoryZoom, FALSE),
    GAMEPLAY_BOOL("ShowPlayerNames", bShowPlayerNames, FALSE),
    GAMEPLAY_BOOL("ShowPlayerHPBars", bShowPlayerHPBars, FALSE),
    GAMEPLAY_BOOL("AutoEquipItems", bAutoEquipItems, TRUE),
    GAMEPLAY_BOOL("EnableSmartCoopMinimap", bEnableSmartCoopMinimap, FALSE),
    GAMEPLAY_BOOL("AlwaysShowCampaignObjectives", bAlwaysShowCampaignObjectives, FALSE),
    GAMEPLAY_BOOL("AlwaysShowAdventureObjectives", bAlwaysShowAdventureObjectives, TRUE),
    { "SelectedTextLocale", DT_INT, offsetof(GameplayPrefs, nSelectedTextLocale), TRUE, TRUE, PrefValue(UIOptions::Console::GetDefaultTextLocale()), PrefValue(XLOCALE_INVALID), PrefValue(NUM_XLOCALES) },
    { "SelectedAudioLocale", DT_INT, offsetof(GameplayPrefs, nSelectedAudioLocale), TRUE, TRUE, PrefValue(UIOptions::Console::GetDefaultAudioLocale()), PrefValue(XLOCALE_INVALID), PrefValue(NUM_XLOCALES) },
    GAMEPLAY_BOOL("ShowTutorials", bShowTutorials, TRUE),
    GAMEPLAY_BOOL("ShowBreadCrumbs", bShowBreadCrumb, TRUE),
    GAMEPLAY_BOOL("ShowClock", bShowClock, TRUE),
    GAMEPLAY_BOOL("ShowAdvancedTooltips", bShowAdvancedTooltips, FALSE),
    GAMEPLAY_BOOL("ElectiveMode", bElectiveMode, FALSE),
    GAMEPLAY_BOOL("DisableCutscenes", bDisableCutscenes, FALSE),
    GAMEPLAY_BOOL("ItemRarityIcons", bItemRarityIcons, FALSE),
    GAMEPLAY_BOOL("DisableShortFloatingNumbers", bDisableShortFloatingNumbers, FALSE),
    GAMEPLAY_BOOL("EquipmentManagerDropToStash", bEquipmentManagerDropToStash, TRUE),
};

static const PrefField sgtMacPrefFields[] = {
    { "DisableOSShortcuts", DT_INT, offsetof(MacPrefs, bDisableOSShortcuts), TRUE, TRUE, PrefValue(FALSE), PrefValue(FALSE), PrefValue(TRUE) },
    { "UseCommandAsControl", DT_INT, offsetof(MacPrefs, bUseCommandAsControl), TRUE, TRUE, PrefValue(TRUE), PrefValue(FALSE), PrefValue(TRUE) },
    { "UseCommandClickAsRightClick", DT_INT, offsetof(MacPrefs, bUseCommandClickAsRightClick), TRUE, TRUE, PrefValue(TRUE), PrefValue(FALSE), PrefValue(TRUE) },
};

static const PrefField sgtConsoleMultiplayerPrefFields[] = {
    { "MatchmakeRegionOverride", DT_INT, offsetof(ConsoleMultiplayerPrefs, nMatchmakeRegion), TRUE, TRUE, PrefValue(0), PrefValue(0), PrefValue(7) },
    { "MatchmakeRegionStrict", DT_INT, offsetof(ConsoleMultiplayerPrefs, bMatchmakeRegionStrict), TRUE, TRUE, PrefValue(TRUE), PrefValue(FALSE), PrefValue(TRUE) },
    { "MatchmakeLanguageStrict", DT_INT, offsetof(ConsoleMultiplayerPrefs, bMatchmakeLanguageStrict), TRUE, TRUE, PrefValue(FALSE), PrefValue(FALSE), PrefValue(TRUE) },
};

#define SOCIAL_BOOL(name, member, def) \
    { name, DT_INT, offsetof(SocialPrefs, member), TRUE, TRUE, PrefValue(def), PrefValue(FALSE), PrefValue(TRUE) }

static const PrefField sgtSocialPrefFields[] = {
    SOCIAL_BOOL("NotifyFriendOnline", bNotifyFriendOnline, TRUE),
    SOCIAL_BOOL("NotifyFriendOffline", bNotifyFriendOffline, FALSE),
    SOCIAL_BOOL("NotifyFriendRequest", bNotifyFriendRequest, TRUE),
    SOCIAL_BOOL("NotifyFriendAchievement", bNotifyFriendAchievement, TRUE),
    SOCIAL_BOOL("NotifyDisplayWindow", bNotifyDisplayWindow, TRUE),
    SOCIAL_BOOL("NotifyDuration", nNotifyDuration, 1),
    SOCIAL_BOOL("AllowQuickJoin", bAllowQuickJoin, TRUE),
    SOCIAL_BOOL("NotifyAuctionHouseEvent", bNotifyAuctionHouseEvents, TRUE),
    SOCIAL_BOOL("NotifyFriendBroadcastDisabled", bNotifyFriendBroadcastDisabled, FALSE),
    SOCIAL_BOOL("ShowLocalPlayersListDisabled", bShowLocalPlayersListDisabled, FALSE),
    SOCIAL_BOOL("NotifyGuildMemberOnlineDisabled", bNotifyGuildMemberOnlineDisabled, FALSE),
    SOCIAL_BOOL("NotifyGuildAchievementDisabled", bNotifyGuildAchievementDisabled, FALSE),
    SOCIAL_BOOL("SocialFeaturesDisabled", bSocialFeaturesDisabled, FALSE),
};

static const PrefField sgtKeybindingPrefFields[] = {
    { "KeybindingVersion", DT_INT, offsetof(KeybindingPrefs, nKeybindingVersion), FALSE, TRUE, PrefValue(KEYBINDING_VERSION) },
};

static const PrefField sgtChatPrefFields[] = {
    { "MatureLanguageFilter", DT_INT, offsetof(ChatPrefs, bMatureLanguageFilter), TRUE, TRUE, PrefValue(TRUE), PrefValue(FALSE), PrefValue(TRUE) },
    { "EchoQuestDialogToChat", DT_INT, offsetof(ChatPrefs, bEchoQuestDialogToChat), TRUE, TRUE, PrefValue(TRUE), PrefValue(FALSE), PrefValue(TRUE) },
};

#define PREF_FIELD_COUNT(a) (sizeof(a) / sizeof((a)[0]))

namespace Prefs {

inline PreferencesDefinition * Get() {
    if (!s_bLoaded) {
        Load(NULL);
    }
    return &s_tPrefsDef;
}

static CRefString sGetPrefsLocation() {
    if (OnlineService::IsPTR()) {
        return SNOFilesGetUserDataPath(NULL) + CRefString("\\D3PrefsPTR.txt");
    }
    return SNOFilesGetUserDataPath(NULL) + CRefString("\\D3Prefs.txt");
}

const VideoPrefs & GetVideoPrefs() {
    return Get()->tVideoPrefs;
}

VideoPrefs & GetMutableVideoPrefs() {
    return Get()->tVideoPrefs;
}

inline const ActionBinding & GetKeybinding(uint32 uBindingIndex) {
    return Get()->tKeybindingPrefs.tBindings[uBindingIndex];
}

const SoundPrefs & GetSoundPrefs() {
    return Get()->tSoundPrefs;
}

SoundPrefs & GetMutableSoundPrefs() {
    return Get()->tSoundPrefs;
}

const ChatPrefs & GetChatPrefs() {
    return Get()->tChatPrefs;
}

const GameplayPrefs & GetGameplayPrefs() {
    return Get()->tGameplayPrefs;
}

GameplayPrefs & GetMutableGameplayPrefs() {
    return Get()->tGameplayPrefs;
}

ConsoleMultiplayerPrefs & GetConsoleMultiplayerPrefs() {
    return Get()->tConsoleMultiplayerPrefs;
}

BOOL & GetFirstTimeFlowCompleted() {
    return Get()->bFirstTimeFlowCompleted;
}

float & GetWhatsNewSeen() {
    return Get()->flWhatsNewSeen;
}

int32 GetEulaAccepted() {
    return Get()->nEulaAccepted;
}

void SetEulaAccepted(int32 nAccepted) {
    Get()->nEulaAccepted = nAccepted;
}

int32 & GetWhatsNewSeenNX64() {
    return Get()->nWhatsNewSeenNX64;
}

int32 & GetLeaderboardSeenSeason() {
    return Get()->nLeaderboardSeenSeason;
}

int32 & GetSeasonTutorialsSeen() {
    return Get()->nSeasonTutorialsSeen;
}

int32 & GetAnniversarySeen() {
    return Get()->nAnniversarySeen;
}

void InitializeForApp() {
    Load(NULL);
}

}  // namespace Prefs

static void sGetDefaultDataForPrefFields(void * pdef, const PrefField * ptFields, const int32 nCount) {
    for (int32 i = 0; i < nCount; i++) {
        CRefString sValue;
        if (ptFields[i].ptDataType == DT_INT) {
            *(int32 *)((uintptr_t)pdef + ptFields[i].nDataOffset) = ptFields[i].tDefaultValue.nValue;
        } else if (ptFields[i].ptDataType == DT_BYTE) {
            *(uint8 *)((uintptr_t)pdef + ptFields[i].nDataOffset) = (uint8)ptFields[i].tDefaultValue.nValue;
        } else if (ptFields[i].ptDataType == DT_FLOAT) {
            *(float *)((uintptr_t)pdef + ptFields[i].nDataOffset) = ptFields[i].tDefaultValue.flValue;
        }
    }
}

struct PrefsWriter {  // size 0x10
    uint8 * m_ptBuffer;  // 0x00
    int32 m_nBufferSize;  // 0x08
    int32 m_nCurrentPos;  // 0x0c
};

inline BOOL WritePrefs(PrefsWriter & tFileRef, const void * lpBuf, const uint32 dwOffset, const uint32 dwSize, ErrorCode eErrorCodeIfFails) {
    int32 nDataLeft = tFileRef.m_nBufferSize - tFileRef.m_nCurrentPos;
    if ((int32)dwSize > nDataLeft) {
        return FALSE;
    }
    SigmaMemoryCopy(tFileRef.m_ptBuffer + tFileRef.m_nCurrentPos, lpBuf, dwSize);
    tFileRef.m_nCurrentPos += dwSize;
    return TRUE;
}

static BOOL sSavePrefFields(PrefsWriter & tFileRef, const void * pdef, const PrefField * ptFields, const int32 nCount) {
    for (int32 i = 0; i < nCount; i++) {
        CRefString sValue;
        const PrefField & tField = ptFields[i];
        if (tField.ptDataType == DT_INT) {
            sValue = CRefString().Format("%d", *(const int32 *)((uintptr_t)pdef + tField.nDataOffset));
        } else if (tField.ptDataType == DT_BYTE) {
            sValue = CRefString().Format("%d", *(const uint8 *)((uintptr_t)pdef + tField.nDataOffset));
        } else if (tField.ptDataType == DT_FLOAT) {
            sValue = CRefString().Format("%f", *(const float *)((uintptr_t)pdef + tField.nDataOffset));
        }
        CRefString sPreference = CRefString().Format("%s \"%s\"\r\n", tField.szFieldName, sValue.str());
        if (!WritePrefs(tFileRef, sPreference.str(), 0, sPreference.Length(), ERROR_NONE)) {
            return FALSE;
        }
    }
    return TRUE;
}

static void sLoadPrefFields(KeyValueMap & mapKeyValues, const void * pdef, const PrefField * ptFields, const int32 nCount) {
    for (int32 i = 0; i < nCount; i++) {
        CRefString sValue;
        if (mapKeyValues.Lookup(ptFields[i].szFieldName, sValue)) {
            const PrefField & tField = ptFields[i];
            if (tField.ptDataType == DT_INT) {
                int32 nValue = 0;
                if (sscanf(sValue.str(), "%d", &nValue) == 1) {
                    if (tField.bBounded && (nValue < tField.tMinValue.nValue || nValue > tField.tMaxValue.nValue)) {
                        nValue = tField.tDefaultValue.nValue;
                    }
                    *(int32 *)((uintptr_t)pdef + tField.nDataOffset) = nValue;
                }
            } else if (tField.ptDataType == DT_BYTE) {
                int32 nValue = 0;
                if (sscanf(sValue.str(), "%d", &nValue) == 1) {
                    if (tField.bBounded && (nValue < tField.tMinValue.nValue || nValue > tField.tMaxValue.nValue)) {
                        nValue = tField.tDefaultValue.nValue;
                    }
                    *(uint8 *)((uintptr_t)pdef + tField.nDataOffset) = (uint8)nValue;
                }
            } else if (tField.ptDataType == DT_FLOAT) {
                float flValue = 0.0f;
                if (sscanf(sValue.str(), "%f", &flValue) == 1) {
                    if (tField.bBounded && (flValue < tField.tMinValue.flValue - 0.000001f || flValue > tField.tMaxValue.flValue + 0.000001f)) {
                        flValue = tField.tDefaultValue.flValue;
                    }
                    *(float *)((uintptr_t)pdef + tField.nDataOffset) = flValue;
                }
            }
        }
    }
}

void PreferencesGetDefaultData(PreferencesDefinition * pdef) {
    SigmaMemoryClear(pdef, sizeof(PreferencesDefinition));
    sGetDefaultDataForPrefFields(pdef, sgtPrefFields, PREF_FIELD_COUNT(sgtPrefFields));
    pdef->tVideoPrefs.ResetToDefaults();
    pdef->tSoundPrefs.ResetToDefaults();
    pdef->tGameplayPrefs.ResetToDefaults();
    pdef->tKeybindingPrefs.ResetToDefaults();
    pdef->tSocialPrefs.ResetToDefaults();
    pdef->tChatPrefs.ResetToDefaults();
    pdef->tConsoleMultiplayerPrefs.ResetToDefaults();
}

namespace Prefs {

inline uint32 GetCurrentKeyModifierFlags() {
    uint32 nCurrentFlags = 0;
    if (InputAltKeyIsDown(INPUTCONTEXT_DEFAULT)) {
        nCurrentFlags |= 1;
    }
    if (InputCtrlKeyIsDown(INPUTCONTEXT_DEFAULT)) {
        nCurrentFlags |= 2;
    }
    if (InputShiftKeyIsDown(INPUTCONTEXT_DEFAULT)) {
        nCurrentFlags |= 4;
    }
    return nCurrentFlags;
}

// NON_MATCHING: 2 instructions; the branch on the second binding's modifier test has the
// opposite sense (b.ne/b where the original has b.eq/b). Same blocks and targets. The line table puts the no-modifier path (1259) first; that swap, setting fKeyDown instead of returning, and !(mask) forms keep b.ne.
#line 1236
BOOL CheckAction(
    const UIAction eAction,
    BOOL bValidateModifiers)
{


    const ActionBinding * ptBinding;
	if (eAction < NUM_BINDABLE_ACTIONS)
    {
        ptBinding = &GetKeybinding(eAction);
    }
    else
    {
		ptBinding = &cgtDefaultBindings[eAction];
    }


    const InputState & tInputState = InputGetCurrentState(INPUTCONTEXT_DEFAULT);

    BOOL fKeyDown;

    if (!bValidateModifiers)
    {
        fKeyDown = (ptBinding->eKey1 != -1 && tInputState.tKeyboardState.bCurrentKeys[ptBinding->eKey1]) || (ptBinding->eKey2 != -1 && tInputState.tKeyboardState.bCurrentKeys[ptBinding->eKey2]);
    }
    else
    {
        uint32 dwCurrentKeyModifiers = GetCurrentKeyModifierFlags();

        fKeyDown = ptBinding->eKey1 != -1 && tInputState.tKeyboardState.bCurrentKeys[ptBinding->eKey1] &&
            (ptBinding->dwKeyModifierFlags1 & ~dwCurrentKeyModifiers) == 0;




        if (ptBinding->eKey2 != -1 && tInputState.tKeyboardState.bCurrentKeys[ptBinding->eKey2] &&
            (ptBinding->dwKeyModifierFlags2 & ~dwCurrentKeyModifiers) == 0)
        {
            return TRUE;
        }
    }

    if (fKeyDown)
    {
        return TRUE;
    }
    return FALSE;

}
#line 618

BOOL ActionKeyMatch(const int32 nActionIndex, const Key eKey, const BOOL bExactMatch) {
    uint32 dwCurrentKeyModifiers = GetCurrentKeyModifierFlags();
    const ActionBinding * ptBinding = nActionIndex < NUM_BINDABLE_ACTIONS ? &GetKeybinding(nActionIndex) : &cgtDefaultBindings[nActionIndex];
    if (ActionAllowsModifierAsAssignment(nActionIndex)) {
        if (ptBinding->eKey1 == eKey || ptBinding->eKey2 == eKey) {
            return TRUE;
        }
    } else {
        if (ptBinding->eKey1 == eKey) {
            if (bExactMatch) {
                if (ptBinding->dwKeyModifierFlags1 == dwCurrentKeyModifiers) {
                    return TRUE;
                }
            } else if ((ptBinding->dwKeyModifierFlags1 & ~dwCurrentKeyModifiers) == 0) {
                return TRUE;
            }
        }
        if (ptBinding->eKey2 == eKey) {
            if (bExactMatch) {
                if (ptBinding->dwKeyModifierFlags2 == dwCurrentKeyModifiers) {
                    return TRUE;
                }
            } else if ((ptBinding->dwKeyModifierFlags2 & ~dwCurrentKeyModifiers) == 0) {
                return TRUE;
            }
        }
    }
    return FALSE;
}

CRefString GetActionKeyName(const int32 nActionIndex, const BOOL fShortForm, const uint32 nForcedAssignmentIndex) {
    CRefString usKey;
    uint32 dwModifiers = 0;
    Key eKey = -1;
    if (nActionIndex != ACTION_INVALID) {
        GetActionKey(nActionIndex, eKey, dwModifiers, nForcedAssignmentIndex);
    }
    if (eKey == -1) {
        usKey = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_UILAYOUT), "OptionNotSet");
    } else {
        usKey = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_KEYNAMES), SymbolToName(eKey, gKeysTex));
    }
    if (dwModifiers) {
        if (dwModifiers & 4) {
            usKey = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_KEYNAMES), fShortForm ? "MODIFIERSHIFT_SHORT" : "MODIFIERSHIFT") + usKey;
        }
        if (dwModifiers & 1) {
            usKey = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_KEYNAMES), fShortForm ? "MODIFIERALT_SHORT" : "MODIFIERALT") + usKey;
        }
        if (dwModifiers & 2) {
            usKey = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_KEYNAMES), fShortForm ? "MODIFIERCTRL_SHORT" : "MODIFIERCTRL") + usKey;
        }
    }
    return usKey;
}

inline void GetActionKey(const int32 nActionIndex, Key & eKey, uint32 & dwModifiers, uint32 nForcedAssignmentIndex) {
    const ActionBinding * ptBinding = nActionIndex < NUM_BINDABLE_ACTIONS ? &GetKeybinding(nActionIndex) : &cgtDefaultBindings[nActionIndex];
    if (nForcedAssignmentIndex == 1) {
        eKey = ptBinding->eKey1;
        dwModifiers = ptBinding->dwKeyModifierFlags1;
    } else if (nForcedAssignmentIndex == 2) {
        eKey = ptBinding->eKey2;
        dwModifiers = ptBinding->dwKeyModifierFlags2;
    } else if (ptBinding->eKey1 != -1) {
        eKey = ptBinding->eKey1;
        dwModifiers = ptBinding->dwKeyModifierFlags1;
    } else {
        eKey = ptBinding->eKey2;
        dwModifiers = ptBinding->dwKeyModifierFlags2;
    }
}

void ResetToBlizzConDefaults() {
    PreferencesGetDefaultData(&s_tPrefsDef);
    if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT)) {
        SoundPrefs & tSoundPrefs = s_tPrefsDef.tSoundPrefs;
        GameplayPrefs & tGameplayPrefs = s_tPrefsDef.tGameplayPrefs;
        tSoundPrefs.nChannelsToUse = SOUND_CHANNEL_COUNT_MAX;
        tGameplayPrefs.bAutoEquipItems = FALSE;
        tGameplayPrefs.bShowTutorials = FALSE;
        tGameplayPrefs.bShowPlayerNames = FALSE;
        tGameplayPrefs.bElectiveMode = TRUE;
    }
}

}  // namespace Prefs

void VideoPrefs::ResetToDefaults() {
    sGetDefaultDataForPrefFields(this, sgtVideoPrefFields, PREF_FIELD_COUNT(sgtVideoPrefFields));
    tVideoPrefsToConfirm.nShadowQuality = SHADOWPREF_HIGH_SOFT;
    nClutterQuality = CLUTTER_HIGH;
    bLowFX = FALSE;
    bSSAO = TRUE;
    nShadowDetail = SHADOWDETAILPERF_HIGH;
}

void SoundPrefs::ResetToDefaults() {
    sGetDefaultDataForPrefFields(this, sgtSoundPrefFields, PREF_FIELD_COUNT(sgtSoundPrefFields));
    flTypeVolume[SOUNDTYPE_EFFECT_UI] = 1.0f;
    bMuteSoundType[SOUNDTYPE_EFFECT_UI] = FALSE;
    if (UnicodeText::GetCurrentLocale() != UnicodeText::GetStringDisplayLocale()) {
        bQuestSubtitlesEnabled = TRUE;
        bCinematicsSubtitlesEnabled = TRUE;
    }
}

void GameplayPrefs::ResetToDefaults() {
    sGetDefaultDataForPrefFields(this, sgtGameplayPrefFields, PREF_FIELD_COUNT(sgtGameplayPrefFields));
}

void ConsoleMultiplayerPrefs::ResetToDefaults() {
    sGetDefaultDataForPrefFields(this, sgtConsoleMultiplayerPrefFields, PREF_FIELD_COUNT(sgtConsoleMultiplayerPrefFields));
}

void SocialPrefs::ResetToDefaults() {
    sGetDefaultDataForPrefFields(this, sgtSocialPrefFields, PREF_FIELD_COUNT(sgtSocialPrefFields));
}

void ChatPrefs::ResetToDefaults() {
    sGetDefaultDataForPrefFields(this, sgtChatPrefFields, PREF_FIELD_COUNT(sgtChatPrefFields));
}

void KeybindingPrefs::ResetToDefaults() {
    nKeybindingVersion = KEYBINDING_VERSION;
    SigmaMemoryCopy(tBindings, cgtDefaultBindings, sizeof(tBindings));
    XLocale eXLocale = UnicodeText::GetCurrentLocale();
    if (eXLocale == XLOCALE_FRENCH) {
        tBindings[ACTION_SYSTEM_MASTER_VOLUME_DOWN].eKey1 = 155;
        tBindings[ACTION_SYSTEM_MASTER_VOLUME_DOWN].dwKeyModifierFlags1 = 2;
    } else if (eXLocale == XLOCALE_TURKISH) {
        tBindings[ACTION_SYSTEM_MASTER_VOLUME_UP].eKey1 = 85;
        tBindings[ACTION_SYSTEM_MASTER_VOLUME_UP].dwKeyModifierFlags1 = 0;
        tBindings[ACTION_SYSTEM_MASTER_VOLUME_DOWN].eKey1 = 84;
        tBindings[ACTION_SYSTEM_MASTER_VOLUME_DOWN].dwKeyModifierFlags1 = 0;
    }
}

BOOL sSaveAllPrefs(PrefsWriter & tFileRef) {
    BOOL bSuccess = sSavePrefFields(tFileRef, &s_tPrefsDef, sgtPrefFields, PREF_FIELD_COUNT(sgtPrefFields));
    bSuccess = bSuccess && sSavePrefFields(tFileRef, &s_tPrefsDef.tVideoPrefs, sgtVideoPrefFields, PREF_FIELD_COUNT(sgtVideoPrefFields));
    bSuccess = bSuccess && sSavePrefFields(tFileRef, &s_tPrefsDef.tSoundPrefs, sgtSoundPrefFields, PREF_FIELD_COUNT(sgtSoundPrefFields));
    bSuccess = bSuccess && sSavePrefFields(tFileRef, &s_tPrefsDef.tGameplayPrefs, sgtGameplayPrefFields, PREF_FIELD_COUNT(sgtGameplayPrefFields));
    bSuccess = bSuccess && sSavePrefFields(tFileRef, &s_tPrefsDef.tMacPrefs, sgtMacPrefFields, PREF_FIELD_COUNT(sgtMacPrefFields));
    bSuccess = bSuccess && sSavePrefFields(tFileRef, &s_tPrefsDef.tSocialPrefs, sgtSocialPrefFields, PREF_FIELD_COUNT(sgtSocialPrefFields));
    bSuccess = bSuccess && sSavePrefFields(tFileRef, &s_tPrefsDef.tChatPrefs, sgtChatPrefFields, PREF_FIELD_COUNT(sgtChatPrefFields));
    bSuccess = bSuccess && sSavePrefFields(tFileRef, &s_tPrefsDef.tKeybindingPrefs, sgtKeybindingPrefFields, PREF_FIELD_COUNT(sgtKeybindingPrefFields));
    bSuccess = bSuccess && sSavePrefFields(tFileRef, &s_tPrefsDef.tConsoleMultiplayerPrefs, sgtConsoleMultiplayerPrefFields, PREF_FIELD_COUNT(sgtConsoleMultiplayerPrefFields));
    return bSuccess;
}

namespace Prefs {

int32 Save(uint8 * ptBuffer, int32 nBufferSize) {
    PrefsWriter tPrefsWriter;
    tPrefsWriter.m_ptBuffer = ptBuffer;
    tPrefsWriter.m_nBufferSize = nBufferSize;
    tPrefsWriter.m_nCurrentPos = 0;
    BOOL bSuccess = sSaveAllPrefs(tPrefsWriter);
    if (!bSuccess) {
        return -1;
    }
    return tPrefsWriter.m_nCurrentPos;
}

void ParseFileDataToKeyValues(CHAR * sFileData, uint32 uFileSize, KeyValueMap & mapKeyValues) {
    CHAR * sLineStart = sFileData;
    CHAR * sLineEnd = sFileData;
    CHAR * sFileEnd = sFileData + uFileSize;
    while (sLineEnd < sFileEnd) {
        CHAR c = *sLineEnd;
        sLineEnd++;
        if (c == '\n' || sLineEnd == sFileEnd) {
            CRefString sLine(sLineStart, (int32)(sLineEnd - sLineStart));
            char szKeyName[128];
            char szValueName[128];
            if (sscanf(sLine.str(), "%127s \"%127[^\"]\"", szKeyName, szValueName) == 2) {
                szKeyName[127] = 0;
                mapKeyValues.SetAt_Tracked(szKeyName, szValueName);
            } else if (sscanf(sLine.str(), "%127s \"\"", szKeyName) == 1) {
                szKeyName[127] = 0;
                szValueName[127] = 0;
                mapKeyValues.SetAt_Tracked(szKeyName, "");
            }
            sLineStart = sLineEnd;
        }
    }
}

static void sProcessKeyValues(KeyValueMap & mapKeyValues) {
    {
        CRefString sVersionValue;
        int32 nVersionValue;
        if (!mapKeyValues.Lookup(VERSION_KEY, sVersionValue)) {
            return;
        }
        if (sscanf(sVersionValue.str(), "%d", &nVersionValue) != 1) {
            return;
        }
        if (nVersionValue != PREFERENCES_VERSION) {
            return;
        }
    }
    sLoadPrefFields(mapKeyValues, &s_tPrefsDef, sgtPrefFields, PREF_FIELD_COUNT(sgtPrefFields));
    sLoadPrefFields(mapKeyValues, &s_tPrefsDef.tVideoPrefs, sgtVideoPrefFields, PREF_FIELD_COUNT(sgtVideoPrefFields));
    sLoadPrefFields(mapKeyValues, &s_tPrefsDef.tSoundPrefs, sgtSoundPrefFields, PREF_FIELD_COUNT(sgtSoundPrefFields));
    sLoadPrefFields(mapKeyValues, &s_tPrefsDef.tGameplayPrefs, sgtGameplayPrefFields, PREF_FIELD_COUNT(sgtGameplayPrefFields));
    sLoadPrefFields(mapKeyValues, &s_tPrefsDef.tMacPrefs, sgtMacPrefFields, PREF_FIELD_COUNT(sgtMacPrefFields));
    sLoadPrefFields(mapKeyValues, &s_tPrefsDef.tSocialPrefs, sgtSocialPrefFields, PREF_FIELD_COUNT(sgtSocialPrefFields));
    sLoadPrefFields(mapKeyValues, &s_tPrefsDef.tChatPrefs, sgtChatPrefFields, PREF_FIELD_COUNT(sgtChatPrefFields));
    sLoadPrefFields(mapKeyValues, &s_tPrefsDef.tKeybindingPrefs, sgtKeybindingPrefFields, PREF_FIELD_COUNT(sgtKeybindingPrefFields));
    sLoadPrefFields(mapKeyValues, &s_tPrefsDef.tConsoleMultiplayerPrefs, sgtConsoleMultiplayerPrefFields, PREF_FIELD_COUNT(sgtConsoleMultiplayerPrefFields));
    s_tPrefsDef.tVideoPrefs.tVideoPrefsToConfirm.bVsync = TRUE;
    s_tPrefsDef.bSuccessfullyLoadedBNetAccountSettings = FALSE;
    if (CGameIsValid() && UI_TestID(cgUIIDRoot)) {
        UI_GetWindowFromID(cgUIIDRoot).UpdateAbsolutePositions();
        GfxSetPrefsChanged();
    }
}

inline void ResetToDefaults() {
    PreferencesGetDefaultData(&s_tPrefsDef);
    if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT)) {
        ResetToBlizzConDefaults();
    }
}

void Load(const char * szPath) {
    s_bLoaded = TRUE;
    ResetToDefaults();
    FileReference tFileRef;
    FileReferenceInit(tFileRef, szPath ? szPath : sGetPrefsLocation().str());
    BOOL bExists = FileExists(tFileRef);
    if (!bExists) {
        return;
    }
    if (!FileOpen(tFileRef, 1)) {
        return;
    }
    uint32 uFileSize;
    CHAR * sFileData = FileReadIntoMemory(tFileRef, uFileSize, ERROR_NONE);
    if (!sFileData) {
        return;
    }
    FileClose(tFileRef);
    KeyValueMap mapKeyValues(16, NULL);
    ParseFileDataToKeyValues(sFileData, uFileSize, mapKeyValues);
    SigmaMemoryFree(sFileData, NULL);
    sProcessKeyValues(mapKeyValues);
    if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT)) {
        ResetToBlizzConDefaults();
    }
}

// Not in the binary (the linker dropped it), but the DWARF keeps it at line 1981 with these
// parameter and local names; the body is inferred from them. Its call is
// what keeps sProcessKeyValues out of line in Load.
void Parse(char * szFileData, uint32 uFileSize) {
    KeyValueMap mapKeyValues(16, NULL);
    ParseFileDataToKeyValues(szFileData, uFileSize, mapKeyValues);
    sProcessKeyValues(mapKeyValues);
}

}  // namespace Prefs

HardwareClass VideoPrefs::GetHWClass() const {
    return eHWClass;
}

void VideoPrefs::SetGamma(float flGammaValue) {
    flGamma = flGammaValue < GAMMA_MIN ? GAMMA_MIN : (flGammaValue > GAMMA_MAX ? GAMMA_MAX : flGammaValue);
}

int32 VideoPrefs::GetShadowQuality() const {
    return tVideoPrefsToConfirm.nShadowQuality;
}

void VideoPrefs::SetShadowQuality(int32 nQuality) {
    if (nQuality > SHADOWPREF_HIGH_SOFT) {
        nQuality = SHADOWPREF_HIGH_SOFT;
    }
    if (nQuality < SHADOWPREF_NONE) {
        nQuality = SHADOWPREF_NONE;
    }
    tVideoPrefsToConfirm.nShadowQuality = nQuality;
}

int32 VideoPrefs::GetShadowMapSize() const {
    switch (tVideoPrefsToConfirm.nShadowQuality) {
    case SHADOWPREF_MID:
        return 1024;
    case SHADOWPREF_HIGH:
        return 2048;
    case SHADOWPREF_LOW_SOFT:
        return 512;
    case SHADOWPREF_MID_SOFT:
        return 1024;
    case SHADOWPREF_HIGH_SOFT:
        return 2048;
    default:
        return 512;
    }
}

int32 VideoPrefs::GetPhysicsQuality() const {
    return nPhysicsQuality;
}

int32 VideoPrefs::GetReflectionQuality() const {
    return 0;
}

void VideoPrefs::SetReflectionQuality(int32 nQuality) {
    if (nQuality > REFLECTION_QUALITY_MAX) {
        nQuality = REFLECTION_QUALITY_MAX;
    }
    if (nQuality < REFLECTION_QUALITY_MIN) {
        nQuality = REFLECTION_QUALITY_MIN;
    }
    nReflectionQuality = nQuality;
}

int32 VideoPrefs::GetTextureMipOffset() const {
    return tVideoPrefsToConfirm.nMipOffset;
}

float VideoPrefs::GetTextureMipBias() const {
    return flMipBias;
}

BOOL SoundPrefs::QuestSubtitlesEnabled() const {
    return bQuestSubtitlesEnabled;
}

BOOL SoundPrefs::CinematicsSubtitlesEnabled() const {
    return bCinematicsSubtitlesEnabled;
}

BOOL Prefs::ActionAllowsModifierAsAssignment(UIAction eAction) {
    switch (eAction) {
    case ACTION_UI_TOGGLE_PLAYER_HP_BARS:
    case ACTION_UI_TOGGLE_DAMAGE_NUMBERS:
    case ACTION_UI_TOGGLE_ITEMS_ON_GROUND:
    case ACTION_UI_HOLD_POSITION:
    case ACTION_UI_PASTE_ITEM:
    case ACTION_UI_SECONDARY_TIPS:
    case ACTION_UI_QUICK_INVENTORY_KEY:
        return TRUE;
    }
    return FALSE;
}

// The setters that dirty the account prefs share one shape; the macro name is invented.
#define GAMEPLAY_SET_DIRTY(member, value) \
    if (member != value) {                \
        s_bAccountPrefsDirty = TRUE;      \
    }                                     \
    member = value;

void GameplayPrefs::SetShowTutorials(BOOL bShow) {
    GAMEPLAY_SET_DIRTY(bShowTutorials, bShow);
}

void GameplayPrefs::SetShowMonsterHPBars(BOOL bShow) {
    GAMEPLAY_SET_DIRTY(bShowMonsterHPBars, bShow);
}

void GameplayPrefs::SetShowPlayerHPBars(BOOL bShow) {
    GAMEPLAY_SET_DIRTY(bShowPlayerHPBars, bShow);
}

void GameplayPrefs::SetShowDefensiveMessages(BOOL bShow) {
    GAMEPLAY_SET_DIRTY(bShowDefensiveMessages, bShow);
}

void GameplayPrefs::SetShowCriticals(BOOL bShow) {
    GAMEPLAY_SET_DIRTY(bShowCriticals, bShow);
}

void GameplayPrefs::SetShowDamageNumbers(BOOL bShow) {
    GAMEPLAY_SET_DIRTY(bShowDamageNumbers, bShow);
}

void GameplayPrefs::SetShowHealNumbers(BOOL bShow) {
    GAMEPLAY_SET_DIRTY(bShowHealNumbers, bShow);
}

void GameplayPrefs::SetShowPlayerNames(BOOL bShow) {
    GAMEPLAY_SET_DIRTY(bShowPlayerNames, bShow);
}

void GameplayPrefs::SetShowItemsOnGround(BOOL bShow) {
    GAMEPLAY_SET_DIRTY(bShowItemsOnGround, bShow);
}

void GameplayPrefs::SetUseElectiveMode(BOOL bShow) {
    GAMEPLAY_SET_DIRTY(bElectiveMode, bShow);
}

void GameplayPrefs::SetAutoEquipItems(BOOL bShow) {
    GAMEPLAY_SET_DIRTY(bAutoEquipItems, bShow);
}

void GameplayPrefs::SetShowItemToolTipOnDrop(BOOL bShow) {
    GAMEPLAY_SET_DIRTY(bShowItemTooltipOnDrop, bShow);
}

void GameplayPrefs::SetShowAdvancedTooltips(BOOL bShow) {
    GAMEPLAY_SET_DIRTY(bShowAdvancedTooltips, bShow);
}

BOOL GameplayPrefs::ShowAdvancedTooltips() const {
    return bShowAdvancedTooltips || InputCtrlKeyIsDown(INPUTCONTEXT_DEFAULT);
}

BOOL GameplayPrefs::GetEnableSmartCoopMinimap() const {
    return bEnableSmartCoopMinimap;
}

void GameplayPrefs::SetEnableSmartCoopMinimap(BOOL bEnable) {
    bEnableSmartCoopMinimap = bEnable;
}

BOOL GameplayPrefs::GetAlwaysShowCampaignObjectives() const {
    return bAlwaysShowCampaignObjectives;
}

void GameplayPrefs::SetAlwaysShowCampaignObjectives(BOOL bEnable) {
    bAlwaysShowCampaignObjectives = bEnable;
}

BOOL GameplayPrefs::GetAlwaysShowAdventureObjectives() const {
    return bAlwaysShowAdventureObjectives;
}

void GameplayPrefs::SetAlwaysShowAdventureObjectives(BOOL bEnable) {
    bAlwaysShowAdventureObjectives = bEnable;
}

XLocale GameplayPrefs::GetSelectedTextLocale() const {
    return nSelectedTextLocale;
}

void GameplayPrefs::SetSelectedTextLocale(XLocale nLocale) {
    nSelectedTextLocale = nLocale;
}

XLocale GameplayPrefs::GetSelectedAudioLocale() const {
    return nSelectedAudioLocale;
}

void GameplayPrefs::SetSelectedAudioLocale(XLocale nLocale) {
    nSelectedAudioLocale = nLocale;
}

BOOL GameplayPrefs::GetDisableCutscenes() const {
    if (GameIsOpenWorld() || GameIsWeeklyChallenge()) {
        return TRUE;
    }
    return bDisableCutscenes;
}

BOOL GameplayPrefs::GetItemRarityIcons() const {
    return bItemRarityIcons;
}

void GameplayPrefs::SetItemRarityIcons(BOOL bVal) {
    GAMEPLAY_SET_DIRTY(bItemRarityIcons, bVal);
}

BOOL GameplayPrefs::GetDisableShortFloatingNumbers() const {
    return bDisableShortFloatingNumbers;
}

void GameplayPrefs::SetDisableShortFloatingNumbers(BOOL bVal) {
    GAMEPLAY_SET_DIRTY(bDisableShortFloatingNumbers, bVal);
}

BOOL VideoPrefs::GetUseLetterBox() const {
    return bLetterbox;
}

float VideoPrefs::GetGamma() const {
    return flGamma;
}

float VideoPrefs::GetGammaMin() const {
    return GAMMA_MIN;
}

float VideoPrefs::GetGammaMax() const {
    return GAMMA_MAX;
}

float VideoPrefs::GetGammaDefault() const {
    return GAMMA_DEFAULT;
}

void VideoPrefs::GetSafeZoneDefualtValues(float & _flSafeZoneLeft, float & _flSafeZoneTop, float & _flSafeZoneRight, float & _flSafeZoneBottom) const {
    _flSafeZoneLeft = SAFEZONE_DEFAULT_LEFT;
    _flSafeZoneTop = SAFEZONE_DEFAULT_TOP;
    _flSafeZoneRight = SAFEZONE_DEFAULT_RIGHT;
    _flSafeZoneBottom = SAFEZONE_DEFAULT_BOTTOM;
}

int32 VideoPrefs::GetForegroundFPSLimit() const {
    return nMaxForegroundFPS;
}

uint8 * Prefs::GetPlayedCutScenes(uint32 & nDataLen) {
    nDataLen = sizeof(s_tPrefsDef.bPlayedCutscene);
    return Get()->bPlayedCutscene;
}

void VideoPrefs::SetUseAntialiasing(BOOL bUse) {
    bAntialiasing = bUse;
}

BOOL VideoPrefs::GetDisableScreenShake() const {
    return bDisableScreenShake;
}

BOOL VideoPrefs::GetDisableChromaEffects() const {
    return bDisableChromaEffects;
}

BOOL VideoPrefs::GetLimitForegroundFPS() const {
    return bLimitForegroundFPS;
}

int32 VideoPrefs::GetClutterQuality() const {
    return nClutterQuality;
}

BOOL VideoPrefs::GetUseLowFXMode() const {
    return bLowFX;
}

void VideoPrefs::SetUseLowFXMode(BOOL bUseLowFX) {
    bLowFX = bUseLowFX;
}

BOOL VideoPrefs::GetUseSSAO() const {
    return bSSAO;
}

int32 SoundPrefs::GetChannelsToUse() const {
    return SOUND_CHANNEL_COUNT_MAX;
}

uint32 SoundPrefs::GetDriverHash() const {
    return dwDriverHash;
}

SpeakerModes SoundPrefs::GetSpeakerMode() const {
    return eSpeakerMode;
}

BOOL VideoPrefs::GetDisableTrilinearFiltering() const {
    return bDisableTrilinearFiltering;
}

BOOL VideoPrefs::GetUseColorCorrection() const {
    return bColorCorrection;
}

int32 ConsoleMultiplayerPrefs::GetMatchmakeRegion() {
    return nMatchmakeRegion;
}

void ConsoleMultiplayerPrefs::SetMatchmakeRegion(const int32 nSetting) {
    nMatchmakeRegion = nSetting;
}

BOOL ConsoleMultiplayerPrefs::GetMatchmakeRegionStrict() {
    return bMatchmakeRegionStrict;
}

void ConsoleMultiplayerPrefs::SetMatchmakeRegionStrict(BOOL bSetting) {
    bMatchmakeRegionStrict = bSetting;
}

BOOL ConsoleMultiplayerPrefs::GetMatchmakeLanguageStrict() {
    return bMatchmakeLanguageStrict;
}

void ConsoleMultiplayerPrefs::SetMatchmakeLanguageStrict(BOOL bSetting) {
    bMatchmakeLanguageStrict = bSetting;
}

// The key codes are #defines in the original; the DWARF has no names for them.
BOOL Prefs::KeyIsModifier(Key eKey) {
    switch (eKey) {
    case 28:
    case 41:
    case 53:
    case 55:
    case 106:
    case 117:
        return TRUE;
    }
    return FALSE;
}

UIAction Prefs::GetUIActionFromInput(const BufferedInput & tInput) {
    for (int32 i = ACTION_UI_SKILL_1; i <= ACTION_UI_QUICK_INVENTORY_KEY; i++) {
        if ((InputIsKeyboard(tInput) && ActionKeyMatch(i, tInput.tBufKeyData.eKey, tInput.tBufKeyData.bExactMatch)) ||
            (InputIsController(tInput) && Controller::ActionButtonMatch(i, tInput))) {
            return i;
        }
    }
    if (InputIsController(tInput)) {
        if (Controller::ActionButtonMatch(ACTION_UI_LEFT_MOUSE_BUTTON, tInput)) {
            return ACTION_UI_LEFT_MOUSE_BUTTON;
        }
        if (Controller::ActionButtonMatch(ACTION_UI_RIGHT_MOUSE_BUTTON, tInput)) {
            return ACTION_UI_RIGHT_MOUSE_BUTTON;
        }
        if (Controller::ActionButtonMatch(ACTION_CONSOLE_EVASION, tInput)) {
            return ACTION_CONSOLE_EVASION;
        }
    }
    if (KeyIsModifier(tInput.tBufKeyData.eKey)) {
        const InputState & tInputState = InputGetCurrentState(INPUTCONTEXT_DEFAULT);
        uint32 dwCurrentKeyModifiers = GetCurrentKeyModifierFlags();
        for (int32 nActionIndex = ACTION_UI_SKILL_1; nActionIndex < ACTION_UI_QUICK_INVENTORY_KEY; nActionIndex++) {
            const ActionBinding * ptBinding = nActionIndex < NUM_BINDABLE_ACTIONS ? &GetKeybinding(nActionIndex) : &cgtDefaultBindings[nActionIndex];
            if ((ptBinding->eKey1 != -1 && tInputState.tKeyboardState.bCurrentKeys[ptBinding->eKey1] && dwCurrentKeyModifiers == ptBinding->dwKeyModifierFlags1) ||
                (ptBinding->eKey2 != -1 && tInputState.tKeyboardState.bCurrentKeys[ptBinding->eKey2] && dwCurrentKeyModifiers == ptBinding->dwKeyModifierFlags2)) {
                return nActionIndex;
            }
        }
    }
    return ACTION_INVALID;
}

// LINKER-REMOVED: not in the binary (DWARF low_pc 0, 0x40 bytes, line 3333). Without its store
// GlobalOpt folds s_bBackupValid to FALSE and Restore loses its body.
void Prefs::Backup() {
    s_tPrefsBackup = s_tPrefsDef;
    s_bBackupAccountPrefsDirty = s_bAccountPrefsDirty;
    s_bBackupValid = TRUE;
}

void Prefs::Restore() {
    if (s_bBackupValid == TRUE) {
        s_tPrefsDef = s_tPrefsBackup;
        s_bAccountPrefsDirty = s_bBackupAccountPrefsDirty;
    }
}
