#include "UIGame.h"
#include "../../CGame.h"
#include "../../CPlayer.h"
#include "../UI.h"
#include "../UIID.h"
#include "../Controls/UIWindowControl.h"
#include "../../Definitions/PreferencesDefinition.h"
#include "../../../Common/CommonTypes.h"
#include "../../../Common/GameCommonData.h"
#include "../../../Common/Definitions/StringListDefinition.h"
#include "../../../Common/UserInterface/UITextControl.h"
#include "../../../Sigma/DataStructures/XGrowableList.h"
#include "../../Battlenet/Subsystems/FanfareManager.h"
#include "../../EffectGroup.h"
#include "../UIUtils.h"
#include "../Controls/UIHotKeyControl.h"
#include "../Controls/UITextControl.h"
#include "UIConfirmation.h"
#include "../../../Common/Definitions/GameBalanceDefinition.h"
#include "../../../Common/TagMaps/ActorTags.h"
#include "../../../Sigma/LanguageRules.h"
#include "../../../Common/ActorInventory.h"
#include "../../../Common/Game.h"
#include "../UIStoryboard.h"
#include "../../Camera/Camera.h"
#include "../../Camera/CameraLocation.h"
#include "../../CWorld.h"
#include "../../Gfx/Gfx.h"
#include "../../../Common/GameConstants.h"
#include "../Controls/UIActorTagControl.h"
#include "../Controls/UIButtonControl.h"
#include "../../Camera/CameraManager.h"
#include "../../RActors/RActors.h"
#include "../../../Common/Definitions/MonsterDefinition.h"
#include "../Controls/UIRActorControl.h"
#include "../Controls/UIProgressBarControl.h"
#include "../../../Common/HotbarButton.h"
#include "../../../Common/Items.h"
#include "../../Sound/Sound.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/Player.h"
#include "../../../Common/ActorCommonData.h"
#include "../../../Sigma/TypeDescriptor/SymbolicValue.h"
#include "../../../Sigma/UnicodeText.h"
#include "../../Network/CTrickle.h"
// Declared here because their headers are not adopted yet. Each comment names the file that
// defines the function or object.













// X/Client/UserInterface/UI.cpp
UIStoryboard & UI_GetStoryboardFromID(const UIID & id);
// X/Client/RActors/RActors.cpp:10610
BOOL RActorGetTooltip(const RActor * ptRActor, CRefString & usText, Player *& ptBannerPlayer);
UIControl & UI_GetControlFromID(const UIID & id);
// X/Client/UserInterface/UIUtils.cpp
void UI_ConstrainRectToViewport(Rect2D & rect, float flIndent);
void ConvertUICDimensionsToPixels(const Vector2D & vecUIC, IVector2D & vecPixels);
void ConvertPixelsDimensionsToUIC(const IVector2D & vecPixels, Vector2D & vecUIC);
UIActorTag & UI_GetActorTagFromID(const UIID & id);
void UI_DeleteControl(UIID & idControl, const BOOL fAssertIfDeletingRoot);
void UI_Print(LPCSTR szText, BOOL bChat);
// X/Client/Network/CGameMessage.cpp
void MessageSendToServer(GameMessageType eType, void * pMessage, uint32 dwMessageSize, const Player * ptPlayer);
#include "../../../Common/Networking/GameMessage.h"
#include "../Controls/UIHotbarButtonControl.h"
// X/Client/UserInterface/UI.cpp:5411
UIHotbarButton & UI_GetHotbarButtonFromID(const UIID & id);
#include "../../Gfx/GfxVB.h"  // header statics run by this file's static initializer (RGBAColor.h:40 normals)
// X/Client/UserInterface/UI.cpp
UIRActor & UI_GetRActorFromID(const UIID & id);
// X/Client/UserInterface/UI.cpp:1137
UIHotkey & UI_GetHotkeyFromID(const UIID & id);
// X/Client/UserInterface/UIGlobals.cpp
extern UIID UIDLG_NOTIFY_MESSAGE;
extern UIID UIDLG_ITEMTAGS;
extern UIID UITEXT_NOTIFY_MESSAGE_TITLE;
extern UIID UITEXT_NOTIFY_MESSAGE_TEXT;
extern UIID UIHOTKEY_NOTIFY_MESSAGE;
extern UIID UIWND_DUNGEON_FINDER_CONTAINER;
extern UIID UIWND_SAVEPOINTSPLASH;
extern UIID UISTORYBOARD_SAVEPOINTSPLASH;
extern UIID UIWND_TIERED_RIFT_CONTAINER;
extern UIID UIWND_CHALLENGE_RIFT_CONTAINER;
extern UIID UIWND_TIERED_RIFT_BONUS_OBJECTIVE_WRAPPER;
extern UIID UIWND_WEEKLY_CHALLENGE_BONUS_OBJECTIVE_WRAPPER;
extern UIID UIWND_WEEKLY_CHALLENGE_SECONDARY_BONUS_OBJECTIVE_WRAPPER;
extern UIID UIPROGRESS_TIERED_RIFT_PROGRESSBAR;
extern UIID UIWND_TIERED_RIFT_TIMER_PROGRESSBAR;
// X/Client/UserInterface/Interface/UIBuffs.cpp:1656
namespace UIBuffs { bool BuffShowsActiveOnSkillButtons(SNO snoPower, const Player * ptPlayer); }
// X/Client/UserInterface/Console/UIConsoleHUD.cpp
namespace UIHUD { namespace Console { int32 PlayerGetHUDSlot(const Player * ptPlayer); } }
namespace UIHUD { namespace Console { void ApplyFreeCastEffects(const Player * ptPlayer, const int32 nSkillButtonIndex); } }
namespace UIHUD { namespace Console { void UpdatePlayerTargetingIndicator(const Player * ptPlayer, const RActor * ptRActor, BOOL bApply); } }
void RActorHighlight(RActor & tRActor, const BOOL fHighlight);
SNO GetTargetLockSNOForPlayer(const Player * ptPlayer);
namespace UINPC { namespace Console { BOOL CachedInteractMenuIsActive(); } }
class UIRActor;
void sSetRActorTooltip(const RActor * ptRActor, const Player * ptPlayer, UIWindow & tWindow, UIText & tText, UIRActor & tUIRActor, EffectGroupID & idEffectGroup);
// X/Common/Player.cpp:2589 and :2600
ACDID PlayerGetFirstPet(ACDID idACDPlayer, PetType eType, int32 & nPos);
ACDID PlayerGetNextPet(ACDID idACDPlayer, PetType eType, int32 & nPos);
struct NotificationData;
void sConsoleProcessPlayerNotification(const NotificationData & tData);
static void sUpdatedCachedHPBarForACD(int32 & nIndex, ActorCommonData * ptACD, BOOL fShowName, BOOL fShowHPBar);
// X/Client/CPlayer.cpp:5216
BOOL PlayerIsLocal(const ACDID idACD);
// X/Client/CPlayerInput.cpp
void PlayerSendPotionSlotChangeToServer(const HotbarButtonData & tButtonData, Player * ptPlayer);
// X/Client/CPlayer.cpp:3307
void CPlayerSayError(uint32 dwSoundTag, RActorID idRActor, BOOL bIgnoreCooldown);
// X/Client/UserInterface/Interface/UITutorials.h:34 (only what this file calls)
class UITutorial;
#include "UITutorials.h"
// X/Common/UserInterface/UISkillsCommon.cpp:21
namespace UISkills { BOOL GetName(SNO snoPower, CRefString & sSkillName); }
#include "../../Battlenet/UI/Util/UIUtil.h"
#include "../../../Common/Quests/Quests.h"
extern UITutorialManager gtTutorialMgr;
// X/Common/Tutorial.cpp
namespace Tutorial { BOOL PlayerHasSeen(const Player & tPlayer, const SNO snoTutorial); }
// X/Client/UserInterface/Interface/UIInventory.cpp
namespace UIInventory {
void Show(const BOOL fShow);
void Display(const float flDeltaTime);
BOOL IsOpen();
}  // namespace UIInventory
// X/Common/Player.cpp
BOOL PlayerCanOpenInventory(const ActorCommonData & tACDPlayer);
// X/Common/Player.h:280 and :318
#include "../../../Common/Definitions/LevelAreaDefinition.h"
struct ActiveEvent; ActiveEvent * CTimedEventsGetActiveEvent(const SNO snoTimedEvent); // CTimedEvents.cpp:101
BOOL ACDIsInChallengeRift(const ActorCommonData & tACD); // ActorCommonData.cpp:9299
BOOL QuestIsComplete(const QuestInstance & tQuest); // Quests.cpp:995
SNO WorldGetLevelAreaSNOFromRActor(RActorID idRActor); // CWorld.cpp:1075
extern UIID UIDLG_CHALLENGERIFT_TARGET_TIME, UITEXT_CHALLENGERIFT_TARGET_TIME, UITEXT_CHALLENGERIFT_TARGET_TIME_PLAYER_TO_BEAT; // UIGlobals.cpp
// this file
namespace UIGame { void DisplayError(const GameErrorData & tData, BOOL fDisplayBnetError, const Player * ptPlayer); }
namespace UIFloatingMessage { void Show(LPCSTR uszMessage, uint32 dwDuration); }
// X/Common/GameErrors.cpp (the symbol table for GameError)
extern const SymbolicEntry gGameErrors[];
namespace Console { namespace Screens { BOOL IsScreenSetOpen(); } }  // ConsoleScreenManager.cpp:935
namespace UIEventQueue { namespace Console { void DisplayLongMessageForPlayer(const CRefString & sMessage, const Player * ptPlayer, const ImageTextureFrame & tImage, BOOL bUseLargeIcon); } }  // UIConsoleEventQueue.cpp
#include "../../../../NintendoSDK/Include/nn/ldn/ldn_Types.h"  // header statics run by this file's static initializer (ldn_Ipv4Address.h:153-163, ldn_MacAddress.h:101-106)
#include "../UIIDs.h"


namespace UISeasonStash { namespace Console { void DisposeForGame(); } }  // X/Client/UserInterface/Console/UIConsoleSeasonStash.cpp:1189
namespace UIEquipmentManagerDetails { void DisposeForGame(); }  // X/Client/UserInterface/Console/UIConsoleEquipmentManagerDetails.cpp:1601
namespace UIEquipmentManager { namespace Console { void DisposeForGame(); } }  // X/Client/UserInterface/Console/UIConsoleEquipmentManager.cpp:197
namespace UIRiftComplete { void DisposeForGame(); }  // X/Client/UserInterface/Console/UIConsoleRiftComplete.cpp:127
namespace UIJoinRift { void DisposeForGame(); }  // X/Client/UserInterface/Console/UIConsoleJoinRift.cpp:180
namespace UIConsoleRiftEntry { void DisposeForGame(); }  // X/Client/UserInterface/Console/UIConsoleRiftEntry.cpp:529
namespace UIGameSettings { namespace Console { void DisposeForGame(); } }  // X/Client/UserInterface/Console/UIConsoleGameSettings.cpp:720
namespace UIFriendsList { namespace Console { void DisposeForGame(); } }  // X/Client/UserInterface/Console/UIConsoleFriendsList.cpp:166
namespace UIMail { namespace Console { void DisposeForGame(); } }  // X/Client/UserInterface/Console/UIConsoleMail.cpp:3883
namespace UIParagonBonuses { namespace Console { void DisposeForGame(); } }  // X/Client/UserInterface/Console/UIConsoleParagon.cpp:1263
namespace UIHUD { namespace Console { void DisposeForGame(); } }  // X/Client/UserInterface/Console/UIConsoleHUD.cpp:3009
namespace UICharacterDetails { namespace Console { void DisposeForGame(); } }  // X/Client/UserInterface/Console/UIConsoleCharacterDetails.cpp:1137
namespace UIFastEquip { namespace Console { void DisposeForGame(); } }  // X/Client/UserInterface/Console/UIConsoleFastEquip.cpp:185
namespace UIEventQueue { namespace Console { void DisposeForGame(); } }  // X/Client/UserInterface/Console/UIConsoleEventQueue.cpp:183
namespace UILore { namespace Console { void DisposeForGame(); } }  // X/Client/UserInterface/Console/UIConsoleLore.cpp:781
namespace UIParty { namespace Console { void DisposeForGame(); } }  // X/Client/UserInterface/Console/UIConsolePartyTab.cpp:2065
namespace UIFollower { namespace Console { void DisposeForGame(); } }  // X/Client/UserInterface/Console/UIConsoleFollower.cpp:1835
namespace UISkills { namespace Console { void DisposeForGame(); } }  // X/Client/UserInterface/Console/UISkillsConsole.cpp:3130
namespace UIStash { namespace Console { void DisposeForGame(); } }  // X/Client/UserInterface/Console/UIConsoleStash.cpp:1599
namespace UIMystic { namespace Console { void DisposeForGame(); } }  // X/Client/UserInterface/Console/UIConsoleMystic.cpp:2774
namespace UIBlacksmith { namespace Console { void DisposeForGame(); } }  // X/Client/UserInterface/Console/UIConsoleBlacksmith.cpp:2809
namespace UIJeweler { namespace Console { void DisposeForGame(); } }  // X/Client/UserInterface/Console/UIConsoleJeweler.cpp:2319
namespace UIBloodShardVendor { namespace Console { void DisposeForGame(); } }  // X/Client/UserInterface/Console/UIConsoleBloodShardVendor.cpp:1384
namespace UIShop { namespace Console { void DisposeForGame(); } }  // X/Client/UserInterface/Console/UIShopConsole.cpp:2073
namespace UIInventory { namespace Console { void DisposeForGame(); } }  // X/Client/UserInterface/Console/UIInventoryConsole.cpp:2928
namespace UIPause { namespace Console { void DisposeForGame(); } }  // X/Client/UserInterface/Console/UIConsolePause.cpp:1660
namespace UIJewelUpgrade { void DisposeForGame(); }  // X/Client/UserInterface/Console/UIConsoleJewelUpgrade.cpp:346
namespace UIEquipLegendaryPowers { void DisposeForGame(); }  // X/Client/UserInterface/Console/UIConsoleEquipLegendaryPowers.cpp:570
namespace UITransmute { void DisposeForGame(); }  // X/Client/UserInterface/Interface/UITransmuteCommon.cpp:316
namespace UIGoldCounter { void DisposeForGame(); }  // X/Client/UserInterface/Interface/UIGoldCounter.cpp:58
namespace UIFollowerSwap { void DisposeForGame(); }  // X/Client/UserInterface/Interface/UIFollowerSwap.cpp:61
namespace UICallouts { void DisposeForGame(); }  // X/Client/UserInterface/Interface/UICallouts.cpp:824
namespace UIBuffs { void DisposeForGame(); }  // X/Client/UserInterface/Interface/UIBuffs.cpp:115
namespace UIChatBalloons { void DisposeForGame(); }  // X/Client/UserInterface/Interface/UIChatBalloons.cpp:104
namespace UIBlizzconEnd { void DisposeForGame(); }  // X/Client/UserInterface/Interface/UIBlizzconEnd.cpp:69
static const uint32 PROGRESSION_PERCENT_DISPLAY_SPRINT_TIME = 10; // DWARF UIGame.cpp:194
#include "../../../Common/Shell/CmdLine.h"
// X/Client/UserInterface/Interface/UILoreCommon.cpp:109
void UILoreCommon_DisposeForGame();
#include "../../../Common/Powers/PowersCommon.h"
#include "../../../Common/GameSwig.h"
// X/Client/ImageCollection.cpp:107
ImageHandle StringToImageHandle(LPCSTR pszName);
// X/Client/UserInterface/Interface/UIGame.cpp:203; original initializer supplies these fade durations.
static const FadeRamp scgtErrorTextFadeRamp(0, 240, 300);
// X/Client/UserInterface/Console/UIConsoleEventQueue.cpp:1055
namespace UIEventQueue { namespace Console { void DisplayMessageForPlayer(const CRefString & sMessage, const Player * ptPlayer, const ImageHandle hImage, CRefString * psMessageLine2, BOOL bUseLargeIcon); } }
// X/Client/UserInterface/Console/UIConsoleHUD.cpp:523
namespace UIHUD { namespace Console { UIID PlayerGetResourceUIControl(const Player * ptPlayer); } }
extern UIID UIWND_CONSOLE_HUD_PLAYER_SLOTS;  // X/Client/UserInterface/UIGlobals.cpp
static const FadeRamp scgtGameTextFadeRamp(0, 360, 480);
#include "../../../Sigma/StringManip.h"

CRefString PowerGetRuneStoneTextureFrame(const RuneType eRuneType); // PowersCommon.cpp:5919


static const FadeRamp scgtActTitleFadeRamp(30, 180, 240);
namespace UIPortraits { ImageTextureFrame GetPortraitBackgroundImage(GBID gbidPortrait); ImageTextureFrame GetPortraitBannerImage(GBID gbidPortrait); ImageTextureFrame GetPortraitFrameImage(GBID gbidPortrait); } // UIPortraits.cpp:3478/3522/3544
BOOL UI_DrawingFullscreenUI(); // X/Client/UserInterface/UI.cpp:6519

void CPlayerGetPortraitImage(const Player * ptPlayer, ImageTextureFrame & tFrame, BOOL bUseReversedPortrait); // CPlayer.cpp:1576
GBID PlayerGetPortraitGBIDFromParagonLevel(int32 nLevel); // Player.cpp:5467
static const FadeRamp scgtChallengeRiftTargetTimeFadeRamp(0, 240, 300);
namespace Tutorial { void Shown(Player & tPlayer, const SNO snoTutorial); } // Tutorials.cpp:108
void ACDInventoryGetItemDrawInfoWithGBID(const GBID gbidItem, ImageTextureFrame & tImageTexFrame, const ItemDrawType eDrawType); // CActorInventory.cpp:153
CRefString PowerGetRuneName(SNO snoPower, RuneType eRuneType); // PowersCommon.cpp:2431
extern UIID UITEXT_GAMETEXT_LINE0, UITEXT_GAMETEXT_LINE1, UITEXT_GAMEERROR, UITEXT_BNET_GAMEERROR, UIWND_GAMEERROR_BKGRND;
extern UIID UIDLG_ACT, UITEXT_ACT_TITLE, UITEXT_ACT_SUBTITLE; // X/Client/UserInterface/UIGlobals.cpp
#include "../Console/ConsoleScreenManager.h"
namespace UIFloatingMessage { void Update(); }
// Indexed by GameError; values copied from the binary (tools/data.py sarnErrorSoundTag).
static uint32 sarnErrorSoundTag[] = {
#include "../../../Common/generated-game-data/error_sounds.inc"
#line 250
};

extern UIID UIHOTBARBTN_LEVELUP_NEWSKILL_ICON; // UIGlobals.cpp
extern UIID UIHOTBARBTN_LEVELUP_NEWTRAIT_ICON; // UIGlobals.cpp
extern UIID UISTORYBOARD_LEVELUPSPLASH; // UIGlobals.cpp
extern UIID UISTORYBOARD_LEVELUP_NEWRUNE; // UIGlobals.cpp
extern UIID UISTORYBOARD_LEVELUP_NEWSKILL; // UIGlobals.cpp
extern UIID UISTORYBOARD_LEVELUP_NEWSKILLSLOT; // UIGlobals.cpp
extern UIID UISTORYBOARD_LEVELUP_NEWTRAIT; // UIGlobals.cpp
extern UIID UISTORYBOARD_LEVELUP_NEWTRAITSLOT; // UIGlobals.cpp
extern UIID UISTORYBOARD_POWER_EXTRACTION_REWARD; // UIGlobals.cpp
extern UIID UISTORYBOARD_TRANSMOG_REWARD; // UIGlobals.cpp
extern UIID UITEXT_ALTLEVELUPSPLASH; // UIGlobals.cpp
extern UIID UITEXT_ALTLEVELUP_REWARDS; // UIGlobals.cpp
extern UIID UITEXT_LEVELUPSPLASH; // UIGlobals.cpp
extern UIID UITEXT_LEVELUP_NEWRUNE; // UIGlobals.cpp
extern UIID UITEXT_LEVELUP_NEWRUNE_TITLE; // UIGlobals.cpp
extern UIID UITEXT_LEVELUP_NEWSKILL; // UIGlobals.cpp
extern UIID UITEXT_LEVELUP_NEWTRAIT; // UIGlobals.cpp
extern UIID UITEXT_POWER_EXTRACTION_REWARD_NAME; // UIGlobals.cpp
extern UIID UITEXT_TRANSMOG_REWARD_NAME; // UIGlobals.cpp
extern UIID UIWND_ALTLEVELUPSPLASH; // UIGlobals.cpp
extern UIID UIWND_ALTLEVELUP_NEWPORTRAIT; // UIGlobals.cpp
extern UIID UIWND_ALTLEVELUP_NEWPORTRAIT_BANNER; // UIGlobals.cpp
extern UIID UIWND_ALTLEVELUP_NEWPORTRAIT_BKGRND; // UIGlobals.cpp
extern UIID UIWND_ALTLEVELUP_NEWPORTRAIT_FRAME; // UIGlobals.cpp
extern UIID UIWND_ALTLEVELUP_NEWPORTRAIT_PORTRAIT; // UIGlobals.cpp
extern UIID UIWND_ALTLEVELUP_REWARDS; // UIGlobals.cpp
extern UIID UIWND_LEVELUPSPLASH; // UIGlobals.cpp
extern UIID UIWND_LEVELUP_NEWRUNE; // UIGlobals.cpp
extern UIID UIWND_LEVELUP_NEWRUNE_ICON; // UIGlobals.cpp
extern UIID UIWND_LEVELUP_NEWSKILL; // UIGlobals.cpp
extern UIID UIWND_LEVELUP_NEWSKILLSLOT; // UIGlobals.cpp
extern UIID UIWND_LEVELUP_NEWTRAIT; // UIGlobals.cpp
extern UIID UIWND_LEVELUP_NEWTRAITSLOT; // UIGlobals.cpp
extern UIID UIWND_POWER_EXTRACTION_REWARD; // UIGlobals.cpp
extern UIID UIWND_POWER_EXTRACTION_REWARD_ICON; // UIGlobals.cpp
extern UIID UIWND_POWER_EXTRACTION_REWARD_ICON_SMALL; // UIGlobals.cpp
extern UIID UIWND_TRANSMOG_REWARD; // UIGlobals.cpp
extern UIID UIWND_TRANSMOG_REWARD_ICON; // UIGlobals.cpp
#include "../../../Common/Definitions/GlobalDefinition.h"
#include "../../../Common/TimeConstants.h"
int32 CTimedEventGetRemainingTime(const SNO snoTimedEvent); // CTimedEvents.cpp:137
uint32 CTimedEventGetDurationTicks(const SNO snoTimedEvent); // CTimedEvents.cpp:173
namespace UICountDown { BOOL IsVisible(); void Close(); } // UICountDown.cpp:87/52
namespace UICountDown { void Show(uint32 dwDurationTicks, LPCSTR szMessage, UIFunctionPtr pfnOnExpire, UIFunctionPtr pfnOnCanceled, BOOL bCancelOnMovement, BOOL bPositionHigh, SNO snoTimedEvent); } // UICountDown.cpp:125
namespace UICountDown { void Show(float flDuration, LPCSTR szMessage, UIFunctionPtr pfnOnExpire, UIFunctionPtr pfnOnCanceled, BOOL bCancelOnMovement, BOOL bPositionHigh, SNO snoTimedEvent); } // UICountDown.cpp:182
static void sAddFXToProgressBar(const UIID & idProgressBar, const SNO snoParticle); // UIGame.cpp:7298
static void sPlayRiftFX(const SNO snoParticle, const Vector2D vecUIC); // UIGame.cpp:7281
extern UIID UITEXT_DUNGEON_FINDER_PROGRESS_VALUE, UIWND_DUNGEON_FINDER_WRAPPER, UIWND_TIERED_RIFT_FLASH; // UIGlobals.cpp
namespace UIEquipmentManager { void DisposeForGame(); }  // X/Client/UserInterface/Interface/UIEquipmentManagerCommon.cpp:298
void RActorChangeLook(const RActorID idRActor, LPCSTR szLookName); // RActors.cpp:3168
static_assert(sizeof(UIGlobals) == 0xd58, "UIGlobals"); static_assert(offsetof(UIGlobals, dwRiftClosingTick) == 0xd50, "UIGlobals::dwRiftClosingTick");
static BOOL sIsInTieredRift(); // UIGame.cpp:5759
extern UIID UIPROGRESS_CHALLENGE_RIFT_PROGRESSBAR, UIWND_CHALLENGE_RIFT_TIMER_PROGRESSBAR; // UIGlobals.cpp
extern UIID UIWND_CHALLENGE_RIFT_PROGRESS_MARKER, UIWND_TIERED_RIFT_PROGRESS_MARKER, UIWND_TIERED_RIFT_TIMER_MARKER, UIWND_TIERED_RIFT_CHALLENGE_TIMER_MARKER; // UIGlobals.cpp
extern UIID UIWND_CHALLENGE_RIFT_COMPLETION_TIME_MARKER, UIWND_TIERED_RIFT_COMPLETION_TIME_MARKER, UITEXT_CHALLENGE_RIFT_PROGRESS, UITEXT_TIERED_RIFT_PROGRESS, UITEXT_TIERED_RIFT_TIMER; // UIGlobals.cpp
extern UIID UIWND_TIERED_RIFT_CHALLENGE_SPRINT, UIWND_TIERED_RIFT_CHALLENGE_SPRINT_AHEAD_ICON, UIWND_TIERED_RIFT_CHALLENGE_SPRINT_BEHIND_ICON; // UIGlobals.cpp
extern UIID UISTORYBOARD_TIERED_RIFT_CHALLENGE_SPRINT_AHEAD_FADEIN, UISTORYBOARD_TIERED_RIFT_CHALLENGE_SPRINT_AHEAD_FADEOUT, UISTORYBOARD_TIERED_RIFT_CHALLENGE_SPRINT_BEHIND_FADEIN, UISTORYBOARD_TIERED_RIFT_CHALLENGE_SPRINT_BEHIND_FADEOUT; // UIGlobals.cpp
extern UIID UITEXT_TIERED_RIFT_CHALLENGE_SPRINT_AHEAD, UITEXT_TIERED_RIFT_CHALLENGE_SPRINT_BEHIND; // UIGlobals.cpp
#include "../../../Common/SNO/SymbolicValue.h"
enum { GB_MONSTER_AFFIXES = 18, AT_IMMUNITY = 7 }; // DWARF GameBalanceDefinition.h:34/294
enum { MT_INVALID = -1, NUM_MONSTER_TYPE = 13 }; // DWARF ActorCommonDataSwig.h:13
extern UIID UIWND_CONSOLE_HUD; // UIGlobals.cpp
BOOL RActorIsHidden(const RActor * ptRActor); // RActors.cpp:12137
BOOL GameTypeIsPvP(); // GameTemplate.cpp:255
BOOL PlayersAreHostileToEachOther(const Player * ptPlayer0, const Player * ptPlayer1); // Player.cpp:5188
#include "../../../Common/Definitions/ActorDefinition.h"
#include "../../../Common/Console/Avenger.h"
extern const UIID UIWND_HP_BAR_NORMAL, UIWND_HP_BAR_MINION, UIWND_HP_BAR_SPECIAL, UIWND_HP_BAR_AVENGER, UIWND_HP_BAR_PLAYER, UIWND_HP_BAR_PVP, UIWND_HP_BAR_BOUNTY; // UIGame.cpp UIID globals
namespace UIGame { void Update(const float flDeltaTime); void ShowTrialEndMessage(); } // UIGame.cpp:6000/7754
namespace UIChatBalloons { void Update(); } // UIChatBalloons.cpp:237
namespace UILoreNotify { void Update(); } namespace UILorePlayer { void Update(); } // UILoreCommon.cpp:214/433
namespace UIFollowerSwap { void Update(); } // UIFollowerSwap.cpp:234
namespace UIBuffs { void Update(); } // UIBuffs.cpp:1606
namespace UIGoldCounter { void Update(); } // UIGoldCounter.cpp:65
namespace UICallouts { void Update(); } // UICallouts.cpp:607
void UIMinimap_Update(); // UIMinimap.cpp:4721
namespace UIMap { void Update(); } // UIMap.cpp:55
void UIActTransitions_Update(); // UIActTransitions.cpp:28
namespace UIHireling { void Display(); } // UIHireling.cpp:783
namespace UIStash { void Update(); } // UIStash.cpp:502
namespace UIHUD { namespace Console { void Update(); } } // UIConsoleHUD.cpp:2688
namespace UIGameSettings { namespace Console { BOOL CanLowerGameDifficulty(); } } // UIConsoleGameSettings.cpp:926
namespace UIInventory { void UpdateToolTips(ActorCommonData & tACDPlayer); void UpdateItemManipulationTimers(); } // UIInventory.cpp:6946/3194
void UIDeathMenu_Console_Display(); // UIConsoleDeathMenu.cpp:338
namespace UIMultiplayerDeathMenu { namespace Console { void Update(); } } // UIConsoleMultiplayerDeathMenu.cpp:270
namespace Console { namespace GamerProfile { int32 GetPrimaryProfileUserIndex(); } } // GamerProfileNX64.cpp:1130
namespace Console { namespace Screens { BOOL OpenToScreen(const ScreenSets eScreenSet, const ScreenID idScreen, int32 nControllerOpening, BOOL fSkipTransition); } } // ConsoleScreenManager.cpp:1104
static void sActTitleUpdate(); // UIGame.cpp:6492
static void sChallengeRift_TargetTime_UpdateDisplay(); // UIGame.cpp:6545

































































































































enum {
    MINIMUM_HP_BAR_WIDTH = 100,
    MAX_HP_BARS = 256,
};

struct AreaTitleParams {  // size 0x14
    FadeState eState;  // 0x00
    uint32 dwStateTimestamp;  // 0x04
    SNO snoLevelArea;  // 0x08
    SNO snoLevelAreaDisplayed;  // 0x0c
    uint8 bAlpha;  // 0x10
};
static_assert(sizeof(AreaTitleParams) == 0x14, "AreaTitleParams");

struct ActTitleParams {  // size 0x808
    uint32 dwTimestamp;  // 0x00
    FadeState eState;  // 0x04
    CHAR uszTitle[1024];  // 0x08
    CHAR uszSubtitle[1024];  // 0x408
};
static_assert(sizeof(ActTitleParams) == 0x808, "ActTitleParams");

struct GameTextParams {  // size 0x404
    uint32 dwStateTimestamp;  // 0x00
    CHAR uszText[1024];  // 0x04
};
static_assert(sizeof(GameTextParams) == 0x404, "GameTextParams");

typedef XGrowableList<GameTextParams, const GameTextParams &> GameTextList;

struct ErrorTextParams {  // size 0x408
    ErrorTextParams();
    uint32 dwTimestamp;  // 0x00
    CHAR uszText[1024];  // 0x04
    BOOL fDisplayBnetError;  // 0x404
};
static_assert(sizeof(ErrorTextParams) == 0x408, "ErrorTextParams");

struct UIHPBar {  // size 0x20
    UIID idWndWrapper;  // 0x00
    UIID idProgressBar;  // 0x08
    UIID idText;  // 0x10
    BOOL bProgressBarState;  // 0x18
    BOOL bTextState;  // 0x1c
};
static_assert(sizeof(UIHPBar) == 0x20, "UIHPBar");

enum NotifyType {
    NT_SAVEPOINT_SPLASH = 0,
    NT_LEVELUP_SPLASH = 1,
    NT_LEVELUP_NEW_SKILL_SLOT = 2,
    NT_LEVELUP_NEW_TRAIT_SLOT = 3,
    NT_LEVELUP_NEW_SKILL = 4,
    NT_LEVELUP_NEW_RUNE = 5,
    NT_LEVELUP_NEW_TRAIT = 6,
    NT_ALT_LEVELUP_SPLASH = 7,
    NT_ALT_LEVELUP_REWARDS = 8,
    NT_ALT_LEVELUP_NEW_PORTRAIT = 9,
    NT_DEMO_LEVELCAP_REACHED = 10,
    NT_TRANSMOG_REWARD = 11,
    NT_NEW_CRAFTER = 12,
    NT_NEW_HIRELING = 13,
    NT_EXTRACTED_POWER = 14,
};

struct NotificationData {  // size 0x24
    NotifyType eType;  // 0x00
    int32 nLevel;  // 0x04
    SNO snoPower;  // 0x08
    RuneType eRuneType;  // 0x0c
    GBID gbidTransmog;  // 0x10
    GBID gbidExtractedPower;  // 0x14
    CrafterType eCrafterType;  // 0x18
    HirelingClass eHirelingClass;  // 0x1c
    uint32 nPlayerIndex;  // 0x20
};
static_assert(sizeof(NotificationData) == 0x24, "NotificationData");

struct UIGameGlobals {  // size 0x2e88
    ActTitleParams tTitleParams;  // 0x00
    AreaTitleParams tAreaTextInfo;  // 0x808
    GameTextList * ptGameTextList;  // 0x820
    RActorID idRActorHighlight[4];  // 0x828
    UIID idRActorTag[4];  // 0x838
    ErrorTextParams tErrorText;  // 0x858
    RActorID idRActorAlternateHighlight[4];  // 0xc60
    UIID idRActorAltTag[4];  // 0xc70
    EffectGroupID idEffectButtonPrompt[4];  // 0xc90
    UIID idRActorLoopingMeter[4];  // 0xca0
    UIID idPlayerHUD[4];  // 0xcc0
    static const int32 SKILL_BUTTON_COUNT = 6;
    UIID idPlayerSkillGlow[4][6];  // 0xce0
    UIHPBar tUIHPBar[256];  // 0xda0
    XGrowableList<UIID, const UIID &> listActorTagControls;  // 0x2da0
    XGrowableList<NotificationData, const NotificationData &> listNotifications;  // 0x2df8
    uint32 dwSplashTextTurnOffTime;  // 0x2e50
    uint32 dwConsoleNotificationTimeEnd;  // 0x2e54
    uint32 dwFloatingMessageStartTime;  // 0x2e58
    uint32 dwFloatingMessageDuration;  // 0x2e5c
    uint32 dwTrialMessageStartTime;  // 0x2e60
    uint32 dwTrialMessageDelay;  // 0x2e64
    BOOL fShowNewSkillButton;  // 0x2e68
    BOOL fShowNewSkillKitTutorial;  // 0x2e6c
    BOOL fShowParagonPointsButton;  // 0x2e70
    bool bCheckedToShowParagonPointsButtonAtGameStart;  // 0x2e74
    uint32 dwTimestampChallengeRiftTargetTime;  // 0x2e78
    BOOL bChallengeRiftWasAhead;  // 0x2e7c
    BOOL bPassed10Percent;  // 0x2e80
};
static_assert(sizeof(UIGameGlobals) == 0x2e88, "UIGameGlobals");

struct ConsoleBackupPotionIterateParams {  // size 0x18
    GBID gbidToMatch;  // 0x00
    ActorCommonData * ptMatchedPotionACD;  // 0x08
    BOOL bFoundMatch;  // 0x10
    ConsoleBackupPotionIterateParams();
};
static_assert(sizeof(ConsoleBackupPotionIterateParams) == 0x18, "ConsoleBackupPotionIterateParams");

static const uint32 CONSOLE_TIME_BETWEEN_NOTIFICATIONS = 240;
static const GlobalSNO cgeDefaultFontForInGameText = GLOBALSNO_FONT_SANS_SERIF;
static const GlobalSNO cgeDefaultFontForRetroText = GLOBALSNO_FONT_EXOCETLIGHT;
UIGameGlobals * sgptUIGameGlobals;
// X/Client/RActors/RActors.cpp:11481
BOOL ShouldShowPrompt(const ActorCommonData * ptACDGizmo, const ActorCommonData * ptACDPlayer);
// X/Client/Input/InputNX64.cpp:993
BOOL InputPlatformAcceptButtonIsSwapped();
// X/Common/Items.cpp:5631
int32 ItemGetPlayerRecipient(const ActorCommonData & tACD);
// X/Common/Player.cpp:663
CRefString PlayerGetHeroDisplayName(const Player & tPlayer, BOOL fDisplayPound);
// X/Client/EffectGroup.cpp:1311
EffectGroupID EffectGroupCreateTracked(const SNO snoEffectGroup, const RActorID idRActorSource, const RActorID idRActorDest, ParentType eTypeOverride, const BOOL bIgnoreOwnerAlpha);

#include "../../RActors/RActorAttachments.h"
#include "../../Definitions/CAttachmentDefinition.h"
#include "../../../Common/Definitions/AttachmentDefinition.h"
// X/Client/Camera/CameraManager.cpp:1247
BOOL WorldPointToScreenPointUIC(const Vector3D & wp, Vector2D & wpScreenUIC, int32 nViewIndex);
#include "../../../Common/Definitions/SkillKitDefinition.h"
#include "../../../Common/Powers/PowersCommon.h"
extern const int32 cgnActiveSkillSlotLevelReq[6];  // X/Common/Player.cpp:345
extern const int32 cgnTraitSlotLevelReq[4];  // X/Common/Player.cpp:346
namespace UIFastEquip { namespace Console { void ReportNewSkill(const Player *, SNO); void ReportNewRune(const Player *, SNO, RuneType); } }  // UIConsoleFastEquip.cpp:272,291
namespace UIGame { void ShowLevelUpButton(ACDID idACDPlayer); }
static void sHPBarCreate(int32 nIndex);
namespace UIChatBalloons { void InitializeForGame(); } namespace UIBuffs { void InitializeForGame(); } void UILoreCommon_InitializeForGame();
namespace UICountDown { void InitializeForGame(); } namespace UICallouts { void InitializeForGame(); } namespace UIFollowerSwap { void InitializeForGame(); }
namespace UIGoldCounter { void InitializeForGame(); } namespace UITransmute { void InitializeForGame(); } namespace UIEquipLegendaryPowers { void InitializeForGame(); }
namespace UIJewelUpgrade { void InitializeForGame(); } namespace UIBlizzconEnd { void InitializeForGame(); } namespace UIInventory { namespace Console { void InitializeForGame(); }}
namespace UIStash { namespace Console { void InitializeForGame(); }} namespace UIShop { namespace Console { void InitializeForGame(); }} namespace UIBloodShardVendor { namespace Console { void InitializeForGame(); }}
namespace UIJeweler { namespace Console { void InitializeForGame(); }} namespace UIBlacksmith { namespace Console { void InitializeForGame(); }} namespace UIMystic { namespace Console { void InitializeForGame(); }}
namespace UISkills { namespace Console { void InitializeForGame(); }} namespace UIFollower { namespace Console { void InitializeForGame(); }} namespace UIParty { namespace Console { void InitializeForGame(); }}
namespace UILore { namespace Console { void InitializeForGame(); }} namespace UIEventQueue { namespace Console { void InitializeForGame(); }} namespace UIFastEquip { namespace Console { void InitializeForGame(); }}
namespace UIPause { namespace Console { void InitializeForGame(); }} namespace UICharacterDetails { namespace Console { void InitializeForGame(); }} namespace UIHUD { namespace Console { void InitializeForGame(); }}
namespace UIParagonBonuses { namespace Console { void InitializeForGame(); }} namespace UIMail { namespace Console { void InitializeForGame(); }} namespace UIFriendsList { namespace Console { void InitializeForGame(); }}
namespace UIGameSettings { namespace Console { void InitializeForGame(); }} namespace UIConsoleRiftEntry { void InitializeForGame(); } namespace UIJoinRift { void InitializeForGame(); }
namespace UIRiftComplete { void InitializeForGame(); } namespace UIEquipmentManager { namespace Console { void InitializeForGame(); }} namespace UIEquipmentManagerDetails { void InitializeForGame(); }
namespace UISeasonStash { namespace Console { void InitializeForGame(); }} namespace UIEquipmentManager { void InitializeForGame(); }
extern UIID UIDLG_GAME_MAIN; extern UIID UIDLG_GAME_LOWER;  // X/Client/UserInterface/UIGlobals.cpp





namespace UIGame {
void InitializeForGame()
{



    sgptUIGameGlobals = (UIGameGlobals *)SigmaMemoryNew(sizeof(UIGameGlobals), 4, NULL, TRUE);

    sgptUIGameGlobals->tAreaTextInfo.eState = FADESTATE_HIDDEN;
    sgptUIGameGlobals->tAreaTextInfo.snoLevelArea = -1;
    sgptUIGameGlobals->tAreaTextInfo.snoLevelAreaDisplayed = -1;
    sgptUIGameGlobals->tAreaTextInfo.bAlpha = 0;
    sgptUIGameGlobals->listActorTagControls.Initialize(NULL);

    for (int i = 0; i < 4; ++i)
    {
        sgptUIGameGlobals->idRActorHighlight[i] = RACTORID_INVALID;
        sgptUIGameGlobals->idRActorTag[i] = GetUIIDNone();


        sgptUIGameGlobals->idRActorAlternateHighlight[i] = RACTORID_INVALID;
        sgptUIGameGlobals->idRActorAltTag[i] = GetUIIDNone();
        sgptUIGameGlobals->idEffectButtonPrompt[i] = EFFECTGROUPID_INVALID;
        sgptUIGameGlobals->idRActorLoopingMeter[i] = GetUIIDNone();
        sgptUIGameGlobals->idPlayerHUD[i] = UIIDCreateFromParent(UIWND_CONSOLE_HUD_PLAYER_SLOTS, CRefString().Format(".Player%d", i + 1));
        for (int32 j = 0; j < UIGameGlobals::SKILL_BUTTON_COUNT; ++j)
        {
            sgptUIGameGlobals->idPlayerSkillGlow[i][j] = UIIDCreateFromParent(UIWND_CONSOLE_HUD_PLAYER_SLOTS,
                CRefString().Format(".Player%d.Local.SkillsParent.Skills.Slot%d.BuffGlow", i + 1, j + 1));
        }
    }


    sgptUIGameGlobals->bCheckedToShowParagonPointsButtonAtGameStart = false;

    sgptUIGameGlobals->ptGameTextList = new GameTextList;
    sgptUIGameGlobals->bChallengeRiftWasAhead = FALSE;
    sgptUIGameGlobals->bPassed10Percent = FALSE;






    UIChatBalloons::InitializeForGame();
    UIBuffs::InitializeForGame();
    UILoreCommon_InitializeForGame();
    UICountDown::InitializeForGame();
    UICallouts::InitializeForGame();



    UIFollowerSwap::InitializeForGame();
    UIGoldCounter::InitializeForGame();





















    UITransmute::InitializeForGame();
    UIEquipLegendaryPowers::InitializeForGame();
    UIJewelUpgrade::InitializeForGame();

    if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
    {
        UIBlizzconEnd::InitializeForGame();
    }









    UIInventory::Console::InitializeForGame();
    UIStash::Console::InitializeForGame();
    UIShop::Console::InitializeForGame();
    UIBloodShardVendor::Console::InitializeForGame();
    UIJeweler::Console::InitializeForGame();
    UIBlacksmith::Console::InitializeForGame();
    UIMystic::Console::InitializeForGame();
    UISkills::Console::InitializeForGame();
    UIFollower::Console::InitializeForGame();
    UIParty::Console::InitializeForGame();
    UILore::Console::InitializeForGame();
    UIEventQueue::Console::InitializeForGame();
    UIFastEquip::Console::InitializeForGame();
    UIPause::Console::InitializeForGame();
    UICharacterDetails::Console::InitializeForGame();
    UIHUD::Console::InitializeForGame();
    UIParagonBonuses::Console::InitializeForGame();
    UIMail::Console::InitializeForGame();
    UIFriendsList::Console::InitializeForGame();
    UIGameSettings::Console::InitializeForGame();
    UIConsoleRiftEntry::InitializeForGame();
    UIJoinRift::InitializeForGame();
    UIRiftComplete::InitializeForGame();
    UIEquipmentManager::Console::InitializeForGame();
    UIEquipmentManagerDetails::InitializeForGame();
    UISeasonStash::Console::InitializeForGame();


    UIEquipmentManager::InitializeForGame();

    for (int32 i = 0; i < 256; ++i)
    {
        sHPBarCreate(i);
    }








    {
        TextCreationParams tTagParams(UIT_TEXT, "RActorTag", UIDLG_GAME_MAIN);
        tTagParams.fActive = FALSE;
        tTagParams.snoFont = GlobalSNOGet(GLOBALSNO_FONT_SANS_SERIF);
        tTagParams.nFontSize = 16;
        tTagParams.eJustifyHorizontal = UIJH_CENTER;
        tTagParams.eJustifyVertical = UIJV_MIDDLE;
        tTagParams.rgbaText = crgbaWhite;
        tTagParams.tImageTexBkgrnd.snoTexture = GlobalSNOGet(GLOBALSNO_TEXTURE_ITEM_TAG);
        tTagParams.tImageTexBkgrnd.nFrame = -1;
        tTagParams.snoCustomShader = GlobalSNOGet(GLOBALSNO_SHADER_TEXT);


        UIControlSourceKey tNameplateTemplateSrcKey = UIControlSourceKey::Parse("ConsoleGameHUDMisc:game_dialog_backgroundScreen.nameplatebuttonprompt", -1, NULL, NULL);
        UIControlSourceKey tLoopingMeterTemplateSrcKey = UIControlSourceKey::Parse("ConsoleGameHUDMisc:game_dialog_backgroundScreen.loopinganimmeter", -1, NULL, NULL);


        for (int32 i = 0; i < 4; ++i)
        {
            tTagParams.id.SetFromParent(UIDLG_GAME_MAIN, CRefString().Format("RActorTag%d", i));
            UIText & tTag = *new UIText(tTagParams, Vector2D(0.0f, 0.0f), Vector2D(10.0f, 10.0f));
            tTag.SetWindowColor(crgbaBlack);
            sgptUIGameGlobals->idRActorTag[i] = tTag.GetID();


            UIWindow * ptNameplateWindow = (UIWindow *)UI_CreateControlFromTemplate(tNameplateTemplateSrcKey, DynamicControlCreationParams(CRefString().Format("Nameplate%d", i), UIDLG_GAME_LOWER), NULL);
            sgptUIGameGlobals->idRActorAltTag[i] = ptNameplateWindow->GetID();

            UIID idText = UIIDCreateFromParent(ptNameplateWindow->GetID(), ".nameplate");
            UIText & tText = UI_GetTextFromID(idText);
            tText.SetTextureFrame(tTagParams.tImageTexBkgrnd);

            UIWindow * ptLoopingMeterWindow = (UIWindow *)UI_CreateControlFromTemplate(tLoopingMeterTemplateSrcKey, DynamicControlCreationParams(CRefString().Format("LoopingMeter%d", i), UIDLG_GAME_LOWER), NULL);
            sgptUIGameGlobals->idRActorLoopingMeter[i] = ptLoopingMeterWindow->GetID();
        }

    }

    sgptUIGameGlobals->listNotifications.Initialize(NULL);
    sgptUIGameGlobals->dwSplashTextTurnOffTime = 0;









}
}

namespace UIGame {
void DisposeForGame()
{
    if (!sgptUIGameGlobals) return;









    for (int32 i = 0; i < MAX_LOCAL_PLAYERS; ++i) {

        if (sgptUIGameGlobals->idRActorTag[i] != GetUIIDNone()) {

            UI_DeleteControl(sgptUIGameGlobals->idRActorTag[i], TRUE);
        }

        if (sgptUIGameGlobals->idRActorAltTag[i] != GetUIIDNone()) {

            UI_DeleteControl(sgptUIGameGlobals->idRActorAltTag[i], TRUE);
        }
        if (sgptUIGameGlobals->idRActorLoopingMeter[i] != GetUIIDNone()) {

            UI_DeleteControl(sgptUIGameGlobals->idRActorLoopingMeter[i], TRUE);
        }
    }


    for (int32 i = 0; i < 256; ++i) {

        UIHPBar & tHPBar = sgptUIGameGlobals->tUIHPBar[i];
        UI_DeleteControl(tHPBar.idProgressBar, TRUE);
        UI_DeleteControl(tHPBar.idText, TRUE);
        UI_DeleteControl(tHPBar.idWndWrapper, TRUE);
    }


    UISeasonStash::Console::DisposeForGame();
    UIEquipmentManagerDetails::DisposeForGame();
    UIEquipmentManager::Console::DisposeForGame();
    UIRiftComplete::DisposeForGame();
    UIJoinRift::DisposeForGame();
    UIConsoleRiftEntry::DisposeForGame();
    UIGameSettings::Console::DisposeForGame();
    UIFriendsList::Console::DisposeForGame();
    UIMail::Console::DisposeForGame();
    UIParagonBonuses::Console::DisposeForGame();
    UIHUD::Console::DisposeForGame();
    UICharacterDetails::Console::DisposeForGame();
    UIFastEquip::Console::DisposeForGame();
    UIEventQueue::Console::DisposeForGame();
    UILore::Console::DisposeForGame();
    UIParty::Console::DisposeForGame();
    UIFollower::Console::DisposeForGame();
    UISkills::Console::DisposeForGame();
    UIStash::Console::DisposeForGame();
    UIMystic::Console::DisposeForGame();
    UIBlacksmith::Console::DisposeForGame();
    UIJeweler::Console::DisposeForGame();
    UIBloodShardVendor::Console::DisposeForGame();
    UIShop::Console::DisposeForGame();
    UIInventory::Console::DisposeForGame();
    UIPause::Console::DisposeForGame();









    UIJewelUpgrade::DisposeForGame();
    UIEquipLegendaryPowers::DisposeForGame();
    UITransmute::DisposeForGame();

















    UILoreCommon_DisposeForGame();




    UIGoldCounter::DisposeForGame();
    UIFollowerSwap::DisposeForGame();
    UICallouts::DisposeForGame();
    UIBuffs::DisposeForGame();



    UIChatBalloons::DisposeForGame();

    if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT)) {

    UIBlizzconEnd::DisposeForGame();
    }

    UIEquipmentManager::DisposeForGame();

    if (sgptUIGameGlobals) {

        sgptUIGameGlobals->listNotifications.Dispose();

        POS posControls = sgptUIGameGlobals->listActorTagControls.GetHeadPosition();
        while (posControls) {

            POS posCurrent = posControls;
            UIID idControl = sgptUIGameGlobals->listActorTagControls.GetNext(posControls);
            UI_DeleteControl(idControl, TRUE);
            sgptUIGameGlobals->listActorTagControls.RemoveAt(posCurrent);
        }

        sgptUIGameGlobals->listActorTagControls.Dispose();
        delete sgptUIGameGlobals->ptGameTextList;
        SigmaMemoryFree(sgptUIGameGlobals, NULL);
        sgptUIGameGlobals = NULL;
    }
}
}

static void sHPBarCreate(
    int32 nIndex)
{
    UIHPBar & tHPBar = sgptUIGameGlobals->tUIHPBar[nIndex];

    UIControlSourceKey tHPBarTemplateSrcKey = UIControlSourceKey::Parse(GameTypeIsPvP() ? "Game:game_dialog_main.HPBar_PVP" : "Game:game_dialog_main.HPBar", -1, NULL, NULL);
    UIWindow * ptWrapper = (UIWindow *)UI_CreateControlFromTemplate(tHPBarTemplateSrcKey, DynamicControlCreationParams(CRefString().Format("HPBar_Wrapper_%d", nIndex), UIDLG_GAME_MAIN), NULL);
    tHPBar.idWndWrapper = ptWrapper->GetID();


    tHPBar.idProgressBar = UIIDCreateFromParent(tHPBar.idWndWrapper, ".HP_ProgressBar");
    tHPBar.idText = UIIDCreateFromParent(tHPBar.idWndWrapper, ".HP_Text");

    if (!UI_TestID(tHPBar.idText)) return;
    UIText & tText = UI_GetTextFromID(tHPBar.idText);
    tText.SetFontSize(16);





    if (!UI_TestID(tHPBar.idProgressBar)) return;
    UIProgressBar & tProgressBar = UI_GetProgressBarFromID(tHPBar.idProgressBar);
    UIWindow & tFGWindow = UI_GetWindowFromID(tProgressBar.GetForegroundChild());
    Vector2D vecBkgrndSize;
    tFGWindow.GetTextureFrameSize(vecBkgrndSize);
    tProgressBar.SetRelativePosition(Vector2D(0.0f, 0.0f), vecBkgrndSize);

    tHPBar.bProgressBarState = FALSE;
    tHPBar.bTextState = FALSE;
}















































































static SNO sGetClassResourceTutorial(
    PlayerClass eClass)
{
    SNO snoTutorial = -1;

    switch (eClass) {

    case PLAYER_CLASS_DEMONHUNTER:
        snoTutorial = GlobalSNOGet(GLOBALSNO_TUTORIAL_RESOURCE_HATREDDISC); break;


    case PLAYER_CLASS_BARBARIAN:
        snoTutorial = GlobalSNOGet(GLOBALSNO_TUTORIAL_RESOURCE_FURY); break;


    case PLAYER_CLASS_WIZARD:
        snoTutorial = GlobalSNOGet(GLOBALSNO_TUTORIAL_RESOURCE_ARCANE); break;


    case PLAYER_CLASS_WITCHDOCTOR:
        snoTutorial = GlobalSNOGet(GLOBALSNO_TUTORIAL_RESOURCE_MANA); break;


    case PLAYER_CLASS_MONK:
        snoTutorial = GlobalSNOGet(GLOBALSNO_TUTORIAL_RESOURCE_SPIRIT); break;


    case PLAYER_CLASS_CRUSADER:
        snoTutorial = GlobalSNOGet(GLOBALSNO_TUTORIAL_RESOURCE_FAITH); break;


    case PLAYER_CLASS_NECROMANCER:
        snoTutorial = GlobalSNOGet(GLOBALSNO_TUTORIAL_RESOURCE_ESSENCE); break;

    }


    return snoTutorial;
}






void UIGame::InitializeForPlayer()
{









































































































}













































































































































static void sUpdateParagonPtsVisiblity()
{



















    if (sgptUIGameGlobals->fShowParagonPointsButton)
    {
        sgptUIGameGlobals->fShowParagonPointsButton = FALSE;


        gtTutorialMgr.ShowTutorial(GetUIIDNone(), GlobalSNOGet(GLOBALSNO_TUTORIAL_ALT_LEVEL), NULL, TRUE);
    }
}





static void sHandleBasicNotification(const UIID & idWnd, const UIID & idStoryboard)
{
    // ...
    if (sgptUIGameGlobals->dwSplashTextTurnOffTime)
    {
        if (CGameGetClientTicks() > sgptUIGameGlobals->dwSplashTextTurnOffTime)
        {
            UI_GetWindowFromID(idWnd).SetActive(FALSE);
            sgptUIGameGlobals->dwSplashTextTurnOffTime = 0;
            sgptUIGameGlobals->listNotifications.RemoveHead();
        }
    }
    else
    {
        UI_GetWindowFromID(idWnd).SetActive(TRUE);
        // ...
        UIStoryboard & tStoryboard = UI_GetStoryboardFromID(idStoryboard);
        tStoryboard.Play(FALSE, 0.0f);
        sgptUIGameGlobals->dwSplashTextTurnOffTime = CGameGetClientTicks() + (uint32)(tStoryboard.GetDuration() * TICKS_PER_SECOND);
    }
}



static void sHandleSkillNotification(
    const UIID & idWnd,
    const UIID & idStoryboard,
    const UIID & idText,
    const UIID & idIcon,
    const SNO snoPower)
{
    if (sgptUIGameGlobals->dwSplashTextTurnOffTime) {
        if (CGameGetClientTicks() > sgptUIGameGlobals->dwSplashTextTurnOffTime) {
            UI_GetWindowFromID(idWnd).SetActive(FALSE);
            sgptUIGameGlobals->dwSplashTextTurnOffTime = 0;

            sgptUIGameGlobals->listNotifications.RemoveHead();
        }
    } else {
        UI_GetWindowFromID(idWnd).SetActive(TRUE);


        HotbarButtonData tHotbarButtonData;
        tHotbarButtonData.SetSkill(snoPower, -1);
        UIHotbarButton & tIcon = UI_GetHotbarButtonFromID(idIcon);
        tIcon.UpdateSkillButtonState(tHotbarButtonData, FALSE, TRUE, TRUE, FALSE, FALSE);
        tIcon.DisableTimer();






        CRefString sPowerName;
        UISkills::GetName(snoPower, sPowerName);
        UI_GetTextFromID(idText).SetText(sPowerName, FALSE);

        UIStoryboard & tStoryboard = UI_GetStoryboardFromID(idStoryboard);
        tStoryboard.Play(FALSE, 0.0f);
        sgptUIGameGlobals->dwSplashTextTurnOffTime = CGameGetClientTicks() + (uint32)(tStoryboard.GetDuration() * TICKS_PER_SECOND);
    }
}



static void sHandleRuneNotification(
    const UIID & idWnd,
    const UIID & idStoryboard,
    const UIID & idTextTitle,
    const UIID & idText,
    const UIID & idIcon,
    const SNO snoPower,
    const RuneType eRune)
{
    if (sgptUIGameGlobals->dwSplashTextTurnOffTime) {
        if (CGameGetClientTicks() > sgptUIGameGlobals->dwSplashTextTurnOffTime) {

            UI_GetWindowFromID(idWnd).SetActive(FALSE);
            sgptUIGameGlobals->dwSplashTextTurnOffTime = 0;
            sgptUIGameGlobals->listNotifications.RemoveHead();
        }
    } else {

        UI_GetWindowFromID(idWnd).SetActive(TRUE);

        CRefString sRuneFrame = PowerGetRuneStoneTextureFrame(eRune);
        UI_GetWindowFromID(idIcon).SetTextureFrame(sRuneFrame);

        CRefString sFormatStr = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_H2O_LAYOUT), "RuneUnlockedTitle");
        CRefString sPowerName;
        UISkills::GetName(snoPower, sPowerName);
        UI_GetTextFromID(idTextTitle).SetText(LocalizedSprintf(sFormatStr.str(), sPowerName.str(), NULL), FALSE);

        CRefString sRuneName = PowerGetRuneName(snoPower, eRune);
        UI_GetTextFromID(idText).SetText(sRuneName, FALSE);

        UIStoryboard & tStoryboard = UI_GetStoryboardFromID(idStoryboard);
        tStoryboard.Play(FALSE, 0.0f);
        sgptUIGameGlobals->dwSplashTextTurnOffTime = CGameGetClientTicks() + (uint32)(tStoryboard.GetDuration() * TICKS_PER_SECOND);
    }
}



static void sHandleTransmogRewardNotification(
    const UIID & idWnd,
    const UIID & idStoryboard,
    const UIID & idText,
    const UIID & idIcon,
    const GBID gbidTransmog)
{
    if (sgptUIGameGlobals->dwSplashTextTurnOffTime) {
        if (CGameGetClientTicks() > sgptUIGameGlobals->dwSplashTextTurnOffTime) {

            UI_GetWindowFromID(idWnd).SetActive(FALSE);
            sgptUIGameGlobals->dwSplashTextTurnOffTime = 0;
            sgptUIGameGlobals->listNotifications.RemoveHead();
        }
    } else {

        UI_GetWindowFromID(idWnd).SetActive(TRUE);

        ImageTextureFrame tImageTexFrame;
        ACDInventoryGetItemDrawInfoWithGBID(gbidTransmog, tImageTexFrame, ITEMDRAW_NORMAL);
        UI_GetWindowFromID(idIcon).SetTextureFrame(tImageTexFrame);

        CRefString sItemName = ItemGetDisplayName(gbidTransmog);
        UI_GetTextFromID(idText).SetText(sItemName.str(), FALSE);

        UIStoryboard & tStoryboard = UI_GetStoryboardFromID(idStoryboard);
        tStoryboard.Play(FALSE, 0.0f);
        sgptUIGameGlobals->dwSplashTextTurnOffTime = CGameGetClientTicks() + (uint32)(tStoryboard.GetDuration() * TICKS_PER_SECOND);
    }
}



static void sHandleExtractedPowerNotification(
    const UIID & idWnd,
    const UIID & idStoryboard,
    const UIID & idText,
    const UIID & idIcon,
    const UIID & idIconSmall,
    const GBID gbidPowerItem)
{
    if (sgptUIGameGlobals->dwSplashTextTurnOffTime) {
        if (CGameGetClientTicks() > sgptUIGameGlobals->dwSplashTextTurnOffTime) {

            UI_GetWindowFromID(idWnd).SetActive(FALSE);
            sgptUIGameGlobals->dwSplashTextTurnOffTime = 0;
            sgptUIGameGlobals->listNotifications.RemoveHead();
        }
    } else {

        UI_GetWindowFromID(idWnd).SetActive(TRUE);

        ImageTextureFrame tImageTexFrame;
        ACDInventoryGetItemDrawInfoWithGBID(gbidPowerItem, tImageTexFrame, ITEMDRAW_NORMAL);
        UIWindow & tLargeIcon = UI_GetWindowFromID(idIcon);
        UIWindow & tSmallIcon = UI_GetWindowFromID(idIconSmall);

        if (ItemIsLargeSlotItem(gbidPowerItem)) {

            tSmallIcon.SetActive(FALSE);
            tLargeIcon.SetActive(TRUE);
            tLargeIcon.SetTextureFrame(tImageTexFrame);
        }
        else {

            tLargeIcon.SetActive(FALSE);
            tSmallIcon.SetActive(TRUE);
            tSmallIcon.SetTextureFrame(tImageTexFrame);
        }

        CRefString sItemName = ItemGetDisplayName(gbidPowerItem);
        UI_GetTextFromID(idText).SetText(sItemName.str(), FALSE);

        UIStoryboard & tStoryboard = UI_GetStoryboardFromID(idStoryboard);
        tStoryboard.Play(FALSE, 0.0f);
        sgptUIGameGlobals->dwSplashTextTurnOffTime = CGameGetClientTicks() + (uint32)(tStoryboard.GetDuration() * TICKS_PER_SECOND);
    }
}














































































































void sConsoleProcessPlayerNotification(const NotificationData & tData)
{
    switch (tData.eType) {








    case NT_LEVELUP_SPLASH: {

        CRefString sTitle = LocalizedSprintf(StringListFetch("H2OLayout:LevelUpTitle"), UnicodeText::FormatNumber(tData.nLevel, TRUE, FNS_NORMAL, XLOCALE_INVALID).str(), NULL);
        CRefString sLevel = LocalizedSprintf(StringListFetch("H2OLayout:LevelUp"), UnicodeText::FormatNumber(tData.nLevel, TRUE, FNS_NORMAL, XLOCALE_INVALID).str(), NULL);
        ImageHandle hImage = StringToImageHandle("Levelup_Normal");
        UIEventQueue::Console::DisplayMessageForPlayer(sTitle, PlayerGetByPlayerIndex(tData.nPlayerIndex), hImage, &sLevel, FALSE);
        break;
    }

    case NT_LEVELUP_NEW_SKILL_SLOT: {

        Player * ptPlayer = PlayerGetByPlayerIndex(tData.nPlayerIndex);

        CRefString sTitle = StringListFetch("H2OLayout:SkillSlotUnlockedTitle");
        CRefString sDetail = StringListFetch("H2OLayout:SkillSlotUnlocked");
        ImageHandle hImage = StringToImageHandle("Console_SkillUnlocked_Active");
        UIEventQueue::Console::DisplayMessageForPlayer(sTitle, ptPlayer, hImage, &sDetail, FALSE);

        if (tData.nLevel == 2) {

            UIID idSkillButton = UIIDCreateFromParent(UIWND_CONSOLE_HUD_PLAYER_SLOTS, CRefString().Format(".Player%d.Local.SkillsParent.Skills.Slot6", UIHUD::Console::PlayerGetHUDSlot(ptPlayer) + 1));
            gtTutorialMgr.ShowTutorial(idSkillButton, GlobalSNOGet(GLOBALSNO_TUTORIAL_LEVEL2_SKILL_UNLOCK), ptPlayer, TRUE);

            gtTutorialMgr.QueueTutorial(UIHUD::Console::PlayerGetResourceUIControl(ptPlayer), sGetClassResourceTutorial(ptPlayer->ePlayerClassPortrait), 0, ptPlayer->nPlayerIndex);
        }
        break;
    }

    case NT_LEVELUP_NEW_TRAIT_SLOT: {

        CRefString sTitle = StringListFetch("H2OLayout:TraitSlotUnlockedTitle");
        CRefString sDetail = StringListFetch("H2OLayout:TraitSlotUnlocked");
        ImageHandle hImage = StringToImageHandle("Console_SkillUnlocked_Passive");
        UIEventQueue::Console::DisplayMessageForPlayer(sTitle, PlayerGetByPlayerIndex(tData.nPlayerIndex), hImage, &sDetail, FALSE);
        break;
    }

    case NT_LEVELUP_NEW_SKILL: {

        Player * ptPlayer = PlayerGetByPlayerIndex(tData.nPlayerIndex);


        ScopedPowerDefinitionLockR pdefPower(tData.snoPower);

        ImageHandle hImage = PowerGeneralTagGetInt(pdefPower, TAG_POWER_ICON_NORMAL);

        CRefString sPowerName;
        UISkills::GetName(tData.snoPower, sPowerName);

        CRefString sTitle = StringListFetch("H2OLayout:SkillUnlockedTitle");
        UIEventQueue::Console::DisplayMessageForPlayer(sTitle, ptPlayer, hImage, &sPowerName, FALSE);

        if (tData.nLevel == 3) {

            gtTutorialMgr.ShowTutorial(GetUIIDNone(), GlobalSNOGet(GLOBALSNO_TUTORIAL_LEVEL3_SKILL_UNLOCK), ptPlayer, TRUE);
        }
        break; }


    case NT_LEVELUP_NEW_RUNE: {

        Player * ptPlayer = PlayerGetByPlayerIndex(tData.nPlayerIndex);


        ScopedPowerDefinitionLockR pdefPower(tData.snoPower);

        ImageHandle hImage = PowerGeneralTagGetInt(pdefPower, TAG_POWER_ICON_NORMAL);

        CRefString sFormatStr = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_H2O_LAYOUT), "RuneUnlockedTitle");

        CRefString sPowerName;
        UISkills::GetName(tData.snoPower, sPowerName);

        CRefString sTitle = LocalizedSprintf(sFormatStr.str(), sPowerName.str(), NULL);

        CRefString sRuneName = PowerGetRuneName(tData.snoPower, tData.eRuneType);
        UIEventQueue::Console::DisplayMessageForPlayer(sTitle, ptPlayer, hImage, &sRuneName, FALSE);

        break; }



    case NT_LEVELUP_NEW_TRAIT: {

        ScopedPowerDefinitionLockR pdefPower(tData.snoPower);

        ImageHandle hImage = PowerGeneralTagGetInt(pdefPower, TAG_POWER_ICON_NORMAL);

        CRefString sPowerName;
        UISkills::GetName(tData.snoPower, sPowerName);

        CRefString sTitle = StringListFetch("H2OLayout:TraitUnlockedTitle");
        UIEventQueue::Console::DisplayMessageForPlayer(sTitle, PlayerGetByPlayerIndex(tData.nPlayerIndex), hImage, &sPowerName, FALSE);
        break; }


    case NT_ALT_LEVELUP_SPLASH: {

        CRefString sLevel = LocalizedSprintf(StringListFetch("H2OLayout:AltLevelUp"), UnicodeText::FormatNumber(tData.nLevel, TRUE, FNS_NORMAL, XLOCALE_INVALID).str(), NULL);
        CRefString sTextureName = CRefString().Format("UI:Levelup_Trait_Normal");
        UIEventQueue::Console::DisplayLongMessageForPlayer(sLevel, PlayerGetByPlayerIndex(tData.nPlayerIndex), ImageTextureFrame(sTextureName), FALSE);
        break; }

    case NT_ALT_LEVELUP_REWARDS: {

        float flMFBonusFromAltLevel = 0.03f;
        CRefString usMFBonus = CRefString("+") + UnicodeText::FormatNumber(flMFBonusFromAltLevel, TRUE, FNS_PERCENT, XLOCALE_INVALID, 0, 0);
        CRefString usGFBonus = usMFBonus;
        CRefString sLevel = LocalizedSprintf(StringListFetch("H2OLayout:AltLevelUpRewards"), usMFBonus.str(), usGFBonus.str(), NULL);

        CRefString sTextureName = CRefString().Format("UI:Levelup_Trait_Normal");
        UIEventQueue::Console::DisplayLongMessageForPlayer(sLevel, PlayerGetByPlayerIndex(tData.nPlayerIndex), ImageTextureFrame(sTextureName), FALSE);
        break; }


    case NT_ALT_LEVELUP_NEW_PORTRAIT: {

        CRefString sLevel = LocalizedSprintf(StringListFetch("H2OLayout:AltLevelUnlocked"), UnicodeText::FormatNumber(tData.nLevel, TRUE, FNS_NORMAL, XLOCALE_INVALID).str(), NULL);
        CRefString sTextureName = CRefString().Format("UI:Levelup_Trait_Normal");
        UIEventQueue::Console::DisplayLongMessageForPlayer(sLevel, PlayerGetByPlayerIndex(tData.nPlayerIndex), ImageTextureFrame(sTextureName), FALSE);
        break; }


    case NT_DEMO_LEVELCAP_REACHED: {

        CRefString sMessage = StringListFetch("ConsoleDemo:LevelCapReached_EventQueueLine1");
        CRefString sTextureName = CRefString().Format("UI:Levelup_Normal");
        UIEventQueue::Console::DisplayLongMessageForPlayer(sMessage, PlayerGetByPlayerIndex(tData.nPlayerIndex), ImageTextureFrame(sTextureName), FALSE);
        break; }

    default: break;
    }
}


void sConsoleMultiplayerUpdateNotificationQueue()
{
    if (sgptUIGameGlobals->dwConsoleNotificationTimeEnd > CGameGetClientTicks())
    {
        // ...
        return;
    }
    // ...
    sgptUIGameGlobals->dwConsoleNotificationTimeEnd = CGameGetClientTicks() + CONSOLE_TIME_BETWEEN_NOTIFICATIONS;
    // ...
    BOOL bFoundPlayerNotification[MAX_LOCAL_PLAYERS] = { FALSE, FALSE, FALSE, FALSE };
    // ...
    POS posNotifications = sgptUIGameGlobals->listNotifications.GetHeadPosition();
    // ...
    while (posNotifications)
    {
        POS posCurrent = posNotifications;
        NotificationData & tData = sgptUIGameGlobals->listNotifications.GetNext(posNotifications);
        // ...
        if (tData.eType != NT_SAVEPOINT_SPLASH)
        {
            if (!bFoundPlayerNotification[tData.nPlayerIndex])
            {
                bFoundPlayerNotification[tData.nPlayerIndex] = TRUE;
                sConsoleProcessPlayerNotification(tData);
                sgptUIGameGlobals->listNotifications.RemoveAt(posCurrent);
            }
        }
        else
        {
            // ...
            sHandleBasicNotification(UIWND_SAVEPOINTSPLASH, UISTORYBOARD_SAVEPOINTSPLASH);
        }
    }
}















void sUpdateNotificationQueue()

{


    if (PlayerGetLocalPlayerCount() > 1 && sgptUIGameGlobals->listNotifications.Count()) {

        sConsoleMultiplayerUpdateNotificationQueue();
        return;
    }


    Player * ptPlayer = LocalPlayerGet();

    if (ptPlayer->nCurrentLevel >= 24 && !Tutorial::PlayerHasSeen(*ptPlayer, GlobalSNOGet(GLOBALSNO_TUTORIAL_ELECTIVE_MODE_P6)) && !gtTutorialMgr.IsAnyShowing(FALSE) && !GameIsWeeklyChallenge()) {

        GameplayPrefs & tGameplayPrefs = Prefs::GetMutableGameplayPrefs();

        if (!tGameplayPrefs.GetSkillElectiveMode()) {

            tGameplayPrefs.SetUseElectiveMode(TRUE);



            gtTutorialMgr.ShowTutorial(GetUIIDNone(), GlobalSNOGet(GLOBALSNO_TUTORIAL_ELECTIVE_MODE_P6), NULL, TRUE);
        }
        else {


            Tutorial::Shown(*ptPlayer, GlobalSNOGet(GLOBALSNO_TUTORIAL_ELECTIVE_MODE_P6));
        }
    }

    if (!sgptUIGameGlobals->listNotifications.Count()) {
        return;
    }


    NotificationData & tData = sgptUIGameGlobals->listNotifications.GetHead();
    switch (tData.eType) {






    case NT_LEVELUP_SPLASH:


        if (sgptUIGameGlobals->dwSplashTextTurnOffTime) {

            if (CGameGetClientTicks() > sgptUIGameGlobals->dwSplashTextTurnOffTime) {

                UI_GetWindowFromID(UIWND_LEVELUPSPLASH).SetActive(FALSE);
                sgptUIGameGlobals->dwSplashTextTurnOffTime = 0;
                sgptUIGameGlobals->listNotifications.RemoveHead();
            }
        } else {

            UI_GetWindowFromID(UIWND_LEVELUPSPLASH).SetActive(TRUE);

            CRefString sLevel = LocalizedSprintf(StringListFetch("H2OLayout:LevelUp"), UnicodeText::FormatNumber(tData.nLevel, TRUE, FNS_NORMAL, XLOCALE_INVALID).str(), NULL);
            UI_GetTextFromID(UITEXT_LEVELUPSPLASH).SetText(sLevel, FALSE);

            UIStoryboard & tStoryboard = UI_GetStoryboardFromID(UISTORYBOARD_LEVELUPSPLASH);
            tStoryboard.Play(FALSE, 0.0f);
            sgptUIGameGlobals->dwSplashTextTurnOffTime = CGameGetClientTicks() + (uint32)(tStoryboard.GetDuration() * TICKS_PER_SECOND);
        }
        break;













    case NT_LEVELUP_NEW_SKILL_SLOT:
        if (!sgptUIGameGlobals->dwSplashTextTurnOffTime && tData.nLevel == 2) {



            UIID idSkillButton = UIIDCreateFromParent(UIWND_CONSOLE_HUD_PLAYER_SLOTS, CRefString().Format(".Player%d.Local.SkillsParent.Skills.Slot6", 1));
            gtTutorialMgr.ShowTutorial(idSkillButton, GlobalSNOGet(GLOBALSNO_TUTORIAL_LEVEL2_SKILL_UNLOCK), NULL, TRUE);

            Player * ptPlayer = LocalPlayerGet();
            gtTutorialMgr.QueueTutorial(UIHUD::Console::PlayerGetResourceUIControl(ptPlayer), sGetClassResourceTutorial(ptPlayer->ePlayerClassPortrait), 0, -1);
        }


        sHandleBasicNotification(UIWND_LEVELUP_NEWSKILLSLOT, UISTORYBOARD_LEVELUP_NEWSKILLSLOT);
        break;


    case NT_LEVELUP_NEW_TRAIT_SLOT:

        sHandleBasicNotification(UIWND_LEVELUP_NEWTRAITSLOT, UISTORYBOARD_LEVELUP_NEWTRAITSLOT);
        break;





    case NT_LEVELUP_NEW_SKILL:



        if (tData.nLevel == 3 && sgptUIGameGlobals->dwSplashTextTurnOffTime && CGameGetClientTicks() > sgptUIGameGlobals->dwSplashTextTurnOffTime) {

            gtTutorialMgr.ShowTutorial(GetUIIDNone(), GlobalSNOGet(GLOBALSNO_TUTORIAL_LEVEL3_SKILL_UNLOCK), NULL, TRUE);
        }

        if (PlayerGetLocalPlayerCount() == 1) {

            sConsoleProcessPlayerNotification(tData);
        }

        sHandleSkillNotification(UIWND_LEVELUP_NEWSKILL, UISTORYBOARD_LEVELUP_NEWSKILL, UITEXT_LEVELUP_NEWSKILL, UIHOTBARBTN_LEVELUP_NEWSKILL_ICON, tData.snoPower);
        break;





    case NT_LEVELUP_NEW_RUNE:

        if (PlayerGetLocalPlayerCount() == 1) {

            sConsoleProcessPlayerNotification(tData);
        }

        sHandleRuneNotification(UIWND_LEVELUP_NEWRUNE, UISTORYBOARD_LEVELUP_NEWRUNE, UITEXT_LEVELUP_NEWRUNE_TITLE, UITEXT_LEVELUP_NEWRUNE, UIWND_LEVELUP_NEWRUNE_ICON, tData.snoPower, tData.eRuneType);
        break;


    case NT_LEVELUP_NEW_TRAIT:


        sHandleSkillNotification(UIWND_LEVELUP_NEWTRAIT, UISTORYBOARD_LEVELUP_NEWTRAIT, UITEXT_LEVELUP_NEWTRAIT, UIHOTBARBTN_LEVELUP_NEWTRAIT_ICON, tData.snoPower);
        break;


    case NT_SAVEPOINT_SPLASH:

        sHandleBasicNotification(UIWND_SAVEPOINTSPLASH, UISTORYBOARD_SAVEPOINTSPLASH);
        break;
    case NT_ALT_LEVELUP_SPLASH:


        if (sgptUIGameGlobals->dwSplashTextTurnOffTime) {

            if (CGameGetClientTicks() > sgptUIGameGlobals->dwSplashTextTurnOffTime) {

                UI_GetWindowFromID(UIWND_ALTLEVELUPSPLASH).SetActive(FALSE);
                sgptUIGameGlobals->dwSplashTextTurnOffTime = 0;
                sgptUIGameGlobals->listNotifications.RemoveHead();
            }
        } else {

            UI_GetWindowFromID(UIWND_ALTLEVELUPSPLASH).SetActive(TRUE);

            CRefString sLevel = LocalizedSprintf(StringListFetch("H2OLayout:AltLevelUp"), UnicodeText::FormatNumber(tData.nLevel, TRUE, FNS_NORMAL, XLOCALE_INVALID).str(), NULL);
            UI_GetTextFromID(UITEXT_ALTLEVELUPSPLASH).SetText(sLevel, FALSE);

            UIStoryboard & tStoryboard = UI_GetStoryboardFromID(UISTORYBOARD_LEVELUPSPLASH);
            tStoryboard.Play(FALSE, 0.0f);
            sgptUIGameGlobals->dwSplashTextTurnOffTime = CGameGetClientTicks() + (uint32)(tStoryboard.GetDuration() * TICKS_PER_SECOND);
        }
        break;

    case NT_ALT_LEVELUP_REWARDS:
        if (sgptUIGameGlobals->dwSplashTextTurnOffTime) {

            if (CGameGetClientTicks() > sgptUIGameGlobals->dwSplashTextTurnOffTime) {

                UI_GetWindowFromID(UIWND_ALTLEVELUP_REWARDS).SetActive(FALSE);
                sgptUIGameGlobals->dwSplashTextTurnOffTime = 0;
                sgptUIGameGlobals->listNotifications.RemoveHead();
            }
        } else {

            UI_GetWindowFromID(UIWND_ALTLEVELUP_REWARDS).SetActive(TRUE);

            float flMFBonusFromAltLevel = 0.03f;
            CRefString usMFBonus = CRefString("+") + UnicodeText::FormatNumber(flMFBonusFromAltLevel, TRUE, FNS_PERCENT, XLOCALE_INVALID, 0, 0);
            CRefString usGFBonus = usMFBonus;
            CRefString sLevel = LocalizedSprintf(StringListFetch("H2OLayout:AltLevelUpRewards"), usMFBonus.str(), usGFBonus.str(), NULL);
            UIText & tText = UI_GetTextFromID(UITEXT_ALTLEVELUP_REWARDS);




            CRefString sRewards;
            StringTokenizer tTokenizer(sLevel, "\n\r");
            char szToken[128];
            while (tTokenizer.GetNextToken(szToken, sizeof(szToken))) {

                sRewards.Format("%s%s ", sRewards.str(), szToken);
            }
            sLevel = sRewards;

            tText.SetText(sLevel, FALSE);
            UIStoryboard & tStoryboard = UI_GetStoryboardFromID(UISTORYBOARD_LEVELUPSPLASH);
            tStoryboard.Play(FALSE, 0.0f);
            sgptUIGameGlobals->dwSplashTextTurnOffTime = CGameGetClientTicks() + (uint32)(tStoryboard.GetDuration() * TICKS_PER_SECOND);
        }
        break;

    case NT_ALT_LEVELUP_NEW_PORTRAIT:
        if (sgptUIGameGlobals->dwSplashTextTurnOffTime) {


            if (CGameGetClientTicks() > sgptUIGameGlobals->dwSplashTextTurnOffTime) {

                UI_GetWindowFromID(UIWND_ALTLEVELUP_NEWPORTRAIT).SetActive(FALSE);
                sgptUIGameGlobals->dwSplashTextTurnOffTime = 0;
                sgptUIGameGlobals->listNotifications.RemoveHead();
            }
        } else {

            UI_GetWindowFromID(UIWND_ALTLEVELUP_NEWPORTRAIT).SetActive(TRUE);

            ImageTextureFrame tFrame;
            CPlayerGetPortraitImage(LocalPlayerGet(), tFrame, FALSE);
            UI_GetWindowFromID(UIWND_ALTLEVELUP_NEWPORTRAIT_PORTRAIT).SetTextureFrame(tFrame);

            GBID gbidPortrait = PlayerGetPortraitGBIDFromParagonLevel(tData.nLevel);
            UI_GetWindowFromID(UIWND_ALTLEVELUP_NEWPORTRAIT_BKGRND).SetTextureFrame(UIPortraits::GetPortraitBackgroundImage(gbidPortrait));

            UIWindow & tWndBanner = UI_GetWindowFromID(UIWND_ALTLEVELUP_NEWPORTRAIT_BANNER);
            if (tData.nLevel > 0) {

                tWndBanner.SetTextureFrame(UIPortraits::GetPortraitBannerImage(gbidPortrait));
                tWndBanner.SetActive(TRUE);
            } else {
                tWndBanner.SetActive(FALSE);
            }


            UI_GetWindowFromID(UIWND_ALTLEVELUP_NEWPORTRAIT_FRAME).SetTextureFrame(UIPortraits::GetPortraitFrameImage(gbidPortrait));

            UIStoryboard & tStoryboard = UI_GetStoryboardFromID(UISTORYBOARD_LEVELUPSPLASH);
            tStoryboard.Play(FALSE, 0.0f);
            sgptUIGameGlobals->dwSplashTextTurnOffTime = CGameGetClientTicks() + (uint32)(tStoryboard.GetDuration() * TICKS_PER_SECOND);
        }
        break;















    case NT_TRANSMOG_REWARD:

        sHandleTransmogRewardNotification(UIWND_TRANSMOG_REWARD, UISTORYBOARD_TRANSMOG_REWARD, UITEXT_TRANSMOG_REWARD_NAME, UIWND_TRANSMOG_REWARD_ICON, tData.gbidTransmog);

        if (PlayerGetLocalPlayerCount() == 1) {
            sConsoleProcessPlayerNotification(tData);
        }
        break;




    case NT_EXTRACTED_POWER:

        sHandleExtractedPowerNotification(UIWND_POWER_EXTRACTION_REWARD, UISTORYBOARD_POWER_EXTRACTION_REWARD, UITEXT_POWER_EXTRACTION_REWARD_NAME, UIWND_POWER_EXTRACTION_REWARD_ICON, UIWND_POWER_EXTRACTION_REWARD_ICON_SMALL, tData.gbidExtractedPower);
        if (PlayerGetLocalPlayerCount() == 1) {
            sConsoleProcessPlayerNotification(tData);
        }
        break;
















    case NT_NEW_CRAFTER:
    case NT_NEW_HIRELING:

        sgptUIGameGlobals->listNotifications.RemoveHead();
        break;
    default:
        break;
    }
}





BOOL UIGame::IsShowingNotification()
{
    // ...
    return sgptUIGameGlobals->listNotifications.Count() > 0;
}




void UIGame::ShowAltLevelUpButton(ACDID idACDPlayer)
{
    ActorCommonData & tACDPlayer = ACDGet(idACDPlayer);
    int32 nNewLevel = tACDPlayer.AttributesGetInt(ATTRIBUTE_ALT_LEVEL);
    // ...
    if (GameIsWeeklyChallenge())
    {
        // ...
        // ...
        // ...
        // ...
        // ...
        return;
    }
    // ...
    NotificationData tData;
    tData.eType = NT_ALT_LEVELUP_SPLASH;
    tData.nLevel = nNewLevel;
    tData.nPlayerIndex = PlayerGetByACD(idACDPlayer)->nPlayerIndex;
    sgptUIGameGlobals->listNotifications.AddTail_Tracked(tData);
    // ...
    // ...
    // ...
    if (nNewLevel > 0)
    {
        if ((nNewLevel <= 100 && (nNewLevel == 1 || nNewLevel % 10 == 0)) ||
            (nNewLevel <= 800 && nNewLevel % 100 == 0))
        {
            // ...
            tData.eType = NT_ALT_LEVELUP_NEW_PORTRAIT;
            sgptUIGameGlobals->listNotifications.AddTail_Tracked(tData);
        }
    }
    // ...
    sgptUIGameGlobals->fShowParagonPointsButton = TRUE;
}

void UIGame::ShowLevelUpButton(
    ACDID idACDPlayer)
{
    ActorCommonData & tACDPlayer = ACDGet(idACDPlayer);
    int32 nNewLevel = tACDPlayer.AttributesGetInt(ATTRIBUTE_LEVEL);

    NotificationData tData;
    tData.eType = NT_LEVELUP_SPLASH;
    tData.nLevel = nNewLevel;

    tData.nPlayerIndex = PlayerGetByACD(idACDPlayer)->nPlayerIndex;

    sgptUIGameGlobals->listNotifications.AddTail_Tracked(tData);

    if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
    {
        return;
    }



    for (int32 i = 0; i < 6; ++i)
    {
        if (cgnActiveSkillSlotLevelReq[i] == nNewLevel)
        {
            NotificationData tDataScoped;
            tDataScoped.eType = NT_LEVELUP_NEW_SKILL_SLOT;
            tDataScoped.nLevel = nNewLevel;

            tDataScoped.nPlayerIndex = PlayerGetByACD(idACDPlayer)->nPlayerIndex;

            sgptUIGameGlobals->listNotifications.AddTail_Tracked(tDataScoped);

            sgptUIGameGlobals->fShowNewSkillButton = TRUE;
            break;
        }
    }


    for (int32 i = 0; i < 4; ++i)
    {
        if (cgnTraitSlotLevelReq[i] == nNewLevel)
        {
            NotificationData tDataScoped;
            tDataScoped.eType = NT_LEVELUP_NEW_TRAIT_SLOT;
            tDataScoped.nLevel = nNewLevel;

            tDataScoped.nPlayerIndex = PlayerGetByACD(idACDPlayer)->nPlayerIndex;

            sgptUIGameGlobals->listNotifications.AddTail_Tracked(tDataScoped);

            sgptUIGameGlobals->fShowNewSkillButton = TRUE;
            break;
        }
    }

    SNO snoSkillKit = tACDPlayer.GetSkillKit();
    ScopedSkillKitDefinitionLockR pdefSkillKit(snoSkillKit);


    {
        int32 nCount = pdefSkillKit->serActiveSkillEntries.dwSize / sizeof(ActiveSkillEntry);
        for (int32 i = 0; i < nCount; ++i)
        {
            ActiveSkillEntry & tSkill = pdefSkillKit->arActiveSkillEntries[i];

            if (tSkill.snoPower != -1 &&
                PowerIsDisplayed(AutoPowerDefinitionR(tSkill.snoPower)))
            {
                if (tSkill.nMasteryReq == nNewLevel)
                {
                    NotificationData tDataScoped;
                    tDataScoped.eType = NT_LEVELUP_NEW_SKILL;
                    tDataScoped.nLevel = nNewLevel;
                    tDataScoped.snoPower = tSkill.snoPower;

                    tDataScoped.nPlayerIndex = PlayerGetByACD(idACDPlayer)->nPlayerIndex;
                    UIFastEquip::Console::ReportNewSkill(PlayerGetByPlayerIndex(tDataScoped.nPlayerIndex), tDataScoped.snoPower);

                    sgptUIGameGlobals->listNotifications.AddTail_Tracked(tDataScoped);
                }


                for (int32 nRune = 0; nRune < 5; ++nRune)
                {
                    if (tSkill.GetRuneLevelRequirement((RuneType)nRune) == nNewLevel)
                    {
                        NotificationData tDataScoped;
                        tDataScoped.eType = NT_LEVELUP_NEW_RUNE;
                        tDataScoped.nLevel = nNewLevel;
                        tDataScoped.snoPower = tSkill.snoPower;
                        tDataScoped.eRuneType = (RuneType)nRune;

                        tDataScoped.nPlayerIndex = PlayerGetByACD(idACDPlayer)->nPlayerIndex;
                        UIFastEquip::Console::ReportNewRune(PlayerGetByPlayerIndex(tDataScoped.nPlayerIndex), tDataScoped.snoPower, tDataScoped.eRuneType);

                        sgptUIGameGlobals->listNotifications.AddTail_Tracked(tDataScoped);
                    }
                }
            }
        }
    }

    {
        int32 nCount = pdefSkillKit->serTraitEntries.dwSize / sizeof(TraitEntry);
        for (int32 i = 0; i < nCount; ++i)
        {
            TraitEntry & tSkill = pdefSkillKit->arTraitEntries[i];

            if (tSkill.snoPower != -1 &&
                PowerIsDisplayed(AutoPowerDefinitionR(tSkill.snoPower)) &&
                tSkill.nMasteryReq == nNewLevel)
            {
                NotificationData tDataScoped;
                tDataScoped.eType = NT_LEVELUP_NEW_TRAIT;
                tDataScoped.snoPower = tSkill.snoPower;

                tDataScoped.nPlayerIndex = PlayerGetByACD(idACDPlayer)->nPlayerIndex;

                sgptUIGameGlobals->listNotifications.AddTail_Tracked(tDataScoped);
            }
        }
    }
}














void UIGame::ShowSavePointButton()
{
    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_GAME_SAVED), cgtPSODefaults);
    // ...
    // ...
    // ...
    NotificationData tData;
    tData.eType = NT_SAVEPOINT_SPLASH;
    tData.nPlayerIndex = LocalPlayerGet()->nPlayerIndex;
    // ...
    sgptUIGameGlobals->listNotifications.AddTail_Tracked(tData);
}



void UIGame::ShowTransmogRewardNotification(ACDID idACDPlayer, GBID gbidTransmog)
{
    // ...
    // ...
    // ...
    // ...
    // ...
    // ...
    // ...
    // ...
    // ...
    // ...
    // ...
    // ...
    // ...
    // ...
    NotificationData tData;
    tData.eType = NT_TRANSMOG_REWARD;
    tData.gbidTransmog = gbidTransmog;
    tData.nPlayerIndex = PlayerGetByACD(idACDPlayer)->nPlayerIndex;
    // ...
    // ...
    sgptUIGameGlobals->listNotifications.AddTail_Tracked(tData);
}






















void UIGame::ShowNewCrafterNotification(Player * ptPlayer, CrafterType eCrafterType)
{
    // ...
    // ...
    NotificationData tData;
    tData.eType = NT_NEW_CRAFTER;
    tData.eCrafterType = eCrafterType;
    // ...
    tData.nPlayerIndex = ptPlayer->nPlayerIndex;
    // ...
    // ...
    sgptUIGameGlobals->listNotifications.AddTail_Tracked(tData);
}





void UIGame::ShowNewHirelingNotification(Player * ptPlayer, HirelingClass eHirelingClass)
{
    // ...
    // ...
    NotificationData tData;
    tData.eType = NT_NEW_HIRELING;
    tData.eHirelingClass = eHirelingClass;
    // ...
    tData.nPlayerIndex = ptPlayer->nPlayerIndex;
    // ...
    // ...
    sgptUIGameGlobals->listNotifications.AddTail_Tracked(tData);
}



















static CRefString sGetMonsterResistanceLine(
    const ActorCommonData & tACD)
{
    CRefString sResistances;
    LPCSTR uszSpacer = "  ";
    int32 nCount = 0;
    for (int32 nDamageType = 0; nDamageType < NUM_DAMAGE_TYPES; ++nDamageType)
    {
        BOOL fImmune = tACD.AttributesGetInt(MakeAttribKey(ATTRIBUTE_IMMUNITY, nDamageType));
        CRefString sLabel;
        if (fImmune)
        {
            sLabel = CRefString().Format("Immune_%s", SymbolToName(nDamageType, gDamageTypes));
        }
        else
        {
            float flResistanceTotal = tACD.AttributesGetFloat(MakeAttribKey(ATTRIBUTE_RESISTANCE_TOTAL, nDamageType));
            if (flResistanceTotal > 0.000001f)
            {
                sLabel = CRefString().Format("Resistance_%s", SymbolToName(nDamageType, gDamageTypes));
            }
        }

        if (sLabel.Length())
        {
            if (nCount > 0)
            {
                sResistances += uszSpacer;
            }
            sResistances += StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GENERAL), sLabel.str());
            ++nCount;
        }
    }

    return sResistances;
}



static CRefString sGetMonsterAffixLine(
    const ActorCommonData & tACD,
    BOOL bShowImmunities)
{
    CRefString usAffixLine;
    GBHandle hGB;
    hGB.eType = GB_MONSTER_AFFIXES;

    SNO snoStringList = GlobalSNOGet(GLOBALSNO_STRINGLIST_MONSTER_AFFIXES);
    int32 nCount = 0;
    LPCSTR uszSpacer = "  ";

    for (int32 i = 0; i < NUM_MONSTER_AFFIXES; ++i)
    {
        hGB.gbid = tACD.pMonsterAffixGBIDs[i];
        if (hGB.gbid == GBID_NONE)
            break;



        const GameBalanceDefinition * pdefGB;
        const MonsterAffixesEntry * ptAffix = static_cast<const MonsterAffixesEntry *>(GBLockR(hGB, pdefGB, TRUE));

        BOOL bIsImmunity = ptAffix->eAffixType == AT_IMMUNITY;






        if (ptAffix->bShown && (bShowImmunities || !bIsImmunity))
        {

            CRefString usAffix;
            BOOL bMinionNameFound = FALSE;
            if (tACD.eMonsterRarity == MR_RARE_MINION)
            {

                CRefString usMinionName;
                usMinionName.Format("%s_Minion", (char *)ptAffix->tHeader.szName);
                bMinionNameFound = StringListFetch(snoStringList, usMinionName, usAffix);
            }
            if (!bMinionNameFound)
            {
                StringListFetch(snoStringList, ptAffix->tHeader.szName, usAffix);
            }

            if (nCount > 0)
            {
                usAffixLine += uszSpacer;
            }

            usAffixLine += usAffix;
            ++nCount;
        }

        GBUnlockR(pdefGB);
    }

    if (tACD.eMonsterRarity == MR_RARE_MINION)
    {

        if (nCount > 0)
        {
            usAffixLine += uszSpacer;
        }

        CRefString usMinion;
        StringListFetch(snoStringList, "Minion", usMinion);
        usAffixLine += usMinion;

        ++nCount;
    }

    if (tACD.eMonsterRarity == MR_NORMAL)
    {
        MonsterType eType = MonsterGetMonsterType(tACD.tSNOName);
        if (eType > MT_INVALID && eType < NUM_MONSTER_TYPE)
        {
            CRefString usType;
            const char * pszTypeName = SymbolToName(eType, gMonsterTypes);
            SNO snoTypeStringList = GlobalSNOGet(GLOBALSNO_STRINGLIST_MONSTER_TYPES);
            StringListFetch(snoTypeStringList, pszTypeName, usType);

            if (nCount > 0)
            {
                usAffixLine += uszSpacer;
            }
            usAffixLine += usType;
        }

        ++nCount;
    }

    if (nCount > 0)
        usAffixLine += "\n";

    return usAffixLine;
}






static void sSetHPBarTexture(
    UIProgressBar & tProgressBar,
    const ActorCommonData & tACD,
    const ActorCommonData & tACDPlayer)
{
    UIWindow & tForeground = UI_GetWindowFromID(tProgressBar.GetForegroundChild());
    if (GameTypeIsPvP())
    {

        if (ACDIsFriendly(tACD, tACDPlayer, FALSE))
        {

            tForeground.SetTextureFrame("UI:PVP_HealthFill_Blue");
        }
        else
        {

            tForeground.SetTextureFrame("UI:PVP_HealthFill_Red");
        }


        const float PLAYER_BAR_WIDTH = 120.0f;
        const float PLAYER_BAR_HEIGHT = 20.0f;
        const float PET_BAR_WIDTH = 60.0f;
        const float PET_BAR_HEIGHT = 10.0f;



        if (!ACDIsPlayer(tACD.id))
        {
            const RGBAColor crgbaPetMask(128, 128, 128, 255);
            if (!tACD.IsMirrorImage())
            {
                Rect2D tRectNew(0.0f, 0.0f, PET_BAR_WIDTH, PET_BAR_HEIGHT);
                tProgressBar.SetWindowRect(tRectNew);

                tForeground.SetWindowColor(crgbaPetMask);
            }
            else if (ACDIsFriendly(tACD, tACDPlayer, FALSE))
            {

                Rect2D tRectNew(0.0f, 0.0f, PET_BAR_WIDTH, PET_BAR_HEIGHT);
                tProgressBar.SetWindowRect(tRectNew);
                tForeground.SetWindowColor(crgbaPetMask);
            }
            else
            {

                Rect2D tRectNew(0.0f, 0.0f, PLAYER_BAR_WIDTH, PLAYER_BAR_HEIGHT);
                tProgressBar.SetWindowRect(tRectNew);
                tForeground.SetWindowColor(crgbaWhite);
            }
        }
        else
        {

            Rect2D tRectNew(0.0f, 0.0f, PLAYER_BAR_WIDTH, PLAYER_BAR_HEIGHT);
            tProgressBar.SetWindowRect(tRectNew);

            tForeground.SetWindowColor(crgbaWhite);
        }
    }
    else
    {
        if (ACDIsFriendly(tACDPlayer, tACD, FALSE) ||

            tACD.IsDueling())
        {


            tForeground.SetTextureFrame("UI:PartyHealthBar4");
        }
        else
        {
            tForeground.SetTextureFrame("UI:PartyHealthBar2");
        }
    }
}




static void sUpdateHPBarForACD(
    ActorCommonData & tACD,
    int32 & nIndex,
    BOOL bShowName,
    BOOL bShowBar,
    BOOL & bActivateProgressBar,
    BOOL & bActivateText)
{



    if (nIndex >= 256)
    {
        return;
    }

    float flCurHP = tACD.GetHitPointsCurrent();
    if (flCurHP < 0.000001f)
    {
        return;
    }

    float flMaxHP = tACD.GetHitPointsMaxTotal();
    BOOL fAtFullHealth = flCurHP >= flMaxHP;

    if (tACD.AttributesGetInt(ATTRIBUTE_ROOT_TARGET_ACD) != -1)
    {
        flMaxHP = 1.0f;
        flCurHP = tACD.AttributesGetFloat(ATTRIBUTE_ROOT_UNIT_VALUE);
    }


    BOOL bHostileMirrorImage = FALSE;
    ActorCommonData * ptACDPlayer = ACDTryToGet(LocalPlayerACDGet());
    if (ptACDPlayer &&
        (GameTypeIsPvP() || ptACDPlayer->IsDueling()) &&
        tACD.IsMirrorImage() &&
        !ACDIsFriendly(tACD, *ptACDPlayer, FALSE))
    {
        bHostileMirrorImage = TRUE;
    }

    if (!ACDIsPlayer(tACD.id) && !bHostileMirrorImage)
    {
        bShowName = FALSE;
    }











    flMaxHP = Math::Max(flMaxHP, 0.0f);
    flCurHP = Math::Pin(flCurHP, 0.0f, flMaxHP);

    Vector2D wpStatusUIC;
    {

        Vector3D wpStatus;
        BOOL fFallback = TRUE;
        RActor * ptRActor = RActorTryToGet((RActorID)ACDGetOwner(tACD.id));
        if (ptRActor)
        {
            int32 nHPName = HardpointIndexFromName(ptRActor->snoAppearance, HardpointHash(AURA_HARDPOINT_NAME));
            if (nHPName != -1)
            {
                PRTransform transformHP;
                RActorGetHardpointTransformByIndex(*ptRActor, nHPName, transformHP);
                wpStatus = transformHP.wp;
                fFallback = FALSE;
            }
        }

        if (fFallback)
        {
            const WorldLocation & loc = tACD.GetWorldLocation();
            wpStatus = loc.transform.wp;
            wpStatus.z += loc.wcylBounds.wdHeight + 1.0f;
        }

        BOOL bResult = WorldPointToScreenPointUIC(wpStatus, wpStatusUIC, -1);
        if (!bResult)
        {
            return;
        }
    }

    UIHPBar & tHPBar = sgptUIGameGlobals->tUIHPBar[nIndex++];
    bActivateProgressBar = bShowBar;

    if (!ptACDPlayer)
    {
        return;
    }

    if (UI_TestID(tHPBar.idProgressBar))
    {

        UIProgressBar & tProgressBarMonster = UI_GetProgressBarFromID(tHPBar.idProgressBar);
        sSetHPBarTexture(tProgressBarMonster, tACD, *ptACDPlayer);

        Vector2D vecTL;
        Vector2D vecDim;
        tProgressBarMonster.GetRelativePosition(vecTL, vecDim);
        tProgressBarMonster.SetRelativePosition(vecTL, vecDim);

        UIWindow & tWndWrapper = UI_GetWindowFromID(tHPBar.idWndWrapper);
        Rect2D rectWrapper;
        tWndWrapper.GetWindowRect(rectWrapper);

        Vector2D vecDelta(
            wpStatusUIC.x - (rectWrapper.left + rectWrapper.Width() * 0.5f),
            wpStatusUIC.y - (rectWrapper.top + rectWrapper.Height() * 0.5f));
        tWndWrapper.Move(vecDelta);

        tProgressBarMonster.SetExtentsAndPosition(0.0f, flMaxHP * 100.0f, flCurHP * 100.0f, -1.0f);
    }

    BOOL bShowHPBarNumbers = Prefs::GetGameplayPrefs().GetShowHPBarNumbers();
    if (bShowHPBarNumbers || bShowName)
    {
        UIText & tHealthText = UI_GetTextFromID(tHPBar.idText);
        bActivateText = TRUE;

        CRefString usText;
        if (bShowName)
        {
            Player * ptPlayer = PlayerGetByACD(ACDGetControllingActor(tACD.id));
            if (ptPlayer)
            {
                usText = CPlayerGetServiceName(*ptPlayer, FALSE) + CRefString(" ");
            }
        }

        if (bShowHPBarNumbers)
        {

            int64 nCurHP = Math::Max((int64)flCurHP, (int64)1);
            int64 nMaxHP = (int64)flMaxHP;

            if (bHostileMirrorImage)
            {
                Player * ptPlayerOwner = tACD.GetPetOwnerPlayer();
                if (ptPlayerOwner)
                {
                    ActorCommonData * ptACDOwner = ACDTryToGet(ptPlayerOwner->idPlayerACD);
                    if (ptACDOwner)
                    {
                        nMaxHP = Math::FloatToInt(ptACDOwner->GetHitPointsMaxTotal());
                        nCurHP = Math::Max(Math::FloatToInt((float)nMaxHP * tACD.GetHitpointPercent()), 1);
                    }
                }
            }

            CRefString strHPs;
            strHPs.Format("(%s / %s)", UnicodeText::FormatNumber(nCurHP, TRUE, FNS_NORMAL, FALSE).str(), UnicodeText::FormatNumber(nMaxHP, TRUE, FNS_NORMAL, FALSE).str());

            usText += strHPs;
        }

        tHealthText.SetText(usText, FALSE);

        if (GameTypeIsPvP())
        {
            tHealthText.SetTextColor(crgbaWhite);
        }


        Vector2D tTextTL; Vector2D tTextDim;
        tHealthText.GetRelativePosition(tTextTL, tTextDim);

        Vector2D pCenterUIC(wpStatusUIC.x, wpStatusUIC.y - 10.0f);
        Rect2D rectTextUIC;
        rectTextUIC.Set(
            pCenterUIC.x - tTextDim.x * 0.5f,
            pCenterUIC.y - tTextDim.y,
            pCenterUIC.x + tTextDim.x * 0.5f,
            pCenterUIC.y);

        tHealthText.SetWindowRect(rectTextUIC);
    }
}



static void sUpdatedCachedHPBarForACD(
    int32 & nIndex,
    ActorCommonData * ptACD,
    BOOL fShowName,
    BOOL fShowHPBar)
{
    int32 i = nIndex;
    BOOL bProgressBarState = FALSE;
    BOOL bTextState = FALSE;
    sUpdateHPBarForACD(*ptACD, nIndex, fShowName, fShowHPBar, bProgressBarState, bTextState);

    if (i != nIndex)
    {
        UIHPBar * ptUIHPBars = sgptUIGameGlobals->tUIHPBar;
        UIHPBar & tHPBar = ptUIHPBars[i];

        tHPBar.bProgressBarState = bProgressBarState;
        tHPBar.bTextState = bTextState;

        if (UI_TestID(tHPBar.idProgressBar))
        {
            UI_GetProgressBarFromID(tHPBar.idProgressBar).SetActive(bProgressBarState);
        }
        if (UI_TestID(tHPBar.idText))
        {
            UI_GetTextFromID(tHPBar.idText).SetActive(bTextState);
        }
    }
}





void UpdatePlayersPetsFloatingHPBars(RActor * ptPlayerRActor, int32 & nIndex)
{
    // ...
    // ...
    int32 nPos = 0;
    ACDID idACDPet = PlayerGetFirstPet(ptPlayerRActor->idACD, -1, nPos);
    while (idACDPet != ACDID_INVALID)
    {
        ActorCommonData * ptACDPet = ACDTryToGet(idACDPet);
        if (ptACDPet)
        {
            ScopedMonsterDefinitionLockR pdefMonster(ptACDPet->GetMonsterSNO());
            if (pdefMonster->bShowHealthBarWithOwnerOnConsole)
            {
                sUpdatedCachedHPBarForACD(nIndex, ptACDPet, FALSE, TRUE);
            }
        }
        idACDPet = PlayerGetNextPet(ptPlayerRActor->idACD, -1, nPos);
    }
}


static void sUIUpdateFloatingHPBars()
{

    UIGlobals *& ptUIGlobals = CGameGet().ptUIGlobals;
    if (!ptUIGlobals)
        return;



    ACDID idACDLocalPlayerGet = LocalPlayerACDGet();
    ActorCommonData * ptACDLocalPlayerGet = ACDTryToGet(idACDLocalPlayerGet);
    if (!ptACDLocalPlayerGet)
        return;



    BOOL bShowMonsterBars = Prefs::GetGameplayPrefs().GetShowMonsterHPBars();
    bShowMonsterBars |= GameIsParamFlagSet(GPF_SHOW_MONSTER_HP_BARS);




    BOOL bShowPlayerHPBars = Prefs::GetGameplayPrefs().GetShowPlayerHPBars();
    BOOL bShowPlayerName = Prefs::GetGameplayPrefs().GetShowPlayerNames();

    int32 nIndex = 0;
    if (GameTypeIsPvP())
    {
        ACDIterator tIt;
        while (ActorCommonDataIterate(tIt))
        {
            ActorCommonData * ptACD = tIt.ptElement;


            if (!ptACD)
                continue;



            RActorID idRActor = RActorID(ACDGetOwner(ptACD->id));
            RActor * ptRActor = RActorTryToGet(idRActor);

            if (!ptRActor || RActorIsHidden(ptRActor))
                continue;



            BOOL fShowHPBar = FALSE;
            BOOL fShowName = FALSE;

            if (ptACD->IsTargetable() &&
                !ptACD->IsStealthed())
            {
                fShowHPBar = !(ptRActor->dwVisibilityFlags & (1 << RACTOR_NOT_IN_LOS_BIT));
            }




            if (ACDIsPlayer(ptRActor->idACD))
            {
                if (!ptACD->IsObserver())
                {
                    fShowHPBar = TRUE;
                    fShowName = TRUE;

                    UpdatePlayersPetsFloatingHPBars(ptRActor, nIndex);
                }
            }



            if (!fShowName && !fShowHPBar)
                continue;



            sUpdatedCachedHPBarForACD(nIndex, ptACD, fShowName, fShowHPBar);
        }
    }
    else
    {

        if (bShowMonsterBars)
        {
            ACDIterator tIt;
            while (ActorCommonDataIterate(tIt))
            {
                ActorCommonData * ptACD = tIt.ptElement;


                if (!ptACD)
                    continue;



                RActorID idRActor = RActorID(ACDGetOwner(ptACD->id));
                RActor * ptRActor = RActorTryToGet(idRActor);

                if (!ptRActor || RActorIsHidden(ptRActor))
                    continue;



                if (ptACD->IsNecroCorpse())
                    continue;



                BOOL fShowHPBar =

                    ACDIsHostile(*ptACDLocalPlayerGet, *ptACD, FALSE) &&
                    ptACD->IsTargetable() &&
                    !ptACD->IsStealthed() &&

                    !(ptRActor->dwVisibilityFlags & (1 << RACTOR_NOT_IN_LOS_BIT)) &&

                    ptACD->IsMonster() &&



                    ptACD->snoActor != GlobalSNOGet(GLOBALSNO_ACTOR_BELIAL_HUGE_FORM);








                if (fShowHPBar)
                {
                    sUpdatedCachedHPBarForACD(nIndex, ptACD, FALSE, fShowHPBar);
                }
            }
        }

        if (bShowPlayerName || bShowPlayerHPBars)
        {
            for (const Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
            {
                ActorCommonData * ptACD = ACDTryToGet(ptPlayer->idPlayerACD);


                if (!ptACD)
                    continue;


                RActorID idRActor = RActorID(ACDGetOwner(ptACD->id));
                RActor * ptRActor = RActorTryToGet(idRActor);

                if (!ptRActor || RActorIsHidden(ptRActor))
                    continue;



                BOOL fShowHPBar = FALSE;
                BOOL fShowName = FALSE;
                Player * ptLocalPlayer = LocalPlayerGetPrimary();

                if (PlayersAreHostileToEachOther(ptLocalPlayer, ptPlayer))
                {
                    fShowHPBar = !ACDIsInTown(*ptACD);
                    fShowName = TRUE;
                }





                if ((ACDIsFriendly(*ptACD, *ptACDLocalPlayerGet, FALSE) ||

                    (ptACD->IsDueling() && (PlayerIsLocal(ptPlayer) || !(ptRActor->dwVisibilityFlags & (1 << RACTOR_NOT_IN_LOS_BIT))))) &&



                    !ptACD->IsObserver())
                {
                    fShowHPBar = bShowPlayerHPBars;
                    fShowName = bShowPlayerName;
                }
















                if (fShowName || fShowHPBar)
                {
                    sUpdatedCachedHPBarForACD(nIndex, ptACD, fShowName, fShowHPBar);
                }


                if (fShowHPBar && PlayerIsLocal(ptPlayer->nPlayerIndex))
                {
                    UpdatePlayersPetsFloatingHPBars(ptRActor, nIndex);
                }
            }
        }
    }


    UIHPBar * ptUIHPBars = sgptUIGameGlobals->tUIHPBar;

    for (int32 i = nIndex; i < 256; ++i)
    {
        UIHPBar & tHPBar = ptUIHPBars[i];

        if (tHPBar.bProgressBarState)
        {
            if (UI_TestID(tHPBar.idProgressBar))
            {
                UI_GetProgressBarFromID(tHPBar.idProgressBar).SetActive(FALSE);
            }
        }

        if (tHPBar.bTextState)
        {
            if (UI_TestID(tHPBar.idText))
            {
                UI_GetTextFromID(tHPBar.idText).SetActive(FALSE);
            }
        }
    }
}




static void sUIUpdateMainMonsterHPBar()
{

    UIGlobals *& ptUIGlobals = CGameGet().ptUIGlobals;
    if (!ptUIGlobals)
    {
        return;
    }

    ACDID idACD = ACDID_INVALID;
    BOOL fDisplay = FALSE;
    ActorCommonData * ptACDPlayer = ACDTryToGet(LocalPlayerACDGet());
    if (ptACDPlayer)
    {
        ActorCommonData * ptACDMonster = ACDTryToGet(ptUIGlobals->idACDBossHPBar);
        if (ptACDMonster && ptACDMonster->IsAlive())
        {
            idACD = ptUIGlobals->idACDBossHPBar;
            fDisplay = TRUE;
        }
        else
        {
            UIGame::SetBossHPBar(ACDID_INVALID);
        }

        ptACDMonster = ACDTryToGet(ptUIGlobals->idACDMonsterHPBar);
        if (ptACDMonster && ptACDMonster->IsAlive())
        {
            idACD = ptUIGlobals->idACDMonsterHPBar;
            fDisplay = TRUE;
        }
        else
        {
            UIGame::SetMonsterHPBar(ACDID_INVALID);
        }

        if (ACDIsPlayer(ptUIGlobals->idACDMonsterHPBar) &&
            !ptACDPlayer->IsDueling() && !GameTypeIsPvP())
        {

            UIGame::SetMonsterHPBar(ACDID_INVALID);
            fDisplay = FALSE;
        }







    }

    if (fDisplay)
    {
        fDisplay = !Console::Screens::IsScreenSetOpen();
    }



    BOOL bAvenger = FALSE;

    UIID idHPBarToUse = UIWND_HP_BAR_NORMAL;
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (!ptACD)
    {
        fDisplay = FALSE;
    }
    else
    {

        bAvenger = ptACD->IsAvenger();

        if (GameTypeIsPvP() && UI_TestID(UIWND_HP_BAR_PVP))
        {
            idHPBarToUse = UIWND_HP_BAR_PVP;
        }
        else if (ptACD->IsMirrorImage())
        {
            idHPBarToUse = UIWND_HP_BAR_PLAYER;
        }
        else if (ptACD->AttributesGetInt(ATTRIBUTE_BOUNTY_OBJECTIVE))
        {
            idHPBarToUse = UIWND_HP_BAR_BOUNTY;
        }
        else if (ptACD->IsMonster())
        {
            switch (ptACD->eMonsterRarity)
            {
            case MR_CHAMPION:
            case MR_RARE_MINION:
                idHPBarToUse = UIWND_HP_BAR_MINION;
                break;
            case MR_RARE:
            case MR_BOSS:
                idHPBarToUse = UIWND_HP_BAR_SPECIAL;
                break;

            case MR_UNIQUE:
                idHPBarToUse = bAvenger ? UIWND_HP_BAR_AVENGER : UIWND_HP_BAR_SPECIAL;
                break;

            default:
                break;
            }
        }

        else if (ACDIsPlayer(idACD) && ptACDPlayer->IsDueling())
        {
            idHPBarToUse = UIWND_HP_BAR_PLAYER;
        }
    }


    UI_GetWindowFromID(UIWND_HP_BAR_MINION).SetActive(fDisplay && idHPBarToUse == UIWND_HP_BAR_MINION);
    UI_GetWindowFromID(UIWND_HP_BAR_SPECIAL).SetActive(fDisplay && idHPBarToUse == UIWND_HP_BAR_SPECIAL);

    UI_GetWindowFromID(UIWND_HP_BAR_AVENGER).SetActive(fDisplay && idHPBarToUse == UIWND_HP_BAR_AVENGER);

    UI_GetWindowFromID(UIWND_HP_BAR_NORMAL).SetActive(fDisplay && idHPBarToUse == UIWND_HP_BAR_NORMAL);

    if (UI_TestID(UIWND_HP_BAR_PLAYER))
    {
        UI_GetWindowFromID(UIWND_HP_BAR_PLAYER).SetActive(fDisplay && idHPBarToUse == UIWND_HP_BAR_PLAYER);
    }

    if (UI_TestID(UIWND_HP_BAR_PVP))
    {
        UI_GetWindowFromID(UIWND_HP_BAR_PVP).SetActive(fDisplay && idHPBarToUse == UIWND_HP_BAR_PVP);
    }

    if (UI_TestID(UIWND_HP_BAR_BOUNTY))
    {
        UI_GetWindowFromID(UIWND_HP_BAR_BOUNTY).SetActive(fDisplay && idHPBarToUse == UIWND_HP_BAR_BOUNTY);
    }

    if (!fDisplay)
    {
        return;
    }

    UIID idWndProgressBar = UIIDCreateFromParent(idHPBarToUse, ".progressbar");
    UIID idTxtName = UIIDCreateFromParent(idHPBarToUse, ".name");
    UIID idTxtAffixes = UIIDCreateFromParent(idHPBarToUse, ".affixes");

    UIProgressBar & tProgressBarMonster = UI_GetProgressBarFromID(idWndProgressBar);
    UIText & tTextMonsterHPBar = UI_GetTextFromID(idTxtName);
    UIText & tTextMonsterAffixes = UI_GetTextFromID(idTxtAffixes);

    if (!GameTypeIsPvP())
    {
        tTextMonsterAffixes.SetActive(TRUE);
    }
    else
    {
        tTextMonsterAffixes.SetActive(FALSE);
    }

    BOOL bForceFlavorText = FALSE;
    BOOL bBossMonsterHasRandomAffixes = FALSE;
    CRefString usName;
    if (ACDIsPlayer(idACD))
    {
        Player * ptPlayer = PlayerGetByACD(idACD);
        usName = CPlayerGetServiceName(*ptPlayer, FALSE);
    }
    else if (ptACD->IsMirrorImage())
    {
        Player * ptOwnerPlayer = ptACD->GetPetOwnerPlayer();
        if (ptOwnerPlayer)
        {
            usName = CPlayerGetServiceName(*ptOwnerPlayer, FALSE);
        }
        else
        {
            usName = MonsterGetName(ptACD, FALSE);
        }
    }
    else if (ptACD->IsMonster())
    {
        usName = MonsterGetName(ptACD, FALSE);

        SNOName tMonsterSNOName;
        if (MonsterGetSNONameFromActor(ptACD->snoActor, tMonsterSNOName))
        {
            ScopedMonsterDefinitionLockR pdefMonster(tMonsterSNOName.snoHandle);


            bForceFlavorText = (pdefMonster->dwFlags >> MONSTERDEF_FORCES_FLAVOR_TEXT_BIT) & 1;


            if (ptACD->eMonsterRarity == MR_BOSS)
            {
                bBossMonsterHasRandomAffixes = pdefMonster->nRandomAffixes > 0;
            }

        }
    }

    tTextMonsterHPBar.SetText(usName, FALSE);
    RGBAColor rgbaColor = crgbaWhite;

    if (ptACD->IsMonster())
    {
        switch (ptACD->eMonsterRarity)
        {

        default:

            rgbaColor = crgbaWhite;
            break;

        case MR_CHAMPION:
            rgbaColor = FontGetColorForFontSpecifier(FONTCOLOR_BONUS, FALSE);
            break;

        case MR_RARE:
            rgbaColor = crgbaYellow;
            break;

        case MR_UNIQUE:
        case MR_BOSS:
            rgbaColor = FontGetColorForFontSpecifier(FONTCOLOR_EPIC, FALSE);
            break;
        }
    }

    tTextMonsterHPBar.SetTextColor(rgbaColor);


    float flCurHP = ptACD->GetHitPointsCurrent();
    float flMaxHP = ptACD->GetHitPointsMaxTotal();

    float flFrac = flCurHP / flMaxHP;
    flFrac = Math::Max(0.01f, flFrac);
    tProgressBarMonster.SetExtentsAndPosition(0.0f, 1.0f, flFrac, -1.0f);

    if (ptACD->eMonsterRarity == MR_UNIQUE ||
        (ptACD->eMonsterRarity == MR_BOSS && !bBossMonsterHasRandomAffixes) || bForceFlavorText)
    {




        ScopedActorDefinitionLockR pdefActor(ptACD->tSNOName.snoHandle);


        CRefString sFlavor;

        if (ptACD->IsAvenger())
        {
            uint32 nNumberOfKills = ptACD->ptAvengerData->arVictims.size();
            for (uint32 nKill = 0; nKill < 5; ++nKill)
            {
                UIID idKillIcon = UIIDCreateFromParent(idHPBarToUse, CRefString().Format(".AvengerKill%i", nKill + 1));
                UIWindow & tWndVictim = UI_GetWindowFromID(idKillIcon);
                BOOL bShowVictim = nKill < nNumberOfKills;
                tWndVictim.SetActive(bShowVictim);
                if (bShowVictim)
                {
                    AvengerVictim & tVictim = ptACD->ptAvengerData->arVictims[nKill];
                    UIID idPlayerPortraits = UIIDCreateFromParent(idKillIcon, ".PlayerPortraits");
                    int32 nIconPathIndex = (tVictim.eClass << 1) | !tVictim.bIsFemale;
                    Avenger::Console::ShowOnly(idPlayerPortraits, nIconPathIndex);
                }
            }
            CRefString sMonsterDescription;
            AvengerData * ptAvengerData = ptACD->ptAvengerData;
            if (nNumberOfKills && ptAvengerData->sName.Length())
            {
                sMonsterDescription = LocalizedSprintf(
                    StringListFetch("X1_ConsoleAvengers:HealthBarDescription"),
                    ptAvengerData->sName.str(), NULL);
            }

            else if (nNumberOfKills && ptAvengerData->eAvengerSource == AVENGER_SOURCE_FRIEND && PlayerIsLocal(ptAvengerData->nPlayerIndex) && PlayerGetAvengerGiftRecipientName().Length())
            {

                sMonsterDescription = LocalizedSprintf(
                    StringListFetch("X1_ConsoleAvengers:HealthBarDescription"),
                    PlayerGetAvengerGiftRecipientName().str(), NULL);
            }
            else
            {

                sMonsterDescription = StringListFetch("X1_ConsoleAvengers:HealthBarDescription_FirstSighting");
            }
            CRefString sAvengerName = StringListFetch("X1_ConsoleAvengers:HealthBarName");
            UIID idAvengerDescription = UIIDCreateFromParent(idHPBarToUse, ".affixesNemesis");
            tTextMonsterAffixes.SetText("", FALSE);
            UI_GetTextFromID(idAvengerDescription).SetText(sMonsterDescription, FALSE);
            tTextMonsterHPBar.SetText(sAvengerName, FALSE);
        }


        else if (MonsterGetFlavor(pdefActor->snoMonster, sFlavor))
        {
            tTextMonsterAffixes.SetText(sFlavor, FALSE);
        }
        else
        {
            tTextMonsterAffixes.SetText("", FALSE);
        }
    }

    else if (!ptACD->IsMirrorImage())
    {

        CRefString sAffixes = sGetMonsterAffixLine(*ptACD, FALSE);
        tTextMonsterAffixes.SetText(sAffixes, FALSE);
    }


    if (!tTextMonsterAffixes.GetText().Length())
    {

        CRefString sResistances = sGetMonsterResistanceLine(*ptACD);
        tTextMonsterAffixes.SetText(sResistances, FALSE);
    }
}



void UIGame::UpdateACDTags()
{
    // ...
    POS posControls = sgptUIGameGlobals->listActorTagControls.GetHeadPosition();
    while (posControls)
    {
        POS posCurrent = posControls;
        UIID idControl = sgptUIGameGlobals->listActorTagControls.GetNext(posControls);
        UIActorTag & tActorTag = UI_GetActorTagFromID(idControl);
        uint32 dwExpireTick = tActorTag.GetExpirationTime();
        if (dwExpireTick < CGameGetClientTicks())
        {
            UI_DeleteControl(idControl, TRUE);
            sgptUIGameGlobals->listActorTagControls.RemoveAt(posCurrent);
        }
    }
}














static void sDisplayGameText(
    POS & pos,
    UIText & tTextLine,
    float flFrac)
{
    if (!pos)
    {
        tTextLine.SetActive(FALSE);
        return;
    }

    GameTextParams & tTextParams = sgptUIGameGlobals->ptGameTextList->GetNext(pos);

    tTextLine.SetActive(TRUE);
    tTextLine.SetText(tTextParams.uszText, FALSE);
    tTextLine.SetFont(GlobalSNOGet(CWorldIsRetro() ? GLOBALSNO_FONT_EXOCETLIGHT : GLOBALSNO_FONT_SANS_SERIF));

    RGBAColor rgbaColor = crgbaWhite;
    rgbaColor.a = (uint8)Math::FloatToInt(flFrac * 255.0f);
    tTextLine.SetWindowColor(rgbaColor);
    tTextLine.SetTextColor(crgbaWhite);
}



static void sGameTextUpdate()
{



    const int32 MAX_GAME_TEXT_LINES = 2;
    UIText * ptTextLines[MAX_GAME_TEXT_LINES] = {
        &UI_GetTextFromID(UITEXT_GAMETEXT_LINE0),
        &UI_GetTextFromID(UITEXT_GAMETEXT_LINE1)
    };

    if (sgptUIGameGlobals->ptGameTextList->IsEmpty())
    {

        for (int32 i = 0; i < MAX_GAME_TEXT_LINES; ++i)
        {
            ptTextLines[i]->SetActive(FALSE);
        }
        return;
    }


    float flLineFrac[MAX_GAME_TEXT_LINES];
    SigmaMemoryClear(flLineFrac, sizeof(flLineFrac));

    int32 nIndex = 0;
    uint32 dwCurrentTimestamp = CGameGetRealTicks();
    POS pos = sgptUIGameGlobals->ptGameTextList->GetHeadPosition();
    while (pos)
    {
        POS posLast = pos;
        GameTextParams & tTextParams = sgptUIGameGlobals->ptGameTextList->GetNext(pos);

        uint32 dwTicksElapsed = dwCurrentTimestamp - tTextParams.dwStateTimestamp;

        float flFrac;
        FadeState eFadeState = scgtGameTextFadeRamp.Evaluate(dwTicksElapsed, flFrac);
        if (eFadeState == FADESTATE_HIDDEN)
        {
            sgptUIGameGlobals->ptGameTextList->RemoveAt(posLast);
        }
        else
        {
            if (nIndex < MAX_GAME_TEXT_LINES)
            {
                flLineFrac[nIndex] = flFrac;
            }
            ++nIndex;
        }
    }

    pos = sgptUIGameGlobals->ptGameTextList->GetHeadPosition();
    for (int32 i = 0; i < MAX_GAME_TEXT_LINES; ++i)
    {
        sDisplayGameText(pos, *ptTextLines[i], flLineFrac[i]);
    }
}



static void sErrorTextUpdate()
{

    ErrorTextParams & tTextParams = sgptUIGameGlobals->tErrorText;











    UIID idErrorText = tTextParams.fDisplayBnetError ? UITEXT_BNET_GAMEERROR : UITEXT_GAMEERROR;
    UIText & tText = UI_GetTextFromID(idErrorText);


    UIID idOtherErrorText = tTextParams.fDisplayBnetError ? UITEXT_GAMEERROR : UITEXT_BNET_GAMEERROR;
    UI_GetTextFromID(idOtherErrorText).SetActive(FALSE);

    uint32 dwTicksElapsed = CGameGetRealTicks() - tTextParams.dwTimestamp;

    RGBAColor rgbaColor = crgbaWhite;
    float flFrac = 0.0f;
    FadeState eFadeState;

    if (UI_DrawingFullscreenUI() ||

        CGameInScriptedSequence() ||
        Console::Screens::IsScreenSetOpen(Console::Screens::OPTIONS_SCREEN_SET))
    {
        eFadeState = FADESTATE_HIDDEN;
    }



    else
    {
        eFadeState = scgtErrorTextFadeRamp.Evaluate(dwTicksElapsed, flFrac);
    }

    if (eFadeState == FADESTATE_HIDDEN)
    {
        tText.SetActive(FALSE);
    }
    else
    {
        rgbaColor.a = (uint8)Math::FloatToInt(flFrac * 255.0f);
        tText.SetActive(TRUE);
        tText.SetWindowColor(rgbaColor);
        tText.SetTextColor(FontGetColorForFontSpecifier(FONTCOLOR_RED, FALSE));
        tText.SetText(tTextParams.uszText, FALSE);
    }



    UIWindow & tBkgrnd = UI_GetWindowFromID(UIWND_GAMEERROR_BKGRND);
    if (eFadeState != FADESTATE_HIDDEN)
    {
        tBkgrnd.SetWindowColor(rgbaColor);
        tBkgrnd.SetActive(TRUE);
    }
    else
    {
        tBkgrnd.SetActive(FALSE);
    }
}





void UIGame::UpdateCalloutDisplay(CalloutEvent * ptEvent, const uint8 bOpacity)
{
    // ...
    // ...
    // ...
    // ...
    // ...
    // ...
}







































































































































































































void sSetRActorTooltip(const RActor * ptRActor, UIText & tTag)
{
    // ...
    // ...
    // ...
    Vector3D wp;
    RActorGetTopPoint(ptRActor, wp);
    // ...
    // ...
    wp.z += 4.0f;
    // ...
    // ...
    // ...
    Vector2D wpUIC;
    if (!WorldPointToScreenPointUICUnclipped(wp, wpUIC, -1))
        return;
    // ...
    // ...
    // ...
    Player * ptBannerPlayer = NULL;
    CRefString usTooltipText;
    // ...
    if (!RActorGetTooltip(ptRActor, usTooltipText, ptBannerPlayer))
        return;
    if (usTooltipText.Length() == 0 && !ptBannerPlayer)
        return;
    // ...
    // ...
    // ...
    // ...
    // ...
    Rect2D rectPosition;
    rectPosition.left = wpUIC.x; rectPosition.top = wpUIC.y;
    // ...
    rectPosition.right = wpUIC.x; rectPosition.bottom = wpUIC.y;
    // ...
    // ...
    tTag.SetActive(TRUE);
    tTag.SetFont(GlobalSNOGet(CWorldIsRetro() ? cgeDefaultFontForRetroText : cgeDefaultFontForInGameText));
    // ...
    // ...
    // ...
    // ...
    // ...
    // ...
    // ...
    // ...
    // ...
    // ...
    // ...
    // ...
    // ...
    // ...
    tTag.SetText(usTooltipText, FALSE);
    // ...
    Vector2D tTextSizeUIC;
    tTag.GetTextRenderedSizeUIC(tTextSizeUIC, FALSE);
    // ...
    // ...
    // ...
    // ...
    // ...
    float flPad = 12.0f;
    tTextSizeUIC.x += flPad;
    tTextSizeUIC.y += flPad;
    // ...
    // ...
    Rect2D rectTextUIC;
    rectTextUIC.Set(0.0f, 0.0f, tTextSizeUIC.x, tTextSizeUIC.y);
    rectTextUIC.AlignToRect(rectPosition, AP_HORIZONTAL_CENTER, AP_VERTICAL_ABOVE, 0.0f);
    // ...
    // ...
    UI_ConstrainRectToViewport(rectTextUIC, 10.0f);
    // ...
    tTag.SetWindowRect(rectTextUIC);
    // ...
    if (GfxStereo3DEnabled())
    {
        Vector3D wpCamera = CameraManagerGetCamera(0).cmState.transform.wp;
        float flDist = Vector3D::Distance(wp, wpCamera);
        tTag.SetStereoDepth(flDist);
    } }
void sSetRActorTooltip(
    const RActor * ptRActor,
    const Player * ptPlayer,
    UIWindow & tWindow,
    UIText & tText,
    UIRActor & tUIRActor,
    EffectGroupID & idEffectGroup)
{


    Vector3D wp;
    RActorGetTopPoint(ptRActor, wp);
    wp.z += 4.0f;

    Vector2D wpUIC;
    if (!WorldPointToScreenPointUICUnclipped(wp, wpUIC, -1)) {
        tText.SetActive(FALSE);
        tUIRActor.SetActive(FALSE);
        return;
    }


    Player * ptBannerPlayer = NULL;
    CRefString usTooltipText;

    BOOL fUIRActorActive = FALSE;
    if (RActorGetTooltip(ptRActor, usTooltipText, ptBannerPlayer) &&
        (usTooltipText.Length() || ptBannerPlayer)) {

        tWindow.SetActive(TRUE);
        tText.SetFontBoostSize(4);
        tText.SetActive(TRUE);

        if (ptBannerPlayer) {

            if (PlayerIsLocal(ptBannerPlayer)) {

                tText.SetText(PlayerGetHeroDisplayName(*ptBannerPlayer, FALSE), FALSE);

            } else {

                tText.SetText(LocalizedSprintf(StringListFetch("Console:BannerPlayerName"), CPlayerGetIndicatorString(*ptBannerPlayer).str(), PlayerGetHeroDisplayName(*ptBannerPlayer, FALSE).str(), NULL), FALSE);
            }

        } else {

            if (PlayerGetLocalPlayerCount() > 1) {

                ActorCommonData * ptACD = ACDTryToGet(ptRActor->idACD);
                if (ptACD) {

                    int32 nPlayerIndex = ItemGetPlayerRecipient(*ptACD);

                    if (nPlayerIndex != -1) {

                        Player * ptOwnerPlayer = PlayerGetByPlayerIndex(nPlayerIndex);
                        if (ptOwnerPlayer && PlayerIsLocal(ptOwnerPlayer)) {

                            usTooltipText = LocalizedSprintf(StringListFetch("ConsoleUI:PlayerOwnerTooltip"), CPlayerGetIndicatorString(*ptOwnerPlayer).str(), usTooltipText.str(), NULL);
                        }
                    }
                }
            }

            tText.SetFont(GlobalSNOGet(CWorldIsRetro() ? cgeDefaultFontForRetroText : cgeDefaultFontForInGameText));
            tText.SetText(usTooltipText, FALSE);
        }

    } else {

        tText.SetActive(FALSE);
    }

    if (ptPlayer) {

        ActorCommonData * ptACD = ACDTryToGet(ptRActor->idACD);
        ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);

        if (ptACD && ptACDPlayer && ShouldShowPrompt(ptACD, ptACDPlayer)) {

            tWindow.SetActive(TRUE);
            fUIRActorActive = TRUE;
            tUIRActor.SetActive(TRUE);

            if (idEffectGroup == EFFECTGROUPID_INVALID) {

                if (!InputPlatformAcceptButtonIsSwapped()) {


                    idEffectGroup = EffectGroupCreateTracked(GlobalSNOGet(GLOBALSNO_EFFECT_GROUP_INTERACT_ICON), tUIRActor.GetRActor(), RACTORID_INVALID, 0, FALSE);



                } else {


                    idEffectGroup = EffectGroupCreateTracked(GlobalSNOGet(GLOBALSNO_EFFECT_GROUP_INTERACT_ICON_NEUTRAL), tUIRActor.GetRActor(), RACTORID_INVALID, 0, FALSE);
                }
            }

        } else {

            if (idEffectGroup != EFFECTGROUPID_INVALID) {

                EffectGroupOutro(idEffectGroup);
                idEffectGroup = EFFECTGROUPID_INVALID;
            }
        }
    }

    if (!fUIRActorActive) {

        tUIRActor.SetActive(FALSE);
    }

    if (tWindow.IsActive()) {


        Rect2D rectPosition;
        rectPosition.left = wpUIC.x;
        rectPosition.top = wpUIC.y;
        rectPosition.right = wpUIC.x;
        rectPosition.bottom = wpUIC.y;


        if (tUIRActor.IsActive()) {

            rectPosition.bottom += tUIRActor.GetWindowRect().Height();
        }

        Rect2D rectUIC;
        tWindow.GetWindowRect(rectUIC);
        rectUIC.AlignToRect(rectPosition, AP_HORIZONTAL_CENTER, AP_VERTICAL_BOTTOM, 0.0f);


        UI_ConstrainRectToViewport(rectUIC, 10.0f);

        tWindow.SetWindowRect(rectUIC);

        if (GfxStereo3DEnabled()) {

            Vector3D wpCamera = CameraManagerGetCamera(0).cmState.transform.wp;
            float flDist = Vector3D::Distance(wp, wpCamera);
            tWindow.SetStereoDepth(flDist);
        }
    }
}





void UIGame::UpdateRActorHighlights()
{
    // ...
    // ...
    // ...
    UIGlobals *& ptUIGlobals = CGameGet().ptUIGlobals;
    if (!ptUIGlobals)
    {
        // ...
        return;
    }
    // ...
    // ...
    SetMonsterHPBar(ACDID_INVALID);
    // ...
    for (int32 i = 0; i < MAX_LOCAL_PLAYERS; i++)
    {
        RActorID idRActor = GetRActorHighlight(i);
        RActor * ptRActor = RActorTryToGet(idRActor);
        if (ptRActor)
        {
            RActorHighlight(*ptRActor, TRUE);
            // ...
            Player * ptPlayer = PlayerGetByControllerIndex(i);
            if (ptPlayer)
            {
                UIHUD::Console::UpdatePlayerTargetingIndicator(ptPlayer, ptRActor, TRUE);
                // ...
                if (ptUIGlobals->idACDMonsterHPBar == ACDID_INVALID)
                {
                    ActorCommonData * ptACD = ACDTryToGet(ptRActor->idACD);
                    ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
                    // ...
                    if (ptACD && ptACD->IsMonster() && ptACDPlayer && ACDIsHostile(*ptACDPlayer, *ptACD, FALSE))
                    {
                        // ...
                        if (PlayerGetLocalPlayerCount() == 1 || ptACD->IsElite() || ptACD->IsAvenger() || ptACD->eMonsterRarity == MR_UNIQUE)
                        {
                            // ...
                            SetMonsterHPBar(ptRActor->idACD);
                        }
                    }
                }
                // ...
                // ...
                // ...
                if (!ptPlayer->tTargetComponent.GetInAimMode())
                {
                    ActorCommonData * ptACDLockedTarget = ptPlayer->tTargetComponent.ResolveLockedTarget();
                    if (ptACDLockedTarget && ptPlayer->tTargetComponent.GetLockedTargetType() == ConsoleTargetComponent::TARGETLOCK_HARD)
                    {
                        ptACDLockedTarget->AddClientEffect(GetTargetLockSNOForPlayer(ptPlayer), 0);
                    }
                }
            }
        }
        // ...
        RActorID idRActorAlt = GetRActorAlternateHighlight(i);
        RActor * ptRActorAlt = RActorTryToGet(idRActorAlt);
        if (idRActorAlt != idRActor && ptRActorAlt)
        {
            RActorHighlight(*ptRActorAlt, TRUE);
        }
    }
    // ...
    // ...
}



void UIGame::UpdateRActorTooltips()
{
    // ...
    UIGlobals *& ptUIGlobals = CGameGet().ptUIGlobals;
    if (!ptUIGlobals)
    {
        // ...
        return;
    }
    // ...
    const UIID idTarget = UI_GetMousedOverControl();
    // ...
    if (idTarget != GetUIIDNone())
    {
        // ...
        if (UI_GetControlFromID(idTarget).IsType(UIT_WINDOW) &&
            UI_GetWindowFromID(idTarget).EatsClicks() == EC_EAT)
        {
            // ...
            for (int32 i = 0; i < MAX_LOCAL_PLAYERS; i++)
            {
                UIText & tTag = UI_GetTextFromID(sgptUIGameGlobals->idRActorTag[i]);
                tTag.SetActive(FALSE);
            }
            // ...
            return;
        }
    }
    // ...
    // ...
    for (int32 i = 0; i < MAX_LOCAL_PLAYERS; i++)
    {
        UIText & tTag = UI_GetTextFromID(sgptUIGameGlobals->idRActorTag[i]);
        tTag.SetActive(FALSE);
        // ...
        // ...
        UIWindow & tAltTag = UI_GetWindowFromID(sgptUIGameGlobals->idRActorAltTag[i]);
        tAltTag.SetActive(FALSE);
        // ...
        if (sgptUIGameGlobals->idRActorAlternateHighlight[i] == RACTORID_INVALID &&
            // ...
            // ...
            sgptUIGameGlobals->idRActorHighlight[i] == RACTORID_INVALID)
        {
            continue;
        }
        // ...
        // ...
        // ...
        // ...
        // ...
        // ...
        // ...
        // ...
        // ...
        // ...
        // ...
        // ...
        if (UINPC::Console::CachedInteractMenuIsActive() &&
            i == UI_GetPrimaryUIController())
        {
            continue;
        }
        // ...
        // ...
        // ...
        // ...
        // ...
        const RActor * ptRActor = RActorTryToGet(sgptUIGameGlobals->idRActorHighlight[i]);
        if (!ptRActor)
        {
            // ...
            sgptUIGameGlobals->idRActorHighlight[i] = RACTORID_INVALID;
        }
        else
        {
            // ...
            sSetRActorTooltip(ptRActor, tTag);
        }
        // ...
        // ...
        ptRActor = RActorTryToGet(sgptUIGameGlobals->idRActorAlternateHighlight[i]);
        if (!ptRActor)
        {
            sgptUIGameGlobals->idRActorAlternateHighlight[i] = RACTORID_INVALID;
        }
        else
        {
            // ...
            // ...
            UIID idText = UIIDCreateFromParent(tAltTag.GetID(), ".nameplate");
            UIText & tText = UI_GetTextFromID(idText);
            // ...
            UIID idUIRActor = UIIDCreateFromParent(tAltTag.GetID(), ".buttonprompt");
            UIRActor & tUIRActor = UI_GetRActorFromID(idUIRActor);
            // ...
            // ...
            if (tUIRActor.GetRActor() == RACTORID_INVALID)
            {
                // ...
                tUIRActor.CreateRActor(RENDERCONTEXT_HUD_0, GlobalSNOGet(GLOBALSNO_ACTOR_GENERIC_PROXY));
            }
            // ...
            Player * ptPlayer = PlayerGetByControllerIndex(i);
            sSetRActorTooltip(ptRActor, ptPlayer, tAltTag, tText, tUIRActor, sgptUIGameGlobals->idEffectButtonPrompt[i]);
        }
    }
    // ...
    // ...
}























































































































































void UIGame::ApplyActiveBuffButtonEffect(const Player * ptPlayer, const SkillButton eSkill, const HotbarButtonData & tButtonData)
{
    // ...
    // ...
    // ...
    if (!ptPlayer || !PlayerIsLocal(ptPlayer))
        return;
    // ...
    // ...
    // ...
    // ...
    int32 nHudSlot = UIHUD::Console::PlayerGetHUDSlot(ptPlayer);
    if (eSkill < 0 || eSkill >= UIGameGlobals::SKILL_BUTTON_COUNT || nHudSlot < 0 || nHudSlot >= MAX_LOCAL_PLAYERS)
        return;
    // ...
    // ...
    // ...
    // ...
    // ...
    // ...
    // ...
    // ...
    // ...
    BOOL bNeedsGlow = UIBuffs::BuffShowsActiveOnSkillButtons(tButtonData.m_snoPower, ptPlayer);
    UIID idBuffFX = sgptUIGameGlobals->idPlayerSkillGlow[nHudSlot][eSkill];
    UI_GetRActorFromID(idBuffFX).SetActive(bNeedsGlow);
}




void UIGame::ApplyFreeCastEffects(Player * ptPlayer, const int32 nSkillButtonIndex)
{
    // ...
    // ...
    UIHUD::Console::ApplyFreeCastEffects(ptPlayer, nSkillButtonIndex);
}




























































































































































































































































































































































































































































































































































































































static void sPositionProgressMarker(const UIProgressBar & tProgressBar, UIWindow & tMarkerWnd)
{
    // ...
    // ...
    Vector2D vecProgressBarTL, vecProgressBarDim;
    tProgressBar.GetRelativePosition(vecProgressBarTL, vecProgressBarDim);

    UIID idChild = tProgressBar.GetForegroundChild();
    if (UI_GetControlFromID(idChild).IsType(UIT_RACTOR))
    {
        UIRActor & tRActorBar = UI_GetRActorFromID(idChild);
        RActor * ptRActor = RActorTryToGet(tRActorBar.GetRActor());
        if (ptRActor)
        {
            float flScale = ptRActor->tObjTexAnimParam.flValue;
            vecProgressBarDim.x *= flScale;
        }
        else
        {
            // ...
            vecProgressBarDim.x = 0.0f;
        }
    }
    else
    {
        // ...
        // ...
        UIWindow & tBarWnd = UI_GetWindowFromID(idChild);
        tBarWnd.SetNoSnap(TRUE);
        Rect2D rectBar;
        tBarWnd.GetClippingRect(rectBar);
        // ...
        vecProgressBarDim.Set(rectBar.Width(), rectBar.Height());
    }
    // ...
    // ...
    IVector2D vecProgressBarDimPix;
    ConvertUICDimensionsToPixels(vecProgressBarDim, vecProgressBarDimPix);
    ConvertPixelsDimensionsToUIC(vecProgressBarDimPix, vecProgressBarDim);

    Vector2D vecMarkerTL, vecMarkerDim;
    tMarkerWnd.GetRelativePosition(vecMarkerTL, vecMarkerDim);
    vecMarkerTL.x = vecProgressBarTL.x + vecProgressBarDim.x;
    tMarkerWnd.SetRelativePosition(vecMarkerTL, vecMarkerDim); }
static CRefString sFormatElapsedTime(
    uint32 nTicks,
    BOOL fDisplayMilliseconds)
{
    ElapsedTimeDisplayOptions tElapsedTimeDisplayOptions;
    tElapsedTimeDisplayOptions.fAlwaysShowSeconds = tElapsedTimeDisplayOptions.fOnlyDisplayNumber = TRUE;

    tElapsedTimeDisplayOptions.fDisplayMilliseconds = fDisplayMilliseconds;
    return DisplayElapsedTime(uint32(0), nTicks, tElapsedTimeDisplayOptions);
}



static CRefString sFormatElapsedTime(
    const Blizzard::Time::Timestamp & tTime)
{
    ElapsedTimeDisplayOptions tElapsedTimeDisplayOptions;
    tElapsedTimeDisplayOptions.fAlwaysShowSeconds = tElapsedTimeDisplayOptions.fOnlyDisplayNumber = TRUE;

    tElapsedTimeDisplayOptions.fDisplayMilliseconds = TRUE;
    return DisplayElapsedTime(Blizzard::Time::Timestamp(0), tTime, tElapsedTimeDisplayOptions);
}












































































static void sUpdateRiftUI()
{

    UIProgressBar & tDungeonFinderProgressBar = UI_GetProgressBarFromID(UICONTROL_DUNGEON_FINDER_PROGRESSBAR);
    UIText & tDungeonFinderProgressValueText = UI_GetTextFromID(UITEXT_DUNGEON_FINDER_PROGRESS_VALUE);

    UIID idRActorBar;
    idRActorBar.SetFromParent(UICONTROL_DUNGEON_FINDER_PROGRESSBAR, "game_progressBarFlash_dungeonFinder");
    UIRActor & tRActorBar = UI_GetRActorFromID(idRActorBar);
    RActorChangeLook(tRActorBar.GetRActor(), "Look 1");

    CGameGlobals & tCGameGlobals = CGameGlobalsGet();
    float flDungeonFinderProgress = tCGameGlobals.flDungeonFinderProgress;

    ScopedGlobalDefinitionLockR pdefGlobal;
    float flMaxXPPercent = pdefGlobal->flLootRunXPPercentToComplete;

    float flProgress = flMaxXPPercent == 0.0f ? 0.0f : flDungeonFinderProgress / flMaxXPPercent;

    static int32 snPreviousPct = 0;
    if (flProgress > 0.999999f)
    {
        snPreviousPct = 0;
        UI_GetWindowFromID(UIWND_DUNGEON_FINDER_WRAPPER).SetActive(FALSE);
        UI_GetWindowFromID(UIWND_DUNGEON_FINDER_CONTAINER).SetActive(FALSE);
    }
    else
    {

        tDungeonFinderProgressBar.SetActive(TRUE);
        tDungeonFinderProgressBar.SetNoSnap(TRUE);

        tDungeonFinderProgressBar.SetExtentsAndPosition(0.0f, 1.0f, flProgress, -1.0f);

        gtTutorialMgr.ShowTutorial(UICONTROL_DUNGEON_FINDER_PROGRESSBAR, GlobalSNOGet(GLOBALSNO_TUTORIAL_LOOT_RUN_BAR), NULL, TRUE);

        int32 nProgressPercent = Math::FloatToInt(flProgress * 100.0f);
        if (flDungeonFinderProgress > 0.000001f)
        {
            nProgressPercent = Math::Max(nProgressPercent, 1);
        }

        const int32 BUCKET_SIZE = 10;
        if (snPreviousPct / BUCKET_SIZE != nProgressPercent / BUCKET_SIZE)
        {

            sAddFXToProgressBar(UICONTROL_DUNGEON_FINDER_PROGRESSBAR, GlobalSNOGet(GLOBALSNO_PARTICLE_DUNGEONFINDER_PROGRESS_UI_FX));
        }
        snPreviousPct = nProgressPercent;





        tDungeonFinderProgressValueText.SetActive(TRUE);
        tDungeonFinderProgressValueText.SetText(CRefString().Format("%d%%", nProgressPercent), FALSE);

        UI_GetWindowFromID(UIWND_DUNGEON_FINDER_WRAPPER).SetActive(TRUE);
        UI_GetWindowFromID(UIWND_DUNGEON_FINDER_CONTAINER).SetActive(TRUE);
        UI_GetWindowFromID(UIWND_CHALLENGE_RIFT_CONTAINER).SetActive(FALSE);
    }
}










static void sUpdateTrialRiftUI()
{

    int32 nTicksRemaining = CTimedEventGetRemainingTime(GlobalSNOGet(GLOBALSNO_TIMEDEVENT_TRIAL_RIFT_TIMER));
    if (nTicksRemaining >= 0 && !UICountDown::IsVisible())
    {
        UICountDown::Show(uint32(nTicksRemaining), StringListFetch("NephalemRift:TieredRiftTrialTimerText"), NULL, NULL, FALSE, FALSE, GlobalSNOGet(GLOBALSNO_TIMEDEVENT_TRIAL_RIFT_TIMER));
    }
}




static void sClipFlashEffect()
{



    UIProgressBar & tProgressBar = UI_GetProgressBarFromID(UIPROGRESS_TIERED_RIFT_PROGRESSBAR);
    UIID idForeground = tProgressBar.GetForegroundChild();
    UIWindow & tForegroundWnd = UI_GetWindowFromID(idForeground);
    UIWindow & tFlash = UI_GetWindowFromID(UIWND_TIERED_RIFT_FLASH);

    Rect2D rectProgress;
    if (tForegroundWnd.IsType(UIT_RACTOR))
    {
        RActor * ptRActor = RActorTryToGet(UI_GetRActorFromID(idForeground).GetRActor());
        if (ptRActor)
        {
            float flScale = ptRActor->tObjTexAnimParam.flValue;
            Vector2D vecTL, vecDim;
            tForegroundWnd.GetUnshiftedWindowRect(rectProgress);
            rectProgress.right = rectProgress.left + flScale * rectProgress.Width();
        }
    }
    else
    {
        tForegroundWnd.GetUnshiftedClippingRect(rectProgress);
    }

    Rect2D rectFlash;
    tFlash.GetUnshiftedClippingRect(rectFlash);
    rectFlash.right = rectProgress.left + rectProgress.Width() - 1.0f;
    tFlash.SetClipRect(rectFlash);
}




static void sSnapMarkerToPixels(
    UIWindow & tMarkerWnd)
{
    Rect2D rectWnd;
    tMarkerWnd.GetWindowRect(rectWnd);

    Vector2D vecMarkerUIC = rectWnd.Size();
    IVector2D vecMarkerPix;
    ConvertUICDimensionsToPixels(vecMarkerUIC, vecMarkerPix);
    ConvertPixelsDimensionsToUIC(vecMarkerPix, vecMarkerUIC);

    rectWnd.Set(rectWnd.left, rectWnd.top, rectWnd.left + vecMarkerUIC.x - 1.0f, rectWnd.top + vecMarkerUIC.y - 1.0f);
    tMarkerWnd.SetWindowRect(rectWnd);
}




static void sUpdateTieredRiftUI()
{

    BOOL bUseChallengeRiftUI = GameIsWeeklyChallenge();


    UIProgressBar & tDungeonFinderProgressBar = UI_GetProgressBarFromID(bUseChallengeRiftUI ? UIPROGRESS_CHALLENGE_RIFT_PROGRESSBAR : UIPROGRESS_TIERED_RIFT_PROGRESSBAR);
    tDungeonFinderProgressBar.SetNoSnap(TRUE);








    UIID idProgressRActorBar = UIIDCreateFromParent(tDungeonFinderProgressBar.GetID(), ".game_progressBarFlash_dungeonFinder");
    UIRActor & tProgressRActorBar = UI_GetRActorFromID(idProgressRActorBar);
    RActorChangeLook(tProgressRActorBar.GetRActor(), "Look 1");

    UIProgressBar & tDungeonFinderTimerBar = UI_GetProgressBarFromID(bUseChallengeRiftUI ? UIWND_CHALLENGE_RIFT_TIMER_PROGRESSBAR : UIWND_TIERED_RIFT_TIMER_PROGRESSBAR);

    UIID idTimerRActorBar = UIIDCreateFromParent(tDungeonFinderTimerBar.GetID(), ".timer_progressBarFlash_dungeonFinder");
    UIRActor & tTimerRActorBar = UI_GetRActorFromID(idTimerRActorBar);
    RActorChangeLook(tTimerRActorBar.GetRActor(), "Look 1");

    CGameGlobals & tCGameGlobals = CGameGlobalsGet();
    float flDungeonFinderProgress = tCGameGlobals.flDungeonFinderProgress;

    ScopedGlobalDefinitionLockR pdefGlobal;
    float flMaxXPPercent = pdefGlobal->flTieredLootRunXPPercentToComplete;

    UIWindow & tProgressMarker = UI_GetWindowFromID(bUseChallengeRiftUI ? UIWND_CHALLENGE_RIFT_PROGRESS_MARKER : UIWND_TIERED_RIFT_PROGRESS_MARKER);

    UI_GetWindowFromID(bUseChallengeRiftUI ? UIWND_TIERED_RIFT_PROGRESS_MARKER : UIWND_CHALLENGE_RIFT_PROGRESS_MARKER).SetActive(FALSE);

    UIWindow & tTimerMarker = UI_GetWindowFromID(bUseChallengeRiftUI ? UIWND_TIERED_RIFT_CHALLENGE_TIMER_MARKER : UIWND_TIERED_RIFT_TIMER_MARKER);
    tTimerMarker.SetActive(TRUE);

    UI_GetWindowFromID(bUseChallengeRiftUI ? UIWND_TIERED_RIFT_TIMER_MARKER : UIWND_TIERED_RIFT_CHALLENGE_TIMER_MARKER).SetActive(FALSE);

    UIWindow & tCompletionTimeMarker = UI_GetWindowFromID(bUseChallengeRiftUI ? UIWND_CHALLENGE_RIFT_COMPLETION_TIME_MARKER : UIWND_TIERED_RIFT_COMPLETION_TIME_MARKER);
    tCompletionTimeMarker.SetActive(FALSE);

    BOOL bBlizzcon = CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT);

    tProgressMarker.SetNoSnap(TRUE);
    tTimerMarker.SetNoSnap(TRUE);


    sSnapMarkerToPixels(tProgressMarker);
    sSnapMarkerToPixels(tTimerMarker);

    float flProgress = flMaxXPPercent == 0.0f ? 0.0f : flDungeonFinderProgress / flMaxXPPercent;

    static int32 snPreviousPct = 0;
    if (flProgress > 0.999999f)
    {
        snPreviousPct = 0;
        tDungeonFinderProgressBar.SetExtentsAndPosition(0.0f, 1.0f, 1.0f, -1.0f);
        sPositionProgressMarker(tDungeonFinderProgressBar, tProgressMarker);
    }
    else
    {










        gtTutorialMgr.ShowTutorial(UICONTROL_DUNGEON_FINDER_PROGRESSBAR, GlobalSNOGet(GLOBALSNO_TUTORIAL_LOOT_RUN_BAR), NULL, TRUE);

        tDungeonFinderProgressBar.SetExtentsAndPosition(0.0f, 1.0f, flProgress, -1.0f);

        sClipFlashEffect();


        sPositionProgressMarker(tDungeonFinderProgressBar, tProgressMarker);

        int32 nProgressPercent = Math::FloatToInt(flProgress * 100.0f);
        if (flDungeonFinderProgress > 0.000001f)
        {
            nProgressPercent = Math::Max(nProgressPercent, 1);
        }

        const int32 BUCKET_SIZE = 10;
        if (snPreviousPct / BUCKET_SIZE != nProgressPercent / BUCKET_SIZE)
        {

            if (bUseChallengeRiftUI)
            {
                sAddFXToProgressBar(UIPROGRESS_CHALLENGE_RIFT_PROGRESSBAR, GlobalSNOGet(GLOBALSNO_PARTICLE_CHALLENGE_RIFT_PROGRESS_UI_FX));
            }
            else
            {
                sAddFXToProgressBar(UIPROGRESS_TIERED_RIFT_PROGRESSBAR, GlobalSNOGet(GLOBALSNO_PARTICLE_TIERED_RIFT_PROGRESS_UI_FX));
            }
        }
        snPreviousPct = nProgressPercent;

        if (nProgressPercent >= 95)
        {
            CRefString sProgressText = LocalizedSprintf(StringListFetch("NephalemRift:PercentComplete").str(), UnicodeText::FormatNumber(nProgressPercent, 2, FNS_NORMAL, FALSE).str(), NULL);
            UI_GetTextFromID(bUseChallengeRiftUI ? UITEXT_CHALLENGE_RIFT_PROGRESS : UITEXT_TIERED_RIFT_PROGRESS).SetText(sProgressText, FALSE);
        }
        else
        {
            UI_GetTextFromID(bUseChallengeRiftUI ? UITEXT_CHALLENGE_RIFT_PROGRESS : UITEXT_TIERED_RIFT_PROGRESS).SetText("", FALSE);
        }
    }










    GameSyncedData & tGameSyncedData = GameGetActiveGameCommonData().tGameSyncedData;
    BOOL bHideTieredRiftUI = tGameSyncedData.nOpenLootRunLevel >= 0 && tGameSyncedData.bLootRunBossDead;
    BOOL bGameIsWeeklyChallenge = GameIsWeeklyChallenge();
    BOOL bPlayerEligibleForWeeklyChallengeReward = bGameIsWeeklyChallenge && ACDGet(LocalPlayerACDGet()).AttributesGetInt(ATTRIBUTE_ELIGIBLE_FOR_WEEKLY_CHALLENGE_REWARD);

    SNO snoTimedEvent = tCGameGlobals.snoDungeonFinderTimedEvent;
    if (snoTimedEvent != SNO_NONE && !bHideTieredRiftUI)
    {

        int32 nDurationTicks = CTimedEventGetDurationTicks(snoTimedEvent);

        int32 nRemainingTicks = Math::Min(CTimedEventGetRemainingTime(snoTimedEvent), nDurationTicks);
        if (!tGameSyncedData.bLootRunBossDead && !tGameSyncedData.bTieredLootRunFailed && !bBlizzcon)
        {

            if (nDurationTicks > 0 && nRemainingTicks <= 60 * TICKS_PER_SECOND && !UICountDown::IsVisible())
            {
                CRefString sDisplay = StringListFetch("NephalemRift:TieredRiftCountdownTitle");
                int32 nSecondsRemaining = nRemainingTicks / TICKS_PER_SECOND;
                UICountDown::Show(float(nSecondsRemaining), sDisplay.str(), NULL, NULL, FALSE, FALSE, snoTimedEvent);
            }
        }
        if (!sIsInTieredRift())
        {
            UICountDown::Close();
        }


        if (!bGameIsWeeklyChallenge && nRemainingTicks && (nRemainingTicks % TICKS_PER_SECOND == 0 && nRemainingTicks < 13 * 60 * TICKS_PER_SECOND))
        {
            if (!gtTutorialMgr.IsAnyShowing(0))
            {
                gtTutorialMgr.ShowTutorial(UITEXT_TIERED_RIFT_TIMER, GlobalSNOGet(GLOBALSNO_TUTORIAL_TIERED_LOOT_RUN_TIMER), NULL, TRUE);
            }
        }

        UI_GetWindowFromID(UIWND_TIERED_RIFT_BONUS_OBJECTIVE_WRAPPER).SetActive(!bGameIsWeeklyChallenge && !bBlizzcon);
        UI_GetWindowFromID(UIWND_WEEKLY_CHALLENGE_BONUS_OBJECTIVE_WRAPPER).SetActive(bGameIsWeeklyChallenge && bPlayerEligibleForWeeklyChallengeReward);
        UI_GetWindowFromID(UIWND_WEEKLY_CHALLENGE_SECONDARY_BONUS_OBJECTIVE_WRAPPER).SetActive(bGameIsWeeklyChallenge && !bPlayerEligibleForWeeklyChallengeReward);

        tDungeonFinderTimerBar.SetActive(TRUE);
        tDungeonFinderTimerBar.SetNoSnap(TRUE);

        UIID idTimerRActorBarScoped;
        idTimerRActorBarScoped.SetFromParent(bUseChallengeRiftUI ? UIWND_CHALLENGE_RIFT_TIMER_PROGRESSBAR : UIWND_TIERED_RIFT_TIMER_PROGRESSBAR, "timer_progressBarFlash_dungeonFinder");
        UIRActor & tTimerRActorBarScoped = UI_GetRActorFromID(idTimerRActorBarScoped);
        RActorChangeLook(tTimerRActorBarScoped.GetRActor(), "Look 1");

        float flPercentTimer;


        if (nDurationTicks > 0 && nRemainingTicks > 0)
        {
            uint32 nTicksSpent = nDurationTicks - nRemainingTicks;
            flPercentTimer = float(nTicksSpent) / nDurationTicks;
        }
        else
        {
            flPercentTimer = nDurationTicks == 0 ? 0.0f : 1.0f;
        }

        if (tGameSyncedData.bTieredLootRunFailed || (bGameIsWeeklyChallenge && nRemainingTicks == 0))
        {
            flPercentTimer = 1.0f;
        }


        UI_GetWindowFromID(UIWND_TIERED_RIFT_CHALLENGE_SPRINT).SetActive(FALSE);
        if (bGameIsWeeklyChallenge && sgptUIGameGlobals)

        {
            int32 nCompletionTicks = tCGameGlobals.nChallengeRiftCompletionMilliseconds / 1000u * TICKS_PER_SECOND;

            if (nCompletionTicks <= nDurationTicks)
            {
                float flCompletionTimePercent = float(nCompletionTicks) / nDurationTicks;
                tDungeonFinderTimerBar.SetExtentsAndPosition(0.0f, 1.0f, flCompletionTimePercent, -1.0f);
                sPositionProgressMarker(tDungeonFinderTimerBar, tCompletionTimeMarker);
            }

            int32 nProgressPercent = Math::FloatToInt(flProgress * 100.0f);
            UI_GetWindowFromID(UIWND_TIERED_RIFT_CHALLENGE_SPRINT).SetActive(nProgressPercent > PROGRESSION_PERCENT_DISPLAY_SPRINT_TIME);

            BOOL bCurrentlyAhead = tCGameGlobals.nChallengeRiftSprintMilliseconds >= 0.0f;

            UIStoryboard & tAheadFadeIn = UI_GetStoryboardFromID(UISTORYBOARD_TIERED_RIFT_CHALLENGE_SPRINT_AHEAD_FADEIN);
            UIStoryboard & tAheadFadeOut = UI_GetStoryboardFromID(UISTORYBOARD_TIERED_RIFT_CHALLENGE_SPRINT_AHEAD_FADEOUT);
            UIStoryboard & tBehindFadeIn = UI_GetStoryboardFromID(UISTORYBOARD_TIERED_RIFT_CHALLENGE_SPRINT_BEHIND_FADEIN);
            UIStoryboard & tBehindFadeOut = UI_GetStoryboardFromID(UISTORYBOARD_TIERED_RIFT_CHALLENGE_SPRINT_BEHIND_FADEOUT);


            if (nProgressPercent > PROGRESSION_PERCENT_DISPLAY_SPRINT_TIME)
            {
                UI_GetWindowFromID(UITEXT_TIERED_RIFT_CHALLENGE_SPRINT_BEHIND).SetActive(!bCurrentlyAhead);
                UI_GetWindowFromID(UITEXT_TIERED_RIFT_CHALLENGE_SPRINT_AHEAD).SetActive(bCurrentlyAhead);

                if (bCurrentlyAhead)
                {
                    Blizzard::Time::Timestamp tSprintMilliseconds = int64(tCGameGlobals.nChallengeRiftSprintMilliseconds) * TimeConstants::NANOSECONDS_IN_A_MILLISECOND;
                    CRefString sTime = sFormatElapsedTime(tSprintMilliseconds);
                    UI_GetTextFromID(UITEXT_TIERED_RIFT_CHALLENGE_SPRINT_AHEAD).SetText(sTime.str(), FALSE);


                    if (!sgptUIGameGlobals->bChallengeRiftWasAhead || !sgptUIGameGlobals->bPassed10Percent)
                    {
                        tAheadFadeIn.Play(FALSE, 0.0f);
                        tAheadFadeOut.Stop();
                        tBehindFadeIn.Stop();
                        if (UI_GetWindowFromID(UIWND_TIERED_RIFT_CHALLENGE_SPRINT_BEHIND_ICON).GetTransparency() == 255)
                        {
                            tBehindFadeOut.Play(FALSE, 0.0f);
                        }
                        else
                            UI_GetWindowFromID(UIWND_TIERED_RIFT_CHALLENGE_SPRINT_BEHIND_ICON).SetTransparency(0);
                    }
                }
                else
                {


                    Blizzard::Time::Timestamp tSprintMilliseconds = int64(abs(tCGameGlobals.nChallengeRiftSprintMilliseconds)) * TimeConstants::NANOSECONDS_IN_A_MILLISECOND;
                    CRefString sTime = sFormatElapsedTime(tSprintMilliseconds);
                    UI_GetTextFromID(UITEXT_TIERED_RIFT_CHALLENGE_SPRINT_BEHIND).SetText(sTime.str(), FALSE);


                    if (sgptUIGameGlobals->bChallengeRiftWasAhead || !sgptUIGameGlobals->bPassed10Percent)
                    {
                        tBehindFadeIn.Play(FALSE, 0.0f);
                        tBehindFadeOut.Stop();
                        if (sgptUIGameGlobals->bPassed10Percent)
                        {
                            tAheadFadeIn.Stop();
                            if (UI_GetWindowFromID(UIWND_TIERED_RIFT_CHALLENGE_SPRINT_AHEAD_ICON).GetTransparency() == 255)
                            {
                                tAheadFadeOut.Play(FALSE, 0.0f);
                            }
                            else
                            {
                                UI_GetWindowFromID(UIWND_TIERED_RIFT_CHALLENGE_SPRINT_AHEAD_ICON).SetTransparency(0);
                            }
                        }
                    }
                }

                sgptUIGameGlobals->bPassed10Percent = TRUE;
            }
            else
            {
                sgptUIGameGlobals->bPassed10Percent = FALSE;
                UI_GetWindowFromID(UIWND_TIERED_RIFT_CHALLENGE_SPRINT_AHEAD_ICON).SetTransparency(0);
                UI_GetWindowFromID(UIWND_TIERED_RIFT_CHALLENGE_SPRINT_BEHIND_ICON).SetTransparency(0);
            }


            int32 nDurationTicks = CTimedEventGetDurationTicks(snoTimedEvent);
            if (nDurationTicks > 0)
            {
                int32 nRemainingTicks = Math::Min(CTimedEventGetRemainingTime(snoTimedEvent), nDurationTicks);

                int32 nCurrentMilliseconds = (nDurationTicks - nRemainingTicks) * 1000u / TICKS_PER_SECOND;

                tCGameGlobals.nChallengeRiftSprintMilliseconds = tCGameGlobals.nChallengeRiftCompletionMilliseconds * (nProgressPercent * 0.01f) - nCurrentMilliseconds;

                if (sgptUIGameGlobals->bChallengeRiftWasAhead != bCurrentlyAhead)
                    sgptUIGameGlobals->bChallengeRiftWasAhead = bCurrentlyAhead;
            }
        }



        tDungeonFinderTimerBar.SetExtentsAndPosition(0.0f, 1.0f, flPercentTimer, -1.0f);


        if (!bBlizzcon)
        {
            CRefString sTimerText = sFormatElapsedTime(nRemainingTicks, FALSE);

            UI_GetTextFromID(UIIDCreateFromParent(UIWND_TIERED_RIFT_TIMER_MARKER, ".rift_timer_text")).SetText(sTimerText, FALSE);
        }





        sPositionProgressMarker(tDungeonFinderTimerBar, tTimerMarker);

        UI_GetWindowFromID(UIWND_DUNGEON_FINDER_WRAPPER).SetActive(TRUE);
        UI_GetWindowFromID(UIWND_CHALLENGE_RIFT_CONTAINER).SetActive(bUseChallengeRiftUI);
        UI_GetWindowFromID(UIWND_TIERED_RIFT_CONTAINER).SetActive(!bUseChallengeRiftUI);
    }
    else
    {

        UI_GetWindowFromID(UIWND_DUNGEON_FINDER_WRAPPER).SetActive(FALSE);
        UI_GetWindowFromID(UIWND_TIERED_RIFT_BONUS_OBJECTIVE_WRAPPER).SetActive(FALSE);
        UI_GetWindowFromID(UIWND_WEEKLY_CHALLENGE_BONUS_OBJECTIVE_WRAPPER).SetActive(FALSE);
        UI_GetWindowFromID(UIWND_WEEKLY_CHALLENGE_SECONDARY_BONUS_OBJECTIVE_WRAPPER).SetActive(FALSE);
        UI_GetWindowFromID(UIWND_TIERED_RIFT_CONTAINER).SetActive(FALSE);
        UI_GetWindowFromID(UIWND_CHALLENGE_RIFT_CONTAINER).SetActive(FALSE);


        if (CGameGetPredictedServerTime() >= CGameGet().ptUIGlobals->dwRiftClosingTick && (!tGameSyncedData.bLootRunBossDead || !tGameSyncedData.bTieredLootRunFailed))
            UICountDown::Close();
    }
}









static BOOL sIsInTrialRift()
{

    return CTimedEventsGetActiveEvent(GlobalSNOGet(GLOBALSNO_TIMEDEVENT_TRIAL_RIFT_TIMER)) != NULL;
}




static BOOL sIsInTieredRift()
{

    ActorCommonData * ptACDPlayer = ACDTryToGet(LocalPlayerACDGet());
    return ptACDPlayer && ACDIsInTieredLootRun(*ptACDPlayer);
}




static BOOL sIsInChallengeRift()
{

    ActorCommonData * ptACDPlayer = ACDTryToGet(LocalPlayerACDGet());
    return ptACDPlayer && ACDIsInChallengeRift(*ptACDPlayer);
}




static BOOL sIsInRift()
{

    SNO snoQuestDungeonFinder = GlobalSNOGet(GLOBALSNO_QUEST_DUNGEONFINDER);
    QuestInstance * ptDungeonFinderQuestInstance = QuestsGetQuestInstance(snoQuestDungeonFinder, ACDID_INVALID);
    SNO snoLevelArea = WorldGetLevelAreaSNOFromRActor(LocalPlayerRActorGet());
    BOOL bInDungeonFinderLevelArea = LevelAreaGetDungeonFinderLevel(snoLevelArea);
    const int32 DUNGEONFINDER_QUEST_ENTER_LOOTRUN_STEP_UID = 7;

    return ptDungeonFinderQuestInstance && bInDungeonFinderLevelArea &&
        ptDungeonFinderQuestInstance->nCurrentStepUID != -1 &&
        ptDungeonFinderQuestInstance->nCurrentStepUID != DUNGEONFINDER_QUEST_ENTER_LOOTRUN_STEP_UID &&
        !QuestIsComplete(*ptDungeonFinderQuestInstance);
}





void sHideRiftUIs()
{
    UI_GetWindowFromID(UIWND_DUNGEON_FINDER_CONTAINER).SetActive(FALSE);
    UI_GetWindowFromID(UIWND_TIERED_RIFT_CONTAINER).SetActive(FALSE);
    UI_GetWindowFromID(UIWND_CHALLENGE_RIFT_CONTAINER).SetActive(FALSE);
    UI_GetWindowFromID(UIWND_TIERED_RIFT_BONUS_OBJECTIVE_WRAPPER).SetActive(FALSE);
    UI_GetWindowFromID(UIWND_WEEKLY_CHALLENGE_BONUS_OBJECTIVE_WRAPPER).SetActive(FALSE);
    UI_GetWindowFromID(UIWND_WEEKLY_CHALLENGE_SECONDARY_BONUS_OBJECTIVE_WRAPPER).SetActive(FALSE);
    // ...
    UI_GetProgressBarFromID(UIPROGRESS_TIERED_RIFT_PROGRESSBAR).SetExtentsAndPosition(0.0f, 0.0f, 0.0f, -1.0f);
    UI_GetProgressBarFromID(UIWND_TIERED_RIFT_TIMER_PROGRESSBAR).SetExtentsAndPosition(0.0f, 0.0f, 0.0f, -1.0f);
}





void sUpdateWeeklyChallenge()
{
    if (GameIsWeeklyChallenge())
    {
        Player * ptPlayer = LocalPlayerGet();
        // ...
        // ...
        if (!CPlayerIsValid(ptPlayer))
            return;
        // ...
        // ...
        // ...
        // ...
        SNO snoChallengeRiftTutorial = GlobalSNOGet(GLOBALSNO_TUTORIAL_CHALLENGE_RIFT_GENERAL);
        if (!gtTutorialMgr.IsAnyShowing(0) && !Tutorial::PlayerHasSeen(*ptPlayer, snoChallengeRiftTutorial))
        {
            gtTutorialMgr.ShowTutorial(GetUIIDNone(), snoChallengeRiftTutorial, ptPlayer, TRUE);
        }
        // ...
        // ...
        // ...
        // ...
        // ...
        // ...
        // ...
        SNO snoChallengeRiftSkill = GlobalSNOGet(GLOBALSNO_TUTORIAL_CHALLENGE_RIFT_SKILL);
        if (!gtTutorialMgr.IsAnyShowing(0) && !Tutorial::PlayerHasSeen(*ptPlayer, snoChallengeRiftSkill))
        {
        }
    }
}








static void sUpdateDungeonFinder()
{


    if (!UI_GetWindowFromID(UIWND_CONSOLE_HUD).IsActive())
    {
        sHideRiftUIs();
        return;
    }


    if (sIsInTrialRift())
    {
        sUpdateTrialRiftUI();
    }
    else if (sIsInTieredRift() || sIsInChallengeRift())
    {
        sUpdateTieredRiftUI();
    }
    else if (sIsInRift())
    {
        sUpdateRiftUI();
    }
    else
    {
        sHideRiftUIs();
    }
}












inline ConsoleBackupPotionIterateParams::ConsoleBackupPotionIterateParams()
{
    gbidToMatch = GBID_NONE;
    ptMatchedPotionACD = NULL;
    bFoundMatch = FALSE;
}


void sFindMatchingPotion(
    ACDID idACDSquare,
    InventorySlotType,
    void * pUserData)
{
    ConsoleBackupPotionIterateParams * ptParams = (ConsoleBackupPotionIterateParams *)pUserData;
    if (ptParams->bFoundMatch || idACDSquare == ACDID_INVALID)
        return;









    ActorCommonData & tACDSlot = ACDGet(idACDSquare);
    if (!ItemHasLabel(tACDSlot.hGB.gbid, 40))
        return;




    if (ptParams->gbidToMatch != GBID_NONE && tACDSlot.hGB.gbid == ptParams->gbidToMatch)
    {

        ptParams->ptMatchedPotionACD = &tACDSlot;
        ptParams->bFoundMatch = TRUE;
    }
    else
    {
        ptParams->ptMatchedPotionACD = &tACDSlot;
    }

}

void sCheckForEquippedPotion()
{
    if (GameIsPaused() || GameIsSoftPaused())
        return;
    // ...
    uint32 dwFrameCount = sgtAppGlobals.dwFrameCounter;
    // ...
    if (dwFrameCount % TICKS_PER_SECOND == 0)
    {
        for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextAll(ptPlayer))
        {
            ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
            // ...
            if (PlayerIsLocal(ptPlayer->idPlayerACD) && ptACDPlayer && ptACDPlayer->ptInventory)
            {
                const HotbarButtonData & tPotionData = PlayerGetPotionBtnAssignment(ptPlayer);
                // ...
                ACDID idItem = ACDID_INVALID;
                GBID gbidPotion = GBID_NONE;
                // ...
                if (tPotionData.m_annItem != ANN_INVALID)
                {
                    ActorCommonData * ptACDPotion = ACDTryToGet(ACDFromANN(tPotionData.m_annItem));
                    if (ptACDPotion)
                    {
                        gbidPotion = ptACDPotion->hGB.gbid;
                    }
                }
                // ...
                // ...
                int32 nPotionCount = ActorInventoryCountItemsOfTypeOnPlayer(ptPlayer->idPlayerACD, gbidPotion, &idItem);
                // ...
                if (nPotionCount <= 0)
                {
                    // ...
                    ConsoleBackupPotionIterateParams tPotionParams;
                    tPotionParams.gbidToMatch = GlobalGBIDGet(GLOBALGBID_ITEM_HEALTHPOTION_BOTTOMLESS);
                    // ...
                    const ActorCommonData & tACD = ACDGet(ptPlayer->idPlayerACD);
                    ACDInventoryIterateItems(tACD, 16, 6, sFindMatchingPotion, &tPotionParams);
                    // ...
                    // ...
                    // ...
                    // ...
                    // ...
                    // ...
                    if (tPotionParams.ptMatchedPotionACD)
                    {
                        // ...
                        HotbarButtonData tButtonData;
                        tButtonData.SetItemANN(ANNFromACD(tPotionParams.ptMatchedPotionACD->id));
                        // ...
                        PlayerSendPotionSlotChangeToServer(tButtonData, ptPlayer);
                    }
                }
            }
        }
    }
}

// NON_MATCHING: 14820 bytes. Inlined UI helpers use different stack slots for their temporaries.
void UIGame::Update(
    const float flDeltaTime)
{


    if (!sgptUIGameGlobals)
    {
        return;
    }










    sUpdateDungeonFinder();
    sUIUpdateMainMonsterHPBar();
    sUIUpdateFloatingHPBars();
    UpdateACDTags();
    UpdateRActorHighlights();
    UpdateRActorTooltips();
    UIChatBalloons::Update();

    sUpdateParagonPtsVisiblity();
    sUpdateNotificationQueue();

    sUpdateWeeklyChallenge();
    UILoreNotify::Update();
    UILorePlayer::Update();
    UIFollowerSwap::Update();
    UIBuffs::Update();
    UIGoldCounter::Update();
    UICallouts::Update();




    UIMinimap_Update();
    UIMap::Update();
    UIActTransitions_Update();


    UIHireling::Display();

    UIStash::Update();
    UIFloatingMessage::Update();

    ACDID idACDPlayer = LocalPlayerACDGet();
    ActorCommonData * ptACDPlayer = ACDTryToGet(idACDPlayer);
    if (ptACDPlayer)
    {
        UIInventory::UpdateToolTips(*ptACDPlayer);
    }



    sGameTextUpdate();
    sErrorTextUpdate();
    sActTitleUpdate();
    if (sgptUIGameGlobals->dwTrialMessageStartTime != 0 &&
        CGameGetClientTicks() - sgptUIGameGlobals->dwTrialMessageStartTime > sgptUIGameGlobals->dwTrialMessageDelay)
    {
        ShowTrialEndMessage();
    }


    UIHUD::Console::Update();

    sCheckForEquippedPotion();



    if (!gtTutorialMgr.IsAnyShowing(0) && UIGameSettings::Console::CanLowerGameDifficulty())
    {
        if (CGameGetPredictedServerTime() > 60 * TICKS_PER_MINUTE)
        {
            Player * ptPlayer = LocalPlayerGetPrimary();
            if (ptPlayer->idPlayerACD != ACDID_INVALID)
            {
                gtTutorialMgr.ShowTutorial(GetUIIDNone(), GlobalSNOGet(GLOBALSNO_TUTORIAL_LOWER_DIFFICULTY), ptPlayer, TRUE);
            }
        }
    }
    UIInventory::UpdateItemManipulationTimers();
    UIDeathMenu_Console_Display();
    UIMultiplayerDeathMenu::Console::Update();






























    sChallengeRift_TargetTime_UpdateDisplay();
}




UIResult UIGame::HUD_ClickPredicate(const UIID &)
{
    // ...
    // ...
    // ...
    // ...
    // ...
    // ...
    // ...
    // ...
    // ...
    return 1;
}

void UIGame::InvOpen_OnClick(const UIID &)
{
    // ...
    // ...
    // ...
    // ...
    // ...
    // ...
    if (UIInventory::IsOpen())
        return;
    // ...
    Player * ptPlayer = LocalPlayerGet();
    if (ptPlayer)
    {
        ActorCommonData * ptACD = ACDTryToGet(ptPlayer->idPlayerACD);
        if (ptACD && !PlayerCanOpenInventory(*ptACD))
        {
            UIGame::DisplayError(GameErrorData(149), FALSE, NULL);
            return;
        }
    }
    // ...
    // ...
    // ...
    UIInventory::Show(TRUE);
    UIInventory::Display(1.0f);
}











void UIGame::BugOpen_OnClick(const UIID &)
{
}









































































































































































void UIGame::InitializeFor_Server()
{
}




void UIGame::DisposeFor_Server()
{
}




void UIGame::SetMonsterHPBar(const ACDID idACD)
{
    UIGlobals *& ptUIGlobals = CGameGet().ptUIGlobals;
    if (!ptUIGlobals)
    {
        // ...
        return;
    }
    ptUIGlobals->idACDMonsterHPBar = idACD;
}




void UIGame::SetBossHPBar(const ACDID idACD)
{
    UIGlobals *& ptUIGlobals = CGameGet().ptUIGlobals;
    if (!ptUIGlobals)
    {
        // ...
        return;
    }
    ptUIGlobals->idACDBossHPBar = idACD;
}




ACDID UIGame::GetBossHPBarACD()
{
    UIGlobals *& ptUIGlobals = CGameGet().ptUIGlobals;
    if (!ptUIGlobals)
    {
        // ...
        return ACDID_INVALID;
    }
    return ptUIGlobals->idACDBossHPBar;
}




void UIGame::SetRActorHighlight(RActorID idRActor, int32 nControllerIndex)
{
    // ...
    // ...
    // ...
    // ...
    sgptUIGameGlobals->idRActorHighlight[nControllerIndex] = idRActor;
}



RActorID UIGame::GetRActorHighlight(int32 nControllerIndex)
{
    // ...
    if (!sgptUIGameGlobals)
    {
        // ...
        // ...
        // ...
        // ...
        return RACTORID_INVALID;
    }
    // ...
    return sgptUIGameGlobals->idRActorHighlight[nControllerIndex];
}


void UIGame::SetRActorAlternateHighlight(RActorID idRActor, int32 nControllerIndex)
{
    // ...
    // ...
    // ...
    // ...
    sgptUIGameGlobals->idRActorAlternateHighlight[nControllerIndex] = idRActor;
}



RActorID UIGame::GetRActorAlternateHighlight(int32 nControllerIndex)
{
    // ...
    if (!sgptUIGameGlobals)
    {
        // ...
        // ...
        // ...
        // ...
        // ...
        return RACTORID_INVALID;
    }
    return sgptUIGameGlobals->idRActorAlternateHighlight[nControllerIndex];
}
void UIGame::ClearRActorAlternateHighlightPrompt(int32 nControllerIndex)
{
    if (sgptUIGameGlobals->idEffectButtonPrompt[nControllerIndex] != EFFECTGROUPID_INVALID)
    {
        EffectGroupDelete(sgptUIGameGlobals->idEffectButtonPrompt[nControllerIndex]);
        sgptUIGameGlobals->idEffectButtonPrompt[nControllerIndex] = EFFECTGROUPID_INVALID;
    }
}





void UIGame::SetSpecialAreaText(LPCSTR uszText, LPCSTR uszTextSubtitle)
{
    // ...
    UIGlobals * ptUIGlobals = CGameGet().ptUIGlobals;
    ptUIGlobals->bGameSpecialTextPresent = TRUE;
    Blizzard::String::Copy(ptUIGlobals->uszGameSpecialText, uszText, sizeof(ptUIGlobals->uszGameSpecialText));
    Blizzard::String::Copy(ptUIGlobals->uszGameSpecialTextSubTitle, uszTextSubtitle, sizeof(ptUIGlobals->uszGameSpecialTextSubTitle));
}













static void sActTitleUpdate()
{

    UIWindow & tDlg = UI_GetWindowFromID(UIDLG_ACT);

    ActTitleParams & tTitleParams = sgptUIGameGlobals->tTitleParams;

    float flFrac;
    uint32 dwTicksElapsed = CGameGetRealTicks() - tTitleParams.dwTimestamp;
    FadeState eFadeState = scgtActTitleFadeRamp.Evaluate(dwTicksElapsed, flFrac);

    UIGlobals * ptUIGlobals = CGameGet().ptUIGlobals;
    if (ptUIGlobals->bGameSpecialTextPresent == TRUE)
    {
        tTitleParams.dwTimestamp = CGameGetRealTicks();
        Blizzard::String::Copy(tTitleParams.uszTitle, ptUIGlobals->uszGameSpecialText, sizeof(tTitleParams.uszTitle));
        Blizzard::String::Copy(tTitleParams.uszSubtitle, ptUIGlobals->uszGameSpecialTextSubTitle, sizeof(tTitleParams.uszSubtitle));

        ptUIGlobals->bGameSpecialTextPresent = FALSE;
        flFrac = 1.0f;
    }

    if (eFadeState == FADESTATE_HIDDEN)
    {
        tDlg.SetActive(FALSE);
    }
    else
    {
        tDlg.SetActive(TRUE);
        uint8 bAlpha = (uint8)Math::FloatToInt(flFrac * 255.0f);
        tDlg.SetTransparency(bAlpha);

        UI_GetTextFromID(UITEXT_ACT_TITLE).SetText(tTitleParams.uszTitle, FALSE);
        UI_GetTextFromID(UITEXT_ACT_SUBTITLE).SetText(tTitleParams.uszSubtitle, FALSE);
    }
}




void UIGame::ChallengeRift_SetTargetTimeNotification()
{
    // ...
    if (GameIsWeeklyChallenge() && sgptUIGameGlobals)
    {
        // ...
        // ...
        // ...
        sgptUIGameGlobals->dwTimestampChallengeRiftTargetTime = CGameGetRealTicks();
    }
}


static void sChallengeRift_TargetTime_UpdateDisplay()
{
    if (!UI_TestID(UIDLG_CHALLENGERIFT_TARGET_TIME)) return;

    UIWindow & tChallengeRiftTargetTime = UI_GetWindowFromID(UIDLG_CHALLENGERIFT_TARGET_TIME);

    if (!GameIsWeeklyChallenge() || !sIsInTieredRift() || !sgptUIGameGlobals || !sgptUIGameGlobals->dwTimestampChallengeRiftTargetTime)
    {
        tChallengeRiftTargetTime.SetActive(FALSE);
        return;
    }
    if (!UI_TestID(UITEXT_CHALLENGERIFT_TARGET_TIME) || !UI_TestID(UITEXT_CHALLENGERIFT_TARGET_TIME_PLAYER_TO_BEAT)) return;


    UIText & tChallengeRiftTargetTimeText = UI_GetTextFromID(UITEXT_CHALLENGERIFT_TARGET_TIME);
    UIText & tChallengeRiftChallengerText = UI_GetTextFromID(UITEXT_CHALLENGERIFT_TARGET_TIME_PLAYER_TO_BEAT);
    int32 nCompletionMilliseconds = CGameGlobalsGet().nChallengeRiftCompletionMilliseconds;
    CRefString sHeroName = CGameGlobalsGet().sChallengeRiftCompletionMillisecondsHeroName;
    if (nCompletionMilliseconds > 0)
    {
        Blizzard::Time::Timestamp tTime = int64(nCompletionMilliseconds) * 1000000;
        CRefString sTime = sFormatElapsedTime(tTime);

        CRefString sMessage = LocalizedSprintf(
            StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CHALLENGE_RIFT), "TargetTimeNotification"),
            sTime.str(), NULL);


        tChallengeRiftTargetTimeText.SetText(sMessage.str(), FALSE);

        tChallengeRiftChallengerText.SetActive(TRUE);
        if (sHeroName.Length())
        {
            CRefString sInstigatorName = LocalizedSprintf(
                StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CHALLENGE_RIFT), "ChallengeMode_Challenger"),
                sHeroName.str(), NULL);
            tChallengeRiftChallengerText.SetText(sInstigatorName.str(), FALSE);
        }
        else
        {
            tChallengeRiftChallengerText.SetText(StringListFetch("ChallengeRifts:OriginalRunnerTime").str(), FALSE);
        }
    }



    float flFrac = 0.0f;
    RGBAColor rgbaColor = crgbaWhite;
    FadeState eFadeState;
    uint32 dwTicksElapsed = CGameGetRealTicks() - sgptUIGameGlobals->dwTimestampChallengeRiftTargetTime;
    eFadeState = scgtChallengeRiftTargetTimeFadeRamp.Evaluate(dwTicksElapsed, flFrac);

    if (eFadeState == FADESTATE_HIDDEN)
    {
        tChallengeRiftTargetTime.SetActive(FALSE);
        sgptUIGameGlobals->dwTimestampChallengeRiftTargetTime = 0;
    }
    else
    {
        rgbaColor.a = (uint8)Math::FloatToInt(flFrac * 255.0f);
        tChallengeRiftTargetTime.SetActive(TRUE);
        tChallengeRiftTargetTime.SetWindowColor(rgbaColor);
    }
}


































































































































































void UIGame::SetGameText(const CRefString & usMsg)
{
    // ...
    GameTextParams tTextParams;
    // ...
    Blizzard::String::Copy(tTextParams.uszText, usMsg, sizeof(tTextParams.uszText));
    tTextParams.dwStateTimestamp = CGameGetRealTicks();
    // ...
    // ...
    // ...
    sgptUIGameGlobals->ptGameTextList->AddTail_Tracked(tTextParams);
}




static CRefString sGetStringForError( const GameErrorData & tData )
{
    switch ( tData.eError )
    {
    case 13:
        {
            GBID gbid = tData.dwVal;
            if ( gbid == GlobalGBIDGet( (GlobalGBID)68 ) )
                return StringListFetch( GlobalSNOGet( (GlobalSNO)723 ), "PowerRequiresMeleeWeapon" );
            if ( gbid == GlobalGBIDGet( (GlobalGBID)70 ) )
                return StringListFetch( GlobalSNOGet( (GlobalSNO)723 ), "PowerRequiresShield" );
        }
        break;

    case 179:
        {
            CRefString sError;
            CRefString sTime;
            CRefString sMsg;
            if ( tData.dwVal > 3600 )
                sError = LocalizedSprintf( StringListFetch( GlobalSNOGet( (GlobalSNO)741 ), "GeneralHours" ).str(), UnicodeText::FormatNumber( (int32)tData.dwVal / 3600, TRUE, FNS_NORMAL, XLOCALE_INVALID ).str(), NULL );
            else if ( tData.dwVal > 60 )
                sError = LocalizedSprintf( StringListFetch( GlobalSNOGet( (GlobalSNO)741 ), "GeneralMinutes" ).str(), UnicodeText::FormatNumber( (int32)tData.dwVal / 60, TRUE, FNS_NORMAL, XLOCALE_INVALID ).str(), NULL );
            else
                sError = LocalizedSprintf( StringListFetch( GlobalSNOGet( (GlobalSNO)741 ), "GeneralSeconds" ).str(), UnicodeText::FormatNumber( (int32)tData.dwVal, TRUE, FNS_NORMAL, XLOCALE_INVALID ).str(), NULL );

            sTime = StringListFetch( GlobalSNOGet( (GlobalSNO)723 ), SymbolToName( tData.eError, gGameErrors ) );
            sMsg = LocalizedSprintf( sTime.str(), sError.str(), NULL );
            return sMsg;
        }
    }

    return StringListFetch( GlobalSNOGet( (GlobalSNO)723 ), SymbolToName( tData.eError, gGameErrors ) );
}






























static void sProcessErrorForPlayer( const CRefString & sError, const Player * ptPlayer )
{
    if ( !ptPlayer || Console::Screens::IsScreenSetOpen() )
    {
        UIGame::DisplayErrorString( sError );
    }
    else
    {
        CRefString sTextureName( CRefString().Format( "UI:Console_ItemStatus_Unusable" ) );
        UIEventQueue::Console::DisplayLongMessageForPlayer( sError, ptPlayer, ImageTextureFrame( sTextureName ), FALSE );
    }
}







void UIGame::DisplayError( const GameErrorData & tData, BOOL fDisplayBnetError, const Player * ptPlayer )
{
    const uint32 MIN_COOLDOWN_TO_REPORT = 300;

    if ( tData.eError == 2 )
        return;

    if ( tData.eError == 16 && tData.dwVal > 0 && tData.dwVal <= MIN_COOLDOWN_TO_REPORT )
        return;

    CRefString sError = sGetStringForError( tData );
    sProcessErrorForPlayer( sError, ptPlayer );

    uint32 dwSoundTag = sarnErrorSoundTag[tData.eError];
    if ( dwSoundTag == (uint32)-1 )
        return;

    if ( tData.eError >= 31 && tData.eError <= 38 )
    {
        ACDID idACDHireling = (ACDID)tData.dwVal;
        if ( idACDHireling == ACDID_INVALID )
            return;

        RActorID idRActorHireling = (RActorID)ACDGetOwner( idACDHireling );
        if ( idRActorHireling != RACTORID_INVALID )
            CPlayerSayError( dwSoundTag, idRActorHireling, FALSE );
    }
    else
    {
        RActorID idRPlayerRActor = ptPlayer ? ptPlayer->idPlayerRActor : RACTORID_INVALID;
        CPlayerSayError( dwSoundTag, idRPlayerRActor, FALSE );
    }
}







































void UIGame::DisplayErrorString(const CRefString & sError)
{
    ErrorTextParams & tErrorText = sgptUIGameGlobals->tErrorText;
    Blizzard::String::Copy(tErrorText.uszText, sError, sizeof(tErrorText.uszText));
    tErrorText.dwTimestamp = CGameGetRealTicks();
}




void UIGame::DisplayItemRequiredError(const GBID gbidItem, const StringLabelHandle hLabel)
{
    CRefString sError;
    if (gbidItem == GBID_NONE)
    {
        if (hLabel != (StringLabelHandle)-1)
        {
            sError = StringListFetchByLabel(GlobalSNOGet(GLOBALSNO_STRINGLIST_GAME_ERRORS), hLabel);
        }
        else
        {
            sError = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GAME_ERRORS), "ItemRequiredVague");
        }
    }
    else
    {
        if (ItemHasLabel(gbidItem, ITL_QUEST))
        {
            sError = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GAME_ERRORS), "QuestItemRequired");
        }
        else
        {
            sError = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GAME_ERRORS), "ItemRequired");
        }
        // ...
        ScopedItemDefinitionLockR ptItem(gbidItem);
        if (!ptItem)
            return;
        {
            // ...
            // ...
            CRefString usInitialItemName, sName1, sName2Ignored;
            if (StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_ITEMS), ptItem->tHeader.szName, usInitialItemName))
            {
                // ...
                uint32 dwGrammaticalForm;
                LanguageRules::ExtractGrammaticalForm(usInitialItemName, sName1, sName2Ignored, dwGrammaticalForm);
            }
            // ...
            sError = StringListEntryReplace(sError, "{Item}", sName1.str(), 999);
        }
    }
    // ...
    CPlayerSayError(TAG_ACTOR_WARNING_OBJECT_LOCKED, RACTORID_INVALID, FALSE);
    // ...
    ErrorTextParams & tErrorText = sgptUIGameGlobals->tErrorText;
    CRefString sAfterLanguageRules(LanguageRules::ApplyLanguageRules(sError.str()));
    Blizzard::String::Copy(tErrorText.uszText, sAfterLanguageRules, sizeof(tErrorText.uszText));
    tErrorText.dwTimestamp = CGameGetRealTicks();
}
















































void UIGame::Panel_ToolTip(const UIID & id)
{
    // ...
    // ...
    // ...
    // ...
    // ...
    // ...
    // ...
    // ...
    // ...
    // ...
    // ...
}



BOOL UIGame::MenuIsActive()
{
    // ...
    // ...
    // ...
    // ...
    // ...
    // ...
    // ...
    // ...
    // ...
    // ...
    // ...
    // ...
    // ...
    // ...
    // ...
    // ...
    // ...
    // ...
    // ...
    // ...
    // ...
    // ...
    // ...
    // ...
    // ...
    // ...
    // ...
    // ...
    // ...
    // ...
    // ...
    // ...
    return FALSE;
}




BOOL UIGame::ModalIsActive()
{
    return FALSE;
}






























void UIGame::AddTagToACD(
    ACDID idACD,
    LPCSTR szUIControlName,
    LPCSTR szText,
    const SNO snoFont,
    const int32 nFontSize,
    const RGBAColor rgbaText,
    const RGBAColor rgbaDropShadow,
    const uint32 dwDuration,
    const float flYOffset)
{
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD)
    {
        RActorID idRActor = (RActorID)ACDGetOwner(idACD);
        RActor * ptRActor = RActorTryToGet(idRActor);
        if (ptRActor)
        {
            ButtonCreationParams tParams(UIT_ACTORTAG, szUIControlName, UIDLG_ITEMTAGS);
            tParams.eEatsClicks = EC_IGNORE;
            tParams.snoFont = snoFont;
            tParams.nFontSize = nFontSize;
            tParams.rgbaText = rgbaText;
            tParams.rgbaDropShadow = rgbaDropShadow;
            // ...
            UIActorTag & tTag = *new UIActorTag(tParams, Vector2D(0.0f, 0.0f), Vector2D(0.0f, 0.0f));
            tTag.SetText(szText, FALSE);
            tTag.SetExpirationTime(CGameGetClientTicks() + dwDuration);
            tTag.SetACD(idACD);
            tTag.SetOffset(0.0f, flYOffset);
            sgptUIGameGlobals->listActorTagControls.AddHead_Tracked(tTag.GetID());
        }
    }
}



void UIGame::AddHUDUntrackedEffectGroup(const SNO snoEffectGroup)
{
}





















































































































static void sAddFXToProgressBar(
    const UIID & idProgressBar,
    const SNO snoParticle)
{
    UIProgressBar & tProgressBar = UI_GetProgressBarFromID(idProgressBar);
    Rect2D rectUIC;
    tProgressBar.GetWindowRect(rectUIC);
    float flVal = tProgressBar.GetPos();

    Vector2D vecUIC(rectUIC.left + flVal * rectUIC.Width(), rectUIC.top + rectUIC.Height() * 0.5f);
    sPlayRiftFX(snoParticle, vecUIC);
}

















































































static void sPrintToggleMessage(UIAction eAction, BOOL fEnabled, const char * szEnabled, const char * szDisabled)
{
    // ...
    // ...
    // ...
    // ...
    CRefString sFmt = StringListFetch(fEnabled ? szEnabled : szDisabled);
    CRefString sOutput = LocalizedSprintf(sFmt.str(), Prefs::GetActionKeyName(eAction, FALSE, 0).str(), NULL);
    // ...
    Set_Text_Color(sOutput, 21);
    UI_Print(sOutput, TRUE);
}



void UIGame::ToggledMonsterHPBars()
{
    // ...
    BOOL fEnabled = Prefs::GetGameplayPrefs().GetShowMonsterHPBars();
    // ...
    sPrintToggleMessage(22, fEnabled, "General:MonsterHealthBarsEnabled", "General:MonsterHealthBarsDisabled");
}







void UIGame::ToggledPlayerHPBars()
{
    // ...
    BOOL fEnabled = Prefs::GetGameplayPrefs().GetShowPlayerHPBars();
    // ...
    sPrintToggleMessage(23, fEnabled, "General:PlayerHealthBarsEnabled", "General:PlayerHealthBarsDisabled");
}







void UIGame::ToggledItemsOnGround()
{
    // ...
    BOOL fEnabled = Prefs::GetGameplayPrefs().GetShowItemsOnGround();
    // ...
    sPrintToggleMessage(25, fEnabled, "General:ItemsOnGroundEnabled", "General:ItemsOnGroundDisabled");
}


































































































































































































void UIGame::SetBreathMeter(const float flUnitSuffocation)
{
}
































void UIGame::SetLoopingAnimMeter(const RActor & tRActor, const float flUnitLoopingAnim)
{
    // ...
    // ...
    // ...
    // ...
    Player * ptPlayer = PlayerGetByACD(tRActor.idACD);
    if (!ptPlayer)
        return;
    // ...
    // ...
    UIWindow & tWndMeter = UI_GetWindowFromID(sgptUIGameGlobals->idRActorLoopingMeter[ptPlayer->nControllerIndex]);
    // ...
    if (flUnitLoopingAnim < 1e-06f)
    {
        // ...
        tWndMeter.SetActive(FALSE);
    }
    else
    {
        // ...
        tWndMeter.SetActive(TRUE);
        UIID idWndProgressBar = UIIDCreateFromParent(tWndMeter.GetID(), ".progressbar");
        UIProgressBar & tProgressBar = UI_GetProgressBarFromID(idWndProgressBar);
        tProgressBar.SetExtentsAndPosition(0.0f, 1.0f, flUnitLoopingAnim, flUnitLoopingAnim);
        // ...
        // ...
        // ...
        Vector3D wp;
        RActorGetTopPoint(&tRActor, wp);
        Vector2D wpUIC;
        if (WorldPointToScreenPointUICUnclipped(wp, wpUIC, -1))
        {
            Rect2D rectPosition;
            rectPosition.left = wpUIC.x; rectPosition.top = wpUIC.y;
            // ...
            rectPosition.right = wpUIC.x; rectPosition.bottom = wpUIC.y;
            // ...
            // ...
            Rect2D rectWndUIC;
            tWndMeter.GetWindowRect(rectWndUIC);
            rectWndUIC.AlignToRect(rectPosition, AP_HORIZONTAL_CENTER, AP_VERTICAL_ABOVE, 0.0f);
            tWndMeter.SetWindowRect(rectWndUIC);
        }
    }
}

















void UIGame::HideLoopingAnimMeter(const int32 nControllerIndex)
{
    // ...
    UIWindow & tWndMeter = UI_GetWindowFromID(sgptUIGameGlobals->idRActorLoopingMeter[nControllerIndex]);
    tWndMeter.SetActive(FALSE);
}





void UIGame::StartTrialTimer(const uint32 dwDelay)
{
    // ...
    sgptUIGameGlobals->dwTrialMessageStartTime = CGameGetClientTicks();
    sgptUIGameGlobals->dwTrialMessageDelay = dwDelay;
}




void UIGame::ShowTrialEndMessage()
{

    sgptUIGameGlobals->dwTrialMessageStartTime = 0;
    sgptUIGameGlobals->dwTrialMessageDelay = 0;

















    Console::Screens::OpenToScreen(
        Console::Screens::TRIAL_END_SCREEN_SET,
        0,
        Console::GamerProfile::GetPrimaryProfileUserIndex(),
        FALSE);
}




void sHandleSequenceSkip(const UIID &)
{
    // ...
    BoolDataMessage tMessage;
    tMessage.bVal = FALSE;
    MessageSendToServer(384, &tMessage, sizeof(tMessage), LocalPlayerGet());
}




void UIGame::ConfirmSkipEvent()
{
    // ...
    if (GameIsOpenWorld())
    {
        sHandleSequenceSkip(GetUIIDNone());
        return;
    }
    // ...
    // ...
    CRefString sMsg = StringListFetch("H2OLayout:ConfirmSkipTitle");
    // ...
    // ...
    if (PlayerGetNumPlayers() > 1)
    {
        // ...
        sMsg += StringListFetch("H2OLayout:ConfirmSkipWarning");
    }
    // ...
    // ...
    UIConfirmation::Show(sMsg.str(), sHandleSequenceSkip, NULL, FALSE, FALSE, TRUE, -1);
}


































































void UIGame::OnPlayerLeavingGame(
    const Player * ptPlayerLeaving)
{
    if (PlayerIsLocal(ptPlayerLeaving->nPlayerIndex))
    {
        {
            RActorID idRActor = GetRActorHighlight(ptPlayerLeaving->nControllerIndex);
            RActor * ptRActor = RActorTryToGet(idRActor);
            if (ptRActor)
            {
                RActorHighlight(*ptRActor, FALSE);
                UIHUD::Console::UpdatePlayerTargetingIndicator(ptPlayerLeaving, ptRActor, FALSE);
            }
            // ...
            SetRActorHighlight(RACTORID_INVALID, ptPlayerLeaving->nControllerIndex);
        }
        // ...
        {
            RActorID idRActor = GetRActorAlternateHighlight(ptPlayerLeaving->nControllerIndex);
            RActor * ptRActor = RActorTryToGet(idRActor);
            if (ptRActor)
            {
                RActorHighlight(*ptRActor, FALSE);
            }
            // ...
            SetRActorAlternateHighlight(RACTORID_INVALID, ptPlayerLeaving->nControllerIndex);
        }
        // ...
        POS posNotifications = sgptUIGameGlobals->listNotifications.GetHeadPosition();
        while (posNotifications)
        {
            POS posCurrent = posNotifications;
            NotificationData & tData = sgptUIGameGlobals->listNotifications.GetNext(posNotifications);
            // ...
            if (tData.nPlayerIndex == ptPlayerLeaving->nPlayerIndex)
            {
                sgptUIGameGlobals->listNotifications.RemoveAt(posCurrent);
            }
        }
    }
}




UIID UIGame::GetPlayerHUDID(const int32 nHUDIndex)
{
    if (nHUDIndex < 0 || nHUDIndex >= MAX_LOCAL_PLAYERS)
    {
        // ...
        return GetUIIDNone();
    }
    // ...
    // ...
    return sgptUIGameGlobals->idPlayerHUD[nHUDIndex];
}




































void UIFloatingMessage::Show(LPCSTR uszMessage, uint32 dwDuration)
{
    // ...
    // ...
    if (!UI_TestID(UIDLG_NOTIFY_MESSAGE) || !UI_TestID(UITEXT_NOTIFY_MESSAGE_TITLE))
    {
        // ...
        // ...
        return;
    }
    UIText & tTextNotifyTitle = UI_GetTextFromID(UITEXT_NOTIFY_MESSAGE_TITLE);
    tTextNotifyTitle.SetText(uszMessage, FALSE);
    // ...
    UIText & tTextNotifyText = UI_GetTextFromID(UITEXT_NOTIFY_MESSAGE_TEXT);
    tTextNotifyText.SetText("", FALSE);
    // ...
    UI_GetHotkeyFromID(UIHOTKEY_NOTIFY_MESSAGE).SetActive(FALSE);
    // ...
    // ...
    UI_GetWindowFromID(UIDLG_NOTIFY_MESSAGE).SetActive(TRUE);
    // ...
    if (dwDuration)
    {
        // ...
        sgptUIGameGlobals->dwFloatingMessageStartTime = CGameGetRealTicks();
        sgptUIGameGlobals->dwFloatingMessageDuration = dwDuration;
    }
}

#include "../../Font/Font.h"
#include "../../Font/FormattedText.h"
// Raw CTrickle.cpp:336 only; no recovered header declaration.
ClientTrickleList & CTrickleGetList();
// Raw CPlayer.cpp:3914 and :3967 only; no recovered header declarations.
void FloatingDamageNumbersDraw(float & flZSlice);
void FloatingTextDraw(float & flZSlice);
#line 4333 "X/Client/UserInterface/Interface/UIGame.cpp"
static void sUIDrawHeadstoneTimers()
{

    SWorldID idSWorldCurrent = CGameGetCurrentSWorldID();


    ClientTrickleList & listTrickle = CTrickleGetList();
    POS posTrickle = listTrickle.GetHeadPosition();
    while (posTrickle)
    {
        const ClientTrickleEntry & tTrickleEntry = listTrickle.GetNext(posTrickle);


        if (tTrickleEntry.eType != TRICKLE_INFO_FRIENDLY_PLAYER &&

            (tTrickleEntry.eType != TRICKLE_INFO_LOCAL_PLAYER ||
             PlayerGetLocalPlayerCount() < 2))
        {
            continue;
        }



        const WorldPlace & tPlace = tTrickleEntry.tPlace;
        if (tPlace.idSWorld != idSWorldCurrent)
        {
            continue;
        }


        int32 nReviveTime = tTrickleEntry.dwHeadstoneCorpseReviveTime;
        int32 nServerTime = CGameGetPredictedServerTime();
        int32 nTicksLeft = nReviveTime - nServerTime;
        if (nTicksLeft <= 0)
        {
            continue;
        }

        Vector2D wpUIC;
        if (WorldPointToScreenPointUICUnclipped(tPlace.wp, wpUIC, CAMERA_CURRENT))
        {
            CRefString sDisplayText;

            ACDID idACD = ACDFromANN(tTrickleEntry.ann);
            ActorCommonData * ptACD = ACDTryToGet(idACD);
            if (ptACD && ptACD->IsPet())
            {
                sDisplayText += StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GIZMOS), "HeadstoneHirelingRes");
            }


            int32 nSecondsLeft = Math::Max<int32>((nTicksLeft + TICKS_PER_SECOND - 1) / TICKS_PER_SECOND, 0);
            sDisplayText += LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GIZMOS), "HeadstoneTimeRemaining"),
                UnicodeText::FormatNumber(nSecondsLeft, TRUE, FNS_NORMAL, XLOCALE_INVALID).str(), NULL);

            TextRenderProperties tProps(GlobalSNOGet(cgeDefaultFontForInGameText), FONT_DEFAULT_SIZE, 1.0f, 1.0f);


            Vector2D vecTextSizeUIC;
            FontStringGetRenderedSizeUIC(tProps, FormattedText(sDisplayText, PARSE_DEFAULT), vecTextSizeUIC);


            Rect2D rectTextUIC;
            rectTextUIC.Set(wpUIC.x, wpUIC.y, wpUIC.x + vecTextSizeUIC.x, wpUIC.y + vecTextSizeUIC.y);


            UI_ConstrainRectToViewport(rectTextUIC, 10.0f);

            wpUIC.x = Math::FloatTrunc(rectTextUIC.left);
            wpUIC.y = Math::FloatTrunc(rectTextUIC.top);

            FontStringDrawUIC(tProps, FormattedText(sDisplayText, PARSE_DEFAULT), wpUIC, crgbaGold, &crgbaBlack, 0.8f, TRUE, NULL, RENDERLAYER_INVALID, FALSE);
        }
    }
}



namespace UIGame { void Draw(
    const uint8,
    float & flZSlice)
{
    DebugRenderMode eDRM = sgtAppGlobals.eDebugRenderMode;
    if (!AppDrawFlagIsSet(APP_DRAW_NO_UI_BIT) && eDRM != DRM_AO && eDRM != DRM_AO_SM &&
        eDRM != DRM_ENVMAP && eDRM != DRM_GLOSS && eDRM != DRM_FLATSHADED)




    {
        FloatingDamageNumbersDraw(flZSlice);
        FloatingTextDraw(flZSlice);
    }


    sUIDrawHeadstoneTimers();
} }

#include "../../../Common/Quests/Quests.h"
#include "../../../Common/Definitions/LevelAreaDefinition.h"
// Raw Quests.cpp:995 only; no recovered header declaration.
BOOL QuestIsComplete(const QuestInstance & tQuest);
// Raw CWorld.cpp:1075 only; no recovered header declaration.
SNO WorldGetLevelAreaSNOFromRActor(RActorID idRActor);
#line 5759 "X/Client/UserInterface/Interface/UIGame.cpp"






#line 7281 "X/Client/UserInterface/Interface/UIGame.cpp"
static void sPlayRiftFX(
    const SNO snoParticle,
    const Vector2D vecUIC)
{
    UIRActor & tUIRActor = UI_GetRActorFromID(UIRACTOR_RIFT_FX);
    RActor * ptRActor = RActorTryToGet(tUIRActor.GetRActor());

    if (ptRActor)
    {


        tUIRActor.AttachParticleSystem(snoParticle, vecUIC, NULL);
    }
}
#line 7316 "X/Client/UserInterface/Interface/UIGame.cpp"
static void sPlayTieredRiftOrbPickUpHUDFX(
    const UIID & idProgressBar)
{
    ActorCommonData * ptACD = ACDTryToGet(LocalPlayerACDGet());
    BOOL bIsTieredRift;
    BOOL bIsWeeklyChallenge = GameIsWeeklyChallenge();
    bIsTieredRift = !ptACD || ACDIsInTieredLootRun(*ptACD);




    GlobalSNO arParticleSNOs[3];



















    if (bIsWeeklyChallenge)
    {
        arParticleSNOs[0] = GLOBALSNO_PARTICLE_CHALLENGE_RIFT_ORB_PICKUP_UI_FX_CONSOLE;
        arParticleSNOs[1] = GLOBALSNO_PARTICLE_CHALLENGE_RIFT_ORB_PICKUP_UI_FX_B_CONSOLE;
        arParticleSNOs[2] = GLOBALSNO_PARTICLE_CHALLENGE_RIFT_ORB_PICKUP_UI_FX_C_CONSOLE;
    }
    else
    {
        arParticleSNOs[0] = bIsTieredRift ? GLOBALSNO_PARTICLE_TIERED_RIFT_ORB_PICKUP_UI_FX_CONSOLE : GLOBALSNO_PARTICLE_NORMAL_RIFT_ORB_PICKUP_UI_FX_CONSOLE;
        arParticleSNOs[1] = bIsTieredRift ? GLOBALSNO_PARTICLE_TIERED_RIFT_ORB_PICKUP_UI_FX_B_CONSOLE : GLOBALSNO_PARTICLE_NORMAL_RIFT_ORB_PICKUP_UI_FX_B_CONSOLE;
        arParticleSNOs[2] = bIsTieredRift ? GLOBALSNO_PARTICLE_TIERED_RIFT_ORB_PICKUP_UI_FX_C_CONSOLE : GLOBALSNO_PARTICLE_NORMAL_RIFT_ORB_PICKUP_UI_FX_C_CONSOLE;
    }












    UIProgressBar & tProgressBar = UI_GetProgressBarFromID(idProgressBar);
    Rect2D rectUIC;
    tProgressBar.GetWindowRect(rectUIC);

    Vector2D vecUIC;



    vecUIC = Vector2D(rectUIC.left - rectUIC.Width() * 2.0f, rectUIC.top + rectUIC.Height() * 0.5f);


    for (uint32 i = 0; i < sizeof(arParticleSNOs) / sizeof(arParticleSNOs[0]); ++i)
    {
        sPlayRiftFX(GlobalSNOGet(arParticleSNOs[i]), vecUIC);
    }
}
#line 7943 "X/Client/UserInterface/Interface/UIGame.cpp"
namespace UIGame { void FlashTieredRiftProgressBar(
    float flProgress)
{


    SNO snoQuestDungeonFinder = GlobalSNOGet(GLOBALSNO_QUEST_DUNGEONFINDER);
    QuestInstance * ptDungeonFinderQuestInstance = QuestsGetQuestInstance(snoQuestDungeonFinder, ACDID_INVALID);
    SNO snoLevelArea = WorldGetLevelAreaSNOFromRActor(LocalPlayerRActorGet());
    BOOL bInDungeonFinderLevelArea = LevelAreaGetDungeonFinderLevel(snoLevelArea) != 0;
    const int32 DUNGEONFINDER_QUEST_ENTER_LOOTRUN_STEP_UID = 7;

    if (GameIsWeeklyChallenge() ||

        (ptDungeonFinderQuestInstance && bInDungeonFinderLevelArea &&
         ptDungeonFinderQuestInstance->nCurrentStepUID != QUEST_UNASSIGNED_STEP_UID && ptDungeonFinderQuestInstance->nCurrentStepUID != DUNGEONFINDER_QUEST_ENTER_LOOTRUN_STEP_UID &&


         !QuestIsComplete(*ptDungeonFinderQuestInstance)))
    {




        UIStoryboard & tFlashStoryboard = UI_GetStoryboardFromID(UISTORYBOARD_TIERED_RIFT_FLASH);
        tFlashStoryboard.Stop();
        tFlashStoryboard.Reset();
        tFlashStoryboard.Play(FALSE, 0.0f);

        sPlayTieredRiftOrbPickUpHUDFX(sIsInTieredRift() ? UIPROGRESS_TIERED_RIFT_PROGRESSBAR : UICONTROL_DUNGEON_FINDER_PROGRESSBAR);
    } } }

#include "../../../Common/GameTemplate.h"
// Raw UIQuests.cpp:1052 only; no recovered header declaration.
namespace UIQuests { void MakeCurrentObjectivesReappear(); }

#line 6613 "X/Client/UserInterface/Interface/UIGame.cpp"
namespace UIGame { void Area_Title_Update(
    const UIID &)
{




    UIWindow & tDlg = UI_GetWindowFromID(UIDLG_LEVELAREA);

    BOOL fHide = FALSE;
    if (UI_GetWindowFromID(UIDLG_QUESTREWARD).IsActive())
    {
        fHide = TRUE;
    }

    else
    {

        ActTitleParams & tTitleParams = sgptUIGameGlobals->tTitleParams;
        uint32 dwTicksElapsed = CGameGetRealTicks() - tTitleParams.dwTimestamp;

        float flFrac;
        FadeState eFadeState = scgtActTitleFadeRamp.Evaluate(dwTicksElapsed, flFrac);
        if (eFadeState != FADESTATE_HIDDEN)
        {
            fHide = TRUE;
        }
    }


    if (GameTypeIsPvP())
    {
        fHide = TRUE;
    }

    if (fHide)
    {
        tDlg.SetActive(FALSE);
        return;
    }


    RActorID idPlayerRActor = LocalPlayerRActorGet();
    SNO snoLevelArea = WorldGetLevelAreaSNOFromRActor(idPlayerRActor);

    if (GameIsOpenWorld() && snoLevelArea != SNO_NONE)
    {


        ScopedLevelAreaDefinitionLockR pdefLevelArea(snoLevelArea);
        if (pdefLevelArea->snoOpenWorldDisplayLevelArea != SNO_NONE)
        {
            snoLevelArea = pdefLevelArea->snoOpenWorldDisplayLevelArea;
        }
    }


    AreaTitleParams & tAreaTextInfo = sgptUIGameGlobals->tAreaTextInfo;
    if (snoLevelArea != tAreaTextInfo.snoLevelArea)
    {
        tAreaTextInfo.snoLevelArea = snoLevelArea;


        tAreaTextInfo.eState = FADESTATE_HIDING;
    }

    if (tAreaTextInfo.eState == FADESTATE_HIDDEN)
    {
        tDlg.SetActive(FALSE);
        return;
    }

    tDlg.SetActive(TRUE);


    switch (tAreaTextInfo.eState)
    { case FADESTATE_SHOWING: {
        const int32 nMaxAlpha = 255;

        tAreaTextInfo.bAlpha = Math::Min<int32>(tAreaTextInfo.bAlpha + 4, nMaxAlpha);
        if (tAreaTextInfo.bAlpha == nMaxAlpha)
        {
            tAreaTextInfo.eState = FADESTATE_SHOWN;
            tAreaTextInfo.dwStateTimestamp = CGameGetRealTicks();
        }
        break; }

    case FADESTATE_SHOWN: {

        uint32 dwCurrentTime = CGameGetRealTicks();
        if (dwCurrentTime - tAreaTextInfo.dwStateTimestamp >= 2 * TICKS_PER_SECOND)
        {
            tAreaTextInfo.eState = FADESTATE_HIDING; tAreaTextInfo.dwStateTimestamp = dwCurrentTime;
        }
        break; }


    case FADESTATE_HIDING:

        tAreaTextInfo.bAlpha = Math::Max<int32>(tAreaTextInfo.bAlpha - 2, 0);
        if (tAreaTextInfo.bAlpha == 0)
        {
            tAreaTextInfo.eState = FADESTATE_HIDDEN;
            tAreaTextInfo.dwStateTimestamp = CGameGetRealTicks();

            BOOL fShowLevelAreaText = FALSE;
            if (tAreaTextInfo.snoLevelArea != SNO_NONE && tAreaTextInfo.snoLevelArea != tAreaTextInfo.snoLevelAreaDisplayed)
            {

                if (tAreaTextInfo.snoLevelAreaDisplayed == SNO_NONE)
                {
                    fShowLevelAreaText = TRUE;
                }
                else
                {
                    CRefString sNameLabelNew = LevelAreaGetLocalizedName(tAreaTextInfo.snoLevelArea);
                    CRefString sNameLabelPrev = LevelAreaGetLocalizedName(tAreaTextInfo.snoLevelAreaDisplayed);

                    if (sNameLabelNew != sNameLabelPrev)
                    {
                        fShowLevelAreaText = TRUE;
                    }
                }
            }
            if (fShowLevelAreaText)
            {
                CRefString sName = LevelAreaGetLocalizedName(tAreaTextInfo.snoLevelArea);

                int32 nLevel = LevelAreaGetDungeonFinderLevel(tAreaTextInfo.snoLevelArea);
                if (nLevel > 0)
                {
                    Player * ptPlayer = LocalPlayerGet();
                    if (ptPlayer)
                    {
                        ActorCommonData * ptACDLocalPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
                        if (ptACDLocalPlayer && ACDIsInTieredLootRun(*ptACDLocalPlayer))
                        {
                            CRefString sVal = UnicodeText::FormatNumber(nLevel, TRUE, FNS_NORMAL, XLOCALE_INVALID);
                            sName = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_LEVEL_AREAS), "TieredRift").str(), sVal.str(), NULL);
                        }
                    }
                }

                UI_GetTextFromID(UITEXT_LEVELAREA).SetText(sName, FALSE);
                tAreaTextInfo.eState = FADESTATE_SHOWING;

                UIQuests::MakeCurrentObjectivesReappear();
            }

            tAreaTextInfo.snoLevelAreaDisplayed = tAreaTextInfo.snoLevelArea;
        }
        break;
    }
    tDlg.SetTransparency(tAreaTextInfo.bAlpha);
} }

#line 8015 "X/Client/UserInterface/Interface/UIGame.cpp"
void UIFloatingMessage::Update()
{

    UIWindow & tWnd = UI_GetWindowFromID(UIDLG_NOTIFY_MESSAGE);
    if (tWnd.IsActive() && sgptUIGameGlobals->dwFloatingMessageDuration)
    {

        const uint32 FADE_TIME = 30;

        uint32 dwTime = CGameGetRealTicks();
        uint32 dwFadeStartTime = sgptUIGameGlobals->dwFloatingMessageStartTime + sgptUIGameGlobals->dwFloatingMessageDuration;
        if (dwTime > dwFadeStartTime + FADE_TIME)
        {

            tWnd.SetTransparency(255);
            tWnd.SetActive(FALSE);
        }
        else if (dwTime > dwFadeStartTime)
        {
            float flAlpha = 1.0f - float(dwTime - dwFadeStartTime) / float(FADE_TIME);
            uint8 bAlpha = (uint8)Math::FloatToInt(flAlpha * 255.0f);
            tWnd.SetTransparency(bAlpha);
        }
    }
}
#line 8432 "X/Client/UserInterface/Interface/UIGame.cpp"
